// @flags /O2 /GL-
// Havok (third-party physics library): casting a segment against a sphere
// centred on the origin, reporting the first entry point.
#include <math.h>
#include <xmmintrin.h>
#include "sphere_2deda0.h"

static __declspec(align(16)) const float g_ones_412420[4] = {1.0f, 1.0f, 1.0f, 1.0f};

// three-term dot product of two vectors
inline float dot3_2deda0(__m128 const &a, __m128 const &b)
{
	__m128 const product = _mm_mul_ps(a, b);
	float result;
	_mm_store_ss(&result, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
	return result;
}

// a long segment is first moved to the point nearest the centre, which keeps
// the quadratic well conditioned; the hit must lie within the segment
// @retail 0x2deda0
answer_2deda0 sphere_2deda0::function_2deda0(segment_2deda0 const *segment, hit_2deda0 *hit) const
{
	float radius_squared = radius * radius;
	__m128 start = segment->start;
	__m128 end = segment->end;
	__m128 direction = _mm_sub_ps(end, start);
	float projection = dot3_2deda0(direction, start);
	float coefficient = projection;
	if (projection >= 0.0f)
		return answer_2deda0(false);
	float length_squared = dot3_2deda0(direction, direction);
	__m128 origin;
	if (projection * projection > length_squared * radius_squared * 100.0f)
	{
		if (length_squared < radius_squared)
			return answer_2deda0(false);
		projection = -projection;
		float fraction = projection / length_squared;
		__m128 scale = _mm_set_ps1(fraction);
		origin = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(*(__m128 const *)g_ones_412420, scale), start), _mm_mul_ps(scale, end));
		coefficient = 0.0f;
	}
	else
	{
		projection = 0.0f;
		origin = start;
	}
	float origin_squared = dot3_2deda0(origin, origin);
	float discriminant = coefficient * coefficient - (origin_squared - radius_squared) * length_squared;
	if (discriminant <= 0.0f)
		return answer_2deda0(false);
	coefficient = -coefficient;
	float numerator = coefficient - sqrt(discriminant);
	numerator += projection;
	if (numerator >= length_squared || numerator < 0.0f)
		return answer_2deda0(false);
	float fraction = numerator / length_squared;
	hit->fraction = fraction;
	__m128 scale = _mm_set_ps1(fraction);
	__m128 point = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(*(__m128 const *)g_ones_412420, scale), segment->start), _mm_mul_ps(scale, segment->end));
	hit->normal = point;
	float inverse_radius = 1.0f / radius;
	hit->part = -1;
	__m128 inverse_scalar = _mm_set_ss(inverse_radius);
	__m128 inverse_scale = _mm_shuffle_ps(inverse_scalar, inverse_scalar, 0);
	hit->normal = _mm_mul_ps(inverse_scale, point);
	return answer_2deda0(true);
}
