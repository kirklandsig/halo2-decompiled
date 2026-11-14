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
	real alpha = arg_1[3];
	arg_2 = function_12efb1(arg_2);
	if (alpha == 0.0f)
		alpha = 1.0f;
	arg_0->field_0.red += arg_1[0] * arg_2;
	arg_0->field_0.green += arg_1[1] * arg_2;
	arg_0->field_0.blue += arg_1[2] * arg_2;
	arg_0->field_c += alpha * arg_2;
	arg_0->field_10 += arg_2;
	arg_0->field_14 *= 1.0f - arg_2;
}

// MiniMax H (11-12): no new form, no lower score (607); the palette slot is still sign-extended and compared 16-bit where retail compares the byte with -1.
// MiniMax I (11-11): no new form, no lower score (607): the 0x130f60 call's result still stays in EAX where retail moves it to EDX and keeps NONE in EAX, so the palette slot is still read as a word and the body is one instruction short from there on; that register home is set by the clamp in the three-scalar helper here, whose other callers keep their own match.
// score 607 (was 607): earlier 616. Frame 0x4820, clears and metadata cursor agree; palette and accumulator carriers remain.
// MiniMax E (11-11): no new form, no lower score (607); the palette and accumulator carriers still differ from
// retail in their scheduling only - every statement, field, condition and callee agrees, so the gap is register
// and stack home placement that no legal spelling of this source reaches.
// Behaviour walk (w14J, 11-06): every block compared with retail 0x12efb0-0x12fe96 (accumulator and output offsets, the
// four weight tests, tag/palette dispatch, fallback double weight, f0 store, tag-arm best-weight rule, final publication
// clamps): no logic, field, condition or callee difference; the 607 is scheduling and register homes only.
// MiniMax E (11-05): no new form, no lower score; the palette and accumulator carriers are as the earlier
// rounds left them, and retail's `mulss scalar, t` product form has no legal spelling here.
// Earlier (616, was 616): Frame 0x4820, clears and metadata cursor agree; palette and accumulator carriers remain.
// Earlier at 616: Frame 0x4820, clears and metadata cursor agree; palette and accumulator carriers remain.
// Earlier 616 (was 616): Frame 0x4820, clears and metadata cursor agree; palette and accumulator carriers remain.
// Earlier at 616 (was 616): Frame 0x4820, clears and metadata cursor agree; palette and accumulator carriers remain.
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
// Round 111 (lane 7; an LTCG env link of this file, the caller's file and its callee closure reproduces the full
// listing, checked against a plain build at 683): retail keeps the record base in ESI through the arm and
// re-reads the slot byte at the source_index store (`movsx edx, byte [esi+0x6f]`), compares the slot as a byte
// (`cmp cl, al`, NONE in EAX, the 0x130f60 result moved to EDX), and keeps the palette record in EBX. The slot byte
// read from the record at every use 683 (3832 bytes, plain); a char slot with the store re-reading the byte 683;
// a named cluster-index local 616 (same code), with the byte reads 683; accumulator statement orders in the
// three-scalar helper: m,w,colours,i,d,h 616; colours,w,m,i,d,h 616; colours,i,d,h,m,w 703; colours,i,d,h,w,m
// 691; w,colours,i,d,h,m 691; w,m,i,d,h,colours 687; i,d,h,colours,w,m 691; reversed 691; colours,w,i,d,h,m 691;
// the weight parameter bound first or second (diagnostic) 616 (same code); the clamped weight in a new local
// 617; weight-first products 616; the three scalars named in locals first 617.
// Round 113 (m3 lane 7, env link = check): char slot with `long slot_index = palette_slot;` at the top of the branch,
// the NULL record assigned in the else arm before the table test and the source index re-read from the byte 683 (3832
// bytes; widening in retail's block, but index and record share EDI); that with the source index from `slot_index`
// 756 (3849 bytes); with the slot read before the flag and source stores 683; the normalization helper reading the
// weight, or the intensity, into a named local first 616/616 (same code). Diagnostic: the fog accumulators escaping
// to an out-of-line call still leave the normalization stores interleaved (retail keeps load-multiply-store order).
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
// Round 286: Existing unit-interval producer lower/upper early returns 661/660;
// lower/upper separate result 615/621; lower/upper conditional result 661/660;
// lower/upper default guarded result 646/711. Original helper and body restored.
// Round 290: actual index-output default owned by its provider, short/char palette with immutable
// tag-result/readonly-record combinations, 703/703/703/703/699/699/699/699; frame 0x4820 agrees.
// Original caller default, field observations, helpers and body restored; palette/record and SIMD carriers remain.
// Round 444: Double scalar-contribution/merge-fraction/color-contribution result domains,
// subsets 1/2/4/3/5/6/7 then all with complete publication reciprocals: 838/839/772/823/867/828/833/927. Original helper
// precision, alias-safe source-weight capture and full body restored; palette/record and SIMD carriers remain.
// Round 446: native palette/radius/alpha field lvalues, seven subsets
// 683/616/683/616/683/616/683; all with previous-index field 681. Original observations, helpers and body restored.
// Round 292: Overlap-safe scalar-RGB/merge-record/four-channel input copies, subsets 1/2/4/3/5/6/7,
// 1100/888/961/1214/1194/1077/1328; all with normalized-publication input copy 1552. All enlarge
// frame/code; original helper bodies and source-weight capture restored. Palette/record/SIMD homes remain.
// Round 76 (lane 12; plain 695 on the last form, fast otherwise; "al" is the aligned count against this body's 353,
// "st" the same with register names masked against 109): retail's shape comes from a whole set of helper-level
// forms. Weight and remaining updates last in the three-scalar helper (as matched 0x1317a0) with the nested clamp
// call al 361; with the clamp written in the helper (`if (w < 0) w = 0; else if (w > 1) w = 1;`) 696, al 332 and that
// helper then has retail's exact schedule apart from register names (the same clamp as a named local or through the
// clamp inline 723, al 361); that plus char slot/getter shaped like matched 0x1318a0/re-read byte at the source-index
// store and a named tag local surviving 0x1317a0 (EDI in retail, both arms) 701, al 424; the merge helper's weight
// and remaining last, the four-channel helper taking r, g, b, a by value 688, al 181, st 57; the three output blocks
// written directly in the body with their own inverse (every load/copy product side then right) 672, al 124, st 32;
// the tag test nested with the 0x88 pointer inside it 701 (3817 bytes; -1 back in a register), st 27; the second-arm clamp as
// `x < 0 ? 0 : (x > 1 ? 1 : x)` on a named x 705, st 21; `long slot_index = palette_slot;` with one tag local at
// loop scope: 695, al 98, st 21, the 3819-byte form: record in EBX, cluster index in EDI, frame and loop head as retail.
// Left on that form: retail moves the 0x130f60 result to EDX and keeps -1 in EAX (ours -1 in EDX, result in EAX, one
// instruction short, so every later line is one off), the 0x88 pointer read, the second/third colour product sides
// (retail L, C, L; ours C, L, C), the second four-channel call's schedule, the 0x6c store. Tried for -1 in EAX, all
// unchanged: result declared before the out value, at function scope, split from its call; tag at function scope;
// slot declared earlier; `NONE !=` tests; the tag lookup through a helper; a named no-source value (696, al 82);
// diagnostics adding -1 uses or dropping a result use. The 0x88 pointer before the tag test loses -1 again (674).
// None kept: the positional count stays above 616 until the -1 tie clears. Retry from these forms.
// N295: Independent palette-input/metadata-default/weighted-state lifetimes before the provider,
// subsets 1-7 625/718/755/618/625/625/625; all with signed-byte palette 749. Original order,
// body and helpers restored; palette/record and SIMD accumulator homes remain.
// Round 510: complete pre-store scalar/merge/color product families 616/616/616/599/599/599/600; all
// with publication products 600. Cross-store one-use products rejected without a match. Color phases
// ICWR/IWCR/IWRC/WICR/WRIC/CWIR/CWRI/CRWI (C RGB, I intensity, W weight, R remainder)
// 775/775/775/748/748/616/614/612; two/four-point leads rejected. Original helpers and complete body
// restored; palette/record and SIMD carriers remain.
// Round 44 (lane 15, fast, with a CAVEAT line): the palette slot as a char local, retail's `cmp cl, al`, 705 (3835
// bytes, masked 131 against 130), read right after the palette call 705; long slot with the test on the byte field 761
// (3856); the record pointer cleared only in the palette arm, retail's `xor ebx, ebx` position, 616 (same code), with
// the char slot 705. The -1 in EAX tie stays.
// N306: Native output combined/separate rejection, nested/intensity-first acceptance, each with
// original/early-exit merge: 616/616/616/616/616/616/616/616. Original helpers and complete body restored;
// palette/record and SIMD zero/contribution carriers remain.
// P271: Conditional intensity computed after the RGB stores, load/default, guarded
// default, if/else and named conditional, with short/char palette:
// 609/712/720/720/698/702/708/708. Original helper and body restored; palette/record/SIMD homes remain.
// P271 follow-up: Late two-use intensity with CWRI/CRWI/CWIR, conditional-result
// CIWR/CWRI/CRWI/CWIR, and late two-use CRIW: 703/703/609/609/703/703/609/703.
// Original helpers/body restored; isolated seven-point lead remains below the keep gate.
// Round 541: Standard weighted-array multiplication ranges a/b/c, seven subsets and returned
// output-iterator chain: 795/789/773/804/780/754/766/774. Frame becomes 0x4824; every
// form regresses. Original ten stores, includes, helpers and complete body restored.
// Round 54 (m3 lane 14, fast listed with 0x131030): the three-scalar helper clamping into a named weight
// instead of reassigning its parameter 617; that helper in colour/intensity/weight/remaining order 723; its
// scalar products as weight * value 616 (same code: ours copies the weight for the first two scalar products
// where retail multiplies the loaded scalar in place, the 22-byte length gap); the clamp helper as plain
// static inline 616 (same code); the merge helper's weight/remaining after its colour block 715.
// Round 64 (m3 lane 1, fast): char palette slot 705 (3835 bytes: byte compare with -1 as retail, record still
// in ESI); long slot 713; char slot with the record chosen by one nested ternary 705; that with the source index
// re-read from the byte 683 (3832; ESI keeps the cluster record, the selected record moves to EDI, not EBX);
// that with a long slot index at the top of the branch 683.
// Lane 18 round 34 (fast): the flag declared before the index output and palette call 618; the best-weight locals
// declared after the clears 779 (3827 bytes, +0xa); the previous-index test inverted with the scenario lookup nested
// 616; the flag declared after the source stores 616; the short slot read right after the palette call 616.
// Round 82 (m3 lane 16, fast listed with 0x131030): the three-scalar helper with weight/remaining after the height
// 723 (3845 bytes), after the blue 616 (same code); the palette record's else arm as one ternary with a NULL default
// 616 (same code); the 131030 arm's fog range clamped straight from the definition field like the palette arm
// (`def[0x44] > 0 ? (def[0x44] < 10 ? def[0x44] : 10) : g_547f80`) 619 (3842 bytes: that clamp block then has retail's
// shape, re-reading the field at each test), with the char palette slot 708 (3838).
// Round 205 (m3 lane 3, plain; fast carries a 0x131030 CAVEAT): the normalization helper's output as `real (&)[6]` 616
// (same code); the three six-field publications written in the body like the four-field one 710 (3850 bytes): those
// three blocks then are retail's exactly (load, multiply, store in field order, intensity through the copied
// inverse), so retail's publication is not our interleaving helper, but the loop grows; that with a char slot 723;
// the NULL-record `goto` replaced by a second copy of the default arm 853 (3956, not tail-merged); the body
// publications with the palette byte read at every use 698 (3842). Loop left: zero in XMM5 (retail XMM7 for the
// whole arm), slot byte in CL with the 0x130f60 result in EDX, the accumulator store interleaving.
// Round 68 (m3 lane 1, plain; the one-link harness is not exact here, aligned 10 against the build: g_547f80 hoisted): lane 12's 3819-byte
// form rebuilt 695 (its masked listing is close; what it lacks is retail's `mov edx, eax; or eax, -1` after the 0x130f60 call, so every later line is
// one off). -1 stays in EDX with NONE in a named local at all five uses 696, the result typed dword 695; on the harness the result as const
// or unsigned, declared apart from its call, after the flag, after the slot read: all the same code. The call after the source stores moves
// -1 to EAX only ahead of the call. Probes (a dword load and global store) after the call, after the flag, either side of the slot read:
// -1 stays in EDX; two after the call: 785 instructions but -1 still in EDX. Not a rotation of one step at those places.
// Round 82 (m3 lane 16, fast listed with 0x131030): the three-scalar helper with weight/remaining after the height
// 723 (3845 bytes), after the blue 616 (same code); the palette record's else arm as one ternary with a NULL default
// 616 (same code); the 131030 arm's fog range clamped straight from the definition field like the palette arm
// (`def[0x44] > 0 ? (def[0x44] < 10 ? def[0x44] : 10) : g_547f80`) 619 (3842 bytes: that clamp block then has retail's
// shape, re-reading the field at each test), with the char palette slot 708 (3838).
// N355: Caller clamped-result residence, with best-weight/byte-palette/publication-reciprocal domains:
// 968/877/1089/964/975/1088/1081/1044. NOT kept: no exact match from the tested structural forms.
// Complete original executable source restored; Palette/record and zero/contribution SIMD carriers remain.
// N357: Complete paired source-identity/best-contribution winner selections, bool/byte/short/long positive/complementary decisions:
// 616/616/616/616/720/720/720/720. NOT kept: no byte match from these source forms.
// Original executable source restored; Palette/record and zero/contribution SIMD carriers remain.
// N359: Complete kind/index/weight metadata results with common publication, subsets 1/2/4/3/5/6/7 then entry declarations:
// 720/660/613/647/711/700/800/800. NOT kept: no byte match; complete original body restored.
// Weight-only result 613: three-point gain rejected. Palette/selected-record and zero/contribution SIMD carriers remain.
// N363: Complete native alternate-source tag fields, weighted-record/byte-palette/readonly
// source masks 0..7: 616/616/705/705/616/616/705/705. Original raw source path and full body restored;
// NOT kept: unmatched representation forms. Palette/record and SIMD accumulator homes remain.
// N365: Existing color helper with separate twice-used scalar opacity, crossed with
// shared RGB/clamped-weight/native per-use palette, 699/699/699/699/696/696/696/696.
// Original helper and body restored; NOT kept: unmatched input/contract ownership.
// Round 149 (m3 lane 6, fast; control 616, aligned 497, masked 249; masked ignores registers and stack offsets): accumulator statement orders (r g b, i d h, w weight, m remaining)
// rgbidhwm 723 (masked 225); rgbidhmw 703 (3836 bytes, 214); rhgwbidm 723 (226); rgbwidhm 723 (213); rgwbidhm 723 (211); rhgwidmb 721 (247); hrgwbidm 723 (222). Products written `t * x` no
// change on any order; helper parameters with the weight before the scalars no change on four orders; a fresh clamped local 704/724/722, kept order 617. `char` slot 705 (3835, 250), with orders
// 718/676 (676 at 3832 bytes, 215); slot read at the test then `long` from a second read 756 (246), with orders 752/769 (211/210). Retail holds 0.0 in xmm7 across the palette arm's
// accumulators, so its products run in place (`mulss xmm1, xmm0`); ours overwrites the zero and rematerialises it, leaving a spare temporary and `movaps t, x` products.
// M383: Existing scalar destination/color/scalars/weight, merge and publication formal
// lexical homes, then scalar-merge/all helpers: eight 616s. NOT kept: no palette/SIMD
// allocation repair; original helper symbols, input-weight capture and complete body restored.
// Round 211 (m3 lane 3, fast; aligned with registers and stack/both masked, control 353/130): retail's six-field
// accumulation runs in parallel registers (red, height, green, weight, blue, intensity, distance, remaining, with arguments
// loaded ahead and multiplied in place); ours finishes one field before the next in every form. The three six-field
// accumulations written on the accumulators directly 697 (retail's 3817 bytes, first difference +0x6d, 506/147), with the
// char slot 725 (3797, 508/140), with the body publications 697 (506/148), all three 726 (3796, 508/141); also the
// other three accumulations direct, so no helper takes an accumulator's address, 711 (3804, 521/157), still one field at
// a time. On the original helpers, char slot and body publications together 723 (3846, 403/102), the best register-blind
// helper form; that with all six values computed before any store 738 (3852, 412/90). Original body restored.
// Round 64 (m3 lane 14, fast listed with 0x131030; masked aligned 247 at 785 instructions on both sides): the key to the accumulator
// blocks is retail's `mulss scalar, t` in place ([scalar, t]; ours copies t), which is what lets its schedule interleave the
// preloaded scalars. Weight after the height, field order 723 (3845, aligned 223); retail's scheduled order red, height, green, weight, intensity,
// distance, remaining, blue 721 (245); weight parameter first, after the accumulator, after the colour 616 each (same code);
// the scalars `real const` 616 (same), `real const &` 699 (3828, 241), with field order 723 (223) and 694 (251); a named clamped
// weight in field order 724 (223), in the old order 617 (247). Body unchanged.
// B452: Complete scalar inputs in reused scalar/merge/color helpers, seven subsets then
// all with scalar publication: fast 751/658/770/717/809/738/780/769. NOT kept: no gain;
// palette/SIMD homes remain, some frames grow; all original helper contracts/body restored.
// Round 137 (m3 lane 8, fast; aligned = difflib count, base 497): retail keeps NONE in EAX from the slot test to the weighted
// tag test deep in the palette arm (`cmp edi, eax` there, ours `cmp edi, -1`) and on into the fallback store, so -1 outranks the
// 0x130f60 result for EAX. The 0xd4 tag in a named two-use local 723 (3850 bytes, its read hoists above the 0xa4 test); the
// result declared first in the loop body and assigned at the call 616 (same code); both weight tests through the 0x88
// pointer 687 (3830, aligned 792); the clamp helper as a result with two independent tests 778 (3865); the short slot with
// the source-index store re-reading the byte 689 (3837).
// Round 218 (m3 lane 3; fast, the kept form plain): the four-channel helper's alpha default as a named if-form,
// read before the weight clamp (retail loads the channel ahead of the clamp), 607 plain, kept; the same if-form after
// the clamp 609; as a named ternary 720; alpha and weight ahead of the colour terms 609, on the kept form 609; the
// channels as by-value parameters 757 (3855 bytes, frame grows); the clamp's upper test first 622; the missing-record
// exit as an early goto 616; on the kept form: the merge helper's weight read ahead of the clamp 607 (same code), the
// tag arm's range clamp read from the field at each use 610 (aligned 236 -> 230), the three-scalar helper in retail's
// scheduled order (red, height, green, weight, blue, intensity, distance, remaining) 718 (two more instructions,
// aligned 210), its red term ahead of weight and remaining 607 (aligned 224).
// M3 lane 8 r150 (fast; "al" aligned count 491 and register-masked 220 on this body): the palette byte read from the
// cluster record at every use, `*(signed char *)(record + 0x6f)` as matched 0x12eb00 does (test, bounds, index, source
// index store), with the else-if lookup through an inline getter shaped like matched 0x1318a0 (NULL result, then the
// global, the bounds and the entry) gives 600 at retail's 3817 bytes (al 777, masked 228): the lookup's control flow is
// then retail's (every failure through the `test record` join, the byte compared as a byte, the source index store
// re-reading the byte), but it is a new one-caller getter and 7 under the keep bar, so it is a lead, not kept. Left on
// it: the 0x130f60 result stays in EAX with NONE in EDX (retail result EDX, NONE EAX), record EDI and tag EBX (retail
// EBX/EDI, cluster index in EDI until the tag), zero in XMM5 (retail XMM7), so the accumulator products copy the weight.
// On it: a named cluster index 600 (masked 228 -> 220; re-measure of rounds 78/111's form); the getter's index `signed char` 707; the byte as a typed
// record field 600 (same code); the table as a typed view 600 (same code); the clamp written in the three-scalar
// helper as 0x1317a0 writes its own 684 (3799 bytes, masked 186; re-measure of lane 12 r76's form); the getter forceinline
// 600 (same code); the per-use byte with a copy of 0x12eb00's entry_at_index (NULL default inside, the global tested by
// the caller) 603 (3822 bytes). Alone: the per-use byte 666 (3827); the getter alone
// 604 (3822, re-measure of round 107's form); the char slot 700 (re-measure); the NULL default in the else arm with
// the per-use byte 666 (same code as without it), with the global in a local 666.
// Round 181 (m3 lane 4, fast): retail tests the +0x6f byte as a char (`cmp cl, al` against EAX's NONE), widens it once
// (`movsx ecx, cl`) before the previous-index test, and re-reads the byte for the source-index store. The slot as char 700
// (3828 bytes: the byte compare, but widened at each use); as long 706; char with a long index for the lookup and the
// store 751 (the index takes a stack home); the byte read at each use (no slot local) 666 (3827: retail's `mov cl`, the
// record in EBX as retail); only the store re-reading the byte 703; the byte at each use with a long index for the range
// test and the lookup 666 (retail's early movsx); on that, NULL assigned in the lookup arm instead of at the declaration
// 666 (same code: still threaded). Left in that block: NONE in EDX and the lookup result in EAX (retail EAX/EDX), the zero
// in XMM5 (retail XMM7) and retail's unthreaded NULL test; the accumulator schedule after it is unchanged.
// M3 lane 9 r105 (fast; "masked" = aligned count with registers and stack offsets ignored, control 607 masked 220): a
// grid of the palette slot (short / char) x the three-scalar helper's statement order (w m r g b i d h as now /
// r g b i d h w m / r g w b i d h m / r h g w i d m b) x the merge helper's weight and remaining updates after
// the colour block (no / yes): 16 forms 607-732; the merge helper's late weight lowers the masked count in every
// pairing (220 -> 191 alone, 705), best r g w b i d h m with it 732 (3847 bytes, masked 153); on that the
// four-channel helper's weight terms first 714 (masked 173); r g b w i d h m with the late merge weight 732
// (masked 155); r g w b i d m h 698 (masked 260); the merge helper reading the source weight at each use 732 (same
// code). Retail keeps 0.0 in xmm7 for the whole loop body (43 uses, no other value), which none of these give.
// M3 lane 10 r86 (fast with 0x131030, 0x1317a0, 0x130f60, all still MATCH; counts are score, bytes, aligned/masked aligned; control
// 607, 3832, 487/220). Body unchanged; the closest form is build/r86/lead-12efb0.patch (693, 3820 bytes, 128/28). Match-only lead (it writes the three output
// helpers and the clamp out by hand). Joint steps, none under 607:
// weight and remaining last in the three-scalar and merge helpers 732 (3847, 346/167), with a char slot 728 (3843, 355/169); with the clamp
// written in the three-scalar helper 698 (3821, 291/114), plus char slot and the source index re-read from the byte 670 (retail's 3817 bytes,
// 280/114), without the merge reorder 696 (3807), without the char slot 691 (3820, 269/111); then a named tag surviving 0x1317a0 670 (286/122),
// tag-arm range read from the field at each use with a nested ternary clamp 681 (293/117), with the library clamp 681 (295/120), the 0x6c
// store before the last weighted store 681 (same counts); a named cluster index 681 (294/118; not retail: it re-reads the index after
// 0x1317a0); a result-first record getter shaped like 0x1318a0 taking the char slot 703 (315/116), taking a `long slot_index` declared in the
// palette arm 629 (3817 bytes, 292/111; record in EBX as retail), with the source index from that long 635 (3826); on the 629 form without the
// named cluster index 629 (284/100); the four-channel helper taking r, g, b, a by value, alpha and weight first, clamp written in it 697
// (3808, 223/74), with the library clamp 703 (3825, 250/101), with the getter taking the char slot or the byte field 712 (3807, 257/78);
// the three six-field output blocks written in the body 693 (3820, 128/28); on that: the 0x88 pointer declared before the tag test 676
// (3825, 154/38), also with one joined test 676; one `long` slot 666 (3817, 144/39; -1 in EDI across 0x130f60); slot as field reads only, or a
// char slot passed to the getter, 705 (3819, 160/32; record in EDI); a one-use inline tag getter 693 (same code); slot read before the three
// source stores 693 (same code); `long slot_index` declared before those stores, or read from the byte ahead of the test, 669 (3829, 110/38;
// no -1 register at all); slot_index from a second byte read inside the arm 693 (same code); a `use_default` flag in place of the goto 694
// (141/39); the result declared ahead of the out index 693 (same code).
// Left on the 693 form (28 masked): retail copies the 0x130f60 result to EDX (`mov edx, eax` after the slot read, `mov eax, edx` before
// 0x131030) and takes EAX for -1, ours keeps the result in EAX and -1 in EDX; the -1 register only exists while a register is free, so the
// result's home is the cause; `movsx ecx, cl` then follows the previous-index compare. Also the second accumulator's red product side, the
// 0x88 pointer placement, the tag-arm weight store, the second four-channel call's weight position and the 0x6c store.
// MiniMax S (11-10): no new form, no lower score (607): retail moves the 0x130f60 result into EDX and keeps NONE in EAX,
// so the palette slot is compared as a byte (`cmp cl, al` at 0x12f15d) and the palette record stays in EBX; ours keeps
// NONE in ECX, reads the slot as a word and is one instruction short from there on, so every later line is off by one.
// The char-slot and re-read forms recorded above (683-756) and the statement orders in the three-scalar helper all score
// worse than this body.
// Round 73 (m3 lane 15, fast with 0x131030/0x1317a0/0x130f60; masked = register-masked aligned count, control 222): the merge
// helper's clamp written in it 607 (same code). Re-measured on this base: the clamp written in the three-scalar helper 663
// (3817 bytes, masked 184), with weight and remaining last 695 (masked 150), the clamp written in both helpers 663 (same code
// as the three-scalar one alone). Diagnostics (a body in another file cannot be kept): the three-scalar helper as an
// out-of-line function in another file, inlined by LTCG, with its clamp written in 663 (3817 bytes, masked 177: the preloaded
// scalars and in-place `mulss scalar, t` products appear, as they do with the clamp written in the front-end helper, so the
// clamp is the lever and LTCG inlining adds little); the normalization helper moved the same way 607 (same code).
// Round 73, continued (lead): the three-scalar accumulations through the existing fog_accumulate of src/unknown_130f60.cpp
// (ternary clamp, fog_color_add_scaled for the colour, then intensity, distance, height, weight, remaining; copied in for the
// test, the colour passed as `(color3f const *)(local_19 + 1)` etc.) 703 (3807 bytes) but masked 222 -> 140, aligned 491 ->
// 410: the three accumulation blocks then match retail apart from registers. With the palette byte read at each use 696 (3802
// bytes, masked 139), with a char slot 705 (masked 140), with a long slot 703 (masked 146). What keeps the positional count up
// is one instruction right after the 0x130f60 call: retail moves the result to EDX (`mov edx, eax`) and keeps NONE in EAX
// through the slot compare (`cmp cl, al`), the source-index store and the deep tag test, where ours leaves the result in EAX
// and NONE in EDX, so every later line is one off. To keep the helper it must move to a header shared with
// src/unknown_130f60.cpp (0x131030 must stay matched).
// Round 73, continued (re-measures on the fog_accumulate base with the byte read at each use): the 0x130f60 result declared at
// function scope, declared ahead of the out value, and the NONE tests written `NONE != x` 696 each (same code: the result
// stays in EAX and NONE in EDX).
// M3 lane 1 r89 (fast with 0x131030, 0x1317a0, 0x130f60, all still MATCH; control 607, 3832 B, aligned 348, masked 118): retail's
// colour accumulation (function_12efb5's body) loads alpha, blue, green, red (+0xec..+0xe0) before the weight clamp and
// stores the alpha term and the weight before the colour, like four by-value arguments evaluated right to left. The helper
// reading all four fields into locals first, retail's store order, 757 (3855 B, frame changed); the three-scalar helper with its
// weight and remaining stores last (that helper alone) 718 (3838 B); both together 736 (3853 B); a char palette slot with the
// colour helper's alpha term and weight first 702 (3828 B); a char slot with the three-scalar weight/remaining last 707 (3834 B).
// Re-measured listed forms: four scalar colour arguments 774 (3863 B), the alpha term and weight first alone 609 (3832 B).
// M3 lane 12 r110 (fast with 0x130f60, 0x131030, 0x1317a0, all MATCH; aligned/masked control 354/118): the fog
// globals read once into a byte pointer local for the palette lookup 607 (same code; re-measure on this body of lane 8
// r150's global-in-a-local form); the cluster loop over a cluster
// pointer with an end pointer 794 (3861 bytes); the no-tag fallback weight through the file's clamp 725 (3850); the
// red publication product as red * inverse * (1 - remaining) 607 (same code); the palette range as `g_547f80` then
// `if (range > 0)` with the inner ternary 702 (3827); the palette byte read before the 0x130f60 call 617 (3828). No retail function holds the fog-palette lookup (global, +0x340
// bound, +0x344 table, 0xf4 stride) out of line, so the 0x1318a0-shaped getter lead stays a new one-caller helper.
// M3 lane 13 r82 (fast with 0x131030, 0x1317a0, 0x130f60, all MATCH; control 607, aligned 446, masked 118): the cluster entry
// read by index at each use (no entry pointer) 609 (masked 136); re-measured on this body: the slot as `long` 706, the 0x88
// pointer declared before the tag test 714; the three 0x131030 outputs declared at the tag arm's top 607 and the NULL-record
// test as an early `goto` ahead of the record block 607 (same code each). No untried natural form is left: retail moves
// the 0x130f60 result to EDX with -1 in EAX and holds 0.0 in XMM7 across the loop, and no recorded form gives either.
// M3 lane 8 r161 (fast with 0x131030, 0x1317a0, 0x130f60, all MATCH; control 607, 3832 B, aligned 487, register-masked 232):
// the clamp written in place (0x1317a0's own if/else-if) in the three-scalar and four-channel helpers 692 (3817 B, masked 155);
// plus the three-scalar helper's weight/remaining after its products 698 (3806 B, masked 111); plus the merge helper in
// retail's order (clamp in place, colour block under the source-weight test read from the field, then weight and remaining)
// 685 (3817 B, aligned 283, masked 84); plus a char slot widened once into a long index at the top of the arm, the NULL
// record set in the lookup arm and the source index re-read from the byte 693 (3817 B, masked 94); the merge-helper change
// alone 705 (3841 B). Lead, not kept (one-caller getter): the last form with a 0x1318a0-shaped lookup getter 705 (3800 B,
// masked 67, the lowest masked count recorded); left there: NONE in EDX and the 0x130f60 result in EAX (retail EAX/EDX), the
// zero in XMM6 (retail XMM7). Body unchanged.
// P435: function-scope provider output, provider output/return role names, immutable
// provider return on the original word-palette graph, readonly cluster/tag sources, and
// immutable palette/weighted source receivers all 607. NOT kept: no gain; native
// return EDX, byte palette CL, selected record EBX and zero XMM7 remain unrecovered.
// T96: Actual palette/alternate/both upper-cap-first range branches 604/608/605;
// corresponding conditional selections 604/608/605; direct rejecting palette/alternate
// chains 606/607. NOT kept: the three-point 604 lead is below the gate; first palette
// carrier and zero/SIMD allocation remain. Original range expressions and body restored.
// P456: unified actual 0xb0-stride source/provider-owner type, mutable/readonly short palette 607/607;
// native char 700/700; uniform per-use byte 666/666; promoted long 706; actual readonly element
// reference 607. NOT kept: ties or regressions; original opaque input type, all observations, helpers
// and complete executable source restored. Native provider-result/NONE, palette/selected-record and
// zero/SIMD carriers remain.
// Existing real/double-factor scale helpers at weighted a/b triples, paired
// direct/real/double producers: 607/697/607/606/696/703/702/701. NOT kept:
// real/real gains only one at 3830 bytes; double-factor forms regress. Original
// ten scalar stores, original includes and complete body restored; provider/NONE,
// palette/selected-record and zero/accumulator carriers remain.
// P485: Complete palette/tag/default source-choice result in byte/char/signed-char/
// short/word/int/long/dword: eight 618s at 3832 bytes. NOT kept: no byte match;
// original dispatch, helpers, alias-safe input reads and full body restored.
// Native provider/NONE, palette/record and zero/accumulator SIMD homes remain.
// T106: Actual weighted contribution real/double and source-tag long/dword receipts,
// retained short palette 688/811/748/748; uniform native byte reads 760/787/744/744.
// NOT kept: no native provider/NONE or accumulator-home recovery; double receipts
// enlarge frame to 0x4828. Original completion, observations and full body restored.
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
