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
#define MEMSIZE 4096
#define MEMSMALL 1024

typedef unsigned char uchar;
typedef unsigned int uint;

/*Half-World Virtual Machine

(AT THE BOTTOM OF THIS BIG COMMENT BLOCK YOU HAVE THE ACTUAL CODE)

Instructions are terminated by newline, and the number of arguments shown in
comments is REQUIRED Literals that are not addresses should be enclosed in
square braces e.g. [6]


BINARY FORMAT:
You code each instruction as 4 bits of which the least significant 3 indicate
which arguments are literals and which addresses (1010 - first is literal,
second address, third literal, fourth doesn't matter), then 4 bits for the
instructions themselves (check the enum values, max 12), then simply read the
corresponding number of 16-bit 'arguments' (0-3), and repeat. EXAMPLE:

      BINARY: 0010 0000000000000001 0000000000000011 0000000000000010 0000 0000000000000000
	NOTE: You can safely ignore the most significant bit of the second value
(0010), since only the least significant 3 code which arguments are literals.
      DECIMAL: 2 1 3 2 0 0
      ASSEMBLY:add 1 [3] 2\n halt
      ENGLISH:
	The first instruction has the middle argument as a literal.
	Add the CONTENTS of register 1 and the literal integer '3', then put the
result into register '2'. Halt.

  ASSEMBLY FORMAT:
    It codes almost directly to the binary format. The available instructions
are: {halt, nop, jmp, jcz, set, add, sub, not, and, xor, or, not, subs, sube, call, jcnz}
    The syntax is the following:
      instruction ARGx ARGx ARGx\n
    Where ARGx is an address (Rx), a literal (ID), or either (Vx)
    Instructions are terminated by newline '\n'

    For arguments labeled 'Value' (NOT for arguments labeled 'Register'),
literals are allowed in the form [ARGx] (Number enclosed in brackets) The enum
below specifies in a comment the behaviour of each proper instruction and the
corresponding

EXAMPLE (THE LINE NUMBERING AND EVERYTHING AFTER THE '#' IN EACH LINE IS NOT PART OF THE LANGUAGE):

```
0. set [33] FFFC # Write literal 0x33 (51, 00110011) to the OUTPUT REGISTER
1. jmp 4 # Jump to instruction 4
2. sub [4] 3 # Do the subtraction [4] - 3. We don't know what address three contains, but it will set the zero flag if it is 0x4
3. jcz 7 # If the previous comparison was indeed correct and address 3 contains 0x4, jump to instruction 7
4. add [FE] [8] FFFC # Write the addition of 0xFE (254, 11111110) and 0x8 (8, 00001000) to the OUTPUT register. It should regult in OVERFLOW
5. set FFFE FFFF # Copy the carry flag to the zero flag
6. jcnz 8 # If the zero flag is not zero (it shouldn't be, there was overflow and the carry flag was cloned!), jump to instruction 8
7.
8. jmp 2 # Jump to instruction 2
9. halt
```

*/

typedef enum {
	/*THE INSTRUCTION COUNTING SHALL START AT 0, NOT 1*/
	/*IMPORTANT NOTE: LITERALS ARE ONLY ALLOWED IN REGISTER ARGUMENTS
	   DOCUMENTED AS VALUES 'Vx', NOT IN REGISTERS 'Rx'*/
	halt = 0, /* ; halt*/
	nop = 1,  /* ; do nothing*/
	set = 2,  /* V1 R2; set address R2 to value V1*/
	jmp = 3,  /* V1; hand execution to instruction numbered V1*/
	jcz = 4,  /* V1; if ZF == 0, jmp to instrucion V1*/
	/*ADD trough to NOT: put result of doing
	stuff with Vn values into Rn address*/
	add = 5,  /* V1 V2 R3; addition, carry goes to carry flag*/
	sub = 6,  /* V1 V2 R3; substraction, carry flag set if result is negative*/
	and = 7,  /* V1 V2 R3; binary and*/
	or = 8,   /* V1 V2 R3; binary or*/
	xor = 9,  /* V1 V2 R3; binary exclusive or*/
	not = 10, /* V1 R2; binary negation*/
	cmp = 11, /* V1 V2; if V1 is bigger than V2, sets the carry flag to 0 and
		  the zero flag to 1 if V1 is smaller than V2, sets the carry
		  flag to 1 and the zero flag to 0 if V1 is equal to V2, sets
		  the carry flag to 0 and the zero flag to 0*/
	/*SUBS, SUBE, CALL: Stackless subroutines*/
	subs = 12, /* ID; Marks the start of a subroutine with the literal
		      ( '[]' brace enclosed ) identifier ID.
		     Add*/
	sube = 13, /*ID; Marks the end of subroutine ID. It JMPs to the instruction
			after the corresponding CALL instruction.*/
	call = 14, /* ID; JMP to the start of execution (post-SUBS) of subroutine ID*/
	jcnz = 15 /* V1; if ZF != 0, jmp to instrucion V1*/
} iset;

/*Execution memory*/
typedef struct {
	/*Every first byte*/
	iset inst[MEMSIZE*4];

	/*Every second, third and fourth bytes*/
	/*int because we need this to be at least 16 bits by default*/
	uint opnd[MEMSIZE*4][4];
	/*The fourth row of the array indicates which arguments are addresses,
	trough its least significant 3 bits.
	E.G 000 ALL ADDRESSES -- 101 FIRST AND THIRD ARE LITERALS*/

} xmem;

/*Data memory*/
typedef struct {
	/*GEN mem, 0x0000 to 0x3FFF*/
	uchar gp[MEMSIZE*4];

	/*DRIVE (persistent) mem, read-only 0x4000 to 0xBFFF*/
	uchar dr[MEMSIZE*8];

	/*Zero flag, 0xFFFF*/
	uchar zf;

	/*Carry flag, 0xFFFE*/
	uchar cf;

	/*Input register, read-only, 0xFFFD*/
	uchar in;

	/*Output register, write-only, 0xFFFC*/
	uchar ou;

	/*Program counter, read-only, 0xFFFB*/
	uint co;

} vmem;

typedef struct {
	xmem m1; /*Code memory*/
	vmem m2; /*Data memory*/

	uchar hf; /*Halt flag, 1 if it has halted.*/

	uint sub_co[MEMSMALL]; /*For storing counter values corresponding to the SUBS
				instruction of each subroutine ID*/

	uint return_co[MEMSMALL]; /*For storing counter values corresponding to the execution
				environment of the branch that executes CALLS, for each subroutine ID*/

	uint skip_co[MEMSMALL]; /*Counter values subs has to skip to*/
} mem;

/*Half-World VM interface*/

extern mem fxmem(xmem code); /*Generate mem struct from xmem struct*/

extern uint execnext(mem *program); /*Execute one instruction from the program*/
