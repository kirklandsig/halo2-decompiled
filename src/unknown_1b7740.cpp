// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"

/* slot type 0x80 */

struct s_slot_80
{
	s_slot_header header;
	short ticks;
	byte unknown0e[2];
	vector3f vector;
	byte unknown1c[0x40 - 0x1c];
};

/* the bounds of the actor's tag (function_1e4a10) */
struct s_bounds_view
{
	byte unknown00[0x54];
	real lower;
	real upper;
};

long function_1e4a10(long index);

// @retail 0x1b7740
short __stdcall function_1b7740(long actor_index)
{
	short result = 0;

	if (actor_get(actor_index)->unknown3b8)
		result = 3;
	return result;
}

// @retail 0x1b7770
bool __stdcall function_1b7770(long actor_index, s_slot *slot)
{
	bool result = false;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown3b8)
	{
		s_bounds_view *bounds = (s_bounds_view *)function_1e4a10(actor->unknown054);

		if (bounds->upper > 0.0f)
		{
			s_slot_80 *state = (s_slot_80 *)slot;
			double local_0 = (double)bounds->upper - bounds->lower;
			real local_1 = slot_random();
			double local_2 = local_0 * local_1;
			real local_3 = (real)(bounds->lower + local_2 * actor->unknown3bc);
			real ticks = local_3 * g_510c54->field_2_3;
			long rounded;

			__asm
			{
				fld ticks
				fistp rounded
			}
			state->ticks = (short)rounded;
			state->vector = actor->unknown3c0;
			result = true;
		}
	}
	return result;
}

// @retail 0x1b7850
void __stdcall function_1b7850(long actor_index, s_slot *slot)
{
	s_slot_80 *state = (s_slot_80 *)slot;

	state->ticks--;
}

// @retail 0x1b7860
void __stdcall function_1b7860(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_80 *state = (s_slot_80 *)slot;

	actor->unknown450 = 0x70000c9;
	if (dot3f(&state->vector, &actor->unknown290) > 0.1)
	{
		actor->unknown456 = true;
		actor->unknown458 = state->vector;
	}
	actor->unknown488 = false;
	actor->unknown41c = 2;
	actor->unknown420 = 0;
}

// @retail 0x1b7900
short __stdcall function_1b7900(long actor_index, s_slot *slot, bool active)
{
	long result = (long)g_46fbe8;
	s_slot_80 *state = (s_slot_80 *)slot;

	if (state->ticks <= 0)
		result = (long)g_46fbe4;
	return result;
}

s_slot_handler_2 g_47e7f8 =
{
	{
		0x80, 2, 0, -2, 0,
		function_1b7740, function_1b7900, function_1b7770, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	0, function_1b7850, function_1b7860
};
