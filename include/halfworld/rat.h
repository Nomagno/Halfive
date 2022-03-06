#include <stdint.h>

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

typedef uintmax_t uRatInt;

typedef struct {
	_Bool sign;
	uRatInt num;
	uRatInt denom;
} Rat;


#define GCD Rat_GCD
#define LCM Rat_LCM
#define RSwap Rat_XorSwap
#define RSimp Rat_Simplify
#define REq Rat_Equate
#define RCmp Rat_Compare
#define RAdd Rat_Add
#define RProd Rat_Product
#define RFloat Rat_toFloat

uRatInt Rat_GCD(uRatInt a, uRatInt b);
uRatInt Rat_LCM(uRatInt a, uRatInt b);
void Rat_XorSwap(uRatInt *a, uRatInt *b);

Rat Rat_Simplify(Rat a);
void Rat_Equate(Rat *a, Rat *b);
_Bool Rat_Compare(Rat a, Rat b);
Rat Rat_Add(Rat a, Rat b);
Rat Rat_Product(Rat a, Rat b);

#ifdef FLOATS_SUPPORTED
float Rat_toFloat(Rat a);
#endif
