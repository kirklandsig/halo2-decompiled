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
		: field_0(0), field_4(0), field_8(0), field_c(0), field_10(0), field_14(0x10)
	{
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

	// the groups before `end` moved to the tree's origin with their distances
	// doubled, and their normals' magnitudes kept alongside
	__forceinline void finish_groups(tree_2ddbd0 const *tree)
	{
		for (long remaining = group_count; remaining > 0; --remaining)
		{
			planes4_2dd930 *group = &groups[remaining-1];
			planes4_2dd930 *magnitude = &magnitudes[remaining-1];
			magnitude->x = _mm_and_ps(group->x, *(__m128 const *)abs_mask_2dd930);
			magnitude->y = _mm_and_ps(group->y, *(__m128 const *)abs_mask_2dd930);
			magnitude->z = _mm_and_ps(group->z, *(__m128 const *)abs_mask_2dd930);
			magnitude->d = _mm_setzero_ps();
			__m128 const origin = tree->origin;
			_mm_store_ps(group->d.m128_f32, _mm_add_ps(group->d, _mm_add_ps(_mm_add_ps(
				_mm_mul_ps(group->x, _mm_shuffle_ps(origin, origin, 0x00)),
				_mm_mul_ps(group->y, _mm_shuffle_ps(origin, origin, 0x55))),
				_mm_mul_ps(group->z, _mm_shuffle_ps(origin, origin, 0xaa)))));
			group->d = _mm_add_ps(group->d, group->d);
		}
	}
	void function_2dc220(pending_2ddbd0 *pending, void const *root, fixed_point_2dc220 const *fixed_point);
};

// collects what the planes touch: the planes moved into the tree's
// coordinates four at a time (the last group padded with the first of its
// planes), with their normals' magnitudes alongside
// The remaining differences are the entry register choices and the SIMD
// load/store schedule while translating packed groups.
// collects what the planes touch: the planes moved into the tree's
// coordinates four at a time (the last group padded with the first of its
// planes), with their normals' magnitudes alongside
// score 171 (was 178): entry registers, load order and final-loop layout
// still differ. Before the SIMD store, indexed arrays reached 672 bytes. Previously,
// retail computes the groups' magnitudes first and then
// their distances, reads the distance back from the group before doubling it
// and loads the query through esi; ours takes the origin's terms first, keeps the
// distance in a register across the doubling and takes the query in ecx. The
// loop's registers and the order of its two halves are the whole difference.
// Further trials: local bit/vector mask 184; intrinsic mask 183; doubling
// loads 174; distance/magnitude stores and direct origin uses 174; pointer
// before count and writable mask 174; value/output magnitude helpers 184;
// mixed distance reads/origin intrinsic 178; consumed group pointer 173;
// vector planes 166; shared sum 168
// with a larger frame; finishing arguments 166; minimal iterator 167;
// early query snapshot 167. Original mask and arithmetic retained.
// Two explicit loads of the distance for the doubling 166 (not kept: they only force the reloads).
// Further trials: indexed arrays 178; ordinary inline 348; forced setters
// 184; aligned inputs 184; plane references 184; descending lanes 379;
// guarded remainder 278; named products 184; packed lane construction 379;
// indexed arrays without the unused boundary argument 178.
// Further trials: reverse end pointers 180; SIMD output stores 171; query
// object reference 178; translation-only SIMD store 171; packed inputs with
// that store 171; input countdown 171; finishing optimization disabled 171;
// local writable mask 171; reverse pointers with the store 174. The single
// translated-distance SIMD store is retained, at 674/672 bytes.
// score 171 (was 171): input registers and the SIMD load/store schedule differ.
// Positive finishing counter with the index derived from it 166 (not kept: a hand-inverted loop); early query reads, cursor-first reads,
// array-entry references, a shared magnitude helper, first-output ordering
// and an input-object reference 171; early second output 191;
// direct index countdown 168; counter with entry references
// or an earlier origin read 166.
// @retail 0x2dd930
void __cdecl function_2dd930(void *data, s_visibility_plane_query const *query,
	s_visibility_query_list *second, s_visibility_query_list *first)
{
	tree_2ddbd0 const *tree = (tree_2ddbd0 const *)data;
	class_2dc220 walk;
	walk.scale = 1.0f / tree->origin.m128_f32[3];
	walk.second = second;
	long const count = query->count;
	plane_2dd930 const *plane = query->planes;
	void const *root = tree->root;
	pending_2ddbd0 pending;
	fixed_point_2dc220 fixed_point;
	fixed_point.offset = _mm_setzero_ps();
	planes4_2dd930 *group = walk.groups;
	walk.group_count = 0;
	fixed_point.scale = _mm_set_ps1(walk.scale * 16777216.0f);
	long quads_done = 0;
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
	walk.finish_groups(tree);
	walk.first = first;
	walk.function_2dc220(&pending, root, &fixed_point);
}

// The remaining differences are the tree/query register lifetimes and the
// order of the origin load, pending-state stores, and centre copy.
// score 33 (was 33): retail reads the tree's origin before the second list,
// takes the root with a lea before the query is read and keeps the query in
// eax; ours reads the second list first, adds 0x30 to the tree pointer for
// the root and keeps the query in ecx. Reading the origin, the root, the
// query and the first list into locals of their own, early, leaves the code
// exactly as it was.
// Further trials: early difference/doubling 42/42; early fourth-list store
// 42; sphere reference 33; renamed state/position/deltas 33; memcpy centre
// 33; vector/point view 33; a shared coordinate setter failed to build, then
// timed out. Original body retained.
// Further trials: shared scalar/vector origin 60; origin field reference 33;
// scalar SIMD coordinate stores 53; descending coordinate stores 33; early
// diameter 46; scalar sphere fields 33; scalar header view 33; global
// optimization disabled 158. Opening body retained.
// Further trials: split reciprocal lifetime 49; late reciprocal field 49;
// sphere value snapshot 61; size preference 40; explicit scalar splat 48;
// in-place delta 33; named radius times two 33; aligned intrinsic loads 48.
// Further trials: zero-initialized iterator local 71; pending value construction
// 33; point-value output 82; paired coordinate store 61; centre array
// or indexed coordinate copy 33; initialized array copy 71; initialized
// point value 108. Origin/list order and root/query registers still differ.
// Further trials: chained pending clears 32; pending initializer list 30;
// second list after pending state 32; zero offset after pending state 33;
// splat after diameter 41; named scale product 33; left scale factor 33;
// vector doubling by multiplication 63.
// Further trials: native centre/diameter field 33; packed output 75; pending
// before state with original/native field 36/36; coordinate scope 33/33;
// existing zero setter with original/native field 71/71.
// @retail 0x2ddbd0
void __cdecl function_2ddbd0(void *data, s_visibility_sphere_query const *query,
	s_visibility_query_list *second, s_visibility_query_list *first)
{
	tree_2ddbd0 const *tree = (tree_2ddbd0 const *)data;
	class_2dce60 walk;
	walk.scale = 1.0f / tree->origin.m128_f32[3];
	walk.offset = _mm_setzero_ps();
	walk.second = second;
	pending_2ddbd0 pending;
	walk.fixed_scale = _mm_set_ps1(walk.scale * 16777216.0f);
	walk.diameter = query->centre.m128_f32[3] + query->centre.m128_f32[3];
	__m128 const relative = _mm_sub_ps(query->centre, tree->origin);
	__m128 doubled = _mm_add_ps(relative, relative);
	walk.centre.x = doubled.m128_f32[0];
	walk.centre.y = doubled.m128_f32[1];
	walk.centre.z = doubled.m128_f32[2];
	walk.first = first;
	walk.function_2dce60(&pending, tree->root, &walk.offset);
}
