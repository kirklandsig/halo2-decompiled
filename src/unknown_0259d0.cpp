/* UNKNOWN_0259D0.CPP: two header inline functions that retail keeps out of line
in some callers: distance3d (0x3ea30) and function_259d0 (0x259d0).

The source's shape decides the match:
- distance3d squares k, then j, then i: written i*i + j*j + k*k, the
  compiler evaluates the operands in another order.
- function_259d0 takes the random value into a local first and returns the
  sum through a (real) cast; without the cast, callers that keep the result
  on the x87 stack fuse the lower bound's add (faddp) where retail adds and
  pops it separately (0x21ce60). LTCG also deletes its unused file and line
  arguments, as in retail. */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <math.h>

__inline real square_root(
	real x)
{
	return (real)sqrt(x);
}

__inline vector3f *subtract_points3f(
	point3f const *a,
	point3f const *b,
	vector3f *result)
{
	result->i = b->x - a->x;
	result->j = b->y - a->y;
	result->k = b->z - a->z;
	return result;
}

// @retail 0x3ea30
inline real distance3d(
	point3f const *a,
	point3f const *b)
{
	vector3f v;
	subtract_points3f(a, b, &v);
	return square_root(v.k*v.k + v.j*v.j + v.i*v.i);
}

// @retail 0x259d0
inline real function_259d0(
	dword *seed,
	char const *file,
	long line,
	real lower_bound,
	real upper_bound)
{
	real random = function_x82e52f(seed, file, line);
	return (real)(lower_bound + (upper_bound - lower_bound) * random);
}
