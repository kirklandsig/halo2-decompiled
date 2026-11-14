// @flags /O2 /Gr /arch:SSE
#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_11cc90.h"
#include "normalize3d.h"
#include <math.h>

#define MIN(a, b) ((a) > (b) ? (b) : (a))

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
PRIVATE __forceinline real function_29bab4(vector3f const *arg_0, vector3f const *arg_1)
{
	return arg_0->k * arg_1->k + arg_0->j * arg_1->j + arg_1->i * arg_0->i;
}
// MiniMax G (11-13): no new form, no lower score (715): the body is 3482 bytes against retail's
// 3499 and the three-way branch and the float constants still differ from retail's own expression
// trees.
// score 715 (was 750): s1E walk against retail (3482 bytes, retail 3499; frame 0x88 agrees). Behaviour fixes: 0x29b2e0's
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
// P481: Complete established/radial decision byte/char/word/long, original true
// default then fully assigned leaves: 648/690/648/690/722/721/724/723 (fast control
// 648, full 715). NOT kept: no native entry/home equality or gain; original full
// Boolean decision, radial double, helpers and complete body restored.
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
			point2f local_14 = { local_8.x + arg_2->i, local_8.y + arg_2->j };
			vector2f local_15 = { arg_2->i, arg_2->j };
			real local_16 = normalize2d((point2f *)&local_15);
			vector2f local_17 = { local_4->i, local_4->j };
			normalize2d((point2f *)&local_17);
			real local_18 = (real)((double)local_2->field_1c * 0.5);
			real local_19 = 0.0f >= local_2->field_14 ? local_1->unknown03c : local_2->field_14;
			real local_20 = 1.0f;
			bool local_21 = true;
			if (*(bool *)(local_0 + 0x654))
			{
				if ((double)local_15.i * local_17.i + (double)local_15.j * local_17.j < 0.99)
					local_21 = false;
				else if (!*(bool *)(local_0 + 0x4ae) && local_18 > local_16)
					local_21 = false;
			}
			else
			{
				bool local_22 = arg_2->i * -local_17.j + arg_2->j * local_17.i > 0.0f;
				real local_23 = local_2->field_20;
				real local_24 = local_22 ? local_23 : -local_23;
				point2f local_25 = { local_8.x + -local_17.j * local_24, local_8.y + local_17.i * local_24 };
				vector2f local_26 = { local_14.x - local_25.x, local_14.y - local_25.y };
				double local_27 = sqrt((double)local_26.i * local_26.i + (double)local_26.j * local_26.j);
				if (local_23 > local_27 && (*(short *)(local_0 + 0x656) != 2 || !*(bool *)(local_0 + 0x4ae)))
					local_21 = false;
				else if (!(local_23 > local_27) && !*(bool *)(local_0 + 0x4ae) && local_18 > local_16)
					local_21 = false;
				else if (arg_2->j * local_17.j + arg_2->i * local_17.i > 0.0f)
					local_21 = true;
				else
				{
					local_21 = local_26.j * local_17.i + local_26.i * -local_17.j > 0.0f;
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
					real local_25 = local_16 * local_16;
					if (!(local_25 > 0.001f)) local_25 = 0.001f;
					real local_26 = local_2->field_1c * 0.5f;
					local_26 = local_26 * local_26 / local_25;
					real local_27 = local_15.i * local_17.i + local_15.j * local_17.j;
					if (!(local_27 > 0.0f)) local_27 = 0.0f;
					local_24 = (1.0f - local_27) * 0.5f * (local_26 * 0.5f) * 4.0f;
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
				vector2f local_26 = { local_25.i, local_25.j };
				normalize2d((point2f *)&local_26);
				real local_27 = function_1f9e70(arg_0, arg_1, (point2f const *)&local_8, &local_15, &local_26,
					&local_17, NULL, local_10, local_22 != 2) * local_2->field_50;
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
				local_6.i *= local_27;
				local_6.j *= local_27;
				local_6.k *= local_27;
				if (local_2->field_68 > 0.0f)
				{
					vector3f const *local_28 = (vector3f const *)((byte *)local_1 + 0x7c);
					vector3f local_29;
					local_29.i = local_28->j * local_4->k - local_28->k * local_4->j;
					local_29.j = local_28->k * local_4->i - local_28->i * local_4->k;
					local_29.k = local_28->i * local_4->j - local_28->j * local_4->i;
					real local_30 = 1.0f / local_2->field_68;
					real local_31 = local_6.i - local_30 * function_29bab4(&local_1->velocity, local_4);
					real local_32 = local_6.j - local_30 * function_29bab4(&local_1->velocity, &local_29);
					real local_33 = local_6.k - local_30 * function_29bab4(&local_1->velocity, local_28);
					local_6.i = function_29bab3(local_6.i, local_31, local_2);
					local_6.j = function_29bab3(local_6.j, local_32, local_2);
					local_6.k = function_29bab3(local_6.k, local_33, local_2);
				}
				local_6.i += local_7.i;
				local_6.j += local_7.j;
				local_6.k += local_7.k;
				function_1f3e40(arg_1, &local_5, &local_6);
				local_6.i = *(real *)(local_0 + 0x678) * local_2->field_64 + local_6.i * (1.0f - local_2->field_64);
				local_6.j = *(real *)(local_0 + 0x67c) * local_2->field_64 + local_6.j * (1.0f - local_2->field_64);
				local_6.k = *(real *)(local_0 + 0x680) * local_2->field_64 + local_6.k * (1.0f - local_2->field_64);
				local_6.i = function_29bab2(local_6.i, -1.0f, 1.0f);
				local_6.j = function_29bab2(local_6.j, -1.0f, 1.0f);
				local_6.k = function_29bab2(local_6.k, -1.0f, 1.0f);
			}
			if (*(bool *)(local_0 + 0x482) && *(bool *)(local_0 + 0x5d0) && !function_110ab0(arg_1) && local_10 > 0.0f && (local_22 == 2 || local_22 == 3))
			{
				function_ba1d0(*(long *)(local_0 + 0x18), &local_7, NULL);
				if (function_30bf0(&local_7) > 0.0f && function_29bab4(&local_9, &local_7) > 0.85f)
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
