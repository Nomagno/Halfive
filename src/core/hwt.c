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
OF, OR IN CONNECTION WITH THE WORK OR THE USE OF OR OTHER DEALINGS IN THE
WORK.*/

#include "hwt.h"
#define MAX_SEGMENTS 40

char section[10][20], param[10][20], value[10][20], segment_param[10][20];
char buffer[80][100];
char toml_errbuf[100];

hwtrack get_track(FILE *track_file)
{
	//  FILE *track_file = fopen(track_path, "r");
	if (track_file == NULL) {
		perror("Failed to open track file");
	}
	hwtrack track;

	/*FILL ERROR STRUCT*/
	hwtrack error;
	strcpy(error.name, "ERROR");
	error.biome.the_biome = forest;
	error.biome.main_color = green;
	error.time = day;
	error.max_players = 1;
	error.segment_number = 0;

	int segment_length = 0, finish_line = 0;

	toml_table_t *track_toml =
	    toml_parse_file(track_file, toml_errbuf, 100);
	if (!track_toml) {
		fprintf(stderr, "Cannot parse: %s\n", toml_errbuf);
		return error;
	}
	toml_table_t *track_table = toml_table_in(track_toml, "track");
	if (!track_table) {
		perror("Please include a [track] table\n");
		return error;
	}
	toml_array_t *time_array = toml_array_in(track_table, "time");
	if (!time_array) {
		printf("Consider including track.time\n");
	}

	toml_array_t *biome_array = toml_array_in(track_table, "biome");
	if (!biome_array) {
		printf("Consider including track.biome\n");
	}
	toml_array_t *player_array = toml_array_in(track_table, "max_players");
	if (!player_array) {
		printf("Consider including track.max_players\n");
	}
	toml_array_t *name_array = toml_array_in(track_table, "name");
	if (!name_array) {
		perror("track.name not found - ABORT\n");
		return error;
	}
	int max_players;
	toml_datum_t time_toml = toml_string_at(time_array, 0);
	toml_datum_t biome_toml = toml_string_at(time_array, 0);
	toml_datum_t player_toml = toml_string_at(player_array, 0);
	toml_datum_t name_toml = toml_string_at(name_array, 0);

	if (strlen(time_toml.u.s) <= 9) {
		strcpy(track.name, time_toml.u.s);
	} else {
		perror("track.name length exceeds 8 characters - ABORT\n");
		return error;
	}

	if (strcmp(time_toml.u.s, "night") == 0) {
		track.time = night;
	} else if (strcmp(time_toml.u.s, "day") == 0) {
		track.time = day;
	}

	if (strcmp(biome_toml.u.s, "snow") == 0) {
		track.biome.the_biome = snow;
		track.biome.main_color = white;
	} else if (strcmp(biome_toml.u.s, "forest") == 0) {
		track.biome.the_biome = forest;
		track.biome.main_color = green;
	} else if (strcmp(biome_toml.u.s, "desert") == 0) {
		track.biome.the_biome = desert;
		track.biome.main_color = yellow;
	}

	max_players = atoi(player_toml.u.s);
	track.max_players = max_players;

	toml_table_t *segments_table = toml_table_in(track_toml, "segments");
	if (!segments_table) {
		perror("Please include a [segments] table\n");
		return error;
	}

	toml_table_t *segments_subtable[MAX_SEGMENTS];
	toml_array_t *shapes[MAX_SEGMENTS];
	toml_array_t *widths[MAX_SEGMENTS];
	toml_array_t *local_params[MAX_SEGMENTS];
	toml_datum_t local_params_values[MAX_SEGMENTS];
	toml_datum_t points[MAX_SEGMENTS][6];
	toml_datum_t width_values[MAX_SEGMENTS];
	int a_point;
	int i;
	for (i = 0; i < MAX_SEGMENTS; i++) {
		char num[4] = {0};
		snprintf(num, 3, "%d", i + 1);

		segments_subtable[i] = toml_table_in(segments_table, num);
		if (!segments_subtable[i]) {
			segment_length = i;
			break;
		}
		widths[i] = toml_array_in(segments_subtable[i], "width");
		if (!widths[i]) {
			fprintf(stderr, "At segment %d\n", i + 1);
			perror("Width parameter not found in segment. Abort\n");
			return error;
		} else {
			width_values[i] = toml_string_at(widths[i], 0);
			track.segments[i].width = atol(width_values[i].u.s);
		}
		shapes[i] = toml_array_in(segments_subtable[i], "shape");
		local_params[i] =
		    toml_array_in(segments_subtable[i], "local_param");
		if (local_params[i]) {
			local_params_values[i] =
			    toml_string_at(local_params[i], 0);
		}
		if ((local_params[i]) &&
		    (strcmp(local_params_values[i].u.s, "finish_line") == 0)) {
			finish_line++;
		}
		int e;
		for (e = 0; e < 6; e++) {
			points[i][e] = toml_string_at(shapes[i], e);
			if (!points[i][e].ok) {
				fprintf(stderr, "At segment %d, point %d",
					i + 1, e + 1);
				perror(
				    "Segment %d, point %d: Not Found. Abort\n");
				return error;
			}
			a_point = atol(points[i][e].u.s);
			if (e == 0) {
				track.segments[i].shape.p1.x = a_point;
			}
			if (e == 1) {
				track.segments[i].shape.p1.y = a_point;
			}
			if (e == 2) {
				track.segments[i].shape.p2.x = a_point;
			}
			if (e == 3) {
				track.segments[i].shape.p2.y = a_point;
			}
			if (e == 4) {
				track.segments[i].shape.p3.x = a_point;
			}
			if (e == 5) {
				track.segments[i].shape.p3.y = a_point;
			}
		}

		track.segments[i].speed = normal;
		if ((local_params[i]) &&
		    (strcmp(local_params_values[i].u.s, "antiboost") == 0)) {
			track.segments[i].speed = antiboost;
		} else if ((local_params[i]) &&
			   (strcmp(local_params_values[i].u.s, "boost") == 0)) {
			track.segments[i].speed = boost;
		}

		track.segments[i].racing_line = no;
		if ((local_params[i]) &&
		    (strcmp(local_params_values[i].u.s, "finish_line") == 0)) {
			track.segments[i].racing_line = finish_line;
		} else if ((local_params[i]) &&
			   (strcmp(local_params_values[i].u.s, "checkpoint") ==
			    0)) {
			track.segments[i].racing_line = checkpoint;
		}

		track.segments[i].guardrails = empty;
		if ((local_params[i]) &&
		    (strcmp(local_params_values[i].u.s, "guardrails") == 0)) {
			track.segments[i].guardrails = metal;
		} else if ((local_params[i]) &&
			   (strcmp(local_params_values[i].u.s, "checkpoint") ==
			    0)) {
			track.segments[i].guardrails = electric;
		}
	}
	if (finish_line == 0) {
		fprintf(stderr, "No finish_line detected in the local "
				"parameters. Please review "
				"them.\n");
		return error;
	}
	track.segment_number = segment_length;
	printf("Parsing OK.\n");
	printf("There are %d segments.\n", segment_length);

	toml_free(track_toml);
	return track;
}

/*
int main() {
	FILE *track = fopen("../../assets/tracks/triangular_parade.hwt", "r");
	get_track(track);
}
*/
