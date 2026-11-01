// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_14B560.CPP: the players subsystem entries (initialize, dispose,
   initialize for new map, dispose from old map; the table at 0x441084) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_123b30.h"
#include "data_array.h"
#include "engine_peer.h"
#include "unknown_067e10.h"
#include <string.h>

/* the players globals (0x130 bytes; globals.h views them as s_index_table) */
struct s_players_globals
{
	long unknown00;
	byte unknown04[4];
	short unknown08;
	short unknown0a;
	long field_x9b462b[4];
	long unknown1c[4];
	/* the machines in the game: which are valid, and their addresses */
	dword machine_valid_mask;
	s_machine_address machine_addresses[16];
	bool local_machine_valid;
	s_machine_address local_machine_address;
	byte unknown97;
	long local_machine_index;
	short unknown9c;
	bool unknown9e;
	bool unknown9f;
	long unknowna0;
	short unknowna4;
	byte unknowna6[0xac - 0xa6];
	long unknownac;
	byte unknownb0[0x130 - 0xb0];
};

/* set while a saved game is being restored: the map entries leave the
   players alone */
bool g_4ed39c;

void ai_players_reset(void);

static inline void player_index_array_reset(long *indices)
{
	memset(indices, NONE, 4 * sizeof(long));
}

static inline void players_globals_reset(s_players_globals *globals)
{
	globals->unknown08 = 0;
	globals->unknown0a = 0;
	player_index_array_reset(globals->field_x9b462b);
	player_index_array_reset(globals->unknown1c);
}

// @retail 0x14b4b0
void function_14b4b0(void)
{
	g_4e8c24 = data_new_inlined("players", 0x10, 0x21c, 0, g_510c2c);
	s_players_globals *globals = (s_players_globals *)function_123d40("players globals", "players_global", sizeof(s_players_globals));
	g_4e8c20 = (s_index_table *)globals;
	players_globals_reset(globals);
}

// @retail 0x14b560
void function_14b560(void)
{
	if (g_4e8c24)
	{
		g_4e8c24 = 0;
	}
	if (g_4e8c20)
	{
		g_4e8c20 = 0;
	}
}

// @retail 0x14b580
void function_14b580(void)
{
	if (!g_4ed39c)
	{
		s_players_globals *globals = (s_players_globals *)g_4e8c20;
		s_record_pool *players = g_4e8c24;

		memset(globals, 0, sizeof(s_players_globals));
		players_globals_reset(globals);
		globals->unknowna4 = NONE;
		globals->unknownac = NONE;
		globals->unknown9c = 0;
		players->valid = true;
		record_pool_release_all(players);
		void (*reset_players)(void) = ai_players_reset;
		reset_players();
		((s_players_globals *)g_4e8c20)->local_machine_index = NONE;
	}
}

// @retail 0x14b600
void function_14b600(void)
{
	if (!g_4ed39c)
	{
		g_4e8c24->valid = false;
		void (*reset_players)(void) = ai_players_reset;
		reset_players();

		s_players_globals *globals = (s_players_globals *)g_4e8c20;

		globals->unknown00 = 0;
		players_globals_reset(globals);
	}
}

#define PIN(value, lower, upper) ((value) < (lower) ? (lower) : ((value) > (upper) ? (upper) : (value)))

/* the configuration of a player (0x90 bytes) */
struct s_type_b07538
{
	byte unknown00[0x44];
	char representation_index;
	byte unknown45[0x7c - 0x45];
	char team_index;
	char unknown7d;
	char unknown7e;
	char unknown7f;
	byte unknown80;
	char unknown81;
	byte unknown82[2];
	long unknown84;
	short unknown88;
	short unknown8a;
	long unknown8c;
};

/* the scenario's starting profiles (0x34 bytes each) */
struct s_scenario_starting_profile_view
{
	byte unknown00[0x2c];
	short representation_index;
	byte unknown2e[0x34 - 0x2e];
};

struct s_scenario_starting_profiles_view
{
	byte unknown000[0x100];
	long starting_profile_count;
	s_scenario_starting_profile_view *starting_profiles;
};

/* the globals tag's player representations (0xbc bytes each) */
struct s_player_representation
{
	byte unknown00[0xb4];
	long field_b4;
	long third_person;
};

struct s_globals_representations_view
{
	byte unknown000[0x138];
	long representation_count;
	s_player_representation *representations;
};

/* the appearance of a player (at +0x84 in a player) */
struct s_player_appearance
{
	byte unknown00[4];
	char representation_index;
	byte primary_change;
	byte secondary_change;
	byte flags;
};

/* a player (0x21c bytes) */
struct s_player
{
	short salt;
	union
	{
		struct
		{
			word connected : 1;
			word field_2_2 : 1;
			word unknown : 14;
		} flags;
		word flags_word;
	};
	byte unknown04[0x14 - 0x4];
	s_machine_address machine_address;
	/* the player's machine, and the user and controller it plays with there */
	short machine_index;
	short machine_user_index;
	byte unknown1e[2];
	long machine_controller_index;
	long controller_index;
	short user_index;
	byte unknown2a[2];
	long unit_index;
	long previous_unit_index;
	byte unknown34[0x38 - 0x34];
	dword latched_buttons;
	word latched_flags;
	byte unknown3e[0x84 - 0x3e];
	s_player_appearance appearance;
	byte unknown8c[0x17c - 0x8c];
	short field_17c;
	short field_17e;
	byte unknown180[0x218 - 0x180];
	short unknown218;
	byte unknown21a[2];
};

/* an object header (12 bytes; globals.h's g_4e0300) */
struct s_object_header_entry
{
	byte unknown00[8];
	byte *object;
};

#define FLAG(bit) (1 << (bit))
#define TEST_FLAG(flags, bit) (((flags) & FLAG(bit)) != 0)
#define SET_FLAG(flags, bit, value) ((value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)))

static inline s_player *player_get(long player_index)
{
	return (s_player *)g_4e8c24->data + (player_index & 0xffff);
}

PRIVATE bool player_appearance_get_function_value(long name, s_player_appearance const *appearance, real *value);

/* a copy of the game-has-teams test (0x15eaf0), which retail inlines here */
static inline bool game_has_teams()
{
	bool result = false;

	if (g_55e4d0[g_4e9ae8->engine_index])
		result = g_4e6948->flags184.bit0;

	return result;
}

// @retail 0x14bc80
void player_configuration_verify(s_type_b07538 *configuration)
{
	s_scenario_starting_profiles_view *scenario = (s_scenario_starting_profiles_view *)g_4e0350;
	s_globals_representations_view *globals = (s_globals_representations_view *)g_4e034c;

	if (g_4e6948->state == 1)
	{
		long representation_index = 0;

		for (long i = 0; i < scenario->starting_profile_count; i++)
		{
			s_scenario_starting_profile_view *profile = &scenario->starting_profiles[i];

			if (profile->representation_index != NONE)
			{
				representation_index = profile->representation_index;
				break;
			}
		}
		/* retail stores the team before the representation */
		*(char volatile *)&configuration->team_index = 1;
		*(char volatile *)&configuration->representation_index = (char)representation_index;
	}

	if (configuration->representation_index != NONE)
	{
		configuration->representation_index = PIN(configuration->representation_index, 0, globals->representation_count - 1);
	}
	if (configuration->unknown7e != NONE)
	{
		configuration->unknown7e = PIN(configuration->unknown7e, 0, 0x7f);
	}
	if (configuration->unknown7f != NONE)
	{
		configuration->unknown7f = PIN(configuration->unknown7f, 0, 0x7f);
	}

	if (configuration->unknown84 != NONE && configuration->unknown84 >= 0 && configuration->unknown84 < 16)
	{
		configuration->unknown8c = PIN(configuration->unknown8c, 0, 0x3fffffff);
		configuration->unknown88 = PIN(configuration->unknown88, 0, 0x7f);
		configuration->unknown8a = PIN(configuration->unknown8a, 0, 0x7f);
	}
	else
	{
		configuration->unknown84 = NONE;
		configuration->unknown8c = NONE;
		configuration->unknown8a = NONE;
		configuration->unknown88 = NONE;
	}

	configuration->unknown7d = PIN(configuration->unknown7d, 0, 3);
	if (PIN(configuration->unknown81, 0, 7) != configuration->unknown81)
	{
		configuration->unknown81 = 0;
	}

	if (game_has_teams() && configuration->team_index != NONE && !(g_4e9ae8->wa & (1 << configuration->team_index)))
	{
		configuration->team_index = NONE;
	}
}

// @retail 0x14cca0
bool player_get_function_value(long player_index, long name, real *value)
{
	real result = 0.0f;
	bool found = false;

	if (player_index != NONE)
	{
		s_player *player = &((s_player *)g_4e8c24->data)[player_index & 0xffff];

		found = player_appearance_get_function_value(name, &player->appearance, &result);
	}

	*value = result;
	return found;
}

// @retail 0x14ccf0
PRIVATE bool player_appearance_get_function_value(long name, s_player_appearance const *appearance, real *value)
{
	real result = 0.0f;

	switch (name)
	{
	case 0x0d0005ff:
		result = 1.0f;
		break;
	case 0x170005fa:
		result = (real)appearance->primary_change * 0.015625f;
		break;
	case 0x170005fb:
		result = (real)appearance->secondary_change * 0.03125f;
		break;
	case 0x180005fc:
		if (!(appearance->flags & 1))
		{
			result = 1.0f;
		}
		break;
	case 0x1d0005fd:
		result = (appearance->flags & 2) ? 1.0f : -1.0f;
		break;
	case 0x1d0005fe:
		result = (appearance->flags & 4) ? 1.0f : -1.0f;
		break;
	}

	*value = result;
	return true;
}

static inline long game_seconds_to_ticks_round(real seconds)
{
	real ticks = g_510c54->field_2_3 * seconds;
	long result;

	__asm
	{
		fld ticks
		fistp result
	}
	return result;
}

// @retail 0x14cd80
void players_start_countdown(void)
{
	s_players_globals *globals = (s_players_globals *)g_4e8c20;

	if (!globals->unknown9e)
	{
		globals->unknown9e = true;
		globals->unknown9f = false;
		globals->unknowna0 = game_seconds_to_ticks_round(3.0f);
	}
}

/* what a player's input aims at: a type and an index (8 bytes) */
struct s_player_action_target_reference
{
	long type;
	long index;
};

struct s_player_action_target
{
	s_player_action_target_reference reference;
	long unknown08;
};

/* the block at 0x4c of a player's input (0xc bytes) */
struct s_player_action_unknown4c
{
	word flags;
	byte unknown02[2];
	real unknown04;
	real unknown08;
};

/* the input of a player for one tick (0x5c bytes) */
struct s_player_action
{
	dword buttons;
	real facing_yaw;
	real facing_pitch;
	real throttle_i;
	real throttle_j;
	real field_14_3;
	real field_18_2;
	union
	{
		struct
		{
			byte unknown1c;
			byte flags1d;
		};
		word flags;
	};
	short unknown1e;
	char weapon_index;
	char grenade_index;
	short unknown22;
	short unknown24;
	byte unknown26[2];
	s_player_action_target target;
	long unknown34;
	long unknown38;
	long unknown3c;
	byte unknown40[0x4c - 0x40];
	s_player_action_unknown4c unknown4c;
	byte unknown58[4];
};

static inline void player_action_target_reference_clear(s_player_action_target_reference *reference)
{
	memset(reference, 0, sizeof(s_player_action_target_reference));
}

static inline void player_action_target_clear(s_player_action_target *target)
{
	memset(target, 0, sizeof(s_player_action_target));
	player_action_target_reference_clear(&target->reference);
}

static inline void player_action_unknown4c_clear(s_player_action_unknown4c *value)
{
	value->flags = 0;
	value->unknown04 = 0.0f;
	value->unknown08 = 0.0f;
}

// @retail 0x14d040
void player_action_initialize(s_player_action *action)
{
	memset(action, 0, sizeof(s_player_action));
	action->unknown1e = NONE;
	action->weapon_index = NONE;
	action->grenade_index = NONE;
	action->unknown22 = NONE;
	action->unknown24 = NONE;
	player_action_unknown4c_clear(&action->unknown4c);
	action->unknown34 = NONE;
	action->unknown38 = NONE;
	action->unknown3c = NONE;
	s_player_action_target *target = &action->target;
	player_action_target_clear(target);
	target->reference.index = NONE;
	target->unknown08 = NONE;
}

static inline bool function_x41b793(real value)
{
	return (*(long *)&value & 0x7f800000) != 0x7f800000;
}

static inline bool player_action_target_valid(s_player_action_target const *target)
{
	short type;

	if (!target)
	{
		return false;
	}
	type = (short)target->reference.type;
	if (type < 0 || type >= 9)
	{
		return false;
	}
	if (type != 0 && target->reference.index == NONE)
	{
		return false;
	}
	return true;
}

// @retail 0x14d0a0
bool function_14d0a0(s_player_action const *action)
{
	if (!action)
	{
		return false;
	}

	if (!function_x41b793(action->facing_pitch) || !function_x41b793(action->facing_yaw) ||
		!function_x41b793(action->field_14_3) || !function_x41b793(action->field_18_2))
	{
		return false;
	}

	if (!(action->field_14_3 >= 0.0f && action->field_14_3 <= 1.0f))
	{
		return false;
	}

	if (!function_x41b793(action->throttle_i) || !function_x41b793(action->throttle_j))
	{
		return false;
	}

	if (!(action->throttle_i >= -1.0f && action->throttle_i <= 1.0f &&
		action->throttle_j >= -1.0f && action->throttle_j <= 1.0f))
	{
		return false;
	}

	if (action->weapon_index != NONE && (action->weapon_index < 0 || action->weapon_index >= 4))
	{
		return false;
	}
	if (action->grenade_index != NONE && (action->grenade_index < 0 || action->grenade_index >= 4))
	{
		return false;
	}
	if (action->weapon_index != NONE && action->weapon_index == action->grenade_index)
	{
		return false;
	}
	if (action->unknown22 != NONE && (action->unknown22 < 0 || action->unknown22 >= 2))
	{
		return false;
	}
	if (action->unknown24 != NONE && (action->unknown24 < 0 || action->unknown24 >= 4))
	{
		return false;
	}
	if (action->flags1d & 0xfe)
	{
		return false;
	}

	bool valid_values = action->unknown4c.unknown04 >= 0.0f && action->unknown4c.unknown04 <= 1.0f &&
		action->unknown4c.unknown08 >= 0.0f && action->unknown4c.unknown08 <= 1.0f;

	bool active = action->unknown4c.unknown04 != 0.0f || action->unknown4c.unknown08 != 0.0f || (action->unknown4c.flags & 1);

	if (active)
	{
		if (!valid_values || action->unknown34 == NONE)
		{
			return false;
		}
	}
	else if (!valid_values)
	{
		return false;
	}

	s_player_action_target const *target = &action->target;
	if (!target)
		return false;
	short type = (short)target->reference.type;
	if (type < 0 || type >= 9)
		return false;
	if (type != 0 && target->reference.index == NONE)
		return false;
	return true;
}

bool simulation_machine_is_ready(const s_machine_address *address);
void function_b58c0(long index, dword mask);

static inline void player_connection_changed(long player_index)
{
	if (g_55e4d0[g_4e9ae8->engine_index])
	{
		long slot = g_4e9ae8->slots[(short)player_index];

		if (slot != NONE)
		{
			function_b58c0(slot, 0x200);
		}
	}
}

// @retail 0x14d2a0
void players_update_connections(void)
{
	if (g_4e6948->mode != 4)
	{
		s_record_pool_iterator iterator;
		s_player *player;

		iterator.data = g_4e8c24;
		iterator.index = NONE;
		while ((player = (s_player *)data_iterator_next_inlined(&iterator)) != NULL)
		{
			if (player->flags.field_2_2)
			{
				continue;
			}

			bool connected = simulation_machine_is_ready(&player->machine_address);

			if (TEST_FLAG(player->flags_word, 0) != connected)
			{
				SET_FLAG(player->flags_word, 0, connected);
				if (connected)
				{
					c_engine_peer *engine = g_55e4d0[g_4e9ae8->engine_index];
					if (engine)
					{
						engine->p6(iterator.datum_index);
					}
				}
				player_connection_changed(iterator.datum_index);
			}
		}
	}
}

// @retail 0x14dde0
long players_first_active_local_player(void)
{
	s_players_globals *globals = (s_players_globals *)g_4e8c20;
	long result = NONE;

	for (long i = 0; i < 4; i++)
	{
		if (globals->field_x9b462b[i] != NONE)
		{
			result = i;
			break;
		}
	}
	return result;
}

/* out of line, as retail calls it from 0x1391ed, 0x219ed0 and 0x240820; the callers retail
   inlines it into use local_player_next_inline (globals.h) */
__declspec(noinline) long function_14de10(long index);

// @retail 0x14de10
long function_14de10(long index)
{
	long result = NONE;

	if (index == NONE)
	{
		index = 0;
	}
	else
	{
		index++;
	}

	for (long i = index; i < 4; i++)
	{
		if (((s_players_globals *)g_4e8c20)->field_x9b462b[i] != NONE)
		{
			result = i;
			break;
		}
	}
	return result;
}

// @retail 0x14de50
long player_index_from_absolute_index(long index)
{
	return data_datum_index(g_4e8c24, index);
}

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);

struct s_unit_player_view
{
	byte unknown000[0x13c];
	long player_index;
};

// @retail 0x14de90
long unit_get_player_index(long unit_index)
{
	s_unit_player_view *unit = (s_unit_player_view *)function_badc0(unit_index, 3);

	long player_index = NONE;

	if (unit)
	{
		player_index = unit->player_index;
	}
	return player_index;
}

struct s_unit_flags_view
{
	byte unknown000[0x10a];
	word unknown10a_bit0 : 1;
	word unknown10a_bit1 : 1;
	word unknown10a_bit2 : 1;
	word unknown10a_bit3 : 1;
};

static inline byte *object_get_unchecked(long object_index)
{
	return ((s_object_header_entry *)g_4e0300->data)[object_index & 0xffff].object;
}

// @retail 0x14deb0
bool __stdcall function_14deb0(long *unit_index)
{
	s_record_pool_iterator iterator;
	s_player *player;

	iterator.data = g_4e8c24;
	iterator.index = NONE;
	while ((player = (s_player *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		if (player->unit_index != NONE &&
			TEST_FIELD_BIT(((s_unit_flags_view *)object_get_unchecked(player->unit_index))->unknown10a_bit3))
		{
			*unit_index = player->unit_index;
			return true;
		}
	}
	return false;
}

/* the fields of a unit, its vehicle and their root object read below */
struct s_unit_vehicle_view
{
	long definition_index;
	struct
	{
		dword unknown_bits : 18;
		dword bit18 : 1;
	} flags;
	byte unknown08[0x14 - 0x8];
	long parent_index;
	byte unknown18[0xaa - 0x18];
	byte unknownaa;
	byte unknownab[0x348 - 0xab];
	byte unknown348;
	byte unknown349[3];
	byte unknown34c;
};

struct s_vehicle_definition_view
{
	byte unknown000[0x1ec];
	struct
	{
		dword unknown_bits : 6;
		dword bit6 : 1;
	} flags;
};

struct s_tag_instance_view
{
	byte unknown00[8];
	byte *data;
	byte unknown0c[4];
};

long function_baf40(long object_index);
bool function_e4050(long object_index);

/* data_datum_iterator_next (0x6b380), which retail inlines here */
static inline bool player_datum_iterator_next_inlined(s_data_datum_iterator *iterator)
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

// score 43 (was 43): the datum address is still computed in EBX where retail computes it in EDI, and the
// walk is still 0xbaf80 called out of line because another lane's file pins it noinline: BLOCKED-BY 0xbaf80.
// score 43 (was 43): the 44 differing lines are 34 real ones (the rest are the
// unmatched callees and the relocated globals): the datum address is computed in
// EBX where retail computes it in EDI, and the two are used the other way round.
// The walk reads the first parent before the loop and keeps the root in a local,
// retail's inlined 0xbaf80 keeps the entry test and the root in its register:
// BLOCKED-BY 0xbaf80 (another file), so the swap is not ours to fix.
// score 43 (was 43): the record pointer is kept in EBX here and in EDI in
// retail, and the two are used the other way round after the index is masked;
// the walk differs (retail's starts from the unit with the root preset to NONE and keeps its entry test,
// the shape of 0xbaf80 inlined; ours is pinned out of line there: BLOCKED-BY 0xbaf80).
// Round 199: consistent per-use unit, iterator-player and both observations 55/55/55; each with
// retained ancestry pointer 113/113/113 (414 bytes); unsigned parent with unit/both observations
// 55/55. Original body restored; datum/unit roles and root-walk entry remain.
// Round 22 (lane 10): retail walks from the unit itself (root NONE, 0xbaf80's shape) and keeps the
// walk's entry test, with the walk's object pointer in EAX. That walk shape 106 (our compiler
// threads the entry test away); from a re-read unit field 171 and with a dword unit 171 (the test
// returns but the unit takes EAX and the iterator index leaves the frame); with the unit object
// fetched after the walk 100; the in-body inline_depth pair removed 185 (0xbaf40/0xbadc0 inline,
// 508 bytes). Screened (one-file LTCG): the walk as an inline or cross-module function, under a
// nested `if` for the unit test, or with a named object pointer: no entry test, or EAX unit.
// Round 24 (lane 10): the walk through the existing 0xbaf80 126 (400 bytes; it stays a call here, retail
// inlines it); the rest of the loop body nested under `if (unit != NONE)` 55; player/unit declared at
// function scope 55; the walk as a for loop from the parent 55; the unit object fetched after the walk 171.
// Round 294: Selected-handle/return-result common output, signed/unsigned handle, entry/late
// storage and positive/failure completion, all eight 86 (404 bytes). Exact-only
// output copies and complete body restored; player/unit roles and walk entry remain.
// Lane 17 round 2 (kept, 55 -> 43): the e4050 and flag-bit exits as one `||` test (the exits' registers are then
// retail's). Not kept: retail's walk from the unit with a NONE root 106 (player in EDI as in retail, but no entry
// test), with the merged exits 109; a forced-inline copy of the walk (diagnostic) 106; nested 0x34c test 56; named
// root object 56; the parent walk as a for loop 44; root declared before the unit object 46; the walk with its
// load in the loop test 171; one class-dispatch test for both exits 49; else-if after the root exit 44.
// Round 47: unit read before the player local 44; the root walk as a do/while from the unit 109;
// the iterator step through the existing data_datum_iterator_next_inlined 44; exits storing the
// unit local 182; the parent declared before the root 44. Retail inlines 0xbaf80 here (it calls
// it elsewhere); ours keeps it out of line, pinned in another lane's file: BLOCKED-BY 0xbaf80.
// @retail 0x14df40
bool __stdcall function_14df40(long *unit_index)
{
	s_data_datum_iterator iterator;

	iterator.data = g_4e8c24;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while (player_datum_iterator_next_inlined(&iterator))
	{
		s_player *player = (s_player *)iterator.datum;
		long unit = player->unit_index;

		if (unit == NONE)
		{
			continue;
		}

		s_unit_vehicle_view *unit_object = (s_unit_vehicle_view *)object_get_unchecked(unit);
		long root_index = unit;
		long parent = unit_object->parent_index;
		while (parent != NONE)
		{
			root_index = parent;
			parent = ((s_unit_vehicle_view *)object_get_unchecked(root_index))->parent_index;
		}

		if (TEST_FIELD_BIT(((s_unit_vehicle_view *)object_get_unchecked(root_index))->flags.bit18))
		{
			*unit_index = player->unit_index;
			return true;
		}

		if (unit_object->parent_index == NONE)
		{
			if (unit_object->unknownaa == 0)
			{
				if (function_e4050(unit) || (unit_object->unknown348 & 1))
				{
					*unit_index = player->unit_index;
					return true;
				}
			}
			else if (unit_object->unknownaa == 1 && unit_object->unknown34c > 0)
			{
				*unit_index = player->unit_index;
				return true;
			}
		}
		else
		{
#pragma inline_depth(0)
			s_unit_vehicle_view *vehicle = (s_unit_vehicle_view *)function_badc0(function_baf40(unit_object->parent_index), 2);
#pragma inline_depth()

			if (vehicle &&
				TEST_FIELD_BIT(((s_vehicle_definition_view *)g_4e3b44[vehicle->definition_index & 0xffff].bytes)->flags.bit6) &&
				vehicle->unknown34c > 0)
			{
				*unit_index = unit_object->parent_index;
				return true;
			}
		}
	}
	return false;
}

void function_154d70(s_speed_request *request, s_speed_slot *slot, real scale);

static inline void player_speed_request(short user_index, s_speed_request *request)
{
	long index = user_index;

	if (index != NONE)
	{
		function_154d70(request, &g_510c5c->slots[index], 1.0f);
	}
}

// @retail 0x14ea20
void player_speed_request_14ea20(long player_index)
{
	if (player_index != NONE)
	{
		s_player *player = player_get(player_index);

		if (player->user_index != NONE)
		{
			static short type = 5;
			static real duration = 2.0f;
			static short curve;
			static real amount = 1.0f;
			static real scale;
			static real vector_i = 0.8f;
			static real vector_j;
			static real vector_k = 0.8f;
			s_speed_request request = { 0 };

			request.duration = duration;
			request.curve = curve;
			request.amount = amount;
			request.shake.scale = scale;
			request.shake.vector.i = vector_i;
			request.shake.vector.j = vector_j;
			request.type = type;
			request.priority = 2;
			request.shake.vector.k = vector_k;
			player_speed_request(player->user_index, &request);
		}
	}
}

// @retail 0x14eb10
void player_speed_request_14eb10(long player_index)
{
	if (player_index != NONE)
	{
		s_player *player = player_get(player_index);

		if (player->user_index != NONE)
		{
			static short type = 2;
			static real duration = 2.0f;
			static short curve;
			static real amount = 1.0f;
			static real scale;
			static real vector_i = 0.35f;
			static real vector_j = 0.35f;
			static real vector_k;
			s_speed_request request = { 0 };

			request.duration = duration;
			request.curve = curve;
			request.amount = amount;
			request.shake.scale = scale;
			request.shake.vector.i = vector_i;
			request.shake.vector.j = vector_j;
			request.type = type;
			request.priority = 2;
			request.shake.vector.k = vector_k;
			player_speed_request(player->user_index, &request);
		}
	}
}

// @retail 0x14ec00
void player_speed_request_14ec00(long player_index)
{
	if (player_index != NONE)
	{
		s_player *player = player_get(player_index);

		if (player->user_index != NONE)
		{
			s_speed_request request = { 0 };

			request.duration = 2.0f;
			request.curve = 1;
			request.amount = 0.5f;
			request.shake.scale = 1.0f;
			request.type = 6;
			request.priority = 2;
			request.shake.vector.i = 0.9176470041275024f;
			request.shake.vector.j = 0.9176470041275024f;
			request.shake.vector.k = 0.9176470041275024f;
			player_speed_request(player->user_index, &request);
		}
	}
}

struct s_unit_speed_view
{
	byte unknown000[0x264];
	real value264;
	byte unknown268[0x2b0 - 0x268];
	real value2b0;
};

static inline s_unit_speed_view *player_unit_get(long unit_index)
{
	return (s_unit_speed_view *)((s_object_header_entry *)g_4e0300->data)[unit_index & 0xffff].object;
}

// @retail 0x14ece0
bool function_14ece0(void)
{
	s_record_pool_iterator iterator;
	s_player *player;

	iterator.data = g_4e8c24;
	iterator.index = NONE;
	while ((player = (s_player *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		if (player->unit_index != NONE && player_unit_get(player->unit_index)->value2b0 == 1.0f)
		{
			return true;
		}
	}
	return false;
}

// @retail 0x14ed80
bool function_14ed80(void)
{
	s_record_pool_iterator iterator;
	s_player *player;

	iterator.data = g_4e8c24;
	iterator.index = NONE;
	while ((player = (s_player *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		if (player->unit_index != NONE && player_unit_get(player->unit_index)->value264 == 1.0f)
		{
			return true;
		}
	}
	return false;
}


// score 8 (was 8): the global is kept in EDX here and in ESI in retail, which
// also moves the push of ESI in front of the add. Also tried: one declaration
// for both outputs 8, the outputs declared between the player and the
// globals 8 or after the index 8, the globals declared first and assigned
// after the player 8, the outputs assigned after their declarations 8 (third
// first 9), the representation pointer declared at the top 8, const views 8,
// dword outputs 8. The globals pointer always takes the register the player
// lookup frees. Round 41: the clamp through the file's PIN macro 19 (166 bytes); maximum at
// function scope 8; globals declared after the index 8; player declared first 8; the bound
// read from the globals at each use without the maximum local 21.
// Round 43: all 23 other orders of the first/third/player/globals declarations: globals before the
// player 49-50, the rest 8-9; no globals local (the global read at each use) 53 (first then takes
// EDX, the globals EDI); index declared first 8, after the outputs 8; the third-person field assigned
// first 9; renaming the globals or first local 8 (names do not steer it).
// @retail 0x14f040
void player_get_representation(long player_index, long *field_b4, long *third_person)
{
	long first = NONE;
	long third = NONE;
	s_player *player = player_get(player_index);
	s_globals_representations_view *globals = (s_globals_representations_view *)g_4e034c;
	long index;

	if (player->appearance.representation_index < 0)
	{
		index = 0;
	}
	else
	{
		index = player->appearance.representation_index;
		long maximum = globals->representation_count - 1;
		if (index > maximum)
		{
			index = maximum;
		}
	}

	if (g_4e6948->state == 2)
	{
		if (index == 0)
		{
			index = 2;
		}
		else if (index == 1)
		{
			index = 3;
		}
	}

	if (index >= 0 && index < globals->representation_count)
	{
		s_player_representation *representation = &globals->representations[index];

		first = representation->field_b4;
		third = representation->third_person;
	}

	if (field_b4)
	{
		*field_b4 = first;
	}
	if (third_person)
	{
		*third_person = third;
	}
}

void player_control_set_unit(long player_index, long unit_index);
void function_1682bf(long unit_index, long user_index, long representation_index);

/* this and function_14f270 keep retail's stack arguments (ret 8) when
   declared __stdcall, unlike most functions under LTCG; perhaps because they
   are self-recursive */
// @retail 0x14f190
void __stdcall player_set_local_user(long player_index, long user_index)
{
	s_player *player = &((s_player *)g_4e8c24->data)[player_index & 0xffff];

	if (player->user_index != user_index)
	{
		if (user_index == NONE)
		{
			player_control_set_unit(player->user_index, NONE);
			function_1682bf(NONE, player->user_index, NONE);
			((s_players_globals *)g_4e8c20)->field_x9b462b[player->user_index] = NONE;
			((s_players_globals *)g_4e8c20)->unknown08--;
			player->user_index = NONE;
		}
		else
		{
			player_set_local_user(player_index, NONE);
			player->user_index = (short)user_index;
			((s_players_globals *)g_4e8c20)->field_x9b462b[user_index] = player_index;
			((s_players_globals *)g_4e8c20)->unknown08++;
			player_control_set_unit(player->user_index, player->unit_index);

			long representation_index = player->appearance.representation_index;

			if (g_4e6948->state == 2)
			{
				if (representation_index == 0)
				{
					representation_index = 2;
				}
				else if (representation_index == 1)
				{
					representation_index = 3;
				}
			}
			function_1682bf(player->unit_index, player->user_index, representation_index);
		}
	}
}

// @retail 0x14f270
void __stdcall function_14f270(long player_index, long controller_index)
{
	s_player *player = &((s_player *)g_4e8c24->data)[player_index & 0xffff];

	if (player->controller_index != controller_index)
	{
		if (controller_index == NONE)
		{
			((s_players_globals *)g_4e8c20)->unknown1c[player->controller_index] = NONE;
			((s_players_globals *)g_4e8c20)->unknown0a--;
			player->controller_index = NONE;
		}
		else
		{
			function_14f270(player_index, NONE);
			player->controller_index = controller_index;
			((s_players_globals *)g_4e8c20)->unknown1c[controller_index] = player_index;
			((s_players_globals *)g_4e8c20)->unknown0a++;
		}
	}
}

struct s_location;
struct s_effect_owner;
struct s_tail_traits;
struct s_marker_position_angle;
void marker_record_get_position_and_angle(s_marker_position_angle const *, point3f *, real *);
void function_b75a0(long, point3f const *, vector3f const *, vector3f const *, s_location const *, bool);
void function_b9c60(long, bool);
void function_b7290(long);
bool function_b9d20(long);
void __stdcall function_bef30(long, long, long, long, bool);
void function_b8b70(long);
void __stdcall function_b8540(long);
void function_b7930(void *, long, long, s_effect_owner const *);
long __stdcall function_b7b40(void *);
void __stdcall function_a7870(long);
void function_a7a30(long, dword);
void function_15e970(long);
s_tail_traits *function_1529a0(long);
bool function_152bf0(byte const *, long);
void function_cce00(long, short, bool, bool);
void motion_sensor_reset(long);
void function_153b80(long);
void function_152340(void);
void function_14cad0(long player_index, long unit_index);
long function_14ef00(long player_index, long tag_index, bool deterministic, bool preserve_counter);
bool function_15f330(s_player_appearance const *appearance, short team_index, color3f *colors);
void __stdcall function_16f4b0(void *player);

/* the fields of a player (0x21c bytes) that giving it a unit uses */
struct s_player_spawn_view_14f2e0
{
	short salt;
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word flag3 : 1;
	word flag4 : 1;
	word flag5 : 1;
	word flag6 : 1;
	word flag7 : 1;
	word flag8 : 1;
	word flag9 : 1;
	word flag10 : 1;
	word unknown11 : 5;
	byte unknown04[0x28 - 0x4];
	short user_index;
	byte unknown2a[0x34 - 0x2a];
	long kept_unit_index;
	byte unknown38[0x84 - 0x38];
	s_player_appearance appearance;
	byte unknown8c[0xc0 - 0x8c];
	char team_index;
	byte unknownc1[0x17c - 0xc1];
	short field_17c;
	short field_17e;
	byte unknown180[0x194 - 0x180];
	long field_194;
	long field_198;
};

/* the creation data of a new object (0xc4 bytes; 0xb7930 fills in the defaults) */
struct s_object_creation_14f2e0
{
	byte unknown00[0xc];
	long field_0c;
	byte unknown10[0x1c - 0x10];
	point3f position;
	vector3f forward;
	vector3f up;
	byte unknown40[0x5c - 0x40];
	long player_index;
	byte unknown60[0x64 - 0x60];
	long team_index;
	byte unknown68[0x74 - 0x68];
	dword change_color_mask;
	color3f colors[4];
	dword field_a8;
	byte unknownac[0xc4 - 0xac];
};

/* the scenario's starting profiles and starting locations */
struct s_scenario_spawn_view_14f2e0
{
	byte unknown000[0x12];
	byte flags12;
	byte unknown013[0xf8 - 0x13];
	long starting_profile_count;
	void *starting_profiles;
	long starting_location_count;
	s_scenario_starting_profile_view *starting_locations;
};

static inline s_players_globals *players_globals_14f2e0()
{
	return (s_players_globals *)g_4e8c20;
}

/* gives a player its unit (the unit kept for it, the unit waiting to be
   reused, or a new unit at a starting location) and resets its spawn state */
// score 46 (was 46): the NONE outputs are materialised with `mov eax, 0xffffffff` where retail
// uses `or eax, 0xffffffff` (two bytes shorter, so every later stack address differs), and the
// zero/flag store schedule differs; the success BL agrees. Round 29 (lane G): with the two
// outputs declared and assigned separately and the timers cleared with a memset the score is 43,
// but the constant stays a mov in every form tried (a struct of the two outputs, one and two
// declarations, chained and separate assignments, ~0 and -1 spellings), so the gain is under the
// keep threshold and the original body is restored.
// score 46 (was 46): NONE materialization and the zero/flag store schedule differ; success BL agrees.
// Round 39: joint/dependent output initialization 75/75; chained word/flag clears 75/75;
// result before/after the final reset 60/75; after field_198/before the user tests 46/46;
// early result with chained NONE 48; with chained words/flags or flags first 46/46/46;
// shared field zero 46; word clears from field_198 58. Kept before the user tests; plain 46.
// Shared direction helper for reuse/create/both 251/201/276; creation/colors/both entry scopes
// 152/114/152; native/byte success 174/177; sentinel clears 114;
// representation inline/out-of-line declarations 114/114.
// Earlier: BLOCKED-BY 0x152bf0, whose retail body takes the loadout in eax and the unit in
// esi (ours ecx/eax): without that call the unit takes retail's esi (diagnostic). `result = true`
// before the user-index calls gives retail's bl result but 147 (one-line shift); at the tail start
// 147; globals and reuse flag declared before the guard 114; chained NONE outputs 116; unit index
// initialised NONE 114.
// Round 199: actual reusable-unit query pointer through admission/fallback; conditional/guarded
// acquisition, direct global observations and early final success:
// 170/163/203/232/198/191/206/235. All restored; the pointer lifetime changes the frame and does
// not fix the protected spawned-unit convention.
// Round 22 (lane 10): only two real differences are left. The 0x198 field is the player's two
// short timers (src/unknown_152000.cpp's view); clearing them with memset gives retail's tail
// order exactly (43; not kept, a 3-point gain). The other is the NONE outputs' constant: ours
// `mov eax, 0xffffffff`, retail `or eax, -1` (2 bytes, so every later address is flagged). One-file
// LTCG screens: our build compares the mode byte through AL, and a partial AL write makes the
// constant a mov (a dword compare or no mode test gives `or`). With the memset: globals read at the
// top 290, chained outputs 45, outputs swapped 45, result declared first 43, kept-unit result before
// its call 43. Screened, all `mov`: one-declaration/separate/chained outputs, nested result test,
// mode-first or negated tests, long/char mode locals, byte-typed mode, a mode predicate, unit
// initialised NONE, reused flag first, location declared first, long location, swapped bit arms.
// Round 47: with the timers' memset the only real difference left is that `mov eax, -1` (masked
// diff one line, 43). Ours loads the mode byte into AL at the join before folding it into
// `cmp byte [edx+0xc], 4`, so the next -1 is a mov; any word/dword compare, or no mode test,
// gives `or`. Real trials: outputs declared and set at the top 275; set just before the test
// 268; declared at the top, set in the block 46; timers' memset 43; nested result and mode tests
// 46. Diagnostics: 0xb8540 (a stub) replaced by a matched call 244 / removed 259, both still mov.
// Compile-only screens, all mov: byte cast, nested, negated, De Morgan, named bool, long cast,
// swapped operands, mode local, `result == false`, unit/result declaration order, unit NONE,
// options pointer local, kept index cleared after its test, hidden-bit local, result set before
// 0x14cad0; bitfield/unsigned/char mode fields in a small model.
// @retail 0x14f2e0
bool __stdcall function_14f2e0(long player_index)
{
	s_player_spawn_view_14f2e0 *player = (s_player_spawn_view_14f2e0 *)player_get(player_index);
	bool result = false;
	long unit_index;

	if (g_4e6948->state == 1)
	{
		unit_index = player->kept_unit_index;
		player->kept_unit_index = NONE;
		if (unit_index != NONE)
		{
			if (TEST_FIELD_BIT(((s_unit_flags_view *)object_get_unchecked(unit_index))->unknown10a_bit2))
			{
				function_b8540(unit_index);
			}
			else
			{
				function_b7290(unit_index);
				byte *unit = object_get_unchecked(unit_index);
				if (unit[4] & 1)
				{
					*(dword *)(unit + 4) &= ~1;
					if (function_b9d20(unit_index))
					{
						function_bef30(unit_index, 0, 1, 0, false);
					}
					function_b8b70(unit_index);
				}
				function_14cad0(player_index, unit_index);
				result = true;
			}
		}
	}

	if (!result && g_4e6948->mode != 4)
	{
		long tag_index = NONE;
		long third_person = NONE;

		player_get_representation(player_index, &tag_index, &third_person);
		short location_index = (short)function_14ef00(player_index, tag_index, true, false);
		if (location_index == NONE || tag_index == NONE)
		{
			goto done;
		}

		s_players_globals *globals = players_globals_14f2e0();
		bool reused = false;

		if (globals->unknownac != NONE && function_badc0(globals->unknownac, 3))
		{
			s_scenario_spawn_view_14f2e0 *scenario = (s_scenario_spawn_view_14f2e0 *)g_4e0350;
			s_marker_position_angle const *location = (s_marker_position_angle const *)&scenario->starting_locations[location_index];
			point3f position;
			real facing;
			vector3f forward;

			marker_record_get_position_and_angle(location, &position, &facing);
			forward.i = (real)cos(facing);
			forward.j = (real)sin(facing);
			forward.k = 0.0f;
			function_b75a0(players_globals_14f2e0()->unknownac, &position, &forward, g_4687b0, NULL, false);
			globals = players_globals_14f2e0();
			function_b9c60(globals->unknownac, false);
			function_b7290(globals->unknownac);
			unit_index = players_globals_14f2e0()->unknownac;
			players_globals_14f2e0()->unknownac = NONE;
			reused = true;
		}
		else
		{
			s_scenario_spawn_view_14f2e0 *scenario = (s_scenario_spawn_view_14f2e0 *)g_4e0350;
			s_marker_position_angle const *location = (s_marker_position_angle const *)&scenario->starting_locations[location_index];
			s_object_creation_14f2e0 creation;
			real facing;
			color3f colors[4];

			function_b7930(&creation, tag_index, NONE, NULL);
			creation.field_0c = third_person;
			marker_record_get_position_and_angle(location, &creation.position, &facing);
			creation.forward.i = (real)cos(facing);
			creation.forward.j = (real)sin(facing);
			creation.forward.k = 0.0f;
			creation.up = *g_4687b0;
			creation.player_index = player_index;
			creation.team_index = player->team_index;
			creation.field_a8 = *(dword *)((byte *)player + 0x89);
			if (function_15f330(&player->appearance, player->team_index, colors))
			{
				creation.change_color_mask |= 0xf;
				memcpy(creation.colors, colors, sizeof(colors));
			}
			unit_index = function_b7b40(&creation);
		}

		if (unit_index == NONE)
		{
			goto done;
		}
		function_14cad0(player_index, unit_index);
		function_a7870(unit_index);
		function_a7a30(unit_index, 0x400);
		if (g_4e6948->state == 2)
		{
			function_15e970(unit_index);
		}
		else if (!reused)
		{
			s_scenario_spawn_view_14f2e0 *scenario = (s_scenario_spawn_view_14f2e0 *)g_4e0350;
			bool initial = !TEST_FIELD_BIT(player->flag3);
			long profile_index = initial ? 1 : 0;
			bool loaded = false;

			if (profile_index > scenario->starting_profile_count - 1)
			{
				profile_index = scenario->starting_profile_count - 1;
			}
			if (!initial && (scenario->flags12 & 0x20))
			{
				s_tail_traits *loadout = function_1529a0(player_index);
				if (loadout)
				{
					loaded = function_152bf0((byte const *)loadout, unit_index);
				}
			}
			if (!loaded && profile_index >= 0 && profile_index < scenario->starting_profile_count)
			{
				function_cce00(unit_index, (short)profile_index, true, initial);
			}
		}
	}

	player->field_198 = 0;
	player->flag9 = false;
	player->flag10 = false;
	player->field_17c = 0;
	player->field_17e = 0;
	result = true;
	if (player->user_index != NONE)
	{
		motion_sensor_reset(player->user_index);
	}
	player->field_194 = g_510c54->field_2_3;
	if (player->user_index != NONE)
	{
		function_153b80(player->user_index);
	}
	if (player->user_index != NONE)
	{
		function_16f4b0(&g_4e9bd4[player->user_index]);
	}
	function_152340();
	player->flag3 = false;

done:
	return result;
}

/* sets the local machine: the players lose their local users and controllers,
   and the local machine's players take theirs */
// @retail 0x14c880
void players_set_local_machine(s_machine_address const *machine_address)
{
	bool valid = machine_address != NULL;
	long machine_index = NONE;

	if (machine_address != NULL)
	{
		s_players_globals *globals = (s_players_globals *)g_4e8c20;

		{ long i = 0; if (i < 16) { do {
			if (TEST_FLAG(globals->machine_valid_mask, i) &&
				memcmp(machine_address, &globals->machine_addresses[i], sizeof(s_machine_address)) == 0)
			{
				machine_index = i;
			}
		
i++; } while (i < 16); } }
	}

	s_players_globals *globals = (s_players_globals *)g_4e8c20;

	if (valid != globals->local_machine_valid ||
		valid && memcmp(machine_address, &globals->local_machine_address, sizeof(s_machine_address)) != 0 ||
		machine_index != globals->local_machine_index)
	{
		s_record_pool_iterator iterator;
		s_player *player;

		iterator.data = g_4e8c24;
		iterator.index = NONE;
		while ((player = (s_player *)data_iterator_next_inlined(&iterator)) != NULL)
		{
			player_set_local_user(iterator.datum_index, NONE);
			function_14f270(iterator.datum_index, NONE);
		}

		globals = (s_players_globals *)g_4e8c20;
		globals->local_machine_valid = valid;
		globals->local_machine_index = machine_index;
		if (valid)
		{
			globals->local_machine_address = *machine_address;
		}
		else
		{
			memset(&globals->local_machine_address, 0, sizeof(s_machine_address));
		}

		if (globals->local_machine_index != NONE)
		{
			iterator.data = g_4e8c24;
			iterator.index = NONE;
			while ((player = (s_player *)data_iterator_next_inlined(&iterator)) != NULL)
			{
				if (player->machine_index == ((s_players_globals *)g_4e8c20)->local_machine_index)
				{
					player_set_local_user(iterator.datum_index, player->machine_user_index);
					function_14f270(iterator.datum_index, player->machine_controller_index);
				}
			}
		}
	}
}

/* buttons held since the last tick don't press again, and two flags latch
   their presses and releases */
// @retail 0x14f6f0
void player_action_update_latches(long player_index, s_player_action *action)
{
	s_player *player = player_get(player_index);
	dword buttons = action->buttons;
	bool primary_was_down;
	bool primary_down;
	bool secondary_was_down;
	bool secondary_down;

	action->buttons = buttons & ~player->latched_buttons;
	player->latched_buttons = buttons & 0x16100074;
	primary_down = TEST_FLAG(action->flags, 0);
	secondary_down = TEST_FLAG(action->flags, 4);
	primary_was_down = TEST_FLAG(player->latched_flags, 0);
	secondary_was_down = TEST_FLAG(player->latched_flags, 4);

	SET_FLAG(action->flags, 2, primary_down && !primary_was_down);
	SET_FLAG(action->flags, 3, !primary_down && primary_was_down);
	SET_FLAG(player->latched_flags, 0, primary_down);
	SET_FLAG(action->flags, 6, secondary_down && !secondary_was_down);
	SET_FLAG(action->flags, 7, !secondary_down && secondary_was_down);
	SET_FLAG(player->latched_flags, 4, secondary_down);
	if (TEST_FLAG(player->flags_word, 4))
	{
		action->flags &= ~0xf;
		SET_FLAG(player->flags_word, 4, primary_down);
	}
	if (TEST_FLAG(player->flags_word, 5))
	{
		action->flags &= ~0xf0;
		SET_FLAG(player->flags_word, 5, secondary_down);
	}
}

void function_13ac42(long index);
void __stdcall function_cbf60(long unit_index, bool active);
void function_1584c0(long index);
real function_1588b0(long player_index, long type);
void function_152340(void);

struct s_unit_player_assignment
{
	byte unknown00[0xd4];
	long field_d4;
	byte unknownd8[0xf0 - 0xd8];
	real field_f0;
	byte unknownf4[0x13c - 0xf4];
	long player_index;
	long previous_player_index;
};

// score 17 (was 17): post-call scratch registers and the representation push schedule differ;
// the second new-unit lookup uses EDX for its object pointer, and the final engine load is one byte shorter.
// Inline notification 17; inline control set 17; force object getter 17; unsigned new target 17;
// unsigned representation 17; unit references 17; player reference 17; second lookup local 17;
// engine globals local 17.
// Tried: the engine test through game_engine_get 17; the state test in each
// representation arm 45; the representation declared before the call 17;
// the stored unit index field passed to function_1682bf 16. Round 106: the representation local
// renamed 17; the existing first_person_character_from_player for the remap 17, the existing
// function_a7a30 for both +0xd4 notifications 17, and both 17 (LTCG inlines each: same code);
// block-scoped unit pointers 17.
// Round 43: the control-set argument read back from player->unit_index 17; both +0xd4 fields read
// at each use 17; both together 17; the outer comparison's operands swapped 18; the remap as one
// conditional expression 17; the unit pointer declared before the player 17. Matched 0x14f190
// makes the same 0x1682bf call with our schedule.
// @retail 0x14cad0
void function_14cad0(long player_index, long unit_index)
{
	s_player *player = player_get(player_index);
	s_unit_player_assignment *unit;
	if (player->unit_index != unit_index)
	{
		if (player->unit_index != NONE)
		{
			unit = (s_unit_player_assignment *)object_get_unchecked(player->unit_index);
			if (player->user_index != NONE)
			{
				player_control_set_unit(player->user_index, NONE);
				function_1682bf(NONE, player->user_index, NONE);
			}
			unit->player_index = NONE;
			unit->previous_player_index = player_index;
			long target = ((s_unit_player_assignment *)object_get_unchecked(player->unit_index))->field_d4;
			if (target != NONE)
			{
				function_b58c0(target, 0x400);
			}
			function_cbf60(player->unit_index, false);
			player->unit_index = NONE;
		}
		if (player->user_index != NONE)
		{
			function_13ac42(player->user_index);
		}
		player->latched_buttons = 0;
		player->latched_flags = 0;
		player->field_17c = 0;
		player->field_17e = 0;
		if (unit_index != NONE)
		{
			unit = (s_unit_player_assignment *)object_get_unchecked(unit_index);
			unit->player_index = player_index;
			unit->previous_player_index = NONE;
			long target = ((s_unit_player_assignment *)object_get_unchecked(unit_index))->field_d4;
			if (target != NONE)
			{
				function_b58c0(target, 0x400);
			}
			function_cbf60(unit_index, true);
			player->unit_index = unit_index;
			player->previous_unit_index = NONE;
			if (player->user_index != NONE)
			{
				player_control_set_unit(player->user_index, unit_index);
				long representation = player->appearance.representation_index;
				if (g_4e6948->state == 2)
				{
					if (representation == 0)
					{
						representation = 2;
					}
					else if (representation == 1)
					{
						representation = 3;
					}
				}
				function_1682bf(unit_index, player->user_index, representation);
				function_1584c0(player->user_index);
			}
			if (g_55e4d0[g_4e9ae8->engine_index])
			{
				unit->field_f0 = function_1588b0(player_index, 2);
			}
		}
		function_152340();
	}
}

void function_196470(void);
void function_bb8f0(long player_index);

// @retail 0x14c540
void function_14c540(long player_index)
{
	if (g_4e6948->state == 1)
	{
		for (long i = 0; i < MAXIMUM_AI_PLAYERS; i++)
		{
			if (g_4f55cc[i].player_index == player_index)
			{
				g_4f55cc[i].player_index = NONE;
			}
		}
	}
	function_14cad0(player_index, NONE);
	player_set_local_user(player_index, NONE);
	function_14f270(player_index, NONE);
	record_pool_release(g_4e8c24, player_index);
	if (g_55e4d0[g_4e9ae8->engine_index])
	{
		long slot = g_4e9ae8->slots[(short)player_index];
		if (slot != NONE)
		{
			function_b58c0(slot, 0x7ff);
		}
		function_196470();
	}
	function_bb8f0(player_index);
}


bool function_bacc0(long object_index, long index, point3f const *point);
void __stdcall function_be240(long object_index, dword color_mask, color3f const *colors);
bool function_15f330(s_player_appearance const *appearance, short team_index, color3f *colors);

// @retail 0x14bfc0
void function_14bfc0(long player_index, long object_index)
{
    s_player *player = player_get(player_index);
    byte *object = object_get_unchecked(object_index);
    if ((1 << object[0xaa]) & 3)
        *(short *)(object + 0x138) = (short)*(char *)((byte *)player + 0xc0);
    *(long *)(object + 0x24) = *(long *)((byte *)player + 0x89);
    color3f colors[4];
    if (function_15f330(&player->appearance, (short)*(char *)((byte *)player + 0xc0), colors))
    {
        function_bacc0(object_index, 0, (point3f *)&colors[0]);
        function_bacc0(object_index, 1, (point3f *)&colors[1]);
        function_bacc0(object_index, 2, (point3f *)&colors[2]);
        function_bacc0(object_index, 3, (point3f *)&colors[3]);
    }
    else
        function_be240(object_index, 0, NULL);
}


void function_158000(long previous_index, long player_index);
void ai_player_add(long player_index);
struct s_object_values;
void function_1e9e80(long first, s_object_values *table, long second);
void function_152df0(long player_index);

static inline s_player *player_get_absolute(long index)
{
    s_player *result = NULL;
    s_record_pool *data = g_4e8c24;
    if (index != NONE && index >= 0 && index < data->high_water_index)
    {
        s_player *player = (s_player *)(data->data + data->size * index);
        if (player->salt != 0)
            result = player;
    }
    return result;
}

// @retail 0x14c320
void function_14c320(long first, long second)
{
    long const *first_reference = &first;
    long const *second_reference = &second;
    s_player *first_player = player_get_absolute(*first_reference);
    s_player *second_player = player_get_absolute(*second_reference);
    long first_old = NONE;
    long first_new = NONE;
    long second_old = NONE;
    long second_new = NONE;
    s_player first_copy;
    s_player second_copy;
    if (first_player)
    {
        first_old = data_datum_index(g_4e8c24, *first_reference);
        first_copy = *first_player;
        record_pool_release(g_4e8c24, first_old);
    }
    if (second_player)
    {
        second_old = data_datum_index(g_4e8c24, *second_reference);
        second_copy = *second_player;
        record_pool_release(g_4e8c24, second_old);
    }
    if (first_old != NONE)
    {
        first_new = function_16b990(g_4e8c24, *second_reference);
        s_player *player = player_get(first_new);
        *player = first_copy;
        player->salt = (short)(first_new >> 16);
    }
    if (second_old != NONE)
    {
        second_new = function_16b990(g_4e8c24, *first_reference);
        s_player *player = player_get(second_new);
        *player = second_copy;
        player->salt = (short)(second_new >> 16);
    }
    if (g_55e4d0[g_4e9ae8->engine_index])
        function_1e9e80(*first_reference, (s_object_values *)((byte *)g_4e9ae8 + 0x304), *second_reference);
    if (first_old != NONE)
    {
        function_158000(first_old, first_new);
        if (g_4e6948->state == 1)
        {
            for (long i = 0; i < MAXIMUM_AI_PLAYERS; i++)
                if (g_4f55cc[i].player_index == first_old)
                    g_4f55cc[i].player_index = NONE;
        }
        ai_player_add(first_new);
    }
    if (second_old != NONE)
    {
        function_158000(second_old, second_new);
        if (g_4e6948->state == 1)
        {
            for (long i = 0; i < MAXIMUM_AI_PLAYERS; i++)
                if (g_4f55cc[i].player_index == second_old)
                    g_4f55cc[i].player_index = NONE;
        }
        ai_player_add(second_new);
    }
}

// @retail 0x14c630
void function_14c630(dword valid_mask, s_machine_address const *addresses)
{
    dword const *valid_reference = &valid_mask;
    s_machine_address const *const *addresses_reference = &addresses;
    s_players_globals *globals = (s_players_globals *)g_4e8c20;
    dword old_valid = globals->machine_valid_mask;
    s_machine_address old_addresses[16];
    long remapping[16];
    memcpy(old_addresses, globals->machine_addresses, sizeof(old_addresses));
    memset(remapping, NONE, sizeof(remapping));
    for (long i = 0; i < 16; i++)
    {
        if (TEST_FLAG(old_valid, i))
        {
            for (long j = 0; j < 16; j++)
            {
                if (TEST_FLAG(*valid_reference, j) &&
                    memcmp(&(*addresses_reference)[j], &old_addresses[i], sizeof(s_machine_address)) == 0)
                {
                    remapping[i] = j;
                    break;
                }
            }
        }
    }
    s_record_pool_iterator iterator;
    s_player *player;
    iterator.data = g_4e8c24;
    iterator.index = NONE;
    while ((player = (s_player *)data_iterator_next_inlined(&iterator)) != NULL)
    {
        if (player->machine_index != NONE && remapping[player->machine_index] == NONE)
            function_152df0(iterator.datum_index);
    }
    globals = (s_players_globals *)g_4e8c20;
    globals->machine_valid_mask = *valid_reference;
    memcpy(globals->machine_addresses, *addresses_reference, sizeof(globals->machine_addresses));
    iterator.data = g_4e8c24;
    iterator.index = NONE;
    while ((player = (s_player *)data_iterator_next_inlined(&iterator)) != NULL)
    {
        if (player->machine_index != NONE)
        {
            long new_index = remapping[player->machine_index];
            if (new_index != NONE)
                player->machine_index = (short)new_index;
        }
    }
    if (globals->local_machine_valid)
    {
        long machine_index = NONE;
        for (long i = 0; i < 16; i++)
        {
            if (TEST_FLAG(*valid_reference, i) &&
                memcmp(&(*addresses_reference)[i], &globals->local_machine_address, sizeof(s_machine_address)) == 0)
            {
                machine_index = i;
                break;
            }
        }
        if (machine_index != NONE)
        {
            if (globals->local_machine_index == NONE)
                players_set_local_machine(&globals->local_machine_address);
            else
                globals->local_machine_index = machine_index;
        }
    }
}

#include <wchar.h>
void function_158090(long player_index, short old_team);
void function_1582d0(long player_index, short old_team);
void function_1583d0(short old_value, short new_value, long player_index);

void __stdcall function_14be90(long player_index, dword const *configuration);

static __forceinline void player_configuration_apply(long player_index, dword const *configuration)
{
    s_player *player = player_get(player_index);
    s_type_b07538 *current = (s_type_b07538 *)((byte *)player + 0x44);
    s_type_b07538 previous = *current;
    *(s_type_b07538 *)((byte *)player + 0xd4) = *(s_type_b07538 const *)configuration;
    s_type_b07538 verified = *(s_type_b07538 const *)configuration;
    player_configuration_verify(&verified);
    function_158090(player_index, verified.team_index);
    *current = verified;
    wcsncmp((wchar_t const *)&previous, (wchar_t const *)current, 0x20);
    if (previous.team_index != *(char *)((byte *)player + 0xc0))
        function_1582d0(player_index, *(char *)((byte *)player + 0xc0));
    if (previous.unknown7d != *(char *)((byte *)player + 0xc1))
        function_1583d0(previous.unknown7d, *(char *)((byte *)player + 0xc1), player_index);
    if (player->unit_index != NONE)
        function_14bfc0(player_index, player->unit_index);
}

// @retail 0x14be90
void __stdcall function_14be90(long player_index, dword const *configuration)
{
	player_configuration_apply(player_index, configuration);
}

struct s_spawn_influence
{
    point3f point;
    real value0c, value10, value14, value18, value1c;
};
struct s_spawn_influence_list
{
    long count;
    s_spawn_influence influences[0x80];
};
void function_23bc40(long player_index, s_spawn_influence_list *list);
real function_23ba90(point3f const *point, long player_index, bool deterministic,
    s_spawn_influence_list *list, dword *seed);
bool function_23bb70(long tag_index, point3f const *position);

// @retail 0x14ef00
long function_14ef00(long player_index, long tag_index, bool deterministic, bool preserve_counter)
{
    s_player *player = player_get(player_index);
    s_scenario_starting_profiles_view *scenario = (s_scenario_starting_profiles_view *)g_4e0350;
    s_game_options_view *options = g_4e6948;
    short result = NONE;
    real best = -1000.0f;
    dword seed = ((1 << (player_index & 0xffff)) ^ options->id_b ^ options->id_a) + player->unknown218;
    s_spawn_influence_list influences;
    function_23bc40(player_index, &influences);
    for (short index = 0; index < scenario->starting_profile_count; ++index)
    {
        s_scenario_starting_profile_view *profile = &scenario->starting_profiles[index];
        real score = function_23ba90((point3f const *)profile, player_index, deterministic, &influences, &seed);
        if (score > best && function_23bb70(tag_index, (point3f const *)profile))
        {
            result = index;
            best = score;
        }
    }
    if (!preserve_counter && result != NONE)
        ++player->unknown218;
    return result;
}

#include "object_queries.h"
void function_11bed0(s_location *location, point3f const *point);
void function_3f500(long cluster_index);
extern "C" dword __stdcall GetTickCount();
dword g_55e738;

// @retail 0x14f0f0
void function_14f0f0(long player_index)
{
    if (*(short *)((byte *)player_get(player_index) + 0x28) != NONE)
    {
        dword now = GetTickCount();
        if (now > g_55e738)
        {
            long tag_index = NONE;
            player_get_representation(player_index, &tag_index, 0);
            long profile_index = function_14ef00(player_index, tag_index, false, true);
            if (profile_index != NONE)
            {
                s_scenario_starting_profiles_view *scenario = (s_scenario_starting_profiles_view *)g_4e0350;
                s_location location;
                function_11bed0(&location, (point3f const *)&scenario->starting_profiles[profile_index]);
                if (location.cluster_index != NONE)
                    function_3f500(location.cluster_index);
            }
            g_55e738 = now + 250;
        }
    }
}

#pragma pack(push, 2)
struct s_player_creation_record
{
    bool active;
    bool field_2_2;
    short controller_index;
    long value04;
    s_machine_address machine;
    dword key[3];
    byte unknown1a[2];
    dword configuration[0x24];
    byte unknownac[0xe4 - 0xac];
};
#pragma pack(pop)

// score 0 (was 33): matches retail (plain check): the reset of the player's controller is written
// out in the NONE arm, where retail has 0x14f270's reset expanded, and the other arm's call to
// 0x14f270 passes the controller through a local, so the compiler leaves the call out of line as
// retail does instead of specializing it on the literal. Both forms (the written-out reset and the
// one-use local) are retail's code shape and are kept only with this byte match.
// The rounds that led here are kept below.
// score 33 (was 33): retail inlines function_14f270(index, record->value04) here one level (its NONE
// arm inline, its recursive NONE call kept); our link keeps that variable-argument call out of line
// (it inlines only constant-NONE calls), so the tail is written out and differs from +0x1f6.
// Tried: a plain call with an inline declaration of 0x14f270 267; with inline_recursion pragmas or a
// reworded 0x14f270 the call is still kept. Frame 0x10 agrees; the controller reset expands on the
// opposite arm. Caller depth 1/2 33/33; size preference 335; common reset 55; auto-inline off 33;
// global optimization off 395; depth 0 305; common reset with size preference 343. Written in
// function_14f270's arm order, the NONE call in the other arm inlines instead (49).
// Round 199: direct previous-controller comparison on the current partial-helper graph, with
// native current-player fields, requested-field observations and assignment-first arms:
// 33/33/198/198/47/47/195/195. All restored.
// Round 98: plain call 267; with 0x14f270 reading player_get 267; `inline` on 0x14f270's definition
// 267; an old-controller local in 0x14f270, alone/with `inline` 267/267; 0x14f270 __forceinline
// (definition, or a later declaration in this file) MATCHES here but also inlines 0x14c880's
// variable call (278), so it is not kept: retail inlines the call here and not in 0x14c880's loop.
// frame 0x10 and everything up to the controller block agree. Retail inlines
// 0x14f270(index, record->value04) one level here (its NONE arm inline, its recursive (index, NONE)
// call kept), as 0x152f80 also shows; our build keeps every non-loop variable call to 0x14f270 out
// of line and inlines every constant-NONE call, so the written-out block expands on the opposite
// arm, and the index pushed for 0x14f190 sits in EDI where retail has EDX.
// reworded 0x14f270 the call is still kept; written in 0x14f270's arm order 49; common reset 55;
// caller depth 1/2 33/33; size preference 335; auto-inline off 33; global optimization off 395;
// depth 0 305; common reset with size preference 343. Round 199: direct previous-controller
// comparison, native current-player fields, requested-field observations and assignment-first arms
// 33/33/198/198/47/47/195/195. Round 22 (lane 10): a plain call with `inline` on 0x14f270's
// definition 267 (still a call); constant NONE in the NONE arm's call 59 (both arms inline); a
// globals pointer local in the set arm 33; the current controller read in the test 33; the
// requested controller read from the record at each use 198. Round 24 (lane 10): the controller block
// through the earlier player pointer 37; the change test as `controller != old_controller` 33; the requested
// controller read after the current one 38; the set arm's player store after the globals updates 34; the set
// arm first 47 (as the earlier assignment-first arms).
// Round 47: the body reduced to the bare call (diagnostic) still calls 0x14f270, so its refusal
// is 0x14f270's own cost for a variable argument, not this caller's size. With the plain call:
// 0x14f270 with a globals local per arm 267; that plus `inline` 267; through player_get plus
// the locals 267; its arms swapped 267 (0x14f270 itself 15); the locals with the current
// written-out block 33. Retail also expands both calls in 0x152f80 (ours calls the variable one).
// Round 319: Empty-old-controller reset bypass with live old-field reads, install-first
// arm and shared globals receiver, all eight subsets 42 each (650/650 bytes,
// first +0x210). All restored; opposite recursive expansion and requested homes remain.
// Round 33 (m3 lane 11), fast, the plain call each time (0x14c880 and 0x14f270 matched unless
// noted): plain 267; 0x14f270 without its explicit convention 268 (0x14f270 28); 0x14f270 calling
// player_set_local_user instead of itself 268 (0x14c880 then inlines its variable call, 360); a
// bare body holding only the call still calls it (diagnostic); inline_recursion on from 0x14f270
// (diagnostic) 268; 0x14f270 with an early return 267; 0x14f270 with one function-scope globals
// local 267 (0x14f270 36). Our link never inlines a variable-argument
// 0x14f270 into this caller, recursive or not, though 0x14c880 takes a non-recursive one.
// Round 136 (lane 4, retail's address order puts 0x14f190/0x14f270 after this function): 0x14f270 moved to the end of
// the file + the plain call 267; moved with the written block 33; moved right after this function + the plain call 267,
// with `inline` on it 267; 0x14f190 and 0x14f270 both moved after it + the plain call 267 (the variable call stays out
// of line in every order).
// @retail 0x14c090
void __stdcall function_14c090(long index, bool reuse, s_player_creation_record const *record)
{
    byte *player = (byte *)player_get(index);
    long machine_index = NONE;
    if (!record->field_2_2)
    {
        s_players_globals *globals = (s_players_globals *)g_4e8c20;
        for (long i = 0; i < 16; ++i)
        {
            if ((globals->machine_valid_mask & (1 << i)) &&
                memcmp(&record->machine, &globals->machine_addresses[i], sizeof(record->machine)) == 0)
            {
                machine_index = i;
                break;
            }
        }
    }
    s_record_pool_iterator iterator;
    iterator.data = g_4e8c24;
    iterator.index = NONE;
    while (data_iterator_next_inlined(&iterator)) {}
    *(word *)(player + 2) = 0;
    *(long *)(player + 0x10) = NONE;
    if (reuse) *(word *)(player + 2) = 8; else *(word *)(player + 2) = 0;
    memcpy(player + 4, record->key, sizeof(record->key));
    *(short *)(player + 0x1a) = (short)machine_index;
    memcpy(player + 0x14, &record->machine, sizeof(record->machine));
    *(short *)(player + 0x1c) = record->controller_index;
    *(long *)(player + 0x20) = record->value04;
    player[0x3e] = 0;
    *(short *)(player + 0x17e) = 0;
    *(short *)(player + 0x17c) = 0;
    *(long *)(player + 0x194) = 0;
    *(long *)(player + 0x18c) = 0;
    player[0x1a7] = 0;
    *(long *)(player + 0x190) = 0;
    *(short *)(player + 0x28) = NONE;
    *(long *)(player + 0x24) = NONE;
    *(long *)(player + 0x2c) = NONE;
    *(long *)(player + 0x30) = NONE;
    *(long *)(player + 0x34) = NONE;
    *(short *)(player + 0x2a) = NONE;
    *(long *)(player + 0x174) = NONE;
    *(long *)(player + 0x184) = NONE;
    *(short *)(player + 0x188) = NONE;
    *(real *)(player + 0x19c) = 1.0f;
    *(short *)(player + 0x1a4) = NONE;
    *(long *)(player + 0x1a0) = NONE;
    *(long *)(player + 0x1b0) = NONE;
    *(char *)(player + 0x1a8) = NONE;
    short *mark = (short *)(player + 0x1d8);
    long count = 8;
    do
    {
        *mark = NONE;
        mark += 4;
    } while (--count);
    function_14be90(index, record->configuration);
    if (*(short *)(player + 0x1a) != NONE &&
        *(short *)(player + 0x1a) == ((s_players_globals *)g_4e8c20)->local_machine_index)
    {
        player_set_local_user(index, record->controller_index);
        long controller = record->value04;
        byte *current = (byte *)player_get(index);
        long old_controller = *(long *)(current + 0x24);
        if (old_controller != controller)
        {
            if (controller == NONE)
            {
                ((s_players_globals *)g_4e8c20)->unknown1c[old_controller] = NONE;
                --((s_players_globals *)g_4e8c20)->unknown0a;
                *(long *)(current + 0x24) = NONE;
            }
            else
            {
                long none_controller = NONE;
                function_14f270(index, none_controller);
                *(long *)(current + 0x24) = controller;
                ((s_players_globals *)g_4e8c20)->unknown1c[controller] = index;
                ++((s_players_globals *)g_4e8c20)->unknown0a;
            }
        }
    }
    if (record->field_2_2)
    {
        ((s_player *)player)->flags_word |= 2;
        *(long *)(player + 0x10) = g_510c54->game_time;
    }
}


#include "unit_requests.h"
#include <math.h>
struct s_target_candidate;
bool function_151c10(long, s_target_candidate *);
bool function_151c90(long, s_target_candidate *);
bool function_151940(long, s_target_candidate *, bool);
bool function_151aa0(long, bool);
bool function_151b80(long, bool);
bool function_0c7070(long);
void function_a9390(long);
void function_a8dd0(long);
void function_1628f0(long, char);
void function_151ea0(long, long);
void function_ccd20(long);
bool function_1012c0(long);
bool function_101300(long);
bool function_101340(long);
bool function_101160(long);
bool unit_animation_control_channel_playing(long);
bool function_cc3c0(long);
bool function_e4050(long);
s_object *function_bae20(long, dword);
point3f *function_b9dd0(long, point3f *);

static __forceinline long player_current_weapon(long unit_index, long hand)
{
    byte *unit = object_get_unchecked(unit_index);
    short slot = *(signed char *)(unit + 0x212 + hand);
    return slot == NONE ? NONE : *(long *)(unit + 0x218 + slot*4);
}
static __forceinline void player_clear_weapon_view(long player_index)
{
    short user = player_get(player_index)->user_index;
    if (user != NONE)
    {
        byte *view = (byte *)g_4ed284 + user*0x94 + 0x14;
        if (view[0x8e]) view[0x8f] = true;
        *(short *)(view + 0x28) = NONE;
    }
}
static __forceinline long player_round_ticks(real value)
{
    long result;
    __asm { fld value }
    __asm { fistp result }
    return result;
}
// score 775 (was 701): the first 0x151b80 call's boolean argument passed through a named local; the
// call was no longer expanded, the body shrank to 2636 bytes and the score rose. Reverted; the frame
// and the two expanded calls remain the blocker.
// score 701 (was 701): frame 0x4c versus 0x74 and selective call expansion remain.
// Earlier: frame 0x4c versus 0x74; request/pointer homes and call expansion remain.
// frame 0x4c versus 0x74; request/pointer homes and call expansion differ.
// Frame 0x4c versus 0x74; request/pointer homes and selective expansion remain.
// Slot helper ordinary inline 723; view helper ordinary inline 812; sentinel result join 710;
// absent-slot exit 701; word hand selector 701.
// Earlier: frame 0x4c versus 0x74; request/pointer homes and call expansion differ.
// Frame pointer 816; native input fields 701; native player cooldown fields 701;
// direct permission result 707; combined native fields and direct result 707.
// Native unit view 701; candidate reference 701; request at dispatch 701;
// native activity results 707; native permission 701. Original body kept.
// Cooldown scalar names 701; rest-first clamp stores 706; gated idle cap 705;
// separate melee button tests 709; target-age store before test 701.
// Frame 0x4c versus 0x74 and selective expansion remain. Entry request clear 722; native request reset 862; mode-derived acceptance 714; query-derived readiness 711; ordered parented counter reset 701 (fast).
// Branch-local separate requests 804; live movement fields 701; joined cooldown clamp 707;
// non-primary melee arm first 701; target completion returns 701.
// Earlier: Frame 0x4c versus 0x74 and selective expansion remain. Entry request clear 722; native request reset 862; mode-derived acceptance 714; query-derived readiness 711; ordered parented counter reset 701 (fast).
// Clamped idle local 704; explicit cooldown scale locals 701; hoisted unit pointer 713;
// inlined candidate pointer 708; two request workspaces 804; permuter 697 but a false match
// (swapped callee arguments and byte/short volatile type stores, all wrong against retail),
// its neutral remainder 701. Original body kept.
// Round 214: Shared rounding 735; complete lunge/movement/both squared results 701 each;
// shared rounding with either/both squared results and the renamed request 735 each.
// Native view-helper input with two-use latch/view player receivers 700 (one-point gain rejected).
// Original helper, complete body and includes restored; frame 0x4c/0x74 and call expansion remain.
// Round 235: Native seat bitfield, existing weapon-flag query, both, actual player latch fields, paired
// latch/metadata and complete native receiver forms 707/707/715/701/707/707/715/715. Original metadata,
// declarations, player fields and body restored.
// Complete player/unit/both field observations 1153/772/1221; each with separate
// request records 1207/860/1332; both receivers with entry point outputs 1221;
// with separate requests and entry points 1332. Original receiver/request/point graph restored.
// Round 280: both cooldown outputs in short result carriers 713; earlier field
// bindings 713; with activity-bound result 727, earlier bindings 727. Full-width
// output carriers 718, earlier bindings 718; with activity-bound result 728,
// earlier bindings 728. Original field/result lifetimes and stores restored.
// Round 415: activity/decay/both/clock observations 723/715/702/702; activity-clock/decay-clock/all
// 724/714/703; complete settings receiver 715; complete duration helper/both plus melee 702/702;
// both activity result accumulators alone/with direct inputs 723/709; two real function-scope
// requests with direct decay/decay-clock recover frame 0x74 at 2813/2812 bytes, scores 778/777.
// Original body restored.
// Round 433: Repeated unit/player/both absolute slots at getter inputs, shared/separate requests;
// both slots with direct decay and either request extent; late unit-slot acquisition and native
// unsigned word slot domains; fast 764/769/825/827/871/923/792/856/765/745/745/827.
// Original body and existing helper restored.
// Round 47: retail fills two 0x20-byte requests (types 0x1b and 0x16 at +0x44/+0x64) where
// ours reuses one, and retail calls 0x151b80 at both sites and 0xccd20 out of line, where ours
// expands the first 0x151b80 (constant false) and 0xccd20 (retail expands 0xccd20 at its other
// two callers; its callee 0xb8540 is a stub here). Block-scoped requests 804 (frame 0x6c, 2842
// bytes, not packed); two function-scope requests 804; the lunge cooldown through the file's
// game_seconds_to_ticks_round 701; that with two requests 804; the 0x80 test nested 701.
// Round 239: complementary readiness/admission/lunge ownership, seven nonempty
// subsets and the complete byte graph, all eight 701. Original producers, request
// storage, fields, call tuples and complete body restored; frame remains 0x4c.
// Round 244: complete native action/button lvalues and readonly unit receiver,
// seven subsets all 701; complete graph with candidate addresses at each call
// 708. Original reference carriers, candidate, receiver and complete body restored.
// Round 50 (lane 1, fast): noinline on this file's declarations of 0x151b80/0xccd20 701 (ignored under
// LTCG; BLOCKED-BY 0x151b80 and 0xccd20 expansion). Retail tests seat bit 11 and definition bits 12 and 3
// as `shr reg,n; test reg8,1` and builds both activity bools branch-free (`|` of bits 28,0 and
// 26,17,16,5,4,0 gives its exact shr/or chains); scored (difflib aligned, base 708): bit 11 707 (705),
// bit 12 704 (707), bit 3 707 (684), all three 720 (686), activity bools with `|` 719 (596), all 739
// (575). Positional scoring rejects them while the early expansion shifts the listing; retail's
// branch-free `ticks < 0 ? 0 : ticks` (sets/dec/and) and `cmovne` are further leads.
// @retail 0x14f820
void __stdcall function_14f820(long player_index, long unit_index, byte *input)
{
    s_unit_request request;
    byte *player;
    byte *unit;
    player = (byte *)player_get(player_index);
    unit = object_get_unchecked(unit_index);
    word &actions = *(word *)(input + 0x1c);
    dword &buttons = *(dword *)input;
    s_target_candidate *candidate = (s_target_candidate *)(input + 0x28);
    if ((actions & 4) && function_151c10(player_index, candidate))
    {
        player_get(player_index)->flags_word |= 0x10;
        input[0x1c] &= ~4;
    }
    if (actions & 2)
    {
        if (function_151c90(player_index, candidate))
        {
            player_get(player_index)->flags_word |= 0x10;
            actions &= 0xfffd;
        }
        if ((actions & 2) && function_151940(player_index, candidate, true))
        {
            player_get(player_index)->flags_word |= 0x10;
            actions &= 0xfffd;
        }
    }
    if ((actions & 0x20) && function_151940(player_index, candidate, false))
    {
        player_get(player_index)->flags_word |= 0x20;
        player_clear_weapon_view(player_index);
        actions &= 0xfffd;
    }
    if ((actions & 0x80) && (function_151aa0(player_index, false) || function_151b80(player_index, false)))
    {
        player_get(player_index)->flags_word |= 0x20;
        player_clear_weapon_view(player_index);
        input[0x1c] &= 0x7f;
    }
    if ((actions & 4) && (player_current_weapon(unit_index, 0) != NONE || player_current_weapon(unit_index, 1) != NONE))
    {
        buttons |= 0x10000000;
        function_a8dd0(unit_index);
        input[0x1c] &= ~4;
    }
    long parent_index = *(long *)(unit + 0x14);
    short seat_index = *(short *)(unit + 0x1fc);
    if (parent_index != NONE && seat_index != NONE)
    {
        byte *parent = object_get_unchecked(parent_index);
        byte *definition = g_4e3b44[*(long *)parent & 0xffff].bytes;
        byte *seat = *(byte **)(definition + 0x1cc) + seat_index*0xb0;
        if ((*(dword *)seat & (1 << 11)) && !function_0c7070(unit_index) &&
            (g_4e6948->mode == 4 || !function_e68c0(0x1f, unit_index)) && (buttons & 0x4000000))
            function_e68c0(0x16, unit_index);
        if (actions & 6)
        {
            bool ready = true;
            if (function_0c7070(unit_index))
            {
                byte *current = object_get_unchecked(unit_index);
                byte *animation = current + *(short *)(current + 0x12a);
                real progress = 0.0f;
                if (*(long *)animation != NONE && *(short *)(animation + 6) != NONE)
                    progress = *(real *)(animation + 0x1c) * 0.03333333507180214f;
                if (1.0f > progress) ready = false;
            }
            if (ready)
            {
                bool accepted = true;
                if (g_4e6948->mode == 4) function_a9390(unit_index);
                else accepted = function_e68c0(0x1d, unit_index);
                if (accepted)
                {
                    player_get(player_index)->flags_word |= 0x10;
                    input[0x1c] &= ~6;
                }
            }
        }
    }
    if (g_4e6948->state == 2)
    {
        if (buttons & 0x80) function_1628f0(player_index, 5);
        if (buttons & 0x100) function_1628f0(player_index, 4);
    }
    if ((buttons & 0x10) && *(long *)(unit + 0x238) != NONE)
    {
        function_151ea0(player_index, *(long *)(unit + 0x238));
        function_ccd20(unit_index);
    }
    if (buttons & 0x60)
    {
        long weapon = player_current_weapon(unit_index, 0);
        bool lunge = false;
        bool allowed = true;
        if ((buttons & 0x40) && weapon != NONE)
        {
            if (function_1012c0(weapon))
            {
                long victim = *(long *)(unit + 0x1c8);
                if (*(real *)(unit + 0x1e4) >= 1.0f && function_bae20(victim, 3))
                {
                    point3f victim_position, unit_position;
                    function_b9dd0(victim, &victim_position);
                    function_b9dd0(unit_index, &unit_position);
                    real z = unit_position.z-victim_position.z;
                    real y = unit_position.y-victim_position.y;
                    real x = unit_position.x-victim_position.x;
                    lunge = z*z + y*y + x*x > 0.809999942779541f;
                }
                if (*(signed char *)(player + 0x3f) > 0 || unit[0x212] != unit[0x216]) allowed = false;
            }
            else if (!function_101300(weapon)) allowed = false;
        }
        else
        {
            if (!(buttons & 0x20) || *(signed char *)(player + 0x40) > 0) allowed = false;
            else if (function_1012c0(weapon) && *(signed char *)(player + 0x3f) > 0) allowed = false;
        }
        if (allowed)
        {
            long target = *(long *)(input + 0x30);
            if (!function_bae20(target, 3)) target = NONE;
            memset(&request, 0, sizeof(request));
            request.type = 0x1b;
            *(short *)request.arguments = lunge ? 2 : 1;
            *(long *)(request.arguments + 4) = target;
            if (function_e6900(unit_index, &request) && lunge)
                player[0x3f] = (byte)player_round_ticks((real)g_510c54->field_2_3 * 1.1f);
        }
    }
    long weapon = player_current_weapon(unit_index, 0);
    if (weapon != NONE)
    {
        byte *definition = g_4e3b44[*(long *)object_get_unchecked(weapon) & 0xffff].bytes;
        if (*(dword *)(definition + 0x12c) & (1 << 12)) buttons &= ~0x4000000;
    }
    if (buttons & 0x4000000)
    {
        memset(&request, 0, sizeof(request));
        request.type = 0x16;
        function_e6900(unit_index, &request);
    }
    byte *current = object_get_unchecked(unit_index);
    if (current[0x212] != 0xff && current[0x213] != 0xff)
        *(long *)(input + 0x1e) = *(long *)(unit + 0x214);
    else
    {
        weapon = player_current_weapon(unit_index, 0);
        if (weapon != NONE)
        {
            byte *definition = g_4e3b44[*(long *)object_get_unchecked(weapon) & 0xffff].bytes;
            if (*(dword *)(definition + 0x12c) & 8)
            {
                *(long *)(input + 0x1e) = *(long *)(unit + 0x214);
                if (buttons & 0x4010000) function_151b80(player_index, true);
            }
        }
    }
    short &idle = *(short *)(player + 0x17c);
    short &rest = *(short *)(player + 0x17e);
    if (*(long *)(unit + 0x14) == NONE && g_4e6948->state != 2)
    {
        byte *settings = *(byte **)((byte *)g_4e034c + 0xf4);
        real tick_scale = (real)g_510c54->field_2_3;
        long first = player_round_ticks(tick_scale * *(real *)(settings + 0x20));
        long second = player_round_ticks(tick_scale * *(real *)(settings + 0x24));
        real decay = *(real *)(settings + 0x28);
        weapon = player_current_weapon(unit_index, 0);
        bool local_85417d = weapon != NONE && function_101340(weapon);
        real angle = (real)atan2(*(real *)(input + 0x10), *(real *)(input + 0xc));
        real x = *(real *)(input + 0xc), y = *(real *)(input + 0x10);
        bool moving = sqrt(x*x + y*y) > *(real *)(settings + 0x30) &&
            angle > *(real *)(settings + 0x34)*-0.01745329238474369f &&
            angle < *(real *)(settings + 0x34)*0.01745329238474369f;
        real remaining = idle - rest*decay;
        if (remaining < 0.0f) { idle = 0; rest = 0; }
        else if (remaining >= first+second) { idle = (short)(first+second); rest = 0; }
        dword activity = buttons;
        bool first_activity = unit_animation_control_channel_playing(unit_index) || *(short *)(input + 0x24) != *(signed char *)(unit + 0x240) || (activity & 0x10000001);
        bool second_activity = function_cc3c0(unit_index) || (activity & 0x4030031) || local_85417d;
        if (first_activity || second_activity)
        {
            long ticks = idle - player_round_ticks(rest*decay);
            idle = ticks < 0 ? 0 : (short)ticks;
            rest = 0;
            if (first_activity && idle > first) idle = (short)first;
            if (second_activity) idle = 0;
        }
        if (unit[0xaa] != 0 || !function_e4050(unit_index))
        {
            if (moving) ++idle;
            else ++rest;
        }
    }
    else idle = rest = 0;
    long first_weapon = player_current_weapon(unit_index, 0);
    long second_weapon = player_current_weapon(unit_index, 1);
    long &target = *(long *)(player + 0x184);
    short &target_node = *(short *)(player + 0x188);
    long &target_age = *(long *)(player + 0x18c);
    if ((first_weapon != NONE && function_101160(first_weapon)) || (second_weapon != NONE && function_101160(second_weapon)))
    {
        if (*(long *)(input + 0x34) != NONE && !(input[0x4c] & 1))
        {
            target = *(long *)(input + 0x34);
            target_node = *(short *)(input + 0x38);
            target_age = 0;
        }
        else if (target != NONE && (real)++target_age * g_510c54->rate > 5.0f)
        {
            target = NONE; target_node = NONE; target_age = 0;
        }
    }
    else { target = NONE; target_node = NONE; target_age = 0; }
}



#include "unknown_157450.h"
#include "unknown_03d380.h"
struct s_effect_owner;
struct s_tail_traits;
struct s_marker_position_angle { point3f position; real angle; };
void marker_record_get_position_and_angle(s_marker_position_angle const *, point3f *, real *);
void function_b75a0(long, point3f const *, vector3f const *, vector3f const *, s_location const *, bool);
void function_b9c60(long, bool);
void function_b7290(long);
bool function_b9d20(long);
void __stdcall function_bef30(long, long, long, long, bool);
void function_b8b70(long);
void __stdcall function_b8540(long);
void function_b7930(void *, long, long, s_effect_owner const *);
long __stdcall function_b7b40(void *);
void __stdcall function_a7870(long);
void function_a7a30(long, dword);
void function_15e970(long);
s_tail_traits *function_1529a0(long);
bool function_152bf0(byte const *, long);
void function_cce00(long, short, bool, bool);
void motion_sensor_reset(long);
void function_153b80(long);
void function_152340(void);

// Disabled: retail reset requires an array-element address prohibited by the shared-global rule.
#if 0
// Retail 0x14f2e0; arg_9b462b exposes the required local-player storage binding.
bool __stdcall function_14f2e0(long player_index, s_player_4e9bd4 *arg_9b462b)
{
    byte *player = (byte *)player_get(player_index);
    bool reused = false;
    long unit_index = NONE;
    if (g_4e6948->state == 1)
    {
        unit_index = *(long *)(player + 0x34);
        *(long *)(player + 0x34) = NONE;
        if (unit_index != NONE)
        {
            byte *unit = object_get_unchecked(unit_index);
            if (unit[0x10a] & 4)
            {
                function_b8540(unit_index);
                unit_index = NONE;
            }
            else
            {
                function_b7290(unit_index);
                unit = object_get_unchecked(unit_index);
                if (*(dword *)(unit + 4) & 1)
                {
                    *(dword *)(unit + 4) &= ~1;
                    if (function_b9d20(unit_index)) function_bef30(unit_index, 0, 1, 0, false);
                    function_b8b70(unit_index);
                }
                function_14cad0(player_index, unit_index);
                goto initialized;
            }
        }
    }
    if (g_4e6948->mode != 4)
    {
        long local_287b6e_2 = NONE, tag = NONE;
        player_get_representation(player_index, &local_287b6e_2, &tag);
        short spawn = (short)function_14ef00(player_index, tag, true, false);
        if (spawn == NONE || tag == NONE) return false;
        s_players_globals *globals = (s_players_globals *)g_4e8c20;
        s_marker_position_angle *marker = (s_marker_position_angle *)(*(byte **)((byte *)g_4e0350 + 0x104) + spawn*0x34);
        if (globals->unknownac != NONE && function_badc0(globals->unknownac, 3))
        {
            point3f position;
            real angle;
            marker_record_get_position_and_angle(marker, &position, &angle);
            vector3f forward = {(real)cos(angle), (real)sin(angle), 0.0f};
            function_b75a0(globals->unknownac, &position, &forward, g_4687b0, NULL, false);
            globals = (s_players_globals *)g_4e8c20;
            function_b9c60(globals->unknownac, false);
            function_b7290(globals->unknownac);
            globals = (s_players_globals *)g_4e8c20;
            unit_index = globals->unknownac;
            globals->unknownac = NONE;
            reused = true;
        }
        else
        {
            byte creation[0xc4];
            function_b7930(creation, tag, NONE, NULL);
            *(long *)(creation + 0xc) = local_287b6e_2;
            real angle;
            marker_record_get_position_and_angle(marker, (point3f *)(creation + 0x1c), &angle);
            ((vector3f *)(creation + 0x28))->i = (real)cos(angle);
            ((vector3f *)(creation + 0x28))->j = (real)sin(angle);
            ((vector3f *)(creation + 0x28))->k = 0.0f;
            *(vector3f *)(creation + 0x34) = *g_4687b0;
            *(long *)(creation + 0x5c) = player_index;
            *(long *)(creation + 0x64) = *(signed char *)(player + 0xc0);
            *(long *)(creation + 0xa8) = *(long *)(player + 0x89);
            color3f colors[4];
            if (function_15f330(&player_get(player_index)->appearance, *(signed char *)(player + 0xc0), colors))
            {
                *(dword *)(creation + 0x74) |= 0xf;
                memcpy(creation + 0x78, colors, sizeof(colors));
            }
            unit_index = function_b7b40(creation);
        }
        if (unit_index == NONE) return false;
        function_14cad0(player_index, unit_index);
        function_a7870(unit_index);
        function_a7a30(unit_index, 0x400);
        if (g_4e6948->state == 2) function_15e970(unit_index);
        else if (!reused)
        {
            byte *scenario = (byte *)g_4e0350;
            bool initial = (player[2] & 8) == 0;
            long profile = initial ? 1 : 0;
            if (profile > *(long *)(scenario + 0xf8)-1) profile = *(long *)(scenario + 0xf8)-1;
            bool loadout = false;
            if (!initial && (scenario[0x12] & 0x20))
            {
                s_tail_traits *traits = function_1529a0(player_index);
                if (traits) loadout = function_152bf0((byte const *)traits, unit_index);
            }
            if (!loadout && profile >= 0 && profile < *(long *)(scenario + 0xf8))
                function_cce00(unit_index, (short)profile, true, initial);
        }
    }
initialized:
    *(long *)(player + 0x198) = 0;
    player[3] &= 0xf9;
    *(short *)(player + 0x17c) = 0;
    *(short *)(player + 0x17e) = 0;
    short user = *(short *)(player + 0x28);
    if (user != NONE) motion_sensor_reset(user);
    *(long *)(player + 0x194) = g_510c54->field_2_3;
    user = *(short *)(player + 0x28);
    if (user != NONE) function_153b80(user);
    user = *(short *)(player + 0x28);
    if (user != NONE) function_16f4b0(arg_9b462b + user);
    function_152340();
    player[2] &= ~8;
    return true;
}
#endif
bool __stdcall function_14f2e0(long player_index);

bool __stdcall function_cc170(long *);
bool function_1ca670(void);
bool function_13cb40(void);
bool function_14e970(long, long, point3f const *);
struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;

/* data_datum_iterator_next (0x6b380), which retail inlines here */
static inline bool player_datum_iterator_next(s_data_datum_iterator *iterator)
{
    s_record_pool *data = iterator->data;
    long index = function_16bc00(data, iterator->index + 1);
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

// score 103 (was 103): reading g_4e8c20 directly at every use instead of through the named
// `globals` local builds to the same bytes in the full build (611 = 611, first difference
// +0x8) and scores 175 against 193 in the reduced image, so the local is not what holds the
// pointer in EDI; what still differs is the register itself: ours keeps g_4e8c20 in EDI where
// retail has it in ESI, and retail's iterator holds the pool's index array in ECX where ours
// has ESI, so the two roles are swapped.
// Earlier: the store through the named `globals` local was tried again and scored
// 180 in the full build against the 103 baseline, so it was reverted; the globals pointer stays
// live in EDI through both iterator loops and retail keeps it in ESI, reloading it only at the
// two stores; target/point homes still differ.
// target/point homes still differ.
// Earlier: the global is kept in EDI here and in ESI in retail,
// so the prologue saves the other registers and the field that is written
// afterwards is reached through a different base register.
// pool through a helper parameter 141; no pool local 141; busy by duplicated
// store plus continue 123; position through a pointer local 103; wide busy 134;
// failed after the iterator init 108; failed at function scope 128; body wrapped
// in the unit test 103; the existing out-of-line root helper called with its
// noinline removed inlines to the same 103 but regresses 0x14e200 17 -> 534 and
// 0x31910 139 -> 201, so the attribute and the written-out walk stay.
// Round 415: retained unit 141; existing point setter 117; both 134; unsigned retained handle 103;
// unit/setter/both with unsigned handle 141/117/134; setter with direct component receivers 130.
// Original body restored.
// Round 423: stage status published at the join as short/long/byte 191/191/163;
// each with early globals scope 191/191/163; short status with retry accumulation 178,
// also with early scope 178. Complete original body restored.
// Round 433: Existing first iterator with direct data observations, returned datum ownership and
// exhaustion-first arms; direct-data forced expansion; fast 103/103/103/102/102/102/102/103.
// Original body and existing helper restored.
// Round 51: the cause is the final `unknown9e = false` store: our build keeps the globals pointer
// (EDI) available across both loops for it, reloading it on loop 1's back edge and after each loop-2
// call, so `last_unit` takes EBP and the loop-1 registers swap; retail reloads the global only at that
// store and keeps it in ESI for the prologue tests alone. On an LTCG env link (this file, its closure
// and 0x13cb40's file; equal to the full listing; aligned counts against 77): the existing
// players_globals_14f2e0() for the in-loop store, or for both stores 77; busy arm first 79;
// `if (!failed)` store 77; global re-read into `globals` before the store 77; the store through
// `globals` 73 (the global takes ESI as retail, but 618 bytes: the field address is kept on the
// stack); `globals` scoped to the prologue 77; `failed` declared with the loop-1 locals 81.
// Round 47: the EDI global is g_4e8c20 promoted from the entry read to the final
// `unknown9e = false` (reloaded after the loop calls): LTCG sees a path with no possible
// write of it (unknown9f set, no loop trips), retail reads it fresh at the end and keeps
// globals in ESI. Final store through the getter 103; with the `= 6` store too 103;
// `if (!failed)` final store 103; status block reading the global at each use 103; second
// loop's own iterator 155; one unit_index for 0xcc170's out value and the last unit 103/103.
// Diagnostics, not kept: final store to another global 91; an opaque call after the
// status block 91 (retail prologue); g_4e8c20's address taken 91 but 21 matches lost;
// 0x16bc00 built without link-time code generation 90 but 98 lost; 0x6b380 through a
// pointer 146; 0x6b380 as the record-pool step on its +4 fields 103.
// Round 131 (lane 4): ours reloads the global into EDI after the second loop's 0x14f2e0/0x14e970
// calls and carries it through the 0x6b380 calls to the final store; retail loads it at the store.
// The status block in its own braces 103, the entry pointer through players_globals_14f2e0() 103,
// the busy store through the entry local 103 (all the same code); last_unit = NONE declared at the
// function's top 157 (612 bytes); `if (last_unit == NONE) return;` before the second loop 103.
// Round 244: joint busy/retry-failure byte, signed-byte and paired short/long
// carriers 193/193/193/193/193/193/198/190 (fast control 193, 592 bytes);
// paired long saves only three and is dropped. Existing-sibling control remains
// 193; full baseline 103. Original predicate domains and body restored.
// Round 50 (lane 1, plain checks): with the final store aimed at another global (diagnostic) 88: the
// prologue then matches; left are loop 1's datum/unit in ESI/EDI swapped and NONE not kept in EDX after
// the 0x16bc00 call (retail `or edx,-1; cmp eax,edx`, reused for the NONE store, the last-unit test and
// loop 2's set-up). Loop 1 on the record-pool iterator (current + cursor, data_iterator_next_calling) with
// loop 2's own datum iterator 155; the header's data_datum_iterator_next_called 103; loop 2 in 0x14d3a0's
// word-flags/continue form 103; position copied before the last unit is stored 101; loop 1's body nested
// under `unit != NONE` 103. On that 101 form: loop 2 in 0x14d3a0's form 101, `if (!failed)` final store 101,
// busy store through `globals` 101, `if (last_unit == NONE) return;` before loop 2 101.
// Round 48 (m3 lane 11, plain; --fast gives 193 here): the root walk as a do-while 103 (same code); the
// final store through `globals` under `if (!failed)` 176 (618 bytes). The hoisted g_4e8c20 load passes
// only the out-of-line 0x6b380 call: loop 2 stepping through the inline copy (0x16bc00 called directly)
// gives retail's ESI prologue and the final store's own reload (first difference +0x3e) but 566 bytes,
// 173; loop 1 through 0x6b380 180 (562 bytes); both swapped 175 (566 bytes). Retail calls 0x6b380 in
// loop 2 and still reloads at the store, so what to find is why retail's call there ends the load's reach.
// @retail 0x14cdd0
void function_14cdd0(void)
{
    s_players_globals *globals = (s_players_globals *)g_4e8c20;
    globals->unknown9c = 0;
    if (globals->unknowna0 > 0 && !function_13cb40()) --globals->unknowna0;
    if (!globals->unknown9e) return;
    if (function_13cb40()) globals->unknown9c = 2;
    else if (!globals->unknown9f)
    {
        if (globals->unknowna0 > 0) globals->unknown9c = 3;
        else if (((byte *)g_4e6948)[0x1121]) globals->unknown9c = 8;
        else
        {
            long value;
            if (function_cc170(&value)) globals->unknown9c = 4;
            else if (function_1ca670()) globals->unknown9c = 5;
        }
    }
    if (globals->unknown9c != 0) return;
    s_record_pool *players = g_4e8c24;
    long last_unit = NONE;
    point3f last_position;
    s_data_datum_iterator iterator;
    iterator.data = players;
    iterator.index = NONE;
    iterator.datum_index = NONE;
    while (player_datum_iterator_next(&iterator))
    {
        s_player *player = (s_player *)iterator.datum;
        long unit = player->unit_index;
        if (unit == NONE) continue;
        long root = unit;
        for (long current = unit; current != NONE; current = *(long *)(object_get_unchecked(current) + 0x14)) root = current;
        bool busy = false;
        if (root == unit)
        {
            if (function_badc0(unit, 1)) busy = function_e4050(unit);
        }
        else
        {
            byte *vehicle = (byte *)function_badc0(root, 2);
            if (vehicle) busy = vehicle[0x34c] > 0;
        }
        if (!busy)
        {
            last_unit = player->unit_index;
            last_position = *(point3f *)(object_get_unchecked(last_unit) + 0x30);
        }
        else ((s_players_globals *)g_4e8c20)->unknown9c = 6;
    }
    if (last_unit != NONE)
    {
        bool failed = false;
        iterator.data = players;
        iterator.index = NONE;
        iterator.datum_index = NONE;
        while (data_datum_iterator_next(&iterator))
        {
            s_player *player = (s_player *)iterator.datum;
            if (!(player->flags_word & 2) && (player->flags_word & 1) && player->unit_index == NONE)
            {
                long index = iterator.datum_index;
                if (!function_14f2e0(index) || !function_14e970(index, last_unit, &last_position)) failed = true;
            }
        }
        if (failed) return;
        ((s_players_globals *)g_4e8c20)->unknown9e = false;
    }
}

struct s_damage_owner
{
	long player_index;
	long object_index;
	short team;
};

/* a damage event (0x88 bytes) */
struct s_type_1e6529
{
	long definition_index;
	union
	{
		byte unknown04[4];
		dword flags;
	};
	s_damage_owner owner;
	long unknown14;
	long unknown18;
	long unknown1c;
	short unknown20;
	short unknown22;
	point3f position;
	point3f origin;
	vector3f direction;
	vector3f node_direction;
	real unknown54;
	real unknown58;
	real unknown5c;
	real distance;
	real distance_scale;
	bool in_unknown_radius;
	byte unknown69[3];
	vector3f cone_direction;
	real unknown78;
	short unknown7c;
	short unknown7e;
	byte unknown80[4];
	bool unknown84;
	byte unknown85[3];
};


#include "game_engine_events.h"
#include "effects.h"
void function_d6660(s_type_1e6529 *, long);
void __stdcall object_get_damage_owner(long, s_damage_owner *);
void __stdcall function_d7b80(s_type_1e6529 *, long, short, short, short, vector3f const *);
bool __stdcall function_cc460(long);
void function_b7360(long);
void function_153cd0(long);
void function_153bd0(long, real);
void function_1521d0(long);
bool function_11c470(long, point3f const *);
bool function_11c5b0(long object_index, long trigger_volume_index);
void function_12b850(short);
void __stdcall function_1508e0(long);
void function_1e6bd0(long);

struct s_player_update_iterator
{
    byte *current;
    s_record_pool_iterator cursor;
};

struct s_unit_flags_14d8b0
{
	word bit0 : 1;
	word bit1 : 1;
	word bit2 : 1;
	word bit3 : 1;
	word bit4 : 1;
	word bit5 : 1;
	word bit6 : 1;
	word bit7 : 1;
	word bit8 : 1;
};

// score 9 (was 9): the late mode receiver is EDX rather than ECX; reset-slot
// scratch registers are EAX/ECX rather than EDX/EAX, and the root pool uses EDX rather
// than ECX. Frame 0xc8 and 1287-byte extent agree; first difference is +0x304.
// The timer callee 0x1521d0 now matches. Earlier: 59 statement variants stayed at 9.
// Earlier: late mode, reset-slot and root-pool scratch registers remain.
// the nine differing lines have one root cause: the last g_4e6948->mode test
// keeps the options pointer in EDX where retail keeps it in ECX, and that single allocation
// cascades into the game-engine block (EAX and ECX where retail has EDX and EAX) and into the
// root walk's global load (EDX where retail has ECX); the callee 0x1521d0 splits its own
// globals and data registers the other way round as well. A half-hour permuter run over the
// function (59 variants) left the score at 9, so the choice is not reachable from its statements.
// Earlier (9): late mode, reset-slot and root-pool scratch registers remain.
// Earlier: frame 0xc8 and retail length agree; tick/rate scheduling, the damage-tag
// pointer load and scratch registers in the later mode, slot and root blocks remain.
// Earlier isolated damage-byte store order 19 was below the keep threshold.
// Round 32: the trigger test through 0x11c5b0 148; the damage-type pointer read in the call 130;
// long user index 19 with it (both loops get retail's padding). Then: typed byte decrements 19;
// one inline helper for both slot notifications 19; `time = rate; time *= ticks` 226 (folds);
// rate read after the increment, either product order, 223 (folds); root initialised NONE 19;
// a 600 s permuter run (13 tries), none below 19.
// Earlier at 153: iterator store/test and rate/tick scheduling differ;
// flag snapshot 155; long timer 204; parent-carried root walk 182;
// low-time arm first 179; inline depth one 155. Damage call convention differs: BLOCKED-BY 0xd66d0
// (retail pushes both arguments; ours passes the object in eax).
// Native tick field 155; native tick and typed first flag 153; immutable native-field rate 155;
// native reset timer 155; both native long/short fields 155; signed native countdowns 155;
// all native countdown fields 155; full native current-player record 153.
// Earlier: iterator publication, rate/store, damage-call and trigger allocation remain.
// Separate effect/tick/root/trigger receivers 155/155/155/155; combined receivers 155;
// incremented-tick snapshot 155; stored absence scalar 155; decoded volume kind 155.
// Earlier: native root-bit observation recovers SHR/TEST; iterator and tick scheduling remain.
// Wide user/table entry/both 157/158/162; wide/entry/both with first flag 155/156/160;
// root with both 129, plus first flag 127; root alone 128 plain; root with wide/entry 125/132.
// Three-point wide-user addition dropped; only the root-bit expression retained.
// iterator/tick scheduling and damage transport remain;
// shared event send 155; existing initializer 304f; both event APIs 283f;
// event/damage/both workspace scopes 155/155/155; scopes with send 207f;
// both APIs with typed first flag 281f. Original body retained.
// Iterator publication, tick/rate schedule, damage call and trigger registers
// remain. current record consumer 155; successful current publication 155; explicit iterator
// initialization 155; defined current at advance 155; saturating source expression 254; saturate
// before increment 224; native flag boolean 264; publication with native results 300.
// Earlier: iterator/tick scheduling, damage transport and trigger registers remain.
// Native trigger record ordinary/const 128/128; each with native point member 128/128;
// native first-word flag with those four interfaces 126/126/126/126. Two-point forms dropped.
// Earlier: iterator/tick schedule, damage transport and trigger homes remain.
// Existing Boolean datum API 128; iteration receiver scope 128; input-only iterator
// initialization 128; called datum API 395; existing parent query 197; both APIs 197;
// Boolean iterator with native first flag 126; parent query with native first flag 195.
// Initial/all native flag references 126/126; shared high-byte reference 128;
// all-flags with tick pointer 126, direct tick product 228, float/count roles 126/126,
// admitted expiry 199. Two-point forms dropped; original body retained.
// Long user index in the expired-ticks arm 125 (retail movsx + cmp eax, -1; that arm then differs only in
// register rotation; under the keep threshold alone); with the tick stored before the rate read 233 (rate folded
// into mulss), rate named after the increment 233; trigger test as one condition 125; datum index read from
// the iterator at each use 125.
// Round 29 (from 160): the countdown at +0x180 read only under the mode test 153 kept
// (retail tests the mode first). Left: the tick store sits after the rate multiply
// (retail stores, then loads the rate unfolded), register rotation in the damage and
// slot blocks, and a one-byte shorter mode load (eax) that drops the root walk's loop
// alignment. Tick field at each use 153; ticks * rate 153; rate read in the product
// after the increment 228 (folds the rate load); a ticks-to-seconds inline 228; long
// user index in the expired arm 157.
// Low-time/reset/both inline declarations 21/229/229; signed-word counter 23; unsigned datum 21;
// unsigned handles 21; low-time inline with first native flag 19; native controls with that flag 21;
// combined first native flag and damage-byte store order 17 (plain whole-program confirmed).
// Round 379: function-scope event/damage/both 17/23/23; early tag input alone/with event/damage/both
// 150/150/151/151; tag value after scratch store 17. Original executable body restored.
// Round 164: separate root handle, branch-local short expiry result, tick/result/rate names,
// all three names, joined slot result and root+slot result: 17 each. Original body retained.
// Kimi lane 2 r37 (on the 17 body): typed time receiver with the else arm rereading the rate 11;
// late rate with a second low-time use 13; in-place multiply 205; tag-list pointer 145.
// Round 415: first/second/both byte countdown results 9/9/9; paired acquisition 16; long/short
// results 114/114; byte results with short reset result/native reset flag 9/9. Original body
// restored.
// Round 98: mode and reset countdown as one condition 9; reset countdown through a short reference 9;
// the two byte countdowns through a signed char pointer 9; short slot index local in both slot
// blocks 9; flag bytes through the player's flags word 9. At that stage, 0x1521d0 also split
// global/data registers ECX/EDX where ours reused one; that callee now matches.
// Round 219: engine-global snapshots, native byte countdown members and both 9,9,9;
// timer inline alone/with snapshots/with byte members/with both 9,9,9,9;
// expiry result alone/with snapshots 9,9; native reset member/all native
// countdown members/inline with reset member 9,9,9. Original body retained.
// blocks 9; flag bytes through the player's flags word 9. Retail also splits the global/data
// registers in the callee 0x1521d0 (ecx/edx where ours reuses one).
// Round 145: one forceinline engine-slot notify helper for both slot blocks 9; root walk as a while
// loop from NONE 9, as a do/while 9; reset countdown through a short pointer 9; both slot blocks reading
// the engine globals directly 9 (all the same code: mode pointer EDX, retail ECX).
// @retail 0x14d8b0
void function_14d8b0(void)
{
    byte *player;
    s_player_update_iterator iterator = { NULL, { g_4e8c24, NONE, NONE } };
    while ((player = iterator.current = data_iterator_next_inlined(&iterator.cursor)) != NULL)
    {
        long index = iterator.cursor.datum_index;
        if (((s_player *)player)->flags_word & 0x2000)
        {
            byte *globals = *(byte **)((byte *)g_4e034c + 0x134);
            long effect = *(long *)(globals + 0xb4);
            long unit = *(long *)(player + 0x2c);
            if (unit != 0 && effect != NONE) function_176780((s_effect_owner const *)NULL, unit, 0.0f, effect, 0.0f, NULL, NULL);
            player[3] &= ~0x20;
        }
        long unit = *(long *)(player + 0x2c);
        if (unit != NONE && g_4e6948->mode != 4)
        {
            long counter;
            if (((g_510c54->game_time + (short)index) % 16 == 0 || player[0x3e]) && function_cc460(unit))
            {
                counter = player[0x3e] + 1;
                if (counter > 254) counter = 254;
            }
            else counter = 0;
            player[0x3e] = (byte)counter;
            long limit = player_round_ticks((real)g_510c54->field_2_3 * 0.5f);
            if (player[0x3e] > limit)
            {
                unit = *(long *)(player + 0x2c);
                if (unit != NONE)
                {
                    s_unit_flags_14d8b0 *flags = (s_unit_flags_14d8b0 *)(object_get_unchecked(unit) + 0x10a);
                    flags->bit5 = true;
                    function_b7360(unit);
                }
            }
        }
        else player[0x3e] = 0;
        long &ticks = *(long *)(player + 0x16c);
        if ((player[2] & 4) && (unit = *(long *)(player + 0x2c)) != NONE)
        {
            s_game_time_globals *time_globals = g_510c54;
            ++ticks;
            real time = time_globals->rate * (real)ticks;
            if (time_globals->rate * (real)ticks >= 3.0f)
            {
                if (!TEST_FIELD_BIT(((s_unit_flags_14d8b0 *)(object_get_unchecked(unit) + 0x10a))->bit5))
                {
                    if (g_4e6948->mode != 4)
                    {
                        s_event event;
                        game_engine_event_initialize_inline(&event, 0, 0x2e);
                        event.effect_player_index = index;
                        function_a7c50(&event);
                        function_19eb30(&event);
                        {
                            s_type_1e6529 damage;
                            damage.unknown7c = NONE;
                            function_d6660(&damage, *(long *)(*(byte **)((byte *)g_4e034c + 0x144) + 0x28));
                            *(byte *)&damage.unknown84 = 0x29;
                            damage.flags |= 4;
                            object_get_damage_owner(*(long *)(player + 0x2c), &damage.owner);
                            function_d7b80(&damage, *(long *)(player + 0x2c), NONE, NONE, NONE, NULL);
                        }
                    }
                    function_153cd0(index);
                }
            }
            else function_153bd0(*(short *)(player + 0x28), time * 0.3333333432674408f);
            if (g_4e6948->mode != 4) player[2] &= ~4;
        }
        else if (ticks > 0 && --ticks == 0)
        {
            if (game_engine_get())
            {
                long slot = function_xaee93d()->slots[(short)index];
                if (slot != NONE) function_b58c0(slot, 8);
            }
            long user = player_get(index)->user_index;
            if (user != NONE)
            {
                g_502120->entries[user].value80 = 0.0f;
                g_502120->entries[user].value84 = 0.0f;
            }
        }
        if (*(signed char *)(player + 0x3f) > 0) --player[0x3f];
        if (*(signed char *)(player + 0x40) > 0) --player[0x40];
        if (g_4e6948->mode != 4)
        {
            if (*(short *)(player + 0x180) > 0 && --*(short *)(player + 0x180) == 0)
            {
                player[3] &= ~8;
                if (game_engine_get())
                {
                    long slot = function_xaee93d()->slots[(short)index];
                    if (slot != NONE) function_b58c0(slot, 0x100);
                }
            }
        }
        if (*(long *)(player + 0x2c) != NONE) function_1521d0(index);
        unit = *(long *)(player + 0x2c);
        if (unit != NONE)
        {
            long root = unit;
            for (long current = unit; current != NONE; current = *(long *)(object_get_unchecked(current) + 0x14)) root = current;
            if (!(bool)((*(dword *)(object_get_unchecked(root) + 4) >> 18) & 1))
            {
                byte *scenario = (byte *)g_4e0350;
                for (short trigger = 0; trigger < *(long *)(scenario + 0x130); ++trigger)
                {
                    byte *volume = *(byte **)(scenario + 0x134) + trigger*0xe;
                    if (*(short *)(volume + 2) == g_4686c4)
                    {
                        if (function_11c5b0(*(long *)(player + 0x2c), *(short *)volume))
                        {
                            byte *globals = (byte *)g_4e8c20;
                            *(short *)(globals + 0xa4) = trigger;
                            *(short *)(globals + 0xa6) = 0;
                            *(long *)(globals + 0xa8) = index;
                            function_12b850(*(short *)(volume + 4));
                        }
                    }
                }
            }
        }
        function_1508e0(index);
        if (*(short *)(player + 0x28) != NONE) function_1e6bd0(index);
    }
    byte *globals = (byte *)g_4e8c20;
    if (*(long *)(globals + 0xa8) != NONE && ++*(short *)(globals + 0xa6) > 12)
    {
        *(long *)(globals + 0xa8) = NONE;
        *(short *)(globals + 0xa6) = 0;
    }
}

bool function_15dd40(long);
void function_15ed60(void);
void function_14f0f0(long);
bool function_138820(void);
bool function_1388a0(void);
struct s_request_source;
void function_1e6b00(long, s_request_source *);
void function_1505f0(long, long, byte const *);
void function_150790(long);
bool function_163b60(void);
bool function_162c50(long, long *);
__declspec(noinline) bool function_2259f0(long);
void function_2259a0(long);
void function_a9400(long, long);
void game_engine_boot_player(long);
#include "main_globals.h"

// @retail 0x14d3a0
void __stdcall function_14d3a0(byte const *input)
{
    byte const *const *input_reference = &input;
    players_update_connections();
    long pending[16];
    long count = 0;
    s_player_update_iterator iterator;
    iterator.cursor.data = g_4e8c24; iterator.cursor.index = NONE; iterator.cursor.datum_index = NONE;
    byte *player;
    while ((player = iterator.current = data_iterator_next_inlined(&iterator.cursor)) != NULL)
    {
        long index = iterator.cursor.datum_index;
        word flags = *(word *)(player + 2);
        if ((flags & 2) || !(flags & 1) || *(long *)(player + 0x2c) != NONE) continue;
        if (g_4e6948->state == 2)
        {
            if (g_4e6948->mode != 4 && function_15dd40(index)) pending[count++] = index;
            else function_14f0f0(index);
        }
        else if (g_4e6948->state == 1 && g_4e6948->mode != 4)
        {
            if (flags & 8) function_14f2e0(index);
            else if (function_138820() && function_1388a0()) players_start_countdown();
        }
    }
    while (count > 0)
    {
        long selected = 0;
        if (count > 1)
        {
            g_4e7408->unknown0 = g_4e7408->unknown0 * 1664525 + 1013904223;
            selected = (long)(g_4e7408->unknown0 >> 16) % count;
        }
        long index = pending[selected];
        if (function_15dd40(index))
        {
            function_14f2e0(index);
            function_15ed60();
        }
        else
        {
            player = (byte *)player_get(index);
            if (player[2] & 8)
                *(long *)(player + 0x164) = player_round_ticks((real)*(long *)((byte *)g_4e6948 + 0x1bc) * (real)g_510c54->field_2_3);
        }
        if (selected != count-1) pending[selected] = pending[count-1];
        --count;
    }
    function_14cdd0();
    iterator.cursor.data = g_4e8c24; iterator.cursor.index = NONE; iterator.cursor.datum_index = NONE;
    while ((player = iterator.current = data_iterator_next_inlined(&iterator.cursor)) != NULL)
    {
        long unit = *(long *)(player + 0x2c);
        if (unit == NONE) unit = *(long *)(player + 0x30);
        if (unit == NONE) continue;
        long root = NONE;
        for (long current = unit; current != NONE; current = *(long *)(object_get_unchecked(current) + 0x14)) root = current;
        byte *root_object = object_get_unchecked(root);
        if (*(short *)(root_object + 0x2c) != NONE) *(short *)(player + 0x2a) = *(short *)(root_object + 0x2c);
    }
    iterator.cursor.data = g_4e8c24; iterator.cursor.index = NONE; iterator.cursor.datum_index = NONE;
    while ((player = iterator.current = data_iterator_next_inlined(&iterator.cursor)) != NULL)
    {
        long index = iterator.cursor.datum_index;
        long absolute = index & 0xffff;
        byte const *source = *input_reference;
        if (!(*(dword *)(source + 8) & (1 << absolute))) continue;
        s_player_action const *action = (s_player_action const *)(source + 0xc + absolute*0x5c);
        if (!function_14d0a0(action) || !action) continue;
        long unit = *(long *)(player + 0x2c);
        byte *object = NULL;
        if (unit != NONE) object = object_get_unchecked(unit);
        if (unit != NONE && (*(dword *)(object + 0x134) & 1))
        {
            byte *game = (byte *)g_510c50;
            if (action->flags & 0x100)
            {
                byte reset = game[6];
                if (reset && game[0x22]) reset = !function_163b60();
                if (reset)
                {
                    main_globals.switch_structure_bsp = false;
                    main_globals.save_map = false;
                    main_globals.unknown72 = true;
                }
            }
            byte *globals = (byte *)g_4e8c20;
            if (!globals[6])
            {
                s_player_action current = *action;
                if (globals[7] && player_get(index)->user_index != NONE)
                {
                    dword yaw = *(dword *)&current.facing_yaw, pitch = *(dword *)&current.facing_pitch;
                    player_action_initialize(&current);
                    *(dword *)&current.facing_yaw = yaw; *(dword *)&current.facing_pitch = pitch;
                }
                player_action_update_latches(index, &current);
                function_1e6b00(index, (s_request_source *)&current);
                function_14f820(index, *(long *)(player + 0x2c), (byte *)&current);
                function_1505f0(index, *(long *)(player + 0x2c), (byte const *)&current);
            }
            else if (*(long *)(object + 0x12c) == NONE) function_150790(*(long *)(player + 0x2c));
        }
        else if (*(short *)(player + 0x28) != NONE)
        {
            long spectated;
            if (function_162c50(index, &spectated) && function_2259f0(index))
            {
                word &flags = *(word *)(player + 2);
                if (action->flags & 2)
                {
                    if (!(flags & 0x200) && (flags & 0x400))
                    {
                        flags |= 0x200;
                        function_2259a0(index);
                        if (g_4e6948->mode == 4) function_a9400(spectated, index);
                        else game_engine_boot_player(spectated);
                    }
                    player[3] &= ~4;
                }
                else flags = (flags & 0xfdff) | 0x400;
            }
        }
    }
}

