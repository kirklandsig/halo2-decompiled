// @flags /O2 /arch:SSE /Gr
#include "projection_storage.h"
#include "unknown_11c920.h"
#include "camera_storage.h"
#include "unknown_0259d0.h"
#include "unknown_030290.h"
#include "normalize3d.h"
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
    vector3f scaled = { arg_1->i, arg_1->j, arg_1->k };
    if (arg_0 != 1.f)
        vector3f_scale(arg_1, arg_0, &scaled);
    arg_2->i = g_4b9e14.right.i * scaled.i + g_4b9e14.up.i * scaled.j + g_4b9e14.forward.i * scaled.k;
    arg_2->j = g_4b9e14.right.j * scaled.i + g_4b9e14.up.j * scaled.j + g_4b9e14.forward.j * scaled.k;
    arg_2->k = g_4b9e14.right.k * scaled.i + g_4b9e14.up.k * scaled.j + g_4b9e14.forward.k * scaled.k;
}
// score 221 (was 221): Frame 0x20 versus retail 0x14. The second displacement and first rotation coefficients still have different homes.
// Earlier 234.
// MiniMax C (11-19): no new form, no lower score (221): the two offset loops written as explicit component assignments
// reach 218 (1115 B) on the reduced image, three points under the keep bar, so the loops stay; no other new form lowers
// the score.
// MiniMax R (11-16): no new form, no lower score (206): re-measured in this round's nine-address set, the body is 1140 bytes against retail's 1136 and the frame is 0x18 against retail's 0x14; retail keeps the first point's three offsets in registers (XMM1, XMM3 and XMM5) and spills the second point's, where this build materialises both offset vectors in the frame, and the orthographic dot still runs i, j, k where retail runs j, k, i.
// MiniMax C (11-16): no new form, no lower score (206): retail keeps the first point's three offsets in registers
// (xmm1, xmm3, xmm5) and spills the second point's (frame 0x14), where ours materialises both offset vectors and
// both views in the frame (0x20); the orthographic dot still runs i, j, k where retail runs j, k, i, and no helper in
// include/ offers that order, so the body stands.
// MiniMax E (11-16): no new form, no lower score (206): the frame is still 0x20 against retail's 0x14 (retail keeps
// the first point's three offsets in registers and the second point's in the frame, this build materialises both
// offset vectors and both views in the frame), the orthographic dot product still runs i, j, k where retail runs
// j, k, i, and the scale is still applied in place where retail copies the raw offsets before the test; the source
// permuter's search on this function was cut at its time limit before it scored any variant.
// Earlier 221 (was 234): see the last entry (one `scale` local for the blend weight and the segment length). Earlier 234 (was 304): see lane 17 r45 (each offset transformed in place). Earlier 304 (was 206): frame 0x20 versus 0x14 (the second offset vector keeps its own home, retail shares it with
// the orthographic segment and keeps right.i in xmm7 for both views), the scale applied in place (retail copies the
// raw offsets before the test) and the coefficients folded into the products.
// MiniMax G (11-13): no new form, no lower score (206): the diff re-read this round puts the first difference in the frame size (ours 0x20, retail's 0x14), and the forms that change the two offset loops' order or the scale step measure at or above the body, so the body stands.
// MiniMax H (11-13): no new form, no lower score (206).
// MiniMax I (11-11): no new form, no lower score (206): retail computes the second point's offsets first, spills all three and keeps the camera coefficients in registers; taking the two offset loops in retail's order scores 208, so the body keeps its own order.
// MiniMax B (11-11): no new form, no lower score (206); the body is unchanged, and the forms listed above are exhausted.
// MiniMax B (11-12): no new form, no lower score (206): the frame is still 0x20 against retail's 0x14 - the second
// offset vector keeps its own home where retail shares it with the orthographic segment and keeps right.i in
// xmm7 for both views - the scale is applied in place where retail copies the raw offsets before the test, and
// the coefficients are still folded into the products.
// Earlier (206, was 206): frame 0x20 versus 0x14 (the second offset vector keeps its own home, retail shares it with
// the orthographic segment and keeps right.i in xmm7 for both views), the scale applied in place (retail copies the
// raw offsets before the test) and the coefficients folded into the products.
// MiniMax S (11-10): no new form, no lower score (206); our frame is still 0x20 against retail's 0x14 and the two view
// coefficients are still folded into the products.
// MiniMax D (11-08): no new form, no lower score (206); our frame is 0x20 against retail's 0x14, the transform helper
// folds the coefficients into the products and keeps the second view's width in a register where retail keeps the
// first view's coefficients in registers and the second view's offsets in the frame, and the sum tree is still
// (right*width + up*height) + forward*depth against retail's forward*depth + up*height + right*width; the six orders
// of the transform sum's terms all give 206 and the six parenthesized trees give 213 or 245, so the tree is the
// compiler's reassociation and not the source's.
// MiniMax I (11-06): no new form, no lower score; the offsets differenced inside the transform helper give 304 and 321 against 206.
// Earlier 221: first transform
// sum/product order and register numbering (retail: forward*depth +
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
// Round 35 (m3 lane 2, fast): both camera offsets through the existing vector3d_from_points3d 221 (1147 bytes),
// with the orthographic segment through it too 224 (1148 bytes); the segment alone through it 221 (1145 bytes);
// both offsets through it with the depth limits written in the condition 233 (1153 bytes); the first offset alone
// through it 222 (1146 bytes). Retail computes each depth limit lazily after the two `k < 0` tests (`mulss` by the
// 4.0 in memory); ours hoists both limits and the 4.0 into registers ahead of the tests.
// Round 111 (lane 7, fast; a one-file LTCG harness with src/unknown_01cf50.cpp and src/unknown_030290.cpp
// reproduces the build, 221 at 1145 bytes): retail scales with the scale as destination (`movaps xmm0,xmm6;
// mulss xmm0,xmm1`, the unscaled copies made before the test) where ours scales in place. A camera-pointer
// helper with `i *= camera->scale` and the component loop 224 (1148 bytes), with an explicit three-row body 306;
// the scaled locals set in an if/else 305 (1078); a caller `real scale = g_4b9e14.scale;` declared first 253
// (1136 bytes), declared at the calls 252; a scaled vector copy through vector3f_scale in place 283 (1160), from
// the input 309; explicit rows with (real) product casts 306 (1081), grouped 301; locals declared k-first
// 258, scaled k-first 245; the per-component ternary scale 256 (1136, three tests).
// Round 131 (m3 lane 6, fast): each view transformed right after its own offset loop 309 (1079 bytes);
// with each depth limit right after its view 295 (1094 bytes); the view vectors declared before the
// offsets 221 (same code); the blend divisor as `local_4 + local_5` 221 (same code); the depth limits
// grouped `k * (k * 4.f)` 226 (1148 bytes).
// Round 477: Native input-vector scale induction long/dword/short/word with cursor 312/312/355/355; indexed rows 309/309/358/358.
// Original scalar snapshots, scale producers, separate offset loops, output cursor and entire body restored; frame and coefficient/input/result homes remain.
// Round 483: coherent radius from output/producer, branch/common publication
// and original/immediate clamp consumption 266/266/257/257/259/258/259/259.
// All regress; initial stores, alias-sensitive reads, original helper/body restored.
// Round 497: Real coefficient-array termination !=/< with forward/up/right owners
// 379/379/379/379/378/378; all three streams without an ordinal 375/375. Original helper and body
// restored; frame and coefficient/input/result homes remain.
// Round 131 (m3 lane 6, fast): each view transformed right after its own offset loop 309 (1079 bytes);
// with each depth limit right after its view 295 (1094 bytes); the view vectors declared before the
// offsets 221 (same code); the blend divisor as `local_4 + local_5` 221 (same code); the depth limits
// grouped `k * (k * 4.f)` 226 (1148 bytes).
// Round 47 (m3 lane 14, fast): retail's extra frame word is not a view temporary: retail keeps all three second
// offsets in [esp+8]/[esp+0xc]/[esp+0x10] from the start (the orthographic segment reuses those same slots) and
// spills nothing in the second view, ours keeps the second offset's i in xmm7 and spills a second-view partial
// sum at [esp+8]. Segment vector declared at function scope after the offsets 221, before them 221, after the
// views 221, at the top of the projection block 221; view vectors declared with the offsets 221 (same code).
// Round 499: First/second/both source-coordinate cursors with long/dword ordinals
// 222/222/223/222/222/223; both source-end !=/< forms 276/582. Original input walks and body
// restored; extra frame word and coefficient/result homes remain.
// Round 500: native ordinal !=3/<=2 with long/dword/short/word domains
// 409/221/401/221/400/399/400/399. Equality bounds change unrolling but regress;
// extra frame word and coefficient/input/result homes remain. Original helper/body restored.
// Round 72 (lane 16, fast; compile-only register-blind distance in brackets, control [178]): explicit offset rows alone
// 278 [156]; rows with a `vector3f scaled` copied field by field and overwritten from the input under the scale test,
// component loop or cursor 278 [122], which gives retail's opening 32 instructions (raw kept, copies before the test,
// scale-first products); three row statements 235 (1113 bytes) [152]; the copy as reals 278 [156]; scaled declared in the
// caller, a camera pointer or an inline getter: no change to the folded coefficients. Calling matched 0x142640 562 (827
// bytes, stays out of line under /Ob1); a static or inline copy of its body 235 (1113). Retail loads the first view's
// coefficients into registers where ours folds them; without the scale branch ours loads them too. Restored.
// Round 502 (reduced): identity-conditional native/memcpy input protection, alias/scale-first
// ownership 287/289/287/289; indexed outputs 290/305/290/305. Complete alias-safe scalar
// helper and original body restored; extra frame dword and coefficient/result homes remain.
// Round 504: twice-used scalar origin capture orders xyz/xzy/yxz/yzx/zxy/zyx
// 281/280/281/281/280/281; shared per-axis origin in long/dword offset loops 226/226.
// Complete original offset loops, helper and body restored; extra frame word remains.
// Round 518: on separate offsets/cursor outputs, in-place first/second/both
// transforms 285/259/275; with the first transform before second offsets
// 291/279/262; both in-place with indexed output 278, early shared scale 306.
// Original helper/body restored; extra frame word and coefficient/result homes remain.
// Round 521: real endpoint-pair array traversal, separate/fused phases, long/short
// endpoint indices and cursor/indexed view rows 389/373/380/376/380/377/376/374.
// Grouped workspace forms do not match; original helper and body restored.
// Round 45 (m3 lane 15; compile-only aligned count against retail, control 189, structure-only 116): both retail views are
// 0x142640's form. A copy of it reading the camera through a pointer parameter or the global 203 (structure 127); 1100
// random forms of that copy (scale local, global or parameter; x/y/z as reals or a vector; branch reading the input or the
// copy; row and term orders; offsets as rows, a helper, loops or a named origin; both view orders) stay at 185 or above.
// The best of them through fast: pointer-parameter copy 235 (1113 bytes), the same reading the input in the branch 235,
// vector copy with rows k, j, i 230 (1113 bytes, aligned 162 against 171), the same with the input read 230, with reals
// 271, rows i, k, j 266. Retail's first view loads the coefficient and multiplies by the scaled value, its second copies
// the scaled value and multiplies from memory; no form gave that split.
// Round 524: real forward-coefficient cursor with complete raw/scaled inputs,
// explicit arms ijk/kji and separate/shared scale 295/297/295/297;
// default-first counterparts 241/242/241/242. The latter recover frame
// 0x14 and 1136 bytes but regress. Original helper, observations and body restored.
// Round 526: Actual coefficient-row vectors consumed by existing dot3f, original/native
// input ownership, cursor/indexed output and both product orientations:
// 254/254/250/250/254/254/250/250 at 1136 bytes. Original helper/body restored;
// extra frame word and coefficient/input/result homes remain.
// Round 545: Existing shared axis rotation with complete scalar/native/shared-scale
// input producers, direct/in-place output: 309/309/283/283/414/456/560/560. All nonmatching
// association/storage forms restored; original helper/body and extra frame word remain.
// Round 63 (m3 lane 10, fast): retail computes each depth limit only after the two `k < 0` tests and the second only
// after both tests on the first, and homes the blend scale at [esp] where it is computed (ours keeps it in xmm2 and
// spills it at [esp+8] for the x87 tail). The cone test as a forceinline predicate with a named limit, called per view
// 290 (1091 bytes); the second offset loop before the first 223; the blend scale initialised to 0.5f at its declaration
// with only the perspective arm assigning it 260 (1151 bytes); the blended position as a component loop 222 (1146
// bytes); the `k < 0` tests in an outer if with the named limits inside it, an early return and the clear at the end
// 224. All restored.
// Round 548: Complete post-view double weight-pair/reciprocal/both-radicand domains,
// subsets 1/2/4/3/5/6/7, then all with normalized scalar results: 267/267/293/270/279/271/282/327.
// NOT kept: precision/frame shape misses; original float producers, transform helper and full body restored.
// Round 65 (m3 lane 1, fast, control 221): the second offset loop before the first 223; named first/second
// point pointers for arg_0[0]/arg_0[1] 221 (1147 bytes); the view helper scaling into a copy and pointing
// its input at it 294 (1174 bytes); both offsets in one fused loop 226 (1146 bytes); the depth limits
// written into the condition with no limit locals 231 (1151 bytes). Original helper and body restored.
// Round 550: Plain weighted-output coordinate walks on separate-offset/cursor-transform
// graph: long/dword index 222/222; long/dword output cursor 221/221; first/second/both
// input cursors 221/223/221; all cursors 221. NOT kept: no gain, 1146/1147 bytes and
// extra frame word remain; original three publications, helper and complete body restored.
// B394: Existing complete weighted-point setter with segment-difference helper, immediate clamp;
// consumer and entry-owned reciprocal subsets: 272/272/272/272/272/272/272/272.
// NOT kept: no exact match; original complete executable source restored.
// Round 160 (fast): the view helper's input by value 348 (1215 bytes), as three reals 275 (1153); a copied input for the
// first/second view 334/268 (1205/1151); the first view transformed in place then copied 279 (1104). All restored.
// Round 41 (m3 lane 17, fast; plain confirmed): the view helper's locals renamed 221 (same code); a shared scale
// local with the input re-read in the scale arm 252 (1136 bytes); a shared scale with a scaled vector 255 (frame
// 0x20); the clamped value in a fresh local (retail clamps into a fresh value, written per arm) 215; that with
// both offsets through vector3d_from_points3d 215 (1147 bytes), with i/k/j helper reads 214, with the shared scale
// 252; that with the scaled values in a `vector3f scaled` initialised from the input and scaled by the existing
// vector3f_scale under the test 206 (kept; 1140 bytes, retail's (forward*depth + up*height) + right*width sum
// tree in both views; alone, without the fresh clamp, 211); the same as a struct copy 330 (1198), set through
// vector3f_assign 252 (1146). On the kept body: offsets and views in their own block 206 (same code); shared scale
// 255; indexed output rows 213; three row statements 304 (1084); depth limits written in the condition 214.
// M369: Complete basis-axis values before/after the scaler on the aggregate-scaled graph:
// forward/up/right 278/280/268 and 281/283/271; all axes 446/444.
// NOT kept: all regress and grow; original helper/body restored; frame and coefficient homes remain.
// AC516: Aggregate-scaled/native-row-sum graph, first/second/both in-place views, early first view,
// early first/both overlap, second-first overlap and native segment reuse: 281/258/271/305/287/259/287/271.
// NOT kept: overlap/lifetime diagnostics unmatched; original two-call helper and complete body restored.
// AC520: Ordered lazy depth/first-cone/second-cone admission, nested-success/early-failure
// stage masks 0-7: 279/279/279/279/279/279/279/279. NOT kept: all regress and lengthen;
// original aggregate transform, gate, input observations and complete body restored.
// F367: On the retained aggregate-scaled helper, complete caller vector/orthographic/weight/span/cone names, helper workspace/formal names and all caller names: 206 each. NOT kept: eight byte-neutral home trials; original complete helper and body restored.
// B423: Existing aggregate helper scale-admission result, required/unit in bool/byte/short/long,
// 213/213/213/213/213/213/213/213 at 1143/1136 bytes. NOT kept: no exact match;
// original aggregate defaults, shared scale call, helper and complete body restored.
// DH340: Complete raw-vector capture aggregate/native/setter/CRT with native scaled defaults
// 345/391/342/391; shared-setter scaled defaults 252/336/252/336. NOT kept: all regress
// and lengthen the body; original aggregate-scaled helper and complete body restored.
// P331: Aggregate-scaled graph with branch-owned/unit-default/conditional/const-conditional
// weights: 206/248/247/247; immediate clamp in each: 206/248/247/247. NOT kept: native eager
// producer shape/equality absent; original helper and body restored.
// Y580: Current aggregate-scaled/native-sum graph, actual endpoint traversal with long/dword
// ordinals: separate/fused/iteration-owned/in-place displacement workspaces
// 384/373/375/378/441/441/382/382. NOT kept: storage graphs miss equality and native extent;
// original helper and complete body restored; frame 0x20 versus 0x14 remains.
// Y582: On the aggregate-scaled helper, live forward/up/right coefficient streams, seven
// nonempty subsets and all streams with indexed output: 252/253/253/252/252/252/254/252.
// NOT kept: all regress; original helper/body restored; frame and coefficient/input homes remain.
// M371: Scaler-return parameter/mutable-source/conditional-source/complete-value producers,
// cursor rows 295/292/297/379; indexed rows 298/294/299/377. NOT kept: all regress
// and lengthen; original aggregate defaults, helper and complete body restored.
// M373: Complete scale/copy branch-owned scaled workspace, native/setter/memcpy/memmove,
// nonunit-first 327/288/327/374; unit-first 331/288/331/370. NOT kept: all regress;
// original aggregate-default helper and full body restored; frame/coefficient homes remain.
// Round 52 (m3 lane 2, fast; aligned difflib count in brackets, base 206 [241]): LEAD, not kept (scores rise). Retail's scale
// and rows are separate steps with the rows in the caller: scaled vectors `vector3f s1, s2` in the caller filled by a small
// forceinline scale step (default copies, then the scale arm, through vector3f_scale or by hand: same code), rows in the caller
// naming the fields x first, depth limits written in the condition, separate second offset and segment locals. That gives retail's
// frame 0x14, 1136 bytes, load-first first-view products, copy-first z/y and load-first x second-view products and the retail
// tail from the blend down: 218 [163] with offsets 0i 0j 0k 1k 1i 1j, 217 [172/173] with 0k 0i 0j 1k 1i 1j / 0k 0j 0i 1k 1j 1i
// (all 72 block orders 217-252). Scalar x/y/z locals inside any inline helper rank above the camera global (both views copy-first,
// 230-306); written directly in the caller both views are load-first 270 (1146 bytes). Same form with the second offset reused for
// the segment 218-253 (frame 0x20); rows through a pointer helper 231; by-value rows helper 293-299; rows as accumulated statements
// 257-264; (real) product casts same code; offsets through vector3d_from_points3d 294-316; offset loops 293-318; arm reading the
// scaled copy same code. What is left: with rows x first the first-stored second-offset component keeps a register through the first
// view (xmm0), so x/y/z take xmm2/4/5 and the scale xmm7, where retail has xmm0/2/4, raw xmm1/3/5, scale xmm6, the camera's right.i in
// xmm7 and all three second-offset components in the frame. Rows named z first give those registers (1i 1j 1k 0i 0j 0k: 261 [129], 1146
// bytes, frame 0x20, the second offset's i in xmm7) but the sum tree y,x,z; retail's tree z,y,x needs x named first.
// AC537: Actual projected-radius square-root admission/rejection truth in
// bool/byte/short/long: 207/207/207/207/207/207/207/207. NOT kept: no match; original transform helper,
// ordered test, alias-sensitive outputs, memset and complete body restored.
// DL263: Native origin value with existing shared scaled-displacement producer on retained
// aggregate-scaled transform, first/second/segment subsets then native segment point producer:
// 206/209/209/213/206/206/213/213. NOT kept: unmatched complete-value contracts; original
// observations, helper and complete body restored. Frame 0x20 versus 0x14 and
// coefficient/input/result homes remain.
// P333: Standard binary differences with one captured origin, seven producer subsets
// then complete raw copy: 324/328/260/304/300/297/277/386. NOT kept: no match or gain; original helper/body restored.
// Excluded standard-transform global-array arguments; uncredited and restored.
// Round 56 (m3 lane 15, fast; one-link harness exact; masked = aligned diff with stack offsets and registers masked, 192 here): 0x142640's shape with staged rows 230 (1113 B,
// masked 161); plain rows and the depth limits in the condition 260 (1113 B, masked 155); `vector3f scaled` copied by field and overwritten from the input under the test, loop rows,
// offsets as three-statement rows 274 (1156 B, masked 121: retail's opening 32 instructions); scaled reals, loop rows, limits in the condition 292 (1176 B, masked 149); scaled by field,
// plain rows, limits in the condition 260 (1113 B, masked 155). 214 harness forms of copy x scale arm x rows x depth x offsets: none below 206.
// T59: On the retained aggregate-scaled graph, complete coefficient-row dot with reversed
// input before/after scale, aggregate/shared-setter rows and both dot input roles: 254 each.
// NOT kept: all regress at 1136 bytes; original helper and whole body restored.
// M3 lane 11 r72 (fast, control 206, masked 170): the matched 0x30290 inlines src/unknown_030290.cpp's transform_vector in place after a
// vector3d_from_points3d offset, and its retail code is this function's first view exactly (unscaled copies before the scale test, then
// `movaps xmm, scale; mulss xmm, x`, (forward*z + up*y) + right*x). A local copy of that helper out of place 306 (1081, masked 156); in place after
// vector3d_from_points3d offsets 231 (1113, masked 155); the file's own helper in place after vector3d_from_points3d offsets 273 (1155, masked 110:
// second offset spilled to retail's +8..+0x10, first in registers, copy-then-scale as retail; left: 1.0 held in XMM7, first-view coefficients as
// memory operands where retail loads them and keeps right.i in XMM7, both depth limits hoisted ahead of the k tests, frame 0x20); on it the depth
// limits in the condition 289 (1170, masked 150); its products written variable-first 273 (same code); the helper in place on the original offset
// loops 276 (1111, masked 109), so the in-place transform is what gives the copy-then-scale.
// Y603: Captured-vector existing in-place vector/particle scale producers with
// uninitialized weight/segment lifetime subsets: 206/206/206/206/206/206/206/206. NOT kept:
// all tie; original helper/body restored, frame 0x20 and coefficient/raw-input homes remain.
// Y605: Caller-owned complete scaled vectors with aggregate/native/shared-setter/CRT defaults,
// rotation-only existing helper with input pointer then reference: 272/330/274/330/272/330/274/330.
// NOT kept: all regress and lengthen; original aggregate-scaled helper and full body restored.
// N395: Aggregate-scaled complete view returns with pointer/reference/value input,
// cursor/indexed output 278/281/278/281/401/404; actual returned-output consumers
// 276/273. NOT kept: no equality/native frame recovery; original helper and full body restored.
// MiniMax A (11-05): no new form, no lower score; nothing I tried moved the frame off 0x20 or the sum tree onto
// retail's forward*depth + up*height + right*width order.
// M3 lane 15 r60 (fast with 0x30290, which stays matched): the existing transform_vector of src/unknown_030290.cpp called across
// files (declared here; LTCG inlines it): out of place 306 (1081 bytes), in place on the offset loops 231 (1113); in place
// through an `s_camera const *camera = &g_4b9e14` local 266 (1146: retail's opening, the second offset at +8..+0x10 and the first
// in registers with copy-then-scale, but the 1.0 constant takes XMM7 and the coefficients fold into the products as memory
// operands, where 0x30290's camera in a register keeps them loaded, coefficient first); that with the camera local declared at
// the top 266 (same code); out of place through it 253 (1141).
// N398: Current aggregate helper static-inline/static/external-inline/external-forceinline, void then output-pointer result 206/206/206/206/206/206/206/206.
// NOT kept: no byte equality or eligible native-shape gain; original complete body restored.
// T72: Helper-local scaled defaults as native initialization/assignment, library copy,
// existing setter, indexed for/while, component cursors and array aggregate:
// 330/330/330/252/255/255/333/206. NOT kept: no native frame/coefficient recovery;
// original aggregate, input-before-output helper and complete body restored.
// Round 162 (m3 lane 6, fast with 0x30290, which stays matched; masked = aligned count with registers and stack offsets masked):
// a one-argument in-place helper on the global with an aggregate copy of the input (`vector3f copy = *v;`), `scale * v->i`
// under the scale test and coefficient-first rows, on the offset loops or after vector3d_from_points3d offsets 318/318 (1188 bytes,
// frame 0x2c, masked 125); the same through a camera pointer with separate input and output arguments, called in place, after
// either offsets 318/318; the scale arm through vector3f_scale 318. All five are the same code: the aggregate copy is made
// through integer moves into its own frame slots, so the copy has to be three reals.
// Round 84 (m3 lane 10, fast), at 206: the second offset vector reused for the orthographic segment on this body
// 261 (1136 bytes, frame 0x2c); the blend weights defined second-first (local_5 then local_4, declaration and else
// arm) 247 (1136 bytes, the rows still sum the first point's term first); the else arm's weights as
// `local_4 = local_5 = 1.f` 206 (same code); the orthographic dot as dot3f(&local_10, &g_4b9dac) 206 (same code); one
// counter for both offset loops 207 (1141 bytes).
// M3 lane 10 r90 (fast with 0x30290, MATCH; the fast image leaves six camera globals unresolved, so fast scores are a screen only): no
// gain. The blend weights as unary negations `-local_2.k` / `-local_3.k` 206 and the width differences doubled constant-first 206 (both same
// code). No other untried natural form found: the remaining levers (lazy depth limits, the second offset kept in the frame, the blend
// scale's home) are all listed above.
// Round 86 (m3 lane 1, fast; aligned/masked counts in brackets, base 206 [167/103]): the blend products written
// weight first 206 (same code); a shared `real scale` local with the depth limits written in the condition 256
// (1136 B [170/81]: the second offset spilled whole, both scale tests early). LEAD: the existing transform_vector of
// src/unknown_030290.cpp called in place on each offset (`transform_vector(&g_4b9e14, &local_0, &local_0)`), with
// the depth limits written in the condition: 292 at 1089 B [78/46], retail's 0x14 frame. Left there: the first view
// scales in place (retail copies the raw offsets first and loads the coefficients), the second view's products, the
// sqrt argument and the blend scale/length in each other's slot ([esp] and [esp+4]) at all three tails. On that form:
// the second offset loop first 292 (same code); offsets through vector3d_from_points3d 297 [164/55]; blend terms
// swapped 292 (same code); through an `s_camera const *camera = &g_4b9e14` local 298 at 1083 B [155/55].
// Round 179 (m3 lane 4, fast with 0x30290, 0x142640 and 0x30bf0, all matched each time): the helper's scaled copy
// assigned field by field 206 (same code: the copy still coalesces with the raw offsets, scaled in place); the second
// offset through the existing vector3d_from_points3d 207 (1141 bytes); one shared scale local for both views 255 at
// retail's 1136 bytes (frame still 0x20), with that offset helper too 255; the copy scaled in place through
// vector3f_scale 206 (same code). Retail keeps raw and scaled apart (copies before the test, `scale * raw` into the
// copies) and loads the first view's coefficients, holding right.i in xmm7 for the second view.
// Round 78 (m3 lane 14, fast): the orthographic length and clamped value renamed so their names sort the other way
// (two spellings) 206 each (same code: names do not move these two slots, retail has the length at [esp], ours the
// clamped value); the copied word field stored before the two halved fields 206; the halves as `0.5f * (a + b)` 206;
// the blend divisor declared at the function's top 206 (all the same code). Retail loads the scale once into XMM6
// and tests it for both views where ours reloads it; the natural forms aimed at that are listed above (a caller scale
// local 253, the helper reading the global 318, rows indexed into the output 226), so none was left to try.
// Round 185 (m3 lane 4, fast with 0x30bf0; fast base 206): the helper's rows through the existing
// vector3f_rotate_into_axes on the camera's right/up/forward axes 309 (1078 bytes), through matrix_transform.h's
// function_2477b1 304 (1084: both rotate in straight-line rows where retail keeps the loop's code); the orthographic
// segment declared with the offsets 206 (same code); the depth limits as `4.f * (k * k)` 213 (1143, register-masked
// aligned 107 -> 101), as `4.f * k * k` 206 (same code).
// m3 lane 5 r169 (fast with 0x30bf0, base 206 [aligned 168, masked 103]): the transforms as calls to matched 0x142640
// through `(transform4x3f const *)&g_4b9e14` with a `__forceinline` declaration of it here 562 (827 B: the declaration
// does not inline it; re-measure of round 72's call); `inline` on 0x142640's definition (diagnostic) inlines it, 306 at
// 1081 B, and loses matched 0x16c2f0; the helper's rows through include/matrix_transform.h's function_2477b1 on the
// camera as a transform4x3f 304 (1084 B); the scale test as `1.f != arg_0` 256 (1148 B); one shared variable for the two
// square-root arguments 209; the view outputs declared with the offsets 206 (same code). Retail's second view keeps its
// scaled copy apart from the raw offsets ([esp+4], XMM0, [esp]), and those slots carry the blend scale and the root
// arguments later, so the frame and the [esp]/[esp+4] order follow the uncoalesced copy.
// m3 lane 7 r150 (one-link LTCG harness, which reproduces fast's 206 and round 52's 218/217 exactly; brackets are aligned and
// register-masked aligned counts, base [241/185]): the helper's cursor stores keep the second view from reusing the first view's
// camera loads; with three row statements the scale load is shared, as in retail (one scale register, right.i held in xmm7 for
// both views). Fast, on round 52's lead (rows in the caller, a two-call scale step): the scale step written in the caller as component
// copies plus vector3f_scale, or by hand on the input or on the copy, 253 [183/75] (one code, 1136 bytes, frame 0x14), the same with
// retail's offset order 1i 1j 1k 0k 0i 0j 234 [190/80]; the second view transformed in place 253 (same code), the first in place 299
// (1178 bytes, frame 0x2c); the depth limits named again 250 (1136 bytes); rows z first with the second point's offsets first 261
// (1146 bytes) [132/78]; both scale steps ahead of both views, offset loops 217 (1136 bytes, frame 0x20). None below 206.
// MiniMax A (11-12): no new form, no lower score (206): our frame is 0x20 against retail's 0x14 - retail keeps the first
// view's depth and the second view's height in registers across the two inlined transforms, where ours spills them, so the
// coefficient and result registers and the tail still differ.
// m3 lane 10 r99 (fast with 0x30290, 0x30bf0 and 0x142640 matched; register-masked aligned count 185 for this body): the
// helper's scaled copy as an aggregate copy of the input (`vector3f scaled = *arg_1;`) 330 (1198 B); same code (206): the
// offsets renamed start/end so their names sort the other way, the view outputs renamed the same way, the blend scale and
// orthographic length renamed, the two blend weights renamed.
// M3 lane 13 r89 (fast with 0x30290, 0x30bf0, 0x142640; control 206): the clamp as `l13 > 1 ? 1 : l13 < 0 ? 0 : l13` 213; the
// helper's scaled copy made in an else arm (`else scaled = *arg_1;`) 327 (1198 bytes); the widths undoubled with `local_9 =
// (l8 * l8 + l7 * l7) * 4.f` 249 (at retail's 1136 bytes); the blend weights computed only in the `arg_1` arm 206 (the code
// changes); the clamp in place on local_13 with two ifs 211. Same code: the else arm's 0.5 first, the width squares summed
// l7 first, the halved fields first point first, the segment as an aggregate initializer, the depth limits `k * 4.f * k`.
// M3 lane 9 r120 (fast): `local_4 = local_5 = 1.f;` and `local_5 = local_4 = 1.f;` 206/206; the scaled copy as
// `vector3f scaled = *arg_1;` 330 (1198 B); `1.f != arg_0` 256 (1148 B); the depth signs as `0.f > k` 206.
// P478 m3: Per-axis reused origin/subtraction kernel; complete blend/average/depth tails,
// projected sum, standard clamp, uncaptured-origin segment transform and reciprocal:
// 303/400/324/308/275/230/260/343. NOT kept: all regress; original helper/body restored.
// Direct-global scalar kernel 295 excluded because its operands bind references; not a credited trial.
// P480 m3: Raw offsets with common/first/second lexical ends, separate endpoint scopes in
// either order, then common-region view/raw/both declaration pairs reversed: 206/206/206/305/320/206/206/206.
// NOT kept: native extent/homes unrecovered; original helper and complete body restored.
// M3 lane 11 r100 (fast with 0x30290 and 0x30bf0, both MATCH; masked = register-masked aligned count, 170 at 206): retail
// keeps the raw offsets apart from their scaled copies, keeps right.i in xmm7 for both views and loads the scale once
// for both tests. Round 86's lead re-measured: the existing transform_vector in place on each offset with the depth
// limits in the condition 292 (1089 B, frame 0x14, masked 63); in place for the first view only 306 (1078 B, masked 145);
// the lead with the second point's offsets first 292 (masked 60); this file's helper rewritten in place with scalar
// copies scaled from the input and forward-first rows 295 (1086 B, masked 79), with the second offsets first 295 (masked
// 76). Body unchanged.
// M3 lane 5 r184 (fast with 0x30bf0/0x30290/0x142640, all matched; control 206, register-masked aligned 176): the helper
// reading the camera's scale itself (no scale parameter) 209 (1143 B); the copy scaled in place (`vector3f_scale(&scaled,
// arg_0, &scaled)`) 206 (masked 176, no change); one offset loop with the second point's statement first 211 (1141 B); the
// first view's two depth tests before the second view's sign test 257 (1143 B); the second offset loop's counter named
// `component` like the first 206. Body unchanged.
// M3 lane 12 r124 (fast with 0x30290 and 0x30bf0, both MATCH; base 206): retail loads the first view's coefficients into
// registers (coefficient first, right.i kept in xmm7 for the second view) and keeps the scale in xmm6 for both tests, so
// nothing it stores between the views can alias the camera; ours folds the coefficients as memory operands and reloads the
// scale after the cursor stores. On the helper: a helper-local result filled by index then copied out 281 (1156 B); the
// camera passed as a pointer with its scale read inside 209 (1143 B, same structure); the scaled copy assigned in an
// if/else 327 (1198 B); a source pointer selected by the scale step 292 (1170 B); the second view in place (first out of
// place) 258 (1136 B). Re-measured: the existing transform_vector in place with the limits in the condition 260 (1113 B).
// M3 lane 12 r126 (fast with 0x30bf0/0x30290, matched; control 206, register-masked aligned 185): the view helper
// returning the view vector by value (`vector3f local_2 = world_vector_to_view(scale, &local_0);`) 278 (1153 bytes)
// [202]. Re-measured: the existing transform_vector in place on each offset with the later reads through `vector3f &`
// aliases 309 (1080 bytes) [117]; the depth limits written in the condition 214 [157]. Body unchanged.
// M3 lane 13 r96 (fast with 0x30290, 0x30bf0, 0x142640, all MATCH; control 206, LCS-aligned 241, register/stack-masked 176):
// both offsets through the existing vector3d_from_points3d 207 (1142 B); the segment difference through it 206 (same code).
// The view transform as a macro writing the output vector directly (no output pointer) gives retail's opening: the raw
// offsets kept, the copies made before the scale test, `scale * raw` products, the scale loaded once and the second
// offsets spilled first: with the component loop 270 (1146 B, masked 146), with three row statements 262 (1136 B, frame
// 0x14 as retail, masked 98), that with the offset loops in retail's order 262 (masked 100), with the depth limits
// written in the condition 256 (1136 B, masked 74). Left on that last form (build/r96/lead-501e0.patch in lane 13's
// worktree): the second view keeps its scaled copy in registers where retail spills i and k to [esp+4]/[esp] and
// multiplies from memory, the opening loads' order, and the blend scale and root argument in each other's slot
// ([esp+4]/[esp] for retail's [esp]/[esp+4]; renaming the blend scale changes nothing).
// M3 lane 2 r88 (fast with 0x30290 and 0x30bf0, both MATCH; control 206): the clamped orthographic value declared at the top of
// the orthographic arm 206 (same code: its slot stays [esp], retail's [esp+4]); the orthographic dot written out with retail's
// load order (segment i first, global j and k first) 206 (same code: the operands canonicalise). On lane 13 r96's macro lead
// (re-measured 256 at 1136 B): its rows in transform_vector's right/up/forward term order 256 (same code); x/y/z reals scaled in
// place inside the macro, as transform_vector does, 295 (1174 B); both offsets through vector3d_from_points3d 256 (other code).
// P537: Independent perspective-root/orthographic-root/normalizer double API-result
// producers, seven subsets and immutable all: 207/206/238/207/239/238/239/239.
// NOT kept: no exact result or frame recovery; original math APIs, helper and full body restored.
// MiniMax E (11-15): no new form, no lower score (206): the permuter over the body found nothing below 206.
// P567: Standard transform pair/final/both sum boundaries, weighted-position sums,
// their combinations and complete metadata sums: 277/277/328/432/454/454/501/485.
// NOT kept: regressed native frame/input/coefficient residence; original complete helper/body restored.
// b7L (11-17) walk against retail, fast build on r830: branch senses (0x5025e and 0x50324 scale != 1;
// 0x503c3..0x50431 depth and cone tests; 0x5044a perspective split; 0x50556 radicand test; 0x505fa and 0x5061e
// clamp), NaN sides, constants (1, 0.5, 2, 4, 0), field offsets and the dword copy at 0x50507, weight pairing, blend
// and dot add order, fstp rounding and the eax set at 0x50598 all agree; the weights negated at 0x5043f and 0x50446
// before the je and overwritten at 0x50461 and 0x50471 are retail's shape. One difference, retail's evaluation order
// and not a behaviour fix: retail adds (forward*z + up*y) + right*x in every row of both views (first view i 0x502a4
// and 0x502af, j 0x502c3 and 0x502d3, k 0x50301 and 0x5030f; second view i 0x50370 and 0x503c6, j 0x50387 and
// 0x50399, k 0x503aa and 0x503bc); the loop form compiled i and j so but the depth row as (up*y + right*x) +
// forward*z, so k rounded differently in both views. Writing the rows right, up, forward as matched 0x30290's
// transform_vector does gives retail's tree in all six sums (checked on the symbolic trees of the fast build); loop
// form 206, rows indexed through the output 213 (same k tree), the call of transform_vector itself 306 (gives
// (right*x + up*y) + forward*z). Rows 304 (1084 B), 0x4f7e0 stays 325, 0x30290 and 0x30bf0 stay MATCH. The clamp at
// 0x505fa..0x50626 is PIN_126660's body; its header needs a new include, so it stays written out. Not kept: the
// segment and both offsets through vector3d_from_points3d, which this file doesn't use yet, 301 (no behaviour change).
// Complete view aggregate 278; staged coordinate stores 304; staged coordinate aggregate 278;
// scoped raw displacements with aggregate/staged stores/staged aggregate 278/304/278;
// first/second displacement scoped to its transform with aggregate 279/270.
// NOT kept: the lower scores have frame 0x2c versus 0x14, even at retail length.
// M3 lane 17 r45 (fast with 0x30290, 0x30bf0 and 0x142640, all MATCH, 0x4f7e0 unchanged at 325; the kept form confirmed plain;
// "sums" = the six view sums compared symbolically with retail's (forward*z + up*y) + right*x, build/r45/symtree.py). Kept: each
// offset transformed in place by the unchanged helper, the views living in the offset vectors as in matched 0x30290, 304 -> 234
// (1110 bytes, sums as retail). On it: the helper reading the camera's scale itself 231; scalar x, y, z copies in the helper 231,
// with `scale * v->i` in the arm 231, with `*=` 231; the helper's rows written forward, up, right 231 but the sums become
// (right*x + up*y) + forward*z; the depth limits in the condition 260; both offsets through vector3d_from_points3d 231 (with the
// scale-reading helper; the same code as the loops); the second offset first 233 with other sums; each view right after its
// offset 307; the orthographic segment in the second or the first vector 236/240 (frame still 0x20). Other in-place helpers: the
// neighbour file's transform_vector declared here 260 with sums (right*x + up*y) + forward*z (an LTCG inline keeps the text
// order of the sum, a same-file inline of direct globals reverses it); a camera-pointer parameter given &g_4b9e14 260, other
// sums; a camera pointer that defaults to the global when NULL (0x30830's idiom) 286 at 1167 bytes with retail's 0x14 frame and
// one coefficient held across both views, but other sums whichever way the rows are written; axis-pointer locals 280 (frame
// 0x14, other sums). Diagnostic (not kept, the caller changed and restored): the camera as a fourth parameter of this function,
// passed &g_4b9e14 by 0x4f7e0, 307: every first-view coefficient is then loaded before its product, as in retail (through EDX
// where retail has absolute addresses), with retail's frame. Left: retail loads each first-view coefficient into a register
// before multiplying where ours copies the variable and multiplies from memory, keeps right.i in XMM7 for the second view and
// spills the second view's x and z.
// M3 lane 7 r175 (fast with 0x30290/0x30bf0, both MATCH; plain confirmed, 0x4f7e0 stays 325). Kept: the orthographic arm
// stores the normalized segment's length in the blend scale's local (renamed `scale`: in both arms the factor applied to
// the width), 234 -> 221 (1112 B): retail keeps both in [esp] and the root argument in [esp+4], and the orthographic tail
// now matches. On the 234 body: the camera passed as a pointer with forward-first rows 231; the blend terms written first
// point first 234 (same code). On the kept body: the blend factors written local first 221 (same code); the depth limits
// written in the condition 259; the camera-pointer helper with forward-first rows 218 (1115 B, -3); the existing
// transform_vector of unknown_030290.cpp in place 218 (1115 B, -3, sums (right*x + up*y) + forward*z); `scale` declared at
// function scope 221 (same code); scalar copies scaled in an if (transform_vector's shape) 221 (same code); the scale
// step as three per-component ternaries 289 (1162 B: three tests, but retail's opening with the second offsets spilled
// first, and forward.i and up.j then carried across both views in registers); the scale step as a pointer select
// (`input = scale != 1 ? vector3f_scale(...) : v`) 312 (1183 B). Diagnostic (reverted): with no scale step at all the
// first view's coefficients load into registers and up.i is carried into the second view, so the statement-level
// scale branch is what keeps our coefficient loads from crossing into the second view; retail crosses it with one
// test per view. The camera read from a separate plain global outside its storage aggregate (diagnostic) gives the same code.
// Shared right-axis coefficient after offsets 266, Shared right-axis coefficient after offsets with one scale
// read 292, Shared right-axis coefficient before offsets 266, Shared right-axis coefficient before offsets with
// one scale read 292, Horizontal camera row after offsets 258, Horizontal camera row after offsets with one scale
// read 276, Horizontal camera row before offsets 258, Horizontal camera row before offsets with one scale read
// 276. NOT kept: Capturing the horizontal coefficient row reaches 1136 bytes in two forms at 258 but raises the
// score. Sharing the scale shortens both forms and the original offset and transform layout is restored.
// M3 lane 3 r263 (fast; base 221): separate view outputs (raw offsets kept) with the depth limits written in the condition 304
// (1078 B); the limits in the condition on the in-place form 259 (1114 B); `i*i < limit` compares 221, `4.f * k * k` limits
// 221, `0.f > k` sign tests 221 (all the same code). Diagnostic: 1..7 dead initialised locals 222/222/223/223/221/221/221.
// Point-to-view indexed/aggregate/shared-helper/copied-scale displacement, first endpoint first 335/333/315/331;
// second endpoint first 328/327/308/330. All shorten away from retail; original helper and body restored.
// M3 lane 12 r150 (fast with 0x30290 and 0x30bf0, both MATCH each time, 0x4f7e0 unchanged at 325; control 221): a helper taking
// the camera as a pointer (given &g_4b9e14) with scalar x, y, z copies scaled under the test, as the neighbour file's
// transform_vector: in place, rows forward-first or right-first, first offsets first 218/218 (same code: the pointer folds
// and the coefficients stay memory operands), second offsets first 217/217; with separate view outputs 304/304 and
// 303/303 (1083 B). The neighbour file's transform_vector itself, declared here and inlined at link time, in place 218
// (1115 B, aligned 272 against the control's 249). One helper for both vectors (each scale step followed by its rows) in
// place 218, with separate outputs 299 (1089 B); that helper with both scale steps ahead of both sets of rows 263 (1142 B):
// retail's frame 0x14 and its first seventeen instructions up to register names (the second offsets stored at +8, +0xc,
// +0x10, copy then scale), but 1.0 is held in XMM7 and the rows after it differ throughout (aligned 269); the three
// axes copied into locals and passed by pointer to the helper 358 (1213 B). Reading retail again: every product of the
// first view loads its coefficient and multiplies by the value's register, even at a value's last use, while the second
// view copies y or loads z and multiplies by the coefficient in memory and only its x products load the coefficient, so
// the two views do not come from one expansion with direct global operands; right.i alone is carried from the first
// view into the second (XMM7), across the second scale test.
// shared scale after offsets, both in place 306, second offset copied 290, first offset copied 287, both offsets
// copied 266, entry scale, both in place 306, second offset copied 290, first offset copied 286, both offsets
// copied 266. NOT kept: Every shared scale gate regresses. The lowest is 266 at 1152 bytes with both offsets
// copied, and the opening frame mismatch remains.
// Round 640: shared vector setter 221. Right/up/forward vector products through
// scale and scaled-add, three scaled axes, two scaled axes, or a separate forward
// product 281/256/267/271. Endpoint traversal in two passes, fused, or axis-first
// 289/389/310. The second raw displacement and shared coefficient still have
// different homes. Original view helper and body restored.
// In-place transform pointer, direct scaled default 221.
// In-place transform reference, direct scaled default 221.
// In-place transform pointer, scaled default used by the scale branch 221.
// In-place transform reference, scaled default used by the scale branch 221.
// In-place transform pointer, separate raw input retained for the scale branch 355.
// In-place transform reference, separate raw input retained for the scale branch 355.
// In-place transform pointer, scale result chosen before the raw default store 310.
// In-place transform reference, scale result chosen before the raw default store 310.
// NOT kept: the pointer and reference forms tie with the default copy and in-place scale.
// A separate raw snapshot or branch-owned copy raises the score and grows past retail length.
// The original 0x20 frame, raw displacement homes and coefficient schedule remain.
// First depth limit before the second transform 268, with both first lateral squares 274,
// first cone predicate 265, first depth sign 250, independent first lateral predicates 262,
// complete first visibility 274, paired cone predicates 264, and paired visibility 276.
// NOT kept: all regress and preserve the extra frame space. The depth-sign form reaches retail length.
// M3 lane 5 r211 (fast with 0x30290, 0x30bf0, 0x142640, all MATCH; control 221). Slot count: ours uses +0x1c (the first
// view's k spilled to the first offset's home) and keeps the second view in the second offset's home; retail's first
// view never gets a home (XMM3/5/1 through the second view) and its 0x14 frame is the second offset at +8..+0x10 plus two
// scratch slots. The orthographic segment written into the second offset vector 223; that with the second offset loop
// first 222 (frame still 0x20); a scale-step helper with the rows in the caller (separate view vectors, limits in the
// condition) 314 (1192 B); round 96's macro transform (out of place, limits in the condition) on this tail 252 (1136 B,
// frame 0x14, the first view loads its coefficients as retail but the scale takes XMM7 where retail keeps right.i, the
// scale in XMM6); that macro applied in place 301 (1180 B).
// Paired indexed view rows with separate/common scale tests 329/297, with shared
// coefficient values 465/448, direct paired rows 367/335, separate completed
// view results 477/454. NOT kept: all enlarge the frame and score higher.
// The second displacement spill and the live right-axis coefficient still differ.
// M3 lane 8 r201 (fast with 0x30290, 0x30bf0, 0x142640, all MATCH, 0x4f7e0 unchanged at 325; control 221 at 1112 B,
// difflib-aligned 249, register-masked 154). Lane 13 r96's macro lead rebuilt on this body (separate view vectors, the
// rows written straight into them, the depth limits in the condition) 252 at retail's 1136 B and 0x14 frame, aligned 174,
// masked 62: the positional score rises while two thirds of the masked differences go. Reading of what it leaves: retail
// loads the scale after the last origin subtraction into XMM6 (the origin z's register) and gives XMM7 to right.i, which
// it loads once and carries across the second scale test, so the second view's scaled x and z live in [esp+4] and [esp]
// and its products take those from memory; ours gives XMM7 to the scale, which lets the scale test rise to the top of
// the block, carries no coefficient across the test and keeps the second view's copy in registers. On the lead: the
// depth limits as named locals 260 (masked 88); the second offsets computed first 249 (masked 65); the scale read
// once into a local ahead of both views 256 (masked 93); the scale step written out per component in the macro 252
// (same code). Lead saved as build/r201/lead-0x501e0.patch; nothing kept (every form is above 221).
// Caller scale test at each view, its own scale sample, one shared test, or one shared sampled scale and
// test 278/283/282/308. Selecting the complete scaled vector in the two arms on those forms
// 311/319/310/306. NOT kept: all raise the score. The 0x20 frame, second displacement and coefficient
// homes still differ.
// M3 lane 11 r133 (fast with 0x30290, 0x30bf0 and 0x142640, all MATCH, 0x4f7e0 unchanged at 325; control 221, aligned 249,
// register-masked 154; body unchanged). Lane 8 r201's macro lead re-measured 252 (1136 B, frame 0x14, aligned 174, masked
// 62). On it: the second offset loop first 249 (masked 65); the orthographic segment written into the second offset vector
// 253 (masked 70), with the second offset first too 250 (73); the macro's scaled copy as three reals set to `scale * input`
// under the test 297 (1176 B, masked 119); the two offset loops as six component statements with the second point first
// 226 (1136 B, masked 67). On this body: those six statements with the second point first 220, the first point first 218
// (1115 B; three points, under the keep bar). Reading: retail computes the second point's offsets first through one
// register and stores x and y at once, never carries a second-point offset in a register through the first view (ours
// holds its x in XMM0 from the opening to the second view's default copy and stores it last), loads the scale only after
// the last origin use (XMM6, the origin z's register) and gives XMM7 to right.i, loaded once in the first view (`movaps
// xmm1, xmm7` before its product) and used up by the second view's first x product; the second view's scaled x and z then
// live in [esp+4] and [esp] with y in XMM0. Every first-view product loads its coefficient first, and in the second view
// only the three x products do: the nine coefficients behave as values shared between the views, of which only right.i gets
// a register.
// MiniMax E (11-20): no new form, no lower score (221): the frame is still 0x20 against retail's 0x14 - retail keeps the first point's three offsets in registers and spills the second point's, where this build materialises both offset vectors; the explicit-component form measured 218 and the swapped offset loops 220, both under the 8-point bar, so the body stands.
// MiniMax B (11-20): no new form, no lower score (221): the frame stays 0x20 against retail's 0x14 because the first
// offset vector still needs a stack home (its address reaches the view helper); writing the offsets as explicit
// component assignments reaches 218 (under the 8-point bar) and a by-component view helper 265, both kept out.
// Input-coordinate cursors for first, second or both displacements 221/221/226;
// with destination cursors in those passes 222/221/222. A fused source-cursor
// loop with indexed or cursor destinations 218/221.
// NOT kept: the fused indexed form gains three and retains the 0x20 frame.
// M3 lane 14 r133 (fast with 0x30290, 0x30bf0 and 0x142640, all MATCH; control 221): the view helper taking the vectors by
// reference 221 (same code); the scaled vector assigned in an if/else (`scaled = *in` when the scale is 1) 310 (1187 B);
// the scaled vector initialised by aggregate copy 317 (1187 B); the weights as unary minus 221 (same code); the else
// arm's memset as an aggregate zero assignment 227 (1120 B).
// Paired coordinate displacements, first/second endpoint first, loop 218/218 and scalar
// rows 216/217. Per-coordinate scaled selection with direct/shared scale tests,
// aggregate 289/256 and loop 303/279. NOT kept: the four displacement forms gain at
// most five. The frame stays 0x20, and the second displacement and coefficient homes differ.
// M3 lane 11 r143 (h2m fast with 0x30290, 0x30bf0 and 0x142640, all MATCH; control 221): the two input points through
// named `first`/`second` pointers for every use 220 (1114 B); the view helper as ordinary `inline` 221 (same code); one
// shared loop counter for both offset loops 220 (1113 B); both offsets in one loop 218 (the paired form above,
// re-measured); the second view written to a new vector (the first in place) 284 (1104 B).
// First displacement initializer, in-place/separate view 220/219; second 218/217;
// both first-endpoint first 218/217; both second-endpoint first 220/218.
// NOT kept: gains of at most four, with frame 0x20 and different second-offset homes.
// M3 lane 2 r129 (h2m fast with 0x30290, 0x30bf0 and 0x142640, all MATCH, 0x4f7e0 unchanged at 325; control 221): the second
// offset loop first with the depth limits written in the condition 260 (1114 B); the else arm's weights set as local_5,
// scale, then local_4 from local_5 221 (same code); the blend terms written first point first 221 (same code: the
// operands canonicalise, ours still adds into the first point's product); the helper reading the camera's scale itself
// (no scale parameter) on this body 218 (1115 B, -3); the origin copied once into a const local for both offset loops 262
// (1136 B, retail's length, but the copy goes through the frame as dwords; frame still 0x20).
// @retail 0x501e0
void function_501e0(s_501e0 const *arg_0, bool arg_1, s_501e0 *arg_2)
{
    vector3f local_0;
    vector3f local_1;
    for (long component = 0; component < 3; ++component)
        local_0.n[component] = arg_0[0].field_0.n[component] - g_4b9da0.n[component];
    for (long second_component = 0; second_component < 3; ++second_component)
        local_1.n[second_component] = arg_0[1].field_0.n[second_component] - g_4b9da0.n[second_component];
    world_vector_to_view(g_4b9e14.scale, &local_0, &local_0);
    world_vector_to_view(g_4b9e14.scale, &local_1, &local_1);
    real first_depth_limit = local_0.k * local_0.k * 4.f;
    real second_depth_limit = local_1.k * local_1.k * 4.f;
    if (local_0.k < 0.f && local_1.k < 0.f &&
        first_depth_limit > local_0.i * local_0.i &&
        first_depth_limit > local_0.j * local_0.j &&
        second_depth_limit > local_1.i * local_1.i &&
        second_depth_limit > local_1.j * local_1.j)
    {
        real local_4 = 0.f - local_0.k;
        real local_5 = 0.f - local_1.k;
        real scale;
        if (arg_1)
            scale = 1.f / (local_5 + local_4);
        else
        {
            local_4 = 1.f;
            local_5 = 1.f;
            scale = 0.5f;
        }
        arg_2->field_0.x = (arg_0[1].field_0.x * local_4 + arg_0[0].field_0.x * local_5) * scale;
        arg_2->field_0.y = (arg_0[1].field_0.y * local_4 + arg_0[0].field_0.y * local_5) * scale;
        arg_2->field_0.z = (arg_0[1].field_0.z * local_4 + arg_0[0].field_0.z * local_5) * scale;
        arg_2->field_c = (arg_0[1].field_c + arg_0[0].field_c) * 0.5f;
        arg_2->field_10 = (arg_0[1].field_10 + arg_0[0].field_10) * 0.5f;
        arg_2->field_14 = arg_0[0].field_14;
        if (arg_1)
        {
            real local_7 = (local_1.i * local_4 - local_0.i * local_5) * 2.f;
            real local_8 = (local_1.j * local_4 - local_0.j * local_5) * 2.f;
            real local_9 = local_8 * local_8 + local_7 * local_7;
            if (local_9 >= 0.f)
                arg_2->field_c = (real)(sqrt(local_9) * arg_2->field_c * scale);
        }
        else
        {
            vector3f local_10;
            local_10.i = arg_0[1].field_0.x - arg_0[0].field_0.x;
            local_10.j = arg_0[1].field_0.y - arg_0[0].field_0.y;
            local_10.k = arg_0[1].field_0.z - arg_0[0].field_0.z;
            scale = function_30bf0(&local_10);
            real local_12 = dot3f(&g_4b9dac, &local_10);
            real local_13 = 1.f - local_12 * local_12;
            real local_14 = local_13 < 0.f ? 0.f : local_13 > 1.f ? 1.f : local_13;
            arg_2->field_c = (real)(sqrt(local_14) * arg_2->field_c * scale);
        }
    }
    else
    {
        memset(arg_2, 0, sizeof(*arg_2));
    }
}
