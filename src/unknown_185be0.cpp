#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "unknown_11cb00.h"
#include "unknown_11cc90.h"
#include <math.h>
#include <string.h>
// @flags /O2 /Ob1 /arch:SSE /Gr

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
    real field_0;
    real field_4;
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
struct s_185be0
{
    byte field_0[0x10];
    word field_10[0x10];
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
struct s_input_state
{
    byte unknown00[0x10];
    byte values[0x38];
};
struct s_185be6
{
    long field_0;
    byte field_4[8];
    word field_c;
    word field_e;
    byte field_10[0x2e - 0x10];
    short field_2e;
    byte field_30[4];
    vector3f field_34;
    long field_40;
    byte field_44[0x68 - 0x44];
    bool field_68;
    byte field_69[0x78 - 0x69];
    real field_78;
    real field_7c;
    byte field_80[8];
    byte field_88;
    byte field_89[5];
    byte field_8e;
    byte field_8f;
    byte field_90;
    byte field_91[2];
    byte field_93;
};
struct s_small_index;
struct s_havok_component;
struct s_unknown_13bf00;
struct s_player_action;
struct s_player_action_triggers;
extern byte g_4e61b9;
extern s_input_state g_4e61dc[3];
extern s_input_state g_4e630c;
extern s_unknown_13bf00 *g_510c50;
extern byte g_51ea18[];
real magnitude3d(vector3f const *arg_0);
real function_185b30(short arg_0, real const *arg_1, real arg_2);
real function_c8880(long arg_0, short arg_1);
short function_0b67a0(s_small_index const *arg_0);
bool function_e4050(long arg_0);
bool function_100f70(long arg_0);
vector2f *function_11df30(vector2f *arg_0, vector3f const *arg_1);
void __stdcall function_1a4900(long arg_0, s_186ab2 *arg_1, vector3f *arg_2);
void player_control_update_action_flags(s_player_action *arg_0, s_player_action_triggers const *arg_1);

__forceinline real function_185be1(real arg_0, real arg_1, real arg_2)
{
    if (arg_0 < arg_1) arg_0 = arg_1;
    else if (arg_0 > arg_2) arg_0 = arg_2;
    return arg_0;
}
__forceinline byte *function_185be2(long arg_0)
{
    return *(byte **)(g_4e0300->data + (arg_0 & 0xffff) * 12 + 8);
}
__forceinline bool function_185be3(long arg_0)
{
    return arg_0 != NONE && ((1 << g_4e0300->data[(arg_0 & 0xffff) * 12 + 3]) & 1);
}
__forceinline void function_185be4(dword *flags, dword mask, bool enabled)
{
    if (enabled)
        *flags |= mask;
    else
        *flags &= ~mask;
}
__forceinline void function_185be5(s_186ab2 *arg_0)
{
    arg_0->field_1c = 0.0f;
    arg_0->field_20 = 0.0f;
    arg_0->field_0 = NONE;
    arg_0->field_4 = NONE;
    arg_0->field_8 = NONE;
    arg_0->field_18 = 0;
}

union s_185be7
{
    real field_0[2];
    vector2f field_8;
};

#pragma inline_depth(1)
// score 731 (was 731): Frame 0xe0 and state/output carriers agree; control-root EBX versus NONE, settings/zero homes and flag joins remain.
// Earlier 780 (was 780): Frame 0xe0 and state/output carriers agree; the flag bits are now set with retail's branches. Left: settings/zero homes, clamp-arm placement and unthreaded repeated flag tests.
// Round 86 (fast, aligned counts in brackets, 383 at the start): turn rates assigned in both arms (retail loads
// the stored rates on the else path) 788 [367] and the attachment index NONE per path (retail sets it on each
// failure path) 780 [356], both kept; on 780: selected input as long (retail movzx/test edx/setg) 779 [348];
// both selector bytes in byte locals identical; SET_FLAG(flags, bit, value) as 0x187b30 uses, in place of the
// mask helper 922 at 3740 bytes [310: the repeated tests thread as retail, flag section now retail's length,
// homes shifted 4]; PIN on the two settings clamps 934/951 (retail's field re-reads, lockstep worse); named
// clamped rates 789 [401]; magnitude sum terms swapped identical.
// Frame 0xe0 and state/output carriers agree; the flag bits are now set with retail's branches. Left: settings/zero homes, clamp-arm placement and unthreaded repeated flag tests.
// Retail gives EBX to NONE for its seven early uses (the input test, the three clears, the two index tests and
// the short `cmp dx, bx`) because it re-reads the control global at the flag test; ours keeps the global
// there from entry. Round 30 (fast): the flag read through a file-local forceinline accessor 790 (still
// held); powf 790; settings pointer read after the clears 799; the global named in a local before its test
// 790; magnitude3d (0x24590) defined inline in its file 821 (still held; 0x24590 matches either way).
// Round 9 (lane 10): retail clamps the range fraction in place (division result and clamp share its slot)
// and threads every repeated flag test (one test, both stores per arm), testing the selected input as a
// dword. Fraction computed then clamped into itself 790 (same code); fraction declared before its bounds
// 793; descriptive fraction/minimum/range names 790 (slots unchanged); selected input as long 790 (dword
// test, still unthreaded); flag helper as a conditional expression 789. Retail reloads the control
// global after the magnitude call (0x24590, another lane's file) as if that call could write memory.
// Retail re-reads the player-control global before its flag test, after the conditional magnitude/pow
// calls, and keeps NONE in EBX; ours keeps the global in EBX from entry (LTCG trusts those callees here;
// retail's address is never taken). Round 73: settings pointer after the clears 959; state entry after the
// clears 957; state entry first 941 at 3779 bytes (entry loads out of retail's order); clears swapped 949;
// if/else output pointer 986.
// Earlier: frame 0xe4 versus 0xe0; state/output carriers and mask/flag schedules still differ.
// Previous 1082 to 1023; counted running mask 1059; copy running mask 1130; matrix/vector scopes 1082; native pairs 1082; indexed mask 1068.
// Selected-mask groups 1180; action-mask group 1179; paired-input mask 1140.
// Input memcpy/reset inline 1059; selected byte 1063; separate masks 1059; counted word mask 1032;
// word mask plus selected byte 1023, plus word index 1080.
// Whole-output reset 1082; output clear first 1056; inactive input clear 1047;
// common flag store 1042; action byte 1044; chained index reset 1023;
// common flag store with indexed mask 1013.
// Shared curve inputs 1043; late input buffers 1074; active-branch pointers 1075;
// native clear mask 1023; retained response predicate 1023.
// Round 357: Settings read at each use 1021 (frame 0xe0); four byte predicates 1017;
// output copy cursors 1016; stepped copy bit 1017; settings/cursor combination 1019.
// Round 126: Paired output branch 994; complete flag result 1002; false-first flags 1016; ordinary clamp
// inline 1017; no outer zero-mask guard 1018; second-copy continue/final input ternary/const settings 1017
// each; paired branch with complete flags 989 (plain kept); paired branch with external state reference
// 994; combined with promoted component 1010, word copy ordinal 989, input-first mask 1016 (fast).
// Native flag/scalar/state-bool declarations 989/989/985; flag+scalar 989;
// flag+bool/scalar+bool/all fields 985; all fields with promoted selected value 984.
// First-mask byte/word index with long/byte/word copy indices: all six 1174;
// shared byte/word ordinals 1174/1174. Native first indices lose useful expansion;
// original long ordinals and body restored.
// Native value-result flag helper in all six formal orders 979 each; complete
// branch-result and negative enabled input 979/979. Six-point reduction dropped;
// 3760/3777 bytes. Original pointer-input helper and every call restored.
// State root at every field use 987; with reset output result/flag value/both 987/988/999;
// state and settings roots at each use 1048; with reset result/flag value/both 1048/1026/1014.
// Original root snapshots, reset, pointer-input flag helper and body restored.
// Round 166: Native actual settings record with cached/live roots, mutable/const
// receivers and forced/ordinary existing object getter 991/1022/991/1022/991/1022/991/1022
// fast. Original settings observations, getter declaration and complete body restored.
// Flag-value producer with shared curve count/pointer/both/values-first 979 each; long count 978;
// native pairs with short/long inputs 979/978; shared short inputs and complete clamp result 969.
// Complete clamp alone 978; flag-value plus complete clamp 969 plain (kept); curve inputs plus clamp 978.
// Shared curve locals and native-pair trials restored; original game-call expressions unchanged.
// Round 337: signed controller slot short/long 969/997; snapshot before turns 969;
// existing turn/response/both pair aggregate initialization 969 each;
// long slot with early snapshot/with both pair initializers 997/997. All restored.
// Clear-and-select flag result 947 kept, native long predicate 944 (extra three points dropped);
// clear-first/set-first results with bool 1002/982, with long 1005/985;
// named selected-mask result bool/long 947/944. Original Boolean helper input retained.
// Round 97: the global defined in a non-LTCG file 947 (still held in EBX); clamp helper with early
// returns/result variable/ternary 952/973/952; the flag test through a const pointer to the global
// 884 (reloads it as retail, NONE then misses EBX) but its taken address moves 0x185ab0's store and
// loses 0x187b30, so retail did not treat the global as address-taken. Reverted.
// Round 221: recovered-frame settings/scalar/workspace names and active declarations,
// eight forms 947 each; cached/state-live/settings-live/both roots with native names
// 947/985/1004/1036/947/985/1004/1036. Original observations and body restored.
// Consistent parent-control pointer snapshots with original/native entry addresses,
// mutable/const pointers and either pre-clear acquisition order 947 each (eight forms).
// Original root observations, entry calculation, helpers and complete body restored.
// Ordinary inline clamp/flag/both 947/947/947; state predicate alone and all
// combinations plus all-private inline producers 968 each. Original force-inline helpers and body restored.
// DH round 189: Range-result default before lower/range/tag/parent reads 950 each, before initial object 966;
// conditional/exhaustive positive-first/negative-first results 875/875/963. Exhaustive positive-first
// retained after ordinary full validation; frame/root/settings and later flag schedules remain.
// Round 7: pointer flag helper with if/else 890 (frame 0xe4); with its inputs tested directly instead
// of through bool locals 817 (kept); long input 820, with raw values 820; ternary store 1023; clamp as a
// re-evaluating macro 1014; range result defaulted before its test 790 (kept); declared first 793,
// between bound and range 793; clamp helper as a ternary 970; byte input with raw values 790; action
// input in a byte local 790. Retail tests a repeated condition once for consecutive flags; ours
// re-tests it, and retail places the clamp's upper arms after the hot path.
// Round 66 (fast): input record copied before the two turn rates (stores after the copy as retail) 789;
// a re-evaluating PIN macro on the two settings-field clamps (retail re-reads those fields in each arm)
// 934, on every clamp but the magnitude one 971; the selected input byte as a long (retail's movzx edx /
// test edx / setg / jle) 790, aligned 289 -> 281, the repeated test still not threaded; long input plus
// early copy 789. Retail also reads the two selector bytes as one dword (EBX, then BL/BH).
// Round 209: Caller-local inline declarations for magnitude/curve/response, all seven nonempty subsets, and
// all three with separate noinline declarations: 780 each. All restored; frame/state/output agree, but
// control-root reload, NONE/settings/zero homes and repeated flag tests remain.
// Round 241: Shared clamp formal orders (0,2,1)/(1,0,2)/(1,2,0)/(2,0,1)/(2,1,0): 780/780/780/780/780;
// independent result with orders (0,2,1)/(1,0,2)/(2,0,1): 926/926/926. Original helper, calls and body
// restored.
// Round 243: Native latch fields 88/8e/8f/90/93, paired 8e+8f/88+90 and all five:
// 780/780/780/780/777/780/780/777. Three-point nonmatch gains rejected; original byte fields restored.
// Round 164: shared retained-response weight 926; paired bounded contributions staged
// before blending 932; both 932; paired interpolated rates 780; rates with weight/
// contributions/both 926/932/934; existing native forward-vector setter 920. All restored.
// Round 235: Native output flag dword/analog field/trailing bytes, paired and complete layouts
// 780/780/778/780/778/778/778; complete trailing dword 812. Two-point leads rejected; original output layout
// and body restored.
// Round 379: Packed word/dword selectors 820/778; word with selected-input promotion/conditional flags/both 821/820/821; dword with promotion/conditional flags/both 777/778/777. Gains at most three rejected; original layout, helper and body restored.
// DH round 215: Pointer-output flag-helper formal orders (0,2,1)/(1,0,2)/(1,2,0)/
// (2,0,1)/(2,1,0) 780 each; canonical byte/long predicate inputs 780/778; actual
// destination reference 780. Two-point lead rejected; original contracts and body restored.
// DH round 218: whole input snapshot aggregate with native/byte/word/both scratch initialization
// 1046/1077/1046/1072; same four with readonly settings 1046/1077/1046/1072.
// All grow to 3817 or 3837 bytes versus retail 3777; complete original body restored.
// Round 184: Separate current/previous angle outputs and carried direction, masks 1-7, then all
// three with entry output declarations: [780, 780, 780, 969, 975, 969, 975, 975]. Angle splits tie;
// direction copies grow the frame and regress. Original unions, vector reuse, helpers, values and
// complete body restored.
// Round 265: adjacent selected-input/input-five/input-four shared-mask groups, all
// seven subsets and complete grouping with full-width selection 813/895/1033/954/990/990/1031/1030.
// Original individual masks, selected byte and complete body restored.
// Independent lower/upper bounds, parameter/result 780/928; independent upper/lower
// 943/929; upper-first exclusive 943/949; ordinary lower/upper parameter/result 780/928.
// Original clamp, complete body and all caller expressions restored.
// Round 186: Native input memcpy with byte/word scratch aggregate masks 0-3 780/979/942/979; memmove
// with the same masks 1109/1104/1111/1102. Complete original body and helpers restored.
// Round 188: existing reset whole-output void/result/subobject-reference/whole-output-reference
// contracts, forced then ordinary inline: 961/961/780/961/961/961/780/961.
// Complete output contracts grow the frame; original reset and whole body restored.
// Round 267: reset groups float/word/index 781, index/float/word 782,
// index/word/float 783, word/float/index 783, word/index/float 783;
// split admission 780; inactive update/return 1026; chained indices first 782.
// Original six-store producer and complete body restored.
// Round 417: native output one-bit groups at 18/1c/20, subsets 1-7:
// 1008/958/993/803/995/825/938; all Boolean bitfield domains 1066.
// Output size/offsets agree; every native view and store form restored.
// Round 220: Parent-before-attachment acquisition, nested/direct-parent/direct-slot/direct-both 767 each;
// the same four with promoted component result 767 each. Keep nested native-short acquisition, plain 767;
// all other observations, widths and helpers restored. Control-root/NONE, settings/zero and flag joins remain.
// Round 222: native control bit with arithmetic/real-array entry; live/cached/constant roots 767
// each, complete per-field roots 1006 each. Original body and shared type restored; control-
// root/NONE and settings/zero homes remain.
// Round 231: all four rate bounds once 963; exponent once 731 (kept, plain); both 962;
// shared two-row retained weight 913; bounds/weight 966; exponent/weight 811; all three
// 963, with read-only rate receiver 963. Keep only the exponent shared by its test and
// power call; control-root/NONE, settings/zero homes and flag joins remain.
// Independent final input-five/four/seven Boolean ownership, subsets 1-7: 728/754/776/750/777/757/786; all with selected-input Boolean 784.
// The three-point input-five lead is below the retention gate; every original predicate observation, individual helper call, root and full body restored.
// Round 452: shared object/header getters and readonly settings, subsets 1-7,
// 731 each; complete shared graph with ordinary lookup/predicate inline 810.
// The control-root/NONE and range-result homes remain; original full source restored.
// Round 461: raw third-input byte/word/long/dword across duration and latch tests
// 734/730/733/733; complementary inactive bool/byte/short/long 731/731/728/730.
// Gains at most three rejected; original Boolean carrier and complete body restored.
// Round 265: library range / paired axis-duration / paired response-weight-angle
// selection, their pairs and all: 951/1095/1037/1123/1181/1293/1311; complete
// upper-first nesting 1320. Native frame, original custom clamps and full body restored.
// Round 484: shared-exponent range-result positive/conditional/unordered ownership
// 935/935/800; default before lower bound/selected record 735/735; the three
// branch-owned forms with exponent declaration at range scope 935/935/800.
// Original fraction producer, exponent observation and complete body restored.
// Round 270: Unsigned controlled/parent/attached datum carriers, seven subsets 731 each;
// all four datum carriers 731. Minimum/range/fraction scalar identities on the shared-exponent
// graph, seven subsets 731 each; exponent identity too 731. Original carriers, names and full
// body restored; control-root/NONE, settings/zero homes and flag joins remain.
// Round 487: plain mapping loops for independent bindings / action masks / both / movement masks /
// bindings+movement / action+movement / all 725/725/725/731/725/725/725; all with native output
// initialization 1027. Bindings or action masks with the two-use short inactive predicate 723/723;
// the action-mask loop with the inactive predicate scored 723 at 3770/3777 bytes. Not kept (m3 review):
// retail has no mask table or loop (one hoisted test of the flag, then three immediate ors or three
// ands), and the short inverted predicate alone is a 2-point form. Entry root/NONE and scalar homes remain.
// Round 270: Unsigned controlled/parent/attached datum carriers, seven subsets 731 each;
// all four datum carriers 731. Minimum/range/fraction scalar identities on the shared-exponent
// graph, seven subsets 731 each; exponent identity too 731. Original carriers, names and full
// body restored; control-root/NONE, settings/zero homes and flag joins remain.
// Round 245: existing current/previous angle return pointers, separately/together, 805/837/830;
// selected input source retained alone/with current/previous/both angle returns 961/995/959/980;
// both returns with complete two-coordinate correction 821. Every form regresses; original
// input observation, direct output consumers and complete executable source restored.
// @retail 0x185be0
void __stdcall function_185be0(long arg_0, long arg_1, real arg_2, real arg_3, s_186ab4 *arg_4, s_186ab3 *arg_5)
{
    byte *local_0 = *(byte **)((byte *)g_4e034c + 0xf4);
    s_185be6 *local_1 = (s_185be6 *)((byte *)g_4ed284 + 0x14) + arg_1;
    s_185be0 local_2;
    memset(&local_2, 0, sizeof(local_2));
    memset(arg_5, 0, sizeof(*arg_5));
    function_185be5(&arg_5->field_28);
    if (arg_0 != NONE && g_4e61cc[(short)arg_0])
    {
        arg_4->field_0 = g_4e61b9 ? &g_4e630c : &g_4e61dc[(short)arg_0];
        byte local_3[16];
        word local_4[16];
        memset(local_3, 0, sizeof(local_3));
        memset(local_4, 0, sizeof(local_4));
        s_185be7 local_5;
        local_5.field_0[0] = *(real *)(g_51ea18 + arg_0 * 0x1c) * 0.01745329238474369f;
        local_5.field_0[1] = *(real *)(g_51ea18 + arg_0 * 0x1c + 4) * 0.01745329238474369f;
        local_2 = ((s_185be0 *)(g_51ea18 + 0x70))[arg_0];
        long local_7 = local_1->field_0;
        if (local_7 != NONE)
        {
            byte *local_8 = function_185be2(local_7);
            long local_9 = *(long *)(local_8 + 0x14);
            if (local_9 != NONE)
            {
                short local_10 = *(short *)(local_8 + 0x1fc);
                if (local_10 != NONE)
                {
                    byte *local_11 = function_185be2(local_9);
                    byte *local_12 = (byte *)g_4e3b44[*(long *)local_11 & 0xffff].data;
                    byte *local_13 = *(byte **)(local_12 + 0x1cc) + local_10 * 0xb0;
                    real local_14 = *(real *)(local_13 + 0x54);
                    real local_15 = *(real *)(local_13 + 0x58) - local_14;
                    real local_16 = 0.0f;
                    if (local_15 > 0.0f)
                        local_16 = function_185be1((magnitude3d((vector3f *)(local_11 + 0x88)) - local_14) / local_15, 0.0f, 1.0f);
                    real exponent = *(real *)(local_13 + 0x5c);
                    if (exponent != 0.0f)
                        local_16 = (real)pow(local_16, exponent);
                    if (*(real *)(local_13 + 0x44) > 0.0f || *(real *)(local_13 + 0x48) > 0.0f)
                        local_5.field_0[0] = ((*(real *)(local_13 + 0x48) - *(real *)(local_13 + 0x44)) * local_16 + *(real *)(local_13 + 0x44)) * 0.01745329238474369f;
                    if (*(real *)(local_13 + 0x4c) > 0.0f || *(real *)(local_13 + 0x50) > 0.0f)
                        local_5.field_0[1] = ((*(real *)(local_13 + 0x50) - *(real *)(local_13 + 0x4c)) * local_16 + *(real *)(local_13 + 0x4c)) * 0.01745329238474369f;
                }
            }
        }
        arg_5->field_0 = local_2.field_38;
        arg_5->field_4 = local_2.field_3c;
        if (!(g_4ed284->flags10 & 1) && (!g_510c54->active || !g_510c54->unknown01))
        {
            real local_8 = (real)fabs(local_2.field_44);
            real local_9 = (real)fabs(local_2.field_40);
            real local_10 = 1.0f;
            if (local_8 > 0.1f && local_9 > 0.1f)
            {
                if (local_8 > local_9) { local_9 /= local_8; local_8 = 1.0f; }
                else { local_8 /= local_9; local_9 = 1.0f; }
                local_10 = (real)sqrt(local_9 * local_9 + local_8 * local_8);
            }
            real local_11 = function_185be1(local_10 * local_2.field_40, -1.0f, 1.0f);
            real local_12 = function_185be1(local_10 * local_2.field_44, -1.0f, 1.0f);
            s_185be7 local_13;
            local_13.field_0[0] = function_185b30(*(short *)(local_0 + 0x74), *(real **)(local_0 + 0x78), local_11) * local_5.field_0[0];
            local_13.field_0[1] = function_185b30(*(short *)(local_0 + 0x74), *(real **)(local_0 + 0x78), local_12) * local_5.field_0[1];
            if (fabs(local_11) >= *(real *)(local_0 + 0x48))
            {
                real local_15 = function_185be1(local_1->field_78 / *(real *)(local_0 + 0x4c), 0.0f, 1.0f);
                local_13.field_0[0] *= (*(real *)(local_0 + 0x50) - 1.0f) * local_15 + 1.0f;
                local_1->field_78 += arg_2;
            }
            else local_1->field_78 = 0.0f;
            if (fabs(local_12) >= *(real *)(local_0 + 0x48))
            {
                real local_15 = function_185be1(local_1->field_7c / *(real *)(local_0 + 0x54), 0.0f, 1.0f);
                local_13.field_0[1] *= (*(real *)(local_0 + 0x58) - 1.0f) * local_15 + 1.0f;
                local_1->field_7c += arg_2;
            }
            else local_1->field_7c = 0.0f;
            if (local_1->field_0 != NONE && local_1->field_2e != NONE)
            {
                real local_15 = 1.0f / function_c8880(local_1->field_0, local_1->field_2e);
                local_13.field_0[0] *= local_15;
                local_13.field_0[1] *= local_15;
            }
            if (local_1->field_0 != NONE)
            {
                real local_15 = 1.0f - *(real *)(function_185be2(local_1->field_0) + 0x2e4) *
                    *(real *)(*(byte **)((byte *)g_4e034c + 0x134) + 0x7c);
                local_13.field_0[0] *= local_15;
                local_13.field_0[1] *= local_15;
            }
            vector3f local_15;
            function_1a4900(arg_1, &arg_5->field_28, &local_15);
            local_1->field_68 = local_15.k > 0.0f;
            real local_16;
            real local_17;
            if (local_1->field_68 && arg_2 > 0.0f && (fabs(local_13.field_0[0]) > 0.0001f || fabs(local_13.field_0[1]) > 0.0001f ||
                fabs(arg_5->field_0) > 0.0001f || fabs(arg_5->field_4) > 0.0001f))
            {
                real local_18 = arg_3 / arg_2;
                real local_19 = function_185be1(*(real *)local_0, 0.0f, 1.0f) * local_15.k;
                real local_20 = function_185be1(*(real *)(local_0 + 4), 0.0f, 1.0f) * local_15.k;
                local_16 = function_185be1(local_15.i * local_18, -3.1415927410125732f, 3.1415927410125732f) * local_20 + (1.0f - local_19) * local_13.field_0[0];
                local_17 = function_185be1(local_15.j * local_18, -1.5707963705062866f, 1.5707963705062866f) * local_20 + (1.0f - local_19) * local_13.field_0[1];
            }
            else
            {
                local_16 = local_13.field_0[0];
                local_17 = local_13.field_0[1];
            }
            arg_5->field_10 = local_16 * arg_2;
            arg_5->field_14 = local_17 * arg_2;
            *(real *)((byte *)arg_5 + 0x24) = local_2.field_48;
            if (function_185be3(local_1->field_0))
            {
                long local_18 = *(long *)(function_185be2(local_1->field_0) + 0x3e4);
                if (local_18 != NONE)
                {
                    long local_19 = *(long *)(function_185be2(local_18) + 0xb4);
                    if (local_19 != NONE)
                    {
                        s_havok_component *local_20 = (s_havok_component *)(g_51e9b8->data + (local_19 & 0xffff) * 0xa0);
                        short local_24 = function_0b67a0((s_small_index *)local_20);
                        if (local_24 != NONE)
                        {
                            transform4x3f local_21;
                            local_20->rigid_body_matrix_get(local_24, &local_21);
                            local_15 = *(vector3f *)((byte *)&local_21 + 4);
                            if (local_18 == local_1->field_40)
                            {
                                function_11df30(&local_5.field_8, &local_15);
                                function_11df30(&local_13.field_8, &local_1->field_34);
                                arg_5->field_10 += local_5.field_8.i - local_13.field_8.i;
                                arg_5->field_14 += local_5.field_8.j - local_13.field_8.j;
                            }
                        }
                    }
                    local_1->field_34 = local_15;
                }
                local_1->field_40 = local_18;
            }
        }
        else
        {
            arg_5->field_10 = 0.0f;
            arg_5->field_14 = 0.0f;
            *(real *)((byte *)arg_5 + 0x24) = 0.0f;
        }
        word local_8 = local_1->field_e & local_1->field_c;
        if (local_8)
        {
            for (long local_9 = 0; local_9 < 16; ++local_9)
            {
                if ((local_8 & (1 << local_9)) && !local_2.field_0[local_9])
                {
                    local_1->field_c &= ~(1 << local_9);
                    local_1->field_e &= ~(1 << local_9);
                }
            }
        }
        local_8 = local_1->field_c;
        for (long local_24 = 0; local_24 < 16; local_24++)
            if (!(local_8 & (1 << local_24))) { local_3[local_24] = local_2.field_0[local_24]; local_4[local_24] = local_2.field_10[local_24]; }
        bool local_9 = function_185be3(local_1->field_0) && function_e4050(local_1->field_0);
        if (local_3[10] == 1)
        {
            if (local_9 || arg_5->field_4 * arg_5->field_4 + arg_5->field_0 * arg_5->field_0 < 0.25f) local_1->field_88 = 1;
        }
        else if (!local_3[10]) local_1->field_88 = 0;
        function_185be4(&arg_5->field_18, 1, local_1->field_88 != 0);
        bool local_10 = local_3[11] == 1;
        if (local_10 && local_1->field_2e == NONE && arg_4->field_0)
        {
            long local_11;
            if (local_1->field_0 != NONE)
            {
                byte *local_12 = function_185be2(local_1->field_0);
                short local_13 = *(char *)(local_12 + 0x212);
                local_11 = local_13 != NONE ? *(long *)(local_12 + local_13 * 4 + 0x218) : NONE;
            }
            else local_11 = NONE;
            if (((byte *)arg_4->field_0)[6] > 0x7f || ((byte *)arg_4->field_0)[7] > 0x7f) local_10 = false;
            if (local_11 != NONE && !function_100f70(local_11) && local_2.field_44 * local_2.field_44 + local_2.field_40 * local_2.field_40 > 0.64000004529953f) local_10 = false;
        }
        function_185be4((dword *)&arg_5->field_1c, 4, local_10);
        function_185be4((dword *)&arg_5->field_1c, 8, local_3[11] != 0);
        if (local_2.field_4e)
        {
            arg_5->field_8 = local_2.field_30;
            arg_5->field_c = local_2.field_34;
        }
        else
        {
            arg_5->field_8 = local_2.field_34;
            arg_5->field_c = local_2.field_30;
        }
        byte local_11 = local_3[local_2.field_4c];
        byte *local_12 = &local_3[local_2.field_4c];
        if (local_1->field_0 != NONE && function_185be2(local_1->field_0)[0x212] != 0xff && function_185be2(local_1->field_0)[0x213] != 0xff)
        {
            *local_12 = 0;
            local_1->field_90 = local_11 > 0;
        }
        else if (local_1->field_90)
        {
            if (local_11 > 0) { local_11 = 0; *local_12 = 0; }
            else local_1->field_90 = 0;
        }
        function_185be4(&arg_5->field_18, 0x10000, local_3[7] != 0);
        function_185be4(&arg_5->field_18, 0x200000, local_11 != 0);
        function_185be4(&arg_5->field_18, 0x20000, local_11 != 0);
        if (g_4e6948->state != 2) function_185be4(&arg_5->field_18, 0x20000000, local_3[local_2.field_4d] != 0);
        function_185be4(&arg_5->field_18, 0x800, local_11 != 0);
        function_185be4(&arg_5->field_18, 0x4000, local_11 != 0);
        function_185be4(&arg_5->field_18, 0x4000000, local_3[6] != 0);
        function_185be4(&arg_5->field_18, 0x80, local_3[12] != 0);
        function_185be4(&arg_5->field_18, 0x100, local_3[13] != 0);
        function_185be4(&arg_5->field_18, 4, local_3[5] != 0);
        function_185be4(&arg_5->field_18, 0x100000, local_3[5] != 0);
        function_185be4(&arg_5->field_18, 0x2000000, local_3[5] != 0);
        local_1->field_93 = local_3[0] && ((g_510c50 && ((byte *)g_510c50)[5]) || local_1->field_93);
        function_185be4(&arg_5->field_18, 2, local_3[0] && !local_1->field_93);
        function_185be4(&arg_5->field_18, 0x10, local_3[4] != 0);
        function_185be4(&arg_5->field_18, 0x20, local_3[4] != 0);
        function_185be4(&arg_5->field_18, 0x40, local_3[7] != 0);
        function_185be4((dword *)&arg_5->field_1c, 2, local_3[1] == 1);
        function_185be4((dword *)&arg_5->field_1c, 0x10, local_3[14] != 0);
        function_185be4((dword *)&arg_5->field_1c, 0x20, local_3[15] != 0);
        if (local_3[14] == 1) ((byte *)arg_5)[0x21] |= 1;
        else ((byte *)arg_5)[0x21] &= ~1;
        s_input_state *local_14 = NULL;
        if (g_4e61cc[(short)arg_0]) local_14 = g_4e61b9 ? &g_4e630c : &g_4e61dc[(short)arg_0];
        if (local_14 && ((byte *)local_14)[0x19]) *(dword *)&arg_5->field_1c |= 0x40;
        long local_15 = (long)(*(real *)(*(byte **)((byte *)g_4e034c + 0xf4) + 0x7c) * 1000.0f);
        if (local_3[2] > 0) ((byte *)arg_5)[0x20] |= 1; else ((byte *)arg_5)[0x20] &= ~1;
        if (local_4[2] >= local_15) ((byte *)arg_5)[0x20] |= 2; else ((byte *)arg_5)[0x20] &= ~2;
        bool input_3_active = local_3[3] > 0;
        if (input_3_active) ((byte *)arg_5)[0x20] |= 0x10; else ((byte *)arg_5)[0x20] &= ~0x10;
        if (local_4[3] >= local_15) arg_5->field_20 |= 0x20; else ((byte *)arg_5)[0x20] &= ~0x20;
        if (input_3_active) local_1->field_8e = 1;
        else
        {
            if (local_1->field_8e && !local_1->field_8f) *(dword *)&arg_5->field_1c |= 1;
            local_1->field_8e = 0;
            local_1->field_8f = 0;
        }
    }
    player_control_update_action_flags((s_player_action *)arg_5, (s_player_action_triggers *)&local_2);
}
#pragma inline_depth(255)
