// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_260670.CPP: scoring an actor's candidate positions and choosing the
   reference to follow */

#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_0259a0.h"
#include "unknown_0259d0.h"
#include "normalize3d.h"
#include "unknown_2626b0.h"
#include <float.h>
#include "unknown_25fc30.h"

bool function_260160(long actor_index, s_prop_search *search, s_261d20_entry *entry);
#include "props.h"
#include <string.h>
typedef bool (__stdcall *t_reference_compare)(long, long, const void *);
void sort_4byte(void *unused, long *elements, unsigned long count, t_reference_compare compare, const void *context);
bool __stdcall function_2600e0(long a, long b, void *unused);
void __stdcall function_25f7b0(long actor_index, s_type_967e20 *context, s_type_b36ac5 *position);
void function_270590(s_path_source *source, s_type_c3b527 const *point, long node_index);

struct firing_position_post_evaluator
{
	short flags;
	bool (__stdcall *proc)(long, s_type_967e20 *, s_type_b36ac5 *);
};
extern firing_position_post_evaluator g_44adf0[12];
extern s_type_b36ac5 *g_51eca0;
short g_51ec9c;

struct s_reference_candidate_view;
struct s_reference_direction_request;
real function_30bf0(vector3f *vector);
void function_260530(s_reference_candidate_view *entry, s_reference_direction_request const *request);
void function_260060(long actor_index, short count, s_type_967e20 *context, s_type_b36ac5 *position);
bool function_2600a0(long actor_index, s_type_967e20 *context, s_type_b36ac5 *position);
long function_1e4a50(long index);
long function_262a30(s_reference reference);
real function_11e000(point3f const *b, point3f const *a, vector3f const *d);

// MiniMax E (11-20): no new form, no lower score (440): re-read against retail, the frame stays 0x8a8 against retail's
// 0x8a0 because the two flags take a dword each here and share one dword in retail (our bytes at 0xb and 0x13 against
// retail's adjacent pair at 0x6 and 0x7), so every home above them and the sorted array's start move with the frame;
// the sorted loop still calls 0x2600a0 where retail inlines 0x262a30's owner lookup, and the evaluator loop still tests
// the running result before the table's proc. The body stands.
// MiniMax B (11-19): score 440 (tried 416, NOT kept: a 510-entry sort array gives retail's 0x8a0 frame, but callers pass up to
// 0x200 entries, so it can overflow and only steers the frame): with 510 the first
// difference moves to +0x3b; the early homes still differ (the reference at 0x18 against retail's 0xc, the two flag bytes
// eight apart at 0xb and 0x13 against retail's adjacent pair at 0xe and 0xf) and the sorted loop still calls 0x2600a0
// where retail inlines 0x262a30's owner lookup.
// MiniMax G (11-18): no new form, no lower score (440): the frame stays 0x8a8 against retail's 0x8a0 and the two
// flag bytes still take homes eight bytes apart (ours 0xb and 0x13, retail 0x6 and 0x7), so every home above
// them moves with the frame; the sorted loop still calls 0x2600a0 where retail inlines 0x262a30's owner lookup
// and the other way round, and the two flags written as bytes still give each its own home (439, one point,
// below the bar); all restored.
// score 440 (was 440): The frame and flag homes differ, and the sorted evaluator and owner lookup have opposite inlining decisions (round 196's forms at the end of this note).
// Earlier 440 (was 528): earlier 611, 700 (was 700). Enabled from the draft: first difference +0x2a, 3148 bytes against 3077. The draft's two `goto done` as returns 700 (697 with them). Enabling it lets callers keep
// MiniMax H (11-18): no new form, no lower score (440): the frame stays 0x8a8 against retail's 0x8a0 and the early
// homes keep their order (ours the reference at 0x18, the flag bytes at 0xb and 0x13, the index offset at 0x20 and
// the score at 0x24; retail the reference at 0xc, the flag bytes at 0xe and 0xf, the offset at 0x1c and the score at
// 0x20), so the gap is not one local's size: dropping the one-use distance_squared local changes no home and no
// frame (the value stays in an xmm register), and the two call sites still differ (retail calls 0x2600a0 in the sorted
// loop and inlines 0x262a30's owner lookup, this build does the opposite); the body stands.
// MiniMax I (11-17): no new form, no lower score (440): the frame stays 0x8a8 against retail's 0x8a0 and this build
// cannot pack the two flag bytes into one home, so every home sits four bytes above retail's (the reference at 0x18 where
// retail has 0xc) and the tail's length follows from that; the two call sites still differ (retail calls 0x2600a0 in the
// sorted loop and inlines 0x262a30's owner lookup, ours does the opposite) and the x87 and SSE operand orders in the loops
// differ, so the body stands;
// Earlier 528 (was 528, 611, 700): enabled from the draft: first difference +0x2a, 3148 bytes against 3077. The draft's two `goto done` as returns 700 (697 with them). Enabling it lets callers keep
// earlier 700 (was 700). Enabled from the draft: first difference +0x2a, 3148 bytes against 3077. The draft's two `goto done` as returns 700 (697 with them). Enabling it lets callers keep
// (the merged second note line: was 528, 611, 700): enabled from the draft: first difference +0x2a, 3148 bytes against 3077. The draft's two `goto done` as returns 700 (697 with them). Enabling it lets callers keep
// frame 0x8a8 against 0x8a0, 3097 bytes against 3077; site 2 inlines 0x2600a0 where retail calls it, and 0x262a30 is
// called where retail inlines it around a call to 0x13e2d0 (see the r111 lines below the notes).
// Earlier 700 (was 700): enabled from the draft: first difference +0x2a, 3148 bytes against 3077. The draft's two `goto done` as returns 700 (697 with them). Enabling it lets callers keep
// Earlier 528 (was 528, 611, 700): enabled from the draft: first difference +0x2a, 3148 bytes against 3077. The draft's two `goto done` as returns 700 (697 with them). Enabling it lets callers keep
// a bool output in a register across a later call, as retail (0x1b0540 matched with plain output locals).
// Round 178 (m3 lane 4, plain checks; aligned = difflib count with addresses masked, 293 here): the call sequence is
// retail's except two callees: retail calls 0x2600a0 in the sorted loop and 0x13e2d0 for the owner, ours inlines both
// (the first evaluator pass is inline in both). The aggregate of five locals keeps retail's frame; as plain locals
// the result/bool/word stores take retail's forms but the frame grows to 0x8a4 (840), with the sort scratch word
// scoped to its block 0x8a0 again (832, aligned 272). Tried: plain locals with the 4c store before 0x270590, the
// first evaluator pass as a 0x2600a0 call and the owner store as if/else 847; those three without plain locals 719;
// plain locals and the evaluator call 832; scoped scratch with the four 841 (aligned 251); plus the history reset as
// an inline helper used twice, the segment test through the existing 0x11e000 and a float fabs 862 (frame 0x8ac,
// the helper outlined at one site); the helper __forceinline 849; without 0x11e000 856 (aligned 248); then the tail
// nested under `selected != NONE`, a position pointer and the pathfinding select as an if 854 (aligned 255); the
// sorted loop's body nested under the continue test 854. 0x2600a0 and 0x13e2d0 stay inlined in every form.
// 700 before r75. M3 lane 13 r77: 611 -> 528 plain (3097 bytes) by two changes together: the five state values as
// plain locals (the grouping aggregate removed), and 0x260060's second parameter as `short` (retail's call pushes count's raw dword with no
// movsx, and 0x260060 hands it to the pre-evaluator procs, whose typedef takes `short`; every 16-bit count load here then becomes
// retail's 32-bit load). Before r77: the owner is read through the existing 0x262a30 (its file is /Ob1, so the hash lookup 0x13e2d0 stays a call
// as in retail; the private copy of its lookup let 0x13e2d0 inline here): 700 -> 611 plain at 3072 bytes. Left: the five state values'
// stack homes (retail: bools 0x6/0x7, result 0x8, a dword-stored selected at 0xc, best score 0x18; ours keep them in one aggregate at
// 0x4..0x10), 0x2600a0 still inlined at the sorted-loop call where retail calls it, retail inlines 0x262a30's body (with 0x13e2d0
// called) where ours calls it, the random path's jump to the shared `selected == NONE` test, and x87/SSE operand orders in the loops.
// Round 75 (m3 lane 13, fast; aligned count in brackets, masked+stack, base 610 [205-270 masked]): first evaluator pass as a call to
// 0x2600a0 708 (613 with the owner call; retail's `result && proc` loop shape, but site 2 stays inlined); perpendicular distance through the
// existing 0x11e000 752 (frame 0x8ac); all three 629; `inline` on 0x262a30's definition 726 (0x13e2d0 inlines with it), with the evaluator
// call 721, with 0x11e000 756, both 750; the `selected == NONE` test after the if/else (retail's flow) 623 [262], the tail nested under
// `selected != NONE` 628, both with the inline 724/737; the sorted loop as a while 610, its call test nested 610, the 25f7b0/score/
// 2600a0 steps as an inline helper 610 (same code: site 2 stays inlined in every form); the five state values as plain locals 742
// (frame 0x8a4, retail's dword store for selected and best at 0x18), in another order 742, with the sort scratch word declared in its
// block 734 [240] (frame 0x8a0), with the sorted array there too 730, plus the shared selected test 752 [238], plus normalize3d and the
// evaluator call 766 [230]; normalize3d for the unknown40 loop 610 [268] (retail's x87 length order exactly; only the scale products
// differ). Earlier: score 700 (was 697): enabled from the draft: first difference +0x2a, 3148 bytes against 3077. The draft's two `goto done`
// as returns 700 (697 with them). Enabling it lets callers keep
// Round 77 (m3 lane 13, fast unless noted; masked aligned count, then masked+stack, in brackets; base 528 [278/168], frame 0x8a8 against
// 0x8a0, 3097 bytes): the short parameter alone 707 [226/172] (3083), plain locals alone 742 (the r75 form). Retail packs the two
// bools into one dword (0x6/0x7) and the sort scratch word into the actor offset's dead slot (0x14); ours gives each its own dword.
// The bools declared first 528 (same code); the sort scratch word declared in its block 528 [277/171] (frame 0x8a4); the first
// evaluator pass as the existing 0x2600a0 610 [243/162] (frame 0x8a4: the written-out loop's `possible` bool had split the two bools,
// which now pack at 0x2/0x3; 3057 bytes, site 2 still inlined, the loop head padded where retail's is not); both 605 [198/156] (frame
// 0x8a0 as retail, 3041 bytes); the `selected == NONE` test after the if/else on the 528 base 567 [271/161] (3045), on the 605 form
// 624 [197/155] (3021). The score is lockstep by instruction, so these structural gains read as rises while the body stays short of
// retail's length: retail inlines 0x262a30 (calling 0x13e2d0) and calls 0x2600a0 at site 2; ours does the opposite (file flags).
// (v2-v6 above are forms of the r75 list re-measured on the new base.) New on the 528 base: both squared lengths through the
// existing length_sq3f 528 (same code), the projection dot and both perpendicular squared lengths through dot3f/length_sq3f 632
// [274/164], the projection update through vector3f_add_scaled 528, the delta's scale through vector3f_scale 528, the reported
// entry as an aggregate assignment in place of memcpy 528 (all same code but the second). Aligned counts of the kept change,
// plain: exact 393 -> 413, masked 266 -> 278, masked+stack 205 -> 168; frame 0x8a0 -> 0x8a8, length 3072 -> 3097 (retail 3077).
// MiniMax S (11-10): no new form, no lower score (700): retail keeps the two flags as separate byte locals at F+6/F+7,
// the reference at F+8 and the selection in a four-byte slot at F+0xc (a dword -1 store with word compares), while our
// state aggregate puts the flags at F+0xc/F+0xd and the selection in a word at F+0xe. Splitting the aggregate into
// standalone locals gives retail's F+8 and F+0xc but leaves the flags at F+2/F+3 and the frame at 0x8a4 against retail's
// 0x8a0 (841), the same split with a four-byte selection scores 836, and an aggregate whose flags come first is kept
// in registers by the compiler (836), so the aggregate stays.
// M3 lane 12 r111 (plain; 0x2605d0, 0x2613d0, 0x2600a0, 0x262a30, 0x13e2d0 and the other users of 0x11e000/0x2600a0 stay matched;
// aligned = difflib count, then with registers masked, base 278/106). The perpendicular block is 0x11e000's body: calling
// `function_11e000(target, actor position, &delta)` inside the sqrt 629 (3097 B, 284/108; it inlines, but with the contributor's
// unmatched statement order and a spill); with the first evaluator pass as `function_2600a0(actor_index, context, NULL)` as well 529
// (3072 B, frame 0x8b0, 246/100: site 1 then compiles to retail's inline `call edx` loop); plus the random pick through the existing
// random_index 529 (246/99); site-1 call and random_index without 0x11e000 543 (3056 B, frame 0x8a4, 240/101); on the base, the
// delta's length block (normalize3d's body with its arms swapped) as `position->unknown18 = normalize3d(&delta)` 576 (frame 0x8ac,
// 277/103: the block itself becomes retail's, float constant, `ja` and the zero store). Diagnostic, not kept: /Ob1 on this file
// 624 (site 2 still inlined: LTCG inlines by the callee's file, so retail's site-2 call needs 0x2600a0's file built without
// auto-inlining, a flags decision). Next: the helper calls together (0x2600a0, random_index, normalize3d, and 0x11e000 once
// 0x11e000 matches, BLOCKED-BY 0x11e000) on one base; most of the positional count is the frame size moving every parameter
// offset. Body restored.
// m3 lane 2 r69 (plain checks; aligned/masked in brackets, base 528 [341/174], frame 0x8a8): the segment block is
// the existing 0x11e000's body written out (retail's clamp `0 > t ... else if (t > 1)` joins in one register, and
// 0x11e000 is inlined here in retail), and the length block is normalize3d's (retail's float epsilon and its
// scale-first arm order). normalize3d for the length 576 [346/173] (frame 0x8ac: the squared length gets two homes);
// with the squared length as dot3f(&delta, &delta) 547 [347/160]; with that and 0x11e000 for the segment 577
// [316/144] (3097 bytes, frame 0x8a8; next round's base: the blocks then take retail's shape); 0x11e000 alone 629
// [351/176]; 0x11e000 + normalize3d 579 [338/154]; 0x11e000 + dot3f 622 [349/171]. The lockstep score rises because
// retail keeps count in EBX from the main loop through the random pick (`test bx, bx`, `movsx ecx, bx`) and loads
// scratch from memory in the loop, where ours keeps scratch in EBX and reads count from memory: retail's loop head
// then pads with a jmp and every line to 0x260e3c shifts by one.
// M3 lane 8 r152 (fast, base 528, masked aligned 105): retail's first evaluator pass is 0x2600a0's body inlined with a NULL
// position, its sorted-loop site calls 0x2600a0, and its owner lookup is 0x262a30 inlined (0x13e2d0 called, an inlined NULL test
// of the output pointer, `rep movsd`). Re-measured: round 77's first pass through 0x2600a0 with the sort scratch in its block 605
// (3041 B, frame 0x8a0). Diagnostic on it: `inline` on 0x262a30's definition 611 (3155 B: 0x262a30 and 0x13e2d0 both inline and
// the sorted-loop site stays inlined, so that site is not an inline-budget effect of the owner lookup; reverted). New: the
// path-data select as an if-assignment through a NULL-initialised local 528 (masked 104: retail's `xor` before the test), on
// round 77's form 606; the owner store as if/else with two stores 638 (3102 B), with the if-assignment select 638. Body unchanged.
// m3 lane 11 r90 (plain; callers 0x2605d0/0x2613d0 matched throughout): kept 528 -> 440 by calls to existing functions in place of
// their bodies written out: the random pick through random_index (alone 624: same pick code, but the inline call flips two
// normalize_inline products and drops a loop-head jmp), plus 0x11e000 for the perpendicular block (inlined, as retail), normalize3d
// for the length and dot3f for the squared length 515, plus dot3f for the target squared length 440 (aligned 341 -> 292, masked 298
// -> 256; both squared lengths now sum in retail's order, the first line for line, the second but for register names). On 440: the tail's path-state setup as the existing 0x271300 (retail inlines it: its `xor ecx, ecx` pathfinding select
// and store order) 576 (aligned 288; the normalize products flip again); with normalize3d in the unknown40 loop 566; the first pass
// as 0x2600a0 (re-measured) 605 at 3072 bytes (aligned 249, masked 216); that with the 0x271300 tail 528 (3072, aligned 240, masked
// 208); plus the sort scratch in its block 519 (3056 bytes, frame 0x8a0, first difference +0x3b, aligned 195, masked 161). Left on
// that last form: retail calls 0x2600a0 at the sorted-loop site and inlines 0x262a30 (0x13e2d0 called; its `&reference` key read
// keeps the result in memory, loaded once at a shared exit), ours the reverse, so it is 21 bytes short and the lockstep score stays
// above 440. Diagnostic (flags, not kept): `inline` on 0x262a30 with 0x13e2d0's file /Ob1 inlines 0x262a30 as retail but the
// sorted-loop site stays inlined (564 on the 528 base). On 440, the two history resets through the actor view's typed fields
// (`unknown3fe`, `unknown400[i].reference` in a counted loop, 0x262800's view) 773 at 3098 bytes.
// M3 lane 2 r72 (fast with 0x2605d0/0x261280/0x2600a0/0x262a30/0x260060/0x2613d0, all matched throughout; aligned/masked in
// brackets, base 528 [343/179]): retail's main loop keeps count in EBX (dword load, `test bx, bx`) through the random pick and
// reads scratch from its stack slot at the 0x270750 call; ours keeps scratch in EBX there and compares count in memory. The
// main loop index as `long` 555 [360/193]; the two flags declared bare and cleared by one chained `= false` after the
// declarations 581 [346/184]; result declared after the four state values 528 (same code: frame slots do not follow
// declaration order); the length test with a float epsilon (`fabs(length) < 0.0001f` or `0.0001f > fabs(length)`) 632
// [347/182] (retail's `fld dword` constant, one instruction shorter), that with the arms swapped as in normalize3d (`if
// (!(fabs(length) < 0.0001f))` scale, else zero) 624 [345/179]. Diagnostics (not kept): scratch read once through
// `byte *const *` to it at the loop call 528 (same code); a global in place of scratch at that call 555 (the global takes
// EBX instead: count never gets a register in our main loop). Compile-only (no /GL) packs the two flags at adjacent bytes
// as retail does; our LTCG build gives each its own dword.
// M3 lane 10 r106 (P1 fast, rest plain; masked aligned count in brackets, base 130; 0x2605d0/0x2613d0 matched throughout):
// the sort scratch declared in its block 440 [137] (frame 0x8a4); the `selected == NONE` test after the if/else (retail's
// random path ends `je exit; jmp shared-test`) 548 [115] (3061 B, 775 instructions as retail); that with the scratch in
// its block 569 [117]; that with the first evaluator pass as `function_2600a0(actor_index, context, NULL)` 618 [118] (3052
// B, frame 0x8a4); the unknown40 normalize through the existing function_30bf0 (retail's inlined block has 0x30bf0's load
// order k, j, i and its `i * inv`, `inv * j` products; normalize_inline loads k, i, j and multiplies `inv * i`) 860 [193]
// (it stays a call, 2985 B). Retail's 8-byte smaller frame fits its inlining: 0x262a30 inlined (its 4-byte result at
// frame 0) and 0x2600a0 called at the sorted-loop site, ours the reverse.
// MiniMax D (MM-DD) round: score 440 (was 440): no form found that lowers the lockstep score by the review bar, so the
// body is unchanged. Read from retail: the frame is 0x8a0 with the locals at fixed homes (the two bools 0x6/0x7, the
// result 0x8, the dword-stored selected 0xc, the actor 0x10, the count 0x14, the best score 0x18, the source at 0x34,
// the settings at 0x84, the sorted array at 0xa0); ours is 0x8a8 with the two bools in their own dwords (0x3/0xb), the
// result 0x14, the count 0x18, the best score 0x1c, the settings below the source and the array at 0xa8, so every
// parameter offset moves with the frame. Retail's evaluator loop tests the table's proc pointer at its head and
// calls through it (`call edx`), with the `possible` test after the index advance; the `selected == NONE` test (0x2610b9)
// and the exit (0x261264) are one each, the random path jumping to the shared test. Trials (fast, base 440): the
// source's unknown4c stored before the 0x270590 call as retail does 439 (one point, below the bar, reverted); the two flags
// as bytes 439 (same code, our LTCG build still gives each its own dword); the evaluator loop with the proc test at its
// head 460; the `selected == NONE` test moved after the if/else (retail's flow, re-measured from round 75) 547; one delta
// for both deltas 630. The permuter (300 s) found no variant better than the base.
// Probe source/probe settings/fallback source scope, seven nonempty subsets, then all with search-local records: 577/442/442/577/577/442/577/577.
// NOT kept: phase-local records regress and keep the 0x8a8 frame and 3097-byte extent.
// Original executable body restored.
// Indexed callback with explicit result or direct rejection 654/440, with sort-local scratch
// 634/440. Single callback read in those two forms 542/420, with sort-local scratch 507/412.
// Single callback read with row advance before testing result 543, with sort-local scratch 505.
// NOT kept: the 420/412 forms grow to 3113 bytes and change the callback register. The explicit
// result forms at the 0x8a0 frame and 3077-byte extent score 507 and 505.
// Range flags and two call boundaries still differ. Existing request admission, direction/bypass
// and published evaluator members, seven subsets at 440. All with branch-local sort scratch 440
// (frame 0x8a4). NOT kept: the frame is four bytes closer, with the same score and extent.
// M3 lane 15 r97 (fast; 0x2605d0, 0x2613d0, 0x2600a0, 0x262a30 matched unless noted; aligned = LCS count in brackets, base
// 440 [119]): where 0x2600a0 inlines is decided by its size at the inliner. With the first pass written as the call
// (604 at 3077 B, both sites inline) every form of 0x2600a0 that still matches inlines at both sites: an indexed loop 526, a
// named mask, a while loop, copied parameters and an inline flag-test helper 604 each (same code); the sorted-loop argument
// as `&positions[index]` 529, as `&positions[(short)sorted_indices[i]]` 595, the call's result in a named bool 604 (same
// code). Diagnostics (0x2600a0 then unmatched, removed): one extra byte store in 0x2600a0 inlines both sites, two stores of
// one value (65 B) give retail's decisions (the NULL-position pass inline, the sorted-loop site a call: 445 at 3049 B), a
// byte result under the bool return (65 B) the same (656 at 3026 B), one constant store more calls both; a third call site
// outside the loop and sixteen more all inline (no budget, no loop bonus), so retail's 0x2600a0 weighs about two
// instructions more at the inliner than ours at the same 60 bytes, and a NULL argument earns the discount. A threshold
// option on this file's flags (80, 78, 70) changes nothing in the reduced image. The 0x270590 store order (`unknown4c`
// before the call, as retail) 439. Grid on that store order (A first pass as 0x2600a0, C tail as 0x271300, D sort scratch
// in its block, B1 the `selected == NONE` test after the if/else, B2 plus `selected = NONE` when the random pick's
// 0x260160 fails, B3 plus the tail nested under `selected != NONE`): D 439, C 575, CD 577, A 602, AD 600, AC 525, ACD 515
// [105], B1 547, B1D 568, B1C 567, B1CD 537, AB1 615, AB1D 613, AB1C 537, AB1CD 529 [107], B2 549, B2D 559, B2C 553, B2CD
// 548, AB2 598, AB2D 579, AB2C 520, AB2CD 494 [103], B3 559, B3D 569, B3C 565, B3CD 560, AB3 635, AB3D 616, AB3C 560,
// AB3CD 531 [101, frame 0x8a0, 3034 B]. normalize3d for the unknown40 loop on the 440 base 626 (retail's x87 order, the
// scale products one instruction short). In the frame-0x8a0 forms the lockstep count is lost at two places: the unknown40
// loop's `inv * i` product is one byte short of retail's `i * inv`, which turns the main loop's pad `jmp; lea ecx` into
// one `lea ebx`, and the random path keeps its own epilogue where retail has `je exit; jmp shared test`.
// m3 lane 14 r113 (fast; 0x2605d0, 0x2613d0, 0x2600a0 matched throughout; aligned/masked in brackets, base 440 [290/254]):
// retail's first evaluator pass tests the running result before the table's proc (0x2600a0's own `result && e->proc`
// order: proc load at the head, `test al, al; jne` at the foot). The first pass's test as `possible && proc` 526 [248/216]
// (3077 bytes, retail's length; that loop becomes retail's, the frame stays 0x8a4, site 2 still inlined and the owner
// lookup still a call); that with the sort scratch in its block 518 [199/165] (3057 bytes, frame 0x8a0, first difference
// +0x3b); the first pass as a pointer walk, `possible && e->proc` 604 [252/219] (3077 bytes), `e->proc && possible` 440
// (same code); the pointer walk with the sorted index as long 611 [252/218] (3069 bytes). Lockstep score rises: not kept.
// Finite-distance OR/logical OR/conditional accumulation 585/610/440. Range OR
// after/before invalidation 576/581. Both OR 556, logical finite with range OR 558,
// logical range 791. NOT kept: smallest frame 0x8a4 still exceeds retail's 0x8a0
// and every smaller-frame form raises the score. The call boundaries still differ.
// m3 lane 9 r145 (plain control 440; trials fast unless noted, 0x2600a0 checked each time): retail passes the sort's unused first
// argument as the address of frame 0x14 (the actor offset's dead slot); ours puts that word at 0x88. Retail's 0x2600a0 weighs more at
// the link-time inliner than ours (lane 15 r97), so spellings of 0x2600a0 that keep its 60 bytes were tried, each with the first
// pass here as `function_2600a0(actor_index, context, NULL)`: `!= 0` on the mask test 604, the mask in a named local 604, the
// iterator declared before the loop 604, `&g_44adf0[0]` with `!= NULL` 604, the result through a named local 604 (all 3077 bytes,
// both sites still inlined), an indexed loop 526 (re-measured); spellings that change 0x2600a0's bytes: `!= false` on the proc
// result 705 (plain; 0x2600a0 65 bytes, 15), `result ? true : false` 656 (0x2600a0 65 bytes), `result && proc(...)` 704 (74 bytes).
// None moves the sorted-loop site to a call. Both files restored.
// m3 lane 1 r129 (fast with 0x2605d0, 0x2613d0 and 0x2600a0, all matched throughout; control 440, frame 0x8a8 here against
// retail's 0x8a0; aligned 244/233 = unmatched instructions ours/retail, masked 86/75; body unchanged). Retail packs the two
// flags in one dword (+0xe/+0xf after the two entry pushes) and gives the sort's unused first argument the actor offset's
// dead slot; ours keeps each flag in a dword of its own (+0xb and +0x13) and the sort scratch in a slot of its own. The sort
// scratch declared in the sort's block 440 (same frame, aligned 244/233); the two flags renamed so that their names sort
// together 440 (aligned 246/237: names do not pack them); both 440; the selected index as a long 563 (3084 B); that with
// the scratch in its block 561. The frame's two extra dwords and the two opposite expansion decisions remain.
// M3 lane 14 r125 (fast with 0x2605d0, 0x2613d0 and 0x2600a0, all MATCH; control 440, aligned 292, frame 0x8a8): the sort
// scratch in a block of its own around just the sort call 439 (aligned 291, frame 0x8a4: the scratch then takes retail's
// dead actor-offset slot, `lea ecx, [esp + 0x2c]` as retail); the scratch declared first 440; the two flags declared the
// other way round 440; on the tight-block form the flags as `char` 439 (same code). Diagnostic (a grouping form, match-only):
// on the tight-block form the two flags as one two-element bool array 510 at 3081 B (frame 0x8a0 as retail, aligned 204,
// first difference +0x2a): retail keeps the two flags as adjacent bytes of one cell (+6/+7), ours spreads them over two
// cells (+3 and +0xb), which is the frame's last extra dword. Left there: the small-cell order (flags +0/+1 against +6/+7,
// actor and result swapped) and the two opposite inlining decisions. Body restored.
// M3 lane 12 r154 (fast with 0x2605d0/0x2613d0/0x2600a0/0x262a30, all MATCH each time; control 440, aligned 538 equal): the
// tail's path-data select as a NULL-initialised local and an if 442; that with the sort scratch in its own block 441 (frame
// 0x8a4); the two history resets through one forced-inline helper taking the actor index 628, as a plain inline helper 793
// (3049 B: one site stays a call); the reported owner as `if (owner == actor_index) owner = NONE;` and one store 632.
// Diagnostic (reverted): 1, 2 or 3 dead initialised locals ahead of the flags 442 each (the flags keep their own dwords,
// frame 0x8a8). Read from retail: the unknown40 normalize loads k, j, i and scales `i * inv`, `inv * j`, `k * inv`, where
// normalize_inline loads k, i, j and scales `inv * i` (both sum (i*i + k*k) + j*j); retail calls 0x271300 at its first two
// sites and expands it only in the tail.
// M3 lane 7 r196 (fast with 0x2605d0, 0x2613d0, 0x2600a0, 0x262a30, 0x13e2d0, 0x260060, 0x2600e0; control 440 at
// 3097 B, aligned 246, register/offset-masked 84; body unchanged). Read from retail: 0x262a30 is called out of line only
// from 0x2623a0 and expanded here (0x13e2d0 called); 0x2600a0 is called from 0x260160 and here at the sorted loop. The
// expanded owner lookup takes the address of its by-value reference, which is why retail keeps `result` in memory (frame
// +8, `mov eax, [esp + 0x18]` at the exit) where this build keeps it in EBX. The perpendicular block's remaining operand
// orders are inside the expansion of 0x11e000 (a contributor's finished function: BLOCKED-BY 0x11e000). Tried: the two
// flags declared just before the main loop 772 (3081 B, frame 0x8a4: they are then enregistered, retail clears them in
// memory at entry); the sorted-loop evaluator call and the score test as nested ifs 440 (same code); the sorted loop's
// two break tests as separate ifs 440 (same code); the range read from the field at both multiplication operands, no
// `range` local 632; the random path's history reset through the entry `actor` pointer 786 (3049 B: retail recomputes
// the actor from the saved index offset); `*has_path = false` ahead of the random pick 449. Diagnostic (removed): the
// owner lookup through a file-private copy of the find-and-copy helper 663 (3191 B: 0x13e2d0 is then expanded here,
// where the matched 0x140b20 keeps it a call with the same helper shape).
// M3 lane 14 r131 (fast with 0x2605d0, 0x2613d0, 0x2600a0, all MATCH; fast CAVEATs this function; control 440): the
// written-out first evaluator pass in a block of its own 440 (same code); the two flags in one declaration 440 (same
// code); the sort scratch in a block of its own with the first pass in its own block 439 (frame 0x8a4), with `possible`
// declared ahead of the flags 439; `__forceinline` on 0x262a30's definition 499 (3191 B: 0x13e2d0 expands with it, as
// with `inline`) and 0x2623a0 65 -> 182 (diagnostic, restored at once).
// M3 lane 1 r137 (H2M_FAST with 0x2605d0/0x2613d0 MATCH, 0x1af160 12): the two flags declared and cleared just before the
// main loop 772 (3081 bytes, frame 0x8a4: they then pack into one dword at retail's 0x6/0x7, swapped, and the offset and
// best score take retail's homes; the late clears are positional misses); the sort scratch word in its block 440 (same
// code); `possible` declared with the flags at the top 491 (3113 bytes); the flags declared after the sort scratch 441;
// that with selected and best score assigned after them 440. Retail's low frame: a temporary at 0x0, the flags at 0x6/0x7,
// the result at 0x8; ours a flag at 0x3, the temporary at 0x4, the other flag at 0xb.
// M3 lane 6 r211 (fast with 0x2605d0/0x2613d0/0x2600a0/0x1af160/0x1e1f20; all matched but 0x1af160, which stays 12;
// control 440): re-measured the sort scratch in a block of its own around the sort call 439 (frame 0x8a4); on it the two
// flags in one declaration 439 (same code), declared after the best score 440, the evaluator loop's flag declared at the
// function's top and set before the loop 439 (same code); on main's body the sorted index as `long index =
// sorted_indices[i]` 619 (3085 bytes). The flags still take a dword each (+3 and +0xb against retail's +6/+7).
// M3 lane 12 r163 (H2M_FAST with 0x2605d0/0x2613d0 MATCH; control 440): the range read once into `range` for the limit
// test as well (main mixes the local and a field re-read) 810 (3113 B); the two flags' declarations exchanged 440 (same
// code); `within_limit` alone declared just before the scoring loop 758 (3065 B), `finite_distance` alone there 615
// (3081 B); the sort scratch declared at the top ahead of the result 440 (same code). Body unchanged.
// @retail 0x260670
s_reference __stdcall function_260670(long actor_index, s_2605d0_request const *request,
	s_261d20_entry *entries, short count, long reported_entry, long reported_owner, byte *scratch, bool *has_path)
{
	byte *actor = (byte *)actor_get(actor_index);
	s_type_967e20 *context = (s_type_967e20 *)request;
	byte *parameters = (byte *)context;
	s_type_b36ac5 *positions = (s_type_b36ac5 *)entries;
	s_reference result = g_470fa0;
	bool within_limit = false;
	bool finite_distance = false;
	short selected = NONE;
	real best_score = 0.0f;
	s_path_source source;
	s_path_settings settings;
	long sorted_indices[512];
	long sort_scratch;
	*has_path = false;
	((byte *)g_4f55d0)[3] = true;
	if (count == 0)
	{
		byte *history_actor = (byte *)actor_get(actor_index);
		*(short *)(history_actor + 0x3fe) = 3;
		byte *history = history_actor + 0x402;
		long remaining = 4;
		do
		{
			*(s_reference *)history = g_470fa0;
			history += 6;
		} while (--remaining);
		return g_470fa0;
	}
	if (parameters[0x618] && parameters[0x53])
	{
		if (parameters[0x56])
		{
			for (short i = 0; i < count; i++)
				function_260530((s_reference_candidate_view *)&positions[i], (s_reference_direction_request *)parameters);
		}
		else if (*(long *)(parameters + 0x64c) != NONE)
		{
			memset(&source, 0, sizeof(source));
			source.radius = *(real *)((byte *)function_1e4a50(*(long *)(actor + 0x54)) + 4);
			source.unknown04 = actor[0x3e4];
			source.object_index = NONE;
			source.unknown0c = NONE;
			function_270590(&source, (s_type_c3b527 *)(parameters + 0x650), *(long *)(parameters + 0x64c));
			source.unknown4c = 0.0f;
			source.unknown45 = true;
			source.unknown48 = 20.0f;
			function_1f9240(actor_index, &settings);
			byte *buffer = ai_scratch_buffer_get();
			function_271300((s_type_f17a25 *)buffer, NULL, &settings, &source, 0);
			function_2715a0(buffer);
			for (short i = 0; i < count; i++)
			{
				s_type_b36ac5 *position = &positions[i];
				if ((position->unknown58 && parameters[0x5a]) || (position->unknown59 && parameters[0x58]))
					function_260530((s_reference_candidate_view *)position, (s_reference_direction_request *)parameters);
				else
					function_270750(buffer, position->definition->unknown14, (s_actor_point_target *)position->definition,
						&position->unknown28, 0, parameters[0x54] ? (long)&position->unknown34 : 0);
			}
			ai_scratch_buffer_release(buffer);
		}
	}
	else if (parameters[0x54])
	{
		for (short i = 0; i < count; i++)
		{
			vector3d_from_points3d((point3f *)(parameters + 0x620), &positions[i].position, &positions[i].unknown34);
			function_30bf0(&positions[i].unknown34);
		}
	}
	if (parameters[0x55])
	{
		for (short i = 0; i < count; i++)
		{
			vector3d_from_points3d((point3f *)(parameters + 0x620), &positions[i].position, &positions[i].unknown40);
			normalize_inline(&positions[i].unknown40);
		}
	}
	if (!parameters[0x56])
	{
		function_1f90f0(actor_index, &source);
		source.unknown45 = true;
		source.unknown48 = *(real *)(parameters + 0x1c);
		if (parameters[0x46] && parameters[0x618])
		{
			long target_object = NONE;
			if (*(long *)(parameters + 0x664) != NONE)
				target_object = prop_ref_get(*(long *)(parameters + 0x664))->object_index;
			source.unknown28[0] = true;
			*(point3f *)(source.unknown28 + 4) = *(point3f *)(parameters + 0x620);
			*(long *)(source.unknown28 + 0x10) = target_object;
			*(real *)(source.unknown28 + 0x14) = *(real *)(parameters + 0x4c);
			source.unknown44 = !parameters[0x50];
			*(real *)(source.unknown28 + 0x18) = *(real *)(parameters + 0x48);
		}
		else if (*(short *)(actor + 0x358) > 0)
		{
			source.unknown28[0] = true;
			*(point3f *)(source.unknown28 + 4) = *(point3f *)(actor + 0x370);
			*(real *)(source.unknown28 + 0x14) = *(real *)(actor + 0x36c);
			*(long *)(source.unknown28 + 0x10) = *(long *)(actor + 0x360);
			source.unknown44 = true;
			*(real *)(source.unknown28 + 0x18) = 10.0f;
		}
		function_1f9240(actor_index, &settings);
		function_271300((s_type_f17a25 *)scratch, NULL, &settings, &source, 0);
		if (function_2715a0(scratch))
			*has_path = true;
	}
	for (short i = 0; i < count; i++)
	{
		s_type_b36ac5 *position = &positions[i];
		if (parameters[0x618])
		{
			vector3f delta;
			vector3d_from_points3d((point3f *)(parameters + 0x620), &position->position, &delta);
			position->unknown30 = dot3f(&delta, &delta);
		}
		vector3f delta;
		vector3d_from_points3d((point3f *)(actor + 0x238), &position->position, &delta);
		real distance_squared = dot3f(&delta, &delta);
		real range = *(real *)(parameters + 0x1c);
		if (range * range > distance_squared)
		{
			if (!parameters[0x56] && !(position->unknown58 && parameters[0x5a]) && !(position->unknown59 && parameters[0x58]))
				function_270750(scratch, position->definition->unknown14, (s_actor_point_target *)position->definition,
					&position->unknown18, (long)&position->unknown2c, parameters[0x51] ? (long)&position->unknown1c : 0);
			else
			{
				position->unknown2c = (real)sqrt((double)function_11e000((point3f *)(parameters + 0x620), (point3f *)(actor + 0x238), &delta));
				real length = normalize3d(&delta);
				position->unknown18 = length;
				if (parameters[0x51])
					position->unknown1c = delta;
			}
		}
		if (*(real *)(parameters + 0x1c) > position->unknown18)
			within_limit = true;
		else
		{
			position->unknown4c = false;
			if (FLT_MAX > position->unknown18)
				finite_distance = true;
		}
	}
	if (!within_limit && !finite_distance && !parameters[0x56] && !actor[0x264] &&
		(*(long *)(actor + 0x28c) == NONE || (*has_path && count > 0 && *(short *)(scratch + 0xae) < 8)))
		++*(short *)(actor + 0x5e8);
	else
		*(short *)(actor + 0x5e8) = 0;
	if (parameters[0x15] && !within_limit)
	{
		selected = random_index((dword *)g_4e7408, count);
		*has_path = false;
		byte *history_actor = (byte *)actor_get(actor_index);
		*(short *)(history_actor + 0x3fe) = 3;
		byte *history = history_actor + 0x402;
		long remaining = 4;
		do
		{
			*(s_reference *)history = g_470fa0;
			history += 6;
		} while (--remaining);
		if (!function_260160(actor_index, (s_prop_search *)context, &entries[selected]))
			return result;
	}
	else
	{
		function_260060(actor_index, count, context, positions);
		for (short i = 0; i < count; i++)
			sorted_indices[i] = i;
		g_51ec9c = count;
		g_51eca0 = positions;
		sort_4byte(&sort_scratch, sorted_indices, count, (t_reference_compare)function_2600e0, NULL);
		context->unknown680 = 0.0f;
		bool possible = true;
		for (long evaluator = 0; g_44adf0[evaluator].proc && possible; evaluator++)
		{
			if ((1 << context->type) & g_44adf0[evaluator].flags)
				possible = g_44adf0[evaluator].proc(actor_index, context, NULL);
		}
		parameters[0x67c] = possible;
		for (short i = 0; i < count; i++)
		{
			short index = (short)sorted_indices[i];
			s_type_b36ac5 *position = &positions[index];
			if (!position->unknown4c || (parameters[0x67c] && best_score >= position->score + context->unknown680))
				break;
			if (parameters[0x618])
				function_25f7b0(actor_index, context, position);
			position->unknown50 = position->score;
			if (function_2600a0(actor_index, context, position) && position->score > best_score)
			{
				selected = index;
				best_score = position->score;
			}
		}
		if (selected == NONE)
			return result;
	}
	if (reported_entry)
		memcpy((void *)reported_entry, &positions[selected], sizeof(s_type_b36ac5));
	result = positions[selected].reference;
	if (reported_owner)
	{
		long owner = function_262a30(result);
		*(long *)reported_owner = owner != actor_index ? owner : NONE;
	}
	if (actor[0x3f2] && !actor[0x220] && !positions[selected].unknown5b)
		actor[0x3f2] = false;
	if (parameters[0x56])
	{
		function_1f90f0(actor_index, &source);
		s_type_f17a25 *path = (s_type_f17a25 *)scratch;
		path->unknown54 = false;
		path->unknown90 = NONE;
		path->unknownae = 0;
		path->heap_count = 0;
		path->location.unknown00 = 0;
		path->location.unknown02 = 0;
		path->pathfinding = *(long *)((byte *)g_4e0348 + 0xc4) > 0 ? *(void **)((byte *)g_4e0348 + 0xc8) : NULL;
		path->flags = 0;
		path->unknownac = false;
		path->source = source;
		memset(&path->settings, 0, sizeof(path->settings));
		path->unknown14188 = 0;
		memcpy(path->unknown140d4, parameters + 0x6a0, 0xb6);
		*has_path = true;
	}
	return result;
}
