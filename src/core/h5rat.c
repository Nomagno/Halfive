#include <halfive/h5rat.h>

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

void H5Rat_XorSwap(h5umax *a, h5umax *b)
{
	*a = *a ^ *b;
	*b = *a ^ *b;
	*a = *a ^ *b;
	return;
}

h5umax H5Rat_GCD(h5umax a, h5umax b)
{
	h5umax tmp;
	while (b != 0){
		if (b > a)
			H5Rat_XorSwap(&a, &b);
		else
			tmp = b,
			b = a % b,
			a = tmp;
	}			
		return a;
}

h5umax H5Rat_LCM(h5umax a, h5umax b) { return ((a * b) / H5Rat_GCD(a, b)); }

H5Rat H5Rat_Simplify(H5Rat a)
{
	h5umax gcd = H5Rat_GCD(a.num, a.denom);
	return (H5Rat){a.sign, a.num / gcd, a.denom / gcd};
}

void H5Rat_Equate(H5Rat *a, H5Rat *b)
{
	*a = H5Rat_Simplify(*a);
	*b = H5Rat_Simplify(*b);

	h5umax lcm = H5Rat_LCM(a->denom, b->denom);
	h5umax amult = lcm / a->denom;
	h5umax bmult = lcm / b->denom;

	a->num *= amult;
	a->denom *= amult;
	b->num *= bmult;
	b->denom *= bmult;
}

_Bool H5Rat_Compare(H5Rat a, H5Rat b) { return (a.num * b.denom == a.denom * b.num); }

H5Rat H5Rat_Add(H5Rat a, H5Rat b)
{
	H5Rat_Equate(&a, &b);
	H5Rat result;
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
			H5Rat_XorSwap(&a.num, &b.num);
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

H5Rat H5Rat_Product(H5Rat a, H5Rat b)
{
	a = H5Rat_Simplify(a);
	b = H5Rat_Simplify(b);

	return H5Rat_Simplify(
	    (H5Rat){(a.sign != b.sign), a.num * b.num, a.denom * b.denom});
}

h5ulong H5Rat_toUlong(H5Rat a){
	return a.denom / a.num;
}


#ifdef FLOATS_SUPPORTED
float H5Rat_toFloat(H5Rat a)
{
	a = H5Rat_Simplify(a);
	return (a.sign) ? (a.num / a.denom) : (-a.num / a.denom);
}
#endif
