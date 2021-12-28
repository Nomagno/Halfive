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

#include "hwdoc.h"
#include "hwnet/hwnet.h"
#include "hwt/hwt.h"
#include "hwvm/hwstring.h"
#include "hwvm/hwvm.h"

#define RACENUM 32

typedef struct {

	/*Intrinsic properties of the car
	  (BACKEND WILL NOT MODIFY THESE)*/

	unsigned int width;          /*MILLIMETERS*/
	unsigned int length;         /*MILLIMETERS*/
	unsigned int baseline_speed; /*KILOMETERS PER HOUR*/
	unsigned int baseline_accel; /*KILOMETERS PER HOUR SQUARED*/
	unsigned int engineweight;   /*KILOGRAMS*/

	/*Input properties of the car
	 (BACKEND WILL NOT MODIFY THESE)*/

	_Bool active;       /*0 OR 1*/
	unsigned int axis1; /*0 TO 65535*/
	unsigned int axis2; /*0 TO 65535*/
	unsigned int axis3; /*0 TO 65535*/
	unsigned int axis4; /*0 TO 65535*/
	_Bool btn1;         /*0 OR 1*/
	_Bool btn2;         /*0 OR 1*/
	_Bool btn3;         /*0 OR 1*/
	_Bool btn4;         /*0 OR 1*/

	/*Simulated properties of the car (MODIFYING THESE CAN LEAD TO
    FUNKY RESULTS, HOWEVER THERE IS NOTHING INHERENTLY WRONG WITH IT)*/

	unsigned int leaderboard1; /*RACE LEADERBOARD POSITION*/
	unsigned int leaderboard2; /*GLOBAL LEADERBOARD POSITION*/

	int pos1;
	int pos2;
	int revolutions; /*ENGINE REVOLUTIONS PER MINUTE*/

	int state1; /*FIRST INTERNAL STATE OF CAR, RESERVED FOR GRIP*/
	int state2; /*SECOND INTERNAL STATE OF CAR, RESERVED FOR DAMAGE*/
	int state3; /*THIRD INTERNAL STATE OF CAR*/
	int state4; /*FOURTH INTERNAL STATE OF CAR*/

	enum TYPE1 car_state; /*SEE THE HWNET SPEC*/
	enum TYPE2 mov_state; /*SEE THE HWNET SPEC*/
	enum TYPE3 con_state; /*SEE THE HWNET SPEC*/

} car;

typedef struct {
	car racers[RACENUM];
	hwtrack track;
} world;

extern int H5Init(char *trck, world *stage);
extern int H5Sim(world *stage, unsigned int milli);
extern int H5TransformServer(world stage, hwpack_server *serv, enum MODE mode);
extern int H5TransformClient(world *stage, hwpack_client cli);
