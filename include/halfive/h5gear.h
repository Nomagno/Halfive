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

#ifndef H5GEAR_H
#define H5GEAR_H

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

/*Convert from Sheewol Gear's internal pixel representation to RGBA5551*/
h5uint H5Gear_getColor(h5uint in);

/*Pixels in the legacy packed pixel layout are
  packed so that one 16-bit machine word contains
  four 4-bit pixels*/
/*For legacy packed pixel layout*/
h5uint H5Gear_extractPixel(h5uint pixel, unsigned index);

/*Dump native 64x64 (4096/4 = 1024 words) screen to a 64x64 RGBA5551 buffer*/
/*For legacy packed pixel layout*/
int H5Gear_toPixelData(h5uint native[restrict 1024], H5Render_PixelData *retval);

/*Dump native 64x64 (4096 words) screen to a 64x64 RGBA5551 buffer*/
/*For current unpacked pixel layout*/
int H5Gear_toPixelData_UNPACKED(h5uint native[restrict 4096], H5Render_PixelData *retval);

/*userdata struct of H5Coordinate_GraphicalEventData*/
struct H5Gear_EventData {
	H5VM_DefaultMemSetup *mem;
	H5VM_ReadWriteInfo *rwinf;
	H5VM_GeneralMemory *prog;
	_Bool *is_paused;
	_Bool *quit;
};

int H5Gear_simulateOneFrame(H5Coordinate_GraphicalEventData *opaque_handle);

#endif
