#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_2626b0.h"
#include "unknown_20fe20.h"
#include <math.h>
#include <string.h>
// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_29de20 { short field_0; short field_2; short field_4; };
struct s_29de21 { s_29de20 field_0[30]; short field_b4; };
short function_29de20(short arg_1, s_29de21 *arg_0, point3f const *arg_2, long arg_3);

struct s_29e0f0 { long field_0; point3f *field_4; };
struct s_29e0f1 { byte field_0[0x40]; long field_40; s_29e0f0 *field_44; };
struct s_29e0f2 { byte field_0[0x6c]; long field_6c; s_29e0f1 *field_70; };
struct s_29e0f3 { byte field_0[0xc4]; long field_c4; s_29e0f2 *field_c8; };
struct s_29e0f4 { short field_0; short field_2; };
struct s_29e0f5 { byte field_0[0x80]; long field_80; s_29e0f4 *field_84; };
struct s_29e0f6
{
    byte field_0[0x24];
    short field_24;
    byte field_26[0xa];
    long field_30;
    s_29e0f5 *field_34;
};
struct s_29e0f7 { byte field_0[0x168]; long field_168; s_29e0f6 *field_16c; };
struct s_29e0f8
{
    short field_0;
    short field_2;
    long field_4;
    short field_8;
    byte field_a[2];
    point3f field_c;
    short field_18;
    byte field_1a[2];
};
struct s_29e0f9
{
    bool field_0;
    byte field_1[3];
    s_type_c3b527 field_4;
    long field_14;
    real field_18;
    point3f field_1c;
    short field_28;
    byte field_2a[2];
    bool field_2c;
    char field_2d;
    bool field_2e;
    byte field_2f;
    s_29e0f8 field_30[5];
    byte field_bc[8];
};

PRIVATE __forceinline real function_29e2ea(point3f const *arg_0, point3f const *arg_1)
{
    real local_0 = arg_1->x - arg_0->x;
    real local_1 = arg_1->y - arg_0->y;
    real local_2 = arg_1->z - arg_0->z;
    return local_1 * local_1 + (local_0 * local_0 + local_2 * local_2);
}
PRIVATE __forceinline void function_29e64d(s_29e0f8 *arg_0, point3f const *arg_1)
{
    arg_0->field_0 = NONE;
    arg_0->field_8 = NONE;
    arg_0->field_c = *arg_1;
    arg_0->field_18 = NONE;
    arg_0->field_4 = NONE;
    arg_0->field_2 = 0;
}

PRIVATE __forceinline real function_29e3b1(point3f const *arg_0, point3f const *arg_1)
{
    real local_0 = arg_0->x - arg_1->x;
    real local_1 = arg_0->y - arg_1->y;
    real local_2 = arg_0->z - arg_1->z;
    return (real)sqrt(local_2 * local_2 + local_1 * local_1 + local_0 * local_0);
}

// @retail 0x29e0f0
bool __stdcall function_29e0f0(point3f const *arg_0, long arg_1, s_reference arg_2,
    s_29de21 *arg_3, s_29e0f9 *arg_4)
{
    long local_0 = NONE;
    long local_1 = NONE;
    long local_2 = NONE;
    real local_3 = 3.402823466e38f;
    bool local_4 = false;
    s_29e0f3 *local_5 = (s_29e0f3 *)g_4e0348;
    s_29e0f2 *local_6 = NULL;
    if (local_5->field_c4 > 0) local_6 = local_5->field_c8;
    if (local_6 && local_6->field_6c > 0 && !(arg_2.unknown2 & 0x8000) && arg_2.unknown2 >= 0)
    {
        s_29e0f7 *local_7 = (s_29e0f7 *)g_4e0350;
        if (arg_2.unknown2 < local_7->field_168)
        {
            s_262b40_result *local_8 = function_262b40(arg_2);
            if (local_8)
            {
                s_29e0f6 *local_9 = &local_7->field_16c[(word)arg_2.unknown2];
                if (local_8->unknown10 >= 0 && local_8->unknown10 < local_9->field_30 && local_9->field_24 == g_4686c4)
                {
                    s_29e0f5 *local_10 = &local_9->field_34[local_8->unknown10];
                    s_29e0f1 *local_11 = local_6->field_70;
                    point3f local_12;
                    function_210850((s_type_c3b527 const *)local_8, &local_12);
                    dword local_13[2] = {0, 0};
                    for (short local_14 = 0; local_14 < local_10->field_80; ++local_14)
                    {
                        long &local_15 = *(long *)&arg_2;
                        local_15 = local_10->field_84[local_14].field_0;
                        if ((local_13[local_15 >> 5] & (1 << (local_15 & 31))) || local_15 < 0 || local_15 >= local_11->field_40) continue;
                        local_13[local_15 >> 5] |= 1 << (local_15 & 31);
                        dword local_16[1] = {0};
                        short local_17 = local_10->field_84[local_14].field_2;
                        short local_18 = local_17;
                        short local_19 = NONE;
                        short local_20 = NONE;
                        real local_21 = 3.402823466e38f;
                        s_29e0f0 *local_22 = &local_11->field_44[local_15];
                        for (short local_23 = local_14; local_23 < local_10->field_80 && local_10->field_84[local_23].field_0 == local_15; ++local_23)
                        {
                            local_18 = local_10->field_84[local_23].field_2;
                            local_16[local_18 >> 5] |= 1 << (local_18 & 31);
                            if (local_18 >= 0 && local_18 < local_22->field_0)
                            {
                                real local_24 = function_29e2ea(&local_12, &local_22->field_4[local_18]);
                                if (local_21 > local_24) { local_21 = local_24; local_19 = local_18; }
                            }
                        }
                        if (local_19 == NONE) continue;
                        for (short local_25 = 0; local_25 < arg_3->field_b4; ++local_25)
                            if (arg_3->field_0[local_25].field_0 == local_15) { local_20 = local_25; break; }
                        short local_26 = 1;
                        short local_28 = local_19;
                        for (short local_27 = 0; local_27 < 2; ++local_27)
                        {
                            short local_29 = local_19;
                            point3f *local_30 = &local_22->field_4[local_19];
                            real local_31 = function_29e3b1(&local_12, local_30);
                            bool local_32 = false;
                            while (local_28 >= 0 && local_28 < local_22->field_0)
                            {
                                if (local_32 && ((local_27 == 0 && local_28 > local_18) || (local_27 == 1 && local_28 < local_17))) break;
                                if (local_16[local_28 >> 5] & (1 << (local_28 & 31)))
                                {
                                    local_30 = &local_22->field_4[local_28];
                                    local_29 = local_28;
                                    local_31 = function_29e3b1(&local_12, local_30);
                                }
                                short local_33 = local_20 + local_28;
                                if (local_33 >= 0 && local_33 < arg_3->field_b4 && arg_3->field_0[local_33].field_4 != 2)
                                {
                                    point3f *local_34 = &local_22->field_4[local_28];
                                    real local_35 = function_29e3b1(local_34, arg_0) + function_29e3b1(local_30, local_34) + local_31;
                                    if (local_3 > local_35 && function_29de20(local_33, arg_3, arg_0, arg_1) == 1)
                                    {
                                        local_3 = local_35;
                                        local_0 = (short)local_15;
                                        local_1 = local_28;
                                        local_2 = local_29;
                                        local_32 = true;
                                    }
                                }
                                local_28 += local_26;
                            }
                            local_26 = -local_26;
                            local_28 = local_19 + local_26;
                        }
                    }
                    if ((short)local_0 != NONE && (short)local_1 != NONE && (short)local_2 != NONE)
                    {
                        s_29e0f0 *local_36 = &local_11->field_44[(short)local_0];
                        short local_37 = 0;
                        do
                        {
                            function_29e64d(&arg_4->field_30[local_37], &local_36->field_4[(short)local_1]);
                            ++local_37;
                            if ((short)local_1 == (short)local_2) break;
                            local_1 += (short)local_2 > (short)local_1 ? 1 : -1;
                        } while (local_37 < 4);
                        if (local_37 < 4)
                        {
                            function_29e64d(&arg_4->field_30[local_37], &local_12);
                            ++local_37;
                            arg_4->field_2c = true;
                        }
                        else arg_4->field_2c = false;
                        arg_4->field_2d = (byte)local_37;
                        local_4 = true;
                    }
                }
            }
        }
    }
    return local_4;
}

long function_baf80(long arg_0);
long function_1e4990(long arg_0);
bool function_2702a0(long arg_0, long arg_1, long arg_2, long arg_3, long arg_4,
    point3f const *arg_5, vector3f const *arg_6, point3f *arg_7,
    long *arg_8, vector3f *arg_9, long *arg_10);
struct s_29db90;
void function_29db90(short arg_1, short arg_0, s_29db90 *arg_2);
bool function_29dd50(point3f const *arg_0, point3f const *arg_1, long arg_2, long arg_3);

struct s_29e730
{
    short field_0;
    byte field_2[0xa];
    s_reference field_c;
    bool field_10;
    byte field_11[0x18];
    bool field_29;
    byte field_2a[2];
    vector3f field_2c;
};
struct s_29e731 { byte field_0[0x18]; long field_18; byte field_1c[0x38]; long field_54; };

// @retail 0x29e730
bool __stdcall function_29e730(s_29e730 *arg_0, long arg_1, point3f const *arg_2,
    s_type_c3b527 const *arg_3, s_reference arg_4, s_29de21 *arg_5,
    long arg_6, s_29e0f9 *arg_7)
{
    bool local_0 = false;
    s_29e731 *local_1 = (s_29e731 *)(g_4f55f0->data + (arg_1 & 0xffff) * 0x888);
    long local_2 = local_1->field_18 == NONE ? NONE : function_baf80(local_1->field_18);
    memset(arg_7, 0, sizeof(*arg_7));
    s_type_c3b527 local_3;
    vector3f local_4;
    point3f local_5;
    s_type_c3b527 local_6;
    point3f local_7;
    vector3f local_8;
    point3f local_9;
    s_262b40_result *local_10;
    if (arg_0->field_0 == 6)
    {
        if (!function_262a90(arg_0->field_c, &local_5, &local_4)) goto local_20;
        if (!function_2702a0(arg_1, 0xf000545, 0x5000049, 0x6000542, 0x400000c,
            &local_5, &local_4, &local_9, NULL, &local_8, NULL)) goto local_20;
        local_10 = function_262b40(arg_0->field_c);
        short local_11 = ((s_type_c3b527 *)local_10)->output_index;
        if (!function_210770(local_11, (vector3f *)&local_5, &local_8)) goto local_20;
        if (!function_210690(local_11, &local_9, &local_3.point)) goto local_20;
        local_5.x = local_9.x + local_4.i * 1.5f;
        local_5.y = local_9.y + local_4.j * 1.5f;
        local_5.z = local_9.z + local_4.k * 1.5f;
        local_3.output_index = local_11;
        if (arg_0->field_10)
        {
            arg_0->field_29 = true;
            arg_0->field_2c = local_8;
        }
        else if (function_210690(local_11, &local_5, &local_6.point))
        {
            local_6.output_index = local_11;
            local_0 = true;
        }
        goto local_21;
    }
    if (arg_0->field_0 == 4)
    {
        long local_12 = function_1e4990(local_1->field_54);
        if (!local_12 || (*(byte *)local_12 & 2)) goto local_20;
        local_10 = function_262b40(arg_0->field_c);
        if (!local_10 || !((byte)local_10->flags & 0x40)) goto local_20;
        if (!function_262af0(arg_0->field_c, &local_5, &local_8)) goto local_20;
        if (!function_2702a0(arg_1, 0x6000542, 0x400069d, 0x6000542, 0x400000c,
            (point3f *)&local_8, (vector3f *)&local_5, &local_9, NULL, &local_4, NULL)) goto local_20;
        local_3.output_index = ((s_type_c3b527 *)local_10)->output_index;
        if (!function_210690(local_3.output_index, &local_9, &local_3.point)) goto local_20;
        goto local_21;
    }
local_20:
    local_3 = *arg_3;
    local_0 = false;
local_21:
    if (local_3.output_index == NONE || !function_2104b0(local_3.output_index, &local_3.point, &local_7))
        local_7 = local_3.point;
    arg_7->field_1c = *arg_2;
    arg_7->field_4 = local_3;
    arg_7->field_14 = NONE;
    arg_7->field_28 = NONE;
    arg_7->field_2e = false;
    arg_7->field_18 = 0.0f;
    bool local_13;
    if (function_29dd50(arg_2, &local_7, local_2, arg_6))
    {
        *(s_type_c3b527 *)&arg_7->field_30[0].field_c = local_3;
        arg_7->field_30[0].field_0 = NONE;
        arg_7->field_30[0].field_4 = NONE;
        arg_7->field_30[0].field_8 = NONE;
        arg_7->field_2d = 1;
        arg_7->field_2c = true;
        local_13 = true;
    }
    else
    {
        s_reference local_14 = g_470fa0;
        if (*(long *)&arg_4 == *(long *)&local_14 || (arg_4.unknown2 & 0x8000))
        {
            local_13 = false;
            goto local_22;
        }
        s_29de21 local_15;
        if (!arg_5)
        {
            s_262b40_result *local_16 = function_262b40(arg_4);
            arg_5 = &local_15;
            local_15.field_b4 = 0;
            function_29db90(local_16->unknown10, arg_4.unknown2, (s_29db90 *)&local_15);
        }
        local_13 = function_29e0f0(arg_2, local_2, arg_4, arg_5, arg_7);
        if (local_13 && arg_7->field_2c && arg_7->field_2d > 0)
        {
            s_29e0f8 *local_17 = &arg_7->field_30[arg_7->field_2d - 1];
            local_17->field_0 = NONE;
            *(s_type_c3b527 *)&local_17->field_c = local_3;
            local_17->field_4 = NONE;
            local_17->field_8 = NONE;
        }
    }
    if (arg_7->field_2c && local_0)
    {
        short local_18 = arg_7->field_2d - 1;
        if (arg_7->field_2d < 4)
        {
            arg_7->field_30[arg_7->field_2d] = arg_7->field_30[arg_7->field_2d - 1];
            ++arg_7->field_2d;
        }
        else arg_7->field_2c = false;
        s_29e0f8 *local_19 = &arg_7->field_30[local_18];
        local_19->field_c = local_6.point;
        local_19->field_0 = NONE;
        local_19->field_8 = NONE;
        local_19->field_18 = local_6.output_index;
        local_19->field_4 = NONE;
        local_19->field_2 = 0;
    }
local_22:
    arg_7->field_0 = local_13;
    if (!local_13)
    {
        short *local_23 = (short *)(g_4f55f0->data + (arg_1 & 0xffff) * 0x888 + 0x5b4);
        if (++*local_23 > 32) *local_23 = 0;
    }
    return arg_7->field_0;
}
