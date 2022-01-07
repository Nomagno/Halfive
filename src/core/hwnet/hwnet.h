/*
Copyright Nomagno 2021

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

#include "../hwreq.h"
#define MAXCHAR_HWNET 9
#define MAXPLAYERS_HWNET 32

enum HWNET_Type1Enum { o, u, x, s, z, e, c, y };
enum HWNET_Type2Enum { f, b, k, i };
enum HWNET_Type3Enum { a, m, t, h };
enum HWNET_ModeEnum { r, l };
struct HWNET_ServerData {
	enum HWNET_Type1Enum flags_1; /*If it is -1, SHOULD BE LEFT EMPTY*/
	enum HWNET_Type2Enum flags_2; /*If it is -1, SHOULD BE LEFT EMPTY*/
	enum HWNET_Type3Enum flags_3; /*If it is -1, SHOULD BE LEFT EMPTY*/
	int leaderboard;    /*If it is -1, SHOULD NOT BE SEND*/

	int position1;
	int position2;
};

struct HWNET_ClientData {
	int thruster_1;
	int thruster_2;
	int steer;
};

typedef struct {
	char trackname[MAXCHAR_HWNET];
	char gamemode[MAXCHAR_HWNET];
	char extension[MAXCHAR_HWNET];

	struct HWNET_ServerData data[MAXPLAYERS_HWNET];
} HWNET_ServerPacket;

typedef struct {
	char extension[MAXCHAR_HWNET];
	enum HWNET_ModeEnum mode;
	struct HWNET_ClientData data[MAXPLAYERS_HWNET];
} HWNET_ClientPacket;

extern int HWNET_ClientParse(const char *input, HWNET_ClientPacket *output);
extern int HWNET_ServerParse(const char *, HWNET_ServerPacket *output);
