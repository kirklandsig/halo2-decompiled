#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259a0.h"
#include "vector2f_products.h"
// @flags /O2 /arch:SSE /Gr

struct s_1badc0
{
    byte field_0[0x24];
    short field_24;
    byte field_26[0x5c - 0x26];
};

real normalize2d(point2f *arg_0);

/* 2-D helpers of the avoidance offset below: the left-hand perpendicular, a negation, and a dot
   product that sums its j term first (retail's order for the clearance-side test) */
static inline void perpendicular_1badc0(vector2f const *in, vector2f *out)
{
    out->i = 0.0f - in->j;
    out->j = in->i;
}

static inline void negate_1badc0(vector2f *v)
{
    v->i = 0.0f - v->i;
    v->j = 0.0f - v->j;
}

static inline real dot_product2d_j_first(vector2f const *a, vector2f const *b)
{
    real result = a->j * b->j;
    result += a->i * b->i;
    return result;
}

// @retail 0x1badc0
bool __stdcall function_1badc0(long arg_0, long arg_1, point3f const *arg_2,
    s_type_c3b527 *arg_3, long *arg_4, bool arg_5, bool *arg_6)
{
    s_actor_view *local_0 = actor_get(arg_0);
    bool result = false;
    if (arg_6)
        *arg_6 = false;
    if (!arg_5)
    {
        point3f local_1 = *arg_2;
        byte *local_2 = (byte *)object_get(arg_1);
        point3f local_3 = *(point3f *)(local_2 + 0x30);
        real local_4 = *(real *)(local_2 + 0x3c);
        point2f local_9;
        (void)local_9.x; // first mention of the perpendicular: retail's operand order in its side test follows it
        vector3f local_6 = *(vector3f const *)arg_2;
        vector3f local_c;
        vector3f local_5;
        local_c.i = local_3.x - local_0->position.x;
        local_5.i = local_6.i - local_0->position.x;
        local_c.j = local_3.y - local_0->position.y;
        local_5.j = local_6.j - local_0->position.y;
        vector3d_from_points3d((point3f const *)&local_6, &local_3, &local_6);
        real local_7 = local_5.j * local_5.j + local_5.i * local_5.i;
        if (local_7 > 0.0f)
        {
            real local_8 = dot_product2d((vector2f const *)&local_5, (vector2f const *)&local_c) / local_7;
            if (local_8 > 0.0f && local_8 < 1.2f)
            {
                perpendicular_1badc0((vector2f const *)&local_5, (vector2f *)&local_9);
                if (dot_product2d((vector2f const *)&local_c, (vector2f const *)&local_9) > 0.0f)
                    negate_1badc0((vector2f *)&local_9);
                if (normalize2d(&local_9) > 0.0f)
                {
                    local_4 *= 1.1f;
                    local_1.x = local_9.x * local_4 + local_3.x;
                    local_1.y = local_9.y * local_4 + local_3.y;
                    vector3f local_10;
                    local_10.i = local_1.x - local_0->position.x;
                    local_10.j = local_1.y - local_0->position.y;
                    local_10.k = local_1.z - local_0->position.z;
                    real local_11 = local_10.k * local_10.k + local_10.j * local_10.j + local_10.i * local_10.i;
                    if (local_11 > 0.0001f && local_11 < 4.0f)
                    {
                        local_11 = (real)sqrt(local_11);
                        vector3f local_12;
                        perpendicular_1badc0((vector2f const *)&local_6, (vector2f *)&local_12);
                        if (0.0f > dot_product2d_j_first((vector2f const *)&local_10, (vector2f const *)&local_12))
                            negate_1badc0((vector2f *)&local_12);
                        local_12.k = 0.0f;
                        if (normalize2d((point2f *)&local_12) > 0.0f)
                        {
                            real local_13 = 2.0f - local_11;
                            local_1.x += local_12.i * local_13;
                            local_1.y += local_12.j * local_13;
                            local_1.z += local_12.k * local_13;
                        }
                    }
                    if (arg_6)
                        *arg_6 = true;
                }
            }
        }
        byte *local_14 = (byte *)g_4e0348;
        volatile long local_15 = 0;
        if (*(long *)(local_14 + 0xc4) > 0)
            local_15 = *(long *)(local_14 + 0xc8);
        s_1badc0 local_16;
        local_16.field_24 = NONE;
        long local_17 = function_26d100(g_4687b0, &local_1, (long *)arg_3, (s_collision_result_1697c0 *)&local_16);
        if (local_17 != NONE)
        {
            result = true;
            if (arg_4)
                *arg_4 = local_17;
        }
    }
    else
    {
        real local_18 = local_0->position.x - arg_2->x;
        real local_19 = local_0->position.y - arg_2->y;
        real local_20 = (real)sqrt(local_19 * local_19 + local_18 * local_18);
        if (local_20 > 2.0f)
        {
            arg_3->point.x = arg_2->x + g_4687b0->i;
            arg_3->point.y = arg_2->y + g_4687b0->j;
            arg_3->point.z = arg_2->z + g_4687b0->k;
            arg_3->output_index = NONE;
            if (arg_6)
                *arg_6 = true;
        }
        else
        {
            arg_3->point = *arg_2;
            arg_3->output_index = NONE;
        }
        result = true;
    }
    return result;
}
