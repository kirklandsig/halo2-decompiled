#include "unknown_1af160.h"
#include "props.h"

// @flags /O2 /arch:SSE /Gr

long function_1e4990(long arg_0);
bool function_1f8660(long arg_0);
real function_1f8940(long arg_0);
void function_1f86a0(long arg_0);
long function_2003a0(long arg_0);
void __stdcall function_26c2d0(long arg_0);
short function_1af5c0(long arg_0, long arg_1, point3f const *arg_2, long arg_3);

// @retail 0x1af810
bool __stdcall function_1af810(long arg_0, s_slot *arg_1)
{
    s_actor_view *local_0 = (s_actor_view *)(g_4f55f0->data + (arg_0 & 0xffff) * sizeof(s_actor_view));
    s_1af160 *local_1 = (s_1af160 *)((byte *)arg_1 + 0xc);
    bool local_2 = true;
    if (local_0->unknown040 && !local_0->unknown267)
    {
        s_prop_node_view *local_3 = (s_prop_node_view *)(g_502418->data + (local_0->prop_index & 0xffff) * sizeof(s_prop_node_view));
        bool local_4 = false;
        dword *local_5 = (dword *)function_1e4be0(arg_0);
        if (local_5 && ((*local_5 & 2) || local_0->unknown229 && (*local_5 & 4) && local_0->unknown4ac != 4) &&
            (!function_1f8660(arg_0) || 1.0f > function_1f8940(arg_0)))
        {
            local_4 = true;
            if (!REFERENCE_EQUAL(local_0->unknown418, g_470fa0))
                function_262800(arg_0, local_0->unknown418, false);
        }
        if (local_1->field_7)
            local_4 = true;
        else if (!local_4)
        {
            byte *local_6;
            if (local_0->unknown229 && (local_6 = (byte *)function_1e4990(local_0->unknown054)) &&
                !(*local_6 & 2) && !function_1f8660(arg_0))
                local_4 = true;
            else
            {
                real local_7;
                real local_8;
                if (function_1af530(arg_0, (long *)&local_8, (long *)&local_7))
                {
                    s_type_5cfb45 *local_9 = function_25d690((s_prop_datum *)local_3);
                    real local_10 = 0.0f;
                    bool local_11 = false;
                    local_8 *= 0.66f;
                    if (function_1f8660(arg_0))
                    {
                        local_10 = function_210ac0((s_type_c3b527 const *)((byte *)local_0 + 0x4ec), &local_9->position);
                        local_11 = true;
                    }
                    if (local_3->unknown28 > local_7)
                    {
                        if (!local_11 || !(local_7 > local_10) || !(local_10 > local_8))
                            local_4 = true;
                    }
                    else if (!local_1->field_8 && local_8 > local_3->unknown28)
                    {
                        if (local_11)
                        {
                            if (!(local_7 > local_10) || !(local_10 > local_8))
                                local_4 = true;
                        }
                        else
                        {
                            s_type_f95cd3 *local_12 = function_25d740((s_prop_node *)local_3);
                            if (!local_12 || local_12->unknown64)
                                local_4 = true;
                        }
                    }
                }
                if (!local_4)
                {
                    *(byte *)&local_4 = (byte)function_2003a0(arg_0);
                    if (!local_4)
                    {
                        function_26c180(arg_0);
                        function_26c2d0(local_0->prop_index);
                        s_type_5cfb45 *local_13 = function_25d690((s_prop_datum *)local_3);
                        local_4 = *(short *)((byte *)local_0 + 0x288) != local_13->unknown54;
                    }
                }
            }
        }
        if (!local_0->unknown227 && local_3->unknown27 < 2)
            function_1f86a0(arg_0);
        if (local_4)
        {
            local_2 = function_1af160(arg_0, local_1);
        }
        else if (function_1f8660(arg_0))
        {
            point3f local_14;
            function_210850((s_type_c3b527 const *)((byte *)local_0 + 0x4ec), &local_14);
            if ((short)function_1af5c0(arg_0, local_0->prop_index, &local_14, 1) > 0)
                local_1->field_7 = true;
        }
        else if (!function_1f8660(arg_0) && local_3->unknown24 >= 1 && local_3->unknown24 <= 2 &&
            !local_0->unknown229 && !local_0->unknown5d0)
        {
            byte *local_16 = (byte *)function_1e5240(arg_0);
            long local_15 = 0;
            if (local_16 && (*local_16 & 2))
                local_15 = 2;
            local_1->field_9 = (short)function_1af5c0(arg_0, local_0->prop_index, &local_0->position, local_15) > 0;
        }
    }
    return local_2;
}
