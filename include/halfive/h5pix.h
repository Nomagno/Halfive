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

/*Halfive Pixel utilities interface*/

#ifndef H5PIX_H
#define H5PIX_H

#include <halfive/h5req.h>
#include <halfive/h5render.h>

/*
Import from .pam files following the format:
--------
P7
HEIGHT h
WIDTH w
DEPTH 255
MAXVAL 255
TUPLTYPE RGB_ALPHA
ENDHDR
--------
Followed by a single image's raw data (substitute h and w with the height and
width in ASCII decimal)
*/
int H5Pix_getPAM_Size(const char *filename, size_t *height, size_t *width);
int H5Pix_getPAM_Contents(const char *filename, H5Render_PixelData buf);

int H5Pix_getINFO_TilesetSize(
	const char *filename, size_t *height, size_t *width);
int H5Pix_getINFO_TilesetContents(
	const char *filename, H5Render_Tileset *out_tileset);
#endif
