// @flags /O2 /GL- /GX
// Havok (third-party physics library): a second copy of 0x2eb290's query: the
// two sides swapped for the tree query, into a collector of the nearest
// result; and a cast of a box through a grid.
#include "timer_mark_2ebc80.h"
#include <xmmintrin.h>
#include "lane_b_flip_collectors.h"

struct body_2ea940;
struct input_2ea940;

void __cdecl function_2ea940(body_2ea940 const *a, body_2ea940 const *b, input_2ea940 const *input, void *collector);

// a collector that starts with no result: the largest float as its distance
class nearest_2eb610
{
public:
	nearest_2eb610() : distance(3.40282347e+38f) {}
	virtual ~nearest_2eb610() {}

	float distance;
};

class nearest_value_2eb610 : public nearest_2eb610
{
public:
	nearest_value_2eb610(long value_) : value(value_) {}

	long value;
};

// @retail 0x2eb610
void __cdecl function_2eb610(body_2ea940 const *a, body_2ea940 const *b, input_2ea940 const *input, long value)
{
	/* the caller's collector, wrapped in the one that turns what the
	   swapped query finds back (retail table 0x412970) */
	lane_b_flip_point_collector collector((lane_b_cd_point_collector *)value);
	function_2ea940(b, a, input, &collector);
}

// three axes and a position (0x2da4d0)
struct c_query_transform
{
	__m128 axes[3];
	__m128 position;
	void inverse_product(const c_query_transform *a, const c_query_transform *b);
};

struct vector_2da6e0
{
	__m128 value;
	void inverse_rotate(const c_query_transform *matrix, const __m128 *direction);
};

static __declspec(align(16)) const float g_ones_412420[4] = {1.0f, 1.0f, 1.0f, 1.0f};

// a box given by its two corners
struct bounds_2eb670
{
	__m128 minimum;
	__m128 maximum;
};

class shape_2eb670
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual long count() { return 0; }
	// the shape's bounds under a transform, widened by a tolerance
	virtual void bounds(c_query_transform const *transform, float tolerance, bounds_2eb670 *out) const {}
};

// where a body sits: two vectors, then its transform
struct placement_2eb670
{
	__m128 field_0;
	__m128 field_10;
	c_query_transform transform;
};

// one side of a query
struct body_2eb670
{
	shape_2eb670 const *shape;
	long key;
	placement_2eb670 const *placement;
	body_2eb670 const *parent;
};

// a cast's input: the tolerance and the path the first side moves along
struct cast_input_2eb670
{
	void *dispatcher;
	void *filter;
	float tolerance;
	long field_c;
	__m128 path;
};

class counted_3028c0;
struct grid_holder_3028c0;

// a segment and the direction it sweeps (0x3028c0)
struct input_3028c0
{
	__m128 start;
	__m128 end;
	__m128 direction;
	cast_input_2eb670 const *cast;
	counted_3028c0 **counted;
	grid_holder_3028c0 **holder;
};

struct grid_3028c0;

class walker_3028c0
{
public:
	char unknown0[0x10];
	grid_3028c0 const *grid;
	float inverse_cell_size;
	long count;
	float field_1c;
	float field_20;
	long field_24;
	long field_28;
	input_3028c0 const *input;

	void function_3028c0(input_3028c0 const *input_, long arg_1, long arg_2);
};

// a vector of four copies of a float
inline __m128 splat_2eb670(float value)
{
	__m128 const single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

// the first side's box, taken into the second side's frame, swept along the
// path through the second side's grid
// @retail 0x2eb670
void __cdecl function_2eb670(void const *a, void const *b, void const *input, void *collector, void *second_collector)
{
	timer_add_2ebc80("TtMopp");
	body_2eb670 const *const first = (body_2eb670 const *)a;
	body_2eb670 const *const second = (body_2eb670 const *)b;
	cast_input_2eb670 const *const cast = (cast_input_2eb670 const *)input;
	c_query_transform transform;
	transform.inverse_product(&second->placement->transform, &first->placement->transform);
	bounds_2eb670 bounds;
	first->shape->bounds(&transform, cast->tolerance, &bounds);
	vector_2da6e0 path;
	path.inverse_rotate(&second->placement->transform, &cast->path);
	input_3028c0 walk;
	__m128 const half = splat_2eb670(0.5f);
	walk.start = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(*(__m128 const *)g_ones_412420, half), bounds.minimum), _mm_mul_ps(half, bounds.maximum));
	walk.end = _mm_add_ps(walk.start, path.value);
	walk.counted = (counted_3028c0 **)first;
	walk.holder = (grid_holder_3028c0 **)second;
	walk.direction = _mm_add_ps(_mm_mul_ps(splat_2eb670(0.5f), _mm_sub_ps(bounds.maximum, bounds.minimum)), _mm_set1_ps(cast->tolerance));
	walk.cast = cast;
	walker_3028c0 walker;
	walker.function_3028c0(&walk, (long)collector, (long)second_collector);
	timer_add_2ebc80("Et");
}
