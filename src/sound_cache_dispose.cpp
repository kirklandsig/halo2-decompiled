// @flags /O2 /Ob1 /Gr
/* SOUND_CACHE_DISPOSE.CPP: the sound cache's dispose (part of retail's
   sound_cache_request_part.cpp, with the rest in src/unknown_218850.cpp). It is in a
   file of its own built /Ob1 because retail calls it from function_125600
   (0x125600) where LTCG inlines it from an /Ob2 file. */

#include "unknown_11c920.h"
#include "unknown_218850.h"
#include "physical_memory.h"
#include "data_array.h"

// @retail 0x2186b0
void function_2186b0(void)
{
	data_dispose(g_502104);
	((s_physical_object *)g_50210c)->allocator->deallocate(g_50210c);
	g_502108 = 0;
}
