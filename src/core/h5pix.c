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
#include <halfive/h5pix.h>

unsigned digitCount(int n)
{
	unsigned count = 0;
	while (n != 0) {
		count++;
		n = n / 10;
	}
	return count;
}

#include <stdio.h>
int H5Pix_getPAM_Size(const char *filename, size_t *height, size_t *width)
{
	FILE *file = fopen(filename, "rb");
	size_t h = 0, w = 0;
	unsigned depth = 0, maxval = 0;
	if (fscanf(file,
			"P7\n"
			"WIDTH %zu\n"
			"HEIGHT %zu\n"
			"DEPTH %u\n"
			"MAXVAL %u\n"
			"TULTYPE RGB_ALPHA\n"
			"ENDHDR\n",
			&w, &h, &depth, &maxval) != 4) {
		fclose(file);
		return 1; /*Not all elements were read*/
	};
	fclose(file);
	*height = h;
	*width	= w;
	if (h == 0 || w == 0 || depth != 4 || maxval != 255) {
		return 1;
	} /*format not supported*/

	return 0;
}

int H5Pix_getPAM_Contents(const char *filename, H5Render_PixelData buf)
{
	FILE *file = fopen(filename, "rb");
	size_t h = 0, w = 0;
	unsigned depth = 0, maxval = 0;
	if (fscanf(file,
			"P7\n"
			"WIDTH %zu\n"
			"HEIGHT %zu\n"
			"DEPTH %u\n"
			"MAXVAL %u\n"
			"TULTYPE RGB_ALPHA\n"
			"ENDHDR\n",
			&w, &h, &depth, &maxval) != 4) {
		fclose(file);
		return 1; /*Not all elements were read*/
	};
	if (h == 0 || w == 0 || depth != 4 || maxval != 255) {
		fclose(file);
		return 1;
	} /*format not supported*/
	h5uchar data[h * w * 4];
	for (h5ulong i = 0; i < (h * w * 4); i++) {
		data[i] = 0;
	}
	fseek(file, 63 + digitCount(h) + digitCount(w),
		SEEK_SET); /*63 is a precalculated constant from the header string
					  size*/
	fread(data, 1, h * w * 4, file);
	fclose(file);

	for (h5ulong i = 0; i < h; i++) {
		for (h5ulong j = 0; j < w; j++) {
			if (MATRIX_INDEX(data, w, j * 4 + 3, i * 4) != 0) { /*only change pixel if
                                                             its opaque*/
				MATRIX_GET(buf, j, i) =
					((MATRIX_INDEX(data, w, j * 4, i * 4) >> 3) << 11) |
					((MATRIX_INDEX(data, w, j * 4 + 1, i * 4) >> 3) << 6) |
					((MATRIX_INDEX(data, w, j * 4 + 2, i * 4) >> 3) << 1) |
					1;
			} else { /*Can add a debug taint color for transparent pixels here*/
				MATRIX_GET(buf, j, i) = 0x0000;
			}
		}
	}
	return 0;
}

int H5Pix_getINFO_TilesetSize(
	const char *filename, size_t *height, size_t *width)
{
	FILE *file = fopen(filename, "rb");
	size_t tile_height, tile_width;
	unsigned padding;
	if (fscanf(file,
			"MULTIPLETILES\n"
			"TILE_HEIGHT %zu\n"
			"TILE_WIDTH %zu\n"
			"HEIGHT %zu\n"
			"WIDTH %zu\n"
			"PADDING %u\n",
			&(tile_height), &(tile_width), height, width, &(padding)) != 5) {
		fclose(file);
		return 1; /*Wrong number of elements read*/
	} else {
		fclose(file);
		return 0; /*all OK*/
	}
}

int H5Pix_getINFO_TilesetContents(
	const char *filename, H5Render_Tileset *out_tileset)
{
	FILE *file = fopen(filename, "rb");
	if (fscanf(file,
			"MULTIPLETILES\n"
			"TILE_HEIGHT %zu\n"
			"TILE_WIDTH %zu\n"
			"HEIGHT %zu\n"
			"WIDTH %zu\n"
			"PADDING %u\n",
			&(out_tileset->tile_height), &(out_tileset->tile_width),
			&(out_tileset->height), &(out_tileset->width),
			&(out_tileset->padding)) != 5) {
		fclose(file);
		return 1; /*Wrong number of elements read*/
	}
	fseek(file,
		63 + digitCount(out_tileset->tile_height) +
			digitCount(out_tileset->tile_width) +
			digitCount(out_tileset->height) + digitCount(out_tileset->width) +
			digitCount(out_tileset->padding),
		SEEK_SET); /*63 is a precalculated constant from the
					 number of characters in the initial section*/
	size_t w	 = out_tileset->width;
	char(*n)[32] = out_tileset->names;
	for (size_t i = 0; i < out_tileset->height; i++) {
		char linearr[512] = {0};
		char *line		  = &linearr[0];
		fgets(linearr, sizeof(linearr), file);
		for (size_t j = 0; j < out_tileset->width + 1; j++) {
			unsigned currarraypos = 0;
			if (j == 0) { /*If this is the first word of the line, compare it to
							 a tag*/
				if (linearr[0] == 'S' &&
					linearr[1] ==
						'Y') { /*I know this is hacky, but no more is needed*/
					out_tileset->tags[i] = tile_Symbol;
				} else if (linearr[0] == 'S' && linearr[1] == 'P') {
					out_tileset->tags[i] = tile_Symbol;
				} else if (linearr[0] == 'I' && linearr[1] == 'M') {
					out_tileset->tags[i] = tile_Image;
				}
				while (*line != ' ' && *line != '\n' && *line != '\0') {
					line++;
				}
				line++;
			} else { /*Else save it as a name*/
				while (*line != ' ' && *line != '\n' && *line != '\0') {
					MATRIX_INDEX(n, w, j - 1, i)[currarraypos] = *line;
					currarraypos++;
					line++;
				}
				MATRIX_INDEX(n, w, j - 1, i)[currarraypos] = '\0';
				line++;
			}
		}
	}
	fclose(file);
	return 0;
}
