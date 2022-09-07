#ifndef H5RAT_H
#define H5RAT_H

#include <halfive/h5req.h>

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
	h5umax num;
	h5umax denom;
} H5Rat;

h5umax H5Rat_GCD(h5umax a, h5umax b);
h5umax H5Rat_LCM(h5umax a, h5umax b);
void H5Rat_XorSwap(h5umax *a, h5umax *b);

H5Rat H5Rat_Simplify(H5Rat a);
void H5Rat_Equate(H5Rat *a, H5Rat *b);
_Bool H5Rat_Compare(H5Rat a, H5Rat b);
H5Rat H5Rat_Add(H5Rat a, H5Rat b);
H5Rat H5Rat_Product(H5Rat a, H5Rat b);


h5ulong H5Rat_toUlong(H5Rat a);
#ifdef FLOATS_SUPPORTED
	float H5Rat_toFloat(H5Rat a);
#endif

#ifdef H5RAT_SHORTHAND
	#define GCD H5Rat_GCD
	#define LCM H5Rat_LCM
	#define RSwap H5Rat_XorSwap
	#define RSimp H5Rat_Simplify
	#define REq H5Rat_Equate
	#define RCmp H5Rat_Compare
	#define RAdd H5Rat_Add
	#define RProd H5Rat_Product
	#define RUlong H5Rat_toUlong
	#define RFloat H5Rat_toFloat
#endif
#endif
