// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_2626b0.h"
#include "unknown_2605d0.h"
#include "unknown_0259a0.h"

/* slot type 6 */

/* g_51e9d8: 0x98 byte elements, each naming an entry (0x7c bytes) of the
   table at +0x244 of g_4e0350 */
struct s_51e9d8_element
{
	byte unknown00[0x2a];
	short entry_index;
	byte unknown2c[0x80 - 0x2c];
	long unknown80;
	byte unknown84[0x98 - 0x84];
};

struct s_4e0350_entry
{
	byte unknown00[0x24];
	dword flags;
	byte unknown28[0x4e - 0x28];
	short unknown4e;
	byte unknown50[0x7c - 0x50];
};

struct s_4e0350_view
{
	byte unknown000[0x244];
	s_4e0350_entry *entries;
};

short __stdcall function_1b0780(long actor_index);
bool __stdcall function_1b0ab0(long actor_index, s_slot *slot);


point3f *function_b9dd0(long object_index, point3f *result);

real distance_sq3f(point3f const *a, point3f const *b); /* unknown_023540.cpp */

void function_26c180(long actor_index);
void function_26bfa0(long object_index, long *location_index, s_location_view *location);
real function_30bf0(vector3f *v);

// @retail 0x1b0ab0
bool __stdcall function_1b0ab0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	if (actor->unknown040 && actor->unknown030 != NONE)
	{
		s_51e9d8_element *element = (s_51e9d8_element *)(g_51e9d8->data + (actor->unknown030 & 0xffff) * sizeof(s_51e9d8_element));
		if (element->unknown80 != NONE)
		{
			s_slot_object_view *object = object_get(element->unknown80);
			s_2605d0_request request;
			memset(&request, 0, sizeof(request));
			*(short volatile *)&request.type = 5;
			*((bool volatile *)&request + 0x20) = true;
			function_b9dd0(((volatile s_51e9d8_element *)element)->unknown80, (point3f *)((byte *)&request + 0x24));
			*(long *)((byte *)&request + 0x40) = NONE;
			*(short *)((byte *)&request + 0x44) = *(short *)((byte *)object + 0x2c);
			*((bool *)&request + 0x11) = true;
			*((bool *)&request + 0x59) = true;
			*((bool *)&request + 0x668) = true;
			vector3f *direction = (vector3f *)((byte *)&request + 0x66c);
			*direction = object->velocity;
			function_30bf0(direction);
			if (object->actor_index != NONE)
			{
				s_actor_view *other = actor_get(object->actor_index);
				function_26c180(object->actor_index);
				*(long *)((byte *)&request + 0x40) = other->unknown27c.unknown10;
				*(s_type_c3b527 *)((byte *)&request + 0x30) = other->unknown27c.point;
			}
			else
			{
				function_26bfa0(((volatile s_51e9d8_element *)element)->unknown80, (long *)((byte *)&request + 0x40),
					(s_location_view *)((byte *)&request + 0x30));
			}
			byte *scratch = ai_scratch_buffer_get();
			long other_index;
			bool unknown;
			s_reference reference = function_261280((s_prop_search *)&request, actor_index, NULL, &other_index, scratch, &unknown);
			if (!REFERENCE_EQUAL(reference, g_470fa0))
				function_2626b0(actor_index, reference, other_index, scratch, unknown, true);
			ai_scratch_buffer_release(scratch);
		}
	}
	return true;
}

/* the squared distances within which the actor follows the object */
// @retail 0x1b0710
void function_1b0710(long object_index, long actor_index, real *distance_squared, real *maximum_distance_squared)
{
	real result = 100.0f;

	if (object_get(object_index)->unknown1fc == NONE && actor_get(actor_index)->unknown26c == NONE)
		result = 16.0f;
	if (distance_squared)
		*distance_squared = result;
	if (maximum_distance_squared)
		*maximum_distance_squared = result * 2.25f;
}

// @retail 0x1b0780
short __stdcall function_1b0780(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (actor->unknown030 != NONE)
	{
		s_51e9d8_element *element = (s_51e9d8_element *)(g_51e9d8->data + (actor->unknown030 & 0xffff) * sizeof(s_51e9d8_element));

		if (element->entry_index != NONE)
		{
			s_4e0350_entry *entry = &((s_4e0350_view *)g_4e0350)->entries[element->entry_index];

			if (entry && ((entry->flags & 0x10) || (entry->flags & 0x20) && entry->unknown4e != NONE) &&
				element->unknown80 != NONE &&
				!function_1df560(actor->unknown024, object_get(element->unknown80)->team))
			{
				real maximum_distance_squared;
				point3f position;

				function_1b0710(element->unknown80, actor_index, NULL, &maximum_distance_squared);
				function_b9dd0(element->unknown80, &position);
				if (distance_sq3f(&actor->position, &position) > maximum_distance_squared &&
					(*(long *)&actor->unknown418 == *(long *)&g_470fa0 || !actor->unknown5d0 ||
					function_210b60((s_type_c3b527 *)function_262b40(actor->unknown418), &actor->position) > maximum_distance_squared))
				{
					s_261d20_entry entries[0x200];
					short count = function_261d20(actor_index, entries, 0x200, NULL);

					for (short i = 0; i < count; i++)
					{
						if (maximum_distance_squared > distance_sq3f(&entries[i].point, &position))
						{
							result = 3;
							break;
						}
					}
				}
			}
		}
	}
	return result;
}

// @retail 0x1b09b0
bool __stdcall function_1b09b0(long actor_index, s_slot *slot)
{
	actor_reset_state(actor_index);
	return true;
}

// @retail 0x1b0a10
short __stdcall function_1b0a10(long actor_index, s_slot *slot, bool active)
{
	short result = g_46fbe4;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown504 != 2 && actor->unknown030 != NONE)
	{
		s_51e9d8_element *element = (s_51e9d8_element *)(g_51e9d8->data + (actor->unknown030 & 0xffff) * sizeof(s_51e9d8_element));

		if (element->entry_index != NONE)
		{
			s_4e0350_entry *entry = &((s_4e0350_view *)g_4e0350)->entries[element->entry_index];

			if (entry && ((entry->flags & 0x10) || (entry->flags & 0x20) && entry->unknown4e != NONE) &&
				element->unknown80 != NONE)
			{
				result = g_46fbe8;
			}
		}
	}
	return result;
}

// @retail 0x1b0c70
void __stdcall function_1b0c70(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown270 != 4)
	{
		if (actor->unknown086 >= 7)
		{
			actor->unknown488 = true;
			actor->unknown41c = 3;
			actor->unknown420 = 2;
		}
		else
		{
			actor->unknown41c = 2;
			actor->unknown420 = 2;
		}
	}
}

s_slot_handler_2 g_47df10 =
{
	{
		6, 2, 0, -2, 0,
		function_1b0780, function_1b0a10, function_1b09b0, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1b0ab0, slot_proc_nothing, function_1b0c70
};
