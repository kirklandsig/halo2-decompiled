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
		result = 0;
		if (value != 1)
		{
			do
			{
				value >>= 1;
				++result;
			} while (value != 1);
		}
	}
	return result;
}

// MiniMax B (11-06): no new form, no lower score; the argument registers are retail's (the width in EAX,
// the two coordinates in EDX and ECX, the height and the output on the stack), so only the body's use of
// them differs: this build widens the third argument into EDI and the second into ESI at the entry where
// retail copies the third into SI as a word and widens the second only where it is used.
// MiniMax S (11-06): no new form, no lower score: the two coordinates still arrive in the other registers (retail x in EDX, ours in CX), so the widened copies, the two table lookups and the common count keep different homes; retail's minimum branches once and stores the common count after the join.
// score 57 (was 82): both logarithms now have retail's loop and the common count is
// taken by retail's branch, so the height count sits in ebx as in retail; left: the row
// coordinate (retail keeps it 16-bit in si and widens it late), the column (retail
// widens dx only after the mask) and the two bit registers (retail x in edi, ours in edx).
// MiniMax B (11-04b): no new form, no lower score; a 16-bit row copy with the widening deferred scores 66/84 but pushes the row argument onto the stack, so the matched caller 0x136140 no longer matches.
// MiniMax B (11-04): the 57 form is the local optimum; the y parameter used directly, a
// short y or x copy, both copies short, the minimum compared the other way round, the
// minimum seeded from the height count, a long or unsigned common count and the y lookup
// first all score 58/59/120/136/150 with 0x136140 still matched.
// Earlier at 82 (was 82): the coordinates now take retail's registers (the second in
// Earlier score 82 (was 82): the coordinates now take retail's registers (the second in
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
// Round 437: First-axis word/byte/signed-byte counts with short/word common count
// 90/99, 99/104, 85/97; long first-axis with word/long common count 124/127.
// Original counts and complete body restored; coordinate promotion and count homes remain.
// Round 483: six complete width/height/shared count naming orders 82 each;
// first two naming orders with the existing shared logarithm 87/87. Native
// count-home allocation does not change; original names and body restored.
// Round 485: Complement low-bit mask with both tail insertions OR/add/XOR,
// private/shared logarithms 83/83/83/89/89/89; sign-extended mask private/shared
// 84/90. All three controls matched; original mask, helpers and tails restored.
// Round 230: Mutable-formal scan with zero exit before/after result construction and
// nonzero-first/zero-first test-owned result 105/91/82/82; private scan copy on
// those four result graphs 105/91/82/82. Original helper/body restored.
// Round 49 (lane 10, fast, 0x136140 kept unless noted): the outputs as a two-entry bits array 82 (same
// code); a named `common_bits * 2` shift local 100; one inline spread helper for both coordinates 313
// (kept out of line, 0x136140 1); `mask & x_value` lookups 82; `common_bits << 1` 82.
// Round 40 (lane 13, fast): retail's 0x136140 call sets ECX, then EDX, then EAX (the declared order). x and y
// used directly with log2_floor 119 (y colours to EBP and becomes a stack argument; 0x136140 28); that with
// `common_bits = width_bits; if (height_bits < width_bits) common_bits = height_bits;` 119 (still cmov); that
// with y looked up before x in both arms 125 (302 bytes, 0x136140 kept); `long const` copies 82 (same
// code); y_bits declared before x_bits 82 (same code).
// Round 277: standard minimum with short/word/byte/long shared count 109/93/93/101;
// native short less predicate with the same carriers 117/119/119/119. Exact caller
// and both neighbors survive; original minimum, counts, helper and body restored.
// Round 193 (lane 3, fast with 0x136140): x and y used directly with the existing log2_floor (retail's
// loops) 119 again: our allocator then keeps the width count in EDI (retail ECX) and moves x into EBX, and
// LTCG passes width/x in EAX/ECX and y on the stack (0x136140 28). On that form: the minimum as if/else 119;
// the height logarithm first 118 (0x136140 8, y still on the stack), with the if/else minimum 118; the high
// arm's ORs low part first 119; `!=` tail tests 119.
// Round 483: six complete width/height/shared count naming orders 82 each;
// first two naming orders with the existing shared logarithm 87/87. Native
// count-home allocation does not change; original names and body restored.
// Round 485: Complement low-bit mask with both tail insertions OR/add/XOR,
// private/shared logarithms 83/83/83/89/89/89; sign-extended mask private/shared
// 84/90. All three controls matched; original mask, helpers and tails restored.
// Round 231: Actual excess relation reused at minimum and suffix, width/height bool/long
// 101/107/101/97; with shared logarithms 95/88/101/90. All callers and neighbours survive;
// original helper calls, relations and body restored.
// Round 495: Nonzero scans with negative-seeded index or complete bit length,
// zero-before/after result selection and long/short counts: 101/94/114/95/100/98/112/93.
// Original helper and body restored; coordinate promotion and count homes remain.
// Round 499: Reused width/height dominance with bool/long decisions and private/shared
// logarithms: 101/95/101/101/107/88/97/90. Original decisions, coordinates, helpers and body
// restored; promotions and count homes remain.
// Round 501: high-block logarithm peeling 16/8/4/16+8/16+4/8+4/16+8+4/16+8+4+2
// 129/129/133/172/161/161/199/137; last loses the matched caller. Original helper/body
// restored; delayed coordinate promotion and height/shared-count homes remain.
// Round 223: Isolated non-SSE unit 94 (diagnostic); immutable/next-bit windows,
// descending index/mask, ascending thresholds and three-/two-bit chunks give
// 88/103/97/105/97/96/132/99. The next-bit/index/mask/first-threshold forms lose
// the exact caller. Original file, table, private scan and full body restored.
// Round 231: Actual excess relation reused at minimum and suffix, width/height bool/long
// 101/107/101/97; with shared logarithms 95/88/101/90. All callers and neighbours survive;
// original helper calls, relations and body restored.
// Round 43 (lane 15, fast with 0x136140): parameters as (width, height, x, y, out), which retail's call setup allows
// (it fixes EAX, EDX, ECX and height pushed before out, not height against the coordinates), caller updated: 82 (same
// code); with log2_floor 87; with long copies after the mask 100 (0x136140 125); with only the y copy at entry 93
// (0x136140 7). The outputs written inside the tail's arms (out[1] then out[0] under the width test, out[0] then
// out[1] in the else) 77, not kept (a gain of 5): x then stays in EDX as in retail and the first exit gets retail's pops,
// leaving y EDI / x bits ESI where retail has ESI / EDI. On that tail: out[0] as one `x_bits | ...` expression 77, the
// part written first 77, a named high part 77, out[0] stored first in the width arm 78, out[0] stored before the height
// test 83, one out[1] store ahead of the tests 101, the else arm with an early return 77; with swapped copies 77, with
// log2_floor 87, swapped with log2_floor 84 (0x136140 29); long copies after the mask 99, y at entry and x after 93/86 and
// x at entry and y after 93/83 (log2_floor/private, all lose 0x136140); y looked up first in both arms 81, low arm 98, high arm
// 98; y_bits declared first 77; the minimum as `if` 77, with y first 81. x and y used directly 119, with the arms tail 124
// (log2_floor, x copied in 32 bits to EBX, y stacked), private logarithm 128. Retail's 16-bit `mov si, cx` stays out of reach;
// the branch minimum still needs an object without /arch:SSE (a build-flag lead, not changed here).
// Round 503: present high-bit scan predicate, original/no zero guard, scan/count update
// order, shifted predicate 82/82/98/98; masked predicate 89/89/96/96. All controls
// remain matched; original helper/body restored. Early promotion and count homes remain.
// Round 505: Shared low-band producer with optional high-band insertion, short/long low mask,
// OR/add joins and symmetric y-shift production: 104/104/104/104/103/105/103/105.
// All remain 270 bytes; original branch producers and body restored.
// Round 516: Signed axis-count difference in short/long storage, both orientations, reused by
// suffixes and optionally minimum. Early coordinate promotion and height/common homes remain.
// Scores 119/120/125/125/119/125/125/125; original executable body restored.
// Round 518: independent unsigned quotient scan with long/dword/short/word
// counts 98/98/98/98, with zero-first exit 105/105/103/103. Original helper
// and body restored; delayed coordinate promotions and height/common homes remain.
// Round 521: symmetric native table-expression indices/low/high nodes, seven subsets
// and unshifted high node 82/82/82/82/82/82/82/82. All original executable code
// restored; coordinate promotion and height/common homes remain.
// Round 524: top-down unsigned mask scan with long/dword/short/word counts
// 111/111/112/118; top-down bit-index scan with the same domains
// 105/105/118/139. Original helper and body restored; delayed coordinate
// promotion, height/common homes and branched minimum remain.
// Round 526: Repeated whole-nibble/byte logarithm scans with threshold/window tests and
// long/short counts 134/134/135/135/136/136/135/135. Original helper/body restored;
// delayed coordinate promotion, height/common homes and minimum branch remain.
// M321: Consumed x/y/both coordinate suffixes, private/shared logarithms, then both native short formals
// 82/82/82/87/87/87/130/119. Original body restored; early promotions, height/common homes and minimum branch remain.
// Y565: Branch-owned common count and matching low mask, count-first/mask-first, native/
// sign-preserving long mask, private/shared logarithms: 99/89/110/109/73/109/93/93.
// NOT kept: 73 combines individually regressing forms and grows to 271/270 bytes;
// all other forms regress. Original body restored; coordinate promotion/count homes remain.
// B398: Eight formal interleavings preserving width/x/y register order and
// height/out stack order, h/o/w/x/y, h/w/o/x/y, h/w/x/o/y, h/w/x/y/o,
// w/h/o/x/y, w/h/x/o/y, w/x/h/o/y, w/x/h/y/o: 82 each. Sole declaration
// and call changed consistently; original contract/caller/body restored.
// Frame four agrees; delayed coordinate promotion and height/common homes remain.
// Y567: Existing logarithm short/word/long/dword output pointers, then
// output references: 82/99/105/105/82/99/105/105. NOT kept: no byte match; original helper,
// counts and body restored. Narrow-coordinate/count homes and minimum remain.
// Y570: Coordinate suffix bands before one shift, complement x/y/both 99/99/103,
// XOR x/y/both 99/99/103, shared high mask 97, shared low bands 94.
// NOT kept: all regress and lose the matched caller; original full body restored.
// Narrow-coordinate/count homes and the minimum branch remain.
// Y577: Direct axis-count suffix comparisons, width/height/both with private/shared
// logarithms, then both with branch-owned stores: 95/96/116/77/93/104/111/99. NOT kept:
// five-point best below gate. Original body restored; narrow-coordinate/count homes and minimum branch remain.
// P347: Coupled independent axis scans, terminate at one or zero then convert bit length, short/word/long/dword counts:
// 113/125/132/132/102/95/106/106. NOT kept: All regress; terminate-at-one forms lose both exact caller controls. Native independent scans and complete body restored.
// T61: Actual two-word output array references, original formal order and seven other
// interleavings preserving the native register/stack banks: eight at 57; both callers exact.
// NOT kept: output identity does not delay coordinate promotion; original contract, caller
// declaration/call, helper and full body restored.
// Y605: On the retained branched-minimum graph, width/other then three-way
// output completions with long/long, short/long, long/short, short/short x/y: 58/109/128/134/85/143/161/177.
// NOT kept: all regress; narrow forms lose the caller. Original body and contract restored.
// Y607: Destructive width/height/both dimension-to-bit-count parameters, private/shared
// logarithms, then complete early-common/height-first lifetimes: 57/57/57/87/87/87/91/79.
// NOT kept: no exact native coordinate/count lifetime recovery; original body and interface restored.
// DH378: Whole-register x/y/both formals with original signed-word captures 96/97/97;
// native short x/y/both working values 120/136/150; both whole formals with mixed short/long captures 134/138.
// NOT kept: all regress and alter native entry/caller allocation; original body, sole declaration and contract restored.
// Round 77 (m3 lane 1, fast, 0x136140 kept): the minimum through a forceinline single-exit helper 57 (identical);
// `common = height; if (width <= height) common = width;` 57; `if (height < width)` 58; an if/else into the
// minimum 80; `if (common > height)` on the copy 57 (identical); the function, table and logarithm in a new `/O2
// /Gr` file without /arch:SSE (the exception) 57 (identical); x and y used directly 150 again. Retail's minimum
// shares ECX with the width count and is stored once after the join; ours takes EBP with a store at each definition.
// T77: On the retained branched minimum, x-only then y-only promotions after width log,
// both logs or common selection 62/72/72/56/71/66; both promotions after width/both logs
// 72/97. NOT kept: best gain one, longer extent and lost exact caller; all forms lose
// the direct caller, both late paired forms lose its caller too. Original body restored.
// M3 lane 11 r96 (fast with 0x136140, 0x359e0, 0x1d6f0; control 57, 0x136140 MATCH): retail copies y out of CX as a 16-bit
// value at entry (`mov si, cx`), leaves x in DX, and promotes both once just ahead of the mask branch; ours promotes both at
// entry. x and y used directly 150 (315 bytes; x arrives in ECX and is copied to EBX, y becomes a stack argument homed in
// EBP, the bit counts go to memory; 0x136140 28); `short` copies at entry 150 (same code); long copies after the mask 84
// (275 bytes, both become stack arguments; 0x136140 125); the body in an inline helper called from the function 352 (it
// stays out of line), in a forceinline helper with direct use / late copies / entry copies 150 / 84 / 57 (same code as
// without it); a `short point[2]` local filled at entry with late long copies 84, used directly 150, y stored first 84 (all
// folded away); the entry copies declared x then y 54 (0x136140 29: x and y swap ECX/EDX, so the copy order sets the
// convention); late copies for the lookups with the parameters in the tails 112 (285 bytes); entry copies with the
// parameters in the tails 146 (313 bytes). The register arguments follow the last/second-to-last/first rule over the
// parameters the body keeps in registers at entry (width EAX, x EDX, y ECX), so retail keeps x and y enregistered as
// shorts across both logarithms; no form here does that without moving the bit counts to memory.
// P465: Five incoming scalar identities and native mask/column-output/row-output
// identities independently named by role: eight 57, both callers exact. NOT kept:
// no delayed SI/DX promotion or common-count recovery; original complete body restored.
// P482: Unsigned coordinate bit transport x/y word-long/long-word/word-word/dword-long/
// long-dword/dword-dword/word-dword/dword-word, signed interpretation at every use:
// 120/136/150/57/57/57/120/136. NOT kept: word forms enlarge and lose the exact caller;
// whole-width forms tie. Original captures, signed arithmetic, helper, contract and body restored.
// @retail 0x358d0
void function_358d0(short width, short x, short y, short height, dword *out)
{
	long y_value = y;
	long x_value = x;
	short width_bits = (short)swizzle_log2(width);
	short height_bits = (short)swizzle_log2(height);
	short common_bits = width_bits;
	if (width_bits > height_bits)
		common_bits = height_bits;
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
