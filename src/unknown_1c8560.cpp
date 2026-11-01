#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "data_array.h"
#include "data_iterator.h"
// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_1c8560
{
	short field_0;
	byte field_2[2];
	long field_4;
	byte field_8[0xc];
	short field_14;
};

extern s_record_pool *g_5044c8;
void function_1e2a90(long actor_index);
void __stdcall function_b83b0(long object_index, bool unused);
void __stdcall function_b8460(long object_index, bool detach);
void __stdcall swarm_delete(long arg_0);

/* walks the swarms of g_5044c8, only while the ai is active */
struct s_swarm_datum_walk
{
	s_data_datum_iterator iterator;
	long walk_datum;
};

static inline void swarm_datum_walk_new(s_swarm_datum_walk *walk)
{
	if (g_4f55d0->active)
	{
		walk->iterator.data = g_5044c8;
		walk->iterator.index = NONE;
		walk->iterator.datum_index = NONE;
	}
}

/* the next swarm while the ai is active, or NULL */
PRIVATE __forceinline s_1c8560 *swarm_datum_walk_next(s_swarm_datum_walk *walk, bool active)
{
	s_1c8560 *swarm = NULL;

	if (active)
	{
		if (data_datum_iterator_next_inlined(&walk->iterator))
			swarm = (s_1c8560 *)walk->iterator.datum;
		walk->walk_datum = walk->iterator.datum_index;
	}
	return swarm;
}

// @retail 0x1c8560
long __stdcall function_1c8560(long arg_0, void *arg_1, long arg_2, bool *arg_3, void *arg_4, long arg_5)
{
	long local_0 = 0;
	s_swarm_datum_walk swarms;
	s_1c8560 *swarm;

	swarm_datum_walk_new(&swarms);
	while ((swarm = swarm_datum_walk_next(&swarms, g_4f55d0->active)) != NULL)
	{
		long local_7 = swarm->field_4;
		if (local_7 != NONE)
		{
			s_actor_view *actor = actor_get(local_7);
			if (actor->unknown009 || *(long *)((byte *)actor + 0x10) == NONE)
				continue;
		}
		local_0 += swarm->field_14;
		if (local_7 != NONE)
		{
			s_actor_view *local_4 = actor_get(local_7);
			long local_5 = local_4->unknown018;
			long local_6 = *(long *)((byte *)local_4 + 0x1c);
			function_1e2a90(local_7);
			if (local_5 != NONE)
			{
				function_b83b0(local_5, true);
				function_b8460(local_5, true);
			}
			else if (local_6 != NONE)
				swarm_delete(local_6);
		}
		else
			swarm_delete(swarms.walk_datum);
	}
	*arg_3 = false;
	return local_0;
}

