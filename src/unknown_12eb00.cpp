// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "scenario_ramps.h"
#include <string.h>

struct s_fog_cluster_distance
{
	long cluster_index;
	real distance;
	byte unknown08[0xc];
	long previous_index;
};

struct s_12ec00
{
	byte field_0[0xc];
	short field_c;
	short field_e;
};

struct s_12eb00
{
	byte field_0[0xd8];
	long field_d8;
	s_12ec00 *field_dc;
	byte field_e0[0x14];
};

struct s_12ed00
{
	byte field_0[0x340];
	long field_340;
	s_12eb00 *field_344;
	byte field_348[0x3c0 - 0x348];
	long field_3c0;
};


/* the entry at index in the table, or NULL when the index is out of range */
static inline s_12eb00 *entry_at_index(s_12ed00 *table, long index)
{
	s_12eb00 *entry = NULL;
	if (index >= 0 && index < table->field_340)
		entry = &table->field_344[index];
	return entry;
}

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

// score 11 (was 11): the result and cluster cursor homes are swapped (0x18 and 0x1c; both have
// five references) and the blend's accumulator load addresses [eax + edx] for [edx + eax]. Round 102:
// the table global read at each use (no local copy reloaded after each output) with a plain indexed
// loop fixed both pins and the blend store (21 -> 11). On it, one-file LTCG harness aligned counts
// (base 12 there, one extra reload after the ramp call): cluster/output/both pointer locals 12/12/12;
// walking cluster pointer 15, declared first 19; walking output pointer 14, both declared first 23;
// parameters walked (clusters/output/both) 94/93/77; result declared after the table 40; previous
// index stored after the copy 13, at the end 39; `+= 1` 12; `count > 0` guard 12; while loop 12;
// named inverse 33; blend terms swapped, offsets swapped, lerp operand orders, memcpy copy 12 each.
// On the old body (harness 21): renamed result/cursors/remaining/entry/table 21 each; cursor
// declaration orders 21/22/24/25/23/25; `x == PIN` 23; `(long)x` 21; `-1 + n` 23; nested ifs 21;
// `x >= lo` form 55; CEILING macro 21; `x <= hi ? x : hi` 23; `lo > x` 21. Real --fast on the 11 body:
// cluster pointer local 11, output pointer local 11, previous index after the copy 16, `+= 1` 11, while 11.
// Earlier at 21: the count/cursor homes and the two index pins still differ, and the
// blend store addresses [esi + eax] for [eax + esi]. Index pins only as x <= hi ? x : hi or
// x < hi ? x : hi 21/21 (arm order never moves the result register); hi >= x ? x : hi or
// hi < x ? hi : x 19/19 (compare order only); indexed for loop over clusters[i] 27, with the
// count guard 23; for loop stepping the cursor 23; named contribution 20; hi < x pins with the
// named contribution 18. The product update written out fixed
// the blend's operand order (40 -> 21). Tried: an interpolation helper for the blend 40;
// no element pointer 215; long index locals 259; count declared after the table pointer 82;
// the running sum written out 21; a 600 s permuter run, none below 21.
// Earlier: the count and cluster cursor homes are swapped (0x18 and
// Earlier at 40 (was 136): the count and cluster cursor homes were swapped (0x18 and
// 0x1c); the two index pins keep the bound rather than the index in the result
// register (jg for jle); the final blend multiplies the weight product by
// memory instead of loading the field first, and stores through [esi + eax].
// Tried: cursor copies declared clusters-first 43; parameters as cursors 328;
// count declared after the table 99; cluster cursor declared first 95; pins as
// x <= hi ? x : hi or x < hi ? x : hi 40, hi < x ? hi : x 38 (cmp order only),
// an inline pin function 254-263, equality operands swapped 42; blend terms or
// factors swapped 40. Earlier: unsigned table 136; indexed output 140;
// indexed clusters 136; component accumulation 538; component blend 615.
// Traversal count/cluster/output scopes 21/21/21; all three scopes 21;
// accumulation record before ramps/function scope 21/21; count initialized
// after table 82; earlier table-receiver scope with unchanged load point 21.
// Entry/ramp/both bound expressions 49/199/200; input/output/both cursors
// initialized before the table 78/78/317; const element view 21; int result 21.
// Count/cursor homes, pin-result registers and blend address remain.
// Accumulation row cursor 116; blend row cursor 91; both row cursors 142;
// accumulation short ordinal 159; blend short ordinal 110; both short ordinals 178;
// unsigned output count 21; unsigned remaining count 21. Restored.
// Named entry/ramp/both pin results 21/21/21; explicit count update 21;
// contribution part 20; named blend 21; full accumulation/output-step assignments 21/21.
// Round 15 (ours ends `ret 8` like retail): clusters and output indexed by a for loop and the count, no
// cursors 21 (identical code); pins as hi < x ? hi : x 19; short index and long pinned locals in nested tests
// 21; cursor copies output/remaining/clusters 22; pins as x >= lo ? (x <= hi ? x : hi) : lo 29; renamed
// result/cursors 21; count parameter second or last (caller updated) 21; result read once in
// `previous_index = count++` moves its slot to +0x2c (101). The result is the heaviest spilled local.
// Output pointer addition 21; offset addition 21; hi < x pins with a named contribution 18;
// with unsigned final/accumulation offsets 18/18; with an outer countdown while 18;
// reversed pin order 239; nested pin tests 18 (fast). Restored: best gain only 3.
// Round 186 (ours ends `ret 8` like retail; screens of the reduced image, real --fast where marked):
// the result renamed added/count_out/output_count/result/written 11 each (result real 11); the
// accumulation/blend address terms as `offset + (byte *)base` (accumulator, entry, output) 11 each
// (accumulator real 11); blend terms swapped 11; `output + result` 11; the offset sum swapped 11; a
// walking cluster pointer declared before the loop and stepped at the loop end 15 (real 15, same
// homes), with the result renamed added/result/written 15 each; that pointer at function scope
// before/after the result 68/68; a cluster element pointer 11; the accumulator declared at the
// cluster block top / function top 11/11 (real 11); the inverse before the memset 11; the sample
// array declared first 11; the product declared before the sum 15 (real 15).
// @retail 0x12eb00
long function_12eb00(long count, s_fog_cluster_distance *clusters, s_12eb00 *output)
{
	long local_1 = 0;
	long local_0[18][2] = {
		{4, 12}, {0x10, 4}, {0x18, 4}, {0x1c, 4}, {0x20, 4}, {0x24, 12},
		{0x34, 4}, {0x38, 4}, {0x3c, 4}, {0x44, 12}, {0x50, 4}, {0x54, 4},
		{0x58, 4}, {0x88, 24}, {0xa0, 4}, {0xa4, 4}, {0xa8, 4}, {0xac, 4}
	};
	if (g_4e0350)
	{
		for (long i = 0; i < count; i++)
		{
			byte *local_3 = *(byte **)((byte *)g_4e0348 + 0xa0) + clusters[i].cluster_index * 0xb0;
			s_12eb00 *local_5 = NULL;
			if (*(signed char *)(local_3 + 0x6f) != NONE)
				local_5 = entry_at_index((s_12ed00 *)g_4e0350, *(signed char *)(local_3 + 0x6f));
			if (local_5 && local_5->field_d8 > 0)
			{
				real local_6 = 0.0f;
				real local_7 = 1.0f;
				real local_8[2];
				memset(local_8, 0, sizeof(local_8));
				for (long local_9 = 0; local_9 < local_5->field_d8; local_9++)
				{
					s_12ec00 *local_10 = &local_5->field_dc[local_9];
					if (PIN(local_10->field_c, 0, ((s_12ed00 *)g_4e0350)->field_340 - 1) == local_10->field_c &&
						PIN(local_10->field_e, 0, ((s_12ed00 *)g_4e0350)->field_3c0 - 1) == local_10->field_e)
					{
						real local_12 = scenario_ramp_evaluate_current(local_10->field_e);
						local_12 = PIN(local_12, 0.0f, 1.0f);
						local_6 += local_12;
						local_8[local_9] = local_12;
						local_7 = local_7 * (1.0f - local_12);
					}
				}
				if (local_6 > 0.0f)
				{
					s_12eb00 local_13;
					memset(&local_13, 0, sizeof(local_13));
					real local_14 = 1.0f / local_6;
					for (long local_15 = 0; local_15 < local_5->field_d8; local_15++)
					{
						real weight = local_8[local_15];
						if (weight > 0.0f)
						{
							long local_16 = local_5->field_dc[local_15].field_c;
							s_12eb00 *local_18 = entry_at_index((s_12ed00 *)g_4e0350, local_16);
							for (long local_19 = 0; local_19 < 18; local_19++)
							{
								for (long local_20 = 0; local_20 < local_0[local_19][1]; local_20 += 4)
								{
									long local_21 = local_0[local_19][0] + local_20;
									*(real *)((byte *)&local_13 + local_21) += *(real *)((byte *)local_18 + local_21) * weight * local_14;
								}
							}
						}
					}
					clusters[i].previous_index = local_1;
					output[local_1] = *local_5;
					for (long local_22 = 0; local_22 < 18; local_22++)
					{
						for (long local_23 = 0; local_23 < local_0[local_22][1]; local_23 += 4)
						{
							long local_24 = local_0[local_22][0] + local_23;
							*(real *)((byte *)&output[local_1] + local_24) = *(real *)((byte *)&local_13 + local_24) * (1.0f - local_7) + *(real *)((byte *)local_5 + local_24) * local_7;
						}
					}
					local_1++;
				}
			}
		}
	}
	return local_1;
}
