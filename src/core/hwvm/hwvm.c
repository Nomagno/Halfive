/*
Copyright Nomagno 2021, 2022

Redistributions, modified or unmodified, in whole or in part, must retain
applicable notices of copyright or other legal privilege, these conditions, and
the following license terms and disclaimer.  Subject to these conditions, each
holder of copyright or other legal privileges, author or assembler, and
contributor of this work, henceforth "licensor", hereby grants to any person
who obtains a copy of this work in any form:

1. Permission to reproduce, modify, distribute, publish, sell, sublicense, use,
and/or otherwise deal in the licensed material without restriction.

2. A perpetual, worldwide, non-exclusive, royalty-free, gratis, irrevocable
patent license to make, have made, provide, transfer, import, use, and/or
otherwise deal in the licensed material without restriction, for any and all
patents held by such licensor and necessarily infringed by the form of the work
upon distribution of that licensor's contribution to the work under the terms
of this license.

NO WARRANTY OF ANY KIND IS IMPLIED BY, OR SHOULD BE INFERRED FROM, THIS LICENSE
OR THE ACT OF DISTRIBUTION UNDER THE TERMS OF THIS LICENSE, INCLUDING BUT NOT
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS, ASSEMBLERS, OR HOLDERS OF
COPYRIGHT OR OTHER LEGAL PRIVILEGE BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER
LIABILITY, WHETHER IN ACTION OF CONTRACT, TORT, OR OTHERWISE ARISING FROM, OUT
OF, OR IN CONNECTION WITH THE WORK OR THE USE OF OR OTHER DEALINGS IN THE
WORK.*/

#include <halfworld/hwreq.h>
#include <halfworld/hwvm/hwvm.h>

enum optype { adr = 0, lit = 1, ptr = 2 };

#define ISADR(num, pos) (((num | (1 << (2 - pos))) != num))
#define ISLIT(num, pos)                                                        \
	(((num | (1 << (2 - pos))) == num) && ((num | 8) != num))
#define ISPTR(num, pos)                                                        \
	(((num | (1 << (2 - pos))) == num) && ((num | 8) == num))


#define _PROG_CO program->co

#define CURR_OP program->code.opnd[_PROG_CO]
#define MASK program->mask
#define DATA program->data
#define _BREAK                                                                 \
	rwinf->was_err = !!return_code;                                                    \
	return return_code

#define GETPTR(arr, where) ((arr[where] << 8) | (arr[where + 1]))

#define PTR_VALID(arr, where)                                                  \
	((arr[where] != NULL && arr[where + 1] != NULL) &&                     \
	 (arr[GETPTR(*arr, where)] != NULL))

#define GETVAR(dest, arr, pos, rvar)                                           \
	if (ISLIT(arr[2], pos)) {                                              \
		dest = arr[pos - 1];                                           \
	} else if (ISADR(arr[2], pos) && DATA[arr[pos - 1]] != NULL) {         \
		dest = *DATA[arr[pos - 1]];                                    \
		if (rvar) {                                                    \
			rwinf->adrr = arr[pos - 1];                            \
			rwinf->read_adrr = 1;                                  \
		} else {                                                       \
			rwinf->adrw = arr[pos - 1];                            \
			rwinf->read_adrw = 1;                                  \
		}                                                              \
	} else if (ISPTR(arr[2], pos) && PTR_VALID(DATA, arr[pos - 1])) {      \
		dest = *DATA[GETPTR(*DATA, arr[pos - 1])];                     \
		if (rvar) {                                                    \
			rwinf->adrr = GETPTR(*DATA, arr[pos - 1]);             \
			rwinf->read_adrr = 1;                                  \
		} else {                                                       \
			rwinf->adrw = GETPTR(*DATA, arr[pos - 1]);             \
			rwinf->read_adrw = 1;                                  \
		}                                                              \
	}

#define GETTYPE(arr, pos)                                                      \
	((ISPTR(arr[2], pos)) ? ptr : ((ISADR(arr[2], pos)) ? adr : lit))

#define _ZF 0xFFFF
#define _CF 0xFFFE
#define _IN 0xFFFD
#define _OU 0xFFFC
#define _PC_HIGH 0xFFFB
#define _PC_LOW 0xFFFA
#define _ERROR_ADDR 0xFFF0

#define _MEMMAX 0x3FFF
#define _DRIVMAX 0xBFFF

/*Unknown address*/

HWVM_GeneralMemory HWVM_Init(HWVM_CodeMemory *code,
			     HWVM_DefaultMemSetup *rawmem);
unsigned HWVM_Execute(HWVM_GeneralMemory *program, HWVM_ReadWriteInfo *rwinf);

/*Return value meaning:
0 - successful
1 - error - unmmapped memory
2 - error - write to read-only address
3 - error - wrong usage*/

HWVM_GeneralMemory HWVM_Init(HWVM_CodeMemory *code,
			     HWVM_DefaultMemSetup *rawmem)
{
	HWVM_GeneralMemory returnval = {(HWVM_InstructionSet)0};
	returnval.code = *code;
	rawmem->co_high = (IS_LITTLE_ENDIAN) ? ((hwuchar *)&returnval.co)
					     : ((hwuchar *)&returnval.co + 1);
	rawmem->co_low = (IS_LITTLE_ENDIAN) ? ((hwuchar *)&returnval.co + 1)
					    : ((hwuchar *)&returnval.co);

	hwuint ival = 0;
	hwuint i = 0;
	while (1) {
		if ((ival != !!i) && (ival != 0))
			break;
		ival = !!i;
		if (i <= _MEMMAX) {
			returnval.data[i] = &(rawmem->gmem[i]);
		} else if (i <= _DRIVMAX) {
			returnval.data[i] = &(rawmem->driv[i - 0x4000]);
			returnval.mask[i] = 1; /*Read-only*/
		} else if (i == _ZF) {
			returnval.data[i] = &(rawmem->zf);
		} else if (i == _CF) {
			returnval.data[i] = &(rawmem->cf);
		} else if (i == _IN) {
			returnval.data[i] = &(rawmem->in);
			returnval.mask[i] = 1; /*Read-only*/
		} else if (i == _OU) {
			returnval.data[i] = &(rawmem->ou);
		} else if (i == _PC_HIGH) {
			returnval.data[i] = rawmem->co_high;
			returnval.mask[i] = 1; /*Read-only*/
		} else if (i == _PC_LOW) {
			returnval.data[i] = rawmem->co_low;
			returnval.mask[i] = 1; /*Read-only*/
		}
		i += 1;
	}

	return returnval;
}

unsigned HWVM_Execute(HWVM_GeneralMemory *program, HWVM_ReadWriteInfo *rwinf)
{
	*rwinf = (HWVM_ReadWriteInfo){0};

	unsigned return_code = 0;

	hwuchar getnum_orig = 0;
	hwuint setnum_dest = 0;
	enum optype dest_type = (enum optype)0;

	_Bool donot_save = 0; /*0 - set main address; 1 - only set zf/cf*/
	hwuchar set_cf = 0; /*0 - don't touch; 1 - set to 1; 2 - set to zero;*/
	hwuchar set_zf = 0; /*0 - don't touch; 1 - set to 1; 2 - set to zero;*/
	hwuint tmpchar1 = 0;
	hwuint tmpchar2 = 0;

	hwuint subval = 0; /*For 'func' instruction. This can't be a safe 'ret' place, so it's an error code*/

	switch (program->code.inst[_PROG_CO]) {
	case Inst_halt:
		program->hf = 1;
		break;
	case Inst_nop:
		_PROG_CO += 1;
		break;
	case Inst_jmp:
		goto _jmp;
	case Inst_skpz:
		if(*(DATA[_ZF]) == 0)
			_PROG_CO += CURR_OP[0];
		_PROG_CO += 1;
		break;
	case Inst_skmz:
		if(*(DATA[_ZF]) == 0){
			_PROG_CO -= CURR_OP[0];
			_PROG_CO -= 1;
		} else _PROG_CO += 1;
		break;
	case Inst_set:
		GETVAR(getnum_orig, CURR_OP, 2, 1);
		setnum_dest = CURR_OP[0];
		dest_type = GETTYPE(CURR_OP, 1);
		goto _set;
		break;
	case Inst_add:
		GETVAR(tmpchar1, CURR_OP, 1, 0);
		GETVAR(tmpchar2, CURR_OP, 2, 1);
		setnum_dest = CURR_OP[0];
		dest_type = GETTYPE(CURR_OP, 1);
		getnum_orig = tmpchar1 + tmpchar2;
		set_cf = ((getnum_orig < tmpchar1) || (getnum_orig < tmpchar2))
			     ? 1
			     : 2;
		set_zf = (getnum_orig == 0) ? 2 : 1;
		goto _set;
		break;
	case Inst_sub:
		GETVAR(tmpchar1, CURR_OP, 1, 0);
		GETVAR(tmpchar2, CURR_OP, 2, 1);
		setnum_dest = CURR_OP[0];
		dest_type = GETTYPE(CURR_OP, 1);
		getnum_orig = tmpchar1 - tmpchar2;
		set_cf = (getnum_orig < tmpchar2) ? 1 : 2;
		set_zf = (getnum_orig == 0) ? 2 : 1;
		goto _set;
		break;
	case Inst_cmp:
		GETVAR(tmpchar1, CURR_OP, 1, 0);
		GETVAR(tmpchar2, CURR_OP, 2, 1);
		getnum_orig = tmpchar1 - tmpchar2;
		set_cf = (getnum_orig > tmpchar1) ? 1 : 2;
		set_zf = (getnum_orig == 0) ? 2 : 1;
		donot_save = 1;
		goto _set;
		break;
	case Inst_and:
		GETVAR(tmpchar1, CURR_OP, 1, 0);
		GETVAR(tmpchar2, CURR_OP, 2, 1);
		setnum_dest = CURR_OP[0];
		dest_type = GETTYPE(CURR_OP, 1);
		getnum_orig = tmpchar1 & tmpchar2;
		set_zf = (getnum_orig == 0) ? 2 : 1;
		goto _set;
		break;
	case Inst_xor:
		GETVAR(tmpchar1, CURR_OP, 1, 0);
		GETVAR(tmpchar2, CURR_OP, 2, 1);
		setnum_dest = CURR_OP[0];
		dest_type = GETTYPE(CURR_OP, 1);
		getnum_orig = tmpchar1 ^ tmpchar2;
		set_zf = (getnum_orig == 0) ? 2 : 1;
		goto _set;
		break;
	case Inst_or:
		GETVAR(tmpchar1, CURR_OP, 1, 0);
		GETVAR(tmpchar2, CURR_OP, 2, 1);
		setnum_dest = CURR_OP[0];
		dest_type = GETTYPE(CURR_OP, 1);
		getnum_orig = tmpchar1 | tmpchar2;
		set_zf = (getnum_orig == 0) ? 2 : 1;
		goto _set;
		break;
	case Inst_shift:
		GETVAR(tmpchar1, CURR_OP, 1, 0);
		GETVAR(tmpchar2, CURR_OP, 2, 1);
		setnum_dest = CURR_OP[0];
		dest_type = GETTYPE(CURR_OP, 1);
		getnum_orig = (tmpchar2 < 8) ? (tmpchar1 << tmpchar2)
					     : (tmpchar1 >> (tmpchar2 - 8));
		set_zf = (getnum_orig == 0) ? 2 : 1;
		goto _set;
		break;

	case Inst_func:;
		program->func_co[CURR_OP[0]] =
		    _PROG_CO; /*Write down this instruction's position*/
		for (hwuint i = 0;
		     i < (sizeof(program->code.inst) - (_PROG_CO + 1));
		     i++) { /*Look for closest 'ret' */
			if (program->code.inst[_PROG_CO + 1 + i] == Inst_ret) {
				subval = _PROG_CO + 1 + i;
				break;
			}
		}
		if (subval) {
			program->skip_co[CURR_OP[0]] = subval;
			_PROG_CO = subval + 1;
			_BREAK;
		} /*Write down ret position, skip subroutine*/
		else {
			return_code = 3;
			_BREAK;
		} /*No matching 'ret'!*/
		break;
	case Inst_ret:
		_PROG_CO = program->return_co[CURR_OP[0]]; /*Return to caller*/
		_BREAK;
		break;
	case Inst_call:
		program->return_co[CURR_OP[0]] =
		    _PROG_CO + 1; /*Note next instruction*/
		_PROG_CO =
		    program->func_co[CURR_OP[0]] + 1; /*Jump to subroutine*/
		_BREAK;
		break;
	}
	_BREAK;

_set:
	if (set_cf == 1)
		*DATA[_CF] = 1;
	else if (set_cf == 2)
		*DATA[_CF] = 0;

	if (set_zf == 1)
		*DATA[_ZF] = 1;
	else if (set_zf == 2)
		*DATA[_ZF] = 0;

	if (!donot_save) { /*We don't care about any memory
			     stuff unless we need to use the memory!*/
		if (DATA[setnum_dest] == NULL)
			return 1; /*Unmapped memory*/
		if (dest_type == lit) {
			return_code = 3; /*Wrong usage*/
			_BREAK;
		} else if (dest_type == adr) {
			if (DATA[setnum_dest] == NULL) {
				return_code = 1; /*Unmapped memory*/
				_BREAK;
			} else if (MASK[setnum_dest] == 1) {
				return_code = 2; /*Write to read-only address*/
				_BREAK;
			};
			*DATA[setnum_dest] = getnum_orig;
			rwinf->adrw = setnum_dest;
			rwinf->wrote_adrw = 1;
		} else if (dest_type == ptr) {
			if (PTR_VALID(DATA, setnum_dest)) {
				*DATA[GETPTR(*DATA, setnum_dest)] = getnum_orig;
				rwinf->adrw = GETPTR(*DATA, setnum_dest);
				rwinf->wrote_adrw = 1;
			} else {
				return_code = 1;
				_BREAK;
			};
		}
	}
	_PROG_CO += 1;
	_BREAK;

_jmp:
	if (ISADR(CURR_OP[2], 1) || ISLIT(CURR_OP[2], 1)) {
		/*If dealing with an address OR a literal, just
			 move the PC to the operand*/
		_PROG_CO = CURR_OP[0];
	} else if (ISPTR(CURR_OP[2], 1)) {
		/*If we are dealing with a pointer*/
		if (PTR_VALID(
			DATA,
			CURR_OP[0])) { /*If the pointer addresses are mapped*/
			/*Read the address presented and set the PC*/
			_PROG_CO = GETPTR(*DATA, CURR_OP[0]);
		} else {
			return_code = 1; /*Else report they are unmmapped*/
		}
	}
	_BREAK;
}
