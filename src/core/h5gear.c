/*
Copyright Nomagno 2023

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
#include <halfive/h5coord.h>

#include <halfive/h5vm/h5vm_gear.h>
#include <halfive/h5vm/h5asm_gear.h>
#include <halfive/h5gear.h>

#include <halfive/h5vi.h>

h5uint H5Gear_getColor(h5uint in) {
	switch (in & 0xF){
	case H5C_C_NONE: return H5C_H_NONE;
	case H5C_C_RED: return H5C_H_RED;
	case H5C_C_YELLOW: return H5C_H_YELLOW;
	case H5C_C_GREEN: return H5C_H_GREEN;
	case H5C_C_BLUE: return H5C_H_BLUE;
	case H5C_C_MAGENTA: return H5C_H_MAGENTA;
	case H5C_C_BLACK: return H5C_H_BLACK;
	case H5C_C_GREY: return H5C_H_GREY;
	case H5C_C_NIGHT: return H5C_H_NIGHT;
	case H5C_C_BROWN: return H5C_H_BROWN;
	case H5C_C_BEIGE: return H5C_H_BEIGE;
	case H5C_C_NAVY: return H5C_H_NAVY;
	case H5C_C_ORANGE: return H5C_H_ORANGE;
	case H5C_C_PINK: return H5C_H_PINK;
	case H5C_C_INDIGO: return H5C_H_INDIGO;
	case H5C_C_CYAN: return H5C_H_CYAN;
	default: return 0xFFFF;
	}
}

/*For legacy packed pixel layout*/
h5uint H5Gear_extractPixel(h5uint pixel, unsigned index) {
	return (pixel >> (16-((index+1)*4))) & 0xF;
}

/*For legacy packed pixel layout*/
int H5Gear_toPixelData(h5uint native[restrict 1024], H5Render_PixelData *retval){
	if ((retval->height != 64) || (retval->width != 64)) {
		return 1;
	}
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			retval->data[i*64+j] = H5Gear_getColor(H5Gear_extractPixel(native[i*16+(j/4)], j%4));
		}
	}
}

/*For current unpacked pixel layout*/
int H5Gear_toPixelData_UNPACKED(h5uint native[restrict 4096], H5Render_PixelData *retval){
	if ((retval->height != 64) || (retval->width != 64)) {
		return 1;
	}
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			retval->data[i*64+j] = H5Gear_getColor(native[i*64+j]);
		}
	}
}


#define WCONSTANT 1024
#define HCONSTANT 1024
#define SCALE_FACTOR_64 16 /*Factor needed to scale a 64x64 buffer to the screen*/

#include <halfive/code_setup.h>
#include <halfive/h5stdlib.h>
#include <time.h>
#include <stdio.h>

void H5Gear_printPerformanceData(h5umax time_available, h5umax time_taken, _Bool is_paused, _Bool quit) {
	h5umax performance_percent = 100*(double)((double)time_available/(double)(time_taken));
	if (time_taken >= time_available) {
		maybe_printf("H5Vi: WARNING: Timing objective not met - Performance: %li%%\n",
		       performance_percent);
	} else {
		maybe_printf("H5Vi: Frame time: %li nanoseconds - Performance: %li%%\n",
		       (is_paused) ? 0 : time_taken, (is_paused) ? 100 : performance_percent);
	}
	if (quit) {
		maybe_printf("H5Vi: Quitting\n");
	}
}

/*Taking a pointer is done to accomodate emscripten. Screw emscriptem*/
int H5Gear_simulateOneFrame(H5Coordinate_GraphicalEventData *opaque_handle) {
	clock_t start_t, end_t;
	start_t = clock(); /*Microseconds in XSI-compliant systems*/

	H5Coordinate_GraphicalEventData generalData = *opaque_handle;
	struct H5Gear_EventData specificData = *(struct H5Gear_EventData *)(generalData.userdata);
	H5VI_InputData input_keys = generalData.input_keys;
	int return_code;

	/*Bitmask: Up Down Left Right Button4 Button3 Button2 Button1*/
	h5uchar button_mask = input_keys.keys[H5KEY_UP] << 7    |
	                      input_keys.keys[H5KEY_DOWN] << 6  |
	                      input_keys.keys[H5KEY_LEFT] << 5  |
	                      input_keys.keys[H5KEY_RIGHT] << 4 |
	                      input_keys.keys[H5KEY_B4] << 3    |
	                      input_keys.keys[H5KEY_B3] << 2    |
	                      input_keys.keys[H5KEY_B2] << 1    |
	                      input_keys.keys[H5KEY_B1];
	specificData.mem->in = button_mask; /*Set input register to the button bitmask*/
	if (input_keys.keys[H5KEY_QUIT] == 1) {
		*specificData.quit = 1;
		goto GO_BACK;
	}
	if (input_keys.keys[H5KEY_PAUSE] == 1 && *specificData.is_paused == 0) {
		*specificData.is_paused = 1;
		goto GO_BACK;
	} else if (input_keys.keys[H5KEY_PAUSE] == 1 && *specificData.is_paused == 1) {
		*specificData.is_paused = 0;
		goto GO_BACK;
	}

	if (*specificData.is_paused == 1) {
		goto GO_BACK;
	}

	/*This while loop executes the console's code
	until the finish register is changed to
	nonzero. This is taken to mean that its
	done rendering the current frame.*/
	while (specificData.mem->ff == 0) {
		return_code = H5VM_execute(specificData.prog, specificData.rwinf);
		if ((specificData.rwinf->adrw == 0xFFFC) && (specificData.rwinf->wrote_adrw)) {
			maybe_printf("Gear: OU: %X\n", specificData.mem->ou);
		}
		if (specificData.prog->hf) {
			maybe_printf("\nGear: ---- HALT ----\n"); *specificData.quit = 1;
			goto GO_BACK;
		}
		switch (return_code) {
		case 0: break;
		case 1:
			maybe_printf("Gear: NONFATAL ERROR AT PC 0x%X: R/W UNMAPPED MEM\n", specificData.prog->co);
			break;
		case 2:
			maybe_printf("Gear: NONFATAL ERROR AT PC 0x%X: WRITE READ-ONLY MEM\n", specificData.prog->co);
			break;
		case 3:
			maybe_printf("Gear: NONFATAL ERROR AT PC 0x%X: WRONG ADDRESSING MODE\n", specificData.prog->co);
			break;
		case 4:
			maybe_printf("Gear:    FATAL ERROR AT PC 0x%X: CALLSTACK OVERFLOW\n", specificData.prog->co);
			*specificData.quit = 1;
			return return_code;
		default: maybe_printf("Gear:    ERROR AT PC 0x%X: UNKNOWN ERROR %u\n",specificData.prog->co,return_code);
			*specificData.quit = 1;
			return return_code;
		}
	}
	specificData.mem->ff = 0; /*Reset the finish register*/

	/*Convert the console's internal screen representation into the internal one*/
	H5Gear_toPixelData_UNPACKED(&specificData.mem->graphical_output[0], &generalData.rendered_output);
	goto GO_BACK;

GO_BACK:
	end_t = clock(); /*Microseconds in XSI-compliant systems*/
	*generalData.time_taken = ((end_t-start_t)*1000);
	return 0;
}	

static h5uint array_outputbuf[64][64] = {0};
static h5uint array_main_buf[HCONSTANT][WCONSTANT] = {0};
static H5VM_DefaultMemSetup mem = {0};
static H5VM_ReadWriteInfo rwinf = {0};
static H5VM_CodeMemory code = {(H5VM_InstructionSet)0};
static H5VM_GeneralMemory prog = {0};

int main(int argc, char **argv) {
	/*WARNING: PROGRAM DOESN'T WORK CORRECTLY IF 'CLOCKS_PER_SEC' IS NOT XSI-MANDATED 1000000*/
	FILE *codefile;
	FILE *dromfile;
	FILE *gromfile;
	if (argc == 1) { codefile = stdin; }
	if (argc >= 2 && h5streq(argv[1], "-")) { codefile = stdin; }
		else if (argc >= 2) { codefile = fopen(argv[1], "r"); }
	if (argc >= 3) { dromfile = fopen(argv[2], "r"); }
		else { dromfile = fopen("/dev/null", "r"); }
	if (argc >= 4) { gromfile = fopen(argv[3], "r"); }
		else { gromfile = fopen("/dev/null", "r"); }
	char arr[30];
	int i = 0;
	while (fscanf(codefile, "%[^\n] ", arr) != EOF) {
		H5ASM_parse(arr, &code.inst[i], code.opnd[i]); i++;
	}
	prog = H5VM_init(&code, &mem);
	fread(mem.rom, 1, sizeof(mem.rom), dromfile);
	fread(mem.grom, 1, sizeof(mem.grom), gromfile);

	H5VI_Reference main_ref;

	H5Render_PixelData outputbuf = {64, 64, .data = &array_outputbuf[0][0]};
	H5Render_PixelData main_buf = {HCONSTANT, WCONSTANT, .data = &array_main_buf[0][0]};
	H5Render_fill(outputbuf, 0xFFFF);
	H5Render_fill(main_buf, 0xFFFF);

	if (H5VI_init(&main_ref, HCONSTANT, WCONSTANT)) {
		H5VI_destroy(&main_ref);
		goto EXIT;
	}
	H5VI_setBuffer(&main_ref, &main_buf);

	H5Coordinate_GraphicalEventData loop_data = {
		.ref = &main_ref,
		.time_available = 33333333, /*average frame length in nanoseconds --- FPS = (1/time_available_in_seconds)*/
		.delta_time = 0, /*To-do: update this per-run*/
		.time_taken = &(h5umax){0},
		.rendered_output = outputbuf,
		.const_userdata = NULL,
		.input_keys = {0},
		.userdata = &(struct H5Gear_EventData){
			.mem = &mem,
			.rwinf = &rwinf,
			.prog = &prog,
			.is_paused = &(_Bool){0},
			.quit = &(_Bool){0}
		}
	};

	/*NOTE: this loop is a basic reference for how to implement H5Coord's main-loop
	  scheduler that makes use of an event queue and can adapt to variable performance*/
	/*This little event loop is where you'd hack away if
	   you were to integrate Sheewol Gear in a cooperative
	   multitasking environment. Screw emscriptem*/
	while (1) {
		_Bool old_is_paused = *(((struct H5Gear_EventData *)(loop_data.userdata))->is_paused);

		H5VI_getInput(&main_ref, &loop_data.input_keys);
		H5Gear_simulateOneFrame((void *)&loop_data);

		_Bool new_is_paused = *(((struct H5Gear_EventData *)(loop_data.userdata))->is_paused);
		_Bool quit = *(((struct H5Gear_EventData *)(loop_data.userdata))->quit);

		H5Gear_printPerformanceData(loop_data.time_available, *loop_data.time_taken, new_is_paused, quit);

		if (old_is_paused != new_is_paused) { /*If pause state changed, sleep 500ms instead of usual amount*/
			nanosleep(&(struct timespec){0, 500000000 }, NULL);
		} else {
			/*Scale the console's screen to the
			  window size, then  draw to the screen*/
			H5Render_scale(outputbuf, main_buf, SCALE_FACTOR_64, 0);
			H5VI_setBuffer(&main_ref, &main_buf);
			/*WARNING:
			    This performs a conversion from RGBA5551 to the system's
			    pixel representation. It's done pretty efficiently with a 
			    256KBs color lookup table, but it's still the bottleneck
			*/

			h5smax sleep_time = loop_data.time_available - *loop_data.time_taken;

			/*How long do we sleep? Primitive calculation for now, better
			  not have a potato computer or it'll just drop the framerate and complain*/
			nanosleep(&(struct timespec){0, (sleep_time > 0) ? sleep_time : 1 }, NULL);
		}

		if (quit) {
			goto EXIT;
		}
	}

	EXIT:
		fclose(codefile);
		fclose(dromfile);
		fclose(gromfile);
		H5VI_destroy(&main_ref);
}
