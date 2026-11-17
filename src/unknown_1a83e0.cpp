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
    real length_squared = segment->k * segment->k + segment->j * segment->j + segment->i * segment->i;
    if (length_squared > 0.0001f)
    {
        delta.i = point->x - start->x;
        delta.j = point->y - start->y;
        delta.k = point->z - start->z;
        real t = (delta.k * segment->k + delta.j * segment->j + delta.i * segment->i) / length_squared;
        if (t < 0.0f)
            t = 0.0f;
        else if (t > 1.0f)
            t = 1.0f;
        t = -t;
        delta.i += segment->i * t;
        delta.j += segment->j * t;
        delta.k += segment->k * t;
        squared_distance = delta.k * delta.k + delta.j * delta.j + delta.i * delta.i;
    }
    else
    {
        delta.i = start->x - point->x;
        delta.j = start->y - point->y;
        delta.k = start->z - point->z;
        squared_distance = delta.k * delta.k + delta.j * delta.j + delta.i * delta.i;
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

// score 205 (fast build at 32fb6007d; was 205): walked statement by statement against retail 0x1a83e0 (r726), no behaviour
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
