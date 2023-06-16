/*
Copyright Nomagno 2021, 2022, 2023

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice (including the next
paragraph) shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
*/

#include <halfive/h5req.h>
#include <halfive/h5vm/h5vm_gear.h>

enum optype { adr = 0, lit = 1, ptr = 2 };

/*
typeinfo (type nibble of the instruction, second half of the LSB):
P 0 X Y
| | | ---- If this bit is set (1 << 0), the second operand is a literal
| | ------ If this bit is set (1 << 1), the first operand  is a literal
| -------- Always zero
---------- If this bit is set, any literals are instead treated as pointers
*/
#define ISADR(typeinfo, operand_position) \
	(((typeinfo | (1 << (2 - operand_position))) != typeinfo))
#define ISLIT(typeinfo, operand_position)                        \
	(((typeinfo | (1 << (2 - operand_position))) == typeinfo) && \
		((typeinfo | 8) != typeinfo))

#define ISPTR(typeinfo, operand_position)                        \
	(((typeinfo | (1 << (2 - operand_position))) == typeinfo) && \
		((typeinfo | 8) == typeinfo))

#define _PROG_CO program->co

#define CURR_OP program->code.opnd[_PROG_CO]
#define MASK	program->mask
#define DATA	program->data
#define _RETURN                     \
	rwinf->was_err = !!return_code; \
	return return_code;

#define FIRST_OPERAND  1
#define SECOND_OPERAND 2

/*
pointer structure:
[0xYYYY]      [0xYYYY + 1]
bbbbbbbb      bbbbbbbb
^<<<<<<<      ^<<<<<<<
|             |
|             -------------- Least Significant Byte of the address pointed to
|--------------------------- Most Significant Byte  of the address pointed to
*/
#define GETPTR(memory, address) (memory[address])

/*Check no null pointers will be dereferenced*/
#define PTR_VALID(memory, address)                               \
	((memory[address] != NULL) && \
		(memory[GETPTR(*memory, address)] != NULL))

#define GETVAR(out, operands, operand_position, rvar)                        \
	{                                                                        \
		if (ISLIT(operands[2], operand_position)) {                          \
			out = operands[operand_position - 1];                            \
		} else if (ISADR(operands[2], operand_position) &&                   \
				   DATA[operands[operand_position - 1]] != NULL) {           \
			out = *DATA[operands[operand_position - 1]];                     \
			if (rvar) {                                                      \
				rwinf->adrr		 = operands[operand_position - 1];           \
				rwinf->read_adrr = 1;                                        \
			} else {                                                         \
				rwinf->adrw		 = operands[operand_position - 1];           \
				rwinf->read_adrw = 1;                                        \
			}                                                                \
		} else if (ISPTR(operands[2], operand_position) &&                   \
				   PTR_VALID(DATA, operands[operand_position - 1])) {        \
			out = *DATA[GETPTR(*DATA, operands[operand_position - 1])];      \
			if (rvar) {                                                      \
				rwinf->adrr = GETPTR(*DATA, operands[operand_position - 1]); \
				rwinf->read_adrr = 1;                                        \
			} else {                                                         \
				rwinf->adrw = GETPTR(*DATA, operands[operand_position - 1]); \
				rwinf->read_adrw = 1;                                        \
			}                                                                \
		}                                                                    \
	}

#define GETTYPE(operands, operand_position) \
	((ISPTR(operands[2], operand_position)) \
			? ptr                           \
			: ((ISADR(operands[2], operand_position)) ? adr : lit))

/*DEFAULTS IN COMMENTS*/
/*MEMUNIT: 4096*/
/*MEMSIZE: 16*/
#define _ZF	        (MEMUNIT * MEMSIZE - 1)	 /*0xFFFF*/
#define _CF	        (MEMUNIT * MEMSIZE - 2)	 /*0xFFFE*/
#define _IN	        (MEMUNIT * MEMSIZE - 3)	 /*0xFFFD*/
#define _OU	        (MEMUNIT * MEMSIZE - 4)	 /*0xFFFC*/
#define _PC_ADR     (MEMUNIT * MEMSIZE - 5)	 /*0xFFFB*/
#define _FF	        (MEMUNIT * MEMSIZE - 6)	 /*0xFFFA*/
#define _ERROR_ADDR (MEMUNIT * MEMSIZE - 15) /*0xFFF0*/

/*GUARANTEED TO BE CONTIGUOUS*/
#define _RWMMAX (MEMUNIT*RWMSIZE - 1)		/*0x6FFF*/
#define _ROMMAX (MEMUNIT*ROMSIZE + _RWMMAX) /*0x8FFF*/
#define _GROMMAX (MEMUNIT*GROMSIZE + _ROMMAX) /*0xCFFF*/
#define _GOUTMAX (MEMUNIT*GOUTSIZE + _GROMMAX) /*0xDFFF*/
#define _AROMMAX (MEMUNIT*AROMSIZE + _GOUTMAX) /*0xDFFF*/
#define _AOUTMAX (MEMUNIT*AOUTSIZE + _AROMMAX) /*0xDFFF*/
#define _STACKMAX (MEMUNIT*STACKSIZE + _AOUTMAX) /*0xEFFF*/

/*Unknown address*/

H5VM_GeneralMemory H5VM_init(
	H5VM_CodeMemory *code, H5VM_DefaultMemSetup *rawmem);
unsigned H5VM_execute(H5VM_GeneralMemory *program, H5VM_ReadWriteInfo *rwinf);

H5VM_GeneralMemory H5VM_init(
	H5VM_CodeMemory *code, H5VM_DefaultMemSetup *rawmem)
{
	H5VM_GeneralMemory returnval = {(H5VM_InstructionSet)0};
	returnval.code = *code;
	rawmem->prog_co_adr = &returnval.co;

	for (h5ulong i = 0; i < (MEMUNIT * MEMSIZE); i++) {
		if        (i <= _RWMMAX) {
			returnval.data[i] = &(rawmem->rwm[i]);
		} else if (i <= _ROMMAX) {
			returnval.data[i] = &(rawmem->rom[i - _RWMMAX - 1]);
			returnval.mask[i] = 1; /*Read-only*/
		} else if (i <= _GROMMAX) {
			returnval.data[i] = &(rawmem->grom[i - _ROMMAX - 1]);
			returnval.mask[i] = 1; /*Read-only*/
		} else if (i <= _GOUTMAX) {
			returnval.data[i] = &(rawmem->graphical_output[i - _GROMMAX - 1]);
		} else if (i <= _AROMMAX) {
			returnval.data[i] = &(rawmem->arom[i - _GOUTMAX - 1]);
		} else if (i <= _AOUTMAX) {
			returnval.data[i] = &(rawmem->audio_output[i - _AROMMAX - 1]);
		} else if (i <= _STACKMAX) {
			returnval.data[i] = &(rawmem->stack[i - _AOUTMAX - 1]);
		} else if (i == _FF) {
			returnval.data[i] = &(rawmem->ff);
		} else if (i == _ZF) {
			returnval.data[i] = &(rawmem->zf);
		} else if (i == _CF) {
			returnval.data[i] = &(rawmem->cf);
		} else if (i == _IN) {
			returnval.data[i] = &(rawmem->in);
			returnval.mask[i] = 1; /*Read-only*/
		} else if (i == _OU) {
			returnval.data[i] = &(rawmem->ou);
		} else if (i == _PC_ADR) {
			returnval.data[i] = rawmem->prog_co_adr;
			returnval.mask[i] = 1; /*Read-only*/
		}
	}
	return returnval;
}

/*Return value meaning:
0 - Successful
1 - ERROR - Unmmapped memory
2 - ERROR - Write to read-only address
3 - ERROR - Wrong usage/malformed instruction
4 - ERROR - Unmapped memory - CALLSTACK OVERFLOW*/
unsigned H5VM_execute(H5VM_GeneralMemory *program, H5VM_ReadWriteInfo *rwinf)
{
	*rwinf = (H5VM_ReadWriteInfo){0};

	unsigned return_code = 0;

	h5uint getnum_orig    = 0;
	h5uint setnum_dest    = 0;
	enum optype dest_type = (enum optype)0;

	_Bool donot_save = 0; /*0 - set main address; 1 - only set zf/cf*/
	h5uint set_cf   = 0; /*0 - don't touch; 1 - set to 1; 2 - set to zero;*/
	h5uint set_zf   = 0; /*0 - don't touch; 1 - set to 1; 2 - set to zero;*/
	h5uint tmpint1   = 0;
	h5uint tmpint2   = 0;

	switch (program->code.inst[_PROG_CO]) {
	case Inst_halt:
		program->hf = 1;
		break;
	case Inst_jmp:
		goto _jmp;
	case Inst_skpz:
		if (*(DATA[_ZF]) == 0) {
			_PROG_CO += CURR_OP[0];
		}
		_PROG_CO += 1;
		break;
	case Inst_skmz:
		if (*(DATA[_ZF]) == 0) {
			_PROG_CO -= CURR_OP[0];
			_PROG_CO -= 1;
		} else
			_PROG_CO += 1;
		break;
	case Inst_set:
		if ((CURR_OP[0] == CURR_OP[1]) && (GETTYPE(CURR_OP, 1) == GETTYPE(CURR_OP, 2))) {
			_PROG_CO += 1;
			_RETURN;
			break; /*This is obviously a soft-nop*/
		}
		else {
			GETVAR(getnum_orig, CURR_OP, SECOND_OPERAND, 1);
			setnum_dest = CURR_OP[0];
			dest_type	= GETTYPE(CURR_OP, 1);
			goto _set;
		}
		break;
	case Inst_add:
		GETVAR(tmpint1, CURR_OP, FIRST_OPERAND, 0);
		GETVAR(tmpint2, CURR_OP, SECOND_OPERAND, 1);
		setnum_dest = CURR_OP[0];
		dest_type	= GETTYPE(CURR_OP, 1);
		getnum_orig = tmpint1 + tmpint2;
		set_cf = ((getnum_orig < tmpint1) || (getnum_orig < tmpint2)) ? 1 : 2;
		set_zf = (getnum_orig == 0) ? 2 : 1;
		goto _set;
		break;
	case Inst_sub:
		GETVAR(tmpint1, CURR_OP, FIRST_OPERAND, 0);
		GETVAR(tmpint2, CURR_OP, SECOND_OPERAND, 1);
		setnum_dest = CURR_OP[0];
		dest_type	= GETTYPE(CURR_OP, 1);
		getnum_orig = tmpint1 - tmpint2;
		set_cf		= (getnum_orig < tmpint2) ? 1 : 2;
		set_zf		= (getnum_orig == 0) ? 2 : 1;
		goto _set;
		break;
	case Inst_cmp:
		GETVAR(tmpint1, CURR_OP, FIRST_OPERAND, 0);
		GETVAR(tmpint2, CURR_OP, SECOND_OPERAND, 1);
		getnum_orig = tmpint1 - tmpint2;
		set_cf		= (getnum_orig > tmpint1) ? 1 : 2;
		set_zf		= (getnum_orig == 0) ? 2 : 1;
		donot_save	= 1;
		goto _set;
		break;
	case Inst_and:
		GETVAR(tmpint1, CURR_OP, FIRST_OPERAND, 0);
		GETVAR(tmpint2, CURR_OP, SECOND_OPERAND, 1);
		setnum_dest = CURR_OP[0];
		dest_type	= GETTYPE(CURR_OP, 1);
		getnum_orig = tmpint1 & tmpint2;
		set_zf		= (getnum_orig == 0) ? 2 : 1;
		goto _set;
		break;
	case Inst_xor:
		GETVAR(tmpint1, CURR_OP, FIRST_OPERAND, 0);
		GETVAR(tmpint2, CURR_OP, SECOND_OPERAND, 1);
		setnum_dest = CURR_OP[0];
		dest_type	= GETTYPE(CURR_OP, 1);
		getnum_orig = tmpint1 ^ tmpint2;
		set_zf		= (getnum_orig == 0) ? 2 : 1;
		goto _set;
		break;
	case Inst_or:
		GETVAR(tmpint1, CURR_OP, FIRST_OPERAND, 0);
		GETVAR(tmpint2, CURR_OP, SECOND_OPERAND, 1);
		setnum_dest = CURR_OP[0];
		dest_type	= GETTYPE(CURR_OP, 1);
		getnum_orig = tmpint1 | tmpint2;
		set_zf		= (getnum_orig == 0) ? 2 : 1;
		goto _set;
		break;
	case Inst_shift:
		GETVAR(tmpint1, CURR_OP, FIRST_OPERAND, 0);
		GETVAR(tmpint2, CURR_OP, SECOND_OPERAND, 1);
		setnum_dest = CURR_OP[0];
		dest_type	= GETTYPE(CURR_OP, 1);
		getnum_orig = (tmpint2 < 0x10) ? (tmpint1 << tmpint2)
									 : (tmpint1 >> (tmpint2 - 0x10));
		set_zf		= (getnum_orig == 0) ? 2 : 1;
		goto _set;
		break;

	case Inst_func:
		program->func_co[CURR_OP[0]] = _PROG_CO; /*Annotate function start*/
		h5uint subval = 0; /*Zero can't be a safe 'ret' place*/
		for (h5uint i = 0; i < (sizeof(program->code.inst) - (_PROG_CO + 1)); i++) { /*Look for closest 'ret' */
			if (program->code.inst[_PROG_CO + 1 + i] == Inst_ret) {
				subval = _PROG_CO + 1 + i;
				break;
			}
		}
		if (subval) {
			_PROG_CO  = subval + 1;
			_RETURN;
		} /*Skip subroutine*/
		else {
			return_code = 3;
			_PROG_CO += 1;
			_RETURN;
		} /*ERROR: No matching 'ret'!*/
		break;
	case Inst_ret: {
		h5uint oldval = *DATA[*DATA[(CURR_OP[1] >> 8)*256]]; /*Return address is stored where the stack pointer points*/
		*DATA[(CURR_OP[1] >> 8)*256] -= (CURR_OP[1] & 0x00FF) + 1; /*Must move stack pointer backwards*/
		_PROG_CO = oldval + 1; /*Go to return address+1*/
		_RETURN;
		break;
	}
	case Inst_call: {
		h5uint oldval = *DATA[(CURR_OP[1] >> 8)*256];
		h5uint newval;
		if (oldval == 0) {
			/*Initialize stack pointer*/
			*DATA[(CURR_OP[1] >> 8)*256] = (CURR_OP[1] >> 8)*256 + 1; /* +1 because first frame*/
			newval = *DATA[(CURR_OP[1] >> 8)*256];
 		} else {
			/*Increase stack pointer */
			*DATA[(CURR_OP[1] >> 8)*256] += (CURR_OP[1] & 0x00FF) + 1;
			newval = *DATA[(CURR_OP[1] >> 8)*256];
		}
		if (DATA[newval] == NULL) {
			return_code = 4; /*Callstack overflow*/
			_RETURN;
		}
		*DATA[newval] = _PROG_CO; /*Annotate return address*/
		_PROG_CO = program->func_co[CURR_OP[0]]+1; /*Go to function start+1*/
		_RETURN;
		break;
	}
	case Inst_frame: {
		h5uint val = *DATA[(CURR_OP[1] >> 8)*256];
		setnum_dest = CURR_OP[0];
		dest_type	= GETTYPE(CURR_OP, 1);
		getnum_orig = val;
		goto _set;
		break;
	}
	}
	_RETURN;

_set:
	if (set_cf == 1) /*Shall we set the carry flag?*/
		*DATA[_CF] = 1;
	else if (set_cf == 2)
		*DATA[_CF] = 0;

	if (set_zf == 1) /*Shall we set the zero flag?*/
		*DATA[_ZF] = 1;
	else if (set_zf == 2)
		*DATA[_ZF] = 0;

	if (!donot_save) { /*We don't care about any memory
			 stuff unless wrote something!*/
		if (DATA[setnum_dest] == NULL)
			return 1; /*ERROR: Trying to read/write unmapped memory!*/
		if (dest_type == lit) {
			return_code = 3; /*ERROR: Wrong usage! Can't set a literal, that's
					nonsensical*/
			_PROG_CO += 1;
			_RETURN;
		} else if (dest_type == adr) {
			if (DATA[setnum_dest] == NULL) {
				return_code =
					1; /*ERROR: Trying to read/write unmapped memory!*/
				_PROG_CO += 1;
				_RETURN;
			} else if (MASK[setnum_dest] == 1) {
				return_code = 2; /*ERROR: Write to read-only address!*/
				_PROG_CO += 1;
				_RETURN;
			};
			*DATA[setnum_dest] = getnum_orig;
			rwinf->adrw		   = setnum_dest;
			rwinf->wrote_adrw  = 1;
		} else if (dest_type == ptr) {
			if (PTR_VALID(DATA, setnum_dest)) {
				*DATA[GETPTR(*DATA, setnum_dest)] = getnum_orig;
				rwinf->adrw						  = GETPTR(*DATA, setnum_dest);
				rwinf->wrote_adrw				  = 1;
			} else {
				return_code = 1; /*ERROR: Trying to read/write unmapped memory!*/
				_PROG_CO += 1;
				_RETURN;
			};
		}
	}
	_PROG_CO += 1;
	_RETURN;

_jmp:
	if (ISADR(CURR_OP[2], 1) || ISLIT(CURR_OP[2], 1)) {
		/*If dealing with an address OR a literal, just
			 move the PC to the operand*/
		_PROG_CO = CURR_OP[0];
	} else if (ISPTR(CURR_OP[2], 1)) {
		/*If we are dealing with a pointer*/
		if (PTR_VALID(
				DATA, CURR_OP[0])) { /*If the pointer addresses are mapped*/
			/*Read the address presented and set the PC*/
			_PROG_CO = GETPTR(*DATA, CURR_OP[0]);
		} else {
			return_code = 1; /*Else report they are unmmapped*/
		}
	}
	_RETURN;
}
