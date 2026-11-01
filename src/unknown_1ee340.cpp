#include "unknown_11c920.h"
#include "globals.h"
#include "normalize3d.h"
#include "vector_products.h"
#include <xmmintrin.h>
#include <float.h>
#include <new>
#include "unknown_1eb350.h"
#include "havok_reference.h"
#include "shape_2de750.h"

struct c_transformed_point
{
	__m128 value;
	void transform(const void *matrix, const __m128 *point);
};

void __cdecl function_2fe730(const void *points, long count, long stride, void *output);

// @flags /O2 /arch:SSE /Gr

/* a shape holding an array of 16-byte aligned vertices starting at +0x20 */
struct s_shape_surface
{
	word field_0;
	word edge;
	byte flags;
	byte field_5;
	short material;
};

struct s_shape_edge
{
	word vertices[2];
	word next[2];
	short surfaces[2];
};

struct s_shape_vertex
{
	point3f point;
	long field_c;
};

struct s_shape_source
{
	byte field_0[0x2c];
	s_shape_surface *surfaces;
	long edge_count;
	s_shape_edge *edges;
	long vertex_count;
	s_shape_vertex *vertices;
};

struct s_shape_ray
{
	point3f start;
	long field_c;
	point3f end;
};

struct s_shape_ray_result
{
	vector3f normal;
	real field_c;
	long field_10;
	real fraction;
};

struct c_vertex_shape : radius_shape_2de750
{
	long surface_index;
	long vertex_count;
	short material;
	byte flags;
	byte kind;
	long value;
	real vertices_raw[8 * 4];
	c_vertex_shape(s_shape_source *source, byte kind, long value, long surface_index,
		long user, const transform4x3f *matrix);

	__m128 *vertices() { return (__m128 *)this->vertices_raw; }

	virtual void get_supporting_vertex(const __m128 *direction, __m128 *out);
	virtual void gather_vertices(const word *indices, long count, __m128 *out);
	virtual void get_first_vertex(__m128 *out);
	virtual void v3();
	virtual void get_bounds(const void *matrix, real expansion, void *output);
	virtual void ray_test(bool *hit, const s_shape_ray *ray, s_shape_ray_result *result);
};

PRIVATE inline void shape_transform_point(const transform4x3f *matrix, const point3f *point, point3f *result)
{
	real x = point->x, y = point->y, z = point->z;
	if (matrix->scale != 1.0f)
	{
		x = matrix->scale * x;
		y = matrix->scale * y;
		z = matrix->scale * z;
	}
	result->x = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x + matrix->position.x;
	result->y = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x + matrix->position.y;
	result->z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z;
}

// This round (score 6, was 6): the entry block still loads the index before
// the radius constant, and the loop still materialises the vertex address
// before the x store; a fresh address expression for the point helper, the w
// and mask stores written from separate expressions, the pointer written as
// vertices_raw + count * 4 and a point3f pointer all compile identically.
// Also this round: the member stores reordered (surface_index before kind, and
// surface_index first) and the vertex pointer taken as a point3f pointer all
// leave the two differences in place, and the w store written through the array
// again is worse (38).
// score 6 (was 6): radius/index loads swap; vertex x uses the completed
// address before its store. Derived count/kind/value initializers 33; all
// member initializers 6; index/count/kind initializers 6; const source 6;
// point memcpy 120; point value with in-place transform 128; word edge
// cursors 97; full-width side 97.
// Source-surface reference 6; source-edge reference 6; cached edge table 120;
// cached vertex table 125; output ordinal snapshot 101; outer transform workspace 6;
// short side 97; external geometry reference 6. Radius/index and x-store order remain.
// Also tried: tag stored through (long *)vertices_raw or vertices() 6; vertex
// as vertices_raw + n, an x local, surfaces[surface_index], surface_index set
// first 6; w stored by member index 38; offset local 68; separate x/y/z
// stores 10/106; point struct copy 120; __m128 vertex pointer 6.
// @retail 0x1edd00
c_vertex_shape::c_vertex_shape(s_shape_source *source, byte type, long data, long index,
	long user_data, const transform4x3f *matrix) : radius_shape_2de750(0.01f)
{
	vertex_count = 0;
	kind = type;
	surface_index = index;
	value = data;
	s_shape_surface *surface = &source->surfaces[index];
	long edge_index = surface->edge;
	long first_edge = edge_index;
	do
	{
		s_shape_edge *edge = &source->edges[edge_index];
		bool side = edge->surfaces[1] == index;
		const point3f *point = &source->vertices[edge->vertices[side]].point;
		point3f transformed;
		if (matrix)
		{
			shape_transform_point(matrix, point, &transformed);
			point = &transformed;
		}
		real *vertex = &vertices_raw[vertex_count * 4];
		real_point3d_set((point3f *)vertex, point->x, point->y, point->z);
		vertex[3] = 0.0f;
		*(long *)&vertices_raw[vertex_count * 4 + 3] = vertex_count | 0x3f000000;
		vertex_count++;
		edge_index = edge->next[side];
	} while (edge_index != first_edge);
	material = surface->material;
	flags = surface->flags;
	user_value = user_data;
}

short function_120850(const vector3f *vector);
bool function_23a220(const point2f *point, short count, const point2f *points, real epsilon);

// MiniMax S (10-31): no new form, no lower score. Retail keeps the shape's edges and the ray direction
// in xmm1-xmm7 and computes the three edge vectors in registers, storing only four floats; ours spills
// every component of every difference to the stack and re-reads them, so the frame is 0x84 against 0x90.
// score 192 (was 192): frame 0x84 versus 0x90: retail keeps origin.x/y in xmm6/xmm7 until the
// offset (ours re-reads all three from the shape), so it spills first.i/second.i into homes and puts
// the found byte at +0x38 and the plane at +4; every later stack offset differs by that. Retail also
// computes the direction from its start loads (ours from memory operands) and has a dead second
// `movsx` of the axis. Round 81 (fixed, 247 -> 192): the projection as the matched sibling 0x14a370
// writes it (bool positive, the table read at each use, vertex2d/vertex element pointers in the loop:
// retail's countdown unroll with a destination pointer, `result` no longer held in ebp), `found = true`
// first in the hit block (retail's two exits), the existing cross3f, the direction from ray->start
// before the start copy, the second edge before the first. Tried then: 0x120810 for the projections
// 354; aggregate polygon copy 435; origin pointer 290; struct ray copy 279; origin struct copy or
// fields, found at top/after the normalize, offset before the plane/edges or after the dot, `&&`
// range test, call count without the cast, hit-point product orders, normal before fraction, offset
// from ray->start, dot3f operand order: 192-335, none lower.
// Round 16: retail's frame is 0x90 against our 0x84: it preloads the ray start's components,
// keeps the shape in ESI and lets the compiler unroll the polygon copy four ways, ours copies
// the origin vertex with integer moves and writes the unroll out.
// Before round 81 (247): frame 0x7c versus 0x90; retail reloads `result` from its stack slot and
// shrink-wraps ebx/ebp/edi into the hit block, where ours keeps `result` in ebp (pushed at entry)
// and duplicates the early failure exits. Retail's unrolled copy loop counts down with a
// destination pointer; ours keeps the index. Retail's frame: length/fraction, plane, direction
// (later the projection), ray start, second edge (later the hit point), found byte, two spills,
// first edge, polygon; its edge vectors keep homes for their i components.
// Round 59: the existing normalize3d in place of its written-out body 247 (same code, kept);
// found declared first 247; existing cross3f 265; bool side term without the ternary 247;
// named bool side term 249 (gives retail's `mov al, 1 / xor al, al; movzx`).
// Round 62: short loop index 331 (not unrolled); `result` read through its own address 247 (same
// code: its ebp home is not what pushes ebp at entry); direction from ray->start instead of the copy
// 245; fraction as one ternary 249; ray->start direction plus the named bool side term 249. Retail also
// keeps origin.x/y in xmm6/xmm7 and spills first.i/second.i, where ours re-reads origin from the shape.
// Round 57: existing plane3f for the normal and its zero d, with the projection loop written
// plainly (VC unrolls it itself, as retail) 247 kept; plane alone 272; plain loop alone 258;
// vertex pointer in the loop 293; destination pointer 336; vertices() rows 247 (same code);
// count argument without the short cast 247 (same code); reversed compare 247; vector copy
// of the normal into the result 268.
// Hit point setter 250; component normalization loop 250; short projection side 255;
// direct hit result 251; ray before origin with float setters 251.
// Early offset 259; late ray direction 264; squared-normal value 250; descending projection 358; offset/direction combination 264.
// Frame 0x78 versus 0x90 and normal/ray/polygon homes remain. Native ray 266
// (frame 0x88); separate unscaled normal 307 (0x84); both 311 (0x94); projected initializer 250;
// both with plain projection loop 278 (0x90).
// Frame, ray/normal homes and polygon input registers still differ. Tried native plane 257;
// early distance 260; entry plane 257; early hit result 257; plain plane projection 259; plane normal copy 262;
// native ray start 251; early edge vectors 257.
// Native full/geometry/polygon vertex records 250/250/250; native-record plain projection loop
// 260; late hit lifetime 251; late hit with full/geometry/polygon records 251/251/251.
// Round 210: Word hit result 259; native byte result 262; normal memcpy 263; scoped edge-vector
// lifetimes 250; direct origin fields 282. Frame 0x78 versus 0x90 and ray/normal/polygon homes
// remain.
// Round 214: signed-byte hit state 262; unsigned-word hit state 259; byte projection side 253; unsigned-word projection side 255; byte dominant axis 251.
// Round 178: Corresponding positive/negative/all row-pair/all grouped cross terms 255/255/253/255;
// with native aggregate normal 255/255/253/255. Frame 0x7c versus 0x90 persists;
// original terms restored. Normal/ray/hit and polygon homes remain.
// Full shared normalization with cross/point/plain-loop combinations 253/250/253/250/258/261/258/261. Three-
// point 250 forms restored; frame is now 0x7c versus 0x90.
// Round 64: retail's extra 0x14 bytes are the edge vectors' homes (first.i and second.i are spilled
// into them, the hit point later shares second's) and two loop spills: retail keeps origin.x/y in
// xmm6/xmm7 until the offset, ours reloads all of origin from the shape, so no edge spills. Origin
// as a vertex pointer 278 (aligned 194 vs 203); destination and vertex pointers in the loop 328 (not
// unrolled); per-vertex row pointer 293; origin copied after the direction 248; that with the
// direction from ray->start 245.
// Lane 9 r23: retail's homes (frame offsets) are fraction 0, plane 4, direction 0x14, ray start
// 0x20, hit point 0x2c, hit byte 0x38, spills 0x3c..0x4f, polygon 0x50; ours are size-ordered (hit
// byte 3, fraction 4, direction 8, ray start 0x14, point 0x20, plane 0x2c, polygon 0x3c). Hit point
// or direction renamed to change name order 247/247 (same code); fraction as a ternary 249;
// plane d cleared before normalizing 252; clamp as if/else 250.
// Round 148: start copy first with the direction from the copy 230 (retail's single start loads,
// but per-component end loads); start copy first with the direction from ray->start 192; start copy
// as a point assignment 204; direction as an initializer 223; offset from the first vertex in place
// of origin 259; origin as a reference to the first vertex 275; ray_start renamed 192 (same code:
// names do not move these slots).
// Round 262: entry polygon/projected/hit-point storage, all seven combinations, 192 each;
// all three with entry fraction storage 192. All computations stayed at their original uses;
// original declarations restored. Frame 0x84 and origin/edge spills remain.
// Round 218: Direct ray-start observations, polygon inline/noinline, both direct-ray combinations,
// direct ray with point-set/cross/normalize forceinline declarations:
// 282/192/192/282/282/282/282/282 (fast control 192). Original restored.
// Round 222: dominant-axis long/word/dword/signed byte 192/193/192/193;
// the same four carriers with immutable native origin construction 192/193/192/193.
// All restored; origin retention and edge homes still leave frame 0x84 versus 0x90.
// Round 161: Original-ray difference-helper forceinline alone/with point-set/with cross/with both
// 192/192/192/192; complete origin/start/both setter components 192/192/192;
// both component captures with difference forceinline 192. Original captures and boundaries restored.
// Round 207: existing array-based unit-scaled differences for ray direction, both edges,
// ray plus edges, offset, edges plus offset, ray plus offset, all differences and second
// edge alone: 192 each at 1007/1001. All exact-match-only forms restored;
// frame 0x84 versus 0x90 and origin/edge homes remain.
// Coupled origin-start/edge-pair/plane-result lexical identities, all seven subsets and complete
// projection/ray identities: 192/192/192/192/192/192/192/192. Origin retention and edge spills still leave
// the short frame
// original identities restored.
// Round 222: Intrinsic origin/start/both copies 282/204/270; complete vector constructors 192; origin/start/both with constructors 282/204/270; all with normal-output constructor 269. All restored; frame 0x84 versus 0x90 and retained-origin/edge homes remain.
// Round 163: endpoint float setters, second/first/both/both before edges,
// 260/251/216/214; with shared ray-start input 260/247/217/215.
// Paired endpoints recover frame 0x90 but retain entry/edge scheduling differences;
// every one-use endpoint capture and original ray input restored.
// Round 72: origin as a pointer to the first vertex 275 (the vertex loads fold into memory operands,
// none shared); start copy first with the direction from the copy 230 (retail's shared start
// loads); both 276; that with the first edge before the second 232; count argument without the
// short cast 192 (same code: a separate word load where retail pushes the loop-bound EDX).
// Round 87: direction from the value setter 192 (same code); `long count = vertex_count;` for the loop
// and the call 193 (retail's `push edx` and esi tail-loop temp, but a third exit block), declared
// before the polygon 193; fraction as if/else 259; edges and cross in an inline triangle helper 192.
// Round 72 (lane 8): retail passes the vertex count to 0x23a220 from the register its copy loop
// loaded (`push edx`); ours reloads the word. `long count = vertex_count;` before the polygon 193
// (push edx, a third exit), declared before the axis or at the bounded test 199/199 (1001 B, this
// then in EDI); count local with copied start/end points 197 (1020 B); copied start/end points
// alone 196.
// Round 191: complete admission result through fraction/clamp/polygon tests, nested/merged
// range with original/shared count: bool 266/262/258/256; byte 258/258/262/258.
// All regress; original result flow, count observations and native geometry restored.
// DL192: First/second edge reused for offset 192/192; start/hit point reused alone or with either edge
// 232/232/232; combined workspaces with native normal publication 263/263; first-edge/point reuse with point
// setter 261. Frame/ray/normal homes remain; all restored.
// Round 298: existing staged cross with original/origin-live/start-live/both input
// observations 192/275/282/292; the same four with entry hit-result lifetime
// 192/275/282/292. All restored; frame 0x84 and retained-origin/edge spills remain.
// DL202: Staged cross in the first/second edge home, normalized before/after plane publication,
// with separate/reused offset storage: 299/299/297/297/299/299/297/297.
// Normalizing before plane publication recovers frame 0x90 but grows to 1039/1001 bytes;
// origin retention, ray loads and plane homes still differ. All original source restored.
// Round 454: Existing scaled-add hit-point producer 196, with native normal publication 275;
// entry hit-point storage 196/275. Existing scale/add producers 202/272, with entry point
// storage 202/272. All frames remain 0x84; original point products, normal stores and body restored.
// Round 462: One complete ray value with direct native start versus duplicate start, native/library
// copy before/after origin: 248 in all eight forms, 1001/1001 bytes but frame 0x98 versus 0x90.
// Original input observations, separate start and complete body restored.
// @retail 0x1edf50
void c_vertex_shape::ray_test(bool *hit, const s_shape_ray *ray, s_shape_ray_result *result)
{
	point3f origin;
	real_point3d_set(&origin, vertices_raw[0], vertices_raw[1], vertices_raw[2]);
	vector3f direction;
	vector3d_from_points3d(&ray->start, &ray->end, &direction);
	point3f ray_start;
	real_point3d_set(&ray_start, ray->start.x, ray->start.y, ray->start.z);
	vector3f first, second;
	vector3d_from_points3d(&origin, (point3f *)&vertices_raw[8], &second);
	vector3d_from_points3d(&origin, (point3f *)&vertices_raw[4], &first);
	plane3f plane;
	cross3f(&first, &second, &plane.n);
	bool found = false;
	normalize3d(&plane.n);
	plane.d = 0.0f;
	vector3f offset;
	vector3d_from_points3d(&origin, &ray_start, &offset);
	real dot = dot3f(&plane.n, &direction);
	real fraction = 0.0f;
	if (dot != 0.0f)
		fraction = dot3f(&plane.n, &offset) * (-1.0f / dot);
	if (fraction < result->fraction)
	{
		real bounded = fraction < 0.0f ? 0.0f : fraction > 1.0f ? 1.0f : fraction;
		if (bounded == fraction)
		{
			short axis = function_120850(&plane.n);
			bool positive = plane.n.n[axis] > 0.0f;
			point3f point;
			point.x = fraction * direction.i + ray_start.x;
			point.y = fraction * direction.j + ray_start.y;
			point.z = fraction * direction.k + ray_start.z;
			point2f projected;
			projected.x = point.n[g_440b94[axis * 2 + positive][0]];
			projected.y = point.n[g_440b94[axis * 2 + positive][1]];
			point2f polygon[8];
			for (long i = 0; i < vertex_count; ++i)
			{
				point2f *vertex2d = &polygon[i];
				point3f const *vertex = (point3f const *)&vertices_raw[i * 4];
				vertex2d->x = vertex->n[g_440b94[axis * 2 + positive][0]];
				vertex2d->y = vertex->n[g_440b94[axis * 2 + positive][1]];
			}
			if (function_23a220(&projected, (short)vertex_count, polygon, 0.0f))
			{
				found = true;
				result->fraction = fraction;
				result->normal.i = plane.n.i;
				result->normal.j = plane.n.j;
				result->normal.k = plane.n.k;
				result->field_c = plane.d;
			}
		}
	}
	*hit = found;
}

// @retail 0x1eded0
void c_vertex_shape::get_bounds(const void *matrix, real expansion, void *output)
{
	c_transformed_point points[8];
	for (long i = 0; i < vertex_count; i++)
		points[i].transform(matrix, &vertices()[i]);
	function_2fe730(points, vertex_count, sizeof(c_transformed_point), output);
}

// @retail 0x1edf40
void c_vertex_shape::v3()
{
	__asm int 3
	__assume(0);
}

// @retail 0x1ee340
void c_vertex_shape::get_first_vertex(__m128 *out)
{
	*out = vertices()[0];
}

// @retail 0x1ee350
void c_vertex_shape::get_supporting_vertex(const __m128 *direction, __m128 *out)
{
	long best = 0;
	real best_dot = -FLT_MAX;
	for (long i = 0; i < vertex_count; i++)
	{
		__m128 p = _mm_mul_ps(vertices()[i], *direction);
		__m128 sum = _mm_add_ss(_mm_shuffle_ps(p, p, 0x55), p);
		sum = _mm_add_ss(_mm_shuffle_ps(p, p, 0xaa), sum);
		real dot;
		_mm_store_ss(&dot, sum);
		if (dot > best_dot)
		{
			best_dot = dot;
			best = i;
		}
	}
	*out = vertices()[best];
}

// @retail 0x1ee3d0
void c_vertex_shape::gather_vertices(const word *indices, long count, __m128 *out)
{
	for (long i = count - 1; i >= 0; i--)
	{
		out[i] = vertices()[indices[i]];
	}
}

struct s_tag_block_owner
{
	byte unknown00[0x5c];
	s_tag_block_entry *entries;
};

struct s_tag_instance_ref
{
	byte unknown00[8];
	s_tag_block_owner *data;
	byte unknown0c[4];
};

struct s_lookup_source
{
	byte unknown00[0x18];
	short index;
	short pad1a;
	long tag_index;
};

short g_54e898;

// @retail 0x1ee410
short *function_1ee410(const s_lookup_source *source, short *result)
{
    struct { short value; } local;
    long tag_index;
    local.value = (tag_index = source->tag_index, g_54e898);
	if (tag_index != NONE)
	{
		s_tag_instance_ref *tags = (s_tag_instance_ref *)g_4e3b44;
		local.value = tags[tag_index & 0xffff].data->entries[source->index].value10;
	}
	else if (source->index != NONE)
	{
		local.value = g_4e0348->entries[source->index].value08;
	}
    *result = local.value;
    return result;
}

struct s_count_entry
{
	dword unknown0;
	dword flags;
};

struct s_count_owner
{
	byte unknown00[0x28];
	long count;
	s_count_entry *entries;
};


struct c_count_interface
{
	byte unknown04[8];
	s_count_owner *owner;
	byte field_10[0x30 - 0x10];
	long shape_value;

	virtual void v0() {}
	virtual void v1() {}
	virtual void v2() {}
	virtual void v3() {}
	virtual void v4() {}
	virtual void v5() {}
	virtual void get_bounds(const __m128 *matrix, real expansion, __m128 *output);
	virtual void v7() {}
	virtual void v8() {}
	virtual void v9() {}
	virtual long get_count();
	virtual long test_count();
	virtual long next_index(dword index);
	virtual c_vertex_shape *make_shape(long index, void *storage);
	__declspec(noinline) c_vertex_shape *make_transformed_shape(long index, long user_data, void *storage,
		const transform4x3f *matrix);
};

void __cdecl function_1ce210(const __m128 *matrix, const __m128 *extent,
	const __m128 *center, real expansion, __m128 *output);

// @retail 0x1ef040
void c_count_interface::get_bounds(const __m128 *matrix, real expansion, __m128 *output)
{
	function_1ce210(matrix, (const __m128 *)(field_10 + 0x10),
		(const __m128 *)field_10, expansion, output);
}

// @retail 0x1ee470
long c_count_interface::get_count()
{
	long result = 0;
	s_count_owner *o = owner;
	result = o->count;
	if (o->count > 0)
	{
		const s_count_entry *last;
		last = &o->entries[o->count - 1];
		word flags = last->flags;
		if ((flags & 0x10) != 0)
		{
			result--;
		}
	}
	return result;
}

// @retail 0x1ee490
long c_count_interface::test_count()
{
	return get_count() > 0 ? 0 : -1;
}

// @retail 0x1ee4a0
long c_count_interface::next_index(dword index)
{
	if (index != NONE && index < (dword)(get_count() - 1))
		return index + 1;
	return NONE;
}

// @retail 0x1ee4d0
c_vertex_shape *c_count_interface::make_shape(long index, void *storage)
{
	return new (storage) c_vertex_shape((s_shape_source *)owner, 0, shape_value, index, 0, NULL);
}

// @retail 0x1ee500
c_vertex_shape *c_count_interface::make_transformed_shape(long index, long user_data,
	void *storage, const transform4x3f *matrix)
{
	return new (storage) c_vertex_shape((s_shape_source *)owner, 0, shape_value, index, user_data, matrix);
}

struct s_surface_key_array
{
 dword *keys;
 long count;
};

struct c_query_transform
{
 __m128 axes[3];
 __m128 position;
 void inverse_product(const c_query_transform *a, const c_query_transform *b);
};

struct c_query_point
{
 __m128 value;
 void inverse_transform(const c_query_transform *matrix, const __m128 *point);
};

struct s_query_bounds
{
 __m128 minimum, maximum;
};

struct c_surface_query_library
{
 void query_sphere(const __m128 *sphere, s_surface_key_array *keys);
 void query_box(const c_query_transform *matrix, const __m128 *extent, real tolerance, s_surface_key_array *keys);
 void query_bounds(const s_query_bounds *bounds, s_surface_key_array *keys);
};

#include "unknown_1c3b20.h"

c_shape_owner *g_51e9d4;

// @retail 0x1ee530 deleting c_shape_owner

// @retail 0x1ee560
c_shape_owner::~c_shape_owner()
{
	havok_reference_remove(object);
	object = 0;
	g_51e9d4 = 0;
}

#include "unknown_1efac0.h"

#include "unknown_1c3b70.h"

c_shape_global_owner *g_51e9d0;

struct s_bounds3d;
struct s_slot_entry_list;
extern s_bounds3d *g_4687e0;
extern s_slot_entry_list *g_4e0340;

struct s_vertex_bounds
{
	real x0, x1, y0, y1, z0, z1;
};

struct s_bounds_vertex
{
	point3f point;
	long field_c;
};

struct s_bounds_source
{
	byte field_0[0x38];
	long count;
	s_bounds_vertex *vertices;
};

PRIVATE __forceinline void bounds_half_extent_set(real maximum, real minimum, real volatile *destination)
{
    *destination = (maximum - minimum) * 0.5f;
}

// @retail 0x1eed40
c_shape_global_owner::c_shape_global_owner()
{
	field_8 = 0;
	g_51e9d0 = this;
	s_vertex_bounds bounds = *(s_vertex_bounds *)g_4687e0;
	s_bounds_source *source = (s_bounds_source *)g_4e0340;
	for (long i = 0; i < source->count; i++)
	{
		point3f *point = &source->vertices[i].point;
		if (point->x < bounds.x0) bounds.x0 = point->x;
		if (point->x > bounds.x1) bounds.x1 = point->x;
		if (point->y < bounds.y0) bounds.y0 = point->y;
		if (point->y > bounds.y1) bounds.y1 = point->y;
		if (point->z < bounds.z0) bounds.z0 = point->z;
		if (point->z > bounds.z1) bounds.z1 = point->z;
	}
	center.x = (bounds.x1 + bounds.x0) * 0.5f;
	center.y = (bounds.y1 + bounds.y0) * 0.5f;
	center.z = (bounds.z1 + bounds.z0) * 0.5f;
	center.w = 0.0f;
	bounds_half_extent_set(bounds.x1, bounds.x0, &extent.x);
	bounds_half_extent_set(bounds.y1, bounds.y0, &extent.y);
	bounds_half_extent_set(bounds.z1, bounds.z0, &extent.z);
	extent.w = 0.0f;
}

// @retail 0x1eefe0 deleting
c_shape_global_owner::~c_shape_global_owner()
{
	g_51e9d0 = 0;
}

void function_1eece0(s_surface_key_array *array, void *owner);
bool function_1ef3e0(void const *owner, dword key);

/* the sphere a child query takes: a centre with the radius in its w lane */
struct c_sphere_probe
{
 __m128 value;
 c_sphere_probe(const __m128 *center, real radius)
 {
  value = *center;
  ((real *)&value)[3] = radius;
 }
};

struct c_instance_surface_query
{
 virtual void v0() {}
 virtual void v1() {}
 virtual void v2() {}
 virtual void v3() {}
 virtual void v4() {}
 virtual void v5() {}
 virtual void v6() {}
 virtual void v7() {}
 virtual void v8() {}
 virtual void v9() {}
 virtual void query_sphere(const c_sphere_probe &sphere, s_surface_key_array *keys) {}
 virtual void query_box(const c_query_transform *matrix, const __m128 *extent, real tolerance, s_surface_key_array *keys) {}
};

struct s_query_instance
{
 transform4x3f matrix;
 short owner;
 byte field_36[0x58 - 0x36];
};

struct s_query_owner
{
 byte field_0[0x98];
 long surface_count;
 byte field_9c[0xb4 - 0x9c];
 byte *shape;
 byte field_b8[0xc8 - 0xb8];
};

struct s_query_globals
{
 byte field_0[0x13c];
 s_query_owner *owners;
 long count;
 s_query_instance *instances;
};

PRIVATE __forceinline void query_transform(const transform4x3f *matrix, c_query_transform *output)
{
 real *a = (real *)output;
 a[0] = matrix->forward.i; a[1] = matrix->forward.j; a[2] = matrix->forward.k; a[3] = 0.0f;
 a[4] = matrix->left.i; a[5] = matrix->left.j; a[6] = matrix->left.k; a[7] = 0.0f;
 a[8] = matrix->up.i; a[9] = matrix->up.j; a[10] = matrix->up.k; a[11] = 0.0f;
 __m128 translation;
 real *t = (real *)&translation;
 t[0] = matrix->position.x;
 t[1] = matrix->position.y;
 t[2] = matrix->position.z;
 t[3] = 0.0f;
 output->position = translation;
}

PRIVATE __forceinline void query_remove_key(s_surface_key_array *keys, long index)
{
 --keys->count;
 for (long i = index; i < keys->count; ++i)
  keys->keys[i] = keys->keys[i + 1];
}

PRIVATE __forceinline void query_remap_keys(void const *owner, s_surface_key_array *keys, long first, long instance_index)
{
 for (long i = first; i < keys->count; ++i)
 {
  dword key = ((keys->keys[i] | 0xffffa000) << 16) | instance_index;
  if (function_1ef3e0(owner, key)) keys->keys[i] = key;
  else { query_remove_key(keys, i); --i; }
 }
}

// @retail 0x1ee820
void c_shape_owner::query_box(const c_query_transform *matrix, const __m128 *extent,
 real tolerance, s_surface_key_array *keys)
{
 ((c_surface_query_library *)this)->query_box(matrix, extent, tolerance, keys);
 function_1eece0(keys, this);
 for (long i = 0; i < keys->count; ++i)
 {
  dword key = keys->keys[i];
  long instance_index = key & 0xffff;
  if ((key & 0xe0000000) == 0x40000000)
  {
   s_query_globals *globals = (s_query_globals *)g_4e0348;
   s_query_instance *instance = &globals->instances[instance_index];
   s_query_owner *owner = &globals->owners[instance->owner];
   if (owner->surface_count <= 0x2000)
   {
    byte *shape = owner->shape;
    c_query_transform transform, local;
    query_transform(&instance->matrix, &transform);
    local.inverse_product(&transform, matrix);
    query_remove_key(keys, i);
    long first = keys->count;
    --i;
    c_instance_surface_query *instance_query = (c_instance_surface_query *)(shape + 0x50);
    instance_query->query_box(&local, extent, tolerance, keys);
    query_remap_keys(&object, keys, first, instance_index);
   }
  }
 }
}

// @retail 0x1ee5b0
void function_1ee5b0(c_shape_owner *owner, const __m128 *sphere, bool expand_instances,
 bool filter, s_surface_key_array *keys)
{
 ((c_surface_query_library *)owner)->query_sphere(sphere, keys);
 if (filter) function_1eece0(keys, owner);
 if (expand_instances)
 {
  for (long i = 0; i < keys->count; ++i)
  {
   dword key = keys->keys[i];
   long instance_index = key & 0xffff;
   if ((key & 0xe0000000) == 0x40000000)
   {
    s_query_globals *globals = (s_query_globals *)g_4e0348;
    s_query_instance *instance = &globals->instances[instance_index];
    s_query_owner *definition = &globals->owners[instance->owner];
    if (definition->surface_count <= 0x2000)
    {
     byte *shape = definition->shape;
     c_query_transform transform;
     c_query_point center;
     center.value = *sphere;
     query_transform(&instance->matrix, &transform);
     center.inverse_transform(&transform, &center.value);
     query_remove_key(keys, i);
     long first = keys->count;
     --i;
     ((c_instance_surface_query *)(shape + 0x50))->query_sphere(c_sphere_probe(&center.value, ((const real *)sphere)[3]), keys);
     query_remap_keys(&owner->object, keys, first, instance_index);
    }
   }
  }
 }
}

// @retail 0x1eea50
void c_shape_owner::query_bounds(const s_query_bounds *bounds, s_surface_key_array *keys)
{
 ((c_surface_query_library *)this)->query_bounds(bounds, keys);
 function_1eece0(keys, this);
 for (long i = 0; i < keys->count; ++i)
 {
  dword key = keys->keys[i];
  long instance_index = key & 0xffff;
  if ((key & 0xe0000000) == 0x40000000)
  {
   s_query_globals *globals = (s_query_globals *)g_4e0348;
   s_query_instance *instance = &globals->instances[instance_index];
   s_query_owner *owner = &globals->owners[instance->owner];
   if (owner->surface_count <= 0x2000)
   {
    byte *shape = owner->shape;
    c_query_transform transform, world, local;
    query_transform(&instance->matrix, &transform);
    __m128 half = _mm_set_ss(0.5f);
    __m128 extent = _mm_mul_ps(_mm_shuffle_ps(half, half, 0), _mm_sub_ps(bounds->maximum, bounds->minimum));
    world.axes[0] = _mm_setzero_ps();
    world.axes[1] = _mm_setzero_ps();
    world.axes[2] = _mm_setzero_ps();
    ((real *)&world.axes[0])[0] = 1.0f;
    ((real *)&world.axes[1])[1] = 1.0f;
    ((real *)&world.axes[2])[2] = 1.0f;
    world.position = _mm_add_ps(bounds->minimum, extent);
    local.inverse_product(&transform, &world);
    query_remove_key(keys, i);
    long first = keys->count;
    --i;
    c_instance_surface_query *query = (c_instance_surface_query *)(shape + 0x50);
    query->query_box(&local, &extent, 0.0001f, keys);
    query_remap_keys(&object, keys, first, instance_index);
   }
  }
 }
}


// @retail 0x1ee800
void c_shape_owner::query_sphere(const __m128 *sphere, s_surface_key_array *keys)
{
 function_1ee5b0(this, sphere, true, true, keys);
}

struct s_type_1a7926
{
 byte unknown00[8];
 transform4x3f root_matrix;
 byte unknown3c[0x48 - 0x3c];
 short *node_indices;
 byte unknown4c[4];
 transform4x3f *field_50;
};

bool function_20a9a0(s_type_1a7926 *matrices, long object_index);
extern long *g_51e9cc;

struct c_child_transform : c_havok_reference_counted
{
 dword user;
 long field_c;
 c_query_transform transform;
 c_child_transform(c_havok_reference_counted *child);
};

// @retail 0x1ef070
c_child_transform *function_1ef070(dword key, void *storage)
{
 long position = (key >> 16) & 0x1fff;
 long object_index = g_51e9cc[key & 0xffff];
 byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
 byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
 byte *model = g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes;
 byte *region = *(byte **)(model + 0x74) + (position & 31) * 16;
 byte *choice = *(byte **)(region + 0xc) + ((position >> 5) & 255) * 8;
 if ((key & 0xe0000000) == 0x80000000)
 {
  byte *collision = g_4e3b44[*(long *)(model + 0xc) & 0xffff].bytes;
  byte *group = *(byte **)(collision + 0x20) + *(signed char *)(region + 4) * 12;
  byte *entry = *(byte **)(group + 8) + *(signed char *)(choice + 5) * 20;
  c_havok_reference_counted *child = (c_havok_reference_counted *)(*(byte **)(entry + 0x10) + 0x40);
  c_child_transform *result = new (storage) c_child_transform(child);
  havok_reference_remove(child);
  object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
  const transform4x3f *matrix = (const transform4x3f *)(object + *(short *)(object + 0x116));
  c_query_transform transform;
  query_transform(matrix, &transform);
  c_query_transform *destination = &result->transform;
  for (long axis = 0; axis < 3; ++axis)
   destination->axes[axis] = transform.axes[axis];
  destination->position = transform.position;
  result->user = key;
  return result;
 }
 else
 {
  s_type_1a7926 info;
  function_20a9a0(&info, object_index);
  byte *physics = (byte *)info.node_indices;
  byte *group = *(byte **)(physics + 0xc4) + *(signed char *)(region + 5) * 12;
  byte *entry = *(byte **)(group + 8) + *(signed char *)(choice + 6) * 12;
  long index = **(short **)(entry + 8);
  byte *body = *(byte **)(physics + 0x3c) + index * 0x90;
  c_child_transform *result = new (storage) c_child_transform(*(c_havok_reference_counted **)(body + 0x38));
  havok_reference_remove(*(c_havok_reference_counted **)(body + 0x38));
  long node = *(short *)body;
  const transform4x3f *matrix = node == NONE ? &info.root_matrix : &info.field_50[node];
  c_query_transform transform;
  query_transform(matrix, &transform);
  c_query_transform *destination = &result->transform;
  for (long axis = 0; axis < 3; ++axis)
   destination->axes[axis] = transform.axes[axis];
  destination->position = transform.position;
  result->user = key;
  return result;
 }
}

struct s_havok_transform;
void function_181f80(s_havok_transform *transform, const transform4x3f *matrix);
c_a *surface_empty_shape(void *storage);

// @retail 0x1ef950
void *c_shape_global_owner::make_shape(dword key, void *storage)
{
 if (function_1ef3e0(this, key))
 {
  long index = key & 0xffff;
  long surface = (key >> 16) & 0x1fff;
  long kind = key >> 29;
  if (kind == 1)
   return new (storage) c_vertex_shape((s_shape_source *)g_4e0340, kind, NONE, index, key, NULL);
  else if (kind == 2)
  {
   s_query_globals *globals = (s_query_globals *)g_4e0348;
   s_query_instance *instance = &globals->instances[index];
   c_havok_reference_counted *child = (c_havok_reference_counted *)(globals->owners[instance->owner].shape + 0x40);
   c_child_transform *result = new (storage) c_child_transform(child);
   havok_reference_remove(child);
   function_181f80((s_havok_transform *)&result->transform, &instance->matrix);
   return result;
  }
  else if (kind == 5)
  {
   s_query_globals *globals = (s_query_globals *)g_4e0348;
   s_query_instance *instance = &globals->instances[index];
   c_count_interface *owner = (c_count_interface *)globals->owners[instance->owner].shape;
   return owner->make_transformed_shape(surface, key, storage, &instance->matrix);
  }
  else return function_1ef070(key, storage);
 }
 return surface_empty_shape(storage);
}
