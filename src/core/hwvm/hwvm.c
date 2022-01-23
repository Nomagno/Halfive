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

/*What each of the three macros do*/
/*HWVM_OU_HEX -> Setting FFFC prints memory address as hexadecimal number*/
/*HWVM_OU_RAW -> Setting FFFC prints memory address as raw character*/
/*HWVM_OU_DEBUG -> Print ERROR, HALT, and UNIMPLEMENTED messages*/
/*If none of the three are set, output is handled externally and the output flag will be set*/
#if defined(HWVM_OU_HEX) || defined(HWVM_OU_RAW) || defined(HWVM_OU_DEBUG)
#include <stdio.h>
#endif

/*Additional macro:*/
/*HWVM_IN_EXTERN -> If input is handled externally. If NOT set, the input flag will be set, and so asking for input will always happen one cycle too late*/

#define UADDR 0xFFF0
/*Unknown address*/

HWVM_GeneralMemory HWVM_Init(HWVM_CodeMemory code);
hwuint HWVM_Execute(HWVM_GeneralMemory *program);


hwuint hbin(hwuint op[4], HWVM_DataMemory *space, hwuint flag, _Bool do_save);
hwuint hset(hwuint op[4], HWVM_DataMemory *space);
hwuint hjump(hwuint op[4], HWVM_DataMemory *space, hwuint *co);
hwuint hrot(hwuint op[4], HWVM_DataMemory *space);
hwuint hcall(hwuint *co, hwuint id, HWVM_GeneralMemory *prog);
hwuint hsubs(hwuint *co, hwuint id, HWVM_GeneralMemory *prog);
hwuint hsube(hwuint *co, hwuint id, HWVM_GeneralMemory *prog);

HWVM_GeneralMemory HWVM_Init(HWVM_CodeMemory code)
{
	HWVM_GeneralMemory memory = {0};
	memory.m1 = code;
	memory.m2.zf = 1;
	return memory;
}

hwuint HWVM_Execute(HWVM_GeneralMemory *program)
{
	hwuint errno;
	program->m2.fo = 0;
	program->m2.fi = 0;
	if (program->m2.co < (MEMSIZE * 4)) {
		switch (program->m1.inst[program->m2.co]) {
		case halt:
			program->hf = 1; /*Stop the CPU*/
			program->m2.co += 1;
#ifdef HWVM_OU_DEBUG
			printf("HALT\n");
#endif
			return 0;
		case nop:
			program->m2.co += 1; /*Do nothing*/
			return 0;
		case set:
			errno = hset(program->m1.opnd[program->m2.co],
				     &program->m2); /*Set address to value*/
			program->m2.co += 1;
			return 0;
		case jmp:
			errno = hjump(program->m1.opnd[program->m2.co],
				      &program->m2, &program->m2.co); /*Jump*/
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case jcz:
			if (program->m2.zf == 0)
				errno =
				    hjump(program->m1.opnd[program->m2.co],
					  &program->m2,
					  &program->m2.co); /*Jump if ZF is 0*/
			else {
				program->m2.co += 1;
				errno = 0;
			}
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case jcnz:
			if (program->m2.zf)
				errno =
				    hjump(program->m1.opnd[program->m2.co],
					  &program->m2,
					  &program->m2.co); /*Jump if ZF is not zero*/
			else {
				program->m2.co += 1;
				errno = 0;
			}
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case add:
			errno = hbin(program->m1.opnd[program->m2.co],
				     &program->m2, 4, 1); /*Add and save*/
			program->m2.co += 1;
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case sub:
			errno = hbin(program->m1.opnd[program->m2.co],
				     &program->m2, 5, 1); /*Substract and save*/
			program->m2.co += 1;
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case and:
			errno = hbin(program->m1.opnd[program->m2.co],
				     &program->m2, 1, 1); /*Perform binary AND*/
			program->m2.co += 1;
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case or:
			errno = hbin(program->m1.opnd[program->m2.co],
				     &program->m2, 2, 1); /*Perform binary OR*/
			program->m2.co += 1;
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case xor:
			errno =
			    hbin(program->m1.opnd[program->m2.co], &program->m2,
				 3, 1); /*Perform binary EXCLUSIVE OR*/
			program->m2.co += 1;
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case rot:
			errno = hrot(program->m1.opnd[program->m2.co],
				     &program->m2); /*Perform binary NOT*/
			program->m2.co += 1;
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case cmp:
			errno =
			    hbin(program->m1.opnd[program->m2.co], &program->m2,
				 5, 0); /*Substract but don't save*/
			program->m2.co += 1;
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case subs: /*UNIMPLEMENTED*/
			errno = hsubs(&(program->m2.co),
				      program->m1.opnd[program->m2.co][0],
				      program); /*Mark start of subroutine*/
			program->m2.co += 1;
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case sube: /*UNIMPLEMENTED*/
			errno = hsube(&(program->m2.co),
				      program->m1.opnd[program->m2.co][0],
				      program); /*Mark end of subroutine*/
			program->m2.co += 1;
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case call: /*UNIMPLEMENTED*/
			errno = hcall(&(program->m2.co),
				      program->m1.opnd[program->m2.co][0],
				      program); /*Call subroutine*/
			program->m2.co += 1;
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		}
	}

#ifdef HWVM_OU_DEBUG
	printf("UNIMPLEMENTED\n");
#endif
	return 1; /*UNIMPLEMENTED*/
}

/*
addrcheck() returns the actual address space of the argument address

0xFFF0: UNKNOWN ADDRESS SPACE
1: GENERAL PURPOSE MEMORY
2: CENTRAL PROCESSING UNIT MEMORY
3: ZERO FLAG
4: CARRY FLAG
5: INPUT REGISTER (READ-ONLY)
6: OUTPUT REGISTER (WRITE-ONLY)
7: PERSISTENT STORAGE
*/

hwuint addrcheck(hwuint arg)
{
	if (arg <= 0x3FFF)
		return 1;
	else if (arg <= 0xBFFF)
		return 7;
	else if (arg == 0xFFFF)
		return 3;
	else if (arg == 0xFFFE)
		return 4;
	else if (arg == 0xFFFD)
		return 5;
	else if (arg == 0xFFFC)
		return 6;
	else if (arg == 0xFFFB)
		return 2;
	else if (arg == 0xFFFA)
		return 8;
	else
		return 0;
}

hwuint addrconvert(hwuint arg, hwuint addr)
{
	switch (arg) {
	case 0:
		return UADDR;
	case 1:
		return addr;
	case 2:
		return 0;
	case 3:
		return 0;
	case 4:
		return 0;
	case 5:
		return 0;
	case 6:
		return 0;
	case 7:
		return (addr - 0x4000);
	case 8:
		return 0;
	default:
		return UADDR;
	}
}

/*INCREDIBLY COMMON BOILERPLATE MOVED TO OWN FUNCTION*/
hwuint auxset(hwuint *val, HWVM_DataMemory *space, hwuint ad, hwuint conv, _Bool do_write,
	      _Bool isptr, _Bool jmpmode)
{
	if (!do_write) {
		switch (ad) {
		case 1:; /*If we're dealing with a pointer, treat the value of
			two contiguous byes as the address and check if is
			within bounds. Else just*/
			if (isptr) {
				unsigned ptrval =
				    (((hwuint)space->gp[conv] << 8) |
				     space->gp[conv + 1]);
				unsigned adptr = addrcheck(ptrval);
				unsigned adptrconv = addrconvert(adptr, ptrval);
				if (adptr == 1) {
					*val = (jmpmode) ? ptrval : space->gp[adptrconv];
				} else if (adptr == 7) {
					*val = (jmpmode) ? ptrval : space->dr[adptrconv];
				}
			} else
				*val = space->gp[conv];
			break;
		case 2:;
			*val = space->co & 0xFFFF;
			break;
		case 8:;
			*val = space->co >> 8;
			break;
		case 3:;
			*val = space->zf;
			break;
		case 4:;
			*val = space->cf;
			break;
		case 5:;
			*val = space->in;
#ifndef HWVM_IN_EXTERN
			space->fi = 1;
#endif
			break;
		case 6:;
			return 1; /*Can't read output register!*/
			break;
		case 7: /*If we're dealing with a pointer, treat the value of
			two contiguous byes as the address and check if is
			within bounds. Else just*/
			if (isptr) {
				unsigned ptrval =
				    (((hwuint)space->dr[conv] << 8) +
				     space->dr[conv + 1]);
				unsigned adptr = addrcheck(ptrval);
				unsigned adptrconv = addrconvert(adptr, ptrval);
				if (adptr == 1)
					*val = (jmpmode) ? adptrconv : space->gp[adptrconv];
				else if (adptr == 7)
					*val = (jmpmode) ? adptrconv : space->dr[adptrconv];
			} else
				*val = space->gp[conv];
			break;
		default:
			return 1; /*ERROR, WE DON'T KNOW WHAT THAT ADDRESS
				     MEANS*/
		}
	} else {
		switch (ad) {
		case 1:; /*If we're dealing with a pointer, treat the value of
			two contiguous byes as the address and check if is
			within bounds. Else just*/
			if (isptr) {
				unsigned ptrval =
				    (((hwuint)space->gp[conv] << 8) +
				     space->gp[conv + 1]);
				unsigned adptr = addrcheck(ptrval);
				unsigned adptrconv = addrconvert(adptr, ptrval);
				if (adptr == 1)
					space->gp[adptrconv] = *val;
				else if (adptr == 7)
					return 1; /*You can't set the drive!*/
			} else
				space->gp[conv] = *val;
			break;
		case 2:;
			return 1; /*Can't set program counter! You can use JMP
				     for that*/
			break;
		case 8:;
			return 1; /*Can't set program counter! You can use JMP
				     for that*/
			break;
		case 3:;
			space->zf = *val;
			break;
		case 4:;
			space->cf = *val;
			break;
		case 5:;
			return 1; /*Can't set input register!*/
			break;
		case 6:;
			space->ou = *val;
#ifdef HWVM_OU_RAW
			printf("%c", space->ou);
#elif defined(HWVM_OU_HEX)
			printf("%X\n", space->ou);
#else
			space->fo = 1;
#endif

			break;
		case 7:
			return 1; /*You can't set the drive!*/
			break;
		default:
			return 1; /*ERROR, WE DON'T KNOW WHAT THAT ADDRESS
				     MEANS*/
		}
	}
	return 0;
}

/*Meta-instruction that has two configurations:
1. first argument is (pointer, or address)
    operate first and second and save result to first argument (Pointer, or
address). set flags accordingly (zero and possibly carry)

2. Same as 1, but don't save the result

Available binary operations are:
ADD, SUB, XOR, OR, AND

Note substract (SUB) may use the second configuration,
in the form of the compare (CMP) instruction
*/

hwuint hbin(hwuint op[4], HWVM_DataMemory *space, hwuint flag, _Bool do_save)
{
	hwuint ad1;
	hwuint ad2;

	hwuint isptr1 = 0;

	hwuint conv1;
	hwuint conv2;

	hwuint val1;
	hwuint val2;
	hwuchar result;
	hwuint castresult;

	ad1 = addrcheck(op[0]);
	conv1 = addrconvert(ad1, op[0]);
	if (op[2] !=
	    (op[2] |
	     2)) { /*We're dealing with an address, corresponding bit not set*/
		if (auxset(&val1, space, ad1, conv1, 0, 0, 0)) {
#ifdef HWVM_OU_DEBUG
			printf("ERROR\n");
#endif
			return 1;
		}

	} else { /*We're dealing with a literal pointer, corresponding bit set
		    AND literal bit set*/
		isptr1 = 1;
		if (op[2] >> 3) {

			if (auxset(&val1, space, ad1, conv1, 0, 1, 0)) {
#ifdef HWVM_OU_DEBUG
				printf("ERROR\n");
#endif
				return 1;
			}
		}
	}

	ad2 = addrcheck(op[1]);
	conv2 = addrconvert(ad2, op[1]);
	if (op[2] != (op[2] | 1)) {
		if (auxset(&val2, space, ad2, conv2, 0,
			   0, 0)) { /*We're dealing with an address, corresponding
				    bit not set*/
#ifdef HWVM_OU_DEBUG
			printf("ERROR\n");
#endif
			return 1;
		}
	} else {
		if (op[2] >> 3) { /*We're dealing with a literal pointer,
				     corresponding bit set AND literal bit set*/
			if (auxset(&val2, space, ad2, conv2, 0, 1, 0)) {
#ifdef HWVM_OU_DEBUG
				printf("ERROR\n");
#endif
				return 1;
			}
		} else { /*We're dealing with a literal number, corresponding
			    bit set AND literal bit not set*/
			val2 = op[1];
		}
	}

	switch (flag) {
	case 1:
		result = (hwuchar)val1 & (hwuchar)val2;
		break;
	case 2:
		result = (hwuchar)val1 | (hwuchar)val2;
		break;
	case 3:
		result = (hwuchar)val1 ^ (hwuchar)val2;
		break;
	case 4:
		result = (hwuchar)val1 + (hwuchar)val2;
		space->cf = (result < val1);
		break;
	case 5:
		result = (hwuchar)val1 - (hwuchar)val2;
		space->cf = (result > val1);
		break;
	}
	if (result == 0)
		space->zf = 0;
	else
		space->zf = 1;

	if (do_save) {
		castresult = result;
		if (auxset(&castresult, space, ad1, conv1, 1,
			   isptr1, 0)) {
#ifdef HWVM_OU_DEBUG
			printf("ERROR\n");
#endif
			return 1;
		}
	}

	return 0;
}

/*Takes two operands, if second is less than 8, bitshift left that many bits.
	 if second is less than 16, substract 8 and bitshift right that many
  bits. if second is equal to or greater than 16, do nothing. First argument
  is destination address (Pointer, or address)*/
hwuint hrot(hwuint op[3], HWVM_DataMemory *space)
{
	hwuint ad1;
	hwuint ad2;
	hwuint isptr1 = 0;

	hwuint conv1;
	hwuint conv2;

	hwuint val1;
	hwuint val2;
	hwuint result;
	_Bool do_nothing = 1;

	ad1 = addrcheck(op[0]);
	conv1 = addrconvert(ad1, op[0]);
	if (op[2] !=
	    (op[2] |
	     2)) { /*We're dealing with an address, corresponding bit not set*/
		if (auxset(&val1, space, ad1, conv1, 0, 0, 0)) {
#ifdef HWVM_OU_DEBUG
			printf("ERROR\n");
#endif
			return 1;
		}
	} else {
		if (op[2] >> 3) { /*We're dealing with a literal pointer,
				     corresponding bit set AND literal bit set*/
		isptr1 = 1;
			if (auxset(&val1, space, ad1, conv1, 0, 1, 0)) {
#ifdef HWVM_OU_DEBUG
				printf("ERROR\n");
#endif
				return 1;
			}
		} else { /*We're dealing with a literal number, corresponding
			    bit set AND literal bit not set*/
			val1 = op[0];
		}
	}

	ad2 = addrcheck(op[1]);
	conv2 = addrconvert(ad2, op[1]);
	if (op[2] != (op[2] | 1)) {
		if (auxset(&val2, space, ad2, conv2, 0, 0, 0)) {
#ifdef HWVM_OU_DEBUG
			printf("ERROR\n");
#endif
			return 1;
		}
	} else {
		if (op[2] >> 3) {
			if (auxset(&val2, space, ad2, conv2, 0, 1, 0)) {
#ifdef HWVM_OU_DEBUG
				printf("ERROR\n");
#endif
				return 1;
			}
		} else {
			val2 = op[1];
		}
	}
	if (val2 < 8)
		result = val1 << val2;
	else if (val2 < 16)
		result = val1 >> (val2 - 8);
	else
		do_nothing = 0;


	if (do_nothing && auxset(&result, space, ad1, conv1, 1,
				 isptr1, 0)) {
#ifdef HWVM_OU_DEBUG
		printf("ERROR\n");
#endif
		return 1;
	}

	return 0;
}

/*Takes one argument, move program counter to value (Pointer, literal, or
 * address)*/
hwuint hjump(hwuint op[4], HWVM_DataMemory *space, hwuint *co)
{
	hwuint adr;
	hwuint conv;
	hwuint val;

	adr = addrcheck(op[0]);
	conv = addrconvert(adr, op[0]);
	if (op[2] != (op[2] | 2)){
	     val = op[0]; /*Addresses are treated as literals*/
	} else {
		if (op[2] >> 3) { /*We're dealing with a literal pointer,
				     corresponding bit set AND literal bit set*/
			if (auxset(&val, space, adr, conv, 0, 1, 1)) {
#ifdef HWVM_OU_DEBUG
				printf("ERROR\n");
#endif
				return 1;
			}
		} else { /*We're dealing with a literal number, corresponding
			    bit set AND literal bit not set*/
			val = op[0];
		}
	}
	*co = val;
	return 0;
}

/*Takes one argument, look up ID on out-of-memory
chip and jump to its corresponding code address.
Note down current code address for returning from
the subroutine*/
hwuint hcall(hwuint *co, hwuint id, HWVM_GeneralMemory *prog)
{
	prog->return_co[id] = *co;
	*co = prog->sub_co[id];
	return 0;
}

/*Takes one argument, write to ID slot on out-of-memory
chip and look for end of declaration (closest sube). Note it
down too. Jump to after the sube*/
hwuint hsubs(hwuint *co, hwuint id, HWVM_GeneralMemory *prog)
{
	hwuint val = 0; /*It is impossible counter 0
			is a safe place to return to,
			which is why it is an error value*/
	prog->sub_co[id] = *co;
	for (int i = 0;
	     (long unsigned int)i < (sizeof(prog->m1.inst) - (*co + 1)); i++) {
		if (prog->m1.inst[*co + 1 + i] == sube) {
			val = *co + 1 + i;
			break;
		}
	}
	if (val)
		*co = val;
	else
		return 1;

	return 0;
}

/*Takes one argument, look up ID on out-of-memory chip
and jump to the intruction right after the call*/
hwuint hsube(hwuint *co, hwuint id, HWVM_GeneralMemory *prog)
{
	*co = prog->return_co[id];
	return 0;
}

/*Takes two arguments, copy value of second
(Pointer, literal, or address) to second (Pointer, or address)*/
hwuint hset(hwuint op[3], HWVM_DataMemory *space)
{
	hwuint ad1;
	hwuint ad2;
	hwuint isptr1 = 0;

	hwuint conv1;
	hwuint conv2;

	hwuint val;

	ad1 = addrcheck(op[0]);
	conv1 = addrconvert(ad1, op[0]);
	if (op[2] !=
	    (op[2] |
	     2)) { /*We're dealing with an address, corresponding bit not set*/
		if (auxset(&val, space, ad1, conv1, 0, 0, 0)) {
#ifdef HWVM_OU_DEBUG
			printf("ERROR\n");
#endif
			return 1;
		}
	} else {
		if (op[2] >> 3) { /*We're dealing with a literal pointer,
				     corresponding bit set AND literal bit set*/
			isptr1 = 0;
			if (auxset(&val, space, ad1, conv1, 0, 1, 0)) {
#ifdef HWVM_OU_DEBUG
				printf("ERROR\n");
#endif
				return 1;
			}
		} else { /*We're dealing with a literal number, corresponding
			    bit set AND literal bit not set*/
			val = op[0];
		}
	}

	ad2 = addrcheck(op[1]);
	conv2 = addrconvert(ad2, op[1]);

	if (auxset(&val, space, ad2, conv2, 1,
		   isptr1, 0)) {
#ifdef HWVM_OU_DEBUG
		printf("ERROR\n");
#endif
		return 1;
	}
	return 0;
}

