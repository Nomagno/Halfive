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

#include <halfive/h5stdlib.h>
#include <halfive/h5req.h>
#include <halfive/h5render.h>
#include <halfive/h5pix.h>
#include <halfive/h5vi.h>

/*
Available interfaces:
SDL2 Graphics: H5VI_GSERV_IMPL_SDL2
SDL2 Audio: H5VI_AUDIOSERV_IMPL_SDL2
SDL2 Input: H5VI_STDINPUT_IMPL_SDL2
Portable Input (STDIN): H5VI_STDINPUT_IMPL_PORTABLE

SDL2 is available for the following targets (Official or unofficial):
Linux, Android, FreeBSD, OpenBSD, NetBSD, MacOS, Windows,
iOS, AmigaOS, Haiku, tvOS, RISC OS, 3DS, PS Vita, HorizonOS (Nintendo Switch),
PS4, Emscriptem... (Probably more)

X11 (Graphics server unimplemented) is available for the following targets:
Linux (Xorg, XWayland), FreeBSD (Xorg, XWayland), NetBSD (Xorg), Solaris/Illumos
(Xorg), OpenBSD (Xenocara), Android (Termux-Xorg), MacOS (XFree86 fork), Windows
(Cygwin X), Haiku, AmigaOS, RISC OS... (And a lot more)

Portable input is available for the following platforms:
All platforms

STDIN input key format (string):
quit,pause,b1,b2,b3,b4,b5,b6,b7,b8,m1,m2,up,down,left,right,axis1,axis2,axis3,axis4

comma-separated booleans (0 or 1) replacing from 'quit' until 'right'
comma separated 8-bit uints (0-255) replacing from 'axis1' until 'axis4'
*/

#include <halfive/code_setup.h>

#define JOYSTICK_INDEX 0

#define H5VI_GSERV_IMPL_SDL2
#define H5VI_STDINPUT_IMPL_SDL2

#if !defined(H5VI_GSERV_IMPL_SDL2)
#error Please define a graphics implementation (src/core/h5vi.c)
#endif

#if !defined(H5VI_STDINPUT_IMPL_SDL2) && !defined(H5VI_STDINPUT_IMPL_STDINPUT)
#error Please define an input implementation (src/core/h5vi.c)
#endif

#if defined(H5VI_GSERV_IMPL_SDL2) || defined(H5VI_AUDIOSERV_IMPL_SDL2)
#define H5VI_GEN_SDL2
#endif

#if defined(H5VI_GEN_SDL2)

#include <SDL2/SDL.h>

/*Implementation-specific global mutable state*/
struct h5vi_sdl2_track {
#if defined(H5VI_GSERV_IMPL_SDL2)
	SDL_Surface *globsurf;
	SDL_Surface *convsurf;
	SDL_Window *globwindow;
#endif
#if defined(H5VI_AUDIOSERV_IMPL_SDL2)
	SDL_AudioSpec globsound;
	int globstream;
#endif
#if defined(H5VI_STDINPUT_IMPL_SDL2)
	/*SDL_Joystick *controller;*/
#endif
	H5VI_InputData input;
	uint32_t globPixels[(1 << 15) << 1];
};

void generatePrecomputation_RGBA5551_to_RGBB888(uint32_t arr[(1 << 15) << 1], uint8_t r_pos, uint8_t g_pos, uint8_t b_pos, uint8_t a_pos) {
	for (uint_fast32_t i = 0; i < (1 << 15); i++) {
		arr[(i << 1) | 0] |= (((i >> 10) & 0x1F)*0xFF/0x1F) << (r_pos * 8);
		arr[(i << 1) | 1] |= (((i >> 10) & 0x1F)*0xFF/0x1F) << (r_pos * 8);

		arr[(i << 1) | 0] |= (((i >> 5) & 0x1F)*0xFF/0x1F) << (g_pos * 8);
		arr[(i << 1) | 1] |= (((i >> 5) & 0x1F)*0xFF/0x1F) << (g_pos * 8);

		arr[(i << 1) | 0] |= (((i >> 0) & 0x1F)*0xFF/0x1F) << (b_pos * 8);
		arr[(i << 1) | 1] |= (((i >> 0) & 0x1F)*0xFF/0x1F) << (b_pos * 8);

		if (a_pos != 0xFF) {
			arr[(i << 1) | 0] |= 0xFF;
			arr[(i << 1) | 1] |= 0xFF;
		}
	}
}

struct h5vi_sdl2_track globalref;

#include <stdio.h>
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
/*
	SDL_JoystickEventState(SDL_ENABLE);
	SDL_PumpEvents();
	for (int i = 0; i < SDL_NumJoysticks(); i++) {
		printf("CTRL: %s\n", SDL_JoystickNameForIndex(i));
	}
	globalref.controller = SDL_JoystickOpen(JOYSTICK_INDEX);
	printf("CTRLNUM: %i\n",  SDL_NumJoysticks());
*/
	ref->platform[0] = 'S';
	ref->platform[1] = 'D';
	ref->platform[2] = 'L';
	ref->platform[3] = '2';
	ref->platform[4] =  0 ;
	
	globalref.globwindow = SDL_CreateWindow("Halfive Visual Engine",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, 0);
	globalref.globsurf	 = SDL_GetWindowSurface(globalref.globwindow);

	SDL_PixelFormat *format = globalref.globsurf->format;
	uint32_t formatEnum = globalref.globsurf->format->format;
	if (format->BytesPerPixel != 4 || format->Rmask == 0) {
		maybe_printf("FATAL: Pixel format of window isn't supported, must be 4 bytes long and non-palette-based: Name: %s - BitsPerPixel: %i, BytesPerPixel: %i - RGBA Mask %X / %X / %X / %X\n",
		       SDL_GetPixelFormatName(formatEnum), format->BitsPerPixel, format->BytesPerPixel, 
		       format->Rmask, format->Gmask, format->Bmask, format->Amask);
		return 1;
	}

	uint8_t r_pos;
	uint8_t g_pos;
	uint8_t b_pos;
	uint8_t a_pos;

	switch(format->Rmask) {
		case 0xFF000000:
			r_pos = !IS_LITTLE_ENDIAN ? 0 : 3;
			break;
		case 0x00FF0000:
			r_pos = !IS_LITTLE_ENDIAN ? 1 : 2;
			break;
		case 0x0000FF00:
			r_pos = !IS_LITTLE_ENDIAN ? 2 : 1;
			break;
		case 0x000000FF:
			r_pos = !IS_LITTLE_ENDIAN ? 3 : 0;
			break;
	}	

	switch(format->Gmask) {
		case 0xFF000000:
			g_pos = !IS_LITTLE_ENDIAN ? 0 : 3;
			break;
		case 0x00FF0000:
			g_pos = !IS_LITTLE_ENDIAN ? 1 : 2;
			break;
		case 0x0000FF00:
			g_pos = !IS_LITTLE_ENDIAN ? 2 : 1;
			break;
		case 0x000000FF:
			g_pos = !IS_LITTLE_ENDIAN ? 3 : 0;
			break;
	}	

	switch(format->Bmask) {
		case 0xFF000000:
			b_pos = !IS_LITTLE_ENDIAN ? 0 : 3;
			break;
		case 0x00FF0000:
			b_pos = !IS_LITTLE_ENDIAN ? 1 : 2;
			break;
		case 0x0000FF00:
			b_pos = !IS_LITTLE_ENDIAN ? 2 : 1;
			break;
		case 0x000000FF:
			b_pos = !IS_LITTLE_ENDIAN ? 3 : 0;
			break;
	}	

	switch(format->Amask) {
		case 0xFF000000:
			a_pos = !IS_LITTLE_ENDIAN ? 0 : 3;
			break;
		case 0x00FF0000:
			a_pos = !IS_LITTLE_ENDIAN ? 1 : 2;
			break;
		case 0x0000FF00:
			a_pos = !IS_LITTLE_ENDIAN ? 2 : 1;
			break;
		case 0x000000FF:
			a_pos = !IS_LITTLE_ENDIAN ? 3 : 0;
			break;
		case 0:
			a_pos = 0xFF;
			break;
	}

	maybe_printf("H5Vi: Precomputing pixel conversion...\n");
	generatePrecomputation_RGBA5551_to_RGBB888(globalref.globPixels, r_pos, g_pos, b_pos, a_pos);

	ref->data			 = (void *)&globalref;

	return 0;
}

unsigned H5VI_destroy(H5VI_Reference *ref)
{
	SDL_DestroyWindow(((struct h5vi_sdl2_track *)ref->data)->globwindow);
#ifdef H5VI_AUDIOSERV_IMPL_SDL2
	SDL_CloseAudioDevice(((struct h5vi_sdl2_track *)ref->data)->globstream);
#endif
	SDL_Quit();
	return 0;
}

unsigned H5VI_setBuffer(H5VI_Reference *ref, const H5Render_PixelData *inbuf)
{
	SDL_Surface *surfptr = ((struct h5vi_sdl2_track *)ref->data)->globsurf;

	SDL_LockSurface(surfptr);

	for (unsigned long i = 0; i < inbuf->width*inbuf->height; i++) {
		uint32_t *pix = ((uint32_t *)(surfptr->pixels)) + i;
		uint16_t indx = inbuf->data[i];
		*pix = ((struct h5vi_sdl2_track *)ref->data)->globPixels[indx];
	}
	SDL_UnlockSurface(surfptr);
	SDL_UpdateWindowSurface(((struct h5vi_sdl2_track *)ref->data)->globwindow);
	return 0;
}
#else

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

	((struct h5vi_sdl2_track *)(handle->data))->globsound =
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

unsigned H5VI_updateDelayData(H5VI_Reference *handle, H5VI_InputData *keys) {
	for (size_t i = 0; i < ELEMNUM(keys->keys); i++){
		if (keys->keys[i] && !keys->previous_keys[i]) {
			keys->fetch_elapsed[i] = 0;
		} else if (keys->keys[i] && keys->previous_keys[i]) {
			keys->fetch_elapsed[i] += 1;
		} else if (!keys->keys[i] && keys->previous_keys[i]) {
			keys->fetch_elapsed[i] = 0;
		} else if (!keys->keys[i] && !keys->previous_keys[i]) {

		}
	}
	return 0;
}

_Bool H5VI_isOnPress(H5VI_InputData *keys, unsigned key_index) {
	if (keys->keys[key_index] == 1 && keys->previous_keys[key_index] == 0) {
		return 1;
	} else {
		return 0;
	}
}

_Bool H5VI_isOnRelease(H5VI_InputData *keys, unsigned key_index) {
	if (keys->keys[key_index] == 0 && keys->previous_keys[key_index] == 1) {
		return 1;
	} else {
		return 0;
	}
}

_Bool H5VI_autoRepeat(H5VI_InputData *keys, unsigned key_index, _Bool initial_press_allowed, unsigned long initial_delay, unsigned long autoRepeatTime) {
	if (keys->keys[key_index] == 1) {
		if (keys->fetch_elapsed[key_index] > initial_delay) {
			if (keys->fetch_elapsed[key_index] % autoRepeatTime == 0) {
				return 1;
			} else if ((keys->fetch_elapsed[key_index] == initial_delay)) {
				return 1;
			} else {
				return 0;
			}
		} else if (keys->fetch_elapsed[key_index] == 0 && initial_press_allowed) {
			return 1;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

unsigned long H5VI_pressTime(H5VI_InputData *keys, unsigned key_index) {
	return keys->fetch_elapsed[key_index];
}

unsigned H5VI_addVirtualButton(H5VI_Reference *handle, H5VI_InputData *keys, H5VI_VirtualButton button) {
	unsigned retcode = 1;
	for (unsigned i = 0; i < ELEMNUM(keys->virKeys); i++) {
		if (keys->virKeys[i].isActive == 0) {
			keys->virKeys[i] = button;
			retcode = 0;
			break;
		}
	}
	return retcode;
}


unsigned H5VI_defaultVirtualLayout(H5VI_Reference *handle, H5VI_InputData *keys) {

	unsigned max_height = keys->view_height/2;
	unsigned max_width_left = keys->view_width/3;
	unsigned max_width_right = keys->view_width-max_width_left;

	H5VI_VirtualButton dpad_up = {
		.isActive = 1,
		.isPressed = 0,
		.key_code = H5KEY_UP,
		.pos = {max_width_left/3*1, max_height+(max_height/3*0)},
		.size = { max_width_left/3, (max_height/3)},
	};
	H5VI_VirtualButton dpad_down = {
		.isActive = 1,
		.isPressed = 0,
		.key_code = H5KEY_DOWN,
		.pos = {max_width_left/3*1, max_height+(max_height/3*2)},
		.size = { max_width_left/3, (max_height/3)},
	};

	H5VI_VirtualButton dpad_left = {
		.isActive = 1,
		.isPressed = 0,
		.key_code = H5KEY_LEFT,
		.pos = {max_width_left/3*0, max_height+(max_height/3)},
		.size = { max_width_left/3, (max_height/3)},
	};

	H5VI_VirtualButton dpad_right = {
		.isActive = 1,
		.isPressed = 0,
		.key_code = H5KEY_RIGHT,
		.pos = {max_width_left/3*2, max_height+(max_height/3)},
		.size = { max_width_left/3, (max_height/3)},
	};


	H5VI_VirtualButton b1 = {
		.isActive = 1,
		.isPressed = 0,
		.key_code = H5KEY_B1,
		.pos = {max_width_right+(max_width_left/3*2), max_height+(max_height/3)},
		.size = { max_width_left/3, (max_height/3)},
	};
	H5VI_VirtualButton b2 = {
		.isActive = 1,
		.isPressed = 0,
		.key_code = H5KEY_B2,
		.pos = {max_width_right+(max_width_left/3*1), max_height+(max_height/3*2)},
		.size = { max_width_left/3, (max_height/3)},
	};
	H5VI_VirtualButton b3 = {
		.isActive = 1,
		.isPressed = 0,
		.key_code = H5KEY_B3,
		.pos = {max_width_right+(max_width_left/3*1), max_height+(max_height/3*0)},
		.size = { max_width_left/3, (max_height/3)},
	};
	H5VI_VirtualButton b4 = {
		.isActive = 1,
		.isPressed = 0,
		.key_code = H5KEY_B4,
		.pos = {max_width_right+(max_width_left/3*0), max_height+(max_height/3)},
		.size = { max_width_left/3, (max_height/3)},
	};

	H5VI_addVirtualButton(handle, keys, dpad_up);
	H5VI_addVirtualButton(handle, keys, dpad_down);
	H5VI_addVirtualButton(handle, keys, dpad_left);
	H5VI_addVirtualButton(handle, keys, dpad_right);
	H5VI_addVirtualButton(handle, keys, b1);
	H5VI_addVirtualButton(handle, keys, b2);
	H5VI_addVirtualButton(handle, keys, b3);
	H5VI_addVirtualButton(handle, keys, b4);

	return 0;
}

unsigned H5VI_renderVirtualButtons(H5Render_PixelData surf, H5VI_InputData *keys, h5uint colour) {
	for (unsigned i = 0; i < ELEMNUM(keys->virKeys); i++) {
		if (keys->virKeys[i].isActive == 1) {
			VEC2(h5ulong) p1 = { keys->virKeys[i].pos.x, keys->virKeys[i].pos.y };
			VEC2(h5ulong) p2 = { keys->virKeys[i].pos.x+keys->virKeys[i].size.x, keys->virKeys[i].pos.y };
			VEC2(h5ulong) p3 = { keys->virKeys[i].pos.x+keys->virKeys[i].size.x, keys->virKeys[i].pos.y+keys->virKeys[i].size.y };
			VEC2(h5ulong) p4 = { keys->virKeys[i].pos.x, keys->virKeys[i].pos.y+keys->virKeys[i].size.y };
			H5Render_ulong_drawRectangle(surf, p1, p2, p3, p4, colour);
		}
	}
	return 0;
}

/*Do not negate existing hardware buttonpresses, only add one if there isn't already*/
void updateVirtualInputNonAuthoritative(H5VI_Reference *handle, H5VI_InputData *keys, unsigned x, unsigned y) {
	for (unsigned i = 0; i < ELEMNUM(keys->virKeys); i++) {
		if (keys->virKeys[i].isActive == 1) {
			if (x > keys->virKeys[i].pos.x && x < (keys->virKeys[i].pos.x + keys->virKeys[i].size.x) &&
			    y > keys->virKeys[i].pos.y && y < (keys->virKeys[i].pos.y + keys->virKeys[i].size.y))
			{
				keys->virKeys[i].isPressed = 1;
				keys->keys[keys->virKeys[i].key_code] = 1;
			} else {
				keys->virKeys[i].isPressed = 0;
			}
		}
	}
}


#ifdef H5VI_STDINPUT_IMPL_SDL2

#define H5IN_K_UP SDL_SCANCODE_UP
#define H5IN_K_DOWN SDL_SCANCODE_DOWN
#define H5IN_K_LEFT SDL_SCANCODE_LEFT
#define H5IN_K_RIGHT SDL_SCANCODE_RIGHT
#define H5IN_K_B1 SDL_SCANCODE_A
#define H5IN_K_B2 SDL_SCANCODE_S
#define H5IN_K_B3 SDL_SCANCODE_D
#define H5IN_K_B4 SDL_SCANCODE_F
#define H5IN_K_B5 SDL_SCANCODE_Z
#define H5IN_K_B6 SDL_SCANCODE_X
#define H5IN_K_B7 SDL_SCANCODE_C
#define H5IN_K_B8 SDL_SCANCODE_V
#define H5IN_K_QUIT SDL_SCANCODE_1
#define H5IN_K_PAUSE SDL_SCANCODE_2

#define H5IN_J_UP getB_fromA_dual(H5IN_J_LEFTX, 1, 50)
#define H5IN_J_DOWN getB_fromA_dual(H5IN_J_LEFTX, 0, 50)
#define H5IN_J_LEFT getB_fromA_dual(H5IN_J_LEFTY, 1, 50)
#define H5IN_J_RIGHT getB_fromA_dual(H5IN_J_LEFTY, 0, 50)
#define H5IN_J_B1 1
#define H5IN_J_B2 0
#define H5IN_J_B3 4
#define H5IN_J_B4 3
#define H5IN_J_B5 6
#define H5IN_J_B6 7
#define H5IN_J_B7 13
#define H5IN_J_B8 14
#define H5IN_J_QUIT 10
#define H5IN_J_PAUSE 11

#define H5IN_J_LEFTX 0
#define H5IN_J_LEFTY 1
#define H5IN_J_TLEFT 4
#define H5IN_J_TRIGHT 5


/*_Bool getB(unsigned x) {
	return SDL_JoystickGetButton(globalref.controller, x);
}*/

/*h5uchar getA(unsigned x) {
	return (255*SDL_JoystickGetAxis(globalref.controller, x)/65535)+128;
}*/

/*_Bool getB_fromA_dual(h5uchar axis_u, _Bool sign, h5uchar deadzone){
//0 positive, 1 negative
	//h5schar axis = axis_u - 128; if ((H5_ABS(axis) - deadzone > 0) && ((sign == 0) ? ( axis > 0 ) : ( axis < 0 ))) { return 1; }
	return 0;
}*/

/*_Bool getB_fromA_single(h5uchar axis, h5uchar deadzone){
	if (axis - deadzone > 0) { return 1; }
	else return 0;
}*/

unsigned H5VI_getVirtualInput(H5VI_Reference *handle, H5VI_InputData *keys) {
	SDL_TouchID touchDevice = SDL_GetTouchDevice(0);
	unsigned fingernum = SDL_GetNumTouchFingers(touchDevice);

	if (keys->keys[H5KEY_M1] == 1) {
		//maybe_printf("H5Vi TOUCH (mouse left button): %u, %u\n", keys->cursor_x, keys->cursor_y);
		updateVirtualInputNonAuthoritative(handle, keys, keys->cursor_x, keys->cursor_y);
	}

	if (keys->keys[H5KEY_M2] == 1) {
		//maybe_printf("H5Vi TOUCH (mouse right button): %u, %u\n", keys->cursor_x, keys->cursor_y);
		updateVirtualInputNonAuthoritative(handle, keys, keys->cursor_x, keys->cursor_y);
	}

	if (fingernum > 0) {
		for (unsigned i = 0; i < fingernum; i++) {
			SDL_Finger *finger = SDL_GetTouchFinger(touchDevice, i);
			if (finger != NULL) {
				float relativeX = finger->x;
				if (relativeX < 0) {
					relativeX = 0;
				}
				if (relativeX > 1) {
					relativeX = 1;
				}

				float relativeY = finger->y;
				if (relativeY < 0) {
					relativeY = 0;
				}
				if (relativeY > 1) {
					relativeY = 1;
				}

				unsigned x = ((float)keys->view_width-1)*relativeX;
				unsigned y = ((float)keys->view_height-1)*relativeY;
				maybe_printf("H5Vi TOUCH: %u, %u\n", x, y);
				updateVirtualInputNonAuthoritative(handle, keys, x, y);
			} else {
				maybe_printf("Finger is NULL, what??\n");
			}
		}
	} else {
		maybe_printf("No finger events\n");
	}
}

unsigned H5VI_getInput(H5VI_Reference *handle, H5VI_InputData *keys) {
	SDL_PumpEvents();
	const uint8_t *keyArray = SDL_GetKeyboardState(NULL);
	int tmp_button_mask, tmp_x, tmp_y;
	tmp_button_mask = SDL_GetMouseState(&tmp_x, &tmp_y);

	H5VI_InputData returnval = {0};

	returnval.view_height = keys->view_height;
	returnval.view_width = keys->view_width;

	for (size_t i = 0; i < ELEMNUM(keys->keys); i++){
		returnval.previous_keys[i] = keys->keys[i];
		returnval.fetch_elapsed[i] = keys->fetch_elapsed[i];
	}

	returnval.keys[H5KEY_UP] = keyArray[H5IN_K_UP] /*| H5IN_J_UP*/;
	returnval.keys[H5KEY_DOWN] = keyArray[H5IN_K_DOWN] /*| H5IN_J_DOWN*/;
	returnval.keys[H5KEY_LEFT] = keyArray[H5IN_K_LEFT] /*| H5IN_J_LEFT*/;
	returnval.keys[H5KEY_RIGHT] = keyArray[H5IN_K_RIGHT] /*| H5IN_J_RIGHT*/;
	returnval.keys[H5KEY_B1] = keyArray[H5IN_K_B1] /*| getB(H5IN_J_B1)*/;
	returnval.keys[H5KEY_B2] = keyArray[H5IN_K_B2] /*| getB(H5IN_J_B2)*/;
	returnval.keys[H5KEY_B3] = keyArray[H5IN_K_B3] /*| getB(H5IN_J_B3)*/;
	returnval.keys[H5KEY_B4] = keyArray[H5IN_K_B4] /*| getB(H5IN_J_B4)*/;
	returnval.keys[H5KEY_B5] = keyArray[H5IN_K_B5] /*| getB(H5IN_J_B5)*/;
	returnval.keys[H5KEY_B6] = keyArray[H5IN_K_B6] /*| getB(H5IN_J_B6)*/;
	returnval.keys[H5KEY_B7] = keyArray[H5IN_K_B7] /*| getB(H5IN_J_B7)*/;
	returnval.keys[H5KEY_B8] = keyArray[H5IN_K_B8] /*| getB(H5IN_J_B8)*/;
	returnval.keys[H5KEY_QUIT] = keyArray[H5IN_K_QUIT] /*| getB(H5IN_J_QUIT)*/;
	returnval.keys[H5KEY_PAUSE] = keyArray[H5IN_K_PAUSE] /*| getB(H5IN_J_PAUSE)*/;

	returnval.keys[H5KEY_M1] = tmp_button_mask & SDL_BUTTON(1);
	returnval.keys[H5KEY_M2] = tmp_button_mask & SDL_BUTTON(3);
	returnval.cursor_x = tmp_x;
	returnval.cursor_y = tmp_y;

	/*returnval.axis[0] = getA(H5IN_J_LEFTX);
	returnval.axis[1] = getA(H5IN_J_LEFTY);
	returnval.axis[2] = getA(H5IN_J_TLEFT);
	returnval.axis[3] = getA(H5IN_J_TRIGHT);*/

	for (size_t i = 0; i < ELEMNUM(keys->virKeys); i++){
		returnval.virKeys[i] = keys->virKeys[i];
	}
	H5VI_getVirtualInput(handle, &returnval);

	*keys = returnval;

	H5VI_updateDelayData(handle, keys);

	return 0;
}
#elif defined(H5VI_STDINPUT_IMPL_PORTABLE)
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
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv)
{
	if (argc < 2) {
		maybe_printf("USAGE: pamview file [scale] [tileset file] [tileY,tileX]\n");
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
			maybe_printf("\nROW %zu TYPE: %s\n", i,
				(tiles.tags[i] == tile_Symbol)
					? "SYMBOL"
					: ((tiles.tags[i] == tile_Sprite)
							  ? "TILE_SPRITE"
							  : ((tiles.tags[i] == tile_Image) ? "IMAGE"
															   : "UNKNOWN")));
			for (size_t j = 0; j < tileset_w; j++) {
				maybe_printf("Y: %zu - X: %zu - %s\n", i, j,
					MATRIX_INDEX(tiles.names, tiles.width, j, i));
			}
		}
	}

	char *endptr   = NULL;
	unsigned scale = (argc > 2) ? h5strtoul(argv[2], &endptr, 10) : 1;
	h5uint buf[h * scale][w * scale];
	H5Render_PixelData sprite = {h * scale, w * scale, &buf[0][0]};
	H5Render_scale(smallsprite, sprite, scale, 0);

	H5VI_Reference myref;
	if (H5VI_init(&myref, h * scale, w * scale)) {
		maybe_printf("ERROR: failed to initialize H5VI screen");
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
#include <stdio.h>
#include <time.h>

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
					(VEC2(h5ulong)[]){POINT_UL(50 + i * 4, 50 + i * 2),
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
