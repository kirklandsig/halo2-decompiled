#include "unknown_11c920.h"
#include <xmmintrin.h>
// @flags /O2 /arch:SSE /Gr

__declspec(align(16)) const dword g_455940[4] = {0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff};
__declspec(align(16)) const dword g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

// the bounds of a box under a transform: the half extents turned by the rotation, their
// absolute values summed and widened by a radius, then moved to the transformed centre.
// Its callers (one of them a library function) push every argument, and its prologue
// aligns the frame, so it keeps the standard convention.
// @retail 0x1ce210 standard
void __cdecl function_1ce210(const __m128 *arg_0, const __m128 *arg_1, const __m128 *arg_2, real arg_3, __m128 *arg_4)
{
 const __m128 *column_2 = &arg_0[2];
 __m128 *upper = &arg_4[1];
 __m128 *lower = &arg_4[0];
 __m128 local_0 = *arg_1;
 __m128 local_1 = *(__m128 *)g_455940;
 __m128 local_2 = *column_2;
 __m128 local_5 = _mm_set1_ps(arg_3);
 __m128 local_3 = _mm_shuffle_ps(local_0, local_0, 0xaa);
 local_3 = _mm_mul_ps(local_3, local_2);
 __m128 local_4 = _mm_shuffle_ps(local_0, local_0, 0);
 __m128 local_6 = _mm_shuffle_ps(local_0, local_0, 0x55);
 local_2 = _mm_and_ps(local_3, local_1);
 local_2 = _mm_add_ps(local_2, local_5);
 local_3 = _mm_and_ps(_mm_mul_ps(local_6, arg_0[1]), local_1);
 local_4 = _mm_and_ps(_mm_mul_ps(local_4, arg_0[0]), local_1);
 *upper = _mm_add_ps(_mm_add_ps(local_4, local_3), local_2);
 *lower = _mm_xor_ps(*upper, *(__m128 *)g_4124a0);
 local_0 = *arg_2;
 local_2 = _mm_add_ps(_mm_mul_ps(*column_2, _mm_shuffle_ps(local_0, local_0, 0xaa)), arg_0[3]);
 local_3 = _mm_mul_ps(arg_0[1], _mm_shuffle_ps(local_0, local_0, 0x55));
 local_4 = _mm_mul_ps(arg_0[0], _mm_shuffle_ps(local_0, local_0, 0));
 local_0 = _mm_add_ps(_mm_add_ps(local_4, local_3), local_2);
 *upper = _mm_add_ps(*upper, local_0);
 *lower = _mm_add_ps(*lower, local_0);
}
