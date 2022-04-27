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

#ifndef HALFIVE_H
#define HALFIVE_H
#include <halfworld/hwdoc/hwdoc.h>
#include <halfworld/hwdoc/hwnet.h>
#include <halfworld/hwdoc/hwt.h>
#include <halfworld/hwreq.h>
#include <halfworld/hwstdlib.h>
#include <halfworld/hwcr.h>
#include <halfworld/rat.h>

#ifdef HALFIVE_VM_SIMULATION
#include <halfworld/hwvm/hwasm.h>
#include <halfworld/hwvm/hwvm.h>
#endif

/*Approximate default storage with VM: 140KBs
Without VM worst-case scenario: 256Bs
*/

typedef enum {
	H5_Tag_Forced,
	H5_Tag_Natural,
	H5_Tag_Quantum
} Engine_Type;

typedef enum {
	Horizontal_Magnets,
	Vertical_Magnets
} Traction_Type;

typedef struct {

	/*Intrinsic properties of the vehicle
	  (BACKEND WILL NOT MODIFY THESE, FRONTEND CAN MODIFY THEM)*/

	char name[16]; /*String, ASCII printable characters*/
	char info[16]; /*String, ASCII printable characters, following format:
	                 Characters 1 - 3: String to identify personal team
	                 Characters 4 - 6: String to identify racing team
	                 Characters 7 - 9: String to identify racing category
	                 Characters 10-13: Decimal number string, position in category
	                 Characters 14-16: Decimal number string, driver number*/
	
	hwuchar additional_network_data[16]; /*String*/

	Engine_Type engine_type;
	Traction_Type traction_type;
	hwuint width;           /*MILLIMETERS*/
	hwuint length;          /*MILLIMETERS*/
	hwuchar agility;        /*FORCIBLY BOOST TURNING DEGREES OF CAR*/
	hwuint baseline_speed;  /*METERS PER SECOND*/
	hwuint baseline_accel;  /*METERS PER SECOND SQUARED*/
	hwuint weight;          /*HECTOGRAMS*/
	hwuint grip_level;      /*VERY COMPLEX, IF UNSURE KEEP AT 16k-32k*/
	hwuint downforce_level; /*0 TO 65535
	                        0 - NO DOWNFORCE
	                        65535 - CAR GLUED TO FLOOR*/

	/*Input properties of the car
	 (BACKEND WILL NOT MODIFY THESE, FRONTEND CAN MODIFY THEM)*/

	_Bool active; /*0 OR 1*/
	hwuint axis1; /*0 TO 65535*/
	hwuint axis2; /*0 TO 65535*/
	hwuint axis3; /*0 TO 65535*/
	hwuint axis4; /*0 TO 65535*/
	_Bool btn1;   /*0 OR 1*/
	_Bool btn2;   /*0 OR 1*/
	_Bool btn3;   /*0 OR 1*/
	_Bool btn4;   /*0 OR 1*/

	/*Simulated properties of the car (MODIFYING THESE CAN LEAD TO
    FUNKY RESULTS, HOWEVER THERE IS NOTHING INHERENTLY WRONG WITH IT)*/

#ifdef HALFIVE_VM_SIMULATION
	HWVM_DefaultMemSetup vmmem;
	HWVM_GeneralMemory computer; /*More or less 140KBs of storage with default settings*/
	hwuchar
	    status_screen[16]; /*8x8 4-color screen for VM, each two contiguous
				  bits are a pixel. Mapped 0xC000 to 0xC00F*/
	hwuchar status_lights; /*Each two contiguous bits is a single light,
			       which can be off, color one, two, or three.
			       Mapped 0xC011 to 0xC014*/
#endif

	hwuint leaderboard_local;  /*RACE LEADERBOARD POSITION*/
	hwuint leaderboard_global; /*GLOBAL LEADERBOARD POSITION*/

	hwuchar cuadrantX, cuadrantY; /*Most significant byte of position
					 starting (0,0), in millimeters*/
	hwuint posX, posY; /*Least significant two bytes of position starting
			      (0,0), in millimeters*/

	hwuint revolutions; /*ENGINE REVOLUTIONS PER MINUTE*/
	hwuint states[4]; /*Internal states of the car, in total 64 bits of
			     storage*/
	enum HWNET_Type1Enum car_state; /*SEE THE HWNET SPEC*/
	enum HWNET_Type2Enum mov_state; /*SEE THE HWNET SPEC*/
	enum HWNET_Type3Enum con_state; /*SEE THE HWNET SPEC*/

} H5_Vehicle;

/*Size for RACENUM = 4, with average VM: 600KBs max
Size for RACENUM = 4 without VM: 1KB max*/
typedef struct {
	H5_Vehicle racers[RACENUM];
	HWT_Circuit track;
} H5_World;

extern unsigned H5_Init(const char *trck, H5_World *stage);
extern unsigned H5_Sim(H5_World *stage, hwuint delta, _Bool do_hwvm_sim);
extern unsigned H5_TransformServer(const H5_World *stage,
				   HWNET_ServerPacket *serv,
				   enum HWNET_ModeEnum mode);
extern unsigned H5_TransformClient(H5_World *stage,
				   const HWNET_ClientPacket *cli);
#endif

