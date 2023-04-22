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

/*H5VM words are 16-bits long*/

#ifndef H5VM_H
#define H5VM_H
#include <halfive/h5req.h>

#define MEMUNIT 0x1000 /*Minimal byte chunks*/
#define MEMSIZE 16	   /*Size in MEMUNIT word chunks*/

/* THERE MUST BE AT LEAST ENOUGH SPACE FOR THE 16 SPECIAL REGISTERS: */
/* (MEMUNIT * (MEMSIZE - GMEMSIZE - DRIVSIZE)) >= 16*/

#define CODE_MEMSIZE                                   \
	4 /*Size in MEMUNIT instruction chunks of the code \
	 memory*/

/*These three are guaranteed to be contiguous*/
#define RAMSIZE    7 /*Size in MEMUNIT word chunks*/
#define DROMSIZE   2 /*Size in MEMUNIT word chunks*/
#define GROMSIZE   4 /*Size in MEMUNIT word chunks*/
#define OUTPUTSIZE 1 /*Size in MEMUNIT word chunks*/
#define STACKSIZE  1 /*Size in MEMUNIT word chunks*/

#define FMEMSIZE 256 /*Size in words of the subroutine memory*/

/*Halfive Virtual Machine

(AT THE BOTTOM OF THIS BIG COMMENT BLOCK YOU HAVE THE ACTUAL DEFINITIONS)

Instructions are terminated by newline, and the number of arguments shown in
comments is REQUIRED
Literals should be prefixed by 'and' e.g. &6
Pointers should be prefixed by 'equal' e.g. =6
BINARY FORMAT:
You code each instruction as 4 bits of which the least significant 2 indicate
which arguments are literals and which addresses, and the highest indicates if
the literals are pointers or not (0001 - from right to left:  first is literal,
second address, third bit is padding, high bit indicates they are literals and
NOT pointers), then 4 bits for the instructions themselves (check the listed
values) then two 16-bit 'arguments' (0-2), and repeat.

BINARY: 0001 0101 000000000000001 000000000011111 0000 0000 0000000000000000 0000000000000000
HEX:    0x01 0x05            0x01            0x1F 0x00 0x00             0x00             0x00
DECIMAL:   1    5               1              31 0       0                0                0
ASSEMBLY:     add               1             =1F      halt
ENGLISH: add the contents of address ONE and the constant 0x1F, put the result in address ONE; stop execution
PROPER ASSEMBLY:
	add 1 =1F
	halt

ASSEMBLY FORMAT:
	It codes almost directly to the binary format. The available instructions
are: {halt, jmp, skpz, skmz, set, add, sub, not, and, xor, or, shift, func,
ret, call, frame} The syntax is the following: instruction ARGx ARGx \n Where ARGx is
an address/pointer (Rx), a literal (Lx), or any of these (Vx). Instructions are
terminated by newline '\n'


The enum below specifies in a comment the behaviour of each instruction and the
type of its arguments
```

*/

/*
0. halt - Stop program execution immediately and permanently

1. jmp `V1` - Jump
	- Change program counter to the value of `V1`
	- Addresses are treated the same as constants. Derreferences are treated as pointers to the address containing the program counter to set.

2. skpz `C1` - Skip plus if zero
	- If _ZF has the value 0, add `C1`+1 to program counter.

3. skmz `C1` - Skip minus if zero
	- If _ZF has the value 0, substract (`C1`+1) from program counter.

4. set `R1 V2` - Copy the value of `V2` to the address of `R1`.

5. add `R1 V2` - Addition
	- Perform addition between the value of `R1` and `V2`.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.
	- Otherwise, set _ZF to 1.
	- If the operation results in overflow, set _CF to 1.
	- Otherwise, set _CF to 0.

6. sub `R1 V2` - Substraction
	- Perform substraction between the value of `R1` and `V2`.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.
	- Otherwise, set _ZF to 1.
	- If the operation results in overflow, set _CF to 1.
	- Otherwise, set _CF to 0.

7. and `R1 V2` - Binary And
	- Perform binary AND with the value of `R1` and `V2`.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.
	- Otherwise, set _ZF to 1.

8. or `R1 V2` - Inclusive Or
	- Perform inclusive OR with the value of `R1` and `V2`.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.
	- Otherwise, set _ZF to 1.

9. xor `R1 V2` - Exclusive Or
	- Perform exclusive OR with the value of `R1` and `V2`.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.
	- Otherwise, set _ZF to 1.

10. shift `R1 V2` - Bitshift
	- If the value of `V2` is 0-7, bitshift the value of `R1` LEFT  by `V2` bits.
	- If the value of `V2` is 8-F, bitshift the value of `R1` RIGHT by (`V2-8`) bits.
	- If the value of `V2` is greater than F, the value of `R1` remains unchanged.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.
	- Otherwise, set _ZF to 1.

11. cmp `V1 V2` - Comparison
	- Perform substraction between the value of `V1` and `V2`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.
	- Otherwise, set _ZF to 1.
	- If the operation results in underflow, set _CF to 1.
	- Otherwise, set _CF to 0.

12. func `C1`
	- Writes down (preferrably in a place not normally addressable) the program counter as (start of subroutine `C1`).
	- Moves the program counter to directly after the closest `ret` instruction.

13. ret `C1 C2`
	- Marks the end of subroutine `C1`.
	- When reached, the current stack frame is deleted.
	- Afterwards, the program counter is set to the return address.

14. call `C1 C2`
	- Move program counter to (start of subroutine `C1`)+1.
	- Creates a new stack frame by increasing the stack pointer at (High octet of `C2`)`*`256.
	- The stack frame has size (Low octet of `C2` +  1) in memory cells.
	- The first memory cell of the stack frame is the return address.
	- The return address is written down as the program counter corresponding to the instruction after this one.

15. frame `R1 C2`
	- Get current stack frame information
	- (High octet of `C2`)`*`256 is the start address of the stack.
	- (Low  octet of `C2` + 1) is the size in memory cells of each stack frame.
	- The address of the current stack frame is deposited as a 16-bit number to the address `R1`.
*/
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

/*Callstack layout for example info byte C00F:
C000: Stack pointer

C001: Return address of first frame
C002: Variable 1 of first frame
C003: Variable 2 of first frame
C004: Variable 3 of first frame
C005: Variable 4 of first frame
C006: Variable 5 of first frame
C007: Variable 6 of first frame
C008: Variable 7 of first frame
C009: Variable 8 of first frame
C00A: Variable 9 of first frame
C00B: Variable 10 of first frame
C00C: Variable 11 of first frame
C00D: Variable 12 of first frame
C00E: Variable 13 of first frame
C00F: Variable 14 of first frame
C010: Variable 15 of first frame

C011: Return address of second frame
...
*/


/*Execution memory*/
typedef struct {
	/*Every first byte*/
	H5VM_InstructionSet inst[MEMUNIT * CODE_MEMSIZE];

	/*Every second, third and fourth bytes*/
	/*int because we need this to be at least 16 bits by default*/
	h5uint opnd[MEMUNIT * CODE_MEMSIZE][3];
	/*The third  row of the array indicates which arguments are addresses,
	trough its least significant 2 bits.
	E.G 00 ALL ARE ADDRESSES -- 11 BOTH ARE LITERALS*/

} H5VM_CodeMemory;

/*Approximate size on disk for MEMUNIT = 0x1000, MEMSIZE = 16:
200KBs*/
typedef struct {
	H5VM_CodeMemory code;			  /*Code memory*/
	h5uint *data[MEMUNIT * MEMSIZE]; /*Data memory, Sheewol Gear setup:
	- RAM (RW), 0x0000 to 0x6FFF
	- DROM (R-only), 0x7000 to 0x8FFF
	- GROM (R-only), 0x9000 to 0xCFFF
	- OUTPUT (RW), 0xD000 to 0xDFFF
	- CALLSTACK (RW), 0xE000 to 0xEFFF
	- Zero flag (RW), 0xFFFF
	- Carry flag (RW), 0xFFFE
	- Input register (R-only), 0xFFFD, LSB bitmask: UP DOWN LEFT RIGHT B4 B3 B2 B1
	- Output register, 0xFFFC*/

	/*If 0, data index is RW, if 1 it is read only*/
	_Bool mask[MEMUNIT * MEMSIZE];

	h5uint func_co[FMEMSIZE];

	/*Halt flag, 1 if it has halted.*/
	_Bool hf;

	/*Program counter (R-only), 0xFFFB (low) and 0xFFFA (high), OBLIGATORY*/
	h5uint co;
} H5VM_GeneralMemory;

/*Default memory setup*/
typedef struct {
	h5uint ram[MEMUNIT * RAMSIZE];
	h5uint drom[MEMUNIT * DROMSIZE];
	h5uint grom[MEMUNIT * GROMSIZE];
	h5uint output[MEMUNIT * OUTPUTSIZE];
	h5uint stack[MEMUNIT * STACKSIZE];
	h5uint zf;
	h5uint cf;
	h5uint in;
	h5uint ou;
	h5uint *prog_co_adr;
} H5VM_DefaultMemSetup;

typedef struct {
	_Bool was_err;	  /*Was there an error?*/
	_Bool wrote_adrw; /*Wrote write address THIS CYCLE?*/
	_Bool read_adrw;  /*Write address read THIS CYCLE?*/
	_Bool read_adrr;  /*Read address read THIS CYCLE?*/
	_Bool write_zf;	  /*Additionally, wrote zero flag?*/
	_Bool write_cf;	  /*Additionally, wrote carry flag?*/
	h5uint adrw;	  /*Write address*/
	h5uint adrr;	  /*Read address*/
} H5VM_ReadWriteInfo;

/*Halfive VM interface*/

/*Generate the whole VM from code and the default memory setup*/
extern H5VM_GeneralMemory H5VM_init(
	H5VM_CodeMemory *code, H5VM_DefaultMemSetup *rawmem);

/*Execute one instruction from the program*/
extern unsigned H5VM_execute(
	H5VM_GeneralMemory *program, H5VM_ReadWriteInfo *rwinf);
#endif
