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

#include <halfive/h5req.h>
#include <halfive/h5vm/h5vm.h>

enum optype { adr = 0, lit = 1, ptr = 2 };

/*
typeinfo (type nibble of the instruction, second half of the LSB):
P 0 X Y
| | | ---- If this bit is set (1 << 0), the second operand is a literal
| | ------ If this bit is set (1 << 1), the first operand  is a literal
| -------- Always zero
---------- If this bit is set, any literals are instead treated as pointers
*/
#define ISADR(typeinfo, operand_position)                                      \
    (((typeinfo | (1 << (2 - operand_position))) != typeinfo))
#define ISLIT(typeinfo, operand_position)                                      \
    (((typeinfo | (1 << (2 - operand_position))) == typeinfo) &&               \
     ((typeinfo | 8) != typeinfo))

#define ISPTR(typeinfo, operand_position)                                      \
    (((typeinfo | (1 << (2 - operand_position))) == typeinfo) &&               \
     ((typeinfo | 8) == typeinfo))

#define _PROG_CO program->co

#define CURR_OP program->code.opnd[_PROG_CO]
#define MASK program->mask
#define DATA program->data
#define _RETURN                                                                \
    rwinf->was_err = !!return_code;                                            \
    return return_code;

#define FIRST_OPERAND 1
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
#define GETPTR(memory, address) ((memory[address] << 8) | (memory[address + 1]))

/*Check no null pointers will be dereferenced*/
#define PTR_VALID(memory, address)                                             \
    ((memory[address] != NULL && memory[address + 1] != NULL) &&               \
     (memory[GETPTR(*memory, address)] != NULL))

#define GETVAR(out, operands, operand_position, rvar)                          \
    {                                                                          \
	if (ISLIT(operands[2], operand_position)) {                            \
	    out = operands[operand_position - 1];                              \
	} else if (ISADR(operands[2], operand_position) &&                     \
		   DATA[operands[operand_position - 1]] != NULL) {             \
	    out = *DATA[operands[operand_position - 1]];                       \
	    if (rvar) {                                                        \
		rwinf->adrr = operands[operand_position - 1];                  \
		rwinf->read_adrr = 1;                                          \
	    } else {                                                           \
		rwinf->adrw = operands[operand_position - 1];                  \
		rwinf->read_adrw = 1;                                          \
	    }                                                                  \
	} else if (ISPTR(operands[2], operand_position) &&                     \
		   PTR_VALID(DATA, operands[operand_position - 1])) {          \
	    out = *DATA[GETPTR(*DATA, operands[operand_position - 1])];        \
	    if (rvar) {                                                        \
		rwinf->adrr = GETPTR(*DATA, operands[operand_position - 1]);   \
		rwinf->read_adrr = 1;                                          \
	    } else {                                                           \
		rwinf->adrw = GETPTR(*DATA, operands[operand_position - 1]);   \
		rwinf->read_adrw = 1;                                          \
	    }                                                                  \
	}                                                                      \
    }

#define GETTYPE(operands, operand_position)                                    \
    ((ISPTR(operands[2], operand_position))                                    \
	 ? ptr                                                                 \
	 : ((ISADR(operands[2], operand_position)) ? adr : lit))

/*DEFAULTS IN COMMENTS*/
/*MEMUNIT: 4096*/
/*MEMSIZE: 16*/
#define _ZF (MEMUNIT * MEMSIZE - 1)          /*0xFFFF*/
#define _CF (MEMUNIT * MEMSIZE - 2)          /*0xFFFE*/
#define _IN (MEMUNIT * MEMSIZE - 3)          /*0xFFFD*/
#define _OU (MEMUNIT * MEMSIZE - 4)          /*0xFFFC*/
#define _PC_HIGH (MEMUNIT * MEMSIZE - 5)     /*0xFFFB*/
#define _PC_LOW (MEMUNIT * MEMSIZE - 6)      /*0xFFFA*/
#define _ERROR_ADDR (MEMUNIT * MEMSIZE - 15) /*0xFFF0*/

/*GUARANTEED TO BE CONTIGUOUS*/
#define _MEMMAX (MEMUNIT * GMEMSIZE - 1)        /*0x3FFF*/
#define _DRIVMAX (MEMUNIT * DRIVSIZE + _MEMMAX) /*0xBFFF*/

/*Unknown address*/

H5VM_GeneralMemory H5VM_init(H5VM_CodeMemory *code,
			     H5VM_DefaultMemSetup *rawmem);
unsigned H5VM_execute(H5VM_GeneralMemory *program, H5VM_ReadWriteInfo *rwinf);

H5VM_GeneralMemory H5VM_init(H5VM_CodeMemory *code,
			     H5VM_DefaultMemSetup *rawmem)
{
    H5VM_GeneralMemory returnval = {(H5VM_InstructionSet)0};
    returnval.code = *code;
    rawmem->co_high = (IS_LITTLE_ENDIAN) ? ((h5uchar *)&returnval.co)
					 : ((h5uchar *)&returnval.co + 1);
    rawmem->co_low = (IS_LITTLE_ENDIAN) ? ((h5uchar *)&returnval.co + 1)
					: ((h5uchar *)&returnval.co);

    for (h5ulong i = 0; i < (MEMUNIT * MEMSIZE); i++) {
	if (i <= _MEMMAX) {
	    returnval.data[i] = &(rawmem->gmem[i]);
	} else if (i <= _DRIVMAX) {
	    returnval.data[i] = &(rawmem->driv[i - _DRIVMAX - 1]);
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
    }

    return returnval;
}

/*Return value meaning:
0 - Successful
1 - ERROR - Unmmapped memory
2 - ERROR - Write to read-only address
3 - ERROR - Wrong usage/malformed instruction*/
unsigned H5VM_execute(H5VM_GeneralMemory *program, H5VM_ReadWriteInfo *rwinf)
{
    *rwinf = (H5VM_ReadWriteInfo){0};

    unsigned return_code = 0;

    h5uchar getnum_orig = 0;
    h5uint setnum_dest = 0;
    enum optype dest_type = (enum optype)0;

    _Bool donot_save = 0; /*0 - set main address; 1 - only set zf/cf*/
    h5uchar set_cf = 0;   /*0 - don't touch; 1 - set to 1; 2 - set to zero;*/
    h5uchar set_zf = 0;   /*0 - don't touch; 1 - set to 1; 2 - set to zero;*/
    h5uint tmpchar1 = 0;
    h5uint tmpchar2 = 0;

    h5uint subval = 0; /*For 'func' instruction. This can't be a safe 'ret'
			  place, so it's an error code*/

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
	if (*(DATA[_ZF]) == 0)
	    _PROG_CO += CURR_OP[0];
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
	GETVAR(getnum_orig, CURR_OP, SECOND_OPERAND, 1);
	setnum_dest = CURR_OP[0];
	dest_type = GETTYPE(CURR_OP, 1);
	goto _set;
	break;
    case Inst_add:
	GETVAR(tmpchar1, CURR_OP, FIRST_OPERAND, 0);
	GETVAR(tmpchar2, CURR_OP, SECOND_OPERAND, 1);
	setnum_dest = CURR_OP[0];
	dest_type = GETTYPE(CURR_OP, 1);
	getnum_orig = tmpchar1 + tmpchar2;
	set_cf = ((getnum_orig < tmpchar1) || (getnum_orig < tmpchar2)) ? 1 : 2;
	set_zf = (getnum_orig == 0) ? 2 : 1;
	goto _set;
	break;
    case Inst_sub:
	GETVAR(tmpchar1, CURR_OP, FIRST_OPERAND, 0);
	GETVAR(tmpchar2, CURR_OP, SECOND_OPERAND, 1);
	setnum_dest = CURR_OP[0];
	dest_type = GETTYPE(CURR_OP, 1);
	getnum_orig = tmpchar1 - tmpchar2;
	set_cf = (getnum_orig < tmpchar2) ? 1 : 2;
	set_zf = (getnum_orig == 0) ? 2 : 1;
	goto _set;
	break;
    case Inst_cmp:
	GETVAR(tmpchar1, CURR_OP, FIRST_OPERAND, 0);
	GETVAR(tmpchar2, CURR_OP, SECOND_OPERAND, 1);
	getnum_orig = tmpchar1 - tmpchar2;
	set_cf = (getnum_orig > tmpchar1) ? 1 : 2;
	set_zf = (getnum_orig == 0) ? 2 : 1;
	donot_save = 1;
	goto _set;
	break;
    case Inst_and:
	GETVAR(tmpchar1, CURR_OP, FIRST_OPERAND, 0);
	GETVAR(tmpchar2, CURR_OP, SECOND_OPERAND, 1);
	setnum_dest = CURR_OP[0];
	dest_type = GETTYPE(CURR_OP, 1);
	getnum_orig = tmpchar1 & tmpchar2;
	set_zf = (getnum_orig == 0) ? 2 : 1;
	goto _set;
	break;
    case Inst_xor:
	GETVAR(tmpchar1, CURR_OP, FIRST_OPERAND, 0);
	GETVAR(tmpchar2, CURR_OP, SECOND_OPERAND, 1);
	setnum_dest = CURR_OP[0];
	dest_type = GETTYPE(CURR_OP, 1);
	getnum_orig = tmpchar1 ^ tmpchar2;
	set_zf = (getnum_orig == 0) ? 2 : 1;
	goto _set;
	break;
    case Inst_or:
	GETVAR(tmpchar1, CURR_OP, FIRST_OPERAND, 0);
	GETVAR(tmpchar2, CURR_OP, SECOND_OPERAND, 1);
	setnum_dest = CURR_OP[0];
	dest_type = GETTYPE(CURR_OP, 1);
	getnum_orig = tmpchar1 | tmpchar2;
	set_zf = (getnum_orig == 0) ? 2 : 1;
	goto _set;
	break;
    case Inst_shift:
	GETVAR(tmpchar1, CURR_OP, FIRST_OPERAND, 0);
	GETVAR(tmpchar2, CURR_OP, SECOND_OPERAND, 1);
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
	for (h5uint i = 0; i < (sizeof(program->code.inst) - (_PROG_CO + 1));
	     i++) { /*Look for closest 'ret' */
	    if (program->code.inst[_PROG_CO + 1 + i] == Inst_ret) {
		subval = _PROG_CO + 1 + i;
		break;
	    }
	}
	if (subval) {
	    program->skip_co[CURR_OP[0]] = subval;
	    _PROG_CO = subval + 1;
	    _RETURN;
	} /*Write down ret position, skip subroutine*/
	else {
	    return_code = 3;
	    _RETURN;
	} /*No matching 'ret'!*/
	break;
    case Inst_ret:
	_PROG_CO = program->return_co[CURR_OP[0]]; /*Return to caller*/
	_RETURN;
	break;
    case Inst_call:
	program->return_co[CURR_OP[0]] = _PROG_CO + 1; /*Note next instruction*/
	_PROG_CO = program->func_co[CURR_OP[0]] + 1;   /*Jump to subroutine*/
	_RETURN;
	break;
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
	    _RETURN;
	} else if (dest_type == adr) {
	    if (DATA[setnum_dest] == NULL) {
		return_code =
		    1; /*ERROR: Trying to read/write unmapped memory!*/
		_RETURN;
	    } else if (MASK[setnum_dest] == 1) {
		return_code = 2; /*ERROR: Write to read-only address!*/
		_RETURN;
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
		return_code =
		    1; /*ERROR: Trying to read/write unmapped memory!*/
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
	if (PTR_VALID(DATA,
		      CURR_OP[0])) { /*If the pointer addresses are mapped*/
	    /*Read the address presented and set the PC*/
	    _PROG_CO = GETPTR(*DATA, CURR_OP[0]);
	} else {
	    return_code = 1; /*Else report they are unmmapped*/
	}
    }
    _RETURN;
}
