// @flags /O2 /Oi /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include <math.h>

struct s_input_state
{
    byte unknown00[0x10];
    byte values[0x38];
};

struct s_gamepad_preferences
{
    real look_sensitivity_horizontal;
    real look_sensitivity_vertical;
    char button_map[16];
    short unknown18;
    bool unknown1a;
    bool unknown1b;
};

struct s_217c40
{
    byte field_0[16];
    word field_10[16];
    real field_30;
    real field_34;
    real field_38;
    real field_3c;
    real field_40;
    real field_44;
    real field_48;
    byte field_4c;
    byte field_4d;
    bool field_4e;
    byte field_4f;
};

extern byte g_4e61b9;
extern s_input_state g_4e61dc[3];
extern s_input_state g_4e630c;
extern byte g_51ea18[0x6f * 4];
extern bool g_51ebcc[4];
bool function_218510(long arg_0);

PRIVATE real const g_4570a0[4] =
{
    0.7853981852531433f, 2.356194496154785f,
    -0.7853981852531433f, -2.356194496154785f
};

PRIVATE __forceinline real function_217c41(real arg_0)
{
    return arg_0 < -1.0f ? -1.0f : (arg_0 > 1.0f ? 1.0f : arg_0);
}

/* standard marker before the body matches (frame alignment): retail's
   prologue self-aligns its frame (`push ebp; mov ebp, esp; and esp, -8`),
   takes no arguments and returns with a bare `ret`. Our build self-aligns
   such a function only when its address escapes (a scan of the 170
   self-aligning functions showed this). Retail holds no reference to its
   address, and its caller 0x12b450 is LTCG code. With the marker 0x12b450 and
   0x12b690 match; without it they don't. The body itself doesn't match yet. */
// score 25 (was 25; earlier 474): the x87 order of the first offset (retail keeps `angle - table` on the stack past both magnitudes), then the two hoisted
// constant loads (retail loads the coordinate scale first; ours 1.0, which
// the D3D library defines; retail's copy of the scale lies in the Bink object's data at 0x43ba70). Earlier: frame 0x38 versus 0x40; angle homes and snap-work lifetimes differed.
// Earlier 501 from 541.
// Role names 564; four-controller bound 541; ordinary snap inline 1438; short quadrant 564; shared default 548; combined 562.
// Iteration-wide axes 541; direct double-angle helper 541; offset before magnitude 556;
// shared axis signs 735; reverse layout-case order 541.
// Native short coordinates 603; native long coordinates 551; quadrant through tests 555;
// preference rereads 543; late preference/output bases 598.
// Snap magnitude value parameter 541; snap axes by value 541; short four-controller ordinal 554; byte selected button 539; shared mapping index 541.
// Shared coordinate/trig/coordinate-trig workspaces 541/541/540; shared normalization scale 501;
// coordinates-scale 541; trig-scale 501; all scratch values/scoped normalization 540/540. Scale workspace retained.
// Separate rounded-angle double formal 541; paired clamps before stores 548;
// reversed final axis stores 541 at retail length; double/paired, double/reversed,
// paired/reversed, all three and native quadrant on all three 548/541/548/548/548.
// Round 197: Eight snap-helper pointer/real-angle/double-angle/threshold formal orders 501 each. Frame 0x38, helper and complete original body restored; both callers remain matched.
// Shared prepare/apply stages, each called twice: immediate 501; both prepared 564; magnitude
// first 561; original double-angle preparation 559 (retail frame 0x40); absolute offset 531;
// absolute/magnitude-first 531; native quadrant 564; value-input application 564. All restored.
// Round 368: complete four-axis scaled/normalized/both products 501/501/501; shared snap attenuation alone
// or with each/all product forms and angle role names 566 each. Original body and helpers restored.
// Raw coordinates reused as first/second/both normalized component locals 501/501/501;
// vertical normalized components first on original/first/second/both reuse 551/551/551/551;
// both coordinate reuses with shared trig magnitudes 501. All restored; frame 0x38 remains.
// Round 161: First/second/both double coordinate carriers 564/553/586;
// native double trig magnitudes alone/first/second/both coordinates 557/588/576/580;
// actual input axes with all carriers 580; early first/second/both real angles 562/543/566.
// All original carriers, observations, casts and arithmetic restored.
// Round 208: existing shared controller successor 515 at 2024/2012 bytes; independent
// raw/angle/trig/normalization phase schedules 546/537/537/536/536/543/560/508. Both exact callers
// preserved in all nine comparisons. All restored; frame 0x38/0x40, rounded angle homes and
// simultaneous snap work remain.
// Round 370: double threshold, rounded absolute-angle carrier and rounded-angle formal
// domains, then all with short quadrant 501/508/508/501/501/508/508/508; helpers and body restored.
// Round 254: native short copy/first-inverse/both/second-inverse/copy-second/both-inverse/all
// mapping induction 625/538/626/535/646/544/648; all byte 648. Original long loops restored.
// Round 220: Selected trig magnitude in shared/separate native phase scalars, with long/word/byte/char
// controller induction, 501/548/539/539 each. Byte/char recover frame 0x40 and first +0x1b but regress;
// every form retains 2012 bytes and both matched callers. Original body restored.
// Round 27: retail's instruction order is followed closely, but our frame is 0x38 against retail's
// 0x40: retail holds one more dword local below the angles, so its dwords sit four bytes higher and
// its double angle at 0x48 against our 0x40, and every stack reference differs; no form found.
// MiniMax S (10-31): no new form, no lower score. Our branch condition is retail's; the apparent inversion
// comes from the constant pool order (our 0x4a7f28 is 2.356194496154785 and 0x4a7f2c is 0.7853981852531433,
// so the compiler swapped the two against their order in the source). What remains is the frame (0x38
// against 0x40), the shared quadrant/magnitude preparation and the slot and register rotations.
// Retained output/preferences declaration order: the per-controller block and the preferences are declared in retail's order,
// so the block pointer keeps the register retail loads it into. The frame 0x38 versus 0x40, the loop
// counter's home and the remaining field and register rotations remain.
// Round 222: first/second/both native coordinate reads at every use 550/474/550;
// normalized-output/double-angle/both/coordinate-trig/all scalar identities 474 each
// on the shared-scale, output-first graph. Original body and both helpers restored.
// Earlier 474 from 501: the per-controller block and the preferences are declared in retail's order,
// Round 338: native external input axes/channel-word/trigger views in seven subsets,
// then all with a read-only receiver, 474/474/474/474/474/474/474/474. Original input layout,
// observations, helper input copies and complete body restored; frame/angle/snap homes remain.
// AC474: Existing snap-producer input-pair/angular-scratch/magnitude lexical identities,
// seven subsets then complete quadrant identity: 474/474/474/474/474/474/474/474.
// Original helper and full body restored; frame 0x38/0x40 and angle/snap homes remain.
// AC484: Separate native short/word/byte/long quadrant components with a short total
// 474/474/488/474; with a long total 474/474/488/474. Original components, helper
// and body restored; frame 0x38/0x40, rounded angles and simultaneous snap homes remain.
// B398: Native axis-pair snap returns with scalar/value/pointer/reference inputs,
// component publication 586/706/586/586; aggregate publication 586/706/586/586.
// NOT kept: native pair storage does not match and grows the frame/length.
// Original helper, body and includes restored; both exact callers stay matched.
// B400: One rounded snap angle for offset/absolute-angle consumers, with first/second/both
// atan2-producer lifetimes 485/558/558/561; producer-only first/second/both 501/499/563;
// complete rounded-angle graph with entry declarations 561. NOT kept: no exact match or gain;
// original helper, full double inputs and complete body restored; frame/angle/snap homes remain.
// B410: Existing clamp result-first branch/default/complete/lower-default/upper-default/
// bound-predicate/lower-expression/upper-first results: 577/474/483/545/549/826/558/538.
// NOT kept: neutral or nonretail result/compare shape and frame/length regressions.
// Original six-use clamp, snap helper, complete body and alignment evidence restored.
// P304: Existing snap-input observations at quadrant/magnitude/dominance-sign consumers,
// masks 1/2/4/3/5/6/7, and immutable input/output separation: eight forms 474 each.
// Original helper and body restored; frame 0x38/0x40 and angle/snap homes remain.
// M358: Two-mode snap admission, positive/complementary bool/byte/short/long:
// 474 each. NOT kept: one-use admission predicates do not match; original helpers
// and complete body restored; frame 0x38/0x40, angle and snap-work homes remain.
// DH332: Standard lower/upper clamps and paired divisor selection, seven subsets then real magnitude API:
// 980/1001/1211/555/982/992/1250/1250. NOT kept: all regress and miss native frame/length; original helper and standard alignment evidence retained.
// Original complete body and contracts restored.
// DH336: Native coordinate/axis/double-angle array subsets, then point pairs:
// 474/474/473/475/474/474/475/475. NOT kept: the one-point angle-array gain
// and grouped storage need equality; original helpers and complete body restored.
// F367: Complete snap squared components/norm in real or double storage: six forms 474; rounded magnitude in double with component or sum node 616 each (native 0x40 frame, 1932/2012 bytes). NOT kept: storage/precision nonmatches; original helpers, full body and standard alignment evidence restored.
// Round 149 (m3 lane 5; fast at 2012 bytes unless marked, kept forms marked): the structure, read from retail: loop on a NONE
// sentinel with a named next (kept) 113; prepared snap (both quadrants, offsets, magnitudes first) with an apply helper 568 (2040),
// with the clamp as one conditional expression 309 (1996), macro or if/else if 309/568; flat apply, no pointer helper (kept)
// 149; fabs written at each use of the offset (kept) 140, a named absolute with early fabs 381-384; field_38 stored after the first
// subtraction in cases 0, 2, default (kept) 103; coordinates read at each use, so they are defined after the scale (kept; product
// loads the coordinate) 89; separate scale per stick 569 (1972); the two sticks as point2f (kept; every slot retail's) 4; an array of
// two pairs 21; real angles straight from atan2, no doubles (kept; fixes the qword/dword `fst` order) 2 plain, as `(real)fabs(first_offset)` at each
// use of the offset, not kept without a match (a named absolute 25, kept; in place on the offset 88, first only 25; named in the branch 404
// at 2024 bytes). Named constant at
// 0x43ba70 (private, extern or local static) 2 plain, MATCH only on a one-function reduced image: not retail's code (a literal
// from a library object), so a literal is kept. Also the constant in the Bink stub (diagnostic) 2 plain. Lead: load order follows
// where the constant's COMDAT comes from at link; it needs a non-LTCG object holding it ahead of the D3D library's object. Identity pointers 1-5/6-7 ahead
// of the first angle, an input-bytes pointer: 2/3/2. Consecutive retail constant loads run both ways (208 ascending, 249 descending), so address order is no general rule.
// On the kept body: the apply step as a by-value or reference helper 629 (2148 bytes)/538 (1996); offsets written at their use, both/first only 405 (1996)/25.
// DL261: Shared negative-axis predicates; existing value/output and in-place clamp completion: 652/652/652/652/623/623/640/640/474/474.
// NOT kept: no exact native form; original complete body and all temporary contracts/types restored.
// P340: Common dominant-axis selection across snap and fallback, branch-owned then conditional
// bool/byte/short/long results: 665/665/665/665/585/585/585/585. NOT kept: native branch ownership/frame
// and extent are not recovered; original helper and complete body restored. Frame 0x38/0x40 and
// angle/snap homes remain; both exact callers survive.
// AC551: Double first/second/both offsets, double first/second/both absolute offsets, all four
// doubles, all four immutable real offsets: 25/395/395/471/388/521/514/25. NOT kept: no qualifying
// supported improvement; complete executable source restored. First offset x87 retention and
// hoisted literal-load order remain.
// DL268: Native sqrtf first/second/both magnitudes, fabsf paired offsets/angles/trig magnitudes,
// float trig functions and atan2f producer: 25/25/25/25/25/25/25/25. NOT kept: no gain;
// original scalar interfaces and complete body restored. First offset x87 retention and literal-load order remain.
// DH378: Paired real-angle identities (two orders), offset identities and absolute-offset identities 25 each;
// first/second/both compound offset subtraction producers 25/25/25; existing paired magnitude API 26.
// NOT kept: no exact match or improvement; original complete body, helper, inputs and includes restored.
// Round 97 (m3 lane 12, fast, on the fabs-at-each-use 2 form): the two hoisted loads follow each constant's first mention in
// the loop body (a probe use of the scale ahead of the first `1.0f /` flips them to retail's order, one after it does not), so
// retail's body reaches the scale first. Division by a named maximum 20 (one reciprocal from 1.0, 1.0 still first); the scale
// as `/ 32767.0f` 2; named maximum then `1.0f / maximum` 2; `coordinate * scale * (1.0f / maximum)` 20 (the constant is
// reassociated to the end, 1.0 still first); scaled stick locals ahead of the angles 562 (1988 bytes).
// MiniMax B (11-04): no new form, no lower score.
// Round 143 (m3 lane 8, fast, all on the fabs-at-each-use 2 form): `coordinate * scale / maximum` 20 (one reciprocal
// from 1.0, still loaded first); scaled stick locals between the trig and the reciprocal 536 (2020 bytes; scale loads
// first, products early); a named scale local at the top of the stick block 2 (folded, no flip); a two-use inline stick
// helper taking the scale as an argument 537 (1996 bytes, 1.0 first); named float coordinates with
// `x * scale * (1.0f / maximum)` 581 (2036 bytes). Diagnostic (reverted): the scale used in a separate function earlier
// in this file 2, so the order is set inside this function after inlining and the reciprocal rewrite, not by the module.
// Round 74 (m3 lane 2, fast, on the fabs-at-each-use 2 form): reciprocal as `scale / maximum` with one product per
// coordinate 588 (1972 bytes, a real divss); the clamp helper defined after the function 2 (same code, so not a per-file
// constant order); scaled axes stored first and normalized after the trig 562 (1988 bytes; re-measured, as round 97's
// scaled stick locals); the reciprocal taken in each arm of the maximum 458; a `real const` coordinate scale at the loop
// top 2 (same code; re-measured, as round 143's named scale local). Map: our 1.0 comes from the D3D library's state
// member and the scale from src/unknown_157450.cpp; retail's scale lies in the Bink library's data. Then: atan2 over
// coordinates already scaled by 1/32767 (named, products from them) 571 (1972 bytes); the raw coordinates as a named
// point2f for atan2 and the products 554 (1996 bytes).
// Round 115 (m3 lane 9, fast, all on the fabs-at-each-use 2 form, which the fast image reproduces): the scaled
// coordinate passed to a two-argument forceinline boost helper that holds the reciprocal (`value * (1.0f / maximum)`
// or `value / maximum`) 20/20 (1.0 still first; the product becomes inverse * coordinate); the scaled x named ahead of
// the reciprocal, y in place 557 (1988 bytes: computed ahead of the trig), the same after a named maximum 6 (the scale
// product then precedes the inverse product in both sticks, 1.0 still first: a one-use value is forwarded to its use,
// behind the reciprocal); the reciprocal written in place in both products, as `coordinate * (1.0f / maximum) * scale`,
// `scale * (coordinate * (1.0f / maximum))` or `(1.0f / maximum) * coordinate * scale` 20/20/20, as
// `coordinate * ((1.0f / maximum) * scale)` 578 (1972 bytes); the reciprocal assigned inside the x product, after the
// coordinate 2, after `coordinate * scale` 2, after `scale * coordinate` 2, the last with y as `coordinate * inverse *
// scale` 2 (all same code); the reciprocal, or only the maximum, through a two-argument forceinline helper 2/2 (same
// code); the clamp as a macro 2 (same code); the numerator as the integer 1, 2 (same code), as a double 1.0, 561 (2044
// bytes, a double division). In every form the 1.0 of the first reciprocal is reached before the scale.
// Round 119 (m3 lane 9, fast, on the kept 25 body): the two offsets as one point2f pair 90, as a `real [2]` array 90:
// either gives retail's offset block instruction for instruction (first offset subtracted on the x87 stack ahead of
// both magnitudes, second offset by SSE), but the pair takes an 8-byte slot after the double angle, so every home
// below the axes moves down 4 and the axes and the double 8 (the pair named to sort first or last: 90/90). The two
// magnitudes as a point2f pair 51 (the offset is still forwarded to its use).
// @retail 0x217c40 standard
void function_217c40(void)
{
    long local_0 = 0;
    while (local_0 != NONE)
    {
        s_input_state *local_1 = NULL;
        if (g_4e61cc[(short)local_0])
            local_1 = g_4e61b9 ? &g_4e630c : &g_4e61dc[(short)local_0];
        if (local_1)
        {
            point2f first_axes;
            point2f second_axes;
            s_217c40 *local_21 = &((s_217c40 *)(g_51ea18 + 0x70))[local_0];
            s_gamepad_preferences *local_20 = &((s_gamepad_preferences *)g_51ea18)[local_0];
            real first_angle = (real)atan2((real)*(short *)((byte *)local_1 + 0x42), (real)*(short *)((byte *)local_1 + 0x40));
            real local_6 = (real)fabs(sin(first_angle));
            real local_7 = (real)fabs(cos(first_angle));
            real axis_scale = 1.0f / (local_6 > local_7 ? local_6 : local_7);
            first_axes.x = function_217c41((real)*(short *)((byte *)local_1 + 0x40) * axis_scale * 0.000030518509447574615f);
            first_axes.y = function_217c41((real)*(short *)((byte *)local_1 + 0x42) * axis_scale * 0.000030518509447574615f);
            real second_angle = (real)atan2((real)*(short *)((byte *)local_1 + 0x46), (real)*(short *)((byte *)local_1 + 0x44));
            real local_15 = (real)fabs(sin(second_angle));
            real local_16 = (real)fabs(cos(second_angle));
            axis_scale = 1.0f / (local_15 > local_16 ? local_15 : local_16);
            second_axes.x = function_217c41((real)*(short *)((byte *)local_1 + 0x44) * axis_scale * 0.000030518509447574615f);
            second_axes.y = function_217c41((real)*(short *)((byte *)local_1 + 0x46) * axis_scale * 0.000030518509447574615f);
            for (long local_22 = 0; local_22 < 16; local_22++)
            {
                long local_23 = (byte)local_20->button_map[local_22];
                local_21->field_0[local_22] = local_1->values[local_23];
                local_21->field_10[local_22] = *(word *)((byte *)local_1 + local_23 * 2 + 0x20);
            }
            local_21->field_30 = *((byte *)local_1 + 6) * 0.003921568859368563f;
            local_21->field_34 = *((byte *)local_1 + 7) * 0.003921568859368563f;
            local_21->field_4e = false;
            long local_24 = 6;
            if (local_20->button_map[7] == 6)
            {
                local_24 = 7;
                local_21->field_4e = true;
            }
            local_21->field_4c = 6;
            for (long local_25 = 0; local_25 < 16; local_25++)
            {
                if ((byte)local_20->button_map[local_25] == local_24)
                {
                    local_21->field_4c = (byte)local_25;
                    break;
                }
            }
            local_21->field_4d = 4;
            for (long local_26 = 0; local_26 < 16; local_26++)
            {
                if (local_20->button_map[local_26] == 1)
                {
                    local_21->field_4d = (byte)local_26;
                    break;
                }
            }
            short local_27 = local_20->unknown18;
            if (local_27 == 2 || local_27 == 3)
            {
                short first_quadrant = (first_axes.x < 0.0f ? 1 : 0) | (first_axes.y < 0.0f ? 2 : 0);
                short second_quadrant = (second_axes.x < 0.0f ? 1 : 0) | (second_axes.y < 0.0f ? 2 : 0);
                real first_offset = first_angle - g_4570a0[first_quadrant];
                real second_offset = second_angle - g_4570a0[second_quadrant];
                real first_magnitude = (real)sqrt(first_axes.y * first_axes.y + first_axes.x * first_axes.x);
                real second_magnitude = (real)sqrt(second_axes.y * second_axes.y + second_axes.x * second_axes.x);
                real first_absolute_offset = (real)fabs(first_offset);
                if (first_absolute_offset < 0.6108652353286743f)
                {
                    real first_absolute_angle = (real)fabs(first_angle);
                    if (first_absolute_angle < 0.7853981852531433f || first_absolute_angle > 2.356194496154785f)
                    {
                        first_axes.x = (first_axes.x < 0.0f ? -1 : 1) * first_magnitude;
                        first_axes.y = (1.0f - first_absolute_offset * 1.6370222568511963f) * (first_axes.y < 0.0f ? -1 : 1) * first_magnitude;
                    }
                    else
                    {
                        first_axes.y = (first_axes.y < 0.0f ? -1 : 1) * first_magnitude;
                        first_axes.x = (1.0f - first_absolute_offset * 1.6370222568511963f) * (first_axes.x < 0.0f ? -1 : 1) * first_magnitude;
                    }
                }
                else if (fabs(first_axes.x) > fabs(first_axes.y))
                {
                    first_axes.x = (first_axes.x < 0.0f ? -1 : 1) * first_magnitude;
                    first_axes.y = 0.0f;
                }
                else
                {
                    first_axes.y = (first_axes.y < 0.0f ? -1 : 1) * first_magnitude;
                    first_axes.x = 0.0f;
                }
                first_axes.x = function_217c41(first_axes.x);
                first_axes.y = function_217c41(first_axes.y);
                real second_absolute_offset = (real)fabs(second_offset);
                if (second_absolute_offset < 0.1745329201221466f)
                {
                    real second_absolute_angle = (real)fabs(second_angle);
                    if (second_absolute_angle < 0.7853981852531433f || second_absolute_angle > 2.356194496154785f)
                    {
                        second_axes.x = (second_axes.x < 0.0f ? -1 : 1) * second_magnitude;
                        second_axes.y = (1.0f - second_absolute_offset * 1.6370222568511963f) * (second_axes.y < 0.0f ? -1 : 1) * second_magnitude;
                    }
                    else
                    {
                        second_axes.y = (second_axes.y < 0.0f ? -1 : 1) * second_magnitude;
                        second_axes.x = (1.0f - second_absolute_offset * 1.6370222568511963f) * (second_axes.x < 0.0f ? -1 : 1) * second_magnitude;
                    }
                }
                else if (fabs(second_axes.x) > fabs(second_axes.y))
                {
                    second_axes.x = (second_axes.x < 0.0f ? -1 : 1) * second_magnitude;
                    second_axes.y = 0.0f;
                }
                else
                {
                    second_axes.y = (second_axes.y < 0.0f ? -1 : 1) * second_magnitude;
                    second_axes.x = 0.0f;
                }
                second_axes.x = function_217c41(second_axes.x);
                second_axes.y = function_217c41(second_axes.y);
            }
            bool local_28 = local_20->unknown1a;
            if (!local_28 && local_20->unknown1b)
                local_28 = function_218510(local_0);
            switch (local_27)
            {
            case 0:
                local_21->field_3c = 0.0f - first_axes.x;
                local_21->field_38 = first_axes.y;
                local_21->field_40 = 0.0f - second_axes.x;
                local_21->field_48 = second_axes.y;
                local_21->field_44 = (local_28 ? -1.0f : 1.0f) * second_axes.y;
                break;
            case 1:
                local_21->field_40 = 0.0f - first_axes.x;
                local_21->field_48 = first_axes.y;
                local_21->field_44 = (local_28 ? -1.0f : 1.0f) * first_axes.y;
                local_21->field_3c = 0.0f - second_axes.x;
                local_21->field_38 = second_axes.y;
                break;
            case 2:
                local_21->field_40 = 0.0f - first_axes.x;
                local_21->field_38 = first_axes.y;
                local_21->field_3c = 0.0f - second_axes.x;
                local_21->field_48 = second_axes.y;
                local_21->field_44 = (local_28 ? -1.0f : 1.0f) * second_axes.y;
                break;
            case 3:
                local_21->field_3c = 0.0f - first_axes.x;
                local_21->field_48 = first_axes.y;
                local_21->field_44 = (local_28 ? -1.0f : 1.0f) * first_axes.y;
                local_21->field_40 = 0.0f - second_axes.x;
                local_21->field_38 = second_axes.y;
                break;
            default:
                local_21->field_3c = 0.0f - first_axes.x;
                local_21->field_38 = first_axes.y;
                local_21->field_40 = 0.0f - second_axes.x;
                local_21->field_48 = second_axes.y;
                local_21->field_44 = (local_28 ? -1.0f : 1.0f) * second_axes.y;
                break;
            }
            if (!g_51ebcc[local_0])
                g_51ebcc[local_0] = true;
        }
        else
            g_51ebcc[local_0] = false;
        long next = NONE;
        if (local_0 >= 0 && local_0 < 3)
            next = local_0 + 1;
        local_0 = next;
    }
}
