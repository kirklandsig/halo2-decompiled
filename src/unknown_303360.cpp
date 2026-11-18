// @flags /O2 /GL-
// Havok (third-party physics library): which sides of a triangle's three
// edges a point lies inside, as three sign bits; and, when asked, the
// triangle's normal kept and the triangle turned round when the point lies
// over all three edges but the normal faces away from the search direction.
// Also a segment against a triangle (0x3037a0), which is here rather than in
// a file of its own because retail calls 0x303360 from it without saving
// `this`: the compiler knew 0x303360 leaves ecx alone, as it does only for a
// function earlier in the same file.
#include <math.h>
#include <xmmintrin.h>
#include "search_flag_3037a0.h"

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
static __declspec(align(16)) const float g_ones_412420[4] = {1.0f, 1.0f, 1.0f, 1.0f};

// the closest point on the first segment, the offset from the closest point
// on the second, and the square of that offset's length (0x308590)
struct closest_points_308590
{
	__m128 first_point;
	__m128 offset;
	float distance_squared;
};

int __cdecl function_308590(__m128 const *first_start, __m128 const *first_direction, __m128 const *second_start, __m128 const *second_direction, closest_points_308590 *result);


// the cross product of two vectors' first three terms
inline __m128 cross_303360(__m128 const &a, __m128 const &b)
{
	__m128 b_yzx = _mm_shuffle_ps(b, b, 0xc9);
	__m128 b_zxy = _mm_shuffle_ps(b, b, 0xd2);
	__m128 a_zxy = _mm_shuffle_ps(a, a, 0xd2);
	__m128 a_yzx = _mm_shuffle_ps(a, a, 0xc9);
	__m128 second = _mm_mul_ps(a_zxy, b_yzx);
	return _mm_sub_ps(_mm_mul_ps(a_yzx, b_zxy), second);
}

inline void swap_vectors_303360(__m128 &x, __m128 &y)
{
	__m128 t = x;
	x = y;
	y = t;
}

inline void swap_floats_303360(float &x, float &y)
{
	float t = x;
	x = y;
	y = t;
}

struct triangle_303360
{
	__m128 vertices[3];
	__m128 field_30;
	__m128 field_40;
	__m128 field_50;
};

// a segment's two ends, and where each came from
struct segment_3037a0
{
	__m128 ends[2];
	char unknown20[0x40 - 0x20];
	__m128 sources[2];
};

// a triangle's corners, and where each came from
struct triangle_3037a0
{
	__m128 corners[3];
	__m128 field_30;
	__m128 sources[3];
};

// the sum of a vector's first three terms
inline __m128 sum_3037a0(__m128 const &v)
{
	return _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v));
}

// the point a fraction of the way between two points
inline __m128 between_3037a0(__m128 const &from, __m128 const &to, __m128 const &fraction)
{
	return _mm_add_ps(_mm_mul_ps(_mm_sub_ps(*(__m128 const *)g_ones_412420, fraction), from), _mm_mul_ps(fraction, to));
}

class class_303360
{
public:
	char unknown0[0x10];
	long field_10;
	char unknown14[0x20 - 0x14];
	__m128 field_20;
	char unknown30[0xa0 - 0x30];
	__m128 field_a0;
	char unknownb0[0x120 - 0xb0];
	__m128 normal;
	char unknown130[0x160 - 0x130];
	union
	{
		__m128 edge_dots;
		float edge_dot[4];
	};

	int function_303360(__m128 const *point, triangle_303360 *triangle, int orient);
	int function_3037a0(segment_3037a0 *segment, triangle_3037a0 *triangle, int *segment_count, int *facet_total, search_flag_3037a0 own_plane, long value);
};

// @retail 0x303360
int class_303360::function_303360(__m128 const *point, triangle_303360 *triangle, int orient)
{
	__m128 a = triangle->vertices[0];
	__m128 b = triangle->vertices[1];
	__m128 c = triangle->vertices[2];
	__m128 cb = _mm_sub_ps(c, b);
	__m128 ac = _mm_sub_ps(a, c);
	__m128 ba = _mm_sub_ps(b, a);
	__m128 n = cross_303360(cb, ac);
	__m128 p = *point;
	__m128 across_cb = cross_303360(_mm_sub_ps(p, b), cb);
	__m128 across_ac = cross_303360(_mm_sub_ps(p, triangle->vertices[2]), ac);
	__m128 across_ba = cross_303360(_mm_sub_ps(p, triangle->vertices[0]), ba);

	// the three cross products' dot products with the normal
	__m128 low = _mm_shuffle_ps(across_cb, across_ac, 0x44);
	__m128 last_low = _mm_shuffle_ps(across_ba, across_ba, 0x44);
	__m128 zs = _mm_shuffle_ps(_mm_shuffle_ps(across_cb, across_ac, 0xee), _mm_shuffle_ps(across_ba, across_ba, 0xee), 0x88);
	__m128 ys = _mm_shuffle_ps(low, last_low, 0xdd);
	__m128 xs = _mm_shuffle_ps(low, last_low, 0x88);
	__m128 dots = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(xs, _mm_shuffle_ps(n, n, 0x00)),
		_mm_mul_ps(ys, _mm_shuffle_ps(n, n, 0x55))),
		_mm_mul_ps(zs, _mm_shuffle_ps(n, n, 0xaa)));

	int outside = _mm_movemask_ps(dots) & 7;
	edge_dots = dots;
	if (orient)
	{
		normal = n;
		if (outside == 7)
		{
			__m128 d = _mm_mul_ps(n, _mm_sub_ps(field_a0, field_20));
			__m128 sum = _mm_add_ss(_mm_shuffle_ps(d, d, 0xaa), _mm_add_ss(_mm_shuffle_ps(d, d, 0x55), d));
			if (_mm_movemask_ps(sum) & 1)
			{
				normal = _mm_xor_ps(n, *(__m128 const *)g_4124a0);
				swap_vectors_303360(triangle->vertices[0], triangle->vertices[1]);
				swap_vectors_303360(triangle->field_40, triangle->field_50);
				swap_floats_303360(edge_dot[0], edge_dot[1]);
			}
		}
	}
	return outside;
}

// 1 when the segment crosses the triangle inside it (the crossing is the
// search's point), 0 when one end of the segment is dropped, 2 when one
// corner of the triangle is
// @retail 0x3037a0
int class_303360::function_3037a0(segment_3037a0 *segment, triangle_3037a0 *triangle, int *segment_count, int *facet_total, search_flag_3037a0 own_plane, long value)
{
	if (own_plane.value)
	{
		__m128 const second_edge = _mm_sub_ps(triangle->corners[1], triangle->corners[2]);
		__m128 const first_edge = _mm_sub_ps(triangle->corners[0], triangle->corners[2]);
		__m128 const plane = cross_303360(second_edge, first_edge);
		__m128 const start = segment->ends[0];
		__m128 const start_height = sum_3037a0(_mm_mul_ps(plane, _mm_sub_ps(start, triangle->corners[0])));
		__m128 const end = segment->ends[1];
		__m128 const end_height = sum_3037a0(_mm_mul_ps(plane, _mm_sub_ps(end, triangle->corners[0])));
		if ((_mm_movemask_ps(end_height) ^ _mm_movemask_ps(start_height)) & 1)
		{
			__m128 fraction = _mm_div_ss(start_height, _mm_sub_ss(start_height, end_height));
			fraction = _mm_shuffle_ps(fraction, fraction, 0);
			__m128 crossing = between_3037a0(start, end, fraction);
			if (function_303360(&crossing, (triangle_303360 *)triangle, 0) == 7)
			{
				return 1;
			}
		}
		else
		{
			float start_value;
			_mm_store_ss(&start_value, start_height);
			float end_value;
			_mm_store_ss(&end_value, end_height);
			if (fabs(start_value) > fabs(end_value))
			{
				swap_vectors_303360(segment->ends[0], segment->ends[1]);
				swap_vectors_303360(segment->sources[0], segment->sources[1]);
			}
			if (function_303360(&segment->ends[0], (triangle_303360 *)triangle, 1) == 7)
			{
				(*segment_count)--;
				return 0;
			}
		}
		int dropped;
		{
			__m128 direction = _mm_sub_ps(segment->ends[1], segment->ends[0]);
			closest_points_308590 to_second;
			function_308590(&segment->ends[0], &direction, &triangle->corners[2], &second_edge, &to_second);
			closest_points_308590 to_first;
			function_308590(&segment->ends[0], &direction, &triangle->corners[2], &first_edge, &to_first);
			dropped = to_second.distance_squared > to_first.distance_squared ? 1 : 0;
		}
		triangle->corners[dropped] = triangle->corners[2];
		triangle->sources[dropped] = triangle->sources[2];
		(*facet_total)--;
		return 2;
	}
	__m128 const old_dots = edge_dots;
	__m128 const corner = triangle->corners[0];
	__m128 const end = segment->ends[1];
	__m128 const end_height = sum_3037a0(_mm_mul_ps(normal, _mm_sub_ps(end, corner)));
	__m128 const start = segment->ends[0];
	__m128 const start_height = sum_3037a0(_mm_mul_ps(normal, _mm_sub_ps(start, corner)));
	if (_mm_movemask_ps(_mm_mul_ss(start_height, end_height)) & 1)
	{
		__m128 fraction = _mm_div_ss(start_height, _mm_sub_ss(start_height, end_height));
		fraction = _mm_shuffle_ps(fraction, fraction, 0);
		__m128 crossing = between_3037a0(start, end, fraction);
		if (function_303360(&crossing, (triangle_303360 *)triangle, 0) == 7)
		{
			return 1;
		}
	}
	int const inside = function_303360(&segment->ends[1], (triangle_303360 *)triangle, 0);
	if (inside == 7)
	{
		(*segment_count)--;
		segment->ends[0] = segment->ends[1];
		segment->sources[0] = segment->sources[1];
		field_10 = value;
		return 0;
	}
	float const *old_dot = old_dots.m128_f32;
	if (inside == 6)
	{
		goto drop_first;
	}
	if (inside == 5)
	{
		goto drop_second;
	}
	if (inside == 3)
	{
		goto drop_third;
	}
	if (inside == 1)
	{
		if ((edge_dot[1] - old_dot[1]) * old_dot[2] > (edge_dot[2] - old_dot[2]) * old_dot[1])
		{
			goto drop_third;
		}
		goto drop_second;
	}
	if (inside == 2)
	{
		if ((edge_dot[2] - old_dot[2]) * old_dot[0] > (edge_dot[0] - old_dot[0]) * old_dot[2])
		{
			goto drop_first;
		}
		goto drop_third;
	}
	if (inside == 4)
	{
		if ((edge_dot[0] - old_dot[0]) * old_dot[1] > (edge_dot[1] - old_dot[1]) * old_dot[0])
		{
			goto drop_second;
		}
	}
drop_first:
	(*facet_total)--;
	triangle->corners[0] = triangle->corners[2];
	triangle->sources[0] = triangle->sources[2];
	return 2;
drop_second:
	(*facet_total)--;
	triangle->corners[1] = triangle->corners[2];
	triangle->sources[1] = triangle->sources[2];
	return 2;
drop_third:
	(*facet_total)--;
	return 2;
}
