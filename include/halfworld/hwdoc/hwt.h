/*
Copyright Nomagno 2021, 2022

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

/* To calculate point [t] of the Bezier curve:
 A(1-t)(1-t) + B(1-t)(2t) + C(t)(t)
*/

#ifndef HWT_H
#define HWT_H

#include <halfworld/hwreq.h>
#include <halfworld/hwmath.h>

enum HWT_TimeEnum { day, night };
enum HWT_SpeedEnum { normal, boost, antiboost };
enum HWT_FinishEnum { no, finish_line, checkpoint };
enum HWT_RailEnum { empty, metal, electric };
enum HWT_BiomeEnum { forest, desert, snow };
enum HWT_ColorEnum { white, red, blue, yellow, green, pink };

struct HWT_Biome {
	enum HWT_ColorEnum main_color;
	enum HWT_BiomeEnum the_biome;
};

struct HWT_Segment {
	hwbezier inner_shape;
	hwbezier outer_shape;

	enum HWT_SpeedEnum speed;
	enum HWT_FinishEnum racing_line;
	enum HWT_RailEnum guardrails;
};

/*Memory footprint worst-case scenario: 256 bytes*/
typedef struct {
	char name[20];

	struct HWT_Biome biome;
	enum HWT_TimeEnum time;
	hwuchar max_players;

	hwuint segment_number;
	struct HWT_Segment segments[40];
} HWT_Circuit;

extern unsigned HWT_Parse(const char *instring, HWT_Circuit *outstruct);
#endif
