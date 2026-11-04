/* VERTEX_INPUTS.H: the vertex input search the immediate-mode surface drawers expand inline */

#ifndef VERTEX_INPUTS_H
#define VERTEX_INPUTS_H

#include "unknown_11c920.h"

/* the register index of the first input of the given type in a vertex input declaration
   (input count at +4, the inputs' type words at +8), or NONE */
static __inline long vertex_input_index(byte const *declaration, short type)
{
	long result = NONE;
	for (long i = 0; i < *(long const *)(declaration + 4); ++i)
	{
		if ((*(short const *const *)(declaration + 8))[i] == type)
		{
			result = i;
			break;
		}
	}
	return result;
}

#endif
