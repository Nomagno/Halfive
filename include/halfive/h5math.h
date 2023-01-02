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

/*Composite 2D vector*/
typedef struct {
    h5umax x;
    h5umax y;
} h5point_umax;

typedef struct {
    h5ulong x;
    h5ulong y;
} h5point_ulong;

typedef struct {
    h5uint x;
    h5uint y;
} h5point_uint;
/*Composite signed 2D vector*/
typedef struct {
    h5smax x;
    h5smax y;
} h5point_smax;

typedef struct {
    h5slong x;
    h5slong y;
} h5point_slong;

typedef struct {
    h5sint x;
    h5sint y;
} h5point_sint;

/*Composite bezier curve*/
typedef struct {
    h5point_umax p1;
    h5point_umax control;
    h5point_umax p3;
} h5bezier_umax;

typedef struct {
    h5point_ulong p1;
    h5point_ulong control;
    h5point_ulong p3;
} h5bezier_ulong;

typedef struct {
    h5point_uint p1;
    h5point_uint control;
    h5point_uint p3;
} h5bezier_uint;
/*Composite signed bezier curve*/
typedef struct {
    h5point_smax p1;
    h5point_smax control;
    h5point_smax p3;
} h5bezier_smax;

typedef struct {
    h5point_slong p1;
    h5point_slong control;
    h5point_slong p3;
} h5bezier_slong;

typedef struct {
    h5point_sint p1;
    h5point_sint control;
    h5point_sint p3;
} h5bezier_sint;

h5point_uint
H5Math_uint_pointAddPoint(h5point_uint a,
			  h5point_uint b); /*Composite point addition*/
h5point_ulong
H5Math_ulong_pointAddPoint(h5point_ulong a,
			   h5point_ulong b); /*Composite point addition*/
h5point_umax
H5Math_umax_pointAddPoint(h5point_umax a,
			  h5point_umax b); /*Composite point addition*/

h5point_sint
H5Math_sint_pointAddPoint(h5point_sint a,
			  h5point_sint b); /*Composite signed point addition*/
h5point_slong
H5Math_slong_pointAddPoint(h5point_slong a,
			   h5point_slong b); /*Composite signed point addition*/
h5point_smax
H5Math_smax_pointAddPoint(h5point_smax a,
			  h5point_smax b); /*Composite signed point addition*/

h5point_uint
H5Math_uint_pointSubPoint(h5point_uint a,
			  h5point_uint b); /*Composite point substraction*/
h5point_ulong
H5Math_ulong_pointSubPoint(h5point_ulong a,
			   h5point_ulong b); /*Composite point substraction*/
h5point_umax
H5Math_umax_pointSubPoint(h5point_umax a,
			  h5point_umax b); /*Composite point substraction*/

h5point_sint H5Math_sint_pointSubPoint(
    h5point_sint a, h5point_sint b); /*Composite signed point substraction*/
h5point_slong H5Math_slong_pointSubPoint(
    h5point_slong a, h5point_slong b); /*Composite signed point substraction*/
h5point_smax H5Math_smax_pointSubPoint(
    h5point_smax a, h5point_smax b); /*Composite signed point substraction*/

h5uint
H5Math_uint_integerSquareRoot(h5uint a); /*Integer square root of h5uint*/
h5ulong
H5Math_ulong_integerSquareRoot(h5ulong a); /*Integer square root of h5ulong*/
h5umax
H5Math_umax_integerSquareRoot(h5umax a); /*Integer square root of h5umax*/

h5slong h5_abs(h5slong a);

#define H5_ABS(n) (((n) < 0) ? (-(n)) : (n))
#define CEIL(n) ((h5smax)((n) + (((n) < 0) ? (1) : (-1))))
#define CAP(n, cap) (((n) < (cap + 1)) ? (n) : (cap))
#define FLOOR(n) ((h5smax)(n))

#ifdef FLOATS_SUPPORTED
h5point_uint
H5Math_uint_pointMultScalar(h5point_uint a,
			    h5float k); /*Multiply h5point_uint by scalar [k]*/
h5point_ulong H5Math_ulong_pointMultScalar(
    h5point_ulong a, h5float k); /*Multiply h5point_ulong by scalar [k]*/
h5point_umax
H5Math_umax_pointMultScalar(h5point_umax a,
			    h5float k); /*Multiply h5point_umax by scalar [k]*/

h5point_sint
H5Math_sint_pointMultScalar(h5point_sint a,
			    h5float k); /*Multiply h5point_sint by scalar [k]*/
h5point_slong H5Math_slong_pointMultScalar(
    h5point_slong a, h5float k); /*Multiply h5point_slong by scalar [k]*/
h5point_smax
H5Math_smax_pointMultScalar(h5point_smax a,
			    h5float k); /*Multiply h5point_smax by scalar [k]*/

/*Number of iterations to do with Newton's method, 5 by default*/
#ifndef H5MATH_SQRT_ITER
#define H5MATH_SQRT_ITER 5
#endif
h5float H5Math_squareRoot(h5float a); /*Floating point square root*/

/*Calculate point [t] of bezier curve, where  [t] is a number between zero and
 * one*/
h5point_uint H5Math_uint_getBezierPoint(h5bezier_uint curve, h5float t);
h5point_ulong H5Math_ulong_getBezierPoint(h5bezier_ulong curve, h5float t);
h5point_umax H5Math_umax_getBezierPoint(h5bezier_umax curve, h5float t);

/*Calculate point [t] of signed bezier curve, where  [t] is a number between
 * zero and one*/
h5point_sint H5Math_sint_getBezierPoint(h5bezier_sint curve, h5float t);
h5point_slong H5Math_slong_getBezierPoint(h5bezier_slong curve, h5float t);
h5point_smax H5Math_smax_getBezierPoint(h5bezier_smax curve, h5float t);
#endif
#endif
