/*
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
OF, OR IN CONNECTION WITH THE WORK OR THE USE OF OR OTHER DEALINGS IN THE WORK.*/

/*Nomagno's boring VM interface and instruction set*/

#define MEMSIZE 4096
typedef unsigned char uchar;
typedef unsigned int uint;
/*Nomagno's boring instruction set

(AT THE BOTTOM OF THIS BIG COMMENT BLOCK YOU HAVE THE ACTUAL CODE)

Instructions are terminated by semicolon, and the number of arguments shown in comments is REQUIRED
Literals that are not addresses should be enclosed in square braces e.g. [6]


  BINARY FORMAT: 
  You can turn this into a little binary format where you start 
  with the number of bits of each arguments (N) as an 8-bit value,
  then code each instruction as 4 bits of which the least significant 3 indicate which 
  arguments are literals and which addresses 
  (1010 - first is literal, second address, third literal, fourth doesn't matter), 
  then 4 bits fir the instructions themselves (check the enum values, max 12), then
  simply read the corresponding number of N-bit 'arguments' (0-3), and repeat.
  EXAMPLE:
     
      BINARY: 00000100 0010 0110 0001 0011 0010 0000 0000
        NOTE: You can safely ignore the most significant bit of the second value (0010), since only the least significant 3 code which arguments are literals.
      DECIMAL: 4 2 6 1 3 2 0 0
      ASSEMBLY: bits 4; add 1 [3] [2]; halt;
      ENGLISH: 
        This binary contains 4-bit args. 
        The first instruction has the middle argument as a literal.
        Add the CONTENTS of register 1 and the literal integer '3', then put the result into register '2'. 
        Halt.

  ASSEMBLY FORMAT:
    It codes almost directly to the binary format. The available instructions are: {halt, nop, jmp, jcz, set, add, sub, not, and, xor, or, not}
    The following pseudo instructons are available: {bits, START, END, CALL}

    The syntax is the following:
      instruction ARG1 ARG2 ARG3;
    Where ARGx is a number
    Instructions are terminated by semicolon ';'

    For arguments labeled 'Value' (NOT for arguments labeled 'Register'), literals are allowed in the form [ARGx] (Number enclosed in brackets)
    The enum below specifies in a comment the behaviour of each proper instruction and the corresponding

    PSEUDO INSTRUCTIONS (THEY DO NOT GO IN FOR THE PROGRAM'S):
      bits Lx; Takes ONLY literal, it has to go at the start of every assembly program to determine the number of maximum bits of each argument

    SUBROUTINES (Labels, really):
      START Sx; Start of subroutine. Takes ANY string prefixed by underscore '_' (E.G. '_MYFUNC'). The assembler SHALL insert a 'jmp' instruction to the next (Non-subroutine) instruction in its place (So if it is encountered in normal execution, the whole subroutine up until the END is ignored). The assembler SHALL keep track of the number of the next (real) instruction

      END Sx; End of subroutine. Takes ANY 6-CHARACTER UPPERCASE STRING (A-Z) prefixed by underscore '_' (E.G. '_MYFUNC'). The assembler SHALL replace it with a 'jmp' instruction to an arbitrary (CPU address space) register that references an as of yet unknown part of the program

      CALL Sx; Start of subroutine. Takes ANY string prefixed by underscore '_' (E.G. '_MYFUNC'). The assembler SHALL replace it with two instructions: 
        1. a 'set' instruction that sets the same address assigned in 'END' to the number of the next (real) instruction
        2. a 'jmp' instruction to the start of the subroutine (the number kept track of in START)
    The objective of this is to have re-callable pieces of code that don't get in the way of normal programming
*/
typedef enum {
  /*THE INSTRUCTION COUNTING SHALL START AT 0, NOT 1*/
  /*IMPORTANT NOTE: LITERALS ARE ONLY ALLOWED IN REGISTER ARGUMENTS DOCUMENTED AS VALUES 'Vx', NOT IN REGISTERS 'Rx'*/
  halt = 0, /* ; halt*/
  nop = 1, /* ; do nothing*/
  set = 2, /* V1 R2; set address R2 to value V1*/
  jmp = 3, /* V1; hand execution to instruction numbered V1*/
  jcz = 4, /* V1; if ZF == 0, jmp to instrucion V1*/
  /*ADD trough to NOT: put result of doing 
  stuff with Vn values into Rn address*/
  add = 5, /* V1 V2 R3; addition, carry goes to carry flag*/
  sub = 6, /* V1 V2 R3; substraction, carry flag set if result is negative*/
  and = 7, /* V1 V2 R3; binary and*/
  or =  8,  /* V1 V2 R3; binary or*/
  xor = 9, /* V1 V2 R3; binary exclusive or*/
  not = 10, /* V1 R2; binary negation*/
  cmp = 11 /* V1 V2 ; if V1 is bigger than V2, sets the carry flag to 0 and the zero flag to 1
                      if V1 is smaller than V2, sets the carry flag to 1 and the zero flag to 0
                      if V1 is equal to V2, sets the carry flag to 0 and the zero flag to 0*/
} iset;

/*Execution memory*/
typedef struct {
  /*Every first byte*/
  iset inst[MEMSIZE]; 
  /*0x2000 to 0x2FFF*/

  /*Every second, third and fourth bytes*/
  /*int because we need this to be at least 16 bits by default*/
  uint opnd[MEMSIZE][4]; 
  /*The fourth row of the array indicates which arguments are addresses, trough its least significant 3 bits.
  E.G 000 ALL ADDRESSES -- 101 FIRST AND THIRD ARE LITERALS*/ 
  /*0x2000 to 0x2FFF*/
} xmem;

/*Data memory*/
typedef struct {
  /*Zero flag, 0xFFFF*/
  uchar zf; 

  /*Carry flag, 0xFFFE*/
  uchar cf; 

  /*Input register, read-only, 0xFFFD*/
  uchar in;

  /*Output register, write-only, 0xFFFC*/
  uchar ou;

  /*GEN mem, 0x0000 to 0x0FFF*/
  uchar gp[MEMSIZE]; 

  /*CPU mem, 0x1000 to 0x1FFF*/
  uchar cp[MEMSIZE]; 
} vmem;

typedef struct {
  xmem m1;
  vmem m2;
  uint co; 
  /*Instruction counter*/

  uchar hf; 
  /*Halt flag, 1 if it has halted.*/
} mem;


/*Nomagno's boring VM interface*/

extern mem fxmem(xmem code); /*Generate mem struct from xmem struct*/

extern uint execnext(mem *program); /*Execute one instruction from the program*/
