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

/*Halfive Mathematics library*/

#ifndef H5MATH_H
#define H5MATH_H

#include <halfive/h5req.h>

#define MAKEVEC2(type) typedef struct { type x; type y; } vec2_##type
#define VEC2(type) vec2_##type

#define MAKEVEC3(type) typedef struct { type x; type y; type z; } vec3_##type
#define VEC3(type) vec3_##type


MAKEVEC2(h5uint);
MAKEVEC2(h5ulong);
MAKEVEC2(h5umax);

MAKEVEC2(h5sint);
MAKEVEC2(h5slong);
MAKEVEC2(h5smax);

/*Composite bezier curve*/
typedef struct {
	VEC2(h5umax) p1;
	VEC2(h5umax) control;
	VEC2(h5umax) p3;
} h5bezier_umax;

typedef struct {
	VEC2(h5ulong) p1;
	VEC2(h5ulong) control;
	VEC2(h5ulong) p3;
} h5bezier_ulong;

typedef struct {
	VEC2(h5uint) p1;
	VEC2(h5uint) control;
	VEC2(h5uint) p3;
} h5bezier_uint;
/*Composite signed bezier curve*/
typedef struct {
	VEC2(h5smax) p1;
	VEC2(h5smax) control;
	VEC2(h5smax) p3;
} h5bezier_smax;

typedef struct {
	VEC2(h5slong) p1;
	VEC2(h5slong) control;
	VEC2(h5slong) p3;
} h5bezier_slong;

typedef struct {
	VEC2(h5sint) p1;
	VEC2(h5sint) control;
	VEC2(h5sint) p3;
} h5bezier_sint;

VEC2(h5uint) H5Math_uint_vecAddVec(
	VEC2(h5uint) a, VEC2(h5uint) b); /*Composite vector addition*/
VEC2(h5ulong) H5Math_ulong_vecAddVec(
	VEC2(h5ulong) a, VEC2(h5ulong) b); /*Composite vector addition*/
VEC2(h5umax) H5Math_umax_vecAddVec(
	VEC2(h5umax) a, VEC2(h5umax) b); /*Composite vector addition*/

VEC2(h5sint) H5Math_sint_vecAddVec(
	VEC2(h5sint) a, VEC2(h5sint) b); /*Composite signed vector addition*/
VEC2(h5slong) H5Math_slong_vecAddVec(
	VEC2(h5slong) a, VEC2(h5slong) b); /*Composite signed vector addition*/
VEC2(h5smax) H5Math_smax_vecAddVec(
	VEC2(h5smax) a, VEC2(h5smax) b); /*Composite signed vector addition*/

VEC2(h5uint) H5Math_uint_vecSubVec(
	VEC2(h5uint) a, VEC2(h5uint) b); /*Composite vector substraction*/
VEC2(h5ulong) H5Math_ulong_vecSubVec(
	VEC2(h5ulong) a, VEC2(h5ulong) b); /*Composite vector substraction*/
VEC2(h5umax) H5Math_umax_vecSubVec(
	VEC2(h5umax) a, VEC2(h5umax) b); /*Composite vector substraction*/

VEC2(h5sint) H5Math_sint_vecSubVec(
	VEC2(h5sint) a, VEC2(h5sint) b); /*Composite signed vector substraction*/
VEC2(h5slong) H5Math_slong_vecSubVec(
	VEC2(h5slong) a, VEC2(h5slong) b); /*Composite signed vector substraction*/
VEC2(h5smax) H5Math_smax_vecSubVec(
	VEC2(h5smax) a, VEC2(h5smax) b); /*Composite signed vector substraction*/

h5uint H5Math_uint_integerSquareRoot(
	h5uint a); /*Integer square root of h5uint*/
h5ulong H5Math_ulong_integerSquareRoot(
	h5ulong a); /*Integer square root of h5ulong*/
h5umax H5Math_umax_integerSquareRoot(
	h5umax a); /*Integer square root of h5umax*/

h5slong h5_abs(h5slong a);

#define H5_ABS(n)	(((n) < 0) ? (-(n)) : (n))
#define CEIL(n)		((h5smax)((n) + (((n) < 0) ? (1) : (-1))))
#define CAP(n, cap) (((n) < (cap + 1)) ? (n) : (cap))
#define FLOOR(n)	((h5smax)(n))

#ifdef FLOATS_SUPPORTED
VEC2(h5uint) H5Math_uint_vecMultScalar(
	VEC2(h5uint) a, h5float k); /*Multiply VEC2(h5uint) by scalar [k]*/
VEC2(h5ulong) H5Math_ulong_vecMultScalar(
	VEC2(h5ulong) a, h5float k); /*Multiply VEC2(h5ulong) by scalar [k]*/
VEC2(h5umax) H5Math_umax_vecMultScalar(
	VEC2(h5umax) a, h5float k); /*Multiply VEC2(h5umax) by scalar [k]*/

VEC2(h5sint) H5Math_sint_vecMultScalar(
	VEC2(h5sint) a, h5float k); /*Multiply VEC2(h5sint) by scalar [k]*/
VEC2(h5slong) H5Math_slong_vecMultScalar(
	VEC2(h5slong) a, h5float k); /*Multiply VEC2(h5slong) by scalar [k]*/
VEC2(h5smax) H5Math_smax_vecMultScalar(
	VEC2(h5smax) a, h5float k); /*Multiply VEC2(h5smax) by scalar [k]*/

/*Number of iterations to do with Newton's method, 5 by default*/
#ifndef H5MATH_SQRT_ITER
#define H5MATH_SQRT_ITER 5
#endif
h5float H5Math_squareRoot(h5float a); /*Floating point square root*/

/*Calculate point [t] of bezier curve, where  [t] is a number between zero and
 * one*/
VEC2(h5uint) H5Math_uint_getBezierPoint(h5bezier_uint curve, h5float t);
VEC2(h5ulong) H5Math_ulong_getBezierPoint(h5bezier_ulong curve, h5float t);
VEC2(h5umax) H5Math_umax_getBezierPoint(h5bezier_umax curve, h5float t);

/*Calculate point [t] of signed bezier curve, where  [t] is a number between
 * zero and one*/
VEC2(h5sint) H5Math_sint_getBezierPoint(h5bezier_sint curve, h5float t);
VEC2(h5slong) H5Math_slong_getBezierPoint(h5bezier_slong curve, h5float t);
VEC2(h5smax) H5Math_smax_getBezierPoint(h5bezier_smax curve, h5float t);
#endif
#endif
