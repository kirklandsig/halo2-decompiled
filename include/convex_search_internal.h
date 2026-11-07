#ifndef CONVEX_SEARCH_INTERNAL_H
#define CONVEX_SEARCH_INTERNAL_H
// Private views of existing feature helpers; their implementations stay
// in unknown_303360.cpp and unknown_303b80.cpp.
#include "convex_pair_2ee2e0.h"
#include <math.h>
struct triangle_303360;
struct segment_3037a0;
struct triangle_3037a0;
class class_303360
{
public:
	int function_303360(__m128 const *, triangle_303360 *, int);
	int function_3037a0(segment_3037a0 *, triangle_3037a0 *, int *, int *, bool, long);
	void function_303b80();
};
struct closest_points_308590
{
	__m128 first_point;
	__m128 offset;
	float distance_squared;
};
int __cdecl function_308590(__m128 const *, __m128 const *, __m128 const *, __m128 const *, closest_points_308590 *);
// Pending integration with upstream's file-local 0x303540 implementation.
// Preserve the reconstruction's explicit ordinary-convention test boundary.
int __cdecl convex_search_tetrahedron_boundary(__m128 const *point, __m128 const *tetrahedron, float orientation);
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
inline __m128 search_negate(__m128 a) { return _mm_xor_ps(a,*(__m128 const *)search_sign); }
inline __m128 search_transform(c_query_transform const *t, __m128 p)
{
	return _mm_add_ps(_mm_add_ps(_mm_mul_ps(t->axes[0],_mm_shuffle_ps(p,p,0)),_mm_mul_ps(t->axes[1],_mm_shuffle_ps(p,p,0x55))),
		_mm_add_ps(_mm_mul_ps(t->axes[2],_mm_shuffle_ps(p,p,0xaa)),t->position));
}
inline __m128 search_inverse_direction(c_query_transform const *t, __m128 p)
{
	__m128 low = _mm_shuffle_ps(t->axes[0],t->axes[1],0x44);
	__m128 last_low = _mm_shuffle_ps(t->axes[2],t->axes[2],0x44);
	__m128 high = _mm_shuffle_ps(t->axes[0],t->axes[1],0xee);
	__m128 last_high = _mm_shuffle_ps(t->axes[2],t->axes[2],0xee);
	return search_negate(_mm_add_ps(_mm_add_ps(
		_mm_mul_ps(_mm_shuffle_ps(low,last_low,0x88),_mm_shuffle_ps(p,p,0)),
		_mm_mul_ps(_mm_shuffle_ps(low,last_low,0xdd),_mm_shuffle_ps(p,p,0x55))),
		_mm_mul_ps(_mm_shuffle_ps(high,last_high,0x88),_mm_shuffle_ps(p,p,0xaa))));
}
// One means overlap; zero leaves a separating direction.
__forceinline int search_reduce(search_features_303dd0 &s)
{
	class_303360 *helper = (class_303360 *)&s;
	s.restricted_side = 0;
	for (;;)
	{
		switch ((s.count_a << 3) | s.count_b)
		{
		case 9:
			s.normal = _mm_sub_ps(s.b[0],s.a[0]);
			goto complete;
		case 19: case 26:
		{
			int result;
			if (s.count_a == 2)
				result = helper->function_3037a0((segment_3037a0 *)s.a,(triangle_3037a0 *)s.b,&s.count_a,&s.count_b,s.previous_b == 2,1);
			else
				result = helper->function_3037a0((segment_3037a0 *)s.b,(triangle_3037a0 *)s.a,&s.count_b,&s.count_a,s.previous_b == 2,-1);
			if (result == 1) return 1;
			if (result != 2) goto complete;
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
			goto complete;
		}
		case 12: case 33:
		{
			bool second = s.count_b == 4;
			__m128 *corners = second ? s.b : s.a;
			__m128 *point = second ? s.a : s.b;
			int index = convex_search_tetrahedron_boundary(point,corners,second ? -1.0f : 1.0f);
			if (index < 0) return 1;
			if (second) { --s.count_b; s.source_b[index] = s.source_b[3]; } else --s.count_a;
			corners[index] = corners[3];
		}
		case 11: case 25:
		{
			bool second = s.count_b == 3;
			__m128 *corners = second ? s.b : s.a;
			__m128 *point = second ? s.a : s.b;
			int &count = second ? s.count_b : s.count_a;
			int inside = helper->function_303360(point,(triangle_303360 *)corners,second ? 1 : -1);
			if (inside == 7) goto complete;
			int index = search_corner[inside];
			if (index >= 0)
			{
				int previous = search_previous[index], next = search_previous[index+2];
				__m128 delta = _mm_sub_ps(point[0],corners[index]);
				__m128 before = _mm_sub_ps(corners[previous],corners[index]);
				__m128 after = _mm_sub_ps(corners[next],corners[index]);
				count = 2;
				if (search_negative_dot(delta,before))
				{
					corners[previous] = corners[2];
					if (second) s.source_b[previous] = s.source_b[2];
					if (search_negative_dot(delta,after))
					{
						if (next == count) next = previous;
						--count;
						corners[next] = corners[count];
						if (second) s.source_b[next] = s.source_b[count];
					}
				}
				else { corners[next] = corners[2]; if (second) s.source_b[next] = s.source_b[2]; }
				continue;
			}
			index += 8;
			if (index > 3) index = 1;
			--count;
			corners[index] = corners[2];
			if (second) s.source_b[index] = s.source_b[2];
		}
		case 10: case 17:
		{
			bool second = s.count_b == 2;
			__m128 *corners = second ? s.b : s.a;
			__m128 point = second ? s.a[0] : s.b[0];
			__m128 direction = _mm_sub_ps(corners[1],corners[0]);
			__m128 start = _mm_sub_ps(corners[0],point), end = _mm_sub_ps(corners[1],point);
			int end_sign = search_negative_dot(end,direction);
			if (end_sign != search_negative_dot(start,direction))
			{
				__m128 across = search_cross(end,start);
				s.normal = second ? search_cross(across,direction) : search_cross(direction,across);
				goto complete;
			}
			if (end_sign)
			{
				corners[0] = corners[1];
				if (second) s.source_b[0] = s.source_b[1]; else s.source_a[0] = s.source_a[1];
			}
			if (second) --s.count_b; else --s.count_a;
			s.normal = _mm_sub_ps(s.b[0],s.a[0]);
			goto complete;
		}
		default: return 1;
		}
	}
complete:
	s.previous_b = s.count_b;
	return 0;
}
#endif
