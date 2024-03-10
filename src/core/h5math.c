/*
Copyright Nomagno 2022, 2023

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

#include <halfive/h5req.h>
#include <halfive/h5math.h>
#include <math.h>

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
	return (VEC2(h5uint)){.x = a.x * H5_ABS_INT(k), .y = a.x * H5_ABS_INT(k)};
}
VEC2(h5ulong) H5Math_ulong_vecMultScalar(VEC2(h5ulong) a, h5float k)
{
	return (VEC2(h5ulong)){.x = a.x * H5_ABS_INT(k), .y = a.x * H5_ABS_INT(k)};
}
VEC2(h5umax) H5Math_umax_vecMultScalar(VEC2(h5umax) a, h5float k)
{
	return (VEC2(h5umax)){.x = a.x * H5_ABS_INT(k), .y = a.x * H5_ABS_INT(k)};
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

/*Composite floating point vector addition*/
VEC2(h5float) H5Math_float_vecAddVec(VEC2(h5float) a, VEC2(h5float) b)
{
	return (VEC2(h5float)){.x = a.x + b.x, .y = a.y + b.y};
}
/*Composite floating point vector substraction*/
VEC2(h5float) H5Math_float_vecSubVec(VEC2(h5float) a, VEC2(h5float) b)
{
	return (VEC2(h5float)){.x = a.x - b.x, .y = a.y - b.y};
}
/*Composite floating point vector scaling*/
VEC2(h5float) H5Math_float_vecMultScalar(VEC2(h5float) a, h5float k)
{
	return (VEC2(h5float)){.x = a.x * k, .y = a.x * k};
}

h5float H5Math_float_clamp(h5float x, h5slong lower_bound, h5slong upper_bound, _Bool *clamped){
	if (x < lower_bound){
		*clamped = 1;
		return lower_bound;
	}
	else if (x > upper_bound){
		*clamped = 1;
		return upper_bound;
	}
	else {
		*clamped = 0;
		return x;
	}
}

h5float H5Math_float_crossProductVecVec(VEC2(h5float) a, VEC2(h5float) b) {
  return a.x * b.y - a.y * b.x;
}
VEC2(h5float) H5Math_float_crossProductVecScalar(VEC2(h5float) a, h5float s) {
	return (VEC2(h5float)){s * a.y, -s * a.x};
}
VEC2(h5float) H5Math_float_crossProductScalarVec(h5float s, VEC2(h5float) b) {
	return (VEC2(h5float)){-s * b.y, s * b.x};
}

h5float H5Math_float_dotProduct(VEC2(h5float) a, VEC2(h5float) b){
	return a.x*b.x + a.y*b.y;
}

_Bool H5Math_float_checkForOverlap_1D(VEC2(h5float) l1, VEC2(h5float) l2) {
	if ((l1.x > l2.x) && (l1.x < l2.y)) {
		return 1;
	}
	if ((l2.x > l1.x) && (l2.x < l1.y)) {
		return 1;
	}

	return 0;
}

/* https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect */
/* https://gist.github.com/tp/75cb619a7e40e6ad008ef2a6837bbdb2 */
/* https://stackoverflow.com/questions/16314069/calculation-of-intersections-between-line-segments */

/*(2, 2) (3, 2) (2.5, 1) (2.5, 3)*/

H5Math_IntersectData H5Math_float_getLineFromSegment(VEC2(h5float) line_start, VEC2(h5float) line_end) {
	TUPLE2(h5float) retval = {0, 0};
	H5Math_IntersectType type = 0;
	_Bool line_isvertical = H5FLOAT_CMP(line_start.x, line_end.x);
	_Bool line_ishorizontal = H5FLOAT_CMP(line_start.y, line_end.y);
	if (line_isvertical && line_ishorizontal) {
		/*Nonsense! This segment is closer to being a point than a segment*/
		type = Overlap_None;
		retval.x = line_start.x;
		retval.y = line_start.y;
	} else if (line_isvertical) {
		/*Describing a vertical line as slope-intercept is not possible, but we do it anyways*/
		type = Overlap_SinglePoint;
		retval.x = H5FLT_MAX; /*Slope*/
		retval.y = line_start.x; /*Intercept*/
	} else if (line_ishorizontal) {
		type = Overlap_SinglePoint;
		retval.x = 0; /*Slope*/
		retval.y = line_start.y; /*Intercept*/
	} else {
		/*Generic case, a line with a certain non-zero slope*/
		type = Overlap_SinglePoint;
		retval.x = (line_end.y - line_start.y)/(line_end.x - line_start.x); /*Slope*/
		retval.y = line_start.y - (retval.x*line_start.x); /*Intercept*/
	}
	return (H5Math_IntersectData){ type, retval };
}

/*
y = lx + j
y = mx + k

mx + k = lx + j
mx - lx = j - k
(m-l)x = j - k
x = (j-k)/(m-l)
*/

H5Math_IntersectData H5Math_float_lineIntersection(TUPLE2(h5float) l1, TUPLE2(h5float) l2) {
	h5float slope1 = l1.x;
	h5float slope2 = l2.x;
	h5float intercept1 = l1.y;
	h5float intercept2 = l2.y;
	_Bool sameslope = H5FLOAT_CMP(slope1, slope2);
	_Bool sameintercept = H5FLOAT_CMP(intercept1, intercept2);
	if (sameslope && sameintercept) {
		/*They are the same line, of course the intersection is multiple points!!!*/
		return (H5Math_IntersectData) { Overlap_MultiplePoints, {1, 1} };
	}
	if (sameslope && !sameintercept) {
		/*They are parallel, of course they don't have an intersection!*/
		return (H5Math_IntersectData) { Overlap_None, {1, 0} };
	}
	h5float intersectx = (intercept1-intercept2)/(slope2-slope1);
	h5float intersecty = slope1*intersectx+intercept1;
	return (H5Math_IntersectData) { Overlap_SinglePoint, {intersectx, intersecty} };
}

_Bool H5Math_isPointWithinSegment(VEC2(h5float) line_start, VEC2(h5float) line_end, VEC2(h5float) point) {
	const h5float extra_tolerance = 0.0;
	if (line_start.x <= line_end.x) {
		return (point.x >= (line_start.x-extra_tolerance) && point.x <= (line_end.x+extra_tolerance) );
	} else {
		return (point.x >= (line_end.x-extra_tolerance) && point.x <= (line_start.x+extra_tolerance));
	}
}

H5Math_IntersectData H5Math_float_pointOfIntersection(VEC2(h5float) l1_start, VEC2(h5float) l1_end, VEC2(h5float) l2_start, VEC2(h5float) l2_end) {
	H5Math_IntersectData lineA = H5Math_float_getLineFromSegment(l1_start, l1_end);
	h5float slopeA, interceptA;
	_Bool lineA_isvertical;
	if (lineA.type == Overlap_None) {
		return (H5Math_IntersectData){ Overlap_None, {1, 0} };
	} else if (lineA.type == Overlap_MultiplePoints) {
		/*Burn the computer*/
		return (H5Math_IntersectData){ Overlap_MultiplePoints, {0, 0} };
	} else {
		lineA_isvertical = (lineA.data.x == H5FLT_MAX);
		slopeA = lineA.data.x;
		interceptA = lineA.data.y;
	}
	H5Math_IntersectData lineB = H5Math_float_getLineFromSegment(l2_start, l2_end);
	h5float slopeB, interceptB;
	_Bool lineB_isvertical;
	if (lineB.type == Overlap_None) {
		return (H5Math_IntersectData){ Overlap_None, {2, 0} };
	} else if (lineB.type == Overlap_MultiplePoints) {
		/*Burn the computer*/
		return (H5Math_IntersectData){ Overlap_MultiplePoints, {1, 0} };
	} else {
		lineB_isvertical = (lineB.data.x == H5FLT_MAX);
		slopeB = lineB.data.x;
		interceptB = lineB.data.y;
	}

	if (!lineA_isvertical && !lineB_isvertical) {
		H5Math_IntersectData virtual_intersect = H5Math_float_lineIntersection((TUPLE2(h5float)){ slopeA, interceptA }, (TUPLE2(h5float)){ slopeB, interceptB });
		if (virtual_intersect.type == Overlap_None) {
			return (H5Math_IntersectData){ Overlap_None, {3, 0} };
		} else if (virtual_intersect.type == Overlap_MultiplePoints) {
			return (H5Math_IntersectData){ Overlap_MultiplePoints, {2, 0} };
		} else {
			if (H5Math_isPointWithinSegment(l1_start, l1_end, virtual_intersect.data) &&
			    H5Math_isPointWithinSegment(l2_start, l2_end, virtual_intersect.data)) {
				return (H5Math_IntersectData){ Overlap_SinglePoint, virtual_intersect.data };
			} else {
				return (H5Math_IntersectData){ Overlap_None, {4, 0} };
			}
		}
	} else if (!lineA_isvertical && lineB_isvertical) {
		VEC2(h5float) virtual_intersection_point = { l2_start.x, slopeA*l2_start.x+interceptA };
		if (H5Math_isPointWithinSegment(l1_start, l1_end, virtual_intersection_point) &&
		    H5Math_isPointWithinSegment(l2_start, l2_end, virtual_intersection_point)) {
				return (H5Math_IntersectData){ Overlap_SinglePoint, virtual_intersection_point };
			} else {
				return (H5Math_IntersectData){ Overlap_None, {5, 0} };
			}
	} else if (lineA_isvertical && !lineB_isvertical) {
		VEC2(h5float) virtual_intersection_point = { l1_start.x, slopeB*l1_start.x+interceptB };
		if (H5Math_isPointWithinSegment(l1_start, l1_end, virtual_intersection_point) &&
		    H5Math_isPointWithinSegment(l2_start, l2_end, virtual_intersection_point)) {
				return (H5Math_IntersectData){ Overlap_SinglePoint, virtual_intersection_point };
			} else {
				return (H5Math_IntersectData){ Overlap_None, {6, 0} };
			}
	} else /*Both lines are vertical*/ {
		if (H5FLOAT_CMP(l1_start.x, l2_start.x)){
			return (H5Math_IntersectData){ Overlap_MultiplePoints, {3, l1_start.x} };
		} else {
			return (H5Math_IntersectData){ Overlap_None, {7, 0} };
		}
	}
}

VEC2(h5float) H5Math_float_rotateVector(VEC2(h5float) vec, h5float angle) {
	/*angle = CLAMPANGLE(angle);*/
	return (VEC2(h5float)) {
		cos(angle)*vec.x - sin(angle)*vec.y,
		sin(angle)*vec.x + cos(angle)*vec.y
	};
}

void H5Math_float_calculateRectCorners(H5Math_Rect rect, VEC2(h5float) *fourvecs, h5float angle) {
			VEC2(h5float) p1 = { /*Top left*/
				-(float)rect.size.x,
				-(float)rect.size.y
			};
			VEC2(h5float) p2 = { /*Top right*/
				rect.size.x,
				-(float)(rect.size.y)
			};
			VEC2(h5float) p3 = { /*Bottom right*/
				rect.size.x,
				rect.size.y
			};
			VEC2(h5float) p4 = { /*Bottom left*/
				-(float)rect.size.x,
				rect.size.y
			};

			VEC2(h5float) np1 = H5Math_float_rotateVector(p1, angle);
			VEC2(h5float) np2 = H5Math_float_rotateVector(p2, angle);
			VEC2(h5float) np3 = H5Math_float_rotateVector(p3, angle);
			VEC2(h5float) np4 = H5Math_float_rotateVector(p4, angle);

			fourvecs[0] = np1;
			fourvecs[1] = np2;
			fourvecs[2] = np3;
			fourvecs[3] = np4;
}

/*Newton's method*/
h5float H5Math_squareRoot(h5float a)
{
	float guess = H5Math_uint_integerSquareRoot(a);
	for (unsigned i = 0; i < H5MATH_SQRT_ITER; i++) {
		guess = (guess + (a / guess)) / 2;
		guess = H5_ABS_FLOAT(guess);
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
#endif
