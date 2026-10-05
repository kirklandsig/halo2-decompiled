// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_1f4460.h"

/* slot type 0x20 */

struct s_slot_20
{
	s_slot_header header;
	bool unknown0c;
	bool unknown0d;
	byte unknown0e[2];
	long unknown10;
	long unknown14;
	s_location_view unknown18;
	byte unknown2c[0x40 - 0x2c];
};

short __stdcall function_1be4b0(long actor_index);
short __stdcall function_1be6e0(long actor_index, s_slot *slot, bool active);
bool __stdcall function_1be630(long actor_index, s_slot *slot);
void __stdcall function_1c1520(long actor_index, s_slot *slot, long index);
bool __stdcall function_1be840(long actor_index, s_slot *slot);
void __stdcall function_1be8f0(long actor_index, s_slot *slot);
void function_26bfa0(long object_index, long *location_index, s_location_view *location);
void function_1f86a0(long index);

struct s_character_a50
{
	byte unknown00[4];
	real unknown4;
};

long function_1e4a50(long index);
bool function_26fc80(long actor_index, long object_index, real distance, void *path, point3f *point);
bool function_e4050(long object_index);

/* where the actor goes to reach the object */
// @retail 0x1be410
bool function_1be410(point3f *point, long object_index, long actor_index)
{
	s_slot_object_view *object = object_get(object_index);
	real distance = 0.2f;
	s_character_a50 *character = (s_character_a50 *)function_1e4a50(actor_get(actor_index)->unknown054);
	byte path[0x70];

	if (character)
		distance = character->unknown4;
	if (!function_26fc80(actor_index, object_index, distance, path, point))
	{
		*point = object->unknown030;
		point->z += object->unknown03c;
	}
	return true;
}

// @retail 0x1be630
bool __stdcall function_1be630(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = false;

	if (actor->prop_index != NONE && actor->unknown348 != NONE)
	{
		s_slot_20 *state = (s_slot_20 *)slot;

		function_26bfa0(actor->unknown348, &state->unknown14, &state->unknown18);
		if (state->unknown14 != NONE)
		{
			state->unknown10 = actor->unknown348;
			function_1f86a0(actor_index);
			return true;
		}
	}
	return result;
}

// @retail 0x1be6b0
void __stdcall function_1be6b0(long actor_index, s_slot *slot)
{
	actor_get(actor_index)->unknown228 = false;
}

// @retail 0x1be840
bool __stdcall function_1be840(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_20 *state = (s_slot_20 *)slot;
	bool result = true;

	if (state->unknown10 == NONE)
		return false;
	if (actor->unknown348 != state->unknown10 && !state->unknown0d)
		return true;
	if (state->unknown0c)
	{
		if (object_get(actor->unknown018)->type != 0)
			return false;
		if (function_e4050(actor->unknown018))
			state->unknown0d = true;
		return true;
	}
	if (!function_1f4460(actor_index, &state->unknown18.point, state->unknown14, state->unknown10, false))
		return false;
	return result;
}

// @retail 0x1be9d0
void __stdcall function_1be9d0(long actor_index, s_slot *slot)
{
	s_slot_20 *state = (s_slot_20 *)slot;

	state->unknown10 = NONE;
}

s_slot_handler_2 g_47ed58 =
{
	{
		0x20, 2, 0, -2, 0,
		function_1be4b0, function_1be6e0, function_1be630, function_1be6b0, NONE, {0},
		0, 0, function_1c1520, function_1be9d0, 0, 0, 0
	},
	(t_slot_proc)function_1be840, slot_proc_nothing, function_1be8f0
};
