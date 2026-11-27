// @flags /O2 /Gr /arch:SSE
#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_11cc90.h"
#include "normalize3d.h"
#include "vector_products.h"
#include "vector2f_products.h"
#include <math.h>

#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

real g_47ffd0 = 0.5f;

struct s_29bab0
{
	byte field_0[0x14];
	real field_14;
	byte field_18[4];
	real field_1c;
	real field_20;
	byte field_24[0x20];
	real field_44;
	byte field_48[8];
	real field_50;
	byte field_54[0x10];
	real field_64;
	real field_68;
	real field_6c;
};
long function_1f5f30(long arg_0);
bool function_1f9760(long arg_0, long arg_1, point3f const *arg_2, real arg_3,
	bool *arg_4, bool *arg_5, vector3f *arg_6);
real function_1f9e70(long arg_0, long arg_1, point2f const *arg_2,
	vector2f const *arg_3, vector2f const *arg_4, vector2f const *arg_5,
	vector2f const *arg_6, real arg_7, bool arg_8);
point3f *function_b9dd0(long arg_0, point3f *arg_1);
real function_30bf0(vector3f *arg_0);
real normalize2d(point2f *arg_0);
bool __stdcall function_2922b0(long arg_0, long arg_1, vector3f *arg_2,
	vector3f *arg_3, real *arg_4, real *arg_5, bool arg_6);
void function_29b2e0(long arg_0, long arg_1, vector3f const *arg_2,
	vector3f const *arg_3, real arg_4, vector3f *arg_5);
void function_29b990(long arg_0, long arg_1, vector3f const *arg_2,
	vector3f const *arg_3, vector3f *arg_4);
void __stdcall function_1f8510(bool arg_0, vector3f const *arg_1, vector3f const *arg_2, vector3f *arg_3);
void __stdcall function_1f3e40(long arg_0, vector3f const *arg_1, vector3f *arg_2);
bool __stdcall function_110ab0(long arg_0);
void function_ba1d0(long arg_0, vector3f *arg_1, vector3f *arg_2);
void function_1e4290(long arg_0, bool arg_1);

PRIVATE __forceinline real function_29bab2(real arg_0, real arg_1, real arg_2)
{
	return arg_1 > arg_0 ? arg_1 : arg_0 > arg_2 ? arg_2 : arg_0;
}
PRIVATE __forceinline real function_29bab3(real arg_0, real arg_1, s_29bab0 const *arg_2)
{
	long local_0 = arg_1 < 0.0f ? -1 : 1;
	return (real)(sqrt(fabs((double)arg_1)) * local_0 * arg_2->field_6c + arg_0);
}
// score 462 (was 489): The 0x88 frame agrees. Position uses +0x8c for retail's +0x80, and the normalization squares and steering allocation differ.
// (A second header line, "489 (was 489)", with the same text, merged here in m3 lane 14 r145.)
// Earlier header line, a stack merged here in m3 lane 2 r139: 489 (was 489; m3 lane 15 r132, m3 lane 8 r220 and m3 lane 11 r151 unchanged, their lines just above the marker): The 0x88 frame agrees. Position uses +0x8c for retail's +0x80, and the normalization squares and steering allocation differ.
// Earlier first line (stacked, merged here in m3 lane 15 r132), 489 (was 489): The 0x88 frame agrees. Position uses +0x8c for retail's +0x80, and the normalization squares and steering allocation differ.
// The 0x88 frame agrees. Position uses +0x8c for retail's +0x80, and the normalization squares and steering allocation differ.
// M3 lane 8 r220 unchanged, its line just above the marker.
// Earlier score 489 (was 489). The 0x88 frame agrees. Position is at +0x8c for retail's +0x80, and the normalization squares and steering allocation differ.
// M3 lane 4 r230 (fast; control 489, difflib-aligned 224, register/offset-masked 77). With local_6.i/.j set before the k
// test (retail's order: its `movaps xmm7, xmm5` / `movaps xmm6, xmm1` copies sit ahead of the branch, the zero is made once
// before `cmp di, 2` and the field_68 test compares against it) the whole mode-2 block is retail's but for the MIN operand
// order: 677 (3474 B, aligned 165, masked 64); that with both clamps as MIN(1.0f, ratio) (retail tests `1.0 > ratio` at
// both) 676 (aligned 158, masked 59), the second only 677. The 25-byte shortfall is retail's per-arm y of the side point
// (+4 instructions: local_8.y read in each arm, which may also be what gives local_8 the lower home), the flip's
// `test al, al; setne al` (+2) and the 0x47c block (-1). The side ternary written in each coordinate, with the defaults
// first 384, with MIN(1.0f, ...) too 373 (3483 B at retail's 844 instructions, but the arms become an x87 temporary and
// the frame moves, masked 69: positional only); the y coordinate assigned before x 708 (frame moves); local_25 copied as
// a planar vector 706 (frame moves); vector3f_add's operands swapped 489 and local_13 declared before local_11 489 (both
// the same code); the side distance as an if/else assignment 489 and the flip as `local_21 = local_22 ? local_21 :
// !local_21` 489 (both the same code). All match-only or worse; body unchanged.
// MiniMax C (11-20): no new form, no lower score (489): the position and the sum still take each other's homes (the
// position at +0x8c where retail has +0x80), and the normaliser still squares k, j, i where retail squares i, k, j;
// that order is fixed in include/vector_products.h, which this lane may not edit.
// MiniMax C (11-19): no new form, no lower score (489): the position and the sum still take each other's homes (the
// position at +0x8c where retail has +0x80); neither declaring the position ahead of the sum nor hoisting the sum's
// declaration to the top of the body moves that slot, and the normaliser still squares k, j, i where retail squares i, k, j.
// M3 lane 8 r189 (fast; control 489): local_8 renamed `position` 489, local_14 renamed `destination` 489, both 489
// (same code: names do not order these 12-byte homes); local_9 filled through vector3f_assign 665 (3499 B), through a
// brace initializer of the three fields 765 (3486 B). Compile-only reading: the squares follow local_9's field ranks;
// the aggregate copy, vector3f_assign, the three field stores and the initializer all give k, j, i and
// normalize_inline k, i, j, against retail's i, k, j.
// Earlier score history (679 before m3 lane 4 r203).
// M3 lane 10 r119 (fast; control 489): local_13/local_14 renamed to sort after local_8 489 (same code: names do not order
// these homes); the input copy as `memcpy` 489 (same code); the input copy defined j, k, i (diagnostic) 767 (the squares then
// run i, k, j as retail, but the loads follow and 3481 B); the add's point through the position query's returned pointer
// 804 (3468 B); the blend bound as `real const &` for the three rows 572 (3502 B). Body unchanged.
// MiniMax G (11-13): no new form, no lower score (715): the body is 3482 bytes against retail's
// 3499 and the three-way branch and the float constants still differ from retail's own expression
// trees.
// MiniMax I (11-17): no new form, no lower score (489): the body is 3503 bytes against retail's 3499 and the slot homes still differ (local_8 at +0x8c against retail's +0x80, with local_14 the other way round), which needs the declarations moved; the normaliser still squares k, j, i where retail has i, k, j, and the retail grouping through normalize_inline for local_9 scores 775; the body is unchanged.
// Earlier: Frame 0x88 agrees. Position is at +0x8c for retail's +0x80,
// and the normalization squares and steering allocation differ. At 679, the target was
// changed to a vector3f from the existing
// Earlier at 489: m3 lane 4 r203 (plain, starting at 679) kept the target point local_14 as a vector3f from the existing
// Earlier score history (679 before m3 lane 4 r203).
// Earlier at 489: m3 lane 4 r203 (plain) kept the target point local_14 as a vector3f from the existing
// Earlier: m3 lane 4 r203 (plain) kept the target point local_14 as a vector3f from the existing
// Earlier: The 0x88 frame agrees. Position is at +0x8c for retail's +0x80, and the normalization squares and steering allocation differ.
// Earlier: m3 lane 4 r203 (plain) kept the target point local_14 as a vector3f from the existing
// vector3f_add (k is dead and stored nowhere): it now shares the 0x1f9760 output vector's 12-byte home as in retail and the
// frame is retail's 0x88 (first difference +0x2 -> +0x4e). Left: slot homes (local_8 at +0x8c for retail's +0x80 with
// local_14 the other way round, local_2 at +0x30 for retail's +0x1c between local_5 and local_6, local_6/local_7/local_15
// 4-8 bytes low), the normaliser's k, j, i squares (retail i, k, j) and the steering-flag block layout. Tried in r203:
// constant-left float compares at 12 sites 702 each but the 0x47c test 698 (3477 B, worse code there) and field_1c 704;
// own dead locals n = 1..7 702/716/716/716/702/702/702; the target point written into the output vector (diagnostic) 679;
// a three-component initializer `{ x + i, y + j, z + k }` 693; a point3f local_14 with a second cast 679 (same code); on
// 679 the outputs declared at function scope before/after local_8, local_7 before local_6, local_8 declared first 679
// each (same code), own dead locals 1..7 679/693/693/693/679/679/681.
// Earlier at 702 (was 715): m3 lane 16 r119 (plain) kept three retail shapes together: the second radial test's implied
// `!(local_23 > local_27) &&` dropped with the root written in the first test (no named double; EBX is then pushed after
// the 0x26c test as retail), `0.0f - local_17.j` named once after the heading's normalize for its three uses (retail's
// subss from the shared zero, at retail's place), and local_15/local_17 as planar aggregate copies (retail's integer
// moves). 3473 bytes, frame 0x8c against retail's 0x88: retail keeps local_2 between local_5 and local_6 and shares
// local_14's home with the 0x1f9760 output vector (+0x8c); ours puts local_2 above local_1 and gives local_14 its own home.
// Earlier at 715 (was 715): earlier 750. s1E walk against retail (3482 bytes, retail 3499; frame 0x88 agrees). Behaviour fixes: 0x29b2e0's
// third argument is the +0x290 pointer in EBX (0x29c2a4-0x29c2bb; was &local_5), and 0x1f9760's arg_4 (last_out) is the
// stack byte tested at 0x29c7e5 (was arg_5). Forms kept: direct input (no reference local), shared normalize3d for the
// private copy, MIN at both ratio clamps (retail divides twice, 0x29bfdf-0x29c015 and 0x29c3a3-0x29c3d0), float root
// (0x29c068 fsubr dword), the signed-root helper reading the scale from the definition at the product (0x29c595).
// Tried: complement local for 1 - field_64 (771, no change), vector2f struct copies for local_15 / local_17 (717 / 723),
// double root (775). Left: early input/heading and normalizer memory-product homes, stack slot order, per-region re-reads.
// MiniMax A (11-12): no new form, no lower score (715): the fast check still reads 715 (648 when the reduced image links the same normalize2d differently), and the calls that show up as different in the listing are that helper reached at our own address; what is left is the early input and heading homes, the normalizer memory-product homes, the stack slot order and retail per-region re-reads of its arguments, so the 3482-byte body with the 0x88 frame stands.
// MiniMax C (11-11): no new form, no lower score (715 in the full build; the fast check gives 648 or 715 depending on
// how the reduced image links); the calls that show up as different in the listing are the
// same normalize2d (retail 0x2b400) reached at our own address, so they are not real differences; what is left is
// the early input/heading homes, the normalizer memory-product homes, the stack slot order and retail's per-region
// re-reads of its arguments, with retail saving EBX around the middle for the +0x290 pointer where this build holds
// the input in EBX from the prologue.
// MiniMax H (11-05): no new form, no lower score; retail saves EBX around the middle for the +0x290
// pointer and re-reads its arguments from their stack homes per region, where ours holds the input in
// EBX from the prologue, and the 748 form is a held lead under the keep bar.
// Earlier 750 (was 750): Frame 0x88 agrees; early input/heading homes and normalizer storage remain. Alias-free normalizer loop 822; early-return clamp 829; output scale loop 824; output clamp loop 812; smoothing loop 825 (fast).
// Earlier 750 (was 750): Frame 0x88 agrees; early input/heading and normalization homes remain. Alias-free
// planar response snapshot 853; shared default vector 807; native input copy 804; shared radial predicate
// 809; native output copies 804 (fast).
// Direct-input short correction sign 840; blocked-direction predicate 805; opposite side 804;
// unsigned-word response mode 804; short sign plus blocked direction 841. Original body and helpers
// restored.
// Round 151: Alias-free actor/object/both observations 1089/874/1156; correction magnitude/signed part 804/804; all blend terms 750; native perpendicular 804; actor plus magnitude 1089.
// Frame 0x88 agrees; early input/heading homes and normalizer storage remain. Alias-free normalizer loop 822; early-return clamp 829; output scale loop 824; output clamp loop 812; smoothing loop 825 (fast).
// Frame 0x88 agrees; early input/heading and normalization homes remain. Alias-free
// Direct-input dot-product terms 804; complete velocity snapshot 870; output addition loop 824; positive
// configuration guard 831; fallback vector memcpy 804. Extra all-square-terms control 748 is excluded from the
// five new trials and gains only two. Frame/input/heading and normalization homes remain; original body/helpers
// Direct-input normalizer native input/output contracts 859/856; native normalizer reference 804;
// upper-bound-first clamp 803; native projection references 804 (fast). Original complete body/helpers restored.
// Direct input with immutable parameter 804; parallel blend complement 804; signed-byte correction
// sign 828; native projection values 1183; ordinary correction inline boundary 840 (fast).
// Round 212: Actor after configuration query 812; with short/long default mode 753/821;
// complete normalizer inputs 755, with short/long mode 821/821; all norm squares 755, with short mode 821.
// Round 173: Direct-input shared native and dot-based normalizers 793/785; magnitude output
// pointer/reference/output-first/vector-result 805/805/805/805; magnitude output with shared scaler
// 804; with real absolute-length boundary 809. All exact-match-only forms restored. Frame 0x88
// agrees; early input/heading retention and normalizer memory-product homes remain. Original body
// and helpers restored.
// Round 140: Direct input with complete immutable turn-distance/correction/history observations
// 854/853/819/818/855/854/815/814; response defaults three/two in short/long/word/byte storage
// 815/748/766/816/815/748/804/821; complete normalizer inputs/sum/all products and one-result exit on the new
// default-two graph 814/814/814/814/814/814/750/814. Best default 748 gains only two; original body, parameter
// access, helpers and mode assignments restored.
// Round 367: Returned position receiver/shared two-dimensional difference/both 838/750/838; immutable pre-
// blend snapshot 804; snapshot with position/difference/both 838/750/838; all with native position
// workspace name 838. Direct input in every form; all restored.
// Round 224: direct input plus shared cross/speed/publication 748/750/881; cross+speed 748;
// cross+publication, speed+publication, all, all+native normalizer setter 886 each. Two-point
// cross gains rejected; original input access, helpers and outputs restored.
// Round 267: Isolated response/position/normalized-input home identities on direct input,
// all seven subsets and jointly with the magnitude identity, 804 each. Frame 0x8c versus
// retail 0x88; original body, input access and helpers restored.
// Round 392: Native scalar/flag/heading actor fields, scalar+flag/scalar+heading/flag+heading/all,
// then all with object-first acquisition: 804/804/804/804/804/804/804/829. All direct-input forms restored.
// Round 394: Direct-input shared addition/scalers/both/projection/addition-projection/scalers-
// projection/all/all with shared normalizer 747/804/747/804/747/804/747/785. Three-point gains rejected; all restored.
// DH round 234: direct input/live lateral-radius field with last/scale/direction query
// output defaults, eight combinations 835/835/808/808/832/832/805/805. All restored;
// early input/heading and normalizer memory-product homes remain on frame 0x88.
// Round 408: direct input with lookup/position ordinary/inline/noinline declaration
// pairs, excluding ordinary/ordinary, 804 each (eight forms). Original declarations,
// complete body, input home and all arithmetic helpers restored.
// Round 417: direct input, per-use heading/up receivers and planar-normalizer inline
// declaration, seven subsets then all with staged cross: 903/803/891/803/903/803/891/803.
// All executable source, helper calls and declarations restored.
// Round 425: Direct input with collision-output flag/flag/vector lexical homes, all seven
// subsets 803 each; all three with the false flag initialized at first use 805. Original
// input access, query homes, initialization, helpers and complete body restored.
// Initial heading/default/input setter subsets and all with output setters
// 833/847/886/744/832/839/827/871; new-frame normalizer fabsf/input/inline subsets
// 744/746/746/744/744/936/936; best 744 at frame 0x88 and retail length
// gains six, below the keep threshold. Original input access, helpers and body restored.
// Round 246: initial heading library copy with paired defaults, normalized input
// and final output transfers in all eight combinations 803/803/803/803/803/803/803/810,
// all on direct input. Original copies, parameter observation and whole body restored.
// Round 47 (lane 10, fast with 0x1f9e70 and 0x110ab0 listed; retail keeps arg_1 in EDI and the +0x290 pointer in
// EBX from the prologue, ours the other way): the +0x290 pointer defined right after the actor 833, before the
// arg_8 test 833, before the 0x26c test 833; local_5 copied from the field with the pointer defined after it 741
// (positional only: aligned 590 -> 614), the pointer after local_7/local_8/local_10 741 each; local_7 before
// local_6 753; local_3 declared after the 0x26c test 746; local_8 and its 0xb9dd0 call before the pointer 816.
// Round 144 (lane 4, fast): ours keeps the input pointer in EBX from the prologue (retail reads it from its
// stack home, pushes EBX late for the heading pointer and loads the input into EDI only in the final arm), and
// caches the three normalizer components on the x87 stack (retail `fld [x]; fmul [x]` each). A branch-local
// input pointer copy 750 (same code); the normalizer squares through a two-pointer double dot 807; the epsilon
// test as `fabs(local_0) < 0.0001f` 750 (retail's dword constant, nothing else moves); the input reference
// taken after the position query 750; the normalized copy declared before the heading copies 750.
// Round 260: Native i/k/j norm with real-absolute/shared-scale/single-result combinations
// 792/786/792/786/791/785/791/785 on direct input. Original promoted products, input access, helpers and
// entire body restored.
// DH round 262: direct-input normalized setter with branch planar target/input/heading lexical-home
// subsets, then all with the planar magnitude identity, eight 750 at 3499 bytes. Frame 0x8c; all restored.
// DH round 264: Direct input and native normalized-input setter with zero-default/magnitude-default/
// canonical-magnitude/entry-result normalizer exits 807/754/811/750; matching float-absolute
// forms 807/754/811/750. All at retail length but frame 0x8c; original input, helper and body
// restored; early handle/heading and memory-product homes remain.
// Round 47 (m3 lane 13, fast, input reference kept; without it the input moves to EAX, frame 0x8c): the shared
// normalize3d() from include/normalize3d.h gives retail's normalizer instruction for instruction (memory squares,
// fst/fabs, dword epsilon, in-place mulss, zero store; homes 4 off) 771 (aligned 421 -> 413); the planar input and
// heading copies as `*(vector2f const *)` struct copies (retail's dword moves) 750 at 3499 bytes; both 797; both
// plus magnitude2d() for the radial distance (retail's float squares) 833 at frame 0x8c; that plus the radial test
// evaluated once through a conditional expression (retail's single fcompi branch) 850. Swapping the names of two
// vector locals moves no home. Retail keeps the object index in EDI and pushes EBX late for the heading pointer;
// ours holds the input pointer in EBX from the prologue.
// Y492: direct-input native planar input/heading/both copies 813/813/821;
// with normalized-input setter 758/758/766; both planar copies with initial-heading
// setter, without/with normalized-input setter 820/823. All restored; early
// input/heading and normalizer memory-product homes remain.
// Round 288: On native normalized-input setter/direct input: mode default/history coefficient/both/correction root/mode+root/history+root/all/all with shared addition; shared addition alone and with mode/history/root/mode+history/mode+root/history+root,
// 822/807/822/750/822/807/822/813/749/822/755/749/813/822/755 (fast). Original complete body, observations, helpers and call roles restored.
// Round 298: Uniform existing bound macro/direct input with normalized-input setter, staged cross
// and real absolute API in all eight combinations 804/750/750/809/807/750/748/816. Two-point gain
// rejected; original input, helpers and body restored. Frame/input/heading and norm homes remain.
// DH round 262: direct-input normalized setter with branch planar target/input/heading lexical-home
// subsets, then all with the planar magnitude identity, eight 750 at 3499 bytes. Frame 0x8c; all restored.
// DH round 264: Direct input and native normalized-input setter with zero-default/magnitude-default/
// canonical-magnitude/entry-result normalizer exits 807/754/811/750; matching float-absolute
// forms 807/754/811/750. All at retail length but frame 0x8c; original input, helper and body
// restored; early handle/heading and memory-product homes remain.
// Round 500: direct input and normalizer output orders ikj/jik/jki/kij,
// 804 each; with real absolute-length storage 748 each (two-point gains rejected).
// Original input access, helper and full body restored; early input/heading homes remain.
// DH round 266: heading/response/spare initialization after input normalization, subsets
// 1/2/4/3/5/6/7 and all with reversed defaults: 834/827/796/831/831/850/837/837.
// Direct-input lifetime graphs miss early heading and memory-product homes; original restored.
// Round 153 (lane 4, fast): collision flag declared first 750; input reference declared after the character
// lookup 750, at its first use 750; collision flag between the actor and object lookups 750; heading pointer
// declared const 750 (all the same code: retail still holds the object index in EDI and pushes EBX late).
// DH271: Native component aggregates for heading/default pair/normalized input, masks 1/2/4/3/5/6/7, then
// all with reversed independent defaults, 819/840/811/882/784/813/847/832. All new forms use direct input
// and regress; original input observation, real vector copies, private helpers and full body restored. Early
// input/object/heading and memory-product homes remain.
// DL231: Direct-input complete radial square/scaled projections/scaled correction, seven subsets; all with
// ordinary correction inline: 750/748/748/804/750/748/748/840. Frame/input/heading and normalizer memory-
// product homes remain; original executable source restored.
// Round 254: Direct input normalized before readonly position provider, with complete input
// setter, real absolute API and single normalizer exit combinations
// 812/813/815/817/768/829/768/829. Original input access, helper and body restored; early
// input/heading and normalization homes remain.
// Round 29 (lane 17; register-masked aligned difference 156 -> 122, 3499 bytes). Kept, all retail's shapes: the first clamp as MIN
// of the expression (retail evaluates it twice), normalize3d(), planar struct copies, a named blend complement, one test for the
// two false dot/range exits, the float sqrt(1 - g*g). Fast, from the old body: the volatile parameter
// read at each use 824 (3516 bytes), with normalize3d 766 (3502), with the planar copies too 760 (3479), planar copies alone 807; MIN
// at both clamps 922 (3589; masked 140), first only 859 (3537), second only 861 (3547); on normalize3d and the planar copies, MIN
// at both 838 (3553; masked 118), first only 727 (3509), second only 780 (3519), with the float sqrt 785/788. Grid of those plus the
// single radial test: 63 forms 727..., best without MIN 767 (normalize3d, float sqrt). On the 727 form: dots named before the inverse
// 771 (3503), with the named blend 720 (3504; call results in one-use locals, not kept), named blend 768 (3500; masked 112), complement
// only 768 (3503), i and j stored before the second clamp's test 791 (3518), with the named dots 784; single radial test through
// magnitude2d or sqrt 818/818 (3474, frame 0x8c: retail's shape but its x87 squares need two more slots), with one false test 827;
// direct input 714 (3499, frame 0x8c: the input moves to EAX, not retail's stack argument, so the reference stays), with one false
// test 743, float sqrt 770, both 720, named dots only 768, named blend only 712, neither 721. One false test alone 742, float sqrt
// alone 785, both 719, with a named factor and complement 718 (masked 114; the factor copy beside a direct read of the field is a
// mix, not kept), with the complement only 716 (kept).
// On the kept body: MIN at both clamps 771 (3546 bytes; masked 109), as retail evaluates both; at neither 817 (3461; masked 127).
// The two clamps differ only because of those scores: the second still needs its MIN once other bytes fall.
// Not kept at review (11-02): the set needs MIN at the first of two identical clamps only, chosen by score (a mix);
// no consistent form beats 750 (on that body MIN at both 771, at neither 817), so the body is restored at 750.
// DH282: Complete double squared-magnitude producer with repeated component inputs, all square parts and
// single-result completion, seven nonempty subsets then all with k/j/i component declaration priority [750,
// 750, 805, 798, 798, 798, 801, 801]. Original executable body restored.
// M315: direct input/native normalized setter with all six complete projection-product declaration
// orders 818 each; with scaled products at each residual or before all residuals 806/806.
// Original body and private helper restored; frame/input/heading and normalizer memory homes remain.
// Round 541: Standard component-range heading copy, paired default/normalized-input/final-output
// families, masks 0..7, with direct input: 839/840/834/829/828/848/851/836. Two forms reach
// retail length but regress; original input storage, transfers, includes and full body restored.
// Round 84 (m3 lane 12, fast): retail saves EBX only around the middle (push after the 0x26c test, pop
// before the closing arg_7 test) for the +0x290 pointer, where ours holds the input in EBX from the
// prologue; that pointer and the whole middle in their own block, with the two output vectors declared
// ahead, 750 (identical code: scope does not shrink-wrap it); the half factor as a float product 806
// (3505 bytes); the side offset as `0.0f - local_23` 750; the lateral default as an assignment and an
// if 809 (3505); the last output copied from the 0x66c store 780 (3518).
// Round 54 (m3 lane 14, fast listed with 0x1f9e70/0x110ab0): the first steering-flag arm setting true in its own
// else (retail sets the flag per path and its false paths share one `xor al, al`) 810 (3506 B); the offset point
// as local_24 * component 750 (same code); a named perpendicular { -j, i } of the normalized heading at its three
// uses (retail computes the negated j once, right after that normalize) 832 (3514 B), declared after the
// normalize 832; direct input 804 (3499 B). Original body restored.
// Round 64 (m3 lane 1, fast): normalizer test on `fabs(length)` without the double cast 750; dual double casts
// in the squares 750; normalize3d's shape (length zeroed in an else, one return) 798 (3499 bytes: retail's float
// epsilon and zero store, but our squares still keep i/j/k on the x87 stack where retail multiplies from the copy's
// memory, so the length is stored and reloaded); that with the cast on the other operand 798; with the input read
// directly 805.
// Lane 18 round 34 (fast): the private normalizer's test as `fabs(local_0) < 0.0001f` 750; its squares as float products
// (retail's fld/fmul memory squares) 791 at 3499 bytes; that plus the normalize3d-shaped if/else exit (retail's dword
// epsilon, ja and divss, the length stored at [esp+0x40] as retail) 771 at 3499; plus float radial squares 803 (3482);
// plus the scale as `local_1 * v` 771. The input/heading homes (input pointer in EBX from entry, local_2 at
// [esp+0x2c] for retail's [esp+0x20]) keep the rest positional.
// DH321: Direct-input ordinary inline declarations for existing mode/query/trace/response/turn/
// direction/transform/velocity providers individually: 804/804/804/804/804/804/804/804. All declarations,
// input storage, arithmetic helpers and complete executable body restored.
// Lane 14 r57 (fast, from 750): retail tests the radial distance once (float squares from memory, fsqrt, the
// gap loaded after it, one fcompi), each arm then taking its own exit tests. The two radial tests as one
// conditional expression 816 (frame 0x90); with float squares 804 (3504 bytes; the squares are retail's, but
// the gap is loaded before them); with the distance on the left of the compare 818; float squares and the
// distance on the left in the original two tests 785; the square root written in the conditional's test 831
// (frame 0x8c).
// Round 41 (m3 lane 17, fast with 0x1f9e70/0x110ab0, control 750): the +0x290 pointer written at each use 905 (3578
// bytes); the input copy before the position query 758; the three velocity dots named before the reciprocal 748
// (retail divides after the dots, ours still schedules the divide first); the signed-root scale by reference 833
// (3475 bytes: retail's `fmul [esi+0x6c]`, a parameter change, diagnostic only); the 0x654 arm's two false exits
// in one test 800 (3501), with the named dots 798 (3499); a named blend complement 801; one radial test with the
// dot tests in both arms 825. Retail also keeps `0 - heading.j` in a register for its three uses and tail-merges
// every false exit of the steering flag into two `xor al, al` blocks.
// DH321: Direct-input ordinary inline declarations for existing mode/query/trace/response/turn/
// direction/transform/velocity providers individually: 804/804/804/804/804/804/804/804. All declarations,
// input storage, arithmetic helpers and complete executable body restored.
// B415: Direct-input completed turn-snapshot/planar-query/velocity workspaces, last-consumer scope
// B415: masks 1/2/4/3/5/6/7/15: 804/804/804/804/804/804/804/804. Native length but frame 0x8c; original input access,
// B415: complete body and all helpers restored; early input/heading and normalizer homes remain.
// DH342: Existing vector component arrays throughout normalization/projection/body,
// seven subsets 750/750/807/750/807/807/807; all plus marker point components 807.
// Original fields, parameter storage, helpers and body restored; early input/heading and norm homes remain.
// DH345: Direct-input complete direction results: logical/conditional admission and
// equality/inequality side completion, eight graphs 776/814/813/822/783/811/800/757.
// All regress; original input storage, decisions, helpers and complete body restored.
// DH348: Direct-input immutable initial/planar magnitudes, half-span, lateral radius,
// side width, response reciprocal, complete residuals and correction distance: eight at
// 804/3499, frame 0x8c. Original input storage and body restored; early input/object/
// heading retention and normalizer memory products remain.
// Round 52 (m3 lane 2, fast; lead, not kept, score rises): retail's 0x29b2e0 reads its first vector from EBX and retail's EBX at this
// function's call is the +0x290 pointer, so the call is (arg_0, arg_1, local_4, &local_5, local_24, &local_5), not &local_5 first. That
// corrected call alone 848 (3464 bytes). The same correction on 0x29ab90 (with its tail rewritten around it) took that function 345 -> 267.
// M3 lane 4 r165 (fast with 0x1f9e70/0x110ab0; count/aligned/bytes, control 750/a430/3508, register-masked aligned 156): retail reads the input from its stack
// home only for the copy and again into EDI in the final arm, with the object index in EDI until then and the heading pointer in EBX. Input read directly 804/a439/3499;
// directly with an arm-local const reference bound to a converted temporary (the form that matched 0x45560; the input stays on the stack with no
// address taken, but EBX still holds it from the prologue) 750/a429/3508, with the reference kept for the first read 750/a431/3508; arm-local plain copy on direct input
// 748/a442/3501 (the input arrives in EAX), on the reference 750/a432/3508. Grid with planar struct copies and normalize3d: reference + copies 750/a439/3499, + normalize3d 771/a429/3499, both
// 797/a420/3474; arm const reference + copies 750/a438/3499, + normalize3d 771/a428/3499 (masked 136), both 797/a419/3474; arm plain copy + copies 760/a443/3485, + normalize3d
// 785/a440/3485 (masked 138), both 802/a433/3469. Two definitions of the arm's input (diagnostic) 748/a442/3501, with normalize3d 785/a440/3485. Volatile read at the copy and arm
// (diagnostic, reverted at once) 790/a447/3479.
// B433: Library transfers at history/working-direction/pre-blend copies, seven subsets,
// then all with the initial heading: eight at 804 on direct input. NOT kept: frame
// grows to 0x8c despite retail length; original transfers, input storage and body restored.
// Round 211 (m3 lane 3, fast; aligned with registers and stack/both masked, control 430/156): applied together, retail's
// forms take the aligned count to 391/87 at 3499 bytes, but 783 positional, so none kept. They are: normalize3d for the
// private normalizer; `*(vector2f const *)` whole copies for the 2D views; `real perp = -local_17.j` before the half
// span; the two `1.0f > expr ? expr : 1.0f` clamps with the expression written twice, each `(1.0f - t) * k` / `t * k`;
// `sqrt(1.0f - g * g)` on floats; `MAX` written out for the squared distance and the dot; `(... * 0.5f) ^ 2 / d * 0.5f`;
// the three velocity dots named before the reciprocal; the signed root with `real const &` for the sum and scale
// (retail's `fmul [esi + 0x6c]`); `local_7.i + local_6.i`; a named blend complement; the radial test once, false exits
// in one `||`. Single forms: admission as a forceinline helper with early returns 829 (3454 bytes, 415/141), that with
// all above 788 (3514 bytes, 395/91); the `||` 790 (3507, 400/89); `result != false` 787 (401/95); y then x named 797
// (416/103); delta rows j then i 781 (3499, 459/97); no helper, a `blocked` bool 787 (3507, 396/102); y inside each
// side arm 766 (3540 bytes, 401/98, x87 deltas); field_50 named first +8 frame 0x90, unnamed 783 (3499, frame 0x8c,
// 391/91); old double-root admission with the rest 783 (3499, frame 0x88, 411/104). Bare blocks around the out-query,
// the admission point or the later body: same code. Diagnostics, reverted: `volatile` input reads 790 and 835; `standard`
// marker with direct input 818 (local_2 takes EBX); `(void)&arg_2` and `if (&arg_2)` leave it in EAX; input unused in
// the arm (wrong values) still pushes EBX first. What decides the rest: retail keeps arg_1 in EDI to the position query
// and pushes EBX late for the +0x290 pointer (EDI then the input, then the mode; local_2 stays in memory), and ours loads
// arg_2 into EBX at entry. Original body and helpers restored.
// Lane 12 r91 (held, coordinator): the 750 -> 748 below is under the keep bar (a gain of 8 for a function above 30), and it rests on
// score-picked forms (the float root, the planar struct copies, MIN) that pay back the corrected argument's 848; main's body kept,
// build/held/lane12-r91-29bab0-748.patch is the whole 748 form with the corrected 0x29b2e0 argument, a lead toward the match.
// M3 lane 12 r91 (fast; masked with registers and stack offsets, 255 of 844 at 750): retail's third argument at the 0x29b2e0 call is the +0x290 pointer in EBX;
// this body passed &local_5, a wrong value, now corrected. Kept together, 748 (3502 bytes, masked 164): the corrected argument, direct input (the reference local to it gone), normalize3d in
// place of the private normalizer, planar struct copies, the float root, MIN at both clamps (retail evaluates the expression twice in each), a named blend complement.
// LEAD, not kept (an inlined helper's parameter chosen by score, needs a ruling): retail's `fmul [esi + 0x6c]` comes from the signed-root helper reading the scale at the product. The scale as `real const &` on
// the set above 724 (3466, 161); that set without the planar copies and complement 713 (3484, 165), the definition pointer as the parameter 713 (same code), the helper as a macro 753 (3480). The same set by value 774 (3520, 174).
// Steps: corrected argument 848 (3464; EBX pushed mid-function as retail); with direct input 823 (3460, masked 227), direct input alone 804 (3499, frame 0x8c); with normalize3d 806
// (3446, 207); planar copies 814 (3430, 197); float root 819; both clamps 750 (3504, 174); the complement 748. By value without planar copies 774, complement only 772, planar only 750.
// Leave-one-out at 724: no corrected argument 730 (3512, 186), reference input 724 (158), no normalize3d 834, no planar copies 716 (3482, 169), no float root 784, no clamps 862 (3388), no complement 724
// (3468, 171), no reference 748. On 713: reference input 730 (3486, 262), with the complement 716 and 718, half-span scale folded 713 (173), velocity dots named first 728 (3488, 172), `local_7.i + local_6.i` 713 (same code).
// The negated j named for its three uses reaches retail's 3499 with the reference scale, planar copies and the complement, 758 (masked 152).
// EBX save still in the prologue (retail pushes it after the 0x26c test, which shifts about 40 lines): empty else arm on the 0x26c test or the arg_8 test, both, a compiled-out loop at the
// join, or around the whole middle: same code at 748. Retail re-reads its arguments from their stack homes per region (arg_0 in ESI at the top, arg_1 in ESI in the arm).
// M3 lane 4 r168 (fast; a one-file harness does not reproduce this function, 844). New: the parameter as a function-scope
// `vector3f const *const &` reference 750 (same code); the stuck test as `*arg_7 || !actor[0x5d0]` 750 (same code); the
// definition test's arms swapped (`if (local_2) { ... } else` the 0x654 clear) 844 (3524 bytes); the final output stores
// with arg_3 first 753; the first arm's 0x654 clear before its 0x655 set 749 (one point, under the bar). Re-measured
// listed forms: arg_2 read directly 804 (LTCG passes it in EAX, frame 0x8c); the input copy before the position query
// 758; the heading pointer right after the actor lookup 833 (3510 bytes); the reference declared at its first use 750.
// Retail keeps arg_1 in EDI and reads arg_2 from its slot, loading it into EDI only for the long else arm (a memory
// variable's loads combined per region); ours holds arg_2 in EBX from the prologue. All restored.
// M397: Direct input with fixed absolute-residual producer and double root/signed-root/
// scaled-correction parts, masks 0..7: 804/804/804/810/804/810/810/810. NOT kept: no exact or
// eligible native-frame gain; original input storage, private helpers and full body restored.
// DH381: Direct-input query-admission positive/rejecting prefixes at four ordered boundaries
// 750/750/750/750/750/750/750/750. NOT kept: ties retain frame 0x8c versus 0x88; original body restored.
// DH383: Complete lower-priority/default numeric clamp producers on direct input:
// 834/807/837/834/805/834/834/832. NOT kept: all regress, lose the native frame and shorten away from
// retail. Original input access, clamp and every helper/body restored; early object/input/heading homes
// remain.
// Round 76 (m3 lane 10, fast with 0x1f9e70/0x110ab0; aligned = registers and stack offsets kept, base 590): the held
// lane 12 r91 set rebuilt on this body (0x29b2e0's first vector corrected to the +0x290 pointer, direct input,
// normalize3d, planar copies, the float root, MIN at both clamps, a named blend complement) 748 (3502 bytes, aligned 517);
// with the first steering arm as an if / else-if / else chain without an initial value 714 plain (3500, aligned 506),
// its positive spelling 714; that arm as the direct boolean expression 761 (3499, aligned 509), as one if/else on the
// combined condition 761. NOT kept: the chain's gain does not hold in the direct form. Leave-one-out on 714: no planar
// copies 738, no float root 748, no MIN 826, no complement 716, no corrected argument 787, no normalize3d 841, the input
// reference kept 715 (3499). Float radial squares 771 on 748, 731 on 714. The private normalizer in normalize3d's exit
// shape with float squares i, k, j 717, with its early return 800, through vector3f_scale 712, written j, k, i 714/719
// (the squares are reassociated; none gives retail's i, k, j). Retail evidence for the set: each clamp divides twice, the
// root is a float expression (`fsubr dword [1.0f]`), `1 - field_64` is computed once, and the first steering arm falls
// through to `mov al, 1` with its false paths in one shared `xor al, al` block. Left: EBX saved in the prologue (retail
// pushes it after the 0x26c test and pops it before the closing arg_7 test) and frame homes 4 bytes off.
// DH385: Direct-input complete initial heading/default/normalizer memmove producers,
// seven subsets then native history transfer: 849/840/822/834/835/835/836/799. NOT kept:
// no gain or native frame/entry recovery; original input storage, helper arithmetic,
// vector transfers and complete body restored. Early object/input/heading homes remain.
// Lane 5 r164 (plain; aligned/register-masked LCS [394/232] for 715): the input copy's length through the existing
// normalize_inline (retail sums i, k, j; ours k, j, i either way) 807; the +0x290 address written at each use (no local_4)
// 875; local_15 and local_17 both as planar vector2f copies 723 (3466 bytes [380/226]); the clamp helper as an if/else-if on
// its parameter 795; field_64 read once into a named local for the three blend rows (retail keeps it in XMM6) 731. The
// region from local_4 to the end of the if-chain in its own block (diagnostic) 715, the same code. Lead: matched 0x4d640 and
// 0x9eac0 save EBX around plain sequential statements, so the region's form is not what keeps our push at entry; ours uses
// EBX only inside that region too (same live range and jumps), and the head/tail esp shifts cost about 70 instructions.
// Lane 12 r102 (plain; the fast image 648 carries the builds-differently caveat): both planar inputs as aggregate copies
// (`*(vector2f const *)arg_2`, `*(vector2f const *)local_4`) 723 (3466 bytes; both copies then take retail's integer
// moves, the shifts cost more), local_17's alone 723, local_15's alone 717; with normalize_inline for local_9 824;
// local_9 renamed `direction` 715 (identical). EBX: our tail after the arg_7 test uses no EBX either, so retail's late
// push/early pop has no visible source cause yet.
// m3 lane 7 r146 (plain unless marked fast; fast builds this function differently, control 648): retail saves EBX only from
// the `lea ebx, [ebp + 0x290]` join to the closing *arg_7 test; ours uses EBX in exactly the same lines but saves it at entry, and
// several frame slots (local_6, local_8, local_9) also sit 4 or 0xc bytes apart. The middle region inside `do { } while (false)` 715
// (same code); the +0x290 pointer read at each use (no local_4) 875 (3569 bytes); point local_8 declared with the opening locals
// 648 fast (same code); local_7 declared before local_6 651 fast; local_9 declared before local_8 648 fast (same code).
// Diagnostics: the disabled caller 0x1f38c0 (and 0x1f6090) enabled 715 (same code, so the missing caller is not the cause); the
// 0x26c conditional call removed 848 (EBX still saved at entry). A retail scan finds 24 matched functions with a mid-function EBX
// save and no path around it (0x1cbad0, 0x17c040, 0x40e90: EBX used only for a call's register argument, a byte test, or ahead
// of tail calls); none of their shapes is a natural rewrite here.
// Round 80 (m3 lane 13; all plain, because the fast image hides the cause below; EBX save given as the listing index
// of push/pop, retail 45/811): the entry EBX save comes from the named `double local_27` (the radial distance). The
// fast image already saves EBX late as retail (fast 648 differs from plain only in that save and the esp offsets it
// moves). In the plain build that double as a `real` (diagnostic) 750 at frame 0x8c gives retail's 45/808; 0x1f9e70's
// `double dot` as a `real` 715 (EBX still at entry; 0x1f9e70 137 -> 302); both 750. The same double pads 0x1f38c0's
// callers when that body is enabled (#581, round 78). Leave-one-out of each kept form, EBX still at entry in each:
// reference input 715, private normalizer 818, clamp temps for MIN 847, double root 775, by-value scale 771, the
// 0x1f9760 flags swapped back 716; all of them together (lane 12 r91's corrected-argument step) 848 at 3464 bytes,
// EBX still at entry, so lane 12's mid-function save was a fast result. Retail tests the radial distance once (fsqrt,
// the gap loaded after it, one fcompi, each arm its own exits): as one test `gap > d ? mode != 2 || !flag : !flag &&
// local_18 > local_16` with the named double (one use) 805 (EBX at entry); with the square root in the test (no named
// double) 778 (3460 bytes, frame 0x8c, EBX 45/803 as retail); that with a named blend complement 780, with
// local_15/local_17 as planar aggregate copies 789, with both 791; `field_50 * 0x1f9e70(...)` 715 and on the
// square-root form 778 (same code each; retail loads field_50 before the normalize2d call and keeps it in a slot).
// Next: the double-free forms add a slot (frame 0x8c; retail's lowest slot, shared by the steering scale, the root
// and the ratio, moves to +0x30); find it. Body unchanged at 715.
// M3 lane 8 r151 (plain each; fast 648 builds differently): `real perp = -local_17.j;` at its three uses on this base 775
// (3488 bytes); the forward dot `arg_2->i * local_17.i + arg_2->j * local_17.j` 715 (same code); both 775; the last dot
// as `local_26.i * -local_17.j + local_26.j * local_17.i` 715 (same code); the side flip as one comparison,
// `local_21 = (... > 0.0f) == local_22;`, 721 (3484 bytes). Lead on the EBX save: matched 0x17c040 shows the same
// mid-function `push ebx ... pop ebx` as a short borrow of a callee-saved register no web holds (its byte test needs BL
// with EAX, ECX and EDX busy), as 0xa5a60 borrows EBP. So retail never gave EBX to a web in the main allocation here
// and borrowed it for the +0x290 pointer; ours allocates EBX in the main pass. That is register pressure (retail
// reloads the definition pointer into ESI per region and re-reads arg_0 from its slot), not block structure.
// m3 lane 14 r84 (plain each, with 0x29b990, 0x1f8510, 0x1f3e40, 0xba1d0, 0x1e4290, 0xb9dd0, 0x1f5f30, 0x1f9e70,
// 0x29b2e0 and 0x29ab90, all unchanged; aligned/register-masked/register+esp-masked rows, base 583/341/174): re-measured
// lane 10 r93's trial 2 (the implied `!(local_23 > local_27) &&` dropped, the root written in its test) 776 (3464 B). On
// it: local_14 computed in the arm that uses it, just before local_25, 770 (3446 B, 531/310/178; frame 0x88 again and
// EBX pushed where retail pushes it); local_14 folded into local_26's two components 770 (3453 B). On the 715 body:
// local_14 computed in its arm 694 (3458 B, 510/323/261, EBX still saved at entry; match-only, a declaration moved, and
// retail computes local_14 early, so the gain is slot packing, not retail's order); local_14 folded into local_26 746
// (3467 B); with local_14 in its arm, local_19 computed at its one use 786 (3508 B). Retail copies local_15 with integer
// moves and lays the local_21 chain out differently; those forms are listed.
// Later, on the 694 form (local_14 in its arm): local_14 after local_25 760 (3464 B); local_20 declared just before the
// side-test result's `local_5 = *arg_2;` 708 (3455 B); local_25 folded into local_26 655 (3464 B, 588/327/268; match-only:
// both the moved declaration and the folded point). On 655: local_24 folded too 785, with local_20 declared late 669, with
// the root in its test (lane 10's base) 771 (3450 B, frame 0x88, 535/314/175), local_23 read before the side test 790.
// local_25 folded alone on the 715 body 718 (3499 B, retail's length, 548/352/184).
// Round 93 (m3 lane 10, plain; 0x29b2e0 176, 0x29b990 MATCH and 0x1f9e70 137 unchanged; al/masked = aligned difflib
// counts, base 583/174): the second radial test's implied `!(local_23 > local_27) &&` dropped (a failed first test with the
// radius larger already makes `!flag` false), double kept, 805 (3471 B); with the root written in the test 776 (3464 B,
// al 571, masked 162: EBX saved after the 0x26c test as retail, one fcompi and the flag test threaded as retail, but frame
// 0x8c); on that form, listed forms re-measured: float squares 781 (3458 B: retail's `fld [x]; fmul [x]` squares), with
// the distance on the left 781 (same code); `real perp = -local_17.j;` for its three uses 778 (3462 B, al 545); the
// existing magnitude2d(&local_26) 780 (3462 B). Re-measured on the 776 form: local_15 as a planar aggregate copy 781 (3456 B), with
// local_17's too 787 (3448 B); frame still 0x8c. There the extra slot is local_14 at +0x20 beside local_3's byte (+0x1f),
// where retail keeps local_14 at +0x7c and local_3 at +0x27. A no-LTCG compile of the file gives frame 0x90.
// m3 lane 14 r89 (plain; control 715, aligned 583, register/stack-masked 174): the three `-local_17.j` uses written
// `(0.0f - local_17.j)`, the decal plane flip's negation, 782 (3508 B, frame 0x90; aligned 539, masked 171); one
// `real perpendicular = 0.0f - local_17.j;` after the heading's normalize for the three uses 774 (3488 B, masked 271;
// unary minus and `0 - x` compile differently, so this is not the listed perp form); the side flip as `local_21 =
// local_22 ? local_21 : !local_21;` 715 (same code), as `(dot > 0.0f) != !local_22` 765 (3499 B, masked 184), as
// `local_21 ^= !local_22;` 725 (3480 B). Retail ends that arm with `test cl, cl; jne; test al, al; sete al; test al,
// al; setne al`: the flipped value is converted to bool once more at the join, so its type there is not bool.
// Round 81 (m3 lane 15; fast with 0x1f9e70, 0x110ab0, 0x29b990 and 0x29b2e0 listed gives the plain build's code on this base: 715,
// EBX pushed at entry; aligned 394, stack-masked 279, stack-and-register-masked 170). New: retail's side arm ends with `test al, al;
// setne al` after the optional `sete` (0x29bf27), so the flag there is a byte-sized value that is not a bool, converted to bool at
// the join. Written `byte side = cross > 0.0f; if (!local_22) side = !side; local_21 = side != 0;` it gives exactly those two
// instructions (3487 bytes, stack-masked 277) but 772, everything behind sitting two lines later; `char` the same code; `long` 765
// at 3499 bytes (dword tests, not retail's); the bool compared with false 772. The grid's singles re-measured on this base:
// local_15 as a planar copy 717, local_17 723, the negated j named 774 (`-x` and `0.0f - x` give the same code), float squares
// 780, one radial test with the root in it 778 (3460 bytes, frame 0x8c, EBX pushed at retail's place), that with float squares
// 789. Combined: side byte + named negated j + one radial test + float squares + both planar copies 755 (3468 bytes, frame 0x8c,
// EBX as retail, masked 163); that with local_14 computed in its arm 790 (3431 bytes, frame 0x88, EBX as retail, aligned 365); one
// radial test + float squares + local_14 in its arm 783; one radial test + local_14 in its arm 778 (3442 bytes); side byte + named
// negated j + one radial test + float squares + local_14 in its arm 775 (3444 bytes). Retail keeps local_14 as one 8-byte home at
// the top of the frame (stored ahead of the two normalizes, read once in the far arm); the forms without the named double split it
// into two 4-byte slots low in the frame here, which is their extra slot (frame 0x8c). Body unchanged.
// m3 lane 2 r81 (plain each, with 0x1f9e70, 0x29b2e0 and 0x29b990; fast 648 builds differently; masked = aligned count with
// registers and esp offsets masked, base 340). Kept at 715: the private k/j/i dot helper replaced by the existing dot3f
// (same code), the hand-written cross product by the existing cross3f (715, masked 337), the two float dots by the existing
// dot_product2d (same code each). Trials: the planar copy for 0x1f9e70 as `*(vector2f const *)&local_25` 766 (3470 B,
// masked 326; retail copies it with integer moves); field_50 read into the scale before that planar normalize, then
// `scale *= function_1f9e70(...)` 759 (3499 B: retail's spilled scale, `setne cl` and push order there); both 742 (masked
// 323). In the 0x47c arm: the squared-speed floor as MAX(local_16 * local_16, 0.001f) 713 (retail's `jbe` floor shape);
// on it `((1.0f - dot) * 0.5f) * ...` 759 (3499 B, retail's separate 0.5 multiply), `(half * half) / floor` 721 (3479 B,
// retail's square-then-divide in retail's registers), both 730; the dot floor as MAX too 709 (masked 333, but retail keeps
// the if form there: `ja; movaps`), the later `local_24` floor as MAX (same code). On 709: the side offset as an assignment
// and an if 728 (3480 B). Same code: the 0.99 dot's products and operands written mirrored, local_17 declared before
// local_15 (VC orders those operands itself). Left: retail re-reads arg_0 from its slot and holds local_2 and then the
// saved vector's first component in ESI, where ours keeps arg_0 in ESI throughout; EBX is still saved at entry.
// P481: Complete established/radial decision byte/char/word/long, original true
// default then fully assigned leaves: 648/690/648/690/722/721/724/723 (fast control
// 648, full 715). NOT kept: no native entry/home equality or gain; original full
// Boolean decision, radial double, helpers and complete body restored.
// m3 lane 15 r84 (plain each with 0x29b990 MATCH, 0x29b2e0 176 and 0x1f9e70 137 unchanged; control 715; fast gives 648 here):
// local_6 += local_7 through the existing vector3f_add 757 (3478 B). Same code as the control (not counted): local_26 through
// the existing vector2d_from_points2d; the side test and the flip test through the existing cross_product2d, each and both;
// `__assume(local_2 != NULL)` in the steering arm; the output scale through the existing vector3f_scale; `local_27 =
// MIN(local_27, local_20)`. Retail computes `0 - local_17.j` once (xorps/subss into XMM3) and reuses it in the side test
// and local_25, where ours folds each negation into a subtraction (the named negation forms are listed above). Body unchanged.
// m3 lane 16 r119 (plain each with 0x29b2e0 176, 0x29b990 MATCH and 0x1f9e70 137 unchanged; fast gives 648 here): on the 715
// body both clamps as `MIN(1.0f, ratio)` 715 (retail's `1.0f > ratio` branch shape, count unchanged); the 0.85 dot as
// dot3f(&local_7, &local_9) 715 (same code). Lane 10 r93's form re-measured 778 (3462 B); on it `real perpendicular = 0.0f
// - local_17.j;` 764 (3468 B), with `MIN(1.0f, ratio)` 767; local_26 through vector2d_from_points2d 764 (same code); with
// both planar copies 702 (kept). On 702: float squares in the root 756 (3467 B); `MIN(1.0f, ratio)` at both clamps 702;
// the 0x654 arm with `else local_21 = true;` and no initial value 746 (3471 B); the squared-speed floor as
// `MAX(local_16 * local_16, 0.001f)` 704; a named `1 - field_64` complement for the three blend rows 702 (operands move).
// P512: Actual collision-query blocked/clear result in bool/byte/short/long, on the double-free
// 702 graph: eight 702 at 3473/3499 bytes. NOT kept: one-use receivers do not recover frame
// 0x88 or shared query/planar-point home; original complete executable source restored.
// Round 183 (m3 lane 6, fast with 0x1f38c0, now enabled, and 0x1f9e70, which reads 702 as the full build; all
// diagnostics, restored): 0x1f9760's two outputs in their own block with the result in a bool 702 (frame stays 0x8c);
// local_14 declared in the arm that uses it 779 (3437 bytes, frame 0x88 as retail); local_5, local_2, local_6, local_1
// declared in retail's slot order 702 (same code: local_1/local_2 are spill homes); local_1/local_2 renamed object/
// definition 702 (same code); local_3 declared first 703 (it then takes retail's home, frame +0x27, but its store
// moves to the top).
// P514: Independent configuration/query-vector/planar-point/position/response/correction/
// persistent-collision/normalized-input lexical identities on the double-free 702 graph:
// eight 702 at 3473 bytes. NOT kept: frame remains 0x8c versus 0x88; original body restored.
// P517: Completed collision-query output scope ending before either branch, carrying blocked/clear
// bool/byte/short/long results; 702/702/702/702/702/702/702/702.
// NOT kept: no exact or qualifying native-shape gain; original executable body restored.
// P529: Actual three-component target/side-point/radial-displacement storage, seven
// subsets then the complete unused spatial coordinate: 715/606/578/737/745/729/741/717.
// NOT kept: 578 recovers native frame/length but retail stores and consumes only the two
// radial components; unsupported workspace width needs equality. Original full body restored.
// P531: Current double-free target/side-point/radial-workspace member producers, seven
// subsets then the shared difference producer: eight 702 at 3473 bytes.
// NOT kept: frame remains 0x8c versus 0x88; native query/planar-point home sharing
// does not recover. Original complete executable source restored.
// P534: Immutable complete planar target/side-point/radial-displacement producers, seven subsets,
// then all with immutable pre-blend snapshot: eight 702 at 3473 bytes. NOT kept: frame remains
// 0x8c versus 0x88; native query/target home sharing does not recover. Original full body restored.
// P548: Independent configuration, normalized-input/magnitude, heading/default, planar-input,
// scalar-control, radial-workspace, pre-blend and velocity-residual declaration phases:
// eight 679 at 3479 bytes. NOT kept: no native spill/home recovery on the shared query/target
// storage graph. Every initializer and complete original executable source restored.
// P550: Complete final latch outcome in bool/byte/short/long 679/679/679/679; backward transfers
// at heading/paired defaults/normalized input/pre-blend snapshot 779/818/726/770. NOT kept:
// no qualifying native spill/home repair; original full body and headers restored.
// Configuration, query/position/target homes and normalization/steering allocation remain.
// P552: Uninitialized component transfers at heading, paired defaults, normalized input, history,
// working directions, saved steering, final outputs and the complete graph.
// Fast scores 779/759/726/810/724/770/743/833. NOT kept: no permitted confirmed gain; original body restored.
// P554: Scalar correction in-place pointer/reference, separate pointer/reference,
// returned output pointer/reference and wider already-rounded return: seven 679s;
// complete residual vector with a plain correction loop 795. NOT kept: no native
// spill/home repair; original helper, all three correction calls and full body restored.
// Round 129 (m3 lane 9, plain each; aligned/masked = difflib counts on the listing, masked = registers and esp offsets
// masked). Kept, 679 -> 489 at 3503 bytes, frame 0x88: the 0x1f9e70 planar copy as `*(vector2f const *)&local_25`
// (retail's integer moves) with the scale read from field_50 into local_27 before the call and `local_27 *= ...` (retail
// loads field_50 ahead of the call) 637; the first steering arm as the direct expression (no initial true; retail's
// false paths share one `xor al, al`) 615 (3499 B); the three velocity dots as one vector3f frame_velocity ahead of the
// reciprocal (retail divides after the dots) 540; the speed floor as MAX, the half span squared then divided in two
// statements and the turn factor as `(1 - dot) * 0.5f`, `*= local_26 * 0.5f`, `*= 4.0f` (one expression folds 0.5 * 4.0
// into 2.0 and divides first) 527; field_64 read once into `blend` for the three rows plus float squares in the radial
// root (retail's `fld [x]; fmul [x]`; either alone 623/662) 496; the first arm's dot without the double casts 488.
// Self-review: both clamps as MIN(1.0f, ratio) (retail compares 1.0 > ratio) gives 488 against 489 for MIN(ratio, 1.0f),
// restored to the latter; the field_50 read placed before the planar normalize 489 (same as after it, so after it).
// Tried, not kept: MIN(1.0f, ratio) alone 679 (aligned 463 -> 458); with both floors as MAX 723; with the speed floor as
// MAX 681; the halves as `/ 2.0f` 538 (same code as `* 0.5f`); the square/divide split without the turn-factor statements
// 669; the dot floor as MAX (retail keeps the if form); the first arm's dot through dot_product2d 660; the three dots as
// one-use named locals 589; the up vector (object +0x7c) at each use in place of local_28, a field lead: 622 on 527, 498
// on 496 (first difference +0x60 -> +0xf9, local_6 then register-allocated as retail); slot locals renamed to forced-order
// names (same code: names do not order these slots). Leads, only keepable with a match (each a reverted form): the side
// offset ternary written in each coordinate gives retail's per-arm y 550 on 615 (561 on 488, frame moves); the flip through
// `byte side` with `local_21 = side != 0` (retail's `test al, al; setne al`) 669; local_6.i/.j assigned before the k branch
// (retail's order; aligned 213 -> 156, masked 57) 676; flip + order 673; all three 708. Left: local_8 at +0x7c with the
// 0x1f9760 output/local_14/local_25 slot at +0x70 (retail the other way round); the normaliser's squares k, j, i (retail
// i, k, j); the post-0x1f8510 block's order; the per-arm y and the flip conversion (ours 6 lines short in the steering block).
// P556: Actual object/configuration/input/query-output/position/response/radial/final-output
// lvalue receivers 679/679/683/679/682/704/693/766. NOT kept: no native spill/home equality;
// original executable body, all producer helpers and both final publications restored.
// P558: Actual collision-query output workspace, six complete flag/flag/vector field orders then both
// flag-pair orders with the vector independent: 730/730/730/730/730/730/713/713. NOT kept: unmatched
// output grouping; original independent outputs, all helpers and entire body restored.
// P560: Actual entry actor/object/settings/query-flag lexical identities on the retained
// shared query/target frame graph, eight behavioral sets: 679/679/679/679/679/679/679/679.
// NOT kept: no native object/settings/flag spill equality; original complete body and helpers restored.
// M3 lane 11 r109 (fast, control 489 as the full build): local_8 renamed body_position (a name sorting ahead of the
// shared 12-byte home's names) 489 (same code: names do not order these two homes either); the side point written in both
// arms of `if (local_22)` (the negated distance named in the else arm) 751 (3538 B, frame 0x8c; the arms are not
// tail-merged, the point gets a stack home); the k clamp as one ternary assignment 489 (same code); the three blend rows
// reading field_64 at each use with the complement product first 663 (3499 B); the velocity-frame cross through the
// existing cross3f_staged 489 (same code). Re-measured: the side ternary written in each coordinate 573 (3500 B: two
// separate ternaries, not retail's per-arm y with the x part after the join); the flip as `== local_22` 658. Body unchanged.
// M3 lane 14 r105 (fast = full 489 here): the normaliser's input copied component by component 765 (3486 B); that copy
// made before the 0xb9dd0 call 500; local_13 declared before local_11 489, local_8 or local_9 declared first in the
// function 489/489, local_10 declared then assigned 489 (all the same code: local_8 stays at +0x8c, the squares k, j, i);
// the 0.99 test's dot through the existing dot_product2d 661 (3499 B). Kept: the output scale as the existing
// vector3f_scale(&local_6, local_27, &local_6) 489 (identical code).
// M3 lane 13 r105 (fast with 0x1f38c0/0x29b990 matched, 0x2922b0/0x29b2e0 unchanged; control 489, register-masked aligned 79).
// The normaliser's square order follows when each component of local_9 is first named: a no-code read names it early, and a
// component named early also takes the scale-copy-first product (retail: squares i, k, j; products copy, load, load). Forms:
// `(void)local_9.j; (void)local_9.k;` before normalize3d 756 (3486 B: the squares become retail's i, k, j but j and k turn
// copy-first and the frame moves); a private normaliser summing `s = i*i; s += k*k; s += j*j` through a squared-length
// helper 646 (3499 B, masked 85: squares and products as retail, other products elsewhere flip); the 0.99 test's dot
// written `local_17.i * local_15.i + local_17.j * local_15.j` 491 (3506 B, masked 74: its i term loads local_15 first as
// retail), only its i term swapped 491 (same code); local_28 declared after local_4 at the top 493.
// Round 614: one query/target vector 489; position as a full vector 489; planar position copied
// at the position call 660, after query failure 602, after target addition 602, and after addition
// with the shared path vector 602; earlier query vector declaration 489; position and both path
// vectors renamed together 489. NOT kept: planar copies reach 3499 bytes in the later forms but
// move the position call away from retail's +0x80. Sharing the path vector leaves it at +0x8c.
// Deep round 410: world position/candidate direction/target names 489; sampled position/trace
// direction/translated target names 489; position before motion outputs 489; query output
// scoped to fallback test 665; target-position name 489; returned position pointer 795; query
// flag at motion-mode test 563; target declared with query direction 489. NOT kept: lexical and
// declaration forms tie at 489. Ending the query workspace at its result reaches the 3499-byte
// retail extent but regresses to 665. The returned point and later query flag also regress. The
// position and target stack homes and the order of the normalizer squares still differ.
// Query/target/pre-turn shared vector 487; target/pre-turn shared vector 489; position
// result copy 679; typed heading and index fields 489; typed actor predicates 489;
// y-first side point 708; signed side branch 489; typed radial difference 665 at retail length.
// NOT kept: two-point gain below threshold, ties or regressions. Position and query homes still differ.
// Round 455: Common fallback with path admission and fallback arm first 753, Common fallback with path
// admission and fallback arm first, query outputs declared before admission 753, Common fallback with path
// admission and movement arm first 753, Common fallback with path admission and movement arm first, query
// outputs declared before admission 753, Common fallback with path bypass and fallback arm first 753,
// Common fallback with path bypass and fallback arm first, query outputs declared before admission 753,
// Common fallback with path bypass and movement arm first 753, Common fallback with path bypass and
// movement arm first, query outputs declared before admission 753. The common fallback reaches 3499 bytes
// with frame 0x8c and shifts the position to +0x90. All scores rise. Restored the original path check and
// fallback branches.
// Planar target with point-difference helper 699; target from the planar
// input 729, with the point-difference helper 745; shared three-component
// input for target and planar copy 729; planar pre-turn snapshot 706;
// snapshot normalized directly 752, with planar target 785, with shared
// planar input and point difference 776. The position and alternate
// vector homes remain interchanged in the retained body.
// The position and query homes are exchanged before the translated target reuses the query slot.
// Pre-turn snapshot as fallback/weighted/both inputs 674/674/682; separate turn result with
// delayed publication 716; planar snapshot before separate turns with immediate/late publication
// 673/714; planar aggregate/component capture before in-place turns 752/765.
// NOT kept: all scores rise. The separate-result forms reach retail length but lose the native alias.
// Translated target through the point setter 489, point aggregate 669, position
// copy plus coordinate updates 718, immutable vector aggregate 669, point
// destination of the vector addition 489, coordinate loop after copying position
// 733, direct coordinate loop 615, and target scoped to side selection 771. The
// aggregates reach retail length while the position still uses the other stack
// home. Original body retained.
// Collision vector supplies the query direction, initialized on the miss before
// location, after location, at collision processing or before the result branch
// 577/589/790/786. Response vector supplies it with fresh/saved defaults
// 848/834. Collision reuse with a late target or the response default 827/786.
// Restore the original collision default into the expired query vector 641.
// The first forms recover the position slot but regress the opening copies.
// Working heading as the ignored query output, with the initial copy at entry, after
// position lookup, after normalization, or on the no-settings path 487/771/790/804.
// With a named query result 487/771/790/804. NOT kept: the entry form restores the
// position's +0x80 home, but changes the separate query destination and gains only two.
// The position and path workspaces remain exchanged. The normalizer's squares also run in a different order.
// Tried position and normalized input scoped before the final latch 489;
// spatial inputs scoped through the final latch 489; position and correction scoped before the final latch 489;
// normalized input scoped through the final latch 489;
// path query as a direct alternative to the bypass branch 489;
// direct path branch with position scoped before the final latch 489;
// direct path branch with correction scoped through the final latch 489;
// direct path branch with normalized input scoped before the final latch 489.
// NOT kept: All eight tie at the original length, with the same first differing stack slot.
// M3 lane 16 r137 (fast; control 489): the input copy written member by member 765 (3486 B); local_7 copied from local_6
// 489 (same code); local_9 declared ahead of local_8 and assigned after the 0xb9dd0 call 489 (same code); local_10
// declared ahead of local_9 489 (same code); local_3 declared just before the input copy 538.
// Input capture before position, or complete normalization before position 500/762.
// Admitted position only, both producers, normalization only, input and
// normalization only, position between input and normalization, or position after
// normalization 750/998/874/890/998/998. NOT kept: the position home remains
// unresolved. Moving both producers under admission also raises the caller score.
// Round 431: planar position pointer/reference before query or after translation 497 each; planar target
// pointer/reference 497 each; radial-only translation before side selection or before radial difference
// 764/771. The position and alternate vector homes remain exchanged. Original body restored.
// Split saved planar components 766, all components 766, point-shaped planar save 762,
// and split capture with member stores 766. Reuse query lateral/both tail components
// 746/729, or translated-target lateral/both 734/734.
// The translated-target forms put the position at +0x80 with frame 0x88, then differ at +0x4e.
// NOT kept: every form regresses and shortens the body.
// M3 lane 5 r200 (fast; control 489, aligned 178, register-masked 79, frame-blind 45): the velocity-frame cross through the
// existing cross3f_staged 489 (same code: its operand sides do not follow the helper's spelling); a `velocity` pointer local
// for the three dots 640 (3499 B); the three dots as named speeds 671 (3487 B); the target point computed just before the
// steering-side block 730 (3463 B); the input copy and its normalize ahead of the position query 762 (3499 B). Diagnostic,
// reverted: 1..7 dead initialised locals 665/665/665/665/669/500/493.
// M3 lane 6 r203 (fast with 0x30bf0; control 489, aligned 178/178): dead-local scan K = 0..31 at the top (diagnostic,
// removed): K = 1-5, 9-13, 17-21, 25-29 move the frame homes (3499 B, 658-669), K = 6-8, 14-16, 30-31 493-500, K = 22-23
// 574 (3509 B), K = 24 484 (aligned 176); the normaliser's squares stay k, j, i at every K, so that order is not a
// symbol-count state. A matched-function scan of retail for the x87 square sums finds i, k, j only in 0x195070 (a vector
// filled by a callee, then normalize_inline); k, j, i (ours) in 0x195370, 0x1b81c0, 0x1f1ee0, 0x23cbb0, 0x25dd50.
// Tried: the input copy normalised through the existing function_30bf0 (retail calls it out of line three times
// here) 874 (3398 B: stays a call); local_26 through the existing vector2d_from_points2d 665 (3499 B, frame moves);
// local_25's components written y first (diagnostic: retail computes y in the select's arms) 708; the vector add's
// operands as (arg_2, local_8) 489 (same code).
// The frame still agrees with the position at +0x8c for retail's +0x80.
// Point-typed target, query and shared query/target 489/489/489. Shared point
// setter 487. Closing alignment products in j-k-i, k-i-j, j-i-k and i-k-j
// order 741/750/747/747. NOT kept: the shared setter's gain is two. The named
// products reach retail's length and one recovers the normalizer square
// order, but the position slot stays different and the scores rise.
// M3 lane 5 r204 (fast with 0x1f9e70 and 0x110ab0; control 489; all restored): the input normalised through a
// file-local inline with the sum written j, k, i and the scale sides inverse * i, j * inverse, k * inverse 763 (3501 B:
// the squares still run k, j, i, so the written order is not what sets them); LEAD: the closing dot written with the
// copy's fields mentioned j, k, i (`local_9.j * local_7.j + local_9.k * local_7.k + local_9.i * local_7.i`) gives
// retail's normaliser squares exactly (copy stores and squares i, k, j) but 756 overall (3486 B, homes move from +0x60,
// the j/k scale sides flip); with local_7's fields first in each product 756; mentioned k, j, i 762; the j, k, i
// mentions with normalize_inline 758 (3508 B). Retail's squares need local_9's fields ranked j, k, i by a direct
// mention, not by the sum's spelling.
// Independent collision vector seeded from the existing response default before/after location,
// after planar normalization or at the collision split, scoped/entry declaration:
// 790/790, 791/791, 790/790, 794/794. NOT kept: collision-split seeding reaches
// 3499 bytes and position +0x80, but drops the early vector copy and shifts the heading spills.
// M3 lane 2 r113 (fast with 0x1f9e70/0x110ab0/0x29b990/0x2922b0/0x29b2e0, each at its baseline; control 489, aligned/masked
// 178/43): declarations of the locals whose addresses reach calls moved to function scope (the lever that gives 0x2922b0
// retail's per-point reloads): local_11 and local_13 489; local_15 and local_17 (assigned where they were) 489; local_14
// 489; all five 489; local_20 and the outer local_23 489; all seven 489 (all the same code: here no store is followed by
// a read through a pointer that the scope decides, so the position/target homes and the squares stay as they are).
// Shared heading fallback, with the no-settings arm or the settings-present arm first. Direct
// fallback test 753/764, named fallback result 762/762, direct steering test 753/764, named steering
// result 753/769. NOT kept: Every shared-call form grows the frame and raises the score.
// The position query still uses ESP+0x8c for retail's ESP+0x80.
// Planar snapshot after position 775, after normalization 751, at query admission 745.
// Planar snapshot before or after the actor update 725/723, after the translated target 723,
// after both planar normalizations 725, and before side selection 720.
// NOT kept: The planar snapshots move the position to +0x68 or +0x58 and disturb the entry spills.
// Shared query and steering direction-validity byte, separate target, direct/named arrival test 489/489.
// same byte with shared query vector and 3D target 487/489.
// same byte with shared query vector and XY target 669/669.
// same byte with the target calculated in the lateral test 773/771.
// Written-differs at 489. NOT kept: Sharing the query vector for the full translated target gains two
// and leaves the position at +0x8c. The two-component target reaches 3499 bytes at 669 and moves the
// first difference earlier. The opening body is restored.
// M3 lane 13 r128 (screened, control 489): local_8 declared first in the function 489; local_14 declared ahead of the
// 0x26c180 call 489; local_9 declared bare and assigned from the input 489; local_9 declared ahead of local_8 and
// assigned after the position query 489 (all the same code: neither moves the position's +0x8c home); local_7
// declared ahead of local_6 495.
// Position remains at +0x8c for retail's +0x80. Both path vectors declared together before
// the heading, before the position, before or after its query, or after normalization 489 each;
// one path vector before the position or before or after its query 489 each.
// Translated target as two scalars, query-vector coordinates, explicit planar writes, or shared setter into the query vector before planar normalization 702/669/546/487.
// The same forms inside side selection 775/775/763/764. NOT kept: the setter gains two and retains the wrong position home.
// Shared planar radial and heading vector with direct/helper subtraction
// and copied/assigned heading 671/727/503/727. The same workspace as a
// point 671/727/503/727. The 671 forms reach retail's 3499 bytes.
// NOT kept: the initial position still goes to +0x8c and every score rises.
// Per-arm Y with common X, positive/negative first: point field 718/718,
// scalar forming point 703/703, scalar consumed by radial difference 720/720,
// radial Y in each arm 694/686. NOT kept: the scalar Y forming a point reaches
// position +0x80 and frame 0x88 but shortens the function to 3483 bytes and raises the score.
// M3 lane 5 r219 (H2M_FAST; control 489): the closing test as dot3f(&local_7, &local_9) 489 (same code: retail's
// local_7-first products there come from operand ranks, not argument order); the side flip held in a byte and converted
// at the join (`local_21 = side != 0`, diagnostic for retail's `test al, al; setne al` after the flip: that pair then
// appears, but the new byte takes a home and every slot below moves) 663 at 3499 B; local_25 assigned y first 708
// (3488 B, frame moves); that with the byte flip 718 (3481 B); y first with local_26 through the existing
// vector2d_from_points2d 721 (3476 B). Body unchanged.
// Branch-owned side Y 720, advancing the planar base 718, selecting full Y 693 or its
// displacement 698, branch/selected radial Y 694/627, shared planar cross test 702,
// and shared planar difference 719. NOT kept: full Y selection recovers the +0x80
// position home and frame 0x88 but shifts the first difference to +0x60 and regresses.
// M3 lane 12 r166 (fast screen; control 489, aligned 156, register-masked 43): the 0x47c arm's dot with its operands
// swapped 489 (same code); `local_24 *= (local_26 * 0.5f);` 503 (3506 B, masked 47: the half product is then formed apart
// as retail, but the 0x47c arm stores its own result and the zero register stays XMM4); the byte side flag re-measured
// 663 (3499 B, slot homes move); the dot and its floor ahead of the speed floor 498 (3500 B); local_25 folded into
// local_26's initializer 711 (3484 B). Body unchanged.
// M3 lane 8 r213 (H2M_FAST with 0x1f9e70/0x110ab0/0x29b990; control 489): the side flip through a `long side` (set from
// the cross test, negated when the side flag is clear, then `local_21 = side != 0`) 627 (3501 B: retail's `test al, al;
// setne al` conversion appears but in 32-bit form, and the new symbol moves the homes below, first difference +0x60),
// with `local_21 = (bool)side` 627 (same code); the side point's y as a second ternary on the side flag (arms written with
// local_23 and -local_23) ahead of x 746 (3524 B, frame moves); local_24 and y set in each arm of an `if (local_22)`, x
// after it 718 (3501 B: the arms tail-merge, frame 0x90); the flip as one ternary `local_22 ? cross > 0 : !(cross > 0)`
// 693 (3525 B). Reading: retail's flip value is a byte converted to bool at the join (byte ops, then `test al, al; setne
// al`), and local_8 sits below the shared local_13/local_14/local_25 home, the order a second local_8.y mention gives.
// Shared i/k/j normalizer with setter, aggregate, coordinate walk, or explicit assignment 772/764/755/761.
// Working-heading query destination on the i/k/j graph, with copy, setter, aggregate, or named
// outcome 755/767/760/755.
// NOT kept: Setter and aggregate with the working-heading query output recover the +0x80 position
// and postpone the first mismatch to +0x123. All eight forms raise the score.
// M3 lane 4 r241 (plain check: the reduced image scores this body 654; control 489, aligned 178): retail squares i, k, j and
// scales `inv * i`, `j * inv`, `k * inv`, the shape of src/projectiles.cpp's file-local normalize (`j*j + k*k + i*i`). Compile-
// only grid of three normalizers by eight ways to fill local_9: only the aggregate copy gives retail's scale, and with it every
// square-sum spelling starts at k (field ranks k > j > i); a field-by-field fill in the order j, k, i gives retail's squares
// but `inv * j` and an in-place `inv * k`. Plain: normalize_inline 761 (3502 B); squares written j, k, i through
// vector3f_scale (diagnostic) 741 (3499 B, aligned 174); a file-local copy of the projectiles normalize 763 (3501 B), with
// local_9 filled j, k, i 585; local_11/local_13 declared at the top 489 (same code). Body unchanged.
// M3 lane 12 r170 (plain each; control 489): the side distance as `local_24 = local_23; if (!local_22) local_24 =
// -local_24;` 648 (3501 B); the side point as a planar copy of local_8 with `+=` of each offset 701 (3504 B, the frame
// moves); the side distance read from field_20 at each use with no local_23 727 (3480 B); the flip as `local_21 = (cross >
// 0.0f) == local_22;` 658; `bool local_21 = false;` at its declaration 489 (same code). Body unchanged.
// Low-priority position name 654, target as radial difference by fields/helper
// 718/718, and shared query-target overwritten by fields/helper 714/714.
// Side point as radial difference by fields/helper, or with shared query target,
// 656 each. NOT kept: the reduced control scores 654 against 489 full, and the
// 3499-byte forms change the frame or early stack stores.
// Returned position for path query, side coordinates, or steering query 752/789/773.
// Path query with target translation, side coordinates, or steering query 818/767/784.
// Side coordinates and steering 782, or all fallback geometry 695. NOT kept: the last
// form shortens to 3479 bytes and grows the frame to 0x8c. The frame-preserving forms
// move the first difference into the entry stores and raise the score.
// Scaled side translation with aggregate axis, vector or point output, with direct or shared
// planar difference 656/656/656/656. Axis through setter at the same uses 654/654/654/654.
// Reduced control 654; full score 489. NOT kept: the length-exact forms move the first difference
// earlier, and the setter forms leave the position slot displaced.
// M3 lane 8 r220 (plain each: the reduced image still builds this body differently, 654 with or without 0x30bf0,
// 0x1f38c0 and 0x29b990 listed; control 489): the side distance as `if (local_22) local_24 = local_23; else local_24 =
// -local_23;` 489 (retail duplicates the y row's product and add into both arms of that select, ours computes it after
// the join); the sum point written into the 0x1f9760 output vector (one 12-byte local for both, local_14 dropped) 489
// (same homes: local_8 stays at +0x8c); local_9 filled field by field i, k, j 765 (3486 B: squares j, k, i), j, k, i 767
// (3481 B: retail's i, k, j squares but its loads and the frame move); `perpendicular` declared after local_18-local_20
// 501. Body unchanged.
// M3 lane 11 r151 (plain each; the reduced image still gives 654 with 0x30bf0, 0x1f38c0, 0x29b990 and 0x2922b0 listed;
// control 489): local_14's add with arg_2 first 489 (same code); local_19 with the compare sense flipped (`field_14 > 0.0f
// ? field_14 : unknown03c`) 701 (3499 B, first difference +0x4e); the 0.99 test as `>= 0.99` 679 (3505 B); the side
// test's products written vector first 489 (same code); local_25 assigned member by member 489 (same code). Body unchanged.
// Complete/planar target after planar input normalization 669/701, after heading
// normalization 744/744, at the steering test 730/730, and after the side point with
// shared planar difference 781/781 (reduced control 654). NOT kept: the forms
// that restore position at +0x80 shorten the body and leave the initial square order.
// Frame 0x88 agrees; the position at +0x8c and path-output home are still exchanged.
// Separate last unit velocity, steering motion, and rotated motion, singly and together:
// velocity 672, steering 738, both 752, rotated 715, velocity/rotated 722, steering/rotated 749,
// all 740, all with shared collision addition 764. Reduced control 654, full score 489.
// NOT kept: the length-exact velocity form moves the first difference earlier; the rest grow.
// M3 lane 14 r145 (plain check each, with 0x30bf0, 0x1f9760, 0x29b990 matched and 0x2922b0 521 throughout; control 489; body
// unchanged). With general registers, XMM registers and ESP offsets masked the aligned difference is 77 of 844 rows (125 with the
// offsets kept): the structure is close and most of the 489 is the exchanged position/path-output homes. What the 77 rows are: the
// side point's y computed inside both arms of the side select in retail (ours once after it) with the planar difference's i squared
// first; a `test al, al; setne al` after the first 0x2922b0 call; the half-angle block's 0.5 products; the stopping block's zero
// kept in XMM1 (ours compares with the pooled zero). Tried: the call's result compared `!= false` 667 (3508 B, masked 109: the
// conversion appears and the homes move); the side select as an assignment and an `if` 648 (masked 105); the side point as two
// scalars with y first 688 (3499 B, masked 103); diagnostics, removed: one dead initialised local at the top 665 (3499 B, masked
// 103), one after the position query 665 (masked 101), two at the top 665 (masked 99). Every added symbol moves the frame homes
// and the float product sides together (masked 99 to 109), so a form has to be judged over the count phases, which costs a plain
// check per phase here (the reduced image gives another body).
// M3 lane 15 r132 (plain each; the reduced image still reads 654; control 489): retail 0x29be20-0x29be5f selects the side
// distance into XMM1 with `0.0 - d` in the else arm and computes the y row (`* local_17.i + local_8.y`) inside each arm,
// the x row after the join; ours computes both rows after the join. Forms: the select written in each coordinate (no
// local_24) 589 (3512 B, first +0x4e; the 384 measured on an older base no longer holds); local_14 declared beside local_9
// 489 (same code); both 589; the else arm as `0.0f - local_23` 489 (same code); local_14 typed point3f 489 (same code).
// Body unchanged.
// M3 lane 2 r139 (plain each; control 489, aligned 178, slot-masked 155): the side point built as two statements with the
// y row first (retail's order) 708 (3488 B, first +0x4e), with the select written into each row 722 (3514 B), with the
// select as an if/else 708; local_26 formed straight from the rows (no local_25) 711 (3484 B); the perpendicular as a
// 2-D vector `{ 0.0f - local_17.j, local_17.i }` with both side tests through dot_product2d 661 (3499 B, aligned 235).
// Body unchanged.
// M3 lane 6 r234 (plain each; control 489, aligned-equal 799 of 844 masked rows): the blend rows reading field_64 at
// each use with no `blend` local (retail's complement reads the field from memory while XMM6 holds it) 572 (3502 B);
// that with both clamps as `MIN(1.0f, ratio)` 571; the side point with the y row inside each arm of an if/else select
// and the x row after the join (retail 0x29be20-0x29be5f's shape) 718 (3501 B, first +0x2); the 0x47c override read
// into a named local used for the test and the store 665 (3499 B, first +0x60); the turn factor's dot as
// `dot_product2d(&local_17, &local_15)` 489 (same code). Body unchanged.
// M3 lane 15 r139: the h2m fast screen reproduces the full check (489 at 3503 B, 0x29b2e0 130, 0x29b990 MATCH) once
// 0x29b2e0, 0x29b990, 0x170d70 (the file of the rotation 0x29b2e0 expands), 0x11ce20, 0x11cc90 and 0x1e5450 are listed with
// 0x1f9e70, 0x110ab0 and 0x2922b0; without 0x170d70 it builds 0x29b2e0 at 1309 B and this body at 654. Call list and order
// agree with retail (23 calls). Control 489, aligned 224. Forms: the first 0x2922b0 result read through a `bool const &`
// 665 (3499 B, aligned 305: the new symbol moves the homes from +0x60); the side decision cleared to false at the top of
// the steering arm 489 and initialised false at its declaration 489 (same code each); the planar distance read through
// a `real const &` at the field_44 test 665 (3499 B); the blend field read at each of its six uses (no local) 572
// (3502 B). Body unchanged.
// Location still uses ESP+0x8c instead of +0x80. Native planar points for input/actor heading/
// both/pre-turn/input and pre-turn/actor and pre-turn/all seven forms score 654 in the reduced image.
// All three points initialized from their two source fields 693. Reduced control 654, full 489.
// NOT kept: the view forms tie and the coordinate initializers grow the frame.
// M3 lane 3 r304 (h2m fast with lane 15 r139's companion list, which reads the full check's 489; control 489, aligned
// 224, register-masked 125; body unchanged). Frame reading: this 0x88 frame is ordered by memory references per byte. The
// position's home has 7 references and the home shared by the translated target, the second side distance and the path
// output has 8, so the position sits above it; retail's second read of the position's y (the y row inside both arms of
// the side select) is the one reference that puts the position below it, at +0x80. Forms: the side point through the
// existing 2-D point setter 775 (3560 B: the setter's file is outside the reduced image, so the call stays out of line;
// not a measure); the side point's y member assigned ahead of its x 708 (3488 B); one, two and three redundant pairs of
// parentheses in the side point's rows 499/497/505 (aligned 234/230/238: main already sits on the best count phase); the
// rows written product first, with either factor order, 489 (same code each); diagnostic: the y row written in each arm
// of the side select 718 (3501 B: the compiler merges the two rows again, so retail's per-arm row is not two written rows).
// With the setter's file (0x22bbca) added to the screen's list: the side point through the existing 2-D point setter,
// declared here, 487 (3503 B, aligned 222, register-masked 123: a gain of 2, under the bar, homes unchanged); the setter
// with the signed side distance written in both arguments (no local) 721 (3476 B).
// M3 lane 15 r143 (fast screens with lane 15 r139's companion list; control 489 at 3503 B, aligned 202, 44 hunks left
// once registers, stack offsets and addresses are masked; body unchanged). The count is two things at once: the frame's
// home order, which any change to the set of locals moves (1, 2 or 3 unused locals at the top, diagnostic, 665 each at
// retail's 3499 B with the first difference at +0x60), and about forty small structure differences that stay the same
// in both frame states. Structure forms that add no local: both ratio clamps written `MIN(1.0f, ratio)`, which is
// retail's `1.0 > ratio` compare with the ratio evaluated again in the taken arm, 488 (41 hunks), the first only 488,
// the second only 489; with those, the half product of the squared ratio taken as its own statement ahead of the dot
// clamp (`local_26 *= 0.5f;` after the divide, then `local_24 *= local_26;`) 502 (3506 B, 35 hunks: the half-angle
// block is then retail's instruction for instruction but for register names, the square in place and the half
// product between the compare and the jump), with the statement between the dot and its clamp 502 (39 hunks); the
// scale factor read from field_50 ahead of the planar normalize (retail loads and stores it before the 0x2b400 call)
// 489 on this body, 488 with the clamps, 502 with both (35 hunks; patch build/r143/lead-0x29bab0-structure.patch).
// Forms that add a local: the side flag as a byte converted at the join (retail's `test al, al; setne al`) 663 with 0
// to 3 unused locals, with both clamps 663, on the 35-hunk form 650 (3499 B, 50 hunks). Left after the 35-hunk form:
// the side point's rows, the byte conversion, the scale factor's load order, the zero held in XMM1 through the stop
// block, the five products of the frame-velocity block (retail keeps the object's axis on the left except its first
// member) and the blend tail. Nothing kept: the two forms that lower the count gain one point.
// M3 lane 6 r242 (fast screens with lane 15 r139's companion list plus 0x22bbca; control 489 at 3503 B, aligned 178, 158 with
// stack offsets masked, 45 with registers masked too; body unchanged). Frame reading: the position's 12-byte home has 7 packing
// references and the home shared by the path output, the translated target and the pre-turn copy has 8, so the position sorts
// above it; retail reads the position's y twice (0x29be33-0x29be56: `A = r; y = r * h + py` in one arm, `t = 0 - r; A = t;
// y = t * h + py` in the other, the x row after the join from A), which is the one count that puts it below. Both arms use
// the selected value itself, so retail's row was cloned into the arms after the select, not written on A. The normaliser:
// our squares are leaf * leaf on the copy's three forwarded members (ranks i < inverse < j < k, hence k, j, i); retail's
// i, k, j with `inverse * i`, `j * inverse`, `k * inverse` cannot come from one numbering of those members.
// Forms (score at bytes; aligned / stack-masked / register-masked): the y row inside the select's arms with a second select
// for x (`side ? r * h + py : -r * h + py`, then `(side ? r : -r) * n + px`) 718 at 3533 B (334/197/67; the second select is
// threaded into the first, but the else arm folds to `py - r * h` and forms `0 - r` again); the signed distance as
// `r * (side ? 1.0f : -1.0f)` 712 at 3488 B (314/171/51); both rows written in each arm with `r` and `-r`, y first or x
// first, 750 at 3539 B each (371/191/70: the position then sits below the shared home, nothing is merged); an assignment
// inside the y row's select with x after it (diagnostic) 708 at 3488 B (309/171/49). Re-measured to read their code: two
// selects with the y row first 722 at 3514 B (356/176/57; not threaded, the y row sits between them), the y row in both arms
// with different operands 718 (351/175/55; the rows are merged after the join again), the side point as two scalars y first
// 688 at 3499 B (263/182/64), the copy through vector3f_assign 665 at 3499 B (249/185/71; squares still k, j, i). A scan of
// every game function for a float select whose arms both end in the same product-and-add row finds only this one.
// GA4 (fast with lane 6 r242's companion list, kept confirmed plain with caller 0x1f38c0 MATCH; control 489): kept the blend tail
// through existing vector3f_scale + vector3f_scale + vector3f_add (`vector3f previous; vector3f_scale(&local_6, 1.0f -
// local_2->field_64, &local_6); vector3f_scale((vector3f const *)(local_0 + 0x678), local_2->field_64, &previous);
// vector3f_add(&previous, &local_6, &local_6);`) 489 -> 462 (-27, 3504/3499 B: matches all 49 blend-and-clamp instructions
// byte for byte, including retail's 5-byte `subss xmm0, [esi + 0x64]` and `xmm7`/`xmm6`/`xmm3`/`xmm1`/`xmm2`). Also tried (28
// trials): blend as scale + add_scaled 485 (prev first 491, tmp 486, copy diag 626), two scales prev-first + add 483, scale +
// scalar adds 572, vector3f_add + blend helpers 663 (3499 B); frame_velocity without local_28 pointer 487 (with swapped dots +
// cross3f_staged 491); side select if/else with `real neg = -local_23` (y/x 751, x/y 751, scalar y/x 738, x/y 738, point2f_set
// both arms 751, helper both arms 731 / ternary 721, direct local_26 both arms 593); inline bool flip helper 665; combined clean
// structure with mode-2 early i/j + MIN(1.0f, ratio) + field_50 early + no local_28 + blend helpers 673-675 (3475-3478 B,
// aligned 112, reg+esp-masked 43: all 53 mode-2/scale instructions match byte for byte), without mode-2 early 466-493.
// Incoming/radial/both planar cross tests 669/703/703; planar normal in both dots 665.
// Planar scale to a separate or in-place offset 706/714; one normal for dots and scale 720;
// both crosses with a scaled offset 700. NOT kept: all keep the position at ESP+0x8c and
// raise the score. The position and the path query outputs still occupy exchanged homes.
// GA6 (21 trials on 462 base, fast calibrated 462 at 3504 B with 0x1f38c0/0x29b990 MATCH; body unchanged): closing dot3f(&local_7, &local_9)
// 462 (aligned 147, masked 86); + dot3f(local_4, &local_1->velocity) 460 (with cross3f_staged 460, without local_28 465); clean mid
// (dot79 + MIN(1.0f, ratio) both clamps + local_26 *= 0.5f + field_50 early) 475 (aligned 131, masked 62); + mode-2 early i/j 673
// (3478 B, aligned 71, masked 34; without local_28 673 aligned 76); + byte local_21 with `local_21 = local_21 != 0;` matches retail's
// `test al, al; setne al` at 0x29bf27 with frame 0x88 intact (670 alone at 3509 B; 670 on the clean-mid+mode-2 base at 3483 B, aligned
// 71, masked 32); `byte side` in else arm on that base 674; `(bool)(!local_22 ? !local_21 : (byte)local_21)` 472; vector2d_from_points2d
// 665 (3499 B); separate perp vector2f on clean base 545 (3501 B); side-select scalar/arm forms on clean base: symmetric cx/cy both arms
// 699 (3512 B, frame 0x8c, local_8 moves below local_14, masked 37), negated offset 724, if-assigned local_24 + scalar cx/cy 715,
// diagnostic asymmetric y-in-arms 719/735 (739 on clean base, aligned 80, masked 31; scalar cy 702/700 at 3499 B, frame 0x90).
// GB6 (fast with lane 6 r242's companion list; control 462 at 3504/3499 B, +0x100, aligned 182, masked 118; 22 trials, body unchanged):
// on the 462 base: separate scoped velocity at line 1162 682 (3499 B, +0x60); MIN(1.0f, ratio) both 461 (al 170, mk 108); early field_50
// alone 462 (al 173, mk 109), + MIN(1.0f, ratio) both 461 (al 161, mk 99), + separate local_26 *= 0.5f 475 (al 171, mk 103; folded single-expr
// 651); vector3f_add_scaled side point 665 (3499 B); sqrt(j^2+i^2) 462 (same code); magnitude2d(&local_26) 475; frame_velocity dots swapped 664.
// Combined retail lead B2_01 (mode-2 early i/j + MIN(1.0f, ratio) both + early field_50 + separate local_26 *= 0.5f, held by the coordinator):
// 673 (3478 B, fr=0x88, +0x100, 839/844 insns, al 98, mk 72; folded T04 677, al 103, mk 79); on B2_01: byte local_21 673 (same code),
// (bool)((byte)local_21 != 0) 670 (3483 B, 841/844 insns, al 93, mk 76: recovers retail's test/setne without moving local_2; cast, match-only),
// byte side_ok 554 (3499 B, +0x60), + per-arm center_y 682 (3488 B, pos +0x80, +0x60), + sep velocity 682, zero-new-local per-arm local_25.y
// (direct or +=) 739 (fr=0x90), two-ternary side 644 (al 131, mk 94), 29bab3 mul order 673, cross3f_staged 673.
// Side y accumulated in the two arms 719, conditional y product 560, named conditional y in the point
// initializer 712, paired displacement before translation 714, planar position copy incremented y then x
// 706, the two branch accumulations with the planar difference helper 719, conditional y with that
// helper 713, and conditional y with the shared planar dot 558. NOT kept: all raise the score.
// GB9 (fast with lane 6 r242's companion list, 7 peers MATCH; control 462 at 3504/3499 B, +0x100, fr=0x88; 14 trials, restored):
// `local_24 = local_23; if (!local_22) local_24 = -local_23;` 649 (3502 B); `local_24 = -local_23; if (local_22) local_24 = local_23;` 654 (3502 B);
// flipped `if (!local_22)` 478; `planar_scale` ternary 721 (3476 B, +0x4e), if/else 702 (3499 B, +0x4e); `local_26` member assigns 462 (same code);
// `byte same` in 4th else arm keeping `bool local_21` 665 (3499 B, +0x60: recovers retail's exact 8-bit `test al, al; setne al` at 0x29bf27 without a cast);
// norm `(i*i+k*k)+j*j` 747 (3499 B), `.n[0..2]` 750 (3499 B, mk 105 vs 117); drop `local_28` alone 467, + dot orders 469, + MIN(1.0f, ratio) + early field_50 468;
// 0x29ab90's steering formula 655 (3500 B); combined retail structure + `byte same` + `dot3f(&local_7, &local_9)` 670 (3483/3499 B, 841/844 insns, mk 64 vs 117: leaves only the 16-byte side-point y-in-arms block as missing length).
// M3 lane 1 r169 (h2m fast with lane 15 r139's companion list plus 0x1f38c0 and 0x22bbca; control 462 at 3504 B,
// 0x1f38c0 and 0x29b990 MATCH each run). Frame reading from the back end's slot packing: the frame is over 0x80 bytes, so
// slots sort by references per byte; the position's 12-byte slot has 7 (583) and the slot shared by the translated
// target, the pre-turn copy and 0x1f9760's unused output has 8 (4 + 3 + 1, 666), so the shared slot sorts below it
// (+0x70, position +0x7c; retail has the position below). One more reference of the position (retail reads its y in
// each arm of the side select) makes the two equal, and equal densities keep creation order. Retail copies the
// pre-turn vector whole (its k is stored) and forms only i and j of the translated target, as ours does. Forms: the
// side point copied from the position and offset in place 694 (first difference +0x4e: the point gets an 8-byte home,
// the position still 7), with y offset first 694; 0x1f9760's unused output declared ahead of its flag 462 (same
// code); the side distance chosen by if/else with the point's y member then x member assigned 706 (3499 B, +0x4e,
// the point again in memory); the if/else choice with main's point initializer 462 (same code).
// 12-byte workspace names for the heading/defaults/request, location/query/target, matched point types
// with distinct names, immutable request magnitude, and vector/planar request names: 462 each.
// Immutable translated point 668; point setter 473; shared default-vector pointer 664; in-place
// translation of the position or displacement copy 719/737; completed planar request copy 588;
// both planar points immutable 668. NOT kept: the naming forms tie, and the other forms
// raise the score. The position output still needs ESP+0x80 with the query output at +0x8c.
// M3 lane 14 r157 (fast screens with GB15's companion list; control 462 at 3504 B, aligned 193, registers and stack
// offsets masked 66). Frame reading from the back end's packer: the position's 12-byte slot has 7 references (583 per
// 1000 bytes) and the slot shared by the translated target (4), the pre-turn copy (3) and 0x1f9760's unused output (1)
// has 8 (666), so the shared slot sorts nearer ESP; the frame is otherwise retail's slot for slot. Forms: the two velocity
// dots axis first (`dot3f(local_4, &local_1->velocity)`, `dot3f(local_28, &local_1->velocity)`) 458; with both clamps
// as `MIN(1.0f, ratio)` 457 (aligned 180, masked 58: the cross product's six products are left, retail a.j, a.k, a.k,
// b.k, a.i, a.j first); on that, the override read straight into the result (`local_24 = override; if (local_24 ==
// 0.0f) {...}`) 663 (3511 B); the velocity vector built member by member 457 (same code), with the override form 663;
// local_6.i/.j set ahead of the k test 675 (3475 B, 839 instructions, aligned 123, masked 44); that with the override
// test written `== 0.0f` and the override in the else arm 655 (3499 B, but 850 instructions). A forced-ranking search
// on the 457 form found no single allocator decision that lowers it (12 of 2718 candidates). The middle dot axis first
// too (all three) 663 (3499 B, first difference +0x60: the frame's homes move). NOT kept: 457 is a gain of 5, under
// the bar.
// GB15 (fast with 0x29b2e0/0x29b990/0x170d70/0x11ce20/0x11cc90/0x1e5450/0x1f9e70/0x110ab0/0x2922b0/0x22bbca/0x1f38c0;
// control 462 at 3504/3499 B, +0x100, fr=0x88; 17 trials, body unchanged): scalar cx/cy if/else (no neg local) cy-first/
// cx-first/sub/local-s 718/718/717/738 (fr=0x8c, moves pos below local_14 at +0x84 but spills cx/cy); direct vector2f
// local_26 if/else 584 (fr=0x88); existing vector2d_from_points2d 665 (3499 B); vector2f perp + dot_product2d 660 (3499 B);
// scalar cx/cy with local_24 ternary 686 (fr=0x8c); 4th else arm bool == / ternary / long same 661/663/628; tail dot3f
// swapped 462; frame_velocity retail dot3f argument orders 458 (-4, matches lines 628-653; + cross3f_staged 458), + both
// MIN(1.0f, ratio) clamps 457 (-5, al 336, mk 106; + tail dot3f / cross3f_staged 457; NOT kept: under 8-point bar).
// M3 lane 16 r192 (H2M_FAST with GB15's companion list, control 462 as plain; with only 0x1f38c0 listed it reads 656):
// the side point through the existing point2f_set after the side ternary (its arguments evaluate right to left) 467;
// the turn weight's half factor folded into local_26 (`local_26 *= 0.5f;` ahead of the dot, then `local_24 *=
// local_26;`) 476 (3507 B); both 479 (3507 B); the +0x7c axis pointer as a typed member address beside forward 462 and
// as `&local_1->forward + 1` 462 (same code each). Body unchanged.
// M3 lane 11 r173 (H2M_FAST with GB15's companion list, all companions as in the control; control 462). Retail's side
// select computes the y row in each arm (`A = r; y = r * h + py` and `t = 0 - r; A = t; y = t * h + py`, each arm in its
// own registers, so the rows exist before allocation) and the x row after the join; ours merges any y row written per
// arm back into one row on the selected value, even with different operands per arm. Forms: an if/else with the y row
// in each arm on the selected value and the else value as `0.0f - local_23`, x after the join, 719 (3502 B, frame
// 0x90: the y row merged after the join and stored); the same with the true arm's row on local_23 itself 719 (same
// code); the y row as a second select whose else arm multiplies `(0.0f - local_23)` 734 (3521 B); the y member assigned
// first from a `0.0f - local_23` select 706 (3499 B, first difference +0x4e); that with both clamps as `MIN(1.0f,
// ratio)` and the first and last frame-velocity dots axis first 707 (3499 B, +0x4e). Re-measured: those two forms alone
// 457 (a gain of 5, under the bar); the else value as `0.0f - local_23` alone 462 (same code).
// GA22 (12 fast trials on calibrated companion list, control 462 at 3504/3499 B, +0x100, fr=0x88, pos=+0x8c, al=178, mk=64, 0x1f38c0 MATCH, body unchanged):
// value-returning point2f_add_scaled_val in if/else (xy or yx) 793 (3555 B, fr=0x8c, pos=+0x84), ternary yx 838 (3590 B, fr=0x94);
// symmetric if/else with same arm-local s (y before x or x before y) 729 (3527 B, fr=0x8c, pos=+0x84, ins=846/844: moves pos below local_14 but spills point2f to x87 fsub);
// vector2f perp with dot_product2d at both perpendicular dots 660 (3499 B, +0x60, fr=0x88); normalize_inline on 462 base 763 (3503 B, +0x60);
// on the 457 base (MIN(1.0f, ratio) both + axis-first frame_velocity dot3f): dot_product2d at 0.99 test 660 (3499 B, +0x60), double-parens 0.99 + cross3f_staged + dot3f(&local_7, &local_9) 467,
// symmetric if/else s yx 729, direct == local_22 bool flip 661, early field_50 457 (3504 B, al=165, mk=56: under 8-point keep bar).
// M3 lane 7 r237 (h2m fast with GB15's companion list; control 462): the side point's sum with the two operands swapped
// 462, local_14 declared with the opening locals 462 (both the same code); the side value as `local_24 = local_23;
// if (!local_22) local_24 = -local_24;` 649 (3502 B); the side point's y member assigned ahead of its x on the ternary
// 706 (3499 B, first difference +0x4e: every 4-byte home four bytes lower, as the earlier select form); the direction copied
// as a brace initializer from the three input members in i, j, k order 764 (3487 B); local_19's test written as
// `field_14 > 0.0f ? field_14 : ...` 706 (3500 B); local_18 as the float product `field_1c * 0.5f` 703 (3499 B).
// GB28 (28 fast trials on calibrated companion list, all 8 matched peers including 0x1f38c0 and 0x29b990 MATCH; control 462 at 3504/3499 B, +0x100, fr=0x88, pos=+0x8c; body unchanged):
// scalar `local_24` + `cy` in arms / `cx` after join 720 (3535 B, fr=0x90: keeps `y` unmerged in both arms but spills `local_24`/`cy`); direct `local_26` in if/else with `local_24` xy 595 (fr=0x88, pos=+0x8c), yx no-s 707; ternary in both coords 589;
// drop `local_31/32/33` 696; scalar `vx/vy/vz` frame_velocity 673; `vector3f_add_scaled` frame_velocity diff 672 (+0xf9, pos=+0x64); `vector3f_add(&local_7, &local_6, &local_6)` 664 (3499 B, +0x60); `0.99` dot swapped / ji 464/462;
// turn factor `MAX(dot, 0.0f)` single expr 671; `dot_product2d` before `local_26` 461 (-1); `basis_dot_xzy(&local_9, &local_9)` + `vector3f_scale` 664 (3499/3499 B, +0x60: matches all 21 `i, k, j` normalize instructions verbatim; + 457 + early `local_6.i/.j` + `byte same` 673); zero-extra-local inline `i*i+k*k+j*j` + `vector3f_scale` 747 (3499 B, +0x100);
// on the 457 base (MIN(1.0f, ratio) both + axis-first frame_velocity dots): `dot_product2d` before `local_26` 456 (-6, under 8-point keep bar; + tail `dot3f(&local_7, &local_9)` / `0.99` ji / early `field_50` / `cross3f_staged` 456, al=333, mk=104), `local_23 > local_24` 652, single-expr `local_24` 666, `vector3f_add(&local_6, &local_7, &local_6)` 663, single-stmt `local_27` 656, early `field_50` 457 (al=328, mk=97), clean combo 702, turn `MAX` 671.
// @retail 0x29bab0
void __stdcall function_29bab0(long arg_0, long arg_1, vector3f const *arg_2,
	vector3f *arg_3, short *arg_4, vector3f *arg_5, bool *arg_6, bool *arg_7, short arg_8)
{
	byte *local_0 = (byte *)actor_get(arg_0);
	s_slot_object_view *local_1 = object_get(arg_1);
	s_29bab0 const *local_2 = (s_29bab0 const *)function_1e5450(arg_0, local_1->tag_index);
	bool local_3 = false;
	*(bool *)(local_0 + 0x6d1) = false;
	if (!arg_8)
	{
		*(real *)(local_0 + 0x658) = *(real *)(local_0 + 0x65c);
		*(real *)(local_0 + 0x660) = *(real *)(local_0 + 0x664);
		*(vector3f *)(local_0 + 0x678) = *(vector3f *)(local_0 + 0x66c);
	}
	if (*(long *)(local_0 + 0x26c) == NONE)
		*(long *)(local_0 + 0x7fc) = function_1f5f30(arg_0);
	vector3f const *local_4 = (vector3f const *)(local_0 + 0x290);
	vector3f local_5 = *local_4;
	vector3f local_6 = *g_4687a4;
	vector3f local_7 = *g_4687a4;
	point3f local_8;
	function_b9dd0(arg_1, &local_8);
	vector3f local_9 = *arg_2;
	real local_10 = normalize3d(&local_9);
	if (!local_2)
		*(bool *)(local_0 + 0x654) = false;
	else if (!*(bool *)(local_0 + 0x5d0) || *arg_7)
	{
		*(bool *)(local_0 + 0x655) = true;
		*(bool *)(local_0 + 0x654) = false;
		function_29b990(arg_0, arg_1, local_4, NULL, &local_5);
	}
	else
	{
		bool local_11;
		vector3f local_13;
		if (function_1f9760(arg_0, arg_1, &local_8, local_10, &local_3, &local_11, &local_13))
		{
			*(bool *)(local_0 + 0x655) = true;
			*(bool *)(local_0 + 0x654) = false;
			*arg_7 = true;
			function_29b990(arg_0, arg_1, local_4, NULL, &local_5);
		}
		else
		{
			function_26c180(arg_0);
			vector3f local_14;
			vector3f_add((vector3f const *)&local_8, arg_2, &local_14);
			vector2f local_15 = *(vector2f const *)arg_2;
			real local_16 = normalize2d((point2f *)&local_15);
			vector2f local_17 = *(vector2f const *)local_4;
			normalize2d((point2f *)&local_17);
			real perpendicular = 0.0f - local_17.j;
			real local_18 = (real)((double)local_2->field_1c * 0.5);
			real local_19 = 0.0f >= local_2->field_14 ? local_1->unknown03c : local_2->field_14;
			real local_20 = 1.0f;
			bool local_21;
			if (*(bool *)(local_0 + 0x654))
			{
				local_21 = !(local_15.i * local_17.i + local_15.j * local_17.j < 0.99) &&
					(*(bool *)(local_0 + 0x4ae) || !(local_18 > local_16));
			}
			else
			{
				bool local_22 = arg_2->i * perpendicular + arg_2->j * local_17.i > 0.0f;
				real local_23 = local_2->field_20;
				real local_24 = local_22 ? local_23 : -local_23;
				point2f local_25 = { local_8.x + perpendicular * local_24, local_8.y + local_17.i * local_24 };
				vector2f local_26 = { local_14.i - local_25.x, local_14.j - local_25.y };
				if (local_23 > sqrt(local_26.i * local_26.i + local_26.j * local_26.j) && (*(short *)(local_0 + 0x656) != 2 || !*(bool *)(local_0 + 0x4ae)))
					local_21 = false;
				else if (!*(bool *)(local_0 + 0x4ae) && local_18 > local_16)
					local_21 = false;
				else if (dot_product2d((vector2f const *)arg_2, &local_17) > 0.0f)
					local_21 = true;
				else
				{
					local_21 = local_26.j * local_17.i + local_26.i * perpendicular > 0.0f;
					if (!local_22) local_21 = !local_21;
				}
			}
			local_5 = *arg_2;
			short local_22;
			real local_23;
			if (!(local_2->field_44 > local_16) && local_21 && !*(bool *)(local_0 + 0x4d5) && !*(bool *)(local_0 + 0x5d1))
			{
				bool local_24 = function_2922b0(arg_0, arg_1, &local_5, &local_7, &local_20, &local_23, true);
				*(bool *)(local_0 + 0x655) = false;
				if (!local_24)
				{
					local_22 = 3;
					*(bool *)(local_0 + 0x654) = true;
				}
				else
				{
					local_22 = 2;
					if (!*(bool *)(local_0 + 0x4ae) && local_2->field_1c > local_16)
					{
						real local_25 = 1.0f - MIN((local_2->field_1c - local_16) / (local_2->field_1c - local_18), 1.0f);
						local_5.k *= local_25;
					}
					if (function_30bf0(&local_5) > 0.0f && fabs((double)local_5.k) > g_47ffd0)
					{
						real local_25 = (real)sqrt(1.0f - g_47ffd0 * g_47ffd0);
						if (normalize2d((point2f *)&local_5) > 0.0f)
						{
							local_5.i *= local_25;
							local_5.j *= local_25;
							local_5.k = function_29bab2(local_5.k, -g_47ffd0, g_47ffd0);
						}
					}
				}
				if (function_30bf0(&local_5) == 0.0f) local_5 = *local_4;
			}
			else
			{
				local_22 = 3;
				*(bool *)(local_0 + 0x655) = false;
				*(bool *)(local_0 + 0x654) = true;
				if (local_19 > local_16) local_5 = *local_4;
				function_2922b0(arg_0, arg_1, &local_5, &local_7, &local_20, &local_23, false);
			}
			real local_24 = 0.0f;
			if (local_22 == 2)
			{
				if (*(real *)(local_0 + 0x47c) != 0.0f)
					local_24 = *(real *)(local_0 + 0x47c);
				else
				{
					real local_25 = MAX(local_16 * local_16, 0.001f);
					real local_26 = local_2->field_1c * 0.5f;
					local_26 *= local_26;
					local_26 /= local_25;
					real local_27 = dot_product2d(&local_15, &local_17);
					if (!(local_27 > 0.0f)) local_27 = 0.0f;
					local_24 = (1.0f - local_27) * 0.5f;
					local_24 *= local_26 * 0.5f;
					local_24 *= 4.0f;
				}
				local_24 = function_29bab2(local_24, 0.05f, 1.0f);
				if (!(local_24 > local_23)) local_24 = local_23;
			}
			else if (local_22 == 1) local_24 = 1.0f;
			vector3f local_25 = local_5;
			if (local_22 == 3)
				function_29b990(arg_0, arg_1, local_4, &local_5, &local_5);
			else
				function_29b2e0(arg_0, arg_1, local_4, &local_5, local_24, &local_5);
			*(real *)(local_0 + 0x668) = local_24;
			if (local_22 == 1)
			{
				*arg_6 = false;
				*(bool *)(local_0 + 0x654) = true;
			}
			else
			{
				vector2f local_26 = *(vector2f const *)&local_25;
				normalize2d((point2f *)&local_26);
				real local_27 = local_2->field_50;
				local_27 *= function_1f9e70(arg_0, arg_1, (point2f const *)&local_8, &local_15, &local_26,
					&local_17, NULL, local_10, local_22 != 2);
				function_1f8510(true, &local_9, local_4, &local_6);
				if (local_22 == 2)
				{
					*(bool *)(local_0 + 0x654) = false;
					if (local_2->field_1c > local_16)
					{
						local_6.k *= MIN((local_2->field_1c - local_16) / (local_2->field_1c - local_18), 1.0f);
					}
					else local_6.k = 0.0f;
					local_6.i = 1.0f;
					local_6.j = 0.0f;
					*arg_6 = false;
				}
				if (local_27 > local_20) local_27 = local_20;
				vector3f_scale(&local_6, local_27, &local_6);
				if (local_2->field_68 > 0.0f)
				{
					vector3f const *local_28 = (vector3f const *)((byte *)local_1 + 0x7c);
					vector3f local_29;
					cross3f(local_28, local_4, &local_29);
					/* the velocity along the forward, side and up axes */
					vector3f frame_velocity = { dot3f(&local_1->velocity, local_4), dot3f(&local_1->velocity, &local_29),
						dot3f(&local_1->velocity, local_28) };
					real local_30 = 1.0f / local_2->field_68;
					real local_31 = local_6.i - local_30 * frame_velocity.i;
					real local_32 = local_6.j - local_30 * frame_velocity.j;
					real local_33 = local_6.k - local_30 * frame_velocity.k;
					local_6.i = function_29bab3(local_6.i, local_31, local_2);
					local_6.j = function_29bab3(local_6.j, local_32, local_2);
					local_6.k = function_29bab3(local_6.k, local_33, local_2);
				}
				local_6.i += local_7.i;
				local_6.j += local_7.j;
				local_6.k += local_7.k;
				function_1f3e40(arg_1, &local_5, &local_6);
				vector3f previous;
				vector3f_scale(&local_6, 1.0f - local_2->field_64, &local_6);
				vector3f_scale((vector3f const *)(local_0 + 0x678), local_2->field_64, &previous);
				vector3f_add(&previous, &local_6, &local_6);
				local_6.i = function_29bab2(local_6.i, -1.0f, 1.0f);
				local_6.j = function_29bab2(local_6.j, -1.0f, 1.0f);
				local_6.k = function_29bab2(local_6.k, -1.0f, 1.0f);
			}
			if (*(bool *)(local_0 + 0x482) && *(bool *)(local_0 + 0x5d0) && !function_110ab0(arg_1) && local_10 > 0.0f && (local_22 == 2 || local_22 == 3))
			{
				function_ba1d0(*(long *)(local_0 + 0x18), &local_7, NULL);
				if (function_30bf0(&local_7) > 0.0f && dot3f(&local_9, &local_7) > 0.85f)
					function_1e4290(arg_0, true);
			}
			*(short *)(local_0 + 0x656) = local_22;
		}
	}
	if (*arg_7 && !local_3 && *(short *)(local_0 + 0x4ac) == 6)
		*(bool *)(local_0 + 0x4bc) = true;
	*(vector3f *)(local_0 + 0x66c) = local_6;
	*arg_4 = 0;
	*arg_3 = local_5;
	*arg_5 = local_6;
}
