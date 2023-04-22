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

/*Halfive Rendering utilities*/

#ifndef H5RENDER_H
#define H5RENDER_H

#include <halfive/h5req.h>
#include <halfive/h5math.h>

/*Pixels are RGBA 16-bit 5551 format*/
#define RGB(r, g, b) \
	((CAP(r, 0x20) << 11) | (CAP(g, 0x20) << 6) | (CAP(b, 0x20) << 1))
#define RGBA(r, g, b, a) \
	((CAP(r, 0x20) << 11) | (CAP(g, 0x20) << 6) | (CAP(b, 0x20) << 1) | !!(a))
#define ALPHA_ON(n)		(n | 0x0001)
#define ALPHA_OFF(n)	(n & (~0x0001))
#define ALPHA_TOGGLE(n) (n ^ 0x0001)

/*Pixel buffer*/
#define MATRIX_GET(var, x, y)	   (var).data[(((y) * (var).width) + (x))]
#define MATRIX_INDEX(var, w, x, y) var[(((y) * (w)) + (x))]

#define POINT_U(x, y) \
	(VEC2(h5uint)) { x, y }
#define POINT_UL(x, y) \
	(VEC2(h5ulong)) { x, y }
#define POINT_UM(x, y) \
	(VEC2(h5umax)) { x, y }
#define POINT_I(x, y) \
	(VEC2(h5sint)) { x, y }
#define POINT_L(x, y) \
	(VEC2(h5slong)) { x, y }
#define POINT_M(x, y) \
	(VEC2(h5smax)) { x, y }

typedef struct {
	size_t height;
	size_t width;
	h5uint *data;
} H5Render_PixelData;

typedef enum {
	tile_None = 0,
	tile_Symbol,
	tile_Sprite,
	tile_Image
} TileCategory;

typedef struct {
	size_t tile_height; /*In pixels*/
	size_t tile_width;	/*In pixels*/
	size_t height;		/*In tiles */
	size_t width;		/*In tiles */
	unsigned padding;	/*How many pixels are used as padding between tiles*/
	H5Render_PixelData buffer;
	TileCategory *tags; /*Array of size [height], indicates                */
						/*the category tag of each row of tiles            */
	char (*names)[32];	/*2D string array of size [height*width],          */
						/*indicates the machine readable name of each tile*/
} H5Render_Tileset;

void H5Render_fill(H5Render_PixelData surf, h5uint colour);

void H5Render_scale(H5Render_PixelData insurf, H5Render_PixelData outsurf,
	unsigned scale_factor, _Bool respect_transparency);

void H5Render_getTile(H5Render_Tileset *tileset, H5Render_PixelData outsurf,
	size_t row, size_t column);

void H5Render_slong_getLinePoints(VEC2(h5slong) p1, VEC2(h5slong) p2,
	h5uint length, VEC2(h5slong) *ret, size_t n);
int H5Render_ulong_getRasterInfo(
	VEC2(h5ulong) p1, VEC2(h5ulong) p2, h5ulong edges[][2], size_t n);
void H5Render_ulong_drawLine(
	H5Render_PixelData surf, VEC2(h5ulong) p1, VEC2(h5ulong) p2, h5uint colour);
void H5Render_ulong_drawPolygon(
	H5Render_PixelData surf, VEC2(h5ulong) *points, size_t n, h5uint colour);

void H5Render_ulong_drawTriangle(H5Render_PixelData surf, VEC2(h5ulong) p1,
	VEC2(h5ulong) p2, VEC2(h5ulong) p3, h5uint colour);
void H5Render_ulong_drawLineSize(H5Render_PixelData surf, VEC2(h5ulong) p1,
	VEC2(h5ulong) p2, h5uint colour, h5uint size);
#endif
