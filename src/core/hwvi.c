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

/*Available interfaces: MACRO
SDL2 Graphics: HWVI_GSERV_IMPL_SDL2
SDL2 Audio: HWVI_AUDIOSERV_IMPL_SDL2
STDIN Read input: HWVI_STDINPUT_IMPL_PORTABLE
*/

/*SDL2 is available for the following targets (Official or unofficial):
Linux, Android, FreeBSD, OpenBSD, NetBSD, MacOS, Windows,
iOS, AmigaOS, Haiku, tvOS, RISC OS, 3DS, PS Vita, HorizonOS (Nintendo Switch),
PS4... (Probably more)
*/

/*X11 (Graphics server unimplemented) is available for the following targets:
Linux (Xorg, XWayland), FreeBSD (Xorg, XWayland), NetBSD (Xorg), Solaris/Illumos (Xorg),
OpenBSD (Xenocara), Android (Termux-Xorg), MacOS (XFree86 fork), Windows (Cygwin X), Haiku,
AmigaOS, RISC OS... (And a lot more)
*/

/*STDIN reading is available for the following platforms:
All platforms
*/
/*
STDIN input key format (string):
quit,b1,b2,b3,b4,b5,b6,b7,b8,b9,m1,m2,keyup,keydown,keyleft,keyright,axis1,axis2,axis3,axis4

quit, bX, mx, and keyX are booleans 0 or 1, axisX are 8-bit ints 0-255
*/

#include "hwvi.h"

int HWVI_Init(HWVI_Reference *ref, size_t h, size_t w);
int HWVI_Destroy(HWVI_Reference *ref);
int HWVI_PlaySound(HWVI_Reference *stream, const HWVI_SoundData *const insound);

#define HWVI_GSERV_IMPL_SDL2

/*Comment these two macros out to
disable sound and/or input*/
#define HWVI_AUDIOSERV_IMPL_SDL2
#define HWVI_STDINPUT_IMPL_PORTABLE


#if defined(HWVI_GSERV_IMPL_SDL2)
#include <SDL2/SDL.h>
#endif

#if defined(HWVI_GSERV_IMPL_SDL2) || defined(HWVI_AUDIOSERV_IMPL_SDL2)
#define HWVI_GEN_SDL2
#endif

#if defined(HWVI_GEN_SDL2)

#include <SDL2/SDL.h>

/*Implementation-specific global mutable state*/
struct hwvi_sdl_track {
#if defined(HWVI_GSERV_IMPL_SDL2)
	SDL_Surface *globsurf;
	SDL_Surface *convsurf;
	SDL_Window *globwindow;
#endif
#if defined(HWVI_AUDIOSERV_IMPL_SDL2)
	SDL_AudioSpec globsound;
	int globstream;
#endif
	HWVI_InputData input;
};

struct hwvi_sdl_track globalref;

int HWVI_Init(HWVI_Reference *ref, size_t h, size_t w)
{

#if defined(HWVI_AUDIOSERV_IMPL_SDL2)
	int initerror = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	if (initerror) {
		return 1;
	}
	const SDL_AudioSpec dessound = {.freq = 32000,
					.format = AUDIO_U16LSB,
					.channels = 1,
					.silence = 0,
					.samples = 16,
					.size = 0,
					.callback = NULL,
					.userdata = NULL};
	globalref.globstream =
	    SDL_OpenAudioDevice(NULL, 0, &dessound, &globalref.globsound,
				SDL_AUDIO_ALLOW_FORMAT_CHANGE);
	SDL_PauseAudioDevice(globalref.globstream, 0);
#else
	int initerror = SDL_Init(SDL_INIT_VIDEO);
	if (initerror) {
		return 1;
	}
#endif

	globalref.globwindow =
	    SDL_CreateWindow("Half-World Visual Engine", SDL_WINDOWPOS_CENTERED,
			     SDL_WINDOWPOS_CENTERED, w, h, 0);
	globalref.globsurf = SDL_GetWindowSurface(globalref.globwindow);
	ref->data = (void *)&globalref;

	return 0;
}

int HWVI_Destroy(HWVI_Reference *ref)
{
	SDL_DestroyWindow(((struct hwvi_sdl_track *)ref->data)->globwindow);
#ifdef HWVI_AUDIOSERV_IMPL_SDL2
	SDL_CloseAudioDevice(((struct hwvi_sdl_track *)ref->data)->globstream);
#endif
	SDL_Quit();
	return 0;
}

int HWVI_SetBuffer(HWVI_Reference *ref, const HWVI_PixelData *const inbuf)
{
	SDL_Surface *surfptr = ((struct hwvi_sdl_track *)ref->data)->globsurf;
	SDL_LockSurface(surfptr);
	SDL_ConvertPixels(inbuf->size.w, inbuf->size.h,
			  SDL_PIXELFORMAT_RGBA5551, inbuf->pix,
			  2 * (inbuf->size.w), surfptr->format->format,
			  surfptr->pixels, surfptr->pitch);
	SDL_UnlockSurface(surfptr);
	SDL_UpdateWindowSurface(
	    ((struct hwvi_sdl_track *)ref->data)->globwindow);
	return 0;
}

int HWVI_GetBuffer_Size(size_t *h, size_t *w, const char *spritename)
{
	SDL_Surface *surfptr = SDL_LoadBMP(spritename);
	*h = surfptr->h;
	*w = surfptr->w;
	SDL_FreeSurface(surfptr);
	return 0;
}

int HWVI_GetBuffer_Data(const char *spritename, HWVI_PixelData *inbuf)
{
	SDL_Surface *surfptr = SDL_LoadBMP(spritename);
	SDL_ConvertPixels(inbuf->size.w, inbuf->size.h, surfptr->format->format,
			  surfptr->pixels, surfptr->pitch,
			  SDL_PIXELFORMAT_RGBA5551, inbuf->pix,
			  2 * (inbuf->size.w));
	SDL_FreeSurface(surfptr);
	return 0;
}
#else
#error No defined graphics server implementation!
#endif

#ifdef HWVI_AUDIOSERV_IMPL_SDL2

#include "hwvm/hwstring.h"

/*For sound media caching*/

int HWVI_PlaySound(HWVI_Reference *stream, const HWVI_SoundData *const insound)
{
	uint8_t *buf;
	uint32_t size;
	SDL_AudioSpec auspec;

	((struct hwvi_sdl_track *)(stream->data))->globsound =
	    *SDL_LoadWAV(insound->name, &auspec, &(buf), &(size));
	SDL_QueueAudio(globalref.globstream, buf, size);
	SDL_FreeWAV(buf);
	return 0;
}
#else

/*No sound support, stub*/
int HWVI_PlaySound(HWVI_Reference *stream, const HWVI_SoundData *const insound){
	return 0;
}

#endif

#ifdef HWVI_STDINPUT_IMPL_PORTABLE
#include <stdio.h>
int HWVI_GetInput(HWVI_Reference *tty, HWVI_InputData *keys)
{
	tty = tty;
	int i = 0;
	char str[60];
	char *token;
	while (fgets(str, 60, stdin) != NULL)
		;
	token = strtok(str, ",");
	while (token != NULL) {
		if(i < 16) {
		keys->keys[i] = !!hwstrtoul(token, NULL, 10);
		} else if(i < 20){
			keys->axis[i - 16] = (uint8_t)hwstrtoul(token, NULL, 10);
		}
		token = hwstrtok(NULL, ",");
		i+=1;

	}

	return 0;
}
#else
/*No input support, stub*/
int HWVI_GetInput(HWVI_Reference *tty, HWVI_InputData *keys){
	return 0;
}
#endif

/*EXAMPLE:*/
/*
#define WCONSTANT 1080
#define FRAMERATE 60
int main(void)
{
	HWVI_Reference myref;
	uint16_t array_one[WCONSTANT][WCONSTANT] = {0};

	size_t bmpsize_1;
	size_t bmpsize_2;
	HWVI_GetBuffer_Size(&bmpsize_1, &bmpsize_2,
			"../../assets/sprites/zoom.bmp");

	HWVI_PixelData mybuf_green = {.size = {WCONSTANT, WCONSTANT},
				  .pix = &array_one[0][0]};

	if (HWVI_Init(&myref, WCONSTANT, WCONSTANT)) {
		HWVI_Destroy(&myref);
		return 1;
	}

	HWVI_PlaySound(&myref, &(const HWVI_SoundData){
		.name = "../../assets/sound/applause.wav"});

	for (unsigned int i = 0; i < FRAMERATE*8; i++) {
		for (unsigned int j = 0; j < (WCONSTANT * WCONSTANT); j++) {
		mybuf_green.pix[j] =
		((j + i - (j*2)) % 23)
		? 0x0F00 : 0x00F0;
		mybuf_green.pix[j] =
		((j - (i*3)) % 3)
		? (mybuf_green.pix[j] | 0xF260) : (mybuf_green.pix[j] + 0x0FF0 + i);
		    }
		HWVI_SetBuffer(&myref, &mybuf_green);
	}

	HWVI_Destroy(&myref);
	return 0;
}
*/
