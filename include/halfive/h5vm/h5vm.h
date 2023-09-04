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

#define JUMPTABLESIZE 256 /*Size in words of the subroutine jumptable*/


typedef enum {
	Inst_halt = 0,
	Inst_jmp  = 1,
	Inst_skpz = 2,
	Inst_skmz = 3,
	Inst_set = 4,
	Inst_add = 5,
	Inst_sub = 6,
	Inst_and = 7,
	Inst_or	 = 8,
	Inst_xor = 9,
	Inst_shift = 10,
	Inst_cmp = 11,
	Inst_func = 12,
	Inst_ret = 13,
	Inst_call = 14,
	Inst_frame = 15
} H5VM_InstructionSet;

typedef enum { H5VM_ModeAdr = 0, H5VM_ModeLit = 1, H5VM_ModePtr = 2 } H5VM_AddressingMode;

typedef enum {
	H5VM_CharA = 0,
	H5VM_CharB = 1,
	H5VM_CharC = 2,
	H5VM_CharD = 3,
	H5VM_CharE = 4,
	H5VM_CharF = 5,
	H5VM_CharH = 6,
	H5VM_CharI = 7,
	H5VM_CharO = 8,
	H5VM_CharS = 9,
	H5VM_CharX = 10,
	H5VM_Char1 = 11,
	H5VM_Char2 = 12,
	H5VM_Char3 = 13,
	H5VM_Char4 = 14,
	H5VM_Char5 = 15
} H5VM_Character;

typedef enum {
	H5VM_SWLibrary = 0,
	H5VM_SWModule = 1,
	H5VM_SWApplication = 2,
	H5VM_SWOther = 3,
} H5VM_SoftwareType;

/*Execution memory*/

typedef struct {
	H5VM_Character char1: 4;
	H5VM_Character char2: 4;
	h5uchar namespace: 8;
	H5VM_SoftwareType sw_type: 4;
	h5uchar multitasking_model: 4;
} H5VM_Header;

typedef struct {
	H5VM_InstructionSet opcode: 4;
	H5VM_AddressingMode optype_1: 2;
	H5VM_AddressingMode optype_2: 2;
	h5uint operand_1;
	h5uint operand_2;
} H5VM_Instruction;

typedef struct {
	H5VM_Header header;
	H5VM_Instruction inst;
} H5VM_ExecutableUnit;

typedef struct {
	_Bool isActive;
	H5VM_Character char1: 4;
	H5VM_Character char2: 4;
	h5uint id;
	h5uint pc;
} H5VM_CrossProcessProgramCounter;

typedef struct {
	H5VM_ExecutableUnit code[0x10000];
	h5uint data[0x10000]; /*Array of pointers*/
	H5VM_CrossProcessProgramCounter jumptable[JUMPTABLESIZE];
	_Bool hf; /*Halt flag*/
	h5uint co; /*Program counter*/ /*MAPPED TO 0xFFFA*/
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
	_Bool wrote_adrw; /*Was there a write to memory?*/
	_Bool read_adrw;  /*Was the address written to also read?*/
	_Bool read_adrr;  /*Was there a (independent) read from memory?*/
	_Bool wrote_zf;	  /*Was the zero flag written to?*/
	_Bool wrote_cf;	  /*Was the carry flag written to?*/
	h5uint adrw;	  /*Which address was written to?*/
	h5uint adrr;	  /*Which address was read from?*/
} H5VM_ReadWriteInfo;

/*Halfive VM interface*/

/*Update the memory setup, please don't call every cycle, just when needed...*/

/*Execute one instruction from the program*/
extern unsigned H5VM_execute(
	H5VM_VirtualMachine *program, H5VM_ReadWriteInfo *rwinf);
#endif
