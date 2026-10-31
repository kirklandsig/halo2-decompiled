// @flags /O2 /Gr /arch:SSE
#include "unknown_2551c0.h"
#include "globals.h"
#include "unknown_293070.h"
#include <math.h>

struct s_293130
{
	short field_0;
	short field_2;
	byte field_4[0xc];
	short field_10;
	byte field_12[0x16];
};

struct s_293131
{
	point3f field_0;
	real field_c;
	byte field_10[4];
	real field_14;
	real field_18;
};

struct s_293132
{
	byte field_0[0xc];
	long field_c;
	s_293131 *field_10;
	long field_14;
	byte field_18[8];
	real field_20;
	real field_24;
	byte field_28[4];
	long field_2c;
	byte field_30[0x54];
};

struct s_293133
{
	byte field_0[0x350];
	long field_350;
	s_293132 *field_354;
};

struct s_293134
{
	byte field_0[0x1c];
	point3f field_1c;
	vector3f field_28;
	byte field_34[0x24];
	real field_58;
	byte field_5c[0x58];
	word field_b4;
	word field_b6;
	byte field_b8[0xc];
};

struct s_293135
{
	long field_0;
	short field_4;
	byte field_6[6];
	bool field_c;
	bool field_d;
	bool field_e;
	byte field_f;
	short field_10;
	short field_12;
	long field_14;
	short field_18;
	short field_1a;
	byte field_1c[0xc];
	point3f field_28;
};

struct s_effect_owner;
extern s_record_pool *g_51ecb4;
void function_b7930(void *arg_0, long arg_1, long arg_2, s_effect_owner const *arg_3);
long __stdcall function_b7b40(void *arg_0);
void __stdcall function_b8540(long arg_0);
bool function_2936b0(long arg_0, long arg_1);

__forceinline void function_293133(real arg_0, vector3f const *arg_1, vector3f *arg_2)
{
	real local_0 = (real)sin(arg_0);
	real local_1 = (real)cos(arg_0);
	real vector_i = arg_2->i;
	real vector_j = arg_2->j;
	real vector_k = arg_2->k;
	real local_2 = dot3f(arg_1, arg_2) * (1.0f - local_1);
	real local_3 = arg_1->k * vector_j - vector_k * arg_1->j;
	real local_4 = vector_k * arg_1->i - arg_1->k * vector_i;
	real local_5 = vector_i * arg_1->j - arg_1->i * vector_j;
	real cosine_term_i = vector_i * local_1;
	real axis_term_i = arg_1->i * local_2;
	real cross_term_i = local_3 * local_0;
	real projected_term_i = cosine_term_i + axis_term_i;
	arg_2->i = projected_term_i - cross_term_i;
	real cosine_term_j = vector_j * local_1;
	real axis_term_j = arg_1->j * local_2;
	real cross_term_j = local_4 * local_0;
	real projected_term_j = cosine_term_j + axis_term_j;
	arg_2->j = projected_term_j - cross_term_j;
	real cosine_term_k = vector_k * local_1;
	real axis_term_k = arg_1->k * local_2;
	real cross_term_k = local_5 * local_0;
	real projected_term_k = cosine_term_k + axis_term_k;
	arg_2->k = projected_term_k - cross_term_k;
}

// score 30 (was 30): frame 0xd4, length and range adds agree; the rotation's dot sums the j and k
// products before i (retail i and k, then j) with v.j/v.k in each other's registers, and the +0x18
// sentinel store with the argument reload is scheduled early.
// Earlier at 43: frame 0xd4, length and all range adds agree; rotation dot/register ranks and final sentinel scheduling remain.
// This round's sentinel store orders (the +0x18 word before the bytes, after the
// +0x1a word, the bytes swapped, the bytes written as numbers, the words cast to short) all compile
// to the same 43, so the store order is the compiler's scheduling, not the source's.
// This round ran the source permuter over the body; the rotation's xmm ranking
// (retail's xmm1..xmm5 cycle against ours) and the final sentinel store order still differ.
// Permuter round at 43 (from 43): the source permuter over the body; the rotation's xmm ranking
// Earlier at 43 (from 43): frame 0xd4, length and all range adds agree; rotation dot/register ranks and final sentinel scheduling remain.
// Earlier: y/z range-add order, rotation registers and final sentinel
// three causes remain. The y/z point draws add the point component before the range
// start (x is right); the rotation's xmm registers are retail's shifted by one (xmm1..xmm5 cycle) with
// two products scheduled the other way; the arg_0 reload for 0x2936b0 and the +0x18 NONE store are
// hoisted above the +0xc..+0x10 stores. Frame, result, conversion and sine homes agree. Kept (round
// 253): separate total and threshold, 68 -> 52. Tried: random_index for both index draws 68 (identical
// code); function_x82e52f for the weighted fraction 84; both 143; function_259d0 for the four range
// draws 140, with random_index 137; all three helpers 78 (adds as retail, the rotation then ranks j
// where retail ranks i); on that: dot3f(vector, axis) 78, the rotated vector in a local copy 193, the
// angle passed straight 87, the angle read after the copy 87, cosine before sine 84; angle passed
// straight 79 (80 on its own; function_x82e52f alone 145); fraction without its local 125; (1 - cos)
// first 68; count inline 68; dot operands swapped 68; named 1 - cos 68; named dot 126; the z flag
// inlined 325; the handler pointer through an if 111; the slot pointer as a sum 68; descriptive helper
// names 68; result renamed 68; result declared first 69; result before the workspace 69; workspace
// after the lookups 68; weight sum before the cumulative sum 70; cumulative sum only in the weighted
// branch 361; one loop index for the three loops 273; result set on both validity paths 123; a
// transient creation handle with range result, trig input and shared cross 357-364; coherent
// selected-radius inputs 229-294. On the total/threshold form: word draw/long choice/both 470/248/463,
// cumulative-scope counterparts 361/470/301/463; function_259d0 for the four draws 113, all shared
// random helpers 53 (draws as retail, dot then j, k, i; with the rotation parameters reordered or
// taking sine/cosine 53); function_259d0 for the three point draws 65; the rotation helper taking sine
// and cosine 130, with them named before the call 123 (dot i, j, k); the point component first in the
// sums 52; the rotation helper as plain static inline 52; its parameters as (vector, axis, angle) 52.
// Diagnostic only: the range helper's result cast to real 48 (draws as retail, rotation unchanged).
// y/z range-add order, rotation registers and final sentinel
// scheduling differ. Result, conversion and sine stack homes now agree.
// retail packs the random-to-float temp at frame +8 and the
// result at +0xc (the sine shares +0 with the first sum); ours swaps those two
// slots. Ours also adds the field before the range start in the three point
// draws, colours the rotation's xmm registers differently and stores +0x18
// ahead of the +0xc..+0x10 stores.
// Tried: random_index for both index draws 68 (identical code); function_x82e52f
// for the weighted fraction 84; both 143; function_259d0 for the four range
// draws 140, with random_index 137; all three helpers 78 (adds as retail, the
// rotation then ranks j where retail ranks i); on that: dot3f(vector, axis) 78,
// the rotated vector in a local copy 193, the angle passed straight 87, the
// angle read after the copy 87, cosine before sine 84.
// four causes. Frame: retail packs the random-to-float temp at +8 and the
// result, conversion and sine stack homes differ; rotation
// registers also differ.
// Transient creation handle with range result, trig input and shared cross boundaries:
// 361/358/360/357/360/357/359/364. Result, conversion and sine homes plus rotation registers
// remain.
// The shared random helpers (function_259d0 for the four draws, function_x82e52f for the
// fraction, random_index for both index draws) 68 at 1409 bytes: the three draw sums then
// add as retail, but the rotation's dot turns j,i,k (retail i,k,j). On that form: angle
// passed straight 79; fraction without its local 125 (dot as retail, fraction product
// reassociated); (1 - cos) first 68; count inline 68; dot operands swapped 68; named
// 1 - cos 68; named dot 126; the z flag inlined 325; the handler pointer through an if 111;
// the slot pointer as a sum 68; descriptive helper names 68 (also on the old form, 68).
// result at +0xc (the sine temp shares +0 with the first sum), ours the result at +8 (sharing it
// with the sine temp) and the random temp at +0xc. Ours reassociates `range draw + base` (adds
// the base first); the rotation's xmm colouring differs; the scheduler hoists the 0x18 NONE store.
// Round 75: the existing random-range function (0x259d0) for the four draws 140 (fixes the three
// fadd orders, reorders the rotation); result renamed object_index/result/zz_index/aa_index 68 each
// (slot unchanged); result declared first 69; existing random real for the fraction 145; angle
// field passed straight to the rotation 80.
// Round 253: Separate total/threshold 52; with word draw/long choice/both 470/248/463;
// cumulative-scope counterparts 361/470/301/463. On the recovered graph, shared range/all random
// helpers 113/53. Keep ordinary-confirmed total/threshold split only: 68 to 52 at retail length,
// scalar homes agree; remaining y/z adds, rotation ranks and sentinel scheduling.
// Round 289: Coherent selected radius, radius/negative, two reused bounds, complete selected row; each with shared double trig input:
// [247, 243, 242, 294, 250, 229, 229, 285]. All executable source and temporary types restored.
// Round 186: Native member lookup and shared random APIs, with normalized raw-draw arithmetic at the
// fraction producer; cosine/axis/cross products and the cosine-plus-axis sum named in all three rotation
// rows: 43 from 52 at 1405 bytes. Shared/staged cross 55/55; named range return 65, with cross 71/71;
// shared random/cross 69/69; full outputs 157. Native member getter via handler/header 57/57, byte flag 57;
// with shared random 48. Complete-row parts 123/117/53/111/71/60/60/43; double trig/commuted dot/both/byte
// flag 60/43/60/43. Fraction raw-draw/seed-step normalization 43/43; original raw draw without cast 60;
// direct real-fraction operand 62. The direct fraction-call copy at 43 was dropped. Arithmetic grouping,
// draw order and game-call tuples stay fixed.
// Round 191: native sentinel/zero/both chains 47/44/48; complete helper axis/source/both
// input captures 211/186/242; axis with sentinel chain 211; both with all chains 242.
// All restored; rotation input/register ranking and final sentinel scheduling remain.
// Round 82: projection factor first 43; projection declared before trig 43; angle declared before draws 43;
// output-returning rotation 43; cosine first 55; output/axis/angle parameters 43; i projected sum axis-first 43;
// named dot and cosine complement 60 (1409 bytes). All restored; the dot order and sentinel schedule remain.
// Round 127 (lane 4): the dot written grouped j + (i + k) 42 (it then sums i, k, then j as retail, but
// ours makes the k product the accumulator where retail makes the i product); on it: inner pair as
// (k + i) or the products' operands swapped 42 (same code); the rows without named terms 42 (same
// code); sine/cosine named in the caller and passed in 42; constant-first or named (1 - cos) 42/42;
// point component first in the x draw 42; cross terms before the dot 63; a named radius for the
// three point draws 254; the angle passed straight 54; the threshold drawn through function_x82e52f
// 61. Groupings i + (j + k) 53 and k + (i + j) 114; on the flat dot: cross terms first 66, the helper
// taking sine and cosine 55 (cosine evaluated first), the parameter read through its address for
// 0x2936b0 43 (same code). Diagnostics only: extra no-op products cycle the flat dot through four
// states with period 4 (43/60/53/111), none with i in the first pair; on the grouped dot one extra
// product before the dot gives retail's i accumulator (59, with the extra code).
// Round 47: dot written in 0x2922b0's i, k, j term order (vector-first/axis-first) 60/60 (1409
// bytes; still k, j, i); +0x18 sentinel stored after +0x1a 43; the four flag/zero stores before
// the NONE stores 43; the angle read after the vector copy 54; PRIVATE helper 43 (same code);
// permuter 49 tries, best 43.
// Round 31 (lane 10): the only structural rotation difference is the dot's sum order (retail
// (a.i*v.i + a.k*v.k) + v.j*a.j, ours (a.k*v.k + a.j*v.j) + a.i*v.i); the xmm numbering follows from
// it. Every dot spelling (12 term/operand orders, both dot3f orders, (1 - cos) first) and all 120
// sine/cosine/projection/cross statement orders keep it (one-file LTCG harness and compile-only both
// reproduce our block). It is a whole-function effect: compile-only, deleting the y and z draws,
// the selection loops or the tail flips it (x draw alone gives retail's order), while declaration
// orders, in-for indices, ++/+= forms, if/ternary/continue rewrites and draw spellings (point first,
// named radius, copy then +=, inverted or ternary z) do not. Real trials: 0x194a10's helper body with
// the copy between sine and cosine 53 (1409 bytes); v.i and v.j named in the helper 30 (only two of
// three components: not keepable); all three named 43; a.i and v.j named 87; a.i and a.k named 149
// (both give retail's dot order but keep a.i in a register); cross-file rotation helpers
// (0x170d70's, rotate_direction, biped_rotate_about, 0x26db50's) keep our order (harness).
// Compile-only, 300 random sets of 28 code-identical respellings (++/+=, a + i, != 0, -1 for NONE,
// reversed compares, chained stores, g[0]) and seven tail forms (init order, random count inline,
// else first) all keep it.
// Round 10 (m3 lane 2): register-masked aligned difference 8: the dot's (i + k) + j schedule and
// the +0x18 sentinel store with the argument reload, which ours schedules right after the last
// NONE store where retail keeps source order. Member vector copied before the sentinels 91; the
// true flag stored before the two false ones 42 (the +0x18 store then lands after the two false
// ones, still ahead of the rest); the two false flags as one chained assignment 43; grouped dot
// with the vector copied first 90; grouped dot with the true flag first 41 (masked difference 5).
// Round 33 (lane 10): the rotated vector's three components named once in the helper (vector_i/j/k,
// used by the cross and cosine terms; dot3f unchanged) 30 (kept) in the orders i,j,k / j,i,k /
// j,k,i, 111 with k named before j (i,k,j / k,i,j / k,j,i). On the 30 form: dot3f(vector, axis) 30;
// (1 - cos) first 30; the names declared before the sine 30, after the dot 111; cosine before
// sine 50; the axis components named too 169 (1424 bytes). Caveat for review: the helper overwrites
// the vector row by row, so the names snapshot its input, but dot3f still reads the same fields
// through the pointer (writing dot3f out by hand is not allowed; a whole-vector copy scored 193).
// Round 108 (lane 6): the helper's shape is the classic rotate-about-axis (dot(axis, vector) times
// 1 - cos, the same three cross terms, rows cos*v + k*axis - sin*cross), so only colouring is left.
// The cross terms before the dot 66 (dot then sums j, k, i); the dot grouped (i + k) + j 42 (retail's
// sum order, but ours makes the k product the accumulator: v.k in XMM4 where retail has XMM1); on
// it: the rows as one expression each 42 (same code), the helper returning the vector 42, a vector
// pointer local in the caller for the copy and the call 118, (vector, axis, sine, cosine) parameters
// with sine and cosine named in the caller 42, the grouped dot named before the product 42.
// Lane 16 r50 (fast): the +0x18 NONE store written after the +0x14 one 30 (same code); the member
// mask read once into a short for the gate and the three selection loops 406 (1368 B); the two
// creation-record stores swapped 32; `random_fraction * total_weight` 30 (same code); the angle
// passed straight after the vector copy 41. Permuter skipped (full-image build over 15 minutes).
// Round 33 (m3 lane 11), fast: the rotation rows written k, j, i 52 and j, i, k 54; the cross
// components computed 5, 4, 3 41 and 4, 5, 3 30 (same code); the cross components written inside
// the rows (no local_3/4/5) 92 (1407 bytes). All restored.
// @retail 0x293130
long __stdcall function_293130(long arg_0)
{
	s_293134 local_13;
	s_293130 *local_2 = &((s_293130 *)g_51ecb4->data)[arg_0 & 0xffff];
	s_293132 *local_3 = &((s_293133 *)g_4e0350)->field_354[local_2->field_2];
	long local_0 = NONE;
	if (local_3->field_2c != NONE && local_3->field_c > 0 && local_2->field_10 > 0)
	{
		real local_5 = 0.0f;
		real total_weight = 0.0f;
		short local_6 = 0;
		short local_7;
		short local_16;
		short local_17;
		short local_8 = NONE;
		for (local_7 = 0; local_7 < local_3->field_c; local_7++)
		{
			if (local_2->field_10 & (1 << local_7))
			{
				total_weight += local_3->field_10[local_7].field_18;
				local_6++;
			}
		}
		if (total_weight > 0.0f)
		{
			real random_fraction = _random(&g_4e7408->unknown0, NULL, 0) * (1.0f / 65535.0f);
			real selection_weight = total_weight * random_fraction;
			for (local_16 = 0; local_16 < local_3->field_c; local_16++)
			{
				if (local_2->field_10 & (1 << local_16))
				{
					local_5 += local_3->field_10[local_16].field_18;
					if (local_5 >= selection_weight)
					{
						local_8 = local_16;
						break;
					}
				}
			}
		}
		else if (local_6 > 0)
		{
			short local_9 = random_index(&g_4e7408->unknown0, local_6);
			short local_10 = 0;
			for (local_17 = 0; local_17 < local_3->field_c; local_17++)
			{
				if (local_2->field_10 & (1 << local_17))
				{
					if (local_9 == local_10)
					{
						local_8 = local_17;
						break;
					}
					local_10++;
				}
			}
		}
		if (local_8 != NONE)
		{
			s_293131 *local_11 = &local_3->field_10[local_8];
			bool local_12 = !((*(dword *)(g_4e3b44[local_3->field_2c & 0xffff].bytes + 0xd4) >> 4) & 1);
			function_b7930(&local_13, local_3->field_2c, NONE, NULL);
			local_13.field_b4 = 1;
			local_13.field_b6 = 0x34;
			if (local_3->field_20 > 0.0f && local_3->field_24 > 0.0f)
				local_13.field_58 = function_259d0(&g_4e7408->unknown0, NULL, 0, local_3->field_20, local_3->field_24);
			local_13.field_1c.x = function_259d0(&g_4e7408->unknown0, NULL, 0, -local_11->field_14, local_11->field_14) + local_11->field_0.x;
			local_13.field_1c.y = function_259d0(&g_4e7408->unknown0, NULL, 0, -local_11->field_14, local_11->field_14) + local_11->field_0.y;
			if (local_12)
				local_13.field_1c.z = local_11->field_0.z;
			else
				local_13.field_1c.z = function_259d0(&g_4e7408->unknown0, NULL, 0, -local_11->field_14, local_11->field_14) + local_11->field_0.z;
			real local_18 = local_11->field_c;
			local_13.field_28 = *g_4687a8;
			function_293133(local_18, g_4687b0, &local_13.field_28);
			local_0 = function_b7b40(&local_13);
			if (local_0 != NONE)
			{
				s_293135 *local_15 = (s_293135 *)flock_object_get_member(((s_flock_object_header *)g_4e0300->data)[local_0 & 0xffff].object);
				if (local_15)
				{
					local_15->field_0 = NONE;
					local_15->field_4 = NONE;
					local_15->field_12 = NONE;
					local_15->field_14 = NONE;
					local_15->field_c = false;
					local_15->field_d = false;
					local_15->field_e = true;
					local_15->field_10 = 0;
					local_15->field_18 = NONE;
					local_15->field_1a = 0;
					local_15->field_28 = *g_468788;
					function_2936b0(arg_0, local_0);
					if (local_3->field_14 > 0)
					{
						short count = (short)local_3->field_14;
						local_15->field_4 = random_index(&g_4e7408->unknown0, count);
					}
				}
				else
				{
					function_b8540(local_0);
					local_0 = NONE;
				}
			}
		}
	}
	return local_0;
}
