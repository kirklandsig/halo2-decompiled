// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1fb7e0.h"
#include "slot_handler.h"
static __forceinline long real_to_long(real value);

struct s_audio_priority_definition
{
    byte unknown00[0x1a];
    short priority;
    byte unknown1c[0x60 - 0x1c];
};

struct s_audio_priority_table
{
    long count;
    s_audio_priority_definition *entries;
};

struct s_audio_selection
{
    short type;
    short unknown02;
    long object_index;
    long target_index;
    long tag_index;
    real weight;
};

struct s_audio_weighted_entry
{
    byte unknown00[0x10];
    real weight;
};

struct s_20d0c0
{
    short field_0;
    short field_2;
    long field_4;
    s_1fb7e0_data field_8;
};

struct s_audio_queue;
extern s_audio_queue *g_4f939c;

long __declspec(noinline) function_20f040(short arg_0);
bool unit_submit_entry_request(short fallback_index, short entry_index, long unit_index, long priority, void const *extra);
long function_20f2d0(short arg_1, long arg_0, long arg_2, long arg_3, long arg_4, s_20d0c0 const *arg_5);
void function_20cbd0(short arg_0, s_audio_priority_table *arg_1, long arg_2,
    long arg_3, long arg_4, long arg_5, short arg_6,
    s_audio_selection *arg_7, short arg_8, short *arg_9);
short function_20f0a0(s_audio_weighted_entry *arg_0, short arg_1);
struct s_object;
s_object *function_badc0(long arg_0, dword arg_1);
void function_20c440(s_audio_priority_table *arg_1, byte const *arg_0, long arg_2,
    long arg_3, long arg_4, long arg_5, short arg_6, short arg_7,
    short arg_8, short arg_9, s_audio_selection *arg_10, short *arg_11);

PRIVATE __forceinline s_audio_priority_table *function_20cdc1(void)
{
    s_audio_priority_table *local_0 = NULL;
    byte *local_1 = (byte *)g_4e034c;
    if (local_1 && *(long *)(local_1 + 0xc8) > 0)
    {
        long local_2 = *(long *)(*(byte **)(local_1 + 0xcc) + 0x64);
        if (local_2 != NONE)
            local_0 = (s_audio_priority_table *)g_4e3b44[local_2 & 0xffff].bytes;
    }
    return local_0;
}

// @retail 0x20d0c0
bool __stdcall function_20d0c0(s_audio_priority_table *arg_0, long arg_1, short arg_2,
    s_audio_selection const *arg_3, s_1fb7e0_data const *arg_4, long arg_5, long *arg_6)
{
    bool local_7 = false;
    long local_0 = g_510c54->game_time;
    s_audio_selection const *local_18 = arg_3;
    long local_1 = local_18->object_index;
    byte *local_2 = *(byte **)(g_4e0300->data + (local_1 & 0xffff) * 12 + 8);
    short local_11 = *(short *)(local_2 + 0x138);
    short local_3 = (short)function_20f040(local_11);
    byte *local_4 = (byte *)g_4f939c + local_3 * 0x7dc;
    s_audio_priority_definition *local_5 = &arg_0->entries[local_18->type];
    long *local_19 = arg_6;
    if (local_19)
        *local_19 = NONE;
    s_20d0c0 local_6;
    local_6.field_0 = arg_2;
    local_6.field_4 = local_18->target_index;
    if (arg_4)
        local_6.field_8 = *arg_4;
    else
        local_6.field_8.unknown00 = 0;
    if (*(short *)((byte *)local_5 + 0xa) >= 13)
    {
        short local_14 = local_18->type;
        local_7 = unit_submit_entry_request(NONE, local_14, local_1, 13, &local_6);
    }
    else
    {
        long local_8 = function_20f2d0(local_18->type, local_18->tag_index, local_1, arg_1, arg_5, &local_6);
        if (local_8 != NONE)
        {
            if (local_19)
                *local_19 = local_8;
            local_7 = true;
        }
    }
    if (local_7)
    {
        real local_9 = g_510c54->field_2_3 * *(real *)((byte *)&arg_0->entries[local_18->type] + 0x30);
        long local_10 = real_to_long(local_9);
        *(long *)(local_4 + local_18->type * 4) = local_0 + local_10;
    }
    return local_7;
}

// @retail 0x20ba60
bool __stdcall function_20ba60(short arg_0, long arg_1, long arg_2, long arg_3, short arg_4, s_1fb7e0_data const *arg_5)
{
    volatile bool local_0 = false;
    if (*((byte *)g_4f55d0 + 0x20) && *(short *)((byte *)g_4f55d0 + 0x22) <= 0)
    {
        long local_1 = 0;
        s_audio_selection local_2[100];
        s_audio_priority_table *local_3 = NULL;
        byte *local_4 = (byte *)g_4e034c;
        if (local_4 && *(long *)(local_4 + 0xc8) > 0)
        {
            long local_5 = *(long *)(*(byte **)(local_4 + 0xcc) + 0x64);
            if (local_5 != NONE)
                local_3 = (s_audio_priority_table *)g_4e3b44[local_5 & 0xffff].bytes;
        }
        if (local_3)
        {
            short const volatile *local_7 = &arg_0;
            short local_8 = *local_7;
            function_20cbd0(local_8, local_3, arg_1, arg_2, NONE, arg_3, (short)arg_4,
                local_2, 100, (short *)&local_1);
            short local_6 = function_20f0a0((s_audio_weighted_entry *)local_2, (short)local_1);
            if (local_6 != NONE)
            {
                long local_9 = g_510c54->game_time;
                if (function_20d0c0(local_3, local_9, local_8,
                        &local_2[local_6], arg_5, NONE, &local_1))
                    local_0 = true;
            }
        }
    }
    return local_0;
}

// @retail 0x20cdc0
bool __stdcall function_20cdc0(long arg_0, long arg_1, long arg_2, s_audio_selection *arg_3, long *arg_4)
{
    long local_0 = 0;
    bool local_16 = false;
    s_audio_priority_table *local_2 = function_20cdc1();
    byte *local_3 = g_4f9398->data + (arg_2 & 0xffff) * 0x54;
    long local_4 = *(short *)(local_3 + 2);
    byte *local_5 = (byte *)function_badc0(arg_1, NONE);
    s_audio_selection local_6[100];
    byte local_7[0x40];
    while (local_4 != NONE)
    {
        byte *local_8 = (byte *)&local_2->entries[local_4];
        if (*(long *)(local_8 + 0x50) > 0)
        {
            for (short local_9 = 0; local_9 < *(long *)(local_8 + 0x50); local_9++)
            {
                if ((short)local_0 >= 100)
                    break;
                byte *local_10 = *(byte **)(local_8 + 0x54) + local_9 * 12;
                short local_11 = *(short *)(local_10 + 6);
                if (local_11 != NONE)
                {
                    *(short *)(local_7 + 0) = NONE;
                    *(long *)(local_7 + 4) = *(long *)local_10;
                    *(short *)(local_7 + 2) = local_11;
                    *(short *)(local_7 + 0xa) = 0;
                    *(short *)(local_7 + 0x14) = 0;
                    *(short *)(local_7 + 0x16) = NONE;
                    *(short *)(local_7 + 0x18) = NONE;
                    *(short *)(local_7 + 0x20) = 0;
                    *(short *)(local_7 + 0x22) = 0;
                    *(short *)(local_7 + 0x24) = 0;
                    *(long *)(local_7 + 0x3c) = NONE;
                    *(short *)(local_7 + 0x26) = 0;
                    *(long *)(local_7 + 0x30) = 0;
                    *(long *)(local_7 + 0x28) = 0;
                    *(short *)(local_7 + 0x2c) = 0;
                    *(short *)(local_7 + 0x1a) = 0;
                    switch (*(short *)(local_10 + 8))
                    {
                    case 0: *(short *)(local_7 + 8) = 2; break;
                    case 1: *(short *)(local_7 + 8) = 4; break;
                    case 2: *(short *)(local_7 + 8) = 1; break;
                    case 3: *(short *)(local_7 + 8) = 6; break;
                    case 4: *(short *)(local_7 + 8) = 11; break;
                    }
                    *(short *)(local_7 + 0xc) = 0;
                    function_20c440(local_2, local_7, arg_0, local_5 ? arg_1 : NONE,
                        arg_2, NONE, 0, NONE, NONE, NONE, local_6, (short *)&local_0);
                }
                else if ((local_10[4] & 2) && *(short *)(local_10 + 0xa) != NONE &&
                    local_5 && ((1 << local_5[0xaa]) & 3))
                {
                    function_20cbd0(*(short *)(local_10 + 0xa), local_2, arg_1, arg_0,
                        arg_2, NONE, NONE, local_6, 100, (short *)&local_0);
                }
            }
        }
        if ((short)local_0 > 0)
            break;
        local_4 = *(short *)(local_8 + 8);
    }
    short local_12 = function_20f0a0((s_audio_weighted_entry *)local_6, (short)local_0);
    if (local_12 != NONE)
    {
        long local_1;
        real local_13 = g_510c54->field_2_3 * 0.2f;
        __asm
        {
            fld local_13
            fistp local_0
        }
        long local_14 = *(long *)(local_3 + 0x14) + *(short *)(local_3 + 0x1c) + local_0;
        s_audio_selection *local_15 = &local_6[local_12];
        if (function_20d0c0(local_2, local_14, NONE, local_15, NULL, arg_2, &local_1))
        {
            if (arg_3)
                *arg_3 = *local_15;
            if (arg_4)
                *arg_4 = local_1;
            local_16 = true;
        }
    }
    return local_16;
}
