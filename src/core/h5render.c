/*
Copyright Nomagno 2022, 2023

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

#include <halfive/h5math.h>
#include <halfive/h5render.h>
#include <halfive/h5req.h>

/*Pixels are RGBA 16-bit 5551 format*/

void H5Render_fill(H5Render_PixelData surf, h5uint colour)
{
    for (h5ulong x = 0; x < surf.width; x++) {
	for (h5ulong y = 0; y < surf.height; y++) {
	    MATRIX_GET(surf, x, y) = colour; /*WHITE*/
	}
    }
}

void H5Render_scale(H5Render_PixelData insurf, H5Render_PixelData outsurf, unsigned scale_factor){
	for (unsigned long i = 0; i < insurf.height; i++){
	for (unsigned long j = 0; j < insurf.width; j++) {
	for (unsigned long k1 = 0; k1 < scale_factor; k1++) {
	for (unsigned long k2 = 0; k2 < scale_factor; k2++) {
	MATRIX_GET(outsurf, j*scale_factor+k2, i*scale_factor+k1) = MATRIX_GET(insurf, j, i);
	}
	}
	}
	}
}
/*Bresenham's line drawing algorithm*/
void H5Render_ulong_drawLine(H5Render_PixelData surf, h5point_ulong p1,
			     h5point_ulong p2, h5uint colour)
{
    h5slong diffx = (h5_abs(p2.x - p1.x));
    h5slong diffy = -(h5_abs(p2.y - p1.y));

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
int H5Render_ulong_getRasterInfo(h5point_ulong p1, h5point_ulong p2,
				 h5ulong edges[][2], size_t n)
{
    h5slong diffx = (h5_abs(p2.x - p1.x));
    h5slong diffy = -(h5_abs(p2.y - p1.y));

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
void H5Render_slong_getLinePoints(h5point_slong p1, h5point_slong p2,
				  h5uint length, h5point_slong *ret, size_t n)
{
    h5slong diffx = (h5_abs(p2.x - p1.x));
    h5slong diffy = -(h5_abs(p2.y - p1.y));

    h5slong error = diffx + diffy;

    h5slong signx = (p1.x < p2.x) ? 1 : -1;
    h5slong signy = (p1.y < p2.y) ? 1 : -1;

    for (h5uint i = 0; i < length && i < n; i++) {
	ret[i] = (h5point_slong){p1.x, p1.y};
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

void H5Render_ulong_drawPolygon(H5Render_PixelData surf, h5point_ulong *points,
				size_t n, h5uint colour)
{
    h5ulong edges[surf.height][2];
    for (h5ulong i = 0; i < surf.height; i++) {
	edges[i][0] = H5ULONG_MAX - 1;
	edges[i][1] = H5ULONG_MAX;
    }

    for (size_t i = 0; i < (n - 1); i++) {
	H5Render_ulong_drawLine(surf, points[i], points[i + 1], colour);
	H5Render_ulong_getRasterInfo(points[i], points[i + 1], edges,
				     sizeof(edges));
    }
    H5Render_ulong_drawLine(surf, points[0], points[n - 1], colour);
    H5Render_ulong_getRasterInfo(points[0], points[n - 1], edges,
				 sizeof(edges));

    for (h5ulong y = 0; y < surf.height; y++) {
	if (edges[y][0] != (H5ULONG_MAX - 1) && edges[y][1] != (H5ULONG_MAX)) {
	    for (h5ulong x = edges[y][0]; x <= edges[y][1]; x++) {
		MATRIX_GET(surf, x, y) = colour;
	    }
	}
    }
}

void H5Render_ulong_drawTriangle(H5Render_PixelData surf, h5point_ulong p1,
				 h5point_ulong p2, h5point_ulong p3,
				 h5uint colour)
{
    H5Render_ulong_drawPolygon(surf, (h5point_ulong[]){p1, p2, p3}, 3, colour);
}

void H5Render_ulong_drawRectangle(H5Render_PixelData surf, h5point_ulong p1,
				  h5point_ulong p2, h5point_ulong p3,
				  h5point_ulong p4, h5uint colour)
{
    H5Render_ulong_drawPolygon(surf, (h5point_ulong[]){p1, p2, p3, p4}, 4,
			       colour);
}
