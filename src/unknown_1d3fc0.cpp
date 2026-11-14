#include "unknown_11c920.h"
#include "unknown_1cec30.h"
#include <math.h>
#include "vector_products.h"
// @flags /O2 /Ob1 /arch:SSE /Gr

void function_1d1260(s_havok_component *arg_0);
void __stdcall function_1d01c0(s_havok_component *arg_0);
void __stdcall function_bd020(long arg_0);
void function_1cf120(long arg_0);
void function_1d1540(s_havok_component *arg_0);
transform4x3f *function_ba160(long arg_0, transform4x3f *arg_1);
bool __stdcall function_e0d70(long arg_0, real *arg_1);
void havok_component_rigid_body_matrix_set(long arg_0, s_havok_component *arg_1, transform4x3f const *arg_2);
void function_1d4360(s_havok_component *arg_0, transform4x3f *volatile arg_1);
void function_141590(transform4x3f const *arg_0, transform4x3f *arg_1);
int __fastcall function_142a60(transform4x3f const *arg_0, transform4x3f const *arg_1, transform4x3f *arg_2);

PRIVATE __forceinline vector3f *function_1d3fc1(vector3f *arg_0)
{
 real j_squared = arg_0->j * arg_0->j;
 real local_0 = (real)sqrt(arg_0->i * arg_0->i + j_squared + arg_0->k * arg_0->k);
 if (!(fabs(local_0) < 0.0001f))
 {
  real local_1 = 1.0f / local_0;
  arg_0->i = local_1 * arg_0->i;
  arg_0->j = arg_0->j * local_1;
  arg_0->k = arg_0->k * local_1;
 }
 return arg_0;
}

// MiniMax G (11-14): no new form, no lower score (110): the frame 0x13c and the 922-byte length agree; the axis
// block's dot product still multiplies up's i first where retail multiplies forward's i first, and the shapes that
// flip that need the existing dot3f body written out by hand.
// score 110 (was 110): Frame 0x13c and length 922 agree; first reciprocal/component carriers and
// magnitude/projection schedules remain. Both axis receivers begin after normalization.
// MiniMax B (11-12): no new form, no lower score (110, was 110): the frame 0x13c and the 922-byte length agree; the
// axis block's dot product still multiplies up's i first where retail multiplies forward's i first, and the only
// shapes that flip it need the existing dot3f body written out by hand and a second access path to one field.
// MiniMax R (11-11): no new form, no lower score (110); the axis block's dot product still multiplies up's i
// first where retail multiplies forward's i first, and the only shapes that flip it need the existing dot3f
// body written out by hand and a second access path to one field, so they stay dropped; swapping the dot's
// two arguments (dot3f(up, forward)) compiles to the same code, and the normalizer's square order, the
// reciprocal carriers and the projection schedule are unchanged.
// M3 lane 1 r96 (fast, 0x1c4a80 matched each time): the normalizer's locals and parameter renamed (length, inverse,
// vector) 110 (identical code: names move nothing here); the flag9 branch's real declared in that branch 110; the axes
// addressed through the matrix's rotation member (`&local_1[0].rotation.forward`, up, left) 110; the normalizer's scale
// through the existing vector3f_scale with the reciprocal local 110 (re-measure of the listed scale helper), with
// `1.0f / length` as its argument 147 (936 B); the loop counter declared at function scope 110; `up =` assigned from the
// third normalization's returned pointer 110. The rotation-member addresses re-measure round 222's named rotation member.
// MiniMax R (11-08): no new form, no lower score (110); the shapes that reach 2 need an existing helper's body
// written out by hand, so the axis block still multiplies up's i first where retail multiplies forward's i first.
// MiniMax D (11-06): no new form, no lower score; the helper's scale order k, i, j 115 and the k product with the
// reciprocal first 110 (the same code); the axis block's dot product still multiplies up's i first where retail
// multiplies forward's i first, and no shared helper computes that product order.
// MiniMax H (11-06): no new form, no lower score; the only difference in the axis block is the dot product's
// i term, where ours multiplies up's i first and retail multiplies forward's i first; the two shapes that flip
// it need the dot written out by hand and the k term read through the matrix field, so they stay score-picked
// forms; the normalizer's squared length still has to go through the existing dot3f.
// MiniMax A (11-05, held at m3 review): 110 -> 2 with the normalizer's squared length through the existing
// dot3f and the caller's dot3f(forward, up) written out by hand with its k term read as
// local_1[0].forward.k (the same field forward points to); only the i term then differs (ours copies up.i,
// retail forward.i). Not kept without a match: an existing helper's body written by hand and a second
// access path to one field are score-picked forms; the normalizer change alone 172. Addends reordered or
// pointers declared apart 2; the up side or forward.i from the matrix 145/148; the normalizations swapped
// 71; calls through the pointer variables 94. build/held/mma-1106-1d3fc0-handwritten-dot.patch
// MiniMax B (11-04): a second forceinline normalize twin (k, j, i summed in turn, used for up and left while forward keeps 0x1d3fc1) with the dot product written out 2; NOT kept: a score-picked helper twin (no new inline twins).
// Round 34 (lane 17, fast): forward/up pointers taken first, then both normalized through them 150;
// a const forward pointer 110; named i and j squares (real; retail's forward sum j, i, k but k still
// preloaded) 111; all three squares named 154; the i and j squares summed j, i, k 111. Restored.
// Round 150 lead (best 2, not kept: needs a match): forward's k preload follows how later code reads
// forward.k. One read of the matrix field `local_1[0].forward.k` in the projection's k row (or the dot's k
// term), with the i/j rows through `forward`, gives retail's whole forward block (j, i preloaded, `mulss
// xmm6, [k]`, skip-path k reload, sum j, i, k). On it, one helper for all four with the squared length in
// its own local (`real squared = dot3f(v, v); real length = sqrt(squared);`) gives every sum and the
// projection retail's order: 2 at 922 bytes (plain), left only the dot's i term (retail `forward.i * up.i`,
// ours up first). Not kept: the k row read through the field while i/j use the pointer is a mixed read.
// Real: k-row field read alone 64 (926 bytes); with a k,j,i statement-sum second helper for up/left 11
// (923); the squared-length helper alone 154. Compile-only on the 2-form: all six dot term orders x
// operand orders, statement-sum dots, the k field read in the dot instead, 1-32 unused-local probes before
// forward/up/dot/projection/cross (move only the term order), renamed pointers, the helper's scale order
// and operand order (48), else/`>=`/division/sqrtf/`*=`/vector3f_scale bodies, the cross written out or
// cross3f_staged, the pointer taken after the normalization: the i term stays up-first every time.
// Direct field reads of forward.i/j anywhere in the block (dot, projection or cross) drop their preloads.
// A 600 s permuter run on the 2-form got 8 tries (machine load), nothing better. Real on the 2-form:
// helper `inline` 2, dot3f(up, forward) 2, the field read in the dot's k term instead 2, forward and up in
// one declaration 2, a left pointer for the cross and its normalization 4 (compile-only). A field read
// anywhere in the function (even a dead one in another branch) drops that field's preload; a read of
// another array element's forward.k does nothing.
// Earlier: magnitude order, reciprocal/SSE carriers and projection schedule remain.
// Retail sums the forward magnitude j,i,k (k loaded late, reloaded on the short-vector path) and the
// other three k,j,i; in this /Ob1 file every sum compiles i,j,k whatever the source order (dot3f,
// k*k+j*j+i*i, i*i+j*j+k*k each 155 with the normalize3d shape), only a named square moves it.
// Axis consumer scopes 155/155/155; symmetric dot 155; projection order 155; dot/projection 155;
// early inputs 155; native scaled result 352; component-input inline 155; output-first component
// inline 155; helper-return receivers 153; rotation receiver 155; squared magnitude 155; float square
// root 155; float absolute value 155; reverse stores 155; j/k/i stores 155; complete projection 155;
// component scale loop 155; shared self-dot 155; i/j/k squared values 152/149/154; all squares 155;
// self-dot or all squares with scale loop 155 (keep the j-square intermediate); paired i/j and j/k
// squares 152/153; first-plane subtotal 149; sequential squared length 152; paired squares with
// subtotal 152/149; separate reciprocal division 149; paired squares with scaled components 153;
// normalized i/j/k products 149/149/149; ij/ik/jk products 149/149/149; all products and k/j/i
// product evaluation 149/149; native n-array normalization 149; array receiver 149; scalar-array
// helper 149; array scale loop 149; each array form with indexed projection 149/149/149; indexed
// projection alone 149. Header normalize3d (not inlined under /Ob1: four calls) 453; six transforms
// at function scope instead of the array (same frame and slots, identical code) 149.
// Round 36: separate remaining-axis k-square helper 152; remaining terms reversed 152; k/j square pair 152;
// pair declarations swapped 152; k-first sum 152; paired k/j-first sum 152; grouped j/i 149; named j/i subtotal 149.
// Round 37: named squares come first in component order (j before k whatever their statement order), then the
// unnamed ones i, j, k; an accumulating statement sum keeps its order. A k, j, i statement sum (`m = k*k;
// m += j*j; m += i*i;`) for up and left with the j-square form kept for forward gives retail's square order in
// all four normalizations (152; the rest is forward's registers: retail preloads only j and i, puts 1/m in xmm6
// and multiplies k from memory, `inv * k`, with the skip path reloading k). Plain i, j, k for up and left 152;
// plain everywhere 155; the k, j, i statement sum everywhere 155; j, i, k statement sum for forward 152.
// Round 418 on the j-square graph: complete scalar inputs, magnitude return,
// both, native reference output, reference/scalars, reference/magnitude, all,
// and scalar/reference receiver return 149/149/149/149/149/149/149/149. All restored;
// first reciprocal/component carriers and later magnitude schedules remain.
// Round 19: unchanged; the forward normalisation's reciprocal and component carriers (retail
// preloads j and i, divides 1/m into xmm6 and multiplies k from memory) still differ; the
// operand-order-only forms of the helper give 149.
// Lane 15 round 18: the left axis through the existing cross3f(up, forward, left): identical code (149,
// kept). Retail's forward never preloads k (its scale is `inv * [k]` into the reciprocal's register and
// the short path reloads k); every helper form preloads all three. All four through normalize_inline 155
// (sums i,k,j); file helper for forward with header normalize3d for the rest 354 (normalize3d stays out
// of line, 718 B); normalize_inline for the third normalization 149 (projection schedule moves only);
// for the third and the left 152.
// Round 130: a compile-only screen of all six product statement orders with each operand order (48 forms)
// preloads all three forward components every time (only the register mapping moves); retail's mapping
// (j xmm4, i xmm5, k xmm6) comes from the products in k, i, j order. Real: k, i, j products with
// i/k reciprocal-first 147, all component-first 147; k, j, i 149; j, i, k 148; k, i, j with a second
// helper summing k, j, i for up and left 150. All restored.
// Round 29 (lane 11): the existing normalize3d (header, with a __forceinline redeclaration so it inlines
// under /Ob1) for up and left with the file helper for forward 152, for all four 155, for forward only 152;
// the dot's argument order with the six projection statement orders 149 each (same code: the terms are
// ordered by rank); `*=` for the k product or all three products the same code (compile-only listing).
// Compile-only listing: with nothing after the forward normalization every component is load-multiply-store;
// the preloads (ours k,j,i into xmm4-6, retail j,i) appear only once the dot reads forward afterwards.
// Round 53: unnamed `j*j + i*i + k*k` 155; `j*j + (i*i + k*k)` 155; a named k square summed k, j, i 154;
// the sum cast to double 155; the projection written `up->i = up->i - local_10 * forward->i` 149 (same code).
// Round 456: Direct forward/up, then direct-forward with partial/complete scalar inputs,
// i/j subtotal and post-normalization forward/up/both receivers:
// 157/169/157/157/157/150/160/119. Both receivers retained at 119, 923/922 bytes
// after the plain whole-image check; all other forms restored. Original math and game calls preserved.
// Round 10 (one-file LTCG probe and compile-only screen, both reproduce this loop): one helper always gives
// all four normalizations the same square order (j-square form j,k,i; plain sums reassociate to k,j,i or
// j,k,i; a statement sum keeps its order), so retail's forward j,i,k with the others k,j,i needs two forms.
// The helper as `m = k*k; m += j*j; m += i*i;` gives retail's sums for up, up and left (153 alone: forward
// stays one instruction short, shifting everything after it); a forward-only form summing j,i,k with the
// products written k, i, j gives retail's j/i registers (xmm4/xmm5) and, with the pointer receiver kept,
// retail's cross (148; 150 with the header normalize3d for the other three; 153 with normalize3d for
// forward; an `else m = 0` forward form 148). 144 forward forms (sum form x product statement order x operand
// order): the operand order never matters, the statement order only permutes the preload registers, and
// every form preloads k; retail loads only j and i, scales k as `mulss xmm6, [k]` into the reciprocal's
// register and reloads k on the short-vector path. Division, a temporary vector, the existing scale helper,
// `>=`/`>` tests, a magnitude return and an else all keep the k preload. Forward's shape is the gate.
// Round 33 (lane 14; aligned counts from a difflib alignment): forward through normalize_inline 121 (aligned
// 67); the shared helper summed as `m = k*k; m += j*j; m += i*i;` 153 at 922 bytes (aligned 39: both up
// normalizations and left become retail's, forward stays one instruction short), with forward through
// normalize_inline 153 (aligned 46); summed j, i, k 120 (forward's x87 sum right, up wrong, k still
// preloaded); the left axis addressed directly at its two uses, its pointer local dropped, 110 (kept: the
// left normalization's first product becomes retail's movaps/mulss-from-memory and the length 922); on it
// the k, j, i sum 154 (aligned 38), the cross product on the matrix fields directly 110 (same code),
// compound `*=` products 110 (same code), cross3f_staged 179, dot3f(up, forward) 110 (same code).
// Round 120 (m3 lane 5): forward's block is one instruction short (k preloaded, no skip-path reload),
// which shifts every later line. The helper's k product reciprocal-first 119 (same code); its test as
// `0.0001f > fabs(m)` 119 (same code); `left` declared after forward and up 119 (same code); forward
// alone through the existing normalize_inline 121 (sums i, k, j, still preloads k); the loop's world
// matrix as its own block-scoped transform 174 (frame 0x170). The preload order (k, j, i into
// xmm4-6) follows the dot's term order, not the sum. Original body restored.
// Round 35 (fast): diagnostics: with no k term in the dot and the projection, forward preloads only j
// and i as retail; with k read by either one it preloads k (so forward's k preload follows how the
// dot/projection consume forward.k); retail's dot sums j, k, i and its projection rows run k, j, i (ours
// k, j, i and j, k, i). Projection through the existing vector3f_add_scaled with `-local_10` 156 (929
// bytes); the up pointer taken before forward's normalization, normalized after it, with dot3f(forward,
// up) or dot3f(up, forward) 151/151; a const forward receiver 110 (same code); the second up
// normalization's returned pointer passed straight to cross3f 110 (same code).
// Round 37 (m3 lane 14, fast): unused-local probes (1-16 before forward, 1-8 at the top, 1-8 before the
// dot) 110 each, identical code; the transform array as six separate transforms (either name order) 110
// (identical code, same slots); the projection as a loop over `n[axis]` 110; through a named scaled
// vector 110; pointer locals dropped with the projection on the matrix fields 181 (953 bytes: forward
// then folds all three products), the same on six transforms 181.
// Round 66 (m3 lane 16, fast; 0x1c4a80 matched each time): helper products k, i, j 115 (first
// difference +0x167: forward gets retail's j XMM4 / i XMM5, k still preloaded into XMM6); j, i, k 114;
// k, j, i 152; j, k, i 153; i, k, j 113; on k, i, j the sum spelled j+k+i, k+i+j, i+k+j, j+i+k, k+j+i
// 115 each; k, i, j with an accumulating k, j, i sum 151, with a j, i, k sum 116 (forward's x87 sum is
// then retail's); original products with the j, i, k accumulating sum 111. Lead: with k, i, j products and the
// j, i, k accumulating sum, forward's x87 sum and its j/i registers are retail's; the remaining gate is forward's k
// preload (retail scales k from memory into the reciprocal's register and reloads it on the short path).
// Round 19 (lane 17; compile-only harness reproduces the forward block, x87 j,k,i with k,j,i preloaded):
// 216 forward-only and 96 shared-helper spellings screened; naming the i and j squares gives retail's
// forward x87 order j,i,k, but every form preloads forward.k and multiplies it into k's register
// (retail: reciprocal in xmm6, `mulss xmm6, [k]`, k reloaded on the short path), also with k captured
// before the sqrt, a `>=` test, an early return, `*=` products or the existing vector3f_scale; the
// dot/projection spellings compile the same. Real (fast): shared helper with a named k square (the
// harness's best, giving retail's up/left x87 order) 154; no named square 153; named j and k squares
// 154; named i and k squares 153 (all move the first difference to the +0x44 exit).
// Round 435: up-first split receiver declarations on the returned-receiver/direct-left graph,
// crossed with early uninitialized projection/reciprocal and loop-entry receiver lifetimes,
// eight contexts 110 each. Actual input reads and assignments stay in place; all forms restored.
// Round 279: Complete independent orthogonalized scalar results before stores in all six
// producer row orders 188 each; with all three removed products named 188, existing staged
// cross 208. Scalar frames 0x144, staged cross 0x14c; original helper and full body restored.
// Round 190 (fast; compile-only screen scored against retail's block, control 71 mismatches): forward-only
// helpers (one caller, so only keepable with a match): products k, i, j with k reciprocal-first 108 (retail's
// j/i/k register mapping xmm4/xmm5/xmm6, k still preloaded, sum still j, k, i; screen 63); statement sum j, i, k
// with products j, i, k 108 (screen 55); plain j, i, k sum with products j, i, k 111 (55), with k, i, j 110;
// named i square with k, i, j 110; the k-first forward plus a k, j, i statement sum in the shared helper 149;
// forward normalised in a local copy 251 (982 bytes). Screen only: 144 forms (three sum forms x six product
// orders x every operand spelling) never drop the k preload and the spelling never matters; the dot's argument
// order and all six projection row orders compile the same; distinct in/out pointers, products through
// locals and a magnitude helper compile like the control.
// Round 133 (m3 lane 6, fast with the file's callees listed): forward's k preload goes away when the dot reads its k
// term from the matrix field while i and j come through the pointer (`forward->i * up->i + forward->j * up->j +
// local_1[0].forward.k * up->k`): 64 (926 bytes, aligned 56 -> 25; forward's whole block is then retail's, k formed as
// `inv * [k]` with the short-path reload). On it, a second helper summing `m = k*k; m += j*j; m += i*i;` for up, up and
// left (forward keeps the j-square helper) 2 at 922 bytes; the only difference left is the dot's i term (retail copies
// forward.i and multiplies by up.i, ours copies up.i). A mixed read, so not kept without the match. On the 2 form: the i
// term spelled up-first, the dot in j, k, i order, parenthesised groupings (four), the projection's k row through the
// field: 2 each (same code); the other field/pointer mixes in the dot 72-159; forward helper scale orders j,i,k /
// i,k,j / k,i,j 11/140/139 and k,j,i in the second helper 80; unused-local probes before forward (1-9, 11, 13, 15),
// up (1-4, 6, 8, 10, 12, 14) and the dot (1-7) 2, or 4 at 9/11/13 and 10/12/14 (they swap the dot's k and i terms, never
// the i operands); a permuter run (10 tries in 629 s after its own 1015 s baseline build) nothing below 2. Rule found: a
// direct field mention makes that field's symbol oldest (so up.i read directly flips the i term to retail's order) but,
// placed after that vector's normalisation, also stops its preload: up.i read directly in the projection's i row
// 126 (929 bytes, aligned 5), in the dot 126, in the cross after the second normalisation 120 (aligned 11). Lead: a
// natural form that names up.i before forward.i is created without reading it after up's normalisation.
// Round 291: up evaluated before forward, returned/pre-call receiver masks and symmetric
// dot roles, 112/112/153/153/112/112/153/153. Original helper and body restored.
// Round 19 (lane 17; compile-only harness reproduces the forward block, x87 j,k,i with k,j,i preloaded):
// 216 forward-only and 96 shared-helper spellings screened; naming the i and j squares gives retail's
// forward x87 order j,i,k, but every form preloads forward.k and multiplies it into k's register
// (retail: reciprocal in xmm6, `mulss xmm6, [k]`, k reloaded on the short path), also with k captured
// before the sqrt, a `>=` test, an early return, `*=` products or the existing vector3f_scale; the
// dot/projection spellings compile the same. Real (fast): shared helper with a named k square (the
// harness's best, giving retail's up/left x87 order) 154; no named square 153; named j and k squares
// 154; named i and k squares 153 (all move the first difference to the +0x44 exit).
// Round 279: Complete independent orthogonalized scalar results before stores in all six
// producer row orders 188 each; with all three removed products named 188, existing staged
// cross 208. Scalar frames 0x144, staged cross 0x14c; original helper and full body restored.
// Round 264: Common completed normalization outputs in ijk/ikj/jik/jki/kij/kji store orders 151/151/151/151/150/150; existing setter 151; early short result 110. Original four-use helper and complete body restored; forward late-k/reciprocal ownership and later magnitude/projection schedules remain.
// Round 291: up evaluated before forward, returned/pre-call receiver masks and symmetric
// dot roles, 112/112/153/153/112/112/153/153. Original helper and body restored.
// Round 222: named rotation member 110; complete matrix receiver in loop/branch/entry
// 149/149/149; direct forward/up/both consumers 160/169/181; native helper component
// array on the named-member graph 110. Original private helper and complete body restored.
// Round 318: Up-before-forward evaluation with complete squared-dot magnitude, native shared
// scale, early short return or j-square magnitude staging, each with direct k in
// dot/projection: 71/71/71/71/71/71/124/124.
// All mixed observations are exact-only and none matches; original four-use helper and complete
// body restored.
// Round 256: plain native-array dot reduction, long/dword/short/byte counters,
// original projection 110/110/221/221; indexed projection 110/110/221/221.
// Original shared dot call, private normalizer and complete body restored.
// DH282: Complete native forward/up/both values after normalization and before normalization with matrix
// publication, then paired forms with symmetric dot roles [228, 278, 333, 255, 253, 336, 333, 336].
// Original executable body restored.
// Round 73 (m3 lane 16; 110 stands): the 2-form reconfirmed real (helper with the squared length named, k row through the
// matrix field; 922 bytes, only the dot's i term operands differ). Real on it: k read into a named local after the dot 2;
// the scaled values as three named locals 2; left as `forward + 1` at the cross and its normalization 11 (923); up.i through
// the field at the projection row with forward.k through the field in the dot 126 (929); up.i through the field in the dot
// 126 (929). Compile-only, which reproduces the 2-form exactly: pointer views of up.i or forward.i at the dot and the
// projection (dead, top of block, at use) 2; named i product 6, staged 2; named forward.i/up.i leaves in the dot, either or
// both 2; address spellings in the dot, cross and third normalization 2; a matrix pointer declared first 128; `up` declared
// first 72, both pointers first 78; `forward + 2` for up 72; an axis pointer 78; a named k before the dot 118; a separate up
// normalizer (24 sum and scale orders) 2 or worse; a forward normalizer with named values (288 product, store and operand
// orders) and named leaves (24) never flips the i term; 800 unused-local probe combinations at five sites and 40 extra-read
// diagnostics never flip it (reads recolour only); single and paired direct field reads across 24 uses, with the dot and
// the cross written out (301 forms), none at 6 or below; a randomized search of helper counts, dot, projection and cross
// spellings, stopped after about 14 minutes, nothing below 2. Direct up.i flips the i term (102 compile-only, aligned 7) but
// drops up.i's preload and flips the cross's `up.i * forward.k`.
// Round 318: Complete normalizer scalar input capture in ijk/ikj/jik/jki/kij/kji order, native
// vector input and native input with squared magnitude; 110/110/110/110/110/110/342/342. Original
// whole body and helpers restored. Extra input/output helper contracts 110/110/110, excluded.
// M329: Complete projection products before original stores, ijk/ikj/jik/jki/kij/kji
// producer orders: 110/110/110/110/110/110; each product named at its own row producer
// 110; existing setter publication 188 (937 bytes). Original product expressions, stores,
// four-use normalizer and complete body restored; forward late-k/reciprocal issue remains.
// Round 38 (m3 lane 18, fast): dot3f over the address expressions `&local_1[0].forward/up` 110; the whole
// orthonormalisation as a forceinline helper over a transform pointer 149; the dot written out j, i, k 110; the
// helper's sum written `j_squared + i*i + k*k` 110; the axis pointers taken after both normalisations 110.
// Round 139 (m3 lane 6, fast, 0x1c4a80 stays matched): the projection through the existing vector3f_add_scaled
// with the negated dot 156 (929 B); the products scalar first 110; `up->x = up->x - ...` 110 (same code); the
// header normalize_inline for up and left 151; a left pointer for the cross and its normalization 119 (923 B).
// Round 121 (fast; on the 2 form, aligned count in brackets): dot3f through the matrix fields 152 (39), with the projection reading
// forward through the fields 157 (22), up 163 (107), both 177 (108); the 2 form with the projection reading forward through the fields
// 157 (12), up 147 (100), both 177 (98); the dot's i term as `up->i * local_1[0].forward.i` 148 (12): the i term stays up-first in all.
// Round 65 (m3 lane 11; fast, on the 2 form): `up_i` local 2; dot's i term through the matrix field 126 at 929 (aligned 5: the
// term flips but up.i loses its preload, `movaps xmm3, xmm0; mulss xmm3, [i]`, a jmp and a reload); cross3f_staged 181
// at 936. Compile-only, i term or registers unchanged or worse (96-130 aligned, base 96): address of up.i as the helper's argument,
// `*(real *)up` or `*(real *)forward` in the dot, a pointer or reference to up.i declared before forward, pointer reads
// of up.i before the normalizations, partial sums named, a rotation sub-record pointer for forward, up or a term (102-118; real, for both vectors, 76 at 922).
// A dead direct read of up.i anywhere flips the term and drops the preload the same way. No matched function has this
// dot encoding (scan of retail for `movaps xmm7, xmm5; mulss xmm7, xmm3; addss xmm0, xmm7`).
// Round 57 (m3 lane 13, fast): the normalizer's products as i*inv, j*inv, inv*k 110 (same code); `transform4x3f
// local_1[6]` declared before `local_0` 110; the normalizer returning the length, both pointers taken after the two
// calls 110; the header normalize3d for forward only 233 (839 bytes); the projection rows in k, j, i order 110.
// J249: Complete normalizer vector results, pointer/reference/value inputs with published
// matrix axes 501/501/501, or local axis values 541/541/541; in-place value results
// 110/349. NOT kept: no match or gain; aggregate ownership expands frame/length except
// the 110 tie. Original helper/body restored; late forward-k and reciprocal carriers remain.
// DL256: Complete normalized forward/up/left field-reference consumers, subsets 1/2/4/3/5/6/7,
// then mutable forward on all three: 110/110/119/110/119/119/119/119. Left-reference forms
// grow to 923/922 bytes and move the first gap to +0x44; other forms retain +0x159.
// No control score changes. Original four-use normalizer and entire body restored.
// P305: Existing shared first-up/post-projection-up/left normalization subsets
// 156/110/152/156/153/152/153; all with readonly forward 153. Original helper,
// includes and complete body restored; reciprocal/late-K ownership and schedules remain.
// M352: Four-use normalizer input-reference/returned-reference/both contracts, each
// with original/short-vector exit, then input/both references with float absolute API:
// 110/110/110/110/110/110/110/110. All canonicalize; original helper and complete body restored.
// P327: Native-array squared-length reduction, long/dword/short/byte index, original
// scale 221/221/311/311; indexed scale 221/221/391/391. NOT kept: all regress
// and grow beyond 922 bytes; original four-use helper and complete body restored.
// DL262: Four-use normalizer actual scaling-status bool/byte/short/long results, result-first
// then exact short-vector return with post-call matrix receivers: 110/110/110/110/110/110/110/110.
// NOT kept: no byte equality; original normalizer, receivers and complete body restored.
// M3 lane 10 r71 (fast, 0x1c4a80 stays matched; aligned in brackets, base 110 [56]): the dot written as accumulating statements j,k,i / k,j,i / j,i,k 110 [55]/[56]/[52]; both axis
// pointers taken after both normalizations 110 [56]; `up` declared before `forward` and assigned after 110 [56]; both pointers declared at function scope 110 [56]. One-file
// harness on the 2 form (axis block scored, exact to the plain listing; 2 s a form), all still only the dot's i term: chains of 1-16 identity pointer locals at the function top, in the
// else arm, between forward and up, before the dot and inside the normalizer (the i and k terms swap at 9-14, never the i operands); both pointers declared up first, at function
// scope, or taken after the normalizations; a private dot with chains on either or both inputs (0-8); six named transforms in either order; the working matrix at 1-8 dwords into a
// struct, which rules out field offset modulo 8 as the rank (retail's three operand pairs do fit it); named i inputs, either order; `(&matrix.forward)->k` and `((real *)&matrix.forward)[2]`
// for the k row 2 (both count as a direct read). Taking the axis address through a local before the normalizer (ahead of forward or up) loses retail's sums 25-92.
// Round 166 (m3 lane 4, one-file harness, axis block scored, 2 on the 2-form; none kept, body unchanged): new forms on the 2-form:
// the whole block as a forced inline helper taking forward, up and left in all six orders 105 each, taking the matrix 130,
// matrix plus forward and up either order 104; a dot helper with the i term written either way 2; `up` through a reference 2; the
// normalizer's result not used for the pointer 64/68/89; projection rows in all six orders with `f * d`, `d * f` and `x = x - f * d`
// 2 each; cross3f_staged 120; a left pointer for the cross and its normalization 4. The dot's i term stays up-first.
// Declaration placement on the 2-form: the dot's local first in the block or between the pointers, `up` declared ahead of forward and assigned
// after, all three declared first, the dot in its own scope, the pointers or the dot's local at function or loop scope: 2 each.
// Round 59 (m3 lane 15, fast, base 110): a named `transform4x3f *matrix = &local_1[0]` for the combine output, the block and the set call 149 (first
// difference moves to the +0x44 exit), the same through a reference 149; the flag9 branch on its own block-scoped transform 195 (938 B, frame
// grows); dot3f(up, forward) with the rows in retail's k, j, i store order 110 (same code); the axis addresses spelled `&local_1->forward` 110;
// the cross into a local `left` normalized then stored 202 (946 B); up normalized inside the dot's argument 151.
// AC554: Existing pointer-return normalizer short-vector guard truth in bool/byte/char/
// signed-char/short/word/int/long 110/110/110/110/110/110/110/110. NOT kept: no exact
// guard-producer repair; original four-use helper and entire body restored.
// DL267: Four-use normalizer double magnitude, reciprocal, both, complete squared-length, squared plus magnitude/reciprocal/both, then double j-square: 266/266/281/239/266/266/281/239.
// NOT kept: no byte match or eligible native repair; original executable source restored.
// DL272: Shared planar dot at projection/magnitude boundaries, native/late-k/squared-length views, then zero/first-product standard numeric reductions with symmetric input roles; late-k mixed views reach 2 but remain unmatched. Scores 110/110/110/64/2/2/111/111/195/195/203/203.
// DH382: Existing four-use numeric magnitude return, successful/failure-first/common-
// magnitude/result-first completions with post-normalization receivers 110/110/110/110;
// early actual receivers 150/150/150/150. NOT kept: no late forward-k/reciprocal repair;
// original pointer-return helper, all calls and complete body restored.
// Round 141 (m3 lane 7; one-file LTCG harness with 0x1c4a80 and the callee files, scored on the axis block, which it reproduces
// exactly: 38 here, 2 for the 2 form): dot3f with each input as the pointer or the matrix field, either order, either normalizer
// 28/38 (same code); 96 written-out dots around the 2 form (i term through the pointer or the matrix on either side, six term
// orders, i operand order, `a + (b + c)`) best 2; the normalizer's scale all scale-first or all value-first 2 (no change); rows
// through vector3f_add_scaled 37/44, rows k, j, i 2/28, cross3f_staged 82/100; the existing normalize3d 80-136. Fast on the current body: rows through
// vector3f_add_scaled(up, forward, -dot, up) 156 (929 B), with `0 - dot` 156, with dot3f(&up field, &forward field) 156; the normalizer's
// scale all value-first 110, all scale-first 110 (same code).
// M3 lane 14 r74 (fast): round 150's 2-form rebuilt (the normalizer's squared length through dot3f, the projection's k row read from the matrix
// field) 2 at 922 bytes, 0x1c4a80 still matched; left: the dot's i product copies up.i (retail copies forward.i). On it, all still 2 with the same
// two lines: 1, 2, 3, 4, 6 and 8 unused initialized long locals at the function top, at the top of the axis block, and just before the dot (17 builds:
// plain dead locals move nothing, not even the term order); the projection rows written k, j, i. Worse: a `transform4x3f &` name for the working
// matrix 155; the dot, or the cross, taking `&local_1[0].forward` in place of the k row's field read (either dot argument order) 154 each (forward
// preloads k again). Body unchanged at 110: the 2-form needs the mixed field read, which is kept only with a match.
// Round 78 (m3 lane 13, fast; register-masked aligned 23 at 922 bytes): mixes of the two existing header normalizers in
// place of the private helper (normalize3d through a __forceinline redeclaration, axis pointers taken after the first two
// normalizations): forward normalize_inline, the rest normalize3d 154 (925 bytes, masked 39); forward normalize3d, the rest
// normalize_inline 148 (926, masked 20; forward's sum and reciprocal become retail's, the reciprocal then copied); that with
// dot3f(up, forward) 148; forward and left normalize3d, both up normalizations normalize_inline 147 (925, masked 18, the
// lowest yet without a mixed read); the private helper with normalize3d for left only 152. All move the first difference
// to the +0x44 exit through the length gap.
// Round 148 (m3 lane 8, fast with 0x1c4a80, matched each time): the projection rows as `+= forward->x * -local_10` 110; `up` as a
// reference bound to the helper's result 110; the helper declared `inline` 110; the dot's local declared ahead of the axis
// pointers 110; both axis pointers `vector3f *const` 110 (perhaps round 34's const forward pointer again); the dot as a
// `real const` local 110 (all the same code). A compile-only harness of this file (/O2 /Ob1
// /arch:SSE /Gr, no /GL) reproduces the 2-form's x87/SSE sequence exactly (2, the dot's i term only) and the current body at 23
// such instructions; on it the helper's squared length through the existing length_sq3f gives the same code as through dot3f.
// M3 lane 16 r100 (fast, 0x1c4a80 stays matched): the second up normalization folded into the cross argument,
// `cross3f(function_1d3fc1(up), forward, ..)`, 110 (same code); the header normalize_inline for all four normalizations 155 (first
// difference +0x44); the private normalizer's sum as `j*j + (i*i + k*k)` 152, through dot3f(arg_0, arg_0) 153 (the 2-form's
// normalizer alone; aligned 32 against 38, forward then preloads k first), through the existing length_sq3f with dot3f(up, forward)
// 153; normalize_inline for forward only (the private helper for up and left) 112 (first difference +0x153). Body unchanged.
// M3 lane 16 r102 (fast, 0x1c4a80 stays matched): the 2-form rebuilt 2 (masked 0: only the dot's i term operands); on it the squared
// length through the existing length_sq3f 2, `vector3f *const` pointers 2, a `real const` dot 2 (same code each); the shared
// normalizer's scale stores ordered k, j, i 102, j, i, k 28, i, k, j 140 (925 B). Body unchanged at 110.
// M3 lane 12 r111 (fast, 0x1c4a80 matched each time): in the normalizer, i and j read into locals first (k read at its uses)
// 110 (forward still preloads k, j, i); only k read into a local 110; the squared length named as `i*i + j_squared + k*k` 110;
// a block-scoped `transform4x3f *matrix = &local_1[0]` for the forward, up and left addresses 149 (first difference +0x44);
// dot3f(&local_1[0].forward, up) with only the forward side spelled as the address 110. Body restored.
// M3 lane 8 r152 (fast, 0x1c4a80 stays matched; masked aligned 4: forward's k preload and left's k-first sum only): a scan of
// every retail function for the reciprocal-reuse idiom (`divss xmmA`, two `mulss xmmB, xmmA`, then `mulss xmmA, [mem]`) finds
// only this function and the unmatched 0x170630, so no matched body shows the form. A compile-only screen of 300 natural
// combinations (seven sum shapes, four scale spellings, two tests, both dot argument orders, four projection spellings, cross3f
// or cross3f_staged, helper-return or pointers-first) never drops forward's k preload. Real: the helper's squared length as an
// i, j, k statement sum 112; a k, j, i statement sum with both pointers taken first 156; products k, i, j with a plain
// `i*i + j*j + k*k` sum 150, with the squared length named through dot3f 151, with an i, j, k statement sum 117. Re-measured
// (compile-only identical to listed forms): the j, i, k statement sum 111, the k, j, i statement sum everywhere 154, the squared
// length named through dot3f 154. Body unchanged.
// M3 lane 15 r75 (fast, 0x1c4a80 and 0x30bf0 stay matched): the existing out-of-line normalizer function_30bf0 (its definition
// already `inline`) stays a call in this /Ob1 file, also with a `__forceinline` redeclaration here: all four normalizations 449
// (624 bytes), forward only 236 (836 bytes, plain 236), the other three 350 (722 bytes). Body unchanged. I found no other
// untried natural normalizer, dot, projection or cross form; the forms above cover the rest.
// m3 lane 3 r233 (fast, 0x1c4a80 matched each time; control 110, aligned 38, register-masked 21). Forms: forward through the
// existing header normalize3d (a __forceinline redeclaration so it expands under /Ob1) with the private helper's sum written
// plain `i*i + j*j + k*k` for the other three 153 (925 B, masked 21: up, up and left then sum k, j, i as retail, forward sums
// i, j, k and preloads all three); forward through the existing normalize_inline with that plain helper 153 (masked 22);
// normalize3d for forward and left, the plain helper for both up normalizations 154 (masked 20); normalize3d for forward with
// the plain helper's squared length named through dot3f 154 (masked 20); normalize3d run on the matrix field
// (`normalize3d(&local_1[0].forward);`) and the forward pointer taken after it, plain helper for the rest, 148 at 926 B,
// aligned 30, masked 12 (forward then preloads only two components and scales one from memory into the reciprocal's register
// with a reload on the short path, retail's shape, but the one from memory is i, first and through a copied reciprocal, where
// retail's is k, last, and forward sums k, j, i for retail's j, i, k); the same with normalize_inline on the field 152 (masked
// 14); with the private helper unchanged 150 (929 B, masked 23). The 148 form reproduces round 78's listed "forward
// normalize3d, the rest normalize_inline 148 (926 bytes, ...the reciprocal then copied)" (same score, length and shape; the
// masked counts come from a different script). Still open: a forward normalizer that sums j, i, k and scales k last into the
// reciprocal's register. Body unchanged.
// M3 lane 3 r238 (fast, 0x1c4a80 matched): round 150's 2 form re-measured 2 (922 B; the dot's i product copies up.i where
// retail copies forward.i). No untried natural form found. Body unchanged at 110.
// Round 194 (m3 lane 4, fast, 0x1c4a80 matched): the projection through the existing helpers with a named offset vector,
// `vector3f_scale(forward, -local_10, &offset); vector3f_add(up, &offset, up);` 156 (929 B), with `0.0f - local_10` 156;
// `vector3f_scale(forward, local_10, &offset)` then `up->x -= offset.x` rows 110 (same code); each with the normalizer's
// squared length through dot3f 155 (925 B) and 154 (922 B). Body unchanged.
// @retail 0x1d3fc0
void __stdcall function_1d3fc0(long arg_0, s_havok_component *arg_1)
{
 real local_0;
 transform4x3f local_1[6];
 if (TEST_FIELD_BIT(arg_1->flag10))
 {
  bool local_7 = TEST_FIELD_BIT(arg_1->flag5);
  if (local_7)
   function_1d1260(arg_1);
  function_1d01c0(arg_1);
  function_bd020(arg_1->object_index);
  function_1cf120(arg_0);
  if (local_7)
   function_1d1540(arg_1);
 }
 else if (TEST_FIELD_BIT(arg_1->flag9))
 {
  function_ba160(arg_1->object_index, &local_1[0]);
  byte *local_8 = (byte *)g_4e0300->data;
  if (((1 << local_8[(arg_1->object_index & 0xffff) * 12 + 3]) & 1) &&
      function_e0d70(arg_1->object_index, &local_0))
   local_1[0].position.z -= local_0;
  havok_component_rigid_body_matrix_set(0, arg_1, &local_1[0]);
 }
 else
 {
  function_1d4360(arg_1, &local_1[4]);
  function_ba160(arg_1->object_index, &local_1[2]);
  function_141590(&local_1[4], &local_1[1]);
  function_142a60(&local_1[2], &local_1[1], &local_1[5]);
  for (long local_9 = 0; local_9 < arg_1->rigid_bodies.size; ++local_9)
  {
   arg_1->rigid_body_matrix_get(local_9, &local_1[3]);
   function_142a60(&local_1[5], &local_1[3], &local_1[0]);
   if (arg_1->unknown04 & 0x10000)
   {
    vector3f *forward = function_1d3fc1(&local_1[0].forward);
    vector3f *up = function_1d3fc1(&local_1[0].up);
    real local_10 = dot3f(forward, up);
    up->i -= forward->i * local_10;
    up->j -= forward->j * local_10;
    up->k -= forward->k * local_10;
    function_1d3fc1(up);
    cross3f(up, forward, &local_1[0].left);
    function_1d3fc1(&local_1[0].left);
   }
   havok_component_rigid_body_matrix_set(local_9, arg_1, &local_1[0]);
  }
  arg_1->unknown04 |= 0x10000;
 }
}
