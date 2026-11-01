/* VECTOR_PRODUCTS.H: inline vector products and sums shared by several files */

#ifndef VECTOR_PRODUCTS_H
#define VECTOR_PRODUCTS_H

#include "unknown_0259d0.h"

/* the cross product a x b */
static inline void cross3f(vector3f const *a, vector3f const *b, vector3f *result)
{
	result->i = a->j * b->k - a->k * b->j;
	result->j = a->k * b->i - a->i * b->k;
	result->k = a->i * b->j - a->j * b->i;
}

/* the cross product a x b, every component computed before the stores, so
   result may be either input */
static inline void cross3f_staged(vector3f const *a, vector3f const *b, vector3f *result)
{
	real k = a->i * b->j - a->j * b->i;
	real j = a->k * b->i - a->i * b->k;
	real i = a->j * b->k - a->k * b->j;

	result->i = i;
	result->j = j;
	result->k = k;
}

/* input scaled by a double factor, each component rounded back to real;
   input and output may be the same vector */
static inline vector3f *scale_vector_by_double(vector3f const *input, double scale, vector3f *output)
{
	output->i = scale * input->i;
	output->j = scale * input->j;
	output->k = scale * input->k;
	return output;
}

/* the vector expressed in the axes of a matrix: forward, left and up scaled
   by its components; vector and result may be the same */
static inline void vector3f_rotate_into_axes(matrix3x3 const *matrix, vector3f const *vector, vector3f *result)
{
	vector3f copy;

	if (vector == result)
	{
		copy = *vector;
		vector = &copy;
	}
	real i = vector->j * matrix->left.i;
	i += vector->k * matrix->up.i;
	i += vector->i * matrix->forward.i;
	result->i = i;
	real j = vector->i * matrix->forward.j;
	j += vector->j * matrix->left.j;
	j += vector->k * matrix->up.j;
	result->j = j;
	real k = vector->i * matrix->forward.k;
	k += vector->j * matrix->left.k;
	k += vector->k * matrix->up.k;
	result->k = k;
}

/* the sum a + b; result may be either input */
static __forceinline void vector3f_add(vector3f const *a, vector3f const *b, vector3f *result)
{
	result->i = a->i + b->i;
	result->j = a->j + b->j;
	result->k = a->k + b->k;
}

/* a + b * scale, for a position or velocity moved along b; result may be either input */
static __forceinline void vector3f_add_scaled(vector3f const *a, vector3f const *b, real scale, vector3f *result)
{
	result->i = a->i + b->i * scale;
	result->j = a->j + b->j * scale;
	result->k = a->k + b->k * scale;
}


static __forceinline void scaled_difference3f(real const *first, real const *second, real scale, real *result)
{
    for (long component = 0; component < 3; ++component)
        result[component] = (second[component] - first[component]) * scale;
}

#endif
