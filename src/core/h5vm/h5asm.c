/*
Copyright Nomagno 2021, 2022, 2023

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
#include <halfive/h5stdlib.h>
#include <halfive/h5vm/h5asm.h>

h5uint H5ASM_parse(char *linestr, H5VM_InstructionSet *inst, h5uint opnds[3]);
H5VM_InstructionSet _isinst(char *instr);
h5uint _isxupdigit(h5uchar inchar);
H5VM_InstructionSet _isinst(char *instr);

/*Define H5ASSEMBLY to enable assembling*/
#ifdef H5ASSEMBLY
#include <stdio.h>
int main(int argc, char **argv)
{

    FILE *codefile;
    FILE *drivefile;

    if (argc == 1)
	codefile = stdin;

    if (argc >= 2 && h5strcmp(argv[1], "-") == 0) {
	codefile = stdin;
    } else if (argc >= 2) {
	codefile = fopen(argv[1], "r");
    }

    if (argc < 3) {
	drivefile = fopen("/dev/null", "r");
    } else {
	drivefile = fopen(argv[2], "r");
    }
    char arr[30];

    H5VM_CodeMemory code = {(H5VM_InstructionSet)0};
    int i = 0;
    while (fscanf(codefile, "%[^\n] ", arr) != EOF) {
	H5ASM_Parse(arr, &code.inst[i], code.opnd[i]);
	i += 1;
    }

    H5VM_DefaultMemSetup mem = {0};
    H5VM_ReadWriteInfo rwinf = {0};
    H5VM_GeneralMemory prog = H5VM_init(&code, &mem);
    fread(mem.driv, 1, sizeof(mem.driv), drivefile);

    int return_code = 0;
    h5uint prevcode = 0;
    while ((!prog.hf) && (!return_code)) {
	prevcode = prog.co;
	if ((prog.code.opnd[prog.co][0] ==
	     0xFFFD) || /*Preemtive/non-polling-but-ontime input, cheats
			  a bit by essentially peeking at the operands*/
	    (prog.code.opnd[prog.co][1] == 0xFFFD)) {
	    putchar('>');
	    unsigned readin;
	    scanf("%X", &readin);
	    mem.in = readin;
	}

	h5uint prevco = prog.co;

#ifdef H5ASM_VERBOSE
	printf(
	    "BEFORE: PC %4u -- %u, %u, %u, %u, %u, %u, %4X (%2X), %4X (%2X)\n",
	    prog.co, rwinf.was_err, rwinf.wrote_adrw, rwinf.read_adrw,
	    rwinf.read_adrr, rwinf.write_zf, rwinf.write_cf, rwinf.adrw,
	    *prog.data[rwinf.adrw], rwinf.adrr, *prog.data[rwinf.adrr]);
#endif
	return_code = H5VM_execute(&prog, &rwinf);

	if ((rwinf.adrw == 0xFFFC) && (rwinf.wrote_adrw)) {
#ifdef H5ASM_VERBOSE
	    printf("OUTPUT at PC %4u: %X\n", prevco, mem.ou);
#else
	    printf("OU: %X\n", mem.ou);
#endif
	}

#ifdef H5ASM_VERBOSE
	printf(
	    "AFTER:  PC %4u -- %u, %u, %u, %u, %u, %u, %4X (%2X), %4X (%2X)\n",
	    prog.co, rwinf.was_err, rwinf.wrote_adrw, rwinf.read_adrw,
	    rwinf.read_adrr, rwinf.write_zf, rwinf.write_cf, rwinf.adrw,
	    *prog.data[rwinf.adrw], rwinf.adrr, *prog.data[rwinf.adrr]);
	putchar('\n');
#endif
    }

    fclose(codefile);
    fclose(drivefile);
    if (prog.hf)
	printf("HALT\n");
    else {
	switch (return_code) {
	case 1:
	    printf("ERROR AT INST 0x%X: READ/WRITE UNMAPPED MEM\n", prog.co);
	    break;
	case 2:
	    printf("ERROR AT INST 0x%X: WRITE TO READ-ONLY MEM\n", prog.co);
	    break;
	case 3:
	    printf("ERROR AT INST 0x%X: WRONG ADDRESSING MODE\n", prog.co);
	    break;
	default:
	    printf("ERROR AT INST 0x%X: UNKNOWN ERROR %u\n", prog.co,
		   return_code);
	    break;
	}
    }
    return return_code;
}
#endif

h5uint H5ASM_parse(char *linestr, H5VM_InstructionSet *inst, h5uint opnds[3])
{
    char *token = h5strtok(
	linestr, " "); /*linestr should not be const strictly speaking,
			 but strtok behaves properly in this situation.*/
    H5VM_InstructionSet myinst;
    int i = 0;
    while ((token != NULL) && (i < 2)) {
	if ((myinst = _isinst(token)) != 16) {
	    *inst = myinst;
	} else if (_isxupdigit(token[0])) {
	    opnds[i] = (h5uint)h5strtoul(token, NULL, 16);
	    i += 1;
	} else if (token[0] == '=') {
	    token += 1;
	    opnds[i] = (h5uint)h5strtoul(token, NULL, 16);

	    opnds[2] = (opnds[2] | (1 << (1 - i)));

	    i += 1;
	} else if (token[0] == '*') {
	    token += 1;
	    opnds[i] = (h5uint)h5strtoul(token, NULL, 16);
	    opnds[2] = (opnds[2] | 8);
	    opnds[2] = (opnds[2] | (1 << (1 - i)));

	    i += 1;
	} else {
	    return 2; /*CATASTROPHIC ERROR*/
	}
	token = h5strtok(NULL, " ");
    }
    return 0;
}

h5uint _isxupdigit(h5uchar inchar)
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

H5VM_InstructionSet _isinst(char *instr)
{
    if (h5strcmp(instr, "halt") == 0)
	return Inst_halt;
    else if (h5strcmp(instr, "nop") == 0)
	return Inst_nop;
    else if (h5strcmp(instr, "jmp") == 0)
	return Inst_jmp;
    else if (h5strcmp(instr, "skpz") == 0)
	return Inst_skpz;
    else if (h5strcmp(instr, "skmz") == 0)
	return Inst_skmz;

    else if (h5strcmp(instr, "set") == 0)
	return Inst_set;
    else if (h5strcmp(instr, "add") == 0)
	return Inst_add;
    else if (h5strcmp(instr, "sub") == 0)
	return Inst_sub;
    else if (h5strcmp(instr, "and") == 0)
	return Inst_and;
    else if (h5strcmp(instr, "or") == 0)
	return Inst_or;
    else if (h5strcmp(instr, "xor") == 0)
	return Inst_xor;
    else if (h5strcmp(instr, "shift") == 0)
	return Inst_shift;
    else if (h5strcmp(instr, "cmp") == 0)
	return Inst_cmp;

    else if (h5strcmp(instr, "func") == 0)
	return Inst_func;
    else if (h5strcmp(instr, "ret") == 0)
	return Inst_ret;
    else if (h5strcmp(instr, "call") == 0)
	return Inst_call;
    else
	return (H5VM_InstructionSet)16;
}
