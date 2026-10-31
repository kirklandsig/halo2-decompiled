// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_239B80.CPP: two-dimensional polygon helpers (convex hulls,
   clipping against lines and point-in-polygon tests over point2f
   arrays) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <float.h>
#include <string.h>

#define k_real_epsilon 0.0001f
#define MAXIMUM_POLYGON_POINTS 48
#define FLAG(bit) (1 << (bit))
#define TEST_FLAG(flags, bit) (((flags) & FLAG(bit)) != 0)
#define SET_FLAG(flags, bit, value) ((value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)))

struct vector2f { real i, j; };

/* a line in the plane: points p with dot(n, p) == d */
struct plane2f
{
	vector2f n;
	real d;
};

PRIVATE __forceinline bool polygon_clip_intersection_23a430(plane2f const *plane,
	point2f const *first, point2f const *second, long count, short capacity, point2f *output)
{
	real fraction = 0.0f;
	real dx = first->x - second->x;
	real dy = first->y - second->y;
	real denominator = plane->n.i * dx + plane->n.j * dy;
	if ((denominator < -0.0001f || denominator > 0.0001f) && count < capacity)
	{
		fraction = (plane->n.j * first->y + plane->n.i * first->x - plane->d) / denominator;
		output->x = first->x - dx * fraction;
		output->y = first->y - dy * fraction;
		return true;
	}
	return false;
}

// score 5 (was 101): loop setup stores [esp+0x10]/[esp+0x18] in another order (retail stores
// previous*4, loads the output, then current*8 and current*4; ours current*8 first), and the
// first intersection's x product loads the plane before the point. A compile-only build of this
// file reproduces both; the product operand orders follow the whole function (a no-op product in
// the classification loop rotates them), not the helper's spelling. Round 45: a plane-distance
// helper for the classification and the numerator 11 (x-first body 11); for the numerator only
// 7, with dy computed first 25; dot product numerator with dy first 25; a named current-side
// local 7; 576 compile-only combinations (distance spellings, dx/dy and output store orders,
// result placement, epsilon compare, returned count) never fixed the x product without flipping
// the denominators or the second numerator, and none moved the setup stores.
// Round 72 (full build): helper
// parameters (first, second, plane) 7 and (first, plane, second) 7, both making the second
// intersection's x product plane-first too; (plane, second, first) 5; a separate input pointer
// for the points or their copy 9 (x term first in the first intersection), declared first 9.
// The fraction defaulted first,
// x-first displacements and the scale inside the output expressions gave retail's XMM homes
// and length (101 -> 5). Then: current declared before previous 5; named current/previous
// point pointers 28; dot_product2d numerator 5; fraction shared with the classification
// distance 5; previous updated at the loop end 5; staged/reversed denominators and helper
// parameter orders screened compile-only without change. Earlier: frame 0x618 agrees; cursor/result initialization, delta XMM
// homes and extra fraction copies remain. X-first deltas 109; full denominator 103;
// scales before stores 101; first input record 207; Boolean helper result 101;
// aggregate intersection 152; capacity-first gate 108; native input count 153;
// intersection role names 101; output-first helper 101; count-result helper 101;
// x-first deltas with full denominator 107.
// Shared difference/projection/both 109/101/109; actual output cursor 256;
// difference/projection with output cursor 256/256; natural previous update 101; cursor with previous update 256.
// Second/both input records 163/247; complete/staged projection numerator 101/103;
// parallel output offsets 101; second record with offsets 163; numerator with offsets
// 101; nested eligibility/capacity scopes 101. Original helper and body restored.
// Immutable displacements 101; both projection products 101; plane value 269; word output/classification/clipping ordinals 231/220/202; common output result 138; current before previous 101.
// Round 54: numerator `y + (x - d)` 14; split sum then `(sum - d) / denominator` 7 (both
// products point-first but x first; explicit parentheses always order x first); both indices
// declared in the for 5; while loop with the updates at the end 5; `y - d + x` 14.
// Round 8: previous/current/result names 5 each; previous declared early or before the exits 5;
// point-first numerator and plain inline 5; normal-i/first-x locals 110/180; named x/y products
// 9/7; y product with point/plane parameter orders 7; both named products 9; defaulted y product 7.
// Original helper and body restored.
// @retail 0x23a430
short function_23a430(short count, point2f const *points, plane2f const *plane,
	real epsilon, short capacity, point2f *output)
{
	point2f copy[128];
	long sides[128];
	if (points == output)
	{
		memcpy(copy, points, count * sizeof(point2f));
		points = copy;
	}
	long original_count = count;
	long positive = 0;
	long negative = 0;
	for (long i = 0; i < original_count; i++)
	{
		real distance = points[i].y * plane->n.j + points[i].x * plane->n.i - plane->d;
		if (distance > epsilon)
		{
			sides[i] = 1;
			positive++;
		}
		else if (0.f - epsilon > distance)
		{
			sides[i] = 0;
			negative++;
		}
		else
			sides[i] = NONE;
	}
	long result = 0;
	if (!negative)
	{
		memcpy(output, points, original_count * sizeof(point2f));
		return (short)original_count;
	}
	if (!positive)
		return 0;
	long previous = original_count - 1;
	for (long current = 0; current < original_count; previous = current++)
	{
		if (sides[current] == 0)
		{
			if (sides[previous] == 1 && polygon_clip_intersection_23a430(plane,
				&points[current], &points[previous], result, capacity, &output[result]))
				result++;
		}
		else
		{
			if (sides[current] == 1 && sides[previous] == 0 && polygon_clip_intersection_23a430(plane,
				&points[previous], &points[current], result, capacity, &output[result]))
				result++;
			if (result < capacity)
				output[result++] = points[current];
		}
	}
	return (short)result;
}

static inline real dot_product2d(vector2f const *a, point2f const *b)
{
	return a->i * b->x + a->j * b->y;
}

static inline real normalize2d(vector2f *v)
{
	real m = (real)sqrt(v->i * v->i + v->j * v->j);
	if (fabs(m) < k_real_epsilon)
		return 0.f;
	real inv = 1.f / m;
	v->i = inv * v->i;
	v->j = v->j * inv;
	return m;
}

static inline bool points2d_equal(point2f const *a, point2f const *b)
{
	return fabs(a->x - b->x) < k_real_epsilon && fabs(a->y - b->y) < k_real_epsilon;
}

/* two pi, set up at run time */
real g_55e470;

long function_239d50(long count, point2f const *points, point2f *hull);
short function_239e50(short count, point2f const *points, short *indices);

/* 0 when every point is the same, 1 when they are on one line, 2 otherwise */
// score 42 (was 42): state/index/point registers (retail ECX/EDX/ESI, ours EDX/ESI/ECX) and the
// XMM homes differ at frame 0x18. Retail loads the unscaled normal j into a fresh XMM2 before the
// reciprocal (`movss xmm2,[j]` ... `mulss xmm2,xmm0`) where ours scales it in the reciprocal's
// register, as matched 0x23a2b0's inline normalize does, so retail's constants sit one XMM register
// higher; retail's dot then scales i in place (`mulss xmm1,[p.x]`), and it stores the first point's
// y before its x.
// Tried: local/formal point cursor with short ordinal 94/93; short remaining count 94/93; long
// ordinal 97/97; long remaining count 94/93; byte state 91; unsigned word ordinal 85; state
// increments 76; terminal equality 43; first point before scan 85; normalizer reference 42;
// point-first dot 42; success normalizer 102; normalizer input vector 126; compound scales 42;
// factor-first scales 42; positive normalizer scope 42; long point index 101; count-first guard 99;
// long state 86; plane/point names 42; forced normalizer, forced dot, both forced 42; single-result
// normalizer 42; squared-length expression 42; staged scales 42; reverse scale publication 42;
// length workspace 56; signed-distance expression 42; native first-point components 63; accumulated
// dot result 46; repeated j/i input 42/42; both scalar inputs 126; state renamed to sort before the
// index 42; zero-length arm first 44; `>= epsilon` far test 68; named normalizer length 56; line
// declared before the first point 42; shared normalizer/magnitude 42/42; tested initial/distance
// states 81/76; both tested states 79; early distance default 65; shared normalizer with tested
// states/default 79/65; bounded geometric enum with original/count-first/terminal/zero-first graph,
// independent-point word/enum/count-first/terminal graph, first-point memcpy/memmove
// 86/94/86/86/54/58/99/58/42/99; indexed point receiver across dispatch with short/long/signed-byte/
// enum state 89/104/109/104, the same receiver at function scope 89/104/109/104. Round 43: the
// normalizer's scale statements as `*=`, j first, both vector-first or both reciprocal-first 42 each
// (the compiler commutes them to the same code); the dot product's terms j first 42.
// factor-first scales 42; positive normalizer scope 42; long point index 101; count-first guard
// 99; long state 86; plane/point names 42; forced normalizer 42; forced dot 42; both forced 42;
// single-result normalizer 42; squared-length expression 42; staged scales 42; reverse scale
// publication 42; length workspace 56; signed-distance expression 42; native first-point
// components 63; accumulated dot result 46; repeated j input 42; repeated i input 42; both scalar
// inputs 126; state renamed to sort before the index 42; zero-length arm first 44; `>= epsilon`
// far test 68; named normalizer length 56; line declared before the first point 42; shared
// normalizer/magnitude 42/42; tested initial/distance states 81/76; both tested states 79; early
// distance default 65; shared normalizer with tested states/default 79/65; bounded geometric enum
// with original/count-first/terminal/zero-first graph, independent-point word/enum/count-first/
// terminal graph, first-point memcpy/memmove 86/94/86/86/54/58/99/58/42/99; indexed point receiver
// across dispatch with short/long/signed-byte/enum state 89/104/109/104, the same at function
// scope 89/104/109/104; the index declared at function scope 42 (same code); a case-0 element
// pointer 42 (same code); line.d cleared before the normalize test 65; line.n.j computed before
// line.n.i 49; the loop as a while 42 (same code); state declared first, scale and dot term
// orders (screened) the same code.
// differ; retail keeps the line normal's j in XMM2 through the scaling, so its constants
// sit one XMM register higher. Local/formal point cursor with short ordinal 94/93; short
// remaining count 94/93; long ordinal 97/97; long remaining count 94/93; byte state 91;
// unsigned word ordinal 85; state increments 76; terminal equality 43; first point before
// scan 85; normalizer reference 42; point-first dot 42; success normalizer 102; normalizer
// input vector 126; compound scales 42; factor-first scales 42; positive normalizer scope
// 42; long point index 101; count-first guard 99; long state 86; plane/point names 42;
// forced normalizer 42; forced dot 42; both forced 42; single-result normalizer 42;
// squared-length expression 42; staged scales 42; reverse scale publication 42; length
// workspace 56; signed-distance expression 42; native first-point components 63;
// accumulated dot result 46; repeated j input 42; repeated i input 42; both scalar inputs
// 126; state renamed to sort before the index 42; zero-length arm first 44; `>= epsilon`
// far test 68; named normalizer length 56; line declared before the first point 42.
// state/index/point and normalization SSE homes still differ at frame 0x18.
// Earlier: coupled state/index/XMM homes and normalized Y load remain;
// local/formal point cursor with short ordinal 94/93; short remaining count
// 94/93; long ordinal 97/97; long remaining count 94/93. Opening body restored.
// zero/one SSE homes, state/index/point registers and normalized Y load remain;
// byte state 91; unsigned word ordinal 85; state increments 76; terminal equality 43;
// first point before scan 85; normalizer reference 42; point-first dot 42; success normalizer 102.
// Frame 0x18 agrees; state/index, point pointer and SSE constant registers differ.
// normalizer input vector 126; compound scales 42; factor-first scales 42; positive normalizer scope 42; long point index 101; count-first guard 99; long state 86; plane/point names 42; forced normalizer 42; forced dot 42; both forced 42.
// Earlier: state/index/point and normalization XMM homes remain.
// Single-result normalizer 42; squared-length expression 42; staged scales 42;
// reverse scale publication 42; length workspace 56; signed-distance expression 42;
// native first-point components 63; accumulated dot result 46.
// Repeated j input 42; repeated i input 42; both scalar inputs 126.
// Shared normalizer/magnitude 42/42; tested initial/distance states 81/76; both tested states 79;
// early distance default 65; shared normalizer with tested states/default 79/65.
// Bounded geometric enum with original/count-first/terminal/zero-first graph; independent-point
// word/enum/count-first/terminal graph; first-point memcpy/memmove 86/94/86/86/54/58/99/58/42/99.
// Original executable source restored.
// Round 96 (one-file LTCG harness, reproduces the build; aligned 35, positional 42): 89 combinations of five
// scale spellings in normalize2d, three dot_product2d operand orders, three declaration orders of first/line/
// state and both loop-condition orders: 35/42 or worse (the swapped condition adds three instructions);
// normalize2d with j read into a scalar first 42, both scalars 42, the reciprocal written at each scale 42,
// j scaled first 42, an else arm setting the result to zero 42.
// Indexed point receiver across dispatch with short/long/signed-byte/enum state 89/104/109/104;
// the same receiver at function scope 89/104/109/104. Original indexed expressions and state
// restored.
// components 63; accumulated dot result 46; repeated j input 42; repeated i input 42; both
// scalar inputs 126; state renamed to sort before the index 42; zero-length arm first 44;
// `>= epsilon` far test 68; named normalizer length 56; line declared before the first point
// 42; shared normalizer/magnitude 42/42; tested initial/distance states 81/76; both tested
// states 79; early distance default 65; shared normalizer with tested states/default 79/65;
// bounded geometric enum with original/count-first/terminal/zero-first graph, independent-point
// word/enum/count-first/terminal graph, first-point memcpy/memmove 86/94/86/86/54/58/99/58/42/99;
// indexed point receiver across dispatch with short/long/signed-byte/enum state 89/104/109/104,
// the same receiver at function scope 89/104/109/104.
// Round 18 (lane 11): state declared first 42; loop index declared at the top or first 42/42;
// while loop with the increment at the end 42; count test first 99. Compile-only (reproduces
// our SSE block exactly): 0x2b400's normalizer form, compound, operand-order and j-first
// scales, single-result normalizer, early inv/m defaults, j read into a local, named square,
// each with four dot operand orders: all give our block (j-first one worse); field-wise or
// memcpy first-point copies keep the x-first stores.
// state/index/point registers (retail ECX/EDX/ESI) and the XMM homes differ;
// retail loads the line normal's j into XMM2 before the division and scales it there, so its
// constants sit one XMM register higher (frame 0x18 agrees). Tried: local/formal point cursor with
// short ordinal 94/93; short remaining count 94/93; long ordinal 97/97; long remaining count 94/93;
// byte state 91; unsigned word ordinal 85; state increments 76; terminal equality 43; first point
// before scan 85; normalizer reference 42; point-first dot 42; success normalizer 102; normalizer
// input vector 126; compound scales 42; factor-first scales 42; positive normalizer scope 42; long
// point index 101; count-first guard 99; long state 86; plane/point names 42; forced normalizer,
// forced dot, both forced 42; single-result normalizer 42; squared-length expression 42; staged
// scales 42; reverse scale publication 42; length workspace 56; signed-distance expression 42;
// native first-point components 63; accumulated dot result 46; repeated j input 42; repeated i
// input 42; both scalar inputs 126; state renamed to sort before the index 42; zero-length arm
// first 44; `>= epsilon` far test 68; named normalizer length 56; line declared before the first
// point 42; shared normalizer/magnitude 42/42; tested initial/distance states 81/76; both tested
// states 79; early distance default 65; shared normalizer with tested states/default 79/65; bounded
// geometric enum with original/count-first/terminal/zero-first graph and independent-point
// Round 12 (lane 15): if/else chain for the switch 90; the normal's j component computed first 49;
// while loop 42; normalizer reading j before the division 42; j read once for the length and the
// scale 42; every scale and length operand order in the normalizer the same code.
// Round 171: parameters as (points, count) 42; the shared 2-D header's union vector2f and
// normalize2d_fast 42; every switch case order 42; state set before the first-point copy 42;
// field-wise first-point copy, either order, 63 (310 bytes); normalizer dividing by the length 42;
// dot helper by reference or forceinline 42, by value 118/124/162 (harness); the dot written out in
// place of the helper, or the length named in a braced case 0, 56 each (retail's dot shape, `mulss
// xmm1,[p.x]`, but j still scaled in the reciprocal's register, one instruction short).
// Round 34 (fast): separate normal/distance locals in place of the plane2f 52; case 0 setting the
// state before the distance 42; the normalizer reading j into a scalar before the length and
// scaling that scalar 42 (j still folded as `mulss xmm0,[j]`); an explicit `default: break;` 42;
// the shared header's union vector2f with that j-scalar normalizer 42.
// Round 132 (an LTCG env link reproduces it): every order of the first/line/state declarations with
// the scales as written or `*=` j first and the dot taking (normal, point) or (point, normal): 42 each
// (24 forms); the normalizer scaling through a 2-D scale helper (normalize3d's shape) 42; dividing by
// the length 42; the length's sum j first 42; the normal built in a local vector and assigned 71.
// Round 110: a block-local point pointer in case 0 (fast) 42. One-file LTCG harness (aligned 35, positional
// 42 as the build): normalize2d with the zero length set in an if arm and the scale in its else 42; j read
// into a scalar after the reciprocal 42; the vector copied into a local first 72 (positional); the
// reciprocal written at both scales 42; a single result set in the scale arm 42. Matched 0xde080's
// normalizer keeps `movss xmm4,[v->i]; mulss xmm4,xmm0` (no commute), while ours commutes `v->j * inv`
// into the reciprocal's register here: retail's reciprocal looks live past both scales.
// Round 25 (fast): a `vector2f *` local to line.n in case 0, with only the i store through it 69 (305
// bytes), the same with j scaled first 73 (308); i store, normalize and dot through it 73; only the j
// store through it 44 (304 bytes: retail's exact normalize block, constants XMM5/XMM4; the dot's
// `mulss xmm1, [p.x]` fold and the state/index/point registers remain); both stores 53; normalize/dot
// argument alone 42 (same code). Compile-only diagnostic: the reciprocal kept live past both scales
// gives retail's XMM numbering. Opening body restored.
// Round 54 (lane 12, fast; compile-only reproduces both the SSE block and the integer allocation):
// operand order follows which normal component is written by name first. line.n.i written by name with
// line.n.j and the case-0 dot through a `vector2f *normal = &line.n` gives retail's whole SSE block (1.0 in
// xmm5, 0 in xmm4, j loaded fresh into xmm2, `mulss xmm1, [p.x]`) 23 (first difference +0xe; not kept: a
// by-name/pointer mix kept only with a match), also with case 1's dot through `normal` 23; the same with
// the dot through the helper 44 (304 bytes); every normal access in case 0 through the pointer 39 (301),
// with case 1 too 66; with a y-first field copy of the first point 44 (310). Left after the SSE block:
// state/index/point colouring (ours EDX/ESI/ECX, retail ECX/EDX/ESI), which compile-only diagnostics tie to
// case 1 and to the case-0 point pointer living across `lahf` (without either, retail's ECX/EDX and y-first
// copy appear); unchanged by a loop-top or block-local point pointer, the index declared first, state
// first, NONE case last, a while loop, `default: break`, `line.d = 0` before the test, the test inverted,
// case 1's dot by name or through the pointer in either term order.
// @retail 0x239b80
short function_239b80(short count, point2f const *points)
{
	point2f first;
	plane2f line;
	short state = NONE;

	for (short i = 0; state < 2 && i < count; i++)
	{
		switch (state)
		{
		case NONE:
			first = points[i];
			state = 0;
			break;
		case 0:
			line.n.i = first.y - points[i].y;
			line.n.j = points[i].x - first.x;
			if (normalize2d(&line.n) != 0.f)
			{
				line.d = dot_product2d(&line.n, &points[i]);
				state = 1;
			}
			else
			{
				line.d = 0.f;
			}
			break;
		case 1:
			if (!(fabs(dot_product2d(&line.n, &points[i]) - line.d) < k_real_epsilon))
				state = 2;
			break;
		}
	}

	return state;
}

// @retail 0x239cb0
long function_239cb0(long count, point3f const *points, point2f *hull)
{
	point2f points2d[MAXIMUM_POLYGON_POINTS];

	if (count > MAXIMUM_POLYGON_POINTS)
		count = MAXIMUM_POLYGON_POINTS;
	for (long i = 0; i < count; i++)
	{
		points2d[i].x = points[i].x;
		points2d[i].y = points[i].y;
	}

	return function_239d50(count, points2d, hull);
}

// @retail 0x239d50
long function_239d50(long count, point2f const *points, point2f *hull)
{
	point2f unique_points[MAXIMUM_POLYGON_POINTS];
	short indices[MAXIMUM_POLYGON_POINTS];

	short unique_count = (short)count;
	memcpy(unique_points, points, sizeof(point2f) * (count > MAXIMUM_POLYGON_POINTS ? MAXIMUM_POLYGON_POINTS : count));
	for (long i = 0; i < unique_count - 1; i++)
	{
		for (long j = i + 1; j < unique_count; j++)
		{
			if (fabs(unique_points[i].x - unique_points[j].x) < k_real_epsilon &&
				fabs(unique_points[i].y - unique_points[j].y) < k_real_epsilon)
			{
				unique_points[j] = unique_points[unique_count - 1];
				unique_count--;
			}
		}
	}

	short hull_count = function_239e50(unique_count, unique_points, indices);
	if (hull_count >= 3)
	{
		for (long i = 0; i < hull_count; i++)
			hull[i] = unique_points[indices[i]];
	}
	else
	{
		hull_count = 0;
	}
	return hull_count;
}

/* gift wrapping: the indices of the convex hull of the points, starting from
   the lowest */
// score 203 (was 203): Frame 0x14 agrees; count/departure homes, scan bound, equality and compaction differ.
// Round 27 (lane 9): memmove for the compaction (retail calls the CRT) with a bool flag and the heading set before the scan gives
// retail's count home +0x10, scan bound EDX and early heading/flag stores, but points takes EBP and the flag lands in points' home
// (retail: indices in EBP, the flag in its home) 265 (759 B); with the flag declared first 265; long flag 242; memcpy kept 250;
// `left_start = left_start || !points2d_equal(...)` 273.
// Frame 0x14 agrees; retail keeps `count` in memory (ebx only as temporaries:
// the call argument, then the hull count), ours gives `count` ebx for the whole body.
// Earlier: scan/count registers, equality scheduling and compaction inline boundary differ. Tried direct index output 261; plain lowest scan 250; full current index 299; full candidate counter 256; native completion returns 279; plain scan direct output 250; plain scan append then count 250.
// Full count tests 274; current-point snapshot 271; byte start state 250; float workspace names 250; still-at-origin state 264; inline classifier declaration 250; forced equality declaration 250.
// Unsigned word result 275; unsigned first/candidate ordinals 247/209 (caller lost); native compaction ordinals 285;
// monotonic departure 263; compaction shared exit 279; heading initialized after lowest scan 234.
// signed-word departure state 208; native selected-next index 273; departure before classifier 218; unsigned compaction ordinals 248; lowest constants after current index 234.
// Round 212: Unsigned-word/native-long departure 208/203; result lexical home 208; native lowest-scan ordinal 612; entry heading lifetime 237. Result/departure slots, bound and compaction boundary remain.
// Round 229: Equality-helper false-first/true-first/separate/early rejection results with long
// departure 260/260/260/212; with Boolean departure 249/249/249/234. Original helper/body restored.
// Departure/output/both/current/heading names 203 each; byte departure 234; late departure and
// lowest-coordinate order 203/203; heading before scan 220.
// Shared difference predicate early returns bool/byte/short/long 220/220/202/213;
// initialized difference result bool/byte/short/long 271/271/250/250. All restored.
// On the long departure state: signed-word/unsigned-word/unsigned-native/int result
// 257/257/203/203; native input count 259, with signed/unsigned word result
// 268/268; observed signed-word input count 203. Caller checked with each.
// Round 152: Classifier rejection with entry result, long/bool/byte/short departure
// 262/262/262/259; post-classification result 256/256/256/251. All restored.
// Equality input contracts (pointer order, first/second/both native references), each
// with long/Boolean departure: 203/227/203/234/203/234/203/234. Helper and body restored.
// Round 59: retail keeps the hull count at +0x10 and the departure flag as a byte in the index
// output's argument home, sets the heading and the flag before the lowest scan, copies the count for
// the scan bound, does not rotate the hull loop (test at its head, three back edges) and stores the
// flag as `x = 0; if (...) x = 1`. Tried: departure bool/char/short 234/234/208, each with the heading
// set before the scan 252/252/227 (bool: retail's two homes), long with it 220; on bool + heading
// first: flag set only when unequal 265 (751 B, aligned closest), loop test without the short cast 245,
// heading before the flag 250, flag after the current index 251.
// Round 102: the overlapping compaction copy is retail's CRT call (`memmove`, as matched
// memmove callers): 230 (759 B; `count` then leaves ebx, but `points`/`indices` move); with a
// bool flag and `left_start = left_start || !points2d_equal(...)` (retail's two direct stores) 247;
// plus heading/flag stored before the scan and a done flag in the hull condition (retail's
// unrotated loop and j-loop registers) 229; best declaration order of those 227; lowest_y
// declared first 203. One use of `count` fewer moves it to memory (diagnostic only).
// Round 105: on the done-flag form `count` keeps a callee-saved register whichever single use is
// removed (outer bound, scan bound, the current index's start, the call argument), so it is not one
// use over a threshold; renaming count/hull_count/left_start changes nothing. With `current = count`
// declared before the classifier call: 153 (784 B; done flag, kept only on a match), lowest_y before
// lowest_x 151; the hull-loop declarations after the index store 155; memcpy for the compaction 215;
// the increment at the end of the pass 176; the flag as `if (!left_start)` 246; the two final exits
// as an else-if 151. There points/current/count sit in EBX/ESI/EDI (retail: points ESI, current
// EDI, count in memory, the hull count cached in EBX).
// Round 16: count read through its address at the first-scan/hull bounds 203 each; explicit false/true
// departure branches with Boolean state and early heading, with memmove/memcpy 253/286; existing
// displacement helper 203; shared start-point pointer 253; read-once start index 265, with memmove 263.
// Original body restored.
// Round 239: separate lowest-scan/hull-pass/candidate-scan bound values, all
// seven combinations 203 each; all bounds with overlapping CRT compaction 230.
// Original bounds, equality helper, compaction and body restored; caller intact.
// Round 382: all four coordinate epsilon thresholds in four producer orders,
// with/without both candidate-angle thresholds, 204/204/204/204/204/204/204/204.
// Original threshold expressions and complete executable body restored.
// Round 385: Natural nonterminal continuation pairs with long/bool departure
// and memcpy 340/340/338/338, with memmove 323/340/307/307. Original terminal tests restored.
// Round 387: Parameter-derived current index live before classification, with Boolean departure, CRT overlap compaction and constant heading combinations 245/215/236/277/261/247/250/251; no added completion flag. Original body restored.
// Round 233: Complete current/candidate coordinate carriers at each candidate,
// squared-distance expression in seven subsets, then overlap CRT compaction:
// 253/287/278/203/253/287/278/289. All original coordinate reads, arithmetic and body restored.
// Round 182: native unsigned-word input count, long/bool departure and
// late/early heading crossed with memcpy/memmove 260/232/258/239/253/259/261/266;
// matched caller becomes 34 in each reduced trial. Original signature and body restored.
// Round 181: lowest/candidate-distance/ranking rejection subsets 203 each (seven forms); complete rejection graph with explicit stdcall 203. Original body and contract restored.
// Round 185: Coupled entry heading/departure bool/byte/short/long with memcpy/memmove
// 259/267/259/267/259/249/259/244; actual output cursor, long/bool departure and CRT combinations
// 249/266/277/264, with early heading 268/250/276/263. Cursor frame grows to 0x1c and four memcpy forms lose
// the caller. All state lifetimes, ordinals, original compaction and body restored; native result/departure
// homes and count register remain.
// Round 309: Advancing native lowest/candidate/both point cursors 203/200/200; duplicate
// index cursor alone/with lowest/candidate/both 231/230/229/228; all with overlap CRT 229.
// The 200 forms gain only three; original scans and full body restored. Count/departure homes remain.
// Round 171: retail's hull loop is tested at its head with the entry and all three back edges
// jumping to one `hull_count < count` test, and its three early exits skip the compaction: the
// shape of a completion flag in the loop condition (`while (!done && ...)`, compaction under
// `if (!done)`); an ordinary while, `for (;;)` with the test or with the compaction inside it at the
// head all compile to our rotated loop (203). On the flag form (match-only): 151; with the
// departure flag as `if (!left_start) left_start = ...` 246 (793 bytes); with memcpy 215. Without the
// flag: Boolean departure, heading first, memmove, `||` update and `current` at function scope 253
// (746 bytes); direct returns 262; short hull count 257; `current` at function scope 245, with the
// Boolean departure 215; the heading at function scope 231. Original body restored.
// @retail 0x239e50
short function_239e50(short count, point2f const *points, short *indices)
{
	long hull_count = 0;

	if (function_239b80(count, points) == 2)
	{
		long left_start = false;
		real lowest_x = FLT_MAX;
		real lowest_y = FLT_MAX;
		short current = count;

		for (short i = 0; i < count; ++i)
		{
			if (lowest_y - k_real_epsilon > points[i].y ||
				lowest_y > points[i].y && lowest_x + k_real_epsilon > points[i].x ||
				lowest_y + k_real_epsilon > points[i].y && lowest_x - k_real_epsilon > points[i].x)
			{
				lowest_x = points[i].x;
				lowest_y = points[i].y;
				current = i;
			}
		}

		real previous_angle = 0.f;
		while ((short)hull_count < count)
		{
			real best_distance = -FLT_MAX;
			indices[(short)hull_count++] = current;
			short next = NONE;
			real best_angle = FLT_MAX;
			real best_delta = FLT_MAX;

			for (short j = 0; j < count; j++)
			{
				real dx = points[current].x - points[j].x;
				real dy = points[current].y - points[j].y;
				real distance = (real)sqrt(dx * dx + dy * dy);
				if (distance > 0.000244140625f)
				{
					real angle = (real)atan2(points[j].y - points[current].y, points[j].x - points[current].x);
					real delta = angle - previous_angle;
					while (delta < -k_real_epsilon)
						delta += g_55e470;
					if (delta < best_delta - k_real_epsilon ||
						!(delta > best_delta + k_real_epsilon) &&
						(j == indices[0] || next != indices[0] && distance > best_distance))
					{
						best_distance = distance;
						best_angle = angle;
						best_delta = delta;
						next = j;
					}
				}
			}

			current = next;
			previous_angle = best_angle;
			if (next == NONE)
				goto finished;
			if (!left_start)
				left_start = !points2d_equal(&points[next], &points[indices[0]]);
			if (next == indices[0])
				goto finished;
			if (left_start && points2d_equal(&points[next], &points[indices[0]]))
				goto finished;
		}

		for (short i = 0; i < (short)hull_count; i++)
		{
			short j;
			for (j = 0; j < i; j++)
			{
				if (indices[j] == indices[i])
				{
					hull_count = i - j;
					memcpy(indices, &indices[j], sizeof(short) * (short)hull_count);
					break;
				}
			}
			if (j < i)
				break;
		}
	}

finished:
	return hull_count;
}

PRIVATE __forceinline void polygon_difference_23a220(point2f const *a, point2f const *b, vector2f *result)
{
	result->i = a->x - b->x;
	result->j = a->y - b->y;
}

// The polygon's edges and the point's offset are both differences of two vertices: the edge
// runs from the current vertex to the next one (wrapping round the end), the offset from the
// current vertex to the tested point. A point that lies farther from an edge than the radius
// is outside the polygon, so the function returns false for it.
// @retail 0x23a160
bool function_23a160(point2f const *point, real radius, short count, point2f const *points)
{
	short total = count;
	bool result = true;
	real radius_squared = radius * radius;
	for (short i = 0; i < count; i++)
	{
		long next = (i + 1 >= total) ? 0 : i + 1;
		point2f const *current = &points[i];
		vector2f edge;
		polygon_difference_23a220(&points[next], current, &edge);
		vector2f offset;
		polygon_difference_23a220(point, current, &offset);
		real edge_j_value = edge.j;
		real edge_i_value = edge.i;
		real length_squared = edge_i_value * edge_i_value + edge_j_value * edge_j_value;
		if (length_squared != 0.f)
		{
			real cross = offset.i * edge.j - offset.j * edge.i;
			if (cross > 0.f && cross * cross > length_squared * radius_squared)
			{
				result = false;
				break;
			}
		}
	}
	return result;
}

/* true when the point lies on the inner side of every edge of the polygon,
   to within epsilon */
// @retail 0x23a220
bool function_23a220(point2f const *point, short count, point2f const *points, real epsilon)
{
	bool result = true;
	for (short i = 0; i < count; i++)
	{
		point2f const *current = &points[i];
		long next = (i + 1 >= count) ? 0 : i + 1;
		vector2f offset;
		polygon_difference_23a220(point, current, &offset);
		vector2f edge;
		polygon_difference_23a220(&points[next], current, &edge);
		if (edge.i * offset.j - edge.j * offset.i < -epsilon)
		{
			result = false;
			break;
		}
	}
	return result;
}

PRIVATE __forceinline bool clip_points_equal_23a6f0(point2f const *a, point2f const *b, real epsilon)
{
	return fabs((double)a->x - b->x) < epsilon && fabs((double)a->y - b->y) < epsilon;
}

// score 129 (was 129): Retail length and the mask update agree; retail keeps the previous side in memory
// aligned frame and stride/prior homes agree; prior-side byte and SIMD homes differ.
// Earlier: Retail length and the mask update agree; retail keeps the previous side in memory
// (ours CL across the back edge, one instruction short from the side test on), loads count once into ECX
// for the scaled index and the guard, and keeps zero in XMM2. Earlier: count/stride/prior homes remained
// (retail keeps the copy size at +0x18, the previous point at +0x1c and both side flags in memory).
// Earlier: count/stride homes and distance/intersection lifetimes differ. Tried capacity break common result 245; shared mask return 254; existing plane dot 243; plain previous side 265; previous side branches 259; byte side presence 243; named distance products 243; native clipping role homes 243.
// Native current ordinal 249; long side states 264; prior-point ordinal 288; ordered fraction clamp 243; clear-mask arm first 243; shared normalized count 275; existing point difference 243.
// Explicit source bit 243; unsigned input ordinal 237; word side states 263; retained signed distance 273; parallel edge scales 243.
// signed side-presence words 248; signed current-side byte 256; double equality tolerance 302; native equality references 237; ordinary unsigned previous-side byte 278.
// Round 212: Current-side unsigned word/native long 257/250; input ordinal name 237; true-first current side 341; positive-first final policy 236. Count/stride/prior homes and shared publication remain.
// Round 229: Equality helper native first/second/both point arguments 356/357/444;
// result-first with those arguments 356/357/444; pointer result-first/early rejects 237/237.
// Original helper/body restored.
// Previous-point, distance, positive-state, negative-state and result names 237 each;
// status declaration order 264; previous-point address 237; displacement order 237.
// Actual NONE failure/common publication unsigned/signed ordinal 212/252; ordinary previous side 227/254;
// positive-first policies 211/251/256/253. Keep only the minimal 212 form; one-point tail edit restored.
// Native previous/plane/both observations 378/370/354; both native points 355;
// all native inputs 376; complete plane/previous/both scalar observations 347/359/368.
// Original inputs, copies and body restored.
// Fraction default between/before deltas, before clipped publication and at iteration entry:
// unsigned ordinal 211/211/209/209; signed ordinal 259/259/263/275. All restored;
// the best three-point change does not qualify. Count/stride/prior and SIMD homes remain.
// On the common failure-result graph: signed word/native count at entry or after
// alias handling, each without/with shared copy extent: 212/289/211/211/303/225/212/212.
// One-point forms rejected; original count observations, helpers and body restored.
// Round 143: retail materialises the point-mask test as a 0/1 value (`neg; sbb; neg; je`) before
// the set/clear; SET_FLAG with TEST_FLAG / with a `(bool)` test 212/212, a named bool 212, `!!` 212,
// `? true : false` 212 (all simplified to `test`); SET_FLAG plus signed short index 252, plus the
// previous side not volatile 227, plus both 254.
// Round 59: retail keeps the index as a word in memory (+0x14, tested `cmp ax, [count]`), the
// previous point at +0x1c, the copy size at +0x18 and the plane reloaded from its argument home.
// Tried: the previous side's volatile removed 227; signed short index 252 (869 B; retail's loop
// tail, but the previous point stays in ESI); both 254 (frame +8, the widened index spilled);
// long index without the volatile 272; the distance sums with the terms swapped (loop, fraction,
// both) 212 (identical).
// Common-failure graph mask production: conditional result 212/252; clear then set
// 275/266; named output bit 222/252; named bit with conditional result 222/252
// (unsigned/signed word ordinal). Original ordinal, mask and body restored.
// Round 62: without the volatile the plane sits in ESI and the previous point spills as in retail,
// but the previous side stays in CL and the frame grows 8 (227); with a signed short index too it
// gives retail's loop tail but spills the widened index separately (254, 879 B); signed short alone
// 252; `(bool)` on the point-mask bit 212 (identical; retail materialises it with neg/sbb/neg),
// with no volatile and short index 254.
// Round 102: with the volatile: previous side as `= false; if (...) = true` 255, long index 266;
// without it: that if/else 250, the current side declared beside the previous one 227, the previous
// point through a named `count - 1` index 229 (harness: `points + count - 1` and a word-cast index
// 220/277, unchanged or worse; without the volatile the previous side lives in CL, retail's in memory).
// Round 105: the mask update as SET_FLAG(mask, result, point_mask ? TEST_FLAG(...) : false) (retail's
// materialised bit, `neg; sbb; neg`, with the NULL mask threaded to the clear arm; `&&` folds it to `test`)
// with a signed short index and the previous side's volatile removed 133, plus the previous distance
// named like the loop's 129 (kept); the bit as `(*point_mask >> i) & 1` alone 177; on 129: the fraction as
// an if/else 249 (XMM4/XMM5 constants as retail, flag still in CL); side flags byte/char/long/short,
// function-scope sides, swapped side test or loop-end copies, dot_product2d distances, a named
// `count - 1` or copy size, the current point after the distance: no gain.
// Round 17: entry previous-side direct assignment with signed/unsigned ordinal 254/227; entry previous
// pointer with signed ordinal 254; false-first entry side with signed/unsigned ordinal 272/246;
// previous-point lexical name 254; shared copy extent 265; output-mask lexical name with that extent
// 265. Original declarations, qualifier, ordinal, names and body restored.
// Round 267: On the native mask graph, prior true-first/false-first side results 132/130;
// current true-first/false-first 284/284; paired true/true, true/false, false/true,
// false/false results 284 each. Count ECX and prior-side byte home remain; original body restored.
// Round 366: previous-distance carry 156; two-use intersection pointer 129; both 156; shared 2-D difference 129, with distance/pointer/both 156/129/156; joint point cursor 262. All restored.
// Round 369: prior/current/both outside values 257/268/325; outside values with test-assigned fraction 253/266/316; both with negative accumulation 344; complementary side presence 134. All restored.
// Round 109: retail loads count once into ECX after the alias copy (`mov ecx, [count]; movsx edi, cx`,
// guard `test cx, cx`), so ECX is busy at entry and the previous side spills (+0xd, current +0xc);
// ours reads count from memory twice and keeps the previous side in CL. The alias test reversed 130;
// the clipped reset after the alias copy 220; mask declared first 132; plane n.j * dy 129; the
// previous side as `0 <= distance` 129 (compile-only screens of nine more orders and spellings: none).
// Round 262: equality-producer byte/short/long result domains and ordinary inline
// metadata 129 each; single-exit bool/byte/short/long equality results on the
// native-mask/signed-ordinal graph 129 each. Original helper and body restored.
// Round 256: Native short/native long point counts at entry/alias join, each with default/test-
// assigned fraction 129/249/133/251/296/298/254/259. All restored.
// Round 217: shared final-copy predicate with original/positive-first policy and copy
// after/before normalization 279/299/287/291; with reused byte extent 292/308/296/300. Original
// body restored; count ECX, prior-side byte and SIMD homes remain.
// DL196: coordinated equality parameter priorities 129/129/129/129/129;
// complete intersection and two priority interactions 255/255/255.
// Original executable body and all supporting contracts restored.
// Round 171: retail loads count once after the alias copy (`mov ecx,[ebp+8]` for the stride and
// `test cx,cx` for the guard), so ECX is busy at entry, the previous side stays in memory and the
// loop head gets ECX for `points`; ours keeps the previous side in CL (loop end one store short).
// Every operand order of both distances, the deltas, the denominator, the numerator and the outputs
// compiles identically (120-form screen); mask first 132; side flags before the result 132; the
// fraction default before the deltas 240 (880 bytes); one named copy size 129; one inline
// append-and-dedupe helper for both tails 129; `points + count - 1` / `&points[count] - 1` 129.
// Round 453: Plain current-point cursor with native/memcpy/memmove publication 280/280/306;
// original indexed walk with point memcpy/memmove 129/337. Native intersection on the cursor
// graph with native/memcpy append 235/235; complete memmove publications 298.
// All original indexed traversal, native stores, copies and helper contracts restored.
// @retail 0x23a6f0
short function_23a6f0(point2f *output, short count, point2f const *points,
	plane2f const *plane, short capacity, dword *point_mask, bool *clipped, real epsilon)
{
	point2f copy[512];
	short result = 0;
	bool positive = false;
	bool negative = false;
	dword mask = 0;
	if (clipped)
		*clipped = false;
	if (points == output)
	{
		memcpy(copy, points, count * sizeof(point2f));
		points = copy;
	}
	point2f const *previous = &points[count - 1];
	real previous_distance = previous->y * plane->n.j + previous->x * plane->n.i - plane->d;
	bool previous_inside = previous_distance >= 0.f;
	for (short i = 0; i < count; i++)
	{
		point2f const *current = &points[i];
		real distance = current->y * plane->n.j + plane->n.i * current->x - plane->d;
		bool inside = distance >= 0.f;
		if (distance > epsilon)
			positive = true;
		else if (0.f - epsilon > distance)
			negative = true;
		if (inside != previous_inside)
		{
			if (result == capacity)
			{
				result = NONE;
				break;
			}
			if (clipped)
				*clipped = true;
			real dy = previous->y - current->y;
			real dx = previous->x - current->x;
			real denominator = plane->n.i * dx + dy * plane->n.j;
			real fraction = 0.f;
			if (denominator != 0.f)
				fraction = 0.f - (current->y * plane->n.j + plane->n.i * current->x - plane->d) / denominator;
			fraction = fraction < 0.f ? 0.f : fraction > 1.f ? 1.f : fraction;
			output[result].x = dx * fraction + current->x;
			output[result].y = dy * fraction + current->y;
			mask |= FLAG(result);
			result++;
			if (result != 1 && (clip_points_equal_23a6f0(&output[result - 1], output, epsilon) ||
				clip_points_equal_23a6f0(&output[result - 1], &output[result - 2], epsilon)))
				result--;
		}
		if (inside)
		{
			if (result == capacity)
			{
				result = NONE;
				break;
			}
			output[result] = *current;
			SET_FLAG(mask, result, point_mask ? TEST_FLAG(*point_mask, i) : false);
			result++;
			if (result != 1 && (clip_points_equal_23a6f0(&output[result - 1], output, epsilon) ||
				clip_points_equal_23a6f0(&output[result - 1], &output[result - 2], epsilon)))
				result--;
		}
		previous = current;
		previous_inside = inside;
	}
	if (result != NONE)
	{
		if (result < 3)
			result = 0;
		if (!positive)
			result = 0;
		else if (!negative)
		{
			result = count;
			memcpy(output, points, count * sizeof(point2f));
		}
	}
	else
		memcpy(output, points, count * sizeof(point2f));
	if (point_mask)
		*point_mask = mask;
	return result;
}

// @retail 0x23a2b0
short function_23a2b0(short count, point2f const *points, short clip_count,
    point2f const *clip, short capacity, point2f *output, real epsilon)
{
    point2f buffers[2][512];
    for (short i = 0; i < clip_count && count > 0; i++)
    {
        short previous = i ? i - 1 : clip_count - 1;
        point2f *destination = i == clip_count - 1 ? output : buffers[i & 1];
        plane2f plane;
        plane.n.i = clip[previous].y - clip[i].y;
        plane.n.j = clip[i].x - clip[previous].x;
        real length = (real)sqrt(plane.n.i * plane.n.i + plane.n.j * plane.n.j);
        if (!(fabs(length) < 0.0001f))
        {
            real inverse = 1.f / length;
            plane.n.i *= inverse;
            plane.n.j *= inverse;
        }
        else
            length = 0.f;
        if (length != 0.f)
        {
            plane.d = plane.n.i * clip[i].x + plane.n.j * clip[i].y;
            count = function_23a430(count, points, &plane, epsilon, capacity, destination);
            if (count == NONE)
                return NONE;
        }
        else
        {
            plane.d = 0.f;
            memcpy(destination, points, count * sizeof(point2f));
        }
        points = destination;
    }
    return count;
}
