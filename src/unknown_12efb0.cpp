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

// score 607 (was 607): Retail compares the palette byte before the metadata stores and keeps the returned tag in EDX.
// MiniMax G (11-20): score 607 (was 607): no new form, no lower score; the palette slot's declared type is the
// only lever left at the loop head and none of the three spellings pays: a char local gives retail's byte load
// (mov bl/cl, no sign extension at the load) but NONE then lands in ECX with the result still in EAX and the
// whole tail after the loop breaks, 700; a byte local zero-extends into EBX, 653; the plain short keeps the
// sign-extended word, 607. Retail still keeps the 0x130f60 result in EDX, the slot byte in CL, NONE in EAX, the
// zero in XMM7 and the palette record in EBX; ours keeps the result in EAX, the slot in BX, NONE in ECX, the zero
// in XMM5 and the record in ESI, so the sign-extended slot is materialised into the record register itself.
// m3 lane 13 r149 unchanged, its lines just above the marker.
// Earlier (607, was 607): The 0x4820 frame, clears and metadata cursor agree. The first difference spans the
// loop exit. Beyond it, retail keeps the tag result in EDX, compares the palette byte in CL against NONE in
// EAX, and joins failed palette lookups at one record test. The build uses EAX, BX and ECX and threads past
// the join.
// Earlier score 607 (was 607): frame 0x4820, clears and metadata cursor agree; first +0x15f spans the loop; provider/NONE, palette/record and zero/SIMD homes differ.
// MiniMax S (11-16): no new form, no lower score (607): the palette slot is still sign-extended and read as a word where retail reads the byte unsigned at each use, and the 0x130f60 result still stays in EAX where retail moves it to EDX and keeps NONE in EAX; the native byte palette measures 611 and the signed short 616 in this build, and that register home is set by the clamp in the three-scalar helper in this file, whose other callers keep their own match.
// Earlier at 607: MiniMax I (11-16, round 2): no new form, no lower score: the first differing row is the stack
// Earlier MiniMax I (11-16, round 2): no new form, no lower score: the first differing row is the stack
// Earlier 607 (was 607), MiniMax I (11-16, round 2): no new form, no lower score: the first differing row is the stack
// probe's call target for the 0x4820 frame, whose address belongs to this build and cannot match; the palette slot's
// sign extension and the helper differences in the notes below are unchanged. Body unchanged.
// MiniMax H (11-12): no new form, no lower score (607); the palette slot is still sign-extended and compared 16-bit where retail compares the byte with -1.
// MiniMax I (11-11): no new form, no lower score (607): the 0x130f60 call's result still stays in EAX where retail moves it to EDX and keeps NONE in EAX, so the palette slot is still read as a word and the body is one instruction short from there on; that register home is set by the clamp in the three-scalar helper here, whose other callers keep their own match.
// Earlier header detail: 607 (was 607): earlier 616. Frame 0x4820, clears and metadata cursor agree; palette and accumulator carriers remain.
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
// M3 lane 15 r83 (fast, 0x12e5e0 matched each time): `__assume` on arg_3, arg_1, `local_4 <= 512`, local_19 and local_13
// 607 each.
// m3 lane 1 r103: reviewed (aligned 487, register-masked 222 on this body). The first structural difference is still right after
// the 0x130f60 call: retail moves the result to EDX, makes NONE in EAX and compares the palette byte in CL (`cmp cl, al`); ours
// reads the short slot with `movsx bx` and keeps NONE in ECX, the result in EAX (the tag arm then compares `eax, ecx` where
// retail has `edx, eax`). Every lever for that point is listed above; no untried natural form found, no trial run.
// P492: Intensity destination owns zero-alpha branch, two branch polarities,
// conditional contribution/destination sum, short then char palette:
// 775/775/698/775/778/778/637/778. NOT kept: no byte equality; original
// input capture, helpers, palette and full body restored at 607.
// m3 lane 9 r123 (fast with 0x12e5e0/0x130f60/0x131030/0x1317a0): the NULL-record `goto` removed by declaring the record
// before the slot test and testing it after (`if (record) ... else if (slot == NONE && tag != NONE) ... else default`)
// 1087 (4151 bytes: VC keeps the repeated slot test); the field_6c store first in its publication block 604 (statement
// order only: retail stores it before the last two fields, ours after them); the tag arm's third-accumulator weight added
// before the three merges 607; every cluster record field indexed through local_0[local_11] (no record pointer) 609;
// a walking record pointer for the loop 794 (3861 bytes).
// P504: Independent immutable destination/source pointer bindings at scalar, merge, four-channel and
// normalized-publication helper boundaries: 607/607/607/607/607/607/607/607. NOT kept: All eight tie 607 at 3832
// bytes. Provider-return/NONE, palette/selected-record and zero/SIMD carriers remain unchanged. Original pointer
// contracts, alias-safe input captures and complete helpers restored. Original complete executable body restored.
// M3 lane 15 r87 (fast with 0x12e5e0/0x130f60/0x131030/0x1317a0, all stay matched; base 607, register-masked aligned 275):
// the best-tag and best-weight defaults declared after the two cluster lookups 776 (3821 bytes, the clears reorder); the
// palette records declared ahead of the cluster distances 607 (same code); the palette arm's range read into its local
// and then clamped like the tag arm 680 (3838 bytes); the three 0x131030 outputs declared with the function's other
// accumulators 607 (same code); both fallback clamps as `x = x < 0.0f ? 0.0f : x;` 607 (same code); the six-field
// helper's intensity product written inverse first (retail: `movaps xmm1, xmm0; mulss xmm1, xmm2` after the blue store)
// 607 (same code: ours still computes it first, in place in the test's register).
// M3 lane 13 r96 (fast with 0x131030/0x1317a0/0x130f60/0x12e5e0, all MATCH each run; control 607, LCS-aligned 487,
// register/stack-masked aligned 220): the palette byte read at each use re-measured 666 (masked 219); on it a named 0xd4 tag
// read before the 0x29 test 673. Cumulative build-up toward retail's content, all positional-worse because the top
// still lacks retail's `mov edx, eax` (the 0x130f60 result in EDX, NONE in EAX), so every later line stays one off:
// per-use byte + the three-scalar helper as 0x1317a0 writes its own (clamp in place, products, then weight and
// remaining) + the merge helper in retail's order (clamp, colour block under the source-weight test, then weight and
// remaining) 670 (3817 B, masked 121); that with the 0xd4 tag named inside the 0x29 test (tag test joined to the
// 0x1c test) 707 (masked 120: NONE leaves its register); the four-channel helper taking r, g, b, a by value with the
// clamp in place, alpha term and weight first 693 (3817 B, masked 101; the zero now in XMM7 as retail); plus the tag
// arm's range read from the field at each use 705 (masked 99); plus the six-field publication as a macro used three
// times (direct accumulator access) 715 (3826 B, masked 35: the three blocks are then retail's load-multiply-store
// order, which the inlined pointer helper never gives); plus a 0x1318a0-shaped getter for the palette lookup 704
// (3818 B, masked 23; one-caller getter, match-only); a named cluster index on that 704 (same code). Left on the
// masked-23 lead (build/r96/lead-12efb0.patch in lane 13's worktree): retail keeps the cluster index in EDI across the
// 0x130f60 call for the first `== arg_0` test (ours reloads it after the call, so the record takes EDI and the tag
// EBX, retail EBX/EDI), the result/NONE pair (EDX/EAX in retail), the early `movsx ecx, cl`, the 0x88 pointer's `lea`
// before the tag test's branch, the tag arm's clamp store and `mov eax, edx`, one fallback add's operand order, the
// tag arm's four-channel schedule and the 0x6c store.
// M3 lane 2 r88 (fast with 0x130f60, 0x131030, 0x1317a0, 0x12e5e0, all MATCH; control 607): retail adds the third accumulator's
// weight with its operands reversed (`movaps xmm1, xmm0; addss xmm1, [acc]`) in the tag arm and the fallback; written as
// `local_7.weight = local_25 + local_7.weight` in the tag arm 607, the same in the fallback 607, both 607 (all the same code: the
// add canonicalises); the tag arm's third-accumulator weight added between the second and third merges (accumulator order) 607
// (other code, same count); the palette range as a `g_547f80` default overridden by an if 702 (3827 bytes). The top cause (the 0x130f60 result
// moved to EDX with NONE in EAX) is unchanged.
// P537: Actual returned alternate identity has an early existence decision, positive then
// complementary bool/byte/short/long: 620/620/698/697/620/620/698/697. NOT kept:
// extra code, no provider/NONE or palette/SIMD recovery; original helpers and body restored.
// P566: Existing alternate-source/vector/weighted/cluster-output ordinary inline
// declarations, four isolated, source-vector/weighted-cluster/source-weighted/all:
// 757/607/879/607/757/879/911/911. NOT kept: no native provider/NONE or palette/SIMD
// recovery; original target, helper bodies and all provider declarations restored.
// P580: Scalar memcpy at cluster ordinal/signed palette byte/previous ordinal/tag radius/
// palette radius/both best identities/both best weights/all source-weight publications:
// 607/607/607/747/668/607/821/831. NOT kept: no exact provider/NONE or palette/SIMD recovery; original
// source observations, alias-safe helpers, arithmetic and complete body restored.
// M3 lane 8 r175 (fast with 0x12e5e0/0x130f60/0x131030/0x1317a0, all MATCH; control 607): the char slot re-measured
// on this body 700 (3828 B; retail's `cmp cl, al`, but the 0x130f60 result stays in EAX with NONE in ECX, slot in BL);
// on it the palette record's NULL declared at the loop top 700 (same code), the handled flag declared before the 0x130f60 call
// 702, a `long palette_index = palette_slot;` for the table test and lookup 751 (3842 B, first difference +0x2e); on the
// short slot, the tag arm's radius read from the field at each use (as the palette arm reads its own) 610 (3835 B).
// Body unchanged.
// P591: Incoming binding masks 1/2/4/8/3/5/9/15: eight 607s; seven pointer forms
// excluded for unresolved caller 24. Original-site count/radius/contribution/fraction/
// reciprocal/coefficient bindings: seven 607s. NOT kept: native homes unchanged; all restored.
// Round 179 (m3 lane 8, fast; control 607, register-masked difflib count 275): the direct field_4c publication
// already matches; the three publications through function_12efb4 differ only in order (ours hoists the intensity
// product and its store above red/green/blue; retail keeps source order), and in the field_70 block retail stores
// field_6c before the last two products' stores. The char slot re-measured 700 (masked 275); field_6c stored first
// after the reciprocal 604 (masked 273); field_6c stored just before field_70[7] 604 (masked 273); the helper's
// intensity product written factor first 607 (same code); the char slot with field_6c first 700. Body unchanged.
// M3 lane 9 r133 (fast; control 607 at 3832 B): a redundant `if (0 < count)` guard around the cluster loop 607; the cluster
// entry pointer `const` 607; the palette slot `short const` 607; the unit clamp helper as two independent tests (`if (w < 0)
// w = 0; if (w > 1) w = 1;`) 607 (all four the same code); the clamp helper as one nested ternary 695 at retail's 3817 bytes
// (register-masked differing lines 662 against the control's 552, so the length is a coincidence). Body unchanged.
// Typed cluster-table stride with the encoded byte in short, char, long or at each use scored
// 607/700/706/666. The same forms reading the palette-owner member scored 607/700/706/667.
// Typed palette stride, direct typed result arms, table receiver, previous-ordinal snapshot,
// conditional result with snapshot, missing-first arm, early signed byte with snapshot,
// and early byte with promoted index: 607/607/607/606/606/607/699/749.
// NOT kept: the best gain is one point. The provider result and NONE still occupy different
// registers, and lookup failures still bypass the record test.
// N404: Shared previous-record/tag index, previous index read once, shared palette/tag payload,
// and both shared locals 607/607/607/607. The same four with signed-byte palette admission score
// 700 each. NOT kept: no lower score. The native provider/NONE, palette/record and
// SIMD register assignments still differ.
// Unsigned palette-byte admission before and after defaults, each with signed char, short, long
// or signed use-site decoding 604/796/794/604/604/796/794/604. NOT kept: the 604 forms gain three
// bytes only and still compare BL with an immediate. Provider/NONE, palette/record and zero/SIMD
// homes still differ.
// Separate table/lower/upper gates 607; interval inside table guard 607; accepted slot via compound
// or nested bounds 721/721; required entry count 608; conditional record inside table guard 607;
// fresh lookup before previous record 607; signed-byte slot with separate guards 700.
// NOT kept: the shared record test and provider/NONE, palette/record and SIMD homes still differ.
// M3 lane 4 r213 (fast with 0x12e5e0/0x130f60/0x131030/0x1317a0, all MATCH each run; control 607, difflib-aligned
// 354 exact / 118 with registers and slots masked). Nothing kept: every form below is positional-worse.
// Lane 13's lead (build/r96/lead-12efb0.patch) re-measured on today's base 704 at 3818 bytes, aligned 124 / 15: what
// is left is the loop head and nine small schedules. Head findings: (1) retail forms the 0x88 source pointer (the
// EDX argument of 0x1317a0) before the tag test's branch and reads the 0x1c test through it; with the tag read into
// a local and the pointer declared ahead of `tag != NONE && source[7] > 0` ours does the same and NONE leaves EDX,
// 676 (3824 bytes, 116 / 20), but ours then keeps no NONE register at all (`cmp cl, 0xff`, immediate stores) where
// retail copies the 0x130f60 result to EDX and puts NONE in EAX: the EDX pointer is what keeps retail's NONE out of
// EDX. Pointer declared ahead of both tests 708; the tag local with the pointer inside 704 (same code as the lead).
// (2) on the lead a char slot local kept to the source-index store keeps the cluster index in EDI across 0x130f60
// with the base loaded first, as retail (695, 3818 bytes, 124 / 15; slot BL, record ESI); a short slot local 710;
// with the per-use byte reads the record takes EDI and the index is reloaded. A named cluster index for the lookup
// and the first test, or for all three tests, 674 (same code as reloading: the build knows 0x130f60 writes nothing).
// On the 676 form: a long palette index local for the lookup 676, reused for the source index 624 (3840 bytes,
// first difference +0x2e), the record as a conditional expression 676, with the index local 676; the handled flag
// before the call 675 or after the three stores 676; the result declared NONE first 676; `NONE != tag` in the
// else-if 676; the weight store first 676. On the control: the palette byte at each use 666 (3827 bytes, 419 / 117),
// with the lookup's NULL inside its arm 666 (same code); the lead's lookup written in the arm 715.
// Retained palette tag, nested/compound 607/714; alternate tag, compound/nested 607/607.
// Both retained tags, compound/nested 712/606; with signed palette byte 597/699.
// NOT kept: the 597 gain only occurs in combination, while the provider homes and shared record test still differ.
// The provider result stays in EAX and the palette, record and SIMD homes differ.
// Joined palette/tag dispatch with separate lookup, missing-palette tag assignment and conditional
// tag 1087/1085/1085 with a short slot, and 1083/1081/1081 with a signed byte.
// Joined dispatch with a shared presence decision 1089; one sparse-table receiver 1085.
// NOT kept: the common dispatch adds over 300 bytes and moves the first difference into the clear prefix.
// shared uncovered weight from distance 604; shared uncovered weight from cluster 604; shared weighted
// remainder addition 702; common default accumulation at both missing sources 703; ordinary inline default
// accumulation 703; common default with native palette reads 699; common default with calculated weight 608;
// common default and shared uncovered weight 609. NOT kept: the two shared uncovered-weight forms gain three
// points at 3828 bytes and retain the provider, palette and record assignment gap. The common default block
// forms regress. The opening executable source is restored.
// M3 lane 7 r184 (fast with 0x130f60, 0x131030, 0x1317a0, 0x12e5e0, all MATCH; control 607, aligned 355, masked 122):
// 1..8 dead locals (diagnostic) 606-608 (the integer ties do not move); the record lookup as two inline getters (previous
// index, else the palette entry by slot: NULL result, global, bounds, entry) taking the slot byte read at each use (test,
// argument, source-index store) 683 at retail's 3817 bytes, aligned 424, masked 118: the lookup block is then retail's
// (byte compare, one widening, record in EBX) but for the NONE/result registers; with function_12eb00's count passed to
// the outer getter 683 (masked 115); that with the three-scalar helper accumulating colour, intensity, distance, height,
// then weight and remaining 693 (3823 B, masked 105); that getter form with each clear followed by its unit default 683
// (same code). Left on the getter form: NONE in EDX with the 0x130f60 result kept in EAX (retail moves the result to
// EDX and keeps NONE in EAX down to the 0xd4 tag test), zero in XMM5 (retail XMM7) and the accumulator schedules.
// Select table and ordinal, table and byte offset, conditional scenario table and ordinal,
// or nested scenario table with late byte offset: short 719/720/724/724; signed byte
// 619/696/619/619. The best measured forms disturb the matching clear prefix.
// M3 lane 2 r102 (fast; control 607). Reading: retail's NONE stays in EAX from the loop head through the palette arm to
// the +0xd4 tag test (`cmp edi, eax`), so it outranks the 0x130f60 result, which moves to EDX; ours stops using the NONE
// register before the tag test (`cmp edi, -1`, the field_f0 store's scratch registers take ECX/EDX), so the result keeps
// EAX. Tried: the slot as char 700 (re-measure; byte compare in BL, live to the store); char with the store re-reading the
// byte 666 (retail's CL slot and EBX record; NONE in EDX); the byte at each use with the +0x88 pointer declared ahead of
// the tag test 678; that with the tag in a named local used for the test and the best-tag store 679 (no NONE register at
// all); the byte at each use with the handled flag declared before the 0x130f60 call 669. Body unchanged.
// m3 lane 5 r202 (fast with 0x131030, 0x1317a0 and 0x130f60, all matched; control 607, aligned 354, masked 146): the
// touched flag declared ahead of the out value and the 0x130f60 call 609; the three per-cluster source resets stored
// weight, index, kind 607 (store order only); the best-weight and best-index declarations swapped 609 (first difference
// +0x2b); the palette arm's source-index store re-reading the +0x6f byte (retail re-reads it there) 703 (3831 B, aligned
// 332); the record pointer declared NULL ahead of the palette test 607 (same code).
// M3 lane 15 r102 (fast with 0x130f60; control 607): the palette byte read from the cluster at each use as a char
// (`*(char *)(cluster + 0x6f)` at the test, the bounds, the index and the source-index store; retail re-reads it with
// a fresh movsx for the store) 666 (3827 B); with the NULL record assigned in the else arm 666 (same code); with a
// file-local forceinline lookup that assigns NULL, tests g_4e0350 and the bounds and returns the entry (retail's
// `xor ebx, ebx` ahead of the global test and the `mov ebx, ecx` copy of the returned entry) 599 at 3818 B, the best
// count seen here, NOT kept (a new one-caller helper; lead: retail's lookup is an inline returning the entry with the
// global test inside); that lookup with the short palette local 604 (3822 B); the per-use char reads with 0x12eb00's
// existing entry_at_index behind an outer `g_4e0350` test (a local copy, as a header move would give) 603 (3822 B),
// the same as an if/else or a ternary around it 603/603; the short palette local with the ternary 667 (frame moves).
// Under the per-use reads the cluster index leaves EDI (retail keeps it there for the arg_0 compare).
// Palette read before the kind store, between kind and index, or between index and weight, then between
// kind and index with a separate promoted lookup index. Signed byte 700/700/700/749; long 706/706/706/704.
// NOT kept: the byte comparison width agrees at 700, but BL remains the slot carrier and EAX still holds
// the tag result.
// MiniMax A (11-18): no new form, no lower score (607): retail reads the palette byte into a byte register and compares it
// with the low byte of the -1 it also stores to the record, while this build keeps the slot in a 16-bit register and holds
// the -1 in ecx; the record's three initial stores sit before the palette read here and after it in retail, and moving
// them into the palette branch measures 725, the palette byte read before the stores 607, and a signed palette byte read
// before the stores 700.
// M3 lane 12 r149 (fast with 0x130f60/0x131030/0x1317a0/0x12e5e0, all MATCH; control 607): the two range clamps
// through one shared inline taking the range by value 679 (3837 B), by pointer (read at each use in both arms)
// 609 (3834 B), that with the char slot 702; the four source publications through one shared inline setter 607
// (same code); the tag result and its out value declared outside the loop 607 (same code).
// M3 lane 15 r106 (fast with 0x130f60/0x131030/0x1317a0/0x12e5e0/0x12eb00, all MATCH; control 607, aligned 487, register-
// and stack-masked 220): the cluster entry indexed at each use instead of the entry pointer 609; the palette index out value
// declared once at function scope and reset per cluster 607 (same code); the weight clamp as two independent ifs 607 (same
// code); the three per-cluster source resets ahead of the 0x130f60 call 713 (3836 B); the scenario lookup with the global
// tested in its own `if` around the bounds test 607 (same code). All restored.
// M3 lane 15 r111 (fast with 0x130f60/0x131030/0x1317a0/0x12e5e0/0x12eb00, all MATCH; control 607): the 0x130f60 call made
// through a file-local inline forwarder 607 (same code); the out value left without its NONE initialiser (diagnostic) 698,
// registers unchanged. On lane 7 r184's two-getter form rebuilt (683 at retail's 3817 B; the loop head equals retail down
// to the call, then result EAX / NONE EDX where retail has EDX / EAX): the outer getter's parameters index-first or
// index-second 683/683 (the previous index moves to EBX, the pair stays); the tag arm's 0x131030 argument, or its
// definition lookup, taken from the out value instead of the result (diagnostics, one result use fewer) 699/701 with the
// pair unchanged, so the result's use count is not what keeps it in EAX; one variable for the result and the previous
// index 683 (same code); the three source stores through an inline setter at all four places 684 (3818 B), the pair
// and the XMM5 zero unchanged. Ours never uses XMM7 in any form here; retail holds the zero there for 43 rows.
// Body unchanged.
// Y643: provider slot long at return 684; dword at return 684; word at return 696; long in tag arm 607; dword in tag
// arm 607; word in tag arm 696; word at return with signed palette byte 668; word in tag arm with signed palette byte
// 668.
// NOT kept: Late long and dword slots tie 607. The early decodes and the low-word forms increase the score. The
// signed-byte low-word forms narrow the length gap without recovering the provider and sentinel registers.
// Compound tag admission before the shared palette/default arm with a guarded lookup, conditional
// record or explicit missing slot scores 714 with a word and 703 with a signed byte.
// Testing provider validity first scores 832 with a word or 729 with a signed byte.
// Select record base and ordinal before forming the pointer. Short slot with default
// base, guarded/conditional 705/716; explicit base 709/617. Signed byte slot with
// default base 721/720; explicit base 677/698. NOT kept: the provider and palette
// registers remain different, including the 3818-byte guarded form. Original body restored.
// Palette upper before lower 607, signed byte 700. Positive table extent 755,
// signed byte 724. Inclusive last entry 725, positive extent with inclusive last
// entry 674. Lower bound before the scenario table 607, missing table as an
// empty extent with signed byte 689. Provider, sentinel and palette homes still differ.
// MiniMax B (11-19): no new form, no lower score (607): retail reads the palette byte into a byte register and compares it
// with the low byte of the -1 it also stores to the record, while this build keeps the slot in a 16-bit register and holds
// the -1 in ecx; the byte at each use and the char and signed-char slot measure 666 to 700, and the record's three initial
// stores sit before the palette read here and after it in retail, but moving them into the palette branch measures 725.
// Three-way source selection as conditional, explicit branches, missing-record default, or provider default
// with palette precedence scores 734/618/618/737 for the short slot and 730/704/708/724 for the byte.
// NOT kept: all raise the score, with the provider and palette register assignments still different.
// M3 lane 15 r113 (fast with 0x12e5e0/0x12e960/0x12eb00/0x130f60/0x131030/0x1317a0, all MATCH; control 607): reading
// retail again, its NONE stays in EAX from the `or eax, -1` after the 0x130f60 call through the whole palette arm (the
// +0xd4 tag test compares `cmp edi, eax` after all the inlined accumulations) and into the fallback store, so that web
// has seven uses; this body tests the tag against an immediate. Lane 10 r86's lead patch (its build/r86) re-applied on
// this base 693 (3820 B, aligned 128, register-masked 33; NONE reaches the tag test there, in EDX, and the result keeps
// EAX). On that lead: the result declared at function scope 693, declared ahead of the out value 693, the out value
// declared at function scope and reset per cluster 693, the out value declared ahead of the cluster record 693 (all
// the same code), the flag declared ahead of the out value and the call 697. Declaration order does not move the
// result/NONE pair. Body unchanged.
// Default stores sentinel-kind-weight 606, kind-weight-sentinel 607, weight-sentinel-kind 607,
// weight-kind-sentinel 606, sentinel-weight-kind 607. Completion default after metadata 607.
// Explicit admission before defaults with a retained ordinal 607, with slot re-reads 666.
// NOT kept: the one-point gains leave the palette width, sentinel and provider registers different.
// M3 lane 16 r153 (fast, file alone; control 607, aligned 487). Reading: retail reads the palette byte again at the
// source-index store (`movsx edx, byte [esi+0x6f]`), so the cluster pointer stays in ESI through the palette arm and the
// record takes EBX. New: the slot as `long` 706; the record pointer declared at the loop top 607 (same code); the
// source-index store reading the byte again with the short slot kept for the tests 703 (3831 B, aligned 464: record in
// EBX and cluster in ESI as retail; the slot is still a word and NONE is in EDX), the same with a char slot 666 (3827 B:
// retail's `mov cl` and byte compare, NONE in EDX against retail's EAX with the call result moved to EDX); the slot read
// ahead of the three source resets 607 (same code).
// M3 lane 16 r158 (fast; control 607): the 0x130f60 result read through a `long const &` at its three uses 607 (same
// code), with the palette slot as a char local 700 (3828 B); the slot tested through a `short const &` 607 (same code);
// the slot read ahead of the three source stores 607 (same code); the slot tested on the field and declared inside the
// branch 750 (3848 B). Aligned reading: retail presets the record NULL (`xor ebx, ebx`) just before the table test and
// forms the table record in ECX, then copies it to EBX, as if returned by an inline getter.
// M3 lane 8 r204 (fast with 0x130f60, 0x131030, 0x1317a0, all MATCH; control 607): a `char` slot compared with `(char)NONE`
// 700 (3828 B); the 0x130f60 result typed `dword` 607 (same code); the tag flag declared ahead of the out value 609; the out
// value declared at the loop body's top 607 (same code); the three source resets ahead of the out value and the call 713.
// Current-cluster predicate after palette lookup, at its first publication, or before lookup 739/735/726.
// Predicate shared across both sources after defaults, after the provider, or before the provider 733/733/750.
// Shared cluster index for lookup and predicate 741; complementary predicate across both sources 733.
// NOT kept: All grow and regress, with a first difference before the original loop span.
// The provider result, palette width and selected record homes still differ.
// Weighted palette/tag/either arm continues and common completion continue, short slot 634/706/774/607;
// signed byte slot 631/691/771/700. NOT kept: the common short-slot continue ties and the other forms regress.
// countdown for with advancing cluster cursor 607; countdown while with pointer then count 1442;
// countdown while with count then pointer 1442; cluster pointer for with body countdown 607;
// separate remaining count and cluster cursor 607; cluster cursor compared with end 831;
// cluster cursor stopped at end 806; cluster cursor while with end 1508.
// NOT kept: The for countdowns tie. The other walks increase the length and leave the provider, sentinel and palette allocation unresolved.
// M3 lane 1 r142 (fast with 0x12e5e0/0x12eb00/0x130f60/0x131030/0x1317a0, all matched throughout; al = aligned differing
// lines / the same with registers and stack offsets masked, 487/222 on this body). The masked count is the structural
// gap, and most of it is the three-scalar helper's eight updates: retail multiplies each by-value scalar in its own
// register (`mulss xmm1, xmm0`, three independent chains that the scheduler interleaves with the colour updates) where
// this build copies the weight and multiplies the copy by the scalar (`movaps xmm6, xmm0 / mulss xmm6, xmm5`), one serial
// chain through XMM6. Forms: the palette byte read at each use, no slot local, 666 at 3827 B (al 747/220: the registers
// move, the structure does not); the clamped weight in a new local 608 (al 487/222); the scalars scaled in place by
// separate statements and then added 607 (same code); the helper's statements as colours, scalars, weight, remaining 718
// at 3838 B (al 466/198); colours, weight, scalars, remaining 718 (455/186); weight, colours, scalars, remaining 718
// (455/185); colours, scalars, remaining, weight 698 at 3829 B (451/187); scalars, colours, weight, remaining 718
// (463/187); colours, the scalars reversed, weight, remaining 718 (476/196); red, height, green, weight, blue, intensity,
// distance, remaining (retail's order of first appearance) 718 (467/199); colours, height, weight, intensity, distance,
// remaining 718 (458/193); height first 718 (455/217); colours, intensity, distance, weight, height, remaining 718
// (455/188); colours, weight, remaining, scalars 607 (488/201). None gives the in-place scalar products. Diagnostic: 1 to 7
// dead initialised locals 606-608 (al 487-488/222-232), so the product sides are not a symbol-count phase here.
// Condition-declared record, cached-record initializer, cache-miss scenario receiver, and conditional scenario entry:
// short slot 607/683/683/705; signed-byte slot 700/700/700/607.
// The last form ties in 3830 bytes with the provider, sentinel and palette registers displaced. Body restored.
// Split cluster lookup into ordinal before table, table before ordinal,
// displacement before table, or typed table before ordinal 606/606/606/606.
// With the signed-byte slot 699/699/699/699. NOT kept: the short forms gain
// one point and retain the cluster-base, provider and palette registers.
// M3 lane 8 r212 (h2m fast, control 607): the found flag declared ahead of the out index and the provider call 609; the
// three source defaults stored ahead of the provider call 713 (3836 B); the slot test reading the byte field directly with
// the short local kept for the arm 728 (3847 B); the provider result bound to a `long const &` 607 (same code); the
// three source defaults stored weight, index, kind 607 (same code). Body unchanged.
// M3 lane 16 r165 (fast with 0x12e5e0, 0x12eb00, 0x130f60, 0x131030, 0x1317a0; control 607): a `char` palette slot read
// right after the palette call, ahead of the flag and the three source stores, 700 (3828 B); the short slot read ahead
// of the palette call 617 (3828 B); the flag declared after the three source stores 607 (same code); the three source
// stores in reverse field order 607 (same code); a `long` slot read ahead of the palette call 689 (3823 B).
// M3 lane 14 r140 (h2m fast with 0x130f60/0x131030/0x1317a0, all MATCH; control 607): retail re-reads the palette byte
// for the source-index store (`movsx edx, byte ptr [esi + 0x6f]` after the accumulators) and widens the first read once,
// ahead of the previous-index branch, for the range test and the index. Forms: a `long` slot index named in the palette
// arm for the range test, the index and the source store, on a char slot 751 (3842 B, first +0x2e), on the short slot 797
// (3862 B); the palette lookup with the scenario pointer in a local and the record cleared inside the else arm
// (retail's `xor ebx, ebx` there and a join test) 607 (same code); that with the byte read at each use 666 (3827 B); the
// accumulator helper in retail's store order (red, height, green, weight, intensity, distance, remaining, blue) 716
// (3838 B). Body unchanged.
// M3 lane 6 r222 (H2M_FAST with 0x12e5e0, 0x12e960, 0x12eb00, 0x130f60, 0x131030, 0x1317a0, all matched in every run;
// control 607 at 3832 B, aligned 487, register-masked 220; body unchanged). Read from retail: the inlined accumulator
// merge scales the six source fields first and adds the weight and scales the remainder last (0x12f694-0x12f765, three
// times); the palette byte is loaded twice, `mov cl` for the test and `movsx edx, byte` again at the source-index store
// (0x12f152, 0x12f561); the scalar helper multiplies its three by-value scalars in place (`mulss xmm5, xmm0`) where ours
// copies the weight for each (the length gap). Measured by aligned count as well as score: the merge helper's colour
// block first 705 (3841 B, aligned 367); the slot as a char local with the store re-reading the field, the field at
// each use, or a char test with a long index declared inside the branch 666 (3827 B, aligned 746-747: the registers turn
// round); the short local with the store re-reading 703; merge order + one of the three slot forms 697 (3836 B, aligned
// 353-354). On merge order + the long index inside the branch, the scalar helper's eight statements in twelve orders
// (R, G, B the colour sums, I, D, H the three scalars, W the weight sum, M the remainder product): RGBIDHMW 607 (3833 B,
// 785 rows as retail, aligned 319, masked 153: LEAD at equal score, build/r222/lead-0x12efb0-aligned319.patch),
// RGBIDHWM 699 (aligned 333), RGBWMIDH 698, WRGBIDHM 699 (322), RGBWIDHM 699 (322), MWRGBIDH 696, RHGWBIDM 701,
// RGBHDIWM 701, IDHRGBWM 699 (330), WMIDHRGB 720, RGBHWIDM 701, RGWBIDHM 699 (322). On that lead, same code each (607,
// aligned 319): the weight parameter moved to first, second or third place; the scalar products spelled weight first;
// the clamp into a named local in the scalar helper (608), in the merge helper, in both (608); the helper's parameters
// renamed five ways. The clamp applied at the three call sites 666-699 (3806-3817 B, 776-779 rows: it is then
// computed once, where retail clamps in each expansion). Diagnostic: one to seven dead locals in the body 605-608, one
// to three in the helper 606-607 (aligned 319 each: not a phase effect). Left on the lead: the call result in EAX with
// -1 in EDX (retail EDX and EAX), the record lookup threaded past retail's `test record` join (the listed lookup
// helper gives it), the three scalar products through a weight copy, and the scheduling of the helper's stores.
// M3 lane 15 r128 (H2M_FAST; control 607): retail's failed palette lookups reach one shared record test with the record
// zeroed (`xor ebx, ebx`), ours threads them to the default arm. The lookup as one nested ternary 607, the record left
// uninitialised and set NULL inside the else block 607, the scenario test as its own `if` around the range test 607, the
// not-found case as a guard `goto` ahead of the body 607 (all the same code: still threaded); the slot byte tested in place
// with the short slot declared inside the arm 750 (3848 B). Body unchanged.
// M3 lane 14 r140 (h2m fast with 0x130f60/0x131030/0x1317a0, all MATCH; control 607): retail re-reads the palette byte
// for the source-index store (`movsx edx, byte ptr [esi + 0x6f]` after the accumulators) and widens the first read once,
// ahead of the previous-index branch, for the range test and the index. Forms: a `long` slot index named in the palette
// arm for the range test, the index and the source store, on a char slot 751 (3842 B, first +0x2e), on the short slot 797
// (3862 B); the palette lookup with the scenario pointer in a local and the record cleared inside the else arm
// (retail's `xor ebx, ebx` there and a join test) 607 (same code); that with the byte read at each use 666 (3827 B); the
// accumulator helper in retail's store order (red, height, green, weight, intensity, distance, remaining, blue) 716
// (3838 B). Body unchanged.
// Provider index into source field, then saved before defaults, short/byte/per-use slot
// 617/613/712. Keep the index in the field with branch clears 691/735/692 or
// with the common metadata check 726/716. NOT kept: retail uses a separate provider
// out slot and the palette and provider registers still differ.
// The 0x4820 frame and metadata cursor agree. Provider result, selected record and SIMD zero still use different registers.
// Captured palette byte with short in the arm 750; short/long fresh lookup 728/619; early byte admission 748.
// Cached previous index with short or explicit null and long 726/618; original capture with short/long lookup 729/620.
// NOT kept: The long lookup forms recover the CL byte load, but spill that byte, keep NONE in EDX and raise the score. The four callee controls remain matched.
// M3 lane 4 r247 (fast): the publication helper's weight named as the merge helper names it 607, weight and intensity named
// 607, intensity named 607 (all the same code: weight and intensity keep XMM2/XMM1 where retail has XMM1/XMM2); the
// three-scalar accumulation helper in statement orders r g b i d h w m 718 (3838 B), r h g w b i d m (retail's store
// order) 718, w r g b i d h m 718, r g b w m i d h 607. Body unchanged.
// M3 lane 3 r294 (h2m fast with 0x12e5e0/0x12eb00/0x130f60/0x131030/0x1317a0, all MATCH; control 607): on the palette
// byte read at each use (re-measured 666, 3827 B: retail's CL byte, one widening and EBX record, but NONE in EDX and the
// 0x130f60 result kept in EAX, so the field_f0 store's EDX ends the NONE register and the +0xd4 tag test compares an
// immediate): NONE written first in the +0xd4 test 666, in the tag-arm test 666, in the previous-index test 666, in all
// four tests with the byte test too 666 (same code each: operand order canonicalises); the source index reset stored
// first 665. Retail keeps NONE in EAX through the palette arm to that tag test (loaded into EDI), the result in EDX.
// M3 lane 7 r216 (H2M_FAST with 0x130f60, 0x131030 and 0x1317a0, MATCH each run; control 607 at 3832 B): the palette
// byte read at each use re-measured 666 (3827 B: retail's CL byte and EBX record, but NONE in EDX and the 0x130f60
// result kept in EAX where retail moves it to EDX and keeps NONE in EAX); the 0x130f60 result declared ahead of the out
// index and assigned at the call 607 (same code), on the per-use byte 666; the cluster pointer typed `byte const *` 607,
// on the per-use byte 666; on the per-use byte, every read through a file-local forceinline byte accessor 664 (3825 B),
// the first test only 666. Body unchanged.
// M3 lane 11 r153 (H2M_FAST with 0x130f60, 0x131030 and 0x1317a0, all MATCH; control 607, aligned 487, masked 232): the
// found flag declared ahead of the 0x130f60 call 608 (aligned 493); the out index declared ahead of the cluster record
// pointer 607; the palette bound test as `0 <= slot` 607; the loop bound as `count > index` 607; `NONE != slot` 607 (the
// last four the same code). Body unchanged.
// M3 lane 2 r140 (h2m fast with 0x130f60/0x131030/0x1317a0/0x12e5e0/0x12eb00, all MATCH; control 607, aligned 487,
// registers masked 275): lane 15 r102's lead re-measured 599 (3818 B); with the tag arm's radius clamp read from the
// field at each test (retail re-reads `[esi + 0x44]` there) 602 (3821 B); the scalar accumulate helper's weight and
// remaining updates after its six channels 718 (3838 B, aligned 466); the merge helper's after its weighted block 705
// (3841 B, aligned 367); both 732 (3847 B, aligned 346, masked 192); the `char` slot re-measured 700. Body unchanged.
// M3 lane 13 r149 (H2M_FAST with 0x130f60/0x131030/0x1317a0/0x12e5e0/0x12eb00, all MATCH; control 607). Retail's three
// publications through function_12efb4 are strict load-multiply-store in field order (as the directly written field_4c
// block is in both builds); ours hoists the intensity product whose operand the test already loaded. In the helper: the
// intensity product written `local_0 * arg_0->intensity` 607, the weight and intensity read once into named locals 607,
// the destination taken as `real (&)[6]` 607, both parameters as references 607 (all identical code). Diagnostic (the
// helper's body written out at its three sites, reverted): 701 (3843 B), the three publications in retail's order
// (aligned groups 123 -> 106) but an earlier mulss operand swapped. Body unchanged.
// M3 lane 4 r256 (H2M_FAST with 0x130f60/0x131030/0x1317a0/0x12e5e0/0x12eb00, all MATCH each run; control 607 at 3832 B,
// aligned 354, masked 147; body unchanged). Asked whether the top cause (the 0x130f60 result moved to EDX with NONE in
// EAX) is a tie that follows the number of locals, as operand ties elsewhere do. It is not: 1 to 8 unused initialised
// longs at the top (diagnostic) on this body 607, 606, 606, 606, 608, 608, 607, 607, and on the palette byte read at each
// use (re-measured 666 at 3827 B) 666, 666, 664, 664, 665, 667, 667 for 1 to 7; the instructions after the call are the
// same in every one (result in EAX, NONE in ECX here and in EDX on the per-use byte). On the per-use byte: the result
// declared first with a NONE initialiser and assigned at the call (two definitions) 666 (same code); the result in a
// second named local for the tests with the first kept for the lookup and the 0x131030 argument 666 (same code, on this
// body 607: the copy folds, no `mov edx, eax`); the result declared without an initialiser and assigned after the
// handled flag 668; the handled flag declared ahead of the out index 669.
// M3 lane 8 r232 (h2m fast with 0x130f60, 0x1317a0 and 0x12eb00, all MATCH each run; control 607 at 3832 B, aligned 487,
// register/stack-masked 220; body unchanged). Frame reading: this frame's homes are sorted by references x 1000 / size
// with an unstable order among equal densities, so any form that adds or drops one home moves the clears and the
// accumulator homes together (the +0x2e first differences below) and the positional count cannot fall before the home
// count is retail's. Retail's own listing re-reads the palette byte for the source-index store (`movsx edx, byte ptr [esi +
// 0x6f]` after the accumulator calls), compares it as a byte at the top and sign-extends it once ahead of the previous-index
// branch for a lookup shaped like the listed getter. Forms: a char slot local 700 (re-measure); the char local with a named
// long index for the bounds and the entry 751 (3842 B, first +0x2e, masked 228), that index also for the source-index store
// 751, with the NULL inside the lookup arm 751 / 751 (same code each); the short local with the long index 797 (3862 B); the
// char local with a file-local inline lookup taking a long index (NULL first, the global and the bounds inside) 689 at
// retail's 3817 B (masked 219: slot BL, NONE EDX, result EAX, record ESI), the same as one conditional expression 689; the
// short local with that lookup 604 (3822 B, re-measure of the listed 604); the byte read at each use with that lookup 600 at
// 3817 B (masked 228; slot CL as retail, NONE EDX for retail's EAX, record EDI for retail's EBX, the sign extension inside
// the lookup arm), with a named cluster index for the lookup and the three `== arg_0` tests 600 (masked 220, the listed
// form); the byte at each use with a long index local and no lookup function 666 (3827 B, masked 217; re-measure). None
// kept: the lookup is a new one-caller helper (match-only) and 600 is 7 under the control. Diagnostic, reverted at once: an
// undefined stand-in for 0x130f60 on the 600 form keeps the cluster index in a callee-saved register across the call
// (EBX, retail EDI; 747 at 3855 B because the stand-in takes the standard convention), so retail's index in EDI is what a
// call without a memory summary gives, as the earlier "the build knows 0x130f60 writes nothing" line says.
// M3 lane 15 r143 (fast screens with 0x130f60, 0x131030, 0x1317a0, 0x12e5e0 and 0x12eb00, all MATCH each run; control 607 at
// 3832 B, difflib-aligned 362; body unchanged). Cause found for the cluster index in EDI: retail loads it once ahead of the
// 0x130f60 call, copies it for the cluster pointer (`mov esi, edi`, with the cluster array loaded into ECX and not folded
// into the add) and compares EDI with the argument at the first test only; the two later tests re-read `[ebp - 8]`. Every
// earlier form folds both loads and re-reads the index at the first test, because the build can prove that the three
// source-record stores after the call leave the index alone, so a named index local is the same as the field. When the
// index is read through the array element (`local_0[local_11].cluster_index`, ahead of the record pointer) and the source
// record is written through the record pointer, or the other way round, the stores count as possible writes to it: the
// load pair then comes out as retail's (index in a callee-saved register, the array in ECX, `mov r2, r; imul; add r2,
// ecx`) and the record goes to EBX as retail. The allocator then ranks the cursor 170 (ESI), the cluster pointer 166
// (EDI), the record 138 (EBX) and the index 30 (EBP); retail has the cluster pointer in ESI, the index in EDI, the record
// in EBX and the cursor in EBP, so its cursor ranks below the record and its index above it.
// Forms (the lead is the palette byte read at each use with a file-local inline lookup taking a long index, re-measured
// 599 at 3818 B, aligned 449): on the lead, a named index local for the pointer and all three tests 600 (3817 B; the
// allocator ranks the whole range -132 and splits it into one piece per test); the local for the pointer and the first
// test only 600 (same code: the local folds into two field reads); the cluster pointer through a file-local inline
// accessor taking the index, or the structure and the index, 599/599; named index and array locals 600; those with the
// palette index initialiser between them and the pointer 600, the first test reading the field 600; the three initial
// source stores through a `long *` to the source kind (diagnostic) 694 at 3817 B and through a second record pointer
// 694 (aligned 448, the retail load pair with the registers rotated); no record pointer at all, every access through the
// array element 599 (aligned 458, no retail load pair); the index read through the array element ahead of the record
// pointer, used for the pointer and the first test 694; only the three initial stores through the array element 694.
// On this body: the index local for the pointer and the first test 607 (same code); index and array locals with the
// palette index initialiser between 606 (3830 B), the first test reading the field 606; the index read through the
// array element ahead of the record pointer and used at the first test 605 (3831 B, aligned 449), at all three tests 749
// (3847 B, first +0x1: the index takes a frame home), at none 715 (3834 B, first +0x1). On the lead, the index through
// the array element at all three tests 744 (3837 B). Diagnostic, restored at once: 0x130f60 defined `inline` on the lead
// 749 (3855 B, first +0x6d): the 1.0 constant leaves XMM3 across the call, which retail keeps, so retail's caller knows
// the callee's registers and the index in EDI does not come from an unknown callee. Forcing single allocator picks on
// the 694 form: the previous index into EDX leaves 604 (aligned distance 844 -> 527), the 0x130f60 result into EDX
// 706 (546), the cursor into EBP 689; no single pick reaches retail. Nothing kept: the retail load pair needs the
// index local at one use and the field at the two later ones (a mix, match-only), and every form with it is above 607.
// Encoded palette byte then signed-char/short/long ordinal or sign-bit bounds 604/796/794/680.
// Byte read before metadata defaults gives the same four scores. NOT kept: 604 gains three
// at 3829 bytes. Provider and lookup register assignments still differ.
// Shared palette address formation from ordinal, explicit source, byte offset or availability
// 716/617/718/713 with a short slot and 720/698/602/597 with a signed byte.
// Nullable table base or default missing offset with a signed byte 709/602.
// NOT kept: 597 sets and re-tests an availability flag; the offset forms gain only five.
// M3 lane 8 r237 (h2m fast with 0x130f60, 0x131030 and 0x1317a0, all MATCH; control 607). Allocator reading: the
// 0x130f60 result ranks 66 and colours first with its EAX preference; the loop-top NONE piece (stores and compares
// after the call, the else-if test and the fallback store) ranks -10 (live unreferenced through the palette lookup's
// blocks), so it gets ECX, which the field_f0 copy then clobbers, and the 0xd4 tag test falls back to an immediate where
// retail compares against EAX. Forms: the publication helper's intensity read into a named local for its test and
// product 607, its weight likewise 607, both 607 (same code each); the palette byte read at each use (test, bounds,
// index, source index) 666 (re-measured on its own: retail's `mov cl` byte load, NONE then in EDX); the three
// six-field outputs typed as a color plus three reals with the helper writing members 607 (same code). Body unchanged.
// Separate selection from contribution by resolving the tag in the no-palette arm,
// selecting it after lookup, checking palette precedence, or setting the palette
// pointer in each arm: 1085/1085/1087/1087 with short, 1081/1081/1083/1083 with char.
// NOT kept: the provider and palette roles still differ and the body grows past 4140 bytes.
// Typed scenario table indexing, shared scenario receiver, cached previous index,
// or record-selection initializer: 607/607/607/607 with short palette, 700/700/700/700
// with signed byte. NOT kept: the provider register and shared lookup test still differ.
// M3 lane GB r7 (fast with 0x12e5e0, 0x12e960, 0x12eb00, 0x130f60, 0x131030 and 0x1317a0, all MATCH; control 607 at
// 3832/3817 B, al=362, mk=232; 82 trials; body unchanged): copying 0x131030's `fog_accumulate` (ternary clamp +
// `fog_color_add_scaled` + intensity/distance/height/weight/remaining order), putting `if (weight > 0.0f)` first in
// `function_12efb3`, passing 4 scalar channels with ternary `(alpha == 0.0f ? 1.0f : alpha)` first in `function_12efb5`,
// writing the three `field_1c/field_34/field_c8` publications inline, moving `arg_3->field_6c = local_2;` before the
// reciprocal, hoisting `tag` and `local_22` before the palette tag test, and reading tag radius directly in the ternary
// makes ALL 785 float/XMM instructions, stack slots, branches and all 6 publications match retail 100%; paired with
// `long cluster_index = local_0[local_11].cluster_index;` (used at `local_13` and `field_f0`) and a file-local
// `fog_palette_entry_get` lookup (setting NULL inside `else`) this hits 86 at 3817/3817 B (al=64, mk=8; saved
// build/gb7/12efb0-lead-86.cpp), leaving ONLY a 3-way loop GP rotation (cursor ESI / local_13 EDI / cluster_index+tag+
// local_23 EBP vs retail EBP / ESI / EDI+EDI+ESI). Same 86 with `bool has_fog` (no goto), `widen_local_only`, typed
// cluster struct, outer loop var, inverted `if (!local_18)`, `local_13` before `local_12`, `local_12` after `0x130f60`,
// and `ptr_idx_arr_rst`/`arr_idx_arr_st`; sub-struct `source` with helper 99 (with `entry_at_index` 689); `entry_at_index`
// alone 689 (null store before `if` jump-threads); inline `else { local_18 = NULL; }` 701; `source` setter 653; flipped
// `arg_0 ==` 86; `while` loop 1435; pointer-end loop 885; reset before `0x130f60` 709; all `cluster_index` 762.
// Strictly clean non-mixing, no-new-helper forms with the new helpers/publications slash masked diffs 232->26 (al 362->85)
// and recover `cursor=EBP, local_13=ESI, local_23=ESI`, but score 702 (short slot) / 705 (signed char inline) / 691
// (all array indexing) / 694 (no alias with lookup helper) because `cluster_index` folds into `imul`. Lead 86 NOT kept
// (mixes `cluster_index` local with `local_12->cluster_index` and uses a one-caller lookup helper without byte match).
// Current-cluster predicate shared after provider, before provider, within the palette arm, or
// across weighted winners: short slot 733/741/739/717, signed byte 729/740/735/713.
// First difference moves into accumulator setup. Retail repeats the cluster comparisons.
// Existing tag block with direct members or one block pointer, direct or saved previous index:
// short slot 607/607/607/606, signed-byte slot 700/700/700/699. NOT kept: the best count falls by one,
// and the provider result, native palette compare and shared lookup test still differ.
// M3 lane GA r11 (fast with 0x12e5e0/0x12e960/0x12eb00/0x130f60/0x131030/0x1317a0, all MATCH across 42 trials;
// control 607 at 3832/3817 B, mk=169; body unchanged): clean `arg_3->field_6c = local_2;` before reciprocal 604 (-3,
// under keep bar); clean `fog_accumulate` (from 0x131030) / `efb3` weight-first / `efb5` 4-scalar alone 703/705/708;
// all four combined on main's short slot (no mixing, no new helper) 688 at retail's exact 3817/3817 B (mk 169->47:
// recovers `cursor=EBP`, `cluster_index=EDI`, `local_13=ESI`, `zero=XMM7` and all three `function_12efb2` schedules);
// with inline publications + short/per-use slot 701/703 (mk=35). Match-only probing on GB7's lookup-helper form:
// `static inline` else-only lookup restores `xor ebx, ebx` at 3817 B (`L00` 704, mk=20); using `cluster_index == arg_0`
// in the palette weighted test (`L07`) drops cursor priority by 1 use and breaks the 3-way GP rotation, snapping all
// 4 callee-saved registers to retail (`cursor=EBP`, `cluster_index=EDI`, `local_13=ESI`, `record=EBX`, `zero=XMM7`) at
// 555 (mk=13), but spills `cluster_index` 4 B across `tag` (`frame=0x4824`, `+0x1`, 3824 B) and mixes local/field reads;
// `static inline` full lookup + tag/fallback/palette block variations (`C00..C11`) 678..739 (mk 17..27; clean `C11` 704).
// M3 lane 7 r230 (h2m fast with 0x12e5e0, 0x12e960, 0x12eb00, 0x130f60, 0x131030, 0x1317a0, all MATCH; control 607):
// the slot read right after the lookup call, ahead of the flag and the three metadata stores 607 (same code); the lookup
// call after the metadata stores 616 (3835 B); that slot read as a char local 700 (3828 B); the flag declared first 609;
// the metadata stores in reverse order 607. The allocator ranks the lookup result (66, EAX preferred as the return
// register) far above the NONE constant and the slot (24 and below), so the result keeps EAX where retail moves it
// to EDX for NONE. Body unchanged.
// M3 lane GA r21 (53 fast trials + 22 whole-image full-link diagnostics; control 607 at 3832/3817 B, +0x15f): a byte MATCH
// (3817/3817 B, 0x12e5e0/0x12e960/0x12eb00/0x130f60/0x131030/0x1317a0 all MATCH, 62 verified targets, 0 differ), NOT kept:
// every matching variant loses downstream 0x1c4590 (score 5, eax/ecx swap in add) and 0x24cf66 (score 2, +0x3bd mov swap) in
// the full image (this translation unit links before both; 0xc2d00 and 0x185be0 stay MATCH), so the body is restored and the
// match is held as a patch. Matching recipe (all pieces verified necessary by ablation): call existing vector3f_add_scaled
// (include/vector_products.h) inside function_12efb2 with ternary clamp and intensity/distance/height/weight/remaining order
// (direct assign/+= without helper 13 at 3817/3817 B); function_12efb3 tests if (weight > 0.0f) before division; function_12efb5
// takes 4 scalar channels with (alpha == 0.0f ? 1.0f : alpha) first; tag arm clamps named raw_weight via ternary; inline
// field_1c/field_34/field_c8 publications (dropping function_12efb4, restoring it 184); arg_3->field_6c before reciprocal; widen
// long slot_index = *(signed char *)(local_13 + 0x6f); before previous_index test and call 0x12eb00's entry_at_index
// (if (table && index >= 0 && index < table->field_340)) in the else arm (inlining lookup 695..696, dropping slot_index 697,
// dropping table && 13); read palette tag via local_22 = local_19 + 0x22; long tag = *(long *)((byte *)local_22 + 0x4c);
// (same +0x4c offset as tag arm local_29, keeping local_18 in EBX, cluster_index in EDI, and local_22 live across cmp tag, NONE
// so NONE takes EAX and local_15 moves to EDX). Whole-image diagnostics: calling vector3f_add_scaled on HEAD alone flips
// 0x1c4590/0x24cf66 (including vector_products.h unused does not); all 16 matching 7- and 8-function variants (inline/forceinline
// helpers, typed s_1317a0_source, entry_at_index order/const/rename, dropping s_12ed00, local helper without vector_products.h,
// vector3f_add_scaled in function_12efb3, fog_accumulator_reset) flip both; the 6-function score-13 form (without vector3f_add_scaled)
// keeps both 0x1c4590 and 0x24cf66 MATCH (not kept: still requires match-only entry_at_index, one-use slot_index, and repeated
// publication blocks), and macro forms replacing any of the 6 functions to stay at 6 functions with vector3f_add_scaled score 245..714.
// Body unchanged (607).
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
