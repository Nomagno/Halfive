#include <halfworld/hwreq.h>
#include <halfworld/hwrat.h>
#include <halfworld/hwmath.h>

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

hwcompuint HWMath_CompAdd(hwcompuint a, hwcompuint b){
	hwuchar low_add = a.low + b.low;
	hwuint high_add = a.high + b.high;
	return (hwcompuint){
		.low = low_add,
		.high = ((low_add < a.low) || (low_add < b.low)) 
		          ? (high_add + 1) 
		          : (high_add)
	};
}

hwcompuint HWMath_CompSub(hwcompuint a, hwcompuint b){
	hwuchar low_sub = a.low - b.low;
	hwuint high_sub = a.high - b.high;
	return (hwcompuint){
		.low = low_sub,
		.high = (low_sub > a.low) 
		          ? (high_sub - 1) 
		          : (high_sub)
	};
}

hwcompuint HWMath_CompMult(hwcompuint a, hwcompuint b){
	hwcompuint incr = (hwcompuint){0};
	for(hwcompuint i = b; !i.high && !i.low; i = HWMath_CompSub(i, (hwcompuint){0, 1})){
		incr = HWMath_CompAdd(incr, a);
	}
	return incr;
}

hwcomppoint HWMath_CompPointAdd(hwcomppoint a, hwcomppoint b){
	return (hwcomppoint){
		.x = HWMath_CompAdd(a.x, b.x),
		.y = HWMath_CompAdd(a.y, b.y)
	};
}

hwcomppoint HWMath_CompPointSub(hwcomppoint a, hwcomppoint b){
	return (hwcomppoint){
		.x = HWMath_CompSub(a.x, b.x),
		.y = HWMath_CompSub(a.y, b.y)
	};
}

hwcomppoint HWMath_CompPointMultScalar(hwcomppoint a, hwuint k){
	return (hwcomppoint){
		.x = HWMath_CompMult(a.x, (hwcompuint){0, k}),
		.y = HWMath_CompMult(a.y, (hwcompuint){0, k})
	};
}

/*A(1-t)(1-t) + B(1-t)(2t) + C(t)(t)*/
hwcomppoint HWMath_getBezierPoint(hwcompbezier curve, hwuint t){
	hwcomppoint a = HWMath_CompPointMultScalar(curve.p1, (255 - t)*(255 - t));

	hwcomppoint b = HWMath_CompPointMultScalar(curve.control, 2*(255 - t)*t);

	hwcomppoint c = HWMath_CompPointMultScalar(curve.p1, t*t);

	a = HWMath_CompPointAdd(a, b);
	b = HWMath_CompPointAdd(a, c);
	return a;
}
