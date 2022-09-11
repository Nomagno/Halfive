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

#ifndef H5PHY_H
#define H5PHY_H
#include <halfive/h5doc/h5doc.h>
#include <halfive/h5doc/h5net.h>
#include <halfive/h5doc/h5t.h>
#include <halfive/h5req.h>
#include <halfive/h5stdlib.h>
#include <halfive/h5cr.h>
#include <halfive/h5rat.h>
#include <halfive/h5math.h>

#ifdef H5PHY_VM_SIMULATION
#include <halfive/h5vm/h5asm.h>
#include <halfive/h5vm/h5vm.h>
#endif

/*[h5vi graphics + audio] <& [Monolithic Object Scheduler (H5PHY MOS, handles physics)] (<-> [Network Play Module (H5PHY NETPM)]) <- [h5t track data, h5vi input, h5vm instances]*/

/*Approximate default storage with VM: 140KBs
Without VM worst-case scenario: 256Bs
*/

typedef enum {
	Horizontal_Magnets,
	Vertical_Magnets
} Magnet_Type;

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
	
	h5uchar additional_network_data[16]; /*String*/

	Magnet_Type magnet_type;
	h5uint width;           /*MILLIMETERS*/
	h5uint length;          /*MILLIMETERS*/
	h5uint height;          /*MILLIMETERS*/
	h5uchar agility;        /*FORCIBLY BOOST TURNING DEGREES OF CAR*/
	h5uint baseline_speed;  /*METERS PER SECOND*/
	h5uint baseline_accel;  /*METERS PER SECOND SQUARED*/
	h5uint weight;          /*HECTOGRAMS*/
	h5uint grip_level;      /*VERY COMPLEX, IF UNSURE KEEP AT 16k-32k*/
	h5uint downforce_level; /*0 TO 65535
	                        0 - NO DOWNFORCE
	                        65535 - CAR GLUED TO FLOOR*/
	h5uint battery_capacity; /*0 TO 65535, in mAH. Amperage determined 
	                           automatically by the rest of car stats*/


	/*Input properties of the car
	 (BACKEND WILL NOT MODIFY THESE, FRONTEND CAN MODIFY THEM)*/

	_Bool active; /*0 OR 1*/
	h5uchar axis1; /*0 TO 255*/
	h5uchar axis2; /*0 TO 255*/
	h5uchar axis3; /*0 TO 255*/
	h5uchar axis4; /*0 TO 255*/
	_Bool btn1;   /*0 OR 1*/
	_Bool btn2;   /*0 OR 1*/
	_Bool btn3;   /*0 OR 1*/
	_Bool btn4;   /*0 OR 1*/

	/*Simulated properties of the car (MODIFYING THESE CAN LEAD TO
    FUNKY RESULTS, HOWEVER THERE IS NOTHING INHERENTLY WRONG WITH IT)*/

#ifdef H5PHY_VM_SIMULATION
	H5VM_DefaultMemSetup vmmem;
	H5VM_GeneralMemory computer; /*More or less 140KBs of storage with default settings*/
	h5uchar
	    status_screen[16]; /*8x8 4-color screen for VM, each two contiguous
				  bits are a pixel. Mapped 0xC000 to 0xC00F*/
	h5uchar status_lights; /*Each two contiguous bits is a single light,
			       which can be off, color one, two, or three.
			       Mapped 0xC011 to 0xC014*/
#endif

	h5ulong leaderboard_local;  /*RACE LEADERBOARD POSITION*/
	h5ulong leaderboard_global; /*GLOBAL LEADERBOARD POSITION*/

	h5ulong speed; /*Meters per second*/
	h5ulong accel; /*Meters per second square*/	
	h5point_ulong pos; /*position, x and y, unsigned 32-bit (in millimiters)*/
	h5uchar rideheight; /*ride height, in mm, lower means more stable and faster, but also more succeptible to yaw/pitch changes*/
	h5uchar roll; /*roll relative to floor, in degrees. Depending on ride height, car width, length and height, might collide*/
	h5uchar pitch; /*pitch relative to floor, in degrees. Depending on ride height, car width, length and height, might collide*/
	h5uchar yaw; /*absolute yaw, biggest determinant of car direction, depending on grip a fast change in yaw
	               might contribute to loosing the rear of the car*/

	h5uint avg_curr_load; /*Average current load, in milliamperes*/
	h5uint battery_level; /*Percentage of battery level, where 0xFFFF is 100%*/
	h5uint revolutions; /*ENGINE REVOLUTIONS PER MINUTE*/
	h5ulong states[2];
	h5uchar engine1[4]; /*Store how ramped up it is, its current power, degradation info, and flags*/
	h5uchar engine2[4]; /*Store how ramped up it is, its current power, degradation info, and flags*/


	enum H5NET_Type1Enum car_state; /*SEE THE H5NET SPEC*/
	enum H5NET_Type2Enum mov_state; /*SEE THE H5NET SPEC*/
	enum H5NET_Type3Enum con_state; /*SEE THE H5NET SPEC*/

} H5_Vehicle;

/*Size for RACENUM = 4, with average VM: 3MiBs max
Size for RACENUM = 4 without VM: 3KiBs max*/
typedef struct {
	H5_Vehicle racers[RACENUM];
	H5T_Circuit track;
} H5_World;

extern unsigned H5_Init(const char *trck, H5_World *stage);
extern unsigned H5_Sim(H5_World *stage, h5uint delta /*Time since last frame, in milliseconds*/);
extern unsigned H5_TransformServer(const H5_World *stage,
				   H5NET_ServerPacket *serv,
				   enum H5NET_ModeEnum mode); /*Convert H5_World to a packet ready to send to clients*/
extern unsigned H5_TransformClient(H5_World *stage,
				   const H5NET_ClientPacket *cli); /*Mix client packet information with current H5_World*/
#endif
