// @flags /O2 /GL-
// Havok (third-party physics library): a capsule against a triangle: up to
// two contacts, from the capsule's ends projected onto the triangle, from the
// closest points of its segment and the triangle's edges, or where the
// segment passes through the triangle. The agent that keeps contacts (with
// ids the owner hands out while a contact lies within the tolerance, and
// takes back when it no longer does) gets up to three: when the segment lies
// outside an edge at both ends, the edges are taken as segments of a capsule.
// The queries are marked in the timing stream.
// The cached and uncached queries share the same endpoint and edge tests.
#include "pair_agents_2eab70.h"
#include "timer_mark_2ebc80.h"
#include "growable_array_2d91d0.h"
#include "triangle_agent_2f42f0.h"
#include <math.h>
#include <xmmintrin.h>

static __declspec(align(16)) const float g_412420[4] = {1.0f, 1.0f, 1.0f, 1.0f};
static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

// corner indices: the corner before a corner at [i], and after it at [i + 2]
static const signed char g_4136b0[5] = {2, 0, 1, 2, 0};

// three axes and a position
struct frame_2f4f00
{
	__m128 axes[3];
	__m128 position;
};

// a point taken out of a frame
inline __m128 frame_point_2f4f00(frame_2f4f00 const *m, __m128 const *point)
{
	__m128 p = *point;
	__m128 z = _mm_add_ps(_mm_mul_ps(m->axes[2], _mm_shuffle_ps(p, p, 0xaa)), m->position);
	__m128 y = _mm_mul_ps(m->axes[1], _mm_shuffle_ps(p, p, 0x55));
	__m128 x = _mm_mul_ps(m->axes[0], _mm_shuffle_ps(p, p, 0x00));
	return _mm_add_ps(_mm_add_ps(x, y), z);
}

// where a body sits: two vectors, then its frame
struct placement_2f4f00
{
	__m128 field_0;
	__m128 field_10;
	frame_2f4f00 frame;
};

// a capsule or a triangle: its radius and its points (two ends or three corners)
struct shape_2f4f00
{
	void const *table;
	long field_4;
	long field_8;
	float radius;
	__m128 points[3];
};

// one side of a query: a shape, the key of the part concerned, where it sits,
// and the side it is a part of
struct body_2f4f00
{
	shape_2f4f00 const *shape;
	long key;
	placement_2f4f00 const *placement;
	body_2f4f00 const *parent;
};

struct input_2f4f00
{
	long field_0;
	long field_4;
	float tolerance;
};

// takes the pairs a query finds
class pair_collector_2f4f00
{
public:
	virtual void slot_0() {}
	virtual void add_pair(void const *a, void const *b) {}
};

// a contact: its point, with the separation in the fourth term, its normal,
// and the two sides
struct contact_2f4f00
{
	__m128 point;
	__m128 normal;
	void const *a;
	void const *b;
};

// the closest point on the first segment, the offset from the closest point
// on the second, and the square of that offset's length (0x308590)
struct closest_points_308590
{
	__m128 first_point;
	__m128 offset;
	float distance_squared;
};

int __cdecl function_308590(__m128 const *first_start, __m128 const *first_direction, __m128 const *second_start, __m128 const *second_direction, closest_points_308590 *result);

// the weight of each edge, of the normal, and of the last row (0x308c20)
struct weights_308c20
{
	float edges[3];
	float normal;
	float last;
};

struct vector_308c20
{
	__m128 value;
};

// the triangle's edge planes and its plane, as a frame
struct matrix_308c20
{
	vector_308c20 rows[4];
};

void __cdecl function_308c20(__m128 const *corners, weights_308c20 const *weights, matrix_308c20 *out, __m128 *normal);

// the object kept for the pair: the triangle's weights from its 20th byte
struct triangle_agent_2f4f00
{
	char unknown0[0x14];
	weights_308c20 weights;
};

// the cross product of two vectors' first three terms
inline __m128 cross_2f4f00(__m128 const &a, __m128 const &b)
{
	__m128 b_yzx = _mm_shuffle_ps(b, b, 0xc9);
	__m128 a_zxy = _mm_shuffle_ps(a, a, 0xd2);
	__m128 second = _mm_mul_ps(a_zxy, b_yzx);
	__m128 b_zxy = _mm_shuffle_ps(b, b, 0xd2);
	__m128 a_yzx = _mm_shuffle_ps(a, a, 0xc9);
	return _mm_sub_ps(_mm_mul_ps(a_yzx, b_zxy), second);
}

// the sum of a vector's first three terms, in the first
inline __m128 sum_2f4f00(__m128 const &v)
{
	return _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v));
}

// a vector of four copies of a float
inline __m128 splat_2f4f00(float value)
{
	__m128 const single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

// a vector scaled to unit length
inline __m128 normalized_2f4f00(__m128 const &v)
{
	__m128 const sum = sum_2f4f00(_mm_mul_ps(v, v));
	__m128 const inverse = _mm_rsqrt_ss(sum);
	__m128 const correction = _mm_sub_ss(_mm_set_ss(3.0f), _mm_mul_ss(_mm_mul_ss(sum, inverse), inverse));
	__m128 const scale = _mm_mul_ss(_mm_mul_ss(_mm_set_ss(0.5f), inverse), correction);
	return _mm_mul_ps(_mm_shuffle_ps(scale, scale, 0), v);
}

// a vector of three terms and a zero
inline void set_vector_2f4f00(__m128 &v, float x, float y, float z)
{
	float *terms = (float *)&v;
	terms[3] = 0.0f;
	terms[0] = x;
	terms[1] = y;
	terms[2] = z;
}

// the library's true or false value: one byte
struct bool_2f4f00
{
	bool value;

	bool_2f4f00(bool value_)
	{
		value = value_;
	}
};

// a vector, copied a lane group at a time, with its lanes as floats
struct vector_2f4f00
{
	__m128 value;

	void operator=(__m128 const &other)
	{
		value = other;
	}
	float &operator()(int index)
	{
		return value.m128_f32[index];
	}
	float const &operator()(int index) const
	{
		return value.m128_f32[index];
	}
};

// a contact found by a query: its point, with the separation in the fourth
// term, its normal, and an id
struct candidate_2f4f00
{
	vector_2f4f00 point;
	vector_2f4f00 normal;
	unsigned short id;
};

struct contact_point_2f28f0;
struct body_2f28f0;

void __cdecl function_2f28f0(body_2f28f0 const *body, __m128 const *segment, float radius, contact_point_2f28f0 *points);

typedef void *(__fastcall *constructor_proc)(void *);
void __stdcall vector_constructor_iterator(void *array, unsigned size, int count, constructor_proc constructor);
void *__fastcall function_31bc70(void *arg_0);

// the up to two contacts of a capsule and a triangle, each with the
// separation in its point's fourth term, or that term left at the largest
// float when there is none; with edges_as_segments, a segment outside an edge
// at both ends gets the contacts of those edges taken as a capsule's segments
// instead
__forceinline void capsule_triangle_2f4f00(void const *a, void const *b, void const *input, weights_308c20 const *weights, int edges_as_segments, candidate_2f4f00 *contacts)
{
	shape_2f4f00 const *capsule = ((body_2f4f00 const *)a)->shape;
	shape_2f4f00 const *triangle = ((body_2f4f00 const *)b)->shape;
	__m128 ends[2];
	__m128 corners[3];
	__m128 direction;
	{
		frame_2f4f00 const *frame = &((body_2f4f00 const *)a)->placement->frame;
		__m128 const *end = capsule->points;
		__m128 *placed = ends;
		for (int count = 2; count != 0; count--)
		{
			*placed = frame_point_2f4f00(frame, end);
			placed++;
			end++;
		}
	}
	{
		__m128 const *corner = triangle->points;
		frame_2f4f00 const *frame = &((body_2f4f00 const *)b)->placement->frame;
		__m128 *placed = corners;
		for (int count = 3; count != 0; count--)
		{
			*placed = frame_point_2f4f00(frame, corner);
			placed++;
			corner++;
		}
	}
	matrix_308c20 planes;
	__m128 normal;
	function_308c20(corners, weights, &planes, &normal);

	// each end's distances outside the three edges, and above the triangle
	__m128 heights[2];
	heights[0] = _mm_sub_ps(ends[0], corners[0]);
	heights[1] = _mm_sub_ps(ends[1], corners[0]);
	{
		frame_2f4f00 const *frame = (frame_2f4f00 const *)&planes;
		__m128 *height = heights;
		for (int count = 2; count != 0; count--)
		{
			*height = frame_point_2f4f00(frame, height);
			height++;
		}
	}
	float const radius = capsule->radius + triangle->radius;
	float const reach = radius + ((input_2f4f00 const *)input)->tolerance;
	contacts[0].point(3) = 3.402823466e+38f;
	contacts[1].point(3) = 3.402823466e+38f;
	contacts[2].point(3) = 3.402823466e+38f;
	__m128 const reach4 = splat_2f4f00(reach);
	if (_mm_movemask_ps(_mm_cmplt_ps(reach4, heights[1])) & _mm_movemask_ps(_mm_cmplt_ps(reach4, heights[0])))
	{
		return;
	}
	if (heights[0].m128_f32[3] < -reach && heights[1].m128_f32[3] < -reach)
	{
		return;
	}
	int signs[2];
	signs[0] = _mm_movemask_ps(heights[0]);
	signs[1] = _mm_movemask_ps(heights[1]);
	if ((signs[0] ^ signs[1]) & 8)
	{
		// the segment passes through the triangle's plane: where, in the edges' terms
		float const along = heights[0].m128_f32[3] / (heights[0].m128_f32[3] - heights[1].m128_f32[3]);
		__m128 const along4 = splat_2f4f00(along);
		__m128 const crossing = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(*(__m128 const *)g_412420, along4), heights[0]), _mm_mul_ps(along4, heights[1]));
		if ((_mm_movemask_ps(crossing) & 7) == 7)
		{
			// inside the triangle: the deeper end, or the closest point of an
			// edge to the segment
			candidate_2f4f00 *contact = &contacts[1];
			contact->point(3) = -3.402823466e+38f;
			for (int end = 0; end < 2; end++)
			{
				int const sign = signs[end];
				if ((sign & 7) == 7)
				{
					float const separation = -(float)fabs(heights[end].m128_f32[3]) - radius;
					if (separation > contact->point(3))
					{
						if (sign & 8)
						{
							contact->normal = normal;
							float const shift = triangle->radius - heights[end].m128_f32[3];
							contact->point = _mm_add_ps(ends[end], _mm_mul_ps(splat_2f4f00(shift), normal));
						}
						else
						{
							contact->normal = _mm_xor_ps(normal, *(__m128 const *)g_4124a0);
							float const shift = -triangle->radius - heights[end].m128_f32[3];
							contact->point = _mm_add_ps(ends[end], _mm_mul_ps(splat_2f4f00(shift), normal));
						}
						contact->point(3) = separation;
					}
				}
			}
			__m128 change = _mm_sub_ps(heights[1], heights[0]);
			float const rise_squared = change.m128_f32[3] * change.m128_f32[3];
			float const height = heights[0].m128_f32[3];
			for (int edge = 0; edge < 3; edge++)
			{
				float scale = change.m128_f32[3];
				float slope = change.m128_f32[edge];
				float const inverse = 1.0f / (slope * slope + rise_squared);
				// Keep the height load before the slope multiplication.
				float const volatile &height_for_offset = heights[0].m128_f32[3];
				float const offset = height_for_offset * slope - change.m128_f32[3] * heights[0].m128_f32[edge];
				float const distance_squared = offset * offset * inverse;
				if (distance_squared < (contact->point(3) + radius) * (contact->point(3) + radius))
				{
					float const at = -((slope * heights[0].m128_f32[edge]) + change.m128_f32[3] * height) * inverse;
					if (at > -0.0001f && at < 1.0001f)
					{
						__m128 plane;
						set_vector_2f4f00(plane, planes.rows[0].value.m128_f32[edge], planes.rows[1].value.m128_f32[edge], planes.rows[2].value.m128_f32[edge]);
						if (change.m128_f32[3] < 0.0f)
						{
							scale = -change.m128_f32[3];
							slope = -slope;
						}
						__m128 const scale4 = splat_2f4f00(scale);
						float const distance = (float)sqrt(distance_squared);
						plane = _mm_mul_ps(scale4, plane);
						__m128 const slope4 = splat_2f4f00(-slope);
						contact->normal = normalized_2f4f00(_mm_add_ps(plane, _mm_mul_ps(slope4, normal)));
						__m128 const at4 = splat_2f4f00(at);
						float const depth = -distance;
						contact->point = _mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_sub_ps(*(__m128 const *)g_412420, at4), ends[0]), _mm_mul_ps(at4, ends[1])), _mm_mul_ps(splat_2f4f00(triangle->radius - depth), contact->normal.value));
						contact->point(3) = depth - radius;
					}
				}
			}
			return;
		}
	}

	// each end that lies over the triangle gives a contact, and each that does
	// not, the closest of the edges; when the segment passes outside one edge
	// on both ends, only the closest of the edges
	{
		direction = _mm_sub_ps(ends[1], ends[0]);
		int end = 0;
		candidate_2f4f00 *contact = contacts;
		bool_2f4f00 outside = false;
		if (((signs[0] | signs[1]) & 7) != 7)
		{
			if (edges_as_segments)
			{
				// outside an edge at both ends: each such edge taken as a
				// capsule's segment
				float const tolerance = ((input_2f4f00 const *)input)->tolerance;
				contacts[0].point(3) = tolerance;
				contacts[1].point(3) = tolerance;
				contacts[2].point(3) = tolerance;
				__m128 segment[2];
				vector_constructor_iterator(segment, sizeof(__m128), 2, function_31bc70);
				int mask = signs[0] | signs[1];
				int bit = 1;
				for (int edge = 0; edge < 3; bit <<= 1, edge++)
				{
					if (mask & bit)
					{
						continue;
					}
					segment[0] = corners[g_4136b0[edge + 2]];
					segment[1] = corners[g_4136b0[edge]];
					function_2f28f0((body_2f28f0 const *)a, segment, triangle->radius, (contact_point_2f28f0 *)contacts);
				}
				return;
			}
			// outside mode goes straight to the edges of end 0
			outside = true;
			goto edges;
		}
		for (; end < 2;)
		{
			if ((signs[end] & 7) == 7)
			{
				if (signs[end] & 8)
				{
					float const shift = -triangle->radius - heights[end].m128_f32[3];
					contact->point = _mm_add_ps(ends[end], _mm_mul_ps(splat_2f4f00(shift), normal));
					contact->point(3) = -heights[end].m128_f32[3] - radius;
					contact->normal = _mm_xor_ps(normal, *(__m128 const *)g_4124a0);
				}
				else
				{
					float const shift = triangle->radius - heights[end].m128_f32[3];
					contact->point = _mm_add_ps(ends[end], _mm_mul_ps(splat_2f4f00(shift), normal));
					contact->point(3) = heights[end].m128_f32[3] - radius;
					contact->normal = normal;
				}
			}
			else
			{
			edges:
				int bit = 1;
				for (int edge = 0; edge < 3; bit <<= 1, edge++)
				{
					if (!outside.value)
					{
						if (signs[end] & bit)
						{
							continue;
						}
					}
					else if (signs[0] & (bit & signs[1]))
					{
						continue;
					}
					__m128 const *start = &corners[g_4136b0[edge + 2]];
					__m128 const side = _mm_sub_ps(corners[g_4136b0[edge]], *start);
					closest_points_308590 closest;
					int const clamped = function_308590(&ends[0], &direction, start, &side, &closest);
					float const limit = radius + contact->point(3);
					if (!(closest.distance_squared < limit * limit))
					{
						continue;
					}
					if (clamped & (1 << end))
					{
						if (!outside.value)
						{
							continue;
						}
						end = 1;
					}
					__m128 across;
					if (clamped == 0)
					{
						across = cross_2f4f00(direction, side);
					}
					else if (!(closest.distance_squared <= 1.4210854715202004e-14f))
					{
						across = closest.offset;
					}
					else
					{
						set_vector_2f4f00(closest.offset, planes.rows[0].value.m128_f32[edge], planes.rows[1].value.m128_f32[edge], planes.rows[2].value.m128_f32[edge]);
						across = cross_2f4f00(direction, side);
						float length_squared;
						_mm_store_ss(&length_squared, sum_2f4f00(_mm_mul_ps(across, across)));
						if (length_squared <= 1.4210854715202004e-14f)
						{
							across = closest.offset;
						}
					}
					across = normalized_2f4f00(across);
					float dot;
					_mm_store_ss(&dot, sum_2f4f00(_mm_mul_ps(across, closest.offset)));
					float distance = dot;
					if (dot < 0.0f)
					{
						across = _mm_xor_ps(across, *(__m128 const *)g_4124a0);
						distance = -dot;
					}
					contact->point = _mm_add_ps(closest.first_point, _mm_mul_ps(splat_2f4f00(triangle->radius - distance), across));
					contact->point(3) = distance - radius;
					contact->normal = across;
				}
				if (outside.value)
				{
					break;
				}
			}
			contact++;
			end++;
		}
	}
}

// a contact kept: its point, with the separation in the fourth term, its
// normal, and its id
struct contact_2f4440
{
	__m128 point;
	__m128 normal;
	unsigned short id;
};

ELEMENTS_2D91D0_ALIGNED(contacts_2f4440, contact_2f4440);

struct results_2f4440
{
	contacts_2f4440 *contacts;
};

// hands out the ids of contacts, and takes them back
class contact_owner_2f4440
{
public:
	virtual void slot_0() {}
	virtual unsigned short add_contact(void const *a, void const *b, void const *input, contact_point_2f28f0 *contact) { return 0; }
	virtual void release_id(unsigned short id) {}
};

// the object kept for the pair: the owner, the three contacts' ids, and the
// triangle's weights
struct agent_2f4440
{
	char unknown0[8];
	contact_owner_2f4440 *owner;
	unsigned short ids[3];
	weights_308c20 weights;
};

// Contacts within the tolerance are added with their persistent owner IDs.
// @retail 0x2f4440
void class_2f4440::function_2f4440(void const *a, void const *b, void const *input, void *collector)
{
	timer_add_2ebc80("TtCapsuleTri");
	agent_2f4440 *agent = (agent_2f4440 *)this;
	candidate_2f4f00 points[3];
	capsule_triangle_2f4f00(a, b, input, &agent->weights, 1, points);
	for (int i = 0; i < 3; i++)
	{
		if (points[i].point(3) < ((input_2f4f00 const *)input)->tolerance)
		{
			if (agent->ids[i] == 0xffff)
			{
				agent->ids[i] = agent->owner->add_contact(a, b, input, (contact_point_2f28f0 *)&points[i]);
				if (agent->ids[i] == 0xffff)
				{
					continue;
				}
			}
			contact_2f4440 &kept = ((results_2f4440 *)collector)->contacts->next_slot();
			kept.point = points[i].point.value;
			kept.normal = points[i].normal.value;
			kept.id = agent->ids[i];
		}
		else if (agent->ids[i] != 0xffff)
		{
			agent->owner->release_id(agent->ids[i]);
			agent->ids[i] = 0xffff;
		}
	}
	timer_add_2ebc80("Et");
}

// takes the contacts a query finds
class contact_collector_2f4f00
{
public:
	virtual void slot_0() {}
	virtual void add_contact(contact_2f4f00 const *contact) {}
};

// the closer of a capsule's and a triangle's two contacts, when it is within
// the tolerance
__forceinline bool closest_contact_2f4f00(void const *a, void const *b, void const *input, weights_308c20 const *weights, contact_2f4f00 *out)
{
	candidate_2f4f00 contacts[3];
	capsule_triangle_2f4f00(a, b, input, weights, 0, contacts);
	if (contacts[0].point(3) < contacts[1].point(3))
	{
		if (contacts[0].point(3) < ((input_2f4f00 const *)input)->tolerance)
		{
			out->point = contacts[0].point.value;
			out->normal = contacts[0].normal.value;
			return true;
		}
		return false;
	}
	if (contacts[1].point(3) < ((input_2f4f00 const *)input)->tolerance)
	{
		out->point = contacts[1].point.value;
		out->normal = contacts[1].normal.value;
		return true;
	}
	return false;
}

// the pair passed to the collector when either contact overlaps
__forceinline void report_pair_2f4f00(candidate_2f4f00 const *contacts, void const *a, void const *b, void *collector)
{
	// Preserve both separation reads, including the no-contact paths.
	float const volatile &first = contacts[0].point(3);
	float const volatile &second = contacts[1].point(3);
	if (first < 0.0f || second < 0.0f)
	{
		((pair_collector_2f4f00 *)collector)->add_pair(a, b);
	}
}

// The closer contact, using the triangle weights cached in the agent.
// @retail 0x2f4f00
void class_2f4f00::function_2f4f00(void const *a, void const *b, void const *input, void *collector)
{
	timer_add_2ebc80("TtCapsuleTriangle");
	contact_2f4f00 out;
	out.a = a;
	out.b = b;
	if (closest_contact_2f4f00(a, b, input, &((triangle_agent_2f4f00 const *)this)->weights, &out))
	{
		((contact_collector_2f4f00 *)collector)->add_contact(&out);
	}
	timer_add_2ebc80("Et");
}

// The closer contact, computing the triangle weights for this query.
// @retail 0x2f5960
void __cdecl function_2f5960(void const *a, void const *b, void const *input, void *collector)
{
	timer_add_2ebc80("TtCapsuleTriangle");
	triangle_measures_308a50 measures;
	function_308a50(((body_2f4f00 const *)b)->shape->points, &measures);
	contact_2f4f00 out;
	out.a = a;
	out.b = b;
	if (closest_contact_2f4f00(a, b, input, (weights_308c20 const *)&measures, &out))
	{
		((contact_collector_2f4f00 *)collector)->add_contact(&out);
	}
	timer_add_2ebc80("Et");
}

// The first transformed-end store still exchanges its address registers
// in the SIB encoding; the effective address and all other instructions agree.
// The pair passed to the collector when the capsule and the triangle overlap,
// with the triangle's weights kept in the object
// @retail 0x2f63d0
void class_2f63d0::function_2f63d0(void const *a, void const *b, void const *input, void *collector)
{
	timer_add_2ebc80("TtCapsuleTriangle");
	candidate_2f4f00 contacts[3];
	capsule_triangle_2f4f00(a, b, input, &((triangle_agent_2f4f00 const *)this)->weights, 0, contacts);
	report_pair_2f4f00(contacts, a, b, collector);
	timer_add_2ebc80("Et");
}

// The first transformed-end store still exchanges its address registers
// in the SIB encoding; the effective address and all other instructions agree.
// The pair passed to the collector when the capsule and the triangle overlap,
// with the triangle's weights worked out here
// @retail 0x2f6db0
void __cdecl function_2f6db0(void const *a, void const *b, void const *input, void *collector)
{
	timer_add_2ebc80("TtCapsuleTriangle");
	triangle_measures_308a50 measures;
	function_308a50(((body_2f4f00 const *)b)->shape->points, &measures);
	candidate_2f4f00 contacts[3];
	capsule_triangle_2f4f00(a, b, input, (weights_308c20 const *)&measures, 0, contacts);
	report_pair_2f4f00(contacts, a, b, collector);
	timer_add_2ebc80("Et");
}
