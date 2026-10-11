#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "unknown_2626b0.h"
#include "unknown_20fe20.h"
// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_collision_result_1697c0;
bool __stdcall function_1697c0(long, point3f const *, vector3f const *,
    long, long, s_collision_result_1697c0 *);

struct s_29dd50
{
    long field_0;
    real field_4;
    byte field_8[0x24 - 8];
    short field_24;
    byte field_26[0x5c - 0x26];
};

// @retail 0x29dd50
bool function_29dd50(point3f const *arg_0, point3f const *arg_1, long arg_2, long arg_3)
{
    s_29dd50 local_1;
    local_1.field_24 = NONE;
    bool local_2 = true;
    vector3f local_0;
    local_0.i = arg_1->x - arg_0->x;
    local_0.j = arg_1->y - arg_0->y;
    local_0.k = arg_1->z - arg_0->z;
    if (function_1697c0(0x1808c2d, arg_0, &local_0, arg_2, arg_3,
        (s_collision_result_1697c0 *)&local_1) && !(local_1.field_4 >= 1.0f))
    {
        real local_3 = arg_1->x - arg_0->x;
        real local_4 = arg_1->y - arg_0->y;
        real local_5 = arg_1->z - arg_0->z;
        real local_6 = 1.0f - local_1.field_4;
        if (! (0.1f > (local_4 * local_4 + (local_3 * local_3 + local_5 * local_5)) * (local_6 * local_6)))
            local_2 = false;
    }
    return local_2;
}

struct s_29de20
{
    short field_0;
    short field_2;
    short field_4;
};
struct s_29de21
{
    s_29de20 field_0[30];
    short field_b4;
};
struct s_29de22 { long field_0; point3f *field_4; };
struct s_29de23 { byte field_0[0x44]; s_29de22 *field_44; };
struct s_29de24 { byte field_0[0x6c]; long field_6c; s_29de23 *field_70; };
struct s_29de25 { byte field_0[0xc4]; long field_c4; s_29de24 *field_c8; };

// @retail 0x29de20
short function_29de20(short arg_1, s_29de21 *arg_0, point3f const *arg_2, long arg_3)
{
    s_29de20 *local_0 = &arg_0->field_0[arg_1];
    short local_3 = local_0->field_4;
    if (local_3 == 0)
    {
        s_29de25 *local_1 = (s_29de25 *)g_4e0348;
        s_29de24 *local_2 = NULL;
        if (local_1->field_c4 > 0) local_2 = local_1->field_c8;
        local_3 = 2;
        if (local_2 && local_2->field_6c > 0)
        {
            point3f *local_4 = &local_2->field_70->field_44[local_0->field_0].field_4[local_0->field_2];
            local_3 = function_29dd50(arg_2, local_4, arg_3, NONE) ? 1 : 2;
        }
        local_0->field_4 = local_3;
    }
    return local_3;
}

// @retail 0x29de90
bool __stdcall function_29de90(long arg_0, point3f const *arg_1, long arg_2, s_29de21 *arg_3)
{
    (void)&arg_0;
    bool local_0 = false;
    bool local_1 = false;
    for (short local_2 = 0; local_2 < arg_3->field_b4; ++local_2)
    {
        if (arg_3->field_0[local_2].field_0 == arg_0)
        {
            local_1 = true;
            if (arg_3->field_0[local_2].field_4 == 1)
                local_0 = true;
        }
    }
    if (!local_0 && local_1)
    {
        for (short local_3 = 0; local_3 < arg_3->field_b4; ++local_3)
            if (arg_3->field_0[local_3].field_0 == arg_0 &&
                function_29de20(local_3, arg_3, arg_1, arg_2) == 1)
            {
                local_0 = true;
                break;
            }
    }
    return local_0;
}

struct s_29df30 { short field_0; short field_2; };
struct s_29df31 { byte field_0[0x80]; long field_80; s_29df30 *field_84; };
struct s_29df32 { byte field_0[0x30]; long field_30; s_29df31 *field_34; };
struct s_29df33 { byte field_0[0x168]; long field_168; s_29df32 *field_16c; };

// @retail 0x29df30
bool __stdcall function_29df30(point3f const *arg_0, long arg_1, s_reference arg_2, s_29de21 *arg_3)
{
    bool local_0 = false;
    (void)&local_0;
    short local_1 = arg_2.unknown2;
    if (!(local_1 & 0x8000))
    {
        s_262b40_result *local_2 = function_262b40(arg_2);
        if (local_2 && local_1 >= 0)
        {
            s_29df33 *local_3 = (s_29df33 *)g_4e0350;
            if (local_1 < local_3->field_168)
            {
                short local_10 = local_2->unknown10;
                s_29df32 *local_4 = &local_3->field_16c[(word)local_1];
                if (local_10 >= 0 && local_10 < local_4->field_30)
                {
                    s_29df31 *local_5 = &local_4->field_34[local_10];
                    dword local_6[2] = {0, 0};
                    dword volatile *local_9 = local_6;
                    for (short local_7 = 0; local_7 < local_5->field_80; ++local_7)
                    {
                        long local_8 = local_5->field_84[local_7].field_0;
                        if (!(local_6[local_8 >> 5] & (1 << (local_8 & 31))))
                        {
                            local_9[local_8 >> 5] |= 1 << (local_8 & 31);
                            if (function_29de90(local_8, arg_0, arg_1, arg_3))
                            {
                                local_0 = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return local_0;
}

struct s_29e050 { point3f field_0; short field_c; };
struct s_type_d4fbfa;

PRIVATE __forceinline long function_29e051(s_reference arg_0)
{
    return *(long *)&arg_0;
}

// @retail 0x29e050
bool function_29e050(byte *arg_0, long arg_1, s_type_d4fbfa *arg_2,
    s_reference arg_3, long *arg_4)
{
    bool local_4 = true;
    point3f local_0;
    s_29e050 *local_3 = (s_29e050 *)arg_2;
    if (local_3->field_c == NONE || !function_2104b0(local_3->field_c, &local_3->field_0, &local_0))
        local_0 = local_3->field_0;
    bool local_1 = !function_29dd50((point3f *)arg_0, &local_0, arg_1, NONE);
    if (local_1 && arg_4 && function_29e051(arg_3) != function_29e051(g_470fa0) &&
        !(arg_3.unknown2 & 0x8000))
        local_1 = !function_29df30((point3f *)arg_0, arg_1, arg_3, (s_29de21 *)arg_4);
    if (local_1) local_4 = false;
    return local_4;
}
