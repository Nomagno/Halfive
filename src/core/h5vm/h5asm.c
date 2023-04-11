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
#include <halfive/h5vm/h5vm.h>
#include <halfive/h5vm/h5asm.h>
#include <halfive/h5stdlib.h>

h5uint H5ASM_parse(char *linestr, H5VM_InstructionSet *inst, h5uint opnds[3]);
H5VM_InstructionSet _isinst(char *instr);
h5uint _isxupdigit(h5uchar inchar);

/*Define H5ASSEMBLY to enable assembling*/
#ifdef H5ASSEMBLY
#include <stdio.h>
int main(int argc, char **argv)
{
	FILE *codefile;
	FILE *drivefile;

	if (argc == 1)
		codefile = stdin;

	if (argc >= 2 && h5streq(argv[1], "-")) {
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
	int i				 = 0;
	while (fscanf(codefile, "%[^\n] ", arr) != EOF) {
		H5ASM_parse(arr, &code.inst[i], code.opnd[i]);
		i += 1;
	}

	H5VM_DefaultMemSetup mem = {0};
	H5VM_ReadWriteInfo rwinf = {0};
	H5VM_GeneralMemory prog	 = H5VM_init(&code, &mem);
	fread(mem.driv, 1, sizeof(mem.driv), drivefile);

	int return_code = 0;
	while (1) {
		if ((prog.code.opnd[prog.co][0] ==
				0xFFFD) || /*Preemtive/non-polling-but-ontime input, cheats
					 a bit by essentially peeking at the operands*/
			(prog.code.opnd[prog.co][1] == 0xFFFD)) {
			putchar('>');
			unsigned readin;
			scanf("%X", &readin);
			mem.in = readin;
		}

#ifdef H5ASM_VERBOSE
		h5uint prevco = prog.co;
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

		if (prog.hf) {
			printf("\n----\nHALT\n----\n");
			break;
		}
		switch (return_code) {
		case 0:
			break;
		case 1:
			printf("NONFATAL ERROR AT INST 0x%X: READ/WRITE UNMAPPED MEM\n", prog.co);
			break;
		case 2:
			printf("NONFATAL ERROR AT INST 0x%X: WRITE TO READ-ONLY MEM\n", prog.co);
			break;
		case 3:
			printf("NONFATAL ERROR AT INST 0x%X: WRONG ADDRESSING MODE\n", prog.co);
			break;
		case 4:
			printf("FATAL ERROR AT INST 0x%X: CALLSTACK OVERFLOW\n", prog.co);
			break;
		default:
			printf(
				"ERROR AT INST 0x%X: UNKNOWN ERROR %u\n", prog.co, return_code);
			break;
		}
		if (return_code >= 4) {
			break;
		}
	}

	fclose(codefile);
	fclose(drivefile);

	return return_code;
}
#endif

h5uint H5ASM_parse(char *linestr, H5VM_InstructionSet *inst, h5uint opnds[3])
{
	char *token = h5strtok(linestr,
		" "); /*linestr can not be const because it's modified by strtok*/
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
	if (h5streq(instr, "halt"))
		return Inst_halt;
	else if (h5streq(instr, "jmp"))
		return Inst_jmp;
	else if (h5streq(instr, "skpz"))
		return Inst_skpz;
	else if (h5streq(instr, "skmz"))
		return Inst_skmz;

	else if (h5streq(instr, "set"))
		return Inst_set;
	else if (h5streq(instr, "add"))
		return Inst_add;
	else if (h5streq(instr, "sub"))
		return Inst_sub;
	else if (h5streq(instr, "and"))
		return Inst_and;
	else if (h5streq(instr, "or"))
		return Inst_or;
	else if (h5streq(instr, "xor"))
		return Inst_xor;
	else if (h5streq(instr, "shift"))
		return Inst_shift;
	else if (h5streq(instr, "cmp"))
		return Inst_cmp;

	else if (h5streq(instr, "func"))
		return Inst_func;
	else if (h5streq(instr, "ret"))
		return Inst_ret;
	else if (h5streq(instr, "call"))
		return Inst_call;
	else if (h5streq(instr, "frame"))
		return Inst_frame;
	else
		return (H5VM_InstructionSet)16;
}

unsigned H5ASM_run(char **str, size_t stringnum, H5VM_GeneralMemory *mem, h5uint *addresses, h5uint *mappings, size_t mapping_size) {
	unsigned return_co;

	for (size_t j = 0; j < mapping_size; j++) {
		if ((mem->data[addresses[j]]) == NULL)
			return 10;
		*(mem->data[addresses[j]]) = mappings[j];
	}

	for (size_t i = 0; i < stringnum; i++) {
		H5ASM_parse(str[i], &(mem->code.inst[mem->co+i]), mem->code.opnd[mem->co+i]);
	}
	while(!mem->hf) {
		return_co = H5VM_execute(mem, &(H5VM_ReadWriteInfo){0});
		if (return_co <= 3) {}
		else return return_co; /*Fatal return code*/
	}
	mem->hf = 0; /*unterminate VM*/
	for (size_t j = 0; j < mapping_size; j++) {
		if ((mem->data[addresses[j]]) == NULL)
			return 10;
		mappings[j] = *(mem->data[addresses[j]]);
	}

	return 0;
}

/*
#include <stdio.h>

int main(void){
	H5VM_DefaultMemSetup defmem = {0};
	H5VM_GeneralMemory context = H5VM_init(&(H5VM_CodeMemory){0}, &defmem);
	h5uint intlist[] = {0xDE, 0xAD, 0xBE, 0xEF}; // Integers mapped to the VM
	h5uint adrlist[] = { 0, 1, 2, 3 }; // Addresses to be mapped in the VM
	char str[40] = {0};
	
	while(fgets(str, ELEMNUM(str), stdin) != NULL){
		ASM(adrlist, intlist, str); // Read a line of assembly, then execute it
		printf("%X,%X,%X,%X\n", intlist[0], intlist[1], intlist[2], intlist[3]);
		// the elements of intlist[] are now mapped to the VM addresses in adrlist[]
	}
}
*/
