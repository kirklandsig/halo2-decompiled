// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0A58D0.CPP: the simulation entity database: an entity's object */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0a58d0.h"
#include "unknown_xa19f52.h"

/* the entity an index stands for, or none when its salt is stale; retail
   has no copy of its own (LTCG inlines it everywhere), and inlining it from
   here keeps the null test after the identifier comparison that retail has */
s_simulation_entity *simulation_entity_try_get(s_simulation_entity_table *table, long entity_index)
{
	s_simulation_entity *result = 0;
	long absolute_index = entity_index & 0x3ff;
	if (table->entities[absolute_index].identifier == entity_index)
		result = table->entities + absolute_index;
	return result;
}

/* 0xa58d0, kept out of the build: retail takes the index in ecx (the
   __fastcall the stub in src/stubs/unknown_09a9f0.cpp has), our LTCG passes it
   in eax or edx (it depends on the body), which breaks the matched caller
   0xa3a20. With simulation_entity_try_get as it is now, the body is otherwise
   the same as retail's (the null test after the identifier comparison
   included). */
#if 0
long function_a58d0(long entity_index)
{
	long object_index = NONE;
	if (entity_index != NONE)
	{
		s_simulation_world_view *world = (s_simulation_world_view *)g_4cf77c;
		s_simulation_entity *entity = simulation_entity_try_get(&world->database->table, entity_index);
		if (entity)
		{
			s_entity_definitions *definitions = (s_entity_definitions *)g_4cf784;
			if (definitions->definitions[entity->type]->v7((s_entity *)entity) && entity->object_index != NONE)
				object_index = entity->object_index;
		}
	}
	return object_index;
}
#endif
