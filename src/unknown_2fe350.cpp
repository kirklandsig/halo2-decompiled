// @flags /O2 /GL-
// Havok (third-party physics library): the bounds of a box placed by a
// transform: each axis's half extent turned by the transform, their absolute
// values summed and widened by a tolerance, about the transform's origin.
#include <xmmintrin.h>

static __declspec(align(16)) const unsigned long g_455940[4] = {0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff};
static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

// a rotation's three columns and a translation
struct transform_2fe350
{
	__m128 columns[3];
	__m128 translation;
};

// the lower and upper corners
struct bounds_2fe350
{
	__m128 lower;
	__m128 upper;
};

// score 31 (was 31): the same operations; retail reads the half extents before
// the transform and the tolerance later, and keeps the mask in one register.
// Products split from absolute 31; mask after third product 31; all three
// products before absolute 31; broadcast intrinsic 43; half extents first 31;
// mask at each use 31; delayed third absolute 31; separate splat steps 31.
// NOT kept: no match or qualifying gain; original body restored.
// @retail 0x2fe350
void __cdecl function_2fe350(void const *query, const __m128 *half_extents, float tolerance, void *out)
{
	transform_2fe350 const *transform = (transform_2fe350 const *)query;
	bounds_2fe350 *bounds = (bounds_2fe350 *)out;
	__m128 const absolute = *(__m128 const *)g_455940;
	__m128 const h = *half_extents;
	__m128 z = _mm_and_ps(_mm_mul_ps(_mm_shuffle_ps(h, h, 0xaa), transform->columns[2]), absolute);
	__m128 widen = _mm_set_ss(tolerance);
	widen = _mm_shuffle_ps(widen, widen, 0);
	z = _mm_add_ps(z, widen);
	__m128 const y = _mm_and_ps(_mm_mul_ps(_mm_shuffle_ps(h, h, 0x55), transform->columns[1]), absolute);
	__m128 const x = _mm_and_ps(_mm_mul_ps(_mm_shuffle_ps(h, h, 0x00), transform->columns[0]), absolute);
	bounds->upper = _mm_add_ps(_mm_add_ps(x, y), z);
	bounds->lower = _mm_xor_ps(bounds->upper, *(__m128 const *)g_4124a0);
	bounds->upper = _mm_add_ps(bounds->upper, transform->translation);
	bounds->lower = _mm_add_ps(bounds->lower, transform->translation);
}
