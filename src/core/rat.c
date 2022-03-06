#include <halfworld/rat.h>

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

void Rat_XorSwap(uRatInt *a, uRatInt *b)
{
	*a = *a ^ *b;
	*b = *a ^ *b;
	*a = *a ^ *b;
	return;
}

uRatInt Rat_GCD(uRatInt a, uRatInt b)
{
	if (b > a)
		Rat_XorSwap(&a, &b);
	if (b == 0)
		return a;
	else
		return Rat_GCD(b, a % b);
}

uRatInt Rat_LCM(uRatInt a, uRatInt b) { return ((a * b) / Rat_GCD(a, b)); }

Rat Rat_Simplify(Rat a)
{
	uRatInt gcd = Rat_GCD(a.num, a.denom);
	return (Rat){a.sign, a.num / gcd, a.denom / gcd};
}

void Rat_Equate(Rat *a, Rat *b)
{
	*a = Rat_Simplify(*a);
	*b = Rat_Simplify(*b);

	uRatInt lcm = Rat_LCM(a->denom, b->denom);
	uRatInt amult = lcm / a->denom;
	uRatInt bmult = lcm / b->denom;

	a->num *= amult;
	a->denom *= amult;
	b->num *= bmult;
	b->denom *= bmult;
}

_Bool Rat_Compare(Rat a, Rat b) { return (a.num * b.denom == a.denom * b.num); }

Rat Rat_Add(Rat a, Rat b)
{
	Rat_Equate(&a, &b);
	Rat result;
	if (a.sign && b.sign) {
		result.sign = 1;
		result.num = a.num + b.num;
		result.denom = a.denom;
	} else if (!a.sign && !b.sign) {
		result.sign = 0;
		result.num = a.num + b.num;
		result.denom = a.denom;
	} else {
		if (a.sign) {
			a.sign = 0;
			b.sign = 1;
			Rat_XorSwap(&a.num, &b.num);
		}
		result.num = a.num - b.num;
		result.denom = a.denom;
		result.sign = 0;
		if (result.num > a.num) {
			result.num = ~result.num;
			result.num += 1;
			result.sign = 1;
		}
	}
	return result;
}

Rat Rat_Product(Rat a, Rat b)
{
	a = Rat_Simplify(a);
	b = Rat_Simplify(b);

	return Rat_Simplify(
	    (Rat){(a.sign != b.sign), a.num * b.num, a.denom * b.denom});
}

#ifdef FLOATS_SUPPORTED
float Rat_toFloat(Rat a)
{
	a = Rat_Simplify(a);
	return (a.sign) ? (a.num / a.denom) : (-a.num / a.denom);
}
#endif
