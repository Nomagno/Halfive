/*
Copyright Nomagno 2022, 2023

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
#include <halfive/h5math.h>
#include <halfive/h5render.h>
H5Render_GlobalTileSetData global_font;
unsigned global_padding;

/*Pixels are RGBA 16-bit 5551 format*/


#include <halfive/h5stdlib.h>
#undef C_A
#define C_A
void H5Render_blitSprite(H5Render_PixelData insurf, H5Render_PixelData outsurf, unsigned x, unsigned y, _Bool respect_transparency) {
	for (size_t i = 0; i < insurf.height; i++) {
		for (size_t j = 0; j < insurf.width; j++) {
			_Bool tr = (respect_transparency && (C_A(MATRIX_GET(insurf, j, i)) == 0));
			MATRIX_GET(outsurf, j+x, i+y) = tr ? MATRIX_GET(outsurf, j+x, i+y) : MATRIX_GET(insurf, j, i);
		}
	}
}

void H5Render_blitSpriteWithTransparency(H5Render_PixelData insurf, H5Render_PixelData outsurf, unsigned x, unsigned y, unsigned transparency /*out of 100*/) {
		for (size_t i = 0; i < insurf.height; i++) {
			for (size_t j = 0; j < insurf.width; j++) {
				MATRIX_GET(outsurf, j+x, i+y) = MIX(MATRIX_GET(insurf, j+x, i+y), MATRIX_GET(outsurf, j, i), transparency);
			}
		}
}

/*These two memset/memcpy calls MUST be provided by the host platform, they are one of the biggest data throughtput bottleneck calls in the entire engine and are absolutely performance critical*/
#include <string.h>
void H5Render_fill(H5Render_PixelData surf, h5uint colour)
{
	/*memset's dramatically faster, useful for 0x0000 and 0xFFFF*/
	if ((colour >> 8) == (colour & 0xFF)) {
		memset(surf.data, (h5uchar)colour, surf.height*surf.width*sizeof(h5uint));
	} else {
		for (h5ulong i = 0; i < surf.width*surf.height; i++) {
			surf.data[i] = colour;
		}
	}
}

typedef uintmax_t render_t;
void H5Render_scale(H5Render_PixelData insurf, H5Render_PixelData outsurf,
	unsigned scale_factor, _Bool respect_transparency)
{
	if (scale_factor == 1) {
		for (render_t i = 0; i < insurf.height; i++) {
			for (render_t j = 0; j < insurf.width; j++) {
				_Bool tr = (respect_transparency && (MATRIX_GET(insurf, j, i) == 0));
				MATRIX_GET(outsurf, j, i) = tr ? MATRIX_GET(outsurf, j, i) : MATRIX_GET(insurf, j, i);
			}
		}
	} else {
		for (render_t i = 0; i < insurf.height; i++) {
			const render_t y = i * scale_factor;
			const render_t width = outsurf.width;
			const render_t flat_index_y = width*y;
			_Bool line_was_transparent = 1;
			for (render_t j = 0; j < insurf.width; j++) {
				const render_t x = j * scale_factor;
				for (render_t k1 = 0; k1 < scale_factor; k1++) {
					_Bool tr = (respect_transparency && (MATRIX_GET(insurf, j, i) == 0));
					line_was_transparent &= tr ? 1 : 0;
					MATRIX_GET(outsurf, x+k1, y) = tr ? MATRIX_GET(outsurf, x+k1, y) : MATRIX_GET(insurf, j, i);
					MATRIX_GET(outsurf, x+k1, y) = MATRIX_GET(insurf, j, i);
				}
			}
			if (!line_was_transparent) {
				for (render_t k2 = 1; k2 < scale_factor; k2++) {
					memcpy(&outsurf.data[width*(y+k2)], &outsurf.data[flat_index_y], width*sizeof(h5uint));
				}
			} else {
				/*printf("Wow! a totally transparent line!\n");*/
			}
		}
	}
}

/*h5render_spam.c contains the definition of H5Render_mapCharToSymbol.
It makes use of a 64-case-long switch statement.*/
#include "h5render_spam.c"
VEC2(h5ulong) H5Render_getStartingPositionOfTile(H5Render_Tileset *tileset, unsigned x, unsigned y) {
	return (VEC2(h5ulong)) {
		.x = tileset->tile_width*x + tileset->padding*x,
		.y = tileset->tile_height*y + tileset->padding*y
	};
}
void H5Render_getTileByPosition(H5Render_Tileset *tileset, H5Render_PixelData outsurf,
	_Bool respect_transparency, size_t column, size_t row) {
	VEC2(h5ulong) pos = H5Render_getStartingPositionOfTile(tileset, column, row);
	unsigned x = pos.x;
	unsigned y = pos.y;
	for (size_t i = 0; i < tileset->tile_height; i++) {
		for (size_t j = 0; j < tileset->tile_width; j++) {
			_Bool tr = (respect_transparency && (C_A(MATRIX_GET(tileset->buffer, j, i)) == 0));
			MATRIX_GET(outsurf, j, i) = tr ? MATRIX_GET(outsurf, j, i) : MATRIX_GET(tileset->buffer, j+x, i+y);
		}
	}
}
char *H5Render_getTileNameFromPosition(H5Render_Tileset *tileset, size_t column, size_t row) {
	return (MATRIX_INDEX(tileset->names, tileset->width, column, row));
}
VEC2(h5ulong) H5Render_getTilePositionFromName(H5Render_Tileset *tileset, char *name) {
	for (size_t i = 0; i < tileset->height; i++) {
		for (size_t j = 0; j < tileset->width; j++) {
			char *tmpname = H5Render_getTileNameFromPosition(tileset, j, i);
			if (h5strcmp(tmpname, name) == 0) {
				return (VEC2(h5ulong)){ .x = j, .y = i };
			}
		}
	}
	return (VEC2(h5ulong)){ 0, 0 };
}
void H5Render_getTileByName(H5Render_Tileset *tileset, H5Render_PixelData outsurf,
	_Bool respect_transparency, char *name) {
	VEC2(h5ulong) pos = H5Render_getTilePositionFromName(tileset, name);
	H5Render_getTileByPosition(tileset, outsurf, respect_transparency, pos.x, pos.y);
}

void H5Render_renderText(char *string, H5Render_Tileset *tileset, H5Render_PixelData tmpsurf1, H5Render_PixelData tmpsurf2, H5Render_PixelData outsurf, unsigned x, unsigned y, unsigned padding, unsigned scale_factor) {
	int i = 0;
	for (char *c = string; *c != '\0'; c++) {
		char converted_name[32] = {0};
		H5Render_mapCharToSymbol(*c, converted_name, sizeof(converted_name));
		H5Render_fill(tmpsurf1, 0xFFFF);
		H5Render_fill(tmpsurf2, 0xFFFF);
		H5Render_getTileByName(tileset, tmpsurf1, 0, converted_name);
		H5Render_scale(tmpsurf1, tmpsurf2, scale_factor, 0);
		
		h5ulong calculated_x = x + tileset->tile_width*scale_factor*i + padding*i;
		H5Render_blitSprite(tmpsurf2, outsurf, calculated_x, y, 0);
		i++;
	}
}

/*Bresenham's line drawing algorithm*/
void H5Render_ulong_drawLine(
	H5Render_PixelData surf, VEC2(h5ulong) p1, VEC2(h5ulong) p2, h5uint colour)
{
	h5slong diffx = (H5_ABS(p2.x - p1.x));
	h5slong diffy = -(H5_ABS(p2.y - p1.y));

	h5slong error = diffx + diffy;

	h5slong signx = (p1.x < p2.x) ? 1 : -1;
	h5slong signy = (p1.y < p2.y) ? 1 : -1;

	while (1) {
		MATRIX_GET(surf, p1.x, p1.y) = colour;
		if ((p1.x == p2.x) && (p1.y == p2.y))
			break;
		h5slong e2 = 2 * error;
		if (e2 >= diffy) {
			if (p1.x == p2.x)
				break;
			error += diffy;
			p1.x += signx;
		}
		if (e2 <= diffx) {
			if (p1.y == p2.y)
				break;
			error += diffx;
			p1.y += signy;
		}
	}
}
int H5Render_ulong_getRasterInfo(
	VEC2(h5ulong) p1, VEC2(h5ulong) p2, h5ulong edges[][2], size_t n)
{
	h5slong diffx = (H5_ABS(p2.x - p1.x));
	h5slong diffy = -(H5_ABS(p2.y - p1.y));

	h5slong error = diffx + diffy;

	h5slong signx = (p1.x < p2.x) ? 1 : -1;
	h5slong signy = (p1.y < p2.y) ? 1 : -1;

	size_t i;
	for (i = 0; i < n; i++) {
		if (p1.x < edges[p1.y][0])
			edges[p1.y][0] = p1.x;
		if ((edges[p1.y][1] == H5ULONG_MAX) || p1.x > edges[p1.y][1])
			edges[p1.y][1] = p1.x;
		if ((p1.x == p2.x) && (p1.y == p2.y))
			break;
		h5slong e2 = 2 * error;
		if (e2 >= diffy) {
			if (p1.x == p2.x)
				break;
			error += diffy;
			p1.x += signx;
		}
		if (e2 <= diffx) {
			if (p1.y == p2.y)
				break;
			error += diffx;
			p1.y += signy;
		}
	}
	return (i >= n) ? 1 : 0;
}
void H5Render_slong_getLinePoints(VEC2(h5slong) p1, VEC2(h5slong) p2,
	h5uint length, VEC2(h5slong) *ret, size_t n)
{
	h5slong diffx = (H5_ABS(p2.x - p1.x));
	h5slong diffy = -(H5_ABS(p2.y - p1.y));

	h5slong error = diffx + diffy;

	h5slong signx = (p1.x < p2.x) ? 1 : -1;
	h5slong signy = (p1.y < p2.y) ? 1 : -1;

	for (h5uint i = 0; i < length && i < n; i++) {
		ret[i] = (VEC2(h5slong)){p1.x, p1.y};
		if ((p1.x == p2.x) && (p1.y == p2.y))
			break;
		h5slong e2 = 2 * error;
		if (e2 >= diffy) {
			if (p1.x == p2.x)
				break;
			error += diffy;
			p1.x += signx;
		}
		if (e2 <= diffx) {
			if (p1.y == p2.y)
				break;
			error += diffx;
			p1.y += signy;
		}
	}
}

void H5Render_ulong_drawPolygon(
	H5Render_PixelData surf, VEC2(h5ulong) *points, size_t n, h5uint colour)
{
	h5ulong edges[surf.height][2];
	for (h5ulong i = 0; i < surf.height; i++) {
		edges[i][0] = H5ULONG_MAX - 1;
		edges[i][1] = H5ULONG_MAX;
	}

	for (size_t i = 0; i < (n - 1); i++) {
		H5Render_ulong_drawLine(surf, points[i], points[i + 1], colour);
		H5Render_ulong_getRasterInfo(
			points[i], points[i + 1], edges, sizeof(edges));
	}
	H5Render_ulong_drawLine(surf, points[0], points[n - 1], colour);
	H5Render_ulong_getRasterInfo(
		points[0], points[n - 1], edges, sizeof(edges));

	for (h5ulong y = 0; y < surf.height; y++) {
		if (edges[y][0] != (H5ULONG_MAX - 1) && edges[y][1] != (H5ULONG_MAX)) {
			for (h5ulong x = edges[y][0]; x <= edges[y][1]; x++) {
				MATRIX_GET(surf, x, y) = colour;
			}
		}
	}
}

void H5Render_ulong_drawPolygonOutline(
	H5Render_PixelData surf, VEC2(h5ulong) *points, size_t n, h5uint colour)
{
	for (size_t i = 0; i < n-1; i++) {
		H5Render_ulong_drawLine(surf, points[i], points[i+1], colour);
	}
	H5Render_ulong_drawLine(surf, points[n-1], points[0], colour);	
}

void H5Render_ulong_drawTriangle(H5Render_PixelData surf, VEC2(h5ulong) p1,
	VEC2(h5ulong) p2, VEC2(h5ulong) p3, h5uint colour)
{
	H5Render_ulong_drawPolygon(surf, (VEC2(h5ulong)[]){p1, p2, p3}, 3, colour);
}

void H5Render_ulong_drawRectangle(H5Render_PixelData surf, VEC2(h5ulong) p1,
	VEC2(h5ulong) p2, VEC2(h5ulong) p3, VEC2(h5ulong) p4, h5uint colour)
{
	H5Render_ulong_drawPolygon(
		surf, (VEC2(h5ulong)[]){p1, p2, p3, p4}, 4, colour);
}
