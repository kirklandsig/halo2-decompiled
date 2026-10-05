// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_2605d0.h"
#include "unknown_2626b0.h"
#include <string.h>

/* slot group 0x22 and slot type 0x27 */

short __stdcall function_1b5aa0(long actor_index, s_slot *slot, bool active);
bool __stdcall function_1b5c00(long actor_index, s_slot *slot);
short __stdcall function_1b5e00(long actor_index, short level, bool active);
short __stdcall function_1b5ee0(long actor_index, s_slot *slot, bool active);
bool __stdcall function_1b09b0(long actor_index, s_slot *slot);
bool __stdcall function_1b5f30(long actor_index, s_slot *slot);

// @retail 0x1b5a20
short __stdcall function_1b5a20(long actor_index)
{
	short result = 0;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_prop_view_fields *view = prop_node_view(node);

		if (view && node->unknown24 >= 3 && !view->unknown69)
			result = 3;
	}
	return result;
}

/* the state of slot group 0x22 */
struct s_slot_22
{
	s_slot_header header;
	bool unknown0c;
	byte unknown0d;
	short unknown0e;
	short unknown10;
	byte unknown12[2];
	long unknown14;
	long unknown18;
	byte unknown1c[0x40 - 0x1c];
};

/* the block of the actor's character tag function_1e4db0 returns */
struct s_character_db0_delays
{
	dword flags;
	real unknown4;
	real unknown8;
};

void *function_1e4db0(long actor_index);
bool function_26ba60(long prop_index, long actor_index, long clump_index);

// @retail 0x1b5c00
bool __stdcall function_1b5c00(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_22 *state = (s_slot_22 *)slot;
	real delay = 8.0f;
	s_character_db0_delays *character = (s_character_db0_delays *)function_1e4db0(actor_index);
	real ticks;
	long value;

	if (character)
		delay = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, character->unknown4, character->unknown8);
	ticks = g_510c54->field_2_3 * delay;
	__asm
	{
		fld ticks
		fistp value
	}
	state->unknown0e = (short)value;
	state->unknown14 = g_510c54->game_time;
	state->unknown0c = false;
	state->unknown18 = NONE;
	actor->unknown220 = true;
	actor->unknown221 = slot_type_enabled(0x25);
	state->unknown10 = NONE;
	actor->unknown222 = false;
	if (actor->prop_index != NONE)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_prop_view_fields *view = prop_node_view(node);

		if (view && view->unknown10 >= 0)
		{
			ticks = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, 2.0f, 4.0f) * g_510c54->field_2_3;
			__asm
			{
				fld ticks
				fistp value
			}
			state->unknown10 = (short)value;
			function_26ba60(node->unknown08, actor_index, actor->unknown07c);
		}
	}
	return true;
}

// @retail 0x1b5dd0
void __stdcall function_1b5dd0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	actor->unknown220 = false;
	actor->unknown221 = false;
}

// @retail 0x1b5ee0
short __stdcall function_1b5ee0(long actor_index, s_slot *slot, bool active)
{
	short result = g_46fbe8;

	if (function_1b5a20(actor_index) > 0)
		result = 0x22;
	else if (actor_get(actor_index)->unknown504 == 2)
		result = g_46fbe4;
	return result;
}

// @retail 0x1b5fe0
void __stdcall function_1b5fe0(long actor_index, s_slot *slot)
{
	actor_get(actor_index)->unknown484 = true;
}

/* the elements of g_50241c (0xc4 bytes) keep the last 5 references they
   were given */
struct s_50241c_element
{
	byte unknown00[0x3e];
	s_reference history[5];
	char history_index;
	char history_count;
	s_reference current;
	byte unknown58[0xc4 - 0x58];
};

s_reference g_471004 = {NONE, NONE};

inline s_50241c_element *element_50241c_get(long index)
{
	return (s_50241c_element *)(g_50241c->data + (index & 0xffff) * sizeof(s_50241c_element));
}

// @retail 0x1b6010
bool function_1b6010(long index)
{
	s_50241c_element *element = element_50241c_get(index);

	if (*(long *)&element->current != *(long *)&g_471004)
	{
		element->history[element->history_index] = element->current;
		element->current.unknown0 = NONE;
		element->current.unknown2 = NONE;
		element->history_index = (element->history_index + 1) % 5;
		element->history_count = element->history_count + 1 > 5 ? 5 : element->history_count + 1;
	}
	return true;
}

// @retail 0x1b6070
bool function_1b6070(long index, short unknown0, short unknown2)
{
	s_50241c_element *element = element_50241c_get(index);
	s_reference reference;
	bool result = false;

	reference.unknown0 = unknown0;
	reference.unknown2 = unknown2;
	for (short i = 0; i < element->history_count; i++)
	{
		if (*(long *)&reference == *(long *)&element->history[i])
			return true;
	}
	return result;
}

// @retail 0x1b60d0
bool function_1b60d0(long index, short unknown0, short unknown2)
{
	bool result = false;

	if (!function_1b6070(index, unknown0, unknown2))
	{
		s_50241c_element *element = element_50241c_get(index);

		element->current.unknown0 = unknown0;
		element->current.unknown2 = unknown2;
		return true;
	}
	return result;
}

/* the children of slot group 0x22 */
s_slot_child g_46fa50[5] =
{
	{0x23, 0, NONE, {0}, 0.0f, 0, 0},
	{0x28, 0, NONE, {0}, 0.0f, 0, 0},
	{0x24, 0, NONE, {0}, 0.0f, 0, 0},
	{0x25, 0, NONE, {0}, 0.0f, 0, 0},
	{0x26, 0, NONE, {0}, 0.0f, 0, 0},
};

s_slot_handler_1 g_47e5a8 =
{
	{
		0x22, 1, 0, -2, 0,
		function_1b5a20, function_1b5aa0, function_1b5c00, function_1b5dd0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	function_1b5e00, 5, g_46fa50
};

s_slot_handler_2 g_47e5f8 =
{
	{
		0x27, 2, 0, -2, 0,
		0, function_1b5ee0, function_1b09b0, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1b5f30, 0, function_1b5fe0
};

// @retail 0x1b5f30
bool __stdcall function_1b5f30(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	if (!actor->unknown50c && actor->unknown040)
	{
		byte *scratch = ai_scratch_buffer_get();
		s_2605d0_request request;
		bool unknown;

		request_initialize(&request);
		request.type = 8;

		s_reference reference = function_2605d0(actor_index, &request, 0, 0, scratch, &unknown);
		if (!REFERENCE_EQUAL(reference, g_470fa0))
			function_2626b0(actor_index, reference, NONE, scratch, unknown, true);
		ai_scratch_buffer_release(scratch);
	}

	return true;
}

short function_1a78a0(long actor_index, long argument, short level);
void function_25d420(long prop_ref_index, short type, long actor_index);

// @retail 0x1b5e00
short __stdcall function_1b5e00(long actor_index, short level, bool active)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;
	s_slot_22 *state = (s_slot_22 *)&actor->slots[level];

	if (actor->prop_index != NONE)
	{
		s_prop_view_fields *view = prop_node_view(prop_node_get(actor->prop_index));

		if (view && !view->unknown69)
		{
			if (state->unknown18 != actor->prop_index)
			{
				state->unknown18 = actor->prop_index;
				state->header.unknown4 = NONE;
			}
			if (view->unknown6a[2])
			{
				view->unknown6a[2] = 0;
				state->header.unknown4 = NONE;
			}
			/* The choice helper consumes the complete argument slot. */
			result = function_1a78a0(actor_index, *(long *)&active, level);
			if (result == g_46fbe4)
				function_25d420(actor->prop_index, 0, actor_index);
		}
	}
	return result;
}
