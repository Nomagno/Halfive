/*
Copyright Nomagno 2021, 2022

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
#ifndef H5VI_H
#define H5VI_H

#include <halfive/h5req.h>
#include <stddef.h>
#include <stdint.h>

/*Halfive VIsual interface*/

/*Pixels are RGBA 16-bit 5551 format:
00000 00000 00000 0
RED   GREEN BLUE  ALPHA
*/

typedef struct {
	const size_t w;
	const size_t h;
} H5VI_Point;

/*Pixel buffer*/
typedef struct {
	H5VI_Point size;
	h5uint *pix;
} H5VI_PixelData;

/*Direction keys,
9 general-purpose buttons,
2 mouse buttons,
quit button*/
enum H5VI_KeyEnum {
	up = 0,
	down = 1,
	left = 2,
	right = 3,
	b1 = 4,
	b2 = 5,
	b3 = 6,
	b4 = 7,
	b5 = 8,
	b6 = 9,
	b7 = 10,
	b8 = 11,
	b9 = 12,
	m1 = 13,
	m2 = 14,
	quit = 15
};

typedef struct {
	_Bool keys[16];    /*See enum for what each pos means*/
	h5uchar axis[4];   /*Max 4 axis*/
	h5uint cursor_x; /*Mouse X*/
	h5uint cursor_y; /*Mouse Y*/
} H5VI_InputData;

typedef struct {
	int16_t length_milli;
	_Bool do_loop;
	_Bool loop_length_milli;
	_Bool do_block;

	char name[128];
} H5VI_SoundData;

typedef struct {
	/*
	Platform string examples:
	X11, WAYLAND, WIN32,
	MACOS, ANDROID, SDL
	*/
	char platform[16];
	void *data;
	/*Opaque hangle to identify at
	the very least the display,
	but also possibly the sound
	and input devices. It SHOULD
	NOT be touched by the frontend.
	*/
} H5VI_Reference;

extern unsigned H5VI_Init(H5VI_Reference *buf, size_t h, size_t w);
/*Initialize display*/

extern unsigned H5VI_Destroy(H5VI_Reference *ref);
/*Exit gracefully*/

extern unsigned H5VI_GetBuffer_Size(size_t *h, size_t *w,
				    const char *spritename);
/*Get size of sprite buffer*/

extern unsigned H5VI_GetBuffer_Data(const char *const spritename,
				    H5VI_PixelData *inbuf);
/*Ger sprite buffer copied*/

extern unsigned H5VI_SetBuffer(H5VI_Reference *surf,
			       const H5VI_PixelData *const inbuf);
/*Set display to buffer*/

extern unsigned H5VI_PlaySound(H5VI_Reference *stream,
			       const H5VI_SoundData *const sound);
/*Play in a nonblocking manner (within reason).
The length of the sound will get adjusted if it
is more than that of the sound itself.
*/

extern unsigned H5VI_GetInput(H5VI_Reference *tty, H5VI_InputData *keys);
/*Get current user input*/
#endif
