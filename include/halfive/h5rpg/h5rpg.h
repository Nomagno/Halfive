/*
Copyright Nomagno 2022, 2023

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

/*Halfive RPG battle simulator (Yes, this is a joke component)*/

#ifndef H5RPG_H
#define H5RPG_H

#include <halfive/h5req.h>

enum H5RPG_Agent {
    H5RPG_empty = 0,
    H5RPG_human = 1,
    H5RPG_monster = 2,
    H5RPG_strongplant = 3,
    H5RPG_goblin = 4,
    H5RPG_cyclops = 5,
    H5RPG_bear = 6,
    H5RPG_eagle = 7,
    H5RPG_bigfrog = 8
};

enum H5RPG_Move {
    H5RPG_none = 0,
    H5RPG_slash = 1,        /*Depends on attack*/
    H5RPG_bite = 2,         /*Depends on attack*/
    H5RPG_punch = 3,        /*Depends on attack*/
    H5RPG_kick = 4,         /*Depends on attack*/
    H5RPG_magic_beam = 5,   /*Consumes special, depends on attack*/
    H5RPG_parry = 6,        /*Depends on speed*/
    H5RPG_strangle = 7,     /*Depends on attack*/
    H5RPG_block = 8,        /*Depends on defense*/
    H5RPG_magic_shield = 9, /*Consumes special, depends on defense*/
    H5RPG_heal = 10,        /*Depends on speed*/
    H5RPG_magic_heal = 11   /*Consumes special*/
};

enum H5RPG_Special {
    H5RPG_fail = 0, /*Cause enemy attack to be more likeky to fail by 10 */
    H5RPG_super_destroy = 1, /*Remove ~25% hp from target*/
    H5RPG_super_defend = 2,  /*Boost defense of all team by ~25% for 3 turns*/
    H5RPG_super_heal = 3 /*Refill health of all team members by ~50% or to full
			    health, whichever gives a smaller result*/
};

/*
Relationships (Advantage = ~25% damage boost for REGULAR MOVES):
fire -> plant
plant -> water
water -> fire
rock -> thunder
thunder -> ghost
ghost -> rock

fire -> ghost
plant -> thunder
water -> rock
rock -> fire
thunder -> water
ghost -> plant
*/
enum H5RPG_Elem {
    H5RPG_normal = 0,
    H5RPG_rock = 1,
    H5RPG_fire = 2,
    H5RPG_water = 3,
    H5RPG_thunder = 4,
    H5RPG_plant = 5,
    H5RPG_ghost = 6
};

typedef struct {
    enum { movement, special_movement, run, wait } type;
    union {
	enum H5RPG_Move move : 4;
	enum H5RPG_Special special : 4;
    } data;
} H5RPG_Action;

typedef struct {
    enum H5RPG_Agent type : 4;
    h5uchar max_health;
    h5uchar current_health;

    h5uchar attack;
    h5uchar attack_boost;
    h5uchar attack_boost_turns;

    h5uchar defense;
    h5uchar defense_boost;
    h5uchar defense_boost_turns;

    h5uchar speed; /*75% chance fastest agent will attack first*/
    h5uchar speed_boost;
    h5uchar speed_boost_turns;

    h5uchar max_special;
    h5uchar special; /*Refills at fixed rate per turn, needs to be full to
			perform special move*/
    h5uchar magic_special_consumption; /*How much special do normal magic
					  attacks consume*/
    h5uchar special_refill_rate;

    enum H5RPG_Move moves[4];
    enum H5RPG_Special special_move : 4;

    enum H5RPG_Elem element : 4;

    h5uchar fail_chance; /*0 - can't fail, 255 - will fail*/
    h5uchar fail_chance_boost;
    h5uchar fail_chance_boost_turns;
} H5RPG_Agent;

typedef struct {
    H5RPG_Agent team_1[4];
    H5RPG_Action actions_1[4];

    H5RPG_Agent team_2[4];
    H5RPG_Action actions_2[4];

    _Bool team1_winner;
    _Bool team2_winner;

    h5uchar max_turns;
    h5uchar turn_number;
} Match;

extern unsigned H5RPG_battleSim(Match *match);
#endif
