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

// score 110 (was 110): Frame 0x13c and length 922 agree; first reciprocal/component carriers and
// magnitude/projection schedules remain. Both axis receivers begin after normalization.
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
