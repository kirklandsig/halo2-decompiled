// @flags /O2 /Gr /arch:SSE
/* SPHERICAL_HARMONICS.CPP: real spherical harmonics up to the fourth order
   (16 coefficients): the basis evaluated in a direction, and a directional
   light projected onto it */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <math.h>

#define k_spherical_harmonics_pi 3.14159265f

/* the basis functions of the first order*order bands in a direction */
// score 59 (was 59): from +0x50 the x87/SSE schedule differs (retail loads band1 into xmm0 before
// result[0]'s x87 product); retail keeps sqrt(2) and sqrt(105) on the x87 stack to the end
// (fmul st(1)/st(2), two fstp st(0) before the return) where ours consumes sqrt(2) (fmulp st(2));
// retail computes coefficient 15 right after band3_35 with -0.125 held in xmm3; and the cubic homes
// rotate (retail: zz*5 in zz's slot, band3_35 in sqrt(15)'s, band3_21 new; ours: band3_35 in zz's,
// band3_21 in sqrt(15)'s, zz*5 new). These homes follow lifetimes (slot sharing); a no-/GL /FAs
// build shows names never move them.
// Tried: outer/cubic order return 123/123; roots before inputs 137; early Z square 128; first-band
// Z store first 128; repeated inline sqrt(2.0)/sqrt(105.0) 158-162 (retains the roots like retail
// but moves the band-3 products); /Op 383 (0x146020 then differs); first-band, first-Y,
// square-difference and quadratic mixed-product stagings 123; cubic Y staging 129; mixed-root
// staging 123; final-root product chain 113 (kept then, 651/660); linear plus final chain 113; XZ
// product chain 59 (kept); second cubic root chain, coefficient 8/12/9/5/6 chains, root/edge
// combination 113; repaired XZ plus early coefficient 15 147/148; cubic X chain 139; first cubic
// root chain 59; repaired-prefix linear Y/Z/X/all values 59; late root-five/depth square 59/59;
// the +0x50 x87/SSE schedule (retail loads the band-1 product into SSE before
// result[0]'s x87 product), the cubic band's shared product, spill homes and root lifetimes differ.
// Ours computes zz5_1 * band3_21 * inverse_sqrt_pi once for result[11] and result[13] (VC reorders
// every spelling of the chain the same way); retail computes result[13] in source order
// (inverse_sqrt_pi * x first) with nothing shared, which is the six bytes. Retail keeps zz*5 in
// zz's slot, band3_35 in sqrt(15)'s and band3_21 in a new one, and never consumes sqrt(2) or
// sqrt(105) at their last use (fmul st(1)/st(2), two fstp st(0) before the return).
// Z store first 128; repeated inline sqrt(2.0)/sqrt(105.0) 158-162; /Op 383 (0x146020 then
// differs); first-band, first-Y, square-difference and quadratic mixed-product stagings 123; cubic
// Y staging 129; mixed-root staging 123; final-root product chain 113 (kept); linear plus final
// chain 113; XZ product chain 59 (kept); second cubic root chain, coefficient 8/12/9/5/6 chains,
// root/edge combination 113; repaired XZ plus early coefficient 15 147/148; cubic X chain 139;
// first cubic root chain 59; linear Y/Z/X/all values 59; late root-five/depth square 59/59; linear
// scope 59; later common root 59; isotropic result lifetime, band-two normalization, Z-square,
// cubic-axis/polynomial accumulators 59; repaired XZ plus cubic-Y chain 137; external direction
// reference, both double root chains 59; per-use common root 59; per-use mixed/both roots 78/78;
// completed cubic X/Y/paired results 59; paired results with per-use root 59; complete mixed
// result 59; first Y store before isotropic 59; linear factor after isotropic 143; quadratic root
// after squares 138; axis-root scope 59; depth/edge/axis/joint scalar homes 59; double or
// +0x50 x87/SSE schedule (the band-1 SSE load comes before result[0]'s x87
// product in ours), the cubic band's spill homes and the root lifetimes differ: retail never
// consumes sqrt(2) or sqrt(105) at their last use (fmul st(1)/st(2), two fstp st(0) before the
// return), computes coefficient 15 in SSE right after band3_35 (-0.125 already in xmm3), keeps zz*5
// in zz's slot, band3_35 in sqrt(15)'s and band3_21 in a new one; ours consumes sqrt(2) (fld st(1);
// fmulp, then fmulp st(2); fxch) and stores 11, 13 and 15 together. With sqrt(2), sqrt(105) and
// sqrt(7) all double the x87 root code is retail's, but the spill slots rotate (zz*5 new, band3_35
// in zz's, band3_21 in sqrt(15)'s), -0.125 loads late and 0.5/0.25 become double constants: 77.
// Tried (from 123): outer/cubic order return 123/123; roots before inputs 137; early Z square 128;
// first-band Z store first 128; repeated inline sqrt(2.0)/sqrt(105.0) 158-162 (retains the roots like
// retail but moves the band-3 products); /Op 383 (0x146020 then differs); first-band product, first
// Y product, square-difference and quadratic mixed-product stagings 123; cubic Y staging 129;
// mixed-root staging 123; final-root product chain 113 (651/660); linear plus final chain 113; XZ
// product chain 59 (kept); second cubic root chain, coefficient 8/12/9/5/6 chains and root/edge
// root chain 59.
// At 59: repaired-prefix linear Y/Z/X/all values 59 each; late root-five/depth square 59/59; linear
// scope 59; later common root 59; isotropic result lifetime, band-two normalization, Z-square and
// cubic-axis/polynomial accumulators 59; repaired-XZ plus cubic-Y chain 137; external direction
// reference and both double root chains 59; per-use common root 59; per-use mixed/both roots 78/78;
// completed cubic X, Y and paired results 59/59/59; paired results with per-use root 59; complete
// mixed result 59; first Y store before isotropic 59; linear factor after isotropic 143; quadratic
// root after squares 138; axis-root scope 59; depth/edge/axis/joint scalar homes 59 each; double or
// unrounded common root 158 (keeps band3_35 on x87); root rounded per factor 158; plus early
// coefficient 15 140; early coefficient 15 alone 147; retail store order 144; native float roots
// common/mixed/both 59/59/59, linear/quadratic 56/57, axis pair 59, cubic factor pair 158, every
// root 156, linear plus quadratic 54 (float constants, not retail's double ones), other rounded
// roots added 54; both early root boundaries with four/six cubic product chains 139/139; all
// three roots double 77 (x87 root code retail's, slots rotate, -0.125 loads late); sqrt(105) and
// sqrt(7) double 66; sqrt(105) double 73; zz scaled in place 70; all double with coefficient 15
// ahead of 9 150 (645 bytes); double roots with `(real)` casts at both/one/other sqrt(105) use
// 80/82/82; double sqrt(2)/sqrt(105) with real sqrt(7) 82; coefficient 15 recomputing the root
// product 130; result[0] after result[3] 59; double sqrt(105)/sqrt(7) with real sqrt(2) 66;
// result[0] in double 64; coefficient 15 at each position, root types and local names (screened)
// no better; renamed spilled locals 59; result[13] before result[11] 59 (same code); zz5 - 1 at
// both uses 90; result[14] as one product chain 69 (650 bytes); result[13] staged from
// inverse_sqrt_pi * x 139; band3_21 initialised at its declaration 59 (same code); about 25 other
// spellings of the band-3 chains (operand orders, the constant first, a named 3xx - yy, const
// roots, declaration positions) screened, all the same code.
// roots added 54; both early root boundaries with four/six cubic product chains 139/139; double
// sqrt(2)/sqrt(105)/sqrt(7) with (real) sqrt105 at both uses 80, at coefficient 10 or 14 only 82/82;
// double sqrt(2)/sqrt(105) with real sqrt(7) 82; coefficient 15 recomputing the root product 130;
// result[0] after result[3] 59; double sqrt(105)/sqrt(7) with real sqrt(2) 66; result[0] in double
// 64; double sqrt(105) alone 73; zz scaled in place 70; all double with coefficient 15 before
// coefficient 9 150 (645 bytes); coefficient 15 at each position in the order, root types and local
// names (screened) no better; renamed spilled locals (slot names) 59.
// Round 17: all three roots double 77 (aligned 38); on it zz5, band3_35 or band3_21 renamed to sort
// first or last 77/77/77 (names don't move these homes); double sqrt(2) alone 158.
// product in ours), cubic Y/scalar homes and the retained roots differ: retail never consumes
// sqrt(2) or sqrt(105) at their last use (fmul st(1)/st(2), two fstp st(0) before the return;
// result[14] in source order), ours folds them in (fmulp st(2), result[14] reassociated around
// sqrt105). Tried: outer order return 123; cubic order return 123; roots before inputs 137; early
// Z square 128; first-band Z store first 128; repeated inline sqrt(2.0)/sqrt(105.0) 158-162; /Op 383
// (and 0x146020 stops matching); first-band product staging, first Y product, square-difference
// staging and quadratic mixed-product staging 123; cubic Y staging 129; mixed-root staging 123;
// final-root product chain 113 (651/660); linear plus final chain 113; XZ product chain 59 kept;
// second cubic root chain, coefficient 8/12/9/5/6 chains and root/edge combination 113; repaired
// XZ plus early coefficient 15 147/148; cubic X chain 139; first cubic root chain 59; repaired-prefix
// +0x50 x87/SSE scheduling, the cubic band's spill homes and the root lifetimes
// differ: retail keeps sqrt(2) and sqrt(105) on the x87 stack to the end (fmul st(1)/st(2), two
// fstp st(0)), computes coefficient 15 in SSE right after band3_35 (-0.125 already in xmm3), keeps
// zz*5 in zz's slot and band3_35 in sqrt(15)'s. Forms (123 at the start): outer/cubic order
// return 123/123; roots before inputs 137; early Z square 128; first-band Z store first 128;
// repeated inline sqrt(2.0)/sqrt(105.0) 158-162; /Op 383; first-band, first-Y, square-difference,
// quadratic mixed-product stagings 123; cubic Y staging 129; mixed-root staging 123; final-root
// product chain 113; XZ product chain 59 (kept); second cubic root chain, coefficient 8/12/9/5/6
// chains, root/edge combination 113; repaired XZ plus early coefficient 15 147/148; cubic X chain
// 139; first cubic root chain 59; linear Y/Z/X/all values 59; late root-five/depth square 59/59;
// linear scope 59; later common root 59; isotropic result lifetime, band-two normalization,
// Z-square, cubic-axis/polynomial accumulators 59; repaired-XZ plus cubic-Y chain 137; external
// direction reference, both double root chains 59; per-use common root 59; per-use mixed/both roots
// 78/78; completed cubic X/Y/paired results 59; paired results with per-use root 59; complete mixed
// result 59; first Y store before isotropic 59; linear factor after isotropic 143; quadratic root
// after squares 138; axis-root scope 59; depth/edge/axis/joint scalar homes 59; double or unrounded
// common root 158 (keeps band3_35 on x87); root rounded per factor 158; plus early coefficient 15
// 140; early coefficient 15 alone 147; retail store order 144; native float roots common/mixed/both
// 59, linear/quadratic 56/57, axis pair 59, cubic factor pair 158, every root 156, linear plus
// quadratic 54 (float constants, not retail's double ones), other rounded roots added 54; both early
// root boundaries with four/six cubic product chains 139/139; all three roots double 77 (retail's
// x87 root code, but zz*5 new, band3_35 in zz's slot, band3_21 in sqrt(15)'s, -0.125 loads late);
// sqrt(105) and sqrt(7) double 66; sqrt(105) double 73; double sqrt(2)/sqrt(105) with a real sqrt(7)
// 82; double roots with `(real)` at both/one/other sqrt(105) use 80/82/82; coefficient 15
// recomputing the root product 130; result[0] after result[3] 59; result[0] in double 64; zz scaled
// in place 70; all double with coefficient 15 ahead of 9 150 (645 bytes); coefficient 15 at each
// position, root types and local names (screened) no better; renamed spilled locals 59. Round 43:
// band3_21 defined before band3_35 158; zz*5 after the roots 129; sqrt(2) double alone 158;
// result[14] as one expression 69; sqrt(7) written at its use 59.
// root boundaries with four/six cubic product chains 139/139; double sqrt(2)/sqrt(105)/sqrt(7) with
// (real) sqrt105 at both uses 80, at coefficient 10 or 14 only 82/82; double sqrt(2)/sqrt(105) with
// real sqrt(7) 82; coefficient 15 recomputing the root product 130; result[0] after result[3] 59;
// double sqrt(105)/sqrt(7) with real sqrt(2) 66; result[0] in double 64; double sqrt(105) alone 73;
// renamed spilled locals (slot names) 59.
// root 59.
// first +0x50 x87/SSE schedule and retained sqrt(2) still differ.
// Isotropic result lifetime, band-two normalization, Z-square and cubic-axis/polynomial accumulators 59;
// repaired-XZ plus cubic-Y chain 137; external direction reference and both double root chains 59.
// +0x50 scheduling, cubic homes and sqrt(2) retention remain.
// Repaired-prefix per-use common root 59; per-use mixed/both roots 78/78; completed cubic X,
// Y and paired results 59/59/59; paired results with per-use root 59; complete mixed result 59.
// x87/SSE scheduling and retained common root still differ.
// First Y store before isotropic 59; linear factor after isotropic 143; quadratic root
// after squares 138; axis-root scope 59; depth/edge/axis/joint scalar homes 59/59/59/59.
// a double or unrounded common root is retained like retail's but then keeps
// band3_35 on x87 and moves coefficient 15 to x87: double root 158; root rounded per factor 158;
// plus early coefficient 15 140; early coefficient 15 alone 147; retail store order 144.
// Native float roots: common/mixed/both 59/59/59; linear/quadratic 56/57; axis pair 59;
// cubic factor pair 158; every root 156; linear plus quadratic roots 54; other rounded roots added 54;
// both early root boundaries with four/six cubic product chains 139/139. The 54 form is below the eight-point gate.
// +0x50 x87/SSE scheduling, the cubic homes and root retention differ: retail
// never consumes sqrt(2) or sqrt(105) at their last use (fmul st(1)/st(2), two fstp st(0) before the
// return), computes coefficient 15 right after band3_35 with -0.125 held in xmm3, and keeps zz*5 in
// zz's slot, band3_35 in sqrt(15)'s and band3_21 in a new one; ours consumes sqrt(2) (fmulp st(2)).
// With sqrt(2), sqrt(105) and sqrt(7) all double the x87 root code is retail's, but the spill
// slots rotate (zz*5 new, band3_35 in zz's, band3_21 in sqrt(15)'s) and -0.125 loads late: 77.
// Tried, at 123: outer order return; cubic order return; roots before inputs 137; early Z square
// 128; first-band Z store first 128; repeated inline sqrt(2.0)/sqrt(105.0) 158-162 (retains the
// roots like retail but moves the band-3 products); /Op 383 (0x146020 then differs). First-band
// product staging, first Y product, square-difference staging and quadratic mixed-product staging
// 123; cubic Y staging 129; mixed-root staging 123; final-root product chain 113 kept (651/660);
// linear plus final chain 113. XZ product chain 59 kept; second cubic root chain, coefficient
// 8/12/9/5/6 chains and root/edge combination 113; repaired XZ plus early coefficient 15 147/148;
// cubic X chain 139; first cubic root chain 59. At 59: repaired-prefix linear Y/Z/X/all values 59
// each; late root-five/depth square 59/59; linear scope 59; later common root 59; isotropic result
// lifetime, band-two normalization, Z-square and cubic-axis/polynomial accumulators 59;
// repaired-XZ plus cubic-Y chain 137; external direction reference and both double root chains 59;
// per-use common root 59; per-use mixed/both roots 78/78; completed cubic X, Y and paired results
// 59/59/59; paired results with per-use root 59; complete mixed result 59; first Y store before
// isotropic 59; linear factor after isotropic 143; quadratic root after squares 138; axis-root
// scope 59; depth/edge/axis/joint scalar homes 59/59/59/59; double root 158; root rounded per
// 144. All three roots double 77; sqrt(105) and sqrt(7) double 66; sqrt(105) double 73; zz scaled
// in place 70; all double with coefficient 15 before coefficient 9 150 (645 bytes); coefficient
// 15 at each position in the order, root types and local names (screened) no better. With the float
// roots: linear only 56; quadratic only 57; linear and quadratic, or every rounded root 54 (gives retail's two early schedules; not kept, a gain of 5); a
// double sqrt(2) 156 (639 bytes); double sqrt(2) and sqrt(105) 78; coefficient 15 right after
// band3_35 146; coefficient 15 as the first cubic output 145; band3_21 before band3_35 128.
// 15 at each position in the order, root types and local names (screened) no better.
// Round 10 (statement orders, real builds): natural 9-15 69; 15 first 147; 15 right after band3_21
// 149; 9 first 67; 10 and 14 before 9 81. All three roots double rebuilt: 77 (sqrt(2) kept on the
// x87 stack like retail, but the 0.5/0.25 factors of the sqrt(105)/sqrt(7) products turn double).
// result[0]'s x87 product), the shared cubic product, the spill homes and the root lifetimes differ.
// Ours canonicalises result[11] and result[13] into one shared zz5_1 * band3_21 * inverse_sqrt_pi
// product (every operand order compiles the same way), which is the six bytes; retail computes every
// chain in source order with nothing shared, computes coefficient 15 right after band3_35 (-0.125 in
// xmm3), keeps zz*5 in zz's slot, band3_35 in sqrt(15)'s and band3_21 in a new one, and never consumes
// sqrt(2) or sqrt(105) at their last use (fmul st(1)/st(2), two fstp st(0) before the return).
// Tried (59 where no score is given): outer/cubic order return 123/123; roots before inputs 137; early
// (0x146020 then differs); first-band, first-Y, square-difference and quadratic mixed-product stagings
// 123; cubic Y staging 129; mixed-root staging 123; final-root product chain 113 (kept, 651/660);
// linear plus final chain 113; XZ product chain (kept); second cubic root chain, coefficient
// 8/12/9/5/6 chains, root/edge combination 113; repaired XZ plus early coefficient 15 147/148; cubic X
// chain 139; first cubic root chain; linear Y/Z/X/all values; late root-five/depth square; linear
// scope; later common root; isotropic result lifetime, band-two normalization, Z-square, cubic-axis/
// polynomial accumulators; repaired XZ plus cubic-Y chain 137; external direction reference; both
// double root chains; per-use common root; per-use mixed/both roots 78/78; completed cubic X/Y/paired
// results; paired results with per-use root; complete mixed result; first Y store before isotropic;
// linear factor after isotropic 143; quadratic root after squares 138; axis-root scope; depth/edge/
// axis/joint scalar homes; double or unrounded common root 158 (keeps band3_35 on x87); root rounded
// per factor 158; plus early coefficient 15 140; early coefficient 15 alone 147; retail store order
// 144; native float roots common/mixed/both 59/59/59, linear/quadratic 56/57, axis pair 59, cubic
// factor pair 158, every root 156, linear plus quadratic 54 (float constants, not retail's double
// ones), other rounded roots added 54; both early root boundaries with four/six cubic product chains
// 139/139; all three roots double 77 (x87 root code retail's, slots rotate, -0.125 loads late);
// sqrt(105) and sqrt(7) double 66; sqrt(105) double 73; zz scaled in place 70; all double with
// coefficient 15 ahead of 9 150 (645 bytes); double roots with `(real)` casts at both/one/other
// sqrt(105) use 80/82/82; double sqrt(2)/sqrt(105) with real sqrt(7) 82; coefficient 15 recomputing
// the root product 130; result[0] after result[3]; double sqrt(105)/sqrt(7) with real sqrt(2) 66;
// result[0] in double 64; renamed spilled locals; statement orders natural 9-15 69, 15 first 147, 15
// right after band3_21 149, 9 first 67, 10 and 14 before 9 81; result[13] before result[11] (same
// code); `zz5 - 1.0f` at both uses 90; result[14] as one chain 69; result[13] staged 139; band3_21
// set at its declaration (same code).
// Round 12 (lane 15): result[11] staged 69 (retail's 660 bytes, a product still shared); band3_21's
// product written at both uses 176; xx - yy unnamed 133; all roots double with `zz5 - 1.0f` at both
// uses 97, plus result[11] staged 94; operand-order respellings of 18 other statements, one at a
// time, all the same code.
// N226: Complete linear/quadratic/cubic product-stage band subsets
// 59/59/59/139/139/139/139; all with constant root-five/root-seven initialization
// at declaration 139. Original complete body, precision and expression trees restored.
// Round 45 (lane 9): compile-only screen of sqrt(2)/sqrt(105) real or double x natural / retail store order /
// current order: retail's store order (15, 11, 13, 9, 10, 12, 14) only with both roots double in that statement
// order. Real builds: that form 150 (645 bytes, 0.5/0.25 become double constants); with both roots real 135; with
// sqrt(105) real 132; with sqrt(2) real 138; current order with coefficient 14 as one chain and sqrt(7) at its use 141.
// Round 27 (lane 11): `>= 3`/`>= 4` order tests 61 (the compare changes); the direction copied into a
// vector local first 165 (645 bytes); `0.5f * inverse_sqrt_pi` 59; a double 1.0 numerator for the
// inverse root 60; the first-band products with the constant first 59.
// Round 84: all cubic roots double with a double final accumulator 79; two-stage zz5 79;
// early coefficient 15 151; independent cubic X/Y accumulators 42; in-place depth polynomial 53;
// native linear/quadratic roots on that graph 37; original/renamed in-place depth 49/49;
// direct coefficient 11 37; direct final chain 39; inline declaration target 37 but sibling 169 fast;
// band3_35 before zz5 36; band3_21 before zz5 49; only sqrt2 double 30; early zz5/band3_21
// declarations 30 each. Best 30 plain at 660/660 but sibling 0x146020 scores 11; original body restored.
// Round 107: that 30 form's sibling loss is 0x146020's frame growing 0x40 -> 0x44 (a double local makes
// this function want an 8-aligned stack); retail has no double local. Retail keeps sqrt(2) only because
// sqrt(105) is pushed above it before band3_21 (it can't pop a buried root), so its band3_21 product is
// `fmul st(2)` where ours absorbs the dying root (`fmulp st(2)`). Fast trials, 0x146020 matched in all: a
// fresh cubic block (roots real locals or inline, zz5 new or zz scaled in place) in retail's order
// (15, 11, 13, 9, 10, 12, 14) 141-156, in the current order with result[14] as one expression 69-162
// (both roots local 69/80, inline 79/89); sqrt(2.0) written in both band products on this body 158.
// Lane 14 r19: why 0x146020 drops to 11: a named `double` root gives this function an 8-byte stack
// alignment need that LTCG passes to the caller (its frame grows 0x40 -> 0x44); `sqrt(2.0)` written in
// both band products gives the same retained x87 root without it. 0x146020 also keeps `order` in EDX
// across the call, so this body must never write EDX. Forms (fast; 0x146020 checked each time):
// double sqrt2 with result[13] staged as an accumulator 131 (prologue reshuffled); that staging with
// the float root 139; plus band3_35 defined before zz5 67 (662 bytes, prefix back, coefficient 15
// lands where retail computes it); with sqrt2 double 35 at 660 (plain: 0x146020 11, frame 0x44); with
// `sqrt(2.0)` in both products instead 35 (plain 35, 0x146020 matched); zz5/band3_21 renamed 35/35;
// band3_21 defined before zz5 / before zz5_1 / after xx_3yy / first 56/54/54/137; band3_21 and sqrt7
// declared at the top 35; result[11] operands swapped 35; `zz5 - 1.0f` at both uses (no zz5_1) 14
// (plain 14); zz scaled in place 30; a named `real sqrt3 = (real)sqrt(3.0);` with band1 = sqrt3 *
// inverse_sqrt_pi 11 (fixes the +0x50 schedule; on the original body alone 56); band3_21's product at
// both uses 8 (plain 8, 0x146020 matched: left the quadratic sqrt15-load/z*z swap, zz5 in a new slot
// where retail reuses zz's, and the sqrt(105) fsqrt/mulss order); on that: zz5 first 131, result[12]
// earlier 38, result[13] first 70, zz*5 at each use 12, sqrt(105) at both uses 51, result[6]
// operands swapped / sqrt5 initialised / zz after sqrt5 / zz before band2 8 each, sqrt15 and band2
// after the squares 150, band2 after the squares 98. Legal forms tried without the at-each-use
// products: the named sqrt3 root alone on the original 56; sqrt3 + band3_35 first + staged result[13]
// 64; nothing kept. Then: the sqrt3 root on the 35 form (named zz5_1/band3_21) 32; on the 8 form
// named float roots `real sqrt35 = (real)sqrt(35.0);` / `real sqrt21 = (real)sqrt(21.0);` feeding the
// products 6 (plain 6, 0x146020 matched; fixes the sqrt(105) order; left: the quadratic sqrt15-load/
// z*z swap and zz5 in a new slot where retail reuses zz's); on that: band3_21 named again 8, zz5_1 named
// again 33, sqrt21 declared with the other roots 6, zz5 defined before result[11] 188 (700 bytes),
// zz5 after xx_3yy 6, sqrt5 initialised at its declaration 6. The 6 form still writes sqrt(2.0),
// `zz5 - 1.0f` and the band3_21 product at each use, so it is only keepable as a full match.
// Round 14 (lane 2): that 6 form spliced into the current file is still 6 (fast, 0x146020
// matched); left the sqrt15 load retail puts between `fld z` and `fmul z`, and zz5/band3_21 trading
// stack slots (retail stores zz5 over zz). On it: zz5 renamed a_zz5/zz_5/z5 6/6/6, zz renamed 6;
// permuter (anneal, 600 s) 50 tries, best 6. Original body restored.
// Lane 16 r46: that 6 form rebuilt (fast 6, 0x146020 matched); on it: zz5 renamed azz5/zzz5 6/6; a named
// band3_21 (three names, zz5 renamed too) 8 each, declared before zz5 8, computed before zz5 51; zz*5 at
// each use 188 (700 B), with a named band3_21 12; result[6] `(zz * 3 - 1) * sqrt5` / `3 * zz` 6/6;
// result[4] band2 first, result[8] sqrt15 first, result[5] z first, band2 isp-first 6 each; `5.0f * zz`,
// result[12] zz5 term first, sqrt7 initialised 6 each; zz declared before sqrt15 and assigned after band2
// or after xx_yy 6/6. Names and declaration order never move zz5's slot, and none of these statement or
// operand orders moved the quadratic sqrt15 load ahead of the x87 z*z.
// Round 135 (lane 4, fast with 0x146020 unless noted): the 6-point form (named sqrt3/sqrt35/sqrt21 roots,
// `sqrt(2.0)` in both cubic products, `zz5 - 1.0f` and the sqrt21 product at each use, result[13] staged)
// rebuilt 6. A compile-only harness shows the new stack homes follow statement order: with the sqrt21 product
// named right after sqrt21 (before result[9]) and result[11]/result[13] written after result[12] (order 9, 10,
// 12, 11, 13, 14, 15, xx_3yy after result[12]), zz5 takes zz's home and band3_21 the new one as in retail: 4
// (plain 4; left the quadratic sqrt15-load/`fmul z` swap and the result[13] store after result[9]'s `fmul y`);
// zz5_1 named on it 31; with a named float `real sqrt2 = (real)sqrt(2.0);` in both products 2 (only the
// quadratic swap left; it still writes `zz5 - 1.0f` at each use, so it is keepable only as a match); that with
// zz5_1 named 31 (rests on one-use sqrt3/sqrt35/sqrt21 locals); with those three roots inline again 66 (662
// bytes); the 9, 10, 12, 11, 13 order on this body 59. Harness on the 2 form: 196 quadratic-block orders and
// spellings (sqrt5 with its value, band2/result[6]/xz operand orders, zz staged) and 77 first-block orders keep
// the sqrt15 load after `fmul z`; without the cubic block the swap stays, so it is not a cubic-block effect.
// Round 33 (m3 lane 11): lane 14 r19's 6-point form rebuilt (fast 6, 0x146020 matched). A compile-only
// /FAs build of this file reproduces both residual differences. Screened there: band3_21 or zz5
// renamed (8 names) and band3_21 declared at five earlier places: no change; 80 single statement
// moves and 150 random order/operand mixes in the quadratic block, 120 in the first block, and the
// xz/x/difference stagings undone: the sqrt15 load never moves ahead of the x87 z*z. zz5 takes zz's
// home (as retail) only when it is defined after the sqrt(21) product: with a named
// `real band3_21 = (real)(sqrt21 * sqrt(2.0));` after sqrt105 and zz5 after it, fast 12 (band3_21's
// home then lands below xx, yy and xy where retail has it above them, and the result[13] store slips
// one place); 360 orders of the later statements keep that store slip.
// Round 55 (lane 12; a one-file LTCG harness reproduces this function and 0x146020 exactly; 0x146020 matched in
// every trial): legal forms only. The named sqrt3 root with band3_35 defined right after sqrt2 (before zz5) 55
// (band3_35 then takes sqrt(15)'s slot as retail; a gain of 4, not kept); that with sqrt(35) named 77, with
// sqrt(21) named 71; band3_35 first with both roots named 80; the sqrt3/band3_35 form with band3_21 defined before
// sqrt105 73. Harness, on the sqrt3/band3_35 form: `sqrt2 * sqrt(35.0)` / `sqrt2 * sqrt(21.0)` operand orders
// (same code), sqrt105 defined right after sqrt2 (much worse), result[15] at each earlier position (worse), and
// every single-statement move of the nine cubic statements (none better).
// Round 9 (m3 lane 18; fast, 0x146020 matched in each): lane 4's 2-point form rebuilt 2; on it band2 staged
// (`band2 = sqrt15; band2 *= inverse_sqrt_pi;`) 2, band2 declared at the top but assigned after zz 92 (663 bytes),
// zz staged (`zz = z; zz *= z;`) 2, a double sqrt5 3, result[7] as one product (no quadratic_xz) 133 (669 bytes).
// Compile-only screens on the 2-point form (the harness reproduces the swap): 450 random quadratic-block and
// first-block orders with operand/staging spellings, zz written at each use, double sqrt15/sqrt5 (with and
// without casts), zz through a double product, sqrt5 written inline or first, zz first, the zz read from
// direction->k, `zz + zz + zz`, band2 as a double product, and /G7: the sqrt15 load never moves ahead of
// `fmul z` (result[8] through band2 moves it, but then band2 goes to x87 and sqrt15 disappears).
// Round 18 (m3 lane 2, fast; 0x146020 matched in every trial): the named sqrt3 root with band3_35 right after sqrt2
// rebuilt 55 (retail keeps sqrt(2) dead on the x87 stack to the end and computes result[15] first, between
// fsqrt(105) and band3_21); on it: sqrt2/band3_35/sqrt105 as `real const` 55; result[13] before result[11] 55;
// constant-first `3.0f * xx`/`3.0f * yy` 55; result[15] right before band3_21 146 (647 bytes); xx_3yy defined just
// before result[15] 55. Original body restored.
// Round 102 (m3 lane 8, fast; 0x146020 matched in each): band1 from a float root `(real)sqrt(3.0) * inverse_sqrt_pi`
// 56 (fixes +0x50); with the cubic statements in the 9, 10, 12, 11, 13 order 56, plus xx_3yy after result[12] 56;
// result[11] staged as `zz5_1 * band3_21`, `*= inverse_sqrt_pi`, `*= y` 69 at retail's 660 bytes; both cubic X/Y
// accumulators from inverse_sqrt_pi first 139 (653 bytes).
// Round 22 (m3 lane 2, fast; 0x146020 matched in each): lane 4's 2-point form rebuilt 2; permuter on it (anneal,
// 600 s, 52 tries) best 2. Compile-only on the 2-point form, the quadratic sqrt15 load stays after `fmul z` for:
// the five other input load orders, const inputs, one input declaration, the 23 other first-block store orders,
// inverse_sqrt_pi with a double numerator, a float root or declared first, zz right after sqrt15 or at the block
// top, result[5] `band2 * y * z`, sqrt5 first, result[6] first or inverse_sqrt_pi first, result[7] unstaged.
// Real builds on this body: const inputs 59; result[5] `band2 * y * z` 59; sqrt5 set first 59; result[6] with
// inverse_sqrt_pi first 59.
// Round 59 (lane 16, fast; 0x146020 matched in each): lane 4's 2-point form still 2. On lane 12's named-sqrt3
// / band3_35-first form (55 again): result[11] staged 72 (662 bytes); result[13] before result[11] 55
// (identical); result[13] staged 64 (662 bytes); both staged 70 (662 bytes); result[15] first, right after
// xx_3yy 147 (647 bytes).
// @retail 0x143360
void spherical_harmonics_evaluate_direction(vector3f const *direction, dword order, real *result)
{
	real x = direction->i;
	real y = direction->j;
	real z = direction->k;
	real inverse_sqrt_pi = (real)(1.0f / sqrt(k_spherical_harmonics_pi));
	real band1 = (real)(sqrt(3.0) * inverse_sqrt_pi);

	result[0] = inverse_sqrt_pi * 0.5f;
	result[1] = band1 * y * -0.5f;
	result[2] = band1 * z * 0.5f;
	result[3] = band1 * x * -0.5f;
	if (order > 2)
	{
		real sqrt15 = (real)sqrt(15.0);
		real band2 = sqrt15 * inverse_sqrt_pi;
		real xy = y * x;
		real xx = x * x;
		real yy = y * y;
		real xx_yy = xx - yy;
		real zz = z * z;
		real sqrt5;

		result[4] = xy * band2 * 0.5f;
		result[5] = band2 * z * y * -0.5f;
		sqrt5 = (real)sqrt(5.0);
		result[6] = sqrt5 * (zz * 3.0f - 1.0f) * inverse_sqrt_pi * 0.25f;
		real quadratic_xz = z * x;
		quadratic_xz *= band2;
		result[7] = quadratic_xz * -0.5f;
		result[8] = xx_yy * sqrt15 * inverse_sqrt_pi * 0.25f;
		if (order > 3)
		{
			real sqrt2 = (real)sqrt(2.0);
			real zz5 = zz * 5.0f;
			real zz5_1 = zz5 - 1.0f;
			real xx_3yy = xx - yy * 3.0f;
			real band3_35 = (real)(sqrt(35.0) * sqrt2);
			real sqrt105 = (real)sqrt(105.0);
			real sqrt7;
			real band3_21;

			band3_21 = (real)(sqrt(21.0) * sqrt2);
			result[11] = zz5_1 * band3_21 * inverse_sqrt_pi * y * -0.125f;
			result[13] = inverse_sqrt_pi * x * zz5_1 * band3_21 * -0.125f;
			result[9] = (xx * 3.0f - yy) * band3_35 * inverse_sqrt_pi * y * -0.125f;
			result[10] = xy * sqrt105 * inverse_sqrt_pi * z * 0.5f;
			sqrt7 = (real)sqrt(7.0);
			result[12] = sqrt7 * (zz5 - 3.0f) * inverse_sqrt_pi * z * 0.25f;
			real cubic_difference = inverse_sqrt_pi * z;
			cubic_difference *= sqrt105;
			cubic_difference *= xx_yy;
			result[14] = cubic_difference * 0.25f;
			result[15] = xx_3yy * band3_35 * inverse_sqrt_pi * x * -0.125f;
		}
	}
}

/* a directional light's colour in the basis of the first order*order bands */
// @retail 0x146020
bool spherical_harmonics_evaluate_directional_light(vector3f const *direction, dword order, real red, real green, real blue,
	real *red_result, real *green_result, real *blue_result)
{
	real coefficients[16];
	real normalization;
	real scale;
	dword count;
	dword i;

	spherical_harmonics_evaluate_direction(direction, order, coefficients);
	normalization = 0.75f;
	if (order > 2)
	{
		normalization = 1.0625f;
	}
	normalization = 3.141593f / normalization;
	count = order * order;

	scale = normalization * red;
	for (i = 0; i < count; i++)
	{
		red_result[i] = coefficients[i] * scale;
	}
	if (green_result)
	{
		scale = normalization * green;
		for (i = 0; i < count; i++)
		{
			green_result[i] = coefficients[i] * scale;
		}
	}
	if (blue_result)
	{
		scale = normalization * blue;
		for (i = 0; i < count; i++)
		{
			blue_result[i] = coefficients[i] * scale;
		}
	}
	return true;
}
