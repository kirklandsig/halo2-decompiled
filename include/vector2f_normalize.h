/* VECTOR2F_NORMALIZE.H: a 2-D vector scaled, and made unit length through
   that scale; src/unknown_1f5560.cpp and src/unknown_1ff010.cpp share them,
   so they live here */

#ifndef VECTOR2F_NORMALIZE_H
#define VECTOR2F_NORMALIZE_H

#include "unknown_11cc90.h"

/* a planar vector times a scale, into out (which may be the vector itself); the vector counterpart of the
   point pair in src/unknown_0a76b0.cpp */
static inline void planar_scale(vector2f const *vector, real scale, vector2f *out)
{
	out->i = scale * vector->i;
	out->j = scale * vector->j;
}

/* makes a planar vector unit length and returns the length it had, or 0 for one too short to scale */
static inline real planar_normalize(vector2f *vector)
{
	real magnitude = magnitude2d(vector);

	if (fabs(magnitude) < 0.0001f)
		return 0.0f;
	planar_scale(vector, 1.0f / magnitude, vector);
	return magnitude;
}

#endif
