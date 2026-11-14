// @flags /O2 /Oi /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "impacts.h"
#include <math.h>
#include "slot_handler.h"
static __forceinline long real_to_long(real value);

#define MIN(arg_0, arg_1) ((arg_0) > (arg_1) ? (arg_1) : (arg_0))
#define MAX(arg_0, arg_1) ((arg_0) > (arg_1) ? (arg_0) : (arg_1))
#define PIN(arg_0, arg_1, arg_2) ((arg_0) < (arg_1) ? (arg_1) : ((arg_0) > (arg_2) ? (arg_2) : (arg_0)))

struct s_vehicle_physics_state;
struct s_vehicle_ray;
struct s_vehicle_contact_result;
struct s_query_batch_result;
struct s_impact;
struct s_unknown_1eb550;
extern s_unknown_1eb550 *g_51e9c4;
extern s_record_pool *g_51ebfc;
extern s_record_pool *g_51ec00;
extern vector3f *g_4687bc;

void function_ba1d0(long arg_0, vector3f *arg_1, vector3f *arg_2);
real function_17c900(short arg_0, real arg_1);
class c_bounded_query_world
{
public:
	bool function_182800(s_vehicle_ray *arg_0, long arg_1);
};
void __stdcall function_1821d0(long arg_0, real arg_1, s_query_batch_result *arg_2, long arg_3);
void function_207ab0(long arg_0, s_vehicle_physics_state *arg_1, s_vehicle_contact_result const *arg_2);
void havok_component_rigid_body_linear_velocity_change(long arg_0, s_havok_component *arg_1, vector3f const *arg_2);
long function_2078f0(long const *arg_0, s_impact_data const *arg_1, vector3f const *arg_2, real arg_3, bool arg_4);
void function_2079f0(long const *arg_0, long arg_1, bool arg_2);
void impact_rigid_body_indices_get(s_impact const *arg_0, long arg_1, long *arg_2, long *arg_3);
void function_2294a0(long arg_0, long arg_1, long arg_2, s_impact *arg_3);
void impact_set_contact_from_component(s_impact *arg_0);

struct s_206090_contact
{
    byte padding_0[4];
    vector3f field_4;
    byte padding_10[0x24];
    point3f field_34;
    vector3f field_40, field_4c, field_58, field_64;
    byte field_70;
    byte padding_71[3];
    vector3f field_74;
    byte field_80, field_81, field_82, field_83;
    real field_84, field_88, field_8c, field_90;
    vector3f field_94, field_a0;
    byte padding_ac[4];
    short field_b0;
    byte field_b2, field_b3;
    byte padding_b4[0x24];
};
typedef char assert_206090_contact_size[(sizeof(s_206090_contact) == 0xd8) ? 1 : -1];

struct s_206090
{
    long field_0;
    byte *field_4;
    transform4x3f field_8;
    matrix3x3 field_3c;
    vector3f field_60;
    real field_6c;
    real field_70;
    vector3f field_74;
    vector3f field_80;
    real field_8c;
    byte field_90[16 * 0xa8];
    s_206090_contact field_b10[16];
};

struct s_206091
{
    point3f field_0;
    vector3f field_c;
    real field_18;
    vector3f field_1c;
};

PRIVATE __forceinline void function_206092(vector3f const *arg_0, vector3f const *arg_1, vector3f *arg_2)
{
    real second_i = arg_1->i;
    real second_j = arg_1->j;
    real second_k = arg_1->k;
    arg_2->k = arg_0->i * second_j - arg_0->j * second_i;
    arg_2->j = arg_0->k * second_i - arg_0->i * second_k;
    arg_2->i = arg_0->j * second_k - arg_0->k * second_j;
}

PRIVATE __forceinline real function_206093(vector3f const *arg_0)
{
    vector3f magnitude_input = *arg_0;
    return (real)sqrt(magnitude_input.k * magnitude_input.k + magnitude_input.j * magnitude_input.j + magnitude_input.i * magnitude_input.i);
}

PRIVATE __forceinline real function_206094(vector3f *arg_0)
{
    real local_0 = function_206093(arg_0);
    if (fabs(local_0) < 0.0001f)
        return 0.0f;
    real local_1 = 1.0f / local_0;
    arg_0->i *= local_1;
    arg_0->j *= local_1;
    arg_0->k *= local_1;
    return local_0;
}

PRIVATE __forceinline real function_206097(vector3f *arg_0)
{
    double local_2 = arg_0->i * arg_0->i;
    local_2 += arg_0->k * arg_0->k;
    local_2 += arg_0->j * arg_0->j;
    real local_0 = (real)sqrt(local_2);
    if (fabs(local_0) < 0.0001f)
        return 0.0f;
    real local_1 = 1.0f / local_0;
    arg_0->i *= local_1;
    arg_0->j *= local_1;
    arg_0->k *= local_1;
    return local_0;
}

PRIVATE __forceinline void function_206095(vector3f *arg_0, vector3f const *arg_1)
{
    arg_0->i += arg_1->i;
    arg_0->j += arg_1->j;
    arg_0->k += arg_1->k;
}

PRIVATE __forceinline void function_206096(vector3f const *arg_0, real arg_1, vector3f *arg_2)
{
    for (long component = 0; component < 3; ++component)
        arg_2->n[component] = arg_0->n[component] * arg_1;
}

#include "vector_products.h"
// score 1391 (was 1367): earlier 1379, 1355; frame 0x5d0 versus 0x58c, component/velocity homes and indexed contact induction remain.
// g1B walk (2026-11-05), every retail block 0x206090-0x2078eb read against this source, no behaviour difference found:
// fields/offsets, test directions, callees and argument order (0xba1d0, 0x182800, 0x17c900, 0x1821d0, 0x207ab0, 0x1d1010,
// 0x2078f0 twice with unknown38 0 then 1, 0x2079f0 on both its paths, 0x227990, 0x2294a0, 0x22a4b0), loop shapes (the found
// search does not stop the outer list loop, 0x207862 -> 0x20786b), the three final counters and all 19 float literals
// (read from retail's .rdata at 0x44ae90, 0x44541c, 0x45dbbc-0x45e5d0) agree. Only code shape differs.
// Frame 0x5d0 versus 0x58c; component/velocity homes, contact induction and scheduling remain.
// Earlier 1585 (was 1596): Frame 0x590 versus 0x58c; component/velocity homes and contact induction remain.
// Earlier 1600 from 1600: frame 0x580 versus 0x58c; velocity homes and contact induction differ;
// frame pointer 2092; inline depth zero 3049, eight 1600; point/contact row tables 1600/1600;
// double magnitude accumulator 1654.
// frame 0x580 versus 0x58c; velocity homes and indexed contact records differ.
// Function-scope impact record 1655; function-scope contact direction 1600; function-scope projection 1600;
// short fixed initialization index 1664; duplicate search in loop condition 1600.
// Earlier 1600 from 1600: frame 0x580 versus 0x58c and indexed contact homes remain.
// Shared point subtraction first/contact/both 1625/1656/1676;
// indexed normalized-direction copy 1645; indexed tangential-velocity copy 1638.
// Earlier 1596 from 1600: frame 0x58c recovered; velocity homes and indexed contacts differ. Contact velocity
// 1604; point velocity 1663; both 1659; result normalizers 1641; cross j/k/i 1642. On contact form:
// normalizers 1646; scale loop 1596 kept; cross order 1608; velocity names 1604; add loop 1622; scale/add
// loops 1618. Original-frame scale loop 1626.
// On recovered frame: entry return result 1599; native query records 1596; both configuration
// counts read once 1644 (frame 0x590); shared first/contact point subtraction 1620/1616.
// Round 357: Typed contact/point/both records 1612/1596/1612; indexed typed contacts
// 1585 kept (plain), indexed typed point/contact records 1593 (three-point gain rejected).
// Native configuration header/point rows/contact rows 1585 each; header/point, header/contact,
// both row tables and all layouts 1585 each; all layouts with both tables indexed at each use 1640.
// Round 140: cross first/second/both scalar captures 1581/1570/1587. Native-argument and cross-result forms
// 1620/1570/1955/1423/2158/1426/1402/1456/1504/1655/1680/1402/1772 reverted for generated local address
// cancellations.
// Pointer-interface magnitude vector capture alone/with second cross 1423/1402; three scalar magnitude
// captures alone/with second cross 1577/1604. Vector magnitude snapshot plus second cross scalars kept, plain
// 1402; all original interfaces and call expressions preserved.
// Complete cross-vector second/first/both captures with magnitude snapshot 1655/1552/1765; rejected.
// Round 143: pointer-interface complete add/scale/both captures 1991/1892/2424;
// first/second/both dot inputs 1885/1772/2221; add+dot and scale+dot 2664/2554.
// On the retained magnitude/second-cross graph, velocity/buffers/magnitude home names and
// all four combinations 1402 each; all three second-cross component names 1402. All restored.
// Consistent configuration/object/both pointer observations 1569/1524/1522; separate first/second/both
// normalizer input/output contracts 1402 each; both contracts with configuration/object reads 1569/1524.
// Round 208: named squared magnitude 1435; first/second normalizer component loops 1432/1410;
// squared magnitude with first/second loops 1434/1411; both loops 1432; all three 1450;
// all three with bounded word impact count 1445 (6238 bytes, frame 0x5d0).
// Round 149: Existing scale/add/cross output-return masks 1474/1402/1474/1402/1474/1402/1474; returned
// scale outputs at two immediate consumers 1474. First/second/both single-exit normalizers
// 1456/1410/1402; matching plain component loops 1410/1432/1432; both with first/second result name
// 1402/1402. All restored.
// Round 166: Ordinary inline at existing magnitude/cross/paired normalizer boundaries,
// individually and jointly, then all with scale/add: all eight 1402 fast at 6239/6238.
// Original producer declarations, captures, bodies and all call expressions restored.
// Round 135 (aligned difflib counts in brackets; base [1470]): the contact query rows sit at
// 0x1d8 and the 32-entry list at 0x15c in retail, 0x40 higher in ours. Magnitude read through
// the pointer 1570 (frame 0x590) [1449]; normalizer only through the pointer 1480 (0x5b4)
// [1460]; cross product without the scalar captures 1423 [1463]; both pointer forms 1585
// (0x590) [1442]; contact arrays declared at function scope 1402 [1470].
// Round P208: complete magnitude/cross/second-normalizer product terms 1471/1429/1410;
// magnitude-cross/magnitude-normalizer/cross-normalizer/all terms 1452/1435/1404/1461;
// library copy of repeated magnitude input 1402. All original producer bodies restored.
// Shared point/contact/both scales 1446/1467/1455; shared remaining crosses 1422; crosses with point/contact/both
// scales 1432/1478/1497; two output library copies 1402. Contact-scale frames 0x5c8, others 0x5cc; all restored.
// Round 159: Fast control 1402 versus full 1435. Direct
// arm/velocity/both/normal/arm-normal/velocity-normal/all/forward fields
// 1470/1462/1414/1434/1407/1475/1431/1484; direct velocity with no/arm/velocity/both field changes
// 1489/1476/1453/1460. Entry/completion/named/test-produced results 1428/1408/1404/1413 fast; completion
// result 1414 plain kept, named predicate 1433 plain restored. Result/field combinations
// 1412/1403/1408/1411 fast; completion result with direct arm/velocity fields 1405 plain kept. On that
// graph normal/forward/direct velocity/byte flags 1436/1517/1470/1403 fast, all restored.
// Round 237: Contact-counter entry constants individually/paired/joint
// 1444/1444/1493/1422/1463/1463/1492; all with entry moving-surface result 1484 fast (control 1403
// versus full 1405). Complete original counter/result lifetimes restored.
// Earlier 1405 from 1435.
// Round 161: Native point storage with direct record/arm/velocity/record-arm/record-velocity/arm-velocity/all
// observations 1445/1402/1428/1416/1415/1506/1429; complete native point members 1429.
// Fast control 1403 versus full 1405; one-point direct-arm gain rejected. Original storage and body restored.
// Round 219: Shared vector sums, point/contact/both/output/point-output/contact-output/all
// 1440/1441/1405/1405/1440/1441/1405; all with native output copies 1405. Single-phase local sums
// reach retail length but regress; combined sums restore frame 0x5d0. Original body and every
// private producer restored.
// Round 153: float products in the magnitude helper and the normalizer give retail's fld/fmul-mem x87 shape, 1364
// kept (plain). Without the snapshot copy 1606 (5990 bytes); buffered cross 1431/1445; float third-block magnitude
// 1402; float time-phase expression 1386 plain (retail's exact x87 block); single-exit normalizer through
// vector3f_scale 1581 and tangent vector from the contact field 1624 (both without the snapshot); contact pointer 1644.
// Round 108 (fast control 1403): the first loop's element pointer starts at +0x50 in retail, +0x54
// here; retail reads +0x88 once, keeps the first velocity's components in xmm2-4 from the loop top
// and computes the cross k, j, i (stores j, k, i). First-loop cross through function_206092 1472;
// +0x88 named once 1451 (read once, as retail); operands of the steering product swapped 1403
// (same code); retail's cross operand form written out 1439 (diagnostic); ray record at function
// scope 1403 (frame 0x5cc). All restored.
// Round 400: Joint coefficient branches 1364; word impact count 1386; word list probes 1369;
// coefficient/count, coefficient/probes, count/probes, all word and all byte
// 1378/1364/1399/1394/1410. The byte form has frame 0x5c8; the others 0x5d0.
// Joint coefficients reach 6238 bytes without lowering the score.
// Round 175: Native reference cross-formal subsets and magnitude input contract all 1364;
// canonical object calls, no address cancellations. On the current float-product graph, ordinary
// magnitude components/first-cross components/both/second-normalizer components/magnitude-normalizer/
// cross-normalizer/all/all with scalar cross outputs 1606/1394/1555/1400/1587/1440/1554/1554.
// Magnitude components recover frame 0x590 but shorten the body to 5990; all original input captures,
// private helper bodies, interfaces and complete force body restored. Frame and indexed homes remain.
// Round 253: Native component cursors scale/add/paired normalizers/scale-add/
// scale-normalizers/add-normalizers/all/all with ordered end tests
// 1638/1877/1557/1671/1535/1809/1633/3684. Original complete kernels, captures and body restored.
// Round 274: Narrow velocity/query/ray phase scopes, seven subsets 1364/1364/1364/1362/1362/1362/1362;
// all with bounded contact-velocity publication 1362. Two-point ray-scope lead below gate;
// original full body and every helper restored, frame remains 0x5d0 versus 0x58c.
// Round 189: char search result 1364; short search result 1435; long search result 1435; component role name
// 1364; all pointer-root names 1364; separate velocity declarations 1364; direct native byte admission 1417;
// existing point setter 1364.
// Original body retained; frame 0x5d0 versus 0x58c, component/velocity homes and contact induction remain. All checked matches survive.
// Round 41: Native impact list/pool/both schemas and signed list count, then list/count,
// pool/count, all three and all with direct kind equality:
// 1368/1364/1368/1365/1382/1365/1382/1404. List-only schema reaches 6238 bytes;
// frame stays 0x5d0 and component/velocity homes still differ. All original types and body restored.
// Round 195: On current float products, scalar magnitude/second-normalizer/first-cross
// input captures and their four combinations 1606/1364/1394/1606/1555/1394/1555;
// all with first-normalizer result join 1540. Complete original helpers, vector snapshot,
// cross captures and body restored; frame 0x5d0 versus 0x58c and indexed homes remain.
// Actual point-state/point-definition/contact-definition byte offsets, subsets 1-7: 1383/1381/1400/1400/1369/1404/1390; all with typed contact offset 1456.
// Reduced control 1363/6238 with frame 0x5cc differs from full 1364/6243 with frame 0x5d0; every offset form regresses against its control.
// Full original signed loops, live counts/bases, input captures, helpers, math groups and body restored.
// Round 452: native vector memcpy at magnitude/default/dynamic copy boundaries,
// subsets 1-7, 1364/1366/1366/1364/1364/1366/1366; complete memmove graph 1477.
// The caller/callee control reproduces full 1364/6243 without a target caveat.
// Original native copies, magnitude capture, helpers and complete body restored.
// Round 251: Native vector returns in scale/add/scale-add/cross/scale-cross/add-cross/all
// 1885/1941/2425/1674/2156/2117/2599; all with direct magnitude input 2277. Complete original helpers, magnitude
// snapshot, cross captures and body restored; frame 0x5cc versus 0x58c remains.
// Round 265: existing staged cross at point velocity/point torque/contact velocity/contact torque
// 1418/1417/1422/1384; paired points/paired contacts/private sites/all 1455/1360/1400/1445.
// Three-point contact-pair lead rejected; original body/captures restored, frame remains 0x5cc.
// Round 276: Projection snapshots through the existing scalar setter:
// direction/velocity/normalized-force/direction-velocity/direction-force/velocity-force/all
// 1438/1400/1360/1447/1441/1410/1452; all with contact-velocity publication 1421. Three-point lead
// rejected.
// Original executable source restored; Frame 0x5cc versus 0x58c, component/velocity/query homes and contact induction remain.
// Round 245: direct impact-position scalar stores with descriptor calculation/contact/phases/entry
// lifetimes 1402/1392/1397/1444; existing point setter with those lifetimes 1404/1392/1397/1444.
// The scalar forms reduce frame 0x5cc to 0x5c0 but regress; original position copy,
// descriptor lifetime, magnitude capture and complete executable source restored.
// DH round 268: retained second-input scalar capture orders ikj/jik/jki/kij/kji
// 1363/1363/1383/1383/1383; const captures ijk/kji/kij 1363/1383/1383.
// All inputs stayed before the first store; frame 0x5cc remained. Original helper and body restored.
// Round 505: Existing impact initializer, calculation/contact/phase/entry descriptor scopes,
// each with original/existing-setter position: 1401/1401/1358/1358/1383/1383/1439/1439.
// Contact-scope five-point gain rejected; original field construction and all body scopes restored.
// DL227: Real collected-array duplicate/active search cursors, index/relational/sentinel pairs
// 0-1/0-2/1-0/1-1/1-2/2-0/2-1/2-2: 1415/1415/1432/1432/1432/1433/1433/1433. Original searches
// restored; frame 0x5cc and component/velocity/contact homes remain.
// Round 309: Direct absolute force/range/nonnegative-root predicate subsets 1..7
// 1415/1389/1411/1388/1419/1386/1407; paired strict open intervals 1442.
// Original conditional magnitudes, both root guards, captures, private helpers and complete body restored; frame/home differences remain.
// Round 312: actual selected friction vector, default/explicit selection with native/setter
// transfer and separate/in-place normalized direction: 1417/1372/1465/1395;
// 1357/1474/1485/1400. Native explicit selection with a separate direction gains six
// at 6243 bytes, below the gate; pointer selection, original copies, all kernels and body restored.
// Round 314: Captured magnitude through the existing k/j/i dot kernel, with original/
// first/second/both shared scale tails, mutable or immutable capture, all 1403 at 6259
// bytes. Original magnitude, normalizers, includes and body restored; frame remains 0x5cc.
// M298: Separate collected membership and compatible-contact searches; body/head
// membership exits, byte/native list indexing and admitted/rejected filtering, eight forms
// [1449, 1449, 1449, 1449, 1449, 1449, 1449, 1449]. Original nested searches and all helpers restored.
// Round 356: Existing scaled translation with point/contact/both lever-arm subtraction,
// without/with shared contact-velocity sum: 1355/1407/1380/1362/1355/1407/1380/1362.
// Scaled translation alone kept, 1355 plain at 6243/6238; original captures and helper bodies preserved.
// DL236: Branch-owned tangent default/late force default/live control-bit subsets 1..7, then shared scalar default setters: 1365/1376/1367/1425/1423/1391/1382/1423.
// Native byte/short/long/dword final result at completion/entry: 1411/1407/1408/1408/1423/1392/1393/1393. All restored; frame 0x5cc versus 0x58c and component/velocity/query/list homes remain; Boolean interface and caller preserved.
// Y522: Existing rigid-body mass/keyframe/get helpers, seven subsets
// 1417/1387/1470/1355/1417/1387/1470; direct member mass/keyframe with getter 1470.
// All original producer bodies and force source restored; frame and velocity/query homes remain.
// DL240: Native contact direction/velocity/residual projection loops, masks 1/2/4/3/5/6/7/15:
// 1396/1405/1382/1417/1389/1373/1376/1398. Final mask includes moving-surface correction. All
// regress; original body, float trees, captures and helpers restored. Frame 0x5cc versus 0x58c and
// component/velocity/query homes remain.
// Round 137 (m3 lane 6, fast control 1355 = plain): the velocity outputs declared one per line, angular first
// 1355; the ray record and force vector declared at the first loop's top 1355; contact velocity at function scope
// 1355; the two collected arrays declared at entry 1355; the two arrays in swapped order 1355 (all the same code:
// no declaration moves a slot, so the extra 0x40 bytes are compiler temporaries, not declared locals).
// N322: Fused point-spring/contact-slip sums and native ray-origin setter, subsets 1/2/4/3/5/6/7:
// 1406/1468/1365/1403/1381/1395/1396; all with native contact-velocity setter 1416.
// Final publication uses frame 0x5c0 but regresses; original body and all kernels/captures restored,
// with frame 0x5cc versus 0x58c.
// DL245: Frame 0x5cc and component/velocity/query/list homes remain. Static/active count dword subsets 1355 each; word/long and long/word 1394; word/word and short/short 1351; byte/byte 1400. Four-point leads rejected. Matched impact-producer inline subsets 1355 each; every user unchanged. Original bodies and all declarations restored.
// P275: Actual contact-settings control bitfield with native point-row typing, read-only
// receivers and full-width zero/one control, subsets 0-7: 1355/1355/1355/1355/1356/1356/1356/1356.
// Original types and full body restored; frame 0x5cc/0x58c and component/velocity/query homes remain.
// AC486: Native-array magnitude loops, reverse/forward and zero/first-product sums
// 1398/1387/1535/1572; with reverse component dot loop 1468/1433/1608/1599. NOT kept:
// no match; original captures, float trees, kernels and whole body restored; frame/home differences remain.
// Round 490: Explicit near/increasing/decreasing compression-ramp result arms,
// seven subsets 1355 each; complete arms with twice-used ray length 1382.
// Original complete body and every kernel restored; excess frame and early homes remain.
// Y539: Existing peak/release lifecycle boundaries 1389/1389; native object/clock getters
// 1355/1355; peak/release with object or clock 1389 each. Original helpers and body
// restored; lifecycle APIs shorten to 6201 bytes without recovering frame 0x58c.
// F342: Direction-projection/velocity-projection/friction-ramp last-use scopes, masks 1/2/4/3/5/6/7,
// then all with the three coefficient lifetimes ended: 1355/1355/1355/1355/1355/1355/1355/1355. NOT
// kept: no byte match; original complete body and every helper restored. Excess frame and
// component/velocity/query homes remain.
// Round 35 (m3 lane 17, fast): the second normalizer's accumulator as real 1380 (6211 bytes); the steering
// magnitudes through fabs 1438 (6208 bytes); the three friction coefficients from one if/else on the dynamic
// flag 1353 (two points, not kept); the twice-used dynamic/moving test named for both impact calls 1395 (6211
// bytes, frame 0x5d0); the rest-length product computed before the other 1355 (same code).
// F347: Existing shared normalizer at direction/friction/tangent, masks 1/2/4/3/5/6/7, then
// all with the existing squared-length producer: 1385/1387/1414/1409/1421/1414/1457/1488. NOT kept: no match;
// original floating trees, all captured inputs, private kernels and complete body restored.
// Round 128 (lane 8, fast control 1355 = plain): retail recomputes `imul index, 0xd8/0x4c/0x3c` at the contact
// loop's top each iteration (no pointer induction) and leaves the duplicate search straight for its found exit.
// Tried: a contact pointer for the loop's element 1401 (6211 bytes, still walked); the result-record pointer
// declared at the loop's top 1355 (same code); the component index read at each use in the ray test 1474; the
// ray length named and used twice 1382; a collected flag for the duplicate search 1402.
// Round 201 (fast; aligned = difflib count): the five 12-byte magnitude copies are the frame excess
// (a compile-only frame probe lists one copy per inlined magnitude). The two buffers declared at the top
// 1355 (same code); the magnitude read through its pointer 1608 (6022 bytes, frame 0x590, aligned 968
// against 969); the contact element as a pointer local throughout the contact loop 1401 (frame 0x5c8,
// aligned 962); both together give retail's exact frame 0x58c and first difference +0x4e (local_4/local_5
// and the component homes) but 1689 (5942 bytes, aligned 933): our build still biases the element
// pointers (edi - 0x50) where retail keeps the element base and `lea ebp, [edi + 0xa0]`, and the first
// loop's per-element pointer starts at +0xe4 where retail's starts at +0xe0; on that form a single-exit
// normalizer 1689 (same code). Original body restored.
// Round 79 (m3 lane 16, fast): first-loop cross through cross3f_staged 1462 (6179 bytes); through the file's own
// k/j/i cross helper 1426 (6238 bytes, retail's length: local_5 then preloads like retail, but the local_4 add folds
// into the cross registers where retail stores the cross and re-adds); that plus vector3f_add(&local_4, v, v) 1426;
// the contact cross written straight into field_4c and summed in place (retail's form there: no contact-velocity
// local) 1388 (6238 bytes), with the first-loop helper too 1451. Aligned (difflib) counts stay 1340-1380 of 1455.
// Round 144 (m3 lane 5, fast = full 1355): seven of the accumulations through the existing vector3f_add 1401
// (6211 bytes); the crosses through the existing cross3f 1402 (6259 bytes); the dot product summed i, j, k 1355
// (same code); the second normalizer's sum i, k, j -> i, j, k 1355; the scales through the existing vector3f_scale
// 1433 (frame 0x5c8, 6211 bytes). Original body and helpers restored.
// Round 49 (m3 lane 15, fast control 1355): the two steering magnitudes as `x >= 0.0f ? x : -x` 1375 (6238 bytes,
// that block then exact: retail tests `jb` to the negation); the signed square grouped `(real)(l * l) * sign * f88`
// with the casts on both factors or the square only 1427 (6211, retail's mulss order but the clamp above shortens);
// the final loop's member count read through the element pointer local 1359 (6238); that with the magnitude form
// 1390 (6211). Retail places the duplicate-contact search out of line after the impact loop (rotated, entered by a
// forward jump when the list is non-empty); ours keeps it inline.
// P312: Native value-input magnitude/first-normalizer/second-normalizer masks 1/2/4/3/5/6/7,
// then immutable complete inputs: 1355/1357/1390/1357/1390/1410/1410/1410. NOT kept: no match;
// original pointer kernels/captures restored. Second-normalizer values enlarge frame to 0x5dc.
// Round 84 (m3 lane 9, fast control 1355): retail ends with `return a && b && c && d;` (two exits, `mov eax, 1` and
// `xor eax, eax`, registers popped before the tests); that return alone gives retail's exact tail but 1366 (6252
// bytes). Retail tests the impact list's count in the final loop's head, as impacts.cpp's sibling loop does (`for (i
// = 0; i < (list == NONE ? 0 : count); i++)`): that gives retail's loop head and 6238 bytes but 1383; with the list
// pointer local dropped (pool global read at each use) 1407; both with the direct return 1382. The magnitude helper
// without the snapshot, products in double 1548 (frame 0x590, but the double loads are shared: `fld` once and `fmul
// st(i)`, where retail reloads with `fld m; fmul m`); the component index read after the 0x81 flag test (retail's
// order) 1474 (6195 bytes). All restored.
// DL260: Current-float complete dot/magnitude/second-normalizer product parts, seven subsets, then complete squared length: 1381/1415/1415/1394/1394/1387/1424/1395.
// NOT kept: no exact native form; original complete executable source and every temporary contract restored.
// F365: Vector-owned captured magnitude and paired normalizers, masks 1/2/4/3/5/6/7, then ordinary inline:
// 1354/1354/1355/1354/1354/1354/1354/1354 fast. NOT kept: no qualifying byte match; complete original body and helpers restored.
// F367: Complete pointer-boundary aggregate magnitude/first-normalizer/second-normalizer input captures, masks 1/2/4/3/5/6/7, then all via existing setter: 1608/1413/1395/1618/1569/1453/1641/1666. NOT kept: all regress and miss native storage/extent; original captures, private helpers, APIs and full body restored.
// M3 lane 4 r165 (fast; count/aligned/bytes, control 1355/a969/6243, register-masked aligned 584 for g_contact below): LEAD: retail's first normalizer
// has no input snapshot: with the magnitude as `(real)sqrt(dot3f(v, v))` and normalize3d() ours gives retail's `movss [home], xmm; fld [home]; fmul
// [home]` at retail's offsets, `fst`, `fabs`, the epsilon `ja`, frame 0x590, but it threads the zero exit past the compare; a `do {} while (false);`
// between `real length = ...` and the compare (or an out parameter) stops that and gives retail's exact scale block and `xorps; ucomiss; lahf`:
// 1584/a946/6038 (masked 568), out parameter 1580/a947/6038. Length falls because of forms still to find. Steps: magnitude alone 1608/a967/6022; normalize3d
// for the first normalizer 1492/a1033/6112, for the double one 1384/a970/6259, both 1486/a1034/6112; magnitude with the first 1639/a977/5974, all three
// 1643/a977/5974. A const reference for the result does not stop the threading: 1395/a983/6195 on this body, on the magnitude form 1568/a963/6022,
// with normalize3d 1639/a977/5974, plus the double one's result 1608/a967/6022, all 1643/a977/5974. One result local in the normalizer 1639/a978/5974;
// `0.0f != x` 1642/a976/5974; `!(x == 0.0f)` 1639/a977/5974. Contact pointer local (retail's `imul; lea edi, [eax + ebp + 0xb10]` head) 1401/a962/6211;
// k/j/i cross in the first loop 1426/a984/6238, contact cross in place 1388/a983/6238, both 1451/a967/6185; with the join form 1610/a944/6022,
// 1631/a964/5980, 1675/a966/5964.
// Round 586: Magnitude input last-consumer scope at real/double squared-length or
// real/double magnitude producer, direct/complete named products: 1395/1383/1395/1433/1355/1386/1355/1383.
// NOT kept: nonexact lifetime/precision/staging; original helper and whole body restored.
// F385: Existing vector-union component-array observations in dot/cross/magnitude,
// seven producer subsets and the complete normalizer/add graph: eight at 1355.
// NOT kept: byte-neutral frame/home diagnostics; original complete kernels/body restored.
// P331: Point activity/query/compression admission results, masks 1/2/4/3/5/6/7 then complementary
// all: 1355/1355/1354/1351/1350/1350/1350/1380. NOT kept: at most five points, below margin; no
// exact predicate-storage match. Original helpers and body restored.
// Round 211 (m3 lane 3, fast; aligned with registers/register-blind, control 969/590): retail's contact loop keeps
// its three `imul` per iteration and no induction pointers. On Round 201's pair (re-measured 1689, 969 -> 933/575) a
// `short` contact index gives them from a dword load 1498 (6102 bytes, 918/484), but adds `movsx eax, ax` to the latch;
// the short alone 1631 (6496 bytes, 973/571). Compile-only on the pair, still reduced: while, do-while, two increments,
// a next-index local, a pointer or reference to the index, by-value and by-reference inline helpers, the whole body as
// a by-value inline helper, a dword index, one index shared with the first loop, 800 extra calls in the loop. Only the
// short or an address that survives (a real call taking it, `&index &&`) stop the reduction, so the real cause is open.
// x87 tree `(... * 0.1f + 0.9f) * (double)game_time * rate * 0.3f + index * 42.0f` 1383 (6211 bytes, 982/579): dword
// constants as retail's except the one after the double; retail's are all dword, as under a `sin` root (0x1a2197).
// Found arm first with `continue` in the impact loop 1408 (6253 bytes, 975/593). Original body restored.
// T60: Actual escaped-query schema, material initialization by long index/pointer/short/unsigned,
// private/native ray type: 1355/1421/1382/1355 in both sets. NOT kept: no native
// frame/home recovery; pointer/short traversal changes miss extent. Original raw query
// storage, private ray, captures, scaled translation and entire body restored.
// Round 136 (fast): the magnitude helper's input copy and result in an inner block 1395 (6195 bytes, frame 0x5cc);
// the copy const 1355; `sqrtf` 1415 (6259 bytes); the copy declared, then assigned 1355; the first normalizer with
// one exit (zero in the if arm, the scaling in the else) 1355.
// M3 lane 16 r91 (fast; aligned unmatched in brackets, control 1355 [969]): the first loop's offset and point-velocity
// vectors written as element expressions at each use (no pointer locals) 1403 (6211 B) [997]; the duplicate-impact search
// with the compare in the loop condition 1355 (same code); the k/j/i cross with the +0x88 factor named once for both
// steering products 1425 (6211 B) [983]; the add helper written `a = b + a` (retail loads the added vector first) 1355
// (same code), with the k/j/i cross 1426 (6238 B, round 79's code) [984]. Retail keeps the first loop's cross stores and
// re-reads j and k for the add (only i is forwarded); ours drops the cross stores and sums in registers.
// M3 lane 12 r93 (fast; 0x2056e0 matches): the first loop's element position through a named point pointer (also
// for the ray origin) 1385 (6195 bytes); its offset through the existing vector3d_from_points3d 1407 (6259 bytes); the
// magnitude summed i, j, k 1355 (same code); the magnitude from three named component locals 1608 (6022 bytes, as the
// pointer-read form); the snapshot declared const 1355 (same code). The frame excess is the snapshot copy (ours stores
// each component, copies it to the snapshot and loads x87 from there; retail loads x87 from the first store).
// Round 136 (fast): the magnitude helper's input copy and result in an inner block 1395 (6195 bytes, frame 0x5cc);
// the copy const 1355; `sqrtf` 1415 (6259 bytes); the copy declared, then assigned 1355; the first normalizer with
// one exit (zero in the if arm, the scaling in the else) 1355.
// Round 101 (m3 lane 9): 0x182800 became a member of the bounded world (it matched 0xf6670); here the world now
// loads straight into ESI as in retail (0x2063e6), one instruction fewer (6243 bytes still, masked aligned 986 ->
// 977), and the positional score moves 1355 -> 1379.
// m3 lane 1 r85 (fast = full here): the magnitude helper's result in a named real 1424 (6195 B, frame still 0x5cc,
// register-blind aligned 870 -> 743), as a double local cast on return 1448 (6275 B); re-measured: that with the sum
// i, j, k 1424, the snapshot initialised member by member 1622 (6022 B), snapshot and named result const 1424.
// Away from the helper: the contact flag bit as `(dword & 2) != 0` 1376 (colouring only); the query-record NONE loop
// stepping by the record size 1427 (6259 B); `++local_32`, the counters one per line, `0.0001f < x` 1379 each (same
// code); the first loop's arg_2 accumulation before arg_1's 1367 (not retail's order: retail adds arg_1 first); on it
// the bit test as above 1364, the named magnitude result 1424, then the 0xb4 field read at each use 1440 (6211 B),
// `1.0f + c * x` 1364, the products as `x * scale` 1364 (same code), `-a * b` 1388.
// M3 lane 1 r88 (fast with 0x2056e0, which stays matched; aligned/register-masked difflib counts, control 891/585):
// re-measured, the magnitude helper reading its input directly (no input copy) 1622 (6022 B, frame 0x590, 813/580: the
// first magnitude then is retail's `movss [esp + 0x80], xmm; fld [esp + 0x80]; fmul [esp + 0x80]` at retail's offsets).
// On it: the first loop's cross through the file's k/j/i helper 1634 (6006 B, 807/576); through cross3f_staged 1636 (6006 B,
// 806/575); the k/j/i helper reading its second vector at each use instead of staging it 1618 (5990 B, 802/570); both 1691
// (5958 B, 853/587). With the input copy kept: the k/j/i first-loop cross plus that unstaged helper 1429 (6211 B, 878/572).
// The direct-input magnitude is closer by every aligned count; the 216 missing bytes are retail's store-then-reload rows
// (results stored to their fields and read back) spread over the contact loop.
// Round 228 (m3 lane 3, fast; aligned counts exact/stack-masked/register-masked in brackets, base 1379 [1384/1485/988]).
// Re-measured on today's base: the direct `return a && b && c && d;` 1390 [1370/1457/977]; the final loop's count in
// its head (`i < (list == NONE ? 0 : count)`, the list read in the body) 1397 (6216 B) [1389/1588/916]. New: both
// 1389 (6238 B) [1371/1566/918]; on that, the found arm first 1401 [1374/1566/955], the magnitude read without its
// input copy 1632 (5991 B) [1434/1333/860], the list entries re-derived from the pool at each use (round 84's
// form, re-measured) 1426 (6287 B) [1372/1454/921], the steering sign as `if (field_74.i >= 0.0f) value = -value;` 1378 (6238 B) [1388/1560/914];
// with that, the two steering magnitudes as `x >= 0.0f ? x : -x` 1427 (6273 B) [1379/1479/904], those and the two
// threshold magnitudes in the same form 1376 (6238 B) [1381/1553/892] (three points, not kept), the threshold
// pair alone on the sign form 1427 (6273 B) [1392/1492/905], alone on main 1403 (6211 B) [1390/1549/963]. On the
// 1376 form: the first loop's cross through the file's k/j/i helper 1445 (6193 B) [1372/1536/876], the found arm
// first 1393 [1388/1556/929], the list entries re-derived 1379 (6255 B) [1370/1543/895]. Retail's final loop is not
// rotated (the latch jumps back to a head that re-reads the list handle) and its duplicate search sits after the
// loop, entered by `jg` when the found list is not empty. All restored.
// Round 184 (m3 lane 4, fast; stack- and register-masked difflib count 579 at 1379): kept the four magnitudes
// (local_95/96 and local_97/98) as `x >= 0.0f ? x : -x`, 1379 -> 1367 (masked 568): retail tests each as `comiss x, 0;
// jb` to the negating arm, so a NaN takes `0 - x`, which `x < 0.0f ? -x : x` did not; the steering pair alone 1391
// (6211 bytes). On 1367: the contact ratio's three arms in retail's predicates (`local_24 >= local_26 ? 0.0f : 1.0f`,
// `local_24 >= local_26 ? 0.0f : (local_26 >= local_23 ? 1.0f : ...)`, `local_23 >= local_26 ? 1.0f : (local_26 >=
// local_24 ? 0.0f : ...)`, each read from retail's branches) 1392 (6211 bytes, masked 560: the block is then retail's
// but for local_98, which retail computes only after the first threshold test); the clamp's mass bounds named before
// PIN (retail computes both before its first branch) 1378; both 1386 at 6238/6238 bytes (masked 574); the ratio arms
// with the final loop's count in its head 1402 (6264 bytes, masked 534), with the bounds too 1406 (masked 535).
// M3 lane 13 r81 (fast = plain 1367; aligned exact/fully masked counts, base 1108/580): retail's first loop head (0x206154)
// has no alignment pad because it would need 12 bytes; ours needs 9 (`jmp` + filler) only because local_2's home sits at
// [esp+0x94] (a 4-byte longer store than retail's [esp+0x54]), so the pad follows the frame and is not a separate cause.
// Diagnostics: a bounds test in local_8's read at the loop top 1451 (not threaded); the loop as `if (...) do ... while
// (++i < count)` 1364 (still padded; a hand-inverted loop, not kept). Kept, same code: the inline_depth pragmas around the
// function removed 1367; the file's own k/j/i dot helper replaced by the existing dot3f at all seven calls 1367. Also:
// local_77 through dot3f 1373 (6238 bytes); the magnitude helper as `(real)sqrt(dot3f(&copy, &copy))` 1412 (6195 bytes);
// the sign of local_13 as a named long 1367 (same code). Retail computes local_13 as (l * l) * sign * f88 in source order;
// ours multiplies sign * f88 first (the listed `(real)(l * l)` grouping gives retail's order). One first-loop add through
// the existing vector3f_add 1364 (the do-while's 3-point colour flip in local_28's products); the ray direction alone
// through the existing vector3f_scale 1334 (6211 bytes; positional only: that site then differs from retail, the other
// inlined scale loops flip to `s * v` where retail has `v * s`, exact aligned 1108 -> 1122; not kept); the first-loop
// contact cross through cross3f 1390 (6211 bytes); local_68 through sqrtf 1430 (6259 bytes); the impact count test as
// `(char)x == 1` 1407 (6195 bytes); local_18 computed before local_17 1367, the found flag stored after the ratio 1367, the
// component test before the 0x81 flag 1367 (each different code, same score); local_93 written into the call 1367 (same code).
// M3 lane 13 r83 (fast; fully masked aligned count in brackets, control 1367 [580]): retail tests the release time's NONE
// first and converts the 0.2 s threshold only inside the second test, reading the game time before the conversion; ours
// converts ahead of the whole condition. The conversion written into that test (local_93/local_94 dropped) 1374 (6249 B)
// [558: that block then has retail's order but for the game-time load]; that with the game time through the existing
// getter 0x146650 1405 (6201 B) [558: the game time is then read before the conversion as retail, and every product's
// operand order flips, the first-loop add included, as other added inline calls do]. Retail also reloads the timing
// global after 0x227990 inside the countdown loop where ours hoists it (0x227990 is unmatched, in a held file). The ray
// origin copied after the direction scale 1361 (statement order, six points, not kept); both normalizers' scale step
// through the existing vector3f_scale 1367 [581] (only the first component turns to retail's `s * v`); the second
// projection reading a copy of the contact normal (retail copies the normal to [esp+0xd0] there) 1489 (6313 B) [619];
// the existing magnitude3d (0x24590) at the three direct magnitude sites 1486 (6102 B, inlined) [569]; the impact
// position written straight by vector3f_add_scaled (no point local) 1431 (6211 B) [588]; the contact offset through the
// existing vector3d_from_points3d 1376 [569]. All restored.
// T95: Existing scaled-add direction/velocity/residual families, masks 1/2/4/3/5/6/7:
// 1397/1393/1388/1374/1380/1393/1365; all plus moving-surface correction 1394.
// NOT kept: 1374/1380/1365 clear the margin only as combinations; the first two
// improve only length with more differing instructions, and the last substitutes
// negated-scale addition for retail positive-product subtraction without equality.
// Frame remains 0x5d0 versus 0x58c; original captures, kernels and complete body restored.
// T97: Existing magnitude/first-normalizer/second-normalizer real values transported by double returns,
// seven subsets then all with explicit real consumers: 1411/1380/1432/1410/1381/1437/1461/1378.
// NOT kept: all regress against reduced control 1367 (full 1391); original contracts, captures and body restored.
// P444: first lever-arm array difference 1432f; contact lever-arm array difference 1410f; moving-surface array difference 1399f; residual-velocity array difference 1391f; direction-normalization result 1367f; tangent-normalization result 1367f; peak-update producer 1367f; release-due producer 1367f.
// NOT kept: no byte match or supported native frame/result recovery; original complete body restored.
// P446: Actual slip, force-transfer, movable-body, impact-record, append, collected-member, compatible-contact and created-impact producers;
// 1388/1388/1388/1388/1388/1388/1414/1388. NOT kept: three-point nonmatch gains are below the eight-point bar; original complete body restored.
// M3 lane 10 r101 (fast with 0x2056e0, which stays matched; fast = plain here, 1391; brackets = difflib counts exact / registers
// masked / registers and stack offsets masked, control [1384/1148/966]). NOT kept (coordinator: 5 points, under the 8-point bar): the contact velocity formed in
// place (the cross written straight into the contact's field_4c and local_4 added there, the `contact_velocity` local copy
// dropped) 1391 -> 1386 (6249 bytes, frame 0x5c8) [1487/1216/980]: retail stores that cross into the field and re-adds in
// memory (`movss [ebx], xmm3; addss xmm0, xmm3; movss [ebx], xmm0`). Before it: 0 to 7 unused longs ahead of the body
// (symbol-count phase scan) 1391 each, same code (no phase effect here); the magnitude helper reading its input directly
// re-measured 1623 (6022 bytes, frame 0x590) [1444/1080/915]; that with the in-place velocity 1639 (5964 bytes, frame 0x588)
// [1541/1111/880]. On that form: a `contact` pointer local for the whole contact loop 1443 (6166 bytes, frame 0x5c0)
// [1379/1464/905], the pointer without the in-place velocity 1430 (6195 bytes); the impact position written straight by
// vector3f_add_scaled 1448 (6163 bytes); the wheel force through the existing vector3f_add_scaled (no scaled temporary) 1395;
// the stored length read back from field_90 1437. Retail keeps the contact pointer in EDI, reads field_40 from memory at each
// cross product use and reloads field_40.k right after storing it.
// P449: Dominant-force/positive-force, moving-surface and friction-sign outcomes in bool/long:
// eight 1364 fast (control 1367; full 1391). NOT kept: three-point context-relative gain; no exact frame/result repair.

// P452: Independent magnitude/velocity-publication/query-origin/tangent/direction/velocity/friction-input/impact-normal memmove boundaries:
// 1436/1376/1409/1397/1440/1407/1425/1376. NOT kept: 1376 leads retain nonnative record-transfer boundaries and excess frame; no byte equality. Original captures, all helpers, call tuples and full body restored; matched caller survives.
// P454: Existing magnitude snapshot as aggregate/shared setter, five remaining native component-store
// schedules ikj/jik/jki/kij/kji, plain component copy: 1623/1675/1623/1623/1623/1623/1623/1627.
// NOT kept: no gain; construction shortens to 6022/5974 or grows to 6480 versus 6238, without native
// frame recovery. Original snapshot, reduction, every helper and full body restored.
// m3 lane 7 r159 (plain; fast prints a CAVEAT for this function, fast 1367 against full 1391): the two steering magnitudes
// through fabs 1442 (6272 B); the scale helper as three component statements in place of its 3-pass loop 1446 (6195 B,
// aligned 1384 -> 1358, frame 0x5cc); the cross helper reading its second operand directly 1396; the magnitude helper
// reading its input directly 1623 (6022 B, frame 0x590: re-measured); both of the last two together 1673 (5974 B).
// A /GL harness of this file alone gives frame 0x5cc for the body (full 0x5d0), so callee bodies add one spill here.
// Without the snapshot copy retail's magnitude shape appears (components stored, then fld/fmul from those slots) but our
// spill slots sit 4 bytes above retail's (one extra spill slot low in the frame, shared by a float and an int).
// P477: Standard ordered first-loop/contact-normal/direction/tangent/friction/peak dots, retained-snapshot magnitude and mixed-precision normalizer reductions:
// 1373/1487/1375/1404/1435/1372/1900/1449. NOT kept: 1373/1375/1372 add an arithmetic call absent from retail (15 versus 14); frame stays 0x5d0. Other forms regress; matched caller survives. Original helpers, captures, call tuples and complete body restored.
// @retail 0x206090
bool __stdcall function_206090(s_vehicle_physics_state *arg_0, vector3f *arg_1, vector3f *arg_2)
{
    s_206090 *local_0 = (s_206090 *)arg_0;
    byte *local_1 = local_0->field_4;
    byte *local_2 = *(byte **)(g_4e0300->data + (local_0->field_0 & 0xffff) * 12 + 8);
    s_havok_component *local_3 = havok_component_get(*(long *)(local_2 + 0xb4));
    vector3f local_4, local_5;
    function_ba1d0(local_0->field_0, &local_4, &local_5);
    *arg_1 = *g_4687a4;
    *arg_2 = *g_4687a4;
    for (long local_6 = 0; local_6 < *(long *)(local_1 + 0x3c); local_6++)
    {
        byte *local_7 = local_0->field_90 + local_6 * 0xa8;
        byte *local_8 = *(byte **)(local_1 + 0x40) + local_6 * 0x4c;
        vector3f *local_9 = (vector3f *)(local_7 + 0x40);
        vector3f *local_10 = (vector3f *)(local_7 + 0x4c);
        local_9->i = *(real *)(local_7 + 0x34) - local_0->field_60.i;
        local_9->j = *(real *)(local_7 + 0x38) - local_0->field_60.j;
        local_9->k = *(real *)(local_7 + 0x3c) - local_0->field_60.k;
        cross3f(&local_5, local_9, local_10);
        function_206095(local_10, &local_4);
        real local_11 = PIN(*(real *)(local_1 + 0x30) * local_0->field_8c * 0.31830987f, -1.0f, 1.0f);
        local_11 = (local_0->field_74.i >= 0.0f ? -1.0f : 1.0f) * local_11;
        real local_12 = -(local_0->field_74.j * *(real *)(local_7 + 0x88));
        real local_13 = local_11 * local_11 * (local_11 != 0.0f ? (local_11 < 0.0f ? -1 : 1) : 0) * *(real *)(local_7 + 0x88);
        real local_95 = local_12 >= 0.0f ? local_12 : -local_12;
        real local_96 = local_13 >= 0.0f ? local_13 : -local_13;
        real local_14 = local_95 > local_96 ? local_12 : local_13;
        real local_15 = local_14 > 0.0f ? PIN(local_14 - 0.1f, 0.0f, 1.0f) : PIN(local_14 + 0.1f, -1.0f, 0.0f);
        real local_16 = *(real *)(local_1 + 0x2c) * local_15 + 1.0f;
        real local_17 = local_16 * *(real *)(local_8 + 0x20);
        real local_18 = local_16 * *(real *)(local_8 + 0x10);
        s_206091 local_19;
        local_19.field_0 = *(point3f *)(local_7 + 0x34);
        function_206096(g_4687bc, local_17 + local_18, &local_19.field_c);
        long local_20 = *(long *)(local_2 + 0xb4);
        if (local_7[0x81] && local_20 != NONE)
        {
            s_havok_component *local_21 = havok_component_get(local_20);
            if (TEST_FIELD_BIT(local_21->flag5) && local_21->unknown9c &&
                ((c_bounded_query_world *)local_21->unknown9c)->function_182800((s_vehicle_ray *)&local_19, NONE))
            {
                real local_22 = (local_17 + local_18) * local_19.field_18 - local_17;
                real local_23 = *(real *)(local_8 + 0x18);
                real local_24 = *(real *)(local_8 + 0x1c);
                real local_25;
                real local_26 = *(real *)(local_7 + 0x24);
                real local_97 = local_23 - local_24;
                real local_98 = local_24 - local_23;
                if ((local_97 >= 0.0f ? local_97 : -local_97) < 0.001f ||
                    (local_98 >= 0.0f ? local_98 : -local_98) < 0.001f)
                    local_25 = local_24 < local_26 ? 1.0f : 0.0f;
                else if (local_23 > local_24)
                    local_25 = local_24 < local_26 ? (local_26 < local_23 ? (local_26 - local_24) / local_97 : 1.0f) : 0.0f;
                else
                    local_25 = local_23 < local_26 ? (local_26 < local_24 ? (local_24 - local_26) / local_98 : 0.0f) : 1.0f;
                real local_27 = local_22 > 0.0f ? 1.0f - local_22 / local_18 : 1.0f;
                real local_28 = (*(real *)g_51e9c4 * local_27 * local_27 - *(real *)(local_8 + 0x14) * dot3f(local_10, &local_19.field_1c)) *
                    *(real *)(local_7 + 0x9c) * *(real *)(local_8 + 8) * local_0->field_6c * local_25;
                if (*(real *)(local_7 + 0xa0) > 0.0f)
                {
                    real local_29 = (real)(((double)((local_6 + 1) / *(long *)(local_1 + 0x3c)) * 0.10000002384185791f + 0.9f) *
                        g_510c54->game_time * g_510c54->rate * 0.3f + (double)local_6 * 42.0f);
                    local_28 *= 1.0f - *(real *)(local_7 + 0xa0) * (function_17c900(8, local_29) * 0.55f + 0.45f);
                }
                vector3f local_30;
                function_206096(&local_19.field_1c, local_28, &local_30);
                function_206095((vector3f *)(local_7 + 0x90), &local_30);
                local_7[0x80] = true;
                *(real *)(local_7 + 0xa4) = local_27;
            }
        }
        function_206095((vector3f *)(local_7 + 0x58), (vector3f *)(local_7 + 0x90));
        function_206092(local_9, (vector3f *)(local_7 + 0x58), (vector3f *)(local_7 + 0x64));
        function_206095(arg_1, (vector3f *)(local_7 + 0x58));
        function_206095(arg_2, (vector3f *)(local_7 + 0x64));
    }
    long local_31 = 0, local_32 = 0, local_33 = 0;
    bool local_34 = false;
    long local_35[32];
    byte local_36[16 * 0x3c];
    for (long local_37 = 0; local_37 < 16; local_37++)
        *(short *)(local_36 + local_37 * 0x3c + 8) = NONE;
    if (*(long *)(local_1 + 0x44) > 0)
        function_1821d0(*(long *)(local_2 + 0xb4), *(real *)(local_1 + 0x18), (s_query_batch_result *)local_36, *(long *)(local_1 + 0x44));
    for (long local_38 = 0; local_38 < *(long *)(local_1 + 0x44); local_38++)
    {
        if (!local_0->field_b10[local_38].field_b3)
        {
            byte *local_40 = *(byte **)(local_1 + 0x48) + local_38 * 0x4c;
            bool local_41 = (bool)((*(dword *)(local_40 + 4) >> 1) & 1);
            vector3f local_42 = *g_4687a4;
            local_0->field_b10[local_38].field_40.i = local_0->field_b10[local_38].field_34.x - local_0->field_60.i;
            local_0->field_b10[local_38].field_40.j = local_0->field_b10[local_38].field_34.y - local_0->field_60.j;
            local_0->field_b10[local_38].field_40.k = local_0->field_b10[local_38].field_34.z - local_0->field_60.k;
            vector3f contact_velocity;
            function_206092(&local_5, &local_0->field_b10[local_38].field_40, &contact_velocity);
            function_206095(&contact_velocity, &local_4);
            local_0->field_b10[local_38].field_4c = contact_velocity;
            local_32++;
            byte *local_45 = local_36 + local_38 * 0x3c;
            function_207ab0(local_38, arg_0, (s_vehicle_contact_result *)local_45);
            if (local_0->field_b10[local_38].field_70)
            {
                local_0->field_b10[local_38].field_4c.i -= local_0->field_b10[local_38].field_74.i;
                local_0->field_b10[local_38].field_4c.j -= local_0->field_b10[local_38].field_74.j;
                local_0->field_b10[local_38].field_4c.k -= local_0->field_b10[local_38].field_74.k;
                local_34 = true;
            }
            if (local_0->field_b10[local_38].field_84 > 0.0001f)
            {
                vector3f *local_46 = &local_0->field_b10[local_38].field_a0;
                real local_47 = dot3f(&local_0->field_b10[local_38].field_4c, local_46);
                local_0->field_b10[local_38].field_88 = local_0->field_6c * (*(real *)g_51e9c4 / *(real *)(local_1 + 8) * local_0->field_b10[local_38].field_84 - *(real *)(local_1 + 0xc) * local_47);
                function_206096(local_46, local_0->field_b10[local_38].field_88, &local_0->field_b10[local_38].field_94);
                vector3f local_48 = *g_4687b0;
                if (*(long *)(local_45 + 0x24) != NONE && local_0->field_b10[local_38].field_88 > 0.0f)
                {
                    s_havok_component *local_49 = havok_component_get(*(long *)(local_45 + 0x24));
                    long local_50 = *(long *)(local_45 + 0x28);
                    hkRigidBody *local_51 = local_49->rigid_bodies.data[local_50].rigid_body;
                    if (!local_51->m_fixed && local_51->m_motion->kind_code() != 6)
                    {
                        real local_52 = MAX(1.0f, havok_component_rigid_body_get(local_50, local_49)->m_motion->getMass());
                        real local_53 = PIN(local_52, local_0->field_6c * 0.0625f, local_0->field_6c * 16.0f);
                        real local_54 = (local_46->k + 1.0f) * 0.5f / local_53 * local_0->field_b10[local_38].field_88;
                        local_54 *= local_54;
                        local_54 = MIN(local_54, MAX(8.0f, g_510c54->field_2_3 * local_47));
                        vector3f local_55;
                        function_206096(local_46, g_510c54->rate * local_54 * -1.0f, &local_55);
                        havok_component_rigid_body_linear_velocity_change(local_50, local_49, &local_55);
                    }
                }
                vector3f local_56 = local_0->field_b10[local_38].field_4;
                real local_57 = dot3f(&local_56, local_46);
                local_56.i -= local_46->i * local_57;
                local_56.j -= local_46->j * local_57;
                local_56.k -= local_46->k * local_57;
                if (function_206094(&local_56) != 0.0f)
                {
                    vector3f local_58 = local_0->field_b10[local_38].field_4c;
                    real local_59 = dot3f(&local_0->field_b10[local_38].field_4c, local_46);
                    local_58.i -= local_46->i * local_59;
                    local_58.j -= local_46->j * local_59;
                    local_58.k -= local_46->k * local_59;
                    vector3f local_60;
                    if (local_41)
                    {
                        function_206096(&local_56, -local_0->field_b10[local_38].field_8c, &local_60);
                        function_206095(&local_60, &local_58);
                    }
                    else
                    {
                        vector3f local_61;
                        function_206096(&local_56, dot3f(&local_58, &local_56), &local_61);
                        local_60.i = local_58.i - local_61.i;
                        local_60.j = local_58.j - local_61.j;
                        local_60.k = local_58.k - local_61.k;
                        local_0->field_b10[local_38].field_8c = function_206093(&local_61) * (dot3f(&local_61, &local_56) > 0.0f ? 1.0f : -1.0f);
                    }
                    real local_66 = function_206093(&local_60);
                    local_0->field_b10[local_38].field_90 = local_66;
                    real local_62 = local_0->field_b10[local_38].field_82 ? *(real *)(local_40 + 0x1c) : *(real *)(local_1 + 0x10);
                    real local_63 = local_0->field_b10[local_38].field_82 ? *(real *)(local_40 + 0x20) : *(real *)(local_1 + 4);
                    real local_64 = local_0->field_b10[local_38].field_82 ? *(real *)(local_40 + 0x24) : *(real *)(local_40 + 0x18);
                    vector3f *local_65 = &local_60;
                    if (local_0->field_b10[local_38].field_82 || local_0->field_b10[local_38].field_83)
                    {
                        local_65 = &local_58;
                        local_66 = function_206093(local_65);
                    }
                    real local_67 = 1.0f - PIN((*(real *)(local_1 + 0x14) - local_46->k) / (*(real *)(local_1 + 0x14) - *(real *)(local_1 + 0x18)), 0.0f, 1.0f);
                    if (local_66 > local_64)
                    {
                        vector3f local_99 = *local_65;
                        function_206094(&local_99);
                        real local_68 = (real)sqrt(local_62 * 0.9f);
                        function_206096(&local_99, -(*(real *)(local_40 + 8) * local_68 * local_67 * local_0->field_b10[local_38].field_88), &local_42);
                        local_0->field_b10[local_38].field_81 = true;
                    }
                    else
                    {
                        real local_69 = (real)sqrt(local_63 * 0.9f);
                        function_206096(local_65, -(local_69 * *(real *)(local_40 + 8) * g_510c54->rate * local_67 * local_0->field_b10[local_38].field_88), &local_42);
                        local_31++;
                    }
                    local_48 = local_60;
                }
                local_0->field_b10[local_38].field_80 = true;
                if (*(real *)(local_40 + 0xc) > 0.0001f && local_46->k > *(real *)(local_1 + 0x18))
                {
                    real local_70 = MAX(0.0f, local_0->field_b10[local_38].field_90 - *(real *)(local_40 + 0x18));
                    if (function_206097(&local_48) == 0.0f)
                        local_48 = *g_4687b0;
                    real local_71 = -(*(real *)(local_40 + 0xc) - local_0->field_b10[local_38].field_84 - 0.0001f);
                    point3f local_100;
                    vector3f_add_scaled((vector3f const *)&local_0->field_b10[local_38].field_34, local_46, local_71, (vector3f *)&local_100);
                    s_impact_data local_72;
                    local_72.unknown00 = false;
                    local_72.component_a = *(long *)(local_2 + 0xb4);
                    local_72.unknown08 = 0;
                    local_72.material_a.m_index = *(short *)(local_40 + 0x40);
                    local_72.component_b = NONE;
                    local_72.unknown14 = NONE;
                    local_72.material_b.m_index = local_0->field_b10[local_38].field_b0;
                    local_72.position = local_100;
                    local_72.normal = *local_46;
                    local_72.type = local_38;
                    local_72.unknown38 = false;
                    local_72.shape.type = NONE;
                    local_72.shape.index = NONE;
                    function_2078f0(&local_0->field_0, &local_72, &local_48, local_70, local_0->field_b10[local_38].field_82 || local_0->field_b10[local_38].field_83);
                    local_72.unknown38 = true;
                    long local_74 = function_2078f0(&local_0->field_0, &local_72, &local_48, local_70, local_0->field_b10[local_38].field_82 || local_0->field_b10[local_38].field_83);
                    if (local_74 != NONE)
                    {
                        long local_75;
                        for (local_75 = 0; local_75 < local_33; local_75++)
                            if (local_35[local_75] == local_74)
                                break;
                        if (local_75 == local_33 && (dword)local_33 < 32)
                            local_35[local_33++] = local_74;
                    }
                }
            }
            else
                function_2079f0(&local_0->field_0, local_38, false);
            function_206095(&local_0->field_b10[local_38].field_58, &local_0->field_b10[local_38].field_94);
            function_206095(&local_0->field_b10[local_38].field_58, &local_42);
            function_206092(&local_0->field_b10[local_38].field_40, &local_0->field_b10[local_38].field_58, &local_0->field_b10[local_38].field_64);
            function_206095(arg_1, &local_0->field_b10[local_38].field_58);
            function_206095(arg_2, &local_0->field_b10[local_38].field_64);
            if (local_41 && local_0->field_b10[local_38].field_80)
            {
                vector3f *local_76 = &local_0->field_b10[local_38].field_4;
                real local_77 = local_42.i * local_76->i + local_76->k * local_42.k + local_76->j * local_42.j;
                vector3f local_78;
                function_206096(local_76, local_77, &local_78);
                real local_95 = (real)sqrt((double)local_78.i * local_78.i + (double)local_78.k * local_78.k + (double)local_78.j * local_78.j);
                if (dot3f(local_76, &local_42) > 0.0f)
                    local_0->field_70 -= local_95;
                else
                    local_0->field_70 += local_95;
            }
        }
        else
            function_2079f0(&local_0->field_0, local_38, false);
    }
    for (long local_79 = 0; ; local_79++)
    {
        long local_80 = local_3->unknown20;
        byte *local_81 = g_51ec00->data + (local_80 & 0xffff) * 0x40;
        long local_82 = local_80 == NONE ? 0 : *(short *)(g_51ec00->data + (local_80 & 0xffff) * 0x40 + 2);
        if (local_79 >= local_82)
            break;
        long local_83 = *(long *)(local_81 + 4 + local_79 * 4);
        byte *local_84 = g_51ebfc->data + (local_83 & 0xffff) * 0xa0;
        if (!local_84[0xe])
            continue;
        bool local_85 = false;
        for (long local_86 = 0; local_86 < local_33; local_86++)
        {
            if (local_35[local_86] == local_83)
            {
                for (long local_87 = 0; local_87 < (local_80 == NONE ? 0 : *(short *)(local_81 + 2)); local_87++)
                {
                    long local_88 = *(long *)(local_81 + 4 + local_87 * 4);
                    byte *local_89 = g_51ebfc->data + (local_88 & 0xffff) * 0xa0;
                    if ((char)local_89[0xd] != NONE && !local_89[0xe] && *(short *)(local_89 + 0x2c) == *(short *)(local_84 + 0x2c) &&
                        *(short *)(local_89 + 0x2e) == *(short *)(local_84 + 0x2e))
                    {
                        local_85 = true;
                        break;
                    }
                }
            }
        }
        if (!local_85)
        {
            while (*(short *)(local_84 + 8) > 0)
            {
                if (*(short *)(local_84 + 8) == 1 && PIN((char)local_84[0x10], 1, 1) == (char)local_84[0x10])
                {
                    long local_90, local_91;
                    impact_rigid_body_indices_get((s_impact *)local_84, local_83, &local_90, &local_91);
                    real local_92 = *(real *)(local_84 + 0x14);
                    if (!local_84[0x11] || local_92 > *(real *)(local_84 + 0x18))
                    {
                        real local_93 = g_510c54->field_2_3 * 0.2f;
                        long local_94 = real_to_long(local_93);
                        if (*(long *)(local_84 + 0x94) == NONE || g_510c54->game_time - *(long *)(local_84 + 0x94) > local_94 ||
                            local_92 > *(real *)(local_84 + 0x98) * 1.3f)
                        {
                            local_84[0x11] = true;
                            *(real *)(local_84 + 0x18) = local_92;
                        }
                    }
                    function_2294a0(local_83, local_90, local_91, (s_impact *)local_84);
                }
                (*(short *)(local_84 + 8))--;
            }
        }
        else
            impact_set_contact_from_component((s_impact *)local_84);
    }
    bool result = false;
    if (!local_34 && *(long *)(local_1 + 0x44) && local_31 > (local_32 >> 1) && local_0->field_8.up.k > *(real *)(local_1 + 0x18))
        result = true;
    return result;
}
