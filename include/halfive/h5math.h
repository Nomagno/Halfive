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

MAKEVEC3(h5uint);
MAKEVEC3(h5ulong);
MAKEVEC3(h5umax);
MAKEVEC3(h5sint);
MAKEVEC3(h5slong);
MAKEVEC3(h5smax);

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

#define CEIL(n)		((h5smax)((n) + (((n) < 0) ? (1) : (-1))))
#define CAP(n, cap) (((n) < (cap + 1)) ? (n) : (cap))
#define FLOOR(n)	((h5smax)(n))

#ifdef FLOATS_SUPPORTED
MAKEVEC2(h5float);

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
