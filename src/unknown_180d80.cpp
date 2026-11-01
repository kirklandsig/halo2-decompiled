#include "unknown_11c920.h"
#include "globals.h"
#include "physical_memory.h"
#include <string.h>
// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_cluster_query
{
    byte unknown00[8];
    point3f point;
    byte unknown14[0xc];
    short cluster_index;
};
struct s_180d80
{
    point3f field_0;
    real field_c;
    real field_10;
};
struct s_180d81
{
    s_180d80 field_0[1024];
    short field_5000;
    short field_5002[1024];
    short field_5802;
};
struct s_180d82
{
    point3f field_0;
    dword field_c;
};
struct s_180d83
{
    bool field_0;
    byte field_1[7];
    real field_8;
    real field_c;
    real field_10;
};
struct s_180d84
{
    byte field_0[0x8c];
    real field_8c;
    real field_90;
    real field_94;
    real field_98;
    real field_9c;
    real field_a0;
    byte field_a4[0xec - 0xa4];
    real field_ec;
    real field_f0;
    real field_f4;
    real field_f8;
    byte field_fc[4];
    byte field_100;
};
union s_180d85
{
    color3f field_0;
    vector3f field_c;
};
real function_30bf0(vector3f *arg_0);
void *function_439d0(long arg_0);
bool __stdcall function_d47d0(s_cluster_query const *arg_0, color3f *arg_1);
color3f *function_131c20(color3f const *arg_0, color3f const *arg_1, dword arg_2, real arg_3, color3f *arg_4);
dword __cdecl pack_color3f(color3f const *arg_0);
long function_17cfa0(long first_index, long definition_index, short cluster_index, short decal_type, bool permanent);
void __stdcall function_180b60(s_cluster_query const *arg_0, real arg_1, long arg_2);
dword vector3d_pack(vector3f const *arg_0);

__forceinline void function_180d81(real const *arg_0, real const *arg_1, real *arg_2)
{
    real const volatile *local_0 = arg_0;
    real const volatile *local_1 = arg_1;
    if ((local_0[1] - local_0[0]) * *local_1 + local_0[0] < 0.0f) *arg_2 = 0.0f;
    else if ((local_0[1] - local_0[0]) * *local_1 + local_0[0] > 1.0f) *arg_2 = 1.0f;
    else *arg_2 = (local_0[1] - local_0[0]) * *local_1 + local_0[0];
}
__forceinline real function_180d82(void)
{
    g_4e7408->seed = g_4e7408->seed * 0x19660d + 0x3c6ef35f;
    return (real)(g_4e7408->seed >> 16) * 1.5259021893143654e-05f;
}
__forceinline void function_180d83(transform4x3f const *arg_0, point3f *arg_1)
{
    real local_0 = arg_1->x;
    real local_1 = arg_1->y;
    real local_2 = arg_1->z;
    real local_4;
    if (arg_0->scale != 1.0f)
    {
        local_0 *= arg_0->scale;
        local_1 *= arg_0->scale;
        local_2 *= arg_0->scale;
    }
    local_4 = arg_0->up.i * local_2;
    local_4 += local_1 * arg_0->left.i;
    local_4 += arg_0->forward.i * local_0;
    arg_1->x = local_4 + arg_0->position.x;
    local_4 = arg_0->forward.j * local_0;
    local_4 += arg_0->up.j * local_2;
    local_4 += arg_0->left.j * local_1;
    arg_1->y = local_4 + arg_0->position.y;
    local_4 = arg_0->forward.k * local_0;
    local_4 += arg_0->up.k * local_2;
    local_4 += arg_0->left.k * local_1;
    arg_1->z = local_4 + arg_0->position.z;
}

#pragma inline_depth(1)
// score 94 (was 94): BLOCKED-BY 0x17cfa0 (its first index travels on the stack, retail's in
// eax: `push -1` for `mov eax, -1` shifts every later offset by 3 bytes). A short strip-loop
// index gives retail's prologue and count loop but 547 (the loop head's padding grows to a
// jmp); local names for the count and vertex index 94; vertex index declared first 547;
// luminance terms red, blue, green 92 (other orders 94-95, grouped 290); transform rows
// with the left product first 93, operands swapped 94, one expression per row 317;
// luminance and transform changes together 91.
// early input receiver, opposite count/counter homes and
// transform carriers differ; strip equality 100, short equality 548, vertex
// equality 95, both 548, combined count rejection 647; transform references
// and packed-record memcpy 94, separate transform input/output 170.
// early input register and count homes remain; normalizer inline 94; pack noinline 94; buffer inline 94; transform inline 94; nesting depth two 94; allocation byte count 169; published short count 169; short count and inline 169.
// Packed vertex published after position/delta/squared-distance/maximum 589/568/572/574; same with
// output record receiver 568/581/554/569. Original publication and receivers restored.
// Shared vertex ordinal with output base/offset 284/293; shared short/long strip ordinal 528/418;
// base/strip and offset/strip combinations 531/405/533/415. Restored.
// Round 418: result before array 631; result after array 631; result after guard 651; result assigned at
// exits 106; short count/result before array 648; short count/result after array 648; short count/result
// after guard 668; short count/result at exits 546. early mesh receiver, count/counter homes and
// creation-call setup remain.
// Typed tag/output/both 94/94/94; complete direct tag raw/typed 661/661; complete direct output
// 608; both direct raw/typed 698/698. Original observations and record layouts restored.
// Round 51: count before tag/vector 95; count initialization split 94; unsigned long polygon counter 94;
// short counter with count before tag/vector 548; with split count 547; with count between tag/vector 547;
// with outer maximum distance 547; with both count placement and outer distance 548. Original restored.
// @retail 0x180d80
long __stdcall function_180d80(s_180d84 const *arg_0, transform4x3f const *arg_1, long arg_2, s_cluster_query const *arg_3, bool arg_4, s_180d81 const *arg_5, s_180d83 *arg_6)
{
    s_180d82 local_0[1024];
    if (arg_5->field_5802 <= 0 || arg_5->field_5000 <= 0 || arg_3->cluster_index == NONE) return NONE;
    {
        byte const *local_2 = (byte const *)g_4e3b44[arg_2 & 0xffff].data;
        vector3f local_3 = {0.0f, 0.0f, 0.0f};
        long local_4 = 0;
        if (arg_0->field_90 - arg_0->field_8c <= 0.5f &&
            arg_0->field_98 - arg_0->field_94 <= 0.5f &&
            arg_0->field_a0 - arg_0->field_9c <= 0.5f)
        {
            local_3.i = arg_0->field_90 + arg_0->field_8c;
            local_3.j = arg_0->field_98 + arg_0->field_94;
            local_3.k = arg_0->field_a0 + arg_0->field_9c;
            function_30bf0(&local_3);
            local_3.i *= 0.00390625f;
            local_3.j *= 0.00390625f;
            local_3.k *= 0.00390625f;
        }
        for (long local_5 = 0; local_5 < arg_5->field_5802; local_5++)
            local_4 += (arg_5->field_5002[local_5] - 1) / 2;
        long local_5 = function_13d370((s_physical_object *)g_509448, (short)local_4 * 0x40, 1);
        if (local_5 != NONE)
        {
            long local_6 = function_17cfa0(NONE, local_5, arg_3->cluster_index, *(short *)(local_2 + 4), arg_4);
            if (local_6 != NONE)
            {
                byte *local_7 = g_4ea950->data + (local_6 & 0xffff) * 0x40;
                s_180d82 *local_8 = (s_180d82 *)function_439d0(local_5);
                if (local_8)
                {
                    real local_9 = 0.0f;
                    real local_10 = 1.0f;
                    s_180d85 local_11;
                    if ((*(short *)(local_2 + 4) == 0 || *(short *)(local_2 + 4) == 1) && function_d47d0(arg_3, &local_11.field_0))
                        local_10 = local_11.field_0.green * 0.5870000123977661f + local_11.field_0.blue * 0.11400000005960464f + local_11.field_0.red * 0.29899999499320984f;
                    short local_12 = 0;
                    local_11.field_c.k = local_10;
                    for (; local_12 < arg_5->field_5000; local_12++)
                    {
                        s_180d80 const *local_13 = &arg_5->field_0[local_12];
                        function_180d81(&arg_0->field_ec, &local_13->field_c, &local_11.field_c.i);
                        function_180d81(&arg_0->field_f4, &local_13->field_10, &local_11.field_c.j);
                        local_0[local_12].field_c = vector3d_pack(&local_11.field_c);
                        local_0[local_12].field_0.x = local_13->field_0.x + local_3.i;
                        local_0[local_12].field_0.y = local_13->field_0.y + local_3.j;
                        local_0[local_12].field_0.z = local_13->field_0.z + local_3.k;
                        real local_17 = local_0[local_12].field_0.x - arg_3->point.x;
                        real local_15 = local_0[local_12].field_0.y - arg_3->point.y;
                        real local_16 = local_0[local_12].field_0.z - arg_3->point.z;
                        real local_18 = local_16 * local_16 + local_15 * local_15 + local_17 * local_17;
                        if (local_12 == 0) local_9 = local_18;
                        else if (local_18 > local_9) local_9 = local_18;
                    }
                    *(point3f *)(local_7 + 0xc) = arg_3->point;
                    *(long *)(local_7 + 0x18) = g_510c54->game_time;
                    local_7[0x29] = arg_0->field_100;
                    if (!arg_6->field_0)
                    {
                        arg_6->field_c = function_180d82();
                        arg_6->field_10 = function_180d82();
                    }
                    *(real *)(local_7 + 0x1c) = (*(real *)(local_2 + 0x38) - *(real *)(local_2 + 0x34)) * arg_6->field_c + *(real *)(local_2 + 0x34);
                    *(real *)(local_7 + 0x20) = (*(real *)(local_2 + 0x40) - *(real *)(local_2 + 0x3c)) * arg_6->field_10 + *(real *)(local_2 + 0x3c);
                    *(short *)(local_7 + 0x2a) = (short)local_4;
                    *(long *)(local_7 + 0x2c) = arg_2;
                    if (!arg_6->field_0) arg_6->field_8 = function_180d82();
                    function_131c20((color3f *)(local_2 + 0x1c), (color3f *)(local_2 + 0x28), ((dword)local_2[0] >> 1) & 3, arg_6->field_8, &local_11.field_0);
                    *(dword *)(local_7 + 0x24) = pack_color3f(&local_11.field_0);
                    local_7[0x27] = 0xff;
                    short local_13 = 0;
                    for (short local_14 = 0; local_14 < arg_5->field_5802; local_14++)
                    {
                        short local_15 = arg_5->field_5002[local_14];
                        for (short local_16 = 1; local_16 + 1 < local_15; local_16 += 2)
                        {
                            local_8[0] = local_0[local_13];
                            local_8[1] = local_0[local_13 + local_16];
                            local_8[2] = local_0[local_13 + local_16 + 1];
                            local_8[3] = local_0[local_16 + 2 >= local_15 ? local_13 : local_13 + local_16 + 2];
                            if (arg_1)
                            {
                                function_180d83(arg_1, &local_8[0].field_0);
                                function_180d83(arg_1, &local_8[1].field_0);
                                function_180d83(arg_1, &local_8[2].field_0);
                                function_180d83(arg_1, &local_8[3].field_0);
                            }
                            local_8 += 4;
                        }
                        local_13 += local_15;
                    }
                    arg_6->field_0 = (local_2[0] & 1) != 0;
                    long local_1 = *(long *)(local_2 + 0xc);
                    function_180b60(arg_3, local_9, local_6);
                    return local_1;
                }
            }
            ((s_physical_object *)g_509448)->block_delete(local_5);
        }
    }
    return NONE;
}
#pragma inline_depth(255)
