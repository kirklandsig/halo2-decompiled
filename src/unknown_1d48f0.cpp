#include "unknown_11c920.h"
#include "unknown_1d5120.h"
#include "object_queries.h"
#include "unknown_11cc90.h"
#include <float.h>
#include <math.h>
// @flags /O2 /arch:SSE /Gr

extern hkWorld *g_51e9a4;
struct s_vehicle_ray;
bool __stdcall function_168f40(long arg_0, s_vehicle_ray const *arg_1, long arg_2, long arg_3);
bool function_182800(s_vehicle_ray *arg_0, long arg_1, void *arg_2);
void function_1d43d0(s_havok_component *arg_0, transform4x3f const *arg_1, transform4x3f *arg_2);
void function_11bed0(s_location *arg_0, point3f const *arg_1);

class c_contact_shape_view
{
public:
 virtual void slot0() = 0;
 virtual void slot1() = 0;
 virtual void slot2() = 0;
 virtual void slot3() = 0;
 virtual void slot4() = 0;
 virtual long type() = 0;
 long unknown04;
 long metadata;
};
struct s_1d48f0
{
 point3f field_0;
 vector3f field_c;
 real field_18;
 vector3f field_1c;
};
const dword g_455978[81] = {
 0x0, 0x0, 0x0, 0x3f800000, 0x0, 0x0, 0x0, 0x0, 0x3f800000,
 0x3f3504f3, 0x0, 0x3f3504f3, 0x3f13cd3a, 0x3f13cd3a, 0x3f13cd3a, 0x3f13cd3a, 0xbf13cd3a, 0x3f13cd3a,
 0x3f3504f3, 0x3f3504f3, 0x0, 0x3f3504f3, 0xbf3504f3, 0x0, 0x0, 0x3f3504f3, 0x3f3504f3,
 0x0, 0xbf3504f3, 0x3f3504f3, 0xbf800000, 0x0, 0x0, 0x0, 0x3f800000, 0x0,
 0x0, 0xbf800000, 0x0, 0xbf3504f3, 0xbf3504f3, 0x0, 0xbf3504f3, 0x3f3504f3, 0x0,
 0xbf3504f3, 0x0, 0x3f3504f3, 0xbf13cd3a, 0xbf13cd3a, 0x3f13cd3a, 0xbf13cd3a, 0x3f13cd3a, 0x3f13cd3a,
 0x0, 0x0, 0xbf800000, 0x3f3504f3, 0x0, 0xbf3504f3, 0xbf3504f3, 0x0, 0xbf3504f3,
 0x0, 0x3f3504f3, 0xbf3504f3, 0x0, 0xbf3504f3, 0xbf3504f3, 0x3f13cd3a, 0x3f13cd3a, 0xbf13cd3a,
 0x3f13cd3a, 0xbf13cd3a, 0xbf13cd3a, 0xbf13cd3a, 0xbf13cd3a, 0xbf13cd3a, 0xbf13cd3a, 0x3f13cd3a, 0xbf13cd3a
};
// score 298 (was 298): aligned frame 0x1d4 agrees; optional bounds/exclusion homes and selective helper expansion remain.
// Earlier: aligned frame 0x1d4 agrees; first +0x21 is bounds/exclusion homes.
// Round 87: the sample end as `point3f p = *arg_11;` plus in-place adds (retail's three dword copies and
// `movss x, [home]; addss x, [vector]`) gives retail's homes for the bounds/exclusion locals and the loop
// spills, 341 (2106 bytes; the entry esi/edi roles swap back while 0x11bed0 stays inlined). With that form
// and a noinline 0x11bed0 (diagnostic) the entry matches through +0x54 and aligned falls to 80/96 (from
// 129/130), but 0x14a280's bsp moves to the stack (0x14a280 14, 0x11bed0 19). Also tried: renamed
// bounds/exclusion locals 306 (names do not move them); upper bound set first 323; vector-first sums
// 341; existing vector3f_add on the copy 341; 0x11bed0 through 0x11be90 (still matched, still inlined).
// Earlier: the sample count matches (retail's
// setne/dec/and/sub is `27 - (flag ? 0 : 9)`, which keeps the flag on the stack). Structural cause of
// the register differences: our LTCG inlines 0x11bed0 here (retail calls it, esi = location), and the
// aligned frame 0x1d4 and incoming registers agree; small stack homes
// and selective location/range expansion remain. Count is `27 - (flag ? 0 : 9)`. Earlier graph:
// The register differences came from our LTCG inlining 0x11bed0 here (retail calls it, esi = location), and the
// inlined copy takes esi, which swaps the optional inputs (esi/edi) and evicts the table cursor from esi
// in the loop; retail also inlines 0x279740's range check at the first release only, ours calls it twice.
// A separate noinline declaration of 0x11bed0 in its own file keeps the call (entry registers then match
// retail through +0x54, 337; with the first release's range check written inline 291 at 2064 bytes), but
// it moves 0x14a280's bsp parameter from ecx to the stack and loses its 18 matched callers (0x21a100,
// which also inlines 0x11bed0 in our build, then matches). Round 83: description shape stored before its
// transform 317 kept; then (re-tested on this graph) length terms swapped 316 and translation as three
// statements 339; success flag declared first 407; upper bound set before lower 334; filter stored
// first 317.
// Earlier forms: optional-input registers, scalar homes and selective helper expansion: retail calls
// 0x11bed0 out of line and inlines the cache-range check of the first reference release (the form of
// 0x1d01c0), while ours does the opposite.
// Aligned frame 0x1d4 agrees; the sample count now matches (retail's
// setne/dec/and/sub is `27 - (flag ? 0 : 9)`, which keeps the flag on the stack); optional input
// registers (esi/edi swapped), scalar homes and selective helper expansion remain.
// Round 8 (lane 10): noinline redeclaration of 0x11bed0 here 321 (ignored); a separate noinline
// declaration in 0x11bed0's own file 337 (called as retail, but 0x11bed0 and its caller 0x168f40 stop
// matching); bounds coordinate as a named local 360; table direction pointer for both samples 321;
// excluded index declared first, its test braced 323.
// Earlier: Aligned 0x1d4 frame agrees; optional-input registers, scalar homes and selective helper expansion remain.
// Earlier 350 (was 350): Aligned 0x1d4 frame agrees; optional-input registers, scalar homes and selective helper expansion remain.
// Frame 0x1d4 agrees; optional input registers, scalar homes and selective location/release expansion remain.
// Earlier: Aligned frame 0x1d4 agrees; optional-input registers, scalar homes and helper expansion remain.
// frame 0x1d4 agrees; optional-input registers, scalar homes and inline boundaries differ.
// aligned frame 0x1d4 agrees; optional input/register roles (arg_9/arg_10 in
// esi/edi swapped), scalar homes and selective helper expansion remain: ours inlines 0x11bed0 (retail
// calls it at all its sites) and calls 0x279740 at both releases (retail inlines the first).
// aligned frame 0x1d4 agrees; optional-input registers (ESI/EDI swapped), scalar homes
// and selective helper expansion remain: retail calls 0x11bed0 out of line and inlines the cache-range
// check of the first reference release (the form of 0x1d01c0), while ours does the opposite.
// Native/byte success 427/382; path-local excluded index 449; complete volume arms 487;
// both path lifetimes 485; count just before scan 366; read-only body 370; native transform copies 370.
// the presence callback at its retail 12-byte size, initialised by its constructor.
// Frame 0x1d4 agrees; optional input/scalar homes and helper expansion differ. Tried early
// bounds filter 430; early transform filter 361; both early filters 423; reset at query 361; direct ray rejection
// 393; per query callback 376; callback and direct ray 376; early filters and reset 423.
// Native callback context: conditional count 389; earlier conditional 495; earlier arithmetic count 354 kept (plain).
// Lane 6 r85: ray endpoint and translation copy through `real_point3d_set` kept (317 -> 308: entry esi/edi
// roles and the movss translation copy now retail's); planar length through `magnitude2d` in place of the
// volatile casts kept (306). Translation alone 339; sample end through `real_point3d_set` 424, with the
// kept forms 308; direction locals renamed to flip their name order 308; `normalize3d` with k cleared 323
// (the short-vector exit is no longer threaded straight to the loop end); shape pointer declared before
// the body 306; radius scale and unit scale through `vector3f_scale` 306/308; scale factor first 306.
// With 0x11bed0 built /Ob1 (diagnostic) it is called as in retail but 0x14a280's bsp moves to the stack;
// with 0x279740 inlinable (diagnostic) 0x11bed0 is called here but both releases inline.
// Exact sample end 361; transform slot names 361; native ray endpoint 394; expanded radius name 361; unsigned sample ordinal 361.
// Candidate/endpoint/ray component loops 382/354/354; basis loop 524;
// translation loop 350 kept (plain); radial scaling/blend loops 354/354; combined producers 492.
// Direction/radial/both/endpoint/ray/all spatial declaration scopes 350 each;
// complete native endpoint value 403; paired bounds-axis producer 466. All restored.
// Complete candidate/blend/both product phases 350/350/350; both radial square terms 391;
// radial terms with candidate/all phases 391/391; six-use bounds origin 492, with candidate products 492. All restored.
// Complete lower/upper/both bound selections 413/466/484; complete horizontal square 350;
// bounds plus square 484; callback ending before cleanup 350; bounds/callback 484; square/callback 350. All restored.
// Round 187: bounds short/byte, sample long, translation long/short all 548 (2011 bytes);
// the four corresponding short-sample forms all 573 (1986 bytes). All eight ordinal-width forms restored.
// Sample-element reference 350; object-index field reference 357; both references 357;
// world snapshot alone/with sample 484/484; with object field/with both references 483/483;
// body/shape declaration homes at entry 350. All restored.
// Transform description after body/shape load/shape choice/pose copy 351/351/351/411;
// bounds description ends before add/exclusion 406/363; full shape selection 350; word success 415.
// Round 214: single native vector table, separate/reused radial sample and direct/shared ray difference
// 350/350/350/350; real-row table with the same producer grid 447/447/362/362. All restored.
// Noinline redeclaration of 0x11bed0 in this file 350 (ignored; still inlined); sample count as a
// 27/18 ternary 477 (retail's form, but the flag then goes in a register and caller 0xc5460 changes);
// ray endpoint adds as `x = x + i` 350; translation copy as three statements 354; upper bound as
// radius-plus-coordinate 350.
// Round 54: noinline declaration of 0x11bed0 447 (and 0x11bed0 itself 19 in --fast: CAVEAT on its
// callee 0x14a280); first release with the inline range check 442; both 401 (structural diff 36 vs 66);
// with an `arg_8 ? 27 : 18` count 427, count alone 477.
// Round 223: Both positive validation stages 350; result-terminated scan 395; optional failure
// first 351; both 395; zero rejection removed on these graphs 408/411/407/411. All restored.
// Round 148: native candidate value 486; native ray-offset value 467; both vector values 516;
// coordinated transform inline 350; inline/candidate 486; inline/offset 467; inline/both values
// 516; force-inline/both values 954. All executable forms restored.
// Lane 5 r77: `arg_8 ? 27 : 18` gives neg/sbb and makes the flag a register argument (477, caller
// 0xc5460 251 -> 255); `18`, then `27` if the flag 399; `27 - (arg_8 ? 0 : 9)` kept (350 -> 321).
// Then: volatile casts dropped from the radial length 388 (before the count change); length terms
// swapped 320; component index as a ternary 427; k cleared before the length, with or without its
// term 321/321; named lower/upper pointers in the bounds loop 321.
// Shared radial producers on the accepted count graph: in-place radius scale 321; complete blend setter 321.
// both shared producers 321; blend factor before normalization 331.
// factor with radius scale 331; factor with blend setter 331.
// all three producers 331; complete normalized-vector setter 321.
// All executable forms restored.
// Round 222: scan-lifetime transform description, original/late construction with set/native
// copies 320/320/330/330; description-owned pose on the same four graphs 382/382/391/391.
// Original description scope, standalone pose and copies restored; scalar homes and selective expansion remain.
// Current query/radial/all scalar identities 306; shared planar normalization and its
// three identity combinations 307; presence object identity 306. Coordinated range-helper
// inline 419, plus shared planar normalization 416; both lose matched controls and are
// restored. Original executable body and every external declaration retained.
// CRT input samples/ray origin/both 306/306/306; native body pose 374, with samples/origin/both
// 374/374/374; all inputs plus query pose 422. All restored.
// Shared setters for all three native basis rows, with early/late scale publication, entry/local
// input-output matrix declarations and native/library point publication: all eight 306. All restored.
// Round 161: Transform opaque/reference ownership, then optional opaque with
// derived/opaque/reference transform, then optional reference with derived/opaque/reference transform:
// 306/306/306/306/306/306/306/306. Original persistent pointer types and all casts restored.
// Round 206: Optional bounds/exclusion initialization after control-word setup [315, 315, 315];
// after SSE setup [315, 315, 315]; early presence construction alone/with late optional state [467, 458]. All restored.
// Round 228: optional-state/descriptor/receiver storage identity subsets 306 each;
// all three plus immutable body binding 306. First +0x21 optional-state homes remain; all restored.
// Round 232: shared affine candidate alone/radius/blend/both 306 each;
// with planar normalization on the same four graphs 307/307/307/304.
// Two-point gain rejected; original producers and all helper contracts restored.
// Round 214: Endpoint home alone/with sample/with radial/with both direction names 306 each;
// immutable named count alone/with endpoint/with all three/with full ray-query name 306 each.
// All restored; optional-state, sample/ray homes and selective helper expansion remain.
// Round 163: shared endpoint sum into point/native copy/float-set copy/vector
// 306/341/314/306; each with shared optional-ray difference 306/341/314/306.
// Native copies lengthen the body; optional-state homes and selective
// location/range expansion remain. All endpoint producers and helpers restored.
// Round 387: Optional-state homes and selective location/range expansion remain. Actual in-place
// transform with input/output native home, aggregate/CRT point publication and complete basis
// setter combinations: eight forms 352 each at 2070/2083 bytes. Original separate input/output
// matrices and every existing producer restored.
// Round 389: native missing-location result bool/byte/short/long 477/477/487/486;
// each with entry constant initialization 477/477/487/486. Original positive result and
// lifetime restored; aligned frame agrees, optional-state homes and helper expansion remain.
// Round 392: selected-cluster outcome in short at current/entry/control/SSE boundaries
// 314/474/472/472; long 314/477/475/475. Current lifetime reaches retail length,
// but homes and selective helper expansion remain. Original Boolean outcome restored.
// Round 257: inflated-bound scalar before descriptor construction 461, const 461;
// both with six-use center 496/496; early filter 459/459; center and filter 497/497.
// Original scalar lifetime, field observations and descriptor stores restored.
// Round 250: native lower/upper/translation vector producers, subsets 1..7
// 494/490/547/365/508/521/562; all with complete endpoint copy 601.
// All restored; optional-state homes and selective helper expansion remain.
// Round 181: complete table-sample/ray-origin/output coordinate-copy subsets 395/306/306/395/395/306/395; all with basis-row loops 430. Original native copies and body restored.
// Round 397: scan-lifetime bounds descriptor; entry constructor, native endpoint copy and shared table-sample masks 0..7
// 465/470/486/491/465/470/486/491; all restored.
// Round 185: Optional/required/both registration returns 307/312/313; integrated
// optional/both-return/required/both construction 307/313/330/331; early count 424. Required-result frame
// shrinks to 0x194; actual transform descriptor through scan/cleanup restores 0x1d4 at 326/327/320/321, with
// both complete native copies the same four scores. All original ownership, scopes, copies, query inputs and
// body restored; optional homes and selective helper expansion remain.
// Round 191: native planar radial scratch 319; with shared planar normalization 298 kept
// (plain whole-program, no match losses); ray-difference counterparts 319/298; in-place
// transform counterparts 351/351; complete candidate setter counterparts 323/323. Direct
// zero-on-failure admission 321; inherited later zero guard removed 334; both restored. Frame
// 0x1d4 and optional-state homes remain; original admission tests, zero products and cleanup preserved.
// Typed row-pointer planar read 298; the same radial row, two input components,
// vertical zero product, query inputs and original admission tests are preserved.
// Round 224: complete affine input aggregate with output-first, readonly input and
// native matrix identity subsets: eight forms 298 each. Original complete input
// producer, separate output matrix and every call restored.
// Round 234: Complete native radius-offset vector on planar scratch, producer/sample lifetime and
// component/shared setter 358/358/358/358; each with complete table contribution 357/357/347/347. All
// restored; optional homes and selective helper expansion remain.
// Round 205: Complete native full/planar/both sample inputs 373/298/373; ray-origin setter 317;
// full-origin/planar-origin/all inputs 370/317/370; all with shared ray difference 370.
// All restored; optional-state homes and selective location/range expansion remain.
// Round 253: Coherent unsigned bounds/sample/count domains in seven subsets, then immutable complete
// count; scores 298/298/298/298/298/298/298/298.
// Original executable source restored; no qualifying gain.
// Round 62 (m3 lane 16, fast): retail's planar block reads as `vector3f v = table[i]; v.k = 0.0f; if
// (normalize_inline(&v) == 0.0f) continue;` (a three-component copy, the fabs < 0.0001 skip, i, j and the zero k
// scaled by the inverse, then the `length == 0` skip), with v.i/j/k scaled by arg_6: 308 at 2078 bytes (LCS-aligned
// 205 -> 173; the planar block is then retail's code apart from its frame slots: ours puts the table vector at +0x20,
// the length at +0x2c and the copy at +0x30, retail the copy at +0x2c, the length at +0x44 and the table vector at
// +0x48); the length as a named local 308; the two vectors renamed offset/horizontal, sample_direction/radial,
// direction/planar 308 each (names do not move these slots).
// On that form: the copy declared before the table vector 308; the blend factor first and the copy in its own
// nested block 317 (2075 bytes).
// Round 473: coherent component identity after SSE setup, original/shared filter and per-sample/shared
// invariant blend: 488/476/512/524; entry identity on the same four graphs 493/521/494/502. Original body
// restored; optional-state/sample/ray homes and selective location/release expansion remain.
// Round 30 (lane 2): retail's radial block is 0x29b2e0's inlined planar normaliser (function_29b32b in
// src/unknown_29b2e0.cpp): the whole table entry copied, k cleared, x87 sqrt of i*i + j*j, the epsilon test,
// i scaled by (1 / length) first, and `continue` when it returns 0. That helper moved unchanged into
// include/unknown_11cc90.h and used here (3-D copy, `if (function_29b32b(&radial) == 0.0f) continue;`, scaled
// by arg_6) 306 plain (2078 bytes against 2083, sequence-aligned 205 -> 172, 485 against 486 instructions;
// 0x29b990 kept its match): the positional count rises only because the sample/radial/length/count/optional
// homes differ (ours sample +0x20, radial +0x30, length +0x2c; retail endpoint +0x1c, radial +0x2c, length
// +0x44, sample +0x48). On that form: the endpoint copied from *arg_11 then offset in place 337 (2104 bytes);
// the sample renamed, the radial scale through vector3f_scale, the endpoint declared at the loop top 306 each
// (same code). Restored; a later round should keep this helper form once the homes can be moved.
// @retail 0x1d48f0
bool function_1d48f0(point3f *arg_11, s_havok_component *arg_0, long arg_1, long arg_2, point3f const *arg_3,
 vector3f const *arg_4, s_location *arg_5, real arg_6, real arg_7, bool arg_8,
 point3f const *arg_9, long arg_10)
{
 c_contact_query_bounds_volume *local_1 = NULL;
 long local_2 = NONE;
 _control87(0x9001f, 0x8001f);
 _mm_setcsr(_mm_getcsr() | 0x1f80);
 if (arg_9)
 {
  c_contact_query_bounds_info local_3;
  real local_4 = arg_7 + 0.001f;
  local_3.lower.set(arg_3->x - local_4, arg_3->y - local_4, arg_3->z - local_4);
  local_3.upper.set(arg_3->x + local_4, arg_3->y + local_4, arg_3->z + local_4);
  for (long local_5 = 0; local_5 < 3; ++local_5)
  {
   if (((real *)&local_3.lower.m_quad)[local_5] + 0.001f > arg_9->n[local_5])
    ((real *)&local_3.lower.m_quad)[local_5] = arg_9->n[local_5] - 0.001f;
   if (arg_9->n[local_5] > ((real *)&local_3.upper.m_quad)[local_5] - 0.001f)
    ((real *)&local_3.upper.m_quad)[local_5] = arg_9->n[local_5] + 0.001f;
  }
  local_3.filter = ((arg_0->object_index + 1) << 16) | arg_2;
  local_1 = new c_contact_query_bounds_volume(&local_3);
  ((c_contact_query_world *)g_51e9a4)->add(local_1);
  if (arg_10 != NONE) local_2 = havok_object_get(arg_10)->havok_component_index;
 }
 bool local_0 = false;
 long local_6 = 27 - (arg_8 ? 0 : 9); /* the table's 27 directions, or the first 18 */
 hkTransform local_7;
 c_contact_query_transform_volume *local_8;
 {
  c_contact_query_transform_info local_9;
  hkRigidBody *local_10 = havok_component_rigid_body_get(arg_1, arg_0);
  c_contact_shape_view *local_11 = *(c_contact_shape_view **)((byte *)local_10 + 0xc);
  if (local_11->type() == 0x17) local_11 = *(c_contact_shape_view **)((byte *)local_11 + 0x30);
  local_7.set(local_10->m_motion->m_transform);
  local_9.shape = local_11;
  local_9.transform.set(local_7);
  local_9.filter = ((arg_0->object_index + 1) << 16) | arg_2;
  local_8 = new c_contact_query_transform_volume(&local_9);
 }
 ((c_contact_query_world *)g_51e9a4)->add(local_8);
 c_contact_presence local_12;
 for (long local_13 = 0; local_13 < local_6; ++local_13)
 {
  vector3f local_14 = ((vector3f const *)g_455978)[local_13];
  if (arg_6 > 0.0f)
  {
   vector2f local_15 = *(vector2f const *)((vector3f const *)g_455978 + local_13);
   real local_16 = normalize2d_fast(&local_15);
   if (fabs(local_16) < 0.0001f) continue;
   real local_17 = 1.0f / local_16;
   real radial_height = 0.0f * local_17;
   if (local_16 == 0.0f) continue;
   local_15.i *= arg_6;
   local_15.j *= arg_6;
   radial_height *= arg_6;
   real local_18 = (arg_7 - arg_6) / arg_7;
   local_14.i = local_14.i * local_18 + local_15.i;
   local_14.j = local_14.j * local_18 + local_15.j;
   local_14.k = local_14.k * local_18 + radial_height;
  }
  arg_11->x = local_14.i * arg_7 + arg_3->x;
  arg_11->y = local_14.j * arg_7 + arg_3->y;
  arg_11->z = local_14.k * arg_7 + arg_3->z;
  local_7.m_translation.set(arg_11->x, arg_11->y, arg_11->z);
  local_12.found = false;
  ((hkRigidBody *)local_8)->pose_write(local_7);
  ((c_contact_query_dispatch *)local_8)->query(&local_12);
  if (local_12.found) continue;
  point3f local_19;
  real_point3d_set(&local_19, arg_11->x + arg_4->i, arg_11->y + arg_4->j, arg_11->z + arg_4->k);
  if (function_168f40(0x1480040d, (s_vehicle_ray const *)&local_19, arg_0->object_index, NONE)) continue;
  if (local_1)
  {
   s_1d48f0 local_20;
   local_20.field_0 = *arg_9;
   local_20.field_c.i = local_19.x - local_20.field_0.x;
   local_20.field_c.j = local_19.y - local_20.field_0.y;
   local_20.field_c.k = local_19.z - local_20.field_0.z;
   if (function_182800((s_vehicle_ray *)&local_20, local_2, local_1) && !local_0) continue;
  }
  transform4x3f local_21;
  transform4x3f local_22;
  local_21.scale = 1.0f;
  local_21.forward.i = local_7.m_rotation.m_col0(0);
  local_21.forward.j = local_7.m_rotation.m_col0(1);
  local_21.forward.k = local_7.m_rotation.m_col0(2);
  local_21.left.i = local_7.m_rotation.m_col1(0);
  local_21.left.j = local_7.m_rotation.m_col1(1);
  local_21.left.k = local_7.m_rotation.m_col1(2);
  local_21.up.i = local_7.m_rotation.m_col2(0);
  local_21.up.j = local_7.m_rotation.m_col2(1);
  local_21.up.k = local_7.m_rotation.m_col2(2);
  real_point3d_set(&local_21.position, local_7.m_translation(0), local_7.m_translation(1), local_7.m_translation(2));
  function_1d43d0(arg_0, &local_21, &local_22);
  *arg_11 = local_22.position;
  function_11bed0(arg_5, arg_11);
  local_0 = arg_5->cluster_index != NONE;
  if (local_0) break;
 }
 ((c_contact_query_world *)g_51e9a4)->remove(local_8);
 havok_reference_remove((c_havok_reference_counted *)local_8);
 if (local_1)
 {
  ((c_contact_query_world *)g_51e9a4)->remove(local_1);
  havok_reference_remove((c_havok_reference_counted *)local_1);
 }
 _mm_setcsr(_mm_getcsr() & 0xffffffc0);
 _clearfp();
 _control87(0x9001f, 0xfffff);
 return local_0;
}
