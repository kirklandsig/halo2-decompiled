// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259a0.h"
#include <math.h>

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

// @retail 0x1c0300
short __stdcall function_1c0300(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (actor->prop_index != NONE && function_1c0290(actor_index))
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_prop_state_position *prop = (s_prop_state_position *)prop_node_state(node);
		s_actor_tag_entry_1e4f90 *entry;

		if ((prop->unknown3c != NONE || object_header_type(node->object_index) == 1) &&
			(entry = (s_actor_tag_entry_1e4f90 *)function_1e4f90(actor_index)) != NULL &&
			entry->unknown04 >= distance2d(&prop->position, (point2f *)&actor->position) &&
			(actor->times[3] == NONE || game_ticks_to_seconds(g_510c54->game_time - actor->times[3]) > entry->unknown18) &&
			function_1c9ee0(entry->unknown08) > function_259a0(&g_4e7408->unknown0))
		{
			return 3;
		}
		return 0;
	}
	return result;
}

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

		if (function_110ab0(actor->unknown018))
		{
			result = g_46fbe8;
		}
		else if (prop->unknown3c != NONE || object_header_type(node->object_index) == 1)
		{
			s_actor_tag_entry_1e4f90 *entry = (s_actor_tag_entry_1e4f90 *)function_1e4f90(actor_index);
			real distance = distance2d(&prop->position, (point2f *)&actor->position);

			result = g_46fbe8;
			if (state->unknown18)
			{
				if (g_510c54->game_time - state->unknown14 > game_seconds_to_ticks_round(4.0f))
				{
					result = g_46fbe4;
				}
			}
			else if (distance > entry->unknown10 ||
				g_510c54->game_time - state->unknown10 > function_1469f0(entry->unknown14))
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
				state->unknown18 = true;
				state->unknown14 = g_510c54->game_time;
			}
			if (!function_110ab0(actor->unknown018))
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
		s_collision_result_1697c0 collision;

		vector.i = g_4687bc->i * 4.0f;
		vector.j = g_4687bc->j * 4.0f;
		vector.k = g_4687bc->k * 4.0f;
		position.x += g_4687b0->i;
		position.y += g_4687b0->j;
		position.z += g_4687b0->k;
		collision.unknown24 = NONE;
		if (function_1697c0(0x1808c0d, &position, &vector, node->object_index, actor->unknown018, &collision))
		{
			long object_index = function_baf80(node->object_index);
			s_actor_point_target target;
			bool result = true;

			target.point.x = g_4687b0->i * 2.5f + collision.point.x;
			target.point.y = g_4687b0->j * 2.5f + collision.point.y;
			target.point.z = g_4687b0->k * 2.5f + collision.point.z;
			target.output_index = NONE;
			actor = actor_get(actor_index);
			if (!actor->unknown229)
			{
				function_1f86a0(actor_index);
				result = false;
			}
			else
			{
				function_2628f0(actor_index, g_470fa0);
				if (actor->unknown4ac != 2 || actor->unknown4c8 != NONE ||
					function_210a30(&target, &actor->unknown4b8) > 0.01f)
				{
					function_1f86a0(actor_index);
					actor->unknown4ac = 0;
					actor->unknown4ae = false;
					actor->unknown4b0 = 0.0f;
					actor->unknown4b4 = 0.0f;
					actor->unknown4cc = 0.0f;
					actor->unknown4d0 = 0.0f;
					actor->unknown4d4 = false;
					actor->unknown4d5 = false;
					actor->unknown4e8 = false;
					actor->unknown4e4 = NONE;
					actor->unknown4b8 = target;
					actor->unknown4c8 = NONE;
					actor->unknown4ac = 2;
					actor->unknown4ae = true;
					actor->unknown4e4 = object_index;
					if (actor->unknown229)
					{
						actor->unknown656 = 0;
					}
					result = function_1f8a70(actor_index, false);
				}
				else if (actor->unknown040 && !actor->unknown506)
				{
					result = function_1f8a70(actor_index, false);
				}
			}
			return result;
		}
		return false;
	}
	return true;
}

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

// @retail 0x1c0a30
void __stdcall function_1c0a30(long actor_index, s_slot *slot)
{
	s_slot_7c *state = (s_slot_7c *)slot;

	if (state->unknown0c)
	{
		actor_get(actor_index)->unknown810.bit13 = true;
	}
	else
	{
		s_actor_view *actor = actor_get(actor_index);

		if (actor->unknown50c && actor->unknown504 == 1)
		{
			s_prop_state_position *prop = (s_prop_state_position *)prop_node_state(prop_node_get(actor->prop_index));
			point2f direction;
			point2f facing;

			direction.x = prop->position.x - actor->position.x;
			direction.y = prop->position.y - actor->position.y;
			if (normalize2d(&direction) > g_45dbd8)
			{
				facing = *(point2f *)&actor->unknown290;
				if (normalize2d(&facing) > g_45dbd8 && facing.x * direction.x + facing.y * direction.y > 0.9f)
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
