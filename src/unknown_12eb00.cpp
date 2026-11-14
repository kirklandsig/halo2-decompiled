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
// Round 464: absolute byte-span induction in accumulation/blend/both 172/80/142;
// named span ends 172/80/142; both exact-end tests 142; accumulation destination
// span cursor 534. Original inner displacement lifetimes restored.
// Round 59 (fast): the offset table as an {offset, size} struct array 11 (same code); the weights
// zeroed by an initializer 221; the entry lookup as a ternary 29; blend terms entry-first 11;
// the blend accumulator zeroed by `= {0}` 216 (1256 bytes). 1-8 unused locals at the top (diagnostic) 11 each:
// the slot tie does not follow the symbol count.
// Round 104: outer-index preincrement/compound increment 11/11; index renamed local_2/cluster_ordinal
// 11/11; function-scope index before the result/between result and table/after the table 11/11/11;
// initialized at its declaration 11; on that scope, result preincrement/full assignment 11/11. Restored.
// Round 120 (fast; ours ends `ret 8` like retail): the result declared after the table 72; an early
// `return 0` for a missing fog table 304; a const offset table 11; `continue` for a missing or empty
// entry 11; the blend offset written out at each of its three uses 113. Restored.
// Round 75 (fast; ours ends `ret 8` like retail): the blend source looked up without the field_c local 11;
// the weight read from the array at each use 30; the sample stored before the sum 11; `local_7 *=` 29;
// a sample-count local for both loops 173 (1175 bytes); previous index after the copy 16 (again).
// Round 116 (fast): cluster parameter renamed 11; result declaration separated from initialization 72
// (1181 bytes); walking output cursor 81 (1193); guarded do loop 262 (1196); accumulator byte subscript 11;
// typed component index 76 (1194); accumulator pointer local 11 (NOT kept: one-use address local);
// walking cluster parameter 305 (1174). Original body restored.
// Round 210 (fast): the cluster record re-read at each use, no record local 218; the output record named beside the
// previous-index store and read through it in the blend 14 (the two parameter loads flip, homes do not); the blend
// accumulator or all three addresses as `&((byte *)base)[offset]` 11/11; the blend products weight-first, entry term
// first 11; the output record initialised `output + result` 11. Diagnostic: the previous-index store at the end of the
// block beside the increment 109 (1178 bytes; the result moves to 0x2c).
// Round 152 (m3 lane 6, fast): the previous-index store at the block top / after the memset / after the inverse 99/99/99
// (homes unchanged); with the copy also at the block top 112 (1194 bytes); `continue` when the sum is not positive 11, with
// `continue` for a missing or empty entry as well 11 (same code); the clusters parameter first (definition and the 0x12efb0
// call) 11. Diagnostics: no final read of the result moves it to 0x2c and the cluster cursor to retail's 0x18 (306); no
// previous-index store 110. A 600 s permuter run reached 10 only through a volatile weight read and casts (not applied).
// Round 155 (m3 lane 6, fast): the palette index in a named `signed char` local for the test and the lookup 219; the output
// record copied by memcpy (real build) 11; the blend's `1.0f - product` named before the copy 99 (1208 bytes); the palette
// entry lookup as an if/else assignment 29; the lookup helper with two returns and no result local 73.
// Round 80 (m3 lane 10, fast; ours ends `ret 8` like retail): a reference to the cluster element for the index read and the previous-index store
// 11; `0.0f < sum` 11; a pointer to the palette source entry 11; the accumulation product weight-first 11; the entry test as nested ifs 11.
// Round 101 (m3 lane 12, fast): the blend written entry term first 11 (same code); count as the last parameter (with the
// 0x12efb0 call) 11 (same code); the accumulator as a byte array or a real array of the record's size 11/11 (same code); the
// record copy ahead of the previous-index store 16; a walking cluster pointer declared before the result and stepped in the
// for header 68, declared after the result 68, declared at the loop 15, with a walking output pointer as well 317 (1172
// bytes). In all three pointer forms the result keeps 0x18 and the pointer takes 0x1c, so declaration order does not break
// the tie between the two homes.
// Round 85 (m3 lane 14, fast; ours ends `ret 8` like retail in both the fast and the full build): the increment moved
// above the blend, which then writes `output[local_1 - 1]`, 93; the result declared and returned inside the table test
// with `return 0` after it 293; the blend reading the copied entry back from the output record 297 (1197 bytes);
// `1.0f - local_7` named right after the copy 98 (1208); the product update as `(1.0f - local_12) * local_7` 11 (same
// code). Re-measured, not counted: the three blend addresses as `offset + base` in all eight combinations 11 (identical
// code: the operand order in source never reaches the [eax + edx] address).
// Round 111 (m3 lane 16, fast; control 11): a walking output pointer beside the result, declared ahead of the table test with
// the pointer stepped after the result 51, stepped before it 64 (1193 bytes), declared inside the table test 15 (the result
// keeps 0x18 and the cluster cursor 0x1c, so the output indexing does not weigh the result); the entry pointer declared at
// function scope 11; the sum and the product declared at function scope ahead of the result 11; both sample indices declared
// at function scope 11 (11 = same code). The low homes are ordered by reference count (8, 7, 5, 5, 5, 4, 4, 3, 3, 2): the
// cluster cursor, the result and the output cursor tie at five, and retail orders that tie cursor, result, output cursor where
// ours orders it result, cursor, output cursor. Diagnostics: the result first set inside the table test 75 (1181 bytes, homes
// unchanged); an extra store through the cluster element at the loop top 249. The other two ties (sum before entry pointer,
// product before the remaining count) sit as if ordered by name, so: a cluster element pointer for both accesses named
// local_2, zcluster, cluster or a_cluster 11 each (same code); an explicit walking cluster pointer stepped at the loop end,
// named zcluster, cluster or local_2 and declared at the loop 15 each, declared at function scope before or after the result
// 15 each. In every one the result keeps 0x18 and the pointer 0x1c: neither its name nor its declaration place orders the tie.
// Round 163 (m3 lane 8, fast; ours ends `ret 8` like retail, the `ret 0xc` in the listing is 0x12efb0's, the same in both):
// 1-7 initialised dead locals at the top (diagnostic phase scan) 11 at every count, the `[eax + edx]` address included, so
// it does not follow the symbol count; the result as `dword` 11; the output index taken by a post-increment before the
// previous-index store and used for the copy and blend 178 (1198 bytes); the two pin tests as nested ifs 11 (same code);
// the pins written bound-first (`lo > x ? lo : hi < x ? hi : x`) 15.
// Also round 163: the result as `short` 306 (1177 bytes); its increment as `local_1 = local_1 + 1` 11 (same code).
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
