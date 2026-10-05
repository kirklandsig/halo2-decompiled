// @flags /O2 /Ob1 /arch:SSE /Gr
/* whether a game is in progress (g_4e6948). Retail calls it out of line from
   every caller, so it has a file of its own built /Ob1, where LTCG doesn't
   inline it (it was in unknown_138800.cpp). */

#include "unknown_11c920.h"
#include "globals.h"

// @retail 0x138800
bool function_138800()
{
	bool result = false;
	if (g_4e6948 && g_4e6948->flag1120)
	{
		result = true;
	}
	return result;
}
