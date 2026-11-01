// @flags /O2 /Gr /arch:SSE
#include "unknown_2551c0.h"
#include "globals.h"
#include "swarm.h"

struct s_28dab1
{
	byte field_0[0x18];
	long field_18;
};

struct s_28dab2
{
	byte field_0[0x2e];
	short field_2e;
};

long function_1e4990(long arg_0);
long function_28dbd0(long arg_0, long arg_1, long arg_2, s_28dab2 const *arg_3, bool arg_4);
void __stdcall swarm_delete(long arg_0);

// creates a swarm for a spawn definition: allocates and clears the record, then places up to field_2e
// creatures (10 when it is not positive) and deletes the swarm again when none were placed
// @retail 0x28dab0
long __stdcall function_28dab0(long arg_0, s_28dab2 const *arg_1)
{
	long local_0 = NONE;
	s_28dab1 *local_1 = (s_28dab1 *)g_4e3b44[arg_0 & 0xffff].bytes;
	if ((*(byte *)function_1e4990(arg_0) & 1) && local_1->field_18 != NONE)
	{
		local_0 = record_pool_allocate(g_5044c8);
		if (local_0 != NONE)
		{
			s_swarm *local_3 = swarm_get(local_0);
			short local_4 = 10;
			long local_2 = local_1->field_18;
			local_3->actor_index = NONE;
			local_3->first_creature_index = NONE;
			local_3->creature_count = 0;
			local_3->initial_creature_count = 0;
			local_3->position = *g_468788;
			local_3->event_count = 0;
			local_3->gather_position = *g_468788;
			local_3->gather_position_valid = false;
			local_3->spawn_slot_reference = true;
			if (arg_1->field_2e > 0)
				local_4 = arg_1->field_2e;
			for (short local_5 = 0; local_5 < local_4; local_5++)
				function_28dbd0(local_0, arg_0, local_2, arg_1, local_5 == 0);
			if (local_3->creature_count <= 0)
			{
				swarm_delete(local_0);
				local_0 = NONE;
			}
			else
			{
				local_3->initial_creature_count = local_3->creature_count;
			}
		}
	}
	return local_0;
}
