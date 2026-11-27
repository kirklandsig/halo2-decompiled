#include "unknown_11c920.h"
#include "ai_actor.h"
#include "unknown_26c380.h"

// @flags /O2 /arch:SSE /Gr

extern point2f *g_468778;
real normalize2d(point2f *arg_0);
bool function_1f5110(long arg_0, point2f const *arg_1, real arg_2,
    real arg_3, bool *arg_4, s_path_trace_result *arg_5);

PRIVATE __forceinline real function_1a83e1(point3f const *start, vector3f const *segment, point3f const *point)
{
    vector3f delta;
    real squared_distance;
    real length_squared = length_sq3f(segment);
    if (length_squared > 0.0001f)
    {
        vector3d_from_points3d(start, point, &delta);
        real t = (delta.k * segment->k + delta.j * segment->j + delta.i * segment->i) / length_squared;
        if (t < 0.0f)
            t = 0.0f;
        else if (t > 1.0f)
            t = 1.0f;
        t = -t;
        delta.i += segment->i * t;
        delta.j += segment->j * t;
        delta.k += segment->k * t;
        squared_distance = length_sq3f(&delta);
    }
    else
    {
        vector3d_from_points3d(point, start, &delta);
        squared_distance = length_sq3f(&delta);
    }
    return (real)sqrt(squared_distance);
}

static inline void point_offset_along_vector(point3f const *point, vector3f const *vector, real t, point3f *result)
{
    result->x = vector->i * t + point->x;
    result->y = vector->j * t + point->y;
    result->z = vector->k * t + point->z;
}

PRIVATE __forceinline real function_1a83e2(point2f *arg_0)
{
    real local_0 = (real)sqrt(arg_0->y * arg_0->y + arg_0->x * arg_0->x);
    if (!(fabs(local_0) < 0.0001f))
    {
        real local_1 = 1.0f / local_0;
        arg_0->x = local_1 * arg_0->x;
        arg_0->y = local_1 * arg_0->y;
        return local_0;
    }
    return 0.0f;
}

// score 78 (was 205): walked statement by statement against retail 0x1a83e0 (r726), no behaviour
// difference found: field offsets, the three constants (0.0001f, +-0.3f), every condition direction (retail's tests at
// 0x1a8491, 0x1a84c6, 0x1a8963, 0x1a897c), both f5110 calls (arguments, order, result slots), the answer cascade
// (0x1a8941-0x1a89bc) and the result stores (0x1a89c1-0x1a8a07) agree. Rounding agrees too: fst/fstp dword at
// 0x1a8481 (normalize: |len| is tested unrounded), 0x1a87bb and 0x1a891e (both sqrt results); the sums add k, j, i
// terms in retail's order. The remaining 205 is register allocation in the first inlined segment-distance block:
// retail loads the segment's k, j, i into xmm3, xmm4, xmm2 (0x1a8666-0x1a8678), ours xmm4, xmm3, xmm2, and the first
// block's sqrt argument is stored per branch (0x1a875e, 0x1a87ad) where ours stores once after the join; the second
// block (0x1a880a-0x1a8910) already matches apart from the shift this causes. function_1a83e2 and function_1a83e1 are
// two inlines local to this function, not copies of existing helpers: the term order differs from normalize2d (retail
// sums y*y then x*x at 0x1a8471-0x1a847d, 0x2b400 sums x*x then y*y) and from 0x11e000 (len^2 j, k, i at
// 0x11e00e-0x11e024 and dot i, k, j at 0x11e05a-0x11e070, here k, j, i at 0x1a8678-0x1a8691 and 0x1a86e2-0x1a86f8);
// retail calls 0x2b400 out of line right after (0x1a84fc, 0x1a8526). Calls to the nearest existing helpers score 422
// (normalize2d) and 920 (function_11e000, itself todo). normalize2d_fast (unknown_11cc90.h) compiles to 205 here but
// needs a new include, so it was not added. Tried (all fast): update order k, j, i (229, 1550 bytes); k, i, j,
// `t * seg` or `delta = seg * t + delta` forms, length_squared assigned inside the test (205); sqrt returned in each
// branch (273, 1549 bytes); plain `inline` (944); reversing the local_7 stores (95) or the delta loads (113) lowers
// the score but emits k first where retail emits i first (0x1a8559, 0x1a86ac), so those were dropped as score-only.
// Round 158 (m3 lane 14, fast = plain): the projected branch's delta through the existing
// vector3d_from_points3d(start, point, &delta) 78 (kept). Left: the segment's k and j loads are still xmm4/xmm3
// (a forced-ranking search that ranks the j load below k gives 72), the dot evaluates j, i, k with delta.k
// reloaded (retail k, j, i from registers), the else branch now evaluates i first (retail k) and the projected
// sum is still stored after the join. Also kept, the same code: length_sq3f for length_squared and both squared
// distances and vector3d_from_points3d(point, start, &delta) in the else branch (existing helpers for their bodies
// written out by hand; 78). Tried on top (fast): dot term orders k i j 94, i k j 84, i j k 205, j k i 212,
// j i k 213; length_squared term orders 78 each; dot3f(&delta, segment) or dot3f(segment, &delta) for the
// projection 205; else-branch delta orders k j i 236, i k j 82, j i k 82; update orders k i j 219, k j i 221, i k j 236;
// local_7 stores i, k, j 192; an else-branch offset vector of its own 236 (with or without the helpers); the
// segment itself through vector3d_from_points3d 237.
// Round 159 (m3 lane 14, fast): `(real)sqrt(function_11e000(point, start, segment))` at both sites 920 (1026 B, the
// helper stays a call in our link); the inline written in 0x11e000's statement shapes (v.k first, x/y/z scalars) 192;
// the dot spelled i, j, k 205 at retail's length but aligned much closer: the dot, the else branch (k first) and the
// second block's sums become retail's, and what is left is the first block's segment k/j registers, its update order
// (retail updates k first, in the segment-k register) and the per-branch store of the sqrt argument, one instruction
// short, which shifts every later row. On that form: update orders k i j 205, i k j 204; length-squared spellings,
// the existing vector3f_add_scaled for the update and the length squared written twice all give the same code (205).
// A forced-ranking search on it: the three segment reloads tie (i, j, k by creation); j one below k gives retail's
// segment registers (197, update order still differs), j above the rest 29.
// M3 lane 2 r155 (fast). Allocator reading on the i, j, k dot form: the three segment reloads are second-pass pieces
// with equal weight (110 each: the length block, the dot block and the update block), so creation order decides; the
// updated i and j of the first block are first-pass values (144 each) coloured before them, and the update block
// computes k last in the negated scale's own register where retail computes it first in the segment k register (the
// second block runs i, k, j in both builds). A forced-ranking search: the updated i dropped below 48 gives 25, below
// 110 gives 35; the segment j raised to 170 gives 29. Tried on that form (fast): the update through the existing
// vector3f_add_scaled into a separate vector 205, in place with `-t` as the scale 205, the squared distance as
// dot3f(&delta, &delta) 205 (same code each); the update as vector3f_scale into an offset then vector3f_add 243
// (1560 B). On this body: the update through vector3f_add_scaled in place 78 (same code); into a separate vector 222.
// GA23 (43 compiled fast trials, 0x2b400/0x1f5110/0x1a8c30 MATCH): 0x11e000's 0-diff match body (single-exit or two
// returns, dot3f, vector3f_add_scaled, 0.0f-t, ternary or if clamp, `(real)sqrt` outside, `(point, start, segment)`
// order) 205 (al 32, mk 19, same code as `a0`); on `a0`: `length_squared` jki / `+=` kji / `+=` jki / `+=` ijk / `+=` kij
// 205/205/201(al 26)/208/208, `squared_distance` `+=` kji 205, recomputed `length_sq3f` 205, scoped `delta` +
// `vector3f_add_scaled` 205, direct delta sub 205, `local_7` at top 205, nested `segment_dist_sq` (`__forceinline` 205,
// plain `inline` 944), `segment`-first dot kji/ijk 78/205; `dot` `+=` kji/ijk 205/225; `squared_distance` `+=` kji with
// update orders kij 71 (mk 13), ikj 64 (al 41, mk 13; + `length_squared` `+=` jki 60, al 35, mk 13; `t`-first 64/71/205;
// separate scalars 204/176/236; `sq` ijk 240/225; NOT kept: retail length_squared evaluates k, j, i and seg.k/seg.j stay
// xmm4/xmm3); on HEAD: `length_squared` `+=` kji or jki 78. Body unchanged.
// M3 lane 14 r162 (fast). Lead: with 0x11e000's byte-matching single-exit body (the existing helpers, `real result;`,
// held elsewhere because 0x1a8fa0 then inlines it at its first site, 428), `(real)sqrt(function_11e000(point, start,
// segment))` at both sites is inlined as retail does: 1578 B, the second block exactly retail's and the first block's
// dot (k, j, i) and else branch (k first) too, aligned 32 against this body's 90, but 205 by position. Left, all in the
// first block: the segment's k and j in xmm4/xmm3 (retail xmm3/xmm4; the three reloads tie at 104 and colour i, j, k),
// the update computing k last in the scale's register where retail computes it first in the segment k register, so
// the sum ends in xmm3 and its store merges after the join (retail stores per branch). On that form the segment's
// stores in the five other orders 181 (i, k, j: retail's segment registers, its store order and update still differ),
// 214 (j, i, k), 239 (j, k, i), 187 (k, i, j), 225 (k, j, i). A forced-ranking search needs three or more decisions
// (the update's k value and both segment reloads forced: 67 to 243). Body unchanged.
// @retail 0x1a83e0
bool __stdcall function_1a83e0(long arg_0, short *arg_1, real *arg_2, point2f *arg_3, bool *arg_4)
{
    s_actor_view *local_0 = actor_get(arg_0);
    byte *local_1 = g_4e3b44[ai_object_get(local_0->unknown018)->definition_index & 0xffff].bytes;
    bool local_3 = false;
    bool local_4 = false;
    short local_5 = NONE;
    real local_2 = *(real *)(local_1 + 0x124);
    point2f local_6;
    if (local_2 > 0.0f)
    {
        local_6.x = -local_0->unknown37c.i;
        local_6.y = -local_0->unknown37c.j;
        if (function_1a83e2(&local_6) < 1.0f)
        {
            local_6.x = local_0->unknown370.x - local_0->position.x;
            local_6.y = local_0->unknown370.y - local_0->position.y;
            if (normalize2d(&local_6) == 0.0f)
            {
                local_6 = *(point2f const *)&local_0->unknown290;
                if (normalize2d(&local_6) == 0.0f)
                    local_6 = *g_468778;
            }
        }
        vector3f local_7;
        local_7.i = *(real *)((byte *)local_0 + 0x388) - local_0->unknown370.x;
        local_7.j = *(real *)((byte *)local_0 + 0x38c) - local_0->unknown370.y;
        local_7.k = *(real *)((byte *)local_0 + 0x390) - local_0->unknown370.z;
        point3f local_8;
        point3f local_9;
        vector3f local_10;
        vector3f local_11;
        local_10.i = 0.0f - local_6.y;
        local_10.j = local_6.x;
        local_10.k = 0.0f;
        local_11.i = local_6.y;
        local_11.j = 0.0f - local_6.x;
        local_11.k = 0.0f;
        point_offset_along_vector(&local_0->position, &local_10, local_2, &local_8);
        point_offset_along_vector(&local_0->position, &local_11, local_2, &local_9);
        bool local_12;
        bool local_13;
        s_path_trace_result local_14;
        bool local_15 = function_1f5110(arg_0, (point2f const *)&local_10, local_2, 0.0f, &local_12, &local_14);
        real local_16 = function_1a83e1(&local_0->unknown370, &local_7, &local_8);
        bool local_17 = local_15 && local_16 > local_0->unknown36c;
        bool local_18 = function_1f5110(arg_0, (point2f const *)&local_11, local_2, 0.0f, &local_13, &local_14);
        real local_19 = function_1a83e1(&local_0->unknown370, &local_7, &local_9);
        bool local_20 = local_18 && local_19 > local_0->unknown36c;
        if (local_15)
        {
            if (local_18)
            {
                real local_21 = local_16 - local_19;
                if (local_12 > local_13 || local_17 > local_20 || local_21 > 0.3f)
                {
                    local_5 = 0;
                    local_4 = local_12;
                    local_3 = true;
                }
                else if (local_13 > local_12 || local_20 > local_17 || local_21 < -0.3f)
                {
                    local_5 = 1;
                    local_4 = local_13;
                    local_3 = true;
                }
                else
                {
                    local_5 = 4;
                    local_4 = local_12;
                    local_3 = local_17;
                }
            }
            else
            {
                local_5 = 0;
                local_4 = local_12;
                local_3 = local_17;
            }
        }
        else if (local_18)
        {
            local_5 = 1;
            local_4 = local_13;
            local_3 = local_20;
        }
    }
    *arg_1 = local_5;
    *arg_2 = local_2;
    *arg_4 = local_4;
    *arg_3 = local_6;
    return local_3;
}
