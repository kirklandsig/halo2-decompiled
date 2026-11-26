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

// score 14 (was 14): r131, r232 and r234 unchanged. Draw bounds source ADD/LEA and occupied-width/half-height XMM homes differ.
// r133 unchanged. Draw bounds source ADD/LEA and occupied-width/half-height XMM homes differ.
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
// Round 60, on the bounds-record pointer (12): named left/right/top/bottom 37; named top/bottom after the x step 30,
// before it 23; advance named between the half extents 26; named advance 53; scalar center copies 12; a corners
// rectangle 37. Named values flip the running position's XMM6/XMM7 before they reach XMM3/XMM4.
// Round 50 (m3 lane 2, fast): half extents through a force-inline (span, size) helper 85 (846 bytes), through force-inline rectangle width and
// height helpers 85 (846 bytes); the delay's seconds through a force-inline ticks helper 14 (same code); the corners through a force-inline
// (quad, x, y, half width, half height) helper or a (quad, center pointer, ...) helper 14/14 (same code); the two x steps through a force-inline
// (position, half width, pulse) helper 14 (same code); named advance with/without the bounds-record pointer 53/55.
// Round 92 (m3 lane 12, fast): any `__assume` anywhere (float bounds on the pulse, half extents, x, y or a named advance; integer and pointer
// forms) 73 at 848 bytes, with the bounds-record pointer 71: the first loop's base moves and XMM3/XMM4 stay. Named first advance with a raw final
// product, before/after the half height 14/14; half height after the first x step 20, with the bounds-record pointer 18 (the advance takes XMM3, the
// order is lost); named advance before that step 20; a copy of the half height 14; the final product named after the first draw setup 14.
// Round 96 (m3 lane 12, fast, all on the bounds-record pointer, re-measured 12): both x steps as `x = x + product` 12; named advance
// used by both steps 53 (the draw block reschedules and the pulse query moves); the 0.5 factor as a named const 12; the default
// rectangle declared before the bitmap index 12. The advance product still takes XMM4 and the half-height copy XMM3. Without the
// pointer: the converted width/height first in each half extent 14 (same code).
// Round 96 (m3 lane 16, fast control 14): the bitmap entry pointer read at each use 120 (852 bytes); the
// sequence index read at each use 14; the default rectangle set field by field 14; half height computed before
// half width 26; the draw pass's sequence through the existing bitmap-sequence type with the first sprite's
// bounds as `sprites->bounds` / `sprites[0].bounds` 14/14 (the copy source stays ADD). The two XMM3/XMM4
// values are the advance product (then the new x, in place) and the half-height copy (then y + half height).
// Round 64 (m3 lane 2, fast): permuter on this body (600 s, 50 tries) best 14; the corner array declared at the top
// of the draw loop 14; only the first step's product as pulse * half width 14; the draw arm as `if (bitmap_index !=
// NONE) { ... }` in place of `continue` 14; one sprite index variable for both passes 14 (all same code); the
// bounds-record pointer with that positive arm 12 (the pointer's own gain; XMM3/XMM4 unchanged, not kept).
// Round 109 (m3 lane 16, fast control 14): the half extents declared before the draw loop 14; as one 2-D vector local 14;
// the pulse pointer computed at the top of the draw loop 14; the bounds as the existing box2f type 14 (all four the same
// code); a separate draw-loop counter 38. Matched siblings 0x22bbca/0x24f0fc build their quads in a four-corner loop,
// which retail keeps as a loop there; retail's eight straight stores here rule that form out.
// Round 111 (m3 lane 16, fast control 14; all on the bounds-record pointer, re-measured 12): the eight corner stores as all x then
// all y 87, all y then all x 86, corners 0, 3, 1, 2 16, the two top y stores first 87, x stores 0, 3, 1, 2 then the y stores 87,
// the y stores then x stores 0, 3, 1, 2 87 (846 bytes where 86-87); named x and y spans after or before the pulse query 12/12,
// named sprite width and height 83 (846 bytes), full extents named before halving 12, only the height 12, `0.5f * (...)` for the
// half height or both 12/12 (12 = same code); the advance assigned inside the first x step (`x += advance = ...`), declared at the
// step, at the loop top or at function scope, or as `x = (advance = ...) + x`, 53 each (the named-advance code). Re-measured with
// the listing: half height at each use 18 (the advance then takes XMM3 and the half-height copy XMM4 as retail, but the half
// height is computed after the x step, the 0.5 moves to XMM7 and x to XMM4). Diagnostic: the advance kept beside the position in
// one three-real local 53 (same as a named advance). The two free registers at the product are XMM3 and XMM4 in both builds;
// retail gives the product the lower one, ours the higher.
// Round 162 (m3 lane 8, fast with 0x2008fc 0x200891; control 14): each corner copied from the running position and then
// offset (0x24ecae's idiom) 181 (921 bytes); the pulse pointer computed before the bitmap entry pointer 14 (identical).
// A compile-only build without /GL does not reproduce the draw block (the pulse helper's return register differs).
// Round 122 (m3 lane 9, fast with 0x2008fc 0x200891; control 14): 0-7 unused locals at the top 14 at every phase; the draw
// pulse, bitmap entry and sequence entry pointers each as pointers to const 14/14/14; the corner array declared at function
// scope 14 (all same code).
// M3 lane 16 r121 (fast with 0x2008fc 0x200891, control 14): the sprite width and height converted into named reals first 85
// (846 bytes); a block-local center point for the corners with the running x stored once after the draw call 85 (852 bytes);
// the bounds copy as memcpy 14 (same code: ADD stays); the second step's product commuted 14 (same code); the running
// position declared first in the function 14 (same code). Re-measure: named advance 55 (the pulse call then sinks below the
// half extents).
// Round 166 (m3 lane 7, fast with 0x2008fc 0x200891, control 14): the first pass's width scale defaulted in an else arm
// 214 (856 bytes); the corner array as a four-point aggregate initializer 14 (same code); the second x step moved up to just
// after the corners 98 (843 bytes, the product spill goes); one pulse pointer at the draw loop's top ahead of the entry
// pointer and mask test 14 (same code); the first pass's bitmap tag index named once for its test and lookup 14 (same code).
// Round 111 (m3 lane 10, fast with 0x2008fc 0x200891, control 14; on the bounds-record pointer, re-measured 12, where only the
// advance/half-height XMM3/XMM4 pair differs): one pulse-array pointer at the top for both loops 27 (the prologue moves); the running
// position as two scalar reals 36 (frame 0x54); the vertical +50 step before the horizontal centring 19; the lower corners' y as
// `half height + y` 12, with the right corners' x as `half width + x` too 12 (same code). Re-measured: half height after the first
// x step 18 (x moves to XMM4 and 0.5 to XMM7).
// Round 189 (m3 lane 5, fast with 0x2008fc 0x200891; control 14): the repeated corner values read back from the corner
// just stored (`[1].y = [0].y`, `[3].y = [2].y`) 30, the x pair the same way 32, all four 30 (the x/y spans swap XMM1/XMM2);
// the first sprite record through a two-use force-inline accessor in both passes 12 (retail's `lea esi, [eax + 8]` copy
// source, as the bounds-record pointer; only the XMM3/XMM4 pair left; not kept, 2 lower without a match); that accessor
// with the y read-backs 28. All restored.
// Round 93 (m3 lane 2, fast; on that two-use first-sprite accessor, re-measured 12 with only the XMM3/XMM4 pair left):
// the half factor applied to each span before the sprite size 12 and the sprite size first in each half extent 12
// (both reassociated to the same code); the four edges named after the x step (left, top, right, bottom) 12 (same
// code); the lower and right edges named first 91 (852 bytes). All restored to the body without the accessor.
// Round 106 (m3 lane 14, fast with 0x2008fc 0x200891, control 14): the corners stored through a trivial force-inline
// point setter 14; the first x step through a reference bound to the running x 14; the half height and the corners in
// their own block scope 14; the last corner's y read back from the corner before it 14 (forwarded); descriptive names
// for the pulse and the half extents 14 (all five the same code: the advance product keeps XMM4, the half-height copy
// XMM3, so the pick is not a name tie).
// Round 210 (m3 lane 4, fast with the bounds record through a one-use pointer, re-measured 12 with only the XMM3/XMM4
// pair left): the half height computed before the half width 24 (its copy goes straight to XMM3, the advance keeps XMM4);
// the first x step as `x = advance + x` 12 and both steps' products commuted 12 (same code); every corner's y stored
// before the x values 86 (846 bytes); the corners stored in [0], [3], [1], [2] order 16. All restored to the body
// without the pointer.
// Round 212 (m3 lane 4, fast with 0x2008fc 0x200891; control 14): the first sprite record through a two-use forceinline
// accessor in both passes re-measured 12 (round 189's form; an accessor returning the bounds rectangle itself keeps the ADD,
// 14); on the record accessor, every form the same code at 12 (the advance product still XMM4, the half-height copy XMM3):
// the corners through a `point2f const &` to the running position; the half height through a `real const &` in the corners;
// the pulse bound by `real const &` to the call's result; both half extents bound by `real const &` to their temporaries;
// only the half width so bound. All restored to the body without the accessor.
// M3 lane 3 r260 (fast with 0x2008fc 0x200891, control 14): both half extents in one declaration 14; the centring steps as
// plain assignments 14; the first pass's width sum as `sum = product + sum` 14; the corner array declared ahead of the first
// x step 14; the first pass's pulse query written in the sum (no pulse local) 14 (all the same code).
// m3 lane 14 r111 (one-file LTCG screen of this file with its callees' objects; it reproduces the XMM3/XMM4 pair, the
// running x takes XMM5 there where the full build has XMM7): 0 to 32 dead initialised locals at the top never move the
// advance product off XMM4 (not a symbol-count tie); the pulse as `real const`, both half extents as `real const`, the
// corners as a flat array of eight reals, all the same code; the bitmap entry pointer computed after the pulse query
// 14 -> worse (the entry load moves past the call); the four corner edges as const locals named after the x step worse.
// M3 lane 4 r216 (fast with 0x2008fc 0x200891, control 14): the stepped x named where it is computed (`real center_x =
// x + half width * pulse;`, stored back and used by the corners) 89 (852 bytes), with the y in a `center_y` too 89; the
// corners through a `point2f const &` bound to the running position after the step 14 (same code); the second x step
// moved ahead of the draw call 97 (843 bytes); the corners in [2], [3], [0], [1] order 90 (852 bytes).
// M3 lane 3 r266 (fast with 0x2008fc 0x200891, control 14): the half height bound to a `real const &` (alone) 14; the half
// width so bound (alone) 14 (both the same code); the step and the corners in a forceinline (corners, center, half width,
// half height, pulse) helper given the half height as an expression (a parameter-only value) 14 (same code: the advance
// keeps XMM4); a named advance with the half height written at each use 24; the bounds-record pointer with the half
// height at each use 18 (the copy source becomes retail's `lea esi, [eax + 8]` and the advance takes XMM3, but the half
// height is formed after the step, x moves to XMM4 and 0.5 to XMM7).
// M3 lane 4 r221 (fast with 0x2008fc 0x200891, control 14): the advance bound by `real const &` to its product and
// used by both x steps 55 (first difference +0x129), by the first step only (the second written out) 51 (850 bytes);
// the half height built in place (`= y1 - y0; *= height; *= 0.5f;`) 14, the half width so 14, both 14 (all three the
// same code: the advance product keeps XMM4 and the half-height copy XMM3).
// M3 lane 2 r112 (fast with 0x2008fc 0x200891, control 14): the half height declared ahead of the half width and assigned
// after it 14 (same code); the half width computed ahead of the pulse query 28; named top and bottom edges (`y - half
// height`, `y + half height`) for the corners 32 (first difference +0x119); the running y or x held in a scalar local
// copied out of the position 36/38 (first difference +0x5).
// M3 lane 8 r197 (fast with 0x2008fc 0x200891, control 14): both half extents halved as `/ 2` 14; the second x step as
// `x = product + x` 14; the right corners' x and the lower corners' y written half extent first 14 (all three the same
// code); the pulse query after the half extents 55 (first difference +0x129); the vertical step as `y = y + 50.0f` 14.
// M3 lane 3 r274 (fast with 0x2008fc 0x200891, control 14): 1 to 7 dead initialised locals at the top 14 each, and 0 to 7
// on the bounds-record pointer 12 each (diagnostic, full image: the XMM3/XMM4 pair is not a symbol-count tie); on the
// bounds-record pointer: both half extents ahead of the pulse query 53, the corner array declared at the draw loop's top 12,
// each corner's y stored before its x 20.
// M3 lane 3 r276 (fast with 0x2008fc 0x200891 0x1396c7 0x22a664 0x13927e, control 14): the draw pass's bitmap entry as the
// bitmap header's typed dimensions element (width/height fields) 14; the draw pass in the typed group/sequence/dimensions
// idiom of 0x24e9c7 (bounds as a box2f local copied from `sequence->sprites->bounds`, cast at the draw call) 14 (both the
// same code: the copy source stays ADD); that with the first sprite through a one-use typed pointer 12 (retail's
// `lea esi, [eax + 8]`; only the XMM3/XMM4 pair left; a one-use pointer, not kept), and with the first pass typed the
// same way as well 12 (the pair unchanged); the corners as four chained assignments (`[3].x = [0].x = x - half width`
// and so on) 89 (846 B).
// M3 lane 1 r135 (fast with 0x2008fc 0x200891, control 14): the whole draw iteration as a forceinline (position, entry,
// pulse base, definition, bounds, bitmap index, entry index) helper 14; the half extents declared bare ahead of the pulse
// and assigned after it 14; the stepped position read into `real const` x and y for the eight corner stores 14; the
// occupied mask as `long` 14 (all four the same code); the first sprite record through a block pointer with named top
// and bottom edges ahead of the x corners 30 (x and y move to XMM5/XMM6). Diagnostic: dead computed copies of the pulse,
// of both half extents, or of the first pass's sum, 14 each (extra temporaries do not move the XMM3/XMM4 pair).
// M3 lane 8 r205 (fast with 0x2008fc 0x200891, control 14): the corner array declared after the first x step; each half
// extent written converted size first (`size * (x1 - x0) * 0.5f`); the corners' y written half height first (`h + y`,
// `-h + y`); the first x step as `x = x + pulse * half width`; the half extents read through a `s_float_rect const &` to the
// bounds: 14 each (all the same code: the advance product keeps XMM4, the half-height copy XMM3).
// M3 lane 11 r135 (fast with 0x2008fc 0x200891, control 14; register-masked aligned difference 1, the ADD/LEA copy
// source alone): both x steps through a forceinline (position, half width, pulse) helper 14; the half extents built in
// a two-real aggregate and read back into the two locals 14; the first step through a `real &` bound to the running x
// 14; the first pass's scaled sprite width named where it is computed 14; the first pass's width scale declared ahead
// of the sprite index 14 (all five the same code).
// M3 lane 6 r215 (fast with 0x2008fc 0x200891, control 14): the draw pass's sprite height read through a new trivial inline
// (entry, offset) accessor 14, the width so 14, both 14; both half extents through a new inline (low, high, size) helper 14;
// the pulse pointer written out at each of its three uses (no local) 14 (all five the same code: the advance product keeps
// XMM4 and the half-height's new home XMM3). Re-measured: named upper and lower y after the step 32.
// M3 lane 8 r209 (fast with 0x2008fc 0x200891, control 14): the corners by chained assignment (x0 = x3, y0 = y1, x1 = x2,
// y2 = y3) 89 (846 B); the repeated corner values copied from the corner written first (y1 from y0, x2 from x1, x3 from
// x0, y3 from y2) 30; both passes' bounds through an `s_float_rect const *` to the record's rectangle (pass 1 reading
// x1 - x0 through it, pass 2 copying `*bounds` in a block) 193 (853 B), with the copied corners 193; the corners read
// through a `point2f const &` to the position 14 (same code).
// M3 lane 14 r137 (H2M_FAST with 0x2008fc 0x200891, control 14): the draw pass's mask test as a positive arm 14; the centring
// as `/ 2.0f` 14 (both the same code); the corner y values set ahead of the first x step, the x values after it 37; the draw
// entry pointer formed after the mask test 26; the default bounds declared at the draw loop's top 35. Diagnostics (reverted):
// a live float probe at six places in the draw iteration never moves the advance product off XMM4 (no temp rotation); one
// extra live use of the half height after the corners gives the product XMM3 as retail (the half height then takes XMM5),
// an extra use of the product or of the half width does not, so the open cause is the half height's rank against the
// product. Re-measured: named advance 55.
// M3 lane 2 r125 (H2M_FAST with 0x2008fc 0x200891, control 14; the bounds-record pointer re-measured 12): the half height
// written `0.5f * (span * size)` 14 (same code); on the bounds-record pointer: the four corner x values stored before the
// four y values 87 (846 B); one to three redundant parenthesis pairs around the half width's span, the half height's span,
// the first step's product or the first corner's difference 12 each (twelve forms: the XMM3/XMM4 pair is not keyed on
// expression order); the pulse read through a `real const &` at both steps 12 (same code); the half height computed ahead
// of the pulse query 34 (first difference +0x129).
// M3 lane 8 r211 (fast with 0x2008fc 0x200891, control 14; the first sprite record through a one-use pointer re-measured
// 12, only the XMM3/XMM4 pair left). Diagnostics, restored at once: 0x200891 given one more parameter that its body
// never reads (the link drops such an argument after the caller's common-subexpression pass has counted it; that gave
// retail's scratch-and-copy load in 0x12e290), fed on the 12 form the advance product 12, the half height 12, the
// half width 12, the pulse 12, the lower edge `y + half height` 12, and on this body the advance product 14 (all the
// same code: a dropped second use of a float value does not move the XMM3/XMM4 pick).
// M3 lane 1 r146 (H2M_FAST, control 14): both half extents declared with a dead `0.0f` initialiser ahead of the pulse
// query and assigned after it 17 (the product is then formed in the pulse's register, `mulss xmm0, xmm2`); the pulse so
// declared 14 (same code); the corners read from a `point2f const` copy of the stepped position 118 (866 bytes); the
// default rectangle declared at the draw loop's top, ahead of the mask test 35. Re-measured: a named advance used by both
// x steps 55 (the running x leaves XMM7 and the pulse call sinks below the half extents).
// M3 lane 5 r228 (h2m fast with 0x2008fc 0x200891, control 14): the draw block's five statements (bitmap entry pointer E,
// pulse pointer Q, pulse query P, half width W, half height H) in other orders: E Q P H W 26, Q E P W H 14 (same code),
// Q P E W H 83 (850 B), E Q W P H 28, Q E W P H 28, E Q W H P 55; the draw pass's record pointer named as the first pass
// names its own (one-use there) re-measured 12 (only the XMM3/XMM4 pair left; match-only, not kept).
// M3 lane 3 r290 (H2M_FAST, control 14): the height span named (`(y1 - y0) * height`) and halved at each corner use
// (`span * 0.5f`) 19: the advance product takes XMM3 and the half-height value XMM4 as retail, but the halving is then
// scheduled after the product, so the shared 0.5 takes XMM7 and the running x XMM4; with the bounds-record pointer as well
// 17; the span named ahead of the half width 22; `0.5f * span` at each use 19; the width span named too (half width
// `width_span * 0.5f`) 19. So the pair follows which value is created first, the product or the half height.
// M3 lane 5 r228 (h2m fast with 0x2008fc 0x200891, control 14): the draw block's five statements (bitmap entry pointer E,
// pulse pointer Q, pulse query P, half width W, half height H) in other orders: E Q P H W 26, Q E P W H 14 (same code),
// Q P E W H 83 (850 B), E Q W P H 28, Q E W P H 28, E Q W H P 55; the draw pass's record pointer named as the first pass
// names its own (one-use there) re-measured 12 (only the XMM3/XMM4 pair left; match-only, not kept).
// M3 lane 11 r150 (h2m fast with 0x2008fc 0x200891, control 14): each pass's loop index declared in its own `for` 38
// (first difference +0x2d); the vertical offset applied ahead of the horizontal centring 21; the draw pass's pulse
// pointer formed at the iteration's top, ahead of the bitmap entry, as the first pass does 14; the corner array declared
// at the iteration's top 14 (both the same code); the draw pass's definition lookup ahead of the mask test 31. All
// restored.
// M3 lane 15 r131 (h2m fast with 0x2008fc 0x200891, control 14): the pulse and both half extents declared `real const`
// 14; the first x step written product first (`x = w * pulse + x`) 14; both x steps so 14; the corners as a flat `real[8]`
// array 14; the corner sums written extent first (`w + x`, `h + y`) 14 (all five the same code).
// M3 lane 5 r232 (h2m fast with 0x2008fc 0x200891, control 14; on the draw pass's record pointer, re-measured 12 with
// only the XMM3/XMM4 pair left, unless noted): the four edges named (left, top, right, bottom) after the x step 12; the
// half extents as one `point2f half_size` local 12; both x steps as `pulse * half_width` 12; the corners' half height
// through a `real const &` 12 (all four the same code); the half height computed after the first x step 18 (the
// product then takes XMM3 as retail, but the halving constant moves to XMM7 and the running x to XMM4); the half height
// declared beside the half width and assigned after the step 18; on main, the half height after the step 20. All restored.
// M3 lane 5 r234 (h2m fast with 0x2008fc 0x200891, control 14): the sprite size read through a `short const *` used for
// both the width and the height 121 (853 B); the default bounds set by four assignments 14 (same code), by two chained
// assignments (zeros, then ones) 20; the first x step alone written `pulse * half width` 14; both x steps as plain
// assignments 14 (both the same code). Reading: the zero stays in XMM5 across the whole iteration, so only XMM3/XMM4 are
// free for the advance product and the half-height copy. All restored.
// M3 lane 15 r133 (h2m fast, control 14; the draw pass's record pointer re-measured 12, aligned 12 unmatched rows, only
// the XMM3/XMM4 pair): the half height declared after the first x step 20; that on the record pointer 18 (the product
// then takes XMM3 and the half-height copy XMM4 as retail, but the product is formed ahead of the half height's
// chain, so the shared 0.5 moves to XMM7 and the running x to XMM4); with the corner array declared ahead of the half
// extents as well 18; the x corners, then the half height, then the y corners 92 (846 B). On the record pointer: the
// lower and upper corner points named and the four corners filled from their members 12; the half extents as one
// two-member extent local used by both steps and the corners 12; the stepped y read into a local for the y corners 12
// (all three the pointer form's code).
// M3 lane 14 r147 (h2m fast with 0x2008fc 0x200891, control 14): the advance bound by `real const &` to the product and
// used by both x steps 55; the four corner y values stored ahead of the first x step, the x values after it 37; the half
// height grouped `span * (height * 0.5f)` 87 (846 B); the pulse query between the half width and the half height 28; on
// the bounds-record pointer (re-measured 12, only the XMM3/XMM4 pair left) the advance bound by `real const &` 53. All
// restored.
// M3 lane 15 r135 (h2m fast with 0x2008fc 0x200891, control 14): on this body the four edges named (left, right, top,
// bottom, each used twice) ahead of the corner stores 39 (first difference +0x119); the bitmap test as a positive arm
// around the rest of the draw iteration 14; the half height declared bare ahead of the half width and assigned after it
// 14; the corner array and the draw call in their own block 14; the corner array declared at function scope beside the
// position 14 (the last four the same code). All restored.
// M3 lane 6 r236 (h2m fast with 0x2008fc 0x200891, control 14): both half extents as `0.5f * span * size` 14; only the
// half height so 14; both as `size * 0.5f * span` 14; corners 2 and 3 copying corner 1's and 0's x and corner 3 copying
// corner 2's y 32; the bounds copy source read as a `long` and offset as an integer 14 (the ADD stays). All restored.
// M3 lane 13 r151 (h2m fast with 0x2008fc 0x200891, control 14; the bounds record through a one-use pointer re-measured
// 12): the four corners through a new forceinline (corners, center, half width, half height) helper taking the extents
// by value 14, on the record pointer 12 (same code: the copies fold); the half height's `* 0.5f` as its own statement
// after the first x step 20, on the record pointer 18 (the pair becomes retail's but the 0.5 stays live in XMM7 and x
// moves to XMM4); one function-scope sprite-record pointer shared by both passes 12 (the one-use pointer's code); the
// top and bottom y edges named before the first x step 25, on the record pointer 23; only the left and right x edges
// named after the step 39, on the record pointer 37. All restored.
// M3 lane 12 r191 (h2m fast with 0x2008fc 0x200891, control 14; the bounds-record pointer re-measured 12): on the pointer
// the two half extents in one declaration 12 (same code); the pulse query after both half extents 53; the bitmap record
// pointer formed after the pulse query 81 (850 B); the pulse pointer formed at the draw loop's top 12 (same code). Reading
// of the allocator's ranking on the pointer form: the half-height copy (copy, `y - h`, `h + y` in place) is coloured in the
// first pass and takes XMM3; the advance product is used again after the draw call, so its whole range is split and only
// its draw-block piece is coloured, in the second pass, taking XMM4 (the same 162 as the copy); retail's product takes
// XMM3, so its piece outranks the copy in retail's first pass. A forced-ranking search over 30 decisions found nothing.
// Roasted-Codes (#1508 comment, 2026-11-26; fast reduced-image scores): the named advance with the pulse result bound by
// `real const &` ahead of it 15 (849 B); the query and the centring match, so the named advance's 53 is the pulse query
// sinking below the half extents. With the advance moved after the half height on that body: 12, the same XMM3/XMM4 pair.
// M3 lane 1 r172 (h2m fast with 0x2008fc 0x200891, control 14; the bounds record through a one-use pointer re-measured 12):
// on the pointer form the pair is an exact tie in the allocator: the half-height copy (first pass) and the draw block's
// piece of the advance product (second pass) both rank 162 (a reference cost of 9 in a block of 18 candidates), and the
// copy, coloured first, takes XMM3. A forced-ranking search over the two finds one match: the copy taking XMM4. So one
// reference more for the advance piece, or one fewer for the copy, inside the draw block is what has to move. Forms, on
// the pointer: the corners as four chained assignments (left, top, right, bottom) 87 (846 B), grouped x then y 87 (846
// B); the bottom edge as `local_25 + local_14.y` 12, the right edge as `local_24 + local_14.x` 12; the second x step as
// `local_14.x = local_24 * local_23 + local_14.x;` 12 (the 12s the same code). All restored.
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
