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
#include <halfive/h5math.h>
#include <halfive/h5coord.h>


#ifndef H5TEST_H
#define H5TEST_H

typedef struct {
	_Bool exists;
	h5uint colour;
	H5Math_Rect rect;

	VEC2(h5float) position; /* meters */
	VEC2(h5float) velocity; /* meters per second */
	VEC2(h5float) force; /* newton */
	h5uint mass;  /* kilograms */

	h5float angle; /* radians */
	h5float angular_velocity; /* radians per second */
	h5float torque; /* newton-meter*/
	h5float moi; /* kilograms per second squared*/

	h5float restitution_coefficient; /*adimensional*/

} H5Physics_Object;

/*userdata struct of H5Coordinate_GraphicalEventData*/
struct H5Test_EventData {
	_Bool *is_paused;
	_Bool *quit;
	H5Render_PixelData screen;
	H5Physics_Object objects[32];
	H5Physics_Object previousObjects[32];
};

int H5Test_simulateOneFrame
    (H5Coordinate_GameplayEventData *opaque_handle);

#endif
