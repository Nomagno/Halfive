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
#include "hwstring.c"
#define NULL (void *)0

uint _isxupdigit(uchar inchar)
{
	if ((inchar == 'A') || (inchar == 'a') || (inchar == 'B') ||
	    (inchar == 'b') || (inchar == 'C') || (inchar == 'c') ||
	    (inchar == 'D') || (inchar == 'd') || (inchar == 'E') ||
	    (inchar == 'e') || (inchar == 'F') || (inchar == 'f') ||
	    (inchar == '0') || (inchar == '1') || (inchar == '2') ||
	    (inchar == '3') || (inchar == '4') || (inchar == '5') ||
	    (inchar == '6') || (inchar == '7') || (inchar == '8') ||
	    (inchar == '9'))
		return 1;
	else
		return 0;
}

iset _isinst(char *instr)
{
	if (hwstrcmp(instr, "halt") == 0)
		return halt;
	else if (hwstrcmp(instr, "nop") == 0)
		return nop;
	else if (hwstrcmp(instr, "set") == 0)
		return set;
	else if (hwstrcmp(instr, "jmp") == 0)
		return jmp;
	else if (hwstrcmp(instr, "jcz") == 0)
		return jcz;
	else if (hwstrcmp(instr, "add") == 0)
		return add;
	else if (hwstrcmp(instr, "sub") == 0)
		return sub;
	else if (hwstrcmp(instr, "and") == 0)
		return and;
	else if (hwstrcmp(instr, "or") == 0)
		return or ;
	else if (hwstrcmp(instr, "xor") == 0)
		return xor;
	else if (hwstrcmp(instr, "not") == 0)
		return not ;
	else if (hwstrcmp(instr, "cmp") == 0)
		return cmp;
	else if (hwstrcmp(instr, "subs") == 0)
		return subs;
	else if (hwstrcmp(instr, "sube") == 0)
		return sube;
	else if (hwstrcmp(instr, "call") == 0)
		return call;
	else if (hwstrcmp(instr, "jcnz") == 0)
		return jcnz;
	else
		return 16;
}

uint asmparse(char *linestr, iset *inst, uint opnds[4])
{
	char *token = hwstrtok(linestr, " ");
	iset myinst;
	int i = 0;
	while ((token != NULL) && (i < 3)) {

		if ((myinst = _isinst(token)) != 16) {
			*inst = myinst;
		} else if (_isxupdigit(token[0])) {
			opnds[i] = (uint)hwstrtoul(token, NULL, 16);
			i += 1;
		} else if (token[0] == '[') {
			token += 1;
			opnds[i] = (uint)hwstrtoul(token, NULL, 16);

			opnds[3] = (opnds[3] | (1 << (2 - i)));

			i += 1;
		} else {
			return 2; /*CATASTROPHIC ERROR*/
		}
		token = hwstrtok(NULL, " ");
	}
	return 0;
}

#define HWASSEMBLY

#ifdef HWASSEMBLY

#include <stdio.h>
int main(int argc, char **argv)
{
	/*Requires code file and drive file as arguments*/
	if (argc < 2)
		return 1;
	char arr[30];
	FILE *codefile = fopen(argv[1], "r");
	FILE *drivefile = fopen(argv[2], "r");

	xmem code = {0};
	int i = 0;
	while (fscanf(codefile, "%[^\n] ", arr) != EOF) {
		asmparse(arr, &code.inst[i], code.opnd[i]);
		i += 1;
	}
	mem prog = fxmem(code);
	fread(prog.m2.dr, 1, sizeof(prog.m2.dr), drivefile);

	int errno = 0;
	while ((!prog.hf) && (!errno)) {
		if ((prog.m1.opnd[prog.m2.co][0] == 0xFFFD) ||
		    (prog.m1.opnd[prog.m2.co][1] == 0xFFFD)) {
			putchar('>');
			prog.m2.in = getchar();
			putchar('\n');
		}
		errno = execnext(&prog);
	}
	fclose(codefile);
	fclose(drivefile);
	return errno;
}
#endif
