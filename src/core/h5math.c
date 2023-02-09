/*
Copyright Nomagno 2022, 2023

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

/*Composite vector addition*/
VEC2(h5uint) H5Math_uint_vecAddVec(VEC2(h5uint) a, VEC2(h5uint) b)
{
	return (VEC2(h5uint)){.x = a.x + b.x, .y = a.y + b.y};
}
VEC2(h5ulong) H5Math_ulong_vecAddVec(VEC2(h5ulong) a, VEC2(h5ulong) b)
{
	return (VEC2(h5ulong)){.x = a.x + b.x, .y = a.y + b.y};
}
VEC2(h5umax) H5Math_umax_vecAddVec(VEC2(h5umax) a, VEC2(h5umax) b)
{
	return (VEC2(h5umax)){.x = a.x + b.x, .y = a.y + b.y};
}

/*Composite vector substraction*/
VEC2(h5uint) H5Math_uint_vecSubVec(VEC2(h5uint) a, VEC2(h5uint) b)
{
	return (VEC2(h5uint)){.x = a.x - b.x, .y = a.y - b.y};
}
VEC2(h5ulong) H5Math_ulong_vecSubVec(VEC2(h5ulong) a, VEC2(h5ulong) b)
{
	return (VEC2(h5ulong)){.x = a.x - b.x, .y = a.y - b.y};
}
VEC2(h5umax) H5Math_umax_vecSubVec(VEC2(h5umax) a, VEC2(h5umax) b)
{
	return (VEC2(h5umax)){.x = a.x - b.x, .y = a.y - b.y};
}

/*Composite signed vector addition*/
VEC2(h5sint) H5Math_sint_vecAddVec(VEC2(h5sint) a, VEC2(h5sint) b)
{
	return (VEC2(h5sint)){.x = a.x + b.x, .y = a.y + b.y};
}
VEC2(h5slong) H5Math_slong_vecAddVec(VEC2(h5slong) a, VEC2(h5slong) b)
{
	return (VEC2(h5slong)){.x = a.x + b.x, .y = a.y + b.y};
}
VEC2(h5smax) H5Math_smax_vecAddVec(VEC2(h5smax) a, VEC2(h5smax) b)
{
	return (VEC2(h5smax)){.x = a.x + b.x, .y = a.y + b.y};
}

/*Composite signed vector substraction*/
VEC2(h5sint) H5Math_sint_vecSubVec(VEC2(h5sint) a, VEC2(h5sint) b)
{
	return (VEC2(h5sint)){.x = a.x - b.x, .y = a.y - b.y};
}
VEC2(h5slong) H5Math_slong_vecSubVec(VEC2(h5slong) a, VEC2(h5slong) b)
{
	return (VEC2(h5slong)){.x = a.x - b.x, .y = a.y - b.y};
}
VEC2(h5smax) H5Math_smax_vecSubVec(VEC2(h5smax) a, VEC2(h5smax) b)
{
	return (VEC2(h5smax)){.x = a.x - b.x, .y = a.y - b.y};
}

h5uint H5Math_uint_integerSquareRoot(h5uint a)
{
	h5uint i = 1, sq = 1;
	while (sq <= a) {
		i += 1;
		sq = i * i;
	}
	return i - 1;
}
h5ulong H5Math_ulong_integerSquareRoot(h5ulong a)
{
	h5ulong i = 1, sq = 1;
	while (sq <= a) {
		i += 1;
		sq = i * i;
	}
	return i - 1;
}
h5umax H5Math_umax_integerSquareRoot(h5umax a)
{
	h5umax i = 1, sq = 1;
	while (sq <= a) {
		i += 1;
		sq = i * i;
	}
	return i - 1;
}

#ifdef FLOATS_SUPPORTED
/*Composite vector scaling*/
VEC2(h5uint) H5Math_uint_vecMultScalar(VEC2(h5uint) a, h5float k)
{
	return (VEC2(h5uint)){.x = a.x * h5_abs(k), .y = a.x * h5_abs(k)};
}
VEC2(h5ulong) H5Math_ulong_vecMultScalar(VEC2(h5ulong) a, h5float k)
{
	return (VEC2(h5ulong)){.x = a.x * h5_abs(k), .y = a.x * h5_abs(k)};
}
VEC2(h5umax) H5Math_umax_vecMultScalar(VEC2(h5umax) a, h5float k)
{
	return (VEC2(h5umax)){.x = a.x * h5_abs(k), .y = a.x * h5_abs(k)};
}

/*Composite signed vector scaling*/
VEC2(h5sint) H5Math_sint_vecMultScalar(VEC2(h5sint) a, h5float k)
{
	return (VEC2(h5sint)){.x = a.x * k, .y = a.x * k};
}
VEC2(h5slong) H5Math_slong_vecMultScalar(VEC2(h5slong) a, h5float k)
{
	return (VEC2(h5slong)){.x = a.x * k, .y = a.x * k};
}
VEC2(h5smax) H5Math_smax_vecMultScalar(VEC2(h5smax) a, h5float k)
{
	return (VEC2(h5smax)){.x = a.x * k, .y = a.x * k};
}

/*Newton's method*/
h5float H5Math_squareRoot(h5float a)
{
	float guess = H5Math_uint_integerSquareRoot(a);
	for (unsigned i = 0; i < H5MATH_SQRT_ITER; i++) {
		guess = (guess + (a / guess)) / 2;
		guess = h5_abs(guess);
	}
	return guess;
}

/*C + (1-t)(1-t)(P1-C) + (t^2)(P3-C)*/
VEC2(h5uint) H5Math_uint_getBezierPoint(h5bezier_uint curve, float t)
{
	VEC2(h5uint) sumpart1 = curve.control;
	VEC2(h5uint) sumpart2 = H5Math_uint_vecMultScalar(
		H5Math_uint_vecSubVec(curve.p1, curve.control), (1 - t) * (1 - t));
	VEC2(h5uint) sumpart3 = H5Math_uint_vecMultScalar(
		H5Math_uint_vecSubVec(curve.p3, curve.control), (t * t));

	return H5Math_uint_vecAddVec(
		H5Math_uint_vecAddVec(sumpart1, sumpart2), sumpart3);
}
VEC2(h5ulong) H5Math_ulong_getBezierPoint(h5bezier_ulong curve, float t)
{
	VEC2(h5ulong) sumpart1 = curve.control;
	VEC2(h5ulong) sumpart2 = H5Math_ulong_vecMultScalar(
		H5Math_ulong_vecSubVec(curve.p1, curve.control), (1 - t) * (1 - t));
	VEC2(h5ulong) sumpart3 = H5Math_ulong_vecMultScalar(
		H5Math_ulong_vecSubVec(curve.p3, curve.control), (t * t));

	return H5Math_ulong_vecAddVec(
		H5Math_ulong_vecAddVec(sumpart1, sumpart2), sumpart3);
}
VEC2(h5umax) H5Math_umax_getBezierPoint(h5bezier_umax curve, float t)
{
	VEC2(h5umax) sumpart1 = curve.control;
	VEC2(h5umax) sumpart2 = H5Math_umax_vecMultScalar(
		H5Math_umax_vecSubVec(curve.p1, curve.control), (1 - t) * (1 - t));
	VEC2(h5umax) sumpart3 = H5Math_umax_vecMultScalar(
		H5Math_umax_vecSubVec(curve.p3, curve.control), (t * t));

	return H5Math_umax_vecAddVec(
		H5Math_umax_vecAddVec(sumpart1, sumpart2), sumpart3);
}

/*Signed*/
VEC2(h5sint) H5Math_sint_getBezierPoint(h5bezier_sint curve, float t)
{
	VEC2(h5sint) sumpart1 = curve.control;
	VEC2(h5sint) sumpart2 = H5Math_sint_vecMultScalar(
		H5Math_sint_vecSubVec(curve.p1, curve.control), (1 - t) * (1 - t));
	VEC2(h5sint) sumpart3 = H5Math_sint_vecMultScalar(
		H5Math_sint_vecSubVec(curve.p3, curve.control), (t * t));

	return H5Math_sint_vecAddVec(
		H5Math_sint_vecAddVec(sumpart1, sumpart2), sumpart3);
}
VEC2(h5slong) H5Math_slong_getBezierPoint(h5bezier_slong curve, float t)
{
	VEC2(h5slong) sumpart1 = curve.control;
	VEC2(h5slong) sumpart2 = H5Math_slong_vecMultScalar(
		H5Math_slong_vecSubVec(curve.p1, curve.control), (1 - t) * (1 - t));
	VEC2(h5slong) sumpart3 = H5Math_slong_vecMultScalar(
		H5Math_slong_vecSubVec(curve.p3, curve.control), (t * t));

	return H5Math_slong_vecAddVec(
		H5Math_slong_vecAddVec(sumpart1, sumpart2), sumpart3);
}
VEC2(h5smax) H5Math_smax_getBezierPoint(h5bezier_smax curve, float t)
{
	VEC2(h5smax) sumpart1 = curve.control;
	VEC2(h5smax) sumpart2 = H5Math_smax_vecMultScalar(
		H5Math_smax_vecSubVec(curve.p1, curve.control), (1 - t) * (1 - t));
	VEC2(h5smax) sumpart3 = H5Math_smax_vecMultScalar(
		H5Math_smax_vecSubVec(curve.p3, curve.control), (t * t));

	return H5Math_smax_vecAddVec(
		H5Math_smax_vecAddVec(sumpart1, sumpart2), sumpart3);
}

h5slong h5_abs(h5slong a)
{
	if (a < 0)
		return -a;
	else
		return a;
}
#endif

