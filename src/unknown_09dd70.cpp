#include "unknown_11c920.h"
#include "globals.h"
#include "object_types_21_1.h"
#include "bitstream.h"
#include "flags_writer.h"
#include <math.h>
#include <string.h>

// @flags /O2 /arch:SSE /Gr

struct s_player_appearance;
void function_7ee10(s_bitstream *arg_0, s_player_appearance const *arg_1);
bool function_7efa0(s_bitstream *arg_0, s_player_appearance *arg_1);
struct s_effect_owner;
void function_b7930(void *arg_0, long arg_1, long arg_2, s_effect_owner const *arg_3);
bool function_15f330(s_player_appearance const *arg_0, short arg_1, color3f *arg_2);
void __stdcall function_ccff0(long arg_0);

struct s_9d360
{
    byte field_0[0xc];
    vector3f field_c;
    byte field_18[0x90-0x18];
    short field_90;
    short field_92;
    long field_94;
    short field_98;
    byte field_9a[2];
    vector3f field_9c;
    short field_a8;
    char field_aa;
    char field_ab;
    struct s_9d361
    {
        long field_0;
        short field_4;
        short field_6;
        short field_8;
        byte field_a[2];
        real field_c;
    } field_ac[4];
    byte field_ec[4];
    real field_f0;
    real field_f4;
};

// @retail 0x9eac0
bool c_unit_type::v16(long arg_0, long arg_1, long arg_2)
{
    c_unit_type *volatile local_4 = this;
    s_9d360 *local_0 = (s_9d360 *)arg_0;
    s_9d360 *local_1 = (s_9d360 *)arg_1;
    bool local_2 = function_a7180(arg_0, arg_1) && local_0->field_9c.quantized_equal(&local_1->field_9c);
    local_1->field_9c = *g_4687a4;
    local_0->field_9c = *g_4687a4;
    long local_3 = 0;
    do
    {
        local_2 = local_2 && fabs(local_1->field_ac[local_3].field_c - local_0->field_ac[local_3].field_c) < 0.007936508394777775f;
        local_1->field_ac[local_3].field_c = 0.0f;
        local_0->field_ac[local_3].field_c = 0.0f;
        ++local_3;
    } while (local_3 < 4);
    local_2 = local_2 && fabs(local_1->field_f0 - local_0->field_f0) < 0.016129031777381897f;
    local_1->field_f0 = 0.0f;
    local_0->field_f0 = 0.0f;
    local_2 = local_2 && fabs(local_1->field_f4 - local_0->field_f4) < 0.016129031777381897f;
    local_1->field_f4 = 0.0f;
    local_0->field_f4 = 0.0f;
    return local_2;
}

// @retail 0x9de20
bool c_unit_type::v13(long arg_0, s_entity_info *arg_1, s_bitstream *arg_2)
{
    c_unit_type *volatile local_3 = this;
    bool local_0 = function_a6810(arg_1, arg_2);
    bool local_1 = function_7efa0(arg_2, (s_player_appearance *)((byte *)arg_1 + 0x10));
    short *local_2 = (short *)((byte *)arg_1 + 0x20);
    if (function_1957d0(arg_2))
        *local_2 = (short)function_1959c0(arg_2, 4);
    else
        *local_2 = -1;
    if (arg_2->bit_position > arg_2->size_in_bytes * 8 || !local_0 || !local_1)
        return false;
    return true;
}

// @retail 0x9dd70
void c_unit_type::v12(long arg_0, s_entity_info *arg_1, long arg_2, s_bitstream *arg_3)
{
    c_unit_type *volatile local_1 = this;
    function_a6660(arg_1, arg_3);
    function_7ee10(arg_3, (s_player_appearance const *)((byte *)arg_1 + 0x10));
    if ((arg_3->size_in_bytes << 3) - arg_3->bit_position >= 1 && *(short *)((byte *)arg_1 + 0x20) != -1)
        arg_3->data[arg_3->bit_position / 8] |= (byte)(1 << (arg_3->bit_position % 8));
    arg_3->bit_position++;
    short local_0 = *(short *)((byte *)arg_1 + 0x20);
    if (local_0 != -1)
        stream_write_checked(arg_3, local_0, 4);
}

// @retail 0x9d360
bool c_unit_type::v19(long arg_0, long arg_1, long arg_2, s_entity_data *arg_3)
{
    c_unit_type *volatile local_6 = this;
    bool local_0 = false;
    s_9d360 *local_1 = (s_9d360 *)arg_3;
    memset(local_1, 0, sizeof(*local_1));
    if (function_a5bd0((s_entity_info const *)arg_1, (s_z_initial_state *)arg_3))
    {
        local_1->field_90 = -1;
        local_1->field_9c = local_1->field_c;
        local_1->field_92 = -1;
        local_1->field_94 = -1;
        local_1->field_98 = -1;
        local_1->field_a8 = -1;
        local_1->field_aa = -1;
        local_1->field_ab = -1;
        long local_2 = 0;
        do
        {
            local_1->field_ac[local_2].field_0 = -1;
            local_1->field_ac[local_2].field_4 = -1;
            ++local_2;
        } while (local_2 < 4);
        long local_3 = ((long *)arg_1)[1];
        if (local_3 != NONE)
        {
            byte *local_4 = g_4e3b44[local_3 & 0xffff].bytes;
            short local_5 = *(short *)(local_4 + 0x1b4);
            if (local_5 >= 0 && local_5 < 2 && *(short *)(local_4 + 0x1b6) >= 0)
                local_1->field_ec[local_5] = local_4[0x1b6];
        }
        local_0 = true;
    }
    return local_0;
}

// @retail 0x9d890
long c_unit_type::v29(long arg_0, s_entity_info *arg_1, long *arg_2, long arg_3, long arg_4)
{
    c_unit_type *volatile local_0 = this;
    byte local_1[0xc4];
    color3f local_2[4];
    long local_3;
    function_b7930(local_1, arg_1->definition_index, NONE, 0);
    function_a5d90(local_1, arg_1, arg_2, arg_4);
    if (function_15f330((s_player_appearance const *)((byte *)arg_1 + 0x10), *(short *)((byte *)arg_1 + 0x20), local_2))
    {
        *(dword *)(local_1 + 0x74) |= 0xf;
        memcpy(local_1 + 0x78, local_2, sizeof(local_2));
    }
    if (arg_1->field0 != NONE)
        local_3 = function_a73b0(arg_1);
    else
        local_3 = function_b7b40(local_1);
    if (local_3 != NONE)
    {
        byte *local_4 = (byte *)((s_object_header *)g_4e0300->data)[local_3 & 0xffff].object;
        local_4[0xaf] = arg_1->byte8;
        function_ccff0(local_3);
    }
    return local_3;
}

struct s_9e521
{
    byte field_0[0x3d8];
    long field_3d8;
    long *field_3dc;
};

static __forceinline long function_9e521(s_bitstream *arg_0)
{
    long local_0 = NONE;
    long local_1 = function_1959c0(arg_0, 9) - 1;
    if (local_1 != NONE)
    {
        s_9e521 *local_2 = (s_9e521 *)g_4e0350;
        local_0 = NONE;
        if (local_2 && local_2->field_3d8 > 0)
            if ((local_1 < 0 ? 0 : (local_1 > local_2->field_3d8 - 1 ? local_2->field_3d8 - 1 : local_1)) == local_1)
                local_0 = local_2->field_3dc[local_1];
    }
    return local_0;
}

static __forceinline real function_9e522(long arg_0, long arg_1, real arg_2, real arg_3)
{
    if (arg_0 == 0)
        return arg_2;
    if (arg_0 >= arg_1)
        return arg_3;
    return (arg_2 * (arg_1 - arg_0) + arg_3 * arg_0) / arg_1;
}

inline bool function_a0190(vector3f const *arg_0);
real __fastcall function_24f6b0(dword arg_0, vector3f *arg_1);

// @retail 0x9e520
bool c_unit_type::v15(long arg_0, long arg_1, long arg_2, long arg_3, s_bitstream *arg_4)
{
    c_unit_type *volatile local_7 = this;
    bool local_0 = function_a6d50(arg_1, arg_3, arg_4) != 0;
    dword *local_1 = (dword *)arg_1;
    s_9d360 *local_2 = (s_9d360 *)arg_3;
    if (function_1957d0(arg_4))
    {
        if (function_1957d0(arg_4))
        {
            local_2->field_90 = (short)function_1959c0(arg_4, 4);
            local_0 = local_0 && local_2->field_90 >= 0 && local_2->field_90 <= 16;
        }
        else
            local_2->field_90 = -1;
        if (function_1957d0(arg_4))
        {
            local_2->field_92 = (short)function_1959c0(arg_4, 4);
            local_0 = local_0 && local_2->field_92 >= 0 && local_2->field_92 <= 16;
        }
        else
            local_2->field_92 = -1;
        local_0 = local_0 && (local_2->field_90 == -1 || local_2->field_92 == -1);
        *local_1 |= 0x400;
    }
    if (function_1957d0(arg_4))
    {
        if (function_1957d0(arg_4))
        {
            long local_3 = function_1959c0(arg_4, 10);
            local_2->field_94 = ((byte)function_1959c0(arg_4, 4) << 28) | local_3;
            local_2->field_98 = (short)function_1959c0(arg_4, 5);
        }
        else
        {
            local_2->field_94 = NONE;
            local_2->field_98 = -1;
        }
        *local_1 |= 0x800;
        if (local_2->field_94 == NONE)
            local_0 = local_0 && local_2->field_98 == -1;
        else
            local_0 = local_0 && local_2->field_98 >= 0 && local_2->field_98 < 32;
    }
    if (function_1957d0(arg_4))
    {
        long local_4 = function_1959c0(arg_4, 17);
        function_24f6b0(local_4, &local_2->field_9c);
        *local_1 |= 0x1000;
        local_0 = local_0 && function_a0190(&local_2->field_9c);
    }
    if (function_1957d0(arg_4))
    {
        local_2->field_a8 = (short)(function_1959c0(arg_4, 5) - 1);
        local_2->field_aa = (byte)(function_1959c0(arg_4, 3) - 1);
        local_2->field_ab = (byte)(function_1959c0(arg_4, 3) - 1);
        *local_1 |= 0x2000;
        if (local_2->field_aa != -1 && local_2->field_aa == local_2->field_ab)
            local_0 = false;
    }
    long local_5 = 0;
    do
    {
        if (stream_read_bit(arg_4))
        {
            local_2->field_ac[local_5].field_0 = function_9e521(arg_4);
            local_2->field_ac[local_5].field_4 = (short)(function_1959c0(arg_4, 4) - 1);
            *local_1 |= 1 << (local_5 + 14);
            local_0 = local_0 && (local_2->field_ac[local_5].field_4 == -1 ||
                (local_2->field_ac[local_5].field_4 >= 0 && local_2->field_ac[local_5].field_4 < 9));
        }
        if (stream_read_bit(arg_4))
        {
            local_2->field_ac[local_5].field_6 = (short)function_1959c0(arg_4, 8);
            local_2->field_ac[local_5].field_8 = (short)function_1959c0(arg_4, 11);
            local_2->field_ac[local_5].field_c = function_9e522(function_1959c0(arg_4, 7), 126, 0.0f, 1.0f);
            *local_1 |= 1 << (local_5 + 18);
        }
        ++local_5;
    } while (local_5 < 4);
    if (stream_read_bit(arg_4))
    {
        function_195820(arg_4, local_2->field_ec, 16);
        *local_1 |= 0x400000;
        long local_6 = 0;
        do
        {
            local_0 = local_0 && (char)local_2->field_ec[local_6] >= 0;
            ++local_6;
        } while (local_6 < 2);
    }
    long capacity = arg_4->size_in_bytes;
    capacity <<= 3;
    long position = arg_4->bit_position;
    bool present = false;
    if (position <= capacity)
        present = (arg_4->data[position / 8] & (1 << (position % 8))) != 0;
    arg_4->bit_position = position + 1;
    if (present)
    {
        long next_position = arg_4->bit_position;
        bool bit = false;
        if (next_position <= capacity)
            bit = (arg_4->data[next_position / 8] & (1 << (next_position % 8))) != 0;
        arg_4->bit_position = next_position + 1;
        local_2->field_ec[2] = bit;
        local_2->field_f0 = function_9e522(function_1959c0(arg_4, 6), 62, 0.0f, 1.0f);
        local_2->field_f4 = function_9e522(function_1959c0(arg_4, 6), 62, 0.0f, 1.0f);
        *local_1 |= 0x800000;
    }
    if (local_0 && arg_4->bit_position <= arg_4->size_in_bytes * 8)
        return true;
    return false;
}

static __forceinline void function_9dfc1(s_bitstream *arg_0, real arg_1, real arg_2, long arg_3)
{
    real local_0 = arg_1 * arg_2;
    long local_1;
    __asm
    {
        fld local_0
        fistp local_1
    }
    function_195720(arg_0, local_1, arg_3);
}

class c_handle_table_450cd0;
__declspec(noinline) bool function_99640(c_handle_table_450cd0 *arg_0, long arg_1);
void scenario_object_name_encode(long arg_0, s_bitstream *arg_1);
void function_1947e0(s_bitstream *arg_0, dword arg_1, long arg_2);
void function_194830(s_bitstream *arg_0, bool arg_1);
void function_194bc0(s_bitstream *arg_0, vector3f const *arg_1);

static __forceinline void function_9dfc2(s_bitstream *arg_0, dword arg_1, long arg_2)
{
    if (arg_2 < 32 && arg_1 >= (dword)(1 << arg_2))
    {
        char local_0[256];
        local_0[0] = 0;
        csprintf_256(local_0, "%u exceeds max value of %u", arg_1, 1 << arg_2);
    }
    function_195720(arg_0, arg_1, arg_2);
}

static __forceinline void function_9dfc4(s_bitstream *arg_0, bool arg_1)
{
    if ((arg_0->size_in_bytes << 3) - arg_0->bit_position >= 1 && arg_1)
        arg_0->data[arg_0->bit_position / 8] |= (byte)(1 << (arg_0->bit_position % 8));
    arg_0->bit_position++;
}

#pragma inline_depth(1)
// @retail 0x9dfc0
bool c_unit_type::v14(long arg_0, long arg_1, long arg_2, long arg_3, long arg_4, long arg_5, long arg_6, long arg_7)
{
    bool local_7 = false;
    long local_0 = arg_7 + 14;
    if (function_a69a0(arg_0, arg_1 & 0x3ff, arg_2, arg_4, arg_6, v33(), local_0))
    {
        s_flags_writer local_1;
        s_bitstream *local_2 = (s_bitstream *)arg_6;
        s_9d360 const *local_3 = (s_9d360 const *)arg_4;
        flags_writer_initialize(&local_1, local_2, 10, 14, arg_1 & 0xfffc00, arg_7);
        long local_5;
        if (local_1.space)
        {
            if (flags_writer_begin(&local_1, 10, "control-exists"))
            {
                function_194830(local_2, local_3->field_90 != -1);
                if (local_3->field_90 != -1)
                    function_9dfc2(local_2, local_3->field_90, 4);
                function_194830(local_2, local_3->field_92 != -1);
                if (local_3->field_92 != -1)
                    function_9dfc2(local_2, local_3->field_92, 4);
            }
            flags_writer_end(&local_1);
            if (flags_writer_begin(&local_1, 11, "parent-vehicle-exists"))
            {
                long local_4 = local_3->field_94;
                bool eligible = true;
                if (local_4 != NONE && arg_5)
                    eligible = function_99640((c_handle_table_450cd0 *)(*(byte **)(*(byte **)(*(byte **)arg_5 + 4) + 8) + 0x30), local_4);
                if (eligible)
                {
                    function_194830(local_2, local_4 != NONE);
                    if (local_3->field_94 != NONE)
                    {
                        simulation_write_entity_identifier(local_3->field_94, local_2);
                        function_1947e0(local_2, local_3->field_98, 5);
                    }
                }
                else
                    local_1.discarded = (1 << local_1.index) | local_1.discarded;
            }
            flags_writer_end(&local_1);
            if (flags_writer_begin(&local_1, 12, "desired-aiming-vector-exists"))
                function_194bc0(local_2, &local_3->field_9c);
            flags_writer_end(&local_1);
            if (flags_writer_begin(&local_1, 13, "desired-weapon-set-exists"))
            {
                function_9dfc2(local_2, local_3->field_a8 + 1, 5);
                function_9dfc2(local_2, local_3->field_aa + 1, 3);
                function_9dfc2(local_2, local_3->field_ab + 1, 3);
            }
            flags_writer_end(&local_1);
            local_5 = 0;
            do
            {
                short const *local_10 = &local_3->field_ac[local_5].field_6;
                if (flags_writer_begin(&local_1, local_5 + 14, "weapon-type-exists"))
                {
                    scenario_object_name_encode(local_3->field_ac[local_5].field_0, local_2);
                    function_9dfc2(local_2, local_3->field_ac[local_5].field_4 + 1, 4);
                }
                flags_writer_end(&local_1);
                if (flags_writer_begin(&local_1, local_5 + 18, "weapon-state-exists"))
                {
                    function_9dfc2(local_2, local_10[0], 8);
                    function_9dfc2(local_2, local_10[1], 11);
                    function_9dfc1(local_2, local_3->field_ac[local_5].field_c, 126.0f, 7);
                }
                flags_writer_end(&local_1);
                ++local_5;
            } while (local_5 < 4);
            if (flags_writer_begin(&local_1, 22, "grenade-counts-exists"))
                function_1955d0(local_2, local_3->field_ec, 16);
            flags_writer_end(&local_1);
            if (flags_writer_begin(&local_1, 23, "active-camo-exists"))
            {
                function_9dfc4(local_2, local_3->field_ec[2] != 0);
                function_9dfc1(local_2, local_3->field_f0, 62.0f, 6);
                function_9dfc1(local_2, local_3->field_f4, 62.0f, 6);
            }
            flags_writer_end(&local_1);
            *(dword *)arg_2 |= local_1.written;
            local_7 = true;
        }
    }
    return local_7;
}
#pragma inline_depth(255)

__declspec(noinline) void *function_122c10(long arg_0, long arg_1);
void function_d0d20(long arg_0);
void function_d0c10(long arg_0);
void function_d0930(long arg_0, vector3f *arg_1);
void __stdcall function_d09c0(long arg_0, long arg_1, long arg_2, short arg_3, dword const *arg_4);
void function_d0b70(long arg_0, long arg_1, long arg_2, real arg_3, long arg_4);
void function_d0e00(long arg_0, real arg_1);
void function_b58c0(long arg_0, dword arg_1);

static __forceinline byte *function_9d952(long arg_0)
{
    return (byte *)((s_object_header *)g_4e0300->data)[arg_0 & 0xffff].object;
}

// @retail 0x9d950
void c_unit_type::v31(long arg_0, long arg_1, long arg_2, long arg_3)
{
    dword local_1 = arg_1;
    s_9d360 *local_2 = (s_9d360 *)arg_3;
    if (local_1 & 0x3ff)
        function_a6430(this, local_1 & 0x3ff, arg_0, arg_3);
    if (local_1 & 0x400)
    {
        long local_4 = NONE;
        if (datum_get_absolute_inlined(g_4e8c24, local_2->field_90))
            local_4 = data_datum_index(g_4e8c24, local_2->field_90);
        long local_5 = local_2->field_92;
        byte *local_6 = function_9d952(arg_0);
        *(long *)(local_6 + 0x2a8) = local_4;
        *(long *)(local_6 + 0x2ac) = local_5;
        function_d0d20(arg_0);
    }
    if (local_1 & 0x800)
    {
        long local_7 = function_a58d0(local_2->field_94);
        short local_8 = local_2->field_98;
        if (local_7 != NONE)
        {
            byte *local_9 = function_9d952(local_7);
            if (!((1 << local_9[0xaa]) & 3) || local_8 < 0 ||
                local_8 >= *(long *)(g_4e3b44[*(long *)local_9 & 0xffff].bytes + 0x1c8))
            {
                local_7 = NONE;
                local_8 = -1;
            }
        }
        else if (local_8 != -1)
            local_8 = -1;
        byte *local_10 = function_9d952(arg_0);
        *(long *)(local_10 + 0x2a0) = local_7;
        *(short *)(local_10 + 0x2a4) = local_8;
        function_d0c10(arg_0);
    }
    if (local_1 & 0x1000)
        function_d0930(arg_0, &local_2->field_9c);
    if (local_1 & 0x2000)
    {
        byte *local_11 = function_9d952(arg_0);
        long local_12 = *(long *)&local_2->field_a8;
        *(long *)(local_11 + 0x214) = local_12;
        *(short *)(local_11 + 0x210) = *(short *)(local_11 + 0x214);
    }
    long local_13 = 0;
    dword local_14;
    do
    {
        s_9d360::s_9d361 const *local_21 = &local_2->field_ac[local_13];
        local_14 = local_1 & (1 << (local_13 + 14));
        if (local_14)
        {
            long local_15 = local_21->field_0;
            if (local_15 == NONE || function_122c10(0x77656170, local_15))
                function_d09c0(arg_0, local_13, local_21->field_0, local_21->field_4, (dword const *)&local_2->field_a8);
        }
        if ((local_1 & (1 << (local_13 + 14))) || (local_1 & (1 << (local_13 + 18))))
            function_d0b70(arg_0, local_21->field_6, local_21->field_8, local_21->field_c, local_13);
        ++local_13;
    } while ((dword)local_13 < 4);
    if (local_1 & 0x400000)
    {
        byte *local_16 = function_9d952(arg_0);
        long local_17 = 0;
        do
        {
            local_16[local_17 + 0x23e] = local_2->field_ec[local_17];
            ++local_17;
        } while (local_17 < 2);
    }
    if (local_1 & 0x800000)
    {
        byte *local_18 = function_9d952(arg_0);
        *(real *)(local_18 + 0x2b0) = local_2->field_f0;
        if (local_2->field_ec[2])
        {
            byte *local_19 = function_9d952(arg_0);
            *(dword *)(local_19 + 0x134) |= 8;
            *(real *)(local_19 + 0x2b8) = 0.25f;
            byte *local_20 = function_9d952(arg_0);
            if (*(long *)(local_20 + 0xd4) != NONE)
                function_b58c0(*(long *)(local_20 + 0xd4), 0x800000);
        }
        else
            function_d0e00(arg_0, 1.0f);
        *(real *)(local_18 + 0x2b8) = local_2->field_f4;
    }
}
