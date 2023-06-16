#include <halfive/h5req.h>
#include <halfive/h5render.h>
#include <halfive/h5pix.h>
#include <halfive/h5vi.h>

#include <halfive/h5stdlib.h>
#include <stdio.h>
#include <time.h>
#include <halfive/h5vm/h5vm_gear.h>
#include <halfive/h5vm/h5asm_gear.h>

#define USER_BACKGROUND 0xFFFF

/*COLORS IN RGB888*/ /*COLORS IN RGBA5551*/
#define H5C_C_NONE    0 /*NONE*/      /*0x0000*/
#define H5C_C_RED     1 /*0xFF0033*/  /*0xF80D*/
#define H5C_C_YELLOW  2 /*0xFFCC00*/  /*0xFE01*/
#define H5C_C_GREEN   3 /*0x76EC08*/  /*0x7701*/
#define H5C_C_BLUE    4 /*0x2086FD*/  /*0x1C3D*/
#define H5C_C_MAGENTA 5 /*0xB2194C*/  /*0xA8D3*/
#define H5C_C_BLACK   6 /*0x000000*/  /*0x0001*/
#define H5C_C_GREY    7 /*0x505675*/  /*0x4A9D*/
#define H5C_C_NIGHT   8 /*0x21223D*/  /*0x210F*/
#define H5C_C_BROWN   9 /*0x77001A*/  /*0x7007*/
#define H5C_C_BEIGE   10 /*0xFFFFB4*/ /*0xFFEB*/
#define H5C_C_NAVY    11 /*0xCAFFFF*/ /*0xC7FF*/
#define H5C_C_ORANGE  12 /*0xBD7100*/ /*0xB341*/
#define H5C_C_PINK    13 /*0xFF3366*/ /*0xF999*/
#define H5C_C_INDIGO  14 /*0x8E5EE9*/ /*0x8AF9*/
#define H5C_C_CYAN    15 /*0x33CCFF*/ /*0x363F*/

#define H5C_H_NONE    USER_BACKGROUND /*NONE*/      /*0x0000*/
#define H5C_H_RED     0xF80D /*0xFF0033*/  /*0xF80D*/
#define H5C_H_YELLOW  0xFE01 /*0xFFCC00*/  /*0xFE01*/
#define H5C_H_GREEN   0x7701 /*0x76EC08*/  /*0x7701*/
#define H5C_H_BLUE    0x1C3D /*0x2086FD*/  /*0x1C3D*/
#define H5C_H_MAGENTA 0xA8D3 /*0xB2194C*/  /*0xA8D3*/
#define H5C_H_BLACK   0x0001 /*0x000000*/  /*0x0001*/
#define H5C_H_GREY    0x4A9D /*0x505675*/  /*0x4A9D*/
#define H5C_H_NIGHT   0x210F /*0x21223D*/  /*0x210F*/
#define H5C_H_BROWN   0x7007 /*0x77001A*/  /*0x7007*/
#define H5C_H_BEIGE   0xFFEB /*0xFFFFB4*/ /*0xFFEB*/
#define H5C_H_NAVY    0xC7FF /*0xCAFFFF*/ /*0xC7FF*/
#define H5C_H_ORANGE  0xB341 /*0xBD7100*/ /*0xB341*/
#define H5C_H_PINK    0xF999 /*0xFF3366*/ /*0xF999*/
#define H5C_H_INDIGO  0x8AF9 /*0x8E5EE9*/ /*0x8AF9*/
#define H5C_H_CYAN    0x363F /*0x33CCFF*/ /*0x363F*/

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

/*H5Coordinate will eventually be its own library with boilerplate
  for different types of threaded services. H5Gear has merely been
  reorganized to serve as future reference, hence the out-of-place prefix*/
typedef struct {
	H5VI_Reference *ref;
	const long int frame_length;
	long int delta_time;
	H5VI_InputData input_keys;
	long int *time_taken;
	H5Render_PixelData rendered_output;
	void const *const_userdata;
	void *userdata;
} H5Coordinate_graphicalEventData;

struct H5Gear_EventData {
	H5VM_DefaultMemSetup *mem;
	H5VM_ReadWriteInfo *rwinf;
	H5VM_GeneralMemory *prog;
	_Bool *is_paused;
	_Bool *quit;
};

void H5Gear_printPerformanceData(int frame_length, long int time_taken, _Bool is_paused, _Bool quit) {
	long int performance_percent = 100*(double)((double)frame_length/(double)(time_taken));
	if (time_taken >= frame_length) {
		printf("H5Vi: WARNING: Timing objective not met - Performance: %li%% \n",
		       performance_percent);
	} else {
		printf("H5Vi: Frame time: %li nanoseconds - Performance: %li%%\n",
		       (is_paused) ? 0 : time_taken, (is_paused) ? 100 : performance_percent);
	}
	if (quit) {
		printf("H5Vi: Quitting\n");
	}
}

/*Taking a pointer is done to accomodate emscripten. Screw emscriptem*/
int H5Gear_simulateOneFrame(H5Coordinate_graphicalEventData *opaque_handle) {
	clock_t start_t, end_t;
	start_t = clock(); /*Microseconds in XSI-compliant systems*/

	H5Coordinate_graphicalEventData generalData = *opaque_handle;
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
			printf("Gear: OU: %X\n", specificData.mem->ou);
		}
		if (specificData.prog->hf) {
			printf("\nGear: ---- HALT ----\n"); *specificData.quit = 1;
			goto GO_BACK;
		}
		switch (return_code) {
		case 0: break;
		case 1:
			printf("Gear: NONFATAL ERROR AT PC 0x%X: R/W UNMAPPED MEM\n", specificData.prog->co);
			break;
		case 2:
			printf("Gear: NONFATAL ERROR AT PC 0x%X: WRITE READ-ONLY MEM\n", specificData.prog->co);
			break;
		case 3:
			printf("Gear: NONFATAL ERROR AT PC 0x%X: WRONG ADDRESSING MODE\n", specificData.prog->co);
			break;
		case 4:
			printf("Gear:    FATAL ERROR AT PC 0x%X: CALLSTACK OVERFLOW\n", specificData.prog->co);
			*specificData.quit = 1;
			return return_code;
		default: printf("Gear:    ERROR AT PC 0x%X: UNKNOWN ERROR %u\n",specificData.prog->co,return_code);
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

	H5Coordinate_graphicalEventData loop_data = {
		.ref = &main_ref,
		.frame_length = 33333333,
		.delta_time = 0,
		.time_taken = &(long int){0},
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

	/*This little event loop is where you'd hack away if
	   you were to integrate Sheewol Gear in a cooperative
	   multitasking environment. Screw emscriptem*/
	while (1) {
		_Bool old_is_paused = *(((struct H5Gear_EventData *)(loop_data.userdata))->is_paused);

		H5VI_getInput(&main_ref, &loop_data.input_keys);
		H5Gear_simulateOneFrame((void *)&loop_data);

		_Bool new_is_paused = *(((struct H5Gear_EventData *)(loop_data.userdata))->is_paused);
		_Bool quit = *(((struct H5Gear_EventData *)(loop_data.userdata))->quit);

		H5Gear_printPerformanceData(loop_data.frame_length, *loop_data.time_taken, new_is_paused, quit);

		if (old_is_paused != new_is_paused) { /*If pause state changed, sleep 500ms instead of usual amount*/
			nanosleep(&(struct timespec){0, 500000000 }, NULL);
		} else {
			/*Scale the console's screen to the
			  window size, then  draw to the screen*/
			H5Render_scale(outputbuf, main_buf, SCALE_FACTOR_64, 1);
			H5VI_setBuffer(&main_ref, &main_buf);
			/*WARNING:
			    This performs a conversion from RGBA5551 to the system's
			    pixel representation. It's done pretty efficiently with a 
			    256KBs color lookup table, but it's still the bottleneck
			*/

			long int sleep_time = loop_data.frame_length - *loop_data.time_taken;
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
