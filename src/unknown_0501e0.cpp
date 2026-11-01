// @flags /O2 /arch:SSE /Gr
#include "projection_storage.h"
#include "unknown_11c920.h"
#include "camera_storage.h"
#include "unknown_0259d0.h"
#include "unknown_030290.h"
#include <math.h>
#include <string.h>

real function_30bf0(vector3f *arg_0);

struct s_501e0
{
    point3f field_0;
    real field_c;
    real field_10;
    dword field_14;
};

static __forceinline void world_vector_to_view(real arg_0, vector3f const *arg_1, vector3f *arg_2)
{
    real local_0 = arg_1->i;
    real local_1 = arg_1->j;
    real local_2 = arg_1->k;
    if (arg_0 != 1.f)
    {
        local_0 = arg_0 * local_0;
        local_1 = arg_0 * local_1;
        local_2 = arg_0 * local_2;
    }
    real *output = arg_2->n;
    for (long component = 0; component < 3; ++component)
    {
        *output = g_4b9e14.forward.n[component] * local_2 + g_4b9e14.up.n[component] * local_1 + g_4b9e14.right.n[component] * local_0;
        ++output;
    }
}
// score 221 (was 221): first transform sum/product order and register numbering (retail: forward*depth +
// up*height first, coefficients loaded into registers, right.i kept in xmm7 for both views), frame 0x18 versus
// 0x14 (retail spills the blend scale at [esp] where it is computed). Earlier: transform term order
// right/up/forward written point first and
// frame 0x18 versus 0x14; coefficient/input/result overlap remains.
// Earlier: transform term order right/up/forward written point first and
// coefficient first 221 each; the transform as three statements 304, which does use retail's
// sum tree (forward*depth + up*height) + right*width; blend term order and squared-sum order
// 221 each; depth limits written at their uses 230; orthographic dot product in point and
// coefficient order 221 each and depth first 220. What still differs: the loop form keeps the
// sum tree (right*width + up*height) + forward*depth while retail has forward*depth +
// up*height + right*width; retail materialises the coefficients of the first view in registers
// and keeps the second point's offsets in the frame, ours folds the coefficients into the
// products and keeps the second point's width in a register; frame 0x18 versus 0x14.
// Earlier note: frame 0x18 versus 0x14; retail keeps both transformed points in
// registers and the second point's offset (with its scaled copy) in the frame, ours gives
// the second view's x a frame slot of its own at [esp+8].
// both inlined transforms, keeps the second point's offset in the frame and the first in
// registers, and scales copies of the inputs; coefficient/result/tail homes differ. Tried:
// global optimization off 961; depth zero 555; frame pointer 300; input-first products 255;
// k/j/i transform output 272; offset/transform/blend loops 315/291/255; paired loops 237; depth
// bounds 237; both 227; paired loops with blend/unsigned/in-place 236/237/287; scoped bounds 230;
// literal zero 227; bound scalar homes 227; separate offset loops 226; output cursor 225; short
// transform index 405; first/second bound at uses 225/236; bound declaration order 227;
// separate offsets with output cursor 221 (kept); short index/first bound 397; limits after
// depth guards 224; limits after first depth 283; explicit tail clamp 221; constant weight arm
// first 221; delayed limits tail clamp 224; delayed limits weight arms 224; first depth tail
// clamp 283; depth weight clamp lifetimes 224; value direction 348; reference direction/output
// 221/221; input-first scale products 221; selected scaled inputs 275; short offset counters 277;
// byte transform counter 394; value direction with reference output 348; on the output-cursor
// form: forward/up/right snapshots 275/275/269; all coefficients before/after scale 453/448;
// shared scale 252 at 1136 bytes; early/late coefficients with shared scale 478/477; first/
// second/both endpoint clip results 262/280/278; first clip before second view 282; second clip
// first 282; incremental clip result 222; both view/clip boundaries 285; cone rejection results
// 278; shared origin on the separate-offset/output-cursor body 290; first/second/both endpoint
// memcpy producers 298/297/334; both inputs before offsets 335; both input copies with origin
// 287; first/second input copy with origin 264/266 (1136 bytes); descending first/second/both
// displacement indices 220/260/260; first/second/both displacement destination cursors 224/
// 224/226; all axis products 221; scalar-role homes 221; uniform transform first-pair sum/last
// product/row result 227/284/221; pair with last/row, last with row, all, and all product parts
// 222/227/284/222/222; consistent direct scale-field observations with input/output/both
// references and actual output-result contracts in all eight combinations 224 each (1148
// bytes); native post-normalization projection axis with all-product terms, shared point
// difference and same-group ordered dot expression in eight combinations 286 each (1101 bytes,
// frame 0x2c); native vector return with pointer/reference/value input and cursor 278/278/401,
// with indexed result 280/280/404; scaled input vector with output pointer/native return 312/
// 404; actual output-end !=/< with long/dword ordinals 406 each, short/word ordinals 378 each.
// Round 140: offsets through vector3d_from_points3d 220 (one point, not kept); second offsets
// first 223; transform rows as three statements 309 (1078 bytes), with the vector3d offsets 307;
// shared scale local with the vector3d offsets 252 and with second offsets first 253 (1136
// bytes); scaled values read from the input pointer 221, with shared scale 252.
// Frame 0x18 versus 0x14; displacement/coefficient/result overlap remains.
// Earlier: Frame 0x18 versus 0x14 and coefficient/input/result overlap remain.
// Frame 0x18 versus 0x14 and coefficient/input/result homes remain.
// frame 0x18 versus 0x14 and coefficient/input/result overlap remain.
// Global optimization off 961; depth zero 555; frame pointer 300;
// input-first products 255; k/j/i transform output 272.
// Offset/transform/blend loops 315/291/255; paired loops 237; depth bounds 237; both 227.
// Paired loops with blend/unsigned/in-place 236/237/287;
// scoped bounds 230; literal zero 227; bound scalar homes 227 (plain).
// Separate offset loops 226; output cursor 225; short transform index 405;
// first/second bound at uses 225/236; bound declaration order 227;
// separate offsets with output cursor 221 plain (1145/1136 bytes); short index/first bound 397.
// This round: limits after depth guards 224; limits after first depth 283; explicit tail clamp 221;
// constant weight arm first 221; delayed limits tail clamp 224; delayed limits weight arms 224; first
// depth tail clamp 283; depth weight clamp lifetimes 224.
// Value direction 348; reference direction/output 221/221; input-first scale products 221;
// selected scaled inputs 275; short offset counters 277; byte transform counter 394;
// value direction with reference output 348. Retained separate offset loops and cursor restored.
// Earlier: extra frame dword and coefficient/result/tail homes remain.
// Earlier at 221 (was 221): extra frame dword and coefficient/result/tail homes remain.
// On the output-cursor form: forward/up/right snapshots 275/275/269;
// all coefficients before/after scale 453/448; shared scale 252 at 1136 bytes;
// early/late coefficients with shared scale 478/477.
// frame 0x18 versus 0x14 and coefficient/result/tail homes still differ.
// Earlier at 255 (from 255): frame 0x20 versus 0x14 and transform coefficient/result registers differ.
// Earlier at 227 (from 255): frame 0x18 versus 0x14; coefficient/result registers and tail differ.
// Earlier at 221 (from 227): frame 0x18 versus 0x14 and coefficient/result/tail homes still differ.
// First/second/both endpoint clip results 262/280/278; first clip before second view 282;
// second clip first 282; incremental clip result 222; both view/clip boundaries 285;
// cone rejection results 278. Clip-result forms restored.
// Shared origin on separate-offset/output-cursor body 290; first/second/both endpoint
// memcpy producers 298/297/334; both inputs before offsets 335; both input copies with
// origin 287; first/second input copy with origin 264/266 (both at 1136 bytes). All restored.
// Descending first/second/both displacement indices 220/260/260; first/second/both displacement destination cursors 224/224/226; all axis products 221; scalar-role homes 221. Extra frame dword and coefficient/result homes remain; one-point descending-first form rejected.
// Round 230: Uniform transform first-pair sum/last product/row result 227/284/221;
// pair with last/row, last with row, all, and all product parts 222/227/284/222/222.
// Round 132: consistent direct scale-field observations, with input/output/both references
// and actual output-result contracts in all eight combinations, 224 each (1148 bytes).
// Original input copies, separate offset loops, output cursor and scalar parameter restored.
// Native post-normalization projection axis, with all-product terms, shared
// point difference and same-group ordered dot expression in eight combinations:
// each 286 at 1101 bytes, frame 0x2c. Original helper and body restored.
// Native vector return with pointer/reference/value input and cursor 278/278/401;
// with indexed result 280/280/404; scaled input vector with output pointer/native return 312/404. Original helper and body restored.
// Round 146: actual output-end !=/< with long/dword ordinals 406 each and short/word ordinals 378
// each; original ordinal-controlled helper restored.
// Lane 8 r58: second offset vector reused for the normalized segment (retail's [esp+8] slots,
// 1136 bytes) 261; with it: scaled arm reading the input 261, if/else scaled copies 297,
// scaled vector copy 326, camera-pointer helper rows/loop 292/258, plain rows 295, k/j/i
// copies 260, k/j/i copies and scaling 259, k/j/i scaling 250; k/j/i scaling alone 245,
// k/j/i copies and scaling alone 257.
// Round 345: Live forward/up/right coefficient cursors 241/253/253; their pairs/all
// 253/253/253/254; all with descending direct rows 267. Forward alone gives frame
// 0x14 and 1136 bytes but regresses to 241; original helper and body restored.
// Round 196: Descending transform rows with long/int/short/signed-byte ordinals: direct output
// 275/275/407/407; descending output cursor 275/275/394/394. Original helper/body restored.
// Round 259: Independent raw/scaled component families, explicit arms ijk/kji
// 305/305, with shared scale 302/302; default-first ijk/kji 221/221, with
// shared scale 252/252. Original helper/body restored; explicit arms grow frame to 0x20.
// Round 358: Input-copy/scale orders ikj, jik, jki, kij 220/221/257/220; with live forward
// coefficients 253/241/253/253. One-count gain rejected; exact-length 241 still regresses. Original
// helper/body restored.
// Round 23 on the forward-cursor body: shared scale 242; reference scale parameter 243;
// scale-field pointer 264; descending first displacement 241; scale pointer and descending
// first 262; explicit second/first-kij displacement components 270/280; scale-last helper 241.
// Original helper and body restored.
// Round 241: orthographic segment in first raw offset/first view/second view 272/220/220;
// with complete k/j/i input/scale order 284/257/257; first-offset/first-view reuse with
// shared segment difference 221/220. One-point gains rejected; original body/helper restored.
// DL175: Helper axis homes, displacement/view homes and early uninitialized tail scratch in seven
// subsets [221, 221, 221, 221, 221, 221, 221]; helper horizontal/vertical/depth homes 221. Original
// helper and body restored.
// Round 205: scale-output-input and output-scale-input helper orders, each
// void/returned-output/output-reference/both-reference contracts, all eight 221.
// All existing helper formals and both real calls restored; extra frame dword remains.
// Round 304: complete camera ownership, separate/camera-owned scale: helper-local 334/375;
// by-value 334/375; caller pointer 361/344; caller reference 361/344.
// Original camera observations, helper and body restored; extra frame dword remains.
// Independent scale overwrite orders ikj/jik/jki/kij with original ijk captures 244/222/222/244;
// with live forward coefficient stream 250/241/250/250. Extra frame word and coefficient/result
// overlap remain; original helper/body restored.
// Round 452: shared vector scale copied/in-place, branch-owned, copied/source and library-copy
// inputs 309/327/330/309; with live forward coefficients 298/314/318/298. Native
// scalar consolidation does not match; original scalar helper and body restored.
// T47: Plain weighted-point blend loop on retained separate-offset/depth-limit graph,
// long/dword/short/word counters: view cursor 222/222/271/271; indexed view output
// 226/226/271/271. Original complete helper and body restored.
// Round 27 (lane 14): scaled copy + the existing matrix_transform_vector 309 (1179 bytes, copies in memory);
// weights as `local_4 = local_5 = 1.f` 221 (same code); orthographic clamp written inside the sqrt argument
// 215 (retail clamps into a fresh value: input stays in xmm0, the result is written per arm; six points, not
// kept); with the dot through the existing dot3f either argument order 215/215; with the zero local dropped
// 215 (aligned 230). Kept: dot3f for the hand-accumulated dot and 0.f in place of the zero-only local 221
// (aligned 238 -> 232).
// Round 9 (lane 17): a compile-only /O2 screen reproduces this body op for op; in it, every definition order,
// written sum order and product operand order of the helper scores within 181-190 aligned (current 184), so
// sum and product order are canonicalised. Retail's first view copies each raw offset to a second register
// before the scale test and computes `scale * raw` into that copy (the raw value stays live); no screened
// form keeps the copy apart. Real (fast): the explicit if/else arms again 305 (1078 bytes, aligned 162);
// arms with the camera passed by pointer 302 (1081 bytes); arms with i, k, j definitions 305; arms with
// three-statement rows and the camera pointer 310; a scaled copy through the existing vector3f_scale (in
// place) and the existing matrix_transform_vector 285 (1160 bytes).
// Round 460: Complete mutable/immutable view-depth carriers with original/branch/conditional/
// immutable-conditional weight producers, 221/221/249/249/221/221/249/249. Original helper and body restored;
// extra frame word and coefficient/result homes remain.
// Round 49 (m3 lane 1, fast): the view helper `static inline` 221 (same code); the two view calls swapped
// 262 (1154 bytes); the helper rows written to `arg_2->n[component]` 226 (1148 bytes); the orthographic
// clamp into a fresh named value 215 (retail's per-arm tail; six points, not kept), as an if/else-if
// clamp 221. On the 215 form the tail differs mostly by the frame's extra dword (every [esp+N] off by 4),
// which comes from the view region (ours uses six slots there, retail five).
// Round 468: Pure cone-test schedules 102345/013245/012354/014523/023145/145023/032154/105432
// 221/223/221/266/261/261/261/264. Depths 0/1, first x/y 2/3, second x/y 4/5; original bounds
// and comparison order restored. Extra frame word and coefficient/result overlap remain.
// Round 471: Uniform weighted-point second/first product and sum producers, seven
// nonempty combinations 222/222/224/221/222/222/224; complete producers with earlier
// blend-scale storage 224. Original expression producers, helper and full body restored.
// @retail 0x501e0
void function_501e0(s_501e0 const *arg_0, bool arg_1, s_501e0 *arg_2)
{
    vector3f local_0;
    vector3f local_1;
    for (long component = 0; component < 3; ++component)
        local_0.n[component] = arg_0[0].field_0.n[component] - g_4b9da0.n[component];
    for (long second_component = 0; second_component < 3; ++second_component)
        local_1.n[second_component] = arg_0[1].field_0.n[second_component] - g_4b9da0.n[second_component];
    vector3f local_2;
    vector3f local_3;
    world_vector_to_view(g_4b9e14.scale, &local_0, &local_2);
    world_vector_to_view(g_4b9e14.scale, &local_1, &local_3);
    real first_depth_limit = local_2.k * local_2.k * 4.f;
    real second_depth_limit = local_3.k * local_3.k * 4.f;
    if (local_2.k < 0.f && local_3.k < 0.f &&
        first_depth_limit > local_2.i * local_2.i &&
        first_depth_limit > local_2.j * local_2.j &&
        second_depth_limit > local_3.i * local_3.i &&
        second_depth_limit > local_3.j * local_3.j)
    {
        real local_4 = 0.f - local_2.k;
        real local_5 = 0.f - local_3.k;
        real local_6;
        if (arg_1)
            local_6 = 1.f / (local_5 + local_4);
        else
        {
            local_4 = 1.f;
            local_5 = 1.f;
            local_6 = 0.5f;
        }
        arg_2->field_0.x = (arg_0[1].field_0.x * local_4 + arg_0[0].field_0.x * local_5) * local_6;
        arg_2->field_0.y = (arg_0[1].field_0.y * local_4 + arg_0[0].field_0.y * local_5) * local_6;
        arg_2->field_0.z = (arg_0[1].field_0.z * local_4 + arg_0[0].field_0.z * local_5) * local_6;
        arg_2->field_c = (arg_0[1].field_c + arg_0[0].field_c) * 0.5f;
        arg_2->field_10 = (arg_0[1].field_10 + arg_0[0].field_10) * 0.5f;
        arg_2->field_14 = arg_0[0].field_14;
        if (arg_1)
        {
            real local_7 = (local_3.i * local_4 - local_2.i * local_5) * 2.f;
            real local_8 = (local_3.j * local_4 - local_2.j * local_5) * 2.f;
            real local_9 = local_8 * local_8 + local_7 * local_7;
            if (local_9 >= 0.f)
                arg_2->field_c = (real)(sqrt(local_9) * arg_2->field_c * local_6);
        }
        else
        {
            vector3f local_10;
            local_10.i = arg_0[1].field_0.x - arg_0[0].field_0.x;
            local_10.j = arg_0[1].field_0.y - arg_0[0].field_0.y;
            local_10.k = arg_0[1].field_0.z - arg_0[0].field_0.z;
            real local_11 = function_30bf0(&local_10);
            real local_12 = dot3f(&g_4b9dac, &local_10);
            real local_13 = 1.f - local_12 * local_12;
            local_13 = local_13 < 0.f ? 0.f : local_13 > 1.f ? 1.f : local_13;
            arg_2->field_c = (real)(sqrt(local_13) * arg_2->field_c * local_11);
        }
    }
    else
    {
        memset(arg_2, 0, sizeof(*arg_2));
    }
}
