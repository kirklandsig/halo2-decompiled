#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "unknown_11cb00.h"
#include "unknown_11cc90.h"
#include "slot_handler.h"
#include "unknown_1e1f20.h"
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
    dword field_1c;
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

#define FLAG(bit) (1 << (bit))
#define SET_FLAG(flags, bit, value) ((value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)))
#define PIN(value, minimum, maximum) ((value) < (minimum) ? (minimum) : ((value) > (maximum) ? (maximum) : (value)))
__forceinline byte *function_185be2(long arg_0)
{
    return *(byte **)(g_4e0300->data + (arg_0 & 0xffff) * 12 + 8);
}
__forceinline bool function_185be3(long arg_0)
{
    return arg_0 != NONE && ((1 << g_4e0300->data[(arg_0 & 0xffff) * 12 + 3]) & 1);
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

// MiniMax H (11-20): no new form, no lower score (396): the two angle conversions take swapped XMM registers, retail's unit-record copy sits between the products and the two stores, and the body is 21 bytes shorter.
// score 838 (was 838): Frame 0xe0 and the head agree; with 0x24590 expanded inline (main since 11-20) the record copy, the
// stick-scale branch layout and the field_93 latch shift the body, and register ties remain (see r134 below).
// retail's 3777 bytes. Left: the input-record copy is scheduled after the two angle stores here
// (retail before them), 1.0 and zero remade in the other order after pow, the second hold-time add loads the argument
// first, the quotient in XMM4 and 1.0 in XMM3 (retail the other way round), the else arm loads local_16 first, one
// scratch step at the parent lookup, the first mask in EAX (retail EDX) and the 0.64 sum squares field_40 first.
// Earlier 396 (was 396, before 0x24590 became inline on main): Frame 0xe0, the head and the flag block agree. The XMM
// state after 0x24590, one register copy before 0x100f70 and two stack homes differ.
// M3 lane 3 r268 (fast): the input-record copy ahead of the two angle conversions 395 (the copy then precedes the angle
// stores as retail, registers unchanged); that copy first with the second angle converted first 393 (the pair then takes
// retail's XMM7/XMM0 and only the conversion constant stays in XMM2 where retail has XMM1, which retail frees because it
// treats 0x24590 as clobbering); both a gain under the keep bar from statement order only. The angle pair read through one
// `real *` pointer 957; the pair copied as one vector2f and scaled in place 949 (3772 bytes); the range fraction as one
// conditional expression with the clamp inside 911 (3788 bytes). Still BLOCKED-BY 0x24590 (its file is avoided this round).
// MiniMax E (11-17): no new form, no lower score (590): the body is still two bytes over retail's 3777, the control global is still carried in ebx from the entry block where retail reads it into ecx a second time at the flag test after the magnitude and pow calls, and the only spelling seen to reproduce that reload takes the global's address, which this lane may not do.
// MiniMax E (11-14): no new form, no lower score (590): the length is still two bytes over retail's 3777 (the
// `xor bl, bl` of the bit-4 flag block, where retail branches straight to the clear arm), and the flag chain on
// field_18 stays misaligned because retail keeps the flag condition in EDX across the stores and writes the word
// inside each helper arm while ours re-tests the condition per call; the forms tried this round (nested if, a
// second bool, a hoisted "local_11 != 0" flag) all scored worse, and the reload of the control global that would
// free EBX for NONE lives in another file.
// MiniMax H (11-11): no new form, no lower score (590): retail keeps NONE in EBX for its early uses and re-reads
// the control global after the call to 0x24590, whose declaration lives in src/unknown_023540.cpp, so the reload
// cannot be produced from this file; the forms tried here that would force a re-read score worse.
// MiniMax B (11-11): no new form, no lower score (590): retail's entry block reads the control global into ECX and
// forms the +0x14 address from it, ours reads it into EBX and keeps it there, and the NONE constant swaps registers
// with it; I found no spelling that makes this build reload the global.
// MiniMax C (11-10): score 590 (was 590): no lower score this round. Retail reads the control global
// twice, into ECX both times: at the top of the entry block, where it only feeds the local_1 address, and
// again at the flag test after the magnitude and pow calls, so the value is not carried across them and
// EBX is free for the NONE constant that retail holds for its seven early uses. Ours carries the global
// in EBX from entry to the flag test, because this build proves that the calls in between cannot change
// it, and NONE then lives in ECX. The forms that would force a second read were measured before and score
// worse (a NONE local for the first test and the unit tests 739, a volatile re-read 1020, an unknown
// extern in place of pow 1013 or magnitude3d 1017); the only spelling seen to reproduce retail's reload
// exposes the global's address, which this lane may not do.
// Coordinator (m3, round 56 review): kept at 590 is the consistent part of the 516 form: the five `field_20` flags through SET_FLAG on
// the word field (no byte casts) and the selected input as `long`. The 516 form puts PIN and SET_FLAG on a subset of call sites picked
// by score while the rest stay on 0x185be1/0x185be4 (all clamps through PIN 860 on 589; all 22 flags through SET_FLAG 792 on 590), a
// match-only form; it is held as build/held/lane15-r56-185be0-mixed.patch.
// MiniMax E (11-11): no new form tried, no lower score (590): retail reads the control global twice into
// ECX - once at the top of the entry block, where it only feeds the local_1 address, and again at the flag
// test after the magnitude and pow calls - while this build carries it in EBX from entry to the flag test
// and keeps NONE in ECX; the forms that would force the second read score worse (a NONE local for the first
// test and the unit tests 739, a volatile re-read 1020), and the only spelling seen to reproduce the reload
// exposes the global's address, which this lane may not do, so the body stands.
// MiniMax A (11-13): no new form, no lower score (590); retail still reads the control global a second
// time into ecx at the flag test where this build carries it in ebx from the entry block, and retail still
// keeps none in ebx for its early tests.
// MiniMax B (11-14): no new form, no lower score (590); the extent is retail's 3777 bytes to within two, and the walk shows the
// gap is in the block layout and the stack homes of the input/output records, not in the statements; the earlier rounds' counted
// mask, selected-byte and input-copy forms are the only ones that moved the frame at all.
// MiniMax R (11-15): no new form, no lower score (590): the body is still two bytes over retail's 3777 and the
// control global is still carried in EBX from the entry block, where retail reads it into ECX a second time at the
// flag test after the magnitude and pow calls and keeps NONE in EBX for its early uses; the only spelling that
// reproduces the reload takes the global's address, which this lane may not do.
// MiniMax I (11-17): no new form, no lower score (590): the body is still two bytes over retail's 3777 and the
// control global is still carried in EBX from the entry block, where retail reads it into ECX a second time at the
// flag test; the only spelling that reproduces that reload takes the global's address, which this lane may not do,
// and the earlier rounds' counted-mask and selected-input forms all score above the score here.
// Earlier 622 and the mixed form 516: 3773/3777 B, masked aligned about 157 (stack offsets and registers masked; 233 at 622). Round 56 (m3 lane 15, plain): PIN on
// yaw/pitch 608, plus settings 781, plus limit clamps 827, plus turn rates 602, turn rates alone 619, yaw/pitch + turn rates + settings 589 (kept);
// a ternary on named yaw/pitch values 608. Flags: all 22 through a set-flag macro 614 (3760 B, masked 140: retail's threaded tests), with long selected
// input 606, bitfield selectors 604, selector struct copy 606, named selector dword 604, byte-pointer flags 609, word flags 687 (3742 B). On 589, word
// `field_20` flags through SET_FLAG 569 (kept); on that long selected input 562, the four `local_11` flags 526, both 518 (kept), `local_3[5]` flags 577,
// `local_3[4]` flags 566, three `field_1c` flags 569. On 518, `local_3[4]` flags 760 (masked 149), two `field_1c` flags 671 (masked 147), three single-use
// flags 518 (same code), fraction clamped in place as if/else or PIN 515, named `input_3_active` kept 516 (kept); a selected-input mask as long 712 (3778 B).
// Diagnostic, restored: a volatile read of the root global at the flag test 1020: EBX stops holding the root but NONE still stays out of it, masked 232.
// Next: byte offsets run 29 to 49 ahead from the fraction clamp on, because retail sinks that clamp's upper arm and the range <= 0 arm out of line and
// remakes zero after the 0x24590 call (ours keeps XMM1/XMM6 live across it, which `inline` on 0x24590 may change; that file was on the avoid list);
// retail reads the selector bytes as one dword (`mov ebx, [esp+0xb8]`, `movzx ecx, bl`/`bh`) and holds NONE in EBX for the first three tests.
// Coordinator (m3, round 56 review): kept at 590 is the consistent part of the 516 form: the five `field_20` flags through SET_FLAG on
// the word field (no byte casts) and the selected input as `long`. The 516 form puts PIN and SET_FLAG on a subset of call sites picked
// by score while the rest stay on 0x185be1/0x185be4 (all clamps through PIN 860 on 589; all 22 flags through SET_FLAG 792 on 590), a
// match-only form; it is held as build/held/lane15-r56-185be0-mixed.patch.
// Earlier 622 (was 690): Frame 0xe0 and state/output carriers agree (3787/3777 B); control-root EBX versus NONE,
// settings/zero homes, the range-fraction clamp (retail clamps in local_16's slot, upper arm out of line) and
// the materialized `&&` flag arguments (retail threads them) remain.
// Round 130 (m3 lane 8, fast, kept forms plain): the rigid-body index from 0x0b67a0 as `long` (retail's
// `movsx eax, ax; cmp eax, -1`) 690 alone (aligned 268 -> 266); the 16-input copy indexed with no duration
// cursor 731 alone (3766 B, aligned 258), both 648 (kept); the 0x218-table entry as `local_11 = NONE; if (index
// != NONE) local_11 = ...;` 622 (kept, retail's `or edx, -1` preload); with the NONE default at the
// declaration 656. On 622: the view entry as `&g_4ed284->entries[arg_1]` 690 on the old body (same code); view
// entry before the settings pointer 730; `local_1->field_c` tested in the copy loop 733; named yaw/pitch clamps
// 648 on 648 (aligned worse); the latch in a bool local 622 (same code); the latch as `? 1 : 0` 625 (gives
// retail's `mov eax, 1`); every flag through a SET_FLAG/FLAG macro as 0x187b30's file 805 (3760 B; aligned 264
// -> 228: retail's threaded flag tests and stores); the fraction assigned then clamped into itself 622 (same
// code), with SET_FLAG 805.
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
// DH round 268: independent byte/word scratch declarations, clear order and entry scope,
// seven subsets 731/743/743/731/731/743/743; complete graph with purpose names 743.
// Original scratch lifetimes, CRT clears and complete body restored.
// Round 352: Settings/state acquisition after local-input clear or full output reset,
// independently/together and both orders: 1012/743/1010/741/1012/1012/744/744. Original
// acquisition order, helpers and complete body restored; control-root/NONE, settings/zero homes
// and flag joins remain.
// Round 506: single held-input/filter-copy traversal, live/maintained masks with
// dword/word bits and long/short ordinals 1148/1136/1130/1109/1148/1136/1133/1112.
// Frames grow to 0xe4/0xe8; original two scans, masks and complete body restored.
// Round 305: Exponent declarations before range scalars, parent lookup, active lookup and function
// entry: real 731 each; complete double power overload 951 each, frame 0xe8. Original overload, read
// point and full body restored; root/NONE, scalar homes and flag joins remain.
// M294: Consumed range/filter-copy-mask/angular-output scopes, seven subsets,
// then all with immutable controlled index:
// 731/731/731/731/731/731/731/731. Original complete executable source restored.
// Round 257: Actual button/duration source cursors and held-input source cursor, seven
// subsets, then short copy ordinal: 731/690/839/690/839/845/845/854. Keep only the duration
// source cursor, 690 plain at 3782/3777 bytes; the paired source cursor ties.
// Sole caller 0x186ab0 remains 134/1486. Control-root/NONE, settings/zero and flag joins remain.
// Round 237: Native copy destination button/duration/both cursors 690/691/691; inferred byte/word/both
// flag destinations 858/848/864; complete exact-valued promoted angular coefficient
// alone/with native destinations 1018/1075. Original helper, widths, scalars and body
// restored; control-root/NONE, settings/zero homes and flag joins remain.
// AC468: Shared input-five/four/seven branch ownership retaining the individual flag-helper calls,
// seven subsets and all with selected-input ownership: 1121/1111/1152/1134/1155/1159/1150/1140.
// The branches collapse stores and lose the native frame. Original helper calls, masks, observations,
// two scans and retained duration cursor restored; root/NONE, settings/zero and flag joins remain.
// AC474: Existing object-predicate positive/failure result ownership in bool/byte/word/long,
// 826/826/829/835/852/852/846/872. Original conjunction and complete body restored; native frame,
// control-root/NONE, settings/zero homes and flag joins remain.
// AC477: Contiguous invalid-index/float-zero/short-zero reset fills, subsets 1..7, then
// ordinary inline complete fill: 960/1021/1030/694/962/1021/1030/1030. Original reset and
// complete body restored; control-root/NONE, settings/zero homes and flag joins remain.
// Round 526: Actual held-input mask shifted through low bits, fixed/sparse termination,
// word/dword remainder and indexed/cursor input 802/832/821/856/1109/1109/1125/1125.
// Original two scans, duration cursor and complete body restored; control-root/NONE,
// settings/zero homes and flag joins remain.
// N322: Late action-eligibility/selected-input/hold-release phase scopes, subsets 1/2/4/3/5/6/7,
// then complete phase-carrier names: 690/690/690/690/690/690/690/690. All generated target metrics
// tie; original complete body restored, control-root/NONE, settings/zero homes and flag joins remain.
// AC480: Native entry array, branch-owned angular output and shared two-coordinate difference,
// subsets 1/2/4/3/5/6/7, then branch output with early uninitialized response vector: 690/839/764/839/764/855/855/839.
// Original complete body restored; control-root/NONE and scalar homes remain.
// Round 372: complete allowed/blocked turn-admission results in bool/byte/word/long
// storage: 692/692/692/692/692/692/692/692. Original direct gate and body restored;
// control-root/NONE, settings/zero homes and flag joins remain.
// AC486: Diagonal multiplier exhaustive/negative-first/predicate/two-stage producers
// 894/894/894/894; complete paired square parts 894/894/894/894. Original
// multiplier, predicates, float trees and body restored; control-root/NONE and scalar homes remain.
// Y539: Existing time-state/component/matrix producers, subsets 1/2/4/3/5/6/7:
// 692/690/690/692/692/690/692; complete graph with declared forward member 692.
// All retain 3782 bytes and first +0x22. Original producers, declarations and body
// restored; control-root/NONE, settings/zero homes and flag joins remain.
// AC494: Retained-byte/normalized held-input/common release-latch result producers,
// masks 1/2/4/3/5/6/7, then byte held input: 751/698/986/739/1008/980/990/990.
// NOT kept: extra old-value/common latch stores lack the native conditional shape.
// Original complete body, helper calls, stores and all input observations restored.
// Round 127 (lane 8, fast): entry test inverted with an empty then-arm 992; range fraction zeroed in an else arm
// 948; exponent field read at each use 809; lower bound read at each use 927; clamp helper as two independent
// ifs 690 (same code).
// m3 lane 11 r60 (fast 690): diagnostic, taking the view global's address (in the body or a file-scope
// pointer) 1019 at 3798 bytes: ours then loads it at entry into ECX and re-reads it at the flag test after the
// pow call as retail, so retail's reload is an address-exposure kill, but NONE then leaves EBX entirely; the pow
// exponent read from the field at both uses 809 (its loads become retail's `fld [esi+0x5c]`); the range fraction
// declared before the minimum / above the second lookup 694/694; the fraction as a ternary or an if/else 948/948.
// m3 lane 11 r63 (fast): retail places the range-fraction clamp's upper arm after the hot path (blocks at
// 0x185f23); clamp helper forms: `lo > x ? lo : (x > hi ? hi : x)` (the sibling pitch-pin form) 748, early
// returns with an if/else chain 748, const& value with early returns 854 (3798 bytes), const& value nested
// ternary 855 (3798 bytes), `x >= lo ? (x <= hi ? x : hi) : lo` 951 (3801 bytes). Original helper restored.
// AC497: Entry index presence/absence retained across the output clear,
// bool/byte/short/long, eight forms 690 each. Original admission, reset,
// root observations and complete body restored; root/NONE and scalar homes remain.
// AC509: Existing reset returns its actual action subobject, pointer/reference inputs
// and pointer/reference results, ignored then consumed: 622 each (eight forms). Original
// reset, receiver, six stores and complete body restored; control-root/NONE homes remain.
// B396: Final auxiliary-input eligibility bool/byte/short/long at consumption 622/622/622/622;
// admitted-branch source/result 623/623/627/626. NOT kept: no exact one-use/branch-result form;
// original complete body restored. Control-root/NONE, settings/zero and flag schedules remain.
// Round 404: Main/secondary direct flag macros 806/657; low/high/all word-input publications
// 616/616/603; with main/secondary/both 801/645/774. NOT kept: 603 widens native byte stores. Consistent
// byte macros 616 (six-point gain below eight). Original flag widths/helper/body restored; root/NONE and
// scalar homes remain.
// P308: Existing reset as a native subobject/whole-output member, forced/ordinary expansion,
// 622/1013/622/1013; complete clear owned by output, without/with actual returned output,
// 1013/1013/1013/1013. NOT kept: no gain; original free reset, clears and full body restored.
// P310: Plain two-axis turn/attachment-scale/object-scale array loops, subsets 1..7:
// 991/616/991/616/991/616/1185; complete loops with shared axis 1185.
// NOT kept: six-point best gain is below gate; original array producers and full body restored.
// P312: Existing flag setter installed-state bool/byte/word/long returns, common and
// branch-owned results: 622/622/622/622/622/622/622/622. NOT kept: no match or gain;
// original void producer and full body restored; root/NONE and scalar/flag homes remain.
// M351: Immutable existing range minimum/span/exponent, all seven subsets, then all with
// a read-only range-record binding: 622 each. Original observations and complete body restored;
// control-root/NONE, settings/zero homes and clamp/flag joins remain.
// DH326: Existing flag mutator with short/word/dword canonical predicates, pointer then
// native reference destinations: 626/626/625/626/626/625; ignored pointer/reference destination
// results with Boolean predicates 622/622. Original helper and full body restored; root/NONE
// ownership, settings/zero homes and flag joins remain.
// DH330: Own-input homes at each of the six original first consumers 1031/622/622/622/622/622;
// controller/action homes at all consumers 1031/623. NOT kept: unmatched own-input
// indirections; original arguments, helpers and complete body restored.
// DH338: Actual range-lower/angular-bound/curve-count/curve-pointer/paired-curve/
// lower-angular/lower-curve/all native field bindings: 927/622/622/622/622/927/927/927.
// Original body restored; root/NONE and scalar homes remain; lower reload lacks equality.
// Y573: CRT return-owned output/button/duration destinations, seven subsets, then input record
// ownership: 1031/1018/754/1096/1048/1059/1132/1077. NOT kept: all regress; destination aliases
// and own-record indirection miss equality; native root/NONE and scalar homes remain. Original
// complete body and helpers restored.
// Y575: Complete button/duration copy then masked clearing, memcpy/memmove policies:
// 1029/1006/1021/995/988/901/1001/1015. NOT kept: all regress; memcpy shrinks the frame
// below 0xe0; memmove retains root/NONE mismatch. Original two scratch clears, filter
// copy, helper contracts and full body restored.
// J250: Actual sampled-input auxiliary/movement/look/auxiliary-movement native pairs
// 622/622/622/622; opaque auxiliary formal with typed/direct pointer access 622/622;
// complete vector/point input pairs 622/622. NOT kept: no root/NONE, settings, frame
// or flag improvement. Original input record, body and outside declaration restored.
// P319: Existing clamp promoted value/lower/upper formal domains, masks 1/2/4/3/5/6/7,
// then complete double result: 1211/1129/1205/1211/1211/1314/1211/1286. NOT kept:
// no match, wider x87 domains enlarge frame/length. Original helper and whole body restored.
// AC525: Complete initial input-availability/unavailability results in bool/byte/short/long,
// 624/624/624/624/624/624/624/624. NOT kept: predicate residence unmatched; original
// gate, resets, observations and complete body restored; control-root/NONE homes remain.
// AC529: Complete post-callback correction admission, allowed/blocked bool/byte/short/long:
// 622/622/622/622/626/626/626/626. NOT kept: predicate residence does not match; control-root/NONE and scalar/flag homes remain.
// AC532: Controlled/parent-tag/bounds expiry subsets 1/2/4/3/5/6/7:
// 622/626/626/626/626/626/626; complete scopes with immutable index 626.
// NOT kept: no match or gain; original observations, scopes and full body restored.
// N366: Complete initial/interpolated angle and curve-response adjacent result producers, seven families
// and complete duration-scaled outputs, all tie. Original body and helpers restored; control-root/NONE,
// settings/zero homes and clamp/flag joins remain. Scores 622/622/622/622/622/622/622/622. NOT kept:
// unmatched result-ownership forms.
// Round 52 (m3 lane 2, fast; aligned count without stack offsets in brackets, base 622 [348]): range fraction assigned in the arm and clamped in place with an
// if/else, value-first test, ternary or separate declaration 619 each [347] (same code; 3 points, not kept; the upper arm still sits inline, retail's jumps back
// into the first arm's store); named globals pointer at entry 622; diagnostic, reverted at once: the flag test reading the global through its address 1020
// (3803 bytes; the global leaves EBX, NONE stays in ECX), with the clamp 1020 [334]; whole active body in a compiled-out loop 1040 (3805 bytes [358], global
// reloaded as retail, arg_5 then takes EBX); attachment block in one 991 (3803 bytes [355]). Retail's EBX for NONE is still open.
// AC534: First held-input scan collects released/retained word/dword masks for both
// metadata words, indexed then source-cursor reads: 1099/1116/1039/1097/1026/1039/984/998.
// NOT kept: deferred publication regresses, frame moves from 0xe0 and first gap to +2;
// original two scans, per-bit stores and full body restored. Root/NONE and scalar homes remain.
// AC536: Complete post-filter override-query allowed/blocked results in bool/byte/short/long:
// 714/714/726/725/714/714/726/725. NOT kept: result residence lacks equality; original query,
// short circuits, latch, helpers and full body restored. Control-root/NONE homes remain.
// AC538: Complete positive-span admission before range-fraction production, positive/complementary
// bool/byte/short/long: 624/624/624/624/624/624/624/624. NOT kept: unmatched decision/storage forms;
// original complete body restored.
// Control-root/NONE, settings/zero homes and clamp/flag joins remain.
// AC540: Paired range-angle override/default decisions, bool/byte/short/long: 622/622/622/622/750/750/750/750.
// NOT kept: unmatched result residence; control-root/NONE and settings/zero homes remain.
// P340: Existing real truncation helper at hold-threshold conversion, direct/native real/immutable
// real/rounded double/immutable double/seconds/seconds+milliseconds/immutable producers:
// 638/638/638/709/709/638/638/638. NOT kept: unmatched conversion ownership; original cast and complete
// body restored. Control-root/NONE, settings/zero and flag joins remain.
// T59: Complete writable clamp inputs at all nine consumers, pointer/reference inputs and
// void/value/pointer/reference results: 622 each. NOT kept: native x87 slot and control-root/NONE
// ownership unchanged; original value clamp, observations and complete body restored.
// Round 74 (m3 lane 11, plain): the clamp helper with early returns 727, as one conditional expression 730, with a
// separate result local 970 (3811 B); all seventeen field_18 flags through SET_FLAG 801 (3759 B). The file's
// `#pragma inline_depth(1)`/`(255)` pair removed: 590, byte-identical code (kept as a cleanup).
// Round 152 (m3 lane 5, fast; [m, x] masked and exact aligned counts, body [121, 235]): local_2's copy ahead of the rate stores 589
// [120, 235]; pitch rate first 588 [121, 234]. All 23 action flags through SET_FLAG 776 (3747 B) [96, 211], all ten clamps through PIN
// 999 (3827 B) [128, 258], both 949 (3795 B) [100, 220]. On the all-flags form what is left in the flags part is mostly local_3's
// slot (+0x24 for retail's +0x20), the selector pair (retail `mov ebx, [esp + 0xb8]`, then BL and BH) and which constant is CSE'd
// (ours 0x10, retail 0x20 in ESI). Retail holds NONE in EBX for seven uses (the first test, the three stores, three unit tests), with
// `or ebx, -1` ahead of the second memset; ours holds the root global there. Diagnostic, reverted: the root global reread through a
// volatile at the flag test 1003 on the body, 956 (3747 B) [94, 201] on all flags: EBX is then free but NONE stays in ECX for the
// first block only; one, two, three extra NONE compares in the unit block 953/951/959, with that reread 988: NONE still stays out.
// Round s1C (statement-by-statement walk against retail 0x185be0-0x186a9e): no logic or behaviour difference found (fields, offsets, test
// directions, callees, argument order, the 4x-unrolled mask loop and all 23 flag stores agree; float constants read from the XBE agree).
// Tried, dropped: `real hold_ms` local before the (long) cast 590 (same code); the fraction clamp as `>= 0 ? (> 1) : zero` 950 (3795 B);
// a `long none_index = NONE` local for the first test and the three unit tests 739. Diagnostics, not kept: an unknown extern in place of
// pow 1013, or of magnitude3d 1017: the root global is then reloaded at the flag test as in retail (so retail's pow or 0x24590 acts as a
// clobbering call where ours is known side-effect free), but EBX is still not given to NONE (stays `or ecx, -1`, immediates in the unit tests).
// Round 63 (m3 lane 2, fast; control 590): the two angle limits through one `real const *` row pointer 962; the
// settings copy ahead of the angle conversion 589 (order only, not kept); the two clears swapped 592; the flag test's
// game-time pair as `!(active && unknown01)` 590 (same code); the first test written as an empty `if` with the work in
// its `else` 989 (3781 B). Retail 0x24590 is pure x87 (no XMM writes), yet retail's callers here and in 0xd1e10 treat
// it as clobbering (zero remade, root global reloaded): BLOCKED-BY 0x24590's declaration (src/unknown_023540.cpp,
// avoid list this round; `inline` on it is the open lead).
// Round 109 (m3 lane 12; 590): `inline` on magnitude3d's definition (0x24590) 942 plain (3795 B): zero is then remade
// after the call as in retail, but the root stays in EBX, and 0xd1e10 542 -> 596, 0xf6d10 749 -> 759, 0xf8eb0 947 -> 1030,
// 0x1aa0d0 375 -> 402, 0x1d24a0 902 -> 935, 0x2c0d60 923 -> 991, so that lead is closed. Plain, each restored: the two datum
// helpers through the shared typed header view (object_header_get) 590 (same code); the latched mask in its own dword
// local 703 (3770 B: retail's `movzx` load, but the loop entry loses retail's jmp over the pad); the 0.64 sum written
// field_40 first 590 (same code); the flag helper as one conditional expression 590 (same code). Retail threads each
// known-false edge of the `local_3[11]` latch into the bit-4 clear arm (its own `[edi + 0x1c]` load); ours hoists the
// load and keeps `~4` in ESI. No retail code or data takes 0x4ed284's address (58 plain memory operands), yet retail
// re-reads it after the stores through arg_5.
// MiniMax G (11-14): no new form, no lower score (590): retail loads the settings global into ECX after the first
// register save and gives EBX to NONE, this build loads it into EBX before the saves and gives ECX to NONE, and the
// root global is stored after the second save here and after the first in retail, so every later use of the root and
// of NONE lands in the other register; the forms tried for that are in the notes below and none lowers the score.
// MiniMax B (11-14, run 75): no new form, no lower score (590); the body is left as it was. What still differs: retail keeps the root
// global in ECX and gives EBX to NONE, this build keeps the root global in EBX and gives ECX to NONE, and retail stores the first local
// after `push ebp` while this build stores it before, so every later use of the root and of NONE lands in the other register; the
// remaining register pressure comes from retail's out-of-line calls, which this build inlines.
// m3 lane 11 r102 (plain; the fast check builds this function differently): retail treats 0x24590 like a callee LTCG cannot
// see into (a /GL- file's): right after it retail remakes its XMM6 zero and later reloads the root global at the flag test,
// the same way it treats the `_CIpow` call. Retail 0x24590 itself is pure x87, ours is LTCG-visible, and `inline` on it
// was closed in round 109; a lead for the next round (its build model is the user's decision, not a source form). Forms:
// local_2 zeroed by `= {0}` in place of its memset 1050 (3830 B); the two scratch arrays zeroed by `= {0}` 1001 (3798 B);
// the controlled unit index named once for its four uses after the turn clamps 666; the root global held in a named local
// for its two uses 590 (other code: two scheduling lines swap; the root stays in EBX); the arg_4 table selection as an
// if/else 1012 (3795 B). Same code: the two scratch arrays declared at the top, the angle record declared before them.
// MiniMax S (11-15): no new form, no lower score (590): the head differs in register assignment, with the entry record base in
// EBX where retail keeps it in ECX and the -1 sentinel in the other register, and the two zeroing loops and the store order
// follow from that.
// MiniMax I (11-16): no new form, no lower score (590): the head still differs in register assignment - the entry record base in EBX where retail keeps it in ECX and the -1 sentinel in the other register - and the flag test reloads the root global in retail where this build keeps it in EBX; that reload follows from retail treating 0x24590 as an opaque callee, so the fast check builds this body differently from the full image and no source form in this file can force it.
// View-angle pair copied/moved together or one component at a time
// 791/829/791/1009. Native view pair, native movement source, both, and both
// pairs copied 791/590/791/788. NOT kept: the control-root reload and NONE
// register remain unresolved.
// Signed controller index after the full NONE test, nested admission or a
// single exit guard, in mutable/const short and promoted long: 590/590/590/590/989/989/989/989.
// NOT kept: nested admission ties. The exit guard grows to 3781 bytes.
// The second control-root load and its EBX conflict remain.
// MiniMax I (11-17): no new form, no lower score (590): the head still differs in register assignment - retail loads
// the root global into ecx for one lea and gives ebx to the -1 sentinel, this build keeps the root global in ebx and
// gives ecx to the sentinel - and the first local's home sits at esp+0x3c against retail's esp+0x38; retail reloads
// the root global after the magnitude call because it treats 0x24590 as an opaque callee, and that helper's
// declaration lives in src/unknown_023540.cpp, which this lane may not edit.
// M3 lane 15 r97 (fast, plain confirmed; 0x24590, 0xc8880, 0x185ab0, 0x185b00, 0x185b30, 0x187b30 and 0x1a4900 matched
// throughout, 0x186ab0 114 unchanged; aligned = LCS count in brackets, base 590 [204]). KEPT 590 -> 396 [87], the head now
// retail's (NONE in EBX with `or ebx, -1` ahead of the second memset, the root global read twice):
// - the two unit bytes at +0x212/+0x213 read as signed chars against NONE, as 0x186ab0's view and this function's own
//   weapon lookup read them (the shared 0xff constant had taken BL, which kept the selector bytes out of EBX and the NONE
//   constant out of a callee-saved register);
// - every flag of field_18 and field_1c through the file's SET_FLAG macro (field_1c declared as the dword it is used as; the
//   inline flag helper is gone): consecutive flags on one condition then share one branch, as retail's do;
// - every clamp through a PIN macro in place of the inline clamp helper (retail re-reads a memory operand at each
//   comparison and stores the result in each arm); the speed fraction and the two scaled turn values are named before
//   their clamps, and the blend factor is stored as `1 - a` where it is computed;
// - one object header pointer (the shared object_header_get view) for the type test and the object of the controlled unit;
// - the exponent field read at each use (retail's `fld [esi + 0x5c]`); the tick count through the existing real_truncate
//   (retail's store, reload and `cvttss2si eax`); input 3 tested at both uses (retail tests CL twice; with the named bool
//   again 426).
// Steps, each on the one before (fast): the field_18 flags through SET_FLAG 801 [169]; + real_truncate and the direct input-3
// tests 794 [156]; + the field_1c flags 769 [151]; + the header pointer 712 [147]; + the signed-char compares 914 [97]; + the
// exponent at each use 727 [100]; + PIN for the two memory clamps 640 [99]; + the turn values named and clamped 430-439
// (clamp helper 430, PIN 432, PIN in the sum 439); + `1 - a` stored 415 [91] (clamp helper spellings 417/420); + every
// clamp through PIN 396 [88]. Alone on the 590 body: real_truncate 608, direct input-3 tests 589, header pointer 785, field_1c
// flags 639, signed-char compares 903, exponent at each use 713 (none is a gain alone: the lockstep count needs them together).
// Not kept, on the way: the selector bytes as a copied aggregate 712 (same code), as bitfields of one dword 752 or of one
// word 742, as a word or dword field with byte casts 712/751, as a two- or four-byte array field 712 (same code), through a
// getter returning the aggregate by value 712/744; shared conditions in named bools 733; the element stores indexed in
// place of the element pointer 712 (same code); a 0xfe second constant (diagnostic) 755 [128], which showed the 0xff
// constant was what held EBX. On 396: the default 1.0 in an else arm (retail's `jmp; movaps xmm1, xmm4` edge) 916 [86],
// and, in the inline-magnitude diagnostic state (384): the weapon index through an inline getter 384, the zoom test
// through an inline predicate 384 (same code each), the weapon test ahead of the two byte tests 393.
// Left: retail treats 0x24590 as clobbering the XMM registers (zero and 1.0 remade after it, the angle limit reloaded, the
// conversion constant read from memory), which sets the whole block from the speed fraction to the stick clamps and the
// pad ahead of the mask loop; BLOCKED-BY 0x24590 (`inline` on its definition, measured as a diagnostic with this body plus the
// else-arm default: 384 [65], one shift left; it is not kept because lane 12 r109 saw six other callers' counts rise).
// Then retail's `mov eax, edx` before the 0x100f70 call (the weapon index lives in EDX there), the limit kept in XMM7
// (retail the first, ours the second), the two 4-byte homes at +0x10/+0x14 (mask and counter the other way round), and
// the vector copy order after the two 0x11df30 calls.
// M3 lane 15 r98 (fast, 0x24590, 0xc8880, 0x185ab0, 0x185b00, 0x185b30, 0x187b30, 0x1a4900 MATCH throughout; base 396
// [aligned 98]): the current weapon of the controlled unit through the existing inline unit_get_current_weapon
// (include/unknown_1e1f20.h) in place of the slot lookup written out: that block is then retail's byte for byte, `mov eax, edx`
// before 0x100f70 included, 675 [86] (one instruction more, so the lockstep tail shifts); with the second mask loop's mask
// in its own long local (retail's one `movzx eax, word ptr`) 683 [84]; that pair with the default 1.0 in an else arm 552 [82]
// (1000 instructions as retail, but the loop pads move: the front is 24 bytes short before the first mask loop, so its
// alignment pad and the second loop's `jmp` over its pad differ); the weapon helper with the else-arm default only 909 [85].
// Diagnostic, restored: 0x24590 moved into a file that sorts after this one 396 (same code: not link order). Retail scan of
// all 25 call sites of 0x24590: none keeps an XMM value live across the call (0x1aa0d0's retail registers are those of the
// `inline` build), so retail treats it as clobbering everywhere. Lead (diagnostic, not kept): `inline` on 0x24590 plus the
// else-arm default, the weapon helper and the long mask local 76 [59] (fast); plus the havok pool data read first through
// `s_havok_component *const &` (the 0xb9b90 form) 74 [59] (the pool load then precedes the mask as retail; the object
// pointer and the pool global sit in ECX/EDX where retail has EDX/ECX). BLOCKED-BY 0x24590 (its six unmatched callers rise).
// Native angle members 396; one named conversion 396; typed input-pair pointer 957; complete input
// pair by value 949; value pair then snapshot then conversion 949; pair copied and converted in the
// angle workspace 949, with snapshot before conversion 949; typed pointer kept across snapshot 957.
// The pair and snapshot ordering reaches 3772 bytes, but the angle registers and downstream
// instruction schedule still differ. The XMM clobber after magnitude remains unresolved.
// N421 previews: typed seat table access at its existing definition field 396; separate unclamped fraction
// and angle response 823; complete response with unclamped fraction confined to the positive span 942;
// lower and upper velocity fields read through the bounds type 396; bounds view with a separately named
// interpolating fraction 823; upper velocity field retained as a distinct endpoint before the span 394;
// magnitude and scaled speed stated as separate computations 394; complete lower and upper clamp arms over
// the unclamped fraction 823.
// NOT kept: the typed seat-table field (396 plain, no gain; body kept stable for the held 0x24590 inline lead);
// naming the upper speed or magnitude gains only two points in preview, and the separate clamp forms regress.
// M3 lane 4 r222 (fast with 0x24590, 0xc8880, 0x185ab0, 0x185b00, 0x185b30, 0x187b30, 0x1a4900 matched and 0x186ab0 114
// throughout; control 396): the first difference is the angle conversion (retail: factor in XMM1, x in XMM7, y in XMM0, both
// stores after the record copy's `rep movsd`; ours: factor in XMM2, x in XMM0, y in XMM7, stores ahead of the copy). The
// record copy ahead of the two conversions 395 (the stores then follow the copy as retail's, the registers unchanged; a
// statement order picked by score, not kept); that with the factor first in each product 395; the factor first alone 396
// (same code); the factor as a file-level `static real const` for all four uses 396 (same code); the source row through one
// `real const *` for both components 957. The XMM numbering follows the clobber set of 0x24590 (its file is held this round):
// BLOCKED-BY 0x24590 still.
// M3 lane 13 r115 (fast, all restored): the range-fraction clamp as `if (v < 0) v = 0; else if (v > 1) v = 1;` 396 (same
// code); the fraction declared without a value and cleared in an else arm of the span test 942 (3740 B), with the
// if/else clamp too 942; the quotient clamped inside one PIN expression 932 (3791 B); the exponent named for the test
// and the power call 634.
// M3 lane 6 r205 (fast, 0x24590/0xc8880/0x185ab0/0x185b00/0x185b30/0x187b30/0x1a4900/0x186ab0 MATCH throughout; base 396
// [aligned 102]): KEPT, same code: the havok component through the existing havok_component_get in place of the offset
// written out. Not kept: `inline` redeclared for magnitude3d in this file only 396 (same code: the caller's declaration
// does not make 0x24590 opaque); the 0.64 sum written field_40 first 396 (same code); that sum through the existing
// dot_product2d over the stick pair 396 (same code); the scaled turn products written scale first 396 (same code); the
// parent object's havok index through object_header_get or object_get 396 (same code); the field_93 latch as
// `(...) ? 1 : 0` 390 [100] (retail's `mov eax, 1` / `xor eax, eax` before the byte store; a gain of 6, under the keep
// bar, so it waits for the 0x24590 fix). With that latch form: the weapon helper 672 [88], + the else-arm default 906,
// the else-arm default alone 913, the long mask local 676, + the else-arm default 847, weapon helper + long mask +
// else-arm default 545 [84] (552 without the latch form). What is left is the block after 0x24590 (retail remakes zero in
// XMM1/XMM6, does not cache the conversion constant and reloads the first angle limit into XMM7 only after the block),
// one scratch-rotation step at the parent object's lookup (retail EDX, ours ECX; it carries through both vector copies)
// and the loop pads, which follow the 27 bytes this body is short before the first mask loop.
// MiniMax B (11-19): no new form, no lower score (396): retail treats 0x24590 as clobbering the XMM registers, which is
// what fixes the whole block from the speed fraction to the stick clamps; the `inline` lead on 0x24590 is measured above
// (384 [65], one shift left) and is not kept because six of its other callers' counts rise - BLOCKED-BY 0x24590.
// M3 lane 6 r207 (fast with 0x186ab0, 0x187b30, 0x185b30 and 0x24590, all MATCH throughout; body unchanged): the angle
// pair scaled through the existing planar_scale helper ahead of the record copy 957 (same registers, one `add` more),
// after the copy 415; the second hold time as `arg_2 + field_7c` or `field_7c + arg_2`, or both hold times argument-first
// 396 (same code: retail loads the field first, ours the argument); the second hold time accumulated ahead of its rate
// scale 402; the parent object through a named pointer, through the shared header view, or the unit's object named 396
// (same code: the scratch rotation stays one step off at the parent lookup). Still BLOCKED-BY 0x24590 (its file was on
// the avoid list again).
// M3 lane 11 r133 (fast with 0x24590, 0xc8880, 0x185ab0, 0x185b00, 0x185b30, 0x187b30, 0x1a4900 and 0x186ab0, all MATCH
// throughout; control 396, aligned 138, register-masked 50; body unchanged). Held state (diagnostic `inline` on 0x24590's
// definition, saved with the forms below as build/r133/held-0x185be0.patch): this body 836 (3772 B, aligned 101, masked 21);
// with the else-arm default, the unit_get_current_weapon helper and the long mask local 76 (lane 15 r98's lead, 3774 B);
// with the field_93 latch as `? 1 : 0` too 60 (retail's 3777 B, masked 12); with the input-record copy ahead of the two
// angle conversions 57; with the else arm's two assignments in the order local_17, local_16 (retail loads +0x1c first) 55;
// with the havok components bound by `s_havok_component *const &` to the pool's data (0xb8ca0's form: the pool load then
// precedes the index scaling) 53 (aligned 75, masked 6). On the 55 form: the 0.64 sum written field_40 first 55 (same code);
// the quotient written out at both uses in place of local_18 552 (3768 B); local_18 declared after local_19 and local_20
// 77 (aligned 64: the quotient then loads late and other registers move); the second hold time as `arg_2 + field_7c` or
// `field_7c + arg_2` 55 (same code). Left at 53: retail adds the argument to field_7c field first (ours argument first, though
// field_78 is field first in both); the quotient in XMM3 and 1.0 in XMM4 (ours the other way round, 18 lines); the parent
// object's pointer in EDX, reusing the header table's register, where ours takes ECX (one scratch step that carries through
// the pool load and both vector copies); the 0.64 test squares field_44 first.
// On main's state (no `inline`): the else-arm order 394; the pool reference 394; both 392; with the latch form 386; with the
// record copy first too 385; with the else-arm default, weapon helper and long mask local as well 547. Not kept: the
// statement orders and the reference are match-only forms and the latch form alone is 390. Still BLOCKED-BY 0x24590.
// M3 lane 11 r134 (fast with 0x24590, 0xc8880, 0x185ab0, 0x185b00, 0x185b30, 0x187b30, 0x1a4900 and 0x186ab0, all MATCH;
// plain where noted; body unchanged at 838, aligned 103): r133's lead rebuilt on today's main (0x24590 now inline) 53 fast,
// 55 plain (the full image adds one tie: after the pow call retail loads 1.0 into XMM4 before zeroing XMM6, ours the other
// way). On that lead: the parent object's havok index read through object_get 53, through object_header_get(...)->object
// 53, the parent read through object_get as well 53 (same code each); havok_component_get in place of the pool reference
// with object_get 55; 1..7 dead initialised locals at the top 53/55/55/55/55/55/53 (diagnostic: only the post-pow pair
// moves), 1..4 before the quotient 53, 1..3 before the havok index 53 (no symbol-count tie); the two angle products
// clamped where they are formed (`PIN(local_15.i * local_18, ...)`) 64; the second hold-time add ahead of its scale 59.
// On main's body: unit_get_current_weapon alone 840 (3774 B, aligned 88); a long field_c mask local 837, as a word 836
// (aligned 100/101). On the lead: the 0.64 sum's two squares parenthesized, either order, 53/53, the second hold-time add
// as `field_7c = (field_7c + arg_2)` 53 (same code each). Left on the lead: the second hold-time add's operand order, the quotient/1.0 pair in XMM4/XMM3 for
// retail's XMM3/XMM4, one scratch step at the parent lookup (retail EDX, ours ECX, through the pool load and both vector
// copies), the word mask's zero-extension register (EDX, ours EAX) and store order, and the 0.64 sum's square order.
// Lead saved as build/r134/lead-0x185be0.patch (match-only forms: record copy first, else-arm default, swapped else
// assignments, the pool reference, the latch spelling).
// M3 lane 14 r128 (0x24590 inline on main; fast with 0x24590, 0x185b30, 0x186ab0 and 0x187b30, all MATCH; this body
// builds differently in the fast image, so kept forms are plain-confirmed; start 838 plain, 836 fast). KEPT: the 1.0
// default of the rescale factor in an else arm (retail copies 1.0 only on the two failure edges) 382 fast alone; the
// existing unit_get_current_weapon for the hand-written lookup 840 fast alone, 79 with the else arm; a separate long
// local for the field_c mask (the word mask local is no longer reused) 837 alone, 379 with the else arm; all three 76
// fast, 78 plain; the field_c mask read from the field in the loop instead of any local 60 fast on the 60 form (the
// reused word local 63; kept: the field read); with the latch as `? 1 : 0` (retail's full-width `mov eax, 1` /
// `xor eax, eax`) 60 fast, 62 plain at
// 3777 bytes. On 62 (fast 60): local_20 computed before local_19 611; the quotient computed after local_19 49 (1.0 then
// takes XMM4 and the quotient XMM3 as retail, but the quotient is scheduled after the first clamp); the record copy
// first 73; the else arm's loads swapped 74; both 55; those two with the quotient after local_19 44; and the pool
// through a const reference 42 (all match-only orders and forms).
// M3 lane 14 r131 (fast control 60, plain 62; fast CAVEATs this function): the input-record copy ahead of the two
// angle conversions 57 fast; the else arm storing local_17 before local_16 58 fast; the quotient after local_19 49
// fast (retail's XMM3/XMM4, scheduled after the first clamp), after local_20 82, as an expression at both uses 554
// (3768 B); `local_18 * i` 60 and `k * PIN(...)` 60 (same code); the 0.64 squares written field_40 first 60 (same
// code); the parent's component index through object_header_get, havok_object_get or object_get, a named object or a
// named parent pointer 60 (same code each: the parent lookup's scratch rotation, ours ECX where retail reuses EDX,
// does not move); the second hold-time add as `field = field + arg_2` or `arg_2 + field` 60 (same code). Lead
// (statement order only, match-only): copy first + else-arm order + quotient after local_19 46 plain (44 fast);
// left there the 1.0/zero remake after pow, the second hold-time add, the quotient's early schedule, the parent
// lookup's rotation (through the flag loop's mask) and the 0.64 squares.
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
                    {
                        local_16 = (magnitude3d((vector3f *)(local_11 + 0x88)) - local_14) / local_15;
                        local_16 = PIN(local_16, 0.0f, 1.0f);
                    }
                    if (*(real *)(local_13 + 0x5c) != 0.0f)
                        local_16 = (real)pow(local_16, *(real *)(local_13 + 0x5c));
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
            real local_10;
            if (local_8 > 0.1f && local_9 > 0.1f)
            {
                if (local_8 > local_9) { local_9 /= local_8; local_8 = 1.0f; }
                else { local_8 /= local_9; local_9 = 1.0f; }
                local_10 = (real)sqrt(local_9 * local_9 + local_8 * local_8);
            }
            else local_10 = 1.0f;
            real local_11 = PIN(local_10 * local_2.field_40, -1.0f, 1.0f);
            real local_12 = PIN(local_10 * local_2.field_44, -1.0f, 1.0f);
            s_185be7 local_13;
            local_13.field_0[0] = function_185b30(*(short *)(local_0 + 0x74), *(real **)(local_0 + 0x78), local_11) * local_5.field_0[0];
            local_13.field_0[1] = function_185b30(*(short *)(local_0 + 0x74), *(real **)(local_0 + 0x78), local_12) * local_5.field_0[1];
            if (fabs(local_11) >= *(real *)(local_0 + 0x48))
            {
                real local_15 = PIN(local_1->field_78 / *(real *)(local_0 + 0x4c), 0.0f, 1.0f);
                local_13.field_0[0] *= (*(real *)(local_0 + 0x50) - 1.0f) * local_15 + 1.0f;
                local_1->field_78 += arg_2;
            }
            else local_1->field_78 = 0.0f;
            if (fabs(local_12) >= *(real *)(local_0 + 0x48))
            {
                real local_15 = PIN(local_1->field_7c / *(real *)(local_0 + 0x54), 0.0f, 1.0f);
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
                real local_19 = 1.0f - PIN(*(real *)local_0, 0.0f, 1.0f) * local_15.k;
                real local_20 = PIN(*(real *)(local_0 + 4), 0.0f, 1.0f) * local_15.k;
                real local_21 = local_15.i * local_18;
                real local_22 = local_15.j * local_18;
                local_21 = PIN(local_21, -3.1415927410125732f, 3.1415927410125732f);
                local_22 = PIN(local_22, -1.5707963705062866f, 1.5707963705062866f);
                local_16 = local_21 * local_20 + local_19 * local_13.field_0[0];
                local_17 = local_22 * local_20 + local_19 * local_13.field_0[1];
            }
            else
            {
                local_16 = local_13.field_0[0];
                local_17 = local_13.field_0[1];
            }
            arg_5->field_10 = local_16 * arg_2;
            arg_5->field_14 = local_17 * arg_2;
            *(real *)((byte *)arg_5 + 0x24) = local_2.field_48;
            if (local_1->field_0 != NONE)
            {
                s_object_header_view *header = object_header_get(local_1->field_0);
                if ((1 << header->type) & 1)
                {
                    long local_18 = *(long *)(header->object + 0x3e4);
                    if (local_18 != NONE)
                    {
                        long local_19 = *(long *)(function_185be2(local_18) + 0xb4);
                        if (local_19 != NONE)
                        {
                            s_havok_component *local_20 = havok_component_get(local_19);
                            long local_24 = function_0b67a0((s_small_index *)local_20);
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
        for (long local_24 = 0; local_24 < 16; local_24++)
        {
            if (!(local_1->field_c & (1 << local_24)))
            {
                local_3[local_24] = local_2.field_0[local_24];
                local_4[local_24] = local_2.field_10[local_24];
            }
        }
        bool local_9 = function_185be3(local_1->field_0) && function_e4050(local_1->field_0);
        if (local_3[10] == 1)
        {
            if (local_9 || arg_5->field_4 * arg_5->field_4 + arg_5->field_0 * arg_5->field_0 < 0.25f) local_1->field_88 = 1;
        }
        else if (!local_3[10]) local_1->field_88 = 0;
        SET_FLAG(arg_5->field_18, 0, local_1->field_88 != 0);
        bool local_10 = local_3[11] == 1;
        if (local_10 && local_1->field_2e == NONE && arg_4->field_0)
        {
            long local_11;
            if (local_1->field_0 != NONE) local_11 = unit_get_current_weapon(local_1->field_0);
            else local_11 = NONE;
            if (((byte *)arg_4->field_0)[6] > 0x7f || ((byte *)arg_4->field_0)[7] > 0x7f) local_10 = false;
            if (local_11 != NONE && !function_100f70(local_11) && local_2.field_44 * local_2.field_44 + local_2.field_40 * local_2.field_40 > 0.64000004529953f) local_10 = false;
        }
        SET_FLAG(arg_5->field_1c, 2, local_10);
        SET_FLAG(arg_5->field_1c, 3, local_3[11] != 0);
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
        long local_11 = local_3[local_2.field_4c];
        byte *local_12 = &local_3[local_2.field_4c];
        if (local_1->field_0 != NONE && *(char *)(function_185be2(local_1->field_0) + 0x212) != NONE && *(char *)(function_185be2(local_1->field_0) + 0x213) != NONE)
        {
            *local_12 = 0;
            local_1->field_90 = local_11 > 0;
        }
        else if (local_1->field_90)
        {
            if (local_11 > 0) { local_11 = 0; *local_12 = 0; }
            else local_1->field_90 = 0;
        }
        SET_FLAG(arg_5->field_18, 16, local_3[7] != 0);
        SET_FLAG(arg_5->field_18, 21, local_11 != 0);
        SET_FLAG(arg_5->field_18, 17, local_11 != 0);
        if (g_4e6948->state != 2) SET_FLAG(arg_5->field_18, 29, local_3[local_2.field_4d] != 0);
        SET_FLAG(arg_5->field_18, 11, local_11 != 0);
        SET_FLAG(arg_5->field_18, 14, local_11 != 0);
        SET_FLAG(arg_5->field_18, 26, local_3[6] != 0);
        SET_FLAG(arg_5->field_18, 7, local_3[12] != 0);
        SET_FLAG(arg_5->field_18, 8, local_3[13] != 0);
        SET_FLAG(arg_5->field_18, 2, local_3[5] != 0);
        SET_FLAG(arg_5->field_18, 20, local_3[5] != 0);
        SET_FLAG(arg_5->field_18, 25, local_3[5] != 0);
        local_1->field_93 = (local_3[0] && ((g_510c50 && ((byte *)g_510c50)[5]) || local_1->field_93)) ? 1 : 0;
        SET_FLAG(arg_5->field_18, 1, local_3[0] && !local_1->field_93);
        SET_FLAG(arg_5->field_18, 4, local_3[4] != 0);
        SET_FLAG(arg_5->field_18, 5, local_3[4] != 0);
        SET_FLAG(arg_5->field_18, 6, local_3[7] != 0);
        SET_FLAG(arg_5->field_1c, 1, local_3[1] == 1);
        SET_FLAG(arg_5->field_1c, 4, local_3[14] != 0);
        SET_FLAG(arg_5->field_1c, 5, local_3[15] != 0);
        SET_FLAG(arg_5->field_20, 8, local_3[14] == 1);
        s_input_state *local_14 = NULL;
        if (g_4e61cc[(short)arg_0]) local_14 = g_4e61b9 ? &g_4e630c : &g_4e61dc[(short)arg_0];
        if (local_14 && ((byte *)local_14)[0x19]) arg_5->field_1c |= 0x40;
        long local_15 = real_truncate(*(real *)(*(byte **)((byte *)g_4e034c + 0xf4) + 0x7c) * 1000.0f);
        SET_FLAG(arg_5->field_20, 0, local_3[2] > 0);
        SET_FLAG(arg_5->field_20, 1, local_4[2] >= local_15);
        SET_FLAG(arg_5->field_20, 4, local_3[3] > 0);
        SET_FLAG(arg_5->field_20, 5, local_4[3] >= local_15);
        if (local_3[3] > 0) local_1->field_8e = 1;
        else
        {
            if (local_1->field_8e && !local_1->field_8f) arg_5->field_1c |= 1;
            local_1->field_8e = 0;
            local_1->field_8f = 0;
        }
    }
    player_control_update_action_flags((s_player_action *)arg_5, (s_player_action_triggers *)&local_2);
}
