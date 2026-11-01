// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259d0.h"
#include "units.h"
#include "ai_actor.h"
#include "unknown_1e3920.h"
#include "unit_requests.h"

/* slot type 0x50: boarding a vehicle; handler g_47eeb8 */

struct s_slot_vehicle_board
{
	s_slot_header header;
	long prop_index;
	long vehicle_index;
	short seat_index;
	short vehicle_seat_index;
	bool boarding;
	bool requested;
	bool unknown1a;
	bool unknown1b;
	point3f unknown1c;
	byte unknown28[0x40 - 0x28];
};

/* s_slot_vehicle_board without the slot header: the slot's data block (s_slot::unknownc),
   fields by block offset */
struct s_slot_50_data
{
	long prop_index;
	long vehicle_index;
	short seat_index;
	short vehicle_seat_index;
	bool boarding;
	bool requested;
	bool unknown0e;
	bool unknown0f;
	point3f unknown10;
	byte unknown1c[0x34 - 0x1c];
};

/* the block of the actor's character tag character_vehicle_settings_get returns */
struct s_character_vehicle
{
	byte flags;
	byte unknown01[3];
	real unknown04;
	real unknown08;
	real unknown0c;
};

/* a unit's tag and its seats */
struct s_vehicle_board_unit_tag
{
	byte unknown000[0x1c8];
	long seat_count;
	s_unit_seat_definition *seats;
};

long character_vehicle_settings_get(long index);
bool function_10f630(long object_index, long *first, long *second);
bool function_1bf7f0(long actor_index, short seat_index, long object_index);

short __stdcall function_1bf990(long actor_index);
short __stdcall function_1bfd00(long actor_index, s_slot *slot, bool active);
bool __stdcall function_1bfb10(long actor_index, s_slot *slot);
void __stdcall function_1bfc30(long actor_index, s_slot *slot);
void __stdcall actor_behavior_release_prop(long actor_index, s_slot *slot, long index);
void __stdcall function_1c1520(long actor_index, s_slot *slot, long index);
bool __stdcall function_1bff80(long actor_index, s_slot *slot);
void __stdcall actor_behavior_50_update48(long actor_index, s_slot *slot);

inline s_vehicle_board_unit_tag *vehicle_board_unit_tag_get(s_slot_object_view *unit)
{
	return (s_vehicle_board_unit_tag *)g_4e3b44[unit->tag_index & 0xffff].bytes;
}

/* the view of a prop's tracking data, or NULL */
inline s_prop_view_fields *vehicle_board_prop_view_get(long tracking_index)
{
	s_prop_view_fields *view = NULL;

	if (tracking_index != NONE)
	{
		byte *tracking = g_502414->data + (tracking_index & 0xffff) * 0x124;

		if (tracking)
			view = (s_prop_view_fields *)(tracking + 0x70);
	}
	return view;
}

// @retail 0x1bf990
short __stdcall function_1bf990(long actor_index)
{
	short result = 0;
	s_actor_view *actor = actor_get(actor_index);

	if (!actor->unknown224 && actor->prop_index != NONE && actor->unknown26c == NONE && actor->unknown018 != NONE)
	{
		s_character_vehicle *character = (s_character_vehicle *)character_vehicle_settings_get(actor->unknown054);

		if (character)
		{
			s_prop_node_view *node = prop_node_get(actor->prop_index);

			if (node->unknown24 >= 1 && node->unknown24 <= 2 &&
				(!function_1e2030(actor_index) || character->unknown04 > node->unknown28))
			{
				s_prop_state_view *state = prop_node_state(node);

				if (function_25d740((s_prop_node *)node) && state->unknown3c != NONE)
				{
					s_slot_object_view *object = object_get(state->unknown3c);

					if (length_sq3f(&object->velocity) < character->unknown0c * character->unknown0c)
					{
						s_slot_object_view *unit = object_get(node->object_index);

						if (unit->parent_index != NONE && function_1bf7f0(actor_index, unit->unknown1fc, unit->parent_index))
							result = 3;
					}
				}
			}
		}
	}
	return result;
}

// @retail 0x1bfc30
void __stdcall function_1bfc30(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_object_view *unit = object_get(actor->unknown018);

	if (unit->parent_index != NONE && unit->unknown1fc != NONE)
	{
		s_vehicle_board_unit_tag *tag = vehicle_board_unit_tag_get(object_get(unit->parent_index));

		if (TEST_FIELD_BIT(tag->seats[unit->unknown1fc].flags.bit11))
		{
			long first;
			long second = NONE;

			function_10f630(actor->unknown018, &first, &second);
			if (second != 0x50000c3 && !function_e68c0(0x1d, actor->unknown018))
				function_e68c0(0x1e, actor->unknown018);
		}
	}
}

PRIVATE __forceinline word function_1bfd01(long arg_0, s_slot *arg_1, bool arg_2)
{
	s_actor_view *actor = actor_get(arg_0);
	s_slot_vehicle_board *state = (s_slot_vehicle_board *)arg_1;
	short result = g_46fbe4;

	if (actor->prop_index == state->prop_index && state->vehicle_index != NONE)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_prop_view_fields *view = vehicle_board_prop_view_get(node->view_index);

		if (node->unknown24 >= 1 && node->unknown24 <= 2 || view && view->unknown00 >= 5)
		{
			s_slot_object_view *unit;
			s_slot_object_view *target;
			bool seated;

			result = g_46fbe8;
			unit = object_get(actor->unknown018);
			target = object_get(node->object_index);
			seated = target->parent_index == state->vehicle_index && target->unknown1fc == state->seat_index;
			if (actor->unknown26c == NONE)
			{
				state->boarding = false;
				if (seated)
				{
					s_character_vehicle *character = (s_character_vehicle *)character_vehicle_settings_get(actor->unknown054);

					if (character && (!function_1e2030(arg_0) || !(node->unknown28 > character->unknown08)))
					{
						s_prop_state_view *s_type_5cfb45 = prop_node_state(node);

						if (function_25d740((s_prop_node *)node) && s_type_5cfb45->unknown3c != NONE)
						{
							s_slot_object_view *object = object_get(s_type_5cfb45->unknown3c);
							real speed = character->unknown0c * 1.2f;

							if (!(length_sq3f(&object->velocity) > speed * speed))
								return result;
						}
					}
				}
				function_1f86a0(arg_0);
				result = g_46fbe4;
			}
			else if (unit->parent_index == state->vehicle_index && unit->unknown1fc == state->vehicle_seat_index)
			{
				if (state->boarding)
				{
					if (!seated && state->requested)
						result = g_46fbe4;
				}
				else
				{
					long first;
					long second;

					if (function_10f630(actor->unknown018, &first, &second))
					{
						if (second != 0x5000049)
							state->boarding = true;
					}
					else
					{
						result = g_46fbe4;
					}
				}
			}
			else
			{
				result = g_46fbe4;
			}
		}
	}
	return result;
}

// @retail 0x1bfd00
short __stdcall function_1bfd00(long actor_index, s_slot *slot, bool active)
{
	return (short)function_1bfd01(actor_index, slot, active);
}

s_slot_handler_2 g_47eeb8 =
{
	{
		0x50, 2, 0, -2, 0,
		function_1bf990, function_1bfd00, function_1bfb10, function_1bfc30, NONE, {0},
		0, actor_behavior_release_prop, function_1c1520, 0, 0, 0, 0
	},
	(t_slot_proc)function_1bff80, 0, actor_behavior_50_update48
};

long function_1e4a50(long index);
short function_1bf890(long actor_index, long object_index, short seat_index, bool vertical);

// @retail 0x1bfb10
bool __stdcall function_1bfb10(long actor_index, s_slot *slot)
{
    bool result = false;
    s_slot *volatile *local_0 = &slot;
    s_slot *local_1 = *local_0;
    s_actor_view *actor = actor_get(actor_index);
    s_slot_object_view *unit = object_get(prop_node_get(actor->prop_index)->object_index);
    long vehicle_index = unit->parent_index;
    s_slot_vehicle_board *state = (s_slot_vehicle_board *)local_1;
    if (object_header_get(vehicle_index)->type == 1)
    {
        byte *movement = (byte *)function_1e4a50(actor->unknown054);
        byte *boarding = (byte *)character_vehicle_settings_get(actor->unknown054);
        bool vertical = (boarding[0] & 1) && (actor->unknown229 || (movement && (movement[0] & 0x20)));
        short seat = function_1bf890(actor_index, vehicle_index, unit->unknown1fc, vertical);
        if (seat != NONE)
        {
            state->prop_index = actor->prop_index;
            state->vehicle_index = unit->parent_index;
            state->seat_index = unit->unknown1fc;
            state->vehicle_seat_index = seat;
            state->boarding = false;
            state->requested = false;
            state->unknown1b = false;
            state->unknown1a = vertical;
            result = true;
        }
    }
    return result;
}

struct s_1bff80
{
    point3f field_0;
    vector3f field_c;
    real field_18;
    bool field_1c;
    bool field_1d;
    bool field_1e;
};
struct s_seat_approach_result;
bool function_1baae0(long arg_0, long arg_1, short arg_2, bool arg_3,
    bool arg_4, bool arg_5, s_seat_approach_result *arg_6);
bool __stdcall function_1badc0(long arg_0, long arg_1, point3f const *arg_2,
    s_type_c3b527 *arg_3, long *arg_4, bool arg_5, bool *arg_6);
bool function_1f45c0(long arg_0, s_type_c3b527 const *arg_1, long arg_2, bool arg_3);

// @retail 0x1bff80
bool __stdcall function_1bff80(long arg_0, s_slot *arg_1)
{
    volatile bool local_5 = true;
    s_actor_view *local_0 = actor_get(arg_0);
    long local_2 = local_0->unknown018;
    s_slot_object_view *local_3 = object_get(local_2);
    s_slot_50_data *local_1 = (s_slot_50_data *)arg_1->unknownc;
    s_slot_object_view *local_4 = object_get(prop_node_get(local_0->prop_index)->object_index);
    if (local_1->boarding)
    {
        if (local_3->parent_index == local_1->vehicle_index &&
            local_3->unknown1fc == local_1->vehicle_seat_index)
        {
            bool local_6 = local_4->parent_index == local_1->vehicle_index &&
                local_4->unknown1fc == local_1->seat_index;
            long local_7;
            long local_8;
            if (function_10f630(local_2, &local_7, &local_8))
            {
                if (local_8 != 0x400000c)
                    return local_5;
                if (!local_1->requested)
                {
                    function_e68c0(0x1f, local_2);
                    local_1->requested = true;
                    return local_5;
                }
                if (!local_6)
                    return local_5;
                s_unit_request local_9;
                local_9.type = 0x1a;
                local_9.type1a.unknown4 = 0;
                local_9.type1a.unknown6 = false;
                if (function_e6900(local_2, &local_9))
                    return local_5;
            }
            local_5 = false;
        }
    }
    else if (local_0->unknown26c == NONE)
    {
        s_1bff80 local_10;
        if (function_1baae0(arg_0, local_1->vehicle_index, local_1->vehicle_seat_index,
            false, true, local_1->unknown0e, (s_seat_approach_result *)&local_10))
        {
            if (local_10.field_1c && local_10.field_1d)
            {
                s_unit_request local_11;
                local_11.type = 0x1c;
                local_11.type1c.object_index = local_1->vehicle_index;
                local_11.type1c.seat_index = local_1->vehicle_seat_index;
                local_11.type1c.unknowna = false;
                local_11.type1c.unknownb = false;
                if (!function_e6900(local_0->unknown018, &local_11))
                    local_5 = false;
            }
            else
            {
                if (local_0->unknown040)
                {
                    bool local_12;
                    s_type_c3b527 local_13;
                    long local_14;
                    bool local_15 = function_1badc0(arg_0, local_1->vehicle_index, &local_10.field_0,
                        &local_13, &local_14, local_1->unknown0e, &local_12);
                    if (local_15)
                    {
                        if (local_1->unknown0e)
                            local_15 = function_1f45c0(arg_0, &local_13, local_12 ? NONE : local_1->vehicle_index, false);
                        else
                            local_15 = function_1f4460(arg_0, &local_13, local_14, local_12 ? NONE : local_1->vehicle_index, false);
                    }
                    if (!local_15 && (local_0->unknown229 || *(short *)((byte *)local_0 + 0x5b4) >= 0x20))
                        local_5 = false;
                }
                real local_17 = local_10.field_0.x - local_0->position.x;
                real local_18 = local_10.field_0.y - local_0->position.y;
                real distance_squared = local_17 * local_17;
                distance_squared += local_18 * local_18;
                if ((real)sqrt(distance_squared) < 1.5f)
                {
                    local_1->unknown0f = true;
                    local_1->unknown10 = *(point3f *)&local_10.field_c;
                }
                else
                    local_1->unknown0f = false;
            }
        }
    }
    return local_5;
}
