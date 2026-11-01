// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_2605d0.h"
#include "unknown_2626b0.h"

/* slot type 0x6b, and the evaluate callback many handlers share */

short __stdcall function_1bcc90(long actor_index);
bool __stdcall function_1bcd00(long actor_index, s_slot *slot);
real function_1f8940(long actor_index);

/* iterates the squads of an encounter (function_204ec0 and function_205010) */
struct s_squad_iterator
{
	byte unknown00[0x14];
};

void function_204ec0(s_squad_iterator *iterator, short encounter_index, short a, short b);
short function_205010(s_squad_iterator *iterator);

// @retail 0x1bcc90
short __stdcall function_1bcc90(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;
	s_squad_iterator iterator;

	function_204ec0(&iterator, (short)actor->unknown030, 5, actor->unknown26c != NONE);
	if (function_205010(&iterator) != NONE)
		result = 1;
	return result;
}

// @retail 0x1bcd00
bool __stdcall function_1bcd00(long actor_index, s_slot *slot)
{
	bool result = true;
	s_actor_view *actor = actor_get(actor_index);

	if (actor_get(actor_index)->unknown040)
	{
		byte *scratch;
		s_2605d0_request request;
		long other_index;
		bool unknown;

		if (!REFERENCE_EQUAL(actor->unknown418, g_470fa0) &&
			(actor->unknown504 == 2 || (actor->unknown225 ? 1.2f : 0.6f) > function_1f8940(actor_index)))
		{
			function_262800(actor_index, actor->unknown418, false);
		}

		memset(&request, 0, sizeof(request));
		request.type = 4;
		request.unknown015 = true;
		request.unknown05b = true;
		request.unknown69c = actor->unknown328 <= 2;
		scratch = ai_scratch_buffer_get();
		s_reference reference = function_2605d0(actor_index, &request, 0, (long)&other_index, scratch, &unknown);
		if (REFERENCE_EQUAL(reference, g_470fa0))
		{
			actor_get(actor_index)->unknown040 = false;
			result = false;
		}
		else
		{
			s_reference found_reference = reference;

			reference = function_2626b0(actor_index, reference, other_index, scratch, unknown, true);
			if (REFERENCE_EQUAL(reference, g_470fa0) && (actor->unknown5b4 > 4 || actor->unknown26c != NONE))
			{
				function_262800(actor_index, found_reference, false);
			}
		}
		ai_scratch_buffer_release(scratch);
	}
	return result;
}

// @retail 0x1bce80
void __stdcall function_1bce80(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	actor->unknown4ae = true;
	actor->unknown484 = true;
	actor->unknown485 = true;
	if (actor->unknown26c != NONE)
		actor->unknown4b4 = 0.6f;
}

// @retail 0x1bced0
short __stdcall function_1bced0(long actor_index, s_slot *slot, bool active)
{
	return g_46fbe8;
}

s_slot_handler_2 g_47eb58 =
{
	{
		0x6b, 2, 0xfff, -2, 0,
		function_1bcc90, function_1bced0, slot_start_true, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1bcd00, 0, function_1bce80
};
