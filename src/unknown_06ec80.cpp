// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_059ad0.h"
#include "unknown_058dd0.h"
#include "globals.h"
#include <string.h>
#include <xtl.h>

struct s_game_options;
void function_138110(s_game_options *options);
bool __stdcall function_138180(s_session_options const *options);
bool __stdcall function_18e8b0(s_session_options const *arg_0);

union s_6ec80
{
    s_session_options field_0;
    byte field_1[0x1118];
    __int64 field_2;
};

// The mask is declared where retail clears it (inside the guard), and the session's current and own
// member indices are named before their compare: together they keep the players pointer in EDI.
// @retail 0x6ec80
bool function_06ec80(c_class_58d20 *arg_0, bool arg_1)
{
    s_unknown_108 *local_2;
    short local_3;
    byte *stored_data;
    byte *name;
    long session_kind;
    long second_id;
    s_unknown_3648 *players;
    long first_id;
    long value_49c8;
    long value_4da8;
    __int64 value_4da0;
    s_session_machine machine;
    long value_18;
    bool local_0 = false;
    bool local_1 = true;
    first_id = NONE;
    second_id = NONE;
    name = 0;
    value_4da8 = NONE;
    if (session_state_is_live(arg_0))
        value_4da8 = arg_0->value4da8;
    value_4da0 = arg_0->get_values_4da0();
    stored_data = 0;
    if (session_state_is_live(arg_0))
        stored_data = arg_0->data4db0;
    local_3 = NONE;
    if (session_state_is_live(arg_0))
        local_3 = arg_0->value5dd0;
    session_kind = NONE;
    if (session_state_is_live(arg_0))
        session_kind = arg_0->value4dac;
    value_49c8 = NONE;
    value_18 = arg_0->value18;
    arg_0->get_values_4d08(&first_id, &second_id, &name);
    local_2 = 0;
    players = 0;
    if (session_state_is_live(arg_0) && arg_0->flag4f20)
    {
        local_2 = &arg_0->data4f24;
        players = &arg_0->data4f90;
    }
    if (arg_1)
    {
        value_49c8 = arg_0->get_value_49c8();
        if (value_49c8 == NONE)
            local_1 = false;
    }
    if (second_id == NONE)
        local_1 = false;
    if (!name || !*name)
        local_1 = false;
    if (!stored_data)
        local_1 = false;
    if (local_2 && players)
    {
        dword local_14 = 0;
        long local_15 = 0;
        do
        {
            if (((s_session_player *)players)[local_15].active)
                local_14 |= 1 << local_15;
            local_15++;
        } while (local_15 < 16);
        if (!local_2->data[0] || !local_14)
            local_1 = false;
    }
    else
        local_1 = false;
    if (session_kind >= 0 && session_kind < 3 && local_1)
    {
        long current_member = arg_0->current_member;
        long member_index = arg_0->member_index;
        bool local_16 = current_member == member_index;
        long local_17 = arg_0->player_count;
        const s_session_member *member = &arg_0->members[current_member];
        long local_18 = arg_0->member_count;
        machine = *(const s_session_machine *)((const byte *)member + 0xa);
        if (local_3 != NONE)
        {
            if (local_17 > 2)
                local_1 = false;
            if (local_18 > 1)
                local_1 = false;
        }
        if (local_1)
        {
            long mode;
            switch (session_kind)
            {
            case 0: mode = 1; break;
            case 1: mode = local_16 ? 3 : 2; break;
            case 2: mode = local_16 ? 5 : 4; break;
            default: __assume(0);
            }
            s_6ec80 local_21;
            function_138110((s_game_options *)&local_21);
            s_session_options *local_22 = &local_21.field_0;
            *(long *)(local_21.field_1 + 0x14) = first_id;
            *(long *)(local_21.field_1 + 0x18) = second_id;
            strncpy(local_22->name, (char *)name, sizeof(local_22->name));
            local_22->name[sizeof(local_22->name) - 1] = 0;
            local_22->unknown4 = (char)mode;
            switch (value_18)
            {
            case 0: local_22->unknown5 = 1; break;
            case 1: local_22->unknown5 = 2; break;
            case 2: local_22->unknown5 = 3; break;
            }
            *(__int64 *)(local_21.field_1 + 8) = value_4da0;
            *(long *)(local_21.field_1 + 0x10) = value_4da8;
            if (local_3 != NONE)
            {
                local_22->type = 1;
                local_22->unknown12a = local_3;
                local_22->unknown12c = local_17 > 1;
                local_22->unknown12d[0] = 0;
            }
            else
            {
                local_22->type = 2;
                memcpy(local_22->unknown134, stored_data, sizeof(local_22->unknown134));
                *(long *)(local_21.field_1 + 0x130) = value_49c8;
            }
            memcpy(&local_22->machine_mask, local_2, sizeof(*local_2));
            memcpy(local_22->players, players, sizeof(*players));
            local_22->local_machine_valid = true;
            local_22->local_machine = machine;
            g_510548 = false;
            g_51054c = GetTickCount();
            if (function_138180(local_22) && function_18e8b0(local_22))
                local_0 = true;
            g_510548 = true;
            g_51054c = GetTickCount();
        }
    }
    return local_0;
}

struct s_6ec10
{
    byte field_0[0x10];
    long field_10;
    long field_14;
};

// @retail 0x6ec10
void c_session_state::function_06ec10(c_class_58d20 *arg_0)
{
    if (function_06ec80(arg_0, false))
    {
        s_6ec10 *local_0 = (s_6ec10 *)this;
        local_0->field_10 = g_4e6948->id_a;
        local_0->field_14 = g_4e6948->id_b;
    }
    else
    {
        long local_0 = arg_0->state;
        if (local_0 == 5 || local_0 == 6 || local_0 == 7 || local_0 == 8)
            network_session_set_mode(arg_0, 1);
        else
        {
            volatile long local_1 = local_0;
            network_session_leave(arg_0, false);
        }
    }
}
