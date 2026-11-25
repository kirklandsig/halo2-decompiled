// @flags /O2 /arch:SSE /Gr
#include "projection_storage.h"
#include "unknown_11c920.h"
#include "camera_storage.h"
#include "unknown_0259d0.h"
#include "unknown_030290.h"
#include "normalize3d.h"
#include <math.h>
#include <string.h>

real function_30bf0(vector3f *arg_0);

struct s_501e0
{
    point3f field_0;
    real field_c;
    real field_10;
    dword field_14;
};

static __forceinline void world_vector_to_view(vector3f const *v, vector3f *out)
{
    real x = v->i, y = v->j, z = v->k;
    if (g_4b9e14.scale != 1.f)
    {
        x = g_4b9e14.scale * x;
        y = g_4b9e14.scale * y;
        z = g_4b9e14.scale * z;
    }
    out->i = g_4b9e14.forward.i * z + g_4b9e14.up.i * y + g_4b9e14.right.i * x;
    out->j = g_4b9e14.forward.j * z + g_4b9e14.up.j * y + g_4b9e14.right.j * x;
    out->k = g_4b9e14.forward.k * z + g_4b9e14.up.k * y + g_4b9e14.right.k * x;
}

static __forceinline bool vector_within_depth_cone(vector3f const *v)
{
    real limit = v->k * v->k * 4.f;
    return limit > v->i * v->i && limit > v->j * v->j;
}

// @retail 0x501e0
void function_501e0(s_501e0 const *arg_0, bool arg_1, s_501e0 *arg_2)
{
    vector3f local_0;
    vector3f local_1;
    vector3d_from_points3d(&g_4b9da0, &arg_0[0].field_0, &local_0);
    vector3d_from_points3d(&g_4b9da0, &arg_0[1].field_0, &local_1);
    world_vector_to_view(&local_0, &local_0);
    world_vector_to_view(&local_1, &local_1);
    if (local_0.k < 0.f && local_1.k < 0.f &&
        vector_within_depth_cone(&local_0) &&
        vector_within_depth_cone(&local_1))
    {
        real local_4 = 0.f - local_0.k;
        real local_5 = 0.f - local_1.k;
        real scale;
        if (arg_1)
            scale = 1.f / (local_5 + local_4);
        else
        {
            local_4 = 1.f;
            local_5 = 1.f;
            scale = 0.5f;
        }
        arg_2->field_0.x = (arg_0[1].field_0.x * local_4 + arg_0[0].field_0.x * local_5) * scale;
        arg_2->field_0.y = (arg_0[1].field_0.y * local_4 + arg_0[0].field_0.y * local_5) * scale;
        arg_2->field_0.z = (arg_0[1].field_0.z * local_4 + arg_0[0].field_0.z * local_5) * scale;
        arg_2->field_c = (arg_0[1].field_c + arg_0[0].field_c) * 0.5f;
        arg_2->field_10 = (arg_0[1].field_10 + arg_0[0].field_10) * 0.5f;
        arg_2->field_14 = arg_0[0].field_14;
        if (arg_1)
        {
            real local_7 = (local_1.i * local_4 - local_0.i * local_5) * 2.f;
            real local_8 = (local_1.j * local_4 - local_0.j * local_5) * 2.f;
            real local_9 = local_8 * local_8 + local_7 * local_7;
            if (local_9 >= 0.f)
                arg_2->field_c = (real)(sqrt(local_9) * arg_2->field_c * scale);
        }
        else
        {
            vector3f local_10;
            vector3d_from_points3d(&arg_0[0].field_0, &arg_0[1].field_0, &local_10);
            scale = function_30bf0(&local_10);
            real local_12 = dot3f(&g_4b9dac, &local_10);
            real local_13 = 1.f - local_12 * local_12;
            arg_2->field_c = (real)(sqrt(local_13 < 0.f ? 0.f : local_13 > 1.f ? 1.f : local_13) * arg_2->field_c * scale);
        }
    }
    else
    {
        memset(arg_2, 0, sizeof(*arg_2));
    }
}
