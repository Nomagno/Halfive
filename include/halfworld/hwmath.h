#ifndef HWMATH_H
#define HWMATH_H

#include <halfworld/hwreq.h>
#include <halfworld/hwrat.h>

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

#define HWMATH_MAKESIGN(type, name) typedef struct { _Bool sign; type num; } name

HWMATH_MAKESIGN(hwuchar, hwschar);
HWMATH_MAKESIGN(hwuint, hwsint);
HWMATH_MAKESIGN(hwulong, hwslong);
HWMATH_MAKESIGN(hwumax, hwsmax);

/*Composite 2D vector*/
typedef struct {
	hwulong x;
	hwulong y;
} hwpoint;

/*Composite bezier curve*/
typedef struct {
	hwpoint p1;
	hwpoint control;
	hwpoint p3;
} hwbezier;

hwpoint HWMath_PointAdd(hwpoint a, hwpoint b); /*Composite point addition*/
hwpoint HWMath_PointSub(hwpoint a, hwpoint b); /*Composite point substraction*/
hwpoint HWMath_PointMultScalar(hwpoint a, HWRat k); /*Multiply hwpoint by scalar [k]*/

hwumax HWMath_umax_isqrt(hwumax a); /*Integer square root of hmax*/ 
hwulong HWMath_isqrt(hwulong a); /*Integer square root of hwulong*/ 

HWRat HWMath_umax_ratsqrt(hwumax a); /*Rational square root of hwumax*/ /*To be implemented*/ 
HWRat HWMath_ratsqrt(hwulong a); /*Rational square root of hwulong*/ /*To be implemented*/ 

hwpoint HWMath_getBezierPoint(hwbezier curve, HWRat t); /*Calculate point [t] of bezier curve, where 
                                                                   [t] is a number between zero and one*/
/*To be implemented*/
void HWMath_eventSimulation(hwuchar event_num, hwuchar *dist, hwuchar *vals, 
                       hwuchar run_num, hwuchar *run_results); /*Simulate events.
                       [event_num]: number of possible results of the event
                       [dist]: array of size [event_num], filled with numbers from 0-100,
                               where the array has to add up to 100 percent
                       [vals]: array of size [event_num], the value associated
                               with each result at the same index
                       [run_num]: number of times to simulate the event
                       [run_results]: array of size [run_num], each cell contains the 
                                      result of each run of the event*/
#endif
