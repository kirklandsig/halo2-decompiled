#include "unknown_11c920.h"
#include "sound_driver.h"
#include "unknown_21e230.h"
#include <math.h>
#include <stdlib.h>
#include <stddef.h>

// @flags /O2 /arch:SSE /Op /Gr

struct s_looping_channel_properties;
struct s_looping_effect_playback;
struct s_sound_channel_parameters;
struct s_mixbin_list { long count; DSMIXBINVOLUMEPAIR pairs[8]; };
struct s_channel_update_input
{
	dword field_0;
	long format;
	real pitch, gain;
	byte field_10[4];
	real field_14, field_18, field_1c, field_20;
	short voice_index;
	byte field_26[2];
	real field_28;
	long field_2c;
	dword field_30;
	byte field_34[4];
	real levels[4];
};
struct s_channel_send_state
{
	dword flags;
	real level, level_offset, gain0c, gain10, field_14;
	real left_gain, right_gain, rear_left_gain, rear_right_gain;
};
class c_channel_effects_view
{
public:
	virtual void initialize(void *, void *, bool)=0;
	virtual void dispose()=0;
	virtual void set_i3dl2(void *, long)=0;
	virtual void add_effect_sends(s_channel_send_state *, bool, long, s_mixbin_settings *)=0;
	virtual void add_send(long, long, real, s_mixbin_settings *)=0;
	virtual void add_source_mixbins(s_channel_update_input const *, bool, long, s_mixbin_settings *)=0;
	virtual void add_sends(s_channel_send_state *, long, long, s_mixbin_settings *)=0;
};

typedef char check_channel_update_levels[offsetof(s_channel_update_input, levels)==0x38?1:-1];
void function_2215d0(s_mixbin_list *, s_mixbin_settings *, LPDIRECTSOUNDSTREAM);
void function_2216f0(s_mixbin_list *, s_mixbin_settings *, LPDIRECTSOUNDBUFFER);
void function_2201f0(long, s_sound_channel_parameters const *, bool);
real const g_44a23c=-64.0f;

PRIVATE inline real channel_pin_gain(real value)
{
	return value < -64.0f ? -64.0f : value > 0.0f ? 0.0f : value;
}
PRIVATE inline long channel_pin_gain_bits(long bits)
{
	long result;
	if(*(real *)&bits < -64.0f) result=0xc2800000;
	else if(*(real *)&bits > 0.0f) result=0;
	else result=bits;
	return result;
}
PRIVATE inline real channel_linear_gain(real value)
{
	long pinned=channel_pin_gain_bits(*(long *)&value);
	real scaled=*(real *)&pinned*0.05f;
	return (real)exp(scaled*2.3025851f);
}
PRIVATE inline c_channel_effects_view *channel_effects()
{
	return (c_channel_effects_view *)SOUND_DRIVER_GLOBALS->effects;
}

// MiniMax B (11-13): no new form, no lower score (544); the permuter refuses the reduced image for this function, and the full-image search over the declaration and statement forms found nothing better.
// score 544 (was 545): full build; b2D 11-06 statement walk against retail found no logic difference
// (every branch, field offset, callee and argument agrees). Precision fix kept: log10 taken of a double (fyl2x then fmul 20 unrounded, 0x21fe2f-0x21fe37).
// MiniMax A (11-12): no new form, no lower score (544); the file's fixed /Op still decides the mixed
// arithmetic: the gain difference is computed on the x87 stack where retail subtracts in SSE and takes the
// absolute value afterwards, our frame is still 0x22c against retail's 0x224, and we still push EBP before
// the stream pointer is formed where retail forms the pointer first and pushes EBP after it; the names and
// the declaration orders the notes above list all give the same code.
// MiniMax b (11-11): no new form, no lower score (544); our frame is still 0x22c against retail's 0x224, we push
// EBP before the stream pointer is formed (retail forms the pointer first and pushes EBP after it), and the gain
// difference is computed on the x87 stack where retail subtracts in SSE and only then takes the absolute value.
// Found, not kept: retail also sums the level energy unrounded on the x87 stack (faddp at 0x21fde3, fsqrt at 0x21fde9). A double `energy`
// scores 530 here, but in the full build it loses the matched 0x21f8a0, 0x120d50, 0x120d80, 0x121350, 0x121a40, 0x121fc0, 0x125b40 and
// 0x1293f0 (and 0x128ca0 222 -> 293, 0x21c400 339 -> 526), so the energy stays a float.
// Not kept: retail also never rounds the exp result or `scaled` (0x21fdb9-0x21fddd): a double amplitude 574; with a double `scaled` too 528 but 21f8a0 0 -> 67 and callers 293/526;
// `fabs((double)a - b)` on the three field differences puts a CRT fabs call in, 539. Root cause is the file's /Op (retail rounds nothing). Frame 0x22c versus 0x224; stream/gain homes and mixed math remain (masked aligned 233 of 565 lines, the most of this queue).
// MiniMax E (11-05): no new form, no lower score: the named gain difference 537 fast (542 plain, already recorded), the double-cast differences 539 fast and 551 plain, the gain declarations moved ahead of changed 573 and no /Op 723; retail's frame 0x224 and its stream in EBX with the gain in EBP stay unmatched; the permuter was stopped before it printed a result.
// Round 64 (fast control 532, full 545; masked aligned in brackets, control [240]): send bits through a helper with a named bool result 532 [243] (no dead byte: the store
// needs the flags in memory); mode sends as a fall-through two-case switch 545 at 1901 bytes [244], with the helper [247], with the gain difference named [240]; gain
// difference named alone 537 [244], cast to real 532 (same code); direct as a bool cast 532 (same code); `changed` default-true 544 [246], default-false 543 [244], default-true with the
// switch 550 [236]; gain pointer chosen by an if 532 (same code); stream through pointer arithmetic 532 (same code). Retail keeps one stack byte for every send bit and reloads
// the flags after each store; only a volatile bool reproduces that shape, and it stays out. Flag lead unchanged: the /Op math is the file's largest difference and flags are not ours to change.
// Earlier: this round found no gain - the frame is 0x22c against retail's 0x224 and the
// stream and gain homes still differ; the permuter needs a full build per trial and ran past the
// per-function time budget, so it was stopped without a result.
// Earlier first line: frame 0x22c versus 0x224; stream/gain homes and mixed math remain.
// the level-energy block summed in double (the gain converter returning double) 528, but the
// shared converter costs the matched 0x21f8a0 (0 -> 67): restored (MiniMax E run 12).
// frame 0x22c versus 0x224; stream/gain homes and math expansion differ.
// native input direct bit 541; all native input bits 548; captured direct use 540;
// shared gain converter 629; inline depth one 541; native output bits with captured direct 544.
// frame 0x22c versus 0x224; stream and gain homes plus SSE and x87 scheduling differ; native-bit tests gain only three
// tried initial driver value 541; native send bit writes 567; native send bit tests 538; native pitch truncation 551; level pointer traversal 558; real absolute values 541; native flags real abs 538; native flags driver 538.
// Frame 0x22c versus 0x224 and stream/gain homes remain. Redundant level clamp removed: 575 to 545, 1901/1901 bytes. Voice output reuse 575; query output scope 572; complete flag projection 572; output stream observation 614 (plain).
// Voice selector snapshot 572 fast; level-offset snapshot 557 fast; entry status,
// initial voice output and initial settings 532 fast each (unchanged preview 532).
// Real gain value 545; native linear return 545; unsigned pin bits 545; exponential input 561;
// native return/input 561; logarithm/root results 551/549; combined math results 554, frame 0x230.
// Original body and helper contracts restored; frame 0x22c and stream/gain homes remain.
// Native gain read/routing cache/all reads 532/532/532 fast (control 532); native gain/all publication 540/540; common status/buffer volume/stream volume 549/543/566. Full baseline remains 545.
// Linear input pointer 544; native speaker array 545; array plus input pointer 544; known-clear
// send bits 547; send field order 545; routing flag publication last 545; cache from produced send
// fields 545; direct output level 545. One-point reductions dropped; all body/type/helper forms
// restored. Frame 0x22c versus 0x224 and stream/gain/math homes remain.
// Native sixteen-byte speaker copy 529 fast; distinct scalar math names 532; early selected gain
// 577; copy/names 529; copy/early gain 561; names/early gain 577; all three 561;
// send-scope zero energy 533 (reduced control 532). Three-point reductions rejected.
// Round 127: changed/both word predicates 556/574; complete input record 620 (sibling lost); pin
// default-input/early returns 534/537; default level/bounded amplitude 543/530; combined default
// producers 552 (fast control 532). Every source form restored.
// Round 171 reduced control 532: live direct field 555; live mode field 588; shared voice output 532; live
// direct/mode 579; live direct/shared output 555; live mode/shared output 588; live fields/shared output 579;
// shared voice cursor 532.
// Round 213 reduced control 532: playback state-transition gate with bool/byte/word/long
// request domains 549/549/550/550; each with status declaration at query 549/549/550/550.
// Original playback arms, query schedule and all math restored.
// Round 377: Complete cache-valid bool/byte/short/long result graphs
// 542/542/569/569; entry declaration forms 542/542/569/569 (fast control 532,
// full baseline 545). Byte forms recover checker length but regress; all three
// consumers, original status, mixed math and helper contracts restored.
// Force-inline gain pin, bit pin, linear gain and combinations, then all four
// existing helpers: eight forms all 532 on the reduced control. Helpers restored.
// Round 227: Double energy/sample, named complete square, their pairs/all,
// all plus rounded root input 530/532/538/576/531/560/561/566 (precision exact-only). Restored.
// Existing mixbin/final-update inline-noinline declaration contracts, all seven subsets,
// and the complete inline-only graph 532 each (reduced control 532; full baseline 545).
// Every declaration and source body restored; frame and math expansion remain.
// Round 367: Native real exponential/root/logarithm APIs, all seven subsets and the full graph with a
// native gain value: eight forms all 532 (fast control 532; full baseline 545). Original body and helpers
// restored.
// Round 237: one driver/at-use channel/one effects receiver 567/641/539;
// driver-channel/driver-effects/channel-effects/all 567/555/623/555; all with mode interval 556
// (fast control 532; full 545). All helpers and body restored; frame and stream/gain homes remain.
// Round 381: Native integer-bit linear inputs, signed/unsigned and ordinary/forced expansion, copied
// speakers 542 each; original input-array speakers 551 each (fast control 532, full 545). All
// input/helper forms restored; frame and gain homes remain.
// Round 242: Native double absolute-value API across gain/routing/voice gates,
// seven complete subsets and all with real gain value [547, 534, 532, 539, 543, 535, 537, 537].
// Reduced control 532, full 545; all width forms and original helpers restored.
// Round 280: Byte/char/word/long force inputs with signed/unsigned channel indices all 532 fast (control
// 532, full 545); matched sibling preserved. Original signature, both forward declarations, body and
// mixed-math helpers restored.
// Round 396: Pitch bound-test result with long/short offset and switch/if dispatch,
// late destinations 532/535/531/534; entry uninitialized destinations 532/535/531/534
// (fast control 532, full 545). One-point reduction dropped; all original body/helpers restored.
// Round 290: Existing linear/bit-pin/real-pin input reference subsets 542/532/536/532/542/532/536;
// all with native gain value 536 (fast control 532, full 545). Original executable source
// restored.
// Round 218: Complete speaker library copy with seven native voice-output entry-scope subsets, then
// all three role names, 529 each (reduced control 532, full 545). Three-count reductions rejected;
// all native output scopes, copy and body restored.
// Round 197: Complete computed real gain/routing/pitch subexpressions, masks 1..7,
// 542/547/554/545/542/547/554; all with native gain value 554.
// Native send word stores/tests, stores, stores/tests with routing, tests on gain graph
// 545/538/562/550; complete library speaker copy with store-only/both views, then native
// gain values, 566/557/566/557. Three- and seven-point gains rejected; original body restored.
// DL204: staged routing-result bool/byte/short/long 532 each; guarded interval and
// conditional-absolute delta forms 589/544/589/544 (reduced control 532, full 545). Restored.
// Lane 10 round 34: retail sends the voice mixbins through a two-case switch on the channel mode (`sub eax, 0;
// je; dec eax; jne`, case 1 with two calls whose second call is cross-jumped with case 0's one call), where our
// counted loop gives a countdown loop. Switch cases 0 then 1 (or 1 then 0) with their own calls: retail's block
// and branch shape, fast 532 (aligned difference 462 -> 434), plain 568 at 1879 bytes (aligned 481 -> 441;
// the positional score rises with the shorter body). Fallthrough case 1 into case 0 545 fast; a plain real gain
// selected through the pointer 566 fast, with the fallthrough switch 559.
// Round 217: Complete active mixing-workspace aggregate zeros, seven subsets, then all with the
// unused workspace zeroed: 605/597/640/585/619/619/651/665. Original native workspace initialization restored.
// Round 54 (m3 lane 1; fast control 532, full 545): retail rounds nothing in its fabs tests: the gain difference
// is an SSE subss stored once and loaded for an x87 fabs, the three field differences are x87 `fld; fld; fsub
// st(1); fabs` and the two single fields go through a movss copy, each fabs result compared unrounded; ours (the
// float fabs overload under /Op) stores and reloads after every subtraction and fabs. Lead for the user (flags not
// changed): retail's 0x21fa80 looks built without /Op; `#pragma optimize("p", off)` around the body leaves the
// command-line /Op in force (545, identical code, the one compiler-setting diagnostic). Forms: `fabs((double)...)`
// at all six tests 537 fast (/Op turns the double fabs into a CRT call); mode and direct read before the gain bits
// 565 (plain, 1920 bytes); the send bits and the stream's 0x20 bit through bit_vector.h's bit_vector_set and
// byte_flags_set 532 fast (identical code: the flags stay in a register, retail updates them in memory and stores
// each bit's bool to one byte slot); changed computed after the gain bits 577 fast; direct declared just before
// the send block 545 fast.
// Round 479: shared native active-mixbin workspace for first/submix, first/routing,
// submix/routing and all phases, declared before updates 535/542/532/542;
// at entry 535/542/532/542 (fast control 532). Original workspaces and body restored.
// Round 66 (lane 9, fast control 532, full 545): retail's x87 difference compares never round (no fstp/fld after
// the subtraction or after fabs) and load the stored value first (`fld stored; fld input; fsub st(1); fabs;
// fstp st(1)`); the two magnitude compares bounce the input through xmm0. Ours (float fabs under /Op) rounds twice.
// Tried: `fabs((double)(difference))` 537 (fabs becomes a CRT call under /Op); `fabs((double)a - (double)b)` on the
// four differences 534; `fabsf` on all six 532; an inline previous/current compare helper on all six 617, on the
// four differences 585 (one call stays out of line). Diagnostic only (flags are the user's call): the file without
// /Op 723 (1728 bytes: inline fabs without rounding, but `fld input; fsub [stored]` and no xmm bounce).
// Round 25 (lane 18): the voice sends as `switch (mode) { case 1: add; case 0: add; }` (case 1 falling into
// case 0) 542 plain (not kept, three under the bar); the gain compare with a `(real)` cast 545 (same code); with
// the double fabs called directly, a CRT call (547 fast). Retail writes every flag-bit test (shr/and r8, 1) into
// one stack byte (+0xe, shared with the direct flag) and keeps 0 in EBP for the voice branch's zero arguments;
// a shared function-scope bool 549 fast and a set helper with a bool parameter 545 fast both stay in registers.
// Round 277: standard selected-gain/real-pin/bit-pin extrema, subsets 1..7,
// 570/564/561/536/556/558/578; all with real gain value 578 (fast control 532,
// full 545). Original body and helpers restored; frame and stream/gain homes remain.
// Round N284: Energy-loop dword/short/word/byte indices with <4 and !=4 bounds
// 532/563/563/563 and 532/563/563/563 (reduced control 532, full 545). Exact sibling
// preserved; original four-sample loop, all real roundings and complete body restored.
// Round 479: shared native active-mixbin workspace for first/submix, first/routing,
// submix/routing and all phases, declared before updates 535/542/532/542;
// at entry 535/542/532/542 (fast control 532). Original workspaces and body restored.
// DL225: separate four-speaker conversion/reduction using amplitude or square samples,
// energy zero before/after conversion and storage at producer/entry: 604/599/604/599/
// 604/599/604/599 (reduced control 532, full baseline 545). All sample storage restored;
// frame 0x22c versus 0x224 and stream/gain/mixed-math homes remain.
// DL228: branch-native status destinations, distinct active mixbin/voice names, seven subsets,
// then lexical query-record ordering: eight at 532 (reduced control 532; full 545).
// Original native workspaces, receivers and body restored; frame and gain homes remain.
// Branch-native status plus native send stores/tests/both, without/with complete speaker copy:
// 541/551/552/567/535/545 (reduced control 532). The changes share frame 0x228;
// original send word, status scopes, copies and complete body restored.
// DL230: complete gain-update bool/byte/short/long and pitch-update bool/byte/short/long decisions: 532/532/532/532/532/532/532/532.
// Reduced control 532, full 545; original gate expressions and body restored.
// Round 29 (lane 17, fast; control 532, full 545; aligned common 221, register-masked common 392 of 565). Retail stores a dead
// {0, &pairs} pair right before SetMixBins (three instructions our body lacks): an initialized unused s_mixbin_settings there
// 557 (1930 bytes; masked 395, frame unchanged), before add_source_mixbins 557 (1930; masked 392), as separate fields 532 and
// through a forceinline pointer helper 532 (stores dropped). Mode and direct read before the gain 552 (1901 bytes; stream in EBX
// and gain in EBP as retail, aligned common 243, but they then schedule ahead of the gain test); direct then mode 541 (common
// 235, masked 397); declaration-only orders of mode/direct/gain 532 (same code); gain through a pointer local 532; gain as
// s_decibels 532 (same code). `#pragma intrinsic(fabs)` alone 532; with double fabs at all six tests 653 (1793 bytes; inline
// unrounded, but the constant loads as a qword), the four differences 601 (1841), the two fields 584 (1857), the gain alone
// 543 (1901). Combined: two-case switch 532 (masked 402), with the dead pair 557 (masked 405), with mode/direct first 548 (frame
// 0x228, masked 378), all three 568 (0x228, masked 379); dead pair with mode/direct first 577. Retail's stack-byte bit tests
// match the `volatile bool` shape of 0x21ec00, which this round may not add.
// DL236: Actual gain/submix/routing initializer phases moved before queries/send construction, subsets 1..7, then all without the unused private workspace: 547/546/548/547/549/546/548/572 (reduced control equals full 545). All restored; frame 0x22c versus 0x224, stream/gain homes and mixed math remain; matched sibling preserved.
// N323: Existing decibel value types for five send gains/four speaker gains/virtual send gain, seven subsets then native selected gain: 542/542/578/542/542/573/542/542. Original record, interface and full body restored; frame/stream/gain and mixed math remain.
// P284: Complete gain-phase scope ending at cache publication, mode/direct storage
// before the phase or at entry, each original reference/plain gain value: eight at
// 532 fast (control 532; full 545). Original body and lifetimes restored;
// frame 0x22c versus 0x224 and stream/gain/mixed-math homes remain.
// Round 128 (lane 8, plain; the fast image is unfaithful here): the gain difference named before the gain test
// (`real gain_change = gain - stored;`) 542 (retail's SSE subss stored once for the x87 fabs, but computed ahead
// of the changed test where retail computes it after); the difference cast to real inside the test 545 (same
// code); mode/direct first with the named difference 546; the named difference with a real gain value 542;
// fabsf on the gain test alone 545.
// Round 42 (m3 lane 2, plain; fast builds this function differently, CAVEAT): the gain difference named
// (`real gain_change = gain - stored;` tested as `fabs(gain_change)`) 542: retail's SSE `subss` and store appear
// (hoisted above the `changed` test, where retail computes it after) but the /Op float fabs still rounds; that
// with mode/direct read before the gain 546, with a value (not reference) gain 542, with `0.1f > fabs(...)` 542, with
// the two-case mode switch for the voice sends 541 (1902 bytes). Original body restored.
// Round 81 (m3 lane 12, plain unless noted): lane 18's fall-through switch for the mode sends rebuilt 542 (still
// three under the bar); with one settings record for the voice branch 559 fast; direct and mode before the gain
// 552 fast; `switch (mode) { case 0: add; break; case 1: add; add; break; }` 568 (1879 bytes, not tail-merged);
// the voice pointer formed before the stream's voice-properties query 573 (1877). Original body restored.
// Round 79 (m3 lane 16, fast control 532, full 545): the gain difference in a named real before its fabs 537;
// direct read before the stream lookup 579 (1921 bytes); mode read before changed 570 (1917 bytes); the voice
// branch's status declared in each arm 532 (same code); buffer/submix property declarations swapped 532 (same code).
// Round 144 (m3 lane 5, plain; retail's six difference tests never round: `fld stored; fld input; fsub st(1); fabs;
// fstp st(1)`, the /Op lead above): mixbin-branch send tests as `(bool)(flags >> n & 1)` 546; one twice-used
// voice-output test for both branches 577 (1877 bytes); the voice mixbin gain through the gain reference 545 (same
// code); the three field differences as stored minus input 545 (same code); direct read right after changed, mode
// after the gain 546. Original body restored.
// Round 49 (m3 lane 15, plain; fast has a CAVEAT here): the gain test through a `gain_changed` bool set from a
// block-scoped named difference when not `changed` 545 (same code: still x87 fld/fsub, retail's SSE subss needs the
// gain loaded with movss); that with a plain real gain value 545 (same code); the stream's 0x20 bit updated before
// the send flags 576 (1863 bytes); the pitch delta compared without the `(real)` cast 545; the send flag set
// before the two cached level stores 545.
// M360: Extended scaled-value/linear-result producer with sample/energy/root
// consumer widths, masks 0..7: 558/558/555/555/578/578/568/568 fast (control 532). NOT kept:
// no exact match; original precision, helper, body and frame/gain homes restored.
// Round 84 (m3 lane 9, fast control 532, full 545): `#pragma intrinsic(sqrt, exp, log10)` (the one diagnostic) 545
// plain, same code: under /Op the three math calls stay library calls where retail expands them inline (fyl2x,
// f2xm1, fsqrt). Retail also materializes each send bit as a bool in one dead stack byte (+0xe) and reloads the send
// flags from memory after every store, the volatile-bool shape. Forms: stream declared before input 532 (same code);
// gain bits read before `changed` 577 (1921 bytes); the first send bit cast to bool like the other three 550; the
// three send-bit updates as ternary assignments 532 (same code); `direct` as a bool cast 532 (same code). Restored.
// B426: Existing SDK C entry points at first/later query, volume, status, playback,
// mixbin, pitch and complete call phases: 532/532/532/532/532/532/532/532. Original SDK receivers,
// workspaces and complete body restored; frame/stream/gain and mixed math remain.
// Round 212 (m3 lane 3, fast control 532, plain 545; aligned 344): the sends as a fall-through two-case switch 545 fast (1901, aligned 331);
// on it direct declared uninitialised beside changed 545, ahead of changed 545, the send state at the function's top 545, status at the
// function's top 545; direct beside changed on the loop form 532. Frame stays 0x22c: retail's byte flags sit together at +0xe/+0xf, ours at
// +0x13/+0x1b, and the declarations do not move them.
// AC545: Existing real-pin input-first/complete-arm/lower-default/early-return
// producers 545/532/543/532, then forced expansion 545/532/543/532
// (fast control 532; full 545). NOT kept: ties or regressions; native frame,
// stream/gain homes and mixed-math expansion remain. Original helper restored.
// DL263: Actual stream/voice/effects object reference boundaries, seven subsets then complete
// input reference: 545/545/545/545/545/545/545/545. NOT kept: unmatched alias contracts; original
// receiver forms, existing helper and complete body restored. Frame 0x22c versus 0x224,
// stream/gain carriers and mixed math remain.
// M381: Actual voice/buffer/submix API-output zero initialization, seven subsets, then
// all with initialized status: 566/615/591/615/591/621/607/608 (fast control 532, full 545).
// NOT kept: unsupported extra stores and regressions; original outputs, helpers and full body restored.
// Round 217 (m3 lane 3, fast control 532, full 545; the fast image carries the builds-differently caveat): `changed` as a flag preset true and cleared by an if 544; the gain test as `fabs(...) >= 0.1f` 545; `changed` with `== 0` tests 532 (same code);
// mode and direct read ahead of `changed` 555 (1902 B), with the fall-through two-case send switch (round 212's form, 545 alone) 558 (1913 B); the gain source pointer chosen by an if ahead of the bits read 536. Body unchanged.
// M3 lane 11 r72 (fast screens only: fast control 532 at 1905 bytes, masked 235, CAVEAT builds differently; plain 545): `(bool)` on the first
// send bit 550 (1907; still folded to a shift, retail selects with `and al, 1; neg al; sbb; and 4`); gain as a value copy 532 (same code); gain bits
// read before the stream pointer 577 (1920, first difference +0x0); send state declared at function scope 532 (same code); energy loop over
// `input->levels[i]` with no levels pointer 563 (1923, masked 223). Lead unchanged: retail subtracts in SSE and keeps x87 temporaries on the stack
// (`fsub st(1)`) with no rounding stores, which ours has after every step under the file's /Op; the extra temporary is likely the frame's 8 bytes.
// AC553: Existing integer-bit pin input-first/lower-default/upper-default/early-return
// results, ordinary then forced expansion: 534/534/534/537/534/534/534/537 (fast control 532).
// NOT kept: no native frame/register/math recovery; original helper and body restored.
// DL269: Existing decibel value types at real-pin output, bit-pin output and linear input,
// subsets 1..7: 532 each; all with native selected gain 546 (fast control 532, full 545).
// NOT kept: no gain or exact match; original producer interfaces, frame and body restored.
// DL271: Existing integer-clamp pointer/in-place/returned-pointer/reference outputs
// 548/548/548/548; real-clamp output protocols 549/549/545/549 (control 545).
// NOT kept: no equality or gain; original producer ownership, frame and body restored.
// Lane 5 r164 (plain; aligned/register-masked LCS [369/246] for 544): retail builds the first send flag as
// `shr; and al,1; neg al; sbb eax,eax; and eax,4` (a select on a bool value), ours folds it to a shift whatever the
// spelling: `(bool)` on that bit 545, all five input bits as one-bit fields of field_30 declared in the input record 545
// (same code), flags zeroed then file-local SET_FLAG per bit (the macro five files define) 561 (1916 bytes). The four
// levels as one 16-byte memcpy 560 (1885 bytes). Mode and direct read before the gain with the voice sends as a case-1-
// into-case-0 switch 572 (1879 bytes [350/238]). Lead: every flag test in the function (input bits and sends.flags bits)
// stores its bool to [esp+0xe] (direct's home), written about ten times and read once, and sends.flags is reloaded
// after each such store, as if that byte were address-taken memory that may alias the send record.
// Lane 12 r102 (plain; full 544, the fast image 536 with the builds-differently caveat): gain bits read before the
// stream pointer 555; the sends as retail's fall-through two-case switch on the mode (case 1 into case 0) 542 (1901
// bytes; the block then has retail's shape); fabsf at all six tests 544; the three send-bit sets through a set helper
// with a bool parameter 544 (identical) and through one function-scope bool assigned before each test 544 (identical).
// The 17 x87 rounding stores ours has (retail none) still come from the file's /Op: lead for the user, flags unchanged.
// m3 lane 7 r146 (fast control 536 at 1906 bytes, full 544; fast is unreliable here, so plain checks decide): `sends.flags` started at 0 and
// bit 1 set by an if 554 fast; the voice read at its one use (no voice pointer) 525 fast, 544 -> 552 plain; the gain reference dropped
// (gain_bits cast at both uses) 536 fast (same code); the status local declared in each arm 544 plain (same code); the two-case switch
// re-measured 562 plain at 1884 bytes [masked 225 against 235], with the voice read at its use 563 plain (1921 bytes). In every form the
// stream stays in EBP and gain_bits in EBX (retail: EBX and EBP). Retail also inlines exp, sqrt and log10 as x87 code (f2xm1/fscale,
// fsqrt, fldlg2/fyl2x) where this /Op file calls the CRT, and copies the four levels and the send fields through integer registers.
// Round 73 (m3 lane 15, plain; fast builds this function differently): the voice sends as `if (mode == 0 || mode == 1) { add;
// if (mode == 1) add; }` 546 (1904 bytes). Diagnostic (the one compiler-setting probe): the file without /Op 723 fast (1728
// bytes; re-measure of the listed result): the x87 intrinsics (fsqrt, fyl2x, f2xm1) appear, but the pushed byte arguments stay
// `movzx` where retail zero-extends with `xor reg, reg; mov reg8, [mem]`. Retail also stores every flag test's bool in the one
// stack byte at +0xe (the volatile-bool shape of matched 0x21ec00), which this round may not add.
// Round 73, continued: the two-if voice sends drop our send-loop counter slot (frame 0x228 against retail's 0x224; our 0x22c
// holds the counter at +0x14), masked aligned 238 -> 251.
// M3 lane 8 r151 (fast = plain here, 544; masked aligned 170): the XDK stream entry points that matched 0x221490 calls,
// in place of the inline methods: the stream mix-bins call 544, the pitch call 544, both volume calls 544,
// the voice-properties call 544 (same code each); `bool direct=input->field_30&1;` 544 (same code). Retail also zero-extends the
// channel byte as `xor reg, reg; mov reg8, [ebp+1]` where ours has movzx (three sites); with the subtractions and rounding
// stores, that is another /Op-or-flags lead for the user.
// M3 lane 12 r110 (fast control 536 at 1906 bytes, full 544 at 1901; fast carries the global-reference CAVEAT): `direct`
// read before the gain with mode after it 540 (1902 bytes); mode before the gain with direct after it 609 (1962); the mixbin
// loop bound as `send < mode + 1` 532 fast but 545 plain (1901 bytes); the send flags cleared first and every bit set or
// cleared alike 565 (1922); the send bits read from one named input flags word 550 (1908). The file's /Op stays the
// largest cause (flags are the user's decision).
// M3 lane 13 r82 (fast, control 544): the four send-bit tests through an inline `bool result = false; if (bit) result =
// true; return result;` test (the shape behind 0x19e1c0's dead byte store), tried for retail's per-bit stores to one
// stack byte: 558 (1886 bytes, no byte store appears). Retail stores every flag-test bool to +0xe, including `direct` and
// the `!(input->field_30 & 1)` and `!(sends.flags & 0x20)` tests. No untried natural form is left; the file's /Op
// (rounding stores, CRT fabs/exp/sqrt/log10 calls, frame 0x22c) stays the cause.
// m3 lane 14 r89 (plain; control 544, masked aligned 234): `changed` with the global test first 571 (1925 B, masked 249).
// Retail's voice sends are a switch on the mode (`mov eax, esi; sub eax, 0; je; dec eax; jne`, case 1 calling the add
// and jumping into case 0's), and it pushes zeros as `xor eax, eax; push eax` and zero-extends bytes as `xor; mov reg8`
// where ours has `push 0` and movzx: two more settings leads beside /Op. Lane 13 r82's finding stands: no untried
// natural form is left, and the file's flags are the cause.
// P481: Existing bit-value constructors at zero/selected/send volume boundaries,
// masks 1..7, then the original append boundary: 548/536/548/536/547/536/547/536
// (fast control 536, full 544). NOT kept: no native frame/role recovery or equality;
// original constructors, helpers and complete body restored.
// @retail 0x21fa80
void function_21fa80(long channel_index, s_looping_channel_properties const *properties,
	s_looping_effect_playback const *effects, bool force)
{
	s_channel_update_input const *input=(s_channel_update_input const *)properties;
	s_sound_stream *stream=&SOUND_DRIVER_GLOBALS->channels[channel_index];
	bool changed=!(stream->unknown03_0&1) || !SOUND_DRIVER_GLOBALS->unknown0000;
	long gain_bits=*(long const *)(input->gain>-64.0f ? &input->gain : &g_44a23c);
	real const &gain=*(real const *)&gain_bits;
	long mode=stream->channel_count;
	bool direct=(input->field_30&1)!=0;
	if(changed || !(fabs(gain-*(real *)&stream->unknown0c)<0.1f))
	{
		if(input->voice_index>=SOUND_DRIVER_GLOBALS->unknown0008)
		{
			DSVOICEPROPS voice_properties;
			stream->stream->GetVoiceProperties(&voice_properties);
			s_sound_driver_voice *voice=&SOUND_DRIVER_GLOBALS->voices[input->voice_index];
			s_mixbin_settings settings;
			sound_mixbins_initialize(&settings);
			if(mode==0 || mode==1)
				for(long send=0;send<=mode;++send)
					sound_mixbins_add(&settings,voice->unknown00,*(s_decibels const *)&gain_bits);
			function_2215d0((s_mixbin_list *)&voice_properties,&settings,stream->stream);
			stream->stream->SetVolume(sound_decibels_to_volume(0.0f));
		}
		else stream->stream->SetVolume(sound_decibels_to_volume(gain));
		stream->unknown0c=gain_bits;
	}
	if(changed || direct!=((*(byte *)((byte *)stream+3)>>5&1)!=0) ||
		(!direct && (!(fabs(input->field_18-*(real *)(stream->unknown10))<0.1f) ||
		 !(fabs(input->field_14-*(real *)(stream->unknown10+8))<0.1f) ||
		 !(fabs(input->field_1c)<0.1f) || !(fabs(input->field_20)<0.1f))) ||
		(input->voice_index!=NONE && !(fabs(input->field_28-*(real *)(stream->unknown10+4))<0.1f)))
	{
		s_channel_send_state sends;
		sends.flags=(input->field_30>>1&1)?4:0;
		if((bool)(input->field_30>>3&1)) sends.flags|=8; else sends.flags&=~8;
		if((bool)(input->field_30>>4&1)) sends.flags|=16; else sends.flags&=~16;
		if((bool)(input->field_30>>6&1)) sends.flags|=64; else sends.flags&=~64;
		if(direct) *((byte *)stream+3)|=0x20; else *((byte *)stream+3)&=~0x20;
		if((bool)(input->field_30>>5&1))
		{
			sends.left_gain=input->levels[0]; sends.right_gain=input->levels[1];
			sends.rear_left_gain=input->levels[2]; sends.rear_right_gain=input->levels[3];
			real const *levels=&sends.left_gain;
			real energy=0.0f;
			for(long i=0;i<4;++i)
			{
				real amplitude=channel_linear_gain(levels[i]);
				energy+=amplitude*amplitude;
			}
			real amplitude=(real)sqrt(energy);
			real bounded=amplitude<0.0f?0.0f:amplitude>1.0f?1.0f:amplitude;
			// retail multiplies the log10 (fyl2x, 0x21fe2f) by 20 straight from the x87 stack (0x21fe31), so
			// the logarithm is taken as a double: the float overload would round it first
			real level=amplitude>0.0f?channel_pin_gain((real)(log10((double)bounded)*20.0f)):-64.0f;
			sends.level=level;
			sends.level_offset=input->field_14;
			sends.flags|=0x20;
		}
		else
		{
			if(!(input->field_30&1))
			{
				sends.level=input->field_18; sends.level_offset=input->field_14;
				sends.gain0c=input->field_1c; sends.gain10=input->field_20;
				*(real *)(stream->unknown10)=input->field_18;
				sends.flags|=1;
				*(real *)(stream->unknown10+8)=input->field_14;
			}
			if(input->voice_index!=NONE)
			{
				sends.field_14=input->field_28; sends.flags|=2;
				*(real *)(stream->unknown10+4)=input->field_28;
			}
		}
		if(input->voice_index>=SOUND_DRIVER_GLOBALS->unknown0008 && !(sends.flags&0x20))
		{
			s_sound_driver_voice *voice=&SOUND_DRIVER_GLOBALS->voices[input->voice_index];
			s_mixbin_settings unused;
			sound_mixbins_initialize(&unused);
			DSVOICEPROPS buffer_properties, submix_properties;
			voice->buffer->GetVoiceProperties(&buffer_properties);
			voice->submix->GetVoiceProperties(&submix_properties);
			if(sends.flags&2) voice->buffer->SetVolume(sound_decibels_to_volume(sends.field_14));
			else voice->buffer->SetVolume(-10000);
			DWORD status;
			if(sends.flags&1)
			{
				s_mixbin_settings settings;
				sound_mixbins_initialize(&settings);
				channel_effects()->add_effect_sends(&sends,SOUND_DRIVER_GLOBALS->surround,0,&settings);
				function_2216f0((s_mixbin_list *)&submix_properties,&settings,voice->submix);
				voice->submix->GetStatus(&status);
				if(!(status&DSBSTATUS_PLAYING)) voice->submix->Play(0,0,0);
			}
			else
			{
				voice->submix->GetStatus(&status);
				if(status&DSBSTATUS_PLAYING) voice->submix->StopEx(0,0);
			}
		}
		else
		{
			s_mixbin_settings settings;
			sound_mixbins_initialize(&settings);
			if(sends.flags&1) channel_effects()->add_effect_sends(&sends,SOUND_DRIVER_GLOBALS->surround,mode,&settings);
			if(sends.flags&2) channel_effects()->add_send(31,mode,sends.field_14,&settings);
			if(sends.flags&0x20) channel_effects()->add_sends(&sends,SOUND_DRIVER_GLOBALS->surround,mode,&settings);
			channel_effects()->add_source_mixbins(input,SOUND_DRIVER_GLOBALS->surround,mode,&settings);
			stream->stream->SetMixBins(&settings.mixbins);
		}
	}
	if(!force)
	{
		long offset;
		switch(input->format) { case 0: offset=-4597; break; case 1: offset=-501; break; default: offset=-2396; break; }
		real pitch=input->pitch*3.4133334159851074f+(real)offset;
		pitch=pitch<-32767.0f?-32767.0f:pitch>8191.0f?8191.0f:pitch;
		long value=(long)pitch;
		if(changed || (real)abs(value-stream->unknown08)>=1.5f)
		{
			stream->stream->SetPitch(value);
			stream->unknown08=value;
		}
	}
	function_2201f0(channel_index,(s_sound_channel_parameters const *)effects,false);
}

// @retail 0x21f8a0
void function_21f8a0(long channel_index,s_looping_channel_properties const *properties,
	s_looping_effect_playback const *effects)
{
	s_channel_update_input const *input=(s_channel_update_input const *)properties;
	s_sound_stream *stream=&SOUND_DRIVER_GLOBALS->channels[channel_index];
	*((byte *)stream+3)|=0x10;
	stream->unknown28=input->field_2c;
	stream->offset=0;
	if(stream->unknown28!=NONE) stream->stream->Pause(1);
	else stream->stream->Pause(0);
	sound_stream_set_envelope(stream,0.0f,0.1f);
	if(input->voice_index!=NONE)
	{
		stream->stream->SetOutputBuffer(SOUND_DRIVER_GLOBALS->voices[input->voice_index].buffer);
		stream->unknown00=(byte)input->voice_index;
	}
	else
	{
		stream->stream->SetOutputBuffer(NULL);
		stream->unknown00=0xff;
	}
	function_2201f0(channel_index,(s_sound_channel_parameters const *)effects,true);
	*((byte *)stream+3)&=~1;
	function_21fa80(channel_index,properties,NULL,false);
	*((byte *)stream + 3) |= 1;
}
