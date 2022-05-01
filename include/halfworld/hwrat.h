#ifndef HWRAT_H
#define HWRAT_H

#include <halfworld/hwreq.h>

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

typedef struct {
	_Bool sign;
	hwumax num;
	hwumax denom;
} HWRat;

hwumax HWRat_GCD(hwumax a, hwumax b);
hwumax HWRat_LCM(hwumax a, hwumax b);
void HWRat_XorSwap(hwumax *a, hwumax *b);

HWRat HWRat_Simplify(HWRat a);
void HWRat_Equate(HWRat *a, HWRat *b);
_Bool HWRat_Compare(HWRat a, HWRat b);
HWRat HWRat_Add(HWRat a, HWRat b);
HWRat HWRat_Product(HWRat a, HWRat b);


hwulong HWRat_toUlong(HWRat a);
#ifdef FLOATS_SUPPORTED
	float HWRat_toFloat(HWRat a);
#endif

#ifdef HWRAT_SHORTHAND
	#define GCD HWRat_GCD
	#define LCM HWRat_LCM
	#define RSwap HWRat_XorSwap
	#define RSimp HWRat_Simplify
	#define REq HWRat_Equate
	#define RCmp HWRat_Compare
	#define RAdd HWRat_Add
	#define RProd HWRat_Product
	#define RUlong HWRat_toUlong
	#define RFloat HWRat_toFloat
#endif
#endif
