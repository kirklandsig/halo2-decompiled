// @flags /O2 /GL-
// Havok (third-party physics library): collecting what a set of planes and
// what a sphere touch by walking a tree in fixed-point coordinates.
#include <xmmintrin.h>

struct s_visibility_query_list;

// a sphere: its centre, and its radius as the fourth term
struct s_visibility_sphere_query
{
	__m128 centre;
};

// the tree's origin, with the size of one unit as the fourth term, and the
// tree itself at 0x30
struct tree_2ddbd0
{
	__m128 origin;
	char unknown10[0x20];
	char root[1];
};

// the nodes still to visit
struct pending_2ddbd0
{
	long field_0;
	long field_4;
	long field_8;
	long field_c;
	long field_10;
	long field_14;

	pending_2ddbd0()
	{
		field_8 = 0;
		field_c = 0;
		field_10 = 0;
		field_14 = 0x10;
		field_0 = 0;
		field_4 = 0;
	}
};

struct point_2ddbd0
{
	float x;
	float y;
	float z;
};

class class_2dce60
{
public:
	s_visibility_query_list *second;
	char unknown4[0xc];
	float scale;
	char unknown14[0xc];
	s_visibility_query_list *first;
	char unknown24[0xc];
	point_2ddbd0 centre;
	float diameter;
	__m128 offset;
	__m128 fixed_scale;

	void function_2dce60(pending_2ddbd0 *pending, void const *root, __m128 const *offset);
};

// a plane: its normal and its distance
struct plane_2dd930
{
	float x;
	float y;
	float z;
	float d;
};

// the planes to test against
struct s_visibility_plane_query
{
	long count;
	plane_2dd930 const *planes;
};

// a vector's four lanes
static inline void set_lanes_2dd930(__m128 &vector, float a, float b, float c, float e)
{
	vector.m128_f32[0] = a;
	vector.m128_f32[1] = b;
	vector.m128_f32[2] = c;
	vector.m128_f32[3] = e;
}

// four planes, one term to a vector
struct planes4_2dd930
{
	__m128 x;
	__m128 y;
	__m128 z;
	__m128 d;

	// the four planes' terms, one plane to a lane
	void set(plane_2dd930 const *a, plane_2dd930 const *b, plane_2dd930 const *c, plane_2dd930 const *e)
	{
		set_lanes_2dd930(x, a->x, b->x, c->x, e->x);
		set_lanes_2dd930(y, a->y, b->y, c->y, e->y);
		set_lanes_2dd930(z, a->z, b->z, c->z, e->z);
		set_lanes_2dd930(d, a->d, b->d, c->d, e->d);
	}
};

static __declspec(align(16)) const unsigned long abs_mask_2dd930[4] = {0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff};

// the tree's fixed-point coordinates: an offset, then a scale
struct fixed_point_2dc220
{
	__m128 offset;
	__m128 scale;
};

// the walk that tests the tree's nodes against up to 32 planes, four at a
// time (0x2dc220)
class class_2dc220
{
public:
	s_visibility_query_list *second;
	char unknown4[0xc];
	float scale;
	char unknown14[0xc];
	s_visibility_query_list *first;
	char unknown24[0xc];
	long group_count;
	char unknown34[0xc];
	planes4_2dd930 groups[8];
	// the normals' terms' magnitudes
	planes4_2dd930 magnitudes[8];

	// the groups before the first free one moved to the tree's origin with their
	// distances doubled, and their normals' magnitudes kept alongside
	__forceinline void finish_groups(planes4_2dd930 *&group, tree_2ddbd0 const *tree)
	{
		planes4_2dd930 *last_magnitude = &magnitudes[group_count - 1];
		planes4_2dd930 *&magnitude = last_magnitude;
		--group;
		for (; group >= groups; --group, --magnitude)
		{
			magnitude->x = _mm_and_ps(group->x, *(__m128 const *)abs_mask_2dd930);
			magnitude->y = _mm_and_ps(group->y, *(__m128 const *)abs_mask_2dd930);
			magnitude->z = _mm_and_ps(group->z, *(__m128 const *)abs_mask_2dd930);
			magnitude->d = _mm_setzero_ps();
			__m128 const origin = tree->origin;
			_mm_store_ps(group->d.m128_f32, _mm_add_ps(group->d, _mm_add_ps(_mm_add_ps(
				_mm_mul_ps(group->x, _mm_shuffle_ps(origin, origin, 0x00)),
				_mm_mul_ps(group->y, _mm_shuffle_ps(origin, origin, 0x55))),
				_mm_mul_ps(group->z, _mm_shuffle_ps(origin, origin, 0xaa)))));
			__m128 const &distance = group->d;
			group->d = _mm_add_ps(distance, group->d);
		}
	}
	void function_2dc220(pending_2ddbd0 *pending, void const *root, fixed_point_2dc220 const *fixed_point);
};

// collects what the planes touch: the planes moved into the tree's
// coordinates four at a time (the last group padded with the first of its
// planes), with their normals' magnitudes alongside
// Matched (was 166): the groups are finished backwards from the first free one,
// which the helper takes by reference and steps back in place, beside a pointer
// to the last magnitudes that is stepped through a reference (so the mask and the
// tree's origin are read again on every pass and the doubled distance twice);
// the pending state's constructor stores its last field before its first two
// fields; the root is taken first, the count after the fixed-point offset and
// the planes last, just before the loop, which keeps the query in esi.
// @retail 0x2dd930
void __cdecl function_2dd930(void *data, s_visibility_plane_query const *query,
	s_visibility_query_list *second, s_visibility_query_list *first)
{
	tree_2ddbd0 const *tree = (tree_2ddbd0 const *)data;
	class_2dc220 walk;
	walk.scale = 1.0f / tree->origin.m128_f32[3];
	void const *root = tree->root;
	walk.second = second;
	pending_2ddbd0 pending;
	fixed_point_2dc220 fixed_point;
	fixed_point.offset = _mm_setzero_ps();
	long const count = query->count;
	planes4_2dd930 *group = walk.groups;
	walk.group_count = 0;
	fixed_point.scale = _mm_set_ps1(walk.scale * 16777216.0f);
	long quads_done = 0;
	plane_2dd930 const *plane = query->planes;
	for (; quads_done < (count & ~3); quads_done += 4)
	{
		group->set(plane, plane + 1, plane + 2, plane + 3);
		group++;
		plane += 4;
		walk.group_count++;
	}
	plane_2dd930 const *second_plane = plane + 1;
	plane_2dd930 const *third_plane = plane + 2;
	switch (count - quads_done)
	{
	case 1:
		second_plane = plane;
	case 2:
		third_plane = plane;
	case 3:
		group->set(plane, second_plane, third_plane, plane);
		group++;
		walk.group_count++;
	}
	walk.finish_groups(group, tree);
	walk.first = first;
	walk.function_2dc220(&pending, root, &fixed_point);
}

// Matched (was 30): the pending state's constructor is a body in store order, the tree's
// root is bound by a constant reference before the scale store, the origin is read
// straight from the tree, and the diameter is set after the centre copies.
// @retail 0x2ddbd0
void __cdecl function_2ddbd0(void *data, s_visibility_sphere_query const *query,
	s_visibility_query_list *second, s_visibility_query_list *first)
{
	tree_2ddbd0 const *tree = (tree_2ddbd0 const *)data;
	class_2dce60 walk;
	walk.scale = 1.0f / tree->origin.m128_f32[3];
	walk.offset = _mm_setzero_ps();
	walk.second = second;
	char const *const &root = tree->root;
	walk.fixed_scale = _mm_set_ps1(walk.scale * 16777216.0f);
	pending_2ddbd0 pending;
	__m128 const relative = _mm_sub_ps(query->centre, tree->origin);
	__m128 doubled = _mm_add_ps(relative, relative);
	walk.centre.x = doubled.m128_f32[0];
	walk.centre.y = doubled.m128_f32[1];
	walk.centre.z = doubled.m128_f32[2];
	walk.diameter = query->centre.m128_f32[3] + query->centre.m128_f32[3];
	walk.first = first;
	walk.function_2dce60(&pending, root, &walk.offset);
}
