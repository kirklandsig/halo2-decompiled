// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_249E20.CPP: lookups in the structure bsp's audibility data.
   Decompiled by lane F: 0x18c3b0 calls them with register arguments. */

#include "unknown_11c920.h"
#include <string.h>
#include "unknown_249e20.h"

// @retail 0x249e20
long function_249e20(s_structure_bsp_view *bsp, long index)
{
	long result = NONE;

	if (index != NONE && bsp->audibility_count > 0)
	{
		s_structure_audibility *audibility = bsp->audibility;
		for (long i = 0; i < audibility->cluster_count; i++)
		{
			if (audibility->clusters[i] == index)
			{
				result = i;
				break;
			}
		}
	}
	return result;
}

// @retail 0x249e60
long function_249e60(long cluster_index, s_structure_bsp_view *bsp, long index)
{
	long result = NONE;

	if (bsp->audibility_count > 0)
	{
		s_structure_audibility *audibility = bsp->audibility;
		long row = 2 * index;
		dword *bits = audibility->bits;
		long stride = (bsp->cluster_count + 31) >> 5;

		if (bits[stride * row + (cluster_index >> 5)] & (1 << (cluster_index & 0x1f)))
		{
			result = 0;
		}
		else if (audibility->bits[stride * (row + 1) + (cluster_index >> 5)] & (1 << (cluster_index & 0x1f)))
		{
			result = 1;
		}
	}
	return result;
}

/* ---- lane O: the values kept for each pair of clusters ---- */

/* the index of the pair (a, b), a < b, in a triangular table of n clusters */
static inline long cluster_pair_index(long a, long b, long n)
{
	return (n - 1) * a - (a + 1) * a / 2 + b - 1;
}

static inline long cluster_minimum(long a, long b)
{
	long result = b;

	if (a <= b)
		result = a;
	return result;
}

static inline long cluster_maximum(long a, long b)
{
	long result = b;

	if (a > b)
		result = a;
	return result;
}

/* the byte the bsp keeps for a pair of clusters (bit 7: audible, the rest
   a distance) */
// @retail 0x249ed0
byte function_249ed0(short cluster_a, short cluster_b, s_structure_bsp_view *bsp)
{
	if (cluster_a != cluster_b)
	{
		if (cluster_a > cluster_b)
		{
			short swap = cluster_a;

			cluster_a = cluster_b;
			cluster_b = swap;
		}
		return bsp->cluster_pair_values[(bsp->cluster_count - 1) * cluster_a - (cluster_a + 1) * cluster_a / 2 + cluster_b - 1];
	}

	return 0;
}

/* true when sound carries between two clusters */
// @retail 0x249c20
bool function_249c20(s_structure_bsp_view *bsp, long cluster_a, long cluster_b)
{
	bool result = false;

	if (cluster_a != cluster_b)
	{
		if (bsp->audibility_count > 0)
		{
			dword *bits = bsp->audibility->cluster_pair_bits;
			long low = cluster_minimum(cluster_a, cluster_b);
			long high = cluster_maximum(cluster_a, cluster_b);
			long index = cluster_pair_index(low, high, bsp->cluster_count);

			result = (bits[index >> 5] & (1 << (index & 0x1f))) != 0;
		}
		else
		{
			result = function_249ed0((short)cluster_a, (short)cluster_b, bsp) >> 7;
		}
	}
	return result;
}

/* the two rows of cluster bits of an index (512 bits each at most) */
// @retail 0x249c90
void function_249c90(dword *bits_a, s_structure_bsp_view *bsp, long index, dword *bits_b)
{
	if (bsp->audibility_count > 0)
	{
		s_structure_audibility *audibility = bsp->audibility;
		long row = 2 * index;
		long count = (bsp->cluster_count + 31) >> 5;
		dword *source_b = audibility->bits + count * (row + 1);
		dword *source_a = audibility->bits + count * row;

		memcpy(bits_a, source_a, count * sizeof(dword));
		memcpy(bits_b, source_b, ((bsp->cluster_count + 31) >> 5) * sizeof(dword));
	}
	else
	{
		memset(bits_a, 0, 0x40);
		memset(bits_b, 0, 0x40);
	}
}

/* the row of door bits of an index (128 bits at most) */
// @retail 0x249d10
void function_249d10(s_structure_bsp_view *bsp, dword *bits, long index)
{
	if (bsp->audibility_count > 0)
	{
		s_structure_audibility *audibility = bsp->audibility;

		if (audibility->door_count > 0)
		{
			long count = (audibility->door_count + 31) >> 5;

			memcpy(bits, audibility->door_bits + count * index, count * sizeof(dword));
		}
	}
	else
	{
		memset(bits, 0, 0x10);
	}
}

/* the distance sound travels between two clusters */
// The default is a ternary on the same test as the branch below: VC then loads both
// constants before the branch, as retail does, and threads the repeated test.
// score 60 (was 60): No local frame; shared zero/default entry values and native endpoint/exit ownership remain.
// retail tests the byte for 0 first, then 0xff, keeps a 255 default in XMM0
// and a shared zero in XMM1; ours tests the word interior first and builds each zero. Kept: no
// size pragma and the audibility record in a named pointer (60). Pragma removed with the
// direct reads 65; byte tests (0, 0xff, else) 76; byte tests under the pragma 88; byte tests
// with an always-true `value < 0xff` interpolation test 47 (it keeps the 255 default and a
// redundant jae). On the kept form: byte value 64; if/else for the ternary 60; tests 0 / 0xff /
// else on the word 71; `value != 0 && value != 0xff` 70, with a byte 75. Earlier: maximum sentinel first 88; interior interval first 82 kept;
// integer sample 88; read-only audibility 88; ordered cluster parameters 91; named pair
// index/range 88/88; direct fallback 84; both boundaries 82; unequal-cluster scope 81; fallback
// first 81; total result assignment 82; zero/upper result defaults 80/80; word/short samples
// 76/76; native count observation 82; existing minimum/maximum/both/index force-inline
// declarations 82/82/82/82; fallback inline/noinline 82/82; extrema before receiver 81; high
// before low 82; named fraction/span-first/bounds-after-sample/bounds-before-table
// 82/82/82/82; full/final interpolation expressions 82/82; native endpoint tests with/without
// fraction 85/85; sample offset long/short/word 78/65/77; direct receiver 65; table-operation
// first 65; explicit endpoint results 79; word-path returns 65; shared index row parts 76.
// Codex N: wrapped byte offset/native byte pair/record receiver 78/81/80; byte endpoints 83;
// zero/upper defaults with byte offset 75/78; fraction/production exits 78/79;
// fallback masked byte/word/long/dword carriers 78/78/78/78; lower field observations 77;
// produced span 65; both bound fields 77; byte sample with direct bounds 83.
// Real-sample lower/upper/both/endpoint/lower+endpoint/upper+endpoint/all predicates 68/82/76/74/73/82/76; all-real with direct bound span 78.
// Ordered zero/upper guards word/byte/long/short 71/76/76/71; upper-first word/byte 71/76; nested sub-upper range word/byte 67/71.
// Round 186 complete conditional result word/byte/short/long 73/78/73/79;
// the same result returned at its table boundary 73/78/73/79. Original float tree preserved.
// Round 135 byte decode helper writing through a result pointer (255 default) 77, with a zero
// default 87, on a word 72; returning decode helper 77; decode helper with a constant
// maximum argument 80 (the dead constant argument is dropped too).
// Round 11 (byte value, statement interpolation unless noted): result 0 default under
// `if (a != b)` with 0xff then nonzero tests 88 (159 bytes); early returns 81 (163); one
// if/else-if result chain with the expression interpolation 79 (168: retail's code and
// epilogues except the entry constants) and with the statement interpolation 76 (171); the
// 255 default with the decode written as two stages (0 / 0xff, then `value != 0 && value !=
// 0xff`) loads retail's 255 at entry at retail's 182 bytes but re-tests the byte, 65.
// Round 177: Entry cluster extrema with original/normalized equality: word interval/
// zero-first 66/71, byte interval/zero-first 72/76 for each. Original late extrema,
// receiver, sample, interpolation and fallback restored.
// @retail 0x249d60
real function_249d60(s_structure_bsp_view *bsp, long cluster_a, long cluster_b)
{
	real local_0 = cluster_a != cluster_b ? 255.0f : 0.0f;
	if (cluster_a != cluster_b)
	{
		if (bsp->audibility_count > 0)
		{
			s_structure_audibility *audibility = bsp->audibility;
			long low = cluster_minimum(cluster_a, cluster_b);
			long high = cluster_maximum(cluster_a, cluster_b);
			byte value = audibility->cluster_pair_distances[cluster_pair_index(low, high, bsp->cluster_count)];
			if (value == 0)
				local_0 = 0.0f;
			else if (value == 0xff)
				local_0 = 256.0f;
			else
			{
				real local_1 = audibility->distance_lower;
				real local_2 = audibility->distance_upper;
				local_0 = (real)(value - 1) * (1.0f / 253.0f);
				local_2 -= local_1;
				local_0 *= local_2;
				local_0 += local_1;
			}
		}
		else
			local_0 = (real)(function_249ed0((short)cluster_a, (short)cluster_b, bsp) & ~0x80) * (256.0f / 127.0f);
	}
	return local_0;
}
