// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259a0.h"
#include <math.h>

/* slot handler 0x82 (g_47ef58), and the firing position search it uses */

struct s_slot_82
{
	s_slot_header header;
	bool unknown0c;
	byte unknown0d[3];
	long unknown10;
	point3f unknown14;
	bool unknown20;
	bool unknown21;
	byte unknown22[2];
	long unknown24;
	long unknown28[4];
	short unknown38;
	byte unknown3a[0x40 - 0x3a];
};

/* the prop state as these callbacks read it: the position at +4 */
struct s_prop_state_point
{
	long unknown00;
	point3f position;
};

/* the object's tag as function_1c1160 reads it: the seats of the vehicle
   (0xb0 bytes each) */
struct s_vehicle_seat_view
{
	struct
	{
		dword unknown0 : 3;
		dword bit3 : 1;
		dword unknown4 : 28;
	} flags;
	byte unknown04[0xb0 - 0x4];
};

struct s_vehicle_tag_view
{
	byte unknown000[0x1c8];
	long seat_count;
	s_vehicle_seat_view *seats;
};

/* the farthest a firing position may be from the actor */
real g_46fbdc = 25.0f;

PRIVATE inline s_vehicle_tag_view *vehicle_tag_get(long object_index)
{
	return (s_vehicle_tag_view *)g_4e3b44[object_get(object_index)->tag_index & 0xffff].bytes;
}

/* a point target's position: its point, or where function_2104b0 puts the
   target of its type */
PRIVATE inline void actor_point_target_position(s_actor_point_target const *target, point3f *position)
{
	if (target->output_index == NONE || !function_2104b0(target->output_index, &target->point, position))
	{
		*position = target->point;
	}
}

// @retail 0x1c0b80
bool function_1c0b80(long actor_index, long const *excluded, short excluded_count,
	s_actor_point_target *target, long *unknown, long *reference)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = false;

	if (actor->prop_index != NONE)
	{
		s_scenario_firing_view *scenario = (s_scenario_firing_view *)g_4e0350;
		point3f *prop_position = &((s_prop_state_point *)prop_node_state(prop_node_get(actor->prop_index)))->position;

		if (scenario->zone_set_count > 0)
		{
			s_firing_zone_set *zone_set = scenario->zone_sets;
			real best_cost = 3.4028235e38f;
			s_path_query query;
			byte *buffer;
			short zone_index;

			function_1f90f0(actor_index, &query.source);
			query.source.unknown45 = true;
			query.source.unknown48 = g_46fbdc;
			function_1f9240(actor_index, &query.settings);
			buffer = ai_scratch_buffer_get();
			function_271300((s_type_f17a25 *)buffer, NULL, &query.settings, &query.source, 0);
			function_2715a0(buffer);
			for (zone_index = 0; zone_index < zone_set->zone_count; zone_index++)
			{
				s_firing_zone *zone = &zone_set->zones[zone_index];

				if ((zone->flags & 2) && zone->bsp_index == g_4686c4)
				{
					short position_index;

					for (position_index = 0; position_index < zone->position_count; position_index++)
					{
						long position_reference = (zone_index << 16) | (word)position_index;
						s_firing_position *position;
						point3f point;
						vector3f vector;
						real path_distance;
						short i;

						for (i = 0; i < excluded_count; i++)
						{
							if (excluded[i] == position_reference)
							{
								break;
							}
						}
						if (i < excluded_count)
						{
							continue;
						}
						position = &zone->positions[position_index];
						actor_point_target_position(&position->target, &point);
						vector3d_from_points3d(&point, &actor->position, &vector);
						if (!(g_46fbdc * g_46fbdc > length_sq3f(&vector)))
						{
							continue;
						}
						function_270750(buffer, position->unknown30, &position->target, &path_distance, 0, 0);
						if (3.4028235e38f > path_distance)
						{
							real distance;
							real cost;
							vector3f direction;

							actor_point_target_position(&position->target, &point);
							vector3d_from_points3d(&point, prop_position, &vector);
							distance = (real)sqrt(length_sq3f(&vector)) + path_distance;
							actor_point_target_position(&position->target, &point);
							vector3d_from_points3d(prop_position, &point, &direction);
							cost = 3.4028235e38f;
							if (normalize_inline(&direction) > 0.0f)
							{
								vector3f facing;
								real alignment;

								facing.i = (real)cos(position->facing);
								facing.j = (real)sin(position->facing);
								facing.k = 0.0f;
								alignment = -dot3f(&direction, &facing);
								if (alignment > 0.0f)
								{
									cost = (2.0f - alignment) * distance;
								}
							}
							if (best_cost > cost)
							{
								*target = position->target;
								*unknown = position->unknown30;
								best_cost = cost;
								*reference = position_reference;
								result = true;
							}
						}
					}
				}
			}
			ai_scratch_buffer_release(buffer);
		}
	}
	return result;
}

// @retail 0x1c1080
void function_1c1080(s_slot_82 *state, long reference)
{
	if (function_25ab50(reference))
	{
		state->unknown21 = true;
		state->unknown24 = reference;
	}
}

// @retail 0x1c10a0
short __stdcall function_1c10a0(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (actor->unknown22a && !actor->unknown223 && actor->prop_index != NONE)
	{
		result = 3;
	}
	return result;
}

// @retail 0x1c10f0
bool __stdcall function_1c10f0(long actor_index, s_slot *slot)
{
	s_slot_82 *state = (s_slot_82 *)slot;

	actor_reset_state(actor_index);
	state->unknown0c = false;
	state->unknown10 = NONE;
	state->unknown38 = 0;
	state->unknown20 = false;
	return true;
}

// @retail 0x1c1160
short __stdcall function_1c1160(long actor_index, s_slot *slot, bool active)
{
	s_slot_82 *state = (s_slot_82 *)slot;
	short result = g_46fbe8;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown504 == 2)
	{
		if (actor->unknown22a)
		{
			if (!function_110ab0(actor->unknown018) &&
				(!state->unknown20 || dot3f((vector3f *)&state->unknown14, &actor->unknown290) > 0.9f))
			{
				s_unit_request action;

				action.type = 0x35;
				action.type35.has_vector = state->unknown20;
				if (state->unknown20)
				{
					action.type35.vector = *(vector3f *)&state->unknown14;
				}
				state->unknown0c = function_e6900(actor->unknown018, &action);
			}
		}
		else if (!function_110ab0(actor->unknown018))
		{
			long vehicle_index = state->unknown10;

			result = g_46fbe4;
			if (vehicle_index != NONE)
			{
				s_vehicle_tag_view *tag = vehicle_tag_get(vehicle_index);
				short seat_index;

				for (seat_index = 0; seat_index < tag->seat_count; seat_index++)
				{
					if (TEST_FIELD_BIT(tag->seats[seat_index].flags.bit3))
					{
						if (seat_index != NONE && function_c8f60(vehicle_index, seat_index) == NONE)
						{
							s_unit_request action;

							action.type = 0x1c;
							action.type1c.object_index = vehicle_index;
							action.type1c.seat_index = seat_index;
							action.type1c.unknowna = false;
							action.type1c.unknownb = false;
							function_e6900(actor->unknown018, &action);
						}
						break;
					}
				}
			}
		}
	}
	return result;
}

// @retail 0x1c1320
bool __stdcall function_1c1320(long actor_index, s_slot *slot)
{
	s_slot_82 *state = (s_slot_82 *)slot;
	s_actor_view *actor = actor_get(actor_index);
	bool result = true;

	if (actor->unknown040 && !(actor->unknown50c && actor->unknown504 == 1) && actor->unknown504 != 2)
	{
		s_actor_point_target target;
		long unknown = NONE;
		long reference = NONE;

		if (state->unknown21)
		{
			s_firing_position *position;

			reference = state->unknown24;
			position = firing_position_get(reference);
			target = position->target;
			unknown = position->unknown30;
		}
		else
		{
			function_1c0b80(actor_index, state->unknown28, state->unknown38, &target, &unknown, &reference);
		}
		if (reference == NONE)
		{
			return false;
		}
		if (function_1f4460(actor_index, &target, unknown, NONE, false))
		{
			s_firing_position *position = firing_position_get(reference);

			state->unknown14.x = (real)cos(position->facing);
			state->unknown14.y = (real)sin(position->facing);
			state->unknown14.z = 0.0f;
			state->unknown20 = true;
			return true;
		}
		if (state->unknown38 >= 4)
		{
			return false;
		}
		state->unknown28[state->unknown38++] = reference;
	}
	return result;
}

// @retail 0x1c14b0
void __stdcall function_1c14b0(long actor_index, s_slot *slot)
{
	s_slot_82 *state = (s_slot_82 *)slot;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown504 == 2 && state->unknown20)
	{
		actor->unknown41c = 4;
		actor->unknown420 = 4;
		actor->unknown424.point = state->unknown14;
		actor->unknown44d = true;
	}
}

// @retail 0x1c1520
void __stdcall function_1c1520(long actor_index, s_slot *slot, long index)
{
	s_slot_82 *state = (s_slot_82 *)slot;

	if (state->unknown10 == index)
	{
		state->unknown10 = NONE;
	}
}

s_slot_handler_2 g_47ef58 =
{
	{
		0x82, 2, 0x1ff8, -2, 0,
		function_1c10a0, function_1c1160, function_1c10f0, 0, NONE, {0},
		0, 0, function_1c1520, 0, 0, 0, 0
	},
	(t_slot_proc)function_1c1320, slot_proc_nothing, function_1c14b0
};
