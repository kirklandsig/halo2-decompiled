// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_256070.CPP: slot handler 0x70 (handler at 0x47f808) */

#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_2551c0.h"

/* the slot state of handler 0x70 */
struct s_slot_70_state
{
	s_slot_header header;
	short timer;
	byte unknown0e[0x40 - 0xe];
};

short g_470b58 = -2;
short g_470b5c = -3;

short function_1a6fe0(long owner_index, short type);

short __stdcall function_256070(long actor_index);
short __stdcall function_256210(long actor_index, s_slot *slot, bool active);
bool __stdcall function_256150(long actor_index, s_slot *slot);

s_slot_handler_2 g_47f808 =
{
	{
		0x70, 2, 0x7ff, -2, 0,
		function_256070, function_256210, function_256150, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	0, 0, 0
};

PRIVATE __forceinline s_actor_view *function_256071(long arg_0)
{
	return (s_actor_view *)(g_4f55f0->data + (arg_0 & 0xffff) * sizeof(s_actor_view));
}

// @retail 0x256070
short __stdcall function_256070(long actor_index)
{
	short result = 0;
	short count = 0;
	short ready_count = 0;
	s_actor_view *actor = function_256071(actor_index);

	if (actor->unknown07c != NONE)
	{
		long member_index = element_502420_get(actor->unknown07c)->first_actor_index;

		while (member_index != NONE)
		{
			s_actor_view *member = function_256071(member_index);
			long current_index = member_index;
			member_index = member->next_index;

			if (*(volatile short *)&actor->unknown004 == member->unknown004)
			{
				count++;
				if (function_1a6fe0(current_index, 0x1b) != NONE)
				{
					ready_count++;
				}
			}
		}

		if ((real)ready_count / (real)count > 0.6f)
		{
			result = 3;
		}
	}

	return result;
}

// @retail 0x256150
bool __stdcall function_256150(long actor_index, s_slot *slot)
{
	bool local_0 = false;
	s_actor_view *actor = actor_get(actor_index);
	s_slot_70_state *state = (s_slot_70_state *)slot;

	if (actor->unknown07c != NONE)
	{
	state->timer = 0;

	long member_index = element_502420_get(actor->unknown07c)->first_actor_index;
	while (member_index != NONE)
	{
		s_actor_view *member = actor_get(member_index);
		long current_index = member_index;
		member_index = member->next_index;

		if (member != actor)
		{
			short slot_index = function_1a6fe0(current_index, 0x70);

			if (slot_index >= 0 && slot_index <= member->current)
			{
				s_slot_70_state *other = (s_slot_70_state *)&member->slots[slot_index];
				if (other->timer > state->timer)
				{
					state->timer = other->timer;
				}
			}
		}
	}

		local_0 = true;
	}
	return local_0;
}

// @retail 0x256210
short __stdcall function_256210(long actor_index, s_slot *slot, bool active)
{
	s_slot_70_state *state = (s_slot_70_state *)slot;
	real ticks;
	long rounded;

	short result = g_470b58;

	state->timer++;

	ticks = g_510c54->field_2_3 * 1.5f;
	__asm
	{
		fld ticks
		fistp rounded
	}

	if (state->timer > rounded)
	{
		result = g_470b5c;
	}
	return result;
}
