// @flags /O2 /GL- /GX
// Havok (third-party physics library): creating the object kept for a pair
// of shapes one of which is a triangle, holding the terms that solve for a
// point's weights in the triangle's plane, either way round.
#include <xmmintrin.h>
#include "pair_agent_2ed170.h"
#include "point_triangle_308d90.h"

void __cdecl function_308910(__m128 const *corners, float *out);

// a shape whose three corners start at its 16th byte
struct triangle_2f0e70
{
	char field_0[0x10];
	__m128 corners[3];
};

class triangle_terms_2f0e70 : public sized_2ed170
{
public:
	unsigned short field_c;
	float terms[4];

	__forceinline triangle_terms_2f0e70(triangle_2f0e70 const **triangle, long arg_3) :
		sized_2ed170(arg_3)
	{
		field_c = 0xffff;
		function_308910((*triangle)->corners, terms);
	}
};

// the same with the two shapes the other way round
class swapped_triangle_terms_2f0e70 : public triangle_terms_2f0e70
{
public:
	__forceinline swapped_triangle_terms_2f0e70(triangle_2f0e70 const **triangle, long arg_3) :
		triangle_terms_2f0e70(triangle, arg_3)
	{
	}
};

// @retail 0x2f0e70
triangle_terms_2f0e70 *__cdecl function_2f0e70(description_2ed170 const *description, triangle_2f0e70 const **other, dispatcher_2ed170 **dispatcher, long arg_3)
{
	return new triangle_terms_2f0e70(other, arg_3);
}

// @retail 0x2f0ef0
swapped_triangle_terms_2f0e70 *__cdecl function_2f0ef0(triangle_2f0e70 const **description, typed_2ed170 **other, dispatcher_2ed170 **dispatcher, long arg_3)
{
	return new swapped_triangle_terms_2f0e70(description, arg_3);
}

// a shape with a radius: a sphere, or a triangle whose corners follow
struct rounded_2f0f70
{
	char unknown0[0xc];
	float radius;
	__m128 corners[3];
};

struct transform_2f0f70
{
	__m128 axes[3];
	__m128 position;
};

// where a body sits: two vectors, then its transform
struct placement_2f0f70
{
	__m128 field_0;
	__m128 field_10;
	transform_2f0f70 transform;
};

// a point placed by a transform
struct placed_point_2f0f70
{
	__m128 value;

	placed_point_2f0f70 &operator=(__m128 const &point)
	{
		value = point;
		return *this;
	}

	void set_transformed(transform_2f0f70 const *transform, __m128 const *point)
	{
		__m128 p = *point;
		__m128 z = _mm_add_ps(_mm_mul_ps(transform->axes[2], _mm_shuffle_ps(p, p, 0xaa)), transform->position);
		__m128 y = _mm_mul_ps(transform->axes[1], _mm_shuffle_ps(p, p, 0x55));
		__m128 x = _mm_mul_ps(transform->axes[0], _mm_shuffle_ps(p, p, 0x00));
		value = _mm_add_ps(_mm_add_ps(x, y), z);
	}
};

// one side of a query: a shape, a key, where it sits, and its parent side
struct body_2f0f70
{
	rounded_2f0f70 const *shape;
	long key;
	placement_2f0f70 const *placement;
	body_2f0f70 const *parent;
};

struct input_2f0f70
{
	long field_0;
	long field_4;
	float tolerance;
};

// a contact: its point, with the separation in the fourth lane, and its
// normal
struct contact_2f0f70
{
	__m128 point;
	placed_point_2f0f70 normal;
};

// the contact of a sphere with a triangle, the triangle's corners taken into
// world space: 0 when they are further apart than their radii and the
// tolerance, otherwise 1, or 2 for the triangle's other closest parts
// @retail 0x2f0f70
int __cdecl function_2f0f70(body_2f0f70 const *a, body_2f0f70 const *b, input_2f0f70 const *input, float const *terms, contact_2f0f70 *contact)
{
	rounded_2f0f70 const *sphere = a->shape;
	rounded_2f0f70 const *triangle = b->shape;
	__m128 const *centre = &a->placement->transform.position;
	placed_point_2f0f70 corners[3];
	__m128 const *vertex = triangle->corners;
	placed_point_2f0f70 *corner = corners;
	for (int count = 3; count != 0; count--)
	{
		corner->set_transformed(&b->placement->transform, vertex);
		corner++;
		vertex++;
	}
	closest_308d90 closest;
	int status = function_308d90(centre, &corners[0].value, terms, &closest);
	float const radii = sphere->radius + triangle->radius;
	if (radii + input->tolerance > closest.distance)
	{
		__m128 const single = _mm_set_ss(triangle->radius - closest.distance);
		__m128 const scale = _mm_shuffle_ps(single, single, 0);
		placed_point_2f0f70 point;
		point = _mm_add_ps(*centre, _mm_mul_ps(scale, closest.normal));
		contact->normal = closest.normal;
		contact->point = point.value;
		contact->point.m128_f32[3] = closest.distance - radii;
		return status ? 2 : 1;
	}
	return 0;
}
