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

#include <halfive/h5req.h>
#include <halfive/h5render.h>
#include <halfive/h5pix.h>
#include <halfive/h5vi.h>

/*Available interfaces: MACRO
SDL2 Graphics: H5VI_GSERV_IMPL_SDL2
SDL2 Audio: H5VI_AUDIOSERV_IMPL_SDL2
STDIN Read input: H5VI_STDINPUT_IMPL_PORTABLE
*/

/*SDL2 is available for the following targets (Official or unofficial):
Linux, Android, FreeBSD, OpenBSD, NetBSD, MacOS, Windows,
iOS, AmigaOS, Haiku, tvOS, RISC OS, 3DS, PS Vita, HorizonOS (Nintendo Switch),
PS4... (Probably more)
*/

/*X11 (Graphics server unimplemented) is available for the following targets:
Linux (Xorg, XWayland), FreeBSD (Xorg, XWayland), NetBSD (Xorg), Solaris/Illumos
(Xorg), OpenBSD (Xenocara), Android (Termux-Xorg), MacOS (XFree86 fork), Windows
(Cygwin X), Haiku, AmigaOS, RISC OS... (And a lot more)
*/

/*STDIN reading is available for the following platforms:
All platforms
*/
/*
STDIN input key format (string):
quit,pause,b1,b2,b3,b4,b5,b6,b7,b8,m1,m2,up,down,left,right,axis1,axis2,axis3,axis4

comma-separated booleans (0 or 1) replacing from 'quit' until 'right'
comma separated 8-bit uints (0-255) replacing from 'axis1' until 'axis4'
*/

#if defined(H5VI_GSERV_IMPL_SDL2)
#include <SDL2/SDL.h>
#endif

#if defined(H5VI_GSERV_IMPL_SDL2) || defined(H5VI_AUDIOSERV_IMPL_SDL2)
#define H5VI_GEN_SDL2
#endif

#if defined(H5VI_GEN_SDL2)

#include <SDL2/SDL.h>

/*Implementation-specific global mutable state*/
struct h5vi_sdl_track {
#if defined(H5VI_GSERV_IMPL_SDL2)
	SDL_Surface *globsurf;
	SDL_Surface *convsurf;
	SDL_Window *globwindow;
#endif
#if defined(H5VI_AUDIOSERV_IMPL_SDL2)
	SDL_AudioSpec globsound;
	int globstream;
#endif
	H5VI_InputData input;
};

struct h5vi_sdl_track globalref;

unsigned H5VI_init(H5VI_Reference *ref, size_t h, size_t w)
{
#if defined(H5VI_AUDIOSERV_IMPL_SDL2)
	int initerror = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	if (initerror) {
		return 1;
	}
	const SDL_AudioSpec dessound = {.freq = 32000,
		.format							  = AUDIO_U16LSB,
		.channels						  = 1,
		.silence						  = 0,
		.samples						  = 16,
		.size							  = 0,
		.callback						  = NULL,
		.userdata						  = NULL};
	globalref.globstream		 = SDL_OpenAudioDevice(NULL, 0, &dessound,
		&globalref.globsound, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
	SDL_PauseAudioDevice(globalref.globstream, 0);
#else
	int initerror = SDL_Init(SDL_INIT_VIDEO);
	if (initerror) {
		return 1;
	}
#endif

	globalref.globwindow = SDL_CreateWindow("Halfive Visual Engine",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, 0);
	globalref.globsurf	 = SDL_GetWindowSurface(globalref.globwindow);
	ref->data			 = (void *)&globalref;

	return 0;
}

unsigned H5VI_destroy(H5VI_Reference *ref)
{
	SDL_DestroyWindow(((struct h5vi_sdl_track *)ref->data)->globwindow);
#ifdef H5VI_AUDIOSERV_IMPL_SDL2
	SDL_CloseAudioDevice(((struct h5vi_sdl_track *)ref->data)->globstream);
#endif
	SDL_Quit();
	return 0;
}

unsigned H5VI_setBuffer(H5VI_Reference *ref, const H5Render_PixelData *inbuf)
{
	SDL_Surface *surfptr = ((struct h5vi_sdl_track *)ref->data)->globsurf;
	SDL_LockSurface(surfptr);
	SDL_ConvertPixels(inbuf->width, inbuf->height, SDL_PIXELFORMAT_RGBA5551,
		inbuf->data, 2 * (inbuf->width), surfptr->format->format,
		surfptr->pixels, surfptr->pitch);
	SDL_UnlockSurface(surfptr);
	SDL_UpdateWindowSurface(((struct h5vi_sdl_track *)ref->data)->globwindow);
	return 0;
}
#else
#include <stdlib.h>
#include <halfive/h5stdlib.h>

struct h5vi_gen_track {
	H5Render_PixelData pixels;
	H5VI_InputData input;
};

struct h5vi_gen_track globalref;

unsigned H5VI_init(H5VI_Reference *ref, size_t h, size_t w)
{
	globalref.pixels.height = h;
	globalref.pixels.width	= w;
/*	globalref.pixels.data	= h5malloc(h * w * sizeof(h5uint));*/
	ref->data				= (void *)&globalref;

	return 0;
}

unsigned H5VI_destroy(H5VI_Reference *ref)
{
/*	h5free((((struct h5vi_gen_track *)ref->data)->pixels).data);*/
	return 0;
}

#endif

#ifdef H5VI_AUDIOSERV_IMPL_SDL2
/*For sound media caching*/
unsigned H5VI_playSound(H5VI_Reference *handle, const H5VI_SoundData *insound)
{
	h5uchar *buf;
	uint32_t size;
	SDL_AudioSpec auspec;

	((struct h5vi_sdl_track *)(handle->data))->globsound =
		*SDL_LoadWAV(insound->name, &auspec, &(buf), &(size));
	SDL_QueueAudio(globalref.globstream, buf, size);
	SDL_FreeWAV(buf);
	return 0;
}
#else
/*No sound support, stub*/
unsigned H5VI_playSound(H5VI_Reference *stream, const H5VI_SoundData *insound)
{
	return 0;
}
#endif

#ifndef H5VI_STDINPUT_IMPL_PORTABLE
#include <halfive/h5stdlib.h>
#include <stdio.h>
unsigned H5VI_getInput(H5VI_Reference *handle, H5VI_InputData *keys)
{
	int i = 0;
	char str[60];
	char *token;
	while (fgets(str, 60, stdin) != NULL)
		;
	token = h5strtok(str, ",");
	while (token != NULL) {
		if (i < 16) {
			keys->keys[i] = !!h5strtoul(token, NULL, 10);
		} else if (i < 20) {
			keys->axis[i - 16] = (h5uchar)h5strtoul(token, NULL, 10);
		}
		token = h5strtok(NULL, ",");
		i += 1;
	}

	return 0;
}
#else
/*No input support, stub*/
unsigned H5VI_getInput(H5VI_Reference *handle, H5VI_InputData *keys)
{
	return 0;
}
#endif

#ifdef H5VI_IMAGE_VIEWER
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef H5VI_GSERV_IMPL_SDL2
#define H5VI_GSERV_IMPL_SDL2
#endif

#include <halfive/h5vi.h>
int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("USAGE: pamview file [scale] [tileset file] [tileY,tileX]\n");
		return 1;
	}
	size_t h, w;
	H5Pix_getPAM_Size(argv[1], &h, &w);
	h5uint smallbuf[h][w];
	for (h5ulong y = 0; y < h; y++) {
		for (h5ulong x = 0; x < w; x++) {
			smallbuf[y][x] = 0xFFFF; /*background color is white*/
		}
	}
	H5Render_PixelData smallsprite = {h, w, &smallbuf[0][0]};
	H5Pix_getPAM_Contents(argv[1], smallsprite);

	size_t tileset_h, tileset_w;
	if (argc > 3) {
		H5Pix_getINFO_TilesetSize(argv[3], &tileset_h, &tileset_w);
		char names[tileset_h][tileset_w][32];
		for (size_t i = 0; i < tileset_h; i++) {
			for (size_t j = 0; j < tileset_w; j++) {
				for (size_t k = 0; k < 32; k++) {
					names[i][j][k] = 0;
				}
			}
		}
		TileCategory tags[tileset_h];
		H5Render_Tileset tiles = {.tile_height = 0,
			.tile_width						   = 0,
			.height							   = tileset_h,
			.width							   = tileset_w,
			.padding						   = 0,
			.buffer							   = smallsprite,
			.tags							   = &tags[0],
			.names							   = &names[0][0]};
		H5Pix_getINFO_TilesetContents(argv[3], &tiles);
		for (size_t i = 0; i < tileset_h; i++) {
			printf("\nROW %zu TYPE: %s\n", i,
				(tiles.tags[i] == tile_Symbol)
					? "SYMBOL"
					: ((tiles.tags[i] == tile_Sprite)
							  ? "TILE_SPRITE"
							  : ((tiles.tags[i] == tile_Image) ? "IMAGE"
															   : "UNKNOWN")));
			for (size_t j = 0; j < tileset_w; j++) {
				printf("Y: %zu - X: %zu - %s\n", i, j,
					MATRIX_INDEX(tiles.names, tiles.width, j, i));
			}
		}
	}

	char *endptr   = NULL;
	unsigned scale = (argc > 2) ? strtoul(argv[2], &endptr, 10) : 1;
	h5uint buf[h * scale][w * scale];
	H5Render_PixelData sprite = {h * scale, w * scale, &buf[0][0]};
	H5Render_scale(smallsprite, sprite, scale);

	H5VI_Reference myref;
	if (H5VI_init(&myref, h * scale, w * scale)) {
		printf("ERROR: failed to initialize H5VI screen");
		H5VI_destroy(&myref);
		return 1;
	}
	H5VI_setBuffer(&myref, &sprite);

	while (1) {
		char a[10] = {0};
		fgets(a, sizeof(a), stdin);
		switch (a[0]) {
		case 'q':
			goto exit;
			break;
		case 'r':
			nanosleep(&(struct timespec){3, 0}, NULL);
			break;
		}
		H5VI_setBuffer(&myref, &sprite);
	}
exit:
	H5VI_destroy(&myref);
	return 0;
}
#endif

#ifdef H5VI_TEST
#include <halfive/h5stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define WCONSTANT 500
#define HCONSTANT 500
int main(void)
{
	H5VI_Reference myref;
	h5uint array_one[HCONSTANT][WCONSTANT] = {0};
	for (h5ulong y = 0; y < HCONSTANT; y++) {
		for (h5ulong x = 0; x < WCONSTANT; x++) {
			array_one[y][x] = 0xFFFF; /*WHITE*/
		}
	}

	H5Render_PixelData mybuf = {HCONSTANT, WCONSTANT, .data = &array_one[0][0]};
	if (H5VI_init(&myref, HCONSTANT, WCONSTANT)) {
		H5VI_destroy(&myref);
		return 1;
	}

	H5VI_setBuffer(&myref, &mybuf);
	while (1) {
		_Bool exit = 0;
		H5VI_setBuffer(&myref, &mybuf);
		char a[128] = {0};
		fgets(a, sizeof(a), stdin);
		switch (a[0]) {
		case 'q': /*Quit the program*/
			exit = 1;
			break;
		case 'r': /*Reload screen in 3 seconds*/
			nanosleep(&(struct timespec){3, 0}, NULL);
			break;
		case '1': /*Demo 1: Reload, wait 1 second, draw a triangle, move it*/
			nanosleep(&(struct timespec){3, 0}, NULL);
			H5VI_setBuffer(&myref, &mybuf);
			nanosleep(&(struct timespec){1, 0}, NULL);
			for (int i = 0; i < 50; i++) {
				H5Render_ulong_drawPolygon(mybuf,
					(h5point_ulong[]){POINT_UL(50 + i * 4, 50 + i * 2),
						POINT_UL(150 + i * 4, 50 + i * 2),
						POINT_UL(190 + i * 4, 190 + i * 2),
						POINT_UL(100 + i * 4, 240 + i * 2),
						POINT_UL(10 + i * 4, 190 + i * 2)},
					5, 0xF223);
				H5VI_setBuffer(&myref, &mybuf);
				nanosleep(&(struct timespec){0, 10000000}, NULL);
				H5Render_fill(mybuf, 0xFFFF);
			}
			break;
			nanosleep(&(struct timespec){3, 0}, NULL);
		case '-':
			nanosleep(&(struct timespec){3, 0}, NULL);
			H5VI_setBuffer(&myref, &mybuf);
			nanosleep(&(struct timespec){1, 0}, NULL);
		}
		if (exit)
			break;
	}

	H5VI_destroy(&myref);
	return 0;
}
#endif
