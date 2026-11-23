#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "unknown_11cb00.h"
#include "unknown_11cc90.h"
#include "slot_handler.h"
#include "unknown_1e1f20.h"
#include "normalize3d.h"
#include <math.h>
#include <string.h>
// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_186ab2
{
    long field_0;
    long field_4;
    long field_8;
    byte field_c[0xc];
    short field_18;
    byte field_1a[2];
    real field_1c;
    real field_20;
};
struct s_186ab3
{
    real field_0;
    real field_4;
    real field_8;
    real field_c;
    real field_10;
    real field_14;
    dword field_18;
    dword field_1c;
    word field_20;
    byte field_22[6];
    s_186ab2 field_28;
};
struct s_186ab4
{
    void *field_0;
    byte *field_4;
};
struct s_185be0
{
    byte field_0[0x10];
    word field_10[0x10];
    real field_30;
    real field_34;
    real field_38;
    real field_3c;
    real field_40;
    real field_44;
    real field_48;
    byte field_4c;
    byte field_4d;
    bool field_4e;
    byte field_4f;
};
struct s_input_state
{
    byte unknown00[0x10];
    byte values[0x38];
};
struct s_185be6
{
    long field_0;
    byte field_4[8];
    word field_c;
    word field_e;
    byte field_10[0x2e - 0x10];
    short field_2e;
    byte field_30[4];
    vector3f field_34;
    long field_40;
    byte field_44[0x68 - 0x44];
    bool field_68;
    byte field_69[0x78 - 0x69];
    real field_78;
    real field_7c;
    byte field_80[8];
    byte field_88;
    byte field_89[5];
    byte field_8e;
    byte field_8f;
    byte field_90;
    byte field_91[2];
    byte field_93;
};
struct s_small_index;
struct s_havok_component;
struct s_unknown_13bf00;
struct s_player_action;
struct s_player_action_triggers;
extern byte g_4e61b9;
extern s_input_state g_4e61dc[3];
extern s_input_state g_4e630c;
extern s_unknown_13bf00 *g_510c50;
extern byte g_51ea18[];
real magnitude3d(vector3f const *arg_0);
real function_185b30(short arg_0, real const *arg_1, real arg_2);
real function_c8880(long arg_0, short arg_1);
short function_0b67a0(s_small_index const *arg_0);
bool function_e4050(long arg_0);
bool function_100f70(long arg_0);
vector2f *function_11df30(vector2f *arg_0, vector3f const *arg_1);
void __stdcall function_1a4900(long arg_0, s_186ab2 *arg_1, vector3f *arg_2);
void player_control_update_action_flags(s_player_action *arg_0, s_player_action_triggers const *arg_1);

#define FLAG(bit) (1 << (bit))
#define SET_FLAG(flags, bit, value) ((value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)))
#define PIN(value, minimum, maximum) ((value) < (minimum) ? (minimum) : ((value) > (maximum) ? (maximum) : (value)))
__forceinline byte *function_185be2(long arg_0)
{
    return *(byte **)(g_4e0300->data + (arg_0 & 0xffff) * 12 + 8);
}
__forceinline bool function_185be3(long arg_0)
{
    return arg_0 != NONE && ((1 << g_4e0300->data[(arg_0 & 0xffff) * 12 + 3]) & 1);
}
__forceinline void function_185be5(s_186ab2 *arg_0)
{
    arg_0->field_1c = 0.0f;
    arg_0->field_20 = 0.0f;
    arg_0->field_0 = NONE;
    arg_0->field_4 = NONE;
    arg_0->field_8 = NONE;
    arg_0->field_18 = 0;
}

union s_185be7
{
    real field_0[2];
    vector2f field_8;
};

/* scales one turn axis by how long its stick has been held past the threshold: the ramp is {time, maximum scale} */
static __forceinline void turn_hold_scale_185be0(real *turn, real *hold_time, real stick, real const *threshold, real const *ramp, real dt)
{
    if (fabs(stick) >= *threshold)
    {
        real fraction = PIN(*hold_time / ramp[0], 0.0f, 1.0f);
        real scale = (ramp[1] - 1.0f) * fraction + 1.0f;
        *turn *= scale;
        *hold_time += dt;
    }
    else *hold_time = 0.0f;
}

// @retail 0x185be0
void __stdcall function_185be0(long arg_0, long arg_1, real arg_2, real arg_3, s_186ab4 *arg_4, s_186ab3 *arg_5)
{
    byte *local_0 = *(byte **)((byte *)g_4e034c + 0xf4);
    s_185be6 *local_1 = (s_185be6 *)((byte *)g_4ed284 + 0x14) + arg_1;
    s_185be0 local_2;
    memset(&local_2, 0, sizeof(local_2));
    memset(arg_5, 0, sizeof(*arg_5));
    function_185be5(&arg_5->field_28);
    if (arg_0 != NONE && g_4e61cc[(short)arg_0])
    {
        arg_4->field_0 = g_4e61b9 ? &g_4e630c : &g_4e61dc[(short)arg_0];
        byte local_3[16];
        word local_4[16];
        memset(local_3, 0, sizeof(local_3));
        memset(local_4, 0, sizeof(local_4));
        local_2 = ((s_185be0 *)(g_51ea18 + 0x70))[arg_0];
        s_185be7 local_5;
        local_5.field_0[0] = *(real *)(g_51ea18 + arg_0 * 0x1c) * 0.01745329238474369f;
        local_5.field_0[1] = *(real *)(g_51ea18 + arg_0 * 0x1c + 4) * 0.01745329238474369f;
        long local_7 = local_1->field_0;
        if (local_7 != NONE)
        {
            byte *local_8 = function_185be2(local_7);
            long local_9 = *(long *)(local_8 + 0x14);
            if (local_9 != NONE)
            {
                short local_10 = *(short *)(local_8 + 0x1fc);
                if (local_10 != NONE)
                {
                    byte *local_11 = function_185be2(local_9);
                    byte *local_12 = (byte *)g_4e3b44[*(long *)local_11 & 0xffff].data;
                    byte *local_13 = *(byte **)(local_12 + 0x1cc) + local_10 * 0xb0;
                    real local_14 = *(real *)(local_13 + 0x54);
                    real local_15 = *(real *)(local_13 + 0x58) - local_14;
                    real local_16 = 0.0f;
                    if (local_15 > 0.0f)
                    {
                        local_16 = (magnitude3d((vector3f *)(local_11 + 0x88)) - local_14) / local_15;
                        local_16 = PIN(local_16, 0.0f, 1.0f);
                    }
                    if (*(real *)(local_13 + 0x5c) != 0.0f)
                        local_16 = (real)pow(local_16, *(real *)(local_13 + 0x5c));
                    if (*(real *)(local_13 + 0x44) > 0.0f || *(real *)(local_13 + 0x48) > 0.0f)
                        local_5.field_0[0] = ((*(real *)(local_13 + 0x48) - *(real *)(local_13 + 0x44)) * local_16 + *(real *)(local_13 + 0x44)) * 0.01745329238474369f;
                    if (*(real *)(local_13 + 0x4c) > 0.0f || *(real *)(local_13 + 0x50) > 0.0f)
                        local_5.field_0[1] = ((*(real *)(local_13 + 0x50) - *(real *)(local_13 + 0x4c)) * local_16 + *(real *)(local_13 + 0x4c)) * 0.01745329238474369f;
                }
            }
        }
        arg_5->field_0 = local_2.field_38;
        arg_5->field_4 = local_2.field_3c;
        if (!(g_4ed284->flags10 & 1) && (!g_510c54->active || !g_510c54->unknown01))
        {
            real local_8 = (real)fabs(local_2.field_44);
            real local_9 = (real)fabs(local_2.field_40);
            real local_10;
            if (local_8 > 0.1f && local_9 > 0.1f)
            {
                if (local_8 > local_9) { local_9 /= local_8; local_8 = 1.0f; }
                else { local_8 /= local_9; local_9 = 1.0f; }
                local_10 = (real)sqrt(local_9 * local_9 + local_8 * local_8);
            }
            else local_10 = 1.0f;
            real local_11 = PIN(local_10 * local_2.field_40, -1.0f, 1.0f);
            real local_12 = PIN(local_10 * local_2.field_44, -1.0f, 1.0f);
            s_185be7 local_13;
            local_13.field_0[0] = function_185b30(*(short *)(local_0 + 0x74), *(real **)(local_0 + 0x78), local_11) * local_5.field_0[0];
            local_13.field_0[1] = function_185b30(*(short *)(local_0 + 0x74), *(real **)(local_0 + 0x78), local_12) * local_5.field_0[1];
            turn_hold_scale_185be0(&local_13.field_0[0], &local_1->field_78, local_11, (real *)(local_0 + 0x48), (real *)(local_0 + 0x4c), arg_2);
            turn_hold_scale_185be0(&local_13.field_0[1], &local_1->field_7c, local_12, (real *)(local_0 + 0x48), (real *)(local_0 + 0x54), arg_2);
            if (local_1->field_0 != NONE && local_1->field_2e != NONE)
            {
                real local_15 = 1.0f / function_c8880(local_1->field_0, local_1->field_2e);
                local_13.field_0[0] *= local_15;
                local_13.field_0[1] *= local_15;
            }
            if (local_1->field_0 != NONE)
            {
                real local_15 = 1.0f - *(real *)(function_185be2(local_1->field_0) + 0x2e4) *
                    *(real *)(*(byte **)((byte *)g_4e034c + 0x134) + 0x7c);
                local_13.field_0[0] *= local_15;
                local_13.field_0[1] *= local_15;
            }
            vector3f local_15;
            function_1a4900(arg_1, &arg_5->field_28, &local_15);
            local_1->field_68 = local_15.k > 0.0f;
            real local_16;
            real local_17;
            if (local_1->field_68 && arg_2 > 0.0f && (fabs(local_13.field_0[0]) > 0.0001f || fabs(local_13.field_0[1]) > 0.0001f ||
                fabs(arg_5->field_0) > 0.0001f || fabs(arg_5->field_4) > 0.0001f))
            {
                real local_18 = arg_3 / arg_2;
                real local_19 = 1.0f - PIN(*(real *)local_0, 0.0f, 1.0f) * local_15.k;
                real local_20 = PIN(*(real *)(local_0 + 4), 0.0f, 1.0f) * local_15.k;
                vector3f local_23;
                vector3f_scale(&local_15, local_18, &local_23);
                real local_21 = local_23.i;
                real local_22 = local_23.j;
                local_21 = PIN(local_21, -3.1415927410125732f, 3.1415927410125732f);
                local_22 = PIN(local_22, -1.5707963705062866f, 1.5707963705062866f);
                local_16 = local_21 * local_20 + local_19 * local_13.field_0[0];
                local_17 = local_22 * local_20 + local_19 * local_13.field_0[1];
            }
            else
            {
                local_17 = local_13.field_0[1];
                local_16 = local_13.field_0[0];
            }
            arg_5->field_10 = local_16 * arg_2;
            arg_5->field_14 = local_17 * arg_2;
            *(real *)((byte *)arg_5 + 0x24) = local_2.field_48;
            if (local_1->field_0 != NONE)
            {
                s_object_header_view *header = object_header_get(local_1->field_0);
                if ((1 << header->type) & 1)
                {
                    long local_18 = *(long *)(header->object + 0x3e4);
                    if (local_18 != NONE)
                    {
                        byte *const &parent = function_185be2(local_18);
                        long local_19 = *(long *)(parent + 0xb4);
                        if (local_19 != NONE)
                        {
                            s_havok_component *local_20 = havok_component_get(local_19);
                            long local_24 = function_0b67a0((s_small_index *)local_20);
                            if (local_24 != NONE)
                            {
                                transform4x3f local_21;
                                local_20->rigid_body_matrix_get(local_24, &local_21);
                                local_15 = local_21.forward;
                                if (local_18 == local_1->field_40)
                                {
                                    function_11df30(&local_5.field_8, &local_15);
                                    function_11df30(&local_13.field_8, &local_1->field_34);
                                    arg_5->field_10 += local_5.field_8.i - local_13.field_8.i;
                                    arg_5->field_14 += local_5.field_8.j - local_13.field_8.j;
                                }
                            }
                        }
                        local_1->field_34 = local_15;
                    }
                    local_1->field_40 = local_18;
                }
            }
        }
        else
        {
            arg_5->field_10 = 0.0f;
            arg_5->field_14 = 0.0f;
            *(real *)((byte *)arg_5 + 0x24) = 0.0f;
        }
        word local_8 = local_1->field_e & local_1->field_c;
        if (local_8)
        {
            for (long local_9 = 0; local_9 < 16; ++local_9)
            {
                if ((local_8 & (1 << local_9)) && !local_2.field_0[local_9])
                {
                    local_1->field_c &= ~(1 << local_9);
                    local_1->field_e &= ~(1 << local_9);
                }
            }
        }
        for (long local_24 = 0; local_24 < 16; local_24++)
        {
            if (!(local_1->field_c & (1 << local_24)))
            {
                local_3[local_24] = local_2.field_0[local_24];
                local_4[local_24] = local_2.field_10[local_24];
            }
        }
        bool local_9 = function_185be3(local_1->field_0) && function_e4050(local_1->field_0);
        if (local_3[10] == 1)
        {
            if (local_9 || arg_5->field_4 * arg_5->field_4 + arg_5->field_0 * arg_5->field_0 < 0.25f) local_1->field_88 = 1;
        }
        else if (!local_3[10]) local_1->field_88 = 0;
        SET_FLAG(arg_5->field_18, 0, local_1->field_88 != 0);
        bool local_10 = local_3[11] == 1;
        if (local_10 && local_1->field_2e == NONE && arg_4->field_0)
        {
            long local_11;
            if (local_1->field_0 != NONE) local_11 = unit_get_current_weapon(local_1->field_0);
            else local_11 = NONE;
            if (((byte *)arg_4->field_0)[6] > 0x7f || ((byte *)arg_4->field_0)[7] > 0x7f) local_10 = false;
            if (local_11 != NONE && !function_100f70(local_11) && (local_2.field_44 * local_2.field_44) + local_2.field_40 * local_2.field_40 > 0.64000004529953f) local_10 = false;
        }
        SET_FLAG(arg_5->field_1c, 2, local_10);
        SET_FLAG(arg_5->field_1c, 3, local_3[11] != 0);
        if (local_2.field_4e)
        {
            arg_5->field_8 = local_2.field_30;
            arg_5->field_c = local_2.field_34;
        }
        else
        {
            arg_5->field_8 = local_2.field_34;
            arg_5->field_c = local_2.field_30;
        }
        long local_11 = local_3[local_2.field_4c];
        byte *local_12 = &local_3[local_2.field_4c];
        if (local_1->field_0 != NONE && *(char *)(function_185be2(local_1->field_0) + 0x212) != NONE && *(char *)(function_185be2(local_1->field_0) + 0x213) != NONE)
        {
            *local_12 = 0;
            local_1->field_90 = local_11 > 0;
        }
        else if (local_1->field_90)
        {
            if (local_11 > 0) { local_11 = 0; *local_12 = 0; }
            else local_1->field_90 = 0;
        }
        SET_FLAG(arg_5->field_18, 16, local_3[7] != 0);
        SET_FLAG(arg_5->field_18, 21, local_11 != 0);
        SET_FLAG(arg_5->field_18, 17, local_11 != 0);
        if (g_4e6948->state != 2) SET_FLAG(arg_5->field_18, 29, local_3[local_2.field_4d] != 0);
        SET_FLAG(arg_5->field_18, 11, local_11 != 0);
        SET_FLAG(arg_5->field_18, 14, local_11 != 0);
        SET_FLAG(arg_5->field_18, 26, local_3[6] != 0);
        SET_FLAG(arg_5->field_18, 7, local_3[12] != 0);
        SET_FLAG(arg_5->field_18, 8, local_3[13] != 0);
        SET_FLAG(arg_5->field_18, 2, local_3[5] != 0);
        SET_FLAG(arg_5->field_18, 20, local_3[5] != 0);
        SET_FLAG(arg_5->field_18, 25, local_3[5] != 0);
        local_1->field_93 = (local_3[0] && ((g_510c50 && ((byte *)g_510c50)[5]) || local_1->field_93)) ? 1 : 0;
        SET_FLAG(arg_5->field_18, 1, local_3[0] && !local_1->field_93);
        SET_FLAG(arg_5->field_18, 4, local_3[4] != 0);
        SET_FLAG(arg_5->field_18, 5, local_3[4] != 0);
        SET_FLAG(arg_5->field_18, 6, local_3[7] != 0);
        SET_FLAG(arg_5->field_1c, 1, local_3[1] == 1);
        SET_FLAG(arg_5->field_1c, 4, local_3[14] != 0);
        SET_FLAG(arg_5->field_1c, 5, local_3[15] != 0);
        SET_FLAG(arg_5->field_20, 8, local_3[14] == 1);
        s_input_state *local_14 = NULL;
        if (g_4e61cc[(short)arg_0]) local_14 = g_4e61b9 ? &g_4e630c : &g_4e61dc[(short)arg_0];
        if (local_14 && ((byte *)local_14)[0x19]) arg_5->field_1c |= 0x40;
        long local_15 = real_truncate(*(real *)(*(byte **)((byte *)g_4e034c + 0xf4) + 0x7c) * 1000.0f);
        SET_FLAG(arg_5->field_20, 0, local_3[2] > 0);
        SET_FLAG(arg_5->field_20, 1, local_4[2] >= local_15);
        SET_FLAG(arg_5->field_20, 4, local_3[3] > 0);
        SET_FLAG(arg_5->field_20, 5, local_4[3] >= local_15);
        if (local_3[3] > 0) local_1->field_8e = 1;
        else
        {
            if (local_1->field_8e && !local_1->field_8f) arg_5->field_1c |= 1;
            local_1->field_8e = 0;
            local_1->field_8f = 0;
        }
    }
    player_control_update_action_flags((s_player_action *)arg_5, (s_player_action_triggers *)&local_2);
}
