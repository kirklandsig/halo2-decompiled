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

/* the direction turned a quarter turn in the horizontal plane */
PRIVATE __forceinline void function_244de0_perpendicular(vector3f const *direction, vector2f *perpendicular)
{
	perpendicular->i = -direction->j;
	perpendicular->j = direction->i;
}

/* records an edge and, given a height, the edge lowered by it and a prism for each side of
   the strip between the two */
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
		vector2f local_1;
		function_244de0_perpendicular(arg_1, &local_1);
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
	short field_00;
	short field_02;
	byte field_04;
	byte field_05;
	short field_06;
};

struct s_surface_245400
{
	word field_00;
	word field_02;
	byte field_04[4];
	short field_08;
	short field_0a;
};

struct s_vertex_245400
{
	point3f field_00;
	word field_0c;
	word field_0e;
};

struct s_source_245400
{
	byte field_00[0xc];
	plane3f *field_0c;
	byte field_10[0x1c];
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
// score 163 (was 163): The frame remains 0x24 against retail's 0x2c. The source receiver, material code spills
// and independently decoded side homes differ.
// MiniMax B (11-20): no new form, no lower score (164): our frame is 0x24 against retail's 0x2c, retail homes the two decoded material codes as dwords and the first index as a dword where this build keeps two shorts, and retail materialises each side bit as its own stack byte where ours merges the two tests; retail also receives the source in ECX and copies it to ESI, which the declaration in the caller's file decides.
// Earlier score 164 (was 206; 206 before lane 13 r112's single-condition facing test at the end of this note, 214 before MiniMax C 11-17, earlier noted 209 (was 209), 214 since B6G's j-row fix; m3 lane 16 r130 and r132 at the end): the frame is 0x24 against retail's 0x2c; the source now arrives in ECX as in
// retail's 0x2c; the source now arrives in ECX as in retail (earlier note: 206, was 206).
// Earlier 214 before MiniMax C 11-17, earlier noted 209 (was 209), 214 since B6G's j-row fix;
// m3 lane 16 r130 and r132 at the end.
// MiniMax I (11-16): score 206 (was 214): the second material code is read before the first, the
// Earlier note (206, was 214): The frame is 0x24 against retail's 0x2c and the source arrives in ECX.
// Earlier scores were 209 and 214 after the j-row correction.
// MiniMax I (11-16): the second material code is read before the first, the
// matrix is taken in a named local inside the transform block, and the side bits are extracted with
// an unsigned shift so retail's shr/and survives instead of our sar/xor fold; still differs: the
// frame is 0x24 against retail's 0x2c, retail keeps the source in ESI and reloads the matrix into
// EDX at the use point. The permuter (300s) only found 205, by swapping the third and fifth
// arguments of the call to 0x244de0 (retail's push order differs because retail's declaration of
// 0x245540 has its matrix and source the other way round); a one point gain, dropped. Declaring the
// plane table after the point difference scores 207 again and a mask-test side bit scores 208.
// MiniMax G (11-05): no new form, no lower score; declaring the plane table after the point
// difference scores 203 in the reduced and in the full build (a gain of six, below the keep
// threshold, so the body is restored); the source receiver, the frame and the stack homes still
// differ: BLOCKED-BY the caller 0x245aa0 (src/unknown_245850.cpp).
// MiniMax B (11-04b): no new form, no lower score; retail still copies the source into esi at the entry and frames 0x2c against our 0x24, and its declaration and call live in src/unknown_245850.cpp, which this lane may not edit.
// MiniMax S (11-06): no new form, no lower score: reading the second material index before the first only gained one instruction (209 to 208, below the keep threshold) and was restored; retail still reads the two element material indices in the other order, keeps the source in ESI and frames 0x2c against our 0x24.
// The plane table read at the top of the material-test block and the source-first order match
// the caller 0x245aa0. The register homes, the plane table's home and the side bytes still differ.
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
//
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
// Round 288: existing axis-accumulator rotation with staged cross, native side-byte and
// two-use plane-ordinal subsets 253/254/253/254/256/256/256/256; no match.
// Original in-place rotation and complete executable body restored.
// Round 53 (lane 10, fast): ours keeps the material table in EBX for the whole body and the matrix in EBP
// from entry; retail spills the table (+0x18) and the first material index (+0x1c) and uses EBX for the
// second code, the plane table and the side bit. The final call's fields read through `arg_1->field_2c` 205
// (0x245aa0 107); the first material index as `short` 207; the code test as `second != first` 209 (same
// code); the second code read first with that test 208; a first-material record pointer for the final call 209.
// Round 43 (lane 14, fast): plane table read after the edge vector 203 / after the long code copies 203 /
// that with the side bits before the plane pointers 203 (all three move the source back to EDX, 0x245aa0 3);
// side bits before the plane pointers alone 209; second material through a record pointer 209; the emit
// call's material fields read through the source again 205 (0x245aa0 107).
// Round 65 (lane 16, fast): 0x244de0 now takes the point in ECX and the vector in EAX as retail (its first
// difference is at +0x141), so the call setup is not the cause. The side bools named once more as
// `bool same_side = local_10 == local_11;` before the plane test 209 (identical); as one named comparison of
// the shifted codes 225 (767 bytes, frame 0x28: the matrix is no longer held in EBP from entry and EBP takes the
// second code, but the sides still fold into the branch); that with the second material read first 225; that
// with the plane table read after the edge vector 227 (and 0x245aa0 113: the source then arrives in EDX); the
// facing dot through dot3f 237 (755 bytes).
// Round 493: all three facing projection parts in ijk/ikj/jik/jki/kij/kji order 208/208/208/208/208/208;
// sequential accumulation with complete parts in kji/ijk order 209/209. Original expression
// restored; frame 0x24/0x2c, side bytes and delayed matrix load remain.
// Round 260: existing native 3x3 rotation with staged cross/shared dot/unsigned decoded words,
// 253/254/236/237/253/254/236/237; longer bodies and frame 0x24 remain, all restored.
// Round 263: Joint paired-mask/xor side equality, signed/unsigned material words and existing staged-cross combinations
// 239/239/211/211/239/239/211/211. Original side producers, material carriers, cross call and complete body restored.
// DH round 265: complementary facing rejection with bool/byte/word/long sides, original/staged cross:
// 211/211/211/211/211/211/211/211. Original complete body restored.
// Round 288: existing axis-accumulator rotation with staged cross, native side-byte and
// two-use plane-ordinal subsets 253/254/253/254/256/256/256/256; no match.
// Original in-place rotation and complete executable body restored.
// Round 337: target inline contract with late plane-table, native signed-word sides and
// second-code-first subsets 209/203/224/226/208/206/223/224. Six-point best fails the retention
// margin; original body and contract restored. Frame and independent side homes remain.
// Round 340: existing point helper's immediate accumulations, complete product
// parts, deferred accumulated stores and paired axial sums, each with early/late plane table,
// 209/203/209/203/209/203/209/203. The matched helper user stays exact; late planes lose
// the caller and gain only six. Entire helper/body restored; frame 0x24/0x2c and side homes remain.
// Round 353: Native surface endpoints, secondary signed index and material low-word
// declarations on the source-ECX graph, seven subsets 209 each; all with secondary-code-first
// reads 208 (one-point gain rejected). Original data declarations and complete body restored;
// frame 0x24/0x2c, matrix/table homes and independent side bytes remain.
// Round 313: Existing point helper with plain component output loop, complete product parts
// and coordinate-before-translation subsets 209/209/209/209; one scale observation
// 213/213/213/213, also raising matched helper user 0x245400 to 44. Complete helper
// and body restored; frame 0x24/0x2c, matrix lifetime and independent side homes remain.
// Round 467: Existing point helper consumed by explicit destination/final conditional selection,
// phase/entry point storage and point production after/before direction: 209/211/209/211/210/209/210/209.
// Frame and independent side homes remain; original helper and complete body restored.
// Round 355: native paired index/side fields at the actual material record, complete
// first/second material values and existing staged-cross subsets: 210/231/228/229/210/231/228/229.
// Original record and body restored; frame/table/matrix and independent side homes remain.
// Round 59 (m3 lane 10, fast): the matrix in EBP from entry is what keeps retail's shape away. The matrix block under one
// more folded test (the material test again) 209, under two 404 (588 bytes: the two transforms go out of line and EBP goes to
// the material table); force-inlined transforms at no / two / three / four extra tests 209 each (still EBP). Diagnostic, removed
// at once: a volatile read of the matrix at its test 209 (EBX plane table, borrowed EBP, later saves as retail, frame still
// 0x24, sides still folded, so a volatile cast alone would not match and is not allowed). A local copy at the block 213, a read through its
// address 213 (both still EBP from entry). The rotation through a `matrix3x3` local 208, axis pointer locals 208, axis pointers
// as a helper's parameters 208 (one instruction, not kept); a scale local 209; `arg_2 != NULL` 209.
// Round 366: Input-only material/surface/vertex table element declarations, seven
// subsets, then all with native plane metadata: 209/209/209/209/209/209/209/209.
// Original types and body restored; frame 0x24/0x2c, matrix and side homes remain.
// DH306: native plane-receiver identity with bool/byte/short/long sides, original/staged
// cross, 235 each (756 bytes); caller 0x245aa0 becomes 43. NOT kept: retail compares decoded
// ordinals, shorter extent and caller loss. Original body restored; frame and side homes remain.
// Round 390: Complete material-to-facing admission outcome, bool/byte/short/long,
// branch/conditional facing producers: 201/201/231/230/201/201/231/230.
// NOT kept: state set in a test needs a match; every extent grows away from 783.
// Original body restored; frame 0x24/0x2c, material/side homes and late matrix remain.
// M3 lane 4 r157 (fast): the scale test as `1.f != scale` 209; the transformed point declared in the transform block 209
// (and it would dangle); the material table read before the surface's first index 209; the edge vector after the
// side flags 203 (786 bytes, re-measured; 6 points, under the margin), with the second material read first 207, with the
// sides before the plane pointers 203; the endpoint pointers and edge vector both after the sides 212 (aligned 171 -> 147).
// Round 139 (m3 lane 6, fast): the plane table read after the edge vector, as retail schedules it, 203 (and
// after the two point pointers 203), but the source then arrives in EDX and 0x245aa0 drops to 3, so not kept;
// the existing cross3f_staged 209; the side flags before the plane pointers 209; the dot as i, j, k 209.
// Round 129 (m3 lane 8, fast; 0x245aa0 MATCH each time): retail decodes the side bits with `shr` (unsigned): the
// codes as dword 209 (gives retail's shr, the side bits still sink into the branch); with an inline plane decoder
// writing the side through a pointer (diagnostic) 209; dword codes with the second material read first 208; dword
// codes with the edge vector after the side flags 203 (786 bytes); that with the second material first 207 (784,
// frame 0x28, source still ECX, matrix still EBP from entry). Original body restored.
// Round 390: Complete material-to-facing admission outcome, bool/byte/short/long,
// branch/conditional facing producers: 201/201/231/230/201/201/231/230.
// NOT kept: state set in a test needs a match; every extent grows away from 783.
// Original body restored; frame 0x24/0x2c, material/side homes and late matrix remain.
// Round 59 (m3 lane 13, fast; 0x245400, 0x245aa0 and 0x245850 checked): retail keeps the edge vector in XMM3-5 and
// scales a copy into XMM0-2 (a select), holds the matrix in EDX only inside the transform block, and reloads both full
// codes from their stack copies for the side bits after forming the plane pointers. The second material bound to a
// `const &` record and read first 208 (no dead lea); with the plane pointers inside the plane test 222 (767 bytes,
// 0x245aa0 43); with the code test written second-first 208; with the code copies and side bits made second-first 207;
// that with the plane table read after the edge vector 204 (0x245aa0 3). Original body restored.
// Round 50 (m3 lane 15, fast; 0x245aa0 checked): the side flags as `(code & 0x8000) != 0` 209 (same code); the
// facing arms inverted on `!=` 209 (same code); the facing test as one ternary condition 211; the equal-material
// case as an early return 209 (same code); short codes with `< 0` sides 224 (762 bytes, 0x245aa0 lost to 6).
// B412: Complete ordered-facing rejection to the sole void exit, bool/byte/short/long
// sides with original/staged cross: 211 each. Original body restored; frame 0x24/0x2c,
// material/side homes and delayed matrix acquisition remain.
// B415: Decoded-plane phase ending before transform, with cross-ending/native-code/side storage
// B415: masks 0..7: 209/209/240/240/209/209/240/240. Native short codes lose caller 0x245aa0 at 6; all original
// B415: code/side homes and full body restored; frame 0x24/0x2c, material/table spills and late matrix remain.
// B417: Plane-table acquisition ends after native plane pointers, with completed endpoint/cross
// regions and earlier bare point workspace, eight forms: 209/209/209/209/209/209/209/209.
// Original body restored; frame 0x24/0x2c, side homes and delayed matrix acquisition remain.
// B425: External first/second material and surface bindings on source-ECX graph,
// seven subsets, then all with first-material pointer: 209/209/209/209/209/209/209/209. NOT kept: no
// exact match; original receivers/body restored, frame and independent side homes remain.
// Round 52 (m3 lane 2, fast; aligned count ignoring stack offsets in brackets, base 209 [267]; lead, 1 point, not kept): the first transform as a scaled copy
// `vector3f scaled = { edge.i, edge.j, edge.k }; if (scale != 1.f) vector3f_scale(&edge, scale, &scaled);` rotated into the edge 208 [218]: retail's copies
// before the test and scale-first products, only the matrix register (EBP from entry, retail EDX at its test) and the first block's homes left. Rows written
// out instead of matrix_transform_vector 208 (same code); whole body in a compiled-out loop 208 (same); dword codes with named plane indices 208 (same);
// second material read first 209 [220]; with dword codes 209.
// F381: Independent complete side producers, exhaustive bool/byte/short/long
// 230/230/231/236; default-false results 202/202/212/223. NOT kept: no
// exact classification residence; seven-point best fails the retention margin.
// Original side tests and body restored; frame/table/matrix and side homes remain.
// B436: Existing point-transform native input/coefficient/output array observations, seven subsets,
// then one reused matrix scale: 209/209/209/209/209/209/209/213. NOT kept: no exact form or eligible gain;
// original complete body and helpers restored. Native frame/register differences remain.
// B439: Existing alias-safe rotation with bool/byte/short/long side carriers,
// branch/entry cross storage: 253/253/253/253/253/253/253/253 (830/783 bytes).
// NOT kept: longer extent; native frame and material/side homes remain. Body restored.
// B441: Actual secondary material-index producer in short/long, at original/early
// acquisition and producer/entry declaration boundaries: 209/209/209/209/209/209/209/209.
// NOT kept: one-use index transport has no equality; original complete body restored.
// Frame 0x24/0x2c, material/table/side homes and delayed matrix acquisition remain.
// MiniMax B (11-04): no new form, no lower score; a named copy of the index and a named
// copy of the source pointer each give 209 at retail's 783 bytes, and retail still copies
// the source into esi and frames 0x2c against our 0x24; the declaration and the call live
// in src/unknown_245850.cpp.
// Round 150 (m3 lane 6, one-file harness with 0x245aa0's file, exact at 209; masked aligned counts in brackets, base [132]): the first
// pass with the plane table read after the edge vector has retail's shape (source in ESI, matrix read at its test); only the second
// pass differs, where the source arrives in EDX with no copy and the matrix goes to EBP from entry. 300 random forms (second material
// first or through a record, code copies long/dword and in either order, named plane indices, side forms, plane table, edge and
// pointer orders, table read at each use): best 202 at 783 bytes [100] (named indices, sides before the pointers, edge last), masked
// best [98] at 787 bytes, and every one of them costs caller 0x245aa0 its match (3), only the original keeps it. All six orders of
// source, index and matrix, each with the table at the block top or late: arrival unchanged (ECX top, EDX late; 209/203, caller
// 0/3 or 16; matrix first 231/227). Named surface, vertex or material locals with the late table 203-204; an inline accessor for the
// second material's record (forceinline, inline, reference; with a named record) 208 or 204 late, no dead lea, no arrival change.
// Plane table and pointers inside the plane test 222 (766 bytes): the source does arrive in ECX and copies to a callee-saved
// register at entry as retail's, but the matrix then becomes a register argument too (no push, caller 43).
// M3 lane 4 r170 (one-file harness with 0x245aa0's file, exact at 209, caller 0 unless noted; masked aligned in brackets, base [77]):
// the matrix named inside its block with a true `__assume(matrix != NULL)` 205 (also --fast; aligned 162 -> 134 [73]: the matrix
// leaves EBP, the source stays ECX, caller matched; a renaming pointer, so not kept); the three transforms in a plain `inline`
// helper 592 (401 B, out of line), force-inlined 209, with the plane table late 203 (caller 3); a volatile matrix read (diagnostic,
// removed) 209, with the late table 214 (caller 3). On the named matrix: second material read first 205 [70], through `short const &`
// 237 (752 B) [67], through a word reference, a record reference or a record pointer 223 (766 B) [64] (the lea comes ahead of the
// load, retail's after), the second index named with the reference 220 (764 B); a vertex-table local 235 (750 B), with a surface
// local 220 (764 B); `__assume(arg_1 != NULL)` 235 (750 B); the source copied to a named local with an assume 209-214, without
// 208-223 (never retail's `mov esi, ecx`); dword codes with named plane indices 221 (765 B), sides as `!= 0` 207 (aligned 128).
// Round 132 (m3 lane 7, fast; 0x245aa0 and 0x245400 checked): the second material through a record
// pointer (retail's dead `lea edx, [ecx + edx*8]`), the first through one also at the final call, both:
// 209 each (no dead lea, callers keep their matches); the plane table read after the edge vector
// 203 with 0x245aa0 at 3 (the source then arrives in EDX); the scaled edge transform as a scaled copy
// rotated back through a forceinline helper taking the matrix 234 (811 bytes; plain inline stays out
// of line, 387). Retail pushes EBP only around the plane code (`push ebp ... pop ebp` mid-body) and
// reads the matrix at its test; ours keeps the matrix in EBP from entry, so EBP is a prologue save.
// M397: Complete edge/cross/transformed-point defaults, masks 1/2/4/3/5/6/7,
// then all at entry: 213/209/223/213/230/223/230/230. NOT kept: no equality or eligible native-
// frame gain; original workspaces and complete body restored. Four exact controls survive.
// DH381: Existing array edge producer at unit scale, late-plane/independent-side/staged-cross
// subsets 0..7: 208/204/226/227/208/204/226/227. NOT kept: best gains five and loses caller 0x245aa0; original body restored.
// N396: Signed eight-byte primary ordinal/first decoded code/second decoded code,
// seven subsets then complete encoded/decoded residence: 208/281/270/278/279/263/294/298.
// NOT kept: native narrow domains, no qualifying gain and longer decoded forms; original body restored.
// Frame 0x24/0x2c, independent side homes and delayed matrix acquisition remain.
// B6G (11-06, walk of the whole body against retail 0x245540-0x24584c, scores from the fast build): behaviour difference found and fixed.
// The edge is rotated by matrix_transform_vector, which sums every row forward + left + up; retail sums the j row (up + forward) + left
// (0x2456ee movss up.j, 0x2456f3 mulss z, 0x2456fa mulss forward.j, 0x2456ff addss, 0x245703/0x245708/0x24570c left.j*y added last) and the i/k rows
// forward + left + up (0x2456c5-0x2456e8, 0x245716-0x24574d), so ours rounded the j component differently. The j row is now written out
// in retail's grouping (rows i and k written out beside it, since no shared helper has the j order): 214 (helper 209, same bytes, 783).
// Expression-level spellings of the j row (`up*z + forward*x + left*y` in one statement, 209) are regrouped by the compiler; only a split
// sum keeps the grouping. Everything else matches retail: the side tests (0x245661/0x24566a ja, 0x245680 jbe) and the +-0.0001 constants
// (0x45ddfc, 0x45dbdc), cross(first, second) with k + j + i dot (0x245649-0x245661), scale != 1 test (0x24569e, 0x245751), point rotation
// order (forward*x + up*z) + left*y + position, the call arguments (0x24580c-0x245841). Struct fields are now typed as retail reads them
// (word material codes at 0x245558/0x245568, short surface indices at 0x24554d/0x24555d, plane table at source+0xc): the casts go, same bytes.
// No new include, helper or double; callers 0x245aa0 and callees 0x245400, 0x244de0 stay MATCH.
// M3 lane 7 r167 (fast; caller 0x245aa0 MATCH each time): dead-local phase scan 1-7 at the top 214 each; the material
// test written second-first 214; the facing tests constant-first 214; the scale test as `1.f != arg_2->scale` 213;
// the side and plane-index compares swapped 214.
// MiniMax I (11-16), NOT kept: 206 (main 214) came from four changes: the arrays' sign bits as (bool)((unsigned
// long)local_2 >> 15) (same values: local_2 is the sign-extended short) 210 alone, plus the first material's loads
// moved after the second's, the sign-bit compare's operands swapped and arg_2 renamed through a matrix alias
// local, which are kept only with a match. The cast alone is a 4-point gain, under the 8-point bar above 30. Body
// back to main.
// P571: Actual edge-scale nonunit/unit decision at its transform consumer, bool/byte/short/long:
// 228/228/228/228/228/228/228/228. NOT kept: all regress and shorten to 763/783 bytes without native
// frame/table/side-home recovery. Original scale comparison, row arithmetic, point transform and complete
// body restored.
// M3 lane 1 r113 (fast with 0x245aa0 and 0x245400, both MATCH; base 214, offsets-masked aligned 222, registers too 112): the
// edge vector computed after the side bits 228 (763 B, masked 104: the matrix leaves EBP and the material table is spilled,
// but EBX/EBP are then saved mid-body); the plane pointer and side bit from a forceinline decoder with a `bool *` out-parameter
// 214 (same code: the out-parameter is promoted); the side bits from the word codes (`(word)code >> 15`) 213; the transformed
// point declared at the top 214 (same code); the side bits declared ahead of the plane pointers 214 (masked 108). Diagnostic,
// reverted at once: a volatile first side bit 208 (frame 0x28; the side bits are then materialised before the plane-index
// compare as in retail, with a local EBX save around the plane code and the matrix read at its test). Retail computes both
// side bits eagerly (byte spill, `cmp byte`); ours folds them into one xor inside the plane test, which is what frees EBX/EBP.
// M3 lane 8 r175 (fast with 0x245270/0x244de0/0x245400/0x245850/0x245aa0, all MATCH; control 214): the edge scaled into a copy
// before the rows (`vector3f edge = local_6; if (scale != 1.f) vector3f_scale(&local_6, scale, &edge);`, as retail copies then
// scales) with the edge_i/j/k scalars 216 (803 B), with the rows reading edge.i/j/k 216 (803 B); the side flags as sign tests
// of the long codes (`local_2 < 0`) 207, of the short codes 207 (same code: `setl` pairs, a seven-point gain from layout, not
// retail's `shr; and bl, 1`, under the keep bar), with named plane indices used for the pointers and the compare 207; the
// side bits from an unsigned shift `((dword)local_2 >> 15) & 1` 214 (`shr`, still fused into `xor al, dl`). Body unchanged.
// m3 lane 16 r130 (fast with 0x245aa0, 0x244de0 and 0x245400, all MATCH; control 214): retail's call in 0x245aa0 sets
// EAX (index) before ECX (source), so the source-first declared order stands. The second material read first, the plane
// table after the edge vector and dword-shifted side bits together 221 (768 B, source then in EDX, 0x245aa0 3); the masked
// plane indices through `long &` references masked in place 224 (767 B, 0x245aa0 6); the transformed point picked by a
// ternary after the rotation block as 0x245400 writes it 211 (a one-use local); the edge vector from the vertex
// records read in its call, the start point named after it 218 (793 B, 0x245aa0 100); both material codes through an
// inline accessor taking the record by const reference 214 (same code). Re-measured: a block-local matrix copy 207
// (was 213); dword codes 214; dword codes with byte sides 214; cross3f_staged 214 (was 209); the final call's material
// fields read through the source 223 (0x245aa0 107).
// M3 lane 9 r131 (fast with 0x245aa0; base 214): the plane table read after the edge vector, re-measured, 220 (768 B, and
// 0x245aa0 then 3); the side bits as `byte` 214 (same code); the matrix read once through its own address at its test
// (`transform4x3f const *const *ref = &arg_2;`) 229 (763 B: the matrix leaves EBP but the body shortens), with the plane
// table after the edge vector 225 (762 B, 0x245aa0 3), with unsigned-shift side bits 229 (763 B). Retail still computes
// both side bits ahead of the plane-index compare (byte slot plus BL) where ours folds them into one xor in the branch.
// Also the side bits shifted from the short material codes (`(first_material >> 15) & 1`) 213 (positional noise only).
// MiniMax C (11-17): the two material codes are now read in retail's order (the surface's second
// code at field_0a first, then the one at field_08) and the two side bits are sign tests of the sign-extended codes.
// Left: retail's 0x2c frame with the source copied to ESI, its matrix read from its stack slot at the test instead of
// kept in EBP, its shr/and side bytes and the cross-product register order.
// m3 lane 16 r132 (fast with 0x245aa0 and 0x245400, both MATCH; base 206): the edge copied into an aggregate `scaled`,
// scaled into it with vector3f_scale and rotated from it 236 (814 B: the copy goes through memory); that with a
// block-local matrix name 216 (810 B), with unsigned-shift side bits 223 (806 B), with both 226 (808 B); the edge copied
// into edge_i/j/k before the scale test and scaled as `scale * edge_i` in the arm (vector3f_scale written out) 205 (783 B:
// retail's select, `movaps` copies before the `jnp` and scale-first products, but the i/k rows then sum forward + up +
// left; a one-point gain from a helper body written out, not kept); that with the i/k rows accumulated in two statements
// 207, with a block-local matrix name 205 (still EBP from entry), with unsigned-shift side bits 212.
// Round 456: plane codes masked in place after sign tests 209, plane codes masked in place after shifted
// side tests 212, sign and mask completed one plane at a time 209, plane addresses formed in facing
// guard after in-place decoding 231, rotation rows in existing vector setter 206, named rotation results
// in existing vector setter 206, rotation rows in native vector initializer 200 (NOT kept: six-point
// gain and 804 bytes against retail 783), vector setter with direction scale read once 206.
// m3 lane 5 r196 (fast with 0x245aa0/0x245400/0x244de0/0x245270, MATCH unless noted; control 206). A compile-only copy of
// the file (no /GL) folds the side compare into the branch for every side spelling: `(x >> 15) & 1` as bool, long, byte or
// char, the unsigned shift, `& 0x8000`, the short codes shifted, `(word)code >> 15`, ternaries, a named same-side bool, dead
// `= false` initialisers, and the codes decoded through a local overlay of 15/1-bit fields. Only flags preset false and set
// by `if (code & 0x8000)` / `if (code < 0)` keep them as values (a flag form, match-only): then the LTCG build gets retail's
// structure, the matrix read from its home at its test (no EBP from entry), EBP pushed and popped around the plane code,
// the first side in a stack byte compared with the second (`cmp byte`): 207 (784 B, frame 0x28), the `< 0` spelling 213
// (787 B); on the `& 0x8000` form: cross3f_staged for the plane cross 207, the edge copied before the scale test and scaled
// into the copy with vector3f_scale 244 (820 B), the plane table read after the edge vector 208 (0x245aa0 3). Left on the
// flag form: retail's branch-free `shr; and bl, 1` sides, the plane cross's operand order, the scale select (retail copies
// the edge before the `jnp`, scale-first products), frame 0x28 against 0x2c. Body unchanged.
// M3 lane 1 r119 (fast with 0x245aa0/0x245400/0x244de0/0x245270, MATCH unless noted; control 206, aligned 244, register
// and slot masked 134; positional counts stay near 206 because the frame and length move, so the masked count is given
// too). Three causes read from retail's head, each reproduced: (1) retail's dead `lea edx, [ecx + edx*8]` after the second
// code's load is a pointer to the second material record that is read again in the block (the second read becomes
// `movsx edx, bx`); (2) the plane table is read after the codes, which keeps the source live past the first vertex
// pointer, so it leaves ECX for ESI (`mov esi, ecx`); (3) each side bit is read twice. Forms: the codes masked in place
// after shifted sides, plane pointers first 211, sides first 212, one plane at a time 228 (762 B), long codes with a
// dword shift 211, with sign tests 206 (same code), second code first 210 (frame 0x24 each, no local EBP save); the
// side test as two tests (`same && !(dot > -eps)`, then `differ && !(dot < eps)`) with sign sides 214 (masked 93: the
// sides are then values compared before the cross, EBP is pushed and popped around the plane code and the matrix is
// read from its home at its test), the same as two blocks 214 (same code), with shifted dword sides 232 (752 B,
// masked 104: retail's `shr; and` pairs and `cmp` of the two side bytes), as a conditional expression 224 (766 B),
// shifted sides alone 211. The second material through a pointer with both codes read again in the block and the two
// tests: dword codes 234 (754 B, masked 100), with the else form 211, long codes with sign sides 211 (masked 87), both
// materials through pointers 234 (masked 107), both read again by index 232 (752 B), by index with the else form 214.
// The pointer declared ahead of the first index, the codes read again, the plane table read after them and the two
// tests with shifted sides 217 (757 B, masked 91): retail's first 19 instructions then agree but for two slot offsets
// (`push ebx; push esi; mov esi, ecx` through the `cmp di, bx`, with the dead lea); the same with the table read first
// 234 (754 B), with the else form 208 (0x245aa0 lost), main's head with the table read late 207 (0x245aa0 lost), main's
// head with the table late, the two tests and shifted sides 225 (754 B). On the 217 form, the edge vector computed
// after the sides 220 (760 B, frame 0x28, masked 90), with the first code widened first 216 (761 B, masked 89), both
// vertex pointers and the edge after the sides 209 (783 B, frame 0x2c but no local EBP save, masked 133), the first
// vertex pointer early and the second with the edge late 210 (frame 0x30, 0x245aa0 lost), the edge after the plane
// pointers 230 (0x245aa0 lost), after the codes 217 (same code). Diagnostics, restored at once: the two codes in a
// two-element array 217 (the array is promoted: same shape, frame 0x20), with the else form 208, with the edge late
// 216; the 217 form with the scale select written out (the edge copied to three scalars, scaled as `scale * edge`
// in the arm) 212 (761 B, aligned 173, masked 75), with the edge late 214 (764 B, frame 0x28, masked 74); the select
// alone on main's body 205 re-measured. Nothing kept: no form lowers the count. Left on the 217 form: retail keeps
// both codes in stack homes (stored after the widening, masked in the register, reloaded for the shift), the first
// side in a stack byte and EAX unused through the plane code (frame 0x2c against 0x24 to 0x28), the edge in XMM3 to
// XMM5, the cross's operand order, the same-side return with its own epilogue, and the rows.
// Lane 13 r112 (fast with 0x245aa0/0x245400/0x244de0/0x245270, MATCH unless noted; control 206, aligned 244; aligned
// counts in brackets). KEPT: the facing test as one condition, `if (!((same && facing > -eps) || (differ && facing <
// eps))) return;`, 206 -> 164 (785 B, plain-confirmed, [193]): each side is read twice, so both are values ahead of the
// plane-index compare, EBP is saved around the plane code only, the matrix is read from its home at its test and the
// same-side failure has its own epilogue (`jne; comiss; ja; pop, ret; movss; comiss; jbe`), as retail. On it: named
// plane indices 164 (same code); the dot written i, j, k 164; the edge after the long codes 164; the plane table read
// late 166; the or operands swapped 160 (under the keep bar; retail tests the same-side arm first); the sides ahead of
// the plane pointers 207; the sides from the short codes 213 (one epilogue); shifted sides 232 (752 B); the table
// after the codes 205 (789 B); shifted sides with the table late 207 (767 B); the second code widened first 211, with
// the table late 212.
// On lane 1 r119's head (second record pointer, codes read again, table late; its two-test form [173] 212, 761 B):
// the single condition [147] 212 (760 B), with main's rows 198 (783 B), with the scale select 194 (783 B); main's
// if/else on that head 208; main's declaration order with it 234 (754 B); codes ahead of the points 234 (756 B). The
// edge written as three member differences [152] (two tests) / [147] (single condition): the edge is then in XMM3-5
// as retail's and the dot and the select agree (a helper body written out, match-only). Facing-test forms on that:
// if/else [191]; if/else with braces [191]; an empty then-arm with `else return` [177] (two epilogues); the two tests
// differ-first [154]; as two nested blocks [152]; the negated operands joined by `||` [152]; a forceinline bool helper
// with two returns, with if/else returns, with a ternary, with a result local, a bool local set by if/else and a
// ternary in the test [219] each (same code: retail's return layout, but the sides fold into one xor again); a helper
// with two early `return false` tests [114] 211 (763 B, one epilogue); a helper with two positive returns [147]; two
// positive `faces =` assignments under separate tests [120] 202. Vertex part, same code as [173]/[147] unless noted:
// a vertex-table local; index locals; the second pointer written in the edge call; vertex record pointers; a `point`
// local for the call (declared at the matrix test, after the first pointer [139] 212, after the second [171]); the
// call's point by a ternary after the rotation; the transform in the call's argument; the transform's result unused
// with `matrix ? &transformed : first` in the call; the second pointer reused for the call's point; records with the
// existing edge helper [177] 194 (783 B); the second pointer first [180]; codes ahead of the points [192]; the final
// call written in both arms of the matrix test 267 (832 B, 0x245aa0 22: the matrix becomes a register argument).
// Order forms on the single condition: second code, table, first code [164]; named plane indices [153]; long codes
// with dword-cast shifts [164]; the edge after the sides [145] 204 (763 B, frame 0x28; with the helper [174] 198), with
// the table between the codes [166], after the plane pointers [174], after the codes [147], table first [166]; the
// direction computed again after the test from the points [164]; the rows from the points again [184]; the edge
// computed again inside the plane test, written out / helper / after the cross / on the kept body 204 (794 B) / 194 /
// 181 (783 B) / 163 (791 B): each second computation is merged away and changes nothing in the head.
// Diagnostics, restored at once: retail's EAX holds the second vertex pointer through the plane code. A float compare
// through it after the facing test 239 (812 B, [117]): frame 0x2c, the first vertex pointer formed before its loads
// (`shl ecx, 4; add ecx, edx; subss xmm3, [ecx]`), both long codes stored to homes and the first side in a stack byte,
// as retail, with the pointer in ESI. The pointer stored to a scratch global inside the plane test 168 (783 B, [102]):
// retail's frame, prologue, local `push ebp`, early first pointer and stack offsets, the pointer in EAX; stored ahead
// of the index test 177 (783 B), after the facing test 192 (785 B), ahead of the call 205 (785 B). A null test of it
// ahead of the index test, inside it or after the plane pointers [120] and `__assume(second != NULL)` after the test,
// ahead of the index test or in the matrix block [120], ahead of the call [147]: no change in the head. Left: what
// reads the second vertex pointer inside the plane test with no instruction; then the long codes' register roles
// (retail widens the second code into EDX ahead of the table load and the first into EBP) and the float orders.
// Frame 0x24, early matrix load and combined side handling remain.
// Material table and slot names 206; extended-code names 206; independent side names
// 206; all those names 206. First/second/both signed extensions before the word
// test 205/207/207; extensions at the word acquisitions 207. NOT kept: the early
// first extension gains one at retail length with the frame and matrix load still displaced.
// Lane 13 r114 (fast with 0x245aa0, MATCH; control 164, aligned/masked [134/50]): the j row's forward product
// written edge-first 164 (same code); the edge scale as a select (edge_i/j/k copied, `edge_i = arg_2->scale *
// local_6.i` in the arm; vector3f_scale written out, match-only) 164 [123/47] (retail's select shape, the edge in
// XMM4-6 against XMM3-5); that with the edge as three member differences 211 at 783 B [97/53]; the side bits through
// a forceinline helper taking the long code by const reference 232 (752 B, folds as the shifted sides); select,
// member differences and `(local_2 >> 15) & 1` sides together 237 (746 B). Body unchanged.
// Secondary index before material table 163; secondary record retained for extension 211;
// both codes reread by index 210; table after widened codes 204, with bound vertex records 204;
// late table with shifted orientation bits 199; per-plane decoding 200, with equal-code guard 200.
// NOT kept: the one-point gain leaves frame 0x24 and the side homes displaced. The late
// table with signed sides changes the caller. Retail still keeps the source in ESI.
// M3 lane 1 r126 (fast with 0x245aa0/0x245400/0x244de0/0x245270, MATCH each time; control 164, aligned 134): on the single
// condition, sides as `(code & 0x8000) != 0` 232 (752 B) and as `((dword)code >> 15) & 1` 232 (752 B: the two tests merge
// into one jump). The edge rotation through an inline vector form of this file's point transform (copy the components,
// `x = matrix->scale * x` under the scale test, rows `up * z + left * y + forward * x`, the edge passed in and out) 163 at
// 783 B [124]: retail's select shape (`movaps` copies, `jnp`, the scaled copies) but the rows' sums reorder; its rows as
// forward, left, up 163 [124]; i/k forward-first with j as up, forward, left 163 [124]; j as two statements 164 (785 B).
// Diagnostic, restored: on that helper form, the second vertex pointer stored to a scratch global inside the plane test
// 207 (786 B, [114]): the first vertex pointer is then formed before its loads and the edge lands at retail's offsets.
// Not kept: a new one-caller helper for one point.
// Round 433. The frame, independent decoded material homes and late matrix load still differ.
// decoded plane indices word 224; second material comparison first 164; geometry scoped plane receivers 206;
// geometry scoped plane table and receivers 200; ordered thresholds before sides 201; paired rejected orientations
// 214; cross dot running sum 227; cross dot named products 213.
// NOT kept: reversing the material comparison ties. The scoped plane forms regress and disturb the caller in the
// reduced image. The alternatives keep the frame short of 0x2c.
// Round 436. Plane side by raw-code/ordinal inequality, lower bound, equality and bias
// 207/207/207/212; late table with inequality or lower bound 209/209; with native short
// inputs 207, and late-table complementary order 209. NOT kept: these leave the frame
// at 0x24 or 0x28; the late table also disturbs 0x245aa0. Original body restored.
// Separate scaled rotation input with an explicit unscaled arm 247, Separate rotation input with the unit-scale
// arm first 251, Conditional rotation input from the scale helper result 240, Rotation input changes only on the
// scaled branch 239, Explicit scaled input with one matrix scale observation 247, Conditional helper result with
// one matrix scale observation 240, Branch-selected rotation input with one matrix scale observation 239,
// Rotation reads a copied edge scaled in place 231. NOT kept: The pointer-selected rotation inputs recover frame
// 0x2c but grow the body to 820 or 822 bytes and raise the matched caller 0x245aa0 to 6 in the reduced image. All
// other forms keep frame 0x24 and lengthen the body. The original caller and transform shape are restored.
// Material word references 211, primary index field reference 163, resolved plane address comparison 222,
// with late plane table 215 or late edge 210, endpoint member references 212, with late position pointer
// 164 or codes extended ahead of the endpoints 205. NOT kept: The primary field reference gains one point.
// None recovers the native frame and side homes.
// M3 lane 3 r271 (fast with 0x245aa0, 0x245400, 0x244de0 and 0x245270, all matched unless noted; control 164, aligned 193,
// masked 88). LEAD, not kept (statement order and a written-out helper body are match-only): retail's frame 0x2c, the
// `push ebx; push esi; mov esi, ecx` prologue, the local EBP save, both long codes in stack homes, the first side in a
// stack byte and `cmp byte ptr [esp + 0xf], bl` all come from one statement order: both vertex pointers first, then each
// long code with its index, the plane table, the two plane pointers, the two sides, and only then the edge vector (the
// second vertex pointer stays live across the plane code; its loads are still scheduled early). Steps, each on the one
// before: (1) main's body with the plane table read after the codes, the edge after the sides and the dot written
// edge-first 198 (787 B, aligned 112: retail's frame and homes); (2) lane 1 r119's head on it (`s_material_245400 const
// *second = &materials[local_0->field_0a];` ahead of `local_1`, the compare and both long codes read through
// `materials[local_1]` and `second`) 116 (790 B, aligned 98: the first 17 instructions are retail's); (3) the sides as
// shifted bits or mask tests on that 206 (764 B: one code back in a register, frame 0x28), but with named indices
// (`long first_index = local_2 & 0x7fff; long second_index = local_3 & 0x7fff;` for the plane pointers and the index
// test) and `bool local_10 = ((dword)local_2 >> 15) & 1;` 138 at 783 B, and with each index declared right after its
// code 136 (783 B: everything through the `pop ebp; je` is retail's but the jump targets); (4) the dot through the
// existing dot3f(&local_12, &local_6) 128; (5) the edge scale as a select (edge_i/j/k copied, `edge_i = arg_2->scale *
// edge_i;` in the arm, vector3f_scale written out), edge_k/j/i declared in that order, rows i `forward, left, up`, j `up,
// forward, left` in one statement, k `forward, left` then `+= up` 53 (789 B, aligned 51, masked 25; found by a compile-only
// descent over the rows, the scale form, the copies' order, the dot, the cross helper and the edge spelling, which scores
// this file as the LTCG build does but for three call lines). Left at 53: the plane cross (retail loads the first
// plane's k once and has the second plane's member as the loaded operand of the other four products; cross3f and
// cross3f_staged give the same code, and 384 written-out spellings of it are worse), the dot's last add ahead of its
// last product, the select's three copies in i, j, k order, the i and j rows, and the point transform's rows (they move
// with the vector rows' spelling: field ranks). Also measured on the way: step 1 without the head 198, with the plane
// table late but the edge early 212 (0x245aa0 3); sides as `(short)local_2 < 0` on step 2 182 (`cmp word ptr [home], 0;
// setl`), read again from the material records 205; a 15/1-bit overlay of the code on step 2 206 (promoted, same as the
// shifts); the flag form of the sides on step 2 with step 5's select 207 (0x245aa0 3); one row pattern for all three rows, any term
// order or operand side, same code (aligned 62 compile-only: single-expression sums are rank-ordered); extra locals move
// two or three lines with period 8 and nothing else; steps 2 to 5 in main's statement order 236 (751 B). Diagnostics,
// reverted: the function returning the second vertex pointer at the facing failure 214 (the pointer then takes ESI, the
// homes appear); the dot written edge-first on main's body 207 at 783 B (aligned 157: retail's in-place products).
// The 0x24 frame still lacks the separate code and side homes. The matrix already loads at its test.
// Edge/cross names in both orders 164/164; edge/point names in both orders 164/164;
// endpoint and plane receiver names in both orders 164/164; all geometry names 164;
// all geometry names with the plane table after the edge 166. NOT kept: the late
// table still reserves 0x24 and raises caller 0x245aa0 to 3.
// MiniMax C (11-19): no gain (164, baseline; body restored). Retail reads the second surface index (field_0a) before the first (field_08) and homes the two decoded material codes as dwords at +0x14 and +0x18 with the first index at +0x1c, which is the 0x2c frame; ours has two homes and the 0x24 frame. Reading the two material codes in retail order measures 163, a gain of one and under the bar.
// End vertex record 164; retained start vertex 164; both vertex bindings 164; both bindings with late
// plane table 205; retained primary material 164; both material bindings 164; primary material with
// late output point selection 164; bound vertices with shifted side bits 232.
// NOT kept: the late plane-table form lengthens the body and raises caller 0x245aa0 to 3. The shifted
// side bits shorten it to 752 bytes. The record bindings leave the frame at 0x24.
// M3 lane 5 r213 (one-object LTCG screen of this file linked with the caller's object, exact against the fast build;
// counts are aligned rows, lane 3 r271's lead = 51 on it, 53 positional). On that lead: the edge copies declared in each
// order x each edge row as one statement or split `(a + b)` then `+= c` three ways x the dot by hand (two orders) or
// through dot3f (1536 forms) best 41 (copies i, k, j; rows i and j `(forward + up)` then `+= left`, k one statement); on
// it the plane table read at each plane pointer 40 (fast 50 at 789 B), the plane pointers declared second-first 44,
// cross3f_staged and plane pointers typed as pointers to the normals the same code. The plane cross block never moves:
// written out in place, any of its 64 written operand orders 84 each; a file-local inline copy with any of the 64
// operand orders or any of its 6 parameter orders 40 each; no plane pointer locals 132-148 (frame 0x24 or 0x20).
// Dead initialised longs K = 0..31: period 8 ahead of the cross (38-51), flat after it, the cross never retail's
// (retail loads the second normal first in four products and reuses the first normal's k in two).
// M3 lane 11 r137 (fast with 0x245aa0, 0x245400, 0x244de0, all MATCH; control 164, aligned 193): the dot through the
// existing dot3f(&local_12, &local_6) 207 at retail's 783 B (aligned 156), with the operands the other way round 207 (same
// code); lane 1 r119's second-material pointer head alone (compare and codes through `second`) 211 (783 B), with dot3f
// 223 (768 B); the sides from the short material codes (`first_material < 0`) 213 (783 B). On lane 5 r213's best body
// (fast 50 at 789 B, re-measured): cross3f_staged 50 (same code); the first normal's k in a local for a written-out cross
// 190 (801 B; retail's single k load needs more than a named k).
// M3 lane 5 r217 (the same one-file link-time screen, exact against the full build for this body; aligned rows on lane
// 3 r271's lead, 40): the cross does move with the order its rows are stored in. Written out in place with rows in
// each of six orders 84 to 96, in a file-local helper 40 (i, j, k), 44, 43, 47, 54 (k, i, j), 56 (k, j, i): with the k
// row stored first all six products take retail's operand sides (second normal's k, i and j loaded against the first
// normal's i and j, first normal's k loaded against the second's j and i), but the products then run k row first
// where retail runs i, j, k. A staged cross (three scalars, then three stores) over every compute and store order:
// 37 with i computed ahead of j and stores i, j, k (four of six products retail's), 40 with j ahead of i, 54 / 56
// with k stored first. One normal component read into a local first (each of six) 41 to 59; four written operand
// spellings of each form the same code. The facing value as one triple-product expression, four spellings, 80 each.
// One to three parenthesis pairs on the dot's first product 45 / 67 / 80. Ten cross forms x 0 to 7 dead locals ahead
// of the points: 35 with the staged i, j, k cross at 7 (only the two products of the first normal's k still face the
// other way: retail loads that k once and multiplies it by the second normal's j and i from memory). A coordinate
// descent from seven starts over the cross form, both dead-local counts, six dot spellings, the edge copies' order,
// the scale arm's order and twelve spellings per rotation row: best 35, no match. Body unchanged.
// First complete decode, second complete decode, second index before both addresses, and
// second address/side paired 210 each. With shifted side bits 234/234/233/233.
// NOT kept: the sign tests reach retail length but regress. The shifted forms are short
// and all retain the 0x24 frame with the source and separate side homes displaced.
// Scaled axis contributions in six orders 223/222/222/223/222/222; vector setter for their sum 223; aggregate
// rotated direction 250. NOT kept: all grow past 783 bytes. The frame stays at 0x24 or grows past 0x2c to
// 0x30 or 0x3c.
// Equal-code guard with combined facing predicate 164, sides before word comparison 215,
// side comparison scoped to facing 197, signed-word plane ordinals 224, rotated and staged
// edge setters 164 each, one edge scale read 164, signed ordinals with scoped facing 215.
// The frame and code and side homes remain unresolved. NOT kept: ties and regressions.
// m3 lane 4 r238 (fast with 0x245aa0, 0x245400, 0x244de0, 0x245270, all MATCH; brackets = aligned / register-masked;
// control 164 [134 / 103]; lane 3 r271's lead re-applied from its saved patch 53 at 789 B [34 / 34], everything through
// the `pop ebp; je` retail's). On the lead: the point transformed ahead of the edge rotation 127 (788 B), ahead of the
// edge copies and the scale 130 (783 B); the scale arm multiplying the edge's own components (`edge_i = scale *
// local_6.i`, the way retail's arm reads the originals) 53 (same code), with the copies declared i, j, k 56; the
// facing sum written cross-first, k, j, i 182 (799 B). Read from retail's cross on the lead: the first plane's k is
// loaded once and is the register side of its two products, the other four load the second plane's member and
// multiply by the first plane's from memory; ours loads the first plane's j twice and its k and i once each.
// Secondary material record with late edge and split rejection 210; named side relation 231;
// selected acceptance test 207; shared dot 212 or staged cross 210; unsigned codes 210;
// signed sides or secondary widening first 209 each. NOT kept: the frame is 0x28 and the scores rise.
// Direct/primary-early/secondary-early/both-early/both-scoped normal addresses 206/202/203/164/206;
// paired ordinal-normal-side 203; shifted sides with scoped normals and direct/staged cross 234/234.
// The native 0x2c frame and separate side home still differ. Body restored.
// Whole endpoint references or pointers through one vertex table, edge before decode, table, plane addresses
// or after side tests, 165/205/205/218 each. After sides with second material bound first 218;
// separate side/ordinal decodes 199, long codes from records 198, both 167.
// NOT kept: the final two-decode form reaches the 0x2c frame and 783 bytes but scores 167.
// M3 lane 4 r240 (fast with 0x245aa0, 0x245400, 0x244de0, 0x245270; control 164; lane 3 r271's lead rebuilt 53 at 789 B).
// On the lead: cross3f with the normals swapped and both facing tests negated 63; the facing dot as dot3f(&local_6,
// &local_12) 61; the cross local declared beside the edge 53 (same code); cross3f_staged swapped with negated tests 63;
// the swapped cross with the edge-first dot3f 69. The first normal's k is still loaded twice where retail loads it once.
// M3 lane 15 r123 (h2m fast screen with 0x245aa0, 0x245400, 0x244de0, 0x245270, all MATCH unless noted; control 164). Lane 5 r213's
// saved lead re-measured on today's file 50 at 789 B (40 aligned rows). Reading of retail's cross on it: the six products run
// first-plane j (with the second plane's k, then its i), first-plane k (loaded once, with the second's j, then its i),
// first-plane i (with the second's k, then its j), so the k row's second product is computed second; the loaded operand is
// the second plane's member except where the first plane's k takes part. On the lead: the scale select's copies declared
// i, j, k (retail's copy order) 54 (46 aligned); the two normals in named pointers ahead of the cross, first or second
// declared first, 205/205 at 783 B (0x245aa0 48: the frame changes); the plane table in a local ahead of the plane pointers
// 50 (41 aligned); the cross result declared with the edge 50 (same code); the dot through dot3f in either operand order
// 56/56 (47 aligned); cross3f_staged 50 (same code); the second plane pointer declared first 54 (44 aligned). No match; the
// lead's statement order and written-out rotation are match-only, so the body is unchanged.
// On the separate code and ordinal decode, primary material pointer/reference 167/167,
// early normal members with direct/staged cross 167/167, facing-scope normal lvalues
// or facing-scope plane table 208/208, and paired widening with normal selections
// or a primary material pointer 169/169. NOT kept: the 167 and 169 forms reach
// frame 0x2c and 783 bytes but regress. The late plane table also loses the matched caller.
// M3 lane 5 r226 (h2m fast screen with 0x245aa0, 0x245400, 0x244de0, 0x245270, all MATCH each time; control 164 at 785 B).
// Kept (fast and plain 163 at retail's 783 B, neighbours matched): the edge rotated by the existing
// matrix_transform_vector(arg_2, &local_6, &local_6) in place of that helper's rows written out by hand with a staged
// j row (the hand rows no longer score better on today's body). On it: the dot through dot3f in either operand order
// 235 (757 B); the scale after the rotation 204. Leads, not kept (lane 3 r271's statement order is match-only):
// that order with vector3f_scale and matrix_transform_vector 129 at 783 B (dead longs at the top K = 1..7
// 131/131/131/127/127/127/129), with the dot through dot3f 156 (767 B; K = 1..7 158/158/158/154/154/154/156); that
// order with the scaled rotation in a file-local inline helper shaped like function_245400_transform (three copies,
// `i = matrix->scale * i` in the scale arm, then the rows) 110 at 783 B (K = 1..7 112/112/112/108/108/108/110; rows
// written up, left, forward or forward, left, up the same code): its copies, `jnp` and scale arm are retail's
// instruction for instruction, left are the rows' term order (retail i and k (forward + left) + up, j (up + forward)
// + left), the cross and the dot's last add. Dead longs inside that helper or ahead of the cross, 1 to 7 each, 110
// every time (only the function-top count moves anything). That helper on today's statement order 163 (783 B).
// On lane 5 r213's 50 lead (re-measured 50 at 789 B; lane 3 r271's 53): the k row's expression as a dead local ahead
// of cross3f 50 (same code), assigned to the cross's k afterwards 162 (792 B); both normals copied into locals 255
// (823 B); named normal pointers declared second-first 205 (783 B); cross3f_staged 50 (same code); the cross written
// out k, i, j 182 (799 B). Reading of ours against retail there: ours has the first normal's member as the loaded
// operand in four products and loads its k twice; retail loads the second normal's member in four and the first
// normal's k once for both of its products.
// Edge after plane decoding, with retained short codes, named plane indices,
// early secondary material index, or both 207/207/207/207. Retained long-code
// forms 197/197/197/197. All reach 0x2c and 783 bytes. The long forms keep
// the source in ESI but extend the codes at the loads. NOT kept. The short
// forms change matched caller 0x245aa0 and every form scores higher.
// Primary and secondary material records addressed as pointers or references,
// with the secondary code before primary selection 163 each, or after it 164.
// NOT kept: the earlier read gains one at 784 bytes and keeps the 0x24 frame.
// M3 lane 5 r228 (h2m fast with 0x245aa0, 0x245400, 0x244de0, 0x245270, all MATCH each time; control 163 at 783 B): the
// second plane pointer declared ahead of the first 162 (the cross's six products reorder, frame unchanged); the plane
// indices named (`first_index`, `second_index`, each used for its pointer and the compare) 163 (same code), with the
// sides as `((dword)code >> 15) & 1` 197; cross3f with the normals swapped and the dot negated 204; the facing test as one
// ternary on the side relation 209 (786 B), with the swapped plane pointers 209.
// M3 lane 15 r130 (H2M_FAST with 0x245aa0, 0x245400, 0x244de0, 0x245270, all MATCH each time; control 163): the sides
// as `(code >> 15) & 1` on the signed long codes 226 (760 B); the first index read after the second material's code
// (retail loads +0xa first) 165; both 226 (760 B); the transformed point declared at function scope 163, the cross
// result at the block top 163 (same code each).
// Secondary endpoint scoped to edge 163, plane decode scoped after the edge 165,
// or both 165. Material-only surface scope or vertex indices captured on
// admission 163/163. Widened code declarations before admission 165,
// immutable widened codes in facing 165, or immutable codes with normals
// resolved at the cross 212. NOT kept: 0x24 frame and separate material homes
// still differ. Delayed plane-table forms also change the matched caller.
// Signed code compared with ordinal 209, with the ordinal subtracted before
// testing the sign 230, unsigned code compared with ordinal 209, unsigned high
// boundary 213, upper quotient 211, quotient and remainder 216, remainder
// with the opposite side test 175, unsigned code minus ordinal 230.
// NOT kept: the separate material and side homes remain displaced.
// Packed material difference for all tests, signed or unsigned word 229/238. Promoted outer difference
// with separate sides and low-index/side-only tests 214/208. Native outer equality with packed
// side-only test 210/222. Unsigned side-only/side-mask tests 234, and early packed equality rejection 229.
// NOT kept: none recovers retail's frame and each raises the score and changes the matched caller.
// M3 lane 12 r186 (fast with 0x245aa0 and 0x245400, both MATCH; control 163; lane 5 r213's saved lead re-measured 50 at
// 789 B). Read from the compiler's operand ranks (every commutative sum or product is sorted by a 16-bit rank, largest
// first; a field load through a shared address ranks by that address's value-table number modulo 4, a scalar by its
// symbol number): on the lead the cross's four field addresses (first j, second k, second j, first k) rank 3, 2, 1, 2
// where retail's operand sides need 0, 3, 2, 3, that is one more distinct expression ahead of the cross. A diagnostic
// store of a parameter sum to a scratch global just ahead of cross3f (removed at once) gives all six products retail's
// sides (157 at 799 B, the store's own code), the first plane's k still loaded twice where retail keeps it in a
// register. Retail's j rows (up * z, then x * forward.j with forward.j as the memory operand, then left * y, in both
// transforms) cannot come from one sorted sum while the scaled components rank near 0x1500: the left product would
// have to wrap past 0x10000, which needs component ranks of about 0x2000 (symbol numbers near 0x100). Forms on the
// lead: cross3f_staged 50 (same code); sides before the plane pointers 187; a plane-table local 50 (same code); the
// second plane pointer first 54; named normal pointers 205 (0x245aa0 48); dot3f(&local_12, &local_6) 56; sides as
// `(code & 0x8000) != 0` 50, `code < 0` 161 (783 B), `(bool)((dword)code >> 15)` 176, `(code >> 15) & 1` 52,
// `(code & 0x8000) == 0x8000` 207; the scaled edge components as three conditional expressions 179 (818 B, one compare
// each). Body unchanged.
// Shared matrix-axes rotation in place, selected scaled input, assigned input, or initialized input and scaler return 266/276/245/179.
// The same four rotations with edge formation after plane decoding 246/266/236/217. NOT kept: even the 0x2c forms grow past retail's extent.
// On the late edge and independent plane-side decode, secondary code before primary index
// 167, secondary record lvalue 167, named secondary short index 167, primary short index 169,
// direct secondary surface subscript 169, primary bound at emission 167, both short indices
// with that emission 169, paired encoded-field references 167.
// NOT kept: these recover the 0x2c frame and retain the 783-byte extent, but the surface
// and material table registers still differ and every score rises.
// One copied normal with direct/staged cross, first or second 248/241 each.
// In-place staged cross initialized on admission 236/233 or at decoding 221/218.
// NOT kept: all increase the score and grow past 783 bytes. The frame and separate side homes remain.
// The frame remains 0x24 against 0x2c, with separate material and side homes missing.
// material prefix with late endpoint 204, material prefix with late start 205.
// material prefix with endpoint indices 207, material prefix with selected emitter point 139.
// selected point with late endpoint 204, selected point with secondary material reference 139.
// selected point with staged cross 139, vertex records with staged cross 205.
// selected point with plane table between codes 139, selected ordered facing comparison 208.
// explicit facing exits on selected point 212, explicit exits with plane table between codes 212.
// NOT kept: the 139 forms recover frame 0x2c and the entry through the material comparison, but grow to 787 bytes and retain the differing float and code homes. The two explicit facing-exit forms regress.
// M3 lane 12 r188 (h2m fast with 0x245aa0 and 0x245400, both MATCH; control 163, aligned 135; lane 12 r186's body of lane 5
// r213's lead re-measured 50 at 789 B, aligned 30: the cross's operand sides and the j and k rows of the written-out
// transform are all that is left, both operand-rank questions). On that lead: the facing test's side agreement named
// once ahead of the side bits and used in both arms 118 (788 B), named inside the plane test ahead of the cross 73
// (783 B, aligned 42: the two side bytes then fold into one xor where retail compares two stored bytes); main's two
// short material locals in place of the second record pointer 197 (786 B, first +0x11); the edge vector formed right
// after the two point pointers 198; the plane test as `((local_2 ^ local_3) & 0x7fff) != 0` 190.// Read from the rank view on the lead: the cross's four member addresses take classes 3, 2, 1, 2 (first j, second k,
// second j, first k) and retail's sides need each one level higher, that is one more distinct expression met ahead of
// them; a first-material record pointer used like the second one 50 (aligned 31, only the offset-0 levels move), the
// two point pointers as casts of the vertex address 50 (same code), the dot written cross-first k, j, i 50: none moves
// the four classes. Body unchanged.
// Shared packed-code decoder returning index/side/plane/bare plane, promoted codes
// 224/209/211/210, source words 224/209/221/210.
// NOT kept: all raise the score and the two code words still lack retail's separate homes.
// The original decode and both plane lookups are restored.
// M3 lane 16 r189 (H2M_FAST with 0x245aa0 and 0x245400, MATCH throughout; control 163; lane 5 r213's saved lead
// re-measured 50 at 789 B). On this body: the in-place scale with the rotation through function_2477b1 on the rotation
// member 266 (832 B); the edge copied into a local, scaled in place under the scale test and rotated back into the edge by
// matrix_transform_vector 230 (804 B), by function_2477b1 179 (799 B). On the lead's head (second record pointer, the
// codes read at each use, named masked indices, shifted sides, the edge formed after the plane decode): the copy, scale and
// function_2477b1 172 (819 B), with matrix_transform_vector 171 (816 B); this body's own in-place scale and
// matrix_transform_vector 129 at 783 B (lead, not kept: needs the edge statement after the decode; with it at this body's
// place 200, plus `< 0` sides 226 at 766 B, plus the k, j, i dot 190; this body with only the edge statement moved 205); a
// vector version of the file's point-transform helper 110 (783 B; on this body 163). Body unchanged.
// On the widened-code frame with the shared rotation, surface table entry, advanced cursor,
// and surface reference 152 each. Indexed surface reads 155. Start ordinal before its table,
// both ordinals around or ahead of the table, and each ordinal at its lookup 152 each.
// NOT kept: the 152 forms retain the surface address in ECX and grow to 790 bytes.
// Indexed surface reads grow to 789 and change the matched caller.
// On the selected-point form, a sampled direction scale, in-place scaler
// return, sampled scale with scaler return, and separate scaled direction
// score 139/139/139/214. Both transforms under one guard give the same scores.
// NOT kept: the in-place forms recover frame 0x2c and the entry through the
// material comparison, with 787 bytes against retail's 783.
// Packed material code on the selected-point form. Index/side/both/raw
// from code values 219/197/197/208, through record lvalues 204/204/204/206.
// NOT kept: all recover frame 0x2c and regress. Code values and raw record
// reads also raise the caller to 3 in the preview.
// M3 lane 14 r156 (h2m fast with 0x245aa0, 0x245400, 0x244de0 and 0x245270; control 163): the edge vector formed between
// the plane pointers and the sides 209; the two long codes declared right after the material reads, ahead of the
// material test, 164 (790 B; 0x245aa0 6); the second plane pointer and the second side declared first 162 (one point,
// not kept); the plane-index test as `((local_2 ^ local_3) & 0x7fff) != 0` 221 (767 B; 0x245aa0 6); the matrix scale
// read once into a local for the test and the scale 163 (same code). Body unchanged.
// M3 lane 15 r148 (h2m fast with 0x245aa0, 0x245400, 0x244de0, 0x245270, all MATCH; control 163). On lane 5 r213's saved
// lead (re-measured 50 at 789 B, aligned 40): 0-7 unused initialised longs at the top 50/50/52/52/52/48/48/48 (no phase
// break); the plane test as `(local_2 ^ local_3) & 0x7fff` 190 (783 B); the first side bit as `(bool)((dword)code >> 15)`
// 173, the second 167. On this body: both side bits as `((dword)code >> 15) & 1` 226 (760 B). Body unchanged.
// M3 lane 8 r241 (H2M_FAST with 0x245aa0, 0x245400, 0x244de0, MATCH unless noted; control 163). LEAD 138 (787 B, first
// difference +0x35, frame 0x2c; not kept: a later plane-table declaration and a moved edge statement are match-only): the
// second material through a record pointer read twice (compare, long copy), both codes read through their records, named
// masked plane indices, the plane table read after the long codes, the sides as `((dword)code >> 15) & 1`, and the edge
// formed after the side bits. Everything through the plane decode is then retail's but the edge's XMM numbers (ours
// XMM0-2, retail XMM3-5); the cross operand sides, the scaled copy and the row orders are left. Steps: plane table after
// the edge 203 (787 B, 0x245aa0 3); with the record pointer and codes through the records 210 (0x245aa0 3); with the
// shifted sides 190 (783 B, 0x245aa0 MATCH, retail's first 15 instructions); with named indices 191; with the edge after
// the sides 179. On 138: the edge scaled into a copy then matrix_transform_vector 212 (823 B), function_2477b1 221 (833 B),
// in-place scale with function_2477b1 242 (849 B); 0x16f570's file's in-place vector transform (scale test on scalar
// copies, rows up + left + forward) 172 (791 B: it gives retail's copy-before-test select), 0xe59e0's file's 172; that
// helper on main 163, on the 190 form 170. On main's statement order: record pointer and codes through the records 213,
// with named indices 213, with the shifted sides 225 (762 B); named indices alone 163. Body unchanged.
// M3 lane 4 r266 (H2M_FAST with 0x245aa0, 0x245400, 0x244de0, 0x142640, 0x142570, MATCH unless noted; control 163; lane 5
// r213's saved lead re-measured 50 at 789 B). On the lead, the first three parameters in each of the five other orders
// (definition, declaration and the 0x245aa0 call): source/matrix/index 50, index/source/matrix 50 (0x245aa0 16),
// index/matrix/source 50 (0x245aa0 16), matrix first 163/163 (754 B). The existing matched vector transform 0x142640
// (copy, scale under the test, rows) called for the edge: on this body 377 (613 B), on the lead 359 (631 B), each with
// the point through 0x142570 too 576/560: it stays a call, its file being /Ob1. An `inline` re-declaration of it in
// this file 359 (still a call). LEAD, not kept (`inline` on its definition is a held form): with `inline` on 0x142640's
// definition, on this body 163 (same code), on the lead 110 at retail's 783 B: the inlined copy gives retail's scale
// select (copies, `jnp`, scaled copies) row for row; left are the three rows' operand sides in both transforms and the
// cross, all operand-rank ties (the field loads' ranks fall on a mod-8 grid).
// M3 lane 13 r166 (reduced-image screens; lane 5 r213's saved lead re-measured 50 at 783 B): on the
// lead, the first edge subtraction in one redundant float paren pair (diagnostic, one more expression node ahead of
// the cross) 46: the cross's six products then take retail's sides, only the first plane's k stays loaded twice; two
// or three pairs 119/130; the cross written out in the helper's order 182 (with one to three paren pairs 180/167/129).
// On the 46 form: the dot's first product in one to three paren pairs 126/135/57; one to seven unused locals
// 46/48/48/50/46/46/46; the rotation's i row grouped (forward + left) + up as retail adds it 44, as one three-term sum
// 46; the copies declared i, j, k (retail's copy order) 50, with the grouped i row 48. On 44: 60 to 108 unused locals
// (diagnostic) 48-67. Reading the operand ranks: retail's j rows need forward.j at level 0 and left.j at level 7 (that product
// wrapping) in both transforms but up.j unwrapped in the rotation and wrapped in the point transform; the point
// transform reuses the rotation's field loads at the same levels, so the two transforms' scaled components would need
// symbol numbers near 0x100 and far apart, which no tried form gives. On main's body the facing dot through dot3f, either
// operand order, 235 each. Body unchanged.
// M3 lane 6 r254 (H2M_FAST with 0x245aa0, 0x245400 and 0x244de0, MATCH unless noted; control 163). Retail reads the
// second material index (+0x0a) first, forms the second record's address, then reads +0x08. On this body: the second
// material read ahead of the first index local (materials local first) 165; a second record pointer for the second
// material's code 163 (same count); both 165; the facing test as an if/else of two exits 221 (768 B); the plane table
// read after the edge vector 165 (0x245aa0 3). Body unchanged.
// M3 lane GA r33 (43 valid fast trials with 0x245aa0/0x245400/0x244de0/0x245270/0x245850 all MATCH unless noted; control 163
// at 783/783 B, fr 0x24; body unchanged): on main's body, vertex-table local 163, + second-before-first material read 165,
// early return on equal materials 163, inline copy+scale+rotate edge 163, 0x245850-style 3-step accumulated edge rows 213,
// split facing `if`/`else` returns 221 (768 B); on the 0x24-frame second-pointer/shifted-side order (edge before decode),
// 3-step/helper/early-return/vertices/cast-cross/direct-cross/i-k-j/2-step point 168-169, split facing 211 and TEST_FIELD_BIT
// sides 214 (both 0x245aa0 DIFF). On the true 0x2c-frame lead (edge after shifted sides): 3-step accumulated rows on edge and
// point with `k*k+j*j+i*i` 168 (789 B) / with `dot3f(&local_12, &local_6)` 116 at exact 783/783 B (with `function_245400_transform`
// 113, `dot3f` swapped 113, flat edge + helper 110, 2-step y point 114, one-caller helper 113: all 783/783 B with xmm0/xmm1
// exchanged); 2-step accumulated rows on edge + helper point 63 (789 B), + 2-step accumulated point 55 (789 B, 210/210 insns:
// 57/57 opening + 27/27 point-scale/x-row + 33/33 tail exact; cross and j/y/k/z operand-rank ties remain); split facing on
// 0x2c lead 229 (fr 0x28, 0x245aa0 DIFF). Restored to main.
// M3 lane 1 r180 (fast with 0x245aa0, MATCH throughout; control 163, aligned/masked 200/83). Lane 3 r271's lead
// (statement order with the edge after the shifted sides, named masked indices; match-only) re-measured 53 at 789 B
// (aligned/masked 51/28). Retail's vector part holds the scale in one register, copies the edge before the test and sums
// forward, left, up, which is 0x142640's code inlined (its point part reloads the scale three times, as this file's
// point transform does); 0x142640 sits in a /Ob1 file, so it stays a call here. Diagnostic (reverted at once): the
// lead with its vector part through 0x142640 and `inline` on 0x142640's definition 110 at retail's 783 B (aligned/masked
// 60/42: left the product order of the rows in both transforms and the cross). On the 53 lead: the cross through
// cross3f_staged 53 (same code); the facing dot through dot3f, either operand order, 61; the plane pointers indexed
// from the source field with no table local 53; the side bits as `(code & 0x8000) != 0` 53 (same code). A lead for a
// match needs 0x142640 inlinable (BLOCKED-BY 0x142640: its file's /Ob1). Body unchanged.
// M3 lane GA r39 (71 valid fast trials with 0x245270/0x244de0/0x245400/0x245850/0x245aa0 all MATCH throughout; control 163
// at 783/783 B, +0x2): on main's body, both material pointers (second first) / 0.0001f > local_13 / cross3f_staged 163
// (same code), first-first material pointers 213, vertex pointers v0/v1 210, ((code & 0x8000) >> 15) bool or byte /
// ((word)code >> 15) / (code & 0x8000) != 0 226 each, direct long codes (either order) 228, vector3f_rotate_into_axes 266
// (scale after 246), cross returning pointer into dot3f 235, inline 2-step point transform 161 (783 B, -2, not kept). On the
// 0x2c-frame lead (2-step edge and point): direct component edge subtraction drops the k,j,i-dot base 182 (799 B) to 55
// (789 B, 210/210 insns, same as dot3f); cross returning pointer into dot3f (either order), plane-typed cross, array-indexed
// cross, vector3d_from_points3d returning pointer, ((word)code >> 15), byte sides, 0.0001f > local_13, local_14 at block top
// all 55; ((code & 0x8000) >> 15) 57, v0/v1 locals 57, k,j,i edge subtraction 73, scale local 147, vector3f_rotate_into_axes
// 239, vector3f scaled copy 196; clean bindings (vertices, second_idx, first pointer) move offset-0 cross ranks to mk=37
// (55 each), and all five bindings (with planes and v0/v1) reach 53 (789 B, al=54, mk=38, fixing b.j*a.i). Body unchanged.
// @retail 0x245540
void function_245540(s_source_245400 const *arg_1, long arg_0,
	transform4x3f const *arg_2, real arg_3, real arg_4,
	long arg_5, long arg_6, s_collection_245270 *arg_7)
{
	s_surface_245400 const *local_0 = &arg_1->field_34[arg_0];
	long local_1 = local_0->field_08;
	s_material_245400 const *materials = arg_1->field_2c;
	short second_material = materials[local_0->field_0a].field_00;
	short first_material = materials[local_1].field_00;
	if (first_material != second_material)
	{
		plane3f const *local_7 = arg_1->field_0c;
		point3f const *local_4 = &arg_1->field_3c[local_0->field_00].field_00;
		point3f const *local_5 = &arg_1->field_3c[local_0->field_02].field_00;
		vector3f local_6;
		vector3d_from_points3d(local_4, local_5, &local_6);
		long local_2 = first_material;
		long local_3 = second_material;
		plane3f const *local_8 = &local_7[local_2 & 0x7fff];
		plane3f const *local_9 = &local_7[local_3 & 0x7fff];
		bool local_10 = local_2 < 0;
		bool local_11 = local_3 < 0;
		if ((local_2 & 0x7fff) != (local_3 & 0x7fff))
		{
			vector3f local_12;
			cross3f(&local_8->n, &local_9->n, &local_12);
			real local_13 = local_12.k * local_6.k + local_12.j * local_6.j + local_12.i * local_6.i;
			if (!((local_10 == local_11 && local_13 > -0.0001f) || (local_10 != local_11 && local_13 < 0.0001f)))
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
