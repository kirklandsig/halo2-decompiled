// @flags /O2 /GL- /GX
// Havok (third-party physics library): the penetration query between two
// shapes: a simplex of support points grown towards the origin, and when it
// encloses the origin, a hull expanded face by face until the face nearest
// the origin lies on the shapes' difference; restarted with the position
// moved by a tiny random offset when the hull fails.
#include <math.h>
#include <xmmintrin.h>
#include "reference_counted_2d88a0.h"

static __declspec(align(16)) const float g_412420[4] = {1.0f, 1.0f, 1.0f, 1.0f};
static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
static __declspec(align(16)) const unsigned long g_455940[4] = {0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff};
static __declspec(align(16)) const float g_461f20[4] = {0.001f, 0.001f, 0.001f, 0.001f};

// the zero vector (src/unknown_308830.cpp)
extern __m128 g_4801e0;

// Matrix columns read by the vector rotation methods.
struct c_query_transform
{
	__m128 axes[3];
};

// a direction carried through a transform's axes (0x2da730)
struct vector_2da6e0
{
	__m128 value;
	void rotate(const c_query_transform *matrix, const __m128 *direction);
	void inverse_rotate(const c_query_transform *matrix, const __m128 *direction);
};

// a point's weights over a triangle's corners (0x308260)
void __cdecl function_308260(__m128 const *point, __m128 const *a, __m128 const *b, __m128 const *c, float *weights);

// a support point: the first shape's point, the second's in its own frame,
// and that point in the first shape's frame (src/unknown_3093e0.cpp)
struct support_309300
{
	__m128 first;
	__m128 second;
	__m128 second_placed;
};

class shape_309300;

// the supporting point of the pair along a direction (0x309300)
class pair_309300
{
public:
	__m128 axes[3];
	__m128 position;
	shape_309300 *first;
	shape_309300 *second;

	void function_309300(__m128 const *direction, support_309300 *out) const;
};

// a support point and how far along the search direction it lies
struct scored_support_3093e0
{
	support_309300 support;
	float distance;
};

// what the query gives back: the direction of the penetration, the point,
// the distance and a fixed one half
struct first_step_3094d0
{
	__m128 direction;
	__m128 point;
	float distance;
	float field_24;
};

// the one-byte answer of a step, returned through a hidden pointer
class answer_3094d0
{
public:
	answer_3094d0() {}
	answer_3094d0(bool value_) : value((char)value_) {}

	char value;
};

struct simplex_309200;

// the steps of the search over the simplex (src/unknown_3093e0.cpp)
class search_3093e0
{
public:
	__m128 axes[3];
	__m128 position;
	shape_309300 *first;
	shape_309300 *second;
	simplex_309200 *simplex;
	long field_4c;
	__m128 *differences;

	void function_3093e0(__m128 const *direction, __m128 const *origin, scored_support_3093e0 *out);
	answer_3094d0 function_3094d0(first_step_3094d0 *out);
	void function_309610();
	void function_309700();
};

// three pseudo-random values between two bounds (0x309270)
class random_309270
{
public:
	void function_309270(long seed, float low, float high, float *out);
};

struct face_30aac0;

struct edge_30aac0
{
	__m128 const *vertex;
	edge_30aac0 const *next;
	edge_30aac0 const *twin;
	face_30aac0 *face;
};

// a face of the hull: its plane, and its three edges
struct face_30aac0
{
	__m128 normal;
	float distance;
	edge_30aac0 edges[3];
	char mark;
};

struct entry_309120;

// the hull's faces, seen as a list (0x309120)
class list_309120
{
public:
	entry_309120 *function_309120() const;
};

// a vertex of the hull
struct vertex_309b50
{
	__m128 position;
	char unknown10[0x10];
};

// the hull's storage, and its destructor (0x309990)
class holder_309990
{
public:
	char unknown0[0xc];
	long face_pointer_count;
	char unknown10[0x1a0];
	face_30aac0 *faces;
	long face_count;
	long face_capacity_and_flags;
	char unknown1bc[0x1f44];
	vertex_309b50 *vertices;
	long vertex_count;
	long vertex_capacity_and_flags;

	~holder_309990();
};

// the hull built from the simplex's four points (0x309a50)
class class_30a810 : public holder_309990
{
public:
	char unknown210c[4];
	__m128 embedded_vertices[0x6e];

	class_30a810(__m128 const *a, __m128 const *b, __m128 const *c, __m128 const *d);
	face_30aac0 *function_30aac0(face_30aac0 *start);
	int function_30abe0(int *count, edge_30aac0 const **found, face_30aac0 *face, __m128 const *point);
	int function_30ae70(long count, edge_30aac0 *horizon, __m128 const *position);
};

// an array of points with room for 55 of them held inside it
struct points_309b50
{
	__m128 *data;
	int size;
	int capacity_and_flags;
	__m128 embedded[55];

	__forceinline points_309b50()
	{
		data = embedded;
		size = 0;
		capacity_and_flags = 0x80000000 | 55;
	}
	__forceinline ~points_309b50()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(__m128), 0x12);
		}
	}
};

// the simplex: each point's first-shape point (and its second-shape point
// four entries on), and the number of points
struct simplex_309b50
{
	__m128 points[8];
	long count;
};

class class_309b50
{
public:
	__m128 axes[3];
	__m128 position;
	shape_309300 *first;
	shape_309300 *second;
	simplex_309b50 *simplex;
	long attempts;
	__m128 *differences;

	long function_309b50(long arg_0);
};

// the sum of a vector's first three terms, in the first
__forceinline __m128 sum_309b50(__m128 const &v)
{
	return _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v));
}

// three-term dot product of two vectors
__forceinline float dot3_309b50(__m128 const &a, __m128 const &b)
{
	float result;
	_mm_store_ss(&result, sum_309b50(_mm_mul_ps(a, b)));
	return result;
}

// a vector scaled to unit length
__forceinline __m128 normalized_309b50(__m128 const &v)
{
	__m128 const sum = sum_309b50(_mm_mul_ps(v, v));
	__m128 const inverse = _mm_rsqrt_ss(sum);
	__m128 const correction = _mm_sub_ss(_mm_set_ss(3.0f), _mm_mul_ss(_mm_mul_ss(sum, inverse), inverse));
	__m128 const scale = _mm_mul_ss(_mm_mul_ss(_mm_set_ss(0.5f), inverse), correction);
	return _mm_mul_ps(_mm_shuffle_ps(scale, scale, 0), v);
}

// the cross product of two vectors' first three terms
// one point less another
__forceinline __m128 edge_309b50(__m128 const &from, __m128 const &to)
{
	return _mm_sub_ps(from, to);
}

__forceinline __m128 cross_309b50(__m128 const &a, __m128 const &b)
{
	__m128 b_yzx = _mm_shuffle_ps(b, b, 0xc9);
	__m128 a_zxy = _mm_shuffle_ps(a, a, 0xd2);
	__m128 second = _mm_mul_ps(a_zxy, b_yzx);
	__m128 b_zxy = _mm_shuffle_ps(b, b, 0xd2);
	__m128 a_yzx = _mm_shuffle_ps(a, a, 0xc9);
	return _mm_sub_ps(_mm_mul_ps(a_yzx, b_zxy), second);
}

// the point of the simplex's first three first-shape points with these weights
__forceinline __m128 weighted_309b50(__m128 const *points, float const *weights)
{
	__m128 const w0 = _mm_set_ps1(weights[0]);
	__m128 const w1 = _mm_set_ps1(weights[1]);
	__m128 const w2 = _mm_set_ps1(weights[2]);
	__m128 const second = _mm_mul_ps(w1, points[1]);
	__m128 const third = _mm_mul_ps(w2, points[2]);
	return _mm_add_ps(_mm_add_ps(_mm_mul_ps(w0, points[0]), second), third);
}

// The penetration of the two shapes: 0 with the direction, point and
// distance in `arg_0` (or nothing found by the first step), 2 when the
// simplex can't grow or the hull fails too often, and from the hull, 1 when
// it ran out of room
// @retail 0x309b50
long class_309b50::function_309b50(long arg_0)
{
	first_step_3094d0 *result = (first_step_3094d0 *)arg_0;
	for (;;)
	{
		int grown = -1;
		__m128 normal;
		__m128 delta;
		switch (simplex->count)
		{
		case 0:
			goto first;
		case 1:
			goto point;
		case 2:
			goto segment;
		case 3:
			goto triangle;
		case 4:
			goto tetrahedron;
		default:
			goto expand;
		}
	tetrahedron:
		{
			__m128 const a = edge_309b50(differences[0], differences[1]);
			__m128 const b = edge_309b50(differences[1], differences[2]);
			__m128 const a_yzx = _mm_shuffle_ps(a, a, 0xc9);
			__m128 const b_zxy = _mm_shuffle_ps(b, b, 0xd2);
			__m128 const a_zxy = _mm_shuffle_ps(a, a, 0xd2);
			__m128 const b_yzx = _mm_shuffle_ps(b, b, 0xc9);
			__m128 const second = _mm_mul_ps(a_zxy, b_yzx);
			normal = _mm_sub_ps(_mm_mul_ps(a_yzx, b_zxy), second);
			float const height = dot3_309b50(_mm_sub_ps(differences[3], differences[0]), normal);
			if (!((float)fabs(height) <= 0.001f))
			{
				goto expand;
			}
			if (grown >= 3)
			{
				return 2;
			}
			simplex->count--;
		}
	triangle:
		{
			__m128 const a = _mm_sub_ps(differences[0], differences[1]);
			__m128 const b = _mm_sub_ps(differences[1], differences[2]);
			normal = cross_309b50(a, b);
			float length_squared;
			_mm_store_ss(&length_squared, sum_309b50(_mm_mul_ps(normal, normal)));
			if (length_squared != 0.0f)
			{
				goto grow_triangle;
			}
			if (grown >= 2)
			{
				goto collapsed_simplex;
			}
			simplex->count--;
		}
	segment:
		{
			__m128 const along = _mm_sub_ps(differences[0], differences[1]);
			float length_squared;
			_mm_store_ss(&length_squared, sum_309b50(_mm_mul_ps(along, along)));
			if (!(length_squared <= 0.001f * 0.001f))
			{
				goto grow_segment;
			}
			if (grown >= 2)
			{
				return 2;
			}
			simplex->count--;
		}
	point:
		{
			float length_squared;
			_mm_store_ss(&length_squared, sum_309b50(_mm_mul_ps(differences[0], differences[0])));
			if (!(length_squared <= 0.001f * 0.001f))
			{
				goto grow_point;
			}
			if (grown >= 0)
			{
				return 2;
			}
			simplex->count--;
		}
	first:
			if (((search_3093e0 *)this)->function_3094d0(result).value)
			{
				return 0;
			}
			grown = 0;
			goto point;
	grow_point:
		grown = 1;
		((search_3093e0 *)this)->function_309610();
		goto segment;
	grow_segment:
		grown = 2;
		((search_3093e0 *)this)->function_309700();
		goto triangle;
	grow_triangle:
	{
		scored_support_3093e0 scored;
		grown = 3;
		((search_3093e0 *)this)->function_3093e0(&normal, differences, &scored);
		if (scored.distance < 0.001f)
		{
			goto separated;
		}
		differences[simplex->count] = _mm_sub_ps(scored.support.first, scored.support.second_placed);
		simplex->points[simplex->count] = scored.support.first;
		simplex->points[simplex->count + 4] = scored.support.second;
		simplex->count++;
		goto tetrahedron;
	}

	collapsed_simplex:
		{
			// the simplex is a segment: a direction across it, and the point
			// between its ends
			result->distance = 0.0f;
			delta = _mm_sub_ps(differences[0], differences[1]);
			int across = 1;
			int smallest = 0;
			int other = 2;
			float least = (float)fabs(delta.m128_f32[0]);
			float const second = (float)fabs(delta.m128_f32[1]);
			float const third = (float)fabs(delta.m128_f32[2]);
			if (second < least)
			{
				least = second;
				across = 0;
				smallest = 1;
			}
			if (third < least)
			{
				other = smallest;
				smallest = 2;
			}
			float *direction = (float *)&result->direction;
			direction[smallest] = 0.0f;
			direction[across] = delta.m128_f32[other];
			direction[other] = -delta.m128_f32[across];
			result->direction = normalized_309b50(result->direction);
			__m128 const magnitude = _mm_and_ps(differences[0], *(__m128 const *)g_455940);
			float first_size;
			_mm_store_ss(&first_size, sum_309b50(magnitude));
			float second_size;
			_mm_store_ss(&second_size, sum_309b50(magnitude));
			float const share = second_size / (first_size + second_size + 1.192092896e-07f);
			__m128 const share4 = _mm_set_ps1(share);
			result->point = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(*(__m128 const *)g_412420, share4), simplex->points[0]), _mm_mul_ps(share4, simplex->points[1]));
			result->field_24 = 0.5f;
			return 0;
		}

	separated:
		{
			// the triangle's plane is the direction
			normal = normalized_309b50(normal);
			result->distance = 0.0f;
			result->direction = normal;
			float weights[3];
			function_308260(&g_4801e0, &differences[0], &differences[1], &differences[2], weights);
			result->point = weighted_309b50(simplex->points, weights);
			result->field_24 = 0.5f;
			return 0;
		}

	expand:
		{
			class_30a810 hull(&differences[0], &differences[1], &differences[2], &differences[3]);
			points_309b50 seconds;
			points_309b50 firsts;
			firsts.size = 4;
			firsts.data[0] = simplex->points[0];
			firsts.data[1] = simplex->points[1];
			firsts.data[2] = simplex->points[2];
			firsts.data[3] = simplex->points[3];
			seconds.size = 4;
			seconds.data[0] = simplex->points[4];
			seconds.data[1] = simplex->points[5];
			seconds.data[2] = simplex->points[6];
			seconds.data[3] = simplex->points[7];
			long state = 0;
			face_30aac0 *face;
			for (;;)
			{
				face = (face_30aac0 *)((list_309120 const *)&hull)->function_309120();
				support_309300 support;
				((pair_309300 const *)this)->function_309300(&face->normal, &support);
				delta = _mm_sub_ps(support.first, support.second_placed);
				c_query_transform offsets;
				offsets.axes[0] = _mm_sub_ps(delta, *face->edges[0].vertex);
				offsets.axes[1] = _mm_sub_ps(delta, *face->edges[1].vertex);
				offsets.axes[2] = _mm_sub_ps(delta, *face->edges[2].vertex);
				vector_2da6e0 heights;
				heights.inverse_rotate(&offsets, &face->normal);
				if (_mm_movemask_ps(_mm_cmplt_ps(heights.value, *(__m128 const *)g_461f20)) & 7)
				{
					break;
				}
				int count = 0;
				edge_30aac0 const *horizon;
				if (hull.function_30abe0(&count, &horizon, face, &delta) != 0)
				{
					goto restart;
				}
				if (!(hull.vertex_count < (hull.vertex_capacity_and_flags & 0x7fffffff)))
				{
					state = 1;
					break;
				}
				if ((hull.face_capacity_and_flags & 0x7fffffff) - hull.face_count + hull.face_pointer_count < count)
				{
					state = 1;
					break;
				}
				int const failed = hull.function_30ae70(count, (edge_30aac0 *)horizon, &delta);
				firsts.data[firsts.size++] = support.first;
				seconds.data[seconds.size++] = support.second;
				if (failed != 0)
				{
					goto restart;
				}
			}
			goto found;

		restart:
			if (++attempts == 1)
			{
				simplex->count = 1;
				continue;
			}
			if (attempts >= 20)
			{
				state = 2;
				goto found;
			}
			{
				__m128 nudge;
				simplex->count = 0;
				((random_309270 *)this)->function_309270(attempts, -0.0001f, 0.0001f, (float *)&nudge);
				nudge = _mm_add_ps(nudge, position);
				position = nudge;
			}
			continue;

		found:
			// the face the origin projects into, walked to from the nearest
			{
				float weights[3];
				for (int step = 0; ; )
				{
					differences[0] = *face->edges[0].vertex;
					differences[1] = *face->edges[1].vertex;
					differences[2] = *face->edges[2].vertex;
					function_308260(&g_4801e0, &differences[0], &differences[1], &differences[2], weights);
					if (!(weights[0] < 0.0f) && !(weights[1] < 0.0f) && !(weights[2] < 0.0f))
					{
						break;
					}
					face = hull.function_30aac0(face);
					if (++step >= 10)
					{
						break;
					}
				}
				long index = (vertex_309b50 const *)face->edges[0].vertex - hull.vertices;
				simplex->points[0] = firsts.data[index];
				simplex->points[4] = seconds.data[index];
				index = (vertex_309b50 const *)face->edges[1].vertex - hull.vertices;
				simplex->points[1] = firsts.data[index];
				simplex->points[5] = seconds.data[index];
				index = (vertex_309b50 const *)face->edges[2].vertex - hull.vertices;
				simplex->points[2] = firsts.data[index];
				simplex->points[6] = seconds.data[index];
				simplex->count = 3;
				result->point = weighted_309b50(simplex->points, weights);
				result->distance = -face->distance;
				result->direction = _mm_xor_ps(face->normal, *(__m128 const *)g_4124a0);
			}
			return state;
		}
	}
}
