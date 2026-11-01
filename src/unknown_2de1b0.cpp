// @flags /O2 /GL- /GX
// Havok (third-party physics library): casting a segment against a capsule.
// A segment starting inside misses; otherwise the closest points of the
// segment's line and the capsule's axis decide between the side, hit where
// the line comes within the radius, and an end cap, cast as a sphere about
// the nearer end point. (Its own file: it needs /GX for the sphere it
// unwinds.)
#include <math.h>
#include <xmmintrin.h>
#include "unknown_0a9f20.h"

static __declspec(align(16)) const float g_412420[4] = {1.0f, 1.0f, 1.0f, 1.0f};

// a value in the first term of a vector (src/unknown_1c58f0.cpp)
struct s_1c58f0
{
	__m128 field_0;
	s_1c58f0(real arg_0);
};

// the sphere an end cap is cast as (src/unknown_2def90.cpp; its base's
// destructor is 0xdc370)
class c_2def90 : public c_reference_a9f20
{
public:
	c_2def90(real arg_0);

	long user_value;
	float radius;
};

#include "sphere_2deda0.h"

// the point of a segment closest to a point (src/unknown_2ddfc0.cpp)
class segment_2ddfc0
{
public:
	void function_2ddfc0(__m128 const &point, __m128 const &start, __m128 const &end, __m128 &out);
};

// the closest points of two lines (src/unknown_2dddf0.cpp)
class line_query_2dddf0
{
public:
	void function_2dddf0(__m128 const *first_point, __m128 const *first_direction, __m128 const *second_point, __m128 const *second_direction,
		float *distance_squared, float *first_fraction, float *second_fraction, __m128 *first_closest, __m128 *second_closest);
};

// a segment cast at an end cap, as the sphere's cast takes it
struct cap_segment_2de1b0
{
	__m128 start;
	__m128 end;
	long unknown20;
	long unknown24;
};

// the three-term sum of a vector's squares
__forceinline __m128 length_squared_2de1b0(__m128 const &value)
{
	__m128 const product = _mm_mul_ps(value, value);
	return _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product));
}

__forceinline float dot3_2de1b0(__m128 const &a, __m128 const &b)
{
	__m128 const product = _mm_mul_ps(a, b);
	float result;
	_mm_store_ss(&result, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
	return result;
}

// one over the square root of the first term, refined by a Newton step
__forceinline __m128 inverse_sqrt_2de1b0(__m128 const &value)
{
	__m128 const estimate = _mm_rsqrt_ss(value);
	__m128 const half = _mm_set_ss(0.5f);
	__m128 const three = _mm_set_ss(3.0f);
	return _mm_mul_ss(_mm_mul_ss(half, estimate), _mm_sub_ss(three, _mm_mul_ss(_mm_mul_ss(value, estimate), estimate)));
}

// the point a fraction of the way from a to b
__forceinline __m128 interpolate_2de1b0(__m128 const &a, __m128 const &b, float fraction)
{
	__m128 scale = _mm_load_ss(&fraction);
	scale = _mm_shuffle_ps(scale, scale, 0);
	return _mm_add_ps(_mm_mul_ps(_mm_sub_ps(*(__m128 const *)g_412420, scale), a), _mm_mul_ps(scale, b));
}

class capsule_2de1b0
{
public:
	char unknown0[0xc];
	float radius;
	__m128 vertices[2];

	answer_2deda0 function_2de1b0(segment_2deda0 const *segment, hit_2deda0 *hit);
};

// @retail 0x2de1b0
answer_2deda0 capsule_2de1b0::function_2de1b0(segment_2deda0 const *segment, hit_2deda0 *hit)
{
	__m128 start;
	{
		__m128 closest;
		((segment_2ddfc0 *)this)->function_2ddfc0(segment->start, vertices[0], vertices[1], closest);
		start = segment->start;
		float distance;
		_mm_store_ss(&distance, _mm_sqrt_ss(length_squared_2de1b0(_mm_sub_ps(start, closest))));
		if (distance < radius)
		{
			return answer_2deda0(false);
		}
	}

	__m128 end = segment->end;
	__m128 const a = vertices[0];
	__m128 const b = vertices[1];
	__m128 ray = _mm_sub_ps(end, start);
	__m128 axis = _mm_sub_ps(b, a);
	float distance_squared = 3.40282347e+38f;
	float ray_fraction;
	{
		float axis_fraction;
		__m128 ray_closest;
		__m128 axis_closest;
		((line_query_2dddf0 *)this)->function_2dddf0(&segment->start, &ray, &vertices[0], &axis,
			&distance_squared, &ray_fraction, &axis_fraction, &ray_closest, &axis_closest);
	}
	if (distance_squared > radius * radius)
	{
		return answer_2deda0(false);
	}

	// the axis's direction and length
	float axis_length;
	__m128 normal;
	float axis_length_squared = dot3_2de1b0(axis, axis);
	if (axis_length_squared > 1.1920929e-07f)
	{
		__m128 const sum = length_squared_2de1b0(axis);
		__m128 scale = inverse_sqrt_2de1b0(sum);
		scale = _mm_shuffle_ps(scale, scale, 0);
		float length;
		_mm_store_ss(&length, _mm_mul_ss(sum, scale));
		axis_length = length;
		normal = _mm_mul_ps(scale, axis);
	}
	else
	{
		axis_length = 0.0f;
		normal = _mm_setzero_ps();
	}

	// back along the ray to where it enters the infinite cylinder
	float const along = -dot3_2de1b0(ray, normal);
	ray = _mm_add_ps(ray, _mm_mul_ps(_mm_set_ps1(along), normal));
	float inverse_length;
	_mm_store_ss(&inverse_length, inverse_sqrt_2de1b0(length_squared_2de1b0(ray)));
	ray_fraction -= (float)sqrt(radius * radius - distance_squared) * inverse_length;
	if (ray_fraction > 1.0f)
	{
		return answer_2deda0(false);
	}

	float const a_along = dot3_2de1b0(a, normal);
	__m128 const entry = interpolate_2de1b0(start, end, ray_fraction);
	float const entry_along = dot3_2de1b0(entry, normal) - a_along;
	if (ray_fraction >= 0.0f && entry_along > 0.0f && entry_along < axis_length)
	{
		// the side
		s_1c58f0 const fraction(entry_along / axis_length);
		__m128 const scale = _mm_shuffle_ps(fraction.field_0, fraction.field_0, 0);
		__m128 const on_axis = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(*(__m128 const *)g_412420, scale), a), _mm_mul_ps(scale, b));
		__m128 const outward = _mm_sub_ps(entry, on_axis);
		__m128 const inverse = inverse_sqrt_2de1b0(length_squared_2de1b0(outward));
		hit->normal = _mm_mul_ps(_mm_shuffle_ps(inverse, inverse, 0), outward);
		hit->fraction = ray_fraction;
		hit->part = -1;
		return answer_2deda0(true);
	}

	// an end cap: none when the start is outside the capsule's radius and
	// the line entered behind it
	float const start_fraction = (dot3_2de1b0(start, normal) - a_along) / axis_length;
	__m128 const start_on_axis = interpolate_2de1b0(a, b, start_fraction);
	float start_distance;
	_mm_store_ss(&start_distance, _mm_sqrt_ss(length_squared_2de1b0(_mm_sub_ps(start, start_on_axis))));
	if (start_distance > radius && ray_fraction < 0.0f)
	{
		return answer_2deda0(false);
	}
	cap_segment_2de1b0 cap;
	cap.unknown24 = 0;
	if (entry_along <= 0.0f)
	{
		start = _mm_sub_ps(start, a);
		end = _mm_sub_ps(end, a);
	}
	else
	{
		start = _mm_sub_ps(start, b);
		end = _mm_sub_ps(end, b);
	}
	cap.start = start;
	cap.end = end;
	c_2def90 sphere(radius);
	return ((sphere_2deda0 const *)&sphere)->function_2deda0((segment_2deda0 const *)&cap, hit);
}
