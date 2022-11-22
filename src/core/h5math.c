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

h5point_uint H5Math_uint_PointAddPoint(h5point_uint a, h5point_uint b){
	return (h5point_uint){
		.x = a.x + b.x,
		.y = a.y + b.y
	};
}
h5point_uint H5Math_uint_PointSubPoint(h5point_uint a, h5point_uint b){
	return (h5point_uint){
		.x = a.x - b.x,
		.y = a.y - b.y
	};
}
h5point_uint H5Math_uint_PointMultScalar(h5point_uint a, H5Rat_uint k){
	return (h5point_uint){
		.x = H5Rat_uint_toInt(H5Rat_uint_Product((H5Rat_uint){0, a.x, 1}, k)),
		.y = H5Rat_uint_toInt(H5Rat_uint_Product((H5Rat_uint){0, a.y, 1}, k))
	};
}

h5point_ulong H5Math_ulong_PointAddPoint(h5point_ulong a, h5point_ulong b){
	return (h5point_ulong){
		.x = a.x + b.x,
		.y = a.y + b.y
	};
}
h5point_ulong H5Math_ulong_PointSubPoint(h5point_ulong a, h5point_ulong b){
	return (h5point_ulong){
		.x = a.x - b.x,
		.y = a.y - b.y
	};
}
h5point_ulong H5Math_ulong_PointMultScalar(h5point_ulong a, H5Rat_ulong k){
	return (h5point_ulong){
		.x = H5Rat_ulong_toInt(H5Rat_ulong_Product((H5Rat_ulong){0, a.x, 1}, k)),
		.y = H5Rat_ulong_toInt(H5Rat_ulong_Product((H5Rat_ulong){0, a.y, 1}, k))
	};
}

h5point_umax H5Math_umax_PointAddPoint(h5point_umax a, h5point_umax b){
	return (h5point_umax){
		.x = a.x + b.x,
		.y = a.y + b.y
	};
}
h5point_umax H5Math_umax_PointSubPoint(h5point_umax a, h5point_umax b){
	return (h5point_umax){
		.x = a.x - b.x,
		.y = a.y - b.y
	};
}
h5point_umax H5Math_umax_PointMultScalar(h5point_umax a, H5Rat_umax k){
	return (h5point_umax){
		.x = H5Rat_umax_toInt(H5Rat_umax_Product((H5Rat_umax){0, a.x, 1}, k)),
		.y = H5Rat_umax_toInt(H5Rat_umax_Product((H5Rat_umax){0, a.y, 1}, k))
	};
}

h5uint H5Math_uint_IntegerSquareRoot(h5uint a){
	h5uint i = 1, sq = 1;
	while(sq <= a){
			i += 1;
			sq = i*i;
	}
	return i - 1;
}

h5ulong H5Math_ulong_IntegerSquareRoot(h5ulong a){
	h5ulong i = 1, sq = 1;
	while(sq <= a){
			i += 1;
			sq = i*i;
	}
	return i - 1;
}

h5umax H5Math_umax_IntegerSquareRoot(h5umax a){
	h5umax i = 1, sq = 1;
	while(sq <= a){
			i += 1;
			sq = i*i;
	}
	return i - 1;
}


/*Newton's method*/
H5Rat_uint H5Math_uint_RationalSquareRoot(h5uint a){
	H5Rat_uint guess = H5Rat_uint_Simplify((H5Rat_uint){0, H5Math_uint_IntegerSquareRoot(a), 1});
	for(unsigned i = 0; i < H5MATH_SQRT_ITER; i++){
		H5Rat_uint a_divided_by_guess = H5Rat_uint_Product((H5Rat_uint){0, a, 1},
		                                                     (H5Rat_uint){0, guess.denom, guess.num});
		guess = H5Rat_uint_Product((H5Rat_uint){0, 1, 2}, H5Rat_uint_Add(guess, a_divided_by_guess));
		guess.sign = 0;
	}
	return guess;
}

/*Newton's method*/
H5Rat_ulong H5Math_ulong_RationalSquareRoot(h5ulong a){
	H5Rat_ulong guess = H5Rat_ulong_Simplify((H5Rat_ulong){0, H5Math_ulong_IntegerSquareRoot(a), 1});
	for(unsigned i = 0; i < H5MATH_SQRT_ITER; i++){
		H5Rat_ulong a_divided_by_guess = H5Rat_ulong_Product((H5Rat_ulong){0, a, 1},
		                                                     (H5Rat_ulong){0, guess.denom, guess.num});
		guess = H5Rat_ulong_Product((H5Rat_ulong){0, 1, 2}, H5Rat_ulong_Add(guess, a_divided_by_guess));
		guess.sign = 0;
	}
	return guess;
}

/*Newton's method*/
H5Rat_umax H5Math_umax_RationalSquareRoot(h5umax a){
	H5Rat_umax guess = H5Rat_umax_Simplify((H5Rat_umax){0, H5Math_umax_IntegerSquareRoot(a), 1});
	for(unsigned i = 0; i < H5MATH_SQRT_ITER; i++){
		H5Rat_umax a_divided_by_guess = H5Rat_umax_Product((H5Rat_umax){0, a, 1},
		                                                     (H5Rat_umax){0, guess.denom, guess.num});
		guess = H5Rat_umax_Product((H5Rat_umax){0, 1, 2}, H5Rat_umax_Add(guess, a_divided_by_guess));
		guess.sign = 0;
	}
	return guess;
}

/*A(1-t)(1-t) + B(1-t)(2t) + C(t)(t)*/
h5point_uint H5Math_uint_getBezierPoint(h5bezier_uint curve, H5Rat_uint t){
	h5point_uint a = H5Math_uint_PointMultScalar(curve.p1,
	           H5Rat_uint_Product(
	           H5Rat_uint_Add((H5Rat_uint){0, 1, 1}, (H5Rat_uint){0, t.num, t.denom}),
	           H5Rat_uint_Add((H5Rat_uint){0, 1, 1}, (H5Rat_uint){0, t.num, t.denom})));
	h5point_uint b = H5Math_uint_PointMultScalar(curve.control,
	           H5Rat_uint_Product(
	           H5Rat_uint_Add((H5Rat_uint){0, 1, 1}, (H5Rat_uint){0, t.num, t.denom}),
	           H5Rat_uint_Add(t, t)));
	h5point_uint c = H5Math_uint_PointMultScalar(curve.p3, H5Rat_uint_Product(t, t));

	a = H5Math_uint_PointAddPoint(a, b);
	b = H5Math_uint_PointAddPoint(a, c);
	return a;
}

/*A(1-t)(1-t) + B(1-t)(2t) + C(t)(t)*/
h5point_ulong H5Math_ulong_getBezierPoint(h5bezier_ulong curve, H5Rat_ulong t){
	h5point_ulong a = H5Math_ulong_PointMultScalar(curve.p1,
	           H5Rat_ulong_Product(
	           H5Rat_ulong_Add((H5Rat_ulong){0, 1, 1}, (H5Rat_ulong){0, t.num, t.denom}),
	           H5Rat_ulong_Add((H5Rat_ulong){0, 1, 1}, (H5Rat_ulong){0, t.num, t.denom})));
	h5point_ulong b = H5Math_ulong_PointMultScalar(curve.control,
	           H5Rat_ulong_Product(
	           H5Rat_ulong_Add((H5Rat_ulong){0, 1, 1}, (H5Rat_ulong){0, t.num, t.denom}),
	           H5Rat_ulong_Add(t, t)));
	h5point_ulong c = H5Math_ulong_PointMultScalar(curve.p3, H5Rat_ulong_Product(t, t));

	a = H5Math_ulong_PointAddPoint(a, b);
	b = H5Math_ulong_PointAddPoint(a, c);
	return a;
}

/*A(1-t)(1-t) + B(1-t)(2t) + C(t)(t)*/
h5point_umax H5Math_umax_getBezierPoint(h5bezier_umax curve, H5Rat_umax t){
	h5point_umax a = H5Math_umax_PointMultScalar(curve.p1,
	           H5Rat_umax_Product(
	           H5Rat_umax_Add((H5Rat_umax){0, 1, 1}, (H5Rat_umax){0, t.num, t.denom}),
	           H5Rat_umax_Add((H5Rat_umax){0, 1, 1}, (H5Rat_umax){0, t.num, t.denom})));
	h5point_umax b = H5Math_umax_PointMultScalar(curve.control,
	           H5Rat_umax_Product(
	           H5Rat_umax_Add((H5Rat_umax){0, 1, 1}, (H5Rat_umax){0, t.num, t.denom}),
	           H5Rat_umax_Add(t, t)));
	h5point_umax c = H5Math_umax_PointMultScalar(curve.p3, H5Rat_umax_Product(t, t));

	a = H5Math_umax_PointAddPoint(a, b);
	b = H5Math_umax_PointAddPoint(a, c);
	return a;
}
