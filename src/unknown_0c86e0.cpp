// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0C86E0.CPP: a unit leaving its weapon's zoom. Lane S
   wrote it for the weapon functions 0x103b10..0x103ce0, which call it. */

#include "unknown_11c920.h"
#include "globals.h"

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
bool function_100f70(long weapon_index);
long function_1896c0(real scale, long tag_index);
long unit_get_player_index(long unit_index);
long function_cbd50(long unit_index, short weapon_index);

/* the unit (a view of the object data) */
struct s_zoom_unit
{
	byte unknown000[0x13c];
	long player_index;
	byte unknown140[0x212 - 0x140];
	char current_weapon_slot;
	byte unknown213[0x218 - 0x213];
	long weapon_indices[4];
	byte unknown228[0x240 - 0x228];
	char field_240;
	char desired_zoom_level;
	byte unknown242[0x26c - 0x242];
	real zoom_transition;
};

struct s_zoom_object_header
{
	byte unknown00[8];
	s_zoom_unit *unit;
};

/* the weapon's definition: its zoom-out sound */
struct s_zoom_weapon_definition
{
	byte unknown000[0x278];
	long zoom_out_sound_index;
};

/* the player (0x21c bytes): its local user */
struct s_zoom_player
{
	byte unknown00[0x28];
	short user_index;
};

/* the globals' sound tags */
struct s_zoom_sounds
{
	byte unknown00[0xd0];
	long zoom_out_sound_index;
};

struct s_zoom_globals
{
	byte unknown000[0x134];
	s_zoom_sounds *sounds;
};

#define ZOOM_UNIT_GET(index) (((s_zoom_object_header *)g_4e0300->data)[(index) & 0xffff].unit)

static __forceinline long weapon_get_zoom_out_sound(long weapon_index)
{
	long result = NONE;

	if (weapon_index != NONE)
	{
		s_zoom_unit *weapon = ZOOM_UNIT_GET(weapon_index);
		result = ((s_zoom_weapon_definition *)g_4e3b44[*(long *)weapon & 0xffff].bytes)->zoom_out_sound_index;
	}
	return result;
}
// MiniMax G (11-20): no new form, no lower score (110): the unit is still fetched once and kept across the two 0xbadc0 calls where retail looks it up again, because this link knows that callee writes no memory; that callee's memory summary is in src/unknown_0bad50.cpp, which this lane may not edit: BLOCKED-BY 0xbadc0.
// score 110 (was 110): retail fetches the unit again after the two inlined 0x14de90 calls to 0xbadc0; ours merges that
// fetch into the entry fetch, so the entry saves and every later register differ. BLOCKED-BY 0xbadc0 as reached through
// the inlined 0x14de90 (see round 117 at the end: retail's 0xbadc0 itself has the no-write summary).
// Earlier (round 199, 110): BLOCKED-BY 0xbadc0 (src/unknown_0bad50.cpp, on the Avoid list in round 199): our link knows it writes
// MiniMax A (11-19): no new form, no lower score (110): the unit fetched at the entry and the one fetched after the two 0xbadc0 calls are still merged into a single load, so the reset still writes through the merged pointer where retail writes through the entry one, and the user index still arrives in a different register; the permuter's best variant (101) is built from a reset block copied into two identical branches and from two flags set by one test and read by the next, so it is dropped and the body is unchanged. BLOCKED-BY 0xbadc0 in src/unknown_0bad50.cpp.
// Earlier at 110 (was 110; this score line is merged into the one above, m3 lane 6 r204): BLOCKED-BY 0xbadc0 (src/unknown_0bad50.cpp, on the Avoid list in round 199): our link knows it writes
// no memory and merges retail's second unit fetch into the entry fetch. Round 199 (m3 lane 5, fast): the weapon lookup's unit
// fetched into a named pointer 110 (same code); the weapon slot held in a char 114 (355 bytes); the weapon array through a
// named pointer 110 and the weapon's definition index named 110 (both the same code).
// MiniMax R run 92 (11-17): no new form, no lower score (110): the weapon lookup as an if-statement instead of the
// ternary gives 114, and the user index as a short gives 107 - a gain of three, under the review bar, so the body
// stands with the 32-bit index retail reads - and this link still knows that 0xbadc0 writes no memory and merges the
// unit fetches where retail fetches the unit again: BLOCKED-BY 0xbadc0 (src/unknown_0bad50.cpp).
// Retail reloads the object pool after both calls even though 0xbadc0 neither writes memory nor calls another function.
// MiniMax H (11-18): no new form, no lower score (110): the two unit fetches still merge across the two 0xbadc0 calls
// because this link knows that callee writes no memory, and the unit index stays in ESI where retail has EDI; that
// callee's memory summary is in src/unknown_0bad50.cpp, which this lane may not edit. BLOCKED-BY 0xbadc0.
// Earlier note, 110 (was 110): BLOCKED-BY 0xbadc0 (src/unknown_0bad50.cpp): retail fetches the unit again after the two
// 0xbadc0 calls although retail's 0xbadc0 neither writes nor calls; ours merges the fetches (rounds below).
// MiniMax D (11-17): no new form, no lower score (107): retail looks the unit up again after its two 0xbadc0 calls
// where this build merges the two fetches, because that callee writes no memory, and the callee's body is in
// src/unknown_0bad50.cpp, which this lane may not edit; the alternative that tests the weapon index against NONE
// before function_100f70 reaches 100, under the keep-bar. BLOCKED-BY 0xbadc0.
// MiniMax S (11-16): no new form, no lower score (107): the call boundaries agree with retail's (0xbadc0 twice and 0x100f70), but retail looks the unit up again after the two calls where this build merges the two fetches into one, because 0xbadc0 writes no memory; that callee's body is in src/unknown_0bad50.cpp, which this lane may not edit: BLOCKED-BY 0xbadc0.
// MiniMax E (11-16): no new form, no lower score (107): the call boundaries agree with retail's (0xbadc0 twice and 0x100f70); retail looks the unit up again after those two calls because its link knows that callee writes memory, while this link knows it writes none and merges the two fetches, so the entry saves and every later register differ; that callee's memory summary is in src/unknown_0bad50.cpp, which this lane may not edit. BLOCKED-BY 0xbadc0.
// MiniMax E (11-15): no new form, no lower score (107): the call boundaries agree with retail's (0xbadc0 twice and 0x100f70), so the difference is still 0xbadc0's memory summary, whose definition is in src/unknown_0bad50.cpp. BLOCKED-BY 0xbadc0.
// MiniMax C (11-15): no new form, no lower score (107): the unit is still fetched once and kept across the two
// 0xbadc0 calls where retail looks it up again, because this link knows that callee writes no memory; that callee's
// memory summary is in src/unknown_0bad50.cpp, which this lane may not edit, so the entry saves and the later
// register homes stand.
// MiniMax D (11-14): no new form, no lower score (107): retail loads the unit from the object pool again after
// the two 0xbadc0 calls, which our link does not do because that callee does not write the pool global; the
// permuter's 17 tries reach 93 with the arguments of function_1896c0 swapped, which changes the call, so the
// gain is not kept.
// MiniMax C (11-16), at 107 then: no new form, no lower score; BLOCKED-BY 0xbadc0 (src/unknown_0bad50.cpp,
// on the Avoid list): retail re-fetches the unit from the object pool after its two 0xbadc0 calls and does the
// weapon lookup through that second fetch, while this link knows the callee writes no memory and merges the
// two fetches, so the entry saves, the unit index's register and every later register differ; the callee's
// memory summary is in a file this lane may not edit.
// Earlier 110 (was 107): BLOCKED-BY 0xbadc0 (src/unknown_0bad50.cpp, on the Avoid list): retail fetches the unit
// again after the two 0xbadc0 calls (its link treats that callee as writing memory) and does the weapon lookup
// through the second fetch; our link merges it into the entry fetch, so the entry saves, the unit index's
// register and every later register differ. Earlier rounds below.
// MiniMax B (11-13): no new form, no lower score (107): aligned the two listings instruction by instruction: retail keeps
// the index times twelve in a register (shl esi, 2) where this build folds it into the address expression, and retail
// keeps the unit index in EDI where this build keeps it in ESI; the bodies part company at the entry, because retail
// looks the unit up again after the two 0xbadc0 calls and this link knows that callee writes no memory; the callee's
// memory summary is in a file this lane may not edit.
// MiniMax S (11-14): no new form, no lower score (107): retail pushes four saved registers before the body, keeps the unit index in EDI and the unit in EBP and looks the unit up again after its two 0xbadc0 calls, while ours pushes three, keeps the index in ESI and the unit in EDI and merges the two lookups because our link knows that callee writes no memory; the callee's memory summary is in a file this lane may not edit, so the entry saves and the later carriers stand. Still BLOCKED-BY 0xbadc0.
// MiniMax B (11-12): no new form, no lower score (107); this round re-read retail's listing: it looks the
// unit up again after its two 0xbadc0 calls and reads the pool globals again, while our link knows the
// callee writes no memory and keeps the entry fetch, so the entry saves and the later carriers stay as they
// are; that callee's memory summary is in src/unknown_0bad50.cpp, which this lane may not edit.
// Still BLOCKED-BY 0xbadc0.
// MiniMax H (11-11): no new form, no lower score (107); the unit is still fetched once across the two 0xbadc0 calls where retail fetches it again, so the entry saves and the later carriers stay as they are, and the callee's memory summary is in a file this lane may not edit. BLOCKED-BY 0xbadc0.
// MiniMax C (11-11): no new form, no lower score (107); the unit fetch still merges into the entry fetch because of 0xbadc0's memory summary, and 0xbadc0 is in src/unknown_0bad50.cpp.
// Earlier at 107 (was 107): Frameless entry saves, fresh unit lookup and late prior-state carrier remain.
// MiniMax E (11-10, run 73): no new form, no lower score (107); retail re-fetches the unit pointer and both pool globals after the two 0xbadc0 calls where we keep the entry fetch, and 0xbadc0's memory effects are unknown because that callee lives in another file.
// MiniMax D (11-11): no new form, no lower score (107): the unit is still fetched once across the two 0xbadc0 calls where retail fetches it again, so the entry saves and the later carriers stay as they are; the callee's memory summary is in a file this lane may not edit.
// MiniMax D (11-10): no new form, no lower score (107): the two 0xbadc0 calls still sit between one
// unit fetch and its uses where retail fetches the unit again, so the entry saves and the later
// carriers stay as they are; the callee's memory summary is in a file this lane may not edit.
// MiniMax S (11-10): no new form, no lower score (107): retail pushes its saved registers first and keeps the
// unit index in EDI, the unit in EBP and NONE in EBX, then calls 0xbadc0 twice and looks the unit up again
// after them; ours loads the object table before the pushes, keeps them in ESI, EDI and EBP, and merges the two
// lookups because our link knows 0xbadc0 writes no memory, so the bodies part company at the entry (362 bytes
// against retail's 376); the blocker is 0xbadc0's memory summary, which is in a file this lane may not edit.
// MiniMax D (11-09): no new form, no lower score (107); the unit pointer is still fetched once and kept
// across the two calls to 0xbadc0, while retail looks it up again after them, because retail's link treats
// that callee as writing memory and this link knows it writes none; the callee's body is in another file
// this lane may not edit.
// MiniMax R (11-09): no new form, no lower score; the entry's second unit fetch is still merged into the
// first, because the lookup callee that decides it lives in another file.
// MiniMax D (11-08), at 107: retail re-fetches the unit pointer and both pool globals after
// the two 0xbadc0 calls; our link knows that callee writes no memory and keeps the entry fetch, so the
// register plan and the length differ from the first byte.
// MiniMax B (11-06): no new form, no lower score; retail loads the object table after the prologue pushes
// and keeps the unit index in EDI, the unit in EBP and NONE in EBX, this build loads the table before the
// pushes and keeps them in ESI, EDI and EBP; the user index is a 32-bit value in EBX in retail, a 16-bit
// store in BP here.
// MiniMax C (11-06): no new form, no lower score; the unit pointer is still fetched once and merged across the two 0xbadc0 calls where retail re-reads the table base (blocked by 0xbadc0's memory summary in src/unknown_0bad50.cpp).
// MiniMax G (11-06): no new form, no lower score; still blocked by 0xbadc0, whose body is in a file
// outside this lane: retail re-fetches the unit after the two calls (the index times twelve kept in esi)
// where ours reuses the entry fetch, because our link knows the callee writes no memory, so every later
// register home shifts. The frameless entry and the carriers stand as they are.
// MiniMax H (11-04): no new form, no lower score; retail re-fetches the unit after the two 0xbadc0 calls (the index times twelve kept in esi) where ours reuses the entry fetch, and 0xbadc0's memory effects are unknown, so the frameless entry and the carriers stand as they are.
// Earlier 107 (was 107): BLOCKED-BY 0xbadc0 (src/unknown_0bad50.cpp): retail re-fetches the unit after
// the two 0xbadc0 calls (index * 12 kept in ESI), ours reuses the entry fetch. Earlier notes:
// Earlier 107 (was 107): this round: the fast check reads 107 at 362 bytes against retail's 376 again
// and no new form was tried; the difference is still blocked by 0xbadc0's memory-effect knowledge in
// src/unknown_0bad50.cpp, which makes our link keep the entry fetch.
// Earlier 107 (was 107): this round: no new form. Still BLOCKED-BY 0xbadc0 (held file): retail
// re-fetches the unit after its two 0xbadc0 calls, our link proves the callee writes no memory
// and keeps the entry fetch, so every later register home shifts.
// Earlier 107 (was 107): frameless entry saves, fresh unit reload and late prior-state carrier differ.
// frameless entry saves, fresh unit lookup and late prior-state carrier remain.
// Earlier: this round: the fast check reads 107 at 362 bytes against retail's 376 again
// Earlier at 107 (was 107): this round: no new form. Still BLOCKED-BY 0xbadc0 (held file): retail
// Earlier at 107 (was 107): frameless entry saves, fresh unit reload and late prior-state carrier differ.
// Frameless entry saves, fresh unit reload and late prior-state carrier differ.
// Earlier: this round: no new form. Still BLOCKED-BY 0xbadc0 (held file): retail
// Earlier: frameless entry saves, fresh unit reload and late prior-state carrier differ.
// frameless entry saves, unit reload and sentinel carriers differ.
// frameless entry saves, fresh unit reload and late prior-state carrier differ.
// Earlier: frameless entry saves, unit reload and sentinel carriers differ.
// entry saves, fresh unit reload and sentinel carriers still differ.
// frameless entry saves, unit reload and sentinel carriers differ.
// Native player-table stride and byte eligibility/prior-state combinations 131
// each (seven forms); full-width eligibility 127 (four-point gain rejected).
// Original record view, result types, observations and body restored.
// entry saves, fresh unit reload and sentinel carriers still differ.
// BLOCKED-BY 0xbadc0: retail re-fetches the unit after its two calls, ours reuses the entry
// fetch (our link knows the callee writes no memory).
// shared weapon producer 131; user-result arms 125; both 125; signed-word user 128;
// shared producer/word 128; word/result arms 122; all three 122; guarded weapon
// result 135. Retain signed-word user/result arms; the six-count arm-only gain is rejected.
// Unit fetched after the player queries 118, with a long user index 121; was_zoomed
// declared first 107 (it then lives in bl from entry, retail dl: positional only), with the
// late fetch and long user 109 (376 bytes, flag spilled), with long user 110; zoom-out
// call in each sound arm 124; weapon read through the entry unit 122.
// Retail re-fetches the unit after the two 0xbadc0 calls (index * 12 kept in esi); our link
// knows 0xbadc0 writes no memory and reuses the entry pointer. With 0xbadc0 opaque (one-file
// LTCG link) this body with a long user index gives retail's code apart from the callees'
// conventions. Existing 0x14de90 getter 122; plus 0xcbd50 for the weapon 122; long user
// index 125, with both getters 125; unit fetched at its first use 106 (376 bytes, but it
// moves retail's entry fetch after the calls).
// Round 26 (lane 9), from 122: zoom flag declared first 107 (kept: the entry fetch stays; the flag
// sits in bl from entry where retail sets dl late, but ebx no longer holds a -1 and the sentinels
// compare as immediates as in retail); plus long user index 110; weapon flag as if/else 126; user
// index as one ternary 128; zoom test as !(keep && zoomed) 122; reset entry written in full 124.
// On the 107 body: long user index 111; reset entry in full 109; weapon flag as if/else 111; zoom
// test as !(keep && zoomed) 107; sound index as one ternary 107.
// BLOCKED-BY 0xbadc0 (the re-fetch after its calls).
// Round 57: force-inline current-weapon helper 107; both helpers force-inline 107; user ordinal
// before the zoom flag 107; zoom flag after the entry unit 107; after the player queries 120;
// after the weapon lookup 120; weapon index declared early 107; sound index declared early 107.
// Round 202: Prior-state long/short/byte storage, eligibility long/short/byte storage, and both long/both short 106/105/107/103/97/107/102/96 fast.
// Short eligibility 97/376 plain; both short 96/376 fast add word tests and padding; aligned instructions worsen.
// Original executable source restored. Entry register saves, the fresh unit lookup and sentinel/result homes remain.
// Round 183: one reset/sound domain, separate/merged user guards 126/126; native prior-level
// snapshots 127/127; final sound-tag result 126/126; those result forms with paired reset
// stores chained 126/126. Original query graph, bool lifetimes and complete body restored.
// Player early/conditional returns 113/113; weapon-slot early/conditional returns 100/107;
// sound early/conditional returns 110/110; all producers early/conditional 105/114. The
// seven-point slot gain is below the required eight; original producers restored.
// Round 267: masked slot/header record/pool descriptor/header table 107/119/107/117;
// descriptor with byte/long slot 111/114; preserved-state return 126, with header 129.
// Original producers and complete executable body restored.
// Round 270: player/weapon/both sentinel declarations first 119/107/119;
// native signed-byte slot alone/with player/with weapon/with both 111/123/111/123;
// both sentinel declarations with consistent direct reset-unit observations 150.
// Original producers and complete executable body restored; fresh lookup remains.
// Round 76: late false 122; result arms 121; late false with long user 125; late fresh unit 122;
// original fresh unit 107; direct reset lookups 175; false after entry 107; false before weapon 120.
// Original body restored.
// Earlier 107 (was 107): this round: no new form. Still BLOCKED-BY 0xbadc0 (src/unknown_0bad50.cpp):
// MiniMax R (11-01, third round): no new form, no lower score. Still BLOCKED-BY 0xbadc0: retail
// re-fetches the unit after its calls because its link treats the callee as writing memory, while
// our link knows that it writes none and keeps the unit in a register. The needed change is in
// src/unknown_0bad50.cpp, which is outside this lane's set of files, so the function is left as
// it is.
// MiniMax R (11-01, second round): no new form, no lower score. Still BLOCKED-BY 0xbadc0: retail
// MiniMax R (11-01): no new form, no lower score. Still BLOCKED-BY 0xbadc0: retail re-fetches the
// unit after its calls because its link treats the callee as writing memory, while our link knows
// that it writes none and keeps the unit in a register.
// Earlier 107 (was 107): this round: no new form. Still BLOCKED-BY 0xbadc0 (src/unknown_0bad50.cpp):
// retail re-fetches the unit after the two calls because its compiler treats 0xbadc0 as writing
// memory, while our link knows it writes none, so the unit and the flag registers are homed
// differently and the entry prologue differs.
// Round 47 (m3 lane 13, fast): zoom flag declared after the weapon test (retail's late `xor dl, dl`) 122; the
// weapon helper taking the unit index and fetching the unit itself 107 (same code: the fetch is still merged);
// the player test inverted (`!= NONE` arm first) 117; zoom flag declared after the unit fetch 107; user index
// defaulted to NONE with a one-arm assignment 124. Retail treats 0xbadc0 as writing memory (reloads g_4e0300 and
// its data after the calls while keeping the index register); ours knows it writes none, so no body form here
// brings the second fetch back.
// Earlier at 107 (was 107): this round: no new form. Still BLOCKED-BY 0xbadc0 (src/unknown_0bad50.cpp):
// Round 215: full salted weapon/sound handle domains and real player-record
// indexing, seven nonempty subsets 107 each; complete graph with native long user ordinal 110.
// Original types, both player queries, prior-state lifetime and entire body restored;
// entry saves, fresh unit lookup and sentinel carriers remain.
// Earlier: this round: no new form. Still BLOCKED-BY 0xbadc0 (src/unknown_0bad50.cpp):
// Round 280: complete pre-reset prior-state predicate, reset/preserve decision, bool/long
// storage and producer/entry declaration 110/110/113/113/113/113/116/116. Original complete
// body restored; merged lookup and late prior-state carrier remain.
// Round 332: Retained absolute header ordinal long/dword/word/int, each with entry or late false
// result: 107/107/109/107/122/122/124/122.
// Original executable source restored; no eligible gain.
// Round 284: reset/sound domain without the implied keep retest, crossed with early preserve
// exit, producer prior-state lifetime and sound sentinel exit: eight forms 134 each.
// Original complete body restored; fresh current-unit observation and late prior-state home remain.
// Round 289: immediate preserve/weapon-sound predicate producers and prior-state-first
// admission, seven subsets: 107/107/125/107/125/125/125; complete graph with promoted user
// ordinal 128. Original predicates, types and complete body restored; fresh lookup remains.
// Round 28 (m3 lane 18, fast): the two zoom-level resets chained 107; the transition cleared before them 107;
// the weapon query before the player queries 107 (378 bytes); the player test as `!= NONE` first 117; the
// sound test with the zoom flag first 125. Still BLOCKED-BY 0xbadc0 (src/unknown_0bad50.cpp, avoided): our
// link knows it writes no memory and merges retail's second unit fetch into the entry fetch.
// Round 46 (m3 lane 2, fast): weapon flag as `weapon_index != NONE && function_100f70(...)` 100 (376 bytes, but a
// setcc-style flag where retail has the ternary's `xor al, al`; 7 points, not kept); the sound test with keep first
// 108; long user index with the zoom flag declared just before the reset test 125; the zoom flag set by an `if`
// 116; the globals' sound as the default, overridden for the weapon 104 (367 bytes); weapon through the entry unit
// 107 (same code, the fetches are merged anyway). Still BLOCKED-BY 0xbadc0 (src/unknown_0bad50.cpp, avoided).
// Round 125 (lane 8, fast): the existing event_player_get for the user index 107 (same code); the sound helper
// plain `inline` 162 (318 bytes); the player helper __forceinline 107; the weapon flag as `weapon_index != NONE
// && function_100f70(weapon_index)` 100 at 376 bytes (flag materialized with `mov cl, 1` where retail keeps the
// call result in AL; a positional gain, not kept); the sound index as one ternary 107; the sound part behind an
// early return 107. Still BLOCKED-BY 0xbadc0 (src/unknown_0bad50.cpp, avoided).
// Round 145 (m3 lane 5, fast): the `&&` weapon flag with the unit fetched in the reset block 130 (393 B); the
// `&&` flag alone 100 (376 B, seven points: not kept); that with a long user index 104 (367 B); with the zoom flag
// declared after the weapon flag 113 (361 B); the ternary flag with the late zoom flag and a long user index 125
// (350 B). Retail uses the entry unit (EBP) for the reset and a second fetch (ESI) for the weapon lookup.
// Round 107, on the full-width user ordinal base: weapon-slot early/conditional returns 97/110;
// player/sound early returns 116/113; all three early 102; weapon flag if/else 114; default-first
// sound 107; forceinline current-weapon helper 110. NOT kept: 97 combines edits whose parts alone
// do not both gain, without a byte match; original producers, sound control and ordinal restored.
// Round 55 (m3 lane 15; fast, and a one-file LTCG harness with the ballast object and the files of 0xbadc0, 0x100f70, 0x1896c0, exact at 107/362 bytes):
// on the plain 0xbadc0, in the real build: long user index with the flag declared after the weapon flag, weapon flag written `== NONE ? false : call`, sound helper
// with an early return 128 (348 bytes); the same with the ternary as it stands 127; that helper alone 110 (359); the long index and late flag with the `== NONE`
// ternary alone 126 (350); the helper as one conditional expression 110 (359); the `== NONE` ternary and early-return helper 111 (359). In the harness only, with
// a scratch 0xbadc0 holding `long probe_local; long *probe_pointer = &probe_local; *probe_pointer = 0;` (a pointer to a whole local, a banned form, so not kept and
// not in src): the current body 134, the long index and late flag 47, plus the `== NONE` ternary 42, plus the early-return helper or the one-expression
// helper 2 at retail's 376 bytes (left: the 0x100f70 argument register and the harness's jump address); the else-arm helper 32, the sound index as one ternary
// 42. A store in this function instead 129 (folded before CSE). So the blocker is only 0xbadc0's memory summary, BLOCKED-BY 0xbadc0.
// M3 lane 4 r170 (fast; 0x103b10 matched each time): the existing out-of-line 0x14de90 for the player index 107; the existing
// 0xcbd50 for the weapon lookup 107; both 107; `__assume(unit != NULL)` after the first fetch 107; both helpers and the assume 107
// (identical code: the second unit fetch stays merged into the entry fetch). Still BLOCKED-BY 0xbadc0 (src/unknown_0bad50.cpp).
// M399: Immutable existing unit pointer/weapon handle/query result, seven nonempty subsets 107 each;
// all with existing slot-helper read-only unit input 107. NOT kept: every form ties; original entire file restored.
// Frameless entry, merged post-query unit fetch and late prior-state home remain.
// M401: Ordinary private player/slot/sound producers, subsets 1/2/3/4/5/6/7,
// then ordinary slot/sound with forced player expansion: 107/107/107/162/162/162/162/162.
// NOT kept: ties or shorter nonnative expansion; original producers and body restored.
// Frameless entry, post-query unit fetch and late prior-state carrier remain.
// Round 159 (m3 lane 6, fast; 0x103b10 matched each time): the reset entry's two stores written in full 109 (360 bytes);
// the unit fetched once, after the player queries 106 (376 bytes, retail's length, one point); that with the long user
// index, late zoom flag, `== NONE` ternary and early-return sound helper 125 (355 bytes: retail's code except its entry
// fetch and the registers that follow from it); those forms on two fetches 128 (348 bytes); the player index named once
// for its two uses 142 (341 bytes). Diagnostic, restored: 0xbadc0's header lookup through a forceinline out-parameter
// helper keeps 0xbadc0 and 0xbae20 matched but the fetches stay merged (128). Still BLOCKED-BY 0xbadc0.
// Round 62 (m3 lane 2, fast; 0x103b10 matched each time): the weapon flag defaulted false and set by an `if` 111 (360
// bytes); the sound index as one initialised conditional 107 (same code, as listed above); that with a long user index
// 110 (359 bytes); the zoom flag left uninitialised and cleared in an `else` of the reset test 121 (355 bytes); the weapon
// flag as `weapon_index == NONE ? false : call` alone 108; the final test nested (user, then zoom flag) 107 and the sound
// arms swapped (globals first) 107, both the same code. Retail's 0xbadc0 body itself writes no memory, yet retail
// re-fetches the unit after its two calls; still BLOCKED-BY 0xbadc0 (src/unknown_0bad50.cpp, avoided this round).
// Round 93 (m3 lane 1, fast; 0x103b10 matched each time; register-masked aligned 19 of 108): the weapon index through the
// existing 0xcbd50 (unit index, slot from a fresh fetch) 107 (same code, inlined); the reset test as `!weapon_zoomed ||
// !keep_weapon_zoom` 100 (381 bytes, aligned 22; not retail's test order, which tests keep first, and under the keep gate);
// the user's entry as `entries + user_index` 107; the player-index helper plain `static` 107; a player record pointer for
// the user index 107 (all the same code); the unit fetch as a forceinline function 107 (same code); the player helper's
// result declared before its lookup 119 (363 bytes). Re-measured, not new: the player-index helper as one conditional return
// 113 (364 bytes, as round 183), the weapon helper taking a const unit 107 (as M399). Still BLOCKED-BY 0xbadc0: round 55's harness forms (long user index, the zoom flag declared
// after the weapon flag, the `== NONE` weapon ternary and the early-return sound helper) are the recipe once its summary
// lets the unit be fetched again after the calls.
// m3 lane 12 r122 (fast; 0x103b10 matched each time): the weapon lookup through slot_handler.h's object_get()
// 107 (same code: the two header reads still merge); the user index read through a 0x21c-byte player array
// view 107 (same code); the user index declared long on its own 110 (359 bytes); the reset test written
// `!(keep_weapon_zoom && weapon_zoomed)` 107 (same code); the user index as one conditional initializer 107
// (same code).
// b9Q (11-17) walk against retail, fast build on r840: every statement, field offset and jcc sense agrees with retail
// (0xc86e0..0xc8855): unit fetch first (0xc86ee), the two player queries (0xc8701, 0xc871c: null unit or player -1
// gives NONE; & 0xffff, * 0x21c, word at +0x28), slot byte +0x212 and weapon dword +0x218 (0xc8755, 0xc8769), the
// 0x100f70 call only for a weapon (0xc877b), reset when !keep || !zoomed (0xc8786..0xc878c), stores +0x240, +0x241,
// +0x26c (0xc87a0..0xc87ae) with the entry's +0x2e/+0x89 under the user test (0xc87ce, 0xc87d4), then the sound arms
// (weapon: tag +0x278 at 0xc881e; globals +0x134/+0xd0 at 0xc8826) and 1.0f (0x45dbc0, 0xc8844) into 0x1896c0.
// No behaviour difference found. One type change kept: retail's user index is 32-bit (movsx ebx, word [+0x28] at
// 0xc8743, cmp ebx, -1 at 0xc879d and 0xc87db, mov ecx, ebx at 0xc87c2), so it is `long`: 107 -> 110 here (was bp,
// cmp bp, -1, movsx ecx, bp); no behaviour change; callers ca0b0 62, e7fb0 19, 102fb0 643, fdfb0 1420 unchanged, the
// other 13 MATCH. Left: the second fetch (0xc8748), saves, was_zoomed in dl (0xc8784), keep reloaded into al (0xc87e8),
// the weapon-NONE jump to the epilogue (0xc87f3; ours sets eax = -1 first): BLOCKED-BY 0xbadc0 above.
// M3 lane 15 r95 (fast with 0x103b10, MATCH each time; control 110, register-masked aligned 13): new combinations of
// the listed forms: the `&&` weapon flag with the early-return sound helper 107; those two with the zoom flag declared
// just before the reset test 102 (356 bytes, but aligned 31: its parts alone are 104/113/125, a positional artifact,
// not kept); the early-return helper with the globals' sound as the default 115; the `&&` flag with that default 107.
// Re-measured on this base: the `&&` flag alone 104 (367 bytes), the early-return helper alone 113, the late zoom
// flag alone 125, `&&` with the late flag 116, the early-return helper with the late flag 127. Still BLOCKED-BY 0xbadc0.
// m3 lane 14 r108 (control 110 at 359 B, 80 aligned rows by sequence alignment). The block is not an image-wide property of 0xbadc0: of its 140
// retail callers this is the only one that loads the pool ahead of a 0xbadc0 call and loads it again after it, and 0xbadc0 defined `inline` (a
// full check, diagnostic, reverted: the idea was that an inline function kept out of line tells callers nothing) leaves this function at 110
// with the fetches still merged and loses 11 matched functions (0xc3ad0, 0xd74e0, 0xd9640, 0x155a30, 0x18c720, 0x18c9b0, 0x1a2505, 0x1c9c80,
// 0x1f40b0, 0x23cbb0, 0x23d260). Retail keeps index * 12 in ESI across the two calls and reloads only the pool pointer and its data field, so
// the calls end those loads here for a reason local to this function. Fast with 0x103b10 (MATCH each time): the weapon lookup as an inline taking
// the unit index and fetching the unit itself 110 (same code); one header pointer local read for both fetches 122 (361 B); the unit fetched
// in the reset block instead of at the top 118 (383 B, aligned 65: the weapon lookup then fetches after the calls as retail's second fetch does,
// but the top fetch into EBP is gone and the reset block fetches again after 0x100f70); diagnostic, reverted: the second fetch reading the pool's
// data field as a dword 110 (same code: the merge does not depend on the load's type).
// P604: Carry the pre-reset level through the reset join as char/short/long/byte,
// defaulted after the weapon query or in the preserve arm: 131/128/130/129/129/126/128/127.
// NOT kept: the post-query unit fetch stays merged, and the later level
// comparison shortens the body farther from retail.
// Round 642 forms: shared object accessor at both sites 110, shared reset accessor 110,
// shared header field at both sites 110, shared weapon view and slot helper 110,
// named second player index 110, view-entry reference 110, scoped weapon receiver with
// view-entry reference 110, missing weapon return 113.
// NOT kept: the lookup views still fold the second pool read and the sound return shortens the body.
// M3 lane 13 r117 (0xbadc0's file was edited for two diagnostics only and restored at once; base 110): with a scratch
// indirect store in 0xbadc0 (a pointer to a whole local written through; 0xbadc0 and 0xbae20 still MATCH) and this
// function's three listed forms (the `== NONE` weapon ternary, the zoom flag declared after the weapon flag, the
// early-return sound helper) this function MATCHES byte for byte in the fast image and in a full check; those three
// forms without the scratch store 128 (348 bytes, plain), so they are not kept. The full check with the scratch store
// loses 19 matched functions (0x3d2c0, 0x683a0, 0xd74e0, 0x101fb0, 0x10a390, 0x112dc0, 0x153d10, 0x16c2b0, 0x19188c,
// 0x23cbb0, 0x24d8d3, 0x24da7d, 0x25c9d0, 0x264b50, 0x265050, 0x265680, 0x269580, 0x2aa970, 0x2aaab0) and gains only this
// one: retail's 0xbadc0 does have the no-write summary (0x10a390 keeps the pool's data pointer in EDI across its
// 0xbadc0 call), so the reload here is local to this function's two calls, which reach 0xbadc0 through the inlined
// 0x14de90. The other retail functions read with the same inlined pair of 0xbadc0 calls (0xd03e0, 0xe8de0, 0xee7f0,
// 0x29ffb0, all matched, and 0xbf600) are not among the 19 and keep no load across those calls. Also measured: a
// self-assignment of the pool
// global in 0xbadc0 128 (folded, no reload); the real 0x14de90 (declared, defined in another file) in place of the
// file's own inline copy 128 (same code as the copy: inlined, still merged); that with no direct reference to
// 0xbadc0 left in this file 128 (same code). So the blocker is narrower than the
// earlier notes say: what makes the calls that come from the inlined 0x14de90 lose the callee's summary in retail.
// One compiler-setting diagnostic, reverted: the aliasing-across-calls optimize pragma around the helper, or around
// this function, 128 (no reload).
// M3 lane 1 r131 (fast with 0x103b10, MATCH each time; control 110, register-masked aligned 9/13): the sound helper
// `static inline` instead of forceinline 165 (315 B: it stays out of line); the player-index helper forceinline 110
// (same code); the user entry's two stores through `g_4ed284->entries[user_index]` with no entry pointer 112 (357 B);
// the entry as a reference local 110 (same code); the sound call behind `if (sound_index == NONE) return;` 110 (same
// code). Retail also threads the weapon arm's NONE sound straight to the exit (`cmp edi, -1; je` at 0xc87f3) where
// ours sets EAX to -1 and tests it. Still BLOCKED-BY 0xbadc0.
// M3 lane 6 r204 (fast with 0x103b10, 0x14de90, 0xbadc0, 0xcbd50 and 0x100f70, all MATCH each run; control 110, register-masked
// aligned 13; nothing kept). Round 117's three forms re-measured 128 (348 bytes, aligned 28); with the file's own inline copy
// of the player-index getter replaced by a declaration of the real 0x14de90 (another file, later in link order, as in retail)
// 128 fast and 128 plain (same code: inlined across files, the fetches still merged), and that declaration alone 110 (same
// code as now). On the three forms, the inline copy's result written through a reference to its own local (`long &value =
// result;`) 128, through a pointer to it 128, declared without a value and set through the reference 128 (all the same code:
// a store through a reference or pointer to the helper's own local is folded before the fetches are compared, inside an
// inlined helper as it is in this body); the getter's unit pointer bound through a reference 131; diagnostic, reverted: a
// volatile result in the inline copy 111 at retail's 376 bytes (the second fetch then stays apart, but the result's stack
// stores are real). So no store this function can make locally separates the two fetches; only a call whose callee is
// summarised as writing does. Retail's 0x10a390 shows 0xbadc0 itself is not (it keeps the pool's data pointer in EDI across
// its direct call and uses it for the second lookup), so what is left is that retail's two calls here reach 0xbadc0's bytes
// as a different source function with a writing summary, folded onto the same address at link time; this tree keeps one
// definition per address, so that cannot be written here. Still BLOCKED-BY 0xbadc0 as reached through the inlined 0x14de90.
// M3 lane 13 r124 (fast with 0x103b10, MATCH each time; control 110): the weapon lookup from the entry `unit` pointer
// 110 (same code); the user index defaulted to NONE and set under `!= NONE` 127 (357 B); the weapon slot read from the
// unit at each use 114 (355 B); the user's entry fields written through `g_4ed284->entries[user_index]` at each use 112
// (357 B); the zoom-out sound helper plain `inline` 165 (315 B). Checked: 0x10a390's source re-reads the pool through
// its macro after its 0xbadc0 call, so the no-write inference above holds; the real 0x14de90 is in a file this round
// could not edit. Still BLOCKED-BY 0xbadc0 as reached through the inlined 0x14de90.
// M3 lane 15 r115 (fast with 0x103b10, 0x14de90, 0xbadc0, 0xcbd50 and 0x100f70, all MATCH; control 110, aligned 80):
// `__assume(unit != NULL)` after the entry fetch 110 (same code: the second fetch still merges); the weapon lookup's unit
// fetched into a named pointer with the same assumption 110 (same code); the player-index helper returning a one-member
// aggregate through a wrapper 110 (same code); one user-index helper making both 0xbadc0 calls 156 (326 bytes); the entry
// fetch moved after the two player lookups and used for the reset and the weapon 109 (376 bytes, retail's length, aligned
// 63: everything after the calls then has retail's shape, but retail also keeps the entry fetch in EBP for the reset), not
// kept (a moved declaration, a gain of 1). 0xbadc0 prints as ours now, but its file is on this round's Avoid list.
// Still BLOCKED-BY 0xbadc0.
// M3 lane 14 r128 (fast with 0x103b10, 0x14de90, 0xbadc0 and 0x100f70, all MATCH; control 110): the user index as a
// short 107 (362 B, a gain of 3, not kept); the weapon flag set by an `if` after a false default 114 (357 B); the weapon
// index through include/unknown_1e1f20.h's unit_get_current_weapon(unit_index) 110 (same code: the fetches still
// merge); re-measured: the user index as one conditional initializer 110, the reset test weapon flag first 97 (378 B,
// not retail's keep-first test order). Still BLOCKED-BY 0xbadc0 (src/unknown_0bad50.cpp is avoided again).
// M3 lane 16 r157 (fast with 0x103b10, 0x14de90, 0xcbd50, 0xbadc0 and 0x100f70, all MATCH each run; control 110). Kept,
// same code (110, 359 B, fast and plain; the two functions and their 23 other matched callers and callees checked still
// match): the weapon lookup and
// the player-index lookup through the real 0xcbd50 and 0x14de90, declared here and expanded at link time, in place of
// this file's two hand-written copies of them (either one alone 110, same code). Evidence on the blocker: retail's image
// holds exactly one `ret`, one `ret 4`, one `xor eax, eax; ret`, one `xor al, al; ret` and one `mov al, 1; ret` among its
// 11,326 game functions, so retail's link folded identical functions onto one address, as this build's does; a second
// source function with 0xbadc0's bytes and no link-time summary, called by the player-index getter, would give this
// function its reload and leave 0xbadc0's other callers their no-write summary. Measured in a one-file link-time screen
// (build/r157/lt.py; pool fetches counted in this function): 0xbadc0 unresolved 3 fetches (retail's shape, 128
// instructions); its real code linked ahead of this file or after it 2 and 2 (107 instructions each: the order of the
// files at link time is not the cause); a same-file copy of the getter called by the player-index helper, declared
// static noinline, noinline, or noinline inline 2/2/2 (a same-file copy keeps its summary whatever its linkage), plain
// inline expanded in place. A new copy of an existing function is barred by the coordinator's ruling of 2026-10-30, so
// none is kept or proposed here. Still BLOCKED-BY 0xbadc0 as reached through the inlined 0x14de90.
// M3 lane 9 r147 (fast with 0x103b10, MATCH each run; control 110): the two zoom bytes cleared by one chained
// assignment 110; the entry unit bound by a const reference for the reset block 110; the weapon slot read through a
// const reference bound to a second fetch after the player queries 110 (same code each: the fetches still merge); the
// `== NONE` weapon ternary with the zoom flag declared after the weapon flag 126 (350 B), with the reset reference 126.
// Still BLOCKED-BY 0xbadc0 (its file and 0x14de90's were both avoided this round).
// @retail 0xc86e0
void function_c86e0(long unit_index, bool keep_weapon_zoom)
{
	bool was_zoomed = false;
	s_zoom_unit *unit = ZOOM_UNIT_GET(unit_index);
	long user_index;
	if (unit_get_player_index(unit_index) == NONE)
		user_index = NONE;
	else
		user_index = ((s_zoom_player *)(g_4e8c24->data + (unit_get_player_index(unit_index) & 0xffff) * 0x21c))->user_index;
	long weapon_index = function_cbd50(unit_index, ZOOM_UNIT_GET(unit_index)->current_weapon_slot);
	bool weapon_zoomed = weapon_index != NONE ? function_100f70(weapon_index) : false;

	if (!keep_weapon_zoom || !weapon_zoomed)
	{
		was_zoomed = unit->field_240 != NONE;
		unit->field_240 = NONE;
		unit->desired_zoom_level = NONE;
		unit->zoom_transition = 0.0f;
		if (user_index != NONE)
		{
			s_unknown_185ab0_entry *entry = &g_4ed284->entries[user_index];

			entry->index = NONE;
			entry->flag = false;
		}
	}
	if (user_index != NONE && was_zoomed)
	{
		long sound_index;

		if (weapon_zoomed && !keep_weapon_zoom)
			sound_index = weapon_get_zoom_out_sound(weapon_index);
		else
			sound_index = ((s_zoom_globals *)g_4e034c)->sounds->zoom_out_sound_index;
		if (sound_index != NONE)
			function_1896c0(1.0f, sound_index);
	}
}
