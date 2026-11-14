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
PRIVATE __forceinline void function_2922b2(point3f const *arg_0, vector3f const *arg_1, real arg_2, point3f *arg_3)
{
	arg_3->x = arg_0->x + arg_1->i * arg_2;
	arg_3->y = arg_0->y + arg_1->j * arg_2;
	arg_3->z = arg_0->z + arg_1->k * arg_2;
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
PRIVATE __forceinline void function_2922b5(vector3f const *arg_0, vector3f const *arg_1, real arg_2, vector3f *arg_3)
{
	real local_0 = (real)sin(arg_2);
	real local_1 = (real)cos(arg_2);
	real local_2 = (arg_0->i * arg_1->i + arg_0->k * arg_1->k + arg_1->j * arg_0->j) * (1.0f - local_1);
	vector3f local_3;
	function_2922b1(arg_0, arg_1, &local_3);
	real local_4 = local_1 * arg_0->i + arg_1->i * local_2 - local_3.i * local_0;
	real local_5 = local_1 * arg_0->j + arg_1->j * local_2 - local_3.j * local_0;
	real local_6 = local_1 * arg_0->k + arg_1->k * local_2 - local_3.k * local_0;
	arg_3->i = local_4;
	arg_3->j = local_5;
	arg_3->k = local_6;
}

// score 558 (was 576): earlier 592 and 618. Frame 0x108 agrees; first +0x31 stores success at +0x13 instead of +0x17;
// the minimum now has retail's +0x18, the input copy and hit flags sit 4 bytes off retail's.
// Earlier score 576 (was 592): earlier 618. Frame 0x108 agrees; first +0x31 stores success at +0x13 instead of +0x17.
// Earlier score 592 (was 618): Frame 0x108 agrees; first +0x31 stores success at +0x13 instead of +0x17.
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
		function_2922b2(&local_12, local_11, local_2->field_14, &local_13[0]);
		function_2922b2(&local_12, &local_9, local_2->field_14, &local_13[1]);
		function_2922b2(&local_12, local_11, -local_2->field_14, &local_13[2]);
		function_2922b2(&local_12, &local_9, -local_2->field_14, &local_13[3]);
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
			real ray_length = local_14[local_17];
			local_18.i = local_10->i * ray_length;
			local_18.j = local_10->j * ray_length;
			local_18.k = local_10->k * ray_length;
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
			arg_3->i = local_1->forward.k * local_17.k + local_1->forward.j * local_17.j + local_1->forward.i * local_17.i;
			arg_3->j = local_18.k * local_17.k + local_18.j * local_17.j + local_18.i * local_17.i;
			arg_3->k = local_19->k * local_17.k + local_19->j * local_17.j + local_19->i * local_17.i;
		}
	}
	return local_0;
}
