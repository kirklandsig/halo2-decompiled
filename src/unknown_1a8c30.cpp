#include "unknown_11c920.h"
#include "ai_actor.h"
#include "unknown_0259a0.h"

// @flags /O2 /arch:SSE /Gr

bool function_1f50e0(long arg_0);
real function_11e000(point3f const *arg_0, point3f const *arg_1, vector3f const *arg_2);
real function_11e6f0(point3f const *arg_0, vector3f const *arg_1, point3f const *arg_2, real arg_3);
void function_1a8fa0(long arg_0, real arg_1, bool *arg_2, bool *arg_3, bool *arg_4);
bool __stdcall function_1a83e0(long arg_0, short *arg_1, real *arg_2, point2f *arg_3, bool *arg_4);
bool function_1a8a10(long arg_0, point2f const *arg_1, short arg_2, real arg_3, real arg_4);
void *function_1e4c70(long arg_0);

struct s_1a8c30
{
    byte field_0[0x10];
    real field_10;
};

#pragma inline_depth(0)
// The radius square happens after the call to function_11e000, the flag is set
// before it and the second switch assigns the flag from the condition, as in
// retail. Register arguments are set up right to left in declared order, so
// retail's setup means 0x11e6f0 takes (origin, direction, center, radius) and
// 0x1a8a10 (actor index, point, ...); with both orders this function matches.
// @retail 0x1a8c30
short __stdcall function_1a8c30(long arg_0, s_slot *arg_1)
{
    volatile short local_1 = g_46fbe4;
    s_actor_view *local_0 = (s_actor_view *)(g_4f55f0->data + (arg_0 & 0xffff) * sizeof(s_actor_view));
    if (local_0->unknown358 == 0 ||
        ((s_prop_node_view *)g_502418->data)[local_0->unknown368 & 0xffff].unknown24 < 1 || local_0->unknown35e)
        return (short)local_1;
    real local_2 = local_0->unknown39c + 3.0f;
    if (local_0->unknown398 > local_2 * local_2 || function_1f50e0(arg_0) || local_0->unknown018 == NONE)
        return (short)local_1;
    vector3f local_3;
    point3f *local_17 = &local_0->unknown370;
    local_3.i = *(real *)((byte *)local_0 + 0x388) - local_17->x;
    local_3.j = *(real *)((byte *)local_0 + 0x38c) - local_17->y;
    local_3.k = *(real *)((byte *)local_0 + 0x390) - local_17->z;
    real local_4 = local_0->unknown36c;
    real local_15 = function_11e000(&local_0->position, local_17, &local_3);
    bool local_5 = local_15 < local_4 * local_4;
    function_1a8fa0(arg_0, 0.0f, NULL, &local_5, NULL);
    if (local_5)
    {
        bool local_7 = false;
        real local_6 = function_11e6f0(&local_0->unknown370, &local_3, &local_0->position, local_0->unknown36c);
        if (local_6 < 3.402823466e38f)
            local_6 *= 1.5f;
        switch (local_0->unknown358)
        {
        case 2:
            if (local_6 <= 0.0f && *(short *)((byte *)local_0 + 0x3ac) != NONE &&
                0.66f > (real)*(short *)((byte *)local_0 + 0x3ac) * g_510c54->rate)
                local_7 = true;
            break;
        case 4:
            if (local_6 <= 0.0f && *(short *)((byte *)local_0 + 0x3ac) != NONE &&
                0.66f > (real)*(short *)((byte *)local_0 + 0x3ac) * g_510c54->rate)
                local_7 = true;
            break;
        case 1:
            if (local_6 <= 0.0f && *(short *)((byte *)local_0 + 0x3ac) != NONE &&
                1.0f > (real)*(short *)((byte *)local_0 + 0x3ac) * g_510c54->rate)
                local_7 = true;
            break;
        case 3:
            if (1.0f > local_6)
                local_7 = true;
            break;
        }
        long local_8;
        real local_9;
        point2f local_10;
        bool local_11;
        bool local_12 = function_1a83e0(arg_0, (short *)&local_8, &local_9, &local_10, &local_11);
        if ((short)local_8 != NONE && *(bool *)((byte *)local_0 + 0x35c) && local_0->unknown26c == NONE)
        {
            bool local_13 = false;
            switch (local_0->unknown358)
            {
            case 2:
            case 4:
                local_13 = (local_12 || local_11) && 0.25f > local_6;
                break;
            case 1:
                if ((local_12 || local_11) && local_6 <= 0.0f)
                    local_13 = true;
                break;
            case 3:
                break;
            default:
                goto local_done;
            }
            if ((local_13 || local_7) && function_1a8a10(arg_0, &local_10, (short)local_8, local_9, 0.0f))
            {
                s_1a8c30 *local_14 = (s_1a8c30 *)function_1e4c70(arg_0);
                function_1fb7e0(0x24, arg_0, NULL, NONE, NONE);
                if (local_14 && local_14->field_10 > 0.0f &&
                    local_14->field_10 > function_259a0(&g_4e7408->unknown0))
                    local_1 = 0x38;
            }
        }
    }
local_done:
    return (short)local_1;
}
#pragma inline_depth(255)
