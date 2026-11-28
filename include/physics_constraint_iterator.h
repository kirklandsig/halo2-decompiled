#ifndef PHYSICS_CONSTRAINT_ITERATOR_H
#define PHYSICS_CONSTRAINT_ITERATOR_H

#include "unknown_11c920.h"

/* walks the constraints of a physics model, of six kinds */
struct s_physics_constraint_iterator
{
	byte *physics;
	short type;
	short index;
};

struct s_physics_constraint_block
{
	long count;
	byte *elements;
};

void function_1eb110(s_physics_constraint_iterator *iterator);
void function_1eb160(s_physics_constraint_iterator *iterator);
struct s_physics_model_shape_key;
struct s_impact_tag_block;
s_impact_tag_block *physics_model_shape_block_get(byte *physics_model, s_physics_model_shape_key const *key, long *element_size);

/* the constraint the iterator stands on, or NULL past the last one of its kind */
PRIVATE inline byte *damage_constraint_element(s_physics_constraint_iterator *iterator, long *element_size)
{
	s_physics_constraint_block *block = (s_physics_constraint_block *)physics_model_shape_block_get(iterator->physics,
		(s_physics_model_shape_key const *)&iterator->type, element_size);

	if (iterator->index >= block->count)
		return NULL;
	return block->elements + iterator->index * *element_size;
}

#endif
