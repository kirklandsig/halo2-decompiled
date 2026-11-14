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

#define FLAG(bit) (1 << (bit))
#define SET_FLAG(flags, bit, value) ((value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)))
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
// score 590 (was 590; earlier 622, the lane's mixed form 516): 3773/3777 B, masked aligned about 157 (stack offsets and registers masked; 233 at 622). Round 56 (m3 lane 15, plain): PIN on
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
        {
            if (!(local_8 & (1 << local_24)))
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
        function_185be4(&arg_5->field_18, 1, local_1->field_88 != 0);
        bool local_10 = local_3[11] == 1;
        if (local_10 && local_1->field_2e == NONE && arg_4->field_0)
        {
            long local_11;
            if (local_1->field_0 != NONE)
            {
                byte *local_12 = function_185be2(local_1->field_0);
                short local_13 = *(char *)(local_12 + 0x212);
                local_11 = NONE;
                if (local_13 != NONE) local_11 = *(long *)(local_12 + local_13 * 4 + 0x218);
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
        long local_11 = local_3[local_2.field_4c];
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
        SET_FLAG(arg_5->field_20, 8, local_3[14] == 1);
        s_input_state *local_14 = NULL;
        if (g_4e61cc[(short)arg_0]) local_14 = g_4e61b9 ? &g_4e630c : &g_4e61dc[(short)arg_0];
        if (local_14 && ((byte *)local_14)[0x19]) *(dword *)&arg_5->field_1c |= 0x40;
        long local_15 = (long)(*(real *)(*(byte **)((byte *)g_4e034c + 0xf4) + 0x7c) * 1000.0f);
        SET_FLAG(arg_5->field_20, 0, local_3[2] > 0);
        SET_FLAG(arg_5->field_20, 1, local_4[2] >= local_15);
        bool input_3_active = local_3[3] > 0;
        SET_FLAG(arg_5->field_20, 4, input_3_active);
        SET_FLAG(arg_5->field_20, 5, local_4[3] >= local_15);
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
