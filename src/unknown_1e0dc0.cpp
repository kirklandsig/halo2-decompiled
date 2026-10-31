// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1E0DC0.CPP: creating an actor and sending its unit a request
   (built with SSE, apart from its neighbours in
   unknown_1e1f20.cpp) */

#include "unknown_11c920.h"
#include "ai_actor.h"
#include "unknown_2551c0.h"
#include "object_markers.h"
#include "object_queries.h"
#include "unknown_19ec40.h"
#include "unknown_1e46c0.h"
#include <string.h>
#include "unit_requests.h"

extern bool g_4f55e8;
void function_caf00(long unit_index);
long function_1e4a10(long tag_index);
void function_d5c90(long object_index, real *body_vitality, real *shield_vitality);
bool function_1df5d0(short first, short second);
bool function_1e11b0(long actor_index, bool active);
bool unit_submit_entry_request(short fallback_index, short entry_index, long unit_index, long priority, void const *extra);
long __stdcall function_1e0160(long squad_index, long entry_index, long unit_index, bool flag);

void function_1e04a0(long primary_tag, long character_index, long unit_index, long secondary_tag, short grenade_type);

// score 2 (was 2): retail writes the request's fields in the order type (0x31),
// arguments[0] and arguments[2], ours writes arguments[0], type and
// arguments[2]. Retail's time-field load precedes the type store, so the
// product is evaluated before it. Tried: all six store orders 2-15 (type
// first leaves the field load below the type store); a named short, long or
// word duration local in each order 2-25 (it takes AX from the dying time
// pointer and its store moves above `mov eax, ebx`); named duration and
// request-value locals 20. Round 11 (lane 17): type first 6 (the type store is scheduled above
// the time-field load); a short duration local before the type store, at block scope, before the
// request or at function scope 20/20/20 (the product takes AX and its store is hoisted above the
// type store; the e6900 result path then changes registers); the product written `3 * field` 2.
// A typed union member for the two arguments instead of the casts: our order 2, type first 6.
// The one-file harness is not exact here (7: callee conventions).
// @retail 0x1e0dc0
long function_1e0dc0(short team, long unit_index, short request_value, short squad_index)
{
	long result = NONE;
	const long *unit_reference = &unit_index;
	const short *request_reference = &request_value;
	const short *squad_reference = &squad_index;
	byte *unit = (byte *)ai_object_get(*unit_reference);
	function_caf00(*unit_reference);
	function_1e04a0(NONE, *(long *)(unit + 0x3d8), *unit_reference, NONE, NONE);
	long definition_index;
	if (*(long *)(unit + 0x3d8) != NONE &&
		(*(short *)(unit + 0x138) == team || function_1df5d0(*(short *)(unit + 0x138), team)))
		definition_index = *(long *)(unit + 0x3d8);
	else
		definition_index = *(long *)(g_4e3b44[*(long *)unit & 0xffff].bytes + 0x304);
	if (definition_index != NONE)
	{
		result = function_1e0160(*squad_reference, definition_index, *unit_reference, false);
		if (result != NONE)
		{
			if (*squad_reference == NONE)
				function_1e11b0(result, true);
			s_unit_request request;
			*(short *)(request.arguments + 0) = g_510c54->field_2_3 * 3;
			request.type = 0x31;
			*(short *)(request.arguments + 2) = *request_reference;
			if (function_e6900(*unit_reference, &request))
				unit_submit_entry_request(0x15, NONE, actor_get(result)->unknown018, 0xd, NULL);
		}
	}
	return result;
}
