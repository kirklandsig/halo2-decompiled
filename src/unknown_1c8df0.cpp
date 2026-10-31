#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
// @flags /O2 /arch:SSE /Gr

struct s_collision_result_1697c0
{
 long field_0;
 real field_4;
 point3f field_8;
 byte field_14[0x24 - 0x14];
 short field_24;
 byte field_26[0x40 - 0x26];
 long field_40;
 byte field_44[0x5c - 0x44];
};
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
 long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *collision);
bool function_16a040(long flags, point3f const *point0, point3f const *point1, long ignore_object_index,
 long ignore_unit_index, s_collision_result_1697c0 *result);
bool function_11c010(short row, short column);
real function_30bf0(vector3f *v);
real magnitude3d(vector3f const *v);
extern vector3f *g_4687bc;

__declspec(noinline) short __stdcall function_1c8df0(short arg_2, short arg_3, point3f const *arg_4,
 long arg_0, void const *arg_1, long arg_5, bool arg_6, bool arg_7, bool arg_8, long *arg_9);

// score 170 (was 170): Frame 0x8c/0x94, raw secondary-result home and common probe tail remain.
// Round 75 (all match-only, restored): clear state bool, one bool probe result for the mode-1/mode-2
// probes with `category = 1` on a hit 28; with short category 26 (frame, slots, cmp ax all retail);
// centre read through the cast parameter instead of the local copy 20: only the mode-1 x-row fold
// remains (retail `movaps xmm1, xmm0; addss xmm1, [ebp]`, ours loads [ebp] early). On that body:
// product-first rows 20/20, constant-left products 20, literal quarter 20, direction through a local
// copy 26, short-circuit probe result 224; private point-from-line helper 26 (258 inside the pragma,
// which blocks even __forceinline); noinline probe declaration without the pragma 393. Short
// category alone 234, with bool clear state 225.
// Round 81 on that 20 body (plain): a centre-pointer copy in the first probe block and the side
// vector 5 (1175 bytes; mode-1 x row still loads the centre x, z row now folds); also in the mode-1
// rows 5; in the side vector only 26; in the mode-1 rows only 20; in the first block only 33; first
// block and rows 33; side vector and rows 26; side vector as an aggregate 20; negated side j term 259;
// per-row copies (x, z, x and z) on the 5 body 27/5/27; distance blocks through the copy 20/5.
// Compile-only screens reproduce the folds: no read path, row order, declaration order, literal or
// operand order makes the x row fold. All match-only forms restored.
// Round 83 on the 5 body (plain): re-measured 5; rows interleaved plus/minus per axis 53; side
// vector scaled in place, then added 36 (1177 bytes); rows through the copy with the side j term
// negated-first 5; const centre-pointer copy 5; side vector as a rotated point difference 257 (1182
// bytes). Retail's x and y rows (centre components the side
// vector already read) fold, the z row loads first; ours keep x loading first. All restored.
// Earlier: frame 0x8c versus 0x94; secondary-result home and probe-tail normalization differ.
// Word category 225; native clear state 170; direct origin 172; distance category 175; side workspace 175.
// Native clear with direct origin 171, word category 234, short return result 178.
// Earlier: frame 0x8c versus 0x94; secondary-result home and probe tail differ.
// Native clear with byte probe result 74 (not matched); fraction/index names 170/170;
// earlier native clear declaration 170; short clear state 237; short clear with byte result 240 (not matched).
// Earlier: frame 0x8c versus 0x94; raw secondary-result home and common probe tail still differ.
// Early clear/blocked/triple test results 221/173/179; all early test results 241;
// shared preliminary predicate 186; floating consistency 415; size policy 402; target pointer 187.
// frame 0x8c versus 0x94; secondary byte and quarter-point scheduling remain.
// Shared byte plus product-first X/Z/XZ/XYZ/Y 241 each; native point 241;
// axis-wise points 250; offset vector 244; original-arm publication 26 (not matched);
// native category with publication 28; probe/result names 26/26 (not matched).
// Earlier: frame 0x8c versus 0x94 and secondary-result/point schedules remain;
// first-point copy/add 273; displacement then add 5.
// center then add 5; first-point initializer 26.
// shared scalar offsets 47; external origin reference 26.
// center snapshot 329; indexed point loop 86.
// positive indexed add 42; positive indexed center 232.
// separate point loops 34; positive indexed expression 34.
// shared X offset 26; shared Z center 46.
// shared X/Z scalars 46; indexed scale then translate 62.
// All secondary-byte forms above remained unmatched and were restored.
// frame 0x8c versus 0x94 and secondary-result tail remain;
// direct primary miss 207; entry fraction zero 185; early probe bit 181; arithmetic clear bit 288;
// early bit with shared probe and Boolean/native clear 44/85 (not matched);
// shared probe with entry fraction/direct primary miss 44/71 (not matched).
// Separate copied probe output for all/quarter/triple/both arms 428/498/468/397;
// same output scopes with Boolean primary clear 422/492/462/391. Output copies grow frames.
// Staged byte-flow positive/negative/both point names 5/5/5; native positive/
// negative component arrays 5/5; existing shared scale for positive/both points
// 276/210; direct quarter constant 5. All unmatched added-byte forms restored.
// Earlier: Frame 0x8c/0x94 and secondary-result tail remain; byte-flow quarter-point
// XZY/YXZ/YZX/ZXY/ZYX completion 12/23/20/21/18; XYZ products then XYZ/ZXY centers 36/37; ZYX
// products then XYZ centers 47. All added-byte forms restored without a match.
// Earlier: frame 0x8c/0x94 and secondary-result tail remain;
// staged-byte negative point first 230; negative XZY/YXZ/YZX/ZXY/ZYX 11/17/14/18/18;
// staged paired axes 53; both points staged 5. All added-byte forms restored.
// Earlier: frame 0x8c/0x94 and secondary-result tail remain;
// staged-byte center-left completion X/Z/XZ/Y 5/5/5/5; constant-left products X/Z/XZ 5/5/5;
// both operand affinities on XZ 5. All unmatched added-byte forms restored.
// Late length/classification/output/both direction workspaces at entry 170 each;
// length with classification/output/all workspaces 170 each; all role homes 170 (calibrated).
// Round 151: Retained ray 192; with native side/mode/both 192/191/191; Boolean primary with ray/side/mode/both 186/186/185/185.
// Round 127: normalized side snapshot 275; third-probe axis receiver 177; both 273; collision at
// entry with side/axis/both 275/177/273; both with short category 288 or Boolean primary 261.
// Calibrated with magnitude 24590; all restored. Frame 0x8c and secondary-result tail remain.
// Complementary primary-hit bool/byte/short/long outcome arms 180/180/237/171;
// direct trace producers 274/274/274/200 (fast). Original primary/category flow restored.
// Round 187 separate caller/provider inline/noinline declarations: preliminary query 352/170;
// primary collision 170/170; secondary collision 170/170; all three 352/170 (fast).
// All restored; frame 0x8c/0x94 and raw secondary-result home/common tail remain.
// Round 189 category byte/signed-byte/unsigned-dword/unsigned-word with unsigned-dword primary
// 230/230/170/234; with byte primary 238/238/175/225. No extra result flag; original types
// restored.
// Round 161: complete scalar centers direct/staged/native point 321 each; native center plus complete offsets 329; complete scalar inputs direct/native points/staged positive 321 each; library center plus offsets 329. All added-byte forms restored.
// Round 233: Shared side-input scale, quarter in-place/displacement 239/242;
// both modes in-place/displacement 242/252; raw secondary-byte counterparts
// 195/210/200/219. Original side observations and flagless body restored.
// Round 198: Normalizer inline/noinline and magnitude inline/forceinline caller
// declarations in eight distinct pairs 170 each. Original math boundaries and body restored.
// Round 225: complete quarter/triple/both point arrays on the staged raw-byte flow
// 68/90/90; at entry 68/104/112; shared three-point workspace in optional/entry
// scope 104/104. Raw-byte control 20/1176. Every grouped-point/extra-result form
// is unmatched and restored; original flagless category/clear flow retained.
// Round 150: Raw-byte path with common positive/opposite/third input point homes, seven subsets 5/28/28/5/5/28/28; all three before primary output 89. All exact-match-only forms restored.
// Frame 0x8c/0x94 and raw secondary-result/common tail remain; near-five forms still miss X/Z point-add scheduling.
// Primary endpoint helper with outcome long/long, bool/long, long/short, bool/short
// state/category 170/175/234/225; direct-result counterparts 207/202/238/242. All restored.
// Round 7 (lane 10, plain checks; the fast check builds this differently): short category, bool clear
// state, one bool probe result with `category = 1` on a hit 26 (1177 bytes); plus the mode-1 centre
// read through the cast parameter 5 at 1175 bytes, no staged arithmetic needed (left: retail folds the
// x centre as a memory operand and loads the z centre; ours the reverse). On that: x row product first
// 5; cast centre for the positive point only 5; negative point first 230; literal quarter 20; mutable
// spacing 5. Match-only (added result byte); restored.
// Actual category defaults (magnitude-calibrated): entry zero/native category 408; entry zero/short category 425.
// admitted zero/native category 419; admitted zero/short category 436.
// entry zero/native/direct clear 397; entry zero/short/direct clear 414.
// admitted zero/native/direct clear 408; admitted zero/short/direct clear 425.
// All executable forms restored.
// Round 12 (lane 10, plain) on the rebuilt five-difference body (short category, bool clear state,
// one bool probe result with `category = 1` on a hit, mode-1 centre read through the cast
// parameter; 5 at 1175 bytes): named per-row offsets in all three rows 36; x rows through local_3
// 27; z rows through local_3 20 (the z row then loads the centre as retail; only the x row's fold is
// left, our centre-x load hoisted above the product, 1176 bytes); on that, x row product first 20;
// x and z rows through local_3 26. Match-only (added result byte, mixed centre reads); restored.
// Round 86 (plain): the probe kept out of line by a noinline declaration in its own file (0x16a040) in place
// of the in-body inline_depth pragma, 170 (kept; same code); on that, the round-81 five-difference body 5 (same
// x/z row folds), its plus rows through a point-along-vector helper 5 (identical), every centre read through
// local_3 26; short category on the kept body 234.
// Round 30 (plain) on the rebuilt five-difference body: positive-point centre reads through local_3
// for y 20, x 27, x and y 5, y and z 5; z/x/y written product first 5/5/5 (source order of each sum
// has no effect); the side vector's centre reads through the cast parameter for both/j/i 33/26/33 (j
// fixes the mode-1 z row but the 0.1 block then folds its z); x+ centre read as a real array element 5;
// positive rows in z, x, y / y, z, x order 21/20. Ours always loads the x centre ahead of the first
// row's multiply. All match-only; restored.
// Round 63 (plain on the five-difference control): side j/i/both union-array reads 5/5/5;
// side order j-i-k 11; aggregate side 5; existing add-scaled helper both/negative only 238/238;
// side order i-k-j 10. No match; original 170 body restored.
// Round 180: Guarded triple-probe opposite/third/both construction 240/220/256; same with Boolean
// primary 230/209/246; both with short category 277, plus Boolean primary 268. Archived
// secondary-byte control 5; existing side/quarter-add-scaled/triple-point constructor subsets
// 5/26/26/5/5/26/26, native side constructor 170. No exact match; every added-byte, constructor and
// point-lifetime form restored.
// Round 206: Native short probe-mode contract with role-name/Boolean-primary/short-category subsets
// [170, 170, 175, 175, 234, 234, 225, 225]. Original interface and body restored.
// Round 302: Late classifier default-four/default-two/terminal-selection/full-selection
// 176/172/170/170; with Boolean primary state 174/174/175/175. Original category
// producers and primary state restored; frame 0x8c/0x94 and secondary tail remain.
// Round 197: Complete immutable positive-quarter/negative-quarter/triple probe points,
// masks 1..7, and all mutable aggregate points each score 5 on the shared-outcome graph.
// X/Z load scheduling remains; the added result byte is exact-only and every form is restored.
// DL204: shared-outcome side/fallback setters and named normalization length, masks 1..7,
// 5/254/254/26/26/254/254; complete graph with fallback library copy 26. Exact-only forms restored.
// Round 460: Shared-outcome graph with native normalized direction copies for positive,
// negative and endpoint consumers, subsets 1-7 and immutable all, 350/337/370/222/364/365/364/364.
// No exact match; every direction copy and added-result form restored.
// @retail 0x1c8df0
short __stdcall function_1c8df0(short arg_2, short arg_3, point3f const *arg_4, long arg_0,
 void const *arg_1, long arg_5, bool arg_6, bool arg_7, bool arg_8, long *arg_9)
{
 long category;
 real local_1;
 long local_2 = NONE;
 point3f const *local_3 = (point3f const *)arg_1;
 if (arg_9)
  *arg_9 = NONE;
 if (arg_2 != NONE && arg_3 != NONE && !(char)function_11c010(arg_2, arg_3))
 {
  category = 4;
  local_1 = 0.0f;
 }
 else
 {
  s_collision_result_1697c0 local_6;
  local_6.field_24 = NONE;
  long local_4 = arg_8 ? 0x15808c2f : 0x5808c2f;
  if (arg_6)
   local_4 |= 0x8000000;
  if (arg_7)
   local_4 &= ~0x20;
  long local_7;
  {
   vector3f local_5;
   vector3d_from_points3d(local_3, arg_4, &local_5);
   if (!function_1697c0(local_4, local_3, &local_5, arg_0, NONE, &local_6))
    local_7 = true;
   else
   {
    local_7 = false;
    local_1 = local_6.field_4;
    local_2 = local_6.field_40;
   }
  }
  if ((short)arg_5 != 0)
  {
   vector3f local_8;
   local_8.i = local_3->y - arg_4->y;
   local_8.j = arg_4->x - local_3->x;
   local_8.k = 0.0f;
   if (function_30bf0(&local_8) == 0.0f)
    local_8 = *g_4687a8;
   local_4 |= 0x10000000;
   if ((short)arg_5 == 1)
   {
    point3f local_9;
    point3f local_10;
    real const spacing = 0.25f;
    local_9.x = local_3->x + local_8.i * spacing;
    local_9.y = local_3->y + local_8.j * spacing;
    local_9.z = local_3->z + local_8.k * spacing;
    local_10.x = local_3->x - local_8.i * spacing;
    local_10.y = local_3->y - local_8.j * spacing;
    local_10.z = local_3->z - local_8.k * spacing;
    if (local_7)
    {
     category = function_16a040(local_4, &local_9, arg_4, arg_0, NONE, &local_6);
     if (category == 0)
      category = function_16a040(local_4, &local_10, arg_4, arg_0, NONE, &local_6);
     if (category != 0)
     {
      local_2 = local_6.field_40;
      local_1 = local_6.field_4;
     }
    }
    else
    {
     category = !function_16a040(local_4, &local_9, arg_4, arg_0, NONE, &local_6);
     if (category == 0)
      category = !function_16a040(local_4, &local_10, arg_4, arg_0, NONE, &local_6);
    }
   }
   else if (local_7)
   {
    point3f local_9;
    point3f local_10;
    point3f local_11;
    local_9.x = arg_4->x + local_8.i * 0.1f;
    local_9.y = arg_4->y + local_8.j * 0.1f;
    local_9.z = arg_4->z + local_8.k * 0.1f;
    local_10.x = arg_4->x - local_8.i * 0.1f;
    local_10.y = arg_4->y - local_8.j * 0.1f;
    local_10.z = arg_4->z - local_8.k * 0.1f;
    local_11.x = arg_4->x + g_4687bc->i * 0.1f;
    local_11.y = arg_4->y + g_4687bc->j * 0.1f;
    local_11.z = arg_4->z + g_4687bc->k * 0.1f;
    category = function_16a040(local_4, &local_9, local_3, arg_0, NONE, &local_6);
    if (category == 0)
     category = function_16a040(local_4, &local_10, local_3, arg_0, NONE, &local_6);
    if (category == 0)
     category = function_16a040(local_4, &local_11, local_3, arg_0, NONE, &local_6);
    if (category != 0)
    {
     local_2 = local_6.field_40;
     local_1 = local_6.field_4;
    }
   }
   else
    category = 0;
   if (category != 0)
   {
    goto local_14;
   }
  }
  if (local_7)
   category = 0;
  else
  {
   vector3f local_13;
   vector3d_from_points3d(local_3, arg_4, &local_13);
   real local_12 = magnitude3d(&local_13);
   if (local_12 < 1.0f)
    category = 4;
   else if (local_12 * local_1 < 1.0f)
    category = 2;
   else if (!arg_8 && (1.0f - local_1) * local_12 < 4.0f)
    category = 3;
   else
    category = 4;
  }
 }
local_14:
 if (arg_9 && !arg_8 && category >= 1)
 {
  vector3f local_15;
  vector3d_from_points3d(local_3, arg_4, &local_15);
  if ((1.0f - local_1) * magnitude3d(&local_15) < 4.0f)
   *arg_9 = local_2;
 }
 return category;
}
