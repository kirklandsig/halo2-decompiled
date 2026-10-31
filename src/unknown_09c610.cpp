#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0a58d0.h"
#include "event_definitions.h"
#include <string.h>

// @flags /O2 /arch:SSE /Gr

__declspec(noinline) void *function_122c10(long arg_0, long arg_1);
void function_db760(long region_index, long object_index, long permutation_index, long mode);
struct s_18444a;
void __stdcall function_184440(long arg_0, long arg_1, s_18444a const *arg_2, long arg_3);
extern byte *g_4ed280;

static __forceinline long function_9c912(long arg_0, long arg_1, long arg_2)
{
    long local_0;
    if (arg_0 < arg_1)
        local_0 = arg_1;
    else if (arg_0 > arg_2)
        local_0 = arg_2;
    else
        local_0 = arg_0;
    return local_0;
}

/* applies a section response event to an object: when the event names a valid region and an intact
   permutation of it (for mode 1, one with an effect), destroys that permutation through function_db760 */
// @retail 0x9c910
bool c_damage_section_response_event_definition::v11(long arg_0, long const *arg_1, long arg_2, void const *arg_3)
{
    bool result = false;
    long object_index = function_a58d0(arg_1[0]);
    if (object_index != NONE)
    {
        byte *object = (byte *)((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
        byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
        long damage_index = *(long *)(definition + 0x38);
        if (damage_index != NONE)
        {
            byte *region_flags = object + *(short *)(object + 0x122);
            long const *event = (long const *)arg_3;
            long region_index = event[0];
            if (region_index >= 0 && region_index < (long)((dword)(*(short *)(object + 0x120)) / 8))
            {
                byte *damage = g_4e3b44[damage_index & 0xffff].bytes;
                if (*(long *)(damage + 0x60) > 0)
                {
                    byte *region = *(byte **)(*(byte **)(damage + 0x64) + 0xc0) + region_index * 0x38;
                    /* the event pointer parameter carries the permutation index from here on */
                    long permutation_index = event[1];
                    arg_3 = (void const *)permutation_index;
                    if (permutation_index >= 0 && permutation_index < *(long *)(region + 0xc))
                    {
                        if (function_9c912(event[2], 0, 1) == event[2])
                        {
                            long permutation_bit = event[1];
                            if (!(*(word *)(region_flags + region_index * 8) & (1 << permutation_bit)))
                            {
                                if (event[2] != 1 || *(long *)(*(byte **)(region + 0x10) + permutation_bit * 0x50 + 0x34) != NONE)
                                {
                                    function_db760(region_index, object_index, (long)arg_3, event[2]);
                                    result = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

struct s_9cca0
{
    long field_0;
    byte field_4[0x18];
    long field_1c;
    long field_20;
    byte field_24[0xc];
    vector3f field_30;
    vector3f field_3c;
    byte field_48[0x34];
    short field_7c;
    byte field_7e[10];
};

// score 9 (was 9): the two vector copies and the global's test are ordered
// differently; retail sinks the k component's store past the global's test and
// loads the tag after it, ours stores the tag before the test. The test can only
// move above the event's stores when LTCG knows the event's address does not
// escape: passing it to a small non-capturing callee instead of 0x184440 gives
// retail's order, so this waits on 0x184440's body (BLOCKED-BY 0x184440).
// Tried: the first zeroing through the member array 9; the tag in a local 58;
// the tag test on the input 9; the vectors copied with memcpy 9; `&field_7e` 9.
// Round 19: the global flag read into a named bool before the event 53; the marker stored
// before the tag 10; the input as a typed struct 9 (same code); the vectors copied by component
// through source pointers 50; the tag stored first after the zeroing 31. Diagnostics in 0x184440:
// without its set_location call, or with a non-recursive stand-in for it, the test moves above
// the k store as in retail but the vector copies then store i early (12), so the escape comes
// through the recursive set_location and is not the whole difference.
// @retail 0x9cca0
bool c_breakable_surface_damage_event_definition::v11(long arg_0, long const *arg_1, long arg_2, void const *arg_3)
{
    s_9cca0 local_1;
    byte const *local_0 = (byte const *)arg_3;
    if (function_122c10(0x6a707421, *(long const *)(local_0 + 0x30)))
    {
        memset((byte *)&local_1 + 4, 0, 0x78);
        memset(local_1.field_7e, 0, sizeof(local_1.field_7e));
        local_1.field_1c = *(long const *)(local_0 + 0xc);
        local_1.field_20 = *(long const *)(local_0 + 0x10);
        local_1.field_30 = *(vector3f const *)(local_0 + 0x24);
        local_1.field_3c = *(vector3f const *)(local_0 + 0x18);
        local_1.field_0 = *(long const *)(local_0 + 0x30);
        local_1.field_7c = -1;
        if (*g_4ed280 && local_1.field_0 != NONE)
            function_184440(*(long const *)local_0, *(long const *)(local_0 + 4), (s_18444a const *)&local_1, *(long const *)(local_0 + 8));
    }
    return false;
}

struct s_9f880
{
    long field_0;
    long field_4;
    long field_8;
    long field_c;
    short field_10;
    byte field_12[2];
    real field_14;
    byte field_18;
};
struct s_9f881
{
    byte field_0[0x10a];
    struct s_9f882
    {
        word field_0 : 1;
        word field_1 : 1;
        word field_2 : 1;
        word field_3 : 13;
    } field_10a;
};

struct s_unit_melee_hit;
void __stdcall function_cfc90(long arg_0, long arg_1, s_unit_melee_hit const *arg_2);
bool function_183f50(long arg_0, long arg_1, long arg_2);
extern short g_47d8e0;

// @retail 0x9f880
bool c_unit_melee_damage_event_definition::v11(long arg_0, long const *arg_1, long arg_2, void const *arg_3)
{
    long local_0 = function_a58d0(arg_1[0]);
    long local_1 = function_a58d0(arg_1[1]);
    byte const *local_2 = (byte const *)arg_3;
    long local_3 = *(long const *)(local_2 + 4);
    long local_4 = *(long const *)(local_2 + 8);
    long local_5 = *(long const *)(local_2 + 0xc);
    if (local_3 != NONE && !function_183f50(local_4, local_5, local_3))
    {
        local_3 = NONE;
        local_4 = NONE;
        local_5 = NONE;
    }
    byte local_6 = local_2[0x18];
    if ((local_6 & 0x3f) >= 0x2a)
        local_6 &= 0xc0;
    struct s_9f883 { signed short field_0 : 16; };
    s_9f883 local_7;
    local_7.field_0 = *(short const *)(local_2 + 0x10);
    byte *local_8 = 0;
    if (local_7.field_0 != -1 && local_7.field_0 >= 0 && local_7.field_0 < *(long *)((byte *)g_4e034c + 0x150))
        local_8 = *(byte **)((byte *)g_4e034c + 0x154) + local_7.field_0 * 0xb4;
    if (!local_8)
        local_7.field_0 = g_47d8e0;
    if (local_0 != NONE)
    {
        byte *local_9 = (byte *)((s_object_header *)g_4e0300->data)[local_0 & 0xffff].object;
        if (local_9[0xaa] == 0 && !TEST_FIELD_BIT(((s_9f881 *)local_9)->field_10a.field_2) &&
            (local_1 != NONE || local_3 != NONE) && function_122c10(0x6a707421, *(long const *)local_2))
        {
            s_9f880 local_10;
            local_10.field_0 = local_1;
            local_10.field_4 = local_3;
            local_10.field_8 = local_4;
            local_10.field_c = local_5;
            local_10.field_10 = local_7.field_0;
            local_10.field_14 = *(real const *)(local_2 + 0x14);
            local_10.field_18 = local_6;
            function_cfc90(local_0, *(long const *)local_2, (s_unit_melee_hit const *)&local_10);
        }
    }
    return false;
}

struct s_damage_report;
point3f *function_b9dd0(long arg_0, point3f *arg_1);
void function_d6a70(long arg_0);
void function_d9640(s_damage_report const *arg_0, long arg_1);
void __stdcall function_e4a20(point3f const *arg_0, long arg_1, long arg_2, bool arg_3);

struct s_9c610
{
    byte field_0;
    byte field_1[3];
    dword field_4;
    long field_8;
    long field_c;
    long field_10;
    short field_14;
    byte field_16[2];
    vector3f field_18;
    point3f field_24;
    real field_30;
    real field_34;
    real field_38;
    long field_3c;
    short field_40;
    byte field_42[2];
    real field_44;
    real field_48;
    real field_4c;
    dword field_50;
};

struct s_input_9c610
{
    long field_0;
    byte field_4[4];
    short field_8;
    byte field_a;
    byte field_b;
    vector3f field_c;
    real field_18;
    real field_1c;
    dword field_20;
    real field_24;
    real field_28;
    short field_2c;
    short field_2e;
    dword field_30;
    byte field_34;
};

// score 13 (was 13): BLOCKED-BY 0xe4a20. The call to function_e4a20 at the end differs: retail
// passes the point in eax and the object in ecx, ours the point in ecx and the object on the
// stack, which follows from function_e4a20's own body. Flag 7 of +0x20 retail tests as
// `mov al / test al, al / jns`: the plain `input->field_20 & 0x80` gives that test and retail's
// length (429), but with the shorter call setup every later line shifts, 23 (also as a char < 0
// test or a byte mask); use it once 0xe4a20 is fixed. TEST_FIELD_BIT((x >> 7) & 1) 13 (kept),
// TEST_FIELD_BIT(x & 0x80) 13; flag 1 rewritten the same way 56, both 63; one flags local for
// the copy and both tests 49. Earlier (at 23): the flags store first of the second copy group
// 23 (kept); before the vector copy 67; first of all 67; before field_3c 27. Shared absolute
// lookup inline 23, out of line 122; shared index reconstruction 107; both shared inline
// operations 114; point local at notification 23, before guard 23, across dispatch 23; shared
// lookup with point lifetime 23. Final validity predicate 129; byte success result 23; unsigned
// reconstructed handle 23; signed-byte material 46; bool presence field 23; named validated tag
// result 23; source-width pool index 112; unsigned salt word 24. Byte notification read 46; byte
// tail read 23; both byte reads 46; word notification read 46; word tail read 23; both word
// reads 46; signed-byte tail read 23; byte notification/signed tail 46.
// @retail 0x9c610
bool c_damage_aftermath_event_definition::v11(long arg_0, long const *arg_1, long arg_2, void const *arg_3)
{
    bool local_0 = false;
    long local_1 = function_a58d0(arg_1[0]);
    long local_2 = function_a58d0(arg_1[1]);
    if (local_1 != NONE)
    {
        s_input_9c610 const *input = (s_input_9c610 const *)arg_3;
        long local_4 = NONE;
        long local_5 = NONE;
        short local_6 = input->field_8;
        if (local_6 != -1)
        {
            long local_7 = local_6;
            if (local_7 != NONE && local_7 >= 0 && local_7 < g_4e8c24->high_water_index)
            {
                byte *local_8 = g_4e8c24->data + local_7 * g_4e8c24->size;
                short local_9 = *(short *)local_8;
                if (local_9)
                {
                    local_4 = ((long)local_9 << 16) | local_7;
                    local_5 = *(char *)(local_8 + 0xc0);
                }
            }
        }
        if (function_122c10(0x6a707421, input->field_0))
        {
            short local_11 = (short)local_5;
            s_9c610 local_10;
            memset(&local_10, 0, sizeof(local_10));
            local_10.field_10 = local_2;
            local_10.field_c = local_4;
            local_10.field_14 = local_11;
            local_10.field_8 = input->field_0;
            if (input->field_a)
                local_10.field_18 = input->field_c;
            local_10.field_4 = input->field_20;
            local_10.field_34 = input->field_18;
            local_10.field_38 = input->field_1c;
            local_10.field_50 = input->field_30;
            local_10.field_3c = input->field_2c;
            local_10.field_44 = input->field_28;
            local_10.field_40 = input->field_2e;
            local_10.field_48 = input->field_24;
            local_10.field_0 = input->field_34;
            local_10.field_30 = 0.0f;
            function_b9dd0(local_1, &local_10.field_24);
            local_10.field_4c = 0.0f;
            if (input->field_20 & 2)
                function_d6a70(local_1);
            function_d9640((s_damage_report const *)&local_10, local_1);
            if (g_4e0300->data[(local_1 & 0xffff) * 12 + 3] == 0 && TEST_FIELD_BIT((input->field_20 >> 7) & 1))
                function_e4a20((point3f const *)&input->field_c, input->field_2e, local_1, false);
        }
        local_0 = true;
    }
    return local_0;
}

struct s_object;
s_object *function_badc0(long arg_0, dword arg_1);
void __stdcall function_191fab(long arg_0, long arg_1);
void __stdcall function_191e51(long arg_0, long arg_1);
void __stdcall function_191ec4(long arg_0, long arg_1, short arg_2);
void function_f8160(long arg_0);
long function_1896c0(real arg_0, long arg_1);
void player_speed_request_14ea20(long arg_0);
void player_speed_request_14eb10(long arg_0);
void player_speed_request_14ec00(long arg_0);
void teleporter_display_blocked_message(long arg_0);

// @retail 0x9ffa0
bool c_unit_pickup_event_definition::v11(long arg_0, long const *arg_1, long arg_2, void const *arg_3)
{
    long local_0 = function_a58d0(arg_1[0]);
    if (local_0 != NONE)
    {
        byte *local_1 = (byte *)function_badc0(local_0, 3);
        if (local_1 && *(long *)(local_1 + 0x13c) != NONE)
        {
            local_1 = (byte *)function_badc0(local_0, 3);
            long local_2 = NONE;
            if (local_1)
                local_2 = *(long *)(local_1 + 0x13c);
            long local_3 = *(short *)(g_4e8c24->data + (local_2 & 0xffff) * 0x21c + 0x28);
            if (local_3 != NONE)
            {
                byte const *local_4 = (byte const *)arg_3;
                                if (*(short const *)local_4 == 4 || (*(long const *)(local_4 + 4) != NONE && function_122c10(0x6974656d, *(long const *)(local_4 + 4))))
                {
                    switch (*(short const *)local_4)
                    {
                    case 0:
                        function_191fab(*(long const *)(local_4 + 4), local_3);
                        break;
                    case 2:
                        function_191e51(*(long const *)(local_4 + 4), local_3);
                        function_f8160(*(long const *)(local_4 + 4));
                        break;
                    case 1:
                    {
                        byte *local_7 = g_4e3b44[*(long const *)(local_4 + 4) & 0xffff].bytes;
                        if ((1 << local_7[0]) & 4)
                        {
                            long local_8 = *(long *)(local_7 + 0x268);
                            if (local_8 != NONE)
                                function_1896c0(1.0f, local_8);
                        }
                        else
                            function_f8160(*(long const *)(local_4 + 4));
                        function_191ec4(*(long const *)(local_4 + 4), local_3, *(short const *)(local_4 + 8));
                        break;
                    }
                    case 3:
                    {
                        byte *local_9 = g_4e3b44[*(long const *)(local_4 + 4) & 0xffff].bytes;
                        short local_10 = *(short *)(local_9 + 0x12c);
                        byte *local_11 = (byte *)((s_object_header *)g_4e0300->data)[local_0 & 0xffff].object;
                        long local_12 = *(long *)(local_11 + 0x13c);
                        if (local_10 == 2)
                            player_speed_request_14ea20(local_12);
                        else if (local_10 == 5)
                            player_speed_request_14ec00(local_12);
                        else if (local_10 == 3)
                            player_speed_request_14eb10(local_12);
                        function_f8160(*(long const *)(local_4 + 4));
                        function_191fab(*(long const *)(local_4 + 4), local_3);
                        break;
                    }
                    case 4:
                    {
                        byte *local_13 = (byte *)((s_object_header *)g_4e0300->data)[local_0 & 0xffff].object;
                        long local_14 = *(long *)(local_13 + 0x13c);
                        if (local_14 != NONE)
                            teleporter_display_blocked_message(local_14);
                        break;
                    }
                    }
                }
            }
        }
    }
    return false;
}
