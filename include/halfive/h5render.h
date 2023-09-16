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
	((CAP(r, 0x20) << 11) | (CAP(g, 0x20) << 6) | (CAP(b, 0x20) << 1) | 1)
#define RGBA(r, g, b, a) \
	((CAP(r, 0x20) << 11) | (CAP(g, 0x20) << 6) | (CAP(b, 0x20) << 1) | !!(a))
#define ALPHA_ON(n)		(n | 0x0001)
#define ALPHA_OFF(n)	(n & (~0x0001))
#define ALPHA_TOGGLE(n) (n ^ 0x0001)

#define C_R(v) (v >> 11)
#define C_G(v) ((v << 5) >> 11)
#define C_B(v) ((v << 10) >> 11)
#define C_A(v) (v & 1)

#define MIX(v1, v2, alpha) RGB(\
((C_R(v1) * alpha)/100 + (C_R(v2 * (100-alpha)))/100),\
((C_G(v1) * alpha)/100 + (C_G(v2 * (100-alpha)))/100),\
((C_B(v1) * alpha)/100 + (C_B(v2 * (100-alpha))/100))\
)

/*Pixel buffer*/
#define MATRIX_GET(var, x, y)	   ((var).data[(((y) * (var).width) + (x))])
#define MATRIX_INDEX(var, w, x, y) (var[(((y) * (w)) + (x))])

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

typedef struct {
	H5Render_Tileset *tileset;
	H5Render_PixelData tmp1;
	H5Render_PixelData tmp2;
} H5Render_GlobalTileSetData;

extern H5Render_GlobalTileSetData global_font;
extern unsigned global_padding;

#define INIT_FONT_MAIN(v, scaling)\
	size_t tileset_height_pix, tileset_width_pix;\
	H5Pix_getPAM_Size(PREPEND_PATH v ".pam", &tileset_height_pix, &tileset_width_pix);\
	h5uint font_buf[tileset_height_pix][tileset_width_pix];\
	H5Render_PixelData font_sprite = {tileset_height_pix, tileset_width_pix, &font_buf[0][0]};\
	H5Render_fill(font_sprite, 0x0000);\
	H5Pix_getPAM_Contents(PREPEND_PATH v ".pam", font_sprite);\
	size_t tileset_h, tileset_w;\
	H5Pix_getINFO_TilesetSize(PREPEND_PATH v ".info", &tileset_h, &tileset_w);\
	char font_names[tileset_h][tileset_w][32];\
	h5memset(font_names, 0, tileset_h*tileset_w*32);\
	\
	TileCategory font_tags[tileset_h];\
	H5Render_Tileset font_tileset = {\
		.tile_height = 0,\
		.tile_width						   = 0,\
		.height							   = tileset_h,\
		.width							   = tileset_w,\
		.padding						   = 0,\
		.buffer							   = font_sprite,\
		.tags							   = &font_tags[0],\
		.names							   = &font_names[0][0]\
	};\
	H5Pix_getINFO_TilesetContents(PREPEND_PATH v ".info", &font_tileset);\
	\
	size_t font_tmp_h = font_tileset.tile_height, font_tmp_w = font_tileset.tile_width;\
	h5uint font_tmpbuf1[font_tmp_h][font_tmp_w];\
	H5Render_PixelData font_tmpsprite1 = {.height = font_tmp_h, .width = font_tmp_w, &font_tmpbuf1[0][0]};\
	H5Render_fill(font_tmpsprite1, 0x0000);\
	\
	h5uint font_tmpbuf2[font_tmp_h*scaling][font_tmp_w*scaling];\
	H5Render_PixelData font_tmpsprite2 = {.height = font_tmp_h*scaling, .width = font_tmp_w*scaling, &font_tmpbuf2[0][0]};\
	H5Render_fill(font_tmpsprite2, 0x0000);\
	\
	global_font.tileset = &font_tileset;\
	global_font.tmp1 = font_tmpsprite1;\
	global_font.tmp2 = font_tmpsprite2;
	

#define TEXT_SIZE(number_of_chars, ren__scale) (global_font.tileset->tile_width*number_of_chars*ren__scale+global_padding*number_of_chars)

#define SAFE_REDUCE_SIZE(ren__scale) (2*global_font->tile_width*ren__scale+2*global_padding)


#define RENDER_TEXT(ren__string, pos__x, pos__y, ren__scale, outsurf)\
	H5Render_renderText(ren__string, global_font.tileset, global_font.tmp1, global_font.tmp2,\
	outsurf, pos__x, pos__y, global_padding, ren__scale);



void H5Render_fill(H5Render_PixelData surf, h5uint colour);
void H5Render_scale(H5Render_PixelData insurf, H5Render_PixelData outsurf,
	unsigned scale_factor, _Bool respect_transparency);

char H5Render_mapSymbolToChar(char *instring);
VEC2(h5ulong) H5Render_getStartingPositionOfTile(H5Render_Tileset *tileset, unsigned x, unsigned y);
void H5Render_getTileByPosition(H5Render_Tileset *tileset, H5Render_PixelData outsurf,
	_Bool respect_transparency, size_t row, size_t column);
char *H5Render_getTileNameFromPosition(H5Render_Tileset *tileset, size_t row, size_t column);
VEC2(h5ulong) H5Render_getTilePositionFromName(H5Render_Tileset *tileset, char *name);
void H5Render_getTileByName(H5Render_Tileset *tileset, H5Render_PixelData outsurf,
	_Bool respect_transparency, char *name);
void H5Render_renderText(char *string, H5Render_Tileset *tileset, H5Render_PixelData tmpsurf1, H5Render_PixelData tmpsurf2, H5Render_PixelData outsurf, unsigned x, unsigned y, unsigned padding, unsigned scale);
void H5Render_mapCharToSymbol(char inchar, char *outarray, size_t n);

void H5Render_blitSprite(H5Render_PixelData insurf, H5Render_PixelData outsurf, unsigned x, unsigned y, _Bool respect_transparency);
void H5Render_blitSpriteWithTransparency(H5Render_PixelData insurf, H5Render_PixelData outsurf, unsigned x, unsigned y, unsigned transparency /*out of 100*/);


void H5Render_slong_getLinePoints(VEC2(h5slong) p1, VEC2(h5slong) p2,
	h5uint length, VEC2(h5slong) *ret, size_t n);
int H5Render_ulong_getRasterInfo(
	VEC2(h5ulong) p1, VEC2(h5ulong) p2, h5ulong edges[][2], size_t n);
void H5Render_ulong_drawLine(
	H5Render_PixelData surf, VEC2(h5ulong) p1, VEC2(h5ulong) p2, h5uint colour);
void H5Render_ulong_drawPolygon(
	H5Render_PixelData surf, VEC2(h5ulong) *points, size_t n, h5uint colour);
void H5Render_ulong_drawPolygonOutline(
	H5Render_PixelData surf, VEC2(h5ulong) *points, size_t n, h5uint colour);

void H5Render_ulong_drawTriangle(H5Render_PixelData surf, VEC2(h5ulong) p1,
	VEC2(h5ulong) p2, VEC2(h5ulong) p3, h5uint colour);
void H5Render_ulong_drawRectangle(H5Render_PixelData surf, VEC2(h5ulong) p1,
	VEC2(h5ulong) p2, VEC2(h5ulong) p3, VEC2(h5ulong) p4, h5uint colour);
void H5Render_ulong_drawLineSize(H5Render_PixelData surf, VEC2(h5ulong) p1,
	VEC2(h5ulong) p2, h5uint colour, h5uint size);
#endif
