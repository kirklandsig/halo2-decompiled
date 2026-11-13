// @flags /O2 /GL- /GX
// Havok (third-party physics library): a three-axis sweep of boxes
// (axis_sweep_2e1090.h): the boxes a box reaches as it moves along a segment,
// walked endpoint by endpoint through the three sorted axes.
#include "sweep_quantize_2e1090.h"
#include "reference_counted_2d88a0.h"
#include <math.h>

// an array of bytes whose storage comes from the allocator's stack area
// (src/unknown_2e18e0.cpp)
struct array_bytes_2e1be0
{
	char *data;
	long size;
	unsigned long capacity_and_flags;

	array_bytes_2e1be0() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~array_bytes_2e1be0()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(char), 0x12);
		}
	}
};

struct bytes_2e1be0 : array_bytes_2e1be0
{
	char *stack_block;
	long stack_capacity;

	bytes_2e1be0(long capacity);
	~bytes_2e1be0();
};

// the largest quantized corner a moving box may have
static __declspec(align(16)) const float g_4125f0[4] = {65531.0f, 65531.0f, 65531.0f, 65531.0f};

// the moving box: its centre at the start and at the end, its half extent,
// and the axes to start from (the sweep's own when null)
struct ray_2e4660
{
	__m128 from;
	__m128 to;
	__m128 extent;
	endpoints_2e0f80 const *axes;
};

// takes each box the moving box reaches, and gives back how far along the
// segment the walk still has to go
class ray_collector_2e4660
{
public:
	virtual void slot_0() {}
	virtual float add_box(long owner, long arg_1) { return 0.0f; }
};

union vector_2e4660
{
	__m128 vector;
	float terms[4];
	unsigned long bits[4];
};

// a box whose byte counts all three axes is passed on, unless it is a grid
// cell's; the smaller distance left is kept
__forceinline void reach_2e4660(char flags, element_2e1090 const *element, ray_collector_2e4660 *collector, float *left)
{
	if (flags == 7 && (element->owner & 1) == 0)
	{
		float const distance = collector->add_box(element->owner, 0);
		if (!(*left < distance))
		{
			*left = distance;
		}
	}
}

// the axis whose next time is the smallest
__forceinline int earliest_2e4660(float const *times)
{
	if (times[0] < times[1])
	{
		return times[0] < times[2] ? 0 : 2;
	}
	return times[1] < times[2] ? 1 : 2;
}

// an array of longs whose storage comes from the allocator's stack area
// (src/unknown_2e18e0.cpp)
struct array_longs_2e1ab0
{
	long *data;
	long size;
	unsigned long capacity_and_flags;

	array_longs_2e1ab0() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~array_longs_2e1ab0()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(long), 0x12);
		}
	}
};

struct longs_2e1ab0 : array_longs_2e1ab0
{
	long *stack_block;
	long stack_capacity;

	longs_2e1ab0(long capacity);
	~longs_2e1ab0();
};

// segments that all start at one point: their count, where their ends are
// and how far apart, and the axes to start from (the sweep's own when null)
struct rays_2e3d40
{
	__m128 start;
	long count;
	char const *ends;
	long end_stride;
	endpoints_2e0f80 const *axes;
};

// a box that holds the start point on all three axes is passed to every
// segment's collector, unless it is a grid cell's; each segment keeps the
// smaller distance left
__forceinline void reach_all_2e3d40(char flags, element_2e1090 const *element, rays_2e3d40 const *rays,
	ray_collector_2e4660 *collector, long collector_stride, longs_2e1ab0 const &left)
{
	if (flags == 0x77 && (element->owner & 1) == 0)
	{
		for (int r = 0; r < rays->count; r++)
		{
			float &slot = ((float *)left.data)[r];
			float const distance = collector->add_box(element->owner, r);
			float const kept = slot;
			((float *)left.data)[r] = kept < distance ? kept : distance;
			collector = (ray_collector_2e4660 *)((char *)collector + collector_stride);
		}
	}
}
// each box a segment's end reaches from the start point, by the axes' high
// nibbles (the low nibbles say which boxes hold the start point, and the
// high ones start the same for each segment), is passed to that segment's
// collector, until its distance left is passed; the boxes that hold the
// start point go to every collector
// score 539 (was 539): frame 0xe8 agrees; the first array home and later
// pointer/array homes differ. Vector-word start values 539; signed words
// 539; axis steps in body 541; const endpoints 539; pointer-copy loop 539;
// time loop 538 (NOT kept: gain 1); widening loop 664; scalar arrays 539.
// Round 430: default-axis if 547; indexed axes 532 (NOT kept: gain 7);
// vector role names 539; unsigned axis counters 539; byte toggle 536
// (NOT kept: gain 3); direct byte XOR/test 564; guarded indexed minimum
// 559; explicit else blocks 539. All executable forms restored.
// @retail 0x2e3d40
void class_2e1090::function_2e3d40(rays_2e3d40 const *rays, ray_collector_2e4660 *collectors, long collector_stride)
{
	__declspec(align(16)) unsigned long start_words[4];
	{
		corner_words_2e1090 words;
		quantize_corner_2e1090(rays->start, lower_offset, scale, *(__m128 const *)g_4125e0, *(__m128 const *)g_4125f0, words);
		start_words[0] = words.words[0];
		start_words[1] = words.words[1];
		start_words[2] = words.words[2];
		start_words[3] = words.words[3];
	}
	// one byte per box: the low nibble a bit for each axis whose interval
	// holds the start point, the high nibble the same for the walk
	int const count = elements.size;
	bytes_2e1be0 flags(count + 16);
	clear_blocks_2e1090(flags.data, count);
	endpoint_2e0f80 *start_position[3];
	{
		endpoints_2e0f80 const *axis = rays->axes ? rays->axes : axes;
		char *const bits = flags.data;
		int bit = 0x11;
		for (int i = 0; i < 3; i++, axis++, bit <<= 1)
		{
			endpoint_2e0f80 *data = axis->data;
			long const size = axis->size;
			endpoint_2e0f80 const *const middle = data + (size >> 1);
			unsigned long const point = start_words[i];
			endpoint_2e0f80 *p;
			if (point < middle->value)
			{
				endpoint_2e0f80 *const end = data + size - 4;
				for (p = data + 1; p < end && p[3].value <= point; p += 4)
				{
					bits[p[0].id] ^= bit;
					bits[p[1].id] ^= bit;
					bits[p[2].id] ^= bit;
					bits[p[3].id] ^= bit;
				}
				for (; p->value <= point; p++)
				{
					bits[p->id] ^= bit;
				}
			}
			else
			{
				endpoint_2e0f80 *const begin = data + 4;
				for (p = data + size - 2; p >= begin && p[-3].value > point; p -= 4)
				{
					bits[p[0].id] ^= bit;
					bits[p[-1].id] ^= bit;
					bits[p[-2].id] ^= bit;
					bits[p[-3].id] ^= bit;
				}
				for (; p->value > point; p--)
				{
					bits[p->id] ^= bit;
				}
				p++;
			}
			start_position[i] = p;
		}
	}

	// each segment's distance left
	longs_2e1ab0 left(rays->count);
	left.size = rays->count;
	for (int i = 0; i < rays->count; i++)
	{
		((float *)left.data)[i] = 1.0f;
	}

	// the boxes that hold the start point
	{
		element_2e1090 const *element = elements.data;
		unsigned long const *word = (unsigned long const *)flags.data;
		unsigned long const *const words_end = word + (elements.size >> 2) + 1;
		while (word < words_end)
		{
			if (((word[0] + 0x01010101) & 0x08080808) == 0)
			{
				if (((word[1] + 0x01010101) & 0x08080808) == 0)
				{
					if (((word[2] + 0x01010101) & 0x08080808) == 0)
					{
						element += 12;
						word += 3;
						continue;
					}
					element += 8;
					word += 2;
					continue;
				}
				element += 4;
				word++;
				continue;
			}
			char const *bytes = (char const *)word;
			reach_all_2e3d40(bytes[0], element, rays, collectors, collector_stride, left);
			reach_all_2e3d40(bytes[1], element + 1, rays, collectors, collector_stride, left);
			reach_all_2e3d40(bytes[2], element + 2, rays, collectors, collector_stride, left);
			reach_all_2e3d40(bytes[3], element + 3, rays, collectors, collector_stride, left);
			word++;
			element += 4;
		}
	}
	// the first box's byte stands for the axes' sentinels
	flags.data[0] = (char)0x88;

	ray_collector_2e4660 *collector = collectors;
	for (int r = 0; r < rays->count; collector = (ray_collector_2e4660 *)((char *)collector + collector_stride), r++)
	{
		float distance_left = ((float *)left.data)[r];
		endpoint_2e0f80 *position[3];
		position[0] = start_position[0];
		position[1] = start_position[1];
		position[2] = start_position[2];
		vector_2e4660 inverse;
		vector_2e4660 offset;
		long step[3];
		{
			vector_2e4660 end;
			end.vector = *(__m128 const *)(rays->ends + rays->end_stride * r);
			vector_2e4660 direction;
			direction.vector = _mm_sub_ps(end.vector, rays->start);
			for (int i = 0; i < 3; i++)
			{
				float const scaled = direction.terms[i] * scale.m128_f32[i];
				float const magnitude = (float)fabs(scaled);
				float const start = (lower_offset.m128_f32[i] + rays->start.m128_f32[i]) * scale.m128_f32[i];
				float const finish = (end.terms[i] + lower_offset.m128_f32[i]) * scale.m128_f32[i];
				if (magnitude < start * 1.192092896e-07f || magnitude < 1.192092896e-07f * finish)
				{
					inverse.terms[i] = 0.0f;
					offset.terms[i] = -2.0f;
					continue;
				}
				float const reciprocal = 1.0f / scaled;
				step[i] = sizeof(endpoint_2e0f80);
				if (scaled < 0.0f)
				{
					position[i]--;
					step[i] = -(long)sizeof(endpoint_2e0f80);
				}
				inverse.terms[i] = reciprocal;
				offset.terms[i] = (start - field_7c) * reciprocal;
			}
		}
		float time[3];
		time[0] = (float)position[0]->value * inverse.terms[0] - offset.terms[0];
		time[1] = (float)position[1]->value * inverse.terms[1] - offset.terms[1];
		time[2] = (float)position[2]->value * inverse.terms[2] - offset.terms[2];
		char *const bits = flags.data;
		for (;;)
		{
			int const axis = earliest_2e4660(time);
			float *const next_time = &time[axis];
			if (*next_time > distance_left)
			{
				break;
			}
			char const mask = (char)(0x10 << axis);
			for (;;)
			{
				endpoint_2e0f80 *const p = position[axis];
				unsigned long const id = p->id;
				unsigned char const reached = bits[id] ^ mask;
				bits[id] = reached;
				if (reached >= 0x70)
				{
					if (id == 0)
					{
						*next_time = 2.0f;
						break;
					}
					long const owner = elements.data[id].owner;
					if ((owner & 1) == 0)
					{
						float const distance = collector->add_box(owner, r);
						if (!(distance_left < distance))
						{
							distance_left = distance;
						}
					}
				}
				endpoint_2e0f80 *const next = (endpoint_2e0f80 *)((char *)p + step[axis]);
				position[axis] = next;
				if (p->value != next->value)
				{
					*next_time = (float)next->value * inverse.terms[axis] - offset.terms[axis];
					break;
				}
			}
		}
		// the high nibbles back to the start point's
		if (r < rays->count - 1)
		{
			unsigned long *word = (unsigned long *)bits;
			unsigned long *const words_end = word + (count >> 2) + 1;
			for (; word < words_end; word += 2)
			{
				unsigned long const first = word[0] & 0x0f0f0f0f;
				unsigned long const second = word[1] & 0x0f0f0f0f;
				word[0] = first | (first << 4);
				word[1] = second | (second << 4);
			}
		}
	}
}
// each box the moving box overlaps at the start, then each box whose three
// intervals it enters on the way, is passed to the collector, until the
// collector's distance left is passed
// score 511 (was 511): frame 0x118 agrees; saved receiver and vector
// homes differ, followed by scalar homes and axis registers. Four-entry
// scan loop 682; outer axis cursor 511; shared input vectors 513; minimum
// expression 511; displacement vector type 511; reached-element pointer
// 668; direct time slots 511; byte flag 573. All forms restored.
// @retail 0x2e4660
void class_2e1090::function_2e4660(ray_2e4660 const *ray, ray_collector_2e4660 *collector)
{
	__declspec(align(16)) unsigned long lower_words[4];
	__declspec(align(16)) unsigned long upper_words[4];
	__m128 lower = _mm_sub_ps(ray->from, ray->extent);
	__m128 upper = _mm_add_ps(ray->from, ray->extent);
	{
		corner_words_2e1090 words;
		quantize_corner_2e1090(lower, lower_offset, scale, *(__m128 const *)g_4125e0, *(__m128 const *)g_4125f0, words);
		lower_words[0] = words.words[0];
		lower_words[1] = words.words[1];
		lower_words[2] = words.words[2];
		lower_words[3] = words.words[3];
		quantize_corner_2e1090(upper, lower_offset, scale, *(__m128 const *)g_4125e0, *(__m128 const *)g_4125f0, words);
		upper_words[0] = words.words[0];
		upper_words[1] = words.words[1];
		upper_words[2] = words.words[2];
		upper_words[3] = words.words[3];
	}

	// one byte per box, a bit for each axis whose interval overlaps
	int const count = elements.size;
	bytes_2e1be0 flags(count + 16);
	clear_blocks_2e1090(flags.data, count);
	endpoint_2e0f80 *low_position[3];
	endpoint_2e0f80 *high_position[3];
	{
		endpoints_2e0f80 const *axis = ray->axes ? ray->axes : axes;
		char *const bits = flags.data;
		int bit = 1;
		for (int i = 0; i < 3; i++, axis++, bit <<= 1)
		{
			endpoint_2e0f80 *data = axis->data;
			long const size = axis->size;
			endpoint_2e0f80 *p;
			endpoint_2e0f80 const *const middle = data + (size >> 1);
			if (lower_words[i] < middle->value)
			{
				endpoint_2e0f80 *const end = data + size - 4;
				for (p = data + 1; p < end && p[3].value <= lower_words[i]; p += 4)
				{
					bits[p[0].id] ^= bit;
					bits[p[1].id] ^= bit;
					bits[p[2].id] ^= bit;
					bits[p[3].id] ^= bit;
				}
				for (; p->value <= lower_words[i]; p++)
				{
					bits[p->id] ^= bit;
				}
				low_position[i] = p;
				for (; p->value <= upper_words[i]; p++)
				{
					bits[p->id] ^= ((p->value & 1) - 1) & bit;
				}
				high_position[i] = p;
			}
			else
			{
				endpoint_2e0f80 *const begin = data + 4;
				for (p = data + size - 2; p >= begin && p[-3].value > upper_words[i]; p -= 4)
				{
					bits[p[0].id] ^= bit;
					bits[p[-1].id] ^= bit;
					bits[p[-2].id] ^= bit;
					bits[p[-3].id] ^= bit;
				}
				for (; p->value > upper_words[i]; p--)
				{
					bits[p->id] ^= bit;
				}
				high_position[i] = p + 1;
				for (; p->value > lower_words[i]; p--)
				{
					bits[p->id] ^= -(p->value & 1) & bit;
				}
				low_position[i] = p + 1;
			}
		}
	}

	// the boxes overlapped at the start
	float left = 1.0f;
	{
		element_2e1090 const *element = elements.data;
		int const inner_count = elements.size;
		unsigned long const *word = (unsigned long const *)flags.data;
		unsigned long const *const words_end = word + (inner_count >> 2) + 1;
		for (; word < words_end; element += 4, word++)
		{
			if (((*word + 0x01010101) & 0x08080808) == 0)
			{
				continue;
			}
			char const *bytes = (char const *)word;
			reach_2e4660(bytes[0], element + 0, collector, &left);
			reach_2e4660(bytes[1], element + 1, collector, &left);
			reach_2e4660(bytes[2], element + 2, collector, &left);
			reach_2e4660(bytes[3], element + 3, collector, &left);
		}
	}

	// each axis's step, and the times at which the box's ends reach an
	// endpoint
	vector_2e4660 inverse;
	vector_2e4660 low_offset;
	vector_2e4660 high_offset;
	long step[3];
	long high_toggle[3];
	long low_toggle[3];
	{
		__m128 const delta = _mm_sub_ps(ray->to, ray->from);
		for (int i = 0; i < 3; i++)
		{
			float const scaled = delta.m128_f32[i] * scale.m128_f32[i];
			float const magnitude = (float)fabs(scaled);
			if (scaled > 0.0f)
			{
				step[i] = sizeof(endpoint_2e0f80);
				low_toggle[i] = 0;
				high_toggle[i] = 1;
			}
			else
			{
				step[i] = -(long)sizeof(endpoint_2e0f80);
				endpoint_2e0f80 *const position = low_position[i];
				low_position[i] = high_position[i];
				high_position[i] = position;
				float const corner = lower.m128_f32[i];
				lower.m128_f32[i] = upper.m128_f32[i];
				upper.m128_f32[i] = corner;
				unsigned long const words = lower_words[i];
				lower_words[i] = upper_words[i];
				upper_words[i] = words;
				low_position[i]--;
				high_position[i]--;
				low_toggle[i] = 1;
				high_toggle[i] = 0;
			}
			float const start = (lower.m128_f32[i] + lower_offset.m128_f32[i]) * scale.m128_f32[i];
			float const finish = (ray->to.m128_f32[i] + lower_offset.m128_f32[i]) * scale.m128_f32[i];
			if (magnitude < 1.192092896e-07f * start || magnitude < finish * 1.192092896e-07f)
			{
				inverse.terms[i] = 0.0f;
				low_offset.terms[i] = -2.0f;
				high_offset.terms[i] = -2.0f;
				continue;
			}
			float const reciprocal = 1.0f / scaled;
			inverse.terms[i] = reciprocal;
			{
				low_offset.terms[i] = ((lower.m128_f32[i] + lower_offset.m128_f32[i]) * scale.m128_f32[i] - field_7c) * reciprocal;
			}
			{
				high_offset.terms[i] = ((upper.m128_f32[i] + lower_offset.m128_f32[i]) * scale.m128_f32[i] - field_7c) * reciprocal;
			}
		}
	}
	float low_time[3];
	float high_time[3];
	low_time[0] = (float)low_position[0]->value * inverse.terms[0] - low_offset.terms[0];
	low_time[1] = (float)low_position[1]->value * inverse.terms[1] - low_offset.terms[1];
	low_time[2] = (float)low_position[2]->value * inverse.terms[2] - low_offset.terms[2];
	high_time[0] = (float)high_position[0]->value * inverse.terms[0] - high_offset.terms[0];
	high_time[1] = (float)high_position[1]->value * inverse.terms[1] - high_offset.terms[1];
	high_time[2] = (float)high_position[2]->value * inverse.terms[2] - high_offset.terms[2];
	char *const bits = flags.data;
	// the first box's byte stands for the axes' sentinels
	bits[0] = 8;

	// the walk: the leading ends enter boxes, the trailing ends leave them
	int low_axis = earliest_2e4660(low_time);
	int high_axis = earliest_2e4660(high_time);
	for (;;)
	{
		float *const high_next = &high_time[high_axis];
		float *const low_next = &low_time[low_axis];
		if (*high_next < *low_next)
		{
			if (*high_next > left)
			{
				break;
			}
			for (;;)
			{
				endpoint_2e0f80 *const p = high_position[high_axis];
				unsigned long const id = p->id;
				unsigned char const reached = bits[id] ^ (char)(((p->value & 1) ^ (char)high_toggle[high_axis]) << high_axis);
				bits[id] = reached;
				if (reached >= 7)
				{
					if (id == 0)
					{
						*high_next = 2.0f;
						break;
					}
					long const owner = elements.data[id].owner;
					if ((owner & 1) == 0)
					{
						float const distance = collector->add_box(owner, 0);
						if (!(left < distance))
						{
							left = distance;
						}
					}
				}
				endpoint_2e0f80 *const next = (endpoint_2e0f80 *)((char *)p + step[high_axis]);
				high_position[high_axis] = next;
				if (p->value != next->value)
				{
					*high_next = (float)next->value * inverse.terms[high_axis] - high_offset.terms[high_axis];
					break;
				}
			}
			high_axis = earliest_2e4660(high_time);
		}
		else
		{
			if (*low_next > left)
			{
				break;
			}
			char const toggle = (char)low_toggle[low_axis];
			for (;;)
			{
				endpoint_2e0f80 *const p = low_position[low_axis];
				unsigned long const id = p->id;
				unsigned char const left_box = bits[id] ^ (char)(((p->value & 1) ^ toggle) << low_axis);
				bits[id] = left_box;
				if (left_box > 8)
				{
					*low_next = 2.0f;
					break;
				}
				endpoint_2e0f80 *const next = (endpoint_2e0f80 *)((char *)p + step[low_axis]);
				low_position[low_axis] = next;
				if (p->value != next->value)
				{
					*low_next = (float)next->value * inverse.terms[low_axis] - low_offset.terms[low_axis];
					break;
				}
			}
			low_axis = earliest_2e4660(low_time);
		}
	}
}
