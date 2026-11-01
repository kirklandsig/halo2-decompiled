// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include <string.h>

struct s_fog_cluster_distance
{
	long cluster_index;
	real distance;
	long source_kind;
	long source_index;
	real source_weight;
	long previous_index;
};

struct s_12ec00;
struct s_12eb00
{
	byte field_0[0xd8];
	long field_d8;
	s_12ec00 *field_dc;
	byte field_e0[0x14];
};
struct s_palette_owner;
struct s_fog_accumulator
{
	color3f color;
	real intensity;
	real distance;
	real height;
	real weight;
	real remaining;
};
struct s_12efb6
{
	color3f field_0;
	real field_c;
	real field_10;
	real field_14;
};
struct s_weighted_accumulator
{
	real a[3];
	real b[3];
	real c[4];
	real weight;
	real remainder;
};
struct s_12efb0
{
	byte field_0[0x1c];
	real field_1c[6];
	real field_34[6];
	real field_4c[4];
	byte field_5c[0x10];
	long field_6c;
	real field_70[10];
	byte field_98[0x30];
	real field_c8[6];
	real field_e0[4];
	long field_f0;
};

long scenario_fog_clusters_find(long cluster_index, point3f const *point, s_fog_cluster_distance *clusters);
long function_12eb00(long arg_0, s_fog_cluster_distance *arg_1, s_12eb00 *arg_2);
long function_130f60(s_palette_owner const *owner, long *palette_index);
void function_131030(long tag_index, vector3f const *view_direction, s_fog_accumulator *first, s_fog_accumulator *second, s_fog_accumulator *third);
void function_1317a0(real const *a, real const *b, real c0, real c1, real c2, real c3, real t, s_weighted_accumulator *acc);

real g_547f80 = 1.0f;

static __forceinline real function_12efb1(real arg_0)
{
	if (arg_0 < 0.0f) arg_0 = 0.0f;
	else if (arg_0 > 1.0f) arg_0 = 1.0f;
	return arg_0;
}

static __forceinline void function_12efb2(s_fog_accumulator *arg_0, real const *arg_1, real arg_2, real arg_3, real arg_4, real arg_5)
{
	arg_5 = function_12efb1(arg_5);
	arg_0->weight += arg_5;
	arg_0->remaining *= 1.0f - arg_5;
	arg_0->color.red += arg_1[0] * arg_5;
	arg_0->color.green += arg_1[1] * arg_5;
	arg_0->color.blue += arg_1[2] * arg_5;
	arg_0->intensity += arg_2 * arg_5;
	arg_0->distance += arg_3 * arg_5;
	arg_0->height += arg_4 * arg_5;
}

static __forceinline void function_12efb3(s_fog_accumulator *arg_0, s_fog_accumulator const *arg_1, real arg_2)
{
	arg_2 = function_12efb1(arg_2);
	real weight = arg_1->weight;
	arg_0->weight += arg_2;
	arg_0->remaining *= 1.0f - arg_2;
	if (weight > 0.0f)
	{
		real local_0 = arg_2 / weight;
		arg_0->color.red += arg_1->color.red * local_0;
		arg_0->color.green += arg_1->color.green * local_0;
		arg_0->color.blue += arg_1->color.blue * local_0;
		arg_0->intensity += arg_1->intensity * local_0;
		arg_0->distance += arg_1->distance * local_0;
		arg_0->height += arg_1->height * local_0;
	}
}

static __forceinline void function_12efb4(s_fog_accumulator const *arg_0, real *arg_1)
{
	if (arg_0->weight > 0.0f && arg_0->intensity > 0.0f)
	{
		real local_0 = 1.0f / arg_0->weight;
		arg_1[0] = arg_0->color.red * local_0;
		arg_1[1] = arg_0->color.green * local_0;
		arg_1[2] = arg_0->color.blue * local_0;
		arg_1[3] = arg_0->intensity * local_0;
		arg_1[4] = arg_0->distance * local_0;
		arg_1[5] = arg_0->height * local_0;
	}
}

static __forceinline void function_12efb5(s_12efb6 *arg_0, real const *arg_1, real arg_2)
{
	arg_2 = function_12efb1(arg_2);
	arg_0->field_0.red += arg_1[0] * arg_2;
	arg_0->field_0.green += arg_1[1] * arg_2;
	arg_0->field_0.blue += arg_1[2] * arg_2;
	arg_0->field_c += (arg_1[3] == 0.0f ? 1.0f : arg_1[3]) * arg_2;
	arg_0->field_10 += arg_2;
	arg_0->field_14 *= 1.0f - arg_2;
}

// score 616 (was 616): Frame 0x4820, clears and metadata cursor agree; palette and accumulator carriers remain.
// Earlier: Frame 0x4820 and metadata cursor agree; native palette width and accumulator carriers differ.
// Frame, clear prefix and metadata cursor agree; palette/cluster carriers and accumulator scheduling remain.
// Frame 0x4820, clears and metadata cursor agree; cluster-index EDI, palette carriers and helper update scheduling differ.
// Earlier: Frame 0x4820 and clear scheduling agree; output cursor +0x10 versus +8 and palette/cluster homes remain.
// Earlier: Frame 0x4820 agrees; output cursor +0x10 versus +8 and palette/cluster homes remain.
// Earlier at 692: Frame 0x4820 agrees; clear scheduling and output cursor +0x10 versus +8
// Frame 0x4820 agrees; clear setup and +0x10/+8 output induction remain.
// Earlier 692 (was 725): frame 0x4820 agrees; clear scheduling, output cursor
// Frame 0x4820 agrees; clear scheduling and output cursor +0x10 versus +8
// remain. Redundant weight guard removed 1080; chained remainder defaults 698; merge failed-weight
// exit 692; const selected record 692; named uncovered fractions 692.
// Earlier: frame 0x4820 agrees; clear scheduling, output cursor
// +0x10 versus +8 and palette/cluster registers still differ. Interleaved
// scalar helper 727; the alpha term first 707 alone but 713 together with
// the weight local (not kept); ordinary output inline 725; ordinary color
// inline 725; output-helper output-first parameters 725.
// Earlier 692 (was 692): clear schedule and output cursor +0x10 versus +8 remain;
// Earlier: clear schedule and output cursor +0x10 versus +8 remain;
// native output clear 742; output clear before palette lookup 790; previous index 692;
// cluster-distance snapshot 762; zero accumulator copies 835.
// Library clear orders 5-6-7-8-10-9 / 5-6-7-9-8-10 / 5-6-7-9-10-8 /
// 5-6-7-10-8-9 / 5-6-7-10-9-8 / 10-5-6-7-8-9 / 5-6-10-7-8-9 /
// 6-5-7-8-9-10: 687/723/720/771/774/729/770/708 (fast).
// Final two clears swapped: 687 plain, same length, first +0x15f; retained.
// Round 150: native color input 692; native six-field output 692; both 692; wide best weight 827; word cluster ordinal 768; color with wide weight 827; native output with word ordinal 768; wide weight with word ordinal 868 (fast).
// native-source-012 683; native-source-021 682.
// native-source-102 694; native-source-120 694.
// native-source-201 685; native-source-210 685.
// source-record-012 683; source-record-021 682.
// Single-caller source writer 682/681/693/693/684/684; ordinary writer 682/684, all reverted.
// Round 155: cluster-index snapshot/source-record/byte-state and all four combinations 683 each;
// all three with readonly source record 683. Native metadata cursor agrees at +0x1838.
// Consistent palette slot char/short/long, mutable or readonly record: 705/616/713;
// inverse weighted-state with char/long palette slot: 705/713 (fast).
// One signed-short palette observation retained at all five uses: 616 plain, 3839/3817 bytes.
// Native member scalar/merge/publication operations singly 615/616/615;
// scalar+merge/scalar+publication/merge+publication 617/615/616; all three/all four 617/616.
// One-point leads dropped; original complete free-function helpers restored.
// Separate clamped weight in scalar/merge/color helpers 684/683/683; scalar-merge/scalar-color/merge-color/all 684/684/683/684;
// normalized contribution before destination stores 742. Original input captures and helper bodies restored.
// Uniform scalar/merge/scalar-merge/color/scalar-color/merge-color/all contribution product operands; all
// normalized publication products; then source-first scalar/merge/scalar-merge/color/scalar-color/merge-
// color/all helper orders and all with output-first normalization:
// 616/616/616/616/616/616/616/616/615/616/615/617/616/617/616/616 (fast); all restored.
// Complete source-value copies: scalar RGB/merge accumulator/both/color RGB/scalar-color/merge-color/all
// 887/818/984/855/989/953/1101; complete publication source 894. Copies grow code/frame; originals restored.
// No-palette dispatch first, tag/fallback admission with word/byte palette 620/620/712/713;
// conditional selected-record result on each 620/620/712/713. Original dispatch and word restored.
// Round 343: Shared caller falloff/contribution/both lifetimes 616/616/617; uncovered contribution 715;
// radius+uncovered/weight+uncovered/all 716/716/714; all with native byte palette 611 (five-point lead rejected). Original body restored.
// Round 146: Native reference destinations scalar/merge/both/color/scalar-color/merge-color/all
// 616/616/616/616/616/616/616; all native accumulator references 616; scalar-
// field/contribution/both/both-with-destination references 699/617/698/698. Original helper contracts
// and alias-safe weight capture restored.
// frame 0x4820, clears and metadata cursor agree; the palette byte, the palette
// record (EBX in retail), the float constants (zero in XMM7) and the accumulator scheduling differ.
// Retail reads the palette byte at each use (`mov cl, [esi+0x6f]`, a byte compare with -1, one
// `movsx ecx, cl`, and a fresh `movsx edx, byte [esi+0x6f]` for the source index), and loads the
// four colour values of the fog-colour accumulator up front in reverse order (scalar arguments).
// Tried: redundant weight guard removed 1080; chained remainder defaults 698; merged failed-weight exit
// 692; const selected record 692; named uncovered fractions 692; interleaved scalar helper 727; the alpha
// term first 707, with the weight local 713; ordinary output inline 725; ordinary colour inline 725;
// output-helper output-first parameters 725; native output clear 742; output clear before palette lookup
// 790; previous index 692; cluster-distance snapshot 762; zero accumulator copies 835; library clear
// orders 5-6-7-8-10-9/5-6-7-9-8-10/5-6-7-9-10-8/5-6-7-10-8-9/5-6-7-10-9-8/10-5-6-7-8-9/5-6-10-7-8-9/
// 6-5-7-8-9-10 687/723/720/771/774/729/770/708, final two clears swapped 687; native colour input 692;
// native six-field output 692, both 692; wide best weight 827; word cluster ordinal 768; colour with wide
// weight 827; native output with word ordinal 768; wide weight with word ordinal 868; native-source
// 012/021/102/120/201/210 683/682/694/694/685/685; source-record 012/021 683/682; single-caller source
// writer 682/681/693/693/684/684, ordinary writer 682/684; cluster-index snapshot/source-record/byte-state
// and all four combinations 683 each, all three with readonly source record 683; consistent palette slot
// char/short/long, mutable or readonly record 705/616/713; inverse weighted-state with char/long palette
// slot 705/713; one signed-short palette observation at all five uses 616; native member scalar/merge/
// publication operations singly 615/616/615, pairs 617/615/616, all three/four 617/616; separate clamped
// weight in scalar/merge/colour helpers 684/683/683, pairs 684/684/683, all 684; normalized contribution
// before destination stores 742; uniform contribution product operands and helper orders (sixteen
// forms) 615-617; complete source-value copies 887/818/984/855/989/953/1101, publication source 894;
// no-palette dispatch first, tag/fallback admission with word/byte palette 620/620/712/713, conditional
// selected-record result on each 620/620/712/713; shared caller falloff/contribution/both lifetimes
// 616/616/617; uncovered contribution 715; radius+uncovered/weight+uncovered/all 716/716/714, all with
// native byte palette 611. Round 75: palette byte read at each use 683; fog-colour accumulator with
// four scalar colour arguments 770 (frame 0x4824: one colour value spills), with its alpha term first
// 761, with the per-use palette byte 746; named alpha in the pointer helper 698 (3826 bytes, aligned
// 497 -> 489). Round 78: byte test with a long index at the top of the branch and the field re-read at
// the source-index store 683 (fast; retail's movsx position); with a named cluster index 683; lookup through
// the existing entry_at_index getter 612 plain (aligned rows 497 -> 803, ebx/edi/esi roles move), with the
// long index 612. Retail assigns the NULL record before the table's own NULL test.
// Round 107: NULL record assigned in the else arm 616 (identical); with char/long palette 705/713;
// a copy of 0x12eb00's entry_at_index after a g_4e0350 test 677, with char/long palette 707/698;
// a lookup helper that also tests g_4e0350 (NULL result, then the global, bounds, entry) 614, 3830
// bytes: it gives retail's lookup control flow (every failure through the `test record` join),
// leaving register choice; the same with a char test and a long index at the top 753.
// Round 235: query-table/tag-input/falloff-radius consumption scopes, all seven combinations 616 each; source-selection consumption scope 616.
// Round 172: complete native accumulated results in scalar/merge/both/color/scalar-color/merge-color/all
// 1015/1012/1288/1144/1416/1459/1671; all with signed-byte palette 1671.
// Original field publication, input captures and helpers restored; palette/record and SSE carriers remain.
// Round 166: Existing shared scaled-add for scalar/merge/color RGB updates, all seven subsets 616;
// all three plus shared native color scaling at normalization 616. Original free helpers, source-
// weight capture, body and includes restored. Palette/selected-record and zero/accumulator
// carriers remain.
// Round 253: multi-use scenario receiver with short/char/long palette 616/705/713;
// each with plain cluster cursor 616/705/713; readonly selected record alone/with cursor
// 616/616. Original source acquisition, loop and executable body restored.
// Round 387: Existing scalar accumulator takes all three RGB channels as values; destination order, four-channel color inputs and uniform palette-byte combinations 751/751/809/809/753/753/797/797. All original helpers restored.
// Round 221: native destination-return contracts in scalar/merge/both/color/scalar-color/
// merge-color/all helpers, then all with normalized publication, 616 each. All eight restored;
// original free-helper contracts and source-weight capture retained.
// Round 174: bounded contribution contracts without repeated scalar/merge/color
// clamps in seven subsets 733/692/768/695/775/735/812; all with upper-first common
// clamp 812. Color-only reaches retail length but regresses; original helpers restored.
// Round 390: Source-radius input snapshot with per-cluster default-range, color-alpha and
// signed-byte palette combinations 694/770/710/670/702/728/629/728. Original helpers and body restored.
// Round 407: native palette scalar/RGB/weighted/footer fields with radius snapshot, byte
// palette and shared scenario receiver combinations 616/616/705/705/694/694/702/702. Original record and body restored.
// Round 417: contribution-first destination sums in scalar/merge/color, all seven subsets,
// then all with factor-first normalized publication: 616 each. Original helpers and body restored.
// Round 425: Bounded cluster count short/word/unsigned-long, each with original/inverse
// weighted-state, 618/700/618/700/616/698; short/word ordinal paired with its count
// 755/739. Original full-width count, Boolean state, loop and complete body restored.
// Round 228: native aggregate value inputs scalar/merge/both/color/scalar-color/merge-color/all
// 886/813/977/826/976/934/1081; all with by-value publication 1240. Original helper contracts and body restored.
// Round 237: Plain scalar/merge/color native component loops 1/2/3/4/5/6/7, then all with normalized publication loop: 618/616/619/616/619/616/620/620.
// Original executable body restored.
// Round 242: shared two-source range conditional/nested/result/guard forms with word palette
// 619/619/675/618, with byte palette 708/708/670/708. Original helpers and body restored;
// palette/record and zero/SSE accumulator carriers remain.
// Round 251: Native accumulated-value return in scalar/merge/both/color/scalar-color/merge-color/all helpers
// 1013/1013/1287/1145/1419/1426/1647; all with byte palette 1625. Original helper contracts, input-weight
// capture and complete body restored; palette/record and SSE carriers remain.
// Round 264: Library contribution clamp, paired radius caps and paired fallback bounds,
// subsets 1/2/4/3/5/6/7 and full upper-first clamp 2071/836/848/2166/2161/942/2251/2228.
// All enlarge code; original selections, input captures and complete body restored.
// Round 440: complete immediate contribution products in scalar/merge/color, subsets 1/2/4/3/5/6/7, then
// all with six-field publication 616/616/615/616/615/615/616/616. The 615 forms gain only one and are
// rejected. Original product trees, all reads/stores, alias-safe source-weight copy and complete
// helpers/body restored.
// Round 270: Existing weighted-result default owned by each source arm, bool/byte/short/long
// at arm entry 631/631/779/701; after falloff 824/824/828/829.
// Original default, helpers and complete body restored; palette/record and SSE carriers remain.
// Round 275: Byte admission with reused long/short promoted index 756/757; each with immutable provider result, earlier record default, and both 756/757 each.
// Original executable source restored; palette/selected-record and zero/SIMD accumulator carriers remain.
// Round 277: Object-owned library clear/unit-default pairs,
// fog/weighted/color/fog-weighted/fog-color/weighted-color 616/621/622/616/618/622; all is the same
// fog-weighted form. All with byte palette 705; all zero-prefix clears 724. Original initialization
// and body restored; palette/record and SIMD accumulator carriers remain.
// Round 444: Double scalar-contribution/merge-fraction/color-contribution result domains,
// subsets 1/2/4/3/5/6/7 then all with complete publication reciprocals: 838/839/772/823/867/828/833/927. Original helper
// precision, alias-safe source-weight capture and full body restored; palette/record and SIMD carriers remain.
// @retail 0x12efb0
void function_12efb0(long arg_0, point3f const *arg_1, vector3f const *arg_2, s_12efb0 *arg_3)
{
	s_fog_cluster_distance local_0[512];
	s_12eb00 local_1[24];
	long local_2 = NONE;
	real local_3 = 0.0f;
	long local_4 = scenario_fog_clusters_find(arg_0, arg_1, local_0);
	function_12eb00(local_4, local_0, local_1);
	s_fog_accumulator local_5;
	s_fog_accumulator local_6;
	s_fog_accumulator local_7;
	s_fog_accumulator local_8;
	s_12efb6 local_9;
	s_weighted_accumulator local_10;
	memset(&local_5, 0, sizeof(local_5));
	memset(&local_6, 0, sizeof(local_6));
	memset(&local_7, 0, sizeof(local_7));
	memset(&local_8, 0, sizeof(local_8));
	memset(&local_10, 0, sizeof(local_10));
	memset(&local_9, 0, sizeof(local_9));
	local_5.remaining = 1.0f;
	local_6.remaining = 1.0f;
	local_7.remaining = 1.0f;
	local_8.remaining = 1.0f;
	local_10.remainder = 1.0f;
	local_9.field_14 = 1.0f;
	for (long local_11 = 0; local_11 < local_4; local_11++)
	{
		s_fog_cluster_distance *local_12 = &local_0[local_11];
		byte *local_13 = *(byte **)((byte *)g_4e0348 + 0xa0) + local_12->cluster_index * 0xb0;
		long local_14 = NONE;
		long local_15 = function_130f60((s_palette_owner const *)local_13, &local_14);
		bool local_16 = false;
		local_12->source_kind = 0;
		local_12->source_index = NONE;
		local_12->source_weight = 0.0f;
		short palette_slot = *(char *)(local_13 + 0x6f);
		if (palette_slot != NONE)
		{
			s_12eb00 *local_18 = NULL;
			if (local_12->previous_index != NONE)
				local_18 = &local_1[local_12->previous_index];
			else if (g_4e0350 && palette_slot >= 0 && palette_slot < *(long *)((byte *)g_4e0350 + 0x340))
				local_18 = (s_12eb00 *)(*(byte **)((byte *)g_4e0350 + 0x344) + palette_slot * 0xf4);
			if (local_18)
			{
				real const *local_19 = (real const *)local_18;
				real local_20 = local_19[4] > 0.0f ? (local_19[4] < 10.0f ? local_19[4] : 10.0f) : g_547f80;
				real local_21 = function_12efb1(1.0f - local_12->distance / local_20);
				if (local_12->cluster_index == arg_0)
					arg_3->field_f0 = *(word *)((byte *)local_18 + 0xf0);
				if (local_21 > 0.0f)
				{
					function_12efb2(&local_5, local_19 + 1, local_19[6], local_19[7], local_19[8], local_21);
					function_12efb2(&local_6, local_19 + 9, local_19[13], local_19[14], local_19[15], local_21);
					function_12efb2(&local_7, local_19 + 17, local_19[20], local_19[22], local_19[21], local_21);
					if (local_19[0x29] > 0.0f && *(long *)((byte *)local_18 + 0xd4) != NONE)
					{
						real const *local_22 = local_19 + 0x22;
						if (local_22[7] > 0.0f)
						{
							function_1317a0(local_22, local_22 + 3, local_22[6], local_22[7], local_22[8], local_22[9], local_21, &local_10);
							if (local_12->cluster_index == arg_0 || local_21 > local_3)
							{
								local_2 = *(long *)((byte *)local_18 + 0xd4);
								local_3 = local_21;
							}
							local_16 = true;
						}
					}
					if (local_19[0x38] > 0.0f)
						function_12efb5(&local_9, local_19 + 0x38, local_21);
					local_12->source_kind = 2;
					local_12->source_index = palette_slot;
					local_12->source_weight = local_21;
				}
			}
			else
				goto local_30;
		}
		else if (local_15 != NONE)
		{
			byte *local_23 = g_4e3b44[local_15 & 0xffff].bytes;
			real local_24 = *(real *)(local_23 + 0x44);
			local_24 = local_24 > 0.0f ? (local_24 < 10.0f ? local_24 : 10.0f) : g_547f80;
			real local_25 = function_12efb1(1.0f - local_12->distance / local_24);
			if (local_25 > 0.0f)
			{
				s_fog_accumulator local_26, local_27, local_28;
				function_131030(local_15, arg_2, &local_26, &local_27, &local_28);
				function_12efb3(&local_5, &local_26, local_25);
				function_12efb3(&local_6, &local_27, local_25);
				function_12efb3(&local_8, &local_28, local_25);
				local_7.weight += local_25;
				if (*(long *)(local_23 + 0x60) > 0)
				{
					real const *local_29 = *(real **)(local_23 + 0x64);
					if (*(long *)((byte *)local_29 + 0x4c) != NONE && local_29[7] > 0.0f)
					{
						function_1317a0(local_29, local_29 + 3, local_29[6], local_29[7], local_29[8], local_29[9], local_25, &local_10);
						if (local_12->cluster_index == arg_0 || local_25 > local_3)
						{
							local_2 = *(long *)((byte *)local_29 + 0x4c);
							local_3 = local_25;
						}
						local_16 = true;
					}
				}
				if (*(real *)(local_23 + 0x68) > 0.0f)
					function_12efb5(&local_9, (real const *)(local_23 + 0x68), local_25);
				local_12->source_kind = 3;
				local_12->source_index = local_14;
				local_12->source_weight = local_25;
			}
		}
		else
		{
local_30:
			real local_31 = 1.0f - local_12->distance / g_547f80;
			if (local_31 < 0.0f) local_31 = 0.0f;
			local_5.weight += local_31;
			local_6.weight += local_31;
			local_7.weight += local_31;
			local_8.weight += local_31;
			local_10.weight += local_31;
			local_12->source_kind = 1;
			local_12->source_index = NONE;
			local_12->source_weight = local_31;
		}
		if (!local_16)
		{
			real local_32 = 1.0f - local_12->distance / g_547f80;
			if (local_32 < 0.0f) local_32 = 0.0f;
			local_10.weight += local_32;
		}
	}
	function_12efb4(&local_5, arg_3->field_1c);
	function_12efb4(&local_6, arg_3->field_34);
	function_12efb4(&local_7, arg_3->field_c8);
	if (local_8.weight > 0.0f && local_8.intensity > 0.0f)
	{
		real local_33 = 1.0f / local_8.weight;
		arg_3->field_4c[0] = local_8.color.red * local_33;
		arg_3->field_4c[1] = local_8.color.green * local_33;
		arg_3->field_4c[2] = local_8.color.blue * local_33;
		arg_3->field_4c[3] = local_8.intensity * local_33;
	}
	if (local_2 != NONE && local_10.weight > 0.0f && local_10.c[1] > 0.0f)
	{
		real local_34 = 1.0f / local_10.weight;
		arg_3->field_70[0] = local_10.a[0] * local_34;
		arg_3->field_70[1] = local_10.a[1] * local_34;
		arg_3->field_70[2] = local_10.a[2] * local_34;
		arg_3->field_70[3] = local_10.b[0] * local_34;
		arg_3->field_70[4] = local_10.b[1] * local_34;
		arg_3->field_70[5] = local_10.b[2] * local_34;
		arg_3->field_70[6] = local_10.c[0] * local_34;
		arg_3->field_70[7] = local_10.c[1] * local_34;
		arg_3->field_70[8] = local_10.c[2] * local_34;
		arg_3->field_70[9] = local_10.c[3] * local_34;
		arg_3->field_6c = local_2;
	}
	if (local_9.field_10 > 0.0f)
	{
		real local_35 = 1.0f / local_9.field_10;
		arg_3->field_e0[0] = function_12efb1((1.0f - local_9.field_14) * local_9.field_0.red * local_35);
		arg_3->field_e0[1] = function_12efb1(local_9.field_0.green * local_35);
		arg_3->field_e0[2] = function_12efb1(local_9.field_0.blue * local_35);
		arg_3->field_e0[3] = local_9.field_c * local_35;
	}
}
