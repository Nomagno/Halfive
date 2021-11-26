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

#include "iset.h"
#include <stdio.h>
/*<stdio.h> IS NOT NEEDED, IT ONLY ADDS 
PRINTING AND TESTING FUNCTIONALITY*/

int main(void){
  /*
  NOTE A STEP IS NOT NECESSARILY ONE INSTRUCTION
  Test: 
  0-Write the value of address 10 to output register
  1-NOP
  2-JMP two steps ahead
  3-Write the addition of the value address 10 and 
  the literal '12' to output register AND address 10 (This one is skipped first run)
  4-CMP address 10 to 0
  5-JCZ two steps back (step 3) if the previous comparison was true
  6-Write the SUB of '7' and '2' to the output register
  7-Write the bitwise OR of '7' and '2' to the output register
  8-Write the bitwise AND of '7' and '2' to the output register
  9-Write the bitwise XOR of '7' and '2' to the output register
  10-Write the NOT of '7' to the output register
  11-HALT
  */
  xmem mycode = {0};

  /*START OF PROGRAM*/
  /*STEP 0*/
  mycode.inst[0] = set;
  mycode.opnd[0][0] = 9;
  mycode.opnd[0][1] = 0xFFFC;

  /*STEP 1*/
  mycode.inst[1] = nop;

  /*STEP 2*/
  mycode.inst[2] = jmp;
  mycode.opnd[2][0] = 5;

  /*STEP 3*/
  mycode.inst[3] = add;
  mycode.opnd[3][0] = 12;
  mycode.opnd[3][1] = 9;
  mycode.opnd[3][2] = 9;
  mycode.opnd[3][3] = 4;

  mycode.inst[4] = set;
  mycode.opnd[4][0] = 9;
  mycode.opnd[4][1] = 0xFFFC;

  /*STEP 4*/
  mycode.inst[5] = cmp;
  mycode.opnd[5][0] = 9;
  mycode.opnd[5][1] = 0;
  mycode.opnd[5][3] = 2;

  /*STEP 5*/
  mycode.inst[6] = jcz;
  mycode.opnd[6][0] = 3;

  /*STEP 6*/
  mycode.inst[7] = sub;
  mycode.opnd[7][0] = 7;
  mycode.opnd[7][1] = 2;
  mycode.opnd[7][2] = 0xFFFC;
  mycode.opnd[7][3] = 6;

  /*STEP 7*/
  mycode.inst[8] = or;
  mycode.opnd[8][0] = 7;
  mycode.opnd[8][1] = 2;
  mycode.opnd[8][2] = 0xFFFC;
  mycode.opnd[8][3] = 6;

  /*STEP 8*/
  mycode.inst[9] = and;
  mycode.opnd[9][0] = 7;
  mycode.opnd[9][1] = 2;
  mycode.opnd[9][2] = 0xFFFC;
  mycode.opnd[9][3] = 6;

  /*STEP 9*/
  mycode.inst[10] = xor;
  mycode.opnd[10][0] = 7;
  mycode.opnd[10][1] = 2;
  mycode.opnd[10][2] = 0xFFFC;
  mycode.opnd[10][3] = 6;

  /*STEP 10*/
  mycode.inst[11] = not;
  mycode.opnd[11][0] = 7;
  mycode.opnd[11][1] = 2;
  mycode.opnd[11][2] = 0xFFFC;
  mycode.opnd[11][3] = 6;

  /*STEP 11*/
  mycode.inst[12] = halt;
  /*END OF PROGRAM*/

  /*Generate full VM from program*/
  mem prog = fxmem(mycode);

  int myinput;

  #if defined(EOF)
    printf("Enter value for address 10 of general purpose memory (1 character):\n");
    myinput = getchar();
  #else
    #if defined(BRANCHOUT) /*DEFINE BRANCHOUT TO RUN BOTH TESTPATHS WITHOUT STDLIB*/
      myinput = 0;
    #else
      myinput = 4;
    #endif          
  #endif

  prog.m2.gp[9] = myinput;
  int err = 0;
  while((!prog.hf) && (err == 0))
    err = execnext(&prog);
  return 0;
}
