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
	#define h5strtok  _h5libcstrtok
	#define h5memcpy  _h5libcmemcpy
	#define h5strncpy  _h5libcstrncpy
	#define h5memset  _h5libcmemset

	#define h5malloc  _ERROR_UNIMPLEMENTED
	#define h5realloc _ERROR_UNIMPLEMENTED
	#define h5free    _ERROR_UNIMPLEMENTED
#else
	#define h5strtoul strtoul
	#define h5strcmp  strcmp
	#define h5strtok  strtok
	#define h5memcpy  memcpy
	#define h5strncpy  strncpy
	#define h5memset  memset

	#define h5malloc  malloc
	#define h5realloc realloc
	#define h5free    free
#endif

extern sulong _h5libcstrtoul(char *, char **, int);
extern int _h5libcstrcmp(char *, char *);
extern char *_h5libcstrtok(char *, char *);

void *_h5libcmemcpy(void *dest, const void *src, size_t n);
char *_h5libcstrncpy(char *dest, const char *src, size_t n);
void *_h5libcmemset(void *str, int val, size_t n);

#endif
