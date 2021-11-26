
/*
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
OF, OR IN CONNECTION WITH THE WORK OR THE USE OF OR OTHER DEALINGS IN THE WORK.*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../external/toml_parser/toml.h"


/* To calculate point [t] of the Bezier curve:
 A*(1-t)^2 + B*2t(1-t) + C*t^2
*/


enum TIME { day, night };
enum SPEED { normal, boost, antiboost };
enum RACING_LINE { no, finish_line, checkpoint };
enum GUARDRAILS { empty, metal, electric };
enum BIOME { forest, desert, snow };
enum COLORS { white, red, blue, yellow, green, pink };

struct _biome {
  enum COLORS main_color;
  enum BIOME the_biome;
};

struct point {
  int x;
  int y;
};

struct bezier {
  struct point p1;
  struct point p2;
  struct point p3;
};

struct segment {
  struct bezier shape;
	unsigned int width;

  enum SPEED speed;
  enum RACING_LINE racing_line;
  enum GUARDRAILS guardrails;
};

typedef struct {
  char name[20];

  struct _biome biome;
  enum TIME time;
  unsigned int max_players;

	unsigned int segment_number;
  struct segment segments[40];
} hwtrack;

extern hwtrack get_track(FILE *);

