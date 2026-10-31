#include "unknown_11c920.h"
#include "ai_actor.h"
#include "unknown_26c380.h"

// @flags /O2 /arch:SSE /Gr

real function_30bf0(vector3f *arg_0);
bool function_1f5110(long arg_0, point2f const *arg_1, real arg_2,
    real arg_3, bool *arg_4, s_path_trace_result *arg_5);

#pragma inline_depth(0)
// score 104 (was 104): frame 0x3c agrees; half/offset XMM allocation and x87/SSE projection scheduling remain.
// Earlier (104, was 104): the frame 0x3c still agrees; the half and offset XMM allocation
// and the x87/SSE projection scheduling are unchanged, and that round's trials in
// this file found no lower form of them
// Earlier (104): the frame 0x3c agrees; the half and offset XMM allocation and
// the x87/SSE projection scheduling remain, and that round's trials in this file
// found no lower form of them
// Earlier: frame 0x3c agrees; half/offset XMM allocation and x87/SSE projection scheduling differ;
// tried position base 105; origin base 105; displacement base 105; geometry bases 105; unsigned component 104;
// degenerate normal first 104; squared magnitude value 104; rejected direction loop 104.
// Scale/add operand orders 104 each; split squared magnitude 105; inner += 117; earlier magnitude declaration 104;
// component counter scope 104; mode stores before scaled output 104. Original body restored.
// the rotated vector's XMM allocation differs - retail
// keeps the two dot products in xmm1 and xmm0 where ours uses xmm5 and xmm3 -
// and the second subtraction and the x87 sum are swapped.
// Earlier: the negated component in its own local keeps 0.0 in xmm7 for the
// > 0.0f test as retail does, but 177 (aligned 63 against 64; all six term orders of the
// projection give the same code); that with the components written out 188; components
// written out k first 193; the flag store before the copy 104; the copy after the loop 104.
// Round 196: uniform original-direction reads 163; retained scalar inputs 197; double square 105;
// live/scalar inputs with double square 164/197; normalized-vector setter on original/live/scalar inputs
// 217/255/243. All restored; half/offset allocation and x87/SSE schedule remain.
// Round 412: complete normalization scale loop/shared scalar scale/direct division/shared double scale 105/177/105/184; with nonpositive projection first 105/184/105/184. Original body and precision restored.
// Round 288: shared affine/difference producers with in-place/distinct output, ordinary/forced
// difference declaration and original/earlier magnitude 262, 251, 262, 251, 262, 234, 262, 234. Original restored.
// Round 194: Advancing position/scale-input/origin component pointers and complete subsets 105/105/104/105/105/104/105/105; complete second-projection differences/products/both 105/105/104; per-component named half-step 105. All preserve the original grouping but give no gain; original body and inherited inline boundary restored.
// Round 9 (lane 2, fast): the existing prop-level getter 0x2684f0 for the opening test 230 (979 bytes; the
// inline-depth window keeps it out of line), without the window 320 (1072 bytes; the body alone without it
// 383); unary minus for both negations 176 (986 bytes); the half step as `0.5f * v + position` 104; the
// length's squares in i, j, k order 108; the projection straight into its own local (no store to the
// vector first) 104.
// Round 126: without this file's inline_depth(0) pair LTCG inlines 0x25d700 and 0x1e4290 (retail calls
// both): 383 (1119 bytes); and with the header normalize3d for the first normalization 424 (1152), with
// dot3f for the final test 424, with `-x` for `0.0f - x` 424; normalize3d with the pragma kept (then
// called, not inlined) 273 (927 bytes).
// Round 46 (lane 9): local_6 and the final test's dot are each one term rotated against retail ((y, x, z)
// and (k, i, j) where retail sums (z, y, x) and (j, k, i)), the function-wide reassociation order; retail also
// keeps one SSE zero (xmm7) for `0 - j`, both compares and the k store, loads -1.0 at each use, and reads
// unknown290 only after the unknown424 copy. Tried: local_5 computed directly with `0 - j` written twice 104
// (identical); a named negated component used twice with local_5 direct 177 (989 bytes; one xorps zero as
// retail), with the terms k, j, i 177; the reciprocal written at each use 105; fabs on the float 219 (1020
// bytes); the unknown456 store after the offset loop 108; a short component counter 247. Under
// inline_depth(0) even a __forceinline helper is called, so no header helper can take part.
// Diagnostic lead (not kept): `/Ob1` in place of the pragma pair still inlines 0x25d700 and 0x1e4290, 383
// (1119 bytes), with normalize3d and dot3f 424 (1152).
// Round 53: the short-vector arm first (`if (fabs < eps) local_4 = 0; else ...`) 104; `local_8 * k` 104;
// the dot with the negated component first in each product 104; `0.5f * v` 104; the unknown456 store just
// before the magnitude 108. Harness: all but the last compile byte-identically.
// Round 40 (lane 11, fast): the first difference (+0x6f) is register numbering: retail gives the half
// constant xmm3 and the three offset components xmm1/xmm2/xmm0, kept live into the projection, and the
// length xmm7; ours 0.5 in xmm0, the components in xmm5/xmm2/xmm3, the length in xmm1. local_3 declared
// before local_2 / at the top 104/104; the trace result, local_5 with local_6, and local_8 declared at the
// top 104/104/104 (all the same code).
// Round 251: Native direction input/selection/output memcpy, seven subsets, 104 each; complete memmove 233.
// Original body and include restored; half/offset XMM allocation and x87/SSE projection scheduling remain.
// Round 33 (lane 14, fast; aligned counts from a difflib alignment, original 86): the final test accumulated
// j, k, i in a named local 105 (aligned 84; retail's dot instructions, still scheduled above the unknown424
// copy); half-step components written out 192; negated j named and used for the projection and the store
// 177 (aligned 85); the projection accumulated k, j, i (`local_5 = k-term; += j-term; += i-term`) 97
// (aligned 87: z/y/magnitude/j/k registers and the xmm7 zero become retail's, half/x and inverse/i stay
// swapped); that with the negated j named first 176 (989 bytes, aligned 80), plus the side projection
// accumulated z, y, x 176 (aligned 93); the k, j, i projection with the j, k, i final test 98. Retail does not
// hoist the -1.0 load (else branch and memory operand), so it copies the projection into xmm6.
// @retail 0x1a9760
void __stdcall function_1a9760(long arg_0, s_slot *arg_1)
{
    s_actor_view *local_0 = (s_actor_view *)(g_4f55f0->data + (arg_0 & 0xffff) * sizeof(s_actor_view));
    if (local_0->prop_index != NONE)
    {
        short *local_1 = (short *)function_25d700(local_0->prop_index);
        if (local_1 && *local_1 >= 6)
            local_0->unknown488 = true;
    }
    vector3f local_2 = local_0->unknown37c;
    vector3f local_3;
    local_0->unknown456 = true;
    for (long component = 0; component < 3; ++component)
        local_3.n[component] = (local_0->position.n[component] + local_0->unknown258.n[component] * 0.5f) - local_0->unknown370.n[component];
    real local_4 = (real)sqrt((double)(local_2.j * local_2.j + (local_2.i * local_2.i + local_2.k * local_2.k)));
    if (!((real)fabs((double)local_4) < 0.0001f))
    {
        real local_8 = 1.0f / local_4;
        local_2.i = local_8 * local_2.i;
        local_2.j = local_8 * local_2.j;
        local_2.k = local_2.k * local_8;
    }
    else
        local_4 = 0.0f;
    if (local_4 > 1.5f)
    {
        local_3.i = local_3.i * (0.0f - local_2.j) + local_3.j * local_2.i + local_3.k * local_2.k;
        real local_5 = local_3.i;
        local_3.i = 0.0f - local_2.j;
        local_3.j = local_2.i;
        local_3.k = local_2.k;
        if (local_5 > 0.0f)
            local_2 = local_3;
        else
        {
            local_2.i = local_3.i * -1.0f;
            local_2.j = local_3.j * -1.0f;
        }
        real local_6 = (local_0->position.z - local_0->unknown370.z) * local_3.k +
            (local_0->position.y - local_0->unknown370.y) * local_3.j +
            (local_0->position.x - local_0->unknown370.x) * local_3.i;
        if (local_6 * local_5 < 0.0f)
            local_6 *= -1.0f;
        local_2.k = 0.0f;
        if (function_30bf0(&local_2) == 0.0f)
            local_2 = *g_4687a8;
        local_6 += local_0->unknown36c;
        if (local_6 > 0.0f)
        {
            if (!(local_6 > 1.5f))
                local_6 = 1.5f;
        }
        else if (local_6 > -1.5f)
            local_6 = -1.5f;
        s_path_trace_result local_7;
        if (!function_1f5110(arg_0, (point2f const *)&local_2, local_6, 0.0f, NULL, &local_7))
        {
            local_2.i *= -1.0f;
            local_2.j *= -1.0f;
            local_2.k *= -1.0f;
        }
    }
    else
    {
        local_2 = local_3;
        if (function_30bf0(&local_2) == 0.0f)
            local_2 = *g_4687a8;
    }
    local_0->unknown458.i = local_2.i * 10.0f;
    local_0->unknown458.j = local_2.j * 10.0f;
    local_0->unknown458.k = local_2.k * 10.0f;
    local_0->unknown41c = 4;
    local_0->unknown420 = 4;
    local_0->unknown424.vector = local_2;
    if (local_0->unknown290.j * local_2.j + local_0->unknown290.k * local_2.k +
        local_0->unknown290.i * local_2.i > 0.8f)
        function_1e4290(arg_0, true);
}
#pragma inline_depth(255)
