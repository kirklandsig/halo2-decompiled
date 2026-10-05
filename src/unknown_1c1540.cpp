// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259a0.h"
#include <string.h>

/* slot handler 0x15 (g_47efa8) */

struct s_slot_15
{
	s_slot_header header;
	long prop_index;
	bool unknown10;
	bool unknown11;
	byte unknown12[0x40 - 0x12];
};

/* the state of a slot of handler 0x82 (unknown_1c10a0.cpp) */
struct s_slot_82_view
{
	s_slot_header header;
	byte unknown0c[4];
	long unknown10;
};

/* the prop state as these callbacks read it: the position at +4 */
struct s_prop_state_point_15
{
	long unknown00;
	point3f position;
};

short function_1a6fe0(long owner_index, short type);
void function_1f86a0(long index);

// @retail 0x1c1540
void function_1c1540(long actor_index, long value)
{
	s_actor_view *actor = actor_get(actor_index);
	long level = function_1a6fe0(actor_index, 0x82);

	if (level != NONE)
	{
		((s_slot_82_view *)&actor->slots[level])->unknown10 = value;
	}
	actor->unknown22a = false;
}

// @retail 0x1c1590
short __stdcall function_1c1590(long actor_index)
{
	short result = 0;

	if (actor_get(actor_index)->unknown223)
	{
		result = 3;
	}
	return result;
}

// @retail 0x1c15c0
bool __stdcall function_1c15c0(long actor_index, s_slot *slot)
{
	s_slot_15 *state = (s_slot_15 *)slot;
	s_actor_view *actor = actor_get(actor_index);

	actor_reset_state(actor_index);
	state->unknown10 = false;
	state->prop_index = actor->prop_index;
	actor->unknown354 = true;
	function_265cb0(actor_index);
	return true;
}

// @retail 0x1c1640
void __stdcall function_1c1640(long actor_index, s_slot *slot)
{
	actor_get(actor_index)->unknown354 = false;
}

// @retail 0x1c1670
short __stdcall function_1c1670(long actor_index, s_slot *slot, bool active)
{
	s_slot_15 *state = (s_slot_15 *)slot;
	short result = g_46fbe8;
	s_actor_view *actor = actor_get(actor_index);
	s_prop_node_view *node;

	if (!actor->unknown223)
	{
		return g_46fbe4;
	}
	node = prop_node_get(actor->prop_index);
	if (actor->prop_index != state->prop_index)
	{
		state->prop_index = actor->prop_index;
		state->unknown10 = false;
		state->unknown11 = false;
		function_1f86a0(actor_index);
	}
	if (state->unknown10)
	{
		if (node->unknown28 > 3.75f)
		{
			function_1f86a0(actor_index);
			state->unknown10 = false;
			state->unknown11 = false;
		}
	}
	else if (2.5f > node->unknown28)
	{
		function_1f86a0(actor_index);
	}
	return result;
}

// @retail 0x1c1730
bool __stdcall function_1c1730(long actor_index, s_slot *slot)
{
	s_slot_15 *state = (s_slot_15 *)slot;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown040)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);

		if (node->unknown28 > 5.0f)
		{
			if (function_1f4810(actor_index, actor->prop_index, 0.0f, 0))
			{
				actor->unknown4cc = 2.5f;
			}
		}
		else if (state->unknown10 || 1.75f > node->unknown28)
		{
			byte *buffer = ai_scratch_buffer_get();
			s_prop_search search;
			long prop_index;
			long a;
			bool b;
			s_261d20_entry entry;
			s_reference reference;

			memset(&search, 0, sizeof(search));
			search.type = 0;
			for (prop_index = actor_get(actor_index)->first_prop_index; prop_index != NONE; )
			{
				s_prop_node_view *prop = prop_node_get(prop_index);

				prop_index = prop->next_index;
				if (search.point_count >= 32)
				{
					break;
				}
				if (prop->type == 1)
				{
					s_prop_state_point_15 *s_type_5cfb45 = (s_prop_state_point_15 *)prop_node_state(prop);

					search.points[search.point_count].position = s_type_5cfb45->position;
					search.points[search.point_count].weight = 3.75f;
					search.point_count++;
				}
			}
			reference = function_261280(&search, actor_index, &entry, &a, buffer, &b);
			if (*(long *)&reference != *(long *)&g_470fa0)
			{
				function_2626b0(actor_index, reference, a, buffer, b, true);
			}
			state->unknown10 = true;
			ai_scratch_buffer_release(buffer);
		}
	}
	return true;
}

// @retail 0x1c18f0
void __stdcall function_1c18f0(long actor_index, s_slot *slot)
{
	s_slot_15 *state = (s_slot_15 *)slot;
	s_actor_view *actor = actor_get(actor_index);
	s_prop_node_view *node = prop_node_get(actor->prop_index);

	actor->unknown41c = 4;
	actor->unknown420 = 2;
	if (!state->unknown11 && (!state->unknown10 || !(1.25f > node->unknown28)))
	{
		if (2.5f > node->unknown28)
		{
			actor->unknown44a = true;
		}
		actor->unknown483 = true;
		actor->unknown449 = true;
	}
	else
	{
		state->unknown11 = true;
		actor->unknown449 = true;
	}
}

// @retail 0x1c1990
void __stdcall function_1c1990(long actor_index, s_slot *slot, long index)
{
	s_slot_15 *state = (s_slot_15 *)slot;

	if (state->prop_index == index)
	{
		state->prop_index = NONE;
	}
}

s_slot_handler_2 g_47efa8 =
{
	{
		0x15, 2, 0, -2, 0,
		function_1c1590, function_1c1670, function_1c15c0, function_1c1640, NONE, {0},
		0, function_1c1990, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1c1730, slot_proc_nothing, function_1c18f0
};