#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_11cc90.h"
#include <math.h>
#include "unknown_124f90.h"
// @flags /O2 /arch:SSE /Gr

real function_11ce20(vector3f const *arg_0, vector3f const *arg_1);
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

PRIVATE __forceinline void function_29b6c0(vector3f const *arg_0,
    vector3f const *arg_1, real arg_2, vector3f *arg_3)
{
    real sine = (real)sin(arg_2);
    real cosine = (real)cos(arg_2);
    real projection = ((arg_1->i * arg_0->i + arg_1->k * arg_0->k) + arg_0->j * arg_1->j) * (1.0f - cosine);
    real cross_i = arg_0->j * arg_1->k - arg_0->k * arg_1->j;
    real cross_j = arg_0->k * arg_1->i - arg_0->i * arg_1->k;
    real cross_k = arg_0->i * arg_1->j - arg_0->j * arg_1->i;
    real i = arg_0->i * cosine + projection * arg_1->i - cross_i * sine;
    real j = arg_0->j * cosine + projection * arg_1->j - cross_j * sine;
    real k = arg_0->k * cosine + projection * arg_1->k - cross_k * sine;
    arg_3->i = i;
    arg_3->j = j;
    arg_3->k = k;
}

// score 176 (was 176): frame 0x2c agrees (1711/1706 bytes); unity XMM5 versus XMM4, root/acos +0x10 versus yaw +0xc, and rotation homes remain (r83 lead: single-exit normalizer + early XY vector 169).
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
// @retail 0x29b2e0
void function_29b2e0(long arg_0, long arg_1, vector3f const *arg_2,
    vector3f const *arg_3, real arg_4, vector3f *arg_5)
{
    s_actor_view *local_0 = actor_get(arg_0);
    s_slot_object_view *local_1 = object_get(arg_1);
    s_29b2e0 const *local_2 = (s_29b2e0 const *)function_1e5450(arg_0, *(long *)local_1);
    vector3f local_3 = *arg_3;
    real local_4;
    function_29b32b(&local_3, &local_4);
    vector3f local_5 = *arg_2;
    vector2f local_6 = { arg_2->i, arg_2->j };
    if (function_29b32b(&local_5) == 0.0f) local_5 = *arg_2;
    vector3f local_7;
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
        local_7 = *arg_2;
        function_29b6c0(&local_7, g_4687b0, local_8, &local_7);
        vector3f const *local_17 = (vector3f const *)((byte *)local_1 + 0x7c);
        vector3f local_18;
        local_18.i = local_17->k * local_1->forward.j - local_17->j * local_1->forward.k;
        local_18.j = local_1->forward.k * local_17->i - local_1->forward.i * local_17->k;
        local_18.k = local_1->forward.i * local_17->j - local_1->forward.j * local_17->i;
        function_29b6c0(&local_7, &local_18, local_9, &local_7);
    }
    else
    {
        local_7 = *(vector3f *)((byte *)local_0 + 0x290);
        local_8 = 0.0f;
        local_9 = 0.0f;
    }
    *(real *)((byte *)local_0 + 0x65c) = local_8;
    *(real *)((byte *)local_0 + 0x664) = local_9;
    *arg_5 = local_7;
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
