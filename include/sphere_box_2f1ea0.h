#ifndef SPHERE_BOX_2F1EA0_H
#define SPHERE_BOX_2F1EA0_H
// Havok (third-party physics library): the contact between a sphere and a
// box: the sphere's centre is carried into the box's frame; outside the box,
// the contact is along the line from the nearest point of the box, when it is
// within the tolerance; inside, it is along the axis whose face is nearest.
#include <xmmintrin.h>

static __declspec(align(16)) const unsigned long g_sign_mask_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
// the same bits, kept apart for reversing a direction (retail's linker merges
// the two)
static __declspec(align(16)) const unsigned long g_negate_mask_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
static __declspec(align(16)) const unsigned long g_magnitude_mask_455940[4] = {0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff};

// three axes and a position
struct c_query_transform
{
	__m128 axes[3];
	__m128 position;
};

// a point taken into a transform's frame (0x2da660)
struct c_query_point
{
	__m128 value;
	void inverse_transform(const c_query_transform *matrix, const __m128 *point);
};

struct sphere_2f23c0
{
	char unknown0[0xc];
	float radius;
};

// a box: its radius and its half extents
struct box_2f23c0
{
	char unknown0[0xc];
	float radius;
	__m128 half_extents;
};

struct motion_2f23c0
{
	char unknown0[0x20];
	c_query_transform transform;
};

struct body_2f23c0
{
	void const *shape;
	long key;
	motion_2f23c0 const *motion;
};

struct input_2f2130
{
	long field_0;
	long field_4;
	float tolerance;
};

// where two shapes touch: the point, with the separation in its fourth lane,
// and the normal
struct contact_point_2f1ea0
{
	__m128 point;
	__m128 normal;
};

// a direction turned by a transform
__forceinline __m128 rotate_2f2130(c_query_transform const *transform, __m128 const &d)
{
	__m128 z = _mm_mul_ps(transform->axes[2], _mm_shuffle_ps(d, d, 0xaa));
	__m128 y = _mm_mul_ps(transform->axes[1], _mm_shuffle_ps(d, d, 0x55));
	__m128 x = _mm_mul_ps(transform->axes[0], _mm_shuffle_ps(d, d, 0x00));
	return _mm_add_ps(_mm_add_ps(x, y), z);
}

// one over the square root of the low term, refined by a Newton step, in
// all four terms
__forceinline __m128 inverse_sqrt_2f2130(__m128 const &value)
{
	__m128 const estimate = _mm_rsqrt_ss(value);
	__m128 const product = _mm_mul_ss(_mm_mul_ss(value, estimate), estimate);
	__m128 const refined = _mm_mul_ss(_mm_mul_ss(_mm_set_ss(0.5f), estimate), _mm_sub_ss(_mm_set_ss(3.0f), product));
	return _mm_shuffle_ps(refined, refined, 0);
}

// a vector of four copies of a float
inline __m128 splat_2f2130(float value)
{
	__m128 const single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

// the contact of sphere `a` with box `b`; false when they are farther apart
// than the input's tolerance
__forceinline bool sphere_box_contact_2f1ea0(void const *a_, void const *b_, void const *input_, contact_point_2f1ea0 &contact)
{
	body_2f23c0 const *a = (body_2f23c0 const *)a_;
	body_2f23c0 const *b = (body_2f23c0 const *)b_;
	input_2f2130 const *input = (input_2f2130 const *)input_;
	c_query_point local;
	__m128 magnitude;
	__m128 const *centre = &a->motion->transform.position;
	local.inverse_transform(&b->motion->transform, centre);
	box_2f23c0 const *box = (box_2f23c0 const *)b->shape;
	sphere_2f23c0 const *sphere = (sphere_2f23c0 const *)a->shape;
	magnitude = _mm_and_ps(local.value, *(__m128 const *)g_magnitude_mask_455940);
	__m128 const outside = _mm_sub_ps(_mm_min_ps(magnitude, box->half_extents), magnitude);
	float separation;
	if (_mm_movemask_ps(outside) & 7)
	{
		float const radius = box->radius + sphere->radius;
		__m128 const squares = _mm_mul_ps(outside, outside);
		__m128 const length_squared = _mm_add_ss(_mm_shuffle_ps(squares, squares, 0xaa), _mm_add_ss(_mm_shuffle_ps(squares, squares, 0x55), squares));
		__m128 const inverse = inverse_sqrt_2f2130(length_squared);
		float length;
		_mm_store_ss(&length, _mm_mul_ss(length_squared, inverse));
		separation = length - radius;
		if (separation > input->tolerance)
		{
			return false;
		}
		// the normal: the offset to the box's nearest point, given the signs
		// of the centre's terms in the box's frame, then reversed to point
		// at the centre and turned out of that frame
		__m128 const sign = _mm_and_ps(local.value, *(__m128 const *)g_sign_mask_4124a0);
		__m128 const direction = _mm_xor_ps(_mm_mul_ps(inverse, outside), sign);
		contact.normal = rotate_2f2130(&b->motion->transform, _mm_xor_ps(direction, *(__m128 const *)g_negate_mask_4124a0));
	}
	else
	{
		float const dx = magnitude.m128_f32[0] - box->half_extents.m128_f32[0];
		float const dy = magnitude.m128_f32[1] - box->half_extents.m128_f32[1];
		float const dz = magnitude.m128_f32[2] - box->half_extents.m128_f32[2];
		int axis;
		float depth;
		if (dx > dy)
		{
			if (dx > dz)
			{
				depth = dx;
				axis = 0;
				contact.normal = b->motion->transform.axes[0];
			}
			else
			{
				depth = dz;
				axis = 2;
				contact.normal = b->motion->transform.axes[2];
			}
		}
		else if (dy > dz)
		{
			depth = dy;
			axis = 1;
			contact.normal = b->motion->transform.axes[1];
		}
		else
		{
			depth = dz;
			axis = 2;
			contact.normal = b->motion->transform.axes[2];
		}
		if (local.value.m128_f32[axis] < 0.0f)
		{
			contact.normal = _mm_xor_ps(contact.normal, *(__m128 const *)g_negate_mask_4124a0);
		}
		separation = depth - box->radius - sphere->radius;
	}
	contact.point = _mm_add_ps(*centre, _mm_mul_ps(splat_2f2130(-separation - sphere->radius), contact.normal));
	contact.point.m128_f32[3] = separation;
	return true;
}

#endif
