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

/*Halfive Track format utilities*/

#ifndef H5T_H
#define H5T_H

#include <halfive/h5req.h>
#include <halfive/h5math.h>

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
