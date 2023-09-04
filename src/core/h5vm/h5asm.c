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

h5uint H5ASM_parse(char *linestr, H5VM_ExecutableUnit *code);
H5VM_InstructionSet _getinst(char *instr);
h5uint _isxupdigit(h5uchar inchar);

/*Define H5ASSEMBLY to enable assembling*/
#ifdef H5ASSEMBLY
#include <halfive/code_setup.h>
#include <stdio.h>
int main(int argc, char **argv)
{
	FILE *codefile;

	if (argc == 1)
		codefile = stdin;

	if (argc >= 2 && h5streq(argv[1], "-")) {
		codefile = stdin;
	} else if (argc >= 2) {
		codefile = fopen(argv[1], "r");
	}

	/* if (argc < 3) {
		drivefile = fopen("/dev/null", "r");
	} else {
		drivefile = fopen(argv[2], "r");
	} */

	char arr[30];

	H5VM_VirtualMachine vm = {0};
	int i = 0;
	while (fscanf(codefile, "%[^\n] ", arr) != EOF) {
		H5ASM_parse(arr, &vm.code[i]);
		i += 1;
	}

	H5VM_ReadWriteInfo rwinf = {0};

	int return_code = 0;
	while (1) {
		if ((vm.code[vm.co].inst.operand_1 == 0xFFFD) || (vm.code[vm.co].inst.operand_2 == 0xFFFD))
		   /*Preemtive/non-polling-but-ontime input, cheats
             a bit by essentially peeking at the operands*/
		{
			putchar('>');
			unsigned readin;
			scanf("%X", &readin);
			vm.data[0xFFFD] = readin;
		}

		h5uint prevco = vm.co;
#ifdef H5ASM_VERBOSE
		maybe_printf(
			"BEFORE: PC %4u -- %u, %u, %u, %u, %u, %u, %4X (%2X), %4X (%2X)\n",
			vm.co, rwinf.was_err, rwinf.wrote_adrw, rwinf.read_adrw,
			rwinf.read_adrr, rwinf.write_zf, rwinf.write_cf, rwinf.adrw,
			vm.data[rwinf.adrw], rwinf.adrr, vm.data[rwinf.adrr]);
#endif
		return_code = H5VM_execute(&vm, &rwinf);

		if ((rwinf.adrw == 0xFFFC) && (rwinf.wrote_adrw)) {
			maybe_printf("OUTPUT AT ABS. PC 0x%u: 0x%X\n", prevco, vm.data[0xFFFC]);
		}

#ifdef H5ASM_VERBOSE
		maybe_printf(
			"AFTER:  PC %4u -- %u, %u, %u, %u, %u, %u, %4X (%2X), %4X (%2X)\n",
			vm.co, rwinf.was_err, rwinf.wrote_adrw, rwinf.read_adrw,
			rwinf.read_adrr, rwinf.write_zf, rwinf.write_cf, rwinf.adrw,
			vm.data[rwinf.adrw], rwinf.adrr, vm.data[rwinf.adrr]);
		putchar('\n');
#endif

		if (vm.hf) {
			maybe_printf("\n----\nHALT\n----\n");
			break;
		}
		switch (return_code) {
		case 0:
			break;
		case 1:
			maybe_printf("NONFATAL ERROR AT INST 0x%X: READ/WRITE UNMAPPED MEM\n", vm.co);
			break;
		case 2:
			maybe_printf("NONFATAL ERROR AT INST 0x%X: WRITE TO READ-ONLY MEM\n", vm.co);
			break;
		case 3:
			maybe_printf("NONFATAL ERROR AT INST 0x%X: WRONG ADDRESSING MODE\n", vm.co);
			break;
		case 4:
			maybe_printf("FATAL ERROR AT INST 0x%X: CALLSTACK UNDERFLOW\n", vm.co);
			break;
		default:
			maybe_printf(
				"ERROR AT INST 0x%X: UNKNOWN ERROR %u\n", vm.co, return_code);
			break;
		}
		if (return_code >= 4) {
			break;
		}
	}

	fclose(codefile);

	return return_code;
}
#endif

h5uint H5ASM_parse(char *linestr, H5VM_ExecutableUnit *code)
{
	char *token = h5strtok(linestr,
		" "); /*linestr can not be const because it's modified by strtok*/
	H5VM_InstructionSet myinst;
	int i = 0;
	while ((token != NULL) && (i < 2)) {
		if ((myinst = _getinst(token)) != 16) {
			code->inst.opcode = myinst;
		} else if (_isxupdigit(token[0])) {
			if (i == 0) {
				code->inst.operand_1 = (h5uint)h5strtoul(token, NULL, 16);
			} else if (i == 1) {
				code->inst.operand_2 = (h5uint)h5strtoul(token, NULL, 16);
			}
			i += 1;
		} else if (token[0] == '=') {
			token += 1;
			if (i == 0) {
				code->inst.optype_1 = H5VM_ModeLit;
				code->inst.operand_1 = (h5uint)h5strtoul(token, NULL, 16);
			} else if (i == 1) {
				code->inst.optype_2 = H5VM_ModeLit;
				code->inst.operand_2 = (h5uint)h5strtoul(token, NULL, 16);
			}
			i += 1;
		} else if (token[0] == '*') {
			token += 1;
			if (i == 0) {
				code->inst.optype_1 = H5VM_ModePtr;
				code->inst.operand_1 = (h5uint)h5strtoul(token, NULL, 16);
			} else if (i == 1) {
				code->inst.optype_2 = H5VM_ModePtr;
				code->inst.operand_2 = (h5uint)h5strtoul(token, NULL, 16);
			}
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

H5VM_InstructionSet _getinst(char *instr)
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
