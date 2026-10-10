// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_25d020.h"

/* slot types 9 and 0x1d */

struct s_slot_09
{
	s_slot_header header;
	bool unknown0c;
	byte unknown0d[0x40 - 0xd];
};

/* the tag of an object, with a block of flagged elements at +0x5c */
struct s_object_tag_view
{
	byte unknown00[0x5c];
	long count;
	byte *elements;
};

bool __stdcall function_1be0b0(long actor_index, s_slot *slot);
bool __stdcall function_1be120(long actor_index, s_slot *slot);
void __stdcall function_1be370(long actor_index, s_slot *slot);
void __stdcall function_1f4280(long actor_index);

// @retail 0x1be040
short __stdcall function_1be040(long actor_index)
{
	short result = 0;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown5ac != NONE && actor->unknown5b0 == 6 && object_get(actor->unknown5ac)->unknownec > 0.0f)
		result = 3;
	return result;
}

// @retail 0x1be0b0
bool __stdcall function_1be0b0(long actor_index, s_slot *slot)
{
	if (*(long *)&actor_get(actor_index)->unknown418 != *(long *)&g_470fa0)
		function_2628f0(actor_index, g_470fa0);
	return true;
}

// @retail 0x1be0f0
short __stdcall function_1be0f0(long actor_index, s_slot *slot, bool active)
{
	short result = g_46fbe4;

	if (function_1be040(actor_index))
		result = g_46fbe8;
	return result;
}

point3f *function_b9dd0(long object_index, point3f *result);
short __stdcall function_1c8df0(long object_index, point3f const *point, short team, short type,
	void const *origin, long count, bool unknown, bool unknown2, bool unknown3, long *arg_9);

// @retail 0x1be120
bool __stdcall function_1be120(long actor_index, s_slot *slot)
{
	bool result = true;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown040)
	{
		result = false;
		if (actor->unknown5ac != NONE && actor->unknown5b0 == 6)
		{
			s_slot_09 *state = (s_slot_09 *)slot;
			s_slot_object_view *object = object_get(actor->unknown5ac);
			point3f position;
			s_2641c0 origin;
			function_b9dd0(actor->unknown5ac, &position);
			state->unknown0c = false;
			if (function_2641c0(actor_index, &origin, &position) &&
				function_1c8df0(actor->unknown5ac, &position, *(short *)((byte *)object + 0x2c),
					*(short *)((byte *)&origin + 0x28), (point3f const *)&origin, 1,
					false, false, true, NULL) == 0)
				state->unknown0c = true;
			result = true;
		}
	}
	return result;
}

// @retail 0x1be1e0
void __stdcall function_1be1e0(long actor_index, s_slot *slot)
{
	s_slot_09 *state = (s_slot_09 *)slot;
	s_actor_view *actor = actor_get(actor_index);

	if (state->unknown0c)
	{
		actor->unknown480 = true;
		if (actor->unknown5ac != NONE)
		{
			s_slot_object_view *object = object_get(actor->unknown5ac);

			actor->unknown488 = true;
			actor->unknown48c = true;
			actor->unknown4a0 = true;
			actor->unknown490 = object->unknown030;
			actor->unknown49c = NONE;
			actor->unknown41c = 4;
			actor->unknown420 = 6;
			actor->unknown424.object_index = actor->unknown5ac;
		}
	}
	else if (actor->unknown5ac != NONE)
	{
		actor->unknown41c = 2;
		actor->unknown420 = 6;
		actor->unknown424.object_index = actor->unknown5ac;
	}
}

// @retail 0x1be370
void __stdcall function_1be370(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown5d0)
		function_1f4280(actor_index);
	if (actor->prop_index != NONE && actor->unknown348 != NONE)
	{
		s_slot_object_view *object = object_get(actor->unknown348);

		actor->unknown488 = true;
		actor->unknown48c = true;
		actor->unknown4a0 = true;
		actor->unknown490 = object->unknown030;
		actor->unknown49c = NONE;
	}
}

// @retail 0x1be2b0
short __stdcall function_1be2b0(long actor_index)
{
	short result = 0;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE && actor->unknown348 != NONE)
	{
		s_slot_object_view *object = object_get(actor->unknown348);
		s_object_tag_view *tag = (s_object_tag_view *)g_4e3b44[object->tag_index & 0xffff].bytes;

		if (tag->count > 0 && (tag->elements[0] & 1) && object->unknownec > 0.0f)
			result = 3;
	}
	return result;
}

// @retail 0x1be340
short __stdcall function_1be340(long actor_index, s_slot *slot, bool active)
{
	short result = g_46fbe4;

	if (function_1be2b0(actor_index) > 0)
		result = g_46fbe8;
	return result;
}

s_slot_handler_2 g_47ecb8 =
{
	{
		9, 2, 0, -2, 0,
		function_1be040, function_1be0f0, function_1be0b0, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1be120, slot_proc_nothing, function_1be1e0
};

s_slot_handler_2 g_47ed08 =
{
	{
		0x1d, 2, 0, -2, 0,
		function_1be2b0, function_1be340, slot_start_true, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)slot_start_true, slot_proc_nothing, function_1be370
};
