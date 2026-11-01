// @flags /O2 /Ob1 /arch:SSE /Gr /GL-
/* UNKNOWN_067F60.CPP: the simulation watcher's teardown: it detaches the
   watcher from its network observer, clears its session and world, and
   disposes of the simulation world.
   Retail built it without LTCG: the watcher copy (`mov edx, eax`) is taken
   from the global's second load before the observer slot's address is
   formed (`add eax, 8`), and the clears store immediate zeros. That is what
   VC gives when the two loads of g_4cf780 are merged only after code
   generation, as for a global outside the LTCG image; with /GL the loads merge
   first, the slot's address folds into the stores and a zero register takes
   their place (55 bytes against 73, in src/unknown_067e10.cpp). */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_067e10.h"

extern bool g_4ed39c;
void function_195e90(void);
void __stdcall function_68e20(c_class_6a600 *world);

// @retail 0x67f60
void function_67f60(void)
{
	if (!g_4ed39c)
	{
		s_simulation_world_owner *watcher = (s_simulation_world_owner *)g_4cf780;
		byte **link = (byte **)((s_simulation_world_owner *)g_4cf780)->unknown08;
		if (*link)
		{
			*(long *)(*link + 0x80) = 0;
			*link = 0;
		}
		watcher->session = 0;
		watcher->world = 0;
		function_68e20((c_class_6a600 *)g_4cf77c);
		function_195e90();
	}
}
