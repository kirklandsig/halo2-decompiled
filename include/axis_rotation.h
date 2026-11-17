#pragma once

/* a vector turned about a unit axis, as the game's callers inline it (0x2551f0, 0x117dd0) */

#include "unknown_0259d0.h"

/* turns a vector about a unit axis by the angle whose sine and cosine are
   given */
static inline void function_x84d9e8(vector3f *vector, vector3f const *axis, real sine, real cosine)
{
	real vector_k = vector->k;
	real projection = (vector->i * axis->i + vector->j * axis->j + vector_k * axis->k) * (1.f - cosine);
	real cross_i = vector->j * axis->k - vector_k * axis->j;
	real cross_j = vector_k * axis->i - vector->i * axis->k;
	real cross_k = vector->i * axis->j - vector->j * axis->i;
	vector->i = vector->i * cosine + projection * axis->i - cross_i * sine;
	vector->j = vector->j * cosine + projection * axis->j - cross_j * sine;
	vector->k = vector_k * cosine + projection * axis->k - cross_k * sine;
}
