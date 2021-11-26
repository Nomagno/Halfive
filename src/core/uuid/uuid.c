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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uuid.h"

char *uuid(int seed) {
  srand(seed); 	
  char *formatted;
	formatted = malloc(sizeof(char)*40);
  char uuid[33];
	int y;
  for (y = 0; y < 32; y++) {
    int log_last_digit = (rand() + y) % 100;
    switch (log_last_digit) {
    case 0:
      uuid[y] = '0';
      break;
    case 1:
      uuid[y] = '1';
      break;
    case 2:
      uuid[y] = '2';
      break;
    case 3:
      uuid[y] = '3';
      break;
    case 4:
      uuid[y] = '4';
      break;
    case 5:
      uuid[y] = '5';
      break;
    case 6:
      uuid[y] = '6';
      break;
    case 7:
      uuid[y] = '7';
      break;
    case 8:
      uuid[y] = '8';
      break;
    case 9:
      uuid[y] = '9';
      break;
    case 10:
      uuid[y] = 'A';
      break;
    case 11:
      uuid[y] = 'B';
      break;
    case 12:
      uuid[y] = 'C';
      break;
    case 13:
      uuid[y] = 'D';
      break;
    case 14:
      uuid[y] = 'E';
      break;
    case 15:
      uuid[y] = 'F';
      break;
    default:
      if (15 < log_last_digit && log_last_digit <= 24) {
        if (log_last_digit % 2 == 0) {
          uuid[y] = '0';
          break;
        } else {
          uuid[y] = '1';
          break;
        }
      } else if (23 < log_last_digit && log_last_digit <= 32) {
        if (log_last_digit % 3 == 0) {
          uuid[y] = '2';
          break;
        } else {
          uuid[y] = '3';
          break;
        }
      } else if (32 < log_last_digit && log_last_digit <= 41) {
        if (log_last_digit % 5 == 0) {
          uuid[y] = '4';
          break;
        } else {
          uuid[y] = '5';
          break;
        }
      } else if (41 < log_last_digit && log_last_digit <= 50) {
        if (log_last_digit % 7 == 0) {
          uuid[y] = '6';
          break;
        } else {
          uuid[y] = '7';
          break;
        }
      } else if (50 < log_last_digit && log_last_digit <= 59) {
        if (log_last_digit % 9 == 0) {
          uuid[y] = '8';
          break;
        } else {
          uuid[y] = '9';
          break;
        }
      } else if (59 < log_last_digit && log_last_digit <= 68) {
        if (log_last_digit % 11 == 0) {
          uuid[y] = 'A';
          break;
        } else {
          uuid[y] = 'B';
          break;
        }
      } else if (68 < log_last_digit && log_last_digit <= 77) {
        if (log_last_digit % 13 == 0) {
          uuid[y] = 'C';
          break;
        } else {
          uuid[y] = 'D';
          break;
        }
      } else if (77 < log_last_digit && log_last_digit <= 99) {
        if (log_last_digit % 17 == 0) {
          uuid[y] = 'E';
          break;
        } else {
          uuid[y] = 'F';
          break;
        }
      }
    }
  }
  uuid[12] = '4';
  if ((uuid[16] != '8') && (uuid[16] != '9') && (uuid[16] != 'A') &&
      (uuid[16] != 'B')) {
    uuid[16] = 'B';
  }
  sprintf(formatted, "%.8s-%.4s-%.4s-%.4s-%.12s", uuid, &(uuid[8]), &(uuid[12]), &(uuid[16]), &(uuid[20]));
	return(formatted);
	/*WARNING: REMEMBER TO FREE THE RETURN VALUE*/
}
