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
#define H5KEY_UP 0
#define H5KEY_DOWN 1
#define H5KEY_LEFT 2
#define H5KEY_RIGHT 3
#define H5KEY_B1 4
#define H5KEY_B2 5
#define H5KEY_B3 6
#define H5KEY_B4 7
#define H5KEY_B5 8
#define H5KEY_B6 9
#define H5KEY_B7 10
#define H5KEY_B8 11
#define H5KEY_M1 12
#define H5KEY_M2 13
#define H5KEY_QUIT 14
#define H5KEY_PAUSE 15

typedef struct {
	_Bool isActive;
	_Bool isPressed;
	unsigned key_code;
	VEC2(h5ulong) size;
	VEC2(h5ulong) pos;
} H5VI_VirtualButton;

typedef struct {
	h5uint view_height, view_width;
	/*Viewport width and height. This is needed for touch input only
	  because of how the finger reporting is done by the operating system,
	  can be left as zero otherwise*/
	H5VI_VirtualButton virKeys[64];
	_Bool keys[16]; /*See enum for what each pos means*/
	_Bool previous_keys[16]; /*This is for detecting key press / key release*/
	unsigned long fetch_elapsed[16]; /*How many input polls passed since each key press. Usually frames*/

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
	MACOS, ANDROID, SDL2
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

unsigned H5VI_init(H5VI_Reference *buf, size_t h, size_t w);
/*Initialize display (following width w and height h, error if not possible),
sound, and input, following the advice of the foo_enabled booleans, and modify
those if can't enable graphics, sound, or input. Additionally, set the platform string*/

unsigned H5VI_destroy(H5VI_Reference *ref);
/*Exit gracefully*/

unsigned H5VI_setBuffer(
	H5VI_Reference *handle, const H5Render_PixelData *inbuf);
/*Set display to buffer*/

unsigned H5VI_playSound(
	H5VI_Reference *handle, const H5VI_SoundData *sound);
/*Play in a nonblocking manner (within reason).
The length of the sound will get adjusted if it
is more than that of the sound itself.
*/

/*Get current user input*/
unsigned H5VI_getInput(H5VI_Reference *handle, H5VI_InputData *keys);
/*Get current user input based on virtual key presses*/
unsigned H5VI_getVirtualInput(H5VI_Reference *handle, H5VI_InputData *keys);
unsigned H5VI_addVirtualButton(H5VI_Reference *handle, H5VI_InputData *keys, H5VI_VirtualButton button);
unsigned H5VI_defaultVirtualLayout(H5VI_Reference *handle, H5VI_InputData *keys);
unsigned H5VI_renderVirtualButtons(H5Render_PixelData surf, H5VI_InputData *keys, h5uint colour);


unsigned H5VI_updateDelayData(H5VI_Reference *handle, H5VI_InputData *keys);
_Bool H5VI_isOnPress(H5VI_InputData *keys, unsigned key_index);
_Bool H5VI_isOnRelease(H5VI_InputData *keys, unsigned key_index);
_Bool H5VI_autoRepeat(H5VI_InputData *keys, unsigned key_index, _Bool initial_press_allowed, unsigned long initial_delay, unsigned long autoRepeatTime);
unsigned long H5VI_pressTime(H5VI_InputData *keys, unsigned key_index);

#endif
