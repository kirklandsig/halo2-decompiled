// @flags /O2 /Gr /arch:SSE
#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include <math.h>

struct s_2922b0
{
	byte field_0[0x14];
	real field_14;
	byte field_18[0x18];
	real field_30;
	byte field_34[0x14];
	real field_48;
};
struct s_collision_result_1697c0
{
	long field_0;
	real field_4;
	byte field_8[0x24 - 8];
	short field_24;
	byte field_26[0x4c - 0x26];
	long field_4c[3];
	byte field_58[2];
	short field_5a;
};
typedef char assert_collision_result_2922b0_size[(sizeof(s_collision_result_1697c0) == 0x5c) ? 1 : -1];

bool __stdcall function_1697c0(long arg_0, point3f const *arg_1, vector3f const *arg_2,
	long arg_3, long arg_4, s_collision_result_1697c0 *arg_5);
real function_30bf0(vector3f *arg_0);

PRIVATE __forceinline void function_2922b1(vector3f const *arg_0, vector3f const *arg_1, vector3f *arg_2)
{
	real local_0 = arg_0->j * arg_1->k - arg_0->k * arg_1->j;
	real local_1 = arg_0->k * arg_1->i - arg_0->i * arg_1->k;
	real local_2 = arg_0->i * arg_1->j - arg_0->j * arg_1->i;
	arg_2->i = local_0;
	arg_2->j = local_1;
	arg_2->k = local_2;
}
PRIVATE __forceinline void function_2922b2(point3f const *arg_0, vector3f const *arg_1, real arg_2, point3f *arg_3)
{
	arg_3->x = arg_0->x + arg_1->i * arg_2;
	arg_3->y = arg_0->y + arg_1->j * arg_2;
	arg_3->z = arg_0->z + arg_1->k * arg_2;
}
PRIVATE __forceinline real function_2922b3(real arg_0, real arg_1, real arg_2)
{
	return arg_1 > arg_0 ? arg_1 : arg_0 > arg_2 ? arg_2 : arg_0;
}
PRIVATE __forceinline real function_2922b4(real arg_0, real arg_1, real arg_2, real *arg_3)
{
	real local_0 = (real)sqrt(0.0f > 1.0f - arg_0 / arg_1 ? 0.0f : 1.0f - arg_0 / arg_1);
	*arg_3 = local_0;
	return function_2922b3(arg_2 * local_0, 0.0f, arg_2);
}
PRIVATE __forceinline void function_2922b5(vector3f const *arg_0, vector3f const *arg_1, real arg_2, vector3f *arg_3)
{
	real local_0 = (real)sin(arg_2);
	real local_1 = (real)cos(arg_2);
	real local_2 = (arg_0->i * arg_1->i + arg_0->k * arg_1->k + arg_1->j * arg_0->j) * (1.0f - local_1);
	vector3f local_3;
	function_2922b1(arg_0, arg_1, &local_3);
	real local_4 = local_1 * arg_0->i + arg_1->i * local_2 - local_3.i * local_0;
	real local_5 = local_1 * arg_0->j + arg_1->j * local_2 - local_3.j * local_0;
	real local_6 = local_1 * arg_0->k + arg_1->k * local_2 - local_3.k * local_0;
	arg_3->i = local_4;
	arg_3->j = local_5;
	arg_3->k = local_6;
}

// score 629 (was 629): Frame 0x108 agrees; success is retained in BL and saves EBX
// early instead of using retail's stack byte. Hit/basis/ray/root/rotation homes follow.
// Early constant minimum with long success 635, with early point copy 651; final products differ.
// MiniMax H run 14: the cross helper 0x2922b1 with its sign flipped and the operands swapped at
// all four calls (same results) 622; under the keep bar, not kept.
// Frame 0x108 agrees; early success BL/EBX and hit/basis/ray/root/rotation homes remain.
// frame agrees (0x108); early success BL/EBX and basis/ray/root/rotation homes remain;
// retail is 60 bytes longer after the ray loop (12 more loads, 4 more products: (1/range) * minimum
// twice) and re-reads the half-width for each offset point. Round 134: range local read before the
// test (with or without FLT_MAX there) 725/727; ray direction via vector3f_scale, 0 - width,
// minimum * (1/range), sqrt * angle all 641 (same code).
// Frame 0x108 agrees; early EBX/BL success, hit-byte and basis/rotation homes remain.
// Frame 0x108 agrees; early success BL/EBX and hit/basis/ray/root/rotation homes remain.
// Frame 0x108 agrees; early success BL/EBX and hit/basis/ray/root/rotation homes differ.
// Earlier: frame 0xf8 versus 0x108; result/basis/rotation homes differ.
// frame 0x108 agrees; success/basis/rotation homes differ.
// Frame pointer 960; size preference 1163; cosine-difference carrier 652;
// cross before trigonometry 655; inline depth one 652.
// Inverse length 651; separate roots 652; remaining fraction 653; cosine first 652;
// clamp input order 652; direction value with per-ray sample 703 (fast).
// native rotation return 666; native origin return 695; native cross return 864;
// short ray ordinal 712; active minimum lifetime 646 (fast).
// ray length through hit 661; hit clear per ray 719; angle limit observed once 647; ray length array
// initializer 646; joined output zero 648.
// Native object/settings/both record references 646; live length field reference alone
// or with object/settings/both 686; collision inline declaration 646. All eight restored.
// Earlier (652 from 652): Frame 0xf8 versus 0x108 and basis/root/rotation homes remain. Inverse length 651; separate roots 652; remaining fraction 653; cosine first 652; clamp input order 652; direction value with per-ray sample 703 (fast).
// Earlier (646 from 652): Frame 0xf8 versus 0x108 and basis/rotation homes remain.
// Earlier (646 from 646): Frame 0xf8 versus 0x108; result/basis/rotation homes remain.
// Earlier 652 from 652: Frame 0xf8 versus 0x108 and basis/root/rotation homes remain. Inverse length 651; separate roots 652; remaining fraction 653; cosine first 652; clamp input order 652; direction value with per-ray sample 703 (fast).
// Earlier 646 from 652: Frame 0xf8 versus 0x108 and basis/rotation homes remain.
// Earlier 646 from 646: Frame 0xf8 versus 0x108; result/basis/rotation homes remain.
// Existing rotation angle as a double formal or a twice-used double local,
// with original/angle-first/output-first/output-before-angle layouts: all 733.
// Original helpers, input/component snapshots and in-place calls restored.
// Frame 0xf8 versus 0x108; success, basis and rotation homes remain.
// frame 0xf8 versus 0x108; result/basis/rotation homes differ.
// Frame 0xf8 versus 0x108 and basis/root/rotation homes remain. Inverse length 651; separate roots 652; remaining fraction 653; cosine first 652; clamp input order 652; direction value with per-ray sample 703 (fast).
// Frame 0xf8 versus 0x108 and basis/rotation homes remain.
// Frame 0xf8 versus 0x108; result/basis/rotation homes remain.
// Rotation input snapshot 661; axis snapshot 705; both input snapshots 772;
// native point/direction snapshots 776; projection axis fields at use 646.
// Round 171 reduced control 646: rotation role names 646; rotation declaration homes 646; origin pointer
// result 646; rotation pointer result 646; symmetric lateral parts 675; symmetric axial parts 727; both row
// parts 643; row parts/rotation names 643; row parts/declaration homes 643; row parts/origin result 643; row
// parts/rotation result 643.
// bool-logical-clearance 648; bool-compound-clearance 658.
// byte-logical-clearance 648; byte-compound-clearance 660.
// short-logical-clearance 644; short-compound-clearance 655.
// long-logical-clearance 645; long-compound-clearance 702.
// Complete collision output extent/native output type/named material reset 641/641/641;
// with byte/long/word success 714/714/714; native hit bytes 641; active success 646.
// Original result, hit flags, math helpers and in-place copies retained.
// Native coordinate-array output contracts for cross/point/rotation alone, all
// pairs and all three 641 each; all outputs with native point input arrays 641.
// Every input/result copy and original helper contract restored; frame already agrees.
// Round 162: Native hit/length/both/point/hit+point/length+point/all array loops
// 731/730/748/667/721/643/723; shared cross producer on the complete frame 659.
// All regress; original arrays, private producers and in-place snapshots restored.
// Complementary ray-miss bool/byte/word/long arrays 642/642/676/708; active-phase
// arrays 708/708/682/724. Original hit flags, success result and math helpers restored.
// Negative failure result bool/byte/short/long, conditional store: 712/712/713/712;
// negative failure with reused close-hit logical accumulation: 679/679/674/673 (fast).
// Original result, hit arrays and all in-place math snapshots restored.
// Hit aggregate 641; length aggregate 629 kept; later success 661; hit/length
// 642, hit/later success 661, length/later success 650, all 662; success after copy 652.
// Input, hit flags, range, minimum, basis, origins, lengths and distances names each 629;
// original workspace names restored.
// Corrected-frame point-value/direction-value/both 682/694/772; output-reference
// combinations 682/694/772; input references with pointer/reference output 629/629.
// Original helper restored.
// Point-return with pointer/point/direction/both value inputs 680/744/838/899;
// cross first/second/both value inputs 845/858/1045. Original helpers restored.
// Collision-produced bool/byte hit entries with bool/byte/short/long success 700/690/690/690 each;
// memory success recovered but tag/basis homes regress. Original producers and carriers restored.
// Hit clear orders 3210/1032/2301/0213 629/629/629/629; success after
// hit clear 0/1/2/3 686/686/686/686; original entry restored.
// Round 343: Complete tag-range field reads 686; with shared cross products in basis/rotation/both
// 664/686/665; projection/basis+projection/rotation+projection/all 691/668/691/668. All restore; retain original snapshots and helpers.
// Round 146: Real rotation angle-first/output-first, point distance-first/output-first 629/629/629/629;
// cross output-first/all output-first/cross-output with rotation-angle/point-distance with rotation-
// angle 629/629/629/629. Original producer formals and in-place snapshots restored.
// Round 78 (aligned rows in brackets, baseline 482): FLT_MAX minimum declared at the top 635 plain [478];
// length element read at each product 668; early `return true` when the range is not positive 685 [443]
// (retail's prologue: no early EBX push, the flag stored straight to memory), with FLT_MAX at the top
// 680 [439]; nested body ending `return local_0;` then `return true;` 679 [462] (retail's early-exit
// layout and `mov al, 1`). Every stack home is then 4-12 bytes off retail's (flag 0xf, minimum 0x10,
// hit flags 0x14, input copy 0x38 in retail's frame).
// Round 257: shared affine origins 629; early constant minimum 636; collision-produced
// hit entries 700, with early minimum 706; inactive early return on those four
// affine graphs 685/698/681/694. Original helpers, producers and exits restored.
// CRT input copy 629; CRT hit clear 735; both 735; shared zero-output setter 629, with
// input/hit/both transfers 629/735/735; complete entry transfers with early minimum 739. Frame
// agrees; direct memory success and hit/basis/rotation homes remain.
// Round 171: blocked-ray count byte/short/long/unsigned-long at entry 707/707/706/706;
// after input copy and output zero 715/714/713/713. Original success bit, four hit bytes,
// accepted length initializer, every math helper and all in-place input/result copies restored.
// Round 175: ordinary-inline cross/offset/rotation helpers, all seven subsets, 629 each;
// all five existing helpers ordinary-inline 629. Original forced expansions restored;
// frame agrees, early success save and hit/basis/rotation homes remain.
// Round 392: four hit bits as byte mask at entry/active scope 766/766;
// word/long/dword masks 762/765 each. Frame shrinks and code regresses;
// original hit bytes, success, all helpers and in-place copies restored.
// Round 263: normalization/settings inline/noinline declarations singly and all four pairs
// 629/629/629/629/629/629/629/629. Original providers, helpers and in-place snapshots restored.
// Round 250: native reference formals for cross/rotation/root producers,
// subsets 1..7 and all with the point producer: 629 each. All original formals,
// helper calls and in-place snapshots restored; success and workspace homes remain.
// Round 227: Whole rotation sine-cross/base/both shared producers
// 731/648/713; result-first root/with sine/with base/with both
// 726/654/725/633; all with common promoted angle 721.
// Original helpers and all in-place snapshots restored; early success and workspace homes remain.
// Round 177: Success from the completed raw-distance minimum 689; with existing point/rotation/
// both/ray/point+ray/rotation+ray/all setters 707/706/707/711/711/711/711. All restored;
// frame shrinks to 0x104 and native success/hit/basis/rotation homes remain unresolved.
// Round 238: Actual ray origin/length/distance cursors 643/659/659;
// origin+length/origin+distance/length+distance 660/660/645; all three/all plus hit cursor
// 728/728. Original indexed arrays, countdown, success and helper snapshots restored.
// Round 249: Root return / limited-angle output with pointer/reference, output-first/last,
// and limit-first/original input roles: all eight 637. Original outputs, helpers and calls restored.
// Round 424: paired hit-response orders 0213/1023/1032/1302/0132
// 629/629/628/627/628; first three with completed clamp result 726 each.
// Two-point best rejected; original response order and helper restored.
// Existing staged cross in basis/rotation/projection subsets, then all with point setter
// 651/629/651/626/645/626/645/659; three-point best rejected;
// original producers and in-place snapshots restored.
// Round 457: native second-rotation result/two distinct rotation results, original/early
// minimum, original/staged rotation cross: 628/642/634/648/628/642/634/648.
// One-point best rejected; original complete body and all in-place snapshots restored.
// @retail 0x2922b0
bool __stdcall function_2922b0(long arg_0, long arg_1, vector3f *arg_2,
	vector3f *arg_3, real *arg_4, real *arg_5, bool arg_6)
{
	bool local_0 = true;
	s_slot_object_view *local_1 = object_get(arg_1);
	s_2922b0 const *local_2 = (s_2922b0 const *)function_1e5450(arg_0, local_1->tag_index);
	vector3f local_3 = *arg_2;
	*arg_5 = 0.0f;
	bool local_5[4];
	local_5[0] = false;
	local_5[1] = false;
	local_5[2] = false;
	local_5[3] = false;
	if (local_2->field_48 > 0.0f)
	{
		arg_3->i = 0.0f;
		arg_3->j = 0.0f;
		arg_3->k = 0.0f;
		real local_6 = local_2->field_48;
		real local_4 = 3.402823466e38f;
		vector3f local_7;
		vector3f local_8;
		vector3f local_9;
		vector3f const *local_10;
		vector3f const *local_11;
		if (arg_6)
		{
			local_10 = &local_1->forward;
			local_11 = (vector3f const *)((byte *)local_1 + 0x7c);
			function_2922b1(local_11, local_10, &local_9);
		}
		else
		{
			local_7 = *arg_2;
			if (function_30bf0(&local_7) == 0.0f) local_7 = *g_4687a8;
			function_2922b1(g_4687b0, &local_7, &local_9);
			if (function_30bf0(&local_9) == 0.0f) local_9 = *g_4687ac;
			function_2922b1(&local_7, &local_9, &local_8);
			if (function_30bf0(&local_8) == 0.0f) local_8 = *g_4687b0;
			local_10 = &local_7;
			local_11 = &local_8;
		}
		point3f local_12 = local_1->unknown030;
		*arg_4 = 1.0f;
		point3f local_13[4];
		function_2922b2(&local_12, local_11, local_2->field_14, &local_13[0]);
		function_2922b2(&local_12, &local_9, local_2->field_14, &local_13[1]);
		function_2922b2(&local_12, local_11, -local_2->field_14, &local_13[2]);
		function_2922b2(&local_12, &local_9, -local_2->field_14, &local_13[3]);
		real local_14[4] = { local_2->field_14 + local_6, local_2->field_14 + local_6,
			local_2->field_14 + local_6, local_2->field_14 + local_6 };
		real local_15[4];
		s_collision_result_1697c0 local_16;
		local_16.field_24 = NONE;
		long local_17 = 0;
		long remaining = 4;
		do
		{
			vector3f local_18;
			real ray_length = local_14[local_17];
			local_18.i = local_10->i * ray_length;
			local_18.j = local_10->j * ray_length;
			local_18.k = local_10->k * ray_length;
			if (function_1697c0(0x1808c2d, &local_13[local_17], &local_18, arg_1, NONE, &local_16))
			{
				real local_19 = local_14[local_17] * local_16.field_4 - local_2->field_14;
				if (0.1f > local_19 / local_6)
				{
					local_19 = local_6 * 0.1f;
					local_0 = false;
				}
				local_15[local_17] = local_19;
				local_5[local_17] = true;
				if (local_4 > local_19) local_4 = local_19;
			}
			++local_17;
		} while (--remaining);
		if (local_0)
		{
			real local_17 = 0.0f;
			real local_18 = 0.0f;
			real local_19 = local_2->field_30 > 0.0f ? local_2->field_30 * 0.01745329238474369f : 1.5707963705062866f;
			if (3.402823466e38f > local_4)
			{
				*arg_4 = (1.0f / local_6) * local_4 * 0.7f + 0.3f;
				*arg_5 = 1.0f - function_2922b3((1.0f / local_6) * local_4, 0.0f, 1.0f);
			}
			real local_20;
			if (local_5[0])
			{
				local_17 = function_2922b4(local_15[0], local_6, local_19, &local_20);
				arg_3->k -= local_20;
			}
			if (local_5[1])
			{
				local_18 = -function_2922b4(local_15[1], local_6, local_19, &local_20);
				arg_3->j -= local_20;
			}
			if (local_5[2])
			{
				local_17 -= function_2922b4(local_15[2], local_6, local_19, &local_20);
				arg_3->k += local_20;
			}
			if (local_5[3])
			{
				local_18 += function_2922b4(local_15[3], local_6, local_19, &local_20);
				arg_3->j += local_20;
			}
			function_2922b5(&local_3, &local_9, local_17, &local_3);
			function_2922b5(&local_3, local_11, local_18, &local_3);
		}
		if (arg_6)
			*arg_2 = local_3;
		else
		{
			vector3f local_17;
			local_17.i = local_9.i * arg_3->j + local_10->i * arg_3->i + local_11->i * arg_3->k;
			local_17.j = local_9.j * arg_3->j + local_10->j * arg_3->i + local_11->j * arg_3->k;
			local_17.k = local_9.k * arg_3->j + local_10->k * arg_3->i + local_11->k * arg_3->k;
			vector3f local_18;
			vector3f const *local_19 = (vector3f const *)((byte *)local_1 + 0x7c);
			function_2922b1(local_19, &local_1->forward, &local_18);
			arg_3->i = local_1->forward.k * local_17.k + local_1->forward.j * local_17.j + local_1->forward.i * local_17.i;
			arg_3->j = local_18.k * local_17.k + local_18.j * local_17.j + local_18.i * local_17.i;
			arg_3->k = local_19->k * local_17.k + local_19->j * local_17.j + local_19->i * local_17.i;
		}
	}
	return local_0;
}
