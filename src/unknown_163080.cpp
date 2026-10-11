// @flags /O2 /Gr
/* UNKNOWN_163080.CPP: whether the game engine flag at +0xc10 of the
   multiplayer globals is set. Decompiled by lane R for the effects
   (0x175fa0). */

#include "unknown_11c920.h"
#include "globals.h"

struct s_163080_globals
{
	byte unknown00[0xc10];
	byte flag0 : 1;
	byte : 7;
};

__declspec(noinline) bool function_163080(void);

// @retail 0x163080
bool function_163080(void)
{
	bool result = false;

	if (g_55e4d0[g_4e9ae8->engine_index])
		result = ((s_163080_globals *)g_4e9ae8)->flag0;
	return result;
}
