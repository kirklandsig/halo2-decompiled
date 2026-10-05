#ifndef __UNKNOWN_1E46C0_H__
#define __UNKNOWN_1E46C0_H__

/* UNKNOWN_1E46C0.H: iterating the actors (g_4f55f0); function_1e46c0 is
   0x1e46c0 (unknown_1e46c0.cpp), function_x66da2b is inlined everywhere */

#include "unknown_11c920.h"
#include "data_array.h"
#include "globals.h"

struct s_actor_iterator
{
	void *actor;
	s_record_pool_iterator iterator;
	bool active_only;
	long actor_index;
};

static inline void function_x66da2b(s_actor_iterator *iterator, bool active_only)
{
	if (g_4f55d0->active)
	{
		iterator->iterator.data = g_4f55f0;
		iterator->iterator.index = NONE;
		iterator->iterator.datum_index = NONE;
		iterator->active_only = active_only;
	}
}

void *function_1e46c0(s_actor_iterator *iterator);

#endif
