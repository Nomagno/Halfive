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

h5uint H5ASM_parse(char *linestr, h5ulong *code);
H5VM_InstructionSet _getinst(char *instr);
h5uchar _getmode(char c);
H5VM_Registers _getreg(char c1, char c2);
h5uint _ishexdigit(h5uchar inchar);
char *_instToString(H5VM_InstructionSet ininst);
char *_modeToString(h5uchar inmode);
char *_regToString(H5VM_Registers inreg);

#define INPUT_REGISTER_ADDRESS 254
#define OUTPUT_REGISTER_ADDRESS 255
#define _PC program.registers[2]
#define CODE program.code

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

	H5VM_VirtualMachine program = {0};
	int i = 0;
	while (fscanf(codefile, "%[^\n] ", arr) != EOF) {
		H5ASM_parse(arr, &program.code[i]);
		i += 1;
	}

	H5VM_ReadWriteInfo rwinf = {0};

	int return_code = 0;
	while (1) {
		H5VM_Instruction currinst = INST(CODE[_PC]);
		h5uint prevco = _PC;
		maybe_printf("PC: 0x%4X|O: %4s|M: %4s|R: %4s (%2X)|M: %4X (%4s)\n", prevco, _instToString(currinst.opcode), _modeToString(currinst.mode), _regToString(currinst.reg), currinst.reg, currinst.operand, _regToString(currinst.operand));
		if (currinst.operand == INPUT_REGISTER_ADDRESS)
		   /*Preemtive/non-polling-but-ontime input, cheats
             a bit by essentially peeking at the operands*/
		{
			maybe_printf("INPUT AT ABS. PC 0x%X? ", prevco);
			unsigned readin;
			scanf("%X", &readin);
			program.data[INPUT_REGISTER_ADDRESS] = readin;
		}
		return_code = H5VM_execute(&program, &rwinf);

		if (currinst.operand == OUTPUT_REGISTER_ADDRESS && currinst.mode == 0) {
			maybe_printf("OUTPUT AT ABS. PC 0x%X: 0x%X\n", prevco, program.data[OUTPUT_REGISTER_ADDRESS]);
		}

		if (rwinf.adrw == OUTPUT_REGISTER_ADDRESS) {
			maybe_printf("OUTPUT AT ABS. PC 0x%X: 0x%X\n", prevco, program.data[OUTPUT_REGISTER_ADDRESS]);
		}

		if (program.hf) {
			maybe_printf("\n----\nHALT\n----\n");
			break;
		}
		switch (return_code) {
		case 0:
			break;
		case 1:
			maybe_printf("ERROR AT INST 0x%X: WRONG ADDRESSING MODE\n", _PC);
			break;
		case 2:
			maybe_printf("ERROR AT INST 0x%X: UNIMPLEMENTED OPERATION\n", _PC);
			break;
		default:
			maybe_printf(
				"ERROR AT INST 0x%X: UNKNOWN ERROR %u\n", _PC, return_code);
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

/*
LOADd 03 03
LOADa x_ 0003
LOADi _y 003
LOADp __ 3F0F

*/

h5uint H5ASM_parse(char *linestr, h5ulong *code)
{
	size_t length = h5strlen(linestr);
	if (length < 4) return 1;

	H5VM_Instruction currinst = INST(*code);
	char inststr[5] = { linestr[0], linestr[1], linestr[2], linestr[3], '\0'};
	currinst.opcode = _getinst(inststr);
	if (length == 4) {
		currinst.mode = 0;
		currinst.reg = 0;
		currinst.operand = 0;
	} else if (length == 5) {
		currinst.mode = _getmode(linestr[4]);
		currinst.reg = 0;
		currinst.operand = 0;
	} else if (length == 8) {
		currinst.mode = _getmode(linestr[4]);
		/*linstr[5] is whitespace*/
		currinst.reg = _getreg(linestr[6], linestr[7]);
	} else if (length == 11) {
		currinst.mode = _getmode(linestr[4]);
		/*linstr[5] is whitespace*/
		currinst.reg = _getreg(linestr[6], linestr[7]);
		/*linstr[8] is whitespace*/
		currinst.operand = _getreg(linestr[9], linestr[10]);
	} else if (length == 13) {
		currinst.mode = _getmode(linestr[4]);
		/*linstr[5] is whitespace*/
		currinst.reg = _getreg(linestr[6], linestr[7]);
		/*linstr[8] is whitespace*/	
		char operandstr[5] = { linestr[9], linestr[10], linestr[11], linestr[12], '\0'};
		currinst.operand = (h5uint)h5strtoul(operandstr, NULL, 16);
	} else {
		return 1;
	}

	*code = MACHINE_CODE(currinst);
	return 0;
}

h5uint _ishexdigit(h5uchar inchar)
{
	if ((inchar == 'A') || (inchar == 'B') || (inchar == 'C') ||
		(inchar == 'D') || (inchar == 'E') || (inchar == 'F') ||
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
	if (h5streq(instr, "HALT"))
		return Inst_halt;
	else if (h5streq(instr, "JUMP"))
		return Inst_jump;
	else if (h5streq(instr, "SKPZ"))
		return Inst_skpz;
	else if (h5streq(instr, "SKMZ"))
		return Inst_skmz;

	else if (h5streq(instr, "MLOD"))
		return Inst_mlod;
	else if (h5streq(instr, "MSTR"))
		return Inst_mstr;
	else if (h5streq(instr, "VLOD"))
		return Inst_vlod;
	else if (h5streq(instr, "VSTR"))
		return Inst_vstr;

	else if (h5streq(instr, "BADD"))
		return Inst_badd;
	else if (h5streq(instr, "BSUB"))
		return Inst_bsub;
	else if (h5streq(instr, "BAND"))
		return Inst_band;
	else if (h5streq(instr, "INOR"))
		return Inst_inor;
	else if (h5streq(instr, "EXOR"))
		return Inst_exor;
	else if (h5streq(instr, "SHFT"))
		return Inst_shft;
	else if (h5streq(instr, "COMP"))
		return Inst_comp;

	else if (h5streq(instr, "STKI"))
		return Inst_stki;
	else if (h5streq(instr, "STKD"))
		return Inst_stkd;
	else if (h5streq(instr, "FUNC"))
		return Inst_func;
	else if (h5streq(instr, "BACK"))
		return Inst_back;
	else if (h5streq(instr, "CALL"))
		return Inst_call;
	else
		return (H5VM_InstructionSet)63;
}

char *_instToString(H5VM_InstructionSet ininst)
{
	switch(ininst){
	case Inst_halt:
		return "HALT";
	case Inst_jump:
		return "JUMP";
	case Inst_skpz:
		return "SKPZ";
	case Inst_skmz:
		return "SKMZ";
	case Inst_stki:
		return "STKI";
	case Inst_stkd:
		return "STKD";
	case Inst_func:
		return "FUNC";
	case Inst_back:
		return "BACK";
	case Inst_call:
		return "CALL";
	case Inst_mlod:
		return "MLOD";
	case Inst_mstr:
		return "MSTR";
	case Inst_vlod:
		return "VLOD";
	case Inst_vstr:
		return "VSTR";
	case Inst_badd:
		return "BADD";
	case Inst_bsub:
		return "BSUB";
	case Inst_band:
		return "BAND";
	case Inst_inor:
		return "INOR";
	case Inst_exor:
		return "EXOR";
	case Inst_shft:
		return "SHFT";
	case Inst_comp:
		return "COMP";
	default:
		return "UNKNOWN";
	}
}

h5uchar _getmode(char c) {
	switch(c){
	case ' ':
	case 'm':
		return 0;
	case 'i':
		return 1;
	case 'p':
		return 2;
	case 's':
		return 3;
	default:
		return 0;
	}
}

char *_modeToString(h5uchar inmode) {
	switch(inmode){
	case 0:
		return "m";
	case 1:
		return "i";
	case 2:
		return "p";
	case 3:
		return "s";
	default:
		return "UNKOWN";
	}
}

H5VM_Registers _getreg(char c1, char c2){
	if (_ishexdigit(c1) && _ishexdigit(c2)){
		char digitstr[3] = {c1, c2, '\0'};
		return (h5uint)h5strtoul(digitstr, NULL, 16);		
	} else {
		char c = (c1 == '_') ? c2 : c1;
		switch(c){
		case '_':
			/*Case where there is simply no register*/
			return (H5VM_Registers)0;
		case 'F':
			return H5VM_Register_Flag;
		case 'P':
			return H5VM_Register_ProgramCounter;
		case 'S':
			return H5VM_Register_StackPointer;
		case 'R':
			return H5VM_Register_StackSize;

		case 'a':
			return H5VM_Register_A;
		case 'b':
			return H5VM_Register_B;
		case 'c':
			return H5VM_Register_C;
		case 'd':
			return H5VM_Register_D;

		case 'w':
			return H5VM_Register_W;
		case 'x':
			return H5VM_Register_X;
		case 'y':
			return H5VM_Register_Y;
		case 'z':
			return H5VM_Register_Z;
		default:
			return (H5VM_Registers)255;
		}
	}
}

char *_regToString(H5VM_Registers inreg){
	switch(inreg){
	case H5VM_Register_Flag:
		return "F";
	case H5VM_Register_ProgramCounter:
		return "P";
	case H5VM_Register_StackPointer:
		return "S";
	case H5VM_Register_StackSize:
		return "R";

	case H5VM_Register_A:
		return "a";
	case H5VM_Register_B:
		return "b";
	case H5VM_Register_C:
		return "c";
	case H5VM_Register_D:
		return "w";

	case H5VM_Register_W:
		return "w";
	case H5VM_Register_X:
		return "x";
	case H5VM_Register_Y:
		return "y";
	case H5VM_Register_Z:
		return "z";
	default:
		return "UNKNOWN";
	}
}
