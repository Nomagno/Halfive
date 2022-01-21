/*
Copyright Nomagno 2021

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
#include <halfworld/hwreq.h>
#define MEMSIZE 4096
#define MEMSMALL 1024
#include <stdint.h>

typedef uint8_t hwuchar;
typedef uint16_t hwuint;

/*Half-World Virtual Machine

(AT THE BOTTOM OF THIS BIG COMMENT BLOCK YOU HAVE THE ACTUAL CODE)

Instructions are terminated by newline, and the number of arguments shown in
comments is REQUIRED
Literals should be prefixed by 'and' e.g. &6
Pointers should be prefixed by 'equal' e.g. =6

BINARY FORMAT:
You code each instruction as 4 bits of which the least significant 2 indicate
which arguments are literals and which addresses, and the highest indicates if
the literals are pointers or not (0001 - from left to right:  first is address,
second literal, high bit indicates they are literals and NOT
pointers), then 4 bits for the instructions themselves (check the listed values)
then simply read the corresponding number of 16-bit 'arguments' (0-2),
and repeat.

EXAMPLE:
BINARY: 0001 0001 000000000000001 000000000011111
DECIMAL:   1    3               1              31
ASSEMBLY:     add               1             =1F
ENGLISH: add the contents of address ONE and the number 1F, put the result back into address ONE

ASSEMBLY FORMAT:
    It codes almost directly to the binary format. The available instructions
are: {halt, nop, jmp, jcz, set, add, sub, not, and, xor, or, rot, subs, sube,
call, jcnz} The syntax is the following: instruction ARGx ARGx \n Where ARGx
is an address/pointer (Rx), a literal (ID), or any of these (Vx). Instructions are terminated
by newline '\n'


The enum below specifies in a comment the behaviour of each instruction and the
type of its arguments
```

*/

typedef enum {
	/*THE INSTRUCTION COUNTING SHALL START AT 0, NOT 1*/
	/*IMPORTANT NOTE: LITERALS ARE ONLY ALLOWED IN REGISTER ARGUMENTS
	   DOCUMENTED AS VALUES 'Vx', NOT IN REGISTERS 'Rx'*/
	halt = 0, /* ; halt*/
	nop = 1,  /* ; do nothing*/
	set = 2,  /* R1 V2; set address R2 to value V1*/
	jmp = 3,  /* V1; hand execution to instruction numbered V1. SPECIAL EXCEPTION: Take 16-bit literals, addresses are treated as 16-bit literals, and pointers are treated as addresses with a 16-bit value*/
	jcz = 4,  /* V1; if ZF == 0, jmp to instrucion V1*/
	/*ADD trough to NOT: put result of doing
	stuff with Vn values into Rn address*/
	add = 5, /* R1 V2; addition, carry goes to carry flag*/
	sub =
	    6, /* R1 V2; substraction, carry flag set if result is negative*/
	    and = 7, /* R1 V2; binary and*/
	or = 8,      /* R1 V2; binary or*/
	xor = 9,     /* R1 V2; binary exclusive or*/
	rot = 10, /* R1 V2; If V2 is 0-7, bitshift R1 LEFT by V2 bits. Else
		     if V2 is 8-F, bitshift R1 RIGHT by (V2-8) bits. Else do
		     nothing. Put the result into R1*/
	cmp = 11, /* V1 V2; if V1 is bigger than V2, sets the carry flag to 0
		  and    the zero flag to 1 if V1 is smaller than V2, sets the
		  carry    flag to 1 and the zero flag to 0 if V1 is equal to
		  V2, sets    the carry flag to 0 and the zero flag to 0*/
	/*SUBS, SUBE, CALL: Stackless subroutines*/
	subs = 12, /* ID; Marks the start of a subroutine with the literal
                      identifier ID.*/
	sube = 13, /*ID; Marks the end of subroutine ID. It JMPs to the
		      instruction after the corresponding CALL instruction.*/
	call = 14, /* ID; JMP to the start of execution (post-SUBS) of
		      subroutine ID*/
	jcnz = 15  /* V1; if ZF != 0, jmp to instrucion V1*/
} HWVM_InstructionSet;

/*Execution memory*/
typedef struct {
	/*Every first byte*/
	HWVM_InstructionSet inst[MEMSIZE * 4];

	/*Every second, third and fourth bytes*/
	/*int because we need this to be at least 16 bits by default*/
	hwuint opnd[MEMSIZE * 4][3];
	/*The fourth row of the array indicates which arguments are addresses,
	trough its least significant 2 bits.
	E.G 00 ALL ARE ADDRESSES -- 11 BOTH ARE LITERALS*/

} HWVM_CodeMemory;

/*Data memory*/
typedef struct {
	/*GEN mem, 0x0000 to 0x3FFF*/
	hwuchar gp[MEMSIZE * 4];

	/*DRIVE (persistent) mem, read-only 0x4000 to 0xBFFF*/
	hwuchar dr[MEMSIZE * 8];

	/*Zero flag, 0xFFFF*/
	hwuchar zf;

	/*Carry flag, 0xFFFE*/
	hwuchar cf;

	/*Input register, read-only, 0xFFFD*/
	hwuchar in;
	_Bool fi; /*Input flag, 1 if there's any input reading to do*/

	/*Output register, write-only, 0xFFFC*/
	hwuchar ou;
	_Bool fo; /*Output flag, 1 if there's any output to handle*/

	/*Program counter, read-only, 0xFFFB (low) and 0xFFFA (high)*/
	hwuint co;


} HWVM_DataMemory;

/*Approximate size on disk for MEMSIZE = 4096, MEMSMALL = MEMSIZE/4:
132KBs*/
typedef struct {
	HWVM_CodeMemory m1; /*Code memory*/
	HWVM_DataMemory m2; /*Data memory*/

	_Bool hf; /*Halt flag, 1 if it has halted.*/

	hwuint sub_co[MEMSMALL]; /*For storing counter values corresponding to
				the SUBS instruction of each subroutine ID*/

	hwuint return_co[MEMSMALL]; /*For storing counter values corresponding
				to the execution environment of the branch that
				executes CALLS, for each subroutine ID*/

	hwuint skip_co[MEMSMALL]; /*Counter values subs has to skip to*/
} HWVM_GeneralMemory;

/*Half-World VM interface*/

extern HWVM_GeneralMemory HWVM_Init(HWVM_CodeMemory code); /*Generate mem struct from xmem struct*/
extern hwuint HWVM_Execute(HWVM_GeneralMemory *program); /*Execute one instruction from the program*/
