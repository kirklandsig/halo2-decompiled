/* UNKNOWN_1946F0.CPP: the bit stream module's codecs that retail compiled
   with the small helpers (a boolean write, vector normalization, the
   direction decode) expanded inline. The stream primitives they call are in
   unknown_195720.cpp */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "bitstream.h"
#include "unknown_1946f0.h"
#include "normalize3d.h"
#include "slot_handler.h"
#include <math.h>

static __forceinline long real_to_long(real value);
#include <string.h>

// @flags /O2 /Gr /arch:SSE

s_direction_face g_475480[32];

/* the decode path's normalization: the length of the stored components,
   then each component scaled by its inverse */
static __forceinline real normalize_decoded_direction(vector3f *v)
{
	real m = (real)sqrt(v->k * v->k + (v->i * v->i + v->j * v->j));
	if (!(fabs(m) < k_real_epsilon))
	{
		real inv = 1.f / m;
		v->i = inv * v->i;
		v->j = inv * v->j;
		v->k = v->k * inv;
		return m;
	}
	return 0.f;
}

// @retail 0x194830
void function_194830(s_bitstream *stream, bool value)
{
	if (stream->size_in_bytes * 8 - stream->bit_position >= 1 && value)
		stream->data[stream->bit_position / 8] |= (byte)(1 << (stream->bit_position % 8));
	stream->bit_position++;
}

// @retail 0x194c10
void function_194c10(s_bitstream *stream, vector3f const *vector, real lo, real hi, long bits)
{
	vector3f direction = *vector;
	real magnitude = normalize3d(&direction);
	if (magnitude > hi)
		magnitude = hi;
	if (magnitude >= lo)
	{
		function_194830(stream, false);
		function_194bc0(stream, &direction);
		function_194b60(stream, magnitude, lo, hi, bits);
	}
	else
		function_194830(stream, true);
}

/* the direction an index names: a 64 x 64 grid point on one face of the
   direction table, then that face's scale, axes and origin applied to it */
static __forceinline void decode_direction(long index, vector3f *direction)
{
	point3f grid;
	grid.x = (real)(index & 0x3f) * (1.f / 63.f);
	grid.y = (real)((index >> 6) & 0x3f) * (1.f / 63.f);
	grid.x = grid.x * 2.f;
	grid.y = grid.y * 2.f;
	grid.z = 0.f;
	real x = grid.x;
	real y = grid.y;
	real z = grid.z;
	s_direction_face const *face = &g_475480[index >> 12];
	if (face->scale != 1.f)
	{
		x = face->scale * x;
		y = face->scale * y;
		z = face->scale * z;
	}
	direction->i = z * face->axes[2].i + y * face->axes[1].i + x * face->axes[0].i + face->origin.i;
	direction->j = z * face->axes[2].j + y * face->axes[1].j + x * face->axes[0].j + face->origin.j;
	direction->k = z * face->axes[2].k + y * face->axes[1].k + x * face->axes[0].k + face->origin.k;
}

// score 4 (was 4): what still differs is the magnitude sum's first pair: ours sums j*j + i*i, then k*k,
// retail j*j + k*k, then i*i (the decoded k is now popped and reloaded as in retail). Round 127: the
// file's decode-path normalizer (normalize_decoded_direction, grouped k*k + (i*i + j*j)) in place of
// normalize3d, 47 -> 4 (0x195070 56 -> 6); on it: the groupings i*i + (j*j + k*k) (0x294050's
// magnitude form) and j*j + (i*i + k*k) (normalize_inline's) 47/47 (k kept on x87 again; a written
// inner order makes no difference); the normalization inside the decoder 4; the inner pair written
// j*j + i*i 4 (same code); the magnitude as its own inline helper inside the normalizer 4;
// diagnostics: extra no-op products after the normalization 4, before it (with i*i + (j*j + k*k))
// 91/131/145 (k still kept and first); a branch before the normalization with the grouped forms
// 204 (the decode moves to SSE stores).
// Earlier (score 47): what still differs is the normalization: ours keeps the decoded k on the x87
// what still differs is the normalization: ours keeps the decoded k on the x87
// stack (fst) and squares it first; retail pops it and sums j*j, k*k, then i*i from memory, as if a
// block ended after the decode (a diagnostic branch there gives retail's order). These sum orders
// follow VC's tuple numbering, not the source order. Round 13: the decoder builds its grid point
// as a point3f and reads it back into x, y and z (and 0x195070 shares it): its constants now
// match retail (the zero made first in xmm3, 2.0 and 1.0 loaded into xmm2 at their uses), 61 -> 47.
// Before that (cause 1), retail's decoder acted as if a block ended after the two 1/63 products: a
// one-pass loop around them (diagnostic only) gave retail's schedule, 51.
// two causes. (1) The decoder's constants: retail makes the zero first (xmm3)
// and loads 2.0 and 1.0 into xmm2 at their uses; ours loads 2.0 early into xmm3 and makes the zero
// late. Retail's decoder acts as if a block ends after the two 1/63 products: a one-pass loop around
// them (diagnostic only, not kept) gives retail's schedule, 51. (2) Ours keeps the decoded k on x87
// (fst) and squares it first; retail pops it and sums j*j, k*k, then i*i from memory. A block boundary
// before the normalization (diagnostic branch) makes ours reload in retail's order. These sum orders
// follow VC's tuple numbering, not the source order.
// Round 78: the rounding through the existing real_to_long (forceinline declaration, as ai.cpp does)
// in place of the hand-written fld/fistp block 62 -> 61 (kept: the sum is now k, j, i and j scales as
// retail's j * inv); on it: a two-pass grid loop 124 (VC keeps the loop); normalize_inline 61;
// normalize_decoded_direction 61; compound doubling 61; the angle call inside the rounding 101;
// decoder parameters (output, index) 61; before it: normalize_inline 62; grid doubling written first in
// one expression 62; named face index 62.
// Tried: shared face/grid scales 96; initialized grid 133; shared scale value 96; complete output
// aggregate/memcpy 155; default memcpy and direct angle 62; coordinate stages 62; selected scale
// results 62; plane offset first 100; axis table 62; origin snapshot 129; table subobjects 129;
// coordinate names 62; existing normalizer call 187; face scale value 82; integer grid values 62;
// complete shared decoder 503; decoder output reference 62; native component loop 173; complete
// private decode consumer 62; ordinary decoder inline 394; initial direction value 63; workspace
// names, formatter scope, unsigned index, complete bounds, shared rounding, shared/existing decoded
// normalization, completed decode output and decoder pointer/vector results 62, 62, 62, 122, 72,
// 62, 62, 88, 62, 155; sum orders j+k+i, i+(j+k), k+j+i, i+k+j 62 each; decoder stores through the
// n[] view 62 (with i+(j+k) 62); zero declared first in the decoder 84; grid divided by 63 62; the
// existing real_to_long for the angle (plain inline) 73; grid point passed to an inline point
// transform 62; step count as a decoder parameter 62; point2f grid scaled in place 68; a compiled-out
// `do {} while (false)` between the decode and the normalization 62 (identical code: no join); zero
// after y / between x and y / after the face pointer 73/81/62; the decoder returning its output
// pointer into the normalization 62. Round 20: constant-first doubling 61; `x *= face->scale` 61;
// face pointer before the grid values 81; the equal-direction arm first 61; zero between the grid
// products and the doubling 72; the rows written in both scale arms 224 (no tail merge).
// pointer into the normalization 62.
// Round 13, on the grid decoder (harness, the region scored alone): normalize_inline (sums k, i,
// then j) and normalize_decoded_direction no gain; the rows into scalars, a result local copied
// out, a pointer return into the normalizer, a caller-side copy of the decoded vector, an axis
// loop for the rows, the rows reordered: none reloads; the decoder as an address-less plain or
// `inline` function in unknown_195720.cpp is not inlined (393, two callers lost; `inline` leaves an
// unresolved call); `__forceinline` there is untested (its reduced image lacked that file).
// Round 119: the 0x195070 decoder (same decode, retail hoists 2.0 there and makes the zero late, as
// ours does here) shows the block after the 1/63 products is particular to this caller. A two-pass
// grid loop shifting the index by six, the zero first 130; the same with `index >> (6 * n)` 134; the
// face transform as a three-component loop 172 (not unrolled); the grid through an inline
// `(real)value / 63.f` helper 114 (599 bytes); the zero first and the face pointer before the
// doubling 87.
// pointer into the normalization 62. Round 80: the file's decoded-direction normalizer with its sum in
// i, j, k order (here and/or in 0x195070) 61 (the reduced build keeps the decoded k on x87 either way;
// an LTCG harness of the file alone popped it); the face pointer before the grid 81; y before x 75;
// compound face scaling 61.
// Round 34: face pointer and zero before the grid values 87; the three rows into locals, then the
// stores 61; constant-first grid products and doubling 61; y before x 71; `1.f != face->scale` 61.
// pointer into the normalization 62. Round 20: constant-first doubling 61; `x *= face->scale` 61;
// face pointer before the grid values 81; the equal-direction arm first 61; zero between the grid
// products and the doubling 72; the rows written in both scale arms 224 (no tail merge).
// DL201: shared squared length with sqrtf and fabs/fabsf, dot/length forms 87/87/87/87;
// double magnitude with sqrt/sqrtf 118/118. Out-of-line magnitude query 118/118
// is unresolved and excluded. Ordered two-sided magnitude tests, dot/length 92/92.
// Original normalization restored; final-component forwarding remains.
// Round 51 (compile-only, which reproduces both sums): the commutative x87 order follows the order
// the direction's fields are first assigned (a flat sum runs in the reverse of it; k stays on the x87
// stack only when assigned last), and retail's orders need k assigned before j here (i, k, j with the
// flat dot3f sum) and before i in 0x195070 (j, k, i), but the decode assigns i, j, k. Sums i+(j+k),
// (j+k)+i, flat ijk/jki, (i+k)+j, j+(i+k): k kept and first in each; with a diagnostic block boundary
// k is reloaded but still first in its pair; decode rows in other orders move the stores; the
// normalizer's scale statements in all six orders: identical; equal arm first with the struct copy:
// k kept; decode returning the vector by value: extra copies; the existing normalize3d in both
// decoders 47 here, 177 in 0x195070 (384 bytes).
// Round 213: existing default vector setter 140; setter with equal arm first 140; native fallback value
// 185; native fallback with equal arm first 185; named angle offset 86; named scaled angle 86; named
// quantized angle 86; named offset and scale 4; original executable source restored.
// @retail 0x194d30
void function_194d30(s_bitstream *stream, vector3f const *up, vector3f const *forward)
{
	vector3f direction;
	if (!(fabs(forward->i - g_4687b0->i) < k_real_epsilon && fabs(forward->j - g_4687b0->j) < k_real_epsilon && fabs(forward->k - g_4687b0->k) < k_real_epsilon))
	{
		long index = function_24f590(forward);
		function_194830(stream, false);
		if ((dword)index >= k_direction_limit)
		{
			char message[256];
			message[0] = 0;
			csprintf_256(message, "%u exceeds max value of %u", index, k_direction_limit);
		}
		function_195720(stream, index, k_direction_bits);
		decode_direction(index, &direction);
		normalize_decoded_direction(&direction);
	}
	else
	{
		function_194830(stream, true);
		direction = *g_4687b0;
	}
	real angle = function_1949b0(&direction, up);
	function_195720(stream, real_to_long((angle - -k_pi) * 40.42535400390625f), 8);
}

// score 6 (was 56): only the magnitude sum's order differs: ours j*j + i*i, then k*k; retail i*i +
// k*k, then j*j. Round 127: normalize_decoded_direction's sum grouped k*k + (i*i + j*j) (shared with
// 0x194d30) 56 -> 6 at retail's 449 bytes; i*i + (j*j + k*k) and j*j + (i*i + k*k) 56/56 (k kept on
// x87); the normalization inside the shared decoder 6.
// Earlier (score 56): only the normalization differs, as in 0x194d30: ours keeps the decoded k on
// the x87 stack and squares it first; retail pops it and sums i, k, j from memory (a block boundary
// before the normalization), then scales the normalized vector in place. Round 13: the shared grid
// decoder in place of this function's hand-written copy of it, 96 -> 56. Earlier, on that copy:
// single-expression grid values 126 (1/63*2 folds into one constant); x finished before y 96;
// `vector3f_scale` with the magnitude call 96; the old decoder helper 114 (aligned 34 -> 26); that
// helper as plain inline 344 (not inlined). Plain component loops (rows/normalization/output in
// seven subsets) 114/96/114/96/96/96/96; all with complete normalized value 155 at 489 bytes.
// Row/output forms are 440 bytes; no count gain or missing store/reload recovery.
// retail keeps 1/63 in xmm2 and 2 in xmm3 and finishes x (both products) before
// converting y; ours keeps 1/63 in xmm0 and scales x and y by two late. Retail also pops the decoded k
// and sums i, k, j from memory (a block boundary before the normalization), and scales the normalized
// vector in place. Single-expression grid values 126 (1/63*2 folds into one constant); x finished
// before y 96; `vector3f_scale` with the magnitude call 96; the existing decode_direction_194d30
// helper 114 (aligned 34 -> 26: the final scale then matches retail and the hand-written decode copy
// goes; the decode registers and x87 reload remain); that helper as plain inline 344 (not inlined).
// Plain component loops (rows/normalization/output in seven subsets) 114/96/114/96/96/96/96; all
// with complete normalized value 155 at 489 bytes. Row/output forms are 440 bytes; no count gain or
// missing store/reload recovery. Original body and normalizer restored.
// DL190: Whole face value/native output stores, setter, aggregate 175/175/205; memcpy face with the same boundaries 175/175/205. Original face with setter/aggregate 96/136. Complete face snapshot adds spills; grid reciprocal, last-component reload and output carriers remain. All restored.
// Round 434: complete scalar normalizer inputs with sqrt/fabs, sqrt/fabsf, sqrtf/fabs,
// sqrtf/fabsf 32 each; const input/output snapshots with those APIs 56 each. The
// components passed as scalar parameters in the orders ikj/jik/jki/kij/kji 28/34/32/55/55,
// output-first ijk/ikj 32/28 (not kept: a parameter order chosen to steer the loads). On ikj,
// shared output scale/reversed products/direct magnitude query 28/28/97; in-place copy/native
// scaled value 58/58. Original normalizer restored.
// @retail 0x195070
void function_195070(s_bitstream *stream, vector3f *out, real lo, real hi, long bits)
{
	if (!function_1957d0(stream))
	{
		long index = (long)function_1959c0(stream, k_direction_bits);
		vector3f direction;
		decode_direction(index, &direction);
		normalize_decoded_direction(&direction);
		real magnitude = function_194ff0(stream, lo, hi, bits);
		out->i = magnitude * direction.i;
		out->j = magnitude * direction.j;
		out->k = magnitude * direction.k;
	}
	else
		*out = *g_4687a4;
}

// @retail 0x195370
bool __fastcall function_195370(vector3f const *a, vector3f const *b, real lo, real hi, long bits)
{
	vector3f direction_a = *a;
	vector3f direction_b = *b;
	real magnitude_a = normalize3d(&direction_a);
	real magnitude_b = normalize3d(&direction_b);
	bool short_a = lo > magnitude_a;
	bool short_b = lo > magnitude_b;
	bool near_a = !short_a && function_1952f0(magnitude_a, lo, lo, hi, bits);
	bool near_b = !short_b && function_1952f0(magnitude_b, lo, lo, hi, bits);
	if (!short_a || !short_b)
	{
		if (short_a || short_b)
		{
			bool other_near = short_a ? near_b : near_a;
			if (!other_near)
				return false;
		}
		if (!direction_a.quantized_equal(&direction_b) || !function_1952f0(magnitude_a, magnitude_b, lo, hi, bits))
			return false;
	}
	return true;
}
