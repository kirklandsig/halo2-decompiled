// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259a0.h"
#include "unknown_1e3920.h"
#include "unknown_26b230.h"
#include "vector_products.h"
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

struct s_slot_81_data
{
	long flags;
	bool unknown04;
	byte unknown05[0x34 - 5];
};

typedef char assert_slot_81_data_size[(sizeof(s_slot_81_data) == 0x34) ? 1 : -1];

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

/* function_1697c0 fills 0x5c bytes; the shared result type covers the first
   0x4c (as path_transitions.cpp's transition buffer) */
struct s_collision_result_buffer_1697c0
{
	s_collision_result_1697c0 result;
	byte unknown4c[0x5c - 0x4c];
};

/* the priority this handler always reports */
short g_47ffa8 = 3;

real function_30bf0(vector3f *v);
extern vector3f *g_4687bc;
void __stdcall actor_behavior_81_update48(long object_index, long unused);

static __forceinline long real_to_long(real value);

/* the point t along vector from point */
static inline void point_along_vector_1c19b0(point3f const *point, vector3f const *vector, real t, point3f *result)
{
	result->x = vector->i * t + point->x;
	result->y = vector->j * t + point->y;
	result->z = vector->k * t + point->z;
}

// score 23 (was 23): frame 0xd4 and registers agree; the end point sits at 0x40 below the second
// rounded tick count (0x4c) and the actor-position temp (0x50), where retail has 0x48 above 0x40/0x44,
// and its stores are scheduled before the call pushes. Round 152: full 0x5c collision buffer 69;
// point-along-vector steps in place 163, through an `ahead` point 54; single exit 23; end/ahead
// declarations swapped 23; tick locals at loop scope 23; end or delta at function scope 23/30.
// Final call after the loop behind a found flag 337; function-scope ticks,
// endpoint lexical names and location/node declarations 23.
// A compile-only frame probe puts both 4-byte slots below end only with the final call after the
// loop (a found flag); that real form changes the convention and scores 337.
// Earlier: frame 0xc4 versus 0xd4; direction/counter roles and endpoint scheduling differ.
// The collision workspace remains 16 bytes smaller; local offsets and loop layout differ.
// Reused step scale 228; with shared delta 228, endpoint setter 220, both 220;
// with start setter 228, start/delta 228, both setters 227, all three 227.
// Reused actor-position pointer, first-use lifetime, origin/start declarations
// and common-exit combinations 114, 82, 114, 82, 190, 154, 190, 154.
// Round 219: Complete per-step direction snapshot with short/char/byte/long induction
// 240/240/240/226; direct char/byte/long/word induction 210/210/164/81. All restored; frame
// 0xc4/0xd4 and direction/counter roles remain.
// Round 266: existing point producer point/vector/both inputs by value 310/310/453;
// output-pointer return 23; with point/vector/both values 310/310/453;
// complete point-value result 316. Original helper and every producer restored.
// Round 31: force-inlined point helper, delta/end and function-scope tick declarations,
// sample_end/contact_end names, location/node and start/origin declaration orders 23 each.
// Round 393: origin input/trace/collision library copies in seven subsets 23 each;
// native initial origin with both output copies 215. Original producers and complete body restored.
// Round 90: end named below/ground/target 23/26/23; both conversions passed straight to the call 26;
// a position pointer for the final call 160; the step scale written in the call 23; tick names 23.
// Round 40: helper storing a local point once 316; end from a by-value point producer 226;
// end/delta/start/origin renamed 23 each (frame unchanged); ticks and call as an inline/forceinline
// helper 267/335.
// Round 91: a position pointer for origin/trace/final, origin/final, trace/final and origin/trace
// 23/23/131/23; its early declaration 52; height/origin/final and height/trace/final 24/24;
// reversed rounded-count productions 30; all restored.
// @retail 0x1c19b0
bool __stdcall function_1c19b0(long actor_index, vector3f const *direction, s_type_f17a25 *path)
{
	vector3f const *const *direction_slot = &direction;
	s_actor_view *actor = actor_get(actor_index);
	s_structure_bsp_pathfinding_view *bsp = (s_structure_bsp_pathfinding_view *)g_4e0348;
	s_pathfinding_data *pathfinding = NULL;
	point3f origin;
	real height;
	real distance;
	point3f start;
	short i;
	s_collision_result_buffer_1697c0 collision;
	bool result = false;

	if (bsp->pathfinding_count > 0)
	{
		pathfinding = bsp->pathfinding;
	}
	height = actor->position.z;
	collision.result.unknown24 = NONE;
	distance = 0.0f;
	origin = actor->position;
	if (actor->unknown27c.unknown10 != NONE)
	{
		s_path_trace_result trace;

		function_26c590(pathfinding, &actor->position, actor->unknown27c.unknown10, NONE, *direction_slot, 5.0f, NULL, &trace);
		origin = trace.point;
		distance = trace.distance;
		if (function_1697c0(0x1808c2d, &origin, g_4687bc, actor->unknown018, NONE, &collision.result))
		{
			origin = collision.result.point;
		}
	}
	point_along_vector_1c19b0(&origin, g_4687b0, 0.2f, &start);
	for (i = 0; i < 4; i++)
	{
		real scale = (i + 1) * 0.25f;
		point3f ahead;
		point3f end;
		vector3f delta;
		long node_index;
		s_type_c3b527 location;

		point_along_vector_1c19b0(&origin, direction, scale, &ahead);
		point_along_vector_1c19b0(&ahead, g_4687b0, 0.4f, &end);
		vector3d_from_points3d(&start, &end, &delta);
		if (function_1697c0(0x1808c2d, &start, &delta, actor->unknown018, NONE, &collision.result))
		{
			break;
		}
		node_index = function_26d100(g_4687b0, (long *)&location, &collision.result, &end);
		if (!(height + 0.1f > collision.result.point.z) || !(2.0f > origin.z - collision.result.point.z))
		{
			break;
		}
		if (node_index != NONE && (!path || function_272700(path, node_index) == NONE))
		{
			s_pathfinding_node *node = &pathfinding->nodes[node_index];

			if ((node->flags & 1) && (!(node->flags & 2) || !function_1fa6b0(node, pathfinding)))
			{
				long ticks = real_to_long(g_510c54->field_2_3);
				long ticks_extra = real_to_long(g_510c54->field_2_3 * 1.5f);

				result = function_1f34b0(actor_index, &actor->position, direction,
					(i + 1) * 0.25f + distance + 0.5f, (short)((real)ticks * distance + (real)ticks_extra));
				break;
			}
		}
		height = collision.result.point.z;
	}
	return result;
}

struct s_node_view;
void function_26be00(long actor_index, s_iterator *iterator);
s_node_view *function_26be30(s_iterator *iterator);

// @retail 0x1c1d50
bool __stdcall function_1c1d50(long actor_index, s_type_f17a25 *path)
{
	bool result = false;
	long closest_index = NONE;
	s_actor_view *actor = actor_get(actor_index);
	real closest = 3.4028235e38f;
	s_iterator iterator;
	s_prop_node_view *node;

	function_26be00(actor_index, &iterator);
	while ((node = (s_prop_node_view *)function_26be30(&iterator)) != NULL)
	{
		if (node->view_index != NONE && node->unknown27 >= 2 && closest > node->unknown28)
		{
			closest = node->unknown28;
			closest_index = iterator.index;
		}
	}
	if (closest_index != NONE)
	{
		s_prop_state_point_81 *prop = (s_prop_state_point_81 *)prop_node_state(prop_node_get(closest_index));
		vector3f direction;

		vector2d_from_points2d((point2f const *)&actor->position, (point2f const *)&prop->position, (point2f *)&direction);
		direction.k = 0.0f;
		if (function_30bf0(&direction) == 0.0f)
		{
			direction = actor->unknown290;
		}
		function_1c19b0(actor_index, &direction, path);
		result = true;
	}
	return result;
}

void scripted_vector_scale(vector3f const *vector, real scale, vector3f *result);

// @retail 0x1c1e70
bool function_1c1e70(long actor_index, s_type_f17a25 *path)
{
	s_actor_view *actor = actor_get(actor_index);
	vector3f facing = actor->unknown290;
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
	scripted_vector_scale(&facing, 0.70710677f, &directions[1]);
	vector3f_add_scaled(&directions[1], &perpendicular, 0.70710677f, &directions[1]);
	scripted_vector_scale(&facing, 0.70710677f, &directions[2]);
	directions[2].i = directions[2].i - perpendicular.i * 0.70710677f;
	directions[2].j = directions[2].j - perpendicular.j * 0.70710677f;
	directions[2].k = directions[2].k - perpendicular.k * 0.70710677f;
	directions[3] = perpendicular;
	directions[4].i = perpendicular.i * -1.0f;
	directions[4].j = perpendicular.j * -1.0f;
	directions[4].k = perpendicular.k * -1.0f;
	scripted_vector_scale(&facing, -0.70710677f, &directions[5]);
	vector3f_add_scaled(&directions[5], &perpendicular, 0.70710677f, &directions[5]);
	scripted_vector_scale(&facing, -0.70710677f, &directions[6]);
	directions[6].i = directions[6].i - perpendicular.i * 0.70710677f;
	directions[6].j = directions[6].j - perpendicular.j * 0.70710677f;
	directions[6].k = directions[6].k - perpendicular.k * 0.70710677f;
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

// The post-header slot state and output-before-clear initialization match retail.
// score 0 (was 64): the object pointer is kept in ESI here and reloaded from
// the stack
// there, and the flag byte is written from AL here and as a constant 0 in
// retail.
// @retail 0x1c2130
bool __stdcall function_1c2130(long actor_index, s_slot *slot)
{
	s_slot_81_data *state = (s_slot_81_data *)slot->unknownc;
	s_actor_view *actor = actor_get(actor_index);

	if (!actor->unknown605 && !actor->unknown264 && actor->unknown040)
	{
		bool flag;
		byte *buffer;
		long unknown;
		s_2605d0_request request;

		actor->unknown5e8 = 1;
		buffer = ai_scratch_buffer_get();
		flag = false;
		memset(&request, 0, sizeof(request));
		request.unknown015 = true;
		request.type = 4;
		function_2605d0(actor_index, &request, 0, (long)&unknown, buffer, &flag);
		if (actor->unknown5e8 == 0)
		{
			state->unknown04 = true;
		}
		ai_scratch_buffer_release(buffer);
		if (!state->unknown04)
		{
			s_path_settings settings;
			s_path_source source;
			bool valid;
			bool found = false;

			function_26c180(actor_index);
			function_1f9240(actor_index, &settings);
			function_1f90f0(actor_index, &source);
			source.unknown45 = true;
			source.unknown48 = 20.0f;
			buffer = ai_scratch_buffer_get();
			function_271300((s_type_f17a25 *)buffer, NULL, &settings, &source, 0);
			valid = function_2715a0(buffer);
			if (!(state->flags & 1))
			{
				found = function_1c1d50(actor_index, valid ? (s_type_f17a25 *)buffer : NULL);
				state->flags |= 1;
			}
			if (!found && !(state->flags & 4))
			{
				found = function_1c1e70(actor_index, valid ? (s_type_f17a25 *)buffer : NULL);
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
	(t_slot_proc)function_1c2130, 0, (t_slot_proc)actor_behavior_81_update48
};
