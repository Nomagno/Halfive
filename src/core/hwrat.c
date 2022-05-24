#include <halfworld/hwrat.h>

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

void HWRat_XorSwap(hwumax *a, hwumax *b)
{
	*a = *a ^ *b;
	*b = *a ^ *b;
	*a = *a ^ *b;
	return;
}

hwumax HWRat_GCD(hwumax a, hwumax b)
{
	hwumax tmp;
	while (b != 0){
		if (b > a)
			HWRat_XorSwap(&a, &b);
		else
			tmp = b,
			b = a % b,
			a = tmp;
	}			
		return a;
}

hwumax HWRat_LCM(hwumax a, hwumax b) { return ((a * b) / HWRat_GCD(a, b)); }

HWRat HWRat_Simplify(HWRat a)
{
	hwumax gcd = HWRat_GCD(a.num, a.denom);
	return (HWRat){a.sign, a.num / gcd, a.denom / gcd};
}

void HWRat_Equate(HWRat *a, HWRat *b)
{
	*a = HWRat_Simplify(*a);
	*b = HWRat_Simplify(*b);

	hwumax lcm = HWRat_LCM(a->denom, b->denom);
	hwumax amult = lcm / a->denom;
	hwumax bmult = lcm / b->denom;

	a->num *= amult;
	a->denom *= amult;
	b->num *= bmult;
	b->denom *= bmult;
}

_Bool HWRat_Compare(HWRat a, HWRat b) { return (a.num * b.denom == a.denom * b.num); }

HWRat HWRat_Add(HWRat a, HWRat b)
{
	HWRat_Equate(&a, &b);
	HWRat result;
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
			HWRat_XorSwap(&a.num, &b.num);
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

HWRat HWRat_Product(HWRat a, HWRat b)
{
	a = HWRat_Simplify(a);
	b = HWRat_Simplify(b);

	return HWRat_Simplify(
	    (HWRat){(a.sign != b.sign), a.num * b.num, a.denom * b.denom});
}

hwulong HWRat_toUlong(HWRat a){
	return a.denom / a.num;
}


#ifdef FLOATS_SUPPORTED
float HWRat_toFloat(HWRat a)
{
	a = HWRat_Simplify(a);
	return (a.sign) ? (a.num / a.denom) : (-a.num / a.denom);
}
#endif
