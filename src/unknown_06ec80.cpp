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

#pragma pack(push, 1)
struct s_6ec81
{
    byte *field_8;
    byte *field_c;
    long field_10;
    long field_14;
    s_unknown_3648 *field_18;
    long field_1c;
    long field_20;
    long field_24;
    long field_28[2];
};

#pragma pack(pop)
#pragma pack(push, 1)
struct s_6ec83
{
    s_6ec81 field_0;
    byte field_30[4];
    s_session_machine field_34;
    byte field_3a[2];
    long field_3c;
};
#pragma pack(pop)

// score 13 (was 53): the two stack slots are swapped: retail keeps the short at esp+0x14 and
// the pointer at esp+0x10, ours the other way round; the mask is zeroed before the guard.
// The mask scoped inside the guard gives both retail's slots and its zeroing, but then NONE
// and zero take edi and ecx (retail ecx and esi) and the machines pointer is not kept in ebx:
// 223, with the declarations moved 223/223/223/223, with a for loop 348, the counter declared
// first 223, the mask declared first 273. Machines pointer and short as settings fields 171
// (slots right, the short's first store not widened); the existing pair getter for the 4da0
// values 72 (227 scoped); out-parameter getters 13; renamed locals 13.
// Round 32: pointer/short declarations swapped 13, both declared after the settings block 13 (local_9
// for the pointer or local_10 for the short: 13 each, so the slot order is not by name); the short's
// NONE as its declaration's initialiser 42, the pointer's 0 262 (both, harness, no gain); mask scoped
// in an inverted guard 223; with the mask scoped: a players pointer local, the counter declared first,
// the mask declared at the top and zeroed in the guard (harness, 217-219 positional, no gain).
// @retail 0x6ec80
bool function_06ec80(c_class_58d20 *arg_0, bool arg_1)
{
    /* the session's settings, gathered in one block before the options are built */
    s_unknown_108 *local_2;
    short local_3;
    s_6ec83 local_26;
    bool local_0 = false;
    bool local_1 = true;
    local_26.field_0.field_1c = NONE;
    local_26.field_0.field_14 = NONE;
    local_26.field_0.field_c = 0;
    local_26.field_0.field_24 = NONE;
    if (session_state_is_live(arg_0))
        local_26.field_0.field_24 = arg_0->value4da8;
    local_26.field_0.field_28[0] = NONE;
    local_26.field_0.field_28[1] = NONE;
    if (session_state_is_live(arg_0))
    {
        local_26.field_0.field_28[0] = arg_0->value4da0;
        local_26.field_0.field_28[1] = arg_0->value4da4;
    }
    local_26.field_0.field_8 = 0;
    if (session_state_is_live(arg_0))
        local_26.field_0.field_8 = arg_0->data4db0;
    local_3 = NONE;
    if (session_state_is_live(arg_0))
        local_3 = arg_0->value5dd0;
    local_26.field_0.field_10 = NONE;
    if (session_state_is_live(arg_0))
        local_26.field_0.field_10 = arg_0->value4dac;
    local_26.field_0.field_20 = NONE;
    local_26.field_3c = arg_0->value18;
    arg_0->get_values_4d08(&local_26.field_0.field_1c, &local_26.field_0.field_14, &local_26.field_0.field_c);
    local_2 = 0;
    local_26.field_0.field_18 = 0;
    if (session_state_is_live(arg_0) && arg_0->flag4f20)
    {
        local_2 = &arg_0->data4f24;
        local_26.field_0.field_18 = &arg_0->data4f90;
    }
    if (arg_1)
    {
        local_26.field_0.field_20 = arg_0->get_value_49c8();
        if (local_26.field_0.field_20 == NONE)
            local_1 = false;
    }
    if (local_26.field_0.field_14 == NONE)
        local_1 = false;
    if (!local_26.field_0.field_c || !*local_26.field_0.field_c)
        local_1 = false;
    if (!local_26.field_0.field_8)
        local_1 = false;
    dword local_14 = 0;
    if (local_2 && local_26.field_0.field_18)
    {
        long local_15 = 0;
        do
        {
            if (((s_session_player *)local_26.field_0.field_18)[local_15].active)
                local_14 |= 1 << local_15;
            local_15++;
        } while (local_15 < 16);
        if (!local_2->data[0] || !local_14)
            local_1 = false;
    }
    else
        local_1 = false;
    if (local_26.field_0.field_10 >= 0 && local_26.field_0.field_10 < 3 && local_1)
    {
        bool local_16 = arg_0->current_member == arg_0->member_index;
        long local_17 = arg_0->player_count;
        const s_session_member *member = &arg_0->members[arg_0->current_member];
        long local_18 = arg_0->member_count;
        local_26.field_34 = *(const s_session_machine *)((const byte *)member + 0xa);
        if (local_3 != NONE)
        {
            if (local_17 > 2)
                local_1 = false;
            if (local_18 > 1)
                goto local_23;
        }
        if (local_1)
        {
            long mode;
            switch (local_26.field_0.field_10)
            {
            case 0: mode = 1; break;
            case 1: mode = local_16 ? 3 : 2; break;
            case 2: mode = local_16 ? 5 : 4; break;
            default: __assume(0);
            }
            s_6ec80 local_21;
            function_138110((s_game_options *)&local_21);
            s_session_options *local_22 = &local_21.field_0;
            *(long *)(local_21.field_1 + 0x14) = local_26.field_0.field_1c;
            *(long *)(local_21.field_1 + 0x18) = local_26.field_0.field_14;
            strncpy(local_22->name, (char *)local_26.field_0.field_c, sizeof(local_22->name));
            local_22->name[sizeof(local_22->name) - 1] = 0;
            local_22->unknown4 = (char)mode;
            switch (local_26.field_3c)
            {
            case 0: local_22->unknown5 = 1; break;
            case 1: local_22->unknown5 = 2; break;
            case 2: local_22->unknown5 = 3; break;
            }
            *(long *)(local_21.field_1 + 8) = local_26.field_0.field_28[0];
            *(long *)(local_21.field_1 + 0xc) = local_26.field_0.field_28[1];
            *(long *)(local_21.field_1 + 0x10) = local_26.field_0.field_24;
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
                memcpy(local_22->unknown134, local_26.field_0.field_8, sizeof(local_22->unknown134));
                *(long *)(local_21.field_1 + 0x130) = local_26.field_0.field_20;
            }
            memcpy(&local_22->machine_mask, local_2, sizeof(*local_2));
            memcpy(local_22->players, local_26.field_0.field_18, sizeof(*local_26.field_0.field_18));
            local_22->local_machine_valid = true;
            local_22->local_machine = local_26.field_34;
            g_510548 = false;
            g_51054c = GetTickCount();
            if (function_138180(local_22) && function_18e8b0(local_22))
                local_0 = true;
            g_510548 = true;
            g_51054c = GetTickCount();
        }
    }
local_23:
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
