#include "unknown_11c920.h"
#include "unknown_1cec30.h"
#include "unknown_1eb550.h"
#include "object_markers.h"
#include "unknown_1428b0.h"
#include <math.h>
#include "vector_products.h"
// @flags /O2 /arch:SSE /Gr

struct s_object;
s_object *function_badc0(long arg_0, dword arg_1);
word *function_def30(long arg_0);
void function_119250(long arg_0);
transform4x3f *function_ba160(long arg_0, transform4x3f *arg_1);
real function_30bf0(vector3f *arg_0);
real magnitude3d(vector3f const *arg_0);
bool __stdcall function_a75d0(vector3f *arg_0, real arg_1);
quaternionf *function_141f60(matrix3x3 const *arg_0, quaternionf *arg_1);
matrix3x3 *function_142eb0(matrix3x3 const *right, matrix3x3 const *left, matrix3x3 *out);
void function_11d790(quaternionf const *arg_0, real *arg_2, vector3f *arg_1);
void havok_component_rigid_body_angular_velocity_get(s_havok_component *arg_1, long arg_0, vector3f *arg_2);
void havok_component_rigid_body_linear_velocity_get(s_havok_component *arg_1, long arg_0, vector3f *arg_2);
real havok_component_rigid_body_mass_get(long arg_0, s_havok_component *arg_1);
void havok_component_rigid_body_linear_velocity_change(long arg_0, s_havok_component *arg_1, vector3f const *arg_2);
void havok_component_rigid_body_angular_velocity_set(long arg_0, s_havok_component *arg_1, vector3f const *arg_2);
real function_1d2280(bool arg_0, bool arg_1, real arg_2, real arg_3, real arg_4, real arg_5, real arg_6, real arg_7, real arg_8, bool arg_9, real *arg_10);

struct s_1d24a0
{
 signed char field_0;
 signed char field_1;
 byte field_2;
 signed char field_3;
 long field_4;
};
struct s_1d24a1
{
 dword field_0;
 long field_4;
 long field_8;
 long field_c;
 real field_10;
 real field_14;
 real field_18;
 real field_1c;
 real field_20;
 real field_24;
 real field_28;
 real field_2c;
 real field_30;
 real field_34;
 byte field_38[0x54-0x38];
 real field_54;
 real field_58;
 real field_5c;
 byte field_60[8];
};
PRIVATE __forceinline real function_1d24a1(vector3f const *arg_0, vector3f const *arg_1)
{
 return (real)((double)arg_0->k * arg_1->k + (double)arg_0->j * arg_1->j + (double)arg_0->i * arg_1->i);
}
PRIVATE __forceinline real function_1d24a6(vector3f const *arg_0, vector3f const *arg_1)
{
 return arg_0->i * arg_1->i + arg_0->j * arg_1->j + arg_0->k * arg_1->k;
}
PRIVATE __forceinline vector3f *function_1d24a2(vector3f const *arg_0, real arg_1, vector3f *arg_2)
{
 for (short component = 0; component < 3; ++component)
  arg_2->n[component] = arg_0->n[component] * arg_1;
 return arg_2;
}
PRIVATE __forceinline void function_1d24a3(vector3f *arg_0, vector3f const *arg_1)
{
 arg_0->i += arg_1->i;
 arg_0->j += arg_1->j;
 arg_0->k += arg_1->k;
}
PRIVATE __forceinline void function_1d24a4(vector3f *arg_0, vector3f const *arg_1)
{
 arg_0->i -= arg_1->i;
 arg_0->j -= arg_1->j;
 arg_0->k -= arg_1->k;
}
PRIVATE __forceinline long function_1d24a5(real arg_0)
{
 long local_0;
 __asm
 {
  fld arg_0
  fistp local_0
 }
 return local_0;
}

PRIVATE __forceinline real function_1d24a7(point3f const *arg_0, point3f const *arg_1,
 vector3f const *arg_2, vector3f *arg_3)
{
 vector3f local_0;
 local_0.i = arg_0->x - arg_1->x;
 local_0.j = arg_0->y - arg_1->y;
 local_0.k = arg_0->z - arg_1->z;
 real local_1 = function_1d24a6(&local_0, arg_2);
 arg_3->i = local_0.i - arg_2->i * local_1;
 arg_3->j = local_0.j - arg_2->j * local_1;
 arg_3->k = local_0.k - arg_2->k * local_1;
 return function_30bf0(arg_3);
}
// score 902 (was 902): Frame 0x1f0/0x1d4, entry/second activity and record/vector homes remain.
// Earlier score 902 (was 920): Frame 0x1f0 versus 0x1d4; entry EAX/ECX, delayed EBX, second activity byte and record/vector homes differ.
// Round 105: retail has 23 more mulss, 22 more movss and 15 more movaps, ours ~10 extra counted loops
// (xor/dec/jne/add): retail's scale helper is unrolled. The helper as three statements gives retail's frame
// 0x1d4 and the same 32 calls with matching instruction counts between calls in all but four segments, but
// 4008 bytes and 1199: our build keeps the scaled/residual vectors (local_37, local_40, local_50, local_52) in
// XMM registers where retail keeps them in memory (it loads and stores each component). On that base: the
// residual subtract written in both arms 1181/1161 (retail's layout, frame 0x1d0), the add/subtract helpers as
// loops 1074-1192 (frame grows), all three last-block loops unrolled 1233, plain external helpers 1199. On the
// loop base: locals renamed in either order 920 (identical code), local_1 declared after local_2/local_3
// 1010, local_3 before local_2 920, the record pointer as `data + index` 920, a shared simulating-state
// helper for both body tests 939, a word scale counter 920 (not unrolled).
// Round 97: offset vector built in place (no copy) 1019 (frame 0x1e4, 4167 bytes); body position
// through its pointer 1046 (0x1e4, 4153 bytes): retail is longer, so it has more copies, not fewer;
// marker, accumulated velocity and response output at function scope 920 each (no frame change).
// Round 73: shared flags word for both entry tests 920; set-then-clear second flag 1019; result/accumulator
// declared first 927; `>=` threshold flag 994; record object field read first into a named local 923.
// Earlier: frame 0x1f4 versus 0x1d4; entry EAX/ECX, delayed EBX and record/vector homes remained.
// Earlier 1218 from 1259: frame agrees; early EBX save, array/entry allocation and delta reloads differ.
// Function-scope linear change 1259; function-scope velocity outputs 1259; broader matrix workspace 1259;
// inline depth one 1259; counted angular residual 1218, kept after plain check.
// Earlier 1214 from 1218: frame 0x1d0 versus 0x1d4; early EBX/entry allocation and vector reloads differ.
// Direct radial residual loop 1282; planar residual loop 1261; angular residual loop 1214, kept;
// displacement loop 1246; all three residual loops 1227.
// Earlier 1207 from 1214: frame 0x1d4 restored; early EBX, entry homes and vector reloads remain.
// Record-scope angular residual 1214; function-scope angular change 1214; early angular axis 1214;
// native removed-slot element 1214; one record option-word read for all four flags 1207, kept.
// Earlier 1016 from 1207: frame 0x1e8 versus 0x1d4; early EBX, entry homes and vector reloads remain. Plain
// scale helper loop 1016 kept; add loop 1233; subtract loop 1126; all three 1328; shared planar difference
// 1207; scale/subtract 1123; scale/add 1261; add/subtract 1201.
// Scale helper void result 1016; unsigned/signed-word ordinal 1016/983 plain; counted while
// 1016; equality termination 1016. Word counter fully unrolls; frame grows four bytes.
// Complete linear coefficient observations 966; angular pair 953 kept (plain); shared limits 1011;
// linear/angular 980; linear/limits 1065; angular/limits 979; all 1065; all/read-only record 1065.
// Round 155: five alternate scale formal orders, source-first add/subtract and all three
// helper orders together 953 each; every definition and call restored.
// Native scale input/output/both coordinate cursors 1045/1032/953; end cursor 993;
// add/subtract/both coordinate cursors 1155/1115/1218; all producer cursors 1212 (fast).
// Original scale word ordinal, angular coefficient pair and all helpers restored.
// Native scale/add/subtract input values singly and in pairs/all [1295, 992, 977, 1455, 1456, 1130, 1555];
// all with planar input values 1746. Original helpers and coefficient pair restored.
// Complete native vector outputs in scale/add/subtract helpers 1291/1048/984; scale-add/scale-subtract/add-subtract/all 1466/1434/1147/1610;
// all native vector outputs with completed planar workspace 1668. Original helpers and accepted loop/coefficient pair restored.
// Scale signed-byte/unsigned-byte/unsigned-word ordinals 953/953/953; unsigned-word scale with signed-word
// add/subtract/both loops 1152/1115/1215; all unsigned-byte/signed-byte producer loops 1215/1215. Original
// helpers and coefficient pair restored.
// Earlier declarations of vector workspaces local_75, local_37, local_40, local_43, local_50,
// local_52, local_53 and local_58 each 953.
// Round 137: response pairs 34/48, 34/56, 34/66, 48/56, 48/66, 56/66 959,959,957,933,920,925;
// all responses at record/function scope 979,979;
// triples 34-48-56/34-48-66/34-56-66/48-56-66 973,974,987,959;
// disjoint pairs 34-48+56-66/34-56+48-66/34-66+48-56 987,986,984.
// Selected t5 920 plain; the same real response output is read before its next write.
// Round 140: Shared change pairs 37/50,37/58,37/59,50/58,50/59,58/59 and linear/all phases
// 969/945/988/945/988/956/960/1012; residual pairs 40/52,40/68,52/68/all 1012/966/988/1027; axis pairs
// 31/43,31/63,43/63/all 1004/991/980/1018. Existing shared scale on the retained response graph, with shared
// changes and consistent flag reads 1199/1201/1188/1200; all restored. Shared linear changes recover frame
// 0x1d4 at 960; input and record homes still differ.
// Matrix output in dead body rotation, alone/with first/third/both response outputs
// 931/986/959/987; in dead marker rotation 946/978/952/985. Original separate product
// matrix, response outputs, helper calls and complete body restored.
// Native current record value/point setter/both 1003/935/1009; live option word
// 1007, with record/point setter/both 990/1005/985; full component aggregate 985.
// Record value fixes entry registers, point setter fixes the second byte home; all restored.
// Record memmove, center/input/initial-vector library copies and combinations
// 999/920/920/920/999/999/920/999; record copy recovers entry registers but frame grows to 0x1f4. All restored.
// Round 227: Direct array/entry/tag bases, each pair/all, all with descriptor 966/968/1135/997/1153/1160/1193/1405.
// Original body and all helpers restored.
// Round 202: Linear workspace ending 920; rotation/quaternion endings, their pairs with linear,
// both endings, all three and all with tag-chain ending 938 each. Original lifetimes restored.
// Round 364: second activity word/admission byte/both/first activity word
// 997/920/997/993; first-second/first-admission/all/all words
// 988/993/988/987. Word activity can recover entry registers but not frame; all restored.
// Entry predicate long/bool, bool/long, long/long, short/long carriers
// 996/1012/953/1003; complemented step/admission/both 1013/920/1013;
// long/short entry carriers 981. Original entry predicates and full body restored.
// Round 172: native actual-vector reference formals in scale/add/subtract, seven nonempty
// subsets and all with native planar inputs/output, all eight 920. Original pointer contracts,
// helpers and body restored; entry carriers, frame 0x1f0/0x1d4 and record/vector homes remain.
// Round 161: Native external option dword/word/byte units 1007/965/1006; read-only selected record
// with those units 1007/965/1006; native twelve-byte descriptor 920; descriptor plus dword options
// 1007. All declarations, flag observations and original body restored.
// Round 176: Library source captures in scale/add-subtract/both/planar/scale-planar/
// add-subtract-planar/all 1310/1130/1552/1072/1500/1341/1762; all with projected output copy 1796.
// Original helpers, short scale loop, response sharing and coefficient pair restored.
// Round 33: Ordinary inline scale/add/subtract subsets, then all with planar producer: 920/920/920/920/920/920/920/920. Original forced producers, body and response sharing restored.
// Round 388: Shared complete linear/angular change accumulator 1007; with intermediate
// change pairs 37-50/37-58/50-58 or all linear changes 1003/1008/1008/1027;
// with linear residual/axis reuse 1024/1018; all three reuse families 1064.
// Original separate accumulators and all intermediate vectors restored; entry/frame homes remain.
// Round 257: Final rounded-step/level/target declaration pairs at entry, all seven
// nonempty subsets, then all with native scalar role names: 920 each. Every conversion
// and read stayed at its producer; original declarations and complete body restored.
// Round 268: Shared constraint error/axial speed/residual speed, seven subsets, then all with speed
// change: 975/979/991/974/974/991/974/974. Original body restored.
// Round 41: Native signed body-slot and composed owner/other activity carriers:
// [997, 1001, 921, 970, 956, 1010, 1011, 1008]. Narrow byte/short slots recover entry ECX/EAX at unchanged frame 0x1f0;
// body and record/vector homes still differ. Original slot, predicates, helpers and body restored.
// Round 195: Complete wide-dot/real-dot/planar product groups and their four
// combinations 947/938/938/954/947/920/954; all with projected-vector result
// 958. Original kernels, short scale loop, coefficient pair, solver response
// sharing and full body restored; entry, frame and record/vector homes remain.
// Round 203: existing shared scale/accumulation/real-dot kernels, seven
// subsets and all with shared displacement 1199/920/920/1199/1199/920/1199/1199.
// Coordinated solver inline declaration on counted/expanded kernels 920/1199;
// existing center point setter on expanded scale/scale-add 1168/1168.
// Point production retains the expanded entry pair and grows frame to 0x1d8;
// all helpers, declarations, original center copy and body restored.
// Round 97 (lane 7): the angular accumulate through the existing vector3f_add_scaled 961 alone; with the
// angular residual as three statements 902 (4267 bytes); with the residual through the existing
// vector3d_from_points3d instead 902, kept (retail keeps the accumulated vector in XMM registers to the join);
// the residual helper alone 913; the two linear-block residual subtracts through the same helper too 906,
// either one alone 902 (same code); on 902 a long scale counter 933; the attached object index read once into
// a named local 926. A renamed second activity bool moves no byte slot.
// Round 455: Shared scaled accumulation at radial correction/planar projection/both/planar correction/
// radial-planar corrections/both planar sites/all: 941/917/941/917/941/917/920; all with existing
// point-difference producer 920. Frame remains 0x1f0; original residuals, helpers and body restored.
// Round 255: existing shared difference API in the repeated subtract kernel, crossed
// with radial/planar residual and initial displacement consumers, all eight 920.
// Original kernels and full body restored; frame 0x1f0/0x1d4 and entry/record/vector homes remain.
// Round 477: Add/subtract publication ikj/jik/jki/kij/kji 903/902/903/903/903; add-only jki/kji and subtract-only kji 904/904/904.
// Original repeated producers, short scale loop, accepted angular consumers and complete body restored; frame 0x1f0/0x1d4 and entry/record/vector homes remain.
// Round 268: scalar maximum selections at radial/planar/axial/angular response joins,
// linear pairs 1-2/2-3, axial-angular pair and all four, all eight 902. Original branch updates restored.
// Round 448: Early captured bit-25/26 options 993/993/989; late captured bit-2/1
// options 949; the three combinations 907/907/907, complete role names 907.
// Original option lifetimes and all kernels, response sharing and body restored.
// Round 237: native wide-dot/real-dot/planar input values, seven nonempty subsets 1249/1274/1605/1145/1402/1446/1741;
// all with long scale ordinal 1682
// Original executable body and helpers restored.
// Round 50 (m3 lane 11, fast): retail's byte slots hold local_17 (record bit 2) at +9 and local_1 at +0xa where
// ours has them the other way; local_17 renamed to sort before local_1, local_1 renamed to sort after it,
// local_17 declared at the top, local_17 declared between local_0 and local_1: 902 each (identical code); the
// early return as a single-exit if 902 (identical code).
// Round 65 (lane 16, fast): address-taken homes mapped from both listings: the marker, quaternion, body matrix
// and product matrix stack identically at the top (each 0x1c higher in ours); the gap is all below the marker.
// Retail keeps the second-block and angular response fractions in separate homes (+0xdc, +0x2c; ours shares
// +0x10), the angular block's real output at +0xf4 next to the first block's response output (+0xf8). Each block's
// response fraction declared in its own block 935 (frame 0x1f4; homes +0xf8/+0x20, not retail's); the one-use
// motion-position pointer dropped 965 (4284 bytes); the first block's difference through the existing
// vector3d_from_points3d without the staging copy 961 (frame 0x1e4, 4235 bytes); split responses with it 968.
// The body slot index read once into its local everywhere (the angular block's three direct field reads through
// the local) 941 (4232 bytes); the re-read after the lookup calls dropped 985 (frame 0x1ec); both 999.
// Round 217: standard slot upper, paired byte bounds and response-fraction bounds, seven subsets,
// then all with target-level bound: 998/980/1046/963/1006/1067/1131/1135. Original restored.
// Round 331: Complete factor-first scale, reversed wide-dot and real-dot multiplicands, seven subsets
// and all with planar product order: 902 each. Original private kernels and complete body restored;
// entry/frame, second activity byte and lower vector/scalar homes remain.
// Round 259: Shared scale for radial/planar/axial/angular/radial-planar/radial-angular/planar-angular/radial-planar-angular phases
// 1009/1014/912/939/1136/1090/1074/1206. Original body and producer sites restored.
// Round 283: reverse scale short/long/signed-byte 902/929/902; reverse short scale with reverse
// add/subtract/both 1086/1060/1158; with forward add/subtract loops 1157; all reverse long 1239.
// Original producers, accepted angular consumers and full body restored; frame and entry homes remain.
// Round 237: native wide-dot/real-dot/planar input values, seven nonempty subsets 1249/1274/1605/1145/1402/1446/1741;
// all with long scale ordinal 1682
// Original executable body and helpers restored.
// Round 241: In-place direction-to-residual storage at radial/planar/angular sites,
// singles/pairs/all 946/934/939/954/962/954/963; all with shared radial/planar differences 963.
// Original separate direction/residual vectors, short scale loop and complete body restored.
// Round 245: existing component upper-bit field at admission/activity/publication, seven
// subsets 904/902/904/902/904/902/904; all with test-produced activity result 999.
// Original component flag observations and publication, helpers, solver-response sharing
// and complete executable source restored; entry/frame and activity/vector homes remain.
// Round 293: Owner/other body-simulation result preset/fill and exhaustive outcomes, singles and all
// pairings: 945/983/992/966/984/992/991/993. Original predicates, kernels and full body restored;
// entry/frame and activity/record/vector homes remain.
// @retail 0x1d24a0
void __stdcall function_1d24a0(s_havok_component *arg_0, float arg_1)
{
 if (!(arg_0->unknown04 & 0x104000)) return;
 bool local_0 = !(fabs(arg_1) < 0.0001f);
 bool local_1 = local_0 && !(bool)((arg_0->unknown04 >> 13) & 1);
 bool local_2 = false;
 real local_3 = 0.0f;
 if (arg_0->unknown94 && local_0)
 {
  byte *local_4 = (byte *)havok_object_get(arg_0->object_index);
  byte *local_5 = g_4e3b44[*(long *)local_4 & 0xffff].bytes;
  byte *local_6 = g_4e3b44[*(long *)(local_5 + 0x38) & 0xffff].bytes;
  byte *local_7 = g_4e3b44[*(long *)(local_6 + 0x24) & 0xffff].bytes;
  for (long local_8 = 0; local_8 < arg_0->unknown94->size; ++local_8)
  {
   s_havok_array08 *local_9 = arg_0->unknown94;
   s_1d24a0 *local_10 = &((s_1d24a0 *)local_9->data)[local_8];
   byte *local_11 = *(byte **)(local_7 + 0x44) + local_10->field_0 * 12;
   s_1d24a1 *local_12 = &(*(s_1d24a1 **)(local_7 + 0x2c))[*(short *)(local_11 + 8)];
   byte *local_13 = local_10->field_4 != NONE ? (byte *)function_badc0(local_10->field_4, (dword)-1) : NULL;
   s_havok_component *local_14 = NULL;
   if (local_10->field_4 != NONE && local_13 && *(long *)(local_13 + 0xb4) != NONE)
    local_14 = havok_component_get(*(long *)(local_13 + 0xb4));
   long local_15 = local_10->field_1;
   long local_16 = local_14 ? (local_15 < 0 ? 0 : local_15 > local_14->rigid_bodies.size ? local_14->rigid_bodies.size : local_15) : NONE;
   if (!local_14 || local_16 != local_15)
   {
    --local_9->size;
    ((s_1d24a0 *)local_9->data)[local_8] = ((s_1d24a0 *)local_9->data)[local_9->size];
    --local_8;
    continue;
   }
   dword record_flags = local_12->field_0;
   bool local_17 = (bool)((record_flags >> 2) & 1);
   bool local_18 = (bool)((record_flags >> 1) & 1);
   hkRigidBody *local_19 = arg_0->rigid_bodies.data[local_10->field_3].rigid_body;
   bool local_20 = (!local_19->m_fixed && local_19->body_is_simulating().m_bool) || local_1;
   bool local_21 = local_14->rigid_bodies.data[local_15].rigid_body->body_is_simulating().m_bool || local_20;
   bool local_22 = (bool)((record_flags >> 25) & 1);
   bool local_23 = (bool)((record_flags >> 26) & 1);
   s_object_marker local_24;
   if (!function_b8d30(arg_0->object_index, local_12->field_8, &local_24, 1, false))
    function_ba160(arg_0->object_index, &local_24.matrix);
   if (!local_21) continue;
   vector3f local_25 = *g_4687a4;
   real response_fraction;
   byte *local_26 = g_4e0300->data + (local_14->object_index & 0xffff) * 12;
   dword local_27 = 1 << local_26[3];
   if (local_27 & 1) function_def30(local_14->object_index);
   else if (local_27 & 0x1000) function_119250(local_14->object_index);
   local_15 = local_10->field_1;
   point3f const *local_76 = (point3f const *)((byte *)local_14->rigid_bodies.data[local_15].rigid_body->m_motion + 0x70);
   point3f local_28 = *local_76;
   vector3f local_29;
   vector3f local_30;
   havok_component_rigid_body_angular_velocity_get(local_14, local_15, &local_29);
   havok_component_rigid_body_linear_velocity_get(local_14, local_15, &local_30);
   if (local_12->field_20 != 0.0f && local_12->field_24 != 0.0f)
   {
    vector3f local_75;
    local_75.i = local_24.matrix.position.x - local_28.x;
    local_75.j = local_24.matrix.position.y - local_28.y;
    local_75.k = local_24.matrix.position.z - local_28.z;
    vector3f local_31 = local_75;
    real local_32 = function_30bf0(&local_31);
    if (local_32 == 0.0f) local_31 = *g_4687b0;
    real local_33 = function_1d24a1(&local_30, &local_31);
    real local_34;
    real local_35 = function_1d2280(true, local_18, havok_component_rigid_body_mass_get(local_15, local_14),
     local_32, arg_1 * local_12->field_20, local_33, local_12->field_24, local_12->field_1c,
     local_12->field_18, local_22, &local_34);
    real local_36 = g_510c54->rate * local_12->field_20;
    vector3f local_37;
    function_1d24a2(&local_31, local_35, &local_37);
    if (!local_17 && local_36 > local_35)
    {
     real local_38 = g_51e9c4->unknown0 * g_510c54->rate;
     real local_39 = local_36 - local_35;
     if (!(local_38 > local_39)) local_39 = local_38;
     local_37.k += local_39;
    }
    if (local_32 - local_12->field_1c >= 0.0f)
    {
     vector3f local_40;
     function_1d24a2(&local_31, local_33, &local_40);
     local_40.i = local_30.i - local_40.i;
     local_40.j = local_30.j - local_40.j;
     local_40.k = local_30.k - local_40.k;
     real local_41 = magnitude3d(&local_40);
     if ((local_41 >= 0.0f ? local_41 : 0.0f-local_41) > 0.001f)
     {
      if (local_41 > local_36)
      {
       real local_42 = local_36 >= 0.0f ? local_36 : 0.0f-local_36;
       function_1d24a2(&local_40, 1.0f - (local_41-local_42)/local_41, &local_40);
      }
      function_1d24a4(&local_37, &local_40);
     }
     if (!(local_3 > local_34)) local_3 = local_34;
     local_2 = true;
    }
    function_1d24a3(&local_25, &local_37);
   }
   if (local_12->field_28 != 0.0f && local_12->field_2c != 0.0f)
   {
    vector3f local_43;
    real local_46 = function_1d24a7(&local_24.matrix.position, &local_28, &local_24.matrix.forward, &local_43);
    if (local_46 != 0.0f)
    {
     real local_47 = function_1d24a1(&local_43, &local_30);
     real local_49 = function_1d2280(true, local_18, havok_component_rigid_body_mass_get(local_15, local_14),
      local_46, local_12->field_28 * arg_1, local_47, local_12->field_2c, local_12->field_1c,
      local_12->field_18, local_22, &response_fraction);
     vector3f local_50;
     function_1d24a2(&local_43, local_49, &local_50);
     if (local_46 - local_12->field_1c >= 0.0f)
     {
      real local_51 = function_1d24a6(&local_30, &local_24.matrix.forward);
      vector3f local_52;
      function_1d24a2(&local_24.matrix.forward, local_51, &local_52);
      local_52.i = local_30.i - local_52.i;
      local_52.j = local_30.j - local_52.j;
      local_52.k = local_30.k - local_52.k;
      vector3f local_53;
      function_1d24a2(&local_43, local_47, &local_53);
      function_1d24a4(&local_52, &local_53);
      real local_54 = magnitude3d(&local_52);
      if ((local_54 >= 0.0f ? local_54 : 0.0f-local_54) > 0.001f)
      {
       if (local_54 > local_12->field_28 * g_510c54->rate)
       {
        real local_55 = local_12->field_28 >= 0.0f ? local_12->field_28 : 0.0f-local_12->field_28;
        function_1d24a2(&local_52, 1.0f-(local_54-local_55*g_510c54->rate)/local_54, &local_52);
       }
       function_1d24a4(&local_50, &local_52);
      }
      if (!(local_3 > response_fraction)) local_3 = response_fraction;
      local_2 = true;
     }
     function_1d24a3(&local_25, &local_50);
    }
   }
   if (local_12->field_30 != 0.0f && local_12->field_34 != 0.0f)
   {
    real local_56;
    real local_57 = function_1d2280(true, local_18, havok_component_rigid_body_mass_get(local_15, local_14),
     10000.0f, local_12->field_30 * arg_1, function_1d24a6(&local_30, &local_24.matrix.forward),
     local_12->field_34, local_12->field_1c, local_12->field_18, local_22, &local_56);
    vector3f local_58;
    function_1d24a2(&local_24.matrix.forward, local_57, &local_58);
    function_1d24a3(&local_25, &local_58);
    if (!(local_3 > local_56)) local_3 = local_56;
    local_2 = true;
   }
   if (local_17) local_25.k += g_510c54->rate * g_51e9c4->unknown0;
   havok_component_rigid_body_linear_velocity_change(local_15, local_14, &local_25);
   if (!TEST_FIELD_BIT(local_14->flag1))
   {
    if (local_12->field_c != 0x7000001)
     function_b8d30(arg_0->object_index, local_12->field_c, &local_24, 1, false);
    vector3f local_59 = *g_4687a4;
    real coefficient_58 = local_12->field_58;
    real coefficient_5c = local_12->field_5c;
    if (coefficient_58 != 0.0f && coefficient_5c != 0.0f)
    {
     transform4x3f local_60;
     local_14->rigid_body_matrix_get(local_10->field_1, &local_60);
     function_142b80(&local_60.rotation, &local_60.rotation);
     matrix3x3 local_61;
     quaternionf local_62;
     function_141f60(function_142eb0(&local_60.rotation, &local_24.matrix.rotation, &local_61), &local_62);
     vector3f local_63;
     real local_64;
     function_11d790(&local_62, &local_64, &local_63);
     real local_65 = function_1d24a1(&local_63, &local_29);
     real local_67 = function_1d2280(true, local_18, havok_component_rigid_body_mass_get(local_10->field_1, local_14),
      local_64, coefficient_58 * arg_1, local_65, coefficient_5c, 0.0f,
      local_12->field_54, local_23, &response_fraction);
     function_1d24a2(&local_63, local_67, &local_59);
     vector3f local_68;
     function_1d24a2(&local_63, local_65, &local_68);
     vector3d_from_points3d((point3f const *)&local_68, (point3f const *)&local_29, &local_68);
     function_a75d0(&local_68, g_510c54->rate * coefficient_58);
     vector3f_add_scaled(&local_59, &local_68, -1.0f, &local_59);
     if (!(local_3 > response_fraction)) local_3 = response_fraction;
     local_2 = true;
    }
    function_1d24a3(&local_29, &local_59);
    havok_component_rigid_body_angular_velocity_set(local_10->field_1, local_14, &local_29);
   }
  }
 }
 if (arg_0->unknown94 && !arg_0->unknown94->size)
 {
  delete arg_0->unknown94;
  arg_0->unknown94 = NULL;
 }
 long local_69 = function_1d24a5((real)g_510c54->field_2_3 * 0.5f);
 long local_70 = function_1d24a5((real)g_510c54->field_2_3 * 0.5f);
 long local_71 = ((byte *)arg_0)[0x1a];
 long local_72 = ((byte *)arg_0)[0x1b];
 if (local_2)
 {
  real local_73 = local_3 < 0.0f ? 0.0f : local_3 > 1.0f ? 1.0f : local_3;
  long local_74 = (long)(local_73 * 255.0f);
  if (local_74 != local_72)
  {
   if (local_74 > local_72) local_72 += local_70;
   else local_72 -= local_70;
   if (local_72 > local_74) local_72 = local_74;
  }
  local_71 += local_69;
 }
 else
 {
  local_71 -= local_69;
  local_72 -= local_70;
 }
 ((byte *)arg_0)[0x1a] = (byte)(local_71 < 0 ? 0 : local_71 > 255 ? 255 : local_71);
 ((byte *)arg_0)[0x1b] = (byte)(local_72 < 0 ? 0 : local_72 > 255 ? 255 : local_72);
 if (local_0) arg_0->unknown04 |= 0x2000;
 else arg_0->unknown04 &= ~0x2000;
}
