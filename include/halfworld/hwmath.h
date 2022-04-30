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


/*Composite unsigned*/
typedef struct {
	hwuchar high;
	hwuint low;
} hwcompuint;

typedef struct {
	_Bool sign;
	hwuint num;
} hwsint;

/*Composite 2D vector*/
typedef struct {
	hwcompuint x;
	hwcompuint y;
} hwcomppoint;

/*Composite bezier curve*/
typedef struct {
	hwcomppoint p1;
	hwcomppoint control;
	hwcomppoint p3;
} hwcompbezier;

hwcompuint HWMath_CompAdd(hwcompuint a, hwcompuint b); /*Composite addition*/
hwcompuint HWMath_CompSub(hwcompuint a, hwcompuint b); /*Composite substraction*/
hwcompuint HWMath_CompMult(hwcompuint a, hwcompuint b); /*Composite multiplication*/
hwcompuint HWMath_CompDiv(hwcompuint a, hwcompuint b); /*Composite division, beware, doesn't check for zero in the divisor!*/

hwcomppoint HWMath_CompPointAdd(hwcomppoint a, hwcomppoint b); /*Composite point addition*/
hwcomppoint HWMath_CompPointSub(hwcomppoint a, hwcomppoint b); /*Composite point substraction*/
hwcomppoint HWMath_CompPointMult(hwcomppoint a, hwcomppoint b); /*Composite point multiplication*/
hwcomppoint HWMath_CompPointMultScalar(hwcomppoint a, hwuint b); /*Multiply hwcomppoint by scalar*/

hwumax HWMath_rat_umax(HWRat a, hwsint k); /*HWRat to hwumax, with scaling factor k compared to actual
                                             value, negative means division, positive multiplication*/
HWRat HWMath_umax_rat(hwumax a, hwsint k); /*HWRat to hwumax, with scaling factor k compared to actual 
                                             value, negative means division, positive multiplication*/

HWRat HWMath_umax_sqrt(hwumax a); /*Square root of hwumax*/
HWRat HWMath_uint_sqrt(hwuint a); /*Square root of hwuint*/
HWRat HWMath_uchar_sqrt(hwuint a); /*Square root of hwuchar*/

hwcomppoint HWMath_getBezierPoint(hwcompbezier curve, hwuint t); /*Calculate point [t] of bezier curve, where 
                                                                   [t] is a number between zero and one, in this
                                                                   case zero is zero and 0xFFFF is one*/
hwsint HWMath_uintSine(hwuint a); /*Calculate sine of [a], where [a] is expressed 
                                    in sexagesimal degrees in such a way that 360 
                                    degrees equate to 3600 hwuint units,
                                    and return value is a hwsint where +-0 is zero,
                                    and +-0xFFFF is one*/
hwsint HWMath_uintCosine(hwuint a); /*Calculate cosine of [a], where [a] is expressed 
                                    in sexagesimal degrees in such a way that 360 
                                    degrees equate to 3600 hwuint units,
                                    and return value is a hwsint where +-0 is zero,
                                    and +-0xFFFF is one*/
hwsint HWMath_uintTangent(hwuint a); /*Calculate tangent of [a], where [a] is expressed 
                                    in sexagesimal degrees in such a way that 360 
                                    degrees equate to 3600 hwuint units,
                                    and return value is a hwsint where +-0 is zero,
                                    and +-0xFFFF is one*/
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
