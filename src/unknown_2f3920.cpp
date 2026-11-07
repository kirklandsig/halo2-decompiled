// @flags /O2 /GL-
// Havok (third-party physics library): two capsules: the contact between them
// (as a free function and as a method), and whether they overlap: the closest
// points of their segments, in the world, against the sum of their radii. The
// queries are marked in the timing stream.
// Both contact entry points share the segment-distance and fallback-normal path.
#include "timer_mark_2ebc80.h"
#include <math.h>
#include <xmmintrin.h>

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

// three axes and a position (0x2da4d0)
struct c_query_transform
{
	__m128 axes[3];
	__m128 position;
};

// a point taken out of a transform's frame (0x2da610, inlined here)
inline __m128 transform_point_2f40e0(c_query_transform const *m, __m128 const *point)
{
	__m128 p = *point;
	__m128 z = _mm_add_ps(_mm_mul_ps(m->axes[2], _mm_shuffle_ps(p, p, 0xaa)), m->position);
	__m128 y = _mm_mul_ps(m->axes[1], _mm_shuffle_ps(p, p, 0x55));
	__m128 x = _mm_mul_ps(m->axes[0], _mm_shuffle_ps(p, p, 0x00));
	return _mm_add_ps(_mm_add_ps(x, y), z);
}

// where a body sits: two vectors, then its transform
struct placement_2f40e0
{
	__m128 field_0;
	__m128 field_10;
	c_query_transform transform;
};

// a capsule: its radius and the two ends of its segment
struct capsule_2f40e0
{
	void const *table;
	long field_4;
	long field_8;
	float radius;
	__m128 ends[2];
};

// one side of a query: a shape, the key of the part concerned, where it sits,
// and the side it is a part of
struct body_2f40e0
{
	capsule_2f40e0 const *shape;
	long key;
	placement_2f40e0 const *placement;
	body_2f40e0 const *parent;
};

// takes the pairs a query finds
class collector_2f40e0
{
public:
	virtual void slot_0() {}
	virtual void add_pair(void const *a, void const *b) {}
};

// the closest point on the first segment, the offset from the closest point
// on the second, and the square of that offset's length (0x308590)
struct closest_points_308590
{
	__m128 first_point;
	__m128 offset;
	float distance_squared;
};

int __cdecl function_308590(__m128 const *first_start, __m128 const *first_direction, __m128 const *second_start, __m128 const *second_direction, closest_points_308590 *result);

// a contact passed to the collector: its point, with the separation in the
// fourth term, its normal, and the two sides
struct contact_2f3920
{
	__m128 point;
	__m128 normal;
	void const *a;
	void const *b;
};

class contact_collector_2f3920
{
public:
	virtual void slot_0() {}
	virtual void add_contact(contact_2f3920 const *contact) {}
};

struct input_2f3920
{
	long field_0;
	long field_4;
	float tolerance;
};

// the cross product of two vectors' first three terms
inline __m128 cross_2f3920(__m128 const &a, __m128 const &b)
{
	__m128 b_yzx = _mm_shuffle_ps(b, b, 0xc9);
	__m128 b_zxy = _mm_shuffle_ps(b, b, 0xd2);
	__m128 a_zxy = _mm_shuffle_ps(a, a, 0xd2);
	__m128 a_yzx = _mm_shuffle_ps(a, a, 0xc9);
	__m128 second = _mm_mul_ps(a_zxy, b_yzx);
	return _mm_sub_ps(_mm_mul_ps(a_yzx, b_zxy), second);
}

// the sum of a vector's first three terms, in the first
inline __m128 sum_2f3920(__m128 const &v)
{
	return _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v));
}

// a vector of four copies of a float
inline __m128 splat_2f3920(float value)
{
	__m128 const single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

// the contact of two capsules, passed to the collector when their segments
// come within the sum of their radii and the tolerance: its normal is the
// offset between the closest points, or across both segments when they meet,
// or square to the first segment when neither will do
__forceinline void capsule_contact_2f3920(void const *a, void const *b, void const *input, void *collector)
{
	timer_add_2ebc80("TtCapsCaps");
	body_2f40e0 const *side_a = (body_2f40e0 const *)a;
	body_2f40e0 const *side_b = (body_2f40e0 const *)b;
	capsule_2f40e0 const *capsule_a = side_a->shape;
	capsule_2f40e0 const *capsule_b = side_b->shape;
	struct query_head
	{
		__m128 direction_a;
		contact_2f3920 contact;
	};
	struct query_tail
	{
		__m128 direction_b;
		closest_points_308590 closest;
		__m128 ends_a[2];
		__m128 ends_b[2];
	};
	query_head head;
	query_tail tail;
	contact_2f3920 &contact = head.contact;
	__m128 (&ends_a)[2] = tail.ends_a;
	__m128 (&ends_b)[2] = tail.ends_b;
	closest_points_308590 &closest = tail.closest;
	contact.a = a;
	contact.b = b;
	{
		c_query_transform const *frame = &side_a->placement->transform;
		__m128 const *end = capsule_a->ends;
		__m128 *placed = ends_a;
		for (int count = 2; count != 0; count--)
		{
			*placed = transform_point_2f40e0(frame, end);
			placed++;
			end++;
		}
	}
	{
		__m128 const *end = capsule_b->ends;
		c_query_transform const *frame = &side_b->placement->transform;
		__m128 *placed = ends_b;
		for (int count = 2; count != 0; count--)
		{
			*placed = transform_point_2f40e0(frame, end);
			placed++;
			end++;
		}
	}
	__m128 &direction_a = head.direction_a;
	direction_a = _mm_sub_ps(ends_a[1], ends_a[0]);
	__m128 &direction_b = tail.direction_b;
	direction_b = _mm_sub_ps(ends_b[1], ends_b[0]);
	int const clamped = function_308590(&ends_a[0], &direction_a, &ends_b[0], &direction_b, &closest);
	float const radius = capsule_a->radius + capsule_b->radius;
	float const reach = radius + ((input_2f3920 const *)input)->tolerance;
	// Retail rejects ordered separation; an unordered comparison continues.
	if (!(closest.distance_squared > reach * reach))
	{
		float const distance = (float)sqrt(closest.distance_squared);
		if (clamped != 0)
		{
			if (closest.distance_squared > 0.0f)
			{
				contact.normal = closest.offset;
				goto normalize;
			}
		}
		else
		{
			__m128 across = cross_2f3920(direction_a, direction_b);
			float length_squared;
			_mm_store_ss(&length_squared, sum_2f3920(_mm_mul_ps(across, across)));
			if (length_squared > 1.192092896e-07f)
			{
				float along;
				_mm_store_ss(&along, sum_2f3920(_mm_mul_ps(across, closest.offset)));
				if (along < 0.0f)
				{
					across = _mm_xor_ps(across, *(__m128 const *)g_4124a0);
				}
				contact.normal = across;
				goto normalize;
			}
		}
		{
			int smallest = 0;
			int second = 1;
			int third = 2;
			double const &x = (float)fabs(direction_a.m128_f32[0]);
			float minimum = x;
			float const y = (float)fabs(direction_a.m128_f32[1]);
			float const z = (float)fabs(direction_a.m128_f32[2]);
			if (y < minimum)
			{
				second = 0;
				minimum = y;
				smallest = 1;
			}
			if (z < minimum)
			{
				third = smallest;
				smallest = 2;
			}
			contact.normal.m128_f32[smallest] = 0.0f;
			contact.normal.m128_f32[second] = direction_a.m128_f32[third];
			contact.normal.m128_f32[third] = -direction_a.m128_f32[second];
		}
normalize:
		{
			__m128 const normal = contact.normal;
			__m128 const sum = sum_2f3920(_mm_mul_ps(normal, normal));
			__m128 const inverse = _mm_rsqrt_ss(sum);
			__m128 const correction = _mm_sub_ss(_mm_set_ss(3.0f), _mm_mul_ss(_mm_mul_ss(sum, inverse), inverse));
			__m128 const scale = _mm_mul_ss(_mm_mul_ss(_mm_set_ss(0.5f), inverse), correction);
			contact.normal = _mm_mul_ps(_mm_shuffle_ps(scale, scale, 0), normal);
		}
		contact.point = _mm_add_ps(closest.first_point, _mm_mul_ps(splat_2f3920(capsule_b->radius - distance), contact.normal));
		contact.point.m128_f32[3] = distance - radius;
		((contact_collector_2f3920 *)collector)->add_contact(&contact);
	}
	timer_add_2ebc80("Et");
}

class capsule_pair_2f3920
{
public:
	void function_2f3920(void const *a, void const *b, void const *input, void *collector);
};

// @retail 0x2f3920
void capsule_pair_2f3920::function_2f3920(void const *a, void const *b, void const *input, void *collector)
{
	capsule_contact_2f3920(a, b, input, collector);
}

// @retail 0x2f3d00
void __cdecl function_2f3d00(void const *a, void const *b, void const *input, void *collector)
{
	capsule_contact_2f3920(a, b, input, collector);
}
// the ends of both segments, placed in the world; the second segment's ends
// come first in the frame
struct placed_ends_2f40e0
{
	__m128 side_b[2];
	__m128 side_a[2];
};

// @retail 0x2f40e0
void __cdecl function_2f40e0(void const *a, void const *b, void const *input, void *collector)
{
	timer_add_2ebc80("TtCapsCaps");
	body_2f40e0 const *side_a = (body_2f40e0 const *)a;
	body_2f40e0 const *side_b = (body_2f40e0 const *)b;
	capsule_2f40e0 const *capsule_a = side_a->shape;
	capsule_2f40e0 const *capsule_b = side_b->shape;
	placed_ends_2f40e0 ends;
	{
		c_query_transform const *frame = &side_a->placement->transform;
		__m128 const *end = capsule_a->ends;
		__m128 *placed = ends.side_a;
		for (int count = 2; count != 0; count--)
		{
			*placed = transform_point_2f40e0(frame, end);
			placed++;
			end++;
		}
	}
	{
		c_query_transform const *frame = &side_b->placement->transform;
		__m128 const *end = capsule_b->ends;
		__m128 *placed = ends.side_b;
		for (int count = 2; count != 0; count--)
		{
			*placed = transform_point_2f40e0(frame, end);
			placed++;
			end++;
		}
	}
	__m128 direction_a = _mm_sub_ps(ends.side_a[1], ends.side_a[0]);
	__m128 direction_b = _mm_sub_ps(ends.side_b[1], ends.side_b[0]);
	closest_points_308590 closest;
	function_308590(&ends.side_a[0], &direction_a, &ends.side_b[0], &direction_b, &closest);
	float radius = capsule_b->radius + capsule_a->radius;
	if (closest.distance_squared < radius * radius)
	{
		((collector_2f40e0 *)collector)->add_pair(a, b);
	}
	timer_add_2ebc80("Et");
}
