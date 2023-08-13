#include <halfive/h5req.h>
#include <halfive/h5render.h>
#include <halfive/h5vi.h>

#ifndef H5COORD_H
#define H5COORD_H

typedef int (*H5Coordinate_EventHandler)(void *);

typedef enum {
	H5Coordinate_kind_generic,
	H5Coordinate_kind_graphical,
	H5Coordinate_kind_audio,
	H5Coordinate_kind_physics
} H5Coordinate_EventKind;

typedef struct {
	H5Coordinate_EventHandler handler;
	H5Coordinate_EventKind kind; 
	void *const_data;
	void *userdata;
} H5Coordinate_Event;

typedef struct {
	unsigned queue_ptr;
	H5Coordinate_Event events[16];
} H5Coordinate_EventQueue;

#define PUSH(myqueue, val) myqueue.events[++myqueue.queue_ptr] = val
#define POP(myqueue, val) myqueue.events[--myqueue.queue_ptr]


typedef struct {
	H5VI_Reference *ref;
	h5umax time_available; /*nanoseconds*/
	h5umax delta_time; /*nanoseconds*/
	h5umax *time_taken; /*nanoseconds, responsibility of the called function for now*/
	void const *const_userdata;
	void *userdata;
} H5Coordinate_GenericEventData;

typedef struct {
	H5VI_Reference *ref;
	const h5umax time_available; /*nanoseconds*/
	h5umax delta_time; /*nanoseconds*/
	h5umax *time_taken; /*nanoseconds, responsibility of the called function for now*/
	H5VI_InputData input_keys;
	void const *const_userdata;
	void *userdata;
} H5Coordinate_GameplayEventData;

typedef struct {
	H5VI_Reference *ref;
	h5umax time_available; /*Usually 1/framerate*/
	h5umax delta_time; /*nanoseconds*/
	h5umax *time_taken; /*nanoseconds, responsibility of the called function for now*/
	unsigned long frame_counter; /*Increases once per frame*/
	H5VI_InputData input_keys;
	H5Render_PixelData rendered_output;
	void const *const_userdata;
	void *userdata;
} H5Coordinate_GraphicalEventData;

#endif
