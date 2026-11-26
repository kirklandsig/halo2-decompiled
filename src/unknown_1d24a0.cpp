#include "object_tag_definitions.h"
#include "unknown_11c920.h"
#include "unknown_1cec30.h"
#include "unknown_1eb550.h"
#include "object_markers.h"
#include "unknown_1428b0.h"
#include <math.h>
#include "vector_products.h"
// @flags /O2 /arch:SSE /Gr
#define MIN(a, b) ((a) > (b) ? (b) : (a))

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
 arg_2->i = arg_0->i * arg_1;
 arg_2->j = arg_0->j * arg_1;
 arg_2->k = arg_0->k * arg_1;
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
// score 808 (was 838): M3 lane GA r8 kept `function_142eb0(&local_24.matrix.rotation, &local_60.rotation, &local_61)`
// (behaviour fix: 0x142eb0 takes its right matrix in EDX and its left in ECX, and retail's call at 0x1d317e passes
// local_24's rotation in EDX and the transposed body rotation in ECX; main had the two swapped; 808 either way at m3
// review) and a `MIN` slew in the tail, which gives the 30 points (`if (local_74 > local_72) local_72 =
// MIN(local_72 + local_70, local_74); else local_72 = MIN(local_72 - local_70, local_74);`, matching retail's entry `push
// ebx` and entire 79-instruction tail `0x1d343a..0x1d3540`, 838 -> 808 plain, 4260/4259 B, aligned 247 -> 223, masked
// 123 -> 112); earlier 838 notes follow. Earlier first line: r143, m3 lane 8 r221, m3 lane 11 r152 and r158 unchanged (r152's line just above the marker); the 780 removal through the shared Havok array's remove_at lost matched 0x54590 in the full check (see the end of this note). Earlier first line: score 838: m3 lane 11 r141 reached 780 with the list local moved from the loop head to the removal (the head reading the component's field itself), not kept at review (a moved declaration, kept only on a byte match; lead in the r141 note below). Kept: the three rigid-body reads through the existing havok_component_rigid_body_get (838 alone, no rise). The r141 note shows the loop head is not blocked by 0xbadc0. Earlier 838 (was 895): m3 lane 11 r139 kept the validity tests nested around the body with the removal after it, the slot index read from the element, the scale helper as three statements, the motion position copied field by field and the angular residual scaled then added (see the end of this note). Earlier 895 (was 1014): round 116 kept the residual subtract in both limit arms and the record flags read from the field. Earlier at 949 (was 949): retail keeps the argument pointer in ECX and the field it reads in EAX, ours keeps the
// pointer in EAX and the field in ECX; the frame is 0x1f0 against retail's 0x1d4, the second activity flag sits
// at +0xa where ours has +9, and the record pointer and the counter sit at other homes.
// MiniMax A (11-20): no new form, no lower score (949); the first difference is still the argument's register (retail EAX, ours ECX) and the frame (retail 0x1d4 against our 0x1f0), so every later stack home differs, and the rolled scale loop stays unrolled into three multiplies; the cross-file causes stand (BLOCKED-BY 0x24590, 0xbadc0 and the contact array's destructor).
// Earlier score line: 949 (was 949): frame 0x1f0/0x1d4, entry/second activity and record/vector homes remain; earlier 949 (was 902), 902 (was 902).
// Frame 0x1f0/0x1d4 (ours 28 bytes larger), entry argument/flags in EAX/ECX against retail ECX/EAX,
// byte-flag homes and the record/vector homes still differ; the forms below were measured.
// MiniMax B (11-18): no new form, no lower score (949, was 949): the frame is still 0x1f0 against retail's
// 0x1d4, so every stack home differs; retail unrolls the scale helper into three multiplies with both
// vectors in memory while this build rolls the loop, retail reloads the delta from its stack copy after the
// 0x24590 call and keeps two subtract arms, and retail's loop head (the element pointer formed with
// `shl eax,3; add`, the object index in EDI across 0xbadc0) belongs to that callee's own file; the calls,
// the argument values and the float constants are unchanged.
// MiniMax H (11-14): no new form, no lower score (902): this round re-walked retail's listing block by
// block against the source; the conditions, fields, constants and callee arguments agree, so what is
// left opens with the frame and the entry's register roles, and the loop head's shape is settled in the
// callee 0xbadc0's own file; the body is unchanged.
// MiniMax E (11-14): no new form, no lower score (902): the frame is still 0x1f0 against retail's 0x1d4, so every
// stack home differs; retail pushes EBX, forms the element pointer with `shl eax,3; add esi,eax` and keeps the
// object index in EDI across the call to 0xbadc0, whose own file is off-limits here, so the loop head's shape and
// the extra frame slots stay as they are.
// MiniMax E (11-11): no new form, no lower score (902): the frame, the entry/second activity and the record/vector homes remain; the loop head's shape is decided by the callee 0xbadc0, which lives in another file: BLOCKED-BY 0xbadc0.
// MiniMax S (11-11): no new form, no lower score (902): the frame is 0x1f0 against retail's 0x1d4, and retail walks the element array through a computed pointer kept at [esp+0x64] where this build uses indexed addressing.
// MiniMax G (11-12): no new form, no lower score (902): the frame is still 0x1f0 against retail's 0x1d4, the entry flag byte
// still sits at frame+0x09 where retail has frame+0x0a, and the record bit 2 and the second activity byte are still the other
// way round; the loop head's shape is still decided by the callee 0xbadc0 in another file, so the body stands.
// MiniMax B (11-14): no new form, no lower score (902); the frame is 0x1f0 against retail's 0x1d4 (our frame is 28 bytes larger)
// and the difference opens in the first block, so every later home is out of step; the earlier rounds record the work as
// BLOCKED-BY the contact-array destructor, which lives in another file.
// MiniMax R (11-15): no new form, no lower score (902): the frame is still 0x1f0 against retail's 0x1d4, so every
// stack home below the top differs, and the loop head's shape is still decided by the callee 0xbadc0, whose file
// this lane may not edit.
// MiniMax S (11-16): no new form, no lower score (902); retail's scale helper is still unrolled while this build's loop is rolled, the frame stays 0x1f0 against retail's 0x1d4, and the contact array's destructor that blocks the unroll is set in another lane's header.
// Earlier score line: 949 (was 902): frame 0x1f0/0x1d4, entry/second activity and record/vector homes remain.
// MiniMax A (11-18): no new form, no lower score (949 in the reduced image): the first difference is still the argument's register (retail keeps the argument in ECX and tests the field in EAX, ours the other way round) and the frame (0x1f0 against retail's 0x1d4), so every stack home below the top is out of step.
// Earlier 949 (was 949): earlier 902 (was 902); Frame 0x1f0/0x1d4, entry/second activity and record/vector homes remain.
// Earlier score line: 902 (was 902): frame 0x1f0/0x1d4, entry/second activity and record/vector homes remain.
// Earlier at 949 (was 902): Frame 0x1f0/0x1d4, entry/second activity and record/vector homes remain.
// Earlier score line: 949 (was 949): earlier 902 (was 902); Frame 0x1f0/0x1d4, entry/second activity and record/vector homes remain.
// M3 lane 3 r268 (fast, control 949): the three entry flags renamed (time step, newly active, velocity changed) 949
// (same code: the flag bytes stay at +8/+9 where retail leaves +9 between them); the newly-active test with the
// flags-word bit first and the time-step flag second 997; the swap-remove copy reading `[--size]` in one statement 949
// (same code); the speed product written field first 949 (same code); the object-type mask computed at each of its two
// tests (no mask local) 1008 (4283 bytes). The three cross-file causes above (rolled scale loops behind the contact
// array's destructor, 0x24590 treated as clobbering, 0xbadc0 treated as writing memory) are unchanged and their files
// (src/unknown_1cec30.cpp, src/unknown_023540.cpp, src/unknown_0bad50.cpp) are avoided this round.
// Earlier at 902 (was 902): Frame 0x1f0/0x1d4, entry/second activity and record/vector homes remain.
// Walked against retail block by block (b2A, 11-06): conditions, fields, callees, argument order and loop/removal shape agree. One precision
// difference fixed: the angular block's x87 dot product (0x1d31a6-0x1d31f2) adds (i + k) + j, where the helper's other two sites (0x1d289c, 0x1d2c6d) add
// (k + j) + i. The source spells out (i*i + k*k) + j*j; ours adds k*k + i*i first and then j*j, the same grouping. Same score 902.
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
// Round 235: Full selected coefficient value, early bit-25/26 and late low-option subsets
// 998/990/990/993/993/998/998/990. All recover entry ECX/EAX and activity-byte home,
// but frame grows to 0x24c versus 0x1d4. Complete original body restored;
// entry/frame, activity and lower record/vector homes remain.
// DL227: Composed owner/other simulation-result
// byte-bool/word-bool/bool-byte/bool-word/byte-byte/byte-word/word-byte/word-word carriers:
// 902/929/902/982/902/982/929/980. Other-result word forms use frame 0x1ec at 982/980; original
// Boolean expressions restored; entry EAX/ECX, frame 0x1f0 and activity/record/vector homes
// remain.
// DL230: complete explicit add/subtract/standalone vector updates, seven subsets: 902/902/902/902/902/902/902/903.
// Last form also names every complete coefficient gate; original kernels and body restored.
// Round 345: Existing native setter for complete initial linear/angular change and fallback direction values, seven subsets; all with the existing staged displacement producer: 954/912/951/947/991/942/960/960.
// Original executable source restored; Entry EAX/ECX, frame 0x1f0/0x1d4, activity/record/vector homes remain.
// DH277: complete linked-component conditional/compound/nested/failure-first outcomes 901/901/902/992; each
// with result declaration before object lookup 901/901/902/992. One-point leads rejected; all original
// tests, fields, kernels and body restored.
// Round 77 (m3 lane 12, fast control 902): retail reads the element's object field once into EDI ahead of the
// record lookups, materialises the element pointer (`shl 3; add`) and spills it, and clamps the byte body slot
// from AL. A named object-index local read before the record lookups 951 (4283 bytes), after them 926; the first
// block's difference written straight into its normal (no staging copy) 961 (frame 0x1e4, 4235 bytes), with
// the late object-index local 966; the element pointer as `(s_1d24a0 *)data + index` 902 (same code). Original
// body restored.
// Y522: Existing body getter at owner activity/other activity/center, seven subsets
// 902/902/902/902/902/902/902; all with existing free matrix getter 902.
// Original body observations, center snapshot, kernels and complete force body restored.
// DL243: Actual escaped radial/axial/angle output role-name subsets, then the shared response
// name: all eight 902. Saved canonical vertical/mass/high-response option decisions in byte
// storage, seven subsets: 902/989/989/985/985/1030/1030; all four in word storage 1058. Original
// names, Boolean domains and complete body restored. Frame 0x1f0 versus 0x1d4, entry
// receiver/flags, activity byte and lower scalar/vector homes remain.
// DL248 (reduced): Existing scale returned outputs at immediate radial/planar/angular consumers,
// subsets 1..7 [902, 902, 902, 906, 902, 902, 906]; all with axial accumulation 906. Existing
// out-of-line quaternion/inverse/both returned-pointer chains, then all private/output chains [894,
// 899, 900, 904]. Frame 0x1f0 versus 0x1d4; entry receiver/flags, activity byte and lower scalar/vector
// homes remain. NOT kept: Private producer consumers tie or regress; quaternion-return chain 894
// removes the retail q-address copy and moves quaternion/body homes, so its eight-point preview gain is
// unsupported; other out-of-line chains fail the gain threshold. Original executable source restored.
// Round 128 (lane 8, fast control 902): retail reads the record's flag word from memory again for the bit-25/26
// flags and walks the element with a pointer (`[esi + 3]`). Tried: the flag word read at each use (no record_flags)
// 964 (4248 bytes); the element as `data + index` 902 (same code); the clamp test without the NONE-default
// local 917; the activity flag tested before the list pointer 933; the list read through arg_0 at each use (no
// list local) 955.
// Round 42 (m3 lane 2, fast control 902): mnemonic census against retail: ours ~10 rolled scale loops (dec 12/2,
// mulss 35/58, movaps 19/32); a long scale counter re-measured 933 and still rolled (mulss 35). The bit-13 test as
// `!(flags & 0x2000)` 939; the four record option bits as `(flags & mask) != 0` 902 (same code); the zero vector
// copied before `if (!local_21) continue;` 909; the shared response fraction declared at function scope 902; the
// body-slot clamp as a NONE default plus `if (local_14)` 917 (4283 bytes). Original body restored.
// Round 81 (m3 lane 12, fast): the record's object index named once (three uses) 926 (4283 bytes); the clamp,
// its test and the simulating query on the record's byte field directly 955 (4244); the second entry flag
// renamed 902 (same code); the record flag word read at each bit test 964 (4248); the entry flag as `>=` 1009.
// Round 79 (m3 lane 16, fast): the mass-center point copied without the pointer local 965 (4284 bytes); a
// block-scoped response fraction in each of the two blocks that share it 935; the clamp compared inline without
// the NONE-default count 917 (4283 bytes); the linked component as one ternary 901 (one point, not kept); the
// record flags read at each use 964 (4248 bytes). Retail keeps the array header in EBX across the loop (reloaded
// at the top, reused by the swap-remove block) and saves EBX; ours never uses EBX here.
// Round 144 (m3 lane 5, fast = full 902): local_17 renamed to sort after local_0 902 (local_1 stays at +9, retail
// +0xa); the body position read through its pointer (no copy) 965 (frame 0x1e4, 4216 bytes); the accumulations
// through the existing vector3f_add 902 (same code); the scale helper replaced by the existing vector3f_scale 1181
// (retail's frame 0x1d4, 4026 bytes: the vectors stay in registers); the helper's loop as `scale * component` 902.
// Original body and helpers restored.
// Round 49 (m3 lane 15, fast control 902): swap-remove as `data[i] = data[--size]` 902 (same code); the removed
// element overwritten through the element pointer 902 (same code); the linked-object test without the repeated
// `!= NONE` (the pointer is already NULL then) 961 (4251 bytes); owner activity as `local_1 || (...)` 989 (4248);
// other activity as `local_20 || ...` 970 (4254).
// P312: Wide-dot left/right and complete real-dot native value-formal masks 1/2/4/3/5/6/7,
// then immutable complete inputs: 1117/1117/1274/1250/1424/1395/1606/1606. NOT kept: no match;
// native input copies enlarge frames/extents. Original pointer kernels and complete body restored.
// Round 84 (m3 lane 9, fast control 902): retail's scale helper is unrolled with memory homes. In the compile-only
// harness (no /GL) our scale loops unroll, and the frame comes to retail's 0x1d4, only when the body has no destructor
// call: the contact array's `delete` (its inline destructor; every destructor spelling tried, inline, out-of-class,
// template or inside an inlined helper) stops VC unrolling every loop in the function. Under LTCG the delete removed
// (diagnostic only) still leaves the loops rolled, 956 (1072 also without the asm conversion), so a second cause that
// only LTCG shows remains. Forms: long counter 933 (rolled, cmp/jl); dword counter 933; the entry test wrapping the
// body (single exit) 902 (same code); the attached object index in a named local right after the element pointer
// (retail reads it first) 951; the attached component set in an if/else 901 (one point, not kept). All restored.
// DL260: Existing scale/add/subtract kernels with deduced actual vector input types, seven subsets, then existing planar kernel: 1087/1140/1099/1327/1320/1295/1505/1644.
// NOT kept: no exact native form; original complete executable source and every temporary contract restored.
// Round 129 (m3 lane 7, fast; aligned/masked control 548/232): the attached object index named before the lookups 951 (556/237), with the lookup nested under its test 977 (555/236), nested without the name 977 (551/235); removal as an else arm after
// the body 902 (same code); position copied by field 947 (4283 B, 530/219), both 947. Register-masked 366 by line: retail's lower vector/scalar homes still rest on the rolled scale loops; the `inline` lead on 0x24590 stays a coordinator question (it raises other callers).
// M383: Absolute-value APIs at entry/radial/planar bound families, subsets 1/2/4/3/5/6/7,
// then all double APIs with real results: 900/927/944/931/945/974/975/974. NOT kept:
// two-point entry lead fails the gate; native conditional bounds, helpers and full body restored.
// Round 148 (m3 lane 6, fast; aligned in brackets, control 902 [681]): HELD LEAD, needs a ruling on a shared helper: `inline` on
// magnitude3d's definition (0x24590) makes our build spill live xmm values across that call as retail does. Recipe, 849 [384],
// 4241 bytes: the scale helper as three statements (retail's frame 0x1d4 and calls), the rigid-body position copied field by
// field, the subtract in both arms at the two limit sites, the record flags read from the field at each pair (second-bit flag
// first), the removal path as an else arm after the body with no long index local at the test, `inline` on 0x24590. On the way:
// three statements 1181 (4026) [546], with n[] the same, through local pointers 1174; position copy 1173 [539]; both arms 1144
// (4070); `inline` alone 934 (4299) [707]; all with it 956 (4252) [415]; flag reads 897 (4241); product operand swaps 897; else
// arm 861 [395]; attached index first 987; coefficients at each use 893 (4211) [366], frame 0x1cc; `!(a > b)` limit form 863;
// flag order second pair 862; rate product swap 861; local copy field by field 964. Without `inline`: that recipe 1069 (4068)
// [546]; file-local inline or forceinline declaration 1069; each form alone on the body below 964/955/947/906/968/977/926, so
// none is keepable. Plain check of the callers with `inline`, nothing lost, 0x1d24a0 861, but 0xf8eb0 946->1030, 0xd1e10 542->596,
// 0x185be0 622->958, 0x1aa0d0 375->402, 0x2c0d60 882->952, 0xf6d10 749->759 (aligned mixed: three fall, three rise).
// M3 lane 10 r73 (fast; aligned/masked in brackets, control 902 [681/535]): an empty else arm on the body block 1002 (4283) [690/542]; three-statement
// scale helper re-measured 1181 (4026) [546/436], with that else arm 1185 [555/440], as plain `inline` 1181; one flags local for both entry tests 926
// (4283) [686/546]; the element read through its array expression at the attached-object test 902 (same code); response fraction as a struct member
// through a reference 926 (4283) [686/546]; const record flags 902 (same code). Two untried leads from matched 0x247fd0: `lea dst, [idx + base]` where ours
// has `add idx, base; mov dst, idx` is the sum written twice; and an x87 store followed by a split memory compare wants the existing cross-file helper.
// M383: Absolute-value APIs at entry/radial/planar bound families, subsets 1/2/4/3/5/6/7,
// then all double APIs with real results: 900/927/944/931/945/974/975/974. NOT kept:
// two-point entry lead fails the gate; native conditional bounds, helpers and full body restored.
// T61: Actual wide-dot left/right/both and real-dot left/right/both reference inputs,
// both complete dot interfaces, then both with planar input references: eight at 902.
// NOT kept: native input identities do not recover the entry pair or frame; original kernels
// and complete body restored.
// MiniMax E (11-04): the residual subtract in both limit arms 1065, the subtract helper taking a scale in both arms 1017 and with a scale variable 1009, the contact array hoisted out of the loop 1004, the fused scaled subtract in both limit arms 1016 and in the radial arm only 996 or the planar arm only 1017; no lower score.
// M3 lane 16 r91 (fast; aligned unmatched in brackets, control 902 [548]): retail's in-loop byte homes are record bit 2
// at +9 and the second activity flag at +0xa (ours the other way round); record bit 2 renamed to sort before the activity
// flag 902 (same code: names do not move these homes); the clamp tested in the condition after the null test (no clamped
// local) 917 (4283 B) [552]; the removal as one `data[i] = data[--size]` statement 902 (same code); the body position
// copied without its pointer local 965 (4284 B) [612]; the record flags read from the field at each use (retail reads
// the word twice) 964 (4248 B) [547]. Retail also keeps the record element as a pointer (`shl eax, 3; add esi, eax`) and
// the attached object index in EDI across the lookup, where ours re-reads `[ebx + edi*8 + 4]` three times.
// M3 lane 12 r93 (fast): LEAD, the unroll blocker found: under LTCG the scale helper's loop unrolls only with a
// `long` counter (a `short` one stays rolled even alone in a three-line body) and only when the function calls no
// destructor (`delete` of the contact array, its explicit destructor call, plain or inside a helper all keep every
// loop rolled; a non-destructor member doing the same frees unrolls them). Long counter with the frees written out
// (the destructor body by hand, diagnostic) 1174 at 4033 bytes but retail's frame 0x1d4, entry pair and scale shape
// (difflib-matched lines 518 against 444 here); with a release member in the header 1168. Forms on this body: explicit
// destructor call plus operator delete 925, with the long counter 930; long counter alone 933 (listed); a byte local
// renamed on the unrolled body 1174 (same code). Next: retail frees the list with no destructor call, so the contact
// array type wants a plain free path (all three users, 0x1cf9f0 and 0x1d2070 matched); the asm conversion is no blocker.
// M3 lane 12 r108 (fast): `local_0 = fabs(arg_1) >= 0.0001f` 1009 (4299 B; round 81's `>=` form, a re-measure); the attached object as a NULL default with `if
// (field_4 != NONE)` around the lookup 902 (masked aligned 538 against 535); the final free through a `contacts` local for the
// test, size and delete 917 (4283 B); the three rigid-body reads through the existing havok_component_rigid_body_get 917 (4283
// B), the owner's alone 902 (same code), the body position's alone 902 (same code), the simulating test's alone 917 (4283 B). Retail frees the list with the destructor body and operator delete inline (0x1d33f9),
// so the unroll blocker is the destructor itself; its other users (0x1cf9f0, 0x1d2070) sit in src/unknown_1cec30.cpp, held by
// another lane this round. Restored.
// M3 lane 3 r225 (fast = plain 902; aligned [exact, stack-masked, register-masked] base [688, 620, 370]): the type mask through the
// existing object-header element (`1 << header->type`) 902 (same code; retail loads the byte with movzx); the mask as a long 902
// (same code); the position copy without its one-use pointer 965 (4284 bytes) [784, 908, 379]; the position read through a pointer
// with no copy 965 (4216, frame 0x1e4) [642, 591, 369]; that plus the scale helper as three statements 1246 (3971, frame 0x1c8)
// [570, 471, 323]; the three statements alone re-measured 1184 (4023, frame 0x1d4) [557, 471, 338]. Retail walks the element
// array through a computed pointer kept at [esp+0x64] (shl/add) where ours uses indexed addressing, and saves EBX at entry.
// Lane 5 r166 (fast alone, control 902): the repeated `field_4 != NONE` dropped from the attached-component test 960 (4251
// bytes); the second read of field_1 into local_15 dropped 984; a named contacts pointer for the final free 917 (4283); local_3
// declared before local_2 902; the attached component chosen by a ternary 901 (one point, not kept); the object lookup as
// `local_13 = NULL; if (...) local_13 = ...` 902; the entry flag as `fabs(arg_1) >= 0.0001f` 1009 (4299); the two field tests
// nested 902; a short loop index 989 (4272); response_fraction declared at the top 902; the contact array pointer read
// once before the loop and used in its test 1004 (4287). All restored. The unroll lead
// (no destructor call) needs the contact array type in include/unknown_1cec30.h, whose other users 0x1cf9f0 and 0x1d2070
// sit in src/unknown_1cec30.cpp (another lane's file this round).
// m3 lane 14 r77 (fast; difflib counts with registers and offsets masked, control 232): retail never tests the attached
// component pointer after its lookup and places the swap-remove block after the body: the validity tests (object index,
// object, component index) as one `if` around the lookup, the slot clamp compared in a nested `if` around the body, the
// body ending in `continue;` and the removal after both, 964 (4267 B, masked 228; close to round 148's removal-after-body
// part, whose own score the note does not single out); on it the clamped slot named again 964, the component pointer
// declared ahead of the tests 964 (both the same code), the record flags read from the field at each bit 972 (4248 B,
// masked 226), the rigid-body position copied field by field (retail's movss copy) 1004 (4283 B, masked 218). Diagnostics (not keepable): a long scale counter with the
// contact-array frees written out 1177 (4030 B, masked 207: listed as 1174), both with the nested tests 1199 (4022 B,
// masked 201). So the nested validity tests and the unrolled scale loops are independent structural fixes.
// MiniMax D (11-10): no new form, no lower score: the removal path as an else arm after the body 902 (4267 bytes),
// the entry flag word in a named local 917, the motion point copied member by member 943 (4283 bytes); all reverted.
// Round 169 (m3 lane 6, fast; control 902): retail reads the record's option word twice (bits 2 and 1, then 25 and 26
// after the two simulating calls), keeps the element pointer in ESI (`shl 3; add`) with the array in EBX, and keeps the
// element's object index in EDI across the 0x0badc0 call. Re-measured on this body: the option word read at each of the
// four flags 971 (4232 bytes; frame unchanged); the attached object index in a three-use local 917 (4283 bytes). New: the
// clamp folded into the removal test 902 (same code); the element's field_1 read at each use with local_15 defined at
// its later assignment 956 (4244 bytes); `local_1 = local_0` cleared by the bit-13 test 941; the element array as a
// named typed local for the element and the removal copy 917 (4283 bytes).
// M3 lane 1 r90 (fast with 0x1152e0, 0x11bce0, 0x1d2280, 0x24590; aligned/register-masked counts in brackets, control 902
// [657/381]): BLOCKED-BY 0xbadc0 for the loop head. Diagnostic, reverted at once: an opaque library call right after the
// attached-object lookup gives retail's head exactly (array header in EBX, element pointer formed by `shl eax, 3; add`, then
// spilled) 979 [686/373], so retail's link treats 0xbadc0 as writing memory (the same held lead as 0xc86e0's; its file
// src/unknown_0bad50.cpp is not ours to edit this round). Forms: the attached-object lookup moved ahead of the record
// lookups 975 [667/399]; the one-use shape pointer local_11 folded into local_12 902 (same code); the angular pair
// (field_58/field_5c) read from the record at each use 906 [653/375] (4250 B). On lane 14 r77's nested validity body
// (re-measured 964 [615/378], 4267 B): the angular pair read at each use 993 [611/376] (4234 B); local_11 folded 964 (same
// code); the attached-object lookup ahead of the record lookups 973 [634/395]. Lane 10 r73's two leads
// from 0x247fd0 do not apply: neither listing has an `add`/`mov` pair or a two-register `lea` now, and the x87 store with its
// split compare after 0x30bf0 is the same on both sides. The unmatched callee 0x1d2280 (26) uses XMM0-XMM7 as retail, so
// its register use does not explain the caller's spills. The other single-point forms I found are already listed;
// the larger causes are cross-file: the rolled scale loops (destructor blocker, include/unknown_1cec30.h) and
// magnitude3d's `inline` (held).
// MiniMax B (11-14, run 75): no new form, no lower score (902); the body is left as it was. What still differs: retail's scale helper is
// unrolled into three `mulss` and keeps both vectors in memory, while this build's rolled loop walks a counter and leaves a 0x1f0 frame
// against retail's 0x1d4; a by-value helper that returns the scaled vector (measured this round) is worse at 1555 (4795 bytes), and the
// earlier unrolled pointer form is worse at 1199 (4008 bytes).
// M3 lane 10 r106 (fast; masked aligned count in brackets, base 366): the matrix product and quaternion conversion as
// two statements (no nested returned pointer) 917 [367] (4283 B); the slot index read into its local ahead of the
// attached-object lookup 946 [359] (4244 B); the other three solver coefficients as `arg_1 * field` 902 (same code); the
// component and slot locals renamed to the other sort order 902 (same code); the angular pair read from the record at
// each use (retail's [ebp+0x58]/[ebp+0x5c]) with a long scale counter 950 [357] (4220 B). Retail re-makes its zero
// (`xorps`) and reloads every live XMM value after both 0x24590 calls although 0x24590 is pure x87.
// MiniMax D (11-17): no new form, no lower score (902): the first difference is still the argument's
// register (retail reads it into EAX and tests EAX, ours into ECX) and the frame (retail 0x1d4 against our
// 0x1f0), so every later stack home differs by four bytes; retail's rolled scale loop is also still
// unrolled into three multiplies with both vectors in memory, which is what decides the frame size here.
// M3 lane 11 r115 (fast with 0x1cf9f0, 0x1d2070, 0x1d1260, 0x24590, 0x1d2280; the baseline is 949 since main made 0x1d1010 a
// member; aligned unmatched [exact/stack-masked/register-masked], base [528/391/229]): the unroll lead with the header free.
// The contact array freed through a plain release member in place of its destructor and operator delete, at all three users,
// with a long scale counter 1170 (4032 bytes, frame 0x1d4) [450/316/206], but 0x1d2070 loses its match (32, 510 bytes: its
// `delete` carries a null test the member lacks). The release member beside the destructor, used here only: 1170 (4032
// bytes) [450/316/206], 0x1cf9f0 and 0x1d2070 still matched. On that, `inline` on 0x24590's definition 1044 (4171 bytes)
// [536/253/144]; with the rigid-body position copied field by field 1033 (4176 bytes) [435/214/121]; with the record flags
// read from the field at each pair as well 1033 (4156 bytes) [434/213/120]. Positionally every one is above the base (the
// body is then 83 to 103 bytes short, mostly the loop head that 0xbadc0 decides), so none is kept; aligned, the three
// cross-file causes (destructor-free release, `inline` 0x24590, opaque 0xbadc0) account for about half of what differs.
// Body, header and both other files restored.
// M3 lane 4 r222 (fast with 0x1cf9f0, 0x1d2070, 0x1d1260, 0x24590 matched and 0x1d2280 26 throughout; control 949, aligned 689):
// the component flags read once into a `dword flags` for the entry test and the bit-13 test 962 (4283 B), as `long` 962;
// the attached-object lookup as `local_13 = NULL; if (field_4 != NONE) local_13 = ...` 949 (same code); the later
// `field_4 != NONE &&` dropped from the component test (local_13 is NULL then) 982; the slot clamp replaced by its range
// test 964 (retail computes the clamp, `movsx`/`jle`/`mov ecx, edx`, so not retail's form). The cross-file causes stand:
// 0x24590 and 0xbadc0 (both files held this round) and the contact array's destructor.
// M3 lane 13 r115 (fast, all restored): the body as one `if (flags & 0x104000) { ... }` with no early return 949 (same
// code); the flags word read once into a named local for both entry tests 962 (4283 B); the was-active bit tested as
// `!(flags & 0x2000)` 951; the activity flag and response maximum declared before the two entry flags 956; the
// contact array pointer read once before the loop test 1003 (4287 B).
// M3 lane 6 r205 (fast with 0x1cf9f0, 0x1d2070, 0x1d1260, 0x24590, 0x1d2280, 0x1152e0, 0x11bce0; control 949, aligned unmatched
// [exact/stack-masked] [528/391]): the flags word in a named local for both entry tests re-measured 962 [532/395]; the
// validity tests nested around the body with the removal after it re-measured on this baseline 964 [533/405]; that with the
// slot byte read from the element at each use and the long slot local defined at its later assignment (retail keeps the
// byte in AL and widens it at the clamp, the compare and the first body lookup) 974 [540/407]; that with the record flags
// read from the field at each bit 983 [538/405]; the entry test as two shifted bit tests of the flags word 1011 (4283 B)
// [524/391] (still `test ecx, 0x104000` with the component in EAX where retail has EAX and ECX the other way round). Retail
// saves EBX with ESI and EDI for the whole function and only EBP around the loop; ours saves EBX with EBP around the loop,
// which is one instruction of shift from the third push to the loop head. All restored.
// MiniMax B (11-19): no new form, no lower score (949): the frame is still 0x1f0 against retail's 0x1d4 and the first
// difference is still the argument's register (retail keeps the argument in ECX and tests the field in EAX, this build
// the other way round), so every stack home below the top is out of step; the loop head's shape is decided by the callee
// 0xbadc0, which lives in another file - BLOCKED-BY 0xbadc0.
// M3 lane 5 r214 (fast; control 949, aligned 528): record bit 1 declared before bit 2 949 (aligned 521); local_2 and
// local_3 declared ahead of the entry flags 956; the activity flag as `!bit13 && local_0` 997; the activity flag as a
// copy of local_0 cleared when bit 13 is set 994 (retail's entry registers, first difference +0x9, but the flag takes
// its own byte at +3); the record loop counter declared at block scope 949 (same code). Both builds are in asm
// slot-ordering mode (inlined fistp helper, cvttss2si).
// M3 lane 15 r116 (fast = plain; control 1014 at 4299 B, frame 0x1f4): main's count rose from 949 when lane 8 r201 put
// `inline` on magnitude3d's definition (0x24590; the old file swapped into the fast image gives 949 again), the round 148
// held lead's condition. Kept: the residual subtract written in both arms at the two limit sites (retail keeps two subtract
// blocks) and the record flags read from the field at each bit (no flags local): 895 (4278 B, aligned 670, frame 0x1f0).
// LEAD (no count change, so not kept): with the scale helper as three statements (retail's unrolled multiplies) and the motion
// position copied field by field (retail's three movss) as well, 895 at 4240 B, frame 0x1dc, aligned 405 (plain 895).
// Steps (fast): subtract alone 951; helper alone 1048 (4167 B); helper with the subtract 958; with the field copy as well
// 952; all four 895; the scale helper with the subtract and the field reads 921; plus the second-bit flag declared first 895 (same as without); plus the
// removal as an else arm after the body with the clamp test written on the field 898 (aligned 377), with the slot locals
// kept 895 (same code). Not kept: the first block's offset through the existing vector3d_from_points3d with no staging copy
// 948 on the old body (frame 0x1e8), 1028 with the kept forms (frame 0x1cc); the angular block's slot index through local_15
// 944 (aligned 736: retail reads the field there); both 964; each block's response fraction in its own block 1012; the
// motion-position pointer dropped 1017; the two in-place differences through vector3d_from_points3d 948 (same code); the
// attached object index named at its tests 895 (same code), named right after the element pointer 901; the angular
// coefficients read from the record at each use 916 (4210 B, aligned 348). Left: retail pushes EBX at entry and keeps the
// element pointer (shl/add) in a stack home with the attached index in EDI; the frame is 8 bytes over retail's 0x1d4.
// M3 lane 11 r139 (h2m fast = check.py --fast = plain; aligned unmatched [exact/stack-masked/register-masked], control 895
// [498/375/209]): kept, 895 -> 838 at 4255 B [340/196/92], first difference +0x9 (retail's entry registers), frame 0x1d8:
// the scale helper as three statements and the motion position copied field by field (round 116's lead, re-measured 895
// at 4240 B [352/184/104]); the validity tests (attached index, object, component index) as one `if` around the component
// lookup with the slot clamp nested in it, the body ending in `continue` and the swap-remove after both (retail jumps
// straight to the removal and never tests the component pointer); the slot index read from the element at the clamp and
// at the first rigid-body lookup, its long local defined where retail re-reads it after the object-type calls (retail
// keeps the byte in AL at the clamp and re-reads `[element + 1]`); the angular residual scaled by -1.0 and then added
// through the file's scale and add helpers (retail multiplies by -1.0 and adds; the fused add-scaled call folds to a
// subtract). Steps on the lead: slot from the field 898 [323/185/104]; slot in a signed-char local 914; nested tests with
// the long slot local at the clamp 980; nested with the field 857 [344/206/102]; nested with a signed-char local 862. On
// 857: the scale helper's products as `scale * component` 857 (same code); the five coefficient and rate products with
// their sides swapped, one at a time and together, 857 each (same code: the compiler orders them itself); one, two or
// three redundant paren pairs around the first block's x difference 862/871/857 and around the first solver product
// 857/862/871 (a three-state phase, each state flips other product pairs; not keepable); the angular coefficients read
// from the record at each use (retail's [record + 0x58] and [record + 0x5c] reads) 893 at 4205 B [322/199/98]; the
// scale-then-add alone 838; both 871 at 4225 B [312/189/88], frame 0x1cc (the lead for the next round). On 838: the
// aggregate position copy again 868; the helper's loop again 1033; the attached index in a named local after the element
// pointer 973, after the record pointer 970; the slot index read from the element at every use (no local) 938 (first
// difference back to +0x1). Diagnostic, reverted at once: an opaque call after the attached-object lookup on the 871 form
// gives retail's loop head (list in EBX, element pointer formed by shl/add and spilled) 899 [319/190/75], so BLOCKED-BY
// 0xbadc0 stands (its file is another lane's this round), and with it the attached index wants a named local (retail
// keeps it in EDI across the call). Left on 838 besides that head: the operand order of the float products and sums
// (retail loads the vector component and multiplies by the scale in the first two blocks, the other way round in the
// third), the object-type byte read with movzx, EBX saved at entry, and ECX/EDX swapped in the tail's level arithmetic.
// Also on 838: the object-type mask computed at each of its two tests from the byte field (no mask local) 852 (4259 B,
// retail's length; still `mov cl`); from a long type local 834 (4256 B, retail's `movzx` read; under the bar and a
// computed value written out at each use, so not kept).
// M3 lane 11 r141 (h2m fast = check.py --fast = plain; aligned unmatched [exact/stack-masked/register-masked], control 838
// [340/196/92]): kept, 838 -> 780 at 4264 B [282/139/70]. The loop head was not blocked by 0xbadc0: retail keeps the list
// in EBX from the loop top to the removal and re-reads its data pointer there, which is the component's list field read
// once at the loop top (for the element pointer) and once more at the removal into the list local (four uses there):
// 786 [287/145/73], with retail's head (`shl eax, 3`, the attached index in EDI, `add esi, eax`, the element pointer
// spilled) and retail's removal block byte for byte. The same code from a private swap-remove helper taking the list
// (786, a new one-caller helper, not needed) and from the old top-of-loop local with the field read again at the removal
// (786). Other removal spellings on the 838 body: only the size decrement through the component's field 788 (head
// retail's, removal 5 rows off); the decrement inside the index expression through the field 932; every removal read
// through the field 932 (4281 B, the list is reloaded after the size store); no list local at all 932 [323/166/79]; the
// copy through the element pointer 838 (same code), with the decrement through the field 956; the list local at
// function scope, set before the entry test, or also used by the final free 838 each (same code); the counter at
// function scope 838; a named data pointer 838. On 786: the second simulating query through the existing
// havok_component_rigid_body_get 780 (retail forms the body pointer before the result-buffer push), the owner body and
// the motion position through it too 780 (same code; all three kept); the record's bit-1 flag declared ahead of its
// bit-2 flag 773, with the getter 767 (retail copies the flags word for bit 1 and shifts the original for bit 2, as it
// does for bits 25 and 26 in that order; a declaration order, so not kept); the angular coefficients read from the
// record at each use 815 at 4231 B [272/138/69], frame 0x1cc (retail's reads, positional rise), with the bit-1 flag
// first 804. On 780: the list local at function scope (declared only, or set for the entry test) 780 (same code); the
// scale of the first limit arm in a named local 793 [305/163/74], of the second 780 (same code), both 793; the
// object-type byte in a long local 780 (same code). Tail forms on the 838 body (same code unless a count is given): the
// step arms as a ternary, with `<=` and the arms swapped, with the compare turned round; the two levels declared in
// the other order; the clamps in named locals; the else arm's two steps or the two step locals in the other order; the
// final clamp as a ternary; the first clamp testing the upper bound first 834 (4259 B); the first level's step ahead of
// the target test 807 at retail's 4259 B (not kept: a statement order, and the tail is then not retail's; it is the one
// form that uses EBX outside the loop, which moves `push ebx` to the entry as retail has it).
// Left on 780: retail saves EBX with ESI and EDI at the entry though it uses EBX only inside the loop, exactly where
// this build does (the same 18 rows), and this build saves it around the loop with EBP (one row of shift from the
// third push to the loop, and one at the exit); the frame is 0x1d8 against 0x1d4 with the two angular coefficient
// homes retail does not have; the flag pairs' copy roles and the argument loads at the marker calls; `mov cl` against
// retail's `movzx` for the object type; the operand sides of about a dozen float products (retail loads the vector
// component and multiplies by the scale at both limit arms); and the tail (retail keeps the first level in EDX with its
// clamp result in ECX, and loads the second step into ESI only in the add arm).
// MiniMax H (11-20): no new form, no lower score (895): the frame is still 0x1f0 against retail's 0x1d4 and the first
// difference is still the argument's register (retail keeps the argument in ECX and tests the field in EAX, this build
// the other way round), so the stack homes below the top stay out of step: BLOCKED-BY 0xbadc0 in another file.
// M3 lane 3 r289 (fast, single address; control 838): 0..35 unused initialised locals at the top 838 each (no symbol-count
// state here); the two simulating bools declared at function scope 838; the marker record at function scope 838; both 838;
// the record's bit-2 and bit-1 bools at function scope 838 (same code each: scope does not move `push ebx`, which retail has
// with ESI/EDI at the entry and uses only for the loop's list, the simulating bool and the EBX/BL arguments of 0xb8d30 and
// 0x1d2280); the attached-object test without the repeated NONE test of the element's index 970 (4244 B); the first
// simulating bool as `local_1 || (...)` 845; the two step levels declared in the other order 838; the response fraction
// declared ahead of the first zero vector 838; the element pointer as `data + index` 838; the removal's copy through the
// element pointer 838 again. Body unchanged.
// M3 lane 1 r143 (fast = check.py --fast; aligned [exact/stack-masked/register-masked]): on 838 the angular coefficients read
// from the record at each use 871 (4225 B). The removal as the existing `remove_at` of the shared Havok array members
// (s_havok_array08 declared with ELEMENTS_2D91D0_MEMBERS in include/unknown_1cec30.h, the same data/size/capacity layout),
// called on `arg_0->unknown94`, with the loop head reading the component's field (no list local) 780 [282/139/70] at
// 4264 B, r141's code with no moved declaration. Not kept: the full check then loses matched 0x54590 (and flips 0xc2d00
// to matched); with the macros moved into a header free of the memory classes 0x54590 is still lost, while r141's
// hand-written form with the headers untouched passes the full check (12325 matched, nothing lost). So the members
// added to that header, not the 780 code, move 0x54590 (the include of the memory classes alone moved 0x185be0 60 -> 62).
// On 780: one flags word local for the four record bit tests 813 (4270 B), for the bit-2/bit-1 pair only 780;
// `field_20 * arg_1` for `arg_1 * field_20` 780; the shared vector3f_scale for the private scale copy at all eleven
// calls 780; the shared dot3f for the private copy 780 (each the same code). Left at 780 as r141 lists: EBX saved around
// the loop with EBP (retail at the entry), the frame 0x1d8 against 0x1d4, the flag pairs' copy roles, `mov cl` for
// retail's `movzx`, product sides, and the tail's ECX/EDX rotation (ours tests the response flag through DL, retail CL).
// M3 lane 11 r147 (fast, single address, control 838): the entry test as one `if` around the body in place of the early
// return 838 (same code: EBX is still saved around the loop, retail saves it with ESI and EDI); the removal in
// src/unknown_1cec30.cpp's form `data[i] = data[--size]` on the array's own element type 838; the element pointer as a
// cast of `&list->data[i]` 838; the final free through a named list local 838 (all the same code); the half rate named
// once for both rounding calls 869 (4230 B: retail converts it twice). Body unchanged.
// M3 lane 4 r244 (h2m fast; control 838; body unchanged): the removal in the sibling contact-removal function's form
// (`data[i] = data[--list->size]` through the list local) 838; the object-type byte read into a long local for the shift
// 838 (still `mov cl`, the table data still in EDI where retail reuses EDX); local_2 and local_3 initialised ahead of
// local_0/local_1 851; the two angular coefficients bound `real const &` to the record 871 (4225 B, as the reads at each
// use); the element pointer as `data + index` 838 (same code). Reading: retail's head forms and spills the element pointer
// because its data pointer is not kept across the attached-object lookup to the removal; ours keeps it in EBX for that path.
// M3 lane 13 r140 (H2M_FAST, single address; control 838, difflib-aligned 340 with call targets masked): the angular
// coefficient field_5c read from the record at its two uses (field_58's local kept) 843 at 4242 B (aligned 317): the frame is
// then retail's 0x1d4 and the first difference moves to +0x14 (left there: `push ebx` at the entry, which this body saves around
// the loop); field_58 read at each use with field_5c's local kept 857 (4238 B, frame 0x1d4, aligned 316); all four response
// fractions through the one `response_fraction` local 876 (4243 B), with field_5c at each use 888. Lead (match-only, a statement
// order): field_5c at each use plus the first level's step ahead of the target test (lane 11 r141's 807 form) 809 at 4245 B,
// aligned 299: the whole entry block, frame and `push ebx` are then retail's to the loop head, where this body still keeps the
// element list's data pointer in a register (retail re-reads it; r141's 780 removal form). With r141's list local moved to the
// removal as well 739 at 4254 B (aligned 239; three match-only forms together). Body unchanged.
// M3 lane 8 r221 (full checks unless marked). The removal as the
// shared Havok array's existing `remove_at` (s_havok_array08 typed in include/unknown_1cec30.h with the existing
// growable-array members macro, the loop head reading the component's field) 780 (h2m fast and full, 4264 B), but the
// full check loses matched 0x54590 (two loads swapped at +0xff) and gains 0xc2d00, 0x24cf66 114 -> 115; with all four
// arrays of that header typed the same way (a second change, as the coordinator advises for a lone 0x54590 loss) the
// identical result. Diagnostic: lane 11 r141's hand-written 780 body with the header untouched loses nothing (only
// 0x1d24a0 moves), so the header's members, not the body, flip the 0x54590/0xc2d00 pair. Restored. On main's body
// (h2m fast): the type mask as `long` 838 and the four running-maximum updates as conditionals 838 (same code each).
// M3 lane 7 r215 (H2M_FAST, single address; control 838, difflib-aligned 386 here). Lead, under the keep bar (4
// points): the object-type byte in a `long` local read by both mask tests (`(1 << type) & 1`, `(1 << type) & 0x1000`,
// no mask local) 834 at 4256 B: the dispatch block is then retail's byte for byte (`movzx ecx`, the header table in
// EDX; aligned 384). The two tests written on the byte itself with no local 852 (4259 B: `lea` + `mov cl`); the mask
// local with a `(long)` cast on the byte 838 (same code). With the type local and field_5c read at its two uses (lane
// 13 r140's frame form, 843 alone at 4242 B) 838 at 4243 B, frame 0x1d4, first difference +0x14, aligned 365. Diagnostic,
// restored at once: an indirect call right after the attached-object lookup (0xbadc0 made opaque) 820 at 4262 B, aligned
// 333: the list is then in EBX with the element pointer formed by `shl eax, 3; add` as retail, but EBX is still saved
// with EBP around the loop and the frame is still 0x1d8, so neither follows from 0xbadc0's write summary; with the
// type local too 864 (4261 B). Body unchanged.
// M3 lane 11 r152 (h2m fast; control 838). Retail re-reads both angular coefficients at each use (the test, the
// 0x1d2280 arguments, the 0xa75d0 product) and gives the four 0x1d2280 fraction outputs four homes (+0x108, +0xec,
// +0xd8, +0x3c), where this body shares one fraction local between the second limit block and the angular block.
// The coefficients read at each use 871 (4225 B, frame 0x1cc); the shared fraction split into one per block 836
// (frame 0x1dc); both 848 (frame 0x1d4 = retail's, first difference `push ebx`). With the removal through the
// existing remove_at (the array typed with the growable-array members, the head reading the field): alone 780,
// with the reads 808, with the split 776, with both 775 (full check too: the loop head, frame and fraction homes are
// retail's), but the full check again loses matched 0x54590 and gains 0xc2d00, so not kept. Body unchanged.
// M3 lane 7 r218 (H2M_FAST, single address; control 838, difflib-aligned 386): r215's object-type long local (the mask
// written at both tests) with the angular block's fraction split off into its own local 831 (4256 B, aligned 339; 7
// points and the mask computed at each use, so not kept); that with the angular coefficients read at each use 843
// (4226 B, aligned 267, first difference +0x14); the two level bytes read ahead of the two step conversions 837 (a
// statement order, one point); the swap-remove through a named elements pointer 885 (4259 B); the second limit block's
// fraction split off instead of the angular block's 836 (aligned 341, the same count as lane 11 r152's split). The loop
// head stays BLOCKED-BY 0xbadc0 (its file is on this round's avoid list). Body unchanged.
// M3 lane 11 r158 (h2m fast, single address; control 838): the angular coefficients read at each use with one fraction
// local per block (r152's 848, re-measured at 4225 B, frame 0x1d4, first difference `push ebx`); on it the loop counter
// at function scope 848, the list local at function scope set before the entry test and used by it 848 (same code
// each), the object type in a long local for both mask tests 843 (4226 B), one flags-word local for the four record bit
// tests 884 (4230 B); on the body `if (!local_21) continue;` as a block around the rest 838 (same code). Body unchanged.
// M3 lane 3 r302 (H2M_FAST, single address; control 838 at 4255 B, difflib-aligned exact/register-masked 386/299): the
// list bound as `s_havok_array08 *const &` to the component's field 927 (4277 B); the element as `s_1d24a0 &` 838 (same
// code); main's mix of `local_15` and direct `field_1` reads made uniform: the field at every use 934 (4271 B), the local at
// every later use 862 (4253 B). One `real response_fraction;` declared inside each of the two blocks that share it (the
// same name in both scopes; retail gives the four fraction outputs four homes) 836 (aligned 341/260: a structural gain of
// 45 rows, 2 points positional, under the bar); with both angular coefficients read at each use 848 at 4225 B (aligned
// 269/172, frame 0x1d4 = retail's, first difference `push ebx`), with only field_5c at each use 848 (4242 B, 332/246).
// Lead (match-only, a moved declaration): those two plus lane 11 r141's list local at the removal (the head reading the
// field) 775 at 4234 B, aligned 206/149, the same 775 as r152's remove_at form but with no header change. The loop head
// stays BLOCKED-BY 0xbadc0's write summary (src/unknown_0bad50.cpp, on this round's avoid list). Body unchanged.
// M3 lane 7 r224 (fast screen, single address; control 838 at 4255 B, aligned exact / register-and-slot-masked 386/123).
// Rebuilt: both angular coefficients at each use with a fraction local of the angular block's own 848 (4225 B,
// 269/116, frame 0x1d4, first difference `push ebx`); that with lane 11 r141's list local at the removal 775 (4234 B,
// 206/91). On the 775 form the allocator's view of the tail was read for the first time: retail's `push ebx` at the
// entry needs an EBX reference outside the loop, since forcing any one of the three tail values (either level or the
// target) into EBX moves the push to the entry and nothing else (775 -> 736, forced, not source); retail's tail shows
// no EBX, so the reference is one the final code no longer has. Retail keeps the first level in EDX and the target in
// ECX, which needs the target coloured ahead of the first level (ours ranks the level 28 against the target's 18: the
// level is live but unreferenced over the six blocks of the target's compare and step). Source forms on the 775 form:
// the first level's step ahead of the target test (lane 11 r141's 807 form on main's body, re-measured 807 at 4259 B,
// 374/122) 740 (4237 B, 187/89: `push ebx` at the entry, the step's load in EBX); the record's bit-1 flag declared
// ahead of its bit-2 flag 761 (195/85); both 726 (4237 B, 176/83), the lowest count so far; the object type in a long
// local for both mask tests (lane 7 r215's form) on the 775 form 840, with the flag order 827, on the 740 form 802, on
// the 726 form 789 (it helps main's body only). None kept: the 726 form is four match-only forms together (two moved
// declarations, a statement order, the coefficient reads) and is 22 bytes short of retail. Left on it: retail
// multiplies field_28 by the rate in source order at both uses in the second limit arm where ours loads the rate
// first, the same side swap at the scale products of both limit arms, the flag pairs' store order, and the tail's
// ECX/EDX roles. Body unchanged.
// M3 lane 11 r165 (fast screen, single address; control 838 at 4255 B, aligned exact / register-masked 340 / 88).
// Retail holds the list in EBX only from the loop head through the validity tests to the swap-remove (the body's
// paths never reach the removal and reuse EBX as a call argument register) and materialises the element pointer in
// ESI; this build ranks the list's data pointer 204 over the whole loop (EBX) and the index 144 (EDI). The operand
// sort is what orders the limit products: the field_20 load ranks above the step parameter (the first symbol the
// function references, so the lowest rank) and field_28 ties with the rate, where retail's orders need the step
// ranked between the field_20 and field_28 loads; one to four dead locals leave those load ranks alone. Forms: the
// first arm's product written `field_20 * arg_1` 838 (same code); the step named once in the loop body for the three
// limit products 838 (same code, the copy is folded before the sort); the tick rate named once in the second limit
// arm for its two products 839 (4254 B: both products then load in retail's order, aligned 358 / 84); the second arm's
// step limit named once 858 (4236 B); the tick rate named in both limit arms 828 (4268 B, aligned 365 / 88: positional
// only, the length moves from 4 short to 9 long, and retail multiplies by the rate from memory at each use, so not
// kept). Body unchanged.
// Typed object model field 838, also with typed runtime object and/or named linked indices 838.
// Shared tag table base with each of those forms 844. The 0x1d8 frame and late EBX save remain.
// Kept the typed object model field alone. Plain comparison 838 at 4255 bytes.
// M3 lane 1 r167 (h2m fast, single address; control 838, aligned 333, masked 81; full check where noted). The removal
// through the existing remove_at (the array declared with the growable-array members, the loop head reading the
// component's field) re-measured: 780 at 4264 B (masked 64), and the full check still loses matched 0x54590 (two
// loads swapped at +0xff) and 0xc2d00, so reverted. The same 780 code with the header untouched: the head reading the
// field and the removal through a reference bound there (`s_havok_array08 &list = *arg_0->unknown94;`, four uses) 780
// (lead, not kept: it is lane 11 r141's list local moved to the removal under another spelling); r141's pointer form
// re-measured 780; the head's list pointer declared `*const` 838 (same code); the reference form with the object
// type in a long local for both mask tests 838 (4263 B). Body unchanged.
// M3 lane GA r8 (fast with 0x1d2280, 0x1d2070, 0x1cf9f0, 0x54590 and 0xc2d00, all MATCH each run; control 838 at 4255 B,
// aligned 247, masked 123). KEPT 838 -> 808 (plain-confirmed, 4260/4259 B, aligned 223, masked 112): (1) BEHAVIOUR FIX at
// 0x1d3155: `function_142eb0(&local_24.matrix.rotation, &local_60.rotation, &local_61)` (the old call swapped the two
// non-commutative matrix arguments; 838 alone, exact `lea ecx, [esp+0x194]; lea edx, [esp+0x14c]`), and (2) Bungie's `MIN`
// slew in the tail (`if (local_74 > local_72) local_72 = MIN(local_72 + local_70, local_74); else local_72 = MIN(local_72
// - local_70, local_74);`), which explains retail's `>` clamp on the subtract arm, moves `push ebx` to entry with no EBX
// in the tail, and matches retail's entire 79-instruction tail (`0x1d343a..0x1d3540`) instruction for instruction (`je;
// jle; mov esi, [esp+0x1c]; add eax, esi; jmp; sub eax, [esp+0x1c]`, `local_71` in EDX clamped into ECX). Other trials:
// `if (local_74 > local_72) { ... if > } else if (local_74 < local_72) { ... if > }` 805 (4256 B, aligned 232, masked 119,
// + `0x142eb0` fix 805, + staged `function_1d24a2` 805, + 4 separate fraction locals 803); staged `function_1d24a2` alone
// 838; separate `scaled` vector + `local_39 + local_37.k` 838; reversed dot operand order 847; nested `function_142b80`
// inside `function_142eb0` + split tail 811; `if (local_74 > local_72) ... else if (local_74 < local_72) ...; if (local_72
// > local_74) local_72 = local_74;` (shared clamp) 836 (+ split fraction 834, + `field_5c` read 846, + both reads/split
// 848); `if (local_74 != local_72)` with `if/else` clamp duplicated 810 (4256 B, masked 118); `if (local_74 != local_72)`
// with `else if` shared clamp 834 (4259 B); `MIN` with `if/else if` (no outer `!=`) 808 (aligned 225, + split fraction
// 806); `MIN` using `<` 808 (aligned 224); `MIN` + split fraction 806 (frame 0x1dc); split tail + angular reads + split
// fraction 818 (4226 B, frame 0x1d4, +0x14). Lead (not kept here: adding `remove_at` directly to `s_havok_array08` in
// `include/unknown_1cec30.h` without `ELEMENTS_2D91D0_MEMBERS` would have only one caller until `0x1d2070` in another file
// also calls it): `s_havok_array08::remove_at` alone 780 (4264 B, aligned 175, masked 100), + `0x142eb0` fix + split tail
// 753 (4265 B, aligned 160, masked 96), + angular reads + split fraction 742 (4235 B, frame 0x1d4, +0x14, aligned 150,
// masked 89, first 84 rows byte-exact; `0x54590` and `0xc2d00` both MATCH in fast check). Left on 808: frame 0x1d8 vs
// 0x1d4 (angular coefficient homes), loop head / swap-remove (`s_havok_array08::remove_at`), object-type `movzx`, and
// limit-arm float product sides.
// M3 lane GA r12 (fast with 0x1d2280, 0x1d2070, 0x1cf9f0, 0x142eb0, all MATCH; control 808 at 4260/4259 B, fr 0x1d8, al
// 352, mk 112; 29 valid compiled trials, body unchanged): on 808: `long object_type` for both mask tests (`movzx`) 806 (al
// 350, mk 111); `local_18` before `local_17` 796 (al 341, mk 103, exact flag-shift block; declaration order, not kept), +
// `long object_type` 794 (mk 102); `local_23` before `local_22` 809, both flag pairs swapped 797; `field_58`/`field_5c`
// read at each use + 4 block-scoped `response_fraction` locals 809 (4230 B, fr 0x1d4, +0x14, al 228, mk 105), + `long
// object_type` 804 (al 226, mk 104), + `local_18` first 796 (al 216, mk 96), + all 3 791 (al 214, mk 95); `field_5c` read
// alone 807 (fr 0x1d4, al 326), + split fraction 814; aggregate `local_28 = *local_76` 834; operand/dot/limit trials
// (`function_1d24a1(&local_31,&local_30)`, `local_37.k = local_39 + local_37.k`, angular/block-2/block-3 dot orders,
// separate `scaled` vector in limit arms, `k,j,i` `function_1d24a6`): all 808 on main (MSVC canonicalizes operands), 809
// on `fr=0x1d4` (al 222/226), 804 with `long object_type` (al 220, mk 104). Diagnostics (moved list local at removal on
// 808): alone 757 (al 289, mk 89), + `fr=0x1d4` 738 (al 164, mk 82), + `local_18` first 724 (4239 B, fr 0x1d4, +0x14, al
// 153, mk 76; + math/limit 724, al 151), + `long object_type` 800.
// M3 lane 4 r265 (fast screen, control 808 at 4260 B, frame 0x1d8 against 0x1d4; 71 stack cells sit elsewhere
// than retail's): the attachment offset through vector3d_from_points3d into local_31 with no local_75 909 (4218 B);
// into local_75 then copied 808 (same code); local_31 written by three field rows with no local_75 912 (4216 B);
// response_fraction declared above local_25 808 (same code); the body-slot clamp as `local_16 = field_1;` plus an
// if / else-if pair 913 (4259 B); the marker declared at the top of the loop body 808 (same code). Body unchanged.
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
  const s_object_definition *definition = (const s_object_definition *)g_4e3b44[*(long *)local_4 & 0xffff].bytes;
  byte *local_6 = g_4e3b44[definition->model.tag_index & 0xffff].bytes;
  byte *local_7 = g_4e3b44[*(long *)(local_6 + 0x24) & 0xffff].bytes;
  for (long local_8 = 0; local_8 < arg_0->unknown94->size; ++local_8)
  {
   s_havok_array08 *local_9 = arg_0->unknown94;
   s_1d24a0 *local_10 = &((s_1d24a0 *)local_9->data)[local_8];
   byte *local_11 = *(byte **)(local_7 + 0x44) + local_10->field_0 * 12;
   s_1d24a1 *local_12 = &(*(s_1d24a1 **)(local_7 + 0x2c))[*(short *)(local_11 + 8)];
   byte *local_13 = local_10->field_4 != NONE ? (byte *)function_badc0(local_10->field_4, (dword)-1) : NULL;
   if (local_10->field_4 != NONE && local_13 && *(long *)(local_13 + 0xb4) != NONE)
   {
    s_havok_component *local_14 = havok_component_get(*(long *)(local_13 + 0xb4));
    long local_16 = local_10->field_1 < 0 ? 0 : local_10->field_1 > local_14->rigid_bodies.size ? local_14->rigid_bodies.size : local_10->field_1;
    if (local_16 == local_10->field_1)
    {
     bool local_17 = (bool)((local_12->field_0 >> 2) & 1);
     bool local_18 = (bool)((local_12->field_0 >> 1) & 1);
     hkRigidBody *local_19 = havok_component_rigid_body_get(local_10->field_3, arg_0);
     bool local_20 = (!local_19->m_fixed && local_19->body_is_simulating().m_bool) || local_1;
     bool local_21 = havok_component_rigid_body_get(local_10->field_1, local_14)->body_is_simulating().m_bool || local_20;
     bool local_22 = (bool)((local_12->field_0 >> 25) & 1);
     bool local_23 = (bool)((local_12->field_0 >> 26) & 1);
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
     long local_15 = local_10->field_1;
     point3f const *local_76 = (point3f const *)((byte *)havok_component_rigid_body_get(local_15, local_14)->m_motion + 0x70);
     point3f local_28;
     local_28.x = local_76->x;
     local_28.y = local_76->y;
     local_28.z = local_76->z;
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
         function_1d24a4(&local_37, &local_40);
        }
        else
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
          function_1d24a4(&local_50, &local_52);
         }
         else
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
     local_14->rigid_body_linear_velocity_change(local_15, &local_25);
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
       function_141f60(function_142eb0(&local_24.matrix.rotation, &local_60.rotation, &local_61), &local_62);
       vector3f local_63;
       real local_64;
       function_11d790(&local_62, &local_64, &local_63);
       /* retail 0x1d31a6-0x1d31f2 adds this x87 dot product as (i + k) + j (the other two: (k + j) + i) */
       real local_65 = (real)(((double)local_63.i * local_29.i + (double)local_63.k * local_29.k) + (double)local_63.j * local_29.j);
       real local_67 = function_1d2280(true, local_18, havok_component_rigid_body_mass_get(local_10->field_1, local_14),
        local_64, coefficient_58 * arg_1, local_65, coefficient_5c, 0.0f,
        local_12->field_54, local_23, &response_fraction);
       function_1d24a2(&local_63, local_67, &local_59);
       vector3f local_68;
       function_1d24a2(&local_63, local_65, &local_68);
       vector3d_from_points3d((point3f const *)&local_68, (point3f const *)&local_29, &local_68);
       function_a75d0(&local_68, g_510c54->rate * coefficient_58);
       function_1d24a2(&local_68, -1.0f, &local_68);
       function_1d24a3(&local_59, &local_68);
       if (!(local_3 > response_fraction)) local_3 = response_fraction;
       local_2 = true;
      }
      function_1d24a3(&local_29, &local_59);
      havok_component_rigid_body_angular_velocity_set(local_10->field_1, local_14, &local_29);
     }
     continue;
    }
   }
   --local_9->size;
   ((s_1d24a0 *)local_9->data)[local_8] = ((s_1d24a0 *)local_9->data)[local_9->size];
   --local_8;
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
   if (local_74 > local_72) local_72 = MIN(local_72 + local_70, local_74);
   else local_72 = MIN(local_72 - local_70, local_74);
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
