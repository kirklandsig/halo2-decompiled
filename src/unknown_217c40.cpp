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
    if (arg_0 < -1.0f)
        arg_0 = -1.0f;
    if (arg_0 > 1.0f)
        arg_0 = 1.0f;
    return arg_0;
}

PRIVATE __forceinline void function_217c42(real *arg_0, real *arg_1, real arg_2, double arg_3, real arg_4)
{
    real local_0 = *arg_0;
    real local_1 = *arg_1;
    long local_2 = (local_0 < 0.0f ? 1 : 0) | (local_1 < 0.0f ? 2 : 0);
    real local_3 = arg_2 - g_4570a0[(short)local_2];
    real local_4 = (real)sqrt(local_1 * local_1 + local_0 * local_0);
    real local_5 = (real)fabs(local_3);
    if (local_5 < arg_4)
    {
        real local_6 = (real)fabs(arg_3);
        if (local_6 < 0.7853981852531433f || local_6 > 2.356194496154785f)
        {
            local_0 = (local_0 < 0.0f ? -1 : 1) * local_4;
            local_1 = (1.0f - local_5 * 1.6370222568511963f) * (local_1 < 0.0f ? -1 : 1) * local_4;
        }
        else
        {
            local_1 = (local_1 < 0.0f ? -1 : 1) * local_4;
            local_0 = (1.0f - local_5 * 1.6370222568511963f) * (local_0 < 0.0f ? -1 : 1) * local_4;
        }
    }
    else if (fabs(local_0) > fabs(local_1))
    {
        local_0 = (local_0 < 0.0f ? -1 : 1) * local_4;
        local_1 = 0.0f;
    }
    else
    {
        local_1 = (local_1 < 0.0f ? -1 : 1) * local_4;
        local_0 = 0.0f;
    }
    *arg_0 = function_217c41(local_0);
    *arg_1 = function_217c41(local_1);
}

/* standard marker before the body matches (frame alignment): retail's
   prologue self-aligns its frame (`push ebp; mov ebp, esp; and esp, -8`),
   takes no arguments and returns with a bare `ret`. Our build self-aligns
   such a function only when its address escapes (a scan of the 170
   self-aligning functions showed this). Retail holds no reference to its
   address, and its caller 0x12b450 is LTCG code. With the marker 0x12b450 and
   0x12b690 match; without it they don't. The body itself doesn't match yet. */
// score 474 (was 474): Frame 0x38 versus 0x40; angle homes and snap-work lifetimes still differ.
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
// Earlier 474 from 501: the per-controller block and the preferences are declared in retail's order,
// so the block pointer keeps the register retail loads it into. The frame 0x38 versus 0x40, the loop
// counter's home and the remaining field and register rotations remain.
// Round 338: native external input axes/channel-word/trigger views in seven subsets,
// then all with a read-only receiver, 474/474/474/474/474/474/474/474. Original input layout,
// observations, helper input copies and complete body restored; frame/angle/snap homes remain.
// @retail 0x217c40 standard
void function_217c40(void)
{
    for (long local_0 = 0; local_0 < 4; local_0++)
    {
        s_input_state *local_1 = NULL;
        if (g_4e61cc[(short)local_0])
            local_1 = g_4e61b9 ? &g_4e630c : &g_4e61dc[(short)local_0];
        if (local_1)
        {
            s_217c40 *local_21 = &((s_217c40 *)(g_51ea18 + 0x70))[local_0];
            s_gamepad_preferences *local_20 = &((s_gamepad_preferences *)g_51ea18)[local_0];
            real local_3 = (real)*(short *)((byte *)local_1 + 0x42);
            real local_2 = (real)*(short *)((byte *)local_1 + 0x40);
            double local_4 = atan2(local_3, local_2);
            real local_6 = (real)fabs(sin(local_4));
            real local_7 = (real)fabs(cos(local_4));
            real axis_scale = 1.0f / (local_6 > local_7 ? local_6 : local_7);
            real local_9 = function_217c41(local_2 * axis_scale * 0.000030518509447574615f);
            real local_10 = function_217c41(local_3 * axis_scale * 0.000030518509447574615f);
            real local_12 = (real)*(short *)((byte *)local_1 + 0x46);
            real local_11 = (real)*(short *)((byte *)local_1 + 0x44);
            double local_13 = atan2(local_12, local_11);
            real local_15 = (real)fabs(sin(local_13));
            real local_16 = (real)fabs(cos(local_13));
            axis_scale = 1.0f / (local_15 > local_16 ? local_15 : local_16);
            real local_18 = function_217c41(local_11 * axis_scale * 0.000030518509447574615f);
            real local_19 = function_217c41(local_12 * axis_scale * 0.000030518509447574615f);
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
                function_217c42(&local_9, &local_10, (real)local_4, local_4, 0.6108652353286743f);
                function_217c42(&local_18, &local_19, (real)local_13, local_13, 0.1745329201221466f);
            }
            bool local_28 = local_20->unknown1a;
            if (!local_28 && local_20->unknown1b)
                local_28 = function_218510(local_0);
            switch (local_27)
            {
            case 0:
                local_21->field_38 = local_10;
                local_21->field_3c = 0.0f - local_9;
                local_21->field_40 = 0.0f - local_18;
                local_21->field_48 = local_19;
                local_21->field_44 = (local_28 ? -1.0f : 1.0f) * local_19;
                break;
            case 1:
                local_21->field_40 = 0.0f - local_9;
                local_21->field_48 = local_10;
                local_21->field_44 = (local_28 ? -1.0f : 1.0f) * local_10;
                local_21->field_3c = 0.0f - local_18;
                local_21->field_38 = local_19;
                break;
            case 2:
                local_21->field_38 = local_10;
                local_21->field_40 = 0.0f - local_9;
                local_21->field_3c = 0.0f - local_18;
                local_21->field_48 = local_19;
                local_21->field_44 = (local_28 ? -1.0f : 1.0f) * local_19;
                break;
            case 3:
                local_21->field_3c = 0.0f - local_9;
                local_21->field_48 = local_10;
                local_21->field_44 = (local_28 ? -1.0f : 1.0f) * local_10;
                local_21->field_40 = 0.0f - local_18;
                local_21->field_38 = local_19;
                break;
            default:
                local_21->field_38 = local_10;
                local_21->field_3c = 0.0f - local_9;
                local_21->field_40 = 0.0f - local_18;
                local_21->field_48 = local_19;
                local_21->field_44 = (local_28 ? -1.0f : 1.0f) * local_19;
                break;
            }
            if (!g_51ebcc[local_0])
                g_51ebcc[local_0] = true;
        }
        else
            g_51ebcc[local_0] = false;
    }
}
