/*
Copyright Nomagno 2023

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
#include <halfive/h5render.h>
#include <halfive/h5coord.h>
#include <halfive/h5math.h>
#include <math.h>

#include "h5sim.h"

#include <halfive/h5vi.h>

/*
EDGE-VERTEX COLLISION DETECTION BETWEEN RECT A, B
PHYSICS FRAME PRE-OVERLAP
                  BBBBBBBBBBBBBBB
    AA            B             B
  AA  AA V        B             B
A        1<       B             B
  AA  AA ^        B             B
    AA            B             B
                  BBBBBBBBBBBBBBB

PHYSICS FRAME POST-OVERLAP:
                  BBBBBBBBBBBBBBB
                AAB             B
              AA  BAA v         B
            A     B   2<        B
              AA  BAA ^         B
                AAB             B
                  BBBBBBBBBBBBBBB

IMPLEMENT ASAP :[TAKE THE FURTHEST VERTEX INSIDE B AS THE COLLISION VERTEX,
DRAW ITS PATH FROM PRE-OVERLAP TO POST-OVERLAP, TAKE THE FIRST EDGE THE
RESULTING SEGMENT INTERSECTS WITH AS THE COLLISON EDGE]

IMPLEMENT MID PRIORITY: [IF THE ROTATION ANGLE OF BOTH RECTS MODULO 180
(PROBABLY 180?) IS THE SAME BOTH PRE- AND POST- COLLISION, SWITCH TO
EDGE-EDGE COLLISION]

TODO: IMPLEMENT LATER FOR CORRECTNESS: [DRAW ARCS INSTEAD OF SEGMENTS TO REPRESENT
VERTEX PATHS, TAKING INTO ACCOUNT ROTATIONAL PROPERTIES]

WORKING OUT THE COLLISION EDGE:
         1--------B---2        
POINT OF CONTACT: ^
(FIRST EDGE OF THE RECTANGLE FOR WHICH THE PRE-)

*/

#define WCONSTANT 1024
#define HCONSTANT 1024

#include <halfive/code_setup.h>
#include <halfive/h5stdlib.h>
#include <time.h>
#include <stdio.h>


void calculateRectCornersFull(H5Physics_Object object, VEC2(h5float) *fourvecs) {
	VEC2(h5float) flt_pts[4] = {0};
	H5Math_float_calculateRectCorners(object.rect, flt_pts, object.angle);
	VEC2(h5float) pts[4] = {
		{ object.position.x + flt_pts[0].x, object.position.y + flt_pts[0].y },
		{ object.position.x + flt_pts[1].x, object.position.y + flt_pts[1].y },
		{ object.position.x + flt_pts[2].x, object.position.y + flt_pts[2].y },
		{ object.position.x + flt_pts[3].x, object.position.y + flt_pts[3].y },
	};

	fourvecs[0] = pts[0];
	fourvecs[1] = pts[1];
	fourvecs[2] = pts[2];
	fourvecs[3] = pts[3];
}

/*Writes down rect corners if object two is rotated so it is "angle2" degrees*/
/*This is usually used to transform object1 into object2's axis-aligned frame of reference*/
/*Returns the angle needed to reverse the transformation.*/
h5float calculateRectCornersFull_TwoObjects_Relative(H5Physics_Object object1, H5Physics_Object object2, VEC2(h5float) *fourvecs1, VEC2(h5float) *fourvecs2, h5float angle2) {
	VEC2(h5float) relative_position = /*relative position from object two to object one*/ {
		object1.position.x - object2.position.x,
		object1.position.y - object2.position.y,
	};
	/*This code takes a bit of thought to understand, but I just verified it is 100% correct.*/
	relative_position = H5Math_float_rotateVector(relative_position, (angle2 - object2.angle));

	VEC2(h5float) flt_pts_1[4] = {0};
	H5Math_float_calculateRectCorners(object1.rect, flt_pts_1, object1.angle + (angle2 - object2.angle));
	VEC2(h5float) pts_1[4] = {
		{ relative_position.x + object2.position.x + flt_pts_1[0].x, relative_position.y + object2.position.y + flt_pts_1[0].y },
		{ relative_position.x + object2.position.x + flt_pts_1[1].x, relative_position.y + object2.position.y + flt_pts_1[1].y },
		{ relative_position.x + object2.position.x + flt_pts_1[2].x, relative_position.y + object2.position.y + flt_pts_1[2].y },
		{ relative_position.x + object2.position.x + flt_pts_1[3].x, relative_position.y + object2.position.y + flt_pts_1[3].y },
	};

	fourvecs1[0] = pts_1[0];
	fourvecs1[1] = pts_1[1];
	fourvecs1[2] = pts_1[2];
	fourvecs1[3] = pts_1[3];


	VEC2(h5float) flt_pts_2[4] = {0};
	H5Math_float_calculateRectCorners(object2.rect, flt_pts_2, angle2);
	VEC2(h5float) pts_2[4] = {
		{ object2.position.x + flt_pts_2[0].x, object2.position.y + flt_pts_2[0].y },
		{ object2.position.x + flt_pts_2[1].x, object2.position.y + flt_pts_2[1].y },
		{ object2.position.x + flt_pts_2[2].x, object2.position.y + flt_pts_2[2].y },
		{ object2.position.x + flt_pts_2[3].x, object2.position.y + flt_pts_2[3].y },
	};

	fourvecs2[0] = pts_2[0];
	fourvecs2[1] = pts_2[1];
	fourvecs2[2] = pts_2[2];
	fourvecs2[3] = pts_2[3];

	return (angle2 - object2.angle);
}


#define GET_TOPLEFT(x) x[0]
#define GET_TOPRIGHT(x) x[1]
#define GET_BOTTOMLEFT(x) x[3]
#define GET_BOTTOMRIGHT(x) x[2]

_Bool detectOverlap(H5Physics_Object objectA, H5Physics_Object objectB){
	/*Separating axis theorem: two convex polygons intersect if and only if it is 
	  not possible to draw a line between both of them*/
	/*Approach: project all corners of each rectangle into four axis,
	  defined as parallel to their sides, checking for overlaps between
	  the maxima and minima of rect A and rect B*/
	VEC2(h5float) flt_pts_A[4] = {0};
	VEC2(h5float) flt_pts_B[4] = {0};
	calculateRectCornersFull(objectA, flt_pts_A);
	calculateRectCornersFull(objectB, flt_pts_B);
	VEC2(h5float) axis[4] = {
		{
			GET_TOPRIGHT(flt_pts_A).x - GET_TOPLEFT(flt_pts_A).x,
			GET_TOPRIGHT(flt_pts_A).y - GET_TOPLEFT(flt_pts_A).y
		},
		{
			GET_TOPRIGHT(flt_pts_A).x - GET_BOTTOMRIGHT(flt_pts_A).x,
			GET_TOPRIGHT(flt_pts_A).y - GET_BOTTOMRIGHT(flt_pts_A).y
		},
		{
			GET_TOPLEFT(flt_pts_B).x - GET_BOTTOMLEFT(flt_pts_B).x,
			GET_TOPLEFT(flt_pts_B).y - GET_BOTTOMLEFT(flt_pts_B).y
		},
		{
			GET_TOPLEFT(flt_pts_B).x - GET_TOPRIGHT(flt_pts_B).x,
			GET_TOPLEFT(flt_pts_B).y - GET_TOPRIGHT(flt_pts_B).y
		}		
	};
	for (int i = 0; i < 4; i++) {
		h5float axis_length_squared = (axis[i].x*axis[i].x)+(axis[i].y*axis[i].y);
		VEC2(h5float) projection[4][8] = {0};
		h5float projection_scalar[4][8] = {0};
		for (int j = 0; j < 8; j++) {
			#define FLT_ARRAY ((j < 4) ? flt_pts_A[j] : flt_pts_B[j-4])
			projection[i][j] = (VEC2(h5float)) {
				H5Math_float_dotProduct(FLT_ARRAY, axis[i])/axis_length_squared*axis[i].x,
				H5Math_float_dotProduct(FLT_ARRAY, axis[i])/axis_length_squared*axis[i].y,
			};
			projection_scalar[i][j] = H5Math_float_dotProduct(projection[i][j], axis[i]);
			/*The first four scalars of each axis hold the points of A, the last four hold the points of B*/
		}

		h5float minimum_a = 0;
		h5float maximum_a = 0;
		for (int k = 0; k < 4; k++) {
			if (k == 0) {
				minimum_a = projection_scalar[i][k];
				maximum_a = projection_scalar[i][k];
			} else {
				if (projection_scalar[i][k] < minimum_a) minimum_a = projection_scalar[i][k];
				if (projection_scalar[i][k] > maximum_a) maximum_a = projection_scalar[i][k];
			}
		}

		h5float minimum_b = 0;
		h5float maximum_b = 0;		
		for (int l = 4; l < 8; l++) {
			if (l == 4) {
				minimum_b = projection_scalar[i][l];
				maximum_b = projection_scalar[i][l];
			} else {
				if (projection_scalar[i][l] < minimum_b) minimum_b = projection_scalar[i][l];
				if (projection_scalar[i][l] > maximum_b) maximum_b = projection_scalar[i][l];
			}
		}
		if ((minimum_b <= maximum_a) && (maximum_b >= minimum_a)) {
			/*overlap, all okay*/
		} else {
			return 0; /*no overlap, they don't collide*/
		}
	}
	/*If it didn't return 0 anywhere, they do collide.*/
	
	return 1;
}

h5float calculateMomentOfInertia(H5Physics_Object obj) {
	h5float h = obj.rect.size.x;
	h5float w = obj.rect.size.y;
	h5float m = obj.mass;
	return (m/12)*(w*w + h*h);
}

VEC2(h5float) dumbArmVector(H5Physics_Object obj) {
	return (VEC2(h5float)) { obj.rect.size.x / 2.0, obj.rect.size.y / 2.0 };
}


void applyLinearAcceleration(H5Physics_Object *object, VEC2(h5float) accel) {
	object->force.x += accel.x * object->mass;
	object->force.y += accel.y * object->mass;
}
void applyAngularAcceleration(H5Physics_Object *object, VEC2(h5float) accel, VEC2(h5float) arm_vec) {
	object->torque += H5Math_float_crossProductVecVec(arm_vec, accel)*object->moi;
}

void applyLinearForce(H5Physics_Object *object, VEC2(h5float) force) {
	object->force.x += force.x;
	object->force.y += force.y;
}
void applyTorque(H5Physics_Object *object, VEC2(h5float) torque, VEC2(h5float) arm_vec) {
	object->torque += H5Math_float_crossProductVecVec(arm_vec, torque);
}

void applyForce(H5Physics_Object *object, VEC2(h5float) force, VEC2(h5float) arm_vec) {
	applyLinearForce(object, force);
	applyTorque(object, force, arm_vec);
}

void applyAcceleration(H5Physics_Object *object, VEC2(h5float) accel, VEC2(h5float) arm_vec) {
	applyLinearAcceleration(object, accel);
	applyAngularAcceleration(object, accel, arm_vec);
}

struct CollisionDetectResult {
	_Bool are_colliding;
	unsigned error_code;
	_Bool did_swap;
	VEC2(h5float) point_of_collision; /*vector*/
	VEC2(h5float) tangent_of_collision; /*vector*/
};

struct CollisionDetectResult collisionDetect(H5Physics_Object object1, H5Physics_Object object2, H5Physics_Object old_object1, H5Physics_Object old_object2) {
	struct CollisionDetectResult retval = {0};
	H5Physics_Object tmp;
	VEC2(h5float) corners_1[4] = {0};
	VEC2(h5float) corners_2[4] = {0};
	unsigned vertexOfContact = 0; /*none, top left, top right, bottom right, bottom left*/

	calculateRectCornersFull_TwoObjects_Relative(object1, object2, corners_1, corners_2, 0);

	/*corners[0] is top left, corners[2] is bottom right*/
	for (int counter = 0; counter < 4; counter++) {
		if ( (corners_1[counter].x >= corners_2[0].x) &&
		     (corners_1[counter].x <= corners_2[2].x) && 
		     (corners_1[counter].y >= corners_2[0].y) && 
		     (corners_1[counter].y <= corners_2[2].y))
		{
			vertexOfContact = counter+1;
			break;
		};
	}

	/*I know this entire codebase sucks.
	  I'm not a physicist.
	  I barely understand what I'm doing.
	  I'll refactor it at some point [fairy music].*/
	if (vertexOfContact == 0) {
		/*Swap and check for the inverse order: "since the rects overlap but
		   there is no vertex of ONE inside TWO, it must be the other way around"*/
		    retval.did_swap = 1;
			tmp = object1;
			object1 = object2;
			object2 = tmp;

			tmp = old_object1;
			old_object1 = old_object2;
			old_object2 = tmp;

			calculateRectCornersFull_TwoObjects_Relative(object1, object2, corners_1, corners_2, 0);
			/*corners[0] is top left, corners[2] is bottom right*/
		for (int counter = 0; counter < 4; counter++) {
			if ( (corners_1[counter].x >= corners_2[0].x) &&
			     (corners_1[counter].x <= corners_2[2].x) && 
			     (corners_1[counter].y >= corners_2[0].y) && 
			     (corners_1[counter].y <= corners_2[2].y))
			{
				vertexOfContact = counter+1;
				break;
			};
		}
	}
	if (vertexOfContact == 0) {

		maybe_printf("ERROR: IMPOSSIBLE COLLISION CHARACTERISTICS (OR TOO HIGH RELATIVE SPEED)\n"
				"ERROR: PLEASE MANUALLY VERIFY VERTEX COORDINATES:\n"
				"ERROR: \tFORMAT:  TOPLEFT  TOPRIGHT BOTRIGHT BOTLEFT\n"
				"ERROR: \tOBJECT2: (%f, %f) (%f, %f) (%f, %f) (%f, %f)\n"
				"ERROR: \tOBJECT1: (%f, %f) (%f, %f) (%f, %f) (%f, %f)\n",
				corners_1[0].x, corners_1[0].y,
				corners_1[1].x, corners_1[1].y,
				corners_1[2].x, corners_1[2].y,
				corners_1[3].x, corners_1[3].y,

				corners_2[0].x, corners_2[0].y,
				corners_2[1].x, corners_2[1].y,
				corners_2[2].x, corners_2[2].y,
				corners_2[3].x, corners_2[3].y
				);
		retval.error_code = 1;
		return retval;
	}


	H5Physics_Object object_1_prev_in_curr_ref_frame = old_object1;
	VEC2(h5float) corners_prev_in_curr_ref_frame[4] = {0};

	/*Set the copy to the previous object's position in the current frame of reference*/
	object_1_prev_in_curr_ref_frame.position = (VEC2(h5float)) { old_object1.position.x - (object2.position.x - old_object2.position.x), old_object1.position.y - (object2.position.y - old_object2.position.y) };
	calculateRectCornersFull_TwoObjects_Relative(object_1_prev_in_curr_ref_frame, object2, corners_prev_in_curr_ref_frame, corners_2, 0);

	/*The segment from object_1_copy.position to object1.position AND the segment delimited by the first edge of object_2 where there is an intersection now theoretically fairly close to the correct data required to calculate a vertex-edge collision: the intersection is a ; This requires a LOT of physics frames to work reliable at high speeds for now. (does not yet account for rotation changes between the two frames, which requires some parabolic calculations in the case of rectangles that are basically wave modelling*/
	VEC2(h5float) vertex_path_start = corners_prev_in_curr_ref_frame[vertexOfContact-1];
	VEC2(h5float) vertex_path_end = corners_1[vertexOfContact-1];

	/*top left, top right, bottom right, bottom left*/
	VEC2(h5float) edge_path_start = {0};
	VEC2(h5float) edge_path_end = {0};

	H5Math_IntersectData intersection_point = {0};
	
	unsigned edge_id = 0;
	for (edge_id = 1; edge_id <= 4; edge_id++) {
		edge_path_start = corners_2[edge_id-1];
		edge_path_end = corners_2[edge_id%4];
		intersection_point = H5Math_float_pointOfIntersection(vertex_path_start, vertex_path_end, edge_path_start, edge_path_end);
		if (intersection_point.type == Overlap_SinglePoint) {
			maybe_printf ("REPORT: WE GOT A HIT WITH EDGE: %i\n", edge_id-1);
			break;
		} else if (intersection_point.type == Overlap_MultiplePoints) {
			break;
		} else {
			if (edge_id == 4) {
				break;
			}
		}
	}

	if (intersection_point.type == Overlap_None) {
		maybe_printf("REPORT: NO OVERLAP THROUGH PATH TRACING METHOD\n"
		"REPORT: \tTHIS COULD BE EITHER A BUG OR DUE TO ENGINE SIMULATION INACCURACIES AT HIGH ROTATION SPEEDS\n"
		"REPORT: \tABORTING THIS SIMULATION INSTANT!!\n"
		"REPORT: \tPENETRATOR VERTEX IN PAST:    (%f, %f)\n"
		"REPORT: \tPENETRATOR VERTEX IN PRESENT: (%f, %f)\n"
		"REPORT: \tEDGE VERTICES INITIAL:        (%f, %f)\n"
		"REPORT: \tEDGE VERTICES FINAL:          (%f, %f)\n",
		vertex_path_start.x, vertex_path_start.y,
		vertex_path_end.x, vertex_path_end.y,
		edge_path_start.x, edge_path_start.y,
		edge_path_end.x, edge_path_end.y
		);

		retval.error_code = 2;
		return retval;
	} else if (intersection_point.type == Overlap_MultiplePoints) {
		maybe_printf("ERROR: EDGE-EDGE OVERLAP THROUGH PATH TRACING METHOD\n"
		"ERROR: \tTHIS IS AS OF YET UNHANDLED\n"
		"ERROR: \tABORTING THIS SIMULATION INSTANT!!\n");
		retval.error_code = 3;
		return retval;
	} else {
		maybe_printf("REPORT: WE GOT A HIT\n");
		VEC2(h5float) Edge_Vector = { 
			corners_2[edge_id%4].x - corners_2[edge_id-1].x,
			corners_2[edge_id%4].y - corners_2[edge_id-1].y
		};
		VEC2(h5float) Point_Vector = {
			intersection_point.data.x - corners_2[edge_id-1].x,
			intersection_point.data.y - corners_2[edge_id-1].y
		};
		h5float parameter = Point_Vector.x/Edge_Vector.x;

		VEC2(h5float) corners_2_original[4] = {0};
		calculateRectCornersFull(object2, corners_2_original);
		VEC2(h5float) Edge_Vector_Original = { 
			corners_2_original[edge_id%4].x - corners_2_original[edge_id-1].x,
			corners_2_original[edge_id%4].y - corners_2_original[edge_id-1].y
		};
		retval.point_of_collision = (VEC2(h5float)){
			corners_2_original[edge_id-1].x + Edge_Vector_Original.x*parameter,
			corners_2_original[edge_id-1].y + Edge_Vector_Original.y*parameter			
		};
		retval.tangent_of_collision = Edge_Vector_Original;

		retval.error_code = 0;
		retval.are_colliding = 1;
		return retval;
	}
}

/*
E: restitution coefficient (how much energy is absorbed, probably should be lower than 1 but test to confirm)
I: moment of inertia
M: mass
v[AP]: v[A] + w[A]*rotate(r[AP], pi/2)
v[AB]: v[AP] - v[BP] ; v[AB] . n ; must be negative for a collision to be occuring
n: collision normal vector (perpendicular to collision edge and normalized)
j: impulse

numerator   = -(1+E)v[AB] . n

term1 = (n . n(1/M[A] + 1/M[B]))
term2 = (rotate(r[AP], pi/2) . n)^2/(I[A])
term3 = (rotate(r[BP], pi/2) . n)^2/(I[B])
denominator = term1+term2+term3 
j = numerator/denominator

v2[A] = v1[A] + (j/M[A])n
w2[A] = w1[A] + (rotate(r[AP], pi/2) . jn)/I[A]

v2[B] = v1[B] + (-j/M[B])n
w2[B] = w1[B] + (rotate(r[BP], pi/2) . -jn)/I[B]
*/

/*Necessary data from function:
penetration vector - easiest approximation:
get penetrating vertex path, which is the whole point of the function.
then take that point's position vector relative to both the center of mass of the penetrating and penetetrated object
*/

int resolveCollision(H5Physics_Object *object1, H5Physics_Object *object2, struct CollisionDetectResult collision_data) {
	VEC2(h5float) collision_normal = { -collision_data.tangent_of_collision.y, collision_data.tangent_of_collision.x };
	VEC2(h5float) collision_point = { collision_data.point_of_collision.x, collision_data.point_of_collision.y };

	VEC2(h5float) vector_ap_perpendicular = { -(collision_point.y - object1->position.y), collision_point.y - object1->position.x };
	VEC2(h5float) velocity_ap = { object1->velocity.x + object1->angular_velocity*vector_ap_perpendicular.x, object1->velocity.y + object1->angular_velocity*vector_ap_perpendicular.y};
	h5float velocity_ap_dot_n = H5Math_float_dotProduct(velocity_ap, collision_normal);

	VEC2(h5float) vector_bp_perpendicular = { -(collision_point.y - object2->position.y), collision_point.y - object2->position.x };
	VEC2(h5float) velocity_bp = { object2->velocity.x + object2->angular_velocity*vector_bp_perpendicular.x, object2->velocity.y + object2->angular_velocity*vector_bp_perpendicular.y};
	h5float velocity_bp_dot_n = H5Math_float_dotProduct(velocity_bp, collision_normal);

	h5float velocity_ab = velocity_ap_dot_n - velocity_bp_dot_n;

	VEC2(h5float) velocity_ab_vec = { velocity_ap.x - velocity_bp.x, velocity_ap.y - velocity_bp.y };


	if (velocity_ab > 0.0) {
		return 0; /*no collision*/
		maybe_printf("NO COLLISION BECAUSE OF RELATIVE SPEEDS\n");
	}

	h5float restitution_coefficient = object1->restitution_coefficient + object2->restitution_coefficient;
	h5float impulse_numerator = -(1.0 + restitution_coefficient) * H5Math_float_dotProduct(velocity_ab_vec, collision_normal);

	VEC2(h5float) term1_calculation_aid = { collision_normal.x*(1.0/object1->mass + 1.0/object2->mass), collision_normal.y*(1.0/object1->mass + 1.0/object2->mass)};
	h5float term1_of_impulse_denominator = H5Math_float_dotProduct(collision_normal, term1_calculation_aid);
	h5float term2_of_impulse_denominator = H5Math_float_dotProduct(vector_ap_perpendicular, vector_ap_perpendicular) * (1.0/object1->moi);
	h5float term3_of_impulse_denominator = H5Math_float_dotProduct(vector_bp_perpendicular, vector_bp_perpendicular) * (1.0/object2->moi);

	h5float impulse_denominator = term1_of_impulse_denominator + term2_of_impulse_denominator + term3_of_impulse_denominator;

	h5float impulse = impulse_numerator/impulse_denominator;

	VEC2(h5float) impulse_multiplied_by_normal = { impulse * collision_normal.x, impulse * collision_normal.y };

	VEC2(h5float) impulse_multiplied_by_normal_neg = { -impulse * collision_normal.x, -impulse * collision_normal.y };


	VEC2(h5float) incr_vel_1 = {(impulse/object1->mass) * collision_normal.x, (impulse/object1->mass) * collision_normal.y };
	object1->velocity = (VEC2(h5float)){ object1->velocity.x + incr_vel_1.x, object1->velocity.y + incr_vel_1.y };
	object1->angular_velocity = object1->angular_velocity + (H5Math_float_dotProduct(vector_ap_perpendicular, impulse_multiplied_by_normal)/object1->moi);

	VEC2(h5float) incr_vel_2 = {(-impulse/object2->mass) * collision_normal.x, (-impulse/object2->mass) * collision_normal.y };
	object2->velocity = (VEC2(h5float)){ object2->velocity.x + incr_vel_2.x, object2->velocity.y + incr_vel_2.y };
	object2->angular_velocity = object2->angular_velocity + (H5Math_float_dotProduct(vector_bp_perpendicular, impulse_multiplied_by_normal_neg)/object2->moi);

	return 1; /*There was a collision and it was resolved*/	
}

#define FRAME_SUBDIVISION 1
int simulatePhysics(unsigned n, H5Physics_Object *objects, H5Physics_Object *previousObjects, h5slong bound_up, h5slong bound_down, h5slong bound_left, h5slong bound_right, h5ulong time /*tenths of milliseconds*/) {
	_Bool checkedCollision[n][n];
	for (unsigned k = 0; k < n; k++) {
		for (unsigned l = 0; l < n; l++) {
			checkedCollision[k][l] = 0;
		}
	}
	for (unsigned i = 0; i < n; i++) {
			/*If the angle is greater than 2*pi radians, set it back to within range*/
			objects[i].angle = CLAMPANGLE(objects[i].angle);

		if (objects[i].exists) {
			for (unsigned j = 0; j < n; j++) {
				_Bool hasBeenChecked = checkedCollision[i][j] || checkedCollision[j][i];
				checkedCollision[i][j] = 1;
				if (objects[j].exists && i != j && !hasBeenChecked && detectOverlap(objects[i], objects[j])){
					maybe_printf("REPORT: COLLISION RESOLVE: O.%i / O.%i\n", i, j);
					struct CollisionDetectResult detection_result = collisionDetect(objects[i], objects[j], previousObjects[i], previousObjects[j]);
					if (detection_result.error_code == 0 && detection_result.are_colliding == 1) {
						resolveCollision(&objects[i], &objects[j], detection_result);
					}
					if (detection_result.error_code == 1) {
						goto EXIT_SIMULATION_ERROR;
					}
				}
			}

			/*Acceleration = force/mass */
			/* Velocity = velocity + acceleration*time */
			objects[i].velocity = (VEC2(h5float)){
				(h5float)objects[i].velocity.x +
				     (h5float)(((h5float)objects[i].force.x)/((h5float)objects[i].mass))*((h5float)time/10000.0),
				(h5float)objects[i].velocity.y +
				     (h5float)(((h5float)objects[i].force.y)/((h5float)objects[i].mass))*((h5float)time/10000.0)
			};

			objects[i].angular_velocity =
				objects[i].angular_velocity + objects[i].torque/objects[i].moi*((h5float)time/10000.0);

			/*Clamping is a temporary measure to prevent underflow when drawing out of screen bounds*/
			/* Position = position + velocity*time */
			_Bool clamped_horizontal;
			_Bool clamped_vertical;
			objects[i].position = (VEC2(h5float)){
				H5Math_float_clamp(
					(h5float)objects[i].position.x + (h5float)objects[i].velocity.x*((h5float)time/10000.0),
					bound_left,
					bound_right,
					&clamped_horizontal
				),
				H5Math_float_clamp(
					(h5float)objects[i].position.y + (h5float)objects[i].velocity.y*((h5float)time/10000.0),
					bound_up,
					bound_down,
					&clamped_vertical
				)
			};
			VEC2(h5float) object_points[4] = {0};
			calculateRectCornersFull(objects[i], object_points);
			objects[i].angle += objects[i].angular_velocity * ((h5float)time/10000.0);
			maybe_printf(
			"REPORT: OBJECT = %u\n"
			"REPORT: \t TIME    %f \n"
			"REPORT: \t SUBTIME %u \n"			
			"REPORT: \t Pos     (%f, %f)\n"
			"REPORT: \t Corners (%f, %f) (%f, %f) (%f, %f) (%f, %f)\n"
			"REPORT: \t L.Vel   (%f, %f)\n"
			"REPORT: \t L.Force (%f, %f)\n"
			"REPORT: \t L.Accel (%f, %f)\n"
			"REPORT: \t Mass    %u\n"
			"REPORT: \t-    -    -    -    -\n"
			"REPORT: \t Angle   %Lf\n"
			"REPORT: \t A.Vel   %f\n"
			"REPORT: \t Torque  %f\n"
			"REPORT: \t A. Accel %Lf\n"
			"REPORT: \t MoI     %f\n"
			"_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _\n",
			i,
			((h5float)time/10000.0),
			FRAME_SUBDIVISION,
			objects[i].position.x, objects[i].position.y,
			object_points[0].x, object_points[0].y, object_points[1].x, object_points[1].y,
			object_points[2].x, object_points[2].y, object_points[3].x, object_points[3].y,
			objects[i].velocity.x, objects[i].velocity.y,
			objects[i].force.x, objects[i].force.y,
			(objects[i].force.x)/((h5float)objects[i].mass),
			(objects[i].force.y)/((h5float)objects[i].mass),

			objects[i].mass,

			180*objects[i].angle/PI,
			objects[i].angular_velocity,
			objects[i].torque,
			180*(objects[i].torque/objects[i].moi)/PI,
			objects[i].moi
			);

			if (clamped_vertical) {
				objects[i].velocity.y = 0;
			}
			if (clamped_horizontal) {
				objects[i].velocity.x = 0;
			}
			objects[i].torque = 0; /*The torque has been applied. Clear it back to zero*/
			objects[i].force = (VEC2(h5float)){0}; /*The force has been applied. Clear it back to zero*/
		}
	}
	return 0;

	EXIT_SIMULATION_FINE:
		return 0;
	EXIT_SIMULATION_ERROR:
		return 1;
}

void H5Test_printPerformanceData(h5umax time_available, h5umax time_taken, _Bool is_paused, _Bool quit) {
	h5umax performance_percent = 100*(double)((double)time_available/(double)(time_taken));
	if (time_taken >= time_available) {
		maybe_printf("H5VI: WARNING: Timing objective not met - Performance: %li%%\n",
		       performance_percent);
	} else {
		maybe_printf("H5VI: Frame time: %li nanoseconds - Performance: %li%%\n",
		       (is_paused) ? 0 : time_taken, (is_paused) ? 100 : performance_percent);
	}
	if (quit) {
		maybe_printf("H5VI: Quitting\n");
	}
}

void drawObjects(unsigned n, H5Physics_Object *objects, H5Render_PixelData screen) {
	for (unsigned i = 0; i < n; i++) {
		if (objects[i].exists) {
			VEC2(h5float) flt_pts[4] = {0};
			/*H5Math_float_calculateRectCorners(objects[i].rect, flt_pts, objects[i].rect.angle);*/
			calculateRectCornersFull(objects[i], flt_pts);
			/*VEC2(h5ulong) pts[4] = {
				{ objects[i].position.x + flt_pts[0].x, objects[i].position.y + flt_pts[0].y },
				{ objects[i].position.x + flt_pts[1].x, objects[i].position.y + flt_pts[1].y },
				{ objects[i].position.x + flt_pts[2].x, objects[i].position.y + flt_pts[2].y },
				{ objects[i].position.x + flt_pts[3].x, objects[i].position.y + flt_pts[3].y },
			};*/

			VEC2(h5ulong) pts[4] = {
				{ flt_pts[0].x, flt_pts[0].y },
				{ flt_pts[1].x, flt_pts[1].y },
				{ flt_pts[2].x, flt_pts[2].y },
				{ flt_pts[3].x, flt_pts[3].y },
			};

			H5Render_ulong_drawRectangle(screen, pts[0], pts[1], pts[2], pts[3], 0, objects[i].colour, 100);
		}
	}
}

#define GLOBAL_BOUND_UP 100
#define GLOBAL_BOUND_LEFT 100
#define GLOBAL_BOUND_DOWN 900
#define GLOBAL_BOUND_RIGHT 900

VEC2(h5ulong) global_pts[4] = {
	POINT_UL(GLOBAL_BOUND_LEFT, GLOBAL_BOUND_UP),
	POINT_UL(GLOBAL_BOUND_RIGHT, GLOBAL_BOUND_UP),
	POINT_UL(GLOBAL_BOUND_RIGHT, GLOBAL_BOUND_DOWN),
	POINT_UL(GLOBAL_BOUND_LEFT, GLOBAL_BOUND_DOWN)
};

_Bool pauset;

/*Taking a pointer is done to accomodate emscripten*/
int H5Test_simulateOneFrame(H5Coordinate_GameplayEventData *opaque_handle) {
	clock_t start_t, end_t;
	start_t = clock(); /*Microseconds in XSI-compliant systems*/

	H5Coordinate_GameplayEventData generalData = *opaque_handle;
	struct H5Test_EventData specificData = *(struct H5Test_EventData *)(generalData.userdata);
	H5VI_InputData input_keys = generalData.input_keys;


	/*Bitmask: Up Down Left Right Button4 Button3 Button2 Button1*/
	_Bool k_up = input_keys.keys[H5KEY_UP];
	_Bool k_down = input_keys.keys[H5KEY_DOWN];
	_Bool k_left = input_keys.keys[H5KEY_LEFT];
	_Bool k_right = input_keys.keys[H5KEY_RIGHT];
	_Bool k_left2 = input_keys.keys[H5KEY_B5];
	_Bool k_right2 = input_keys.keys[H5KEY_B8];
	_Bool k_down2 = input_keys.keys[H5KEY_B7];
	_Bool k_up2 = input_keys.keys[H5KEY_B6];
	_Bool k_b1 = input_keys.keys[H5KEY_B1];
	_Bool k_b2 = input_keys.keys[H5KEY_B2];
	_Bool k_b3 = input_keys.keys[H5KEY_B3];
	_Bool k_b4 = input_keys.keys[H5KEY_B4];
	if (input_keys.keys[H5KEY_QUIT] == 1) {
		*specificData.quit = 1;
		goto GO_BACK;
	}
	if (input_keys.keys[H5KEY_PAUSE] == 1 && *specificData.is_paused == 0) {
		*specificData.is_paused = 1;
		goto GO_BACK;
	} else if (input_keys.keys[H5KEY_PAUSE] == 1 && *specificData.is_paused == 1) {
		*specificData.is_paused = 0;
		goto GO_BACK;
	}
	if (*specificData.is_paused == 1) {
		goto GO_BACK;
	}

	H5Physics_Object *player1 = &(*(struct H5Test_EventData *)(generalData.userdata)).objects[0];
	H5Physics_Object *player2 = &(*(struct H5Test_EventData *)(generalData.userdata)).objects[1];


	const int movAccel1 = 300;
	const int movAccel2 = 600;
	const VEC2(h5float) hitForce = (VEC2(h5float)) { 7000, 7000 };
	if (k_b3) {
		player1->force = (VEC2(h5float)) {0, 0};
		player1->velocity = (VEC2(h5float)) {0, 0};
		player1->torque = 0;
		player1->angular_velocity = 0;
	}
	if (k_b4) {
		player2->force = (VEC2(h5float)) {0, 0};
		player2->velocity = (VEC2(h5float)) {0, 0};
		player2->torque = 0;
		player2->angular_velocity = 0;
	}
	if (k_b1) {
		applyAngularAcceleration(player1, (VEC2(h5float)) { 0.1, 0 }, dumbArmVector(*player1));
	}
	if (k_b2) {
		applyAngularAcceleration(player2, (VEC2(h5float)) { 0.1, 0 }, dumbArmVector(*player2));
	}


	if (k_up){
		applyLinearAcceleration(player1, (VEC2(h5float)){ 0, -movAccel1 });
	}
	if (k_down){
		applyLinearAcceleration(player1, (VEC2(h5float)){ 0, movAccel1 });
	}
	if (k_left){
		applyLinearAcceleration(player1, (VEC2(h5float)){ -movAccel1, 0});
	}
	if (k_right){
		applyLinearAcceleration(player1, (VEC2(h5float)){ movAccel1, 0});
	}

	if (k_up2){
		applyLinearAcceleration(player2, (VEC2(h5float)){ 0, -movAccel2 });
	}
	if (k_down2){
		applyLinearAcceleration(player2, (VEC2(h5float)){ 0, movAccel2 });
	}
	if (k_left2){
		applyLinearAcceleration(player2, (VEC2(h5float)){ -movAccel2, 0});
	}
	if (k_right2){
		applyLinearAcceleration(player2, (VEC2(h5float)){ movAccel2, 0});
	}

	H5Physics_Object prevobjs[32] = {0};

	for (int i = 0; i < 32; i++) {
		prevobjs[i] = specificData.objects[i];
	}

	int errcode = 0;
	for (int i = 0; i < FRAME_SUBDIVISION; i++) {
		errcode = simulatePhysics(
			/*sizeof(specificData.objects)/sizeof(H5Physics_Object)*/ 32,
			&(*(struct H5Test_EventData *)(generalData.userdata)).objects[0],
			prevobjs,
			GLOBAL_BOUND_UP,
			GLOBAL_BOUND_DOWN,
			GLOBAL_BOUND_LEFT,
			GLOBAL_BOUND_RIGHT,
			generalData.time_available/100000/FRAME_SUBDIVISION
		);
		if (errcode != 0) return 1;
	}

	for (int i = 0; i < 32; i++) {
		specificData.previousObjects[i] = prevobjs[i];
	}

	if (detectOverlap(*player1, *player2)) {
		player1->colour = 0x7701; /*GREEN*/
		player2->colour = 0x7701; /*GREEN*/
	} else {
		player1->colour = 0xF80D; /*RED*/
		player2->colour = 0x8AF9; /*INDIGO*/
	}

	if (errcode != 0) {
		errcode = 1;
		goto GO_BACK;
	}


	goto GO_BACK;

GO_BACK:
	end_t = clock(); /*Microseconds in XSI-compliant systems*/
	*generalData.time_taken = ((end_t-start_t)*1000);
	return errcode;
}


static h5uint array_main_buf[HCONSTANT][WCONSTANT] = {0};

int main(void) {
	/*WARNING: PROGRAM DOESN'T WORK CORRECTLY IF 'CLOCKS_PER_SEC' IS NOT XSI-MANDATED 1000000*/

	H5VI_Reference main_ref;

	H5Render_PixelData main_buf = {HCONSTANT, WCONSTANT, .data = &array_main_buf[0][0]};
	H5Render_fill(main_buf, 0xFFFF);

	if (H5VI_init(&main_ref, HCONSTANT, WCONSTANT)) {
		goto EXIT;
	}
	H5VI_setBuffer(&main_ref, &main_buf);

	H5Coordinate_GameplayEventData loop_data = {
		.ref = &main_ref,
		.time_available = 33333333, /*average frame length in nanoseconds --- FPS = (1/time_available_in_seconds)*/
		.delta_time = 0, /*To-do: update this per-run*/
		.time_taken = &(h5umax){0},
		.const_userdata = NULL,
		.input_keys = {0},
		.userdata = &(struct H5Test_EventData){
			.is_paused = &(_Bool){0},
			.quit = &(_Bool){0},
			.screen = main_buf,
			.objects = {
				{.exists = 1, .position = {300, 300}, .mass = 100, .colour = 0xF80D, .rect = { .size = {20, 20} }, .restitution_coefficient = 0.1},
				{.exists = 1, .position = {200, 700}, .mass = 15, .colour = 0x8AF9, .rect = { .size = {7, 33} }, .restitution_coefficient = 0.1},
			}
		}
	};

	H5Physics_Object *objects = (((struct H5Test_EventData *)(loop_data.userdata))->objects);
	H5Physics_Object *previousObjects = (((struct H5Test_EventData *)(loop_data.userdata))->previousObjects);

	objects[0].moi = calculateMomentOfInertia(objects[0]);
	objects[1].moi = calculateMomentOfInertia(objects[1]);

	for (int i = 0; i < 32; i++) {
		previousObjects[i] = objects[i];
	}

	int errcode = 0;
	while (1) {
		_Bool old_is_paused = *(((struct H5Test_EventData *)(loop_data.userdata))->is_paused);

		H5VI_getInput(&main_ref, &loop_data.input_keys);

		errcode = H5Test_simulateOneFrame((void *)&loop_data);
		if (errcode != 0) {
			maybe_printf("ERROR: H5VI: ^^^ QUITTING DUE TO ERROR IN APPLICATION, CHECK ABOVE FOR LOGS ^^^\n");
			goto EXIT;
		}
		H5Render_fill(main_buf, 0xFFFF);
		drawObjects(32, objects, main_buf);
		H5Render_ulong_drawPolygonOutline(main_buf, global_pts, 4, 0x0001, 1000);

		_Bool new_is_paused = *(((struct H5Test_EventData *)(loop_data.userdata))->is_paused);
		_Bool quit = *(((struct H5Test_EventData *)(loop_data.userdata))->quit);

		H5Test_printPerformanceData(loop_data.time_available, *loop_data.time_taken, new_is_paused, quit);

		if (old_is_paused != new_is_paused) {
			nanosleep(&(struct timespec){0, 500000000 }, NULL);
		} else {
			H5VI_setBuffer(&main_ref, &main_buf);
			h5smax sleep_time = loop_data.time_available - *loop_data.time_taken;
			nanosleep(&(struct timespec){0, (sleep_time > 0) ? sleep_time : 1 }, NULL);
			if (pauset) {
				nanosleep(&(struct timespec){0, 500000000 }, NULL);
				pauset = 0;
			}
		}

		if (quit) {
			goto EXIT;
		}
	}

	EXIT:
		H5VI_destroy(&main_ref);
}
