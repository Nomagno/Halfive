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

/*Halfive Virtual Machine implementation*/
/*IMPORTANT: READ THE H5VM SPEC*/

#ifndef H5VM_H
#define H5VM_H
#include <halfive/h5req.h>

#define MEMUNIT 0x1000 /*Minimal byte chunks*/
#define MEMSIZE 16	   /*Size in MEMUNIT byte chunks*/

/* THERE MUST BE AT LEAST ENOUGH SPACE FOR THE 16 SPECIAL REGISTERS: */
/* (MEMUNIT * (MEMSIZE - GMEMSIZE - DRIVSIZE)) >= 16*/

#define CODE_MEMSIZE                                   \
	4 /*Size in MEMUNIT instruction chunks of the code \
	 memory*/

/*These three are guaranteed to be contiguous*/
#define GMEMSIZE 4 /*Size in MEMUNIT byte chunks*/
#define DRIVSIZE 8 /*Size in MEMUNIT byte chunks*/
#define STACKSIZE 1 /*Size in MEMUNIT byte chunks*/

#define FMEMSIZE 64 /*Size in bytes of the subroutine memory*/

/*Halfive Virtual Machine

(AT THE BOTTOM OF THIS BIG COMMENT BLOCK YOU HAVE THE ACTUAL CODE)

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

EXAMPLE:
BINARY: 0001 0101 000000000000001 000000000011111 0000 0000 0000000000000000 0000000000000000
DECIMAL:   1    5               1              31    0    0                0                0
ASSEMBLY:     add               1             =1F      halt
ENGLISH: add the contents of address ONE and the number 1F, put the result back
into address ONE, halt

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
	- SPECIAL EXCEPTION: Takes 16-bit literals. Addresses are treated the same as literals. Pointers are treated as addresses where the highest 8 bits are the value of the address corresponding to the operand, and the lowest 8 bits are the value of (address of the operand)+1

2. skpz `L1` - Skip plus if zero
	- If _ZF has the value 0, add `L1`+1 to program counter.
	- SPECIAL EXCEPTION: Takes 16-bit literals.

3. skmz `L1` - Skip minus if zero
	- If _ZF has the value 0, substract (`L1`+1) from program counter.
	- SPECIAL EXCEPTION: Takes 16-bit literals.

4. set `R1 V2` - Copy the value of `V2` to the address of `R1`.

5. add `R1 V2` - Addition
	- Perform addition between the value of `R1` and `V2`.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.
	- If the operation results in overflow, set _CF to 1.
	- Otherwise, set _CF to 0.

6. sub `R1 V2` - Substraction
	- Perform substraction between the value of `R1` and `V2`.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.
	- If the operation results in overflow, set _CF to 1.
	- Otherwise, set _CF to 0.

7. and `R1 V2` - Binary And
	- Perform binary AND with the value of `R1` and `V2`.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.

8. or `R1 V2` - Inclusive Or
	- Perform inclusive OR with the value of `R1` and `V2`.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.

9. xor `R1 V2` - Exclusive Or
	- Perform exclusive OR with the value of `R1` and `V2`.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.

10. shift `R1 V2` - Bitshift
	- If the value of `V2` is 0-7, bitshift the value of `R1` LEFT  by `V2` bits.
	- If the value of `V2` is 8-F, bitshift the value of `R1` RIGHT by (`V2-8`) bits.
	- If the value of `V2` is greater than F, the value of `R1` remains unchanged.
	- Put the result into the address of `R1`.
	- If the value 0 was written to the address of `R1`, set _ZF to 0.

11. cmp `V1 V2` - Comparison
	- Perform substraction between the value of `V1` and `V2`.
	- If the operation results in underflow, set _CF to 1.
	- Otherwise, set _CF to 0.

12. func `L1 L2`
	- Writes down (preferrably in a place not normally addressable) the program counter as (start of subroutine `L1`).
	- Moves the program counter to directly after the closest `ret` instruction.

13. ret `L1 L2`
	- Marks the end of subroutine `L1`.
	- When reached, the current stack frame is deleted.
	- Afterwards, the program counter is set to the return address.

14. call `L1 L2`
	- Move program counter to (start of subroutine `L1`)+1.
	- Creates a new stack frame by increasing the stack pointer at (High nibble of `L2`)`*`4096.
	- The stack frame has size (Low nibble of `L2`)`*`2+2.
	- The first two bytes of the stack frame are the return address.
	- The return address is written down as the program counter corresponding to the instruction after this one.

15. frame `V1 L2`
	- Get current stack frame information
	- (High nibble of `L2*4096`) is the start address of the stack.
	- (Low  nibble of `L2`)`*`2+2   is the size in bytes of each stack frame.
	- The address of the current stack frame is deposited as a 16-bit number to the addresses `V1` and `V1+1`.
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

/*Callstack layout for example info byte CF:
C000, C001: Stack pointer
C002, C003: Return address of first frame
C004, C005: Variable 1 of first frame
C006, C007: Variable 2 of first frame
C008, C009: Variable 3 of first frame
C00A, C00B: Variable 4 of first frame
C00C, C00D: Variable 5 of first frame
C00E, C00F: Variable 6 of first frame
C010, C011: Variable 7 of first frame
C012, C013: Variable 8 of first frame
C014, C015: Variable 9 of first frame
C016, C017: Variable A of first frame
C018, C019: Variable B of first frame
C01A, C01B: Variable C of first frame
C01C, C01D: Variable D of first frame
C01E, C01F: Variable E of first frame
C020, C021: Variable F of first frame
C022, C023: Return address of second frame
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
100KBs*/
typedef struct {
	H5VM_CodeMemory code;			  /*Code memory*/
	h5uchar *data[MEMUNIT * MEMSIZE]; /*Data memory, default setup:
	- GEN mem (RW), 0x0000 to 0x3FFF, RECOMMENDED TO EXIST
	- DRIVE mem (R-only), 0x4000 to 0xBFFF, RECOMMENDED TO EXIST
	- CALLSTACK mem (RW), 0xC000 to 0xCFFF, RECOMMENDED TO EXIST
	- Zero flag (RW), 0xFFFF, OBLIGATORY
	- Carry flag (RW), 0xFFFE, OBLIGATORY
	- Input register (R-only), 0xFFFD, OBLIGATORY
	- Output register, 0xFFFC, OBLIGATORY*/

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
	h5uchar gmem[MEMUNIT * GMEMSIZE];
	h5uchar driv[MEMUNIT * DRIVSIZE];
	h5uchar stack[MEMUNIT * STACKSIZE];
	h5uchar zf;
	h5uchar cf;
	h5uchar in;
	h5uchar ou;
	h5uchar *co_high;
	h5uchar *co_low;
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
