/*
Copyright Nomagno 2021

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

#include "hwvm.h"
#include <stdio.h>
/*<stdio.h> IS NOT NEEDED, IT ONLY ADDS
PRINTING AND TESTING FUNCTIONALITY*/

#define UADDR 0xFFF0
/*Unknown address*/

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
uint addrcheck(uint arg)
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
	else
		return 0;
}

uint addrconvert(uint arg, uint addr)
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
	default:
		return UADDR;
	}
}

/*INCREDIBLY COMMON BOILERPLATE MOVED TO OWN FUNCTION*/
uint auxset(uint *val, vmem *space, uint ad, uint conv, _Bool do_write,
	    _Bool isptr)
{
	if (!do_write) {
		switch (ad) {
		case 1:; /*If we're dealing with a pointer, treat the value of
			two contiguous byes as the address and check if is
			within bounds. Else just*/
			if (isptr) {
				unsigned ptrval =
				    (((uint)space->gp[conv] << 8) |
				     space->gp[conv + 1]);
				unsigned adptr = addrcheck(ptrval);
				unsigned adptrconv = addrconvert(adptr, ptrval);
				if (adptr == 1) {
					*val = space->gp[adptrconv];
				} else if (adptr == 7) {
					*val = space->dr[adptrconv];
				}
			} else
				*val = space->gp[conv];
			break;
		case 2:;
			*val = space->co;
			break;
		case 3:;
			*val = space->zf;
			break;
		case 4:;
			*val = space->cf;
			break;
		case 5:;
			*val = space->in;
			break;
		case 6:;
			return 1; /*Can't read output register!*/
			break;
		case 7: /*If we're dealing with a pointer, treat the value of
			two contiguous byes as the address and check if is
			within bounds. Else just*/
			if (isptr) {
				unsigned ptrval =
				    (((uint)space->dr[conv] << 8) +
				     space->dr[conv + 1]);
				unsigned adptr = addrcheck(ptrval);
				unsigned adptrconv = addrconvert(adptr, ptrval);
				if (adptr == 1)
					*val = space->gp[adptrconv];
				else if (adptr == 7)
					*val = space->dr[adptrconv];
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
				    (((uint)space->gp[conv] << 8) +
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
#ifdef EOF
			printf("%i\n", space->ou);
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

void hnop(void) { return; }

uint hbin(uint op[4], vmem *space, uint flag)
{
	uint ad1;
	uint ad2;
	uint ad3;

	uint conv1;
	uint conv2;
	uint conv3;

	uint val1;
	uint val2;
	uint result;

	ad1 = addrcheck(op[0]);
	conv1 = addrconvert(ad1, op[0]);
	if (op[3] != (op[3] | 4)) {
		if (auxset(&val1, space, ad1, conv1, 0, 0)) {
#ifdef EOF
			printf("ERROR\n");
#endif
			return 1;
		}
	} else {
		if (op[3] >> 3) {
			if (auxset(&val1, space, ad1, conv1, 0, 1)) {
#ifdef EOF
				printf("ERROR\n");
#endif
				return 1;
			}
		} else {
			val1 = op[0];
		}
	}

	ad2 = addrcheck(op[1]);
	conv2 = addrconvert(ad2, op[1]);
	if (op[3] != (op[3] | 2)) {
		if (auxset(&val2, space, ad2, conv2, 0, 0)) {
#ifdef EOF
			printf("ERROR\n");
#endif
			return 1;
		}
	} else {
		if (op[3] >> 3) {
			if (auxset(&val2, space, ad2, conv2, 0, 1)) {
#ifdef EOF
				printf("ERROR\n");
#endif
				return 1;
			}
		} else {
			val2 = op[1];
		}
	}

	switch (flag) {
	case 1:
		result = val1 & val2;
		break;
	case 2:
		result = val1 | val2;
		break;
	case 3:
		result = val1 ^ val2;
		break;
	}

	ad3 = addrcheck(op[2]);
	conv3 = addrconvert(ad3, op[2]);
	if (auxset(&result, space, ad3, conv3, 1, (op[3] >> 3))) {
#ifdef EOF
		printf("ERROR\n");
#endif
		return 1;
	}

	return 0;
}

uint hjump(uint op[4], vmem *space, uint *co)
{
	uint adr;
	uint conv;
	uint val;

	adr = addrcheck(op[0]);
	conv = addrconvert(adr, op[0]);
	if (op[3] != (op[3] | 4)) {
		if (auxset(&val, space, adr, conv, 0, 0)) {
#ifdef EOF
			printf("ERROR\n");
#endif
			return 1;
		}
	} else {
		if (op[3] >> 3) {
			if (auxset(&val, space, adr, conv, 0, 1)) {
#ifdef EOF
				printf("ERROR\n");
#endif
				return 1;
			}
		} else {
			val = op[0];
		}
	}
	*co = val;
	return 0;
}

uint hnot(uint op[4], vmem *space)
{
	uint ad1;
	uint ad2;

	uint conv1;
	uint conv2;

	uint val1;

	ad1 = addrcheck(op[0]);
	conv1 = addrconvert(ad1, op[0]);
	if (op[3] != (op[3] | 4)) {
		if (auxset(&val1, space, ad1, conv1, 0, 0)) {
#ifdef EOF
			printf("ERROR\n");
#endif
			return 1;
		}
	} else {
		if (op[3] >> 3) {
			if (auxset(&val1, space, ad1, conv1, 0, 1)) {
#ifdef EOF
				printf("ERROR\n");
#endif
				return 1;
			}
		} else {
			val1 = op[0];
		}
	}
	val1 = ~val1;

	ad2 = addrcheck(op[1]);
	conv2 = addrconvert(ad2, op[1]);
	if (auxset(&val1, space, ad2, conv2, 1, (op[3] >> 3))) {
#ifdef EOF
		printf("ERROR\n");
#endif
		return 1;
	}

	return 0;
}

uint hbop(uint op[4], vmem *space, _Bool do_save, _Bool do_add)
{
	uint ad1;
	uint ad2;
	uint ad3;

	uint conv1;
	uint conv2;
	uint conv3;

	uint val1;
	uint val2;

	uint result;

	ad1 = addrcheck(op[0]);
	conv1 = addrconvert(ad1, op[0]);
	if (op[3] != (op[3] | 4)) {
		if (auxset(&val1, space, ad1, conv1, 0, 0)) {
#ifdef EOF
			printf("ERROR\n");
#endif
			return 1;
		}
	} else {
		if (op[3] >> 3) {
			if (auxset(&val1, space, ad1, conv1, 0, 1)) {
#ifdef EOF
				printf("ERROR\n");
#endif
				return 1;
			}
		} else {
			val1 = op[0];
		}
	}

	ad2 = addrcheck(op[1]);
	conv2 = addrconvert(ad2, op[1]);
	if (op[3] != (op[3] | 2)) {
		if (auxset(&val2, space, ad2, conv2, 0, 0)) {
#ifdef EOF
			printf("ERROR\n");
#endif
			return 1;
		}
	} else {
		if (op[3] >> 3) {
			if (auxset(&val2, space, ad2, conv2, 0, 1)) {
#ifdef EOF
				printf("ERROR\n");
#endif
				return 1;
			}
		} else {
			val2 = op[1];
		}
	}

	if (do_add) {
		result = val1 + val2;
		if ((result < val1) || (result < val2))
			space->cf = 1;
		else
			space->cf = 0;

		if (result == 0)
			space->zf = 0;
		else
			space->zf = 1;
	} else {
		result = val1 - val2;
		if ((result > val1))
			space->cf = 1;
		else
			space->cf = 0;

		if (result == 0)
			space->zf = 0;
		else
			space->zf = 1;
	}

	ad3 = addrcheck(op[2]);
	conv3 = addrconvert(ad3, op[2]);
	if (do_save) {
		if (auxset(&result, space, ad3, conv3, 1, (op[3] >> 3))) {
#ifdef EOF
			printf("ERROR\n");
#endif
			return 1;
		}
	}
	return 0;
}

uint hcall(uint *co, uint id, mem *prog)
{
	prog->return_co[id] = *co;
	*co = prog->sub_co[id];
	return 0;
}

uint hsubs(uint *co, uint id, mem *prog)
{
	uint val = 0; /*It is impossible counter 0
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

uint hsube(uint *co, uint id, mem *prog)
{
	*co = prog->return_co[id];
	return 0;
}

uint hset(uint op[4], vmem *space)
{
	uint ad1;
	uint ad2;

	uint conv1;
	uint conv2;

	uint val;

	ad1 = addrcheck(op[0]);
	conv1 = addrconvert(ad1, op[0]);
	if (op[3] != (op[3] | 4)) {
		if (auxset(&val, space, ad1, conv1, 0, 0)) {
#ifdef EOF
			printf("ERROR\n");
#endif
			return 1;
		}
	} else {
		if (op[3] >> 3) {
			if (auxset(&val, space, ad1, conv1, 0, 1)) {
#ifdef EOF
				printf("ERROR\n");
#endif
				return 1;
			}
		} else {
			val = op[0];
		}
	}

	ad2 = addrcheck(op[1]);
	conv2 = addrconvert(ad2, op[1]);
	if (auxset(&val, space, ad2, conv2, 1, (op[3] >> 3))) {
#ifdef EOF
		printf("ERROR\n");
#endif
		return 1;
	}
	return 0;
}

mem fxmem(xmem code)
{
	mem memory = {0};
	memory.m1 = code;
	memory.m2.zf = 1;
	return memory;
}

uint execnext(mem *program)
{
	uint errno;
	if (program->m2.co < (MEMSIZE * 4)) {
		switch (program->m1.inst[program->m2.co]) {
		case halt:
			program->hf = 1;
			program->m2.co += 1;
#ifdef EOF
			printf("HALT\n");
#endif
			return 0;
		case nop:
			program->m2.co += 1;
			return 0;
		case set:
			errno = hset(program->m1.opnd[program->m2.co],
				     &program->m2);
			program->m2.co += 1;
			return 0;
		case jmp:
			errno = hjump(program->m1.opnd[program->m2.co],
				      &program->m2, &program->m2.co);
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case jcz:
			if (program->m2.zf == 0)
				errno = hjump(program->m1.opnd[program->m2.co],
					      &program->m2, &program->m2.co);
			else {
				program->m2.co += 1;
				errno = 0;
			}
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case jcnz:
			if (program->m2.zf == 1)
				errno = hjump(program->m1.opnd[program->m2.co],
					      &program->m2, &program->m2.co);
			else {
				program->m2.co += 1;
				errno = 0;
			}
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case add:
			errno = hbop(program->m1.opnd[program->m2.co],
				     &program->m2, 1, 1);
			program->m2.co += 1;
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case sub:
			errno = hbop(program->m1.opnd[program->m2.co],
				     &program->m2, 1, 0); /*Substract and save*/
			program->m2.co += 1;
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case and:
			errno =
			    hbin(program->m1.opnd[program->m2.co], &program->m2,
				 1); /*Substract but don't save*/
			program->m2.co += 1;
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case or:
			errno =
			    hbin(program->m1.opnd[program->m2.co], &program->m2,
				 2); /*Substract but don't save*/
			program->m2.co += 1;
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case xor:
			errno =
			    hbin(program->m1.opnd[program->m2.co], &program->m2,
				 3); /*Substract but don't save*/
			program->m2.co += 1;
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case not:
			errno = hnot(program->m1.opnd[program->m2.co],
				     &program->m2); /*Substract but don't save*/
			program->m2.co += 1;
			if (errno != 0) {
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case cmp:
			errno =
			    hbop(program->m1.opnd[program->m2.co], &program->m2,
				 1, 0); /*Substract but don't save*/
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

#if defined(EOF)
	printf("UNIMPLEMENTED\n");
#endif
	return 1; /*UNIMPLEMENTED*/
}
