// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259a0.h"
#include "unknown_1e3920.h"
#include <string.h>

/* slot handler 0x81 (g_47eff8): the actor looks for a way around (a prop to
   go to, else one of eight directions around its facing) */

struct s_slot_81
{
	s_slot_header header;
	long flags;
	bool unknown10;
	byte unknown11[0x40 - 0x11];
};

/* the prop state as these callbacks read it: the position at +4 */
struct s_prop_state_point_81
{
	long unknown00;
	point3f position;
};

/* the structure bsp (g_4e0348) as function_1c19b0 reads it */
struct s_structure_bsp_pathfinding_view
{
	byte unknown00[0xc4];
	long pathfinding_count;
	s_pathfinding_data *pathfinding;
};

/* the priority this handler always reports */
short g_47ffa8 = 3;

real function_30bf0(vector3f *v);
extern vector3f *g_4687bc;
void __stdcall function_1c22c0(long object_index, long unused);

// @retail 0x1c19b0
bool __stdcall function_1c19b0(long actor_index, vector3f const *direction, s_type_f17a25 *path)
{
	s_actor_view *actor = actor_get(actor_index);
	s_structure_bsp_pathfinding_view *bsp = (s_structure_bsp_pathfinding_view *)g_4e0348;
	s_pathfinding_data *pathfinding = NULL;
	point3f origin;
	real height;
	real distance;
	point3f start;
	short i;
	s_collision_result_1697c0 collision;

	if (bsp->pathfinding_count > 0)
	{
		pathfinding = bsp->pathfinding;
	}
	collision.unknown24 = NONE;
	origin = actor->position;
	height = actor->position.z;
	distance = 0.0f;
	if (actor->unknown27c.unknown10 != NONE)
	{
		s_path_trace_result trace;

		function_26c590(pathfinding, &actor->position, actor->unknown27c.unknown10, NONE, direction, 5.0f, NULL, &trace);
		origin = trace.point;
		distance = trace.distance;
		if (function_1697c0(0x1808c2d, &origin, g_4687bc, actor->unknown018, NONE, &collision))
		{
			origin = collision.point;
		}
	}
	start.x = g_4687b0->i * 0.2f + origin.x;
	start.y = g_4687b0->j * 0.2f + origin.y;
	start.z = g_4687b0->k * 0.2f + origin.z;
	for (i = 0; i < 4; i++)
	{
		real scale = (i + 1) * 0.25f;
		point3f end;
		vector3f delta;
		long node_index;
		long unknown;

		end.x = g_4687b0->i * 0.4f + (direction->i * scale + origin.x);
		end.y = g_4687b0->j * 0.4f + (direction->j * scale + origin.y);
		end.z = g_4687b0->k * 0.4f + (direction->k * scale + origin.z);
		delta.i = end.x - start.x;
		delta.j = end.y - start.y;
		delta.k = end.z - start.z;
		if (function_1697c0(0x1808c2d, &start, &delta, actor->unknown018, NONE, &collision))
		{
			break;
		}
		node_index = function_26d100(g_4687b0, &collision, &unknown, &end);
		if (!(height + 0.1f > collision.point.z) || !(2.0f > origin.z - collision.point.z))
		{
			break;
		}
		if (node_index != NONE && (!path || function_272700(path, node_index) == NONE))
		{
			s_pathfinding_node *node = &pathfinding->nodes[node_index];

			if ((node->flags & 1) && (!(node->flags & 2) || !function_1fa6b0(node, pathfinding)))
			{
				long ticks;
				long ticks_extra;
				real seconds = g_510c54->field_2_3;
				real seconds_extra;

				__asm
				{
					fld seconds
					fistp ticks
				}
				seconds_extra = g_510c54->field_2_3 * 1.5f;
				__asm
				{
					fld seconds_extra
					fistp ticks_extra
				}
				return function_1f34b0(actor_index, direction, &actor->position,
					(i + 1) * 0.25f + distance + 0.5f, (short)((real)ticks * distance + (real)ticks_extra));
			}
		}
		height = collision.point.z;
	}
	return false;
}

// @retail 0x1c1d50
bool __stdcall function_1c1d50(long actor_index, s_type_f17a25 *path)
{
	s_actor_view *actor = actor_get(actor_index);
	real closest = 3.4028235e38f;
	long closest_index = NONE;
	long prop_index;

	for (prop_index = actor->first_prop_index; prop_index != NONE; )
	{
		s_prop_node_view *node = prop_node_get(prop_index);
		long index = prop_index;

		prop_index = node->next_index;
		if (node->view_index != NONE && node->unknown27 >= 2 && closest > node->unknown28)
		{
			closest = node->unknown28;
			closest_index = index;
		}
	}
	if (closest_index != NONE)
	{
		s_prop_state_point_81 *prop = (s_prop_state_point_81 *)prop_node_state(prop_node_get(closest_index));
		vector3f direction;

		direction.i = prop->position.x - actor->position.x;
		direction.j = prop->position.y - actor->position.y;
		direction.k = 0.0f;
		if (function_30bf0(&direction) == 0.0f)
		{
			direction = actor->unknown290;
		}
		function_1c19b0(actor_index, &direction, path);
		return true;
	}
	return false;
}

// @retail 0x1c1e70
bool function_1c1e70(long actor_index, s_type_f17a25 *path)
{
	vector3f facing = actor_get(actor_index)->unknown290;
	vector3f perpendicular;
	vector3f directions[8];
	short i;

	if (facing.k != 0.0f)
	{
		facing.k = 0.0f;
		if (function_30bf0(&facing) == 0.0f)
		{
			facing = *g_4687a8;
		}
	}
	perpendicular.i = -facing.j;
	perpendicular.j = facing.i;
	perpendicular.k = facing.k;
	directions[0] = facing;
	directions[1].i = facing.i * 0.70710677f + perpendicular.i * 0.70710677f;
	directions[1].j = facing.j * 0.70710677f + perpendicular.j * 0.70710677f;
	directions[1].k = facing.k * 0.70710677f + perpendicular.k * 0.70710677f;
	directions[2].i = facing.i * 0.70710677f - perpendicular.i * 0.70710677f;
	directions[2].j = facing.j * 0.70710677f - perpendicular.j * 0.70710677f;
	directions[2].k = facing.k * 0.70710677f - perpendicular.k * 0.70710677f;
	directions[3] = perpendicular;
	directions[4].i = perpendicular.i * -1.0f;
	directions[4].j = perpendicular.j * -1.0f;
	directions[4].k = perpendicular.k * -1.0f;
	directions[5].i = perpendicular.i * 0.70710677f + facing.i * -0.70710677f;
	directions[5].j = perpendicular.j * 0.70710677f + facing.j * -0.70710677f;
	directions[5].k = perpendicular.k * 0.70710677f + facing.k * -0.70710677f;
	directions[6].i = facing.i * -0.70710677f - perpendicular.i * 0.70710677f;
	directions[6].j = facing.j * -0.70710677f - perpendicular.j * 0.70710677f;
	directions[6].k = facing.k * -0.70710677f - perpendicular.k * 0.70710677f;
	directions[7].i = facing.i * -1.0f;
	directions[7].j = facing.j * -1.0f;
	directions[7].k = facing.k * -1.0f;
	for (i = 0; i < 8; i++)
	{
		if (function_1c19b0(actor_index, &directions[i], path))
		{
			break;
		}
	}
	return true;
}

// @retail 0x1c20f0
short __stdcall function_1c20f0(long actor_index)
{
	return g_47ffa8;
}

// @retail 0x1c2100
short __stdcall function_1c2100(long actor_index, s_slot *slot, bool active)
{
	s_slot_81 *state = (s_slot_81 *)slot;
	short result = g_46fbe8;

	if (state->unknown10)
	{
		result = g_46fbe4;
	}
	return result;
}

// @retail 0x1c2120
bool __stdcall function_1c2120(long actor_index, s_slot *slot)
{
	s_slot_81 *state = (s_slot_81 *)slot;

	state->flags = 0;
	return true;
}

// @retail 0x1c2130
bool __stdcall function_1c2130(long actor_index, s_slot *slot)
{
	s_slot_81 *state = (s_slot_81 *)slot;
	s_actor_view *actor = actor_get(actor_index);

	if (!actor->unknown605 && !actor->unknown264 && actor->unknown040)
	{
		bool flag;
		byte *buffer;
		long unknown;
		s_prop_search search;

		actor->unknown5e8 = 1;
		buffer = ai_scratch_buffer_get();
		memset(&search, 0, sizeof(search));
		search.unknown19 = true;
		flag = false;
		search.type = 4;
		function_2605d0(actor_index, (s_2605d0_request *)&search, 0, (long)&unknown, buffer, &flag);
		if (actor->unknown5e8 == 0)
		{
			state->unknown10 = true;
		}
		ai_scratch_buffer_release(buffer);
		if (!state->unknown10)
		{
			s_path_query query;
			s_type_f17a25 *path;
			bool found = false;

			function_26c180(actor_index);
			function_1f9240(actor_index, &query.settings);
			function_1f90f0(actor_index, &query.source);
			query.source.unknown45 = true;
			query.source.unknown48 = 20.0f;
			buffer = ai_scratch_buffer_get();
			function_271300((s_type_f17a25 *)buffer, NULL, &query.settings, &query.source, 0);
			path = function_2715a0(buffer) ? (s_type_f17a25 *)buffer : NULL;
			if (!(state->flags & 1))
			{
				found = function_1c1d50(actor_index, path);
				state->flags |= 1;
			}
			if (!found && !(state->flags & 4))
			{
				found = function_1c1e70(actor_index, path);
				state->flags |= 4;
			}
			if (!found)
			{
				state->flags = 0;
			}
			ai_scratch_buffer_release(buffer);
		}
	}
	return true;
}

s_slot_handler_2 g_47eff8 =
{
	{
		0x81, 2, 0xfff, -2, 0,
		function_1c20f0, function_1c2100, function_1c2120, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1c2130, 0, (t_slot_proc)function_1c22c0
};
