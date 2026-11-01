#include "unknown_11c920.h"
#include "matrix_transform.h"
#include "normalize3d.h"
#include "unknown_0259d0.h"
#include <math.h>
#include "vector_products.h"
#include "unknown_11cc90.h"

// @flags /O2 /arch:SSE /Gr

struct s_prism_245270
{
	long field_00;
	long field_04;
	long field_08;
	byte field_0c;
	byte field_0d;
	short field_0e;
	plane3f field_10;
	real field_20;
	short field_24;
	byte field_26;
	byte field_27;
	long field_28;
	point2f field_2c[8];
};

struct s_collection_245270
{
	short field_00;
	short field_02;
	short field_04;
	byte field_06[0x4c08 - 6];
	s_prism_245270 field_4c08[256];
};

short function_120850(vector3f const *v);

PRIVATE inline void function_245270_project(point3f const *point, short axis,
	byte side, point2f *out)
{
	real x = point->n[g_440b94[axis * 2 + side][0]];
	real y = point->n[g_440b94[axis * 2 + side][1]];
	out->x = x;
	out->y = y;
}

PRIVATE __forceinline void function_24526f(real thickness, short count, point3f const *points, plane3f const *plane,
	real height, long field_00, long field_04, long field_08, byte field_0c,
	byte field_0d, short field_0e, s_collection_245270 *collection)
{
	point3f const *const *local_points = &points;
	if (collection->field_04 < 256)
	{
		s_prism_245270 *prism = &collection->field_4c08[collection->field_04++];
		prism->field_00 = field_00;
		prism->field_04 = field_04;
		prism->field_08 = field_08;
		prism->field_0c = field_0c;
		prism->field_0d = field_0d;
		prism->field_0e = field_0e;
		prism->field_10 = *plane;
		prism->field_20 = thickness;
		prism->field_24 = function_120850(&prism->field_10.n);
		prism->field_26 = prism->field_10.n.n[prism->field_24] > 0.f ? 1 : 0;
		prism->field_28 = count;
		for (short i = 0; i < prism->field_28; ++i)
		{
			function_245270_project(&(*local_points)[i], prism->field_24, prism->field_26, &prism->field_2c[i]);
		}
		if (height > 0.f && plane->k < 0.f)
		{
			prism->field_10.d -= prism->field_10.k * height;
			if (prism->field_24 != 2)
			{
				short axis = g_440b94[prism->field_24 * 2 + prism->field_26][1] == 2;
				for (short i = 0; i < prism->field_28; ++i)
					((real *)&prism->field_2c[i])[axis] -= height;
			}
		}
	}
}

// @retail 0x245270
void function_245270(real thickness, short count, point3f const *points, plane3f const *plane,
	real height, long field_00, long field_04, long field_08, byte field_0c,
	byte field_0d, short field_0e, s_collection_245270 *collection)
{
	function_24526f(thickness, count, points, plane, height, field_00, field_04,
		field_08, field_0c, field_0d, field_0e, collection);
}

struct s_244de0
{
	long field_00;
	long field_04;
	long field_08;
	byte field_0c;
	byte field_0d;
	short field_0e;
	point3f field_10;
	vector3f field_1c;
	real field_28;
};

PRIVATE __forceinline void function_244de2(real thickness, short count, point3f const *points, real arg_0, real arg_1, real arg_2, real arg_3,
	real height, long field_00, long field_04, long field_08, byte field_0c,
	byte field_0d, short field_0e, s_collection_245270 *collection)
{
	if (collection->field_04 < 256)
	{
		s_prism_245270 *prism = &collection->field_4c08[collection->field_04++];
		prism->field_00 = field_00;
		prism->field_04 = field_04;
		prism->field_08 = field_08;
		prism->field_0c = field_0c;
		prism->field_0d = field_0d;
		prism->field_0e = field_0e;
		vector3f *normal = &prism->field_10.n;
		normal->i = arg_0;
		normal->j = arg_1;
		normal->k = arg_2;
		prism->field_10.d = arg_3;
		prism->field_20 = thickness;
		prism->field_24 = function_120850(&prism->field_10.n);
		prism->field_26 = prism->field_10.n.n[prism->field_24] > 0.f ? 1 : 0;
		prism->field_28 = count;
		for (short i = 0; i < prism->field_28; ++i)
		{
			function_245270_project(&points[i], prism->field_24, prism->field_26, &prism->field_2c[i]);
		}
		if (height > 0.f && arg_2 < 0.f)
		{
			prism->field_10.d -= prism->field_10.k * height;
			if (prism->field_24 != 2)
			{
				short axis = g_440b94[prism->field_24 * 2 + prism->field_26][1] == 2;
				for (short i = 0; i < prism->field_28; ++i)
					((real *)&prism->field_2c[i])[axis] -= height;
			}
		}
	}
}

// score 6 (was 6): the sum of squares is emitted with the x87 terms in the
// other order and the loads inside the second inlined call are ordered
// differently; the frame, the two prism writes and the calls all match.
// Zero-initialised then x, y 6; the direction's i/j copied and rotated through
// a temporary 244 (y saved) / 224 (x saved); a separate projected direction 155;
// y stored before x with a const view for the sum 136.
// Round 31: the hand-written normalization replaced by the existing normalize2d_fast on a
// vector2f 6 (identical code, kept); on that: y written before x 136 (x squared first but taken
// from st0); the dot computed just before the first prism 92; the swapped corners exchanged the
// other way 18; `-x` for `0.f - x` in the second prism 6. Compile-only: writing only x through a
// pointer gives retail's square order and reload, but puts the second prism's loads in y, x,
// distance order (retail y, distance, x); routing both prisms through function_24526f with a
// plane local grows the frame.
// Round 68: point2f normalization view 6; length local / condition-scoped length 6/6;
// shared 2d dot / 3d add / both 6/6/6; lower corner copied from the first 6;
// upper and lower shared setters together 203. Original body restored.
// @retail 0x244de0
void function_244de0(point3f const *arg_0, vector3f const *arg_1, real arg_2,
	real arg_3, long arg_4, long arg_5, long arg_6, byte arg_7,
	byte arg_8, short arg_9, s_collection_245270 *arg_10)
{
	if (arg_10->field_02 < 256)
	{
		s_244de0 *local_0 = &((s_244de0 *)((byte *)arg_10 + 0x2008))[arg_10->field_02++];
		local_0->field_00 = arg_4;
		local_0->field_04 = arg_5;
		local_0->field_08 = arg_6;
		local_0->field_0c = arg_7;
		local_0->field_0d = arg_8;
		local_0->field_0e = arg_9;
		local_0->field_10 = *arg_0;
		local_0->field_1c = *arg_1;
		local_0->field_28 = arg_3;
	}
	if (arg_2 > 0.f)
	{
		if (arg_10->field_02 < 256)
		{
			s_244de0 *local_0 = &((s_244de0 *)((byte *)arg_10 + 0x2008))[arg_10->field_02++];
			local_0->field_00 = arg_4;
			local_0->field_04 = arg_5;
			local_0->field_08 = arg_6;
			local_0->field_0c = arg_7;
			local_0->field_0d = arg_8;
			local_0->field_0e = arg_9;
			real_point3d_set(&local_0->field_10, arg_0->x, arg_0->y, arg_0->z - arg_2);
			local_0->field_1c = *arg_1;
			local_0->field_28 = arg_3;
		}
		vector2f local_1 = { -arg_1->j, arg_1->i };
		if (normalize2d_fast(&local_1) != 0.f)
		{
			real local_4 = arg_0->y * local_1.j + arg_0->x * local_1.i;
			point3f local_5[4];
			local_5[0] = *arg_0;
			local_5[1].x = arg_0->x + arg_1->i;
			local_5[1].y = arg_0->y + arg_1->j;
			local_5[1].z = arg_0->z + arg_1->k;
			local_5[2] = local_5[1];
			local_5[2].z -= arg_2;
			local_5[3] = *arg_0;
			local_5[3].z -= arg_2;
			function_244de2(arg_3, 4, local_5, local_1.i, local_1.j, 0.f, local_4, 0.f, arg_4, arg_5, arg_6, arg_7, arg_8, arg_9, arg_10);
			point3f local_6 = local_5[1];
			local_5[1] = local_5[3];
			local_5[3] = local_6;
			function_244de2(arg_3, 4, local_5, 0.f - local_1.i, 0.f - local_1.j, 0.f, 0.f - local_4, 0.f, arg_4, arg_5, arg_6, arg_7, arg_8, arg_9, arg_10);
		}
	}
}

struct s_material_245400
{
	long field_00;
	byte field_04;
	byte field_05;
	short field_06;
};

struct s_surface_245400
{
	byte field_00[8];
	short field_08;
	byte field_0a[2];
};

struct s_vertex_245400
{
	point3f field_00;
	word field_0c;
	word field_0e;
};

struct s_source_245400
{
	byte field_00[0x2c];
	s_material_245400 *field_2c;
	long field_30;
	s_surface_245400 *field_34;
	long field_38;
	s_vertex_245400 *field_3c;
};

struct s_line_list;
void function_244ca0(real height, real radius, s_line_list *list, long a, long b, long c,
	byte d, byte e, point3f const *position, short f);

PRIVATE inline point3f *function_245400_transform(transform4x3f const *matrix,
	point3f const *point, point3f *out)
{
	real x = point->x;
	real y = point->y;
	real z = point->z;
	if (matrix->scale != 1.f)
	{
		x = matrix->scale * x;
		y = matrix->scale * y;
		z = matrix->scale * z;
	}
	out->x = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x + matrix->position.x;
	out->y = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x + matrix->position.y;
	out->z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z;
	return out;
}

// @retail 0x245400
void function_245400(s_source_245400 const *source, long index,
	transform4x3f const *matrix, real height, real radius, long field_00,
	long field_04, s_line_list *list)
{
	s_vertex_245400 const *vertex = &source->field_3c[index];
	s_surface_245400 const *surface = &source->field_34[vertex->field_0c];
	s_material_245400 const *material = &source->field_2c[surface->field_08];
	point3f transformed;
	point3f const *point = matrix ?
		function_245400_transform(matrix, &vertex->field_00, &transformed) : &vertex->field_00;
	function_244ca0(height, radius, list, field_00, field_04, surface->field_08,
		material->field_04, material->field_05, point, material->field_06);
}
// score 209 (was 209): the frame is 0x24 against retail's 0x2c; the source now arrives in ECX as in
// retail (the plane table read at the top of the material-test block, the parameters in 0x245400's
// source-first order), which matches the caller 0x245aa0, but the register homes, the plane table's
// home and the side bytes still differ.
// Earlier at 203 (MiniMax H): no new in-file form moved it; retail declares 0x245540 with its
// matrix and source the other way round, and that declaration and its call live in src/unknown_245850.cpp.
// Earlier at 203 (from 203): the frame is 0x24 against retail's 0x2c and the source receiver's home
// differs; retail declares 0x245540 with its matrix and source the other way round, and that
// declaration and its call live in src/unknown_245850.cpp.
// Earlier at 203 (from 203): frame 0x24 versus 0x2c; source receiver and side-byte homes remain.
// Earlier: the transform block is the only place left where retail's arithmetic
// differs from the shared helpers: retail's point transform sums (forward.*x + up.*z) + left.*y
// + position.*, where the shared point transform sums (up.*z + left.*y) + forward.*x, and an
// inline expression written in retail's order reproduces retail's point-transform instructions
// one for one (only the matrix base register and the frame offsets differ); retail's direction
// rows are (forward.*x + left.*y) + up.*z for the i and k rows but (up.*z + forward.*x) +
// left.*y for the j row, an order no shared helper has. The score does not move with either
// order (203 in all three combinations tried), because retail's frame (0x2c against our 0x24)
// and its source receiver (ECX, copied to ESI at entry, against our EDX) misalign the whole
// listing: 203 of the 210 instruction pairs differ whatever the body is. Retail's source
// arrives in ECX and its matrix is the first stack argument, so retail declares 0x245540 as
// (index, matrix, source, height, radius, a5, a6, collection); ours is (index, source, matrix,
// ...), and that declaration and its call live in src/unknown_245850.cpp. Retail's 0x244de0
// takes the direction vector in EAX and the transformed point in ECX, ours takes the point in
// EAX and the vector in EDX; per the parameter-order rule neither is changed.
// Earlier: Frame 0x24 versus 0x2c, source receiver and independent side homes remain;
// frame 0x24 versus 0x2c. Retail receives the source in ecx and keeps it in esi
// until the plane-table read (scheduled after the edge vector), computes both side flags eagerly (one
// spilled to a byte slot, `shr; and bl, 1`) and reads the transform from the stack at its test; ours
// receives the source in edx with no copy, holds the transform in ebp from entry and folds the side
// compare into one xor inside the branch. A separate rotation output vector copied into the direction
// scores 193 but adds 19 bytes (one-use staging copy, not kept).
// frame 0x24 versus 0x2c; the source arrives in edx (retail: ecx, copied
// to esi, which keeps 0x245aa0 at 3); ours holds the transform in ebp from entry where retail
// reads it from the stack at its test and borrows ebp (push/pop) only around the first plane
// code; retail keeps the two material codes and the first side byte in separate stack homes and
// compares the side bytes (`cmp byte`), where ours merges them into `xor bl, dl`; retail gives
// the first failed facing test its own epilogue. Tried: native plane field 204; signed surface
// field 204; combined views 204; native side bitfields 207; indexed vector scaling 205; source/
// surface/plane references 204; vertex table snapshot 204; transform forceinline 204; const
// transform result 204; separate emitter noinline 204; both call boundaries 204; first/second/
// both normal values 230/208/248; first/second/both endpoint values 235/205/244; cross scope/
// late side values 204/204; narrow sides 207; early narrow sides 207; direct narrow codes 239;
// material snapshot 206; shared rotation 204; shared dot 204; both shared products 204;
// snapshot with early sides 209; signed-byte side carriers first/second/both 238/238/204;
// native-long carriers first/second/both 228/228/222; mixed signed-byte/native-long both ways
// 228/228; live first material/matrix/plane table/material table/index 204 each; material plus
// matrix records 208; both table fields 204; records with table fields 204; first/second/both
// live vertex records 204 each; native signed/unsigned low-word code 204/204; current matrix
// scale 207; native code plus scale 207; vertex records plus scale 207; complete first/second/
// both plane values 233/230/262; geometry-scoped first/second/both planes 217/222/255; scoped/
// outer full planes with shared cross 246/252; second material read first 205, with its long
// copy first too 205; a second material record pointer 204; the transform read once through its
// address 226 (still in ebp at entry); the edge vector scaled into a copy and rotated with the
// existing matrix_transform_vector 220; the scaled rotation in an inline helper with its own
// matrix parameter 204; shared in-place direction scale with dot/cross/both 203/203/203/208;
// separate native scaled direction with those products 222 each; two-product rotation sums at
// each row 222; all partial sums before stores in each of the six row orders 222; partial rows
// with shared matrix scale 212; native endpoint-word array alone/with plane-table field/with
// signed material field/with both 204 each; separate native endpoint words with those four
// record views 204 each; the source parameter declared last (definition, declaration and the
// 0x245aa0 call; still edx) 204; the facing returns as `<=`/`>=` tests 204; one ternary
// facing test 210; plane and side bit through a two-call inline helper 204; side bits as
// `(code & 0x8000) != 0` 204.
// Frame 0x24 versus 0x2c, source receiver and independent side homes remain;
// a separate rotation output vector copied into the direction scores 193 but adds 19 bytes (one-use
// staging copy, not kept).

// Round 74: plane table declared as a source field and read at each use 204; plane codes declared
// before the edge vector 203; source parameter declared last (definition, declaration and the one
// call) 204, still edx; material pointer for the final call (as 0x245400) 204; transformed point
// declared with the edge vector 204. Retail's source arrives in ecx and is kept in esi until the
// plane table read, which it schedules after the edge vector; ours reads every source field first.
// Native plane field 204; signed surface field 204; combined views 204; native side bitfields
// 207; indexed vector scaling 205; source reference 204; surface reference 204; vertex table
// snapshot 204; plane references 204; transform forceinline 204; const transform result 204;
// separate emitter noinline 204; both call boundaries 204; first/second/both normal values
// 230/208/248; first/second/both endpoint values 235/205/244; cross scope/late side values
// 204/204; narrow sides 207; early narrow sides 207; direct narrow codes 239; material snapshot
// 206; shared rotation 204; shared dot 204; both shared products 204; snapshot with early sides
// 209; signed-byte side carriers first/second/both 238/238/204; native-long carriers
// first/second/both 228/228/222; mixed signed-byte/native-long both ways 228/228; live first
// material/matrix/plane table/material table/index 204 each; material plus matrix records 208;
// both table fields 204; records with table fields 204; first/second/both live vertex records
// 204 each; native signed/unsigned low-word code 204/204; current matrix scale 207; native code
// plus scale 207; vertex records plus scale 207. Round 72: second material read first 205, with
// its long copy first too 205; a second material record pointer 204; the transform read once
// through its address 226 (still loaded into ebp at entry); the edge vector scaled into a copy
// and rotated with the existing matrix_transform_vector 220; the scaled rotation in an inline
// helper with its own matrix parameter 204.
// Complete first/second/both plane values 233/230/262; geometry-scoped
// first/second/both planes 217/222/255; scoped/outer full planes with shared cross
// 246/252.
// Round 130: shared in-place direction scale, with dot/cross/both, 203/203/203/208;
// separate native scaled direction with those products 222/222/222/222.
// One-point reduction excluded; alternate dot association/grouped values restored.
// Round 327: corresponding two-product rotation sums at each row 222; all partial sums before
// stores in ijk/ikj/jik/jki/kij/kji order 222 each; partial rows with shared matrix scale 212.
// Round 207: native endpoint-word array alone/with plane-table field/with signed material field/
// with both 204 each; separate native endpoint words with those same four record views 204 each.
// Native scaled-vector copy with direct/separate rotation output 220/251;
// native aggregate input with direct/separate output 204/193 (grouping rejected);
// point workspace placement ties each. Original scalar inputs with separate native
// rotation output copied before/after point transform 193/211, both declaration
// scopes; direct final output with initial copy/branch results 193/229.
// Round 211: complete native surface value, mutable/const, with primary-table-first-second /
// primary-table-second-first / table-primary-first-second / table-second-primary-first publication
// orders: 211/211/211/209 each; 789/789/789/787 bytes. Original record, reads and body restored.
// Round 62: second material read first 205; side flags before the plane pointers 204; second material
// record pointer read first 207; source parameter first (definition, declaration and the one call)
// 204, still edx; the side comparison named before the plane test 204. The `/Gd` first pass keeps the
// source in esi and loads the transform at its test (as retail); only the LTCG pass differs.
// Round 225: uniform primary-index/surface/both observations 208/203/208;
// each with the material table read at every use 217/213/209; complete at-use
// graph with signed/unsigned native low-word fields 209/209. Surface-only
// grows four bytes; no qualifying gain. Original receivers and observations restored.
// Round 64: second surface's plane read first 205 (repeats round 62); both surfaces through record
// pointers 204; early return on equal planes 204; facing arms swapped 204; plane codes as dword
// (shr side bits, still fused into one xor at the compare) 204; the shared cross3f for the plane
// cross product 204 (kept: it was that helper's body by hand); dot3f either order 206.
// Lane 9 r23: retail reads the second material's index (+0xa) before forming the surface pointer
// and keeps a dead lea of its material record; the source arrives in ECX (copied to ESI), which
// also costs 0x245aa0 its 3 points. Material table first 205; second material declared first 207;
// second material record pointer 204 (folded, same code); table and second material ahead of the
// first index 205 (+0xa read first, material registers exchanged); that order with the first
// material declared ahead and assigned later 205 (same code).
// Emitter inline / inline-noinline 204/204; with early narrow sides 207; uniform
// material-table / primary-index / both observations 207/208/217; native material
// low word 204; branch-owned planes 223. Original emitter declaration and body restored.
// Round 16: outer transformed-point workspace 204; native material codes without their later copies 211;
// second-first 210; first/second alone 211/210; native codes with inner workspace 211; one matrix-address
// read on that graph 235; signed-byte side carriers on it 211. Native codes move the matrix to ECX, not
// the source, and give frame 0x28. Original body restored.
// Lane 4 r106 (fast): ours loads the transform into ebp at entry and packs the plane table into its dead
// stack slot (frame 0x24); retail reads it at its test. The edge vector's transform through the existing
// vector3f_scale and matrix_transform_vector 203 (kept: it was those helpers' bodies by hand); uniform rows
// 204; a vector sibling of 0x245400's point transform, either row order 204/204; on 203, second material
// read first 206.
// Round 207: complete mutable/const source values before/after surface-index acquisition
// 218/218/225/225; with reused complete first-material value 220/220/224/224.
// Native source plane-pointer declaration included throughout; all restored.
// Frame 0x24 versus 0x2c, source receiver, matrix and side homes remain.
// Coupled source-index/material-side/geometry lexical identities, all seven subsets and complete
// endpoint/transform identities: 203/203/203/203/203/203/203/203. Source receiver, side homes and frame gap
// remain
// original identities restored.
// Round 212: native material-index pair traversal, long/short/dword/word counters, narrow/full
// decoded pairs [222, 233, 222, 233, 235, 229, 235, 229]; exact-match-only grouped storage
// restored. Frame, source receiver and side homes remain.
// Round 246: first/second/both complementary bool sides 207/207/222; both complementary
// byte/short/long sides 222/222/222; first/second complementary short with the other original bool
// 207/207. Original producers, equality and all body operations restored.
// Full native transform values, mutable/const with native plane field and point workspace
// subsets: 223 each; source enters ECX but frame grows to 0x58 and length 796. Original restored.
// Nonnull source-reference formal in original/first positions 203/203; each with native plane
// ordinals 222, uniform material-table reads 210, both 231. Sole caller 3/3/107/107 in each order.
// Original pointer definition, declaration and call restored; source/frame/side homes remain.
// Round 380: two-use masked plane ordinals as word/short, original/signed-code
// side tests and production after/before the edge: 222/222/203/203/207/207/209/209.
// Original full-width masked expressions and complete executable body restored.
// Round 382: signed facing-projection result, conditional/if-else producers with
// bool/byte/word/long side carriers 204/204/204/204/204/204/224/224.
// Original independent ordered facing tests and full executable body restored.
// Round 223: Complete orientation eligibility in bool/byte/short/long storage, exhaustive/default-true producers 202/202/202/202/202/202/202/202. Exact-match-only result forms restored; original early returns and in-place transforms retained.
// Round 72: source-first parameter order as 0x245400 declares it (with the 0x245aa0 call) 203, still
// EDX; second-material record pointer 203; transformed-point call in the transform arm 246 (the
// transform becomes a register argument, 0x245aa0 46); widened codes before the edge 203; side
// bytes before the plane pointers 203.
// Round 175: native const source member, original/native plane field and bool/byte sides
// 203/203/203/203; each with uniform material-table reads 210/210/210/210.
// Original free-function interface, source declaration and every call restored.
// Complete first/second/both native vertex inputs 245/215/270;
// with a complete surface 287/272/318; both with material/surface+material 261/304.
// Original endpoint receivers, source records and complete body restored.
// Round 264: existing difference/cross/rotation force-inline declaration subsets,
// all seven 203; all with the actual point-transform boundary 203. Original
// declarations and body restored; frame 0x24/0x2c, source and side homes remain.
// DH round 236: true cross output at function entry with independent direction, transformed
// point and both side-byte entry storage, all eight combinations 203 each. Initializations and
// input reads stayed fixed. Original declarations/body restored; frame and source/side homes remain.
// Round 398: signed-code side producers with original masks and independent facing exits,
// bool/byte/word/long 223/223/219/220; direct native material words on those graphs
// 222/222/216/217. Complete original body and side producers restored.
// Round 285: existing staged cross with original/first/second/both direct plane receivers,
// then the same four with entry cross-output storage: 203/209/209/231/203/209/209/231 fast.
// Original complete body restored; frame, source receiver and side homes remain.
// Round 13 (lane 14): second code and plane table ahead of the first code 203; plane table first
// 203; early return on equal materials 203; point transform before the vector transform 203; edge
// vector after the side flags 202 (not kept). Source still arrives in EDX.
// Round 203: destructive plane-code decoding, paired/sequential long 204/206, dword 203/205,
// short 206/206, word 206/206; the independent side spill reuses the matrix argument home.
// Actual marked point transform, return/native output 405/405; scaled-vector/both calls 386/582;
// each with a force-inline declaration 405/405/386/582. Original body and declarations restored.
// Round 441: Native point ownership, complete result with copy/setter after rejection 209/219, beside edge 206/216; transform-only in-place point after direction 220/205, before direction 211/208. All exact-only forms restored; original frame/source/side homes remain.
// Round 97 (lane 7): retail's (index, matrix, source, ...) parameter order at the definition,
// declaration and the 0x245aa0 call 203 (source still EDX, matrix still EBP from entry). The source
// arrives in ECX whenever its last use comes before the facing block (the plane table read at the top
// 209; at the top of the material-test block 209), in EDX whenever the plane table is read after the
// edge vector. Plane table at the block top with the right-surface index and code read first 208;
// the matrix read through its address at its test 208 (still EBP from entry); right-surface index and
// code read first alone 206. Retail's borrowed-EBP region and plane table in EBX appear only when the
// matrix is read from its stack home at the test.
// Round 41, with 0x245400's (source, index, matrix, ...) order at the definition, declaration and call:
// plane table read at function entry / at the top of the material-test block / just after the materials
// pointer 209/209/209 (source in ECX, 0x245aa0 MATCH each; the block-top read kept); just before the edge
// vector 203 (EDX, 0x245aa0 3). (index, matrix, source, ...) with the entry read 209 (0x245aa0 16, the
// element loaded through EDX); the old order with the entry read 209 (0x245aa0 16).
// Round 110 (lane 8, fast; the reduced image holds 2 sources): the facing test with the side test
// inverted (`!=` arm first) 209; sides as `(code & 0x8000) != 0` 209; the two sides in a two-element
// array 207; sides computed inside the plane-index test 209; a named `same_side` comparison 209 (all
// compile alike: our build sinks both side bits into one xor, retail computes them eagerly into a
// stack byte and BL and compares bytes).
// Round 259: complete first/second/both material-code reads at every use, with captured/live
// table, 237/208/209/208/207/222; both code observations with native signed low-word
// material field and captured/live table 209/222. Two-point best rejected; original restored.
// Round 53 (lane 10, fast): ours keeps the material table in EBX for the whole body and the matrix in EBP
// from entry; retail spills the table (+0x18) and the first material index (+0x1c) and uses EBX for the
// second code, the plane table and the side bit. The final call's fields read through `arg_1->field_2c` 205
// (0x245aa0 107); the first material index as `short` 207; the code test as `second != first` 209 (same
// code); the second code read first with that test 208; a first-material record pointer for the final call 209.
// Round 43 (lane 14, fast): plane table read after the edge vector 203 / after the long code copies 203 /
// that with the side bits before the plane pointers 203 (all three move the source back to EDX, 0x245aa0 3);
// side bits before the plane pointers alone 209; second material through a record pointer 209; the emit
// call's material fields read through the source again 205 (0x245aa0 107).
// @retail 0x245540
void function_245540(s_source_245400 const *arg_1, long arg_0,
	transform4x3f const *arg_2, real arg_3, real arg_4,
	long arg_5, long arg_6, s_collection_245270 *arg_7)
{
	s_surface_245400 const *local_0 = &arg_1->field_34[arg_0];
	long local_1 = local_0->field_08;
	s_material_245400 const *materials = arg_1->field_2c;
	short first_material = (short)materials[local_1].field_00;
	short second_material = (short)materials[*(short *)local_0->field_0a].field_00;
	if (first_material != second_material)
	{
		plane3f const *local_7 = *(plane3f const *const *)((byte const *)arg_1 + 0xc);
		point3f const *local_4 = &arg_1->field_3c[((word const *)local_0)[0]].field_00;
		point3f const *local_5 = &arg_1->field_3c[((word const *)local_0)[1]].field_00;
		vector3f local_6;
		vector3d_from_points3d(local_4, local_5, &local_6);
		long local_2 = first_material;
		long local_3 = second_material;
		plane3f const *local_8 = &local_7[local_2 & 0x7fff];
		plane3f const *local_9 = &local_7[local_3 & 0x7fff];
		bool local_10 = (bool)((local_2 >> 15) & 1);
		bool local_11 = (bool)((local_3 >> 15) & 1);
		if ((local_2 & 0x7fff) != (local_3 & 0x7fff))
		{
			vector3f local_12;
			cross3f(&local_8->n, &local_9->n, &local_12);
			real local_13 = local_12.k * local_6.k + local_12.j * local_6.j + local_12.i * local_6.i;
			if (local_10 == local_11)
			{
				if (!(local_13 > -0.0001f))
					return;
			}
			else if (!(local_13 < 0.0001f))
				return;
		}
		point3f local_14;
		if (arg_2)
		{
			if (arg_2->scale != 1.f)
				vector3f_scale(&local_6, arg_2->scale, &local_6);
			matrix_transform_vector(arg_2, &local_6, &local_6);
			local_4 = function_245400_transform(arg_2, local_4, &local_14);
		}
		function_244de0(local_4, &local_6, arg_3, arg_4, arg_5, arg_6, local_1,
			materials[local_1].field_04, materials[local_1].field_05,
			materials[local_1].field_06, arg_7);
	}
}
