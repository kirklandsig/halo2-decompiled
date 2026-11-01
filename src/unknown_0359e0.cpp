// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0359E0.CPP: bitmap dimensions and coordinate interleaving */

#include "unknown_11c920.h"

#define MAXIMUM(a, b) ((a) > (b) ? (a) : (b))
#define IS_POWER_OF_TWO(x) (!((x) & ((x) - 1)))

long log2_floor(dword value);

// @retail 0x1d6f0
dword function_1d6f0(bool linear, bool alternate, dword format, long width, long height)
{
	/* Both dimensions occupy stack slots in retail. */
	long const *width_reference = &width;
	long const *height_reference = &height;
	dword type = linear ? (alternate ? 0x2e : 0x12) : (alternate ? 0x2e : 6);
	format = (((format << 8) | type) << 8) | 0x29;
	if (!linear)
	{
		long width_bits = 1;
		long height_bits = 1;
		while ((1 << width_bits) < *width_reference)
			++width_bits;
		while ((1 << height_bits) < *height_reference)
			++height_bits;
		format |= (width_bits | (height_bits << 4)) << 20;
	}
	return format;
}

word const g_450840[64] =
{
	0x000, 0x001, 0x004, 0x005, 0x010, 0x011, 0x014, 0x015,
	0x040, 0x041, 0x044, 0x045, 0x050, 0x051, 0x054, 0x055,
	0x100, 0x101, 0x104, 0x105, 0x110, 0x111, 0x114, 0x115,
	0x140, 0x141, 0x144, 0x145, 0x150, 0x151, 0x154, 0x155,
	0x400, 0x401, 0x404, 0x405, 0x410, 0x411, 0x414, 0x415,
	0x440, 0x441, 0x444, 0x445, 0x450, 0x451, 0x454, 0x455,
	0x500, 0x501, 0x504, 0x505, 0x510, 0x511, 0x514, 0x515,
	0x540, 0x541, 0x544, 0x545, 0x550, 0x551, 0x554, 0x555,
};

PRIVATE inline long swizzle_log2(dword value)
{
	long result = 0;
	if (value > 0)
	{
		while ((value >>= 1) != 0)
			++result;
	}
	return result;
}

// score 82 (was 82): the coordinates now take retail's registers (the second in
// cx, the third in dx) with the column local declared first; retail still keeps
// the row coordinate 16-bit in si, promotes the column one after the mask, uses
// log2_floor's loop shape and picks the common count with a branch (ours cmov).
// Tried: log2_floor 87 (the loops then match), the minimum as if/else 87 (msvc
// if-converts it), short copies or no copies 119 (the long copies fix the argument
// registers, and without them the matched caller 0x136140 no longer matches).
// Round 22: the two entry copies are what hold the argument registers (row in cx,
// column in dx), so retail's 16-bit row copy in si and its late column promotion
// cannot be reached from here: a 16-bit copy or a copy placed after the mask moves
// that argument onto the stack (130, 85), and swapping the two copies gives 79 but
// swaps the argument roles, so the matched caller 0x136140 no longer matches.
// until 1; ours shifts first and tests for 0), the minimum is a branch (ours cmov),
// and the parameters stay 16-bit until the mask is built: y is moved to si at entry
// and both are sign-extended there (ours extends both at entry). Tried: swizzle_log2
// with log2_floor's while (value != 1) loop 87 (the loops then match); with it, the
// minimum as if (common_bits > height_bits) or as <, >=, <= ternaries 87, as a long
// 117; only x copied 113; x and y used directly 119, the long copies after the mask
// 100, short copies 119, only y copied 93, the copies swapped 84 (these five change
// how x and y travel and cost matched caller 0x136140); log2_floor called with the
// body rewritten as above 119.
// Earlier: log2_floor called (retail's loops) 87; with x copied first 84 (costs
// 0x136140); with the minimum as if/else 87; mask as long 90; short copies at entry and long
// copies after the mask make x and y stack parameters.
// Round 284: Native output storage throughout, with private/shared logarithms by
// axis, 96/98/95/98; y shift at production with the same boundaries
// 95/99/94/94. Original result locals and both helper calls restored.
// Round 428: genuine masked coordinates as long/short in both arms 99/102; only low arm
// 82/85; only high arm 83/92; both with second-result shift at production 99/102.
// Original coordinate captures, helper, table expressions and tail restored.
// Round 211: existing logarithm signed short/long inputs crossed with bounded short/long counts
// and short/long results, scores 82/82/82/82/82/82/82/82. Original executable form restored.
// Round 48: `!=` in both tail tests 84; outputs stored y first 83; `!=` tests with log2_floor's loop
// 87; `mask < 64` 82 (same code); y looked up before x in both arms 83. Diagnostic only: in a /O2 /Gr
// object (no /arch:SSE) the minimum becomes retail's branch, yet the coordinate promotion and the
// tail's register roles still differ; x and y used directly put y on the stack (ret 0xc).
// Round 444: explicit termination at one with byte/signed-char scan counters and
// long/short/word/byte returns 92 each. Original helper/body restored; coordinate
// promotion, height/common homes and minimum selection remain.
// Lane 15 r17: the existing log2_floor 87 (both loops retail's); retail's minimum is a branch
// although its neighbour 0x359e0 uses cmov, and the same body moved (diagnostic) into an existing
// /O2 /Gr file without /arch:SSE compiles it as a branch: 86, with the minimum as
// `common_bits = width_bits; if (width_bits > height_bits) common_bits = height_bits;` 86 (retail's
// jle/mov shape, but the short minimum stays in EBX and the height count is spilled, retail the
// reverse); there x and y used directly 122 and long copies after the mask 100 (both pass y on the
// stack, matched caller 0x136140 lost), the x copy first 83 (y in ESI as retail, 0x136140 lost).
// So 0x358d0 looks like a lone object built without /arch:SSE between 0x35850 and 0x359e0.
// Round 48: `!=` in both tail tests 84; outputs stored y first 83; `!=` tests with log2_floor's loop
// 87; `mask < 64` 82 (same code); y looked up before x in both arms 83. Diagnostic only: in a /O2 /Gr
// object (no /arch:SSE) the minimum becomes retail's branch, yet the coordinate promotion and the
// tail's register roles still differ; x and y used directly put y on the stack (ret 0xc).
// Lane 10 round 34 (fast, with 0x136140): x and y used directly with the existing log2_floor 119 (y then arrives on
// the stack: LTCG passes only width and x in registers), with swizzle_log2 130; short entry copies with log2_floor
// 119, with swizzle_log2 130 (the compiler drops them); long copies with log2_floor 87; long copies moved between
// the logarithms and the minimum 102, between the two logarithms 97 (both cost 0x136140). Retail's 16-bit `mov si, cx`
// is a register move out of ECX (the loop count needs CL), so retail's source likely used x and y directly; what
// keeps y a register argument there is still unknown.
// Round 461: shared doubled shift count long/short 88/100; both remaining coordinate parts
// long, direct/long/short count 82/88/100; native short parts 87/94/91. Original
// arithmetic, coordinate captures and full body restored; count homes and promotions remain.
// Round 62 (lane 12, fast, with 0x136140): a compile-only screen of this body under /O2 /arch:SSE /Gr turns
// every minimum form (`a > b ? b : a`, `a < b ? a : b`, `c = a; if (a > b) c = b;`, if/else, `c = b;
// if (a <= b) c = a;`, a long result) into cmov, so retail's jle/mov branch still points at a non-SSE object.
// A `MINIMUM` macro beside the file's `MAXIMUM` 82 (same code); that with the existing log2_floor for both
// logarithms 87 (the height count then takes the height argument's slot, where retail spills the common
// count); on that, both long copies after the mask 100 (0x136140 125), y copied at entry and x after the mask
// 93 (0x136140 6), x at entry and y after the mask 93 (0x136140 28).
// Round 104 (lane 8, fast): the tail tests without `else` 111 (251 bytes); a named high mask (used
// twice) 82 (identical); the height logarithm first 100 (0x136140 lost); the y shift folded into each
// branch's lookup 82 (identical); the branch test on `common_bits <= 6` 82 (identical).
// Lane 18 round 16 (fast, with 0x136140 and 0x1362b0; one-link env through 0x1362b0, equal to the full listing): a
// short y copy with x used directly 130 (295 bytes, 0x136140 lost); log2_floor with only x copied (long) 94 (282
// bytes), with only y copied 93 (279 bytes), with short copies 119 (296 bytes), each losing 0x136140; log2_floor
// with the minimum as `common_bits = width_bits; if (common_bits > height_bits) common_bits = height_bits;` 87.
// Env only: that minimum with swizzle_log2 compiles identically to the ternary (cmov either way).
// Round 16 (m3 lane 17, fast; 0x136140 kept): common_bits declared before the height logarithm 82 (same
// code); tail tests as `common_bits < width_bits` / `< height_bits` 91; the minimum as `height_bits <
// width_bits ? height_bits : width_bits` 82 (same code), as `width_bits < height_bits ? width_bits :
// height_bits` 82 (same code); the output words declared with the coordinate copies 82 (same code).
// Round 468: Second-axis count long/word/byte/signed-byte, each with short/word
// common count, 91/129, 99/91, 95/118, 95/94. All controls remain matched; original
// height/common counts and complete body restored; coordinate promotions and count homes remain.
// Round 470: additive/XOR table-band joins 83/83; low-first OR/add/XOR
// 82/83/83; signed full-width outputs with OR/add/XOR 82/83/83. All restored;
// coordinate promotion, height/common homes and minimum selection remain.
// Round 483: six complete width/height/shared count naming orders 82 each;
// first two naming orders with the existing shared logarithm 87/87. Native
// count-home allocation does not change; original names and body restored.
// Round 485: Complement low-bit mask with both tail insertions OR/add/XOR,
// private/shared logarithms 83/83/83/89/89/89; sign-extended mask private/shared
// 84/90. All three controls matched; original mask, helpers and tails restored.
// Round 49 (lane 10, fast, 0x136140 kept unless noted): the outputs as a two-entry bits array 82 (same
// code); a named `common_bits * 2` shift local 100; one inline spread helper for both coordinates 313
// (kept out of line, 0x136140 1); `mask & x_value` lookups 82; `common_bits << 1` 82.
// Round 40 (lane 13, fast): retail's 0x136140 call sets ECX, then EDX, then EAX (the declared order). x and y
// used directly with log2_floor 119 (y colours to EBP and becomes a stack argument; 0x136140 28); that with
// `common_bits = width_bits; if (height_bits < width_bits) common_bits = height_bits;` 119 (still cmov); that
// with y looked up before x in both arms 125 (302 bytes, 0x136140 kept); `long const` copies 82 (same
// code); y_bits declared before x_bits 82 (same code).
// Round 483: six complete width/height/shared count naming orders 82 each;
// first two naming orders with the existing shared logarithm 87/87. Native
// count-home allocation does not change; original names and body restored.
// Round 485: Complement low-bit mask with both tail insertions OR/add/XOR,
// private/shared logarithms 83/83/83/89/89/89; sign-extended mask private/shared
// 84/90. All three controls matched; original mask, helpers and tails restored.
// Round 495: Nonzero scans with negative-seeded index or complete bit length,
// zero-before/after result selection and long/short counts: 101/94/114/95/100/98/112/93.
// Original helper and body restored; coordinate promotion and count homes remain.
// @retail 0x358d0
void function_358d0(short width, short x, short y, short height, dword *out)
{
	long y_value = y;
	long x_value = x;
	short width_bits = (short)swizzle_log2(width);
	short height_bits = (short)swizzle_log2(height);
	short common_bits = width_bits > height_bits ? height_bits : width_bits;
	short mask = (short)((1 << common_bits) - 1);
	dword x_bits;
	dword y_bits;
	if (mask <= 63)
	{
		x_bits = g_450840[x_value & mask];
		y_bits = g_450840[y_value & mask];
	}
	else
	{
		x_bits = (g_450840[(x_value >> 6) & (mask >> 6)] << 12) | g_450840[x_value & 63];
		y_bits = (g_450840[(y_value >> 6) & (mask >> 6)] << 12) | g_450840[y_value & 63];
	}
	y_bits <<= 1;
	if (width_bits > common_bits)
		x_bits |= (x_value >> common_bits) << (common_bits * 2);
	else if (height_bits > common_bits)
		y_bits |= (y_value >> common_bits) << (common_bits * 2);
	out[0] = x_bits;
	out[1] = y_bits;
}

static inline bool function_x9955e3(long format)
{
	return format >= 14 && format <= 16;
}

/* the levels down to 1x1 (compressed formats stop at 4x4), none for a bitmap
   whose sides are not powers of two, at most maximum_levels */
// @retail 0x359e0
short bitmap_get_mipmap_count(short width, short height, short depth, short format, bool linear, short maximum_levels)
{
	short result = 0;

	if (IS_POWER_OF_TWO(width) && IS_POWER_OF_TWO(height) && IS_POWER_OF_TWO(depth))
	{
		if (linear)
		{
			result = 0;
		}
		else if (function_x9955e3(format))
		{
			result = (short)log2_floor(MAXIMUM(width / 4, MAXIMUM(height / 4, depth)));
		}
		else
		{
			result = (short)log2_floor(MAXIMUM(width, MAXIMUM(height, depth)));
		}
	}
	return maximum_levels > result ? result : maximum_levels;
}
