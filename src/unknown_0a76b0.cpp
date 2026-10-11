// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0A76B0.CPP: the requests a unit performs (its actions)

The handlers of the unit request table at 0x467564
(unknown_0e6900.cpp) and the helpers only they call. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "effects.h"
#include "object_markers.h"
#include "data_array.h"
#include "unknown_1c62f0.h"
#include "unknown_1cafc0.h"
#include "unknown_1dacb0.h"
#include "unit_requests.h"
#include "unknown_1cec30.h"
#include <math.h>
#include <string.h>

/* a unit's actions: a bit per request type being performed, after a header
   dword (the block at the offset in the unit at +0x346) */
struct s_unit_actions
{
	long unknown00;
	dword active[2];
	byte grenade_state;
	bool grenade_cancelled;
	char grenade_throw_count;
	char grenade_throw_maximum;
	long grenade_projectile_index;
	short grenade_type;
	char point_mode;
	byte unknown17;
	long point_target;
	short point_ticks;
	short hoist_ticks;
	vector3f unknown20;
	long melee_name;
	byte melee_flags;
	char melee_unknown31;
	char melee_counter;
	char melee_damage_ticks;
	char melee_end_ticks;
	char melee_ticks;
	short unknown36;
	short surprise_ticks;
	short surprise_value;
	word surprise_type;
	short flinch_ticks;
	bool weapon_planted;
};

/* the unit (the fields read here) */
struct s_unit_action_unit
{
	long definition_index;
	dword object_flags;
	byte unknown008[0x14 - 0x8];
	long parent_object_index;
	byte unknown018[0x70 - 0x18];
	vector3f forward;
	vector3f up;
	vector3f linear_velocity;
	byte unknown094[0xaa - 0x94];
	byte object_type;
	byte unknown0ab[0xb4 - 0xab];
	long havok_component_index;
	byte unknown0b8[0xc0 - 0xb8];
	byte flags_c0;
	byte unknown0c1[0xd4 - 0xc1];
	long unknown0d4;
	byte unknown0d8[0x10a - 0xd8];
	byte flags_10a;
	byte unknown10b[0x12a - 0x10b];
	short animation_state_offset;
	long unknown12c;
	byte unknown130[0x134 - 0x130];
	dword flags_134;
	short unknown138;
	byte unknown13a[2];
	long unknown13c;
	byte unknown140[0x168 - 0x140];
	vector3f field_xe66477;
	byte unknown174[0x1fc - 0x174];
	short seat_index;
	byte unknown1fe[0x212 - 0x1fe];
	char current_weapon_index;
	char current_secondary_weapon_index;
	byte unknown214[2];
	char weapon_slots[2];
	long weapon_object_indices[4];
	long weapon_ready_times[4];
	byte unknown238[0x23c - 0x238];
	char current_grenade_index;
	byte unknown23d;
	char grenade_counts[4];
	byte unknown242[0x344 - 0x242];
	short actions_size;
	short actions_offset;
};

struct s_unit_action_header
{
	byte unknown00[2];
	byte flags;
	byte type;
	byte unknown04[4];
	s_unit_action_unit *unit;
};

/* a unit's animation state, as the actions read it */
struct s_unit_action_animation_state
{
	byte unknown00[0x70];
	long state_name;
	long action_name;
	byte unknown78[4];
	long field_7c;
};

/* a unit request type's handlers (unknown_0e6900.cpp) */
typedef bool (__stdcall *t_unit_request_proc)(long unit_index, s_unit_request *request);
typedef bool (__stdcall *t_unit_request_update_proc)(long unit_index, long type);
typedef void (__stdcall *t_unit_request_end_proc)(long unit_index, long type);

struct s_unit_request_definition
{
	t_unit_request_proc perform;
	t_unit_request_update_proc update;
	t_unit_request_end_proc finished;
	t_unit_request_end_proc interrupted;
};

extern s_unit_request_definition *g_4677c8[60];

#define UNIT_ACTION_UNIT_GET(index) (((s_unit_action_header *)g_4e0300->data)[(index) & 0xffff].unit)
#define UNIT_ACTION_HEADER_GET(index) (&((s_unit_action_header *)g_4e0300->data)[(index) & 0xffff])
#define UNIT_ACTIONS_GET(unit) ((s_unit_actions *)((byte *)(unit) + (unit)->actions_offset))
#define UNIT_ACTION_ANIMATION_STATE_GET(unit) \
	((s_unit_action_animation_state *)((byte *)(unit) + (unit)->animation_state_offset))
#define UNIT_ACTION_ACTIVE(actions, type) (((actions)->active[(type) >> 5] >> ((type) & 0x1f)) & 1)

bool unit_action_active(long unit_index, long action_type);
void function_b58c0(long index, dword mask);
void __stdcall function_b9b90(long object_index, bool disable);
void function_b7360(long object_index);
void function_bba20(long object_index);
void function_b9fc0(long object_index, vector3f *forward, vector3f *up);
bool function_0c7070(long unit_index);
void function_cf040(long unit_index, short type);
byte function_10f3b0(long object_index, long name, long unknown);
bool function_10f340(long object_index, long name, long unknown);
bool function_10f630(long object_index, long *first, long *second);
bool __stdcall function_110ab0(long unit_index);
long function_cbd50(long unit_index, short weapon_index);
bool function_101490(long weapon_index, long magazine_index);
void function_c86e0(long unit_index, bool keep_weapon_zoom);
bool function_1029d0(long weapon_index, short magazine_index);

bool __stdcall function_113e90(long unit_index, long name, real blend, c_animation_channel **channel, long mode);
bool function_100880(long object_index, long slot_index);
void function_1015a0(long weapon_index);
void __stdcall function_ec0d0(long unit_index, long state_name, long action_name);

/* a unit's weapon in a hand (the second hand for the dual wielded
   requests) */
PRIVATE inline long unit_action_weapon_get(s_unit_action_unit *unit, bool secondary)
{
	char index = secondary ? unit->current_secondary_weapon_index : unit->current_weapon_index;
	long weapon_index = NONE;

	if (index != NONE)
		weapon_index = unit->weapon_object_indices[index];
	return weapon_index;
}

/* ends an action (its finished handler), and clears it */
// @retail 0xe6960
void function_e6960(long unit_index, long type)
{
	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));
	t_unit_request_end_proc finished = g_4677c8[type]->finished;

	if (finished)
		finished(unit_index, type);
	actions->active[type >> 5] &= ~(1 << (type & 0x1f));
}

/* stops an action (its interrupted handler), and clears it */
// @retail 0xe69c0
void function_e69c0(long unit_index, long type)
{
	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));
	t_unit_request_end_proc interrupted = g_4677c8[type]->interrupted;

	if (interrupted)
		interrupted(unit_index, type);
	actions->active[type >> 5] &= ~(1 << (type & 0x1f));
}

/* an animation event of a unit's action */
// @retail 0xe6a20
void function_e6a20(long unit_index, long name, bool active, bool keep)
{
	if (active & 1)
	{
		switch (name)
		{
		case 0x90006b2:
			function_cf040(unit_index, 1);
			break;
		case 0xc0006b3:
			function_cf040(unit_index, 2);
			break;
		case 0x500000a:
		case 0xa000040:
		case 0xa0005b9:
		case 0xb0005b2:
		case 0xc000073:
		case 0xc000075:
		case 0xc000077:
		case 0xc0006cd:
		case 0xe000038:
		case 0xe00067d:
		case 0x11000074:
		case 0x11000076:
		case 0x140005b3:
			if (UNIT_ACTION_ACTIVE(UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index)), 0x1a))
			{
				function_cf040(unit_index, 0);
				if (!(keep & 1))
					function_e6960(unit_index, 0x1a);
			}
			break;
		}
	}
}

void function_e6b00(long unit_index, long name, long *state_name, long *action_name);
PRIVATE void function_e70b0(long unit_index, short *value);

/* when an animation of a unit ends: the action it ends, and the state and
   action animations that follow */
// @retail 0xe6b00
void function_e6b00(long unit_index, long name, long *state_name, long *action_name)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	long next_state = 0x7000101;
	long next_action = 0x7000101;

	if (state_name)
		next_state = *state_name;
	if (action_name)
		next_action = *action_name;

	if (function_0c7070(unit_index))
	{
		long current = UNIT_ACTION_ANIMATION_STATE_GET(unit)->state_name;

		if (current == 0x110001b4 || current == 0x120001b5 || current == 0xc0006c3)
		{
			function_e6960(unit_index, 0x24);
			next_state = 0x7000101;
			next_action = 0x400000c;
		}
		else if (current == 0x5000281)
		{
			function_e6960(unit_index, 0x2a);
			next_action = 0x800001e;
			next_state = 0x6000086;
		}
		else if (current == 0x5000534)
		{
			function_e6960(unit_index, 0x2b);
			next_action = 0x400000c;
			next_state = 0x6000086;
		}
		else if (current == 0xf000545 || current == 0x10000546 || current == 0x11000555 || current == 0x12000547)
		{
			function_e6960(unit_index, 0x2d);
			next_state = 0x7000101;
			next_action = 0x400000c;
		}
		else
		{
			function_e6960(unit_index, 0x1c);
		}
	}
	else
	{
		switch (name)
		{
		case 0x400069d:
			function_e6960(unit_index, 0x2f);
			next_state = 0x6000086;
			next_action = 0x400000c;
			break;
		case 0x400004a:
		{
			short count;

			function_e70b0(unit_index, &count);
			if (count == 0)
			{
				function_e6960(unit_index, 0x1d);
				next_state = 0x6000086;
				next_action = 0x800001e;
			}
			else if ((actions->active[1] >> 8) & 1)
			{
				function_e6960(unit_index, 0x28);
				next_state = 0x6000542;
				next_action = 0x400000c;
			}
			else if ((actions->active[1] >> 14) & 1)
			{
				function_e6960(unit_index, 0x2e);
				next_state = 0x6000086;
				next_action = 0x400000c;
			}
			else
			{
				function_e6960(unit_index, 0x27);
				next_state = 0x6000086;
				next_action = 0x800001e;
			}
			break;
		}
		case 0x50000c3:
			function_e6960(unit_index, 0x1f);
			break;
		case 0x60000cc:
			function_e6960(unit_index, 0x22);
			next_state = 0x50000cb;
			next_action = 0x400000c;
			break;
		case 0x60000cd:
			function_e6960(unit_index, 0x23);
			next_state = 0x6000086;
			next_action = 0x800001e;
			break;
		case 0x60006ac:
			function_e6960(unit_index, 0x35);
			next_state = 0x6000086;
			next_action = 0x400000c;
			break;
		case 0x7000543:
			function_e6960(unit_index, 0x2c);
			next_state = 0x6000542;
			next_action = 0x400000c;
			break;
		case 0x8000025:
			function_e6960(unit_index, 8);
			break;
		case 0x80000c4:
			function_e6960(unit_index, 0x20);
			break;
		case 0x800061e:
			next_action = 0x400000c;
			next_state = 0x7000101;
			break;
		case 0xa00003e:
			function_e6960(unit_index, 0x30);
			next_action = 0xd00003f;
			break;
		case 0xa000066:
		case 0xb00006d:
			function_e68c0(0x37, unit_index);
			break;
		case 0xd000539:
			function_e6960(unit_index, 0x12);
			break;
		case 0xe00003b:
		case 0xf00003a:
			function_e6960(unit_index, 0x31);
			next_action = 0x400000c;
			next_state = 0x6000086;
			break;
		case 0xf00067f:
		case 0x1000067e:
			function_e68c0(0x3a, unit_index);
			break;
		}
	}

	if (name == 0x6000007)
	{
		long weapon_index = unit_action_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), false);

		if (weapon_index != NONE &&
			*(short *)(g_4e3b44[UNIT_ACTION_UNIT_GET(weapon_index)->definition_index & 0xffff].bytes + 0x292) == 3)
		{
			function_e68c0(0x37, unit_index);
		}
	}
	else if (name == 0xb00053f)
	{
		long weapon_index = unit_action_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), true);

		if (weapon_index != NONE &&
			*(short *)(g_4e3b44[UNIT_ACTION_UNIT_GET(weapon_index)->definition_index & 0xffff].bytes + 0x292) == 3)
		{
			function_e68c0(0x3a, unit_index);
		}
	}

	if (state_name)
		*state_name = next_state;
	if (action_name)
		*action_name = next_action;
}

/* clears a unit's actions, back to its idle animations */
// @retail 0xe6f90
void function_e6f90(long unit_index)
{
	function_ec0d0(unit_index, 0x6000086, 0x400000c);

	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));

	actions->active[0] = 0;
	actions->active[1] = 0;
	actions->unknown36 = 0;
}

/* whether a unit can play an animation, from its idle state or (when it
   asks) its alternate one */
// @retail 0xe7020
bool function_e7020(long unit_index, bool *alternate, long name)
{
	if (alternate)
		*alternate = false;
	if (function_10f3b0(unit_index, 0x7000101, name) || function_10f340(unit_index, 0x7000101, name))
		return true;
	if (alternate)
	{
		long first;
		long second;

		if (function_10f630(unit_index, &first, &second) && second == 0x6000085 &&
			(function_10f3b0(unit_index, 0x6000086, name) || function_10f340(unit_index, 0x6000086, name)))
		{
			*alternate = true;
			return true;
		}
	}
	return false;
}

// @retail 0xe70b0
PRIVATE void function_e70b0(long unit_index, short *value)
{
	*value = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index))->unknown36;
}

/* readies the weapon in a unit's hand (the second hand when it's dual
   wielding): the ready animation, unless it's already ready */
// @retail 0xe7110
bool function_e7110(long unit_index, bool secondary, long magazine_index)
{
	long action_type;
	long mode;
	long name;

	if (!secondary)
	{
		action_type = 8;
		mode = 0;
		name = 0x8000002 + (magazine_index != 0);
	}
	else
	{
		action_type = 0x12;
		mode = 3;
		name = 0xd00053a + (magazine_index != 0);
	}

	if (function_110ab0(unit_index))
		return false;

	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);

	if (((unit->flags_134 >> 25) & 1) || UNIT_ACTION_ACTIVE(UNIT_ACTIONS_GET(unit), 0x16))
		return false;
	if (unit_action_active(unit_index, action_type) &&
		function_cbd50(unit_index, unit->weapon_slots[secondary]) != NONE)
	{
		function_e69c0(unit_index, action_type);
	}
	function_e69c0(unit_index, 0x1a);
	function_e69c0(unit_index, 0x1b);

	long weapon_index = function_cbd50(unit_index, (&UNIT_ACTION_UNIT_GET(unit_index)->current_weapon_index)[secondary]);

	if (weapon_index == NONE || !function_101490(weapon_index, magazine_index))
		return false;
	function_c86e0(unit_index, 0);
	if (!function_1029d0(weapon_index, (short)magazine_index))
		function_113e90(unit_index, name, 0.267f, NULL, mode);
	return true;
}

/* readies a weapon (types 0, 1, 10 and 11: a hand's first or second
   magazine) */
// @retail 0xe7280
bool __stdcall function_e7280(long unit_index, s_unit_request *request)
{
	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));
	bool result = false;
	long magazine_index;

	switch (request->type)
	{
	case 0:
	case 10:
		magazine_index = 0;
		break;
	case 1:
	case 11:
		magazine_index = 1;
		break;
	default:
		__assume(0);
	}
	if (function_e7110(unit_index, request->type > 1, magazine_index))
	{
		actions->active[request->type >> 5] |= 1 << (request->type & 0x1f);
		result = true;
	}
	return result;
}

// @retail 0xe7320
bool __stdcall function_e7320(long unit_index, long type)
{
	long weapon_index = unit_action_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), type > 1);
	bool result = false;
	long slot_index;

	switch (type)
	{
	case 0:
	case 10:
		slot_index = 0;
		break;
	case 1:
	case 11:
		slot_index = 1;
		break;
	default:
		__assume(0);
	}
	if (weapon_index != NONE)
		result = function_100880(weapon_index, (short)slot_index);
	return result;
}

// @retail 0xe73c0
void __stdcall function_e73c0(long unit_index, long type)
{
	long weapon_index = unit_action_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), type > 1);

	if (weapon_index != NONE)
		function_1015a0(weapon_index);
}

/* plays the animation of a weapon's request (the ready, put away, firing
   and dual wielding animations) */
// @retail 0xe7440
bool __stdcall function_e7440(long unit_index, s_unit_request *request)
{
	real blend = 0.06675f;
	long mode;

	switch (request->type)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 12:
	case 13:
		blend = 0.0f;
	case 4:
	case 5:
	case 6:
	case 7:
	case 54:
	case 55:
	case 56:
		mode = 0;
		break;
	case 14:
	case 15:
	case 16:
	case 17:
	case 57:
	case 58:
	case 59:
		mode = 3;
		break;
	default:
		__assume(0);
	}

	switch (request->type)
	{
	case 0:
	case 1:
	case 2:
		return function_113e90(unit_index, 0x6000006, blend, NULL, mode);
	case 3:
		return function_113e90(unit_index, 0x6000007, blend, NULL, mode);
	case 4:
		return function_113e90(unit_index, 0x9000004, blend, NULL, mode);
	case 5:
		return function_113e90(unit_index, 0x9000005, blend, NULL, mode);
	case 6:
		return function_113e90(unit_index, 0x9000008, blend, NULL, mode);
	case 7:
		return function_113e90(unit_index, 0x9000009, blend, NULL, mode);
	case 12:
		return function_113e90(unit_index, 0xb00053e, blend, NULL, mode);
	case 13:
		return function_113e90(unit_index, 0xb00053f, blend, NULL, mode);
	case 14:
		return function_113e90(unit_index, 0xe00053c, blend, NULL, mode);
	case 15:
		return function_113e90(unit_index, 0xe00053d, blend, NULL, mode);
	case 16:
		return function_113e90(unit_index, 0xe000540, blend, NULL, mode);
	case 17:
		return function_113e90(unit_index, 0xe000541, blend, NULL, mode);
	case 54:
		return function_113e90(unit_index, 0xb00006d, blend, NULL, mode);
	case 55:
		return function_113e90(unit_index, 0xa000066, blend, NULL, mode);
	case 56:
		return function_113e90(unit_index, 0x8000071, blend, NULL, mode);
	case 57:
		return function_113e90(unit_index, 0x1000067e, blend, NULL, mode);
	case 58:
		return function_113e90(unit_index, 0xf00067f, blend, NULL, mode);
	case 59:
		return function_113e90(unit_index, 0xd000680, blend, NULL, mode);
	default:
		__assume(0);
	}
}

/* the object placement data of 0xb7930/0xb7b40 (the fields set here) */
struct s_unit_action_object_placement
{
	byte unknown00[0x18];
	dword flags;
	point3f position;
	vector3f forward;
	vector3f up;
	byte unknown40[0xc4 - 0x40];
};

/* a grenade type in the game globals (0x2c bytes) */
struct s_unit_action_grenade
{
	byte unknown00[0x28];
	long projectile_tag_index;
};

void function_a7a30(long object_index, dword mask);
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner);
long __stdcall function_b7b40(void *data);
void __stdcall function_b93b0(long parent_index, long object_index, long node_index);
vector3f *function_11d000(vector3f const *v, vector3f *out);
real function_30bf0(vector3f *v);

/* creates the grenade a unit is about to throw, in its hand */
// @retail 0xe7730
void __stdcall function_e7730(long unit_index)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);

	if (unit->current_grenade_index != NONE)
	{
		byte *globals = (byte *)g_4e034c;
		s_unit_action_grenade *grenade = *(long *)(globals + 0x100) ?
			&(*(s_unit_action_grenade **)(globals + 0x104))[unit->current_grenade_index] : NULL;
		bool has_grenade = true;

		if (unit->unknown12c == NONE && !actions->grenade_cancelled)
		{
			if (unit->grenade_counts[unit->current_grenade_index] > 0)
			{
				unit->grenade_counts[unit->current_grenade_index]--;
				function_a7a30(unit_index, 0x400000);
			}
			else
			{
				has_grenade = false;
			}
		}

		if (grenade && has_grenade)
		{
			s_object_marker marker;
			s_effect_owner owner;
			s_unit_action_object_placement data;

			function_b8d30(unit_index, 0x9000100, &marker, 1, false);
			owner.unknown0 = unit->unknown13c;
			owner.unknown4 = unit_index;
			owner.unknown8 = unit->unknown138;
			function_b7930(&data, grenade->projectile_tag_index, unit_index, &owner);
			data.flags |= 4;
			data.forward = UNIT_ACTION_UNIT_GET(unit_index)->field_xe66477;
			function_30bf0(function_11d000(&data.forward, &data.up));
			data.position = marker.matrix.position;

			long projectile_index = function_b7b40(&data);

			if (projectile_index != NONE)
			{
				function_b93b0(unit_index, projectile_index, marker.node_index);
				actions->grenade_projectile_index = projectile_index;
				actions->grenade_type = unit->current_grenade_index;
				actions->grenade_state = 2;
				return;
			}
		}
	}
	actions->grenade_state = 3;
}

/* the grenade throwing parameters in the game globals */
struct s_unit_action_grenade_throw
{
	byte unknown00[0x60];
	real forward_offset;
	real left_offset;
	real up_offset;
};

void function_b9a90(long object_index);
bool function_a76b0(long unit_index, long flag);
void function_cafc0(long unit_index, point3f *position);
void function_a91c0(long unit_index, long projectile_index, point3f const *origin, vector3f const *forward);
void __stdcall function_b8540(long object_index);
void function_d0e60(long unit_index, real amount, real limit);
point3f *function_b9dd0(long object_index, point3f *result);
void function_1ff360(long actor_index, point3f const *target, vector3f *velocity);
void function_b75a0(long object_index, point3f const *point, vector3f const *forward, vector3f const *up,
	struct s_location const *location, bool unknown);
bool function_109e00(long object_index, vector3f *velocity, bool definition_flag_required);
void function_fa820(long projectile_index, vector3f const *impulse);
bool __stdcall function_bc1d0(long object_index, point3f *point);
void __stdcall function_a7870(long object_index);
void function_fd560(long projectile_index, long object_index, long node_index, point3f const *point,
	vector3f const *forward);
extern vector3f *g_4687b0;

/* throws the grenade in a unit's hand (or drops it, or attaches it to the
   unit's vehicle seat) */
// @retail 0xe7900
void __stdcall function_e7900(long unit_index, bool spread, point3f const *origin_override,
	vector3f const *direction_override)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	byte *local_98b918 = g_4e3b44[unit->definition_index & 0xffff].bytes;
	long projectile_index = actions->grenade_projectile_index;

	if (projectile_index != NONE)
	{
		s_unit_action_unit *projectile = UNIT_ACTION_UNIT_GET(projectile_index);
		long marker_name = NONE;
		bool attach = false;
		bool flag17;
		point3f origin;
		vector3f velocity;

		actions->grenade_projectile_index = NONE;
		actions->grenade_type = NONE;
		flag17 = (projectile->object_flags >> 17) & 1;
		projectile->object_flags &= ~0x20000;
		projectile->flags_c0 &= ~2;
		function_b9a90(projectile_index);
		if (flag17)
			projectile->object_flags |= 0x20000;
		else
			projectile->object_flags &= ~0x20000;

		if (unit->parent_object_index != NONE && unit->seat_index != NONE)
		{
			byte *parent_definition = g_4e3b44[UNIT_ACTION_UNIT_GET(unit->parent_object_index)->definition_index & 0xffff].bytes;
			byte *seat = *(byte **)(parent_definition + 0x1cc) + unit->seat_index * 0xb0;

			if ((*(dword *)seat >> 11) & 1)
			{
				attach = true;
				marker_name = *(long *)(seat + 0x10);
			}
		}

		if (function_a76b0(unit_index, 1))
		{
			vector3f forward;

			function_cafc0(unit_index, &origin);
			forward = unit->field_xe66477;
			function_a91c0(unit_index, projectile_index, &origin, &forward);
		}
		else if (!actions->grenade_cancelled)
		{
			if (!attach)
			{
				if (unit->unknown12c != NONE)
				{
					point3f center;

					function_b9dd0(projectile_index, &center);
					function_1ff360(unit->unknown12c, &center, &velocity);
				}
				else
				{
					vector3f direction;

					if (origin_override && direction_override)
					{
						origin = *origin_override;
						direction = *direction_override;
					}
					else
					{
						function_cafc0(unit_index, &origin);
						direction = unit->field_xe66477;
					}

					if (unit->unknown13c != NONE)
					{
						s_unit_action_grenade_throw *throw_globals = *(s_unit_action_grenade_throw **)((byte *)g_4e034c + 0x134);
						vector3f const *up = g_4687b0;
						vector3f left;
						vector3f normal;
						point3f position;

						left.i = up->j * direction.k - up->k * direction.j;
						left.j = up->k * direction.i - up->i * direction.k;
						left.k = up->i * direction.j - up->j * direction.i;
						if (function_30bf0(&left) == 0.0f)
							left = *up;
						normal.i = left.k * direction.j - left.j * direction.k;
						normal.j = direction.k * left.i - left.k * direction.i;
						normal.k = left.j * direction.i - direction.j * left.i;
						function_30bf0(&normal);
						position.x = normal.i * throw_globals->up_offset +
							(left.i * throw_globals->left_offset + (direction.i * throw_globals->forward_offset + origin.x));
						position.y = normal.j * throw_globals->up_offset +
							(left.j * throw_globals->left_offset + (direction.j * throw_globals->forward_offset + origin.y));
						position.z = normal.k * throw_globals->up_offset +
							(left.k * throw_globals->left_offset + (direction.k * throw_globals->forward_offset + origin.z));
						function_b75a0(projectile_index, &position, NULL, NULL, NULL, false);
						spread = false;
					}

					real speed = *(real *)(local_98b918 + 0x1b0);

					velocity.i = speed * direction.i;
					velocity.j = direction.j * speed;
					velocity.k = direction.k * speed;
				}

				if (spread && actions->grenade_throw_maximum > 0)
				{
					real ratio = (real)actions->grenade_throw_count / (real)actions->grenade_throw_maximum;

					if (1.0f > ratio)
					{
						real scale = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, 0.6f, 1.4f);

						velocity.i = unit->field_xe66477.i * scale * (1.0f - ratio) + velocity.i * ratio;
						velocity.j = unit->field_xe66477.j * scale * (1.0f - ratio) + velocity.j * ratio;
						velocity.k = unit->field_xe66477.k * scale * (1.0f - ratio) + velocity.k * ratio;
					}
				}

				vector3f parent_velocity;

				if (function_109e00(unit_index, &parent_velocity, true))
				{
					velocity.i += parent_velocity.i;
					velocity.j += parent_velocity.j;
					velocity.k += parent_velocity.k;
				}
				velocity.i -= projectile->linear_velocity.i;
				velocity.j -= projectile->linear_velocity.j;
				velocity.k -= projectile->linear_velocity.k;
				function_fa820(projectile_index, &velocity);
				function_cafc0(unit_index, &origin);
				if (!function_bc1d0(unit_index, &origin))
				{
					function_b8540(projectile_index);
					function_d0e60(unit_index, 0, 0);
					actions->grenade_state = 3;
					return;
				}
			}

			function_a7870(projectile_index);
			if (attach)
			{
				s_object_marker marker;

				function_b8d30(unit->parent_object_index, marker_name, &marker, 1, false);
				*(dword *)((byte *)UNIT_ACTION_UNIT_GET(projectile_index) + 0x12c) |= 0x400;
				function_fd560(projectile_index, unit->parent_object_index, marker.node_index, &marker.matrix.position, NULL);
			}
			function_d0e60(unit_index, 0, 0);
			actions->grenade_state = 3;
			return;
		}
		function_b8540(projectile_index);
		function_d0e60(unit_index, 0, 0);
	}
	actions->grenade_state = 3;
}

/* the request to throw a grenade (type 22) */
struct s_unit_request_throw_grenade
{
	long type;
	bool cancel;
	bool immediate;
	byte unknown06[2];
	point3f origin;
	vector3f direction;
};

bool function_101440(long weapon_index);
short function_cdff0(long unit_index, short grenade_type);
bool function_ee8a0(long unit_index, long field_x11c898);
long unit_get_player_index(long unit_index);
void function_2007b3(long arg_0, long arg_1, long arg_2, long arg_3);
long function_1469f0(real seconds);
void function_edff0(long unit_index);
void function_ee7f0(long unit_index, long type);
long function_176780(long object_index, s_effect_owner const *owner, real scale_a, long tag_index, real scale_b,
	point3f const *origin, vector3f const *direction);
bool function_bbe90(long tag_index);
void __stdcall function_a8c10(long unit_index);

/* the grenade data of a unit's current grenade type */
PRIVATE inline s_unit_action_grenade *unit_action_grenade_get(s_unit_action_unit *unit)
{
	byte *globals = (byte *)g_4e034c;

	return *(long *)(globals + 0x100) ?
		&(*(s_unit_action_grenade **)(globals + 0x104))[unit->current_grenade_index] : NULL;
}

/* a unit's vehicle seat definition */
PRIVATE inline byte *unit_action_seat_get(s_unit_action_unit *unit)
{
	byte *parent_definition = g_4e3b44[UNIT_ACTION_UNIT_GET(unit->parent_object_index)->definition_index & 0xffff].bytes;

	return *(byte **)(parent_definition + 0x1cc) + unit->seat_index * 0xb0;
}

/* starts throwing a grenade (type 22) */
// @retail 0xe7fb0
bool __stdcall function_e7fb0(long unit_index, s_unit_request *request)
{
	s_unit_request_throw_grenade *throw_request = (s_unit_request_throw_grenade *)request;
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	long weapon_index = unit_action_weapon_get(unit, false);
	s_unit_action_grenade *grenade = NULL;
	bool result = false;

	if (unit->current_grenade_index != NONE)
		grenade = unit_action_grenade_get(unit);

	if (unit->object_type != 0 || function_101440(weapon_index))
		return result;
	if ((function_110ab0(unit_index) || ((UNIT_ACTION_UNIT_GET(unit_index)->flags_134 >> 25) & 1)) &&
		!throw_request->immediate)
	{
		return result;
	}

	bool animate = true;

	if (unit->parent_object_index != NONE && unit->seat_index != NONE)
	{
		byte *seat = unit_action_seat_get(unit);
		dword seat_flags = *(dword *)seat;

		if (!((seat_flags >> 5) & 1))
			animate = false;
		if ((seat_flags >> 11) & 1)
		{
			long marker_name = *(long *)(seat + 0x10);
			s_object_marker marker;

			if (!marker_name || function_b8d30(unit->parent_object_index, marker_name, &marker, 1, false) != 1)
				return result;
		}
	}

	if (!grenade || function_cdff0(unit_index, UNIT_ACTION_UNIT_GET(unit_index)->current_grenade_index) <= 0)
	{
		if (animate && unit_get_player_index(unit_index) != NONE &&
			*(short *)(g_4e8c24->data + (unit_get_player_index(unit_index) & 0xffff) * 0x21c + 0x28) != NONE)
		{
			function_2007b3(*(short *)(g_4e8c24->data + (unit_get_player_index(unit_index) & 0xffff) * 0x21c + 0x28), 0, 1, NONE);
		}
		return result;
	}

	if (!throw_request->immediate && UNIT_ACTION_ACTIVE(actions, 0x16))
		return result;

	c_animation_channel *channel = NULL;

	animate = function_ee8a0(unit_index, 0);
	function_e69c0(unit_index, 0x16);
	function_e69c0(unit_index, 8);
	function_e69c0(unit_index, 0x12);
	function_e69c0(unit_index, 0);
	function_e69c0(unit_index, 0xa);
	function_e69c0(unit_index, 0x1b);
	function_e68c0(0x13, unit_index);

	if (throw_request->immediate)
	{
		actions->grenade_state = 1;
		actions->grenade_throw_count = 0;
		actions->grenade_cancelled = false;
		function_e7730(unit_index);
		function_e7900(unit_index, false, &throw_request->origin, &throw_request->direction);
		return result;
	}

	if (!(animate && function_113e90(unit_index, 0x1000006c, 0.1335f, &channel, 0)) &&
		!function_113e90(unit_index, 0xd000021, 0.1335f, &channel, 0))
	{
		return result;
	}

	actions->grenade_state = 1;
	actions->grenade_throw_count = 0;
	actions->grenade_throw_maximum = 0;
	actions->grenade_cancelled = throw_request->cancel;

	real event_time = channel->get_event_time();

	if (event_time >= 0.0f)
		actions->grenade_throw_maximum = (char)function_1469f0(event_time);
	if (unit->parent_object_index == NONE)
	{
		function_edff0(unit_index);
		function_ee7f0(unit_index, 0x1a + (animate != false));
		function_c86e0(unit_index, 0);
	}
	if (*(long *)((byte *)grenade + 8) != NONE)
		function_176780(unit_index, NULL, 0.0f, *(long *)((byte *)grenade + 8), 0.0f, NULL, NULL);
	if (*(long *)((byte *)grenade + 0x20) != NONE)
		function_bbe90(*(long *)((byte *)grenade + 0x20));
	function_a8c10(unit_index);
	actions->active[0] |= 0x400000;
	result = true;
	return result;
}

bool function_114040(long unit_index, long name);
bool __stdcall function_100130(long weapon_index, bool immediate);
void __stdcall function_104080(long weapon_index);
void function_10cd50(long weapon_index);

/* the grenade throw: into the hand, then released, then waiting for the
   throw animation to end */
// @retail 0xe8380
bool __stdcall unit_action_throw_grenade_update(long unit_index, long type)
{
	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));
	char count = ++actions->grenade_throw_count;

	switch (actions->grenade_state)
	{
	case 1:
		if (count >= 2)
			function_e7730(unit_index);
		break;
	case 2:
		if (count >= actions->grenade_throw_maximum)
			function_e7900(unit_index, false, NULL, NULL);
		break;
	case 3:
		if (!function_114040(unit_index, 0xd000021) && !function_114040(unit_index, 0x1000006c))
		{
			actions->grenade_state = 0;
			return false;
		}
		break;
	}
	return true;
}

/* a grenade throw interrupted: the grenade is dropped */
// @retail 0xe8420
void __stdcall unit_action_throw_grenade_interrupted(long unit_index, long type)
{
	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));

	function_e7900(unit_index, true, NULL, NULL);
	actions->grenade_state = 0;
}

/* puts away the weapon in a unit's hand */
// @retail 0xe8460
void function_e8460(long unit_index, bool primary)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	char index = (&unit->current_weapon_index)[!primary];

	if (index != NONE)
	{
		long weapon_index = unit->weapon_object_indices[index];

		if (weapon_index != NONE)
		{
			s_unit_action_unit *current = UNIT_ACTION_UNIT_GET(unit_index);

			function_1015a0(weapon_index);
			function_100130(weapon_index, true);
			*(short *)((byte *)UNIT_ACTION_UNIT_GET(weapon_index) + 0x16e) = 0;
			function_104080(weapon_index);
			function_10cd50(weapon_index);
			(&current->current_weapon_index)[!primary] = NONE;
			function_ee7f0(unit_index, primary ? 0x18 : 0x19);
		}
	}
}

/* starts putting away the weapon in a unit's hand: its put away animation,
   or at once */
// @retail 0xe8510
bool function_e8510(long unit_index, bool immediate, bool silent, bool primary)
{
	long weapon_index = unit_action_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), !primary);
	bool result = true;

	if (weapon_index != NONE)
	{
		long name = primary ? 0x8000025 : 0xd000539;

		if (function_114040(unit_index, name) && !immediate)
			return false;
		if (!(silent && immediate))
		{
			if (function_100130(weapon_index, immediate))
			{
				function_c86e0(unit_index, 0);
				if (!silent)
					function_ee7f0(unit_index, primary ? 0x12 : 0x13);
				if (function_113e90(unit_index, name, 0.267f, NULL, primary ? 0 : 3))
				{
					result = immediate;
					if (!immediate)
						return result;
				}
			}
		}
		function_e8460(unit_index, primary);
	}
	return result;
}

bool function_10fd40(long unit_index, long action_name, long state_name, bool flag);
byte function_10fcd0(long unit_index, long unknown, long state_name, long action_name);
void __stdcall function_d0870(long weapon_index, long unit_index, bool secondary);
void __stdcall function_fff40(long a, long b);

/* a unit's weapon animations: its vehicle seat's weapon's, or the
   default */
// @retail 0xe8620
void __stdcall function_e8620(long unit_index, bool flag)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	long state_name = 0x7000001;
	long action_name = 0x7000001;

	if (unit->parent_object_index != NONE && unit->seat_index != NONE)
	{
		s_unit_action_unit *parent = UNIT_ACTION_UNIT_GET(unit->parent_object_index);
		dword seat_flags = *(dword *)(*(byte **)(g_4e3b44[parent->definition_index & 0xffff].bytes + 0x1cc) +
			unit->seat_index * 0xb0);
		long weapon_index = unit_action_weapon_get(parent, false);

		if (((seat_flags >> 3) & 1) && !((seat_flags >> 2) & 1) && weapon_index != NONE)
		{
			byte *s_type_67e06b = g_4e3b44[UNIT_ACTION_UNIT_GET(weapon_index)->definition_index & 0xffff].bytes;

			action_name = *(long *)(s_type_67e06b + 0x28c);
			state_name = *(long *)(s_type_67e06b + 0x288);
		}
	}
	function_10fd40(unit_index, action_name, state_name, flag);
}

/* takes out the weapon in a unit's slot for a hand: its animations and
   ready animation */
// @retail 0xe8720
void __stdcall function_e8720(long unit_index, long unknown, bool immediate, bool silent, bool primary)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	long hand = !primary;
	long other_weapon_index = NONE;
	char other_slot = UNIT_ACTION_UNIT_GET(unit_index)->weapon_slots[primary != false];
	bool has_other;
	bool is_vehicle;

	if (other_slot != NONE)
		other_weapon_index = UNIT_ACTION_UNIT_GET(unit_index)->weapon_object_indices[other_slot];
	has_other = other_weapon_index != NONE;
	is_vehicle = ((1 << unit->object_type) >> 1) & 1;

	long slot = unit->weapon_slots[hand];

	if (slot != NONE)
	{
		long i;

		for (i = 0; i < 2; i++)
		{
			if ((&unit->current_weapon_index)[i] == slot)
				break;
		}
		if (i == 2)
		{
			long weapon_index = unit->weapon_object_indices[slot];

			if (weapon_index != NONE)
			{
				byte *s_type_67e06b = g_4e3b44[UNIT_ACTION_UNIT_GET(weapon_index)->definition_index & 0xffff].bytes;
				long state_name = *(long *)(s_type_67e06b + 0x288);

				if (has_other)
					state_name = 0x400054b;
				if (function_10fcd0(unit_index, unknown, state_name, *(long *)(s_type_67e06b + 0x28c)) || is_vehicle)
				{
					function_10fd40(unit_index, *(long *)(s_type_67e06b + 0x28c), state_name, primary);
				}
				else if (unit->seat_index == NONE)
				{
					function_e8620(unit_index, primary);
					return;
				}

				function_d0870(weapon_index, unit_index, hand != 0);
				(&unit->current_weapon_index)[hand] = (char)slot;
				unit->weapon_ready_times[slot] = g_510c54->game_time;

				bool animate = function_ee8a0(unit_index, primary != false);

				function_fff40((long)silent, (long)immediate);
				if (!silent)
				{
					long type = primary ? 0x14 : 0x15;

					if (animate)
						type = 0x16;
					function_ee7f0(unit_index, type);
				}
				function_c86e0(unit_index, 0);
				if (immediate || has_other && primary)
					return;
				if (animate &&
					function_113e90(unit_index, primary ? 0x1000006e : 0x15000538, 0.267f, NULL, primary ? 0 : 3))
				{
					return;
				}
				function_113e90(unit_index, primary ? 0x5000024 : 0xa000537, 0.267f, NULL, primary ? 0 : 3);
				return;
			}
		}
	}
	function_e8620(unit_index, primary);
}

/* the request to switch weapons (types 8 and 18) */
struct s_unit_request_weapon_switch
{
	long type;
	bool immediate;
	bool silent;
	byte unknown06[0x20 - 0x6];
};

bool __stdcall function_cd6a0(long unit_index, long unknown, long weapon_index);
bool function_1140b0(long unit_index, long name);
void function_105fa0(long weapon_index, long unknown);
bool function_100350(long weapon_index);

/* switches the weapon in a hand (types 8 and 18: the first and second
   hand) */
// @retail 0xe8980
bool __stdcall unit_action_weapon_switch(long unit_index, s_unit_request *request)
{
	s_unit_request_weapon_switch *switch_request = (s_unit_request_weapon_switch *)request;
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	bool primary = request->type == 8;
	long hand = !primary;

	if (!switch_request->immediate)
	{
		if (unit->weapon_slots[hand] == (&unit->current_weapon_index)[hand])
			return false;
		if (function_110ab0(unit_index) || ((UNIT_ACTION_UNIT_GET(unit_index)->flags_134 >> 25) & 1) ||
			unit_action_active(unit_index, 0x16))
		{
			return false;
		}
	}

	s_unit_action_unit *current = UNIT_ACTION_UNIT_GET(unit_index);
	long weapon_index = NONE;

	if (current->weapon_slots[hand] != NONE)
		weapon_index = current->weapon_object_indices[current->weapon_slots[hand]];
	if (unit->weapon_slots[hand] != NONE && (weapon_index == NONE || !function_cd6a0(unit_index, NONE, weapon_index)))
		return false;

	function_e69c0(unit_index, 0);
	function_e69c0(unit_index, 0xa);
	function_e69c0(unit_index, 0x1b);
	function_e69c0(unit_index, 0x16);
	if (function_e8510(unit_index, switch_request->immediate, switch_request->silent, primary))
	{
		function_e8720(unit_index, 0x7000101, switch_request->silent, switch_request->silent, primary);
		return true;
	}

	long type = primary ? 8 : 0x12;

	actions->active[type >> 5] |= 1 << (type & 0x1f);
	return true;
}

/* a weapon switch waiting for the old weapon to be put away */
// @retail 0xe8b20
bool __stdcall function_e8b20(long unit_index, long type)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	bool primary = type == 8;
	long hand = !primary;
	bool result = true;

	if (unit->weapon_slots[hand] == (&unit->current_weapon_index)[hand])
	{
		if (function_1140b0(unit_index, primary ? 0x8000025 : 0xd000539))
		{
			long weapon_index = unit_action_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), hand != 0);

			if (weapon_index != NONE)
			{
				function_105fa0(weapon_index, 0);
				function_100350(weapon_index);
			}
		}
		return false;
	}
	if (!function_110ab0(unit_index) && function_e8510(unit_index, false, false, primary))
	{
		function_e8720(unit_index, 0x7000101, false, false, primary);
		return false;
	}
	return result;
}

/* a weapon switch interrupted: the old weapon is put away at once and the
   new one taken out */
// @retail 0xe8c10
void __stdcall unit_action_weapon_switch_interrupted(long unit_index, long type)
{
	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));
	bool primary = type == 8;

	if (UNIT_ACTION_ACTIVE(actions, type))
	{
		function_e8510(unit_index, true, false, primary);
		function_e8720(unit_index, 0x7000101, false, false, primary);
	}
}

/* a weapon switch finished */
// @retail 0xe8d10
void __stdcall unit_action_weapon_switch_finished(long unit_index, long type)
{
	bool primary = type == 8;

	function_e8510(unit_index, true, false, primary);
	function_e8720(unit_index, 0x7000101, false, false, primary);
}

struct s_object;
bool function_cd660(long unit_index);
void function_101740(long weapon_index, long other_index);
void function_ce920(long unit_index, long slot_index, long mode, bool flag);
s_object *function_badc0(long object_index, dword type_mask);
void __stdcall function_191f3a(long definition_index, long value);

/* a player's short at +0x28 (the players data, 0x21c bytes each) */
PRIVATE inline short unit_action_player_value28_get(long player_index)
{
	return *(short *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x28);
}

/* drops the weapon in a hand (types 9 and 19) */
// @retail 0xe8de0
bool __stdcall unit_action_drop_weapon(long unit_index, s_unit_request *request)
{
	bool primary = request->type == 9;
	long hand = !primary;
	long slot = (&UNIT_ACTION_UNIT_GET(unit_index)->current_weapon_index)[hand];
	long weapon_index = unit_action_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), hand != 0);
	long mode;
	bool flag;

	if (weapon_index == NONE)
		return false;
	if (!primary)
	{
		mode = 3;
		flag = primary;
	}
	else if (function_cd660(unit_index))
	{
		mode = 2;
		flag = !*((bool *)request + 4);
	}
	else
	{
		mode = 0;
		flag = !*((bool *)request + 4);
	}

	long other_index = unit_action_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), primary != false);

	if (other_index != NONE &&
		UNIT_ACTION_UNIT_GET(weapon_index)->definition_index == UNIT_ACTION_UNIT_GET(other_index)->definition_index)
	{
		function_101740(weapon_index, other_index);
	}
	function_e69c0(unit_index, 8);
	function_e69c0(unit_index, 0x12);
	function_e69c0(unit_index, 0);
	function_e69c0(unit_index, 0xa);
	function_e69c0(unit_index, 0x1b);
	function_e69c0(unit_index, 0x16);
	function_e8460(unit_index, primary);
	function_ce920(unit_index, slot, mode, flag);
	function_e8720(unit_index, 0x7000101, false, false, primary);

	byte *object = (byte *)function_badc0(unit_index, 3);

	if (object && *(long *)(object + 0x13c) != NONE)
	{
		byte *current = (byte *)function_badc0(unit_index, 3);
		long player_index = current ? *(long *)(current + 0x13c) : NONE;
		short value = unit_action_player_value28_get(player_index);

		if (value != NONE)
			function_191f3a(UNIT_ACTION_UNIT_GET(weapon_index)->definition_index, value);
	}
	return true;
}

/* the request to pick up a weapon (type 20) */
struct s_unit_request_pickup_weapon
{
	long type;
	long weapon_index;
	short mode;
	byte unknown0a[0x20 - 0xa];
};

bool __stdcall function_cd7b0(long unit_index, long weapon_index, bool *modes);
bool __stdcall function_cd0c0(long unit_index, long weapon_index, short mode);
void __stdcall function_191fab(long definition_index, long value);
void __stdcall function_1ca260(long actor_index, long player_index, long weapon_index, long other_weapon_index);

/* picks up a weapon (type 20), in the way the request's mode asks */
// @retail 0xe9000
bool __stdcall unit_action_pickup_weapon(long unit_index, s_unit_request *request)
{
	s_unit_request_pickup_weapon *pickup = (s_unit_request_pickup_weapon *)request;
	s_unit_action_unit *weapon = UNIT_ACTION_UNIT_GET(pickup->weapon_index);
	bool result = false;
	bool modes[4];

	if (*(long *)((byte *)weapon + 0x154) != NONE)
		return result;
	if (!function_cd7b0(unit_index, pickup->weapon_index, modes))
		return result;

	switch (pickup->mode)
	{
	case 0:
	case 1:
		if (!modes[0] || function_cbd50(unit_index, UNIT_ACTION_UNIT_GET(unit_index)->current_weapon_index) != NONE)
			return result;
		break;
	case 3:
		if (!modes[0])
			return result;
		break;
	case 4:
		if (!modes[2])
			return result;
		break;
	case 5:
		if (!modes[1])
			return result;
		break;
	case 6:
		if (!modes[3])
			return result;
		break;
	default:
		return result;
	}

	function_e69c0(unit_index, 8);
	function_e69c0(unit_index, 0x12);
	function_e69c0(unit_index, 0);
	function_e69c0(unit_index, 0xa);
	function_e69c0(unit_index, 0x1b);
	function_e69c0(unit_index, 0x16);
	if (function_cd0c0(unit_index, pickup->weapon_index, pickup->mode))
	{
		long value = unit_get_player_index(unit_index) == NONE ? NONE :
			unit_action_player_value28_get(unit_get_player_index(unit_index));

		function_191fab(weapon->definition_index, value);
		function_c86e0(unit_index, 0);
		result = true;
	}
	return result;
}

/* gives a unit's weapon to a player's unit, taking the player's weapon
   in exchange (type 21) */
// @retail 0xe9190
bool __stdcall function_e9190(long unit_index, s_unit_request *request)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	long player_index = *(long *)((byte *)request + 4);

	if (unit->unknown13c != NONE)
		return false;

	long player_unit_index = *(long *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x2c);
	long weapon_index = unit_action_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), false);
	long other_weapon_index = unit_action_weapon_get(UNIT_ACTION_UNIT_GET(player_unit_index), false);

	if (weapon_index == NONE)
		return false;

	s_unit_request drop;

	drop.type = 9;
	*((bool *)&drop + 4) = true;
	unit_action_drop_weapon(unit_index, &drop);

	bool given = function_cd0c0(player_unit_index, weapon_index, 4);

	if (!given)
	{
		function_cd0c0(unit_index, weapon_index, 3);
		return given;
	}

	bool taken = false;

	if (other_weapon_index != NONE)
		taken = function_cd0c0(unit_index, other_weapon_index, 3);
	if (unit->unknown12c != NONE)
	{
		if (taken)
			function_1ca260(unit->unknown12c, *(long *)((byte *)request + 4), weapon_index, other_weapon_index);
		else
			function_1ca260(unit->unknown12c, *(long *)((byte *)request + 4), weapon_index, NONE);
	}
	return given;
}

/* plays an animation on a unit and on its child units */
// @retail 0xe92e0
bool __stdcall function_e92e0(long object_index, long name)
{
	bool result = function_113e90(object_index, name, 0.0f, NULL, 0);

	for (long child_index = *(long *)((byte *)UNIT_ACTION_UNIT_GET(object_index) + 0x10); child_index != NONE; )
	{
		s_unit_action_header *header = UNIT_ACTION_HEADER_GET(child_index);
		s_unit_action_unit *child = header->unit;

		if (((child->object_flags >> 26) & 1) && ((1 << header->type) & 3) && function_e92e0(child_index, name))
			result = true;
		child_index = *(long *)((byte *)child + 0xc);
	}
	return result;
}

/* a unit's animation state */
PRIVATE inline s_animation_state *unit_action_state_get(long unit_index)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);

	return (s_animation_state *)((byte *)unit + unit->animation_state_offset);
}

/* whether a channel is playing an animation */
PRIVATE inline bool unit_action_channel_valid(c_animation_channel const *channel)
{
	return channel->graph_tag_index != NONE && channel->animation_id.index != NONE;
}

/* plays a pausing animation on a unit (and its child units), then stops
   it, or holds or releases its channels */
PRIVATE inline bool unit_action_pause(long unit_index, s_unit_request *request, long name)
{
	bool result = false;

	if (!function_e92e0(unit_index, name))
		return result;

	result = true;
	if (*((byte *)request + 4) == 1)
	{
		s_animation_state *state = unit_action_state_get(unit_index);

		if (state->unknown7c == name)
			state->channels_finish();
		for (long child_index = *(long *)((byte *)UNIT_ACTION_UNIT_GET(unit_index) + 0x10); child_index != NONE; )
		{
			s_animation_state *child_state = unit_action_state_get(child_index);

			if (child_state->unknown7c == name)
				child_state->channels_finish();
			child_index = *(long *)((byte *)UNIT_ACTION_UNIT_GET(child_index) + 0xc);
		}
		return result;
	}

	s_animation_state *state = unit_action_state_get(unit_index);

	if (*((byte *)request + 5) == 1)
	{
		if (unit_action_channel_valid(&state->channels[2]) && TEST_FIELD_BIT(state->channels[2].flag0))
			state->channels[2].unknown11 |= 1;
		if (unit_action_channel_valid(&state->channels[0]) && TEST_FIELD_BIT(state->channels[0].flag0))
			state->channels[0].unknown11 |= 1;
		if (unit_action_channel_valid(&state->channels[1]) && TEST_FIELD_BIT(state->channels[1].flag0))
			state->channels[1].unknown11 |= 1;
	}
	else
	{
		if (unit_action_channel_valid(&state->channels[2]) && TEST_FIELD_BIT(state->channels[2].flag0))
			state->channels[2].unknown11 &= ~1;
		if (!unit_action_channel_valid(&state->channels[2]))
		{
			if (unit_action_channel_valid(&state->channels[0]) && TEST_FIELD_BIT(state->channels[0].flag0))
				state->channels[0].unknown11 &= ~1;
			if (unit_action_channel_valid(&state->channels[1]) && TEST_FIELD_BIT(state->channels[1].flag0))
				state->channels[1].unknown11 &= ~1;
		}
	}
	return result;
}

/* pauses a unit's animation (type 23) */
// @retail 0xe9370
bool __stdcall function_e9370(long unit_index, s_unit_request *request)
{
	return unit_action_pause(unit_index, request, 0x700005c);
}

/* pauses a unit's animation (type 24) */
// @retail 0xe9500
bool __stdcall function_e9500(long unit_index, s_unit_request *request)
{
	return unit_action_pause(unit_index, request, 0x700005d);
}

bool __stdcall function_10f430(long unit_index, long field_7c, long state_name, long weapon_name, long action_name,
	real blend, bool flags, long mode);
void function_edfa0(long unit_index, point2f const *facing);
void function_ba3d0(long unit_index);

/* the request to play an animation impulse (type 25) */
struct s_unit_request_impulse
{
	long type;
	char blend;
	byte unknown05[3];
	long name;
	bool face;
	byte unknown0d[3];
	vector3f facing;
};

/* plays an animation impulse (type 25), from the unit's idle state (or its
   alternate one) */
// @retail 0xe9690
bool __stdcall function_e9690(long unit_index, s_unit_request *request)
{
	s_unit_request_impulse *impulse = (s_unit_request_impulse *)request;
	bool result = false;
	bool alternate = false;

	if (!function_e7020(unit_index, &alternate, impulse->name))
		return result;

	real blend = 0.0f;

	switch (impulse->blend)
	{
	case 1:
		blend = 0.1f;
		break;
	case 2:
		blend = 0.2f;
		break;
	}
	if (alternate)
		function_10f430(unit_index, 0x6000086, 0x7000101, 0x7000101, 0x400000c, blend, 0, 0);
	if (!function_113e90(unit_index, impulse->name, blend, NULL, 0))
		return result;

	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);

	*((byte *)unit + *(short *)((byte *)unit + 0x33e)) |= 1;
	if (impulse->face && unit->parent_object_index == NONE)
		function_edfa0(unit_index, (point2f const *)&impulse->facing);
	return true;
}

/* plays an animation on a unit, facing it the way the animation asks */
// @retail 0xe9780
bool function_e9780(long unit_index, long name, bool keep, vector3f const *facing, c_animation_channel **channel_out)
{
	c_animation_channel *channel = NULL;
	bool result = false;

	if (function_113e90(unit_index, name, 0.0f, &channel, 0))
	{
		s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
		s_animation *animation = NULL;

		if (!keep)
			function_ba3d0(unit_index);
		if (channel->animation_id.index != NONE)
		{
			animation = function_1daea0((s_graph_tag *)g_4e3b44[channel->graph_tag_index & 0xffff].bytes,
				channel->animation_id);
		}
		if (!animation->type && unit->parent_object_index == NONE)
		{
			if (facing)
				function_edfa0(unit_index, (point2f const *)facing);
			else
				function_edff0(unit_index);
		}
		result = true;
	}
	if (channel_out)
		*channel_out = channel;
	return result;
}

/* the request to melee (type 26) */
struct s_unit_request_melee
{
	long type;
	short melee_type;
	bool face;
	byte unknown07;
	vector3f facing;
	byte unknown14[0x20 - 0x14];
};

long function_10f5f0(long object_index);
bool function_e4050(long object_index);

/* a melee attack (type 26): the animation for the unit's state and the
   melee type */
// @retail 0xe9830
bool __stdcall unit_action_melee(long unit_index, s_unit_request *request)
{
	s_unit_request_melee *melee = (s_unit_request_melee *)request;
	bool result = false;

	if (function_110ab0(unit_index))
		return result;

	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);

	if ((unit->flags_134 >> 25) & 1)
		return result;

	byte *definition = g_4e3b44[unit->definition_index & 0xffff].bytes;
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	long state_name = function_10f5f0(unit_index);
	long name = NONE;
	bool airborne = false;

	if (unit->object_type == 0)
		airborne = function_e4050(unit_index);
	if (state_name == 0xd00003f)
	{
		name = 0xa000040;
	}
	else if (airborne || state_name == 0x800001e)
	{
		name = 0xe000038;
	}
	else
	{
		switch (melee->melee_type)
		{
		case 0:
			name = 0x500000a;
			break;
		case 1:
			name = 0xa0005b9;
			break;
		case 2:
			name = 0xe00067d;
			break;
		case 3:
			name = 0xc0006cd;
			break;
		}
	}

	if (!function_e9780(unit_index, name, true, melee->face ? &melee->facing : NULL, NULL))
		return false;
	if ((*(dword *)(definition + 0xbc) >> 8) & 1)
		function_10f430(unit_index, 0x7000101, 0x7000101, 0x7000101, 0xc000043, 0.0f, 0, 2);
	actions->active[0] |= 0x4000000;
	return true;
}

/* a first person melee animation (0x4407f4, 12 bytes each) */
struct s_unit_action_melee_animation
{
	long name;
	long index;
	long first_person_name;
};

s_unit_action_melee_animation const g_4407f4[4] =
{
	{ 0xe000607, 0x21, 0xe000607 },
	{ 0xe000608, 0x22, 0xe000608 },
	{ 0xe000609, 0x23, 0xe000609 },
	{ 0xe00060a, 0x24, 0xe00060a },
};

s_animation const *first_person_weapon_animation_get(long weapon_index, long animation_name, long *frame_out);

/* a random melee animation of a weapon's first person animations */
// @retail 0xe99a0
s_unit_action_melee_animation const *__stdcall function_e99a0(long weapon_index)
{
	long indices[4];
	short count = 0;
	long i;

	for (i = 0; i < 4; i++)
	{
		if (first_person_weapon_animation_get(weapon_index, g_4407f4[i].first_person_name, NULL))
			indices[count++] = i;
	}
	if (count > 0)
		return &g_4407f4[indices[random_index(&g_4e7408->unknown0, count)]];
	return NULL;
}

void function_dee60(long unit_index, long target_index, bool flag);
s_object *function_bae20(long object_index, dword type_mask);

/* a float rounded to an integer (fld, fistp) */
__forceinline long unit_action_round_long(real value)
{
	long result;

	__asm
	{
		fld value
		fistp result
	}
	return result;
}

__forceinline char unit_action_round(real value)
{
	return (char)unit_action_round_long(value);
}

/* starts a melee attack: the animation for the attack's mode (or the next
   in a combo), its damage frames, and the lunge */
// @retail 0xe9a20
bool __stdcall function_e9a20(long unit_index, long mode, long target_index, byte flags)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	long weapon_index = unit_action_weapon_get(unit, false);
	bool is_biped = (byte)(1 << unit->object_type) & 1;
	bool result = false;
	long name;
	long type;
	long first_person_name = NONE;

	if (UNIT_ACTION_ACTIVE(UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index)), 0x1b))
	{
		long current = actions->melee_name;

		switch (current)
		{
		case 0xc000073:
			if (mode == 1)
			{
				name = 0xc000075;
				type = 0x1e;
				first_person_name = name;
			}
			else
			{
				name = 0x11000074;
				type = 0x1d;
			}
			goto play;
		case 0xa0005bb:
			name = 0xb0005b2;
			type = 0x26;
			first_person_name = name;
			goto play;
		case 0xc000075:
			if (mode == 1)
			{
				name = 0xc000077;
				type = 0x20;
				first_person_name = name;
			}
			else
			{
				name = 0x11000076;
				type = 0x1f;
			}
			goto play;
		case 0x130005bc:
			name = 0x140005b3;
			type = 0x28;
			first_person_name = name;
			goto play;
		case NONE:
		case 0xe000607:
		case 0xe000608:
		case 0xe000609:
		case 0xe00060a:
			break;
		default:
			return result;
		}
	}

	if (mode == 2)
	{
		if (is_biped && function_e4050(unit_index))
		{
			name = 0x130005bc;
			type = 0x27;
		}
		else
		{
			name = 0xa0005bb;
			type = 0x25;
		}
		first_person_name = name;
	}
	else if (mode == 1)
	{
		if (weapon_index != NONE)
		{
			s_unit_action_melee_animation const *animation = function_e99a0(weapon_index);

			if (animation)
			{
				name = animation->name;
				type = animation->index;
				first_person_name = animation->first_person_name;
				if (name != NONE)
					goto play;
			}
			if (first_person_weapon_animation_get(weapon_index, 0xc000073, NULL))
			{
				name = 0xc000073;
				type = 0x1c;
				first_person_name = name;
				goto play;
			}
		}
		name = 0x500000a;
		type = 0x11;
		first_person_name = name;
	}
	else
	{
		return result;
	}

play:
	c_animation_channel *channel = NULL;

	if (!function_e9780(unit_index, name, false, NULL, &channel))
	{
		if (name == 0xc000073 || name == 0xc000075 || name == 0xc000077 || name > 0xe000607 && name <= 0xe00060a)
		{
			if (!function_e9780(unit_index, 0xe000607, false, NULL, &channel))
				function_e9780(unit_index, 0x500000a, false, NULL, &channel);
		}
		else if (name == 0xe000607)
		{
			function_e9780(unit_index, 0x500000a, false, NULL, &channel);
		}
	}
	function_ee7f0(unit_index, type);

	if (name != 0xa0005bb && name != 0x130005bc)
	{
		s_animation const *animation = NULL;

		if (weapon_index != NONE && first_person_name != NONE)
			animation = first_person_weapon_animation_get(weapon_index, first_person_name, NULL);
		if (!animation)
		{
			if (!channel)
				return result;
			animation = channel->function_1c6440();
			if (!animation)
				return result;
		}
		if (is_biped && target_index != NONE)
			function_dee60(unit_index, target_index, false);

		long damage_frame = function_1dae20(animation);
		long end_frame = function_1dae50(animation);
		long frame_count = animation->frame_count;

		if (damage_frame != NONE)
			actions->melee_damage_ticks = (char)function_1469f0(damage_frame * 0.033333335f);
		else
			actions->melee_damage_ticks = NONE;
		if (end_frame != NONE)
			actions->melee_end_ticks = (char)function_1469f0(end_frame * 0.033333335f);
		else
			actions->melee_end_ticks = NONE;
		actions->melee_ticks = unit_action_round(frame_count * 0.033333335f * (real)g_510c54->field_2_3);
	}
	else
	{
		if (is_biped)
		{
			long lunge_target_index = *(long *)((byte *)unit + 0x1c8);

			if (!(*(real *)((byte *)unit + 0x1e4) >= 1.0f && lunge_target_index != NONE &&
				function_bae20(lunge_target_index, 3)))
			{
				lunge_target_index = target_index;
			}
			if (lunge_target_index != NONE)
				function_dee60(unit_index, lunge_target_index, true);
		}
		actions->melee_damage_ticks = NONE;
		actions->melee_end_ticks = NONE;
		actions->melee_ticks = unit_action_round((real)g_510c54->field_2_3 * 2.0f);
	}

	actions->active[0] |= 0x8000000;
	actions->melee_name = name;
	actions->melee_flags = flags;
	actions->melee_unknown31 = 0;
	actions->melee_counter = 0;
	result = true;

	real lunge_speed = *(real *)(*(byte **)((byte *)g_4e034c + 0x134) + 0x2c);

	if (lunge_speed > 0.0f &&
		(unit->linear_velocity.k * unit->forward.k + unit->linear_velocity.j * unit->forward.j +
		unit->linear_velocity.i * unit->forward.i) / lunge_speed > 0.9f)
	{
		actions->melee_unknown31 = 0x7f;
	}
	if (unit->object_type == 0 && *((char *)UNIT_ACTION_UNIT_GET(unit_index) + 0x399) > 0xf)
		actions->melee_unknown31 = (char)0xff;
	return result;
}

/* the request to melee (type 27) */
struct s_unit_request_melee_attack
{
	long type;
	short mode;
	byte flags;
	byte unknown07;
	long target_index;
	byte unknown0c[0x20 - 0xc];
};

void __stdcall function_a8cf0(long unit_index, long target_index, long mode);
void __stdcall function_cf3d0(long unit_index, long name, long flags, real scale);

/* a melee attack (type 27): its animation, once any earlier one has
   passed its end frame */
// @retail 0xe9ed0
bool __stdcall unit_action_melee_attack(long unit_index, s_unit_request *request)
{
	s_unit_request_melee_attack *melee = (s_unit_request_melee_attack *)request;
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	long weapon_index = unit_action_weapon_get(unit, false);
	bool can_melee = true;
	bool result = false;

	if (weapon_index != NONE)
	{
		s_unit_action_unit *weapon = UNIT_ACTION_UNIT_GET(weapon_index);
		byte *s_type_67e06b = g_4e3b44[weapon->definition_index & 0xffff].bytes;
		byte state = *((byte *)weapon + 0x20c);

		if (((*(dword *)(s_type_67e06b + 0x12c) >> 9) & 1) || state == 1 || state == 2)
			can_melee = false;
	}

	s_unit_action_unit *current = UNIT_ACTION_UNIT_GET(unit_index);

	if (((current->flags_134 >> 25) & 1) || UNIT_ACTION_ACTIVE(UNIT_ACTIONS_GET(current), 0x16))
		can_melee = false;
	if (UNIT_ACTION_ACTIVE(UNIT_ACTIONS_GET(current), 0x1b) &&
		(actions->melee_end_ticks == NONE || actions->melee_counter < actions->melee_end_ticks))
	{
		return result;
	}
	if (!can_melee)
		return result;

	function_e69c0(unit_index, 8);
	function_e69c0(unit_index, 0x12);
	function_e69c0(unit_index, 0);
	function_e69c0(unit_index, 0xa);

	s_unit_request stop;

	memset(&stop, 0, sizeof(stop));
	stop.type = 0x13;
	function_e6900(unit_index, &stop);
	function_c86e0(unit_index, 0);
	if (function_e9a20(unit_index, melee->mode, melee->target_index, melee->flags))
	{
		function_a8cf0(unit_index, melee->target_index, melee->mode);
		return true;
	}
	return result;
}

/* a melee attack's ticks: its damage frame, then the next attack in the
   combo (or the end) */
// @retail 0xea090
bool __stdcall unit_action_melee_attack_update(long unit_index, long type)
{
	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));

	if (actions->melee_name != NONE)
	{
		bool finished = false;

		if (actions->melee_counter == actions->melee_damage_ticks)
		{
			function_cf3d0(unit_index, actions->melee_name, actions->melee_flags,
				(real)(byte)actions->melee_unknown31 * 0.0039215689f);
		}
		if (actions->melee_name == 0xa0005bb || actions->melee_name == 0x130005bc)
		{
			real seconds = actions->melee_name == 0x130005bc ? 0.22f : 0.15f;
			long ticks = unit_action_round((real)g_510c54->field_2_3 * seconds);
			s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
			long lunge_ticks = *(long *)((byte *)unit + 0x3ec);

			if (*((byte *)unit + 0x3dc) != 6 || lunge_ticks < 0 || lunge_ticks <= ticks)
				finished = true;
		}
		if (++actions->melee_counter < actions->melee_ticks && !finished)
			return true;
		if (function_e9a20(unit_index, 0, NONE, actions->melee_flags))
			return true;
	}
	actions->active[0] &= ~0x8000000;
	actions->melee_name = NONE;
	return false;
}

// @retail 0xea1c0
void __stdcall unit_action_melee_attack_interrupted(long unit_index, long type)
{
	UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index))->melee_name = NONE;
}

bool __stdcall function_c92c0(long unit_index, long vehicle_index, short seat_index, long *a, bool *b);
void function_e5930(long unit_index);
long __stdcall function_c7160(long unit_index, short seat_index, long a, long vehicle_index, long b);
bool function_bbe60(long tag_index);
void function_ce040(long unit_index);
void function_141590(transform4x3f const *in, transform4x3f *out);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
void __stdcall function_b8ee0(long parent_index, long marker_name, long object_index, long a);
void __stdcall function_cc810(long vehicle_index);
bool function_101240(long weapon_index);
bool function_159d80(void);
quaternionf *function_141f60(matrix3x3 const *matrix, quaternionf *out);
void __stdcall function_bd020(long object_index);
void function_15e7f0(long unit_index, long vehicle_index);
void function_d1000(long unit_index);
void __stdcall function_1bb570(long vehicle_index, long actor_index);
void function_1bbc00(long player_index, long vehicle_index);
void function_1874b0(long player_index, vector3f const *forward);
long function_c8f60(long unit_index, short seat_index);

/* puts a unit in a vehicle's seat: attached to the seat's marker, its
   weapon put away (or dropped), its seat animations, and the seat's
   occupant asked to leave */
// @retail 0xea1f0
bool function_ea1f0(long unit_index, long vehicle_index, short seat_index, bool keep_animation, long type,
	bool *animation_reset, bool force)
{
	bool result = false;
	bool reset = false;

	if (!force && !function_c92c0(unit_index, vehicle_index, seat_index, NULL, NULL))
		goto done;

	{
		s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);

		if (unit->parent_object_index != NONE)
			goto done;
		if (UNIT_ACTIONS_GET(unit)->unknown36 != 0)
			function_e6f90(unit_index);

		s_unit_action_header *header = UNIT_ACTION_HEADER_GET(unit_index);

		if (((1 << header->type) & 1) && (*((byte *)header->unit + 0x3dc) == 4 || *((byte *)header->unit + 0x3dc) == 5))
			function_e5930(unit_index);
		function_e69c0(unit_index, 0x1d);
		function_e69c0(unit_index, 0x1c);
		function_e69c0(unit_index, 0x20);
		if (type != 0x1f)
			function_e69c0(unit_index, 0x1f);

		long state_name = NONE;

		if (!keep_animation)
			state_name = function_c7160(unit_index, seat_index, 0, vehicle_index, 0);

		bool is_vehicle = ((1 << UNIT_ACTION_HEADER_GET(unit_index)->type) >> 1) & 1;
		s_unit_action_unit *vehicle = UNIT_ACTION_UNIT_GET(vehicle_index);
		byte *seat = *(byte **)(g_4e3b44[vehicle->definition_index & 0xffff].bytes + 0x1cc) + seat_index * 0xb0;

		if (unit->unknown13c != NONE)
			function_bbe60(unit->definition_index);
		function_ce040(unit_index);

		s_unit_action_unit *current = UNIT_ACTION_UNIT_GET(unit_index);
		transform4x3f const *field_50 =
			(transform4x3f const *)((byte *)current + *(short *)((byte *)current + 0x116));
		s_object_marker marker;
		transform4x3f inverse;
		transform4x3f relative;

		function_b8d30(vehicle_index, *(long *)(seat + 8), &marker, 1, false);
		function_141590(&marker.matrix, &inverse);
		function_142a60(&inverse, field_50, &relative);
		function_b8ee0(vehicle_index, *(long *)(seat + 8), unit_index, 0);
		unit->seat_index = seat_index;
		if (UNIT_ACTION_UNIT_GET(unit_index)->unknown0d4 != NONE)
			function_b58c0(UNIT_ACTION_UNIT_GET(unit_index)->unknown0d4, 0x800);
		function_cc810(vehicle_index);

		if (!is_vehicle)
		{
			function_e68c0(0x13, unit_index);

			long weapon_index = unit_action_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), false);

			if (weapon_index == NONE)
			{
				function_e8620(unit_index, true);
			}
			else if (!function_cd6a0(unit_index, NONE, weapon_index))
			{
				if (function_101240(weapon_index) && !(g_4e6948->state == 2 && function_159d80()))
				{
					s_unit_request drop;

					drop.type = 9;
					*((bool *)&drop + 4) = true;
					unit_action_drop_weapon(unit_index, &drop);
				}
				else
				{
					function_e8460(unit_index, true);
				}
				function_e8620(unit_index, true);
			}
			function_10f430(unit_index, *(long *)(seat + 4), 0x7000101, 0x7000101,
				state_name == NONE ? 0x400000c : state_name, 0.0f, 0, 2);
		}

		byte *seat_transform = (byte *)unit + *(short *)((byte *)unit + 0x10e);
		quaternionf orientation;

		function_141f60(&relative.rotation, &orientation);
		*(point3f *)(seat_transform + 0x10) = relative.position;
		*(quaternionf *)seat_transform = orientation;
		if (state_name == NONE)
		{
			function_ba3d0(unit_index);
			vector3f *seat_velocity = (vector3f *)((byte *)unit + 0x288);
			vector3f *seat_angular_velocity = (vector3f *)((byte *)unit + 0x294);

			seat_velocity->i = 0.0f;
			seat_angular_velocity->i = 0.0f;
			seat_velocity->j = 0.0f;
			seat_angular_velocity->j = 0.0f;
			seat_velocity->k = 0.0f;
			seat_angular_velocity->k = 0.0f;
			reset = true;
		}
		function_bd020(unit_index);
		function_15e7f0(unit_index, vehicle_index);
		function_c86e0(unit_index, 0);
		function_d1000(unit_index);
		*((byte *)unit + 0x258) = 0;
		if (unit->unknown12c != NONE)
			function_1bb570(unit->parent_object_index, unit->unknown12c);
		if (unit->unknown13c != NONE)
			function_1bbc00(unit->unknown13c, unit->parent_object_index);
		if (unit->unknown13c != NONE && ((*(dword *)seat >> 2) & 1))
		{
			short user_index = unit_action_player_value28_get(unit->unknown13c);

			if (user_index != NONE)
				function_1874b0(user_index, (vector3f const *)((byte *)vehicle + 0x150));
		}
		result = true;
		if (force)
		{
			long occupant_index = function_c8f60(vehicle_index, seat_index);

			if (occupant_index != unit_index && occupant_index != NONE)
			{
				s_unit_request exit;

				exit.type = 0x20;
				*((bool *)&exit + 4) = false;
				*((bool *)&exit + 5) = true;
				result = function_e6900(occupant_index, &exit);
			}
		}
	}

done:
	if (animation_reset)
		*animation_reset = reset;
	return result;
}

void function_b9c60(long object_index, bool flag);
void function_ba690(long object_index, unsigned char **states, long *state_count, long *a, long *b);
void __stdcall function_d1360(long vehicle_index, long seat_index, bool a, bool b);
void function_e5300(long unit_index, long a);

/* entering a vehicle's seat finished: the unit's visibility in the seat,
   and the seat's state */
// @retail 0xea6b0
void __stdcall function_ea6b0(long unit_index, long type)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);

	if (unit->parent_object_index != NONE && unit->seat_index != NONE)
	{
		byte *seat = unit_action_seat_get(unit);

		function_b9c60(unit_index, (bool)(*seat & 1));
		if (((*(dword *)seat >> 19) & 1) && *(long *)(seat + 0xac) != NONE)
		{
			unsigned char *states;
			long state_count;
			bool hidden = false;

			function_ba690(unit->parent_object_index, &states, &state_count, NULL, NULL);
			if (*(long *)(seat + 0xac) < state_count)
				hidden = states[*(long *)(seat + 0xac) * 8 + 1] < 3;
			function_b9c60(unit_index, hidden);
		}
		function_d1360(unit->parent_object_index, unit->seat_index, 0, 0);
	}
}

/* entering a vehicle's seat: a biped's settling ticks */
// @retail 0xea7c0
bool __stdcall unit_action_vehicle_entry_update(long unit_index, long type)
{
	s_unit_action_header *header = UNIT_ACTION_HEADER_GET(unit_index);
	s_unit_action_unit *unit = header->unit;

	if (((1 << header->type) & 1) && 0.2f > (real)*((char *)unit + 0x258) * g_510c54->rate)
	{
		function_e5300(unit_index, 1);
		(*((char *)unit + 0x258))++;
	}
	return true;
}

/* the request to enter a vehicle (type 28) */
struct s_unit_request_vehicle_entry
{
	long type;
	long vehicle_index;
	short seat_index;
	bool keep_animation;
	bool force;
	byte unknown0c[0x20 - 0xc];
};

/* enters a vehicle's seat (type 28) */
// @retail 0xea830
bool __stdcall unit_action_vehicle_entry(long unit_index, s_unit_request *request)
{
	s_unit_request_vehicle_entry *entry = (s_unit_request_vehicle_entry *)request;
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	bool reset = false;

	if (!function_ea1f0(unit_index, entry->vehicle_index, entry->seat_index, entry->keep_animation, entry->type,
		&reset, entry->force))
	{
		return false;
	}
	if (reset)
	{
		function_ea6b0(unit_index, entry->type);
		return true;
	}
	actions->active[0] |= 0x10000000;
	function_d1360(unit->parent_object_index, unit->seat_index, 0, 1);
	return true;
}

long function_10eef0(long object_index, bool alternate, bool no_request);
void function_d12b0(long vehicle_index, long seat_index, bool a, bool b);
bool function_b9d20(long object_index);
void __stdcall function_bef30(long object_index, long a, long b, long c, bool d);
void function_b8b70(long object_index);
long __stdcall function_cdeb0(long unit_index, long state_name, long a, long b);
bool __stdcall function_ee460(long unit_index, long state_name, long action_name);
void function_e0070(long unit_index, long vehicle_index);
void function_1bba70(long actor_index, long vehicle_index, long seat_index);
void function_1bbcc0(long player_index, long vehicle_index, long seat_index);
void function_db670(long object_index, long vehicle_index);
void function_a8b10(long unit_index);

/* takes a unit out of its vehicle seat: detached, its weapon back in hand,
   its exit animations */
// @retail 0xea8e0
void __stdcall function_ea8e0(long unit_index, bool hurried)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	bool is_vehicle = ((1 << unit->object_type) >> 1) & 1;
	long vehicle_index = unit->parent_object_index;

	if (vehicle_index == NONE || unit->seat_index == NONE)
		return;

	long seat_index = unit->seat_index;
	long state_name = NONE;
	long action_name = NONE;

	if (!is_vehicle)
	{
		state_name = function_10eef0(unit_index, false, true);
		action_name = hurried ? 0x400000c : 0x800001e;
	}
	function_d12b0(vehicle_index, (short)seat_index, 0, 0);
	*(long *)((byte *)unit + 0x250) = vehicle_index;
	*(long *)((byte *)unit + 0x254) = g_510c54->game_time;
	unit->seat_index = NONE;
	function_b9a90(unit_index);
	function_b9b90(unit_index, false);

	s_unit_action_unit *current = UNIT_ACTION_UNIT_GET(unit_index);

	if (current->object_flags & 1)
	{
		current->object_flags &= ~1;
		if (function_b9d20(unit_index))
			function_bef30(unit_index, 0, 1, 0, false);
		function_b8b70(unit_index);
	}
	if (UNIT_ACTION_UNIT_GET(unit_index)->unknown0d4 != NONE)
		function_b58c0(UNIT_ACTION_UNIT_GET(unit_index)->unknown0d4, 0x800);
	if (!is_vehicle)
	{
		if (function_cbd50(unit_index, UNIT_ACTION_UNIT_GET(unit_index)->current_weapon_index) == NONE)
		{
			unit->weapon_slots[0] = (char)function_cdeb0(unit_index, state_name, NONE, 0);
			function_e8720(unit_index, state_name, false, false, true);
		}
		function_ee460(unit_index, state_name, action_name);
	}
	function_cc810(vehicle_index);
	if (!is_vehicle && unit->object_type == 0)
		function_e0070(unit_index, vehicle_index);
	function_bd020(unit_index);
	if (unit->unknown12c != NONE)
		function_1bba70(unit->unknown12c, vehicle_index, seat_index);
	if (unit->unknown13c != NONE)
		function_1bbcc0(unit->unknown13c, vehicle_index, seat_index);
	function_db670(unit_index, vehicle_index);
	function_d1000(unit_index);
}

/* starts a unit's exit from its seat: at once for a vehicle, else its exit
   animation */
// @retail 0xeaad0
bool function_eaad0(long unit_index)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	long vehicle_index = unit->parent_object_index;
	bool result = false;

	if (vehicle_index != NONE && unit->seat_index != NONE)
	{
		long seat_index = unit->seat_index;

		function_ce040(unit_index);
		if (unit->object_type == 1)
		{
			function_ea8e0(unit_index, false);
		}
		else
		{
			if (!function_10f430(unit_index, 0x7000101, 0x7000101, 0x7000101, 0x400004a, 0.0f, 0, 2))
				return result;
			function_b9c60(unit_index, false);
			function_a8b10(unit_index);
		}
		function_d12b0(vehicle_index, (short)seat_index, 0, 0);
		result = true;
	}
	return result;
}

void function_1bba20(long actor_index);

/* exits a vehicle (type 29) */
// @retail 0xeab80
bool __stdcall unit_action_vehicle_exit(long unit_index, s_unit_request *request)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	bool result = false;

	if (UNIT_ACTION_ACTIVE(UNIT_ACTIONS_GET(unit), 0x1d) ||
		UNIT_ACTION_ACTIVE(UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index)), 0x1f) ||
		UNIT_ACTION_ACTIVE(UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index)), 0x20))
	{
		return result;
	}
	function_e69c0(unit_index, 0x1d);
	function_e69c0(unit_index, 0x1c);
	function_e69c0(unit_index, 0x1f);
	function_e69c0(unit_index, 0x20);
	*((byte *)unit + 0x258) = 0;
	if (!function_eaad0(unit_index))
		return false;
	actions->active[0] |= 0x20000000;
	if (*((bool *)request + 4))
		actions->unknown00 |= 1;
	else
		actions->unknown00 &= ~1;
	if (unit->unknown12c != NONE)
		function_1bba20(unit->unknown12c);
	return true;
}

transform4x3f *function_ba160(long object_index, transform4x3f *matrix);

/* the end of a vehicle exit: the unit leaves the seat with its exit
   animation's velocity */
// @retail 0xeac90
void __stdcall function_eac90(long unit_index, bool hurried, bool push)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	byte *definition = g_4e3b44[unit->definition_index & 0xffff].bytes;
	byte *model = g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes;
	byte *graph = g_4e3b44[*(long *)(model + 4) & 0xffff].bytes;
	s_animation_state *state = (s_animation_state *)((byte *)unit + unit->animation_state_offset);
	vector3f velocity;
	transform4x3f matrix_storage;

	state->animation_velocity_get(state->channels[0].animation_id, state->channels[0].frame_position * 0.033333335f,
		state->channels[0].rate, (long)graph, &velocity);

	transform4x3f *matrix = function_ba160(unit_index, &matrix_storage);
	vector3f world;

	if (matrix->scale != 1.0f)
	{
		velocity.i *= matrix->scale;
		velocity.j *= matrix->scale;
		velocity.k *= matrix->scale;
	}
	world.i = matrix->up.i * velocity.k + matrix->left.i * velocity.j + matrix->forward.i * velocity.i;
	world.j = matrix->up.j * velocity.k + matrix->left.j * velocity.j + matrix->forward.j * velocity.i;
	world.k = matrix->up.k * velocity.k + matrix->left.k * velocity.j + matrix->forward.k * velocity.i;
	function_ea8e0(unit_index, hurried);
	unit->linear_velocity.i += world.i;
	unit->linear_velocity.j += world.j;
	unit->linear_velocity.k += world.k;
	if (push && unit_index != NONE)
	{
		*((byte *)UNIT_ACTION_UNIT_GET(unit_index) + 0x10a) |= 0x20;
		function_b7360(unit_index);
	}
}

// @retail 0xeae60
void __stdcall function_eae60(long unit_index, long type)
{
	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));

	function_eac90(unit_index, (bool)(*(byte *)&actions->unknown00 & 1), false);
}

/* exits a vehicle at once (type 30) */
// @retail 0xeaea0
bool __stdcall unit_action_vehicle_exit_immediate(long unit_index, s_unit_request *request)
{
	function_ea8e0(unit_index, false);
	return true;
}

/* a vehicle exit: its exit animation started (once the seat animation
   allows), and a biped's settling ticks */
// @retail 0xeaeb0
bool __stdcall unit_action_vehicle_exit_update(long unit_index, long type)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	long state_name;
	long action_name;

	function_10f630(unit_index, &state_name, &action_name);
	if (state_name != 0x400004a)
		function_eaad0(unit_index);
	if (((1 << UNIT_ACTION_HEADER_GET(unit_index)->type) & 1) && 0.2f > (real)*((char *)unit + 0x258) * g_510c54->rate)
	{
		function_e5300(unit_index, 0);
		(*((char *)unit + 0x258))++;
	}
	return true;
}

/* the units in a vehicle's seats (damage.cpp's iterator) */
struct s_unit_child_iterator
{
	long object_index;
	long unit_index;
	short seat_index;
	long next_index;
};

struct s_damage_object;
s_damage_object *function_d05c0(s_unit_child_iterator *iterator);
bool function_1df560(short team_a, short team_b);

/* whether a unit (an actor) can board a vehicle's seat: no friend in it,
   and no player boarding it */
// @retail 0xeaf50
bool function_eaf50(long unit_index, long vehicle_index, short seat_index)
{
	if (UNIT_ACTION_UNIT_GET(unit_index)->unknown12c == NONE)
		return true;

	s_unit_action_unit *vehicle = UNIT_ACTION_UNIT_GET(vehicle_index);
	byte *local_a2a045 = g_4e3b44[vehicle->definition_index & 0xffff].bytes;
	byte *unit_object = (byte *)function_badc0(unit_index, 3);
	short team = NONE;
	s_unit_child_iterator iterator;
	byte *occupant;

	if (unit_object)
		team = *(short *)(unit_object + 0x138);
	iterator.object_index = vehicle_index;
	iterator.unit_index = NONE;
	iterator.seat_index = NONE;
	iterator.next_index = *(long *)((byte *)vehicle + 0x10);
	while (occupant = (byte *)function_d05c0(&iterator))
	{
		if (iterator.seat_index == seat_index)
		{
			byte *other = (byte *)function_badc0(iterator.unit_index, 3);
			short other_team = other ? *(short *)(other + 0x138) : NONE;

			if (!function_1df560(team, other_team))
				return false;
		}
		else if (iterator.seat_index != NONE)
		{
			byte *seat = *(byte **)(local_a2a045 + 0x1cc) + iterator.seat_index * 0xb0;

			if (((*(dword *)seat >> 11) & 1) && *(short *)(seat + 0x3e) == seat_index &&
				(*(long *)(occupant + 0x13c) != NONE || unit_action_active(iterator.unit_index, 0x1f)))
			{
				return false;
			}
		}
	}
	return true;
}

void __stdcall function_a8b90(long unit_index);

/* boards another seat of a unit's vehicle (type 31): the seat its seat
   boards */
// @retail 0xeb090
bool __stdcall unit_action_vehicle_board(long unit_index, s_unit_request *request)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	long vehicle_index = unit->parent_object_index;

	if (vehicle_index == NONE)
		return false;

	s_unit_action_header *vehicle_header = UNIT_ACTION_HEADER_GET(vehicle_index);
	s_unit_action_unit *vehicle = vehicle_header->unit;

	if (!((1 << vehicle->object_type) & 3))
		return false;

	byte *local_a2a045 = g_4e3b44[vehicle->definition_index & 0xffff].bytes;
	byte *seat = *(byte **)(local_a2a045 + 0x1cc) + unit->seat_index * 0xb0;

	if (UNIT_ACTION_ACTIVE(actions, 0x1f) || !((*(dword *)seat >> 11) & 1))
		return false;

	short board_seat_index = *(short *)(seat + 0x3e);

	if (board_seat_index == NONE || !function_eaf50(unit_index, vehicle_index, board_seat_index))
		return false;
	function_e69c0(unit_index, 0x1d);
	function_e69c0(unit_index, 0x1c);
	function_e69c0(unit_index, 0x1f);
	function_e69c0(unit_index, 0x20);
	if (!function_113e90(unit_index, 0x50000c3, 0.0f, NULL, 0))
		return false;
	actions->active[0] |= 0x80000000;
	*((byte *)unit + 0x258) = 0;
	function_a8b90(unit_index);
	if (((*(dword *)seat >> 13) & 1) &&
		((*(dword *)(*(byte **)(local_a2a045 + 0x1cc) + *(short *)(seat + 0x3e) * 0xb0) >> 2) & 1))
	{
		function_d12b0(vehicle_index, *(short *)(seat + 0x3e), 0, 0);
		if (unit->unknown13c != NONE)
		{
			short user_index = unit_action_player_value28_get(unit->unknown13c);

			if (user_index != NONE)
				function_1874b0(user_index, (vector3f const *)((byte *)vehicle + 0x150));
		}
	}
	function_cc810(vehicle_index);
	return true;
}

bool __stdcall function_d0f30(long unit_index, bool a, bool b);
long function_10f720(long object_index, bool first);

/* boarding finished: the unit moves to the boarded seat (or just leaves
   its own) */
// @retail 0xeb270
void __stdcall function_eb270(long unit_index, long type)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);

	if (unit->parent_object_index == NONE || unit->seat_index == NONE)
		return;

	long vehicle_index = unit->parent_object_index;
	byte *seat = unit_action_seat_get(unit);

	if (*(short *)(seat + 0x3e) == NONE)
		return;
	if ((*(dword *)seat >> 13) & 1)
	{
		function_d0f30(unit_index, 0, 1);
		function_ea8e0(unit_index, true);
		function_ea1f0(unit_index, vehicle_index, *(short *)(seat + 0x3e), true, type, NULL, false);
		function_ea6b0(unit_index, type);
	}
	else
	{
		function_ea8e0(unit_index, true);
	}
}

/* boarding: a biped's settling ticks, and the boarding pose */
// @retail 0xeb340
bool __stdcall unit_action_vehicle_board_update(long unit_index, long type)
{
	s_unit_action_header *header = UNIT_ACTION_HEADER_GET(unit_index);
	s_unit_action_unit *unit = header->unit;

	if (((1 << header->type) & 1) && 0.2f > (real)*((char *)unit + 0x258) * g_510c54->rate)
	{
		function_e5300(unit_index, 1);
		(*((char *)unit + 0x258))++;
	}
	if (function_10f720(unit_index, true) != 2)
		function_d0f30(unit_index, 0, 0);
	return true;
}

/* ejects from a vehicle (type 32), or (when asked) finishes an ejection at
   once */
// @retail 0xeb3c0
bool __stdcall unit_action_vehicle_ejection(long unit_index, s_unit_request *request)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	long vehicle_index = unit->parent_object_index;
	bool result = false;

	if (vehicle_index == NONE || unit->seat_index == NONE)
		return result;
	if (*((bool *)request + 5))
		function_e69c0(unit_index, 0x20);
	if (UNIT_ACTION_ACTIVE(UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index)), 0x20))
		return false;
	function_e69c0(unit_index, 0x1d);
	function_e69c0(unit_index, 0x1c);
	function_e69c0(unit_index, 0x1f);
	function_e69c0(unit_index, 0x1b);
	function_e69c0(unit_index, 0x1a);
	function_e69c0(unit_index, 0x16);
	if (*((bool *)request + 5))
	{
		function_e6960(unit_index, 0x20);
		function_cc810(vehicle_index);
		return true;
	}
	if (!function_10f430(unit_index, 0x7000101, 0x7000101, 0x7000101, 0x80000c4, 0.0f, 0, 2))
		return false;
	if (*((bool *)request + 4))
		actions->unknown00 |= 2;
	else
		actions->unknown00 &= ~2;
	actions->active[1] |= 1;
	function_cc810(vehicle_index);
	return true;
}

void function_152140(long player_index);

// @retail 0xeb520
void __stdcall function_eb520(long unit_index, long type)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	bool push = (UNIT_ACTIONS_GET(unit)->unknown00 >> 1) & 1;

	if (unit->unknown13c != NONE)
		function_152140(unit->unknown13c);
	function_eac90(unit_index, false, push);

	s_unit_action_header *header = UNIT_ACTION_HEADER_GET(unit_index);

	if ((1 << header->type) & 1)
		*((byte *)header->unit + 0x348) |= 0x40;
}

// @retail 0xeb5a0
bool __stdcall unit_action_vehicle_ejection_update(long unit_index, long type)
{
	bool result = function_114040(unit_index, 0x80000c4);

	if (!result)
		function_eb520(unit_index, type);
	return result;
}

struct s_damage_owner
{
	long player_index;
	long object_index;
	short team;
};

__declspec(noinline) bool function_f5dc0(long object_index);
void object_get_damage_owner(long object_index, s_damage_owner *owner);

/* flips a vehicle back over (type 33): the way it rolls, from how far over
   it is and where the flipping unit stands */
// @retail 0xeb5d0
bool __stdcall unit_action_vehicle_flip(long unit_index, s_unit_request *request)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);

	if (unit->object_type != 1)
		return false;
	if (!function_f5dc0(unit_index))
		return false;

	long flipper_index = *(long *)((byte *)request + 4);

	if (flipper_index != NONE)
	{
		byte *flipper = (byte *)UNIT_ACTION_UNIT_GET(flipper_index);

		*(long *)(flipper + 0x250) = unit_index;
		*(long *)(flipper + 0x254) = g_510c54->game_time;
	}
	if ((real)fabs(unit->forward.k) > 0.70710677f)
	{
		*((byte *)unit + 0x34d) = (byte)((0.0f > unit->forward.k) + 3);
	}
	else if (flipper_index != NONE)
	{
		transform4x3f unit_matrix_storage;
		transform4x3f flipper_matrix_storage;
		transform4x3f *unit_matrix = function_ba160(unit_index, &unit_matrix_storage);
		transform4x3f *flipper_matrix = function_ba160(flipper_index, &flipper_matrix_storage);
		vector3f offset;
		vector3f const *up = g_4687b0;
		vector3f side;

		offset.i = unit_matrix->position.x - flipper_matrix->position.x;
		offset.j = unit_matrix->position.y - flipper_matrix->position.y;
		offset.k = unit_matrix->position.z - flipper_matrix->position.z;
		side.k = up->i * offset.j - up->j * offset.i;
		side.j = up->k * offset.i - up->i * offset.k;
		side.i = up->j * offset.k - up->k * offset.j;
		*((byte *)unit + 0x34d) = (byte)((unit->forward.j * side.j + unit->forward.k * side.k + unit->forward.i * side.i >
			0.0f) + 1);
	}
	else
	{
		*((byte *)unit + 0x34d) = 1;
	}
	*((byte *)unit + 0x348) |= 0x40;
	*((byte *)unit + 0x34e) = 0;

	s_damage_owner owner;

	object_get_damage_owner(*(long *)((byte *)request + 4), &owner);

	byte *current = (byte *)UNIT_ACTION_UNIT_GET(unit_index);

	*(long *)(current + 0xc8) = owner.object_index;
	*(long *)(current + 0xc4) = owner.player_index;
	*(short *)(current + 0xc2) = owner.team;
	return true;
}
// @retail 0xe5670
long function_e5670(long unit_index)
{
	return *((byte *)UNIT_ACTION_UNIT_GET(unit_index) + 0x3dc);
}

void __stdcall function_b9a50(long unit_index);
void __stdcall function_e56f0(long unit_index, point3f const *point);
bool __stdcall function_ee090(long unit_index, long state_name, long mode, long action_name, point3f const *point,
	vector3f const *facing);

/* the request to move a unit to a point (type 34) */
struct s_unit_request_move
{
	long type;
	point3f point;
	point2f facing;
	bool immediate;
	byte unknown19[0x20 - 0x19];
};

/* moves a unit to a point and facing (type 34): at once, or by its
   animation */
// @retail 0xeb7e0
bool __stdcall function_eb7e0(long unit_index, s_unit_request *request)
{
	s_unit_request_move *move = (s_unit_request_move *)request;
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	bool result = false;

	if (unit->parent_object_index != NONE)
		return result;
	if (move->immediate)
	{
		vector3f facing;

		facing.i = move->facing.x;
		facing.j = move->facing.y;
		facing.k = 0.0f;
		function_b9a50(unit_index);
		if (function_30bf0(&facing) > 0.0f)
			function_b75a0(unit_index, NULL, &facing, g_4687b0, NULL, false);
		function_e56f0(unit_index, &move->point);

		s_unit_actions *current = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));

		current->active[0] = 0;
		current->active[1] = 0;
		current->unknown36 = 0;
		return true;
	}
	if (UNIT_ACTION_ACTIVE(actions, move->type))
		return result;

	vector3f facing;

	facing.i = move->facing.x;
	facing.j = move->facing.y;
	facing.k = 0.0f;
	if (!function_ee090(unit_index, 0x50000cb, 1, 0x60000cc, &move->point, &facing))
		return false;
	actions->active[move->type >> 5] |= 1 << (move->type & 0x1f);
	return true;
}

bool function_10f9b0(long unit_index, long state_name, long action_name, long a, transform4x3f *matrix, bool flag);

/* the move finished: the unit's seat transform from where the animation
   left it */
// @retail 0xeb960
void __stdcall function_eb960(long unit_index, long type)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	transform4x3f nodes = *(transform4x3f *)((byte *)unit + *(short *)((byte *)unit + 0x116));
	transform4x3f matrix;
	transform4x3f inverse;
	transform4x3f moved;
	transform4x3f object_matrix;
	transform4x3f object_inverse;
	quaternionf orientation;

	function_ec0d0(unit_index, 0x50000cb, 0x400000c);
	if (!function_10f9b0(unit_index, 0x50000cb, 0x400000c, 3, &matrix, false))
		return;
	function_141590(&matrix, &inverse);
	function_142a60(&nodes, &inverse, &moved);
	function_e56f0(unit_index, &moved.position);

	s_unit_action_unit *current = UNIT_ACTION_UNIT_GET(unit_index);
	byte *seat_transform = (byte *)current + *(short *)((byte *)current + 0x10e);

	function_ba160(unit_index, &object_matrix);
	function_141590(&object_matrix, &object_inverse);
	function_142a60(&object_inverse, &nodes, &matrix);
	function_141f60(&matrix.rotation, &orientation);
	*(point3f *)(seat_transform + 0x10) = matrix.position;
	*(quaternionf *)seat_transform = orientation;
}

/* moves a unit to a point and facing (type 35) */
// @retail 0xebaa0
bool __stdcall function_ebaa0(long unit_index, s_unit_request *request)
{
	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));
	bool result = false;

	if (UNIT_ACTION_ACTIVE(actions, request->type))
		return result;
	if (!function_ee090(unit_index, 0x50000cb, 1, 0x60000cd, (point3f const *)((byte *)request + 0x14),
		(vector3f const *)((byte *)request + 8)))
	{
		return result;
	}
	actions->active[request->type >> 5] |= 1 << (request->type & 0x1f);
	return true;
}

/* a unit's facing in its seat (or its own when it has no parent) */
PRIVATE inline void unit_action_seat_facing_get(s_unit_action_unit *unit, vector3f *facing)
{
	if (unit->parent_object_index == NONE)
	{
		*facing = unit->forward;
	}
	else
	{
		s_unit_action_unit *parent = UNIT_ACTION_UNIT_GET(unit->parent_object_index);
		transform4x3f const *matrix = (transform4x3f const *)((byte *)parent +
			*(short *)((byte *)parent + 0x116) + *((char *)unit + 0x18) * 0x34);

		facing->i = matrix->up.i * unit->forward.k + matrix->left.i * unit->forward.j + matrix->forward.i * unit->forward.i;
		facing->j = matrix->up.j * unit->forward.k + matrix->left.j * unit->forward.j + matrix->forward.j * unit->forward.i;
		facing->k = matrix->up.k * unit->forward.k + matrix->left.k * unit->forward.j + matrix->forward.k * unit->forward.i;
	}
}

/* the request to play a seat animation at a point (type 36) */
struct s_unit_request_seat_animation
{
	long type;
	point3f point;
	vector3f facing;
	short mode;
	byte unknown1e[2];
};

/* plays a seat animation (type 36): the request's, or the one following
   the unit's current animation */
// @retail 0xebb40
bool __stdcall function_ebb40(long unit_index, s_unit_request *request)
{
	s_unit_request_seat_animation *seat_request = (s_unit_request_seat_animation *)request;
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	short mode = seat_request->mode;
	point3f point;
	vector3f facing;
	long state_name;

	if (mode != 0 && UNIT_ACTIONS_GET(unit)->unknown36 != 7)
	{
		point = seat_request->point;
		facing = seat_request->facing;
		switch (mode)
		{
		case 1:
			state_name = 0x110001b4;
			break;
		case 2:
			state_name = 0x120001b5;
			break;
		case 3:
			state_name = 0xc0006c3;
			break;
		default:
			return false;
		}
		return function_ee090(unit_index, state_name, 6, 0x5000049, &point, &facing);
	}

	function_b9dd0(unit_index, &point);
	unit_action_seat_facing_get(unit, &facing);

	long field_c_4;
	long current_action;

	if (!function_10f630(unit_index, &current_action, &field_c_4))
		return false;
	switch (field_c_4)
	{
	case 0xb0006c4:
		state_name = 0xc0006c3;
		break;
	case 0x10000229:
		state_name = 0x110001b4;
		break;
	case 0x1100022a:
		state_name = 0x120001b5;
		break;
	default:
		return false;
	}
	return function_ee090(unit_index, state_name, 6, 0x400000c, &point, &facing);
}

/* plays the return from a seat animation (type 37): the request's, or the
   one following the unit's current animation */
// @retail 0xebd30
bool __stdcall function_ebd30(long unit_index, s_unit_request *request)
{
	s_unit_request_seat_animation *seat_request = (s_unit_request_seat_animation *)request;
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	short mode = seat_request->mode;
	point3f point;
	vector3f facing;
	long state_name;

	if (mode != 0 && UNIT_ACTIONS_GET(unit)->unknown36 != 6)
	{
		point = seat_request->point;
		facing = seat_request->facing;
		switch (mode)
		{
		case 1:
			state_name = 0x10000229;
			break;
		case 2:
			state_name = 0x1100022a;
			break;
		case 3:
			state_name = 0xb0006c4;
			break;
		default:
			return false;
		}
		return function_ee090(unit_index, state_name, 7, 0x5000049, &point, &facing);
	}

	function_b9dd0(unit_index, &point);
	unit_action_seat_facing_get(unit, &facing);

	long field_c_4;
	long current_action;

	if (!function_10f630(unit_index, &current_action, &field_c_4))
		return false;
	switch (field_c_4)
	{
	case 0xc0006c3:
		state_name = 0xb0006c4;
		break;
	case 0x110001b4:
		state_name = 0x10000229;
		break;
	case 0x120001b5:
		state_name = 0x1100022a;
		break;
	default:
		return false;
	}
	return function_ee090(unit_index, state_name, 7, 0x400000c, &point, &facing);
}

/* plays a named seat animation where the unit is (type 38) */
// @retail 0xebf20
bool __stdcall function_ebf20(long unit_index, s_unit_request *request)
{
	point3f point;
	vector3f facing;

	function_b9dd0(unit_index, &point);
	unit_action_seat_facing_get(UNIT_ACTION_UNIT_GET(unit_index), &facing);
	return function_ee090(unit_index, *(long *)((byte *)request + 8), *(long *)((byte *)request + 4), 0x5000049,
		&point, &facing);
}

/* sits a unit down from its pose (type 39) */
// @retail 0xec050
bool __stdcall function_ec050(long unit_index, s_unit_request *request)
{
	bool result = false;

	if (UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index))->unknown36 != 0)
	{
		if (function_10f430(unit_index, 0x7000101, 0x7000101, 0x7000101, 0x400004a, 0.1f, 0, 0))
			return true;
		function_ec0d0(unit_index, 0x6000086, 0x400000c);
	}
	return result;
}

void __stdcall function_b8890(long unit_index);

/* returns a unit to its state and action animations from a pose: a biped
   keeps where the pose left it */
// @retail 0xec0d0
void __stdcall function_ec0d0(long unit_index, long state_name, long action_name)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);

	if (unit->object_type == 0)
	{
		transform4x3f nodes = *(transform4x3f *)((byte *)unit + *(short *)((byte *)unit + 0x116));
		transform4x3f matrix;
		transform4x3f inverse;
		transform4x3f object_matrix;
		transform4x3f object_inverse;
		quaternionf orientation;

		if (function_10f9b0(unit_index, state_name, action_name, 3, &matrix, false))
		{
			vector3f facing;

			function_141590(&matrix, &inverse);
			function_142a60(&nodes, &inverse, &matrix);
			facing.i = matrix.forward.i;
			facing.j = matrix.forward.j;
			facing.k = 0.0f;
			if (function_30bf0(&facing) > 0.0f)
				function_b75a0(unit_index, NULL, &facing, g_4687b0, NULL, false);
		}
		function_e5930(unit_index);
		function_10f430(unit_index, state_name, 0x7000101, 0x7000101, action_name, 0.0f, 0, 2);

		byte *seat_transform = (byte *)unit + *(short *)((byte *)unit + 0x10e);

		function_ba160(unit_index, &object_matrix);
		function_141590(&object_matrix, &object_inverse);
		function_142a60(&object_inverse, &nodes, &matrix);
		function_141f60(&matrix.rotation, &orientation);
		*(point3f *)(seat_transform + 0x10) = matrix.position;
		*(quaternionf *)seat_transform = orientation;
		function_bd020(unit_index);
		actions->unknown36 = 0;
	}
	else
	{
		if (!((unit->flags_c0 >> 6) & 1))
			function_b8890(unit_index);
		function_ee460(unit_index, 0x6000086, 0x400000c);
		actions->unknown36 = 0;
	}
}

// @retail 0xec2b0
void __stdcall function_ec2b0(long unit_index, long type)
{
	function_ec0d0(unit_index, 0x6000086, 0x400000c);
}

/* sits a unit down from its pose, marking it (type 40) */
// @retail 0xec2d0
bool __stdcall function_ec2d0(long unit_index, s_unit_request *request)
{
	bool result = false;

	if (function_ec050(unit_index, request))
	{
		UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index))->active[1] |= 0x100;
		return true;
	}
	return result;
}

void function_e5750(long unit_index);

// @retail 0xec330
void __stdcall function_ec330(long unit_index, long type)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);

	function_ec0d0(unit_index, 0x6000542, 0x400000c);
	if (unit->object_type == 0)
		function_e5750(unit_index);
}

/* raises a weapon to point (type 41): its animation, and when it ends */
// @retail 0xec380
bool __stdcall function_ec380(long unit_index, s_unit_request *request)
{
	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));
	short mode = *(short *)((byte *)request + 4);
	long name;

	if (mode < 0)
		return false;
	if (mode <= 1)
		name = 0xa00022d;
	else if (mode == 2)
		name = 0xb00022e;
	else
		return false;

	c_animation_channel *channel = NULL;

	function_e69c0(unit_index, 8);
	function_e69c0(unit_index, 0x12);
	function_e69c0(unit_index, 0);
	function_e69c0(unit_index, 0xa);
	if (function_113e90(unit_index, name, 0.267f, &channel, 0))
	{
		real event_time = channel->get_event_time();

		actions->point_mode = *((byte *)request + 4);
		actions->point_target = *(long *)((byte *)request + 8);
		if (event_time >= 0.0f)
			actions->point_ticks = (short)unit_action_round_long((real)g_510c54->field_2_3 * event_time);
		else
			actions->point_ticks = (short)unit_action_round_long((real)g_510c54->field_2_3 * 0.8f);
		actions->active[1] |= 0x200;
		return true;
	}
	function_edff0(unit_index);
	actions->point_mode = *((byte *)request + 4);
	actions->point_target = *(long *)((byte *)request + 8);
	actions->point_ticks = 2;
	actions->active[1] |= 0x200;
	return true;
}

struct s_type_1e6529
{
	long definition_index;
	dword flags;
	s_damage_owner owner;
	long unknown14;
	long unknown18;
	s_location location;
	point3f position;
	point3f origin;
	vector3f direction;
	vector3f node_direction;
	real scale;
	byte unknown58[0x7c - 0x58];
	short material_index;
	short unknown7e;
	byte unknown80[4];
	byte unknown84;
	byte unknown85[3];
};

struct s_small_index;
void function_d6660(s_type_1e6529 *data, long definition_index);
void function_d7b80(s_type_1e6529 *data, long object_index, short node_index, short unknown0c, short region_entry_index,
	vector3f const *unknown14);
long function_baf80(long object_index);
short function_0b67a0(const s_small_index *data);
long function_1eb020(long tag_index, long body_index, short *material_index);
void function_188180(point3f const *point, vector3f const *forward, long tag_index, long object_index, long index,
	long variant, long unused, long effect_value, s_location const *location, real scale);
extern short g_54e898;
extern vector3f *g_4687bc;

/* a raised weapon's point (type 41 update): when its time is up, it
   shoves the target in front of the unit, with damage and an impact
   effect */
// @retail 0xec4f0
bool __stdcall function_ec4f0(long unit_index, long type)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	byte *local_98b918 = g_4e3b44[unit->definition_index & 0xffff].bytes;
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	long target_index = actions->point_target;

	actions->point_ticks--;
	if (target_index == NONE)
		return false;
	if (actions->point_ticks > 0)
		return true;

	byte *target = (byte *)function_badc0(target_index, NONE);

	if (!target)
		return false;

	byte *target_definition = g_4e3b44[*(long *)target & 0xffff].bytes;
	s_unit_action_unit *root = UNIT_ACTION_UNIT_GET(function_baf80(target_index));
	vector3f direction;
	point3f target_center;
	point3f unit_center;

	direction.i = unit->forward.i;
	direction.j = unit->forward.j;
	direction.k = 0.0f;
	if (function_30bf0(&direction) == 0.0f)
		return false;

	real negative_j = 0.0f - direction.j;

	function_b9dd0(target_index, &target_center);
	function_b9dd0(unit_index, &unit_center);
	if (!((target_center.y - unit_center.y) * direction.j + (target_center.z - unit_center.z) * direction.k +
		(target_center.x - unit_center.x) * direction.i > 0.0f))
	{
		return false;
	}

	switch (actions->point_mode)
	{
	case 0:
		direction.i *= 3.0f;
		direction.j *= 3.0f;
		break;
	case 1:
	{
		real i = direction.i;

		direction.i = i * 0.3f + negative_j * 3.0f;
		direction.j = direction.j * 0.3f + i * 3.0f;
		break;
	}
	case 2:
	{
		real i = direction.i;

		direction.i = i * 0.3f + negative_j * -3.0f;
		direction.j = direction.j * 0.3f + i * -3.0f;
		break;
	}
	}
	direction.k = 1.5f;

	vector3f shove = direction;

	if (function_30bf0(&shove) == 0.0f)
		return false;

	if (*(long *)(local_98b918 + 0x190) != NONE)
	{
		s_type_1e6529 damage;

		function_d6660(&damage, unit_index);
		damage.material_index = NONE;
		damage.unknown84 = 3;
		damage.scale = 1.0f;
		object_get_damage_owner(unit_index, &damage.owner);
		damage.definition_index = *(long *)(local_98b918 + 0x190);
		damage.position = unit_center;
		damage.origin = unit_center;
		damage.unknown14 = unit_index;
		damage.direction = direction;
		function_30bf0(&damage.direction);
		damage.node_direction = damage.direction;
		function_d7b80(&damage, actions->point_target, NONE, NONE, NONE, NULL);
	}

	if (((1 << *local_98b918) & 1) && root->havok_component_index != NONE)
	{
		byte *definition = g_4e3b44[unit->definition_index & 0xffff].bytes;
		long effect_tag_index = *(long *)(g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes + 0x24);

		if (effect_tag_index != NONE)
		{
			s_havok_component *component = havok_component_get(root->havok_component_index);
			short material_index = g_54e898;
			short rigid_body_index = function_0b67a0((s_small_index const *)component);

			if (rigid_body_index != NONE && component->rigid_bodies[rigid_body_index].node_count > 0 &&
				*component->rigid_bodies[rigid_body_index].nodes != NONE)
			{
				vector3f impulse;
				byte strength;

				impulse.i = shove.i * -1.0f;
				impulse.j = shove.j * -1.0f;
				impulse.k = shove.k * -1.0f;
				function_1eb020(effect_tag_index, (long)*component->rigid_bodies[rigid_body_index].nodes, &material_index);
				strength = target_definition[0x1a];
				if (strength > definition[0x1a])
					strength = definition[0x1a];
				function_188180(&unit_center, &impulse, *(long *)(definition + 0x58), unit_index, 0xb, strength,
					material_index, (long)g_4687bc, (s_location const *)((byte *)unit + 0x28), 1.0f);
			}
		}
	}
	return false;
}

/* plays a pose animation at a point (type 42) */
// @retail 0xec940
bool __stdcall function_ec940(long unit_index, s_unit_request *request)
{
	return function_ee090(unit_index, 0x5000281, 2, 0x5000049, (point3f const *)((byte *)request + 4),
		(vector3f const *)((byte *)request + 0x10));
}

/* plays a pose animation at a point (type 43) */
// @retail 0xec970
bool __stdcall function_ec970(long unit_index, s_unit_request *request)
{
	return function_ee090(unit_index, 0x5000534, 1, 0x5000049, (point3f const *)((byte *)request + 4),
		(vector3f const *)((byte *)request + 0x10));
}

/* a pose (type 44): at once, or by its animation where the unit is */
// @retail 0xec9a0
bool __stdcall function_ec9a0(long unit_index, s_unit_request *request)
{
	bool result = false;

	if (*((bool *)request + 4))
	{
		if (!function_10f430(unit_index, 0x6000542, 0x7000101, 0x7000101, 0x400000c, 0.2f, 0, 0))
			return result;
		function_e5750(unit_index);
		return true;
	}

	point3f point;
	vector3f facing;

	function_b9dd0(unit_index, &point);
	unit_action_seat_facing_get(UNIT_ACTION_UNIT_GET(unit_index), &facing);
	return function_ee090(unit_index, 0x6000542, 3, 0x7000543, &point, &facing);
}

void function_1e54d0(void *state, long a);

// @retail 0xecb20
void __stdcall function_ecb20(long unit_index, long type)
{
	function_ec0d0(unit_index, 0x6000542, 0x400000c);

	byte *unit = (byte *)UNIT_ACTION_UNIT_GET(unit_index);

	function_1e54d0(unit + 0x3dc, 2);
	*(short *)(unit + 0x34c) = NONE;
	unit[0x34e] = 0;
	unit[0x34f] = 0;
}

/* a biped's pose animation at a point (type 47): or, when the pose can't
   start, back to idle at once */
// @retail 0xecb80
bool __stdcall function_ecb80(long unit_index, s_unit_request *request)
{
	bool result = function_ee090(unit_index, 0x6000542, 4, 0x400069d, (point3f const *)((byte *)request + 4),
		(vector3f const *)((byte *)request + 0x10));

	if (UNIT_ACTION_HEADER_GET(unit_index)->type != 0)
		return result;

	long state_name;
	long action_name;

	function_10f630(unit_index, &state_name, &action_name);
	if (result && state_name == 0x400069d)
		return result;
	if (!function_10f430(unit_index, 0x6000086, 0x7000101, 0x7000101, 0x400000c, 8.0f, 0, 0))
		return false;

	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));

	actions->active[0] = 0;
	actions->active[1] = 0;
	actions->unknown36 = 0;
	function_1e54d0((byte *)UNIT_ACTION_UNIT_GET(unit_index) + 0x3dc, 1);
	return true;
}

// @retail 0xecc70
void __stdcall function_ecc70(long unit_index, long type)
{
	function_ec0d0(unit_index, 0x6000086, 0x400000c);

	s_unit_action_header *header = UNIT_ACTION_HEADER_GET(unit_index);

	if (header->type == 0)
		function_1e54d0((byte *)header->unit + 0x3dc, 1);
}

/* a seat pose animation at a point (type 45) */
// @retail 0xeccc0
bool __stdcall function_eccc0(long unit_index, s_unit_request *request)
{
	long state_name;

	switch (*(short *)((byte *)request + 4))
	{
	case 0:
		state_name = 0xf000545;
		break;
	case 1:
		state_name = 0x10000546;
		break;
	case 2:
		state_name = 0x11000555;
		break;
	case 3:
		state_name = 0x12000547;
		break;
	default:
		return false;
	}

	byte state = *((byte *)UNIT_ACTION_UNIT_GET(unit_index) + 0x3dc);
	long action_name = state == 5 || state == 4 ? 0x400000c : 0x5000049;

	return function_ee090(unit_index, state_name, 5, action_name, (point3f const *)((byte *)request + 8),
		(vector3f const *)((byte *)request + 0x14));
}

/* sits a unit down from its pose, keeping a vector (type 46) */
// @retail 0xecd50
bool __stdcall function_ecd50(long unit_index, s_unit_request *request)
{
	bool result = false;

	if (function_ec050(unit_index, request))
	{
		s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));

		actions->active[1] |= 0x4000;
		actions->unknown20 = *(vector3f *)((byte *)request + 4);
		return true;
	}
	return result;
}

bool function_fa1a0(real speed, real gravity_scale, point3f const *origin, point3f const *target,
	real *minimum_speed, real const *time_scale, real const *forced_speed, bool high_arc, vector3f *direction,
	real *speed_out, real *time_out, real *distance, real *vertical_speed, real *horizontal_speed);
void function_b7740(long object_index, vector3f const *linear_velocity, vector3f const *angular_velocity,
	bool unknown);
void function_1c4b00(long object_index, void *a, void *b, long c);

/* the pose sat down from finished: the unit leaps to the kept point on a
   ballistic arc */
// @retail 0xecdc0
void __stdcall function_ecdc0(long unit_index, long type)
{
	function_ec0d0(unit_index, 0x6000086, 0x400000c);

	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));
	point3f center;
	real minimum_speed;
	real speed;
	vector3f velocity;

	function_b9dd0(unit_index, &center);
	function_fa1a0(1.0f, 1.0f, &center, (point3f const *)&actions->unknown20, &minimum_speed, NULL, NULL,
		false, &velocity, &speed, NULL, NULL, NULL, NULL);
	if (function_fa1a0(minimum_speed + 0.01f, 1.0f, &center, (point3f const *)&actions->unknown20,
		NULL, NULL, NULL, false, &velocity, &speed, NULL, NULL, NULL, NULL))
	{
		velocity.i *= speed;
		velocity.j *= speed;
		velocity.k *= speed;
		function_b7740(unit_index, &velocity, NULL, false);
		function_1c4b00(unit_index, &velocity, NULL, 1);
		if (velocity.k * velocity.k + velocity.j * velocity.j + velocity.i * velocity.i > 0.0001f)
		{
			function_b9b90(unit_index, false);
			function_b7360(unit_index);
			function_bba20(unit_index);
		}
	}
}

/* a unit's evade (type 48): its animation, facing the request's vector */
// @retail 0xecf30
bool __stdcall function_ecf30(long unit_index, s_unit_request *request)
{
	bool result = false;

	if (function_110ab0(unit_index))
		return result;
	if (UNIT_ACTION_UNIT_GET(unit_index)->object_type == 0 && function_e4050(unit_index))
		return result;
	if (!function_10f430(unit_index, 0x7000101, 0x7000101, 0x7000101, 0xa00003e, 0.1f, 0, 0))
		return result;
	if (*((bool *)request + 4))
		function_edfa0(unit_index, (point2f const *)((byte *)request + 8));
	return true;
}

bool __stdcall function_e4770(long unit_index);

// @retail 0xecfc0
void __stdcall function_ecfc0(long unit_index, long type)
{
	if (UNIT_ACTION_UNIT_GET(unit_index)->object_type == 0)
		function_e4770(unit_index);
}

void function_11d820(quaternionf const *a, quaternionf const *b, quaternionf *out);
extern vector3f *g_4687ac;

/* hoists a unit onto a ledge (type 49): its animation by the hoist marker,
   facing away from the ledge, its seat transform kept relative to the new
   frame */
// @retail 0xecff0
bool __stdcall function_ecff0(long unit_index, s_unit_request *request)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	transform4x3f object_matrix;
	s_object_marker marker;
	bool result = false;

	function_ba160(unit_index, &object_matrix);
	if (unit->object_type == 0)
		function_1e54d0((byte *)UNIT_ACTION_UNIT_GET(unit_index) + 0x3dc, 1);

	long name;

	switch (*(short *)((byte *)request + 6))
	{
	case 0:
		if (function_b8d30(unit_index, 0xf0005b4, &marker, 1, false) <= 0)
			return false;
		name = 0xc0005b7;
		break;
	case 1:
		if (function_b8d30(unit_index, 0xe0005b5, &marker, 1, false) <= 0)
			return false;
		name = 0xb0005b8;
		break;
	default:
		return false;
	}
	if (!function_10f430(unit_index, 0x6000086, 0x7000101, 0x7000101, name, 0.1f, 0, 0))
		return false;
	result = true;

	vector3f forward;
	vector3f up = object_matrix.up;

	if (unit->parent_object_index == NONE)
	{
		vector3f away;
		vector3f left;

		away.i = marker.matrix.up.i * -1.0f;
		away.j = marker.matrix.up.j * -1.0f;
		away.k = marker.matrix.up.k * -1.0f;
		left.i = up.j * away.k - up.k * away.j;
		left.j = up.k * away.i - up.i * away.k;
		left.k = up.i * away.j - up.j * away.i;
		if (0.0f >= function_30bf0(&left))
		{
			up = *g_4687b0;
			left = *g_4687ac;
		}
		forward.i = left.j * up.k - left.k * up.j;
		forward.j = left.k * up.i - up.k * left.i;
		forward.k = up.j * left.i - left.j * up.i;
		function_b75a0(unit_index, NULL, &forward, &up, NULL, false);
	}

	byte *seat_transform = (byte *)unit + *(short *)((byte *)unit + 0x10e);
	transform4x3f frame;
	vector3f offset = *(vector3f *)(seat_transform + 0x10);
	vector3f world;
	vector3f local;
	quaternionf object_orientation;
	quaternionf local_2e3e32;
	quaternionf orientation;

	frame.scale = 1.0f;
	frame.forward = forward;
	frame.left.i = up.j * forward.k - up.k * forward.j;
	frame.left.j = up.k * forward.i - forward.k * up.i;
	frame.left.k = forward.j * up.i - up.j * forward.i;
	frame.up = up;
	frame.position = object_matrix.position;
	if (object_matrix.scale != 1.0f)
	{
		offset.i *= object_matrix.scale;
		offset.j *= object_matrix.scale;
		offset.k *= object_matrix.scale;
	}
	world.i = object_matrix.left.i * offset.j + object_matrix.up.i * offset.k + object_matrix.forward.i * offset.i +
		object_matrix.position.x - frame.position.x;
	world.j = object_matrix.left.j * offset.j + object_matrix.up.j * offset.k + object_matrix.forward.j * offset.i +
		object_matrix.position.y - frame.position.y;
	world.k = object_matrix.left.k * offset.j + object_matrix.up.k * offset.k + object_matrix.forward.k * offset.i +
		object_matrix.position.z - frame.position.z;
	local.i = frame.forward.k * world.k + frame.forward.j * world.j + frame.forward.i * world.i;
	local.j = frame.left.k * world.k + frame.left.j * world.j + frame.left.i * world.i;
	local.k = frame.up.k * world.k + frame.up.j * world.j + frame.up.i * world.i;
	function_141f60(&object_matrix.rotation, &object_orientation);
	function_141f60(&frame.rotation, &local_2e3e32);
	local_2e3e32.w *= -1.0f;
	function_11d820((quaternionf const *)seat_transform, &object_orientation, &orientation);
	function_11d820(&orientation, &local_2e3e32, &orientation);
	*(vector3f *)(seat_transform + 0x10) = local;
	*(quaternionf *)seat_transform = orientation;

	actions->hoist_ticks = *(short *)((byte *)request + 4);
	if (actions->hoist_ticks <= 0)
		actions->hoist_ticks = g_510c54->field_2_3 << 1;
	actions->active[1] |= 0x20000;
	return result;
}

/* a ledge hoist: its time, then the climb up */
// @retail 0xed560
bool __stdcall function_ed560(long unit_index, long type)
{
	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));
	bool result = false;
	long state_name;
	long action_name;

	function_10f630(unit_index, &state_name, &action_name);
	if ((state_name == 0xc0005b7 || state_name == 0xb0005b8) && actions->hoist_ticks > 0)
	{
		if (--actions->hoist_ticks > 0)
			return true;
		function_10f430(unit_index, 0x6000086, 0x7000101, 0x7000101, 0xf00003a, 0.1f, 0, 0);
	}
	return result;
}

/* plays a unit's surprise animation, and when it ends */
// @retail 0xed600
bool function_ed600(long unit_index, s_unit_actions *actions)
{
	c_animation_channel *channel = NULL;

	if (!function_113e90(unit_index, 0x800061e, 0.0f, &channel, 0))
		return false;

	real event_time = channel->get_event_time();

	if (event_time >= 0.0f)
		actions->surprise_ticks = (short)unit_action_round_long((real)g_510c54->field_2_3 * event_time);
	else
		actions->surprise_ticks = g_510c54->field_2_3 * 3;
	return true;
}

/* surprises a unit (type 50): its surprise animation, and what it plays
   when the surprise ends */
// @retail 0xed680
bool __stdcall function_ed680(long unit_index, s_unit_request *request)
{
	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));
	long state_name;
	long action_name;
	bool result;

	actions->surprise_value = *(short *)((byte *)request + 4);
	actions->surprise_type = *(word *)((byte *)request + 6);
	actions->surprise_ticks = NONE;
	function_10f630(unit_index, &state_name, &action_name);
	if (state_name == 0x700005d)
		result = function_e92e0(unit_index, 0x700005c);
	else
		result = function_ed600(unit_index, actions);
	if (result)
		actions->active[1] |= 0x40000;
	return result;
}

void function_201520(short value, word type, long a, long b, long c);

/* a surprise: waiting out a paused animation, then its time, then what
   follows it */
// @retail 0xed710
bool __stdcall function_ed710(long unit_index, long type)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_animation_state *state = (s_animation_state *)((byte *)unit + unit->animation_state_offset);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	long state_name;
	long action_name;

	function_10f630(unit_index, &state_name, &action_name);
	if (state_name == 0x700005c)
	{
		if (!(*((byte *)state + 0x6c) & 1) && TEST_FIELD_BIT(state->channels[0].flag0) &&
			!(state->channels[0].unknown11 & 1) && !(state->channels[0].unknown11 & 8))
		{
			return true;
		}
	}
	else if (state_name == 0x800061e)
	{
		goto tick;
	}
	if (actions->surprise_ticks <= 0)
		return function_ed600(unit_index, actions);

tick:
	if (actions->surprise_ticks <= 0)
		return true;
	if (--actions->surprise_ticks > 0)
		return true;
	if (!((unit->flags_10a >> 2) & 1))
		function_201520(actions->surprise_value, actions->surprise_type, 1, 0, 1);
	actions->surprise_value = NONE;
	actions->surprise_type = NONE;
	return false;
}

/* a unit's flinch (type 51): its animation, and its time */
// @retail 0xed800
bool __stdcall function_ed800(long unit_index, s_unit_request *request)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);

	actions->flinch_ticks = 0;
	if (function_10f430(unit_index, 0x700002c, 0x7000101, 0x7000101, 0x700002c, 0.2f, 0, 0) ||
		function_10f430(unit_index, 0x6000086, 0x7000101, 0x7000101, 0x700002c, 0.2f, 0, 0))
	{
		s_animation_state *state = (s_animation_state *)((byte *)unit + unit->animation_state_offset);
		real event_time = state->channels[0].get_event_time();

		if (event_time >= 0.0f)
		{
			actions->flinch_ticks = (short)unit_action_round_long((real)g_510c54->field_2_3 * event_time);
		}
		else
		{
			actions->flinch_ticks = (short)unit_action_round_long((real)g_510c54->field_2_3 *
				(state->channels[0].get_duration() * 0.33333334f));
		}
	}
	actions->active[1] |= 0x80000;
	return true;
}

void *function_1e4f90(long actor_index);

/* a flinch: its time, then the weapon dropped (or swapped for the actor's
   spare weapon) */
// @retail 0xed910
bool __stdcall function_ed910(long unit_index, long type)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);

	if (--actions->flinch_ticks > 0)
		return true;

	long weapon_index = unit_action_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), false);

	if (weapon_index == NONE)
		return false;
	if (g_4e3b44[UNIT_ACTION_UNIT_GET(weapon_index)->definition_index & 0xffff].bytes[0x12f] & 1)
		return false;

	long actor_index = unit->unknown12c;

	if (actor_index != NONE)
	{
		byte *actor_definition = (byte *)function_1e4f90(actor_index);

		if (actor_definition && *(long *)(actor_definition + 0x3c) != NONE)
		{
			s_unit_action_object_placement data;

			function_b7930(&data, *(long *)(actor_definition + 0x3c), unit_index, NULL);

			long new_weapon_index = function_b7b40(&data);

			if (new_weapon_index != NONE)
			{
				function_cd0c0(unit_index, new_weapon_index, 4);
				return false;
			}
		}
	}

	s_unit_request drop;

	drop.type = 9;
	*((bool *)&drop + 4) = true;
	unit_action_drop_weapon(unit_index, &drop);
	return false;
}

/* a unit's taunt (type 52) */
// @retail 0xeda30
bool __stdcall function_eda30(long unit_index, s_unit_request *request)
{
	bool result = false;

	if (function_10f430(unit_index, 0x6000086, 0x7000101, 0x7000101, 0xc0006b3, 0.2f, 0, 0))
		return true;
	return result;
}

/* a collision result of function_1697c0 (the fields read here) */
struct s_collision_result_1697c0
{
	long type;
	real t;
	point3f point;
	byte unknown14[0x24 - 0x14];
	short unknown24;
	byte unknown26[0x5c - 0x26];
};

bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);
void function_1c1540(long actor_index, long value);
extern vector3f *g_4687a8;

/* plants a unit's deployable weapon: the deployed object at the weapon's
   plant marker (on the ground under it), the weapon dropped and deleted */
// @retail 0xeda70
void __stdcall function_eda70(long unit_index)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	long weapon_index = unit_action_weapon_get(unit, false);

	if (weapon_index != NONE)
	{
		byte *s_type_67e06b = g_4e3b44[UNIT_ACTION_UNIT_GET(weapon_index)->definition_index & 0xffff].bytes;

		if (*(long *)(s_type_67e06b + 0x2f4) != NONE)
		{
			s_object_marker marker;
			point3f position;
			point3f center;
			vector3f forward;
			vector3f up;

			if (function_b8d30(weapon_index, 0xc0006ad, &marker, 1, false) > 0)
			{
				point3f start;
				vector3f probe;
				s_collision_result_1697c0 collision;

				start.x = g_4687b0->i * 0.2f + marker.matrix.position.x;
				start.y = g_4687b0->j * 0.2f + marker.matrix.position.y;
				start.z = g_4687b0->k * 0.2f + marker.matrix.position.z;
				probe.i = g_4687b0->i * -1.0f;
				probe.j = g_4687b0->j * -1.0f;
				probe.k = g_4687b0->k * -1.0f;
				forward = marker.matrix.forward;
				up = marker.matrix.up;
				collision.unknown24 = NONE;
				if (function_1697c0(0x84000d, &start, &probe, NONE, NONE, &collision))
				{
					position = collision.point;
					up = *g_4687b0;
					forward.k = 0.0f;
					if (function_30bf0(&forward) == 0.0f)
						forward = *g_4687a8;
				}
				else
				{
					function_b9dd0(unit_index, &center);
					function_b9fc0(unit_index, &forward, &up);
					position = marker.matrix.position;
					position.z = center.z;
				}
			}
			else
			{
				point3f weapon_center;

				function_b9dd0(weapon_index, &weapon_center);
				function_b9dd0(unit_index, &center);
				function_b9fc0(unit_index, &forward, &up);
				position = weapon_center;
				position.z = center.z;
			}

			s_unit_request drop;

			drop.type = 9;
			*((bool *)&drop + 4) = true;
			if (unit_action_drop_weapon(unit_index, &drop))
			{
				s_unit_action_unit *current = UNIT_ACTION_UNIT_GET(unit_index);
				s_unit_action_object_placement data;

				function_b7930(&data, *(long *)(s_type_67e06b + 0x2f4), NONE, NULL);
				data.position = position;
				data.forward = forward;
				data.up = up;

				long deployed_index = function_b7b40(&data);

				if (deployed_index != NONE && current->unknown12c != NONE)
				{
					s_unit_request pause;

					pause.type = 0x17;
					*((bool *)&pause + 4) = false;
					function_e9370(deployed_index, &pause);
					function_1c1540(current->unknown12c, deployed_index);
					function_b8540(weapon_index);
				}
			}
		}
	}
	actions->weapon_planted = true;
}

real normalize2d(point2f *v);

/* plants a unit's deployable weapon (type 53): its animation, facing the
   request's direction */
// @retail 0xede60
bool __stdcall function_ede60(long unit_index, s_unit_request *request)
{
	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));
	bool result = false;

	if (!function_10f430(unit_index, 0x6000086, 0x7000101, 0x7000101, 0x60006ac, 0.2f, 0, 0))
		return result;
	actions->active[1] |= 0x200000;
	actions->weapon_planted = false;
	if (*((bool *)request + 4))
	{
		point2f facing = *(point2f *)((byte *)request + 8);

		if (normalize2d(&facing) > 0.0f)
			function_edfa0(unit_index, &facing);
	}
	return true;
}

/* planting: the weapon planted at the animation's plant frame */
// @retail 0xedf10
bool __stdcall function_edf10(long unit_index, long type)
{
	bool done = false;
	long state_name;
	long action_name;

	function_10f630(unit_index, &state_name, &action_name);
	if (state_name != 0x60006ac)
	{
		done = true;
	}
	else if (function_10f720(unit_index, true) == 1)
	{
		function_eda70(unit_index);
		done = true;
	}
	return !done;
}

// @retail 0xedf60
void __stdcall function_edf60(long unit_index, long type)
{
	if (!UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index))->weapon_planted)
		function_eda70(unit_index);
}

/* faces a unit (without a parent) along a horizontal direction */
// @retail 0xedfa0
void function_edfa0(long unit_index, point2f const *facing)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);

	if (unit->parent_object_index == NONE)
	{
		unit->forward.j = facing->y;
		unit->forward.i = facing->x;
		unit->forward.k = 0.0f;
		unit->up = *g_4687b0;
	}
}

/* faces a unit along its aiming direction */
// @retail 0xedff0
void function_edff0(long unit_index)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	point2f facing;

	facing.x = unit->field_xe66477.i;
	facing.y = unit->field_xe66477.j;

	real length = (real)sqrt(facing.y * facing.y + facing.x * facing.x);

	if (!(0.0001f > (real)fabs(length)))
	{
		bool positive = length > 0.0f;
		real inverse = 1.0f / length;

		facing.x = inverse * facing.x;
		facing.y = facing.y * inverse;
		if (positive)
			function_edfa0(unit_index, &facing);
	}
}

void function_b8840(long unit_index);
void __stdcall function_b77d0(long object_index, vector3f const *linear_velocity, vector3f const *angular_velocity);
void __stdcall function_e5690(long unit_index, point3f const *point);
void function_1420f0(transform4x3f *out, point3f const *position, vector3f const *forward,
	vector3f const *up);

/* plays a state animation at a point and facing: a biped keeps its seat
   transform relative to the new frame, anything else is moved there */
// @retail 0xee090
bool __stdcall function_ee090(long unit_index, long state_name, long mode, long action_name, point3f const *point,
	vector3f const *facing)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	bool result = function_10f430(unit_index, state_name, 0x7000101, 0x7000101, action_name, 0.1f, 0, 0);

	if (!result)
	{
		result = function_10f430(unit_index, state_name, 0x7000101, 0x7000101, 0x400000c, 0.1f, 0, 0);
		if (!result)
			return result;
	}
	function_b9a50(unit_index);
	if (unit->object_type != 0)
	{
		if ((unit->flags_c0 >> 6) & 1)
		{
			function_b8840(unit_index);
			function_b77d0(unit_index, g_4687a4, g_4687a4);
		}
		function_b75a0(unit_index, point, facing, g_4687b0, NULL, false);
	}
	else
	{
		vector3f flat;
		transform4x3f object_matrix;
		transform4x3f target;
		vector3f local;
		quaternionf object_orientation;
		quaternionf target_orientation;
		quaternionf orientation;

		flat.i = facing->i;
		flat.j = facing->j;
		flat.k = 0.0f;
		function_ba160(unit_index, &object_matrix);
		function_e5690(unit_index, point);
		if (function_30bf0(&flat) > 0.0f)
			function_b75a0(unit_index, NULL, &flat, g_4687b0, NULL, false);

		byte *seat_transform = (byte *)unit + *(short *)((byte *)unit + 0x10e);
		vector3f offset = *(vector3f *)(seat_transform + 0x10);
		point3f world;

		function_1420f0(&target, point, facing, g_4687b0);
		if (object_matrix.scale != 1.0f)
		{
			offset.i = object_matrix.scale * offset.i;
			offset.j = object_matrix.scale * offset.j;
			offset.k = object_matrix.scale * offset.k;
		}
		world.x = object_matrix.up.i * offset.k + object_matrix.left.i * offset.j + object_matrix.forward.i * offset.i +
			object_matrix.position.x;
		world.y = object_matrix.up.j * offset.k + object_matrix.left.j * offset.j + object_matrix.forward.j * offset.i +
			object_matrix.position.y;
		world.z = object_matrix.up.k * offset.k + object_matrix.left.k * offset.j + object_matrix.forward.k * offset.i +
			object_matrix.position.z;
		if (target.scale != 0.0f)
		{
			vector3f delta;

			delta.i = world.x - target.position.x;
			delta.j = world.y - target.position.y;
			delta.k = world.z - target.position.z;
			if (target.scale != 1.0f)
			{
				real inverse = 1.0f / target.scale;

				delta.i = inverse * delta.i;
				delta.j = inverse * delta.j;
				delta.k = inverse * delta.k;
			}
			local.i = target.forward.k * delta.k + target.forward.j * delta.j + target.forward.i * delta.i;
			local.j = target.left.k * delta.k + target.left.j * delta.j + target.left.i * delta.i;
			local.k = target.up.k * delta.k + target.up.j * delta.j + target.up.i * delta.i;
		}
		else
		{
			local.i = 0.0f;
			local.j = 0.0f;
			local.k = 0.0f;
		}
		function_141f60(&object_matrix.rotation, &object_orientation);
		function_141f60(&target.rotation, &target_orientation);
		target_orientation.w *= -1.0f;
		function_11d820((quaternionf const *)seat_transform, &object_orientation, &orientation);
		function_11d820(&orientation, &target_orientation, &orientation);
		*(vector3f *)(seat_transform + 0x10) = local;
		*(quaternionf *)seat_transform = orientation;
	}
	actions->unknown36 = (short)mode;
	return result;
}

/* puts a unit back on its animation graph's root: placed where its root
   node is, facing the root's horizontal forward, its state animations, and
   the graph's default seat transform */
// @retail 0xee460
bool __stdcall function_ee460(long unit_index, long state_name, long action_name)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	byte *definition = g_4e3b44[unit->definition_index & 0xffff].bytes;
	byte *model = g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes;
	byte *graph = g_4e3b44[*(long *)(model + 4) & 0xffff].bytes;
	byte *root = *(byte **)(graph + 0x4c);
	transform4x3f root_matrix = *(transform4x3f *)(root + 0x28);
	point3f root_position = *(point3f *)(root + 0xc);
	quaternionf root_orientation = *(quaternionf *)(root + 0x18);
	s_unit_action_unit *current = UNIT_ACTION_UNIT_GET(unit_index);
	transform4x3f const *nodes = (transform4x3f const *)((byte *)current + *(short *)((byte *)current + 0x116));
	point3f position = nodes->position;
	transform4x3f matrix;
	vector3f forward;

	position.z = position.z - root_position.z;
	function_142a60(nodes, &root_matrix, &matrix);
	forward.i = matrix.forward.i;
	forward.j = matrix.forward.j;
	forward.k = matrix.forward.k;

	real length = (real)sqrt(forward.i * forward.i + forward.j * forward.j);

	if (0.0001f > (real)fabs(length))
	{
		forward = *g_4687a8;
	}
	else
	{
		real inverse = 1.0f / length;

		forward.i = inverse * forward.i;
		forward.j = inverse * forward.j;
		forward.k = inverse * 0.0f;
		if (length == 0.0f)
			forward = *g_4687a8;
	}
	function_b75a0(unit_index, &position, &forward, g_4687b0, NULL, false);
	if (state_name != NONE && action_name != NONE)
		function_10f430(unit_index, state_name, 0x7000101, 0x7000101, action_name, 0.0f, 0, 2);

	byte *seat_transform = (byte *)unit + *(short *)((byte *)unit + 0x10e);

	*(point3f *)(seat_transform + 0x10) = root_position;
	*(quaternionf *)seat_transform = root_orientation;
	function_bd020(unit_index);
	return true;
}

void function_168896(long user_index, long weapon_index, long field_x11c898, long state);

/* sets a user's first person weapon state, with the unit's matching
   requests (types 54 to 59) and ready ticks */
// @retail 0xee680
void function_ee680(long user_index, long state, long secondary, long weapon_index)
{
	if (user_index != NONE)
	{
		long player_index = g_4e8c20->entries[user_index];

		if (player_index != NONE)
		{
			long unit_index = *(long *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x2c);

			if (unit_index != NONE)
			{
				switch (state)
				{
				case 1:
				case 2:
				case 3:
				case 4:
				{
					byte *unit = (byte *)UNIT_ACTION_UNIT_GET(unit_index);

					if (!secondary)
						unit[0x1fa] = unit_action_round((real)g_510c54->field_2_3 * 0.3f);
					else
						unit[0x1fb] = unit_action_round((real)g_510c54->field_2_3 * 0.3f);
					break;
				}
				case 14:
					function_e68c0(secondary ? 0x39 : 0x36, unit_index);
					break;
				case 15:
					function_e68c0(secondary ? 0x3b : 0x38, unit_index);
					break;
				}
			}
		}
	}
	function_168896(user_index, weapon_index, secondary, state);
}

/* sets the first person state of a unit's weapon (its player's view) */
// @retail 0xee7f0
void function_ee7f0(long unit_index, long state)
{
	byte *object = (byte *)function_badc0(unit_index, 3);
	long user_index;

	if (!object || *(long *)(object + 0x13c) == NONE)
	{
		user_index = NONE;
	}
	else
	{
		byte *current = (byte *)function_badc0(unit_index, 3);
		long player_index = current ? *(long *)(current + 0x13c) : NONE;

		user_index = unit_action_player_value28_get(player_index);
	}

	bool secondary = state == 0x13 || state == 0x15 || state == 0x19;

	function_168896(user_index, unit_action_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), secondary),
		secondary, state);
}


// Disabled: protected caller supplies flags without the required weapon index.
#if 0
void __stdcall function_104080(long object_index);
void function_b7360(long object_index);
long function_101f20(long weapon_index);
bool function_cd660(long unit_index);
bool function_1cd070(void *state, long animation, long first, long second, long third, long flags, long mode);
long function_1039a0(long object_index, long tag_index, long effect_index, real scale_a, real scale_b);
short function_1685a6(long weapon_index, long animation, bool ready);

// Retail 0xfff40
void __stdcall function_fff40(long weapon_index, bool silent, bool immediate)
{
    byte *object = *(byte **)(g_4e0300->data + (weapon_index & 0xffff) * 12 + 8);
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    function_104080(weapon_index);
    byte *current = *(byte **)(g_4e0300->data + (weapon_index & 0xffff) * 12 + 8);
    function_b7360(weapon_index);
    byte *active = *(byte **)(g_4e0300->data + (weapon_index & 0xffff) * 12 + 8);
    if (*(short *)(active + 0x12a) != NONE)
    {
        void *state = current + *(short *)(current + 0x12a);
        long kind = 0x07000001;
        long unit = function_101f20(weapon_index);
        if (unit != NONE && function_cd660(unit)) kind = 0x0400054b;
        if (function_1cd070(state, 0x05000024, 0x07000101, kind, 0x07000001, 0x82, 0x3f)
            || (kind == 0x0400054b && function_1cd070(state, 0x05000024, 0x07000101, 0x07000001, 0x07000001, 0x82, 0x3f)))
            *(long *)(current + 0x178) = 9;
    }
    active = *(byte **)(g_4e0300->data + (weapon_index & 0xffff) * 12 + 8);
    volatile bool selected;
    if ((active[0x12c] & 1) && *(long *)(active + 0x154) != NONE)
    {
        long unit = *(long *)(active + 0x154);
        byte *owner = *(byte **)(g_4e0300->data + (unit & 0xffff) * 12 + 8);
        short slot = *(signed char *)(owner + 0x212);
        long selected_weapon = slot == NONE ? NONE : *(long *)(owner + slot * 4 + 0x218);
        selected = weapon_index == selected_weapon;
    }
    if (!silent) function_1039a0(weapon_index, *(long *)(definition + 0x144), NONE, 0.0f, 0.0f);
    if (immediate)
        *(short *)(object + 0x17c) = 0;
    else
    {
        object[0x16c] &= ~0x20;
        *(short *)(object + 0x19e) = (short)real_to_long((real)*(short *)((byte *)g_510c54 + 2) * *(real *)(definition + 0x138));
        *(short *)(object + 0x17c) = function_1685a6(weapon_index, 0x05000024, true);
    }
}
#endif

