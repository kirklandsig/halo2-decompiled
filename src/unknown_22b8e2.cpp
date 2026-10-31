// @flags /O1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "camera_storage.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "bitmap_sequences.h"

struct s_4e6950
{
	byte field_0[0x28];
	long field_28, field_2c;
	color4f field_30;
	byte field_40[0x40];
};
extern s_4e6950 g_4e6950;
struct s_filter_vector;

struct s_22b8e2
{
	byte field_0[0x1c];
	word field_1c;
	byte field_1e[6];
	long field_24;
	byte field_28[4];
	long field_2c;
	byte field_30[4];
	long field_34;
	byte field_38[4];
	long field_3c;
	signed char field_40[3];
	byte field_43;
	short field_44[6];
};

long function_13a690(long);
bool function_22acb4(long);
void function_1396c7(long, point2f *);
void function_2d140(long, real const *, long, long, s_filter_vector const *);

// score 6 (was 6): only the schedule of two bound conversions differs: retail converts the
// bottom bound after the first two right-bound stores and the top bound after the first left-bound
// store; ours converts each before them. Round 18 (lane 14): the bitmap tag read through the
// sequence/sprite structs 0x22ae21 uses, with the index declared in each count branch (zero tests
// against the zero register, sprite address [eax + ebx]); the offsets added in place to the point
// as 0x22aa16 does (retail's switch layout, the default path reloads the point after the join); the
// blends computed x-low, y-low, x-high, y-high: 64 -> 6. Tried on the way: default arm first 64; default
// assigning y first 64; point copied before the switch with overwriting cases 128; index declared per
// branch alone 63. Compile-only screens on 6 (no change from 4 harness differences): the bound and
// reciprocal declarations in all 720 orders, the grid rows in all 720 orders, each blend product's
// operand order (16), the global stores moved before the reciprocals/grid/blends/blend stores 4/20/20/20.
// Earlier (score 64 (was 64)): the offsets switch's default block still comes first instead of last and the
// sums still sit in xmm4/xmm5 instead of xmm5/xmm6; the two zero tests are still test reg,reg
// instead of cmp against the zero register.
// Earlier (score 64 (was 114)): the offsets switch sums each case's pair onto the point and the default
// copies the point, the x reciprocal comes first with the four bounds converted in retail's
// dd2, dd6, dd4, dd0 order, and the grid's column 0/1 stores precede the four blends.
// Still: the offsets switch keeps its default block first instead of last and the sums sit in
// xmm4/xmm5 instead of xmm5/xmm6, and two of the zero tests are test reg,reg instead of cmp.
// Earlier (score 114 (was 120)): permuted declaration order (local_9 before local_8, local_16 before
// local_15), the case-2 offset written 4 + field_44 and the short local_10 split from its
// initialiser. Mode/resource/point homes and offset/bound schedule remain.
// Earlier (score 120 (was 120)): mode/resource/point frame homes permuted, offset join and bound schedule.
// Retail's offset switch has no return: each case converts its pair and shares the adds, the
// default keeps the point. Per-case sums with a default copy 157 (switch shape retail's, frame
// and bound schedule worse; case/default order 157); offsets added in place 145; a copy plus
// `+=` per case 163; the two reciprocals first 144, with the per-case sums 160 (bound block start
// now retail's, frame +8); the table filled vertex by vertex 173/180.
// Native offset/resource/sequence data declarations, all seven combinations 120;
// complete native graph plus case-local converted pairs/default point 163.
// Mode/resource/point homes, offset join and bound scheduling remain; original declarations retained.
// Round 208: native four-float grid with pointer-selected offsets, bounds read
// everywhere/from reused integer inputs 146/120; case-converted offsets 159/139.
// Early grid declaration repeats those four scores. All body/type changes restored;
// mode/resource/point homes and offset/bound schedule remain.
// Round 424: actual 0..2 mode in signed byte/byte/short/word 210/210/210/210;
// each with both native integer spans named at the reciprocals 210/210/210/210.
// All restored; mode/resource/point homes and offset/bound schedule remain.
// Round 42: the per-case sums put mode and resource in retail's homes (aligned 118 -> 106) but swap
// the point and the result id (the compile-only build gives retail's order there, LTCG does not);
// on them the point/result/x,y declarations moved or default first 157 each (identical), the result
// initialised to NONE at its declaration 172 (alone 134); the bounds converted in retail's dd2, dd6,
// dd4, dd0 order 123, with the reciprocals first 143, with the per-case sums too 159.
// Round 211: plain grid x-row/y-column/both/x-columns loops
// 108/108/128/218.
// Three horizontal spans lose matched caller 0x13a532; rejected.
// parallel span/distance/both/weight/span+weight/distance+weight/all/all+integer-bound producers
// 6/6/6/6/6/6/6/188.
// All source forms restored; bottom/top conversion scheduling remains.
// Round 12 (m3 lane 2; exact one-link harness: this file with 0x13a532's, 0x1396c7's, 0x22acb4's and
// 0x2d140's files): grid as 18 separate statements 6; grid as a vertex struct {x, y, u, v} 6; bounds as
// two real_bounds 6; bounds as an s_float_rect 6; each bound converted just before its first grid row 7.
// Harness only: top/bottom converted after grid row k (k = 1..6) moves each conversion after all three
// stores of a row, never between (positional 6-9); the blends among the grid rows 7-104; the global
// stores before the point/reciprocals/conversions/grid 206/181/93/77 (positional).
// Round 96 (exact one-link harness, 6 there): the four bound and two reciprocal declarations in all 720
// orders, min 6 (15 orders at 6, next 21); top/bottom converted inside the column 1 chains
// (`... = local_14 = (real)g_4b9dd4;`) 7 (each conversion now lands one store after retail's instead of
// one before); both reciprocals before the blends/before the column 1 chains 190/204, the y reciprocal
// alone there 193/193; the grid as a flat real[36] 6; every chain written in reverse order 6 (same code).
// @retail 0x22b8e2
void function_22b8e2(s_22b8e2 const *arg_0, long arg_1, color4f const *arg_2)
{
	long local_0 = function_13a690(arg_0->field_1c);
	long local_1;
	point2f local_4;
	long local_2;
	switch (local_0)
	{
	case 0: local_1 = arg_0->field_2c; break;
	case 1: local_1 = arg_0->field_34; break;
	case 2: local_1 = arg_0->field_3c; break;
	default: return;
	}
	if (local_1 == NONE)
		return;
	local_2 = NONE;
	long local_3 = 0;
	switch (local_0)
	{
	case 0: local_3 = arg_0->field_40[0]; break;
	case 1: local_3 = arg_0->field_40[1]; break;
	case 2: local_3 = arg_0->field_40[2]; break;
	}
	if (arg_0->field_24 != NONE)
	{
		s_bitmap_tag_22ae21 const *tag = (s_bitmap_tag_22ae21 const *)g_4e3b44[arg_0->field_24 & 0xffff].bytes;
		if (local_3 >= 0 && local_3 < tag->sequence_count)
		{
			s_bitmap_sequence_22ae21 const *sequence = &tag->sequences[local_3];
			if (sequence->sprite_count > 0)
			{
				long index = 0;
				if (sequence->sprite_count > 1 && function_22acb4(arg_1))
					index = 1;
				local_2 = sequence->sprites[index].bitmap_index;
			}
			else if (sequence->bitmap_count > 0)
			{
				long index = 0;
				if (sequence->bitmap_count > 1 && function_22acb4(arg_1))
					index = 1;
				local_2 = sequence->first_bitmap + index;
			}
		}
		else if (local_3 == 0 && tag->sequence_count == 0 && tag->bitmap_count > 0)
			local_2 = 0;
	}
	function_1396c7(arg_0->field_1c, &local_4);
	switch (local_0)
	{
	case 0: local_4.x += arg_0->field_44[0]; local_4.y += arg_0->field_44[1]; break;
	case 1: local_4.x += arg_0->field_44[2]; local_4.y += arg_0->field_44[3]; break;
	case 2: local_4.x += arg_0->field_44[4]; local_4.y += arg_0->field_44[5]; break;
	}
	real local_15 = 0.5f / (g_4b9dd6 - g_4b9dd2);
	real local_16 = 0.5f / (g_4b9dd4 - g_4b9dd0);
	real local_11 = (real)g_4b9dd2;
	real local_13 = (real)g_4b9dd6;
	real local_14 = (real)g_4b9dd4;
	real local_12 = (real)g_4b9dd0;
	real local_21[9][4];
	local_21[8][0] = local_21[5][0] = local_21[2][0] = local_13;
	local_21[6][0] = local_21[3][0] = local_21[0][0] = local_11;
	local_21[7][0] = local_21[4][0] = local_21[1][0] = local_4.x;
	local_21[8][1] = local_21[7][1] = local_21[6][1] = local_14;
	local_21[2][1] = local_21[1][1] = local_21[0][1] = local_12;
	local_21[5][1] = local_21[4][1] = local_21[3][1] = local_4.y;
	real local_17 = 1.0f - (local_4.x - local_11) * local_15;
	real local_19 = 1.0f - (local_4.y - local_12) * local_16;
	real local_18 = 1.0f - (local_13 - local_4.x) * local_15;
	real local_20 = 1.0f - (local_14 - local_4.y) * local_16;
	local_21[0][2] = local_17;
	local_21[0][3] = local_19;
	local_21[1][2] = 1.0f;
	local_21[1][3] = local_19;
	local_21[2][2] = local_18;
	local_21[2][3] = local_19;
	local_21[3][2] = local_17;
	local_21[3][3] = 1.0f;
	local_21[4][2] = 1.0f;
	local_21[4][3] = 1.0f;
	local_21[5][2] = local_18;
	local_21[5][3] = 1.0f;
	local_21[6][2] = local_17;
	local_21[6][3] = local_20;
	local_21[7][2] = 1.0f;
	local_21[7][3] = local_20;
	local_21[8][2] = local_18;
	local_21[8][3] = local_20;
	g_4e6950.field_28 = arg_0->field_24;
	g_4e6950.field_2c = local_2;
	g_4e6950.field_30 = *arg_2;
	function_2d140(local_1, (real const *)arg_2, 3, 3, (s_filter_vector const *)local_21);
}
