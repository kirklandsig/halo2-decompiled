// @flags /O2 /arch:SSE /Gr
#include "projection_storage.h"
/* UNKNOWN_0497A0.CPP: view parameter computation */

#include "unknown_11c920.h"
#include "camera_storage.h"
#include <math.h>
#include "unknown_11cc90.h"
#include "unknown_0494b0.h"
#include "globals.h"
#include "object_tag_definitions.h"


#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

#include "vector_products.h"
#include "normalize3d.h"
// MiniMax H (11-13): no new form, no lower score (536).
// MiniMax E (11-11, run 80): no new form, no lower score (536): the two perspective-row crosses through the existing
// staged helper compile to the same code (536) and the special path's two crosses through it score 537; retail
// computes the cross components in the k, j, i order and keeps the first cross's products alive for the second
// one, which gives its frame 0x60 with five vector copies against our 0x54 with four, and it holds the reciprocal
// in xmm5 where we hold it in xmm1, so the shape pointer sits at [esp+0x14] in retail and at [esp+0x1c] here.
// MiniMax G (11-11): no new form, no lower score: the permuter over this file's wave spellings produced no variant that matches retail's bytes; retail's second (dead) uses of sin x, cos 7.8y and cos 7.75y are what keep those three on the x87 stack until after the product, and no plain spelling of the wave expression gives them a second use.
// MiniMax G (11-12): no new form, no lower score (536): the permuter's search over this body (full image, annealed, 26 tries) found
// no better form; the frame is still 0x54 against retail's 0x60 and retail's second (dead) uses of sin x, cos 7.8y and cos 7.75y are
// what keep those three on the x87 stack until after the product.
// score 536 (was 594): earlier first lines 661 (was 661), 661 (was 661; 537 before that), 661 (was 537); frame 0x54
// against retail's 0x60 and the wave sines' x87 order remain; round 151's entry is the last one above the marker.
// Round 16: the argument registers agree (ECX is the source, EDX the result, the camera and the
// flags on the stack); retail holds the secondary shape in EDI, the primary shape at [esp+0x14]
// and the inverse at [esp+0x10], ours holds the secondary shape at [esp+0x10] and the inverse at
// [esp+0x14], and our frame is eight bytes short of retail's.
// MiniMax R (10-31): shape0 hoisted above the inverse 712, shape0 first in the switch 581; permuter 22 tries, best 571 (baseline 571); no lower score.
// Earlier, retail keeps the primary shape in the stack slot at [esp+0x14].
// MiniMax D (11-05): retail's single test on source->type_low, with the shape0 pointer in the home at
// [esp+0x14] and the inverse scalar below it at [esp+0x10], was tried as an if/else over both shape
// pointers in three declaration orders: 621/621/626; the frame stays 0x58 against retail's 0x60, so no
// lower score.
// Earlier, at 581: the frame is 0x58 against retail's 0x60 (ret 0x20 and the EAX result agree) and the body is
// thirteen bytes longer; retail keeps the primary shape in the stack slot at [esp+0x14]
// and the secondary shape in EDI, ours keeps the inverse at [esp+0x14] and the
// secondary shape at [esp+0x10].
// Fixed in the b2E round: the sixth input product `scale * d.k` had been truncated through an integer (a score
// choice); retail (0x49f0d) is a plain mulss.
// Round 134: retail reads the wave's sine arguments from the global point: that form 701 (frame 0x60,
// s1 sinks to its use), s1 before the first cross 701, s0's only 602; both shape pointers
// NULL-initialized then set by an if/else (retail's entry, retail length 2622) 599, with s0's
// global arguments 596, with all global arguments 604; as ternaries with global arguments 716.
// frame 0x64 versus 0x60; shape/basis/scalar homes and nine excess bytes remain.
// Frame 0x64 versus 0x60; shape/basis/scalar homes and nine excess bytes remain.
// Frame 0x64 versus 0x60, primary-shape/basis homes and wave scheduling still differ.
// Earlier: Frame 0x64 versus 0x60 remains; primary-shape, basis and scalar scheduling still differ; nine excess bytes remain.
// frame 0x64 versus 0x60; basis/scalar scheduling remains, retail length recovered.
// Frame 0x64 versus 0x60 remains; primary-shape, basis and scalar scheduling still differ; nine excess bytes remain.
// Earlier: frame 0x64 versus 0x60; basis/scalar scheduling remains, retail length recovered.
// Earlier 647 from 647: frame 0x54 vs 0x60 and camera/source saved registers differ; wave vectors fix the frame but lose overall.
// primary output scale 744; default depth scale 743; alternate depth scales 727; product scale workspaces 734;
// late wave lifetime 743; wave offset workspaces 660; inverse before basis 765; direct integer component 743;
// complete shape branches 743; wave alternate scales 695; wave product scales 692; wave primary scales 659;
// wave basis copy blocks 806; shape discriminant 776; wave shape discriminant 696
// Ordinary camera reads with early/late special cross products 737/747;
// early sine amplitude 738; accumulated maxima 796; common mode publication 743.
// Earlier 604 from 647: Frame 0x60 agrees; scalar/basis scheduling and final integer conversion remain.
// wave with conditional shapes fixed 738; wave with camera offsets 634; wave with early projection
// scales 672; wave with phased cross results 691; shared wave offset 667; camera with early projection
// scales 630; camera with phased cross results 620; camera with shared wave offset 604; camera with late
// inverse 648; shared camera early scales 631; shared camera phased cross 620; shared camera early
// phased 625; shared camera late inverse 618.
// DL134: existing wave origin 599 kept; coordinate/point inputs 631/720; first phase 604;
// first/second/both saved-axis copies 604/604/604; phase with native origin 620.
// Reciprocal/shape/parallel-product names and their five combinations 599;
// reciprocal/source/projection case order and all three together 599;
// wave cross-result/offset homes 599; second input products first 598 (below threshold);
// wave homes with case order 599. Frame 0x64 versus 0x60 and wave scheduling remain;
// original native origin, products, case order and conversion retained.
// consumer-shape 594; direct-primary-field 658.
// bool-shape-domain 633; word-shape-domain 645.
// consumer-shape-wide-special 599; direct-primary-field-wide-special 662.
// bool-shape-domain-wide-special 654; word-shape-domain-wide-special 642.
// Shared double-factor wave scalers first/second/both 671/671/669; with separate
// second displacement 661/670/675; both scalers with shared integer truncation
// 683, with separate displacement 704. Second-scaler forms recover frame 0x60
// (first +6), but regress; original shared scalers and all values restored.
// Round 197: complete first/second input field reads 674/671; both camera offsets at each use 700;
// both inputs 694; first/second input with offsets 661/661; all inputs/offsets 692;
// all observations with read-only secondary shape 692. All retain frame 0x64; original body restored.
// Round 162: Secondary shape null-first/present-first/zero-arm/present-arm forms
// 676/671/594/676; reciprocal constant after the shape 679/672/596/678.
// Original shape selection and reciprocal lifetime restored; frame 0x64 remains.
// Ordinary decision bool/byte/short/long 595/595/605/600; before shape selection
// 721/721/714/727. Original decision polarity, shape scope and arithmetic restored.
// named trig factors 673; projection terms 673.
// camera offset terms 594; trig/projection terms 594.
// trig/camera terms 673; projection/camera terms 673.
// all three term families 594; named modulation 673.
// Round 211: Late special cross 707; reversed sine producers/after-amplitude sine 594/594;
// late cross with either sine order 707/707; cross at first scale 592 (two-point gain dropped);
// first-scale cross with late sine 594; complete native basis arms 727. Original body restored.
// Round 174: consume cross vector in place 673; cross at displacement 707;
// early sine factor 652, with late cross 709; factor after modulation 652, with late cross 709;
// cross after first sine 673, with early factor 652. Original cross/scaler graph restored;
// frame 0x64/0x60 and shape/basis/wave homes remain.
// Camera-shift homes, alternate shift homes, wave-result homes, wave-scale homes,
// and four paired combinations all 594; frame and initial shape homes unchanged.
// Round 201: basis/camera/input-copy consumer scopes, all seven nonempty combinations,
// and the independent wave-scratch end scope 594 each (2631/2622). Original scopes and values restored.
// Round 206: shared vector-add first/second/both wave stages 690/581/690; paired camera
// add-scaled 702, with first/second/both waves 708/703/709; all with shared default scale 708.
// Second-wave vector-add kept at plain 581 (2635 bytes); frame 0x64 and shape/basis homes remain.
// Round 88 (frame now 0x58): both shape pointers NULL then one if/else at entry 622 at retail
// length (retail's entry exactly; the 8-byte frame shift costs every slot); declarations swapped
// 622; test inverted 627; inv after the shapes 621; both ternaries at entry 708.
// Round 275: complete clamp operands 581; named lower/upper-first bounded values
// 747/755; in-place lower/upper-first fields 727/790; explicit result arms 890;
// independent lower-upper/upper-lower bounds 747/755. All restored; current frame
// 0x58 versus 0x60, thirteen excess bytes and native shape/basis/wave homes remain.
// Round 209: Intrinsic copies at history/publication/all outputs 581/581/581; conditional origin 726;
// origin and basis 825; basis 817; ordinary and late input snapshots 581; all boundaries 825. Restored;
// actual frame 0x58/0x60, shape/basis/wave homes remain; original sixth-component conversion unchanged.
// Round 385: Current 0x58-frame basis/wave-vector/wave-scalar homes, all seven subsets, and all with
// the reciprocal home 581 each. Original names and complete body restored; frame 0x58/0x60,
// shape/basis homes and thirteen excess bytes remain.
// Round 227: Complete projected add-scaled values in existing/separate cross outputs 721/721;
// with shared final scales 797/797; second cross at its consumer 699/699,
// with shared final scales 825/825. Original projection graph restored;
// frame 0x58/0x60 and shape/basis/wave homes remain.
// Round 177: Existing vector setters for source-zero/source-other/both/depth/zero+depth/
// other+depth/all 580/700/700/702/700/700/700; all plus native point setters 740.
// One-point gain dropped; original body restored; frame 0x58 versus 0x60 remains.
// Round 180: Separate second displacement, entry projection cross outputs and entry
// uninitialized primary shape, masks 1-7, then all with entry special cross output:
// 656/581/656/581/656/581/656/656. Original executable body restored.
// Round 237: existing integer truncation helper 716; first input scaled in place 581,
// with truncation helper 716; second input scaled in place 593, with helper 716;
// both input scales 593, with helper 716, with direct sixth scalar 716.
// Original snapshots, conversion and complete body restored; frame 0x58/0x60 remains.
// Round 110: retail has no integer truncation of the sixth input product (scale * d.k): written
// as a plain real product 648 (2629 bytes); with both shape pointers set at entry 620 (2622 bytes);
// plus the wave's sine arguments read from the global point, frame 0x60 agrees: 600 (2636 bytes);
// plus the first wave through vector3f_add_scaled 575 (2622 bytes, frame 0x60; v2/v4 homes,
// shape0's slot and the sine evaluation order still differ: not kept, the sines then read the
// global while the waves add to its local copy); that form with the sines reading the copy 660;
// both waves/second wave through vector3f_add_scaled 646/627 (frame 0x54); s1 before the cross
// product or after s0 575/575. The truncation is a behaviour difference retail does not have.
// Round 184: Native source/mode/flag snapshots, masks 1-7, then all plus low-word shape selector:
// [704, 691, 715, 700, 721, 695, 710, 711]. Original selector observations, shape pointers, wave
// helpers, scalar trees and sixth-component integer conversion restored.
// Round 186: Selected native basis receivers, seven subsets and the shared selected source
// 713/680/777/698/755/767/754/800. Original copies, helpers, floating trees and sixth-component
// integer conversion restored.
// Round 403: native double storage for rounded sine/modulation and complete maxima,
// masks 1-7: 580/580/581/756/753/753/756; all with both wave factors 771.
// One-point leads dropped; original scalar precision, frame graph and conversion restored.
// Round 213: existing staged cross for first/final special view and paired projection
// families, seven subsets, 581 each; all with independent second wave displacement 656.
// Original body restored; frame 0x58/0x60 and shape/basis/wave homes remain.
// Round 433: Exhaustive reciprocal result with entry shapes/source case order, four forms
// 611/678/611/678; single denominator counterparts 600/696/600/696. All restored;
// the live-through-wave inverse default is not the missing frame cause.
// Round 317: Existing shared scalar clamp for six parallel rows in mode-family masks 1-7,
// then all with force-inline declaration, 702/702/702/581/702/702/702/702. Scaled
// families grow frame to 0x64; original declarations, inputs and body restored.
// Round 93: truncation removed 648; plus both shape pointers set at entry 620; plus the sine
// arguments from the camera point 600; plus the first wave through vector3f_add_scaled 575
// (2622 bytes, frame 0x60; not kept: the sines read the global while the waves use its local
// copy). On that 575 form: first sine before the cross product 575; second wave through the
// helper 642; second cross product right after the first 679; first sine first with the
// helper 642; `w` declared before v3 or at the top 575/575; renamed locals 575 (identical:
// names and declaration order do not move these slots). Left: slot order (shape0 0x18, v2
// 0x28, v4 0x4c, a wave-shift home at 0x64 against retail's 0x14/0x40/0x64 and no home), the
// x87 sine order (retail rounds both sines to float before the SSE scaling).
// Round 49: the type switch as `if (type == 0) ... else if (type != 1) ...` 580 (retail's layout:
// case 0 inline first); with the sine arguments from the global point 701; with both shape pointers
// NULL then one if/else at entry 638 (entry exact but frame 0x58), plus global sine arguments 593
// (frame 0x60), plus the trig factors as named doubles 591; with the plain sixth product 638.
// Round 26: s1 from the global point with the 8.2x phase, sin x and both cosines as named `real`
// locals 701, as `double` locals 694 (forward-substituted, no factor stack); the same reading the
// local copy v1: `real` 581 (identical), `double` 651; both shape pointers NULL then one if/else 622
// (2622 bytes), plus the sines from the global point 604 (frame 0x60). Retail rounds s0 to float before
// an SSE `* 0.03`; ours folds `* 0.5 * 0.03` in x87.
// Round 40: `if (shape0) inv = ... else inv = ...` in the first mode switch 707 (2663 bytes); input copies d then c
// 580 (one point); the maxima as one expression 760; camera shifts read before the old20/old30 copies 581 (same
// code); md before mc 666.
// Round 460: Native three-component basis-copy loops v2/v3/v4, subsets 1-7, then all with origin,
// 688/725/716/714/763/757/777/803. Original copies and body restored; frame 0x58/0x60,
// shape/basis homes and thirteen excess bytes remain.
// Round 259: Entry uninitialized scaled-product/maxima/wave-scalar storage, seven subsets, then all plus the existing truncation carrier
// 581/581/581/581/581/581/581/581. Original body and producer sites restored.
// Round 480: Native library-copy origin/basis/fallback-axis domains, masks 1/2/4/3/5/6/7,
// then all with late input copies, 581/581/581/580/581/581/580/580. One-point lead dropped;
// original body restored; frame 0x58/0x60, shape/basis homes and excess bytes remain.
// Round 286: Native library maximum boundaries in first/second/final producer
// masks 1/2/3/4/5/6/7 give 747/760/748/790/800/769/771; all with reversed
// component-maximum production 771. Original maxima, includes and body restored.
// Round 211: existing double-factor scaler for complete primary/depth/displacement
// families and combinations 632/694/699/684/702/682/694; all with both wave scalers 768.
// The primary/depth pair reaches 2622 bytes at 684, frame 0x64; no frame 0x60 recovery.
// Original producers, shape/basis homes and sixth-product conversion restored.
// Round 259: Entry uninitialized scaled-product/maxima/wave-scalar storage, seven subsets, then all plus the existing truncation carrier
// 581/581/581/581/581/581/581/581. Original body and producer sites restored.
// Round 444: Double inverse/paired camera shifts/complete projection factors, subsets
// 1/2/4/3/5/6/7 then all with both wave factors: 774/665/716/815/820/750/877/878. Original precision, helper graph
// and sixth-component integer conversion restored; frame and shape/basis homes remain.
// Round 480: Native library-copy origin/basis/fallback-axis domains, masks 1/2/4/3/5/6/7,
// then all with late input copies, 581/581/581/580/581/581/580/580. One-point lead dropped;
// original body restored; frame 0x58/0x60, shape/basis homes and excess bytes remain.
// Round 75 (lane 12): retail stores both sines as floats and scales them in SSE (s1's fsin at its definition);
// ours folds `* 0.03`/`* 0.025` into x87. Only a sine with several uses is stored: the waves per
// component with the scale product repeated give retail's x87/SSE split but 678 (frame 0x50); with
// the plain sixth product 664, with the entry shape pointers 624, with both 635. The trig factors (and
// the 8.2x phase) as named doubles 651; named reals, an inline four-factor helper, sinf/cosf and the
// global point are forward-substituted (compile-only screens). Retail keeps the factors on the x87
// stack (fmul st(1)/st(2), fadd st(3), three late pops), order 8.2x, sin x, cos 7.8y, cos 7.75y.
// Round 509: Native horizontal/vertical/depth output receivers, subsets 1/2/4/3/5/6/7,
// 722/722/588/720/722/722/719; all with depth-offset receiver 728. Main receiver homes add
// four frame bytes but regress; original complete body and output receivers restored.
// Round 512: Complete two-bit mask admission with full-kind/projection-mode/flag
// snapshots, masks 0/1/2/4/3/5/6/7: 677/596/691/671/691/699/598/703.
// Original decision and complete body restored; shape/basis/wave homes remain.
// Round 237: Existing real-factor output scales on the retained second-wave addition graph:
// primary/depth/displacement families, masks 1/2/4/3/5/6/7, 580/702/702/701/700/701/694; all with
// complete native input scaling 693. Older real-factor forms predate this wave/frame graph.
// Original scalar expansions, inputs and integer conversion restored; frame 0x58/0x60 and shape/basis/wave homes remain.
// DL238: Complete shared six-channel clamp with branch-owned attenuation and long/dword/short/byte
// output modes 959/959/956/962; selected-factor counterparts 997/997/994/1000. Original products,
// repeated clamp arms, mode publication and sixth-component conversion restored; frame and
// shape/basis/wave homes remain.
// Round 333: Perspective rows through separate existing scale/add producers, cross/output
// receivers, complete rows/paired phases and separate/nested returned receivers:
// 772/772/844/766/925/837/909/839. All regress; original perspective expressions, helpers,
// frame graph and sixth-component conversion restored.
// Round 540: Returned scaler output consumed by first/second/both wave adds and
// point-setter/second-return contract 690/581/690/581; complete read-only basis inputs
// on those graphs 690/581/690/581. Frame stays 0x58; first-wave shared addition grows
// to 2654 bytes. All producers, basis contracts and complete body restored.
// Round 545: Native indexed input scaling/copy producers, first-scale/second-pair/copy
// subsets 1/2/4/3/5/6/7 and complete unsigned-counter graph 581/581/699/581/699/699/699/699.
// Original scalar producers, helpers, sixth conversion and body restored; frame 0x58/0x60 remains.
// Round 139 (m3 lane 5, fast; both ret 0x20 now): both shape pointers NULL then if/else 622 (2622 bytes), inv after
// them 636, shape1 declared first 622; the 575 composite (plain sixth product, entry shapes, global sine arguments,
// first wave through vector3f_add_scaled; six, under the bar) re-measured 575; on it the four trig factors as named
// doubles 615 (forward-substituted), cos 7.75y first in the sum 575, the inner product parenthesised 575, wave_shift
// declared before s1 575, second wave through vector3f_add_scaled too 642 (2600 bytes).
// Round 548: Complete memmove history/point-publication/fallback-axis boundaries,
// subsets 1/2/4/3/5/6/7, then all with both input snapshots: 709/688/677/675/725/698/659/694.
// NOT kept: library/clobber/frame shape misses; every native copy, include and full body restored.
// Round 160 (fast): both shape pointers NULL then if/else, either order, 622/622 (2622 bytes); plus the global sine
// arguments 604 (2642), plus the first wave through vector3f_add_scaled 578 (2626), the second wave instead 622; the
// sine arguments and first wave without the entry shapes 670; the 575 composite re-measured 575, on it amp written
// into the call 629 (2606), k written into the call 583, both 642 (2595), the cross product between s1 and s0 575;
// the plain sixth product alone 648. The composite stays out: its sines read the global while the waves use a copy.
// Round 203 (fast): ret 0x20 agrees (frame 0x58/0x60 and the entry shape homes remain); both shape
// pointers as entry ternaries with the default case reading shape0 708 (2650 bytes); the wave scales
// written inside the two scale calls 670; the first cross product at function scope 581 (same code);
// the inverse declared after shape1 583; the special flag computed first 715 (2657 bytes).
// Round 146 (m3 lane 5, fast; aligned 461 at 581): the 575 composite (plain sixth product, entry shapes, global sine
// arguments, first wave through vector3f_add_scaled; six, under the bar) re-measured 575 (2622 bytes, aligned 409); on it the
// sines through a named pointer to the global point 578; s1 from the local copy with s0 from the global 652 (2587 bytes);
// staged first/second cross 575/575; a separate vector for the second wave 575; the first-wave vector declared after its
// add 575; the vector locals renamed so retail's low-to-high slot order sorts by name, ascending and descending, 575/575.
// Y572: Coherent existing cross/sum forced/ordinary/plain declaration pairs,
// all eight nonoriginal policies: 581 each. NOT kept: frame remains 0x58/0x60;
// complete target and both helper bodies unchanged, original declarations restored.
// Y582: Standard array transfers for existing v2/v3/v4, seven nonempty subsets,
// then all with ordinary w transfer: 676/736/661/739/719/749/765/803. Full basis
// recovers frame 0x60 but grows to 2712 bytes. NOT kept: all regress; original
// native copies, helpers, integer conversion and complete body restored.
// AC547: Special-cross W receiver 581; first/both perspective input receivers
// across all reuse subsets 676/682/697/697/682/682; all with second projection
// first 662. NOT kept: no native frame/residence match; original body restored.
// Y585: Complete first/second/combined branch-owned maximum result subsets
// 1/2/4/3/5/6/7: 581 each; all with fallback-first arms 662. NOT kept:
// no frame recovery or eligible gain; original maxima and complete body restored.
// Y587: Actual two-axis cross table for traversal long/dword/short/word/byte/
// signed-char, then long/short while: 728/728/803/803/803/803/728/803.
// NOT kept: table/output residence regresses and grows; original direct calls and full body restored.
// M3 lane 4 r167 (fast, one-file link with ballast exact at 581, 3 s a form; masked aligned 204): entry shapes and global sine reads and
// a plain sixth product 600 (2636 bytes, aligned 392 against 460); entry shapes alone 622 (listed, 2622 bytes, aligned 396); the four trig
// factors as named doubles there 601 (2630 bytes), the 8.2 argument as a named double with the three other factors named 629 (2632 bytes,
// aligned 391, shape 203), as a real 600, the same with the 8.2 product written (x * b) * c order 600, the factors as reals with a double argument 628
// (2632 bytes). Retail's s0 leaves sin(x), cos(7.8y) and cos(7.75y) on the x87 stack and pops all three dead after
// the product (fstp st(2), st(1), st(0)), with fsin of the 8.2 argument done last; no form here forward-substitutes them that way yet.
// M3 lane 4 r170 (one-file harness with the ballast object, exact at 581; masked aligned in brackets, base [209]). Kept 537 (plain 536
// with a named `type` local, not kept: it mixes a local copy with direct field reads): both shape pointers NULL then one if/else at
// entry, the type switch as `if (source->type == 0) ... else if (source->type != 1)`, the special-flag arm first in the
// `0x20 & flags` test, the second wave through the existing vector3f_add_scaled. Each part removed alone: 706/631/602/629.
// `sin(v1.y + v1.x * 5.6f)` 537 (identical, dropped). Grid on the kept form: first/second wave as scale+copy, add_scaled, direct
// `+=` either operand order, scale+vector3f_add; amp/k named or inline; cross/sine statement orders; default-arm declaration and
// row orders: best 536 (s0 first, amp and k written inline). Type `if` with a local read everywhere 699. Older forms on this base:
// type if 580 [175], flag arm first 673 [198], both 670 [164], plain sixth product (behaviour fix) 648 [192], both with it 617.
// x87 lead (diagnostic, not kept): retail's non-popping `fmul st(1); fmul st(2); fadd st(3)` and the three late `fstp st(k)` pops
// appear only when sin x, cos 7.8y and cos 7.75y have a second use that is removed late: a dead write through a pointer to a local
// gives retail's x87 sequence exactly with the 8.2 argument named first (frame 0x60, [111], 656 at 2587 B); `if (false)`, an empty
// inline taking them, unread aggregate members 620; `if (!special)` same pops, frame 0x64. An inline helper filling a local wave
// vector from the global point (first sine, then the named factors, the unused third component written through a copy of the
// result pointer) gives the same sequence (659 at 2584 B): retail's direct global reads may come from such a helper.
// MiniMax D (11-10): 594 (was 661): the wave sines read the global point and each wave scale is written at its
// use; the frame is still 0x54 against retail's 0x60 and the sines are still not evaluated in retail's order.
// Retail's two crosses share their products, so its cross result is scalarized while its second cross is a
// vector; forms tried: the two wave values as an array 604, the cross with shared products written out 677,
// the cross moved after the two sines 655, all worse than 594 and reverted.
// At 661 (was 537): behaviour and helper fixes from the b2E walk, each raising the score: the sixth input product is
// a plain mulss as at 0x49f0d (the integer truncation was +78: 537 -> 615); the camera shifts and the vector scales
// call vector3f_add_scaled / vector3f_scale (+10, 2622 bytes); the wave's first add calls vector3f_add_scaled (+11);
// the sine of x * 5.6 + y keeps the sum on the x87 stack as at 0x498a7-0x498d7 (+25, then the sine is stored as a
// float and scaled by SSE as retail does). Every statement of retail was walked against the source: the rest agrees.
// Still differs: frame 0x64 against retail's 0x60 (2582 bytes against 2622); s0 and s1 are x87 values that retail
// rounds to float before the SSE scale by 0.03 and 0.025 (ours folds 0.5 * 0.03 in x87 for s0, a double rounding
// apart: ~1 ulp); retail leaves sin x, cos 7.8y and cos 7.75y on the x87 stack (0x49951-0x4997c) and reads the
// sine arguments from the global point at 0x4b9da0 (with the global named in the sines 654, not kept: no behaviour).
// Tried on this base: (double) casts on the other sine arguments 651/656; double constant 5.6 650; the argument
// as a named double 650; s1 before the cross product / after s0 650/650. Kept spelling (double)(x * 5.6f) + y: same score,
// and fld/fmul dword/fadd dword/fsin as retail (the (double)x * 5.6f spelling gave fmul qword). s0 stays double-rounded: retail
// 0x4995f stores the x87 sum * 0.5 as a float and 0x49963-0x49969 scales it by SSE; ours does fmul 0.5, fmul 0.03, fstp.
// M3 lane 3 r225 (fast = plain 661 here; [exact, stack-masked, register-masked] aligned counts, base [654, 592, 180]; the frame is now
// 0x5c against retail's 0x60, not 0x64, and both return with ret 0x20): global sine arguments with the 8.2 argument and the three
// factors as named doubles 665 (2575 bytes, frame 0x58) [646, 601, 176]; global sine arguments alone re-measured 654 (2579, frame
// 0x58) [655, 582, 178]. Perspective rows (retail scales r by the first reciprocal, then s by the second, then forms inv * 0.5):
// vector3f_add_scaled into r and s with named reciprocals and a named half inverse, then two vector3f_scale 677 (2568) [576, 583, 181];
// the same with the reciprocals and inv * 0.5f written in the calls 664 (2574) [610, 590, 139]; rows in place then two vector3f_scale
// 671 (2622 bytes, retail's length) [562, 557, 163]; every row as (a * b + c) * k 661 (same code). With the global sines: calls form
// 659 [618, 579, 129], in-place form 673 (2626) [549, 545, 153]. The two wave values in one two-float local read from the global
// point 641 (2602 bytes, frame 0x64) [642, 587, 175], as a two-element array 642; with the calls form 639 [622, 583, 128], with the
// in-place form 692 (2645). Not kept: a local that only groups the two values, and no match. Lead: retail stores both sines as
// floats at once (0x4990e, 0x4995f) and keeps the cross product ahead of the first fsin, as if they were written to memory the
// compiler could not sink; with the global arguments in plain locals our build sinks the first sine past the second.
// m3 lane 12 r106 (fast): with offsets and registers normalised the listings differ only in the special arm (global
// sine arguments, retail's x87 order, the cross products' order); the rest of the 661 is the 4-byte frame shift
// (sub esp 0x5c against 0x60, a cross-product spill at [esp+0x14] pushing shape0 to +0x18). On the global sine
// arguments (654, listed): s1 before the first cross and cross3f_staged for c 654 (frame 0x58, same code as without
// the reorder); plus cross3f_staged for w 655; w staged with c plain 654; c staged with s1 after the cross 654; both
// staged with s1 after the cross 655.
// M3 lane 8 r151 (fast; control 594 = plain): kept `real h = 0.5f * ((shape1->f2c - shape1->f28) * 0.025f);` 536 plain (2622
// bytes, retail's length). The earlier `(shape1->f2c - shape1->f28) * 0.025f * 0.5f` was folded into one multiply by 0.0125;
// retail multiplies by 0.025 and then by 0.5 held in a register, as this form does (masked aligned count unchanged at 104;
// the rest is the frame). Also: s0's four sine/cosine arguments as doubles (no float overloads; retail rounds none of
// them) 685 (2564 bytes, frame 0x58, the factors still pop), with 0.5 as a double 685; h as `* 0.025f / 2.0f` 594 (same
// code); the two shift fields read in the calls, no locals, 596. On 536: the shift fields in the calls 537; `0.5f * inv`
// in the default arm or in the perspective rows 536/536 (same code); sy computed after the v20 rows 550; the six input
// products all `scale * x` or all `x * scale` 536/536 (same code). Retail reads the four wave constants (8.2, 7.8,
// 7.75, 5.6) only here, so no shared retail helper accounts for the dead second uses of sin x, cos 7.8y and cos 7.75y.
// M3 lane 13 r81 (fast = plain 536; aligned exact/fully masked counts, base 519/104): perspective rows in place with each
// reciprocal just before its rows, k after both, then two vector3f_scale 612 (2622 bytes, aligned 452, masked 107: the later
// code then aligns with retail apart from inv in xmm3 for retail's xmm5, but the rows store and reload r and s, 8 extra
// instructions); the same through vector3f_add_scaled(&v2, &r, sx, &r) 637 (2604 bytes); six named row sums in that order 536
// (forward-substituted); the 8.2 argument as a named double on this base 585; s0 as `0.5f * (...)` 536 (same code); the
// second cross w computed first, ahead of c, 685 (2596 bytes).
// T86: Separate wave-origin copies, current/global before/after the cross,
// 715/646/715/646; coordinate construction 633; library copy 715; separate
// phase coordinates before/after the cross 626/619. NOT kept: no equality;
// native frame and wave residence remain; original full body restored.
// T95: Plain three-channel final-clamp loops over actual scaled input records,
// mode subsets 1/2/4/3/5/6/7 and all with unsigned ordinals: 618/617/558/656/650/632/647/647.
// NOT kept: no exact scalarized output-record graph; original six scalar products,
// clamp arms, shape/wave helpers and complete body restored.
// P436: Fixed existing view/projection/side basis, special result, both waves, both
// channel inputs, all six channel products and the complete maximum tree: eight fast
// 536s, control 536. NOT kept: frame stays 0x54 versus native 0x60 with no byte change;
// original complete body, every expression and all shared helpers restored.
// P439: Standard lower/upper six-row mode selections, masks 1/2/4/3/5/6/7,
// then complete upper-only selection: 952/940/896/1200/1145/1129/1319/1380. NOT kept: native shape/inverse,
// basis/wave homes and frame remain unresolved; original maxima, clamps, every
// floating tree, helper and complete body restored. Every form adds 12-36 calls
// against native zero while retaining frame 0x54 versus 0x60.
// P441: Existing selected v2/v3/v4 setter producers, singles/pairs/all then point setter: 693/713/646/734/701/698/758/719.
// NOT kept: Native 0x60 frame, selected-basis homes and late x87 factor pops remain. Original complete body restored.
// P462: Ordinary defaults then special overwrite for origin/view/projection/side/
// cross-output, then basis/origin-basis/all: 739/699/698/689/678/805/784/803.
// Cross-output default recovers frame 0x60 and extent 2622, first +6, but regresses.
// NOT kept: no equality; original selections, helpers, math and full body restored.
// T103: Actual first/second/both wave-position results, point then vector receivers,
// and reused sequential position receivers: 522/692/687/522/692/687/693/693.
// NOT kept: first-result forms recover frame 0x60 but grow five bytes and use
// nonmatching one-use results; original helpers and complete body restored.
// P465: Positive special-view result from native lazy tests, bool/byte/short/long,
// false-default 548/548/668/561; exhaustive test-owned 644/644/657/651. NOT kept:
// no native 0x60 frame/result recovery or equality; original complete body restored.
// P468: Special defaults then ordinary overwrite for origin/view/projection/side,
// origin-view/view-projection/basis/origin-basis: 716/686/701/718/720/731/787/815.
// NOT kept: frame/basis/wave residence remains unmatched and all regress;
// original complete body and helpers restored.
// @retail 0x497a0
void function_0497a0(
	s_view_source *source,
	s_view_result *result,
	s_view_camera *camera,
	s_view_flags *flags,
	vector3f const *c_in,
	vector3f const *d_in,
	real scale,
	long *mode,
	vector4f *old20,
	vector4f *old30)
{
	real inv = -1.0f;
	s_view_shape_0 *shape0 = NULL;
	s_view_shape_1 *shape1 = NULL;
	if (source->type_low == 0)
		shape0 = &source->shape0;
	else
		shape1 = &source->shape1;

	bool special = ((0x20 & flags->flags)) && (0x40000 & flags->flags);
	point3f v1 = special ? g_4b9da0 : source->v2c;
	vector3f v2 = special ? g_4b9dac : camera->v28;
	vector3f v3 = special ? g_4b9dac : (shape1 ? shape1->v4 : camera->v28);
	vector3f v4 = special ? g_4b9db8 : camera->v34;
	vector3f w;

	if (special)
	{
		vector3f c;
		cross3f(&v4, &v2, &c);
		/* retail 0x498a7-0x498d7 keeps x * 5.6 + y on the x87 stack up to fsin (no float rounding of the sum) */
		real s1 = (real)sin((double)(g_4b9da0.x * 5.6f) + g_4b9da0.y);
		real s0 = (real)((sin(g_4b9da0.x * 8.2f) * cos(g_4b9da0.y * 7.8f) * sin(g_4b9da0.x) + cos(g_4b9da0.y * 7.75f)) * 0.5f);
		vector3f_add_scaled((vector3f const *)&v1, &v4, s0 * 0.03f, (vector3f *)&v1);
		vector3f_add_scaled((vector3f const *)&v1, &c, s1 * 0.025f, (vector3f *)&v1);
		cross3f(&v2, &v4, &w);
	}
	else
	{
		w = source->v1c;
	}

	switch (flags->w8e)
	{
		case 1:
			if (shape1)
			{
				inv = -1.0f / (shape1->f2c - shape1->f28);
			}
			break;

		default:
		{
			inv = -1.0f / (shape0 ? shape0->f0c : shape1->f2c);
			break;
		}
	}

	if (source->type != 1)
	{
		result->p0 = v1;
		result->f0c = 1.0f;
		result->p10 = v1;
	}

	if (source->type == 0)
	{
		vector3f_scale(&source->v1c, inv, &result->v20);
		vector3f_scale(&camera->v34, inv, &result->v30);
		result->f2c = 0.0f;
		result->f3c = 0.0f;
	}
	else if (source->type != 1)
	{
		vector3f r;
		vector3f s;
		cross3f(&v3, &v4, &r);
		cross3f(&v3, &w, &s);
		real sx = -1.0f / shape1->f14;
		real sy = -1.0f / shape1->f18;
		real k = inv * 0.5f;
		result->v20.i = k * (r.i * sx + v2.i);
		result->v20.j = (r.j * sx + v2.j) * k;
		result->v20.k = (r.k * sx + v2.k) * k;
		result->v30.i = k * (s.i * sy + v2.i);
		result->v30.j = k * (s.j * sy + v2.j);
		result->v30.k = k * (s.k * sy + v2.k);
		result->f2c = 0.0f;
		result->f3c = 0.0f;
	}

	switch (flags->w8e)
	{
	default:
	{
		vector3f_scale(&v2, inv, &result->v40);
		result->f4c = inv * 0.5f;
		result->v50 = *g_4687a4;
		result->f5c = 0.0f;
		break;
	}

	case 1:
	{
		vector3f const * up = special ? &g_4b9dac : &camera->v28;
		if (source->type != 1)
		{
			vector3f_scale(up, inv, &result->v40);
			result->f4c = inv * (shape1->f28 + source->f28);
			if (0x20 & flags->flags)
			{
				result->v50 = *g_4687a4;
				result->f5c = 1.0f;
			}
			else
			{
				real h = 0.5f * ((shape1->f2c - shape1->f28) * 0.025f);
				vector3f_scale(&result->v40, h, &result->v50);
				result->f5c = (source->f28 + 0.0078125f) * h * inv + 0.5f;
			}
		}
		break;
	}
	}

	*old20 = *(vector4f *)&result->v20;
	*old30 = *(vector4f *)&result->v30;
	real side_shift = camera->f6c;
	real vertical_shift = camera->f70;
	vector3f_add_scaled(&result->v20, &result->v40, side_shift, &result->v20);
	vector3f_add_scaled(&result->v30, &result->v40, vertical_shift, &result->v30);

	vector3f c = *c_in;
	vector3f d = *d_in;
	real ca = scale * c.i;
	real cb = scale * c.j;
	real cc = c.k * scale;
	real da = (d.i * scale);
	real db = d.j * scale;
	real dc = scale * d.k;
	real mc = MAX(ca, MAX(cb, cc));
	real md = MAX(da, MAX(db, dc));
	real m = MAX(mc, md);

	if (m > 2.0f)
	{
		result->f60 = PIN(ca * 0.25f, 0.0f, 1.0f);
		result->f64 = PIN(cb * 0.25f, 0.0f, 1.0f);
		result->f68 = PIN(cc * 0.25f, 0.0f, 1.0f);
		result->f70 = PIN(da * 0.25f, 0.0f, 1.0f);
		result->f74 = PIN(db * 0.25f, 0.0f, 1.0f);
		result->f78 = PIN(dc * 0.25f, 0.0f, 1.0f);
		*mode = 2;
	}
	else if (m > 1.0f)
	{
		result->f60 = PIN(ca * 0.5f, 0.0f, 1.0f);
		result->f64 = PIN(cb * 0.5f, 0.0f, 1.0f);
		result->f68 = PIN(cc * 0.5f, 0.0f, 1.0f);
		result->f70 = PIN(da * 0.5f, 0.0f, 1.0f);
		result->f74 = PIN(db * 0.5f, 0.0f, 1.0f);
		result->f78 = PIN(dc * 0.5f, 0.0f, 1.0f);
		*mode = 1;
	}
	else
	{
		result->f60 = PIN(ca, 0.0f, 1.0f);
		result->f64 = PIN(cb, 0.0f, 1.0f);
		result->f68 = PIN(cc, 0.0f, 1.0f);
		result->f70 = PIN(da, 0.0f, 1.0f);
		result->f74 = PIN(db, 0.0f, 1.0f);
		result->f78 = PIN(dc, 0.0f, 1.0f);
		*mode = 0;
	}
}


real function_17ca10(real value, short curve);

// @retail 0x42c50
real function_42c50(point3f const *position, vector3f const *normal, short mode, byte const *flags)
{
	(void)&flags;
	real result = 1.0f;
	if (mode)
	{
		real x = g_4b9da0.x - position->x;
		real y = g_4b9da0.y - position->y;
		real z = g_4b9da0.z - position->z;
		result = (real)fabs((z * normal->k + y * normal->j + x * normal->i) / sqrt(z * z + y * y + x * x));
		if (*flags & 0x40)
			result = function_17ca10(result, 2);
		if (mode == 2)
			result = 1.0f - result;
	}
	return result;
}


extern s_record_pool *g_4ea940;
bool function_bad50(long object_index, long index, point3f *out);

struct s_ribbon_source
{
    byte unknown00[8];
    long object_index;
    short marker_index;
    byte unknown0e[6];
    real scale;
    byte unknown18[4];
    real u, v;
    byte unknown24[12];
    short point_count[4];
    long point_index[4];
};
struct s_ribbon_state
{
    byte unknown00[0x18];
    real width;
    color4f lower, upper;
    dword flags;
};
struct s_ribbon_definition
{
    word flags00, flags02;
    byte unknown04[0x14];
    short mode;
    byte unknown1a[2];
    real u_step, v_width;
    byte unknown24[0xc4];
    long state_count;
    s_ribbon_state *states;
};
struct s_ribbon_point
{
    short salt;
    byte flags;
    char state;
    real transition, unknown08, scale;
    byte unknown10[0xc];
    point3f position;
    vector3f velocity;
    long next;
};
struct s_ribbon_input
{
    s_ribbon_source *source;
    s_ribbon_definition *definition;
    short slot, unknown0a;
    byte *shader;
    bool normal;
    byte unknown11[3];
    long vertex_count;
};
struct s_ribbon_vertex
{
    point3f position;
    real u, v;
    dword color;
};

PRIVATE __forceinline dword ribbon_pixel(color4f const *color)
{
    long b, g, r, a;
    real scale = 255.0f;
    dword pixel = 0;
    __asm
    {
        mov edx, color
        fld dword ptr [edx]
        fld dword ptr [edx + 4]
        fld dword ptr [edx + 8]
        fld dword ptr [edx + 12]
        fld scale
        fmul st(4), st(0)
        fmul st(3), st(0)
        fmul st(2), st(0)
        fmulp st(1), st(0)
        fistp b
        fistp g
        fistp r
        fistp a
        mov edx, b
        mov ebx, g
        mov ecx, r
        mov eax, a
        shl ebx, 8
        shl ecx, 0x10
        shl eax, 0x18
        or edx, ebx
        or edx, ecx
        or edx, eax
        mov pixel, edx
    }
    return pixel;
}

// MiniMax G (11-12): no new form, no lower score (233): the frame is four bytes short (0xa8 against retail's 0xac), which puts the
// definition, source, scale and vertex homes four bytes below retail's, and retail's u step, v and width are in the eight-byte spill
// slots at the top of its frame in another order than ours; no new spelling of the three scalars moves them.
// score 233 (was 233): earlier 250; frame 0xa8 versus retail 0xac; first-edge Y, half-width, cursor and color homes differ.
// MiniMax C (11-11): no new form, no lower score (233); the frame is still 0xa8 against retail's 0xac, and the u
// step and the V end still take 4-byte homes among the channel slots where retail spills them to the 8-byte homes
// at +0xa4 and +0xac, so the body stays four bytes short after case 4; a permuter run over this function (300 s,
// 31 tries, reduced image) found nothing below its 230 baseline, and the case-1/2 second x load and case 4's
// per-arm position pointer remain the open leads.
// Round 16: the argument registers agree (ECX is the source, EDX the result, the camera and the
// flags on the stack); retail keeps the vertex cursor in EDI and ours leaves it in its argument
// slot, and retail reaches the endpoint handling by a forward branch from the loop.
// MiniMax D (11-09): the colour written component by component gives 447/604/623 against 250, and the
// interpolation unrolled on its own gives 250; the scope moves give 620/250/620 and the case-0 and
// colour store orders 246/248/244/250/255. The best is 244, under the bar, so the body is unchanged.
// frame 0xa8 versus retail 0xac; first-edge Y, half-width, cursor and color homes differ.
// MiniMax D (11-05): four declaration orders for the u and v scalars and their step and width partners
// give 274/287/285 against 265; the frame stays 0xa8 against retail's 0xac, so no lower score.
// MiniMax E (11-06): no new form, no lower score; retail's frame is 0xac against our 0xa8. Retail
// keeps u at frame +0x50, v at +0x80, the u step at +0x94 and the v width at +0x9c, ours keeps u at
// +0x48, the u step at +0x80, the v width at +0x88 and v at +0x90, and retail keeps the state width
// in a slot of its own at +0x3c where ours keeps the vertex pointer. Declaring v before the u step,
// with retail's statement order kept, and moving the state width out of the inner scope both give
// the same code and the same 250.
// Earlier (265): frame 0xa8 versus retail 0xac; first-edge Y, half-width, cursor and color homes differ.
// Earlier: Lane 4 r155 reached 230 (not kept by the coordinator, see below): frame 0xac agrees (the u/v texture scale as a vector2f: retail's
// dead home just below v + width is its unused i), case 1/2's side direction is a vector2f normalized by the existing
// normalize2d_fast, and case 0's second point takes XMM0-2 as retail; the first point's z/x/y and the half width take
// XMM4/XMM5/XMM6 and XMM3 where retail has XMM3/XMM6/XMM5 and XMM4; the side's home is one slot above retail's (u and the side
// swapped), the vertex cursor and state width homes are swapped, and case 4's side length order (retail k first) remains.
// Earlier (265): frame 0xa8 against retail's 0xac (ours now spills
// case 0's first.y as retail, one slot lower); the first point and the half width take XMM5/XMM6/XMM4 and XMM3
// where retail has XMM6/XMM5/XMM3 and XMM4; local slots are permuted (vertex cursor and state width swapped,
// pixel and u/v homes). Earlier (276): length 2596 agrees; frame 0xa0 against retail's 0xac (retail spills the first
// length 2596 agrees; frame 0xa0 against retail's 0xac (retail spills the first
// length 2596 agrees; frame 0xa0 against retail's 0xac (retail spills the first
// Earlier (276): length 2596 agrees; frame 0xa0 against retail's 0xac (retail spills the first
// Earlier 276 (was 276): length 2596 agrees; frame 0xa0 against retail's 0xac (retail spills the first
// length 2596 agrees; frame 0xa0 against retail's 0xac (retail spills the first
// edge point's y at +0xb4); retail gives the second edge point XMM0-2 and the half width XMM4 where
// ours gives the first point XMM0/1/4 and the half width XMM6, which moves every geometry case; slots
// differ (state width +0x54 against +0x4c, tint +0x7c against +0x78). The state clamp is now retail's
// (index in the state's register, jle) with the if/else clamp, and cases 1/2 compute dy first.
// Earlier (512): frame 0x9c against retail's 0xac (retail has a second home for the blended
// width, a spill of the first edge point's y and the width at +0x4c where ours rotates it to +0x2c),
// the pool global reloaded per use, the zero colour stored as integers and the 0x80 flag test.
// earlier: the frame 0xac and the body length 2596 agree; retail stages
// the definition in EBX and the output vertices in EDI, ours stages the definition
// in EDI, and retail places the endpoint handling after the loop where ours places
// it before.
// frame 0xac agrees; definition/vertex registers, endpoint-first loop layout and
// color/vector homes differ. Tried definition first 620; paired vertex store order 636; zero color record
// Frame 0xac agrees; definition/output carriers, endpoint placement and color/vector homes remain.
// Tried definition first 620; paired vertex store order 636; zero color record
// 626; shared position setters 621; second output element 640; single result exit 631; plain point chain for
// 620; color factor result 626; shared cross products 615; shared direction difference 620; native vector
// role homes 620; native point initialization 623; loop vertex store order 620; shared geometry helpers 615;
// shared cross position setters 617; shared cross zero color 622.
// round 106: Frame 0xac agrees; definition/vertex carriers, endpoint placement and color/vector homes remain. Indexed vertex output 697; clamp branches 622; color pointer walks 615; geometry case four first 626; scales before coordinates 615.
// Iteration geometry scratch 615; iteration tint output 615; shared transition value 630;
// width after initial color 616; live input definition reads 626.
// Indexed vertex output 697; clamp branches 622; color pointer walks 615; geometry case four first 626; scales before coordinates 615.
// Advancing output cursor 664; segment/camera-delta/plus-minus point names 615/615/615; second
// position copied from first 629; byte state flags 615; true result and one exit 647.
// Paired displacement loop 615; side scaling loop 615; normal scaling loops 615; RGB tint loop 615; planar geometry cases first 615.
// Width-path result 626; scale override 622; unsigned point handle 615; final endpoint base 634; two-use packed pixel 615.
// Earlier form: round 106: Frame 0xac agrees; definition/vertex carriers, endpoint placement and color/vector homes remain. Indexed vertex output 697; clamp branches 622; color pointer walks 615; geometry case four first 626; scales before coordinates 615.
// Earlier form: round 108: definition/output carriers, endpoint placement and color/vector homes remain.
// Persistent workspaces 620; fused color-component loop 643; shared geometry neighbor 620;
// half-width before tint 620; valid-state-first arms 616.
// Current position field pointer with read-only point/state/source contracts,
// all eight combinations 647; frame 0xac and 2632 bytes. Original body restored.
// round 106: Frame 0xac agrees; definition/vertex carriers, endpoint placement and color/vector homes remain. Indexed vertex output 697; clamp branches 622; color pointer walks 615; geometry case four first 626; scales before coordinates 615.
// round 108: definition/output carriers, endpoint placement and color/vector homes remain.
// Round 349: Conversion-local names 615; source pointer rereads 624; normal-output flag 642; position memcpy 615; shared in-place scales 615 (fast).
// Round 211: Lower native/library color seed 684/684; upper native/library seed 661/661;
// complete difference/scaled-difference stages 658/658; upper seed with difference/scaled
// stages 707/700. Original separate color phases, helpers and output ownership restored.
// Native upper bound/point scale/both 615/626/626; state-array receiver/with upper bound 615/615; scale
// plus array/all inputs/all role homes 626/626/626. All restored.
// U/V entry ordering, seven combinations 615; definition then source 615.
// Round 201: state/geometry/both consumer-end scopes 615/615/615; per-point scratch end,
// with state/geometry/both endings, and combined scopes with library tint copy 624 each (2604 bytes).
// Original scratch scopes, tint initialization, helper calls and endpoint masks restored.
// Round 206: Whole camera-facing normal/side normalization through the existing two inline helpers,
// all eight nonoriginal combinations [615, 615, 620, 620, 620, 620, 620, 620]. Grouping changes require an exact match; all restored.
// Round 385: Final vertex extent/endpoint flags/both 618/622/626; native opacity alone/with
// extent/flags/both 624/625/630/628; consistent next handle 634. All restored; frame/carrier, endpoint
// placement and color/vector homes remain.
// Round 227: Geometry point producers: camera plus scaled-add/minus setter/both
// 615/615/615; planar setters alone/with plus/with minus/with both
// 615/615/615/615; all with paired displacement values 630.
// Original point producers restored; definition/output carriers and color/vector homes remain.
// Round 177: Existing x87 conversion with C++ integer expression/accumulator/parallel terms/
// unsigned channels/channel declaration order/late packed result/combined forms 624 each;
// existing complete packer call 728. All restored; definition/output carriers and endpoint layout remain.
// Round 180: First/transition/both short channel indices, first/transition/both word indices,
// both byte and both dword indices: 638/657/739/638/657/739/739/615. Original executable body
// restored.
// Round 396: Direct state-range admission, invalid/valid arm first: native char
// 637/637, short 633/633, long 647/647, consistent field observations 624/624.
// All restored; definition/output carriers, endpoint placement and frame homes remain.
// Round 213: existing staged side/normal/both products 615 each; each with complete
// paired side offset 630, plus shared normal scales 630/630. Paired offset shrinks the
// frame to 0xa8 and length to 2584. Original 0xac-frame body restored; carriers/layout remain.
// Round 433: Independent transition endpoints/shared camera displacement/both 615 each;
// definition observed at every use alone/with either/with both 626 each, all plus source
// observations 634. All restored; native phase receivers do not recover the early carriers.
// Round 93 (kept, 579): the callback's buffer cast to a local vertex cursor, a pointer to the
// second vertex, per-component position stores, the next state's colour staged in its own
// array with the transition read once, the edge points built in each case from the point and
// case 0's normal set in its arms give retail's layout (endpoint block after the loop, second
// cursor at +0x28, definition in EBX). Left: frame 0x9c against 0xac (retail keeps a home for
// the first edge point and a width temporary), EDI holds the point pool where retail keeps the
// second cursor there and reloads the pool, the state-index clamp registers and the zero colour.
// Tried: buffer cursor alone 664; second-vertex pointer alone 666, with component stores 617;
// buffer + pointer + component stores 606, with u/v in vertex order 606, with flipped colour
// chains 610, with per-case points 634, with case-0 normal fields 606/632, definition first
// 606, cursor declared late 609, scale `1.0f` then if 605, transition local 629, staged colour
// 640; then per-case points 595 (frame 0xa4); with the case-0 normal arms 579 (kept); on that,
// scale if 593, object index local 596, flag bitfields 594, both 582, clamp if/else 579 (same
// code) and zero-default 592, zero stores in order 579, renamed locals 579 (no slot change).
// The buffer is untyped at the interface: 0x43740 queues this callback through 0x1e5e0 as
// the fill function 0x35b00 passes to 0x4b220, typed `void (__stdcall *)(void *, long, void *)`
// in unknown_01cf50.cpp.
// Round 443: Existing alpha/tint/pixel ordinary-inline producer boundaries, seven subsets and
// all with definition before source, 615 each. All original declarations, pixel qualifier and
// body restored; output/definition carriers, endpoint layout and color/vector homes remain.
// Lane 7 round 93's 579 form was not kept (coordinator review, round 94): it typed the first
// parameter `void *buffer` with a typed cursor local, a signature form with no call evidence
// (0x42cf0 is only passed as a (long) callback). With `s_ribbon_vertex *vertices` restored, the
// rest of that round's body (transition read once, next colour in its own array then blended,
// edge points set per case, field-wise stores through the second-vertex pointer) scored 622
// against 615 for this body (both --fast), so the body is back to its earlier form.
// Round 49: the round-93 body with the typed parameter and a local output cursor 579 (retail keeps
// the advancing pointer in its own frame slot and never touches the parameter's home); on it, each
// case reads `previous` directly and looks up the next point only in its no-previous arm 512 (kept).
// On 579: memset zero colour 612, with scale if 608; pool read through an address cast (diagnostic)
// 591; clamp `state <= max ? state : max` 579 (same code). On 512: case 0 dx/dy locals 522; memset
// 584, with scale if 584; object index local 566; memset + scale if + object index 577; length sums
// written i,j,k / i,j / dy,dx (VC reverses them into retail's fst-k-first x87 order) 573, with case 0
// dx/dy 593; memset + scale if + object index + those sums 582 (best aligned: retail's loop head and
// pool reloads; the score is positional); with the clamp as if/else on a state-initialised index 596;
// width declared first or renamed 582 (same code). Every `flags00 & 0x80` spelling gives cmp/js.
// Round 26 (kept, 284): endpoint fades through a `faded` local stored own-first 480 (chained own-first
// 578: the own store narrows to a byte store); on that, memset zero colour 592, plus scale `1.0f` then
// if 588, plus `state_width` with `width = state_width` 480 (after the colour 531), plus the state
// record indexed by `point->state` 461, plus an `object_index` local 458 (gives the `test byte, 0x80`),
// plus u/v stores in vertex order 458, plus the tint as a three-component loop 282 (retail length).
// Each part removed alone (on 282, with the pixel stored to vertex 0 first): 320/588/518/562/301/308/
// 290/458; the pixel order itself 284 (-2, not kept); u/v chained again on the kept 284 form 292.
// Other forms: interpolation inline for the colour/width blends 679 (on 512); clamp as an if/else chain
// 604 and with `<` 588 (on the 588 form), `<=`/PIN identical (on 461). On 282: case 0 points as struct
// copies 535, second point first 466, chained copy 535; dy assigned first in cases 1/2 279, plus the
// points declared `second, first` or after the normal 279; `real dy, dx` 282; length sums i,j / i,j,k
// side / i,j,k normal / dy,dx 282/400/282/282. On 284: second point assigned first in every case 479,
// with the stores second-first 479; u/v setup with v first 304.
// Lead: second point assigned first in cases 1/2 and 4 only 386 (frame 0xa8, retail's y spill and blend
// registers appear; edge point/half-width XMM permutation and the u/v slot order left); in all cases 479,
// with second-first stores 479; cases 0+1/2 567, 0+4 474, 4 365, 1/2 398. Renamed pixel locals or
// `v_width` do not move slots (284/386).
// Clamp as `if (state < 0) index = 0; else { index = state; if (index > max) index = max; }` 279 (the
// clamp is then retail's exactly; -5), with dy first in cases 1/2 276; zero-default clamp 579, ternary
// then if 584; the exact clamp with second point first in 1/2 and 4 381 (with dy first 381).
// Round 40 (case 0 edge points): first as `first = point->position; first.z += half_width;` and second likewise
// 535 (2668 bytes), second first 561; first as a struct copy only 468, second only 505; `half_width +
// point->position.z` 284 (same code). The one-file LTCG harness gives 290 here, so it was not used.
// Round 101 (lane 8; kept 276, fast and plain): round 26's listed form (the clamp as `if (state < 0)
// 0; else { state; if (> max) max; }`, retail's own clamp, with dy assigned before dx in both arms of
// cases 1/2), re-measured from the kept 284 and kept for its gain of 8. Exact LTCG harness (stand-in TU,
// through 0x42b20; 265 there for the full 284): the clamp alone 260, dy first alone 262, both 257. On
// 276: case 0's edge points y before x 274 (fast). Harness on that: case 0's normal.i before normal.j
// in the previous arm 250, in the neighbour arm 251, in both arms 407 (6 bytes shorter; aligned 171
// against 185: case 0 is then retail's but for retail's spill of a y copy at +0xb4); case 1/2 edge
// points y before x 347; `real dy, dx` 257, `dy * dy + dx * dx` 257, `dy *= inverse` first 258 (on
// 276's graph); on the both-arms graph case 0 edge points as struct copies then `z +=`/`-=` 455 (2668
// bytes), second.z first 549, both z last 407, `half_width + z` 407; second point first in case 0
// alone 450.
// Round 261: Complete geometry position via mutable/readonly native copy, mutable/readonly aggregate,
// native assignment, existing setter, memcpy, y-x-z stores: 615/615/602/602/615/591/615/602. Match-
// only forms all restored; frame and geometry/color homes remain.
// Round 66 (m3 lane 16, fast, kept form confirmed plain; 0x42c50/0x42b20 matched each time): the edge points named
// positive/negative, declared second-first, both 276 (same code); per-component pairs in cases 0, 1/2 and 4
// first-first 385, second-first 464 (2584 bytes); pairs in cases 1/2 and 4 only first-first 385, second-first 274
// (frame 0xa8, second in XMM0-2 as retail); on that case 0 second-first 464, per-component 274, y before x 274,
// `half_width + z` 274; case 1/2 y pair before x second-first 270 (first-first 272 at 2596, frame 0xa0), with dy
// scaled first 270, x before y with dy scaled first 274; case 0's first point z before x or y 265 (kept: case 0
// then loads z twice and adds the half width as retail), first.z last 270, second block first 465; on 265 case 4
// pairs y,x,z 269, z,y,x 271, x,z,y 356, z,x,y 264; case 1/2 dx/dy scaled by the half width in place 376 (2604);
// case 1/2 z pair first or between 564 (2588), first-first z pair last 266. Case 0's first set z, y, x and z, x, y
// give identical code (z, x, y kept). The difflib-aligned count rose 178 -> 200 with the kept form (case 4 and the
// vertex stores worse; cases 0-2 and the second point's XMM0-2 better), so the next round should start there.
// Round 280: complete per-channel interpolation results in initial/next/transition
// loops, all seven subsets, then all with the complete width interpolation result,
// 276 each. Original entire body and helpers restored; frame 0xa0, native homes
// and first-edge Y spill remain.
// Round 16: the argument registers agree (ECX is the source, EDX the result, the camera and the
// flags on the stack); retail keeps the vertex cursor in EDI and ours leaves it in its argument
// slot, and retail reaches the endpoint handling by a forward branch from the loop.
// Round 64 (lane 9, fast): case 0's edge points written component-interleaved 276 (same code); case 0's x
// and y as chained assignments (`first.x = second.x = ...`) 375 (2600 bytes); `0.5f * width` 276 (same
// code); the half width computed after the u/v stores 283; after the second-vertex pointer 276 (same code).
// Round 68 (m3 lane 16, fast; 0x42c50/0x42b20 MATCH each time): case 0 still gives the first point XMM5/XMM6/
// XMM4 and the half width XMM3 (retail XMM6/XMM5/XMM3 and XMM4; retail's first x/y are movaps copies of the
// second's). The edge points and normal declared before the half width 265; the second-vertex pointer before
// the half width 265; normal declared before the edge points 265; the edge points and the half width declared
// at the loop top 265 (identical code each); the second vertex's u/v stored through vertex[1] 278 (aligned
// 200 -> 212).
// Round 36 (lane 15, fast; 0x43740 62 and 0x42b20 matched in each): case 4's view/direction/side, the
// tint, or the next state's colour declared at the loop top 276 each (same code: slots follow lifetimes,
// not scopes); case 0's normal.i before normal.j in the previous arm 271, in both arms 426 (2592 bytes);
// that previous-arm order with case 0's edge points y before x 269 (-7, not kept; retail's case-0 x/y
// loads, but half_width stays in XMM6 where retail has XMM4, so the edge points keep our XMM homes);
// on that, half_width computed just before the switch 276, `second, first` declared 269 (same code), case
// 4's edge points y first 275, case 1/2's 373 (2604 bytes). Retail's tail colours second x/y/z XMM0-2,
// first.z XMM3, half_width XMM4, first.y XMM5 (spilled at +0xb4 in case 0), first.x XMM6.
// Round 280: complete per-channel interpolation results in initial/next/transition
// loops, all seven subsets, then all with the complete width interpolation result,
// 276 each. Original entire body and helpers restored; frame 0xa0, native homes
// and first-edge Y spill remain.
// Round 16: the argument registers agree (ECX is the source, EDX the result, the camera and the
// flags on the stack); retail keeps the vertex cursor in EDI and ours leaves it in its argument
// slot, and retail reaches the endpoint handling by a forward branch from the loop.
// Round 64 (lane 9, fast): case 0's edge points written component-interleaved 276 (same code); case 0's x
// and y as chained assignments (`first.x = second.x = ...`) 375 (2600 bytes); `0.5f * width` 276 (same
// code); the half width computed after the u/v stores 283; after the second-vertex pointer 276 (same code).
// Round 135 (m3 lane 5): local renames for the same-size slot order (vertex cursor, source, u/v, previous, half
// width) 265 each: the differing homes belong to spilled register variables, which names do not order.
// Round 512: Common geometry length/native transition color/both 265/265/265;
// common reciprocal alone/with length/with color/with both 399 each, and all with
// function-scope scalars 399. Original complete body restored; frame 0xa8/0xac
// and geometry/conversion homes remain.
// M298: Actual first/second output-position workspaces, direct/local, local/direct,
// direct/direct, pointer/local, local/pointer, pointer/pointer, direct/pointer, pointer/direct
// [623, 621, 641, 623, 621, 641, 641, 641]. All output-timing forms restored; temporary points retained.
// DH278: Complete vertical/planar/camera component-square subsets and separate camera roots 265/265/265/374/374/374/492/497.
// Complete opening body restored; no qualifying keeper.
// Round 466: Complete independent current-position scalars in vertical/planar/camera
// geometry subsets 1..7, then shared before dispatch: 570/576/570/568/559/626/538/582.
// Frame/spill homes remain; original coordinate observations and full body restored.
// Round 316: Paired planar/camera/both offsets 376/388/493; native position
// alone/with planar/camera/both and assigned native position 669/657/633/628/628.
// Geometry case orders 0-4-12/12-0-4/12-4-0/4-0-12/4-12-0
// 400/308/407/428/408. Original geometry, products, observations and case order restored.
// Round 518: half-width across tint on the entry-correct paired-point graph,
// original/staged side/staged normal/both 325/325/325/325; with vertical normal
// previous/neighbor/both order 321/334/406; both staged and both orders 406.
// Original body restored; frame 0xa8/0xac and point/color/conversion homes remain.
// Round 471: Emitted-pair capture, cursor step at lookup/color/tint/half-width/geometry/position/alpha/pixel,
// 635/619/635/581/558/606/556/444. Original cursor and body restored.
// M3 lane 4 r155 (fast, kept form plain 250): u_step/v_width as one vector2f (definition field typed vector2f) 254 (frame
// 0xac, v + width at +0xac as retail); as a struct copy of the field 642 (2620 bytes); the sum written at the outer v store
// (either operand order) 267; pixel-helper locals renamed twice 265 (same code). Case 0 second x, z, y then first (any of the
// six first orders) 250 (kept: retail's loads and schedule); second z, x, y 252, y, z, x 252, x, y, z 458, first block first
// 254-259. On 250: case 1/2 `dy *= inverse` first, `real dy, dx`, both, arms dx-first, `dy * dy + dx * dx` 250 each; x pair
// before the y pair 254; `second, first` declared, normal declared first, half width after the outer pointer 250 each.
// Then: case 1/2's dx/dy as one vector2f side (i the y difference, j the x difference) normalized by hand 230, through the
// existing normalize2d_fast 230 (kept; side named aa_/zz_ or declared with the points 230, same code); case 0's normal through
// the existing normalize3d 230 (kept: same code as the hand-written body); case 4's side and normal through normalize3d 473
// (2580 bytes). On 230: side arms j first 228, x pair first 234, both 232, first.z before second.z 231.
// Coordinator: not kept. The vector2f local exists to shape the frame (its i is dead, its j becomes v + width; a locals-
// grouping form), and case 0's order was picked from 36 by score. The two existing normalize helpers alone (normalize2d_fast
// on a side vector, normalize3d on case 0's normal) give 260 fast: a gain of 5, under the bar. Body restored.
// F336: Complete opacity clamp lower-else/upper-else/lower-upper/upper-lower,
// field then actual opacity value storage: 265/269/265/269/276/278/276/278.
// Original product, its intermediate, signed zero, record publication and complete body restored.
// Round 540: Twice-used per-channel lower/current input values, initial/next/transition
// phase subsets 1/2/4/3/5/6/7, 553/550/265/588/553/550/588; all with width owning
// its transition input 582. Next-phase forms recover frame 0xac but grow the body;
// NOT kept: no gain or match, and changed load ownership. Original complete body restored.
// F336: Complete opacity clamp lower-else/upper-else/lower-upper/upper-lower,
// field then actual opacity value storage: 265/269/265/269/276/278/276/278.
// Original product, its intermediate, signed zero, record publication and complete body restored.
// Round 545: Native color/width default producers, aggregate/library color seeds,
// separate/both defaults and positive state arms 336/577/561/615/336/578/558/616. All original
// output producers and body restored; frame 0xa8/0xac and spill homes remain.
// Round 139 (m3 lane 5, fast): with the 4-byte frame shift u_step and the first.y spill already sit in retail's slots;
// retail's v + width lies at +0xac over a dead dword. v_width read before v 266; `v_width = v + v_width` 265; u_step
// before u 265; width scale in its own local 265; `width` declared before `color` 265; `previous` before `index` 265.
// Round 548: Complete double length residence in vertical/planar/camera-pair subsets
// 1/2/4/3/5/6/7, then all with double reciprocals: 433/544/605/556/635/515/632/633.
// NOT kept: precision/normalization shape misses; original float roots, reciprocals and full body restored.
// Round 38 (m3 lane 18, fast): v_width renamed to sort first/last 265/265 (identical code); the colour stored
// `outer->color = vertex[0].color = ...` 263 (retail's store order; a gain of 2, not kept); on it: source and u
// renamed 263, transition declared at the top 263, a named pixel stored to vertex 0 then outer 263, a named strip
// start for the tail 615 (2600 bytes), the next state's colour as a color4f 263. Retail leaves 0xa8/0xb0/0xb8
// (after the pushes) unused and spills point y at 0xb4: its top homes sit eight bytes apart.
// Round 552: branch-produced V endpoint; separate signed U producer with original/separate/branch
// V; branch-produced U with original/separate/branch V; conditional signed U with branch V:
// 615/265/265/615/286/286/632/632. Original texture values, geometry, helper and full body restored.
// NOT kept: no score gain or byte match.
// m3 lane 16 r83 (fast): case 4's side and normal lengths spelled i, j, k give retail's x87 order there (fst k,
// fmul k, then j, i) but 374 (2588 bytes; as DH278's camera squares): ours is already one instruction short in
// case 1/2, where retail keeps the x product live and loads point x into its own register (movss xmm6, [ecx];
// addss xmm6, xmm5), so everything after case 4 shifts. Side only 374, both 374; with case 1/2's first.x before
// second.x 374 (VC copies the product instead); with normalize2d_fast on a case 1/2 side vector and normalize3d
// on case 0's normal 368; that with the x pair before the y pair 372. All restored.
// Y574: Current paired-point callback completion false/true at entry/chain boundary and default-
// case first/last: 609/609/609/609/609/609/634/634. NOT kept: all regress; false entry completion
// recovers frame 0xac but grows to 2608 bytes and changes homes; original 265 body restored.
// Original complete body and helpers restored.
// DH346: On retained paired geometry, staged side/normal/both crosses, shared chain difference,
// immutable source/definition/outer/all pointer bindings: 265/265/265/266/265/265/265/265.
// NOT kept: no native frame/spill recovery; original complete body and helpers restored.
// Y576: One shared current/next/transition channel ordinal, long/short/word/byte
// 265/754/754/754; also owning RGB tint 265/756/756/756. NOT kept: long forms
// canonicalize; narrow forms disrupt unrolling. Original frame 0xa8 and scalar/spill homes remain.
// Y578: Actual V interval in existing bounds, field/aggregate/native/library publication
// at producer/entry storage: 267/267/573/573/267/651/573/573. NOT kept: interval
// residence misses equality; native-frame copy forms grow. Original separate scalars and body restored.
// Y582: On current paired-point geometry, original/shared normalizer pairs for camera side
// and surface normal, all eight nonoriginal pairs: 270/270/377/377/377/377/377/377.
// NOT kept: no byte match; original roots/scales, shared crosses and full body restored.
// AC547: Next-color difference/scaled-difference stages 304/526; upper/lower library
// seeds 563/626; seeded complete stages 579/627; transition difference/scaled-difference
// receiver stages 535/571. NOT kept: all regress; original complete body restored.
// Y585: Current paired geometry, initial/next/blend output-stream cursor subsets
// 1/2/4/3/5/6/7: 265/265/265/268/268/352/350; all with tint cursor 265.
// NOT kept: no native frame recovery or eligible gain; original counters and complete body restored.
// Y587: Float root API at vertical/planar/camera-side/camera-normal/vertical-planar/
// camera-pair/all/all with float epsilon consumers: eight 265s. NOT kept:
// API boundaries canonicalize; native frame/spill homes remain; original body restored.
// M3 lane 4 r167 (kept 250 fast, one-file link with the ballast object 251, ranked by aligned count and shape): both arms of case 0's
// normal setting i before j 255, plus case 1/2's dx/dy as a vector2f through the existing normalize2d_fast 250 (both kept). Not kept:
// side length i, j, k alone 374 (2588 B: retail's k, j, i x87 order and fst, shape better); that on round 155's scale vector and case 0
// order 349 (2588 B, everything up to the switch exact; aligned 108, shape 30); those without the side length 218 (2592 B); named
// offsets i and j in case 1/2 476-495 (2584 B), named offsets in case 4 428, offset vector in case 4 459 (2580 B); first.z, first.y,
// first.x weight probes (extra reads after the switch, up to 3/2/1): aligned 104-118, no colouring flip. Left: retail colours first.z
// XMM3 and the half width XMM4, first.y XMM5 and first.x XMM6 (ours half width XMM3, first.z XMM4, first.x XMM5, first.y XMM6).
// M3 lane 4 r170 (fast in the tree; one-file harness with the ballast object for the searches, 251 for the real 250): the colour
// stored to the outer vertex first 248; case 0's points set second x, z, y then first x, y, z 246; both 244 (order picked by score,
// under the bar); case 4's side scaled in place by the existing vector3f_scale then added and subtracted 423 (frame 0xa4), with case
// 1/2's side scaled in place too 435 (2608 B). Harness lead: retail's colouring of case 0's tail (half width XMM4, first.z XMM3,
// first.y XMM5, first.x XMM6) appears as soon as case 4 stops writing `side.i * half_width` twice per axis (named offsets, in-place
// scaling, an offset vector); then case 0 differs only by retail's first.y spill and two movaps, but the frame drops to 0xa4 and the
// positional score rises to 410-435 (masked aligned 30 -> 47). About 1,500 harness forms over case 0/1-2/4 statement orders, offset
// forms, length orders and case 0's normal block: best positional 245, best aligned 90; no frame 0xac without the rejected u/v vector.
// T64: Texture-word/current-state-dword/state-array input owners, singles/pairs/all and
// all with readonly state/endpoints: eight 250s. NOT kept: native flag/receiver observations
// canonicalize without the extra frame slot or first-edge Y/half-width homes; full body restored.
// T70: Real backing flag views on current paired geometry, texture/state/point/pairs/all/endpoint
// 629/591/250/676/626/591/665/676. NOT kept: no exact native ingress or missing frame-word repair;
// original flag words, observations, geometry, helpers and complete body restored.
// P365: Existing norm-dimension/squared-length producers at vertical/side/normal sites,
// masks 1/2/4/3/5/6/7: 250/368/250/368/250/368/368; planar normalizer retaining
// third-component scaling 485 (frame 0xac, 2628 bytes). NOT kept: no equality; the camera
// side sum changes association. Unscaled-third-component diagnostic 426 excluded.
// Original roots, scaling, geometry, helpers and complete body restored.
// P367: Next-iteration U after UV/geometry/position/opacity consumers
// 606/306/304/274; each with transition width after its color loop
// 564/418/415/386. NOT kept: no native frame/scalar equality; statement-
// lifetime forms regress. Original texture/width sequence and body restored.
// P371: Complete complementary normal request in bool/char/signed-char/byte/short/word/long/dword:
// 567/567/567/567/592/592/596/596, 2576/2596 bytes. NOT kept: regressions and
// nonnative frame/spill allocation; original per-case observations and complete body restored.
// P373: Ordinary pretested channel counters for initial/next/blend color walks, seven subsets,
// all with the tint walk: eight 250s, 2592/2596 bytes. NOT kept: frame 0xa8/0xac
// and scalar homes unchanged; original color traversals and complete body restored.
// T72: Complete computed first/second/both edge scalars, then existing output setters
// for first/second/both scalar graphs: 252/250/252/252/250/252/252/252. NOT kept:
// no native frame or edge-spill recovery; original complete body and helpers restored.
// P389: Complete first/second edge publications through native/standard copy/backward
// range transfers, eight nonoriginal pairs:
// 598/598/733/750/745/769/735/729. NOT kept: no exact native frame/home recovery;
// original complete body, observations, helper and call graph restored.
// M3 lane 8 r144 (fast; control 250, frame 0xa8): retail's unused 4-byte home is at +0xa8, just below v + width (+0xac).
// The V sum formed at the outer vertex's store 252; the U step negated at the advance (`u += -u_step`) 611 (2596 bytes, frame
// 0xa8); both 610; both with the sum's operands swapped 610; a named V end declared after the width scaling 250 (same code); u
// and v read after the two scales 271; one read of the source scale for both tests 608; the colour stored to the first vertex
// first 248. Re-measures of the rejected grouping (a two-member texture scale vector; not kept, the same locals-grouping form
// the coordinator threw out for r155): i negated in place with a named V end 239 (frame 0xac); a named U step from i and the V
// end kept in j 224 (frame 0xac, confirmed in the plain check; the diff is build/r144/lead-42cf0.patch); both in place 250
// (frame 0xa8). On the 224 form: the colour store order 222; case 4's offsets named in all three rows 459 (2580 bytes, frame
// 0xa4); case 1/2's offsets named in both rows 490 (2588); both 443 (2596 bytes, frame 0xa4). Body restored.
// P399: Actual current/next color-bound identities and complete next-color record, subsets 1-7, then output-color identity:
// 250/250/482/250/250/250/490/447. NOT kept: no byte equality or native home repair; original complete body restored.
// Round 85 (m3 lane 10, fast; kept form plain 250; 0x42c50, 0x42b20 and 0x43740 stay matched): the `(void)&unknown;` statement removed 250 (same code, kept: it did
// nothing). On that: the pixel helper's scale declared between its green and red channels 250 (same code: declaration order does not move the asm locals' homes);
// `index = point->next` before `previous = point` 561 (2596 bytes as retail, but register-masked aligned 30 -> 39); `u += u_step` before `vertex += 2` 251; v_width
// read before v 251; the vertex cursor declared after source and definition 252. Frame map (after the pushes): retail has three 8-byte homes at the top, each with
// its upper dword unused: the first.y spill +0xb4, v + width +0xac and the u step +0xa4, then the helper's channels; ours has two (the first.y spill +0xb0 and the
// helper's blue channel +0xa8) and keeps v + width and the u step in 4-byte homes among the channels. Register-masked aligned differences left: 30, all small
// schedules (case 0's half-width add and y spill, case 1/2's x load, case 4's position pointer formed after the neighbour lookup, the side length's `fst`,
// the colour store order and one tail reload).
// Round 66 (m3 lane 2, fast; 0x42c50, 0x42b20 and 0x43740 stay matched): retail's top homes are 8-byte spill slots (as our
// first.y spill), so its u step and v + width are register temporaries there, not named locals with 4-byte homes as ours.
// Retail inlines the matched pack_color4f (0x131e50) at link time where ours inlines the same-file ribbon_pixel copy; the
// copy as plain `inline` 250 and with no inline keyword 250 (both same code: the compiler still inlines it). `u -= u_step`
// with no negation 611 (2596 bytes); the tint as three statements 447 (2580 bytes); the object index read from the source
// at each use 276. Re-measure (diagnostic): `u += -u_step` with the V sum at the outer store 610 (u_step stays a 4-byte home).
// Round 68 (m3 lane 2; fast, kept form confirmed plain and in a full check; 0x42c50, 0x42b20, 0x43740 matched): case 4's normal
// through the existing normalize3d 250 -> 233 (kept: its length is then summed i, j, k, retail's x87 order), case 0's normal through
// it too 233 (identical code, kept: it removes the hand-written copy). Not kept: case 4's side through normalize3d as well (r155's 473
// re-measured on this base) 351 at 2588 bytes, aligned 145, masked 45: the side's x87 is then retail's, but the body is 4 bytes short
// because case 1/2 lacks retail's second x load (a colouring symptom), so everything after case 4 shifts; start the next round there.
// The side alone through it 351; both lengths written i, j, k 362 (diagnostic); case 4's edge points through vector3f_add_scaled 363;
// view and direction through vector3d_from_points3d 443 (2584 bytes). On 233: side scaled in place by vector3f_scale then +/- 416;
// case 1/2's points through real_point3d_set 443 (2580); the cursor as `vertex = outer + 1` 656 (2556); the vertex positions through
// real_point3d_set 233 (same code); case 1/2's normal copy before its points 241; case 1/2's z as `first.z = second.z = ...` 331
// (2600); case 0's normal set through vector3f_assign in each arm 499 (2620); the side's scale through vector3f_scale 233 (same code).
// Round 168 (m3 lane 6, fast; 0x42c50, 0x42b20 and 0x43740 matched each time; control 233, aligned 149, register-
// masked 73): re-measured on this base `u += -u_step` 611 (2596 bytes: the negation moves into the loop, it is not
// hoisted), with the V sum at the outer store 610, the V sum alone 235. New: the negated step as a single-assignment
// local 233 (same code); with a single-assignment V end as well 233 (same code); case 4's direction through the
// existing vector3d_from_points3d in both arms 483 (2584 bytes); case 4's view through it 233 (aligned 164, masked 88:
// the else arm's view moves after the neighbour lookup and the cross operands swap; not kept); a named pointer for
// every use of the point's position 612 (2620 bytes). Body restored.
// R151 (m3 lane 7, fast; 0x42c50, 0x42b20, 0x43740 matched): the pixel through the existing pack_color4f 598 (2448 bytes:
// its declaration is noinline, so it stays a call where retail expands it at link time). The frame word needs the u step and
// the V end as compiler temporaries (retail's 8-byte homes), which only the ruled-out two-member grouping gave; case 1/2's
// early point x load and case 4's per-arm position pointer are schedules of forms listed above. Case 4's direction alone
// through the existing vector3d_from_points3d in both arms 483 (2584 bytes), the view alone through it 233 (same code);
// case 1/2's first.x before second.x 233 (same code); both case 1/2 pairs and z first-row first 394 (2600 bytes).
// T86: Descending initial/next/blend/tint channel indices 509/334/568/244;
// descending channel pointers 697/604/623/622. NOT kept: no byte match or
// frame/home recovery; original traversal, arithmetic and complete body restored.
// MiniMax R (11-11): no new form, no lower score (233): the frame is four bytes shorter (0xa8 against retail's
// 0xac), so every stack home below the top is off by four; retail keeps the u step and v + width in eight-byte
// spill slots at the top of the frame, ours keeps them in four-byte homes among the channel homes, and the
// first-edge Y, half-width, cursor and colour homes differ with them.
// T95: Complete initial lower/upper and next lower/upper color-bound records,
// each alone, initial pair, next pair, all and all by library copy: 618/599/596/544/695/666/867/867.
// NOT kept: input-record copies miss equality and regress; original input reads,
// frame 0xa8/0xac, geometry, conversion helper and complete body restored.
// m3 lane 14 r88 (fast with 0x42c50, 0x42b20, 0x43740, all MATCH; masked = aligned count with registers and stack offsets
// masked, base aligned 149, masked 46): re-measured on the 233 base, the colour stored to the first vertex first (`outer->color
// = vertex[0].color = ...`) 231 (a store order by score, not kept); the zero colour before the zero width 233 (same code); u and
// v as one point2f texture coordinate (the source fields typed point2f) 582 (2596 B); case 4's else arm with the view computed
// after the neighbour lookup 328 at retail's 2596 bytes (aligned 149 -> 134, masked 46 -> 35: case 4's per-arm position
// pointer is then retail's; a statement order, match-only), that with the colour order 329 (masked 33); case 4's direction
// ahead of the view in both arms 356 (2608 B); case 1/2's z row first 566 (2588 B). Left on the 328 form (masked 35): the
// frame (retail's u step and V end in 8-byte temporary homes), case 0's load order, case 1/2's early x load, the side
// length's fst, the colour store order and one tail reload. Body unchanged.
// P436: Fixed existing point/current-state/next-state receivers, transition weight,
// V origin, half-width, tint-object handle and side reciprocal: eight fast 233s,
// control 233. NOT kept: frame 0xa8 versus 0xac and native scalar/spill homes
// remain with no byte change; original complete tuning body restored.
// P436: Existing object-tag attachments.address replaces the raw +0x98 read;
// fast and ordinary checks both preserve 233 and 2592/2596 bytes.
// M3 lane 11 r96 (fast with 0x42c50, 0x42b20, 0x43740, all MATCH; control 233, aligned 149). Frame map: retail has 37 dword
// homes and three 8-byte homes at the top (u step +0xa4, V end +0xac, first.y spill +0xb4); ours has 40 dword homes (the u step,
// the V end and one more) and one 8-byte home (the first.y spill, a promoted aggregate field). The loop and tail in a forceinline
// helper taking the negated step and the V end as argument expressions 527 (2588 bytes, frame still 0xa8: argument temporaries
// take dword homes too); the three scalars renamed (V end, U advance, V start, and two names at the ends of the alphabet) 233
// each (same code: names do not move these spill homes); `point3f second, first;` 233 (same code); case 0 with the second edge
// assigned first 458 (2584 bytes), that with the declarations swapped 458; case 0's first edge copied from the second's x and y
// 439 (2584 bytes); case 0 as two aggregate copies of the position with the z offsets applied after, second first or first
// first 698 / 698 (2680 bytes). Retail loads x and y once into XMM0/XMM1 for the second edge and copies them to XMM6/XMM5 for
// the first; ours loads them for the first and copies down.
// P441: Current/next/both/complete width-product branches, original then output-owned initial width: 510/511/545/558/576/572/584/590.
// NOT kept: Native 0xac frame, first-edge Y and texture/color spill homes remain. Original complete body restored.
// P463: Paired U/V scale-admission positive bool/byte/short/long 627/627/238/238;
// complementary outcomes 634/634/634/634 reduced (control/full 233).
// NOT kept: one-use results lack equality and native frame/spill homes;
// original texture producers, observations and complete body restored.
// T103: Actual output UV point record direct/first/outer/both setters, then original
// fields with first/outer/both setters and native UV value publication:
// 233/241/241/240/241/241/240/410. NOT kept: no native frame/spill equality;
// original vertex fields, UV stores, helper declarations and complete body restored.
// P469: Independent double U/U-step/V/V-endpoint/color-scale/current-width/next-width/transition
// storage 422/631/575/601/670/583/363/604. NOT kept: extended scalar precision misses
// native code and extent; original real producers, geometry and complete body restored.
// T105: Read-only paired color bounds/persistent records/point chain,
// seven subsets then complete current/next state inputs: eight at 233.
// NOT kept: frame/spill homes unchanged; original complete body restored.
// @retail 0x42cf0
bool __stdcall function_42cf0(s_ribbon_vertex *vertices, long unknown, s_ribbon_input *input)
{
    s_ribbon_vertex *vertex = vertices;
    s_ribbon_source *source = input->source;
    s_ribbon_definition *definition = input->definition;
    real u = source->u;
    real u_step = definition->u_step;
    if (definition->flags02 & 0x40)
        u_step *= source->scale;
    u_step = -u_step;
    real v = source->v;
    real v_width = definition->v_width;
    if (definition->flags02 & 0x80)
        v_width *= source->scale;
    v_width += v;
    long index = source->point_index[input->slot];
    s_ribbon_point *previous = NULL;
    while (index != NONE)
    {
        s_ribbon_point *point = (s_ribbon_point *)(g_4ea940->data + (index & 0xffff) * 0x38);
        long state_index;
        if (point->state < 0)
            state_index = 0;
        else
        {
            state_index = point->state;
            if (state_index > definition->state_count - 1)
                state_index = definition->state_count - 1;
        }
        color4f color;
        real width;
        if (state_index != point->state)
        {
            width = 0.0f;
            memset(&color, 0, sizeof(color));
        }
        else
        {
            s_ribbon_state *state = &definition->states[point->state];
            real scale = 1.0f;
            if (state->flags & 0x20)
                scale = point->scale;
            real state_width = state->width;
            if (state->flags & 0x10)
                state_width *= point->scale;
            width = state_width;
            real *lower = (real *)&state->lower;
            real *upper = (real *)&state->upper;
            real *out = (real *)&color;
            for (long i = 0; i < 4; ++i)
                out[i] = (upper[i] - lower[i]) * scale + lower[i];
            if (point->flags & 2)
            {
                s_ribbon_state *next = &definition->states[point->state + 1];
                real transition = point->transition;
                real next_width = next->width;
                if (next->flags & 0x10)
                    next_width *= point->scale;
                real next_color[4];
                lower = (real *)&next->lower;
                upper = (real *)&next->upper;
                for (long j = 0; j < 4; ++j)
                    next_color[j] = (upper[j] - lower[j]) * scale + lower[j];
                width = (next_width - state_width) * transition + state_width;
                for (long k = 0; k < 4; ++k)
                    out[k] += (next_color[k] - out[k]) * transition;
            }
        }
        long object_index = source->object_index;
        if (object_index != NONE && !(definition->flags00 & 0x80))
        {
            point3f tint = *g_468710;
            byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
            s_object_definition *object_tag = (s_object_definition *)g_4e3b44[(*(long *)object) & 0xffff].bytes;
            byte *markers = (byte *)object_tag->attachments.address;
            function_bad50(object_index, *(short *)(markers + source->marker_index * 24 + 12), &tint);
            for (long c = 0; c < 3; ++c)
                (&color.red)[c] *= (&tint.x)[c];
        }
        real half_width = width * 0.5f;
        s_ribbon_vertex *outer = &vertex[1];
        vertex[0].u = u;
        vertex[0].v = v;
        outer->u = u;
        outer->v = v_width;
        point3f first, second;
        vector3f normal;
        switch (definition->mode)
        {
        case 0:
            first.z = point->position.z + half_width; first.x = point->position.x; first.y = point->position.y;
            second.x = point->position.x; second.y = point->position.y; second.z = point->position.z - half_width;
            if (input->normal)
            {
                if (previous)
                {
                    normal.i = previous->position.y - point->position.y;
                    normal.j = point->position.x - previous->position.x;
                }
                else
                {
                    s_ribbon_point *neighbor = (s_ribbon_point *)(g_4ea940->data + (point->next & 0xffff) * 0x38);
                    normal.i = point->position.y - neighbor->position.y;
                    normal.j = neighbor->position.x - point->position.x;
                }
                normal.k = 0.0f;
                normalize3d(&normal);
            }
            break;
        case 1:
        case 2:
            {
                vector2f side;
                if (previous)
                {
                    side.i = previous->position.y - point->position.y;
                    side.j = point->position.x - previous->position.x;
                }
                else
                {
                    s_ribbon_point *neighbor = (s_ribbon_point *)(g_4ea940->data + (point->next & 0xffff) * 0x38);
                    side.i = point->position.y - neighbor->position.y;
                    side.j = neighbor->position.x - point->position.x;
                }
                normalize2d_fast(&side);
                second.y = point->position.y - side.j * half_width; first.y = point->position.y + side.j * half_width;
                second.x = point->position.x - side.i * half_width; first.x = point->position.x + side.i * half_width;
                second.z = point->position.z; first.z = point->position.z;
                if (input->normal) normal = *g_4687b0;
            }
            break;
        case 4:
            {
                vector3f view, direction, side;
                if (previous)
                {
                    view.i = g_4b9da0.x - previous->position.x; view.j = g_4b9da0.y - previous->position.y; view.k = g_4b9da0.z - previous->position.z;
                    direction.i = point->position.x - previous->position.x;
                    direction.j = point->position.y - previous->position.y;
                    direction.k = point->position.z - previous->position.z;
                }
                else
                {
                    view.i = g_4b9da0.x - point->position.x; view.j = g_4b9da0.y - point->position.y; view.k = g_4b9da0.z - point->position.z;
                    s_ribbon_point *neighbor = (s_ribbon_point *)(g_4ea940->data + (point->next & 0xffff) * 0x38);
                    direction.i = neighbor->position.x - point->position.x;
                    direction.j = neighbor->position.y - point->position.y;
                    direction.k = neighbor->position.z - point->position.z;
                }
                cross3f(&view, &direction, &side);
                real length = (real)sqrt(side.k * side.k + side.j * side.j + side.i * side.i);
                if (!(fabs(length) < 0.0001f))
                {
                    real inverse = 1.0f / length;
                    side.i *= inverse; side.j *= inverse; side.k *= inverse;
                }
                second.x = point->position.x - side.i * half_width; first.x = point->position.x + side.i * half_width;
                second.y = point->position.y - side.j * half_width; first.y = point->position.y + side.j * half_width;
                second.z = point->position.z - side.k * half_width; first.z = point->position.z + side.k * half_width;
                if (input->normal)
                {
                    cross3f(&direction, &side, &normal);
                    normalize3d(&normal);
                }
            }
            break;
        default:
            return false;
        }
        vertex[0].position.x = first.x;
        vertex[0].position.y = first.y;
        vertex[0].position.z = first.z;
        outer->position.x = second.x;
        outer->position.y = second.y;
        outer->position.z = second.z;
        color.alpha *= function_42c50(&point->position, &normal, *(short *)(input->shader + 0x2c), (byte *)definition);
        color.alpha = PIN(color.alpha, 0.0f, 1.0f);
        vertex[0].color = outer->color = ribbon_pixel(&color);
        vertex += 2;
        u += u_step;
        previous = point;
        index = point->next;
    }
    vertex -= input->vertex_count;
    if (!(definition->flags00 & 1) && source->point_count[input->slot] > 2)
    {
        dword faded = vertex[0].color & 0xffffff;
        vertex[0].color = faded;
        vertex[1].color = faded;
    }
    if (!(definition->flags00 & 2))
    {
        dword faded = vertex[input->vertex_count - 1].color & 0xffffff;
        vertex[input->vertex_count - 1].color = faded;
        vertex[input->vertex_count - 2].color = faded;
    }
    return true;
}

long function_137590(long group_index, short frame_index, short sequence_index);
struct D3DTexture;
struct s_bitmap_view;
D3DTexture *function_12360(s_bitmap_view *bitmap, real priority);
void function_52d40(long bitmap_index, byte *shader, long tag, dword flags);
bool function_1e5e0(long size, long a, long b, long c,
    long d, long e, long f, long g, void const *data, point3f const *position);

struct s_43740_input
{
    s_ribbon_input vertices;
    long tag;
    long bitmap;
};

// @retail 0x43720
void __stdcall function_43720(s_43740_input *input)
{
    function_52d40(input->bitmap, input->vertices.shader, input->tag, false);
}

/* queues one slot of a ribbon for sorted drawing, once its bitmap's texture is available */
// score 62 (was 62): retail reads the object's field before the frame is set up
// and compares it with -1, ours loads it into ebp and compares ebp. Retail
// rereads the tag for input.tag (now matched), keeps the slot index in ebp after
// the texture test and tests the normal flag as shader + 0x2c. Tried: early
// return 63; the test reading the field and the tag read inside 62; the body as
// an inline helper taking the tag 62; the normal flag read through the shader
// pointer 66 (definition moves to the stack); a vertex count local first 64 (it
// swaps source and definition registers and 0x42b20 stops matching).
// Round 197 fast trials: Populated request definition readback, direct/stored source and slot x
// definition/shader normal x retained/removed slot anchor 62, 62, 67, 67, 62, 62, 67, 67.
// Round 121: the slot anchor `(void)&slot;` dropped (identical code, 0x42b20 still matches); on that:
// the tag as a dword 62; a slot index local for both arrays 62; the vertex count stored first 63; the
// parameters as (definition, source, slot) at the definition and the call 62 (identical code).
// Round 98: retail tests the tag in eax before `push ebp` and copies it to ebp after (ours keeps it
// in ebp from the load, so ebp is saved before the test). The test on the field with the tag read
// inside 62; the normal flag through the stored shader pointer or a shader pointer local 67 (the
// definition moves to the stack and 0x42b20 stops matching); the input record or the bitmap
// declared at the top 62/62; the record filled count, shader, normal, slot, tag, then source,
// definition, bitmap 63; the group lookup re-reading the tag field 62 (CSE gives the same code).
// Round 213: complete mutable request/zero aggregate/library zero/readonly request
// 91/100/105/91; direct-position counterparts 91/100/105/91. Original body restored;
// tag admission before saved EBP and signed-slot/shader-relative homes remain.
// Round 12: the tag field read at each use (test, call, group lookup) 62 and with a tag local
// inside 62 (the reads merge into one value); the normal flag read through the stored
// input.vertices.shader 67 (213 bytes, the definition moves to the stack, 0x42b20 62); the
// position computed before the input fill 62 (retail computes it before the pushes; ours
// still interleaves it); a named vertex count used for the field and the call 64, with the
// position first 64 (0x42b20 4).
// Round 443: Ordinary-inline declarations at lookup/texture/submission boundaries, seven
// subsets 62 each; all plus shader-relative normal 67 and matched caller 62. All declarations
// and original body restored; early tag/EBP and signed-slot homes remain.
// Round 49: normal flag through the stored shader member with (definition, source, slot) at the
// definition and call 67; tag field read at each use 62 (identical); shader pointer local 67, with
// the lookup rereading the field 67, with every tag use read from the field 67 (each still folds the
// read to [definition + 0x68] but makes the definition a stack argument held in EBP).
// Round 26: retail's EAX test / EBP copy head needs the tag's test read and the body's reads to be
// separate values: field test with the group lookup (field) before the index call (field) 68, or a
// tag local read before the call for the lookup only 68 (both split like retail but the tag outranks
// a parameter: ret 8, 0x42b20 60); either with the record's tag from the local 40 (head through the
// index call identical; the tag then lives across the texture call where retail re-reads the field,
// and it mixes local and field reads of one value); 36-form grid of those splits x normal read x slot
// local x fill order 68-70; early-return guard 68; recomputed count argument 68; tag local used
// everywhere 63, masked in place 62. The normal read through the stored shader (member, local,
// accessor inline) still folds to +0x68 under LTCG (compile-only harness gives [ecx+0x2c]) 67/70.
// Round 18 (m3 lane 2, fast; 0x42b20 and 0x43720 matched in each): the position through the sibling's
// `&((s_ribbon_point *)...)->position` 62; the bitmap address written `bitmap * 0x74 + base` 62; `long const tag`
// 62; the group lookup before the index call 62 (LTCG still loads it after the call); the record filled in retail's
// store order (shader, slot, normal, tag, source, definition, count, bitmap) 62.
// Round 40: record filled slot, shader, normal, count, tag, bitmap, then source and definition 64; with the normal
// read through the stored shader 67 (213 bytes, 0x42b20 62); filled count, shader, normal, slot, tag, source,
// definition, bitmap 63; arrays indexed through the stored slot 62 (same code); texture test as an early return 62.
// Round 101 (lane 8; exact LTCG harness through 0x42b20 with the generated stand-in TU, 58 = 62):
// beyond the head, retail keeps the slot index in EBP after the texture test, reads the normal flag as
// shader + 0x2c and computes the position before the pushes. The position computed before the record
// is declared 62; input.tag read through the definition parameter 62; the vertex count as `2 * count`
// 62; `NONE != tag` 62 (identical code each time; 0x42b20 still matches).
// Round 12 (lane 17, fast): the normal flag read through the stored input.vertices.shader 67 (213 bytes:
// still folded to +0x68, the definition moves to the stack, 0x42b20 62); the slot taken once into a long
// local before the texture call for both arrays 69 (215 bytes, 0x42b20 66); the position computed after
// the normal and before the tag, bitmap and count 62; that with the count right after the slot 63; the
// mid-fill position with both arrays indexed through the stored slot 62 (0x42b20 matched in the last three).
// Round 34 (lane 13, LTCG harness through 0x42b20, base 58 aligned 32 = 62; 0x42b20 0 in each): the
// definition given a typed sub-record at +0x3c with the normal flag at its +0x2c, shader and normal read
// through it 62; with the tag as a typed field 62; the typed tag alone 62; a second index local copied
// from the tag for the call and lookup 62; an early return on the field with the tag local after it 62;
// the tag as a dword 62 (all identical code); the lookup through `(word)tag` harness 60, aligned 53 (204 bytes).
// Round 188 (fast, 0x42b20 matched in each): the parameters as (slot, source, definition), (slot,
// definition, source), (source, slot, definition) and (definition, slot, source) at the definition
// and the call 62 each (identical code); the tag test on the field with a tag local for the call and
// the lookup (lookup first) in all six parameter orders 62 each.
// Round 42 (lane 11, fast): the field test with the tag local inside the block and the normal flag read
// back through the stored `input.vertices.shader + 0x2c` 67 (213 bytes; it still folds to the definition's
// +0x68, the definition becomes a stack argument in EBP, the tag takes EBX and 0x42b20 drops to 62); that
// with the record filled in retail's store order 67 (0x42b20 62); the field test with the tag local
// inside and the normal read from the definition 62, plus retail's fill order 62, plus the record declared
// at the top 62 (0x42b20 matched in each).
// Round 124 (fast, 0x42b20 checked each time): the image pointer written into the texture call 62; the
// group pointer written into the image expression 62; the call's count argument recomputed from the
// point count 62; the bitmap stored before the tag 62 (identical code each time); the slot stored
// last 64 (211 bytes, and 0x42b20 stops matching).
// Round 36 (lane 15, fast; 0x42b20 checked each time): the field test with a block-local tag used for
// the lookup (placed before the index call), the call and the record 63 (206 bytes, no split); the
// function-scope tag with the lookup first and the record's tag from it 63; the field read at each use
// with the lookup first (listed 68: the head splits like retail's, but the tag takes EBX, the source
// becomes a stack argument in EBP, ret 8, 0x42b20 60) with the record filled in retail's store order 68,
// with the position computed before the fill 68 (0x42b20 60 both); the texture test as an early return
// 62 (same code). The lookup-first field form is the only one that splits the head here; what keeps the
// split tag below both parameters in retail's colouring is open.
// Round 66 (m3 lane 16, fast; 0x42b20 checked each time): the tag assigned inside the test 62 (same
// code); the count named first with the record filled in retail's store order (shader, normal, slot,
// tag, position, then source, definition, count, bitmap) 64 (0x42b20 4); a long slot index with the
// named count for the field and the call 64 (0x42b20 4); the call's count recomputed 62 (again);
// the position computed before the record is declared 62. No matched retail function has this
// head's one-value EAX test / EBP copy.
// Round 31 (m3 lane 2, fast; 0x42b20 checked each time): on the split form (tag field tested, the
// index call on the field, a tag local for the group lookup and the record: 40, retail's head through
// the index call, a mix of field and local reads kept only with a match): the normal flag through the
// stored shader 67 (212 bytes, 0x42b20 62); the record filled in retail's store order (shader, slot,
// normal, tag, source, definition, count, bitmap) 42; a `long index = slot` for both arrays 40; the
// position computed before the record 40; the count stored first 41. The tag local for the lookup
// only 68 (the tag outranks the source: definition ESI, source stacked; 0x42b20 60).
// Round 64 (lane 9, fast; 0x42b20 checked each time): round 26's split rebuilt (field test and call
// argument, a tag local for the lookup and the record) 40, 208 bytes (retail's head through the call, but a
// field/local mix, and the local lives to the record so the lookup copies EBP where retail consumes it); the
// tag local for the lookup only with the record from the field 68 (retail's tag copy consumed in place, but
// the source parameter then takes EBP and is passed on the stack: ret 8, 0x42b20 60); that with the normal
// read through the stored shader pointer 70 (the definition takes EBP instead, 0x42b20 62); field test with
// the local for the call and lookup and the record from the field 62 (same code); the call argument from the
// field and the rest from the local 63; test, call and lookup on the field with a record-only local 50 (212
// bytes: frame 0x24, the tag spilled; positional).
// Round 68 (m3 lane 16, fast; 0x42b20 and 0x43720 checked each time): `long bitmap` declared ahead of the
// tag 62; the texture test as `!= NULL` 62 (same code each); test and call on the field with the tag local
// after the call 63; round 26's split re-measured (tag local read before the call, test and call on the
// field, the local for the lookup and the record) 40 (retail's head through the call; left: the tag kept for
// the record, so the lookup copies EBP into ECX, the slot index in EAX against retail's EBP, the normal flag
// at +0x68 and the count/position schedule; a mix of field and local reads, kept only with a match); that
// with the record's tag read through the stored definition pointer 68 (the definition takes ESI, the source
// goes to the stack: ret 8, 0x42b20 60).
// Round 135 (m3 lane 5): retail's 0x1e5e0 copies the record with an inline rep movs and calls nothing. Diagnostics
// only: the record passed to a scratch callee that stores the pointer 119 (152 bytes: VC drops the record stores),
// to one that only reads it 66. Renames (slot/register ties): tag 62, bitmap 62, group 62, image 62, the definition
// byte view 62.
// Round 509: Native tag-field/tag-entry/bitmap-table receiver subsets 1/2/4/3/5/6/7
// and all with the source array receivers 62 each. Original body restored;
// early tag EAX/EBP split, signed-slot residence and shader-relative test remain.
// Round 512: Native source snapshot mutable/read-only 81/81, definition
// snapshot mutable/read-only 100/100, both snapshots in four constness forms
// 124 each. Original body restored; all copies lose the matched caller.
// Early tag EAX/EBP admission and later slot/shader homes remain.
// Round 237: Actual owning source frame fields in short/word domains, with no additional layout,
// typed texture field, typed shader/normal record, both: 62/62/62/62/62/62/62/62. Every form includes
// the new source fields; old standalone tag/shader forms are excluded. Original layouts and full
// body restored; early tag EAX/EBP admission, signed-slot residence and shader-relative test remain.
// DL238: Entry source/definition, slot and shader request-header subsets 1/2/3/4/5/6/7/all plus
// shader-relative normal 66/59/68/67/66/73/73/79. Early slot alone reaches retail length at 59, a
// rejected three-point gain. On it, shared texture-pointer/Boolean admission 69/75; native
// count/slot/shader consumer subsets 59/59/59/69/69/69/69. Shader/admission forms lose matched
// 0x42b20. Complete original body restored; tag admission EAX/EBP split and signed-slot/shader
// homes remain.
// M319: Native subordinate header production via sparse member/mutable aggregate/
// readonly aggregate/sparse library copy, count from published request 79/105/105/79;
// count from producer header 81/105/105/81. Extra producer forms require exact
// equality; all restored. Entry tag/EBP split and signed-slot/shader homes remain.
// Y547: Fresh submitted identity reassigned to the original tag, with shader-relative
// normal/two-use signed count/direct position masks 0..7: 62/67/64/66/62/67/64/66.
// Exact-only field-result forms restored; early tag/EBP and signed-slot/shader homes remain.
// N334: Bitmap/texture phases share tag admission in bool/byte/short/long storage,
// nested then combined consumer guards: 73/73/76/75/73/73/76/75. All extend the body; original
// complete source restored. Early EAX/EBP tag admission and signed-slot/shader homes remain.
// Round 33 (m3 lane 18, fast; 0x42b20 checked each time): a long slot index declared at entry for
// both arrays 79 (223 bytes, 0x42b20 60); the record as one aggregate initializer 87 (229 bytes,
// padding zeroed); the group lookup through `(word)tag` 67 (204 bytes, 0x42b20 4); the record
// filled before the texture test 74 (218 bytes); the vertex count without the short cast 68
// (203 bytes). All restored; the early EAX/EBP tag split remains.
// Round 203 (fast; 0x42b20 checked each time): the tag field read at the test, the call and the lookup
// (no local) 62 (same code); a short vertex-count local computed first, used by the record and the call
// 64 (0x42b20 4); the position through a typed ribbon point pointer 62; the record declared at function
// scope 62; the group lookup folded into the image address 62.
// Round 35 (m3 lane 18, fast; 0x42b20 checked each time): the vertex count stored first 65; the slot
// stored after the shader and normal 62; an early `return` when the tag is NONE 62; the record declared
// at function scope 62; a block-local long point index for both arrays 62; the vertex count
// pushed from its own expression (no short store) 66 (215 bytes). All restored.
// Round 160 (fast; 0x42b20 checked each time): the tag test on the field with the tag local inside it 62 (same code);
// a long slot index for both arrays 62, with the vertex count named first 64 (0x42b20 4), the shader local and
// shader-relative normal too 65 (213 bytes, 0x42b20 62), the shader field-relative only 65 (213, 0x42b20 62); a named
// short vertex count without the slot index 64 (0x42b20 4), a long one 65. Retail's definition stays in EBX and its
// slot word in DX only without those; all restored.
// Round 146 (m3 lane 5, fast; aligned 22 at 62): the tag through a force-inline lookup helper masking its by-value
// parameter in place 62, with the test on the field and the tag local inside 62 (no split either way); the record's slot
// stored ahead of the tag test 59 (210 bytes, aligned 30: three points, not kept); a long index and a named short count
// ahead of the fill 64 (retail's slot/index/count opening, but source and definition swap EBX/ESI, 0x42b20 4); on
// that, eight fill orders with the shader, normal, tag and position in retail's sequence 62-64 (0x42b20 4 each);
// with the normal through the stored shader 66 (213 bytes, 0x42b20 62); the fill in a force-inline helper reading the
// normal through the stored shader 67 (213 bytes, 0x42b20 62).
// Y572: Joint original source/definition/slot formal-name priorities, seven
// subsets then complete tag/bitmap role names: eight 62s. NOT kept: native
// tag EAX/EBP split and signed-slot/shader homes stay wrong; body restored.
// Y583: Actual texture-return pointer/bool/byte/long ownership, original image local
// then direct image expression: 62/62/62/62/62/62/62/62. NOT kept: one-use return
// carriers miss equality; complete body restored; early tag/EBP and slot/shader homes remain.
// Y585: External tag/frame-pair/point-index field bindings, seven subsets and
// all read-only, eight 62s. NOT kept: exact-only identities do not match;
// original captured tag, field observations, complete body and matched caller preserved.
// Y587: Tag producer ending at group/image, positive/NONE admission and positive/failure
// texture admission: eight 62s. NOT kept: scoped lookup outputs do not recover
// early EAX/EBP split or signed-slot/shader homes; original full body restored.
// @retail 0x43740
void function_43740(s_ribbon_source *source, s_ribbon_definition *definition, short slot)
{
    byte *bytes = (byte *)definition;
    if (*(long *)(bytes + 0x34) != NONE)
    {
        long tag = *(long *)(bytes + 0x34);
        long bitmap = function_137590(*(long *)(bytes + 0x34), *(short *)((byte *)source + 0x1a), *(short *)((byte *)source + 0x18));
        byte *group = g_4e3b44[tag & 0xffff].bytes;
        s_bitmap_view *image = (s_bitmap_view *)(*(byte **)(group + 0x48) + bitmap * 0x74);
        if (function_12360(image, 0.0f))
        {
            s_43740_input input;
            short count = source->point_count[slot] * 2;
            input.vertices.source = source;
            input.vertices.definition = definition;
            input.vertices.shader = bytes + 0x3c;
            input.vertices.normal = *(short *)(input.vertices.shader + 0x2c) != 0;
            input.vertices.slot = slot;
            input.tag = *(long *)(bytes + 0x34);
            input.vertices.vertex_count = count;
            input.bitmap = bitmap;
            function_1e5e0(0x20, (long)function_43720, (long)function_42cf0, 0, 4, 0x28, 0x18, count, &input,
                (point3f *)(g_4ea940->data + (source->point_index[slot] & 0xffff) * 0x38 + 0x1c));
        }
    }
}

extern s_record_pool *g_4ea944;

// @retail 0x42b20
void __stdcall function_42b20(dword mask)
{
    long index = data_datum_index(g_4ea944, function_16bc00(g_4ea944, 0));
    while (index != NONE)
    {
        s_ribbon_source *source = (s_ribbon_source *)(g_4ea944->data + (index & 0xffff) * 0x48);
        long tag = *(long *)((byte *)source + 4);
        s_ribbon_definition *definition = (s_ribbon_definition *)g_4e3b44[tag & 0xffff].bytes;
        for (short slot = 0; slot < 4; slot++)
        {
            if ((mask & (1 << (byte)definition->mode)) && source->point_count[slot] >= 2)
                function_43740(source, definition, slot);
        }
        index = data_datum_index(g_4ea944,
            data_find_index(g_4ea944, index == NONE ? 0 : (index & 0xffff) + 1));
    }
}

s_camera_storage g_camera_storage;
