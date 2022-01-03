/*
Copyright Nomagno 2021

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

#include <stdint.h>
#include <stddef.h>

/*Half-Workd VIsual interface*/

/*Pixels are RGBA 16-bit 5551 format:
00000 00000 00000 0
RED   GREEN BLUE  ALPHA
*/

typedef struct {
	const size_t w;
	const size_t h;
} hwvi_point;

typedef struct {
	hwvi_point size;
	uint16_t **pix;
} hwvi_pixbuf;

enum hwvi_key {
	b1, b2, b3,
	b4, b5, b6,
	b7, b8, b9,
	m1, m2, quit
};

typedef struct {
	enum hwvi_key keys[5];
	uint8_t axis[4];
	uint16_t cursor_x;
	uint16_t cursor_y;
} hwvi_input;

typedef struct {
	long int length_milliseconds;
	char name[16];
} hwvi_sound;

typedef struct {
	/*
	Platform string examples:
	X11, WAYLAND, WIN32,
	MACOS, ANDROID, SDL
	*/
	char platform[16];
	void *data;
} hwvi_ref;

extern int hwvi_init(hwvi_ref *ref, 
size_t h, size_t w, hwvi_pixbuf *inbuf); 
/*Initialize display*/

extern hwvi_point hwvi_getbufsize(
const char *spritename);
/*Get size of sprite buffer*/

extern int hwvi_getbufpix(const char *spritename, 
hwvi_pixbuf *inbuf);
/*Ger sprite buffer copied*/

extern int hwvi_setbuf(hwvi_ref *surf, 
const hwvi_pixbuf *inbuf);
/*Set display to buffer*/

extern int hwvi_playsound(hwvi_ref *stream, 
_Bool do_block, const hwvi_sound *sound);
/*Play sound in a blocking or nonblocking manner.
It will get truncated or silence will be added, 
if the specified length does not match that of
the sound, ONLY IF the sound is blocking. Else
the length parameter gets ignored*/

extern int hwvi_getinput(hwvi_ref *tty, 
const hwvi_input *keys);
/*Get current user input*/
