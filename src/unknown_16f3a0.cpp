// @flags /O2 /Gr
/* UNKNOWN_16F3A0.CPP: the state of a local player. Decompiled by lane R for
   the effects (0x176210). */

#include "unknown_11c920.h"
#include "globals.h"

__declspec(noinline) s_player_state *function_16f3a0(long index);

// @retail 0x16f3a0
s_player_state *function_16f3a0(long index)
{
	s_player_state *result = 0;

	if (index != NONE && g_4686c4 != NONE)
		result = &g_4e9bd4[index].state;
	return result;
}
