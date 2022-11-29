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

#include <halfive/h5req.h>
#include <halfive/h5math.h>

/*Composite point addition*/
h5point_uint H5Math_uint_PointAddPoint(h5point_uint a, h5point_uint b){
	return (h5point_uint){ .x = a.x + b.x, .y = a.y + b.y };
}
h5point_ulong H5Math_ulong_PointAddPoint(h5point_ulong a, h5point_ulong b){
	return (h5point_ulong){ .x = a.x + b.x, .y = a.y + b.y };
}
h5point_umax H5Math_umax_PointAddPoint(h5point_umax a, h5point_umax b){
	return (h5point_umax){ .x = a.x + b.x, .y = a.y + b.y };
}


/*Composite point substraction*/
h5point_uint H5Math_uint_PointSubPoint(h5point_uint a, h5point_uint b){
	return (h5point_uint){ .x = a.x - b.x, .y = a.y - b.y };
}
h5point_ulong H5Math_ulong_PointSubPoint(h5point_ulong a, h5point_ulong b){
	return (h5point_ulong){ .x = a.x - b.x, .y = a.y - b.y };
}
h5point_umax H5Math_umax_PointSubPoint(h5point_umax a, h5point_umax b){
	return (h5point_umax){ .x = a.x - b.x, .y = a.y - b.y };
}


/*Composite signed point addition*/
h5point_sint H5Math_sint_PointAddPoint(h5point_sint a, h5point_sint b){
	return (h5point_sint){ .x = a.x + b.x, .y = a.y + b.y };
}
h5point_slong H5Math_slong_PointAddPoint(h5point_slong a, h5point_slong b){
	return (h5point_slong){ .x = a.x + b.x, .y = a.y + b.y };
}
h5point_smax H5Math_smax_PointAddPoint(h5point_smax a, h5point_smax b){
	return (h5point_smax){ .x = a.x + b.x, .y = a.y + b.y };
}


/*Composite signed point substraction*/
h5point_sint H5Math_sint_PointSubPoint(h5point_sint a, h5point_sint b){
	return (h5point_sint){ .x = a.x - b.x, .y = a.y - b.y };
}
h5point_slong H5Math_slong_PointSubPoint(h5point_slong a, h5point_slong b){
	return (h5point_slong){ .x = a.x - b.x, .y = a.y - b.y };
}
h5point_smax H5Math_smax_PointSubPoint(h5point_smax a, h5point_smax b){
	return (h5point_smax){ .x = a.x - b.x, .y = a.y - b.y };
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

#ifdef FLOATS_SUPPORTED
/*Composite point scaling*/
h5point_uint H5Math_uint_PointMultScalar(h5point_uint a, h5float k){
	return (h5point_uint){ .x = a.x * ABS(k), .y = a.x * ABS(k) };
}
h5point_ulong H5Math_ulong_PointMultScalar(h5point_ulong a, h5float k){
	return (h5point_ulong){ .x = a.x * ABS(k), .y = a.x * ABS(k) };
}
h5point_umax H5Math_umax_PointMultScalar(h5point_umax a, h5float k){
	return (h5point_umax){ .x = a.x * ABS(k), .y = a.x * ABS(k) };
}


/*Composite signed point scaling*/
h5point_sint H5Math_sint_PointMultScalar(h5point_sint a, h5float k){
	return (h5point_sint){ .x = a.x * k, .y = a.x * k };
}
h5point_slong H5Math_slong_PointMultScalar(h5point_slong a, h5float k){
	return (h5point_slong){ .x = a.x * k, .y = a.x * k };
}
h5point_smax H5Math_smax_PointMultScalar(h5point_smax a, h5float k){
	return (h5point_smax){ .x = a.x * k, .y = a.x * k };
}


/*Newton's method*/
h5float H5Math_SquareRoot(h5float a){
	float guess = H5Math_uint_IntegerSquareRoot(a);
	for(unsigned i = 0; i < H5MATH_SQRT_ITER; i++){
		guess = (guess + (a/guess))/2;
		guess = ABS(guess);
	}
	return guess;
}


/*C + (1-t)(1-t)(P1-C) + (t^2)(P3-C)*/
h5point_uint H5Math_uint_getBezierPoint(h5bezier_uint curve, float t){
	h5point_uint sumpart1 = curve.control;
	h5point_uint sumpart2 = H5Math_uint_PointMultScalar(
	    H5Math_uint_PointSubPoint(curve.p1, curve.control),
	    (1-t)*(1-t)
	);
	h5point_uint sumpart3 = H5Math_uint_PointMultScalar(
	    H5Math_uint_PointSubPoint(curve.p3, curve.control),
	    (t*t)
	);

	return H5Math_uint_PointAddPoint(
	    H5Math_uint_PointAddPoint(sumpart1, sumpart2),
	    sumpart3
	);
}
h5point_ulong H5Math_ulong_getBezierPoint(h5bezier_ulong curve, float t){
	h5point_ulong sumpart1 = curve.control;
	h5point_ulong sumpart2 = H5Math_ulong_PointMultScalar(
	    H5Math_ulong_PointSubPoint(curve.p1, curve.control),
	    (1-t)*(1-t)
	);
	h5point_ulong sumpart3 = H5Math_ulong_PointMultScalar(
	    H5Math_ulong_PointSubPoint(curve.p3, curve.control),
	    (t*t)
	);

	return H5Math_ulong_PointAddPoint(
	    H5Math_ulong_PointAddPoint(sumpart1, sumpart2),
	    sumpart3
	);
}
h5point_umax H5Math_umax_getBezierPoint(h5bezier_umax curve, float t){
	h5point_umax sumpart1 = curve.control;
	h5point_umax sumpart2 = H5Math_umax_PointMultScalar(
	    H5Math_umax_PointSubPoint(curve.p1, curve.control),
	    (1-t)*(1-t)
	);
	h5point_umax sumpart3 = H5Math_umax_PointMultScalar(
	    H5Math_umax_PointSubPoint(curve.p3, curve.control),
	    (t*t)
	);

	return H5Math_umax_PointAddPoint(
	    H5Math_umax_PointAddPoint(sumpart1, sumpart2),
	    sumpart3
	);
}


/*Signed*/
h5point_sint H5Math_sint_getBezierPoint(h5bezier_sint curve, float t){
	h5point_sint sumpart1 = curve.control;
	h5point_sint sumpart2 = H5Math_sint_PointMultScalar(
	    H5Math_sint_PointSubPoint(curve.p1, curve.control),
	    (1-t)*(1-t)
	);
	h5point_sint sumpart3 = H5Math_sint_PointMultScalar(
	    H5Math_sint_PointSubPoint(curve.p3, curve.control),
	    (t*t)
	);

	return H5Math_sint_PointAddPoint(
	    H5Math_sint_PointAddPoint(sumpart1, sumpart2),
	    sumpart3
	);
}
h5point_slong H5Math_slong_getBezierPoint(h5bezier_slong curve, float t){
	h5point_slong sumpart1 = curve.control;
	h5point_slong sumpart2 = H5Math_slong_PointMultScalar(
	    H5Math_slong_PointSubPoint(curve.p1, curve.control),
	    (1-t)*(1-t)
	);
	h5point_slong sumpart3 = H5Math_slong_PointMultScalar(
	    H5Math_slong_PointSubPoint(curve.p3, curve.control),
	    (t*t)
	);

	return H5Math_slong_PointAddPoint(
	    H5Math_slong_PointAddPoint(sumpart1, sumpart2),
	    sumpart3
	);
}
h5point_smax H5Math_smax_getBezierPoint(h5bezier_smax curve, float t){
	h5point_smax sumpart1 = curve.control;
	h5point_smax sumpart2 = H5Math_smax_PointMultScalar(
	    H5Math_smax_PointSubPoint(curve.p1, curve.control),
	    (1-t)*(1-t)
	);
	h5point_smax sumpart3 = H5Math_smax_PointMultScalar(
	    H5Math_smax_PointSubPoint(curve.p3, curve.control),
	    (t*t)
	);

	return H5Math_smax_PointAddPoint(
	    H5Math_smax_PointAddPoint(sumpart1, sumpart2),
	    sumpart3
	);
}
#endif
