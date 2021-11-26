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

#include "halfive.h"

int H5Init(FILE *trck, world *stage) {
	/*
	Initializes the 'stage' world struct

	The 'trck' parameter most be a pointer to a track file, which will then be converted to a proper hwtrack struct

	The 'stage' parameter will then be initialized appropiately, containing appropiate 'simulated properties' appropiate for starting a race in the track.

	Non-zero return value means error
	*/
	stage->track = get_track(trck);
	return 0;
}
int H5Sim(world *stage, unsigned int milli) {
	/*
	Simulates one "tick" for world in the 'stage' parameter, equivalent to the amount of milliseconds specified in 'milli'. 

	Non-zero return value means error
	*/
	return 0;
}
int H5TransformServer(world stage, hwpack_server *serv, enum MODE mode) {
	/*
	Generates a hwpack_server struct at the 'serv' parameter from the 'stage' world state
	Non-zero return value means error
	*/
	strcpy(serv->trackname, stage.track.name);
	strcpy(serv->gamemode, "_Default");
	strcpy(serv->extension, "HalfWrld");
	if (mode == r)
	for (int i = 0; i < RACENUM; i++) {
		serv->player_data[i].position1 = stage.racers[i].pos1;
		serv->player_data[i].position2 = stage.racers[i].pos2;

		serv->player_data[i].flags_1 = stage.racers[i].car_state;
		serv->player_data[i].flags_2 = stage.racers[i].mov_state;
		serv->player_data[i].flags_3 = stage.racers[i].con_state;
	}
	else
		for (int i = 0; i < RACENUM; i++) {
		serv->player_data[i].leaderboard = stage.racers[i].leaderboard1;
		};
	return 0;
}
int H5TransformClient(world *stage, hwpack_client cli){
	/*
	Generates a world struct at the 'stage' parameter from the 'cli' hwpack_client state
	Non-zero return value means error
	*/
	return 0;
}
