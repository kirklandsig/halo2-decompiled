#include "unknown_11c920.h"
#include "globals.h"
#include "object_markers.h"
#include "unknown_11cc90.h"

// @flags /O2 /arch:SSE /Gr

struct s_203780
{
    long field_0;
    point3f field_4;
    short field_10;
    byte field_12[2];
    vector2f field_14;
    byte field_1c[4];
    short field_20;
    byte field_22[0x64 - 0x22];
};

vector2f *function_11df30(vector2f *arg_0, vector3f const *arg_1);
void function_bf380(void);
long __stdcall function_1e0850(long arg_0, long arg_1, s_203780 const *arg_2, bool arg_3);

// @retail 0x203780
long function_203780(long arg_0, short arg_1, short arg_2, bool arg_3)
{
    long local_0 = NONE;
    if (arg_1 != NONE)
    {
        byte *local_3 = *(byte **)((byte *)g_4e0350 + 0x164) + (word)arg_2 * 0x74;
        s_203780 *local_4 = (s_203780 *)(*(byte **)(local_3 + 0x4c) + arg_1 * 0x64);
        word local_5 = local_4->field_20;
        short local_10;
        if ((short)local_5 != NONE)
            local_10 = (short)local_5;
        else
            local_10 = *(short *)(local_3 + 0x36);
        if (local_10 >= 0 && local_10 < *(long *)((byte *)g_4e0350 + 0x178))
        {
            long *local_6 = (long *)(*(byte **)((byte *)g_4e0350 + 0x17c) + local_10 * 8);
            if (local_6[1] != NONE)
            {
                s_203780 local_7;
                s_203780 const *local_8;
                s_object_marker local_9;
                if (arg_0 != NONE && function_b8d30(arg_0, 0x700061d, &local_9, 1, false) > 0)
                {
                    local_7 = *local_4;
                    local_7.field_4 = local_9.matrix.position;
                    local_7.field_10 = NONE;
                    function_11df30(&local_7.field_14, &local_9.matrix.forward);
                    local_8 = &local_7;
                }
                else
                    local_8 = local_4;
                if (arg_3)
                    function_bf380();
                long actor_index = function_1e0850(local_6[1], arg_2, local_8, true);
                if (actor_index != NONE)
                {
                    *(long *)(g_4f55f0->data + (actor_index & 0xffff) * 0x888 + 0x38) = local_4->field_0;
                    local_0 = actor_index;
                }
            }
        }
    }
    return local_0;
}
