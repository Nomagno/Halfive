#include <halfive/h5req.h>
#include <halfive/h5render.h>
#include <halfive/h5vi.h>

#ifndef H5COORD_H
#define H5COORD_H

typedef struct {
	H5VI_Reference *ref;
	long int time_available; /*nanoseconds*/
	long int delta_time; /*nanoseconds*/
	long int *time_taken; /*nanoseconds, responsibility of the called function for now*/
	void const *const_userdata;
	void *userdata;
} H5Coordinate_GenericEventData;

typedef struct {
	H5VI_Reference *ref;
	long int time_available; /*nanoseconds*/
	long int delta_time; /*nanoseconds*/
	long int *time_taken; /*nanoseconds, responsibility of the called function for now*/
	H5VI_InputData input_keys;
	H5Render_PixelData rendered_output;
	void const *const_userdata;
	void *userdata;
} H5Coordinate_GraphicalEventData;

#endif
