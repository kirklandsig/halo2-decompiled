// @flags /O2 /Gr /arch:SSE
#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include <math.h>
#include "normalize3d.h"
#include "vector_products.h"

struct s_2922b0
{
	byte field_0[0x14];
	real field_14;
	byte field_18[0x18];
	real field_30;
	byte field_34[0x14];
	real field_48;
};
struct s_collision_result_1697c0
{
	long field_0;
	real field_4;
	byte field_8[0x24 - 8];
	short field_24;
	byte field_26[0x4c - 0x26];
	long field_4c[3];
	byte field_58[2];
	short field_5a;
};
typedef char assert_collision_result_2922b0_size[(sizeof(s_collision_result_1697c0) == 0x5c) ? 1 : -1];

bool __stdcall function_1697c0(long arg_0, point3f const *arg_1, vector3f const *arg_2,
	long arg_3, long arg_4, s_collision_result_1697c0 *arg_5);
real function_30bf0(vector3f *arg_0);

PRIVATE __forceinline void function_2922b1(vector3f const *arg_0, vector3f const *arg_1, vector3f *arg_2)
{
	real local_0 = arg_0->j * arg_1->k - arg_0->k * arg_1->j;
	real local_1 = arg_0->k * arg_1->i - arg_0->i * arg_1->k;
	real local_2 = arg_0->i * arg_1->j - arg_0->j * arg_1->i;
	arg_2->i = local_0;
	arg_2->j = local_1;
	arg_2->k = local_2;
}
PRIVATE __forceinline real function_2922b3(real arg_0, real arg_1, real arg_2)
{
	if (arg_1 > arg_0)
		arg_0 = arg_1;
	else if (arg_0 > arg_2)
		arg_0 = arg_2;
	return arg_0;
}
PRIVATE __forceinline real function_2922b4(real arg_0, real arg_1, real arg_2, real *arg_3)
{
	real local_0 = (real)sqrt(0.0f > 1.0f - arg_0 / arg_1 ? 0.0f : 1.0f - arg_0 / arg_1);
	*arg_3 = local_0;
	return function_2922b3(arg_2 * local_0, 0.0f, arg_2);
}
/* arg_0 turned about the unit axis arg_1 by the angle arg_2 into arg_3, which may be arg_0 itself: the j and k
   cross terms are taken before the first row overwrites the vector's i */
PRIVATE __forceinline void function_2922b5(vector3f const *arg_0, vector3f const *arg_1, real arg_2, vector3f *arg_3)
{
	real local_0 = (real)sin(arg_2);
	real local_1 = (real)cos(arg_2);
	real local_2 = (arg_0->i * arg_1->i + arg_0->k * arg_1->k + arg_1->j * arg_0->j) * (1.0f - local_1);
	real cross_j = arg_0->k * arg_1->i - arg_1->k * arg_0->i;
	real cross_k = arg_1->j * arg_0->i - arg_0->j * arg_1->i;
	arg_3->i = local_1 * arg_0->i + arg_1->i * local_2 - (arg_0->j * arg_1->k - arg_0->k * arg_1->j) * local_0;
	arg_3->j = local_1 * arg_0->j + arg_1->j * local_2 - cross_j * local_0;
	arg_3->k = local_1 * arg_0->k + arg_1->k * local_2 - cross_k * local_0;
}

// score 518 (was 521): The 0x108 frame agrees. First +0x31 stores success at ESP+0x13 instead of +0x17. The hit slots and inactive exit still differ.
// Earlier scores: 576, 592 and 618. Before round 111: 558.
// Earlier scores 558, 576, 592 and 618. Frame 0x108 agrees; first +0x31 stores success at +0x13 instead of +0x17;
// Earlier at 521 (was 558): earlier 576, 592 and 618. Frame 0x108 agrees; first +0x31 stores success at +0x13 instead of +0x17;
// MiniMax C (11-19): no new form, no lower score (558): the success byte still sits at +0x13 where retail has +0x17, the four hit flags sit at 0x20..0x23 against retail's 0x1c..0x1f, the collision copy lands at 0x3c against retail's 0x40, and the comiss operand order and the exit layout still differ.
// Earlier at 558 (was 558): earlier 576, 592 and 618. Frame 0x108 agrees; first +0x31 stores success at +0x13 instead of +0x17;
// Earlier at 558 (was 558). earlier 576, 592 and 618. Frame 0x108 agrees; first +0x31 stores success at +0x13 instead of +0x17;
// the minimum now has retail's +0x18, the input copy and hit flags sit 4 bytes off retail's.
// M3 lane 10 r119 (fast; control 558): `*arg_5 = 0.0f;` ahead of the minimum's initializer 559; the hit flags as `{ 0 }`
// 689 (2819 B, block store); the success flag declared after the hit flags 552 (first difference +0x2e; the success store
// moves after the call and to +0xf, a declaration move under the bar); the early exit as `field_48 <= 0.0f` and as
// `0.0f >= field_48` 663 each (the first re-measures the listed 663). Body unchanged.
// MiniMax I (11-16): no new form, no lower score (558): retail materialises the zero in XMM0 after the minimum's store
// at +0x18 and keeps the range in XMM1, ours materialises it in XMM1 before the minimum's load and keeps the range in
// XMM0, so every later row is one off. Naming the ray length once (retail computes the half width plus the range once
// into XMM0 and stores it to +0x6c..+0x78) scored 553 and was dropped, so the body stands; the arms swapped, which
// moves the exit out of line, reach retail's 2886 bytes but score 632. The homes still differ: the success byte at
// +0x13 against retail's +0x17, the hit flags at +0x20..+0x23 against +0x1c..+0x1f, the input copy at +0x3c..+0x44
// against +0x40..+0x48.
// MiniMax D (11-15): no new form, no lower score (558): the frame size and the five calls agree with retail, but the
// success byte, the minimum float, the four hit flags and the copied input vector still sit at other frame homes, and
// the inactive exit still comes before the ray loop instead of after it.
// M3 lane 8 r150 (fast; aligned 406, register-masked 216 on this body): retail forms 1/range once and multiplies it by
// the minimum twice with the minimum held in XMM2 (no reload, so not an aliasing effect). Tried: the third and fourth
// offset points' scale as `0.0f - half_width` 592 (same code; re-measure of round 134's form); the point helper's products scale-first 592 (same code);
// its sums product-first 592 (same code); both range products as `minimum / range` 697 (2822 bytes, real divisions);
// the second product written `minimum * (1/range)` 592 (same code, still shared; re-measure of rounds 59/128/134); the
// per-ray hit update (clamp, distance, hit flag, minimum) as a forceinline helper writing through pointers 605 (2817 bytes);
// the arg_6 tail as `if (!arg_6) {...} else *arg_2 = local_3;` 586 (2839 bytes, aligned 439, masked 295: an arm swap
// with the test negated, 6 under the bar, not kept); after it: the ray length read from the array at each use 689; the hit
// distance from the ray-length local 682 (first difference +0x2); both 687 on the swapped tail; no `else` after the early
// return 684 (2834 bytes, aligned 358, masked 183: the lowest aligned count here, a lead), with the swapped tail 675.
// MiniMax E (11-10): no new form, no lower score (592); the frame size agrees but the homes do not: the success byte, the float, the four-flag array and the copied vector sit at other frame offsets here, and this build has thirteen instructions fewer than retail, so the exit blocks fall differently.
// MiniMax S (11-06): The frame size agrees but the homes do not: the success byte sits at frame+0x13 where
// retail keeps it at frame+0x17, the float at frame+0x10 against frame+0x18, the four-flag array at
// frame+0x24 against frame+0x1c and the copied vector at frame+0x4c against frame+0x40, and the first
// comparison branches the other way round.
// W14L: full statement walk against retail (0x2922b0-0x292df3; args 0x120..0x134 after the pushes): fields +0x14/+0x30/+0x48,
// tests, callees (0x1e5450, 0x30bf0 x3, 0x1697c0 with its six stack args), both cross/fallback chains, the four points,
// ray loop, per-ray clamps and signs, both rotations and the arg_6 tails agree; no behaviour difference found. Constants
// checked from the XBE (0.1, 0.7, 0.3, pi/180, pi/2, FLT_MAX). Remaining: stack-slot order (success at +0x0b, minimum at
// +0x10, hit bytes at +0x14), retail's (1/range)*minimum computed twice, field_14 re-read per point, dot-sum order in the
// rotations, inactive exit placed last. Tried (fast): active-if with one `return local_0` 629 (success in BL again);
// range local before the test 603; non-const tag record 592; PIN_126660 for the clamp 626 (with a local 618), cross3f_staged
// 678, cross3f 690 (vector_products.h): the cross helpers are not kept; the PIN form is a separate commit.
// Frame 0x108 agrees; success/input/minimum/hit homes differ from +0x31.
// Earlier: the true initializer uses BL, saving EBX early.
// Frame 0x108 agrees; first +0x31 stores success at +0x1b instead of +0x17.
// Input/minimum/hit homes, inactive-exit layout and later basis/ray/root/rotation homes differ.
// Frame 0x108 agrees; success/hit/input/minimum homes, inactive-exit placement and axis schedules remain.
// Earlier at 629: Frame 0x108 agrees; the true initializer uses BL, saving EBX early.
// Retail stores true directly in its result byte and returns immediate one on the inactive path.
// Hit/basis/ray/root/rotation homes follow.
// Frame 0x108 and the prologue agree (success in retail's stack byte); our
// early-true exit sits inline where retail places it last, then hit/basis/ray/root/rotation homes follow.
// Before round 47: success retained in BL with EBX saved early instead of retail's stack byte.
// Frame 0x108 and the prologue agree (success in retail's stack byte); our
// Frame 0x108 agrees; the true initializer uses BL, saving EBX early.
// Early constant minimum with long success 635, with early point copy 651; final products differ.
// MiniMax H run 14: the cross helper 0x2922b1 with its sign flipped and the operands swapped at
// all four calls (same results) 622; under the keep bar, not kept.
// Frame 0x108 agrees; early success BL/EBX and hit/basis/ray/root/rotation homes remain.
// frame agrees (0x108); early success BL/EBX and basis/ray/root/rotation homes remain;
// retail is 60 bytes longer after the ray loop (12 more loads, 4 more products: (1/range) * minimum
// twice) and re-reads the half-width for each offset point. Round 134: range local read before the
// test (with or without FLT_MAX there) 725/727; ray direction via vector3f_scale, 0 - width,
// minimum * (1/range), sqrt * angle all 641 (same code).
// Frame 0x108 agrees; early EBX/BL success, hit-byte and basis/rotation homes remain.
// Frame 0x108 agrees; early success BL/EBX and hit/basis/ray/root/rotation homes remain.
// Frame 0x108 agrees; early success BL/EBX and hit/basis/ray/root/rotation homes differ.
// Earlier: frame 0xf8 versus 0x108; result/basis/rotation homes differ.
// frame 0x108 agrees; success/basis/rotation homes differ.
// Frame pointer 960; size preference 1163; cosine-difference carrier 652;
// cross before trigonometry 655; inline depth one 652.
// Inverse length 651; separate roots 652; remaining fraction 653; cosine first 652;
// clamp input order 652; direction value with per-ray sample 703 (fast).
// native rotation return 666; native origin return 695; native cross return 864;
// short ray ordinal 712; active minimum lifetime 646 (fast).
// ray length through hit 661; hit clear per ray 719; angle limit observed once 647; ray length array
// initializer 646; joined output zero 648.
// Native object/settings/both record references 646; live length field reference alone
// or with object/settings/both 686; collision inline declaration 646. All eight restored.
// Earlier (652 from 652): Frame 0xf8 versus 0x108 and basis/root/rotation homes remain. Inverse length 651; separate roots 652; remaining fraction 653; cosine first 652; clamp input order 652; direction value with per-ray sample 703 (fast).
// Earlier (646 from 652): Frame 0xf8 versus 0x108 and basis/rotation homes remain.
// Earlier (646 from 646): Frame 0xf8 versus 0x108; result/basis/rotation homes remain.
// Earlier 652 from 652: Frame 0xf8 versus 0x108 and basis/root/rotation homes remain. Inverse length 651; separate roots 652; remaining fraction 653; cosine first 652; clamp input order 652; direction value with per-ray sample 703 (fast).
// Earlier 646 from 652: Frame 0xf8 versus 0x108 and basis/rotation homes remain.
// Earlier 646 from 646: Frame 0xf8 versus 0x108; result/basis/rotation homes remain.
// Existing rotation angle as a double formal or a twice-used double local,
// with original/angle-first/output-first/output-before-angle layouts: all 733.
// Original helpers, input/component snapshots and in-place calls restored.
// Frame 0xf8 versus 0x108; success, basis and rotation homes remain.
// frame 0xf8 versus 0x108; result/basis/rotation homes differ.
// Frame 0xf8 versus 0x108 and basis/root/rotation homes remain. Inverse length 651; separate roots 652; remaining fraction 653; cosine first 652; clamp input order 652; direction value with per-ray sample 703 (fast).
// Frame 0xf8 versus 0x108 and basis/rotation homes remain.
// Frame 0xf8 versus 0x108; result/basis/rotation homes remain.
// Rotation input snapshot 661; axis snapshot 705; both input snapshots 772;
// native point/direction snapshots 776; projection axis fields at use 646.
// Round 171 reduced control 646: rotation role names 646; rotation declaration homes 646; origin pointer
// result 646; rotation pointer result 646; symmetric lateral parts 675; symmetric axial parts 727; both row
// parts 643; row parts/rotation names 643; row parts/declaration homes 643; row parts/origin result 643; row
// parts/rotation result 643.
// bool-logical-clearance 648; bool-compound-clearance 658.
// byte-logical-clearance 648; byte-compound-clearance 660.
// short-logical-clearance 644; short-compound-clearance 655.
// long-logical-clearance 645; long-compound-clearance 702.
// Complete collision output extent/native output type/named material reset 641/641/641;
// with byte/long/word success 714/714/714; native hit bytes 641; active success 646.
// Original result, hit flags, math helpers and in-place copies retained.
// Native coordinate-array output contracts for cross/point/rotation alone, all
// pairs and all three 641 each; all outputs with native point input arrays 641.
// Every input/result copy and original helper contract restored; frame already agrees.
// Round 162: Native hit/length/both/point/hit+point/length+point/all array loops
// 731/730/748/667/721/643/723; shared cross producer on the complete frame 659.
// All regress; original arrays, private producers and in-place snapshots restored.
// Complementary ray-miss bool/byte/word/long arrays 642/642/676/708; active-phase
// arrays 708/708/682/724. Original hit flags, success result and math helpers restored.
// Negative failure result bool/byte/short/long, conditional store: 712/712/713/712;
// negative failure with reused close-hit logical accumulation: 679/679/674/673 (fast).
// Original result, hit arrays and all in-place math snapshots restored.
// Hit aggregate 641; length aggregate 629 kept; later success 661; hit/length
// 642, hit/later success 661, length/later success 650, all 662; success after copy 652.
// Input, hit flags, range, minimum, basis, origins, lengths and distances names each 629;
// original workspace names restored.
// Corrected-frame point-value/direction-value/both 682/694/772; output-reference
// combinations 682/694/772; input references with pointer/reference output 629/629.
// Original helper restored.
// Point-return with pointer/point/direction/both value inputs 680/744/838/899;
// cross first/second/both value inputs 845/858/1045. Original helpers restored.
// Collision-produced bool/byte hit entries with bool/byte/short/long success 700/690/690/690 each;
// memory success recovered but tag/basis homes regress. Original producers and carriers restored.
// Hit clear orders 3210/1032/2301/0213 629/629/629/629; success after
// hit clear 0/1/2/3 686/686/686/686; original entry restored.
// Round 343: Complete tag-range field reads 686; with shared cross products in basis/rotation/both
// 664/686/665; projection/basis+projection/rotation+projection/all 691/668/691/668. All restore; retain original snapshots and helpers.
// Round 146: Real rotation angle-first/output-first, point distance-first/output-first 629/629/629/629;
// cross output-first/all output-first/cross-output with rotation-angle/point-distance with rotation-
// angle 629/629/629/629. Original producer formals and in-place snapshots restored.
// Round 78 (aligned rows in brackets, baseline 482): FLT_MAX minimum declared at the top 635 plain [478];
// length element read at each product 668; early `return true` when the range is not positive 685 [443]
// (retail's prologue: no early EBX push, the flag stored straight to memory), with FLT_MAX at the top
// 680 [439]; nested body ending `return local_0;` then `return true;` 679 [462] (retail's early-exit
// layout and `mov al, 1`). Every stack home is then 4-12 bytes off retail's (flag 0xf, minimum 0x10,
// hit flags 0x14, input copy 0x38 in retail's frame).
// Round 257: shared affine origins 629; early constant minimum 636; collision-produced
// hit entries 700, with early minimum 706; inactive early return on those four
// affine graphs 685/698/681/694. Original helpers, producers and exits restored.
// CRT input copy 629; CRT hit clear 735; both 735; shared zero-output setter 629, with
// input/hit/both transfers 629/735/735; complete entry transfers with early minimum 739. Frame
// agrees; direct memory success and hit/basis/rotation homes remain.
// Round 171: blocked-ray count byte/short/long/unsigned-long at entry 707/707/706/706;
// after input copy and output zero 715/714/713/713. Original success bit, four hit bytes,
// accepted length initializer, every math helper and all in-place input/result copies restored.
// Round 175: ordinary-inline cross/offset/rotation helpers, all seven subsets, 629 each;
// all five existing helpers ordinary-inline 629. Original forced expansions restored;
// frame agrees, early success save and hit/basis/rotation homes remain.
// Round 392: four hit bits as byte mask at entry/active scope 766/766;
// word/long/dword masks 762/765 each. Frame shrinks and code regresses;
// original hit bytes, success, all helpers and in-place copies restored.
// Round 263: normalization/settings inline/noinline declarations singly and all four pairs
// 629/629/629/629/629/629/629/629. Original providers, helpers and in-place snapshots restored.
// Round 250: native reference formals for cross/rotation/root producers,
// subsets 1..7 and all with the point producer: 629 each. All original formals,
// helper calls and in-place snapshots restored; success and workspace homes remain.
// Round 227: Whole rotation sine-cross/base/both shared producers
// 731/648/713; result-first root/with sine/with base/with both
// 726/654/725/633; all with common promoted angle 721.
// Original helpers and all in-place snapshots restored; early success and workspace homes remain.
// Round 177: Success from the completed raw-distance minimum 689; with existing point/rotation/
// both/ray/point+ray/rotation+ray/all setters 707/706/707/711/711/711/711. All restored;
// frame shrinks to 0x104 and native success/hit/basis/rotation homes remain unresolved.
// Round 238: Actual ray origin/length/distance cursors 643/659/659;
// origin+length/origin+distance/length+distance 660/660/645; all three/all plus hit cursor
// 728/728. Original indexed arrays, countdown, success and helper snapshots restored.
// Round 249: Root return / limited-angle output with pointer/reference, output-first/last,
// and limit-first/original input roles: all eight 637. Original outputs, helpers and calls restored.
// Round 424: paired hit-response orders 0213/1023/1032/1302/0132
// 629/629/628/627/628; first three with completed clamp result 726 each.
// Two-point best rejected; original response order and helper restored.
// Existing staged cross in basis/rotation/projection subsets, then all with point setter
// 651/629/651/626/645/626/645/659; three-point best rejected;
// original producers and in-place snapshots restored.
// Round 457: native second-rotation result/two distinct rotation results, original/early
// minimum, original/staged rotation cross: 628/642/634/648/628/642/634/648.
// One-point best rejected; original complete body and all in-place snapshots restored.
// Round 259: all six cross-product parts named before subtraction, paired axis schedules
// ijk/ikj/jik/jki/kij/kji and positive/negative arm schedules, 636 each. Original
// helper, complete input/result snapshots and body restored; frame agrees, success/basis homes remain.
// Round 288: Complete ray-margin snapshot at post-settings/active/post-basis/post-origin phases, each without/with the complete angle-limit observation,
// 695/721/699/724/678/726/678/726 (fast). Original complete body, observations, helpers and call roles restored.
// Round 272: Basis declarations 798/879/897/978/987; entry collision workspace/original basis,
// cross-first basis, cross-first with entry ray workspace 629/629/629/629/629/629/629/632.
// Original executable body and declarations restored; remaining allocation differences persist.
// Round 47 (m3 lane 13): `if (!(range > 0.0f)) return true; else {...}` with the FLT_MAX minimum at the top
// 618 plain (kept; retail's `mov al, 1` exit and early minimum store); the early return alone 623; as a guard
// with the body at function scope 680; `if (range > 0) {...} else return true;` 679 (retail's exit last);
// nested `return local_0;` then `return true;` 679, with the range read first inside it 657 (prologue through
// the arg_6 test exact, divergence starts at the basis cross products); the range read once before the test
// 619; read first inside the else 618 (still reloaded); cross-product operands swapped in the helper's first or
// third part 618 (same code).
// Round 288: Complete ray-margin snapshot at post-settings/active/post-basis/post-origin phases, each without/with the complete angle-limit observation,
// 695/721/699/724/678/726/678/726 (fast). Original complete body, observations, helpers and call roles restored.
// Round 498: Closed origin/query/both regions 618/618/618; branch-owned root outputs alone
// or with origin/query/both 633 each; all regions with the existing staged basis/projection
// cross 690. Memory-result/input/minimum/hit ordering and later producer homes remain; original
// body, helpers, every query and all in-place input/result snapshots restored.
// Round 272: Basis declarations 798/879/897/978/987; entry collision workspace/original basis,
// cross-first basis, cross-first with entry ray workspace 629/629/629/629/629/629/629/632.
// Original executable body and declarations restored; remaining allocation differences persist.
// DH269: Native hit/minimum/uninitialized-input entry lifetimes, subsets 1/2/4/3/5/6/7,
// 631/636/618/639/631/636/639; all with success after acquisitions 639. External input
// copy stays after acquisitions; original full body restored. Success/input/minimum/hit homes remain.
// Round 300: Single inactive goto to the common result exit, staged cross basis/rotation/projection
// masks 0..7: 635/657/635/657/632/651/632/651. All restore early EBX/BL success and regress.
// Original constant inactive return, private producers, in-place snapshots and complete body
// restored; success/hit/input/minimum homes and axis schedules remain.
// DL229: distinct shadowed ray-index/response-scalar/projection-object identities in seven subsets,
// then complete basis identities, eight at 618. Original names and complete body restored;
// native success/input/minimum/hit homes and inactive-exit placement remain.
// Round 306: Native output-vector memset with input memcpy/hit memset/final memcpy subsets:
// 692/692/702/702/692/704/702/709. Output clear recovers the initial success home at 692, first +0x50,
// but all forms regress. Original scalar stores, hit bytes, helpers, in-place snapshots, complete body
// and includes restored at 618.
// DH271: Four-query remaining byte/short/word/dword with native long array ordinal 618/618/618/618; with
// word ordinal 726/726/726/726. Native countdown widths canonicalize; word ordinal loses the matching frame
// and shrinks away from retail. Entire original body, hit/result producers, helpers and in-place snapshots
// restored; success/input/minimum/hit homes remain.
// Round 257: Actual ray/projection coordinate consumers and every rotation-row product node,
// seven subsets, then short coordinate counters: 630/639/624/638/628/641/644/720. Original
// private producer, complete in-place snapshots and full body restored; success/input/
// minimum/hit homes and inactive-return placement remain.
// DH280: Existing native cross/point/rotation publication subsets, then all with the native ray publisher
// [618, 630, 630, 628, 623, 630, 624, 628]. Original executable body restored.
// Round 73 (m3 lane 16, fast; 618 stands, every form below scores higher): the active test as a positive if with `return
// true` last 679 (2839 bytes, aligned 473 -> 462: retail's exit layout at the function's end); with `else local_0 = true` 679; the range read once before the test 666 (2828, aligned 461; retail's
// `comiss xmm1, xmm0` and the later store from xmm1), declared before the input copy 675; the ray loop as a plain for 755
// (2768, frame 0x100: index scaled, no byte offset or count down), with the positive test 746, with the range too 724.
// Eight name sets for success, minimum, hit flags and basis 666 each (same code). Next cause: low slots, retail
// remaining/point cursor/success/offset/minimum/hit/basis/hit cursor/range, ours
// remaining/offset/minimum/success/basis/hit/point cursor/hit cursor/range.
// DH283: Native coordinate input/origin/projection producer subsets 1-7
// 667/721/739/618/667/721/739; complete graph with aggregate hit initialization 730.
// Original input copies, private producers, in-place snapshots and complete body restored.
// DH295: Joint query-count/ordinal/minimum lexical priorities, all six orders, then two
// opposing orders with the reused per-ray length renamed: eight at 618. Original names
// and complete body restored; success/input/minimum/hit and later vector homes remain.
// AC488: final-projection/rotation-axial reductions as shared dots or complete native
// product parts, eight non-original pairs: 616/622/614/628/624/627/624/627.
// NOT kept: shared dots gain at most four and change the sum trees without a match.
// Original body, helpers and all in-place copies restored; success/input/minimum/hit homes remain.
// Round 35 (m3 lane 17, fast): the input copy after the hit clears 629; the hit clears before the minimum and
// output zero 618 (same code); success declared after the object/settings lookups 631; the root clamp as
// `1 - a/b < 0 ? 0 : ...` 618; the output scale stored before the origin copy 618. Retail's homes: success +0x17,
// minimum +0x18, hit bytes +0x1c, input copy +0x40 (ours +0x1b, +0x10, +0x24, +0x4c); retail also reloads the
// half-width for the fourth offset point, where ours reuses the third point's negation.
// DH311: Joint actual equal-size retained-input/center, basis and final-projection lexical
// identities, seven family subsets then complete alternate vocabulary: eight at 618. No native home
// moves; original names/body restored. Success/input/minimum/hit and rotation homes remain.
// Round 128 (lane 8, fast control 618): retail forms 1/range once but multiplies it by the minimum twice
// (ours shares the product). Tried: the clamp's product written `minimum * (1/range)` 618 (same code); success,
// minimum and hit flags renamed (other alphabetical order) 618 (same code); the rotation dot as i, j, k 618
// (same code); the FLT_MAX minimum declared right after success 636; the hit flags from a brace initializer 630
// (2813 bytes).
// Round 201 (fast; aligned = difflib count, 346 here): retail rereads the half-width and negates it again
// for each offset point and stores each point's components as it makes them (the escaped point array
// treated as aliasing the definition), and loads the output's three components once for the basis
// product and stores the three final components last. Tried: the output's components read into three
// named amounts for the basis product 635 (2808 bytes, aligned 342); the final stores in k, j, i order
// 615 (under the keep bar); amounts plus the three final components staged in locals and stored last 620
// (retail's i, j, k stores at the end, aligned 345); the point helper as plain inline 618 (same code); the
// point array declared ahead of the position copy 618 (same code). Original body restored.
// Round 79 (m3 lane 16): the offsets read once into an `offsets` vector local before the basis sums 604 (plain).
// NOT kept (review): a struct value copy where retail reads the three components through the pointer at the
// basis sums (0x292ca9; our copy goes through integer registers); body restored at 618. That local declared before local_17 604; read before the arg_6 test 601; as three real
// locals 635 (2808 bytes, aligned 461); the private cross with retail's product operand order 604 (identical code);
// the hit flags as an aggregate initializer 605.
// DH320: Complete immutable computed scalars in cross/root/rotation producer subsets,
// then all with immutable range/ray inputs: 618/618/618/618/618/618/618/618. Original
// executable body and helpers restored; success/input/minimum/hit homes remain.
// Round 49 (m3 lane 15, fast control 618): ray lengths as `range + half_width` 618 (same code); the minimum as
// `local_4 = local_19 < local_4 ? local_19 : local_4` 618; hit flag stored before the distance 619; the clamp test
// as `local_19 / local_6 < 0.1f` 618; success cleared before the clamped distance 618. Retail reloads and negates the
// half-width again for the fourth offset point (ours reuses the third's negation).
// Round 59 (m3 lane 13, fast; aligned = difflib opcode count, 236 here): the ray loop is retail's instruction for
// instruction up to stack offsets (point cursor F+0x20 for retail's F+0x4, index F+0x4 for F+0xc, minimum F+0x8 for
// F+0x10, success F+0xf for F+0xb, side vector F+0x10 for F+0x18). Retail's offset-point half-width reloads come from
// register pressure (XMM0 is the z product's destination), not aliasing. The definition pointer without const 618
// (same code); the ray loop as a plain counted `for` 755 (2768 bytes, unrolled); the first range product written
// minimum first 618 (same code); as `0.7f * ((1.0f / range) * minimum) + 0.3f` 612 (2833 bytes, aligned 231; the
// product is still shared, the gain is scheduling elsewhere, under the keep bar); on that, the second product
// written minimum first 612, `0.3f + 0.7f * (...)` 612, the cross helper in retail's product operand order 612,
// the output components named for the basis product 627 (2816, aligned 219); a named inverse range used by both
// products 626 (2816). Original body restored.
// DH320: Complete immutable computed scalars in cross/root/rotation producer subsets,
// then all with immutable range/ray inputs: 618/618/618/618/618/618/618/618. Original
// executable body and helpers restored; success/input/minimum/hit homes remain.
// F365: Native vector cross/point translation/in-place rotation receivers, masks 1/2/4/3/5/6/7, then ordinary inline:
// 628/628/628/630/630/630/633/633 fast. NOT kept: no qualifying byte match; complete original body and helpers restored.
// B412: Four-query termination from the real origin/length/hit/distance array extent,
// native long/short ordinal from the cursor: 697/690/700/709/639/719/707/703.
// All regress and move the frame; original traversal, helpers and in-place captures restored.
// DH342: Shared lower-first root/output clamps and real root API, seven subsets
// 618/626/618/626/618/626/626; all with real rotation trig API 624.
// Original scalar producers, observations and body restored; success/input/minimum/hit homes remain.
// DH345: Complete final scalar projections before all three stores, six row orders,
// then immutable ijk/kji results: eight at 613 (2830 bytes). NOT kept: five-point
// gain is below the eight-point threshold; original body and in-place snapshots restored.
// DH348: Complete cross/rotation native aggregate/member-result storage pairs
// 847/847/561/561/944/944/944/944. NOT kept: 561 groups scalar results into a
// vector and does not match. Original scalar producers and in-place snapshots restored;
// success/input/minimum/hit homes and inactive-exit layout remain.
// DH352: Complete lateral/forward/up product parts across all three basis-reconstruction
// rows, subsets 1/2/4/3/5/6/7 then immutable full parts: 635/635/635/624/624/624/624/624.
// Original sum trees, scalar producers, helpers and in-place snapshots restored.
// Round 63 (m3 lane 14, fast): the clamp helper in place on its input (retail's `movaps xmm1, xmm0; jmp; comiss xmm1, xmm3;
// jbe; movaps xmm1, xmm3`) 592 (kept, 2831 B, aligned 227 -> 216). Lead, aligned rather than positional: the body nested under
// `if (field_48 > 0)` with `return true;` last 679 (2839 B); the range read before the test 666 (2828 B); read first inside
// the body 657 (2828 B, retail's opening exact through the hit clear and the range store, aligned 215), with the clamp 656 (204);
// the staged k, j, i cross (the cross3f_staged form, memory products nearly all retail's) on that 652 (2836 B, 206), with the
// clamp 651 (2838 B, 195); direct cross stores 664 (2822 B); on the old body the staged cross 672, direct 689, staged with
// the clamp 678. On 592: inverse range named for both products 604 (2818 B; retail multiplies (1/range) * minimum twice, the
// second in place on the inverse); the root's input named and clamped by an if 607, named in the ternary 592; the angle limit
// defaulted then overwritten 689; the ray rows k, j, i 684; `x = x - f()` for the third root 592. Still short: rows 3 and 4
// negate the half-width separately (ours shares it), and the rotation and basis-return rows schedule differently.
// DH360: Collision record plus material-reset lifetime at success/object/settings/input/minimum/clearance/guard/basis boundaries,
// eight forms 647/647/646/632/632/632/631/628. NOT kept: all regress and shrink to 2815/2886;
// original late reset, complete frame, helper inputs and in-place snapshots restored.
// DH362: Twice-used basis mode captured at entry/acquisitions/input/active boundaries,
// positive bool 626/626/626/626; complementary bool 723/717/702/699. NOT kept: all regress;
// original repeated mode tests, helpers, in-place snapshots and complete body restored.
// DH364: Float root/sine/cosine API subsets 1/2/4/3/5/6/7, then complete graph with
// published float root 618/610/610/610/610/616/616/614. NOT kept: the eight-point
// lead requires asymmetric trig API types without a retail precision distinction;
// complete float trigonometry gains only two. Original helpers and in-place snapshots restored.
// DH367: Chained actual length/hit/output initializer subsets 1/2/4/3/5/6/7,
// then complete chains with ascending length publication: 623/619/623/623/623/624/623/624.
// NOT kept: all regress; original initializers, helpers and in-place captures restored;
// native success/input/minimum/hit homes and inactive-exit placement remain.
// DH369: Standard axial/final-projection coordinate reductions, native/direct/named
// nonoriginal pairs: 789/789/710/917/917/696/919/919. NOT kept: all regress and grow
// beyond native 2886; original helpers, sum trees, in-place captures and body restored.
// Success/input/minimum/hit homes and inactive-exit/basis/rotation scheduling remain.
// DH369 revisit: Root-input lower-clamp positive/complementary comparison results,
// bool/byte/short/long, eight at 630 (2813/2886). NOT kept: all regress and
// shorten away from retail; original root helper and complete body restored. Frame
// 0x108, success/input/minimum/hit homes and inactive-exit/rotation scheduling remain.
// AC557: Native root quotient producer, optional remaining/clamped scalar stages,
// mutable/immutable complete families: eight at 607 (2817/2886). NOT kept:
// all regress and shorten farther from retail; original helper and body restored.
// AC559: Whole active-range admission/rejection, bool/byte/short/long: eight at 687
// (2830/2886). NOT kept: all regress; original live guard, complete helpers and
// in-place snapshots restored. Success/input/minimum/hit homes and inactive exit remain.
// J259: Full root/sine/cosine return storage, separate/paired/all and immutable all: 689/706/698/690/682/701/683/683.
// NOT kept: no eligible native repair; opening executable source restored. Native frame 0x108 is lost; original real helpers and every in-place capture restored; success/input/minimum/hit homes and inactive exit remain.
// M400: Complete original input/basis/origin/distance/ray/root/rotation-cross/projection defaults
// 603/677/637/665/671/592/592/598. NOT kept: none improves; original
// producers and all in-place snapshots restored; result/input/minimum/hit homes remain.
// Round 181 (m3 lane 4, fast): the active test written positively with `return local_0` at the end of its block and
// `return true` after it (or as its else) 674/674 (2841 bytes: retail's jbe to a last inactive exit, but the range
// field is read again after the output clears); with the range local read before that test 665 (2830: retail's reuse
// of the compared register for the range home); the range local before the current negative test 594 (2828); the cross
// helper's products in retail's load order 592 (same code), with the positive range form 665. Diagnostic: the success
// and minimum locals renamed to sort the other way 592 (same code: names do not order these homes). Retail's low frame is
// success byte +0xb, ray index +0xc, minimum +0x10, hit bytes +0x14, cross result +0x18; ours success +0x7, minimum +0x8,
// ray index +0xc, cross result +0x10, hit bytes +0x1c.
// M3 lane 9 r105 (fast; "masked" = aligned count with registers and stack offsets ignored, control 592 masked 216): a
// grid of the inactive exit last (`if (field_48 > 0) { ... return local_0; } return true;`) x the range read first
// inside the body x a named inverse range for both products: 8 forms 592-684 (the inverse range alone 604, re-measured); exit last with the range first 656
// (2830 bytes, masked 204), the inverse range adds 11-12 masked everywhere. On the 656 form: the FLT_MAX minimum
// declared inside the body 669 (masked 209); the hit flags declared inside the body 687 (216); the origin copy taken
// first in the body 691 (289). The ray loop as `do { ... } while (++local_17 < 4);` without the countdown 751 (2772
// bytes, masked 250), on the 656 form 731 (239).
// M3 lane 10 r86 (fast, control 592, aligned 484, masked 216): retail forms 1/range once and multiplies it by the minimum twice
// (second product in place on the inverse, the output store sunk below it) and negates the half-width separately for points 3 and 4.
// Both fractions as `minimum / range` 697 (2822 bytes, one divide shared); only the clamp input or only the output as a divide
// 679/679 (2845); fourth width as `0.0f - width` 592 (same code); widths as `width * -1.0f` 680 (2844); a second point helper
// negating the width at each product 678 (2844), with one named negated width 600 (2821); the half-width read once into a named
// local 688 (2828); the point helper's width as `real const &` 676 (2829); the point helper as plain inline 592 (same code).
// Round 73 (m3 lane 15, fast with 0x29ab90): the four offset points written through a point cursor (`point3f *point =
// local_13; ... point++`) 603 (2821 bytes); the final projection rows through the existing dot3f 597 (2828 bytes); the basis
// product rows written forward term first 592 (identical code); the output clear as one chained assignment 639 (2835 bytes).
// Diagnostic: the point helper as an out-of-line function in another file, inlined by LTCG, gives identical code (592), so
// retail's per-point half-width reloads are not an inlining or alias effect.
// Round 73, continued: the side-vector cross computed ahead of the basis product 594 (2831 bytes). Re-measured: the rotation
// helper's dot through dot3f (round 128's i, j, k order) 604 (2819 bytes).
// M3 lane 1 r89 (fast; control 592, aligned 273, masked 130): the view's +0x7c filler as a typed `vector3f up` member, both
// `(vector3f const *)((byte *)local_1 + 0x7c)` casts as `&local_1->up`, 592 (same code; header restored); the single-exit form
// (`if (local_2->field_48 > 0.0f) { ... } return local_0;`, no early `return true`) 629 (2842 B, first difference +0x1d); the
// minimum update as `if (local_19 < local_4)` 592 (same code); the ray vector as a brace initializer 592 (same code); the ray
// length read from `local_14[local_17]` at each product 689 (2834 B).
// M3 lane 12 r110 (fast; aligned/masked control 337/130): `*arg_5` published before `*arg_4` 680 (2837 bytes); one named
// `fraction = (1/range) * minimum` for both outputs 605 (2818); the arg_6 arm's side axis assigned before the forward 691;
// the root helper's product as root * limit 592 (same code; re-measure of round 134's `sqrt * angle`); the non-arg_6 forward copied from the entry copy of the input
// (`local_7 = local_3`) 673 (2828, first +0x1d); one named full ray length for the four array entries 604 (2820).
// M3 lane 13 r82 (fast, kept form confirmed plain): kept 576: the tail's first transform through the existing
// vector3f_scale/vector3f_add_scaled (forward*i, + left*j, + up*k; register-masked 130 -> 116, retail's i-first column
// loads). Before it: the tail's input read into i/j/k locals first 612. On 576: the back-rotation dots through dot3f 582;
// the three results in named locals, then stored 568; into a local vector copied to the output 547 (2879 bytes, but
// aligned 444 -> 449 and masked 116 -> 123: positional, one-use values ahead of stores); cross3f at all four crosses 660
// (masked 108), at the tail's cross only 570, at the arg_6 cross only 664; the tail's cross before the column transform
// 562; the up pointer declared at the tail's top 576; the ray loop as a plain `for` 718; the offset points through
// vector3f_add_scaled 576 (same code). Retail stores all three back-rotation outputs after their last loads.
// MiniMax A (11-12): no new form, no lower score (576): our frame is 0xf8 against retail's 0x108, and the copied input, the
// basis vectors, the origin and the collision result still share homes; the shared cross and scale helpers and the four-ray
// countdown of the earlier rounds leave that difference alone.
// T95: Existing setters for all input copies/fallback captures/final publications,
// family subsets 1/2/4/3/5/6/7: 653/649/579/685/637/663/693; all plus output clear 693.
// NOT kept: no exact transfer-form repair; original helpers, in-place captures and body restored.
// T97: Ordered early-return/conditional clamp 573/594; upper-first in-place/early-return/conditional
// 592/612/626; sequential caps/lower-default/upper-default results 668/649/655. NOT kept:
// best gains only three; no exact producer-lifetime repair. Original helper/body and in-place snapshots restored.
// T100: Actual success starts after basis/origin/initial scale, each of four origin
// producers, or query-loop setup: 599/599/599/599/599/599/599/599. NOT kept: all
// shorten away from native extent and lose the early native result store. Original
// body, helpers and every in-place input/result snapshot restored.
// P443: Double-factor rays, logical normalization-zero tests, complete basis library copies, named query,
// clearance ratio/free fraction, vector-setter rays and library hit fill: 691/576/578/576/575/575/578/700.
// NOT kept: no native success/workspace repair; original body, helpers and in-place copies restored.
// P446: Clearance failure, nearest-hit admission, finite-minimum admission, angle positivity, branch root ownership, uniform normalization results, uniform response angles, shared ray sample;
// 572/572/572/572/576/577/577/657. NOT kept: four-point nonmatch gains are below the eight-point bar; original complete body restored.
// P448: Uniform clamp lower/upper admissions, uniform normalization-zero decisions, four individual hit admissions, relational fixed-count termination:
// 543/543/578/576/576/576/576/576. NOT kept: clamp comparison locals lack byte equality and native value evidence; original body/helpers restored.
// M3 lane 10 r101 (fast; control 576, aligned 496, register-masked 341, with stack offsets masked too 203): nothing kept.
// Frame homes, relative to ESP after the `sub`: retail remaining 0x0, point cursor 0x4, success 0xb, ray index 0xc, minimum 0x10,
// hit flags 0x14, cross 0x18, hit cursor 0x24, range 0x28, forward copy 0x2c, input 0x38, origin 0x44, up copy 0x50; ours
// remaining 0x0, success 0x7, minimum 0x8, ray index 0xc, cross 0x10, hit flags 0x1c, point cursor 0x20, hit cursor 0x30,
// range 0x34, input 0x44; from the ray lengths at 0x5c upward both agree. 138 of the masked differences are those homes alone.
// The ray index declared at function scope 576 (same code); success, index, minimum, hit flags and cross declared at the top in
// retail's frame order 576 (same code); the hit flags, cross, minimum and success renamed to sort either way 576/576 (same
// code): neither declaration order, scope nor names place these homes. The four offset points through a second inline helper
// taking the definition and the array 567 (2859 bytes; a new one-caller helper, and the negated width is still held in a
// register for points three and four where retail forms `0 - width` from memory for each). The two range products: a named
// product ahead of both stores 577; the first grouped `(1/range) * (minimum * 0.7f)` 659 (2871 bytes, aligned 449: the
// product is then not shared, as in retail, but in the wrong order); `0.3f + 0.7f * (...)` with the second product turned 575.
// Diagnostics (removed): cross and the four points as members of one local aggregate 649 (2878 bytes; hit flags and input then
// land on retail's 0x14 and 0x38, so the sizes of the address-taken locals do move the homes); float consistency on for this
// body, the one setting diagnostic, 842 (3058 bytes), so retail's repeated width loads and unshared product are not that.
// P450: Separate forward/side/up normalization-zero producers in bool/long: 572/570/572/570/572/570;
// final publication mode in bool/long 576/576. NOT kept: one-consumer decisions lack byte equality;
// best six-point gain is below the eight-point bar. Original body, helpers and in-place captures restored.
// P452: Separate real forward/side/up return values, exhaustive angle-limit production, real countdown for, retained-input/forward-input/final-publication memmoves:
// 570/570/570/576/570/689/662/576. NOT kept: best six-point gain is below the bar; transfers regress. Original helpers, four queries and every in-place input/result copy restored.
// P454: Isolated function storage for forward/up/side scratch, center, origins, ray-length scalar,
// hit-distance scalar and collision output on the retained affine-tail graph: 576/576/576/576/648/576/576/576.
// NOT kept: none improves; original producer points, helper inputs and in-place captures restored.
// m3 lane 7 r159 (fast; plain 558 confirmed): kept, the four hit flags as an aggregate initializer `{ false, false,
// false, false }` 558 (2868 B). Tried: the minimum declared at the top of the active branch 570 (on the original body),
// 552 (on the initializer; a moved declaration, 6 under the bar); `{ false }` 689 (2819 B); the early exit as the else
// arm of `range > 0` 669; the output vector zeroed by one chained assignment 611; the ray-length sum named once for the
// four elements 553 (2880 B). Retail computes 1/range once but (1/range)*minimum twice (no shared product). A plain
// `for (i = 0; i < 4; i++)` loses retail's spilled down-counter (frame 0x100, /GL harness of this file).
// P477: Independent standard hit-fill/input-copy/output-fill/forward-copy/fallback-copies/center-copy/final-copy/length-fill boundaries:
// 700/681/656/573/693/575/613/668. NOT kept: best gain is three, with no native low-frame repair; several forms move the frame or shrink away from retail. Original arrays, complete in-place captures, helpers and body restored.
// M3 lane 6 r179 (fast; control 558): a symbol-count phase scan (1-7 unused long locals at the top, diagnostic, removed)
// 559/580/589/617/598/574/574, so phase 0 is best; at 4 and 5 the success byte and the input copy take retail's homes
// (+0x17, +0x40) but the minimum and hit flags move away, and the cross rows' operand order never moves with the phase.
// The positive range test with the exit last and the range read first inside the body, on this body, 646 (2865 bytes:
// retail's jbe and the range kept from the compare, but zero still in XMM1 and loaded early); the angle limit declared
// ahead of the two angle sums 558 (same code); `*arg_5 = 0` ahead of the FLT_MAX minimum 559; the arg_6 cross called on
// the forward and up fields before the two axis pointers are set 654 (2871 bytes); `local_4 < FLT_MAX` 558 (same code).
// Retail also reloads the half-width after each offset point's stores (`0 - [ebp+0x14]` for points 3 and 4), as if the
// point stores could alias the definition; ours keeps the width and its negation in registers across them.
// m3 lane 9 r124 (fast): the range read first in the else block, ahead of the output clear, 558 (same code); the
// output cleared through the existing vector3f_assign 558 (same code); the body nested under `if (field_48 > 0)` with
// the range read first inside and `return true;` last, re-measured 646 (2865 B: retail's jbe to the exit placed last,
// but our zero takes XMM1 and the range XMM0, retail the other way round, and the range home is +0x08 for +0x28); on
// that, `*arg_5 = 0` ahead of the minimum 647, the minimum declared after the hit flags 647, the range local before the
// test 651.
// P520: Standard range/normalization/clearance/minimum/sentinel/angle/clamp/root providers:
// 679/704/686/661/719/703/1055/738. NOT kept: no exact native storage/provider recovery; original complete body and helpers restored.
// P522: Real sine/cosine/paired/sine-root/cosine-root/all APIs, immutable angle/complete scalar formals:
// 561/561/561/556/556/561/561/561. NOT kept: no exact native success/workspace recovery; original complete body and helpers restored.
// M3 lane 11 r105 (fast; control 558): the angle limit as an if/else 558 (same code); the inactive test as
// `field_48 <= 0.0f` 663; in the ray loop's hit block the minimum update ahead of the distance and flag stores 662 (first
// difference +0x40), the hit flag stored before the distance 559; in the near-hit clamp `local_0 = false;` ahead of the
// clamped distance 558 (same code). Lead, not measured: the ray points' address escapes only into 0x1697c0 (still
// unmatched), and retail reloads the half-width after point stores as if they could alias the definition; that callee's
// link-time summary may differ from retail's.
// P524: Actual collision material 16-bit allocation short/word/long/dword/int/unsigned-int/signed-enum/unsigned-enum:
// 558/558/557/557/557/557/557/557. NOT kept: no exact native storage recovery; original record declarations, helpers and full body restored.
// P526: Original-site ray travel product mutable/const, shared local-range clearance floor mutable/const, four paired ownership graphs:
// 563/563/651/651/656/656/656/656. NOT kept: no byte-equal native success/workspace recovery; original ray expressions and all in-place snapshots restored.
// Original complete executable source restored.
// P539: Four-use unclamped angular product mutable/const after/before root publication 574/574/574/574;
// limit-initialized angular result after/before sqrt and root publication 573/573/574/574.
// NOT kept: all regress and shorten farther from retail; original helper/body and in-place captures restored.
// M3 lane 11 r108 (fast; control 558): 0x2922b2 plain `inline` 558; the half-width bound once as `real const &` for the
// four points and the ray lengths 558 (2876 bytes: a register reshuffle in the arg_6 cross and the rotation sums, the
// points still share one negation); `bool local_0 = true;` declared at the top of the active block (with `return
// local_0;` at that block's end) 552 (under the bar and a moved declaration, not kept: the success byte and the minimum
// then sit at retail's +0x17/+0x18, but the hit flags, the input copy and the arg_6 cross output stay 4 bytes off, and
// the true store follows the test); on that, the exit last (`if (field_48 > 0.0f) {...} return true;`) 664 (2865 bytes),
// the hit flags declared in the active block too 562; on this body, the input copy declared after the hit flags 559, the
// hit flags declared before the minimum 558, the two axis pointers initialised to the computed basis at their
// declarations with only the arg_6 arm assigning them 564 (2870 bytes). Reading of the listing: retail reloads the
// half-width for each of the four points (`0 - [ebp+0x14]` separately for points 3 and 4) while the width it loaded for
// point 2 is still in XMM0, so in retail the point stores kill the definition's field; ours keeps one negation for
// points 3 and 4 (its reload at point 2 is only a rematerialisation after point 1 clobbers XMM0).
// M3 lane 16 r127 (fast): both rotations through the existing forced-inline rotate_direction with `(real)sin(angle)` and
// `(real)cos(angle)` as direct arguments: --fast keeps that cross-file helper out of line (1121, 2400 B), and a same-file copy
// of its body screens 576 (2855 B); the ray vector through the existing vector3f_scale 559 (same ray code: the k product still
// reuses the dying length register where retail loads k first); the final projection into a local vector copied to arg_3 547
// (2879 B, positional only: the copy goes through the stack where retail stores the three rows straight from XMM, and a one-use
// local); the range read once before the inactive test (retail compares and later stores field_48 from one register) 661; the
// three projection rows as named parts stored together at the end (retail's i, j, k stores) 555 (2890 B).
// P582: Rounded persistent minimum/active range/complete angle-limit promoted receiver
// storage, seven subsets, then immutable range/angle receivers: 658/691/675/722/740/725/784/784. NOT kept: no exact native
// success/input/hit homes or inactive-exit recovery; original real receivers, helpers and full body restored.
// M3 lane 7 r172 (fast; control 558, aligned 491, register- and slot-masked 326): nothing kept. Diagnostics (removed): 0x1697c0
// storing its point pointer to a global leaves this body byte-identical, so the per-point half-width reloads are not
// the callee's escape summary; the minimum updated through its own address keeps the shared (1/range)*minimum. Tried: the
// two outputs under two separate `FLT_MAX > minimum` tests 558 (same code, product still shared); a named inverse with
// `fraction *= minimum` for the clamp input 556 (still shared; a reused name, under the bar); `*arg_5 = 0` ahead of the
// input copy 559 (the zero still in XMM1 before the input copy); the inactive test as `!(0.0f < field_48)` 558 (same
// code); the ray lengths by one chained assignment 558 (2872 B, aligned 497); the hit flags declared ahead of `*arg_5 =
// 0` 558 (same code); the input copy after `*arg_5 = 0` 559.
// M3 lane 13 r104 (fast; control 558): the range local dropped and field_48 read from the definition at each use 631
// (2892 B); `*arg_5 = 0.0f;` after the hit-flag initialisation 558 (same code: the zero still takes XMM1 ahead of the
// minimum's store); the minimum update as a ternary 558 (same code); the root helper's product written root * angle
// 558 (same code; retail multiplies angle * root for rays 1 and 2 but root * angle for rays 3 and 4, ours root * angle
// for all four); `*arg_4 = 1.0f;` right after the output clear 664 (2912 B).
// M3 lane 3 r256 (fast; control 558): composites of listed small gains: the named ray-length sum with the three projection
// rows named and stored together 560 (2871 B); the sum with the success byte declared at the top of the active block 547
// (2880 B, first difference +0x2e: the true store leaves retail's place ahead of the 0x1e5450 call; a moved declaration,
// not kept); sum, rows and that success 555; the centre copy and the four offset points in their own block 558 (same
// count, some registers move), with the sum 553. Frame reading (rough map from the listing): the low cells differ only
// in packing: retail puts the ray index (later x87 temporaries) at +0x4, success at +0xb and the range alone at +0x28;
// ours leaves +0x4 to the success byte (+0x7), packs the range with the sine/cosine temporaries at +0x8 and moves the
// ray index to +0x14.
// Success still stores at +0x13 instead of +0x17 on the 0x108 frame.
// Clearance predicate accumulated with logical AND or boolean AND assignment before or after the guarded distance
// update 665/659/648/649. Using a selected distance gives the same four scores.
// NOT kept: the AND after distance reaches 2886 bytes but regresses at the same success slot.
// B460: Ray origins by a two-axis table, positive/negative/both passes and opposite pairs
// 685/689/681/689. Opposite pairs with separate cursors 683, signed passes with one
// cursor 680, nested signed-offset and axis loops with cursor/index 775/777.
// The success, input and hit homes remain different. Original body restored.
// DH391: Shared origin generation/query cursor, hit query/response cursor and hit-distance
// query/response cursor, seven subsets 563/563/663/558/665/665/670; all with origin
// advance and rewind 651 (2886 bytes). NOT kept: no gain or matching success-byte home.
// Component copies at the retained input, forward input, center, both inputs, retained input
// and center, forward input and center, or all three 660/661/699/656/691/655/675.
// Direct copy initialization of the retained input and center 558. The success, input and hit
// homes remain different with the original copies.
// Individual up/left/down/right branch-local response roots 559/559/559/559;
// positive, negative and vertical branch-local pairs 558/558/558; root scratch
// shared per opposing axis 559. The success byte remains at +0x13. Restored.
// DH398: Reuse the final side axis, the ray displacement or both 603/578/604.
// The three with the shared scale helper 608/579/603.
// Reuse the side axis or both with the staged cross 599/600.
// NOT kept: The success byte stays at +0x13 and the body shortens. The separate projection workspaces are restored.
// DH401: Separate measured and clipped clearance. For explicit arms and measured
// default, direct fraction with minimum after/before hit stores 564/665
// and 564/665, named fraction 554/587 and 554/587.
// NOT kept: minimum first gives the native success slot but regresses. The
// best reduction is four points and leaves the success slot unchanged.
// Frame 0x108 agrees. The success byte is four bytes below retail. Scoping the collision query did not
// move it; the for count narrowed the extent gap by four bytes. query workspace scope 558; count and ray
// in for scope 558; count and ray in exclusive scope 558; preincremented ray ordinal 667; query scope
// with for count 558; staged hit distance 563; query writes hit flag 559; collision displacement inner
// scope 566. NOT kept: the scored forms tied or regressed.
// The success byte and input/hit homes remain displaced. Hold the vertical/lateral/both
// corrections in scalars 660/645/706, with axis-specific publication 690. Whole correction snapshot
// with per-axis or conditional whole-vector publication 660/709. Opposing pairs collected
// together or published separately 706/687. NOT kept: delayed output differs from the per-hit
// retail stores, and the entry homes remain different.
// DH406: Previous minimum read before the distance or between hit stores 563/563; early comparison at
// those sites 650/650; selected minimum formed before/between the hit stores and published last
// 665/654; formed first and published between stores 665; conditional update between the stores 556.
// NOT kept: selecting the minimum first recovers the success slot but regresses. The conditional
// update gains only two points and leaves ESP+0x13.
// M3 lane 16 r141 (fast, control 558): the active block as the `field_48 > 0` then-branch with one `return local_0;` 667
// (2866 B: retail's forward `jbe` to an end copy, but the success flag moves into BL); the hit flags `= { false }` 689 (2819 B);
// the inactive test as `<= 0.0f` 663; the range bound as `real const &` to field_48 648 (2865 B); 1..8 dead initialised locals
// after the object lookup (diagnostic) 559/580/589/617/598/574/574/574 (the symbol count moves code here; none beats 558).
// m3 lane 10 r121 (fast; control 558, aligned / stack-masked 491 / 415 by this round's counter). Retail's inactive exit is a
// separate `pop edi; mov al, 1; pop ebp; ret` after the main return, reached by `jbe` from the range test, and the range is
// loaded once (XMM1), compared, and stored to its home only inside the active arm after EBX and ESI are pushed. The body inside
// `if (field_48 > 0.0f)` with one shared return 667 (2866 B; 499 / 419: the success flag then lives in BL); the body inside the
// test with its own `return local_0;` and `return true;` last 669 (2865 B; 486 / 409): retail's block order and exit, the entry
// then differs only in the homes (success +0x13 for +0x17, hit flags +0x20 for +0x1c, input copy +0x3c for +0x40), the zero
// in XMM1 loaded ahead of the copies where retail clears XMM0 after the minimum's store, and the range loaded twice; on that the
// range read once into its local ahead of the test 651 (485 / 409: one load, but its home is stored ahead of the branch where
// retail stores it in the arm); the success flag declared after the hit flags on those two forms 663 (486 / 411) and 645
// (486 / 411). Retail's slot counts (stack-tracked) show a 16-reference home at +0x04 that this build does not have and the
// success byte at +0x0b behind it, so the frame differs by more than declaration order.
// M3 lane 13 r114 (fast; control 558, aligned/masked [348/117]): retail lays the inactive exit out as its own
// epilogue at the very end (`jbe` to `pop edi; mov al, 1`). The active test positive with one `return local_0;` 667
// (2866 B: the success flag is then held in BL); `return local_0;` closing the active block and `return true;` after
// it 669 (2865 B) [350/116]; `else return true;` then `return local_0;` 669; descriptive names for five locals 558
// (same code); `bool local_0;` assigned true in both arms of the active test 552 (first difference +0x2e; six
// points, a flag form). Body unchanged.
// Mode-specific returns from the direction copy/projected output/both arms 558/554/558;
// with inactive exit last 669/657/669; one range read with both returns and with
// inactive exit last 661/651. NOT kept: projecting first gains four; the success slot stays at +0x13.
// Success, hit flags and direction-copy homes remain displaced. The minimum and frame size agree. origin cursor
// with countdown tail 561, origin and hit cursors with countdown tail 558, origin cursor with count head 559,
// origin and hit cursors with count head 558, origin cursor with ordinal bound 708, origin and hit cursors with
// ordinal bound 703, origin cursor with end bound 683, origin and hit cursors with end bound 658. NOT kept: Both
// cursor-countdown forms tie at 2876 bytes and leave the success byte at +0x13. The ascending ordinal and origin-
// end bounds change the prologue and regress.
// M3 lane 11 r122 (fast with 0x1e5450, 0x30bf0, 0x1697c0; control 558, aligned/masked [491/208]): the active test
// positive with `else return true;` 669 (2865 B, [486/200], re-measure); with the range read into its local ahead of the
// test 651 ([485/201]: the range is then loaded once as retail, but the zero stays in XMM1 and the range's home moves),
// and with `return local_0;` closing the active block 651 (same code); the range read first on the original layout 661
// (re-measures of the listed 651/661); the four offset points through the existing vector3f_add_scaled 558 (same code).
// Query-local origin and hit cursors 561/559, paired 558. Paired cursors formed
// at origin allocation, hit initialization, both, or the distance and collision
// declarations 558 each; fixed pre-test count with both cursors 558.
// NOT kept: paired forms reach 2876 bytes but the success slot stays at +0x13.
// M3 lane 2 r111 (fast with 0x1e5450, 0x30bf0 and 0x1697c0, all matched; plain confirmed with the callers 0x29ab90 and
// 0x29bab0 unchanged at 286 and 489): kept, the rotation helper in retail's in-place shape (sine, cosine, projection, the
// j and k cross terms taken before the vector's i is overwritten, cross i inside row i, rows stored straight into the
// result) 558 -> 529 (2853 B, aligned 348 -> 269, register/offset-masked 105 -> 84: both rotations are then retail's
// instruction sequence with the vector's three components in rotated registers); kept, the tail's three projections as the
// arguments of the existing vector3f_assign (retail computes all three and stores i, j, k last) 529 -> 521 (2865 B).
// On the 529 form: the same rows staged into locals first 617 (2819 B); the cosine taken after the dot 529 (same code,
// either store form); the six orders of the dot's three terms and the swapped product operands 529 each (same code: the
// sum is canonicalised; the vector's components land in XMM3/XMM1/XMM2 for retail's XMM1/XMM2/XMM3), `i + (k + j)` 570,
// dot3f in either argument order 532; cross k ahead of cross j 551; cosine ahead of sine 554; the sine taken just ahead of
// the cross terms 559; `(1 - cosine) * dot` 529 and cross i named 529 (same code); the helper as a three-parameter in-place
// function 529 (same code); 1-8 unused locals at the top of the function (diagnostic) 529/599/608/599/534/534/565/533 and
// 1-4 in the helper 529/532/532/533. On the 521 form's base: the three projections staged in named locals 518 (one-use
// staged locals, not kept); the setter with dot3f arguments 583. On the old body: the two angles declared at the active
// block's scope ahead of the ray loop 652 (2890 B); the root output declared there, the ray loop with its counters and
// collision record in its own block, the range local declared ahead of the output clear, the basis locals declared
// pointers first: 558 each (same code). Left: the success byte at +0x13 for +0x17 and the entry's zero in XMM1; retail's
// two angle initialisers in the other order; `(1/range) * minimum` computed twice; the root helper's product as limit x
// root for rays 1 and 2; the rotated component registers; the tail's first transform (retail multiplies the forward's
// k, j, i by the first amount before any add).
// Definition/field-pointer signed offset in the point helper 642/644.
// definition/field-pointer width per component 552/662.
// definition/field-pointer assignment with guarded negation 505/505.
// definition/field-pointer subtraction from zero 642/644.
// Written-differs at 521. NOT kept: The 505 forms still share the negative width between the last two
// points and move the range spill to +0x1c instead of retail’s +0x38. The success slot stays four bytes
// low. The opening body and helper are restored.
// M3 lane 2 r113 (fast with 0x1e5450/0x30bf0/0x1697c0, all matched; aligned/masked = difflib count / with registers and
// offsets masked, control 521 [274/86]). LEAD 331 at 2896 B [131/39], plain-confirmed (callers 0x29ab90 286 and 0x29bab0
// 489 unchanged), patch build/r113/lead-0x2922b0-331.patch in the lane 2 worktree; not kept: it rests on a moved declaration
// and needs a byte match. Its parts: the second output's fraction written `local_4 / local_6` (the first keeps
// `(1.0f / local_6) * local_4`: the compiler then forms 1/range once and multiplies it twice, as retail) 649 (2874 B
// [204/75]); the first output as the division instead 649 (same code); on that the active test positive, `return local_0;`
// closing its block and `return true;` last 632 [195/68], with the range read first in the block 623; the point array
// `local_13` declared at function scope 541 (2887 B [138/50]: the half-width is then re-read and negated for each point as
// retail, because the escaped array may alias the definition only when it is declared outside the block that fills it; a
// compile-only test of a small copy shows the same rule); every other local of the active block also at function scope 541
// (same code); the four cross products through the existing cross3f_staged 331 [131/39] (through cross3f 406 at 2872 B;
// the private helper storing directly 406; its products b-first 541; its locals declared k, j, i 331; cross3f_staged at
// the first site only 331, at the second or third only 541, at the tail only 543). Without the function-scope array every
// combination stays above 521 (cross3f_staged alone 654, with the division 639 at 2886 B, plus the positive test 623, the
// array alone 649). On 331: the angles declared local_18 first 327, by one chained assignment 327; the angle limit declared
// ahead of the angles 343; diagnostic dead-local phase scan 1-8 331/604/611/403/338/334/331/330. Left on the lead: the
// second cross takes the global's i and the local's j/k products with commuted operands (ours `mulss xmm1, [esi]`, retail
// loads the global's component into a fresh register each time), the ray loop's padded entry (a length effect of that
// block), the root clamp's product limit x root for rays 1 and 2, the rotation components' XMM colouring and the tail's
// schedule (retail computes the tail cross after the column transform and spills its k at +0x5c).
// M3 lane 6 r209 (fast with 0x1e5450/0x30bf0/0x1697c0, all MATCH; control 521): the range named at the test (`real
// local_6 = field_48;` read once for the test and every later use) 667 (2852 B); the same with the positive test and the
// block ending in the common return 664 (2863 B); the output cleared through vector3f_assign 521 (same code); `*arg_5 = 0.0f`
// after the hit flags 521 (same code); the minimum declared after the hit flags 522. Stacked score lines merged into this note.
// Earlier declarations for the origin cursor or hit cursor alone, initialized at query setup, 517/517;
// both at entry, before the guard, before basis setup, before origin copy, or split between entry and basis
// 522/522/522/522/522/522. NOT kept: four-point best is below the threshold and the success slot stays low.
// M3 lane 4 r232 (fast; control 521; all on lane 2 r113's 331 lead, re-measured 331 at 2896 B, 138 aligned rows): the
// lead's first difference is the second cross only (retail loads each component of the global axis into a fresh register
// and multiplies it by the cached input component; ours folds the global as the memory operand), and the block after it is
// the same code two rows early. Redundant paren pairs on the plain reads ahead of it (the range, the input copies, the
// output clears; one to three pairs) 331 each (same code: with no operator inside they shift nothing, and no arithmetic
// precedes that cross). The private cross helper at the second site, the third, or both 331 (same code); cross3f at the
// second 331 (one row moved), at the second and third 406 (2872 B). A one-use pointer local for the global axis at the
// second cross 582 at 2886 B (retail's length, 138 aligned rows: every product then local-first, retail's are global-first,
// and the listing runs four rows off from +0x91); that local declared ahead of the basis vectors 373 (2896 B), at the
// function top 658, after the input copy 599, used for the up fallback as well 582 (same code); the forward pointer set
// ahead of the crosses and passed to them 598 (2879 B). Body unchanged (the lead is not applied).
// M3 lane 16 r157 (fast with 0x1e5450/0x30bf0/0x1697c0, all MATCH; control 521; "lead" = lane 2 r113's 331 form, re-measured
// 331 at 2896 B, aligned/masked 131/43). On the lead: rays 1 and 2 accumulated with `+=` and `-=` 331 (same code); the root
// assigned inside the product (`limit * (*root = sqrt(...))`) 378 (the root leaves the sqrt argument's slot, still root x
// limit); 1/2/3 redundant paren pairs around a constant 331 each (same code), around the ray's first product 331/584/572
// (three pairs give retail's 2886 B with no loop-entry pad, aligned 134, but the second cross then multiplies every product
// local-first where retail is global-first in all six); the tail's first transform as vector3f_assign arguments 582 (2886 B,
// aligned 131, the same second-cross state as three pairs; on the kept body 516, under the bar, not kept); the rotation rows
// as vector3f_assign arguments 450 (2913 B). The lead's aligned differences by block (exact/masked): second cross 10/9, root
// products 4/2, angle initialisers 2/0, first rotation 25/0 (a pure XMM permutation: retail's i, j, k in XMM1/2/3, ours
// XMM3/1/2), second rotation 30/7, tail 57/23. The paren count and the tail form move only the second cross and the loop
// pad; nothing here moved the rotations' registers or the root products.
// Array ordinal derived from the point cursor with countdown or end test 663/671.
// Ordinal derived from the hit cursor with countdown or end test 518/643.
// Paired cursors, ordinal from point or hit with countdown 659/517.
// Paired cursors, ordinal from hit with do-end or for-end test 639/639.
// NOT kept: The best form is four points lower at 2865 bytes and leaves the success byte at +0x13.
// M3 lane 15 r122 (fast; control 521, LCS-aligned 270/274 unmatched, register-masked 161/165): the range test as
// `if (field_48 > 0.0f) { ... } else return true;` 647 (2866 B, aligned 259/264, masked 150/155: the inactive exit then
// sits last as in retail); on it the range local read before the test and tested 637 (2862 B, aligned 259/264: one
// field_48 load shared by the test and the local as retail's XMM1, but its home is +0x14 against retail's +0x30, stored
// before the branch); the arg_3 clear through vector3f_assign 521 (same code), on the swapped form 637 (same code); the
// hit flags declared ahead of the minimum 521 (same code).
// M3 lane 2 r121 (fast with 0x1e5450/0x30bf0/0x1697c0, all MATCH; control 521, aligned/masked 274/165). Slot map from the
// listings (main-body offsets, retail / ours): point cursor 0x14 / 0x18, success byte 0x1b / 0x17, ray index 0x1c / 0x24,
// minimum 0x20 / 0x20, hit flags 0x24 / 0x28, cross vector 0x28 / 0x2c, hit cursor 0x34 / 0x38, range 0x38 / 0x1c; from
// 0x3c up the two agree. Lane 2 r113's 331 lead rebuilt on this source 331 (2896 B, 131/45): its slot map equals retail's
// at every offset. New forms: the early return kept with no `else`, the rest of the body at function level (no
// declaration moved) 648 (2865 B, 223/148); that with the four crosses through cross3f_staged and the second output as
// `local_4 / local_6` 631 (2887 B against 2886, 140/51: head and slot map are retail's; left at the head are the inline
// exit block, where retail jumps to an exit after the main return, and the range's second read); that with the range
// named ahead of the test 636 (2886 B, 143/55); the lead with one shared `return local_0;` 411 (first difference +0x1d,
// the success flag in BL); the lead with the range named ahead of the test in place of first in the block 340 (132/46);
// the lead with the second angle declared first 327 (129/44, re-measure). Reading of the lead's listing: the second cross
// loads the global axis component into a fresh register for all six products, where this build multiplies the dying
// local component by the memory operand in two of them; the first two rays' root products are limit x root with the
// limit copied ahead of the square root and the last two root x limit, this build root x limit in all four.
// M3 lane 5 r220 (fast with 0x1e5450/0x30bf0/0x1697c0, all MATCH; control 521): retail's ray loop head is reached by fall
// through at +0x440 with no pad, ours 8 bytes earlier with a jmp and a 6-byte pad (the code ahead is shorter). The range
// test positive with `return local_0;` closing its block and `return true;` last 647 (2866 B: the early exit is then laid
// out last as retail's 0x292de9, but the range is read again after the test and the slots move); the shared cross3f at
// the four crosses 660 (2848 B); the ray loop as a plain counted for loop 708 (2820 B); the angle limit as a default
// then an if 641; the root clamp's product written root first 521 (same code). Lane 2 r113's 331 lead re-measured 331 at
// 2896 B on today's file (opening exact to +0x91, first difference the second cross's operand order). Body unchanged.
// M3 lane 12 r165 (fast screen with 0x1e5450/0x30bf0/0x1697c0, all MATCH; control 521 at 2865 B, 270/274 unmatched
// instructions ours/retail in an LCS-aligned diff; "lead" = lane 2 r113's 331 form rebuilt on today's file, 331 at 2896 B,
// 131/129; body unchanged, nothing kept). The rotation helper with its cross product in a local vector through the shared
// cross3f 547 (2886 B, 355/354), through cross3f_staged 547, through this file's own cross 547 (same code each), the
// cross taken ahead of the projection 552 / 553; the helper reading the vector's three components into named locals
// first 553 (2860 B, 268/274), copying the whole vector first 593 (2919 B); the same seven on the lead
// 432/432/432/464/463/590/657 (the cross temporary costs the in-place rows there). The early return with no `else` and
// the body at function level re-measured 648 (220/223), with the four crosses staged and the second output as a division
// 631 (2887 B, 140/139, first difference +0x8f), with the range named ahead of the test 636 (2886 B, retail's length,
// 143/139): from the test on only the exit differs at the head (ours stores the range and lays the `mov al, 1` exit in
// line, five rows early; retail jumps to it at the end and stores the range in the active arm after the pushes). The
// lead with the point array declared at the top of the active block, ahead of or behind the range local, 610 each
// (2886 B, 193/190: the slot map is retail's, but the zero goes to XMM1 with the range in XMM0 and the half-width is not
// read again per point; only the function-scope array gives both). 1 to 8 initialised unused locals on the lead
// (diagnostic) 331/604/611/403/338/334/331/330 as lane 2 found; counted per block, the second cross's 14 unmatched rows
// stay at every count (not a symbol-count effect), the two rotations' rows move (26 to 102 and 54 to 79) but never
// clear. On the function-level form: the early exit as `return local_0;` 636 (same code as `return true;`); the range
// local declared after the early return, ahead of the output clears, 631 (2887 B, 139/138: the range's store is then in
// the active path as retail's, but the field is loaded a second time behind the in-line exit), with `return local_0;`
// 631 (same code). An early return is always laid out in line here, so retail's exit at the end means its `return
// true` stands after the active block, and the per-point reads then need the array declared outside that block.
// M3 lane 5 r225 (h2m fast with 0x1e5450/0x30bf0/0x1697c0, all MATCH; control 521; "lead" = lane 2 r113's 331 form rebuilt
// on today's file, 331 at 2896 B): on the lead, for the second cross (axis x forward, where retail loads each axis
// component into a fresh register for all six products): a staged helper taking (b, a, result) with the products written
// as before 331, taking (result, a, b) 331, computing i, j, k in that order 331 (same code each: the inlined helper's
// parameter order does not move the folds); the axis copied into a local vector ahead of the cross 632 (2919 B). Body
// unchanged.
// Separate first, second or both rotated results 645/535/623; first or second result
// declared before response evaluation 645/535; copied in-place response 566;
// first output reused in place by second 551; separate first input 542.
// All leave the first success byte at ESP+0x13.
// Common scalar ray length 725; seed other lengths from the first entry 516; indexed miss continuation
// 708, with standard minimum 694. Ray scope through the query with separate hit result 522, with
// shared scaler 519. Ray scope through the miss guard 708, with shared scaler 736.
// NOT kept: the five-point lead shortens the body by one byte and leaves success at ESP+0x13.
// M3 lane 2 r131 (H2M_FAST with 0x1e5450/0x30bf0/0x1697c0, all MATCH; control 521, aligned 364, masked 160): the file-local
// cross replaced by the shared cross3f_staged at its four calls 654 (2872 B; aligned 345, masked 130); one exit (positive
// range test, the active block, `return local_0;` last) 633 (2868 B); the angle output as `local_4 / local_6` 649 (2874 B;
// aligned 289); the shared cross with that division 639 (2886 B; aligned 276, masked 110); that plus the positive test with
// the range named first in the active block, `return local_0;` closing it and `return true;` last (lane 2 r113's lead
// without the function-scope array) 610 (2886 B; aligned 267, masked 102: field_48 is read once for the test and the range
// as in retail, but the zero takes XMM1 and the range XMM0, retail the reverse, and the crosses run XMM7/6/5 where
// retail's run XMM6/5/4). Body unchanged.
// M3 lane 16 r168 (h2m fast with 0x1e5450/0x30bf0/0x1697c0, all MATCH; control 521): the output clears as one chained
// assignment 612 (2869 B); the four ray lengths set by one chained assignment (`local_14[0] = ... = local_14[3] =
// field_14 + range;`) 516 (2864 B, under the bar, not kept); on that: the ray clamp's product written constant first 516
// (same code); the hit distance's product written collision fraction first 516 (same code: the array element is still
// the memory operand); the two angles zeroed by one chained assignment 516 (same code); the minimum test as `local_4 <
// FLT_MAX` 516 (same code); the cross helper's rows computed k, j, i 648 (2873 B); the input copy declared after the hit
// flags 518. Body unchanged.
// Assign the collision result to the hit entry before its test 630 or in the test 630.
// Named hit result stored early/late, bool 629/638 and byte 629/638.
// Explicit hit and miss stores, bool/byte 620/620. NOT kept: the four early
// hit clears disappear and the success store keeps the wrong home.
// Round 666. The frame agrees and success stays at ESP+0x13 against +0x17.
// Names for success/minimum/hits 521, ray workspaces 521, response angles 521, and all three
// sets 521. Reused fallback axis 522, shared ray vector setter 564, minimum from the
// stored hit distance 521, ray names with the shared setter 564. The setter shortens the ray loop.
// M3 lane 2 r135 (H2M_FAST with 0x1e5450/0x30bf0/0x1697c0, all MATCH; control 521): both rotations through the existing
// cross-file forced-inline rotate_direction with sine and cosine as direct arguments, plain check (the fast image keeps that
// helper out of line) 565 (2867 B; callers 0x29ab90 286 and 0x29bab0 489 unchanged); the root helper's `1 - x / limit`
// named once 521 (same code); the clamp helper 0x2922b3 as one conditional expression 551 (2863 B); the rotation helper's
// projection dot in plain i, j, k order 521 (same code: the order canonicalises); the hit distance from the loop's named ray
// length instead of the array element 650 (2886 B, the entry registers move). Lane 2 r113's lead rebuilt on today's file
// 331 at 2896 B (not applied). Body unchanged.
// M3 lane 3 r294 (h2m fast with 0x1e5450/0x30bf0/0x1697c0, all MATCH; control 521): the hit flags as `= {}` 663 (2826 B,
// block store); the inactive exit as `return local_0;` 521 (same code). On lane 2 r113's lead (rebuilt, 331 at 2896 B: after
// the second cross every row up to the ray loop is retail's shifted by one, so that cross and the loop-entry pad are
// the block's whole difference): the second cross as direct stores in global-first product order 331 (same code: the
// global's i still folds as the memory operand of two products); the point array declared first in the function 331
// (same code); parens around the three ray products 572 (2886 B, every second-cross product then local-first).
// M3 lane 8 r223 (h2m fast with 0x1e5450/0x30bf0/0x1697c0, all MATCH; control 521): the hit flags as `{ false }` 659 (2848 B,
// block store); the success flag declared after the object lookup 521; the ray clamp as `local_19 / local_6 < 0.1f` 521,
// the minimum test as `local_19 < local_4` 521, the range test as `local_4 < FLT_MAX` written with the literal 521 (same
// code each). Lane 2 r113's 331 lead rebuilt on today's file 331 at 2896 B (not applied): its first difference is still the
// second cross, where retail loads each axis component into a fresh register for all six products (the axis's j and k
// twice) and ours folds the axis's i as the memory operand of two. Body unchanged.
// M3 lane 12 r176 (h2m fast with 0x1e5450/0x30bf0/0x1697c0, all MATCH; control 521, aligned 364): the hit flag set ahead
// of the hit distance store 522; the minimum update as `local_4 = local_19 < local_4 ? local_19 : local_4` 521; the clearance
// test as `local_19 / local_6 < 0.1f` 521; the collision result declared and its index cleared ahead of the start point 521
// (same code each); the first output as `local_4 / local_6 * 0.7f + 0.3f` 649 (2874 B). Lane 2 r113's lead patch
// re-applied 331 (2896 B, aligned 183): beyond its crosses and the loop pad, both rotations differ by an xmm1/xmm2/xmm3 colour
// rotation and the tail by schedule only. Body unchanged.
// M3 lane 6 r231 (h2m fast with 0x1e5450/0x30bf0/0x1697c0, all MATCH; control 521, aligned 364, masked 160): the body after
// the early return without its `else { }` wrapper, so the point array sits at function scope with no declaration moved (lane
// 2 r113's alias rule: retail's per-point `0 - [ebp + 0x14]` reloads come back) 648 (2865 B, aligned 291, masked 126); that
// with cross3f_staged at the four crosses 649 (2872 B, aligned 268, masked 96); with the range named where the test reads it
// 640 (aligned 268); the positive range test around the body with `return local_0;` last 633 (2868 B, aligned 368); the
// no-else form with the crosses, the range and the clamp input as `local_4 / local_6` 636 at retail's 2886 B (aligned 196,
// masked 78: the inactive exit sits inline after the test where retail puts it after the main epilogue, which shifts every
// later row). Body unchanged.
// M3 lane 7 r218 (H2M_FAST; control 521; all on lane 2 r113's 331 lead, re-applied, 331 at 2896 B, not kept): the second
// cross through cross3f 331, through the private helper 331 (same code as cross3f_staged there); with 1/2/3 redundant pairs
// around the ray's first product 331/584/572 for each of the three spellings (the spelling never moves the cycle; no state
// gives retail's six global-first products). The first rotation's pure XMM permutation (retail i/j/k in XMM1/2/3, ours
// XMM3/1/2) does not follow the pair count, nor the helper's projection dot in i, j, k order 331, its cross_k computed
// ahead of cross_j 364, or cos taken ahead of sin 408. Body unchanged.
// M3 lane 6 r235 (H2M_FAST with 0x1e5450/0x30bf0/0x1697c0, all MATCH; control 521): on lane 6 r231's 636 form (crosses,
// range read first, clamp input `local_4 / local_6`, no else) the positive range test around the body with `return true`
// last 615 (2886 B; the success byte at +0x17 and the hit flags at +0x1c become retail's, the exit moves after the main
// epilogue); with the range read first inside the block 610 (its spill store moves after the pushes as retail). Lane 2
// r113's 331 lead rebuilt (331 at 2896 B, aligned 183): its first difference is the second cross, where retail's six
// products all take the axis component as the loaded destination (axis i loaded twice); ours folds axis i as a memory
// operand in both of its products. Screened on the lead (40 variants: the axis through a pointer local, the direction
// through a pointer local, the output through a pointer local, each combination, with 0-3 redundant pairs on the first
// ray product): three orientation states only (axis i folded alone; both b.j/b.k ahead of the whole axis; every product
// direction-first), never axis i first; the axis pointer alone 582 (2886 B, the loop pad goes); the clamp helper's
// product as `local_0 * arg_2` 331 (same code). Body unchanged.
// M3 lane 3 r303 (h2m fast, control 521, aligned 364): on the kept body the shared cross helpers one call site at a time:
// cross3f_staged at the second cross 521 (same code), cross3f there 518 (aligned 361, under the bar), cross3f_staged at the
// first 653 (2873 B), at the tail 523, at the third 521 (same code). On lane 2 r113's lead (rebuilt, 331 at 2896 B, aligned
// 183): a dead-local count scan at the function top (diagnostic), K = 0..40: 331/331/604/611/403/338/334/331/330/334, then
// 589/615/460/455/439/403/403/406/638/481/461/456/408/406/406/406/638/645/461/456/396/394/394/397/639/646/463/423/398 and
// K 39 331, K 40 330 (a 32-period, lowest aligned 168 at K 9 with 2907 B): no count matches, and the second cross's folded
// axis i is the same in every state of the lead's window. The second cross through a pointer-parameter helper with direct
// stores 331 (same code as the staged helper: stores to the local output do not stop the axis loads being shared); that
// helper reading the direction's three components into scalars first 570 (2886 B, aligned 182: the first row
// direction-first with copied registers, the later rows axis-first); the same written in the body with no helper 565
// (2864 B: axis i then loads first, axis j and k fold). Reading: retail loads all three axis components up front and again
// for their second products (six loads, every product axis-first); in every form here only the bare indirection (axis i,
// offset 0) sits on the other side of the direction, as the parameter-index rule for offset-0 operands predicts. Body
// unchanged.
// M3 lane 12 r183 (h2m fast; control 521, aligned distance 531; body unchanged). Frame reading: this frame is laid out
// by reference density (references x 1000 / size, densest lowest) and a new object joins the newest slot it does not
// interfere with. Retail's four-byte slots hold 29, 16, 11, 10, 8 and 5 references from the bottom with the result byte
// (density 3000) above the first two; ours hold 31, 11, 10, 10, 10 and 8, so the byte sits one slot lower and every later
// home is off. Retail's 16 are the loop's point cursor, the first angle total and the first rotation's cosine, and its
// range local has a slot to itself; ours packs that cosine into the range local's slot. LEAD (a moved declaration, so
// match-only; retail's own zero stores precede the angle limit): the two angle totals declared after the minimum block
// 506 at 2886 B (aligned 373, first difference +0x40, the result byte and a 15-reference slot as retail); with a named sum
// for the four ray lengths 506 (aligned 349). Forms: the range read into its local before the test with the early return
// 667; that with a positive test, the body ending `return local_0;` and `return true;` last 637; the field test with the
// range read first inside and the exit last 632; range first with one `return local_0;` 664; the four ray lengths from
// one named sum 521 (aligned 507); the minimum's fraction named once 517; both 524; the angle limit computed after the
// minimum block 553; the minimum block's two stores exchanged 651 (aligned 388); the reciprocal named in that block 522;
// the sample local declared ahead of it 521; the minimum test written minimum-first 521 (same code); the lead on the
// three exit forms 632/627/658.
// M3 lane 2 r146 (h2m fast with 0x1e5450/0x30bf0/0x1697c0, all MATCH each run; control 521, aligned 274, 97 with
// registers and stack offsets masked). KEPT 518 (fast, check.py --fast and plain; callers 0x29ab90 286 and 0x29bab0 489
// unchanged): the four ray origins through the shared vector3f_add_scaled (the file's private copy of it removed; the
// origins alone 521, same code) and the ray direction through the shared vector3f_scale in place of three hand-written
// rows and their `ray_length` local (518, aligned 271, masked 96; with the local kept as the scale argument 519 at
// 2864 B). Not kept: the object arm's cross taken from the object's own two vectors ahead of the pointer assignments 664
// (2851 B), after them 655 (2863 B); the pointers assigned up first 650 (2870 B); the tail's cross and third projection
// from the object's own vectors with no pointer local 564 (2848 B), with the object arm's direct cross 665; the tail's
// three projections through dot3f 583 (2837 B). Retail's first cross forms each product in the operand whose offset in
// the object is the larger multiple of four mod 32 (forward.k before up.j, forward.i before up.k, up.i before forward.j),
// which is what one base pointer with the object's own offsets gives; the direct forms above do not reach it.
// The clearance, hit and range slots still differ. Rejected-ray counts in long/short/byte 662/675/664;
// remaining clear count 675; rejected-ray masks in dword/word/byte 682/694/674; remaining clear mask 719.
// NOT kept: the counts and masks change the retail byte lifetime and shorten the body.
// The clearance byte, hit slots and range still differ. A cursor on the actual ray-length array,
// with long or dword index and equal-end or upper-bound tests: for 561 in all four forms, do-while 556 in all four.
// NOT kept: each drops the frame from 0x108 to 0x104 and shortens the body.
// Assign angle zero on first-axis miss 637, second-axis miss 571, or both 648. Separate
// scaled side or up contribution 519/525, both 554. Shared i-k-j rotation projection 524,
// immutable origin copy 518. NOT kept: first-axis definite assignment recovers the success
// byte home, but shortens the body and leaves the early zero in XMM1.
// The clearance byte still sits four bytes below its retail home, and the ray and inactive-exit layout differ.
// distance array staging 661, split raw/clamped distance stores 683.
// shared selected distance 514, positive range with split distance stores 643.
// collision result assigned to hit byte 637, distance guarded by hit byte 673.
// slot minimum before the hit byte 661, reused ray-vector workspace 518.
// NOT kept: The selected-distance form gains four points and is one byte shorter. The query-result byte changes entry allocation and raises the score. The other forms tie or regress.
// The success byte remains at ESP+0x13. On the outer-origin positive-range form,
// fallback cross by product then subtraction, axis/direction first and two row orders:
// 333 each. Component arrays for axis, direction or both 331 each. Single reads
// of the three axis components 478. NOT kept: the outer-origin form requires a match,
// and the fallback cross still folds the wrong input into its multiply.
// Near-ray turn used to seed the final angle at the opposite ray, first/second/both
// 633/634/663. Opposite-ray test produces the final angle, first/second/both 664/611/667.
// Compose the angle after the direction store, first/both 664/673.
// NOT kept: six forms recover the clearance byte at ESP+0x17, but zero stays in XMM1,
// the active path shortens and every score rises.
// The clearance byte still sits four bytes below its retail home, and the ray and inactive-exit layout differ.
// distance array staging 661, split raw/clamped distance stores 683.
// shared selected distance 514, positive range with split distance stores 643.
// collision result assigned to hit byte 637, distance guarded by hit byte 673.
// slot minimum before the hit byte 661, reused ray-vector workspace 518.
// NOT kept: The selected-distance form gains four points and is one byte shorter. The query-result byte changes entry allocation and raises the score. The other forms tie or regress.
// M3 lane 12 r188 (h2m fast with 0x1e5450/0x30bf0/0x1697c0, all MATCH; control 518, aligned 271; lane 2 r113's lead rebuilt on
// this body 331 at 2896 B, aligned 130). Cause of the lead's second-cross difference, read from the compiler's operand ranks:
// a load through a global pointer at offset 0 ranks by that global's symbol number mod 8 (0x2000 per step), and globals are
// numbered from 32 in order of first mention in this function's own text, objects reached through an inline getter after
// them. Here the forward default global is 32, the up-axis global 33 (rank 0x2008, below the forward copy's j and k at
// 0x5c40/0x5c80), so axis i is folded as a memory operand; retail's six axis-first products need the up-axis global at 35
// to 39. Diagnostic only (removed): `(void)` reads of the left default, forward default and object table globals at the
// top give the up-axis global 35 and the lead then 188 (2896 B, aligned 120): the whole second cross is retail's and the
// loop pad goes; left on it are one load order ahead of the ray loop, the ray loop's minimum-store order, the root
// clamp's product order for rays 1 and 2, the two rotations' XMM colouring and the tail. Natural forms: the object read
// through the existing OBJECT_DATUM_GET macro (object table global then 32, up axis 34, rank 0x4008, still below)
// 518 on this body (same code) and 331 on the lead (same code); the forward copy written member by member (the copy's j
// and k then early symbols, every product direction-first) is worse on both. Up axis needs one more global mentioned
// ahead of it. Body unchanged.
// Selected normalized forward/left/up 518/521/516, forward+left/forward+up/left+up
// 659/520/660, all 652, shared normalization workspace 662.
// NOT kept: upper-axis selection gains two points. All axes reach 2886 bytes at 652.
// The entry clearance byte stays at ESP+0x13. The in-place basis producers are restored.
// M3 lane GA r7 (fast with 0x1e5450/0x30bf0/0x1697c0, all MATCH; control 518, aligned 194, masked 184; 36 trials).
// On lane 2 r113's outer-origin positive-range 331 lead (rebuilt 331, aligned 122, masked 102): replacing object_get with
// existing OBJECT_DATUM_GET and 3.402823466e38f with `static const real k_real_max_2922b0` naturally gives g_4687b0 symbol
// #35 (no dummy reads) and drops the lead 331 -> 188 (2896 B, +0x91, aligned 112, masked 88); declaring local_18 ahead of
// local_17 hits **184** (aligned 110, masked 88: lines 5..456 byte-exact except one 3-load order at 158 and rays 0/1's
// `arg_2 * root` order). Also on that lead: k_zero const 663; up inline 188; cross3f_staged on up 188; dot3f in rot 329/329;
// basis_dot_xzy in rot 445 (fixes rot1 (i+k)+j sum but also forces rot2); rot dot i+j+k / i+k+j clean 188/188; rot with
// cross3f_staged / cross3f / temp out 434/303/432; tail dot3f in vector3f_assign 421, direct 286, without local_19 277
// (reproduces retail's [esp+0x5c] v.k spill and final 3 stores at 666/667 insns, but shifts local_9.k/j to xmm5/xmm6);
// tail dot3f(v, axis) 420; tail local_17 via vector3f_assign 548, +dot3f(v,axis) 361, .k/.j/.i 549; matrix3x3 tail 659;
// permute local_7/8/9 decls 184/184; second static const (#36) 184; inline rays 264, rays 0/1 direct 446; cross3f_staged
// + dot3f tail 333; sincos 316; normalize_inline 334. On 518 base (no moved local_13): OBJECT_DATUM_GET + k_real_max +
// local_18 before local_17 515 (-3, under bar); positive range 637; no-else/div/cross3f_staged 631; range+no-else 636;
// single-exit 643. Body unchanged (outer-origin lead requires byte match).
// Separate the leading side or up response magnitude, or share response magnitude by sweep
// end or rotation axis: 519 each with the opening inactive exit, 648 each with the late exit.
// NOT kept: the 0x108 frame agrees, but the success byte stays below the retail home and
// the alternate response workspaces shorten the former body or raise the score.
// Ray-length or hit-fraction distance accumulators 518/518. Ray-length accumulator
// with named clearance fraction 515, direct floor test 518, clearance written first
// 518, minimum written before hit flag 516, conditional minimum 518, or minimum from
// stored distance 574. NOT kept: the ray-length load takes retail's operand order,
// but still uses the displaced ordinal and range slots and shortens the body.
// The 0x108 frame agrees, with the success and hit slots still displaced.
// World reconstruction through matrix helper 677, world reconstruction through axis helper 714, object
// projection through matrix helper 621, object projection through axis helper 734, world reconstruction
// through matrix helper, object projection through matrix helper 836, world reconstruction through
// matrix helper, object projection through axis helper 763, world reconstruction through axis helper,
// object projection through matrix helper 726, world reconstruction through axis helper, object
// projection through axis helper 826.
// NOT kept: The basis conversions grow the body and raise the score. The helpers can also change
// addition grouping.
// M3 lane 1 r168 (fast with 0x1e5450/0x30bf0/0x1697c0, all MATCH; control 518): the tail's three dot products stored
// straight into arg_3 in place of vector3f_assign 565 (2861 B); the side vector declared ahead of the other two 518; the
// clearance test written `local_19 / local_6 < 0.1f` 518; the two angle totals declared in the other order 518; the
// inactive exit as `return local_0;` 518 (same code each). Body unchanged.
// @retail 0x2922b0
bool __stdcall function_2922b0(long arg_0, long arg_1, vector3f *arg_2,
	vector3f *arg_3, real *arg_4, real *arg_5, bool arg_6)
{
	bool local_0 = true;
	s_slot_object_view *local_1 = object_get(arg_1);
	s_2922b0 const *local_2 = (s_2922b0 const *)function_1e5450(arg_0, local_1->tag_index);
	vector3f local_3 = *arg_2;
	real local_4 = 3.402823466e38f;
	*arg_5 = 0.0f;
	bool local_5[4] = { false, false, false, false };
	if (!(local_2->field_48 > 0.0f))
		return true;
	else
	{
		arg_3->i = 0.0f;
		arg_3->j = 0.0f;
		arg_3->k = 0.0f;
		real local_6 = local_2->field_48;
		vector3f local_7;
		vector3f local_8;
		vector3f local_9;
		vector3f const *local_10;
		vector3f const *local_11;
		if (arg_6)
		{
			local_10 = &local_1->forward;
			local_11 = (vector3f const *)((byte *)local_1 + 0x7c);
			function_2922b1(local_11, local_10, &local_9);
		}
		else
		{
			local_7 = *arg_2;
			if (function_30bf0(&local_7) == 0.0f) local_7 = *g_4687a8;
			function_2922b1(g_4687b0, &local_7, &local_9);
			if (function_30bf0(&local_9) == 0.0f) local_9 = *g_4687ac;
			function_2922b1(&local_7, &local_9, &local_8);
			if (function_30bf0(&local_8) == 0.0f) local_8 = *g_4687b0;
			local_10 = &local_7;
			local_11 = &local_8;
		}
		point3f local_12 = local_1->unknown030;
		*arg_4 = 1.0f;
		point3f local_13[4];
		vector3f_add_scaled((vector3f const *)&local_12, local_11, local_2->field_14, (vector3f *)&local_13[0]);
		vector3f_add_scaled((vector3f const *)&local_12, &local_9, local_2->field_14, (vector3f *)&local_13[1]);
		vector3f_add_scaled((vector3f const *)&local_12, local_11, -local_2->field_14, (vector3f *)&local_13[2]);
		vector3f_add_scaled((vector3f const *)&local_12, &local_9, -local_2->field_14, (vector3f *)&local_13[3]);
		real local_14[4] = { local_2->field_14 + local_6, local_2->field_14 + local_6,
			local_2->field_14 + local_6, local_2->field_14 + local_6 };
		real local_15[4];
		s_collision_result_1697c0 local_16;
		local_16.field_24 = NONE;
		long local_17 = 0;
		long remaining = 4;
		do
		{
			vector3f local_18;
			vector3f_scale(local_10, local_14[local_17], &local_18);
			if (function_1697c0(0x1808c2d, &local_13[local_17], &local_18, arg_1, NONE, &local_16))
			{
				real local_19 = local_14[local_17] * local_16.field_4 - local_2->field_14;
				if (0.1f > local_19 / local_6)
				{
					local_19 = local_6 * 0.1f;
					local_0 = false;
				}
				local_15[local_17] = local_19;
				local_5[local_17] = true;
				if (local_4 > local_19) local_4 = local_19;
			}
			++local_17;
		} while (--remaining);
		if (local_0)
		{
			real local_17 = 0.0f;
			real local_18 = 0.0f;
			real local_19 = local_2->field_30 > 0.0f ? local_2->field_30 * 0.01745329238474369f : 1.5707963705062866f;
			if (3.402823466e38f > local_4)
			{
				*arg_4 = (1.0f / local_6) * local_4 * 0.7f + 0.3f;
				*arg_5 = 1.0f - function_2922b3((1.0f / local_6) * local_4, 0.0f, 1.0f);
			}
			real local_20;
			if (local_5[0])
			{
				local_17 = function_2922b4(local_15[0], local_6, local_19, &local_20);
				arg_3->k -= local_20;
			}
			if (local_5[1])
			{
				local_18 = -function_2922b4(local_15[1], local_6, local_19, &local_20);
				arg_3->j -= local_20;
			}
			if (local_5[2])
			{
				local_17 -= function_2922b4(local_15[2], local_6, local_19, &local_20);
				arg_3->k += local_20;
			}
			if (local_5[3])
			{
				local_18 += function_2922b4(local_15[3], local_6, local_19, &local_20);
				arg_3->j += local_20;
			}
			function_2922b5(&local_3, &local_9, local_17, &local_3);
			function_2922b5(&local_3, local_11, local_18, &local_3);
		}
		if (arg_6)
			*arg_2 = local_3;
		else
		{
			vector3f local_17;
			vector3f_scale(local_10, arg_3->i, &local_17);
			vector3f_add_scaled(&local_17, &local_9, arg_3->j, &local_17);
			vector3f_add_scaled(&local_17, local_11, arg_3->k, &local_17);
			vector3f local_18;
			vector3f const *local_19 = (vector3f const *)((byte *)local_1 + 0x7c);
			function_2922b1(local_19, &local_1->forward, &local_18);
			vector3f_assign(arg_3,
				local_1->forward.k * local_17.k + local_1->forward.j * local_17.j + local_1->forward.i * local_17.i,
				local_18.k * local_17.k + local_18.j * local_17.j + local_18.i * local_17.i,
				local_19->k * local_17.k + local_19->j * local_17.j + local_19->i * local_17.i);
		}
	}
	return local_0;
}
