// @flags /O2 /Oi /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_1cec30.h"
#include "object_markers.h"
#include "normalize3d.h"
#include <string.h>

struct s_vehicle_physics_state;

struct s_205be0_record
{
    transform4x3f field_0;
    point3f field_34;
    byte field_40[0x41];
    bool field_81;
    byte field_82[2];
    point3f field_84;
    byte field_90[0x18];
};

struct s_205be0_record2
{
    transform4x3f field_0;
    point3f field_34;
    byte field_40[0x42];
    bool field_82;
    byte field_83[0x30];
    bool field_b3;
    byte field_b4[0x24];
};

struct s_205be0_marker_flags
{
    dword : 4;
    dword bit4 : 1;
    dword bit5 : 1;
    dword : 26;
};

struct s_205be0_marker
{
    long marker_name;
    s_205be0_marker_flags flags;
    byte field_8[0x3a];
    short field_42;
    byte field_44[4];
    long field_48;
};

struct s_205be0_object_flags
{
    word : 5;
    word bit5 : 1;
    word : 10;
};

struct s_205be0
{
    long field_0;
    byte *field_4;
    transform4x3f field_8;
    matrix3x3 field_3c;
    vector3f field_60;
    real field_6c;
    real field_70;
    vector3f field_74;
    vector3f field_80;
    real field_8c;
    s_205be0_record field_90[16];
    s_205be0_record2 field_b10[16];
};

void function_ba1d0(long arg_0, vector3f *arg_1, vector3f *arg_2);
transform4x3f *function_ba160(long arg_0, transform4x3f *arg_1);
real function_30bf0(vector3f *arg_0);
void havok_component_transform_set(s_havok_component *arg_0, transform4x3f const *arg_1);
void function_1cfb90(s_havok_component *arg_0, transform4x3f const *arg_1, void const *arg_2);
void function_ba690(long object_index, long *state_count, byte **states, long *a, long *b);

/* the vector rotated into the matrix's axes and divided by its scale */
PRIVATE inline vector3f *transform_vector_into_frame_205be0(transform4x3f const *matrix,
    vector3f const *vector, vector3f *result)
{
    real x = vector->i;
    real y = vector->j;
    real z = vector->k;
    if (matrix->scale != 1.f)
    {
        real inverse = 1.f / matrix->scale;
        x = inverse * x;
        y = inverse * y;
        z = inverse * z;
    }
    result->i = matrix->forward.k * z + matrix->forward.j * y + matrix->forward.i * x;
    result->j = matrix->left.k * z + matrix->left.j * y + matrix->left.i * x;
    result->k = matrix->up.k * z + matrix->up.j * y + matrix->up.i * x;
    return result;
}

// @retail 0x205be0
bool __stdcall function_205be0(s_vehicle_physics_state *arg_0, long arg_1)
{
    bool result;
    {
        long local_0 = (arg_1 & 0xffff) * 12;
        byte *local_1 = *(byte **)(g_4e0300->data + local_0 + 8);
        byte *local_2 = g_4e3b44[*(long *)local_1 & 0xffff].bytes + 0x2ac;
        result = false;
        if (*(long *)(local_1 + 0xb4) == NONE)
            goto done;
        s_havok_component *local_4 = havok_component_get(*(long *)(local_1 + 0xb4));
        if ((local_2[0] & 1) || local_4->rigid_bodies.size <= 0)
            goto done;
        vector3f local_5;
        function_ba1d0(arg_1, &local_5, NULL);
        s_205be0 *local_6 = (s_205be0 *)arg_0;
        local_4->rigid_body_inertia_get(0, &local_6->field_3c);
        byte *local_7 = (byte *)local_4->rigid_bodies.data[0].rigid_body->m_motion;
        vector3f const *motion = (vector3f const *)(local_7 + 0x70);
        real_point3d_set((point3f *)&local_6->field_60, motion->i, motion->j, motion->k);
        local_6->field_0 = arg_1;
        local_6->field_4 = local_2;
        local_6->field_6c = local_4->rigid_body_mass_get(0);
        local_6->field_70 = 0.0f;
        transform4x3f *local_8 = &local_6->field_8;
        function_ba160(arg_1, local_8);
        function_30bf0(transform_vector_into_frame_205be0(local_8, &local_5, &local_6->field_80));
        local_6->field_8c = 0.0f;
        local_6->field_74 = *(vector3f *)(local_1 + 0x1b0);
        memset(local_6->field_90, 0, *(long *)(local_2 + 0x3c) * sizeof(s_205be0_record));
        memset(local_6->field_b10, 0, *(long *)(local_2 + 0x44) * sizeof(s_205be0_record2));
        long local_9;
        long local_10;
        function_ba690(arg_1, &local_9, NULL, NULL, &local_10);
        char *local_11 = (char *)local_10;
        s_object_marker local_12;
        for (long local_13 = 0; local_13 < *(long *)(local_2 + 0x3c); local_13++)
        {
            s_205be0_marker *local_14 = &(*(s_205be0_marker **)(local_2 + 0x40))[local_13];
            function_b8d30(arg_1, local_14->marker_name, &local_12, 1, false);
            local_6->field_90[local_13].field_0 = *local_8;
            local_6->field_90[local_13].field_34 = local_12.matrix.position;
            local_6->field_90[local_13].field_84 = local_12.node_matrix.position;
            local_6->field_90[local_13].field_81 = local_12.node_index == 0;
        }
        for (long local_16 = 0; local_16 < *(long *)(local_2 + 0x44); local_16++)
        {
            s_205be0_marker *local_17 = &(*(s_205be0_marker **)(local_2 + 0x48))[local_16];
            function_b8d30(arg_1, local_17->marker_name, &local_12, 1, false);
            local_6->field_b10[local_16].field_0 = *local_8;
            local_6->field_b10[local_16].field_34 = local_12.matrix.position;
            local_6->field_b10[local_16].field_82 = TEST_FIELD_BIT(local_17->flags.bit4) &&
                TEST_FIELD_BIT(((s_205be0_object_flags *)(local_1 + 0x348))->bit5);
            if (TEST_FIELD_BIT(local_17->flags.bit5) && local_17->field_48 != NONE &&
                local_11[local_17->field_48 * 8 + 1] >= local_17->field_42)
                local_6->field_b10[local_16].field_b3 = true;
        }
        havok_component_transform_set(local_4, local_8);
        if (*(long *)(local_2 + 0x4c))
        {
            byte *local_19 = *(byte **)(local_2 + 0x50);
            if (local_19[0x3c] & 1)
            {
                real local_20[6];
                memcpy(local_20, local_19 + 0x48, sizeof(local_20));
                function_1cfb90(local_4, local_8, local_20);
            }
        }
        result = true;
    }
done:
    return result;
}
