/*
Copyright Nomagno 2022, 2023

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

/*H5RPG archetypes for monsters and characters*/

#ifndef H5RPGPROTO_H
#define H5RPGPROTO_H
#include <halfive/h5req.h>
#include <halfive/h5rpg/h5rpg.h>

#define protoHuman                                       \
	(H5RPG_Agent)                                        \
	{                                                    \
		H5RPG_human, /*type*/                            \
			100,	 /*max_health*/                      \
			100,	 /*current_health*/                  \
			60,		 /*attack*/                          \
			0,		 /*attack_boost*/                    \
			0,		 /*attack_boost_turns*/              \
			30,		 /*defense*/                         \
			0,		 /*defense_boost*/                   \
			0,		 /*defense_boost_turns*/             \
			25,		 /*speed*/                           \
			0,		 /*speed_boost*/                     \
			0,		 /*speed_boost_turns*/               \
			30,		 /*max_special*/                     \
			30,		 /*special*/                         \
			10,		 /*magic_special_consumption*/       \
			5,		 /*special_refill_rate*/             \
			{H5RPG_slash, H5RPG_parry, H5RPG_strangle,   \
				H5RPG_heal}, /*moves[4]*/                \
			H5RPG_fail,		 /*special_move*/            \
			H5RPG_thunder,	 /*element*/                 \
			25,				 /*fail_chance*/             \
			0,				 /*fail_chance_boost*/       \
			0				 /*fail_chance_boost_turns*/ \
	}

#define protoMonster                                         \
	(H5RPG_Agent)                                            \
	{                                                        \
		H5RPG_monster, /*type*/                              \
			140,	   /*max_health*/                        \
			140,	   /*current_health*/                    \
			70,		   /*attack*/                            \
			0,		   /*attack_boost*/                      \
			0,		   /*attack_boost_turns*/                \
			55,		   /*defense*/                           \
			0,		   /*defense_boost*/                     \
			0,		   /*defense_boost_turns*/               \
			20,		   /*speed*/                             \
			0,		   /*speed_boost*/                       \
			0,		   /*speed_boost_turns*/                 \
			20,		   /*max_special*/                       \
			5,		   /*special*/                           \
			15,		   /*magic_special_consumption*/         \
			5,		   /*special_refill_rate*/               \
			{H5RPG_bite, H5RPG_punch, H5RPG_strangle,        \
				H5RPG_block},	 /*moves[4]*/                \
			H5RPG_super_destroy, /*special_move*/            \
			H5RPG_rock,			 /*element*/                 \
			25,					 /*fail_chance*/             \
			0,					 /*fail_chance_boost*/       \
			0					 /*fail_chance_boost_turns*/ \
	}

#define protoStrongPlant                                       \
	(H5RPG_Agent)                                              \
	{                                                          \
		H5RPG_strongplant, /*type*/                            \
			80,			   /*max_health*/                      \
			80,			   /*current_health*/                  \
			40,			   /*attack*/                          \
			0,			   /*attack_boost*/                    \
			0,			   /*attack_boost_turns*/              \
			25,			   /*defense*/                         \
			20,			   /*defense_boost*/                   \
			3,			   /*defense_boost_turns*/             \
			10,			   /*speed*/                           \
			20,			   /*speed_boost*/                     \
			5,			   /*speed_boost_turns*/               \
			80,			   /*max_special*/                     \
			80,			   /*special*/                         \
			20,			   /*magic_special_consumption*/       \
			15,			   /*special_refill_rate*/             \
			{H5RPG_magic_beam, H5RPG_bite, H5RPG_strangle,     \
				H5RPG_magic_heal}, /*moves[4]*/                \
			H5RPG_super_defend,	   /*special_move*/            \
			H5RPG_plant,		   /*element*/                 \
			25,					   /*fail_chance*/             \
			0,					   /*fail_chance_boost*/       \
			0					   /*fail_chance_boost_turns*/ \
	}

#define protoGoblin                                              \
	(H5RPG_Agent)                                                \
	{                                                            \
		H5RPG_goblin, /*type*/                                   \
			90,		  /*max_health*/                             \
			90,		  /*current_health*/                         \
			50,		  /*attack*/                                 \
			12,		  /*attack_boost*/                           \
			3,		  /*attack_boost_turns*/                     \
			20,		  /*defense*/                                \
			0,		  /*defense_boost*/                          \
			0,		  /*defense_boost_turns*/                    \
			50,		  /*speed*/                                  \
			0,		  /*speed_boost*/                            \
			0,		  /*speed_boost_turns*/                      \
			20,		  /*max_special*/                            \
			10,		  /*special*/                                \
			10,		  /*magic_special_consumption*/              \
			5,		  /*special_refill_rate*/                    \
			{H5RPG_punch, H5RPG_kick, H5RPG_parry,               \
				H5RPG_magic_shield}, /*moves[4]*/                \
			H5RPG_fail,				 /*special_move*/            \
			H5RPG_fire,				 /*element*/                 \
			25,						 /*fail_chance*/             \
			0,						 /*fail_chance_boost*/       \
			0						 /*fail_chance_boost_turns*/ \
	}
#endif
