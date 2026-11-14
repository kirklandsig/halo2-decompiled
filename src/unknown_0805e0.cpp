#include "unknown_4cf790.h"
// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0805E0.CPP: the cache of the configurations of the
   players met recently (entries of 0x68 bytes at 0x4cf98c, chained through
   their next index). Most of the file is outside lane D's region
   (0x7fb50..0x80be7); lane D has the recent-player iterator. */

#include "unknown_11c920.h"
#include "unknown_2312b4.h"
#include <string.h>
#include <xtl.h>

struct s_player_configuration_cache_entry
{
	s_recent_player player;
	short unknown58;
	short next;
	short previous_other;
	short next_other;
	dword flags;
	union
	{
		byte unknown64[4];
		dword field_64;
	};
};



/* whether a XUID is an offline (machine-local) one */
static inline bool xuid_is_offline(unsigned __int64 xuid)
{
	return (xuid >> 48) == 0xfefe;
}

/* the entry at the iterator, moving the iterator to the next one */
static inline s_player_configuration_cache_entry *player_configuration_cache_iterate(long *iterator)
{
	s_player_configuration_cache_entry *entry = NULL;
	if (*iterator != NONE)
	{
		entry = &g_4cf98c[*iterator];
		*iterator = entry->next;
	}
	return entry;
}

static inline bool player_configuration_is_excluded(s_recent_player const *player)
{
	return (player->unknown08[0] & 3) != 0;
}

/* the next recent player after the iterator's: a valid, online, known player
   with a name; iterator starts at the first entry and ends at NONE */
// MiniMax H (11-14): no new form, no lower score (43): retail's entry pointer is the index scaled in place in
// ESI, the successor is kept apart in EBX and the & 3 mask is hoisted into AL before the loop, while ours copies
// the index into a second register for the entry pointer and keeps the successor in a general register; the forms
// measured this round - the successor in its own local 51, the same with the successor stored at the loop's end
// 52, the early-return shape 49 and the mask carried through the exclusion helper 49 - all score worse than the
// kept 43, so the original body stands.
// score 43 (was 43): early ESI save, EBX successor, mask hoist and loop exit differ;
// retail lets the entry pointer reuse the index register (imul esi, esi) and keeps the
// successor apart in EBX (back-edge mov esi, ebx), which leaves AL free for the & 3 mask;
// ours shares one register for index and successor.
// MiniMax B (11-12): no new form, no lower score (43); retail reads the index straight into esi and scales it
// in place (imul esi, esi, 0x68), keeps the successor in ebx, hoists the & 3 mask into al before the loop and
// tests the successor at the loop's end (cmp ebx, -1), while ours copies the index into a second register for
// the entry, keeps the successor in a volatile register and leaves the mask as an immediate. The forms
// measured: the successor in its own local stepped at the loop's end 49, the same with early returns 52, the
// guarded do-while with the successor 49, the loop condition on the result with the successor 57, the index
// stepped before the tests 57 and 58, the chain's end taken with an explicit break 50 (as an if/else 50 too),
// and the result-terminated loop that leaves the index unchanged when the chain ends 34, which spins in the
// source and was not kept.
// MiniMax S (11-11): no new form, no lower score (43); retail's shape read off the disassembly - the index
// scaled in place in ESI, the successor in EBX and the & 3 mask hoisted into AL before the loop - is not reached
// by giving the successor its own local (51 as a while loop and as a guarded do/while, because the successor then
// lands in ECX) nor by returning from the loop directly (52, which also drops the top zeroing of the result).
// MiniMax S (11-10): no new form, no lower score (43); the successor still rides in EDX where retail
// keeps it in EBX (and retail reads the index straight into ESI where ours copies it), the mask is still
// the immediate 3 where retail hoists it into AL before the loop, and the pushes keep our order.
// The permuter (48 tries) found nothing better than the baseline; giving the successor its own local
// moves it to ECX instead and measures 51, so the original body stands.
// MiniMax D (run 46): early returns with a successor local and the index stepped at the loop end 49,
// the successor read through the table 53; no lower score.
// MiniMax C (11-08): forms tried, none lower: early returns 52, early returns with the successor in
// a local 49, the successor in a local with the result flag 51, the loop turned into a do/while 51
// (base 43).
// native successor 51; persistent entry while/for 57; complete admission 43; identity predicate 53;
// native player assignment 43; native successor with identity 57; admission result 59.
// Native identity OR/logical OR 83/78; each with native name 83/78; native name alone 43;
// shared player receiver 43, with native OR/name 83, with native logical OR/name 78.
// existing iterator while 79; iterator for 56; shared identity 50; shared admission flags 43; combined mask 50;
// direct exclusion bits 43; current-index name 43; rejected entries continue 43.
// iterator-owned walk 71; iterator success exit 73; native/byte results 57/61;
// result-terminated loop 52; iterator result termination 44; output outcomes 68; native termination 47.
// Round 85: successor local with early returns 49; every field read through the table index
// (no entry pointer) 71; iterator store re-read as the index 51; loop over *iterator 71;
// successor read through the table after the entry 60 (EBX successor as retail, 137 bytes);
// short index 64 (entry reuses the index register as retail, 138 bytes).
// Round 7 (lane 15): retail hoists the `& 3` mask into AL (`mov al, 3` before the loop), which is what
// leaves the successor to EBX; the compiler hoists it only when the loop holds two uses of the constant.
// Early returns with a successor local 49, with `index = *iterator` 49, as a for loop 49, index reassigned
// from the successor 52, guarded do-while 49; harness: mask on the dword, inline, as two bit tests or
// through a char cast, `continue` filters, short index/successor mixes - the mask stays an immediate.
// The mask is the XDK guest mask (`XOnlineIsUserGuest`, as this function's caller tests it).
// Round 119: retail's result is not live in the loop (false is made again at the loop exit), so AL is
// free for the mask. Plain early returns (no successor local) 52 (no EBX at all: next in ECX, flags in
// EAX, mask an immediate); the caller's XUID view with XOnlineIsUserGuest 43, with early returns 52;
// the filter as an inline predicate with early returns 40 (`setne cl` for the last test), with the
// xuid tests split 40, with every test an early return 43, the predicate with early returns in the
// loop 54.
// Round 34: the hoisted mask in AL is the one cause: it makes the result constant at each exit (two
// false exits), the successor take EBX and the early ESI save; with early returns everything else
// is retail's. A compile-only screen confirms VC7.1 hoists the 3 only with two uses in the loop and
// retail keeps one, so the second use was removed after hoisting. Tried: the zero-XUID test as a
// null-identity comparison (`!(xuid == 0 && guest number == 0)`) 67 (hoists, but the extra guest
// test stays, 140 bytes); guarded do-while with a named successor and early returns 49; successor
// local with early returns 49; the XDK `XOnlineIsUserGuest` on the flags dword 43, with the
// do-while 49; harness only: the guest test twice (merged before hoisting), dword and byte forms
// together (both stay), a use of 3 outside the loop (no hoist).
// Round 311: trailing name/exclusion/online/identity tests produce the actual result
// 42/42/42/54; native full-width result 57/52/54/56. No match; original body restored.
// Round 106: one inline online-XUID test (set, not a guest, not machine-local) for the three xuid tests 47; with
// the machine-local test before the guest test 43; either with the exclusion test kept too 67/71 (140 bytes: the
// mask is then hoisted into AL as retail, but both guest tests stay); the existing iterate helper with that
// test 79/79 (102 bytes).
// Round 443: Two-use admission flags in byte/char/word/short storage 44/44/43/43;
// direct-success-return counterparts 52/52/51/51. All restored; early ESI, independent
// successor, mask hoist and failure exit remain.
// Round 45 (lane 16), `--fast`: struct assignment for the player copy 43; force-inline offline and
// exclusion helpers 43; the 64-bit id in a two-use local 50 (its load moves above the flag tests);
// the id test as an OR of its two dwords 83 (98 bytes); the result declared after the index 43.
// Compile-only screens (one use of the mask never hoists it): a do-while with a named successor, a
// `continue` filter, an admitted flag, the XUID view, and the null-identity test before or after the
// guest test (both hoist the 3 into BL but keep both guest tests).
// Round 111 (LTCG harness equal to the full listing): the one-use mask is hoisted into AL, with the result
// rematerialized at the exit as retail, when the guest test comes first in the chain, with `continue` filters in
// the while loop, or with `while (!result && index != NONE)`; those loops stay top-tested with index and successor
// sharing one register. A successor local with that loop condition gives retail's EBX successor and ECX/EDX
// scratch but no hoist, and moves the player argument to the stack. `--fast`: `continue` filters 57; the `!result`
// loop 57; it with a successor local 37 (0x1a303b and 0x2b393c then lose their matches: player passed on the
// stack, ret 8); the successor assigned in an else 57; a guarded do-while with a successor and early returns 49.
// Harness only: for loops with `index = next` (no hoist), an inline entry getter (identical), short index or
// successor, the found copy after the loop, result set before the copy.
// Round 98 (lane 7), `--fast`: `*iterator = index = entry->next` with an early return 52; the successor stored after
// the test on both paths 51; the `!result` loop with a successor local and `break` 57; a for loop with `continue`
// filters and `break` 51; one index with `continue` filters and an early return 55. The successor read through the
// table (`long next = g_4cf98c[index].next;` before the entry pointer, or `*iterator = g_4cf98c[index].next;` with the
// index read back) in the `!result` loop with `break` or with `else index = next` gives retail's AL mask and EBX
// successor, 36 at 127 bytes (not kept: an always-true loop test or a mixed read, no match); left are the top-tested
// loop (retail: guard, then push ebx and the mask, latch `cmp ebx, -1; je; mov esi, ebx`) and the entry computed after
// the successor load. Harness: the rotated loop with that successor and a `break` hoists the mask only with the guest
// test first (so it depends on the test block's frequency); test spellings, result and successor types change nothing.
// Harness: an `if (index != NONE)` guard or a do-while around the `!result` loop, the existing iterate helper on a local
// index, booleans computed before the test and (in the rotated loop) the copy after the loop all lose the hoist.
// Round 28 (lane 14), `--fast`: a for loop stepping the index from a successor local with `continue` filters and an
// early return 49 (aligned 27: index and successor share EAX, mask an immediate), with the && chain and an early
// return 49; the while loop with the chain inverted into a `continue` and an early return 52, with the found block
// returning directly 52; the successor for loop with `continue` filters, the result and `break` 51. The XDK's own
// XOnlineAreUsersIdentical is the only header macro with two uses of the guest mask (its null-identity form is the
// round 34 trial). Callers 0x1a303b/0x2b393c kept their matches in every preview.
// Round 99 (lane 8), one-file LTCG harness exact against the build: the mask is hoisted in every `!result` loop
// whatever the guest test's position, in `break` loops only with the guest test first, never with early returns.
// Real: `while (!result && index != NONE)` with a successor local and `index = next` after the test 37, also as a for
// loop with that increment 37 (both move the player argument to the stack, ret 8, and lose 0x1a303b and 0x2b393c);
// the found entry copied after the loop 43; a successor local with `break` 51; a guarded do-while with a successor 51.
// Harness: early returns with a successor 49, `while (true)` with a successor test 52/57, the loop ended by
// `index = NONE` in the found arm 43 (no hoist), every field through the table 69-71.
// Round 28 (m3 lane 2, fast with 0x1a303b/0x2b393c): `while (!result && *iterator != NONE)` indexing through
// the iterator 79 (102 bytes: the mask is hoisted into AL, but the loop reloads the iterator, which moves to
// EBX with `ret`, both callers lose); the `!result` loop with `index = *iterator` re-read at the end 42 (iterator
// in EBX, player on the stack, callers lose); `!result` loop with a successor local, result declared after the
// index 37 (the listed form: player on the stack, ret 8, `!result` not threaded, mask an immediate); the same
// with `index != NONE && !result` 53; a for loop stepping `index = *iterator` with `break` 51 (convention kept,
// index and successor share ECX, mask an immediate).
// Round 59 (fast; 0x1a303b/0x2b393c stayed matched): 1-8 unused locals at the top (diagnostic) 43 each;
// result set before the copy 43; the entry pointer declared at function scope 43; `*iterator = index =
// entry->next` without early returns 43; `g_4cf98c + index` 43; a two-use dword flags local 43 (all same code).
// Round 17 (lane 17, fast): the guest mask as a byte parameter of the exclusion helper 43 (propagated, still an
// immediate); `while (!result && index != NONE)` with a successor local and `else index = next` 57 (AL mask, but
// the loop stays top-tested in ECX); the record's four tests as one inline `&&` predicate 43 (identical), with
// early returns 40 (round 119's form, re-measured), that plus a successor stepped at the loop's end 48; a guarded
// do-while with a successor, `else index = next` and the `!result` test 51 (no hoist).
// Round 44 (lane 13, fast): `for (long index = *iterator; index != NONE; index = *iterator)` with
// `*iterator = entry->next;` in the body 51 (index and successor still share one register); `while (!result &&
// index != NONE)` without break, stepping by `index = *iterator` 42 (AL mask hoisted, but the iterator takes a
// register and the player goes to the stack: 0x1a303b and 0x2b393c stop matching), stepping by `index =
// entry->next` 49 (player in EBX, result kept in AL and tested each pass; callers lose their matches), that with
// `*player = entry->player` 49; no index local, `*iterator` read at each use 71 (109 B). All restored.
// Round 127 (lane 6, fast, with 0x1a303b/0x2b393c): `while (!result && index != NONE)` with a successor local and
// `index = next` at the end 37 again (no hoist in this build; player read from the stack, ret 8, callers 15/27);
// its test as `index != NONE && !result` 53; the test chain assigned to result, copy under `if (result && player)`
// 53 (callers 2/75); a const entry pointer 43 (identical); a for loop stepping `index = entry->next` with the
// entry declared outside 56 (callers 17/74). AL holds the result through our loop, so the mask has no free register.
// Round 73 (fast, with 0x1a303b/0x2b393c, both still matched): round 34's null-identity form
// `!(xuid == 0 && (xuid_flags & 3) == 0)` re-measured 67 as the base, then: on the byte flags 67, with the guest test on the dword too 67, as
// `xuid != 0 || (xuid_flags & 3) != 0` 67, with `(xuid_flags & 3) == 0` for the guest test 67, with the guest test
// moved ahead of the machine-local test 66 (all 140 bytes: the mask is hoisted into AL and false made again at the
// exits as in retail, but the inner guest test stays even right next to the outer one; VC does not thread it).
// Round 37 (lane 15, fast): the result assigned from the test chain on every pass (`result = tests;
// if (result) { copy; break; }`) hoists the mask exactly as retail (`push ebx; mov al, 3` after the guard,
// false remade at the loop exit) 59, but index and successor still share one register (entry copied into
// ESI at the loop top) and the loop head gets a jump over 16-byte padding (135 bytes); on it a successor
// local 59, the successor read through the table first 62 (successor EBX, index ECX), `index = *iterator`
// at the end 59, a for loop stepping `index = *iterator` 59 or from a successor 59, the entry pointer at
// function scope 59, every field read through the table 75, the loop over `*iterator` 70 (iterator then
// passed in a register, callers lose). `while (index != NONE && !result)` with a successor 53; `continue`
// filters with an early return 55; the `!result` loop assigning the result each pass with the copy after
// the loop 71 (top-tested, no hoist); the guest test 3rd/4th (diagnostic) 48/47, no hoist; the parameters
// swapped to (iterator, player) with both callers 43 (identical: player stays in EAX).
// Round 75 (lane 12, fast): player_configuration_cache_iterate on the iterator in the `!result` loop 79 (102
// bytes, callers lose), on a local index with `*iterator = index` after it 57; the plain while with
// `break` and the successor read through the table 60 (137 bytes); `*iterator = entry->next` with
// `index = entry->next` read again at the loop end 56 (callers lose); the successor carried and
// copied into the index at the top 43 (same code).
// Round 127 (lane 6, fast, with 0x1a303b/0x2b393c): `while (!result && index != NONE)` with a successor local and
// `index = next` at the end 37 again (no hoist in this build; player read from the stack, ret 8, callers 15/27);
// its test as `index != NONE && !result` 53; the test chain assigned to result, copy under `if (result && player)`
// 53 (callers 2/75); a const entry pointer 43 (identical); a for loop stepping `index = entry->next` with the
// entry declared outside 56 (callers 17/74). AL holds the result through our loop, so the mask has no free register.
// Round 36 (fast, with 0x1a303b/0x2b393c): no result variable, the loop ending on `index = next` and `return index !=
// NONE;` 54 (131 bytes, `setne al`; both callers drop to 2); the per-pass result with a successor local (as round 37)
// 59 (135 bytes; compile-only it hoists the mask into AL and builds the entry in place, but next shares the index
// register), the same with the result declared after the index 59, with `else index = next` 59; a named `admitted`
// bool with early returns and a successor local 49.
// Round 343: Real admission bitfields: dword bit2/bit4/both, word/byte both,
// then Boolean dword/word/byte both 43/43/43/43/44/50/52/44.
// Original record, helpers and complete body restored; eleven control matches survive.
// M296: Cheap-flag-gated two-use identity, nested/continue admission and break/direct
// success, then those four with single flag observation [43, 52, 43, 52, 43, 52, 43, 52]. Original admission restored.
// M303: Selected entry/player address result, before/after index declarations and break/single-exit completion: 51 in all eight forms, 133/127 bytes; Boolean result and native copy restored.
// N301: Cheap-gated complete native identity value, nested/continue with result/direct
// completion 52/56/52/56; native identity pointer counterparts 43/52/43/52.
// Value forms also regress both matched callers; all restored. Early ESI, successor and mask remain.
// Round 60 (m3 lane 1, fast; one-link harness matches the listing): the entry bound as a reference 43 (same code); a
// successor local read after the entry pointer, `index = next` at the loop end and `break` 51; a guarded do-while
// with it 51; the successor read through the table first in the plain while with `break` 60 (137 bytes: rotated
// loop testing the EBX successor, mask an immediate); an implied second guest test guarding the copy 40 (128 bytes,
// not kept: no hoist, the test stays). Harness: that second test as the found result or with early returns, no hoist.
// Round 45 (m3 lane 15, fast, with 0x1a303b/0x2b393c; al = aligned count against this body's 23; each with a separate
// successor local stepped at the loop's end): while or guarded do-while, `result = true; break;` 51 (al 31; no hoist);
// `result = tests; if (result) {copy; break;}` 59 (135 bytes, al 31; hoist as retail, ECX index and successor); early returns
// 49 (al 24; flags and identity registers retail's, EAX index and successor where retail has a hoisted mask and EBX); each
// with the entry declared outside the loop, or read through an inline index getter, the same code. Retail's register plan
// needs the mask hoisted first, which only the forms with a live result give.
// M310: Eager pure-Boolean cache/identity/player pairs, seven subsets, then complete eager eligibility [56, 86, 97, 51, 75, 104, 115, 123]. Original body restored.
// M332: Existing iterator on the local successor, pointer/declaration while, plain for,
// or native index admission, each with result/break or direct completion: 57/55/57/55/91/73/52/53.
// All regress; original walk, helper, publication and complete body restored.
// m3 lane 11 r63 (fast, with 0x1a303b/0x2b393c): `while ((index = *iterator) != NONE)` with early returns 65
// (112 bytes, both callers lose), with result/break 66 (111 bytes, callers lose); a for loop with a successor
// local stepped at the end and early returns 49; that as a while with the flag tests nested over a named
// 64-bit identity 49; `while (index != NONE && !result)` with no break 52 (callers lose). All restored.
// Round 76 (m3 lane 16, fast, with 0x1a303b/0x2b393c): an unsigned index 43 (same code); the two flag tests as an
// outer `if` around the record tests 43 (same code); the machine-local test on the high xuid dword
// (`xuid[1] >> 16`) 71 (110 bytes, both callers lose); the machine-local test as a 64-bit mask compare 44; as a
// `(word)` of the shifted xuid 49.
// Round 141 (m3 lane 5, fast, with 0x1a303b/0x2b393c): a self-call in the failure arm 72 (108 bytes, callers lose), with
// a single result 77 (103); the existing iterate helper in `while ((entry = iterate()) != NULL)` 79 (102 bytes, callers
// lose; mask hoisted ahead of the loop, iterator reloaded each pass), early return 79, `!result &&` test 79, a do-while on
// the entry 79, a for loop 56 (139), the per-pass result 86 (95), both 86, `*iterator != NONE` test 79; per-pass result
// stepping `index = *iterator` 59 (135, callers keep; hoist as retail, index and successor share ECX, entry copied to ESI,
// padded head), a guarded do-while 59; an index or iterator guard around the helper loop 73/73 (108, callers lose).
// Round 67 (m3 lane 10, fast, with 0x1a303b/0x2b393c): early returns with no result 52; a `short next` successor
// (the field's type) with `index = next` at the loop end and `break` 51, with early returns 49 (index, entry and
// successor still share EAX/ESI), with the per-pass result 59 (135 bytes: mask hoisted into AL as retail, but the
// successor takes ECX, not EBX, and the loop is entered by a jump to a padded head), stepping `index = *iterator` 59.
// Round 76 (m3 lane 16, fast, with 0x1a303b/0x2b393c): an unsigned index 43 (same code); the two flag tests as an
// outer `if` around the record tests 43 (same code); the machine-local test on the high xuid dword
// (`xuid[1] >> 16`) 71 (110 bytes, both callers lose); the machine-local test as a 64-bit mask compare 44; as a
// `(word)` of the shifted xuid 49.
// Round 50 (m3 lane 2, fast, with 0x1a303b/0x2b393c; al = aligned count, this body's 30 by the same count): two iterate-helper expansions around the loop: a for loop with early returns 64 (134 bytes,
// al 59, callers 17/29), with result/break 56 (139, al 37, callers 17/74); `while (entry && !result)` 83 (150, al 51, callers 3/76); a guarded do-while 56 (139, al 37, callers 17/74). The helper's NULL stays
// a live value and its test is not threaded away. Per-pass result with a successor local 59 (135, al 40), with the successor carried and copied to an inner index at the top 59 (135, al 32:
// flags and high identity dword as retail, next in EDX and low dword in EBX where retail has them the other way round); the successor read through the table, then the iterator store and the entry, then the entry
// and the iterator store, or after the entry, 62/62/62 (138, al 28, callers keep): next in EBX, flags in ECX, identity in EDX:ECX, mask in AL as retail, the index still in ECX through `lea esi` and the head still padded.
// Lead: retail's index, its product and the entry are one web in ESI, so the successor stays apart; the back-edge `mov esi, ebx; jmp` looks like a tail-merged copy of the entry computation.
// Round 86 (m3 lane 16, fast with 0x1a303b/0x2b393c): `*iterator` read at each use with a `!result` loop 79 (102 bytes), with
// `break` 71 (109), flag then copy after the loop 86 (95), per-pass result 70 (110), each the same with the parameters as
// (iterator, player) and all with the iterator in EBX, callers lose; a local index with the `!result` loop 57 in both orders;
// per-pass result with a successor, with `index = *iterator` and as a guarded do-while 59 each (135); a guarded `for (;;)` or
// `while (true)` ending `if (next == NONE) break; index = next;` 59 per-pass, 52 with `result = true; break;` (127); a done flag 54.
// M391: Eligibility scopes split at individual flag/identity/online/exclusion boundaries 1,3,4,5,1+3,1+4,1+5,3+4+5: 43/43/43/43/43/43/43/43.
// NOT kept: No exact native index/successor, mask-hoist or loop-exit repair. Original complete body restored.
// AC532: Complete complementary rejection in bool/byte/short/long, outer-result
// 43/43/43/43 and direct-return 52/52/52/52. NOT kept: no exact mask/successor
// allocation; original result, iterator, predicates and memcpy restored.
// M391: Eligibility scopes split at individual flag/identity/online/exclusion boundaries 1,3,4,5,1+3,1+4,1+5,3+4+5: 43/43/43/43/43/43/43/43.
// NOT kept: No exact native index/successor, mask-hoist or loop-exit repair. Original complete body restored.
// Round 213 (m3 lane 3; "h" = a one-file LTCG harness whose base reads 43, confirmed with fast where given): about 2,900
// combinations of loop, entry, test and copy forms lowest 37 (the listed `!result` loop, player on the stack). Closest by shape: a
// guarded `while (!result)` with `else if (next == NONE) break; else index = next;` 52 (fast and h): retail's latch and block layout
// (`cmp next, -1; je; jmp head`) on one web, no hoist; a guarded `while (true)` with the per-pass result, next read through a first
// lookup and the entry looked up after it 55h (68 at 140 bytes in fast): AL mask, EBX successor and the back-edge copy as retail, index
// still in ECX through `imul` and a padded head. Carried successor (`long index = next;` at the top) in the `!result` loop 57 (fast):
// index, product and entry in ESI, mask hoisted, copy at the head. The first flag test through the table 53h: hoists 4 into AL instead
// of 3 (the hoist follows the mask test's block frequency) and splits the successor. Implied index tests, not kept: `tests && index !=
// NONE` in the guarded `!result` loop 39 (fast, both callers 2), `index != NONE && tests` in the guarded `while (true)` 41 (fast, callers
// keep; `cmp ecx, -1` stays in the code). `index = *iterator` on the back edge 49-57h (forwarded before allocation, one web). An index
// predicate helper, the found copy through `&g_4cf98c[index].player` or a getter, every test through a second lookup 49-53h. Left: index
// and successor share a web in every form without a live re-use of the index, and only the forms with a live result hoist the mask.
// P350: Existing exclusion helper result in bool/byte/short/long, false/true defaults:
// 43 each. NOT kept: no early-save, independent successor, mask-hoist or exit repair;
// original predicate and complete body restored.
// M3 lane 4 r168 (one-file harness exact at 43, also at 37/42 for the forms that move the convention): names in three sort
// positions for index/entry/successor/result on the current, separate-successor and early-return loops 43/51/49 in all 97
// forms; a priming lookup before the loop and a second at its end with the tests as the loop condition 54 (retail's early
// ESI save, `imul esi, esi` and cross-jumped back edge `cmp next, -1; je; mov esi, next; jmp`, but the successor in EDX and
// no mask hoist); that as `for (;;) { result = tests; if (result) { copy; break; } ... }` 46 (retail's register plan, no
// hoist), with the validity test as `(xuid != 0 || guest)` 41 (132 bytes: prologue, `mov al, 3`, loop and exit as retail,
// the first guest test not threaded away); `(xuid != 0 || guest)` with early returns 44 (mask in BL), in the current loop 67
// (140); value-context tests with early returns 49, `result = tests; ... break` 59 (135); the guest test repeated after
// itself 71/43/38/70 (VC keeps both); an index-returning iterate helper in the condition 82 (99 bytes); the entry iterate
// helper before the loop and at its end 64/56; advance-until-valid do-while 79/55/48, `for (;;)` 65; a carried successor
// (`long index = next;` at the top) with value tests 59 (135), with early returns 52, the twice-used mask 67/51;
// XOnlineIsUserGuest on the xuid flags compiles as the byte test. Among these, the twice-used mask in the current loop 67
// and `result = tests; ... break` 59 re-measure listed forms (scores agree). All restored.
// Round 76 (m3 lane 10, fast, with 0x1a303b/0x2b393c): retail keeps index and entry in one register (`imul esi, esi`)
// and the successor apart in EBX; every successor form here coalesces the successor with the index instead. A successor
// declared at function scope with early returns 49; a guarded do-while ending on `next != NONE` with early returns 55
// (133 bytes); `index = next` right after the successor read, before the iterator store, with early returns 52;
// `while (index != NONE && !result)` with the successor and `else index = next` 54 (both callers lose); the entry as
// `g_4cf98c + index` with a successor and early returns 49. Re-measured: successor with early returns 49.
// P382: Complementary terminal no-player result in bool/char/signed-char/byte/short/word/
// long/dword: 51/51/51/51/60/60/58/58. NOT kept: 133-137 bytes move away from native 127,
// the result occupies EBX and AL mask/index/successor ownership still differs; wide forms lose both
// callers. Original complete body restored.
// Round 81 (m3 lane 1, fast with 0x1a303b/0x2b393c): the successor read through the table before the entry, early
// returns, `index = next` at the loop end 53 (134 bytes); that with `continue` filters (index stepped before them) 51
// (top-tested, no hoist); `continue` filters on the current body with the result and `break` 57 (the mask is hoisted into
// AL and false remade at the exit as retail, but the loop stays top-tested, `mov al, 3` in the prologue, index and
// successor share EDX); the table-first successor with the result, `break` and `index = next` at the end 60 (137 bytes);
// `continue` filters with the table-first successor 54 (hoisted; successor copied to ECX); diagnostic: the `continue`
// body as a guarded do-while 43 (main's code exactly, no hoist: the hoist with `continue` needs the top-tested loop).
// P390: Nullable-copy admission in positive/complementary bool/byte/short/long:
// 43/43/43/43/43/43/43/43. No exact early-save, independent successor, mask or failure-exit recovery;
// original nullable copy restored. NOT kept: no qualifying exact/native-shape gain; original complete
// body restored.
// M3 lane 4 r176 (fast with 0x1a303b/0x2b393c, both stay matched in every form; base 43 at 127): a guarded do-while
// with a successor local, the found arm returning true and `result = false` after the loop 49 (127 bytes); an early
// `return false` on an empty iterator, then an endless loop with a successor that returns true when found and false
// when the successor is NONE 55 (133); a guarded do-while with the successor declared outside it, result and break,
// and the step in the loop test 51 (127); a for loop stepping from a successor that is read into a short first 51
// (127); the current loop with four `continue` filters (the identity pair and the exclusion pair joined by `||`) and
// `return true` 52 (127). All restored.
// Round 70 (m3 lane 15, fast with 0x1a303b/0x2b393c, both kept their matches): match-only lead 39 (129 B, masked aligned 9
// against this body's 19): a priming lookup, then under `if (index != NONE)` `do { next = entry->next; *iterator = next;
// if (tests) { copy; result = true; break; } if (next != NONE) entry = &g_4cf98c[next]; } while (next != NONE);` gives
// retail's early ESI save and in-place `imul esi, esi`; left: the mask is not hoisted into AL, next takes EDX (retail EBX)
// and the back-edge lookup is not cross-jumped into the head. Not kept: the inner test repeats the loop's. On it: early
// returns 52 (133 B), a short next 63 (136 B, both callers 2), the flag tests as an outer if 39, the index reused for next
// 39, a per-pass result 39, `else if (next != NONE)` with a `!result` loop condition 39 (no hoist in any).
// Round 163 (m3 lane 6, fast with 0x1a303b/0x2b393c, both stay matched; aligned/masked control 30/33): the
// guest test spelled on the xuid_flags dword, through a byte cast of it, as two one-bit byte tests, the name
// test through the record's `name[0]`, and the dword guest test with `name[0]`: 43 each (same code; the mask
// stays an immediate). All restored.
// M3 lane 16 r108: register-masked aligned 33 against retail; every candidate form checked against this note was
// already listed (loop shapes, successor locals, result/early-return exits, the iterate helper, predicate helpers,
// test spellings, the `(xuid != 0 || guest)` validity that hoists the mask but keeps the first guest test); no untried
// natural form found, no trial run.
// Round 91 (m3 lane 1, fast with 0x1a303b/0x2b393c): an entry-pointer loop with `index != NONE ? &g_4cf98c[index] : NULL`
// lookups before the loop and at its end 58 (137 bytes, the NULL test stays); a guarded `while (!result)` with the per-pass
// result and `else if (next == NONE) break; else index = next;` 59 (135: mask hoisted into AL and false remade at the exit
// as retail, but index and successor share ECX behind a padded head); the priming lookup under the guard with that loop
// 39 (129: early ESI save and in-place `imul esi, esi` as retail; no hoist, successor in EDX, the back-edge lookup computed
// in EDX and copied, not cross-jumped into the priming one, the head padded); on it `index = next` before the back-edge
// lookup 39, the result set only in the found arm 39, the iterator read in the guard with a do-while on `!result` 39 (all
// the same code), `return true` in the found arm 52 (133: AL free but no hoist, flags in EAX), a `short` successor 63
// (136, both callers lose). Original body restored.
// m3 lane 7 r150 (fast with 0x1a303b and 0x2b393c, both stay MATCH; a one-link LTCG harness reproduces this body's code, 5 bytes
// of end padding short): retail sets AL false before the guard, hoists the mask into AL and makes false again at the loop exit;
// the early-return forms give retail's early ESI save but neither the hoist nor the EBX successor. New: a found-entry pointer
// with `index != NONE && !found` and the copy after the loop 56 (131 bytes); that with `!found && index != NONE` 55 (129 bytes);
// the iterator stored on both paths after the tests, with result and break 50, with an early return 55; the guest test ahead of
// the machine-local test with a successor local and early returns 44. Re-measured: `*iterator = index = entry->next` with an
// early return 52, a successor local with early returns 49, an early `return false` then a do-while 55, a found pointer with
// break 51 (133 bytes).
// MiniMax A (11-11): no new form, no lower score (43); this round's new forms measured 43 or 71: the exclusion mask as a variable of its own, declared before or inside the loop (43), and the three loop forms that read the iterator in the loop (71).
// MiniMax G (11-11): no new form, no lower score (43); retail reads the index straight into esi and scales it in
// place, keeps the successor in ebx and hoists the & 3 mask into al before the loop, while ours copies the index
// into esi, keeps the successor in edx and leaves the mask as an immediate; giving the successor its own local
// moves it to ecx instead of ebx (51, and 52 with the loop left on a none successor), and passing the mask in at
// the call site does not hoist it either.
// Round 173 (m3 lane 6, fast with 0x1a303b/0x2b393c): the listed early-return form re-measured 52 (no EBX; next ECX,
// flags EAX, mask immediate) and the listed `!result` loop with a successor 37 (player on the stack, ret 8, callers 15/27;
// mask still an immediate there); that loop with the player copied into a local at entry 37 (same, callers lose); the
// `!result` loop with no successor local 57; a guarded do-while with early returns and a `result` returned at the end 52;
// `result` declared after the index 43 (same code); the XUID read once into a local for its two tests 50 (callers 2/2).
// Also: the result as a byte 47 (129 bytes); the index as a short read from the iterator 54 (129 bytes, callers 2/2).
// Round 113 (m3 lane 16, fast with 0x1a303b/0x2b393c): a successor local stepped at the loop's end with `result = tests`
// and `return result` inside the found block 49; the same with the tests in a named bool and `return true`/`return false`
// 49; `result = tests` each pass with the index stepped at the top and `break` 59 (135 bytes); a for loop on
// `index != NONE && !result` assigning the result each pass and copying under `if (result && player)` 52 (callers 75/2);
// `*iterator = entry->next` with `index = *iterator` at the loop's end and early returns 49; early returns with the successor
// read through the table ahead of the entry pointer 53 (134 bytes); early returns with a successor local and the copy as an
// aggregate assignment 49; the same with the guest test as `XOnlineIsUserGuest` on the flags dword 49. Original body restored.
// m3 lane 3 r239: no trial run. Checked against this note and listed: early returns, a successor local stepped at the
// end, per-pass result, `continue` filters, priming lookups, the iterate helper, typed XUID tests, the null-identity
// validity, two-use flags/XUID locals, split eligibility scopes, result/index/successor widths and every loop shape.
// P451: Whole-loop long result 59; final name-word pointer / lazy offline result /
// lazy identity-presence result / nullable early completion / sole-return goto 43/43/43/43/43;
// memmove payload 53; explicit final Boolean return arms 52. NOT kept: no gain or equality;
// original body/helpers restored. Native ESI entry, EBX successor and AL mask remain.
// @retail 0x805e0
bool player_configuration_cache_next_recent_player(s_recent_player *player, long *iterator)
{
	bool result = false;
	long index = *iterator;
	while (index != NONE)
	{
		s_player_configuration_cache_entry *entry = &g_4cf98c[index];
		index = entry->next;
		*iterator = index;
		if (!(entry->flags & 4) && (entry->flags & 0x10) &&
			*(unsigned __int64 *)entry->player.xuid != 0 &&
			!xuid_is_offline(*(unsigned __int64 *)entry->player.xuid) &&
			!player_configuration_is_excluded(&entry->player) &&
			*(word *)&entry->player.unknown08[4] != 0)
		{
			if (player)
				memcpy(player, &entry->player, sizeof(s_recent_player));
			result = true;
			break;
		}
	}
	return result;
}







long __stdcall function_7fc80(const void *identity, long *position);

struct s_cache_property
{
	long unknown00;
	long type;
	long unknown08;
	long unknown0c;
};

struct s_cache_property_record
{
	byte identity[12];
	long type;
	long count;
	s_cache_property *properties;
};

// @retail 0x80940
void function_80940(long record_capacity, s_cache_property_record *records,
	long property_capacity, s_cache_property *properties, long buffer_capacity,
	byte *buffers, long *record_count, long *property_count, long *buffer_count)
{
	long record_index = 0;
	long buffer_index = 0;
	long property_index = 0;
	for (long i = 0; i < g_4cf978; i++)
	{
		s_player_configuration_cache_entry *entry = &g_4cf98c[i];
		if (entry->flags & 2)
		{
			if (record_capacity <= 0 || property_capacity < 4 || buffer_capacity <= 0)
				break;
			memcpy(records[record_index].identity, &entry->player, sizeof(records[record_index].identity));
			s_cache_property *first_property = &properties[property_index];
			records[record_index].type = 0x61;
			records[record_index].count = 4;
			records[record_index].properties = first_property;
			record_index++;
			record_capacity--;
			*(short *)&first_property->unknown00 = -3;
			properties[property_index].type = 4;
			properties[property_index].unknown08 = (long)&buffers[0x40 * buffer_index];
			property_index++;
			buffer_index++;
			property_capacity--;
			buffer_capacity--;
			for (long j = 0; j < 3; j++)
			{
				*(short *)&properties[property_index].unknown00 = (short)(j + 2);
				properties[property_index].type = 1;
				properties[property_index].unknown08 = 0;
				property_index++;
				property_capacity--;
			}
		}
	}
	if (record_count)
		*record_count = record_index;
	if (property_count)
		*property_count = property_index;
	if (buffer_count)
		*buffer_count = buffer_index;
}

void function_80d70(s_cache_property_record *records, long count);

// @retail 0x7fd10
void function_7fd10(long index)
{
	s_player_configuration_cache_entry *entry = &g_4cf98c[index];
	if (entry->previous_other != NONE)
		g_4cf98c[entry->previous_other].next_other = entry->next_other;
	if (entry->next_other != NONE)
		g_4cf98c[entry->next_other].previous_other = entry->previous_other;
	if (g_4cf97c == index)
		g_4cf97c = entry->next_other;
	if (g_4cf980 == index)
		g_4cf980 = entry->previous_other;
}

// @retail 0x7fd80
void function_7fd80(long index)
{
	s_player_configuration_cache_entry *entry = &g_4cf98c[index];
	if (entry->unknown58 != NONE)
		g_4cf98c[entry->unknown58].next = entry->next;
	if (entry->next != NONE)
		g_4cf98c[entry->next].unknown58 = entry->unknown58;
	if (g_4cf984 == index)
		g_4cf984 = entry->next;
	if (g_4cf988 == index)
		g_4cf988 = entry->unknown58;
}

// @retail 0x7fc80
long __stdcall function_7fc80(const void *identity, long *position)
{
	long middle = 0;
	long lower = 0;
	long upper = g_4cf978 - 1;
	long result = NONE;
	bool last = false;
	while (result == NONE && !last && lower <= upper)
	{
		last = lower == upper;
		middle = (lower + upper) >> 1;
		long comparison = memcmp(identity, &g_4cf98c[middle].player, 12);
		if (comparison == 0)
			result = middle;
		else if (comparison < 0)
			upper = middle - 1;
		else
			lower = ++middle;
	}
	if (position)
		*position = middle;
	return result;
}

struct s_cached_player_identity
{
	dword values[3];
};
struct s_cached_player_source
{
	word name[32];
	dword field40;
	union
	{
		dword field44_47;
		struct
		{
			byte field44;
			byte field45;
			byte field46;
			byte field47;
		};
	};
	unsigned __int64 field48;
	byte unknown50[0x70 - 0x50];
	long field70;
	long field74;
	long field78;
	byte unknown7c[3];
	byte field7f;
	byte unknown80[0x10];
};
struct s_cached_player_view
{
	s_cached_player_identity identity;
	word name[32];
	dword field4c;
	byte field50;
	byte field51;
	byte field52;
	byte field53;
	byte unknown54[2];
	byte field56;
	byte unknown57;
};
struct s_cached_player_record
{
	s_cached_player_identity identity;
	byte source_bytes[0x90];
};

// @retail 0x7f8b0
void function_7f8b0(s_cached_player_view *result, const s_cached_player_identity *identity,
	const s_cached_player_source *source, const s_cached_player_view *previous)
{
	memset(result, 0, sizeof(*result));
	result->identity = *identity;
	memcpy(result->name, source->name, sizeof(result->name));
	result->field4c = source->field40;
	result->field50 = source->field44;
	result->field51 = source->field45;
	result->field52 = source->field46;
	result->field53 = source->field47;
	byte index = source->field7f;
	if (index == 0xff && previous)
		result->field56 = previous->field56;
	else
		result->field56 = index;
}

// @retail 0x7fdf0
void function_7fdf0(long index)
{
	if (g_4cf97c != index)
	{
		function_7fd10(index);
		if (g_4cf97c != NONE)
			g_4cf98c[g_4cf97c].previous_other = (short)index;
		s_player_configuration_cache_entry *entry = &g_4cf98c[index];
		entry->previous_other = NONE;
		entry->next_other = (short)g_4cf97c;
		g_4cf97c = index;
	}
}

// @retail 0x7fe40
void function_7fe40(long index)
{
	if (g_4cf984 != index)
	{
		function_7fd80(index);
		if (g_4cf984 != NONE)
			g_4cf98c[g_4cf984].unknown58 = (short)index;
		s_player_configuration_cache_entry *entry = &g_4cf98c[index];
		entry->unknown58 = NONE;
		entry->next = (short)g_4cf984;
		g_4cf984 = index;
	}
}




struct s_player_appearance;
bool function_153750(s_player_appearance *appearance);

// @retail 0x80660
bool function_80660(void)
{
	bool valid = true;
	if (g_4cf970 != 8)
		valid = false;
	if (g_4cf978 < 0 || g_4cf978 > 350)
		valid = false;
	for (long i = 0; valid && i < g_4cf978; i++)
	{
		s_player_configuration_cache_entry *entry = &g_4cf98c[i];
		if (i > 0 && memcmp(&entry[-1].player, &entry->player, 12) >= 0)
			valid = false;
		if (valid)
		{
			byte appearance[16];
			memset(appearance, 0, sizeof(appearance));
			s_cached_player_view *view = (s_cached_player_view *)&entry->player;
			*(dword *)appearance = view->field4c;
			appearance[4] = view->field50;
			appearance[5] = view->field51;
			appearance[6] = view->field52;
			appearance[7] = view->field53;
			valid = function_153750((s_player_appearance *)appearance);
			if (valid)
				valid = !(view->identity.values[2] & 3) && view->identity.values[2] != 0xbad00000;
		}
	}
	if ((g_4cf97c == NONE || g_4cf980 == NONE) && g_4cf97c != g_4cf980)
		valid = false;
	if (g_4cf97c != NONE && (g_4cf97c < 0 || g_4cf97c >= g_4cf978))
		valid = false;
	if (g_4cf980 != NONE && (g_4cf980 < 0 || g_4cf980 >= g_4cf978))
		valid = false;
	if ((g_4cf984 == NONE || g_4cf988 == NONE) && g_4cf984 != g_4cf988)
		valid = false;
	if (g_4cf984 != NONE && (g_4cf984 < 0 || g_4cf984 >= g_4cf978))
		valid = false;
	if (g_4cf988 != NONE && (g_4cf988 < 0 || g_4cf988 >= g_4cf978))
		valid = false;
	if (valid && g_4cf97c != NONE)
	{
		dword visited[11];
		memset(visited, 0, sizeof(visited));
		long index = g_4cf97c;
		for (long j = 0; valid && j < g_4cf978; j++)
		{
			if (visited[index >> 5] & (1 << (index & 31)))
				valid = false;
			else
			{
				visited[index >> 5] |= 1 << (index & 31);
				s_player_configuration_cache_entry *entry = &g_4cf98c[index];
				if (j == 0)
				{
					if (entry->previous_other != NONE)
						valid = false;
				}
				else if (entry->previous_other < 0 || entry->previous_other >= g_4cf978)
					valid = false;
				if (j + 1 == g_4cf978)
				{
					if (entry->next_other != NONE || index != g_4cf980)
						valid = false;
				}
				else if (entry->next_other < 0 || entry->next_other >= g_4cf978)
					valid = false;
				else
				{
					long next = entry->next_other;
					if (g_4cf98c[next].previous_other != index)
						valid = false;
					index = next;
				}
			}
		}
	}
	if (valid && g_4cf984 != NONE)
	{
		dword visited[11];
		memset(visited, 0, sizeof(visited));
		long index = g_4cf984;
		for (long j = 0; valid && j < g_4cf978; j++)
		{
			if (visited[index >> 5] & (1 << (index & 31)))
				valid = false;
			else
			{
				visited[index >> 5] |= 1 << (index & 31);
				s_player_configuration_cache_entry *entry = &g_4cf98c[index];
				if (j == 0)
				{
					if (entry->unknown58 != NONE)
						valid = false;
				}
				else if (entry->unknown58 < 0 || entry->unknown58 >= g_4cf978)
					valid = false;
				if (j + 1 == g_4cf978)
				{
					if (entry->next != NONE || index != g_4cf988)
						valid = false;
				}
				else if (entry->next < 0 || entry->next >= g_4cf978)
					valid = false;
				else
				{
					long next = entry->next;
					if (g_4cf98c[next].unknown58 != index)
						valid = false;
					index = next;
				}
			}
		}
	}
	return valid;
}

struct s_profile_record;
void function_1537f0(s_profile_record *record);

// score 18 (was 18): the loop's entry pointer is based at the entry's link run (EDX = entry + 0x5c) where
// retail bases it at the view's field 0x4c (entry + 0x4c), so every entry store's displacement differs, and the
// view's byte stores and the link stores come out in another order (ours makes the 0x52 byte and the NONE first).
// MiniMax D (11-04): no new form, no lower score; the identity as three named dwords instead of an array
// 53 (same code), it still folds into registers and the frame stays 0x98.
// MiniMax E (11-05): no new form, no lower score; splitting the source record's 8-byte field into two
// dwords to make it 4-aligned leaves the record at +0x18 and the frame at 0x98, and an index loop gives 78.
// MiniMax C (11-08): no new form, no lower score.
// Earlier (score 53, was 53): frame 0x98 versus 0xa0: the three identity words fold away (retail stores them at
// +0x10 and reads them back for the copy, with the source record right after at +0x1c); ours spills a
// 350 countdown to +0x14 where retail compares the entry cursor with the array end (signed `jl`);
// packed source-byte carriers and initialization homes differ.
// Forward entry pointer 133; chained source resets 149; link-word fill 157; signed byte sentinel 149; unsigned entry index 133 (plain).
// Existing record initializer with zero tail 130; short/word counters 133/133;
// counters-before-version stores 132; separate link stores 133; initializer and store order 129 (plain).
// Array-end bound 129; workspace names 129; native identity zero 129; signed induction 122; array-end/native zero 129; signed/native zero 122; signed identity-array fill 122.
// Contiguous tail clear with sentinel restored 73; identity after record helper 122;
// fixed do loop with record helper 122; tail clear plus late identity 73;
// function-scope identity with record helper 122 (fast); tail clear 73 plain.
// Round 167: actual initializer record 73; native identity in that record context 74;
// source function scope/declaration-first 73/73; ordinary inline view on native/original
// source 73/73; out-of-line view on native/original source 80/79. Original body restored.
// With contiguous tail reset: identity/source/both names 73 each; signed byte sentinel 73;
// word ordinal 94; record/view/both force-inline declarations 73 each.
// Immutable identity with tail clear 74; source-before-identity names 73; source-only outer scope 73; identity between initializer and tail 73; tail-clear entry-end traversal 94.
// Complete destination clear 73; contiguous destination-tail clear 67 kept; destination first/after identity 75/75; links before view 98; tail before view 95; direct indexed destination 73; tail fill with early links 100.
// Ordinary record inline/native identity 67/67; immutable source/identity/both once 95/83/86;
// immutable source with native identity 95; split source-tail fills 115, with native identity 115.
// Contiguous link fill 67; link fill with native identity/late identity/intermediate identity 67/67/67;
// link fill with outer source/lexical source/unsigned ordinal/NONE byte 67/67/87/67.
// Complete zero suffix at field44/field40/field48/unknown50 67/67/67/67;
// suffixes at field70/field74/field78 give 99/89/83; whole source suffix 125. Frame remains 0x90.
// Native header zero fields 40/44/40+44/48/40+48/44+48/all 74/90/87/67/74/90/87;
// complete sixteen-byte header fill 154; source/identity homes remain unchanged.
// Entry zero/first sentinel pair/second pair chains and all pair subsets 67 each;
// complete endpoint sentinel chain 67. Frame 0x90 and packed source spills remain.
// Round 136: library copy of the identity in 0x7f8b0 67 (same code); XUID-typed identity 67;
// XUID id/flags zeroed by field 95 (flags store hoisted). Source byte 0x7f declared signed char
// with the tail clear and sentinel removed 78 at 238 bytes (no tail stores, source bytes shifted
// out of EBX as retail; identity still folded); with flags/timestamp as two stores 80 at 236;
// on that: record-typed source local 80, source declared first 80, memset through a sized helper 80,
// 64-bit id + flags identity type 80; per-entry force-inline helper 132.
// Round 146: identity byte/word/signed-word/signed-dword/header/byte-identity/word-flags/byte-
// flags views 67 each.
// Complete shared view library/native copies 128/128; per-visit library/native copies 128/128; shared-view array-end 126; ended input scopes 128; contiguous links 130; complete destination clear 148. Original body retained.
// Round 76 (a one-file harness with the record initializer's file reproduces this listing exactly):
// tail clear and sentinel removed 78 (238 bytes); source 64-bit field as two dwords 67 (the frame still
// realigns); identity zeroed after the record initializer 67; a one-element identity array 67; identity
// declared before the loop and zeroed in it 67; harness only: `= { 0 }`, XUID-based identity types,
// identity zeroed by its XUID fields (only the flags word stays in memory, its store hoisted) - none
// keeps the identity in memory.
// Round 7 (lane 15): identity zeroed by one chained assignment 95 (282 bytes); source declared first 67;
// both declared before the zeroing 67; `= { 0 }` in the real build 67; 0x7f8b0 defined in another file
// (inlined across files) 67. The
// harness also folds a dword[3]/byte[12]/union identity, a zeroing loop, a 64-bit identity type and an
// opaque entry pointer.
// Round 34: retail's frame is one 0x9c-byte object padded to 0xa0 (identity at +0x10, record right
// after at +0x1c): with the identity and the record as two members of one local struct the record's
// block store stops the zeros being forwarded, and the identity is stored and read back as retail
// does: 101 with the tail clear, 45 at 259 bytes without it and with the record's 64-bit field as two
// dwords (4-aligned record), 52 with it 8-aligned; links in retail's store order 45, the global NONE
// stores chained 45. Not kept: no existing type has that layout, and an invented grouping struct is
// not an accepted form. What then remains is NONE hoisted into EBP across the loop (retail keeps the
// record's first zero dword there).
// Round 12 (lane 13): tail clear and sentinel removed with the byte type kept (the initializer's NONE
// byte is forwarded as retail does) 78 at 238 bytes (aligned 43 against 51); with the flags as two
// stores / links as separate statements / the 64-bit field as two dwords 80 each at 236; the entry
// pointer declared first 67, with the tail removed 78.
// DL199: Signed high header bytes separately/pair, low pair and their three-group combinations all
// 67; all four signed bytes plus a signed first word 67. Missing identity homes and frame 0x90
// remain. Every real source type, initializer, initialized byte and all 350 visits restored.
// Round 45 (lane 16): the source as the existing 4-aligned player properties type (0x90, no 64-bit
// member), tail clear and sentinel removed 78 (238 bytes); with an XUID identity 78; with the tail
// clear kept 67, plus an XUID identity 67 (identity still folded each time; retail's +0x1c source
// needs that 4-aligned type); the entry pointer as the loop variable 98 (292 bytes).
// Round 106: identity value-initialized 67; that without the tail clear and sentinel 78 (238 bytes); identity
// declared and zeroed after the entry pointer 67; no tail clear with the links and flags stored before the
// 0x7f8b0 call 81; the entry pointer passed as the view 67.
// Round 6 (lane 17): identity zeroed once before the loop 83 (280 bytes); view built in a local view and
// copied into the entry 128 (311 bytes); without the tail clear and sentinel: links as four statements in
// retail's store order 78, identity zeroed after the entry pointer 78, identity zeroed once before the
// loop 81 (244 bytes). Original body restored.
// Round 111: flags and timestamp as two plain stores 73 (273 bytes); identity declared and zeroed right before
// the entry pointer 67; the link stores as four statements in retail's order 67; the identity as the XDK XUID
// type 67; the entry pointer as `g_4cf98c + i` 67. An LTCG harness copy of round 34's two-member local object
// again gives retail's identity stores and offsets (259 bytes), with NONE then hoisted into EBP.
// Round 98 (lane 7), `--fast`: the XUID identity zeroed by its two fields with the existing 4-aligned
// `s_player_properties` source and no tail clear 80 (249 bytes: retail's 0xa0 frame, but only the flags dword stays in
// memory, its store hoisted out of the loop, and NONE moves into EBP); the flags field zeroed first 75 (251); the
// identity zeroed dword by dword 80 (249); the XUID by memset with flags and timestamp as two stores 80 (236); the
// first form with those two stores 85 (241).
// Round 28 (lane 14), `--fast`: the tail clear dropped with the 0xff sentinel kept 78 (238 bytes); the identity
// initialized `= { { 0, 0, 0 } }` 95 (282); the source as a one-element array 67 (same code); the tail cleared as
// two memsets around the sentinel byte 125 (308); a plain `dword[3]` identity cast at the call 67 (same code, the
// real build folds it as the harness did). Every existing struct with a 0x90-byte properties member was checked
// (s_network_message_player_add, s_session_interface_user, s_network_session_player, the player settings
// source/entry): none puts a 12-byte identity immediately before it, so round 34's two-member layout has no
// existing type.
// Round 99 (lane 8): identity declared and zeroed once before the loop 83 (280 bytes, still folded), with the XUID
// type 83 (280); the source declared before the identity 67; the XUID identity per iteration 67; the identity zeroed
// after the entry pointer 67. No existing type has an identity followed by the 0x90-byte record.
// Round 44 (lane 1, fast): an XUID identity with the source declared first 67; the link stores before the
// identity and the 0x7f8b0 call 100 (284 bytes); the entry's flags/time clear first 71 (273); a function-scope
// XUID identity zeroed in the loop 67; the XUID identity zeroed right before the call 67. A compile-only build
// folds the identity even with the record initializer out of line, so only an identity inside a larger local
// object stays in memory (round 34); no existing 0x9c-byte type was found.
// Round 101 (lane 7), `--fast`: identity `= { 0, 0, 0 }` 95 (282 bytes); a const identity `= { 0 }` 67; assigned from a
// value-initialized temporary 67; memset of its member array 67; the zero identity copied into the entry first and the
// entry's own identity passed to 0x7f8b0 97 (284 bytes).
// MiniMax E (10-31): the record's own tail clear and byte sentinel dropped (the record initializer
// already leaves field 0x7f at 0xff) 78 (238 bytes, plain); the entries walked with a pointer 53 (255 bytes).
// MiniMax E (10-31): the link words' -1 is the same constant as the globals' NONE and so stays in EBP; retail
// keeps it in EAX and gives the record's field 0x40 its own register; 45 (259 bytes).
// MiniMax E (10-31): the identity and the record in one two-member local struct (identity, then record) and the
// entries walked with a signed index 45 (259 bytes, plain); not kept (a made-up struct that sets the stack homes;
// no existing type pairs them). The record's field 0x40 then shares a register with the field 0x44 word and the
// NONE constant lives in EBP, so the entry-tail clear is an LEA with two ECX stores.
// Round 49 (lane 10): the entries walked with a pointer `for (entry = g_4cf98c; entry < g_4cf98c + 350;
// entry++)` with the record's tail clear and 0xff sentinel kept 98 (292 bytes); the identity declared and
// zeroed after the entry pointer 67; the existing 4-aligned s_player_properties as the record with the tail
// clear dropped 78 (238 bytes), with an XUID identity zeroed after the record initializer 78 (238); the index
// loop with the tail clear dropped 78 (238). Kept: the pointer walk with the tail clear and sentinel dropped
// (the tail clear zeroed fields 0x7c-0x8f the inlined view copy never reads, and the sentinel re-stored the 0xff
// the record initializer already leaves in field 0x7f) 53 (255 bytes, plain). On that: `entry !=
// g_4cf98c + 350` 53, the record before the identity 53, an XUID identity 53, the identity `= { 0 }` 53. The
// identity is still folded in every form.
// Round 327: Carried entry pointer with direct long/unsigned-long/short/word count-up and
// countdown bounds, 53 each at 255 bytes. Identity homes still fold; original pointer walk and
// all initializers restored.
// Round 36 (fast, with 0x7f8b0/0x1537f0): retail forwards the record's memset zeros into EBP/EBX but stores and
// re-reads the identity, and its `cmp edx, end; jl` (signed) is the strength-reduced test of an index loop; ours
// counts 350 down in a frame slot. Index loop with every field through `g_4cf98c[i]` 77 (238 bytes); the identity
// zeroed after the record initializer in an index loop 78 (238); the links as four statements in retail's store
// order on the pointer walk 53 (same code); flags and timestamp as two stores on the pointer walk 62 (253); the index
// loop with both 80 (236). The compile-only build cannot inline 0x1537f0 (another file), so it does not reproduce this.
// DL233: Actual four-aligned producer record on the pointer/no-tail graph, native/library
// identity zeroing, identity before/after the producer, destination tail before/after view:
// 53/53/53/53/62/62/63/63. Original source type, forward declarations and full body
// restored; identity still folds and frame remains 0x98 versus 0xa0.
// Complete cache-entry construction including metadata, native/library full-record copies,
// live/ended input scopes and metadata before/after payload: 92/92/92/92/81/81/81/81.
// Original complete body restored; identity homes and signed end-bound remain.
// Round 343: Four-aligned profile/property sources with original/late mutable or immutable
// identity: 53/74/53/74/53/74/53/74. Original types, includes, initializer tuple, pointer walk and
// every byte restored.
// Round 352: Native identity constructors with library/chained field zeros, per-visit/outer
// lifetime and ordinary/forced expansion: 53/83/53/83/72/72/72/72. Identity homes still fold; all
// constructors, original type, initializer calls and 350 visits restored.
// DH274: Native full-byte bitfields at the four source header bytes, separately, as low/high pairs, together, and together with a signed first word: eight scores 53. Every file sibling also ties its preview. Original type/body restored; three identity homes and frame 0xa0 remain unresolved.
// DH276: current pointer walk with adjacent link library fill, late/early flags
// and late/early links, each with separate/chained global sentinels:
// 66/66/72/72/67/67/74/74. All restored; identity words still fold and
// frame 0x98/source-header homes remain. Both original initializers retained.
// N301: Pointer/no-source-tail invariant identity/source/both/native immutable identity
// 83/79/82/79; each with the destination zero-tail before construction 85/82/88/82.
// All restored; identity homes still fold and the native entry-end bound remains absent.
// Round 60 (m3 lane 1, fast; the one-link LTCG harness with 0x1537f0's file matches the listing): the identity
// zeroed by the memset whose return value is passed to 0x7f8b0 keeps the identity in memory (frame 0xa0 as retail; a
// steering form, match-only), but its zeros are stored after the record initializer and a 4-byte pad sits below it
// (identity +0x14, record +0x20; retail +0x10/+0x1c): index loop 53 (261 bytes), with the 4-aligned s_profile_record
// record 53 (261), with the source declared first 53 (261); the index loop with s_profile_record and a plain memset 78
// (238); the identity as a const reference bound to a value-initialized temporary 53 (same code). Harness only: an
// identity zeroed by a constructor, a `&T()` temporary argument and a `T *const &` bound to `&identity` all fold it.
// P265: Pointer/no-tail zero-fill return, four identity/source lifetimes and native/four-aligned
// source, 83 each; early returned-pointer identity with pointer/index traversal and native/four-aligned
// source 81/81/55/55. Original type/body restored; identity homes and end-bound remain.
// M312: Signed source sentinel with aggregate/late identity and native metadata-zero subsets 0-7:
// 53/53/53/53/62/62/62/62. Original body restored.
// M314: Signed source sentinel with full-byte header bitfields 44/45/46/47/44-45/46-47/all/44-46:
// eight 53s. Original real type and complete body restored; identity homes still fold.
// M328: Signed entry distances from start/end, strict/inclusive bounds, each with
// per-visit/reused source workspace: 100/100/85/86/100/100/85/86. Original complete
// body restored; identity homes fold and the native signed entry end remains.
// Round 76 (m3 lane 16, fast): `entry <= &g_4cf98c[349]` 53 (same code); flags and timestamp cleared by one
// chained assignment 62 (253 bytes); the entry pointer declared before the for 53; the four global NONE stores
// chained 53; an index loop with an entry pointer local 78 (238 bytes).
// Round 61 (m3 lane 10, fast): the identity pointer taken from memset's return in its own statement before the
// record initializer (zeros now stored before the record) on the pointer walk 81 (276 bytes); with an index loop
// 55 (259 bytes: frame 0xa0 and retail's `cmp edx, end; jl` exit, but the identity at +0x14 with a 4-byte pad under
// it, the record at +0x20 and the pointer in EBP); that with the 4-aligned s_player_properties record 55; the source
// declared before the identity 55; a function-scope identity 55; the identity named to sort after the source 55.
// Round 57 (m3 lane 13, fast): the 0x7f8b0 identity copy as memcpy 53 (same code); the identity as a plain
// `dword identity[3]` (the 0x18fe9e form) zeroed by memset 53, declared after the source 53, `= { 0, 0, 0 }` 74
// (254 bytes); an index loop with that array 78 (238 bytes). The identity still folds in all of them.
// Round 141 (m3 lane 5, fast): the identity and a 36-dword record in one local object (not an accepted form without a
// match) on an index loop 45 (259), with flags and time as two plain stores 49 (258 bytes, aligned 33: left NONE hoisted
// into EBP and the record's two zeros sharing EBX); the real record type in that object 52; pointer walk 86 (272); on the
// 49 form: four link statements, `(short)NONE`, `-1`, `0xffff`, globals as `0xffffffff` or chained, the object outside the
// loop or first in the function, the entry after the initializer, a do-while, memset of the object's start 49 each; the
// entry read through the table 46; an entry cursor beside the count 84 (270). Probes: globals not NONE 38 (EBP takes the
// record zero as retail, so the hoist comes from the global stores sharing NONE with the links); links not NONE 58; a
// memset of the globals 74 (260).
// Round 68 (m3 lane 1; "h" = a one-link LTCG harness of this file with unknown_153750, listing equal to the full build's): the source
// record under `#pragma pack(push, 4)` 53h (same code: the frame stays aligned for its 64-bit member, but the record may then sit at a
// 4-aligned offset). With that record and an index loop: the identity pointer from memset's return in a local 55h (259 bytes), as the
// call's argument 53h (261); the identity and record as members of one local object 44h (259 bytes), the closest shape yet: frame 0xa0,
// identity +0x10, record +0x1c and every line through the identity copy as retail (the same object without the pack 51h, on the pointer
// walk 85h). Not kept: nothing in the repo is that object, and a struct made up to group locals is not an accepted form. Left on it: NONE
// hoisted into EBP (`or ebp, -1`), so field 0x40 stays in memory where retail keeps it in EBP, and the flags clear. On it: flags and time as
// two stores 49h (257 bytes, aligned 33), the other order 49h; links as statements in two orders, chained or paired, heads chained or
// copied from each other, links and heads through by-value helpers: all 44h (same code); heads after the loop 69h. Diagnostics, value-wrong:
// the hoist ends at 4 links + 2 heads (37h, 258 bytes) or 3 links + 4 heads (39h with two flag stores); with the hoist gone and two flag
// stores 29h, left the byte-store schedule around the record's `rep movsd`. Retail does not hoist NONE: it reloads `or eax, -1` in the loop.
// Y597: Current pointer/no-tail graph with whole source-header zero field subsets
// 1/2/4/3/5/6/7, then contiguous header fill: 53/77/53/86/53/77/86/115.
// NOT kept: no exact publication graph or gain; full original body and initializers restored.
// P332: Named actual array endpoint, mutable/const pointer, strict/equality
// termination and per-visit/outer input storage: eight 53 at 255 bytes; both
// initializers stay exact. NOT kept: no gain; original body restored, identity
// homes still fold, frame 0x98 and spilled countdown remain.
// P334: Existing three-word identity standard fill/fill_n/construct-range contracts,
// forward and backward: 100/101/134/120/106/102/140/116. NOT kept: frames 0xa8/0xb0 versus native
// 0xa0 and 281–310 bytes versus 258; both original initializers and complete body restored.
// DH358: Existing unused eight-byte source field as signed integer, native SDK integer/time
// records, dword/word/byte field arrays and double: eight scores 53; every file control unchanged.
// NOT kept: identity homes still fold and frame 0x98/end-bound remain; original record and body restored.
// M393: Positive pointer do latch, strict then equality endpoint, per-visit/outer identity/source declaration pairs:
// 53/53/53/53/53/53/53/53. NOT kept: nonexact lifetime/latch forms; identity homes still fold and native frame/end-bound remain.
// Original complete body restored.
// T57: Plain linkage-array reset, long/short/dword/word indices, forward then reverse
// 66/66/66/66/62/62/62/62. NOT kept: no exact or qualifying structural gain; original complete body restored.
// Y597: Current pointer/no-tail graph with whole source-header zero field subsets
// 1/2/4/3/5/6/7, then contiguous header fill: 53/77/53/86/53/77/86/115.
// NOT kept: no exact publication graph or gain; full original body and initializers restored.
// Round 213 (m3 lane 3; "h" = a one-file LTCG harness with 0x1537f0's file, base 53): on the invented two-member object (not an accepted
// form) 45 (259 bytes, fast), two flag stores 49 (258, fast), links through 2-byte memset 43 at 259 and 43 at 258 (fast; NONE stays in EBP,
// only the flag tail changes). `do {} while (false)` around, before or after the loop, the heads or the body, and between any link or head
// stores 45/49h (no change); the record's NONE stores removed or spelled 0xff in a hand copy 45/49h (no change: dead stores do not count
// toward the hoist); heads through 4-byte memset 45/49h, through two 8-byte memsets 86-93h (272-274 bytes). On the current pointer walk,
// links through 2-byte memset 53 (fast, same code).
// DL273: Whole-array complete default entry, range/count fill then native/library copies,
// constructed after/before head resets: 76/76/92/92/75/75/90/90. NOT kept: nonexact
// hoisted record storage; original per-visit initialization and pointer walk restored.
// Round 133 (m3 lane 7, fast): index loop with every field through `g_4cf98c[i]` and the 8-byte tail kept 77 (238 bytes:
// retail's `cmp edx, end; jl` exit, identity still folded); on it identity `= { 0 }` 77, the record initialized before the
// identity 77, an entry pointer after the record initializer 78 (238); a short index 53 (the countdown returns).
// P380: Identity retained across the loop edge and reset on each visit, before/after metadata,
// library/aggregate entry zero and per-visit/outer source; four later traversal-entry initializations.
// Scores 87/87/87/87/87/87/87/87/87/87/87/87. NOT kept: all twelve score 87 at 273/258; native frame
// 0xa0 recovers, but identity/source slots, countdown and initial zero placement still differ.
// Original complete body restored.
// Round 81 (m3 lane 1, fast with 0x7f8b0): an index loop with an XDK XUID identity zeroed by memset 78 (238 bytes);
// the record initialized before a plain dword[3] identity on the pointer walk 53 (same code); an index loop through
// `g_4cf98c[i]` with the link stores in retail's order 77 (238); an index loop with an entry pointer, both locals
// declared before their initializers, the links in retail's order and flags/time as two stores 80 (236); the pointer
// walk stepped beside a count-up index 53 (same code). Retail's frame (0xa0, 8-aligned, identity +0x10, a 4-aligned
// record at +0x1c, 4 bytes free at the top) is exactly one 0x9c-byte object {qword, dword, 0x90-byte record}; no
// existing type has that layout, so the identity still folds.
// P387: Whole identity-record standard fill/fill_n/uninitialized-fill/uninitialized-fill_n,
// per-visit then outer raw storage: 110/110/116/108 twice. NOT kept: native identity homes
// still fold or move into a larger frame; 295-300 bytes versus 258. Both original initializers,
// per-visit fills, pointer walk and complete body restored.
// MiniMax C (11-09), 45 -> 18: the four link shorts are now written through a pointer to
// the entry's run of links, so their common base frees the register that holds the -1 and lets it
// be made afresh in EAX before the four stores as retail does, and the flags word and the word
// after it are set with two plain stores, which gives retail's two direct dword stores; the code
// is now 258 bytes, retail's length. Left: our tail stores sit 0x10 above retail's (our run of
// links begins further into the entry), the entry's byte stores come out in another order and
// with the two shifts in the other registers, and the walk's end compare carries the address of
// our own array.
// MiniMax C (11-09), 53 -> 45: the identity and the source now sit in one per-visit
// record, so the identity's address reaches 0x1537f0 and 0x7f8b0 and its three zero words and
// the three loads that copy them into the entry survive as in retail; the walk is an index loop
// over the entries, which keeps retail's compare of the entry pointer with the array end (our
// own array stands elsewhere, so the immediate still differs) and drops the frame countdown, so
// the frame is 0xa0 with the identity at +0x10 and the source at +0x1c as in retail. Left: the
// NONE constant stays in EBP across the walk where retail re-materialises it in EAX, the
// entry's byte stores come out in another order, and our code is 259 bytes against retail's 258.
// MiniMax C (11-09), 53 -> 53: the identity's three words still fold away, so the frame
// is 0x98 against retail's 0xa0, and our walk ends on a countdown kept in a frame slot where
// retail compares the entry pointer with the array end (signed). Untouched this round.
// Round 149 (m3 lane 8, fast): the base follows the link pointer: it lands on one of the four link slots, which one
// depending on the store order (index order 0,1,2,3 and 3,2,1,0 give +0x5c, 2,0,3,1 and 1,3,0,2 +0x5e, 0,2,1,3 +0x5a),
// all 18; with every entry field written through `entry` (one address) the base is retail's +0x4c but NONE is again
// kept in EBP from the global stores (the 49 form, re-measured 49). Tried: the link stores through the pointer in the
// field chain's order (next, next_other, unknown58, previous_other: retail's store order) 18; four separate link
// statements 18; the link pointer declared before the record 18; a view pointer local passed to 0x7f8b0 18; the flags
// and the word after them by one memset 31 (260 bytes); the flags stores before the links 27; a pointer walk with the
// link pointer 80 (275 bytes); every field through `g_4cf98c[index]` with the link pointer taken from it 44 (base
// +0x4c, NONE in EBP); a dword pointer to the flags with the field chain 49.
// Round 110 (m3 lane 16, fast with 0x7f8b0, 0x1537f0 and the caller 0x8d9f0): the link chain through `entry` with no link
// pointer re-measured 49. NONE stays out of EBP only while the NONE stores go through a pointer derived from `entry`, and
// the cursor base then lands on a link slot. A view pointer local for the 0x7f8b0 call with no link pointer 49; the link
// pointer taken from `&g_4cf98c[index].unknown58` with the rest through `entry` 45 (base +0x4c, NONE in EBP, the record's
// two zero words merged in EBX). Mixes of the `entry` local with the array element, not kept without a match: the 0x7f8b0
// view through `&g_4cf98c[index].player` 14, the flags and the word after them through `g_4cf98c[index]` 14 (same code),
// both 14: retail's byte-store order with only the base (+0x5c) left, so every remaining difference is a displacement;
// with the link chain in retail's store order 14 (base +0x5e); the links as four `entry->` statements in retail's store
// order with the tail through the array element 48, with the view through it 46 (NONE in EBP). Re-measures (same code):
// the view and the tail both through the array element 14; the link pointer declared at the top of the loop body on the
// tail-through-array form 14; on main's body the four link stores as separate statements in retail's order 18 (base +0x5e);
// the entry pointer as `g_4cf98c + index` 18. No further untried natural form was found.
// Round 237 (m3 lane 3, fast with 0x7f8b0 and 0x1537f0, both matched each time unless noted): the hoist is a weight contest
// for EBP between the -1 constant (four head stores and four link stores share one value) and the record's first forwarded
// word. Diagnostics: one more read of that word (`entry->flags = source->field40`) ends the hoist with retail's base +0x4c
// and the halves in EBP/EBX; one head store dropped still hoists (74); the record initializer's dead NONE stores zeroed
// still hoist (46, 0x1537f0 2-16). Links as two prev/next pair statements through `entry` 49, with the view through the
// array element 46, through the array element everywhere 44, view through `entry` 45 (all base +0x4c, NONE in EBP); the
// same pairs or four statements through the link pointer, a second pointer per pair, a four-short view pointer 14-18 (no
// hoist, base +0x5e or +0x5c: the base is the second non-zero-offset expression of the offset pointer); two links through
// a pointer and two through `entry` 47 and the flags through a dword pointer 47-49 (base +0xc, hoist). The link value
// from a short local 46, read back from the first link 46, from the record's own NONE shorts 46, from the list ends 60
// (reloaded in the loop); the heads as two chains 46, read back from each other 46, as dword 0xffffffff 46; the entry
// links written `0xffff` 46. Unsigned 16-bit link stores (word lvalues, any spelling of the constant) 5: no hoist, base
// +0x4c and every instruction retail's but `mov eax, 0xffff` for `or eax, -1` (two bytes longer); three signed links and
// one unsigned 6-9 (the unsigned one is an immediate store). A 64-bit read of the record's 0x40 field in 0x7f8b0 45-46
// (0x7f8b0 61-68, 0x80490 85-106); a four-byte aggregate for that field 46-49 (same code); the timestamp through its
// dword cast, a memset or a pointer 46. Diagnostic, the record initializer's NONE byte (the 0xff the view takes) zeroed
// 48 (still hoists, so that byte is not in the contest); the record member typed as the view's source under 4-byte
// packing, or as 36 dwords, 46-49 (same code). Left: what gives the forwarded word one more use, or the -1 constant
// fewer, in retail's loop.
// M3 lane 12 r121 (one-file LTCG harness with 0x1537f0's object, exact on the control 18): the links as four `entry->`
// statements in retail's store order 48 (NONE in EBP); the per-visit record at function scope 18 (same code); 0x1537f0
// called before the identity is cleared 60 (271 bytes); a `short` loop index 63 (275 bytes); the flags and the word after
// them in one chained assignment 18 (same code); the links as two chained pairs through `entry` with the flags chained 48.
// @retail 0x7f930
void function_7f930(void)
{
	g_4cf974 = 0;
	g_4cf978 = 0;
	g_4cf970 = 8;
	g_4cf980 = NONE;
	g_4cf97c = NONE;
	g_4cf984 = NONE;
	g_4cf988 = NONE;
	for (long index = 0; index < 350; index++)
	{
		s_player_configuration_cache_entry *entry = &g_4cf98c[index];
		s_cached_player_record record;
		memset(&record.identity, 0, sizeof(record.identity));
		function_1537f0((s_profile_record *)record.source_bytes);
		function_7f8b0((s_cached_player_view *)&entry->player, &record.identity,
			(const s_cached_player_source *)record.source_bytes, NULL);
		short *links = &entry->unknown58;
		links[0] = links[1] = links[2] = links[3] = NONE;
		entry->flags = 0;
		entry->field_64 = 0;
	}
}

#include "files.h"
#include "crc.h"
#include "async.h"

struct file_reference_data;
void async_create_file_blocking(file_reference_data const *s_type_acf665, dword access_flags, long disposition, dword file_flags, long category, s_file_handle *file);
bool function_1368f0(s_type_acf665 *file);

char const g_450c68[] = "z:\\player_configuration_cache.dat";
bool g_467260 = true;
s_file_handle g_467264 = { INVALID_HANDLE_VALUE };

/* loads the cache from its file; true when the file has the cache's size,
   its checksum and a consistent cache */
// @retail 0x7fa40
bool function_7fa40(void)
{
	bool local_0 = false;
	s_type_acf665 local_1;
	memset(&local_1, 0, sizeof(local_1));
	local_1.signature = FILE_REFERENCE_SIGNATURE;
	local_1.location = NONE;
	if ((byte)local_1.flags & 1)
		function_1373c0(local_1.path);
	function_137320(local_1.path, g_450c68);
	*(byte *)&local_1.flags |= 1;
	if (function_1368f0(&local_1))
	{
		dword local_2 = 0;
		if (file_ref_size(&local_1, &local_2) && local_2 == 0x8e4c)
		{
			dword local_3;
			if (function_136970(&local_1, 1, &local_3))
			{
				if (file_ref_read(&local_1, 0x8e4c, &g_4cf970, true))
				{
					dword local_4 = NONE;
					function_163ba0(&local_4, &g_4cf978, 0x8e44);
					if (local_4 == (dword)g_4cf974 && function_80660())
						local_0 = true;
				}
				function_136bb0(&local_1);
			}
		}
	}
	return local_0;
}

/* starts writing the cache to its file, opening the file the first time;
   false when the file can't be opened */
// @retail 0x7fb50
bool function_7fb50(void)
{
	bool local_0 = false;
	s_type_acf665 local_1;
	file_ref_create(&local_1, false, g_450c68);
	if (g_467264.handle == INVALID_HANDLE_VALUE)
	{
		async_create_file_blocking((file_reference_data const *)&local_1, 2, 4, 4, 2, &g_467264);
		g_467260 = true;
		if (g_467264.handle != INVALID_HANDLE_VALUE)
			function_1a1310(g_467264, 0, 2, 6, NULL, &g_467260);
	}
	if (g_467264.handle != INVALID_HANDLE_VALUE)
	{
		g_4cf974 = NONE;
		function_163ba0((dword *)&g_4cf974, &g_4cf978, 0x8e44);
		g_4cf970 = 8;
		function_120d50(&g_467260, false);
		function_1a1050(g_467264, &g_4cf970, 0x8e4c, 0, s_file_write_flags(0), 2, 6, NULL, &g_467260);
		local_0 = true;
	}
	return local_0;
}

// @retail 0x80bf0
void function_80bf0(long record_capacity, s_cache_property_record *records,
	long property_capacity, s_cache_property *properties, byte *buffers,
	long *record_count, long *property_count, long *buffer_count)
{
	long record_index = 0;
	long property_index = 0;
	long buffer_index = 0;
	for (long i = 0; i < g_4cf978; i++)
	{
		if (g_4cf98c[i].flags & 1)
		{
			if (record_capacity <= 0 || property_capacity < 4)
				break;
			s_player_configuration_cache_entry *entry = &g_4cf98c[i];
			long values[3];
			memcpy(values, (byte *)&entry->player + 0x4c, sizeof(values));
			memcpy(records[record_index].identity, &entry->player, sizeof(records[record_index].identity));
			records[record_index].type = 0x61;
			records[record_index].count = 4;
			s_cache_property *record_properties = &properties[property_index];
			records[record_index].properties = record_properties;
			record_index++;
			record_capacity--;
			memcpy(&buffers[0x40 * buffer_index], (byte *)&entry->player + 0xc, 0x40);
			*(short *)&record_properties->unknown00 = -3;
			properties[property_index].type = 4;
			properties[property_index].unknown08 = (long)&buffers[0x40 * buffer_index];
			property_index++;
			buffer_index++;
			property_capacity--;
			for (long j = 0; j < 3; j++)
			{
				*(short *)&properties[property_index].unknown00 = (short)(j + 2);
				properties[property_index].type = 1;
				properties[property_index].unknown08 = values[j];
				property_index++;
				property_capacity--;
			}
		}
	}
	if (record_count)
		*record_count = record_index;
	if (property_count)
		*property_count = property_index;
	if (buffer_count)
		*buffer_count = buffer_index;
}

#include <xtl.h>
#include <wchar.h>

// @retail 0x80aa0
void function_80aa0(s_cache_property_record *records, long count)
{
	long position;
	if (count <= 0) return;
 s_cache_property *const *local_0 = &records->properties;
 long local_2 = count;
 do
	{
		const s_cached_player_identity *local_1 = (const s_cached_player_identity *)((byte const *)local_0 - 0x14);
		if (*(long const *)((byte const *)local_0 - 8) == 0x61 && *(long const *)((byte const *)local_0 - 4) == 4)
		{
			long index = function_7fc80(local_1, &position);
			if (index != NONE)
			{
				s_cache_property *properties = *local_0;
				long values[3];
				long present = 0;
				long absent = 0;
				if (properties[0].type == 0)
					absent = 1;
				else if (properties[0].type == 4 && properties[0].unknown08 != 0)
					present = 1;
				for (long j = 0; j < 3; j++)
				{
					if (properties[j + 1].type == 0)
						absent++;
					else if (properties[j + 1].type == 1)
					{
						values[j] = properties[j + 1].unknown08;
						present++;
					}
				}
				if (absent + present == 4)
				{
					s_player_configuration_cache_entry *entry = &g_4cf98c[index];
					if (present == 4)
					{
						wchar_t *name = (wchar_t *)((byte *)&entry->player + 0xc);
						wcsncpy(name, (const wchar_t *)properties[0].unknown08, 31);
						name[31] = 0;
						memcpy((byte *)&entry->player + 0x4c, values, sizeof(values));
						entry->flags &= ~8;
					}
					unsigned __int64 time = 0;
					GetSystemTimeAsFileTime((FILETIME *)&time);
					*(dword *)entry->unknown64 = (dword)(time / 3600000000ULL);
					entry->flags &= ~2;
				}
			}
		}
  local_0 = (s_cache_property *const *)((byte const *)local_0 + 0x18);
	} while (--local_2);
}

bool g_51055c;

struct s_profile_record;
void function_1537f0(s_profile_record *record);
long function_7fe90(const s_cached_player_identity *identity, long position, const s_cached_player_source *source);

#pragma inline_depth(0)
static __forceinline void function_80491(long arg_0)
{
 function_7fe40(arg_0);
}
#pragma inline_depth(8)
// @retail 0x80490
void function_80490(const s_cached_player_identity *identity, const s_cached_player_source *source,
	bool local, bool recent)
{
	bool time_changed = false;
	long position = 0;
	long index = function_7fc80(identity, &position);
	bool changed;
	if (index == NONE)
	{
		index = function_7fe90(identity, position, source);
		changed = true;
	}
	else
	{
		s_player_configuration_cache_entry *entry = &g_4cf98c[index];
		s_cached_player_view player;
		function_7f8b0(&player, identity, source, (const s_cached_player_view *)&entry->player);
		changed = memcmp(&entry->player, &player, sizeof(entry->player)) != 0;
		if (changed)
			memcpy(&entry->player, &player, sizeof(entry->player));
		entry->flags &= ~10;
		unsigned __int64 time = 0;
		GetSystemTimeAsFileTime((FILETIME *)&time);
		dword hours = (dword)(time / 3600000000ULL);
		if (hours != *(dword *)entry->unknown64)
			time_changed = true;
		*(dword *)entry->unknown64 = hours;
	}
	s_player_configuration_cache_entry *entry = &g_4cf98c[index];
	if (local)
	{
		if (changed)
			entry->flags |= 1;
		entry->flags |= 4;
	}
	else
		entry->flags &= ~4;
	if (recent)
	{
		changed = changed || !(entry->flags & 16);
		entry->flags |= 16;
	}
	function_80491(index);
	if (changed || time_changed)
		g_51055c = true;
}

#pragma inline_depth(0)
// @retail 0x80330
long __stdcall function_80330(const s_cached_player_identity *identity, long position)
{
	s_cached_player_source source;
	function_1537f0((s_profile_record *)&source);
	((byte *)&source.field40)[2] = 0;
	((byte *)&source.field40)[3] = 0;
	source.field45 = 0;
	source.field46 = 0;
	((byte *)&source.field40)[0] = 10;
	((byte *)&source.field40)[1] = 10;
	long index = function_7fe90(identity, position, &source);
	if (index != NONE)
 {
  s_player_configuration_cache_entry *local_0 = g_4cf98c + index;
  local_0->flags |= 10;
 }
	return index;
}
#pragma inline_depth(8)

/* inserts a player at the given position of the recent-players cache, evicting the oldest when it is full */
// @retail 0x7fe90
long function_7fe90(const s_cached_player_identity *identity, long position, const s_cached_player_source *source)
{
	long removed = NONE;
	if (g_4cf978 == 350)
	{
		removed = g_4cf980;
		function_7fd10(g_4cf980);
		function_7fd80(removed);
	}
	long lower = 0;
	long upper = 0;
	bool downward = false;
	if (removed != NONE)
	{
		lower = removed > position ? position : removed;
		upper = removed > position ? removed : position;
		if (removed > position)
			downward = false;
		else if (removed < position)
		{
			downward = true;
			upper--;
			position--;
		}
	}
	else if (position < g_4cf978)
	{
		lower = position;
		upper = g_4cf978;
		downward = false;
	}
	if (upper > lower)
	{
		if (downward)
		{
			for (long i = lower + 1; i <= upper; i++)
				g_4cf98c[i - 1] = g_4cf98c[i];
		}
		else
		{
			for (long i = upper - 1; i >= lower; i--)
				g_4cf98c[i + 1] = g_4cf98c[i];
		}
		long change = downward ? -1 : 1;
		for (long i = 0; i < 350; i++)
		{
			s_player_configuration_cache_entry *entry = &g_4cf98c[i];
			if (entry->previous_other >= lower && entry->previous_other <= upper) entry->previous_other += change;
			if (entry->next_other >= lower && entry->next_other <= upper) entry->next_other += change;
			if (entry->unknown58 >= lower && entry->unknown58 <= upper) entry->unknown58 += change;
			if (entry->next >= lower && entry->next <= upper) entry->next += change;
		}
		if (g_4cf97c >= lower && g_4cf97c <= upper) g_4cf97c += change;
		if (g_4cf980 >= lower && g_4cf980 <= upper) g_4cf980 += change;
		if (g_4cf984 >= lower && g_4cf984 <= upper) g_4cf984 += change;
		if (g_4cf988 >= lower && g_4cf988 <= upper) g_4cf988 += change;
	}
	s_player_configuration_cache_entry *entry = &g_4cf98c[position];
	entry->next_other = (short)g_4cf97c;
	entry->previous_other = NONE;
	entry->next = NONE;
	entry->unknown58 = (short)g_4cf988;
	entry->flags = 0;
	unsigned __int64 now = 0;
	GetSystemTimeAsFileTime((FILETIME *)&now);
	*(dword *)entry->unknown64 = (dword)(now / 3600000000ui64);
	memset(&entry->player, 0, sizeof(entry->player));
	s_cached_player_view *player = (s_cached_player_view *)&entry->player;
	player->identity = *identity;
	memcpy(player->name, source->name, sizeof(player->name));
	player->field4c = source->field40;
	player->field50 = source->field44;
	player->field51 = source->field45;
	player->field52 = source->field46;
	player->field53 = source->field47;
	player->field56 = source->field7f;
	if (g_4cf97c != NONE && g_4cf97c != position)
		g_4cf98c[g_4cf97c].previous_other = (short)position;
	g_4cf97c = position;
	if (g_4cf980 == NONE) g_4cf980 = position;
	if (g_4cf988 != NONE && g_4cf988 != position)
		g_4cf98c[g_4cf988].next = (short)position;
	g_4cf988 = position;
	if (g_4cf984 == NONE) g_4cf984 = position;
	g_51055c = true;
	if (removed == NONE) g_4cf978++;
	return position;
}

struct s_network_session_player;

// @retail 0x805d0
void __fastcall function_805d0(s_network_session_player *player)
{
 function_80490((const s_cached_player_identity *)player,
  (const s_cached_player_source *)((const byte *)player + 0xa8), false, true);
}

#pragma inline_depth(0)
static __forceinline void function_80441(long arg_0)
{
 function_7fdf0(arg_0);
}
#pragma inline_depth(8)
// @retail 0x80440
void function_80440(const s_cached_player_identity *identity, s_recent_player *player)
{
 long position = 0;
 long index = function_7fc80(identity, &position);
 if (index == NONE)
  index = function_80330(identity, position);
 memcpy(player, &g_4cf98c[index].player, sizeof(*player));
 function_80441(index);
}

bool g_510554;
dword g_510558;
bool function_7fb50(void);

// @retail 0x80390
void function_80390(void)
{
 if (g_51055c && g_510554)
 {
  dword local_0 = GetTickCount();
  if (local_0 - g_510558 > 10000)
  {
   if (function_7fb50())
    g_51055c = false;
   g_510558 = local_0;
  }
 }
 unsigned __int64 local_1 = 0;
 GetSystemTimeAsFileTime((FILETIME *)&local_1);
 dword local_2 = (dword)(local_1 / 3600000000ULL);
 long local_3 = 0;
 s_player_configuration_cache_entry *local_4 = g_4cf98c;
 if (g_4cf978 > 0)
 {
  do
  {
   if (local_2 - local_4->field_64 > 72)
    local_4->flags |= 2;
   local_3++;
   local_4++;
  } while (local_3 < g_4cf978);
 }
}

s_4cf790 g_4cf790_storage;
s_4cf970 g_4cf970_storage;
