#include <halfworld/hwreq.h>
#include <halfworld/hwrat.h>
#include <halfworld/hwmath.h>

/*
Copyright Nomagno 2022

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

hwpoint HWMath_PointAdd(hwpoint a, hwpoint b){
	return (hwpoint){
		.x = a.x + b.x,
		.y = a.y + b.y
	};
}

hwpoint HWMath_PointSub(hwpoint a, hwpoint b){
	return (hwpoint){
		.x = a.x - b.x,
		.y = a.y - b.y
	};
}

hwpoint HWMath_PointMultScalar(hwpoint a, HWRat k){
	return (hwpoint){
		.x = HWRat_toUlong(HWRat_Product((HWRat){0, a.x, 1}, k)),
		.y = HWRat_toUlong(HWRat_Product((HWRat){0, a.y, 1}, k))
	};
}

hwumax HWMath_umax_isqrt(hwumax a){
	hwumax i = 1, sq = 1;
	while(sq <= a){
			i += 1;
			sq = i*i;
	}
	return i - 1;
}

hwulong HWMath_isqrt(hwulong a){
	hwulong i = 1, sq = 1;
	while(sq <= a){
			i += 1;
			sq = i*i;
	}
	return i - 1;
}

/*Newton's method*/
HWRat HWMath_ratsqrt(hwulong a){
	HWRat guess = HWRat_Simplify((HWRat){0, HWMath_isqrt(a), 1});
	for(unsigned i = 0; i < HWMATH_SQRT_ITER; i++){
		HWRat a_divided_by_guess = HWRat_Product((HWRat){0, a, 1}, (HWRat){0, guess.denom, guess.num});
		guess = HWRat_Product((HWRat){0, 1, 2}, HWRat_Add(guess, a_divided_by_guess));
		guess.sign = 0;
	}
	return guess;
}

HWRat HWMath_umax_ratsqrt(hwumax a){
	HWRat guess = HWRat_Simplify((HWRat){0, HWMath_isqrt(a), 1});
	for(unsigned i = 0; i < HWMATH_SQRT_ITER; i++){
		HWRat a_divided_by_guess = HWRat_Product((HWRat){0, a, 1}, (HWRat){0, guess.denom, guess.num});
		guess = HWRat_Product((HWRat){0, 1, 2}, HWRat_Add(guess, a_divided_by_guess));
		guess.sign = 0;
	}
	return guess;	
}

/*A(1-t)(1-t) + B(1-t)(2t) + C(t)(t)*/
hwpoint HWMath_getBezierPoint(hwbezier curve, HWRat t){
	hwpoint a = HWMath_PointMultScalar(curve.p1, 
	           HWRat_Product(
	           HWRat_Add((HWRat){0, 1, 1}, (HWRat){0, t.num, t.denom}),
	           HWRat_Add((HWRat){0, 1, 1}, (HWRat){0, t.num, t.denom})));
	hwpoint b = HWMath_PointMultScalar(curve.control, 
	           HWRat_Product(
	           HWRat_Add((HWRat){0, 1, 1}, (HWRat){0, t.num, t.denom}),
	           HWRat_Add(t, t)));
	hwpoint c = HWMath_PointMultScalar(curve.p3, HWRat_Product(t, t));

	a = HWMath_PointAdd(a, b);
	b = HWMath_PointAdd(a, c);
	return a;
}
