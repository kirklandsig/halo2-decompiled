/* VECTOR2F_PRODUCTS.H: the dot and cross products of two 2-D vectors;
   src/unknown_1b0cd0.cpp, src/unknown_1ff010.cpp and src/unknown_26c380.cpp
   share them, so they live here */

#ifndef VECTOR2F_PRODUCTS_H
#define VECTOR2F_PRODUCTS_H

#include "unknown_11cc90.h"

/* a . b */
static inline real dot_product2d(vector2f const *a, vector2f const *b)
{
	return a->i * b->i + a->j * b->j;
}

/* the z component of a x b (positive when b lies counterclockwise of a) */
static inline real cross_product2d(vector2f const *a, vector2f const *b)
{
	return a->i * b->j - a->j * b->i;
}

#endif
