/* ENGINE_OBJECT_DEFINITION.H: the multiplayer globals tag and the definitions
   of the objects the engines create from it (src/unknown_2420a0.cpp and
   src/unknown_2bd960.cpp) */

#ifndef ENGINE_OBJECT_DEFINITION_H
#define ENGINE_OBJECT_DEFINITION_H

#include "unknown_11c920.h"
#include "globals.h"

/* the engine's constants in the multiplayer globals tag */
struct s_ctf_constants
{
	byte unknown00[0xc4];
	long lc4;
	byte unknownc8[0xd8 - 0xc8];
	real pickup_radius;
};

struct s_mp_globals_block
{
	byte unknown00[4];
	long flag_definition_index;
	byte unknown08[0x0c - 0x08];
	long held_item_definition_index;
	byte unknown10[0x3c - 0x10];
	long bomb_definition_index;
	byte unknown40[0x534 - 0x40];
	s_ctf_constants *constants;
};

struct s_mp_globals_tag
{
	byte unknown00[0xc];
	s_mp_globals_block *block;
};

static inline s_mp_globals_block *mp_globals_block()
{
	long index = g_4e034c->index;

	return ((s_mp_globals_tag *)g_4e3b44[index & 0xffff].bytes)->block;
}

enum e_engine_object
{
	_engine_object_flag,
	_engine_object_bomb,
	_engine_object_held_item
};

/* the definition of one of the engine's objects, NONE without an engine */
static inline long engine_object_definition(long object)
{
	long definition_index = NONE;

	if (g_55e4d0[g_4e9ae8->engine_index])
	{
		s_mp_globals_block *block = mp_globals_block();

		switch (object)
		{
		case _engine_object_flag:
			definition_index = block->flag_definition_index;
			break;
		case _engine_object_bomb:
			definition_index = block->bomb_definition_index;
			break;
		case _engine_object_held_item:
			definition_index = block->held_item_definition_index;
			break;
		}
	}

	return definition_index;
}

#endif
