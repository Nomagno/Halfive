/*
Copyright Nomagno 2021, 2022

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

/*READ THE HWVM SPEC*/

#ifndef HWVM_H
#define HWVM_H
#include <halfworld/hwreq.h>
#include <stdint.h>

#define MEMSIZE 4096

/*Half-World Virtual Machine

(AT THE BOTTOM OF THIS BIG COMMENT BLOCK YOU HAVE THE ACTUAL CODE)

Instructions are terminated by newline, and the number of arguments shown in
comments is REQUIRED
Literals should be prefixed by 'and' e.g. &6
Pointers should be prefixed by 'equal' e.g. =6
BINARY FORMAT:
You code each instruction as 4 bits of which the least significant 2 indicate
which arguments are literals and which addresses, and the highest indicates if
the literals are pointers or not (0001 - from right to left:  first is literal,
second address, third bit is padding, high bit indicates they are literals and NOT
pointers), then 4 bits for the instructions themselves (check the listed values)
then two 16-bit 'arguments' (0-2),
and repeat.

EXAMPLE:
BINARY: 0001 0101 000000000000001 000000000011111 0000 0000 0000000000000000 0000000000000000
DECIMAL:   1    5               1              31 0       0                0                0
ASSEMBLY:     add               1             =1F      halt
ENGLISH: add the contents of address ONE and the number 1F, put the result back into address ONE, halt

ASSEMBLY FORMAT:
    It codes almost directly to the binary format. The available instructions
are: {halt, skz, jmp, jcz, set, add, sub, not, and, xor, or, rot, func, ret,
call, jcnz} The syntax is the following: instruction ARGx ARGx \n Where ARGx
is an address/pointer (Rx), a literal (ID), or any of these (Vx). Instructions
are terminated by newline '\n'


The enum below specifies in a comment the behaviour of each instruction and the
type of its arguments
```

*/

typedef enum {
	/*THE INSTRUCTION COUNTING SHALL START AT 0, NOT 1*/
	/*IMPORTANT NOTE: LITERALS ARE ONLY ALLOWED IN REGISTER ARGUMENTS
	   DOCUMENTED AS VALUES 'Vx', NOT IN REGISTERS 'Rx'*/
	Inst_halt = 0, /* ; halt*/
	Inst_skz = 1,  /* LITERAL; if ZF == 0, add LITERAL+1 to program counter, else do nothing*/
	Inst_set = 2,  /* R1 V2; set address R2 to value V1*/
	Inst_jmp = 3,  /* V1; hand execution to instruction numbered V1. SPECIAL
		     EXCEPTION: Take 16-bit literals, addresses are treated as
		     16-bit literals, and pointers are treated as addresses with
		     a 16-bit value*/
	Inst_jcz = 4,  /* V1; if ZF == 0, jmp to instrucion V1*/
	/*ADD trough to NOT: put result of doing
	stuff with Vn values into Rn address*/
	Inst_add = 5, /* R1 V2; addition, carry goes to carry flag*/
	Inst_sub = 6, /* R1 V2; substraction, carry flag set if result is negative*/
	Inst_and = 7, /* R1 V2; binary and*/
	Inst_or = 8,      /* R1 V2; binary or*/
	Inst_xor = 9,     /* R1 V2; binary exclusive or*/
	Inst_rot = 10,    /* R1 V2; If V2 is 0-7, bitshift R1 LEFT by V2 bits. Else
			if V2 is 8-F, bitshift R1 RIGHT by (V2-8) bits. Else do
			nothing. Put the result into R1*/
	Inst_cmp = 11,    /* V1 V2; if V1 is bigger than V2, sets the carry flag to 0
		     and    the zero flag to 1 if V1 is smaller than V2, sets the
		     carry    flag to 1 and the zero flag to 0 if V1 is equal to
		     V2, sets    the carry flag to 0 and the zero flag to 0*/
	/* FUNC, RET, CALL: Stackless subroutines*/
	Inst_func = 12, /* ID; Marks the start of a subroutine with the literal
		      identifier ID.*/
	Inst_ret = 13,  /*ID; Marks the end of subroutine ID. It JMPs to the
		       instruction after the corresponding CALL instruction.*/
	Inst_call = 14, /* ID; JMP to the start of execution (post-FUNC) of
		      subroutine ID*/
	Inst_jcnz = 15  /* V1; if ZF != 0, jmp to instrucion V1*/
} HWVM_InstructionSet;

/*Execution memory*/
typedef struct {
	/*Every first byte*/
	HWVM_InstructionSet inst[MEMSIZE * 4];

	/*Every second, third and fourth bytes*/
	/*int because we need this to be at least 16 bits by default*/
	hwuint opnd[MEMSIZE * 4][3];
	/*The third  row of the array indicates which arguments are addresses,
	trough its least significant 2 bits.
	E.G 00 ALL ARE ADDRESSES -- 11 BOTH ARE LITERALS*/

} HWVM_CodeMemory;

/*Approximate size on disk for MEMSIZE = 4096:
200KBs*/
typedef struct {
	HWVM_CodeMemory code;        /*Code memory*/
	hwuchar *data[MEMSIZE * 16]; /*Data memory, default setup:*/
	/*GEN mem (RW), 0x0000 to 0x3FFF, RECOMMENDED TO EXIST*/
	/*DRIVE (persistent, R-only) mem, 0x4000 to 0xBFFF, RECOMMENDED TO
	 * EXIST*/
	/*Zero flag (RW), 0xFFFF, OBLIGATORY*/
	/*Carry flag (RW), 0xFFFE, OBLIGATORY*/
	/*Input register (R-only), 0xFFFD, OBLIGATORY*/
	/*Output register, 0xFFFC, OBLIGATORY*/

	/*If 0, data index is RW, if 1 it is read only*/
	_Bool mask[MEMSIZE * 16];
	hwuint func_co[64]; /*For storing counter values corresponding to the FUNC instruction of each subroutine ID*/
	hwuint return_co[64]; /*For storing counter values corresponding to the execution environment of the branch that executes CALLS, for each subroutine ID*/
	hwuint skip_co[64]; /*Counter values FUNC has to skip to*/
	/*Halt flag, 1 if it has halted.*/
	_Bool hf;
	/*Program counter (R-only), 0xFFFB (low) and 0xFFFA (high), OBLIGATORY*/
	hwuint co;
} HWVM_GeneralMemory;

/*Default memory setup*/
typedef struct {
	hwuchar gmem[MEMSIZE * 4];
	hwuchar driv[MEMSIZE * 8];
	hwuchar zf;
	hwuchar cf;
	hwuchar in;
	hwuchar ou;
	hwuchar *co_high;
	hwuchar *co_low;
} HWVM_DefaultMemSetup;

typedef struct {
	_Bool was_err; /*Was there an error?*/
	_Bool wrote_adrw; /*Wrote write address THIS CYCLE?*/
	_Bool read_adrw; /*Write address read THIS CYCLE?*/
	_Bool read_adrr; /*Read address read THIS CYCLE?*/
	_Bool write_zf; /*Additionally, wrote zero flag?*/
	_Bool write_cf; /*Additionally, wrote carry flag?*/
	hwuint adrw; /*Write address*/
	hwuint adrr; /*Read address*/
} HWVM_ReadWriteInfo;

/*Half-World VM interface*/

/*Generate the whole VM from code and the default memory setup*/
extern HWVM_GeneralMemory HWVM_Init(HWVM_CodeMemory *code,
				    HWVM_DefaultMemSetup *rawmem);

/*Execute one instruction from the program*/
extern unsigned HWVM_Execute(HWVM_GeneralMemory *program,
			     HWVM_ReadWriteInfo *rwinf);
#endif
