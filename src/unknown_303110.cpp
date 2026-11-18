// @flags /O2 /GL-
// Havok: eliminate competing triangle-face scores around a tetrahedron.
#include <xmmintrin.h>
#include <float.h>
#include <string.h>

static const signed char g_cycles_4136b0[5] = {2, 0, 1, 2, 0};

inline float dot3_303110(__m128 const &a, __m128 const &b)
{
	__m128 product = _mm_mul_ps(a, b);
	__m128 sum = _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa),
		_mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product));
	float result;
	_mm_store_ss(&result, sum);
	return result;
}

// Internal linkage permits the compiler's library-local register convention.
// @retail 0x303110
static int __cdecl function_303110(__m128 const *vertices, __m128 const *point, float *scores)
{
	int face = 0;
	__m128 const *origin = vertices;
	for (; face < 3; ++face, ++origin)
	{
		int const next = g_cycles_4136b0[face + 2];
		int const previous = g_cycles_4136b0[face];
		if (scores[next] >= 0.0f && scores[previous] >= 0.0f)
		{
			__m128 const base = *origin;
			__m128 const edgeNext = _mm_sub_ps(vertices[next], base);
			__m128 const edgePrevious = _mm_sub_ps(vertices[previous], base);
			__m128 const query = _mm_sub_ps(*point, base);
			__m128 const apex = _mm_sub_ps(vertices[3], base);
			float projection[2][2];
			projection[0][0] = dot3_303110(query, edgeNext);
			projection[1][0] = dot3_303110(query, edgePrevious);
			projection[0][1] = dot3_303110(apex, edgeNext);
			projection[1][1] = dot3_303110(apex, edgePrevious);
			if (projection[0][0] * projection[1][1] < projection[1][0] * projection[0][1])
				scores[previous] = -1.0f;
			else
				scores[next] = -1.0f;
		}
	}
	if (scores[0] > 0.0f) return 0;
	if (scores[1] > 0.0f) return 1;
	if (scores[2] > 0.0f) return 2;
	return -1;
}

inline __m128 cross_303540(__m128 const &a, __m128 const &b)
{
	__m128 const b_yzx = _mm_shuffle_ps(b, b, 0xc9);
	__m128 const b_zxy = _mm_shuffle_ps(b, b, 0xd2);
	__m128 const a_zxy = _mm_shuffle_ps(a, a, 0xd2);
	__m128 const a_yzx = _mm_shuffle_ps(a, a, 0xc9);
	__m128 const second = _mm_mul_ps(a_zxy, b_yzx);
	return _mm_sub_ps(_mm_mul_ps(a_yzx, b_zxy), second);
}

// The tetrahedron reduction shared by the two convex-search functions below.
// Matched (was 111): it is static, and its two callers (0x303dd0, 0x3048a0)
// are in this file, so it takes retail's register inputs and 0x5c frame; the
// vertices are read through references, and a face's distance is set by an
// if/else.
// @retail 0x303540
static int __cdecl function_303540(__m128 const *point, __m128 const *vertices, float orientation)
{
	__m128 const &a = vertices[0];
	__m128 const &b = vertices[1];
	__m128 const &c = vertices[2];
	__m128 const &d = vertices[3];
	__m128 const cb = _mm_sub_ps(c, b);
	__m128 const ac = _mm_sub_ps(a, c);
	__m128 const ba = _mm_sub_ps(b, a);
	__m128 const da = _mm_sub_ps(d, a);
	__m128 const db = _mm_sub_ps(d, b);
	__m128 const dc = _mm_sub_ps(d, c);
	__m128 normals[3];
	normals[0] = cross_303540(cb, db);
	normals[1] = cross_303540(ac, dc);
	normals[2] = cross_303540(ba, da);
	__m128 const delta = _mm_sub_ps(*point, vertices[3]);
	__m128 const sign = _mm_set_ss(orientation);
	__m128 const query = _mm_mul_ps(_mm_shuffle_ps(sign, sign, 0), delta);
	__m128 const low = _mm_shuffle_ps(normals[0], normals[1], 0x44);
	__m128 const last_low = _mm_shuffle_ps(normals[2], normals[2], 0x44);
	__m128 const zs = _mm_shuffle_ps(_mm_shuffle_ps(normals[0], normals[1], 0xee),
		_mm_shuffle_ps(normals[2], normals[2], 0xee), 0x88);
	__m128 const ys = _mm_shuffle_ps(low, last_low, 0xdd);
	__m128 const xs = _mm_shuffle_ps(low, last_low, 0x88);
	__m128 const dots = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(xs, _mm_shuffle_ps(query, query, 0)),
		_mm_mul_ps(ys, _mm_shuffle_ps(query, query, 0x55))),
		_mm_mul_ps(zs, _mm_shuffle_ps(query, query, 0xaa)));
	static const unsigned long absolute_bits[4] = {0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff};
	__m128 absolute_mask;
	memcpy(&absolute_mask, absolute_bits, sizeof(absolute_mask));
	__m128 scores = _mm_mul_ps(dots, _mm_and_ps(dots, absolute_mask));
	float distances[3];
	for (int i = 0; i < 3; i++)
	{
		float const length = dot3_303110(normals[i], normals[i]);
		if (length != 0.0f) distances[i] = scores.m128_f32[i] / length;
		else distances[i] = FLT_MAX;
	}
	int best = 1;
	if (distances[0] > distances[1])
	{
		float const t = distances[0];
		distances[0] = distances[1];
		distances[1] = t;
		best = 0;
	}
	if (distances[1] > distances[2])
	{
		float const t = distances[1];
		distances[1] = distances[2];
		distances[2] = t;
		if (distances[0] > distances[1]) distances[1] = distances[0];
	}
	else best = 2;
	if (distances[2] < 1.0000001111620804e-6f) return -1;
	if (distances[2] > 1.1f * distances[1]) return best;
	scores = dots;
	return function_303110(vertices, point, scores.m128_f32);
}

#include "convex_search_internal.h"

// Havok: seed a penetration query from the current features, reduce its
// returned features, and retain its penetration direction.
struct transform_30a6b0;
long __cdecl function_30a6b0(long, long, transform_30a6b0 const *, long, long, long);
struct simplex_303dd0 { __m128 a[4], b[4]; int count; };
struct penetration_303dd0 { __m128 direction, point; float distance, fraction; };

// h2m 133 (was 642): the simplex is seeded one difference at a time; the
// last corner of the first shape is stored through the float view, so the
// second count is not known to be 3 when it is decremented; the transform loop
// reads the source points by index and steps only the target pointer, so no
// register holds &source_b. Left: the mask tests (retail reads the mask from
// memory in each single test) and the transform loop's pre-header (retail
// steps the source and writes the target through the difference).
// @retail 0x303dd0
int class_3048a0::function_303dd0(convex_2fefd0 const *first, convex_2fefd0 const *second, c_query_transform const *transform, pair_state_2fefd0 *state)
{
	search_features_303dd0 &s = *(search_features_303dd0 *)this;
	simplex_303dd0 simplex;
	__m128 differences[4];
	penetration_303dd0 penetration;
	simplex.count = 4;
	differences[0] = _mm_sub_ps(s.a[0],s.b[0]); simplex.a[0] = s.a[0]; simplex.b[0] = s.source_b[0];
	if (s.count_b >= 3)
	{
		differences[1] = _mm_sub_ps(s.a[0],s.b[1]); simplex.a[1] = s.a[0]; simplex.b[1] = s.source_b[1];
		differences[2] = _mm_sub_ps(s.a[0],s.b[2]); simplex.a[2] = s.a[0]; simplex.b[2] = s.source_b[2];
		if (s.count_b > 3) { differences[3] = _mm_sub_ps(s.a[0],s.b[3]); simplex.a[3] = s.a[0]; simplex.b[3] = s.source_b[3]; }
		else if (s.count_a > 1) { differences[3] = _mm_sub_ps(s.a[1],s.b[0]); simplex.a[3] = s.a[1]; simplex.b[3] = s.source_b[0]; }
		else simplex.count = 3;
	}
	else if (s.count_a >= 3)
	{
		differences[1] = _mm_sub_ps(s.a[1],s.b[0]); simplex.a[1] = s.a[1]; simplex.b[1] = s.source_b[0];
		differences[2] = _mm_sub_ps(s.a[2],s.b[0]); simplex.a[2] = s.a[2]; simplex.b[2] = s.source_b[0];
		if (s.count_a > 3) { differences[3] = _mm_sub_ps(s.a[3],s.b[0]); simplex.a[3] = s.a[3]; simplex.b[3] = s.source_b[0]; }
		else if (s.count_b > 1) { differences[3] = _mm_sub_ps(s.a[0],s.b[1]); simplex.a[3] = s.a[0]; simplex.b[3] = s.source_b[1]; }
		else simplex.count = 3;
	}
	else simplex.count = 1;
	int result = function_30a6b0((long)first,(long)second,(transform_30a6b0 const *)transform,(long)&simplex,(long)differences,(long)&penetration);
	int i;
	for (i=0; i<3; ++i) s.a[i] = simplex.a[i];
	for (i=0; i<3; ++i) s.source_b[i] = simplex.b[i];
	if (simplex.count == 1) {
		s.normal.m128_f32[0] = 1.0f; s.normal.m128_f32[1] = 0.0f; s.normal.m128_f32[2] = 0.0f; s.normal.m128_f32[3] = 0.0f;
		s.count_a = s.count_b = 1;
		return result;
	}
	s.count_a = s.count_b = 3;
	if (!(_mm_movemask_ps(_mm_cmplt_ps(g_4801e0,_mm_and_ps(_mm_sub_ps(s.a[1],s.a[2]),search_magnitude_mask()))) & 7) || !(_mm_movemask_ps(_mm_cmplt_ps(g_4801e0,_mm_and_ps(_mm_sub_ps(s.a[0],s.a[2]),search_magnitude_mask()))) & 7)) s.count_a = 2;
	if (!(_mm_movemask_ps(_mm_cmplt_ps(g_4801e0,_mm_and_ps(_mm_sub_ps(s.a[0],s.a[1]),search_magnitude_mask()))) & 7)) { __m128 const &last = s.a[--s.count_a]; _mm_store_ps(s.a[0].m128_f32, last); }
	if (!(_mm_movemask_ps(_mm_cmplt_ps(g_4801e0,_mm_and_ps(_mm_sub_ps(s.source_b[1],s.source_b[2]),search_magnitude_mask()))) & 7) || !(_mm_movemask_ps(_mm_cmplt_ps(g_4801e0,_mm_and_ps(_mm_sub_ps(s.source_b[0],s.source_b[2]),search_magnitude_mask()))) & 7)) --s.count_b;
	if (s.count_b >= 2 && !(_mm_movemask_ps(_mm_cmplt_ps(g_4801e0,_mm_and_ps(_mm_sub_ps(s.source_b[0],s.source_b[1]),search_magnitude_mask()))) & 7)) { __m128 const &last = s.source_b[--s.count_b]; s.source_b[0] = last; }
	{ __m128 *target = s.b; int k = 0; for (i = s.count_b - 1; i >= 0; --i) { *target = search_transform_ordered(transform,&s.source_b[k]); ++target; ++k; } }
	if (s.count_b+s.count_a > 4) ((class_303360 *)this)->function_303b80();
	s.restricted_side = 0;
	search_reduce_features(s);
	s.normal = search_negate(penetration.direction);
	return result;
}
// Havok: reduce cached convex features, alternate supporting-point queries,
// and return the separating direction and signed distance with updated IDs.

// Only the two observed query slots are called through this interface view.
class search_shape_3048a0
{
public:
	virtual void slot_0() = 0; virtual void slot_1() = 0;
	virtual void slot_2() = 0; virtual void slot_3() = 0;
	virtual void slot_4() = 0; virtual void slot_5() = 0;
	virtual void slot_6() = 0; virtual void slot_7() = 0;
	virtual void slot_8() = 0; virtual void slot_9() = 0;
	virtual void slot_10() = 0; virtual void slot_11() = 0;
	virtual void support(__m128 const *, __m128 *) const = 0;
	virtual int vertices(unsigned short const *, int, __m128 *) const = 0;
};
inline void search_support(convex_2fefd0 const *shape, __m128 const *direction, __m128 *out)
{
	((search_shape_3048a0 const *)shape)->support(direction,out);
}
inline int search_vertices(convex_2fefd0 const *shape, __m128 *out, int count, unsigned short const *ids)
{
	return ((search_shape_3048a0 const *)shape)->vertices(ids,count,out);
}
__forceinline bool search_new_point(__m128 const &first_corner, __m128 const &second_corner, __m128 const &third_corner, __m128 const &point, int count, __m128 const &direction, float threshold)
{
	__m128 const along = direction;
	__m128 const added = point;
	float first = search_dot(_mm_sub_ps(added,first_corner),along);
	if (!(fabs(first)*first > threshold)) return false;
	switch (count)
	{
	case 3:
	{
		float third = search_dot(_mm_sub_ps(added,third_corner),along);
		if (fabs(third)*third < threshold) return false;
	}
	case 2:
	{
		float second = search_dot(_mm_sub_ps(added,second_corner),along);
		if (fabs(second)*second < threshold) return false;
	}
	}
	return true;
}

// MATCH (was 58): the reduction switches on each query's result in its own
// case, the separating results jump to its shared exit, and the source
// pointer receives the second shape's vertices before it is reset for the
// transform loop, after the target pointer.
// @retail 0x3048a0
int class_3048a0::function_3048a0(convex_2fefd0 const *first, convex_2fefd0 const *second, c_query_transform const *transform, pair_state_2fefd0 *state, closest_3048a0 *closest)
{
	search_features_303dd0 &s = *(search_features_303dd0 *)this;
	signed char *counts = (signed char *)state;
	unsigned short *ids = (unsigned short *)state;
	int iteration = 0;
	float tolerance = 1.0e-10f;
	s.count_a = counts[8]; s.count_b = counts[9];
	((search_shape_3048a0 const *)first)->vertices(ids,s.count_a,s.a);
	__m128 *source = s.source_b;
	((search_shape_3048a0 const *)second)->vertices(ids+s.count_a,s.count_b,source);
	{ __m128 *target = s.b; source = s.source_b; for (int i = s.count_b - 1; i >= 0; --i) { *target = search_transform_ordered(transform,source); ++source; ++target; } }
	int result = 0;
	for (;;)
	{
		__m128 direction;
		tolerance += tolerance;
		++iteration;
		s.restricted_side = 0;
		int overlap = search_reduce_features(s);
		float length_squared = search_dot(s.normal,s.normal);
		__m128 const &normal = s.normal;
		float separation = search_dot(_mm_sub_ps(s.b[0],s.a[0]),normal);
		if (overlap || separation*separation*separation <= (1.1920928955078125e-7f*tolerance)*length_squared)
		{
			if (!s.skip_penetration) result = function_303dd0(first,second,transform,state);
			else {
				if (s.count_a+s.count_b > 4) {
					if (s.count_a > s.count_b) { s.count_a=3; s.count_b=1; }
					else { s.count_a=1; s.count_b=3; }
				}
				result = 3;
			}
			break;
		}
		float threshold = length_squared*tolerance;
		if ((iteration & 1) && s.count_a != counts[10] && s.restricted_side != 1)
		{
			search_support(first,&s.normal,&s.a[s.count_a]);
			if (search_new_point(s.a[0],s.a[1],s.a[2],s.a[s.count_a],s.count_a,s.normal,threshold)) { ++s.count_a; continue; }
		}
		if (s.count_b != counts[11] && s.restricted_side != -1)
		{
			direction = search_inverse_direction(transform,s.normal);
			search_support(second,&direction,&s.source_b[s.count_b]);
			if (search_new_point(s.source_b[0],s.source_b[1],s.source_b[2],s.source_b[s.count_b],s.count_b,direction,threshold)) {
				s.b[s.count_b] = search_transform(transform,s.source_b[s.count_b]);
				int &count = s.count_b;
				++count;
				continue;
			}
		}
		if (!(iteration & 1) && s.count_a != counts[10] && s.restricted_side != 1)
		{
			search_support(first,&s.normal,&s.a[s.count_a]);
			if (search_new_point(s.a[0],s.a[1],s.a[2],s.a[s.count_a],s.count_a,s.normal,threshold)) { ++s.count_a; continue; }
		}
		break;
	}
	__m128 const &normal = s.normal;
	__m128 const projected = _mm_mul_ps(_mm_sub_ps(s.b[0],s.a[0]),normal);
	__m128 length = search_sum(_mm_mul_ps(s.normal,s.normal));
	__m128 estimate = _mm_rsqrt_ss(length);
	__m128 correction = _mm_sub_ss(_mm_set_ss(3.0f),_mm_mul_ss(_mm_mul_ss(length,estimate),estimate));
	__m128 inverse = _mm_mul_ss(_mm_mul_ss(_mm_set_ss(0.5f),estimate),correction);
	closest->point = _mm_mul_ps(_mm_shuffle_ps(inverse,inverse,0),normal);
	float distance;
	_mm_store_ss(&distance,_mm_mul_ss(search_sum(projected),inverse));
	closest->point.m128_f32[3] = distance;
	counts[8] = (signed char)s.count_a; counts[9] = (signed char)s.count_b;
	int j=0;
	do { ids[j] = *(unsigned short const *)&s.a[j].m128_f32[3]; ++j; } while (j<s.count_a);
	int k=0;
	do { ids[j++] = *(unsigned short const *)&s.source_b[k].m128_f32[3]; ++k; } while (k<s.count_b);
	return result;
}

