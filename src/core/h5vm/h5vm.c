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

#define _PC program->registers[2]
#define CODE program->code

#define IMM currinst.operand
#define ADR program->data[currinst.operand]
#define PTR program->data[program->data[currinst.operand]]

unsigned H5VM_opcodeHalt(H5VM_VirtualMachine *program) {
	program->hf = 1;
	return 0;
}

unsigned H5VM_opcodeJump(H5VM_VirtualMachine *program) {
	H5VM_Instruction currinst = INST(CODE[_PC]);
	switch(currinst.mode) {
	case 0:
		_PC = ADR;
		break;
	case 1:
		_PC = IMM;
		break;
	case 2:
		_PC = PTR;
		break;
	case 3:
		return 1;
		break;
	}
	return 0;
}

unsigned H5VM_opcodeSkpz(H5VM_VirtualMachine *program) {
	H5VM_Instruction currinst = INST(CODE[_PC]);
	if (program->registers[0] == 0){
		switch(currinst.mode) {
		case 0:
		case 1:
			_PC += IMM + 1;
			break;
		case 2:
			return 1;
			break;
		case 3:
			return 1;
			break;
		}
	}
	return 0;
}

unsigned H5VM_opcodeSkmz(H5VM_VirtualMachine *program) {
	H5VM_Instruction currinst = INST(CODE[_PC]);
	if (program->registers[0] == 0){
		switch(currinst.mode) {
		case 0:
		case 1:
			_PC -= IMM + 1;
			break;
		case 2:
			return 1;
			break;
		case 3:
			return 1;
			break;
		}
	}
	return 0;
}

unsigned H5VM_opcodeMlod(H5VM_VirtualMachine *program) {
	H5VM_Instruction currinst = INST(CODE[_PC]);
	switch(currinst.mode) {
	case 0:
		program->registers[currinst.reg] = ADR;
		_PC += 1;
		break;
	case 1:
		program->registers[currinst.reg] = IMM;
		_PC += 1;
		break;
	case 2:
		program->registers[currinst.reg] = PTR;
		_PC += 1;
		break;
	case 3:
		program->registers[currinst.reg] = program->registers[currinst.operand];
		break;
	}
	return 0;
}

unsigned H5VM_opcodeVlod(H5VM_VirtualMachine *program) {
	H5VM_Instruction currinst = INST(CODE[_PC]);
	switch(currinst.mode) {
	case 0:
		program->data[program->data[0]+1+ADR] = program->registers[currinst.reg];
		_PC += 1;
		break;
	case 1:
		program->data[program->data[0]+1+IMM] = program->registers[currinst.reg];
		_PC += 1;
		break;
	case 2:
		return 1;
		break;
	case 3:
		return 1;
		break;
	}
	return 0;
}

unsigned H5VM_opcodeMstr(H5VM_VirtualMachine *program) {
	H5VM_Instruction currinst = INST(CODE[_PC]);
	switch(currinst.mode) {
	case 0:
		 ADR = program->registers[currinst.reg];
		_PC += 1;
		break;
	case 1:
		return 1;
		break;
	case 2:
		PTR = program->registers[currinst.reg];
		_PC += 1;
		break;
	case 3:
		return 1;
		break;
	}
	return 0;
}

unsigned H5VM_opcodeVstr(H5VM_VirtualMachine *program) {
	H5VM_Instruction currinst = INST(CODE[_PC]);
	switch(currinst.mode) {
	case 0:
		program->registers[currinst.reg] = program->data[program->data[0]+1+ADR];
		_PC += 1;
		break;
	case 1:
		program->registers[currinst.reg] = program->data[program->data[0]+1+IMM];
		_PC += 1;
		break;
	case 2:
		return 1;
		break;
	case 3:
		return 1;
		break;
	}
	return 0;
}

unsigned H5VM_opcodeFset(H5VM_VirtualMachine *program) {
	H5VM_Instruction currinst = INST(CODE[_PC]);
	program->registers[H5VM_Register_Flag] |= 1 << currinst.operand;
}

unsigned H5VM_opcodeFdel(H5VM_VirtualMachine *program) {
	H5VM_Instruction currinst = INST(CODE[_PC]);
	program->registers[H5VM_Register_Flag] &= ~(1 << currinst.operand);
}

unsigned H5VM_opcodeFflp(H5VM_VirtualMachine *program) {
	H5VM_Instruction currinst = INST(CODE[_PC]);
	program->registers[H5VM_Register_Flag] ^= 1 << currinst.operand;
}

/*Callstack:
Stack pointer register: Current stack frame start address
Stack size register:  Size of each stack frame
First address of each stack frame: return program counter
Rest of addresses of each stack frame: free use
Address 0: Zero address, special behaviour
Addresses 1-253: Jumptable
Addresses 1-15: Reserved addresses of jumptable
Addresses 16-253: Free use addresses of jumptable
Address 254: Output address, write to provide a number
Address 255: Input address, write to request a number
*/

unsigned H5VM_opcodeFunc(H5VM_VirtualMachine *program) {
	H5VM_Instruction currinst = INST(CODE[_PC]);
	switch(currinst.mode) {
	case 0:
	case 1:
		program->data[1+IMM] = _PC+1;
		for (unsigned i = _PC; i <= _PC+0x400u; i++) {
			if (i == _PC+0x400u) {
				return 1;
			} else if (INST(CODE[i]).opcode == Inst_back) {
				_PC = i+1;
				break;
			}
		}
		break;
	case 2:
		return 1;
		break;
	case 3:
		program->data[1+IMM] = _PC+1;
		program->data[1+ADR] = program->registers[currinst.reg];
		_PC += 1;
		break;
	}
	return 0;
}
unsigned H5VM_opcodeStki(H5VM_VirtualMachine *program) {
	program->registers[H5VM_Register_StackPointer] += program->registers[H5VM_Register_StackSize];
}

unsigned H5VM_opcodeStkd(H5VM_VirtualMachine *program) {
	program->registers[H5VM_Register_StackPointer] -= program->registers[H5VM_Register_StackSize];
}

unsigned H5VM_opcodeCall(H5VM_VirtualMachine *program) {
	H5VM_Instruction currinst = INST(CODE[_PC]);
	switch(currinst.mode) {
	case 0:
	case 1:
		program->data[program->registers[H5VM_Register_StackPointer]] = _PC + 1;
		_PC = program->data[1+IMM];
		break;
	case 2:
		return 1;
		break;
	case 3:
		program->data[program->registers[H5VM_Register_StackPointer]] = _PC + 1;
		_PC = ADR;
		break;
	}
	return 0;
}

unsigned H5VM_opcodeBack(H5VM_VirtualMachine *program) {
	_PC = program->data[program->registers[H5VM_Register_StackPointer]];
	return 0;
}


unsigned H5VM_mixedOperation(char op, h5uint a, h5uint b) {
	switch(op){
	case '+':
		return a + b;
	case '-':
		return a - b;
	case 'a':
		return a & b;
	case 'o':
		return a | b;
	case 'x':
		return a ^ b;
	case 's':
		if (b < 0x10)
			return a << b;
		else if (b < 0x20)
			return a << (b - 0x10);
		else
			return a;
	case '=':
		return a;
	}
	return 0;
}

unsigned H5VM_opcodeArithmetic(H5VM_VirtualMachine *program, char op){
	H5VM_Instruction currinst = INST(CODE[_PC]);
	h5uint operand1, operand2;
	switch(currinst.mode) {
	case 0:
		operand1 = program->registers[currinst.reg];
		operand2 = ADR;
		program->registers[currinst.reg] = H5VM_mixedOperation(op, operand1, operand2);
		break;
	case 1:
		operand1 = program->registers[currinst.reg];
		operand2 = IMM;
		program->registers[currinst.reg] = H5VM_mixedOperation(op, operand1, operand2);
		break;
	case 2:
		operand1 = program->registers[currinst.reg];
		operand2 = PTR;
		program->registers[currinst.reg] = H5VM_mixedOperation(op, operand1, operand2);
		break;
	case 3:
		operand1 = program->registers[currinst.reg];
		operand2 = program->registers[currinst.operand];
		program->registers[currinst.reg] = H5VM_mixedOperation(op, operand1, operand2);
		break;
	 }
		if (program->registers[currinst.reg] == 0) {
			program->registers[H5VM_Register_Flag] &= 0b1111111111111110;
		} else {
			program->registers[H5VM_Register_Flag] |= 0b0000000000000001;
		}
		if (op == '=') {
			if ((operand1 - operand2) == 0) {
				program->registers[H5VM_Register_Flag] &= 0b1111111111111110;
			} else {
				program->registers[H5VM_Register_Flag] |= 0b0000000000000001;
			}
		}
		if (op == '+' && (program->registers[currinst.reg] < operand2)) {
			program->registers[H5VM_Register_Flag] |= 0b0000000000000010;
		} else if ((op == '-' || op == '=') && ((h5uint)((h5uint)operand1 - (h5uint)operand2) > operand1)) {
			program->registers[H5VM_Register_Flag] |= 0b0000000000000010;
		} else {
			program->registers[H5VM_Register_Flag] &= 0b1111111111111101;
		}
		_PC += 1;
	return 0;

}

unsigned H5VM_execute(H5VM_VirtualMachine *program, H5VM_ReadWriteInfo *rwinf) {
	H5VM_Instruction currinst = INST(CODE[_PC]);
	rwinf->adrr = 0; /*Placeholder*/
	rwinf->adrw = 0; /*Placeholder*/

	unsigned retval;
	switch (currinst.opcode) {
	case Inst_halt:
		retval = H5VM_opcodeHalt(program);
		break;
	case Inst_jump:
		retval = H5VM_opcodeJump(program);
		break;
	case Inst_skpz:
		retval = H5VM_opcodeSkpz(program);
		break;
	case Inst_skmz:
		retval = H5VM_opcodeSkmz(program);
		break;
	case Inst_stki:
		retval = H5VM_opcodeStki(program);
		break;
	case Inst_stkd:
		retval = H5VM_opcodeStkd(program);
		break;
	case Inst_func:
		retval = H5VM_opcodeFunc(program);
		break;
	case Inst_back:
		retval = H5VM_opcodeBack(program);
		break;
	case Inst_call:
		retval = H5VM_opcodeCall(program);
		break;
	case Inst_mlod:
		retval = H5VM_opcodeMlod(program);
		break;
	case Inst_mstr:
		retval = H5VM_opcodeMstr(program);
		break;
	case Inst_vlod:
		retval = H5VM_opcodeVlod(program);
		break;
	case Inst_vstr:
		retval = H5VM_opcodeVstr(program);
		break;
	case Inst_fset:
		retval = H5VM_opcodeFset(program);
		break;
	case Inst_fdel:
		retval = H5VM_opcodeFdel(program);
		break;
	case Inst_fflp:
		retval = H5VM_opcodeFflp(program);
		break;
	case Inst_badd:
		retval = H5VM_opcodeArithmetic(program, '+');
		break;
	case Inst_bsub:
		retval = H5VM_opcodeArithmetic(program, '-');
		break;
	case Inst_band:
		retval = H5VM_opcodeArithmetic(program, 'a');
		break;
	case Inst_inor:
		retval = H5VM_opcodeArithmetic(program, 'o');
		break;
	case Inst_exor:
		retval = H5VM_opcodeArithmetic(program, 'x');
		break;
	case Inst_shft:
		retval = H5VM_opcodeArithmetic(program, 's');
		break;
	case Inst_comp:
		retval = H5VM_opcodeArithmetic(program, '=');
		break;
	default:
		/*Unhandled instruction!*/
		retval = 2;
		break;
	}
	return retval;
}
