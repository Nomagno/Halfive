#include <halfworld/hwreq.h>

#ifndef HWRPG_H
#define HWRPG_H

enum agent {
	empty = 0,
	human = 1,
	monster = 2,
	strongplant = 3,
	goblin = 4,
	cyclops = 5,
	bear = 6,
	eagle = 7,
	bigfrog = 8
};

enum move {
	none = 0,
	slash = 1, /*Depends on attack*/
	bite = 2, /*Depends on attack*/
	punch = 3, /*Depends on attack*/
	kick = 4, /*Depends on attack*/
	magic_beam = 5, /*Consumes special, depends on attack*/
	parry = 6, /*Depends on speed*/
	strangle = 7, /*Depends on attack*/
	block = 8, /*Depends on defense*/
	magic_shield = 9, /*Consumes special, depends on defense*/
	heal = 10, /*Depends on speed*/
	magic_heal = 11 /*Consumes special*/
};

enum special {
	fail = 0, /*Cause enemy attack to be more likeky to fail by 10 */
	super_destroy = 1, /*Remove ~25% hp from target*/
	super_defend = 2, /*Boost defense of all team by ~25% for 3 turns*/
	super_heal = 3 /*Refill health of all team members by ~50% or to full health, whichever gives a smaller result*/
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
enum elem {
	normal = 0,
	rock = 1,
	fire = 2,
	water = 3,
	thunder = 4,
	plant = 5,
	ghost = 6
};

typedef struct {
	enum {
		movement,
		special_movement,
		run,
		wait
	} type;
	union {
		enum move move: 4;
		enum special special: 4;
	} data;
} Action;

typedef struct {
	enum agent type: 4;
	hwuchar max_health;
	hwuchar current_health;

	hwuchar attack;
	hwuchar attack_boost;
	hwuchar attack_boost_turns;
	
	hwuchar defense;
	hwuchar defense_boost;
	hwuchar defense_boost_turns;

	hwuchar speed; /*75% chance fastest agent will attack first*/
	hwuchar speed_boost;
	hwuchar speed_boost_turns;

	hwuchar max_special;
	hwuchar special; /*Refills at fixed rate per turn, needs to be full to perform special move*/
	hwuchar magic_special_consumption; /*How much special do normal magic attacks consume*/
	hwuchar special_refill_rate;

	enum move moves[4];
	enum special special_move: 4;

	enum elem element: 4;

	hwuchar fail_chance; /*0 - can't fail, 255 - will fail*/
	hwuchar fail_chance_boost;
	hwuchar fail_chance_boost_turns;
} Agent;

typedef struct {
	Agent team_1[4];
	Action actions_1[4];

	Agent team_2[4];
	Action actions_2[4];
	
	_Bool team1_winner;
	_Bool team2_winner;

	hwuchar max_turns;
	hwuchar turn_number;
} Match;

extern unsigned HWRPG_BattleSim(Match *match);
#endif
