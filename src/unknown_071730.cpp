// @flags /O2 /Ob1 /Gr
#include "unknown_11c920.h"
#include "unknown_058dd0.h"
#include "unknown_0662e0.h"
#include "globals.h"
#include "unknown_08b110.h"
#include <xtl.h>
#include <string.h>

struct s_surface_description;
struct s_matchmaking_ratings;
struct s_match_player_list;
struct s_session_property_value
{
	long kind;
	long value04;
	long value08;
	long value0c;
	long value10;
	long value14;
	long value18;
};
struct s_71730
{
	long field_0[3];
	long field_c;
	long field_10;
	byte field_14[0xc0];
	long field_d4[16];
	long field_114[16];
	long field_154[16];
	long field_194[16];
	byte field_1d4[0x358 - 0x1d4];
	long field_358;
	dword field_35c;
	dword field_360;
};

void function_71e00(c_session_state *arg_0);
s_surface_description *function_192e60(long arg_0);
long function_193400(s_surface_description *arg_0);
long function_1931d0(s_surface_description *arg_0, long arg_1);
bool function_1934b0(s_surface_description *arg_0);
long function_1933a0(s_surface_description *arg_0);
long function_1933d0(s_surface_description *arg_0);
bool function_1934d0(s_surface_description *arg_0);
long function_193370(s_surface_description *arg_0);
long function_193440(s_surface_description *arg_0);
bool function_193470(s_surface_description *arg_0);
bool function_7e210(c_class_58d20 *arg_0, s_matchmaking_ratings *arg_1);
bool network_session_players_match(c_class_58d20 *arg_0, c_class_58d20 *arg_1);
long network_session_get_maximum_players(c_class_58d20 *arg_0);
long network_session_find_player(c_class_58d20 *arg_0, const dword *arg_1);
bool network_session_members_ready(c_class_58d20 *arg_0, dword *arg_1);
bool function_73ae0(unsigned __int64 *arg_0);
bool network_session_host_set_id49f0(c_class_58d20 *arg_0, const s_session_id *arg_1);
bool network_session_host_set_data49cc(c_class_58d20 *arg_0, const dword *arg_1);
void session_property_value_set_kind2(s_session_property_value *arg_0, long arg_1);
void session_property_value_set_kind3(s_session_property_value *arg_0, long arg_1);
bool function_72140(c_session_state_matchmaking *arg_0);
bool balance_teams_by_count(long arg_0, long const *arg_1, long arg_2, long arg_3,
	bool arg_4, long arg_5, long arg_6, bool arg_7, long *arg_8);
bool balance_teams(long arg_0, long arg_1, bool arg_2, long arg_3, long arg_4,
	bool arg_5, long arg_6, long const *arg_7, long const *arg_8, bool arg_9, long *arg_10);
void function_71ff0(c_session_state_matchmaking *arg_0, long arg_1,
	const s_match_player_list *arg_2, const long *arg_3);

// score 376 (was 376; r239 unchanged; 387 before round 127): The secondary state lives in ESI rather than EAX, both dead state stores share a slot, and the existing data getter expands at its call site.
// MiniMax G (11-20): no new form, no lower score (376): retail calls 0x5b080 out of line for the session data where this
// build expands it at the call site, and the second session state still sits in ESI where retail keeps it in EAX; the
// 0x5b080 body lives in another file, so the two paths diverge from the first difference on. BLOCKED-BY 0x5b080.
// MiniMax H (11-20): no new form, no lower score (376): retail calls 0x5b080 out of line where this build expands get_data_4a00; the second session state sits in ESI against retail's EAX.
// MiniMax E (11-20): no new form, no lower score (376): the secondary state still lives in esi where retail keeps it in eax, because 0x5b080 is expanded here and called out of line in retail; 0x5b080 lives in src/unknown_05b040.cpp, which this lane may not edit, so the difference is not reachable from this file.
// Earlier at 376 (was 387): The secondary state lives in ESI rather than EAX, both dead state stores share a slot, and the existing data getter expands at its call site.
// MiniMax E (11-20): no new form, no lower score (376): the top block still reads the session state into ESI and compares ESI where retail loads it into EAX and compares EAX, and retail's out-of-line call to 0x5b080 for the session data is expanded at the call site in this build, so the two paths diverge from the first difference at 0x6b on.
// Round 127 (m3 lane 10; fast, kept form confirmed plain with 0x706f0 and 0x5b080 matched): the property value cleared by
// memset, as the members-ready arm already clears it, 387 -> 376 (kept: retail's seven zero stores are all `mov [..], ecx`
// from one zero register, where `= {0}` stored the first member as an immediate; 1589 bytes, retail's length; aligned 115 ->
// 113). On it: that clear declared ahead of the player-count test, where retail's stores sit, 376 (aligned 111; a declaration
// move, not kept); `= {0}` there 402. On 387: the elapsed-time read before the data getter 397; the two missing-input tests as
// one `||` 412 (again); on 376 the members-ready arm through the existing kind-4 setter (0x59ab0) 437 (1565 bytes: it
// stays a call here, where retail expands its six zero stores and kind store). Left: 0x5b080 expanded at the top where retail calls it, the second live-state test called where
// retail expands it, and the `(deadline - elapsed) / 1000` block placed after the members-ready test.
// Round 199 (m3 lane 5, fast): the data getter called only when the surface lookup succeeded 428 (1573 bytes; the getter stays
// inlined, so a natural test ahead of it is not enough); the getter called before 0x192e60 429 (diagnostic); the two
// missing-input tests as one `||` 412 (re-measured); the deadline sum with the record's field first 387 and the transition
// flag declared before the reset flag 387 (both the same code).
// Earlier 387 (was 426).
// MiniMax I (11-17): no new form, no lower score (387): retail calls 0x5b080 out of line where this build inlines it (that body lives in another file), keeps the second session state in EAX against our ESI, and writes the two dead state stores to +0x20 and +0x18 where this build shares one slot, so the second dispatch block's value store lands before the pops here and after them in retail and every later branch target differs; the frame is retail's 0x3e4 but the body is 8 bytes longer.
// MiniMax B (11-03): no new form, no lower score. The frame stays 0x3e8 against retail's 0x3e4 and the
// state and result bytes keep their homes; retail still calls 0x5b080 out of line at this site, so the
// remaining gap needs a noinline on the existing getter, whose declaration is in a header this lane
// may not edit.
// MiniMax D (11-05): the transition flag as a bool makes our frame 0x3e4 (retail's) and moves the first
// difference to +0x27, but the result byte home stays at 0x0e against retail's 0x0f: 429; the balance flag
// as a bool gives 420, a gain under 8. No lower score.
// MiniMax E (11-06), not kept: dropping the record's field 0x35c (record 0x360 bytes, frame retail's
// 0x3e4) with the balance flag as a bool gives 416 (from 426), but field_360 then sits at 0x35c while
// retail reads the record at +0x358 (0x718f7) and +0x360 (0x7196d), 8 bytes apart, so the field stays.
// Retail keeps three byte flags in the first three bytes of its frame (the ready byte, the reset byte and
// the result byte); the reset flag as a byte shrinks our frame to 0x3e0 and gives 445, and the record
// field 0x35c with a byte flag (441) and the mode-7 arm split into two ifs on that form (427) are worse.
// Earlier, at 426 (was 439): the two session-state tests now call the existing c_class_58d20::function_058d20
// (0x58d20), which gives retail's dead state stores; 1593 bytes against 1589, frame 0x3e8 versus 0x3e4, both
// dead stores share one slot (retail 0x20/0x18), and 0x5b080 is still inlined here.
// Earlier at 439: retail length; frame 0x3e8 versus 0x3e4, result byte and secondary-state/getter homes remain.
// Earlier: retail length; frame 0x3e8/0x3e4, result byte and getter/readiness handling differ.
// Round 30 (lane D): the permuter found no better form, so the body is unchanged. What still differs:
// retail calls 0x5b080 where this body expands it and expands the elapsed-time helpers 0x75870/0x75890
// where this body calls them, retail's frame is 0x3e4 against our 0x3e8, and the result byte and the
// getter/readiness handling are still the ones the rows below describe.
// Earlier at 439: retail length; frame 0x3e8/0x3e4, result byte and getter/readiness handling differ.
// retail length; frame 0x3e8 versus 0x3e4, result byte and secondary-state/getter homes remain.
// Earlier: retail length; frame 0x3e8/0x3e4, result byte and getter/readiness handling differ.
// Retail stores the tested session state to a dead stack slot before each early exit (the idiom
// 0x706f0 and 0x5c490 reproduce only with volatile locals), so every later row is shifted in the
// lockstep compare. Round 152, nested rewrite without gotos (existing value getter, live-state
// helper, ready = player match then cleared by each failing test, deadline block skipped on the
// success path): calling the /Ob1 time helpers 512; with file-local time copies and a noinline
// data getter 468 (frame, prologue and entry registers now retail's; 353 of 463 retail rows align
// against 268 here); mode-7 arm before a re-tested guard 470; owner progress pointer first 492;
// force-inline time copies 468. All restored.
// Refreshed first balance inputs 454; fallback balance inputs 466; both phases 513;
// native state-value getter 449; final transition-result union 449.
// frame 0x3e8 versus 0x3e4, result-byte home and getter boundary differ;
// output-scalar lifetime 449; type/first-state/second-state scopes 449; inline depth one 449; the five
// settings read into one-use locals first 443 (not kept: the locals only fix the argument order).
// Existing property setter 475; selected value result 451; unsigned reset/balance 449/449;
// balance test outcomes 449; readiness at outcome 450; reset at outcome 481; unsigned found count 449.
// Ratings count, group-size cursor, player-rating cursor, combined ratings inputs, readiness-output
// lifetime, native identifier, paired outputs and combined inputs/outputs all 449.
// player scan while 449; assignment while 449; paired counted whiles 449; capacity admission scopes 449; roster
// admission stages 449; countdown expression homes 449; valid input work scope 449; future deadline first 449.
// Native producer ratings type 449; signed reset/balance/both decision bytes 464/461/483; signed
// transition/advance bytes 468/449; native property clear 454; native ratings plus reset byte 464.
// All forms restored; frame 0x3e8 versus 0x3e4, secondary-state/getter boundary and result home
// remain.
// Complemented final/reset/both decisions 489/467/489; waiting transition decision 439 kept;
// with complemented final/reset/both 492/469/498; complemented balance 446, restored.
// On the waiting transition graph: reset bool/byte 469/469, short/word 442/442;
// balance bool/byte 458/458; paired bool/byte 476/476. All eight widths restored.
// Complementary waiting carrier signed byte/byte 439/439, short/word 450/450, long/dword 454/454; paired
// waiting/final short 450, paired long 443. All restored.
// Round 187 advance result carried through roster/count/member-readiness test outcomes,
// each nonempty combination 459; full combination with native reset byte 465.
// Exact-match-only decision forms; all restored to the complementary waiting graph.
// Existing lookup/ratings/transition inline declarations, all three pairs and all three
// 439 each; all three noinline declarations 439. Original declarations and body restored.
// Consistent live primary/secondary/both/owner receivers 462/463/468/470; primary,
// secondary or both with live owner 461/456/466; all with direct state tests 466.
// All restored; three combined forms report reduced-image callee caveats.
// retail length; frame 1000 versus 996, result byte home and second state/getter
// handling differ. Retail calls 0x5b080 (ours expands it), expands the elapsed-time helpers 0x75870/
// 0x75890 (the start time is loaded before the clock is read), stores each session state to the stack
// before the early exits, and keeps a ready byte at [esp+0x11] (from the player match, cleared by each
// failed check, later from 0x72140) and the reset flag as a byte at [esp+0x12].
// Refreshed first balance inputs 454; fallback balance inputs 466; both phases 513; native state-value
// getter 449; final transition-result union 449; output-scalar lifetime 449; type/first-state/
// second-state scopes 449; inline depth one 449; the five settings read into one-use locals first 443
// (not kept: the locals only fix the argument order); existing property setter 475; selected value
// result 451; unsigned reset/balance 449/449; balance test outcomes 449; readiness at outcome 450;
// reset at outcome 481; unsigned found count 449; ratings count, group-size cursor, player-rating
// cursor, combined ratings inputs, readiness-output lifetime, native identifier, paired outputs and
// combined inputs/outputs all 449; player scan while 449; assignment while 449; paired counted whiles
// 449; capacity admission scopes 449; roster admission stages 449; countdown expression homes 449;
// valid input work scope 449; future deadline first 449; native producer ratings type 449; signed
// reset/balance/both decision bytes 464/461/483; signed transition/advance bytes 468/449; native
// property clear 454; native ratings plus reset byte 464; complemented final/reset/both decisions
// 489/467/489; waiting transition decision 439 kept; with complemented final/reset/both 492/469/498;
// complemented balance 446; on the waiting transition graph reset bool/byte 469/469, short/word
// 442/442, balance bool/byte 458/458, paired bool/byte 476/476; complementary waiting carrier signed
// byte/byte 439/439, short/word 450/450, long/dword 454/454; paired waiting/final short 450, paired
// long 443.
// Round 93: noinline class declaration of 0x5b080 467 (frame 0x3e4; entry registers move); the state
// tests as switches 470 (range compares); the three time reads through file-local inline copies of
// 0x75870/0x75890 (as network_session_membership.cpp has them) 448 (start time loaded first, as
// retail); those with the noinline getter 486 (entry registers and frame now retail's, 1549 bytes);
// plus the reset flag as bool 495.
// Round 190: existing time helpers at first elapsed/growth stamp/both 494/494/508;
// second elapsed/first-and-second/growth-and-second/all 496/503/510/520;
// all with private force-inline declarations 520. Original timing expressions and declarations restored.
// Round 97: the existing value getter for the session index 439; noinline getter 467; with a
// file-local time-since helper 486 (1549 bytes: retail also clears a ready byte in every failed
// check); a ready byte from the player-match chain, cleared in each failure arm and set from
// 0x72140, 448 (1580 bytes); that with the noinline getter and time-since helper 461 (1573 bytes,
// frame, entry and failure stores now retail's; left: the property value and both flag bytes
// cleared before the player-count branch, a separate mode-7 exit per missing pointer).
// Round 369: ready/getter interaction 504; byte reset 511; early property 502; property/reset 511; separate missing-input exits 488, with reset/property/both 493/501/493. Match-only carrier forms, header and body restored.
// Round 235: ratings/assignment/admission consumption scopes, all seven combinations 439 each; readiness-output consumption scope 439.
// Round 201: signed/unsigned outstanding roster counts 463/463; missing counts 447/447;
// direct lookup-result accumulation 469/469; bool/byte all-found results 443/443 (exact-only).
// Original complete lookup count and signed zero/negative admission restored.
// Round 226: Property/balance defaults before count observation 457/439; both 457; each/both
// with Boolean reset 472/469/472; property with Boolean balance 462; all with both Booleans 479. All restored.
// Round 409: roster count at every use 439; actual identity-end traversal with captured/live count
// 450/450; those three with the shared live-state predicate 484/480/480; field-count/end
// traversal with native long final result 444/468. Original complete body restored.
// Round 189: plain strict/fallback balance loop, long ordinal with short/bool/long/byte
// result 580/591/588/591; short ordinal with those four results 580 each. Original
// attempts restored; the rolled loop shortens away from retail and leaves the frame excess.
// Round 214: final canonical 0/1 result storage on the complementary waiting graph:
// signed-byte/byte 465 each; short/word 449 each; int/unsigned-int/long/dword 444 each.
// Original bool result restored; frame 0x3e8/0x3e4, result/state homes and getter boundary remain.
// Round 431: Final result from unchanged transition/reset tests, bool/byte/short/long carriers,
// combined tail then per-phase publication: 449/455/484/480/441/461/476/471. Original conditional true
// stores, result carrier and complete executable body restored.
// Round 441: Deadline expiry owns guard/reset result; bool expiry with long/bool/short/byte reset 469/450/469/450; long expiry 463/447/463/447. Long expiry plus byte/bool reset recovers frame 0x3e4 but regresses; all original computations and body restored.
// Round 449: primary/secondary/both entry states read at each equality
// use 439 each; with secondary receiver declared after first guard 457
// each; primary/both live states with conditional index producer 467 each.
// Original snapshots, receiver lifetime and index producer restored; frame,
// dead state stores and getter boundary remain.
// Round 247: distinct strict/fallback imbalance outputs with phase/entry cost and
// readiness storage, 439/439/439/439; the same four with identifier storage at entry
// 442/442/442/442. Original shared output, declarations and entire body restored.
// Round 259: persistent settings across all four solver uses: team count, capacity, party
// flag, minimum, maximum, capacity pair, party triple and all five,
// 467/474/451/455/455/499/472/455. Original queries, receivers and full body restored.
// Round 478: complete strict solver predicate with short/bool/byte/long result
// 455/458/458/457; both strict and fallback predicates 458/458/458/458.
// All original producers and body restored; frame 0x3e8 versus 0x3e4, state/result homes and getter boundary remain.
// Round 53 (m3 lane 1; kept: the three clock reads through the existing header time_now() and the
// variant index through the existing get_value_49c8(), byte-identical 439, in place of their bodies
// written out): the property value cleared before the player-count test 457 (1577 bytes); the first
// start time read into a local before the clock 439 (aligned diff 155 from 162: retail's load order;
// one-use, not kept); the reset flag as bool 469 (frame 0x3e4). Retail's 0x5b080 call is why ours
// keeps the session state in ESI after the getter call (the inlined body re-tests it).
// Round 478: complete strict solver predicate with short/bool/byte/long result
// 455/458/458/457; both strict and fallback predicates 458/458/458/458.
// All original producers and body restored; frame 0x3e8 versus 0x3e4, state/result homes and getter boundary remain.
// Round 259: Entry uninitialized reset/waiting/balance storage, seven subsets, then all with Boolean reset
// 439/439/439/439/439/439/439/469. Original body and producer sites restored.
// DH round 264: On the complementary waiting graph, native producer record/existing kind-four setter/
// both/native long balance/record-balance/setter-balance/all/all with initial memset
// 439/477/477/454/454/477/477/486. Original record, property construction, short balance
// and complete body restored; frame/result/state/getter homes remain.
// Round 282: Boolean reset with result/completed, reset/reset_required and
// transition_pending/waiting name combinations, eight at 469/1562. Frame 0x3e4 agrees
// but result remains at +0xe; original long reset, names and entire body restored.
// Round 283: positive type/primary-state/secondary-state admission gates, all seven subsets, each 439;
// all gates with delayed secondary receiver 457. Original complementary waiting graph, receivers and
// whole body restored; frame, state stores and getter boundary remain.
// Round 478: complete strict solver predicate with short/bool/byte/long result
// 455/458/458/457; both strict and fallback predicates 458/458/458/458.
// All original producers and body restored; frame 0x3e8 versus 0x3e4, state/result homes and getter boundary remain.
// Round 221: coordinated byte return with Boolean/native-byte final storage 439/439; with Boolean reset
// 469/469. Signed-byte return on those four graphs 439/439/469/469. Every native return declaration
// loses matched dispatcher 0x706f0 (123) and is rejected. Boolean reset recovers frame 0x3e4 but
// shortens to 1562/1589; result remains at +0xe versus +0xf. Original Boolean interface, caller
// declaration, long reset, short balance, waiting decision and whole body restored; frame/state stores
// and getter boundary remain.
// Round 217: actual assignment output cursor, long/int/short/word ordinal at phase/entry scope,
// 441/441/443/443/441/441/443/443. Original indexed producer and complete body restored.
// Round 259: Entry uninitialized reset/waiting/balance storage, seven subsets, then all with Boolean reset
// 439/439/439/439/439/439/439/469. Original body and producer sites restored.
// DH round 264: On the complementary waiting graph, native producer record/existing kind-four setter/
// both/native long balance/record-balance/setter-balance/all/all with initial memset
// 439/477/477/454/454/477/477/486. Original record, property construction, short balance
// and complete body restored; frame/result/state/getter homes remain.
// Round 304: false result after receivers/type/primary/secondary admission, with explicit early
// false outcomes: 441/446/470/470; with Boolean reset 471/449/484/484 (frame 0x3e4). Original
// result default, long reset and complete body restored; result/state homes and getter boundary remain.
// DL224: Coordinated inline count/team balancer and player-lookup declarations in seven subsets
// 439 each; all three with the capacity getter 439. All checked user scores agree; every original
// declaration and body restored. Frame, dead state stores, result-byte home and data-getter
// boundary remain.
// Round 241: Shared long balance-output scratch with completed roster count/deadline/final deadline,
// singles/pairs/all 439/441/439/441/439/441/441; all with first elapsed result 441.
// Original separate scalar phases and complete body restored; frame and result/state/getter homes remain.
// Round 120 (fast): a separate local for the second session state 439 (same code); the existing
// session_state_is_live() at both live-state tests 484 (1554 bytes); the missing variant/data tests as
// two ifs (retail repeats the mode-7 arm) 445 (1590 bytes); retail's positive transition flag (`transition =
// function_72140(arg_0)`, retail stores AL to its byte home) 449 (1578 bytes, LCS-aligned 199 -> 192); with
// a bool reset flag 464 (1568 bytes, frame 0x3e4 agrees, aligned 184; result byte still +0xe against +0xf).
// Retail's byte homes: transition +0x11, reset +0x12, result +0x13 (after four pushes). Retail also calls
// get_data_4a00 (0x5b080, its only out-of-line call site; 0x7e210 and network_session_client.cpp inline it)
// where ours inlines it. Restored.
// Round 40 (m3 lane 2, fast; all restored; BLOCKED-BY 0x5b080: retail calls it at this site only, ours inlines it and keeps the state in ESI, and
// per-site inlining has no source lever without a new noinline). A rewrite from retail's flow (a ready flag from the player match, cleared in each
// failure, set from 0x72140; the mode-7 arms; flags single-exit): fresh opening 473 (1574 bytes, aligned 241); this body's opening 445 (1589, aligned 242,
// arg_0 in EBX where retail has EBP); declarations swapped 445. Grid on it (fast), in order of arms separate/merged, ready bool/long, reset bool/long,
// the 0x3c pointer early/late, the property value early/late, kind four by memset/existing setter:
// 445/482/441/482/443/487/446/483/451/473/446/473/454/481/454/481
// 460/500/460/500/469/510/462/502/441/479/440/478/454/497/447/487
// 458/489/458/489/458/490/463/494/433/464/445/474/463/493/453/484
// 474/503/475/506/487/513/463/498/470/494/470/500/478/503/469/507
// Best 433 (merged arms, bool ready, long reset, 1589 bytes, aligned 239): six points, and its ready flag is set in failure arms and tested later, so not kept.
// Merged arms with both bools 458 give retail's frame 0x3e4 and EBP with aligned 202 (1573 bytes). Long ready with separate arms 460, aligned 204 (1571).
// Round 467: Shared signed entry-state/balance-output workspace with entry declaration,
// Boolean reset and early property default subsets: 441/441/466/466/455/455/467/467.
// Secondary-state spill appears, but frame grows to 0x3ec; original workspace/body restored.
// M304: Roster bound/matched-count/ordinal descriptive name combinations, eight at 439/1589.
// Original scalar identities and body restored; frame, result/state homes and getter boundary remain.
// Round 204 (fast, on the 426 form): the missing variant/data tests as two ifs 442 (1605 bytes); positive
// transition flag with a bool reset 430 (1576 bytes, frame 0x3e4 agrees), with the long reset 425 (-1, not kept);
// the existing function_75890 for both elapsed times 481 (called out of line, 1542 bytes); result/transition renamed
// so the result byte sorts higher 426 (same code). Before it: a volatile copy of the state in each early-exit
// arm 426, a file-local inline state helper 426 (same code as the kept call).
// Round 58 (lane 14, fast; aligned counts in brackets, this body [200]): a test that folds away ahead of the data getter
// (`if (local_2)`, likewise on local_0, local_1 or arg_0) keeps 0x5b080 out of line as retail: 440 [199], so the getter is not a hard blocker,
// but the test is always true and is match-only. Plain nesting does not do it: the opening's arms as else 426, else if 426, type test too 426.
// On the test: positive ready flag cleared in each failure arm 445 [184]; with a bool reset 487 [212]; with the property value zeroed before
// the player-count test 487 [211]; with the mode-7 arms apart 454 [152] (1545 bytes). Left on that form: session_b held in EBX where retail
// spills it and uses EBX for the owner, ESI for scratch. Restored.
// Round 402: Actual returned entry-state admissions, independent/sequential bool/byte/short/long outcomes at unchanged call boundaries.
// Scores 426/426/426/426/426/426/426/426. NOT kept: unmatched outcome forms; original complete body restored.
// J260: Direct state/mode/both reset results, long 460/457/425 and Boolean 421/473/419; accumulated long/Boolean 429/424. Complete Boolean results reach 419, frame 0x3e4, 1590 bytes, first +0x27. NOT kept: seven-point gain below eight. On that new graph, seven flag-name subsets tie 419; false default after receivers 421.
// NOT kept: no eligible native repair; opening executable source restored. Return byte +0xe versus +0xf, secondary-state and getter boundary remain; original defaults, fields, clocks, calls and whole body restored.
// Round 398: Final result produced by primary/secondary admission, bool/byte/short/long:
// 470/484/468/469/475/477/469/447. NOT kept: result/admission coupling does not match; original
// final result, method-call boundaries and complete body restored.
// MiniMax D (11-17): no new form, no lower score (387, was 387); retail's out-of-line 0x5b080 data
// getter, its second session state in EAX against our ESI, the two dead state stores at +0x20 and
// +0x18 and the call at 0x71ac2 remain.
// Earlier 402 (was 426, fast build; 1589 bytes, retail length; first difference +0x6c). b6P walk of the
// whole body against retail, block by block, found one behaviour difference and four structure points:
// - BEHAVIOUR (fixed, score 426 unchanged): the call to 0x71ff0 passed the ratings record plus 4. Retail
//   passes the record itself (0x71cb6 lea ecx,[esp+0x94] after one push is the record base, the same base
//   0x718dd gives 0x7e210); 0x71ff0 reads count at +0xc and the identity rows at +0x14, and the record's
//   own count is field_c (0x71b57, 0x71c97), so +4 made it read the wrong count and shifted every row.
// - Retail's ready byte [esp+0x11] is the 0x72140 result (0x71b14) and is stored at each failing test
//   (0x71964 the match result, then 0x71978, 0x71996, 0x719df, 0x71a24, 0x71a4b, 0x71a63); the draft kept
//   the inverse flag and no stores. Now `transition` is assigned at each of those tests; with the reset flag
//   a bool this is 419 against 426, an evaluation-order and structure change, not a behaviour change.
// - The balance flag is a bool (retail keeps no flag: it tests AL straight after 0x91fc0 at 0x71bc3 and
//   0x71c65), not a short: 420 against 426.
//   The old `short` was a score-only type; the same note recorded 'bool 420' as no gain.
// - The two live-state tests call the existing session_state_is_live() (include/unknown_058dd0.h, already
//   included here; first use in this file): 412 against 419 written out. The call at 0x71ac2 stays out of
//   line here while retail expands it; the one at 0x71a8e is inlined.
// - Retail tests !surface (0x718a9) and !data (0x718c2) separately and repeats the mode-7 arm (0x718b1,
//   0x718c8) rather than one `||`: separate ifs give 402 against 412.
// Retail quirk, not reproducible as a fix: the late result byte [esp+0x13] is written only at 0x718bb, 0x718d2
// and 0x71d18 and read at 0x71d54, so a pass that sets neither mode 7 nor the transition returns an unset byte
// (the draft's `local_3 = false` start value is a separate byte [esp+0xf], 0x71754). The same split shows in our
// build at the previous base. Still different: frame 0x3e4 agrees now but retail calls 0x5b080 (0x717fa) where ours
// inlines get_data_4a00, the two dead state stores (retail 0x71780/0x717c1 at +0x20/+0x18, ours share one slot),
// local_14's seven zero stores sit before the player-count test in retail (0x71849-0x71861; declaring it earlier
// changes nothing), and the call at 0x71ac2. Tried and kept out: nested else-if chain instead of gotos 402 (same
// code); start time read into a local before the clock 420 (before these changes, unchanged); reset flag declared
// after the ready flag 402; long reset flag 427 against bool 419 (before the helper); the 0x75870/0x75890 helpers (earlier rounds)
// 468-512.
// m3 lane 1 r115 (fast with 0x72140, 0x71ff0, 0x71e00, all MATCH; control 387, the fast check warns that callee 0x7e210
// builds differently in its image): a `static __forceinline` redeclaration of the header's live-state test (retail expands
// it at both sites, ours calls the copy twice) 426 (1602 B): both sites expand but 0x5b080 stays inlined at the top.
// Diagnostic, not kept (a private copy of 0x75890): both elapsed times through an inline copy of 0x75890's body 419 (1597 B:
// each start field is then read ahead of the clock as in retail), with the redeclaration too 427 (1602 B); 0x5b080 stays
// inlined in every form, so its call in retail is not explained by the size of the other expansions. BLOCKED-BY 0x75890
// for the two elapsed reads: its file is /Ob1, which LTCG never expands from.
// Captured growth count/roster bound/growth timestamp join, seven nonempty subsets, then all with signed count: 387/402/432/402/432/433/433/433.
// NOT kept: count capture ties. The shared roster bound reaches 1589 bytes at 402, and the timestamp join grows to 1605 bytes.
// Original executable body restored.
// m3 lane 14 r108 (fast with 0x72140, 0x71ff0, 0x71e00, 0x5b060, 0x5b080, all MATCH; control 387 at 1597 B, 191 aligned rows by sequence alignment
// with addresses masked). Retail's block order: the mode-3 block whole right after the fourth test (0x71913, its own flag store and jmp); every failing
// chain arm stores the ready byte and jumps to the not-ready block (0x71a8e, or 0x71a87 where EBP is restored first), which sits mid-function after the
// members-ready arm; the balance-failure arm (0x71c69) falls into the timeout (0x71c7a); the identity loop and the 0x71ff0 call sit between the timeout's
// two arms. Tried: the two label blocks moved into the arms where retail lays them out 387 (same code: the compiler sinks a block that ends in a goto to
// the gap ahead of its target); the whole body as nested if/else with the timeout under `if (!transition)` 382 (1610 B, aligned 208: the not-ready
// block stays mid-function, but the guard costs a run-time test retail lacks and pulls the timeout up after it; five points, longer and further by
// alignment, not kept); nested chain, `if (!transition) {not ready} else {success}` and the timeout reached by fall-through with one goto past it to the
// mode-12 call 416 (1599 B, aligned 182: retail's block order for all four points above, but arg_0 takes EBX and the owner EBP at entry, the reverse
// of retail, and the kind-3 arm takes the place ahead of the not-ready block that retail gives the members-ready arm); that with the chain flat (jumps
// to the not-ready test) 416 (same code); with the reject block as if/else 425 (1597 B, aligned 194); with the original goto tail 416 (same code);
// with the mode-3 block back ahead of the timeout 416 (same code); with the split's arms swapped, success first 406 (1589 B, retail's length, registers
// as retail, aligned 189: the not-ready block goes back to the end). The real 0x58d70 in place of the header's live-state copy at both sites, on this
// body 387 (same code: both sites call it out of line; retail expands both and calls 0x5b080). Diagnostic, reverted: the third form with the live test
// forced inline 439 (1613 B, aligned 172, entry registers still reversed). So retail's source has the structured not-ready/success split; what is
// missing is the form that keeps arg_0 in EBP at entry with it (the swapped-arm form shows the arm order moves that choice).
// Variant-gated getter 428, getter at first data test 426, after both elapsed reads 424,
// between variant key and lookup 429, before both 426, after first elapsed read 397,
// variant gate with merged missing-input arm 428, variant gate after first elapsed read 457.
// NOT kept: the variant gate restores the getter call at a different position and raises the score.
// M3 lane 9 r144 (fast with 0x5b080/0x72140/0x71ff0/0x71e00, control 387, aligned 191): one-object /GL screens give the
// smallest inline threshold that expands the data getter 0x5b080 at each site: 0x6d380 9, 0x7e210 18, 0x6e720 23, here 50.
// Retail expands the first three and calls it here, so any link threshold from 23 to 49 reproduces all four; ours expands
// it here too (the link's threshold comes from the ballast object: /d2inlT40 or 20 on 0x5b080's file, or 40 on this file,
// changes nothing, diagnostic). BLOCKED-BY the build's inline calibration, as for 0x25d740. Trials: the three opening
// exits as `return local_3;` 449 (1578 bytes, the result byte folds); the getter through the owner's session pointer 400
// (1589 bytes, aligned 215); the two missing-input tests as one `||` 412; the second and third exits as an else-if chain
// around the body 387 (same code); the +0x3c pointer read before the clock 377 (1589 bytes but aligned 214, and retail
// reads it after the clock: a positional gain only).
// M3 lane 11 r128 (fast with 0x72140/0x71ff0/0x71e00/0x5b060/0x5b080, all MATCH; control 376 at 1589 B, aligned 190 by a
// sequence-aligned diff, 113 with registers masked): the +0x3c pointer read ahead of the first elapsed time 419 (aligned
// 213); the data pointer named and then read through a `byte *const &` 376 (same code); the surface description the same
// way 376 (same code); the second missing-input test as an `else if` 376 (same code); the second session pointer declared
// after the type test 437 (aligned 203); the +0x3c pointer declared at its first test 427 (aligned 209). Still BLOCKED-BY
// the build's inline calibration for 0x5b080 (expanded here, called in retail) and by 0x75890 for the two elapsed reads.
// M3 lane 13 r119 (fast with 0x706f0, 0x5b080 and 0x75890; control 376, aligned 190): retail expands 0x75890 for both
// elapsed times (the field is loaded before the clock: `mov esi, [ebp + 0xa68]` ahead of the time read, as a call
// argument is); its file is /Ob1, so it stays a call: through an `inline` or `__forceinline` declaration in this file 474
// each (1551 B, still called). Diagnostic only, a file-local copy of 0x75890 expanded at both sites 413 (aligned 181, retail's
// load order; a hand copy of an existing function, not kept). The existing kind-4 setter (0x59ab0, a contributor's /Ob1 file)
// through an `inline` or `__forceinline` declaration 437/437 (still called; aligned 170). The kind-3 arm clearing the flag
// after the setter call 444 (1557 B). Leads: `inline` on 0x75890's definition (its file was on another lane's list this
// round); 0x5b080's expansion here is the link's inline calibration (BLOCKED-BY 0x5b080 calibration).
// Round 438: The secondary state still takes ESI and the data getter still expands. Existing record layouts and the roster and progress views were tried.
// Read candidate count and identities through the actual roster record 376.
// Use the matchmaking state storage members for the count and timer 376.
// Read the timeout base through the existing hopper record 376.
// Use the existing session owner view for both sessions and the progress pointer 376.
// Use the actual progress record for the stage and attempt count 376.
// Read both components of the final deadline through their declared records 376.
// Use the actual owner and progress record layouts together 376.
// Use the roster, owner, progress, hopper and state record forms together 376.
// NOT kept: All eight record forms tie at 376 and 1,589 bytes. The reduced preview agrees with the full score and extent and reports six unresolved global references. The secondary state stays in ESI and the data getter still expands.
// M3 lane 14 r125 (fast with 0x706f0, 0x5b080 and 0x75890, all MATCH; control 376, aligned 190): round 119's lead tried,
// `inline` on 0x75890's own definition (its /Ob1 file) with both elapsed times written as calls to it: it expands here as
// retail (start field loaded ahead of the clock) 413 (1589 B, aligned 181); with the property-value clear ahead of the
// player-count test as well 376 (aligned 184), the clear ahead of the first elapsed time instead 376 (aligned 202); only the
// first elapsed time through it 376 (aligned 190, the base's code), only the second 376 (aligned 185). 0x5b080 stays
// expanded in every form, so the secondary state keeps ESI and the zero register stays EAX (retail ECX, re-zeroed after
// the clock for the two missing-input compares). Both files restored; still BLOCKED-BY 0x5b080's inline calibration.
// M3 lane 12 r154 (fast with 0x72140/0x71ff0/0x71e00/0x5b080/0x706f0, all MATCH each time; control 376, aligned 331 equal):
// the second session as a `const &` to the owner's field 473 (1616 B); the data getter's result and the surface lookup's
// result each bound by `const &` 376/376 (same code); a second session local re-read from the owner for the data getter's
// call 400 (the getter still expands); the first session as a `const &` 461 (1595 B).
// M3 lane 3 r277 (fast with 0x5b080, 0x75890 and 0x192e60; control 376): `inline` on 0x75890's definition 376 here (the clock
// read is written out) but matched 0x776a0 in 0x75890's own file then expands it (102, 680 B), so that lead is closed; with
// both elapsed times through 0x75890 as well 413; the data getter called ahead of the 0x49c8 getter and 0x192e60 444
// (1573 B); the two missing-input tests merged into one `||` 412 (1594 B); the data getter declared at its own test 425.
// The state register follows 0x5b080's expansion: expanded, its state test is CSE'd with the earlier inlined state
// tests across the 0x192e60 call (ESI); retail calls 0x5b080, so its state dies in EAX. BLOCKED-BY 0x5b080 calibration.
// M3 lane 13 r130 (fast with 0x5b080/0x706f0/0x72140/0x71ff0/0x71e00, all MATCH; control 376). Kept: the three early
// exits as if / else if / else in place of `goto finished` (the same code, 376; three gotos and the label gone). On it:
// only the second-session exit as if/else 376 (same code); the property value cleared ahead of the player-count test
// 376. The data getter's expansion follows the estimated frequency of its call site, not a per-caller count:
// (diagnostic) 1, 2, 4 and 8 more expansions of the value getter ahead of it 464/504/629/808 and it still expands,
// while one more conditional level keeps it a call: called only when the surface lookup succeeded 429 (1565 B), as a
// ternary on the lookup 427 (1573 B), read inside the second missing-input test 425 (1589 B, retail's length);
// (diagnostics) the last arm under a test of the still-false result, or under a repeat of the type test, 503 (1525 B:
// other expansions in the arm become calls too), the getter alone under that folded result test 452 (1565 B);
// (diagnostic, flags) the file without /Ob1 376 (same code). Retail calls the getter unconditionally right after
// 0x192e60, so its source has one more two-way split ahead of that point than this body, and no form here supplies one
// without emitting a test or repeating one. Still BLOCKED-BY 0x5b080's call-site calibration.
// M3 lane 1 r138 (fast with 0x72140/0x71ff0/0x71e00/0x5b080/0x706f0, all MATCH; control 376): 0x5b080's own body respelled
// (its file is machine 3's) to move its inline cost here, nested `if (state > 2) { if (state <= 8) ...` 376, a state local
// with one three-part test 376 (both the same code; 0x5b080, 0x6d380 matched, 0x6e720 22 and 0x7e210 871 unchanged); the
// members-ready live test as `!session_state_is_live(local_2) || !local_2->flag49e8` 376 (same code); the property value
// as `= {0}` in place of the memset 387 (1597 B); the not-ready block's live test as two nested ifs 376 (same code). All
// restored; still BLOCKED-BY 0x5b080 calibration.
// M3 lane 13 r132 (fast with 0x5b080, MATCH each time; control 376). The data getter stays expanded in every form:
// the whole last arm as a file-local forced-inline function taking the state, the owner and both sessions 495 (1629 B);
// the last arm ending in its own `return` of the result 434 (1589 B); the type test as a one-case switch 376 (same
// code); the value getter written at the lookup and again at the 0x71ff0 call, no local, 418 (1584 B); the data getter
// read after the first elapsed time and the progress pointer 417 (1581 B).
// Measured law for the getter's expansion (one-object link of this file with the getter's file, build/r132/cost717.py:
// the smallest inline threshold that expands it, against the build's 81): 18 as the function's first statement, 22
// under the type test, 31 in the first live-state arm, 50 in the second arm or in the last arm (this body), 86 under
// one more `if`, about 13.5 + 4.5 * 2^depth. Depth is the count of enclosing `if`, `switch` and `?:` constructs as
// written, also when the compiler folds their test (`if (true)`, `true ? x : NULL`, `switch (0)`, `if (local_2)` all
// 86 with no code of their own); a bare block, `else { if` for `else if`, a `for (;;)` with a break and an `&&` leave
// it at 50, removing or folding the type test or either live-state test leaves it at 50, statements before the call
// in the same arm leave it at 50, and a `do ... while (false)` around the call lowers it to 15. So retail's source has
// the getter call (or the whole last arm) under four conditional constructs where this body has three, and the fourth
// one emits no test. No natural candidate found: a fourth test that is always true where it stands is the steering
// form round 58 measured (`if (local_2)`, 440 then). Still BLOCKED-BY 0x5b080's call-site depth.
// M3 lane 1 r144 (single-address fast, control 376): a byte scan of retail finds one call of 0x5b080 in the whole
// image, here at 0x717fa (its three other users expand it), so the getter is out of line in retail for this site only.
// The data getter called through `local_0->session_b` 400; through a `c_class_58d20 *const &` binding of the second
// session 376 (same code); the main arm under a positive live test with the mode-14 arm last 376 (same code); the
// getter called after the elapsed time and the owner field reads 417 (1581 B); the type test as an early return 457
// (1594 B). Diagnostic (reverted, a folded test): the getter call inside `true ? ... : NULL` keeps it out of line as
// in retail but scores 452 (1565 B: the frame, the entry registers and the dead state stores all move), so the rest
// of the body fits only the expanded getter. Reading: retail's [esp + 0xf] before `push edi` and [esp + 0x13] after it
// are the same result byte, so there is no second, unset result.
// M3 lane 2 r125 (H2M_FAST with 0x5b080/0x706f0, plus 0x6d380/0x6e720 for the getter trial; control 376): the data
// getter's own body as one three-part test with an early return 409 here (0x5b080 itself 13 at 36 B, 0x6e720 126; restored);
// diagnostic, reverted: the getter called through a file-local inline forwarding function 376 (the wrapper and the getter
// both expand); the type test as an early `return` 457 (1594 B); the second live-state test written positively with the
// main work in its arm and the mode-14 exit in the else 376 (same code); the two later live tests through the session's
// own live-test member (`function_058d20()`) 376 (same code). Still BLOCKED-BY 0x5b080's call-site depth.
// M3 lane 15 r126 (h2m fast with 0x5b080, 0x72140, 0x71ff0 and 0x71e00, all MATCH; control 376): the property value
// declared and zeroed ahead of the player-count block (retail stores its seven zero dwords there) 376 (same code); the
// data getter as `local_2 ? local_2->get_data_4a00() : NULL` (a folded null test: the getter is then called as in
// retail) 452 (1565 B: frame 0x3e0, the result byte in BL, the owner in ESI and the second session in EBP where retail
// keeps the result at [esp + 0xf], the state in EBP and the owner in EBX), with the early zeroing as well 447; the data
// getter read ahead of the surface lookup 435 (1573 B); the function's outer bare block removed 376 (same code).
// M3 lane 12 r169 (h2m fast with 0x5b080, 0x706f0, 0x72140, 0x71ff0 and 0x71e00, all MATCH; control 376): the data getter
// as `local_8 ? local_2->get_data_4a00() : NULL` (a real test) 427 (1573 B: the getter is then a call as in retail and the
// state/value block is retail's, but the result byte takes BL and the owner ESI; retail calls the getter with no test
// after the lookup); as `local_9 = NULL; if (local_8) local_9 = ...` 429 (1565 B); the ternary with the `!local_8` exit
// dropped 479 (1533 B); the owner's +0x3c field read before the elapsed time 419; `local_3 = true` ahead of the mode-7
// store in the surface-missing exit 375 (statement swap, 1 point). Still BLOCKED-BY 0x5b080's call-site depth.
// M3 lane 14 r141 (H2M_FAST with 0x5b080/0x706f0/0x72140/0x71ff0/0x71e00, all MATCH each run; control 376, aligned 190):
// the live tests nested positively (`if (live a) { if (!live b) ... else main } else 0x71e00`) 376 (same code); the data
// getter called through `arg_0->owner->session_b` 419; the mode-14 exit as `return true;` 455 (1578 B); the result
// declared after the owner and session locals 379; the data local started NULL and then assigned 376 (same code). The
// getter stays expanded in every form. Still BLOCKED-BY 0x5b080's call-site depth.
// M3 lane 16 r170 (h2m fast with 0x5b080, 0x706f0, 0x72140, 0x71ff0, 0x71e00, all MATCH; control 376): the first elapsed
// time's start field named ahead of the clock 376 (same code: the read sinks past the clock); both start fields named 413
// (the second then loads ahead of its clock as in retail, as the 0x75890-inline forms do); the player count read once into a
// named `dword` for the compare and the store 376 (same code); local_2 declared inside the type test 437 (first +0x16); the
// owner-state test split into an outer state test and an inner timeout test 376 (same code). Still BLOCKED-BY 0x5b080.
// M3 lane 4 r253 (h2m fast with 0x5b080, 0x72140, 0x71ff0, 0x71e00, 0x706f0, plus 0x72330, 0x6daa0 and 0x6dae0 for the
// getter trials, all MATCH unless noted; control 376): the data getter's hand-written expansion in 0x72330 as a call to it
// 376 (0x72330 stays matched); with 0x6daa0's expansion as a call as well 376 (0x6daa0 matched); with 0x6dae0's too 376
// (0x6dae0 134, 268 B), so the number of the getter's call sites does not move its expansion here (all restored); the two
// missing-input exits in the other order 372 (statement order by score, not kept); the surface and data locals declared
// first and assigned after 376 (same code). Still BLOCKED-BY 0x5b080's call-site depth.
// M3 lane 5 r239 (H2M_FAST with 0x5b080, 0x72140, 0x71ff0, 0x71e00 and 0x706f0, all MATCH; control 376). Reading: our
// build also calls the live-state test out of line at both of this body's live tests (session_state_is_live, folded
// with another file's copy), where retail expands it. Diagnostics: a forced-inline file-local copy of the test at both
// sites 431 (1602 B); with the data getter as `true ? ... : NULL` as well 438 (1555 B, stack-masked aligned 185 -> 165,
// the second session state then in EAX as retail); the getter form alone 452 (1565 B). The two tests through the
// existing inline function_058d70 376 (same code: called out of line too), with the getter form 452. Body unchanged;
// still BLOCKED-BY 0x5b080's call-site depth.
// M3 lane 8 r233 (h2m fast with 0x706f0 and 0x5b080, both MATCH unless noted; control 376). Retail calls 0x5b080 out
// of line only at this site (0x6d380, 0x6e720 and 0x7e210 expand it in retail too), so the open point is this site's
// expansion level (round 58's folding test), not the getter's own size: the getter (machine 3's) written with one
// combined condition 376 (same code everywhere); as one conditional return 409 (the getter itself then 13, 36 B); as an
// early `return data4a00;` under the combined test 409 (getter 13), inside the nested tests 409 (getter 13); here, the
// second state test with its arms swapped (`else if (local_2->function_058d20())` with the mode-14 arm last) 376 (same
// code). Body unchanged; the getter restored.
// M3 lane 1 r165 (fast screen with 0x706f0 and 0x5b080, both MATCH; control 376). Aiming at the data getter's call-site
// level (it stays expanded in every form below; retail calls it): the opening written as positive nesting (both
// session-ready tests true around the main arm, the mode-14 and 0x71e00 arms as elses) 376; the validation chain from
// the missing-input exits to the 0x71ff0 call inside `do { ... } while (false);` 376; the first session's ready test held
// in a named bool 376; the second's held in a named bool inside the first test's else arm 376; the type test held in a
// named bool 376 (same code each: a named test does not lower the site's level here); both ready tests named and
// computed ahead of the first branch 455 (+0x13: the second test is then evaluated on the 0x71e00 path too).
// Diagnostic (a test that folds away, not keepable): the getter as `local_2 ? local_2->get_data_4a00() : NULL` keeps it
// out of line as in retail, 452 (1565 B), the listed `true ? ... : NULL` code. Body unchanged; still BLOCKED-BY 0x5b080's
// call-site depth.
// In round 641, the frame agrees. The second state still uses ESI and the getter still expands. Names for the candidate
// receiver 376, both receivers 376, playlist index 376, index and settings 376, settings and roster 376, elapsed times
// 376. Capturing the candidate only after the primary ready check 437; shared ratings and capacity rejection 445.
// The names leave the relevant cells unchanged; the body is restored.
// M3 lane 2 r148 (fast checks with 0x5b080, 0x5b040, 0x58d20, 0x71e00, 0x72140 and 0x706f0, all MATCH each run; control 376
// at 1589 B, aligned 132, 62 with registers and offsets masked; body unchanged). Reading: three link-time expansion choices
// differ, not registers. Retail calls the data getter at the top and expands both live-state tests (after the
// members-ready test and at the not-ready exit); this build expands the getter and calls the header's live-state helper
// at both places. The second state sits in ESI here only because the expanded getter tests it again after the 0x192e60
// call; retail's EAX follows from its call. Forms: the getter diagnostic re-measured 452 (1565 B) with the two live-state
// tests still calls, so the three choices do not share one budget; both live-state tests through the macro spelling
// other session files use 431 (1602 B, retail's rows there, 13 bytes over with the getter still expanded), the first
// only 424 (1597 B), the second only 420 (1594 B); the decision chain with no goto (the transition flag cleared by each
// failing test, each later test guarded by it) 435 (1572 B: the stores of the cleared flag merge into one where retail
// has one per failing test, and the live-state helper is still called, so retail's chain is not that form); the two
// session-state exits as early returns 448 (1578 B). Still BLOCKED-BY 0x5b080's expansion at this site.
// M3 lane GA r7 (fast with 0x5b080/0x5b040/0x58d20/0x71e00/0x72140/0x706f0/0x71ff0, all MATCH; control 376 at 1589/1589 B,
// aligned 304, register-masked 98; 17 real trials). Zero-goto structured rewrite (`bool ok` for the 4 early guards placing
// mode==3 at 0x71913, nested chain, `if (!transition) { not_ready } else { success }`, `if (!transition) { timeout }`):
// success-first 441 (al 261, m 86), not-ready-first 462 (al 253, m 85), + inline `state > 2 && state <= 8` + 6-field kind-4
// clear (no redundant `kind = 0` store) 455 (al 174, m 75), + early `local_14` zeroing ahead of player_count 451 (1620 B,
// **aligned 171, masked 74**: reproduces retail's 0x71913 mode-3 block, mid-function not-ready block, balance-failure
// fall-through into timeout, and identity loop + 0x71ff0 sandwiched between timeout if/else; `arg_0`/`owner` swap EBX/EBP).
// Swapping to `if (ok) { ... } else { mode==3 }` + `goto local_31` + inverted `local_21` test restores entry `EBP`/`EBX` at
// 428 (1577 B, +0x32, al 249, m 84); `if (!ok)` + `goto local_31` + inverted `local_21` 432 (al 248); diagnostics with folded
// `local_2 ? local_2->get_data_4a00() : NULL` on those two 437/460 (1564/1542 B, al 251/230). On 376 base: 6-field kind-4
// clear 409 (m 85), `memset(&local_14.value04, ...)` 407, `bool local_25` dropped 376, `switch (local_1->type)` with default
// 376, narrowed `local_15`/`local_16` 376, shared `expected_count` 376, inline live + 6-field kind-4 + no `local_25` 426
// (m 83), inverted `local_21` 376, early `ok` only 420. Body restored; still BLOCKED-BY 0x5b080 call-site calibration.
// M3 lane GA r12 (fast with 0x5b080, 0x5b040, 0x58d20, 0x71e00, 0x72140, 0x706f0, 0x71ff0, all MATCH; control 376 at
// 1589/1589 B, fr 0x3e4, al 190, mk 113; 16 distinct trials, body unchanged): positive `if (ok)` + not-ready-first +
// inline live + 6-field kind-4 + `goto local_31`: early `local_14` 428 (1577 B, +0x32, al 195, mk 101; header
// `session_state_is_live` produces the exact same 428 code, expanding both sites automatically here unlike on 376), late
// `local_14` 419 (1579 B, +0x32, al 191, mk 100), zero-goto `if (!transition)` timeout 442 (1610 B, al 187), success-first
// with/without `goto local_31` 433/440, `function_058d70` 437 (1563 B, stays a call), early `type != 11` return 457;
// `if (!ok)` + not-ready-first + inline live + 6-field kind-4 + early `local_14` + `goto local_31` 432 (1605 B, al 185, mk
// 83). On 376 base: nested `goto local_29` removal 425 (1599 B), + inline live 447, + 6-field kind-4 434; kind-4 store
// before 6-field clear 409 (1589 B, al 166); `function_75870` alone 473 (1555 B, fr 0x3e0), with `function_75890` 489
// (1535 B); `function_058d70` 376 (same code). 0x5b080 stays expanded in every trial (BLOCKED-BY link inline calibration).
// @retail 0x71730
bool __stdcall function_71730(c_session_state_matchmaking *arg_0)
{
	bool local_3 = false;
	{
		s_session_owner *local_0 = arg_0->owner;
		c_class_58d20 *local_1 = local_0->session_a;
		c_class_58d20 *local_2 = local_0->session_b;
		if (local_1->type == 11)
		{
			if (!local_1->function_058d20())
			{
				function_71e00(arg_0);
			}
			else if (!local_2->function_058d20())
			{
				arg_0->mode = 14;
				local_3 = true;
			}
			else
			{
				long local_7 = local_2->get_value_49c8();
				s_surface_description *local_8 = function_192e60(local_7);
				byte *local_9 = local_2->get_data_4a00();
				long local_10 = time_now() - arg_0->unknowna68;
				byte *local_11 = *(byte **)((byte *)local_0 + 0x3c);
				bool local_12 = false;
				bool transition = false;
				long local_15[16];
				s_71730 local_16;
				if ((dword)local_2->player_count > *(dword *)((byte *)arg_0 + 0xa6c))
				{
					*(dword *)((byte *)arg_0 + 0xa6c) = local_2->player_count;
					*(long *)((byte *)arg_0 + 0xa70) = time_now();
				}
				long local_17 = time_now() - *(long *)((byte *)arg_0 + 0xa70);
				s_session_property_value local_14;
				memset(&local_14, 0, sizeof(local_14));
				if (!local_8)
				{
					arg_0->mode = 7;
					local_3 = true;
					goto local_28;
				}
				if (!local_9)
				{
					arg_0->mode = 7;
					local_3 = true;
					goto local_28;
				}
				if (!function_7e210(local_2, (s_matchmaking_ratings *)&local_16))
					goto local_28;
				if (local_16.field_358 > function_193400(local_8) &&
					*(long *)(local_9 + 0x184) == network_session_get_maximum_players(local_2))
					goto local_28;
				if (*(long *)(local_11 + 8) == 0 &&
					(local_10 > g_network_configuration.value1c0[9] * 1000 ||
					 *(long *)(local_11 + 0x10) > g_network_configuration.value1c0[8]))
					*(long *)(local_11 + 8) = 2;
				transition = network_session_players_match(local_1, local_2);
				if (!transition)
					goto local_29;
				if (local_16.field_360)
				{
					transition = false;
					goto local_29;
				}
				if (local_2->player_count != *(long *)(local_9 + 0x184))
				{
					transition = false;
					goto local_29;
				}
				{
					long local_18 = *(long *)(local_9 + 0x184);
					long local_19 = 0;
					for (long local_20 = 0; local_20 < local_18; local_20++)
						if (network_session_find_player(local_2, (const dword *)(local_9 + 0x188 + local_20 * 12)) != NONE)
							local_19++;
					if (local_19 != local_18)
					{
						transition = false;
						goto local_29;
					}
					long local_21 = function_1931d0(local_8, local_2->player_count);
					if (local_21 == 0x7fffffff || local_21 > local_17)
					{
						transition = false;
						session_property_value_set_kind3(&local_14, (local_21 - local_17) / 1000);
						goto local_29;
					}
					if (function_1934b0(local_8) && local_16.field_358 > function_193400(local_8))
					{
						transition = false;
						goto local_29;
					}
					dword local_22;
					if (!network_session_members_ready(local_2, &local_22))
					{
						transition = false;
						memset(&local_14, 0, sizeof(local_14));
						local_14.kind = 4;
						goto local_29;
					}
					if (!(session_state_is_live(local_2) && local_2->flag49e8))
					{
						s_session_id local_23;
						function_73ae0((unsigned __int64 *)&local_23);
						network_session_host_set_id49f0(local_2, &local_23);
					}
					if (*(long *)(local_11 + 8) != 1)
						*(long *)(local_11 + 8) = 1;
					transition = function_72140(arg_0);
					if (!transition)
						goto local_30;
					if (function_1934b0(local_8))
					{
						long local_24;
						bool local_25 = false;
						if (balance_teams_by_count(local_16.field_c, local_16.field_194,
							function_193440(local_8), function_193370(local_8), function_1934d0(local_8),
							function_1933d0(local_8), function_1933a0(local_8), true, &local_24) &&
							local_24 <= function_193400(local_8))
							local_25 = balance_teams(function_193440(local_8), function_193370(local_8),
								function_1934d0(local_8), function_1933d0(local_8), function_1933a0(local_8),
								true, local_16.field_c, local_16.field_194, local_16.field_d4, true, local_15);
						if (!local_25 && !function_193470(local_8))
						{
							if (balance_teams_by_count(local_16.field_c, local_16.field_194,
								function_193440(local_8), function_193370(local_8), function_1934d0(local_8),
								function_1933d0(local_8), function_1933a0(local_8), false, &local_24) &&
								local_24 <= function_193400(local_8))
								local_25 = balance_teams(function_193440(local_8), function_193370(local_8),
									function_1934d0(local_8), function_1933d0(local_8), function_1933a0(local_8),
									false, local_16.field_c, local_16.field_194, local_16.field_d4, true, local_15);
						}
						if (!local_25)
						{
							transition = false;
							local_12 = true;
							goto local_30;
						}
					}
					else
					{
						for (long local_26 = 0; local_26 < local_16.field_c; local_26++)
							local_15[local_26] = local_26;
					}
					function_71ff0(arg_0, local_7, (const s_match_player_list *)&local_16, local_15);
					goto local_31;
				}
			local_29:
				if (session_state_is_live(local_2) && local_2->flag49e8)
					local_12 = true;
				goto local_30;
			local_28:
				if (arg_0->mode == 3)
					local_12 = true;
			local_30:
				{
					long local_27 = *(long *)((byte *)local_8 + 0x59c) + *(long *)((byte *)arg_0 + 0x974);
					if (local_27 <= local_17)
						local_12 = true;
					else
					{
						if (!local_14.kind)
							session_property_value_set_kind2(&local_14, (local_27 - local_17) / 1000);
						network_session_host_set_data49cc(local_2, (const dword *)&local_14);
					}
				}
				if (transition)
				{
			local_31:
					network_session_set_mode(local_1, 12);
					local_3 = true;
				}
				if (local_12)
				{
					if (*(long *)(local_11 + 8) != 1)
						*(long *)(local_11 + 8) = 1;
					network_session_set_mode(local_1, 6);
					local_3 = true;
				}
			}
		}
	}
	return local_3;
}
