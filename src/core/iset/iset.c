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

#define UADDR 0xFFF0
/*Unknown address*/

/*
addrcheck() returns the actual address space of the argument address

0xFFF0: UNKNOWN ADDRESS SPACE
1: GENERAL PURPOSE MEMORY
2: CENTRAL PROCESSING UNIT MEMORY
3: ZERO FLAG
4: CARRY FLAG
5: INPUT REGISTER (READ-ONLY)
6: OUTPUT REGISTER (WRITE-ONLY)
*/
unsigned int addrcheck(unsigned int arg){
  if(arg < 0x1FFF) /*(MEMSIZE*2)*/ {
  if(arg < 0x0FFF) /*MEMSIZE*/
    return 1;
  return 2;
  }
  else if (arg == 0xFFFF)
    return 3;
  else if (arg == 0xFFFE)
    return 4;
  else if (arg == 0xFFFD)
    return 5;
  else if (arg == 0xFFFC)
    return 6;
  else
    return 0;
}

unsigned int addrconvert(unsigned int arg, unsigned int addr) {
  switch(arg){
    case 0:
      return UADDR;
    case 1:
      return addr;
    case 2:
     return (addr - 0x0FFF);
    case 3:
     return 0;
    case 4:
     return 0;
    case 5:
     return 0;
    case 6:
     return 0;
    default:
      return UADDR;
  }
}

void hnop(void){
  return;
}

unsigned int hset(unsigned int op[4], vmem *space){
  unsigned int ad1;
  unsigned int ad2;

  unsigned int conv1;
  unsigned int conv2;

  unsigned int val;

  switch (op[3]){
    case 0:
      ad1 = addrcheck(op[0]);
      ad2 = addrcheck(op[1]);

      conv1 = addrconvert(ad1, op[0]);
      conv2 = addrconvert(ad2, op[1]);
      switch(ad1){
        case 1: ;
          val = space->gp[conv1];
          break;
        case 2: ;
          val = space->cp[conv1];
          break;
        case 3: ;
          val = space->zf;
          break;
        case 4: ;
          val = space->cf;
          break;
        case 5: ;
          val = space->in;
          break;
        case 6: ;
          val = space->ou;
          break;
        default:
          return 1; /*ERROR, WE DON'T KNOW WHAT THAT ADDRESS MEANS*/
      }

      switch(ad2){
        case 1: ;
          space->gp[conv2] = val;
          break;
        case 2: ;
          space->cp[conv2] = val;          
          break;
        case 3: ;
          space->zf = val;          
          break;
        case 4: ;
          space->cf = val;          
          break;
        case 5: ;
          space->in = val;          
          break;
        case 6: ;
          space->ou = val;          
          #ifdef EOF
          printf("%i\n", val);
          #endif
          break;
        default:
          return 1; /*ERROR, WE DON'T KNOW WHAT THAT ADDRESS MEANS*/
      }
      break;
    case 4:
      val = op[0];
      ad2 = addrcheck(op[1]);

      conv2 = addrconvert(ad2, op[1]);
      switch(ad2){
        case 1: ;
          space->gp[conv2] = val;
          break;
        case 2: ;
          space->cp[conv2] = val;          
          break;
        case 3: ;
          space->zf = val;          
          break;
        case 4: ;
          space->cf = val;          
          break;
        case 5: ;
          space->in = val;          
          break;
        case 6: ;
          space->ou = val;          
          #ifdef EOF
          printf("%i\n", val);
          #endif
          break;
        default:
          return 1; /*ERROR, WE DON'T KNOW WHAT THAT ADDRESS MEANS*/
      }
      break;
    default:
      return 1; /*ERROR, THE INSTRUCTION IS WRONG*/
  }
  return 2; /*SOMETHING WENT TERRIBLY WRONG*/
}

mem fxmem(xmem code) {
  mem memory = {0};
  memory.m1 = code;
  memory.m2.zf = 1;
  return memory;
}

int execnext(mem *program){
  if (program->co < MEMSIZE){
    switch (program->m1.inst[program->co]) {
      case halt:
        program->hf = 1;
        program->co += 1;
        #ifdef EOF
        printf("HALT\n");
        #endif
        return 0;
      case nop:
        program->co += 1;
        return 0;
      case set:
        hset(program->m1.opnd[program->co], &program->m2);
        program->co += 1;
        return 0;
      case jmp:
        program->co = program->m1.opnd[program->co][0];
        return 0;
      case jcz:
        if (!program->m2.zf)
          program->co = program->m1.opnd[program->co][0];
        return 0;
      case add:
        break;
      case sub:
        break;
      case and:
        break;
      case or:
        break;
      case xor:
        break;
      case not:
        break;
      case cmp:
        break;
    };
  }
  #if defined(EOF)
    printf("UNIMPLEMENTED\n");
  #endif
  return -1; /*UNIMPLEMENTED*/
}
