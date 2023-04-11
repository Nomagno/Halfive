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

/*THIS IS HEAVILY OUTDATED*/
/*Halfive Network Protocol utilities*/

#ifndef H5NET_H
#define H5NET_H

#include <halfive/h5req.h>
#include <halfive/h5math.h>
#define MAXCHAR_H5NET 9

enum H5NET_Type1Enum { o, u, x, s, z, e, c, y };
enum H5NET_Type2Enum { f, b, k, i };
enum H5NET_Type3Enum { a, m, t, h };
enum H5NET_ModeEnum { r, l };
struct H5NET_ServerData {
	enum H5NET_Type1Enum flags_1; /*If it is -1, SHOULD BE LEFT EMPTY*/
	enum H5NET_Type2Enum flags_2; /*If it is -1, SHOULD BE LEFT EMPTY*/
	enum H5NET_Type3Enum flags_3; /*If it is -1, SHOULD BE LEFT EMPTY*/
	VEC2(h5ulong) position;
	h5uint leaderboard;
};

struct H5NET_ClientData {
	h5uchar thruster_1, thruster_2, buttons;
};

/*Memory footprint worst-case scenario: 256Bs*/
typedef struct {
	char trackname[MAXCHAR_H5NET], gamemode[MAXCHAR_H5NET],
		extension[MAXCHAR_H5NET];

	struct H5NET_ServerData data[RACENUM];
} H5NET_ServerPacket;

/*Memory footprint worst-case scenario: 128Bs*/
typedef struct {
	char extension[MAXCHAR_H5NET];
	enum H5NET_ModeEnum mode;
	struct H5NET_ClientData data[RACENUM];
} H5NET_ClientPacket;

extern unsigned H5NET_ClientParse(
	const char *input, H5NET_ClientPacket *output);
extern unsigned H5NET_ServerParse(const char *, H5NET_ServerPacket *output);
#endif
