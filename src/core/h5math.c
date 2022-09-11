#include <halfive/h5req.h>
#include <halfive/h5rat.h>
#include <halfive/h5math.h>

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

h5point_ulong H5Math_PointAdd(h5point_ulong a, h5point_ulong b){
	return (h5point_ulong){
		.x = a.x + b.x,
		.y = a.y + b.y
	};
}

h5point_ulong H5Math_PointSub(h5point_ulong a, h5point_ulong b){
	return (h5point_ulong){
		.x = a.x - b.x,
		.y = a.y - b.y
	};
}

h5point_ulong H5Math_PointMultScalar(h5point_ulong a, H5Rat k){
	return (h5point_ulong){
		.x = H5Rat_toUlong(H5Rat_Product((H5Rat){0, a.x, 1}, k)),
		.y = H5Rat_toUlong(H5Rat_Product((H5Rat){0, a.y, 1}, k))
	};
}

h5umax H5Math_umax_isqrt(h5umax a){
	h5umax i = 1, sq = 1;
	while(sq <= a){
			i += 1;
			sq = i*i;
	}
	return i - 1;
}

h5ulong H5Math_isqrt(h5ulong a){
	h5ulong i = 1, sq = 1;
	while(sq <= a){
			i += 1;
			sq = i*i;
	}
	return i - 1;
}

/*Newton's method*/
H5Rat H5Math_ratsqrt(h5ulong a){
	H5Rat guess = H5Rat_Simplify((H5Rat){0, H5Math_isqrt(a), 1});
	for(unsigned i = 0; i < H5MATH_SQRT_ITER; i++){
		H5Rat a_divided_by_guess = H5Rat_Product((H5Rat){0, a, 1}, (H5Rat){0, guess.denom, guess.num});
		guess = H5Rat_Product((H5Rat){0, 1, 2}, H5Rat_Add(guess, a_divided_by_guess));
		guess.sign = 0;
	}
	return guess;
}

H5Rat H5Math_umax_ratsqrt(h5umax a){
	H5Rat guess = H5Rat_Simplify((H5Rat){0, H5Math_isqrt(a), 1});
	for(unsigned i = 0; i < H5MATH_SQRT_ITER; i++){
		H5Rat a_divided_by_guess = H5Rat_Product((H5Rat){0, a, 1}, (H5Rat){0, guess.denom, guess.num});
		guess = H5Rat_Product((H5Rat){0, 1, 2}, H5Rat_Add(guess, a_divided_by_guess));
		guess.sign = 0;
	}
	return guess;	
}

/*A(1-t)(1-t) + B(1-t)(2t) + C(t)(t)*/
h5point_ulong H5Math_getBezierPoint(h5bezier curve, H5Rat t){
	h5point_ulong a = H5Math_PointMultScalar(curve.p1, 
	           H5Rat_Product(
	           H5Rat_Add((H5Rat){0, 1, 1}, (H5Rat){0, t.num, t.denom}),
	           H5Rat_Add((H5Rat){0, 1, 1}, (H5Rat){0, t.num, t.denom})));
	h5point_ulong b = H5Math_PointMultScalar(curve.control, 
	           H5Rat_Product(
	           H5Rat_Add((H5Rat){0, 1, 1}, (H5Rat){0, t.num, t.denom}),
	           H5Rat_Add(t, t)));
	h5point_ulong c = H5Math_PointMultScalar(curve.p3, H5Rat_Product(t, t));

	a = H5Math_PointAdd(a, b);
	b = H5Math_PointAdd(a, c);
	return a;
}
