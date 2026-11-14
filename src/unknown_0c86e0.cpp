// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0C86E0.CPP: a unit leaving its weapon's zoom. Lane S
   wrote it for the weapon functions 0x103b10..0x103ce0, which call it. */

#include "unknown_11c920.h"
#include "globals.h"

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
bool function_100f70(long weapon_index);
long function_1896c0(real scale, long tag_index);

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

static inline long unit_get_player_index(long unit_index)
{
	s_zoom_unit *unit = (s_zoom_unit *)function_badc0(unit_index, 3);
	long result = NONE;

	if (unit)
		result = unit->player_index;
	return result;
}

static inline long unit_get_current_weapon(s_zoom_unit *unit)
{
	short slot = unit->current_weapon_slot;
	long result = NONE;

	if (slot != NONE)
		result = unit->weapon_indices[slot];
	return result;
}

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
// score 107 (was 107): Frameless entry saves, fresh unit lookup and late prior-state carrier remain.
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
// @retail 0xc86e0
void function_c86e0(long unit_index, bool keep_weapon_zoom)
{
	bool was_zoomed = false;
	s_zoom_unit *unit = ZOOM_UNIT_GET(unit_index);
	short user_index;
	if (unit_get_player_index(unit_index) == NONE)
		user_index = NONE;
	else
		user_index = ((s_zoom_player *)(g_4e8c24->data + (unit_get_player_index(unit_index) & 0xffff) * 0x21c))->user_index;
	long weapon_index = unit_get_current_weapon(ZOOM_UNIT_GET(unit_index));
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
