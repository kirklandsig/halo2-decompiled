// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1EFF40.CPP: casts a ray against each shape of a lookup (built with
   SSE, apart from its neighbours in unknown_1efac0.cpp) */

#include <string.h>
#include <float.h>
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1efac0.h"
#include "unknown_1c3b70.h"

struct s_table;
struct s_iterator
{
	s_table *table;
	signed char bytes[16];
	union
	{
		dword position;
		struct
		{
			byte first;
			byte item;
			byte sub;
			byte group;
		};
	};
	void *current;

	bool advance();
};

struct s_mix_output;
struct s_mix_source;
void function_1efbd0(dword id, s_mix_output *output, s_mix_source *source, signed char *indices);

struct s_1de6d2;
struct s_1de6d3
{
 real field_0;
 plane3f const *field_4;
 long field_8;
 long field_c;
 long field_10;
 byte field_14;
 byte field_15;
 short field_16;
 long field_18;
 long field_1c[256];
};

struct s_1de6d0
{
 dword field_0;
 s_1de6d2 const *field_4;
 short field_8;
 dword const *field_c;
 point3f const *field_10;
 vector3f const *field_14;
 s_1de6d3 *field_18;
 long field_1c;
 byte field_20;
 long field_24;
};

struct s_lookup_ray_result
{
 dword position;
 short node;
 short item;
 s_1de6d3 hit;
};

void function_141590(const transform4x3f *matrix, transform4x3f *inverse);
bool __stdcall function_1de6d0(s_1de6d0 *query, long root, real start, real end);

/* the point and the vector in the matrix's frame; the point's x row is
   summed term by term, as unknown_11c380.cpp's transform4x3f_apply_point
   does, which keeps retail's order of the products */
static inline void transform4x3f_apply_point(transform4x3f const *matrix, point3f const *point, point3f *result)
{
 real x = point->x, y = point->y, z = point->z;
 if (matrix->scale != 1.0f)
 {
  x = matrix->scale * x;
  y = matrix->scale * y;
  z = matrix->scale * z;
 }
 real term_a = matrix->up.i * z;
 real term_b = matrix->forward.i * x;
 real term_c = matrix->left.i * y;
 result->x = term_a + term_b + term_c + matrix->position.x;
 result->y = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x + matrix->position.y;
 result->z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z;
}

static inline void transform4x3f_apply_vector(transform4x3f const *matrix, vector3f const *vector, vector3f *result)
{
 real i = vector->i, j = vector->j, k = vector->k;
 if (matrix->scale != 1.0f)
 {
  i *= matrix->scale;
  j *= matrix->scale;
  k *= matrix->scale;
 }
 result->i = matrix->up.i * k + matrix->left.i * j + matrix->forward.i * i;
 result->j = matrix->up.j * k + matrix->left.j * j + matrix->forward.j * i;
 result->k = matrix->up.k * k + matrix->left.k * j + matrix->forward.k * i;
}

// score 9 (was 123): in the point's x row retail multiplies forward first
// and adds up last ((left + forward) + up); every order of the three
// products tried keeps left.i in xmm0 only in up-left-forward order
// Matched in a full build with stand-in SDK headers (was 9): in transform4x3f_apply_point the x result's three
// products are named locals summed as up, forward, left.
// @retail 0x1eff40
bool function_1eff40(s_lookup *lookup, long flags, const point3f *point,
 const vector3f *direction, s_lookup_ray_result *result)
{
 s_1de6d3 *hit = &result->hit;
 bool found = false;
 s_iterator iterator;
 hit->field_0 = FLT_MAX;
 function_1efbd0((dword)lookup->tag_b, (s_mix_output *)&iterator,
  (s_mix_source *)lookup->tag_a, (signed char *)lookup->pointer_a);
 while (iterator.advance())
 {
  dword position = iterator.position;
  long node = iterator.position & 0xff;
  transform4x3f inverse;
  function_141590((const transform4x3f *)lookup->pointer_b + node, &inverse);
  point3f local_point;
  transform4x3f_apply_point(&inverse, point, &local_point);
  vector3f local_direction;
  transform4x3f_apply_vector(&inverse, direction, &local_direction);
  s_1de6d0 query;
  query.field_0 = flags;
  query.field_4 = (s_1de6d2 const *)iterator.current;
  query.field_8 = 0;
  query.field_c = NULL;
  query.field_10 = &local_point;
  query.field_14 = &local_direction;
  query.field_18 = hit;
  real fraction = hit->field_0;
  hit->field_0 = fraction < 0.0f ? 0.0f : fraction;
  hit->field_18 = 0;
  query.field_1c = NONE;
  query.field_20 = 0;
  query.field_24 = NONE;
  if (function_1de6d0(&query, 0, 0.0f, fraction < 0.0f ? 0.0f : fraction > 1.0f ? 1.0f : fraction))
  {
   result->position = position;
   result->node = (short)node;
   result->item = iterator.group;
   found = true;
  }
 }
 return found;
}
