#include <halfive/h5req.h>
#include <halfive/h5render.h>
#include <halfive/h5pix.h>
#include <halfive/h5vi.h>

#include <halfive/h5stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <halfive/h5vm/h5vm_gear.h>
#include <halfive/h5vm/h5asm_gear.h>

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

#define H5C_H_NONE    0x0000 /*NONE*/      /*0x0000*/
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

typedef struct {
	h5uchar val: 4;
} H5Gear_Nibble;

h5uint H5Gear_getColor(H5Gear_Nibble in) {
	switch (in.val){
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

H5Gear_Nibble H5Gear_extractPixel(h5uint pixel, unsigned index) {
//	printf("Extracted color: %i\n", pixel >> (16-((index+1)*4)) & 0xF);
	return (H5Gear_Nibble){ .val = (pixel >> (16-((index+1)*4))) & 0xF };
}

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

#define WCONSTANT 1024
#define HCONSTANT 1024
#define SCALE_FACTOR_64 16 /*Factor needed to scale a 64x64 buffer to the screen*/

struct main_loop_data {
	const int frame_length;
	H5VI_Reference *ref;
	H5Render_PixelData console_buffer;	
	H5Render_PixelData screen_buffer;
	H5VM_DefaultMemSetup *mem;
	H5VM_ReadWriteInfo *rwinf;
	H5VM_GeneralMemory *prog;
	_Bool *is_paused;
	int *frame_count_for_pause;
	long int *sleep_time;
	_Bool *quit;
};

int _main_loop(void *opaque_handle) {
	struct main_loop_data data = *((struct main_loop_data *)opaque_handle);
	int return_code;
	H5VI_InputData myinput = {0};
	clock_t start_t, end_t;
	start_t = clock(); /*Microseconds in XSI-compliant systems*/

	H5VI_getInput(data.ref, &myinput);
	/*Bitmask: UDLR4321*/
	h5uchar button_mask = myinput.keys[H5KEY_UP] << 7    |
	                      myinput.keys[H5KEY_DOWN] << 6  |
	                      myinput.keys[H5KEY_LEFT] << 5  |
	                      myinput.keys[H5KEY_RIGHT] << 4 |
	                      myinput.keys[H5KEY_B4] << 3    |
	                      myinput.keys[H5KEY_B3] << 2    |
	                      myinput.keys[H5KEY_B2] << 1    |
	                      myinput.keys[H5KEY_B1];
	data.mem->in = button_mask; /*Set input register to the button bitmask*/
	if (myinput.keys[H5KEY_QUIT] == 1) {
		printf("H5Vi: Quit key detected, exiting gracefully...\n");
		*data.quit = 1; return 1;
	}
	if (myinput.keys[H5KEY_PAUSE] == 1 && *data.is_paused == 0) {
		printf("H5Vi: PAUSED, you can either quit or unpause now\n");
		*data.is_paused = 1; goto GO_BACK;
	} else if (myinput.keys[H5KEY_PAUSE] == 1 && *data.is_paused == 1) {
		printf("H5Vi: UNPAUSE\n");
		*(data.frame_count_for_pause) = 0;
		*data.is_paused = 0; goto GO_BACK;
	}

	if (*(data.is_paused) == 1) {
		printf("H5Vi: PAUSED, you can either quit or unpause now\n");
		goto GO_BACK;
	}

	while (data.mem->output[1024] == 0) /*"finish register", when !0 end frame*/ {		
	return_code = H5VM_execute(data.prog, data.rwinf);
	if ((data.rwinf->adrw == 0xFFFC) && (data.rwinf->wrote_adrw)) { printf("Gear: OU: %X\n", data.mem->ou); }
	if (data.prog->hf) { printf("\nGear: ---- HALT ----\n"); *data.quit = 1; return 0; }
	switch (return_code) {
		case 0: break;
		case 1: printf("Gear: NONFATAL ERROR AT PC 0x%X: R/W UNMAPPED MEM\n", data.prog->co);
			break;
		case 2: printf("Gear: NONFATAL ERROR AT PC 0x%X: WRITE READ-ONLY MEM\n", data.prog->co);
			break;
		case 3: printf("Gear: NONFATAL ERROR AT PC 0x%X: WRONG ADDRESSING MODE\n", data.prog->co);
			break;
		case 4: printf("Gear:    FATAL ERROR AT PC 0x%X: CALLSTACK OVERFLOW\n", data.prog->co);
			*data.quit = 1; return return_code;
		default: printf("Gear:    ERROR AT PC 0x%X: UNKNOWN ERROR %u\n",data.prog->co,return_code);
			*data.quit = 1; return return_code;
	}
	}
	data.mem->output[1024] = 0;

	H5Gear_toPixelData(&data.mem->output[0], &data.console_buffer);

	H5Render_fill(data.screen_buffer, 0xFFFF);
	H5Render_scale(data.console_buffer, data.screen_buffer, SCALE_FACTOR_64, 1);
	H5VI_setBuffer(data.ref, &data.screen_buffer);

GO_BACK:
	end_t = clock(); /*Microseconds in XSI-compliant systems*/
	time_t diff = ((end_t-start_t)*100); /*multiply to convert to nanoseconds*/
	if (diff >= data.frame_length) {
		printf("H5Vi: WARNING: Timing objective not met - Performance: %li%% \n", (long int)(100*(double)((double)data.frame_length/(double)(diff))));
		*data.sleep_time = 1;
	} else {
		printf("H5Vi: Frame time: %li nanoseconds - Performance: %li%%\n", (*(data.is_paused)) ? 0 : diff, (*(data.is_paused)) ? 100 : ((long int)(100*(double)((double)data.frame_length/(double)(diff)))));
		*data.sleep_time = data.frame_length - diff;
	}
}	

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
	H5VM_CodeMemory code = {(H5VM_InstructionSet)0};
	int i = 0;
	while (fscanf(codefile, "%[^\n] ", arr) != EOF) {
		H5ASM_parse(arr, &code.inst[i], code.opnd[i]); i++;
	}
	H5VM_DefaultMemSetup mem = {0};
	H5VM_ReadWriteInfo rwinf = {0};
	H5VM_GeneralMemory prog	 = H5VM_init(&code, &mem);
	fread(mem.drom, 1, sizeof(mem.drom), dromfile);
	fread(mem.grom, 1, sizeof(mem.grom), gromfile);

	H5VI_Reference main_ref;
	h5uint array_main_buf[HCONSTANT][WCONSTANT] = {0};
	h5uint array_screenbuf[64][64] = {0};
	for (h5ulong y = 0; y < HCONSTANT; y++) {
		for (h5ulong x = 0; x < WCONSTANT; x++) {
			array_main_buf[y][x] = 0xFFFF; /*WHITE*/
		}
	}
	for (h5ulong y = 0; y < 64; y++) {
		for (h5ulong x = 0; x < 64; x++) {
			array_screenbuf[y][x] = 0xFFFF; /*WHITE*/
		}
	}
	H5Render_PixelData screenbuf = {64, 64, .data = &array_screenbuf[0][0]};
	H5Render_PixelData main_buf = {HCONSTANT, WCONSTANT, .data = &array_main_buf[0][0]};
	if (H5VI_init(&main_ref, HCONSTANT, WCONSTANT)) {
		H5VI_destroy(&main_ref);
		goto EXIT;
	}
	H5VI_setBuffer(&main_ref, &main_buf);

	struct main_loop_data loop_data = {
		.frame_length = 33333333,
		.ref = &main_ref,
		.console_buffer = screenbuf,
		.screen_buffer = main_buf,
		.mem = &mem,
		.rwinf = &rwinf,
		.prog = &prog,
		.is_paused = &(_Bool){0},
		.frame_count_for_pause = &(int){0},
		.sleep_time = &(long int){0},
		.quit = &(_Bool){0}
	};

	/*This little event loop is where you'd hack away if
	   you were to integrate Sheewol Gear in a cooperative
	   multitasking environment*/
	while (1) {
		_Bool old_is_paused = *(loop_data.is_paused);
		_main_loop((void *)&loop_data);
		if (old_is_paused != *(loop_data.is_paused)) { /*If pause state changed, sleep 500ms instead of usual amount*/
			nanosleep(&(struct timespec){0, 500000000 }, NULL);
		} else {
			nanosleep(&(struct timespec){0, *loop_data.sleep_time }, NULL);
		}
		if (*(loop_data.quit)) {
			goto EXIT;
		}
	}
	

	EXIT:
		fclose(codefile);
		fclose(dromfile);
		fclose(gromfile);
		H5VI_destroy(&main_ref);
}
