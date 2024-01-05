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

/*Halfive Virtual Machine implementation*/
/*IMPORTANT: READ THE H5VM SPEC*/

#ifndef H5VM_H
#define H5VM_H
#include <halfive/h5req.h>

typedef enum {
	/*Control flow group*/
	Inst_halt =  0,
	Inst_jump  = 1,
	Inst_skpz =  2,
	Inst_skmz =  3,

	/*Subroutine group*/
	Inst_stki =  4,
	Inst_stkd =  5,
	Inst_func =  6,
	Inst_back =  7,
	Inst_call =  8,

	/*Memory management group*/
	Inst_mlod =  9,
	Inst_mstr =  10,
	Inst_vlod =  11,
	Inst_vstr =  12,
	Inst_fset =  13,
	Inst_fdel =  14,
	Inst_fflp =  15,

	/*Arithmetic group*/
	Inst_badd =  16,
	Inst_bsub =  17,
	Inst_band =  18,
	Inst_inor =  19,
	Inst_exor =  20,
	Inst_shft =  21,
	Inst_comp =  22,
} H5VM_InstructionSet;

/*
mode value:
0 - default
1 - immediate
2 - pointer
3 - special
*/

typedef struct {
	h5uchar mode: 2;
	H5VM_InstructionSet opcode: 6;
	h5uchar reg: 8;
	h5uint operand: 16;
} H5VM_Instruction;

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

typedef enum {
	H5VM_Register_Flag=0,
	H5VM_Register_ProgramCounter=1,
	H5VM_Register_StackPointer=2,
	H5VM_Register_StackSize=3,

	H5VM_Register_A=4,
	H5VM_Register_B=5,
	H5VM_Register_C=6,
	H5VM_Register_D=7,

	H5VM_Register_W=8,
	H5VM_Register_X=9,
	H5VM_Register_Y=10,
	H5VM_Register_Z=11
} H5VM_Registers;

/*This enables seamless switching between harvard and von-neumann architectures*/
#define INST(x) (H5VM_Instruction) { (x) >> 30, ((x) >> 24) & 0x3F, ((x) >> 16) & 0xFF, (x) & 0xFFFF }
#define MACHINE_CODE(x) (h5ulong){ (h5ulong)((long int)(x.mode) /*this cast is because C is stupid*/ << 30) | ((x.opcode) << 24) | ((x.reg) << 16) | (x.operand) }

typedef struct {
	h5ulong code[0x08000];
	h5uint data[0x10000];
	h5uint registers[0x100];
	_Bool hf; /*Halt flag*/
} H5VM_VirtualMachine;


/*
Error codes:
0 - No error
1 - NONFATAL - Read/write unmapped mem
2 - NONFATAL - Write to read-only mem
3 - NONFATAL - Wrong addressing mode / Wrong instruction usage
4 -    FATAL - Callstack underflow
Other- FATAL - Unknown error
*/
typedef struct {
	unsigned errcode;	  /*Zero means no error*/
	h5uint adrw;	  /*Which address/register was written to?*/
	h5uint adrr;	  /*Which address/register was read from?*/
} H5VM_ReadWriteInfo;

/*Halfive VM interface*/

/*Execute one instruction from the program*/
extern unsigned H5VM_execute(
	H5VM_VirtualMachine *program, H5VM_ReadWriteInfo *rwinf);
#endif
