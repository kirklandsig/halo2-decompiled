// @flags /O2 /GL-
// Havok: reduce cached convex features, alternate supporting-point queries,
// and return the separating direction and signed distance with updated IDs.
#include "convex_search_internal.h"

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
	virtual void vertices(unsigned short const *, int, __m128 *) const = 0;
};
inline void search_support(convex_2fefd0 const *shape, __m128 const *direction, __m128 *out)
{
	((search_shape_3048a0 const *)shape)->support(direction,out);
}
inline void search_vertices(convex_2fefd0 const *shape, unsigned short const *ids, int count, __m128 *out)
{
	((search_shape_3048a0 const *)shape)->vertices(ids,count,out);
}
inline bool search_new_point(__m128 const *corners, int count, __m128 const &direction, float threshold)
{
	float projection = search_dot(_mm_sub_ps(corners[count],corners[0]),direction);
	if (!(fabs(projection)*projection > threshold)) return false;
	if (count == 3) {
		projection = search_dot(_mm_sub_ps(corners[count],corners[2]),direction);
		if (fabs(projection)*projection < threshold) return false;
	}
	if (count == 2 || count == 3) {
		projection = search_dot(_mm_sub_ps(corners[count],corners[1]),direction);
		if (fabs(projection)*projection < threshold) return false;
	}
	return true;
}

// @retail 0x3048a0
int class_3048a0::function_3048a0(convex_2fefd0 const *first, convex_2fefd0 const *second, c_query_transform const *transform, pair_state_2fefd0 *state, closest_3048a0 *closest)
{
	search_features_303dd0 &s = *(search_features_303dd0 *)this;
	signed char *counts = (signed char *)state;
	unsigned short *ids = (unsigned short *)state;
	s.count_a = counts[8]; s.count_b = counts[9];
	search_vertices(first,ids,s.count_a,s.a);
	search_vertices(second,ids+s.count_a,s.count_b,s.source_b);
	for (int i=0; i<s.count_b; ++i) s.b[i] = search_transform(transform,s.source_b[i]);
	int iteration = 0, result = 0;
	float tolerance = 1.0e-10f;
	for (;;)
	{
		tolerance += tolerance;
		++iteration;
		int overlap = search_reduce(s);
		float length_squared = search_dot(s.normal,s.normal);
		float separation = search_dot(_mm_sub_ps(s.b[0],s.a[0]),s.normal);
		if (overlap || tolerance*1.1920928955078125e-7f*length_squared >= separation*separation*separation)
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
			if (search_new_point(s.a,s.count_a,s.normal,threshold)) { ++s.count_a; continue; }
		}
		if (s.count_b != counts[11] && s.restricted_side != -1)
		{
			__m128 direction = search_inverse_direction(transform,s.normal);
			search_support(second,&direction,&s.source_b[s.count_b]);
			if (search_new_point(s.source_b,s.count_b,direction,threshold)) {
				s.b[s.count_b] = search_transform(transform,s.source_b[s.count_b]);
				++s.count_b;
				continue;
			}
		}
		if (!(iteration & 1) && s.count_a != counts[10] && s.restricted_side != 1)
		{
			search_support(first,&s.normal,&s.a[s.count_a]);
			if (search_new_point(s.a,s.count_a,s.normal,threshold)) { ++s.count_a; continue; }
		}
		break;
	}
	__m128 between = _mm_sub_ps(s.b[0],s.a[0]);
	__m128 length = search_sum(_mm_mul_ps(s.normal,s.normal));
	__m128 estimate = _mm_rsqrt_ss(length);
	__m128 correction = _mm_sub_ss(_mm_set_ss(3.0f),_mm_mul_ss(_mm_mul_ss(length,estimate),estimate));
	__m128 inverse = _mm_mul_ss(_mm_mul_ss(_mm_set_ss(0.5f),estimate),correction);
	closest->point = _mm_mul_ps(_mm_shuffle_ps(inverse,inverse,0),s.normal);
	_mm_store_ss(&closest->point.m128_f32[3],_mm_mul_ss(search_sum(_mm_mul_ps(between,s.normal)),inverse));
	counts[8] = (signed char)s.count_a; counts[9] = (signed char)s.count_b;
	int j=0;
	do { ids[j] = *(unsigned short const *)&s.a[j].m128_f32[3]; ++j; } while (j<s.count_a);
	int k=0;
	do { ids[j++] = *(unsigned short const *)&s.source_b[k].m128_f32[3]; ++k; } while (k<s.count_b);
	return result;
}
