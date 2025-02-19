/*
Copyright Nomagno 2021, 2022, 2023

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice (including the next
paragraph) shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
*/

#include <halfive/h5req.h>
#include <halfive/h5rat.h>

void H5Rat_uint_xorSwap(h5uint *a, h5uint *b)
{
	*a = *a ^ *b;
	*b = *a ^ *b;
	*a = *a ^ *b;
	return;
}
void H5Rat_ulong_xorSwap(h5ulong *a, h5ulong *b)
{
	*a = *a ^ *b;
	*b = *a ^ *b;
	*a = *a ^ *b;
	return;
}
void H5Rat_umax_xorSwap(h5umax *a, h5umax *b)
{
	*a = *a ^ *b;
	*b = *a ^ *b;
	*a = *a ^ *b;
	return;
}

h5uint H5Rat_uint_gcd(h5uint a, h5uint b)
{
	h5uint tmp;
	while (b != 0) {
		if (b > a)
			H5Rat_uint_xorSwap(&a, &b);
		else
			tmp = b, b = a % b, a = tmp;
	}
	return a;
}
h5ulong H5Rat_ulong_gcd(h5ulong a, h5ulong b)
{
	h5ulong tmp;
	while (b != 0) {
		if (b > a)
			H5Rat_ulong_xorSwap(&a, &b);
		else
			tmp = b, b = a % b, a = tmp;
	}
	return a;
}
h5umax H5Rat_umax_gcd(h5umax a, h5umax b)
{
	h5umax tmp;
	while (b != 0) {
		if (b > a)
			H5Rat_umax_xorSwap(&a, &b);
		else
			tmp = b, b = a % b, a = tmp;
	}
	return a;
}

h5uint H5Rat_uint_lcm(h5uint a, h5uint b)
{
	return ((a * b) / H5Rat_uint_gcd(a, b));
}
h5ulong H5Rat_ulong_lcm(h5ulong a, h5ulong b)
{
	return ((a * b) / H5Rat_ulong_gcd(a, b));
}
h5umax H5Rat_umax_lcm(h5umax a, h5umax b)
{
	return ((a * b) / H5Rat_umax_gcd(a, b));
}

H5Rat_uint H5Rat_uint_simplify(H5Rat_uint a)
{
	h5uint gcd = H5Rat_uint_gcd(a.num, a.denom);
	return (H5Rat_uint){a.sign, a.num / gcd, a.denom / gcd};
}
H5Rat_ulong H5Rat_ulong_simplify(H5Rat_ulong a)
{
	h5ulong gcd = H5Rat_ulong_gcd(a.num, a.denom);
	return (H5Rat_ulong){a.sign, a.num / gcd, a.denom / gcd};
}
H5Rat_umax H5Rat_umax_simplify(H5Rat_umax a)
{
	h5umax gcd = H5Rat_umax_gcd(a.num, a.denom);
	return (H5Rat_umax){a.sign, a.num / gcd, a.denom / gcd};
}

void H5Rat_uint_equate(H5Rat_uint *a, H5Rat_uint *b)
{
	*a = H5Rat_uint_simplify(*a);
	*b = H5Rat_uint_simplify(*b);

	h5uint lcm	 = H5Rat_uint_lcm(a->denom, b->denom);
	h5uint amult = lcm / a->denom;
	h5uint bmult = lcm / b->denom;

	a->num *= amult;
	a->denom *= amult;
	b->num *= bmult;
	b->denom *= bmult;
}
void H5Rat_ulong_equate(H5Rat_ulong *a, H5Rat_ulong *b)
{
	*a = H5Rat_ulong_simplify(*a);
	*b = H5Rat_ulong_simplify(*b);

	h5ulong lcm	  = H5Rat_ulong_lcm(a->denom, b->denom);
	h5ulong amult = lcm / a->denom;
	h5ulong bmult = lcm / b->denom;

	a->num *= amult;
	a->denom *= amult;
	b->num *= bmult;
	b->denom *= bmult;
}
void H5Rat_umax_equate(H5Rat_umax *a, H5Rat_umax *b)
{
	*a = H5Rat_umax_simplify(*a);
	*b = H5Rat_umax_simplify(*b);

	h5umax lcm	 = H5Rat_umax_lcm(a->denom, b->denom);
	h5umax amult = lcm / a->denom;
	h5umax bmult = lcm / b->denom;

	a->num *= amult;
	a->denom *= amult;
	b->num *= bmult;
	b->denom *= bmult;
}

_Bool H5Rat_uint_compare(H5Rat_uint a, H5Rat_uint b)
{
	return (a.num * b.denom == a.denom * b.num);
}
_Bool H5Rat_ulong_compare(H5Rat_ulong a, H5Rat_ulong b)
{
	return (a.num * b.denom == a.denom * b.num);
}
_Bool H5Rat_umax_compare(H5Rat_umax a, H5Rat_umax b)
{
	return (a.num * b.denom == a.denom * b.num);
}

H5Rat_uint H5Rat_uint_add(H5Rat_uint a, H5Rat_uint b)
{
	H5Rat_uint_equate(&a, &b);
	H5Rat_uint result;
	if (a.sign && b.sign) {
		result.sign	 = 1;
		result.num	 = a.num + b.num;
		result.denom = a.denom;
	} else if (!a.sign && !b.sign) {
		result.sign	 = 0;
		result.num	 = a.num + b.num;
		result.denom = a.denom;
	} else {
		if (a.sign) {
			a.sign = 0;
			b.sign = 1;
			H5Rat_uint_xorSwap(&a.num, &b.num);
		}
		result.num	 = a.num - b.num;
		result.denom = a.denom;
		result.sign	 = 0;
		if (result.num > a.num) {
			result.num = ~result.num;
			result.num += 1;
			result.sign = 1;
		}
	}
	return result;
}
H5Rat_ulong H5Rat_ulong_add(H5Rat_ulong a, H5Rat_ulong b)
{
	H5Rat_ulong_equate(&a, &b);
	H5Rat_ulong result;
	if (a.sign && b.sign) {
		result.sign	 = 1;
		result.num	 = a.num + b.num;
		result.denom = a.denom;
	} else if (!a.sign && !b.sign) {
		result.sign	 = 0;
		result.num	 = a.num + b.num;
		result.denom = a.denom;
	} else {
		if (a.sign) {
			a.sign = 0;
			b.sign = 1;
			H5Rat_ulong_xorSwap(&a.num, &b.num);
		}
		result.num	 = a.num - b.num;
		result.denom = a.denom;
		result.sign	 = 0;
		if (result.num > a.num) {
			result.num = ~result.num;
			result.num += 1;
			result.sign = 1;
		}
	}
	return result;
}
H5Rat_umax H5Rat_umax_add(H5Rat_umax a, H5Rat_umax b)
{
	H5Rat_umax_equate(&a, &b);
	H5Rat_umax result;
	if (a.sign && b.sign) {
		result.sign	 = 1;
		result.num	 = a.num + b.num;
		result.denom = a.denom;
	} else if (!a.sign && !b.sign) {
		result.sign	 = 0;
		result.num	 = a.num + b.num;
		result.denom = a.denom;
	} else {
		if (a.sign) {
			a.sign = 0;
			b.sign = 1;
			H5Rat_umax_xorSwap(&a.num, &b.num);
		}
		result.num	 = a.num - b.num;
		result.denom = a.denom;
		result.sign	 = 0;
		if (result.num > a.num) {
			result.num = ~result.num;
			result.num += 1;
			result.sign = 1;
		}
	}
	return result;
}

H5Rat_uint H5Rat_uint_product(H5Rat_uint a, H5Rat_uint b)
{
	a = H5Rat_uint_simplify(a);
	b = H5Rat_uint_simplify(b);

	return H5Rat_uint_simplify(
		(H5Rat_uint){(a.sign != b.sign), a.num * b.num, a.denom * b.denom});
}
H5Rat_ulong H5Rat_ulong_product(H5Rat_ulong a, H5Rat_ulong b)
{
	a = H5Rat_ulong_simplify(a);
	b = H5Rat_ulong_simplify(b);

	return H5Rat_ulong_simplify(
		(H5Rat_ulong){(a.sign != b.sign), a.num * b.num, a.denom * b.denom});
}
H5Rat_umax H5Rat_umax_product(H5Rat_umax a, H5Rat_umax b)
{
	a = H5Rat_umax_simplify(a);
	b = H5Rat_umax_simplify(b);

	return H5Rat_umax_simplify(
		(H5Rat_umax){(a.sign != b.sign), a.num * b.num, a.denom * b.denom});
}

h5uint H5Rat_uint_convertToInt(H5Rat_uint a) { return a.denom / a.num; }
h5ulong H5Rat_ulong_convertToInt(H5Rat_ulong a) { return a.denom / a.num; }
h5uint H5Rat_umax_convertToInt(H5Rat_uint a) { return a.denom / a.num; }

h5sint H5Rat_sint_convertToInt(H5Rat_uint a)
{
	a = H5Rat_uint_simplify(a);
	return (a.sign == POS_SIGN) ? (a.denom / a.num) : (-a.denom / a.num);
}
h5slong H5Rat_slong_convertToInt(H5Rat_ulong a)
{
	a = H5Rat_ulong_simplify(a);
	return (a.sign == POS_SIGN) ? (a.denom / a.num) : (-a.denom / a.num);
}
h5smax H5Rat_smax_convertToInt(H5Rat_umax a)
{
	a = H5Rat_umax_simplify(a);
	return (a.sign == POS_SIGN) ? (a.denom / a.num) : (-a.denom / a.num);
}

H5Rat_umax H5Rat_UtoM(H5Rat_uint a)
{
	return (H5Rat_umax){a.sign, a.num, a.denom};
}
H5Rat_umax H5Rat_LtoM(H5Rat_ulong a)
{
	return (H5Rat_umax){a.sign, a.num, a.denom};
}

H5Rat_ulong H5Rat_UtoL(H5Rat_uint a)
{
	return (H5Rat_ulong){a.sign, a.num, a.denom};
}
H5Rat_ulong H5Rat_MtoL(H5Rat_umax a)
{
	return (H5Rat_ulong){a.sign, a.num, a.denom};
}

H5Rat_uint H5Rat_LtoU(H5Rat_ulong a)
{
	return (H5Rat_uint){a.sign, a.num, a.denom};
}
H5Rat_uint H5Rat_MtoU(H5Rat_umax a)
{
	return (H5Rat_uint){a.sign, a.num, a.denom};
}

#ifdef FLOATS_SUPPORTED
float H5Rat_uint_convertToFloat(H5Rat_uint a)
{
	a = H5Rat_uint_simplify(a);
	return (a.sign == POS_SIGN) ? (a.num / a.denom) : (-a.num / a.denom);
}

float H5Rat_ulong_convertToFloat(H5Rat_ulong a)
{
	a = H5Rat_ulong_simplify(a);
	return (a.sign == POS_SIGN) ? (a.num / a.denom) : (-a.num / a.denom);
}

float H5Rat_umax_convertToFloat(H5Rat_umax a)
{
	a = H5Rat_umax_simplify(a);
	return (a.sign == POS_SIGN) ? (a.num / a.denom) : (-a.num / a.denom);
}
#endif
