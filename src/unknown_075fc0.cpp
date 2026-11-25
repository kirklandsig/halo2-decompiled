// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_075870.h"
#include "globals.h"

extern short g_485ac0;
extern s_connection_counter g_4e6398;
real g_4cf5d0;

bool network_observer_channel_has_host(s_network_observer *arg_0, long arg_1);
bool function_76520(long arg_0, bool arg_1, long *arg_2);
long __stdcall function_765c0(long arg_0, long arg_1, byte *arg_2);
long link_connection_get_reliable_window(s_link_connection *arg_0);
long link_connection_get_reliable_bytes(s_link_connection *arg_0);
long link_connection_get_reliable_pending(s_link_connection *arg_0);
long link_connection_owner_active(s_link_connection *arg_0);

struct s_75fc0
{
	byte field_0[0x470];
	long field_470;
	byte field_474[4];
	__int64 field_478;
	long field_480;
	real field_484;
	long field_488;
	bool field_48c;
	bool field_48d;
	bool field_48e;
	byte field_48f[5];
	long field_494;
	long field_498;
	real field_49c;
	bool field_4a0;
	bool field_4a1;
	bool field_4a2;
	byte field_4a3[0x4dc - 0x4a3];
	long field_4dc;
};

union s_75fc2
{
	s_connection_counter field_0;
	__int64 field_8;
};

static __forceinline long function_75fc1(real arg_0)
{
	long local_0;
	__asm
	{
		fld arg_0
		fistp local_0
	}
	return local_0;
}

static __forceinline long function_75fc3(s_network_observer *arg_0, long arg_1)
{
    long local_0 = NONE;
    for (long local_1 = 0; local_1 < MAXIMUM_OBSERVER_CHANNELS; local_1++)
    {
        s_network_observer_channel *local_2 = &arg_0->channels[local_1];
        if (local_2->state && local_2->connection_index == arg_1)
        {
            local_0 = local_1;
            break;
        }
    }
    return local_0;
}
// score 356 (was 356): frame 0x38 against retail's 0x34; `this`, NONE, zero and index registers differ (retail EDI, ESI,
// EBX, EBP), and 0x75910's result is tested as a long (BLOCKED-BY 0x75910); earlier notes below.

// MiniMax I (11-17): no new form, no lower score (356): re-measured the baseline; the first difference is still at
// +0x2, retail's prologue reserves 0x34 with the connection in EDI, the NONE state in ESI and the channel index in
// EBP where this build reserves 0x38 and splits the callee-saved registers differently, and the timed-out check's
// result is tested as a byte in retail where this build's callee returns a long; that callee, 0x75910, is defined
// in another file, so the call's shape cannot be settled here. The body stands.
// MiniMax D (11-14): no new form, no lower score (356): the first difference is still at +0x2, retail's prologue
// `sub esp, 0x34; push ebx; push ebp; push esi; or esi, -1; push edi; mov edi, ecx` against this build's 0x38 frame
// and its other split of the callee-saved registers, and the timed-out check's result is still tested as a byte in
// retail where this build's callee returns a long; that callee, 0x75910, is defined in another file, so the call's
// shape cannot be settled here. The body stands.
// MiniMax H (11-13): no new form, no lower score (356): the frame is still 0x38 here against retail's 0x34 (the
// later value still has a stack home where retail keeps it in ebp) and the callee-saved set still differs (retail
// keeps the observer in edi and the channel index in ebp); the helper whose byte result retail tests is 0x75910, and
// the permuter's 25 tries in 309 s found only the call 0x765c0's two first arguments swapped, worth one point.
// MiniMax G (11-12): no new form, no lower score (356); the frame is still 0x38 here against retail's
// 0x34 (local_18 still has a stack home where retail keeps it in EBP) and the callee-saved set differs
// (retail keeps the connection in EDI and the channel index in EBP); the helper whose byte result retail
// tests is 0x75910, whose definition is in src/unknown_075870.cpp and outside this lane.
// MiniMax C (11-11): no new form, no lower score (356): dropping the host-state copy reaches retail's
// 0x34 frame but rises to 416, and what still differs is the callee-saved set - retail keeps the
// connection in EDI with four register saves and none in ESI and the channel index in EBP, ours
// reserves 0x38 and splits the saves - together with 0x75910's byte result, which is in another file.
// MiniMax B (11-13): no new form, no lower score (356): the prologue still differs in its frame split (ours 0x38
// with three register saves against retail's 0x34 with four), this sits in ebp where retail keeps the connection in
// edi, and the -1 sentinel is ebx where retail keeps it in esi.
// MiniMax S (11-14): no new form, no lower score (356): the extra frame bytes come from the homes the named
// locals still take, and dropping a home means writing that local's expression out at each of its uses, which
// this lane's rules forbid; the goto/label shape that carries the result is kept, and the body stands.
// MiniMax R (11-15): no new form, no lower score (356): the frame is still 0x34 against retail's 0x38 (local_18 still has a stack home where retail keeps it in EBP), and the observer, channel and result homes differ.
// MiniMax S (11-16): earlier 356 (was 356): no new form; retail's prologue reserves 0x34 with four register saves
// and keeps the connection in EDI, the NONE state in ESI and the channel index in EBP, where this build reserves
// 0x38, keeps the observer in EBP and the argument in ECX, so the two disagree from the first instruction on;
// the timed-out callee's byte result is the other open item and lives in another file.
// Earlier 356 (was 371): frame 0x38 versus 0x34 (local_18 still has a stack home, retail EBP); observer/channel/result
// and counter homes remain.
// MiniMax R (11-10): no new form, no lower score (356); the frame is still 0x38 here against retail's 0x34, this
// still takes a frame pointer where retail keeps the connection in EDI, the packet-availability result still lands
// in bl where retail keeps the host state there, and the helper whose result retail tests as a byte is still declared
// to return a long in a file this lane may not edit; the body stands.
// MiniMax D (11-10): no new form, no lower score (356); the frame is still 0x38 here against retail's 0x34, this
// still takes a frame pointer where retail keeps the connection in EDI, the packet-availability result still lands
// in bl where retail keeps the host state there and publishes the packet state in a separate byte, and the helper
// whose result retail tests as a byte is still declared to return a long in a file this lane may not edit.
// MiniMax R (11-06): no new form, no lower score (371); the frame is 0x38 here and 0x34 in retail, and this takes a frame pointer where retail keeps the connection in EDI.
// MiniMax E (11-10): no new form, no lower score (356); the extra frame bytes come from the local that still has
// a stack home where retail keeps the connection in a register.
// MiniMax I (11-01): 374 with the has-host and packet flags as words (score-picked, see below), the record as
// observer + index*0x528 with the 0xa8 folded into the field offsets, the search through the channel array and
// the counter snapshot through the existing union; with the two flags back to bool 430, also without the union
// snapshot 411. Not kept; body unchanged.
// Earlier: the 388 form below (four call-result and state locals as word, exact
// 1369-byte length) was not kept: retail keeps these in 32-bit homes with no 16-bit operations,
// so the word type is score-picked. Frame 0x3c versus retail 0x34; homes differ.
// Earlier at 408: retail's frame is 0x34 and it holds `this` in edi, forming the channel
// pointer as observer + index*0x528 with the 0xa8 folded into every field displacement; ours has a
// 0x3c frame, spills `this` and adds 0xa8 to the channel base, and composes the counter's halves.
// Earlier: frame 0x3c/0x34; channel/result homes and the counter copy remain.
// Native channel 434; primary/both counter products 448/419; native products 446/423;
// native live flag/callback/both 441/439/432; direct counter product 419;
// both/primary word concatenation 423/431; native both/primary concatenation 415/441.
// Native both concatenation plain 415 at 1350/1369 bytes; products add a call and are restored.
// Output name 415; counter name 415; paired output names 415; later channel 430;
// flag test order 416; output declaration first 415 (fast).
// Search direct returns/earlier result 415; indexed search fields and combinations 419; identity-first test 415. Return byte/short/long 421/409/414; common exit on current channel/counter graph 426. Six-point short gain excluded; original helper, result and exits restored.
// Lane 9 r23: result as a volatile stack byte 419 (search, its jmp/found block and the result's
// argument home follow retail); with `if (local_6 < 48) return` 406; store-only-where-retail
// for all three result updates 435 (449 without volatile); `local_23 < local_13` test 407, both
// tests 416, `!(local_23 >= local_13)` 416; fields through channels[index] 410 at 1369 bytes (retail's
// [esi+0x520] displacements), 423 without volatile; window factor as if 413, with the field form 417. Not kept: the volatile result
// (a volatile local steers the allocator; volatiles are kept only with a byte match). Body unchanged.
// Lane 9 r26: the search helper's `#pragma optimize("g", off)` removed 415 (identical code, kept);
// every `return result` through one exit 408 (kept: retail's entry pushes all four registers before the
// search and keeps the result in the first parameter's slot); counter snapshot through the existing
// union 441, with the reset from it 432; channel fields through channels[index] 415; host flag set in
// the host test's arm 431; the window factor as if/else 408 at 1369 bytes (retail's byte test and
// branch; no gain, not kept); with the union snapshot 439 (frame 0x34 as retail), plus the host-flag
// arm 433.
// Round 148 (counts, then difflib-aligned differences with the baseline at 427): snapshot as the existing
// union copy 445/425; plus the reset as a struct copy of the counter through a union member of field_478
// 459/417; window factor if/else 430/432; channel fields through channels[index] 415/257; index + union
// snapshot + struct reset 440/241; that plus the window if/else 425/230 (frame 0x34, `this` in EDI and the
// search's own channel pointer as retail; left: NONE/index/zero registers, the has-host flag set in its
// test (retail stores false first, 436/226), the 0xe4/0xe8 pick, the 0.0 early exit, the reliable-bytes
// tail); on that, window as `= 0; if` 440/226; 0xe4/0xe8 as if/else 438/237; the 0.0 exit as its own
// return 444/334. Body unchanged.
// Round 222: Persistent configuration/updated counter/both 428/415/432; callback-scale arms 415; configuration-arms/counter-arms/all 424/422/415; all with named window terms 415. All restored; frame 0x3c versus 0x34, channel/result homes and counter composition remain.
// MiniMax D run 2026-10-29: the two flag stores in the other order and the reliable-pending call's
// declaration split from its assignment 407 (gain 1, under the keep bar; not kept).
// Round 270 M: sequential snapshot OR/add low word 408/418; scratch declared first
// 408/418; packet output initialized first 424/420; both entry lifetimes 424/420.
// Same counter bits throughout; original composition, lifetimes and complete body restored.
// Round 314: Persistent cursor with pointer/indexed fields and ternary/branch window: state-first
// 407/430/413/419; identity-first 407/430/413/419. One-count best is rejected; original finder,
// channel/counter/result graph and body restored at 408; frame 0x3c versus 0x34 remains.
// Round 207: canonical host-result byte/word/long 408/404/407; packet-result
// byte/word/long 408/410/425; both word/long 397/424. Both word carriers 397
// plain, kept; independent four-point host gain excluded. Provider definitions,
// call tuples, counter bits, returns and every output are unchanged.
// On paired word results: derived callback/channel-active/forced-send word domains,
// seven nonempty subsets [426, 411, 405, 408, 388, 395, 389]; signed short call results 397.
// The paired call results and callback/secondary-output states as words gave
// 388 plain (not kept, see above); channel-active remained Boolean.
// Round 247: single remaining-budget admission with maximum/available block-quotient
// rounding subsets 427/428/430/429; separate raw/aligned capacity counterparts
// 427/428/430/429. Original tail, counters, result graph and entire body restored.
// Round 260: Shared threshold/payload/both rounders 428/428/437; separate raw/capped allowance 408, with
// shared rounders 437; query-phase zero packet result 407, with shared rounders 438, all 438. All
// restored.
// Round 211: late secondary-output producer/host-first admission/fallback flag and their
// combinations 453/426/418/449/434/437/457; joint budget arm 457. Fallback flag alone
// recovers frame 0x34 at 418; with indexed channel fields 426, plus branch window 410.
// Original complete state graph and body restored; observer/result/counter homes remain.
// Round 306: Late selected-limit, packet-allowance and pacing defaults, seven subsets
// 448/436/455/453/477/446/478; all plus late frequency default 490. All regress; original body and
// capacity/result/counter lifetimes restored at 408. Frame 0x3c versus 0x34 and
// observer/result/counter homes remain. Late selected limits alone 448 and late pacing alone 453
// recover frame 0x34 but lose the early default stores; their combination over-shrinks it.
// DH298: Final success lifetime/direct forcing-state output/capacity-output arm,
// seven subsets 420/453/451/433/435/439/444; final capacity-produced result with direct
// forcing-state output 447. All restored; frame, observer and counter homes remain.
// Lane 17 r42 (fast, plain where marked): kept, declarations in retail's initialization order (local_26 and local_6
// set to NONE together after the search, local_18 beside local_13, local_5 after the -1.0 default) 393 plain; the
// first two alone 401 (aligned 427 -> 274). `this` for the `arg_0` alias 408 (same code). On 401: union snapshot 421;
// result stored only where retail stores it (no store in the two threshold tests, `false` in each failing arm of the
// reliable-bytes test) 404, that test nested with `!(a >= b)` for the first 419. On 393: first test without the
// store 417, second 417, union snapshot 436, nested reliable-bytes test 408, the -1.0 default before local_16 393.
// F363: Native record input-flag byte domains, subsets 1/2/4/3/5/6/7,
// then all signed bytes: 408/408/408/408/408/408/408/408. Original flag domains and body restored;
// frame 0x3c/0x34 and observer/result/counter homes remain.
// M352: Reliable payload producer/native failure arms/positive arms/test-produced result/
// failure predicate/quotient producer/nested capacity gate/payload with failure arms 408/418/426/416/408/408/408/418.
// All restored; frame 0x3c versus 0x34 and observer/channel/result/counter homes remain.
// Round 72 (m3 lane 10, fast; each form with a register-masked aligned count after the slash, baseline 393/179). Not kept, because it needs a flag set in a test (match-only): 312/82 at retail's 413
// lines, plain, all callees matched: the channel's fields declared in the header and read as channels[local_0].field (s_75fc0 gone), the has-host result as `local_8 = false; if (has_host(...)) { ...; local_8
// = true; }`, the two NONE locals ahead of the search, the capacity test as if / else if / else, the window factor `= 0; if`, the callback scale as if/else, the first threshold through local_2 as now, the tail as
// `if (local_34 >= 0) { ...; return local_2; } return false; done: return local_2;`. Its has-host result as a plain initialiser, with every on/off grid of window factor, threshold test, NONE locals and tail: 418 to 453.
// On the way: all exits as `return local_2` 438/124; single exit 410/94; every field through a channel pointer local 425/126; that pointer unused, or a dead read of the state 410/94; a byte-pointer base at
// observer + index * size 415/105. A second flag set in the arms for the first threshold, flat `if (!ok) goto done` 422/88 (`setge; test; je` as retail), plus the capacity test 417/80, plus the NONE locals first
// 397/81, plus the window factor as its ternary 367/69 (retail's whole block layout, the zero test out of line before the exit). That threshold with `bool ok = a >= b` in the arm or a direct compare 399/85, through local_2
// 391/89. Nested or && capacity test 422/88; the two budget picks as ifs, or with `do {} while (false);` between them 422/88 (ours merges their tests, retail tests twice). Search loop `i < n && found == NONE` 546 (unrolled),
// `found == NONE && i < n` 395/88. NONE locals after the search or split round it 414/80; a dead init of the index or another local 417/80 (no entry register). On 312: second threshold through local_2 390/89; the tail's false arm
// into local_2 416/81; the label inside the tail block 347/75; local_14 set in an arm 311/86. Lead: retail computes the channel base right after the connection (lea esi, [eax + edi]), so the index stays live in EBP, NONE shares ESI and
// `this` takes EDI; ours computes it at the first field read and coalesces it with the index. One field through a pointer local with the rest direct gives that early base (419/86, one-use pointer: match-only); the byte-pointer base on the 367 form 379/88.
// Round 74 (m3 lane 10, one-file harness then fast and plain): kept 393 -> 371 (1381 bytes): the owner-active scale as if/else, the window size as `= 0; if`, the tail as `if (local_34 >= 0) { ...; return local_2; } return false;`.
// They only pay together (fast): scale 395, tail 399, window size 405, scale + tail 381, scale + window size 412, tail + window size 408, all three 371.
// On those, fast: window test as `!(a > 0 && b > a)` alone 389 (retail's comiss order), no gain on top; capacity test as if/else if/else 368 (a flag set in arms, direct form 371, not kept); all with NONE locals before the search and the
// channel pointer before the connection 367; NONE locals first 408; channel pointer first 422; threshold tests without the store 396, both 407; `local_33` as if/else 378. Harness: named header fields through channels[index], union
// snapshot and reset 375 (aligned 176, masked 65, against 200/103 here: lower aligned, higher score); with a counter pointer and the rest 414 (masked 60); union copies on this file's struct 380 (masked 76). Match-only lead from the r72
// draft (harness, r72 masked metric): a multi-use counter pointer declared first after the found test (`lea esi, [eax + edi]` right after the connection as retail, because the pointer shares `this + index * size`) 263 + 8 bytes, masked 46;
// with the window test 42; a threshold flag set in both arms and tested after (`setge; test; je` as retail, the zero test moves toward the exit) masked 42. Left there: `this` isn't in EDI at entry and the connection takes EBP where retail
// keeps it in memory and gives EBP to the budget; 8 extra budget reads don't flip that, so it isn't plain reference weight; `xor al, al` for the result's first store. Retail's three epilogues: done, success, false.
// Round 95 (m3 lane 12, fast; register-masked aligned count in brackets, base 371 [244]): the rounding helper's names as value/result
// 371, the search helper's as observer/connection_index/result/channel_index/channel 371, both 371 (identical code); the reliable-bytes
// payload `local_29 - 45` as a named local for its test and store 427 [203] (1385 bytes; retail forms it early, `lea ebp, [ecx - 0x2d]`);
// the snapshot and reset as `*(__int64 *)&g_4e6398` (unknown_12b070.cpp's view) 426 [229] (1340 bytes). Retail keeps the result byte in
// arg_1's dead slot and the rounding temp in arg_3's ([esp+0x48], [esp+0x50]), fistp straight into local_13's home.
// T64: Explicit high/low integer-value producers through SDK/existing counter unions,
// snapshot only or snapshot/reset and both component orders: 424/424/425/425/424/425/424/425.
// Frame reaches 0x34 but result/budget/observer homes regress. NOT kept: unmatched native-value
// union/copy forms; original bit composition, all thresholds, outputs and body restored.
// P365: Sole logical-result exit on the current tail, positive scope bool/byte/char/long
// 356/429/429/430; negative scope 400/429/429/443. Keep only the original-bool
// positive scope, plain 356 at 1374/1369 bytes; native three epilogues remain.
// No call, input, arithmetic or output change. Wider/negative forms restored.
// P368: On the sole-result graph, interval/rate/window home identities, seven subsets,
// then all with the elapsed-tick home named: 356/356/356/356/356/356/356/356. NOT kept:
// no native home recovery or byte match; original names and complete body restored.
// P370: Complete host-first/callback-first rate producers, branch/expression, independently or
// jointly selected limits: 422/422/417/417/422/422/417/417 at 1384/1380 bytes. NOT kept: all
// regress and grow from native length; original rate, limits and complete body restored.
// P372: Reciprocal-limit ownership, immutable/mutable/earlier bare storage,
// disjoint clamp arms, full conditional, conditional with reciprocal, carried candidate,
// successive reciprocal division: 399/399/399/421/356/399/399/399.
// NOT kept: ties/regressions; original complete body restored. Native frame 0x34,
// observer EDI, bandwidth EBP and counter/result homes remain unrecovered.
// P375: Entry constant-default lifetimes for selected capacities/pacing/payload-output, masks 1..7,
// then complete defaults with returned Boolean first: 429/488/463/350/424/499/505/506. NOT kept: 350 reaches native
// 1369 bytes but gains only six; no native sentinel/home match. All original defaults and body restored.
// P377: Whole packet-scale branch/negative-chain/callback-first/expression producers,
// then each with its actual scaled-payload node: 356/363/413/363/356/363/413/363. NOT kept: no native observer/budget/frame
// equality; complete original scale, rounder, counter and result graph restored.
// P379: Complete complementary host-query result in bool/byte/char/signed-char/short/word/int/long, all
// original consumers inverted consistently: 425/425/425/425/374/374/417/417. NOT kept: no native
// host-byte/observer/budget/frame equality; original result, counter compositions, outputs and whole body
// restored.
// P381: Complete complementary packet-query result in bool/byte/char/signed-char/short/word/int/long:
// 426/426/426/426/425/425/426/426. NOT kept: all regress and lengthen; native
// packet BL, observer EDI, bandwidth EBP and frame 0x34 remain. Original query result and full body restored.
// P384: Complete complementary captured channel-active state, bool/byte/char/signed-char/short/word/int/long:
// 442/442/442/442/442/442/443/443. NOT kept: all regress and grow; native observer/bandwidth/frame homes remain.
// Original active-state observation, all consumers, counter expressions and complete body restored.
// T73: Complete derived callback-send branch/default/conditional/negative/complementary producers:
// 408/419/399/397/408/419/419/415. NOT kept: no native observer/rate/frame repair;
// original providers, later forcing, bit-correct counters, limits and sole-result body restored.
// P386: Complete complementary forced-send state in bool/byte/char/signed-char/short/word/int/long:
// 416/416/416/416/426/426/429/429. NOT kept: no exact native state/frame recovery; original
// force producers, all observations, counters, outputs and complete body restored.
// P396: Complete selected bandwidth/pending-window conditional or branched result producers,
// single/paired/mixed forms: 401/415/420/401/415/420/420/420. NOT kept: no native observer/rate/result recovery;
// original early defaults, field observations, counters and complete body restored.
// MiniMax E (11-09): no new form, no lower score: retail keeps the packet-availability result in bl and publishes the host state in a separate byte, ours keeps the host in bl and spills the packet state.
// M3 lane 14 r74 (fast, base 356; difflib-aligned count / register-masked aligned count after each score, base 195/157): the snapshot as a copy
// into the existing counter view 431 (1341 bytes) 172/136; the channel's counter field declared as that view, the reset as a copy of the counter and
// the difference from its low half 419 (1356) 179/130; both 429 (1330) 209/162; the reliable-bytes test as `if (a && b) local_6 = ...; else local_2 =
// false;` 402 (1369) 195/157; the second threshold as `if (local_6 < 48) goto done;` 413 205/166; both 408 (1369) 207/165; the two NONE locals ahead of
// the search 358 (1377) 194/155; every field through channels[local_0] (no pointer local) 431 (1387) 198/134, with the reliable-bytes else 429 (1384)
// 198/132, with the counter field view 408 (1374) 181/119, with view and snapshot 421 (1351) 124/77, with view, snapshot and the reliable-bytes else
// 368 (1348) 122/75: frame 0x34, `this` in EDI, retail's plain 8-byte snapshot and low-then-high reset, the best aligned form, but 21 bytes short
// and above the base, so not kept. On that form: second threshold direct 417 (1359) 165/115; the payload `local_29 - 45` named for its test and store
// 429 (1347) 154/93, with the direct threshold 406 (1348) 158/96, plus the NONE locals first 392 (1354) 159/99; view + else + NONE locals first on the
// index form 415 (1377) 177/117. Diagnostics (a flag set in its test, match-only): has-host as `= false; if (...) { ...; = true; }` alone 402 (1376)
// 192/161, with the NONE locals first 418, on the 368 form 403 (1357) 121/75, with the else and NONE locals 416 (1358) 120/74, with payload and direct
// threshold 396 (1356) 155/96. Left on the 368 form: retail keeps NONE in ESI and the index in EBP (ours the reverse), forms the channel base right
// after the connection, zeroes the has-host byte before its call, keeps the reliable payload in EBP ahead of the quotient test with local_6 in memory,
// and tests 0x75910's result as a byte (`test al, al`; BLOCKED-BY 0x75910: its definition returns long and sits in a held file).
// Round 112 (m3 lane 9, fast, base 356): the tick interval as `g_485ac0 > 0 ? g_485ac0 : 60` 430 at 1383 bytes;
// the `arg_3 ? 4.0f : 0.0f` rate as if/else 356 (same code); local_32's ternary as if/else 356 (same code);
// local_30 initialised to 0 with the else arm dropped 391 at 1372; the state-5 block folded into the
// `if (!local_8)` block above it 414 at 1376.
// No further untried natural form: retail's has-host form is a flag set in a test (match-only), the channel-field,
// snapshot, NONE-first, window/threshold/tail and capacity forms are listed, and 0x75910's byte result stays
// BLOCKED-BY 0x75910.
// MiniMax I (11-10): no lower score (356 kept); dropping the redundant copy of the host state (a bool local that only
// ever mirrors another local) does reach retail's 0x34 frame, but the score rises to 416 (1356 bytes) because the
// register allocation shifts with the frame. What still differs: retail keeps `this` in EDI with four register saves
// and none in ESI, keeps the channel index in EBP, and spills into the argument area, ours reserves 0x38 and splits
// the saves differently; retail also keeps the NONE state in ESI and forms the channel base right after the
// connection.
// MiniMax B (11-14): no new form, no lower score (356): retail's prologue still is `sub esp, 0x34; push ebx; push ebp;
// push esi; or esi, -1; push edi; mov edi, ecx` - the connection in EDI, the NONE state in ESI and the channel index in
// EBP - where this build reserves 0x38 and splits the register saves differently, and the timed-out check's result is
// still tested as a byte in retail (`test al, al`) where this build's callee returns a long; that callee, 0x75910, is
// defined in another file, so the call's shape cannot be settled here. The body stands.
// MiniMax R (11-16): no new form, no lower score (356): the frame is 0x38 here against retail's 0x34, and this build
// keeps the observer in ebp and the argument in ecx where retail saves esi and edi around the block.
// Round 244 (m3 lane 3, fast with 0x76520/0x765c0/0x77940/0x75910): the field_48c/48d/48e block nested in the
// `if (!local_8)` block below it 392 (1369 bytes); the has-host arms as `arg_3 = arg_4 = true;` 355 (one point, under the
// gate); the window size as `(flags & 8) ? 128 : 0` 397 (1369 bytes); the tick interval read before the elapsed count
// 404; the nesting with the ternary window 419; the nesting with the chained assignment 391. All restored.
// M3 lane 16 r136 (fast with 0x75910/0x76520/0x765c0/0x77940/0x89030, all MATCH; control 356, aligned 245): HELD (needs the
// owner's OK; build/r136/held-0x75fc0.patch): 0x75910 written as a bool function returning `owner && owner->active`
// still matches byte for byte (retail's 32-bit exits) and gives this function retail's `test al, al`, 355; its
// definition sits in another lane's file this round. The pacing test as `!(interval > 0 && 1 / rate > interval)` 356
// (aligned 241: retail's two comiss orders), as `0 >= interval || interval >= 1 / rate` 356 (same code); with the bool
// callee 355 (aligned 240). The two budget picks as if/else statements 356 (same code), in the other order 356 (same
// code), reading the channel's flag at each pick 438 (1388 bytes, aligned 227: retail's two separate tests, but the
// flag is then reloaded where retail keeps it in BL), with the pacing form 438 (aligned 223). The has-host arm as
// `arg_3 = arg_4 = true` 355 (one point, re-measured). The window flags read into a byte local first 356 (same code);
// the window size as `(flags & 8) ? 128 : 0` 397 (1369 bytes, re-measured).
// M3 lane 1 r127 (fast with 0x765c0/0x75910/0x76520, all MATCH; control 356): local_6 declared ahead of local_26 356
// (other code: the two NONE stores swap); local_7 declared ahead of the two NONE locals 356 (same code); local_10
// declared and then assigned from 0x76520 356 (same code); `this` used directly in place of the arg_0 copy 356 (same
// code); local_2 declared ahead of the channel search 399 (1372 B). Body unchanged.
// M3 lane 14 r116 (fast with 0x75910, 0x76520, 0x765c0, 0x77940 and 0x89030, all MATCH; control 356): the elapsed count
// taken from the 64-bit snapshot 426 (1385 B); `local_7 = false` declared ahead of the search (a constant start) 409
// (1373 B); the send test with the has-host term first 419 (1356 B); diagnostic, the has-host flag started false and set
// in the call's arm (match-only) 402 (1376 B); the reliable window passed straight into the ratio 356 (same code).
// 0x75910's byte result stays BLOCKED-BY 0x75910 (its file was another lane's this round).
// M3 lane 5 r208 (fast with 0x75910, 0x76520, 0x765c0, 0x77940, all MATCH each run; control 356, difflib-aligned 195).
// Nothing kept. The channel's first flag read at each of its five uses, no local copy, 429 (1388 B: retail's two
// separate tests for the two budget picks, but each reloads the flag where retail tests BL); the forced-send store
// written between the two budget picks 442 (1392 B), ahead of both 356 (same code); the secondary output flag as a
// copy of the forcing flag at its one set 416 (1356 B; frame 0x34 as retail, registers as before); 0x75910 returning
// `bool` (`return owner && owner->active;`, its declaration here to match: its file was free this round and 0x75910
// still matches) 355, retail's `test al, al`, one point, not kept, so the BLOCKED-BY 0x75910 item is now only that one
// point. Reading of retail's exits: the `rate == 0` test sits out of line just ahead of the shared exit and falls
// into it; the first threshold is `setge; test; je` with no result store and the second a plain `jl`; the
// reliable-bytes failure stores `false` in two separate arms; the `budget < 0` exit is a literal `xor al, al` return
// of its own. The reliable-bytes failure as two nested tests with `local_2 = false` in each failing arm (retail's two
// stores) 402 at 1369 B, retail's length, aligned 195 as the control (a higher count, not kept); the same with the
// failing arms first 413 (1380 B). The byte homes differ only in the last pair (retail: the secondary output flag +0x16, the packet flag
// +0x17, which it also keeps in BL; ours the reverse).
// M3 lane 13 r122 (fast with 0x75910/0x76520/0x765c0/0x77940/0x89030, all MATCH each time; control 356, aligned 194):
// `bool local_16 = false;` declared beside `local_7` ahead of the has-host call (a constant initialiser moved up; retail
// stores two false bytes from a zero register there) 353 (1370 bytes: `xor ebx, ebx`, `cmp [eax], ebx` in the search and
// the two `bl` stores appear as retail's, the index in EBP); with 0x75910 returning bool (round 136's held form) and the
// has-host arm as `arg_3 = arg_4 = true` 352; 0x75910 bool alone re-measured 355, with the chained arm 354; the two NONE
// locals ahead of the found test on the bool callee 406 (1371 bytes); on the 352 form every channel field read as
// `((s_75fc0 *)&arg_0->channels[local_0])->` with no pointer local 436 (1389 bytes), the pacing test as `!(local_17 > 0
// && 1 / local_22 > local_17)` 352 (aligned 189), the same on the 353 form 353 (aligned 190). Left on 352: NONE in ESI and
// `this` in EDI at entry, the channel base `this + index * 0x528` (0xa8 folded into every field), the 0x76520 result in
// BL, the window size in EDI, the counter snapshot as two plain loads, and the has-host flag set in its arm (match-only).
// M3 lane 1 r132 (fast with 0x75910/0x76520/0x765c0/0x77940, all MATCH; control 356): retail's snapshot is two plain
// loads of the counter and its reset stores the low word it already loaded for the elapsed count and then the high word
// (no composition). Through the 64-bit view `*(__int64 *)&g_4e6398` (src/unknown_075870.cpp's spelling): snapshot and
// reset 429 (1330 B, frame 0x30), the snapshot only 431 (1341 B), the reset only 419 (1356 B): with retail's plain copies
// the body is 39 bytes short, so the composition is hiding missing code elsewhere. Re-measured round 122's
// `bool local_16 = false;` beside local_7 353 (1370 B, a declaration move, three points); with the 64-bit reset 384.
// M3 lane 5 r215 (fast with 0x75910, 0x76520, 0x765c0, 0x77940, all MATCH each time; control 356, aligned 245): the payload
// floor nested under one sign test (`if (local_6 >= 0) { local_33 = local_6 - local_30; if (local_33 <= 48) local_33 = 48; }
// else local_33 = 0x600;`, retail's single `test edi, edi; jl` with the 0x600 arm last) 402 at 1369 bytes, retail's
// length (aligned 240, 411 lines: the tail is then retail's, but every later offset moves against the head's 0x38 frame);
// the same with 0x600 as the initialiser and no else 402 (1369 B, aligned 245); with the floor as a ternary 402 (same code
// as the first); with the failing branch first 402 (aligned 245); in place on a copy of local_6 402 (same code as the
// first). Diagnostics, reverted at once (an unreachable call given a local's address keeps it memory-homed; not a keepable
// form): the connection so 401 at 1369 bytes; the channel pointer so 480 (1419 B); the channel index so 352 (1373 B); the
// index and the connection 410 (1359 B). None gives retail's entry (NONE in ESI, `this` in EDI, the index in EBP), so the
// entry's register split is not a memory-homed local.
// M3 lane 6 r212 (fast with 0x75910 MATCH in every form; control 356). Reading: retail's two zero-register byte stores
// ahead of the has-host call are local_7 and local_8 (local_16's false comes later with the -1.0 default), so the
// has-host flag starts false and is set in the call's arm (match-only); the counter snapshot is a plain 8-byte copy that
// the later low/high reads do not reuse, where ours composes it into EBX:EBP up to the reset. 0x75910 returning bool with
// local_16 declared beside local_7 and the chained has-host arm 352 (four points, not kept); with the flag form 403; the
// flag form alone on the bool callee 403; the snapshot and reset through `*(__int64 *)&g_4e6398` 429 (1330 B), with the
// flag form 419; the flag form with the connection read through link_connection_get at each use 397.
// M3 lane 1 r140 (fast with 0x75910, 0x76520, 0x765c0, 0x77940, all MATCH in every run; control 356, aligned 240 lines, 139 with
// registers and offsets masked). Reading: retail's two NONE stores (+0x30, +0x24) and two false bytes (+0x16, +0x13) sit exactly
// where ESI and EBX are given to the channel base and the has-host argument, so they are spills of values held since entry (NONE
// in ESI, zero in EBX): local_26, local_6, local_7 and the has-host flag are set ahead of the channel search, and the flag is set
// again in the call's arm. Grid of those three choices (NONE locals first / local_7 first / the flag started false and set in the
// arm): 358 / 409 / 402, NONE + local_7 353 (three points, a declaration move), NONE + flag 410, local_7 + flag 389, all three 398
// (entry then has `or esi, -1`, `xor ebx, ebx`, `mov ebp, esi`, `cmp [eax], ebx`, `cmp ebp, esi` as retail, but the four values
// are stored at once and `this` is not in EDI), all three with the flag declared after the search 410. On all three: every field
// through `arg_0->channels[local_0]` with no pointer local 447 (1393 B); the snapshot and the reset as copies through the file's
// counter view 421 (1342 B); both 425 (1357 B, frame 0x34, first difference +0x3, aligned 166 [98]: the nearest form by
// alignment, left are the NONE/index roles of ESI and EBP, the four early stores, the channel base formed at the first field read
// where retail forms `this + index * 0x528` right after the connection, and the 64-bit window update); NONE + local_7 without the
// flag and both 419 (1353 B, aligned 161); the counter view alone 429 (1330 B, re-measured). Body unchanged.
// M3 lane 13 r132 (fast with 0x75910/0x76520/0x765c0/0x77940/0x89030, all MATCH each time; control 356, 1374 B; aligned =
// differing lines of a sequence alignment with stack offsets masked). A rewrite from retail's flow, read block by block:
// every channel field through `arg_0->channels[local_0]` (a file-local view of the element, no pointer local), the
// counter snapshot and reset as plain copies through the existing counter view, the 64-bit steps on that view, the
// has-host flag started false and set in the call's arm (match-only), the pacing test as `!(rate > 0 && 1 / limit > rate)`,
// the first threshold as `if (!(elapsed >= interval)) goto done` with no result store, the second as `if (payload < 48)
// goto done`, the reliable-bytes test nested with the payload named and `local_2 = false` in each failing arm, the
// payload floor under one sign test, and three exits (`return local_2` in the budget block, `return false` after it,
// `done`): 427 at 1337 B, aligned 134 (`this` in EDI with its home written at entry and the whole middle from the
// has-host call to the thresholds line for line as retail but for slots; the index takes ESI where retail keeps NONE
// there, and the connection takes EBP where retail keeps it in memory). On it: the two NONE locals ahead of the search
// 412 (1339 B: NONE in EBP and the index in ESI, retail the reverse, and both NONE stores ahead of the loop); only the
// quotient's NONE local ahead 415 (1348 B); the observer copy made after the search 430; a pointer to the channel's
// counter declared after the connection and used at its five sites 434 (1335 B: frame 0x34 and the channel base
// `this + index * 0x528` formed ahead of the has-host call, both as retail, but `this` then lives only in memory and
// the index takes EDI); all 24 orders of the connection, that pointer, the two NONE locals and the two false flags
// between the found test and the has-host call 397 with the pointer first (1337 B, aligned 132), 400 or 434 otherwise,
// and 425 to 427 for the six orders without the pointer; NONE locals ahead of the search with the pointer 424 (1343 B:
// NONE in ESI and frame 0x34 as retail), with the connection ahead of the pointer 393 (1339 B). Left on the best forms:
// retail gives `this` EDI and the index EBP up to the has-host call and no register to the connection (ours: memory,
// EDI, EBP), stores the two NONE locals after the found test from the NONE register, loads the connection index into
// ECX after the pushes, and returns the first `false` from AL; 0x75910's byte result as before. Body unchanged at 356.
// MiniMax C (11-17): no new form, no lower score (356): retail's prologue is sub esp, 0x34 with the connection in edi, the NONE state in esi and the channel index in ebp, where this build reserves 0x38 and splits the register saves differently, and the timed-out check's result is tested as a byte in retail (test al, al) where this build's callee returns a long; that callee, 0x75910, is defined in another file, so the call's shape cannot be settled here, and dropping the redundant host-state copy reaches retail's frame but scores 416.
// MiniMax D (11-17): no new form, no lower score (356): the prologue is still sub esp, 0x38 here against retail's 0x34,
// with this build keeping the observer in ebp and the NONE state somewhere else where retail keeps the connection in edi,
// the NONE state in esi and the channel index in ebp; the only lower form a permuter run finds (351, one point under the
// 352 the image counts) swaps the arguments of the call to 0x765c0, which is a parameter order picked by score and not a
// gain, so the body stands; the timed-out check's result is still tested as a long here where retail tests it as a byte,
// and that callee (0x75910) is defined in another file, so the call's shape cannot be settled here.
// M3 lane 7 r205 (H2M fast with 0x75910, 0x76520 and 0x765c0, all MATCH each run; control 356 at 1374 B): the first
// threshold as `if (local_23 < local_13) goto done;` with no result store 409 (1369 B); the second as `if (local_6 < 48)
// goto done;` 413 (1374 B); both 414 (1356 B); the two later reads of the channel's first flag through local_15 (one
// read everywhere) 400 (1372 B); the tick count as an if/else of the global and the call 356 (same code). Body unchanged.
// M3 lane 16 r167 (H2M fast with 0x75910/0x76520/0x765c0/0x77940, all matched unless noted; control 356; body unchanged):
// 0x75910 declared and defined as returning bool 355 (the caller's `test al, al` is then retail's, but 0x75910 itself
// becomes `mov al, 1` / `xor al, al` where retail writes EAX, 7: retail's callee is 32-bit, so the byte test is not
// its return type, restored); the counter snapshot and the reset as plain copies through the 64-bit view 429 (1330 B,
// re-measured: with them retail's region from the has-host call to the thresholds is longer than ours, see above);
// `this` used directly with no `arg_0` alias 356 (same code); the has-host arm as `arg_3 = arg_4 = true;` 355 (one
// point, not kept); the channel's first flag read from its field at every use, no local, 429 (1388 B).
// M3 lane 14 r141 (H2M_FAST with 0x75910/0x76520/0x765c0/0x77940, all MATCH each run; control 356, aligned 245): the
// channel pointer declared ahead of the connection 356 (same code); local_18 and local_13 declared with the two early NONE
// locals 405 (1355 B); local_15 read ahead of local_14 368 (1378 B); local_5 declared with the two early NONE locals 398
// (1376 B); the counter snapshot as an aggregate copy of the counter read through a 64-bit view at its two uses 431
// (1341 B). Body unchanged.
// M3 lane 7 r210 (h2m fast with 0x75910, 0x76520, 0x765c0 and 0x77940, all MATCH each run; control 356 at 1374 B): the
// zero-rate exit as `return local_2;` in place of its goto 392 at retail's 1369 B (aligned with registers and offsets
// masked 153 against the control's 148); the connection pointer declared ahead of the channel search (nothing there can
// change it) 409 (1370 B); `bool local_2 = false;` declared ahead of the search 399 (1372 B); the pacing test as
// `!(local_17 > 0.0f && 1.0f / local_22 > local_17)` 356 (same count); local_5's NONE declared beside local_18 403
// (1376 B). Re-measured: the snapshot and reset through the 64-bit view 429 (1330 B). Body unchanged.
// M3 lane 4 r253 (h2m fast with 0x75910, 0x76520, 0x765c0 and 0x77940, all MATCH each run; control 356 at 1374 B;
// aligned = difflib count of unmatched instructions with addresses, registers and stack offsets masked [mnemonic only],
// control 98 [57]): snapshot and reset through the 64-bit view with the zero-rate exit as `return local_2;` 393 (1354 B)
// [99, 49]; the has-host result started false and set in the call's arm (retail stores the zero at entry and the 1 in the
// arm, match-only flag form) 402 (1376 B) [101, 58]; the three together 355 (1372 B, frame 0x30 for retail's 0x34) [82,
// 46]; on that, the second threshold as `if (local_6 < 48) goto done;` 401 (1373 B) [86, 51], the first as `if (!(local_23
// >= local_13)) goto done;` 360 (1369 B) [80, 46]. Reading: retail's done block (`mov al, [esp + 0x48]` and the first
// epilogue) sits after the capacity arm with the zero-rate test falling into it, so the zero-rate exit is a goto there,
// and retail's local_2 is never stored by the first threshold (setge/test straight into the jump). Body unchanged.
// M3 lane 5 r240 (H2M_FAST with 0x75910, 0x76520, 0x765c0 and 0x77940, all MATCH each run; control 356): the window
// query's result read through a `bool const &` at both tests 409 (1378 B); the channel pointer declared `const` 356 (same
// code); the send test written `!local_8 && (callback || window)` 419 (1356 B); the connection pointer bound by a const
// reference 409 (1378 B). Diagnostic, reverted at once: the owner query's result cast to a byte at its test 355 (only
// the `test al, al` row: retail truncates the 32-bit result there). Body unchanged.
// M3 lane 6 r239 (fast with 0x75910/0x76520/0x765c0/0x77940, all MATCH; control 356, aligned 195): the payload floor nested
// under one sign test with local_16 beside local_7 405 (1369 B), with the zero-rate exit as `return local_2;` 404 (1355 B),
// with both 405; lane 4 r253's three pieces re-measured 356 (1372 B), with the nested floor 350 at 1369 B (aligned 177:
// `this` EDI, zero EBX and the index EBP as retail, frame 0x30 for 0x34; a lead, it needs the match-only flag form), with
// local_16 moved too 368; that 350 form with every channel field read through `channels[local_0]` 417 (1381 B). Body unchanged.
// Counter record or separate words 411/411, explicit signed-high and unsigned-low comparison 415/415,
// arithmetic composition 436, elapsed low shared with reset 410, with record 411, with explicit comparison
// 415. Explicit comparison with early budget sentinels, output flag, both, or both plus host flag
// 444/399/439/434. NOT kept: the 0x34 frame and retail length return on the split-comparison forms, but the
// best at that extent is 399 and the NONE and zero homes still differ.
// M3 lane 14 r152 (h2m fast with 0x75910, 0x76520, 0x765c0, 0x77940 and, where said, 0x89030, all MATCH each run; control 356;
// aligned = rows of a sequence alignment with stack offsets masked, control 203; a per-register comparison of retail's
// value ranges with ours: 34 of 118 placed differently). Lane 6 r239's lead rebuilt on today's body (the has-host flag
// started false and set in the call's arm, snapshot and reset through the 64-bit view, the zero-rate exit as `return
// local_2;`, the payload floor nested under one sign test) 349 at 1369 B, aligned 142, 24 of 125 ranges placed
// differently (`this` EDI, zero EBX and the index EBP as retail; left: EAX/ECX swapped from +0x1e, NONE not held in
// ESI, two EBP ranges, every stack cell 4 lower with the frame at 0x30). On that lead: the two NONE locals ahead of the
// search 335 (1370 B, aligned 141); those and local_7 ahead 355 (1369 B, aligned 147); every channel field through
// `channels[local_0]` with no pointer local 417 (1381 B), with the NONE locals ahead 433, with local_7 too 423. New
// evidence for the two budget picks (retail tests BL twice in a row, `test bl, bl` ahead of each pick, where ours
// folds the two picks into one test): with the second pick's condition read from the channel's field instead of
// local_15 (`local_5 = local_4->field_48c ? ... : ...`, 0x89030 in the image: the getter is known not to write, so the
// re-read shares BL) the pick rows are retail's row for row; on the lead 354 (1378 B, aligned 140), on the 335 form
// 414 (1380 B, aligned 140), on this body 438 (1388 B, aligned 191, either pick; the listed flag-at-each-pick count).
// A lead only: a second read of a field a local already holds is match-only. Read from the same alignment: retail's
// first threshold is `setge; test; je` with no store of the result and its second a plain `jl`; the reliable-bytes
// payload is formed early (`lea ebp, [ecx - 0x2d]`) with `false` stored in each failing arm; the channel's first
// flag is read from memory again at both later uses, as here. Body unchanged.
// In round 641, the frame and search register assignments stayed off retail. Const lookup channel 356; skip inactive
// channels 356; pointer walk and index on hit 460; short search ordinal 436; nested lookup tests 356; shared clock
// helper 356; observer and lookup names 356; budget and send-result names 356. The body and lookup were restored.
// M3 lane 2 r148 (screens with 0x75910, 0x76520, 0x765c0, 0x77940, 0x89030, 0x88fd0 and 0x89000 in the image, all MATCH each
// run; control 356 at 1374 B, aligned 195, 98 with registers and offsets masked; body unchanged). New reading: retail's whole
// entry (NONE in ESI, zero in EBX, `this` in EDI, the index in EBP, the result's `xor al, al`, the two NONE stores from ESI and
// then the channel base into ESI) comes out when the channel pointer is declared after the two NONE locals on the has-host flag
// form (the base is then formed after NONE's last store, so both take ESI). Parts: F = has-host flag started false and set in
// the call's arm (match-only), V = snapshot and reset through the 64-bit view, R = zero-rate exit as `return local_2;`, N =
// payload floor under one sign test, P = the pointer after the NONE locals (a declaration move, match-only). FVN 424 (1340 B),
// FV 420 (1344 B), FN 394 (1370 B), VN 433 (1326 B), FVNP 402 (1338 B, aligned 163 [70]: the first 37 rows are retail's but for
// the 0x30 frame and the 0xa8 in the base), FVRNP 347 (1369 B), FP 404 (1376 B), P alone 406 (1371 B). On FVN every field through
// an element view with no pointer local 405 (1353 B, aligned 116 [48]: frame 0x34, but NONE/index in EBP/ESI and the base formed
// at the first field read). On FVNP that view plus a pointer to the 64-bit counter declared after the NONE locals and used at its
// sites 389 (1344 B, aligned 149 [42]: every row up to the has-host call is retail's, `lea esi, [eax + edi]` included, with the
// frame at 0x30 because the has-host flag takes CL and its byte is reused for local_14, where retail reads it from memory at
// each use). On that form: the pacing test as `!(local_17 > 0 && 1 / local_22 > local_17)` 389 (aligned 145); the
// reliable-bytes failure as nested tests with `local_2 = false` in each failing arm 394 (1341 B); the second pick's flag read
// from the field 389 (same code); the first threshold as `if (local_23 < local_13) goto done;` (or a named bool, same code)
// with the second as `if (local_6 < 48) goto done;`, the pacing test and the nested failure arms 336 (1343 B, aligned 137
// [45]; LEAD, build/r148/lead-0x75fc0-336.patch, not kept: F and P are match-only); on the lead the second threshold with an
// explicit `local_2 = false;` 352 (1341 B), the first threshold kept as now with that 383 (1346 B), the named bool alone 399
// (1336 B), the snapshot as a copy into the existing counter view 336 (same code), the reset as the composition 380 (1354 B).
// Diagnostics only (the second threshold without a store while the first still assigns local_2 returns another value): 374 and,
// with the pacing test and failure arms, 368, both at 1369 B. The lead without F: with P 432 (1329 B), without P 433; with F
// and no P 367 (1345 B); with F, P and no view 417 (1369 B); with R 379 (1355 B). Left on the lead: the counter's low word is
// kept in EBP from the snapshot to the elapsed count (retail reads the global again there and gives EBP to local_18), the
// has-host flag in CL, retail's `setge; test` at the first threshold, its two unmerged `false` stores and the zero-rate test
// placed ahead of the shared exit; 26 bytes short. The channel's first flag read from its field at every use, measured with the
// three getters in the image, 429 (1388 B: each use still reloads). Diagnostics, restored at once: `inline` on the three getters'
// definitions expands all three here (494, 1442 B); an undefined call after the window getter 486 (on the lead 384) spills the
// float registers retail keeps across that call, so retail has the getters' register and memory summaries as this build does.
// M3 lane GA r8 (fast with 0x75910, 0x76520, 0x765c0, 0x77940, 0x89030, 0x88fd0 and 0x89000, all MATCH each run; control
// 356 at 1374 B, aligned 192, register-and-offset-masked 123; body unchanged). New structural reading: retail's entire
// second half (from `if (local_14)` through the three tail epilogues) comes out verbatim from zero-goto structured C++:
// `bool can_send = true;` with `if (local_17 > 0.0f) { ... can_send = local_23 >= local_13; } else if (local_17 == 0.0f)
// can_send = false;`, `if (can_send && local_18 >= 0) { ... if (local_6 < 48) can_send = false; }`, `if (can_send)` around
// the counter update and reliable-bytes check (with `long payload = local_29 - 45;` and two `local_2 = false;` failure
// arms), `if (local_2)` around the packet/tail work, nested `local_6 >= 0` payload floor, `*(__int64 *)&g_4e6398`
// snapshot/reset, `!(local_17 > 0.0f && 1.0f / local_22 > local_17)` pacing test, and every channel field indexed through
// `channels[local_0]` (no pointer local): 367 at retail's exact 1369 B, aligned 114, masked 51 (100% clean, no match-only
// forms; recovers both `test bl, bl` budget picks, `setge; test; je` with no result store, the out-of-line `ucomiss`
// falling into the shared exit, `lea ebp, [ecx - 0x2d]` with two unmerged false stores, and all three epilogues). Trials
// on that zero-goto shape: original counter composition with `local_4` pointer 400 (1389 B, masked 84), nested `if (can_send)`
// 400; 64-bit snapshot reused in elapsed/reset 442 (1331 B, frame 0x34); `*(__int64 *)` view with `local_4` pointer 440
// (nested 440, union copy 440, 64-bit subtract 440); reusing `local_2` instead of `can_send` 395 (1369 B, frame 0x34);
// indexed fields with original pacing 367 (1369 B, masked 53), with `local_16 = false` beside `local_7` 421 (1342 B, frame
// 0x34), plus NONE locals before search 378 (1369 B, masked 47); tail `return local_2; / return false;` 437 (with early
// `local_16` 440); `local_3` fetched after the NONE/zero locals 367 (masked 51); counter pointer after NONE 430 (1336 B,
// frame 0x34, +0x7). Match-only diagnostics (`F` = has-host flag started false and set in arm): indexed + `F` 384 (1373 B,
// aligned 104, masked 43); + `F` + `local_3` after NONE 361 (1371 B, aligned 104, masked 39); + `F` + NONE before search
// 412 (1373 B, frame 0x34, `local_3` spilled); + `F` + tail returns 418; + `F` + `local_4` after NONE 404 (1369 B, frame
// 0x34, aligned 101); + `F` + counter pointer after NONE 405 (nested reused snap 406; with NONE before search 415, +0x9
// matching `sub esp, 0x34; push ebx; push ebp; push esi; or esi, -1`). Left on the 367/361 zero-goto forms: `local_3` takes
// EBP instead of spilling to `[esp+0x1c]` (forcing `local_18` to `[esp+0x20]` and frame 0x38, and delaying `this + index *
// 0x528` until `field_48c` instead of right after the NONE stores), plus 0x75910's `long` return. Body unchanged.
// @retail 0x75fc0
bool s_network_observer::connection_v05(long arg_1,
	bool arg_2, bool arg_3, bool arg_4, bool *arg_5, bool *arg_6,
	long *arg_7, long *arg_8, long arg_9, byte *arg_10)
{
	s_network_observer *arg_0 = this;
	long local_0 = function_75fc3(arg_0, arg_1);
	bool local_2 = false;
	if (local_0 == NONE)
		goto done;
	s_link_connection *local_3 = link_connection_get(arg_1);
	s_75fc0 *local_4 = (s_75fc0 *)&arg_0->channels[local_0];
	long local_26 = NONE;
	long local_6 = NONE;
	bool local_7 = false;
	bool local_8 = network_observer_channel_has_host(arg_0, local_0);
	if (local_8)
	{
		arg_3 = true;
		arg_4 = true;
	}
	long local_9;
	bool local_10 = function_76520(local_0, false, &local_9);
	long local_11 = g_510548 ? g_51054c : GetTickCount();
	__int64 counter_snapshot = (__int64)(((unsigned __int64)g_4e6398.high << 32) | (dword)g_4e6398.low);
	long local_18 = NONE;
	long local_13 = NONE;
	bool local_14 = (local_3->callback || local_10) && !local_8;
	bool local_15 = local_4->field_48c;
	bool local_16 = false;
	real local_17 = -1.0f;
	long local_5 = NONE;
	if (local_15 && !local_8)
	{
		if (local_4->field_48d && local_4->field_4dc == 2)
		{
			local_16 = true;
			local_14 = true;
		}
		else if (local_4->field_48e)
			local_16 = true;
	}
	if (!local_8)
	{
		if (local_14)
		{
			if (local_15)
				local_17 = local_4->field_49c;
			else if (local_3->callback)
				local_17 = local_4->field_484;
			else
				local_17 = g_4cf5d0;
		}
		else if (!arg_2)
			local_17 = arg_3 ? 4.0f : 0.0f;
	}
	if (local_3->state == 5 && !local_8)
	{
		long local_19 = 0;
		if (local_3->flags & 8)
			local_19 = 128;
		long local_20 = link_connection_get_reliable_window(local_3);
		real local_21 = (real)local_20 /
			((real)local_19 * *(real *)((byte *)arg_0->configuration + 0x90));
		if (local_21 < 1.0f)
		{
			real local_22 = *(real *)((byte *)arg_0->configuration + 0x94) +
				(1.0f - local_21) * (*(real *)((byte *)arg_0->configuration + 0x98) -
				*(real *)((byte *)arg_0->configuration + 0x94));
			if (local_17 <= 0.0f || 1.0f / local_22 <= local_17)
				local_17 = 1.0f / local_22;
		}
	}
	if (local_14)
	{
		local_18 = local_15 ? local_4->field_494 : local_4->field_480;
		local_5 = local_15 ? local_4->field_498 : local_4->field_488;
		arg_4 = true;
	}
	long local_23 = g_4e6398.low - (long)local_4->field_478;
	long local_24 = g_485ac0;
	if (local_24 <= 0)
		local_24 = 60;
	if (local_23 < 0)
	{
		local_4->field_478 = (__int64)(((unsigned __int64)g_4e6398.high << 32) | (dword)g_4e6398.low);
		local_4->field_470 = local_11;
		local_23 = 0;
	}
	if (local_17 > 0.0f)
	{
		local_13 = function_75fc1((real)local_24 / local_17);
		local_2 = local_23 >= local_13;
		if (!local_2)
			goto done;
	}
	else if (local_17 == 0.0f)
		goto done;
	if (local_18 >= 0)
	{
		local_26 = (local_23 * local_18 / local_24) / 8;
		local_6 = local_26 - 45;
		local_2 = local_6 >= 48;
		if (!local_2)
			goto done;
	}
	if (local_13 >= 0)
	{
		local_4->field_470 = local_11;
		local_4->field_478 += local_13;
		if (local_4->field_478 + local_13 <= counter_snapshot)
			local_4->field_478 = counter_snapshot;
	}
	local_2 = true;
	if (local_16)
		local_7 = true;
	if (local_5 >= 0 && local_26 >= 0)
	{
		long local_27 = link_connection_get_reliable_bytes(local_3);
		long local_28 = link_connection_get_reliable_pending(local_3);
		if (local_28 > 0 && local_27 + local_26 > local_5)
		{
			long local_29 = local_5 - local_27;
			local_2 = local_29 >= local_27 / local_28 && local_29 - 45 >= 48;
			if (local_2)
				local_6 = local_29 - 45;
			if (local_4->field_48c)
				local_4->field_4a2 = true;
			if (!local_2)
				goto done;
		}
	}
	long local_30;
	if (local_10)
	{
		real local_31 = 1.0f;
		if (arg_2)
		{
			if (local_3->callback)
			{
				if (link_connection_owner_active(local_3))
					local_31 = *(real *)((byte *)arg_0->configuration + 0xe4);
				else
					local_31 = *(real *)((byte *)arg_0->configuration + 0xe8);
			}
			else
				local_31 = *(real *)((byte *)arg_0->configuration + 0xec);
		}
		long local_32 = local_6 >= 0 ? function_75fc1((real)local_6 * local_31) : 512;
		if (local_32 > arg_9)
			local_32 = arg_9;
		if (local_4->field_48c && local_32 < local_9)
			local_4->field_4a1 = true;
		local_30 = function_765c0(local_0, local_32, arg_10);
	}
	else
		local_30 = 0;
	long local_33 = local_6 >= 0 ? local_6 - local_30 : 0x600;
	if (local_6 >= 0 && local_33 <= 48)
		local_33 = 48;
	long local_34 = 0x516 - local_30;
	local_34 -= local_34 % 8;
	if (local_34 >= 0)
	{
		if (local_33 > 8)
			local_33 -= local_33 % 8;
		if (local_33 > local_34)
			local_33 = local_34;
		*arg_7 = local_33;
		*arg_8 = local_30;
		*arg_5 = arg_4;
		*arg_6 = local_7;
	}
	else
		local_2 = false;
done:
	return local_2;
}
