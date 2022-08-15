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
#ifndef HWSTRING_H
#define HWSTRING_H
#include <halfworld/hwreq.h>
#define N (UCHAR_MAX + 1)
typedef unsigned char suchar;
typedef unsigned suint;
typedef unsigned long sulong;

#if !defined(HWLIBC_HOSTED)
	#define hwstrtoul _hwlibcstrtoul
	#define hwstrcmp  _hwlibcstrcmp
	#define hwstrtok  _hwlibcstrtok
	#define hwmemcpy  _hwlibcmemcpy
	#define hwmemset  _hwlibcmemset

	#define hwmalloc  _ERROR_UNIMPLEMENTED
	#define hwrealloc _ERROR_UNIMPLEMENTED 
	#define hwfree    _ERROR_UNIMPLEMENTED
#else
	#define hwstrtoul strtoul
	#define hwstrcmp  strcmp
	#define hwstrtok  strtok
	#define hwmemcpy  memcpy
	#define hwmemset  memset

	#define hwmalloc  malloc
	#define hwrealloc realloc
	#define hwfree    free
#endif

extern sulong _hwlibcstrtoul(char *, char **, int);
extern int _hwlibcstrcmp(char *, char *);
extern char *_hwlibcstrtok(char *, char *);
void *_hwlibcmemcpy(void *dest, const void *src, size_t n);
void *_hwlibcmemset(void *str, int val, size_t n);
#endif
