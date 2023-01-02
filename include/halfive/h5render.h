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

/*Halfive Rendering utilities*/

#ifndef H5RENDER_H
#define H5RENDER_H

#include <halfive/h5math.h>
#include <halfive/h5req.h>

/*Pixels are RGBA 16-bit 5551 format*/
#define RGB(r, g, b)                                                           \
    ((CAP(r, 0x20) << 11) | (CAP(g, 0x20) << 6) | (CAP(b, 0x20) << 1))
#define RGBA(r, g, b, a)                                                       \
    ((CAP(r, 0x20) << 11) | (CAP(g, 0x20) << 6) | (CAP(b, 0x20) << 1) | !!(a))
#define ALPHA_ON(n) (n | 0x0001)
#define ALPHA_OFF(n) (n & (~0x0001))
#define ALPHA_TOGGLE(n) (n ^ 0x0001)

/*Pixel buffer*/
#define MATRIX_GET(var, x, y) (var).data[((y * (var).width) + x)]

#define POINT_U(x, y)                                                          \
    (h5point_uint)                                                             \
    {                                                                          \
	x, y                                                                   \
    }
#define POINT_UL(x, y)                                                         \
    (h5point_ulong)                                                            \
    {                                                                          \
	x, y                                                                   \
    }
#define POINT_UM(x, y)                                                         \
    (h5point_umax)                                                             \
    {                                                                          \
	x, y                                                                   \
    }
#define POINT_I(x, y)                                                          \
    (h5point_sint)                                                             \
    {                                                                          \
	x, y                                                                   \
    }
#define POINT_L(x, y)                                                          \
    (h5point_slong)                                                            \
    {                                                                          \
	x, y                                                                   \
    }
#define POINT_M(x, y)                                                          \
    (h5point_smax)                                                             \
    {                                                                          \
	x, y                                                                   \
    }

typedef struct {
    size_t height;
    size_t width;
    h5uint *data;
} H5Render_PixelData;
void H5Render_fill(H5Render_PixelData surf, h5uint colour);

void H5Render_slong_getLinePoints(h5point_slong p1, h5point_slong p2,
				  h5uint length, h5point_slong *ret, size_t n);
int H5Render_ulong_getRasterInfo(h5point_ulong p1, h5point_ulong p2,
				 h5ulong edges[][2], size_t n);
void H5Render_ulong_drawLine(H5Render_PixelData surf, h5point_ulong p1,
			     h5point_ulong p2, h5uint colour);
void H5Render_ulong_drawPolygon(H5Render_PixelData surf, h5point_ulong *points,
				size_t n, h5uint colour);

void H5Render_ulong_drawTriangle(H5Render_PixelData surf, h5point_ulong p1,
				 h5point_ulong p2, h5point_ulong p3,
				 h5uint colour);
void H5Render_ulong_drawLineSize(H5Render_PixelData surf, h5point_ulong p1,
				 h5point_ulong p2, h5uint colour, h5uint size);
#endif
