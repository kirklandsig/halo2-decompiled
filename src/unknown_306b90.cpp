// @flags /O2 /GL-
// Havok (third-party physics library): a box face picked by axis: its
// outward normal, nudged off the axis for a stable contact, and that normal
// placed by the box's transform.
#include <xmmintrin.h>

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

// a value in all four terms
inline __m128 splat_306b90(float value)
{
	__m128 const single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

struct transform_306b90
{
	__m128 axes[3];
	__m128 position;
};

// a point placed by a transform
inline __m128 transform_point_306b90(transform_306b90 const *transform, __m128 const *point)
{
	__m128 const p = *point;
	__m128 const z = _mm_add_ps(_mm_mul_ps(transform->axes[2], _mm_shuffle_ps(p, p, 0xaa)), transform->position);
	__m128 const y = _mm_mul_ps(transform->axes[1], _mm_shuffle_ps(p, p, 0x55));
	__m128 const x = _mm_mul_ps(transform->axes[0], _mm_shuffle_ps(p, p, 0x00));
	return _mm_add_ps(_mm_add_ps(x, y), z);
}

// a face of one of two boxes: its normal in both frames, which side it lies
// on and which axis it is (0 to 2 for the first box, 3 to 5 for the second)
struct face_306b90
{
	__m128 first_frame_normal;
	__m128 second_frame_normal;
	char unknown20[0x10];
	float side;
	char unknown34[4];
	unsigned short axis;
};

class box_306b90
{
public:
	char unknown0[0x20];
	__m128 second_direction;
	__m128 first_direction;
	char unknown40[0x30];
	transform_306b90 transform;
	__m128 axis_values_b0;
	__m128 axis_values_c0;

	void function_306b90(face_306b90 *face, int axis) const;
	void function_306d00(face_306b90 *face, int axis) const;
};

// score 9 (was 35): retail keeps the zero splat's shuffle and the second
// centre in xmm2 where ours uses xmm1, and its sign `and` writes the sum's
// register where ours writes the mask's (written sum first, our mask load
// moves to the top of the block).
// Zero/centre coloring and sign destination still differ.
// Native row loop 9; selected normal value 77; both 77; side result 31;
// row/side 31; normal/side 95; all three 95; intrinsic row with both 79.
// Scalar row array 36; scalar offset array 51; both 54; intrinsic row alone 95;
// intrinsic offset 82; aligned comparison read 9; aligned normal writes 9;
// aligned normal reads and writes 9. NOT kept: ties or larger differences.
// @retail 0x306b90
void box_306b90::function_306b90(face_306b90 *face, int axis) const
{
	face->axis = (unsigned short)axis;
	__m128 &normal = face->second_frame_normal;
	__m128 axis_direction;
	axis_direction.m128_f32[0] = transform.axes[0].m128_f32[axis];
	axis_direction.m128_f32[3] = 0.0f;
	axis_direction.m128_f32[1] = transform.axes[1].m128_f32[axis];
	axis_direction.m128_f32[2] = transform.axes[2].m128_f32[axis];
	if (axis_values_b0.m128_f32[axis] < 0.0f)
	{
		face->side = -1.0f;
		normal = first_direction;
	}
	else
	{
		face->side = 1.0f;
		normal = _mm_xor_ps(first_direction, *(__m128 const *)g_4124a0);
	}
	float const side_value = face->side;
	__m128 const positive = _mm_cmpgt_ps(axis_values_c0, splat_306b90(0.0f));
	__m128 const negative_tiny = {-1e-5f, -1e-5f, -1e-5f, 0.0f};
	__m128 const nudge = _mm_or_ps(_mm_and_ps(positive, splat_306b90(1e-5f)), _mm_andnot_ps(positive, negative_tiny));
	__m128 const signs = _mm_and_ps(*(__m128 const *)g_4124a0, _mm_add_ps(axis_direction, _mm_mul_ps(splat_306b90(side_value), nudge)));
	normal = _mm_xor_ps(normal, signs);
	face->first_frame_normal = transform_point_306b90(&transform, &face->second_frame_normal);
}

// score 13 (was 13): the zero comparison and sign extraction still use different vector registers.
// Earlier: as 0x306b90, the zero splat's shuffle and the first
// centre take xmm3 in retail (xmm2 here), and the sign `and` writes the sum's
// register there, the mask's here.
// Centre/zero coloring and nudge/sign destinations remain.
// Early comparison 34; native normal 82; both 94; side result 22;
// comparison/side 36; normal/side 96; all 102; comparison first 102.
// zero intrinsic 86; zero-left comparison 16; disjoint mask join 14; named mask terms 13;
// named terms and disjoint join 14; aligned axis load 81; aligned normal store 13;
// both vector memory boundaries 81.
// Scalar offset array 60; aligned comparison read 13; aligned branch reads 13;
// scalar axis array 134; axis reference 29; bit-difference selection 63;
// aligned inverse inputs 13; aligned second-frame write 13.
// NOT kept: ties or larger differences.
// @retail 0x306d00
void box_306b90::function_306d00(face_306b90 *face, int axis) const
{
	face->axis = (unsigned short)(axis + 3);
	__m128 const axis_direction = transform.axes[axis];
	if (axis_values_c0.m128_f32[axis] < 0.0f)
	{
		face->side = -1.0f;
		face->first_frame_normal = _mm_xor_ps(second_direction, *(__m128 const *)g_4124a0);
	}
	else
	{
		face->side = 1.0f;
		face->first_frame_normal = second_direction;
	}
	__m128 const positive = _mm_cmpgt_ps(axis_values_b0, splat_306b90(0.0f));
	__m128 const positive_tiny = {1e-5f, 1e-5f, 1e-5f, 0.0f};
	__m128 const nudge = _mm_or_ps(_mm_and_ps(positive, splat_306b90(-1e-5f)), _mm_andnot_ps(positive, positive_tiny));
	__m128 const signs = _mm_and_ps(*(__m128 const *)g_4124a0, _mm_add_ps(axis_direction, _mm_mul_ps(splat_306b90(-face->side), nudge)));
	__m128 const normal = _mm_xor_ps(face->first_frame_normal, signs);
	face->first_frame_normal = normal;
	// the normal's point seen from the second frame
	__m128 const d = _mm_sub_ps(normal, transform.position);
	__m128 const c0 = transform.axes[0];
	__m128 const c1 = transform.axes[1];
	__m128 const c2 = transform.axes[2];
	__m128 const t0 = _mm_shuffle_ps(c0, c1, 0x44);
	__m128 const t2 = _mm_shuffle_ps(c0, c1, 0xee);
	__m128 const t1 = _mm_shuffle_ps(c2, c2, 0x44);
	__m128 const t3 = _mm_shuffle_ps(c2, c2, 0xee);
	__m128 const row_0 = _mm_shuffle_ps(t0, t1, 0x88);
	__m128 const row_1 = _mm_shuffle_ps(t0, t1, 0xdd);
	__m128 const row_2 = _mm_shuffle_ps(t2, t3, 0x88);
	face->second_frame_normal = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(row_0, _mm_shuffle_ps(d, d, 0x00)),
		_mm_mul_ps(row_1, _mm_shuffle_ps(d, d, 0x55))),
		_mm_mul_ps(row_2, _mm_shuffle_ps(d, d, 0xaa)));
}
