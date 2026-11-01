// @flags /O2 /GL-
// Havok: seed a penetration query from the current features, reduce its
// returned features, and retain its penetration direction.
#include "convex_search_internal.h"
struct transform_30a6b0;
long __cdecl function_30a6b0(long, long, transform_30a6b0 const *, long, long, long);
struct simplex_303dd0 { __m128 a[4], b[4]; int count; };
struct penetration_303dd0 { __m128 direction, point; float distance, fraction; };

// @retail 0x303dd0
int class_3048a0::function_303dd0(convex_2fefd0 const *first, convex_2fefd0 const *second, c_query_transform const *transform, pair_state_2fefd0 *state)
{
	search_features_303dd0 &s = *(search_features_303dd0 *)this;
	simplex_303dd0 simplex;
	__m128 differences[4];
	penetration_303dd0 penetration;
	simplex.count = 4;
	simplex.a[0] = s.a[0]; simplex.b[0] = s.source_b[0];
	differences[0] = _mm_sub_ps(s.a[0],s.b[0]);
	if (s.count_b >= 3)
	{
		for (int i=1; i<3; ++i) {
			simplex.a[i] = s.a[0]; simplex.b[i] = s.source_b[i];
			differences[i] = _mm_sub_ps(s.a[0],s.b[i]);
		}
		if (s.count_b > 3) {
			simplex.a[3] = s.a[0]; simplex.b[3] = s.source_b[3];
			differences[3] = _mm_sub_ps(s.a[0],s.b[3]);
		} else if (s.count_a > 1) {
			simplex.a[3] = s.a[1]; simplex.b[3] = s.source_b[0];
			differences[3] = _mm_sub_ps(s.a[1],s.b[0]);
		} else simplex.count = 3;
	}
	else if (s.count_a >= 3)
	{
		for (int i=1; i<3; ++i) {
			simplex.a[i] = s.a[i]; simplex.b[i] = s.source_b[0];
			differences[i] = _mm_sub_ps(s.a[i],s.b[0]);
		}
		if (s.count_a > 3) {
			simplex.a[3] = s.a[3]; simplex.b[3] = s.source_b[0];
			differences[3] = _mm_sub_ps(s.a[3],s.b[0]);
		} else if (s.count_b > 1) {
			simplex.a[3] = s.a[0]; simplex.b[3] = s.source_b[1];
			differences[3] = _mm_sub_ps(s.a[0],s.b[1]);
		} else simplex.count = 3;
	}
	else simplex.count = 1;
	int result = function_30a6b0((long)first,(long)second,(transform_30a6b0 const *)transform,(long)&simplex,(long)differences,(long)&penetration);
	for (int i=0; i<3; ++i) { s.a[i] = simplex.a[i]; s.source_b[i] = simplex.b[i]; }
	if (simplex.count == 1) {
		s.normal = _mm_set_ps(0.0f,0.0f,0.0f,1.0f);
		s.count_a = s.count_b = 1;
		return result;
	}
	s.count_a = s.count_b = 3;
	if (!(_mm_movemask_ps(_mm_cmplt_ps(g_4801e0,_mm_and_ps(_mm_sub_ps(s.a[1],s.a[2]),*(__m128 const *)search_magnitude))) & 7) ||
		!(_mm_movemask_ps(_mm_cmplt_ps(g_4801e0,_mm_and_ps(_mm_sub_ps(s.a[0],s.a[2]),*(__m128 const *)search_magnitude))) & 7)) s.count_a = 2;
	if (!(_mm_movemask_ps(_mm_cmplt_ps(g_4801e0,_mm_and_ps(_mm_sub_ps(s.a[0],s.a[1]),*(__m128 const *)search_magnitude))) & 7)) s.a[0] = s.a[--s.count_a];
	if (!(_mm_movemask_ps(_mm_cmplt_ps(g_4801e0,_mm_and_ps(_mm_sub_ps(s.source_b[1],s.source_b[2]),*(__m128 const *)search_magnitude))) & 7) ||
		!(_mm_movemask_ps(_mm_cmplt_ps(g_4801e0,_mm_and_ps(_mm_sub_ps(s.source_b[0],s.source_b[2]),*(__m128 const *)search_magnitude))) & 7)) --s.count_b;
	if (s.count_b >= 2 && !(_mm_movemask_ps(_mm_cmplt_ps(g_4801e0,_mm_and_ps(_mm_sub_ps(s.source_b[0],s.source_b[1]),*(__m128 const *)search_magnitude))) & 7)) s.source_b[0] = s.source_b[--s.count_b];
	for (int j=0; j<s.count_b; ++j) s.b[j] = search_transform(transform,s.source_b[j]);
	if (s.count_a+s.count_b > 4) ((class_303360 *)this)->function_303b80();
	search_reduce(s);
	s.normal = search_negate(penetration.direction);
	return result;
}
