// @flags /O2 /GL- /GX
// Havok (third-party physics library): creating an object holding a value
// and three 16-bit indices that start unset (0xffff); the contacts of a
// capsule and a segment (0x2f28f0), and of two capsules (0x2f3030), whose
// three contacts keep their ids in that object.
#include "bounded_2e9530.h"
#include "growable_array_2d91d0.h"
#include "timer_mark_2ebc80.h"
#include <math.h>
#include <xmmintrin.h>

struct body_2f28f0;
struct input_2f3030;
struct results_2f3030;

class indexed_2f28b0 : public counted_2e9530
{
public:
	REFERENCED_NEW_2D88A0(0x1a)

	unsigned short indices[3];

	indexed_2f28b0(long value_) : counted_2e9530(value_)
	{
		indices[0] = 0xffff;
		indices[1] = 0xffff;
		indices[2] = 0xffff;
	}

	void function_2f3030(body_2f28f0 const *a, body_2f28f0 const *b, input_2f3030 const *input, results_2f3030 *results);
};

// @retail 0x2f28b0
indexed_2f28b0 *__cdecl function_2f28b0(long arg_0, long arg_1, long arg_2, long value)
{
	return new indexed_2f28b0(value);
}

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

// three axes and a position
struct frame_2f28f0
{
	__m128 axes[3];
	__m128 position;
};

// a point taken out of a frame
inline __m128 frame_point_2f28f0(frame_2f28f0 const *m, __m128 const *point)
{
	__m128 p = *point;
	__m128 z = _mm_add_ps(_mm_mul_ps(m->axes[2], _mm_shuffle_ps(p, p, 0xaa)), m->position);
	__m128 y = _mm_mul_ps(m->axes[1], _mm_shuffle_ps(p, p, 0x55));
	__m128 x = _mm_mul_ps(m->axes[0], _mm_shuffle_ps(p, p, 0x00));
	return _mm_add_ps(_mm_add_ps(x, y), z);
}

// where a body sits: two vectors, then its frame
struct placement_2f28f0
{
	__m128 field_0;
	__m128 field_10;
	frame_2f28f0 frame;
};

// a capsule: its radius and the two ends of its segment
struct capsule_2f28f0
{
	void const *table;
	long field_4;
	long field_8;
	float radius;
	__m128 ends[2];
};

// one side of a query: a capsule, the key of the part concerned, and where
// it sits
struct body_2f28f0
{
	capsule_2f28f0 const *shape;
	long key;
	placement_2f28f0 const *placement;
};

// a contact found: its point, with the separation in the fourth term (on
// entry, how far away a contact still counts), and its normal
struct contact_point_2f28f0
{
	__m128 point;
	__m128 normal;
	__m128 field_20;
};

// what a query asks: how far away a contact still counts
struct input_2f3030
{
	long field_0;
	long field_4;
	float tolerance;
};

// a contact kept: its point, with the separation in the fourth term, its
// normal, and its id
struct contact_2f3030
{
	__m128 point;
	__m128 normal;
	unsigned short id;
};

ELEMENTS_2D91D0_ALIGNED(contacts_2f3030, contact_2f3030);

struct results_2f3030
{
	contacts_2f3030 *contacts;
};

// hands the contacts' ids out and takes them back
class owner_2f3030
{
public:
	virtual void slot_0() {}
	virtual unsigned short add_contact(void const *a, void const *b, void const *input, contact_point_2f28f0 *point) { return 0; }
	virtual void release_id(unsigned short id) {}
};

// the sum of a vector's first three terms, in the first
inline __m128 sum_2f28f0(__m128 const &v)
{
	return _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v));
}

// a vector of four copies of a float
inline __m128 splat_2f28f0(float value)
{
	__m128 const single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

// scales a vector to unit length: one over the square root of its length's
// square, refined by a Newton step
__forceinline void normalize_2f28f0(__m128 &vector)
{
	__m128 const normal = vector;
	__m128 const sum = sum_2f28f0(_mm_mul_ps(normal, normal));
	__m128 const inverse = _mm_rsqrt_ss(sum);
	__m128 const correction = _mm_sub_ss(_mm_set_ss(3.0f), _mm_mul_ss(_mm_mul_ss(sum, inverse), inverse));
	__m128 const scale = _mm_mul_ss(_mm_mul_ss(_mm_set_ss(0.5f), inverse), correction);
	vector = _mm_mul_ps(_mm_shuffle_ps(scale, scale, 0), normal);
}

// the contacts of a capsule and a segment// the contacts of a capsule and a segment of the given radius: the closest
// points of the two segments, clamped to their ends (bits 1 and 2 for the
// capsule's end and start, 4 and 8 for the segment's), give the first
// contact; when the segments are close to parallel, the ends that lie across
// the other segment give up to two more
__forceinline void capsule_segment_contacts_2f28f0(body_2f28f0 const *body, __m128 const *segment, float radius, contact_point_2f28f0 *points)
{
	capsule_2f28f0 const *capsule = body->shape;
	__m128 ends[2];
	{
		frame_2f28f0 const *frame = &body->placement->frame;
		__m128 const *end = capsule->ends;
		__m128 *placed = ends;
		for (int count = 2; count != 0; count--)
		{
			*placed = frame_point_2f28f0(frame, end);
			placed++;
			end++;
		}
	}
	__m128 direction = _mm_sub_ps(ends[1], ends[0]);
	__m128 const other_start = segment[0];
	__m128 other_direction = _mm_sub_ps(segment[1], other_start);
	float along;
	_mm_store_ss(&along, sum_2f28f0(_mm_mul_ps(direction, other_direction)));
	float cosine = along;
	if (along < 0.0f)
	{
		cosine = -along;
		direction = _mm_xor_ps(direction, *(__m128 const *)g_4124a0);
		__m128 const swapped = ends[0];
		ends[0] = ends[1];
		ends[1] = swapped;
	}
	__m128 const offset = _mm_sub_ps(other_start, ends[0]);
	float offset_along;
	_mm_store_ss(&offset_along, sum_2f28f0(_mm_mul_ps(direction, offset)));
	float offset_along_other;
	_mm_store_ss(&offset_along_other, sum_2f28f0(_mm_mul_ps(other_direction, offset)));
	float length_squared;
	_mm_store_ss(&length_squared, sum_2f28f0(_mm_mul_ps(direction, direction)));
	float other_length_squared;
	_mm_store_ss(&other_length_squared, sum_2f28f0(_mm_mul_ps(other_direction, other_direction)));

	float product = other_length_squared * length_squared;
	float const cosine_squared = cosine * cosine;
	float const determinant = (float)fabs(product - cosine_squared);
	float numerator = other_length_squared * offset_along - offset_along_other * cosine;
	float fraction;
	int clamped;
	if (numerator * determinant >= determinant * determinant)
	{
		fraction = 1.0f;
		clamped = 1;
	}
	else if (numerator <= 0.0f)
	{
		fraction = 0.0f;
		clamped = 2;
	}
	else if (determinant > ((float)fabs(product) + cosine_squared) * 9.5367431640625e-07f)
	{
		fraction = numerator / determinant;
		clamped = 0;
	}
	else
	{
		fraction = 1.0f;
		clamped = 1;
	}
	float other_fraction = cosine * fraction - offset_along_other;
	if (other_fraction >= other_length_squared)
	{
		other_fraction = 1.0f;
		clamped = 4;
	}
	else if (other_fraction <= 0.0f)
	{
		other_fraction = 0.0f;
		clamped = 8;
	}
	else
	{
		other_fraction = other_fraction / other_length_squared;
		goto found;
	}
	numerator = other_fraction * cosine + offset_along;
	if (numerator <= 0.0f)
	{
		clamped |= 2;
		fraction = 0.0f;
	}
	else if (numerator >= length_squared)
	{
		clamped |= 1;
		fraction = 1.0f;
	}
	else
	{
		fraction = numerator / length_squared;
	}
found:
	__m128 const closest = _mm_add_ps(ends[0], _mm_mul_ps(splat_2f28f0(fraction), direction));
	float const radii = radius + capsule->radius;
	float const reach = radii + points->point.m128_f32[3];
	__m128 const other_closest = _mm_add_ps(other_start, _mm_mul_ps(splat_2f28f0(other_fraction), other_direction));
	__m128 const between = _mm_sub_ps(closest, other_closest);
	float distance_squared;
	_mm_store_ss(&distance_squared, sum_2f28f0(_mm_mul_ps(between, between)));
	if (distance_squared < reach * reach)
	{
		float const distance = (float)sqrt(distance_squared);
		points->normal = between;
		if (distance_squared < 1.192092896e-07f)
		{
			float minimum = fabsf(direction.m128_f32[0]);
			int smallest = 0;
			int second = 1;
			float const y = fabsf(direction.m128_f32[1]);
			int third = 2;
			float const z = fabsf(direction.m128_f32[2]);
			if (y < minimum)
			{
				second = 0;
				minimum = y;
				smallest = 1;
			}
			if (z < minimum)
			{
				third = smallest;
				smallest = 2;
			}
			points->normal.m128_f32[smallest] = 0.0f;
			points->normal.m128_f32[second] = direction.m128_f32[third];
			points->normal.m128_f32[third] = -direction.m128_f32[second];
		}
		normalize_2f28f0(points->normal);

		points->point = _mm_add_ps(closest, _mm_mul_ps(splat_2f28f0(radius - distance), points->normal));
		points->point.m128_f32[3] = distance - radii;

		// nearly parallel: the ends of each segment that lie across the other
		if (determinant > product * 0.2f)
		{
			return;
		}
		float const inverse_product = 1.0f / product;
		float const inverse_length_squared = other_length_squared * inverse_product;
		float const other_inverse_length_squared = inverse_product * length_squared;
		contact_point_2f28f0 *point = points + 1;
		int end = 0;
		int ends_clamped = 0xa;
		__m128 end_point = ends[0];
		__m128 other_end_point = segment[0];
		float other_projection = -offset_along_other;
		float projection = offset_along;
		for (; ;)
		{
			if ((clamped & ends_clamped) == 0)
			{
				if (other_projection > 0.0f)
				{
					if (other_projection > other_length_squared)
					{
						goto next;
					}
					if (projection > 0.0f)
					{
						if (projection > length_squared)
						{
							goto next;
						}
						if (other_projection * other_projection * other_inverse_length_squared > projection * projection * inverse_length_squared)
						{
							end_point = _mm_add_ps(end_point, _mm_mul_ps(splat_2f28f0(inverse_length_squared * projection), direction));
						}
						else
						{
							other_end_point = _mm_add_ps(other_end_point, _mm_mul_ps(splat_2f28f0(other_projection * other_inverse_length_squared), other_direction));
						}
					}
					else
					{
						other_end_point = _mm_add_ps(other_end_point, _mm_mul_ps(splat_2f28f0(other_projection * other_inverse_length_squared), other_direction));
					}
				}
				else if (projection > 0.0f)
				{
					if (projection > length_squared)
					{
						goto next;
					}
					end_point = _mm_add_ps(end_point, _mm_mul_ps(splat_2f28f0(inverse_length_squared * projection), direction));
				}
				{
					float const end_radii = radius + capsule->radius;
					__m128 const end_between = _mm_sub_ps(end_point, other_end_point);
					float const end_reach = end_radii + point->point.m128_f32[3];
					float end_distance_squared;
					_mm_store_ss(&end_distance_squared, sum_2f28f0(_mm_mul_ps(end_between, end_between)));
					if (end_distance_squared < end_reach * end_reach)
					{
						float const end_distance = (float)sqrt(end_distance_squared);
						point->normal = end_between;
						if (end_distance_squared > 1.192092896e-07f)
						{
							normalize_2f28f0(point->normal);

						}
						else
						{
							point->normal = points->normal;
						}
						point->point = _mm_add_ps(end_point, _mm_mul_ps(splat_2f28f0(radius - end_distance), point->normal));
						point->point.m128_f32[3] = end_distance - end_radii;
					}
				}
			}
		next:
			if (end == 1)
			{
				break;
			}
			end_point = ends[1];
			other_end_point = segment[1];
			other_projection = other_length_squared - other_projection - cosine;
			direction = _mm_xor_ps(direction, *(__m128 const *)g_4124a0);
			other_direction = _mm_xor_ps(other_direction, *(__m128 const *)g_4124a0);
			projection = length_squared - projection - cosine;
			point++;
			end++;
			ends_clamped = 5;
		}
	}
}

// @retail 0x2f28f0
void __cdecl function_2f28f0(body_2f28f0 const *body, __m128 const *segment, float radius, contact_point_2f28f0 *points)
{
	capsule_segment_contacts_2f28f0(body, segment, radius, points);
}

typedef void *(__fastcall *constructor_proc)(void *);
void __stdcall vector_constructor_iterator(void *array, unsigned size, int count, constructor_proc constructor);
void *__fastcall function_31bc70(void *arg_0);

// the contacts of two capsules: the second capsule's segment, in the world,
// is taken as the segment of the capsule-and-segment query above with the
// second capsule's radius; each of the three contacts found keeps its id
// from the owner while it lies within the tolerance and is added to the
// results, and gives its id back when it no longer does
// score 1 (was 15): the store of the transformed capsule ends puts the base
// and the index the other way round: [eax + ecx] here, [ecx + eax] in retail.
// Indexed transforms 575; indexed source 575; indexed output 573; source-bound 505;
// output-bound 620; endpoint-array name 1; later frame 1; frame copy 624;
// source-first advances 1; transform input order 1; y-product first 1.
// Ascending cursor loop 1; while count test 1; unsigned count 1;
// nonnegative count 1; aligned load 1; aligned store 1; point by value 1;
// separate z product 1. All restored.
// @retail 0x2f3030
void indexed_2f28b0::function_2f3030(body_2f28f0 const *a, body_2f28f0 const *b, input_2f3030 const *input, results_2f3030 *results)
{
	timer_add_2ebc80("TtCapsCaps");
	capsule_2f28f0 const *capsule = b->shape;
	contact_point_2f28f0 points[3];
	float const tolerance = input->tolerance;
	points[0].point.m128_f32[3] = tolerance;
	points[1].point.m128_f32[3] = tolerance;
	points[2].point.m128_f32[3] = tolerance;
	{
		__m128 segment[2];
		vector_constructor_iterator(segment, sizeof(__m128), 2, function_31bc70);
		{
			frame_2f28f0 const *frame = &b->placement->frame;
			__m128 const *end = capsule->ends;
			__m128 *placed = segment;
			for (int count = 2; count != 0; count--)
			{
				*placed = frame_point_2f28f0(frame, end);
				placed++;
				end++;
			}
		}
		capsule_segment_contacts_2f28f0(a, segment, capsule->radius, points);
	}
	contact_point_2f28f0 *point = points;
	unsigned short *id = indices;
	for (int count = 3; count != 0; count--)
	{
		if (point->point.m128_f32[3] < input->tolerance)
		{
			if (*id == 0xffff)
			{
				*id = ((owner_2f3030 *)value)->add_contact(a, b, input, point);
				if (*id == 0xffff)
				{
					goto next;
				}
			}
			{
				contact_2f3030 &kept = results->contacts->next_slot();
				kept.point = point->point;
				kept.normal = point->normal;
				kept.id = *id;
			}
		}
		else if (*id != 0xffff)
		{
			((owner_2f3030 *)value)->release_id(*id);
			*id = 0xffff;
		}
	next:
		id++;
		point++;
	}
	timer_add_2ebc80("Et");
}
