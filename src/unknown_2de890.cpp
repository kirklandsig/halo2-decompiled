// @flags /O2 /GL-
// Havok (third-party physics library): an object placing a child object by
// a transform, constructed from the child alone with the identity transform.
#include "shape_2de750.h"
#include "havok_reference.h"

// a 3x4 transform of SSE columns
struct transform_2de890
{
	__m128 columns[4];

	// the element in a row of a column
	float &element(int row, int column) { return ((float *)columns)[column * 4 + row]; }

	void load_unit_columns()
	{
		columns[0] = _mm_setzero_ps();
		columns[1] = _mm_setzero_ps();
		columns[2] = _mm_setzero_ps();
		element(0, 0) = 1.0f;
		element(1, 1) = 1.0f;
		element(2, 2) = 1.0f;
		columns[3] = _mm_setzero_ps();
	}
};

// a point taken into the transform's frame: the position subtracted, then the
// transposed axes applied
static __forceinline __m128 inverse_transform_2de9e0(transform_2de890 const &transform, __m128 const &point)
{
	__m128 c0 = transform.columns[0];
	__m128 c1 = transform.columns[1];
	__m128 c2 = transform.columns[2];
	__m128 offset = _mm_sub_ps(point, transform.columns[3]);
	__m128 t0 = _mm_shuffle_ps(c0, c1, 0x44);
	__m128 t2 = _mm_shuffle_ps(c0, c1, 0xee);
	__m128 t1 = _mm_shuffle_ps(c2, c2, 0x44);
	__m128 t3 = _mm_shuffle_ps(c2, c2, 0xee);
	__m128 row_0 = _mm_shuffle_ps(t0, t1, 0x88);
	__m128 row_1 = _mm_shuffle_ps(t0, t1, 0xdd);
	__m128 row_2 = _mm_shuffle_ps(t2, t3, 0x88);
	return _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(row_0, _mm_shuffle_ps(offset, offset, 0x00)),
		_mm_mul_ps(row_1, _mm_shuffle_ps(offset, offset, 0x55))),
		_mm_mul_ps(row_2, _mm_shuffle_ps(offset, offset, 0xaa)));
}

struct c_query_transform;

// a direction rotated by a transform's axes (0x2da6e0)
struct vector_2da6e0
{
	__m128 value;
	void rotate(const c_query_transform *matrix, const __m128 *direction);
};

// a segment cast against a shape: its two ends and two values passed along
struct ray_2de9e0
{
	__m128 from;
	__m128 to;
	long field_20;
	long field_24;
};

// what a cast fills in, starting with the normal at the hit
struct hit_2de9e0 : public vector_2da6e0
{
};

// the one-byte answer of a cast, returned through a hidden pointer
class answer_2de9e0
{
public:
	answer_2de9e0() {}

	char value;
};

// a placement of a shape: two vectors, then its transform (0x2da450)
struct s_extent_transform
{
	__m128 rows[4];
	void compose(s_extent_transform const *a, s_extent_transform const *b);
};

struct placement_2deb20
{
	__m128 field_0;
	__m128 field_10;
	s_extent_transform transform;
};

// a shape with its placement, its parent's value and its parent
struct description_2deb20
{
	void const *shape;
	long field_4;
	placement_2deb20 const *placement;
	description_2deb20 const *parent;
};

// the declaration game code calls (src/unknown_1ee340.cpp)
class c_child_transform : public shape_2de750
{
public:
	c_havok_reference_counted *child;
	transform_2de890 transform;

	c_child_transform(c_havok_reference_counted *child);
	float function_2de8e0(__m128 const &direction) const;
	answer_2de9e0 function_2de9e0(ray_2de9e0 const *ray, hit_2de9e0 *hit) const;
	void function_2deb20(ray_2de9e0 const *ray, description_2deb20 const *description, void *collector) const;
};

// the child seen through its vtable: slot 7 takes a direction in the child's
// own frame and gives back a distance along it
class child_slots_2de8e0
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void slot_6() {}
	virtual float slot_7(__m128 const *direction) { return 0.0f; }
	// casts a segment in the child's own frame
	virtual void slot_8(answer_2de9e0 *result, ray_2de9e0 const *ray, hit_2de9e0 *hit) {}
	// casts a segment in the child's own frame, results going to a collector
	virtual void slot_9(ray_2de9e0 const *ray, description_2deb20 const *description, void *collector) {}
};

// @retail 0x2de890
c_child_transform::c_child_transform(c_havok_reference_counted *child_)
	: child(child_)
{
	transform.load_unit_columns();
	child->reference_count++;
}

// the direction taken into the child's frame (the transposed axes), the
// child's distance along it, and that point placed back by the transform and
// measured along the direction
// @retail 0x2de8e0
float c_child_transform::function_2de8e0(__m128 const &direction) const
{
	__m128 row0 = transform.columns[0];
	__m128 row1 = transform.columns[1];
	__m128 row2 = transform.columns[2];
	__m128 row3 = row2;
	_MM_TRANSPOSE4_PS(row0, row1, row2, row3);
	__m128 const d = direction;
	__m128 local = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(row0, _mm_shuffle_ps(d, d, 0x00)),
		_mm_mul_ps(row1, _mm_shuffle_ps(d, d, 0x55))),
		_mm_mul_ps(row2, _mm_shuffle_ps(d, d, 0xaa)));
	__m128 const single = _mm_set_ss(((child_slots_2de8e0 *)child)->slot_7(&local));
	__m128 const scaled = _mm_mul_ps(_mm_shuffle_ps(single, single, 0), local);
	__m128 const point = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(transform.columns[0], _mm_shuffle_ps(scaled, scaled, 0x00)),
		_mm_mul_ps(transform.columns[1], _mm_shuffle_ps(scaled, scaled, 0x55))),
		_mm_add_ps(_mm_mul_ps(transform.columns[2], _mm_shuffle_ps(scaled, scaled, 0xaa)), transform.columns[3]));
	__m128 const product = _mm_mul_ps(point, direction);
	float result;
	_mm_store_ss(&result, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
	return result;
}

// a segment cast against the child: its ends taken into the child's frame,
// and the normal of a hit turned back by the transform
// @retail 0x2de9e0
answer_2de9e0 c_child_transform::function_2de9e0(ray_2de9e0 const *ray, hit_2de9e0 *hit) const
{
	ray_2de9e0 local;
	local.field_20 = ray->field_20;
	local.field_24 = ray->field_24;
	local.from = inverse_transform_2de9e0(transform, ray->from);
	local.to = inverse_transform_2de9e0(transform, ray->to);
	answer_2de9e0 result;
	((child_slots_2de8e0 *)child)->slot_8(&result, &local, hit);
	if (result.value)
	{
		__m128 normal = hit->value;
		hit->rotate((c_query_transform const *)&transform, &normal);
	}
	return result;
}

// the same cast with results going to a collector: the child is described
// with its placement composed with the transform
// score 51 (was 51): Frame 0xa8 and extent 336 agree. Translation and point
// loads, metadata stores, argument pushes and descriptor carriers differ.
// Translation named first 51; offset before transpose 52; shared placement
// input 51; both metadata first 51; child field first 51; shared child 51;
// whole descriptor 52; all coordinate products named 51. Both inverse users
// checked. NOT kept: no match or qualifying gain; original body and helper
// restored.
// @retail 0x2deb20
void c_child_transform::function_2deb20(ray_2de9e0 const *ray, description_2deb20 const *description, void *collector) const
{
	ray_2de9e0 local;
	local.field_20 = ray->field_20;
	local.from = inverse_transform_2de9e0(transform, ray->from);
	local.field_24 = ray->field_24;
	local.to = inverse_transform_2de9e0(transform, ray->to);
	placement_2deb20 placement;
	placement.field_0 = description->placement->field_0;
	placement.field_10 = description->placement->field_10;
	placement.transform.compose(&description->placement->transform, (s_extent_transform const *)&transform);
	description_2deb20 child_description;
	child_description.parent = description;
	child_description.placement = &placement;
	child_description.shape = child;
	child_description.field_4 = description->field_4;
	((child_slots_2de8e0 *)child)->slot_9(&local, &child_description, collector);
}
