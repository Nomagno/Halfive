#ifndef H5MATH_H
#define H5MATH_H

#include <halfive/h5req.h>
#include <halfive/h5rat.h>

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

#define H5MATH_MAKESIGN(type, name) typedef struct { _Bool sign; type num; } name

H5MATH_MAKESIGN(h5uchar, h5schar);
H5MATH_MAKESIGN(h5uint, h5sint);
H5MATH_MAKESIGN(h5ulong, h5slong);
H5MATH_MAKESIGN(h5umax, h5smax);

/*Composite 2D vector*/
typedef struct {
	h5ulong x;
	h5ulong y;
} h5point_ulong;

typedef struct {
	h5uint x;
	h5uint y;
} h5point_uint;

/*Composite bezier curve*/
typedef struct {
	h5point_ulong p1;
	h5point_ulong control;
	h5point_ulong p3;
} h5bezier_ulong;

/*Composite bezier curve*/
typedef struct {
	h5point_uint p1;
	h5point_uint control;
	h5point_uint p3;
} h5bezier_uint;

h5point_ulong H5Math_PointAdd(h5point_ulong a, h5point_ulong b); /*Composite point addition*/
h5point_ulong H5Math_PointSub(h5point_ulong a, h5point_ulong b); /*Composite point substraction*/
h5point_ulong H5Math_PointMultScalar(h5point_ulong a, H5Rat k); /*Multiply h5point_ulong by scalar [k]*/

h5umax H5Math_umax_isqrt(h5umax a); /*Integer square root of h5umax*/ 
h5ulong H5Math_isqrt(h5ulong a); /*Integer square root of h5ulong*/ 

/*Number of iterations to do with Newton's method, 5 by default*/
#ifndef H5MATH_SQRT_ITER
	#define H5MATH_SQRT_ITER 5
#endif
H5Rat H5Math_umax_ratsqrt(h5umax a); /*Rational square root of h5umax*/ 
H5Rat H5Math_ratsqrt(h5ulong a); /*Rational square root of h5ulong*/

h5point_ulong H5Math_getBezierPoint(h5bezier_ulong curve, H5Rat t); /*Calculate point [t] of bezier curve, where 
                                                                   [t] is a number between zero and one*/
/*To be implemented*/
void H5Math_eventSimulation(h5uchar event_num, h5uchar *dist, h5uchar *vals, 
                       h5uchar run_num, h5uchar *run_results); /*Simulate events.
                       [event_num]: number of possible results of the event
                       [dist]: array of size [event_num], filled with numbers from 0-100,
                               where the array has to add up to 100 percent
                       [vals]: array of size [event_num], the value associated
                               with each result at the same index
                       [run_num]: number of times to simulate the event
                       [run_results]: array of size [run_num], each cell contains the 
                                      result of each run of the event*/
#endif
