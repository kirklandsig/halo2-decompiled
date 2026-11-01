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
    real projection = (arg_1->i * arg_0->i + arg_1->k * arg_0->k + arg_0->j * arg_1->j) * (1.0f - cosine);
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

// score 244 (was 255): frame 0x2c agrees (1710 bytes against 1706); the sqrt temporaries sit at +0x10 with
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
        real yaw_limit = 6.2831854820251465f;
        if (local_2->field_34 > 0.0f) yaw_limit = local_2->field_34 * 0.01745329238474369f;
        real local_16 = local_8 - *(real *)((byte *)local_0 + 0x658);
        if (!(0.0f > *(real *)((byte *)local_0 + 0x658) * local_16)) yaw_limit *= arg_4;
        local_8 = *(real *)((byte *)local_0 + 0x658) + PIN_126660(local_16, -yaw_limit, yaw_limit);
        real pitch_limit = 6.2831854820251465f;
        if (local_2->field_34 > 0.0f) pitch_limit = local_2->field_34 * 0.01745329238474369f;
        local_16 = local_9 - *(real *)((byte *)local_0 + 0x660);
        if (!(0.0f > *(real *)((byte *)local_0 + 0x660) * local_16)) pitch_limit *= arg_4;
        local_9 = *(real *)((byte *)local_0 + 0x660) + PIN_126660(local_16, -pitch_limit, pitch_limit);
        vector3f rotation_input = *arg_2;
        function_29b6c0(&rotation_input, g_4687b0, local_8, &local_7);
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
