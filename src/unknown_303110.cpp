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

// The real local caller preserves the helper's register convention. This
// larger routine remains a near reconstruction, not an additional solve.
// @retail 0x303540
static int __cdecl function_303540(__m128 const *vertices, __m128 const *point, float orientation)
{
	__m128 const a = vertices[0];
	__m128 const b = vertices[1];
	__m128 const d = vertices[3];
	__m128 const c = vertices[2];
	__m128 const ba = _mm_sub_ps(b, a);
	__m128 const ac = _mm_sub_ps(a, c);
	__m128 const da = _mm_sub_ps(d, a);
	__m128 const db = _mm_sub_ps(d, b);
	__m128 const dc = _mm_sub_ps(d, c);
	__m128 const cb = _mm_sub_ps(c, b);
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
		distances[i] = length != 0.0f ? scores.m128_f32[i] / length : FLT_MAX;
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
