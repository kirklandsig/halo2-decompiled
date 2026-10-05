#include "unknown_11c920.h"

// @flags /O2 /Gr

struct s_havok_component;

struct s_physics_vector_pair
{
	real values[6];
};

struct s_physics_vector_source
{
	byte unknown00[0x3c];
	dword flags;
	byte unknown40[8];
	s_physics_vector_pair vectors;
};

struct s_physics_vector_block
{
	byte unknown00[0x4c];
	long count;
	s_physics_vector_source *data;
};

// @retail 0x205510
bool function_205510(void *buffer, void const *definition_physics, s_havok_component *component)
{
	bool result = false;
	s_physics_vector_block const *block = (s_physics_vector_block const *)definition_physics;
	if (block->count)
	{
		s_physics_vector_source *source = block->data;
		if (source->flags & 1)
		{
			*(s_physics_vector_pair *)buffer = source->vectors;
			result = true;
		}
	}
	return result;
}
