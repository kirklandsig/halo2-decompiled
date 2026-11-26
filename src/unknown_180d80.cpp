#include "unknown_11c920.h"
#include "globals.h"
#include "physical_memory.h"
#include "normalize3d.h"
#include <string.h>
// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_cluster_query
{
    byte unknown00[8];
    point3f point;
    byte unknown14[0xc];
    short cluster_index;
};
struct s_180d80
{
    point3f field_0;
    real field_c;
    real field_10;
};
struct s_180d81
{
    s_180d80 field_0[1024];
    short field_5000;
    short field_5002[1024];
    short field_5802;
};
struct s_180d82
{
    point3f field_0;
    dword field_c;
};
struct s_180d83
{
    bool field_0;
    byte field_1[7];
    real field_8;
    real field_c;
    real field_10;
};
struct s_180d84
{
    byte field_0[0x8c];
    real field_8c;
    real field_90;
    real field_94;
    real field_98;
    real field_9c;
    real field_a0;
    byte field_a4[0xec - 0xa4];
    real field_ec;
    real field_f0;
    real field_f4;
    real field_f8;
    byte field_fc[4];
    byte field_100;
};
union s_180d85
{
    color3f field_0;
    vector3f field_c;
};
real function_30bf0(vector3f *arg_0);
void *function_439d0(long arg_0);
bool __stdcall function_d47d0(s_cluster_query const *arg_0, color3f *arg_1);
color3f *function_131c20(color3f *arg_4, color3f const *arg_0, color3f const *arg_1, dword arg_2, real arg_3);
dword __cdecl pack_color3f(color3f const *arg_0);
long function_17cfa0(long definition_index, long first_index, short cluster_index, short decal_type, bool permanent);
void __stdcall function_180b60(s_cluster_query const *arg_0, real arg_1, long arg_2);
dword vector3d_pack(vector3f const *arg_0);

__forceinline void function_180d81(real const *arg_0, real const *arg_1, real *arg_2)
{
    real const volatile *local_0 = arg_0;
    real const volatile *local_1 = arg_1;
    if ((local_0[1] - local_0[0]) * *local_1 + local_0[0] < 0.0f) *arg_2 = 0.0f;
    else if ((local_0[1] - local_0[0]) * *local_1 + local_0[0] > 1.0f) *arg_2 = 1.0f;
    else *arg_2 = (local_0[1] - local_0[0]) * *local_1 + local_0[0];
}
__forceinline real function_180d82(void)
{
    g_4e7408->seed = g_4e7408->seed * 0x19660d + 0x3c6ef35f;
    return (real)(g_4e7408->seed >> 16) * 1.5259021893143654e-05f;
}
__forceinline void function_180d83(transform4x3f const *arg_0, point3f *arg_1)
{
    real local_0 = arg_1->x;
    real local_1 = arg_1->y;
    real local_2 = arg_1->z;
    real local_4;
    if (arg_0->scale != 1.0f)
    {
        local_0 *= arg_0->scale;
        local_1 *= arg_0->scale;
        local_2 *= arg_0->scale;
    }
    local_4 = arg_0->up.i * local_2;
    local_4 += local_1 * arg_0->left.i;
    local_4 += arg_0->forward.i * local_0;
    arg_1->x = local_4 + arg_0->position.x;
    local_4 = arg_0->forward.j * local_0;
    local_4 += arg_0->up.j * local_2;
    local_4 += arg_0->left.j * local_1;
    arg_1->y = local_4 + arg_0->position.y;
    local_4 = arg_0->forward.k * local_0;
    local_4 += arg_0->up.k * local_2;
    local_4 += arg_0->left.k * local_1;
    arg_1->z = local_4 + arg_0->position.z;
}

// score 91 (was 91; 93 earlier): r176 baseline 91 (fast and full); the 0x17cfa0 call setup now agrees with retail. Left: the entry
// guard's registers, the count loop's movsx (retail tests the word and zero-extends), the count-sum/vertex-index slot pair
// and the inlined point transform's x/y row product forms (24 lines over the four copies).
// MiniMax C (11-19): no new form, no lower score (93): the entry guard's registers, the count loop's extension and the
// 0x17cfa0 call setup all still differ, and the callee that decides the last one lives in another file; the body stands.
// MiniMax E (11-20): no new form, no lower score (93): the entry guard's registers, the count loop's extension and the 0x17cfa0 call setup still differ, and the callee that decides the last one is in another file and does not match yet: BLOCKED-BY 0x17cfa0.
// Earlier first line: score 93 (was 93): BLOCKED-BY 0x17cfa0 (contributor, unmatched: ours passes the NONE first index in ECX and the
// Round 127 (m3 lane 10, fast): dead initialised locals at the top for K = 8/10/12 216/216/261 and K = 14..32 (even)
// 93 each, the transform rows never flip (diagnostic, removed); the count sum as a `short` without its two casts 93
// (same code); with the short count-loop index as well 102 (aligned 36 against 38); the luminance in red, green, blue
// order 93 (same code), with the short index 102; the strip loop's vertex cursor as a `long` 367 (2659 bytes).
// definition in EAX, retail the other way round). Round 199 (m3 lane 5, fast): the short count-loop index re-measured 102
// (aligned 81, register-masked 36); dead initialised locals at the top, k = 1..7, 168/169/169/169/169/213/216 (diagnostic:
// the transform rows' operand sides never move, the field sums and the count slots do); the same on the short index
// 177/178/178/178/178/222/225; k = 1..4 inside the transform helper 169/169/213/216; the transform helper taking and
// returning the point by value 721 (2940 bytes).
// m3 lane 8 r195 (fast, control 93): retail stores the luminance into the packed vector only after the vertex loop's
// pretest (the preheader); the store moved into the loop body as its first statement 547, just before the pack call 509
// (both 2695 bytes: kept in xmm6 and stored every pass, never hoisted, and not hoisted either with the clamp helper's
// volatile reads removed, 680); the loop under `if (count > 0)` with the store inside 545 (the count is loaded into ax and
// tested); under the loop's own test `if (counter < count)` 91 (retail's preheader exactly, but a repeated test, kept only
// with a match); that with the luminance red, blue, green 89, with the short count index 100, all three 98; the x row
// accumulating the left product first 94 (retail's emission order, accumulator register swapped); dead initialised locals
// on the short index (k = 1..7) and inside the transform helper both change code, the rows never flip.
// MiniMax H (11-18): no new form, no lower score (93): retail's transform rows compute the up product first and keep
// each product's first operand in the destination register, where this build computes the left product first with the
// two operands the other way round; spelling the three products in the opposite order compiles to the same bytes, and
// giving each product a named local and summing them shortens the body by 16 bytes and scores 235. What still differs:
// the entry guard's registers (ours loads the strip record into ecx after the pushes, retail into eax before them), the
// count loop's sign extension where retail zero-extends the word, the swapped count sum and vertex cursor slots, and the
// 0x17cfa0 call setup (that callee's own body is in another file).
// MiniMax H (11-18): no new form, no lower score (93): the length agrees (2683 bytes against 2683) and what still
// differs is the entry guard's registers (ours loads the strip record into ecx after the saves, retail into eax
// before them), the count loop's extension (ours sign-extends the word into the counter, retail zero-extends it
// and counts down), the accumulator's slot (0x10 here, 0x14 in retail) and the 0x17cfa0 call setup. The count's
// extension cannot be changed here without a width cast on the compare or a copy of the field in a local of its
// own, which the rules drop: BLOCKED-BY 0x17cfa0, whose body is in another file.
// MiniMax I (11-16): no new form, no lower score (93): reading the third argument's point through a pointer to that
// argument scores 270 and was restored, and the permuter's 35 tries over the reduced image found nothing below 93; what
// still differs is the entry guard's registers (retail takes the fifth argument in eax before the saves, ours in ecx
// after them), the count loop's sign extension where retail zero-extends the word, and the swapped slots of the count
// sum and the vertex cursor.
// MiniMax S (11-15): no new form, no lower score (93): the three field sums spelled as a load and an accumulate
// give 290 (the body then loses 21 bytes and every row moves), the same sums spelled the other way round compile to
// the same code, the luminance red, blue, green gives 91, a gain under the review threshold, and the vertex cursor
// declared in the outer scope gives 496; all were restored.
// What still differs: retail loads the fifth argument into eax before the register saves and keeps it there, ours
// loads it into ecx after the saves; the count loop tests the word and zero-extends it, ours sign-extends; the count
// sum and the vertex cursor take the opposite stack slots; retail's transform rows multiply the x term as up.i*z,
// y*left.i and forward.i*x, ours left.i*y, z*up.i and forward.i*x; retail calls 0x17cfa0 with the first index in eax
// and the definition index in ecx where ours passes them the other way round (that callee is in
// src/unknown_17d2a0.cpp); the global and float-pool addresses differ by the whole-image layout.
// MiniMax C (11-15): no new form, no lower score (93): the body stands as found.
// MiniMax D (11-14): no new form, no lower score (93): retail loads the fifth argument into eax before the
// register saves and calls 0x17cfa0 with the first index in eax, ours loads the strip record into ecx after
// the saves and passes that index in ecx; the callee's own body is in another file, so the gap stays there.
// The permuter's 33 annealed tries over the reduced image found nothing below the 93 baseline.
// MiniMax G (11-14): no new form, no lower score (93): the strip-count index spelled as an unsigned short gives
// retail's prologue and count loop but scores 103 (the three field sums then load the lower field first and the
// 0x17cfa0 register roles turn), the same loop with the sums written the other way round gives 102, and a named
// short strip count read at the entry gives 168; all three were restored. What still differs: the count sum and the
// vertex cursor take the opposite stack slots, and the 0x17cfa0 call still passes the first index in ecx where retail
// passes it in eax (that callee lives in another file).
// MiniMax R run 92 (11-17): no new form, no lower score (93): retail's transform rows multiply up.i*z first and keep
// the second product in the vector's register, where this build computes the second product first and forms the first
// one the other way round; that middle product written with the matrix term first and the row written as an
// assignment instead of an accumulation both measure 93 (the same code) and the row written as one expression
// measures 298, so the body stands; the permuter's run over the reduced image did not finish in its window and was
// stopped; what still differs is the entry guard's registers, the count loop's sign extension, the swapped count and
// vertex slots and the 0x17cfa0 call's argument registers (that callee lives in another file).
// MiniMax R (11-17): no new form, no lower score (93): the permuter's annealed tries over the reduced image found
// nothing below 93, so the body stands; what still differs is that retail loads the fifth argument into eax before
// the register saves where this build loads the strip record into ecx after them, the count loop sign-extends the
// word where retail tests it and zero-extends, the count sum and the vertex cursor take the opposite stack slots,
// and the transform's x row computes left.i*y before up.i*z where retail computes up.i*z before y*left.i; the call to
// 0x17cfa0 that passes the first index in ecx where retail passes it in eax belongs to that callee's own file.
// Earlier 93 (was 93): frame 0x4040 agrees; left are the entry guard's registers (ours loads the strip record into
// ecx after the pushes, retail into eax before them), the count loop's movsx where retail tests the word and
// zero-extends, the count/counter slots swapped, the 0x17cfa0 call setup (BLOCKED-BY 0x17cfa0, a contributor's)
// and the transform rows' product forms. Earlier rounds below.
// MiniMax S (11-14): no new form, no lower score (93): the strip-count index spelled short gives retail's
// prologue and count loop but scores 102 (the three field sums then load the lower field first and the two call
// setups turn), and the luminance spelled red, blue, green emits retail's green, blue, red order for 91, a gain
// under the review threshold, so it was reverted and the original body stands; retail calls 0x17cfa0 with the
// first index in eax and the definition index in ecx where ours passes them the other way round, but our
// declared order already matches retail's source order, so that gap is the callee's own body in another file and
// swapping the arguments here would break the call. The count sum and the vertex cursor still take the opposite
// stack slots, the four inlined transform copies still compute the x row's first product in the opposite order,
// and the entry guard still gives the strip pointer to ecx after the register saves where retail loads it into
// eax before them. The permuter's 27 annealed tries over the reduced image found nothing below the 93 baseline.
// MiniMax H (11-13): no new form, no lower score (93): the entry guard still loads the strip pointer after the register
// saves and into ecx where retail loads it before them into eax, and the call to 0x17cfa0 still passes the first index in
// ecx where retail passes it in eax; that callee lives in another file.
// MiniMax E (11-11): no new form, no lower score (93); the call to 0x17cfa0 still passes the first index in ECX
// where retail passes it in EAX, and that order belongs to the callee's own body in another lane's file, so the
// gap here is a contributor's; the frame, the guard's three tests and the body length agree.
// MiniMax G (11-11): no new form, no lower score (93): the first difference is at +0xa, in the prologue - retail loads the fifth argument into eax before the pushes and keeps it there, ours loads it into ecx after the pushes and puts the cluster query in eax; the guard's three tests and the body length agree (2683 bytes both), and the guard written as three early returns scores 648 and is reverted.
// MiniMax G (11-12): no new form, no lower score (93): the entry guard still loads the strip pointer after the register saves and into
// ecx where retail loads it before them into eax; the guard as one positive test scores 121, a local copy of the strip pointer 168, and
// the permuter's 22 annealed tries over the reduced image found nothing below the 93 baseline.
#pragma inline_depth(1)
// MiniMax E (11-16): no new form kept, the score is still 93: the guard's first index travels on the stack where retail keeps it in eax, and the callee that takes it, 0x17cfa0, has its parameter order in another file (BLOCKED-BY 0x17cfa0); the luminance terms in red, blue, green order score 91 and were restored (a gain of 2 is below the review bar), the guard with the cluster test first scores 650 and the count field as unsigned scores 149, so the body stands.
// Earlier at 93 (was 94): frame 0x4040 agrees; early mesh receiver, count/counter homes,
// MiniMax I (11-10): no new form, no lower score (93); the count sum and the vertex cursor still take the
// opposite stack slots, the three field sums still load the lower field first where retail loads the higher
// and adds the lower (MSVC canonicalises the spelling either way), and the transform x row's product order
// is not steerable by the source order; a short strip counter reproduces retail's prologue exactly but
// scores 102 and was restored, and casting the strip count to unsigned short does reproduce retail's movzx
// of the count but then widens the guard's signed test as well (93, restored), as an unsigned short local
// for the count (168).
// Earlier at 94 (was 94): frame 0x4040 agrees; early mesh receiver, count/counter homes,
// creation-call setup and transform scratch ranks differ.
// MiniMax D (11-09): no new form, no lower score (94); the sixth argument is still read into ECX after the
// prologue's pushes where retail reads it into EAX before them, and the third argument is still kept in
// EAX where retail keeps it in ECX, so the whole body sits two instructions off; copying the sixth
// argument into a local of its own scores 287 and was reverted.
// MiniMax S (11-09): still differs in the count loop: retail tests the count with a signed compare and then
// widens it without sign, ours keeps the count sign-extended; retail also reads arg-5 before the prologue's
// pushes and holds arg-3 in ecx, ours reads arg-5 into ecx after the pushes. The count's spelling was permuted
// for 600 s and nothing came below the current score.
// MiniMax E (11-05): no new form, no lower score; a named local for the guard's struct pointer
// scores 287 (the load moves ahead of the callee-saved pushes and the frame shrinks to 2663 bytes),
// so it is reverted.
// MiniMax B (11-03): no new form, no lower score. The entry still loads the sixth parameter into ECX
// and the fourth into EAX against retail's EAX for the sixth and ECX for the fourth (the two stack
// homes agree, so this is the allocator's choice, not the parameter order); the guard split into three
// separate returns gives 650 and the negated guard compiles identically, so both were reverted.
// MiniMax S (11-06): no new form, no lower score.
// M3 lane 9 r144 (fast with 0x180b60/0xd47d0/0x131c20/0x30bf0, control 93): the short count-loop index again gives
// retail's prologue (the guard's record in EAX before the saves) and count loop, 102 (first difference +0x29); on it the
// three centre sums written lower field first 102 (same code), the luminance written red, blue, green 100, both 100.
// The count sum and the vertex index renamed (quad_count/vertex_index, strip_quads/point_index: both name orders) 93
// each: their swapped frame homes do not follow the names.
// Earlier: BLOCKED-BY 0x17cfa0 (its first index travels on the stack, retail's in
// MiniMax R (11-01): no new form, no lower score (the permuter's 56 tries left it at 94; the
// prologue differs only in the eax/ecx swap of the input pointer, and two 4-byte slots are swapped).
// BLOCKED-BY 0x17cfa0 (its first index travels on the stack, retail's in
// eax: `push -1` for `mov eax, -1` shifts every later offset by 3 bytes). A short strip-loop
// index gives retail's prologue and count loop but 547 (the loop head's padding grows to a
// jmp); local names for the count and vertex index 94; vertex index declared first 547;
// luminance terms red, blue, green 92 (other orders 94-95, grouped 290); transform rows
// with the left product first 93, operands swapped 94, one expression per row 317;
// luminance and transform changes together 91.
// early input receiver, opposite count/counter homes and
// transform carriers differ; strip equality 100, short equality 548, vertex
// equality 95, both 548, combined count rejection 647; transform references
// and packed-record memcpy 94, separate transform input/output 170.
// early input register and count homes remain; normalizer inline 94; pack noinline 94; buffer inline 94; transform inline 94; nesting depth two 94; allocation byte count 169; published short count 169; short count and inline 169.
// Packed vertex published after position/delta/squared-distance/maximum 589/568/572/574; same with
// output record receiver 568/581/554/569. Original publication and receivers restored.
// Shared vertex ordinal with output base/offset 284/293; shared short/long strip ordinal 528/418;
// base/strip and offset/strip combinations 531/405/533/415. Restored.
// Round 418: result before array 631; result after array 631; result after guard 651; result assigned at
// exits 106; short count/result before array 648; short count/result after array 648; short count/result
// after guard 668; short count/result at exits 546. early mesh receiver, count/counter homes and
// creation-call setup remain.
// Typed tag/output/both 94/94/94; complete direct tag raw/typed 661/661; complete direct output
// 608; both direct raw/typed 698/698. Original observations and record layouts restored.
// Round 51: count before tag/vector 95; count initialization split 94; unsigned long polygon counter 94;
// short counter with count before tag/vector 548; with split count 547; with count between tag/vector 547;
// with outer maximum distance 547; with both count placement and outer distance 548. Original restored.
// Round 41: 0x17cfa0 is still todo (score 70) in a contributor's file, so the block stands. Transform helper:
// accumulator declared first 94; scale as `scale * x` 94; one accumulator per row 170; separate input and
// output pointers (in place at the four calls) 170 (the x row's products stay swapped: our copy-first `z * up.i`
// where retail loads up.i first, in all four inlined copies). Count and vertex-index locals renamed
// quad_count/vertex_index 94, count alone 94 (their two slots stay swapped).
// Round 55 (fast): count and vertex counter renamed a_/z_ either way and with the names exchanged 94/94/94
// (the spill slots, ours count 0x10 and counter 0x14, retail the other way, do not follow names); the vertex
// counter declared at the top of the block 94; the count declared just before its loop 624.
// Round 71 (lane 12, fast): retail pairs the luminance-loop counter with the strip loop's first short in one slot
// ([esp+0x10]) and the count in the next ([esp+0x14]); ours pairs the count first. Names and the counter's
// declaration position do not move those slots (compile-only /FAs screen). The short strip-count index re-measured
// 547 (retail's prologue and count loop; the loop head pad before the luminance loop becomes `jmp` + `lea`, with
// 0x17cfa0's call still 3 bytes short); the counter declared in its `for` 145, with the short index 567. Transform
// rows: the y row's left product spelled `local_1 * left.j`, the x row's `left.i * local_1`, both, the x row's up
// product `local_2 * up.i`, all three 94 each (VC reorders the operands itself: retail keeps `up.i * z` and
// `y * left.i` in the x row and `left.j * y` in the y row, ours the reverse in each); the point copied z, y, x 104,
// y, x, z 102.
// Round 278: standard lower/upper/distance extrema subsets 1-7
// 698/694/642/522/683/628/636; complete graph with short count 635. Original helpers restored.
// P255: Existing interpolation provider with one blend/one publication/lower-bound capture/
// ordinary each-use inputs 687/690/697/687; with emission count-pointer traversal
// 659/678/664/659. Original helper and body restored; early mesh/count homes, creation
// call and transform ranks remain. Earlier four-corner loop plan excluded before any trial.
// Round 65 (m3 lane 10, fast; 0x17cfa0 still 70): the entry test as three guard returns 650 (2665 B); the luminance
// counter declared in its for 145; the count loop index renamed apart from the later local_5 94; the scale declared
// before the maximum distance 97; the colour scale stored before the counter declaration 145.
// Round 77 (m3 lane 16, fast): the entry guard as a positive `if` around the body 122 (2664 bytes); the vertex
// array declared inside the block after the guard 94 (same code); the maximum as one `first || greater` test 115;
// the clamp helper without its volatile pointers (one named value) 687 (2551 bytes: the volatile reads are
// load-bearing); the strip-count loop counter renamed 94 (same code). Still BLOCKED-BY 0x17cfa0 (contributor).
// Round 141 (KIMI1, fast): split vertex/count declarations 94/94; descending short countdown on the new count
// graph 165; outer maximum, early allocation handle and post-guard array on that graph 165 each; existing
// scale/rotation/addition helpers in a plain corner loop 944 (2945 bytes), with the other matrix provider 784
// (2821 bytes). Original executable body and helpers restored; entry/count homes and creation-call setup remain.
// Round 55 (m3 lane 15, fast; aligned counts from an instruction diff): x row's first two products in one expression 299 (2658 bytes), with `(real)` casts on
// each 294 (2663), with the left product spelled `left.i * y` 294 (2663); x and y rows one cast expression each 289 (2663); y row's first two in one expression 292
// (2666); a named local for the x row's y product 298 (2661). Rows: retail has one copy-first product (`y * left.i`, x row); ours two (`z * up.i`, x row, and
// `y * left.j`, y row), and these spellings change neither, while their length change moves the whole listing. The short strip index re-measured 547 (aligned 85
// against 67 now: retail's prologue, but the field sums flip to `field_8c + field_90` and the loop head gains a jmp). A compile-only build gives xmm4/xmm5 for the
// rows, retail and the link xmm3/xmm4, so screen rows only in a real build. BLOCKED-BY 0x17cfa0 (contributor, still 70).
// AC557: Complete transform scale observed once, mutable/immutable at late/early
// input boundaries, then each with a named scale test: 227/227/379/379/317/317/392/392.
// NOT kept: all regress and shorten away from retail; original transform inputs,
// field reads, interpolation qualifiers and complete body restored.
// F399: Existing interpolation endpoint/sample/output reference contracts, seven
// subsets and complete output-reference return: eight forms 94 each.
// NOT kept: no gain or equality; original helper, both calls and whole body restored.
// Frame 0x4040 agrees; early mesh, count homes, creation setup and transform ranks remain.
// Round 159 (m3 lane 6, fast; aligned counts from an instruction diff, 64 for the body as it stands): the short
// strip-count index 547 (aligned 85); with the three field sums spelled lower field first 547 (aligned 82, the sums
// compile the same either way); plus luminance terms red, blue, green 545 (aligned 80); the luminance counter
// reused as the strip loop's counter 293 (2663 bytes), as the strip loop's vertex base 284 (2667 bytes); the count
// loop's bound compared as an unsigned word 94 (aligned 66). Still BLOCKED-BY 0x17cfa0 (contributor, unmatched).
// Round 62 (m3 lane 2, fast): the luminance stored into the packed vector's k inside the vertex loop (retail's store
// sits after the loop's pretest) 546 (held in xmm6 and stored every pass, not hoisted); a named short strip count read
// once for the guard and both strip loops 649 (2703 bytes); the vertex source fields read at each use without the
// element pointer 94 (same code); the luminance as one conditional expression 593 (2663 bytes); the squared distance
// through the existing vector3d_from_points3d and length_sq3f 507 (2697 bytes). Still BLOCKED-BY 0x17cfa0.
// Round 86 (m3 lane 11, fast; the header read 94 (was 94), the baseline full check gives 93): the short count-loop index
// re-measured 102 (aligned 81, register-masked 46 against the body's 63/48: with 0x17cfa0 now taking both indices in
// registers it gives retail's prologue and count loop with no pad change; left are the three field sums loading the lower
// field first, the count/counter slots [esp+0x10]/[esp+0x14] swapped, a scratch rotation at the 0x13d370 and 0x17cfa0 call
// setups and the transform rows). Slot pairs: both builds give the strip loop's base 0x10 and its strip length 0x14;
// retail pairs the count with the length and the vertex counter with the base, ours the other way. New forms: the vector
// cleared by one chained assignment (short index) 104; the index as a word 590; a named pointer to the current output
// vertex for its seven uses 577 on both index forms (2690 bytes); the allocation size as `(short)count * 4 *
// sizeof(vertex)` 93 / 102 (same code); the transform helper's point read as one aggregate copy 543 (2779 bytes).
// Re-measures: the index renamed, the count renamed to sort last or first, 102 each (slots do not follow names).
// Still BLOCKED-BY 0x17cfa0: ours passes the NONE first index in ECX and the definition in EAX, retail the other way
// round (its declared order already matches retail's last-to-first setup, so this is the callee's own body, a
// contributor's).
// MiniMax D (11-11): no new form, no lower score (93): the entry guard still gives the `s_180d81` pointer to ECX after
// the register saves and the cluster-query pointer to EAX, where retail gives them the other way round; splitting the
// guard into two tests scored 633 and a local copy of the pointer scored 168, and both were reverted.
// m3 lane 12 r122 (fast; the reduced image warns that callee 0x439d0 builds differently there): the luminance
// terms red, blue, green 91 (VC emits a three-term sum last term first, so this gives retail's green, blue, red
// loads; gain two, not kept); with it the short count-loop index 100 (retail's prologue and count loop, the three
// field sums flip to the lower field first); that with the sums spelled lower field first 100 (same code); the
// luminance order with the transform x row accumulating the left product first 92 (retail's emission order and
// load forms for the row, the accumulator register swapped); the luminance order with the x row's first two
// products in one expression 296 (2658 bytes).
// M3 lane 15 r95 (fast; the plain baseline is 93: 28 of its lines are global, call and float-pool addresses, 24 the
// transform rows): the four transforms through the existing out-of-line transform4x3f_apply_point (0x142570; it stays
// a call) 1352 (1929 bytes); the transform rows each as one expression with `x = scale * x`, the spelling of 0x142570's
// own body, 316 (2647 bytes); the scale written `scale * x` and the locals named x, y, z with a `sum` accumulator 93
// (same code); the scale read once into a const local 226 (2657 bytes); the point read in an if/else of the scale test
// 305 (2663 bytes); the test written `1.0f != scale` 98; the four locals in one declaration 93 (same code). Retail's
// copies keep each product in its spelled operand order (`up.i * z` loaded, `y * left.i` copied, `left.j * y` loaded);
// ours commutes those three whatever the spelling. Still BLOCKED-BY 0x17cfa0 for the call setup.
// m3 lane 14 r108 (fast with 0x17cfa0 105, 0x13d370 201, 0x439d0 5, 0x180b60 MATCH; control 93, 63 aligned rows by sequence alignment with
// addresses masked): the short count-loop index re-measured 102 (aligned 81; first difference moves from +0xa to +0x29: retail's prologue and
// count loop, then the three field sums load the lower field first, the count sum and vertex counter keep the opposite slots, the 0x13d370 and
// 0x17cfa0 setups rotate EAX/ECX/EDX and the loop pad is `mov edi, edi` for retail's nop). New on it: one short index declared ahead of the count
// loop and shared with the strip loop 292 (2667 B, aligned 117); shared with the vertex loop instead 292 (2667 B, aligned 117); the short index
// declared first in the block 102 (same code); the short count loop as a `while` 102 (same code); a pointer cursor over the strip lengths with a
// short countdown 603 (2663 B, aligned 110). The short index is retail's by its prologue and loop, but nothing found turns the field sums back.
// P604: Forward strip-length cursor with long/short/word ordinals, long/short remaining
// counts, the cursor bound before normalization, and checked-count reuse with a short
// ordinal/remaining count: 583/285/660/583/603/637/284/604. NOT kept: the entry load stays late and
// the cursor forms disturb the loop and frame slots.
// M3 lane 5 r214 (fast; control 93; the reduced image warns that callee 0x439d0 builds differently): the short
// count-loop index re-measured 102 (retail's prologue and `test bx, bx; movzx ebx, bx` loop; the centre sums flip to the
// lower field first, the count/counter slots stay swapped, the transform rows keep our operand order); on it dead longs
// K = 1..7 at the top 177/178/178/178/178/222/225 (one centre-sum row and the fourth transform copy change order, the
// first three copies do not), one dead long inside the block or after the count loop 177; the transform helper's point
// read z, y, x 114; the vertex counter as a long 585 (2697 bytes). Still BLOCKED-BY 0x17cfa0 for the call setup.
// M3 lane 11 r128 (fast with 0x180b60 MATCH; control 93, aligned 63 by a sequence-aligned diff, 38 with registers masked;
// row state = how many of the four inlined transforms have retail's x-row and y-row operand forms, 0 of 4 throughout):
// one to seven unused long locals re-measured 168/169/169/169/169/213/216 (aligned 87 to 97, the rows never move); live
// named float locals in the helpers, counted per expansion: a named result in the random-fraction helper 169, a named x
// output in the transform helper 169, x and y 169, x, y and z 213, the fraction result with x 169, with x and y 216, with
// all three 216 (the same states as the unused locals, rows unmoved); the transform helper's accumulator through a `real &`
// 307 (2689 B, aligned 327), for the x row only 298 (2669 B); one accumulator per row 169; the x row's left product in a
// named local 297 (2661 B); the two `+=` of the left terms written `local_4 = local_4 + ...` 93 (same code). Retail's rows
// keep every product in its spelled operand order and statement order; ours evaluates the x row's left product ahead of
// its up product and commutes three products, at every symbol count tried. Still BLOCKED-BY 0x17cfa0 for the call setup.
// m3 lane 4 r248 (h2m fast with 0x180b60, 0x17cfa0, 0x13d370, 0x439d0; control 91, aligned 55): the luminance terms red,
// green, blue 91 (same code); red, blue, green re-measured 89 (gain under 3, not kept); the three centre sums spelled
// lower field first 91 (same code); the vertex counter declared in its `for` after the luminance store 142, with the
// red, blue, green terms 140. Diagnostic: the clamp helper's two input pointers without `volatile` 694 (2551 B), so
// those reads are what give retail's per-test re-reads. Still BLOCKED-BY 0x17cfa0 for the call setup.
// M3 lane 12 r176 (h2m fast, control 91, 63 non-address lines): the short count-loop index again 102 (entry and count
// loop exact, the three centre sums and two call setups turn); that with a short count sum 102 (same code); the scale
// steps as `x = scale * x` 91 (same code); the transform rows as one expression each 314 (2647 B); one named sum per row
// 167 (rows unchanged, the rest moves). The rows never flip. Body unchanged.
// M3 lane 6 r231 (h2m fast, control 91; 0x17cfa0 and 0x180b60 MATCH): kept, the 1/256 scale through the existing
// vector3f_scale (same code). The three centre ranges typed as the existing real_bounds (hi + lo) 91 on the long index and
// 102 on the short index (same code each); the colour/vector overlay split into a colour and a separate vector 267 (2668 B:
// retail shares one slot); vector3f_scale on the short index 102 (same code); the luminance written constant-first in the
// green, blue, red order 91 and red, green, blue 91 (same code each), blue, green, red 92.
// M3 lane 6 r233 (h2m fast with 0x180b60 and 0x17cfa0, both MATCH; control 91): the vertex loop's input element pointer
// `*const` 91; the output pointer `*const` 91; the stored flag as a `(bool)` cast of the masked byte 91; the colour record
// declared at the top of the block 91 (same code each); the fourth strip index as `local_13 + (wrap ? 0 : local_16 + 2)`
// 417. Still BLOCKED-BY 0x17cfa0 for the call setup.
// M3 lane 12 r181 (h2m fast, control 91): the transform helper's rows as src/unknown_11c380.cpp spells its matched
// inline transform (x row left, forward, up term by term; y and z rows one expression each) 282 (2666 B); as
// src/unknown_1eff40.cpp spells it (three named x-row terms) 284 (2664 B); this x row with one-expression y and z rows
// in up, left, forward order 280 and in forward, up, left order 280 (2666 B each); this x row with the y and z rows term
// by term in up, left, forward order 140 (2683 B). The count sum and the vertex counter renamed to sort either way
// round 91/91/91 (same code: the slot pair ignores names).
// M3 lane 7 r224 (fast screen with 0x180b60 MATCH each run; control 91, aligned 61, register-masked 26). The transform
// rows do flip: a redundant parenthesis pair shifts the operand order of every product written after it by one step,
// each row is right at one step of four, and the pairs in the caller count ahead of all four inlined copies. One pair
// on a single helper product 289-295 (2659-2667 B); one, two, three pairs in the vertex loop's three distance terms
// 289/282/288 (the x row is retail's with three); three there plus one on the x row's forward product, or on the x
// row's stored sum, 75 at 2683 B (x and z rows exact in all four copies); plus both 287 (y row exact, z row off); plus
// the y row's left product, the y row's stored sum and the z row's forward product (eight pairs: 3 ahead, 2 between
// x and y, 3 between y and z) 52 at 2683 B, all four copies byte for byte; a doubled or tripled pair in one place
// counts the same (52). On the 52 form: luminance spelled red, blue, green 50; the vertex loop under its own test
// (lane 8 r195's form) 50; both 48 (aligned 28, masked 10); the short count-loop index 63 (first difference +0x9f:
// entry and count loop exact, the three centre sums turn, the 0x13d370 and 0x17cfa0 setups rotate), with the other
// two 59 (aligned 43, masked 14). On that 59 form: pairs round the centre sums in eleven placements 59 (the sums
// never turn; such a pair counts like a caller pair, and four or five ahead of the helper 301-306); the sums spelled
// lower field first 59 (same code), as three assignments then three adds 617, assign-add per component 306. Without
// pairs on the short index: one to seven unused locals (diagnostic) 173-221, sums unturned (their operands keep one
// rank pair at every symbol count, another with the long index); a named strip-count local for the guard and count
// loop 173, also for the strip loop 656, read just before the count loop 173; the same three on the 48 form
// 137/653/137. None kept: the pairs and the loop test are match-only forms. Left on the 48 form: the entry guard
// (retail loads the strip record into EAX ahead of the saves), the count loop's 16-bit test, the count-sum and
// vertex-counter slot pair, and the 0x13d370 argument's register; the short index gives the first two.
// M3 lane 7 r225 (H2M_FAST, on the 48 form): the count sum renamed (two names either side of the vertex counter's in
// sort order) 48 / 48; both renamed to role names 48 (names do not order this pair). Frame reading (diagnostic): the
// frame is over 0x80 bytes, so slots sort by reference density; the count sum shares the densest slot with the strip
// loop's short counter (11 references) and the vertex counter shares the next with a distance term (7), so the
// vertex counter needs the denser slot for retail's pair. On the short-index form (59) the three centre sums are the
// only change in the first 0x60 bytes; its call setups rotate as recorded above.
// M3 lane 7 r226 (fast screen with 0x180b60 MATCH each run; control 91, aligned 61). The slot pair is a shared variable:
// with the vertex loop's short counter reused as the strip loops' running base (no separate base local) the count sum
// and the strip length share [esp+0x14] and the counter/base take [esp+0x10], as retail; alone 281 (2667 B, the
// transform rows move one step), with the short count index 292, with the luminance red, blue, green 279, both 290,
// with the luminance and the vertex loop under its own test 276. Row model (read from the compiler's operand ranks):
// every matrix element load carries a count mod 4 taken where it is written, a product puts the vector component
// first only when its element's count is 0, a parenthesis pair adds one to everything written after it, the shared
// counter/base adds one, and the count loop's short index shared with the strip loop's index (or with the vertex
// counter) adds two; retail's rows are x (3, 0, 2), y (2, 3, 1), z (1, 3, 1) in written order. So the eight pairs of
// round 224 become seven on the shared counter/base (two in the distance terms, the five in the helper): long count
// index 40 (aligned 20: left are the entry guard, the count loop, the count's scratch register and the loop pad),
// short index 51 (aligned 35); with the short index declared once and shared with the strip loop the two caller pairs
// go as well (five helper pairs only) 51, shared with the vertex counter instead 51, declared once but not shared 98
// (51 with the two caller pairs). Left on the 51 forms: the three centre sums (lower field first), the scratch
// registers of the 0x13d370 and 0x17cfa0 setups and of the vertex loop, and the pad. The sums: each field load takes
// its count at the range test, the high field 2 and the low field 1 on the long index but 3 on the short one, and the
// sum loads the higher count first; a pair round the first test's difference turns the second sum only, round all
// three the second only; unchanged by the count sum declared ahead of the vector, the tests as `0.5f >= hi - lo` or
// `!(hi - lo > 0.5f)`, a short count sum, the definition pointer declared after the vector, the vector cleared by
// three assignments, one to four unused locals, a third use of either field. Helper forms on the shared-index form:
// one accumulator per row 293/166/290/282 for zero to three caller pairs; the rows as one expression each in forward,
// left, up order cannot fit at any step (its second and third elements share a count); two of the distance terms
// declared first and assigned in the loop 87 (same code as without). No single forced register or reload decision
// brings the call setups' scratch registers to retail's (100 tried). None kept: every form here needs the pairs
// (match-only) to score. Patches in m3-lane7 build/r226/lead-180d80-*.patch.
// M3 lane 5 r245 (h2m fast with 0x180b60, MATCH each run; control 91, sequence-aligned 61 rows exact, 27 with registers
// and stack offsets masked; body unchanged): the per-vertex maximum as one conditional assignment 112; the vertex
// record element through a pointer used for its stores and the distance terms 588 (2690 B); the time stamp stored
// ahead of the point copy 111; the definition's type word read once into a named short for both tests 166; the vertex
// loop's counter declared in the `for` after the luminance store 142 (masked aligned unchanged at 27).
// M3 lane 3 r315 (H2M_FAST with 0x180b60 and 0x17cfa0, both MATCH; control 91): the centre vector cleared by one chained
// assignment 93; the three range tests spelled `0.5f >= hi - lo` 91; the squared distance summed x, y, z 91; the luminance
// default declared after the colour/vector record 91; the count sum declared ahead of the centre vector 91 (same code
// each but the first). The transform rows are the shape of 0x142570, which retail expands here four times; its file is
// another lane's this round. Body unchanged.
// @retail 0x180d80
long __stdcall function_180d80(s_180d84 const *arg_0, transform4x3f const *arg_1, long arg_2, s_cluster_query const *arg_3, bool arg_4, s_180d81 const *arg_5, s_180d83 *arg_6)
{
    s_180d82 local_0[1024];
    if (arg_5->field_5802 <= 0 || arg_5->field_5000 <= 0 || arg_3->cluster_index == NONE) return NONE;
    {
        byte const *local_2 = (byte const *)g_4e3b44[arg_2 & 0xffff].data;
        vector3f local_3 = {0.0f, 0.0f, 0.0f};
        long local_4 = 0;
        if (arg_0->field_90 - arg_0->field_8c <= 0.5f &&
            arg_0->field_98 - arg_0->field_94 <= 0.5f &&
            arg_0->field_a0 - arg_0->field_9c <= 0.5f)
        {
            local_3.i = arg_0->field_90 + arg_0->field_8c;
            local_3.j = arg_0->field_98 + arg_0->field_94;
            local_3.k = arg_0->field_a0 + arg_0->field_9c;
            function_30bf0(&local_3);
            vector3f_scale(&local_3, 0.00390625f, &local_3);
        }
        for (long local_5 = 0; local_5 < arg_5->field_5802; local_5++)
            local_4 += (arg_5->field_5002[local_5] - 1) / 2;
        long local_5 = function_13d370((s_physical_object *)g_509448, (short)local_4 * 0x40, 1);
        if (local_5 != NONE)
        {
            long local_6 = function_17cfa0(local_5, NONE, arg_3->cluster_index, *(short *)(local_2 + 4), arg_4);
            if (local_6 != NONE)
            {
                byte *local_7 = g_4ea950->data + (local_6 & 0xffff) * 0x40;
                s_180d82 *local_8 = (s_180d82 *)function_439d0(local_5);
                if (local_8)
                {
                    real local_9 = 0.0f;
                    real local_10 = 1.0f;
                    s_180d85 local_11;
                    if ((*(short *)(local_2 + 4) == 0 || *(short *)(local_2 + 4) == 1) && function_d47d0(arg_3, &local_11.field_0))
                        local_10 = local_11.field_0.green * 0.5870000123977661f + local_11.field_0.blue * 0.11400000005960464f + local_11.field_0.red * 0.29899999499320984f;
                    short local_12 = 0;
                    local_11.field_c.k = local_10;
                    for (; local_12 < arg_5->field_5000; local_12++)
                    {
                        s_180d80 const *local_13 = &arg_5->field_0[local_12];
                        function_180d81(&arg_0->field_ec, &local_13->field_c, &local_11.field_c.i);
                        function_180d81(&arg_0->field_f4, &local_13->field_10, &local_11.field_c.j);
                        local_0[local_12].field_c = vector3d_pack(&local_11.field_c);
                        local_0[local_12].field_0.x = local_13->field_0.x + local_3.i;
                        local_0[local_12].field_0.y = local_13->field_0.y + local_3.j;
                        local_0[local_12].field_0.z = local_13->field_0.z + local_3.k;
                        real local_17 = local_0[local_12].field_0.x - arg_3->point.x;
                        real local_15 = local_0[local_12].field_0.y - arg_3->point.y;
                        real local_16 = local_0[local_12].field_0.z - arg_3->point.z;
                        real local_18 = local_16 * local_16 + local_15 * local_15 + local_17 * local_17;
                        if (local_12 == 0) local_9 = local_18;
                        else if (local_18 > local_9) local_9 = local_18;
                    }
                    *(point3f *)(local_7 + 0xc) = arg_3->point;
                    *(long *)(local_7 + 0x18) = g_510c54->game_time;
                    local_7[0x29] = arg_0->field_100;
                    if (!arg_6->field_0)
                    {
                        arg_6->field_c = function_180d82();
                        arg_6->field_10 = function_180d82();
                    }
                    *(real *)(local_7 + 0x1c) = (*(real *)(local_2 + 0x38) - *(real *)(local_2 + 0x34)) * arg_6->field_c + *(real *)(local_2 + 0x34);
                    *(real *)(local_7 + 0x20) = (*(real *)(local_2 + 0x40) - *(real *)(local_2 + 0x3c)) * arg_6->field_10 + *(real *)(local_2 + 0x3c);
                    *(short *)(local_7 + 0x2a) = (short)local_4;
                    *(long *)(local_7 + 0x2c) = arg_2;
                    if (!arg_6->field_0) arg_6->field_8 = function_180d82();
                    function_131c20(&local_11.field_0, (color3f *)(local_2 + 0x1c), (color3f *)(local_2 + 0x28), ((dword)local_2[0] >> 1) & 3, arg_6->field_8);
                    *(dword *)(local_7 + 0x24) = pack_color3f(&local_11.field_0);
                    local_7[0x27] = 0xff;
                    short local_13 = 0;
                    for (short local_14 = 0; local_14 < arg_5->field_5802; local_14++)
                    {
                        short local_15 = arg_5->field_5002[local_14];
                        for (short local_16 = 1; local_16 + 1 < local_15; local_16 += 2)
                        {
                            local_8[0] = local_0[local_13];
                            local_8[1] = local_0[local_13 + local_16];
                            local_8[2] = local_0[local_13 + local_16 + 1];
                            local_8[3] = local_0[local_16 + 2 >= local_15 ? local_13 : local_13 + local_16 + 2];
                            if (arg_1)
                            {
                                function_180d83(arg_1, &local_8[0].field_0);
                                function_180d83(arg_1, &local_8[1].field_0);
                                function_180d83(arg_1, &local_8[2].field_0);
                                function_180d83(arg_1, &local_8[3].field_0);
                            }
                            local_8 += 4;
                        }
                        local_13 += local_15;
                    }
                    arg_6->field_0 = (local_2[0] & 1) != 0;
                    long local_1 = *(long *)(local_2 + 0xc);
                    function_180b60(arg_3, local_9, local_6);
                    return local_1;
                }
            }
            ((s_physical_object *)g_509448)->block_delete(local_5);
        }
    }
    return NONE;
}
#pragma inline_depth(255)
