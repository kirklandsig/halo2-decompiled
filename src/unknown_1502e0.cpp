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

static inline s_tail_object *tail_object_get(long object_index)
{
	return (s_tail_object *)((s_tail_object_header *)g_4e0300->data)[object_index & 0xffff].object;
}

void function_14cad0(long player_index, long unit_index);
void function_187a60(long datum_index);
void function_152340(void);

bool __stdcall function_e5470(long player_index, long object_index);
long __stdcall function_e5bc0(long unit_index, long object_index, long previous_index);
bool function_1df560(short team_a, short team_b);
bool function_151320(s_target_candidate *candidate, s_target_candidate *best, long player_index);

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
	s_1515e0_object *object = (s_1515e0_object *)tail_object_get(object_index);
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
			function_151320(&candidate, *best_reference, *player_reference);
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
	long const *player_reference = &player_index;
	long const *object_reference = &object_index;
	s_tail_player *player = tail_player_get(*player_reference);
	s_tail_object_header *header = (s_tail_object_header *)g_4e0300->data + (*object_reference & 0xffff);
	byte *object = (byte *)header->object;
	byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
	long ticks = game_seconds_to_ticks_round(*(real *)(definition + 0x130));
	bool unavailable = false;
	if (header->flags & 0x10)
		unavailable = true;
	if ((short)ticks > 0 && !unavailable)
	{
		short kind = *(short *)(definition + 0x12c);
		if (kind != 1)
		{
			if (kind == 2)
			{
				if (!function_d6af0(player->unit_index)) return;
				player_speed_request_14ea20(*player_reference);
			}
			else if (kind == 5)
			{
				if (!function_d6790(player->unit_index)) return;
				player_speed_request_14ec00(*player_reference);
			}
			else
			{
				short effect = kind == 3 ? 0 : 1;
				if (!function_152000(*player_reference, effect, (short)ticks)) return;
				if (effect == 0) player_speed_request_14eb10(*player_reference);
			}
		}
		long definition_index = *(long *)tail_object_get(*object_reference);
		function_191fab(definition_index, player->local_index);
		function_a89c0(player->unit_index, definition_index);
		if (player->local_index != NONE) function_f8110(*object_reference);
		function_b8540(*object_reference);
	}
}

// @retail 0x151320
bool function_151320(s_target_candidate *candidate, s_target_candidate *best, long player_index)
{
	bool result = false;
	long const *player_reference = &player_index;
	point3f player_position;
	point3f best_position;
	point3f candidate_position;
	vector3f to_candidate;
	vector3f to_best;
	real candidate_distance;
	real best_distance;

	if (candidate->priority > best->priority)
		goto copy;
	if (candidate->priority == best->priority)
	{
		function_b9dd0(tail_player_get(*player_reference)->unit_index, &player_position);
		function_b9dd0(best->object_index, &best_position);
		function_b9dd0(candidate->object_index, &candidate_position);
		vector3d_from_points3d(&player_position, &candidate_position, &to_candidate);
		vector3d_from_points3d(&player_position, &best_position, &to_best);
		candidate_distance = to_candidate.j * to_candidate.j + to_candidate.i * to_candidate.i + to_candidate.k * to_candidate.k;
		best_distance = length_sq3f(&to_best);
		if (candidate_distance < best_distance)
		{
copy:
			result = true;
			*best = *candidate;
		}
		else
		{
			result = false;
		}
	}
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
	byte const *identifier = ((s_tail_player_identifier *)tail_player_get(player_index))->identifier;
	s_tail_scenario *scenario = (s_tail_scenario *)g_4e0350;
	bool alternate = false;

	if (scenario->entry_count > 0 && scenario->entries[0].value2c == 1)
		alternate = true;
	for (long i = 0; i < 16; i++)
	{
		s_tail_options_player *entry = &((s_tail_options *)g_4e6948)->players[i];
		long difference = memcmp(entry->identifier, identifier, sizeof(entry->identifier));

		if (difference == 0)
			return &entry->traits[alternate ? 1 : 0];
	}
	return 0;
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
	if ((1 << tail_object_get(object_index)->type) & 3)
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

short __stdcall function_cdeb0(long unit_index, long state_name, long a, long b);
long function_cbe60(long unit_index);
bool function_cd660(long unit_index);

// @retail 0x150dd0
bool function_150dd0(long player_index)
{
	s_tail_player *player = tail_player_get(player_index);
	long const &current = player->unit_index;
	if ((short)function_cdeb0(current, NONE, NONE, 0) == NONE)
		return true;
	if (g_4e6948->state != 2)
	{
		long unit_index = current;
		if (function_cbe60(unit_index) == NONE && !function_cd660(unit_index))
			return true;
	}
	return false;
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
	s_player_motion_unit *unit = (s_player_motion_unit *)tail_object_get(unit_index);
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
	s_player_target_object *unit = (s_player_target_object *)tail_object_get(player->unit_index);
	s_player_target_object *object = (s_player_target_object *)tail_object_get(object_index);
	point3f eye;
	function_cafc0(player->unit_index, &eye);
	if (function_11e5e0(&eye, &unit->direction, &object->position, object->radius) &&
		function_1078f0(object_index, &unit->direction) && function_107870(object_index))
	{
		s_target_candidate candidate;
		memset(&candidate, 0, sizeof(candidate));
		candidate.priority = 3;
		candidate.object_index = object_index;
		function_151320(&candidate, *best_reference, *player_reference);
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
	s_player_target_object *unit = (s_player_target_object *)tail_object_get(player->unit_index);
	s_player_pickup_object *object = (s_player_pickup_object *)tail_object_get(object_index);
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
			function_151320(&candidate, *best_reference, *player_reference);
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
		s_player_removed_unit *unit = (s_player_removed_unit *)tail_object_get(unit_index);
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
	s_player_target_unit *object = (s_player_target_unit *)tail_object_get(object_index);
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
					function_151320(&candidate, *best_reference, *player_reference);
				}
			}
		}
		else if (player->unit_index != NONE)
		{
			s_player_target_unit *unit = (s_player_target_unit *)tail_object_get(player->unit_index);
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
					function_151320(&candidate, *best_reference, *player_reference);
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
				s_tail_transfer_unit *unit = (s_tail_transfer_unit *)tail_object_get(unit_index);
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
					s_tail_transfer_unit *first = (s_tail_transfer_unit *)tail_object_get(unit_index);
					short primary_slot = first->primary_slot;
					if (primary_slot != NONE)
						primary = first->weapons[primary_slot];
					s_tail_transfer_unit *second = (s_tail_transfer_unit *)tail_object_get(unit_index);
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

static __forceinline dword player_weapon_flags(long weapon_index)
{
	s_player_pickup_object *weapon = (s_player_pickup_object *)tail_object_get(weapon_index);
	return ((s_player_weapon_definition *)g_4e3b44[weapon->definition_index & 0xffff].bytes)->flags;
}

// @retail 0x1509e0
void function_1509e0(long player_index, long weapon_index, bool *modes)
{
	long const *weapon_reference = &weapon_index;
	bool *const *modes_reference = &modes;
	s_tail_player *player = tail_player_get(player_index);
	s_player_pickup_unit *unit = (s_player_pickup_unit *)tail_object_get(player->unit_index);
	long held[2];
	long pending[2];
	long count = 0;
	{
		long hand = 0;
		if (hand < 2)
		{
			do
			{
				s_player_pickup_unit *current = (s_player_pickup_unit *)tail_object_get(player->unit_index);
				short slot = current->held_slots[hand];
				held[hand] = slot == NONE ? NONE : current->weapons[slot];
				s_player_pickup_unit *next = (s_player_pickup_unit *)tail_object_get(player->unit_index);
				short next_slot = next->pending_slots[hand];
				pending[hand] = next_slot == NONE ? NONE : next->weapons[next_slot];
				if (held[hand] != NONE && (bool)((player_weapon_flags(held[hand]) >> 3) & 1))
				{
					(*modes_reference)[hand] = false;
					(*modes_reference)[hand + 2] = false;
				}
				hand++;
			} while (hand < 2);
		}
	}
	if (unit->parent_index != NONE)
	{
		{
			long hand = 0;
			if (hand < 2)
			{
				do
				{
					(*modes_reference)[hand] = false;
					(*modes_reference)[hand + 2] = false;
					hand++;
				} while (hand < 2);
			}
		}
	}
	for (long slot = 0; slot < 4; slot++)
	{
		long held_weapon = unit->weapons[slot];
		if (held_weapon != NONE && !(bool)((player_weapon_flags(held_weapon) >> 4) & 1))
		{
			dword flags = player_weapon_flags(*weapon_reference);
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
				if (held[1] != NONE && !(bool)((player_weapon_flags(held[1]) >> 4) & 1))
					(*modes_reference)[1] = false;
			}
			else if (!(bool)((player_weapon_flags(*weapon_reference) >> 4) & 1))
			{
				(*modes_reference)[hand] = false;
				if (held[hand] != NONE && (bool)((player_weapon_flags(held[hand]) >> 4) & 1))
					(*modes_reference)[hand + 2] = false;
			}
		}
	}
}

#include "unit_requests.h"

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

// @retail 0x151b80
bool function_151b80(long player_index, bool primary)
{
	s_tail_player *player = tail_player_get(player_index);
	bool result = false;
	if (player->unit_index != NONE)
	{
		long hand = !primary;
		s_player_pickup_unit *unit = (s_player_pickup_unit *)tail_object_get(player->unit_index);
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

// @retail 0x151aa0
bool function_151aa0(long player_index, bool primary)
{
	s_tail_player *player = tail_player_get(player_index);
	bool result = false;
	if (player->unit_index != NONE)
	{
		long hand = !primary;
		s_player_pickup_unit *unit = (s_player_pickup_unit *)tail_object_get(player->unit_index);
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
		s_player_pickup_object *weapon = (s_player_pickup_object *)tail_object_get(weapon_index);
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
			else
			{
				s_unit_request request;
				memset(&request, 0, sizeof(request));
				request.type = 0x14;
				*(long *)request.arguments = weapon_index;
				*(short *)(request.arguments + 4) = mode;
				if (function_e6900(player->unit_index, &request)) result = true;
			}
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
				long definition = *(long *)tail_object_get(target);
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
						byte *unit = (byte *)tail_object_get(blocker);
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

// @retail 0x152f80
void __stdcall function_152f80(void *address, void *option_data)
{
    byte const *machine_address = (byte const *)address;
    s_player_rebind_option const *options = (s_player_rebind_option const *)option_data;
    long controllers[16] = { NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE };
    long users[16];
    long assigned[16];
    dword options_used = 0;
    dword users_used = 0;
    dword controllers_used = 0;
    for (long i = 0; i < 16; i++)
    {
        users[i] = NONE;
        assigned[i] = NONE;
    }
    s_record_pool_iterator iterator;
    s_tail_player *player;
    iterator.data = g_4e8c24;
    for (long pass = 0; pass < 4; pass++)
    {
        iterator.index = NONE;
        while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
        {
            long index = (short)iterator.datum_index;
            if (assigned[index] != NONE) continue;
            for (long option = 0; option < 16; option++)
            {
                s_player_rebind_option const *entry = &options[option];
                if (!entry->active || (options_used & (1 << option))) continue;
                bool match;
                switch (pass)
                {
                case 0:
                    match = player->local_index != NONE && !memcmp(entry->identifier, player->unknown04, 12);
                    break;
                case 1:
                    match = player->local_index != NONE && entry->controller_index == player->controller_index;
                    break;
                case 2:
                    match = player->local_index != NONE;
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
    iterator.index = NONE;
    while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
    {
        long index = (short)iterator.datum_index;
        if (assigned[index] != NONE || player->local_index == NONE) continue;
        long controller = NONE;
        long user = NONE;
        for (long pass = 0; pass < 3; pass++)
        {
            if (controller != NONE) break;
            for (long candidate = 0; candidate != NONE; candidate = candidate >= 0 && candidate < 3 ? candidate + 1 : NONE)
            {
                if (!(controllers_used & (1 << candidate)) &&
                    (pass > 0 || candidate == player->controller_index) &&
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
                if (!(users_used & (1 << candidate)) && (pass > 0 || candidate == player->local_index))
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
    byte *globals = (byte *)g_4e8c20;
    if (*(long *)(globals + 0x98) != NONE)
    {
        iterator.index = NONE;
        while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
            if (player->local_index != NONE) memcpy(player->machine_address, machine_address, 6);
        memcpy(globals + 0x91, machine_address, 6);
        memcpy(globals + 0x30 + *(long *)(globals + 0x98) * 6, machine_address, 6);
    }
    else
    {
        globals[0x90] = 1;
        memcpy(globals + 0x91, machine_address, 6);
        *(long *)(globals + 0x98) = 0;
        *(dword *)(globals + 0x2c) |= 1;
        memcpy(globals + 0x30, machine_address, 6);
    }
    iterator.data = g_4e8c24;
    iterator.index = NONE;
    while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
    {
        long index = (short)iterator.datum_index;
        if (player->local_index != NONE && (player->local_index != users[index] || player->controller_index != controllers[index]))
        {
            player_set_local_user(iterator.datum_index, NONE);
            function_14f270(iterator.datum_index, NONE);
            player->machine_user = NONE;
            player->machine_controller = NONE;
        }
    }
    iterator.data = g_4e8c24;
    iterator.index = NONE;
    while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
    {
        long index = (short)iterator.datum_index;
        if (users[index] == NONE) continue;
        memcpy(player->machine_address, machine_address, 6);
        player->machine_index = (short)*(long *)((byte *)g_4e8c20 + 0x98);
        if (player->local_index == NONE)
        {
            player->machine_user = (short)users[index];
            player->machine_controller = controllers[index];
            player_set_local_user(iterator.datum_index, player->machine_user);
            function_14f270(iterator.datum_index, player->machine_controller);
        }
        s_player_slot *slot = &g_54e8e0[player->controller_index];
        if (TEST_FIELD_BIT(slot->flag4)) *(long *)slot->unknown004 = player->local_index;
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
                byte *object = (byte *)tail_object_get(result);
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
        byte *unit = (byte *)tail_object_get(player->unit_index);
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
    if (loadout[0])
    {
        byte *unit = (byte *)tail_object_get(unit_index);
        function_ccff0(unit_index);
        long primary, special, secondary;
        bool result;
        {
            bool failed = false;
            primary = function_152a50((s_loadout_state_152a50 *)(loadout + 2), unit_index, &failed);
            special = function_152a50((s_loadout_state_152a50 *)(loadout + 0x12), unit_index, &failed);
            secondary = function_152a50((s_loadout_state_152a50 *)(loadout + 0xa), unit_index, &failed);
            result = !failed;
        }
        for (long i = 0; i < 2; ++i)
        {
            unit[0x23e + i] = 0;
            unit[0x23e + i] = loadout[0x1a + i];
        }
        if (primary != NONE && !function_cd0c0(unit_index, primary, 1))
        { function_b8540(primary); result = false; }
        if (secondary != NONE && !function_cd0c0(unit_index, secondary, 1))
        { function_b8540(secondary); result = false; }
        if (special != NONE && !function_cd0c0(unit_index, special, 7))
        { function_b8540(special); return false; }
        return result;
    }
    return false;
}

bool function_cb810(long unit_index, vector3f *vector);
// @retail 0x1505f0
void function_1505f0(long player_index, long unit_index, byte const *input)
{
    (void)&unit_index; (void)&input;
    vector3f facing;
    facing.i = (real)(cos(*(real const *)(input + 4)) * cos(*(real const *)(input + 8)));
    facing.j = (real)(sin(*(real const *)(input + 4)) * cos(*(real const *)(input + 8)));
    facing.k = (real)sin(*(real const *)(input + 8));
    long player_unit = tail_player_get(player_index)->unit_index;
    if (player_unit != NONE) function_cb810(player_unit, &facing);
    vector3f projected_facing, movement;
    function_150400(player_index, &facing, (vector3f const *)(input + 0xc), &projected_facing, &movement);
    byte control[0x7c];
    function_c6ef0((s_unit_state_c6ef0 *)control);
    *(dword *)(control + 0x10) = *(dword const *)input;
    *(vector3f *)(control + 0x14) = movement;
    *(real *)(control + 0x20) = *(real const *)(input + 0x14);
    *(real *)(control + 0x24) = *(real const *)(input + 0x18);
    *(vector3f *)(control + 0x28) = projected_facing;
    *(vector3f *)(control + 0x34) = facing;
    *(vector3f *)(control + 0x40) = facing;
    *(dword *)(control + 6) = *(dword const *)(input + 0x1e);
    *(word *)(control + 0xa) = *(word const *)(input + 0x22);
    *(word *)(control + 0xc) = *(word const *)(input + 0x24);
    *(dword *)control = 0x06000086;
    *(word *)(control + 4) = 0;
    memcpy(control + 0x58, input + 0x34, 0x24);
    dword *flags = (dword *)((byte *)tail_object_get(unit_index) + 0x134);
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
    byte *unit = (byte *)tail_object_get(unit_index);
    if (*(long *)(unit + 0x14) != NONE) return;
    struct s_region { real radius; point3f *center; } region;
    region.radius = *(real *)(unit + 0x3c) + 0.4f;
    region.center = (point3f *)(unit + 0x30);
    long objects[64];
    long count = function_bb050(0, 0xffffebcf, unit + 0x28, region.center, region.radius + 3.0f, objects, 64);
    for (long i = 0; i < count; ++i)
    {
        long object_index = objects[i];
        byte *object = (byte *)tail_object_get(object_index);
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
                    child_index = *(long *)((byte *)tail_object_get(child_index) + 0xc);
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
void __stdcall function_191ec4(long definition_index, long player_index, short count);
void __stdcall function_191e51(long definition_index, long player_index);

// @retail 0x150e30
void __stdcall function_150e30(long player_index, long object_index)
{
    s_tail_player *player = tail_player_get(player_index);
    if (player->unit_index == NONE) return;
    byte *unit = (byte *)tail_object_get(player->unit_index);
    byte *object = (byte *)tail_object_get(object_index);
    long parent_index = *(long *)(object + 0x14);
    bool parent_ok = false;
    if (parent_index != NONE)
    {
        long parent_type = ((s_tail_object_header *)g_4e0300->data)[parent_index & 0xffff].type;
        parent_ok = parent_type == 11 || parent_type == 7;
    }
    if (*(long *)(object + 0x14) != NONE && !parent_ok) return;
    if (*(long *)(object + 0x14c) == player->unit_index) return;
    real y = *(real *)(object + 0x34) - *(real *)(unit + 0x34);
    real x = *(real *)(object + 0x30) - *(real *)(unit + 0x30);
    real z = *(real *)(object + 0x38) - *(real *)(unit + 0x38);
    real radius = *(real *)(unit + 0x3c) + 0.3f;
    if (!(z * z + x * x + y * y < radius * radius)) return;
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
                function_191ec4(*(long *)tail_object_get(weapon_index), player->local_index, rounds);
            break;
        }
        ++i;
    } while (i < 4);
    s_object *pickup = function_badc0(object_index, 8);
    if (pickup)
    {
        byte *definition = g_4e3b44[*(long *)pickup & 0xffff].bytes;
        short kind = *(short *)(definition + 0x12c);
        if (kind == 6)
        {
            if (function_ccba0(player->unit_index, object_index))
                function_191e51(*(long *)pickup, player->local_index);
        }
        else if (kind != 0 && function_ccb80(player->unit_index) == NONE && !function_138880())
            function_151ea0(player_index, object_index);
    }
    byte type = ((s_tail_object_header *)g_4e0300->data)[object_index & 0xffff].type;
    if ((1 << type) & 4)
    {
        bool modes[4];
        if (function_cd7b0(player->unit_index, object_index, modes) && modes[0] && function_150dd0(player_index))
        {
            if (function_138880())
            {
                function_a8f10(player->unit_index, object_index, 3);
                function_15e8e0(object_index, player_index);
                return;
            }
            s_unit_request request;
            memset(&request, 0, sizeof(request));
            request.type = 0x14;
            *(long *)request.arguments = object_index;
            *(short *)(request.arguments + 4) = 3;
            function_e6900(player->unit_index, &request);
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
        byte *unit = (byte *)tail_object_get(player->unit_index);
        if (*(long *)(unit + 0x14) == NONE)
        {
            long count = function_bb050(0, 0x89c, unit + 0x28, (point3f *)(unit + 0x30),
                *(real *)(unit + 0x3c) + 0.3f, objects, 16);
            for (long i = 0; i < count; ++i)
            {
                long index = objects[i];
                byte *object = (byte *)tail_object_get(index);
                long type = *(signed char *)(object + 0xaa);
                if (type >= 2 && type <= 3) function_150e30(player_index, index);
                if (object[0xaa] == 11 || object[0xaa] == 7)
                {
                    long child = *(long *)(object + 0x10);
                    while (child != NONE && (unsigned long)count < 16)
                    {
                        objects[count++] = child;
                        child = *(long *)((byte *)tail_object_get(child) + 0xc);
                    }
                }
            }
        }
    }
}

