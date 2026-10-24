// @flags /O2 /arch:SSE /Gr
#include "projection_storage.h"
#include "view_context_storage.h"
/* UNKNOWN_157450.CPP: the game engine globals: the lifecycle callbacks of
   entry 22 and the team bookkeeping */

#include "unknown_11c920.h"
#include "camera_storage.h"
#include "unknown_123b30.h"
#include "globals.h"
#include "unknown_440dd8.h"
#include "engine_peer.h"
#include "unknown_157450.h"
#include "input_record.h"
#include "game_engine_events.h"
#include "unknown_163110.h"
#include "slot_handler.h"
static __forceinline long real_to_long(real value);
#include <string.h>
#include "unknown_13fd90.h"
#include <math.h>

extern s_record_pool *g_509434;
struct s_location;
void function_ce520(long weapon_index, long unit_index, bool flag);
void function_b75a0(long object_index, point3f const *point, vector3f const *forward,
	vector3f const *up, s_location const *location, bool flag);
void function_b73b0(long object_index);
long __stdcall function_3ddd0(long object_index);

struct s_engine_item_flags
{
	byte unknown00[0x16c];
	word : 7;
	word held : 1;
	word : 8;
};

// @retail 0x15e360
void function_15e360(point3f const *point, long object_index)
{
	if (g_55e4d0[g_4e9ae8->engine_index])
	{
		byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
		if (TEST_FIELD_BIT(((s_engine_item_flags *)object)->held))
			function_ce520(object_index, *(long *)(object + 0x154), true);
		function_b75a0(object_index, point, g_4687a8, g_4687b0, NULL, false);
		function_b73b0(object_index);
		long cache_index = function_3ddd0(object_index);
		if (cache_index != NONE)
			g_509434->data[(cache_index & 0xffff) * 0x100 + 2] = 0;
		*(long *)(object + 0x150) = NONE;
		*(long *)(object + 0x14c) = NONE;
	}
}

/* the players (g_4e8c24, 0x21c bytes each) */
/* a player's recent marks: an identifier, its code and when it was made */
struct s_engine_player_mark
{
	short identifier;
	byte code;
	byte unknown03;
	long time;
};

struct s_engine_player
{
	short salt;
	word flags;
	byte unknown04[0x28 - 4];
	short local_index;
	byte unknown2a[2];
	long unit_index;
	byte unknown30[0xc0 - 0x30];
	char team;
	char value_c1;
	byte unknownc2[0x164 - 0xc2];
	long value164;
	byte unknown168[0x170 - 0x168];
	long value170;
	byte unknown174[0x180 - 0x174];
	short value180;
	byte unknown182[0x194 - 0x182];
	long value194;
	byte unknown198[0x1a7 - 0x198];
	bool flag1a7;
	char value1a8;
	byte unknown1a9[0x1ac - 0x1a9];
	short value1ac;
	byte unknown1ae[0x1d8 - 0x1ae];
	s_engine_player_mark marks[8];
	byte unknown218[0x21c - 0x218];
};

/* the iterator over the players of 0x19f240 */
struct s_engine_player_iterator
{
	s_engine_player *player;
	s_record_pool *data;
	long index;
	long absolute_index;
};

/* the tag the engine needs before it can run (g_4e034c->index) */
struct s_engine_tag
{
	byte unknown00[8];
	void *data;
};

/* the scenario's netgame entries (g_4e0350, the count at +0x120) */
struct s_scenario_netgame_view
{
	byte unknown00[0x120];
	long count;
};

/* the game options' view of the players (16 of 0xe4 bytes at +0x2dc) */
struct s_options_player
{
	bool valid;
	byte unknown01[0x98 - 1];
	char team;
	byte unknown99[0xe4 - 0x99];
};

struct s_engine_options
{
	byte unknown00[0x184];
	dword flags_bit0 : 1;
	dword flags_bit1 : 1;
	dword flags_bit2 : 1;
	dword flags_bit3 : 1;
	dword flags_bit4 : 1;
	dword flags_bit5 : 1;
	dword flags_bit6 : 1;
	dword flags_bit7 : 1;
	dword flags_bit8 : 1;
	dword flags_bit9 : 1;
	dword flags_bit10 : 1;
	dword : 21;
	byte unknown188[0x190 - 0x188];
	long value190;
	byte unknown194[0x1bc - 0x194];
	long value1bc;
	byte unknown1c0[4];
	long value1c4;
	byte unknown1c8[0x1f0 - 0x1c8];
	long maximum_teams;
	byte unknown1f4[0x2dc - 0x1f4];
	s_options_player players[16];
};

dword g_502258[0x27];
s_local_engine_state g_4e9af0;

bool player_iterator_next(long *iterator);
void function_b58c0(long index, dword mask);
void function_1523c0();
void function_23aea0(void);
void teleporters_initialize(void);
void function_23f0a0(void);
void function_157790(void);
void function_157670(void);
void function_158140(void);

static inline s_engine_options *engine_options()
{
	return (s_engine_options *)g_4e6948;
}

static inline long local_player_next(long index)
{
	long result = NONE;
	long start = index == NONE ? 0 : index + 1;

	for (long i = start; i < 4; i++)
	{
		if (g_4e8c20->entries[i] != NONE)
		{
			result = i;
			break;
		}
	}
	return result;
}

// @retail 0x157450
void function_157450(void)
{
	s_mp_globals *data = (s_mp_globals *)function_123d40("unknown", "unknown", sizeof(s_mp_globals));

	memset(data, 0, sizeof(*data));
	memset(g_502258, 0, sizeof(g_502258));
	g_4e9ae8 = data;
	data->engine_index = NONE;
}

// @retail 0x15ace0
void function_15ace0(s_netgame_entry_state *entries)
{
	if (g_4e6948->mode != 4)
	{
		s_scenario_netgame_view *scenario = (s_scenario_netgame_view *)g_4e0350;

		memset(entries, 0, sizeof(s_netgame_entry_state) * 100);
		for (short i = 0; i < scenario->count; i++)
		{
			entries[i].index = NONE;
		}
	}
}

// @retail 0x157970
void function_157970(void)
{
	if (function_x340af0())
	{
		word teams = 0;

		for (long team = 0; team < 8; team++)
		{
			c_engine_peer *engine = game_engine_get();

			if (!engine || engine->p4(team))
			{
				if (!(teams & (1 << team)))
				{
					teams |= 1 << team;
				}
			}
		}
		function_xaee93d()->present_teams = teams;
	}
}

// @retail 0x1579f0
void function_1579f0(void)
{
	if (g_4e6948->mode != 4 && function_x340af0())
	{
		if (function_xaee93d()->field_c_2 == 0)
		{
			function_157790();
		}
		function_157670();
		function_158140();
	}
}

// @retail 0x1574c0
void function_1574c0(void)
{
	s_game_engine_globals *globals = function_xaee93d();
	s_statborg *statborg;
	long index;

	memset(globals, 0, sizeof(*globals));
	memset(&g_4e9af0, 0, sizeof(g_4e9af0));
	globals->engine_index = g_4e6948->mode_180;
	globals->index24 = NONE;
	globals->index28 = NONE;
	memset(globals->slots, NONE, sizeof(globals->slots));
	memset(globals->team_designators, NONE, sizeof(globals->team_designators));
	statborg = game_engine_statborg_inline();
	if (statborg)
	{
		memset(statborg, 0, sizeof(*statborg));
	}
	if (game_engine_get())
	{
		long tag_index = g_4e034c->index;

		if (tag_index == NONE || !((s_engine_tag *)g_4e3b44[tag_index & 0xffff].bytes)->data)
		{
			globals->engine_index = 0;
		}
	}
	if (game_engine_get())
	{
		function_23aea0();
		teleporters_initialize();
		function_23f0a0();
		function_15ace0(globals->netgame_entries);
		if (!game_engine_get()->p1())
		{
			function_xaee93d()->engine_index = NONE;
		}
		function_157970();
		function_1579f0();
	}
	statborg = game_engine_statborg_inline();
	if (statborg)
	{
		statborg->valid = true;
	}
	for (index = local_player_next(NONE); index != NONE; index = local_player_next(index))
	{
		if (index != NONE && g_4e8c20->entries[index] != NONE)
		{
			g_4e9af0.players[index].index = NONE;
		}
	}
}

// @retail 0x157670
void function_157670(void)
{
	long team = game_engine_get()->p26();
	s_game_engine_globals *globals;

	team = team == NONE ? 0 : team;
	globals = function_xaee93d();
	globals->assigned_teams = 0;
	globals->team_flags = 0;
	memset(globals->team_designators, NONE, sizeof(globals->team_designators));
	for (long count = 0; count < 8; count++, team++)
	{
		if (team == 8)
		{
			team = 0;
		}
		if (globals->field_c_2 & (1 << team))
		{
			long i;

			for (i = 0; i < 9; i++)
			{
				if ((globals->present_teams & (1 << i)) && !(globals->assigned_teams & (1 << i)))
				{
					break;
				}
			}
			if (i != 9)
			{
				globals->assigned_teams |= 1 << i;
				globals->team_flags |= 1 << team;
				globals->team_designators[i] = (short)team;
			}
		}
	}
	function_1523c0();
	if (game_engine_get() && function_xaee93d()->index24 != NONE)
	{
		function_b58c0(function_xaee93d()->index24, 1);
	}
}

// @retail 0x157790
void function_157790(void)
{
	s_engine_options *options = engine_options();
	s_game_engine_globals *globals;
	word teams = 0;
	long count = 0;
	long maximum;
	long present;
	long i;

	for (i = 0; i < 16; i++)
	{
		if (options->players[i].valid)
		{
			short team = options->players[i].team;

			if (team != -1 && !(teams & (1 << team)))
			{
				teams |= 1 << team;
				count++;
			}
		}
	}
	maximum = engine_options()->maximum_teams;
	if (!maximum)
	{
		maximum = 8;
	}
	globals = function_xaee93d();
	present = 0;
	for (i = 0; i < 8; i++)
	{
		if (globals->present_teams & (1 << i))
		{
			present++;
		}
	}
	if (maximum > present)
	{
		maximum = present;
	}
	if (count > 0 && count < maximum)
	{
		for (i = 0; i < 8; i++)
		{
			if (!(teams & (1 << i)))
			{
				teams |= 1 << i;
				count++;
				if (count == maximum)
				{
					break;
				}
			}
		}
	}
	globals->field_c_2 = teams;
	if (game_engine_get() && globals->index24 != NONE)
	{
		function_b58c0(globals->index24, 1);
	}
}

// @retail 0x157a40
word function_157a40(void)
{
	word result = 0;

	if (function_x340af0())
	{
		result = function_xaee93d()->team_flags;
	}
	return result;
}

// @retail 0x157a80
void function_157a80(void)
{
	s_engine_player_iterator iterator;

	iterator.data = g_4e8c24;
	iterator.absolute_index = NONE;
	iterator.index = NONE;
	while (player_iterator_next((long *)&iterator))
	{
		iterator.player->flag1a7 = false;
		iterator.player->value1a8 = NONE;
	}
}

// @retail 0x157bb0
void function_157bb0(void)
{
	c_engine_peer *object = g_55e4d0[g_4e9ae8->engine_index];

	if (object)
	{
		object->p2();
		g_4e9ae8->engine_index = NONE;
	}
}

void function_196780(void);

/* unknown_1967d0.cpp's 0x1968b0 (sets an input counter, clamped to its
   range), which retail inlines here; its own file is built /Ob1 */
static inline void input_counter_set_inlined(long c, long a, long b, long value)
{
	if (g_510ca0 && !g_510ca1)
	{
		long minimum = g_46ddc0[b].minimum;
		long maximum = g_46ddc0[b].maximum;
		if (a != NONE)
		{
			s_input_counter *counter = &g_511bf4.all[a * 0x1b5 + b];
			counter->value = value < minimum ? minimum : (value > maximum ? maximum : value);
		}
		if (c != NONE)
		{
			s_input_counter *counter = &g_511bf4.counters[0][c * 0x2d + b];
			counter->value = value < minimum ? minimum : (value > maximum ? maximum : value);
		}
	}
}

/* recomputes the teams that have players */
// score 32 (was 32): the old team mask is kept as a 32 bit value here and as a 16 bit one in
// retail; the comparison and the update of the two masks use different registers, and the input
// counter loop uses different ones too.
// @retail 0x158140
void function_158140(void)
{
	s_game_engine_globals *globals = function_xaee93d();
	dword old_playing_teams = globals->playing_teams;
	word *team_mask = &globals->team_mask;
	dword old_team_mask = *team_mask;
	dword playing_teams = 0;
	volatile dword present_teams = 0;
	s_record_pool_iterator iterator;
	s_engine_player *player;

	iterator.data = g_4e8c24;
	iterator.index = NONE;
	while ((player = (s_engine_player *)data_iterator_next_inlined(&iterator)) != 0)
	{
		long team = player->team;

		if (team != NONE)
		{
			present_teams |= 1 << team;
			if (!(player->flags & 2))
			{
				playing_teams |= 1 << player->team;
			}
		}
	}
	if (playing_teams != old_playing_teams || present_teams != old_team_mask)
	{
		dword added_teams = ~old_playing_teams & playing_teams;
		long team;

		globals->playing_teams = (word)playing_teams;
		*team_mask = old_team_mask | (word)present_teams;
		if (game_engine_get() && globals->index24 != NONE)
		{
			function_b58c0(globals->index24, 1);
		}
		function_196780();
		for (team = 0; team < 8; team++)
		{
			if (added_teams & (1 << team))
			{
				input_counter_set_inlined(team, NONE, 0, 1);
			}
		}
	}
}

// @retail 0x1584c0
void function_1584c0(long index)
{
	long player_index = NONE;

	if (index != NONE)
	{
		player_index = g_4e8c20->entries[index];
	}
	function_xaee93d()->timers[index] = 1.0f;
	function_xaee93d()->timer_flags |= 1 << index;
	if (player_index != NONE)
	{
		((s_engine_player *)g_4e8c24->data)[player_index & 0xffff].value170 = engine_options()->value1bc;
	}
}

// @retail 0x1587b0
long function_1587b0(long player_index)
{
	long result = 0;
	s_statborg *statborg = game_engine_statborg_inline();

	if (statborg)
	{
		result = statborg->players[player_index & 0xffff].score;
	}
	return result;
}

// @retail 0x1587f0
long function_1587f0(long team)
{
	long result = NONE;

	if (game_engine_statborg_inline() && function_x340af0() && game_engine_team_is_active(team))
	{
		result = game_engine_get_statborg()->teams[team].score;
	}
	return result;
}

// @retail 0x158850
void function_158850(long *type, long *count)
{
	if (engine_options()->value190)
	{
		*type = 2;
		short number = function_xaee93d()->value_e0;
		*count = number > 0 ? number : 0;
	}
	else
	{
		*type = 1;
		short number = function_xaee93d()->value_e0;
		*count = number > 0 ? number : 0;
	}
}

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : ((x) > (hi) ? (hi) : (x)))

/* the juggernauts (juggernaut.cpp) */
struct s_juggernaut_globals
{
	word players;
	word teams;
};

extern s_juggernaut_globals *g_510c9c;

bool juggernaut_is(short player_index);
bool function_15db30(long player_index);
long function_c8f60(long unit_index, short seat_index);

static inline s_engine_player *engine_player_get(long player_index)
{
	return (s_engine_player *)g_4e8c24->data + (player_index & 0xffff);
}

static inline bool engine_team_is_active(long team)
{
	bool result = false;

	if (function_x340af0() && team >= 0 && team < 8)
	{
		result = (function_xaee93d()->team_mask & (1 << team)) != 0;
	}
	return result;
}

// @retail 0x1588b0
real function_1588b0(long player_index, long type)
{
	player_index &= 0xffff;
	long index = player_index;
	s_engine_player *player = (s_engine_player *)g_4e8c24->data + index;
	real result = 1.0f;

	switch (type)
	{
	case 2:
		if (g_4e6948->mode_180 == 7 && juggernaut_is((short)player_index))
		{
			if (g_4e6948->flags22c & 2)
			{
				result = 3.0f;
			}
			else
			{
				switch (engine_options()->value1c4)
				{
				case 0:
					result = 1.0f;
					break;
				case 1:
					result = 0.0f;
					break;
				case 2:
					result = 1.0f;
					break;
				}
			}
		}
		else
		{
			switch (engine_options()->value1c4)
			{
			case 0:
				result = 1.0f;
				break;
			case 1:
				result = 0.0f;
				break;
			case 2:
				result = 3.0f;
				break;
			}
		}
		switch (player->value_c1)
		{
		case 0:
			result *= 1.0f;
			break;
		case 1:
			result *= 0.75f;
			break;
		case 2:
			result *= 0.5f;
			break;
		case 3:
			result *= 0.25f;
			break;
		}
		break;
	}
	return result;
}

static inline bool team_designator_is_assigned(long index)
{
	bool result = false;

	if (index >= 0 && index < 9)
	{
		result = (function_xaee93d()->assigned_teams & (1 << index)) != 0;
	}
	return result;
}

// @retail 0x158990
short function_158990(long team)
{
	long i;

	for (i = 0; i < 9; i++)
	{
		if (team_designator_is_assigned(i) && function_xaee93d()->team_designators[i] == team)
		{
			break;
		}
	}
	if (i == 9)
	{
		i = NONE;
	}
	return i;
}

// @retail 0x158a50
bool function_158a50(long player_index)
{
	s_engine_player *player = engine_player_get(player_index);
	word flags = player->flags;
	bool result = false;

	if ((flags & 1) && !(flags & 2) && player->team != NONE)
	{
		if (player->unit_index != NONE)
			result = true;
		else if (!(flags & 0x4000) && !function_15db30(player_index))
			result = true;
	}
	return result;
}

// @retail 0x158ab0
long function_158ab0(void)
{
	long count = 0;

	if (function_x340af0())
	{
		for (long team = 0; team < 8; team++)
		{
			if (engine_team_is_active(team))
			{
				count++;
			}
		}
	}
	else
	{
		s_record_pool_iterator iterator;

		iterator.data = g_4e8c24;
		iterator.index = NONE;
		while (data_iterator_next_inlined(&iterator))
		{
			count++;
		}
	}
	return count;
}

/* data_datum_iterator_next (data_iterator.cpp), which retail inlines here */
static inline bool data_datum_iterator_next_inlined(s_data_datum_iterator *iterator)
{
	s_record_pool *data = iterator->data;
	long index = data_next_absolute_index_inlined(data, iterator->index + 1);
	byte *datum;

	if (index != NONE)
	{
		datum = data->data + data->size * index;
		iterator->index = index;
		iterator->datum_index = (*(short *)datum << 16) | index;
	}
	else
	{
		iterator->index = data->maximum_count;
		iterator->datum_index = NONE;
		datum = 0;
	}
	iterator->datum = datum;
	return datum != 0;
}

// @retail 0x158c00
void function_158c00(long *player_index, long *active_count, long *team_count)
{
	dword field_c_2 = 0;
	dword teams = 0;
	long teams_counted = 0;
	long active_counted = 0;
	s_data_datum_iterator iterator;

	iterator.data = g_4e8c24;
	iterator.index = NONE;
	while (data_datum_iterator_next_inlined(&iterator))
	{
		s_engine_player *player = (s_engine_player *)iterator.datum;
		long team = player->team;
		long datum_index = iterator.datum_index;

		if (!(player->flags & 2) && team >= 0 && team < 16 && team != NONE)
		{
			dword bit = 1 << team;

			if (!(teams & bit))
			{
				teams |= bit;
				teams_counted++;
			}
			if (function_158a50(datum_index) || !(*(byte const volatile *)&player->flags & 3))
			{
				*player_index = datum_index;
				if (!(field_c_2 & bit))
				{
					field_c_2 |= bit;
					active_counted++;
				}
			}
		}
	}
	if (team_count)
	{
		*team_count = teams_counted;
	}
	if (active_count)
	{
		*active_count = active_counted;
	}
}

// @retail 0x158e20
void function_158e20(void)
{
	s_engine_player_iterator iterator;
	long count = 0;

	iterator.data = g_4e8c24;
	iterator.absolute_index = NONE;
	iterator.index = NONE;
	while (player_iterator_next((long *)&iterator))
	{
		count++;
	}
	if (count > 6)
	{
		function_xaee93d()->flags |= 2;
	}
	if (count >= 5)
	{
		function_xaee93d()->flags |= 4;
	}
	if (count >= 9)
	{
		function_xaee93d()->flags |= 8;
	}
}

/* the object header data's view of a unit (g_4e0300) */
struct s_engine_unit_header
{
	byte unknown00[8];
	long *object;
};

struct s_engine_unit_definition
{
	byte unknown00[0x1c8];
	long seat_count;
};

// @retail 0x158ff0
bool function_158ff0(long unit_index)
{
	long *unit = ((s_engine_unit_header *)g_4e0300->data)[unit_index & 0xffff].object;
	s_engine_unit_definition *definition = (s_engine_unit_definition *)g_4e3b44[*unit & 0xffff].bytes;
	long seat_count = definition->seat_count;
	long seat;

	for (seat = 0; seat < seat_count; seat++)
	{
		if (function_c8f60(unit_index, (short)seat) != NONE)
		{
			break;
		}
	}
	return seat != seat_count;
}

static inline real local_player_fraction(long local_index)
{
	real fraction = (real)g_4e9af0.timers[local_index] * g_510c54->rate * 4.0f;

	return PIN(fraction, 0.0f, 1.0f);
}

// @retail 0x159170
bool function_159170(long player_index)
{
	bool result = true;

	if (game_engine_get())
	{
		if (local_player_fraction(engine_player_get(player_index)->local_index) >= 1.0f)
		{
			result = false;
		}
	}
	return result;
}

// @retail 0x1591e0
real function_1591e0(long local_index)
{
	real result = 1.0f;

	if (game_engine_get())
	{
		result = 1.0f - local_player_fraction(local_index);
	}
	return result;
}


// score 14 (was 14): retail compares the timer as a float with SSE: it loads
// 1.0f into xmm1, copies it as the default and always runs comiss, even when
// g_4b9ed8 is NONE. Our build folds the NONE path past the comparison in
// every form. A float timer and 1.0f select SSE, but the shorter folded
// sequence shifts the rest of the function (score 31); the double form's x87
// sequence keeps it aligned (14), so it stays. A conditional expression, an
// inline function, a 1.0f local and const or mutable references to the timer
// or the constant all fold the same way; the 600 s permutation run found no
// match.
// @retail 0x159610
bool function_159610(void)
{
	bool result = false;

	if (game_engine_get())
	{
		double timer = 1.0;

		if (g_4b9ed8 != NONE)
		{
			timer = function_xaee93d()->timers[g_4b9ed8];
		}
		if (timer >= 1.0)
		{
			long player_index = NONE;

			if (g_4b9ed8 != NONE)
			{
				player_index = g_4e8c20->entries[g_4b9ed8];
			}
			result = true;
			if (player_index != NONE && engine_player_get(player_index)->unit_index != NONE)
				result = false;
		}
	}
	return result;
}

static inline bool game_engine_respawn_flag()
{
	bool result = false;
	long mode = g_4e6948->mode_180;

	if (mode == 1 || mode == 9)
	{
		result = !g_4e6948->flags22c_bits.bit4;
	}
	else if (mode == 3)
	{
		result = !g_4e6948->flags22c_bits.bit0;
	}
	return result;
}

// @retail 0x159d40
bool function_159d40(void)
{
	return game_engine_respawn_flag();
}

// @retail 0x159d80
bool function_159d80(void)
{
	return !game_engine_respawn_flag();
}

// @retail 0x159dd0
bool function_159dd0(long player_index)
{
	bool result = false;

	if (game_engine_get() && g_4e6948->mode_180 == 7 && juggernaut_is((short)player_index) && (g_4e6948->flags22c & 0x20))
	{
		result = true;
	}
	return result;
}

// @retail 0x15dea0
bool function_15dea0(void)
{
	return game_engine_get() ? true : false;
}

// @retail 0x15fe50
void function_15fe50(long value)
{
	if (g_4e6948->mode != 4)
	{
		function_xaee93d()->value_c04 = value;
	}
}

// @retail 0x15ff50
bool function_15ff50(long a, long b)
{
	bool result = false;
	c_engine_peer *engine = game_engine_get();

	if (engine)
	{
		result = engine->p35(a, b);
	}
	return result;
}

// @retail 0x15fef0
bool function_15fef0(long player_index)
{
	bool result = true;

	if (game_engine_get())
	{
		if (g_4e6948->mode_180 == 7 && juggernaut_is((short)player_index))
		{
			result = g_4e6948->flags22c_bits.bit0;
		}
		else
		{
			result = g_4e6948->flags184.bit1;
		}
	}
	return result;
}

/* the multiplayer globals tag (g_4e034c->index): the tag references the
   engine picks its default equipment from (each reference's index at +4) */
struct s_tag_reference
{
	dword group_tag;
	long index;
};

struct s_multiplayer_equipment
{
	s_tag_reference references[12];
};

struct s_multiplayer_globals_data
{
	byte unknown00[0x74];
	s_multiplayer_equipment *equipment;
	byte unknown78[4];
	s_tag_reference *objects7c;
	byte unknown80[4];
	s_tag_reference *objects84;
};

struct s_multiplayer_globals_tag
{
	byte unknown00[0xc];
	s_multiplayer_globals_data *data;
};

static inline s_multiplayer_globals_data *multiplayer_globals_data()
{
	return ((s_multiplayer_globals_tag *)g_4e3b44[g_4e034c->index & 0xffff].bytes)->data;
}

static inline char random_option(short range)
{
	return (char)(random_index((dword *)&g_4e7408->unknown0, range) + 1);
}

/* the engine's netgame entries, with the chooser of their default equipment
   (the choosers ignore this) */
class c_engine_equipment
{
public:
	long choose_15a140(long default_value, char option, long *name);
	long choose_15a250(long default_value, char option, long *name);
	long choose_15a310(long default_value, char option, long *name);
	long choose_15a3d0(long default_value, char option, long *name);
	long choose(long default_value, long *name, char kind);
	void release_entry(long object_index, long index);

	s_netgame_entry_state entries[100];
};

// @retail 0x15a140
long c_engine_equipment::choose_15a140(long default_value, char option, long *name)
{
	long result = default_value;
	s_multiplayer_globals_data *data = multiplayer_globals_data();

	switch (option)
	{
	case 1:
		result = data->equipment->references[0].index;
		*name = 0;
		break;
	case 2:
		result = data->equipment->references[0].index;
		*name = 0x5000694;
		break;
	case 3:
		result = data->equipment->references[1].index;
		*name = 0;
		break;
	case 4:
		result = data->equipment->references[6].index;
		*name = 0;
		break;
	case 5:
		result = data->equipment->references[5].index;
		*name = 0;
		break;
	case 6:
		do
		{
			result = choose_15a140(default_value, random_option(5), name);
		}
		while (result == NONE);
		break;
	case 7:
		result = NONE;
		break;
	}
	return result;
}

// @retail 0x15a250
long c_engine_equipment::choose_15a250(long default_value, char option, long *name)
{
	long result = default_value;
	s_multiplayer_globals_data *data = multiplayer_globals_data();

	switch (option)
	{
	case 1:
		result = data->equipment->references[2].index;
		*name = 0xb00078b;
		break;
	case 2:
		result = data->equipment->references[3].index;
		*name = 0xb00078b;
		break;
	case 3:
		do
		{
			result = choose_15a250(default_value, random_option(2), name);
		}
		while (result == NONE);
		break;
	case 4:
		result = NONE;
		break;
	}
	return result;
}

// @retail 0x15a310
long c_engine_equipment::choose_15a310(long default_value, char option, long *name)
{
	long result = default_value;
	s_multiplayer_globals_data *data = multiplayer_globals_data();

	switch (option)
	{
	case 1:
		result = data->equipment->references[4].index;
		*name = 0;
		break;
	case 2:
		result = data->equipment->references[4].index;
		*name = 0;
		break;
	case 3:
		do
		{
			result = choose_15a310(default_value, random_option(2), name);
		}
		while (result == NONE);
		break;
	case 4:
		result = NONE;
		break;
	}
	return result;
}

// @retail 0x15a3d0
long c_engine_equipment::choose_15a3d0(long default_value, char option, long *name)
{
	long result = default_value;
	s_multiplayer_globals_data *data = multiplayer_globals_data();

	switch (option)
	{
	case 1:
		result = data->equipment->references[7].index;
		*name = 0;
		break;
	case 2:
		result = data->equipment->references[9].index;
		*name = 0;
		break;
	case 3:
		result = data->equipment->references[8].index;
		*name = 0;
		break;
	case 4:
		result = data->equipment->references[10].index;
		*name = 0;
		break;
	case 5:
		do
		{
			result = choose_15a3d0(default_value, random_option(4), name);
		}
		while (result == NONE);
		break;
	case 6:
		result = NONE;
		break;
	}
	return result;
}

/* the game options' equipment choices (+0x209..+0x20f) */
struct s_options_equipment
{
	byte unknown00[0x209];
	char options[7];
};

// @retail 0x15a4d0
long c_engine_equipment::choose(long default_value, long *name, char kind)
{
	switch (kind)
	{
	case 1:
		return choose_15a140(default_value, ((s_options_equipment *)g_4e6948)->options[0], name);
	case 2:
		return choose_15a140(default_value, ((s_options_equipment *)g_4e6948)->options[1], name);
	case 3:
		return choose_15a250(default_value, ((s_options_equipment *)g_4e6948)->options[2], name);
	case 4:
		return choose_15a310(default_value, ((s_options_equipment *)g_4e6948)->options[3], name);
	case 5:
		return choose_15a250(default_value, ((s_options_equipment *)g_4e6948)->options[4], name);
	case 6:
		return choose_15a3d0(default_value, ((s_options_equipment *)g_4e6948)->options[5], name);
	case 7:
		return choose_15a3d0(default_value, ((s_options_equipment *)g_4e6948)->options[6], name);
	default:
		__assume(0);
	}
}

// @retail 0x15a5b0
long function_15a5b0(long tag_index)
{
	s_multiplayer_globals_data *data = multiplayer_globals_data();

	if (tag_index == data->objects7c[0].index && !TEST_FIELD_BIT(engine_options()->flags_bit10) ||
		tag_index == data->objects7c[1].index && !TEST_FIELD_BIT(engine_options()->flags_bit10) ||
		tag_index == data->objects84[0].index && !TEST_FIELD_BIT(engine_options()->flags_bit9) ||
		tag_index == data->objects84[1].index && (!TEST_FIELD_BIT(engine_options()->flags_bit8) || engine_options()->value1c4 == 1))
	{
		return NONE;
	}
	return tag_index;
}

/* the objects (g_4e0300) as the game engine sees them */
struct s_engine_object
{
	long definition_index;
	byte unknown04[0x30 - 4];
	point3f position;
	byte unknown3c[0xaa - 0x3c];
	byte type;
	byte unknownab[0xaf - 0xab];
	char netgame_entry;
	byte unknownb0[0xd4 - 0xb0];
	long simulation_index;
	byte unknownd8[0x13c - 0xd8];
	long value13c;
	byte unknown140[0x150 - 0x140];
	long value150;
	long value154;
	byte unknown158[0x16c - 0x158];
	union
	{
		byte flags16c;
		struct
		{
			word flag16c_bit0_5 : 6;
			word flag16c_bit6 : 1;
			word flag16c_bit7 : 1;
		};
	};
	byte unknown16e[0x17e - 0x16e];
	short value17e;
};

struct s_engine_object_header
{
	byte unknown00[8];
	s_engine_object *object;
};

static inline s_engine_object *engine_object_get(long object_index)
{
	return ((s_engine_object_header *)g_4e0300->data)[object_index & 0xffff].object;
}

/* marks an object's simulation entity dirty */
static inline void engine_object_mark_dirty(long object_index, dword mask)
{
	s_engine_object *object = engine_object_get(object_index);

	if (object->simulation_index != NONE)
	{
		function_b58c0(object->simulation_index, mask);
	}
}

static inline s_game_engine_object_entry *game_engine_object_entry(long object_index)
{
	s_game_engine_globals *globals = function_xaee93d();
	s_game_engine_object_entry *result = 0;

	for (long i = 0; i < globals->object_count; i++)
	{
		if (globals->objects[i].object_index == object_index)
		{
			result = &globals->objects[i];
		}
	}
	return result;
}

/* the scenario's netgame equipment (g_4e0350 + 0x124, 0x90 bytes each) */
struct s_scenario_netgame_equipment
{
	byte unknown00[0xe];
	short spawn_time;
	byte unknown10[0x58 - 0x10];
	dword group_tag;
	long tag_index;
	byte unknown60[0x90 - 0x60];
};

struct s_scenario_netgame_equipment_view
{
	byte unknown00[0x124];
	s_scenario_netgame_equipment *equipment;
};

struct s_item_collection
{
	byte unknown00[8];
	short spawn_time;
};

// score 5 (was 5): the tick count is sign extended into eax here and into edx in retail.
// @retail 0x15ac50
long function_15ac50(long index)
{
	s_scenario_netgame_equipment *equipment = &((s_scenario_netgame_equipment_view *)g_4e0350)->equipment[index];
	real seconds = 30.0f;
	short spawn_time = equipment->spawn_time;
	long ticks;
	if (spawn_time)
	{
		ticks = spawn_time;
	}
	else
	{
		if ((equipment->group_tag != 'itmc' && equipment->group_tag != 'vehc') || equipment->tag_index == NONE)
			goto done;
		short collection_time = ((s_item_collection *)g_4e3b44[equipment->tag_index & 0xffff].bytes)->spawn_time;
		if (!collection_time)
			goto done;
		ticks = collection_time;
	}
	seconds = (real)ticks;
done:
	return real_to_long(seconds);
}

// @retail 0x15b1e0
void c_engine_equipment::release_entry(long object_index, long index)
{
	s_engine_object *object = (((s_engine_object_header *)g_4e0300->data) + (object_index & 0xffff))->object;

	if (g_4e6948->mode != 4)
	{
		entries[index].index = NONE;
	}
	object->netgame_entry = NONE;
}

// @retail 0x15b220
void function_15b220(long object_index, long index)
{
	s_engine_object *object = engine_object_get(object_index);

	if (g_4e6948->mode != 4)
	{
		function_xaee93d()->netgame_entries[*(long const volatile *)&index].index = NONE;
	}
	object->netgame_entry = NONE;
}

void function_1967d0(long a, long c, long b, long delta);

// @retail 0x15b930
void function_15b930(long player_index, bool by_team, long counter, long delta)
{
	long index = player_index & 0xffff;
	long const *counter_reference = &counter;
	long const *delta_reference = &delta;
	long team;

	if (by_team)
	{
		team = ((s_engine_player *)g_4e8c24->data)[index].team;
	}
	else
	{
		team = NONE;
	}
	function_1967d0(index, team, *counter_reference, *delta_reference);
}

long function_196ef0(byte code);
void function_196e60(long b, long a, long c, long delta);

/* the identifier of the next player mark */
short g_47ff84 = 42;

// @retail 0x15cca0
short function_15cca0(long player_index, byte code)
{
	short result = NONE;

	if (game_engine_get() && player_index != NONE)
	{
		long maximum_age = g_510c54->field_2_3 * 60;
		s_engine_player *player = engine_player_get(player_index);
		long slot = 0;
		long oldest_age = 0;
		long i;

		for (i = 0; i < 8; i++)
		{
			s_engine_player_mark *mark = &player->marks[i];
			long age = g_510c54->game_time - mark->time;

			if (mark->identifier == NONE || age > maximum_age)
			{
				slot = i;
				break;
			}
			if (age > oldest_age)
			{
				slot = i;
				oldest_age = age;
			}
		}
		long time = g_510c54->game_time;
		result = g_47ff84++;
		player->marks[slot].identifier = result;
		player->marks[slot].code = code;
		player->marks[slot].time = time;
		function_196e60(player_index & 0xffff, 4, function_196ef0(code), 1);
	}
	return result;
}

// score 24 (was 24): the datum index and the record count are kept in swapped registers through
// the whole lookup.
// @retail 0x15cd90
void function_15cd90(long player_index, short identifier, long other_player_index)
{
	s_engine_player *player = (s_engine_player *)datum_get_inlined(g_4e8c24, player_index);

	if (identifier != NONE && player && game_engine_get())
	{
		short team = player->team;
		short other_team = engine_player_get(other_player_index)->team;

		if (game_engine_get()->p27(other_team, team))
		{
			for (long i = 0; i < 8; i++)
			{
				s_engine_player_mark *mark = &player->marks[i];

				if (mark->identifier == identifier)
				{
					function_196e60(player_index & 0xffff, 5, function_196ef0(mark->code), 1);
					mark->identifier = NONE;
				}
			}
		}
	}
}

// @retail 0x15db80
bool function_15db80(long team)
{
	bool result = false;

	if (function_x340af0() && g_4e6948->value1b8 > 0)
	{
		s_engine_player_iterator iterator;

		result = true;
		iterator.data = g_4e8c24;
		iterator.absolute_index = NONE;
		iterator.index = NONE;
		while (player_iterator_next((long *)&iterator))
		{
			s_engine_player *player = iterator.player;

			if (player->team == team && (player->unit_index != NONE || player->value1ac != 0))
			{
				result = false;
				break;
			}
		}
	}
	return result;
}

static inline void game_engine_player_mark_dirty(short absolute_index, dword mask)
{
	if (game_engine_get())
	{
		long index = function_xaee93d()->slots[absolute_index];

		if (index != NONE)
		{
			function_b58c0(index, mask);
		}
	}
}

void function_196470(void);

// @retail 0x158000
void function_158000(long previous_index, long player_index)
{
	c_engine_peer *engine = game_engine_get();

	if (engine)
	{
		long absolute_index = player_index & 0xffff;

		engine->p9(previous_index, player_index);
		game_engine_player_mark_dirty((short)previous_index, 0x7ff);
		game_engine_player_mark_dirty((short)absolute_index, 0x7ff);
		function_196470();
	}
}

// @retail 0x152140
void function_152140(long player_index)
{
	if (g_4e6948->mode != 4)
	{
		s_engine_player *player = engine_player_get(player_index);
		player->flags |= 0x800;
		real ticks = (real)g_510c54->field_2_3 * 2.0f;
		long rounded_ticks;

		__asm
		{
			fld ticks
			fistp rounded_ticks
		}
		player->value180 = (short)rounded_ticks;
		game_engine_player_mark_dirty((short)player_index, 0x100);
	}
}

// @retail 0x15dc40
void function_15dc40(void)
{
	s_engine_player_iterator iterator;

	iterator.data = g_4e8c24;
	iterator.absolute_index = NONE;
	iterator.index = NONE;
	while (player_iterator_next((long *)&iterator))
	{
		long absolute_index = iterator.index & 0xffff;

		iterator.player->value164 = 0;
		function_xaee93d()->players[absolute_index].value0 = 0;
		game_engine_player_mark_dirty((short)absolute_index, 1);
	}
}

// @retail 0x15e300
void function_15e300(long object_index)
{
	if (game_engine_get())
	{
		s_game_engine_object_entry *entry = game_engine_object_entry(object_index);

		if (entry->other_index != NONE)
		{
			entry->value0c = engine_object_get(entry->other_index)->value13c;
		}
	}
}

// @retail 0x15e250
void function_15e250(long object_index, long other_index)
{
	if (game_engine_get())
	{
		s_engine_object *object = engine_object_get(object_index);
		s_engine_object *other = engine_object_get(other_index);
		s_game_engine_object_entry *entry = game_engine_object_entry(object_index);

		entry->other_index = other_index;
		entry->value0c = other->value13c;
		object->flags16c |= 0x80;
		game_engine_get()->p23(object_index, other_index);
	}
}

// @retail 0x15e460
void function_15e460(long object_index, long value)
{
	s_engine_object *object = engine_object_get(object_index);
	s_game_engine_object_entry *entry = game_engine_object_entry(object_index);

	object->flag16c_bit7 = false;
	entry->other_index = NONE;
	entry->value0c = NONE;
	game_engine_get()->p24(object_index, value);
}

// @retail 0x15eb20
bool function_15eb20(long player_index)
{
	bool result = false;
	if (!game_engine_get() || player_index == NONE)
		result = true;
	else if (g_4e6948->mode_180 == 7 && juggernaut_is((short)player_index) && (g_4e6948->flags22c & 2))
		result = true;
	else if (engine_options()->value1c4 != 1)
		result = true;
	return result;
}

/* the multiplayer globals' block at +0x68 (8 bytes per element, a tag index at +4) */
struct s_multiplayer_globals_blocks
{
	byte unknown00[0x68];
	long count;
	s_tag_reference *elements;
};

// @retail 0x15eb80
long function_15eb80(long tag_index)
{
	s_multiplayer_globals_blocks *data = (s_multiplayer_globals_blocks *)multiplayer_globals_data();
	long result = NONE;

	if (data->count > 0)
	{
		for (long i = 0; i < data->count; i++)
		{
			if (tag_index == data->elements[i].index)
			{
				result = i;
				break;
			}
		}
	}
	return result;
}

// @retail 0x1595d0
void function_1595d0(void)
{
	if (g_4b9ed8 != NONE)
	{
		long player_index = g_4e8c20->entries[g_4b9ed8];

		if (player_index != NONE)
		{
			c_engine_peer *engine = game_engine_get();

			if (engine)
			{
				engine->p13(player_index);
			}
		}
	}
}

// @retail 0x15ea50
void function_15ea50(long player_index)
{
	c_engine_peer *engine = game_engine_get();

	if (engine && player_index != NONE)
	{
		engine->p14(player_index);
	}
}

/* the weapon definitions (the type at +0x290) */
struct s_engine_weapon_definition
{
	byte unknown00[0x290];
	short value290;
};

// @retail 0x15e8e0
void function_15e8e0(long object_index, long value)
{
	c_engine_peer *engine = game_engine_get();

	if (engine)
	{
		s_engine_object *object = engine_object_get(object_index);

		if ((1 << object->type) & 4)
		{
			if (((s_engine_weapon_definition *)g_4e3b44[object->definition_index & 0xffff].bytes)->value290)
			{
				engine->p17(value, object_index);
			}
			else
			{
				object->value150 = g_510c54->game_time;
			}
		}
	}
}

// @retail 0x15e050
void function_15e050(long object_index, short value)
{
	s_game_engine_globals *globals = function_xaee93d();

	if (g_55e4d0[globals->engine_index])
	{
		s_engine_object *object = engine_object_get(object_index);
		long count = globals->object_count;

		if (count < sizeof(globals->objects) / sizeof(globals->objects[0]))
		{
			s_game_engine_object_entry *entry;

			globals->object_count = count + 1;
			object->flag16c_bit6 = true;
			entry = &globals->objects[count];
			object->value17e = value;
			engine_object_mark_dirty(object_index, 0x1000);
			entry->value04 = value;
			entry->object_index = object_index;
			entry->other_index = NONE;
			entry->value0c = NONE;
			g_55e4d0[globals->engine_index]->p21(object_index);
			if (object->value154 != NONE)
			{
				function_15e250(object_index, object->value154);
			}
		}
	}
}

// @retail 0x15e130
void function_15e130(long object_index)
{
	if (game_engine_get())
	{
		s_engine_object *object = engine_object_get(object_index);
		s_game_engine_object_entry *entry = game_engine_object_entry(object_index);
		long index;

		if (entry->other_index != NONE)
		{
			function_15e460(object_index, object->value154);
		}
		game_engine_get()->p22(object_index);
		object->flag16c_bit6 = false;
		object->value17e = NONE;
		engine_object_mark_dirty(object_index, 0x1000);
		index = entry - function_xaee93d()->objects;
		if (index < function_xaee93d()->object_count - 1)
		{
			function_xaee93d()->objects[index] = function_xaee93d()->objects[function_xaee93d()->object_count - 1];
		}
		function_xaee93d()->object_count--;
	}
}

void __stdcall function_b8540(long a);

// @retail 0x15e4d0
void function_15e4d0(void)
{
	s_game_engine_globals *globals = function_xaee93d();
	long objects[8];
	long count = 0;
	long i;

	for (i = 0; i < globals->object_count; i++)
	{
		objects[count++] = globals->objects[i].object_index;
	}
	for (i = 0; i < count; i++)
	{
		long object_index = objects[i];

		function_15e130(object_index);
		function_b8540(object_index);
	}
}

// @retail 0x15e7a0
void __stdcall function_15e7a0(long object_index)
{
	s_engine_object *object = engine_object_get(object_index);

	if (((1 << object->type) & 4) && TEST_FIELD_BIT(object->flag16c_bit6))
	{
		function_15e130(object_index);
	}
}

/* the object deletion callbacks: each is called with the index of an object
   being deleted */
void __stdcall function_bb880(long object_index);
void __stdcall function_c1670(long object_index);
void __stdcall function_1c9f30(long object_index);
void __stdcall function_152cf0(long object_index);
void __stdcall function_16651c(long object_index);
void __stdcall function_2095e0(long object_index);
void __stdcall function_17b3c0(long object_index);

void (__stdcall *g_468664[])(long object_index) =
{
	function_bb880,
	function_c1670,
	function_1c9f30,
	function_152cf0,
	function_16651c,
	function_15e7a0,
	function_2095e0,
	function_17b3c0,
};

// @retail 0x15ece0
void function_15ece0(void)
{
	if (g_4e6948->mode != 4)
	{
		s_game_time_globals *time = g_510c54;
		s_game_engine_globals *globals = function_xaee93d();

		if (time->game_time >= globals->update_time + time->field_2_3 * 3)
		{
			for (long i = 0; i < 16; i++)
			{
				game_engine_player_mark_dirty((short)i, 0x10);
			}
			globals->update_time = time->game_time;
		}
	}
}

// @retail 0x159460
void function_159460(long player_index, bool *leading, bool *tied)
{
	long absolute_index = player_index & 0xffff;
	s_engine_player *player = (s_engine_player *)g_4e8c24->data + absolute_index;
	bool is_tied = false;
	bool is_leading = true;

	if (function_x340af0())
	{
		long score = function_1587f0(player->team);

		for (long team = 0; team < 8; team++)
		{
			if (team != player->team && game_engine_team_is_active(team))
			{
				long difference = function_1587f0(team) - score;

				if (difference == 0)
				{
					is_tied = true;
				}
				else if (difference > 0)
				{
					is_leading = false;
					break;
				}
			}
		}
	}
	else
	{
		long score = function_1587b0(player_index);
		s_engine_player_iterator iterator;

		iterator.data = g_4e8c24;
		iterator.absolute_index = NONE;
		iterator.index = NONE;
		while (player_iterator_next((long *)&iterator))
		{
			if (iterator.index != player_index)
			{
				long difference = function_1587b0(iterator.index) - score;

				if (difference == 0)
				{
					is_tied = true;
				}
				if (difference > 0)
				{
					is_leading = false;
					break;
				}
			}
		}
	}
	if (is_leading)
	{
		*leading = true;
		*tied = is_tied;
	}
	else
	{
		*leading = false;
		*tied = false;
	}
}

#include "object_iterator.h"

// score 8 (was 8): the inlined equipment lookup keeps the object table base and the index in
// swapped registers.
// @retail 0x15adb0
void function_15adb0(s_netgame_entry_state *entries)
{
	struct { s_engine_object *object; s_type_f1af8e iterator; } local;

	function_15ace0(entries);
	function_bae80(&local.iterator, 0x1e, 0);
	while ((local.object = (s_engine_object *)function_baeb0(&local.iterator)) != 0)
	{
		long index = local.object->netgame_entry;

		if (index >= 0 && index < ((s_scenario_netgame_view *)g_4e0350)->count)
		{
			if (entries[index].index != NONE)
			{
				((c_engine_equipment *)entries)->release_entry(entries[index].index, index);
			}
			entries[index].value04 = 0;
			entries[index].index = local.iterator.object_index;
		}
		else
		{
			local.object->netgame_entry = NONE;
		}
	}
}

bool function_15e020(short a, short b);

// @retail 0x15dec0
real function_15dec0(long player_index, long other_index)
{
	real result = 1.0f;

	if (game_engine_get() && player_index != other_index)
	{
		result = 1.0f;

		if (player_index != NONE)
		{
			switch (engine_player_get(player_index)->value_c1)
			{
			case 0:
				result *= 1.0f;
				break;
			case 1:
				result *= 0.75f;
				break;
			case 2:
				result *= 0.5f;
				break;
			case 3:
				result *= 0.25f;
				break;
			}
			if (other_index != NONE)
			{
				if (function_15ff50(player_index, 2))
				{
					result *= 1.5f;
				}
				if (function_15ff50(other_index, 3))
				{
					result *= 0.5f;
				}
				if (engine_player_get(other_index)->value194 > 0)
				{
					result *= 0.1f;
				}
				if (!TEST_FIELD_BIT(engine_options()->flags_bit7) && !function_15e020(engine_player_get(player_index)->team, engine_player_get(other_index)->team))
				{
					result = 0.0f;
				}
			}
		}
	}
	return result;
}

// @retail 0x15d6c0
void function_15d6c0(long player_index, long maximum, long minimum)
{
	s_engine_player *player = engine_player_get(player_index);
	s_engine_player_iterator iterator;

	iterator.data = g_4e8c24;
	iterator.absolute_index = NONE;
	iterator.index = NONE;
	while (player_iterator_next((long *)&iterator))
	{
		s_engine_player *other = iterator.player;

		if (other->value164 > 0 &&
			!game_engine_get()->p27(other->team, player->team) &&
			other->value164 >= minimum && other->value164 < maximum)
		{
			maximum = other->value164;
		}
	}
	player->value164 = maximum;
}

// @retail 0x15a090
long function_15a090(long type, long count, short const *types)
{
	long const *count_reference = &count;
	long result = NONE;
	long i;

	if (game_engine_get())
	{
		for (i = 0; i < *count_reference; i++)
		{
			short entry = types[i];
			bool match = entry == type;

			if ((type == 9 || type == 1) && (entry == 9 || entry == 1))
			{
				match = true;
			}
			if (entry == 12)
			{
				match |= true;
			}
			else if (entry == 13)
			{
				match |= type != 1 && type != 9;
			}
			else if (entry == 14)
			{
				match |= type != 1 && type != 9;
			}
			if (match)
			{
				result = i;
				break;
			}
		}
	}
	else
	{
		for (i = 0; i < *count_reference; i++)
		{
			if (types[i] == 0)
			{
				result = i;
				break;
			}
		}
	}
	return result;
}

// @retail 0x15ad30
void function_15ad30(s_netgame_entry_state *entries)
{
	if (g_4e6948->mode != 4)
	{
		s_scenario_netgame_view *scenario = (s_scenario_netgame_view *)g_4e0350;

		if (TEST_FIELD_BIT(engine_options()->flags_bit3) || function_xaee93d()->value6e == 0)
		{
			for (short i = 0; i < scenario->count; i++)
			{
				if (entries[i].index != NONE)
				{
					function_b8540(entries[i].index);
					entries[i].index = NONE;
				}
				entries[i].value04 = 1;
				entries[i].value06 = 0;
			}
		}
	}
}

void __stdcall function_15b650(long team, long delta);
void function_15b3a0(long player_or_team, bool value);

static __forceinline bool score_teams_enabled()
{
	bool result = false;

	if (game_engine_get())
	{
		result = TEST_FIELD_BIT(g_4e6948->flags184.bit0);
	}
	return result;
}

static __forceinline void add_clamped_score(short *score, short delta)
{
	long value = delta;
	value += *score;
	if (value < -30000)
		value = -30000;
	else if (value > 30000)
		value = 30000;
	*score = (short)value;
}

// score 5 (was 5): the two operands of the clamped score are sign extended into swapped registers,
// and the peer lookup keeps them in different ones.
// @retail 0x15b7c0
bool function_15b7c0(long delta, long player_index)
{
	bool result = false;
	s_game_engine_globals *globals = function_xaee93d();

	if (game_engine_get() && globals->value6c == 1 &&
		(g_4e6948->mode == 4 || globals->value_c04 == 1) && player_index != NONE)
	{
		long index = player_index & 0xffff;
		s_engine_player *player = &((s_engine_player *)g_4e8c24->data)[index];
		add_clamped_score(&globals->statborg.players[index].score, (short)delta);
		if (game_engine_get() && globals->index28 != NONE)
		{
			function_b58c0(globals->index28, 1 << index);
		}
		result = true;
		if (score_teams_enabled() && player->team != NONE)
		{
			function_15b650(player->team, delta);
		}
		else if (!score_teams_enabled() && delta > 0 && g_4e6948->score_to_win &&
			globals->statborg.players[player_index & 0xffff].score >= g_4e6948->score_to_win)
		{
			function_15b3a0(player_index, 0);
		}
		function_xaee93d()->flags |= 0x20;
	}
	return result;
}

/* the unit fields used by a player's timed effects */
struct s_engine_unit_effect
{
	byte unknown00[0x134];
	union
	{
		dword flags134;
		struct
		{
			dword unknown134 : 3;
			dword flag3 : 1;
			dword flag4 : 1;
			dword : 27;
		};
	};
	byte unknown138[0x2b0 - 0x138];
	real value2b0;
	real value2b4;
	real value2b8;
};

static __forceinline s_engine_object *effect_object_get(long object_index)
{
	s_engine_object_header *headers = (s_engine_object_header *)g_4e0300->data;
	long index = object_index & 0xffff;
	return (headers + index)->object;
}

// @retail 0x152240
void function_152240(long player_index, short effect)
{
	short const *effect_reference = &effect;

	if (*effect_reference == 0)
	{
		long unit_index = engine_player_get(player_index)->unit_index;
		s_engine_unit_effect *unit = (s_engine_unit_effect *)effect_object_get(unit_index);

		unit->flag3 = true;
		unit->value2b8 = 0.25f;
		s_engine_object *object = effect_object_get(unit_index);
		if (object->simulation_index != NONE)
		{
			function_b58c0(object->simulation_index, 0x800000);
		}
	}
}

// @retail 0x1522b0
void function_1522b0(long player_index, short effect)
{
	if (effect == 0)
	{
		long unit_index = engine_player_get(player_index)->unit_index;
		s_engine_unit_effect *unit = (s_engine_unit_effect *)effect_object_get(unit_index);
		/* the same unit, looked up again for the second flag */
		s_engine_unit_effect *same_unit = (s_engine_unit_effect *)effect_object_get(unit_index);

		unit->flag3 = true;
		same_unit->flag4 = true;
		unit->value2b8 = 0.25f;
		s_engine_object *object = effect_object_get(unit_index);
		if (object->simulation_index != NONE)
		{
			function_b58c0(object->simulation_index, 0x800000);
		}
	}
}

void function_a7a30(long object_index, dword mask);

// @retail 0x15fe70
void function_15fe70(long player_index)
{
	if (player_index != NONE)
	{
		s_engine_player *player = engine_player_get(player_index);

		if (player->unit_index != NONE)
		{
			s_engine_unit_effect *unit = (s_engine_unit_effect *)engine_object_get(player->unit_index);

			if (TEST_FIELD_BIT(unit->flag3))
			{
				unit->value2b0 = unit->value2b0 > 0.5f ? 0.5f : unit->value2b0;
				function_a7a30(player->unit_index, 0x800000);
			}
		}
	}
}

void game_finish_countdown_start(void);

// @retail 0x15ba00
void function_15ba00(void)
{
	if (!g_4e6948->flag1128 && g_4e6948->mode != 4)
	{
		game_finish_countdown_start();
	}
}

void game_engine_format_time(long seconds, word *text);
word *function_1630e0(word *buffer, word const *format, ...);

__declspec(noinline) void function_159130(long value, word *text);

// @retail 0x159130
void function_159130(long value, word *text)
{
	switch (g_4e6948->mode_180)
	{
	case 3:
	case 4:
	case 8:
		game_engine_format_time(value, text);
		break;
	default:
		function_1630e0(text, (word const *)L"%d", value);
		break;
	}
}

long function_1ded60(void);
void function_1dedb0(long list_index, long object_index);

// @retail 0x15e730
long function_15e730(void)
{
	long list_index = function_1ded60();

	if (list_index != NONE)
	{
		s_game_engine_globals *globals = function_xaee93d();

		if (game_engine_get())
		{
			for (long i = 0; i < globals->object_count; i++)
			{
				function_1dedb0(list_index, globals->objects[i].object_index);
			}
		}
	}
	return list_index;
}

// @retail 0x15b980
void function_15b980(bool skip_event)
{
	bool const *skip_reference = &skip_event;
	s_event event;

	if (g_4e6948->mode != 4)
	{
		function_xaee93d()->value_c04 = 2;
		if (g_4e6948->mode != 4 && !g_4e6948->flag1128 && !*skip_reference)
		{
			game_engine_event_initialize_inline(&event, 0, 0x1f);
			function_a7c50(&event);
			function_19eb30(&event);
		}
	}
}

void function_1e9df0(long field, long counter, long team, s_statborg *statistics, short delta);
void function_1e9ce0(long player_index, long counter, s_statborg *statistics, long field, long delta, bool by_team);

// @retail 0x15b650
void __stdcall function_15b650(long team, long delta)
{
	s_statborg *statistics = game_engine_statborg_inline();
	long mode = *(long *)((byte *)g_4e6948 + 0x1e0);
	long old_score = statistics->teams[team].score;
	long score = (long)0x80000000;
	if (mode == 0)
		score = old_score + delta;
	else
	{
		s_engine_player_iterator iterator;
		iterator.data = g_4e8c24;
		iterator.absolute_index = NONE;
		iterator.index = NONE;
		while (player_iterator_next((long *)&iterator))
		{
			if (iterator.player->team == team)
			{
				long current = statistics->players[iterator.index & 0xffff].score;
				if (score == (long)0x80000000) score = current;
				else switch (*(long *)((byte *)g_4e6948 + 0x1e0))
				{
				case 1: if (score > current) score = current; break;
				case 2: if (score <= current) score = current; break;
				}
			}
		}
		if (score <= old_score) return;
	}
	if (score != old_score)
	{
		function_1e9df0(0, NONE, team, statistics, score - old_score);
		switch (g_4e6948->mode_180)
		{
		case 1: function_1967d0(NONE, team, 0xe, score - old_score); break;
		case 3: function_1967d0(NONE, team, 0x17, score - old_score); break;
		case 9: function_1967d0(NONE, team, 0x12, score - old_score); break;
		}
		if (score > old_score && g_4e6948->score_to_win && score >= g_4e6948->score_to_win)
			function_15b3a0(team, false);
	}
}

struct s_round_limit_options
{
	byte unknown00[0x188];
	long limit_kind;
};

/* 0x1968b0's index half (the team half is dead here), clamped with the
   ternary the out-of-line function uses */
static __forceinline void input_counter_set_inlined_15b3a0(long a, long b, long value)
{
	if (g_510ca0 && !g_510ca1)
	{
		long minimum = g_46ddc0[b].minimum;
		long maximum = g_46ddc0[b].maximum;
		if (a != NONE)
		{
			s_input_counter *counter = &g_511bf4.all[a * 0x1b5 + b];
			counter->value = value < minimum ? minimum : (value > maximum ? maximum : value);
		}
	}
}

// @retail 0x15b3a0
void function_15b3a0(long player_or_team, bool value)
{
	s_game_engine_globals *globals = function_xaee93d();
	long wins = 0;

	if (player_or_team != NONE)
	{
		if (score_teams_enabled())
		{
			function_1e9df0(7, 6, player_or_team, &globals->statborg, 1);
			s_engine_player_iterator iterator;

			iterator.data = g_4e8c24;
			iterator.absolute_index = NONE;
			iterator.index = NONE;
			while (player_iterator_next((long *)&iterator))
			{
				if (iterator.player->team == player_or_team)
				{
					s_statborg *statistics = game_engine_statborg_inline();
					long index = iterator.index & 0xffff;
					short *score = &((short *)&statistics->players[index])[7];

					add_clamped_score(score, 1);
					if (game_engine_get() && globals->index28 != NONE)
					{
						function_b58c0(globals->index28, 1 << index);
					}
					input_counter_set_inlined_15b3a0(index, 6, *score);
				}
			}
			wins = ((short *)&game_engine_statborg_inline()->teams[player_or_team])[7];
		}
		else if (record_pool_lookup(g_4e8c24, player_or_team))
		{
			function_1e9ce0(player_or_team, 6, game_engine_get_statborg(), 7, 1, false);
			wins = ((short *)&game_engine_get_statborg()->players[player_or_team & 0xffff])[7];
		}
	}

	long limit_kind = ((s_round_limit_options *)g_4e6948)->limit_kind;
	if ((limit_kind == 0 && (short)globals->value6e >= 0) ||
		(limit_kind == 1 && (short)globals->value6e >= 1) ||
		(limit_kind == 2 && (short)globals->value6e >= 3) ||
		(limit_kind == 3 && (short)globals->value6e >= 5) ||
		(limit_kind == 4 && player_or_team != NONE && wins >= 2) ||
		(limit_kind == 5 && player_or_team != NONE && wins >= 3) ||
		(limit_kind == 6 && player_or_team != NONE && wins >= 4))
	{
		value = true;
	}
	bool finished = (short)globals->value6e >= 31 || value;
	if (finished)
	{
		if (g_4e6948->mode != 4)
		{
			globals->value_c04 = 2;
		}
		if (!g_4e6948->flag1128 && g_4e6948->mode != 4)
		{
			game_finish_countdown_start();
		}
	}
	else
	{
		function_15b980(false);
	}
	function_xaee93d()->flags |= 0x20;
}

void unicode_string_snprintf(word *buffer, long maximum_count, word const *format, ...);

// @retail 0x15ea80
void function_15ea80(long code, word *buffer, long maximum_count)
{
	word text[0x100];

	if (code != 0xe423)
	{
		unicode_string_snprintf(buffer, maximum_count, (word const *)L"invalid game engine character");
	}
	else
	{
		short remaining = function_xaee93d()->value_e0;
		long seconds = 0;

		text[0] = 0;
		if (remaining > 0)
		{
			seconds = remaining;
		}
		game_engine_format_time(seconds, text);
		unicode_string_snprintf(buffer, maximum_count, (word const *)L"%s", text);
	}
}

bool player_iterator_next_with_unit(long *iterator);

static __forceinline real engine_distance_squared(point3f const *a, point3f const *b)
{
	real x = a->x - b->x;
	real y = a->y - b->y;
	real z = a->z - b->z;
	real result = z * z;
	result += x * x;
	result += y * y;
	return result;
}

// @retail 0x15f060
bool function_15f060(point3f const *position, real radius)
{
	s_engine_player_iterator iterator;

	radius *= radius;
	bool result = false;
	iterator.data = g_4e8c24;
	iterator.absolute_index = NONE;
	iterator.index = NONE;
	while (player_iterator_next_with_unit((long *)&iterator))
	{
		s_engine_object *object = engine_object_get(iterator.player->unit_index);

		result = engine_distance_squared(position, &object->position) < radius;
		if (result)
		{
			break;
		}
	}
	return result;
}

struct s_object_values;
void function_1e97b0(s_object_values *table);
bool function_15b2f0(void);

// @retail 0x15dd10
void function_15dd10(void)
{
	s_game_engine_globals *globals = function_xaee93d();
	s_statborg *statistics = game_engine_statborg_inline();
	if (statistics)
		function_1e97b0((s_object_values *)statistics);
	globals->flags |= 0x20;
}

void function_1520a0(long player_index, short effect, short ticks);

// @retail 0x15b270
void function_15b270(long player_index)
{
	c_engine_peer *engine = game_engine_get();
	if (engine && engine->p35(player_index, 1) && engine_player_get(player_index)->unit_index != NONE)
	{
		real ticks = g_510c54->field_2_3 * 0.5f;
		long rounded_ticks;
		__asm
		{
			fld ticks
			fistp rounded_ticks
		}
		function_1520a0(player_index, 0, (short)rounded_ticks);
	}
}

// @retail 0x15ba90
void function_15ba90(long team)
{
	if (g_4e6948->mode != 4 && function_15b2f0() && g_510c54->game_time % g_510c54->field_2_3 == 0)
	{
		s_engine_player_iterator iterator;
		iterator.data = g_4e8c24;
		iterator.absolute_index = NONE;
		iterator.index = NONE;
		while (player_iterator_next((long *)&iterator))
		{
			if (iterator.player->team == team && iterator.player->unit_index != NONE)
			{
				function_1967d0(NONE, team, 13, 1);
				break;
			}
		}
	}
}

struct s_damage_owner;
void function_dbfb0(long object_index, s_damage_owner const *owner, bool a, bool b, bool c);
void function_14cad0(long player_index, long unit_index);

// @retail 0x158090
void function_158090(long player_index, short old_team)
{
	long const *player_reference = &player_index;
	short const *team_reference = &old_team;
	if (function_x340af0() && g_4e6948->mode != 4)
	{
		s_engine_player *player = engine_player_get(*player_reference);
		if (*team_reference != player->team && player->unit_index != NONE)
		{
			function_dbfb0(player->unit_index, 0, false, true, true);
			if (player->unit_index != NONE)
			{
				function_14cad0(*player_reference, NONE);
				player->value164 = 2 * g_510c54->field_2_3;
			}
		}
	}
}

// score 12 (was 12): the player index and the player table base are kept in swapped registers, and
// the peer lookup after it reloads them the other way round.
// @retail 0x157de0
void function_157de0(long player_index)
{
	if (game_engine_get())
	{
		if (g_4e6948->mode != 4 && function_x340af0())
			function_158140();
		game_engine_get()->p7(player_index);
		s_engine_player *players = (s_engine_player *)g_4e8c24->data;
		s_event event;
		game_engine_event_initialize_inline(&event, 0, 11);
		event.cause_player_index = player_index;
		event.cause_team = players[player_index & 0xffff].team;
		function_19eb30(&event);
		function_196470();
		game_engine_player_mark_dirty((short)player_index, 0x200);
	}
}

void function_1e9c90(long object_index, long column, s_object_values *table);

// @retail 0x1582d0
void function_1582d0(long player_index, short old_team)
{
	short const *old_team_reference = &old_team;
	if (function_x340af0())
	{
		s_statborg *statistics = game_engine_statborg_inline();
		if (g_4e6948->mode != 4)
			function_1e9c90(player_index, 0, (s_object_values *)statistics);
		game_engine_get()->p10(player_index);
		if (g_4e6948->mode != 4)
			function_158140();
		if (g_510c54->game_time > 1)
		{
			s_engine_player *player = engine_player_get(player_index);
			long previous_team = *old_team_reference;
			s_event event;
			game_engine_event_initialize_inline(&event, 0, 23);
			event.cause_player_index = player_index;
			event.cause_team = player->team;
			event.effect_team = previous_team;
			game_engine_event_send_inline(&event);
		}
	}
}

struct s_engine_health_object
{
	byte unknown00[0xf0];
	real shield;
	byte unknownf4[0x104 - 0xf4];
	short update_ticks;
};

void function_a7a30(long object_index, dword flags);

// @retail 0x1583d0
void function_1583d0(short old_value, short new_value, long player_index)
{
	if (game_engine_get() && g_4e6948->mode != 4 && old_value != new_value)
	{
		if (old_value > new_value)
		{
			s_engine_player *player = engine_player_get(player_index);
			if (player->unit_index != NONE)
			{
				s_engine_health_object *object = (s_engine_health_object *)engine_object_get(player->unit_index);
				if (object->update_ticks == 0)
					object->update_ticks = 1;
			}
		}
		else
		{
			s_engine_player *player = engine_player_get(player_index);
			if (player->unit_index != NONE)
			{
				s_engine_health_object *object = (s_engine_health_object *)engine_object_get(player->unit_index);
				real maximum = function_1588b0(player_index, 2);
				if (object->shield >= maximum)
				{
					object->shield = maximum;
					function_a7a30(player->unit_index, 0x80);
				}
			}
		}
	}
}

struct s_engine_seated_unit
{
	byte unknown00[0x13c];
	long player_index;
	byte unknown140[0x1fc - 0x140];
	short seat;
};

struct s_engine_seat_entry
{
	dword flags;
	byte unknown04[0xb0 - 4];
};

struct s_engine_seat_definition
{
	byte unknown00[0x1cc];
	s_engine_seat_entry *seats;
};

void function_1970a0(long player_index, long counter, long delta);

// @retail 0x15e7f0
void function_15e7f0(long unit_index, long vehicle_index)
{
	s_engine_seated_unit *unit = (s_engine_seated_unit *)engine_object_get(unit_index);
	if (g_4e6948->state == 2 && g_4e6948->mode != 4 && unit->player_index != NONE && unit->seat != NONE)
	{
		long definition_index = engine_object_get(vehicle_index)->definition_index;
		s_engine_seat_definition *definition = (s_engine_seat_definition *)g_4e3b44[definition_index & 0xffff].bytes;
		if ((bool)((definition->seats[unit->seat].flags >> 11) & 1))
		{
			s_event event;
			game_engine_event_initialize_inline(&event, 0, 44);
			game_engine_event_set_cause_player(&event, unit->player_index);
			function_19eb90(&event);
			function_1970a0(unit->player_index & 0xffff, 11, 1);
		}
	}
}

bool function_15eaf0(void);

struct s_engine_winner_view
{
	byte unknown00[0x2f0];
	long team;
};

struct s_engine_winner_options
{
	byte unknown00[0x240];
	long value240;
};

// @retail 0x158d20
bool function_158d20(long *winner, bool *few_teams)
{
	bool result = false;
	bool teams_result = false;
	bool *const *few_teams_reference = &few_teams;
	s_game_engine_globals *globals = function_xaee93d();
	if (game_engine_get())
	{
		long player_index = NONE;
		long active_count;
		long team_count;
		long total = (short)function_158ab0();
		function_158c00(&player_index, &active_count, &team_count);
		if (total > 1)
		{
			result = active_count < 2;
			teams_result = team_count < 2;
		}
		else
		{
			result = active_count <= 0;
			teams_result = team_count <= 0;
		}
		if (result && active_count == 1 && player_index != NONE)
		{
			if (function_15eaf0())
			{
				*winner = engine_player_get(player_index)->team;
				if (!teams_result && (g_4e6948->mode_180 == 1 || g_4e6948->mode_180 == 9) &&
					((s_engine_winner_options *)g_4e6948)->value240 == 1 &&
					((s_engine_winner_view *)globals)->team == *winner)
					*winner = NONE;
			}
			else
				*winner = player_index;
		}
	}
	if (*few_teams_reference)
		**few_teams_reference = teams_result;
	return result;
}

struct s_engine_round_clock
{
	byte unknown00[0x70];
	long start_time;
};

void __stdcall function_a7840(short player_index, dword mask);

// @retail 0x15dd40
bool function_15dd40(long player_index)
{
	bool result = true;
	if (game_engine_get())
	{
		s_engine_player *player = engine_player_get(player_index);
		if (function_158a50(player_index) && function_15b2f0())
		{
			if (player->value164 > 0)
			{
				long ticks = g_510c54->field_2_3;
				if (player->value164 == ticks * 3 || player->value164 == ticks * 2 || player->value164 == ticks)
				{
					s_event event;
					game_engine_event_initialize_inline(&event, 0, 20);
					event.cause_player_index = player_index;
					event.a = player_index;
					function_19eb90(&event);
				}
				else if (player->value164 == 1)
				{
					s_event event;
					game_engine_event_initialize_inline(&event, 0, 21);
					event.cause_player_index = player_index;
					event.a = player_index;
					function_19eb90(&event);
				}
				player->value164--;
				player->value170 = player->value164 / g_510c54->field_2_3;
				player->value170 = player->value170 < 0 ? 0 : player->value170 > 0x3ff ? 0x3ff : player->value170;
				function_a7840((short)player_index, 0x80);
			}
			result = player->value164 == 0;
			if (result)
			{
				long elapsed = g_510c54->game_time - ((s_engine_round_clock *)g_4e9ae8)->start_time;
				if (elapsed >= 0 && elapsed > 3 && elapsed % 32 != (player_index & 0xffff) % 32)
					result = false;
			}
		}
		else
			result = false;
	}
	return result;
}

/* the test of function_15b2f0, which 0x15ba20 inlines */
struct s_engine_running_state
{
	byte unknown00[0x6c];
	short w6c;
	byte unknown6e[0xc04 - 0x6e];
	long lc04;
	byte unknownc08[0xc14 - 0xc08];
	long engine_index;
};

static inline bool engine_running_inline()
{
	s_engine_running_state *g = (s_engine_running_state *)g_4e9ae8;
	bool mode_gate_open = false;

	if (g_55e4d0[g->engine_index] && g->w6c == 1 && (g_4e6948->mode == 4 || g->lc04 == 1))
		mode_gate_open = true;

	return mode_gate_open;
}

// @retail 0x15ba20
void function_15ba20(void)
{
	long winner = NONE;
	bool few_teams = false;
	if (engine_running_inline() && function_158d20(&winner, &few_teams))
		function_15b3a0(winner, few_teams);
}

// score 16 (was 16): the event fields are stored in a different order than retail, and the player
// lookup keeps the index in a different register.
// @retail 0x157ed0
void function_157ed0(long player_index)
{
	if (game_engine_get())
	{
		long index = player_index & 0xffff;
		if (g_4e6948->mode != 4 && function_x340af0())
			function_158140();
		game_engine_get()->p8(player_index);
		s_event event;
		game_engine_event_initialize_inline(&event, 0, 24);
		event.cause_player_index = player_index;
		event.cause_team = ((s_engine_player *)g_4e8c24->data)[index].team;
		game_engine_event_send_inline(&event);
		function_196470();
		if (g_510ca0 && !g_510ca1)
		{
			long minimum = g_46ddc0[0].minimum;
			long maximum = g_46ddc0[0].maximum;
			if (index != NONE)
			{
				s_input_counter *counter = &g_511bf4.all[index * 0x1b5];
				counter->value = 1 < minimum ? minimum : 1 > maximum ? maximum : 1;
			}
		}
	}
}

struct s_engine_scaled_player
{
	byte unknown00[0x19c];
	real scale;
};

struct s_engine_dirty_slot
{
	byte unknown00[0xc];
	dword flags;
	byte unknown10[0x10];
};

struct s_engine_dirty_state
{
	byte unknown00[0x44];
	struct { byte flags; byte unknown01[7]; } entries[1024];
};

struct s_engine_dirty_pool
{
	byte unknown00[0xc];
	s_engine_dirty_state *state;
	byte unknown10[4];
	s_engine_dirty_slot slots[1024];
};

struct s_engine_dirty_data
{
	byte unknown00[0x2098];
	s_engine_dirty_pool pool;
};

struct s_engine_dirty_world
{
	byte unknown00[4];
	s_engine_dirty_data *data;
	long state;
};

/* The entity dirty update is inlined in the player scale update. */
static __forceinline void engine_player_scale_mark_dirty(short index)
{
	if (game_engine_get())
	{
		long identifier = function_xaee93d()->slots[index];
		if (identifier != NONE)
		{
			s_engine_dirty_world *world = (s_engine_dirty_world *)g_4cf77c;
			long state = world->state;
			if ((state == 4 || state == 5) && state != 3 && state != 5)
			{
				s_engine_dirty_pool *pool = &world->data->pool;
				if (pool->state->entries[identifier & 0x3ff].flags & 4)
				{
					s_engine_dirty_slot *slot = &pool->slots[identifier & 0x3ff];
					slot->flags |= 2;
				}
			}
		}
	}
}

// @retail 0x15c000
void function_15c000(void)
{
	s_engine_player_iterator iterator;
	iterator.data = g_4e8c24;
	iterator.absolute_index = NONE;
	iterator.index = NONE;
	while (player_iterator_next_with_unit((long *)&iterator))
	{
		s_engine_scaled_player *player = (s_engine_scaled_player *)iterator.player;
		real current = player->scale;
		real target = 1.0f;
		c_engine_peer *engine = game_engine_get();
		if (engine)
			target = engine->p19(iterator.index);
		if (current != target)
		{
			long index = iterator.index & 0xffff;
			if (current > target)
				current = target;
			else
			{
				current += 0.15f / g_510c54->field_2_3;
				current = current < 0.0f ? 0.0f : current > target ? target : current;
			}
			player->scale = current;
			engine_player_scale_mark_dirty((short)index);
		}
	}
}

void function_1628f0(long player_index, char state);

// @retail 0x15cbf0
void function_15cbf0(long attacker_index, long player_index, long flags)
{
	long const *flags_reference = &flags;
	if (!datum_get_inlined(g_4e8c24, attacker_index))
		attacker_index = NONE;
	if (!datum_get_inlined(g_4e8c24, player_index))
		player_index = NONE;
	if (player_index != NONE)
		function_1628f0(player_index, 3);
	c_engine_peer *engine = game_engine_get();
	if (engine && player_index != NONE)
		engine->p29(attacker_index, player_index, *flags_reference);
}

struct s_engine_border_state
{
	byte unknown00[0x90];
	s_short_rectangle bounds;
	byte unknown98[0xb4 - 0x98];
	color4f color;
	byte unknownc4[0x138 - 0xc4];
	bool flag138;
	byte unknown139[0x180 - 0x139];
	bool flag180;
	byte unknown181[0x1c0 - 0x181];
	bool flag1c0;
};

void function_36880(color4f const *color, s_short_rectangle const *rectangle);

// score 4 (was 4): the store of the border bottom is scheduled after the call arguments are set up
// instead of before them.
// @retail 0x15ff80
void function_15ff80(s_engine_border_state const *state)
{
	s_short_rectangle bounds = state->bounds;
	color4f color = state->color;
	color.alpha = (((real)sin((real)g_510c54->game_time * 3.1415927410125732f / g_510c54->field_2_3) + 1.0f) * 0.5f) * color.alpha;
	bounds.left -= 45;
	bounds.right += (state->flag1c0 ? 7 : 0) + (state->flag180 ? 54 : 0) + (state->flag138 ? 50 : 0);
	s_short_rectangle border;
	border.top = bounds.top - 1;
	border.left = bounds.left;
	border.right = bounds.right;
	border.bottom = bounds.top;
	function_36880(&color, &border);
	border.right = bounds.right + 1;
	short bottom = bounds.bottom + 1;
	border.left = bounds.right;
	border.top = bounds.top - 1;
	border.bottom = bottom;
	function_36880(&color, &border);
	border.left = bounds.left;
	border.top = bounds.bottom;
	border.right = bounds.right;
	border.bottom = bottom;
	function_36880(&color, &border);
	border.left = bounds.left - 1;
	border.top = bounds.top - 1;
	border.right = bounds.left;
	border.bottom = bottom;
	function_36880(&color, &border);
}

bool function_15d770(long player_index);
bool function_15d920(long player_index);

// @retail 0x15ed60
void function_15ed60(void)
{
	if (g_4e6948->mode != 4)
	{
		s_engine_player_iterator iterator;
		iterator.data = g_4e8c24;
		iterator.absolute_index = NONE;
		iterator.index = NONE;
		while (player_iterator_next((long *)&iterator))
		{
			long player_index = iterator.index;
			bool blocked = false;
			if (function_15d770(player_index) || function_15d920(player_index))
				blocked = true;
			s_engine_player *player = iterator.player;
			if (blocked != (bool)((player->flags >> 14) & 1))
			{
				if (blocked)
					player->flags |= 0x4000;
				else
					player->flags &= ~0x4000;
				game_engine_player_mark_dirty((short)player_index, 0x400);
			}
		}
	}
}

struct s_engine_player_fade
{
	byte unknown00[0x2c];
	long unit_index;
	byte unknown30[0x1a0 - 0x30];
	long target_index;
	byte unknown1a4[2];
	char shared_time;
	char death_time;
	byte unknown1a8[2];
	short target_time;
};

struct s_engine_unit_fade
{
	byte unknown00[0x2b0];
	real amount;
};

long function_baf80(long object_index);

// @retail 0x159690
real function_159690(long player_index, long target_index)
{
	s_engine_player_fade *player = (s_engine_player_fade *)engine_player_get(player_index);
	s_engine_player_fade *target = (s_engine_player_fade *)engine_player_get(target_index);
	real result = 0.0f;
	long target_unit = target->unit_index;
	if (target->unit_index != NONE)
	{
		long unit_index = player->unit_index;
		if (unit_index == NONE && player->death_time != NONE)
		{
			real amount = 1.0f - (real)player->death_time / g_510c54->field_2_3;
			amount = amount < 0.0f ? 0.0f : amount > 1.0f ? 1.0f : amount;
			result = powf(amount, 1.9f);
		}
		else
		{
			if (player->target_index == target_index)
			{
				long ticks = player->target_time < 256 ? player->target_time : 256;
				result = powf(ticks * 0.00390625f, 1.9f);
			}
			char shared_time = player->shared_time;
			if (shared_time != NONE && unit_index != NONE && function_baf80(unit_index) == function_baf80(target_unit))
			{
				real amount = (real)shared_time / g_510c54->field_2_3;
				amount = amount < 0.0f ? 0.0f : amount > 1.0f ? 1.0f : amount;
				result = powf(amount, 1.9f);
			}
		}
		if (result > 0.0f)
		{
			s_engine_unit_fade *unit = (s_engine_unit_fade *)engine_object_get(target_unit);
			real amount = unit->amount < 0.0f ? 0.0f : unit->amount > 1.0f ? 1.0f : unit->amount;
			result *= 1.0f - amount;
		}
	}
	return result;
}

void function_1e9f40(long player_index, s_object_values *table);

// score 2 (was 2): the two global reloads after the event call are in the opposite order: retail
// loads the engine globals before the game state.
// @retail 0x157be0
void function_157be0(dword player_index)
{
	if (game_engine_get())
	{
		long index = player_index & 0xffff;
		if (g_4e6948->mode != 4 && function_x340af0())
			function_158140();
		game_engine_get()->p5(player_index);
		s_statborg *statistics = game_engine_statborg_inline();
		if (g_4e6948->mode != 4)
			function_1e9f40(player_index, (s_object_values *)statistics);
		if ((real)g_510c54->game_time * g_510c54->rate > 1.0f)
		{
			s_event event;
			game_engine_event_initialize_inline(&event, 0, 12);
			event.cause_player_index = player_index;
			event.cause_team = engine_player_get(player_index)->team;
			function_19eb30(&event);
		}
		if (g_4e6948->mode != 4)
		{
			s_engine_player *player = engine_player_get(player_index);
			long lives = g_4e6948->value1b8;
			player->value1ac = (short)(lives ? lives : NONE);
			game_engine_player_mark_dirty((short)index, 0x20);
		}
		s_engine_player *player = engine_player_get(player_index);
		if (player->local_index != NONE)
		{
			function_xaee93d()->timers[player->local_index] = 1.0f;
			function_xaee93d()->timer_flags |= 1 << player->local_index;
		}
		function_196470();
		if (g_510ca0 && !g_510ca1)
		{
			long minimum = g_46ddc0[0].minimum;
			long maximum = g_46ddc0[0].maximum;
			if (index != NONE)
			{
				s_input_counter *counter = &g_511bf4.all[index * 0x1b5];
				counter->value = 1 < minimum ? minimum : 1 > maximum ? maximum : 1;
			}
		}
	}
}

struct s_engine_notice_definition
{
	byte unknown00[4];
	short type;
	byte unknown06[2];
	long text;
	long team_text;
	byte unknown10[0x1c - 0x10];
};

struct s_engine_notice_settings
{
	byte unknown00[0x8c];
	long string_list;
	byte unknown90[0x538 - 0x90];
	long notice_count;
	s_engine_notice_definition *notices;
};

struct s_engine_notice_root
{
	byte unknown00[0xc];
	s_engine_notice_settings *settings;
};

struct s_engine_local_notice
{
	long type;
	long unknown04;
	long definition_index;
};

void function_161ef0(long string_handle, word *buffer);
void function_1a0180(long tag_index, long string_handle, word *buffer);
void function_19e0f0(word const *string, long size, word *buffer, s_event *event);

int unicode_string_vsnprintf(word *buffer, long maximum_count, const word *format, ...);

// score 148 (was 214): setting result on each success instead of returning
// gives retail's convention (buffer arrives in esi; ret 8) and its two
// buffer-clearing exits. What still differs: retail keeps a separate
// formatting call, return and epilogue in each of the three message branches;
// ours shares one tail between them (453 bytes against 516), and the
// registers and order of the early loads differ. Returning true directly in
// those branches, one text buffer per branch, a const reference to the
// buffer and a reordered parameter list score 148 to 218; the 600 s
// permutation runs found nothing that keeps the same calls.
// @retail 0x15f120
bool function_15f120(long player_index, word *buffer, long size, long suppress_score)
{
	long user_index = engine_player_get(player_index)->local_index;
	long const *size_reference = &size;
	long const *suppress_reference = &suppress_score;
	bool result = false;
	if (game_engine_get())
	{
		word text[256];
		short state = function_xaee93d()->value6c;
		if (state == 1)
		{
			if (g_4e9af0.players[user_index].index != NONE && g_4e9af0.players[user_index].value08 != NONE)
			{
				s_engine_notice_settings *settings = ((s_engine_notice_root *)g_4e3b44[g_4e034c->index & 0xffff].bytes)->settings;
				s_engine_notice_definition *definition = &settings->notices[g_4e9af0.players[user_index].value08];
				long string_handle = function_15eaf0() ? definition->team_text : definition->text;
				if ((!(byte)*suppress_reference || (g_4e9af0.players[user_index].index != 25 && g_4e9af0.players[user_index].index != 5 && g_4e9af0.players[user_index].index != 26 &&
					g_4e9af0.players[user_index].index != 6 && g_4e9af0.players[user_index].index != 27 && g_4e9af0.players[user_index].index != 7)) && string_handle != NONE && string_handle != 0)
				{
					text[0] = 0;
					function_1a0180(settings->string_list, string_handle, text);
					function_19e0f0(text, *size_reference, buffer, 0);
					result = true;
				}
				else
				{
					buffer[0] = 0;
					return false;
				}
			}
			else
			{
				buffer[0] = 0;
			}
		}
		else if (g_4e6948->flag1128)
		{
			text[0] = 0;
			function_161ef0(0x0c000108, text);
			unicode_string_vsnprintf(buffer, *size_reference, text);
			result = true;
		}
		else if (state == 2)
		{
			text[0] = 0;
			function_161ef0(0x0d000109, text);
			unicode_string_vsnprintf(buffer, *size_reference, text);
			result = true;
		}
		else if (state == 3)
		{
			text[0] = 0;
			function_161ef0(0x1900010a, text);
			unicode_string_vsnprintf(buffer, *size_reference, text);
			result = true;
		}
	}
	return result;
}

bool function_15db30(long player_index);

// @retail 0x15ee30
long function_15ee30(long player_index)
{
	long const *player_reference = &player_index;
	struct { byte padding[2]; bool override_status; bool tied; } status;
	status.override_status = false;
	long result = game_engine_get()->p37(*player_reference, &status.override_status);
	if (status.override_status || result == NONE)
	{
		bool leading;
		if (g_4e6948->flag1128)
		{
			function_159460(*player_reference, &leading, &status.tied);
			if (leading)
				result = status.tied ? 9 : 8;
			else
				result = 10;
		}
		else
		{
			s_engine_player *player = engine_player_get(*player_reference);
			long countdown = function_xaee93d()->value_e0;
			if (player->unit_index == NONE)
			{
				if (player->team == NONE)
					result = 1;
				else if (function_15db30(*player_reference))
					result = 4;
				else if (player->flags & 0x4000)
					result = 3;
				else if (player->value164 >= 0)
					result = 2;
			}
			else
			{
				long duration = 3 * g_510c54->field_2_3;
				if (player->value1a8 < duration && g_4e6948->value1b8)
				{
					long lives = player->value1ac;
					if (lives == 1)
						result = 24;
					else
						result = lives == 2 ? 23 : 22;
				}
				else
				{
					long elapsed = g_510c54->game_time - ((s_engine_round_clock *)g_4e9ae8)->start_time;
					elapsed = elapsed < 0 ? 0 : elapsed;
					long const *elapsed_reference = &elapsed;
					if (*elapsed_reference < duration)
						result = 18;
					else if (countdown != NONE && countdown != 0 && countdown < 5)
						result = 17;
					else if (result == NONE)
					{
						bool unlimited = g_4e6948->score_to_win == 0;
						function_159460(*player_reference, &leading, &status.tied);
						if (leading)
							result = !status.tied ? (unlimited ? 25 : 5) : (unlimited ? 26 : 6);
						else
							result = unlimited ? 27 : 7;
					}
				}
			}
		}
	}
	return result;
}

// @retail 0x15ebd0
void function_15ebd0(void)
{
	for (long user_index = local_player_next(NONE); user_index != NONE; user_index = local_player_next(user_index))
	{
		if (user_index != NONE)
		{
			long player_index = g_4e8c20->entries[user_index];
			if (player_index != NONE)
			{
				long type = function_15ee30(player_index);
				if (type != g_4e9af0.players[user_index].index)
				{
					s_engine_notice_settings *settings = ((s_engine_notice_root *)g_4e3b44[g_4e034c->index & 0xffff].bytes)->settings;
					g_4e9af0.players[user_index].index = type;
					g_4e9af0.players[user_index].value04 = 0;
					g_4e9af0.players[user_index].value08 = NONE;
					for (long i = 0; i < settings->notice_count; i++)
					{
						if (settings->notices[i].type == type)
						{
							g_4e9af0.players[user_index].value08 = i;
							break;
						}
					}
				}
			}
		}
	}
}

/* the two ids of the game options (+0x10, +0x14), read as a pair */
struct s_seed_pair_15a640
{
	long a;
	long b;
};

// @retail 0x15a640
long function_15a640(long tag_index)
{
    long index = function_15eb80(tag_index);
    if (index != NONE)
    {
    long mode = *(signed char *)((byte *)g_4e6948 + 0x210);
    if (mode == 19)
    {
        s_seed_pair_15a640 ids = *(s_seed_pair_15a640 const *)&g_4e6948->id_a;
        dword seed = (ids.a ^ ids.b) * 1664525 + 1013904223;
        mode = (long)(seed >> 16) % 17 + 2;
    }
    switch (mode)
    {
    case 1:
        index = -1;
        break;
    case 2:
        index = 5;
        break;
    case 3:
        index = 2;
        break;
    case 4:
        index = 14;
        break;
    case 5:
        index = 12;
        break;
    case 6:
        switch (index)
        {
        case 1: index = 0; break;
        case 8: index = 7; break;
        case 10: index = 3; break;
        case 11: index = 2; break;
        case 12: index = 5; break;
        case 13: index = 4; break;
        case 14: index = 2; break;
        case 15: index = 5; break;
        case 16: index = 7; break;
        case 17: index = 5; break;
        }
        break;
    case 7:
        switch (index)
        {
        case 0: index = 1; break;
        case 2: index = 14; break;
        case 3: index = 10; break;
        case 4: index = 13; break;
        case 5: index = 12; break;
        case 6: index = 1; break;
        case 7: index = 10; break;
        case 8: index = 10; break;
        case 9: index = 1; break;
        }
        break;
    case 8:
        switch (index)
        {
        case 0:
        case 1:
        case 2:
        case 3:
            index = 5;
            break;
        case 4:
        case 9:
        case 13:
        case 16:
            index = 15;
            break;
        case 6:
        case 7:
        case 8:
        case 10:
        case 11:
        case 14:
            index = 12;
            break;
        }
        break;
    case 9:
        switch (index)
        {
        case 2: index = 0; break;
        case 3: index = 1; break;
        case 4: index = 0; break;
        case 5: index = 9; break;
        case 10: index = 7; break;
        case 11: index = 6; break;
        case 12: index = 9; break;
        case 13: index = 7; break;
        case 14: index = 6; break;
        case 15: index = 6; break;
        case 16: index = 7; break;
        case 17: index = 8; break;
        }
        break;
    case 10:
        switch (index)
        {
        case 0: index = 3; break;
        case 1: index = 3; break;
        case 6: index = 2; break;
        case 7: index = 10; break;
        case 8: index = 10; break;
        case 9: index = 10; break;
        }
        break;
    case 11:
        switch (index)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 5:
            index = 3;
            break;
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 14:
        case 15:
            index = 10;
            break;
        }
        break;
    case 12:
        switch (index)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            index = 4;
            break;
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
            index = 13;
            break;
        }
        break;
    case 13:
        switch (index)
        {
        case 4: index = 3; break;
        case 13: index = 10; break;
        }
        break;
    case 14:
        switch (index)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            index = 0;
            break;
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
            index = 6;
            break;
        }
        break;
    case 15:
        switch (index)
        {
        case 0: index = 6; break;
        case 1: index = 7; break;
        case 2: index = 7; break;
        case 3: index = 7; break;
        case 4: index = 8; break;
        case 5: index = 8; break;
        case 9: index = 7; break;
        case 10: index = 7; break;
        case 11: index = 7; break;
        case 12: index = 8; break;
        case 13: index = 7; break;
        case 14: index = 8; break;
        case 15: index = 8; break;
        case 16: index = 7; break;
        case 17: index = 8; break;
        }
        break;
    case 16:
        switch (index)
        {
        case 6: index = 0; break;
        case 7: index = 3; break;
        case 8: index = 3; break;
        case 9: index = 1; break;
        case 10: index = 3; break;
        case 11: index = 1; break;
        case 12: index = 5; break;
        case 13: index = 4; break;
        case 14: index = 2; break;
        case 15: index = 5; break;
        case 16: index = 3; break;
        case 17: index = 5; break;
        }
        break;
    case 17:
        switch (index)
        {
        case 0:
            index = 6;
            break;
        case 1:
        case 2:
            index = 9;
            break;
        case 3:
            index = 10;
            break;
        case 4:
            index = 13;
            break;
        case 5:
            index = 12;
            break;
        case 16:
            index = 7;
            break;
        case 17:
            index = 8;
            break;
        }
        break;
    case 18:
        switch (index)
        {
        case 0: index = 16; break;
        case 1: index = 16; break;
        case 2: index = 16; break;
        case 3: index = 16; break;
        case 4: index = 16; break;
        case 5: index = 16; break;
        case 6: index = 16; break;
        case 7: index = 16; break;
        case 8: index = 16; break;
        case 9: index = 16; break;
        case 10: index = 16; break;
        case 11: index = 16; break;
        case 12: index = 16; break;
        case 13: index = 16; break;
        case 14: index = 16; break;
        case 15: index = 16; break;
        }
        break;
    }
    long result = NONE;
    if (index != NONE)
    {
        s_multiplayer_globals_blocks *data = (s_multiplayer_globals_blocks *)multiplayer_globals_data();
        result = data->elements[index].index;
    }
    return result;
    }
    return tag_index;
}

#include "object_markers.h"
long function_1469f0(real seconds);

struct s_player_death_state
{
    byte active;
    byte unknown01[3];
    point3f position;
    short ticks;
    short initial_ticks;
    byte unknown14[4];
};

static inline long death_seconds_to_ticks(real seconds)
{
    real scaled = (real)g_510c54->field_2_3 * seconds;
    long result;
    __asm { fld scaled }
    __asm { fistp result }
    return result;
}

static __forceinline long nonnegative_15ce70(long value)
{
	return value < 0 ? 0 : value;
}

// @retail 0x15ce70
void function_15ce70(long attacker_index, long victim_index, bool betrayal, byte damage)
{
    long award = NONE;
    if (!game_engine_get()) return;
    s_engine_player *victim = engine_player_get(victim_index);
    long damage_type = damage & 0x3f;
    long damage_modifier = damage >> 6;
    bool special = damage_type == 3 || damage_type == 0x1a || damage_type == 0x18 || damage_type == 0x17 || damage_type == 0x14;
    long elapsed = nonnegative_15ce70(g_510c54->game_time - ((s_engine_round_clock *)g_4e9ae8)->start_time);
    *(long *)((byte *)victim + 0x1b4) = elapsed;
    if (attacker_index != NONE)
    {
        s_engine_player *attacker = engine_player_get(attacker_index);
        bool different = attacker_index != victim_index;
        bool same_team = game_engine_get() ? game_engine_get()->p27(attacker->team, victim->team) : false;
        if (!different)
            *(long *)victim->unknown168 += death_seconds_to_ticks((real)*(long *)((byte *)g_4e6948 + 0x1c0));
        else if (!same_team && !victim->value164)
            *(long *)attacker->unknown168 += function_1469f0((real)*(long *)((byte *)g_4e6948 + 0x1e8));
    }
    if (g_4e6948->state == 2 && g_4e6948->mode != 4 && function_15b2f0() && victim->value1ac != NONE)
    {
        long lives = victim->value1ac - 1;
        if (lives <= 0) lives = 0;
        victim->value1ac = (short)lives;
        function_a7840((short)victim_index, 0x20);
    }
    s_player_death_state *state = (s_player_death_state *)&function_xaee93d()->players[(short)victim_index];
    long interval = death_seconds_to_ticks((real)engine_options()->value1bc);
    long minimum = death_seconds_to_ticks(1.0f);
    long penalty = *(long *)victim->unknown168;
    long delay = penalty > minimum ? penalty : minimum;
    long total = penalty + interval;
    if (total <= minimum) total = minimum;
    long mode = 2;
    if (function_x340af0()) mode = *(long *)((byte *)g_4e6948 + 0x1e4);
    switch (mode)
    {
    case 0:
        function_15d6c0(victim_index, total, delay);
        break;
    case 1:
        {
            long now = nonnegative_15ce70(g_510c54->game_time - ((s_engine_round_clock *)g_4e9ae8)->start_time);
            long next = (now / interval + 1) * interval;
            while (next < now + delay) next += interval;
            victim->value164 = next - now;
        }
        break;
    case 2:
        victim->value164 = total;
        break;
    }
    *(long *)victim->unknown168 = 0;
    if (victim->unit_index != NONE)
    {
        s_object_marker marker;
        state->active = 1;
        function_b8d30(victim->unit_index, 0x4000095, &marker, 1, false);
        state->position = marker.matrix.position;
    }
    else
    {
        state->active = 0;
        state->position.x = 0.0f;
        state->position.y = 0.0f;
        state->position.z = 500.0f;
    }
    state->ticks = (short)victim->value164;
    state->initial_ticks = (short)victim->value164;
    if (game_engine_get())
    {
        long object = function_xaee93d()->slots[(short)victim_index];
        if (object != NONE) function_b58c0(object, 1);
    }
    long subtype = NONE;
    if (damage_type != 0x29)
    {
        if (victim->flags & 2) subtype = 0xb;
        else if (attacker_index == NONE)
        {
            if (damage_type == 1) subtype = 0x22;
            else if (damage_type != 0x19) subtype = 0xd;
        }
        else if (attacker_index == victim_index) subtype = damage_type == 1 ? 0x22 : 1;
        else if (betrayal) subtype = 2;
        else if (special)
        {
            award = 0;
            subtype = damage_modifier == 2 ? 0x2b : 0x24;
            if (damage_modifier != 2 && damage_modifier == 1) award = 0xe;
        }
        else if (damage_type == 2 || damage_modifier == 3)
        {
            subtype = 0x23;
            award = 1;
            if (damage_modifier == 1) award = 0xe;
        }
        else if (damage_type == 0x16 && damage_modifier == 2)
        {
            subtype = 0x29;
            award = 2;
        }
        else
        {
            subtype = damage_type == 0xd || damage_type == 0xe ? 0x2a : 0;
            if (damage_modifier == 1) award = 0xe;
        }
    }
    long override_type = game_engine_get()->p31(attacker_index, victim_index, betrayal);
    if (override_type != NONE) subtype = override_type;
    if (subtype != NONE)
    {
        s_event event;
        game_engine_event_initialize_inline(&event, 0, subtype);
        if (attacker_index != NONE)
        {
            event.cause_player_index = attacker_index;
            event.cause_team = engine_player_get(attacker_index)->team;
        }
        game_engine_event_set_effect_player_inline(&event, victim_index);
        game_engine_event_send_inline(&event);
    }
    if (attacker_index != NONE && attacker_index != victim_index && !betrayal)
    {
        long index = attacker_index & 0xffff;
        long chain = *(short *)((byte *)&game_engine_get_statborg()->players[index] + 0x14);
        long streak = *(short *)((byte *)&game_engine_get_statborg()->players[index] + 0x12);
        long victim_streak = *(short *)((byte *)&game_engine_get_statborg()->players[victim_index & 0xffff] + 0x12);
        if (chain >= 7) function_1970a0(index, 5, 1);
        else if (chain == 6) function_1970a0(index, 4, 1);
        else if (chain == 5) function_1970a0(index, 3, 1);
        else if (chain == 4) function_1970a0(index, 2, 1);
        else if (chain == 3) function_1970a0(index, 1, 1);
        else if (chain == 2) function_1970a0(index, 0, 1);
        if (streak == 5) function_1970a0(index, 0xd, 1);
        else if (streak == 10) function_1970a0(index, 0xe, 1);
        else if (streak == 15) function_1970a0(index, 0xf, 1);
        else if (streak == 20) function_1970a0(index, 0x10, 1);
        else if (streak == 25) function_1970a0(index, 0x11, 1);
        if (damage_type == 0xd || damage_type == 0xe) function_1970a0(index, 6, 1);
        if (damage_type == 2 || damage_modifier == 3) function_1970a0(index, 7, 1);
        if (damage_type == 0x16 && damage_modifier == 2) function_1970a0(index, 0xc, 1);
        if (special) function_1970a0(index, damage_modifier == 2 ? 9 : 8, 1);
        long message = NONE;
        if (chain >= 7) { message = 9; award = 8; }
        else if (chain == 6) { message = 8; award = 7; }
        else if (chain == 5) { message = 7; award = 6; }
        else if (chain == 4) { message = 2; award = 5; }
        else if (chain == 3) { message = 1; award = 4; }
        else if (chain == 2) { message = 0; award = 3; }
        else if (streak == 5) { message = 3; award = 9; }
        else if (streak == 10) { message = 4; award = 10; }
        else if (streak == 15) { message = 10; award = 11; }
        else if (streak == 20) { message = 11; award = 12; }
        else if (streak > 20 && streak % 5 == 0) { message = 12; award = 13; }
        else if (victim_streak >= 5) message = 6;
        if (message != NONE)
        {
            s_event event;
            game_engine_event_initialize_inline(&event, 1, message);
            game_engine_event_set_cause_player(&event, attacker_index);
            game_engine_event_set_effect_player(&event, victim_index);
            function_19eb90(&event);
        }
    }
    game_engine_get()->p30(attacker_index, victim_index, betrayal, award);
}

#include "unknown_07f720.h"
#include <wchar.h>

extern color3f *g_468714;
dword __cdecl pack_color3f(color3f const *color);
bool function_15eaf0(void);
bool function_161e10(long team);
void game_engine_format_time(long seconds, word *text);
bool function_162b10(long object_index);
real function_242140(long object_index);

struct s_player_score_display
{
    long type;
    bool teams;
    byte unknown05[3];
    long limit;
    word name[32];
    bool timer;
    byte unknown4d;
    word timer_text[8];
    byte unknown5e[2];
    color3f color;
    bool leading;
    byte unknown6d[3];
    long player_index;
    long score;
    word score_text[8];
    bool rival;
    byte unknown89[3];
    long rival_index;
    color3f rival_color;
    long rival_score;
    word rival_text[8];
    bool charge;
    byte unknownb1[3];
    real charge_fraction;
    long zone_count;
    dword zone_colors[8];
    real zone_progress[8];
};

struct s_player_zone_progress
{
    byte unknown00;
    char zone;
    byte progress;
    byte unknown03[5];
};
struct s_score_zone_view
{
    byte unknown00[0x15c];
    short indices[8];
    long holders[8];
    byte unknown18c[4];
    s_player_zone_progress players[16];
    short count;
};

// score 481 (was 544): scalar homes now agree; text-workspace and later call/store allocation differ.
// Fast bounded copies: team pair 513, individual pair 616, rival pair 482 (plain 485);
// unsigned player 541, best-store order 542; rival pair plus earlier rounds 478 (plain 481).
// @retail 0x15f3a0
bool function_15f3a0(long local_index, s_player_score_display *display)
{
    s_statborg *stats = game_engine_statborg_inline();
    long player_index = NONE;
    if (local_index != NONE) player_index = g_4e8c20->entries[local_index];
    bool result = true;
    memset(display, 0, sizeof(*display));
    if (player_index != NONE && stats)
    {
        bool rounds = false;
        s_engine_player *player = engine_player_get(player_index);
        if (*(long *)((byte *)g_4e6948 + 0x18c) == 1) rounds = true;
        if (player->team != NONE)
        {
            unicode_string_snprintf(display->name, 32, (word const *)L"%s", (word *)((byte *)g_4e6948 + 0x140));
            c_engine_peer *engine = game_engine_get();
            if (engine)
            {
                switch (engine->p0())
                {
                case 1: display->type = 2; break;
                case 9: display->type = 3; break;
                default: display->type = 1; break;
                }
            }
            else display->type = 0;
            if (engine_options()->value190 == 0) display->timer = false;
            else
            {
                display->timer = true;
                function_15ea80(0xe423, display->timer_text, 8);
            }
            if (rounds)
            {
                switch (*(long *)((byte *)g_4e6948 + 0x188))
                {
                case 0: display->limit = 1; break;
                case 1: display->limit = 2; break;
                case 2: display->limit = 4; break;
                case 3: display->limit = 6; break;
                case 4: display->limit = 2; break;
                case 5: display->limit = 3; break;
                case 6: display->limit = 4; break;
                default: __assume(0);
                }
            }
            else display->limit = *(long *)((byte *)g_4e6948 + 0x18c);
            display->teams = function_15eaf0();
            if (display->teams)
            {
                long field = rounds ? 7 : 0;
                long score = ((short *)&stats->teams[player->team])[field];
                display->player_index = NONE;
                display->rival_index = NONE;
                long best_score = -32768;
                long best = NONE;
                for (long team = 0; team < 8; team++)
                {
                    if (team != player->team && function_161e10(team))
                    {
                        long candidate = ((short *)&stats->teams[team])[field];
                        if (candidate > best_score) { best = team; best_score = candidate; }
                    }
                }
                if (best != NONE)
                {
                    word text[256];
                    text[0] = 0;
                    function_159130(best_score, text);
                    unicode_string_copy(display->rival_text, text, 8);
                    color3f color;
                    display->rival_color = *function_7f720(&color, (short)best);
                    display->rival_score = best_score;
                    display->leading = score > best_score;
                    display->rival = true;
                }
                else { display->rival = false; display->leading = true; }
                word text[256];
                text[0] = 0;
                long mode = g_4e6948->mode_180;
                if ((mode >= 3 && mode <= 4) || mode == 8) game_engine_format_time(score, text);
                else function_1630e0(text, (word const *)L"%d", score);
                wcsncpy((wchar_t *)display->score_text, (wchar_t const *)text, 7);
                display->score_text[7] = 0;
                display->score = score;
                color3f color;
                display->color = *function_7f720(&color, player->team);
            }
            else
            {
                long field = rounds ? 7 : 0;
                long score = ((short *)&stats->players[player_index & 0xffff])[field];
                display->player_index = player_index;
                long best = NONE;
                long best_score = -32768;
                s_engine_player_iterator iterator;
                iterator.data = g_4e8c24;
                iterator.index = NONE;
                iterator.absolute_index = NONE;
                while (player_iterator_next((long *)&iterator))
                {
                    if (iterator.index != player_index)
                    {
                        long candidate = ((short *)&stats->players[iterator.index & 0xffff])[field];
                        if (candidate > best_score) { best = iterator.index; best_score = candidate; }
                    }
                }
                if (best != NONE)
                {
                    s_engine_player *other = engine_player_get(best);
                    word text[256];
                    text[0] = 0;
                    function_159130(best_score, text);
                    unicode_string_copy(display->rival_text, text, 8);
                    color3f colors[4];
                    function_7f790(display->teams ? other->team : NONE, false, (s_player_appearance *)((byte *)other + 0x84), colors);
                    display->rival_color = colors[0];
                    display->rival_score = best_score;
                    display->rival_index = best;
                    display->leading = score > best_score;
                    display->rival = true;
                }
                else { display->rival = false; display->leading = true; }
                word text[256];
                text[0] = 0;
                function_159130(score, text);
                wcsncpy((wchar_t *)display->score_text, (wchar_t const *)text, 7);
                display->score_text[7] = 0;
                display->score = score;
                color3f colors[4];
                function_7f790(display->teams ? player->team : NONE, false, (s_player_appearance *)((byte *)player + 0x84), colors);
                display->color = colors[0];
            }
            if (!rounds && display->limit == 0)
            {
                long highest = display->rival_score > display->score ? display->rival_score : display->score;
                display->limit = highest < 1 ? 1 : highest;
            }
            if (g_4e6948->mode_180 == 8)
            {
                display->zone_count = 0;
                dword present = 0;
                s_engine_player_iterator iterator;
                iterator.data = g_4e8c24;
                iterator.index = NONE;
                iterator.absolute_index = NONE;
                while (player_iterator_next((long *)&iterator)) present |= 1 << iterator.index;
                s_score_zone_view *zones = (s_score_zone_view *)g_4e9ae8;
                for (long zone = 0; zone < zones->count; zone++)
                {
                    if (zones->indices[zone] != NONE)
                    {
                        long holder = zones->holders[zone];
                        byte progress = 0;
                        if (holder != NONE)
                        {
                            s_engine_player *owner = engine_player_get(holder);
                            color3f color = *g_468714;
                            if (owner->team != NONE)
                            {
                                color3f team_color;
                                color = *function_7f720(&team_color, owner->team);
                            }
                            for (long p = 0; p < 16; p++)
                                if (((word)present & (1 << p)) && zones->players[p].zone == zone && progress <= zones->players[p].progress)
                                    progress = zones->players[p].progress;
                            display->zone_colors[display->zone_count] = pack_color3f(&color);
                            display->zone_progress[display->zone_count] = 1.0f - (real)progress * (1.0f / 63.0f);
                        }
                        else
                        {
                            long best = NONE;
                            for (long p = 0; p < 16; p++)
                                if (((word)present & (1 << p)) && zones->players[p].zone == zone && zones->players[p].progress > progress)
                                { best = p; progress = zones->players[p].progress; }
                            if (best != NONE)
                            {
                                s_engine_player *owner = 0;
                                if (best >= 0 && best < g_4e8c24->high_water_index)
                                {
                                    s_engine_player *entry = (s_engine_player *)(g_4e8c24->data + g_4e8c24->size * best);
                                    if (entry->salt) owner = entry;
                                }
                                color3f color = *g_468714;
                                if (owner->team != NONE)
                                {
                                    color3f team_color;
                                    color = *function_7f720(&team_color, owner->team);
                                }
                                display->zone_colors[display->zone_count] = pack_color3f(&color);
                                display->zone_progress[display->zone_count] = (real)progress * (1.0f / 63.0f);
                            }
                            else
                            {
                                display->zone_colors[display->zone_count] = 0;
                                display->zone_progress[display->zone_count] = 0.0f;
                            }
                        }
                        display->zone_count++;
                    }
                }
            }
            else display->zone_count = 0;
        }
        else result = false;
        if (player->unit_index != NONE)
        {
            long weapon = NONE;
            byte *unit = ((byte **)(g_4e0300->data + (player->unit_index & 0xffff) * 12))[2];
            short slot = *(char *)(unit + 0x212);
            if (slot != NONE) weapon = ((long *)(unit + 0x218))[slot];
            if (function_162b10(weapon))
            {
                volatile real  charge = function_242140(weapon);
                if (charge > 0.0f) { display->charge = true; display->charge_fraction = charge; }
            }
            else if (g_4e6948->mode_180 == 8)
            {
                s_player_zone_progress *progress = &((s_score_zone_view *)g_4e9ae8)->players[player_index & 0xffff];
                if (progress->progress && progress->zone != NONE)
                { display->charge = true; display->charge_fraction = (real)progress->progress * (1.0f / 63.0f); }
            }
        }
    }
    else result = false;
    return result;
}

struct s_input_state
{
    byte unknown00[0x10];
    byte values[0x38];
};
extern byte g_4e61b9;
extern s_input_state g_4e61dc[3];
extern s_input_state g_4e630c;
bool function_162c50(long player_index, long *spectated_player_index);

/* the fields of a player that 0x15bb20 reads and writes, typed */
struct s_player_view_15bb20
{
    short salt;
    word flags;
    byte unknown04[0x28 - 4];
    short local_index;
    byte unknown2a[2];
    long unit_index;
    byte unknown30[0x170 - 0x30];
    long value170;
    byte unknown174[0x194 - 0x174];
    long value194;
    byte unknown198[0x1a6 - 0x198];
    char seat_timer;
    char dead_timer;
    char alive_timer;
    byte unknown1a9[0x1b0 - 0x1a9];
    long value1b0;
    byte unknown1b4[0x218 - 0x1b4];
    short dead_count;
};


// @retail 0x15bb20
void __stdcall function_15bb20(long player_index)
{
    long absolute_index = player_index & 0xffff;
    s_player_view_15bb20 *player = (s_player_view_15bb20 *)engine_player_get(player_index);
    if (player->value194 > 0)
        --player->value194;
    if (g_4e6948->mode != 4 && player->unit_index != NONE && function_15b2f0() &&
        g_510c54->game_time % g_510c54->field_2_3 == 0)
        function_1967d0(absolute_index, NONE, 13, 1);

    long unit_index = player->unit_index;
    if (unit_index != NONE)
    {
        byte *unit = (byte *)engine_object_get(unit_index);
        if (*(long *)(unit + 0x14) != NONE && *(short *)(unit + 0x1fc) != NONE)
        {
            if (player->seat_timer == NONE)
                player->seat_timer = (char)(g_510c54->field_2_3 * 2);
            else if (player->seat_timer > 0)
                --player->seat_timer;
        }
        else
            player->seat_timer = NONE;
    }
    if (player->unit_index != NONE)
    {
        if (player->dead_timer != NONE && g_4e6948->mode == 4)
            ++player->dead_count;
        player->dead_timer = NONE;
        if (player->alive_timer == NONE)
        {
            player->alive_timer = 0;
            if (g_4e6948->mode != 4)
            {
                player->value1b0 = NONE;
                function_a7840((short)player_index, 0x40);
            }
        }
        else if (player->alive_timer < 0x7f)
            ++player->alive_timer;
    }
    else
    {
        if (player->dead_timer == NONE)
            player->dead_timer = (char)g_510c54->field_2_3;
        else if (player->dead_timer > 0)
            --player->dead_timer;
        player->alive_timer = NONE;
    }

    if (player->local_index != NONE && *(long *)((byte *)player + 0x24) != NONE)
    {
        s_input_state *input = NULL;
        short controller = (short)*(long *)((byte *)player + 0x24);
        if (g_4e61cc[controller] != 0)
            input = g_4e61b9 ? &g_4e630c : &g_4e61dc[controller];
        long ticks = g_510c54->field_2_3;
        long minimum_time = ticks * 3;
        bool active = input && input->values[0xd] != 0;
        if (!function_15b2f0())
        {
            long elapsed = (long)g_4e9af0.unknown04[0] + 1;
            g_4e9af0.unknown04[0] = (byte)(elapsed > ticks ? ticks : elapsed);
        }
        else
            g_4e9af0.unknown04[0] = 0;

        if ((active || g_4e9af0.unknown04[0] == ticks ||
            (player->unit_index == NONE && player->value170 <= 1 && !(player->flags & 0x4000) &&
                !function_15db30(player_index) && g_510c54->game_time > minimum_time && (player->flags & 1))) &&
            !function_162c50(player_index, &player_index))
        {
                long maximum = real_to_long((real)g_510c54->field_2_3 * 0.25f);
                g_4e9af0.timers[player->local_index] = (byte)((long)g_4e9af0.timers[player->local_index] + 1 > maximum ? maximum : (long)g_4e9af0.timers[player->local_index] + 1);
                if (input)
                {
                    short delta = (short &)input->values[0x36];
                    real change = (real)(-delta);
                    change *= 3.0518509447574615e-05f;
                    real scale = 240.0f / (real)g_510c54->field_2_3;
                    change *= scale;
                    (real &)g_4e9af0.unknown04[4 + player->local_index * 4] += change;
                }
        }
        else
        {
            g_4e9af0.timers[player->local_index] = (byte)((long)g_4e9af0.timers[player->local_index] - 1 > 0 ? (long)g_4e9af0.timers[player->local_index] - 1 : 0);
        }
    }
}

long function_23f360(long team, long mode);
long function_23f260(long mode, long player_index, long value);

// @retail 0x158e90
long function_158e90(long team)
{
    bool flag = g_4e6948->flag1128;
    return function_23f360(team, flag != 0) / 2;
}

// @retail 0x158eb0
bool function_158eb0()
{
    bool result = true;
    for (long team = 0; team < 8; ++team)
    {
        if (engine_team_is_active(team) && function_158e90(team) > 0)
        {
            result = false;
            break;
        }
    }
    return result;
}

// score 5 (was 5): the mode and the player index are kept in swapped registers: retail computes the
// mode in ecx and the index in edx, ours the other way round.
// @retail 0x158f50
bool function_158f50()
{
    long count = 0;
    bool result = true;
    s_engine_player_iterator iterator;
    iterator.data = g_4e8c24;
    iterator.absolute_index = NONE;
    iterator.index = NONE;
    while (player_iterator_next((long *)&iterator))
    {
        ++count;
        bool flag = g_4e6948->flag1128;
        if (function_23f260(flag != 0, iterator.index, NONE) / 2 > 0)
        {
            result = false;
            break;
        }
    }
    if (count == 1)
        result = false;
    return result;
}

void __stdcall function_19ef40(long unit_index, long *first_count, long *second_count);

// @retail 0x15e970
void function_15e970(long unit_index)
{
    byte *unit = (byte *)engine_object_get(unit_index);
    long first_limit, second_limit;
    if (function_xaee93d()->flags & 8)
    {
        first_limit = 1;
        second_limit = first_limit;
    }
    else
    {
        first_limit = 2;
        second_limit = first_limit;
    }
    long first_count = first_limit;
    long second_count = 0;
    function_19ef40(unit_index, &first_count, &second_count);
    if (!((bool)((*(dword *)((byte *)g_4e6948 + 0x184) >> 11) & 1)))
        second_count = first_count = 0;
    switch (*(char *)((byte *)g_4e6948 + 0x210))
    {
    case 15:
    case 17:
        second_count += first_count;
        first_count = 0;
        break;
    case 16:
        first_count += second_count;
        second_count = 0;
        break;
    }
    if (first_count > first_limit) first_count = first_limit;
    if (second_count > second_limit) second_count = second_limit;
    unit[0x23e] = (byte)first_count;
    unit[0x23f] = (byte)second_count;
    engine_object_mark_dirty(unit_index, 0x400000);
}

#include "font_loading.h"
#include "unknown_030290.h"
#include "text_style_clear.h"
extern long g_4ba04c;
extern real g_4e69c0[4];
extern point3f *g_468718;
void function_13edb0(long font, long style, long justification, dword flags,
    color4f const *color, color4f const *shadow);
void function_13ec70(color4f const *color);
class c_1fa50
{
public:
    void function_1fa50(short_rectangle2d const *bounds, void const *clip, void const *position,
        long line_gap, real scale, long color, void const *shadow) const;
};
PRIVATE color3f const score_text_color = { 0.4588235318660736f, 0.729411780834198f, 1.0f };
PRIVATE color3f const alternate_score_text_color = { 0.8078431487083435f, 0.5607843399047852f, 0.8705882430076599f };

// score 41 (was 41): zero uses EBP vs EBX; player lookup folds its base before the byte read.
// Fast CRT color copies 41; unsigned font index 41; horizontal bounds before y 70;
// branch font selector 137; scoped size optimization 233.
// @retail 0x15e530
void __stdcall function_15e530(c_1fa50 const *text, real alpha, point2f const *point)
{
    long font_index = (g_4ba04c <= 1) + 5;
    short_rectangle2d bounds = g_4b9dd8;
    s_font_header *font = font_get(g_4e28f4[font_index]);
    long height = 10;
    if (font)
        height = font->leading_height + font->descending_height + font->ascending_height;
    color3f const *rgb = &score_text_color;
    if (g_4b9ed8 != NONE)
    {
        long player_index = g_4e8c20->entries[g_4b9ed8];
        if (player_index != NONE)
        {
            char appearance = *((char *)engine_player_get(player_index) + 0x88);
            if (appearance == 1 || appearance == 3)
                rgb = &alternate_score_text_color;
        }
    }
    color4f color;
    *(color3f *)&color.red = *rgb;
    color.alpha = ((g_4b9ed8 >= 0 && g_4b9ed8 < 4) ? g_4e69c0[g_4b9ed8] : 1.0f) * alpha;
    color4f shadow;
    *(point3f *)&shadow.red = *g_468718;
    shadow.alpha = ((g_4b9ed8 >= 0 && g_4b9ed8 < 4) ? g_4e69c0[g_4b9ed8] : 1.0f) * alpha;
    real x = point->x - g_4b9dd2;
    real y = point->y - g_4b9dd0;
    bounds.left = (short)(x - 300.0f);
    bounds.right = (short)(x + 300.0f);
    bounds.top = (short)(y - (short)height);
    bounds.bottom = (short)y;
    function_13edb0(font_index, NONE, 2, 0, &color, &shadow);
    function_13ec70(&color);
    text->function_1fa50(&bounds, 0, 0, 0, 1.0f, 0, 0);
    text_style_clear();
}

bool function_15b2f0();
long function_162470(bool flag);
void __stdcall function_a7810(dword flags);
long function_15b330(bool teams);

// @retail 0x15be20
void function_15be20()
{
	s_game_engine_globals *globals = function_xaee93d();
	if (game_engine_get() && globals->value6c == 1)
	{
		/* the mode is read as a long for each test, as the matched 0x240d70 does */
		long const &first_mode = (long)g_4e6948->mode;
		if (first_mode == 4 || globals->value_c04 == 1)
		{
			long const &mode = (long)g_4e6948->mode;
			if (mode != 4)
			{
				if (engine_options()->value190)
				{
					long ticks = function_162470(true);
					long seconds = real_truncate((real)ticks * g_510c54->rate);
					if (seconds != function_xaee93d()->value_e0)
					{
						function_xaee93d()->value_e0 = (short)seconds;
						function_a7810(0x10);
						long subtype = NONE;
						if (seconds == 1800) subtype = 0xe;
						else if (seconds == 900) subtype = 0xf;
						else if (seconds == 300) subtype = 0x10;
						else if (seconds == 60) subtype = 0x11;
						else if (seconds == 30) subtype = 0x20;
						else if (seconds == 10) subtype = 0x21;
						if (subtype != NONE)
						{
							s_event event;
							game_engine_event_initialize_inline(&event, 0, subtype);
							function_19eb90(&event);
						}
					}
					if (ticks <= 0)
					{
						s_event event;
						game_engine_event_initialize_inline(&event, 0, 0x12);
						function_19eb90(&event);
						function_15b3a0(function_15b330(false), 0);
					}
				}
				else
				{
					long ticks = 0 > g_510c54->game_time - ((s_engine_round_clock *)globals)->start_time ? 0 :
						g_510c54->game_time - ((s_engine_round_clock *)globals)->start_time;
					long seconds = real_truncate((real)ticks * g_510c54->rate);
					if (seconds > 0x7fff) seconds = 0x7fff;
					if (seconds != globals->value_e0)
					{
						globals->value_e0 = (short)seconds;
						function_a7810(0x10);
					}
				}
			}
		}
	}
}

#include <wchar.h>
bool function_30710(s_camera const *camera, short_rectangle2d const *bounds, vector3f const *vector, point2f *point, s_view const *view);
void parse_text(word *text);

// @retail 0x159250
void function_159250(long player_index, real alpha)
{
	s_engine_player *player = 0;
	if (player_index != NONE)
	{
		s_record_pool *players = g_4e8c24;
		long index = player_index & 0xffff;
		if (index < players->high_water_index)
		{
			s_engine_player *entry = (s_engine_player *)(players->data + players->size * index);
			if (entry->salt && entry->salt == (player_index >> 16))
				player = entry;
		}
	}
	if (player && player->unit_index != NONE)
	{
		word text[0x100];
		point2f point;
		s_object_marker marker;
		function_b8d30(player->unit_index, 0x04000095, &marker, 1, false);
		point3f position = marker.matrix.position;
		position.z += 0.05f;
		real x = position.x;
		real y = position.y;
		real z = position.z;
		real scale = g_4b9e14.scale;
		if (scale != 1.0f)
		{
			x = scale * x;
			y = scale * y;
			z = scale * z;
		}
		vector3f transformed;
		transformed.i = g_4b9e14.forward.i * z;
		transformed.i += g_4b9e14.right.i * x;
		transformed.i += g_4b9e14.up.i * y;
		transformed.i += g_4b9e14.position.x;
		transformed.j = g_4b9e14.forward.j * z;
		transformed.j += g_4b9e14.right.j * x;
		transformed.j += g_4b9e14.up.j * y;
		transformed.j += g_4b9e14.position.y;
		transformed.k = g_4b9e14.forward.k * z;
		transformed.k += g_4b9e14.right.k * x;
		transformed.k += g_4b9e14.up.k * y;
		transformed.k += g_4b9e14.position.z;
		if (function_30710((s_camera const *)&g_4b9e14, 0, &transformed, &point, (s_view const *)&g_camera_storage))
		{
			text[0] = 0;
			wcsncpy((wchar_t *)text, (wchar_t const *)((byte *)player + 0x44), 0xff);
			text[0xff] = 0;
			parse_text(text);
			function_15e530((c_1fa50 const *)text, alpha, &point);
		}
	}
}


struct s_effect_owner;
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner);
long __stdcall function_b7b40(void *creation);
void function_11df60(vector3f const *rotation, vector3f *forward, vector3f *up);
long weighted_choice_random(long tag_index, long *extra);
long weighted_choice_random_15aad0(long tag_index, long *extra);

/* retail calls 0x11df60 out of line here; its own file is built without /Ob1, so the call is kept out of line through a depth-0 helper */
#pragma inline_depth(0)
PRIVATE __forceinline void euler_vectors_get_15aad0(vector3f const *rotation, vector3f *forward, vector3f *up)
{
    function_11df60(rotation, forward, up);
}
#pragma inline_depth(255)

void __stdcall function_b9b90(long object_index, bool disable);
void __stdcall function_a7870(long object_index);

// @retail 0x15aad0
void function_15aad0(s_netgame_entry_state *entries, long index)
{
    s_netgame_entry_state *const *entries_reference = &entries;
    long const *index_reference = &index;
    byte *placement = *(byte **)((byte *)g_4e0350 + 0x124) + *index_reference * 0x90;
    long name = 0;
    long original = NONE;
    if (*(long *)(placement + 0x5c) != NONE)
    {
        /* the second case calls the folded twin of weighted_choice_random (src/unknown_19eda0.cpp) */
        switch (*(long *)(placement + 0x58))
        {
        case 0x69746d63:
            original = weighted_choice_random(*(long *)(placement + 0x5c), &name);
            break;
        case 0x76656863:
            original = weighted_choice_random_15aad0(*(long *)(placement + 0x5c), &name);
            break;
        }
    }
    long selected;
    if ((char)placement[0x14] >= 1 && (char)placement[0x14] <= 7)
        selected = ((c_engine_equipment *)*entries_reference)->choose(original, &name, (char)placement[0x14]);
    else if ((char)placement[0x14] == 8 || (char)placement[0x14] == 9)
        selected = function_15a5b0(original);
    else if ((char)placement[0x14] == 0)
        selected = function_15a640(original);
    else
        selected = original;
    if (selected != NONE)
    {
        byte creation[0xc4];
        function_b7930(creation, selected, NONE, NULL);
        *(long *)(creation + 0xc) = name;
        *(point3f *)(creation + 0x1c) = *(point3f *)(placement + 0x40);
        euler_vectors_get_15aad0((vector3f const *)(placement + 0x4c), (vector3f *)(creation + 0x28), (vector3f *)(creation + 0x34));
        if ((*entries_reference)[*index_reference].index != NONE)
            ((c_engine_equipment *)*entries_reference)->release_entry((*entries_reference)[*index_reference].index, *index_reference);
        long object_index = function_b7b40(creation);
        (*entries_reference)[*index_reference].index = object_index;
        (*entries_reference)[*index_reference].value04 = 0;
        (*entries_reference)[*index_reference].value06 = 0;
        if (object_index != NONE)
        {
            engine_object_get(object_index)->netgame_entry = (char)*index_reference;
            if ((placement[0] & 1) && selected == original)
                function_b9b90((*entries_reference)[*index_reference].index, true);
            function_a7870((*entries_reference)[*index_reference].index);
        }
    }
}


#include "object_iterator.h"
extern byte *g_4ed280;
void __stdcall function_b83b0(long object_index, bool flag);
void __stdcall function_b8460(long object_index, bool flag);
void function_17d520(void);
void contrails_delete_all(void);
void effects_delete_all(void);
void function_1073c0(void);
void function_c01c0(void);
void function_126400(void);
void function_d5560(bool skip_existing);
void function_c09c0(long mode);
void function_d5640(void);

// @retail 0x159e20
void function_159e20(void)
{
    bool clear_all = false;
    if (TEST_FIELD_BIT(engine_options()->flags_bit3)) clear_all = true;
    if (g_4e6948->mode != 4)
    {
        s_engine_player_iterator players;
        players.data = g_4e8c24;
        players.absolute_index = NONE;
        players.index = NONE;
        while (player_iterator_next_with_unit((long *)&players))
        {
            long unit = players.player->unit_index;
            function_b83b0(unit, true);
            function_b8460(unit, true);
        }
        struct { byte *object; s_type_f1af8e iterator; } state;
        dword type_mask = NONE;
        if (!clear_all) type_mask = 0xfffff83f;
        function_bae80(&state.iterator, type_mask, 0);
        while ((state.object = (byte *)function_baeb0(&state.iterator)) != NULL)
        {
            byte *object = state.object;
            long index = state.iterator.object_index;
            long current = index;
            long root = NONE;
            while (current != NONE)
            {
                root = current;
                current = *(long *)((byte *)engine_object_get(current) + 0x14);
            }
            bool remove;
            if (*(char *)(object + 0xaf) == NONE && engine_object_get(root)->netgame_entry == NONE)
            {
                switch (*(char *)(object + 0xaa))
                {
                case 2:
                    remove = *(short *)(g_4e3b44[*(long *)engine_object_get(index) & 0xffff].bytes + 0x290) == 0;
                    break;
                case 11:
                    remove = clear_all || (bool)((*(dword *)(object + 4) >> 14) & 1);
                    break;
                default:
                    remove = true;
                    break;
                }
            }
            else
                remove = clear_all;
            if (remove)
            {
                function_b83b0(index, true);
                function_b8460(index, true);
            }
        }
    }
    else if (clear_all)
    {
        struct { byte *object; s_type_f1af8e iterator; } state;
        function_bae80(&state.iterator, ~0UL, 0);
        while ((state.object = (byte *)function_baeb0(&state.iterator)) != NULL)
        {
            byte *object = state.object;
            if (*(long *)(object + 0x14) == NONE && *(long *)(object + 0xd4) == NONE)
            {
                function_b83b0(state.iterator.object_index, true);
                function_b8460(state.iterator.object_index, true);
            }
        }
    }
    if (clear_all)
    {
        function_17d520();
        memset(g_4ed280, 0xff, 0x4201);
        contrails_delete_all();
        effects_delete_all();
        function_1073c0();
        function_c01c0();
        if (g_55e4d0[g_4e9ae8->engine_index]) function_126400();
        function_d5560(false);
        function_c09c0(g_4e6948->state);
        function_d5640();
    }
}

bool function_100f00(long object_index);
bool function_161cd0(long object_index);
void __stdcall function_b8540(long object_index);
PRIVATE real const g_4448f8[2] = { 30.0f, 30.0f };
PRIVATE real const g_444900[2] = { 30.0f, 5.0f };
PRIVATE real const g_444908[2] = { 90.0f, 15.0f };
PRIVATE real const g_444910[2] = { 15.0f, 5.0f };
PRIVATE real const g_444918[2] = { 90.0f, 10.0f };

struct s_object_flags_159ac0
{
	byte unknown00[0x10a];
	word : 2;
	word bit_10a_2 : 1;
	word : 13;
	byte unknown10c[0x12c - 0x10c];
	word : 2;
	word bit_12c_2 : 1;
	word : 13;
};

// @retail 0x159ac0
void function_159ac0(void)
{
    if (g_4e6948->mode != 4)
    {
        long now = g_510c54->game_time;
        long mode = *(long *)((byte *)g_4e9ae8 + 0xc0c);
        struct { byte *object; s_type_f1af8e iterator; } state;
        function_bae80(&state.iterator, 0x81f, 0);
        while ((state.object = (byte *)function_baeb0(&state.iterator)) != NULL)
        {
            byte *object = state.object;
            long index = state.iterator.object_index;
            if ((index & 0xffff) % 30 != g_510c54->game_time % 30) continue;
            long type = *(char *)(object + 0xaa);
            if ((1 << type) & 0x1c)
            {
                bool available = true;
                long special = 0;
                if ((1 << *(char *)(object + 0xaa)) & 4)
                {
                    byte *definition = g_4e3b44[*(long *)engine_object_get(index) & 0xffff].bytes;
                    special = *(short *)(definition + 0x290);
                    available = function_100f00(index);
                }
                if (!special)
                {
                    if (*(char *)(object + 0xaf) != NONE && !TEST_FIELD_BIT(((s_object_flags_159ac0 *)object)->bit_12c_2) && available) continue;
                    if (function_161cd0(index) && available) continue;
                    if (object[0x12c] & 1) continue;
                    if (g_4448f8[mode] > (real)(now - *(long *)(object + 0x150)) * g_510c54->rate) continue;
                }
                else if (!g_55e4d0[g_4e9ae8->engine_index]->p38(index) || function_161cd0(index)) continue;
            }
            else if ((1 << type) & 1)
            {
                if (!TEST_FIELD_BIT(((s_object_flags_159ac0 *)object)->bit_10a_2)) continue;
                if (g_444900[mode] > (real)(now - *(long *)(object + 0xbc)) * g_510c54->rate &&
                    g_444908[mode] > (real)(now - *(long *)(object + 0x2e0)) * g_510c54->rate) continue;
            }
            else
            {
                if (!(bool)((*(dword *)(object + 4) >> 14) & 1) || now < *(long *)(object + 0x20)) continue;
                if (g_444910[mode] > (real)(now - *(long *)(object + 0xbc)) * g_510c54->rate &&
                    g_444918[mode] > (real)(now - *(long *)(object + 0x20)) * g_510c54->rate) continue;
            }
            function_b9b90(index, false);
            function_b8540(index);
        }
    }
}

__declspec(noinline) void function_15cba0(long mode);

// @retail 0x15cba0
void function_15cba0(long mode)
{
    switch (mode)
    {
    case 1:
        {
            s_game_engine_globals *globals = function_xaee93d();
            *(long *)globals->unknown70 = g_510c54->game_time;
            function_157a80();
            c_engine_peer *engine = game_engine_get();
            if (engine && g_4e6948->mode_180 == 4) engine->p3();
        }
        break;
    case 3: function_159e20(); break;
    }
}

// @retail 0x157ae0
void function_157ae0(void)
{
    s_game_engine_globals *globals = function_xaee93d();
    if (game_engine_get())
    {
        memset(globals->unknown70 + 4, 0, 0x6c);
        globals->value_e0 = *(short *)((byte *)g_4e6948 + 0x190);
        if ((short)globals->value6e > 0)
        {
            function_159e20();
            globals = function_xaee93d();
        }
        if (g_4e6948->mode != 4)
        {
            function_15dc40();
            function_15ad30(globals->netgame_entries);
            globals = function_xaee93d();
            s_statborg *table = game_engine_statborg_inline();
            if (table) function_1e97b0((s_object_values *)table);
            globals->flags |= 0x20;
        }
        if (g_4e6948->mode != 4 && g_510ca0) g_510ca1 = false;
        function_157a80();
        game_engine_get()->p3();
    }
}



#include "unknown_030290.h"
#include "unknown_163110.h"
#include <math.h>
extern long g_4e64a8;
void function_36580(void);
void function_36880(color4f const *color, s_short_rectangle const *rectangle);
void function_161420(long player_index, real alpha);

// @retail 0x159880
void function_159880(void)
{
    s_game_engine_globals *globals = function_xaee93d();
    if (game_engine_get())
    {
        long local_user = g_4b9ed8;
        long player_index = NONE;
        if (local_user != NONE)
        {
            player_index = g_4e8c20->entries[local_user];
            byte *ticks = (byte *)globals + 0xf5 + local_user;
            if (globals->timer_flags & (1 << local_user))
            {
                if (g_4e64a8 > 0)
                {
                    long limit = death_seconds_to_ticks(3.0f);
                    ++*ticks;
                    if (*ticks > limit) globals->timer_flags &= ~(1 << local_user);
                }
                else globals->timer_flags &= ~(1 << local_user);
            }
            else *ticks = 0;
            if (player_index != NONE)
            {
                s_engine_player_iterator iterator;
                iterator.data = g_4e8c24;
                iterator.absolute_index = NONE;
                iterator.index = NONE;
                while (player_iterator_next_with_unit((long *)&iterator))
                {
                    long target_index = iterator.index;
                    if (target_index == player_index) continue;
                    real alpha = function_159690(player_index, target_index);
                    if (alpha > 0.0f)
                    {
                        function_159250(target_index, alpha);
                        globals = function_xaee93d();
                    }
                }
            }
        }
        real fade = local_user == NONE ? 1.0f : globals->timers[local_user];
        if (fade > 0.0f)
        {
            color4f color;
            color.alpha = fade;
            *(point3f *)&color.red = *g_468718;
            s_short_rectangle bounds;
            bounds.left = 0;
            bounds.top = 0;
            bounds.right = (short)(g_4b9dd6 - g_4b9dd2);
            bounds.bottom = (short)(g_4b9dd4 - g_4b9dd0);
            function_36580();
            function_36880(&color, &bounds);
        }
        if (player_index != NONE)
        {
            real alpha = local_player_fraction(local_user);
            real intensity = alpha;
            if (intensity < 0.0f) intensity = 0.0f;
            else if (intensity > 1.0f) intensity = 1.0f;
            if (intensity > 0.0f)
                function_161420(player_index, powf(intensity, 1.9f));
        }
    }
}



void __stdcall function_d6bc0(long object_index);

// @retail 0x15ae70
void __stdcall function_15ae70(s_netgame_entry_state *entries)
{
    if (g_4e6948->mode == 4) return;
    byte *globals_tag = g_4e3b44[*(long *)((byte *)g_4e034c + 0x16c) & 0xffff].bytes;
    byte *preset = *(byte **)(*(byte **)(globals_tag + 0xc) + 0x534);
    long arg_642c39 = g_510c54->field_2_3;
    byte *scenario = (byte *)g_4e0350;
    for (short i = 0; i < *(long *)(scenario + 0x120); ++i)
    {
        byte *placement = *(byte **)(scenario + 0x124) + i * 0x90;
        if (i % arg_642c39 != g_510c54->game_time % arg_642c39) continue;
        short *timer = &entries[i].value04;
        c_engine_peer *engine = game_engine_get();
        long type = NONE;
        if (engine)
            type = engine->p0();
        if (function_15a090(type, 4, (short *)(placement + 4)) == NONE) continue;
        long index = entries[i].index;
        byte *object = NULL;
        if (index != NONE)
        {
            object = (byte *)engine_object_get(index);
            if (object[0xaa] == 1 && *(short *)(placement + 0x10) && ((byte *)g_4e6948)[0x208] != 1)
            {
                bool reset = function_158ff0(index);
                if (!reset)
                {
                    real z = *(real *)(placement + 0x48) - *(real *)(object + 0x38);
                    real y = *(real *)(placement + 0x44) - *(real *)(object + 0x34);
                    real x = *(real *)(placement + 0x40) - *(real *)(object + 0x30);
                    if (z*z + y*y + x*x > 100.0f)
                        reset = function_15f060((point3f *)(object + 0x30), *(real *)(preset + 0x28));
                    else
                        reset = true;
                }
                if (!reset) ++entries[i].value06;
                else entries[i].value06 = 0;
                if (entries[i].value06 > *(short *)(placement + 0x10)) *timer = 1;
            }
        }
        if (*timer == 0)
        {
            long schedule = entries[i].index == NONE;
            if (entries[i].index != NONE)
            {
                switch (*(short *)(placement + 0x12))
                {
                case 0:
                    {
                        if (((1 << object[0xaa]) & 0x1c) && (object[0x12c] & 3)) schedule = true;
                        else
                        {
                            real z = *(real *)(object + 0x38) - *(real *)(placement + 0x48);
                            real y = *(real *)(object + 0x34) - *(real *)(placement + 0x44);
                            real x = *(real *)(object + 0x30) - *(real *)(placement + 0x40);
                            schedule = z*z + y*y + x*x > 1.0f;
                        }
                    }
                    break;
                case 1: schedule = (object[0x10a] & 4) != 0; break;
                }
            }
            if (schedule)
            {
                long delay = function_15ac50(i);
                signed char modifier = 0;
                long kind = *(signed char *)(placement + 0x14);
                if (kind == 0) modifier = *(signed char *)((byte *)g_4e6948 + 0x211);
                else if (kind >= 1 && kind <= 7) modifier = *(signed char *)((byte *)g_4e6948 + 0x208);
                switch (modifier)
                {
                case 1: delay = NONE; break;
                case 2: delay >>= 1; break;
                case 3: delay <<= 1; break;
                }
                *timer = (short)delay;
            }
        }
        short countdown = *timer;
        if (countdown > 0)
        {
            --countdown;
            *timer = countdown;
            if (countdown == 0)
            {
                index = entries[i].index;
                if (index != NONE && (placement[0] & 2) && object[0xaa] != 2)
                {
                    if (object[0xaa] == 1 && (function_158ff0(index) || function_15f060((point3f *)(object + 0x30), *(real *)(preset + 0x28))))
                    {
                        *timer = 1;
                        continue;
                    }
                    function_d6bc0(entries[i].index);
                }
                function_15aad0(entries, i);
            }
        }
    }
}



#include "bitstream.h"
#include "unknown_067e10.h"
struct s_entry;

/* what an entity's update was last sent with (as src/unknown_0aa4d0.cpp
   defines it) */
struct s_relevance_observers;
struct s_update_state
{
	dword flags;
	long time;
	s_relevance_observers const *observers;
};

/* the observers an update is weighed for, as this code sees them */
struct s_update_observers
{
	byte unknown00[4];
	bool none;
};

/* a block an update carries: the creation data (type 0xc) or the state
   (type 0xd) of an entity */
struct s_update_block
{
	short size;
	short type;
	void *block;
};

/* the entity definitions, as this code calls them */
class c_entry_handler
{
public:
	virtual void slot0() {}
	virtual void slot1() {}
	virtual long get_state_size() { return 0; }
	virtual long get_data_size() { return 0; }
	virtual long get_bit_count() { return 0; }
	virtual dword get_update_mask() { return 0; }
	virtual bool v6(s_entry *entry) { return false; }
	virtual void slot7() {}
	virtual bool v8(s_entry *entry, s_update_observers const *observers) { return false; }
	virtual void v9(s_entry *entry, s_update_observers const *observers, long *bits) {}
	virtual void v10(s_entry *entry, long a, long b, long c) {}
	virtual void v11(s_entry *entry, s_update_state const *state, long *bits) {}
	virtual void v12(long data_size, void *data, long a, s_bitstream *stream) {}
	virtual bool v13(long data_size, void *data, s_bitstream *stream) { return false; }
	virtual bool v14(bool writing, dword mask, dword *written_mask, long state_size, void *state, long a, s_bitstream *stream, long b) { return false; }
	virtual bool v15(bool reading, dword *read_mask, long state_size, void *state, s_bitstream *stream) { return false; }
	virtual void slot16() {}
	virtual void slot17() {}
	virtual void v18(s_entry *entry, long data_size, void *data) {}
	virtual bool save(long, long, long, void *) { return false; }
	virtual bool load(void *entry, long *out, long size, void *buffer) { return false; }
	virtual void advance(void *entry) {}
	virtual bool v22(s_entry *entry, long data_size, void *data, long count, long state_size, void *state) { return false; }
	virtual bool v23(s_entry *entry, long a, long state_size, void *state) { return false; }
	virtual bool v24(s_entry *entry) { return false; }
	virtual bool is_valid(void *entry) { return false; }
};


struct s_entry
{
	dword identifier;
	short handler_index;
	byte unknown06;
	signed char unknown07;
	long unknown08;
	long unknown0c;
	long data_size;
	void *data;
	long state_size;
	void *state;
};

/* an update's state, as v12 of the replication interface reads it */
struct s_update_source
{
	long unknown00;
	void *state;
};

struct c_entry_table
{
	virtual bool slot0(long, long) { return true; }
	virtual bool write_busy(long a, s_bitstream *stream, long c, long d);
	virtual long read_busy(long a, s_bitstream *stream);
	virtual void slot3() {}
	virtual void slot4() {}
	virtual bool write_creation(dword identifier, dword flags, long a, s_bitstream *stream, long b, dword *written_mask);
	virtual long read_creation(long a, long *handler_index_out, dword *mask_out, long b, long *count, s_update_block *blocks, s_bitstream *stream);
	virtual void create(dword identifier, long handler_index, long a, long count, s_update_block *blocks);
	virtual void creation_relevance(dword identifier, dword flags, s_update_observers const *observers, real *relevance, long *bits);
	virtual void v9(dword identifier, long a, long b, long c);
	virtual bool write_update(dword identifier, dword flags, long a, s_bitstream *stream, long b, dword *written_mask);
	virtual long read_update(dword identifier, long *size_out, long a, long *count, s_update_block *blocks, s_bitstream *stream);
	virtual void apply_update(dword identifier, long a, long b, s_update_source const *source);
	virtual void update_relevance(dword identifier, dword flags, long time, s_update_observers const *observers, real *relevance, long *bits);
	virtual void deletion_relevance(dword identifier, long a, real *relevance);
	virtual void v15(dword identifier);
	virtual void slot16(long, long, long, long) {}
	virtual void v17(dword identifier);
	virtual bool function_08ad30(dword identifier);
	virtual void function_08ad70();
	virtual dword function_08add0(dword identifier);
	virtual bool slot21(long) { return true; }
	virtual bool slot22(long) { return true; }
	virtual bool function_0843e0(long a, long b);
	bool initialized;
	bool busy;
	byte unknown06[2];
	long unknown08;
	long unknown0c;
	struct { long count; c_entry_handler *handlers[1]; } *handlers;
	s_entry entries[1024];

	void function_08ae80(dword identifier, long handler_index, long a, long b, long c, long d);
	void function_08aed0(dword identifier);
	bool function_08af70(long handler_index, long *size, void **data);
	bool function_08b010(long handler_index, long *size, void **data);
	void function_08a030();
	void function_08a400(dword identifier);
};


long entity_table_new_entity(c_entry_table *table, long handler_index);
long function_b57d0(long handler_index, long object_index);
void function_15cba0(long mode);

static inline long local_user_next_158530(long index)
{
	long result = NONE;
	long start = index == NONE ? 0 : index + 1;

	for (long i = start; i < 4; i++)
	{
		if (g_4e8c20->entries[i] != NONE)
		{
			result = i;
			break;
		}
	}
	return result;
}

// @retail 0x158530
void function_158530(void)
{
    if (game_engine_get())
    {
        if (g_4e6948->mode != 4) function_157ae0();
        s_game_engine_globals *globals = function_xaee93d();
        for (long user = local_user_next_158530(NONE); user != NONE; user = local_user_next_158530(user))
        {
            globals->timers[user] = 1.0f;
            globals->timer_flags |= (byte)(1 << user);
        }
        globals->value6c = 1;
        globals->value_c04 = 1;
        if (g_4e6948->mode == 4) function_15cba0(1);
        c_engine_peer *engine = game_engine_get();
        if (engine)
        {
            long current = engine->get_current_id();
            if (current != NONE)
            {
                long handle = function_b57d0(current, NONE);
                if (handle != NONE)
                {
                    c_entry_table *entry_table = (c_entry_table *)&((c_class_6a600 *)g_4cf77c)->distribution->field_2098;
                    function_xaee93d()->index24 = handle;
                    entry_table->function_08a400(handle);
                }
            }
        }
        long mode = ((c_class_6a600 *)g_4cf77c)->state;
        if ((mode == 4 || mode == 5) && mode != 3 && mode != 5)
        {
            c_entry_table *table = (c_entry_table *)&((c_class_6a600 *)g_4cf77c)->distribution->field_2098;
            long handle = entity_table_new_entity(table, 7);
            if (handle != NONE)
            {
                table->entries[handle & 0x3ff].unknown08 = NONE;
                c_entry_table *entry_table = (c_entry_table *)&((c_class_6a600 *)g_4cf77c)->distribution->field_2098;
                function_xaee93d()->index28 = handle;
                entry_table->function_08a400(handle);
            }
        }
        for (long i = 0; i < 16; ++i)
        {
            c_class_6a600 *state_object = (c_class_6a600 *)g_4cf77c;
            mode = state_object->state;
            if ((mode == 4 || mode == 5) && mode != 3 && mode != 5)
            {
                c_entry_table *table = (c_entry_table *)&state_object->distribution->field_2098;
                long handle = entity_table_new_entity(table, 6);
                if (handle != NONE)
                {
                    table->entries[handle & 0x3ff].unknown08 = NONE;
                    table = (c_entry_table *)&state_object->distribution->field_2098;
                    function_xaee93d()->slots[(short)i] = handle;
                    s_entry *entry = &table->entries[handle & 0x3ff];
                    c_entry_handler *handler = table->handlers->handlers[entry->handler_index];
                    handler->v18(entry, entry->data_size, entry->data);
                    handler->save(entry->data_size, (long)entry->data, entry->state_size, entry->state);
                    entry->unknown06 = true;
                    entry->unknown0c = (1 << handler->get_bit_count()) - 1;
                }
            }
        }
        game_engine_get()->p12();
        function_158e20();
    }
}


// Retail callback records at 0x440dd8, traversed with a 0x24-byte stride.
void function_12560();
void function_141c0();
void function_22590();
void function_226a0(void);
void function_0226b0(void);
void function_226c0();
void function_2b4a0(void);
void function_2b540(void);
void function_02b5a0(void);
void function_2b5c0(void);
void function_03f2b0(void);
void function_67e30(void);
void function_067eb0(void);
void function_67ee0(void);
void function_67f60(void);
void function_8d690(void);
void transport_dispose(void);
void function_8d9f0();
void function_8dd70(void);
void function_b67c0();
void function_b68c0(void);
void function_b69d0(void);
void function_b6ab0(void);
void function_b6bb0();
void function_b6c50();
void __stdcall function_b6df0(dword const *previous, dword const *current, long count);
void function_d4de0(void);
void function_d4e20(void);
void function_d4e30(void);
void function_d4e60(short bsp_index);
void object_list_initialize(void);
void object_list_dispose(void);
void object_list_initialize_for_new_map(void);
void object_list_dispose_from_old_map(void);
void function_10a980(void);
void function_10a9d0(void);
void function_10a9e0(void);
void function_10aab0(void);
void function_11be20(void);
void function_11be40(void);
void function_11cc10(void);
void function_123b30(void);
void function_123bf0(void);
void game_state_map_begin(void);
void game_state_bsp_begin(void);
void function_1253d0(void);
void function_125600(void);
void function_125690(void);
void function_1257b0(void);
void sound_update_locations(void);
void function_131ff0(void);
void function_1320b0(void);
void scenario_ramps_initialize(void);
void scenario_ramps_dispose(void);
void scenario_ramps_reset_for_map(void);
void function_13bf00(void);
void function_13bf60(void);
void function_13bfc0(void);
void function_13e5c7(void);
void interface_initialize_for_new_map(void);
void interface_dispose_from_old_map(void);
void random_initialize(void);
void game_time_globals_allocate(void);
void game_time_globals_initialize(void);
void game_time_stop(void);
void function_1476c7();
void function_14783f(void);
void function_14784e();
void function_147920(void);
void function_14b4b0(void);
void function_14b560(void);
void function_14b580(void);
void function_14b600(void);
void function_14b6a0();
void function_153870(void);
void function_1538b0(void);
void function_1552e0(void);
void function_155380(void);
void function_155490(void);
void function_155ea0(void);
void function_155ed0(void);
void function_157450(void);
void function_1574c0(void);
void function_157bb0(void);
void function_16efb0(void);
void function_16f0e0(void);
void function_16f120(void);
void function_175b50(void);
void effects_dispose(void);
void function_175c10(void);
void effects_dispose_from_old_map(void);
void function_175d70(void);
void function_175f40();
void function_17b7b0(void);
void function_17b850(void);
void function_17b870(void);
void function_17b8a0(void);
void contrails_update_locations(void);
void function_17d220(void);
void function_17d2a0(void);
void decals_initialize_for_new_map(void);
void function_17d330(void);
void decals_update_locations(void);
void function_17d440(void);
void __stdcall decals_render(long a, long b, long c);
void function_183e40();
void function_183ec0();
void function_183ee0(void);
void function_183f00(void);
void function_185ab0(void);
void function_185b00(void);
void object_looping_sounds_initialize(void);
void object_looping_sounds_dispose(void);
void object_looping_sounds_initialize_for_new_map(void);
void object_looping_sounds_dispose_from_old_map(void);
void object_looping_sounds_initialize_for_new_structure_bsp(void);
void level_handle_tables_initialize(void);
void level_handle_tables_dispose(void);
void function_19c190(void);
void function_19c1d0();
void function_1c25a0(void);
void function_1c2600(void);
void function_1c2670(void);
void function_1c2690();
void function_1c27a0();
void function_1c7790(void);
void function_1c79c0(void);
void function_1c79e0(void);
void ai_dispose_from_old_map(void);
void function_1c7bb0();
void function_1c7f60(void);
void function_1df460(void);
void function_1df4b0(void);
void player_request_state_initialize(void);
void player_request_state_initialize_for_new_map(void);
void player_request_state_dispose_from_old_map(void);
void function_1eb550(void);
void function_1eb5e0(void);
void function_1eb5f0(void);
void function_1ec420(void);
void function_1ec470(void);
void function_1ec480(void);
void function_1ec4b0(void);
void function_1fb2f0(void);
void function_1fb330(void);
void function_1fb350(void);
void hs_init(void);
void hs_map_begin(void);
void hs_map_end(void);
void script_nodes_dispose(void);
void function_20aee0(void);
void function_20af50(void);
void function_20af90(void);
void function_210db0(void);
void function_210df0(void);
void function_210e10(void);
void function_211030(void);
void saved_game_storage_initialize(void);
void function_215720(void);
void function_215730(void);
void function_217bc0(void);
void function_217c20(void);
void function_221850(void);
void function_2218a0(void);
void function_2218e0(void);
void function_2218f0(void);
void function_2229d0(void);
void function_222a10(void);
void function_222a60(void);
void function_225700(void);
void function_225a40(void);
void function_225a80(void);
void function_225f80(void);
void function_225fc0(void);
void function_3361a4();

s_subsystem_440dd8 const g_440dd8[68] =
{
    { function_8d690, transport_dispose, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { function_175f40, function_175f40, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { function_175f40, function_175f40, function_11cc10, function_175f40, NULL, NULL, NULL, NULL, NULL },
    { random_initialize, function_175f40, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { function_8d9f0, function_8dd70, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { function_175f40, function_175f40, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { function_175f40, function_175f40, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { function_175f40, function_175f40, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { function_175f40, function_175f40, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { saved_game_storage_initialize, function_215730, function_175f40, function_215720, NULL, NULL, NULL, NULL, NULL },
    { function_217bc0, function_217c20, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { function_155ea0, function_155ed0, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { function_175f40, function_175f40, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { level_handle_tables_initialize, level_handle_tables_dispose, function_19c190, function_19c1d0, NULL, NULL, NULL, NULL, NULL },
    { function_131ff0, function_1320b0, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { function_225700, function_175f40, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { function_123b30, function_123bf0, game_state_map_begin, function_175f40, game_state_bsp_begin, function_175f40, NULL, NULL, NULL },
    { game_time_globals_allocate, function_175f40, game_time_globals_initialize, game_time_stop, NULL, NULL, NULL, NULL, NULL },
    { function_1df460, function_175f40, function_1df4b0, function_175f40, NULL, NULL, NULL, NULL, NULL },
    { function_14b4b0, function_14b560, function_14b580, function_14b600, function_175f40, function_175f40, NULL, NULL, NULL },
    { function_185ab0, function_175f40, function_185b00, function_175f40, NULL, NULL, NULL, NULL, NULL },
    { player_request_state_initialize, function_175f40, player_request_state_initialize_for_new_map, player_request_state_dispose_from_old_map, NULL, NULL, NULL, NULL, NULL },
    { function_157450, function_175f40, function_1574c0, function_157bb0, NULL, NULL, NULL, NULL, NULL },
    { function_67e30, function_067eb0, function_67ee0, function_67f60, NULL, NULL, NULL, NULL, NULL },
    { function_175f40, function_175f40, function_11be20, function_11be40, NULL, NULL, NULL, NULL, NULL },
    { function_1eb550, function_1eb5e0, function_1eb5f0, function_175f40, NULL, NULL, NULL, NULL, NULL },
    { function_175f40, function_175f40, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { function_b67c0, function_b68c0, function_b69d0, function_b6ab0, function_b6bb0, function_b6c50, NULL, NULL, (t_subsystem_440dd8_callback_20)function_b6df0 },
    { object_list_initialize, object_list_dispose, object_list_initialize_for_new_map, object_list_dispose_from_old_map, NULL, NULL, NULL, NULL, NULL },
    { function_10a980, function_10a9d0, function_10a9e0, function_10aab0, NULL, NULL, NULL, NULL, NULL },
    { function_1ec420, function_1ec470, function_1ec480, function_1ec4b0, NULL, NULL, NULL, NULL, NULL },
    { function_12560, function_141c0, function_22590, function_226c0, function_226a0, function_0226b0, NULL, NULL, NULL },
    { function_2b4a0, function_2b5c0, function_2b540, function_02b5a0, NULL, NULL, NULL, NULL, NULL },
    { function_175f40, function_175f40, function_175f40, function_175f40, NULL, NULL, NULL, NULL, NULL },
    { function_183e40, function_175f40, function_183ec0, function_175f40, function_183ee0, function_183f00, NULL, NULL, NULL },
    { function_1552e0, function_175f40, function_155380, function_155490, NULL, NULL, NULL, NULL, NULL },
    { function_16efb0, function_175f40, function_16f0e0, function_175f40, function_16f120, function_175f40, NULL, NULL, NULL },
    { function_17d220, function_17d2a0, decals_initialize_for_new_map, function_17d330, decals_update_locations, function_17d440, NULL, (t_subsystem_440dd8_callback_20)decals_render, NULL },
    { function_175f40, function_175f40, function_175f40, function_175f40, NULL, NULL, NULL, NULL, NULL },
    { function_17b7b0, function_17b850, function_17b870, function_17b8a0, contrails_update_locations, function_175f40, NULL, NULL, NULL },
    { function_175b50, effects_dispose, function_175c10, effects_dispose_from_old_map, function_175d70, function_175f40, NULL, NULL, NULL },
    { function_175f40, function_175f40, function_175f40, function_175f40, function_03f2b0, function_175f40, NULL, NULL, NULL },
    { function_175f40, function_175f40, function_211030, function_175f40, NULL, NULL, NULL, NULL, NULL },
    { function_221850, function_2218f0, function_2218a0, function_2218e0, NULL, NULL, NULL, NULL, NULL },
    { function_1253d0, function_125600, function_125690, function_1257b0, sound_update_locations, function_175f40, NULL, NULL, NULL },
    { function_20aee0, function_20af50, function_20af90, function_175f40, NULL, NULL, NULL, NULL, NULL },
    { object_looping_sounds_initialize, object_looping_sounds_dispose, object_looping_sounds_initialize_for_new_map, object_looping_sounds_dispose_from_old_map, object_looping_sounds_initialize_for_new_structure_bsp, function_175f40, NULL, NULL, NULL },
    { function_225a40, function_175f40, function_225a80, function_175f40, NULL, NULL, NULL, NULL, NULL },
    { function_2229d0, function_175f40, function_222a10, function_222a60, NULL, NULL, NULL, NULL, NULL },
    { function_153870, function_175f40, function_1538b0, function_175f40, NULL, NULL, NULL, NULL, NULL },
    { function_1476c7, function_14783f, function_14784e, function_147920, NULL, NULL, NULL, NULL, NULL },
    { function_13e5c7, function_3361a4, interface_initialize_for_new_map, interface_dispose_from_old_map, NULL, NULL, NULL, NULL, NULL },
    { function_175f40, function_175f40, function_175f40, function_175f40, NULL, NULL, NULL, NULL, NULL },
    { function_13bf00, function_175f40, function_13bf60, function_13bfc0, NULL, NULL, NULL, NULL, NULL },
    { hs_init, script_nodes_dispose, hs_map_begin, hs_map_end, NULL, NULL, NULL, NULL, NULL },
    { function_1fb2f0, function_175f40, function_1fb330, function_1fb350, NULL, NULL, NULL, NULL, NULL },
    { function_175f40, function_175f40, function_175f40, function_175f40, NULL, NULL, NULL, NULL, NULL },
    { function_d4de0, function_175f40, function_d4e20, function_175f40, function_d4e30, function_175f40, (t_subsystem_440dd8_callback_18)function_d4e60, NULL, NULL },
    { function_1c7790, function_1c79c0, function_1c79e0, ai_dispose_from_old_map, function_1c7bb0, function_1c7f60, NULL, NULL, NULL },
    { function_1c25a0, function_1c2600, function_1c2670, function_175f40, function_1c2690, function_1c27a0, NULL, NULL, NULL },
    { function_175f40, function_175f40, function_175f40, function_175f40, function_14b6a0, function_175f40, NULL, NULL, NULL },
    { function_175f40, function_175f40, function_175f40, function_175f40, function_175f40, function_175f40, NULL, NULL, NULL },
    { function_175f40, function_175f40, function_175f40, function_175f40, function_175f40, function_175f40, NULL, NULL, NULL },
    { function_210db0, function_175f40, function_175f40, function_175f40, function_210df0, function_210e10, NULL, NULL, NULL },
    { scenario_ramps_initialize, scenario_ramps_dispose, scenario_ramps_reset_for_map, function_175f40, NULL, NULL, NULL, NULL, NULL },
    { function_175f40, function_175f40, function_175f40, function_175f40, function_175f40, function_175f40, NULL, NULL, NULL },
    { function_225f80, function_175f40, function_225fc0, function_175f40, NULL, NULL, NULL, NULL, NULL },
    { function_175f40, function_175f40, function_175f40, function_175f40, function_175f40, function_175f40, NULL, NULL, NULL },
};

// @retail 0x175f40
void function_175f40()
{
}
