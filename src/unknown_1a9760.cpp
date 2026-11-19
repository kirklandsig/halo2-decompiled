#include "unknown_11c920.h"
#include "ai_actor.h"
#include "unknown_26c380.h"

// @flags /O2 /arch:SSE /Gr

real function_30bf0(vector3f *arg_0);
bool function_1f5110(long arg_0, point2f const *arg_1, real arg_2,
    real arg_3, bool *arg_4, s_path_trace_result *arg_5);

#pragma inline_depth(0)
/* chooses and stores an actor's movement direction from a stored vector, a perpendicular side test and a
   path trace; the length's sum of squares is staged around the flag store, as retail schedules it */
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
    vector3f unit;
    vector3f const *facing = &local_0->unknown290;
    double squares = local_2.i * local_2.i + local_2.k * local_2.k;
    double &sum = squares;
    local_0->unknown456 = true;
    for (long component = 0; component < 3; ++component)
        local_3.n[component] = (local_0->position.n[component] + local_0->unknown258.n[component] * 0.5f) - local_0->unknown370.n[component];
    sum += local_2.j * local_2.j;
    real local_4 = (real)sqrt(sum);
    if (!((real)fabs((double)local_4) < 0.0001f))
    {
        real local_8 = 1.0f / local_4;
        unit.i = local_8 * local_2.i;
        unit.j = local_8 * local_2.j;
        unit.k = local_2.k * local_8;
    }
    else
        local_4 = 0.0f;
    if (local_4 > 1.5f)
    {
        vector3f perpendicular;
        perpendicular.i = 0.0f - unit.j;
        perpendicular.j = unit.i;
        perpendicular.k = unit.k;
        real local_5 = (local_3.j * perpendicular.j + local_3.k * perpendicular.k) + local_3.i * perpendicular.i;
        if (local_5 > 0.0f)
            local_2 = perpendicular;
        else
        {
            local_2.i = perpendicular.i * -1.0f;
            local_2.j = perpendicular.j * -1.0f;
        }
        vector3f delta;
        delta.i = local_0->position.x - local_0->unknown370.x;
        delta.j = local_0->position.y - local_0->unknown370.y;
        delta.k = local_0->position.z - local_0->unknown370.z;
        real local_6 = (delta.k * perpendicular.k + delta.j * perpendicular.j) + delta.i * perpendicular.i;
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
    vector3f *scaled_direction = &local_0->unknown458;
    scaled_direction->i = local_2.i * 10.0f;
    scaled_direction->j = local_2.j * 10.0f;
    scaled_direction->k = local_2.k * 10.0f;
    local_0->unknown41c = 4;
    local_0->unknown420 = 4;
    local_0->unknown424.vector = local_2;
    real alignment = facing->j * local_2.j;
    alignment += facing->k * local_2.k;
    alignment += facing->i * local_2.i;
    if (alignment > 0.8f)
        function_1e4290(arg_0, true);
}
#pragma inline_depth(255)
