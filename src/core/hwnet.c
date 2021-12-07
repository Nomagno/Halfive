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

#include "hwnet.h"

/*
EXAMPLE OF CLIENT PACKET

{"user":{"id":["1","2","3","4"]},"data":{"1":{"thrusters":["123567","1234567","1234567"]},"2":{"thrusters":["123567","1234567","1234567"]},"3":{"thrusters":["123567","1234567","1234567"]},"4":{"thrusters":["123567","1234567","1234567"]},"exten":["HalfWrld"]},"query":{"mode":["r"]}}

EXAMPLE OF SERVER PACKET

{"data":{"1":["1234567","1234567","a","b","c"],"2":["1234567","1234567","a","b","c"],"3":["1234567","1234567","a","b","c"],"4":["1234567","1234567","a","b","c"],"5":["1234567","1234567","a","b","c"],"6":["1234567","1234567","a","b","c"],"7":["1234567","1234567","a","b","c"],"8":["1234567","1234567","a","b","c"],"track":["TRACKADD"],"game":["RACERACE"],"ext":["HelloIts"]}}

	NON-ZERO RETURN VALUE == ERROR
	-1 --- NO MEMORY
	-2 --- INVALID
	-3 --- STRING CORRUPTED
	-4 --- PRIMITIVES (BOOL/INTEGER) NOT ALLOWED BY HWNET
	OTHER --- WTF
*/

int cli_parse(char packet[], hwpack_client *return_pack)
{
	jsmn_parser parser;
	jsmntok_t tokens[47];
	hwpack_client composed_packet;
	hwpack_client error_packet;
	jsmn_init(&parser);
	char jsmn_typestr[47][10];
	strcpy(error_packet.extension, "ERRMODE");
	unsigned int idlist[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	int return_val =
	    jsmn_parse(&parser, packet, strlen(packet), tokens, 47);

	if (return_val < 0) {
		fprintf(stderr, "Invalid JSON %i\n", return_val);
		*return_pack = error_packet;
		return return_val;
	}
	char packetsub[47][400] = {" "};
	int gi;
	for (gi = 0; gi < return_val; gi++) {
		strncpy(packetsub[gi], packet + tokens[gi].start,
			tokens[gi].end - tokens[gi].start);
		switch (tokens[gi].type) {
		case JSMN_UNDEFINED:
			strcpy(jsmn_typestr[gi], "UNDEFINED");
			break;
		case JSMN_OBJECT:
			strcpy(jsmn_typestr[gi], "OBJECT");
			break;
		case JSMN_ARRAY:
			strcpy(jsmn_typestr[gi], "ARRAY");
			break;
		case JSMN_STRING:
			strcpy(jsmn_typestr[gi], "STRING");
			break;
		case JSMN_PRIMITIVE:
			strcpy(jsmn_typestr[gi], "PRIMITIVE");
			break;
		}
		if (tokens[i].type == JSMN_PRIMITIVE) {
			fprintf(stderr,
				"At Token %i [%i,%i]: PRIMITIVE type not "
				"allowed by "
				"HalfWrld-HWPACK. Abort.\n",
				gi + 1, tokens[gi].start, tokens[gi].end);
			*return_pack = error_packet;
			return -4;
		}
		/*
		printf("Token %i START-%i SIZE-%i END-%i TYPE-%s
		VALUE:\n%s\n\n", i + 1, tokens[i].start, tokens[i].size,
		tokens[i].end, jsmn_typestr[i], packetsub[i]);
		*/
	}
	int hi;
	for (hi = 0; hi < return_val; hi++) {
		if (strcmp(packetsub[hi], "user") == 0 &&
		    tokens[hi].type == JSMN_STRING && tokens[hi].size == 1) {

			if (strcmp(packetsub[hi + 2], "id") == 0 &&
			    tokens[hi].type == JSMN_STRING &&
			    tokens[hi + 3].size <= 4) {
				unsigned int d;
				for (d = 0; d <= tokens[hi + 3].size; d++) {
					if (strtol(packetsub[hi + 4 + d], NULL,
						   10) != 0)
						idlist[d] = strtol(
						    packetsub[hi + 4 + d], NULL,
						    10);
					// printf("IDs: %u\n", idlist[d]);
				}
			}
		} else if (strcmp(packetsub[hi], "data") == 0 &&
			   tokens[hi].type == JSMN_STRING) {
			unsigned int f;
			for (f = 0; f < tokens[hi + 1].size; f++) {
				if (strtol(packetsub[hi + 2 + (f * 7)], NULL,
					   10) != 0 &&
				    strtol(packetsub[hi + 2 + (f * 7)], NULL,
					   10) == idlist[f]) {

					composed_packet.player_data[idlist[f]]
					    .thruster_2 =
					    strtol(packetsub[hi + 6 + (f * 7)],
						   NULL, 10);
					composed_packet.player_data[idlist[f]]
					    .thruster_1 =
					    strtol(packetsub[hi + 7 + (f * 7)],
						   NULL, 10);
					composed_packet.player_data[idlist[f]]
					    .steer =
					    strtol(packetsub[hi + 8 + (f * 7)],
						   NULL, 10);

				} else if (strcmp(packetsub[hi + 2 + (f * 7)],
						  "exten") == 0 &&
					   tokens[hi].type == JSMN_STRING) {
					strcpy(composed_packet.extension,
					       packetsub[hi + 4 + (f * 7)]);
				}
			}
		} else if (strcmp(packetsub[hi], "query") == 0 &&
			   tokens[hi].type == JSMN_STRING) {
			if (strcmp(packetsub[hi + 4], "r") == 0) {
				composed_packet.mode = r;
			} else {
				composed_packet.mode = l;
			}
		}
	}

	*return_pack = composed_packet;
	return 0;
}

int serv_parse(char packet[], hwpack_server *return_pack)
{
	jsmn_parser parser;
	jsmntok_t tokens[70];
	hwpack_server composed_packet;
	hwpack_server error_packet;
	jsmn_init(&parser);
	char jsmn_typestr[70][10];
	strcpy(error_packet.extension, "ERRMODE");
	strcpy(error_packet.gamemode, "ERRMODE");
	strcpy(error_packet.trackname, "ERRMODE");
	int return_val =
	    jsmn_parse(&parser, packet, strlen(packet), tokens, 70);
	if (return_val < 0) {
		fprintf(stderr, "Invalid JSON %i\n", return_val);
		*return_pack = error_packet;
		return return_val;
	}
	char packetsub[70][400] = {" "};
	int gi;
	for (gi = 0; gi < return_val; gi++) {
		strncpy(packetsub[gi], packet + tokens[gi].start,
			tokens[gi].end - tokens[gi].start);
		switch (tokens[gi].type) {
		case JSMN_UNDEFINED:
			strcpy(jsmn_typestr[gi], "UNDEFINED");
			break;
		case JSMN_OBJECT:
			strcpy(jsmn_typestr[gi], "OBJECT");
			break;
		case JSMN_ARRAY:
			strcpy(jsmn_typestr[gi], "ARRAY");
			break;
		case JSMN_STRING:
			strcpy(jsmn_typestr[gi], "STRING");
			break;
		case JSMN_PRIMITIVE:
			strcpy(jsmn_typestr[gi], "PRIMITIVE");
			break;
		}
		if (tokens[gi].type == JSMN_PRIMITIVE) {
			fprintf(stderr,
				"At Token %i [%i,%i]: PRIMITIVE type not "
				"allowed by "
				"HalfWrld-HWPACK. Abort.\n",
				gi + 1, tokens[gi].start, tokens[gi].end);
			*return_pack = error_packet;
			return -4;
		}
		/*
		printf("Token %i START-%i SIZE-%i END-%i TYPE-%s
		VALUE:\n%s\n\n", i + 1, tokens[i].start, tokens[i].size,
		tokens[i].end, jsmn_typestr[i], packetsub[i]);
		*/
	}
	int li;
	for (li = 0; li < return_val; li++) {
		if (strcmp(packetsub[li], "data") == 0 &&
		    tokens[li].type == JSMN_STRING && tokens[li].size == 1) {
			unsigned int f;
			for (f = 0; f < tokens[li + 1].size; f++) {
				if (strtol(packetsub[li + 2 + (f * 7)], NULL,
					   10) != 0) {

					composed_packet
					    .player_data[strtol(
						packetsub[li + 2 + (f * 7)],
						NULL, 10)]
					    .position1 =
					    strtol(packetsub[li + 4 + (f * 7)],
						   NULL, 10);
					composed_packet
					    .player_data[strtol(
						packetsub[li + 2 + (f * 7)],
						NULL, 10)]
					    .position2 =
					    strtol(packetsub[li + 5 + (f * 7)],
						   NULL, 10);
					switch (
					    packetsub[li + 6 + (f * 7)][0]) {
					case 'o':
						composed_packet
						    .player_data[strtol(
							packetsub[li + 2 +
								  (f * 7)],
							NULL, 10)]
						    .flags_1 = o;
						break;
					case 'u':
						composed_packet
						    .player_data[strtol(
							packetsub[li + 2 +
								  (f * 7)],
							NULL, 10)]
						    .flags_1 = u;
						break;
					case 'x':
						composed_packet
						    .player_data[strtol(
							packetsub[li + 2 +
								  (f * 7)],
							NULL, 10)]
						    .flags_1 = x;
						break;
					case 's':
						composed_packet
						    .player_data[strtol(
							packetsub[li + 2 +
								  (f * 7)],
							NULL, 10)]
						    .flags_1 = s;
						break;
					case 'z':
						composed_packet
						    .player_data[strtol(
							packetsub[li + 2 +
								  (f * 7)],
							NULL, 10)]
						    .flags_1 = z;
						break;
					case 'e':
						composed_packet
						    .player_data[strtol(
							packetsub[li + 2 +
								  (f * 7)],
							NULL, 10)]
						    .flags_1 = e;
						break;
					case 'c':
						composed_packet
						    .player_data[strtol(
							packetsub[li + 2 +
								  (f * 7)],
							NULL, 10)]
						    .flags_1 = c;
						break;
					case 'y':
						composed_packet
						    .player_data[strtol(
							packetsub[li + 2 +
								  (f * 7)],
							NULL, 10)]
						    .flags_1 = y;
						break;
					}
					switch (
					    packetsub[li + 7 + (f * 7)][0]) {
					case 'f':
						composed_packet
						    .player_data[strtol(
							packetsub[li + 2 +
								  (f * 7)],
							NULL, 10)]
						    .flags_2 = f;
						break;
					case 'b':
						composed_packet
						    .player_data[strtol(
							packetsub[li + 2 +
								  (f * 7)],
							NULL, 10)]
						    .flags_2 = b;
						break;
					case 'k':
						composed_packet
						    .player_data[strtol(
							packetsub[li + 2 +
								  (f * 7)],
							NULL, 10)]
						    .flags_2 = k;
						break;
					case 'i':
						composed_packet
						    .player_data[strtol(
							packetsub[li + 2 +
								  (f * 7)],
							NULL, 10)]
						    .flags_2 = li;
						break;
					}
					switch (
					    packetsub[li + 8 + (f * 7)][0]) {
					case 'a':
						composed_packet
						    .player_data[strtol(
							packetsub[li + 2 +
								  (f * 7)],
							NULL, 10)]
						    .flags_3 = a;
						break;
					case 't':
						composed_packet
						    .player_data[strtol(
							packetsub[li + 2 +
								  (f * 7)],
							NULL, 10)]
						    .flags_3 = t;
						break;
					case 'm':
						composed_packet
						    .player_data[strtol(
							packetsub[li + 2 +
								  (f * 7)],
							NULL, 10)]
						    .flags_3 = m;
						break;
					case 'h':
						composed_packet
						    .player_data[strtol(
							packetsub[li + 2 +
								  (f * 7)],
							NULL, 10)]
						    .flags_3 = h;
						break;
						switch (packetsub[li + 8 +
								  (f * 7)][0]) {
						case '1':
							composed_packet
							    .player_data[strtol(
								packetsub
								    [li + 2 +
								     (f * 7)],
								NULL, 10)]
							    .leaderboard = 1;
							break;
						case '2':
							composed_packet
							    .player_data[strtol(
								packetsub
								    [li + 2 +
								     (f * 7)],
								NULL, 10)]
							    .leaderboard = 2;
							break;
						case '3':
							composed_packet
							    .player_data[strtol(
								packetsub
								    [li + 2 +
								     (f * 7)],
								NULL, 10)]
							    .leaderboard = 3;
							break;
						case '4':
							composed_packet
							    .player_data[strtol(
								packetsub
								    [li + 2 +
								     (f * 7)],
								NULL, 10)]
							    .leaderboard = 4;
							break;
						case '5':
							composed_packet
							    .player_data[strtol(
								packetsub
								    [li + 2 +
								     (f * 7)],
								NULL, 10)]
							    .leaderboard = 5;
							break;
						case '6':
							composed_packet
							    .player_data[strtol(
								packetsub
								    [li + 2 +
								     (f * 7)],
								NULL, 10)]
							    .leaderboard = 6;
							break;
						case '7':
							composed_packet
							    .player_data[strtol(
								packetsub
								    [li + 2 +
								     (f * 7)],
								NULL, 10)]
							    .leaderboard = 7;
							break;
						case '8':
							composed_packet
							    .player_data[strtol(
								packetsub
								    [li + 2 +
								     (f * 7)],
								NULL, 10)]
							    .leaderboard = 8;
							break;
						default:
							break;
						}
					}
				}
			}
		} else if (strcmp(packetsub[li], "track") == 0 &&
			   tokens[li].type == JSMN_STRING &&
			   tokens[li].size == 1) {
			strcpy(composed_packet.trackname, packetsub[li + 2]);
		} else if (strcmp(packetsub[li], "game") == 0 &&
			   tokens[li].type == JSMN_STRING &&
			   tokens[li].size == 1) {
			strcpy(composed_packet.gamemode, packetsub[li + 2]);
		} else if (strcmp(packetsub[li], "ext") == 0 &&
			   tokens[li].type == JSMN_STRING &&
			   tokens[li].size == 1) {
			strcpy(composed_packet.extension, packetsub[li + 2]);
		}
	}

	*return_pack = composed_packet;
	return 0;
}
