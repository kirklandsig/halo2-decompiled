// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259a0.h"
#include <math.h>
#include "unknown_1f4460.h"

/* slot handler 0x7c (g_47ef08) */

struct s_slot_7c
{
	s_slot_header header;
	bool unknown0c;
	byte unknown0d[3];
	long unknown10;
	long unknown14;
	bool unknown18;
	byte unknown19[3];
	long prop_index;
	byte unknown20[0x40 - 0x20];
};

/* the collision result this caller reserves: retail's object is 0x5c bytes with
   its flag word at +0x24 (the shared declaration is 0x4c bytes, so the shared
   declaration cannot describe this caller's output) */
struct s_collision_result_1697c0_caller
{
	byte unknown00[8];
	point3f point;
	byte unknown14[0x24 - 0x14];
	short unknown24;
	byte unknown26[0x5c - 0x26];
};

/* the prop state's position (unknown_25d690.cpp) */
struct s_prop_state_position
{
	long unknown00;
	point2f position;
	byte unknown0c[0x3c - 0xc];
	long unknown3c;
};

/* the object header (g_4e0300) as these callbacks read it: the object type
   at +3 */
struct s_object_header_type_view
{
	byte unknown00[3];
	byte type;
	byte unknown04[4];
	byte *object;
};

/* (0, 0, -1), defined in unknown_11d180.cpp */
extern vector3f *g_4687bc;

real normalize2d(point2f *v);
void function_1f86a0(long index);
real function_1c9ee0(real fraction);
long function_baf80(long object_index);
point3f *function_b9dd0(long object_index, point3f *result);

PRIVATE inline byte object_header_type(long object_index)
{
	return ((s_object_header_type_view *)g_4e0300->data)[object_index & 0xffff].type;
}

PRIVATE inline real distance2d(point2f const *a, point2f const *b)
{
	real dx = a->x - b->x;
	real dy = a->y - b->y;

	return (real)sqrt(dx * dx + dy * dy);
}

PRIVATE inline real game_ticks_to_seconds(long ticks)
{
	return (real)ticks * g_510c54->rate;
}

PRIVATE inline long game_seconds_to_ticks_round(real seconds)
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

// @retail 0x1c0290
bool function_1c0290(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = false;

	if (actor->unknown018 != NONE)
	{
		s_object_marker marker;

		if (function_b8d30(actor->unknown018, 0x11000799, &marker, 1, false))
		{
			long object_index = actor->unknown018;

			if (function_b8d30(object_index, 0x1100079a, &marker, 1, false))
			{
				result = true;
			}
		}
	}
	return result;
}

PRIVATE __forceinline real function_1c0301(point2f const *arg_0, point2f const *arg_1)
{
	real local_0 = arg_0->x - arg_1->x;
	real local_1 = arg_0->y - arg_1->y;
	local_0 *= local_0;
	return (real)sqrt(local_0 + local_1 * local_1);
}

// @retail 0x1c0300
short __stdcall function_1c0300(long actor_index)
{
	long result = 0;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE && function_1c0290(actor_index))
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_prop_state_position *prop = (s_prop_state_position *)prop_node_state(node);
		s_actor_tag_entry_1e4f90 *entry;

		if ((prop->unknown3c != NONE || object_header_type(node->object_index) == 1) &&
			(entry = (s_actor_tag_entry_1e4f90 *)function_1e4f90(actor_index)) != NULL &&
			entry->unknown04 >= function_1c0301(&prop->position, (point2f *)&actor->position) &&
			(actor->times[3] == NONE || game_ticks_to_seconds(g_510c54->game_time - actor->times[3]) > entry->unknown18) &&
			function_1c9ee0(entry->unknown08) > function_259a0(&g_4e7408->unknown0))
		{
			result = 3;
		}
	}
	return result;
}

#pragma inline_depth(0)
PRIVATE __forceinline bool function_1c04f1(s_slot_7c const *arg_0, s_actor_tag_entry_1e4f90 const *arg_1)
{
	long local_0 = g_510c54->game_time;
	long local_1 = function_1469f0(*(real const *)&arg_1->unknown14);
	return local_0 - arg_0->unknown10 > local_1;
}

#pragma inline_depth(255)

// @retail 0x1c04f0
short __stdcall function_1c04f0(long actor_index, s_slot *slot, bool active)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_7c *state = (s_slot_7c *)slot;
	short result = g_46fbe4;

	if (actor->prop_index == state->prop_index && actor->prop_index != NONE && function_1c0290(actor_index))
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_prop_state_position *prop = (s_prop_state_position *)prop_node_state(node);

		long local_4 = actor->unknown018;
		if (function_110ab0(local_4))
		{
			result = g_46fbe8;
		}
		else if (prop->unknown3c != NONE || object_header_type(node->object_index) == 1)
		{
			s_actor_tag_entry_1e4f90 *entry = (s_actor_tag_entry_1e4f90 *)function_1e4f90(actor_index);
			s_slot_7c *local_3 = (s_slot_7c *)*(s_slot *const volatile *)&slot;
            bool local_0 = *(volatile bool const *)&local_3->unknown18;
			real distance = distance2d(&prop->position, (point2f *)&actor->position);

			result = g_46fbe8;
			if (local_0)
			{
				int local_1 = g_510c54->game_time;
                long local_2 = game_seconds_to_ticks_round(4.0f);
                if (local_1 - local_3->unknown14 > local_2)
				{
					result = g_46fbe4;
				}
			}
			else if (distance > entry->unknown10 ||
				function_1c04f1(local_3, entry))
			{
				result = g_46fbe4;
			}
		}
	}
	if (result == g_46fbe4)
	{
		g_46eeb8[0x7c]->unknown8 = g_46f348;
	}
	return result;
}

// @retail 0x1c0670
bool __stdcall function_1c0670(long actor_index, s_slot *slot)
{
	s_slot_7c *state = (s_slot_7c *)slot;
	s_actor_view *actor = actor_get(actor_index);
	s_prop_node_view *node = prop_node_get(actor->prop_index);
	s_actor_tag_entry_1e4f90 *entry = (s_actor_tag_entry_1e4f90 *)function_1e4f90(actor_index);
	point3f position;

	function_b9dd0(node->object_index, &position);
	state->unknown0c = false;
	if (2.0f > distance2d((point2f *)&position, (point2f *)&actor->position))
	{
		real height;

		state->unknown0c = true;
		height = actor->position.z - position.z;
		if (height >= 0.0f && entry->unknown0c > height)
		{
			if (!state->unknown18)
			{
				s_game_time_globals *game = g_510c54;
				state->unknown18 = true;
				state->unknown14 = game->game_time;
			}
			long unit_index = actor->unknown018;
			if (!function_110ab0(unit_index))
			{
				s_unit_request action;

				action.type = 0x1a;
				action.type1a.unknown4 = 0;
				action.type1a.unknown6 = false;
				function_e6900(actor->unknown018, &action);
			}
		}
	}
	if (actor->unknown040)
	{
		vector3f vector;
		s_collision_result_1697c0_caller collision;

		collision.unknown24 = NONE;
		vector.i = g_4687bc->i * 4.0f;
		vector.j = g_4687bc->j * 4.0f;
		vector.k = g_4687bc->k * 4.0f;
		position.x += g_4687b0->i;
		position.y += g_4687b0->j;
		position.z += g_4687b0->k;
		if (function_1697c0(0x1808c0d, &position, &vector, node->object_index, actor->unknown018,
			(s_collision_result_1697c0 *)&collision))
		{
			long object_index = function_baf80(node->object_index);
			s_actor_point_target target;

			point3f point;

			point.x = g_4687b0->i * 2.5f + collision.point.x;
			point.y = g_4687b0->j * 2.5f + collision.point.y;
			point.z = g_4687b0->k * 2.5f + collision.point.z;
			target.point = point;
			target.output_index = NONE;
			return function_1f4460(actor_index, &target, NONE, object_index, true);
		}
		return false;
	}
	return true;
}

// score 26 (was 26): the slot pointer and the actor index are loaded into the
// opposite registers here (index ECX, slot EAX; retail EAX/ECX, so retail scales the
// index before the actor-table load and its `and` is one byte shorter), and the reset
// stores are scheduled in another order (retail keeps the two NONE stores between the
// zero stores). 0x1b3ee0 has the same entry difference.
// Round 272: native slot-data block 26; with one clock 34, chained false stores 26,
// readonly actor 26, late actor 36, actor at use 36; existing reset with data/full slot 85/85.
// Round 70: array-index actor lookup 26; slot declared first 26; reset stores in retail's
// order 26 (identical code: the order is the scheduler's); existing reset function through a
// forceinline declaration 85 (stays a call: its file is /Ob1); prop copy first 37.
// Round 89: the reset's actor address offset-first, as an array element or through actor_get 26 each;
// bool result single exit 26; const state/actor 26; chained time stores 34. Matched 0x1b4490 has the
// same head without the reset and gets retail's eax/ecx: the offset's life into the reset decides it.
// BLOCKED-BY 0x1f86a0 (retail inlines it at link time; its /Ob1 file keeps ours out of line).
// @retail 0x1c0430
bool __stdcall function_1c0430(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_7c *state = (s_slot_7c *)slot;

	state->unknown0c = false;
	state->unknown18 = false;
	state->unknown10 = g_510c54->game_time;
	state->unknown14 = g_510c54->game_time;
	state->prop_index = actor->prop_index;
	actor_reset_state(actor_index);
	return true;
}

// @retail 0x1c04c0
void __stdcall function_1c04c0(long actor_index, s_slot *slot)
{
	actor_get(actor_index)->times[3] = g_510c54->game_time;
}

PRIVATE __forceinline bool function_1c0a31(point2f *arg_0)
{
	return normalize2d(arg_0) > 0.0f;
}

PRIVATE __forceinline void vector2d_from_points2d_1c0a30(point2f const *a, point2f const *b, point2f *out)
{
	out->x = b->x - a->x;
	out->y = b->y - a->y;
}

// @retail 0x1c0a30
void __stdcall function_1c0a30(long actor_index, s_slot *slot)
{
	s_slot_7c *state = (s_slot_7c *)slot;

	if (state->unknown0c)
	{
		s_actor_view *local_0 = actor_get(actor_index);
		local_0->flags810 = local_0->flags810 | 0x2000;
	}
	else
	{
		if (actor_get(actor_index)->unknown50c && actor_get(actor_index)->unknown504 == 1)
		{
			s_actor_view *actor = actor_get(actor_index);
			s_prop_state_position *prop = (s_prop_state_position *)prop_node_state(prop_node_get(actor->prop_index));
			point2f direction;
			point2f facing;

			vector2d_from_points2d_1c0a30((point2f const *)&actor->position, &prop->position, &direction);
			if (function_1c0a31(&direction))
			{
				facing = *(point2f *)&actor->unknown290;
				if (function_1c0a31(&facing) && direction.y * facing.y + direction.x * facing.x > 0.9f)
				{
					actor->unknown482 = true;
				}
			}
		}
	}
}

// @retail 0x1c0b60
void __stdcall function_1c0b60(long actor_index, s_slot *slot, long index)
{
	s_slot_7c *state = (s_slot_7c *)slot;

	if (state->prop_index == index)
	{
		state->prop_index = NONE;
	}
}

s_slot_handler_2 g_47ef08 =
{
	{
		0x7c, 2, NONE, -2, 0,
		function_1c0300, function_1c04f0, function_1c0430, function_1c04c0, NONE, {0},
		0, function_1c0b60, slot_release_nothing, 0, 0, 0, 0
	},
	(t_slot_proc)function_1c0670, 0, function_1c0a30
};
