// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_2626b0.h"
#include "unknown_1f4460.h"
#include "units.h"
#include "data_array.h"
#include "unknown_0d0690.h"
#include "unknown_2605d0.h"
#include "unknown_0259a0.h"

bool function_1b8d80(long actor_index, long object_index, short seat_index, bool ignore_reserved);

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
bool function_e68c0(long type, long unit_index);
real function_11e000(point3f const *b, point3f const *a, vector3f const *d);

/* the slot tests 0x5b and 0x5c, and the slot types 0x5a, 0x59 and 0x51 */

struct s_slot_5a
{
	s_slot_header header;
	long unknown0c;
	byte unknown10[0x40 - 0x10];
};

void function_1f86a0(long index);

short __stdcall function_1bbf40(long actor_index, s_slot *slot);
short __stdcall function_1bc2a0(long actor_index, s_slot *slot);
short __stdcall function_1bc6d0(long actor_index);
short __stdcall function_1bc850(long actor_index, s_slot *slot, bool active);
bool __stdcall function_1bc810(long actor_index, s_slot *slot);
void __stdcall function_1c1520(long actor_index, s_slot *slot, long index);
void __stdcall function_1c1990(long actor_index, s_slot *slot, long index);
bool __stdcall function_1bc980(long actor_index, s_slot *slot);
bool __stdcall function_1bcab0(long actor_index, s_slot *slot);
void __stdcall function_1bcc10(long actor_index, s_slot *slot);

// @retail 0x1bc2a0
short __stdcall function_1bc2a0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	if (actor->unknown858 == NONE && actor->unknown26c != NONE && !actor->unknown269)
	{
		long vehicle_index = actor->unknown26c;
		s_slot_object_view *vehicle = object_get(vehicle_index);
		if (*(long *)((byte *)vehicle + 0x248) == NONE && vehicle->actor_index == NONE)
		{
			if ((actor->unknown358 == 2 || actor->unknown358 == 4) && prop_node_get(actor->unknown368)->unknown24 >= 1)
			{
				vector3f direction;
				point3f const *end = (point3f *)actor->unknown388;
				direction.i = end->x - actor->unknown370.x;
				direction.j = end->y - actor->unknown370.y;
				direction.k = end->z - actor->unknown370.z;
				real radius = actor->unknown36c;
				if (radius * radius > function_11e000(&actor->position, &actor->unknown370, &direction))
					goto trigger;
			}
			if (vehicle->unknownec < 0.1f)
				goto trigger;
		}
		if (actor->unknown358 == 2)
		{
			s_slot_object_view *object = (s_slot_object_view *)function_badc0(actor->unknown360, NONE);
			if (object && (object->parent_index == actor->unknown018 || object->parent_index == vehicle_index))
				goto trigger;
		}
	}
	return g_46fbe4;
trigger:
	function_e68c0(0x1d, actor->unknown018);
	return g_46fbe4;
}

/* the state of slot type 0x59 */
struct s_slot_59
{
	s_slot_header header;
	real unknown0c;
	long unknown10;
	bool unknown14;
	bool unknown15;
	bool unknown16;
	bool unknown17;
	byte unknown18[0x40 - 0x18];
};

inline real distance3d_fast(point3f const *a, point3f const *b)
{
	real i = a->x - b->x;
	real j = a->y - b->y;
	real k = a->z - b->z;

	return (real)sqrt(i * i + j * j + k * k);
}

void function_26c180(long actor_index);

// @retail 0x1bc420
short __stdcall function_1bc420(long actor_index)
{
	short result = 0;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown024 == 1 || !team_is_enemy(actor->unknown024, 1))
	{
		if (actor->unknown31c != NONE)
		{
			real seconds = g_510c54->field_2_3 * 2.0f;
			long ticks;

			__asm
			{
				fld seconds
				fistp ticks
			}
			if (actor->unknown31e > ticks)
				result = 3;
		}
	}
	return result;
}

// @retail 0x1bc4f0
bool __stdcall function_1bc4f0(long actor_index, s_slot *slot)
{
	short team = actor_get(actor_index)->unknown024;
	bool result = false;

	if (team == 1 || !team_is_enemy(team, 1))
	{
		function_1f86a0(actor_index);
		result = true;
	}
	return result;
}

// @retail 0x1bc580
short __stdcall function_1bc580(long actor_index, s_slot *slot, bool active)
{
	s_slot_5a *state = (s_slot_5a *)slot;
	short result = g_46fbe8;

	if (state->unknown0c == NONE || actor_get(actor_index)->unknown31c == NONE)
		result = g_46fbe4;
	return result;
}

/* the nearest actor of the actor's group not in a vehicle */
// @retail 0x1bc5d0
long function_1bc5d0(long actor_index, long ignore_index)
{
	s_actor_view *actor = actor_get(actor_index);
	long result = NONE;

	if (actor->unknown07c != NONE)
	{
		real best_distance = 3.4028235e38f;
		long index = element_502420_get(actor->unknown07c)->first_actor_index;

		while (index != NONE)
		{
			long other_index = index;
			s_actor_view *other = actor_get(other_index);

			index = other->next_index;
			if (other_index != actor_index && other_index != ignore_index && other->unknown26c == NONE)
			{
				real distance = distance3d_fast(&actor->position, &other->position);

				if (best_distance > distance)
				{
					best_distance = distance;
					result = other_index;
				}
			}
		}
	}
	return result;
}

/* whether the clump's vehicle (+0x26c) still has seats free that the actor
   cannot take while none of those it wants is taken */
// @retail 0x1bc6d0
short __stdcall function_1bc6d0(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	long result = 0;

	if (actor->unknown07c == NONE)
		return 0;

	if (element_502420_get(actor->unknown07c)->member_count > 1)
	{
		s_object_seat seats[0x40];
		short count = 0;
		long free_count = 0;
		long taken_count = 0;

		function_c8a40(actor->unknown26c, seats, &count, 0x40);
		for (short i = 0; i < count; i++)
		{
			s_object_seat *seat = &seats[i];

			if (!TEST_FIELD_BIT(seat->definition->flags.bit11))
			{
				bool wanted = TEST_FIELD_BIT(seat->definition->flags.bit3);

				if (function_c8f60(seat->object_index, seat->seat_index) != NONE)
				{
					if (wanted)
						taken_count++;
				}
				else if (!function_1b8d80(actor_index, seat->object_index, seat->seat_index, false) && wanted)
				{
					free_count++;
				}
			}
		}
		if ((short)taken_count == 0 && (short)free_count > 0)
			result = 3;
	}
	return (short)result;
}
// @retail 0x1bc810
bool __stdcall function_1bc810(long actor_index, s_slot *slot)
{
	s_slot_59 *state = (s_slot_59 *)slot;
	bool result = true;

	if (!state->unknown14)
	{
		state->unknown15 = true;
		state->unknown0c = 0.0f;
		state->unknown10 = function_1bc5d0(actor_index, NONE);
		state->unknown16 = true;
		state->unknown14 = true;
		result = state->unknown10 != NONE;
	}
	state->unknown17 = false;
	return result;
}

// @retail 0x1bc980
bool __stdcall function_1bc980(long actor_index, s_slot *slot)
{
	s_slot_59 *state = (s_slot_59 *)slot;
	bool result = true;

	if (state->unknown10 == NONE)
		return false;
	if (actor_get(actor_index)->unknown040)
	{
		s_actor_view *other = actor_get(state->unknown10);

		state->unknown17 = true;
		function_26c180(state->unknown10);
		if (other->unknown27c.unknown10 == NONE)
			return false;
		if (!function_1f4460(actor_index, &other->unknown27c.point, other->unknown27c.unknown10, NONE, false) && state->unknown15)
		{
			state->unknown10 = function_1bc5d0(actor_index, state->unknown10);
			state->unknown17 = false;
			return state->unknown10 != NONE;
		}
	}
	return result;
}

// @retail 0x1bca20
void __stdcall function_1bca20(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown50c)
	{
		actor->unknown41c = 2;
		actor->unknown420 = 0;
	}
}

// @retail 0x1bca60
short __stdcall function_1bca60(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (actor->prop_index != NONE && prop_node_get(actor->prop_index)->unknown26 >= 2)
		result = 3;
	return result;
}

// @retail 0x1bcc10
void __stdcall function_1bcc10(long actor_index, s_slot *slot)
{
	bool moving = actor_get(actor_index)->unknown504 == 2;
	s_actor_view *actor = actor_get(actor_index);

	if (moving)
		function_262800(actor_index, actor->unknown418, false);
}

bool function_1f8660(long actor_index);
real function_1f8940(long actor_index);
real function_1e3920(long actor_index);

// @retail 0x1bcab0
bool __stdcall function_1bcab0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = true;
	if (actor->unknown040)
	{
		s_2605d0_request request;
		memset(&request, 0, sizeof(request));
		request.type = 0;
		if (actor->unknown270 == 4 || actor->unknown270 == 5)
			request.unknown05b = true;
		if (!REFERENCE_EQUAL(actor->unknown418, g_470fa0) &&
			(!function_1f8660(actor_index) || function_1f8940(actor_index) < function_1e3920(actor_index) * 2.0f))
		{
			function_262800(actor_index, actor->unknown418, false);
		}
		else
		{
			*((bool *)&request + 0x692) = true;
			*(s_reference *)((byte *)&request + 0x694) = actor->unknown418;
		}
		byte *scratch = ai_scratch_buffer_get();
		long other_index;
		bool unknown;
		s_reference reference = function_261280((s_prop_search *)&request, actor_index, NULL, &other_index, scratch, &unknown);
		if (REFERENCE_EQUAL(reference, g_470fa0))
		{
			result = false;
			actor_get(actor_index)->unknown040 = false;
		}
		else
		{
			s_reference selected = function_2626b0(actor_index, reference, other_index, scratch, unknown, true);
			if (REFERENCE_EQUAL(selected, g_470fa0))
				function_262800(actor_index, reference, false);
		}
		ai_scratch_buffer_release(scratch);
	}
	return result;
}

// @retail 0x1bcc50
void __stdcall function_1bcc50(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	actor->unknown41c = 3;
	actor->unknown420 = 2;
	actor->unknown4ae = true;
	actor->unknown488 = true;
}

s_slot_handler_0 g_47ea3c =
{
	0x5b, 0, 0, -2, 0, function_1bbf40
};

s_slot_handler_0 g_47ea50 =
{
	0x5c, 0, 0, -2, 0, function_1bc2a0
};

s_slot_handler_2 g_47ea68 =
{
	{
		0x5a, 2, 0xbff, -2, 0,
		function_1bc420, function_1bc580, function_1bc4f0, slot_proc_nothing, NONE, {0},
		0, function_1c1990, 0, 0, 0, 0, 0
	},
	(t_slot_proc)slot_start_true, 0, slot_proc_nothing
};

s_slot_handler_2 g_47eab8 =
{
	{
		0x59, 2, 0xbff, -2, 0,
		function_1bc6d0, function_1bc850, function_1bc810, 0, NONE, {0},
		function_1c1520, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1bc980, 0, function_1bca20
};

s_slot_handler_2 g_47eb08 =
{
	{
		0x51, 2, 0, -2, 0,
		function_1bca60, function_1bced0, 0, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1bcab0, function_1bcc10, function_1bcc50
};

s_ai_player *ai_player_get(long player_index);
short ai_player_index_get(long player_index);
long function_1b8d40(long object_index);
short function_1b8cc0(long object_index, long *seat_object_index);
bool function_1b9200(long object_index, long prop_index);
point3f *function_b9dd0(long object_index, point3f *result);

// @retail 0x1bbf40
short __stdcall function_1bbf40(long actor_index, s_slot *slot)
{
    s_actor_view *actor = actor_get(actor_index);
    bool allow_exit = false;
    if (actor->unknown858 == NONE && actor->unknown26c != NONE && !actor->unknown269)
    {
        byte *tag = g_4e3b44[object_get(actor->unknown26c)->tag_index & 0xffff].bytes;
        bool special = *(short *)(tag + 0x1f0) == 6 && actor->unknown268;
        if (!special)
        {
            if (function_1b8d40(actor->unknown26c) != NONE)
                return g_46fbe4;
            long seat_object = NONE;
            short seat = function_1b8cc0(actor->unknown26c, &seat_object);
            allow_exit = true;
            if (!team_is_enemy(actor->unknown024, 1))
            {
                s_data_datum_iterator iterator;
                iterator.data = g_4e8c24;
                iterator.index = NONE;
                iterator.datum_index = NONE;
                while (data_datum_iterator_next(&iterator))
                {
                    if (iterator.datum_index == NONE)
                        continue;
                    s_ai_player *player = ai_player_get(iterator.datum_index);
                    short player_index = ai_player_index_get(iterator.datum_index);
                    if (player)
                    {
                        if (player->unit_index == seat_object && *(short *)((byte *)player + 8) == seat &&
                            *(short *)((byte *)player + 0xa) > 0)
                            return g_46fbe4;
                        if (actor->unknown31c == player_index)
                            return g_46fbe4;
                    }
                    long unit_index = *(long *)((byte *)iterator.datum + 0x2c);
                    if (unit_index != NONE)
                    {
                        point3f vehicle_point;
                        point3f unit_point;
                        function_b9dd0(actor->unknown26c, &vehicle_point);
                        function_b9dd0(unit_index, &unit_point);
                        real z = unit_point.z - vehicle_point.z;
                        real y = unit_point.y - vehicle_point.y;
                        real x = unit_point.x - vehicle_point.x;
                        if (z * z + y * y + x * x < 6.25f)
                        {
                            short minimum = (short)real_to_long((real)g_510c54->field_2_3 * 3.0f);
                            if (*(short *)actor->unknown2f4 < minimum)
                                *(short *)actor->unknown2f4 = minimum;
                            return g_46fbe4;
                        }
                    }
                }
            }
        }
        if (actor->unknown268)
        {
            if (actor->unknown086 >= 3)
            {
                short minimum = (short)real_to_long((real)g_510c54->field_2_3 * 10.0f);
                if (*(short *)actor->unknown2f4 < minimum)
                    *(short *)actor->unknown2f4 = minimum;
                allow_exit = false;
            }
            else
                allow_exit = true;
        }
        if (special && actor->prop_index != NONE && prop_node_get(actor->prop_index)->unknown24 >= 1 &&
            prop_node_get(actor->prop_index)->unknown24 <= 2 && !function_1b9200(actor->unknown26c, actor->prop_index))
        {
            *(short *)actor->unknown2f4 = 0;
            allow_exit = true;
        }
        if (allow_exit && *(short *)actor->unknown2f4 <= 0)
            function_e68c0(0x1d, actor->unknown018);
    }
    return g_46fbe4;
}
