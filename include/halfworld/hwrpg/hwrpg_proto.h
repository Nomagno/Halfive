#include <halfworld/hwrpg/hwrpg.h>

#ifndef HWRPGPROTO_H
#define HWRPGPROTO_H

#define protoHuman (Agent){ \
	human, /*type*/ \
	100, /*max_health*/ \
	100, /*current_health*/ \
	60, /*attack*/ \
	0, /*attack_boost*/ \
	0, /*attack_boost_turns*/ \
	30, /*defense*/ \
	0, /*defense_boost*/ \
	0, /*defense_boost_turns*/ \
	25, /*speed*/ \
	0, /*speed_boost*/ \
	0, /*speed_boost_turns*/ \
	30, /*max_special*/ \
	30, /*special*/ \
	10, /*magic_special_consumption*/ \
	5, /*special_refill_rate*/ \
	{ slash, parry, strangle, heal }, /*moves[4]*/ \
	fail, /*special_move*/ \
	thunder, /*element*/ \
	25, /*fail_chance*/ \
	0, /*fail_chance_boost*/ \
	0 /*fail_chance_boost_turns*/ \
}

#define protoMonster (Agent){ \
	monster, /*type*/ \
	140, /*max_health*/ \
	140, /*current_health*/ \
	70, /*attack*/ \
	0, /*attack_boost*/ \
	0, /*attack_boost_turns*/ \
	55, /*defense*/ \
	0, /*defense_boost*/ \
	0, /*defense_boost_turns*/ \
	20, /*speed*/ \
	0, /*speed_boost*/ \
	0, /*speed_boost_turns*/ \
	20, /*max_special*/ \
	5, /*special*/ \
	15, /*magic_special_consumption*/ \
	5, /*special_refill_rate*/ \
	{ bite, punch, strangle, block }, /*moves[4]*/ \
	super_destroy, /*special_move*/ \
	rock, /*element*/ \
	25, /*fail_chance*/ \
	0, /*fail_chance_boost*/ \
	0 /*fail_chance_boost_turns*/ \
}

#define protoStrongPlant (Agent){ \
	strongplant, /*type*/ \
	80, /*max_health*/ \
	80, /*current_health*/ \
	40, /*attack*/ \
	0, /*attack_boost*/ \
	0, /*attack_boost_turns*/ \
	25, /*defense*/ \
	20, /*defense_boost*/ \
	3, /*defense_boost_turns*/ \
	10, /*speed*/ \
	20, /*speed_boost*/ \
	5, /*speed_boost_turns*/ \
	80, /*max_special*/ \
	80, /*special*/ \
	20, /*magic_special_consumption*/ \
	15, /*special_refill_rate*/ \
	{ magic_beam, bite, strangle, magic_heal }, /*moves[4]*/ \
	super_defend, /*special_move*/ \
	plant, /*element*/ \
	25, /*fail_chance*/ \
	0, /*fail_chance_boost*/ \
	0 /*fail_chance_boost_turns*/ \
}

#define protoGoblin (Agent){ \
	goblin, /*type*/ \
	90, /*max_health*/ \
	90, /*current_health*/ \
	50, /*attack*/ \
	12, /*attack_boost*/ \
	3, /*attack_boost_turns*/ \
	20, /*defense*/ \
	0, /*defense_boost*/ \
	0, /*defense_boost_turns*/ \
	50, /*speed*/ \
	0, /*speed_boost*/ \
	0, /*speed_boost_turns*/ \
	20, /*max_special*/ \
	10, /*special*/ \
	10, /*magic_special_consumption*/ \
	5, /*special_refill_rate*/ \
	{ punch, kick, parry, magic_shield }, /*moves[4]*/ \
	fail, /*special_move*/ \
	fire, /*element*/ \
	25, /*fail_chance*/ \
	0, /*fail_chance_boost*/ \
	0 /*fail_chance_boost_turns*/ \
}
#endif
