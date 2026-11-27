// @flags /O2 /Gr
/* UNKNOWN_08C5B0.CPP: the settings bungie.net keeps for a signed-in
   user; each handler applies one to the user's controller (lane D) */

#include "unknown_11c920.h"
#include "globals.h"
#include "pending_messages.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>

/* what a handler is given */
struct s_bungienet_user_request
{
	byte unknown00[0xe];
	short kind;
	long controller_index;
};

typedef bool (__stdcall *bungienet_user_handler)(s_bungienet_user_request *request);

struct s_bungienet_user_handler
{
	long unknown00;
	bungienet_user_handler handler;
};

/* the controllers' state (g_54e8e0, 0xc70 bytes each); this file reads only
   the voice setting at +0x204 */
struct s_controller_voice_view
{
	byte unknown000[0x204];
	long voice_setting;
	byte unknown208[0xc70 - 0x208];
};


/* The request header and its payload have separate lifetimes. */
extern s_pending_message_header g_479750;
struct s_user_settings_payload
{
 dword header[4];
 long voice_setting;
 byte field_14[0x300];
};
s_user_settings_payload g_4d8800;

void function_18fe9e(long index);

static inline s_controller_voice_view *controller_voice_view_get(long controller_index)
{
	return &((s_controller_voice_view *)g_54e8e0)[controller_index];
}

// @retail 0x8c5b0
bool __stdcall bungienet_user_voice_reset(s_bungienet_user_request *request)
{
	long controller_index = request->controller_index;
	s_controller_voice_view *controller = controller_voice_view_get(controller_index);
	long old_setting = controller->voice_setting;
	bool changed = false;
	if (old_setting != 1)
		changed = true;
	controller->voice_setting = 1;
	if (changed)
		function_18fe9e(controller_index);
	return true;
}

static inline bool bungienet_block_available(void)
{
	if (g_479750.kind != 0 && (g_479750.flags & 2))
		return true;
	return false;
}

static inline long bungienet_user_voice_setting(short kind)
{
	long setting;
	if (kind == 1)
	{
		if (bungienet_block_available())
			setting = g_4d8800.voice_setting;
		else
			setting = 1;
	}
	else
		setting = 1;
	return setting;
}

// score 4 (was 4): only the request pointer and the kind differ: retail ECX and AX, ours EAX
// and CX. Before round 55 retail kept the request pointer in ECX, the
// kind in AX, the setting in ECX and `changed` in DL; ours used EAX, CX, EDX and CL.
// The volatile read keeps retail's 16-bit load and register compare of the kind (a plain
// read compiles to `cmp word ptr [eax + 0xe], 1`, 32). Same output as the older
// local_0/local_1 copy form. Tried: plain read (32); kind read inside the condition (32);
// controller index read first (12); if/else and `setting = 1; if` forms (11, 15);
// `switch (kind)` (17, puts setting in ECX but movsx/dec the kind); word kind (11);
// bool temp for the condition (41); `changed = setting != old` (25); without the volatile:
// helper reading the kind from the request 32, kind by const reference 32, single-exit setting
// helper 30, nested kind test 32 (every matched function with retail's unfolded word load and
// register compare tests the value twice, e.g. `type == 3 || type == 4`).
// Round 55: the setting helper as a single exit with an else on each failing arm 4 (kept: setting ECX,
// changed DL); nested returns `if (kind == 1) { if (available) return x; return 1; } return 1;` 4; the
// same with a plain read 32 (95 bytes, folded compare), single exit with a plain read 32, the kind passed
// straight 32; an added `kind == 0` test first 32 (103 bytes, but retail's ECX/AX pair); the index read
// first 5; word/long kind parameter 4/4; inverted `kind != 1` chain 4; `old != setting` 5. Names, a
// byte-offset read, the dword's high half, a reference parameter: the compare still folds.
// Round 121: plain read through a `short const *` helper parameter 32 (95 bytes); forceinline helper 4 (same
// code); the controller pointer taken before the setting 15; the controller index read at each use 23 (99
// bytes); a one-expression ternary helper, plain read 32. One-file LTCG probe (exact here), plain read: a 16-bit
// bitfield kind, a byte-pointer read, the header type's view, by-reference/pointer parameters, `1 == kind`, the
// helper written in place: all fold the compare (`cmp word ptr [eax + 0xe], 1`).
// Round 466: Controller before selector 15; old state before selector 24; controller before kind 14;
// controller before kind then old state 24; complete controller/old state before kind 24;
// the latter three early-old-state graphs with direct change-result production 39/38/38.
// Original acquisition order restored; request/kind registers remain.
// Round 219: availability nested/negative-guard/true-result forms 4/4/4;
// native bit availability 31; named selection result 11; availability outside kind 9;
// nested availability with long change result 12, negative guards with short result 13.
// Original request/kind pair remains EAX/CX versus retail ECX/AX.
// Round 145: a plain read stays in a register when the kind has a second use that constant-propagates
// away: `setting = kind` in the not-available arm 4 (same code as the volatile read; not kept, the
// assignment only steers). Kind read, setting, then index 23 (kind AX, request EDX); index first with that
// arm 5; handler typed as the pending-message header with a last-result helper 32; `kind == 0` arm
// merged into the default 32 (103 bytes, retail's ECX/AX pair).
// Round 106: the kind read inside the selector, index first 5; selector first 23;
// selector without inline, availability force-inline, controller access force-inline,
// const request parameter, const scalar/pointer locals and external-inline selector 4 each.
// Original helper boundaries, parameter and local forms restored; request/kind registers remain.
// Round 58: plain reads: a bool from the kind 32 (95 bytes); kind plus an unused bool 32; `setting = 1` seeded 30
// (94 bytes); seeded `kind == setting` 30; the ternary with `: kind` in the not-available arm 4 (same code as the
// volatile read, the assignment only steers; not kept), with the index first 5, selector first 23 (kind AX, request EDX), if/else
// form 4. Field pointers: kind 32, index 32, both 32, index with the volatile read 4, kind or index with the `: kind` arm 4/4.
// `request->kind` written at each use (no local): index first 32, selector first 30, in both arms 32, `else if` setting 45 (110 bytes); a
// second `kind == 1` in the else 4.
// Round 159 (fast): plain read with the kind seeding the setting 31 (103 bytes; retail's ECX/AX pair, but a movsx survives);
// `long setting = kind; if (setting == 1 && available)` 32 (95 bytes); seeded `setting = 1; if (kind == setting && available)` 30
// (94 bytes); `setting = kind` first in the kind arm 12; inner `else setting = kind` with `available && kind == 1` 4 (same code).
// The pair follows the kind's surviving references: it outranks the request only with a third use that reaches allocation.
// Round 85 (fast, plain reads, no volatile): the existing selector called under an outer `kind == 1` test, as a
// ternary 4 and as if/else 4 (unfolded load, request/kind still EAX/CX); the selector under `switch (kind)` with
// case 1/default 38 (104 bytes); `kind != 1` chain written in place 32 (95 bytes); availability read into a bool
// first, `setting = 1` seeded 43 (108 bytes). The matched sibling 0x8c5b0 loads the request into EAX (no kind).
// Round 114: direct kind as the selector argument 5; register kind/request 4/4; pointer/reference request aliases 4/4
// (NOT kept: aliases require a match); request-taking selector with the index before/after it 5/23;
// request-member selector with a kind local/direct kind 4/5. Original helper and body restored.
// Round 75 (lane 1, fast): an `__assume` on the kind keeps its word load in a register without the
// volatile read: plain read with `__assume(kind == 1)` in the kind arm 4 (same code; not kept, it is always true where
// it stands and only holds the value in a register), `__assume(kind >= 0)` after the read, the index or the setting,
// or in the helper 4 each, two and three assumes 4/4. The pair stays EAX/CX; retail ranks the kind above the request.
// A 600 s permuter run on the plain-read form: 48 tries, none below 4. With the assume or the volatile read: `__assume(request != NULL)`
// 4 (plain 32), the index first 5, const reference or pointer local for the kind 4/4, const reference for the index
// 4, getter helper 4/4, the request read once through its own address 4/4. `setting = kind` on both failing arms 4,
// a byte-width second test of the kind 33 (103 bytes).
// Round 172 (m3 lane 4, fast): a shared inline controller-voice setter (index, setting) used by 0x8c5b0 as well
// (0x8c5b0 stays matched): the plain kind read as the selector's argument 30 (95 bytes), the volatile read in
// the call 23; the setter taking the request 30/23 (plain/volatile); the kind and index read first, then the
// setter 4 (same code). Original body restored.
// Round 156 (m3 lane 6): the selector writing through an out parameter 4 (fast; with a plain read 32); `bool result =
// true` declared first 4 (fast). LTCG one-file harness (this file, unknown_190001.cpp and the ballast reproduce 4): 45
// combinations of volatile/plain/cast kind read, five selector shapes, read order and compare sides best 4 (plain reads
// 11 or more); `switch (kind)` with `case 0:` sharing the default and a plain read 5 (29 instructions, `movsx; dec` for
// the kind), with the volatile read 30. CL-only: 0x8c790's identical `kind == 1` stays unfolded only beside its
// 0x80e14 call (nonzero arguments keep the unfold; removing the call folds its compare as well).
// Round 157 (m3 lane 6, harness): the controller and the setting take EAX and ECX first, then whichever of the request
// and the kind comes next takes EAX; retail allocated the kind first. Raw byte-offset reads as in 0x8c790, plain 29 (the
// compare folds); a seeded selector with a plain read 24; `available && kind == 1` 15 (fast 16); the volatile read with the index
// read after the selector 23; a plain read with a caller-side `kind != 1` test around the selector 4 (fast 4; the unfolded load
// without the volatile read, same pair; a repeated test, not kept).
// Round 161 (m3 lane 5, fast, plain reads): the default path as a call of the reset handler 76 (135 bytes, no merge); a word
// kind through the selector 32 (95 bytes); the selector on the field with the index after 30 (a repeat, re-measured); a
// switch with `case 1` falling into `default` 19 (95 bytes, `movsx; dec`); that switch with the index read after it 28 (95
// bytes: retail's ECX request and EAX kind, but the switch keeps `movsx; dec` for the 16-bit compare).
// Round 171 (m3 lane 6, round 157's harness): 1-8 initialised unused locals at the top 4 each (diagnostic); the
// volatile read with a seeded default (`long setting = 1; if (kind == 1 && available) ...`) 12; the request's fields
// read through a const source pointer local 4; index first with the volatile read as the selector's argument 5; the
// kind held as a long 4; the kind arm's choice as a ternary 4.
// Round 86 (m3 lane 14, fast, plain reads, no volatile): a top-level const request parameter 32 (95 bytes); the kind
// held as a long 32; the selection as one ternary in place of the helper call 32; `old_setting != setting` 32; the
// controller index read after the selector 30 (95 bytes). The compare folds in every plain-read form.
// Round 174 (m3 lane 6, round 157's harness, counts as its positional differences): the selector taking the
// request by value 37 (37 instructions, the copy stays), a by-value local copy for both reads 37, the by-value
// selector with the index read after it 37; the selector taking the request by const reference, plain read 29 (28
// instructions, folded). Diagnostic: the matched sibling 0x8c790's identical `kind == 1` stays unfolded only
// through its inlined 0x80e13 time-stamp read (a compile-only copy folds without it, with its result unused, or
// with 64-bit arithmetic alone); here a plain read still folds with an asm block written into the handler.
// Round 195 (m3 lane 4, fast, all with the volatile read): `setting = kind` in the not-available arm 4 (same code); a
// `kind == 0` arm ahead of the `kind == 1` arm 32 (103 bytes: retail's ECX/AX pair, the extra test survives); the volatile
// read passed straight as the selector's argument, index first 5; the availability test as one returned expression 4;
// `short const kind` 4.
// Round 200 (m3 lane 4, fast, all with the volatile read): the controller looked up for the read and again for the store
// 12 (95 bytes); the changed flag declared above the kind read 33 (94 bytes); the controller as a reference 4; the voice
// setting read into a local ahead of the availability test 7; setting and old setting as `dword` 4.
// Round 184 (m3 lane 6, fast, all with the volatile read): the selection written in place as if/else with `kind == 1 &&
// available` 11, seeded `setting = 1` with the availability fields tested in place 15, one ternary 11; the availability
// helper taking the header's address 4, also passed through the selector 4 (same code). Diagnostic: the kind compared with
// the settings definition's first field (a 1 the compiler cannot see) loads the kind with movsx after the request (34).
// Round 128 (m3 lane 9, fast, all with the volatile read): the index through an inline accessor taking the request 4; the
// index declared ahead of the kind and assigned after it 4; the setting declared first and assigned after the reads 4;
// index, setting and kind all declared first and assigned in order 4; a `dword` index 4 (all the same code). Re-measured:
// the index read first 5 (the kind then takes AX but its load follows the index's). Diagnostic (flags, not kept): the file
// with `/arch:SSE` like the matched siblings 0x8c790 and 0x194260 (both load the same +0xe word unfolded with a plain read)
// and a plain read 32 (95 bytes: still folded), so the flag is not what unfolds it.
// M3 lane 14 r101 (fast with 0x8c5b0): with the existing kind read, no old-setting local (the stored setting compared in the
// test) 4 (same code), `changed` set by if/else with no initialiser 25 (91 B); plain reads: the selection written in place
// as `if (request->kind == 1 && available) ... else setting = 1` with the index read first 32, the controller looked up
// before the selector 31, the read passed straight to the selector ahead of the index 30 (95 B each, compare folded).
// M3 lane 14 r104 (fast with 0x8c5b0; plain where noted), all with a plain read (no volatile): an `else if (kind == 0)` arm
// after the kind-1 arm, also giving 1, 4 (the volatile read's code exactly: the second compare keeps the word load in a
// register, then goes when its two arms merge); `else if (kind == 0) ... else if (kind == 2) ...` with every arm but kind 1
// giving 1, MATCH (fast and plain), and `else if (kind == 0 || kind == 2)` the same, MATCH (fast): the extra compares give
// the kind the third use that puts it in AX and the request in ECX. Not kept: those arms are identical to the else (the
// 2026-10-30 ruling on identical branches). Lead: retail's source tests the kind against 0 and 2 as well, in arms that
// compile to the default; a natural form of that with non-identical arms should match. Also: a three-case switch
// (0, 1, 2, default) 33 (100 bytes, `movsx; sub; dec` chain); the selection as one ternary in place 32 (95 bytes).
// M3 lane 11 r112 (fast with 0x8c5b0/0x8c660; base 4): the request read through the pending-message header's own fields
// (the matched siblings 0x8c790/0x194260 read this word the same way), plain: the kind and index from `unknown0c`'s high word
// and `unknown10` 32 (95 B), the selection as the siblings' one condition 32 (95 B: both fold the compare); the selector
// taking `bool succeeded` (`kind == 1` at the call, the volatile read kept) 4 (same code). Diagnostics (identical arms, not
// keepable): plain read with one extra `else if (kind == 2)` arm giving 1, 4; plain read with an `else if (kind < 0 ||
// kind > 2)` arm giving 1, MATCH (fast). So any two late-merged compares of the kind give retail's ECX/AX pair, an equal
// pair or a range test alike; retail's source evidently had arms for other results that compile to the default.
// Round 135 (m3 lane 9, fast with 0x8c5b0): an inline member accessor for the controller index 4 (same code); the
// selection as an inline member of the request reading the kind, ahead of the index 23 (kind AX, request EDX), after
// the index 5 (request EAX, kind AX, the index load first); the request read through a reference binding 4 (same code).
// A plain read into a reference-bound kind (`short kind; short &result = kind; result = request->kind;`) gives 4, the
// volatile read's exact code: the reference also keeps the word load out of the compare (not kept: a steering form
// without a match, but it is the replacement if the volatile read has to go).
// M3 lane 11 r116 (fast with 0x8c5b0/0x8c660, both matched each time; base 4), all with plain reads (no volatile): an
// inline controller-voice setter (index, setting) called in each arm, `kind == 1 && available` with the block's
// setting and the else arm with 1, 69 (130 bytes, the two expansions do not merge); the same as an early return for the
// failing case 69 (same code); inline member accessors on the request for the result word and the controller index 32
// (95 bytes, folded compare); a single-exit `succeeded` helper reading the request, the nested selection written in
// place on that bool 36 (103 bytes: the request then takes ECX, but the bool is built in AL and tested); the result
// word and the index read through one `short const *` to the result field 32 (95 bytes, folded). Reading: this handler
// is the definition's received callback (0x81050 calls it for a finished receive; the +0xe word is that message's
// result, 1 for success), and the matched received callbacks 0x8c790/0x194260 test only `== 1`, so nothing here names
// a second result value for the other arms that round 104's matching chain needs.
// M3 lane 15 r100 (fast with 0x8c5b0/0x8c660), all with plain reads (no volatile): a `word` kind 32; the selector an
// ordinary non-inline function 32 (inlined at link time, folded); the selector taking `short const &` 32; the selector
// `__forceinline` 32; an inline accessor writing the kind through an out pointer 32 (that indirection folds once inlined,
// unlike a reference-bound local).
// Round 103 (m3 lane 2, fast with 0x8c5b0/0x8c660, both matched each time; base 4), all with the existing kind read: the
// controller looked up ahead of the selector 15; the controller and its old setting read ahead of the selector 24; the
// change test as `old_setting != setting` 5; a top-level const request parameter 4 (same code); the selector with early
// returns (`if (kind != 1) return 1;`, then the availability test) 4 (same code).
// M3 lane 6 r198 (fast with 0x8c5b0/0x8c660): the kind as a `short const &` bound to the request field (plain) 32
// (95 bytes, folded); the controller index read into a reference-bound local 4; both reads through a reference-bound
// copy of the request pointer 4 (diagnostic); the controller pointer declared at the top and assigned after the
// selector 4; the availability and selector helpers defined ahead of the controller accessor 4 (all the 4s same code).
// M3 lane 12 r147 (fast with 0x8c5b0/0x8c660, both MATCH each time; control 4): the body after the reads as a forceinline
// helper taking (index, kind) from `request->...` at the call 32 (95 B, plain read folded; as a plain inline helper it
// stays out of line, 24 B); diagnostic, the selector called twice (in the change test and the store) with a plain read 79
// (135 B, the two expansions do not merge); with the existing read, the selector seeded `setting = 1` with the nested
// `kind == 1` and availability tests 15; a `bool received = request->kind == 1;` passed to the selector 32; the kind
// compared with a named enum constant, plain read, 32 (95 B). The identical-arm chain that matched (r104) stays out by
// the 11-18 ruling: retail shows one compare of the kind, not one per value.
// M3 lane 11 r129 (fast with 0x8c5b0, MATCH each time; control 4), all with plain reads: the selection as a switch with
// one case and a default 13 (95 bytes, folded compare), seeded `setting = 1` with a one-case switch 17, the switch written
// in place on the request's field with the unavailable case falling into the default 19; the selection as an inline const
// member of the request, called ahead of the index read 30, after it 32 (95 bytes each, folded).
// M3 lane 14 r119 (fast with 0x8c5b0, MATCH each time; control 4), plain reads (no volatile): the controller index, the
// controller and its old setting read ahead of the selector, the field passed straight, 31 (95 B); the selector as early
// returns (`if (kind != 1) return 1; if (!available) return 1;`) 36 (104 B); the changed flag declared first 29 (91 B);
// the availability as one returned expression with a `short const` kind 32 (95 B). Diagnostic: K = 9 to 31 (odd) dead
// initialised longs at the top, with the existing read, 4 at every K (the request/kind pair is not a symbol-count tie).
// M3 lane 16 r148 (fast with 0x8c5b0/0x8c660, both MATCH each time): the availability as an inline helper returning the
// payload pointer (NULL when unavailable) with the setting seeded 1, plain read, 30 (94 B, folded compare); the store and
// change test as an inline setter returning `changed`, with the volatile read 4 (same code), with a plain read 32 (95 B).
// Re-measured: the index read first with the volatile read 5 (request EAX, kind AX: the request dies before the kind
// loads), with a plain read 32.
// M3 lane 7 r194 (fast with 0x8c5b0/0x8c660, both MATCH each time), plain reads: `register short kind` 32 (95 B); the
// availability part as its own inline helper (seeded, `if (available) setting = block;`) called as `kind == 1 ? helper() : 1`
// 30 (94 B); that helper with early returns under `if (kind == 1) ... else setting = 1;` 32 (95 B); seeded `setting = 1` with
// `switch (kind) { case 1: if (available) setting = block; break; }` 17 (95 B, `movsx; dec`); the same switch on the field
// itself with the index read after it 26 (95 B).
// M3 lane 6 r209 (fast with 0x8c5b0/0x8c660, both MATCH each run), plain reads: the kind copied and passed through a
// `short const &` to the copy 4 (same code as the volatile read); a second request pointer local for the kind read 32 (95 B);
// the kind as a `short const &` to the field passed through a second const reference 32 (95 B); the index as a `long const &`
// to the field with the kind copy through a const reference 23 (99 B); the selector's result stored through a `long &` bound
// to the setting 32 (95 B).
// M3 lane 3 r279 (fast with 0x8c5b0/0x8c660, both MATCH): plain reads: a top-level const request pointer 32 (95 B), the compare
// against `(short)1` 32 (95 B), the selector taking the availability as a bool argument 43 (108 B; with the existing read 43);
// the controller taken from the real slot array (`&g_54e8e0[index]`, then the view cast) 4 (same code).
// M3 lane 6 r213 (fast with 0x8c5b0/0x8c660, both MATCH each run; control 4): the kind read by a two-byte memcpy into a
// plain short (no volatile) 4 (the volatile read's exact code: the load stays unfolded, the pair stays EAX/CX); both
// fields read by memcpy 4 (same code); the index read first with the memcpy kind 5; plain reads: the kind test as `kind > 0
// && kind < 2` 32 (103 B, both compares survive), `(word)(kind - 1) == 0` 12 (96 B); an out-of-range kind reset to 0 ahead
// of the test 45 (111 B, the range compares survive).
// M3 lane 16 r155 (fast with 0x8c5b0/0x8c660, both MATCH each time; control 4): `bool changed = setting !=
// old_setting;` with the existing read 25 (91 B: the flag is built with setne); the selector's parameter `short const`
// 4; the selector `__forceinline` with the existing read 4; the existing read through the parameter's own address
// (`request_reference`) 4 (same code); a plain read of both fields through one read of the parameter's address 32
// (95 B, folded compare).
// M3 lane 7 r203 (h2m fast with 0x8c5b0, MATCH each time), plain reads: an inline test `kind == 1 && available` taking the
// kind, the setting as a ternary on it 32 (95 B); both fields read through a `const &` to the request 32; seeded `setting =
// 1` with `request->kind == 1 && available` in place 30 (94 B); `bool received = kind == 1 && available;` then a ternary 32;
// re-measured: the two-byte memcpy kind read 4 (the volatile read's code).
// M3 lane 3 r281 (fast with 0x8c5b0/0x8c660, both MATCH each time; control 4): the selector's arms as one test (`kind != 1
// || !available` gives 1) 11; no old-setting local, the change test reading the stored setting, 5; the selector defined
// after the handler (forward-declared) 4 (same code); re-measured the reference-bound plain read 4 (same code). A 600 s
// permuter run on the current form, 57 tries, best 4.
// M3 lane 15 r121 (h2m fast with 0x8c5b0, MATCH each time; control 4; the existing kind read kept): the store ahead of the
// change test 28 (88 B); the availability read into a bool first with `kind == 1 && available ? payload : 1` 43 (108 B);
// old_setting declared first and assigned after the lookup 4 (same code); the selection in the handler as `if (kind == 1)
// setting = available ? payload : 1; else setting = 1;` 4 (same code); a single-exit `bool result = true;` 4 (same code).
// M3 lane 14 r136 (h2m fast with 0x8c5b0/0x8c660, both MATCH each run; control 4), all with the existing kind read: the
// selector taking `short const &` 4; the selector as `switch (kind)` with `case 1:` (the availability test) and a default
// giving 1, 30; a second request pointer local for the kind read 4; the setting seeded 1 with a redundant `setting = 1;`
// at the top of the kind arm and the availability test after it 15; diagnostic: the selector given the kind twice through
// an extra unread parameter 4 (plain read 32): one more reference that LTCG drops does not reach the pair.
// M3 lane 15 r127 (h2m fast with 0x8c5b0, MATCH each run; control 4): the volatile read passed straight to the selector
// with the index read after it 23 (kind AX as retail, but the index loads after the selection, so the request lives on
// in EDX); the kind's `== 1` test held in a bool ahead of the index read 32 (100 B: the volatile load folds into the
// compare, then `sete`); a shared forceinline setter (index, setting) for both handlers, 0x8c5b0 calling it with 1 (still
// MATCH), with the volatile read in the setting argument 23, with the named kind local 23 (arguments evaluate right to
// left, so the index loads after the selection); diagnostic, a forceinline (index, kind) helper doing the selection and
// the store 4 (same code: kind and index both load ahead of the branch, request EAX, kind CX). Reading: with both loads
// in the entry block the request (three references) outranks the kind (two); retail's kind ranks first.
// M3 lane 13 r135 (h2m fast with 0x8c5b0/0x8c660, both MATCH each run; control 4): the reference-bound plain read
// with the selection written in the handler (`if (kind == 1) setting = available ? payload : 1; else setting = 1;`) 4,
// with the selector `__forceinline` 4 (both the volatile read's code, no volatile); `changed` declared first with the
// two-byte memcpy kind read 33 (94 B); the reference-bound read after the index read 5; the selector seeded `setting
// = 1` with the nested kind and availability tests 15. Diagnostics, plain reads: a switch with `case 1`, `case 0: case
// 2:` sharing one arm and `default: __assume(0)` 37 (103 B); `__assume(kind == 0 || kind == 2)` in the else arm 32 (95
// B, folded compare): neither gives the two late-merged compares of the identical-arm chain.
// M3 lane 11 r148 (h2m fast with 0x8c5b0, MATCH each run; control 4): the controller index read from the request at
// each use (no index local) 23 (99 B), with a plain kind read 30 (96 B); the selector's result stored straight into the
// controller with the change test on the stored field 39 (88 B), plain 41 (85 B); the availability arm inverted 4 (same
// code); the request parameter and the handler type pointer-to-const with a plain read 32 (95 B, folded).
// M3 lane 3 r293 (h2m fast with 0x8c5b0/0x8c660, both MATCH each run; control 4): with the existing kind read, the
// selection through an inline payload getter (`kind == 1 && available` gives the payload's address, else NULL) and an
// if/else on that pointer 11 (the setting then takes EDX); the kind test inside the availability helper with the setting
// as one ternary on it 11; the payload getter single-exit (`payload = NULL; if (kind == 1) { if (available) payload =
// ...; }`) with a ternary on it 11; the first two with a plain read 32/32 (95 B, folded compare).
// M3 lane 3 r295 (h2m fast with 0x8c5b0/0x8c660, both MATCH each run; control 4): plain reads: the kind through an inline
// accessor on the request returning the result word 32 (95 B, folded compare), the accessor returning `short const &` bound
// to a `short const &` local 32 (95 B), `short kind = 0; kind = request->kind;` 32 (95 B); the availability tests nested in
// the selector (two `if`s on the header's fields, each failing arm giving 1) with the existing read 4 (same code), with a
// plain read 32 (95 B).
// M3 lane 13 r141 (h2m fast with 0x8c5b0/0x8c660, both MATCH each run; control 4), all with the existing kind read:
// the kind local `short const` 4; the controller index declared ahead of the kind and assigned after it 4; the kind
// renamed `result` (the received message's result word, so it sorts after `controller_index`) 4 (the three 4s the
// same code); `changed` declared first 33 (94 B); the selector taking `long` 4 (same code: the widening folds).
// M3 lane 2 r136 (h2m fast with 0x8c5b0/0x8c660, both MATCH each run; control 4), all with plain reads (no volatile): a
// `long` kind local passed to a `long` selector parameter 32 (95 B); an `int` kind local 32 (95 B); the kind arm as
// `setting = available ? payload : 1;` in the selector 32 (95 B); the availability helper single-exit (`bool available`)
// 32 (95 B); the selector seeded `setting = 1` with the payload setting read into a local ahead of the kind test 16 (96 B:
// the compare still folds, `cmp word ptr [eax + 0xe], cx` with the 1 in ECX). Re-measured: seeded `setting = 1` with
// `kind == 1 && available` in place 30 (94 B).
// M3 lane 13 r144 (h2m fast with 0x8c5b0, MATCH each run; control 4), diagnostics with plain reads (no volatile):
// `__assume(kind >= 0 && kind <= 2)` after the reads 4 (the volatile read's code), `__assume(kind != 0 && kind != 2)` 4,
// `__assume(kind == 0 || kind == 1 || kind == 2)` 32 (95 B, folded); the selector's last arm as `else if (kind != 1)`
// (an implied test) 4; the selector seeded 1 with `switch (kind)` on case 1 and empty `case 0: case 2:` arms 30. All
// restored.
// M3 lane 1 r156 (h2m fast with 0x8c5b0, MATCH each run; control 4), all with the existing kind read: the selector as
// `if (kind == 1 && available) return payload; return 1;` 11; no old-setting local with the change test as
// `controller->voice_setting != setting` 5; the selector seeded `setting = 1` with `kind == 1 && available` and the
// availability helper as one returned expression 15; the payload setting read into a local at the selector's top 10.
// Diagnostic (a repeated test, not keepable): `kind == 1 ? selector(kind) : 1` at the call 4 (same code).
// M3 lane 1 r158 (h2m fast with 0x8c5b0, MATCH each run; control 4), all with the existing kind read: the header's kind
// read into a local in the availability helper 4; the kind declared, then assigned 4; the handler taking an untyped
// request pointer cast to the request inside 4; both fields read through a pointer to the result word (`fields[0]`,
// the index at the next dword) 4; the payload read through a const reference in the selector 4 (all the same code).
// M3 lane 8 r226 (h2m fast with 0x8c5b0/0x8c660, both MATCH each run; control 4), all with a reference-bound plain kind
// read (no volatile; 4 alone): the controller index read through a reference-bound copy of the request pointer 4; the
// kind read through that copy with the index through the parameter 4; the controller index read into a reference-bound
// local as well 4 (the three 4s the same code); the setting built through a reference (`selected = 1;` then the test
// storing the block's setting) 15 (the 1 takes ECX ahead of the compare, kind DX). Diagnostic: the controller index
// read from the message header global in place of the request 11 (101 B): the request still takes EAX and dies at the
// kind's load, so the kind lands in AX only because EAX is free again; retail's ECX request means the kind was given
// EAX before the request was placed, which here only the two late-merged compares of r104 reproduce.
// M3 lane 13 r157 (fast with 0x8c5b0, MATCH each run; control 4): allocator trace of the volatile-read body: the
// request and the kind rank equal (20 each: two references apiece in the entry block) and the request wins the tie, so
// it takes EAX; one more kind reference that survives to allocation, or one fewer request reference, gives retail's pair
// (forced, never source). The kind by a two-byte memcpy (no volatile) 4 with the same ranking; that with the index declared
// first 4; both fields by memcpy 4; the controller accessor taking `long const &` 4; 1-3 dead locals leave the ranking as it
// is. The r104 identical-arm chain (diagnostic) ranks the kind 20 over the request 16.
// M3 lane 7 r229 (fast with 0x8c5b0/0x8c660, both MATCH each time; control 4). Allocator reading: the entry block
// holds the request's two uses and the kind's load and compare, both 20; the request wins the tie because its first
// reference comes first (it is created last), and one point less for the request is a byte match (a forced-ranking search).
// Neither has a reference outside the entry block, so only a kind reference that survives in another block, or a
// request live through a block without a use, can break the tie. Forms: the index read after the selector 23; the
// changed flag declared ahead of the selector with the stored setting compared in place 33 (94 B); the availability
// tested ahead of the kind in the selector 16 (first difference +0x0, the global test leads); the controller looked up
// ahead of the selector 15 (the tie stays: 24 each); diagnostic, a plain read with `__assume(kind >= 0 && kind <= 2)`
// in place of the volatile read 4 (same code; the range is not known to hold).
// M3 lane 6 r245 (h2m fast with 0x8c5b0/0x8c660, both MATCH each run; control 4): the body inside `do { } while (false)`
// 96 (150 B: the 0x18fe9e call is then expanded in place; the pair stays EAX/CX), inside `for (;;) { ...; break; }` 4,
// inside `while (true) { ...; return true; }` 4 (both the same code: no block boundary is left ahead of the kind's load);
// plain reads of the result word as the high half of the header's `unknown0c` dword, `(short)(unknown0c >> 16)`, the same
// through a dword cast, or from a named long copy 32/32/32 (95 B each: narrowed to the same folded word compare).
// Allocator reading: an equal ranking is decided by where each value was last defined, later blocks first and, inside
// one block, the earlier definition first; the request's load always precedes the kind's in the entry block, so the tie
// cannot flip there. The r104 chain wins by ranking instead (kind 20 over request 16): its `kind == 0` compare is still
// there when registers are chosen and goes when the arms merge afterwards. A second compare of the kind outside the
// entry block is the only reference found that leaves no code, so a legal form needs arms that really differ.
// M3 lane 6 r246 (h2m fast with 0x8c5b0/0x8c660, both MATCH each run; control 4), all with the existing kind read: the
// selector as two early returns (`kind != 1`, then not available, then the payload setting) 40 (107 B); no
// controller-index local, the lookup and the notify call reading the request's field at each use 23 (99 B); the
// availability helper as one returned `&&` with `(flags & 2) != 0` 4; the kind compared at the call with the selector
// taking a `bool` 4; the handler's request parameter `*const` 4 (the three 4s the same code). Re-measured: no
// old-setting local, the stored setting compared in the test, 4.
// M3 lane 15 r150 (h2m fast with 0x8c5b0/0x8c660, both MATCH each run; control 4). Allocator reading: the request and the
// kind both rank 20 from the entry block alone; the tie goes to the request because its parameter definition is stamped
// later (8) than the kind's load (6) (inside one block an earlier definition stamps higher, a later block higher still),
// so only a kind definition or reference outside the entry block can flip it. Forms, all with the existing kind read: both
// handlers' request parameter as a reference (with the handler type) 4 (same code); the notify call reading the request's
// index field again (the lookup on the local) 27 (99 B), that with the index read after the selector 23 (99 B); the kind
// tested in the handler around a seeded availability helper, `kind == 1 ? helper() : 1`, 12; an early-return availability
// helper under `if (kind == 1) ... else setting = 1;` 4 (same code).
// M3 lane 2 r157 (h2m fast with 0x8c5b0/0x8c660, both MATCH each run; control 4; allocator view re-read: request and
// kind both priority 20 in the entry block, the request ahead on its earlier definition), all with the existing kind
// read: the kind arm through a payload pointer (`available ? &payload : NULL`, then an if/else on it) 50 (114 B); that
// seeded `setting = 1` 49 (111 B); `long const old_setting` 4 (same code); both handlers through one shared plain-inline
// store helper (index, setting) holding the old-setting test and the notify call, the reset still MATCH, 5; an
// `__assume(request != NULL)` at the top (diagnostic) 4 (same code).
// @retail 0x8c5f0
bool __stdcall bungienet_user_voice_apply(s_bungienet_user_request *request)
{
	short kind = *(volatile short *)&request->kind;
	long controller_index = request->controller_index;
	long setting = bungienet_user_voice_setting(kind);
	s_controller_voice_view *controller = controller_voice_view_get(controller_index);
	long old_setting = controller->voice_setting;
	bool changed = false;
	if (setting != old_setting)
		changed = true;
	controller->voice_setting = setting;
	if (changed)
		function_18fe9e(controller_index);
	return true;
}

struct s_user_settings_definition
{
 long kind;
 const wchar_t *filename;
 long version;
 s_bungienet_user_handler handlers[2];
};

s_user_settings_definition g_46729c =
{
 1, L"bungienet_user.dat", 1,
 { {0, bungienet_user_voice_reset}, {0, bungienet_user_voice_apply} }
};
s_pending_message_header g_479750 =
{
 (dword)&g_46729c, 0, 0, 1, 0, 0, NONE,
 {NONE, NONE, 0}, NONE, &g_4d8800, sizeof(g_4d8800)
};

bool pending_message_request_receive(s_pending_message_header *header, long controller,
 s_pending_message_values values, long value06);

static inline XUID *user_settings_identity(XONLINE_USER *user)
{
 XUID *result = 0;
 if (user) result = &user->xuid;
 return result;
}

static __forceinline long user_settings_next_controller(long index)
{
 long result;
 switch (index + 1)
 {
 case 0: result = 0; break;
 case 1: result = 1; break;
 case 2: result = 2; break;
 case 3: result = 3; break;
 default: result = NONE; break;
 }
 return result;
}

static __forceinline void user_settings_request(long controller, XUID *identity)
{
 if (identity && g_479750.state == 0)
 {
  g_479750.flags &= ~2;
  pending_message_request_receive(&g_479750, controller,
   *(s_pending_message_values *)identity, 0);
 }
}

// @retail 0x8c660
void function_8c660(void)
{
 XONLINE_USER users[4];
 memset(users, 0, sizeof(users));
 XONLINE_USER *signed_in = XOnlineGetLogonUsers();
 if (signed_in) memcpy(users, signed_in, sizeof(users));
 for (long controller = 0; controller != NONE; controller = user_settings_next_controller(controller))
 {
  HRESULT status = users[controller].hr;
  XUID *identity = user_settings_identity(&users[controller]);
  if (identity && identity->qwUserID && SUCCEEDED(status))
  {
   if (!XOnlineIsUserGuest(users[controller].xuid.dwUserFlags))
   {
    if (controller_voice_view_get(controller)->voice_setting == 0)
     user_settings_request(controller, user_settings_identity(&users[controller]));
   }
   else
   {
    s_controller_voice_view *slot = controller_voice_view_get(controller);
    long old_setting = slot->voice_setting;
    bool changed = false;
    if (old_setting != 1) changed = true;
    slot->voice_setting = 1;
    if (changed) function_18fe9e(controller);
   }
  }

 }
}
