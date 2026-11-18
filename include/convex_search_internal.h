#ifndef CONVEX_SEARCH_INTERNAL_H
#define CONVEX_SEARCH_INTERNAL_H
// Private views of existing feature helpers; their implementations stay
// in unknown_303360.cpp and unknown_303b80.cpp.
#include "convex_pair_2ee2e0.h"
#include <math.h>
#include <string.h>
struct triangle_303360;
struct segment_3037a0;
struct triangle_3037a0;
#include "search_flag_3037a0.h"
class class_303360
{
public:
	int function_303360(__m128 const *, triangle_303360 *, int);
	int function_3037a0(segment_3037a0 *, triangle_3037a0 *, int *, int *, search_flag_3037a0, long);
	void function_303b80();
};
struct closest_points_308590
{
	__m128 first_point;
	__m128 offset;
	float distance_squared;
};
int __cdecl function_308590(__m128 const *, __m128 const *, __m128 const *, __m128 const *, closest_points_308590 *);
// The real tetrahedron reducer, shared with unknown_303110.cpp.
int __cdecl function_303540(__m128 const *point, __m128 const *vertices, float orientation);
extern __m128 g_4801e0;
static __declspec(align(16)) const unsigned long search_sign[4] = {0x80000000,0x80000000,0x80000000,0x80000000};
static __declspec(align(16)) const unsigned long search_magnitude[4] = {0x7fffffff,0x7fffffff,0x7fffffff,0x7fffffff};
static const signed char search_corner[8] = {-1,0,1,-6,2,-7,-8,-1};
static const signed char search_previous[5] = {2,0,1,2,0};
struct search_features_303dd0
{
	int count_a, count_b, previous_b;
	bool skip_penetration;
	char unknown_d[3];
	long restricted_side;
	char unknown_14[12];
	__m128 a[4], source_a[4], b[4], source_b[4];
	__m128 normal;
	closest_points_308590 closest;
	__m128 edge_dots;
};
inline __m128 search_sum(__m128 v)
{
	return _mm_add_ss(_mm_shuffle_ps(v,v,0xaa), _mm_add_ss(_mm_shuffle_ps(v,v,0x55),v));
}
inline float search_dot(__m128 a, __m128 b)
{
	float result;
	_mm_store_ss(&result,search_sum(_mm_mul_ps(a,b)));
	return result;
}
inline int search_negative_dot(__m128 a, __m128 b)
{
	return _mm_movemask_ps(search_sum(_mm_mul_ps(a,b))) & 1;
}
inline __m128 search_cross(__m128 a, __m128 b)
{
	return _mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(a,a,0xc9),_mm_shuffle_ps(b,b,0xd2)),
		_mm_mul_ps(_mm_shuffle_ps(a,a,0xd2),_mm_shuffle_ps(b,b,0xc9)));
}
inline __m128 search_sign_mask() { __m128 mask; memcpy(&mask,search_sign,sizeof(mask)); return mask; }
inline __m128 search_magnitude_mask() { __m128 mask; memcpy(&mask,search_magnitude,sizeof(mask)); return mask; }
inline __m128 search_negate(__m128 a) { return _mm_xor_ps(a,search_sign_mask()); }
inline __m128 search_transform(c_query_transform const *t, __m128 const &p)
{
	return _mm_add_ps(_mm_add_ps(_mm_mul_ps(t->axes[0],_mm_shuffle_ps(p,p,0)),_mm_mul_ps(t->axes[1],_mm_shuffle_ps(p,p,0x55))),
		_mm_add_ps(_mm_mul_ps(t->axes[2],_mm_shuffle_ps(p,p,0xaa)),t->position));
}
__forceinline __m128 search_inverse_direction(c_query_transform const *t, __m128 const &p)
{
	__m128 c0 = t->axes[0];
	__m128 c1 = t->axes[1];
	__m128 c2 = t->axes[2];
	{
		__m128 const tmp0 = _mm_shuffle_ps(c0,c1,0x44);
		__m128 const tmp2 = _mm_shuffle_ps(c0,c1,0xee);
		__m128 const tmp1 = _mm_shuffle_ps(c2,c2,0x44);
		__m128 const tmp3 = _mm_shuffle_ps(c2,c2,0xee);
		c0 = _mm_shuffle_ps(tmp0,tmp1,0x88);
		c1 = _mm_shuffle_ps(tmp0,tmp1,0xdd);
		c2 = _mm_shuffle_ps(tmp2,tmp3,0x88);
	}
	__m128 const b0 = _mm_shuffle_ps(p,p,0);
	__m128 const b1 = _mm_shuffle_ps(p,p,0x55);
	__m128 const b2 = _mm_shuffle_ps(p,p,0xaa);
	__m128 const r0 = _mm_mul_ps(c0,b0);
	__m128 const r1 = _mm_mul_ps(c1,b1);
	__m128 const r2 = _mm_mul_ps(c2,b2);
	return search_negate(_mm_add_ps(_mm_add_ps(r0,r1),r2));
}
inline __m128 search_transform_ordered(c_query_transform const *t, __m128 const *point)
{
	__m128 const p = *point;
	__m128 const x = _mm_mul_ps(t->axes[0],_mm_shuffle_ps(p,p,0));
	__m128 const y = _mm_mul_ps(t->axes[1],_mm_shuffle_ps(p,p,0x55));
	__m128 const z = _mm_mul_ps(t->axes[2],_mm_shuffle_ps(p,p,0xaa));
	return _mm_add_ps(_mm_add_ps(x,y),_mm_add_ps(z,t->position));
}
__forceinline void search_transform_points(c_query_transform const *t, __m128 *target, __m128 const *source, int count)
{
	for (int i = count - 1; i >= 0; --i) { *target = search_transform_ordered(t,source); ++source; ++target; }
}
// Reduces the feature pair until it is a known separating case.
// One means overlap; zero leaves a separating direction.
__forceinline int search_reduce_features(search_features_303dd0 &s)
{
	class_303360 *helper = (class_303360 *)&s;
	for (;;)
	{
		switch ((s.count_a << 3) | s.count_b)
		{
		{
			int result;
		case 19:
			result = helper->function_3037a0((segment_3037a0 *)s.a,(triangle_3037a0 *)s.b,&s.count_a,&s.count_b,search_flag_3037a0(s.previous_b == 2),1);
			if (0)
			{
		case 26:
				result = helper->function_3037a0((segment_3037a0 *)s.b,(triangle_3037a0 *)s.a,&s.count_b,&s.count_a,search_flag_3037a0(s.previous_b == 2),-1);
			}
			switch (result)
			{
			case 1: return 1;
			case 2: break;
			default: { s.previous_b = s.count_b; return 0; }
			}
		}
		case 18:
		{
			__m128 da = _mm_sub_ps(s.a[1],s.a[0]);
			__m128 db = _mm_sub_ps(s.b[1],s.b[0]);
			int flags = function_308590(&s.a[0],&da,&s.b[0],&db,&s.closest);
			if (flags)
			{
				if (flags & 1) { s.a[0] = s.a[1]; s.count_a = 1; }
				else if (flags & 2) s.count_a = 1;
				if (flags & 4) { s.b[0] = s.b[1]; s.source_b[0] = s.source_b[1]; s.count_b = 1; }
				else if (flags & 8) s.count_b = 1;
				continue;
			}
			s.normal = search_cross(da,db);
			if (!search_negative_dot(s.normal,_mm_sub_ps(s.a[0],s.b[0]))) s.normal = search_negate(s.normal);
			break;
		}
		case 12:
		{
			int index = function_303540(s.a,s.b,-1.0f);
			if (index < 0) return 1;
			--s.count_b;
			s.b[index] = s.b[3];
			s.source_b[index] = s.source_b[3];
		}
		case 11:
		{
			int inside = helper->function_303360(s.a,(triangle_303360 *)s.b,1);
			if (inside == 7) break;
			int index = search_corner[inside];
			if (index >= 0)
			{
				int previous = g_cycles_4136b0[index], next = g_cycles_4136b0[index+2];
				__m128 delta = _mm_sub_ps(s.a[0],s.b[index]);
				__m128 before = _mm_sub_ps(s.b[previous],s.b[index]);
				__m128 after = _mm_sub_ps(s.b[next],s.b[index]);
				int const behind = search_negative_dot(delta,before);
				s.count_b = 2;
				if (behind)
				{
					s.b[previous] = s.b[2];
					s.source_b[previous] = s.source_b[2];
					if (search_negative_dot(delta,after))
					{
						if (next == s.count_b) next = previous;
						--s.count_b;
						s.b[next] = s.b[s.count_b];
						s.source_b[next] = s.source_b[s.count_b];
					}
				}
				else { s.b[next] = s.b[2]; s.source_b[next] = s.source_b[2]; }
				continue;
			}
			index += 8;
			if (index > 3) index = 1;
			--s.count_b;
			s.b[index] = s.b[2];
			s.source_b[index] = s.source_b[2];
		}
		case 10:
		{
			__m128 direction = _mm_sub_ps(s.b[1],s.b[0]);
			__m128 start = _mm_sub_ps(s.b[0],s.a[0]), end = _mm_sub_ps(s.b[1],s.a[0]);
			__m128 end_dot = search_sum(_mm_mul_ps(end,direction));
			__m128 start_dot = search_sum(_mm_mul_ps(start,direction));
			int end_sign = _mm_movemask_ps(end_dot) & 1;
			if (end_sign != (_mm_movemask_ps(start_dot) & 1))
			{
				__m128 across = search_cross(end,start);
				s.normal = search_cross(across,direction);
				break;
			}
			if (end_sign)
			{
				s.b[0] = s.b[1];
				s.source_b[0] = s.source_b[1];
			}
			--s.count_b;
			{ __m128 const &to = s.b[0]; __m128 const &from = s.a[0]; s.normal = _mm_sub_ps(to,from); }
			break;
		}
		case 33:
		{
			int index = function_303540(s.b,s.a,1.0f);
			if (index < 0) return 1;
			--s.count_a;
			s.a[index] = s.a[3];
		}
		case 25:
		{
			int inside = helper->function_303360(s.b,(triangle_303360 *)s.a,-1);
			if (inside == 7) break;
			int index = search_corner[inside];
			if (index >= 0)
			{
				int previous = g_cycles_4136b0[index], next = g_cycles_4136b0[index+2];
				__m128 delta = _mm_sub_ps(s.b[0],s.a[index]);
				__m128 before = _mm_sub_ps(s.a[previous],s.a[index]);
				__m128 after = _mm_sub_ps(s.a[next],s.a[index]);
				int const behind = search_negative_dot(delta,before);
				s.count_a = 2;
				if (behind)
				{
					s.a[previous] = s.a[2];
					if (search_negative_dot(delta,after))
					{
						if (next == s.count_a) next = previous;
						--s.count_a;
						s.a[next] = s.a[s.count_a];
					}
				}
				else s.a[next] = s.a[2];
				continue;
			}
			index += 8;
			if (index > 3) index = 1;
			--s.count_a;
			s.a[index] = s.a[2];
		}
		case 17:
		{
			__m128 direction = _mm_sub_ps(s.a[1],s.a[0]);
			__m128 start = _mm_sub_ps(s.a[0],s.b[0]), end = _mm_sub_ps(s.a[1],s.b[0]);
			__m128 end_dot = search_sum(_mm_mul_ps(end,direction));
			__m128 start_dot = search_sum(_mm_mul_ps(start,direction));
			int end_sign = _mm_movemask_ps(end_dot) & 1;
			if (end_sign != (_mm_movemask_ps(start_dot) & 1))
			{
				__m128 across = search_cross(end,start);
				s.normal = search_cross(direction,across);
				break;
			}
			if (end_sign)
			{
				s.a[0] = s.a[1];
				s.source_a[0] = s.source_a[1];
			}
			--s.count_a;
		}
		case 9:
			{ __m128 const &to = s.b[0]; s.normal = _mm_sub_ps(to,s.a[0]); }
			break;
		default: return 1;
		}
		{ int const &count = s.count_b; s.previous_b = count; }
		return 0;
	}
}
#endif
