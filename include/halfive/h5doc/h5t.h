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

#ifndef H5T_H
#define H5T_H

#include <halfive/h5math.h>
#include <halfive/h5req.h>

enum H5T_TimeEnum { H5T_TimeDay, H5T_TimeNight };
enum H5T_SpeedEnum { H5T_NormalBoost, H5T_Boost, H5T_AntiBoost };
enum H5T_FinishEnum { H5T_NoFinish, H5T_FinishLine, H5T_Checkpoint };
enum H5T_RailEnum { H5T_NoRails, H5T_RailMetal, H5T_RailElectric };
enum H5T_BiomeEnum {
    H5T_BiomeSnow,
    H5T_BiomeLava,
    H5T_BiomeLake,
    H5T_BiomeDesert,
    H5T_BiomeForest,
    H5T_BiomeRosegarden
};
enum H5T_ColourEnum {
    H5T_ColourWhite,
    H5T_ColourRed,
    H5T_ColourBlue,
    H5T_ColourYellow,
    H5T_ColourGreen,
    H5T_ColourPink
};

struct H5T_Biome {
    enum H5T_ColourEnum colour;
    enum H5T_BiomeEnum biome;
};

struct H5T_Segment {
    h5bezier_ulong inner_shape;
    h5bezier_ulong outer_shape;

    enum H5T_SpeedEnum speed;
    enum H5T_FinishEnum racing_line;
    enum H5T_RailEnum guardrails;
};

/*Memory footprint worst-case scenario: 256 bytes*/
typedef struct {
    char name[20];

    struct H5T_Biome biome;
    enum H5T_TimeEnum time;
    h5uchar max_players;

    h5uint segment_number;
    struct H5T_Segment segments[40];
} H5T_Circuit;

extern unsigned H5T_parse(const char *instring, H5T_Circuit *outstruct);
#endif
