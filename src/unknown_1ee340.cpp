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

/* the hit flag ray_test returns by value (retail hands back the caller's pointer in EAX) */
struct s_ray_test_hit
{
	bool value;
	s_ray_test_hit(bool hit) { value = hit; }
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
	virtual s_ray_test_hit ray_test(const s_shape_ray *ray, s_shape_ray_result *result);
};

/* one of the shape's vertices as the physics library's four-real vector: its components are read and
   written through the component accessor */
struct s_shape_vector4
{
	real components[4];

	real &operator()(long index) { return components[index]; }

	void set(real x, real y, real z, real w)
	{
		(*this)(0) = x;
		(*this)(1) = y;
		(*this)(2) = z;
		(*this)(3) = w;
	}
};

/* the radius every vertex shape is made with (retail reads it from .data and only the constructor does) */
real g_47f068 = 0.01f;

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

/* Retail keeps the radius load ahead of the member stores, as it does for a global whose address is taken, so
   the base's argument is read through the global's address. */
// @retail 0x1edd00
c_vertex_shape::c_vertex_shape(s_shape_source *source, byte type, long data, long index,
	long user_data, const transform4x3f *matrix) : radius_shape_2de750(*(real const *)&g_47f068)
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
		s_shape_vector4 *vertex = (s_shape_vector4 *)&vertices_raw[vertex_count * 4];
		vertex->set(point->x, point->y, point->z, 0.0f);
		*(long *)&vertices_raw[vertex_count * 4 + 3] = vertex_count | 0x3f000000;
		vertex_count++;
		edge_index = edge->next[side];
	} while (edge_index != first_edge);
	material = surface->material;
	flags = surface->flags;
	user_value = user_data;
}

short function_120850(const vector3f *vector);
bool function_23a220(const point2f *points, short count, const point2f *point, real epsilon);

// Matched in m3 lane 14 r113 (was 192): 0x23a220 takes the points before the point (all three retail callers, 0x173910,
// 0x184440 and this one, set the point in EBX before the points in EDX), with round 59's chain below; the older notes
// follow. Earlier: frame 0x84 against retail's 0x90, the ray loads and the origin and edge homes; BLOCKED-BY 0x23a220
// (its definition and its count parameter's width are in another lane's file). One note, the rounds' entries below.
// MiniMax H (11-18): no new form, no lower score (192): walking retail's body against this build again, the frame
// stays 0x84 against retail's 0x90 - retail loads the ray's six components and the shape's vertices up front and forms
// the direction and the two edge vectors in xmm registers (the ray start's three components stay in xmm3-xmm5), while
// this build forms each difference and spills it to a stack home - and the polygon count is still pushed as a word
// here and as a dword in retail, which the short count's type (declared outside this lane's files) forbids:
// BLOCKED-BY 0x23a220; the earlier permuter runs over this body (34 tries) found nothing below the baseline.
// MiniMax R run 92 (11-17): no new form, no lower score (192): the permuter's 34 annealed tries over the reduced image
// found nothing below its 187 baseline there, and the frame is still 0x84 against retail's 0x90 with the origin's x
// and y and the edge vectors in stack homes; the point-in-polygon query still needs the points before the point:
// BLOCKED-BY 0x23a220 (src/unknown_239b80.cpp).
// MiniMax C (11-16): no new form, no lower score (192): the call to the matched 0x23a220 already gives the point in
// EBX and the points in EDX as retail does, so the parameter order is no longer the blocker; what is left is retail
// pushing the count as a dword (push edx, the count it already holds) where ours truncates it to a word first (xor
// eax, eax; mov ax, word ptr [esi + 0x14]; push eax), so that count is a 32-bit type in retail and is declared short
// in src/unknown_239b80.cpp and in the declarations of src/unknown_170d70.cpp, src/unknown_184440.cpp and this file,
// which this lane may not edit; the frame map follows from it (retail's polygon at +0x50 in a 0x90 frame where ours
// is at +0x44 in a 0x84 frame, retail spilling the direction and the ray start that we keep in registers).
// MiniMax D (11-16): no new form, no lower score (192): this round compared retail's body with ours instruction by
// instruction and the remaining delta is unchanged: retail's frame is 0x90 and it keeps the ray's origin x and y and
// the two edge vectors in stack homes while ours keeps them in registers in a 0x84 frame, and the point-in-polygon
// query still has to have the points before the point, which is the helper 0x23a220's argument order - the polygon
// count's 32-bit type is declared short in src/unknown_239b80.cpp and in the headers this lane may not edit:
// BLOCKED-BY 0x23a220 and those declarations.
// An earlier round (192): no new form and no lower score; the frame is still 0x84 against retail's 0x90 with the
// origin's x and y and the edge vectors in stack homes, and the point-in-polygon query still needs the points
// before the point: BLOCKED-BY 0x23a220 (src/unknown_239b80.cpp).
// MiniMax B (11-14): no new form, no lower score (192): retail computes the ray direction first from the ray pointer and keeps the start's three components in xmm3-xmm5 while it forms the two edge vectors and the cross product in registers (frame 0x90), where ours forms each difference and spills it to a stack home (frame 0x84); the point-in-polygon query still needs the points before the point: BLOCKED-BY 0x23a220.
// MiniMax C (11-14): no new form, no lower score (192): retail's frame is 0x90 and it loads the ray's and
// the vertices' six components into xmm2-xmm7 before it forms the three edge vectors in registers, while
// ours forms each difference and spills it to a stack home (frame 0x84); a permutation search over this
// body (33 tries) found no variant below the baseline, and the point-in-polygon query still needs the
// points before the point: BLOCKED-BY 0x23a220.
// MiniMax A (11-13): no new form, no lower score (192): the origin as an aggregate initialiser compiles to
// the same bytes; the frame is still 0x84 against retail's 0x90 with the origin's x and y and the edge
// vectors in stack homes, and the point-in-polygon query still needs the points before the point, which
// only the definition in another lane's file fixes: BLOCKED-BY src/unknown_239b80.cpp.
// MiniMax G (11-12): no new form, no lower score (192); the frame is still 0x84 against retail's 0x90 with the origin's x and y and the edge vectors in stack homes, and the point-in-polygon query still needs the points before the point, which only the definition in another lane's file fixes.
// MiniMax E (11-11): no new form, no lower score (192); the frame is still 0x84 against retail's 0x90 with the
// origin's x and y and the edge vectors in stack homes, and the point-in-polygon query still needs the points before
// the point, which only the definition in another lane's file fixes (BLOCKED-BY 0x23a220).
// MiniMax D (11-14): no new form, no lower score (192): retail's frame is 0x90 and it keeps the ray's six
// floats in xmm0-xmm5 while it forms the two edge vectors and the cross product in registers, where ours
// forms each difference and spills it; the point-in-polygon query still needs the points before the point
// (BLOCKED-BY 0x23a220), and the permuter's 40 tries over the reduced image found nothing below the 187
// baseline.
// A later round (192): no new form and no lower score; the frame is still 0x84 against
// retail's 0x90 with the origin's x and y and the edge vectors in stack homes, and the point-in-polygon
// query still needs the points before the point, which only the definition in another lane's file fixes.
// MiniMax I (11-09): no new form, no lower score (192); the aligned view shows retail loading the ray's and the
// vertices' six components into xmm2-xmm7 first and computing the three edge vectors in registers (frame 0x90),
// while ours forms each difference and spills it to the stack (frame 0x84), because the helpers take their
// operands by reference and the edge vectors need homes; the header's normalize_inline and cross3f_staged
// spellings leave the same code, and the count read into a dword local before the query call gives 193 (1020
// bytes). Still different: the ray and edge homes and the call setup, which is BLOCKED-BY 0x23a220 (retail's
// callers pass the points before the point and its definition is in src/unknown_239b80.cpp, another lane's
// file); the chain that reaches 2 only gains on top of that swap.
// MiniMax C (11-09): no new form, no lower score (192); the point-in-polygon query's call setup still differs (BLOCKED-BY src/unknown_239b80.cpp).
// MiniMax I (11-05): no new form, no lower score. The header normalize_inline for the normal gives 193
// and the header cross3f_staged for the cross gives the same code; retail's normal length sums k, j, i
// (with the components scaled inv*i, j*inv, k*inv) and neither header helper has that order. The rest
// is blocked by 0x23a220: retail's callers pass the points before the point, and its definition is in
// src/unknown_239b80.cpp, another lane's file.
// MiniMax B (11-03): no new form, no lower score. Retail still keeps the ray's components and the two
// edge vectors in xmm registers while ours spills the edge vectors to the stack and reloads them; the
// cause is that `first` and `second` are address-taken locals that cross3f takes by reference, so
// they need a home. The caller 0x23a220 passes its arguments in retail's order, but its definition
// lives in another lane's file, so nothing here was changed.
// MiniMax S (11-06): no new form, no lower score. Retail still sets up the polygon query with the point in
// EBX and the polygon array in EDX and the count and epsilon on the stack, so the call to 0x23a220 needs
// that argument order; its definition (src/unknown_239b80.cpp) is another lane's file, so the order cannot
// be changed here, and every call site of it keeps the four arguments on the stack.
// MiniMax S (10-31): no new form, no lower score. Retail keeps the shape's edges and the ray direction
// in xmm1-xmm7 and computes the three edge vectors in registers, storing only four floats; ours spills
// every component of every difference to the stack and re-reads them, so the frame is 0x84 against 0x90.
// Earlier at 192: frame 0x84 versus 0x90: retail keeps origin.x/y in xmm6/xmm7 until the
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
// Round 267: Native producer order (O origin, D direction, S start, A/B first/second edge):
// OBADS/OBDAS/OBDSA/ODBAS/ODBSA/DOBAS/DOBSA/OADBS 192/192/191/191/191/191/191/193.
// One-point forms restored with every original producer; frame 0x84 and origin/edge homes remain.
// Round 269: Fraction numerator/reciprocal/both/reversed producer order, mutable and const,
// 192 each in eight forms. Frame 0x84 and retained-origin/edge-I homes remain. Original
// fraction expression, all geometry helpers and complete body restored.
// Round 23 (lane 18, fast; re-measured on the 192 base, earlier numbers were on 247): found declared
// first 192 (same code); plane d cleared before normalizing 200; fraction as one conditional 259; the
// clamp as an if/else-if on a copy 256; the hit block's fraction stored after the normal and d 192
// (same code: retail's fraction-first store order is scheduling).
// Round 68 (lane 12): start copy first with the direction from the copy 230 (retail's single start loads,
// exact LCS 132 against 133, but origin is still re-read from the shape, so no edge spills); copy first
// with the direction from ray->start 192; copy-first plus origin set after both 230; hit-block result
// writes with `found = true` last 239 (1045 bytes); a short count local for the call 297. Unused-local
// probes +1/+2/+3/+4/+8/+12 all 192 (diagnostic: no symbol-numbering effect here).
// Round 114 (m3 lane 7, fast): `point3f ray_start = ray->start; point3f ray_end = ray->end;` aggregate copies with
// the direction from them 286 (1023 bytes), with the copies ahead of origin 286; origin as an aggregate copy of the
// first vertex 282 (frame 0x94); producer orders ODSAB 193 and OABDS 193.
// Round 40 (m3 lane 15, fast = plain): retail loads the ray's start and end once each before any store. Start copy
// first with the direction from it 230; direction as three scalars (diagnostic, helpers written out) 226 (991 bytes);
// both 225 (991); start copy first with the offset from ray->start 223 (991); start and end copied and the direction
// from both copies 196 (all six loads before the subtractions, start.z first as in retail; the end copy is one-use).
// Round 286: CRT origin/start/both copies with original/CRT normal publication
// 282/307/204/264/270/291; both input copies plus the hit-point setter 270/291.
// Origin retention and edge homes remain; original copies, stores and body restored.
// DL220: Scalar origin with native difference constructors, crossed with scalar start, ray constructor and staged cross:
// 192/195/192/195/192/195/192/195. Original complete body restored; original frame and register homes remain.
// DL225: joint component traversal of edge pairs in both store orders 193/192; with offset
// 255/255, with ray direction 193/192, with both 267/267. All restored; original helpers,
// frame 0x84 and retained-origin/edge homes remain.
// Round 450: Existing dot-helper complete products/partial sum/accumulation/native inputs,
// original then shared polygon count, 192/253/253/320/193/269/269/315. Original complete shape
// query and shared dot helper restored; frame 0x84 and retained-origin/edge homes remain.
// Round 59 (m3 lane 1, fast; all restored, match-only forms). BLOCKED-BY 0x23a220: every retail caller (0x173910, 0x184440,
// this one) sets up the point in EBX before the points in EDX, so its points parameter comes before point; the swap keeps
// 0x23a220 matched, but its definition is in src/unknown_239b80.cpp, another lane's file this round. With that swap, the
// chain below gives a byte match in the fast check. The chain, each on the last: ray start, ray end and the second and
// third vertices copied into named points through real_point3d_set, directions from those copies, 192 (aligned 120: all six
// loads first, origin in xmm6/xmm7, frame 0x90); fraction by if/else 216; the hit flag as a one-byte class with a ctor, local
// and returned by value (retail's slot +0x3c, hidden pointer in EAX) 203; a literal 0.0f for field_c (xmm6 shared with the
// positive test) 203; the count read into a dword local before the call (`push edx`, this moves to ESI, 1001 bytes) 179; the
// normal read through a local pointer in the hit block (retail's in-place j/k normalize) 91; offset assigned directly 58;
// `long const &key_ref = function_120850(..); short axis = (short)key_ref;` (retail's dead movsx) 19; fraction from a
// forceinline helper returning it, the hit point through vector3f_add_scaled 7; the result normal through a
// `vector3f *const &` temporary (fraction store first) 2; only the two lea lines left. Without the chain, no gain: normalize
// statement orders on the 203 form: each `*=` written out, scale local 205, vector3f_scale with a scale local 205, normalize_inline 213; the hit point
// through real_point3d_set 62, a forceinline point helper 58 (on 58); side index, row pointer and named axes before the hit point 19/87/114/117 (on 19);
// four caller-side argument temporaries 2 each.
// DL227: Existing shared difference: complete first/second/both point inputs 431/390/566;
// corresponding scalar inputs 192/192/192; staged scalar/vector outputs 192/466. Original shared
// helper and body restored; frame 0x84, retained-origin/edge homes and ray loads remain.
// DL233: Whole-plane native/library publication, zero-d before/after normalization, producer/entry
// plane storage: 257/257/259/259/257/257/259/259. Original plane/output body restored;
// origin retention and edge homes still leave frame 0x84 versus 0x90.
// DL235: Retained projection-table row, pointer/reference, original/shared polygon count,
// short/long axis: 231/231/247/247/231/231/247/247. All restored;
// frame 0x84 versus 0x90, retained-origin and edge spills remain.
// Round 204 (fast, without the 0x23a220 parameter swap, its definition still in another lane's file): the
// chain's first steps rebuilt here: ray start/end and second/third vertices copied through real_point3d_set,
// directions from the copies 212; plus fraction by if/else 236; plus a dword count local for the loop and call
// 249 (1020 bytes). On the original body: a literal 0.0f for field_c 192; the result normal through a local
// pointer 192 (same code). The chain only gains on top of the swap. Original body restored.
// Round 84 (m3 lane 9, fast control 192; 0x23a220's definition still in another lane's file, so no parameter
// swap): the hit block now has retail's code apart from homes, the dead second `movsx` after the axis call and the
// first product's operand order. Forms: the hit point accumulated onto a copy of the ray start 301 (1039 bytes); a
// named side row `axis * 2 + (n[axis] > 0)` for all four table reads 251; the projected point as an aggregate
// initializer 192 (same code); the polygon array declared before the hit point 192 (same code); the hit-point sums
// written start-first in all three rows 192 (same code). All restored; still BLOCKED-BY 0x23a220.
// J260: Direction dot/fraction/bounded comparison double storage singles 268/293/278; pairs 300/307/293; all 280; immutable all 280; frames 0x84/0x88 versus 0x90. No exact match; original SSE scalar widths restored.
// NOT kept: no eligible native repair; opening executable source restored. Retained origin x/y, edge i spills and plane/ray/polygon homes remain.
// Round 66 (m3 lane 15, fast; 0x23a220, 0x1eded0, 0x1ee350, 0x120850 matched throughout): 0x23a220 still takes the point before the points and its definition is
// still in another lane's file, so the chain stays BLOCKED-BY 0x23a220. Re-measured on today's tree without the swap, all 31 combinations of: the second and third
// vertices copied into named points through real_point3d_set (P), the ray end copied too with the direction from both copies (E), the clamp as if / else-if (I), a
// literal 0.0f for field_c (Z), a dword count for the loop and the call (N). I and Z alone or together 192 (same code); N 195 (1020 bytes); E 196, EN 199; P 214,
// PN 210 (1020), PE 212, PEN 220; I and Z change none of these. Nothing below 192.
// m3 lane 6 r181: no new form, no lower score (192): the first steps of the round 59 chain still need 0x23a220's points
// parameter ahead of its point (src/unknown_239b80.cpp is on this round's avoid list), and every in-function form left
// (start/end copies, count local, clamp, field_c literal, hit-block order, axis width) is listed above at 192 or worse.
// b7N (11-17) walk against retail, fast build on r830: no behaviour difference found, source unchanged (192 control,
// 1007 bytes against retail's 1001). Walked every block: the ray end - start at 0x1edf62..0x1edf82 and the two edges
// from vertex 0 (0x1edf8f..0x1edff6), the cross product (components and operand order), the normalize (float rounding
// of the length at 0x1ee068, skip below 1e-4 at 0x1ee074/0x1ee078, scale 1.0f / length at 0x1ee07a..0x1ee0a3), d = 0
// at 0x1ee0b5, the offset from the start, `dot != 0` (0x1ee0fe..0x1ee105: equal skips, unordered computes), the -1.0f
// / dot fraction (0x1ee113..0x1ee12b), `fraction < result->fraction` (0x1ee148/0x1ee14b), the clamp and its
// ordered-equal test (0x1ee151..0x1ee16d), `n[axis] > 0` (0x1ee185..0x1ee18e), the three hit-point sums from the
// fraction (not the clamped value), the table at 0x440b94 (18 shorts read from the XBE equal g_440b94), the dword
// count and its loop (0x1ee202..0x1ee2c3), the call (push 0 = epsilon, push edx = count; the callee reads the count as
// a word, so short is right) and the hit block (0x1ee2e9..0x1ee316: fraction, normal, field_c = the zero in xmm6,
// result + 0x10 untouched). All of it agrees in field, direction, constant and rounding. Only the known non-behaviour
// deltas remain: frame 0x84 against 0x90, the dead second movsx at 0x1ee182, the count pushed as a dword at 0x1ee2c7,
// retail's store of plane.d at 0x1ee0b5.
// M3 lane 7 r176 (fast with 0x23a220, 0x120850, 0x173910, 0x184440 and the file's functions; control 192; 0x23a220's
// definition is still in another lane's file, so no parameter swap): the result normal as one aggregate copy 275 (1019 B);
// the positive flag as a long 251; the normalize through the out-of-line 0x30bf0 287 (968 B, it stays a call); both dot3f
// calls with their operands exchanged 192 (same code); the offset taken from the shape's first vertex in place of the origin
// copy 259 (991 B); the found flag declared after the normalize 260. Retail loads the ray's start and end into registers
// once, subtracts there and stores the start copy from the same registers (frame 0x90): the value-argument setter form for
// both points, which needs a one-use end copy and so stays with the BLOCKED-BY 0x23a220 chain above.
// MiniMax B (11-18): score 192 (was 192): no new form, no lower score. Read the retail frame again: with the frame base
// at esp, retail keeps the length at +0, the plane at +4 (its three components) with plane.d at +0x10, the direction at
// +0x14, the ray start at +0x20, the projected point's home at +0x14 (the direction's dead home) and the polygon buffer
// at +0x50..+0x8f, so the 0x90 frame holds three more dwords above the ray start than this build's 0x84 frame, and every
// local therefore sits at another offset; retail stores plane.d (the zero) and keeps the offset, the origin and the two
// edge vectors in registers, as this build does, so the extra twelve bytes are the allocator's own, not a missing local.
// The chain stays BLOCKED-BY 0x23a220, whose definition is in another lane's file.
static __forceinline real plane_ray_fraction(vector3f const *normal, vector3f const *along, vector3f const *offset)
{
	real dot = dot3f(normal, along);
	real fraction;
	if (dot != 0.0f)
		fraction = dot3f(normal, offset) * (-1.0f / dot);
	else
		fraction = 0.0f;
	return fraction;
}

// @retail 0x1edf50
s_ray_test_hit c_vertex_shape::ray_test(const s_shape_ray *ray, s_shape_ray_result *result)
{
	point3f origin;
	real_point3d_set(&origin, vertices_raw[0], vertices_raw[1], vertices_raw[2]);
	point3f ray_end;
	real_point3d_set(&ray_end, ray->end.x, ray->end.y, ray->end.z);
	point3f ray_start;
	real_point3d_set(&ray_start, ray->start.x, ray->start.y, ray->start.z);
	vector3f direction;
	vector3d_from_points3d(&ray_start, &ray_end, &direction);
	vector3f first, second;
	point3f p1, p2;
	real_point3d_set(&p1, vertices_raw[4], vertices_raw[5], vertices_raw[6]);
	real_point3d_set(&p2, vertices_raw[8], vertices_raw[9], vertices_raw[10]);
	vector3d_from_points3d(&origin, &p1, &first);
	vector3d_from_points3d(&origin, &p2, &second);
	plane3f plane;
	cross3f(&first, &second, &plane.n);
	s_ray_test_hit found(false);
	normalize3d(&plane.n);
	plane.d = 0.0f;
	vector3f offset;
	offset.i = ray_start.x - origin.x;
	offset.j = ray_start.y - origin.y;
	offset.k = ray_start.z - origin.z;
	real fraction = plane_ray_fraction(&plane.n, &direction, &offset);
	if (fraction < result->fraction)
	{
		real bounded = fraction < 0.0f ? 0.0f : fraction > 1.0f ? 1.0f : fraction;
		if (bounded == fraction)
		{
			long const &dominant_axis = function_120850(&plane.n);
			short axis = (short)dominant_axis;
			bool positive = plane.n.n[axis] > 0.0f;
			point3f point;
			vector3f_add_scaled((vector3f const *)&ray_start, &direction, fraction, (vector3f *)&point);
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
			long count = vertex_count;
			if (function_23a220(polygon, (short)count, &projected, 0.0f))
			{
				found.value = true;
				result->fraction = fraction;
				vector3f const *normal = &plane.n;
				vector3f *const &out = &result->normal;
				out->i = normal->i;
				out->j = normal->j;
				out->k = normal->k;
				result->field_c = 0.0f;
			}
		}
	}
	return found;
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
