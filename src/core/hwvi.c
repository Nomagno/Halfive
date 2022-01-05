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
*/


#include "hwreq.h"
#include "hwvi.h"

/*SDL2 Interface only allows for initializing and setting the display, at the moment*/
#define HWVI_GSERV_IMPL_SDL2
/*
#define HWVI_AUDIOSERV_IMPL_SDL2
*/
#if defined(HWVI_GSERV_IMPL_SDL2)
#include <SDL2/SDL.h>
#endif

#if defined(HWVI_GSERV_IMPL_SDL2) || \
defined(HWVI_AUDIOSERV_IMPL_SDL2)
#define HWVI_GEN_SDL2
#endif

#if defined(HWVI_GEN_SDL2)

#include <SDL2/SDL.h>

/*Implementation-specific global mutable state*/
struct hwvi_sdl_track{
	#if defined(HWVI_GSERV_IMPL_SDL2)
	SDL_Surface *globsurf;
	SDL_Surface *convsurf;
	SDL_Window *globwindow;
	#endif
	#if defined(HWVI_AUDIOSERV_IMPL_SDL2)
	SDL_AudioSpec globsound;
	int globstream;
	#endif
	hwvi_input input;
};

/*For sound media caching*/
struct hwvi_sdl_soundbuf {
	hwvi_sound metadata;
	const size_t size;
	uint8_t *buffer;
};

struct hwvi_sdl_track globalref;
struct hwvi_sdl_soundbuf globsoundbuf[50];

int hwvi_init(hwvi_ref *ref, size_t h, size_t w)
{

	#if defined(HWVI_AUDIOSERV_IMPL_SDL2)
	int initerror = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	if(initerror){
		return 1;
	}
	const SDL_AudioSpec dessound = {
		.freq = 32000,
		.format = AUDIO_U16LSB,
		.channels = 1,
		.silence = 0,
		.samples = 16,
		.size = 0,
		.callback = NULL,
		.userdata = NULL
		};
	globalref.globstream = SDL_OpenAudioDevice(NULL, 0, &dessound,
	         &globalref.globsound, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
	SDL_PauseAudioDevice(globalref.globstream, 0);
	#else
	int initerror =	SDL_Init(SDL_INIT_VIDEO);
	if(initerror){
		return 1;
	}
	#endif

	globalref.globwindow = SDL_CreateWindow("Half-World Visual Engine",
	          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, 0);
	globalref.globsurf = SDL_GetWindowSurface(globalref.globwindow);
	ref->data = (void *)&globalref;

	return 0;
}

extern int hwvi_destroy (hwvi_ref *ref)
{
	SDL_DestroyWindow(((struct hwvi_sdl_track *)ref->data)->globwindow);
	#ifdef HWVI_AUDIOSERV_IMPL_SDL2
	SDL_CloseAudioDevice(((struct hwvi_sdl_track *)ref->data)->globstream);
	#endif
	SDL_Quit();
	return 0;
}

extern int hwvi_setbuf(hwvi_ref *ref, const hwvi_pixbuf *const inbuf)
{
	SDL_Surface *surfptr = ((struct hwvi_sdl_track *)ref->data)->globsurf;
	SDL_LockSurface(surfptr);
	SDL_ConvertPixels(inbuf->size.w, inbuf->size.h, SDL_PIXELFORMAT_RGBA5551, inbuf->pix, 2*(inbuf->size.w),
	                  surfptr->format->format, surfptr->pixels, surfptr->pitch);
	SDL_UnlockSurface(surfptr);
	SDL_UpdateWindowSurface(((struct hwvi_sdl_track *)ref->data)->globwindow);
	return 0;
}

extern int

#else
#error You need to define an implementation preprocessor macro
#endif
/*
#define WCONSTANT 700
int main(void)
{
	hwvi_ref myref;
	uint16_t array_one[WCONSTANT][WCONSTANT] = {0};
	uint16_t array_two[WCONSTANT][WCONSTANT] = {0};


	hwvi_pixbuf mybuf_blue = { .size = {WCONSTANT, WCONSTANT}, .pix = &array_one[0][0] };
	hwvi_pixbuf mybuf_green = { .size = {WCONSTANT, WCONSTANT}, .pix = &array_two[0][0] };

	for(int i = 0; i < WCONSTANT*WCONSTANT; i++){
		mybuf_blue.pix[i] = (i % 2) ? 0x0F00 : 0x00F0;
		mybuf_green.pix[i] = (i % 2) ? 0xF000 : 0xFFF0;
	}

	if (hwvi_init(&myref, WCONSTANT, WCONSTANT)){
	hwvi_destroy(&myref);
	return 1;
	}

	for(int i = 0; i < 6; i++){
		hwvi_setbuf(&myref, &mybuf_blue);
		sleep(1);
		hwvi_setbuf(&myref, &mybuf_green);
		sleep(1);
	}

	hwvi_destroy(&myref);
	return 0;
}
*/
