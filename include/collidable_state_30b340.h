#ifndef COLLIDABLE_STATE_30B340_H
#define COLLIDABLE_STATE_30B340_H
// Shared shape record, independently reconstructed from its retail callers.
#include "reference_counted_2d88a0.h"

// the shape, a key (-1 when built) and two words
struct body_30b3a0
{
	referenced_2d88a0 *shape;
	long key;
	long field_8;
	long field_c;

	body_30b3a0(referenced_2d88a0 *shape_) : shape(shape_), field_8(0)
	{
		field_c = 0;
		key = -1;
	}
};

#endif
