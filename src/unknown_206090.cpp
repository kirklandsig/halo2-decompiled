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
    real first_i = arg_0->i;
    real first_j = arg_0->j;
    real first_k = arg_0->k;
    arg_2->k = first_i * arg_1->j - first_j * arg_1->i;
    arg_2->j = first_k * arg_1->i - first_i * arg_1->k;
    arg_2->i = first_j * arg_1->k - first_k * arg_1->j;
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
// Earlier header (merged into the note above the function in m3 lane 7 r187): 1375 (was 1375): Frame 0x5d0 versus retail 0x58c,
// component and velocity homes, and contact indexing still differ.
// Earlier: earlier 1391, 1367, 1379, 1355; frame 0x5d0 versus 0x58c, component/velocity homes and indexed contact induction remain.
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

// score 1346 (was 1346): Frame 0x5d0 versus retail 0x58c. Component and velocity stack homes still differ (an earlier
// Earlier header line, a second stacked score line merged here in m3 lane 13 r149: 1346 (was 1346; m3 lane 2 r139, m3 lane 8 r221 and m3 lane 1 r154 unchanged, their lines just above the marker): Frame 0x5d0 versus retail 0x58c. Component and velocity stack homes still differ (an earlier
// Earlier header line, a stack merged here in m3 lane 2 r139: 1346 (was 1346; m3 lane 8 r221 and m3 lane 5 r234 unchanged, its line just above the marker): Frame 0x5d0 versus retail 0x58c. Component and velocity stack homes still differ (an earlier
// header line, 1375 (was 1375), sits further up this file).
// (Second note, merged:) Frame 0x5d0 versus retail 0x58c. Component and velocity stack homes still differ.
// Earlier header, merged into this note (1375): Frame 0x5d0 versus retail 0x58c. Component and velocity stack homes still differ.
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
// P490: Standard collected-prefix duplicate/membership find and count producers:
// find/original, original/find, find/find, count/original, original/count, count/count,
// find/count, count/find: 1410/1389/1452/1468/1372/1399/1452/1399.
// NOT kept: 1372 adds a search call absent from retail (15 calls versus 14);
// its frame is 0x5c8 and extent 6220 versus 0x58c/6238. Other forms regress or
// gain only two. Original indexed searches, snapshots, helpers and complete body restored.
// m3 lane 15 r84 (plain each, 0x2056e0 matched throughout; control 1391). Phase regime: one statement or local name added
// or removed flips commutative operand orders and same-size stack homes across the whole body, swinging the count by
// about +-40 with no local change, so single forms here cannot be judged by count. Examples: `__assume(count <= 16)` ahead of
// the wheel loop or of the contact loop 1374/1374 (6238 B, different operand orders each); the two threshold fields
// (+0x18/+0x1c of the wheel definition) read at each use 1355 (6238 B), the wheel +0x24 field at each use 1363 (6211 B),
// both together 1474 (6291 B); none kept. Other forms: one counter shared by the wheel and contact loops 1388 (the contact
// loop is still strength-reduced; retail recomputes `imul i, 0xd8/0x4c/0x3c` each pass, counter in one stack slot shared
// with the wheel loop); the skipped-contact arm first with `continue` 1371 (6243 B, skip call moved to the loop top, not
// retail's layout; arms swapped, not kept); the threshold ratio's nested ternaries as if/else 1423 (6275 B); the mass
// factor `(k + 1) * 0.5f` named 1434 (retail multiplies by 0.5 before the divide; `0.5f * (k + 1)` and `/ 2.0f` same code);
// the tangent dot reading the contact field instead of its copy 1375 (a copy/field mix, not kept); the rigid-body index
// at each use 1423; the impact strength at each use 1456 (6281 B); the field_70 update as one ternary add 1391.
// Same code: local_27 and local_15 as if/else. Re-measured: the 0.2 s conversion inside the test 1398 (6265 B); the direct
// `return a && b && c && d;` 1413 (6275 B; retail's exit shape, pops ahead of the last tests).
// M3 lane 1 r104 (plain; the fast check CAVEATs this function): the duplicate-impact search's not-found test as
// `local_75 >= local_33` 1391 -> 1375 at 6238 B (kept: the loop's own exit compare now threads past the re-test, as
// retail's not-found path goes straight to the capacity test; the found exit still re-tests where retail jumps out). On
// it: the search compare in the loop condition 1375 (same code); the capacity as the array's element count 1375 (same
// code); local_12's negation on its first factor 1420; the steering sign as a named long 1375 (same code); the
// velocity-change bound's product named ahead of the mass clamp (retail computes it there) 1377. Before it: the magnitude
// helper taking its vector by value 1391 (same code as the snapshot copy).
// P501: Standard whole-record
// magnitude/ray/contact-publication/direction/tangent/normalized-force/carried-force/impact-normal transfers:
// 1410/1427/1375/1380/1436/1388/1397/1417. NOT kept: All retain frame 0x5d0 versus native 0x58c. t03/t04 gains
// 16/11 add one absent library call (15 versus 14), and t06 gains only three. All transfer contracts rejected;
// original helpers and body restored. Original complete executable body restored.
// m3 lane 14 r96 (fast = full 1375, 0x2056e0 stays matched): the final loop's inactive-impact skip as a nested if block
// in place of `continue` 1375 (same code); local_4/local_5 named linear_velocity/angular_velocity 1375 (same code: the
// names do not swap their homes, retail has local_4 below local_5, ours the reverse); the final loop's member count read
// through local_81 (`*(short *)(local_81 + 2)`) 1391 (6211 B); the havok component index read at each use after the
// 0x81 flag test (no local_20) 1460 (6259 B); the steering product's first two factors swapped 1375 (same code). Retail's
// final loop reads the pool's data pointer at each use (count, member, search), forms the list pointer only inside the
// found-duplicate search, and reaches that search by `jg` with the countdown as the fall-through.
// P511: Explicit actual steering-range/dominant-force/dead-zone/compression/mass-floor/mass-range/impulse-cap/friction-slope results:
// 1394/1375/1375/1375/1366/1394/1409/1416. NOT kept: best 1366 grows to 6243 versus native 6238,
// with excess frame unchanged; other results tie or regress. Original complete body and all helpers restored.
// P520: Standard steering/dead-zone/mass/impulse/slope/speed/kind/complete range providers:
// 1534/1519/1441/1506/1445/1421/1420/1809. NOT kept: no exact native storage/provider recovery; original complete body and helpers restored.
// P522: Real second-coefficient root/first-normalizer absolute/second-normalizer absolute,
// seven domains then complete real-root API graph: 1396/1394/1396/1449/1394/1449/1375/1394. NOT kept: no exact native frame/API recovery; original complete body and helpers restored.
// P524: Actual contact decision-byte storage at 70/80/81/82/83/b3, then all signed/unsigned:
// 1375/1430/1375/1375/1375/1375/1430/1430. NOT kept: no exact native storage recovery; original record declarations, helpers and full body restored.
// P526: Native first/second/both material value assignments, complete descriptor aggregate, plain paired-publication loop, loop-owned collection, aggregate plus loop, materials plus loop:
// 1375/1375/1375/1409/1368/1425/1449/1391. NOT kept: seven-point loop lead below margin; other forms tie or regress, frame still 0x5d0 versus 0x58c.
// Original complete executable source restored.
// P528: Individual shared scale sites at point impulse/normal force/body impulse/
// driven slip/projection/dynamic friction/static friction/load projection:
// 1394/1443/1394/1394/1394/1394/1394/1380. NOT kept: all regress or move extent away;
// excess frame and indexed homes remain. Original body, kernels and includes restored.
// Round 128 (m3 lane 12, fast; control 1375, frame 0x5d0; retail keeps local_4 below local_5 at 0xb4/0xc0 and the component
// at 0xcc, ours local_5 0xe0, local_4 0xec, component 0x11c; both builds inline the fld/fistp rounding, so both use the
// asm-mode slot order): the two 0xba1d0 outputs named linear_velocity/angular_velocity 1392 (6211 bytes, frame 0x5cc, the pair
// keeps its order); the impact position built by vector3f_add_scaled straight into the impact record 1412 (6163 bytes, 0x5c0);
// the first loop's force added to +0x90 by vector3f_add_scaled in place of the scaled temporary 1373 (6243 bytes, 0x5cc); both
// 1419 (0x5c0); the first loop walking its element and definition pointers (`+= 0xa8`, `+= 0x4c` at the body's end) 1397.
// P542: Independent computed point/contact spring-damping, slope quotient, wave response, kinetic/static force, impulse-bound and excess-speed parts: 1398/1413/1426/1377/1397/1397/1375/1394.
// NOT kept: ties or regressions; native frame 0x58c and component/velocity homes remain unresolved. Original body and all kernels restored.
// P545: Standard component transforms at point/contact lever arms, moving-surface correction,
// residual subtraction, repeated add/scale and first/second normalization tails:
// 1453/1419/1408/1393/1592/1712/1476/1398. NOT kept: no native frame/extent
// recovery; full control 1375, reduced 1392. Original kernels and complete body restored.
// P556: Actual escaped velocity/query/impact object references, then contact relative-velocity/
// normal-force/accumulated-force/lever-arm/torque field lvalues: 1375/1391/1375/1379/1429/1379/1431/1398.
// NOT kept: no native frame recovery or equality; complete body, input captures and existing kernels retained.
// Round 112 (m3 lane 10; fast on the reduced image, whose control reads 1392 at frame 0x5cc): the magnitude helper with
// double-promoted products through the pointer, no snapshot copy, 1596 (6006 bytes, frame 0x590: each component is loaded
// once, `fld st(0); fmul st(1)`); with float products through the pointer 1637 plain (5974 bytes, frame 0x590): every
// magnitude is then retail's own `movss [esp+X], xmm; fld [X]; fmul [X]` from the vector's home (the first at retail's
// 0x80/0x7c/0x78), so retail has no snapshot copy; the stack disp32/disp8 use counts then agree (133/199 against
// 133/192), so the 264 missing bytes are block layout and contact-record addressing, not the frame (lead for a
// rewrite on that basis). The helper taking its vector by value 1392 (same code as the copy); the two contact arrays
// declared at the top 1392 (same code); the impact direction through the float normalizer in place of the
// double-accumulating one 1454 (6307 bytes, frame 0x5dc). All restored.
// M3 lane 16 r127 (plain; fast prints a CAVEAT here): a contact pointer for the whole contact loop with the contact cross
// written straight into its field_4c and summed in place 1409 (6182 B, frame 0x5c0; ours still walks a strength-reduced record
// pointer where retail recomputes `imul index, 0xd8` each pass); the steering product as `field_8c * local_1[0x30]` 1375 (same
// code); the wave term as a float tree, `((real)(quotient) * 0.10000002f + 0.9f) * game_time * rate * 0.3f + (real)index *
// 42.0f`, 1421: LEAD, that block is then retail's exact x87 sequence with dword constants, but the first loop's allocation shifts
// (local_13's sign/field_88 product reassociates); that with field_88 read once for local_12 and local_13 1450 (6275 B); the
// float wave term with the contact pointer and in-place cross 1439 (6166 B).
// P583: Advance-before-admission signed point/contact/query-initialization ordinals,
// seven subsets, then both collected-prefix searches: 1375/1375/1375/1375/1375/1375/1375/1375. NOT kept: no exact
// native frame/indexed-contact recovery; original loops, helpers and complete body restored.
// M3 lane 7 r172 (fast prints the reduced-image CAVEAT: fast control 1392 at 6211 B, aligned 1379; plain control 1375,
// aligned 1402): nothing kept. Plain: the result returned directly as `return !local_34 && ... && ...;` (no flag) 1376
// (6243 B, aligned 1384: retail's tail exactly, `mov eax, 1` / `xor eax, eax` exits with the pops before the tests, but one
// over the control; fast 1384); the last loop as a counted `for` testing the impact count in its condition (no `for (;;)` and
// break) 1397 (6264 B; fast 1378); the contact velocity formed in place (the cross straight into field_4c, local_4 added
// there, no copy local) re-measured 1421 (6217 B). Fast: the steering sign folded into local_11's initialiser 1433; the
// impact offset scale written into the vector3f_add_scaled call (no named local_71) 1392 (same as the fast control).
// Retail's impact record sits at 0xd4 below ours (0x11c against 0x160 on the listing), so the whole 0x44 frame excess
// lies below it; retail lays the found-search loop out after the impact loop.
// M3 lane 13 r104 (fast = plain here, 0x2056e0 matched; control 1375, difflib exact/masked aligned 981/585): three
// retail-shape leads, each raising the count through the phase regime. The impact-list loop as a plain `for` testing
// `h == NONE ? 0 : arrays[h & 0xffff].count` through a typed 0x40-byte impact array, the element read as
// `arrays[h & 0xffff].impact_indices[i]` and the inner search the same way, 1398 (982/556: the loop head is then retail's
// exactly, the count read in the `jne` arm from a fresh pool load and the element at `[eax + (h * 16 + i) * 4 + 4]`); that
// with a typed array pointer for the inner search 1428 (6289 B); `return a && b && c && d;` for the flag-set result alone
// 1376 (6243 B, retail's `mov eax, 1` / `xor eax, eax` tail; one over the control), with the plain loop 1397; the friction
// arm's `sqrt(x * 0.9f)` before the direction copy and its normalize 1405 (6240 B, 945/568: retail computes and stores that
// root first, then multiplies in SSE where ours keeps it on the x87 stack); root first with the direct return 1420, with
// both other leads 1468. One to five unused locals on the plain-loop form 1397 each (unused locals do not move the phase).
// M3 lane 11 r112 (fast = full at 1375; "aligned" = difflib-aligned differing lines with numbers masked, then with
// registers masked too, control 901/537): the magnitude helper without its snapshot copy (read through the pointer) 1637 at
// 5974 B, aligned 822/519: the frame drops to 0x590 (retail 0x58c; the five 12-byte copies were the extra slots) and every
// magnitude reads its vector in place as retail does (`fld [v+8]; lea eax, [v]; fmul [v+8]`), but the body is 264 B short;
// the existing cross3f for the first loop's torque cross 1417, for both torque crosses 1371 (6243 B; retail reloads the
// second operand at each use there); the contact velocity's cross written straight into the contact's +0x4c field with
// the base velocity added in place (no `contact_velocity` copy, as retail's `lea ebx, [edi + 0x4c]`) 1421 (6217 B,
// aligned 831/443); that with the copy-free magnitude 1656 (5948 B, 775/436); with cross3f at both torque crosses 1401
// (6201 B, 832/445); all three together 1626 (5980 B, aligned 700/427, build/r112/leads/lead-0x206090.patch). So those
// forms are retail's but leave the body about 260 B short. Retail's contact loop also does no strength reduction (each
// pass forms the contact, the +0x48 record and the query result with `imul` from the counter at +0x1c, the counter the
// first loop also uses); ours steps all three as induction pointers. Compile-only screen of the loop: a `continue` for
// the inactive contact, a `while` loop, the bound written first, `++local_38`, the counter declared outside or shared
// with the first loop, a `dword` counter, a contact pointer local: all keep the pointers; only a `short` counter gives
// the `imul`s (with a `movsx` and a second slot, not retail).
// Native frame 0x58c, component/velocity homes and indexed contact induction still differ.
// magnitude helper for projection 1421; magnitude helper for slip 1421; magnitude helper for tangent 1433;
// magnitude helper for projection and slip 1430;
// magnitude helper for projection and tangent 1452; magnitude helper for slip and tangent 1446; squared-length
// helper for slip 1421; squared-length helper at the three direct roots 1518.
// NOT kept: removing individual magnitude input copies reduces the frame to 0x5c0, pairs reach 0x5b8, and
// replacing all three direct roots reaches 0x5a8 at 6070 bytes. Every preview regresses and the magnitude
// grouping requires equality.
// The frame remains 0x5d0 against 0x58c. Component and velocity homes and contact indexing differ.
// Slip in impact buffer 1447. Save slip before in-place force normalization 1517.
// Direct tangential velocity 1373. Direct projected forward 1445. Both projections 1403.
// Both with slip in impact buffer 1559. Both with in-place normalization 1508.
// Direct tangential with slip in impact buffer 1454. Direct forward with in-place normalization 1547.
// NOT kept: the direct tangential form gains two points. The smaller frames shorten the body.
// Round 415. typed physics object 1375, shared collision ray 1375, ray length shared with compression 1389,
// conditional world choice 1396, branch-selected world 1394, ray scoped to admitted query 1440, shared ray scale
// 1371, typed object with conditional world 1396. NOT kept: the shared ray scale gains four points while adding 21
// bytes. The frame is still 0x5d0.
// M3 lane 4 r213 (fast = plain 1375 here; aligned = difflib-aligned differing instructions exact / registers and
// slots masked, control 981 / 585). Lead: retail's contact loop keeps its counter in memory and forms the contact, the
// definition row and the query row with `imul` each pass; (diagnostic only) letting the counter's address reach an
// opaque call on a never-taken path gives exactly that (1666, 6512 bytes, aligned 952 / 548), and with lane 11's
// copy-free forms (magnitude read in place, cross3f at both torque crosses, the contact velocity formed in place) the
// body is 6278 bytes against retail's 6238 with the diagnostic's own ~15 bytes in it (1438, frame 0x580, 933 / 535);
// with the copy-free magnitude alone 1393 (6278 bytes, 938 / 545). So the snapshot copies stand in for the bytes the
// unreduced indexing costs, and what keeps the counter out of a register in retail is the open question (no `lea` of
// its slot in retail; an inlined out-parameter does not do it, see 0x1608e0's note). Source forms: lane 11's three
// forms re-measured 1626 (5980 bytes, frame 0x588, 945 / 549); a contact pointer local at the top of the pass 1422
// (6211 bytes, 957 / 589); that on the copy-free forms 1650 (5948 bytes, 959 / 567); with one function-scope counter
// for the first two loops as well 1649 (955 / 562). All three keep the stepped pointers (one `imul` in the listing
// against retail's three). Body unchanged.
// (An earlier second header line at 1375, was 1375, merged into this note in lane 13 r112.)
// The extra 0x44 frame bytes, escaped velocity and component homes, and indexed contact loop remain.
// Independent slip/tangent lengths use a default then tangent 1447, complete branch results
// with direct or selected tangent 1455/1455, and a common predicate with selected or direct
// tangent 1388/1455. Selected-pointer test 1389, choice before coefficients 1453,
// slip branch first 1455. Every form retained the 0x5d0 frame.
// Query scope 1375. Ray scope 1374. Velocity scope 1394. Typed query scope 1375.
// Descriptive workspace names 1375. Query and ray scopes 1374.
// Velocity scope with typed query 1394, with workspace names 1394.
// NOT kept: ray scope gains one point and all forms retain the larger frame.
// Live point or contact count field references 1394/1394; live point or contact
// definition-table field references 1375/1375; count and table references together
// in either phase 1394/1394; shared motion-origin field reference 1449, with all
// count and table references 1449. The frame and original contact induction remain.
// Shared in-place scale in the mixed-precision normalizer, named reciprocal/argument: 1449/1375.
// Float-sum normalizer, named reciprocal alone/with named/with direct mixed reciprocal: 1424/1421/1424.
// Direct float-sum reciprocal alone/with named/with direct mixed reciprocal: 1436/1390/1436.
// The 0x5d0 frame and displaced velocity and component homes remain.
// Lane 13 r112 (plain checks: a `--fast` run of this function alone or with its eleven callees, 0x24590 and 0x30bf0 builds
// it differently, 1392 at 6211 B with frame 0x5cc, so fast counts for it need a wider list). Retail's first loop and
// contact loop keep their counters in one home (0x1c, read with `fild` in the first loop and passed to 0x2079f0 in the
// second) and the definition pointer in the next (0x20). One counter variable for the first loop and the contact loop
// 1375; for the first, the 16-entry and the contact loops 1375; for the 16-entry and the contact loops 1375; the contact
// loop as a `while` with its counter declared ahead and stepped at the end 1375 (the same code each: the contact, the
// definition row and the query row stay stepped pointers, no `imul`); one counter for the first and the 16-entry loops
// 1450 (6275 B).
// Copy-free squared accumulator or named terms 1625/1625; descending component loop with
// direct or named term 1568/1558; double accumulator 1591; shared squared-length helper 1637;
// staged float root 1599; ascending component accumulator 1625. NOT kept: all regress and
// shorten. Direct scalar forms recover frame 0x590, still four bytes above retail.
// One magnitude workspace at contact phase or entry, assignment/memcpy, 1515 each. Separate root and
// normalization workspaces at the same scopes, assignment/memcpy, 1457 each. NOT kept: the frames shrink
// to 0x5a0/0x5a8 but the body loses 120/88 bytes and retail's component homes still differ.
// M3 lane 7 r187 (plain each, 0x2056e0 MATCH each time; control 1375, difflib-aligned 1402). Kept: function_206092 holds
// its first operand in locals and reads the second through its pointer at each use (it held the second): 1346 at 6243 B
// (aligned 1399); the same with other local names 1346. Retail evidence: the contact loop's cross loads local_5's three
// components once (`movss xmm4/xmm3/xmm2, [esp+0xc0/0xc4/0xc8]`) and reads field_40's at each use (two loads each).
// Tried: the first loop's cross through cross3f_staged (before the kept change) 1422 (6243 B); on the kept change the
// contact cross written straight into field_4c with local_4 added there (no contact_velocity copy; retail's `lea ebx,
// [edi+0x4c]` shape) 1419 (6185 B, frame 0x5c8); the first loop's cross through function_206092 1405 (6238 B: k, j, i
// stores as retail, but the products interleave where retail computes all six first); cross3f_staged at the three
// function_206092 sites 1357 (6243 B: the second contact cross then reads both operands at each use and stores after all
// products, as retail, with some products' operand loads the other way round); the first loop's cross through
// cross3f_staged on the kept change 1421 (6238 B); the kept helper computing k, j, i into locals before its three stores
// 1386 (6243 B, aligned 1388).
// M3 lane 9 r143 (a reduced-link LTCG screen of this file, faithful to the fast build; aligned difflib rows against retail,
// control 1382, frame 0x5d0): the magnitude helper's snapshot bound by const reference 1433 (frame 0x590, 23 rows short);
// the add helper as a three-pass component loop 1399; the cross helper staging its second vector as one vector3f copy 1439
// (0x5e8); the add helper plain `inline` 1382 (same code); the scale loop's counter as short 1460; the cross's staged
// reals declared k, j, i 1383. Not kept.
// M3 lane 10 r126 (plain with 0x207ab0, 0x2078f0, 0x2079f0; fast builds this body at 6211 B, so plain only; control 1375):
// the magnitude helper taking the vector by value 1375 (same code; a compile-only screen keeps five 12-byte copies, the
// by-value temporaries do not share a slot either); the steering sign as `if (...) local_11 = -local_11;` 1378; the result
// returned as one expression 1376 (6243 B); the impact loop's count test in the `for` condition 1397 (6264 B); the two
// query arrays declared at the top 1375 (same code). This note and the one above it were one stacked pair; merged here.
// M3 lane 7 r189 (plain each, 0x2056e0 MATCH each time; control 1346; aligned = rows outside a minimal diff against retail,
// exact / stack offsets masked / registers and stack offsets masked, control 937 / 768 / 519). Nothing kept. LEAD for the
// contact loop's three per-pass `imul`s (compile-only screen, then plain): retail needs BOTH a contact pointer formed at the top
// of the pass AND a counter the optimiser does not treat as an induction variable. Forms that stop the reduction without
// emitting code: the counter read once through a null-checked pointer in an inline helper (`p ? *p : NONE` on its address), or
// the counter held in a one-long aggregate that is passed whole, by value, to the real call 0x2079f0 (0x207ab0 taking the
// plain long); both give retail's exact loads of the counter (top of the pass, the impact data's index store, the 0x2079f0
// argument, the latch) and put the counter in the first loop's counter slot right below the definition pointer's home, as
// retail's 0x1c / 0x20. Forms that still reduce: a counter that is a member of a local aggregate read only by member; a
// two-member overlay of the counter; a discarded address-of; the aggregate passed by value into an inline helper; the increment
// through a pointer helper; the counter read after the loop; an inline-assembly block in the pass. One function-scope counter
// for the first loop and the contact loop keeps the first loop's stepped pointers and still gives the contact loop's three
// multiplies, so retail's shared slot can be one variable. Without the contact pointer the exposed counter gives 23
// multiplies (4 with the by-value aggregate), with it exactly 3.
// Plain trials (all use the null-checked read, a diagnostic only): contact pointer + exposed counter 1512 (6320 B, aligned
// 955 / 815 / 511); that with the magnitude read in place (no input copy) 1565 (6083 B, frame 0x588, 930 / 798 / 491); that
// with the contact cross written straight into field_4c and local_4 added there 1558 (6073 B, 859 / 737 / 479); that with the
// direct `return a && b && c && d;` 1543 (6089 B, 854 / 727 / 477); that with the impact data filled by the existing
// impact_data_set (shape NULL) 1522 (6095 B, frame 0x580, 848 / 718 / 476, build/r189/lead-206090-t5.diff): the loop head is
// then retail's but for registers (ours counter EAX and state EBX, retail EDX and EBP), and retail still has 24 more
// disp32 stack rows (133 against 109) and 12 more frame bytes, so one 12-byte copy that retail makes is still missing.
// On main's body alone: impact_data_set in place of the fourteen stores 1370 (6243 B, 934 / 769 / 520); with the impact data
// declared at the top of the admitted contact's block 1363 (6259 B, 929 / 772 / 523). Retail's duplicate search leaves
// straight past the append when it finds the impact (`je` out of the scan), the shape of a return inside an inlined
// add-if-absent helper; ours re-tests the scan index.
// Same round, evidence trial for the aggregate reading (plain; restored): 0x2079f0's index parameter as a one-long aggregate
// by value (definition, its three declarations, 0x2078f0's call passing the impact data's field through a cast, this
// function's two calls passing a temporary initialised from the counter) with the callee's discarded address-of of that
// parameter removed: 0x2078f0 still MATCH; 0x2079f0 20 at 190 bytes (the parameter stays on the stack by itself with
// `ret 8`, but it is loaded after the component lookup and EBX/EDI swap); this function 1658 at 6496 bytes (the contact
// counter is no longer an induction variable although only a copy of it is passed, so every indexed use re-multiplies:
// the contact pointer form above is what turns that into retail's three).
// Captured slip with branch/conditional/direction selection 1348/1345/1374. Published slip with
// these forms 1393/1393/1375. Selected speed before/after coefficient reads 1371/1663.
// NOT kept: captured conditional gains one at retail length with frame 0x5d0. Late speed
// reaches frame 0x5c8 at 6502 bytes. The component and velocity homes remain displaced.
// M3 lane 15 r112 (fast = full 1346, 0x2056e0 matched; difflib aligned 1399): the contact loop's counter as a short
// 1687 (6528 B); the contact count named once for the query test and the loop bound 1355 (6238 B, aligned 1395); the
// wheel count named once for the bound and the wave term 1432 (6238 B); the velocity pair declared local_5, local_4 or
// in two statements 1346 (same code: the homes do not move).
// M3 lane 16 r154 (fast = full here, 0x2056e0 MATCH each run; control 1346, difflib-aligned 1399): a named contact pointer
// (`&local_0->field_b10[local_38]`) for the whole contact pass, every contact access through it, 1445 (6179 B, aligned
// 1374: still stepped, no `imul`); with the contact velocity's cross written into field_4c and local_4 added there 1426
// (6182 B, aligned 1465); with the query row pointer formed beside the definition row 1445 (same code); with the result
// returned as one expression 1427 (6195 B, aligned 1373); that plus the wave term as a float tree 1396 (6211 B, aligned
// 1379); the float wave term alone on this body 1360 (6243 B, aligned 1404). Retail's first loop is strength-reduced
// (stepped record pointer, row offset in a slot) and only its contact loop is not, with the counter in the +0x1c home
// the first loop also uses for its `fild`; the cause is still open.
// degenerate spring result 1346, falling spring result 1346, rising spring result 1346, explicit spring
// results 1346, spring coefficient default 1359, steering sign result 1391, steering deadband arms 1346,
// compression default and adjustment 1406, direct wide magnitude products 1566.
// The 0x58c frame and native velocity homes remain unresolved. Direct wide magnitude products reduce the
// frame to 0x590 and shorten the body to 6022 bytes.
// M3 lane 14 r131 (fast = plain 1346 here, 0x2056e0 MATCH each run; body unchanged): the contact cross written straight
// into field_4c and local_4 added there, alone on this body, 1419 (6185 B); descriptive names for the four tail
// counters and the flag 1346 (same code: names do not move these homes); the object lookup through the existing
// havok_object_get and its component-index field 1346 (same code); the signed square written sign first 1346 (same
// code: the chain is canonicalised; ours multiplies the sign by the +0x88 factor first, retail squares local_11 first);
// local_11 declared at function scope 1346 (same code).
// M3 lane 2 r119 (fast = plain here, 0x2056e0 MATCH each run; control 1346 at 6243 B; aligned = rows outside a minimal diff
// against retail with addresses, stack offsets and registers masked, control 932). A chain, each step on the one before:
// (1) the magnitude helper reading its input directly (no snapshot) 1624 (5990 B, frame 0x590, aligned 939); (2) in the
// friction arm the root `sqrt(local_62 * 0.9f)` computed before the normalised copy (retail evaluates and stores it first,
// then does the scale product in SSE from the normalised components held in xmm) 1603 (6018 B, aligned 912); (3) the
// contact cross written straight into field_4c and local_4 added there (retail's `lea ebx, [edi + 0x4c]` shape) 1618
// (5977 B, frame 0x588, aligned 827); (4) the friction ternaries in retail's order (local_62, local_64, local_63) 1618
// (5977 B, aligned 817); on (3), a named bool for `field_82 || field_83` ahead of the ternaries with that order 1602 (5993 B,
// aligned 889; retail recomputes the `||` at both impact calls). Not kept: the positional count rises because the body is
// 261 bytes short; retail's contact loop still multiplies the counter (three `imul`) and its homes need disp32 offsets.
// Patch: build/r119/lead-206090.patch in the m3-lane2 worktree.
// M3 lane 5 r216 (plain, 0x2056e0 MATCH each time; control 1346): a compile-only listing of this file names the frame's
// locals: the five inlined copies of the magnitude helper's input (12 bytes each) are 60 of the 68 extra frame bytes, and
// retail computes every magnitude in x87 straight from the vector's own slots; the helper reading its input directly
// re-measured 1624 (5990 B, frame 0x590), so retail's extra length is elsewhere. The helpers' bodies as the existing
// shared ones: the cross product as cross3f_staged 1357; the in-place add as vector3f_add(a, b, a) 1346 (same code); the
// scale as vector3f_scale 1412 (6238 B, retail's length); the normalize as normalize3d 1470 (6131 B).
// Round 499: entry object index, common zero-vector source, ray-admission component read, point pulse clock, named
// excess slip, normalized direction scope, impact-position scope, and contact-velocity scope.
// Fast scores in order 1370/1370/1447/1346/1370/1346/1346/1346.
// NOT kept: Four forms tie at 1346. The named object index, common zero-vector source, ray admission and excess-
// slip forms raise the score.
// The excess magnitude snapshot storage, 0x58c frame and retail component and velocity homes remain.
// Point velocity snapshots at the arm/cross (angular), sum (linear), or both
// boundaries 1415/1415/1417/1490. Contact equivalents 1361/1361/1362/1469.
// NOT kept: all regress. Point copies retain 0x5d0; contact copies add frame storage.
// The 0x58c frame and the escaped velocity homes remain.
// Projected heading aggregate 1424, tangent aggregate 1440, both 1510, unpowered projection aggregate 1374,
// all three 1460, kinetic direction normalized in the force output 1433, translated impact point aggregate
// 1349, both projections through component setters 1524. NOT kept: all regress from the reduced control. The
// 1374 and 1349 forms reach retail length but retain a frame difference.
// M3 lane 16 r166 (fast with 0x2056e0, MATCH each run; control 1346, difflib-aligned 1399, register-and-slot-masked 920).
// Retail's contact pass loads the counter once from its home, forms the contact with `mov eax, edx; imul eax, eax, 0xd8;
// lea edi, [eax + ebp + 0xb10]` ahead of the disabled test, then multiplies the same register by 0x4c and by 0x3c: a
// copy and an in-place multiply each time, the shape an index conversion leaves. A contact pointer at the top of the
// pass 1445 (6179 B, re-measure; still stepped); that with `(dword)` on the index in the three products 1445 (same code:
// the conversion folds); `(dword)` on every indexed access without the pointer 1347 (6238 B, retail's length; masked
// 911); a dword counter under a signed test 1347 (the same code as that), with the contact pointer 1445; diagnostic,
// the index masked `& 0xffff` in the three products with the contact pointer 1434 (6320 B, aligned 1242, masked 788:
// the three per-pass multiplies appear and nothing is stepped, at the cost of three `and`s). Body unchanged.
// contact normalization returns success, early failure 1368, impact normalization returns success, early
// failure 1386, contact normalization and impact normalization returns success, early failure 1368,
// contact normalization returns success, test result 1350, impact normalization returns success, test
// result 1390, contact normalization and impact normalization returns success, test result 1405, contact
// normalization returns success, early success 1350, contact normalization and impact normalization
// returns success, early success 1405.
// NOT kept: All eight forms retain the 0x5d0 frame and raise the score. Three reach retail length.
// The 0x5d0 frame exceeds 0x58c. The component and velocity homes remain displaced.
// M3 lane 2 r129 (h2m fast, 0x2056e0 MATCH each run; control 1346 at 6243 B, first loop's element pointer at +0xe4 against
// retail's +0xe0): the first loop's offset and velocity rows bound as references instead of pointers 1346 (same code);
// local_2 declared ahead of local_1 1346 (same code: retail stores local_2's home first, ours local_1's, unchanged); the
// ray record local_19 declared at function scope 1386 (6243 B); the magnitude helper taking its vector by value (the
// copy as its parameter) 1346 (same code: still one 12-byte copy per site); the contact loop placed under the query
// call's `count > 0` test 1456 (6257 B).
// M3 lane 13 r139 (fast with 0x2056e0, MATCH each run; control 1346): the magnitude helper's input read directly 1624
// (5990 B, frame 0x590), through a const reference 1686 (5958 B), as three scalar copies 1624; the snapshot kept with double
// products 1393 (6238 B); the snapshot summed i, j, k 1346 (same code). The direct form's instruction count is within two
// of the control's, so its short length is displacement size: its busiest locals sit below +0x80 where retail's sit above.
// M3 lane 8 r217 (h2m fast with 0x2056e0, MATCH each run; control 1346). Retail pads the two join points after the contact
// loop's skip calls (0x207420, 0x207640) to 16 bytes as if they were loop heads; ours pads neither. Tried: the disabled
// contact's skip call written first (`if (field_b3) skip; else { ... }`, no inverted test) 1349 (6238 B); the zero-depth skip
// call written first 1366 (6238 B); both 1402 (6211 B); the first loop's cross through function_206092 re-measured 1405 (6238 B);
// both skip calls first with that cross 1451 (6195 B). Body unchanged.
// Entry object identifier 1370, typed lookup with entry identifier 1370, scoped component index 1346,
// scoped component with entry identifier 1370, both identifiers scoped through velocity call 1370,
// entry identifier with shared zero-vector pointer 1346, typed lookup and scoped component 1346,
// typed lookup and both identifiers 1370. NOT kept: the 0x5d0 frame, velocity homes and 6243-byte
// extent persist in all eight forms.
// M3 lane 12 r172 (H2M_FAST, 0x2056e0 MATCH each run; control 1346, difflib-aligned 1399, register-and-slot-masked
// 1355): the magnitude helper's terms written i, j, k with the snapshot kept 1346 (6243 B; aligned 1388, masked 1161:
// every magnitude then evaluates k*k, j*j, i*i in retail's order, where the current text gives i*i, j*j, k*k; a lead,
// the count does not move); the magnitude read directly with i, j, k terms 1624 (5990 B, masked 1099); the two velocity
// outputs declared local_5 first 1346 (same code); on the i, j, k form, the contact cross written into the contact's
// velocity field with the base velocity added there 1419 (6185 B); the scale helper copying its input vector (retail
// copies the normal before the projection at 0x206d0c) with the magnitude read directly 1543 (6358 B, frame 0x5e8).
// M3 lane 12 r174 (h2m fast with 0x2056e0, MATCH each run; control 1346): the first loop's offset rows through the
// existing vector3d_from_points3d(&field_60, element + 0x34, local_9) 1379; the definition row pointer declared ahead of
// the element pointer 1347; the torque pointer declared ahead of the offset pointer 1346 (same code); the ground probe's
// component reused from the entry lookup (same index) 1453; the probe's component index read at the loop top 1431.
// Diagnostic, the magnitude read directly (1624, 5990 B, frame 0x590): 1426 instructions against retail's 1455; between
// the two 0x2078f0 skip calls 425/439, and the three tail calls sit in another block order (119/72, then 8/60
// instructions), so most of the byte shortfall is displacement size and tail block order. Body unchanged.
// M3 lane 8 r221 (h2m fast with 0x2056e0, MATCH each run; control 1346): lane 2 r119's lead rebuilt (magnitude read in
// place, the contact cross straight into field_4c, local_64 ahead of local_63, the root ahead of the normalised copy)
// 1618 (5977 B); that with `(dword)` on every contact index 1618 (same code); the duplicate-impact search with a found
// flag set at the match and `if (!found && ...)` 1342 (6243 B; four points, a flag set inside a test: match-only); the
// handbrake bit as `(... & 2) != 0` 1346 (same code); the first loop's wheel record read at each of its six uses (no
// local_8) 1446. Body unchanged.
// M3 lane 1 r154 (H2M_FAST with 0x2056e0, MATCH each run; control 1346): the magnitude helper reading its input directly
// re-measured 1624 (5990 B); its stack references then follow retail's map (the same counts per offset, ours 4 bytes
// higher from the middle of the frame up: 0x158/0x15a and 0x1e4/0x1e8 for retail's 0x154/0x156 and 0x1e0/0x1e4; 133
// references at +0x80 or above in both), so the 248 missing bytes are not displacement size: retail has 25 more movss,
// 9 more mulss, 6 fewer fld/fmul, three imul and a second ret. On that direct form: local_5 declared before local_4 1624
// (same code); the ray record local_19 declared at function scope 1608; the scale helper as three component statements
// 1649 (5974 B); the two normalisations through the existing normalize3d 1609 (5990 B). Body unchanged.
// M3 lane 5 r234 (h2m fast with 0x2056e0, MATCH each run; control 1346). Retail materialises `field_82 || field_83` into
// DL right after the field_90 store and tests it at the local_65 choice, recomputing the `||` at both impact calls; at
// that choice it reads local_58/local_60 straight from their homes where ours copies them into the magnitude snapshot.
// A named bool for the `||` ahead of the friction ternaries, tested at the choice, 1431 (6262 B); declared at the friction
// block's top and used at the choice and both impact calls 1440 (6195 B), that with the first loop's cross through
// function_206092 1442 (6195 B; function_206092 alone re-measured 1405 at 6238 B); the friction ternaries in the order
// local_62, local_64, local_63 on this body 1346 (same code); the else arm's residual as `local_60 = local_58;` then three
// in-place `-=` 1426 (6259 B). All restored.
// M3 lane 11 r157 (H2M_FAST with 0x2056e0, MATCH each run; control 1346, aligned 1399, register-masked 1100). The frame's
// scalar area is 0x44 bytes (17 cells) larger than retail's (cells 0x10..0x1ac against 0x10..0x168; the aggregates then
// start 0x44 higher). The first loop's cross through function_206092 1405 (6238 B, aligned 1403, masked 1126: retail's
// length but further off); the steering factor's product written field_8c first 1346 (same code: ours still loads field_8c
// first where retail loads the definition's +0x30 first); the sign applied as `*=` 1346 (same code), as a negation under
// an if 1396; local_13's sign factor in reals 1402. Body unchanged.
// M3 lane 2 r139 (h2m fast with 0x2056e0, MATCH each run; control 1346, register-masked aligned 896). Retail aligns two
// forward-only joins in the contact pass as if they were loop heads (0x207420 after the impact block and its skip arm,
// padded behind a `jmp`; the latch 0x207640 behind `mov edi, edi`) and does not align the wheel loop's head, where ours
// pads the wheel loop and neither join. Forms: the disabled contact's skip call with `continue` at the top of the pass
// (no else) 1351 (6238 B, retail's length; masked 908), with the result returned as `return !local_34 && ... ;` 1349
// (6243 B), with the magnitude helper reading its input directly 1634 (5990 B) and with both 1618 (6006 B); the wheel
// loop's cross through function_206092 re-measured 1405, with the `continue` 1425 (6211 B), with both other forms 1411
// (6238 B). The `continue` form does not pad the latch. Body unchanged.
// M3 lane 13 r149 (H2M_FAST with 0x2056e0, MATCH each run; control 1346): one `long local_6` counter for the wheel and
// contact loops (retail keeps both counters in one slot) 1345 (frame unchanged, ours already shares the slot); that on the
// direct magnitude helper (re-measured 1624, 5990 B, frame 0x590) 1626; in the slip arm, the sqrt of 0.9 times the friction
// term computed ahead of the normalisation of the copied direction (retail interleaves that sqrt with the copy) 1428 (6256
// B); that with the scale helper as three component statements 1371 (6224 B); both on the direct magnitude helper 1665
// (5986 B). LEAD, not kept: with the straight-line scale and the direct magnitude, the slip arm is retail's shape (the
// normalised components stay in XMM registers, the factor is formed in SSE, three mulss and three stores) and the opcode
// census moves to retail's (mulss 118 -> 126 against 127, fld 35 -> 32 against 29): retail's scale helper is straight-line
// and its normalisation scalar-replaces the copy. Retail still has 25 more movss, three imul (the contact loop indexed from
// its counter each pass) and a second ret. Body unchanged.
// M3 lane 11 r162 (H2M_FAST with 0x2056e0, MATCH each run; control 1346). Reading: retail's two exits are `mov eax, 1` and
// `xor eax, eax` each with its own `add esp; ret`, the four pops ahead of the final tests (only EAX, ECX and EDX used
// there), the shape matched bool functions such as 0x1ecef0 get from `return a && b ...;`. Here every spelling gives one
// exit with `xor al, al` set ahead of the tests and `mov al, 1`: the result as `if (...) return true; return false;` 1345
// (retail's 6238 B, frame 0x5cc: the result's cell goes), as `return !local_34 && ...;` 1345 (same code), as an early
// `return false` on the negated chain 1345 (same code; the fast check flags these as building differently from the full
// build); the magnitude helper reading its input directly 1624 (5990 B, frame 0x590) with or without the two returns.
// The 0x5d0 frame and velocity homes remain. Typed point pointer/reference/direct access 1346/1346/1413.
// Base-first shared point/both velocity sums 1346/1346; shared output sums 1346; read-only point definition 1346; ray-origin aggregate 1426.
// NOT kept: direct typed reads reach 6238 bytes but regress at the same frame. The ray aggregate grows the body and the other forms tie.
// M3 lane 12 r184 (h2m fast with 0x2056e0, MATCH each run; control 1346, aligned distance 1727; opcode census against
// retail). The scale helper straight-line re-measured 1412 (6238 B, aligned 1514, frame 0x44 -> 0x3c over retail's); with
// the magnitude read in place re-measured 1649 (5974 B, aligned 1514, frame exactly retail's 0x58c, 264 bytes short: 28
// fewer movss, 7 fewer mulss, six more each of fld/fmul/fstp = the slip arm's factor and scale in x87 where retail
// stores the early sqrt as a float and scales in SSE); the slip arm's sqrt ahead of the copy and normalisation
// re-measured 1428 (6256 B, aligned 1477); all three 1665 (5986 B, aligned 1447, then 25 movss short). New on that
// combination: the ending as `if (...) return true; return false;` 1619 (6031 B, aligned 1421, 9 movss short, but two
// epilogues with 8 pops against retail's 4 pops ahead of the final tests); as one `return a && b && c && d;` 1621 (6002
// B, aligned 1437). Body unchanged.
// M3 lane 16 r185 (h2m fast with 0x2056e0, MATCH each run; control 1346, register-masked aligned 922). Frame reading: the
// five magnitude snapshot copies each keep a 12-byte home of their own (their ranges do not share one), 60 of the 68
// extra frame bytes; retail's first normalise site (0x206c25-0x206c66) stores the vector's own three components to
// its home and squares them from there in x87, so retail's helper reads its input in place and the 264 bytes that form
// loses are elsewhere. Re-measured: magnitude in place 1624 (5990 B, masked 929), with the straight-line scale 1649
// (5974 B, 987), with the slip arm's root first 1665 (5986 B, 940), with two returns 1619 (6031 B, 885). New: magnitude
// in place with two returns 1599 (5987 B, masked 903), with the straight-line scale too 1620 (6019 B, 943); the frame is
// 0x590 in each of the six here, with 13 vector homes (retail has no contact-velocity one). First loop: retail preloads
// all three components of the first cross operand at the loop top, forms the six products before any store, stores
// the cross (j, k, i) and re-reads j and k for the sum. cross3f_staged there 1421 (6238 B, masked 955: three preloads,
// but the sum folds into the cross registers and the first stores go); the file's k/j/i helper there 1405 (6238 B, masked
// 975: two preloads, k and j stored and re-read, the i product folds into the sum). That helper's statements as i, j, k
// with the first loop through it 1385 (masked 967), at its three sites only 1375 (938); as j, k, i with the first loop
// 1415 (6238 B, 936), at its three sites only 1345 (934; one point, not kept). The first loop's element pointer is
// biased +0xe4 (retail +0xe0) in every form. The two joins retail pads like loop heads (0x207420, 0x207640) each follow
// the statement whose else arm is the skip call; retail's wheel loop head is unpadded only because it would need 12
// bytes. `do {} while (false);` after the hit/skip statement, at the end of the contact loop's body, or both: 1346 each
// (same code, no new padding). Body unchanged.
// M3 lane GB r1 (fast with 0x2056e0, MATCH each run; control 1346, 6243/6238 B, frame 0x5d0, aligned exact/reg-masked
// 1419/883). Diagnostic `/Ob1` flag 1346 (same code). Retail XBE scan: 0x206090 is the only function with a non-loop
// 16-byte pad immediately after a `call` (0x207639 call 0x2079f0 -> 0x207640; and 0x20740a -> 0x207420). Tried 10 forms:
// line 1233 float-product magnitude as double accumulator 1371 (aligned 1486/903) or float expr 1371 (1490/903);
// inline add-unique helper with early return for duplicate-impact search 1342 (6243 B, 1455/1455 insns, 1417/881:
// reproduces 0x2073cf-0x20740f instruction-for-instruction; 4 pts / 1-caller helper, NOT kept); that + lazy 0.2s 1350
// (846); retail final impact loop (fresh pool count/reads, list ptr inside match, lazy 0.2s, direct return) 1491 (6303 B,
// 1405/828); direct proj56 + normal_copy in proj58 1407 (6288 B, 0x5e0); shared normalize_inline + add-unique + direct
// proj56 1475 (6160 B, 0x5cc); combined retail shapes on direct magnitude (direct i,j,k mag, straight-line scale, unstaged
// k,j,i cross at all 4 sites, float wave, in-place contact vel, early slip sqrt, float line 1233 mag, add-unique, retail
// final loop + direct return) 1672 (5973 B, frame 0x580, aligned 970/631); that + direct proj56 + normal_copy + found-first
// tail 1600 (6047 B, 0x598, 956/669); that + contact ptr 1571 (6076 B, 0x598, 960/641). NOT kept: contact loop stays
// strength-reduced in all forms; body restored.
// Contact-normal ramp numerator 1344 at 6238 bytes, angle range 1404, both 1381.
// Named quotient 1344, named limited fraction 1346, live quotient in explicit arms
// 1350, one quotient in explicit arms 1344, and quotient limited in place 1424.
// NOT kept: best gain two points, with the same 0x5d0 frame and displaced velocity homes.
// Kinetic coefficient root after one direction store (i/j/k, i/k/j, j/i/k, j/k/i,
// k/i/j, k/j/i), after two (i/j/k, k/j/i), or with a named radicand and the root
// after the copy or before its final store: 1446/1446/1446/1446/1446/1446/1446/1446/1403/1403. NOT kept: every form
// regresses while retaining the frame and displaced component/velocity homes.
// M3 lane 8 r238 (plain, 0x2056e0 MATCH each run; control 1346): `local_2` declared ahead of `local_1` 1346 (the entry
// spill stores then come in retail's order, ESI's first, but at homes 0x98/0x3c against retail's 0x54/0x24); the two
// 0xba1d0 outputs declared local_5 first 1346 (same code); in the first loop `local_10` ahead of `local_9` 1346 (same
// code, element pointer still biased +0xe4 where retail has +0xe0), `local_8` ahead of `local_7` 1347; the magnitude
// helper as `sqrt(dot3f(v, v))` 1624 (5990 B, the direct-read code).
// Remove magnitude snapshots for projected/residual/alternate velocity 1375/1379/1396,
// pairwise 1421/1458/1402, all three with normalizer assignment or memcpy 1518/1518.
// NOT kept: the frame shrinks as far as 0x5a8, but the body shortens and the score rises.
// The retail 0x58c frame and earlier component and velocity homes remain different.
// GB6 (fast with 0x2056e0, then plain with 0x2056e0/0x2078f0/0x2079f0/0x207ab0 all MATCH; control 1346, 6243/6238 B,
// al=1555, mk=1178; 25 trials): 1334 at 6243 B (fr=0x5d0, al=1376, mk=1143), NOT kept (coordinator review: statement
// order, a moved declaration and one-use locals, all kept only with a byte match), from: (1) local_19.field_c
// computed ahead of local_19.field_0 (1341 alone; retail lines 175-194 store field_c before field_0); (2) local_48 = *g_4687b0
// moved to the top of `if (field_84 > 0.0001f)` (1343 alone, al=1444; retail loads g_4687b0 ahead of field_88/field_94);
// (3) speed_change_limit = g_510c54->field_2_3 * local_47 ahead of getMass() with minimum_mass/maximum_mass bounds for PIN
// (1342 alone, al=1486, mk=1162; matches 61 retail insns across the mass/impulse block, keeping g_510c54 in a register;
// pairs 1338/1337/1339, dv-only 1346, bounds-only 1366). Also tried: wheel local_25 interpolation with lazy
// `local_24 - local_23` inside `||` and if/else bounds 1383 (exact 6238 B, al=1521, mk=1141: matches all 49 insns of retail's
// local_25 block; on the 1334 form 1375 at 6238 B, al=1330, mk=1106); local_20 inside `if (local_7[0x81])` 1447 (6259 B); bundle
// with float wave + in-place vector3f_add_scaled 1461 (al=1304, mk=1098); typed query array 1346; `for (;;)` contact loop
// 1346; direct local_56 + early local_48 1387 (6208 B); split local_54 on the 1334 form 1390 (6238 B, al=1274); split local_13
// square on the 1334 form 1420 (al=1277, mk=1095); all together 1461 (al=1231, mk=1041); dot3f(&local_42, local_76) 1334; direct
// return on the 1334 form 1349; diagnostic `/Op` flag 1833 (6652 B).
// The 0x5d0 frame, escaped velocity homes and ray setup schedule still differ.
// Typed ray 1346; with point setter 1331, shared direction scale 1318, shared hit scale 1370, both scales 1357;
// shared ray length 1370, with point setter 1375, shared hit-normal address 1340 at 6238 bytes.
// Direction before origin with shared length 1367, origin memcpy 1346, in-place direction scale 1444, shared length
// with shared direction scale 1389, copied hit normal 1379. NOT kept: the point setter uses SSE for retail's integer
// origin copy. The direction scale still publishes the origin and tests admission before the first direction store.
// The hit-normal address gains six at retail length, below the threshold.
// The magnitude captures still displace the escaped velocity homes.
// Individually captured coordinates with named squares, a planar subtotal,
// or real or double running sums 1654/1612/1654/1613.
// The same direct-field reducers 1612/1612/1612/1623.
// NOT kept: removing the full input capture loses the retail extent.
// The point loop still uses different relative-vector homes in the 0x5d0
// frame. Typed point with vector pointers, direct fields, reference, or index
// 1346/1394/1394/1372. Typed settings alone or with typed points 1346/1346.
// Shared point difference or with k-first cross 1379/1415.
// NOT kept: both retail-length forms raise the score and retain the frame gap.
// M3 lane 3 r311 (fast screen with 0x24590 and 0x30bf0; the fast image builds this body at 1345 with the "builds
// differently" caveat, the full build 1346). Frame reading: the five inlined copies of the magnitude helper's input
// snapshot take five separate 12-byte homes (60 bytes of the 0x44 by which this frame exceeds retail's), where retail's
// magnitudes read the vectors' own stack homes just after their component stores. Forms: the magnitude helper returning
// the existing out-of-line magnitude (inlined) 1624 (5990 B); the sign flip as `local_11 *= ... ? -1.0f : 1.0f` 1345,
// the gain as `1.0f + rate * local_15` 1345 and the speed sum passed as `local_18 + local_17` 1345 (all three the same
// code); the steering product negated on its first factor (`-field_74.j * x`) 1349 (6243 B). Body unchanged.
// M3 lane 16 r193 (H2M_FAST with 0x2056e0, MATCH each run; the reduced image builds this body at 1346 like the full
// build; control masked-aligned 922): retail adds the base velocity first (`local_4.k + cross.k`); the wheel loop's add as
// the existing vector3f_add(&local_4, local_10, local_10) 1346, the contact loop's as well 1346 (same code: the sum's
// operands follow ranks); the wheel loop's velocity cross through this file's k-first cross helper 1405 (6238 B), through
// cross3f_staged 1421 (6238 B); the centre (+0x60) typed as a point with both lever arms through the existing
// vector3d_from_points3d 1344 (1455 instructions like retail, masked-aligned 946). Retail's contact loop recomputes its
// three element addresses with `imul` from the counter each pass and reloads the state pointer from its argument slot,
// where ours steps induction pointers.
// M3 lane 3 r314 (H2M_FAST with 0x2056e0, MATCH each run; control 1346, masked-aligned 922 over 85 clusters; the wheel loop's
// element pointer stays biased +0xe4 against retail's +0xe0 in every form): the velocity pointer declared ahead of the
// lever-arm pointer 1346 (masked 924); the speed sum named once for the ray scale and the hit fraction 1370 (6238 B); the
// definition element pointer computed ahead of the state element pointer 1347; local_18 computed ahead of local_17 1346
// (same code); the steering choice's two magnitudes through fabs 1397 (6240 B). Body unchanged.
// where ours steps induction pointers. On the point-typed centre form (1344): both base-velocity adds through vector3f_add
// as well 1344 (same code); every call of this file's scale helper through the existing vector3f_scale 1425 (6211 B);
// both end-of-loop torque crosses through cross3f 1378 (6238 B).
// M3 lane 13 r166 (reduced-image screens, control 1346): the two output clears written arg_2 first 1346 (same code); the
// first normalizer's scaling through this file's scale helper with 1 / length, in place, 1419; the magnitude helper's
// three products cast to double, copy kept, 1393; the definition pointer read after the datum pointer 1346 (same code);
// the result array and query buffer declared at the top of the function 1346 (same code, frame still 0x44 over).
// Body unchanged.
// M3 lane 8 r248 (fast screen with 0x2056e0, MATCH each run; control 1346; body unchanged). The magnitude helper reading its
// input directly re-measured 1624 (5990 B): its first magnitude then stores the vector at [esp+0x80]/0x7c/0x78 and the
// result at [esp+0x14] as retail (the x87 leaves are those stores), but the body is 248 B short elsewhere. Forms: the
// direct read with every square cast to double 1566 (6022 B); the input copy declared `const` 1346 (same code); the copy
// as a separate assignment 1346 (same code); the helper as `(real)sqrt(dot3f(arg_0, arg_0))` 1624 (5990 B); the copy
// kept with the squares summed into a `double` (k, j, i) 1370.
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
                        local_49->rigid_body_linear_velocity_change(local_50, &local_55);
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
                        if (local_75 >= local_33 && (dword)local_33 < 32)
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
