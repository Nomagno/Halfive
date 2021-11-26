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

#include "../../external/json_parser/jsmn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXCHAR_HWNET 9
#define MAXPLAYERS_HWNET 32

enum TYPE1 { o, u, x, s, z, e, c, y };
enum TYPE2 { f, b, k, i };
enum TYPE3 { a, m, t, h };
enum MODE { r, l };
struct player_data_server {
	enum TYPE1 flags_1; /*If it is -1, SHOULD BE LEFT EMPTY*/
	enum TYPE2 flags_2; /*If it is -1, SHOULD BE LEFT EMPTY*/
	enum TYPE3 flags_3; /*If it is -1, SHOULD BE LEFT EMPTY*/
	int leaderboard; /*If it is -1, SHOULD NOT BE SEND*/

	int position1;
	int position2;
};

struct player_data_client {
	int thruster_1;
	int thruster_2;
	int steer;
};

typedef struct {
	char trackname[MAXCHAR_HWNET];
	char gamemode[MAXCHAR_HWNET];
	char extension[MAXCHAR_HWNET];

	struct player_data_server player_data[MAXPLAYERS_HWNET];
} hwpack_server;

typedef struct {
	char extension[MAXCHAR_HWNET];
	enum MODE mode;
	struct player_data_client player_data[MAXPLAYERS_HWNET];
} hwpack_client;

extern int cli_parse(char[], hwpack_client*);
extern int serv_parse(char[], hwpack_server*);


