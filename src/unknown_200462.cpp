// @flags /O1 /Oi /Oy- /arch:SSE /Gr
#include "view_context_storage.h"
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "unknown_13927e.h"
#include "globals.h"
#include <string.h>

struct s_510c4c;
extern s_510c4c *g_510c4c;
extern point3f *g_468720;
extern real g_4e69c0[4];

struct s_4e6950
{
    s_color_bits field_0;
    point3f field_c;
    real value18;
    real value1c;
    real value20;
    real value24;
    byte unknown28[0x70 - 0x28];
    real user_values[4];
};
extern s_4e6950 g_4e6950;

struct s_interface_pulse
{
    char delay;
    char ticks;
    char repeats;
    byte unknown03;
};

struct s_float_rect
{
    real x0;
    real x1;
    real y0;
    real y1;
};

struct s_widget_quad_2b11;
void function_1396c7(long arg_0, point2f *arg_1);
real function_200891(s_interface_pulse const *arg_0, bool arg_1);
real function_2008fc(s_interface_pulse const *arg_0, bool arg_1);
void function_22a664(s_widget_quad_2b11 const *arg_0, s_float_rect const *arg_1,
    long arg_2, long arg_3, long arg_4);

// score 14 (was 14): draw bounds source ADD/LEA and occupied-width/half-height XMM homes differ.
// Round 74: half-height lower/all/upper/early-lower copies 14/20/14/14;
// half-width right/all/early-right copies 14/14/14; bounds pointer with early-right copy 12
// (one-use pointer, not kept without a match). Opening graph restored.
// Earlier at 70 (from 103): frame 0x50 agrees; pulse/entry setup and second-pass
// the copy source address and occupied-width/height registers still differ.
// Earlier: frame 0x50 agrees; pulse/entry setup and second-pass
// tag/index and row-X scheduling differ; aggregate quad 103;
// half-height first 103; signed mask 103; shared ordinal word 229, byte 229.
// shared resolved result 103; shared tag 103; shared pulse receiver 103;
// all three shared 103; positive first-pass validity block 103.
// Fresh first-pass entry base 70; indexed user 103; user before table 215;
// shared entry/sequence/user combination 103/103/103; extent values 103;
// late accepted mask 229. Pulse-base setup and second-pass homes still differ.
// First-pass coordinate value 250; coordinate library copy 250; first entry value 239; draw entry value
// 164; both entries 243; horizontal real array 203; width sprite value 189; draw sprite value 174. Pulse
// setup, draw carriers and running-X allocation remain.
// Earlier at 70 (from 70): native pulse row, first/draw/both pulse receivers, occupied-width
// Earlier: native pulse row, first/draw/both pulse receivers, occupied-width
// accumulation and half-width/half-height/both product chains all 70. Pulse-base setup,
// draw tag/sequence homes and running-X scheduling remain different.
// pulse-base setup and draw-pass row/sequence homes remain;
// scalar row X/Y/both 89/87/89; native rectangle library copy 70;
// pulse array after user base/base before row offset/named row offset 70/70/70; sequence count once 161.
// Earlier at 70 (from 70): pulse/table entry schedule and draw-pass carriers remain;
// pulse/image/sprite reads at use 224/155/137; pulse guard first 78;
// first sequence count once 209; positive draw arm 70; occupied width first 70; exhaustive bitmap result 148.
// Earlier at 70 (from 70): pulse/table setup and draw-pass register scheduling still differ.
// selected-entry scope 81; live draw sequence 70; live first sequence 70; both sequence bindings 70
// native first entry 70; native draw entry 70; both entry references 70; native dimension inputs 70.
// Earlier at 70 (from 70): pulse/table setup, draw-pass carriers and running-X scheduling remain.
// Earlier: pulse/table entry schedule and draw-pass carriers remain;
// Earlier: pulse/table setup and draw-pass register scheduling still differ.
// Earlier: pulse/table setup, draw-pass carriers and running-X scheduling remain.
// Complete first index 209; absent-index guard 70; both 209; bit after query 70;
// first/draw/both image field bindings 70/70/70; complete first index with both fields 209.
// pulse/table setup, row X and query/width scheduling remain.
// Half-height after center advance 39; chained quad stores 86; center scalar/point 134/190;
// completed bounds 133; bounds with late half-height 62; center and bounds 137; center through draw 134.
// Earlier at 39 (from 39): table/pulse setup, row-X carrier and query/width schedule remain;
// Earlier: table/pulse setup, row-X carrier and query/width schedule remain;
// query before sprite 39; horizontal/vertical/both coordinate spans 39/39/39;
// direct query product right/left 39/39; query with spans 39; spans/direct query 39.
// Round 277: draw-shared quad/rectangle/both and pre-center quad/rectangle/both 39 each;
// signed-word selection mask 169; read-only computed draw scalars 39.
// Earlier at 29 (from 39): pulse/table setup, rectangle source address and occupied/height registers differ.
// Two-use width/query operands 35; height before advance with those operands 29 kept;
// horizontal/vertical output phases 112/111; operands with horizontal phase 109;
// coordinate source base 37 (rejected); native output names 39; operands with names 35.
// Earlier at 29 (from 29): table/pulse setup, rectangle source and occupied/height registers remain;
// rectangle memcpy 29; native zero rectangle 132; both 132; occupied result 70; copy/result 70;
// zero/result 156; all boundaries and native quad 156/156.
// Earlier at 14 (from 29): the prologue now matches (per-player record base with the
// Earlier: pulse/table setup, rectangle source address and occupied/height registers differ.
// Earlier: table/pulse setup, rectangle source and occupied/height registers remain;
// Earlier: the prologue now matches (per-player record base with the
// pulses at +0x2c); the draw pass's bounds source (ADD/LEA) and the advance/half-height
// XMM3/XMM4 roles remain. Bounds record pointer local 12 (once-used, not kept);
// query-first advance 12; half-height first 24; named advance 53; advance as
// product + x 12; quad declared first 12; dimension names 12; half-height declared
// first 12; halving as division 12; a 600 s permuter run, none below 12.
// Frame and query schedule agree; pulse/table acquisition, draw-copy ADD versus
// LEA and occupied-width/height SSE roles remain different; native draw bounds 29; native width
// bounds 29; native bounds records 29; complete native quad 29; quad and draw bounds 29; quad and
// width bounds 29; complete native boundaries 29; vertical corner order 32.
// pulse/table setup, rectangle source and occupied/height homes remain;
// dimension names, entry height/width, height/width at use and vertical/horizontal/early vertical bounds
// 29, 29, 29, 35, 200, 47, 54, 40.
// Native first/draw/both sequence records and full image-group fields 14 each; occupied width
// before half-height 28; combined draw/both/group source types with early occupied width 28 each.
// source fields 144; native quad 14; fields and quad 144; initialized quad 14;
// fields and initialized quad 144; input record fields 152; input record and quad 152;
// input record and initialized quad 152.
// Round 7: real-pointer bounds and memcpy 14 each; staged both/width/height conversions 85/72/88;
// width/height product splits 26/14; final-address pointers with split/const/original dimensions 100 each;
// record-base pointer 12 (not kept); with const dimensions/height split/width split 12/12/24;
// height at each use 18.
// Round 13, all on the bounds-record pointer (12): half-height after the advance 18; corners from
// a bounds rectangle 37; corner 3 before 1 and 2 16; shared corner values copied from the quad 28;
// pulse-first products 12; conversion-first extents 12; y before x in each corner 20; pulse query
// after the extents 53; half-height/half-width written at each use 18/189. The advance product
// takes XMM4 and the half-height copy XMM3 under every statement order (retail XMM3/XMM4).
// Round 41, all on the bounds-record pointer (12): pulse/half-width renamed to sort last 12/12; const
// extents 12; corner stores in a block scope 12; named advance 53; final/both steps as `x = product + x`
// 12/12; final/both products with swapped operands 12/12. Without the final x step (diagnostic only) the
// draw block takes retail's XMM3/XMM4, so the advance product's spill across the calls sets the colouring.
// Round 52: both passes' sequence lookup as one force-inline helper (first bitmap and first sprite's
// bounds) 244 (868 bytes); that helper in the draw pass only 14 (same code); both advance products as
// pulse * half width 14; width/height conversions first in the half extents 14; the default bounds
// set field by field 14. Retail keeps the advance in XMM3 and the half height in XMM4 (ours the reverse)
// after the 0.5 constant in XMM3 and the converted height in XMM4 both die.
// @retail 0x200462
void function_200462(long arg_0)
{
    byte *local_0 = (byte *)g_510c4c + arg_0 * 0x6c;
    real local_2 = 0.0f;
    dword local_3 = 0;
    long entry_index;
    for (entry_index = 0; entry_index < 9; entry_index++)
    {
        s_interface_pulse *local_6 = (s_interface_pulse *)(local_0 + 0x2c) + entry_index;
        byte *local_5 = *(byte **)((byte *)g_510c94 + 0x404) + entry_index * 0x1c;
        if (*(long *)(local_5 + 4) == NONE || *(long *)(local_5 + 0xc) == NONE ||
            (!local_6->delay && !local_6->ticks))
            continue;
        byte *local_7 = g_4e3b44[*(long *)(local_5 + 4) & 0xffff].bytes;
        long sprite_index = NONE;
        real local_9 = 1.0f;
        if (*(short *)(local_5 + 0x10) >= 0 && *(short *)(local_5 + 0x10) < *(long *)(local_7 + 0x3c))
        {
            byte *local_10 = *(byte **)(local_7 + 0x40) + *(short *)(local_5 + 0x10) * 0x3c;
            sprite_index = *(short *)(local_10 + 0x20);
            if (*(long *)(local_10 + 0x34) > 0)
            {
                byte *local_11 = *(byte **)(local_10 + 0x38);
                local_9 = *(real *)(local_11 + 0xc) - *(real *)(local_11 + 8);
            }
        }
        else if (*(short *)(local_5 + 0x10) == 0 && *(long *)(local_7 + 0x3c) == 0 && *(long *)(local_7 + 0x44) > 0)
            sprite_index = 0;
        if (sprite_index != NONE)
        {
            local_3 |= 1 << entry_index;
            real local_12 = function_2008fc(local_6, true);
            byte *local_13 = *(byte **)(local_7 + 0x48) + sprite_index * 0x74;
            local_2 += local_12 * ((real)*(short *)(local_13 + 4) * local_9);
        }
    }
    point2f local_14;
    function_1396c7(4, &local_14);
    local_14.x -= local_2 * 0.5f;
    local_14.y += 50.0f;
    for (entry_index = 0; entry_index < 9; entry_index++)
    {
        byte *local_16 = *(byte **)((byte *)g_510c94 + 0x404) + entry_index * 0x1c;
        if (!(local_3 & (1 << entry_index)))
            continue;
        byte *local_17 = g_4e3b44[*(long *)(local_16 + 4) & 0xffff].bytes;
        short local_18 = *(short *)(local_16 + 0x10);
        long bitmap_index = NONE;
        s_float_rect local_19 = {0.0f, 1.0f, 0.0f, 1.0f};
        if (local_18 >= 0 && local_18 < *(long *)(local_17 + 0x3c))
        {
            byte *local_20 = *(byte **)(local_17 + 0x40) + local_18 * 0x3c;
            bitmap_index = *(short *)(local_20 + 0x20);
            if (*(long *)(local_20 + 0x34) > 0)
                local_19 = *(s_float_rect *)(*(byte **)(local_20 + 0x38) + 8);
        }
        else if (local_18 == 0 && *(long *)(local_17 + 0x3c) == 0 && *(long *)(local_17 + 0x44) > 0)
            bitmap_index = 0;
        if (bitmap_index == NONE)
            continue;
        byte *local_21 = *(byte **)(local_17 + 0x48) + bitmap_index * 0x74;
        s_interface_pulse *local_22 = (s_interface_pulse *)(local_0 + 0x2c) + entry_index;
        real local_23 = function_2008fc(local_22, true);
        real local_24 = (local_19.x1 - local_19.x0) * (real)*(short *)(local_21 + 4) * 0.5f;
        real local_25 = (local_19.y1 - local_19.y0) * (real)*(short *)(local_21 + 6) * 0.5f;
        local_14.x += local_24 * local_23;
        point2f local_27[4];
        local_27[0].x = local_14.x - local_24;
        local_27[0].y = local_14.y - local_25;
        local_27[1].x = local_14.x + local_24;
        local_27[1].y = local_14.y - local_25;
        local_27[2].x = local_14.x + local_24;
        local_27[2].y = local_14.y + local_25;
        local_27[3].x = local_14.x - local_24;
        local_27[3].y = local_14.y + local_25;
        g_4e6950.field_0 = *function_13927e(g_4b9ed8);
        g_4e6950.field_c = *g_468720;
        g_4e6950.value18 = 0.0f;
        g_4e6950.value20 = (real)local_22->delay * g_510c54->rate * 2.0f;
        g_4e6950.value1c = function_200891(local_22, false) * g_4e69c0[g_4b9ed8];
        function_22a664((s_widget_quad_2b11 const *)local_27, &local_19,
            *(long *)(local_16 + 4), bitmap_index, *(long *)(local_16 + 0xc));
        local_14.x += local_24 * local_23;
    }
}
