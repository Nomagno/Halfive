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

#include <halfworld/hwreq.h>
#include <halfworld/hwdoc/hwdoc.h>
#include <halfworld/hwdoc/hwnet.h>
#include <halfworld/hwdoc/hwt.h>
#include <halfworld/hwstring.h>
#include <halfworld/hwvm/hwvm.h>

#define RACENUM 32

typedef struct {

	/*Intrinsic properties of the vehicle
	  (BACKEND WILL NOT MODIFY THESE, FRONTEND CAN MODIFY THEM)*/

	uint16_t width;          /*MILLIMETERS*/
	uint16_t length;         /*MILLIMETERS*/
	uint16_t baseline_speed; /*METERS PER SECOND*/
	uint16_t baseline_accel; /*METERS PER SECOND SQUARED*/
	uint16_t engineweight;   /*GRAMS*/

	/*Input properties of the car
	 (BACKEND WILL NOT MODIFY THESE, FRONTEND CAN MODIFY THEM)*/

	_Bool active;       /*0 OR 1*/
	uint16_t axis1; /*0 TO 65535*/
	uint16_t axis2; /*0 TO 65535*/
	uint16_t axis3; /*0 TO 65535*/
	uint16_t axis4; /*0 TO 65535*/
	_Bool btn1;         /*0 OR 1*/
	_Bool btn2;         /*0 OR 1*/
	_Bool btn3;         /*0 OR 1*/
	_Bool btn4;         /*0 OR 1*/

	/*Simulated properties of the car (MODIFYING THESE CAN LEAD TO
    FUNKY RESULTS, HOWEVER THERE IS NOTHING INHERENTLY WRONG WITH IT)*/

	unsigned int leaderboard1; /*RACE LEADERBOARD POSITION*/
	unsigned int leaderboard2; /*GLOBAL LEADERBOARD POSITION*/

	uint16_t pos1;
	uint16_t pos2;
	uint16_t revolutions; /*ENGINE REVOLUTIONS PER MINUTE*/

	uint16_t state1; /*FIRST INTERNAL STATE OF CAR, RESERVED FOR GRIP*/
	uint16_t state2; /*SECOND INTERNAL STATE OF CAR, RESERVED FOR DAMAGE*/
	uint16_t state3; /*THIRD INTERNAL STATE OF CAR*/
	uint16_t state4; /*FOURTH INTERNAL STATE OF CAR*/

	enum HWNET_Type1Enum car_state; /*SEE THE HWNET SPEC*/
	enum HWNET_Type2Enum mov_state; /*SEE THE HWNET SPEC*/
	enum HWNET_Type3Enum con_state; /*SEE THE HWNET SPEC*/

} H5_Vehicle;

typedef struct {
	H5_Vehicle racers[RACENUM];
	HWT_Circuit track;
} H5_World;

extern int H5_Init(char *trck, H5_World *stage);
extern int H5_Sim(H5_World *stage, unsigned int milli);
extern int H5_TransformServer(const H5_World *stage, HWNET_ServerPacket *serv,
			     enum HWNET_ModeEnum mode);
extern int H5_TransformClient(H5_World *stage, const HWNET_ClientPacket *cli);
