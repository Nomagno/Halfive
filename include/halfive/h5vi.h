/*
Copyright Nomagno 2021, 2022, 2023

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

/*Halfive Visual utilities*/

#ifndef H5VI_H
#define H5VI_H

#include <halfive/h5req.h>
#include <halfive/h5render.h>
#include <halfive/h5pix.h>

/*Halfive VIsual interface*/

/*Direction keys,
8 general-purpose buttons,
2 mouse buttons,
quit button,
pause button*/
enum H5VI_KeyEnum {
	H5Key_up	= 0,
	H5Key_down	= 1,
	H5Key_left	= 2,
	H5Key_right = 3,
	H5Key_b1	= 4,
	H5Key_b2	= 5,
	H5Key_b3	= 6,
	H5Key_b4	= 7,
	H5Key_b5	= 8,
	H5Key_b6	= 9,
	H5Key_b7	= 10,
	H5Key_b8	= 11,
	H5Key_m1	= 12,
	H5Key_m2	= 13,
	H5Key_quit	= 14,
	H5Key_pause = 15
};

typedef struct {
	_Bool keys[16];	 /*See enum for what each pos means*/
	h5uchar axis[4]; /*Max 4 axis*/
	h5uint cursor_x; /*Cursor/mouse pointer X*/
	h5uint cursor_y; /*Cursor/mouse pointer Y*/
} H5VI_InputData;

typedef struct {
	_Bool do_block; /*Set to true to disable playing multiple sounds at once*/
	h5uchar volume; /*Relative volume, 0 = mute, 255 = maximum volume, default =
			   127*/

	h5ulong length_milli; /*Length, must not exceed length of the sound, 0 =
				 autodetect sound length*/

	_Bool do_loop;			  /*Set to true to loop at the loop point*/
	h5ulong loop_point_milli; /*At which point in the audio loop?*/

	char name[128]; /*Name*/
} H5VI_SoundData;

typedef struct {
	/*
	Platform string examples:
	X11, WAYLAND, WIN32,
	MACOS, ANDROID, SDL
	*/
	char platform[16];

	_Bool graphics_enabled; /*might be */
	_Bool sound_enabled;	/*might be */
	_Bool input_enabled;	/*might be */

	void *data;
	/*Opaque handle to identify at
	the very least the display,
	but also possibly the sound
	and input devices. It SHOULD
	NOT be dereferenced by the user.
	*/
} H5VI_Reference;

extern unsigned H5VI_init(H5VI_Reference *buf, size_t h, size_t w);
/*Initialize display (following width w and height h, error if not possible),
sound, and input, following the advice of the foo_enabled booleans, and modify
those if can't enable graphics, sound, or input*/

extern unsigned H5VI_destroy(H5VI_Reference *ref);
/*Exit gracefully*/

extern unsigned H5VI_setBuffer(
	H5VI_Reference *handle, const H5Render_PixelData *inbuf);
/*Set display to buffer*/

extern unsigned H5VI_playSound(
	H5VI_Reference *handle, const H5VI_SoundData *sound);
/*Play in a nonblocking manner (within reason).
The length of the sound will get adjusted if it
is more than that of the sound itself.
*/

extern unsigned H5VI_getInput(H5VI_Reference *handle, H5VI_InputData *keys);
/*Get current user input*/
#endif
