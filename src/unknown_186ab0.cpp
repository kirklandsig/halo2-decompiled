#include "unknown_4e8c38.h"
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_11cc90.h"
#include "network_message_types.h"
#include <math.h>
#include <string.h>
// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;
struct s_186ab0
{
    byte field_0[0x24];
    long field_24;
    short field_28;
    byte field_2a[0x21c - 0x2a];
};
struct s_186ab1
{
    long field_0;
    dword field_4;
    short field_8;
    byte field_a[6];
    real field_10;
    real field_14;
    vector2f field_18; /* a pair of angles */
    real field_20;
    real field_24;
    short field_28;
    char field_2a;
    char field_2b;
    short field_2c;
    short field_2e;
    bool field_30;
    char field_31;
    byte field_32[0x44 - 0x32];
    byte field_44[0x24];
    bool field_68;
    byte field_69[3];
    byte field_6c[0xc];
    byte field_78[0x89 - 0x78];
    bool field_89;
    short field_8a;
    short field_8c;
    byte field_8e[3];
    bool field_91;
    bool field_92;
    byte field_93;
};
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
    vector2f field_0; /* a pair of angles */
    real field_8;
    real field_c;
    real field_10;
    real field_14;
    dword field_18;
    byte field_1c;
    byte field_1d[3];
    word field_20;
    byte field_22[6];
    s_186ab2 field_28;
};
struct s_186ab4
{
    void *field_0;
    byte *field_4;
};
struct s_186ab5
{
    byte field_0[0x14];
    long field_14;
    byte field_18[0x168 - 0x18];
    real field_168;
    real field_16c;
    real field_170;
    byte field_174[0x212 - 0x174];
    char field_212;
    char field_213;
    short field_214;
    byte field_216[0x23d - 0x216];
    char field_23d;
    char field_23e[2];
    byte field_240[0x348 - 0x240];
    word field_348;
};
struct s_186ab6
{
    word field_0;
    byte field_2;
    byte field_3;
    long field_4;
    s_186ab5 *field_8;
};

#pragma pack(push, 1)
struct s_186ab7
{
    dword field_0;
    real field_4;
    real field_8;
    real field_c;
    real field_10;
    real field_14;
    real field_18;
    word field_1c;
    long field_1e;
    word field_22;
    word field_24;
    word field_26;
    byte field_28[0xc];
    byte field_34[0x24];
    byte field_58[4];
};

#pragma pack(pop)
struct s_186ab8
{
    dword field_0;
    dword field_4;
    dword field_8;
};
__forceinline void function_186ab8(s_186ab8 *arg_0, s_186ab8 const *arg_1)
{
    *arg_0 = *arg_1;
}
void __stdcall function_185be0(long arg_0, long arg_1, real arg_2, real arg_3, s_186ab4 *arg_4, s_186ab3 *arg_5);
void __stdcall function_151110(long arg_0, void *arg_1);
bool __stdcall function_148fff(long arg_0);
bool function_12be90(void);
short __stdcall function_cdeb0(long arg_0, long arg_1, short arg_2, short arg_3);
short function_cfe40(long arg_0);
short function_cdff0(long arg_0, short arg_1);
bool function_c85c0(long arg_0);
short function_c8960(long arg_0, short arg_1);
long function_155760(long arg_0);
void __stdcall function_187510(long arg_0, real arg_1, real arg_2);
void player_action_initialize(s_player_action *arg_0);

#pragma inline_depth(0)
// @retail 0x186ab0
void __stdcall function_186ab0(long arg_0, real arg_1, real arg_2, s_player_action *arg_3)
{
    s_186ab0 *local_0 = (s_186ab0 *)g_4e8c24->data + (arg_0 & 0xffff);
    s_186ab1 *local_1 = (s_186ab1 *)((byte *)g_4ed284 + 0x14) + local_0->field_28;
    s_186ab4 local_2;
    s_186ab3 local_3;
    byte *local_5 = (byte *)g_4e034c;
    local_2.field_4 = *(byte **)(local_5 + 0xf4);
    local_2.field_0 = NULL;
    function_185be0(local_0->field_24, local_0->field_28, arg_1, arg_2, &local_2, &local_3);
    byte reset = function_148fff(local_0->field_24) | function_12be90() | ((byte *)g_4e8c44)[local_0->field_28 * 0x140 + 0x56];
    if (reset)
    {
        bool local_4 = (local_3.field_20 >> 8) & 1;
        memset(&local_3, 0, sizeof(local_3));
        local_3.field_28.field_1c = 0.0f;
        local_3.field_28.field_20 = 0.0f;
        local_3.field_28.field_0 = NONE;
        local_3.field_28.field_4 = NONE;
        local_3.field_28.field_8 = NONE;
        local_3.field_28.field_18 = 0;
        if (local_4)
            local_3.field_20 |= 0x100;
        else
            local_3.field_20 &= ~0x100;
    }
    if (local_1->field_0 != NONE)
    {
        s_186ab5 *local_4 = ((s_186ab6 *)g_4e0300->data)[local_1->field_0 & 0xffff].field_8;
        if (local_1->field_28 != local_4->field_214)
            *(long *)&local_1->field_28 = *(long *)&local_4->field_214;
        short local_5 = (local_3.field_1c & 1) != 0;
        char local_6 = local_1->field_2a;
        if (local_4->field_213 != NONE)
            local_5 = false;
        if (local_6 == NONE)
            local_6 = local_4->field_212;
        if (local_5 || local_6 == NONE)
            local_1->field_2a = (char)function_cdeb0(local_1->field_0, NONE, local_6, local_5);
        short local_7 = function_cfe40(local_1->field_0);
        if (local_7 != NONE && local_7 != local_4->field_212)
        {
            local_1->field_2a = (char)local_7;
            local_1->field_2b = NONE;
        }
        if (local_1->field_2a == local_1->field_2b && local_1->field_2a != NONE)
            local_1->field_2b = NONE;
        if (local_1->field_2c == NONE ||
            !(short)((s_186ab6 *)g_4e0300->data)[local_1->field_0 & 0xffff].field_8->field_23e[local_1->field_2c])
            local_1->field_2c = local_4->field_23d;
        if ((local_3.field_1c & 2) || local_1->field_2c == NONE || !function_cdff0(local_1->field_0, local_1->field_2c))
        {
            s_186ab5 *local_11 = ((s_186ab6 *)g_4e0300->data)[local_1->field_0 & 0xffff].field_8;
            short local_8 = local_1->field_2c;
            short local_9 = NONE;
            if (local_8 == NONE)
                local_8 = 0;
            short local_10 = local_8;
            do
            {
                if (local_11->field_23e[local_10] > 0)
                {
                    local_9 = local_10;
                    if (local_10 != local_8)
                        break;
                }
                local_10 = local_10 == 1 ? 0 : local_10 + 1;
            } while (local_10 != local_8);
            local_1->field_2c = local_9;
        }
        if ((!g_510c50 || !((byte *)g_510c50)[5]) && !(g_4ed284->flags10 & 1) &&
            (!g_510c54->active || !g_510c54->unknown01) && !function_155760(local_0->field_28) &&
            local_1->field_0 != NONE && function_c85c0(local_1->field_0))
        {
            if (local_3.field_1c & 4)
            {
                local_1->field_8a = local_1->field_2e;
                local_1->field_2e = function_c8960(local_1->field_0, local_1->field_2e);
                if (local_1->field_2e != NONE)
                {
                    local_1->field_89 = true;
                    local_1->field_8c = 0;
                }
            }
            else if (local_3.field_1c & 8)
            {
                if (local_1->field_89 && local_1->field_8c < 0x7f)
                    local_1->field_8c++;
            }
            else
            {
                if (local_1->field_89 && local_1->field_8c > 0xf)
                    local_1->field_2e = local_1->field_8a;
                local_1->field_89 = false;
            }
        }
        else
        {
            local_1->field_2e = NONE;
            local_1->field_89 = false;
        }
        if (!((byte *)g_4e8c44)[local_0->field_28 * 0x140 + 0x55])
            function_187510(local_0->field_28, local_3.field_10, local_3.field_14);
        if (local_4->field_14 == NONE)
        {
            bool local_14 = false;
            if ((bool)((*(dword *)&g_54e8e0[local_0->field_24] >> 4) & 1))
                local_14 = (bool)((*(dword *)((byte *)&g_54e8e0[local_0->field_24] + 0x114) >> 3) & 1);
            if (local_14 &&
                (real)fabs((double)local_1->field_18.i) > 0.5f && local_3.field_14 < 0.0001f && !local_1->field_68)
            {
                long local_9 = local_1->field_31 + 1;
                if (local_9 < 0) local_9 = 0;
                else if (local_9 > 0x7f) local_9 = 0x7f;
                local_1->field_31 = (char)local_9;
                local_1->field_30 = (short)(char)local_9 > *(short *)(local_2.field_4 + 0x6e);
            }
            else
            {
                local_1->field_31 = 0;
                local_1->field_30 = false;
            }
        }
        else
            local_1->field_30 = false;
        if ((byte *)g_4e6948 + 8 && ((byte *)g_4e6948)[0xd] != 3)
        {
            if (local_3.field_1c & 0x40)
            {
                if (!local_1->field_92)
                {
                    local_1->field_92 = true;
                    local_1->field_91 = !local_1->field_91;
                }
            }
            else
                local_1->field_92 = false;
            if (local_3.field_18 & 0x4630260)
                local_1->field_91 = false;
            if (local_1->field_91 && local_4->field_14 == NONE)
                local_3.field_18 |= 0x4000;
        }
    }
    function_151110(arg_0, local_1->field_6c);
    local_1->field_8 = local_3.field_20;
    memcpy(local_1->field_44, &local_3.field_28, sizeof(local_1->field_44));
    local_1->field_4 = local_3.field_18;
    local_1->field_18 = local_3.field_0;
    local_1->field_20 = local_3.field_8;
    local_1->field_24 = local_3.field_c;
    if (local_1->field_0 != NONE)
    {
        s_186ab6 *local_4 = (s_186ab6 *)g_4e0300->data + (local_1->field_0 & 0xffff);
        if ((1 << local_4->field_3) & 1)
        {
            s_186ab5 *local_5 = local_4->field_8;
            if ((bool)((local_5->field_348 >> 5) & 1))
            {
                local_1->field_10 = (real)atan2((double)local_5->field_16c, (double)local_5->field_168);
                local_1->field_14 = (real)atan2((double)local_5->field_170,
                    sqrt((double)local_5->field_16c * local_5->field_16c + (double)local_5->field_168 * local_5->field_168));
                local_1->field_10 = (real)fmod((double)(local_1->field_10 + 6.2831854820251465f), 6.2831854820251465);
                local_1->field_14 = (real)fmod((double)(local_1->field_14 + 9.424777984619141f), 6.2831854820251465) - 3.1415927410125732f;
                local_5->field_348 &= ~0x20;
            }
        }
    }
    player_action_initialize(arg_3);
    s_186ab7 *local_4 = (s_186ab7 *)arg_3;
    local_4->field_4 = local_1->field_10;
    local_4->field_8 = local_1->field_14;
    local_4->field_1c = local_1->field_8;
    local_4->field_0 = local_1->field_4;
    local_4->field_1e = *(long *)&local_1->field_28;
    local_4->field_22 = local_1->field_2c;
    local_4->field_24 = local_1->field_2e;
    local_4->field_c = local_1->field_18.i;
    local_4->field_10 = local_1->field_18.j;
    local_4->field_14 = local_1->field_20;
    local_4->field_18 = local_1->field_24;
    s_186ab8 const *local_11 = (s_186ab8 const *)local_1->field_6c;
    s_186ab8 *local_12 = (s_186ab8 *)local_4->field_28;
    *local_12 = *local_11;
    memcpy(local_4->field_34, local_1->field_44, sizeof(local_4->field_34));
}
#pragma inline_depth(255)
