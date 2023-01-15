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

/*Halfive freestanding Standard Library*/

#ifndef H5STRING_H
#define H5STRING_H

#include <halfive/h5req.h>

#define N (UCHAR_MAX + 1)

typedef unsigned char suchar;
typedef unsigned suint;
typedef unsigned long sulong;

#if !defined(H5LIBC_HOSTED)
#define h5strtoul _h5libcstrtoul
#define h5strcmp  _h5libcstrcmp
#define h5streq	  _h5libstreq
#define h5strtok  _h5libcstrtok
#define h5memcpy  _h5libcmemcpy
#define h5strncpy _h5libcstrncpy
#define h5memset  _h5libcmemset

#define h5malloc  _ERROR_UNIMPLEMENTED
#define h5realloc _ERROR_UNIMPLEMENTED
#define h5free	  _ERROR_UNIMPLEMENTED
#else
#define h5strtoul strtoul
#define h5strcmp  strcmp
#define h5streq	  _h5libstreq
#define h5strtok  strtok
#define h5memcpy  memcpy
#define h5strncpy strncpy
#define h5memset  memset

#define h5malloc  malloc
#define h5realloc realloc
#define h5free	  free
#endif

extern sulong _h5libcstrtoul(char *, char **, int);
extern int _h5libcstrcmp(char *, char *);
extern _Bool _h5libstreq(char *, char *);
extern char *_h5libcstrtok(char *, char *);

void *_h5libcmemcpy(void *dest, const void *src, size_t n);
char *_h5libcstrncpy(char *dest, const char *src, size_t n);
void *_h5libcmemset(void *str, int val, size_t n);

#define H5_IS_DEC_DIGIT(x)                                                   \
	(x == '0' || x == '1' || x == '2' || x == '3' || x == '4' || x == '5' || \
		x == '6' || x == '7' || x == '8' || x == '9')

#define H5_IS_HEX_DIGIT_UPPER(x)                                      \
	(ISDECDIGIT(x) || x == 'A' || x == 'B' || x == 'C' || x == 'D' || \
		x == 'E' || x == 'F')

#define H5_IS_HEX_DIGIT_LOWER(x)                                      \
	(ISDECDIGIT(x) || x == 'a' || x == 'b' || x == 'c' || x == 'd' || \
		x == 'e' || x == 'f')

#define H5_IS_HEX_DIGIT(x)                                            \
	(ISDECDIGIT(x) || x == 'A' || x == 'B' || x == 'C' || x == 'D' || \
		x == 'E' || x == 'F' || x == 'a' || x == 'b' || x == 'c' ||   \
		x == 'd' || x == 'e' || x == 'f')

#define H5_IS_LETTER_UPPER(x)                                                \
	(x == 'A' || x == 'B' || x == 'C' || x == 'D' || x == 'E' || x == 'F' || \
		x == 'G' || x == 'H' || x == 'I' || x == 'J' || x == 'K' ||          \
		x == 'L' || x == 'M' || x == 'N' || x == 'O' || x == 'P' ||          \
		x == 'Q' || x == 'R' || x == 'S' || x == 'T' || x == 'U' ||          \
		x == 'V' || x == 'W' || x == 'X' || x == 'Y' || x == 'Z')

#define H5_IS_LETTER_LOWER(x)                                                \
	(x == 'a' || x == 'b' || x == 'c' || x == 'd' || x == 'e' || x == 'f' || \
		x == 'g' || x == 'h' || x == 'i' || x == 'j' || x == 'k' ||          \
		x == 'l' || x == 'm' || x == 'n' || x == 'o' || x == 'p' ||          \
		x == 'q' || x == 'r' || x == 's' || x == 't' || x == 'u' ||          \
		x == 'v' || x == 'w' || x == 'x' || x == 'y' || x == 'z')

#endif
