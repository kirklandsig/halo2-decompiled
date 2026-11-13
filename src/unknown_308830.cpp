// @flags /O2 /GL-
// Havok (third-party physics library): triangle and segment queries: the
// point of a segment closest to a given point, with which part of the segment
// it is; the terms that solve for a point's weights in a triangle's plane; a
// triangle's edge and area measures; its weighted edge planes as a matrix;
// and the closest point of a triangle to a point.
#include "point_triangle_308d90.h"
#include <math.h>
#include <xmmintrin.h>

// three-term dot product of two vectors
inline void dot3_308830(float *result, __m128 const &a, __m128 const &b)
{
	__m128 const product = _mm_mul_ps(a, b);
	_mm_store_ss(result, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
}

// out: start (returns 8) when the point projects before it, end (returns 4)
// when it projects past it, else the projection itself (returns 0)
// @retail 0x308830
long __cdecl function_308830(__m128 const &point, __m128 const &start, __m128 const &end, __m128 &out)
{
	__m128 const b = start;
	__m128 const direction = _mm_sub_ps(end, b);
	float along;
	dot3_308830(&along, direction, _mm_sub_ps(b, point));
	float length_squared;
	dot3_308830(&length_squared, direction, direction);
	if ((along = -along) <= 0.0f)
	{
		out = b;
		return 8;
	}
	if (along >= length_squared)
	{
		out = _mm_add_ps(b, direction);
		return 4;
	}
	__m128 const single = _mm_set_ss(along / length_squared);
	out = _mm_add_ps(b, _mm_mul_ps(_mm_shuffle_ps(single, single, 0), direction));
	return 0;
}

// the cross product of two vectors' first three terms
inline __m128 cross_308910(__m128 const &a, __m128 const &b)
{
	return _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(a, a, 0xc9), _mm_shuffle_ps(b, b, 0xd2)),
		_mm_mul_ps(_mm_shuffle_ps(a, a, 0xd2), _mm_shuffle_ps(b, b, 0xc9)));
}

// a vector's first term
inline float first_term_308910(__m128 v)
{
	float result;
	_mm_store_ss(&result, v);
	return result;
}

// a triangle's two edges from its second corner, as the terms that solve for
// a point's weights in its plane: the edges' squared lengths and their dot
// product over the determinant, then one over the length of their cross
// product
// score 15 (was 27): division and final scalar spill still run at different
// points. Earlier: separate product 24, double inverse 15, vector memory
// update 61, scalar root 46, late scalar 15, local renames 15, vector scopes
// 15, direct lane 23, output scaling loop 59. This round: cross inputs by
// value (both or either) 15, cross input order 15, dot inputs by value 15,
// array-element reference 15, double product 15, outer result scope 15.
// Further history: inverse/shuffle and final scalar extraction still differ.
// Output dot storage a 70; b 65; a/b 88; a/dot 59; b/dot 51; all 87;
// full output storage with cursor 84; original scalars with output cursor 39.
// Existing scalar-return helpers 15/15; existing sum helpers 15/15; existing
// cross helper 15; dot output reference 15; dot float return 15; helper sum
// call 15. All restored.
// @retail 0x308910
void __cdecl function_308910(__m128 const *corners, float *out)
{
	__m128 const edge_a = _mm_sub_ps(corners[0], corners[1]);
	__m128 const edge_b = _mm_sub_ps(corners[2], corners[1]);
	float length_a_squared;
	dot3_308830(&length_a_squared, edge_a, edge_a);
	float length_b_squared;
	dot3_308830(&length_b_squared, edge_b, edge_b);
	float const squared_product = length_b_squared * length_a_squared;
	float edges_dot;
	dot3_308830(&edges_dot, edge_b, edge_a);
	double const inverse = 1.0f / (squared_product - edges_dot * edges_dot);
	__m128 const normal = cross_308910(edge_a, edge_b);
	__m128 const squares = _mm_mul_ps(normal, normal);
	__m128 const length = _mm_sqrt_ss(_mm_add_ss(_mm_shuffle_ps(squares, squares, 0xaa), _mm_add_ss(_mm_shuffle_ps(squares, squares, 0x55), squares)));
	out[0] = length_a_squared * inverse;
	out[1] = length_b_squared * inverse;
	out[2] = edges_dot * inverse;
	out[3] = 1.0f / first_term_308910(length);
}

struct triangle_measures_308a50
{
	float inverse_edge_lengths[3];
	float inverse_double_area;
	float double_area;
};

// each inverse length is the estimate refined by one Newton step
// @retail 0x308a50
void __cdecl function_308a50(__m128 const *vertices, triangle_measures_308a50 *measures)
{
	__m128 const edge_0 = _mm_sub_ps(vertices[2], vertices[1]);
	__m128 const edge_2 = _mm_sub_ps(vertices[0], vertices[2]);
	__m128 const edge_1 = _mm_sub_ps(vertices[1], vertices[0]);

	__m128 squares = _mm_mul_ps(edge_0, edge_0);
	__m128 sum = _mm_add_ss(_mm_shuffle_ps(squares, squares, 0xaa), _mm_add_ss(_mm_shuffle_ps(squares, squares, 0x55), squares));
	__m128 estimate = _mm_rsqrt_ss(sum);
	__m128 correction = _mm_sub_ss(_mm_set_ss(3.0f), _mm_mul_ss(_mm_mul_ss(sum, estimate), estimate));
	float first_inverse_length;
	_mm_store_ss(&first_inverse_length, _mm_mul_ss(_mm_mul_ss(_mm_set_ss(0.5f), estimate), correction));
	measures->inverse_edge_lengths[0] = first_inverse_length;

	squares = _mm_mul_ps(edge_2, edge_2);
	sum = _mm_add_ss(_mm_shuffle_ps(squares, squares, 0xaa), _mm_add_ss(_mm_shuffle_ps(squares, squares, 0x55), squares));
	estimate = _mm_rsqrt_ss(sum);
	correction = _mm_sub_ss(_mm_set_ss(3.0f), _mm_mul_ss(_mm_mul_ss(sum, estimate), estimate));
	float second_inverse_length;
	_mm_store_ss(&second_inverse_length, _mm_mul_ss(_mm_mul_ss(_mm_set_ss(0.5f), estimate), correction));
	measures->inverse_edge_lengths[1] = second_inverse_length;

	squares = _mm_mul_ps(edge_1, edge_1);
	sum = _mm_add_ss(_mm_shuffle_ps(squares, squares, 0xaa), _mm_add_ss(_mm_shuffle_ps(squares, squares, 0x55), squares));
	estimate = _mm_rsqrt_ss(sum);
	correction = _mm_sub_ss(_mm_set_ss(3.0f), _mm_mul_ss(_mm_mul_ss(sum, estimate), estimate));
	float third_inverse_length;
	_mm_store_ss(&third_inverse_length, _mm_mul_ss(_mm_mul_ss(_mm_set_ss(0.5f), estimate), correction));
	measures->inverse_edge_lengths[2] = third_inverse_length;

	__m128 const normal = _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(edge_0, edge_0, 0xc9), _mm_shuffle_ps(edge_2, edge_2, 0xd2)),
		_mm_mul_ps(_mm_shuffle_ps(edge_0, edge_0, 0xd2), _mm_shuffle_ps(edge_2, edge_2, 0xc9)));
	squares = _mm_mul_ps(normal, normal);
	__m128 const area_squared = _mm_add_ss(_mm_shuffle_ps(squares, squares, 0xaa), _mm_add_ss(_mm_shuffle_ps(squares, squares, 0x55), squares));
	float double_area;
	_mm_store_ss(&double_area, _mm_sqrt_ss(area_squared));
	measures->inverse_double_area = 1.0f / double_area;
	measures->double_area = double_area;
}

// the cross product of two vectors' first three terms
inline __m128 cross_308c20(__m128 const &a, __m128 const &b)
{
	return _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(a, a, 0xc9), _mm_shuffle_ps(b, b, 0xd2)),
		_mm_mul_ps(_mm_shuffle_ps(a, a, 0xd2), _mm_shuffle_ps(b, b, 0xc9)));
}

// a vector of four copies of a float
inline __m128 splat_308c20(float value)
{
	__m128 const single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

// two floats exchanged
inline void swap_308c20(float &a, float &b)
{
	float const old_a = a;
	float const old_b = b;
	a = old_b;
	b = old_a;
}

// the weight of each edge, of the normal, and of the last row
struct weights_308c20
{
	float edges[3];
	float normal;
	float last;
};

// a vector set in place
struct vector_308c20
{
	__m128 value;

	void set_cross(__m128 const &a, __m128 const &b) { value = cross_308c20(a, b); }
};

struct matrix_308c20
{
	vector_308c20 rows[4];

	vector_308c20 &column(int index) { return rows[index]; }
};

// @retail 0x308c20
void __cdecl function_308c20(__m128 const *corners, weights_308c20 const *weights, matrix_308c20 *out, __m128 *normal)
{
	__m128 const edge_0 = _mm_sub_ps(corners[2], corners[1]);
	__m128 const edge_1 = _mm_sub_ps(corners[0], corners[2]);
	__m128 const edge_2 = _mm_sub_ps(corners[1], corners[0]);
	__m128 const scaled_normal = _mm_mul_ps(splat_308c20(weights->normal), cross_308c20(edge_0, edge_1));
	*normal = scaled_normal;
	__m128 const weighted_0 = _mm_mul_ps(splat_308c20(weights->edges[0]), edge_0);
	__m128 const weighted_1 = _mm_mul_ps(splat_308c20(weights->edges[1]), edge_1);
	__m128 const weighted_2 = _mm_mul_ps(splat_308c20(weights->edges[2]), edge_2);
	out->column(0).set_cross(weighted_0, scaled_normal);
	out->column(1).set_cross(weighted_1, scaled_normal);
	out->column(2).set_cross(weighted_2, scaled_normal);
	float *terms = (float *)out->rows;
	swap_308c20(terms[4], terms[1]);
	swap_308c20(terms[8], terms[2]);
	swap_308c20(terms[9], terms[6]);
	float const *normal_terms = (float const *)normal;
	terms[3] = normal_terms[0];
	terms[7] = normal_terms[1];
	terms[11] = normal_terms[2];
	out->column(3).value = _mm_setzero_ps();
	terms[12] = -(weights->last * weights->edges[0]);
}

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

// a vector of four copies of a float
inline __m128 splat_308d90(float value)
{
	__m128 const single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

// a vector of zeros the weights are compared with
__m128 g_4801e0;

// for each set of weights inside the triangle (a bit per corner), the corner
// to measure from (0 to 2), or the edge (-8 to -6), or -1
static const signed char g_4136a8[8] = {-1, 0, 1, -6, 2, -7, -8, -1};

// corner indices: the corner before a corner at [i], and after it at [i + 2]
// The zero-mask path also reads entries 7 and 9.
static const signed char g_4136b0[10] = {2, 0, 1, 2, 0, 0, 0, 0, 0, 0};

// the dot product of two vectors' first three terms
__forceinline float dot3_308d90(__m128 const &a, __m128 const &b)
{
	__m128 const product = _mm_mul_ps(a, b);
	float result;
	_mm_store_ss(&result, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
	return result;
}

// one over the square root of the first term, refined by a Newton step, in
// all four terms
__forceinline __m128 inverse_sqrt_308d90(__m128 const &value)
{
	__m128 const estimate = _mm_rsqrt_ss(value);
	__m128 const product = _mm_mul_ss(_mm_mul_ss(value, estimate), estimate);
	__m128 const refined = _mm_mul_ss(_mm_mul_ss(_mm_set_ss(0.5f), estimate), _mm_sub_ss(_mm_set_ss(3.0f), product));
	return _mm_shuffle_ps(refined, refined, 0);
}

// 0 when the point lies over the triangle (the normal faces the point), 1
// when it is nearest an edge or a corner
// @retail 0x308d90
int __cdecl function_308d90(__m128 const *point, __m128 const *corners, float const *terms, closest_308d90 *out)
{
	__m128 const edge_0 = _mm_sub_ps(corners[0], corners[1]);
	__m128 const to_corner = _mm_sub_ps(corners[1], *point);
	float const along_0 = dot3_308d90(to_corner, edge_0);
	__m128 const edge_2 = _mm_sub_ps(corners[2], corners[1]);
	float const along_2 = dot3_308d90(to_corner, edge_2);
	float const u = along_2 * terms[2] - along_0 * terms[1];
	float const v = along_0 * terms[2] - along_2 * terms[0];
	union
	{
		__m128 vector;
		float terms[4];
	} weights;
	weights.terms[0] = u + 0.001f;
	weights.terms[1] = 1.001f - u - v;
	weights.terms[2] = v + 0.001f;
	weights.terms[3] = 0.001f;
	int const inside = _mm_movemask_ps(_mm_cmplt_ps(g_4801e0, weights.vector)) & 7;
	if (inside == 7)
	{
		__m128 const normal = cross_308910(edge_0, edge_2);
		out->normal = normal;
		__m128 const scaled = _mm_mul_ps(splat_308d90(terms[3]), normal);
		float const distance = dot3_308d90(scaled, to_corner);
		out->normal = scaled;
		out->distance = distance;
		if (distance > 0.0f)
		{
			out->normal = _mm_xor_ps(scaled, *(__m128 const *)g_4124a0);
		}
		else
		{
			out->distance = -distance;
		}
		return 0;
	}
	int index = g_4136a8[inside];
	if (index < 0)
	{
		index += 8;
		__m128 nearest;
		function_308830(*point, corners[g_4136b0[index + 2]], corners[g_4136b0[index]], nearest);
		__m128 const offset = _mm_sub_ps(*point, nearest);
		__m128 const squares = _mm_mul_ps(offset, offset);
		__m128 const length_squared = _mm_add_ss(_mm_shuffle_ps(squares, squares, 0xaa), _mm_add_ss(_mm_shuffle_ps(squares, squares, 0x55), squares));
		__m128 const inverse = inverse_sqrt_308d90(length_squared);
		out->normal = _mm_mul_ps(inverse, offset);
		out->distance = first_term_308910(_mm_mul_ss(length_squared, inverse));
		return 1;
	}
	__m128 first;
	function_308830(*point, corners[index], corners[g_4136b0[index + 2]], first);
	__m128 second;
	function_308830(*point, corners[g_4136b0[index]], corners[index], second);
	__m128 const first_offset = _mm_sub_ps(*point, first);
	__m128 const second_offset = _mm_sub_ps(*point, second);
	float const first_squared = dot3_308d90(first_offset, first_offset);
	float const second_squared = dot3_308d90(second_offset, second_offset);
	if (first_squared < second_squared)
	{
		float const inverse = 1.0f / (float)sqrt(first_squared);
		out->distance = first_squared * inverse;
		out->normal = _mm_mul_ps(splat_308d90(inverse), first_offset);
	}
	else
	{
		float const inverse = 1.0f / (float)sqrt(second_squared);
		out->distance = second_squared * inverse;
		out->normal = _mm_mul_ps(splat_308d90(inverse), second_offset);
	}
	return 1;
}
