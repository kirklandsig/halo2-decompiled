#include "unknown_4e8c38.h"
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_11cc90.h"
#include "network_message_types.h"
#include <math.h>
#include <string.h>
// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;
struct s_186ab0
{
    byte field_0[0x24];
    long field_24;
    short field_28;
    byte field_2a[0x21c - 0x2a];
};
struct s_186ab1
{
    long field_0;
    dword field_4;
    short field_8;
    byte field_a[6];
    real field_10;
    real field_14;
    vector2f field_18; /* a pair of angles */
    real field_20;
    real field_24;
    short field_28;
    char field_2a;
    char field_2b;
    short field_2c;
    short field_2e;
    bool field_30;
    char field_31;
    byte field_32[0x44 - 0x32];
    byte field_44[0x24];
    bool field_68;
    byte field_69[3];
    byte field_6c[0xc];
    byte field_78[0x89 - 0x78];
    bool field_89;
    short field_8a;
    short field_8c;
    byte field_8e[3];
    bool field_91;
    bool field_92;
    byte field_93;
};
struct s_186ab2
{
    long field_0;
    long field_4;
    long field_8;
    byte field_c[0xc];
    short field_18;
    byte field_1a[2];
    real field_1c;
    real field_20;
};
struct s_186ab3
{
    vector2f field_0; /* a pair of angles */
    real field_8;
    real field_c;
    real field_10;
    real field_14;
    dword field_18;
    byte field_1c;
    byte field_1d[3];
    word field_20;
    byte field_22[6];
    s_186ab2 field_28;
};
struct s_186ab4
{
    void *field_0;
    byte *field_4;
};
struct s_186ab5
{
    byte field_0[0x14];
    long field_14;
    byte field_18[0x168 - 0x18];
    real field_168;
    real field_16c;
    real field_170;
    byte field_174[0x212 - 0x174];
    char field_212;
    char field_213;
    short field_214;
    byte field_216[0x23d - 0x216];
    char field_23d;
    char field_23e[2];
    byte field_240[0x348 - 0x240];
    word field_348;
};
struct s_186ab6
{
    word field_0;
    byte field_2;
    byte field_3;
    long field_4;
    s_186ab5 *field_8;
};

#pragma pack(push, 1)
struct s_186ab7
{
    dword field_0;
    real field_4;
    real field_8;
    real field_c;
    real field_10;
    real field_14;
    real field_18;
    word field_1c;
    long field_1e;
    word field_22;
    word field_24;
    word field_26;
    byte field_28[0xc];
    byte field_34[0x24];
    byte field_58[4];
};

#pragma pack(pop)
struct s_186ab8
{
    dword field_0;
    dword field_4;
    dword field_8;
};
__forceinline void function_186ab8(s_186ab8 *arg_0, s_186ab8 const *arg_1)
{
    *arg_0 = *arg_1;
}
void __stdcall function_185be0(long arg_0, long arg_1, real arg_2, real arg_3, s_186ab4 *arg_4, s_186ab3 *arg_5);
void __stdcall function_151110(long arg_0, void *arg_1);
bool __stdcall function_148fff(long arg_0);
bool function_12be90(void);
short __stdcall function_cdeb0(long arg_0, long arg_1, short arg_2, short arg_3);
short function_cfe40(long arg_0);
short function_cdff0(long arg_0, short arg_1);
bool function_c85c0(long arg_0);
short function_c8960(long arg_0, short arg_1);
long function_155760(long arg_0);
void __stdcall function_187510(long arg_0, real arg_1, real arg_2);
void player_action_initialize(s_player_action *arg_0);

#pragma inline_depth(0)
// MiniMax R (11-01): no new form, no lower score (the score-1 form needs a new inline_depth region
// and a one-caller forceinline wrapper of 0x14d040; the retail callees stay out of line at inline_depth(0)).
// score 134 (was 134): frame 0x54 and selection inputs agree; comparison order,
// byte-count extension, cyclic operand order, cold clear and output/angle scheduling differ.
// MiniMax I run 21 (not kept, m3 review): 113 with 0xcdff0 called at the count test, 0xcbec0(unit, type, 1)
// for the cycle loop and the field-first selection comparison, both calls inlined only with this function at
// inline_depth(255) and player_action_initialize behind a forceinline wrapper at inline_depth(0) (as 0x825e0);
// that wrapper is kept only with a byte match. The same calls with the pragma as here stay out of line: 457.
// Round 30 (fast; "aligned" = differing lines after a sequence alignment, base 57): retail's
// first grenade-count test is the existing function_cdff0 inlined (the NONE check, then a short
// count: `movsx ax`, `test ax, ax`), but calling it there leaves both calls out of line 403;
// float stores for field_20/24 164 (aligned 71); an object pointer local for the angle block
// 184 (aligned 81: retail's esi code there, but the tail's scratch registers rotate); both 178
// (aligned 30); both with the nested 0x40 latch and the 0x60/0x4630200 masks tested apart 166
// (aligned 28, 1499 bytes). Left in that form: the latch's `field_92 = false` arm placed inline
// (retail after the return), the field_8/field_4 copy order, the x87 inclination magnitude (retail
// squares y in place, `fld st(2); fmulp st(3)`; compile-only screens of four spellings give ours),
// the comparison order, the byte-count extension and one SIB order.
// Earlier input-convention gaps are absent in this baseline; both selection helpers and the count query match.
// Positive nested latch with toggle-first/state-first 159/157; state-first with split invalidation 161;
// field-first selection comparison 133 (one-point gain dropped); zero comparisons 134 each;
// reversed cyclic subscript 134; request store before record copy 136. Original body restored.
// Earlier 279 (was 323): frame 0x54 agrees; selection widths/registers at calls differ;
// named root / first atan2 / second atan2 / first fmod / second fmod results 161 each;
// both atan2 / both fmod / all results 161 each (frame 0x5c vs 0x54).
// Native request bit/all request fields 323/323; native clear fields 387; direct held-state output 323, with request bit 323; native reset bit 323; request/clear fields 387; all fields/state output 381.
// Cyclic result after start 330; candidate before result 331; explicit cyclic step 326;
// start from field selection 324; traversal storage identities 323; final memcpy 323;
// native context initializer 323; result lifetime with explicit step 332. All eight forms restored.
// Unsigned word/dword request 323/331; signed-byte/word/dword reset 323 each;
// late object pointer fields 306 plain kept (1500/1515); mode-state pointer 323;
// unsigned word request plus late object fields 306. Keep only late field observations.
// First/second/both scalar before pair 306/306/306; library angle/scalar/whole-request copies 306/279/321;
// native output pair/with scalar phase 306/306. Scalar-pair library copy 279 plain kept (1489/1515).
// Packed source/destination/both selection words 279/279/279; selection memcpy 279;
// native destination plus final word copy 279; both native words plus final copy 279;
// native three-word source/output fields 279; native fields with packed selection words 279.
// Short saturation counter 311; two-use held selection 279; both 311; sentinel first 283;
// sentinel with short/held/both 315/283/315; int counter with held/sentinel 283.
// Output angle/vector/scalar pair library copies, each pairwise combination
// and all three pairs 279 each; angle pair plus complete four-float control copy 314.
// Complete request flags once before admission at byte/word/short/dword width 279/282/282/267;
// after admission 279/281/281/266. Dword after admission 266 plain; frame 0x54 agrees.
// Shared request with bool/byte/word permission 379/379/266; definite short/bool arms 266/379;
// declaration fallback with short/bool 255/379; combined permission 376; snapshot after object lookup 258;
// after selection stores with short/bool/byte 252/231/231; late snapshot plus bool/short fallback 366/369.
// Late coherent request and Boolean permission 231 plain; selection/count and cold output scheduling remain.
// Round 185 late complete flags byte/word/short/signed byte with bool permission 158/319/319/158;
// with short permission 279/246/246/288 (fast). Native unsigned byte flags with bool permission
// kept at 158 plain; every value, observation boundary and call tuple unchanged.
// Round 189 pulse-latch combined/release-first guards with union/two masks 181/177/165/169;
// test-derived latch publication 135, with two masks 175; two-use pulse plus publication
// with union/two masks 172/175. Keep direct latch publication; protected input, count,
// cold clear and output/angle scheduling remain.
// Direct selection fallback 327; cyclic result/start/both at long width 289/327/326; high-first
// saturation 316; logical availability 279; fallback/result 323; cyclic/saturation/availability 331.
// Native orientation vector 135; coherent unit source 185; source/vector 185; existing state query 304;
// query/source 310, query/vector 304, all three 310; complete native orientation value 220.
// Original fields, pointer observations and query guard restored.
// Round 56: object pointer local in the angle block 185 (retail's esi there; the tail's scratch
// registers rotate); nested 0x40 latch with the 0x60 and 0x4630200 masks tested apart 164, its
// latch store first 162, else-first latch 169, with float stores for field_20/24 158 at retail's
// 1515 bytes. The nested latch and float stores are retail's code (aligned 58 -> 54), but retail
// puts the `field_92 = false` arm after the return; bool/byte/held locals, store orders and
// a ternary toggle all leave that arm inline, so the lockstep count shifts.
// Complete toggle result in bool/byte, union invalidation, early publication 181/184;
// split masks 182/183; publication after action update 143/184, split masks 185/184.
// Round 342: Complete inclination squared-product pairs, reversed pairs and named sum 148 each; squared accumulator 150;
// with symmetric named angle shifts 148/148/148/150. All enlarge the frame to 0x5c; original expressions restored.
// Round 194: shared initial/count/both object producers 388/212/406; cyclic producer 385;
// initial/cyclic, count/cyclic and all object producers 416/395/426; all with late header
// accessor 438. Original observations and numerical/output expressions restored.
// Round 413: state object/input/view identity observed once 283/431/372; both associations 385;
// object with input/view/both 419/451/423; all with field-first selection comparison 423.
// Input/view captures disturb the 0x54 frame; original observations and comparison restored.
// Round 288: coherent count-pointer/native-array and mutable/readonly unit observations across
// both count consumers, each with original/field-first selection comparison 389, 389, 389, 389, 384, 384, 384, 384. Restored.
// Round 194: Coherent request byte at use 134; native request bits alone/with bounded short counter 172/165; native saved output bit alone/with per-use byte/native request bits/native bits plus counter/original snapshot plus counter 406/406/407/403/401. All restored; comparison/count and cold output/angle scheduling remain.
// Round 27 (lane 11): retail inlines existing helpers here, which the inline_depth(0) pragma blocks. With
// the pragma only around a forceinline wrapper of 0x14d040 (as 0x84750/0x825e0/0x874c0 keep it out of
// line), 0xcdff0 called at the first count test (LTCG inlines that site only, as retail), 0x13cb40 for
// the g_510c50 test, 0xcbec0(unit, type, 1) for the cycle loop and 0x11df30 for the yaw/pitch, plus the
// 0x40 latch through a toggle flag set in the test (retail's arm after the return), the field-first
// selection compare, float stores for field_20/24, an object pointer for the angle block and the copies
// in the order block, field_4, field_8: 1 (plain), only the inlined 0xcbec0 count read [edx + edi]
// where retail has [edi + edx]. Not kept: the flag form needs a match, and without it the arm stays
// inline (nested latch 140, original latch 152). Steps: short count ternary 114; flag latch and compare
// 113; float stores and object pointer 84; y squared in its own statement gives retail's x87 pitch only
// through the float overloads; pragma removed 569 (0x14d040 inlined); copy orders 9/11/11/5, struct
// block copy 8; SIB call-site forms (result local, unit index local) 1; permuter 600 s, 21 tries, none
// below 1. Non-flag latch spellings (else first, named pressed bool, store first) all keep the arm inline.
// Round 53 (lane 9, fast with every callee listed; the reduced image then scores it as the full build):
// lane 11's round-27 form reproduced at 1 (1515 bytes); still only the inlined 0xcbec0 count read [edx + edi]
// against retail's [edi + edx]. On it: the +0x28/+0x214 four-byte copy as a real short/char/char struct
// member instead of the long cast, with the output copy as a long cast/a struct copy 1/1 (identical code);
// the cycle loop written out with the unit looked up inside it 316. Diagnostics (all reverted): new-field
// stores to the control record, the player record or the unit, dropped or duplicated selection stores and dead
// temporaries before the call or at entry never flip the order. Compile-only probe with an inlinable copy of
// 0xcbec0's body reproduces [edx + edi]; only a unit read inside the loop gives base first, but then the
// lookup is scheduled after the NONE normalization (retail has it first), and that body out of line no
// longer gives retail's 0xcbec0. The score-1 source is saved as build/r53/186ab0.s1.cpp in the m3-lane9 tree.
// Round 243: Native persistent toggle/pulse/hold byte fields, subsets 1..7
// 134/134/134/134/134/134/134; all signed byte 134. Original Boolean fields,
// latch graph and complete body restored; comparison/count and cold output scheduling remain.
// Round 116 (lane 6, fast): the round 27 lane 11 form still scores 1 on this tree (only the inlined 0xcbec0 count
// read `[edx + edi + 0x23e]` against retail's `[edi + edx + 0x23e]`, same registers). Retail calls 0xcbec0 out of
// line only from 0xc5ae3, and that copy reads `[edx + edi]` (type as base) like ours, so the order comes from the
// inlined instance. On that form: the first count test written out instead of the 0xcdff0 call 29; one unit-index
// local for the whole selection block 355; a unit-index local for the second count test and the cycle call 1.
// Compile-only probes of the inlined cycle (result/unit-index/grenade locals, constant or variable direction,
// nested inlining, a caller-side unit lookup just before the call, callee local renames) all give type as base;
// only a lookup inside the loop gives unit as base, and it also flips the out-of-line copy and moves the lookup
// below the NONE test (the LTCG build then shifts everywhere, 316). BLOCKED-BY 0xcbec0 (contributor) as far as
// found: the inlined order needs a cause outside this body.
// Round 42 (lane 14, fast): lane 9's round-53 score-1 source rebuilt on today's file still scores 1 (only
// the inlined 0xcbec0 count read [edx + edi] against [edi + edx]); with `#pragma inline_depth(8)` after
// the player_action_initialize wrapper (as 0x84750/0x825e0/0x874c0 use) in place of 255, still 1. On it,
// latch spellings without the toggle flag (the release arm stays inline, retail's is after the return):
// `if (pressed && !held) { held = true; toggle; } else if (!pressed) held = false;` 146 (the masked
// byte is kept and stored as the release value); the same with the toggle before the held store 145;
// as two ifs (the release test after the toggle block) 147; with a named pressed bool 140; the nested
// latch with the release store only when held 136. BLOCKED-BY 0xcbec0 (a contributor's function inlined
// here), and the toggle flag set inside the test needs a byte match to be kept.
// @retail 0x186ab0
void __stdcall function_186ab0(long arg_0, real arg_1, real arg_2, s_player_action *arg_3)
{
    s_186ab0 *local_0 = (s_186ab0 *)g_4e8c24->data + (arg_0 & 0xffff);
    s_186ab1 *local_1 = (s_186ab1 *)((byte *)g_4ed284 + 0x14) + local_0->field_28;
    s_186ab4 local_2;
    s_186ab3 local_3;
    byte *local_5 = (byte *)g_4e034c;
    local_2.field_4 = *(byte **)(local_5 + 0xf4);
    local_2.field_0 = NULL;
    function_185be0(local_0->field_24, local_0->field_28, arg_1, arg_2, &local_2, &local_3);
    byte reset = function_148fff(local_0->field_24) | function_12be90() | ((byte *)g_4e8c44)[local_0->field_28 * 0x140 + 0x56];
    if (reset)
    {
        bool local_4 = (local_3.field_20 >> 8) & 1;
        memset(&local_3, 0, sizeof(local_3));
        local_3.field_28.field_1c = 0.0f;
        local_3.field_28.field_20 = 0.0f;
        local_3.field_28.field_0 = NONE;
        local_3.field_28.field_4 = NONE;
        local_3.field_28.field_8 = NONE;
        local_3.field_28.field_18 = 0;
        if (local_4)
            local_3.field_20 |= 0x100;
        else
            local_3.field_20 &= ~0x100;
    }
    if (local_1->field_0 != NONE)
    {
        s_186ab5 *local_4 = ((s_186ab6 *)g_4e0300->data)[local_1->field_0 & 0xffff].field_8;
        if (local_1->field_28 != local_4->field_214)
            *(long *)&local_1->field_28 = *(long *)&local_4->field_214;
        byte request_flags = local_3.field_1c;
        bool local_5 = (request_flags & 1) != 0;
        char local_6 = local_1->field_2a;
        if (local_4->field_213 != NONE)
            local_5 = false;
        if (local_6 == NONE)
            local_6 = local_4->field_212;
        if (local_5 || local_6 == NONE)
            local_1->field_2a = (char)function_cdeb0(local_1->field_0, NONE, local_6, local_5);
        short local_7 = function_cfe40(local_1->field_0);
        if (local_7 != NONE && local_7 != local_4->field_212)
        {
            local_1->field_2a = (char)local_7;
            local_1->field_2b = NONE;
        }
        if (local_1->field_2a == local_1->field_2b && local_1->field_2a != NONE)
            local_1->field_2b = NONE;
        if (local_1->field_2c == NONE ||
            !(short)((s_186ab6 *)g_4e0300->data)[local_1->field_0 & 0xffff].field_8->field_23e[local_1->field_2c])
            local_1->field_2c = local_4->field_23d;
        if ((request_flags & 2) || local_1->field_2c == NONE || !function_cdff0(local_1->field_0, local_1->field_2c))
        {
            s_186ab5 *local_11 = ((s_186ab6 *)g_4e0300->data)[local_1->field_0 & 0xffff].field_8;
            short local_8 = local_1->field_2c;
            short local_9 = NONE;
            if (local_8 == NONE)
                local_8 = 0;
            short local_10 = local_8;
            do
            {
                if (local_11->field_23e[local_10] > 0)
                {
                    local_9 = local_10;
                    if (local_10 != local_8)
                        break;
                }
                local_10 = local_10 == 1 ? 0 : local_10 + 1;
            } while (local_10 != local_8);
            local_1->field_2c = local_9;
        }
        if ((!g_510c50 || !((byte *)g_510c50)[5]) && !(g_4ed284->flags10 & 1) &&
            (!g_510c54->active || !g_510c54->unknown01) && !function_155760(local_0->field_28) &&
            local_1->field_0 != NONE && function_c85c0(local_1->field_0))
        {
            if (request_flags & 4)
            {
                local_1->field_8a = local_1->field_2e;
                local_1->field_2e = function_c8960(local_1->field_0, local_1->field_2e);
                if (local_1->field_2e != NONE)
                {
                    local_1->field_89 = true;
                    local_1->field_8c = 0;
                }
            }
            else if (request_flags & 8)
            {
                if (local_1->field_89 && local_1->field_8c < 0x7f)
                    local_1->field_8c++;
            }
            else
            {
                if (local_1->field_89 && local_1->field_8c > 0xf)
                    local_1->field_2e = local_1->field_8a;
                local_1->field_89 = false;
            }
        }
        else
        {
            local_1->field_2e = NONE;
            local_1->field_89 = false;
        }
        if (!((byte *)g_4e8c44)[local_0->field_28 * 0x140 + 0x55])
            function_187510(local_0->field_28, local_3.field_10, local_3.field_14);
        if (local_4->field_14 == NONE)
        {
            bool local_14 = false;
            if ((bool)((*(dword *)&g_54e8e0[local_0->field_24] >> 4) & 1))
                local_14 = (bool)((*(dword *)((byte *)&g_54e8e0[local_0->field_24] + 0x114) >> 3) & 1);
            if (local_14 &&
                (real)fabs((double)local_1->field_18.i) > 0.5f && local_3.field_14 < 0.0001f && !local_1->field_68)
            {
                long local_9 = local_1->field_31 + 1;
                if (local_9 < 0) local_9 = 0;
                else if (local_9 > 0x7f) local_9 = 0x7f;
                local_1->field_31 = (char)local_9;
                local_1->field_30 = (short)(char)local_9 > *(short *)(local_2.field_4 + 0x6e);
            }
            else
            {
                local_1->field_31 = 0;
                local_1->field_30 = false;
            }
        }
        else
            local_1->field_30 = false;
        if ((byte *)g_4e6948 + 8 && ((byte *)g_4e6948)[0xd] != 3)
        {
            if ((request_flags & 0x40) && !local_1->field_92)
                local_1->field_91 = !local_1->field_91;
            local_1->field_92 = (request_flags & 0x40) != 0;
            if (local_3.field_18 & 0x4630260)
                local_1->field_91 = false;
            if (local_1->field_91 && local_4->field_14 == NONE)
                local_3.field_18 |= 0x4000;
        }
    }
    function_151110(arg_0, local_1->field_6c);
    local_1->field_8 = local_3.field_20;
    memcpy(local_1->field_44, &local_3.field_28, sizeof(local_1->field_44));
    local_1->field_4 = local_3.field_18;
    local_1->field_18 = local_3.field_0;
    memcpy(&local_1->field_20, &local_3.field_8, 2 * sizeof(real));
    if (local_1->field_0 != NONE)
    {
        s_186ab6 *local_4 = (s_186ab6 *)g_4e0300->data + (local_1->field_0 & 0xffff);
        if ((1 << local_4->field_3) & 1)
        {
            if ((bool)((local_4->field_8->field_348 >> 5) & 1))
            {
                local_1->field_10 = (real)atan2((double)local_4->field_8->field_16c, (double)local_4->field_8->field_168);
                local_1->field_14 = (real)atan2((double)local_4->field_8->field_170,
                    sqrt((double)local_4->field_8->field_16c * local_4->field_8->field_16c + (double)local_4->field_8->field_168 * local_4->field_8->field_168));
                local_1->field_10 = (real)fmod((double)(local_1->field_10 + 6.2831854820251465f), 6.2831854820251465);
                local_1->field_14 = (real)fmod((double)(local_1->field_14 + 9.424777984619141f), 6.2831854820251465) - 3.1415927410125732f;
                local_4->field_8->field_348 &= ~0x20;
            }
        }
    }
    player_action_initialize(arg_3);
    s_186ab7 *local_4 = (s_186ab7 *)arg_3;
    local_4->field_4 = local_1->field_10;
    local_4->field_8 = local_1->field_14;
    local_4->field_1c = local_1->field_8;
    local_4->field_0 = local_1->field_4;
    local_4->field_1e = *(long *)&local_1->field_28;
    local_4->field_22 = local_1->field_2c;
    local_4->field_24 = local_1->field_2e;
    local_4->field_c = local_1->field_18.i;
    local_4->field_10 = local_1->field_18.j;
    local_4->field_14 = local_1->field_20;
    local_4->field_18 = local_1->field_24;
    s_186ab8 const *local_11 = (s_186ab8 const *)local_1->field_6c;
    s_186ab8 *local_12 = (s_186ab8 *)local_4->field_28;
    *local_12 = *local_11;
    memcpy(local_4->field_34, local_1->field_44, sizeof(local_4->field_34));
}
#pragma inline_depth(255)
