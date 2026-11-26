#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_11cc90.h"
#include <math.h>
#include "unknown_124f90.h"
// @flags /O2 /arch:SSE /Gr
#include "axis_rotation.h"
#include "normalize3d.h"

real function_11ce20(vector3f const *arg_0, vector3f const *arg_1);
void rotate_vector_170d70(vector3f *v, vector3f const *axis, real sine, real cosine); /* inline in src/unknown_170d70.cpp */
real function_11cc90(vector2f const *a, vector2f const *b);

struct s_29b2e0
{
    byte field_0[0x30];
    real field_30;
    real field_34;
    real field_38;
};

PRIVATE __forceinline real function_29b32b(vector3f *arg_0, real *length = NULL)
{
    arg_0->k = 0.0f;
    real local_0 = (real)sqrt(arg_0->i * arg_0->i + arg_0->j * arg_0->j);
    if (!(0.0001f > fabs(local_0)))
    {
        arg_0->i = (1.0f / local_0) * arg_0->i;
        real local_1 = 1.0f / local_0;
        arg_0->j *= local_1;
        arg_0->k *= local_1;
        if (length) *length = local_0;
        return local_0;
    }
    if (length) *length = 0.0f;
    return 0.0f;
}

// score 130 (was 130): The 0x2c frame agrees. The projected J and depth stores exchange order at +0x101.
// Rotation scheduling still differs (retail reads the dot i, k, j and finishes cross_i last, after cross_j and cross_k).
// Earlier first line (151): rotation scheduling and first trig homes still differ.
// Earlier detail: frame 0x2c, the prologue and everything up to the first rotation now agree (1696/1706 bytes; first
// difference +0x101). Left: the two rotations' product order and XMM scheduling, the first rotation's sine and cosine homes
// (+0x3c and +0x10 for retail's +0x10 and +0x3c), the yaw clamp's zero copy below its branch and one store pair's order in
// the second normalization.
// Earlier at 176: r121, r123, r125 unchanged. frame 0x2c agrees (1711/1706 bytes); unity XMM5 versus XMM4, root/acos +0x10 versus yaw +0xc, and rotation homes remain (r83 lead: single-exit normalizer + early XY vector 169).
// r123, r125 unchanged. frame 0x2c agrees (1711/1706 bytes); unity XMM5 versus XMM4, root/acos +0x10 versus yaw +0xc, and rotation homes remain (r83 lead: single-exit normalizer + early XY vector 169).
// MiniMax D (11-19): score 176 (was 176): no new form, no lower score. The single-exit normalizer with the XY vector read before the second copy measures 169 (seven points, under the eight-point bar), the same helper with the original vector order 171 and the two-exit helper with the early vector 174, the length taken from the helper's return 265 and the rotation rows written with the projection term first unchanged, all dropped; the magnitude and the aim length still sit at +0x10 where retail keeps them at +0xc, the final pitch takes a local slot where retail uses the fourth argument's home, and the rotations' sine, cosine and vector copies keep different slots.
// Earlier score 176 (was 176): r123 unchanged. frame 0x2c agrees (1711/1706 bytes); unity XMM5 versus XMM4, root/acos +0x10 versus yaw +0xc, and rotation homes remain (r83 lead: single-exit normalizer + early XY vector 169).
// r123 unchanged (merged from a second score line). frame 0x2c agrees (1711/1706 bytes); unity XMM5 versus XMM4, root/acos +0x10 versus yaw +0xc, and rotation homes remain (r83 lead: single-exit normalizer + early XY vector 169).
// MiniMax B (11-18): score 176 (was 176): no new form, no lower score. The single-exit normalizer with the length
// stored after the branch measures 171 (five points, under the eight-point bar), the length taken from the helper's
// return value 267 and the length local initialised before the call 171, all dropped; the helper's locals renamed
// change nothing, so the magnitude still spills to +0x10 where retail keeps it at +0xc and the unity constant still
// loads into XMM5 against retail's XMM4.
// MiniMax C (11-17): score 176 (was 176): no new form, no lower score. The first normalizer's magnitude still spills
// to -0x10 where retail spills it to -0xc (the vector homes differ too), the unity constant still loads into XMM5
// against retail's XMM4; the length from the helper's return (263), the shared length slot (422) and the length local
// declared first (176) were measured and dropped.
// MiniMax B (11-12): no new form, no lower score (176); the permuter's 129 tries find nothing better than the
// baseline (172 on its reduced image), and retail keeps the unity vector in XMM4 where ours keeps it in XMM5
// and the root at +0xc where ours keeps the yaw there.
// MiniMax H (11-11): no new form, no lower score (176); a normalizer with one exit 403 and the root taken from
// the helper's return value instead of the pointer 263, so the root still sits at +0x10 where retail keeps it with
// the yaw at +0xc.
// MiniMax I (11-10): no new form, no lower score (176).
// MiniMax S (11-06): no new form, no lower score; the single-exit normalizer with the XY vector read before
// the second copy reaches 169, one point short of the eight-point bar, and the original body is restored.
// The rotation's cross terms and rows written with the axis component first, which is retail's order, score
// 176 on the restored body, and the permuter reached no variant on this loaded machine.
// MiniMax C (11-05): no new form, no lower score; the XY vector before the second copy scores 174 and the single-exit normalizer 180/182, both under the eight-point bar, and the permuter reached no variant on this loaded machine.
// MiniMax D (11-09): no new form, no lower score; the length the normalizer stores sits four
// bytes higher in the frame than retail's (ours +0x10, retail +0xc), and the unity constant and
// the scaled components then get different registers.
// Frame 0x2c agrees at 1711/1706 bytes; unity XMM5/XMM4, root +0x10/yaw +0xc and rotation product/register homes remain.
// up to the rotations only the root/1.0 homes and registers differ; the rotation products are hoisted differently. frame 0x2c agrees (1711 bytes against 1706); the sqrt temporaries sit at +0x10 with
// up to the rotations only the root/1.0 homes and registers differ; the rotation products are hoisted differently. frame 0x2c agrees (1710 bytes against 1706); the sqrt temporaries sit at +0x14 with
// frame 0x2c agrees (1710 bytes against 1706); the sqrt temporaries sit at +0x10 with
// frame 0x2c agrees (1710 bytes against 1706); the sqrt temporaries sit at +0x14 with
// Frame 0x2c and early J preload agree; root/yaw homes, one constant in XMM5 versus XMM4, and rotation registers remain.
// Earlier at 244: frame 0x2c agrees (1710 bytes against 1706); the sqrt temporaries sit at +0x10 with
// local_11 where retail packs them with the yaw at +0xc, the XMM numbering from the 1.0 constant on and the
// rotations' register and slot assignment differ; the clamps now have retail's shape.
// Earlier (255, was 267): frame 0x2c agrees (1707 bytes against 1706); the sqrt/acos temporaries sit at +0x14
// frame 0x2c agrees (1707 bytes against 1706); the sqrt/acos temporaries sit at +0x14
// where retail keeps them with the yaw at +0xc (local_11 at +0x10, the final pitch in the scale parameter's
// home), the XMM numbering from the 1.0 constant on, the yaw clamp (retail clamps it in its stack home) and
// the rotations' register and slot assignment differ.
// Earlier (308): length 1706 agrees; frame 0x24 versus 0x2c (local_11 and the rotation spills share
// local_6's slot, retail gives local_11 [esp+0x10] and a spill [esp+0x14] of their own); normalization
// register numbers, the turn-rate clamps (retail clamps the yaw in its stack home, keeps 0.01745329 in xmm7)
// and the rotation differ.
// Round 89 (fast; aligned count 239 at 389): the 2-D normalize through normalize3d after the k store
// 398 (sibling 0x29b990 lost, 10); the actor angle fields read at each use as retail does 404; the two
// angle defaults assigned in the else arm 397 (the three magnitudes then share retail's [esp+0xc]
// slot, but the frame drops to 0x24); only the first angle's default in the else arm 406 (frame 0x28);
// J read into a local before the length 399 (still folded). Retail's else arm copies the angle zero
// from the register that holds the other angle (movaps xmm0, xmm2).
// early J preload and magnitude/yaw home remain; indexed long/short normalization
// loops 417/421; pointer loop 423; loop common result 423; real absolute first/both 399/395; loop with
// real absolute first/both 421/416.
// Earlier: frame 0x2c and length agree; early J load and magnitude/angle homes differ; K first 389, shared axis cross 409, magnitude return 406, global optimization off 2150, float consistency 656, K first plus return 399.
// scalar XY inputs 401 (sibling 10); epsilon failure first 398; geometry before trig 398;
// ordinary inline 389; single dot clamp 396. Early J and scalar homes still differ.
// J snapshot 395; I/J snapshots 392; axis by value 502; destination first 389; XY common result 389.
// real sine/cosine/pair 395/395/389; real yaw inverse cosine 401; shared
// weighted axis/cross/both/input vectors 431/427/420/423; all restored;
// matched sibling and both matched callees preserved throughout.
// Earlier: frame 0x2c agrees; early J load and magnitude/angle homes still differ.
// frame 0x2c agrees; early J load and magnitude/angle homes still differ.
// Earlier at 389 (was 389): frame 0x2c agrees; early J load and magnitude/angle homes still differ.
// Complete input for length 497; input for normalization 496; point input 419; both inputs 513;
// immutable one/both magnitudes 389/389; earlier length publication 396; immutable rotation outputs 389.
// Widened first/point/both numeric returns 407/410/412; widened optional
// magnitude output 404, with first/both widened returns 423/430;
// independent normalization order 411, with widened output 415 (control 389).
// First-return widening loses the matched sibling to 45; all helper contracts
// and normalization order restored. Early J and magnitude/yaw homes remain.
// Earlier: early J preload and shared magnitude/yaw home remain. Fast trials: vector
// early J preload and shared magnitude/yaw home remain. Fast trials: vector
// Earlier at 389 (was 389): early J preload and shared magnitude/yaw home remain. Fast trials: vector
// input/output split 389; first external vector input 441; second external vector input 410; both
// external vector inputs 452; point input/output split 389; both normalizer splits 389; both splits
// and first external input 441; both splits and both external inputs 452. All restored.
// External first-input forms lose the matched sibling in the reduced image; all contracts restored.
// Vector/point/both squared-length arguments 389 each; rotation alignment 389;
// cosine remainder 389; both projection parts 389; clamp parameter result 391;
// squared argument plus clamp parameter 391. Original expressions and helper contracts restored.
// Shared normalizer scale 399; common result 398; early length publication 409;
// nonnegative vector/point/both magnitude tests 382/399/352, shared scale with
// nonnegative vector/point 362/390. Vector-test forms lose the matched sibling; all restored.
// Native real vector/point/both roots 399/401/395; exact-factor shared scale 414, with both real roots 412
// (sibling 59 in both); all-row axis/input/both terms 401/394/389. Original helpers and bodies restored; early
// J and magnitude/yaw homes remain.
// Round 142: shared XY magnitude for vector/point/both 389/389/389;
// with common numeric results 405/389/405; equivalent ordered epsilon tests
// on shared vector/both magnitudes 389/389. Early J and magnitude/yaw homes remain.
// Round 169: Clamp outputs projection/inverse-cosine/both/paired limits/paired steps
// 395/399/395/395/395; all pointer/reference/output-first 395/395/395.
// Branch-owned angles on the current rotation graph 397; all restored. Early J and scalar homes remain.
// Round 160: default numeric length at entry/root 386/396;
// native numeric truth alone/with entry default 397/394; rotation input/axis/both
// native copies 466/502/553; both copies with root default 557. All restored.
// Round 251: Rotation helper role orders axis-input-angle-output, input-angle-axis-output, axis-angle-input-output, angle-axis-input-output, input-output-axis-angle, axis-output-input-angle, output-axis-input-angle, output-angle-axis-input: 389/389/389/389/389/389/389/389. Original helper contract and both alias-safe calls restored.
// Round 373: All-row sine-cross parts, with rounded reciprocal, input parts,
// input+reciprocal, axis parts, axis+reciprocal, all parts, all+reciprocal
// 395/393/389/400/398/404/395/393. Reciprocal forms lose the matched sibling; all restored.
// Round 211: Caller-side CRT copies for first/second/rotation inputs, all seven combinations, 389
// each; all three plus planar input copy 404/1696. Original copies and helpers restored; early J
// and magnitude/yaw home reuse remain. All three matched controls survived.
// Round 87: retail gives the two angles no home of their own: the yaw shares the magnitude slot
// (+0xc), the pitch uses the scale parameter's slot, and the else path writes 0 from the zero register
// at the merge. Both zeroed only in the else branch gives that ending, but the frame shrinks to 0x24
// (the first angle's spill then reuses the 2D vector's slot) 397; yaw only 406; pitch only 406; that with
// the first angle declared at function scope 397; plain reciprocal scaling in the normalizer 400
// (0x29b990 6).
// Round 393: Complete XY square terms in vector/point/both, sums in vector/point/both, both phases and role names:
// 398/395/400/389/389/389/398/398. Original roots/products and all helper contracts restored.
// Round 395: paired square operations J-first in vector/point/both 398/395/400; reversed two-term
// sums in vector/point/both 389/389/389; both J-first and reversed 400, with named sums 398. All restored.
// Round 395: actor/object/both pools at use 490/428/481; all repeated coefficients
// once 400; actor/object/both with coefficients 485/455/494; all with coefficient
// role names 494. Original receivers, coefficient reads, copies and helpers restored.
// Round 238: Returned native rotation value with input/axis pointer-pointer, value-pointer,
// pointer-value, value-value, reference-pointer, pointer-reference, reference-reference and value-
// reference: 450/448/558/556/450/450/450/448. Original scalar captures, two calls and complete
// body restored.
// Round 249: Complete scalar normalizer outputs vector/point/both 392/389/392;
// native vector/point/both 462/428/480; scalar/native and native/scalar 410/462.
// Original helper publications, contracts and matched sibling restored.
// Round 209: Second-vector/XY/both captures before first normalization 401/397/405; branch-owned angle
// defaults with those captures 401/400/401; complete early captures with role names 405, with branch-
// owned angles 401. Original inputs, helper contracts and normalization order restored; early J and
// magnitude/yaw homes remain.
// Round 304: common unchanged/scaled output publication, scalar vector/point/both 480/401/486;
// native vector/point/both 475/454/530; scalar/native and native/scalar 524/469.
// Original normalizers and body restored; early J and magnitude/yaw homes remain.
// Round 442: Five rotation-result scalar declaration orders and three complete role-name/home
// orders, 389 each in eight compiled forms. All matched controls preserved; original helper,
// sibling and body restored.
// Round 27 (lane 14): both 0x11ce20 calls in retail's argument order (retail ecx = arg_3, &local_5) 388;
// with the yaw through the existing function_11cc90 (retail inlines it) 350 (early J preload as retail,
// frame 0x2c); with the angle defaults assigned in the else arm 309 (kept; tail exact, frame 0x24); only the
// yaw/only the pitch default in the else arm 368/368 (frame 0x28); the XY through normalize2d_fast 352 on
// the 350 form, 308 on 309 (kept: retail length 1706, the private 2-D normalizer copy removed); first
// normalizer through vector3f_scale 318.
// Lane 18 round 6: retail's early J load is the else arm's J reload hoisted above the length branch: its then
// arm scales J in place in XMM1 (`mulss xmm1, xmm0`) and I as a memory operand, while ours forms scale * J
// from memory, so only one arm loads J. Retail also keeps the degree factor in XMM7 for all three products.
// Normaliser scaling J first as `j = j * scale` then `i = scale * i` 401 (0x29b990 9); J alone as
// `j = j * scale` 389 and as `j = scale * j` 389 (same code); k zeroed after the length 393; magnitude
// initialised at declaration with both angles zeroed only in the else arm 397. Original helper and body restored.
// Round 54 (lane 16, fast): the actor's +0x658/+0x660 angle fields read at each use (no local_15) 310 (1711
// bytes; the degree factor then loads into XMM7 as in retail); with each delta before its limit 310 (same
// code); the pitch accumulated in local_9 (no local_11) 324; both angles initialised at declaration 368 (1723
// bytes, frame 0x2c); per-use fields with both turn limits as `limit = 2pi; if (field_34 > 0) limit = ...` 295
// (kept: retail's default-then-override shape), with the first limit (pi/2) in that form too 297 (retail's first
// limit is an if/else with jmp); on 295, the rotation helper in the projection-first form with three scalar
// cross terms and rows `v.i * cos + projection * axis.i - cross_i * sin` (src/unknown_2551c0.cpp's unit-axis
// rotation shape) 267 (kept: frame 0x2c, first difference +0x6e; the projection's term order compiles the same);
// on 267: descriptive helper names 267 (same code), the yaw renamed or the normalizer's length and local_11
// renamed to sort last 267/267 (same code), both angles initialised at declaration 369, the existing cross3f for
// the second axis 295 (1737 bytes), the turn test's product delta-first 267 (same code).
// Round 29 (lane 14): retail's frame holds two scalar slots at +0x10/+0x14 (first pitch value, sines, one
// rotation spill) apart from the XY vector (+0x18); the first copy's slot (+0x20) carries the rotation input
// and result, ours uses the second copy's. Fast: the rotation input, the cross axis or the first pitch value
// at function scope 308/308/308 (same code); the XY vector initialised after the second normalization 384;
// both rotations in place on the first copy 311 (frame 0x34); a later XY read (diagnostic) gives frame 0x28.
// Round 120 (m3 lane 5): retail clamps every value in place (the yaw in its +0xc stack home: load,
// conditional stores, reload; the pitch and both turn deltas in their own registers), where ours
// copies each into a separate result register first. The yaw clamp written as direct conditional
// assignments 290 (1732 bytes; that clamp then has retail's in-home shape); the clamp helper as an
// in-place pointer helper at all four uses 292 (1726 bytes; every clamp then has retail's shape, but
// the zero moves XMM1 -> XMM2 and the rest shifts); the helper updating its value parameter and
// returning it 267 (same code); the normalizer's length local renamed to sort before the local_N
// names 267 (same code: its slot stays +0x14); the first length from the normalizer's return (no
// out-parameter) 295. Original body restored.
// Round 267: Conditional sign selection target/current/both 267/267/267; branch-owned angle calls
// target/current/both 294/310/315; both selections with entry pitch 267, both branches with role names 315.
// Original scalar call results, sign tests and body restored; magnitude/yaw homes and XMM allocation remain.
// Round 269: Independent yaw/pitch delta/limit/both 265/255/255; entry declarations 265/255/255;
// per-axis scopes without/with entry declarations 255/255. Keep separate multi-use step limits,
// plain 255; magnitude/yaw homes and XMM numbering remain. All other forms restored.
// Round 112 (m3 lane 7, fast, 0x29b990 listed): the normalizer scaling all three by one reciprocal local
// (`*=` or `local_1 * x`) 370/370 (1714 bytes; retail's 1.0 in XMM4 then, but X is preloaded and the sqrt
// home moves to +0x14; 0x29b990 6); the reciprocal on the left for X only with Y/K scaled in place 370; the
// epsilon test as `fabs(local_0) >= 0.0001f` 392 (0x29b990 42); the clamp helper as one conditional
// expression 277 (1727 bytes). Compile-only screens (calls opaque, inlined angle helper copy) do not track
// the real build here (the reciprocal form scored better there).
// Round 272: Entry declarations target/current/target+current/XY/target+XY/current+XY/all; all
// with complete role names 255/255/255/255/255/255/255/255.
// Original executable body and declarations restored; remaining allocation differences persist.
// Round 30 (lane 2, fast): retail packs the sqrt/acos temporaries with the yaw in [esp+0xc] and gives local_11
// [esp+0x10] alone; ours packs the temporaries with local_11 in [esp+0x10] and the yaw alone in [esp+0xc]
// (the inlined acos result then needs its own slot). The yaw renamed to sort before local_11 255; local_11
// renamed to sort after the yaw 255; both angle declarations moved to the function's top 255 (all the same
// code); the XY vector initialised before the second copy 253 (two-point gain, not kept), before the first
// copy 361 (1710 bytes). Retail also spills the pitch into arg_4's home after its last use.
// Round 272: Entry declarations target/current/target+current/XY/target+XY/current+XY/all; all
// with complete role names 255/255/255/255/255/255/255/255.
// Original executable body and declarations restored; remaining allocation differences persist.
// Round 56 (m3 lane 1, fast, plain-confirmed; 0x29b990 kept its match, 0x29ab90/0x29bab0 unchanged at
// 383/750): the four clamps through the existing PIN_126660 (include/unknown_124f90.h, the same tests as the
// private clamp helper, which is removed) 244 (kept: the yaw clamped in its +0xc home and the pitch in a
// register as retail); on 244: each turn delta clamped in place before the add 264 (1719 bytes); each delta
// computed before its limit 269; a separate pitch delta local 244 (same code); the first rotation reading
// arg_2 with no copy 323 (0x29b990 lost); the XY vector initialised before the second copy 242.
// Round 498: Closed angle/clamp, yaw-rate and pitch-rate regions in all seven nonempty
// combinations, with independent deltas, 244 each; all regions with complete role names 244.
// Root/acos +0x14 versus yaw +0xc and XMM allocation remain; original body restored.
// Round 272: Entry declarations target/current/target+current/XY/target+XY/current+XY/all; all
// with complete role names 255/255/255/255/255/255/255/255.
// Original executable body and declarations restored; remaining allocation differences persist.
// M291: Existing staged rotation cross with result orders ijk/jik/kij/kji, without/with staged pitch axis (grouping exact-only):
// 292/292/292/292/279/279/279/279. Original executable body restored.
// Round 452: pitch-axis production at successful validation, after signed angles, after angular clamps or
// after turn updates, original then shared cross: 407/370/364/284/401/351/345/263. Original late axis, helper
// bodies and full source restored; unity register and magnitude/yaw/pitch homes remain.
// Round 300: Pitch result carries the signed target through the current-angle call: in-place
// conditional/selection, each with current conditional/selection, 244/244/244/244; complete target
// arms/selection with the same current forms 276/276/276/276. Original separate target lifetime,
// native clamps and full body restored; root/yaw homes and constant/result registers remain.
// Round 27 (m3 lane 18, fast): the deltas computed and clamped in place on the angle locals: 244 -> 201 (1715 bytes; yaw and pitch code then
// has retail's registers and homes); both rotations in place on one vector, no separate input: 201 -> 178 (1711); the dot grouped (i + k) + j:
// 178 -> 176. Separate delta locals 211 (1721); hand conditional clamps 295; rotations on the first copy with a final copy 194, first copy as input only 198,
// in place then to the result 241; 27 row/cross spellings with a named or inline cross, temps: 178 (same code), input copy inside the helper 282 up;
// other dot groupings 177-206, accumulated 204-206; named complement 178; shared cross helper inside the rotation 213 (1707, frame 0x24), for the second
// axis 239, both 237; renames of the vectors or pitch 178 (seven forms). Retail's frame shows the rotations on the first copy's slot (+0x20).
// M293: First clamp limit before yaw/after yaw/after gain/after signed target pitch,
// conditional then if-else producers: 344/290/279/282/344/290/279/282. Original lifetime restored.
// Round 286: Joint four-vector lexical priorities for both inputs, result and pitch axis, eight
// forms 176/176/176/176/176/176/176/176. Original in-place body and helpers restored; unity/root/yaw and rotation homes remain.
// Round 517: On the in-place angle/rotation graph, returned first magnitude 263; entry declaration
// 263; XY before second vector 261; both input copies before first normalization 370; first
// magnitude after second normalization 407; early XY with original output/returned magnitude
// 365/371; both vector copies before normalization 289; direct length-test call 407. Original
// numeric output, inputs, helpers and complete body restored; unity/root/yaw and rotation homes
// remain.
// Round 307: Complete projection-dot/cross producers at eight pairs of native trig boundaries on
// the in-place angle/rotation graph, 178 each (1711 bytes). All matched controls preserved;
// original helper and body restored. Unity register, root/yaw and rotation homes remain.
// Round 197 (lane 3, fast with 0x29b990): the pitch's final value in its own local after a separate delta 176 (same code), the
// clamp as its add's left operand 260 (1716 bytes); both rotations on the first copy with a final copy, re-measured 194 (1713
// bytes; retail's slot, other registers); cross terms in other statement orders, the i term written in its row, the
// projection term first in the i and k rows (six forms) 176 (same code); sine, cosine, yaw, pitch and root renamed to sort
// first or last, all 32 combinations 176: names do not move these homes. Body restored. Retail's rows: i projection first,
// j cosine first, k projection first, cross_i computed after cross_j and cross_k and spilled into the cosine's home.
// Round 380: Complete direction-validation truth, bool/byte/short/long positive then
// complementary: 279/279/279/279/279/279/279/279. All retain the wrong root/unity homes and lengthen the body;
// original short-circuit guard and complete body restored.
// Round 382: Both signed rate-policy predicates, bool/byte/short/long sustained
// truth then opposed complement: 284/284/284/284/285/285/285/285. No exact-only predicate qualifies; original
// comparisons, parallel clamps and full body restored. Unity/root/yaw and rotation homes remain.
// DH309: Numeric/optional length result ownership on the in-place angle/rotation graph:
// magnitude common, zero-result common, magnitude-result common, failure-first magnitude/result,
// early success result, branch outputs with common return success/failure first: 171/403/412/182/212/417/203/212.
// NOT kept: five-point best gain; common magnitude fixes early unity and first +0x84, but root/yaw homes remain.
// Round 550: Existing vector setter at normalizer/rotation/pitch-axis output boundaries,
// masks 1/2/4/3/5/6/7: 241/176/238/241/281/238/281; native rotation-result publication 251.
// NOT kept: result/frame forms miss; normalizer setter also loses the matched sibling to 12.
// Original helpers, alias-safe scalar outputs and complete body restored.
// N340: Complete normalized scalar outputs in ijk/jik/kij/jki producer order,
// magnitude publication after/before component stores: 241/241/241/241/363/363/401/363.
// NOT kept: unmatched scalar staging/order; original helper and whole body restored.
// The shared helper also raises sibling 0x29b990 to 12/15/41; no edit retained.
// DH311: Plain native rotation component-array production, with complete cosine/projection/sine
// phase masks 0-7: 218/269/285/340/269/368/315/410. Native output/phase records are exact-only;
// all regress, original alias-safe helper/body restored; root/yaw and rotation homes remain.
// Round 82 (m3 lane 9, fast): retail's acos result, the clamped yaw and the sqrt temporaries share +0xc (ours:
// acos result and sqrt temporaries with local_11 at +0x10). Both arg_2 copies declared before the arg_3 copy 375;
// the yaw in a branch-local copied to local_8 at the arm's end 272 (1727 bytes); result declarations reversed 176
// (same code); the failure arm first 176 (same code); the up vector read through the field at each use 178.
// m3 lane 16 r83 (fast, the 169 plain-confirmed; 0x29b990 matched in each but the 383): the normalizer single-exit
// (`else local_0 = 0.0f;` then one `if (length) *length = local_0; return local_0;`) 171 (DH309's form: unity in XMM4
// as retail); with the XY vector declared before the second copy 169 (retail's local_5.k/local_6.j store order; a
// gain of 7, under the bar: the lead), its components assigned one by one 169 (same code); with the first length
// from the return (no out-parameter) 267; the XY vector right after the first copy 365; the plain reciprocal in
// the normalizer 383 (0x29b990 6); the half-pi limit as if/else 169 (same code); the else arm's zeros chained 172.
// Left on 169: the x87 transfer temporaries (roots, acos) at +0x10 with local_11 where retail packs them at +0xc
// with the yaw, the length/first-i/second-length XMM3/6/5 (ours 6/5/3), the yaw clamp's zero copy hoisted above
// its branch, the pitch spilled into arg_4's home, and the rotations' slots/schedule. All restored.
// J244: Existing staged rotation cross on the retained in-place angle/rotation graph,
// with original/ordinary/staged pitch axis, result setter and shared dot:
// 213/237/237/213/237/237/240/240. NOT kept: all regress; frames 0x24/0x28
// versus 0x2c. Original helper, arithmetic, includes and body restored; unity/root/yaw homes remain.
// Round 82 (m3 lane 9, fast): retail's acos result, the clamped yaw and the sqrt temporaries share +0xc (ours:
// acos result and sqrt temporaries with local_11 at +0x10). Both arg_2 copies declared before the arg_3 copy 375;
// the yaw in a branch-local copied to local_8 at the arm's end 272 (1727 bytes); result declarations reversed 176
// (same code); the failure arm first 176 (same code); the up vector read through the field at each use 178.
// Y571: In-place graph with actual two-use gain/bound and symmetric per-policy rate captures,
// seven subsets 212/270/258/269/261/254/269; complete common numeric normalizer result 264.
// All regress; original helpers and complete body restored; unity, root/yaw and rotation homes remain.
// DL258: Existing vector-union array access in planar normalizer/rotation/caller,
// all seven region subsets, 176 each; complete array graph with shared XY magnitude
// 176; shared magnitude independently on the opening graph 176. Root/yaw homes,
// unity register and rotation homes remain; all other calibrated scores unchanged.
// Original helper contracts, alias-safe reads/publications and complete body restored.
// P320: Sequential magnitude/yaw and target/final-pitch scalar ownership, expired argument
// P320: pitch storage and complete phase combinations 176/179/271/179/271/271/271/271.
// P320: NOT kept: no byte match for explicit scalar/parameter-home reuse; original
// P320: complete body restored. Unity register, root/yaw and rotation homes remain.
// DH342: Existing vector setter at target/current/rotation input construction,
// seven subsets 413/378/211/418/431/403/441; all plus final output setter 436 (sibling 46).
// Original copies, helper contracts and body restored; unity/root/yaw homes remain.
// Y584: Existing normalizer/rotation ordinary-inline and unannotated contracts on
// the retained in-place graph, eight pairs 176 each. Original forced contracts,
// scalar snapshots and full body restored; unity/root/yaw and rotation homes remain.
// J252: Existing rotation output identity and helper expansion contracts: 176/176/176/176/176/176/176/176.
// NOT kept: no qualifying native-frame gain or byte equality; original full graph restored.
// DH349: Complete scalar cosine/projection/sine-cross parts on retained in-place
// graph, seven family subsets 178/195/176/178/176/194/178; all row-local scopes 178.
// NOT kept: no gain; native unity/root/yaw and rotation homes remain. Original
// scalar results, all input reads before output stores, helper and full body restored.
// DH357: Usability return with unchanged optional magnitude and caller expressions,
// bool/byte/short/long direct then common results: 403/403/406/396/405/405/408/398.
// NOT kept: all regress and raise the matched sibling to 64-70; original numeric
// contract, alias-safe helper publications and complete body restored.
// M3 lane 12 r90 (fast; masked = register-masked aligned count, base 176 at 1711 bytes, masked 138): the helper storing its three
// rows directly in place, no i/j/k scalars, 189 at retail's 1706 bytes, masked 114: this gives retail's frame homes (vector +0x20, roots
// +0xc, sine +0x10, cosine +0x3c, pitch +0x40), with the rotation's products and one instruction's shift at the tail left; same form plus
// the 0x2551c0 dot order or a named complement, a named dot, both, a one-pointer in-place helper 189 each; sine/cosine as helper parameters
// 179 (1710), with direct rows 249 (1686, frame 0x28); the first rotation on the first copy with a final copy 194; a scoped fallback pointer
// for the second copy's restore 289. On direct rows: single-exit normalizer 185 (unity still XMM5, masked 114), with the early XY vector 183
// (masked 112), the XY vector alone 187. Diagnostics, identity pointer chains in the helper (not keepable): axis 1-7 links 180/184/184/232/231/231/232
// (masked 119/120/120/162/161/161/162); body chains 1-7 185/184/184/184/184/184/184; axis 0/1/2/4 links each with vector 1-6 links 185/237/185/189; axis 2 links plus vector 1 link 185, masked 100: all three axis product forms as retail
// (x and z load first, y a memory operand), left: dot terms run k first in retail, then the early cross products; with the single-exit
// normalizer and XY vector 233 (1694). dot3f in the helper with axis links 0-7 186/232/181/181/185/185/234/232. Cross statement orders ikj/jik/jki/kij/kji
// on the chain state 185 each (masked 102/100/100/102/102); grouped dot spellings 185, flat ones 181 (masked 113); chains split by dot/cross/rows 185/187/184/237/232/231/192/189/187. Body restored.
// Round 586: Paired yaw/pitch rate-policy table traversal, long/dword/short/word
// ordinals with repeated/captured current-angle values: 426/426/434/434/343/343/337/337. NOT kept:
// nonexact array/field-pointer residence; first four also lose sibling 0x29b990 (48); body restored.
// F385: Paired signed-pitch observation lifetimes, bool/byte/short/long negative then
// complementary truths at their original comparisons: 284/284/284/284/285/285/285/285.
// NOT kept: no exact predicate form; original helper and complete body restored.
// M386: Existing complete XY normalizer with original K tail, common/failure-first/
// success-first/early numeric output, direct then native-planar input: 423/441/441/423/515/544/544/515.
// NOT kept: no byte equality with native unity/root/yaw and all shared users;
// original numeric helper, alias-safe rotations and complete body restored.
// DH362: Native planar epsilon truth bool/byte/short/long, positive then complementary,
// 262/262/262/262/263/263/263/263. NOT kept: all regress and lengthen to 1719/1706;
// original numeric/optional results, every helper and complete body restored.
// DH369: Standard input range copies at desired/current/rotation boundaries,
// subsets 1/2/4/3/5/6/7, then backward copies: 341/437/456/457/448/483/488/488.
// NOT kept: all regress and lengthen; seven forms also lose the matched sibling. Original
// copies, helpers and complete body restored; unity/root/yaw and rotation homes remain.
// DH369 revisit: Void in-place rotation input/axis/output references, subsets
// 1/2/4/3/5/6/7, all 176; full references with the angle also referenced 178.
// NOT kept: no byte match; original helper and both in-place calls restored;
// unity/root/yaw and rotation homes remain. The numeric helper and sibling stay fixed.
// M3 lane 4 r168 (one-file harness with src/unknown_11cc90.cpp and src/character_tag_accessors.cpp, exact at 176, sibling
// 0x29b990 matched throughout). New: the else arm's zeros in reverse order 179, on 169 172; the success test's operands
// swapped (`normalize2d_fast(&local_6) != 0.0f && local_4 > 0.0f`) 397 (1724 bytes), on 169 245; `0.0f < local_4` and
// `0.0f != normalize2d_fast(...)` 176 (same code), on 169 169; the angles declared before the rotation result 176, on 169
// 169. Re-measured listed forms (scores agree): single-exit normalizer 171, with the XY vector before the second copy 169,
// zeros chained 179 / 172. On 169 the roots and the inlined 0x11cc90's acos result sit at +0x10 with local_11 where retail
// packs them at +0xc with the acos argument and the yaw; first root and scaled X in XMM6/XMM5 (retail XMM3/XMM6). Restored.
// DL268: Rotation sinf/cosf/pair, normalizer fabsf/sqrtf/both, all native float APIs,
// then all with one numeric normalizer exit: 179/179/177/263/263/176/177/172.
// NOT kept: four-point best gain is below the eight-point gate; unity changes but root/yaw
// and rotation homes still differ. Original helpers, alias-safe inputs/stores and full body restored.
// DL272: Standard native ranges at final output, failed-normalization restore and facing fallback, seven subsets then backward transfers; unity/root/yaw homes remain. Scores 464/450/471/514/460/512/515/512.
// NOT kept: all forms regress and seven lose matched sibling 0x29b990; original executable source restored.
// DH378: Existing object-tag/actor-fallback/object-axis member sources, subsets 1/2/4/3/5/6/7,
// then all with a read-only object receiver: 176 each. NOT kept: all canonicalize; original complete body,
// raw address expressions, helpers, arithmetic and both alias-safe rotations restored; native root/yaw homes remain.
// DH382: Existing alias-safe rotation results as row-local/full-phase cosine-add-subtract
// or initial-sum/subtract accumulators: 176/178/176/178; with actual dot accumulation
// 178/176/178/176. NOT kept: no gain or native root/yaw repair; original helper and full body restored.
// Round 88 (m3 lane 16, fast with 0x29b990/0x1e5450/0x11ce20/0x11cc90; aligned, masked in brackets; base 176 [229, 138]): single-exit
// normalizer + XY first 169 [222, 136], still one short of the bar; direct rows 189 [180, 105] at 1706 bytes; all three 183 [175, 103]
// at 1706: everything before the rotations then has retail's structure, the rest is the rotations' operand forms and schedule. Rows from
// retail's listing with each cross in its row 264 (1670), with SE/XY 260; 0xd0080's rows with x/y copies 181 (1706) [208, 110]; 0xc7840's named rows
// 176; retail-spelled named rows 176, with SE/XY 170. On all three: named axis at the first call 183 [180, 130]; helper axis link 227
// [350, 157]; helper vector link 179 [177, 122]; both 233 (1694); call + helper axis 227; call axis + helper vector 183 [174, 103]; all
// 233 (1694). Helper parameter orders vaso avso svao ovas vaos soav oasv asvo saov oavs 183 each (same code: copy-propagated). Rows named i 193
// (1690), j 187 (1694), k 183, i+j 181 [244, 128], i+k 193, j+k 183; rows written k,j,i 199 (1683), j,k,i 198 (1687), k,i,j 256 (1675). On
// SE + XY: limit updates as ternaries 169 (same code); named turn products 278 (1716); named low bounds 277 (1716); rate limits as ternaries
// 242 (1715); else-arm zeros reversed 172; pitch computed in its target local 173.
// P388: Complete vector cosine/projection/sine-cross parts through existing
// shared producers on the in-place graph, seven subsets then shared publication:
// 234/247/257/265/282/254/262/265. NOT kept: no exact unity/root/yaw/rotation homes;
// original numeric contract, alias-safe scalar results and full body restored.
// M3 lane 13 r72 (fast with 0x29b990, which stayed matched except where marked). Both builds put spills in dead
// parameter homes: retail the final pitch in the scale's home, the first sine at +0x10 and the cosine in the first
// stack parameter's home; ours the pitch at +0x10, the sine and cosine in those two homes. Retail reads the actor's two
// angle fields from memory at each use and loads the degree factor into XMM7 ahead of the first limit. New forms: the
// two turn-rate blocks as one inline helper taking the current angle and the rate by value, on the single-exit
// normalizer + early XY form 246 (1706 bytes, the fields then read once), on the body below 253 (1706); the same
// helper taking the angle's address and the rates record 278/285 (1716); both angles as arm locals with the two actor
// stores written in each arm 244 on the lead, 251 on the body below (1743), with the result store in each arm too 471
// (1749, 0x29b990 6). Lead re-measured 169. NOT kept: all regress; body below unchanged.
// P399: Actual previous-yaw/previous-pitch/policy field references, subsets 1-7, then immutable actor/object bindings:
// 299/280/262/303/305/272/272/272. NOT kept: no byte equality or native home repair; original complete body restored.
// Round 163 (m3 lane 5, fast, sibling 0x29b990 MATCH throughout): the second normalization's length through its
// out parameter into a local 263 (1719 bytes); the normalizer's length store ahead of the K scaling 176 (same
// code); the first length from the normalizer's return value 263 (1719); the XY pair built after the second
// normalization 368 (1712); the first length from the return value with the planar test as `> 0.0f` 316 (1715).
// M3 lane 10 r90 (fast with 0x29b990, MATCH throughout, and 0x11cc90): no gain. The score line moved to the top of this note (it sat
// below the MiniMax lines). Lead re-measured: single-exit normalizer + XY vector before the second copy 169. On it: both turn tests as
// `product >= 0.0f` 174 (masked 196); the second axis through the existing cross3f 234 (1706 bytes, frame changes, first difference +0x2).
// On 169 the x87 transfer temporaries (roots, acos) still share +0x10 with local_11 where retail packs them at +0xc with the clamp input and
// the yaw; the yaw clamp's zero copy still sits below its branch.
// Round 86 (m3 lane 1, fast with 0x29b990, which stays matched; aligned/masked in brackets, base 176 [133/69]): the
// second axis through the existing cross3f (vector_products.h) on the single-exit normalizer 234 (1706 B [133/75]);
// the rotation helper's three cross terms through cross3f into a vector 213 (1707 B [131/55], frame 0x24 against
// 0x2c, the root then at retail's +0xc), with the single-exit normalizer 209 [127/55], plus the rows stored directly
// 209 (same code), plus the XY vector before the second copy 209 [126/54]. Single-exit normalizer re-measured 171.
// Round 179 (m3 lane 4, fast with 0x29b990, 0x1e5450, 0x11ce20 and 0x11cc90, all matched each time): the rotation
// vector as the first input copy's own variable (no separate rotation local) 192 (1708 bytes); the second axis rows
// with every product's factors swapped 176 (same code); those rows written k, j, i 190 at retail's 1706 bytes (masked
// 72 against 69), with the single-exit normalizer 185 (that normalizer alone re-measured 171); the rotation's
// projection computed after the three cross terms 176 (same code). Left: sine/cosine/pitch homes (retail +0x10/arg_3's
// home/arg_4's home, ours the pitch at +0x10 and sine/cosine in the two homes) and the rotations' schedule.
// Round 78 (m3 lane 14, fast with 0x29b990; masked aligned in brackets, base 176 [125]): the normalizer with one named
// inverse for all three components 384 (1715 bytes) [162], 0x29b990 then 6; the else arm's angle zeros chained 179
// [126]; the result vector stored before the two angle fields 182 [128]; the second normalization's failure test as
// `!` 203 (1718 bytes); the XY normalization's result tested as a bool 176 (same code).
// Round 105 (m3 lane 16, fast with 0x29b990, which stayed matched): the entry convention (ECX/EAX indices) agrees and
// 0x11cc90 is already called. From +0x6e the differences are homes: retail's first root sits at +0xc (ours +0x10), and
// the rotated result's spill homes use the first input copy's slot (+0x20, as retail's `mov ecx, [esp+0x20]` at the
// join shows) where ours uses the second's (+0x2c), which moves every load order in the two rotations. New forms: the
// rotation helper in 0x1f6090's actor_path_rotate spelling (cross with the axis first, aggregate result copy) 289 at
// 1757; the actor fields and `*arg_5` stored in each arm with no shared result local 246 at 1741; the XY pair read
// from the `local_5` copy instead of `arg_2` 430 at 1744; the second rotation's axis computed before the first rotation
// 220 at 1731 (masked 95 -> 89). Re-measured: 0x11d3b0's rotation spelling (k, j, i dot, rows stored in place) 189 at
// 1706 (the listed direct rows; masked 95 -> 68); the result local declared first 176 (same code).
// m3 lane 5 r169 (fast with 0x29b990, which stays matched; aligned/masked in brackets): re-measured the r83 lead
// (single-exit normalizer + the XY vector before the second copy) 169 [147/68]. On it, new: the second axis computed
// before the first rotation's input copy 213 (1731 B); yaw_limit declared before the yaw delta 169 (same code); the
// else arm's zeros before its vector copy 169 (same code); the half-pi limit computed before the yaw call 339 (1730 B).
// The normalizer's single exit through a separate `result` initialised to zero 215 (1718 B). Left on 169: the x87
// sqrt temporaries at +0x10 (retail +0xc with the yaw), the XMM3/5/6 rotation in the normalizers, the yaw clamp's zero
// copy above its branch and the rotations' pitch/sine/cosine homes (retail pitch in arg_4's home, sine at +0x10).
// m3 lane 14 r84 (fast with 0x29b990, 0x11cc90, 0x11ce20 and 0x1e5450; masked = registers and esp offsets masked,
// base 114): the existing rotate_direction (unknown_1eb8a0.cpp) for both rotations, sin/cos as arguments or one named
// pair, 817/817 (not inlined here: 1309 B, 0x29b990 48); the helper taking sine/cosine with one pair named here and
// reused 225 (1735 B; the root temps move to +0x14); four named sine/cosine locals 179 (1710 B). Re-measured on this
// base: the first rotation reading arg_2 with no copy 265 (1706 B, 0x29b990 1). No untried natural form found: the
// rest is the rotations' and the second axis's schedule, which every listed spelling leaves. Also new: the two angle
// fields stored inside each arm (zeros written directly in the else arm, the result vector still shared) 251 (1743 B).
// Round 93 (m3 lane 10, fast with 0x29b990/0x11cc90/0x11ce20/0x1e5450, all matched): on the re-measured 169 lead
// (single-exit normalizer, XY vector before the second copy) the rotation's projection dot named before sin/cos, the
// cross terms before sin/cos, both, `local_11` declared at function scope and `local_4` declared before the first copy
// all give 169 (same code). The yaw/pitch clamp block already matches retail instruction for instruction. Slot map on
// the lead: retail packs the root and acos temporaries with the yaw at +0x00 (23 uses), ours with local_11 at +0x04
// and the yaw alone at +0x00; the XMM numbering and the rotations' schedule follow from that.
// m3 lane 1 r98 (fast with 0x29b990, 0x1e5450, 0x11ce20, 0x11cc90, all matched throughout; control 176). New, all fruitless: the
// second angle (arg_2 against its copy) computed ahead of the first 283 (1706 B); the planar test as `> 0.0f` alone 328 (1707 B);
// the second normalization's failure test as `!function_29b32b(&local_5)` 203 (1718 B); the XY vector as the planar copy
// `*(vector2f const *)arg_2` 356 (1715 B, first difference +0x60); the pitch field stored before the yaw field at the end 178.
// Re-measured on this base (listed on the 169 lead): the turn limits as ternary initializers, yaw/pitch/both 261/242/249
// (1715/1713/1715 B); the elevation limit as a default plus `if` 272 (1709 B; same code on the 169 lead). Same code: the
// first angle's sign as a self-assigning ternary 176.
// m3 lane 3 r239 (fast with 0x29b990, 0x1e5450, 0x11ce20, 0x11cc90, all matched throughout; control 176): the second axis
// read through a named `forward` pointer beside the up pointer 178, on the r83 lead (re-measured 169) 171; the second axis
// as an aggregate initializer 176 (same code). On the lead retail's pitch, sine and cosine sit in arg_4's home, +0x10 and
// arg_3's home where ours puts them at +0x10, arg_3's and arg_4's homes, and the acos temporary shares the yaw's +0xc in
// retail but local_11's +0x10 here.
// P485: Conditional input/axis/both/shared alias copies in the existing rotation:
// 286/485/459/428; shared overlap decisions bool/byte/short/long 428 each.
// NOT kept: no byte match; frames/extents regress, axis-only also loses the sibling.
// Original alias-safe staging, helpers and whole body restored; unity/root/yaw homes remain.
// M3 lane 15 r83 (fast, 0x29b990 matched each time; LCS-aligned plain/register-masked/stack-masked, control 153/95/74): any
// `__assume` (definition, arg_3, arg_2, actor or arg_5 non-NULL) 289 (1724 B). Rotation helper: the projection sum without
// its parentheses, all six term orders 178 (154/95/74; one code: an unparenthesized float sum is put in a fixed order, the
// explicit `(i + k) + j` here is honoured and is retail's grouping); rows as `(a + b) - c`, either operand first, 178
// (157/105/103); retail-ordered operands (axis-first cross terms and rows) 178; the cross product in a named vector3f 214
// (1707 B, 193/128/61).
// M3 lane 12 r123 (fast with 0x29b990, matched each time): the single-exit normalizer re-measured 171 (first +0x84); on it the
// rotation helper's cosine before its sine 173, its cross terms ahead of the projection 171 (same code), the result copy ahead
// of the two angle stores 177, both turn limits as conditional expressions 244 (1715 B). Retail keeps the rotation's sine at
// +0x10, its cosine in arg_3's dead home and the final pitch in arg_4's; ours gives the two dead homes to sine and cosine.
// M3 lane 16 r119 (fast with 0x29b990, 0x11ce20, 0x11cc90, 0x1e5450, all matched each time; control 176): the two angle
// clamps in the other order 194; the pitch delta computed after the elevation limit 213; the second angle's sign folded
// into a conditional around the 0x11ce20 call 302 (1724 B); the yaw and pitch deltas as named locals with each clamp
// result added back in one statement 244 (1712 B); the object lookup ahead of the actor lookup 423 (1708 B, first +0x9).
// Same code: the rotation helper's cross terms in j, k, i order; the helper made in place (vector, axis, angle); its
// projection written `(1.0f - cosine) * (...)`. Retail's second rotation writes local_7 over local_3's dead home (+0x20),
// ours over local_5's (+0x2c).
// M3 lane 11 r103 (fast with 0x29b990, matched each time; control 176 at 1711 B against retail's 1706): the rotation helper's
// cross terms and rows in the path-rotate operand order (axis-first cross terms; rows `projection * a.i + v.i * cosine`,
// `cosine * v.j + projection * a.j`, `projection * a.k + cosine * v.k`) 176 (identical code: the commutative operands are
// canonicalized); renames of local_7 (direction, result), local_3 (aim, wanted), local_5 (target), local_8/local_9 (yaw,
// pitch) and the normalizer's root (magnitude, root) 176 each (identical code); local_5 declared ahead of local_3 176
// (identical); the clamp bounds as `0.0f - limit` for yaw, yaw and pitch, or all four 176 (identical); local_9, local_8,
// local_7 declared in reverse order 176 (identical). Retail packs the three x87 roots and the inlined 0x11cc90 angle into
// the yaw's +0xc slot (ours +0x10, with local_11) and the rotated vector into local_3's home.
// M3 lane 13 r96 (fast with 0x29b990, 0x11cc90, 0x11ce20, 0x1e5450, all MATCH each run; control 176): the r83 lead (single-exit
// normalizer, XY vector before the second copy) re-measured 169. Retail packs the rotated vector into local_3's dead home
// (+0x20); the rotation written into local_3 itself (no local_7) 192 on this body (1708 B: the vector then sits at +0x20, but the
// pitch, sine and cosine homes stay +0x10/+0x3c/+0x40 against retail's +0x40/+0x10/+0x3c), 185 on the lead; the second axis
// through the existing cross3f of vector_products.h (`cross3f(&forward, axis, &local_18)`) 239 (frame and first difference
// +0x2 move); on the lead the XY vector declared before the first copy 365 (1714 B); the angles initialised to zero at their
// declarations with the else arm copying only the vector 360 (1723 B). Same code on the lead: the angles declared at the
// top of the function; the normalizer's locals renamed (magnitude, inverse); local_11/local_12 renamed (aim_pitch,
// target_pitch); rows with the projection term first.
// M3 lane 2 r88 (fast with 0x29b990, MATCH each time; control 176): the keepable half of the r83 lead, the single-exit
// normalizer alone, re-measured 171 (first +0x84; a gain of 5, under the bar). On it the remaining slot differences are the
// normalizer roots and the inlined angle's acos result at +0x10 (retail +0xc with the acos argument and the yaw), and in the
// rotations the final pitch at +0x10 with sine/cosine in the two dead parameter homes (retail: pitch in arg_4's home, sine +0x10,
// cosine arg_3's home). New on it: rotation rows with the projection term first in the i and k rows 171; the second
// normalization's failure test as `!` 198 (1718 B); the half-pi limit as if/else 171; the else arm's zeros chained 174; the
// two angle declarations moved up to the lookups 171.
// P538: Unrounded planar-root/rotation-sine/rotation-cosine API producers, subsets
// 1/2/4/3/5/6/7 then complete immutable: 263/179/179/263/263/177/264/264.
// NOT kept: all regress; combined root/trig forms lose the matched sibling (24); original helpers and full body restored.
// MiniMax E (11-15): no new form, no lower score (176): the normalizer's length taken from its return value instead of the
// out-parameter measures 263 at 1719 bytes, and retail keeps that length in a register where ours holds an address-taken local.
// P569: Maximum-angle policy positive bool/byte/short/long outcomes 281/281/281/281;
// complementary outcomes 287/287/287/287. NOT kept: no native storage equality;
// original producer/body restored. Unity, root/yaw and rotation homes remain.
// M3 lane 11 r111 (fast with 0x29b990, 0x11cc90, 0x11ce20 matched): the rotation dot's products with their factors in
// each order (v first in i, in k, in both, k term first) 176 x4 (same code: products and sums are canonicalized); the two
// pitch terms in their own block 176 (same code); single-exit normalizer re-measured 171, with that block 171; the
// rotation vector or either input copy renamed (rotated, aim_direction, zz_direction, unit_facing, unit_target) 176 each:
// names do not move retail's packing of the rotation vector into the first copy's slot.
// M3 lane 4 r207 (fast with 0x29b990; masked = registers and stack offsets masked, aligned, base 176 at masked 114): the
// rotations through the in-place helper 0x2551f0 and 0x117dd0 now share (include/axis_rotation.h) with the sine and
// cosine as call arguments, or as locals before or after the copy: 244 (1694 B, frame 0x28, masked 97); with the second
// axis through the existing cross3f 257 (1679 B, masked 81); that axis computed before the first rotation 184 (1706 B,
// frame 0x2c, aligned 187, masked 112; retail computes it after); the matched sibling 0x29c860's angle-taking helper for
// both rotations 245 (1691 B, masked 97); the first two normalizations through the existing normalize3d after the k
// store 186 (1718 B, masked 179).
// M3 lane 7 r175 (fast with 0x29b990, MATCH each time; control 176): the first pitch angle accumulated in local_9 (no
// local_11: `local_9 = angle; negate; local_9 -= local_12;`) 179; the single-exit normalizer re-measured 171 (unity now
// XMM4 as retail; the first root takes XMM6 and the first i result XMM5 where retail has XMM3 and XMM6); on it one turn-limit
// local reassigned for both clamps 173 and the XY vector declared after the second normalization 368 (1712 B); the
// normalizer with a `result = 0.0f` local declared first, set to the root in the success arm 217 (1718 B). The
// normalizer's locals renamed (magnitude, inverse) re-measured 176 (listed, same code).
// Round 401 tested shared planar magnitude from the vector prefix or complete XY value, each direct, before the depth clear, with common length publication, or with shared scaling: 176/396/171/255/178/396/173/254.
// The common-publication prefix form restores XMM4 and starts differing at +0x84. The root home remains +0x10, and the five-point gain is below the threshold.
// M3 lane 13 r107 (fast with 0x29b990, 0x11cc90, 0x11ce20, 0x1e5450, all MATCH in each; control 176, difflib-aligned 229, masked
// 114): the normalizer storing its length before it scales 407 (1720 B); one `limit` local reassigned for the elevation limit
// and both turn limits 176 (same code); the final pitch as a new local used by the second rotation and the store 203 (1718 B);
// the final yaw and pitch both as new locals 179 (1711 B, masked 199); the pitch difference as `local_9 = local_11; local_9 -=
// local_12;` 287 (1721 B). Read from the listing: retail gives its 4-byte homes out in the order roots/acos/yaw (+0xc), first
// pitch then the first sine (+0x10), cosine (arg_3's home), final pitch (arg_4's home), i.e. the final pitch gets its home
// after the first rotation's sine and cosine; ours homes the yaw first (+0xc) and the final pitch before the sine and cosine.
// Body unchanged.
// Earlier first rotation source at angle solve or completed yaw 430/435. First yaw rotation before pitch
// subtraction, limit initialization, base limit selection, limit scaling, clamp or final add
// 484/471/480/467/479/281. The last retains frame 0x2c and XMM5 for unity. Earlier sites grow the frame.
// Round 416: The 0x2c frame agrees. The first differing instruction loads unity into XMM5, where retail uses
// XMM4. The root and acos scratch share +0x10 in the build and share the yaw slot at +0xc in retail.
// fallback first at planar admission 176; fallback first with common planar length publication 171; initial
// angle computations in one local scope 176; initial angle scope with common planar length publication 171;
// independent yaw and pitch clamp scopes 176; independent clamp scopes with common planar length publication
// 171; reuse the first direction after admission and publish planar length once 187; reuse the first
// direction with fallback first and publish planar length once 187. NOT kept: Common planar length
// publication restores XMM4 and shifts the first difference to +0x84. Its best score is 171, five points
// below baseline, with the root still at +0x10. Reusing the first direction moves the extent to 1708 bytes
// but scores 187. The opening body and normalizer are restored.
// M3 lane 15 r99 (fast with 0x29b990, 0x11cc90, 0x11ce20, 0x1e5450, all MATCH unless noted; [n] = aligned differing lines,
// control 176 [229]). The rotation helper's result through the by-value vector3f_assign with the three rows as arguments
// 190 at retail's 1706 B [214] (the rows are then evaluated k, j, i: the same code as the rows written k, j, i). The
// normalizer's scaling through the existing vector3f_scale (normalize3d.h) in place of its written-out body 255 (1719 B,
// first difference +0x84) [243]; with the rotation setter 226 (1710 B) [259]. The normalizer's scaling through
// vector3f_assign 395 (1715 B), with the rotation setter 368 (1709 B) [197]; 0x29b990 5 in both. The single-exit normalizer
// (`else local_0 = 0.0f;` then one length store and one return) with vector3f_scale 245 (1719 B) [233]: unity XMM4, the
// first root XMM3, the first i product XMM6 and the second root XMM5 are then retail's (the written-out single exit, 171
// re-measured, has XMM6/XMM5/XMM3 there), the first 155 instructions differ only in the root slot (+0x10 against +0xc),
// but the yaw scale then multiplies into a copy (`movaps xmm1, xmm2; mulss xmm1, xmm0`) and the code is 13 bytes long; that
// with the rotation setter 216 (1710 B), with the XY vector before the second copy 243, with both 214; with the planar
// length through dot3f 245 (same code). Re-measured: single-exit 171, with the XY vector first 169. Reading: retail packs
// the three roots and the inlined angle helper's arc-cosine result into the slot its clamp input and the yaw share (+0xc);
// ours gives the clamp input and the yaw +0xc but packs the roots and the arc-cosine result with the first pitch angle at
// +0x10, so the yaw's first value is a copy of the helper's result where retail's is the result itself.
// Shared 2-D normalization of the XY prefix with depth scaling and common length, early zero,
// separate length arms, or length before depth: 423/441/441/423. With the first magnitude returned:
// 420/429/430/420. NOT kept: root and yaw homes still differ and the shared normalizer raises 0x29b990 to 31.
// M3 lane 6 r195 (fast with 0x29b990, MATCH each time; control 176): the yaw through a `real &` binding for its computation
// and scale 272 (1726 B); the pitch through `real &pitch = local_9; pitch = local_11; pitch -= local_12;` 290 (1723 B); the
// rotated vector through a `vector3f &` binding for the copy and the first rotation 181; the normalizer's squared length named
// before the root 176, on the single-exit normalizer 171 (both the same code as without the name).
// M3 lane 10 r123 (one-object /GL harness with src/unknown_11cc90.cpp and src/character_tag_accessors.cpp linked, byte-exact
// against the fast build; aligned counts against retail in brackets, base [229]): renames do not move the 4-byte homes here:
// local_8 as two names on either side of the others [229], the normaliser's root local as two such names [229], local_9 as
// five names [229]. The else arm's zeros chained `local_8 = local_9 = 0.0f` [232], `local_9 = local_8 = 0.0f` [229]; the final
// pitch add and the final yaw add each written field-first [229]; local_9, local_8, local_7 declared in that order [229]. A
// forceinline turn-rate helper shared by the yaw and pitch clamps (delta, default limit, field override, scale unless
// turning back, clamp, add back; the current angle through a pointer) [248], fast 285 (1716 B); with the current angle by
// value [314]; the delta formed after the limit [317]; the clamp and add returned as one expression [337]. Mapping the
// harness homes onto retail's: retail shares the root locals' home with the yaw (+0xc) and keeps the first pitch component
// alone at +0x10, then the first rotation's sine at +0x10 and its cosine in the first stack argument's home, and the final
// pitch in the scale's home; ours shares the roots with the first pitch and the final pitch (+0x10).
// Unity remains in XMM5 and the root still shares the pitch slot. Separate desired/bounded/difference
// angle pairs 269/269/244. Desired plus bounded, desired plus difference, bounded plus difference,
// or all three 269/265/265/265. One lower bound for both angular clamps 284.
// M3 lane 10 r125 (fast with 0x29b990/0x11cc90/0x11ce20, all MATCH; control 176; the r123 harness gives the slot groups):
// ours groups the normalizer roots, the inlined yaw helper's arc-cosine result and the planar magnitude with the first
// and final pitch (+0x10) and gives the yaw only the clamp input, where retail groups them with the yaw (+0xc) and keeps
// the final pitch in the scale parameter's home. Fast: the second pitch angle computed before the first 283 (1706 B);
// the yaw's field_38 scale moved after the pitch difference 312 (1718 B); the else arm's zeros stored before the
// fallback vector copy 176 (same code); the pitch kept in local_11 through both clamps with local_9 first assigned at the
// final add 176 (same code: the homes follow the value webs, not the names); that on the single-exit normalizer with the
// XY vector first 174. Harness only: the yaw helper written with a returned conditional (in a copy of its file) worse;
// local_8/local_9 initialised to zero at their declarations gives each its own slot (aligned 253). Body unchanged.
// Single yaw-gain read, in-place/conditional update 212/212. Separate projected yaw
// with repeated/one gain read 293/288, and separate result arms gain/fallback first
// 288/301. Narrow yaw phase 314, gain read in the scaling arm 303.
// NOT kept: all scores rise. Unity and the shared root/yaw slot still differ.
// Separate XY transfers at the copy or around the current-vector copy 176/176;
// two-component union copy 356. Early desired-heading test after the first root,
// current-vector copy or XY copy 408/402/420. Early failure test 387; test after
// current normalization with component copies 279. NOT kept: early heading
// and failure tests recover unity in XMM4 but still spill the root at +0x10.
// M3 lane 7 r188 (fast with 0x29b990; control 176, aligned 229, register-masked 179): the private normalizer as `k = 0;
// m = normalize3d(v);` (include/normalize3d.h) 245 at 1719 B: the XMM numbering of all three normalizers is then retail's
// (the first 155 rows differ only by the root cell, +0x10 against +0xc), but the yaw scale multiplies into a copy and the
// matched sibling 0x29b990 drops to 10; the 2-D root written out with vector3f_scale and one exit 245 (same code), with the
// early return 255; the two angles declared in the turning block with their two stores written in both arms 251 (1743 B),
// with the normalize3d form 300. Reading: retail stores everything that comes off the x87 stack (the three roots, the
// arc-cosine result) into the cell the yaw then lives in (+0xc); ours gives those +0x10 (shared with the first pitch angle)
// and keeps +0xc for the clamped cosine and the yaw in every one of these forms, block scope included.
// M3 lane 13 r121 (fast with 0x29b990, 0x29ab90, 0x29bab0, 0x11ce20, 0x11cc90, 0x1e5450; control 176 = plain, aligned 229):
// the length, yaw, pitch or first-angle local renamed to sort first or last, or all three given natural names, 176 each
// (same code); the rotated result, the cross axis or either normalised copy renamed 176 each (same code): neither the
// 4-byte nor the 12-byte homes follow names here. The rotated result kept in the first normalised copy's variable 192
// (1708 B, aligned 228), in the second's 209 (1715 B); the final yaw and pitch as their own variables with the deltas
// block-local 179; the two angle locals declared at the top 176 (same code). Read from the listing: retail's three
// square-root homes and the yaw share +0xc, its final pitch is stored in the dead scale parameter's home (+0x40) with
// the first rotation's sine at +0x10, and the rotated result is written into the first copy's home (+0x20); ours gives
// the yaw +0xc, the roots +0x10, the pitch +0x10, that sine +0x40 and the result the second copy's home (+0x2c).
// M3 lane 13 r123 (fast with 0x29b990, 0x11cc90, 0x11ce20, 0x1e5450, all MATCH unless noted; control 176, difflib-aligned 229,
// masked 114): what puts the three roots in the pitch's slot. Diagnostics (reverted): with the second rotation fed the yaw, or
// with the final pitch store removed (1706 B, frame 0x28), the roots and the arc-cosine result move to the yaw's +0xc as in
// retail; so they follow the pitch whenever one value serves both the second rotation's x87 loads and the store after the
// join. Forms: the final pitch add moved to just before the second rotation 279 (1719 B), to after the first rotation 281
// (1723 B), to after the vector copy 176 (same code); the rotation helper's angle by const reference 178, copied into a
// helper local 178, the cosine taken before the sine 178, the angle typed double 171 (1707 B; a type picked by score, five
// points); block-local yaw and pitch with the function-scope results assigned at the end of the arm 179, pitch only 203
// (1718 B); the pitch result assigned right after the final add with the rotation reading the block-local 209 (1727 B: roots
// +0xc as retail, aligned 194, but the pitch is then stored twice), assigned just before the second rotation 214 (1733 B, roots
// +0xc); the rotation reading the result variable 176 (same code); the two angle stores written in each arm 251 (1743 B); the
// final pitch formed as the second rotation's argument with the add repeated after it 298 (1742 B, roots +0xc), as a named
// local after the first rotation 291, before it 203. Diagnostic: the final pitch kept in the scale parameter's variable 173
// (aligned 202): its home becomes retail's +0x40, the first sine then takes +0x3c and the cosine +0x10 (retail +0x10 and
// +0x3c) and the roots stay at +0x10; a parameter reused for another value, not kept. Reading: retail gives the pitch its
// home after the first rotation's sine and cosine have theirs (roots and yaw +0xc, first pitch angle and both sines +0x10,
// both cosines in the first stack argument's home, the pitch in the scale's home); ours homes the pitch first. Body unchanged.
// MiniMax C (11-19): no gain (176, baseline; body restored). The single-exit normalizer with the XY vector before the second copy is the known 169, a gain of seven and under the bar; on it the roots and the inlined 0x11cc90 arc-cosine result sit at +0x10 where retail packs them at +0xc with the clamp input and the yaw. Re-measured here: the single-exit normalizer written with a result local and a length store 404, with a bool and ternaries 270, and the XY vector before the second copy 174.
// shared planar scale at yaw admission 176; planar dot for projected lengths 263; planar dot with shared
// yaw scale 263; rotation basis dot with shared yaw scale 177; staged rotation cross with shared yaw scale
// 213; actor angle lvalues with shared yaw scale 284; shared rotation products with shared yaw scale 209;
// aggregate rotation output with shared yaw scale 251.
// NOT kept: the shared planar scaler ties at 176. The shared rotation products reach 1706 bytes with a
// smaller, nonretail frame and a higher score. Unity in XMM5 and the shared root, inverse cosine and yaw
// home remain unresolved.
// M3 lane 7 r193 (fast with 0x29b990, 0x11cc90, 0x11ce20, 0x1e5450, all MATCH each time; control 176, aligned 153 by
// difflib with addresses masked): the private normalizer single-exit (`result = 0.0f`, set to the root in the success arm,
// one length store after the branch) 217 (1718 B, aligned 157). LEAD: this function's first two normalizations as
// `k = 0.0f;` then the existing normalize3d (0x29b990 keeps the private helper and stays matched) 186 (1718 B) but aligned
// 139: unity in XMM4 and every XMM of the three normalizers retail's; left are the root and arc-cosine cell (+0x10 for
// +0xc), one store pair's order, the yaw clamp's zero copy below the branch, and the first rotation, which ours
// interleaves with the cross axis. On it: the second pitch angle computed before the first 270 (1711 B, aligned 151); the
// rotated copy stored before the two angles 186 (aligned 142); only the first normalization through normalize3d 266
// (1717 B, aligned 205). Body unchanged.
// M3 lane 5 r215 (fast with 0x29b990, 0x11ce20, 0x11cc90, 0x1e5450, all MATCH unless given; control 176; plain check 151
// with 0x29b990 MATCH, 0x29ab90 286 and 0x29bab0 489 unchanged). KEPT, 151 (1696 B): both normalizations through the
// existing normalize3d after the k store, both rotations through the existing in-place helper of axis_rotation.h with the
// sine and cosine as its arguments, and the turned facing built in the first copy's variable (local_3; the separate
// local_7 is gone, as is the private rotation helper nothing else used). Retail keeps the turned vector in the first
// copy's home (+0x20) and has two 12-byte vector homes; with a separate result variable this body measures 234 (1687 B,
// frame 0x28: the result is packed with the second copy and one 4-byte cell is missing), whatever the three vectors'
// declaration order (result declared first, between the copies, ahead of everything, second copy declared first: 234
// each, same code). On the kept body the final pitch sits in the scale argument's home, the roots, the arc-cosine result
// and the yaw share +0xc, the second rotation's sine and cosine are at +0x10 and +0x3c and the tail is retail's.
// Measured on the way: normalize3d alone 186; with the final pitch a new block local read by the second rotation and
// copied to local_9 at the arm's end 160, copied right after the add 179 (first difference +0x101), copied ahead of the
// second rotation 188; that final-pitch local on the old body 203; a final-yaw local as well 232, alone 161; the pitch
// change in a block local with the function-scope pitch first assigned by the final add 186, on the old body 176; the
// change kept in the first angle's variable 186 (old body 176), in the second angle's 186 (same code each: the homes follow
// values, not variables); block-local change plus final-pitch local copied at the end / at once / mid 160 / 179 / 188.
// The shared rotation helper on the old body 244, on normalize3d with the sine and cosine as arguments 234, as two locals
// reassigned for the second rotation 244, declared at the function top 244. With the turned vector in the first copy:
// sine and cosine as arguments 151 (kept), as reassigned locals 234 (frame 0x28), declared at the top 234, as two named
// pairs 224, a named sine with the cosine as argument 151 (same code), a named cosine with the sine as argument 179; with
// the final-pitch local copied at the end 176 (1721 B), at once 193, with two named pairs 178; the cross axis through the
// existing cross3f 171; the old normalizer with the shared rotation 237; the private rotation with normalize3d 162, with
// the final-pitch local 159; the turned vector in the second copy's variable 162 (1707 B); the copy made ahead of the yaw
// clamp 428 (0x29b990 48). On the kept body: the XY vector read ahead of the second copy 149 (first difference +0x179; a
// two-point gain, not kept), after the depth clear 151 (same code), built from the copy 316.
// M3 lane 5 r217 (fast with 0x29b990, 0x11ce20, 0x11cc90, 0x1e5450, 0x29c860, all MATCH; control 151, aligned 178 rows
// by difflib with jump targets masked; a one-file link-time harness of this file with the angle helper's and the tag
// accessor's objects reproduces the fast build row for row and screens a form in half a second). Phase diagnostic
// (reverted): one, two or three redundant parenthesis pairs on the pitch difference 255 / 295 / 245 (1721 / 1706 /
// 1708 B, aligned 242 / 291 / 214); pairs around a lone field read change nothing, pairs around a product do.
// The matched sibling 0x29c860's private rotation (angle taken, sine before cosine, cross j and k staged in a vector,
// cross i inline in its row, rows `a.i * p + c * v.i`, `c * v.j + p * a.j`, `a.k * p + c * v.k`) has retail's operand
// spelling for every product and row here; called for both rotations through a copy of it 192 (retail's 1706 B, but
// frame 0x34: its cross vector gets 8 bytes of its own, aligned 254). Harness screens, aligned rows, each a private
// rotation in that spelling (a new one-caller helper, so match-only): cross j and k as scalars with sine and cosine as
// arguments 162 (frame 0x2c again) for every dot order; with the vector's k read first 164; taking the angle 171; the
// cross in a vector 220 to 238. With 0 to 3 parenthesis pairs around the second axis's first product and the three cross
// terms as scalars: 134 at 3 pairs with the dot as (j + k) + i. A coordinate descent over the trig form, the k local,
// an axis alias, 12 dot groupings, 3 product spellings, 6 cross forms, 4 row forms, in or out pointer, both pair
// counts, the XY pair ahead of the second copy and the row order (9 starts, about 2000 forms): best 113 (named sine and
// cosine locals per rotation, axis alias, dot (i + k) + j, cross k then j as scalars, two pairs in the helper, one in
// the second axis, XY ahead of the second copy, rows j, k, i); other starts end at 115 to 161. The planar XY pair moved
// into a yaw helper with the sibling's rotation: frame 0x24 (the cross vector then shares the second copy's home and
// XY the scalar cells). No form matches; nothing kept. Left: the two rotations' schedule (retail loads the vector's k
// first, sums the dot as (i + k) + j, keeps cross j and k in registers in the first rotation and spills cross i into
// the cosine's home) and the yaw clamp's zero copy above its branch.
// M3 lane 11 r134 (fast with 0x29b990, 0x11cc90, 0x11ce20 and 0x1e5450, all MATCH; control 151, aligned 178, masked 104):
// the first rotation through a file-local copy of the in-place rotation with its dot summed i, k, j (retail's first-rotation
// order) 151 (same code: the three-term sum is reordered by the compiler); the yaw clamp's lower limit as `0.0f - yaw_limit`
// 151 (same code); sine and cosine of the first rotation named before the copy 153 (aligned 162), after the copy 153; the
// second rotation's axis through the existing cross3f (forward, up) 171 (aligned 158, masked 77); cross3f with the first
// rotation's sine and cosine named (before or after the copy) 141 (1707 B, aligned 123, masked 85: sine and cosine take
// retail's +0x10/+0x3c homes), with the second rotation's named as well 141; not kept: the one-use call-result locals are
// needed for the gain (lead saved as build/r134/lead-0x29b2e0.patch). Diagnostic: 1..8 dead initialised locals at the top
// 151/151/214/214/214/214/214/170 (the first rotation's dot stays j, i, k at every count; retail's is i, k, j).
// Sized copies at second 3D, XY, both, first 3D, both 3D, XY with first 3D, all three and all with
// the degenerate restore: 256/378/378/256/256/378/378/378. They grow the frame and do not recover the ordered stores.
// M3 lane 11 r137 (fast with 0x29b990, 0x11cc90, 0x11ce20, 0x1e5450, all MATCH; control 151, aligned 178): both rotations
// through a file-local helper taking the angle (sine and cosine named inside it, two callers) with the second axis
// through the existing cross3f 169 (1706 B, aligned 148: the roots move to +0x10 and the first rotation's dot stays
// j, i, k); that helper with the cross written out 204; with the cosine taken first 177; with cross3f_staged 169 (same
// code); with cross3f reading the up vector straight from the object (no `local_17` pointer) 156 (1706 B, aligned 144).
// Re-measured: round 134's lead (named first-rotation sine and cosine with cross3f) 141 at 1707 B, aligned 123.
// Paired coordinate stores, projection or normalized copy first, 389/389; chained shared reads 420/398.
// Depth-only fallback 321, with paired stores 392/392 or chained reads 419.
// NOT kept: the reordered copies diverge before the original store pair. The depth-only fallback
// changes the three-coordinate retail restoration and raises the score.
// Coordinate-array copies of the second 3D input, XY, or both 400/346/386;
// first 3D alone, with second 3D, with XY, or with both 429/403/418/1095;
// all with array-based fallback restore 1115. NOT kept: all regress and disturb the
// matching entry or first normalization. The XY and depth store order still differs.
// M3 lane 3 r284 (h2m fast screen with 0x29b990, 0x11ce20, 0x11cc90; [n] = aligned rows, control [116]): the XY vector
// declared ahead of the second copy 149 [115] (plain fast check agrees; first difference +0x179: its y store now precedes
// the copy's k store as retail's; a declaration move worth 2, not kept); the four clamps' lower bounds as `0.0f - limit`
// 151 (same code); named sine and cosine locals for both rotations ahead of the facing copy 224 [171] (1693 B), after it
// 224; the turn tests as `!(x * delta < 0.0f)` 151 (same code); the facing copy ahead of the pitch clamp (with the XY
// vector first) 424 (0x29b990 48). Retail stores the first rotation's sine at +0x10 and its cosine in the dead
// first-parameter home (+0x3c), ours the reverse (the second rotation agrees); the yaw clamp's zero copy (`movaps xmm5,
// xmm1`) sits above retail's `ja` and below ours.
// Second projection as a zero-depth aggregate or through the component setter 389/372, or from the XY
// workspace 420. Alternate shared normalizer for the second projection or both 289/393. Shared approach
// operation for yaw, pitch or both 298/280/291. All regress. The exchanged depth and XY stores, yaw zero
// copy and the first rotation's product order and spill homes remain.
// M3 lane 11 r142 (fast with 0x29b990/0x170d70, plain confirmed with 0x29ab90/0x29bab0/0x154df0): kept 151 -> 130 (1707 B):
// both rotations through the existing rotate_vector_170d70 (inline in src/unknown_170d70.cpp, called from other files as
// 0x154df0 does), which reads the vector's k at each use: retail's first rotation reads that k from memory late
// (`mulss xmm6, [esp + 0x28]`), which include/axis_rotation.h's helper, holding k in a local, cannot give. Measured on the
// way: include/axis_rotation.h without its k local 130 (but 0x2551f0 and 0x1ff360 lose their matches and 0x117dd0 goes
// 130 -> 147), with the k local used only by the k row 130 (0x117dd0 147), with it read after the projection 214 (0x117dd0
// 117), the dot order i, k, j or the cross order j, k, i alone 151 (same code); named sine/cosine with either name order
// 153/153 (names do not order those homes); cross3f with no `local_17`, on the old call form 171, with named sine/cosine
// 141. On the kept form: only the first rotation through the helper 130 (same code); the XY vector before the second copy
// 128 (two points, not kept); named sine/cosine 164; the axis through cross3f with no `local_17` 222 (1715 B); both 173.
// Round 454: the 0x2c frame agrees, with the XY J and depth stores inverted at +0x101. Native XY aggregate 346;
// XY from full input copy 303; vector setter for second, first or both input copies 413/418/426; second setter
// plus XY aggregate 393; setter for degenerate restore 271; setter for second copy and restore 398. Body restored.
// M3 lane 7 r207 (h2m fast with 0x170d70 and 0x29b990, both MATCH; control 130): both rotations through a private copy
// of matched 0x194a10's written-out rotation (its rows and cross order are retail's here) 130 (same code: the text is
// canonicalised); the turned facing as its own function-scope vector 227 (frame grows); declared in the arm with the
// angle stores and the output copy written in each arm 253; the first planar normalization through the file's planar
// normalizer (no direct depth clear) 312, both normalizations through it 264 (1706 B); the second copy written
// member-wise j, i, k 410. A compile-only screen of the rotation region (helper pasted in; control 93 rows off retail,
// the real build 86) ranked all six member-wise copy orders at 120-123 and the normalizer forms at 106-113.
// M3 lane 11 r144 (h2m fast with 0x29b990, 0x170d70, 0x154df0; control 130, aligned 162): the yaw scale as a conditional
// assignment (`yaw_limit = 0.0f > x ? yaw_limit : yaw_limit * arg_4`) 130, for both angles 130 (same code); as a multiply by
// a selected factor (`yaw_limit *= 0.0f > x ? 1.0f : arg_4`) 234 (1726 B); both scales through a file-local inline taking
// the limit, the product and the factor 248 (frame 0x28); both clamps through a file-local inline pin of (value, limit)
// 130 (same code); all 64 operand orders of the second axis's six products 130 each (same code: the written order is
// ignored). Diagnostics (reverted): 1-7 dead inline-helper locals at the top, before the gate, before the first
// rotation or before the second axis give two states only, 222 (frame 0x28) for 1-3 and 130 for 4-7, and 1-7 dead
// function locals 222/219/219/126/126/126/126; in every state the second axis keeps its six products
// (uj*fk, ui*fk, uj*fi, fj*uk, fi*uk, ui*fj as loaded; retail uj*fk, uk*fj, fi*uk, fk*ui, fi*uj, fj*ui), so this body's
// product sides follow neither the spelling nor the symbol phase that moves 0xef870's. A lower-bound local started at
// zero ahead of the yaw scale and reduced after it (diagnostic for retail's zero copy above the branch) 295.
// Full fallback value selected before validation, on the valid-angle path, after yaw, or before
// current pitch 379/386/386/391. Fallback address selected at those sites 364/256/291/367.
// NOT kept: the original J/depth pair remains or the first difference moves earlier.
// M3 lane 6 r225 (H2M_FAST with 0x170d70, 0x154df0 and 0x29b990; control 130, difflib-aligned 162, register- and
// offset-masked 80, all of it the two rotations' schedule plus the +0x101 store pair and the yaw zero copy). The shared
// rotate_vector_170d70's text, measured as a diagnostic and restored (0x170d70 and contributor 0x154df0 use it too): the dot
// parenthesised as retail's (i + k) + j 127 (0x170d70 48, 0x154df0 28), as (k + i) + j 127 (0x170d70 48), (i + j) + k 128
// (0x170d70 111), i + (j + k) 216 (0x170d70 7); the j and k rows with the projection term first 130 (same code in all
// three); the rows parenthesised as (v * c + p * a) - x * s or (p * a + v * c) - x * s 214 (0x170d70 kept, 0x154df0 17),
// the i row alone 214, the j and k rows alone 130 (0x170d70 106), v * c + (p * a - x * s) 171 (0x170d70 106), p * a + (v * c
// - x * s) 168 (0x170d70 79): no text keeps 0x170d70 and lowers this body (explicit parentheses are reassociation barriers;
// unparenthesised rows and sums are reordered by the compiler). In this body: both clamps' lower bounds as named locals
// (`real yaw_lower = -yaw_limit;`) 292 (1706 B, frame 0x28); the first rotation through include/axis_rotation.h with the
// second through rotate_vector_170d70 222 (1695 B); the second copy declared ahead of the first 130 (same code); the
// existing rotate_direction (src/unknown_1eb8a0.cpp) screened with 0x1ecf50 listed, so that its file is in the reduced
// image (round 84's 817 had it out of line): both rotations 222 (1695 B), the first only 222, the second only 130 (same code).
// M3 lane 7 r210 (h2m fast with 0x29b990 and 0x170d70, both MATCH; control 130): the second axis as one aggregate
// initializer 130 (same code); the up-vector pointer declared at the top after the object pointer 239 (1710 B); the second
// copy's depth clear written ahead of the XY vector 130 (same code); a const copy of the object's forward vector for the six
// cross products 203 (1726 B); the actor's current yaw and pitch read once into const locals 267 (1685 B). Body unchanged.
// Full fallback value selected before validation, on the valid-angle path, after yaw, or before
// current pitch 379/386/386/391. Fallback address selected at those sites 364/256/291/367.
// NOT kept: the original J/depth pair remains or the first difference moves earlier.
// shared coordinates with full aggregate first 407; shared coordinates with planar aggregate first 407;
// scoped shared coordinates with full copy first 407; scoped shared coordinates with planar copy first
// 407; full aggregate from coordinate array 406; both aggregates from coordinate array 406; full copy
// with named planar assignments 130; separate depth publication after the planar pair 406.
// NOT kept: Explicit planar member assignments reproduce 130. Sharing the two input coordinates changes
// the first normalizer, and the full aggregate forms alter the 0x2c frame. The projected J and depth
// stores remain exchanged in the tying form.
// The planar J and depth stores still exchange order. One immutable input for both projections scored 394,
// with degenerate restore 394, pitch angle 383, rotation input 384, restore and pitch 383, restore and
// rotation 384, pitch and rotation 375, and all consumers 375. NOT kept because the frame grew to 0x38 or 0x3c.
// M3 lane 1 r155 (H2M_FAST with 0x29b990 and 0x170d70, both MATCH; control 130): the second axis vector declared at
// function scope 130, declared ahead of the facing copy 130 (same code each); the global up axis read into a pointer local
// ahead of the facing copy 151 (1709 B); the second rotation's sine and cosine as named locals 228 (1691 B); the first
// rotation's sine and cosine as named locals ahead of the facing copy 164. Reading: retail gives both rotations' sines one
// home (+0x10) and spills the first rotation's turned j into the facing's j home (+0x24), keeping the second axis in
// registers; ours gives the second sine +0x14 and spills the second axis's k into +0x34.
// M3 lane 3 r299 (H2M_FAST with 0x29b990 and 0x170d70, both MATCH; control 130, difflib-aligned 162). Reading of the
// +0x101 pair: stores wait until their register is needed again and are then written in source order, so retail's planar
// j store ahead of the copied depth says the planar pair is declared ahead of the second full copy: `vector2f local_6`
// ahead of `vector3f local_5` 128 (first difference then +0x179, aligned 160; a moved declaration two below the control,
// not kept). The depth clear ahead of the planar pair 130 (same code); the pair by member stores 130 (same code); the
// pair as a planar view of the argument 346; the pair read from the copy 303; the second normalisation through the
// file's own planar normaliser 271 (1724 B). Compare operands named ahead of their tests: the second length 383 (1709
// B), the yaw product 295, the pitch product 222 (1695 B), both products 234, the yaw scale field 229 (1695 B); the gate
// as nested tests with the fallback in both arms 321 (1714 B). Each moves the frame off 0x2c. Diagnostics (reverted):
// the sign tests through a bool-returning forced-inline (yaw, pitch, both, the two depth signs, all four) 309/225/212/
// 212/212; the second axis's product sides change with each, never to retail's six. Retail copies the zero for the yaw
// lower bound ahead of the scale's branch (0x29b64a) and ours behind it.
// M3 lane 8 r225 (h2m fast with 0x29b990 and 0x170d70, both MATCH; control 130): compare sides with the zero on the
// other side for the field_38, arg_3->k, arg_2->k, field_30 and both turn tests 130 each (same code), for either
// field_34 test 214 (1708 B, first +0x2); `0.0f == normalize3d(&local_5)` 131; the second axis computed ahead of the
// first rotation or between the facing copy and it 202 (1737 B); the object's up as `&local_1->forward + 1` 130 (same
// code); a forward pointer for the six cross products 151 (1696 B); the clamp bounds as `0.0f - limit` for the two
// turn limits or all four 130 (same code).
// M3 lane 15 r135 (the coordinator's rotation-family screen, h2m fast): both rotate_vector_170d70 calls through include/axis_rotation.h's helper 151 (1696 B). Restored.
// M3 lane 5 r237 (H2M_FAST with 0x29b990 and 0x170d70, both MATCH; control 130): both angles started at zero in their
// declarations with the else arm's two zero stores dropped 329 (1716 B), the pitch alone 308 (1710 B); descriptive names
// for the angles, the copies, the planar pair and the side axis in four sets 130 each (same code: these homes do not
// follow names). Body unchanged.
// M3 lane 15 r141 (fast screen with 0x29b990 and 0x170d70, both MATCH each time; control 130; aligned = rows off retail
// after alignment, control 140, of which 33 are operand-order rows). Reading of the operand orders: each product's and
// sum's operands are sorted by a rank, higher first. A local or temporary ranks by its creation number; a float read
// through a pointer global at offset 0 ranks by that global's position among the globals the body names (the ones the
// body names itself first, then those its inline lookups bring in; three low bits); a read at an offset ranks by a
// two-bit phase that parenthesised groups and the clamp macro ahead of it advance. Retail's first rotation has axis i
// and k above the vector's components and axis j below them; this body has axis i below (the up-axis global is the
// first global it names, the two data arrays arrive through the inline lookups) and j, k above. The actor and object
// lookups written as direct array references (the up axis then third) put axis i on retail's side in all four of its
// products: both 131 (1711 B, aligned 124, 25 order rows), actor only 131 (1708 B), object only 131 (1708 B); each
// with the planar pair declared ahead of the second copy 129. On both direct + planar pair first: sine and cosine
// named ahead of the facing copy 128 (aligned 122), cosine named first 126, named after the copy 128; the up axis in a
// pointer local 126; one sine/cosine pair assigned for each rotation 165 (1718 B, aligned 137, 14 order rows); the
// pitch clamp and its add in one statement 303, both clamps so 293; the pitch lower bound as `0.0f - limit` 129 (same
// code); the second rotation's sine and cosine named 224. Left on that form: axis j and k sit one phase too high (j
// must be the lowest), and no caller-side form moved them without moving the clamp fields: a parenthesis pair around
// the first call's cosine argument raises both one phase (172), around its sine argument 210 (phases unchanged, the
// frame shrinks). Diagnostics
// (reverted): parenthesis pairs around the pitch difference x around the second axis's first product, 0-3 each, with
// and without the planar pair first, 128-311 (0/0 with the pair first 128, 0/3 155, 1/3 208, 2/2 210, the rest 225 to
// 311); 0-7 unused locals at the top x 0-3 pairs on the pitch difference with the pair first: 128/308/309/237,
// 222/299/232/256, 219/296/232/256 (2 and 3 locals), 124/204/299/237 (4 to 7 locals); no match. Body unchanged.
// The planar J and full-depth stores still exchange order. Fixed-size copies for the full snapshot 130,
// planar snapshot 346, failed restore 130, both snapshots 346, full snapshot and restore 130, both full
// inputs 130, full snapshot plus restore and turned input 130, and all four facing transfers 346. The planar
// copy moves the first difference earlier.
// M3 lane 13 r153 (h2m fast with 0x29b990 and 0x170d70, both MATCH each run; control 130, aligned 162). Rank levels of the
// first rotation here: axis i 0, j 4, k 2 against the facing copy's three members just above level 0, so retail (axis i and k
// above the members, j below) needs i off level 0 and j and k four levels on. The degree factor written as a parenthesised
// constant quotient at its three sites 130 (same code: a folded constant in parentheses does not advance the levels); the
// yaw clamp's lower bound in a named local 295 (retail's 1706 B, but the frame is 0x28; the levels of the rotation do not
// move, one pitch field read moves from level 0 to 6); both turn clamps' lower bounds named 292 (1706 B, frame 0x28); the
// shared lower bound of the first two clamps named 295 (1706 B, frame 0x28); the turn limit field read once into a local
// for both limits 291 (1706 B, frame 0x28); the shared limit computed ahead of the pitch difference 167. Body unchanged.
// The planar J and full-depth stores still exchange order. Fixed-size copies for the full snapshot 130,
// planar snapshot 346, failed restore 130, both snapshots 346, full snapshot and restore 130, both full
// inputs 130, full snapshot plus restore and turned input 130, and all four facing transfers 346. The planar
// copy moves the first difference earlier.
// Round 479: Planar J and copied depth still exchange stores. Declare the planar pair first with
// late member writes 130; planar i early 130; planar j early 137. Interleaved full and planar member
// stores in four orders 406 each; full component initializer with the independent planar pair 406.
// The full component forms shrink the frame and length. Body restored.
// Four-byte planar transfers of I, J, both I-first or both J-first 269/314/335/335.
// I copied before the full snapshot and J assigned afterward 272; I assigned before
// and J copied afterward 314; both copied first 345; J copied before and I afterward 345.
// All move the first difference earlier, even at retail extent.
// Round 469: whole-value conditional at the pitch calculation 379. Immediate branch
// choosing the pitch comparison input 365, and the branch at the angle use 368. Five
// input and projection name orders tie 130. The projected J/depth stores remain exchanged.
// Planar pair before the full copy, with branch/conditional base rate, in-place bounds or
// named lower bound on yaw only: 220/220/237/295. The same rates and bounds on both angles:
// 208/208/299/292. NOT kept: the better forms fix the store pair but grow the body, and
// the separate lower bounds reach retail length with a smaller frame.
// Dot-first in-place rotation for yaw/pitch/both 222/130/222. On planar-first
// initialization the same forms score 222/128/222. Both dot-first rotations with
// the shared direct or staged side-axis cross score 222 each.
// NOT kept: the pitch-only rotation gives the same code with either prefix. The
// planar-first two-point lead fixes the store pair, below the keeper threshold.
// Planar pair before the full copy, with branch/conditional base rate, in-place bounds or
// named lower bound on yaw only: 220/220/237/295. The same rates and bounds on both angles:
// 208/208/299/292. NOT kept: the better forms fix the store pair but grow the body, and
// the separate lower bounds reach retail length with a smaller frame.
// Planar-first yaw/both. Selected or assigned final step 149/219 each. Selected time factor
// 246/255. Named directional test 295/232. Early zero bound 295/292. NOT kept: the 149 forms
// recover the store pair but shorten to 1696 bytes and regress. The 0x2c frame and opening body remain.
// Planar-first plus memcpy at planar/spatial/fallback transfers 356/128/128;
// planar with spatial or fallback 356/356; spatial with fallback 128; all three 356;
// all three through memmove 402. NOT kept: 128 only gains two; planar copy changes the
// first normalization. The opening J/depth store order and later rotation differences remain.
// Planar-first with bounded-add on yaw/pitch/both 240/247/299; with one current-angle
// read 208/230/304; with a distinct bounded result for yaw/pitch 200/222.
// NOT kept: the single-axis forms recover the store pair but all regress. The yaw zero
// placement and later rotation schedule remain.
// With planar first, selected target, selected current, two selected pitches, or selected current
// in the difference: 296/237/251/251. Both angles before signs with selection or updates 312/148.
// Assign signed target first and subtract selected or updated current 311/312.
// NOT kept: all regress, including the 148 form with retail length and the J/depth store pair.
// M3 lane 12 r189 (reduced-image screen with 0x29b990, 0x170d70, 0x154df0, 0x11cc90, 0x11ce20 and 0x1e5450; control 130,
// 33 rows that differ in meaning): 1/2/3 redundant pairs around the actor lookup's masked index 130 each (same code:
// integer parentheses add no expression node, float ones do); the actor and object lookups written out as direct array
// references 131 (25 meaning rows; the axis i moves from level 0 to 2), with 1/2/3 pairs around the masked index 131
// each (same code); 1/2/3 pairs around the first rotation's cosine argument 231/226/158 (the first two grow the frame by
// 4), the same on the direct lookups 236/228/140 (at 3 pairs 17 meaning rows are left: the first rotation's dot starts
// as retail's, its accumulation order and cross terms do not). Body unchanged.
// For the object side axis, staging the positive terms, the subtracted terms,
// both sets, or accumulating in the destination scores 202/202/149/220
// with the original projection and 202/202/147/220 with planar first.
// NOT kept: both-set staging with planar first fixes the projected J/depth
// stores, then shortens to 1696 bytes and raises the score.
// M3 lane 3 r311 (fast screen with 0x29b990, 0x170d70, 0x154df0 (13), 0x11cc90, 0x11ce20 and 0x1e5450; control 130). Phase
// diagnostic (removed): 1-7 unused initialised locals at the top give 222/219/219 for 1-3 and 126 for 4-7 (1695 B at 1-3),
// and 1-3 redundant pairs around the yaw test's length or the pitch's stored-angle load move nothing, so no symbol or
// node phase cell holds retail's rotation operand order. Forms: the planar copy taken from the planar vector (`{ local_5.i,
// local_5.j }`) 303 (frame changes, first +0x2); the planar k cleared before the planar copy 130 (same code); both limits
// as ternary initialisers like the half-range 210 (1711 B); the half-range as a default plus `if` 236; the two sign tests
// as `0.0f > k` 130 (same code); the yaw scale as `local_8 = field_38 * local_8` 130 (same code). Body unchanged.
// M3 lane 16 r193 (H2M_FAST with 0x29b990, 0x170d70, 0x11cc90, 0x11ce20 and 0x1e5450, all MATCH; control 130, aligned
// 162, masked 80): the planar copy as `*(vector2f const *)arg_2` 346 (first difference +0x60); the planar copy declared
// ahead of the 3-D copy 128 (retail's store order at +0x101, first difference now +0x179: the yaw clamp's `movaps` and the
// first rotation, whose dot retail sums as (i + k) + j and ours as (k + j) + i, the copy's loads j, i, k against ours j,
// k, i); the yaw sine and cosine named ahead of the first rotation 164; the planar members assigned as two statements 130
// (same code); the planar i taken from the 3-D copy 292.
// @retail 0x29b2e0
void function_29b2e0(long arg_0, long arg_1, vector3f const *arg_2,
    vector3f const *arg_3, real arg_4, vector3f *arg_5)
{
    s_actor_view *local_0 = actor_get(arg_0);
    s_slot_object_view *local_1 = object_get(arg_1);
    s_29b2e0 const *local_2 = (s_29b2e0 const *)function_1e5450(arg_0, *(long *)local_1);
    vector3f local_3 = *arg_3;
    local_3.k = 0.0f;
    real local_4 = normalize3d(&local_3);
    vector3f local_5 = *arg_2;
    vector2f local_6 = { arg_2->i, arg_2->j };
    local_5.k = 0.0f;
    if (normalize3d(&local_5) == 0.0f) local_5 = *arg_2;
    real local_8;
    real local_9;
    if (local_4 > 0.0f && normalize2d_fast(&local_6) != 0.0f)
    {
        local_8 = function_11cc90(&local_6, (vector2f const *)&local_3);
        if (local_2->field_38 > 0.0f) local_8 *= local_2->field_38;
        real local_11 = function_11ce20(&local_3, arg_3);
        if (arg_3->k < 0.0f) local_11 = -local_11;
        real local_12 = function_11ce20(arg_2, &local_5);
        if (arg_2->k < 0.0f) local_12 = -local_12;
        local_9 = local_11 - local_12;
        real local_13 = local_2->field_30 > 0.0f ? local_2->field_30 * 0.01745329238474369f : 1.5707963705062866f;
        local_8 = PIN_126660(local_8, -local_13, local_13);
        local_9 = PIN_126660(local_9, -local_13, local_13);
        local_8 -= *(real *)((byte *)local_0 + 0x658);
        real yaw_limit = 6.2831854820251465f;
        if (local_2->field_34 > 0.0f) yaw_limit = local_2->field_34 * 0.01745329238474369f;
        if (!(0.0f > *(real *)((byte *)local_0 + 0x658) * local_8)) yaw_limit *= arg_4;
        local_8 = PIN_126660(local_8, -yaw_limit, yaw_limit);
        local_8 += *(real *)((byte *)local_0 + 0x658);
        local_9 -= *(real *)((byte *)local_0 + 0x660);
        real pitch_limit = 6.2831854820251465f;
        if (local_2->field_34 > 0.0f) pitch_limit = local_2->field_34 * 0.01745329238474369f;
        if (!(0.0f > *(real *)((byte *)local_0 + 0x660) * local_9)) pitch_limit *= arg_4;
        local_9 = PIN_126660(local_9, -pitch_limit, pitch_limit);
        local_9 += *(real *)((byte *)local_0 + 0x660);
        /* the turned facing is built in the first copy: retail keeps it in that copy's home and has two vector homes */
        local_3 = *arg_2;
        rotate_vector_170d70(&local_3, g_4687b0, (real)sin(local_8), (real)cos(local_8));
        vector3f const *local_17 = (vector3f const *)((byte *)local_1 + 0x7c);
        vector3f local_18;
        local_18.i = local_17->k * local_1->forward.j - local_17->j * local_1->forward.k;
        local_18.j = local_1->forward.k * local_17->i - local_1->forward.i * local_17->k;
        local_18.k = local_1->forward.i * local_17->j - local_1->forward.j * local_17->i;
        rotate_vector_170d70(&local_3, &local_18, (real)sin(local_9), (real)cos(local_9));
    }
    else
    {
        local_3 = *(vector3f *)((byte *)local_0 + 0x290);
        local_8 = 0.0f;
        local_9 = 0.0f;
    }
    *(real *)((byte *)local_0 + 0x65c) = local_8;
    *(real *)((byte *)local_0 + 0x664) = local_9;
    *arg_5 = local_3;
}

// @retail 0x29b990
void function_29b990(long arg_0, long arg_1, vector3f const *arg_2,
    vector3f const *arg_3, vector3f *arg_4)
{
    s_actor_view *local_0 = actor_get(arg_0);
    vector3f local_1;
    if (*(bool *)((byte *)local_0 + 0x4d5))
        local_1 = *(vector3f *)((byte *)local_0 + 0x4d8);
    else if (*(bool *)((byte *)local_0 + 0x5d1))
        local_1 = *(vector3f *)((byte *)local_0 + 0x5f8);
    else if (arg_3)
        local_1 = *arg_3;
    else
        local_1 = *(vector3f *)((byte *)local_0 + 0x290);
    if (function_29b32b(&local_1) == 0.0f)
        local_1 = *(vector3f *)((byte *)local_0 + 0x290);
    function_29b2e0(arg_0, arg_1, arg_2, &local_1, 1.0f, arg_4);
}
