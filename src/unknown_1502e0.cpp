// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1502E0.CPP: player helpers at the end of the players code (target
   candidates, the local players' view state, the players' census) */

#include "unknown_11c920.h"
#include "segment_sphere.h"
#include "globals.h"
#include "sound_sources.h"
#include "unknown_0259d0.h"
#include <string.h>
#include "object_iterator.h"
#include "unknown_06c1d0.h"

/* the players (g_4e8c24, 0x21c bytes each) */
struct s_tail_player
{
	short salt;
	word flags;
	byte unknown04[0x10 - 4];
	long leave_time;
	byte machine_address[6];
	short machine_index;
	short machine_user;
	byte unknown1e[2];
	long machine_controller;
	long controller_index;
	short local_index;
	byte unknown2a[2];
	long unit_index;
	long previous_unit_index;
	byte unknown34[0x174 - 0x34];
	long object_index174;
	byte unknown178[0x21c - 0x178];
};

/* the object header data (g_4e0300, 12 bytes each) */
struct s_tail_object_header
{
	short salt;
	byte flags;
	byte type;
	byte unknown04[4];
	void *object;
};

/* the objects as the players code sees them */
struct s_tail_object
{
	byte unknown00[0xaa];
	byte type;
};

/* a candidate: its priority and its object */
struct s_target_candidate
{
	short priority;
	byte flags;
	byte unknown03;
	long object_index;
};

/* g_4e8c20 seen as the players' census */
struct s_player_census
{
	byte unknown00[4];
	bool all_without_units;
	bool any_available;
};

/* the view entries of g_4ed284 (0x94 bytes from +0x14) */
struct s_view_entry
{
	byte unknown00[0x28];
	short index28;
	byte unknown2a[0x8e - 0x2a];
	bool flag8e;
	bool flag8f;
	byte unknown90[0x94 - 0x90];

	__forceinline void clear_flags()
	{
		if (flag8e)
			flag8f = true;
		index28 = NONE;
	}
};

struct s_view_globals
{
	byte unknown00[0x14];
	s_view_entry entries[4];
};

struct s_object;
s_object *function_bae20(long object_index, dword type_mask);
point3f *function_b9dd0(long object_index, point3f *result);

static inline s_tail_player *tail_player_get(long player_index)
{
	return (s_tail_player *)g_4e8c24->data + (player_index & 0xffff);
}

static inline s_tail_object *tail_object_get(s_record_pool *pool, long object_index)
{
	return (s_tail_object *)((s_tail_object_header *)pool->data)[object_index & 0xffff].object;
}

void function_14cad0(long player_index, long unit_index);
void function_187a60(long datum_index);
void function_152340(void);

bool __stdcall function_e5470(long player_index, long object_index);
long __stdcall function_e5bc0(long unit_index, long object_index, long previous_index);
bool function_1df560(short team_a, short team_b);
bool function_151320(long player_index, s_target_candidate *candidate, s_target_candidate *best);

/* the object fields 0x1515e0 reads */
struct s_1515e0_object
{
	byte unknown00[0x10a];
	word unknown10a_0 : 1;
	word unknown10a_1 : 1;
	word unknown10a_2 : 1;
	word unknown10a_3 : 1;
	word unknown10a_4 : 1;
	word unknown10a_5 : 1;
	word unknown10a_6 : 1;
	word unknown10a_7 : 1;
	word unknown10b_0 : 8;
	byte unknown10c[0x138 - 0x10c];
	short team;
};

// @retail 0x1515e0
void function_1515e0(long object_index, long player_index, s_target_candidate *best)
{
	long const *player_reference = &player_index;
	s_target_candidate *const *best_reference = &best;
	s_tail_player *player = tail_player_get(*player_reference);
	s_1515e0_object *object = (s_1515e0_object *)tail_object_get(g_4e0300, object_index);
	byte type = ((s_tail_object_header *)g_4e0300->data)[player->unit_index & 0xffff].type;
	if (((1 << type) & 1) && !TEST_FIELD_BIT(object->unknown10a_2))
	{
		if (function_1df560((short)*((char *)player + 0xc0), object->team))
			*(long *)((byte *)*best_reference + 8) = function_e5bc0(player->unit_index, object_index, *(long *)((byte *)*best_reference + 8));
		else if (function_e5470(*player_reference, object_index))
		{
			s_target_candidate candidate;
			memset(&candidate, 0, sizeof(candidate));
			candidate.priority = 2;
			candidate.object_index = object_index;
			function_151320(*player_reference, &candidate, *best_reference);
		}
	}
}

bool function_d6af0(long unit_index);
bool function_d6790(long unit_index);
void player_speed_request_14ea20(long player_index);
void player_speed_request_14eb10(long player_index);
void player_speed_request_14ec00(long player_index);
bool function_152000(long player_index, short effect, short ticks);
void __stdcall function_191fab(long definition_index, long local_index);
void function_a89c0(long unit_index, long definition_index);
void function_f8110(long object_index);
void __stdcall function_b8540(long object_index);

/* game ticks in the given number of seconds, rounded */
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

// @retail 0x151ea0
void function_151ea0(long player_index, long object_index)
{
	s_tail_player *player = tail_player_get(player_index);
	s_tail_object_header *header = (s_tail_object_header *)g_4e0300->data + (object_index & 0xffff);
	byte *object = (byte *)tail_object_get(g_4e0300, object_index);
	byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
	long ticks = game_seconds_to_ticks_round(*(real *)(definition + 0x130));
	bool unavailable = false;
	if (header->flags & 0x10)
		unavailable = true;
	if ((short)ticks > 0 && !unavailable)
	{
		bool proceed = true;
		short kind = *(short *)(definition + 0x12c);
		if (kind != 1)
		{
			if (kind == 2)
			{
				proceed = function_d6af0(player->unit_index);
				if (proceed) player_speed_request_14ea20(player_index);
			}
			else if (kind == 5)
			{
				proceed = function_d6790(player->unit_index);
				if (proceed) player_speed_request_14ec00(player_index);
			}
			else
			{
				/* only kinds 3 and 4 remain here */
				short effect;
				switch (kind)
				{
				case 3: effect = 0; break;
				case 4: effect = 1; break;
				default: __assume(0);
				}
				proceed = function_152000(player_index, effect, (short)ticks);
				if (proceed && effect == 0) player_speed_request_14eb10(player_index);
			}
		}
		if (proceed)
		{
			long definition_index = *(long *)tail_object_get(g_4e0300, object_index);
			function_191fab(definition_index, player->local_index);
			function_a89c0(player->unit_index, definition_index);
			if (player->local_index != NONE) function_f8110(object_index);
			function_b8540(object_index);
		}
	}
}

// @retail 0x151320
bool function_151320(long player_index, s_target_candidate *candidate, s_target_candidate *best)
{
	bool result = false;
	point3f player_position;
	point3f best_position;
	point3f candidate_position;
	vector3f to_candidate;
	vector3f to_best;
	real candidate_distance;
	real best_distance;

	if (candidate->priority > best->priority)
	{
		result = true;
	}
	else if (candidate->priority == best->priority)
	{
		function_b9dd0(tail_player_get(player_index)->unit_index, &player_position);
		function_b9dd0(best->object_index, &best_position);
		function_b9dd0(candidate->object_index, &candidate_position);
		vector3d_from_points3d(&player_position, &candidate_position, &to_candidate);
		vector3d_from_points3d(&player_position, &best_position, &to_best);
		candidate_distance = to_candidate.j * to_candidate.j + to_candidate.i * to_candidate.i + to_candidate.k * to_candidate.k;
		best_distance = length_sq3f(&to_best);
		result = candidate_distance < best_distance;
	}
	if (result)
		*best = *candidate;
	return result;
}


static __forceinline bool player_candidate_has_type(s_target_candidate *candidate, dword type_mask)
{
	bool result = false;
	long object_index = candidate->object_index;

	if (function_bae20(object_index, NONE))
	{
		s_tail_object_header *header = (s_tail_object_header *)g_4e0300->data + (object_index & 0xffff);

		result = (type_mask & (1 << header->type)) != 0;
	}
	return result;
}

__declspec(noinline) bool function_1518f0(s_target_candidate *candidate, dword type_mask);

// @retail 0x1518f0
bool function_1518f0(s_target_candidate *candidate, dword type_mask)
{
	return player_candidate_has_type(candidate, type_mask);
}

PRIVATE __forceinline void local_player_view_clear(s_view_globals *views, short index)
{
	views->entries[index].clear_flags();
}

// @retail 0x1520f0
void function_1520f0(long player_index)
{
	s_tail_player const volatile *player = tail_player_get(player_index);
	short local_index = player->local_index;

	if (local_index != NONE)
		local_player_view_clear((s_view_globals *)g_4ed284, local_index);
}

// @retail 0x152340
void function_152340(void)
{
	s_player_census *census = (s_player_census *)g_4e8c20;
	s_record_pool_iterator iterator;
	s_tail_player *player;

	census->all_without_units = true;
	census->any_available = false;
	iterator.data = g_4e8c24;
	iterator.index = NONE;
	while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
	{
		if (player->unit_index != NONE)
		{
			census->all_without_units = false;
		}
		else if (player->flags & 8)
		{
			census->all_without_units = false;
		}
		else
		{
			census->any_available = true;
		}
	}
}

/* the game options' players (16 of 0xe4 bytes at +0x2dc): an identifier
   and two sets of traits */
struct s_tail_traits
{
	byte unknown00[0x1c];
};

struct s_tail_options_player
{
	byte unknown00[0xe];
	byte identifier[12];
	byte unknown1a[0xac - 0x1a];
	s_tail_traits traits[2];
};

struct s_tail_options
{
	byte unknown00[0x2dc];
	s_tail_options_player players[16];
};

struct s_tail_scenario_entry
{
	byte unknown00[0x2c];
	short value2c;
};

struct s_tail_scenario
{
	byte unknown00[0x100];
	long entry_count;
	s_tail_scenario_entry *entries;
};

struct s_tail_player_identifier
{
	byte unknown00[4];
	byte identifier[12];
};

// @retail 0x1529a0
s_tail_traits *function_1529a0(long player_index)
{
	s_tail_player_identifier *player = (s_tail_player_identifier *)tail_player_get(player_index);
	bool alternate = false;

	if (((s_tail_scenario *)g_4e0350)->entry_count > 0 && ((s_tail_scenario *)g_4e0350)->entries[0].value2c == 1)
		alternate = true;
	long i = 0;
	byte const *identifier = player->identifier;
	s_tail_options_player *entry = ((s_tail_options *)g_4e6948)->players;
	s_tail_traits *result = 0;
	while (i < 16)
	{
		long difference = memcmp(entry->identifier, identifier, sizeof(entry->identifier));

		if (difference == 0)
		{
			result = &entry->traits[alternate ? 1 : 0];
			break;
		}
		++i;
		++entry;
	}
	return result;
}

/* the units and their definitions as the census of 0x152580 reads them */
struct s_tail_unit
{
	long definition_index;
	byte unknown04[0x184 - 4];
	real value184;
	byte unknown188[0x22a - 0x188];
	short value22a;
	short value22c;
};

struct s_tail_seat
{
	byte unknown00[0x88];
	byte value88;
	byte unknown89[0x90 - 0x89];
	long tag90;
	byte unknown94[4];
	long tag98;
};

struct s_tail_unit_definition
{
	byte unknown00[0x1fc];
	byte value1fc;
	byte unknown1fd[0x2c0 - 0x1fd];
	long count2c0;
	byte unknown2c4[0x2d0 - 0x2c4];
	long seat_count;
	s_tail_seat *seats;
};

struct s_tail_seat_definition
{
	byte unknown00[0x128];
	byte value128;
};

struct s_tail_unit_state
{
	short value0;
	short value2;
	short value4;
	short value6;
};

// @retail 0x152580
void function_152580(long unit_index, s_tail_unit_state *state)
{
	s_tail_unit *unit = (s_tail_unit *)((s_tail_object_header *)g_4e0300->data)[unit_index & 0xffff].object;
	s_tail_unit_definition *definition = (s_tail_unit_definition *)g_4e3b44[unit->definition_index & 0xffff].bytes;
	long value = 0;
	real scaled;
	long rounded;

	if (definition->seat_count > 0)
	{
		s_tail_seat *seat = definition->seats;

		if (seat->tag90 != NONE)
		{
			value = ((s_tail_seat_definition *)g_4e3b44[seat->tag90 & 0xffff].bytes)->value128;
		}
		else if (seat->tag98 != NONE)
		{
			value = seat->value88;
		}
	}
	if (!value)
	{
		value = definition->value1fc;
	}
	state->value0 = (short)value;
	scaled = (1.0f - unit->value184) * 100.0f;
	__asm
	{
		fld scaled
		fistp rounded
	}
	state->value6 = (short)rounded;
	if (definition->count2c0 > 0)
	{
		state->value2 = unit->value22a;
		state->value4 = unit->value22c;
	}
}

/* a horizontal forward vector and its left */
struct s_horizontal_axes
{
	vector3f forward;
	vector3f left;
};

real function_30bf0(vector3f *v);

static __forceinline void horizontal_left_set(real i, real j, vector3f *left)
{
	left->i = 0.0f - j;
	left->j = i;
	left->k = 0.0f;
}

// @retail 0x1502e0
void function_1502e0(vector3f const *forward, vector3f const *fallback, s_horizontal_axes *axes)
{
	axes->forward = *forward;
	axes->forward.k = 0.0f;
	if (function_30bf0(&axes->forward) == 0.0f)
	{
		axes->forward = *fallback;
		axes->forward.k = 0.0f;
		if (function_30bf0(&axes->forward) == g_45dbd8)
		{
			axes->forward = *g_4687a8;
		}
	}
	horizontal_left_set(axes->forward.i, axes->forward.j, &axes->left);
}

/* an object is being deleted: the players forget it (one of the object
   deletion callbacks, g_468664) */
// @retail 0x152cf0
void __stdcall function_152cf0(long object_index)
{
	if ((1 << tail_object_get(g_4e0300, object_index)->type) & 3)
	{
		s_record_pool_iterator iterator;
		s_tail_player *player;

		iterator.data = g_4e8c24;
		iterator.index = NONE;
		while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
		{
			if (player->unit_index == object_index)
			{
				s_tail_player *owner = tail_player_get(iterator.datum_index);

				owner->previous_unit_index = owner->unit_index;
				function_14cad0(iterator.datum_index, NONE);
				function_152340();
			}
			if (player->object_index174 == object_index)
			{
				player->object_index174 = NONE;
			}
			if (player->previous_unit_index == object_index)
			{
				player->previous_unit_index = NONE;
			}
		}
	}
	function_187a60(object_index);
}

short __stdcall function_cdeb0(long unit_index, long state_name, short slot, short direction);
long function_cbe60(long unit_index);
bool function_cd660(long unit_index);

// @retail 0x150dd0
bool function_150dd0(long player_index)
{
	s_tail_player *player = tail_player_get(player_index);
	long index = player->unit_index;
	if ((short)function_cdeb0(index, NONE, NONE, 0) == NONE)
		return true;
	else
	{
		if (g_4e6948->state != 2)
		{
			long unit_index = player->unit_index;
			if (function_cbe60(unit_index) == NONE && !function_cd660(unit_index))
				return true;
		}
		return false;
	}
}

struct s_player_motion_unit
{
	byte unknown00[0x150];
	vector3f vector150;
	vector3f vector15c;
	byte unknown168[0x180 - 0x168];
	vector3f vector180;
};

struct s_player_motion_control
{
	byte unknown00[0x28];
	vector3f vector28;
	vector3f vector34;
	vector3f vector40;
	byte unknown4c[0x7c - 0x4c];
};

struct s_unit_state_c6ef0;
void function_c6ef0(s_unit_state_c6ef0 *state);
void function_c6de0(long object_index, void *control);

// @retail 0x150790
void function_150790(long unit_index)
{
	s_player_motion_control control;
	s_player_motion_unit *unit = (s_player_motion_unit *)tail_object_get(g_4e0300, unit_index);
	function_c6ef0((s_unit_state_c6ef0 *)&control);
	control.vector28 = unit->vector150;
	control.vector34 = unit->vector15c;
	control.vector40 = unit->vector180;
	function_c6de0(unit_index, &control);
}

struct s_player_target_object
{
	byte unknown00[0x30];
	point3f position;
	real radius;
	byte unknown40[0x168 - 0x40];
	vector3f direction;
};

void function_cafc0(long unit_index, point3f *position);
bool function_1078f0(long control_index, vector3f const *direction);
bool function_107870(long device_index);

// @retail 0x1516c0
void function_1516c0(long object_index, long player_index, s_target_candidate *best)
{
	long const *player_reference = &player_index;
	s_target_candidate *const *best_reference = &best;
	s_tail_player *player = tail_player_get(*player_reference);
	s_player_target_object *unit = (s_player_target_object *)tail_object_get(g_4e0300, player->unit_index);
	s_player_target_object *object = (s_player_target_object *)tail_object_get(g_4e0300, object_index);
	point3f eye;
	function_cafc0(player->unit_index, &eye);
	if (function_11e5e0(&eye, &unit->direction, &object->position, object->radius) &&
		function_1078f0(object_index, &unit->direction) && function_107870(object_index))
	{
		s_target_candidate candidate;
		memset(&candidate, 0, sizeof(candidate));
		candidate.priority = 3;
		candidate.object_index = object_index;
		function_151320(*player_reference, &candidate, *best_reference);
	}
}

struct s_player_pickup_object
{
	long definition_index;
	byte unknown04[0x30 - 4];
	point3f position;
	byte unknown3c[0x12c - 0x3c];
	dword flags;
	byte unknown130[0x14c - 0x130];
	long field_14c;
};

struct s_player_pickup_definition
{
	byte unknown00[0x290];
	short type;
};

bool __stdcall function_cd7b0(long unit_index, long weapon_index, bool *modes);

/* two of the four answers of 0xcd7b0 */
struct s_pickup_hands
{
	bool primary;
	bool secondary;
};

// @retail 0x151780
void function_151780(long object_index, long player_index, s_target_candidate *best)
{
	long const *player_reference = &player_index;
	s_target_candidate *const *best_reference = &best;
	s_tail_player *player = tail_player_get(*player_reference);
	s_player_target_object *unit = (s_player_target_object *)tail_object_get(g_4e0300, player->unit_index);
	s_player_pickup_object *object = (s_player_pickup_object *)tail_object_get(g_4e0300, object_index);
	s_player_pickup_definition *definition = (s_player_pickup_definition *)g_4e3b44[object->definition_index & 0xffff].bytes;
	bool special = definition->type != 0;
	real radius = unit->radius + (special ? 0.1f : 0.0f) + 0.3f;
	if (!(object->flags & 1) && object->field_14c != player->unit_index)
	{
		vector3f delta;
		vector3d_from_points3d(&unit->position, &object->position, &delta);
		real distance = delta.j * delta.j + delta.i * delta.i + delta.k * delta.k;
		s_pickup_hands hands[2];
		if (distance < radius * radius && function_cd7b0(player->unit_index, object_index, &hands[0].primary))
		{
			s_pickup_hands first = hands[0];
			s_pickup_hands second = hands[1];
			long flags = 0;
			if (first.primary)
				flags = 1;
			else if (second.primary)
				flags = 2;
			if (first.secondary)
				flags |= 4;
			else if (second.secondary)
				flags |= 8;
			s_target_candidate candidate;
			memset(&candidate, 0, sizeof(candidate));
			candidate.object_index = object_index;
			candidate.flags = (byte)flags;
			candidate.priority = special ? 7 : 1;
			function_151320(*player_reference, &candidate, *best_reference);
		}
	}
}

struct s_player_removed_unit
{
	byte unknown00[0x10a];
	word flags;
	byte unknown10c[0x2a8 - 0x10c];
	long player_index;
};

struct s_player_census_counts
{
	long count;
	byte unknown04[4];
	short user_count;
	short controller_count;
	long users[4];
	long controllers[4];
	dword machine_valid_mask;
	byte machine_addresses[16][6];
	bool local_machine_valid;
	byte local_machine_address[6];
	byte unknown97;
	long local_machine_index;
};

extern byte g_4cf77b;

class c_class_6a600;
void function_69580(c_class_6a600 *world, long player_index);
void function_b7360(long object_index);
void player_control_set_unit(long user_index, long unit_index);
void function_1682bf(long unit_index, long user_index, long character_index);
void function_157de0(long player_index);

// @retail 0x152df0
void function_152df0(long player_index)
{
	long const *player_reference = &player_index;
	s_tail_player *player = tail_player_get(*player_reference);
	long unit_index = player->unit_index;
	if (unit_index != NONE)
	{
		s_player_removed_unit *unit = (s_player_removed_unit *)tail_object_get(g_4e0300, unit_index);
		word *flags = &unit->flags;
		*flags |= 0x400;
		function_b7360(unit_index);
	}
	if (g_4e6948->mode == 4)
	{
		struct
		{
			s_player_removed_unit *unit;
			s_type_f1af8e iterator;
		} state;
		state.iterator.signature = 0x86868686;
		state.iterator.type_mask = 3;
		state.iterator.flags = 0;
		state.iterator.index = 0;
		state.iterator.object_index = NONE;
		while ((state.unit = (s_player_removed_unit *)function_baeb0(&state.iterator)) != 0)
		{
			if (state.unit->player_index == *player_reference)
				state.unit->player_index = NONE;
		}
	}
	function_14cad0(*player_reference, NONE);
	player->previous_unit_index = NONE;
	/* read through its address: retail loads the flag after the previous_unit_index store */
	if (*(byte const *)&g_4cf770 && !g_4cf77b)
		function_69580((c_class_6a600 *)g_4cf77c, *player_reference);
	player->machine_index = NONE;
	memset(player->machine_address, 0, sizeof(player->machine_address));
	player->machine_user = NONE;
	player->machine_controller = NONE;
	s_tail_player *local_player = tail_player_get(*player_reference);
	if (local_player->local_index != NONE)
	{
		player_control_set_unit(local_player->local_index, NONE);
		function_1682bf(NONE, local_player->local_index, NONE);
		s_player_census_counts *globals = (s_player_census_counts *)g_4e8c20;
		globals->users[local_player->local_index] = NONE;
		globals->user_count--;
		local_player->local_index = NONE;
	}
	s_tail_player *controller_player = tail_player_get(*player_reference);
	if (controller_player->controller_index != NONE)
	{
		s_player_census_counts *globals = (s_player_census_counts *)g_4e8c20;
		globals->controllers[controller_player->controller_index] = NONE;
		globals->controller_count--;
		controller_player->controller_index = NONE;
	}
	player->flags |= 2;
	player->leave_time = g_510c54->game_time;
	player->flags &= ~1;
	((s_player_census_counts *)g_4e8c20)->count--;
	function_157de0(*player_reference);
}

struct s_player_target_unit
{
	byte unknown00[0x30];
	point3f position;
	real radius;
	byte unknown40[0x10a - 0x40];
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word : 13;
	byte unknown10c[0x248 - 0x10c];
	long parent_index;
	byte unknown24c[0x348 - 0x24c];
	word flag348_0 : 1;
	word flag348_1 : 1;
	word flag348_2 : 1;
	word flag348_3 : 1;
	word flag348_4 : 1;
	word flag348_5 : 1;
	word flag348_6 : 1;
	word : 9;
};

__declspec(noinline) bool function_f5dc0(long object_index);
bool function_cc0c0(long unit_index);
short __stdcall function_c8ef0(long unit_index, long object_index, long *target_index, short *seat_index);

void function_151430(long object_index, long player_index, s_target_candidate *best);

static __forceinline void player_object_target_candidate_evaluate(long object_index, long player_index, s_target_candidate *best)
{
	long const *player_reference = &player_index;
	s_target_candidate *const *best_reference = &best;
	s_tail_player *player = tail_player_get(*player_reference);
	s_player_target_unit *object = (s_player_target_unit *)tail_object_get(g_4e0300, object_index);
	if (!TEST_FIELD_BIT(object->flag2))
	{
		if (!function_f5dc0(object_index))
		{
			long unit_index = player->unit_index;
			if (!function_cc0c0(unit_index) && !(player->flags & 0x800))
			{
				long target_index = NONE;
				long seat_index = NONE;
				short type = function_c8ef0(unit_index, object_index, &target_index, (short *)&seat_index);
				if (type)
				{
					s_target_candidate candidate;
					memset(&candidate, 0, sizeof(candidate));
					switch (type)
					{
					case 1: candidate.priority = 4; break;
					case 2: candidate.priority = 5; break;
					case 3: candidate.priority = 6; break;
					}
					candidate.object_index = target_index;
					*(short *)&candidate.flags = (short)seat_index;
					function_151320(*player_reference, &candidate, *best_reference);
				}
			}
		}
		else if (player->unit_index != NONE)
		{
			s_player_target_unit *unit = (s_player_target_unit *)tail_object_get(g_4e0300, player->unit_index);
			if (!TEST_FIELD_BIT(object->flag348_6) && object->parent_index == NONE)
			{
				vector3f delta;
				vector3d_from_points3d(&unit->position, &object->position, &delta);
				real radius = unit->radius + object->radius + 0.2f;
				real distance = delta.j * delta.j + delta.i * delta.i + delta.k * delta.k;
				if (distance < radius * radius)
				{
					s_target_candidate candidate;
					memset(&candidate, 0, sizeof(candidate));
					candidate.priority = 8;
					candidate.object_index = object_index;
					function_151320(*player_reference, &candidate, *best_reference);
				}
			}
		}
	}
}

// @retail 0x151430
void function_151430(long object_index, long player_index, s_target_candidate *best)
{
	player_object_target_candidate_evaluate(object_index, player_index, best);
}

struct s_tail_transfer_options
{
	byte unknown00[0x2d4];
	s_tail_options_player players[16];
};

struct s_tail_transfer_unit
{
	byte unknown00[0x1fc];
	short seat_index;
	byte unknown1fe[0x212 - 0x1fe];
	char primary_slot;
	char secondary_slot;
	byte unknown214[4];
	long weapons[4];
	long weapon_times[4];
	byte unknown238[6];
	byte grenades[2];
};

struct s_tail_transfer_traits
{
	bool valid;
	byte unknown01;
	s_tail_unit_state weapons[3];
	byte grenades[2];
};

// score 153 (was 153): unchanged this round: the first 0x11c bytes agree with retail; from the
// selection loop on, the traits pointer comes back in EBP where retail keeps it in EAX, and our three
// NONE homes sit at +0x18/+0x20/+0x28 against retail's two at +0x1c/+0x24.
// MiniMax S (11-11): no new form, no lower score (153): the traits pointer comes back from the selection loop in ebp where retail keeps it in eax, and our three NONE homes sit at +0x18/+0x20/+0x28 against retail's two at +0x1c/+0x24.
// MiniMax C (11-10): the score is 153 (was 153); the traits pointer comes back from the selection loop in EBP where retail keeps it in EAX and the unit index takes the other register, and our three NONE homes sit at +0x18/+0x20/+0x28 against retail's two at +0x1c/+0x24.
// MiniMax D (11-10): no new form, no lower score (153); the selected traits, the unit index and the NONE home are
// still kept in different registers and at different stack offsets than retail, and the no-seat branch still
// reuses the unit pointer where retail re-reads the object table and the unit pointer there.
// MiniMax D (11-08): the difference starts at the selection loop's unit index - the
// unit pointer, the chosen weapon, the age and the NONE home are kept in different registers and at different
// stack offsets than retail.
// MiniMax B (11-06): no new form, no lower score; the first 0x11c bytes agree with retail and the rest is
// allocation: retail puts the unit index in EAX and NONE in EDI and reloads the object table from the
// global for the seat test, this build keeps the unit index in EBP, NONE in EDX and the table in ESI.
// MiniMax C (11-06): no new form, no lower score; the traits, unit and NONE homes and the pool reloads remain.
// MiniMax G (11-06): no new form, no lower score; the first 0x11c bytes agree with retail and the rest is
// allocation: retail keeps the unit index in ebp and NONE in edi and spills the selected traits to the
// stack, this build keeps the traits in ebp and the unit index in eax, with the four homes shifted.
// Reading the game time once before the selection loop, the way retail keeps it in ebx, scores 164 and is
// not kept; retail's unrolled four slots are not reachable from the loop form.
// MiniMax D (11-04): no new form, no lower score; the unit index masked once into a local 153 (same code),
// the seat-index branch still splits the object table and the unit index over different registers.
// logic walked against retail, no behaviour difference found (loops, offsets, order of the three
// weapon slots, grenade copy and the memset path agree). Only allocation differs: retail keeps the player in ebx, the
// unit index in ebp and NONE in edi, and re-reads g_4e0300->data and the unit pointer in the no-seat branch (0x1528af,
// 0x1528d4) while ours reuses the unit pointer in esi. Tried: primary/secondary/other declared before the unit_index
// read (159), the three weapons as a long[3] (152), a held[2] array with the no-seat branch as a hand loop over
// held_slots[] (151, frame grows by 8), held[2] replacing primary/secondary (156), a store through a pointer to other (153).
// AC502: Failed-traits continuation/missing-unit completion/both 153/171/171; real
// identifier cursor/copy cursors/search plus continuation/both cursors/all admissions
// 217/258/217/270/283. Original traversals and body restored; selected-traits/unit/NONE homes remain.
// Round 552: existing shared current-weapon producer, all shared object getters, native selection
// do-while, admitted age, once/per-use clock, weapon-result home names and per-use second lookup:
// 154/154/154/180/165/165/154/154. All complete original source and existing helper bodies restored.
// NOT kept: no score gain or byte match.
// Round 51 (m3 lane 15): clock read once before the selection loop 164; alternate as one bool expression 175;
// primary through the existing unit_get_current_weapon 154; results declared other/secondary/primary 153; no-seat
// arm first 169. Retail's no-seat primary shares the selection counter's slot and lives in EDI in the seat arm.
// DH319: Last-match array ordinal long/dword/short/byte, range admission
// 189/189/196/197; unequal-sentinel admission 189/189/196/197. Original
// pointer selection restored; selected-traits/unit/NONE homes and pool reloads remain.
// Branch-owned primary/secondary/other default subsets 1/2/4/3/5/6/7:
// 157/158/162/162/152/160/151; complete conditional slot producers 151.
// NOT kept: at most two points and unmatched lifetime interaction. Original
// defaults and body restored; selected-traits/unit/NONE homes and reloads remain.
// AC512: Readonly player/main-unit/no-seat-unit contracts, seven subsets 153
// each; complete graph with selected-output lexical identity 153. All restored;
// selected-traits/unit/NONE homes and native no-seat pool reloads remain.
// B397: Native traits-element/player-identifier domains and complete inner choice
// eligibility subsets, then complete rejection complement 153/153/153/153/153/153/153/153.
// Original layouts/tests restored; traits/unit/NONE homes and pool reloads remain.
// Y574: Actual weapon-output zero-first arms, seven subsets, then two-byte library grenade
// publication: 153/153/153/155/153/153/155/152. NOT kept: no gain; native traits/unit/NONE homes
// and object-table reloads remain. Original complete body and helpers restored.
// N355: Returned-player canonical for traversal with independent bounded selection/slot domains:
// 373/378/413/373/284/284/373/283. NOT kept: no exact match from the tested structural forms.
// Complete original executable source restored; Selected-traits/unit/NONE homes and pool reloads remain.
// N370: Actual handle-field/unit-record/selected-output references, seven subsets and complete scalar role identities:
// 153/153/153/153/153/153/153/153. NOT kept: no equality; original complete body restored.
// Selected traits/unit/NONE homes and native no-seat pool reloads remain.
// Y576: Winning weapon as long/short/signed-byte slot or element receiver, original
// for traversal 199/217/206/253; plain while 199/217/206/253. NOT kept: alternate
// winner producers grow beyond retail and leave traits/unit/NONE homes and reloads unresolved.
// F377: Last matched options-entry/traits-array results, pointer/reference outputs and
// original/early unit-index producers: 196/196/201/201/224/224/218/218. NOT kept:
// no gain; original full body restored; selected-traits/unit/NONE homes and reloads remain.
// P325: Iteration-entry unit-index/unit-receiver/weapon-result declaration subsets
// 1..7 all tie 153; the complete graph with both no-seat slot declarations also ties 153.
// All initializers and observations stayed at their original producers. Complete original
// body restored; selected-traits/unit/NONE homes and native pool reloads remain.
// Y597: Independent minimum-scan rejection continuations, six guard orders, then
// weapon/grouped-rejection and age/grouped-rejection: 153/169/151/153/180/153/153/180.
// NOT kept: two-point best gain below the gate; other forms tie or regress. Original
// scan, observations and whole body restored; traits/unit/NONE homes and reloads remain.
// F380: Direct primary/other winner publication, scalar arms/conditional destinations with
// long/dword/short/byte passes: 176/176/185/177/171/171/180/184. NOT kept: regressions
// and nonnative frame/extent; original chosen results, defaults, observations and body restored.
// F382: Selected-traits, unit-receiver and selection-scalar lexical home groups,
// seven subsets then all with the transfer parameter role name: eight 153 at 826 bytes.
// NOT kept: no gain; original names, initializer sites, helpers and complete body restored.
// DL265: Weapon/timestamp walks, weapon/time/both, dword/short/word selectors, range end;
// matched serializer inline: 172/153/163/163/268/268/304/153. NOT kept: no eligible native result; original
// traversals, serializer and body restored; traits/unit/NONE homes and pool reloads remain.
// Y597: Independent minimum-scan rejection continuations, six guard orders, then
// weapon/grouped-rejection and age/grouped-rejection: 153/169/151/153/180/153/153/180.
// NOT kept: two-point best gain below the gate; other forms tie or regress. Original
// scan, observations and whole body restored; traits/unit/NONE homes and reloads remain.
// Round 73 (m3 lane 11, fast): unit_index read before the identifier scan 193; `!memcmp(...)` without the
// difference local 240 (804 bytes); `traits[alternate]` 196 (809 bytes); primary/secondary/other declared before
// the unit_index read 159; selection arms swapped 153.
// Round 78 (m3 lane 11, fast): the main unit lookup through a pool copy `objects`, the no-seat lookups on the global 157 (831
// bytes; retail's `shl ecx, 2` offset and the first no-seat reload appear); the first no-seat lookup through the copy instead
// 157; main+second, first+second, all three, main+first through copies 171 each (843 bytes). Still: retail keeps primary in EDI
// (its NONE preset shares the NONE constant's register, its home the selection counter's slot) where ours keeps it on the
// stack, and the unit index in EBP where ours uses EAX.
// P393: Selected-traits availability at the unit join, positive/complementary
// bool/byte/short/long: 153/153/153/153/153/153/153/153. NOT kept: unmatched one-use outcomes; native
// traits/unit/NONE homes and no-seat pool reloads remain. Original complete body restored.
// P402: Shared NONE production, full forward/reverse chains, three paired chains, initialized complete
// declaration, separate assignments and complete declaration with chain:
// 153/153/153/153/153/153/153/153. NOT kept: no eligible native default/home recovery; original defaults and
// complete body restored. Traits/unit/NONE and no-seat pool reloads remain.
// T78: Per-pass first/exclusion truths, conditional/default/primary-seeded excluded handles,
// slot admission/rejection predicates and conditional first-pass admission: 150/150/188/185/185/153/153/202.
// NOT kept: no eligible match; best three points is below the gain gate. Original complete
// body restored; selected-traits/unit/NONE homes and native no-seat reloads remain.
// Round 172 (m3 lane 5, fast; aligned/masked counts in brackets, control [106/23]): the no-seat primary/secondary
// through the existing 0xcbd50 with the slots read from fresh lookups 153 (816 bytes) [94/35] (same code: the main
// lookup through a pool copy with the slots on the global, primary through unit_get_current_weapon, the slots read
// from `unit`); with a pool copy `objects` for the main lookup and both slot reads 161 (826) [76/20]; with the copy
// declared in the no-seat arm 162 (835); 0xcbd50 with `long primary;` left without its NONE preset (every path
// assigns it) 150 (826, frame 0x28 as retail, 0xcbd50's `or r, -1` defaults appear) [94/27]; the copy form with
// primary and other unpreset 148 (828) [74/20], other only 147 (838) [90/16], primary only 168 (840); 0xcbd50 with
// other only 151 (826), both 156 (816); re-measured on these bases (round 51's listed clock-first and no-seat-first
// forms): the 150 form plus the clock before the selection loop 162, the 147 form plus the clock 157, the 147 form
// with the seat arms swapped 165; new on the 147 form: cbe60 first 173, `valid` stored before the grenades 147; the
// 147 form with the primary slot on the global and the secondary through the copy 144 (833) [94/16] (a copy/global
// mix, not keepable), the reverse 150. Same code: the preset declarations in three orders, chained NONE presets.
// Diagnostic: the 147 form with a dummy second use of the copy 143: the first no-seat query then reloads the data and
// shares one unit lookup between the slot read and 0xcbd50's weapon read (retail's block); the second query still
// reuses it. Observed in these trials (a lead, not a rule): 0xcbd50's lookup shares loads with caller lookups on the
// global, not with lookups through a pool copy that stays a real variable (used twice or more; a single-use copy is
// propagated). Still missing: a natural form that keeps the copy real.
// M3 lane 12 r118 (fast, 0x152580 and 0xcbe60 still matched): primary alone declared (NONE) before the unit-index test 155;
// a `weapons` pointer local for the three weapon publications 184 (778 B); `alternate` declared after the trait copy loop 170
// (first difference at +0x0); shortest declared before chosen 153 (same code); the no-seat slots read from the fields at
// each use (no slot locals) 152 (gain under the gate). On that 152 form: `other` without its NONE preset 167 (808 B); `valid`
// stored before the grenades 152 (same code); the no-seat primary through the existing 0xcbd50 (slot from a fresh lookup, primary
// unpreset) 160 (816 B); the no-seat secondary through 0xcbd50 158 (812 B); `other` unpreset with `valid` first 167. Body restored.
// P438: Original scenario/unit-index/unit/first/second and per-pass clock/age/weapon
// bindings fixed independently: eight 153. NOT kept: selected-traits/unit/NONE homes
// and no-seat reloads unchanged; original complete body restored.
// T102: Complete primary-traits selection polarity in bool/byte/char/signed-char/
// short/word/long/dword: 156/156/156/156/255/255/242/242. NOT kept: no exact
// selected-traits/unit/NONE-home repair; original selector and complete body restored.
// @retail 0x152660
void function_152660(s_tail_transfer_options *options)
{
	bool alternate = false;
	for (long i = 0; i < 16; i++)
		memcpy(options->players[i].traits, ((s_tail_options *)g_4e6948)->players[i].traits, sizeof(options->players[i].traits));
	s_tail_scenario *scenario = (s_tail_scenario *)g_4e0350;
	if (scenario->entry_count > 0 && scenario->entries[0].value2c == 1)
		alternate = true;
	s_record_pool_iterator iterator;
	iterator.data = g_4e8c24;
	iterator.index = NONE;
	s_tail_player *player;
	while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
	{
		s_tail_transfer_traits *traits = 0;
		for (long i = 0; i < 16; i++)
		{
			long difference = memcmp(((s_tail_player_identifier *)player)->identifier, options->players[i].identifier, 12);
			if (difference == 0)
				traits = (s_tail_transfer_traits *)&options->players[i].traits[alternate ? 1 : 0];
		}
		if (traits)
		{
			long unit_index = player->unit_index;
			if (unit_index != NONE)
			{
				s_tail_transfer_unit *unit = (s_tail_transfer_unit *)tail_object_get(g_4e0300, unit_index);
				long primary = NONE;
				long secondary = NONE;
				long other = NONE;
				if (unit->seat_index != NONE)
				{
					for (long selection = 0; selection < 2; selection++)
					{
						long now = g_510c54->game_time;
						long chosen = NONE;
						long shortest = 0x7fffffff;
						for (long slot = 0; slot < 4; slot++)
						{
							long age = now - unit->weapon_times[slot];
							long weapon = unit->weapons[slot];
							if (weapon != NONE && (!selection || weapon != primary) && age < shortest)
							{
								shortest = age;
								chosen = weapon;
							}
						}
						if (!selection)
							primary = chosen;
						else
							other = chosen;
					}
				}
				else
				{
					s_tail_transfer_unit *first = (s_tail_transfer_unit *)tail_object_get(g_4e0300, unit_index);
					short primary_slot = first->primary_slot;
					if (primary_slot != NONE)
						primary = first->weapons[primary_slot];
					s_tail_transfer_unit *second = (s_tail_transfer_unit *)tail_object_get(g_4e0300, unit_index);
					short secondary_slot = second->secondary_slot;
					if (secondary_slot != NONE)
						secondary = second->weapons[secondary_slot];
					other = function_cbe60(unit_index);
				}
				if (primary != NONE)
					function_152580(primary, &traits->weapons[0]);
				else
					traits->weapons[0].value0 = 0;
				if (secondary != NONE)
					function_152580(secondary, &traits->weapons[2]);
				else
					traits->weapons[2].value0 = 0;
				if (other != NONE)
					function_152580(other, &traits->weapons[1]);
				else
					traits->weapons[1].value0 = 0;
				traits->grenades[0] = unit->grenades[0];
				traits->grenades[1] = unit->grenades[1];
				traits->valid = true;
			}
			else
				memset(traits, 0, sizeof(*traits));
		}
	}
}

struct s_player_pickup_unit
{
	byte unknown00[0x14];
	long parent_index;
	byte unknown18[0x212 - 0x18];
	char held_slots[2];
	byte unknown214[2];
	char pending_slots[2];
	long weapons[4];
};

struct s_player_weapon_definition
{
	byte unknown00[0x12c];
	dword flags;
};

static __forceinline dword player_weapon_flags(s_tag_instance *tags, long weapon_index)
{
	s_player_pickup_object *weapon = (s_player_pickup_object *)tail_object_get(g_4e0300, weapon_index);
	return ((s_player_weapon_definition *)tags[weapon->definition_index & 0xffff].bytes)->flags;
}

long function_cbd50(long unit_index, short weapon_index);

// score 215 (was 215): the tag table stays enregistered (EBX) through the first loop, so the player spills (frame 0x1c against 0x18).
// MiniMax S (11-11): no new form, no lower score (215): retail computes the pool element address before the prologue's register saves and keeps it whole in ebp, while this build splits the multiply from the base add, keeps the pool pointer in a register of its own and frames 0x1c against retail's 0x18.
// MiniMax G (11-12): no new form, no lower score (215): eight forms tried this round (unit local dropped or re-derived, pool and tag table hoisted or reloaded, player in a register or spilled) all score above 215; retail still inlines the weapon-slot lookup and derives the unit twice per iteration where ours calls function_cbd50.
// MiniMax E (11-09): no new form, no lower score (254); taking the tag table straight from the global in every loop reaches retail's 1006 bytes but scores 275, and hoisting the copy to the declaration keeps the same 254 with the same code.
// MiniMax H (11-06): the permuter's best form (246) needs a volatile cast, hand-written helper bodies and a
// player expression that reads differently, so only its legal part was kept and it gives 284; the tag table
// declared without an initializer and the first loop's stores written through byte pointers both give 254,
// the same code; the two mode stores of the first loop written through a byte pointer give 254 again, the
// same code, so the store's type does not decide the table's reload.
// MiniMax H (11-06): retail keeps g_4e0300 in edi, the player in ebp and the weapon index in a stack slot,
// spills the unit at [esp+0x14] and loads g_4e3b44 inside the two arms of the first loop, keeping it in ecx
// for the rest; ours keeps the pool in ebx, the tag table in ebp and the unit in ecx, and the loads and the
// frame slots are the first difference.
// MiniMax H (11-06): the flag helper takes the tag table as an argument and 0x1509e0 keeps the table in a
// local, so the table is a local copy rather than a hoisted global; the score drops to 254 and the first
// difference moves to +0x9, where retail loads the player pool's data before the index arithmetic.
// MiniMax H (11-06): the pool pointer as a local, with every lookup through pool->data, gives retail's shape of
// a held pool pointer with the data re-read at each use, but the pool lands in EBX, the player stays on the
// stack and the frame grows to 0x1c, 296; the score stays 284.
// MiniMax H (11-06): writing both slot lookups as if/else instead of ?: puts the branch between the two unit
// derivations, as in retail, and the score drops to 280; the player still lands in ebx instead of ebp, the pool
// is still re-read from the global in the loop and g_4e3b44 is still hoisted into ebp.
// MiniMax H (11-06): the shared object getter takes the pool and 0x1509e0 keeps it in a local, so the pool
// pointer is held in a register and its data is re-read at each use, as in retail; the score drops to 275 and
// the length is retail's 1006 bytes; the player still lands in ebx instead of ebp, the pool in ebp instead of
// edi, and g_4e3b44 is still hoisted into edi instead of being loaded into ecx inside the loop.
// held and pending now come from the existing slot-to-weapon function 0xcbd50 (the idiom matched
// 0x1e83c0 uses), which gives retail's per-slot `or r, -1; cmp si, -1; je; movsx; mov` code and the pool's data reload
// inside each lookup, and the tag table is read from its global at each flag test (no `tags` local). Still different:
// our build keeps the tag table in EBX for the whole function and the pool in EBP, so the player spills to the stack
// (frame 0x1c against 0x18); retail keeps the player in EBP and the pool in EDI and loads the tag table lazily into ECX.
// Earlier, at 254 (was 275):
// The logic matches retail block by block (reads, flag bits 3/4/22/23, stores, count >= 2 pass); only the
// register allocation differs. Retail keeps g_4e0300 in edi for the whole function, keeps the player in ebp
// and loads g_4e3b44 lazily into ecx inside the loop; this build keeps the pool in ebx, the player in eax and
// hoists g_4e3b44 into ebp, so the prologue and the first loop differ and the rest of the function shifts
// with them.
// register allocation differs. Retail keeps g_4e0300 in edi for the whole function and loads g_4e3b44 lazily
// into ecx, with the player in ebp; this build reloads g_4e0300 every iteration of the first loop and hoists
// g_4e3b44 into ebp, so the player and temporaries land in other registers. Retail's first loop is not
// aligned (no pad before it), ours is.
// Tried (scores): taking the parameters' addresses (322, the old form, 962 bytes), `if/else` instead of `?:`
// for the slot lookups (286), locals for held/pending weapon (295), inline helpers for the held/pending weapon
// lookups (295), writes through &held[hand] (287), a local copy of g_4e0300 passed to helpers (286, 1019
// bytes), one 4-entry array for held/pending (295), a struct for them (292), do-while with a hand<2 guard (284),
// the pool pointer as a local with the getter taking the pool (275), the same with the lookup written out (275),
// the same with the modes stored as bytes (275) and the same with the flag test nested (275), the tag table as
// a local passed to the flag helper (254), the same with the local set at the top of the loop body (264), the
// same with the local read inside the loop (264) and the same with the modes stored as bytes (264).
// bytes), one 4-entry array for held/pending (295), a struct for them (292), do-while with a hand<2 guard (284).
// AC502: Shared object getter at initial/held/pending/flag lookups, complete main/all
// receivers, immutable receivers and direct tag-field access: 284 each. Original getters,
// include set and body restored; pool/player/tag-table allocation remains.
// Round 552: original ternary producers with direct first-loop do/while, then native slot/final/both
// loops on each entry graph 284/636/284/636/284/636/284/636. All loops, observations, helper and complete body restored.
// NOT kept: no score gain or byte match.
// P295: Original ternary results with delayed held/both publication, completed held-slot scope,
// immutable results and readonly slot receivers: 284/281/284/281/284/281/284/281.
// NOT kept: three-point gain and one-use pending receiver; pool/player/tag carriers remain.
// Y559: Bounded slot count char/short/int/dword, entry then count-phase zero
// declaration: 287/283/284/284/305/293/294/294. NOT kept: one-point lead below
// the keep gate; retained pool/player and lazy tag-table ownership remain.
// Y561: Initial hand selector char/short/word/dword 345/345/345/284;
// final hand selector char/short/word/dword 283/282/282/284. NOT kept:
// two-point best below the gate; native full-width selectors/body restored.
// Y563: Count-slot rejection guards/held guard/Boolean accumulation/rejection plus
// accumulation, final guarded/early-exit tail: 284/284/318/318/284/284/318/318. NOT kept:
// no gain; original count admission, field observations and complete body restored.
// N353: Existing complete slot selector at held/pending/inventory subsets 1-7
// 288/294/295/337/337/351/346; all with direct slot arguments 346.
// NOT kept: no qualifying gain; original pool/player/tag acquisition and body restored.
// DH340: Initial held/pending/modes array walks, subsets 1/2/4/3/5/6/7 then complete
// plain-step graph: 275/275/284/208/275/275/208/208. NOT kept: the 275 forms grow
// to 1012 bytes; paired 208 forms keep 1006 bytes but grow frame 0x18 to 0x1c and
// add a one-use pending receiver. Original indexed producers and full body restored.
// DH349: Existing nested player/object getter policies ordinary/forced/unhinted,
// eight nonbaseline pairs, all 284. Original getters and full body restored;
// native pool/player retention and delayed tag-table acquisition remain.
// DH351: Existing raw flag pointer/reference returns with local/direct object input
// 284 each; native bit-field pointer/reference returns 253 each; bit-field value return
// 290. NOT kept: pointer forms add a flag-address calculation absent in retail; value form regresses.
// F382: Direct held/pending array-element producers, default-valid/exhaustive-valid/
// exhaustive-sentinel arms at individual sites and asymmetric pairs: 293/279/283/277/283/277/287/289.
// NOT kept: best gain seven is below the gate; original ternaries and full body restored.
// N372: Initial/attached chained false stores and absent-held continuation, seven subsets,
// then complete flag rejection: 284 each at 1006 bytes with frame 0x18. Original body
// restored; retained pool/player and lazy tag-table homes remain.
// DL265: Flag producer table-base domains late/early, then caller-owned object input/byte output:
// 284/284/284/284/284/284/284/284/284/284. NOT kept: unmatched base/input/output ownership; helper/body restored.
// Retained pool/player and lazy tag-table allocation remain.
// N384: Existing whole-metadata pointer/reference result, mutable/read-only and
// captured/direct object input: eight 284. No field-address result is introduced, but
// native pool/player/tag homes remain. Original getter and entire body restored.
// Round 73 (m3 lane 11, fast): held/pending as a NONE default then a slot test 286; positive ternaries 282; one
// unit_index local for the three lookups 300 (992 bytes); a short hand counter 345 (1053 bytes); the flags helper
// as plain inline 284. Diagnostics: a char store through modes between the held and pending lookups gives retail's
// first loop (g_4e0300 hoisted, both lookups redone, no loop pad, lazy g_4e3b44) at 308; held/pending stores
// through element pointers do not (292).
// Round 96 (m3 lane 12, fast): held/pending as a NONE store then a guarded store 286; both through the matched slot-to-weapon
// function 0xcbd50 295 (retail's exact per-slot `or r, -1; cmp si, -1; je; movsx; mov` code, but the held and pending lookups stay
// shared and the frame grows to 0x1c), with the mode clears chained 295, with the held value in a named local for the store and
// the flag test 295. Diagnostics: that form with the arrays' addresses taken 288
// (1009 bytes), the original with them taken 301 (1020 bytes), held stored through an element pointer 287: none redoes the lookups.
// Round 78 (m3 lane 11, fast): per-lookup pool copies (held_objects / pending_objects, as 0xe8de0 reads its hands) declared
// first with the named current/next pointers 303 (1020 bytes; the held, pending and flag lookups each reload pool->data from
// EDI as retail, but the player spills and the frame grows to 0x1c), declared after `pool` 303, written in place 303, the entry
// unit through the global 303, one copy for pending only 303; the copies with held/pending as ternaries 285 (1006 bytes, frame
// 0x18, pool in EDI; the player lands in EBX, the tag table is hoisted into EBP), with `tags = g_4e3b44` at the loop top or
// before the flag test 285/285, with the tags before the test and if/else stores 291; the ternaries alone 292; the copies with
// a NONE store then a guarded store 279 (aligned rows 148 against 227: retail's reloads, but held and pending share one
// unit_index read where retail re-reads [ebp+0x2c], and the loop top is still padded); the existing 0xcbd50 for held with a
// pending copy and if/else, ternary or 0xcbd50 pending 309/305/293; the copies with 0xcbd50 for both 276 (1010 bytes, four
// lookups per iteration); player pointer copies per lookup 279 (same code); the player re-derived per lookup through
// tail_player_get with the NONE store, with ternaries, alone 289/306/297; a file helper taking the pool and the player 642
// (647 bytes).
// P393: Attachment admission before the paired clear traversal, positive/complementary
// bool/byte/short/long: 254/254/254/254/254/254/254/254. NOT kept: one-use outcomes remain unmatched;
// pool/player carriers and lazy tag-table acquisition remain. Original whole body restored.
// Round 160 (m3 lane 6, fast, confirmed plain): NONE-default weapon locals stored after both lookups 303 (992 bytes); a
// NONE store then a guarded store with the pool global 286; 0xcbd50 for both with the `tags` local kept 215, with the
// pool global as well 293, with both slots read from the entry unit 308 (1023 bytes); 0xcbd50 with the tag-table global
// and the pool global 295. On the kept form: the existing bit-3 getter 0x101240 for the held test 215, the existing
// bit-22/23 getter 0x101640 for the count test 213, both 213 (both inline; gain under 8, not kept).
// P402: Attachment output walk, lower/upper range, direct hand count, while range and cursor stepped at use:
// 263/300/252/254/263/300/263/300. NOT kept: best native-length form gains only two; original indexed
// attachment clear and entire body restored. Pool/player and lazy tag-table homes remain.
// T77: Live unit-index field references mutable/read-only and pointer before pool acquisition
// 303/303/303; first-loop-only references 254/254; real weapon/paired-slot array references
// 254/254; paired selected-slot field references 280. NOT kept: no native pool/player/tag
// ownership equality; original lookups, observations and complete body restored.
// Round 172 (m3 lane 5, fast): the pool copy declared before the player lookup 215 (identical code); the bit-3 held
// flag test moved ahead of the pending lookup 309 (1020 bytes); the last loop's arms as `if (hand == 0) ... else if
// (held[1] ...)` 228; the entry unit lookup moved down to the parent test 286. Diagnostics (copy/global mixes, not
// keepable): the held slot through the pool copy and the pending slot on the global 295 (1018 bytes, aligned 72
// against 155: the pending query then gets its own data load as retail), the reverse 293 (1022 bytes); both slots on
// the global 295 (the copy is then propagated, round 160's form). See 0x152660's note: in these trials 0xcbd50's own
// lookup shared loads with caller lookups on the global, not with lookups through a pool copy that stays a real
// variable; every way found of keeping the copy real while the slot reads use the global is a copy/global mix. No
// other untried natural form is left (the notes above cover lookups, selectors, scopes, types, flag producers and
// walks).
// MiniMax B (11-11): no new form, no lower score (215); retail computes the pool element address before the
// prologue's register saves and keeps it whole in ebp, while this build splits the multiply from the base add,
// keeps the pool pointer in a register of its own and frames 0x1c against retail's 0x18.
// MiniMax G (11-11): no new form, no lower score (215); retail keeps the player in ebp and the pool in edi and
// reloads the tag table inside the loop, this build keeps the tag table in ebx and the pool in ebp and spills the
// player to a stack slot, so the frame is 0x1c against retail's 0x18; the pool global used directly (295), the tag
// table read inside the loop or before it (215 each), the helper reading the tag table global itself (215) and
// both of those with the pool global (293) all leave the player in a slot or in a volatile register.
// T90: Inventory count pass through end-tested for/count-plus-cursor/while/outer
// cursor, each mutable/read-only: 608/608/215/215/608/608/608/608.
// End-tested cursors recover frame 0x18 but roll the count pass to 675/1006 bytes; counted cursors tie 215.
// NOT kept: No native pool/player/tag ownership equality; original inventory index and complete body restored.
// M3 lane 12 r118 (fast, 0xcbd50 still matched): `count` declared first 216; the held/pending arrays declared before the player
// 215 (same code); the flag helper plain inline on this body 215; a `short` slot counter 624 (659 B, the slot loop no longer
// unrolled); the last loop's `hand == 1` arm indexing `held[hand]`/`modes[hand]` 213 (1007 B, gain under the gate). On that 213
// form: the count test through the existing 0x101640 getter 211 (1007 B), the held test through the existing 0x101240 getter 213,
// both 211; on the 211 form: the flag helper reading the tag table global itself 211 (same code), `hand != 0` for the arm 221,
// `count` declared after the unit lookup 211. Best 211 is a gain of 4, under the gate; body restored.
// P437: Held/pending/inventory/player lexical identities 215 each; immutable player/pool,
// read-only inventory and fixed shared flag result 215 each. NOT kept: frame 0x1c versus 0x18
// and native pool/player/tag homes unchanged; original complete body and helpers restored.
// P440: First-loop do/while with the existing slot provider, and held/pending/paired/output/held-output/all walks: 215/215/330/329/313/215/330/313.
// NOT kept: Native player/pool/tag homes remain; direct entry ties and genuine producer cursors regress. Original complete body restored.
// P456: standard one-element zero clears across initial/attached/final output phases, all seven
// complete phase families 215 each; one complete attached four-Boolean clear 309 at 988/1006 bytes.
// NOT kept: no gain; original Boolean stores, all slot providers and complete body restored. The tag
// table still hoists, player/pool homes remain nonnative and frame stays 0x1c versus 0x18.
// P458: Retained slot-provider count char/short/word/dword, entry then count-phase lifetime:
// 293/216/216/215/292/274/274/273. NOT kept: late count restores frame 0x18 but regresses length; player/pool/tag homes remain; original complete body restored.
// P460: Complete count-admission/retained-pair outcomes in bool/byte/short/long on the retained slot-provider graph: eight 215. NOT kept: extra frame word and player/pool/tag homes unchanged; complete body restored.
// @retail 0x1509e0
void function_1509e0(long player_index, long weapon_index, bool *modes)
{
	s_tail_player *player = tail_player_get(player_index);
	s_record_pool *pool = g_4e0300;
	s_player_pickup_unit *unit = (s_player_pickup_unit *)tail_object_get(pool, player->unit_index);
	long held[2];
	long pending[2];
	long count = 0;
	for (long hand = 0; hand < 2; hand++)
	{
		held[hand] = function_cbd50(player->unit_index, ((s_player_pickup_unit *)tail_object_get(pool, player->unit_index))->held_slots[hand]);
		pending[hand] = function_cbd50(player->unit_index, ((s_player_pickup_unit *)tail_object_get(pool, player->unit_index))->pending_slots[hand]);
		if (held[hand] != NONE && (bool)((player_weapon_flags(g_4e3b44, held[hand]) >> 3) & 1))
		{
			modes[hand] = false;
			modes[hand + 2] = false;
		}
	}
	if (unit->parent_index != NONE)
	{
		for (long hand = 0; hand < 2; hand++)
		{
			modes[hand] = false;
			modes[hand + 2] = false;
		}
	}
	for (long slot = 0; slot < 4; slot++)
	{
		long held_weapon = unit->weapons[slot];
		if (held_weapon != NONE && !(bool)((player_weapon_flags(g_4e3b44, held_weapon) >> 4) & 1))
		{
			dword flags = player_weapon_flags(g_4e3b44, weapon_index);
			if (!((bool)((flags >> 22) & 1) || (bool)((flags >> 23) & 1)) || held_weapon != held[1] || held_weapon != pending[1])
				count++;
		}
	}
	if (count >= 2)
	{
		for (long hand = 0; hand < 2; hand++)
		{
			if (hand == 1)
			{
				if (held[1] != NONE && !(bool)((player_weapon_flags(g_4e3b44, held[1]) >> 4) & 1))
					modes[1] = false;
			}
			else if (!(bool)((player_weapon_flags(g_4e3b44, weapon_index) >> 4) & 1))
			{
				modes[hand] = false;
				if (held[hand] != NONE && (bool)((player_weapon_flags(g_4e3b44, held[hand]) >> 4) & 1))
					modes[hand + 2] = false;
			}
		}
	}
}

#include "unit_requests.h"

/* asks a unit to pick up a weapon (request 0x14) */
static __forceinline bool unit_pickup_request(long unit_index, long weapon_index, short mode)
{
	s_unit_request request;
	memset(&request, 0, sizeof(request));
	request.type = 0x14;
	*(long *)request.arguments = weapon_index;
	*(short *)(request.arguments + 4) = mode;
	return function_e6900(unit_index, &request);
}

void function_a90e0(long first, long second);
void function_a9030(long index, short slot);
void function_a8f80(long index, short slot);
bool __stdcall function_cd4e0(long unit_index, short hand, bool flag);
struct s_16e150_bits;
void function_16de20(short index, s_16e150_bits const *table, dword *output);

// @retail 0x151c10
bool function_151c10(long player_index, s_target_candidate *candidate)
{
	s_tail_player *player = tail_player_get(player_index);
	bool result = false;
	if (candidate->priority == 8 && function_1518f0(candidate, 2))
	{
		if (g_4e6948->mode == 4)
			function_a90e0(player->unit_index, candidate->object_index);
		else
		{
			s_unit_request request;
			request.type = 0x21;
			*(long *)request.arguments = player->unit_index;
			function_e6900(candidate->object_index, &request);
		}
		result = true;
	}
	return result;
}

// score 17 (was 17):
// MiniMax C (11-06): no new form, no lower score; the unit index still lands in ESI where retail keeps it in EDI and the object table pointer lands in EDI where retail keeps ESI.
// MiniMax D (11-06): no new form, no lower score (17): the unit index still lands in esi where retail keeps it in
// edi and the object table pointer lands in edi where retail keeps esi; this round a read-only player, a read-only
// pickup unit, a byte, bool or short hand, the hand before the unit index test, the unit index in a local, the two
// slot tests nested, the else branch as an else-if, the mode comparison reversed, the mode in a local and the
// globals in a local all give 17, and the hand as a byte, bool or short and the unit index local give 44, 44, 38
// and 46.
// MiniMax B (11-06): no new form, no lower score; the whole body agrees with retail except the two
// callee-saved registers: retail keeps the unit index in EDI and the object table in ESI, this build
// keeps the index in ESI and the table in EDI, and the held-slot byte moves with them.
// MiniMax A (11-04, run 45): no new form, no lower score (17); the unit index still swaps registers with the object table.
// MiniMax H (11-04): no new form, no lower score; the unit index and the object table still swap registers.
// MiniMax E (11-05): no new form, no lower score; the unit index still lands in esi where retail keeps it in
// edi, and the object table pointer lands in edi where retail keeps esi.
// Logic and structure match; only the callee-saved registers differ: retail keeps player->unit_index in
// edi and the object data pointer in esi, this build uses esi and edi. Tried (scores): unit_index in a local
// (45), `hand` before the unit_index test (27), unit before hand (20), the held-weapon test through an
// inline helper (17), `short` hand (38), `long` slot (38), `!primary` inline in the index (44).
// Permuter (25 min on r654): best 12, only with a volatile read of the unit index; not kept.
// F352: Complete rejection returns / sole-exit rejection / exhaustive outcomes / retained-false
// dispatch outcomes, saved player receiver: 50/35/48/78; existing lookup at every player
// field: 46/46/46/71. All restored; saved unit/table EDI/ESI still reversed.
// M337: Returned availability owns unit and held-slot admission; bool/byte/short/long
// states with the original short slot [48, 68, 85, 80]; native signed-byte slot [46, 66, 81, 76].
// All restored; the saved unit-index and table/slot registers remain exchanged.
// Round 51 (m3 lane 15): mode arms swapped 27; `primary ? 0 : 1` hand 17; result declared before the player 35;
// e68c0 code from hand 60; unit index local inside the dispatch block 17; dispatch outcome in its own success flag
// copied to result (0x152bf0's matching form) 17. Diagnostic: 1-8 unused entry locals 17 each (not symbol numbering).
// Round 104: unit declared at entry 17; slot declared at entry 17; unit then result before player 35;
// read-only player/unit receivers 17 each; nested slot/weapon tests 17; held-slot array pointer 17;
// weapons array pointer 17. NOT kept: no byte match or gain; original body restored, EDI/ESI still exchanged.
// Round 359: byte result 44; signed-byte result 44; short result 60; long result 57; dispatch
// outcome accumulated 17; dispatch test sets success 67; weapon snapshot in valid-slot scope 17;
// unavailability result 67. NOT kept: no qualifying byte match or gain; original body restored.
// Round 78 (m3 lane 11, fast): a pool copy `s_record_pool *objects = g_4e0300;` declared first for the unit lookup 17; the
// player read at each use (no player local) 46 (the player index moves to EDX); the unit lookup written in place at both reads
// 17; the slot read in place through the pool copy and the weapon read through the global 17; the player at each use plus the
// pool copy 46.
// Round 172 (m3 lane 5, fast): the slot-to-weapon test through the existing 0xcbd50
// (`function_cbd50(player->unit_index, <fresh unit>->held_slots[hand]) != NONE`) 17 (identical code), with the `unit`
// local or the result in a named weapon local 17 (identical); `result = true` before the a9030 call 59 (146 bytes);
// `dword` hand 17 (identical); `unsigned short` hand 38 (139 bytes, movzx where the short hand has movsx); the held
// slot read at each use (no slot local) 42 (137 bytes). Diagnostics, reverted: with no unit-index use in either arm
// the table/slot web takes ESI (40), so in our build the unit index outranks it for ESI; one use removed from either
// arm 30/31, an extra slot use 45, an extra unit-index use 42, integer no-op tuples at the player lookup (`* 1`, `+
// 0`, `^ 0`, identity inline, identical code), volatile global reads at entry / after the player lookup 57/54 and the
// player index kept to the end 60 never move the pair, so reference counts do not decide it; the integer no-op probes
// fold before allocation, so tuple phase is untested (only one-step volatile probes, which did not flip it).
// Round 153 (m3 lane 7, fast): `primary == false` hand 17 (same code); the weapon read into a local by a ternary on the
// slot 17 (same code); the slot declared at function scope 17 (same code); `(short)hand` at the 0xa9030 call 17 (same
// code); the hand declared ahead of the result 48 (140 bytes). Re-measured: the unit index in a local declared after
// the result 46 (the player index then arrives in EDX), the unit index local inside the dispatch block 17, the mode
// arms swapped 27. Sibling 0x151aa0's retail passes its hand to 0xa8f80 in EBX, so EBX for the hand here is the
// callee's parameter register; only the ESI/EDI pair (unit index against table and slot) is open. A unit index local
// declared inside the tested block (first, or after the hand) and used for the lookup and both calls 17 (same code);
// used for the lookup only, the calls reading the field, 17 (same code).
// @retail 0x151b80
bool function_151b80(long player_index, bool primary)
{
	s_tail_player *player = tail_player_get(player_index);
	bool result = false;
	if (player->unit_index != NONE)
	{
		long hand = !primary;
		s_player_pickup_unit *unit = (s_player_pickup_unit *)tail_object_get(g_4e0300, player->unit_index);
		short slot = unit->held_slots[hand];
		if (slot != NONE && unit->weapons[slot] != NONE)
		{
			if (g_4e6948->mode == 4)
			{
				function_a9030(player->unit_index, hand);
				result = true;
			}
			else
				result = function_e68c0(primary ? 9 : 0x13, player->unit_index);
		}
	}
	return result;
}

// @retail 0x150820
void function_150820(dword *bits, bool local_only)
{
    s_match_globals *table = g_4e0348;
    s_record_pool_iterator iterator;
    iterator.data = g_4e8c24;
    iterator.index = NONE;
    s_tail_player *player;
    while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
    {
        if (local_only && player->local_index == NONE) continue;
        short cluster = *(short *)player->unknown2a;
        if (cluster != NONE)
        {
            dword cluster_bits[16];
            function_16de20(cluster, (s_16e150_bits const *)table, cluster_bits);
            for (long word_index = ((table->list_count + 31) >> 5) - 1; word_index >= 0; --word_index)
                bits[word_index] |= cluster_bits[word_index];
        }
    }
}

// MiniMax B (11-13): no new form, no lower score (43): the hand before the result (62), the result before the player (43), the unit pointer before the hand (70), the hand as an int (43) and the hand at the entry (86) all leave the register rotation - retail keeps the player in esi, the unit index in edx and the hand in edi, ours keeps them in edi, ecx and esi.
// score 43 (was 43):
// MiniMax B (11-14): no new form, no lower score (43); the gap is still the one register rotation (retail keeps
// the player in esi, the hand in edi and the unit index in edx, ours keeps them in edi, esi and ecx) and both
// bodies are 210 bytes; a const player pointer, a unit-index local and the lookup moved above the hand all
// leave the code unchanged or worse.
// MiniMax S (11-11): no new form, no lower score (43): every difference is one clean pairwise register rotation (retail: player esi, hand edi, unit index edx, temp ecx; ours: player edi, hand esi, unit index ecx, temp edx).
// MiniMax D (11-08): the saved roles are exchanged - retail keeps the player record in esi
// MiniMax I (11-10): no new form, no lower score (43); every difference is one clean pairwise register
// rotation (retail: player esi, hand edi, unit index edx, temp ecx; ours: player edi, hand esi, unit index
// ecx, temp edx) and the permuter was killed at its time limit without reporting.
// MiniMax D (11-08): score 43 (was 43): the saved roles are exchanged - retail keeps the player record in esi
// and the hand in edi, ours keeps the player in edi and the hand in esi, and the unit index stays in ecx where
// retail reuses esi.
// MiniMax B (11-06): no new form, no lower score; the player pointer and the hand still swap ESI and EDI
// and the hand's comparison result still lands in DL where retail's lands in AL, and the unit index is in
// ECX where retail keeps it in EDX.
// MiniMax C (11-06): no new form, no lower score; the player pointer still lands in EDI where retail keeps it in ESI and the hand in ESI where retail keeps EDI.
// MiniMax G (11-06): no new form, no lower score; four ways of deriving the hand (the negation, a
// conditional, an int and a bool) and two declaration orders (the result before the player) all leave
// this body exactly as it was (43, and 89 for the bool hand): the player pointer and the hand still
// swap esi and edi, and the hand's comparison result still lands in dl where retail's lands in al.
// MiniMax D (11-04): no new form, no lower score; retail keeps the player in ESI and the hand in EDI and this
// build swaps them (the unit index takes ECX instead of EDX), and the permuter was killed before it reported.
// the walk found no difference in logic (same stores, calls, argument order, held_slots/weapons
// offsets, mode == 4 byte test, result kept in bl with its stack copy). Only the callee-saved pair is swapped:
// retail keeps the player in esi and the hand in edi (edi pushed late), ours the reverse, and the unit index and the
// `primary` temporary sit in ecx/edx and edx/al instead of edx and cl/eax. Tried: hand declared before the player
// (43), result declared first (43), `primary ? 0 : 1` / `primary == 0` (43), `primary ^ 1` (75), bool hand (89),
// hand after the unit lookup (70), unit_index cached in a local (43), modes declared at the top (43), the weapon and
// mode tests nested (47).
// Permuter (25 min on r654): best 16, only with a volatile read of the unit index and the held slot read as a
// char (a behaviour change); not kept.
// Round 45 (m3 lane 2, fast, base 43): `else if (function_cd4e0(...)) result = true;` 47; `!primary` at each use with
// no hand local 117 (236 bytes); the player fetched at each use 89 (220 bytes); the unit fetched at each use 43
// (same code); hand declared at function scope 43.
// Round 552: guarded command success/direct command/short-circuit result 47/95/87; all admission
// returns 108, absent-unit return 78, branch-owned result 106, success-default result 88, admission
// result before dispatch 101. All original calls, observations and executable body restored.
// NOT kept: no score gain or byte match.
// N345: Isolated player/hand/unit/held-slot/held-weapon/selector/parameter role names
// and the int hand-negation result: all eight 43. Original body and calls restored;
// ESI/EDI player/hand ownership still differs.
// F357: Canonical shared object/header lookup with mutable or read-only
// player/unit receivers: eight 43; existing player/object inline, forced and
// unhinted expansion policies: eight 43. Every other user keeps its score.
// NOT kept: no gain; original helpers, includes, receivers and complete body restored.
// F353: Native hand arm/default producers, original then actual mode/result storage names
// 43/45/45/43/43/45/45/43.
// NOT kept: no gain; original full body/helpers restored.
// Y559: Complementary final failure bool/byte/short/long, conditional then
// guarded command clearing: 85/85/87/89/73/73/87/89. NOT kept: no byte match;
// native ESI player/EDI hand and temporary entry registers remain.
// N345: Isolated player/hand/unit/held-slot/held-weapon/selector/parameter role names
// and the int hand-negation result: all eight 43. Original body and calls restored;
// ESI/EDI player/hand ownership still differs.
// M348: Hand unsigned-long/word/byte/signed-byte 43/81/89/89; positive result
// byte/signed-byte/short/long 92/92/90/91. NOT kept: no byte match;
// native player/hand homes remain. Original body and call tuples restored.
// M350: Immutable captured hand/slot/weapon values, seven subsets: 43 each;
// initialized four-byte mode output 86. Original body and calls restored;
// native ESI player/EDI hand ownership remains.
// F371: Selected-inventory guarded/default/paired/conditional result boundaries
// 43/43/43/43/43; existing complete slot selector without/with slot admission
// and with closed lookup scope 43/43/43. NOT kept: no gain or byte match;
// native player/hand ownership remains. Original body and APIs restored.
// Y567: One-return failed unit/slot/eligibility admissions, seven subsets
// and separately tested complete eligibility: 60/63/60/63/60/63/60/60. Original body restored;
// native player/hand and entry temporary ownership remain.
// P330: Terminal command success accumulated into bool/byte/short/long result,
// bitwise OR 95/97/95/98; logical OR 87/92/90/92. NOT kept: no match,
// changed entry allocation and shorter extent. Original result graph and full body restored.
// F373: Native query-return admission/failure in bool/byte/short/long,
// at the original call boundary: 43/43/43/43/43/43/43/43. NOT kept:
// no byte match; original body restored, player/hand ownership remains.
// F375: Actual selected-mode byte availability/failure after the query, bool/byte/short/long:
// 43/43/43/43/43/43/43/43. NOT kept: no byte match; original body restored.
// Saved player/hand ESI/EDI and entry temporary allocation still differ.
// N368: Selected slot at every use / actual char / int / entry short: 49/49/72/43;
// direct selected-weapon reads with direct/char/int/short slot: 49/49/72/43.
// NOT kept: ties or regressions; original short slot and weapon capture restored.
// Native ESI player/EDI hand and entry temporaries remain exchanged.
// DH372: Isolated mode-success/command-success/actual-command exits, then both success-exit
// pairs 90/86/110/112/110; sole-return common success in bool/byte/long 63/73/91.
// NOT kept: no byte match or gain; original body restored, saved player/hand roles remain.
// P332: Terminal game-mode equality/inequality producers at the original
// observation, bool/byte/short/long: eight 43 at 210 bytes. NOT kept:
// no byte match; original body and calls restored, player/hand homes remain.
// N374: Hand lifetime after slot/slot admission/weapon/query, selected slot
// indexed then conditional on primary: 70/70/70/70/81/82/104/86. Original selector,
// lifetimes and full body restored; NOT kept: unmatched shorter extents
// and native ESI-player/EDI-hand ownership remains.
// F393: Existing player/object getter masked native ordinal long/dword/int/word:
// 43/43/43/85/43/43/43/64. NOT kept: no byte match; original complete body/helpers restored;
// saved player/hand ESI/EDI and entry temporaries remain exchanged.
// Word object ordinal recovers saved roles but replaces native AND32 with MOVZX; 64 at 206/210 bytes.
// DH372: Isolated mode-success/command-success/actual-command exits, then both success-exit
// pairs 90/86/110/112/110; sole-return common success in bool/byte/long 63/73/91.
// NOT kept: no byte match or gain; original body restored, saved player/hand roles remain.
// Round 73 (m3 lane 11, fast): weapon test folded into the slot test as 0x151b80 has it 43; mode arms swapped 72
// (219 bytes); modes declared before the slot 43; weapon index declared with the hand 43; result set before the
// a8f80 call 71 (215 bytes). A dword load/store probe at entry, after the result, before or after the hand and
// before the unit lookup (87/84/81/79) never moves the player/unit-index homes: the swap is not a scratch rotation.
// Round 96 (m3 lane 12, fast): hand as short 81 (217 bytes), as char 89 (197 bytes); hand computed at entry before the player
// lookup 86 (200 bytes); modes declared ahead of the slot test 43 (same code). Diagnostics: `!primary` in place of the hand at the
// modes test 43 (CSE'd, same code), at the a8f80 call 89.
// Round 78 (m3 lane 11, fast): a pool copy declared first for the unit lookup 43; the slot read in place through the pool copy
// and the weapon read through the global, no unit pointer 43; a unit_index local for the test, the lookup and the 0xcd7b0
// argument with player->unit_index after the call 43; the weapon index as `slot != NONE ? unit->weapons[slot] : NONE` 43; the
// hand declared after the unit lookup 70 (207 bytes).
// Round 160 (m3 lane 6, fast over the file's markers with 0x14f820, 0xcd7b0, 0xa8f80 and 0xcd4e0): every difference is one
// rotation of the scratch registers (retail EDX/ECX/EAX for the unit index, the primary byte and the hand where ours has
// ECX/EAX/EDX) plus the ESI/EDI swap; 0x151b80 shows the same swap the other way round. The definition moved next to
// 0x151b80 43 (0x151b80 17, so not a source-order effect); a unit_index local for the test and the lookup 43, at every
// use 80 (203 bytes); the player as the address of the pool element 43; a 600 s permuter run (49 tries) none below 43;
// the weapon from the existing 0xcbd50 with the slot read from a fresh unit lookup (the form that took 0x1509e0 from 254
// to 215) 43.
// T77: Live unit-index field mutable/read-only references and field pointer, entry/guarded
// lifetimes 73/73/73/73/73; real held-slot/weapon array references and selected-weapon field
// reference 43/43/70. NOT kept: no native saved-role equality; original body restored.
// Round 172 (m3 lane 5, fast): the 0xcd7b0 test nested under `if (weapon_index != NONE)` with modes declared there 43
// (identical code). Re-measured: the weapon from 0xcbd50 with the slot read from the `unit` local 43 (identical to
// round 160's fresh-lookup form), `result = function_cd4e0(...)` 95, the initialized modes 86. Diagnostics, reverted:
// volatile reads at entry / after the player lookup 87/84 move neither the scratch rotation nor ESI/EDI; integer
// no-op probes at the player lookup fold away (identical code), so tuple phase is untested. No untried natural form
// is left that compiles differently: the notes above cover declaration orders, hand/slot/result types, result graphs,
// early exits, existing getters, pool copies, lifetimes and the permuter.
// MiniMax G (11-11): no new form, no lower score (43); the difference is a register rotation only - retail keeps
// the player in esi, the unit index in edx and the hand in edi, ours keeps them in edi, ecx and esi, and both
// bodies are 210 bytes; the hand as a conditional expression, the unit index in a local of its own and the modes
// array declared before the unit all leave the code as it was.
// Round 154 (m3 lane 7, fast; 0x151b80 17 each time unless noted). New: `long hand = 1 - primary;` 68; `result =
// result || function_cd4e0(...)` 87 (198 bytes); `result |= function_cd4e0(...)` 95 (193 bytes). Applied to 0x151aa0
// and 0x151b80 together (re-measures here, all listed for 0x151aa0): `primary ? 0 : 1` 43 (0x151b80 17), the result
// declared before the player 43 (0x151b80 35, 138 bytes), a unit_index local for the test and the lookup 43 (0x151b80
// 17). Diagnostic: one to four unused named locals change nothing (43). 0x151b80 shows the same ESI/EDI exchange the
// other way round (retail unit index in EDI, ours ESI). No other untried natural form found: declaration orders,
// hand/slot/result widths and spellings, result graphs, early exits, existing getters, pool copies, lifetimes, joint
// forms with 0x151b80 and permuter runs are all listed above.
// P437: Fixed player/unit receivers and primary/player inputs, separate mode-consumer and
// three-way admission scopes, and complete receiver/input binding graphs: 43 each. NOT kept:
// no native player/hand or scratch-register recovery; original whole body and call tuples restored.
// Round 178 (m3 lane 5, fast with 0x151b80/0x151940/0x151c10/0x14f820 and the callees; control 43, 0x151b80 17): the
// same two pairs are exchanged in both functions (here player/hand in ESI/EDI and slot/unit index in ECX/EDX; in 0x151b80
// slot/unit index in ESI/EDI), and our link-time inliner expands 0x151b80 into 0x14f820 where retail calls it, so retail's
// source is heavier than ours in a way the final code does not show. New forms, each on both functions: an unused middle
// candidate parameter as the family's other members have (the two declarations and two call lines in src/unknown_14b560.cpp
// changed for the trial and put back) 43 / 17 (same code); the unit as a byte pointer with the offsets the matched callees
// 0xa8f80/0xa9030 use 43 / 17 (same code); the held weapon through the existing 0xcbd50 with the slot read through a fresh
// lookup and no unit or slot local, watching 0x14f820 for the expansion, 43 / 17 (same code, 0x14f820 unchanged at 701, 2740
// bytes). Here only: every local declared at the function top and assigned in place, in six orders (player/result/hand/
// unit/slot/weapon/modes, hand first, hand last, unit and slot first, result first, reversed) 43 each (same code).
// Diagnostic, reverted: the unit index read through a dword lvalue at the three call arguments 43 / 17 (same code).
// Flags diagnostic, reverted (the one compiler-setting trial): this file built with /Ob1 leaves both functions as they are
// (43 / 17) and stops the 0x151b80 expansion in 0x14f820 (2636 bytes there against 2740, 775), so the exchange is not an
// effect of that expansion.
// P458: Initial unit/selected-slot presence outcomes, bool/byte/short/long:
// 43/43/43/43/43/43/43/43. NOT kept: native saved player/hand and entry scratch roles not recovered; original complete body restored.
// @retail 0x151aa0
bool function_151aa0(long player_index, bool primary)
{
	s_tail_player *player = tail_player_get(player_index);
	bool result = false;
	if (player->unit_index != NONE)
	{
		long hand = !primary;
		s_player_pickup_unit *unit = (s_player_pickup_unit *)tail_object_get(g_4e0300, player->unit_index);
		short slot = unit->held_slots[hand];
		if (slot != NONE)
		{
			long weapon_index = unit->weapons[slot];
			bool modes[4];
			if (weapon_index != NONE && function_cd7b0(player->unit_index, weapon_index, modes) && modes[hand])
			{
				if (g_4e6948->mode == 4)
				{
					function_a8f80(player->unit_index, hand);
					result = true;
				}
				else
					result = function_cd4e0(player->unit_index, hand, false) ? true : result;
			}
		}
	}
	return result;
}

void function_a8f10(long index, long target, short value);
bool function_138880(void);

// @retail 0x151940
bool function_151940(long player_index, s_target_candidate *candidate, bool primary)
{
	s_tail_player *player = tail_player_get(player_index);
	bool result = false;
	if ((candidate->priority == 1 || candidate->priority == 7) && player_candidate_has_type(candidate, 4))
	{
		long weapon_index = candidate->object_index;
		s_player_pickup_object *weapon = (s_player_pickup_object *)tail_object_get(g_4e0300, weapon_index);
		bool modes[4];
		if (*(long *)((byte *)weapon + 0x154) == NONE && function_cd7b0(player->unit_index, weapon_index, modes))
		{
			short mode;
			if (primary)
			{
				if (modes[0]) mode = 3;
				else if (modes[2]) mode = 4;
				else goto done;
			}
			else
			{
				if (modes[1]) mode = 5;
				else if (modes[3]) mode = 6;
				else goto done;
			}
#pragma inline_depth(0)
			if (function_138880())
#pragma inline_depth()
			{
				function_a8f10(player->unit_index, weapon_index, mode);
				result = true;
			}
			else if (unit_pickup_request(player->unit_index, weapon_index, mode))
				result = true;
		}
	}
 done:
	return result;
}

void function_a9180(long first, long second);
void function_a9340(long first, long second, short seat);
bool __stdcall function_c92c0(long unit_index, long vehicle_index, short seat_index, long *blocker_index, bool *grouped);
void function_107840(long device_index, long unit_index);
bool function_1c9500(long unit_index, long actor_index, bool notify);
void function_1e8f60(long player_index);

// @retail 0x151c90
bool function_151c90(long player_index, s_target_candidate *candidate)
{
	s_tail_player *player = tail_player_get(player_index);
	bool result = false;
	switch (candidate->priority)
	{
	case 3:
		if (function_1518f0(candidate, 0x380))
		{
			if (g_4e6948->mode == 4)
				function_a9180(player->unit_index, candidate->object_index);
			else
				function_107840(candidate->object_index, player->unit_index);
			function_1e8f60(player_index);
			result = true;
		}
		break;
	case 4:
	case 5:
	case 6:
		if (function_1518f0(candidate, 3))
		{
			short seat = *(short *)&candidate->flags;
			if (seat >= 0)
			{
				long target = candidate->object_index;
				long definition = *(long *)tail_object_get(g_4e0300, target);
				byte *tag = g_4e3b44[definition & 0xffff].bytes;
				if (seat < *(long *)(tag + 0x1c8))
				{
					long blocker = NONE;
					bool grouped;
					if (function_c92c0(player->unit_index, target, seat, &blocker, &grouped))
					{
						if (function_138880())
							function_a9340(player->unit_index, candidate->object_index, seat);
						else
						{
							s_unit_request request;
							request.type = 0x1c;
							request.type1c.object_index = candidate->object_index;
							request.type1c.seat_index = seat;
							request.type1c.unknowna = false;
							request.type1c.unknownb = false;
							function_e6900(player->unit_index, &request);
						}
						result = true;
					}
					else if (blocker != NONE)
					{
						byte *unit = (byte *)tail_object_get(g_4e0300, blocker);
						if (!function_138880() && *(long *)(unit + 0x12c) != NONE)
							function_1c9500(player->unit_index, *(long *)(unit + 0x12c), true);
						result = true;
					}
				}
			}
		}
		break;
	case 2:
		if (g_4e6948->mode != 4)
		{
			s_unit_request request;
			request.type = 0x15;
			*(long *)request.arguments = player_index;
			result = function_e6900(candidate->object_index, &request);
		}
		break;
	}
	return result;
}

struct s_player_rebind_option
{
    bool active;
    byte unknown01;
    short local_index;
    long controller_index;
    byte unknown08[6];
    byte identifier[12];
    byte unknown1a[0xe4 - 0x1a];
};
struct s_16658d_group;
extern s_16658d_group *g_4e9bc8;
void __stdcall function_167e86(long user_index, long hand);

void __stdcall player_set_local_user(long player_index, long user_index);
void __stdcall function_14f270(long player_index, long controller_index);

// MiniMax H (11-06, run 40): matches retail byte for byte. The binding pass keeps the player pointer in its own
// local instead of the traversal struct's member; that is what makes retail's order come out (the local_index
// test `cmp word ptr [esi+0x28], -1` before the address copy, and both player_set_local_user arguments pushed
// before the machine_user store).
// score 12 (was 12): retail's length (1992 bytes). The unbind arm calls player_set_local_user(index, NONE) and
// function_14f270(index, NONE), which LTCG expands as retail does, and 0x1682bf (inline on its definition) now
// expands inside it. Left: in the binding pass retail tests local_index (`cmp word ptr [esi+0x28], -1`) before
// the address copy and pushes both player_set_local_user arguments before the machine_user store; ours tests
// after the copy and stores between the pushes.
// Earlier 285 (was 587): frame 0xe4, 78 bytes short; mask/index homes, release expansion and late binding homes remain.
// The existing controller iterator restores the native memory cursor and pool reload (plain 285).
// Earlier 587 (was 628): the unbind in the second-to-last loop follows retail's expansion of 0x14f190/0x14f270's
// NONE arms (through freshly fetched player pointers), the machine fields come from s_player_census_counts, the
// iterator sits in a struct after a pointer (retail's frame 0xe4: a free slot at +0x24), and the arrays are filled
// by loops (the preamble matches retail instruction for instruction). Retail also expands 0x1682bf(NONE, user,
// NONE) here (0x1535b8-0x1535f2); ours calls it, as unknown_1682bf.cpp builds with /Ob1. Measured trial, not kept
// (a written-out copy stays only with a byte match): 0x1682bf's NONE arm written out here as a forceinline
// helper gives 497, so that inlined shape is the right one. Still differs: retail keeps iterator.index in memory
// in the second (controller/user choice) loop, ours keeps it in a register (retail +0x177: 8 more
// instructions there, so everything after is shifted); retail compares with immediate -1, ours sometimes keeps
// -1 in a register; the rebind arm passes `user` (short) from a register.
// Tried: iterator beside the free slot as separate locals (545), explicit `= {NONE...}` controllers (556),
// the three masks in all six declaration orders (543-559), `index` at function scope (545), the loop
// over two weapons as `for` with an index (555 unrolled) or as a helper (510, unrolled).
// N341: retained local-index short, controller/user loop admission and failure guards 587 each;
// real datum-iterator API 934; absolute cursor subscripts 586; subscript name 587.
// NOT kept: one-point cursor reduction; native mask/index homes and missing inline expansion remain.
// N343: Six semantic mask-home naming forms and independent traversal/pass home names 587 each.
// Original names and complete body restored; native mask/index homes, memory cursor and
// missing inline expansion remain. NOT kept: unmatched storage-only renaming.
// P295: Existing option/user/controller vector setters 618/543/663/576;
// complete test/set families 629/536/644/435. NOT kept: scalar masks become
// indexed word addresses with sar 5, absent in retail; original mask code restored.
// DH323: Actual controller/user/both output-array search accumulators, decoded
// then absolute ordinal 587/587/578/588/587/443; controller/user lvalues 587/587.
// NOT kept: native searches retain EAX/EBP results before final array stores;
// array workspaces shift mask/iterator homes despite 443 at 1898/1992 bytes.
// M344: Complete default-owned acceptance/rejection pass outcomes in bool/byte/short/long
// storage 668 each (1850/1992 bytes). NOT kept: outcome flags do not match and shorten
// the body; original switch, masks, traversal state, helpers and complete body restored.
// Y563: Eight existing pass-switch case orders, including 2/1/default/0: 587 each.
// Original Boolean result and mask/index homes persist; complete source restored.
// NOT kept: no gain or byte match; native memory cursor and inline expansion remain.
// N353: Existing controller iterator/user release/controller release subsets 1-7
// 285/587/285/587/285/587/285; complete release graph with a bounded controller loop 604.
// Keep only the existing controller iterator (plain 285, 1914/1992 bytes); release calls and bounded loop restored.
// DH340: Matching-pass short/word/int/dword carriers 618/618/285/286; controller-pass
// counterparts 297/299/285/288. NOT kept: no gain or byte match; native full-width
// pass declarations, retained controller iterator and complete body restored.
// DH349: Decoded ordinal short in matching/fallback/release/binding 451/591/359/280;
// all short 585; immutable full-width ordinals 285; option-mask last-use scope 288;
// shared full-width ordinal 285. NOT kept: five-point 280 below gate; native mask/index
// homes and release expansion remain. Original per-traversal full-width ordinals restored.
// DH351: Assignment workspace ends after fallback; nested option/mask endings
// 285/288/286/288; late user result on those scopes and separate search names
// 605/608/608/608. NOT kept: no gain; native mask/index homes and release expansion remain.
// DH353: Inner option ordinal int/dword/short/word 285/286/642/642;
// per-player declaration 285; ordinal/entry role names 285/285; complete indexed
// option view 596. NOT kept: no gain; original scalar masks and entry view restored.
// DH355: Decoded ordinal declared before matching/pass/fallback/release/binding
// traversal, then phase pairs/all four: 285/285/285/285/285/285/285/285. NOT kept: all tie;
// original scopes/body restored; native mask/index homes and release expansion remain.
// DH357: First matching-pass repeated option-user/controller/player-user captures,
// seven subsets then admitted-option user lifetime: 285/668/669/674/260/628/612/636.
// NOT kept: 260 combines tying/regressing parts, hoists separate pass tests and
// shortens away from retail; every field observation and full body restored.
// N384: Actual controller/user/option array role names, seven subsets and alternate complete
// names: 285 each. Original names and full body restored; mask/index homes and missing
// release expansion remain. NOT kept: no byte match or gain.
// AC536: Actual option/user/controller zero-mask initialization after array filling,
// seven subsets and entry declarations with chained late zero: 288/287/288/287/288/287/338/338.
// NOT kept: no equality or qualifying supported gain; original full body restored.
// B434: Failure-first release admission, enclosed binding/matching lifetimes and
// all seven subsets, then enclosed active-option acceptance: eight 285. NOT kept:
// no gain; native mask/index homes and release expansion remain; full body restored.
// DH359: Full-width signed option/user/controller masks, seven subsets 285 each;
// word option mask 297; existing release-helper inline/forced declarations 285/285.
// NOT kept: no gain; scalar homes and release expansion remain. All bodies and declarations restored.
// AC548: Option/user/controller mask lexical homes, subsets 1-7, then player ordinal on the retained controller iterator
// 285/285/285/285/285/285/285/285. NOT kept: no qualifying supported gain or byte match.
// Native mask/index homes, release expansion and late binding homes remain. Original complete body restored.
// N384: Actual controller/user/option array role names, seven subsets and alternate complete
// names: 285 each. Original names and full body restored; mask/index homes and missing
// release expansion remain. NOT kept: no byte match or gain.
// Round 73 (m3 lane 11, fast): unbind through the two existing calls 285 (same code as the hand-written arms, kept);
// inline on 0x1682bf 12 (kept). On 12: the bind call passing the stored machine_user field, with/without the user
// local 37/37; the local_index test taken into a bool before the copy 57; the machine_index store in both arms of
// that test 56; machine_controller stored before machine_user 13; the user local read before the copy 27; a 600 s
// permuter run (44 tries) none below 12. Renaming the per-player index local never moved its frame slot.
// Round 69 (m3 lane 14, fast): copy and machine_index in both arms of the local_index test 82 (2036 bytes, no head
// hoisting), bound arm first 105 (2035 bytes); no user local 12 (same code); the bind body under `users[index] != NONE`
// instead of `continue` 13. Diagnostics: the copy as a byte-array aggregate assignment 12 (same code), as dword+word 55;
// the test in a bool before the copy 57 (the cmp moves ahead of the copy as in retail, but `sete`/`test` remain).
// @retail 0x152f80
void __stdcall function_152f80(void *address, void *option_data)
{
    byte const *machine_address = (byte const *)address;
    s_player_rebind_option const *options = (s_player_rebind_option const *)option_data;
    long controllers[16];
    long users[16];
    long assigned[16];
    dword options_used = 0;
    dword users_used = 0;
    dword controllers_used = 0;
    for (long i = 0; i < 16; i++)
        controllers[i] = NONE;
    for (long i = 0; i < 16; i++)
    {
        assigned[i] = NONE;
        users[i] = NONE;
    }
    struct
    {
        s_tail_player *player;
        s_record_pool_iterator iterator;
    } state;
    state.iterator.data = g_4e8c24;
    for (long pass = 0; pass < 4; pass++)
    {
        state.iterator.index = NONE;
        while ((state.player = (s_tail_player *)data_iterator_next_inlined(&state.iterator)) != 0)
        {
            long index = (short)state.iterator.datum_index;
            if (assigned[index] != NONE) continue;
            for (long option = 0; option < 16; option++)
            {
                s_player_rebind_option const *entry = &options[option];
                if (!entry->active || (options_used & (1 << option))) continue;
                bool match;
                switch (pass)
                {
                case 0:
                    match = state.player->local_index != NONE && !memcmp(entry->identifier, state.player->unknown04, 12);
                    break;
                case 1:
                    match = state.player->local_index != NONE && entry->controller_index == state.player->controller_index;
                    break;
                case 2:
                    match = state.player->local_index != NONE;
                    break;
                default:
                    match = true;
                    break;
                }
                if (!match) continue;
                users[index] = entry->local_index;
                assigned[index] = option;
                controllers[index] = entry->controller_index;
                options_used |= 1 << option;
                users_used |= 1 << entry->local_index;
                controllers_used |= 1 << entry->controller_index;
                break;
            }
        }
    }
    state.iterator.index = NONE;
    while ((state.player = (s_tail_player *)data_iterator_next_inlined(&state.iterator)) != 0)
    {
        long index = (short)state.iterator.datum_index;
        if (assigned[index] != NONE || state.player->local_index == NONE) continue;
        long controller = NONE;
        long user = NONE;
        for (long pass = 0; pass < 3; pass++)
        {
            if (controller != NONE) break;
            for (long candidate = 0; candidate != NONE; candidate = controller_index_next(candidate))
            {
                if (!(controllers_used & (1 << candidate)) &&
                    (pass > 0 || candidate == state.player->controller_index) &&
                    (pass > 1 || g_4e61cc[(short)candidate]))
                {
                    controller = candidate;
                    break;
                }
            }
        }
        for (long pass = 0; pass < 2; pass++)
        {
            if (user != NONE) break;
            for (long candidate = 0; candidate < 4; candidate++)
            {
                if (!(users_used & (1 << candidate)) && (pass > 0 || candidate == state.player->local_index))
                {
                    user = candidate;
                    break;
                }
            }
        }
        controllers[index] = controller;
        users[index] = user;
        users_used |= 1 << user;
        controllers_used |= 1 << controller;
    }
    s_player_census_counts *globals = (s_player_census_counts *)g_4e8c20;
    if (globals->local_machine_index != NONE)
    {
        state.iterator.index = NONE;
        while ((state.player = (s_tail_player *)data_iterator_next_inlined(&state.iterator)) != 0)
            if (state.player->local_index != NONE) memcpy(state.player->machine_address, machine_address, 6);
        memcpy(globals->local_machine_address, machine_address, 6);
        memcpy(globals->machine_addresses[globals->local_machine_index], machine_address, 6);
    }
    else
    {
        globals->local_machine_valid = true;
        memcpy(globals->local_machine_address, machine_address, 6);
        globals->local_machine_index = 0;
        globals->machine_valid_mask |= 1;
        memcpy(globals->machine_addresses[0], machine_address, 6);
    }
    state.iterator.data = g_4e8c24;
    state.iterator.index = NONE;
    while ((state.player = (s_tail_player *)data_iterator_next_inlined(&state.iterator)) != 0)
    {
        long index = (short)state.iterator.datum_index;
        if (state.player->local_index != NONE && (state.player->local_index != users[index] || state.player->controller_index != controllers[index]))
        {
            player_set_local_user(state.iterator.datum_index, NONE);
            function_14f270(state.iterator.datum_index, NONE);
            state.player->machine_user = NONE;
            state.player->machine_controller = NONE;
        }
    }
    state.iterator.data = g_4e8c24;
    state.iterator.index = NONE;
    s_tail_player *player;
    while ((player = (s_tail_player *)data_iterator_next_inlined(&state.iterator)) != 0)
    {
        long index = (short)state.iterator.datum_index;
        if (users[index] == NONE) continue;
        memcpy(player->machine_address, machine_address, 6);
        player->machine_index = (short)((s_player_census_counts *)g_4e8c20)->local_machine_index;
        if (player->local_index == NONE)
        {
            short user = (short)users[index];
            player->machine_user = user;
            player->machine_controller = controllers[index];
            player_set_local_user(state.iterator.datum_index, user);
            function_14f270(state.iterator.datum_index, player->machine_controller);
        }
        if (TEST_FIELD_BIT(g_54e8e0[player->controller_index].flag4)) *(long *)g_54e8e0[player->controller_index].unknown004 = player->local_index;
    }
}

#include "data_array.h"
void __stdcall function_14be90(long player_index, dword const *configuration);

struct s_player_configuration_copy
{
    dword words[0x24];
};

// @retail 0x1523c0
void function_1523c0()
{
    s_record_pool *players = g_4e8c24;
    long index = NONE;
    for (;;)
    {
        long next = data_find_index(players, index + 1);
        if (next == NONE)
            break;
        byte *player = players->data + players->size * next;
        long datum = ((long)*(short const volatile *)player << 16) | next;
        s_player_configuration_copy configuration;
        index = next;
        memcpy(&configuration, player + 0xd4, sizeof(configuration));
        function_14be90(datum, configuration.words);
    }
}


struct s_tag_iterator
{
    long unknown00, unknown04, datum_index, next_index, group_tag;
};
struct s_loadout_state_152a50 { short kind, rounds, total, depleted; };
struct s_effect_owner;
long function_122c70(s_tag_iterator *iterator);
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner);
long __stdcall function_b7b40(void *creation);

// @retail 0x152a50
long function_152a50(s_loadout_state_152a50 const *state, long owner_index, bool *failed)
{
    long const *owner_reference = &owner_index;
    bool *const *failed_reference = &failed;
    long result = NONE;
    if (state->kind != 0)
    {
        s_tag_iterator iterator;
        iterator.next_index = 0;
        iterator.group_tag = 0x77656170;
        long tag;
        while ((tag = function_122c70(&iterator)) != NONE)
        {
            byte *definition = g_4e3b44[tag & 0xffff].bytes;
            if (*(long *)(definition + 0x2d0) > 0)
            {
                byte *seat = *(byte **)(definition + 0x2d4);
                long ammunition = *(long *)(seat + 0x90);
                if (ammunition != NONE)
                {
                    if (state->kind == (short)g_4e3b44[ammunition & 0xffff].bytes[0x128]) break;
                }
                else if (*(long *)(seat + 0x98) != NONE && state->kind == (short)seat[0x88]) break;
            }
            if (state->kind == (short)definition[0x1fc]) break;
        }
        if (tag != NONE)
        {
            byte creation[0xc4];
            function_b7930(creation, tag, *owner_reference, NULL);
            result = function_b7b40(creation);
            if (result != NONE)
            {
                byte *object = (byte *)tail_object_get(g_4e0300, result);
                byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
                if (*(long *)(definition + 0x2c0) > 0)
                {
                    *(short *)(object + 0x22c) = state->total;
                    *(short *)(object + 0x22a) = state->rounds;
                }
                *(real *)(object + 0x184) = 1.0f - (real)state->depleted * 0.01f;
            }
            else **failed_reference = true;
        }
        else if (state->kind != 0) **failed_reference = true;
    }
    return result;
}



#include <math.h>
byte function_150380(long player_index);
void function_e64e0(vector3f const *reference, vector3f const *direction, vector3f const *fallback,
    bool three_dimensional, vector3f *result, short *axis_index);

struct s_xy_150400 { real i; real j; };

// @retail 0x150400
void function_150400(long player_index, vector3f const *forward, vector3f const *movement,
    vector3f *new_forward, vector3f *new_movement)
{
    vector3f const *const *local_41f0cc_2 = &forward;
    vector3f *const *new_forward_reference = &new_forward;
    vector3f *const *new_movement_reference = &new_movement;
    s_tail_player *player = tail_player_get(player_index);
    vector3f facing = *(*local_41f0cc_2);
    s_xy_150400 xy = *(s_xy_150400 const *)movement;
    if (function_150380(player_index))
    {
        byte *unit = (byte *)tail_object_get(g_4e0300, player->unit_index);
        s_horizontal_axes axes;
        function_1502e0(*local_41f0cc_2, (vector3f const *)(unit + 0x70), &axes);
        vector3f const *axis_forward = &axes.forward;
        vector3f const *axis_left = &axes.left;
        real move_i = movement->i;
        vector3f world;
        world.i = axis_forward->i * move_i;
        world.j = axis_forward->j * move_i;
        world.k = axis_forward->k * move_i;
        real move_j = movement->j;
        world.i += axis_left->i * move_j;
        world.j += axis_left->j * move_j;
        world.k += axis_left->k * move_j;
        function_e64e0(&world, *local_41f0cc_2, (vector3f const *)(unit + 0x70), false, &facing, NULL);
        function_1502e0(&facing, (vector3f const *)(unit + 0x70), &axes);
        xy.i = axes.forward.j * world.j + axes.forward.k * world.k + axes.forward.i * world.i;
        xy.j = axes.left.k * world.k + axes.left.j * world.j + axes.left.i * world.i;
        real square = xy.j * xy.j + xy.i * xy.i;
        if (square > 1.0f)
        {
            real inverse = 1.0f / (real)sqrt(square);
            xy.i *= inverse;
            xy.j *= inverse;
        }
    }
    **new_forward_reference = facing;
    (*new_movement_reference)->j = xy.j;
    (*new_movement_reference)->i = xy.i;
    (*new_movement_reference)->k = 0.0f;
}



void __stdcall function_ccff0(long unit_index);
bool __stdcall function_cd0c0(long unit_index, long weapon_index, short mode);

// @retail 0x152bf0
bool function_152bf0(byte const *loadout, long unit_index)
{
    bool result = false;
    if (loadout[0])
    {
        byte *unit = (byte *)tail_object_get(g_4e0300, unit_index);
        function_ccff0(unit_index);
        long primary, special, secondary;
        bool success;
        {
            bool failed = false;
            primary = function_152a50((s_loadout_state_152a50 *)(loadout + 2), unit_index, &failed);
            special = function_152a50((s_loadout_state_152a50 *)(loadout + 0x12), unit_index, &failed);
            secondary = function_152a50((s_loadout_state_152a50 *)(loadout + 0xa), unit_index, &failed);
            success = !failed;
        }
        for (long i = 0; i < 2; ++i)
        {
            unit[0x23e + i] = 0;
            unit[0x23e + i] = loadout[0x1a + i];
        }
        if (primary != NONE && !function_cd0c0(unit_index, primary, 1))
        { function_b8540(primary); success = false; }
        if (secondary != NONE && !function_cd0c0(unit_index, secondary, 1))
        { function_b8540(secondary); success = false; }
        if (special != NONE && !function_cd0c0(unit_index, special, 7))
        { function_b8540(special); return false; }
        result = success;
    }
    return result;
}

bool function_cb810(long unit_index, vector3f *vector);
// @retail 0x1505f0
void function_1505f0(long player_index, long unit_index, byte const *input)
{
    (void)&unit_index; (void)&input;
    s_tail_player *player = tail_player_get(player_index);
    long unit = player->unit_index;
    long current_unit = player->unit_index;
    real cos_second_angle = (real)cos(*(real const *)(input + 8));
    vector3f facing;
    facing.i = (real)(cos(*(real const *)(input + 4)) * cos_second_angle);
    facing.j = (real)(sin(*(real const *)(input + 4)) * cos_second_angle);
    facing.k = (real)sin(*(real const *)(input + 8));
    if (current_unit != NONE) function_cb810(unit, &facing);
    vector3f forward_copy, projected_movement;
    function_150400(player_index, &facing, (vector3f const *)(input + 0xc), &forward_copy, &projected_movement);
    byte control[0x7c];
    function_c6ef0((s_unit_state_c6ef0 *)control);
    *(dword *)(control + 0x10) = *(dword const *)input;
    *(vector3f *)(control + 0x34) = facing;
    *(vector3f *)(control + 0x40) = facing;
    *(vector3f *)(control + 0x28) = forward_copy;
    *(vector3f *)(control + 0x14) = projected_movement;
    *(real *)(control + 0x20) = *(real const *)(input + 0x14);
    *(real *)(control + 0x24) = *(real const *)(input + 0x18);
    *(dword *)(control + 6) = *(dword const *)(input + 0x1e);
    *(word *)(control + 0xa) = *(word const *)(input + 0x22);
    *(word *)(control + 0xc) = *(word const *)(input + 0x24);
    *(dword *)control = 0x06000086;
    *(word *)(control + 4) = 0;
    memcpy(control + 0x58, input + 0x34, 0x24);
    dword *flags = (dword *)((byte *)tail_object_get(g_4e0300, unit_index) + 0x134);
    if (input[0x58]) *flags |= 0x40000000;
    else *flags &= ~0x40000000;
    function_c6de0(unit_index, control);
}



short __stdcall function_bb050(long, dword, void const *, point3f const *, real, long *, short);

// @retail 0x151110
void __stdcall function_151110(long player_index, void *output)
{
    s_tail_player *player = tail_player_get(player_index);
    memset(output, 0, 12);
    memset(output, 0, 8);
    *(long *)((byte *)output + 4) = NONE;
    *(long *)((byte *)output + 8) = NONE;
    long unit_index = player->unit_index;
    if (unit_index == NONE) return;
    byte *unit = (byte *)tail_object_get(g_4e0300, unit_index);
    if (*(long *)(unit + 0x14) != NONE) return;
    struct s_region { real radius; point3f *center; } region;
    region.radius = *(real *)(unit + 0x3c) + 0.4f;
    region.center = (point3f *)(unit + 0x30);
    long objects[64];
    long count = function_bb050(0, 0xffffebcf, unit + 0x28, region.center, region.radius + 3.0f, objects, 64);
    for (long i = 0; i < count; ++i)
    {
        long object_index = objects[i];
        byte *object = (byte *)tail_object_get(g_4e0300, object_index);
        long type = *(signed char *)(object + 0xaa);
        dword mask = 1 << type;
        if (mask & 0x1430) continue;
        if (mask & 0x184)
        {
            real reach = *(real *)(object + 0x3c) + region.radius;
            real x = *(real *)(object + 0x30) - region.center->x;
            real y = *(real *)(object + 0x34) - region.center->y;
            real z = *(real *)(object + 0x38) - region.center->z;
            real distance = x * x;
            distance += z * z;
            distance += y * y;
            if (!(distance <= reach * reach)) continue;
        }
        s_target_candidate *best = (s_target_candidate *)output;
        switch (type)
        {
        case 2: function_151780(object_index, player_index, best); break;
        case 8: function_1516c0(object_index, player_index, best); break;
        case 1: function_151430(object_index, player_index, best); break;
        case 0: function_1515e0(object_index, player_index, best); break;
        default:
            {
                long child_index = *(long *)(object + 0x10);
                while (child_index != NONE && (unsigned long)count < 64)
                {
                    objects[count++] = child_index;
                    child_index = *(long *)((byte *)tail_object_get(g_4e0300, child_index) + 0xc);
                }
            }
            break;
        }
    }
}

bool __stdcall function_ffc60(long weapon_index, long other_index, long unit_index, long local_user, short *rounds);
bool __stdcall function_ccba0(long unit_index, long grenade_index);
long function_ccb80(long unit_index);
void function_15e8e0(long object_index, long player_index);
void __stdcall function_191ec4(long player_index, long definition_index, short count);
void __stdcall function_191e51(long definition_index, long player_index);

// @retail 0x150e30
void __stdcall function_150e30(long player_index, long object_index)
{
    s_record_pool *objects = g_4e0300;
    s_tail_player *player = tail_player_get(player_index);
    if (player->unit_index == NONE) return;
    byte *unit = (byte *)tail_object_get(objects, player->unit_index);
    byte *object = (byte *)tail_object_get(objects, object_index);
    bool parent_ok = false;
    if (*(long *)(object + 0x14) != NONE)
    {
        long parent_type = ((s_tail_object_header *)g_4e0300->data)[*(long *)(object + 0x14) & 0xffff].type;
        parent_ok = parent_type == 11 || parent_type == 7;
    }
    if (*(long *)(object + 0x14) != NONE && !parent_ok) return;
    if (*(long *)(object + 0x14c) == player->unit_index) return;
    vector3f delta;
    vector3d_from_points3d((point3f const *)(unit + 0x30), (point3f const *)(object + 0x30), &delta);
    real radius = *(real *)(unit + 0x3c) + 0.3f;
    if (!(delta.j * delta.j + delta.i * delta.i + delta.k * delta.k < radius * radius)) return;
    long weapon_index = NONE;
    short rounds;
    short i = 0;
    do
    {
        long current = *(signed char *)(unit + 0x212);
        current = current < 0 ? 0 : current;
        long slot = (current + i) % 4;
        weapon_index = *(long *)(unit + 0x218 + slot * 4);
        if (weapon_index != NONE && function_ffc60(weapon_index, object_index, player->unit_index, player->local_index, &rounds))
        {
            if (rounds > 0)
                function_191ec4(player->local_index, *(long *)tail_object_get(g_4e0300, weapon_index), rounds);
            break;
        }
        ++i;
    } while (i < 4);
    s_object *pickup = function_badc0(object_index, 8);
    if (pickup)
    {
        byte *definition = g_4e3b44[*(long *)pickup & 0xffff].bytes;
        if (*(short *)(definition + 0x12c) == 6)
        {
            if (function_ccba0(player->unit_index, object_index))
                function_191e51(*(long *)pickup, player->local_index);
        }
        else if (*(short *)(definition + 0x12c) != 0 && function_ccb80(player->unit_index) == NONE && !function_138880())
            function_151ea0(player_index, object_index);
    }
    byte type = ((s_tail_object_header *)g_4e0300->data)[object_index & 0xffff].type;
    if ((1 << type) & 4)
    {
        bool modes[4];
        if (function_cd7b0(player->unit_index, object_index, modes) && modes[0] && function_150dd0(player_index))
        {
            if (function_138880())
                function_a8f10(player->unit_index, object_index, 3);
            else
                unit_pickup_request(player->unit_index, object_index, 3);
        }
    }
    function_15e8e0(object_index, player_index);
}


// @retail 0x1508e0
void __stdcall function_1508e0(long player_index)
{
    s_tail_player *player = tail_player_get(player_index);
    long objects[16];
    if (player->unit_index != NONE)
    {
        byte *unit = (byte *)tail_object_get(g_4e0300, player->unit_index);
        if (*(long *)(unit + 0x14) == NONE)
        {
            long count = function_bb050(0, 0x89c, unit + 0x28, (point3f *)(unit + 0x30),
                *(real *)(unit + 0x3c) + 0.3f, objects, 16);
            for (long i = 0; i < count; ++i)
            {
                long index = objects[i];
                byte *object = (byte *)tail_object_get(g_4e0300, index);
                long type = *(signed char *)(object + 0xaa);
                if (type >= 2 && type <= 3) function_150e30(player_index, index);
                if (object[0xaa] == 11 || object[0xaa] == 7)
                {
                    long child = *(long *)(object + 0x10);
                    while (child != NONE && (unsigned long)count < 16)
                    {
                        objects[count++] = child;
                        child = *(long *)((byte *)tail_object_get(g_4e0300, child) + 0xc);
                    }
                }
            }
        }
    }
}

