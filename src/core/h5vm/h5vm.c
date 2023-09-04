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

#define UNEVENTFUL (H5VM_ReadWriteInfo){ 0 };

/*Yes, I'm seriously doing this, zero-indexing a two-element array is ugly*/
#define OPERAND_ONE 0
#define OPERAND_TWO 1

h5uint operation(char op, h5uint a, h5uint b) {
	switch (op) {
	case '+':
		return a + b;
		break;
	case '?':
	case '-':
		return a - b;
		break;
	case 'a':
		return a & b;
		break;
	case 'o':
		return a | b;
		break;
	case 'x':
		return a ^ b;
		break;
	case 's':
		if (b < 16)
			return a << b;
		else
			return a >> (b - 16);
		break;
	default:
		return 0;
		break;
	}
}

/*readReadAddress*/
h5uint read_RA(H5VM_VirtualMachine *vm, H5VM_ReadWriteInfo *rwinf) {
	H5VM_Instruction inst = vm->code[vm->co].inst;
	switch (inst.optype_2) {
	case H5VM_ModeLit:
		rwinf->read_adrr = 0;
		return inst.operand_2;
		break;
	case H5VM_ModeAdr:
		rwinf->read_adrr = 1;
		rwinf->adrr = inst.operand_2;
		return vm->data[inst.operand_2];
		break;
	case H5VM_ModePtr:
		rwinf->read_adrr = 1;
		h5uint address = vm->data[inst.operand_2];
		rwinf->adrr = address;
		return vm->data[address];
		break;
	default:
		return 0;
		break;
	}
}

/*readWriteAddress*/
h5uint read_WA(H5VM_VirtualMachine *vm, H5VM_ReadWriteInfo *rwinf, _Bool cmp_mode) {
	H5VM_Instruction inst = vm->code[vm->co].inst;
	switch (inst.optype_1) {
	case H5VM_ModeLit:
		if (cmp_mode) {
			rwinf->read_adrw = 0;
			return inst.operand_1;
		} else {
			/*Wrong addressing mode*/
			rwinf->errcode = 3;
			rwinf->read_adrw = 0;
			return inst.operand_1;
		}
		break;
	case H5VM_ModeAdr:
		rwinf->read_adrw = 1;
		rwinf->adrw = inst.operand_1;
		return vm->data[inst.operand_1];
		break;
	case H5VM_ModePtr:
		rwinf->read_adrw = 1;
		h5uint address = vm->data[inst.operand_1];
		rwinf->adrw = address;
		return vm->data[address];
		break;
	default:
		return 0;
		break;
	}
}

/*writeWriteAddress*/
void write_WA(H5VM_VirtualMachine *vm, H5VM_ReadWriteInfo *rwinf, h5uint val) {
	H5VM_Instruction inst = vm->code[vm->co].inst;
	switch (inst.optype_1) {
	case H5VM_ModeLit:
		/*Wrong addressing mode*/
		rwinf->errcode = 3;
		rwinf->wrote_adrw = 1; /*Should be 0? IDK*/
		return;
		break;
	case H5VM_ModeAdr:
		rwinf->wrote_adrw = 1;
		rwinf->adrw = inst.operand_1;
		vm->data[inst.operand_1] = val;
		break;
	case H5VM_ModePtr:
		rwinf->wrote_adrw = 1;
		h5uint address = vm->data[inst.operand_1];
		rwinf->adrw = address;
		vm->data[address] = val;
		return;
		break;
	}
}

h5uint getFrameData(H5VM_VirtualMachine *vm, H5VM_ReadWriteInfo *rwinf) {
	/*The type must be literal, so no need to check*/
	H5VM_Instruction inst = vm->code[vm->co].inst;
	h5uint stack_start = inst.operand_2 & 0xFF00;
	rwinf->read_adrr = 1;

	h5uint stackPtr = vm->data[stack_start];
	rwinf->adrr = stackPtr;
	return vm->data[stackPtr];
}

void setFunction(H5VM_CrossProcessProgramCounter *jumptable, unsigned n, H5VM_Header header, unsigned id, unsigned pc) {
	for (unsigned i = 0; i < n; i++) {
		if (jumptable[i].isActive == 0)
		{
			jumptable[i].isActive = 1;
			jumptable[i].char1 = header.char1;
			jumptable[i].char1 = header.char2;
			jumptable[i].id = id;
			jumptable[i].pc = pc;
			break;
		}
	}
}

h5uint getFunction(H5VM_CrossProcessProgramCounter *jumptable, unsigned n, H5VM_Header header, unsigned id) {
	for (unsigned i = 0; i < n; i++) {
		if (jumptable[i].char1 == header.char1 &&
		    jumptable[i].char1 == header.char2 &&
		    jumptable[i].id == id && jumptable[i].isActive == 1)
		{
			return i;
		}
	}
	return n-1;
}

void doCall(H5VM_VirtualMachine *vm, H5VM_ReadWriteInfo *rwinf) {
	/*The type must be literal, so no need to check*/
	H5VM_Header header = vm->code[vm->co].header;
	H5VM_Instruction inst = vm->code[vm->co].inst;
	h5uint stack_start = inst.operand_2 & 0xFF00;
	h5uint frame_size = (inst.operand_2 & 0x00FF) + 1;
	rwinf->read_adrr = 1;

	h5uint stackPtr = vm->data[stack_start];
	rwinf->adrr = stack_start;
		rwinf->adrw = stack_start;
	if (stackPtr == stack_start || stackPtr == 0) {
		vm->data[stack_start] = stack_start+1;
	} else {
		vm->data[stack_start] += frame_size;
	}
	stackPtr = vm->data[stack_start];

	vm->data[stackPtr] = vm->co+1;

	vm->co = 1+(vm->jumptable[getFunction(vm->jumptable, JUMPTABLESIZE, header, inst.operand_1)].pc);

	return;
}

void doReturn(H5VM_VirtualMachine *vm, H5VM_ReadWriteInfo *rwinf) {
	/*The type must be literal, so no need to check*/
	H5VM_Instruction inst = vm->code[vm->co].inst;
	h5uint stack_start = inst.operand_2 & 0xFF00;
	h5uint frame_size = (inst.operand_2 & 0x00FF) + 1;
	rwinf->read_adrr = 1;

	h5uint stackPtr = vm->data[stack_start];
	rwinf->adrr = stack_start;
		rwinf->adrw = stack_start;
	if (stackPtr == stack_start+1) {
		vm->data[stack_start] = stack_start;
	} else if (stackPtr == stack_start || stackPtr == 0) {
		rwinf->errcode = 3; /*Callstack underflow*/
		return;
	} else {
		vm->data[stack_start] -= frame_size;
	}
	vm->co = vm->data[stackPtr];

	return;
}

unsigned doGenericOperation(H5VM_VirtualMachine *vm, H5VM_ReadWriteInfo *rwinf, char type) {
	h5uint op2 = 0;
	if (type != 'f') {
		op2 = read_RA(vm, rwinf);
		if (rwinf->errcode != 0) goto END;
	}

	h5uint result = 0;
	h5uint op1 = 0;

	if (type == 'c' /*copy, aka SET opcode*/) {
		result = op2;
	} else if (type == 'f') /*FRAME opcode*/ {
		result = getFrameData(vm, rwinf);
	} else {
		op1 = read_WA(vm, rwinf, type == '?');
		if (rwinf->errcode != 0) goto END;

		result = operation(type, op1, op2);
		if (rwinf->errcode != 0) goto END;
	}

	if (type != '?') {
		write_WA(vm, rwinf, result);
	}

	/*Set zero flag*/
	if (result == 0) {
		rwinf->wrote_zf = 1;
		vm->data[0xFFFF] = 0;
	} else if (type != 'c') {
		rwinf->wrote_zf = 1;
		vm->data[0xFFFF] = 1;
	} else if (type == 'c') {
		;
	}

	/*Set carry flag*/
	if (type == '+') {
		if (result < op1) {
			/*Overflow*/
			rwinf->wrote_cf = 1;
			vm->data[0xFFFE] = 1;
		} else {
			rwinf->wrote_cf = 1;
			vm->data[0xFFFE] = 0;
		}
	} else if (type == '-' || type == '?') {
		if (result > op1) {
			/*Underflow*/
			rwinf->wrote_cf = 1;
			vm->data[0xFFFE] = 1;
		} else {
			rwinf->wrote_cf = 1;
			vm->data[0xFFFE] = 0;
		}
	} else {
		;
	}

	END:
		vm->co += 1;
		return rwinf->errcode;
}

unsigned H5VM_execute(H5VM_VirtualMachine *vm, H5VM_ReadWriteInfo *rwinf) {
	H5VM_Header header = vm->code[vm->co].header;
	H5VM_Instruction inst = vm->code[vm->co].inst;
	*rwinf = UNEVENTFUL;
	h5uint errcode = 0;

	switch(inst.opcode) {
	case Inst_halt:
		vm->hf = 1;
		break;
	case Inst_jmp: ;
		h5uint newco = read_WA(vm, rwinf, 1);
		vm->co = newco;
		break;
	case Inst_skpz:
		if (vm->data[0xFFFF] == 0) {
			vm->co += inst.operand_1;
			vm->co += 1;
		} else {
			vm->co += 1;
		}
		break;
	case Inst_skmz:
		if (vm->data[0xFFFF] == 0) {
			vm->co -= inst.operand_1;
			vm->co -= 1;
		} else {
			vm->co += 1;
		}
		break;
	case Inst_set:
		errcode = doGenericOperation(vm, rwinf, 'c');

		break;
	case Inst_add:
		errcode = doGenericOperation(vm, rwinf, '+');
		break;
	case Inst_sub:
		errcode = doGenericOperation(vm, rwinf, '-');
		break;
	case Inst_and:
		errcode = doGenericOperation(vm, rwinf, 'a');
		break;
	case Inst_or:
		errcode = doGenericOperation(vm, rwinf, 'o');
		break;
	case Inst_xor:
		errcode = doGenericOperation(vm, rwinf, 'x');
		break;
	case Inst_shift:
		errcode = doGenericOperation(vm, rwinf, 's');
		break;
	case Inst_cmp:
		errcode = doGenericOperation(vm, rwinf, '?');
		break;
	case Inst_func: ;
		h5uint function_id = inst.operand_1;
		/*Register the subroutine in the jumptable*/
		setFunction(vm->jumptable, JUMPTABLESIZE, header, function_id, vm->co);

		/*Skip subroutine*/
		int i = 1;
		do {
			if (i > 512) {
				/*Memory safety safeguard. If you're making subroutines 
				  with over 512 instructions you know to change this anyways*/
				rwinf->errcode = 3; /*Wrong usage: No Matching RET!*/
				break;
			}
			vm->co += 1;
			i += 1;
		} while (vm->code[vm->co].inst.opcode != Inst_ret);
		vm->co += 1;

		break;
	case Inst_ret:
		/*Implementation:
		   - If the stack pointer isn't initialized, error out (underflow, ERR 4)
		   - Else if the stack pointer is at minimum, de-initialize it
		   - Else, decrease the stack pointer by ((operands_2 & 0x00FF)+1)
		   - Set the program counter to the contents of the old stack frame's first address
		*/
		doReturn(vm, rwinf);
		break;
	case Inst_call:
		/*Implementation:
		   - If the stack pointer isn't initialized, set it to minimum
		   - Else if the stack pointer is at a memory map/write access boundary, error out (overflow, ERR 4)
		   - Else, increase the stack pointer ((operands_2 & 0x00FF)+1)
		   - Set the new stack frame's first address to (program_counter+1)
		   - Set the program counter to (jumptable[operands_1]+1)
		*/
		doCall(vm, rwinf);
		break;
	case Inst_frame:
		errcode = doGenericOperation(vm, rwinf, 'f');		
		break;
	}
	vm->data[0xFFFA] = vm->co;
	return errcode;
}
