/* RECIPROCAL_NORMALIZE.H: normalizing a vector with the approximate reciprocal square root, which
   the particle code (src/unknown_2ba3e0.cpp) and the character physics (src/unknown_1e67a0.cpp)
   expand inline; the squared lengths are summed statement by statement, in the order retail keeps */

#ifndef RECIPROCAL_NORMALIZE_H
#define RECIPROCAL_NORMALIZE_H

#include "unknown_11c920.h"

/* the approximate reciprocal square root, read from and written to memory */
PRIVATE __forceinline real inverse_sqrt_2b96(real squared)
{
    real inverse;
    __asm
    {
        rsqrtss xmm0, squared
        movss inverse, xmm0
    }
    return inverse;
}

PRIVATE __forceinline real particle_vector_squared_2b(vector3f const *v)
{
    real squared = v->k * v->k;
    squared += v->i * v->i;
    squared += v->j * v->j;
    return squared;
}

PRIVATE __forceinline void scale_particle_vector_2b(real scale, vector3f *v)
{
    v->i = scale * v->i;
    v->j = v->j * scale;
    v->k = v->k * scale;
}

PRIVATE __forceinline void normalize_particle_vector_2b(vector3f *v)
{
    real squared = particle_vector_squared_2b(v);
    if (squared != 0.0f)
    {
        real inverse = inverse_sqrt_2b96(squared);
        scale_particle_vector_2b(inverse, v);
    }
}

PRIVATE __forceinline real particle_direction_squared_2b(vector3f const *v)
{
    real squared = v->k * v->k;
    squared += v->j * v->j;
    squared += v->i * v->i;
    return squared;
}

PRIVATE __forceinline void normalize_particle_direction_2b(vector3f *v)
{
    real squared = particle_direction_squared_2b(v);
    if (squared != 0.0f)
    {
        real inverse = inverse_sqrt_2b96(squared);
        scale_particle_vector_2b(inverse, v);
    }
}

#endif
