/*
Copyright 2023

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

#include <halfive/h5render.h>
#include <halfive/h5pix.h>

#include <stdio.h>

unsigned digitCount(int n) {
	unsigned count = 0;
	while(n != 0){
		count++;
		n = n/10;
	}
	return count;
}

int H5Pix_getPAM_Size(char *filename, unsigned *height, unsigned *width){
	FILE* file = fopen(filename, "rb");
	unsigned h=0, w=0, depth=0, maxval=0;
	if (fscanf(file, "P7\nWIDTH %u\nHEIGHT %u\nDEPTH %u\nMAXVAL %u\nTULTYPE RGB_ALPHA\nENDHDR\n",
	       &w, &h, &depth, &maxval) != 4) { fclose(file); return 1; }; /*Not all elements were read*/ 
	fclose(file);
	*height = h;
	*width = w;
	if (h == 0 || w == 0 || depth != 4 || maxval != 255) { return 1; } /*format not supported*/

	return 0;
}

int H5Pix_getPAM_Contents(char *filename, H5Render_PixelData buf){
	FILE* file = fopen(filename, "rb");
	unsigned h=0, w=0, depth=0, maxval=0;
	if (fscanf(file, "P7\nWIDTH %u\nHEIGHT %u\nDEPTH %u\nMAXVAL %u\nTULTYPE RGB_ALPHA\nENDHDR\n",
	       &w, &h, &depth, &maxval) != 4) { fclose(file); return 1; }; /*Not all elements were read*/ 
	if (h == 0 || w == 0 || depth != 4 || maxval != 255) { fclose(file); return 1; } /*format not supported*/
	h5uchar data[h*w*4];
	for (h5ulong i = 0; i < (h*w*4); i++) {
	data[i] = 0;
	}
	fseek(file, 63 + digitCount(h) + digitCount(w), SEEK_SET); /*63 is a precalculated constant from the header string size*/
	fread(data, 1, h*w*4, file);
	fclose(file);

	printf("%X\n", data[0]);
	for (h5ulong i = 0; i < h; i++) {
	for (h5ulong j = 0; j < w; j++) {
		buf.data[i*h+j] = ((data[((i*h+j)*4)] >> 3) << 11) | (((data[((i*h+j)*4)+1] >> 3)) << 6) |
		              ((data[((i*h+j)*4)+2] >> 3) << 1) | (!!(data[((i*h+j)*4)+3]));
	}
	}
	return 0;
}
