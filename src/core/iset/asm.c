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
OF, OR IN CONNECTION WITH THE WORK OR THE USE OF OR OTHER DEALINGS IN THE
WORK.*/

#include "iset.h"
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
/*STDIO.H IS ONLY NEEDED FOR main()*/

uint _isxupdigit(uchar inchar) {
  if ((inchar == 'A') || (inchar == 'a') || (inchar == 'B') ||
      (inchar == 'b') || (inchar == 'C') || (inchar == 'c') ||
      (inchar == 'D') || (inchar == 'd') || (inchar == 'E') ||
      (inchar == 'e') || (inchar == 'F') || (inchar == 'f') ||
      (inchar == '0') || (inchar == '1') || (inchar == '2') ||
      (inchar == '3') || (inchar == '4') || (inchar == '5') ||
      (inchar == '6') || (inchar == '7') || (inchar == '8') || (inchar == '9'))
    return 1;
  else
    return 0;
}

iset _isinst(char *instr) {
  if (strcmp(instr, "halt") == 0)
    return halt;
  else if (strcmp(instr, "nop") == 0)
    return nop;
  else if (strcmp(instr, "set") == 0)
    return set;
  else if (strcmp(instr, "jmp") == 0)
    return jmp;
  else if (strcmp(instr, "jcz") == 0)
    return jcz;
  else if (strcmp(instr, "add") == 0)
    return add;
  else if (strcmp(instr, "sub") == 0)
    return sub;
  else if (strcmp(instr, "and") == 0)
    return and;
  else if (strcmp(instr, "or") == 0)
    return or ;
  else if (strcmp(instr, "xor") == 0)
    return xor;
  else if (strcmp(instr, "not") == 0)
    return not ;
  else if (strcmp(instr, "cmp") == 0)
    return cmp;
  /*THE SET OF INSTRUCTIONS 12, 13, 14, 15 CORRESPONDS TO THE
   * PSEUDO-INSTRUCTIONS {bits, START, END, CALL} AND SHALL BE PREPROCESSED
   * ACCORDINGLY*/
  else if (strcmp(instr, "bits") == 0)
    return 12;
  else if (strcmp(instr, "START") == 0)
    return 13;
  else if (strcmp(instr, "END") == 0)
    return 14;
  else if (strcmp(instr, "CALL") == 0)
    return 15;
  else
    return 16;
}

uint asmparse(char *linestr, iset *inst, uint opnds[4]) {
  char *token = strtok(linestr, " ");
  iset myinst;
  int i = 0;
  while ((token != NULL) && (i < 3)) {

    if ((myinst = _isinst(token)) != 16) {
      *inst = myinst;
    } else if (_isxupdigit(token[0])) {
      opnds[i] = (uint)strtoul(token, NULL, 16);
      i += 1;
    } else if (token[0] == '[') {
      token[0] = token[1];
      token[1] = '_';
      opnds[i] = (uint)strtoul(token, NULL, 16);

      opnds[3] = (opnds[3] | (1 << (2 - i)));

      i += 1;
    } else if (myinst > 11) {
      return 1; /*THERE IS STUFF TO PREPROCESS*/
    } else {
      return 2; /*CATASTROPHIC ERROR*/
    }
    token = strtok(NULL, " ");
  }
  return 0;
}

int main(int argc, char**argv) {
  if(argc < 1)
    return 1;
  char arr[30];
  FILE *inputfile = fopen(argv[1], "r");
  xmem code = {0};
  int i = 0;
  while (fscanf(inputfile, "%[^\n] ", arr) != EOF) {
    asmparse(arr, &code.inst[i], code.opnd[i]);
    i+=1;
  }
/*  printf("1: %i\n2: %i\n3: %i\n4: %i\n1_: %i\n2_: %i\n3_: %i\n4_: %i\n",
  code.opnd[0][0], code.opnd[0][1], code.opnd[0][2], code.opnd[0][3], 
  code.opnd[1][0], code.opnd[1][1], code.opnd[1][2], code.opnd[1][3]);
*/
  mem prog = fxmem(code);
  int errno = 0;
  printf("PROGRAM OUTPUT:\n");
  while((!prog.hf) && (!errno))
    errno = execnext(&prog);
  return errno;
}
