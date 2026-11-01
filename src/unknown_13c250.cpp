// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1e1f20.h"

struct s_effect_owner;
void function_b7930(void *arg_1, long arg_2, long arg_3, s_effect_owner const *arg_4);
long __stdcall function_b7b40(void *arg_1);
void __stdcall function_b8ee0(long arg_1, long arg_2, long arg_3, long arg_4);
void function_b7680(long arg_1, real arg_2, real arg_3);
long function_cbe60(long arg_1);
long players_first_active_local_player(void);
bool function_14ddc0(long local_player_index);
long function_14de70(long local_player_index);

struct s_13c250
{
    byte field_0[3];
    byte field_3;
    byte field_4[4];
    byte *field_8;
};

struct s_13c251
{
    byte field_0[0x10a];
    word field_10a0 : 1;
    word field_10a1 : 1;
    word field_10a2 : 1;
    word field_10a3 : 13;
};

__forceinline byte *function_13c251(s_record_pool *arg_1, long arg_2)
{
    return ((s_13c250 *)arg_1->data)[arg_2 & 0xffff].field_8;
}

struct s_player_13c250
{
    byte unknown000[0x2c];
    long unit_index;
    byte unknown030[0x21c - 0x30];
};

__forceinline bool function_13c253(s_13c251 const *arg_1)
{
    return TEST_FIELD_BIT(arg_1->field_10a2);
}

/* the weapon in one of a unit's weapon slots */
static __forceinline long unit_weapon_in_slot(s_unit_weapon_view const *unit, short slot)
{
    long result = NONE;
    if (slot != NONE)
        result = unit->weapon_indices[slot];
    return result;
}

/* the tags an effect may be chosen from, at most four */
struct s_tag_candidates
{
    long count;
    long tags[4];
};

/* picks a tag by its group from the first local player's unit's weapons, else the scenario's two
   fallback tags, else the object's own definition entries, and hands it to 0xb7930/0xb8ee0 */
// @retail 0x13c250
void __stdcall function_13c250(long arg_1, long arg_2, long arg_3)
{
    s_tag_candidates candidates;
    byte local_4[0xc4];
    if (arg_1 != NONE)
    {
        candidates.count = 0;
        long local_6 = players_first_active_local_player();
        for (; local_6 != NONE; local_6 = local_player_next_inline(local_6))
        {
            if (function_14ddc0(local_6))
            {
                long local_8 = ((s_player_13c250 *)g_4e8c24->data)[function_14de70(local_6) & 0xffff].unit_index;
                if (local_8 != NONE && !function_13c253((s_13c251 *)function_13c251(g_4e0300, local_8)))
                {
                    long local_11 = unit_get_current_weapon(local_8);
                    /* the unit looked up again for the second slot, as 0x82d20 does (retail reloads it) */
                    long local_13 = unit_weapon_in_slot(unit_weapon_view_get(local_8), unit_weapon_view_get(local_8)->next_weapon_index);
                    long local_14 = function_cbe60(local_8);
                    if (local_11 != NONE)
                    {
                        long position = candidates.count++;
                        candidates.tags[position] = *(long *)function_13c251(g_4e0300, local_11);
                    }
                    if (local_13 != NONE)
                    {
                        long position = candidates.count++;
                        candidates.tags[position] = *(long *)function_13c251(g_4e0300, local_13);
                    }
                    if (local_14 != NONE)
                    {
                        long position = candidates.count++;
                        candidates.tags[position] = *(long *)function_13c251(g_4e0300, local_14);
                    }
                    break;
                }
            }
        }
        if (candidates.count == 0)
        {
            byte *local_15 = (byte *)g_4e0350;
            if (*(long *)(local_15 + 0xf8) > 0)
            {
                byte *local_16 = *(byte **)(local_15 + 0xfc);
                long local_17 = *(long *)(local_16 + 0x2c);
                if (local_17 != NONE)
                {
                    long position = candidates.count++;
                    candidates.tags[position] = local_17;
                }
                local_17 = *(long *)(local_16 + 0x38);
                if (local_17 != NONE)
                {
                    long position = candidates.count++;
                    candidates.tags[position] = local_17;
                }
            }
            if (candidates.count == 0)
            {
                s_13c250 *local_18 = &((s_13c250 *)g_4e0300->data)[arg_1 & 0xffff];
                if (local_18->field_3 == 0)
                {
                    byte *local_19 = (byte *)g_4e3b44[*(long *)local_18->field_8 & 0xffff].bytes;
                    long local_20 = *(long *)(local_19 + 0x1c0);
                    for (long local_21 = 0; local_21 < local_20; local_21++)
                    {
                        long local_22 = *(long *)(*(byte **)(local_19 + 0x1c4) + local_21 * 8 + 4);
                        if (local_22 != NONE)
                        {
                            long position = candidates.count++;
                            candidates.tags[position] = local_22;
                        }
                    }
                }
            }
        }
        long local_23 = NONE;
        long best = NONE;
        long local_24 = 0x7fffffff;
        for (long index = 0; index < candidates.count; index++)
        {
            long local_26 = candidates.tags[index];
            long local_28;
            switch (*(long *)((byte *)g_4e3b44[local_26 & 0xffff].bytes + 0x288))
            {
            case 0x500000a:
            case 0x5000722:
                local_28 = 2;
                break;
            case 0x6000721:
                local_28 = 1;
                break;
            case 0x700071e:
            case 0x700071f:
                local_28 = 3;
                break;
            case 0x5000720:
                local_23 = local_26;
                goto local_29;
            default:
                local_28 = 4;
                break;
            }
            if (best == NONE || local_28 < local_24)
            {
                local_24 = local_28;
                best = local_26;
            }
        }
local_29:
        if (local_23 == NONE)
            local_23 = best;
        if (local_23 != NONE)
        {
            function_b7930(local_4, local_23, arg_1, 0);
            long local_30 = function_b7b40(local_4);
            if (local_30 != NONE)
            {
                function_b8ee0(arg_1, arg_2, local_30, arg_3);
                function_b7680(local_30, 1.0f, 0.0f);
            }
        }
    }
}
