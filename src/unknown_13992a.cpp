// @flags /O1 /Oi /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "object_dispatch.h"
#include <string.h>

struct s_unit_weapon_hands
{
    byte unknown00[8];
    bool one;
    bool two;
    bool seat_allows;
    byte unknown0b;
};
struct s_hud_weapon_record;
struct s_13b164_status;
struct s_22bfc0_definition;
struct s_22bff0_definition;
struct s_player_score_display
{
    long type;
    bool teams;
    byte unknown05[3];
    long limit;
    word name[32];
    bool timer;
    byte unknown4d;
    word timer_text[8];
    byte unknown5e[2];
    color3f color;
    bool leading;
    byte unknown6d[3];
    long player_index;
    long score;
    word score_text[8];
    bool rival;
    byte unknown89[3];
    long rival_index;
    color3f rival_color;
    long rival_score;
    word rival_text[8];
    bool charge;
    byte unknownb1[3];
    real charge_fraction;
    long zone_count;
    dword zone_colors[8];
    real zone_progress[8];
};


void function_cb430(long arg_1, s_unit_weapon_hands *arg_2);
short function_ce020(long arg_1);
short function_cdff0(long arg_1, short arg_2);
short function_c8860(long arg_1);
long function_cbd50(long arg_1, short arg_2);
bool function_100f70(long arg_1);
bool function_15fef0(long arg_1);
bool function_15eb20(long arg_1);
real function_c53c0(long arg_1);
long function_22bfc0(s_22bfc0_definition const *arg_1, char arg_2);
long function_22bff0(s_22bff0_definition const *arg_1, short arg_2, char arg_3);
void function_13afa9(long arg_1, long *arg_3, s_hud_weapon_record *arg_2);
short function_cfe40(long arg_1);
long function_cbe60(long arg_1);
void function_13b164(long arg_1, s_13b164_status *arg_2);
bool function_15dea0(void);
bool function_15eaf0(void);
bool function_15f3a0(long arg_1, s_player_score_display *arg_2);
void unicode_string_copy(word *arg_1, word const *arg_2, long arg_3);
long function_162fd0(long arg_1);
bool function_138820(void);
long function_14de70(long arg_1);

#define MAX(a, b) ((a) > (b) ? (a) : (b))

/* fills the hud status record for a player: the unit, its seat in its parent and the weapons, then the
   score display */
// @retail 0x13992a
void function_13992a(byte *arg_1, long arg_2)
{
    memset(arg_1, 0, 0x270);
    *(long *)(arg_1 + 0) = NONE;
    *(long *)(arg_1 + 4) = NONE;
    *(long *)(arg_1 + 8) = NONE;
    *(long *)(arg_1 + 0xc) = NONE;
    *(long *)(arg_1 + 0x10) = NONE;
    *(long *)(arg_1 + 0x14) = NONE;
    *(long *)(arg_1 + 0x18) = NONE;
    long local_1 = function_14de70(arg_2);
    if (local_1 != NONE)
    {
        byte const *local_2 = g_4e8c24->data + (local_1 & 0xffff) * 0x21c;
        if (*(long *)(local_2 + 0x2c) != NONE)
        {
            byte const *local_3 = (byte const *)OBJECT_DATUM_GET(*(long *)(local_2 + 0x2c));
            /* the unit again, as a second view for the parent lookup (the parent is read twice, as retail) */
            byte const *unit = (byte const *)OBJECT_DATUM_GET(*(long *)(local_2 + 0x2c));
            s_unit_weapon_hands local_4;
            bool local_5 = false;
            function_cb430(*(long *)(local_2 + 0x2c), &local_4);
            arg_1[0x1c] = true;
            *(long *)(arg_1 + 0x20) = *(long *)(local_3 + 0x1e4);
            *(long *)(arg_1 + 0x24) = *(long *)(local_3 + 0x1e8);
            *(long *)(arg_1 + 0x28) = *(long *)(local_3 + 0xec);
            *(long *)(arg_1 + 0x2c) = *(long *)(local_3 + 0xf0);
            *(short *)(arg_1 + 0x36) = function_ce020(*(long *)(local_2 + 0x2c));
            *(short *)(arg_1 + 0x34) = function_cdff0(*(long *)(local_2 + 0x2c), function_ce020(*(long *)(local_2 + 0x2c)));
            arg_1[0x30] = local_4.one;
            arg_1[0x31] = local_4.two;
            *(short *)(arg_1 + 0x3c) = function_c8860(*(long *)(local_2 + 0x2c));
            arg_1[0x32] = local_4.seat_allows;
            *(short *)(arg_1 + 0x38) = function_cdff0(*(long *)(local_2 + 0x2c), 0);
            *(short *)(arg_1 + 0x3a) = function_cdff0(*(long *)(local_2 + 0x2c), 1);
            *(short *)(arg_1 + 0x1e) = *(short *)(local_3 + 0x10a);
            arg_1[0x3e] = false;
            if (*(short *)(arg_1 + 0x3c) != NONE && function_cbd50(*(long *)(local_2 + 0x2c),
                (short)*(signed char *)((byte const *)OBJECT_DATUM_GET(*(long *)(local_2 + 0x2c)) + 0x212)) != NONE)
            {
                if (!function_100f70(function_cbd50(*(long *)(local_2 + 0x2c),
                    (short)*(signed char *)((byte const *)OBJECT_DATUM_GET(*(long *)(local_2 + 0x2c)) + 0x212))))
                    arg_1[0x3e] = true;
            }
            arg_1[0x3f] = function_15fef0(local_1);
            arg_1[0x40] = function_15eb20(local_1);
            *(real *)(arg_1 + 0x44) = function_c53c0(*(long *)(local_2 + 0x2c));
            *(long *)arg_1 = function_22bfc0((s_22bfc0_definition const *)g_4e3b44[*(long *)local_3 & 0xffff].bytes,
                g_4e6948->state == 2);
            if (*(long *)(local_3 + 0x14) != NONE && *(short *)(local_3 + 0x1fc) != NONE)
            {
                byte const *local_7 = (byte const *)OBJECT_DATUM_GET(*(long *)(unit + 0x14));
                byte const *local_8 = g_4e3b44[*(long *)local_7 & 0xffff].bytes;
                byte const *local_9 = *(byte *const *)(local_8 + 0x1cc) + *(short *)(local_3 + 0x1fc) * 0xb0;
                arg_1[0x48] = true;
                *(long *)(arg_1 + 4) = function_22bff0((s_22bff0_definition const *)g_4e3b44[*(long *)local_7 & 0xffff].bytes,
                    *(short *)(local_3 + 0x1fc), g_4e6948->state == 2);
                if (!(bool)((*(dword *)local_9 >> 5) & 1)) local_5 = true;
                if ((bool)((*(dword *)local_9 >> 3) & 1))
                {
                    long local_10 = function_cbd50(*(long *)(local_3 + 0x14),
                        *(signed char *)((byte const *)OBJECT_DATUM_GET(*(long *)(local_3 + 0x14)) + 0x212));
                    if (local_10 == NONE)
                    {
                        long local_11 = *(long *)(local_7 + 0x10);
                        while (local_11 != NONE)
                        {
                            byte const *local_12 = (byte const *)OBJECT_DATUM_GET(local_11);
                            if ((bool)((*(dword *)(local_12 + 4) >> 26) & 1)
                                && ((1 << local_12[0xaa]) & 3)
                                && (bool)((*(dword *)(g_4e3b44[*(long *)local_12 & 0xffff].bytes + 0xbc) >> 26) & 1))
                            {
                                local_10 = function_cbd50(local_11, *(signed char *)(local_12 + 0x212));
                                if (local_10 != NONE) break;
                            }
                            local_11 = *(long *)(local_12 + 0xc);
                        }
                    }
                    if (local_10 != NONE)
                        function_13afa9(local_10, (long *)(arg_1 + 8), (s_hud_weapon_record *)(arg_1 + 0x50));
                }
            }
            if (!local_5)
            {
                if (function_cbd50(*(long *)(local_2 + 0x2c),
                    *(signed char *)((byte const *)OBJECT_DATUM_GET(*(long *)(local_2 + 0x2c)) + 0x212)) != NONE)
                {
                    function_13afa9(function_cbd50(*(long *)(local_2 + 0x2c),
                        *(signed char *)((byte const *)OBJECT_DATUM_GET(*(long *)(local_2 + 0x2c)) + 0x212)),
                        (long *)(arg_1 + 0xc), (s_hud_weapon_record *)(arg_1 + 0x88));
                }
                if (function_cbd50(*(long *)(local_2 + 0x2c),
                    *(signed char *)((byte const *)OBJECT_DATUM_GET(*(long *)(local_2 + 0x2c)) + 0x213)) != NONE)
                {
                    function_13afa9(function_cbd50(*(long *)(local_2 + 0x2c),
                        *(signed char *)((byte const *)OBJECT_DATUM_GET(*(long *)(local_2 + 0x2c)) + 0x213)),
                        (long *)(arg_1 + 0x10), (s_hud_weapon_record *)(arg_1 + 0xc0));
                }
                if (function_cbd50(*(long *)(local_2 + 0x2c),
                    *(signed char *)((byte const *)OBJECT_DATUM_GET(*(long *)(local_2 + 0x2c)) + 0x213)) != NONE)
                {
                    function_13afa9(function_cbd50(*(long *)(local_2 + 0x2c),
                        *(signed char *)((byte const *)OBJECT_DATUM_GET(*(long *)(local_2 + 0x2c)) + 0x213)),
                        (long *)(arg_1 + 0x10), (s_hud_weapon_record *)(arg_1 + 0xc0));
                }
            }
            long local_13 = function_cfe40(*(long *)(local_2 + 0x2c));
            long local_14 = function_cbe60(*(long *)(local_2 + 0x2c));
            if (local_13 != NONE)
            {
                long local_15 = NONE;
                local_14 = NONE;
                for (long local_16 = 0; local_16 < 4; local_16++)
                {
                    if (local_16 != local_13 && *(long *)(local_3 + 0x218 + local_16 * 4) != NONE
                        && *(long *)(local_3 + 0x228 + local_16 * 4) > local_15)
                    {
                        local_15 = *(long *)(local_3 + 0x228 + local_16 * 4);
                        local_14 = *(long *)(local_3 + 0x218 + local_16 * 4);
                    }
                }
            }
            if (!local_5 && local_14 != NONE)
                function_13afa9(local_14, (long *)(arg_1 + 0x14), (s_hud_weapon_record *)(arg_1 + 0xf8));
            function_13b164(*(long *)(local_2 + 0x2c), (s_13b164_status *)arg_1);
        }
        if (function_15dea0())
        {
            byte const *local_17 = *(byte **)(g_4e3b44[*(long *)((byte *)g_4e034c + 0x16c) & 0xffff].bytes + 0xc);
            arg_1[0x131] = 2 + (function_15eaf0() ? 1 : 0);
            s_player_score_display local_18;
            if (function_15f3a0(arg_2, &local_18))
            {
                arg_1[0x130] = true;
                *(long *)(arg_1 + 0x18) = *(long *)(local_17 + 0x544);
                arg_1[0x174] = local_18.timer;
                unicode_string_copy((word *)(arg_1 + 0x176), local_18.timer_text, 8);
                arg_1[0x132] = local_18.teams;
                arg_1[0x133] = local_18.leading;
                arg_1[0x1d0] = local_18.rival;
                arg_1[0x21c] = local_18.charge;
                *(real *)(arg_1 + 0x220) = local_18.charge_fraction;
                unicode_string_copy((word *)(arg_1 + 0x134), local_18.name, 0x20);
                *(color3f *)(arg_1 + 0x18c) = local_18.color;
                *(long *)(arg_1 + 0x188) = local_18.player_index;
                unicode_string_copy((word *)(arg_1 + 0x1bc), local_18.score_text, 8);
                *(real *)(arg_1 + 0x1cc) = !local_18.limit ? (real)local_18.score / (real)MAX(local_18.score, MAX(local_18.rival_score, 1))
                    : (real)local_18.score / (real)local_18.limit;
                if (local_18.rival)
                {
                    *(color3f *)(arg_1 + 0x1d8) = local_18.rival_color;
                    *(long *)(arg_1 + 0x1d4) = local_18.rival_index;
                    unicode_string_copy((word *)(arg_1 + 0x208), local_18.rival_text, 8);
                    *(real *)(arg_1 + 0x218) = !local_18.limit ? (real)local_18.rival_score / (real)MAX(local_18.score, MAX(local_18.rival_score, 1))
                    : (real)local_18.rival_score / (real)local_18.limit;
                }
                *(long *)(arg_1 + 0x22c) = local_18.zone_count;
                for (long local_20 = 0; local_20 < *(long *)(arg_1 + 0x22c); local_20++)
                {
                    *(dword *)(arg_1 + 0x230 + local_20 * 4) = local_18.zone_colors[local_20];
                    *(real *)(arg_1 + 0x250 + local_20 * 4) = local_18.zone_progress[local_20];
                }
            }
            else arg_1[0x130] = false;
            *(long *)(arg_1 + 0x228) = function_162fd0(arg_2);
            arg_1[0x224] = *(long *)(arg_1 + 0x228) != NONE;
        }
        else
        {
            memset(arg_1 + 0x130, 0, 0x140);
            arg_1[0x130] = true;
            arg_1[0x131] = function_138820() ? 1 : 0;
        }
    }
}
