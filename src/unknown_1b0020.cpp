// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_2626b0.h"
#include "unknown_1e3920.h"

/* slot type 0x3a (its first callbacks, from 0x1afde0, precede the region) */

struct s_slot_3a
{
	s_slot_header header;
	byte unknown0c[4];
	s_reference reference;
	byte unknown14;
	bool unknown15;
	bool unknown16;
	byte unknown17[0x40 - 0x17];
};

short __stdcall function_1afde0(long actor_index);
bool __stdcall function_1afe50(long actor_index, s_slot *slot);
void __stdcall function_1c0b60(long actor_index, s_slot *slot, long index);
void __stdcall function_1aff10(long actor_index, s_slot *slot);
void __stdcall function_1b0020(long actor_index, s_slot *slot);
void __stdcall function_1b0110(long actor_index, s_slot *slot);

// @retail 0x1b0270
short __stdcall function_1b0270(long actor_index, s_slot *slot, bool active)
{
	s_slot_3a *state = (s_slot_3a *)slot;
	short result = g_46fbe8;

	if (state->unknown15 || state->unknown16)
	{
		g_46eeb8[0x3a]->unknown8 = g_46f348;
		result = g_46fbe4;
	}
	return result;
}

// @retail 0x1b06f0
void __stdcall function_1b06f0(long actor_index, s_slot *slot, bool active)
{
	s_slot_3a *state = (s_slot_3a *)slot;

	if (!active || (state->reference.unknown2 & 0x8000))
		state->reference = g_470fa0;
}

s_slot_handler_2 g_47dec0 =
{
	{
		0x3a, 2, 0, -2, 0,
		function_1afde0, function_1b0270, function_1afe50, slot_proc_nothing, 0x38, {0},
		function_1c0b60, 0, 0, 0, function_1b06f0, 0, 0
	},
	function_1aff10, function_1b0020, function_1b0110
};

/* Whether the actor has arrived at its reference and is free of a waiting prop. */
// @retail 0x1b02a0
bool function_1b02a0(long actor_index)
{
	s_reference invalid_reference = g_470fa0;
	s_record_pool *actors = g_4f55f0;
	s_actor_view *actor = (s_actor_view *)(actors->data + (actor_index & 0xffff) * sizeof(s_actor_view));
	bool result = false;

	if (!REFERENCE_EQUAL(actor->unknown418, invalid_reference))
	{
		s_type_c3b527 *target = (s_type_c3b527 *)function_262b40(actor->unknown418);

		if (target)
		{
			if (((s_actor_view *)actors->data)[actor_index & 0xffff].unknown504 == 2 &&
				(actor->unknown4ac == 4 || actor->unknown4ac == 6 || actor->unknown4ac == 5) &&
				REFERENCE_EQUAL(actor->unknown4b8, actor->unknown418))
			{
				return true;
			}

			volatile real radius = function_1e3920(actor_index);
			point3f point;
			vector3f offset;

			function_210850(target, &point);
			vector3d_from_points3d(&actor->position, &point, &offset);
			real distance_squared = offset.j * offset.j + offset.i * offset.i + offset.k * offset.k;
			real distance = radius;
			if (distance * distance > distance_squared)
			{
				if (actor->unknown344 == NONE)
					return true;
				bool waiting = false;
				s_prop_view_fields *view = prop_view_fields_get(actor->unknown344);

				if (view && (view->unknown06 == 0 || view->unknown06 == 1))
					waiting = true;
				return !waiting;
			}
		}
	}
	return result;
}
