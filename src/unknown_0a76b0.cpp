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
#include "units.h"
#include "matrix_transform.h"
#include "vector_products.h"
#include "normalize3d.h"
#include "unknown_1cec30.h"
#include "bit_vector.h"
#include "local_players.h"
#include "game_engine_events.h"
#include "impacts.h"
#include "object_tag_definitions.h"
#include <math.h>
#include <string.h>

#ifndef NUMBEROF
#define NUMBEROF(array) (sizeof(array) / sizeof((array)[0]))
#endif

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
	short surprise_type;
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
	word flags_c0;
	byte unknown0c2[0xd4 - 0xc2];
	long unknown0d4;
	byte unknown0d8[0x10a - 0xd8];
	word flags_10a;
	byte unknown10c[0x12a - 0x10c];
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
	union
	{
		struct { char current_weapon_index; char current_secondary_weapon_index; };
		char current_weapon_indices[2];
	};
	byte unknown214[2];
	char weapon_slots[2];
	long weapon_object_indices[4];
	long weapon_ready_times[4];
	byte unknown238[0x23c - 0x238];
	char current_grenade_index;
	byte unknown23d;
	char grenade_counts[4];
	byte unknown242[0x33e - 0x242];
	short unknown33e;
	byte unknown340[0x344 - 0x340];
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

/* the weapon in a unit's hand (0 or 1), read through the hand's slot index */
PRIVATE inline long unit_action_hand_weapon_get(s_unit_action_unit *unit, long hand)
{
	short index = (&unit->current_weapon_index)[hand];
	long weapon_index = NONE;

	if (index != NONE)
		weapon_index = unit->weapon_object_indices[index];
	return weapon_index;
}

/* the weapon in a unit's hand (0 or 1), the unit looked up through the pool
   the caller passes */
PRIVATE inline long unit_action_pool_hand_weapon_get(s_record_pool *objects, long unit_index, long hand)
{
	short index = (&((s_unit_action_header *)objects->data)[unit_index & 0xffff].unit->current_weapon_index)[hand];
	long weapon_index = NONE;

	if (index != NONE)
		weapon_index = ((s_unit_action_header *)objects->data)[unit_index & 0xffff].unit->weapon_object_indices[index];
	return weapon_index;
}

/* ends an action (its finished handler), and clears it */
// @retail 0xe6960
void function_e6960(long unit_index, long type)
{
	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));
	s_unit_request_definition *definition = g_4677c8[type];

	if (definition->finished)
		definition->finished(unit_index, type);
	actions->active[type >> 5] &= ~(1 << (type & 0x1f));
}

/* stops an action (its interrupted handler), and clears it */
// @retail 0xe69c0
void function_e69c0(long unit_index, long type)
{
	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));
	s_unit_request_definition *definition = g_4677c8[type];

	if (definition->interrupted)
		definition->interrupted(unit_index, type);
	actions->active[type >> 5] &= ~(1 << (type & 0x1f));
}

/* an animation event of a unit's action */
// @retail 0xe6a20
void function_e6a20(long unit_index, long name, bool active, bool keep)
{
	/* retail keeps active on the stack */
	bool const *active_reference = &active;
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
			if (TEST_FIELD_BIT(UNIT_ACTION_ACTIVE(UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index)), 0x1a)))
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
/* a unit action state, returned by value through the caller's temporary */
struct s_unit_action_state
{
	short value;
	s_unit_action_state(short state) : value(state) {}
};
PRIVATE s_unit_action_state function_e70b0(long unit_index);

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
			if (function_e70b0(unit_index).value != 0)
			{
				if (TEST_FIELD_BIT((actions->active[1] >> 8) & 1))
				{
					function_e6960(unit_index, 0x28);
					next_state = 0x6000542;
					next_action = 0x400000c;
				}
				else if (TEST_FIELD_BIT((actions->active[1] >> 14) & 1))
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
			}
			else
			{
				function_e6960(unit_index, 0x1d);
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
			function_e68c0(0x37, unit_index);
			break;
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
			function_e68c0(0x3a, unit_index);
			break;
		case 0x1000067e:
			function_e68c0(0x3a, unit_index);
			break;
		}
	}

	if (name == 0x6000007)
	{
		s_record_pool *objects = g_4e0300;
		long weapon_index = unit_action_hand_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), 0);

		if (weapon_index != NONE &&
			*(short *)(g_4e3b44[((s_unit_action_header *)objects->data)[weapon_index & 0xffff].unit->definition_index & 0xffff].bytes + 0x292) == 3)
		{
			function_e68c0(0x37, unit_index);
		}
	}
	else if (name == 0xb00053f)
	{
		s_record_pool *objects = g_4e0300;
		long weapon_index = unit_action_hand_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), 1);

		if (weapon_index != NONE &&
			*(short *)(g_4e3b44[((s_unit_action_header *)objects->data)[weapon_index & 0xffff].unit->definition_index & 0xffff].bytes + 0x292) == 3)
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

	memset(actions->active, 0, sizeof(actions->active));
	actions->unknown36 = 0;
}

/* whether a unit can play an animation, from its idle state or (when it
   asks) its alternate one */
// @retail 0xe7020
bool function_e7020(long unit_index, bool *alternate, long name)
{
	bool result = false;

	if (alternate)
		*alternate = false;
	if (function_10f3b0(unit_index, 0x7000101, name))
	{
		result = true;
	}
	else if (function_10f340(unit_index, 0x7000101, name))
	{
		result = true;
	}
	else if (alternate)
	{
		long first;
		long second;

		if (function_10f630(unit_index, &first, &second) && first == 0x6000085 &&
			(function_10f3b0(unit_index, 0x6000086, name) || function_10f340(unit_index, 0x6000086, name)))
		{
			result = true;
			*alternate = true;
		}
	}
	return result;
}

/* the unit's action state (its actions' +0x36): stores it and returns where
   it went, so callers test it through the result */
// @retail 0xe70b0
PRIVATE s_unit_action_state function_e70b0(long unit_index)
{
	return s_unit_action_state(UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index))->unknown36);
}

/* readies the weapon in a unit's hand (the second hand when it's dual
   wielding): the ready animation, unless it's already ready */
// @retail 0xe7110
bool function_e7110(long unit_index, long secondary, long magazine_index)
{
	bool result = false;
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

	if (!function_110ab0(unit_index))
	{
		s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);

		if (!(TEST_FIELD_BIT((unit->flags_134 >> 25) & 1) ||
			TEST_FIELD_BIT(UNIT_ACTION_ACTIVE(UNIT_ACTIONS_GET(unit), 0x16))))
		{
			if (unit_action_active(unit_index, action_type) &&
				function_cbd50(unit_index, unit->weapon_slots[secondary]) != NONE)
			{
				function_e69c0(unit_index, action_type);
			}
			function_e69c0(unit_index, 0x1a);
			function_e69c0(unit_index, 0x1b);

			long weapon_index = function_cbd50(unit_index, (&UNIT_ACTION_UNIT_GET(unit_index)->current_weapon_index)[secondary]);

			if (weapon_index != NONE && function_101490(weapon_index, magazine_index))
			{
				function_c86e0(unit_index, 0);
				if (!function_1029d0(weapon_index, (short)magazine_index))
					function_113e90(unit_index, name, 0.267f, NULL, mode);
				result = true;
			}
		}
	}
	return result;
}

/* readies a weapon (types 0, 1, 10 and 11: a hand's first or second
   magazine) */
// @retail 0xe7280
bool __stdcall function_e7280(long unit_index, s_unit_request *request)
{
	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));
	bool result = false;
	long secondary = request->type > 1;
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
	if (function_e7110(unit_index, secondary, magazine_index))
	{
		actions->active[request->type >> 5] |= 1 << (request->type & 0x1f);
		result = true;
	}
	return result;
}

// @retail 0xe7320
bool __stdcall function_e7320(long unit_index, long type)
{
	bool result = false;
	long weapon_index;
	long slot_index;

	if (type > 1)
		weapon_index = function_cbd50(unit_index, UNIT_ACTION_UNIT_GET(unit_index)->current_secondary_weapon_index);
	else
		weapon_index = function_cbd50(unit_index, UNIT_ACTION_UNIT_GET(unit_index)->current_weapon_index);

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
	long weapon_index;

	if (type > 1)
		weapon_index = unit_action_hand_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), 1);
	else
		weapon_index = unit_action_hand_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), 0);
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
	long name;

	switch (request->type)
	{
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
	case 2:
		name = 0x6000006;
		break;
	case 3:
		name = 0x6000007;
		break;
	case 4:
		name = 0x9000004;
		break;
	case 5:
		name = 0x9000005;
		break;
	case 6:
		name = 0x9000008;
		break;
	case 7:
		name = 0x9000009;
		break;
	case 12:
		name = 0xb00053e;
		break;
	case 13:
		name = 0xb00053f;
		break;
	case 14:
		name = 0xe00053c;
		break;
	case 15:
		name = 0xe00053d;
		break;
	case 16:
		name = 0xe000540;
		break;
	case 17:
		name = 0xe000541;
		break;
	case 54:
		name = 0xb00006d;
		break;
	case 55:
		name = 0xa000066;
		break;
	case 56:
		name = 0x8000071;
		break;
	case 57:
		name = 0x1000067e;
		break;
	case 58:
		name = 0xf00067f;
		break;
	case 59:
		name = 0xd000680;
		break;
	default:
		__assume(0);
	}
	return function_113e90(unit_index, name, blend, NULL, mode);
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
void __stdcall function_b93b0(long parent_index, long object_index, short node_index);
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
				function_b93b0(unit_index, projectile_index, (short)(marker.node_index));
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
void function_a91c0(long unit_index, short grenade_type, point3f const *origin, vector3f const *forward);
void __stdcall function_b8540(long object_index);
void function_d0e60(long unit_index, real amount, real limit);
point3f *function_b9dd0(long object_index, point3f *result);
long function_1ff360(long actor_index, point3f const *target, vector3f *velocity);
void function_b75a0(long object_index, point3f const *point, vector3f const *forward, vector3f const *up,
	struct s_location const *location, bool unknown);
bool function_109e00(long object_index, vector3f *velocity, bool definition_flag_required);
void function_fa820(long projectile_index, vector3f const *impulse);
bool __stdcall function_bc1d0(long object_index, point3f const *point, long ignore_unit_index);
void __stdcall function_a7870(long object_index);
void function_fd560(long projectile_index, long object_index, short node_index, point3f const *point,
	vector3f const *forward);
extern vector3f *g_4687b0;

/* a unit's vehicle seat definition */
PRIVATE __forceinline byte *unit_action_seat_get(s_unit_action_unit *unit)
{
	byte *parent_definition = g_4e3b44[UNIT_ACTION_UNIT_GET(unit->parent_object_index)->definition_index & 0xffff].bytes;

	return *(byte **)(parent_definition + 0x1cc) + unit->seat_index * 0xb0;
}

/* throws the grenade in a unit's hand (or drops it, or attaches it to the
   unit's vehicle seat) */
// score 92 (was 92): 152 before m3 lane 14 r83; earlier start 288. Left are the origin and direction frame homes (swapped), the projectile index, grenade type
// MiniMax I (11-14): no new form, no lower score (92): the throw path's origin split from the release point's (the
// override copy and the first 0xcafc0 point in a local of their own) 116, and the spread blend with the velocity term
// first is the same code; the permuter over this function found nothing (4 tries, best 89 against a baseline of 89), so
// the origin and direction homes stay swapped and the forward, grenade type and projectile index keep their homes.
// MiniMax B (11-13): no new form, no lower score (92): the origin and direction homes stay swapped and the forward,
// grenade type and projectile index keep their homes (retail 0x20, 0x2c and 0x30 against our 0x28, 0x20 and 0x24);
// the permuter over this function found no form either.
// MiniMax B (11-13): no new form, no lower score (92): aligned the two listings instruction by instruction: the two
// vector groups are exchanged (retail keeps the left vector at 0x54 and the normal one at 0x48, this build keeps them the
// other way round), the grenade type and the projectile index sit 0xc below retail's homes, and the three blend products
// are grouped the way retail groups them (the three right-hand products first, then the three left-hand ones); declaring
// the normal vector before the left one does not move either home (92), so the frame stays a permutation the declaration
// order does not reach.
// score 92 (was 92): 152 before m3 lane 14 r83; earlier start 288. Left are the origin and direction frame homes (swapped), the projectile index, grenade type
// MiniMax H (11-12): no new form, no lower score (92); the permuter over the function managed 17 tries in 301.7 s and found nothing (best 89, baseline 89, no confirming build), so the slot layout stands; this round added the player block's forward vector declared before the origin (92), copied in its declaration (100), and as a function local at the top of the function, above the projectile index and below the grenade type (92 each), which all leave the player's forward at the same home, so retail's forward at 0x20, grenade type at 0x2c and projectile index at 0x30 against our 0x28, 0x20 and 0x24 stands.
// MiniMax G (11-12): no new form, no lower score (92): the seat index and the direction sit 0xc below retail's slots and the
// shared block-vector slot 0xc above them, so the whole frame is a permutation of retail's; the permuter (killed at its time
// limit) produced no form.
// Earlier 92 (was 152), start 288: Left are the origin and direction frame homes (swapped), the projectile index, grenade type
// and throw-globals homes, the release point (retail its own local in center's slot), the velocity scale's first
// product and the spread blend's schedule. Before this round 288, 298 before lane 15 round 61's throw velocity through
// vector3f_scale; earlier, flag 17 cleared and restored through bit_vector_set (retail reads the flags word
// twice and keeps the unit in esi from entry), the empty grenade slot as an early return (retail's late
// `push edi`) and the 0x400 flag set through bit_vector_set (retail's load/add/or/store). Left: the frame
// slots (direction, origin, center, projectile index and the definition sit elsewhere), retail's second
// copy of the throw direction inside the player block (copied with integer moves before both crosses and
// the offset sum, which retail builds in three phases), and the spread blend (velocity*ratio products first).
// MiniMax G (11-12): no new form, no lower score (92): the seat index and the direction sit 0xc below retail's slots and the
// shared block-vector slot 0xc above them, so the whole frame is a permutation of retail's; the permuter (killed at its time
// limit) produced no form.
// Earlier (392): retail keeps the unit pointer in esi from entry (ours keeps the projectile's pool
// offset there and reloads the unit), works on a second copy of the throw direction inside the player
// block (copied with integer moves before both crosses and the offset sum), and blends the spread with
// the velocity*ratio products first.
// Round 46: the grenade type saved before it is cleared and passed to function_a91c0 (retail's argument;
// ours passed the projectile index) 395; with function_a91c0's value parameter a short (retail pushes the
// short slot raw) 392, retail's length; both crosses through cross3f 393; the offset sum through three
// vector3f_add_scaled 396 (frame 0xc0); with the speed scale and parent velocity through vector3f_scale /
// vector3f_add 428; the scaled aim vector through vector3f_scale before the blend 389, with cross3f 390.
// Round 68 (m3 lane 16, fast; 0xe8420/0xa91c0 MATCH each time): the offset sum in three phases by hand 394
// (frame 0xc0), through vector3f_add_scaled 406 (1687 bytes); diagnostics (not kept: a copy of a local) a
// `forward = direction` copy in the player block with the phased sum 395 (frame 0xcc, aligned 275 -> 261),
// with both crosses through cross3f 395; projectile index declared before the definition 392 (same code);
// flag 17 cleared/restored through bit_vector_set 327 (kept); on it the empty slot as an early return 321,
// the 0x400 set through bit_vector_set 324 (1702 bytes), both 318 (kept, plain confirmed). On 318: the
// spread blend on a vector3f_scale'd aim 312 (aligned 227 -> 232), operands swapped 312; velocity before
// origin, both vectors first, attach/flag17 first 318 (same code each).
// B358: Restore/origin/velocity scopes, subsets 1/2/4/3/5/6/7: 318/315/318/315/318/315/315;
// all with native role names 315. Staged crosses alone/with branch origins 319/316;
// staged with scaled aim/with both 313/310; original/direct shared crosses on the
// branch-origin/scaled-aim graph 309/310. Only branch-owned origins and the complete
// scaled aim blend retained. The direct random-call producer removes the one-use
// scale carrier: 309 fast/plain/image-wide at 1699/1699 bytes; no lost match.
// Unit spill now agrees; definition/index and direction homes, offset/SSE scheduling remain.
// Round 326: Remaining tag-pointer/released-index/saved-type lexical roles,
// seven subsets, then all with the uninitialized type declaration before the
// released index: 309 each. All capture points fixed; original complete body
// restored. Definition/index/type/direction homes and offset/SSE schedules remain.
// Round 362: CRT copies of direction inputs / vehicle direction / fallback axis, seven subsets; then all with enclosing direction lifetime.
// 309/309/309/309/309/309/309/308. Original executable source restored.
// J240: Scaled offset vectors / complete spread stages / shared completion 326/310/343; offset+blend /
// offset+completion / blend+completion / all 343/326/344/327; all with shared parent sum 327. All
// restored; tag/index/type/direction homes and SSE scheduling remain.
// P272: Independent entry-owned marker/center/complete offset workspace subsets, then complete workspace role names: 309/309/309/309/309/309/309/309.
// Original executable source restored; frame 0xcc agrees; tag/index/direction homes and offset/SSE schedules remain.
// P281: Existing seat accessor with admission-time attach/marker defaults and complete forward setter,
// eight forms 309/329/340/345/302/326/332/341. NOT kept: 302 gains seven above 30 and grows to
// 1715/1699 bytes. Original accessor expansion, defaults, copies and complete body restored.
// F339: Plain native component traversals for speed/blend/projectile subtraction, seven
// subsets, then all with parent addition: 376/366/328/425/338/353/412/436.
// All shorten away from retail; original body restored. Frame 0xcc agrees;
// definition/index/direction homes and offset/SSE scheduling remain.
// AC493: Player-only second direction copy, native/CRT, with original/first/second/both
// shared crosses: 328/327/327/328/328/327/327/328. Every form grows frame to 0xd8
// versus 0xcc; original copies, crosses, nested offset expression and complete body restored.
// N332: Complete scalar offsets / shared spread complement / existing speed scale,
// seven subsets then all with shared parent sum: 322/312/323/314/320/314/312/312.
// Original fields, scalar producers, vector operations and full body restored;
// frame 0xcc and tag/index/direction spill and SSE schedules remain.
// AC496: Complete parent/seat attachment result, bool/byte/short/long:
// predicate producers 377/377/375/370; exhaustive branches 371/371/376/373.
// NOT kept: no exact match; original default, full body, vector inputs and calls
// restored. Tag/index/type/direction homes and offset scheduling remain.
// P285: Existing complete scalar-output setters at left/normal/position producers,
// subsets 1..7 then all with the initial velocity setter: 309/309/309/309/310/310/309/314.
// Original scalar trees, stores, helpers, scopes and complete body restored.
// Round 78 (m3 lane 16, fast): direction, origin or left renamed 309 (same code: names do not order these
// slots). LEAD: the throw-offset block as a forceinline helper taking the direction by value gives retail's
// second direction copy at retail's slots with up kept in EBX: with the original sum 316 (frame 0xd8), with
// the sum as three in-place vector3f_add_scaled phases 307 (frame 0xcc, 1693 bytes); on that the offsets
// pointer passed as an argument 307 (same code), direction declared beside origin, before or after, 307
// each, the spread blend as vector3f_scale + vector3f_add_scaled 310 (1699 bytes); the phases in place in
// the parameter then copied out 299 (1717 bytes), the helper calling function_b75a0 on it 332 (1684 bytes);
// the helper returning the point by value 295 (1717 bytes: retail writes the point into the copy's slot
// with no copy, and loads the offsets pointer first). Not kept: one-caller helper, no match.
// F349: Actual speed-tag, native seat flags and throw-global receiver subsets 1..7,
// then complete mutable receivers: 309/309/309/309/309/309/309/309. All original
// record receivers and body restored; frame 0xcc agrees, tag/index/direction homes remain.
// P296: Existing bit-vector test for saved flag/word flag clear/seat-bit test,
// seven subsets then all with the saved flag declared at its producer: 309 each.
// Original helpers, predicates and body restored; tag/index/type/direction homes remain.
// Round 81 (m3 lane 16, fast): the definition local renamed to sort before/after the projectile index
// (two spellings) and the projectile index renamed 309/309/309 (names move no slot); velocity declared
// first in the function 309 (same code); one function-scope origin for both throw branches 312.
// Y570: Complete up/side inputs scalar/none, none/scalar, both scalar, record/none,
// none/record, both record and mixed scalar/record, record/scalar:
// 362/310/363/338/392/450/391/338. NOT kept: all regress and grow away from
// 1699 bytes. Original inputs, copies, frame and full body restored.
// P313: Existing scalar setter at blend/parent-add/projectile-subtract boundaries, seven subsets,
// then all with the initial velocity setter: 309/309/309/309/309/309/309/314. All original
// component producers and full body restored; tag/index/type/direction homes remain.
// P319: Saved type promotion / unsigned projectile / unsigned marker domains, masks
// 1/2/4/3/5/6/7, then all with integral attach: 332/309/309/332/332/309/332/390.
// NOT kept: domains tie or regress; wide saved type shortens away from retail, integral
// attach loses the entry. Original scalar types, observations and complete body restored.
// P321: Player-only forward input plus existing phased position producer, native/CRT
// copy, original/staged crosses and from-origin/in-place output:
// 312/309/312/309/349/388/349/388. NOT kept: ties grow, all others regress;
// copy/interactions remain unmatched. Original complete body restored.
// M361: Existing component-array relative-velocity difference producer alone, then parent-
// add/speed-scale/staged-cross subsets:
// 328/328/306/306/321/321/299/299. NOT kept: no eligible byte match or supported closer.
// The 299 form shrinks the body to 1687 versus native 1699; NOT kept. Original full body and helper
// definitions restored; tag/index/type/direction homes and scheduling remain.
// M370: Actual saved/restored bit truth byte/signed-char/short/long at outer scope
// 309/309/378/370; the same closed release/restore scopes 309/309/378/370.
// NOT kept: ties or regressions; original Boolean producer, full body and all helpers
// restored. Frame 0xcc agrees; tag/index/type/direction homes and scheduling remain.
// Round 128 (m3 lane 7, fast; none kept, no match): in the throw block a forward copy with the sum as three
// vector3f_add_scaled phases 312 (1694 bytes), the sum in named scalars 337 (1713), the phases in place in the
// copy 295 (1718); both crosses through cross3f with those sums 350 (1724), 322 (1707), 363 (1748). With the final
// 0xcafc0 point in its own closed scope, the parent velocity in a closed scope and the returns kept: 311 (1693),
// without the copy 312 (1699). The same scopes with one 0xb8540 and one 0xd0e60 call behind `destroy:` and `finish:`
// labels (two gotos, match-only) make every integer line, frame slot and the tail agree with retail: 229 (frame
// 0xcc, 1666 bytes; 107 lines differ when aligned, all SSE: normal cross, sum, speed scale, spread blend). On that:
// `real inverse` in the blend 230 (1666), named offsets 228 (1667), the normal through cross3f 222 (1673), that
// with named offsets 222 (1673), with the inverse 222 (1673), with both 223 (1672), cross3f_staged 222 (1673); the
// normal through cross3f and the speed through vector3f_scale 215 (1681), that with the inverse 217 (1679), with
// the blend through vector3f_scale + vector3f_add_scaled 241 (1655), with `position = origin` and in-place phases
// 220 (1708), with phases in the copy 195 (1705), with named offsets 215 (1681), phases in the copy with the
// inverse 129 (1702); the normal through cross3f with phases in the copy and the plain speed 129 (1699, but forward
// and left swap slots: 142 lines differ when aligned, against 89 for the 230 form), that with the inverse 184 (1699),
// with named offsets and the inverse 186 (1696); both crosses through cross3f 226 (1729), the hand normal 146 (1700).
// One-file LTCG harness, SSE blocks only (normal 14 lines from retail, sum 33): swapped product operands, eight
// declaration orders, a by-value forward helper (out pointer, in-place, returned point), by-value components with
// the scale first or last, value locals for the offsets or the components, reassigned scale locals, unrolled index
// loops, 0x120220's `+=` per component: none closer on either. Retail loads every sum product first
// (`movss x0, [forward.i]; mulss x0, x1; addss x0, [origin.x]; movaps x3, x0`), as matched 0x137800 and 0xc28b0 do;
// ours copies the scale. Dead pointer locals (diagnostic) flip only the aim's first product.
// M3 lane 14 r68 (fast, plain-confirmed 298 at 1673 bytes): retail has one 0xb8540, one 0xa7870 and one 0xd0e60
// call; the placement failure jumps into the release tail. `if (attach || function_bc1d0(...))` after the
// `if (!attach)` block, its failure falling into the existing release tail, 309 -> 298 (kept; every integer
// line of the tail now agrees). Also tried: one `thrown` flag and a single `if (!thrown) b8540` tail 310 (1665,
// flag form); on 298, the direction copied once from a per-branch source pointer 330 (1644; aligned 221 -> 174),
// the offset sum as three vector3f_add_scaled phases 312 (frame 0xc0); source pointer + a player-block direction
// copy + the phases in place in the copy 308 (1662, frame 0xcc, aligned 137; copy and one-use pointer: diagnostic);
// velocity declared inside the not-attached block 298 (same code); up declared before the throw globals 298 (same code).
// Lane 15 round 61 (kept, plain): the throw velocity through the existing vector3f_scale(&direction, speed, &velocity)
// 298 -> 288 (1682 bytes): velocity is then not written field by field before the blend, whose velocity products
// load first as in retail. Also tried (fast): the release point (the final 0xcafc0 point for 0xbc1d0) as its own
// local apart from the throw origin (retail homes them at [esp+0x24], shared with normal, and [esp+0x54]) 309 (frame
// 0xd8: at the else-if scope it cannot share); on 288: the blend through vector3f_scale + vector3f_add_scaled 316
// (1655), `real inverse` in the blend 293 (1678), the normal through cross3f 288 (same code), up read through
// g_4687b0 at each use 288 (same code), left through cross3f 292.
// b5R (r725 walk, no change, 288): every block checked against retail, no behaviour or precision difference
// found: projectile/type/clear and flag-17 save-restore 0xe7952-0xe79c6; seat test and marker name 0xe79cf-0xe7a29;
// 0xa76b0 branch 0xe7a3a-0xe7a7b; cancelled test 0xe7aa6; attach skip 0xe7ab1; unknown12c center 0xe7ab9-0xe7af6;
// override test (both non-null) 0xe7afb-0xe7b2a; unknown13c guard 0xe7b49; left/normal crosses, `== 0.0f` fallback
// (0x45dbd8 is 0.0) and the ((dir*fwd + origin) + left*lo) + normal*uo sum 0xe7b75-0xe7d0b; velocity scale 0xe7d57;
// spread (signed-byte counts, divss, comiss 1.0, 0x259d0 rounded by fstp dword 0xe7deb, 1-ratio once) 0xe7d92-0xe7e79;
// parent add, projectile subtraction, 0xfa820, release point, 0xbc1d0 0xe7e7c-0xe7f2a; attach marker 0xe7f34-0xe7f9e.
// Remaining differences are stack slots (retail shares one slot among center, normal, parent velocity and the release
// point, direction at [esp+0x48]) and SSE schedule. Retail copies the direction once after the branches (both leave
// eax at the source, dword loads at 0xe7b44-0xe7b50): a `vector3f const *source` chosen per branch and
// `vector3f direction = *source` after them gives 323 (1652 bytes), the compiler copies per branch again (diagnostic).
// m3 lane 11 r83 (fast, plain-confirmed; aligned/masked counts, 288's were 240/94): KEPT 152 (1705 B, frame 0xcc,
// aligned 90, masked 24): a throw-block copy `vector3f forward = direction;` (retail's 0x38 copy is a promoted variable:
// its integer-register spills before each first float use now agree instruction for instruction), the offset sum as
// three vector3f_add_scaled phases on the point (the cast idiom of matched 0x117790) and both crosses through cross3f.
// Steps: the copy alone 280 (frame 0xd8, aligned 248/86); the three offsets read once into named locals 296 (223/88);
// both 260 (frame 0xd8, 178/63); those with the sum in three phases by hand 243 (frame 0xcc, 150/59; a helper's body
// written out, not kept); the three vector3f_add_scaled phases without the copy 303 (frame 0xc0), with it 243;
// offset-first products, the position declared at the sum, one function-scope origin 260/260/260 (same code); on 243
// the normal through cross3f 238, the left 247, both 152. On 152: the parent velocity through vector3f_add 153 (masked
// 20); the blend as vector3f_scale(&velocity, ratio) + vector3f_add_scaled(aim, 1 - ratio) 162 (aligned 77), as
// vector3f_scale(&aim, 1 - ratio) + vector3f_add_scaled(velocity, ratio) 164 (82). Match-only, not kept (a `placed`
// result, the parent velocity and the release point in bare blocks, one function-scope origin): on 152 131 (aligned
// 61); with the first blend 140 (50/26); with the parent add too 128 (38/16); the blend as aim*(1-ratio), velocity*ratio,
// vector3f_add 128 (43/13); velocity*ratio first 129 (54/6: left are the origin/direction homes, the velocity scale's
// folded first product and the blend's ratio register); on that: direction or origin renamed 129/129 (the homes do not
// follow names), direction declared at function scope before/after origin 128/128, at the not-attached block 129, the
// override copies swapped 285, a source pointer per branch with one `vector3f direction = *aim` 222 (1699 B, every slot
// shifts), the left cross on direction 208, the velocity scale by hand 123 (homes still swapped); with the scale by
// hand (a helper body, match-only): the blend as aim*(1-ratio), velocity*ratio, vector3f_add 65 (1699 B, aligned 46,
// masked 16), as vector3f_scale(&aim, 1 - ratio) + vector3f_add_scaled(velocity, ratio) 61 (1700 B, 43/17; best this
// round: left are the origin/direction homes, the velocity scale's folded first product and the blend's registers), the
// a76b0 forward at function scope 123, velocity declared after origin 123/65, without the parent add 123/65/61, the
// speed read at each use 148, direction-first products 65, separate origins 246. On 288 earlier: the
// release point as its own local with a `placed` result 313 (frame 0xd8), with the parent velocity in a block 313, with
// the release point in a block 309 (frame 0xcc), with one origin 287, with the copy 279 (frame 0xd8); one origin 295.
// m3 lane 2 r69 (fast, plain-confirmed 152): the seat test through the existing unit_action_seat_get (its body was
// written out here; the helper now sits above this function) 152, the same code: kept. Retail's frame puts the block
// vectors (the a76b0 forward copy, center, ...) below the grenade type and projectile index slots, ours above
// (slot order follows use weight, not names). Tried: the a76b0 branch passing the unit's facing straight to 0xa91c0
// 341 (1682 bytes: retail copies it); the parent velocity declared at the top of the not-cancelled block 152; the
// projectile subtraction as vector3f_add_scaled(-1) 152 (same code); the blend with the velocity term first 152.
// m3 lane 2 r71 (fast with 0xe8420/0xa91c0): retail's shared block-vector slot (the a76b0 forward, center, parent
// velocity, release point) sits at [esp+0x24] below the grenade type and projectile index, ours at [esp+0x2c] above
// them. Tried: the a76b0 branch's forward declared first, or at its copy 152/152 (same code); the grenade type read
// before the projectile test 191 (1708 bytes); the projectile pointer before the grenade type 170; flag17 initialised
// at its declaration 152 (same code); the seat test as nested ifs 152 (same code); marker_name stored before attach
// 154; the throw globals read after the up pointer 276 (1708); one 0xd0e60 call with 0xb8540 in each failing arm
// (no flag, no goto) 337 (1716).
// MiniMax lane A (11-04): no new form, no lower score.
// M3 lane 14 r83 (fast, plain-confirmed 92; 0xe8420/0xa91c0 MATCH): KEPT `real aim_weight = 1.0f - ratio;` named after the
// aim scale and used in the three blend rows 152 -> 94, then the parent add through the existing vector3f_add (the
// written-out sum it replaces) 94 -> 92 (1703 B, masked 60). Not kept (one-use weight): on 92 the blend as
// vector3f_scale(&aim, aim_weight), vector3f_scale(&velocity, ratio), vector3f_add(&aim, &velocity) 75 (1700 B, masked 49),
// with `1.0f - ratio` written in the call 149 (masked 50), velocity scaled first 75 (masked 51), velocity first with the
// weight in the call 150; without the vector3f_add parent add that blend is 148 (1673 B). Tried: the weight declared before
// the aim scale 172 (the frame grows), with the helper blend 172, with velocity first too 172; the throw globals read after
// the crosses 265 (1699 B); one function-scope origin alone 153; `real speed` / `real aim_speed` named for the two scale
// calls 143/142; on 94 the blend as vector3f_scale + vector3f_add_scaled 159; on 92 that blend 92 (different code), the
// named aim_speed or speed 142. Same code: parent_velocity, velocity, center or aim at wider scopes, flag17 declared at its
// producer, the a76b0 or throw-block declarations reordered, `ratio < 1.0f`, the projectile subtraction through
// vector3f_add_scaled(-1), the vector3f_add operands swapped. Match-only lead on the 75 form: one function-scope origin,
// `bool placed = true;` set from function_bc1d0, the release point and the parent velocity in bare blocks 41 (1699 B, first
// difference +0x146); left there are the origin/direction homes (0x48/0x54 swapped), the velocity scale's first product
// (retail folds dir.i) and the blend's 1.0/ratio registers; on it a separate a76b0 origin 44, `1.0f - ratio` in the call
// 128, that with velocity first 129, velocity first 41.
// m3 lane 1 r95 (fast; kept: the throw speed read as the typed unit definition's grenade velocity, same code 92). Rebuilt
// lane 14 r83's match-only lead (one function-scope origin, `placed` from function_bc1d0, release point and parent
// velocity in bare blocks, the helper blend) at 41 (1699 B, register-masked aligned 22); on it: direction declared at
// function scope beside the origin 41 (same code); origin renamed to sort before direction 41 (same code: names do not
// order these slots); the permuter (928 s, 28 tries) best 41; the aim copied from the field and scaled in place 168
// (1720 B); the random aim scale named before its scale call 46 (1700 B, masked aligned 19: the aim products then load
// all three components as in retail, and the one extra byte is the velocity scale's unfolded first product, which shifts
// every later call and jump); that with the throw speed named at its scale call 41 (the aim fold comes back: the two
// fold choices move together); that with the blend weight computed before the aim scale 53.
// m3 lane 4 r191 (fast; 0xe8420/0xa91c0 MATCH; aligned/masked 70/14 at 92): the placement failure as an early return inside
// the not-attached block (origin scoped there, its own b8540/d0e60 tail) 356 (1730 B: the two tails are not cross-jumped);
// the a76b0 branch passing the throw path's direction (one function-scope direction) 105; origin and direction both at
// function scope, shared with the a76b0 branch, 103; the throw offsets as three vector3f_add_scaled phases in place on
// origin (no position local) 248 (1739 B); the not-attached arms swapped (`unit->unknown12c == NONE` first; retail lays the
// center arm first) 255. Diagnostic only: the blend through a private copy of the static vector3f_interpolate of
// src/unknown_1a58b0.cpp (an Avoid file; its body is this blend) 95.
// P439: Standard complete coordinate transfers at override origin/direction, fallback direction,
// vehicle/player forward and fallback left, then backward override-origin/player-forward:
// 270/312/308/130/313/171/270/313. NOT kept: range transfers do not recover native
// saved-handle/type and shared-vector homes; each adds a copy call (21 versus 20).
// Original copies and full body restored.
// P473: Immutable/native-component player forward, then mutable components, original-boundary
// override-origin/direction, fallback-direction/axis setters and immutable vehicle forward:
// 92/291/291/286/337/343/234/92. NOT kept: input ownership does not match; original complete body restored.
// @retail 0xe7900
void __stdcall function_e7900(long unit_index, bool spread, point3f const *origin_override,
	vector3f const *direction_override)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	s_unit_definition *local_98b918 = (s_unit_definition *)g_4e3b44[unit->definition_index & 0xffff].bytes;
	long projectile_index = actions->grenade_projectile_index;

	if (projectile_index == NONE)
	{
		actions->grenade_state = 3;
		return;
	}

	short grenade_type = actions->grenade_type;
	s_unit_action_unit *projectile = UNIT_ACTION_UNIT_GET(projectile_index);
	long marker_name = NONE;
	bool attach = false;
	bool flag17;
	vector3f velocity;

	actions->grenade_projectile_index = NONE;
	actions->grenade_type = NONE;
	flag17 = TEST_FIELD_BIT((projectile->object_flags >> 17) & 1);
	bit_vector_set(&projectile->object_flags, 17, false);
	projectile->flags_c0 &= ~2;
	function_b9a90(projectile_index);
	bit_vector_set(&projectile->object_flags, 17, flag17);

	if (unit->parent_object_index != NONE && unit->seat_index != NONE)
	{
		byte *seat = unit_action_seat_get(unit);

		if (TEST_FIELD_BIT((*(dword *)seat >> 11) & 1))
		{
			attach = true;
			marker_name = *(long *)(seat + 0x10);
		}
	}

	if (function_a76b0(unit_index, 1))
	{
		point3f origin;
		vector3f forward;

		function_cafc0(unit_index, &origin);
		forward = unit->field_xe66477;
		function_a91c0(unit_index, grenade_type, &origin, &forward);
	}
	else if (!actions->grenade_cancelled)
	{
		point3f origin;
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
					vector3f forward = direction;
					vector3f left;
					vector3f normal;
					point3f position;

					cross3f(up, &forward, &left);
					if (function_30bf0(&left) == 0.0f)
						left = *up;
					cross3f(&forward, &left, &normal);
					function_30bf0(&normal);
					vector3f_add_scaled((vector3f const *)&origin, &forward, throw_globals->forward_offset, (vector3f *)&position);
					vector3f_add_scaled((vector3f const *)&position, &left, throw_globals->left_offset, (vector3f *)&position);
					vector3f_add_scaled((vector3f const *)&position, &normal, throw_globals->up_offset, (vector3f *)&position);
					function_b75a0(projectile_index, &position, NULL, NULL, NULL, false);
					spread = false;
				}

				vector3f_scale(&direction, local_98b918->unit.grenade_velocity, &velocity);
			}

			if (spread && actions->grenade_throw_maximum > 0)
			{
				real ratio = (real)actions->grenade_throw_count / (real)actions->grenade_throw_maximum;

				if (1.0f > ratio)
				{
					vector3f aim;

					vector3f_scale(&unit->field_xe66477, function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, 0.6f, 1.4f), &aim);
					real aim_weight = 1.0f - ratio;

					velocity.i = aim.i * aim_weight + velocity.i * ratio;
					velocity.j = aim.j * aim_weight + velocity.j * ratio;
					velocity.k = aim.k * aim_weight + velocity.k * ratio;
				}
			}

			vector3f parent_velocity;

			if (function_109e00(unit_index, &parent_velocity, true))
				vector3f_add(&velocity, &parent_velocity, &velocity);
			velocity.i -= projectile->linear_velocity.i;
			velocity.j -= projectile->linear_velocity.j;
			velocity.k -= projectile->linear_velocity.k;
			function_fa820(projectile_index, &velocity);
			function_cafc0(unit_index, &origin);
		}

		if (attach || function_bc1d0(projectile_index, &origin, unit_index))
		{
			function_a7870(projectile_index);
			if (attach)
			{
				s_object_marker marker;

				function_b8d30(unit->parent_object_index, marker_name, &marker, 1, false);
				bit_vector_set((dword *)((byte *)UNIT_ACTION_UNIT_GET(projectile_index) + 0x12c), 10, true);
				function_fd560(projectile_index, unit->parent_object_index, marker.node_index, &marker.matrix.position, NULL);
			}
			function_d0e60(unit_index, 0, 0);
			actions->grenade_state = 3;
			return;
		}
	}
	function_b8540(projectile_index);
	function_d0e60(unit_index, 0, 0);
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

/* starts throwing a grenade (type 22) */
// score 19 (was 19): the three unit lookups are data-first (retail index-first) and the success path carries its own
// exit copy where retail's success falls into the shared exit and the notice block carries the copy.
// MiniMax H (11-07): no new form, no lower score (19); the three unit lookups still read the
// table with the other base and index order (`[ecx + edi + 8]` against retail's
// `[edi + ecx + 8]`), the success block still stores the flag byte and the result in the other
// order (ours pops edi and esi, then ORs and stores the result, then sets the flag byte; retail
// sets the flag byte, stores the result and pops last), and the failure branches still reach the
// exit copy at a different distance (the permuter managed one try in 308.8 s, best 19, baseline 19,
// no confirming build).
// MiniMax D (11-06, run 49): no new form, no lower score (19); the three unit lookups still read the table with the other base and index order (`[ecx + edi + 8]` against retail's `[edi + ecx + 8]`) and the tail still stores the flag byte after the OR.
// MiniMax H (11-04): no new form, no lower score (19); the earlier rounds' forms still leave the
// three unit lookups data-first and the exit block with its own copy, so the body stands.
// MiniMax E (11-01): 16 with the unit's ordinal masked once into a local and the table entry read
// through it as a raw address cast (`*(s_unit_action_unit **)(g_4e0300->data + ordinal * 12 + 8)`) in all
// three lookups, so they use retail's address operand order, and the result stored before the action's
// flag store; not kept (an address-cast read stays only with a byte match). The exit block still follows
// the notice block; retail has it before the notice block, which ends with its own copy of it.
// Earlier (19, was 272): Frame, setup and width domains agree; success pop/store/result schedule and
// failure-branch spans remain. The slot index masked once into a local and the macro's lookup
// written out by hand at its three uses gave 16 (not kept: a hand copy of UNIT_ACTION_UNIT_GET).
// Hand getter/result-first/both/split/all-ordinal setup 266/272/266/272/266/272/266/265; common
// exit 185, plus word hand 174, Boolean active field 39, native player selector 22; ordinal 19
// (three-point form dropped), without final domains 171; one scope/late false 19/22; success before
// commit 19/22; paired NULL/false setup 16, result after selection 29. Keep real common result,
// existing word hand getter, Boolean active field and long player value; only constant setup
// reordered. Every field read, call tuple and success commit is
// unchanged.
// Round 21 (lane 13): the first lookup through UNIT_ACTION_HEADER_GET 19; the grenade test as
// if/else with the player notice in the else 19; the player notice ending in `return result` 45
// (958 bytes); immediate and animated throws as one if/else chain 19; animate and player-index
// tests nested 19. A compile-only model keeps the exit block last for every structured, goto and
// inline-helper shape; retail's failure jumps reach the success path's exit copy instead.
// Round 50: `animate` declared with the entry locals 50; the channel declared with them 266
// (958 bytes); the grenade pointer from one ternary 269 (974 bytes); the immediate throw ending in
// `return result` 43; `result = true` before function_a8c10 21. The three `[edi + ecx + 8]` unit
// lookups stay data-first in a compile-only screen for every order of the entry declarations, an
// unused header pointer and a header pointer for the first lookup.
// Round 144: all three self lookups through the existing header accessor 19 (same code); the shared bit setter 19;
// an explicit success goto 19; a computed active value with the result before its store 19 (NOT kept: one-use
// local, no match); direct success return true/result 264/264 (948 B); an addressed result read 19 (same code);
// a one-iteration do/break scope 646 (1266 B, frame 0x84). Original body restored.
// Round 359: byte result 152; signed-byte result 152; short result 285; long result 285; read-
// only request 19; marker field at each use 19; separate object and weapon rejections 19; seat
// eligibility assigned directly 203. NOT kept: no qualifying byte match or gain; original body
// restored.
// Round 78 (m3 lane 11, fast): the entry lookup through a pool copy `objects` declared first 16 (all three self lookups become
// retail's index-first `[edi + ecx + 8]`; not kept: a 3-point gain with a one-use copy); all three through that copy 19; one
// copy per lookup 16; the later two through the copy 17; on the 16 form `result = true` before the active-bit store 16, before
// function_a8c10 18, the notice block ending in `return result` 42 (958 bytes). Left: retail's success path falls into the
// shared exit and the notice block carries its own copy; ours pops edi/esi inside the success path, which then carries it.
// Round 75 (m3 lane 14, fast): re-measured: every goto as `return result` 264 (948 bytes, round 144's code: constants folded);
// the notice block ending in `return false` 45 (958 bytes, round 21's code; the exit then follows the immediate path); round
// 21's structured forms combined with `immediate || !active` 19. New: the notice in the then-arm with the rest in its else 19;
// the animated throw before the immediate 115 (968 bytes); the two early rejections as one condition 19; the marker test as two
// gotos 19; the active test nested under `!immediate` 19. Without the notice block (diagnostic) the exit follows the success
// path as in retail (851 bytes); a compile-only model keeps it last for every other shape tried.
// Round 156 (m3 lane 8, fast): the notice block after `return result`, reached and left by gotos, 19 (same code); the
// notice as a file-local inline helper under `if (animate)` 96 (909 bytes, kept a call), with `animate` as its parameter
// 101 (905), with a player-index local 93 (911); the same helpers forced inline 74/74/74 (989 bytes, the player-index
// getter then inlines too); the existing player value getter for the notice 19 (same code); the pool local for the entry
// lookup 16 (a repeat). Compile-only: retail's layout comes back only when the success path ends with the a8c10 call
// (the bit set and `result = true` before it) or without the notice block; every other order or shape keeps ours.
// Round 153 (m3 lane 7, fast): both builds lay the blocks out alike (success path, an exit, the notice block, an exit);
// retail's ten failure jumps and the notice block's three go to the first exit and the notice block ends in the copy,
// ours send them to the last one and merge the first into the success path (hence its early pops). `result = true`
// ahead of the active-bit store 19 (same code); the notice block ending in `return result` 45 (958 bytes, its result
// folded to `xor al, al`), with the result set first 45; the immediate throw ending in `return result` 43; both ending
// in `return result` 74 (956 bytes); the exit label in the function's own scope (no inner braces) 19 (same code), with
// the three later declarations hoisted without initialisers 19 (same code); the active bit and the result set ahead
// of the 0xa8c10 call 26 (965 bytes: the copy after the call is then left unmerged, but the jumps still go to the
// last exit); the notice block after `return result`, entered and left by gotos, 19 (same code; re-measured).
// The grenade condition as `!(grenade && count > 0)` 19 (same code), as `grenade == NULL || count < 1` 48 (965 bytes,
// `cmp ax, 1`), as two nested tests jumping over the notice block 19 (same code), held in a named bool 176, as an
// empty then-arm with the notice in the else 19 (same code).
// Round 118 (m3 lane 9, fast; same-code spellings, not counted as trials): the seat flag and the 0xee8a0 result as two bools 19; the hand
// weapon read through a fresh UNIT_ACTION_UNIT_GET (0xe8460's spelling) 19; actions through
// UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index)) (0xe8980's spelling) 19; the busy test with `if (!immediate) goto done;`
// nested inside it 19; the success code inside a positive `if ((animate && ...) || ...)` 19. Diagnostics: the notice block
// cut to one unconditional call (295, 885 B), the active and animation tests dropped (245, 892 B), the grenade test as an
// integer compare (19, same code) and round 156's forced-inline notice helper (74) all keep our block order (the success
// path carries an exit copy and the notice falls into the last exit), so neither the notice's own tests, the success
// path's tests, the pointer type nor an inlined helper body sets it. Matched 0xe8460 and 0xe8720 use the `objects` pool
// local (round 78's 16 form) once beside the macro, so that form is the file's idiom once the exit order is found. No
// untried code-changing form is left for the exit order (rounds 21, 75, 153 and 156 above). New (counted): the notice as a
// post-check after `done:` gated by a `no_grenades` flag set in the grenade test 112 (989 B: VC threads the flag, so the
// grenade test still jumps straight to the notice, the block order stays ours, and 0x14de90 then inlines twice).
// @retail 0xe7fb0
bool __stdcall function_e7fb0(long unit_index, s_unit_request *request)
{
	bool result;
	{
		s_unit_request_throw_grenade *throw_request = (s_unit_request_throw_grenade *)request;
		s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
		s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
		s_unit_action_grenade *grenade = NULL;
		long weapon_index = unit_action_hand_weapon_get(unit, 0);
		result = false;

		if (unit->current_grenade_index != NONE)
			grenade = unit_action_grenade_get(unit);

		if (unit->object_type != 0 || function_101440(weapon_index))
			goto done;
		if ((function_110ab0(unit_index) || (TEST_FIELD_BIT((UNIT_ACTION_UNIT_GET(unit_index)->flags_134 >> 25) & 1))) &&
			!throw_request->immediate)
		{
			goto done;
		}

		bool animate = true;

		if (unit->parent_object_index != NONE && unit->seat_index != NONE)
		{
			byte *seat = unit_action_seat_get(unit);
			dword seat_flags = *(dword *)seat;

			if (!(TEST_FIELD_BIT((seat_flags >> 5) & 1)))
				animate = false;
			if (TEST_FIELD_BIT((seat_flags >> 11) & 1))
			{
				long marker_name = *(long *)(seat + 0x10);
				s_object_marker marker;

				if (!marker_name || function_b8d30(unit->parent_object_index, marker_name, &marker, 1, false) != 1)
					goto done;
			}
		}

		if (!grenade || function_cdff0(unit_index, UNIT_ACTION_UNIT_GET(unit_index)->current_grenade_index) <= 0)
		{
			if (animate && unit_get_player_index(unit_index) != NONE)
			{
				long player_value = *(short *)(g_4e8c24->data + (unit_get_player_index(unit_index) & 0xffff) * 0x21c + 0x28);

				if (player_value != NONE)
					function_2007b3(player_value, 0, 1, NONE);
			}
			goto done;
		}

		if (!throw_request->immediate && TEST_FIELD_BIT(UNIT_ACTION_ACTIVE(actions, 0x16)))
			goto done;

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
			goto done;
		}

		if (!(animate && function_113e90(unit_index, 0x1000006c, 0.1335f, &channel, 0)) &&
			!function_113e90(unit_index, 0xd000021, 0.1335f, &channel, 0))
		{
			goto done;
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
	}
done:
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
	bool result = true;
	char count = ++actions->grenade_throw_count;

	switch ((char)actions->grenade_state)
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
			result = false;
		}
		break;
	}
	return result;
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
	s_record_pool *objects = g_4e0300;
	long hand = !primary;
	long weapon_index = unit_action_hand_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), hand);

	if (weapon_index != NONE)
	{
		s_unit_action_unit *current = ((s_unit_action_header *)objects->data)[unit_index & 0xffff].unit;

		function_1015a0(weapon_index);
		function_100130(weapon_index, true);
		*(short *)((byte *)UNIT_ACTION_UNIT_GET(weapon_index) + 0x16e) = 0;
		function_104080(weapon_index);
		function_10cd50(weapon_index);
		(&current->current_weapon_index)[hand] = NONE;
		function_ee7f0(unit_index, primary ? 0x18 : 0x19);
	}
}

/* starts putting away the weapon in a unit's hand: its put away animation,
   or at once */
// @retail 0xe8510
bool function_e8510(long unit_index, bool immediate, bool silent, bool primary)
{
	long weapon_index = unit_action_hand_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), !primary);
	bool result = true;

	if (weapon_index != NONE)
	{
		long name = primary ? 0x8000025 : 0xd000539;

		if (function_114040(unit_index, name) && !immediate)
		{
			result = false;
		}
		else if (silent && immediate)
		{
			result = true;
		}
		else if (function_100130(weapon_index, immediate))
		{
			function_c86e0(unit_index, 0);
			if (!silent)
				function_ee7f0(unit_index, primary ? 0x12 : 0x13);
			result = function_113e90(unit_index, name, 0.267f, NULL, primary ? 0 : 3) ? immediate : true;
		}
		if (result)
			function_e8460(unit_index, primary);
	}
	return result;
}

bool function_10fd40(long unit_index, long action_name, long state_name, bool flag);
byte function_10fcd0(long unit_index, long unknown, long state_name, long action_name);
void __stdcall function_d0870(long weapon_index, long unit_index, long secondary);
void __stdcall function_fff40(long weapon_index, bool silent, bool immediate);

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
		s_unit_seat_definition *seat = (s_unit_seat_definition *)unit_action_seat_get(unit);
		long weapon_index = unit_action_hand_weapon_get(UNIT_ACTION_UNIT_GET(unit->parent_object_index), 0);

		if (TEST_FIELD_BIT(seat->flags.bit3) && !TEST_FIELD_BIT(seat->flags.bit2) && weapon_index != NONE)
		{
			byte *s_type_67e06b = g_4e3b44[UNIT_ACTION_UNIT_GET(weapon_index)->definition_index & 0xffff].bytes;

			state_name = *(long *)(s_type_67e06b + 0x288);
			action_name = *(long *)(s_type_67e06b + 0x28c);
		}
	}
	function_10fd40(unit_index, action_name, state_name, flag);
}

/* takes out the weapon in a unit's slot for a hand: its animations and
   ready animation */
// MiniMax A (11-06, run 52): no new form, no lower score (95) - retail loads the pool after the
// prologue pushes, keeps the hand in cl and reloads the unit through a copy of the data register.
// MiniMax D (11-05): no new form, no lower score; retail loads the pool global after the prologue
// pushes and keeps the hand in cl, ours loads the pool first and keeps the hand in al.
// MiniMax G (11-04): no new form, no lower score; retail loads the pool data and the unit twice with
// a stack spill between the two loads, ours keeps one load, so the index form, the reload of the pool
// data and the register allocation differ.
// MiniMax D (11-09): the animate test written in both arms of the primary test, on the pool-copy form, matches retail byte for byte.
// score 0 (was 95): matches retail: the companion weapon read through the pool copy, the other hand's slot as its own local, and the animate test in both arms of the primary test. Round 220 lead (m3 lane 3):
// the other hand's slot and weapon read through `s_record_pool *objects = g_4e0300;` declared first (0xd1540's per-hand
// pool copy, which matched 0xe8de0/0xe9190/0xfff40 this round) with a long hand gives retail's second unit load through a
// copy of the data register and retail's length: 8 at 604 bytes (fast and plain), not kept (0xcbd50's body written out,
// a pool copy beside the global: kept only on a match). Left on it: the copy goes to EDI where retail spills the hand
// first and copies into ECX, the other slot's address is base-first (retail index-first), and retail hoists the
// animate reload above the 0x14/0x15 branch. See the round 220 forms at the end of this note.
// Earlier (148): frame 0x18 agrees; entry lookup/hand homes and ready-call scheduling remain.
// Numeric primary/shared unit/both 195; direct hand and its combinations 200; conditional state
// with direct hand 148, alone 192; scan fallback exit 148 (kept), with direct hand 203, numeric
// primary 148, both 203; hand before unit lookup 203, with scan 198, with state and scan 201.
// Round 116: retail loads the unit twice at entry (the second load, for the other hand's slot and
// weapon, from a copy of the same data register with the index times 12 shared), spills the hand
// at once and keeps the other weapon in EDI; ours uses one lookup (592 bytes). Retail also picks
// 0x14/0x15 with a branch (`mov eax, 0x14; jne; mov eax, 0x15`). The other weapon through
// function_cbd50 148 (kept: the existing function in place of its body written out); the other
// hand as `!hand` 166; the type as an if/else 148 (same code), `type = 0x15; if (primary)` 189,
// `type = 0x14; if (!primary)` 190 (retail's branch, the tail one instruction later). Locate-only:
// an unused header element after or before the first lookup 170/161.
// Round 187: the hand straight to 0xd0870 (its third parameter is a long; retail pushes the slot
// value) 203; a pool local for the unit/slot lookup/both 148/148/198; a header pointer for both reads
// 157, with the direct hand 145, and with `type = 0x14; if (!primary) type = 0x15;` too 96 (606 bytes;
// reads header->unit at one use while `unit` copies it: not kept); direct hand + that type 198; the slot
// lookup through UNIT_ACTION_HEADER_GET 148; orders other-first/hand-first/other-mid 194/196/149; inline
// getter functions 198; header->unit at every use 218; a data-pointer local for the slot lookup 198/188.
// Retail's `mov ecx, edx` copy of the data register before the second unit load is in no other retail
// function (byte search).
// Round 46 (lane 13): the 0xcbd50 argument through a header-array local 148, through the `unit` local 148,
// `long has_other = function_cbd50(...) != NONE` 148, the slot read through a fresh lookup 148 (all the same
// code: locals are copy-propagated and every lookup shares one load); is_vehicle before has_other 189 (608 B,
// loop padding). The hand straight to 0xd0870 (203) is retail's code at that call, but the body is then 585 B
// and the positional score rises; the entry's second unit load is the structural cause (a compile-only
// harness also merges it for inline getters taking the pool, the data pointer or the index);
// `function_ee8a0(unit_index, primary)` 197 (591 B); the state name as a ternary 188 (604 B, but a branch
// and jump where retail stores the 0x288 field and then overwrites it).
// Round 324: lexical receiver/hand/paired-weapon home subsets 1-7, then all with
// the companion-weapon test named by its role, 148/148/148/148/148/148/148/148. Original body
// restored; early pool load, second unit lookup and hand lifetime remain.
// M291: native bool/long companion predicates with short/char hands and long/short slots
// 190/200/197/152/136/203/189/171; keep only short hand, plain 136 at 606/604 bytes.
// M306: Joined terminal animation fallback on the short-hand graph, with original/direct long hand, event branches and companion-predicate initialization: 136/136/136/136/144/144/144/144. Direct hand recovers frame 0x18 and 604 bytes; all restored.
// M311: Unsigned bounded hand bool/byte/word/unsigned-int, each Boolean/direct long argument:
// 189/184/189/184/136/144/148/203; restored.
// Round 470: Native short/word/byte/char two-slot scan ordinals, then each with
// direct bounded hand at the long argument: 191/191/192/192/197/197/197/197. All restored;
// extra frame word, second entry lookup and hand transport remain.
// Round 364: Actual current-weapon array view, long/short slot, Boolean/direct hand
// and conditional/branch event: 136/203/144/142/139/205/95/188. Keep the real array,
// direct bounded hand and branch event with the original long slot: plain 95, 606/604 bytes.
// All other full-image measurements stay unchanged; entry table and second unit load remain.
// F332: Actual two-element array cursor, endpoint/long/short/word termination
// 103/186/187/187; with consistent direct receiver observations 264/266/267/267.
// Original indexed scan and receiver ownership restored; entry pool/hand and second load remain.
// Round 368: Reused normalized primary-hand long/short/dword/word, before companion then unit lookup:
// 95/96/95/96/94/95/94/95; original complete body restored.
// DH302: Slot/weapon/both early failure guards and one complete exit 95/95/106/95;
// each retaining the companion datum for both predicates 200/200/212/200. Original success
// scopes and predicate lifetime restored; early table and second unit load remain.
// AC483: Actual-array/direct-hand/branch-event graph with long/dword/word/byte hand,
// each long/bool companion result: 198/200/198/200/95/200/184/197. Original short hand,
// long result and complete body restored; second entry lookup and hand spill remain.
// Round 375: Test-produced companion truth, long/Boolean, positive/negative default after/before getter:
// 185/185/182/182/185/185/200/200. Original comparison result and complete body restored;
// second entry unit load, table/hand homes and downstream scheduling remain.
// Round 379: Twice-used animation result byte/char/short/long, at its producer then entry storage:
// 95/95/142/146/95/95/142/146. Original Boolean result and complete body restored; entry pool/hand
// and second unit load remain.
// P280: Native object-type predicate byte/char/short/word/long/dword storage
// 95/95/109/109/108/108; Boolean declarations at observation/before companion test
// 95/185. Original complete body restored; entry lookup and hand homes remain.
// P284: Shared low-word entry ordinal long/dword/word/short 196/196/194/194;
// with the existing later slot getter 234/234/230/230. Complete original body
// restored; early pool acquisition, second unit load and full hand home remain.
// Round 543: Actual first-found equipped-slot result, bool/byte/short/long,
// positive/complementary polarity: 95 each. NOT kept: staged search flags require
// exact equality. Original indexed scan, direct exit and complete body restored.
// Round 549: Conditional/zero-default/one-default/complete-arms hand results at the
// original boundary 95/185/184/95; before unit acquisition 189/188/188/189.
// NOT kept: no match or gain; original hand, lookups and complete body restored.
// F346: Completed companion-index, metadata and scan-ordinal lifetimes in seven subsets,
// then all with completed state selection:
// 95/95/95/95/95/95/95/95. Original complete executable body restored.
// P289: Twice-used animation channel long/dword/short/byte at its producer, then
// entry-owned declarations: 130/130/128/133/130/130/128/133. NOT kept: native recomputes the channel and
// no exact match. Original body restored; early pool/hand and second unit load remain.
// Round 83 (m3 lane 12, fast): diagnostic only (reverted at once): a long hand read through a
// volatile pointer to its home 198 (590 bytes): retail's hand store does not explain the second entry
// load, VC still shares one unit load. `long has_other = function_cbd50(...) != NONE` 95 (same code);
// the slot index as `primary` 193; `long i` declared at the slot 95; 0xee8a0's argument as `primary`
// 201 (589 bytes); a short loop index 188 (604 bytes). Retail's ret 0x10 with primary in EAX already agrees (the ret 0x14 premise is stale).
// B393: Actual input-header/unit/tag identity references, subsets 1-7,
// then all with the twice-read action field bound by reference: 181/95/181/95/181/95/181/181. NOT kept: no exact match;
// original complete body restored; entry table/hand homes and second unit load remain.
// P289: Twice-used animation channel long/dword/short/byte at its producer, then
// entry-owned declarations: 130/130/128/133/130/130/128/133. NOT kept: native recomputes the channel and
// no exact match. Original body restored; early pool/hand and second unit load remain.
// B414: Actual metadata animation-admission outcome, positive/negative bool/byte/short/long:
// 177/170/178/176/179/179/179/178. NOT kept: all regress, including 170 at 604 bytes.
// Original complete body restored; early pool, second unit load and hand storage remain.
// Round 208 (m3 lane 3, fast): a compile-only LTCG probe gives retail's entry (unit load, hand store, unit reload
// through a copy of the data register) only when `hand` is a memory local whose store sits between the two lookups
// (its address passed to a call); every lookup spelling without that shares one load. Diagnostics, reverted at once:
// `(void)&hand` 95, with a long hand 198; a local pointer to hand read at the 0xd0870 argument 95 (short) / 198 (long),
// at the slot store 95/198 (LTCG resolves the pointer away). Retail passes no local's address anywhere here.
// P317: Final readiness gate bool/byte/short/long, play/skip polarity
// 95/95/95/95/95/95/95/95. NOT kept: all gates canonicalize; original entry pool/hand homes and second-load gap remain.
// Original complete body and every existing helper restored.
// P322: Complete three-event selection, positive/negative/conditional/split
// producers, long then short: 144/144/144/144/146/146/146/146. Long reaches 604 bytes;
// all regress. Original event graph and whole body restored; early pool/hand homes
// and the second unit load remain.
// M365: Selected notification type signed-char/short/dword/int64, at its producer then
// declared uninitialized before the animation query: 144/143/95/159/144/143/95/159. NOT kept:
// ties or regressions; original event result, observations and complete body restored.
// Frame 0x18, early table/hand homes and second entry unit load remain.
// M367: Optional ready-query completion/fallback result bool/byte/short/long:
// 95/95/95/95/95/95/95/95. NOT kept: one-use result producers do not match.
// Original terminal returns and complete body restored; early hand spill and second unit load remain.
// M395: Actual two-slot pointer scan, equality/ordered ends with long/short/char/int selected slots:
// 103/103/204/204/187/187/103/103. NOT kept: no exact native-shape candidate; native early hand/table setup and second unit load remain.
// Original complete body restored.
// Y598: Actual late state/tag/selected-weapon lexical identities, seven subsets
// and complete entry-uninitialized storage: 95/95/95/95/95/95/95/95. NOT kept:
// no exact home repair; original body restored; early hand spill and second unit load remain.
// Round 220 (fast; on the pool-copy form above): short hand 157 (606 B), long hand 8; hand declared first 27, between
// the pool copy and the unit 27; the pool copy after the unit 8; a named pointer for the other unit 198 (590 B); the
// type as `primary ? 0x14 : 0x15` 27, as an if/else 27 (neg/sbb); the other slot through `(&...->weapon_slots[0])[...]`
// 8; the NONE default declared before the other slot read 182 (620 B); the unit through the pool copy and the other
// hand through the global 198; 0xcbd50 with the slot argument through the pool copy 198; the unit through
// UNIT_ACTION_HEADER_GET 8; permuter (13 tries in 631 s) no gain.
// P389: Real equipped-array first-match search through standard find/search_n,
// direct range/endpoints/pointer-result/ordinal-result consumption:
// 190/187/206/194/224/228/224/242. NOT kept: no exact native frame/home recovery;
// original complete body, observations, helper and call graph restored.
// P399: Actual equipped/companion/object member-array references, subsets 1-7, then complete array-base bindings:
// 96/95/96/95/96/95/96/96. NOT kept: no byte equality or native home repair; original complete body restored.
// @retail 0xe8720
void __stdcall function_e8720(long unit_index, long unknown, bool immediate, bool silent, bool primary)
{
	s_record_pool *objects = g_4e0300;
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	long hand = !primary;
	long primary_index = primary != false;
	short other_slot = ((s_unit_action_header *)objects->data)[unit_index & 0xffff].unit->weapon_slots[primary_index];
	long other_weapon_index = NONE;
	if (other_slot != NONE)
		other_weapon_index = ((s_unit_action_header *)objects->data)[unit_index & 0xffff].unit->weapon_object_indices[other_slot];
	long has_other;
	bool is_vehicle;

	has_other = other_weapon_index != NONE;
	is_vehicle = ((dword)(1 << unit->object_type) >> 1) & 1;

	long slot = unit->weapon_slots[hand];

	if (slot != NONE)
	{
		long i;

		for (i = 0; i < 2; i++)
		{
			if (unit->current_weapon_indices[i] == slot)
			{
				function_e8620(unit_index, primary);
				return;
			}
		}
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

			function_d0870(weapon_index, unit_index, hand);
			unit->current_weapon_indices[hand] = (char)slot;
			unit->weapon_ready_times[slot] = g_510c54->game_time;

			bool animate = function_ee8a0(unit_index, primary != false);

			function_fff40(weapon_index, silent, immediate);
			if (!silent)
			{
				long type = 0x14;
				if (primary)
				{
					if (animate)
						type = 0x16;
				}
				else
				{
					type = 0x15;
					if (animate)
						type = 0x16;
				}
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
	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));
	bool primary = request->type == 8;
	long hand = !primary;
	bool result = false;

	if (switch_request->immediate ||
		unit->weapon_slots[hand] != (&unit->current_weapon_index)[hand] && !function_110ab0(unit_index) &&
		!TEST_FIELD_BIT((UNIT_ACTION_UNIT_GET(unit_index)->flags_134 >> 25) & 1) && !unit_action_active(unit_index, 0x16))
	{
		long weapon_index = function_cbd50(unit_index, UNIT_ACTION_UNIT_GET(unit_index)->weapon_slots[hand]);

		if (unit->weapon_slots[hand] == NONE || weapon_index != NONE && function_cd6a0(unit_index, NONE, weapon_index))
		{
			function_e69c0(unit_index, 0);
			function_e69c0(unit_index, 0xa);
			function_e69c0(unit_index, 0x1b);
			function_e69c0(unit_index, 0x16);
			if (function_e8510(unit_index, switch_request->immediate, switch_request->silent, primary))
			{
				function_e8720(unit_index, 0x7000101, switch_request->silent, switch_request->silent, primary);
			}
			else
			{
				long type = primary ? 8 : 0x12;

				actions->active[type >> 5] |= 1 << (type & 0x1f);
			}
			result = true;
		}
	}
	return result;
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
			long weapon_index = unit_action_hand_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), hand);

			if (weapon_index != NONE)
			{
				function_105fa0(weapon_index, 0);
				function_100350(weapon_index);
			}
		}
		result = false;
	}
	else if (!function_110ab0(unit_index) && function_e8510(unit_index, false, false, primary))
	{
		function_e8720(unit_index, 0x7000101, false, false, primary);
		result = false;
	}
	return result;
}

/* a weapon switch interrupted: the old weapon is put away at once and the
   new one taken out */
// score 11 (was 11; 29 before): only the entry block's order differs (retail loads the global and the
// actions pointer before `cmp ecx, 8; sete bl`). Tried: unit_action_active() 95; primary
// declared first 11; primary inside the block 82; a unit local 90; an early-return guard 11;
// actions read inside the test with primary first 11; the bit test as its expression 11; the test
// result named first 11; `type == 8` in each call argument 82; the then-block as a call of the
// switch-finished action 252, with that action inline 252 (not inlined).
// Round 31: direct header lookup 11; named header 77; separate actions/primary assignments 11 each;
// primary branch 87; zero/one ternary 11; long primary 96; active pointer 11;
// force-inlined switch-finished action 82.
// Round 187: a function-top pool local (`s_record_pool *objects = g_4e0300;`) for the lookup 11 (same
// entry order); with a unit pointer from it 90.
// Round 359: read-only actions 11; byte selector 76; signed-byte selector 76; short selector 97;
// opposite hand decision 76; primary set from equality 87; signed action displacement 90; full-
// width active result after primary 11. NOT kept: no qualifying byte match or gain; original
// body restored.
// MiniMax H (11-12): no new form, no lower score (11); the entry block keeps the same eleven instructions in the same order, with the type compare still ahead of the index arithmetic.
// MiniMax H (11-11): no new form, no lower score (11); retail still loads the object-pool global into ECX and the unit header into EDX before it loads the type argument, while ours loads the type into ECX first and puts the global in EDX, so the type compare lands before the index arithmetic instead of after it; the rest of the function matches instruction for instruction.
// MiniMax C (11-11): no new form, no lower score (11); the entry block still compares the type with 8 before the index arithmetic where retail loads the pool global and the unit header first and compares afterwards.
// MiniMax C (11-10): score 11 (was 11): no lower score this round. Two new spellings of the lookup were
// measured and dropped: a named unit header pointer 65, and a named pointer to the active bit vector 11 with
// the entry order unchanged. A permuter run (300 s) was stopped at its time limit without a result.
// MiniMax A (11-11): no new form, no lower score (11); the entry block still holds the same eleven instructions in
// another order: retail reads the object pool global, the pool data and the type argument, computes the unit and the
// actions pointer and only then does `cmp ecx, 8; sete bl`, ours loads the type first and compares it before the pool
// data is read. The primary computed inside the test's block (82), the primary declared before the lookup (11) and a
// copy of the type parameter in a local (11) all keep the same order, so the body stands.
// MiniMax I (11-09): no new form, no lower score (11); the entry block keeps the same eleven instructions in
// another order (retail reads the object pool, the pool data and the type, and only then compares the type with 8),
// and a copy of the type parameter in a local changes nothing.
// MiniMax H (11-07): no new form, no lower score (11); the entry block still holds the same
// instructions in another order: retail reads the object pool global, the pool data and the type,
// computes the unit and the actions pointer and only then does `cmp ecx, 8; sete bl`, ours reads
// the type first and compares it before the pool data is read (the permuter managed two tries in
// 306.5 s, best 11, baseline 11, no confirming build).
// MiniMax D (11-06, run 49): no new form, no lower score (11); the entry block still compares the type with 8 first, retail loads the object pool, the unit header and the type, computes the unit and the actions pointer, and only then makes that comparison.
// MiniMax H (11-04, run 33): no new form, no lower score (11); the permuter managed one try in
// 326.9 s (best 11, baseline 11, no confirming build), and the entry block keeps the same eleven
// instructions in another order.
// MiniMax A (11-04): no new form, no lower score (11). What still differs: the entry block's
// order. Retail loads the object-pool global, then the unit header, then the type argument,
// computes the unit and the actions pointer, and only then does `cmp ecx, 8; sete bl`; ours
// compares the type first and reads the global afterwards. Same instructions, different schedule.
// The permuter (300 s) managed one try and found nothing.
// MiniMax E (11-01): no new form, no lower score; the entry block keeps the same eleven
// instructions in a different order in every form tried.
// MiniMax E (11-09): no new form, no lower score (11); the type's flag is read much later (test bl,bl and
// sete cl after the tag lookup), so retail can leave the compare at the end of the entry block, while ours
// hoists it (cmp ecx,8 and sete bl) in front of the index arithmetic and reads the item table after the
// object index; the rest of the function matches instruction for instruction.
// MiniMax C (11-10): no new form, no lower score (11); a copy of the type argument (a named local used
// by both the compare and the bit test) folds away and gives the same listing, and the entry order is
// unchanged: retail loads the pool global into ECX and the type after it, ours loads the type into ECX
// first and puts the global in EDX, so the compare lands before the index arithmetic instead of after
// it.
// Round 160 (m3 lane 8, fast): a statement that loads the unit ahead of the primary flag gives retail's first three
// instructions (the pool global in ECX, the data in EDX, then the type), but the unit pointer is then kept in EAX and
// reused by the put-away code after the branch (233 bytes): a named unit pointer 90, the unit read through a local
// copy of the pool 90, the unit field read as a long 90, an inline accessor with its own unit local 90, an inline
// accessor taking the unit pointer 90. The pool's header array named before the flag 65 (249 bytes; the array is
// spilled and reused after the branch, the type loads first). The actions offset read into a long before the flag
// 95 (233 bytes; it folds into the test, the unit is still reused). Diagnostic: with the actions pointer used twice
// (the test repeated, folded to one) the lookup stays ahead of the flag as a whole statement and the unit is looked
// up again after the branch, 74 (249 bytes), but the offset's add then comes before the compare and the unit index
// takes EBP. Retail has the offset's load before the compare and its add after it, with nothing kept for the
// second lookup but the scaled index.
// P422: Complete mutable/const header tables with primary after/before acquisition
// 65/67/65/67; const pool root 11/11; const pool/table 65/67. Existing object getter
// boundaries, expression/unit receiver: 11/90 twice. All original calls restored.
// NOT kept: table sharing adds a spill; input scheduling remains at 11.
// Round 240 (m3 lane 3, fast): the active test through the file's UNIT_ACTION_ACTIVE macro 29; `bool const primary`
// 11; a file-local inline helper taking (unit_index, type) with unit and actions locals (0xe6fe0's shape), inline or
// forceinline, 95/95, the same helper without the unit local 11; primary started false and set by `if (type == 8)` 87.
// Diagnostics (reverted): with a constant primary the type loads at the bit test, after the movsx (133); with a
// constant bit the compare reads the argument from memory ahead of the lookup (100). Permuter (fast, anneal, 600 s):
// 27 tries, best 11.
// @retail 0xe8c10
void __stdcall unit_action_weapon_switch_interrupted(long unit_index, long type)
{
	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));
	bool primary = type == 8;

	if (bit_vector_test(actions->active, type))
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
void __stdcall function_191f3a(long value, long definition_index);

/* a player's short at +0x28 (the players data, 0x21c bytes each) */
PRIVATE inline short unit_action_player_value28_get(long player_index)
{
	return *(short *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x28);
}

/* drops the weapon in a hand (types 9 and 19); the slot and each hand are
   read through their own copy of the object pool, so each looks the unit up
   again, as retail does */
// @retail 0xe8de0
bool __stdcall unit_action_drop_weapon(long unit_index, s_unit_request *request)
{
	s_record_pool *objects = g_4e0300;
	s_record_pool *weapon_objects = g_4e0300;
	s_record_pool *other_objects = g_4e0300;
	bool primary = request->type == 9;
	long hand = !primary;
	long slot = (&((s_unit_action_header *)objects->data)[unit_index & 0xffff].unit->current_weapon_index)[hand];
	long weapon_index = unit_action_pool_hand_weapon_get(weapon_objects, unit_index, hand);
	long mode;
	bool flag;
	bool result = false;

	if (weapon_index != NONE)
	{
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

		long other_index = unit_action_pool_hand_weapon_get(other_objects, unit_index, primary != false);

		if (other_index != NONE &&
			UNIT_ACTION_UNIT_GET(weapon_index)->definition_index == UNIT_ACTION_UNIT_GET(other_index)->definition_index)
		{
			function_101740(other_index, weapon_index);
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

		if (unit_get_player_index(unit_index) != NONE)
		{
			long value = unit_action_player_value28_get(unit_get_player_index(unit_index));

			if (value != NONE)
				function_191f3a(value, UNIT_ACTION_UNIT_GET(weapon_index)->definition_index);
		}
		result = true;
	}
	return result;
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
void __stdcall function_1ca260(long actor_index, long weapon_index, long player_index, long other_weapon_index);

/* picks up a weapon (type 20), in the way the request's mode asks */
// @retail 0xe9000
bool __stdcall unit_action_pickup_weapon(long unit_index, s_unit_request *request)
{
	s_unit_request_pickup_weapon *pickup = (s_unit_request_pickup_weapon *)request;
	s_unit_action_unit *weapon = UNIT_ACTION_UNIT_GET(pickup->weapon_index);
	bool result = false;
	bool modes[4];

	if (*(long *)((byte *)weapon + 0x154) == NONE && function_cd7b0(unit_index, pickup->weapon_index, modes))
	{
		bool allowed;

		switch (pickup->mode)
		{
		case 0:
		case 1:
			allowed = modes[0] && function_cbd50(unit_index, UNIT_ACTION_UNIT_GET(unit_index)->current_weapon_index) == NONE;
			break;
		case 3:
			allowed = modes[0];
			break;
		case 4:
			allowed = modes[2];
			break;
		case 5:
			allowed = modes[1];
			break;
		case 6:
			allowed = modes[3];
			break;
		default:
			goto done;
		}
		if (allowed)
		{
			function_e69c0(unit_index, 8);
			function_e69c0(unit_index, 0x12);
			function_e69c0(unit_index, 0);
			function_e69c0(unit_index, 0xa);
			function_e69c0(unit_index, 0x1b);
			function_e69c0(unit_index, 0x16);
			if (function_cd0c0(unit_index, pickup->weapon_index, pickup->mode))
			{
				function_191fab(weapon->definition_index,
					unit_get_player_index(unit_index) == NONE ? NONE : unit_action_player_value28_get(unit_get_player_index(unit_index)));
				function_c86e0(unit_index, 0);
				result = true;
			}
		}
	}
done:
	return result;
}

/* gives a unit's weapon to a player's unit, taking the player's weapon
   in exchange (type 21); each weapon is read through its own copy of the
   object pool */
// @retail 0xe9190
bool __stdcall function_e9190(long unit_index, s_unit_request *request)
{
	s_record_pool *objects = g_4e0300;
	s_record_pool *other_objects = g_4e0300;
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	long player_index = *(long *)((byte *)request + 4);
	bool given = false;

	if (unit->unknown13c == NONE)
	{
		long player_unit_index = *(long *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x2c);
		long weapon_index = unit_action_pool_hand_weapon_get(objects, unit_index, 0);
		long other_weapon_index = unit_action_pool_hand_weapon_get(other_objects, player_unit_index, 0);

		if (weapon_index != NONE)
		{
			s_unit_request drop;

			drop.type = 9;
			*((bool *)&drop + 4) = true;
			unit_action_drop_weapon(unit_index, &drop);
			given = function_cd0c0(player_unit_index, weapon_index, 4);
			if (given)
			{
				bool taken = false;

				if (other_weapon_index != NONE)
					taken = function_cd0c0(unit_index, other_weapon_index, 3);
				if (unit->unknown12c != NONE)
				{
					if (taken)
						function_1ca260(unit->unknown12c, weapon_index, *(long *)((byte *)request + 4), other_weapon_index);
					else
						function_1ca260(unit->unknown12c, weapon_index, *(long *)((byte *)request + 4), NONE);
				}
			}
			else
			{
				function_cd0c0(unit_index, weapon_index, 3);
			}
		}
	}
	return given;
}

/* plays an animation on a unit and on its child units */
// @retail 0xe92e0
bool __stdcall function_e92e0(long object_index, long name)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(object_index);
	bool result = function_113e90(object_index, name, 0.0f, NULL, 0);

	for (long child_index = *(long *)((byte *)unit + 0x10); child_index != NONE; )
	{
		s_unit_action_header *header = UNIT_ACTION_HEADER_GET(child_index);
		s_unit_action_unit *child = header->unit;

		if ((TEST_FIELD_BIT((child->object_flags >> 26) & 1)) && ((1 << header->type) & 3) && function_e92e0(child_index, name))
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
PRIVATE __forceinline bool unit_action_pause(long unit_index, s_unit_request *request, long name)
{
	bool result = false;

	if (function_e92e0(unit_index, name))
	{
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
		}
		else if (*((byte *)request + 5) == 1)
		{
			s_animation_state *state = unit_action_state_get(unit_index);

			if (unit_action_channel_valid(&state->channels[2]) && TEST_FIELD_BIT(state->channels[2].flag0))
				state->channels[2].unknown11 |= 1;
			if (unit_action_channel_valid(&state->channels[0]) && TEST_FIELD_BIT(state->channels[0].flag0))
				state->channels[0].unknown11 |= 1;
			if (unit_action_channel_valid(&state->channels[1]) && TEST_FIELD_BIT(state->channels[1].flag0))
				state->channels[1].unknown11 |= 1;
		}
		else
		{
			s_animation_state *state = unit_action_state_get(unit_index);

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
__declspec(noinline) void function_edfa0(long unit_index, point2f const *facing);
__declspec(noinline) void function_ba3d0(long unit_index);

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

	if (function_e7020(unit_index, &alternate, impulse->name))
	{
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
		if (function_113e90(unit_index, impulse->name, blend, NULL, 0))
		{
			s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
			word *animation_flags = (word *)((byte *)unit + unit->unknown33e);

			*animation_flags |= 1;
			if (impulse->face && unit->parent_object_index == NONE)
				function_edfa0(unit_index, (point2f const *)&impulse->facing);
			result = true;
		}
	}
	return result;
}

/* plays an animation on a unit, facing it the way the animation asks */
__declspec(noinline) bool function_e9780(long unit_index, long name, bool keep, vector3f const *facing, c_animation_channel **channel_out);

// @retail 0xe9780
bool function_e9780(long unit_index, long name, bool keep, vector3f const *facing, c_animation_channel **channel_out)
{
	bool result = false;
	c_animation_channel *channel = NULL;

	if (function_113e90(unit_index, name, 0.0f, &channel, 0))
	{
		s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);

		if (!keep)
			function_ba3d0(unit_index);
		s_animation *animation = channel->function_1c6440();

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

	if (!function_110ab0(unit_index))
	{
		s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);

		if (!TEST_FIELD_BIT((unit->flags_134 >> 25) & 1))
		{
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

			if (function_e9780(unit_index, name, true, melee->face ? &melee->facing : NULL, NULL))
			{
				if (TEST_FIELD_BIT((*(dword *)(definition + 0xbc) >> 8) & 1))
					function_10f430(unit_index, 0x7000101, 0x7000101, 0x7000101, 0xc000043, 0.0f, 0, 2);
				actions->active[0] |= 0x4000000;
				result = true;
			}
		}
	}
	return result;
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
	long count = 0;
	long i;

	for (i = 0; i < NUMBEROF(g_4407f4); i++)
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
// score 234 (was 234): 235 before the b5Q walk. Frame 0x14 agrees; pool/index/type/name and completion-byte homes remain.
// MiniMax I (11-14): no new form, no lower score (234): the melee name's selection declared before the weapon getter
// 315 and a function-top pool local for both unit lookups 311, while the selection as an if/else instead of a ternary
// is the same code; the permuter over this function found nothing (2 tries, best 227 against a baseline of 227), so the
// pool and the index keep their register homes and the completion byte's home stays.
// MiniMax B (11-13): reading the weapon index as a 16-bit short, so that the compare is retail's 16-bit one (movsx
// ax, byte / cmp ax, bx against our 8-bit cmp dl, 0xff), scores 297 - the register allocation shifts - so the byte
// compare stays; the pool header is still in edi where retail keeps it in ebp and the index in eax where retail keeps
// it in edi.
// MiniMax H (11-12): no new form, no lower score (234); the tail's register homes and the 0x1b bit test are unchanged.
// MiniMax I (11-11): no new form, no lower score (234): the pool's and the offset's register homes are still ebp and edi where retail has edi and ebx, the NONE sentinel still rides in ebp where retail keeps it in ebx, and the lunge's divide still comes from memory.
// MiniMax B (11-11): no new form, no lower score (234); the frame is retail's 0x14 but the pool, index, type and name
// homes and the completion byte's home still differ, and the total-tick expression is still one product order away
// from retail's, so the body stays at retail's 1188 bytes.
// Round 187 reached 110 (frame 0x14 agrees; pool/type/name homes differ) only with a `goto done`
// single exit and the layout switch: not kept at review (match-only forms); without the gotos 305.
// Numeric hand 297; Boolean active bit 251; both 302; first result 235; all 302;
// shared actions receiver 328; single exit with hand/bit 315; per-use speed with hand/bit 302.
// Entry name default/or hand/or bit/or both 312/314/299/299; with single exit 362;
// original-graph single exit 241; per-use speed 235; entry name/single exit/per-use speed 362.
// Round 187: the fallback-name tests as a switch (retail's `sub eax, 0xc000073; je; sub eax, 2; je`
// and `cmp 0xe000607; je; jle; cmp 0xe00060a; jg` are switch code) 238; with the 0x1b test through
// unit_action_active 256, + single exit 244, + per-use speed 256/341 (0xea090 broke); a pool local for
// the 0x1b lookup 312. First-person default first + hand getter + Boolean 0x1b bit + switch 305, + single
// exit 110 (not kept, see above; the bit through a pool lookup 110, through unit_action_active the convention changes
// and 0xe9ed0/0xea090 break). Without the default/hand getter/Boolean bit/switch/single exit
// 317/116/361/252/305; fewer `goto done` (default+else, animation, default, else, default+animation,
// else+animation) 222/157/198/300/128/300; + per-use speed 357; declaration orders 110 (result before
// the unit 362); is_biped renamed 110; switch on the field 110. Left: retail rotates pool/index/
// first-person name through EBP/EDI/EBX (ours EDI/EBX/EBP, then type EBP vs EBX), the is_biped/result
// stack bytes swap (0x13/0x12), `current` in ESI vs EAX, the frame-count products' order and the
// lunge-speed division from memory.
// Round 46 (lane 13): `real seconds = frame_count * 0.033333335f;` then `seconds * (real)field_2_3` 234 (retail's
// product order; in one expression VC reassociates to field*frames*k); with the lunge speed read at each use too
// 234 (both blocks are then retail's code, one line off for retail's result byte store); with first_person_name
// = NONE declared first and the short-slot hand getter 314 (1195 B; the entry is retail's except pool/offset/NONE
// in EDI/EBX/EBP where retail has EBP/EDI/EBX); with a function-top pool local as well 314; is_biped renamed to
// sort after result 235 (same code: ours keeps result in a register, retail in the stack byte at 0x12).
// On the seconds and per-use speed form: the two animation exits merged into one `goto done` 277 (1213 B);
// TEST_FIELD_BIT on the 0x1b test 249 (1193 B).
// J224: Complete frame-to-seconds parts 234; native short frame domains 279; both 278.
// Complete animation/success local role names 235; with seconds/shorts/both 234/279/278;
// all with success at the final return 278. Sub-eight forms and all other trials restored.
// DH274: First-person default only in the two unsynchronized arms, separate/chained paired names, name/type first and original/common animation-failure exits: 305/306/305/306/305/306/305/306. All restored; frame 0x14 agrees, pool/index/type/name homes remain.
// AC468: Object-kind predicate byte/long/direct with Boolean result 235/325/331;
// with byte result 235/325/331; direct/byte predicate with long result 331/235.
// Original domains and observations restored; frame 0x14 agrees, pool/type/name and success-byte homes remain.
// M309: Reused self-object header/table 328/330; with numeric hand helper 325/327;
// with Boolean active test 297/299; both 295/297. All regress and are restored;
// pool/index/type/name register ownership and result-byte home remain.
// M311: Native bounded event type short/word/char/byte, original/numeric actual-case order:
// 243/243/243/243/270/270/270/270; restored.
// M313: Uniform per-use unit/actions, each with short-hand helper; both with Boolean active test; original body restored;
// fast 326/379/549/338/382/552/556/559.
// Round 537: failure-result lifetime at play/pre-call/post-call/fallback boundaries,
// bool 235/235/235/235; byte 235/235/235/235. Every original selection, call and
// complete body restored; pool/type/name ranks and completion-byte home remain.
// M317: Channel entry lifetime/function-owned animation/conditional animation acquisition subsets
// 1/2/4/3/5/6/7, all with common missing-animation return: 318/235/235/318/318/235/318/318.
// Pool/index/type/name and result-byte homes remain. Original executable body restored.
// N323: Dual-use mode-two selection in bool/byte/short/long, each with original/default-first combo results: 320/337/320/337/322/340/319/337. Original body restored; pool/type/name and completion-byte homes remain.
// F340: Complete original-width combo arms in orders CADB/DCAB/BDCA/CBDA/DABC/
// ADBC/BCAD/CDBA (A/B/C/D are the original four nontrivial arms): 235 each.
// Original complete body restored; pool/type/name and completion-byte homes remain.
// B386: Paired zero-byte/absent-tick chained stores and existing full-width rounding, subsets 1/2/4/3/5/6/7,
// then late constant success: 237/237/235/239/237/237/239/239. Original stores, conversion consumers and body
// restored; pool/type/name and completion-byte homes remain.
// P287: Animation admission owns return truth and gates common publication; four
// producers, special success before/after work: 290/307/303/305/290/306/257/239.
// All restored; pool/index/type/name and completion-byte homes remain; callers stay exact.
// Y558: Complementary final attack-failure bool/byte/short/long 235/235/235/235;
// each with the existing signed-short hand accessor 297/297/297/297. NOT kept:
// negative completion ties and the native-width lookup still regresses. Original
// getter, completion result, all calls and complete body restored.
// Lane 17 r39 (fast): the 0xe000607 fallback tested before the three-name/range arm 235 (same code); the range
// as `>= 0xe000608` 235; name/type/first-person declarations reversed 235, type first 235; is_biped computed
// before the weapon 307 (1191 bytes). Original body restored.
// J251: Weapon getter forceinline 235; byte-round inline 267; full-round inline 253;
// weapon/byte, weapon/full, byte/full and all expansion pairs 267/253/253/253;
// all with the short-hand consumer 277. NOT kept: rounding contracts regress and
// lose twelve or sixteen matched users. Original provider declarations and body restored;
// frame 0x14 and first +9 remain, with pool/type/name ranks and completion-byte home unresolved.
// AC514: Strength-owned target producers default/explicit arms/conditional/per-use 254/234/278/254;
// existing shared dot directions 235/235; complete longitudinal-speed/ratio parts 235/235.
// NOT kept: gain one below gate; all unmatched helper/grouping and receiver forms restored.
// Pool/index/type/name and completion-byte homes remain.
// M363: Native single-exit completion char/byte/short/long, then each with the existing
// signed-short hand accessor:
// 318/318/324/326/319/319/325/327. NOT kept: no eligible byte match or native-supported closer.
// Original complete body restored; pool/index/type/name and completion-byte homes remain.
// M371: Shared mode-one truth bool/byte/short/long at selection 273/273/231/214;
// at entry 350/350/348/358. NOT kept: four-point short lead below gate; long 214
// adds cached ECX/sete before the active test where retail compares mode in each
// selection path. Original full body restored; pool/name/result homes remain.
// M373: Unit/actions/weapon/selection role names eight 235; whole-function success default,
// bool/byte at original/entry declarations 286/338/287/336. NOT kept: ties or regressions;
// original names, false default, exits and full body restored; pool/name/result homes remain.
// Round 128 (m3 lane 7, fast; none kept, no match): round 187's form with the first lookup through a pool local,
// the 0x1b test by the field bit and four `goto done` 110 (1188 bytes); the 0x1b test's lookup by the macro 107
// (1188: the byte flag in dl and the pool callee-saved, but offset ebx and pool edi against retail's edi and ebp);
// both lookups by the macro 107 (1188); first_person_name after actions, with that test by macro, 107 (1188).
// first_person_name declared after the weapon (retail's `or reg, -1; cmp ax, reg; mov [esp + 0x18], reg`) 198 (1188),
// with that test by macro 320 (1192: the weapon takes ebx and first_person_name its own `or`), a local weapon copy
// in the mode arm 320 (1192) and 198 (1188). One-file LTCG harness: register probes of is_biped, first_person_name or
// the pool, is_biped through the field bit or written as a test, index references added through lookups for is_biped,
// the weapon or actions, weapon getter written out, first_person_name at four positions, crossed with pool and
// macro lookups: none gives the flag in dl with the pool in ebp and NONE in ebx. is_biped/result stay 0x12/0x13
// against retail's 0x13/0x12 with up to two more result reads (diagnostic).
// M3 lane 14 r68 (fast, on round 187's single-exit form, 110 at 1188 bytes, register-blind 14; match-only):
// lunge speed read from the field at both uses (retail divides from memory) 357 (1142: the allocation flips),
// through a melee-globals pointer read twice 357; first_person_name after actions 110; `real seconds =
// frame_count * 0.033333335f;` times the field 107 (retail's product order); on 107 the lunge target read at each
// use 161, read inside the `>= 1.0f` test with the NONE/0xbae20 test nested and target_index in both failure
// arms 105 (retail's jb before the field load; regblind 10). On the 235 body: seconds + nested lunge read 233.
// Lane 15 round 61 (fast; plain listing identical): first_person_name = NONE declared first, before the unit, 312
// (1191 bytes; aligned 142 -> 117, the NONE register and the weapon's NONE default become retail's); is_biped
// computed after the declarations 235, result declared before the weapon getter 235, the switch on
// actions->melee_name with no local 235 (same code each); the channel declared with the top locals 301.
// b5Q walk (11-06, fast): every block checked against retail 0xe9a20-0xe9ec1 (entry/hand getter, the 0x1b bit and the melee_name
// switch, mode 2 and mode 1 selection, the e9780 fallbacks, the damage/end/total ticks, the lunge arm, the 0x7f/0xff tail):
// fields, offsets, constants (1.0, 0.0, 0.9, 2.0, 1/30), condition directions, callees and arguments agree. One precision fix:
// the total ticks are (frame_count * 0.033333335f) rounded to float, then * the field (retail 0xe9d61-0xe9d6d: cvtsi2ss,
// mulss k, cvtsi2ss field, mulss); the single expression compiled as (field * frames) * k (235 -> 234). Left (no
// behaviour difference): register homes, the result byte at [esp+0x12] vs duplicated returns, the divide from memory.
// m3 lane 11 r83 (fast): re-measured round 187's form (first_person_name = NONE first, the short-slot hand getter,
// TEST_FIELD_BIT on the 0x1b test, the fallback switch, four `goto done`) 107 (1188 B, aligned 68, masked 12) and lane 14
// r68's nested lunge-target read on it 105 (masked 10); on 105, re-measured: first_person_name after the weapon getter
// 317, is_biped renamed to sort after result 105; new: result renamed to sort before is_biped 105 (the bytes do not follow
// names), the existing unit_action_pool_hand_weapon_get(g_4e0300, unit_index, 0) 320 (1193), `current` declared at
// function scope 105, the weapon getter before the actions lookup 360 (1144); on this body the pool hand getter 306
// (1202); permuter 600 s, 12 tries, best 105. Left on 105: retail's NONE register is first_person_name's EBX and also
// serves the hand getter's `cmp ax, bx` and the weapon default store (ours EBP, immediates), type EBX vs EBP,
// pool/offset EDI/EBX vs EBP/EDI, the result/is_biped bytes, current in EAX vs ESI and the divide from memory.
// m3 lane 5 r169 (fast): the six `goto play` and the label are gone at the same code (234): mode 1's two by a NONE
// name default with `name == NONE` tests, the combo cases' four by switching on `active ? melee_name : NONE` with the
// mode selection inside the NONE/0xe000607-0xe00060a case. Tried: the whole choice without gotos under an
// `if (name == NONE)` after the switch 264 (1189 B); on the kept body, the default and mode-else exits folded into
// `if (name != NONE)` round the play block 351 (1228 B); the channel declared with the top locals 300. Retail has one
// exit reading the result byte, which every goto-free spelling tried here leaves as separate returns.
// MiniMax R (11-06): no new form, no lower score (235); the frame agrees and the pool, index, type, name and completion-byte homes of the note below are unchanged.
// M3 lane 14 r83 (fast; same code: `result = true;` after the lunge-speed read, the arms swapped with the lunge arm first,
// `if (!animation && channel)` then one `if (!animation) return`, `current` as NONE plus an if): the default and mode-else
// exits as `break` with `if (name == NONE) return result;` after the switch 324 (1224 B: the 0xe9780 fallback block is
// duplicated), the same without the type initialiser 324; goto-free single exit (`if (name != NONE)` round the play block,
// a `play` flag cleared when no animation is found, the tail under `if (play)`; a flag form) 329 (1224 B); the 0xe9780
// fallback as a switch re-measured 237 (1192 B, masked 35 against 51); the lunge target defaulting to target_index with
// the 0x1c8 read nested in the `>= 1.0f` test 254 (1201 B); the short-slot hand getter with the fallback switch 267 (1191 B).
// m3 lane 1 r95 (plain; the fast check carries a CAVEAT for callee 0x168470; register-masked aligned counts in brackets,
// base 48): re-measured the existing short-slot unit_action_hand_weapon_get(unit, 0) alone 297 (1196 B) [46] and the lunge
// speed read at its compare and at the divide (here through one `byte *` melee-globals local) 234 (1197 B) [47] (the
// whole tail block, from the speed load to the 0x399 test, is then retail's code but for retail's `mov byte [esp+0x12], 1`
// result store); new: with that, the 0xe9780 fallback as a switch (retail's sub/je and range-tree code) 237 (1192 B) [34];
// with both and the short-slot getter 252 (1191 B) [32]; those with first_person_name = NONE declared first 311 (1191 B)
// [24]: left at that masking are the prologue's pool load (retail loads it before `push esi`), the getter's NONE compare
// and default store (retail `cmp ax, bx` and a register store from first_person_name's EBX, ours an immediate), the 0x1b
// bit (retail `shr eax, 0x1b; test al, 1`), and retail's single exit through the result byte at [esp+0x12]; the switch
// and per-use speed with TEST_FIELD_BIT on the 0x1b test 311 (1188 B, retail's length) [32], that with the short-slot
// getter 241 (1188 B) [31].
// m3 lane 4 r191 (plain, with 0xe7900/0xe8420/0xa91c0/0xe9ed0/0xea090; aligned/masked 141/51; retail's prologue pushes EBX
// before writing it, so no register argument there): the damage/end tick stores as conditional expressions 236 (1194 B);
// the 0x1b test on the `actions` already fetched 343 (1167 B; 0xe9ed0 MATCH -> 27, 0xea090 MATCH -> 3); the mode selection
// as `switch (mode)` 244 (1193 B); `target_index != NONE && is_biped` 234 (3 lines differ); `first_person_name != NONE &&
// weapon_index != NONE` 234 (2 lines differ).
// P473: Whole mode-dependent selection group interleaved with the four original
// continuation groups, EABCD/AEBCD/ABECD/ABCED/EDCBA/DECBA/DCEBA/DCBEA:
// 234/234/234/234/234/234/234/234. NOT kept: no native register/result-home equality; original full body restored.
// @retail 0xe9a20
bool __stdcall function_e9a20(long unit_index, long mode, long target_index, byte flags)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	long weapon_index = unit_action_weapon_get(unit, false);
	bool is_biped = (byte)(1 << unit->object_type) & 1;
	bool result = false;
	long name = NONE;
	long type;
	long first_person_name = NONE;

	long current = UNIT_ACTION_ACTIVE(UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index)), 0x1b) ? actions->melee_name : NONE;

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
		break;
	case 0xa0005bb:
		name = 0xb0005b2;
		type = 0x26;
		first_person_name = name;
		break;
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
		break;
	case 0x130005bc:
		name = 0x140005b3;
		type = 0x28;
		first_person_name = name;
		break;
	case NONE:
	case 0xe000607:
	case 0xe000608:
	case 0xe000609:
	case 0xe00060a:
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
				}
				if (name == NONE && first_person_weapon_animation_get(weapon_index, 0xc000073, NULL))
				{
					name = 0xc000073;
					type = 0x1c;
					first_person_name = name;
				}
			}
			if (name == NONE)
			{
				name = 0x500000a;
				type = 0x11;
				first_person_name = name;
			}
		}
		else
		{
			return result;
		}
		break;
	default:
		return result;
	}

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
		real seconds = frame_count * 0.033333335f;

		actions->melee_ticks = unit_action_round(seconds * (real)g_510c54->field_2_3);
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
void __stdcall function_cf3d0(long unit_index, long name, byte flags, real scale);

/* a melee attack (type 27): its animation, once any earlier one has
   passed its end frame */
// @retail 0xe9ed0
bool __stdcall unit_action_melee_attack(long unit_index, s_unit_request *request)
{
	s_unit_request_melee_attack *melee = (s_unit_request_melee_attack *)request;
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	long weapon_index = unit_action_hand_weapon_get(unit, 0);
	s_record_pool *objects = g_4e0300;
	bool can_melee = true;
	bool result = false;

	if (weapon_index != NONE)
	{
		s_unit_action_unit *weapon = ((s_unit_action_header *)objects->data)[weapon_index & 0xffff].unit;
		byte *s_type_67e06b = g_4e3b44[weapon->definition_index & 0xffff].bytes;

		if ((TEST_FIELD_BIT((*(dword *)(s_type_67e06b + 0x12c) >> 9) & 1)) ||
			*((byte *)weapon + 0x20c) == 1 || *((byte *)weapon + 0x20c) == 2)
			can_melee = false;
	}

	s_unit_action_unit *current = ((s_unit_action_header *)objects->data)[unit_index & 0xffff].unit;

	if ((TEST_FIELD_BIT((current->flags_134 >> 25) & 1)) || TEST_FIELD_BIT(UNIT_ACTION_ACTIVE(UNIT_ACTIONS_GET(current), 0x16)))
		can_melee = false;
	if (!(TEST_FIELD_BIT(UNIT_ACTION_ACTIVE(UNIT_ACTIONS_GET(current), 0x1b)) &&
		(actions->melee_end_ticks == NONE || actions->melee_counter < actions->melee_end_ticks)) && can_melee)
	{
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
			result = true;
		}
	}
	return result;
}

/* a melee attack's ticks: its damage frame, then the next attack in the
   combo (or the end) */
// @retail 0xea090
bool __stdcall unit_action_melee_attack_update(long unit_index, long type)
{
	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));
	bool result = false;

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
			long ticks = unit_action_round_long((real)g_510c54->field_2_3 * seconds);
			s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);

			if (*((byte *)unit + 0x3dc) != 6 || *(long *)((byte *)unit + 0x3ec) < 0 ||
				*(long *)((byte *)unit + 0x3ec) <= ticks)
			{
				finished = true;
			}
		}
		if (++actions->melee_counter < actions->melee_ticks && !finished)
			return true;
		result = function_e9a20(unit_index, 0, NONE, actions->melee_flags);
	}
	if (!result)
	{
		actions->active[0] &= ~0x8000000;
		actions->melee_name = NONE;
	}
	return result;
}

// @retail 0xea1c0
void __stdcall unit_action_melee_attack_interrupted(long unit_index, long type)
{
	UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index))->melee_name = NONE;
}

bool __stdcall function_c92c0(long unit_index, long vehicle_index, short seat_index, long *a, bool *b);
void function_e5930(long unit_index);
long __stdcall function_c7160(long unit_index, short seat_index, long vehicle_index, long a, long b);
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
	bool reset = false;
	bool result = false;

	if (force || function_c92c0(unit_index, vehicle_index, seat_index, NULL, NULL))
	{
		s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
		s_unit_action_unit *const &unit_ref = UNIT_ACTION_HEADER_GET(unit_index)->unit;

		if (unit->parent_object_index == NONE)
		{
			if (UNIT_ACTIONS_GET(unit_ref)->unknown36 != 0)
				function_e6f90(unit_index);

			s_unit_action_header *header = (s_unit_action_header *)(g_4e0300->data + (unit_index & 0xffff) * 12);

			if ((1 << UNIT_ACTION_HEADER_GET(unit_index)->type) & 1)
			{
				long state = *((byte *)header->unit + 0x3dc);

				if (state == 4 || state == 5)
					function_e5930(unit_index);
			}
			function_e69c0(unit_index, 0x1d);
			function_e69c0(unit_index, 0x1c);
			function_e69c0(unit_index, 0x20);
			if (type != 0x1f)
				function_e69c0(unit_index, 0x1f);

			long state_name = NONE;

			if (!keep_animation)
				state_name = function_c7160(unit_index, seat_index, vehicle_index, 0, 0);

			bool is_vehicle = TEST_FIELD_BIT(((1u << UNIT_ACTION_HEADER_GET(unit_index)->type) >> 1) & 1);
			s_unit_action_unit *vehicle = UNIT_ACTION_UNIT_GET(vehicle_index);
			byte *seat = *(byte **)(g_4e3b44[vehicle->definition_index & 0xffff].bytes + 0x1cc) + seat_index * 0xb0;

			if (unit->unknown13c != NONE)
				function_bbe60(unit->definition_index);
			function_ce040(unit_index);

			s_unit_action_unit *current = UNIT_ACTION_UNIT_GET(unit_index);
			transform4x3f const *field_50 =
				(transform4x3f const *)((byte *)current + *(short *)((byte *)current + 0x116));
			s_object_marker marker;
			transform4x3f relative;

			function_b8d30(vehicle_index, *(long *)(seat + 8), &marker, 1, false);
			{
				transform4x3f inverse;

				function_141590(&marker.matrix, &inverse);
				function_142a60(&inverse, field_50, &relative);
			}
			function_b8ee0(vehicle_index, *(long *)(seat + 8), unit_index, 0);
			unit->seat_index = seat_index;
			current = UNIT_ACTION_UNIT_GET(unit_index);
			if (current->unknown0d4 != NONE)
				function_b58c0(current->unknown0d4, 0x800);
			function_cc810(vehicle_index);

			if (!is_vehicle)
			{
				function_e68c0(0x13, unit_index);

				long weapon_index = unit_action_hand_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), 0);

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
				long seat_name = *(long *)(seat + 4);

				function_10f430(unit_index, seat_name, 0x7000101, 0x7000101,
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
			if (unit->unknown13c != NONE && (TEST_FIELD_BIT((*(dword *)seat >> 2) & 1)))
			{
				byte *player = g_4e8c24->data + (unit->unknown13c & 0xffff) * 0x21c;

				if (*(short *)(player + 0x28) != NONE)
					function_1874b0(*(short *)(player + 0x28), (vector3f const *)((byte *)vehicle + 0x150));
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
	}

	if (animation_reset)
		*animation_reset = reset;
	return result;
}

void function_b9c60(long object_index, bool flag);
void function_ba690(long object_index, long *state_count, unsigned char **states, long *a, long *b);
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
		if ((TEST_FIELD_BIT((*(dword *)seat >> 19) & 1)) && *(long *)(seat + 0xac) != NONE)
		{
			unsigned char *states;
			long state_count;
			long choices;
			bool hidden = false;

			function_ba690(unit->parent_object_index, &state_count, &states, NULL, &choices);
			if (*(long *)(seat + 0xac) < state_count)
				hidden = ((char *)choices)[*(long *)(seat + 0xac) * 8 + 1] < 3;
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
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	s_unit_request_vehicle_entry *entry = (s_unit_request_vehicle_entry *)request;
	bool result = false;
	bool reset = false;

	if (function_ea1f0(unit_index, entry->vehicle_index, entry->seat_index, entry->keep_animation, entry->type,
		&reset, entry->force))
	{
		if (reset)
		{
			function_ea6b0(unit_index, entry->type);
		}
		else
		{
			actions->active[0] |= 0x10000000;
			function_d1360(unit->parent_object_index, unit->seat_index, 0, 1);
		}
		result = true;
	}
	return result;
}

long function_10eef0(long object_index, bool alternate, bool no_request);
void function_d12b0(long vehicle_index, long seat_index, bool a, bool b);
short __stdcall function_cdeb0(long unit_index, long state_name, short slot, short direction);
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
	bool is_vehicle = ((1u << unit->object_type) >> 1) & 1;
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
	function_d12b0(vehicle_index, seat_index, 0, 0);
	*(long *)((byte *)unit + 0x250) = vehicle_index;
	*(long *)((byte *)unit + 0x254) = g_510c54->game_time;
	unit->seat_index = NONE;
	function_b9a90(unit_index);
	function_b9b90(unit_index, false);

	function_b9c60(unit_index, false);
	s_unit_action_unit *current = UNIT_ACTION_UNIT_GET(unit_index);

	if (current->unknown0d4 != NONE)
		function_b58c0(current->unknown0d4, 0x800);
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
				goto done;
			function_b9c60(unit_index, false);
			function_a8b10(unit_index);
		}
		result = true;
		function_d12b0(vehicle_index, seat_index, 0, 0);
	}
done:
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

	if (!unit_action_active(unit_index, 0x1d) && !unit_action_active(unit_index, 0x1f) &&
		!unit_action_active(unit_index, 0x20))
	{
		function_e69c0(unit_index, 0x1d);
		function_e69c0(unit_index, 0x1c);
		function_e69c0(unit_index, 0x1f);
		function_e69c0(unit_index, 0x20);
		*((byte *)unit + 0x258) = 0;
		if (function_eaad0(unit_index))
		{
			actions->active[0] |= 0x20000000;
			if (*((bool *)request + 4))
				actions->unknown00 |= 1;
			else
				actions->unknown00 &= ~1;
			if (unit->unknown12c != NONE)
				function_1bba20(unit->unknown12c);
			result = true;
		}
	}
	return result;
}

transform4x3f *function_ba160(long object_index, transform4x3f *matrix);

/* the end of a vehicle exit: the unit leaves the seat with its exit
   animation's velocity */
// MiniMax E (11-01): matched (was 35, plain-confirmed): the channel's animation id read into a local
// before the call, the velocity components as named reals scaled in place, and the sums
// up * z + left * y + forward * x.
// score 35 (was 103): Frame 0x40 and retail length agree; the graph chain is in EDX (retail EBX, the
// animation id loaded early into EDX) and the velocity loads run k, j, i (retail i, j, k; the sums
// then start with up * k). Graph at call 103; complete component preload
// 102; both 102; result reuse 131; graph/result reuse 131; preload/result reuse 135; all 135;
// matrix declaration first 103; scale once 55 (plain kept), with complete component preload 97.
// vector3f_scale for the scale 50, with the word flag through word_flags_set 35 (kept);
// matrix_transform_vector 55/46; scale statements k, j, i 58; `scale * v` 55; sums written
// forward first 55/40; scale read per use 103; vector3f_assign 35; graph at the call, the
// model local only, a channel pointer 35; scalar copies 97/102; a byte OR through the helper 48.
// Round 21 (lane 13): the existing 0x142640 called for the scale and rotation 254 (296 bytes: LTCG
// keeps the call, its file is /Ob1 and it is not inline), with a __forceinline declaration 254;
// state declared before the definition chain 35; the channel's animation id read into a local
// before the call 10 (fixes the whole EBX/EDX chain; a one-use local, kept only with a match);
// `real scale` then x/y/z copies of the velocity scaled in place and up*z + left*y + forward*x
// rows 25; both together MATCH (not kept: the scaled copies write the helper body out by hand).
// BLOCKED-BY 0x142640: retail inlines it here; once it is inline its call plus the id local should match.
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

	c_type_709360 animation_id = state->channels[0].animation_id;

	state->animation_velocity_get(animation_id, state->channels[0].frame_position * 0.033333335f,
		state->channels[0].rate, (long)graph, &velocity);

	transform4x3f *matrix = function_ba160(unit_index, &matrix_storage);
	vector3f world;

	real scale = matrix->scale;
	real velocity_x = velocity.i;
	real velocity_y = velocity.j;
	real velocity_z = velocity.k;

	if (scale != 1.0f)
	{
		velocity_x *= scale;
		velocity_y *= scale;
		velocity_z *= scale;
	}
	world.i = matrix->up.i * velocity_z + matrix->left.i * velocity_y + matrix->forward.i * velocity_x;
	world.j = matrix->up.j * velocity_z + matrix->left.j * velocity_y + matrix->forward.j * velocity_x;
	world.k = matrix->up.k * velocity_z + matrix->left.k * velocity_y + matrix->forward.k * velocity_x;
	function_ea8e0(unit_index, hurried);
	unit->linear_velocity.i += world.i;
	unit->linear_velocity.j += world.j;
	unit->linear_velocity.k += world.k;
	if (push && unit_index != NONE)
	{
		word_flags_set(&UNIT_ACTION_UNIT_GET(unit_index)->flags_10a, 5, true);
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

	function_10f630(unit_index, &action_name, &state_name);
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
	bool result = true;

	if (UNIT_ACTION_UNIT_GET(unit_index)->unknown12c != NONE)
	{
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
		for (occupant = (byte *)function_d05c0(&iterator); occupant; occupant = (byte *)function_d05c0(&iterator))
		{
			if (iterator.seat_index == seat_index)
			{
				byte *other = (byte *)function_badc0(iterator.unit_index, 3);
				short other_team = NONE;

				if (other)
					other_team = *(short *)(other + 0x138);

				if (!function_1df560(team, other_team))
				{
					result = false;
					break;
				}
			}
			else if (iterator.seat_index != NONE)
			{
				byte *seat = *(byte **)(local_a2a045 + 0x1cc) + iterator.seat_index * 0xb0;

				if (TEST_FIELD_BIT((*(dword *)seat >> 11) & 1) && *(short *)(seat + 0x3e) == seat_index &&
					(*(long *)(occupant + 0x13c) != NONE || unit_action_active(iterator.unit_index, 0x1f)))
				{
					result = false;
					break;
				}
			}
		}
	}
	return result;
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
	bool result = false;

	if (vehicle_index != NONE)
	{
		s_unit_action_header *vehicle_header = UNIT_ACTION_HEADER_GET(vehicle_index);
		s_unit_action_unit *vehicle = vehicle_header->unit;

		if ((1 << vehicle->object_type) & 3)
		{
			byte *local_a2a045 = g_4e3b44[vehicle->definition_index & 0xffff].bytes;
			byte *seat = *(byte **)(local_a2a045 + 0x1cc) + unit->seat_index * 0xb0;

			if (!TEST_FIELD_BIT(UNIT_ACTION_ACTIVE(actions, 0x1f)) && TEST_FIELD_BIT((*(dword *)seat >> 11) & 1))
			{
				short board_seat_index = *(short *)(seat + 0x3e);

				if (board_seat_index != NONE)
				{
					if (function_eaf50(unit_index, vehicle_index, board_seat_index))
					{
						function_e69c0(unit_index, 0x1d);
						function_e69c0(unit_index, 0x1c);
						function_e69c0(unit_index, 0x1f);
						function_e69c0(unit_index, 0x20);
						if (function_113e90(unit_index, 0x50000c3, 0.0f, NULL, 0))
						{
							actions->active[0] |= 0x80000000;
							*((byte *)unit + 0x258) = 0;
							function_a8b90(unit_index);
							if ((TEST_FIELD_BIT((*(dword *)seat >> 13) & 1)) &&
								(TEST_FIELD_BIT((*(dword *)(*(byte **)(local_a2a045 + 0x1cc) + *(short *)(seat + 0x3e) * 0xb0) >> 2) & 1)))
							{
								function_d12b0(vehicle_index, *(short *)(seat + 0x3e), 0, 0);
								if (unit->unknown13c != NONE)
								{
									s_event_player *player = event_player_get(unit->unknown13c);

									if (player->local_index != NONE)
										function_1874b0(player->local_index, (vector3f const *)((byte *)vehicle + 0x150));
								}
							}
							result = true;
							function_cc810(vehicle_index);
						}
					}
				}
			}
		}
	}
	return result;
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
	if (TEST_FIELD_BIT((*(dword *)seat >> 13) & 1))
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
	bool result = false;
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	long vehicle_index = unit->parent_object_index;
	if (vehicle_index == NONE || unit->seat_index == NONE)
		goto done;
	if (*((bool *)request + 5))
		function_e69c0(unit_index, 0x20);
	if (!UNIT_ACTION_ACTIVE(UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index)), 0x20))
	{
		function_e69c0(unit_index, 0x1d);
		function_e69c0(unit_index, 0x1c);
		function_e69c0(unit_index, 0x1f);
		function_e69c0(unit_index, 0x1b);
		function_e69c0(unit_index, 0x1a);
		function_e69c0(unit_index, 0x16);
		if (*((bool *)request + 5))
		{
			function_e6960(unit_index, 0x20);
			result = true;
		}
		else if (function_10f430(unit_index, 0x7000101, 0x7000101, 0x7000101, 0x80000c4, 0.0f, 0, 2))
		{
			if (*((bool *)request + 4))
				actions->unknown00 |= 2;
			else
				actions->unknown00 &= ~2;
			bit_vector_set(actions->active, 0x20, true);
			result = true;
		}
		if (result)
			function_cc810(vehicle_index);
	}
done:
	return result;
}

void function_152140(long player_index);

// @retail 0xeb520
void __stdcall function_eb520(long unit_index, long type)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	bool push = ((dword)UNIT_ACTIONS_GET(unit)->unknown00 >> 1) & 1;

	if (unit->unknown13c != NONE)
		function_152140(unit->unknown13c);
	function_eac90(unit_index, false, push);
	if ((1 << UNIT_ACTION_HEADER_GET(unit_index)->type) & 1)
	{
		word *flags = (word *)((byte *)UNIT_ACTION_UNIT_GET(unit_index) + 0x348);

		*flags |= 0x40;
	}
}

// @retail 0xeb5a0
bool __stdcall unit_action_vehicle_ejection_update(long unit_index, long type)
{
	bool result = function_114040(unit_index, 0x80000c4);

	if (!result)
		function_eb520(unit_index, type);
	return result;
}

struct s_unit_request_vehicle_flip
{
	long type;
	long flipper_index;
};

struct s_damage_owner
{
	long player_index;
	long object_index;
	short team;
};

__declspec(noinline) bool function_f5dc0(long object_index);
void __stdcall object_get_damage_owner(long object_index, s_damage_owner *owner);

/* flips a vehicle back over (type 33): the way it rolls, from how far over
   it is and where the flipping unit stands */
// @retail 0xeb5d0
bool __stdcall unit_action_vehicle_flip(long unit_index, s_unit_request *request)
{
	bool result = false;
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);

	if (unit->object_type == 1)
	{
		if (function_f5dc0(unit_index))
		{
			s_unit_request_vehicle_flip *flip = (s_unit_request_vehicle_flip *)request;
			s_damage_owner owner;
			long flipper_index = flip->flipper_index;

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
				vector3f offset;
				vector3f side;

				vector3d_from_points3d(&function_ba160(flipper_index, &flipper_matrix_storage)->position,
					&function_ba160(unit_index, &unit_matrix_storage)->position, &offset);
				side.k = g_4687b0->i * offset.j - g_4687b0->j * offset.i;
				side.j = g_4687b0->k * offset.i - g_4687b0->i * offset.k;
				side.i = g_4687b0->j * offset.k - g_4687b0->k * offset.j;
				*((byte *)unit + 0x34d) = (byte)((dot3f(&side, &unit->forward) > 0.0f) + 1);
			}
			else
			{
				*((byte *)unit + 0x34d) = 1;
			}
			word *flags = (word *)((byte *)unit + 0x348);
			*flags |= 0x40;
			*((byte *)unit + 0x34e) = 0;

			object_get_damage_owner(flip->flipper_index, &owner);

			byte *current = (byte *)UNIT_ACTION_UNIT_GET(unit_index);

			*(long *)(current + 0xc8) = owner.object_index;
			*(long *)(current + 0xc4) = owner.player_index;
			*(short *)(current + 0xc2) = owner.team;
			result = true;
		}
	}
	return result;
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
	bool result = false;
	s_unit_request_move *move = (s_unit_request_move *)request;
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);

	if (unit->parent_object_index == NONE)
	{
		if (move->immediate)
		{
			vector3f facing;

			vector3f_assign(&facing, move->facing.x, move->facing.y, 0.0f);
			function_b9a50(unit_index);
			if (function_30bf0(&facing) > 0.0f)
				function_b75a0(unit_index, NULL, &facing, g_4687b0, NULL, false);
			function_e56f0(unit_index, &move->point);

			s_unit_actions *current = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));

			memset(current->active, 0, sizeof(current->active));
			current->unknown36 = 0;
			result = true;
		}
		else if (!(bit_vector_test(actions->active, move->type)))
		{
			vector3f facing;

			vector3f_assign(&facing, move->facing.x, move->facing.y, 0.0f);
			if (function_ee090(unit_index, 0x50000cb, 1, 0x60000cc, &move->point, &facing))
			{
				bit_vector_set(actions->active, move->type, true);
				result = true;
			}
		}
	}
	return result;
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

	if (!(actions->active[request->type >> 5] & (1 << (request->type & 0x1f))) &&
		function_ee090(unit_index, 0x50000cb, 1, 0x60000cd, (point3f const *)((byte *)request + 0x14),
		(vector3f const *)((byte *)request + 8)))
	{
		actions->active[request->type >> 5] |= 1 << (request->type & 0x1f);
		result = true;
	}
	return result;
}

/* a unit's facing in its seat (or its own when it has no parent) */
PRIVATE __forceinline void unit_action_seat_facing_get(s_unit_action_unit *unit, vector3f *facing)
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

		matrix_transform_vector(matrix, &unit->forward, facing);
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
// The NONE state for the action switch's default and the test after the switch (instead of a goto from the
// default) give retail's layout; kept only with this byte match.
// @retail 0xebb40
bool __stdcall function_ebb40(long unit_index, s_unit_request *request)
{
	bool result = false;
	{
		s_unit_request_seat_animation *seat_request = (s_unit_request_seat_animation *)request;
		s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
		short mode = seat_request->mode;
		point3f point;
		vector3f facing;
		long state_name;
		long context;

		if (mode != 0 && UNIT_ACTIONS_GET(unit)->unknown36 != 7)
		{
			point = seat_request->point;
			facing = seat_request->facing;
			context = 0x5000049;
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
				goto done;
			}
		}
		else
		{
			function_b9dd0(unit_index, &point);
			unit_action_seat_facing_get(unit, &facing);
			context = 0x400000c;

			long field_c_4;
			long current_action;

			if (!function_10f630(unit_index, &current_action, &field_c_4))
				goto done;
			switch (current_action)
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
				state_name = NONE;
				break;
			}
			if (state_name == NONE)
				goto done;
		}
		result = function_ee090(unit_index, state_name, 6, context, &point, &facing);
	}
done:
	return result;
}

/* plays the return from a seat animation (type 37): the request's, or the
   one following the unit's current animation */
// The NONE state for the action switch's default and the test after the switch (instead of a goto from the
// default) give retail's layout; kept only with this byte match.
// score 76 (was 76): Frame and entry carriers agree; one extra byte, provider output role and
// selector tail blocks remain. All original values retained.
// Related common result and precise contexts 76; reverse/third-first case orders 76/76; initially
// selected request context 113; isolated common exit 109; precise contexts without common result
// 133; shared dispatch 134; selected-name result 159. Keep original case order, one dispatch,
// genuine contexts and one Boolean completion exit; selected provider output unchanged.
// Round 34: switching on 0x10f630's first output, as retail does (a value fix, as in 0xebb40),
// 76 with retail's output slots; the same merged case block (0x10000229) is placed after the call.
// Round 50: a compile-only screen reproduces the layout: VC sorts each switch's cases by value, so
// no case order changes it, and the merged mode-1/0x110001b4 block always stays in the request
// branch (retail: after the query's 0x120001b5 block); swapping the if/else branches gives the same
// code. The mode mapped to the current-animation name and one shared name switch 103 (477 bytes,
// the request cases threaded, 0x1100022a duplicated); the query switch as an if/else chain 89;
// `return result` for the mode default 121; the name switch inside `if (query)` 76; default first
// 76. A brute-force screen of switch/if dispatch, the three exits as goto/return result/return
// false and two declaration orders (144 forms) never placed that block in the query branch.
// Round 29: one mode switch after the if/else (the query maps the animation to a mode) 139
// (475 bytes, not threaded); the call returned with `return false` at done 145; the query
// branch first through a named bool 123; the mode default `return false` 121 (487 bytes, the
// result on the stack); the call in each branch 155 (520 bytes). Compile-only screens: the
// context assigned after either switch, or the copies after the switch, stop the case blocks
// merging; the context in every case keeps our placement.
// Round 58: no `mode` local 76; the request context before the copies 83; the query context after the
// 0x10f630 test 83; the query outputs declared before the b9dd0/facing calls 76; no unit local (the lookup
// at each use) 137 (494 bytes). Harness: all 36 orders of the two branches' copy/context statements keep
// our layout.
// @retail 0xebd30
bool __stdcall function_ebd30(long unit_index, s_unit_request *request)
{
	bool result = false;
	{
		s_unit_request_seat_animation *seat_request = (s_unit_request_seat_animation *)request;
		s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
		short mode = seat_request->mode;
		point3f point;
		vector3f facing;
		long state_name;
		long context;

		if (mode != 0 && UNIT_ACTIONS_GET(unit)->unknown36 != 6)
		{
			point = seat_request->point;
			facing = seat_request->facing;
			context = 0x5000049;
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
				goto done;
			}
		}
		else
		{
			function_b9dd0(unit_index, &point);
			unit_action_seat_facing_get(unit, &facing);
			context = 0x400000c;

			long field_c_4;
			long current_action;

			if (!function_10f630(unit_index, &current_action, &field_c_4))
				goto done;
			switch (current_action)
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
				state_name = NONE;
				break;
			}
			if (state_name == NONE)
				goto done;
		}
		result = function_ee090(unit_index, state_name, 7, context, &point, &facing);
	}
done:
	return result;
}

/* plays a named seat animation where the unit is (type 38) */
// @retail 0xebf20
bool __stdcall function_ebf20(long unit_index, s_unit_request *request)
{
	point3f point;
	vector3f facing;

	function_b9dd0(unit_index, &point);
	unit_action_seat_facing_get(UNIT_ACTION_UNIT_GET(unit_index), &facing);

	long state_name = *(long *)((byte *)request + 8);
	long mode = *(long *)((byte *)request + 4);

	return function_ee090(unit_index, state_name, mode, 0x5000049, &point, &facing);
}

/* sits a unit down from its pose (type 39) */
// @retail 0xec050
bool __stdcall function_ec050(long unit_index, s_unit_request *request)
{
	bool result = false;

	if (UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index))->unknown36 != 0)
	{
		if (function_10f430(unit_index, 0x7000101, 0x7000101, 0x7000101, 0x400004a, 0.1f, 0, 0))
			result = true;
		else
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
		s_unit_action_unit *current = UNIT_ACTION_UNIT_GET(unit_index);
		transform4x3f nodes = *(transform4x3f *)((byte *)current + *(short *)((byte *)current + 0x116));
		transform4x3f matrix;

		if (function_10f9b0(unit_index, state_name, action_name, 3, &matrix, false))
		{
			transform4x3f inverse;
			vector3f facing;

			function_141590(&matrix, &inverse);
			function_142a60(&nodes, &inverse, &matrix);
			facing = matrix.forward;
			facing.k = 0.0f;
			if (function_30bf0(&facing) > 0.0f)
				function_b75a0(unit_index, NULL, &facing, g_4687b0, NULL, false);
		}
		function_e5930(unit_index);
		function_10f430(unit_index, state_name, 0x7000101, 0x7000101, action_name, 0.0f, 0, 2);

		{
			byte *seat_transform = (byte *)unit + *(short *)((byte *)unit + 0x10e);
			transform4x3f object_matrix;
			transform4x3f object_inverse;
			quaternionf orientation;

			function_ba160(unit_index, &object_matrix);
			function_141590(&object_matrix, &object_inverse);
			function_142a60(&object_inverse, &nodes, &matrix);
			function_141f60(&matrix.rotation, &orientation);
			*(point3f *)(seat_transform + 0x10) = matrix.position;
			*(quaternionf *)seat_transform = orientation;
			function_bd020(unit_index);
		}
		actions->unknown36 = 0;
	}
	else
	{
		if (!TEST_FIELD_BIT((unit->flags_c0 >> 6) & 1))
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
		dword *active = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index))->active;

		active[1] |= 0x100;
		result = true;
	}
	return result;
}

void function_e5750(long unit_index);

/* The biped test is nested in itself: at one level of nesting the build expands function_e5750 here, where
   retail calls it (as it does under the two tests of function_ec9a0). */
// MiniMax I (11-09): no new form, no lower score (52); function_e5750 still expands here (95 bytes) where retail
// calls it out of line (67 bytes), and the unit pointer is still read after the 0xec0d0 call where retail reads it before.
// MiniMax H (11-07): no new form, no lower score (52); function_e5750 still expands here (95
// bytes) where retail calls it out of line (67 bytes), and that callee is in this file and
// matches, so the expansion cannot be turned off.
// MiniMax D (11-06, run 49): no new form, no lower score (52); function_e5750 still expands here (95 bytes) where retail calls it out of line (67 bytes); that callee is in this file and matches, so it cannot be changed.
// MiniMax H (11-04): no new form, no lower score (52); the inlined callee is still the whole
// difference and the noinline spelling is not allowed.
// MiniMax A (11-04): no new form, no lower score (52). What still differs: our build inlines
// function_e5750 here (95 bytes) where retail calls it out of line (67 bytes). The callee is in
// this file and matches, as do its other callers 0xec9a0 and 0xe5930; only this call site differs.
// BLOCKED-BY 0xe5750's inlining: __declspec(noinline) on its declaration is banned.
// MiniMax E (11-01): no new form, no lower score; a plain check confirms the full
// build inlines 0xe5750 here too (the unit read before the call: 72, 110 bytes).
// score 52 (was 72): unit lookup is at use; the final helper remains expanded (95/67). Round 220 (m3 lane 3, fast):
// the type test as the mask bit `(1 << type) & 1` 62 (102 B, not folded, still inlined); the type through the existing
// unit_weapon_view_get header getter 52; the lookup through a pool copy declared first 72; the unit view pointer read
// before the call 72; diagnostic, the pointer before the call and a one-case switch 72 (still inlined).
// Early guard, comparison local, byte/long/short type, const pointer and header lookup
// all 72; direct post-call unit lookup 52.
// Lane 16 r31: the unit pointer read before the 0xec0d0 call matches with __declspec(noinline) on
// function_e5750's declaration (not allowed, docs/DECOMPILING.md); without it 0xe5750 inlines here: 72.
// Round 10 (lane 14): BLOCKED-BY 0xe5750 (finished contributor code): a compile-only screen inlines it
// under one `if` from /d2inlT70 up, unconditionally from 62 and under 0xec9a0's two tests from 90, and
// every source form here stays under the build's threshold. Pointer before the call with `!type` 72,
// with an early return 72, a bool local before the call 83; after the call an early return 52, `!type` 52.
// Round 146: definition below the final helper, post-call pointer/const pointer/header/byte/bool locals,
// zero-path return and single-exit goto all 52; original restored.
// MiniMax E (11-09): no new form, no lower score (52); retail reads the item table through the global before
// the 0xec0d0 call and keeps the unit pointer in EDI, where ours reads the table after the call and keeps the
// pointer in ESI, because our build expands the matched 0xe5750 into the call (95 bytes against retail's 67);
// 0xe5750 is finished contributor code and the only form that kept it out of line was a noinline declaration,
// which is no longer allowed.
// @retail 0xec330
void __stdcall function_ec330(long unit_index, long type)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);

	function_ec0d0(unit_index, 0x6000542, 0x400000c);
	if (unit->object_type == 0)
	{
		if (unit->object_type == 0)
			function_e5750(unit_index);
	}
}

/* raises a weapon to point (type 41): its animation, and when it ends */
// @retail 0xec380
bool __stdcall function_ec380(long unit_index, s_unit_request *request)
{
	s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));
	long mode = *(short *)((byte *)request + 4);
	long name = NONE;

	if (mode >= 0)
	{
		if (mode > 1)
		{
			if (mode == 2)
				name = 0xb00022e;
		}
		else
			name = 0xa00022d;
	}
	if (name == NONE)
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
		bit_vector_set(actions->active, 41, true);
		return true;
	}
	function_edff0(unit_index);
	actions->point_mode = *((byte *)request + 4);
	actions->point_target = *(long *)((byte *)request + 8);
	actions->point_ticks = 2;
	bit_vector_set(actions->active, 41, true);
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
void __stdcall function_d7b80(s_type_1e6529 *data, long object_index, short node_index, short unknown0c, short region_entry_index,
	vector3f const *unknown14);
long function_baf80(long object_index);
short function_0b67a0(const s_small_index *data);
long function_1eb020(long tag_index, long body_index, short *material_index);
void function_188180(point3f const *point, vector3f const *forward, long tag_index, long object_index, long index,
	long variant, c_type_47f957 material, long effect_value, s_location const *location, real scale);
extern short g_54e898;
extern vector3f *g_4687bc;

/* a raised weapon's point (type 41 update): when its time is up, it
   shoves the target in front of the unit, with damage and an impact
   effect */
// MiniMax H (11-04): no new form, no lower score (45); the first difference is the two-byte branch
// offset at +0x5c, where our block is longer, and the 0x188180 call setup after it is blocked by
// that callee's parameter order in another file.
// MiniMax R (11-06): no new form, no lower score (45); the g_54e898 copy is a dword load here and a word load in retail, which also moves the exit block two bytes; retail's 0x1eb020 writes a dword into the material slot and it is read back as a dword, ours takes a short * and reads it back sign-extended, and that width is set in another file.
// score 8 (was 8): 23 before main took 0x188180's material parameter as the 2-byte type by value (the held patch below); 31 before 0x1883e0 matched with c_type_47f957 parameters; 45 before round 220's material local as the global material type (c_type_47f957, constructed from g_54e898
// as 0x277b80 does) gives retail's word copy, so every branch target now agrees; left are the copy's schedule (retail
// loads into CX early and stores it just before the 0x0b67a0 call, ours stores at once from DX), the rigid body's
// data/offset registers in its address add, and the 0x188180 call setup (BLOCKED-BY 0x188180: retail pushes the
// material as the 2-byte type by value, held behind 0x1883e0's declarations). Round 220 (m3 lane 3, fast): material
// type 31 (kept); constructed after the component lookup 33; short rigid body index 46 (1099 B); rigid body fields at
// each use 35; component and index declared first, assigned after the material 31; rigid body through
// `rigid_bodies.data + index` 31.
// Earlier: score 45 (was 277): g_54e898 is copied with a dword load (retail `mov cx, word`), the rigid body's
// data and offset registers swap in its address add, and the 0x188180 call setup differs
// (BLOCKED-BY 0x188180: ours takes the effect value in ecx and forward in edx; retail passes the point
// in eax, forward in ecx and pushes the short material slot raw).
// Round 236: complete initial vector copy with entry damage lifetime and center/shove/impact names, all subsets 277/277/277/277/277/277/277/277.
// Keep only the whole forward-vector initialization; all workspace and name changes restored.
// Round 46: result false at entry 310; result per exit 277; result true, cleared on failure 220; material
// NONE before function_d6660 222; long rigid body index 221; long node index (nested) 202; dot through
// vector3d_from_points3d + dot3f 203; perpendicular scaled/added in place 227; side cases scaled into a
// fresh vector 99; add operands swapped 227; dot term orders xzy 97, xyz 94, zyx 99, zxy 97, yxz 99;
// dot3f either way 100/100; rigid_bodies.data 94; material read before the component 93; rigid body
// pointer 90; both 89; result declared first 54; unknown14 stored first/second/last 51/51/56;
// x + (y + z) 54; x + (z + y) 48; with unknown14 second 45; rigid body by pointer arithmetic 45;
// the material NONE store back after function_d6660 148 (retail stores it first; d6660 then overwrites it).
// Round 145: material declaration/assignment split, direct initialization and renamed material; rigid-body
// reference, base then index, const component and first-node dereference all 45; shared array base 58. Original restored.
// Round 65 (m3 lane 15, fast; base 31): the component index named ahead of the material 31 (same code); the material
// default-constructed and assigned after the component lookup 33; 0x188180's seventh parameter as the 2-byte material
// type by value, changed at its definition, five declarations and six calls together 45 (0x188180 77 -> 95: the effect
// value takes EAX and the point ECX; 0xe4d90 154 -> 168, 0xceee0 94 -> 103, 0x10c880 67 -> 65, 0x247070 29 -> 15; all
// restored): retail's 0x188180 takes its tag from the stack into EBX for 0x1883e0, so its inputs follow that callee
// (BLOCKED-BY 0x1883e0, whose declaration in src/impacts.cpp is held); the rigid body through a const reference to the
// array 31 (same code); the node count and first node read through the array at each use 35.
// m3 lane 11 r83 (fast): the impulse through vector3f_scale 31 (same code); the material built before the effect-tag
// test 36 (aligned 17); the definition re-lookup before the havok test 116; the strength as a `long` 49; the strength
// read before the 0x1eb020 call 98 (1107 B). Re-measured: `&component->rigid_bodies.data[index]` 31, the material
// copy-initialised from a temporary 31, default-constructed then `m_index = g_54e898` 31, node_count and nodes[0]
// through `rigid_bodies.data[index]` at each use (0x1d1a20's spelling) 35.
// m3 lane 5 r169 (plain; base 23: the one extra byte is the movsx of the material for 0x188180's long parameter, the rest
// is the g_54e898 word in DX stored at once where retail loads CX and stores it just before the 0x0b67a0 call, and the
// rigid body's data/offset registers): re-measured with 0x1883e0 matched, 0x188180's seventh parameter as c_type_47f957 by
// value (definition, five declarations, six calls, and s_item_impact's material declared as c_type_47f957 in items.cpp) 8 at
// 1098 B (0x10c880 69 -> 39, 0x247070 30 -> 6, 0xe4d90 168 -> 163, 0xceee0 90 -> 97 (its own material parameter is
// c_type_47f957 in retail too), 0x1883e0/0xcf040 still MATCH): HELD (needs lane 16's src/bipeds.cpp and src/unit_object_type.cpp
// and the reviewer's OK; patch in m3-lane5 build/r169/held-188180.patch). On that patch: material built after the
// component lookup 10; rigid body as `rigid_bodies.data + index` 8. On main (fast): the rigid body as a const reference to
// the element 23, the component as `s_havok_component const *` 23, 0x0b67a0 given `&component->identifier` 23, the
// strength clamp as a ternary 23 (all the same code); the impulse as `-shove.i` (unary minus) 76 (1084 B: the -1.0f
// multiplies become sign flips). No untried natural form is left for the two register choices.
// m3 lane 1 r95 (fast; base 23: eleven of the lines are branch offsets one byte off from the material's movsx for
// 0x188180's `long` parameter, BLOCKED-BY 0x188180 as above; the rest is the material in DX stored at once where retail
// holds it in CX and stores it before `mov ecx, esi`, and the rigid body's data/offset registers). Kept: the definitions
// read through the typed object/unit definition fields (same code, 23). Tried: the node index as `char` (the nodes'
// element type) 106 (1102 B); the material default-constructed with the block's locals and given a constructed value
// at the effect block 23 (same code); 0x1eb020's out parameter as `c_type_47f957 *` (definition and declaration, the
// call passing `&material_index`; 0x1eb020 still MATCH) 23 (same code); the material built first in the Havok block
// 45 (1100 B); the node-count test folded into the index test with the element read at each use 27; re-measured the
// material built after the component lookup 25 (the global load is not hoisted: retail's early CX load is a material
// built ahead of the lookup, as here; only its register differs).
// m3 lane 4 r193 (base 8 at 1098 B, fast and plain: main has 0x188180's material parameter as the 2-byte type now, so the
// 0x188180 blocker above is gone and two register choices are all that is left: (A) the default material goes through DX
// and is stored before the pool's data load, retail holds it in CX and stores it just before `mov ecx, esi` (5 lines);
// (B) ours loads the array into EDX and scales the index in EAX, retail scales into EDX and loads the array into EAX
// (3 lines)). A whole-file compile-only harness reproduces both sites as the linked build has them (0.8 s a variant).
// Fast: the default material built after the 0xb67a0 call 48 (1099 B); the node through a forceinline first-node getter
// holding an element pointer 8 (same code), the same getter indexing the element at each use 12; the effect tag through a
// forceinline getter 8 (same code); the array pointer in a local used for both reads 32; the index masked to a word at
// the element 75 (1103 B); the default material through a forceinline getter returning the type 8 (same code); permuter
// (27 tries) 8. Harness only, all the body's code unless noted: 0 to 33 unused locals; the index, or the component,
// declared at the top of the function; index + array, byte arithmetic either way round, a named offset; element getters
// taking (index, component) or (component, index); the default material assigned to a default-built local, built from a
// short local, or filled through an out-parameter getter; the pool, the masked index or the pool's data named ahead of the
// material; the definition and the model through forceinline tag getters, the model in a local, the tag assigned inside
// its test; a short index (word compare, 3 lines off). Diagnostics: the material takes CX when one more scratch value is
// loaded between the effect tag test and it (an extra copy placed ahead of it), or one fewer before it (no model lookup),
// and a callee without a register argument moves the pool's data to ECX. So the scratch choice follows the order values
// are loaded in and 0xb67a0's ECX argument, not the spelling of these statements: the lead is a statement whose value
// retail loads between the tag test and the material (or one fewer ahead of it) without adding an instruction.
// @retail 0xec4f0
bool __stdcall function_ec4f0(long unit_index, long type)
{
	bool result = true;
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_definition *local_98b918 = (s_unit_definition *)g_4e3b44[unit->definition_index & 0xffff].bytes;
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	long target_index = actions->point_target;

	actions->point_ticks--;
	if (target_index == NONE)
	{
		result = false;
	}
	else if (actions->point_ticks <= 0)
	{
		byte *target = (byte *)function_badc0(target_index, NONE);

		if (target)
		{
			s_object_definition *target_definition = (s_object_definition *)g_4e3b44[*(long *)target & 0xffff].bytes;
			s_unit_action_unit *root = UNIT_ACTION_UNIT_GET(function_baf80(target_index));
			vector3f direction = unit->forward;
			point3f target_center;
			point3f unit_center;

			direction.k = 0.0f;
			if (function_30bf0(&direction) != 0.0f)
			{
				vector3f perpendicular;

				perpendicular.i = 0.0f - direction.j;
				perpendicular.j = direction.i;
				perpendicular.k = 0.0f;

				function_b9dd0(target_index, &target_center);
				function_b9dd0(unit_index, &unit_center);
				if ((target_center.x - unit_center.x) * direction.i + ((target_center.z - unit_center.z) * direction.k +
					(target_center.y - unit_center.y) * direction.j) > 0.0f)
				{
					switch (actions->point_mode)
					{
					case 0:
						vector3f_scale(&direction, 3.0f, &direction);
						break;
					case 1:
					{
						vector3f ahead;

						vector3f_scale(&perpendicular, 3.0f, &perpendicular);
						vector3f_scale(&direction, 0.3f, &ahead);
						vector3f_add(&ahead, &perpendicular, &direction);
						break;
					}
					case 2:
					{
						vector3f ahead;

						vector3f_scale(&perpendicular, -3.0f, &perpendicular);
						vector3f_scale(&direction, 0.3f, &ahead);
						vector3f_add(&ahead, &perpendicular, &direction);
						break;
					}
					}
					direction.k = 1.5f;

					vector3f shove = direction;

					if (function_30bf0(&shove) != 0.0f)
					{
						if (local_98b918->unit.boarding_melee.obstacle_smash_damage.tag_index != NONE)
						{
							s_type_1e6529 damage;

							damage.material_index = NONE;
							function_d6660(&damage, unit_index);
							damage.unknown84 = 3;
							damage.scale = 1.0f;
							object_get_damage_owner(unit_index, &damage.owner);
							damage.definition_index = local_98b918->unit.boarding_melee.obstacle_smash_damage.tag_index;
							damage.unknown14 = unit_index;
							damage.position = unit_center;
							damage.origin = unit_center;
							damage.direction = direction;
							function_30bf0(&damage.direction);
							damage.node_direction = damage.direction;
							function_d7b80(&damage, actions->point_target, NONE, NONE, NONE, NULL);
						}

						if (((1 << *(byte *)local_98b918) & 1) && root->havok_component_index != NONE)
						{
							s_object_definition *definition = (s_object_definition *)g_4e3b44[unit->definition_index & 0xffff].bytes;
							long effect_tag_index = *(long *)(g_4e3b44[definition->model_tag_index & 0xffff].bytes + 0x24);

							if (effect_tag_index != NONE)
							{
								c_type_47f957 material_index(g_54e898);
								s_havok_component *component = havok_component_get(root->havok_component_index);
								long rigid_body_index = function_0b67a0((s_small_index const *)component);

								if (rigid_body_index != NONE)
								{
									s_havok_component_rigid_body const *rigid_body = &component->rigid_bodies[rigid_body_index];

									if (rigid_body->node_count > 0)
									{
										long node_index = rigid_body->nodes[0];

										if (node_index != NONE)
										{
											vector3f impulse;
											byte strength;

											impulse.i = shove.i * -1.0f;
											impulse.j = shove.j * -1.0f;
											impulse.k = shove.k * -1.0f;
											function_1eb020(effect_tag_index, node_index, &material_index.m_index);
											strength = target_definition->impact_sound_size;
											if (strength > definition->impact_sound_size)
												strength = definition->impact_sound_size;
											function_188180(&unit_center, &impulse, definition->footstep_effects.tag_index, unit_index, 0xb, strength,
												material_index, (long)g_4687bc, (s_location const *)((byte *)unit + 0x28), 1.0f);
										}
									}
								}
							}
						}
					}
				}
			}
		}
		result = false;
	}
	return result;
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
		if (function_10f430(unit_index, 0x6000542, 0x7000101, 0x7000101, 0x400000c, 0.2f, 0, 0))
		{
			function_e5750(unit_index);
			result = true;
		}
	}
	else
	{
		point3f point;
		vector3f facing;

		function_b9dd0(unit_index, &point);
		unit_action_seat_facing_get(UNIT_ACTION_UNIT_GET(unit_index), &facing);
		result = function_ee090(unit_index, 0x6000542, 3, 0x7000543, &point, &facing);
	}
	return result;
}

void function_1e54d0(void *state, long a);

/* Resets the unit action state, switches it to mode 4, and updates the supplied point. */
// @retail 0xe5690
void __stdcall function_e5690(long unit_index, point3f const *point)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);

	*(short *)((byte *)unit + 0x34c) = NONE;
	*((byte *)unit + 0x34e) = 0;
	*((byte *)unit + 0x34f) = 0;

	function_1e54d0((byte *)unit + 0x3dc, 4);
	function_b75a0(unit_index, point, NULL, NULL, NULL, false);
}

/* The same as function_e5690, but switches to mode 5. */
// @retail 0xe56f0
void __stdcall function_e56f0(long unit_index, point3f const *point)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);

	*(short *)((byte *)unit + 0x34c) = NONE;
	*((byte *)unit + 0x34e) = 0;
	*((byte *)unit + 0x34f) = 0;

	function_1e54d0((byte *)unit + 0x3dc, 5);
	function_b75a0(unit_index, point, NULL, NULL, NULL, false);
}

/* Switches the unit to physics mode 2, then resets the state function_e5690
   resets. */
// @retail 0xe5750
void function_e5750(long unit_index)
{
	byte *unit = (byte *)UNIT_ACTION_UNIT_GET(unit_index);

	function_1e54d0(unit + 0x3dc, 2);
	*(short *)(unit + 0x34c) = NONE;
	unit[0x34e] = 0;
	unit[0x34f] = 0;
}

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

	if (UNIT_ACTION_HEADER_GET(unit_index)->type == 0)
	{
		long state_name;
		long action_name;

		function_10f630(unit_index, &action_name, &state_name);
		if (!result || state_name != 0x400069d)
		{
			if (function_10f430(unit_index, 0x6000086, 0x7000101, 0x7000101, 0x400000c, 8.0f, 0, 0))
			{
				s_unit_actions *actions = UNIT_ACTIONS_GET(UNIT_ACTION_UNIT_GET(unit_index));

				memset(actions->active, 0, sizeof(actions->active));
				actions->unknown36 = 0;
				function_1e54d0((byte *)UNIT_ACTION_UNIT_GET(unit_index) + 0x3dc, 1);
				result = true;
			}
			else
			{
				result = false;
			}
		}
	}
	return result;
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
	bool result = false;
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
		goto done;
	}

	long state = *((byte *)UNIT_ACTION_UNIT_GET(unit_index) + 0x3dc);
	long action_name = state == 5 || state == 4 ? 0x400000c : 0x5000049;

	result = function_ee090(unit_index, state_name, 5, action_name, (point3f const *)((byte *)request + 8),
		(vector3f const *)((byte *)request + 0x14));
done:
	return result;
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
		result = true;
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
	real launch_speed;
	real speed;
	vector3f velocity;

	function_b9dd0(unit_index, &center);
	function_fa1a0(1.0f, 1.0f, &center, (point3f const *)&actions->unknown20, &launch_speed, NULL, NULL,
		false, &velocity, &speed, NULL, NULL, NULL, NULL);
	launch_speed += 0.01f;
	if (function_fa1a0(launch_speed, 1.0f, &center, (point3f const *)&actions->unknown20,
		NULL, NULL, NULL, false, &velocity, &speed, NULL, NULL, NULL, NULL))
	{
		vector3f_scale(&velocity, speed, &velocity);
		function_b7740(unit_index, &velocity, NULL, false);
		function_1c4b00(unit_index, &velocity, NULL, 1);
		if (length_sq3f(&velocity) > 0.0001f)
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

	if (!function_110ab0(unit_index) &&
		(UNIT_ACTION_UNIT_GET(unit_index)->object_type != 0 || !function_e4050(unit_index)) &&
		function_10f430(unit_index, 0x7000101, 0x7000101, 0x7000101, 0xa00003e, 0.1f, 0, 0))
	{
		if (*((bool *)request + 4))
			function_edfa0(unit_index, (point2f const *)((byte *)request + 8));
		result = true;
	}
	return result;
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
PRIVATE __forceinline point3f *unit_action_transform_point(transform4x3f const *matrix, point3f const *point, point3f *out);
PRIVATE __forceinline point3f *unit_action_untransform_point(transform4x3f const *matrix, point3f const *point, point3f *out);

/* a frame from its forward and up axes and a position: the body of 0x1420f0,
   which retail inlines here */
PRIVATE __forceinline void unit_action_frame_build(transform4x3f *out, point3f const *position, vector3f const *forward, vector3f const *up)
{
	out->scale = 1.f;
	out->forward = *forward;
	cross3f(up, forward, &out->left);
	out->up = *up;
	out->position = *position;
}

/* a point's scaled components through a matrix's axes and position */
PRIVATE __forceinline point3f *unit_action_point_to_world(transform4x3f const *matrix, real x, real y, real z, point3f *out)
{
	out->x = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x + matrix->position.x;
	out->y = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x + matrix->position.y;
	out->z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z;
	return out;
}

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

	switch (*(short *)((byte *)request + 6))
	{
	case 0:
		result = function_b8d30(unit_index, 0xf0005b4, &marker, 1, false) > 0 &&
			function_10f430(unit_index, 0x6000086, 0x7000101, 0x7000101, 0xc0005b7, 0.1f, 0, 0);
		break;
	case 1:
		result = function_b8d30(unit_index, 0xe0005b5, &marker, 1, false) > 0 &&
			function_10f430(unit_index, 0x6000086, 0x7000101, 0x7000101, 0xb0005b8, 0.1f, 0, 0);
		break;
	}
	if (result)
	{
		vector3f forward;
		vector3f up;

		if (unit->parent_object_index == NONE)
		{
			vector3f left;

			vector3f_scale(&marker.matrix.up, -1.0f, &forward);
			up = object_matrix.up;
			cross3f(&up, &forward, &left);
			if (0.0f >= function_30bf0(&left))
			{
				up = *g_4687b0;
				left = *g_4687ac;
			}
			cross3f(&left, &up, &forward);
			function_b75a0(unit_index, NULL, &forward, &up, NULL, false);
		}

		byte *seat_transform = (byte *)unit + *(short *)((byte *)unit + 0x10e);
		transform4x3f frame;
		point3f world;
		point3f local;
		quaternionf object_orientation;
		quaternionf inverse_rotation;
		quaternionf orientation;

		unit_action_frame_build(&frame, &object_matrix.position, &forward, &up);
		real scale = object_matrix.scale;
		real x = ((point3f *)(seat_transform + 0x10))->x;
		real z = ((point3f *)(seat_transform + 0x10))->z;
		real y = ((point3f *)(seat_transform + 0x10))->y;

		if (scale != 1.0f)
		{
			x *= scale;
			y *= scale;
			z *= scale;
		}
		unit_action_point_to_world(&object_matrix, x, y, z, &world);
		unit_action_untransform_point(&frame, &world, &local);
		function_141f60(&object_matrix.rotation, &object_orientation);
		function_141f60(&frame.rotation, &inverse_rotation);
		inverse_rotation.w *= -1.0f;
		function_11d820(&object_orientation, (quaternionf const *)seat_transform, &orientation);
		function_11d820(&inverse_rotation, &orientation, &orientation);
		*(point3f *)(seat_transform + 0x10) = local;
		*(quaternionf *)seat_transform = orientation;

		actions->hoist_ticks = *(short *)((byte *)request + 4);
		if (actions->hoist_ticks <= 0)
			actions->hoist_ticks = g_510c54->field_2_3 << 1;
		actions->active[1] |= 0x20000;
	}
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

	function_10f630(unit_index, &action_name, &state_name);
	if ((state_name == 0xc0005b7 || state_name == 0xb0005b8) && actions->hoist_ticks > 0)
	{
		if (--actions->hoist_ticks > 0)
			result = true;
		else
			function_10f430(unit_index, 0x6000086, 0x7000101, 0x7000101, 0xf00003a, 0.1f, 0, 0);
	}
	return result;
}

/* plays a unit's surprise animation, and when it ends */
// @retail 0xed600
bool function_ed600(long unit_index, s_unit_actions *actions)
{
	bool result = false;
	c_animation_channel *channel = NULL;

	if (function_113e90(unit_index, 0x800061e, 0.0f, &channel, 0))
	{
		real event_time = channel->get_event_time();

		if (event_time >= 0.0f)
			actions->surprise_ticks = (short)unit_action_round_long((real)g_510c54->field_2_3 * event_time);
		else
			actions->surprise_ticks = g_510c54->field_2_3 * 3;
		result = true;
	}
	return result;
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
	function_10f630(unit_index, &action_name, &state_name);
	if (state_name == 0x700005d)
		result = function_e92e0(unit_index, 0x700005c);
	else
		result = function_ed600(unit_index, actions);
	if (result)
		actions->active[1] |= 0x40000;
	return result;
}

void function_201520(short value, long type, short a, bool b, bool c);

/* a surprise: waiting out a paused animation, then its time, then what
   follows it */
// @retail 0xed710
bool __stdcall function_ed710(long unit_index, long type)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	s_animation_state *state = (s_animation_state *)((byte *)unit + unit->animation_state_offset);
	long state_name;
	long action_name;
	bool result = true;

	function_10f630(unit_index, &action_name, &state_name);
	if (state_name != 0x700005c || (*((byte *)state + 0x6c) & 1) || !TEST_FIELD_BIT(state->channels[0].flag0) ||
		(state->channels[0].unknown11 & 1) || (state->channels[0].unknown11 & 8))
	{
		if (state_name != 0x800061e && actions->surprise_ticks <= 0)
		{
			result = function_ed600(unit_index, actions);
		}
		else if (actions->surprise_ticks > 0 && --actions->surprise_ticks <= 0)
		{
			if (!TEST_FIELD_BIT((unit->flags_10a >> 2) & 1))
				function_201520(1, actions->surprise_value, actions->surprise_type, 1, 0);
			actions->surprise_value = NONE;
			actions->surprise_type = NONE;
			result = false;
		}
	}
	return result;
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
			real fallback_time = state->channels[0].get_duration() * 0.33333334f;
			actions->flinch_ticks = (short)unit_action_round_long((real)g_510c54->field_2_3 * fallback_time);
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
	s_record_pool *objects = g_4e0300;
	bool result = true;

	if (--actions->flinch_ticks <= 0)
	{
		long weapon_index = unit_action_hand_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), 0);

		if (weapon_index != NONE &&
			!(g_4e3b44[((s_unit_action_header *)objects->data)[weapon_index & 0xffff].unit->definition_index & 0xffff].bytes[0x12f] & 1))
		{
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
		}
		result = false;
	}
	return result;
}

/* a unit's taunt (type 52) */
// @retail 0xeda30
bool __stdcall function_eda30(long unit_index, s_unit_request *request)
{
	bool result = false;

	if (function_10f430(unit_index, 0x6000086, 0x7000101, 0x7000101, 0xc0006b3, 0.2f, 0, 0))
		result = true;
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
	s_record_pool *objects = g_4e0300;
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	s_unit_actions *actions = UNIT_ACTIONS_GET(unit);
	long weapon_index = unit_action_hand_weapon_get(unit, 0);

	if (weapon_index != NONE)
	{
		byte *s_type_67e06b = g_4e3b44[((s_unit_action_header *)objects->data)[weapon_index & 0xffff].unit->definition_index & 0xffff].bytes;

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

				collision.unknown24 = NONE;
				position = marker.matrix.position;
				forward = marker.matrix.forward;
				up = marker.matrix.up;
				start.x = g_4687b0->i * 0.2f + position.x;
				start.y = g_4687b0->j * 0.2f + position.y;
				start.z = g_4687b0->k * 0.2f + position.z;
				probe.i = g_4687b0->i * -1.0f;
				probe.j = g_4687b0->j * -1.0f;
				probe.k = g_4687b0->k * -1.0f;
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
				function_b9dd0(weapon_index, &position);
				function_b9dd0(unit_index, &center);
				function_b9fc0(unit_index, &forward, &up);
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

	if (function_10f430(unit_index, 0x6000086, 0x7000101, 0x7000101, 0x60006ac, 0.2f, 0, 0))
	{
		actions->active[1] |= 0x200000;
		actions->weapon_planted = false;
		if (*((bool *)request + 4))
		{
			point2f facing = *(point2f *)((byte *)request + 8);

			if (normalize2d(&facing) > 0.0f)
				function_edfa0(unit_index, &facing);
		}
		result = true;
	}
	return result;
}

/* planting: the weapon planted at the animation's plant frame */
// @retail 0xedf10
bool __stdcall function_edf10(long unit_index, long type)
{
	bool done = false;
	long state_name;
	long action_name;

	function_10f630(unit_index, &action_name, &state_name);
	if (state_name != 0x60006ac)
	{
		done = true;
	}
	else
	{
		switch (function_10f720(unit_index, true))
		{
		case 1:
			function_eda70(unit_index);
			done = true;
			break;
		}
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
		vector3f_assign(&unit->forward, facing->x, facing->y, 0.0f);
		unit->up = *g_4687b0;
	}
}

/* v scaled by s into result (v and result may be the same point): the planar
   counterpart of vector3f_scale */
PRIVATE inline point2f *planar_scale(point2f const *v, real s, point2f *result)
{
	result->x = s * v->x;
	result->y = s * v->y;
	return result;
}

/* scales v to unit length and returns its old length, or 0 for a vector too
   short to normalize: the planar counterpart of normalize3d */
PRIVATE inline real planar_normalize(point2f *v)
{
	real m = (real)sqrt(v->x * v->x + v->y * v->y);
	if (!(fabs(m) < 0.0001f))
		planar_scale(v, 1.f / m, v);
	else
		m = 0.f;
	return m;
}

/* faces a unit along its aiming direction */
// @retail 0xedff0
void function_edff0(long unit_index)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	point2f facing = *(point2f const *)&unit->field_xe66477;

	if (planar_normalize(&facing) > 0.0f)
		function_edfa0(unit_index, &facing);
}

void function_b8840(long unit_index);
void __stdcall function_b77d0(long object_index, vector3f const *linear_velocity, vector3f const *angular_velocity);
void __stdcall function_e5690(long unit_index, point3f const *point);
void function_1420f0(transform4x3f *out, point3f const *position, vector3f const *forward,
	vector3f const *up);

/* a point carried from a matrix's frame into world space, and back into a
   matrix's frame (0 for a matrix with no scale): the bodies of 0x142570 and
   0x142700, which retail inlines here (unknown_141590.cpp is /Ob1, which
   keeps its own out of line) */
PRIVATE __forceinline point3f *unit_action_transform_point(transform4x3f const *matrix, point3f const *point, point3f *out)
{
	real x = point->x;
	real y = point->y;
	real z = point->z;

	if (matrix->scale != 1.0f)
	{
		x = matrix->scale * x;
		y = matrix->scale * y;
		z = matrix->scale * z;
	}
	out->x = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x + matrix->position.x;
	out->y = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x + matrix->position.y;
	out->z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z;
	return out;
}

PRIVATE __forceinline point3f *unit_action_untransform_point(transform4x3f const *matrix, point3f const *point, point3f *out)
{
	if (matrix->scale != 0.0f)
	{
		real x = point->x - matrix->position.x;
		real y = point->y - matrix->position.y;
		real z = point->z - matrix->position.z;

		if (matrix->scale != 1.0f)
		{
			real inverse = 1.0f / matrix->scale;

			x = inverse * x;
			y = inverse * y;
			z = inverse * z;
		}
		out->x = matrix->forward.k * z + matrix->forward.j * y + matrix->forward.i * x;
		out->y = matrix->left.k * z + matrix->left.j * y + matrix->left.i * x;
		out->z = matrix->up.k * z + matrix->up.j * y + matrix->up.i * x;
	}
	else
	{
		out->x = 0.0f;
		out->y = 0.0f;
		out->z = 0.0f;
	}
	return out;
}

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
	}
	if (result)
	{
		function_b9a50(unit_index);
		if (unit->object_type != 0)
		{
			if (TEST_FIELD_BIT((unit->flags_c0 >> 6) & 1))
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
			point3f local;
			quaternionf object_orientation;
			quaternionf target_orientation;
			quaternionf orientation;

			flat = *facing;
			flat.k = 0.0f;
			function_ba160(unit_index, &object_matrix);
			function_e5690(unit_index, point);
			if (function_30bf0(&flat) > 0.0f)
				function_b75a0(unit_index, NULL, &flat, g_4687b0, NULL, false);

			byte *seat_transform = (byte *)unit + *(short *)((byte *)unit + 0x10e);
			point3f world;

			function_1420f0(&target, point, facing, g_4687b0);
			unit_action_transform_point(&object_matrix, (point3f *)(seat_transform + 0x10), &world);
			unit_action_untransform_point(&target, &world, &local);
			function_141f60(&object_matrix.rotation, &object_orientation);
			function_141f60(&target.rotation, &target_orientation);
			target_orientation.w *= -1.0f;
			function_11d820(&object_orientation, (quaternionf const *)seat_transform, &orientation);
			function_11d820(&target_orientation, &orientation, &orientation);
			*(point3f *)(seat_transform + 0x10) = local;
			*(quaternionf *)seat_transform = orientation;
		}
		actions->unknown36 = (short)mode;
	}
	return result;
}

/* puts a unit back on its animation graph's root: placed where its root
   node is, facing the root's horizontal forward, its state animations, and
   the graph's default seat transform */
// @retail 0xee460
bool __stdcall function_ee460(long unit_index, long state_name, long action_name)
{
	s_unit_action_unit *unit = UNIT_ACTION_UNIT_GET(unit_index);
	byte *definition = g_4e3b44[UNIT_ACTION_UNIT_GET(unit_index)->definition_index & 0xffff].bytes;
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
	forward = matrix.forward;
	forward.n[2] = 0.0f;
	if (normalize3d(&forward) == 0.0f)
		forward = *g_4687a8;
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
	if (local_player_exists(user_index))
	{
		long player_index = g_4e8c20->entries[user_index];

		long unit_index = *(long *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x2c);

		if (unit_index != NONE)
		{
			switch (state)
			{
			case 14:
			{
				long request_type;
				if (secondary)
					request_type = 0x39;
				else
					request_type = 0x36;
				function_e68c0(request_type, unit_index);
				break;
			}
			case 15:
			{
				long request_type;
				if (secondary)
					request_type = 0x3b;
				else
					request_type = 0x38;
				function_e68c0(request_type, unit_index);
				break;
			}
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
			}
		}
	}
	function_168896(user_index, weapon_index, secondary, state);
}

/* sets the first person state of a unit's weapon (its player's view) */
// @retail 0xee7f0
void function_ee7f0(long unit_index, long state)
{
	long user_index;

	if (unit_get_player_index(unit_index) == NONE)
		user_index = NONE;
	else
		user_index = unit_action_player_value28_get(unit_get_player_index(unit_index));

	long secondary;

	switch (state)
	{
	case 0x13:
	case 0x15:
	case 0x19:
		secondary = true;
		break;
	default:
		secondary = false;
		break;
	}

	function_168896(user_index, unit_action_hand_weapon_get(UNIT_ACTION_UNIT_GET(unit_index), secondary),
		secondary, state);
}


void __stdcall function_104080(long object_index);
long function_1039a0(long object_index, long tag_index, long effect_index, real scale_a, real scale_b);
short first_person_weapon_animation_ticks(long weapon_index, long animation_name, short type);
bool function_1058b0(long weapon_index, long state, bool force);

// @retail 0xfff40
void __stdcall function_fff40(long weapon_index, bool silent, bool immediate)
{
    byte *object = *(byte **)(g_4e0300->data + (weapon_index & 0xffff) * 12 + 8);
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    function_104080(weapon_index);
    function_1058b0(weapon_index, 9, true);
    if (!silent) function_1039a0(weapon_index, *(long *)(definition + 0x144), NONE, 0.0f, 0.0f);
    if (immediate)
        *(short *)(object + 0x17c) = 0;
    else
    {
        *(word *)(object + 0x16c) &= ~0x20;
        *(short *)(object + 0x19e) = (short)unit_action_round_long((real)*(short *)((byte *)g_510c54 + 2) * *(real *)(definition + 0x138));
        *(short *)(object + 0x17c) = first_person_weapon_animation_ticks(weapon_index, 0x05000024, 1);
    }
}

