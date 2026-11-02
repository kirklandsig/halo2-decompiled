// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259a0.h"
#include <math.h>

__forceinline s_firing_position *firing_position_get(long reference);

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

/* the same slot's data block after the header (s_slot's unknownc), as
   function_1c1160 reads it */
struct s_slot_82_data
{
	bool unknown00;
	byte unknown01[3];
	long unknown04;
	point3f unknown08;
	bool unknown14;
	bool unknown15;
	byte unknown16[2];
	long unknown18;
	long unknown1c[4];
	short unknown2c;
	byte unknown2e[0x34 - 0x2e];
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

PRIVATE __forceinline s_vehicle_tag_view *vehicle_tag_get(long object_index)
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

// score 300 (was 300): our stack frame is 4 bytes smaller than retail's, so
// every local offset is shifted and the two nested zone loops are laid out
// differently.
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
						s_actor_point_target const *position_target;
						point3f point;
						vector3f vector;
						real path_distance;
						short i;

						for (i = 0; i < excluded_count; i++)
						{
							if (excluded[i] == position_reference)
							{
								goto next_position;
							}
						}
						position = &zone->positions[position_index];
						position_target = &position->target;
						if (position_target->output_index == NONE)
							vector3d_from_points3d(&position_target->point, &actor->position, &vector);
						else
						{
							actor_point_target_position(position_target, &point);
							vector3d_from_points3d(&point, &actor->position, &vector);
						}
						if (!(length_sq3f(&vector) < g_46fbdc * g_46fbdc))
						{
							continue;
						}
						function_270750(buffer, position->unknown30, position_target, &path_distance, 0, 0);
						if (3.4028235e38f > path_distance)
						{
							real distance;
							real cost;
							vector3f direction;
							point3f cost_point, direction_point;

							actor_point_target_position(position_target, &cost_point);
							vector3d_from_points3d(&cost_point, prop_position, &vector);
							distance = (real)sqrt(length_sq3f(&vector)) + path_distance;
							if (position_target->output_index == NONE)
								vector3d_from_points3d(prop_position, &position_target->point, &direction);
							else
							{
								actor_point_target_position(position_target, &direction_point);
								vector3d_from_points3d(prop_position, &direction_point, &direction);
							}
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
								else
								{
									cost = 3.4028235e38f;
								}
							}
							else
							{
								cost = 3.4028235e38f;
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
					next_position:;
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
	s_slot *local_1 = *(s_slot *volatile const *)&slot;
	s_slot_82 *state = (s_slot_82 *)local_1;
	s_actor_view *local_2 = actor_get(actor_index);
	s_actor_view *local_4 = actor_get(actor_index);
	local_2->unknown50c = false;
	local_2->unknown5ac = NONE;
	local_2->unknown5b0 = NONE;
	local_4->unknown5b4 = 0;
	local_4->unknown5b6 = 0;
	local_4->unknown4ac = 0;
	local_4->unknown504 = 0;
	state->unknown0c = false;
	state->unknown10 = NONE;
	state->unknown38 = 0;
	state->unknown20 = false;
	return true;
}

// @retail 0x1c1160
short __stdcall function_1c1160(long actor_index, s_slot *slot, bool active)
{
	short result = g_46fbe8;
	s_slot_82_data *state = (s_slot_82_data *)slot->unknownc;

	if (actor_get(actor_index)->unknown504 == 2)
	{
		s_actor_view *actor = actor_get(actor_index);
		long unit_index;

		if (bool vector_request = actor->unknown22a)
		{
			unit_index = actor->unknown018;
			if (!function_110ab0(unit_index))
			{
				real alignment = 1.0f;

				if (state->unknown14)
				{
					alignment = dot3f((vector3f *)&state->unknown08, &actor->unknown290);
				}
				if (alignment > 0.9f)
				{
					s_unit_request request;

					request.type = 0x35;
					request.type35.has_vector = state->unknown14;
					if (state->unknown14)
					{
						request.type35.vector = *(vector3f *)&state->unknown08;
					}
					state->unknown00 = function_e6900(actor->unknown018, &request);
				}
			}
		}
		else
		{
			unit_index = actor->unknown018;
			if (!function_110ab0(unit_index))
			{
				long vehicle_index = state->unknown04;

				result = g_46fbe4;
				if (vehicle_index != NONE)
				{
					s_vehicle_tag_view *tag = vehicle_tag_get(vehicle_index);
					short seat_index;

					for (seat_index = 0; seat_index < tag->seat_count; seat_index++)
					{
						s_vehicle_seat_view *seat = &tag->seats[seat_index];

						if (TEST_FIELD_BIT(seat->flags.bit3))
						{
							if (seat_index != NONE && function_c8f60(vehicle_index, seat_index) == NONE)
							{
								long request_unit = actor->unknown018;
								s_unit_request action;

								action.type1c.object_index = vehicle_index;
								action.type = 0x1c;
								action.type1c.seat_index = seat_index;
								action.type1c.unknowna = false;
								action.type1c.unknownb = false;
								function_e6900(request_unit, &action);
							}
							break;
						}
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
	bool result = true;

	if (actor_get(actor_index)->unknown040 &&
		!(actor_get(actor_index)->unknown50c && actor_get(actor_index)->unknown504 == 1) &&
		actor_get(actor_index)->unknown504 != 2)
	{
		s_slot_82_data *state = (s_slot_82_data *)slot->unknownc;
		s_actor_point_target target;
		long target_index = NONE;
		long position_reference = NONE;

		if (state->unknown15)
		{
			s_firing_position *position;

			position_reference = state->unknown18;
			position = firing_position_get(position_reference);
			target = position->target;
			target_index = position->unknown30;
		}
		else
		{
			function_1c0b80(actor_index, state->unknown1c, state->unknown2c, &target, &target_index, &position_reference);
		}
		if (position_reference == NONE)
		{
			result = false;
			goto done;
		}
		if (function_1f4460(actor_index, &target, target_index, NONE, false))
		{
			s_firing_position *position = firing_position_get(position_reference);

			state->unknown08.x = (real)cos(position->facing);
			state->unknown08.y = (real)sin(position->facing);
			state->unknown08.z = 0.0f;
			state->unknown14 = true;
			result = true;
			goto done;
		}
		if (state->unknown2c < 4)
		{
			state->unknown1c[state->unknown2c++] = position_reference;
		}
		else
		{
			result = false;
		}
	}
done:
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
