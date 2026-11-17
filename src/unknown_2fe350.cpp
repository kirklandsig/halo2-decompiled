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

// score 0 (was 31): matched in the shape of 0x1ce210: the third column read into the variable that
// then holds the z term, the tolerance broadcast with _mm_set1_ps, and the
// y term written over the z product's variable.
// @retail 0x2fe350
void __cdecl function_2fe350(void const *query, const __m128 *half_extents, float tolerance, void *out)
{
	transform_2fe350 const *transform = (transform_2fe350 const *)query;
	bounds_2fe350 *bounds = (bounds_2fe350 *)out;
	__m128 *upper = &bounds->upper;
	__m128 *lower = &bounds->lower;
	__m128 const h = *half_extents;
	__m128 const absolute = *(__m128 const *)g_455940;
	__m128 z = transform->columns[2];
	__m128 const widen = _mm_set1_ps(tolerance);
	__m128 product = _mm_shuffle_ps(h, h, 0xaa);
	product = _mm_mul_ps(product, z);
	__m128 x = _mm_shuffle_ps(h, h, 0);
	__m128 const y = _mm_shuffle_ps(h, h, 0x55);
	z = _mm_and_ps(product, absolute);
	z = _mm_add_ps(z, widen);
	product = _mm_and_ps(_mm_mul_ps(y, transform->columns[1]), absolute);
	x = _mm_and_ps(_mm_mul_ps(x, transform->columns[0]), absolute);
	*upper = _mm_add_ps(_mm_add_ps(x, product), z);
	*lower = _mm_xor_ps(*upper, *(__m128 const *)g_4124a0);
	*upper = _mm_add_ps(*upper, transform->translation);
	*lower = _mm_add_ps(*lower, transform->translation);
}
