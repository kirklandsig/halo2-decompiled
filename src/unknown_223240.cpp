// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "local_players.h"
#include <string.h>
#include <math.h>

struct short_rect
{
	short v0, v1, v2, v3;
};

struct short_rect_pair
{
	short_rect a, b;
};

struct s_223240
{
	long field_0, field_4, field_8;
	byte field_c[0x74];
	byte field_80[0x30];
	short_rect field_b0, field_b8;
	byte field_c0[0x58];
};

struct s_2f800_source;
struct s_2f800_size;
struct s_2f800_view;
struct s_223080;
struct s_223081;
struct s_unknown_13bf00;
struct s_223241
{
	byte field_0[0x358];
};
s_223241 g_4e9c8c[4];
extern s_unknown_13bf00 *g_510c50;
struct s_unknown_485a88
{
    byte flag;
    short_rect_pair rects;
};
extern s_unknown_485a88 g_unknown_485a88;
#define g_485a8a (g_unknown_485a88.rects)
extern long g_4e6470, g_4e64a0;
extern byte g_485ac2;
extern real g_5234dc;
s_223240 g_47f0f8[6];

void function_2f800(s_2f800_view *, s_2f800_source const *, s_2f800_size const *, s_2f800_size const *);
void __stdcall function_2c8a0(byte const *);
void function_2b5d0(long, long, long, long, s_223240 const *);
void function_223080(long, long, long, s_223080 *, long, long, s_223081 const *);
bool function_68290(void);
void main_time_wait_for_vblank(void);
void function_13f10(long, long);

PRIVATE __forceinline s_223240 *function_2232a1(s_223240 *arg_0)
{
	arg_0->field_4 = NONE;
	arg_0->field_8 = NONE;
	arg_0->field_b0 = g_485a8a.a;
	arg_0->field_b8 = g_485a8a.b;
	arg_0->field_0 = 1;
	function_2f800((s_2f800_view *)arg_0->field_80, 0, 0, 0);
	memcpy(arg_0->field_c, arg_0->field_80, 0x74);
	return arg_0;
}

// @retail 0x2232a0
void function_2232a0(void)
{
	function_2c8a0((byte const *)function_2232a1(&g_47f0f8[0]));
}

// score 109 (was 109): this round: the tail call still needs 0x2b5d0's own body (it takes the mode
// in EAX and the count on the stack), which is disabled here and stubbed in src/stubs/lane_k.cpp,
// a file this lane may not edit: BLOCKED-BY 0x2b5d0.
// Earlier (109): one-dword frame agrees; the count block, the count/layout join and the layout
// arms now have retail's code: the clamp is the ternary the other player-count clamps use, and the
// loop counter's zero assigned before the layout test keeps the join from being jump-threaded (with
// it anywhere later every count-one path threads straight to the layout stores); the slot scan uses
// the shared local-player predicate (retail's setne/test). Still differs: retail's outer loop has a
// pretest (`cmp ebp, edi; jle`) and keeps count + 1 in EDX for the tail call, but a while loop
// (160/123 with a named view count) moves our EBX save after the pretest, since our tail call
// (BLOCKED-BY 0x2b5d0, below) needs no extra register. Round 83: layout default before the test 167,
// with a for loop 173, with the ternary clamp 162; ternary clamp + early loop-counter zero 133 kept;
// with a while loop 152, named view count 139, both 184; shared slot predicate 124 kept; then while
// loop 160, named view count 137, both 123, slot data as a ternary 122, slot data declared after the
// scan 133. Compile-only screens: count helpers taking the table pointer, the short, both pointers or
// a flag helper, a count defaulted to one at entry, MIN/MAX macros: all thread the count-one edges.
// The tail call is BLOCKED-BY 0x2b5d0: its body is disabled and a /Gr stub takes the count in ECX and the
// mode in EDX, where retail passes the count in EAX and the mode in ECX. Round 73: sibling clamp ternary
// 182; pretested for loop 161; both 183; callee formals mode-first 151 (mode reaches ECX, count EDX).
// Round 97: the existing 0x16a90 predicate for the layout test 151 (identical code); inverted count
// condition 151, with the for loop 161; layout as one ternary 151; count defaulted to one before the
// test 187 (543 bytes); count and layout declared before the scan index 151.
// One-dword frame agrees; count/layout join, slot predicate and terminal/tail allocation still differ.
// Earlier: One-dword frame agrees; common count/layout join, terminal registers and tail convention remain.
// Earlier: count-one paths still bypass the common count/layout join; tail call differs.
// Signed-byte count alone/with player value 169; signed-word layout alone/with normalized selection 174;
// native selected-player value, normalized selection and their combination 151; redundant scan test removed 154.
// count-one join, slot predicate and terminal/callee registers differ;
// byte count 169; layout enum 151; completed ordinal at terminal/dispatch/both 151/158/158;
// direct terminal initializer 151; layout refinement 151; world-layout result 151.
// count-one paths skip the common count/layout join;
// slot-predicate materialization and final initializer residences differ.
// Mode/options receivers, mode result, external count/slot references, typed selected
// record, terminal record reference and both record receivers each 151 (575 bytes).
// Earlier: common count/layout join, slot predicate and terminal registers remain.
// common count/layout join, slot predicate and terminal registers remain.
// Total before layout/scan 172/152; total scan bound 162; two-use mode 151;
// conditional slot result 157; separate scan/result 158; mode/total 162; scan-result/total 156.
// the common count/layout join, slot predicate and terminal reset still differ.
// First/second/both conditional countdown expressions 169/161/167;
// signed-byte/unsigned-word layout 189/174; unsigned-word output ordinal 201;
// signed-byte selection 164; dispatch noinline declaration 151. Original body retained.
// One-dword frame agrees; count-one paths bypass the shared count/layout join, with slot and reset
// allocation gaps. unsigned selected slot 151; word selected slot 157; word view ordinal 201; byte
// view ordinal 167; exact scan bound 175; existing initializer inline 151; completed search result
// 180; word scan and ordinal 157; byte selected slot 164.
// Earlier: count/layout join still threads. First/second/both native
// countdown scalars 151/151/151; prefix countdown publication 151/151/151;
// paired countdowns with int count or int ordinals 151/151. Original body retained.
// chained sentinels 151; native rectangle pair 150; rectangle library copies 150; direct source argument 152; live count field 151; search in for test 151; sentinel chain with rectangle value 150; live count with direct source 152 (fast). Gains of one are rejected.
// count/layout join still threads. First/second/both native
// Nested slot admission/sentinel continue/selected-source value 151/151/151; source admission before normalization 158; nested source 151; continued source 158; sole output cursor 232; native entry address 151 (restored).
// sentinels/mode/rectangles 151; rectangles/sentinels/mode 151.
// rectangles/mode/sentinels 151; mode/sentinels/rectangles 151.
// mode/rectangles/sentinels 151; reversed rectangle pair 151.
// sentinels/mode/reversed pair 151; reversed pair/sentinels/mode 151.
// wide source count 138; wide count result 215; wide layout 174;
// wide selected slot 227; wide output ordinal 195; unsigned byte layout 189;
// unsigned native layout 151; immutable native source count 151. Wide source count worsens aligned rows.
// Round 155: post-traversal terminal index from count as long/dword/short/word/byte
// 159/159/156/156/157; from completed ordinal as long/word/short 153/150/150. One-point gains rejected.
// Whole countdown placements before count/layout/scan, first/second phases
// 01/02/10/11/12/20/21/22: 185/159/185/190/190/159/190/164 (fast).
// Original timer order, count/layout producers, shared initializer and body restored.
// Negative world-rejection bool/byte/short/long in multiple-first layout 151 each;
// single-first layout on those negative-result graphs 168 each. All restored.
// Contiguous eight-byte sentinel fill 152; fill with native rectangle value/library rectangle copies 152/152;
// fill with existing slot predicate alone/rectangle value/copies 151/151/151; fill with plain pretest 170; all 178.
// Every sentinel-fill form also loses the matched initializer caller in the reduced image; all restored.
// Complete occupied byte/short/long/signed-byte results 151 each; complete vacant byte/short/long/signed-byte
// results 151 each. Original slot test and body restored.
// Layout field-test orders flag/state/index, index/flag/state, index/state/flag,
// state/flag/index, state/index/flag and three global-first orders 151 each.
// Existing complete layout query 151; query with ordinary pretest/single-first layout 161/168;
// query with exclusive/conditional clamp 182/182; exclusive clamp with pretest 183;
// native byte layout with query/do/pretest 189/177. Original expanded gate and loop retained.
// Round 221: outer completed-count do/for/while 151/161/161; last-ordinal
// do/for/while 152/169/169; actual output-table boundary do/for 179/195.
// Original count producer and traversal restored.
// Round 248: Split lower-bound phase across mode selection, snapshot/live field
// with original/pretested traversal 152/163/152/163; split upper-bound phase
// counterparts 185/195/185/195. Original count producer and loop restored.
// Coherent ordinary inline dispatch/reset/both 151/151/151; each with existing slot predicate 150;
// force-inline reset alone/with slot predicate 151/150 and loses the matched initializer caller. All restored.
// Round 212: shared raw count drives clamp and layout, short/long with nested clamp
// 167/177; exclusive clamp 146/149 (five/two-point gains rejected); the four with plain for
// traversal 177/178/165/168. Original count producer, traversal and tail contract restored.
// DH round 189: Native equality-bound ordinals: short/word/byte do 201/201/167; short/word/byte
// for 180/180/166; short/word while 180/180. Original signed clamp, ordinal, scan, initializer and
// tail restored.
// Round 81: count clamp as one ternary/with the mode test in the same ternary/if-else chain
// 182/185/182; short count 153; long raw count 153 (fast). Compile-only screens of an inline count
// helper (single or multiple returns), a pin helper and a layout helper taking the count: each still
// threads the count-one edges past the layout test. All restored.
// Round 7 (lane 10): count from a single-exit inline helper 182 (count 4 no longer threads, count 1
// still does); helper with pretested for loop 183; if/else-if clamp in retail's branch order 182;
// named view count (count + 1) after the loop 159; count arms swapped (clamp first) 151. All restored.
// Standard clamp producers: short min/max clamp 181; long min/max clamp 176.
// short max/min clamp 181; long max/min clamp 176.
// short min/max with equality join 181; long min/max with equality join 176.
// short max/min with equality join 181; long max/min with equality join 176.
// All executable forms restored.
// Round 67: the slot data assigned in every arm (no initializer; NULL before the NONE store on the
// empty scan) 109 kept: the found arm is now inline as in retail; NULL after the NONE store 117;
// arms inverted 118; NULL arm + ternary found arm 117/109; initializer + ternary 122; found arm as
// NULL then test 124; a default-NULL inline getter 124; with a pretested while loop 138; the
// pointer declared before the outer loop 109. Activating
// 0x2b5d0's disabled body 127 (it takes the mode in EAX and the count on the stack; retail count
// EAX, mode ECX; its frame does not self-align as retail's `and esp, -8`), with formals
// players/layout/source/count/mode 123; reverted.
// Round 213: byte-source exact do/for/while and last-ordinal do [109, 138, 138, 91];
// native-record-source counterparts [109, 138, 138, 91]. Last-ordinal 91 adds an unmatched
// bound calculation and worsens aligned instructions; restored. Pretest, optional-source and
// tail transport remain.
// Round 87 (fast): slot data NULL at the loop top with no else arms 124 (gives retail's loop-top zero,
// in EDX, but not the found arm's second zero); the NONE store then one ternary 117, one if/else 117;
// the first with a pretested for loop 179, the second 166. 0x2b5d0's file is held by another lane.
// Round 152: slot data NULL at the loop top + default-last ternary in the found arm 122 (NULL lands in
// ECX, retail EAX); NULL then an `index != NONE && g_4686c4 != NONE` guard after the scan 121 (the
// NONE test is not threaded); that ternary form with the pretested for loop 158 (EBX save sinks past
// the pretest: our tail needs no EBX because the stub takes count + 1 in ECX); NULL first in both arms
// 124. The one-use record pointer for the initializer dropped (identical code, kept).
// Round 223: Complete shared successor with staged/direct optional source in do/for traversal 319/168/319/168; with twice-used terminal count 268/113/268/113. Original scan, source, traversal and terminal restored.
// Separate candidate/result on the recovered prefix: long/int with original/pretested
// traversal 106/172 each; short/word counterparts 160/192 each. Three-count gains rejected;
// original scan restored. Outer pretest, optional-source homes and tail transport remain.
// Round 208: advancing native output receiver on the recovered source arms:
// scan-boundary acquisition, numeric/pointer terminal, do/for 111/170/141/192;
// layout-boundary counterparts 130/171/158/192. All restored; outer pretest,
// source homes and outside tail transport remain.
// Round 81: combined entry/found source clears in do/for traversal 124/120; entry clear with
// original arms 109/138; entry clear without the empty-arm store 122/158; entry clear without
// the final null-arm store 123/160. Original source assignments and traversal restored.
// Round 68 (m3 lane 16, fast; 0x223240 and 0x2232a0 MATCH each time; the two stacked score lines merged):
// the slot data declared outside the outer loop 109; the counter's increment in the loop test 109; the NONE
// store before the empty-scan NULL 117; the scan index declared after count and layout 109; the per-view
// record address as `g_47f0f8 + 1 + index` 109 (identical code where 109). 75 of the differences still sit
// in the tail after the loop (0x2b5d0's convention: count + 1 in EAX, kept in EDX across the record init).
// @retail 0x2234e0
void function_2234e0(void)
{
	long local_1 = NONE;
	long local_2;
	long local_3;
	if (g_4e6470 > 0)
		--g_4e6470;
	if (g_4e64a0 > 0)
		--g_4e64a0;
	if (g_510c50 && ((byte const *)g_510c50)[5])
		local_2 = 1;
	else
	{
		short local_4 = *(short const *)((byte const *)g_4e8c20 + 8);
		local_2 = local_4 < 1 ? 1 : local_4 > 4 ? 4 : local_4;
	}
	long local_0 = 0;
	if (local_2 > 1)
	{
		if (!(g_4e6948 && g_4e6948->flag && g_4e6948->index != NONE &&
			g_4e6948->state == 3) && g_485ac2)
			local_3 = 2;
		else
			local_3 = 1;
	}
	else
		local_3 = 0;
	do
	{
		byte const *local_5;
		for (++local_1; local_1 < 4; ++local_1)
		{
			if (local_player_exists(local_1))
				break;
		}
		if (local_1 >= 4)
		{
			local_5 = 0;
			local_1 = NONE;
		}
		else if (g_4686c4 != NONE)
			local_5 = g_4e9c8c[local_1].field_0;
		else
			local_5 = 0;
		function_223080(0, local_0, local_1, (s_223080 *)&g_47f0f8[local_0 + 1],
			local_2, local_3, (s_223081 const *)local_5);
		++local_0;
	} while (local_0 < local_2);
	function_2232a1(&g_47f0f8[local_2 + 1]);
	function_2b5d0(local_2 + 1, 3, local_2, local_3, &g_47f0f8[1]);
}

// @retail 0x223240
void function_223240(void)
{
	if (g_4e6948 && g_4e6948->flag1120 && g_4e6948->index != NONE && !function_68290())
		function_2234e0();
	else
		function_2232a0();
	bool local_0 = g_4e6470 <= 0 && g_4e64a0 <= 0;
	main_time_wait_for_vblank();
	if (local_0)
		function_13f10(0, 0);
}
