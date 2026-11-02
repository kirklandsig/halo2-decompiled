#ifndef JOINT_BUILDERS_312E00_H
#define JOINT_BUILDERS_312E00_H
#include "lane_a_constraints.h"
#include "solver_rows_31e7b0.h"

struct s_ops_cursors_31e740 { __m128 *term; unsigned char *ops; };
struct s_ops_input_31e770 { long body_a, body_b; };
void __cdecl function_31e770(s_ops_input_31e770 const *, s_ops_cursors_31e740 *, long, long);
float __cdecl approximate_angle_3725e0(float y, float x);

// A body's motion pointer is at +0x3c; its world transform begins at +0x80.
struct joint_body_312e00
{
	unsigned char unknown00[0x3c];
	unsigned char *motion;
	__forceinline solver_transform_31f160 const *transform() const
	{
		return (solver_transform_31f160 const *)(motion + 0x80);
	}
};

static __forceinline void rotate_joint_vectors(__m128 *out, __m128 const *in,
	solver_transform_31f160 const *transform, long count)
{
	for (; count > 0; --count, ++in, ++out)
	{
		__m128 v = *in;
		__m128 z = _mm_mul_ps(transform->rotation[2], _mm_shuffle_ps(v, v, 0xaa));
		__m128 y = _mm_mul_ps(transform->rotation[1], _mm_shuffle_ps(v, v, 0x55));
		__m128 x = _mm_mul_ps(transform->rotation[0], _mm_shuffle_ps(v, v, 0));
		*out = _mm_add_ps(_mm_add_ps(x, y), z);
	}
}

static __forceinline __m128 joint_cross(__m128 const &a, __m128 const &b)
{
	__m128 reverse = _mm_mul_ps(_mm_shuffle_ps(a, a, 0xd2), _mm_shuffle_ps(b, b, 0xc9));
	__m128 forward = _mm_mul_ps(_mm_shuffle_ps(a, a, 0xc9), _mm_shuffle_ps(b, b, 0xd2));
	return _mm_sub_ps(forward, reverse);
}

static __forceinline __m128 joint_sum_three(__m128 const &v)
{
	__m128 xy = _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v);
	return _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), xy);
}

static __forceinline float joint_dot(__m128 const &a, __m128 const &b)
{
	float result;
	_mm_store_ss(&result, joint_sum_three(_mm_mul_ps(a, b)));
	return result;
}

static __forceinline __m128 joint_normalize(__m128 const &v)
{
	__m128 norm = joint_sum_three(_mm_mul_ps(v, v));
	__m128 inverse = _mm_rsqrt_ss(norm);
	__m128 error = _mm_sub_ss(_mm_set_ss(3.0f), _mm_mul_ss(_mm_mul_ss(norm, inverse), inverse));
	__m128 corrected = _mm_mul_ss(_mm_mul_ss(_mm_set_ss(0.5f), inverse), error);
	return _mm_mul_ps(_mm_shuffle_ps(corrected, corrected, 0), v);
}

__declspec(align(16)) static unsigned long const joint_sign_bits[4] =
	{0x80000000, 0x80000000, 0x80000000, 0x80000000};
#endif
