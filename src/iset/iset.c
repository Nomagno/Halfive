/*
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

#include "iset.h"
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
8: CALL STACK
*/
uint addrcheck(uint arg)
{
	if (arg <= 0x3FFF)
		return 1;
	else if (arg <= 0x5FFF)
		return 2;
	else if (arg <= 0xDFFF)
		return 7;
	else if (arg == 0xFFFF)
		return 3;
	else if (arg == 0xFFFE)
		return 4;
	else if (arg == 0xFFFD)
		return 5;
	else if (arg == 0xFFFC)
		return 6;
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
		return (addr - 0x4000);
	case 3:
		return 0;
	case 4:
		return 0;
	case 5:
		return 0;
	case 6:
		return 0;
	case 7:
		return (addr - 0x6000);
	default:
		return UADDR;
	}
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

	switch (op[3]) {
	case 0:
		ad1 = addrcheck(op[0]);
		ad2 = addrcheck(op[1]);
		ad3 = addrcheck(op[2]);

		conv1 = addrconvert(ad1, op[0]);
		conv2 = addrconvert(ad2, op[1]);
		conv3 = addrconvert(ad3, op[2]);
		switch (ad1) {
		case 1:
			val1 = space->gp[conv1];
			break;
		case 2:
			val1 = space->cp[conv1];
			break;
		case 3:
			val1 = space->zf;
			break;
		case 4:
			val1 = space->cf;
			break;
		case 5:
			val1 = space->in;
			break;
		case 6:
			return 1; /*Can't read output register!*/
			break;
		case 7:
			val1 = space->dr[conv1];
			break;
		}
		switch (ad2) {
		case 1:
			val2 = space->gp[conv2];
			break;
		case 2:
			val2 = space->cp[conv2];
			break;
		case 3:
			val2 = space->zf;
			break;
		case 4:
			val2 = space->cf;
			break;
		case 5:
			val2 = space->in;
			break;
		case 6:
			return 1; /*Can't read output register!*/
			break;
		case 7:
			val2 = space->dr[conv2];
			break;
		}
		switch (ad3) {
		case 1:
			if (flag == 1)
				space->gp[conv3] = val1 & val2;
			else if (flag == 2)
				space->gp[conv3] = val1 | val2;
			else if (flag == 3)
				space->gp[conv3] = val1 ^ val2;
			break;
		case 2:
			if (flag == 1)
				space->cp[conv3] = val1 & val2;
			else if (flag == 2)
				space->cp[conv3] = val1 | val2;
			else if (flag == 3)
				space->cp[conv3] = val1 ^ val2;
			break;
		case 3:
			if (flag == 1)
				space->zf = val1 & val2;
			else if (flag == 2)
				space->zf = val1 | val2;
			else if (flag == 3)
				space->zf = val1 ^ val2;
			break;
		case 4:
			if (flag == 1)
				space->cf = val1 & val2;
			else if (flag == 2)
				space->cf = val1 | val2;
			else if (flag == 3)
				space->cf = val1 ^ val2;
			break;
		case 5:
			return 1; /*You can't set the input register!*/
		case 6:
			if (flag == 1)
				space->ou = val1 & val2;
			else if (flag == 2)
				space->ou = val1 | val2;
			else if (flag == 3)
				space->ou = val1 ^ val2;
#ifdef EOF
			printf("%i\n", space->ou);
#endif
			break;
		case 7:
			if (flag == 1)
				space->dr[conv3] = val1 & val2;
			else if (flag == 2)
				space->dr[conv3] = val1 | val2;
			else if (flag == 3)
				space->dr[conv3] = val1 ^ val2;
			break;
		default:
			return 3; /*We don't know what that address means*/
		}
		break;
	case 4:
		ad2 = addrcheck(op[1]);
		ad3 = addrcheck(op[2]);

		conv2 = addrconvert(ad2, op[1]);
		conv3 = addrconvert(ad3, op[2]);

		val1 = op[0];
		switch (ad2) {
		case 1:
			val2 = space->gp[conv2];
			break;
		case 2:
			val2 = space->cp[conv2];
			break;
		case 3:
			val2 = space->zf;
			break;
		case 4:
			val2 = space->cf;
			break;
		case 5:
			val2 = space->in;
			break;
		case 6:
			return 1; /*Can't read output register!*/
			break;
		case 7:
			val2 = space->dr[conv2];
			break;
		}
		switch (ad3) {
		case 1:
			if (flag == 1)
				space->gp[conv3] = val1 & val2;
			else if (flag == 2)
				space->gp[conv3] = val1 | val2;
			else if (flag == 3)
				space->gp[conv3] = val1 ^ val2;
			break;
		case 2:
			if (flag == 1)
				space->cp[conv3] = val1 & val2;
			else if (flag == 2)
				space->cp[conv3] = val1 | val2;
			else if (flag == 3)
				space->cp[conv3] = val1 ^ val2;
			break;
		case 3:
			if (flag == 1)
				space->zf = val1 & val2;
			else if (flag == 2)
				space->zf = val1 | val2;
			else if (flag == 3)
				space->zf = val1 ^ val2;
			break;
		case 4:
			if (flag == 1)
				space->cf = val1 & val2;
			else if (flag == 2)
				space->cf = val1 | val2;
			else if (flag == 3)
				space->cf = val1 ^ val2;
			break;
		case 5:
			return 1; /*You can't set the input register!*/
		case 6:
			if (flag == 1)
				space->ou = val1 & val2;
			else if (flag == 2)
				space->ou = val1 | val2;
			else if (flag == 3)
				space->ou = val1 ^ val2;
#ifdef EOF
			printf("%i\n", space->ou);
#endif
			break;
		case 7:
			if (flag == 1)
				space->dr[conv3] = val1 & val2;
			else if (flag == 2)
				space->dr[conv3] = val1 | val2;
			else if (flag == 3)
				space->dr[conv3] = val1 ^ val2;
			break;
		default:
			return 3; /*We don't know what that address means*/
		}
		break;
	case 2:
		ad1 = addrcheck(op[0]);
		ad3 = addrcheck(op[2]);

		conv1 = addrconvert(ad1, op[0]);
		conv3 = addrconvert(ad3, op[2]);

		val2 = op[1];
		switch (ad1) {
		case 1:
			val1 = space->gp[conv1];
			break;
		case 2:
			val1 = space->cp[conv1];
			break;
		case 3:
			val1 = space->zf;
			break;
		case 4:
			val1 = space->cf;
			break;
		case 5:
			val1 = space->in;
			break;
		case 6:
			return 1; /*Can't read output register!*/
			break;
		case 7:
			val1 = space->dr[conv1];
			break;
		}
		switch (ad3) {
		case 1:
			if (flag == 1)
				space->gp[conv3] = val1 & val2;
			else if (flag == 2)
				space->gp[conv3] = val1 | val2;
			else if (flag == 3)
				space->gp[conv3] = val1 ^ val2;
			break;
		case 2:
			if (flag == 1)
				space->cp[conv3] = val1 & val2;
			else if (flag == 2)
				space->cp[conv3] = val1 | val2;
			else if (flag == 3)
				space->cp[conv3] = val1 ^ val2;
			break;
		case 3:
			if (flag == 1)
				space->zf = val1 & val2;
			else if (flag == 2)
				space->zf = val1 | val2;
			else if (flag == 3)
				space->zf = val1 ^ val2;
			break;
		case 4:
			if (flag == 1)
				space->cf = val1 & val2;
			else if (flag == 2)
				space->cf = val1 | val2;
			else if (flag == 3)
				space->cf = val1 ^ val2;
			break;
		case 5:
			return 1; /*You can't set the input register!*/
		case 6:
			if (flag == 1)
				space->ou = val1 & val2;
			else if (flag == 2)
				space->ou = val1 | val2;
			else if (flag == 3)
				space->ou = val1 ^ val2;
#ifdef EOF
			printf("%i\n", space->ou);
#endif
			break;
		case 7:
			if (flag == 1)
				space->dr[conv3] = val1 & val2;
			else if (flag == 2)
				space->dr[conv3] = val1 | val2;
			else if (flag == 3)
				space->dr[conv3] = val1 ^ val2;
			break;
		default:
			return 3; /*We don't know what that address means*/
		}
		break;
	case 6:
		ad3 = addrcheck(op[2]);

		conv3 = addrconvert(ad3, op[2]);

		val2 = op[1];
		val1 = op[0];

		switch (ad3) {
		case 1:
			if (flag == 1)
				space->gp[conv3] = val1 & val2;
			else if (flag == 2)
				space->gp[conv3] = val1 | val2;
			else if (flag == 3)
				space->gp[conv3] = val1 ^ val2;
			break;
		case 2:
			if (flag == 1)
				space->cp[conv3] = val1 & val2;
			else if (flag == 2)
				space->cp[conv3] = val1 | val2;
			else if (flag == 3)
				space->cp[conv3] = val1 ^ val2;
			break;
		case 3:
			if (flag == 1)
				space->zf = val1 & val2;
			else if (flag == 2)
				space->zf = val1 | val2;
			else if (flag == 3)
				space->zf = val1 ^ val2;
			break;
		case 4:
			if (flag == 1)
				space->cf = val1 & val2;
			else if (flag == 2)
				space->cf = val1 | val2;
			else if (flag == 3)
				space->cf = val1 ^ val2;
			break;
		case 5:
			return 1; /*You can't set the input register!*/
		case 6:
			if (flag == 1)
				space->ou = val1 & val2;
			else if (flag == 2)
				space->ou = val1 | val2;
			else if (flag == 3)
				space->ou = val1 ^ val2;
#ifdef EOF
			printf("%i\n", space->ou);
#endif
			break;
		case 7:
			if (flag == 1)
				space->dr[conv3] = val1 & val2;
			else if (flag == 2)
				space->dr[conv3] = val1 | val2;
			else if (flag == 3)
				space->dr[conv3] = val1 ^ val2;
			break;
		default:
			return 3; /*We don't know what that address means*/
		}
	}
	return 0;
}

uint hjump(uint op[4], vmem *space, uint *co)
{
	uint adr;
	uint conv;

	switch (op[3]) {
	case 0:
		adr = addrcheck(op[0]);
		conv = addrconvert(adr, op[0]);
		switch (adr) {
		case 1:
			*co = space->gp[conv];
			break;
		case 2:
			*co = space->cp[conv];
			break;
		case 3:
			*co = space->zf;
			break;
		case 4:
			*co = space->cf;
			break;
		case 5:
			*co = space->in;
			break;
		case 6:
			return 1; /*You can't read the output register!*/
			break;
		case 7:
			*co = space->dr[conv];
			break;
		default:
			return 3; /*We don't know what that address means*/
		}
		break;
	case 4:
		*co = op[0];
		break;
	default:
		return 3; /*Wrong usage*/
		break;
	}
	return 0;
}

uint hnot(uint op[4], vmem *space)
{
	uint ad1;
	uint ad2;

	uint conv1;
	uint conv2;

	uint val1;
	switch (op[3]) {
	case 0:
		ad1 = addrcheck(op[0]);
		ad2 = addrcheck(op[1]);

		conv1 = addrconvert(ad1, op[0]);
		conv2 = addrconvert(ad2, op[1]);
		switch (ad1) {
		case 1:
			val1 = space->gp[conv1];
			break;
		case 2:
			val1 = space->cp[conv1];
			break;
		case 3:
			val1 = space->zf;
			break;
		case 4:
			val1 = space->cf;
			break;
		case 5:
			val1 = space->in;
			break;
		case 6:
			return 1; /*Can't read output register!*/
			break;
		case 7:
			val1 = space->dr[conv1];
			break;
		}
		switch (ad2) {
		case 1:
			space->gp[conv2] = ~val1;
			break;
		case 2:
			space->cp[conv2] = ~val1;
			break;
		case 3:
			space->zf = ~val1;
			break;
		case 4:
			space->cf = ~val1;
			break;
		case 5:
			return 1; /*Can't set input register!*/
			break;
		case 6:
			space->ou = ~val1;
#ifdef EOF
			printf("%i\n", space->ou);
#endif
			break;
		case 7:
			space->dr[conv2] = ~val1;
			break;
		}
		break;
	case 4:
		ad2 = addrcheck(op[1]);

		conv2 = addrconvert(ad2, op[1]);

		val1 = op[0];
		switch (ad2) {
		case 1:
			space->gp[conv2] = ~val1;
			break;
		case 2:
			space->cp[conv2] = ~val1;
			break;
		case 3:
			space->zf = ~val1;
			break;
		case 4:
			space->cf = ~val1;
			break;
		case 5:
			return 1; /*Can't set input register!*/
			break;
		case 6:
			space->ou = ~val1;
#ifdef EOF
			printf("%i\n", space->ou);
#endif
			break;
		case 7:
			space->dr[conv2] = ~val1;
			break;
		default:
			return 3; /*We don't know what that address means*/
		}
		break;
	default:
		return 2; /*wrong usage*/
		break;
	}
	return 0;
}

uint hsub(uint op[4], vmem *space, uchar do_save)
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

	switch (op[3]) {
	case 0:
		ad1 = addrcheck(op[0]);
		ad2 = addrcheck(op[1]);
		ad3 = addrcheck(op[2]);

		conv1 = addrconvert(ad1, op[0]);
		conv2 = addrconvert(ad2, op[1]);
		conv3 = addrconvert(ad3, op[2]);
		switch (ad1) {
		case 1:
			val1 = space->gp[conv1];
			break;
		case 2:
			val1 = space->cp[conv1];
			break;
		case 3:
			val1 = space->zf;
			break;
		case 4:
			val1 = space->cf;
			break;
		case 5:
			val1 = space->in;
			break;
		case 6:
			return 1; /*Can't read output register!*/
			break;
		case 7:
			val1 = space->dr[conv1];
			break;
		default:
			return 3; /*We don't know what that address means*/
		}
		switch (ad2) {
		case 1:
			val2 = space->gp[conv2];
			break;
		case 2:
			val2 = space->cp[conv2];
			break;
		case 3:
			val2 = space->zf;
			break;
		case 4:
			val2 = space->cf;
			break;
		case 5:
			val2 = space->in;
			break;
		case 6:
			return 1; /*Can't read output register!*/
			break;
		case 7:
			val2 = space->dr[conv2];
			break;
		default:
			return 3; /*We don't know what that address means*/
		}
		result = val1 - val2;
		if ((result > val1))
			space->cf = 1;
		else
			space->cf = 0;

		if (result == 0)
			space->zf = 0;
		else
			space->zf = 1;
		if (do_save) {
			switch (ad3) {
			case 1:
				space->gp[conv3] = result;
				break;
			case 2:
				space->cp[conv3] = result;
				break;
			case 3:
				space->zf = result;
				break;
			case 4:
				space->cf = result;
				break;
			case 5:
				return 1; /*Can't set input register!*/
				break;
			case 6:
				space->ou = result;
#ifdef EOF
				printf("%i\n", space->ou);
#endif
				break;
			case 7:
				space->dr[conv3] = result;
				break;
			default:
				return 3; /*We don't know what that address
					     means*/
			};
		}
		break;
	case 2:
		ad1 = addrcheck(op[0]);
		ad3 = addrcheck(op[2]);

		conv1 = addrconvert(ad1, op[0]);
		conv3 = addrconvert(ad3, op[2]);
		switch (ad1) {
		case 1:
			val1 = space->gp[conv1];
			break;
		case 2:
			val1 = space->cp[conv1];
			break;
		case 3:
			val1 = space->zf;
			break;
		case 4:
			val1 = space->cf;
			break;
		case 5:
			val1 = space->in;
			break;
		case 6:
			return 1; /*Can't read output register!*/
			break;
		case 7:
			val1 = space->dr[conv1];
			break;
		default:
			return 3; /*We don't know what that address means*/
		}
		val2 = op[1];
		result = val1 - val2;
		if ((result > val1))
			space->cf = 1;
		else
			space->cf = 0;

		if (result == 0)
			space->zf = 0;
		else
			space->zf = 1;

		if (do_save) {
			switch (ad3) {
			case 1:
				space->gp[conv3] = result;
				break;
			case 2:
				space->cp[conv3] = result;
				break;
			case 3:
				space->zf = result;
				break;
			case 4:
				space->cf = result;
				break;
			case 5:
				return 1; /*Can't set input register!*/
				break;
			case 6:
				space->ou = result;
#ifdef EOF
				printf("%i\n", space->ou);
#endif
				break;
			case 7:
				space->dr[conv3] = result;
				break;
			default:
				return 3; /*We don't know what that address
					     means*/
			}
		}
		break;
	case 4:
		ad2 = addrcheck(op[1]);
		ad3 = addrcheck(op[2]);

		conv2 = addrconvert(ad2, op[1]);
		conv3 = addrconvert(ad3, op[2]);
		val1 = op[0];

		switch (ad2) {
		case 1:
			val2 = space->gp[conv2];
			break;
		case 2:
			val2 = space->cp[conv2];
			break;
		case 3:
			val2 = space->zf;
			break;
		case 4:
			val2 = space->cf;
			break;
		case 5:
			val2 = space->in;
			break;
		case 6:
			return 1; /*Can't read output register!*/
			break;
		case 7:
			val2 = space->dr[conv2];
			break;
		default:
			return 3; /*We don't know what that address means*/
		}

		result = val1 - val2;
		if ((result > val1))
			space->cf = 1;
		else
			space->cf = 0;

		if (result == 0)
			space->zf = 0;
		else
			space->zf = 1;

		if (do_save) {
			switch (ad3) {
			case 1:
				space->gp[conv3] = result;
				break;
			case 2:
				space->cp[conv3] = result;
				break;
			case 3:
				space->zf = result;
				break;
			case 4:
				space->cf = result;
				break;
			case 5:
				return 1; /*Can't set input register!*/
				break;
			case 6:
				space->ou = result;
#ifdef EOF
				printf("%i\n", space->ou);
#endif
				break;
			case 7:
				space->dr[conv3] = result;
				break;
			default:
				return 3; /*We don't know what that address
					     means*/
			}
		}
		break;
	case 6:
		ad3 = addrcheck(op[2]);

		conv3 = addrconvert(ad3, op[2]);
		val1 = op[0];
		val2 = op[1];

		result = val1 - val2;
		if ((result > val1))
			space->cf = 1;
		else
			space->cf = 0;

		if (result == 0)
			space->zf = 0;
		else
			space->zf = 1;

		if (do_save) {
			switch (ad3) {
			case 1:
				space->gp[conv3] = result;
				break;
			case 2:
				space->cp[conv3] = result;
				break;
			case 3:
				space->zf = result;
				break;
			case 4:
				space->cf = result;
				break;
			case 5:
				return 1; /*Can't set input register!*/
				break;
			case 6:
				space->ou = result;
#ifdef EOF
				printf("%i\n", space->ou);
#endif
				break;
			case 7:
				space->dr[conv3] = result;
				break;
			default:
				return 3; /*We don't know what that address
					     means*/
			}
		}
		break;
	default:
		return 2; /*wrong usage*/
	}
	return 0;
}

uint hadd(uint op[4], vmem *space)
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

	switch (op[3]) {
	case 0:
		ad1 = addrcheck(op[0]);
		ad2 = addrcheck(op[1]);
		ad3 = addrcheck(op[2]);

		conv1 = addrconvert(ad1, op[0]);
		conv2 = addrconvert(ad2, op[1]);
		conv3 = addrconvert(ad3, op[2]);
		switch (ad1) {
		case 1:
			val1 = space->gp[conv1];
			break;
		case 2:
			val1 = space->cp[conv1];
			break;
		case 3:
			val1 = space->zf;
			break;
		case 4:
			val1 = space->cf;
			break;
		case 5:
			val1 = space->in;
			break;
		case 6:
			return 1; /*Can't read output register!*/
			break;
		case 7:
			val1 = space->dr[conv1];
			break;
		default:
			return 3; /*We don't know what that address means*/
		}
		switch (ad2) {
		case 1:
			val2 = space->gp[conv2];
			break;
		case 2:
			val2 = space->cp[conv2];
			break;
		case 3:
			val2 = space->zf;
			break;
		case 4:
			val2 = space->cf;
			break;
		case 5:
			val2 = space->in;
			break;
		case 6:
			return 1; /*Can't read output register!*/
			break;
		case 7:
			val2 = space->dr[conv2];
			break;
		default:
			return 3; /*We don't know what that address means*/
		}
		result = val1 + val2;
		if ((result < val1) || (result < val2))
			space->cf = 1;
		else
			space->cf = 0;

		if (result == 0)
			space->zf = 0;
		else
			space->zf = 1;

		switch (ad3) {
		case 1:
			space->gp[conv3] = result;
			break;
		case 2:
			space->cp[conv3] = result;
			break;
		case 3:
			space->zf = result;
			break;
		case 4:
			space->cf = result;
			break;
		case 5:
			return 1; /*Can't set input register!*/
			break;
		case 6:
			space->ou = result;
#ifdef EOF
			printf("%i\n", space->ou);
#endif
			break;
		case 7:
			space->dr[conv3] = result;
			break;
		default:
			return 3; /*We don't know what that address means*/
		}
		break;
	case 2:
		ad1 = addrcheck(op[0]);
		ad3 = addrcheck(op[2]);

		conv1 = addrconvert(ad1, op[0]);
		conv3 = addrconvert(ad3, op[2]);
		switch (ad1) {
		case 1:
			val1 = space->gp[conv1];
			break;
		case 2:
			val1 = space->cp[conv1];
			break;
		case 3:
			val1 = space->zf;
			break;
		case 4:
			val1 = space->cf;
			break;
		case 5:
			val1 = space->in;
			break;
		case 6:
			return 1; /*Can't read output register!*/
			break;
		case 7:
			val1 = space->dr[conv1];
			break;
		default:
			return 3; /*We don't know what that address means*/
		}
		val2 = op[1];
		result = val1 + val2;
		if ((result < val1) || (result < val2))
			space->cf = 1;
		else
			space->cf = 0;

		if (result == 0)
			space->zf = 0;
		else
			space->zf = 1;

		switch (ad3) {
		case 1:
			space->gp[conv3] = result;
			break;
		case 2:
			space->cp[conv3] = result;
			break;
		case 3:
			space->zf = result;
			break;
		case 4:
			space->cf = result;
			break;
		case 5:
			return 1; /*Can't set input register!*/
			break;
		case 6:
			space->ou = result;
#ifdef EOF
			printf("%i\n", space->ou);
#endif
			break;
		case 7:
			space->dr[conv3] = result;
			break;
		default:
			return 3; /*We don't know what that address means*/
		}
		break;
	case 4:
		ad2 = addrcheck(op[1]);
		ad3 = addrcheck(op[2]);

		conv2 = addrconvert(ad2, op[1]);
		conv3 = addrconvert(ad3, op[2]);

		val1 = op[0];

		switch (ad2) {
		case 1:
			val2 = space->gp[conv2];
			break;
		case 2:
			val2 = space->cp[conv2];
			break;
		case 3:
			val2 = space->zf;
			break;
		case 4:
			val2 = space->cf;
			break;
		case 5:
			val2 = space->in;
			break;
		case 6:
			return 1; /*Can't read output register!*/
			break;
		case 7:
			val2 = space->dr[conv2];
			break;
		default:
			return 3; /*We don't know what that address means*/
		}

		result = val1 + val2;
		if ((result < val1) || (result < val2))
			space->cf = 1;
		else
			space->cf = 0;

		if (result == 0)
			space->zf = 0;
		else
			space->zf = 1;

		switch (ad3) {
		case 1:
			space->gp[conv3] = result;
			break;
		case 2:
			space->cp[conv3] = result;
			break;
		case 3:
			space->zf = result;
			break;
		case 4:
			space->cf = result;
			break;
		case 5:
			return 1; /*Can't set input register!*/
			break;
		case 6:
			space->ou = result;
#ifdef EOF
			printf("%i\n", space->ou);
#endif
			break;
		case 7:
			space->dr[conv3] = result;
			break;
		default:
			return 3; /*We don't know what that address means*/
		}
		break;
	case 6:
		ad3 = addrcheck(op[2]);

		conv3 = addrconvert(ad3, op[2]);

		val1 = op[0];
		val2 = op[1];

		result = val1 + val2;
		if ((result < val1) || (result < val2))
			space->cf = 1;
		else
			space->zf = 0;

		if (result == 0)
			space->zf = 0;
		else
			space->zf = 1;

		switch (ad3) {
		case 1:
			space->gp[conv3] = result;
			break;
		case 2:
			space->cp[conv3] = result;
			break;
		case 3:
			space->zf = result;
			break;
		case 4:
			space->cf = result;
			break;
		case 5:
			return 1; /*Can't set input register!*/
			break;
		case 6:
			space->ou = result;
#ifdef EOF
			printf("%i\n", space->ou);
#endif
			break;
		case 7:
			space->dr[conv3] = result;
			break;
		default:
			return 3; /*We don't know what that address means*/
		}
		break;
	default:
		return 2; /*wrong usage*/
	}
	return 0;
}

uint hset(uint op[4], vmem *space)
{
	uint ad1;
	uint ad2;

	uint conv1;
	uint conv2;

	uint val;

	switch (op[3]) {
	case 0:
		ad1 = addrcheck(op[0]);
		ad2 = addrcheck(op[1]);

		conv1 = addrconvert(ad1, op[0]);
		conv2 = addrconvert(ad2, op[1]);
		switch (ad1) {
		case 1:;
			val = space->gp[conv1];
			break;
		case 2:;
			val = space->cp[conv1];
			break;
		case 3:;
			val = space->zf;
			break;
		case 4:;
			val = space->cf;
			break;
		case 5:;
			val = space->in;
			break;
		case 6:;
			return 1; /*Can't read output register!*/
			break;
		case 7:
			val = space->dr[conv1];
			break;
		default:
			return 1; /*ERROR, WE DON'T KNOW WHAT THAT ADDRESS
				     MEANS*/
		}

		switch (ad2) {
		case 1:;
			space->gp[conv2] = val;
			break;
		case 2:;
			space->cp[conv2] = val;
			break;
		case 3:;
			space->zf = val;
			break;
		case 4:;
			space->cf = val;
			break;
		case 5:;
			return 1; /*Can't set input register!*/
			break;
		case 6:;
			space->ou = val;
#ifdef EOF
			printf("%i\n", space->ou);
#endif
			break;
		case 7:
			space->dr[conv2] = val;
			break;
		default:
			return 1; /*ERROR, WE DON'T KNOW WHAT THAT ADDRESS
				     MEANS*/
		}
		break;
	case 4:
		val = op[0];
		ad2 = addrcheck(op[1]);

		conv2 = addrconvert(ad2, op[1]);
		switch (ad2) {
		case 1:;
			space->gp[conv2] = val;
			break;
		case 2:;
			space->cp[conv2] = val;
			break;
		case 3:;
			space->zf = val;
			break;
		case 4:;
			space->cf = val;
			break;
		case 5:;
			return 0; /*Can't set input register!*/
			break;
		case 6:;
			space->ou = val;
#ifdef EOF
			printf("%i\n", space->ou);
#endif
			break;
		case 7:
			space->dr[conv2] = val;
			break;
		default:
			return 1; /*ERROR, WE DON'T KNOW WHAT THAT ADDRESS
				     MEANS*/
		}
		break;
	default:
		return 1; /*ERROR, THE INSTRUCTION IS WRONG*/
	}
	return 2; /*SOMETHING WENT TERRIBLY WRONG*/
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
	if (program->co < (MEMSIZE*4)) {
		switch (program->m1.inst[program->co]) {
		case halt:
			program->hf = 1;
			program->co += 1;
#ifdef EOF
			printf("HALT\n");
#endif
			return 0;
		case nop:
			program->co += 1;
			return 0;
		case set:
			errno =
			    hset(program->m1.opnd[program->co], &program->m2);
			program->co += 1;
			return 0;
		case jmp:
			errno = hjump(program->m1.opnd[program->co],
				      &program->m2, &program->co);
			if (errno != 0) {
#ifdef EOF
				printf("ERROR\n");
#endif
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case jcz:
			if (program->m2.zf == 0)
				errno = hjump(program->m1.opnd[program->co],
					      &program->m2, &program->co);
			else {
				program->co += 1;
				errno = 0;
			}
			if (errno != 0) {
#ifdef EOF
				printf("ERROR\n");
#endif
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case add:
			errno =
			    hadd(program->m1.opnd[program->co], &program->m2);
			program->co += 1;
			if (errno != 0) {
#ifdef EOF
				printf("ERROR\n");
#endif
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case sub:
			errno = hsub(program->m1.opnd[program->co],
				     &program->m2, 1); /*Substract and save*/
			program->co += 1;
			if (errno != 0) {
#ifdef EOF
				printf("ERROR\n");
#endif
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case and:
			errno =
			    hbin(program->m1.opnd[program->co], &program->m2,
				 1); /*Substract but don't save*/
			program->co += 1;
			if (errno != 0) {
#ifdef EOF
				printf("ERROR\n");
#endif
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case or:
			errno =
			    hbin(program->m1.opnd[program->co], &program->m2,
				 2); /*Substract but don't save*/
			program->co += 1;
			if (errno != 0) {
#ifdef EOF
				printf("ERROR\n");
#endif
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case xor:
			errno =
			    hbin(program->m1.opnd[program->co], &program->m2,
				 3); /*Substract but don't save*/
			program->co += 1;
			if (errno != 0) {
#ifdef EOF
				printf("ERROR\n");
#endif
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case not:
			errno = hnot(program->m1.opnd[program->co],
				     &program->m2); /*Substract but don't save*/
			program->co += 1;
			if (errno != 0) {
#ifdef EOF
				printf("ERROR\n");
#endif
				return 2; /*EXECUTION ERROR*/
			}
			return 0;
		case cmp:
			errno =
			    hsub(program->m1.opnd[program->co], &program->m2,
				 0); /*Substract but don't save*/
			program->co += 1;
			if (errno != 0) {
#ifdef EOF
				printf("ERROR\n");
#endif
				return 2; /*EXECUTION ERROR*/
			}
		}
		return 0;
	}

#if defined(EOF)
	printf("UNIMPLEMENTED\n");
#endif
	return 1; /*UNIMPLEMENTED*/
}
