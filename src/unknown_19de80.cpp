#include "unknown_11c920.h"
#include "globals.h"
#include "game_engine_events.h"
#include "engine_peer.h"
#include "language.h"
#include "unknown_0259d0.h"
#include <string.h>

// @flags /O2 /Ob1 /arch:SSE /Gr
#include "view_context_storage.h"

/* UNKNOWN_19DE80.CPP: the game engine events. An event names a cause (a
   player and a team) and an effect (a player and a team); each player is
   shown the response the multiplayer globals define for the event's type and
   subtype, chosen by how the player relates to the event (the cause player,
   the cause team, the effect player, the effect team, or anyone else). */

/* a sound tag reference, one per language */
struct s_event_sound
{
	long group;
	long index;
};

/* one random sound of a response (0x50 bytes), chosen by its weight */
struct s_event_sound_permutation
{
	struct
	{
		byte bit0 : 1;
	} flags;
	byte unknown01[3];
	s_event_sound sounds[9];
	real weight;
};

/* one response of the multiplayer globals to an event (0xa8 bytes) */
struct s_event_response
{
	struct
	{
		byte plural : 1;
	} flags;
	byte unknown01[3];
	short subtype;
	short audience;
	byte unknown08[4];
	long string_handle;
	short required;
	short excluded;
	long timer_string_id;
	long field_18_3;
	long plural_string_id;
	byte unknown20[0x3c - 0x20];
	real sound_delay;
	struct
	{
		byte bit0 : 1;
	} sound_flags;
	byte unknown41[3];
	s_event_sound sounds[9];
	byte unknown8c[0xa0 - 0x8c];
	long permutation_count;
	s_event_sound_permutation *permutations;
};

struct s_event_response_block
{
	long count;
	s_event_response *responses;
};

/* the multiplayer globals' runtime data: the event strings and the responses
   by event type */
struct s_event_globals
{
	byte unknown00[0x8c];
	long strings;
	byte unknown90[0x98 - 0x90];
	s_event_response_block blocks[11];
};

/* the multiplayer globals' universal data: the team names */
struct s_event_universal_globals
{
	byte unknown00[0xc];
	long team_names;
};

struct s_event_globals_definition
{
	long universal_count;
	s_event_universal_globals *universal;
	long runtime_count;
	s_event_globals *globals;
};

/* the iterator over the players of 0x19f240 */
struct s_player_iterator
{
	s_event_player *player;
	s_record_pool *data;
	long index;
	long absolute_index;
};

bool player_iterator_next(long *iterator);
long function_19e1c0(s_event *event, long token_length, word const *token, word *destination, long remaining);
void function_19e890(long player_index, s_event_response *response, s_event *event);
void function_19ea60(long player_index, word *buffer);
void function_19eae0(long team, word *buffer);

void function_1a0180(long tag_index, long string_handle, word *buffer);
long function_19fd00(long index);
int unicode_string_vsnprintf(word *buffer, long maximum_count, const word *format, ...);
word *function_1630e0(word *buffer, const word *format, ...);
long function_14de70(long local_player_index);
bool function_15eaf0();
bool function_161e10(long team);
long function_1587b0(long player_index);
long function_1587f0(long team);
void function_159130(long score, word *buffer);
void game_engine_format_time(long seconds, word *text);
void function_22d2ee(word *string, long maximum_length);
void function_23ef80(long sound_index, long delay, s_event *event, bool flag);
void function_24caac(long player_index, word const *text, word const *plural_text, long count);
void function_24cbee(long player_index, word const *text);
void function_24cc73(long player_index, word const *text, long value);


inline long game_seconds_to_ticks_round(real seconds)
{
	real ticks_real = (real)g_510c54->field_2_3 * seconds;
	long ticks;
	__asm
	{
		fld ticks_real
		fistp ticks
	}
	return ticks;
}


// @retail 0x19df10
s_event_response *function_19df10(s_event *event, long player_index, long audience)
{
	s_event_response *result = 0;
	s_event_globals *globals = ((s_event_globals_definition *)g_4e3b44[g_4e034c->index & 0xffff].bytes)->globals;
	s_event_response_block *block = 0;

	switch (event->type)
	{
	case 0:
		block = &globals->blocks[0];
		break;
	case 2:
		block = &globals->blocks[2];
		break;
	case 3:
		block = &globals->blocks[3];
		break;
	case 4:
		block = &globals->blocks[4];
		break;
	case 6:
		block = &globals->blocks[6];
		break;
	case 1:
		block = &globals->blocks[1];
		break;
	case 8:
		block = &globals->blocks[8];
		break;
	case 9:
		block = &globals->blocks[9];
		break;
	case 10:
		block = &globals->blocks[10];
		break;
	}

	for (long i = 0; i < block->count; i++)
	{
		s_event_response *response = &block->responses[i];

		if (response->subtype == event->subtype && response->audience == audience)
		{
			bool valid = true;
			bool match;

			switch (response->required)
			{
			case 1:
				valid = event->cause_player_index != NONE;
				break;
			case 2:
				valid = event->cause_team != NONE;
				break;
			case 3:
				valid = event->effect_player_index != NONE;
				break;
			case 4:
				valid = event->effect_team != NONE;
				break;
			}

			switch (response->excluded)
			{
			case 1:
				match = valid && event->cause_player_index != player_index;
				break;
			case 2:
				match = valid && event->cause_team != event_player_get(player_index)->team;
				break;
			case 3:
				match = valid && event->effect_player_index != player_index;
				break;
			case 4:
				match = valid && event->effect_team != event_player_get(player_index)->team;
				break;
			default:
				match = valid;
				break;
			}

			if (match)
			{
				result = response;
				break;
			}
		}
	}

	return result;
}

// @retail 0x19de80
void function_19de80(s_event *event, long player_index)
{
	s_event_player *player = event_player_get(player_index);
	s_event_response *response;

	if (player_index == event->cause_player_index && (response = function_19df10(event, player_index, 0)) != 0 ||
		player_index == event->effect_player_index && (response = function_19df10(event, player_index, 2)) != 0 ||
		player->team == event->cause_team && (response = function_19df10(event, player_index, 1)) != 0 ||
		player->team == event->effect_team && (response = function_19df10(event, player_index, 3)) != 0 ||
		(response = function_19df10(event, player_index, 4)) != 0)
	{
		function_19e890(player_index, response, event);
	}
}

/* copies the source into the buffer, replacing each token (a '#' and the
   lower-case letters and underscores after it) by its text for the event */
// @retail 0x19e0f0
void function_19e0f0(word const *string, long size, word *buffer, s_event *event)
{
	word const *source = string;
	word *destination = buffer;
	long remaining = size - 1;
	word const *token;
	word const *end;

	do
	{
		token = wcschr(source, '#');
		if (token)
		{
			long prefix_length = token - source;
			long token_length;
			long written;

			for (end = token + 1; *end >= 'a' && *end <= 'z' || *end == '_'; end++)
				;
			token_length = end - token;

			if (prefix_length > remaining)
				prefix_length = remaining;
			wcsncpy(destination, source, prefix_length);
			destination += prefix_length;
			remaining -= prefix_length;

			long (__fastcall *const expand_token)(s_event *, long, word const *, word *, long) = function_19e1c0;
			written = expand_token(event, token_length, token, destination, remaining);
			destination += written;
			remaining -= written;
			source = end;
			if (!written)
				source++;
		}
	} while (token);

	{
		long length = wcslen(source);
		wcsncpy(destination, source, length <= remaining ? length : remaining);
		destination[length] = 0;
	}
}

/* whether the current game engine plays with teams */
static inline bool teams_enabled_19e1c0()
{
	bool result = false;

	if (g_55e4d0[g_4e9ae8->engine_index])
		result = TEST_FIELD_BIT(g_4e6948->flags184.bit0);
	return result;
}

/* writes the text of one token for the event, returning its length */
// MiniMax b (11-11): no new form, no lower score (390); our frame is still 0x208 against retail's 0x204, the
// volatile result keeps our four name and team copy tails apart where retail keeps a copy of the length and
// clamp in each case, and retail's prologue forms the first token compare with the string, the stack argument
// and the token in the other registers.
// MiniMax G (11-12): no new form, no lower score (390): the volatile result still keeps our four copy tails apart but frames 0x208 against retail's 0x204, and a plain result reaches that frame and retail's ESI result register only by merging the four tails into one block (691 this round); the permuter's 319 seconds over the body found nothing below its 385 baseline.
// score 390 (was 390): retail clamps the copy length with `cmp remaining, length` and a conditional
// move, so the comparison written as `remaining <= result` was tried: with the volatile result 466,
// with a plain result 652 (against 390 and 691). The plain result does match retail's frame 0x204
// and its ESI result register, but it merges the four name/team copy tails into one block, where
// retail keeps a copy of the length and clamp in each case and shares only the copy itself.
// MiniMax E (11-05): no new form, no lower score.
// MiniMax h (11-04): no new form, no lower score; the volatile result keeps our case tails apart and our frame is 0x208 against retail's 0x204.
// Earlier: this round confirmed the blocker is the frame: the `volatile` result keeps the
// frame at 0x208 where retail has 0x204, and a plain result merges the four name/team copy tails
// (701); the team loops and the token/remaining register homes still differ.
// Earlier: the switch-side-time branch now returns the remaining length directly,
// the team branches keep the event's team in a local; frame 0x208 versus 0x204 and the copy tails differ.
// Depth one with plain result 709; depth one with original result 443; unsigned mode 443;
// opponent eligibility continue 443; opponent text initialized after scan 459; all restored.
// Plain result with global optimisation off 1225; size mode 863; comparison-fed
// terminators 413 (discarded); size mode with existing result 834; global
// optimisation off with existing result 1225; comparison-fed plain result 709;
// first-name comparison terminator 444; score-text comparison terminator 443.
// Word opponent iterator 433 kept; explicit modes 443; native local team 457; native eligibility 443; shared
// player dispatch 593; word iterator plus modes/local team/eligibility 440/451/433.
// Team-test byte/short/long locals with bool returns 433/483/483; native
// byte/short/long returns 433/430/432; unsigned-word/byte opponent counters 433/433.
// Ordinary short/long return forms 430/432; both small gains discarded.
// Round 166: mode observations 433; engine result exit 433; fallback test result 435; mode with
// exit/tests 433/435; native result with exit/tests/mode exit 704/703/704.
// Player-name/team-name/score-format/time-format inline and noinline declarations
// 433 each (eight fast forms); all declarations and the original body restored.
// Immutable capacity/comparison length/both 433/433/433; ordinary result with both 701;
// valid-team-first/inherited and ordinary result 441/729;
// word/native exact-end team loop 433/444.
// Candidate score reuse 445; existing score formatter 492; both 502; their ordinary-result
// forms 708/794/798; original two-call conditional maximum 433, with ordinary result 701.
// Private player/team/all-name/all-text buffers 434/433/412/433; the 412 form
// adds 512 frame bytes and is rejected. Ordinary result with those scopes 692/692/686/701.
// Name event-first/player complete guards/all event-first/all complete event guards
// 475/460/430/480; ordinary result with the same four dispatch forms 716/680/718/697. The
// three-point 430 form is ineligible; all restored.
// Round 150: Shared admission, shared engine getter, forced existing admission and native admission inputs 433 each; ordinary result on those four graphs 701 each. Original helper/include/result restored.
// Round 353: target inline/noinline declarations 433/433; ordinary result
// 701/701; forced admission helper 433/433; both 701/701.
// Target metadata and original result/helper restored; matched caller preserved.
// Round 146: plain result 701 (frame 0x204 and the ESI result agree, but the player/team copy
// tails merge from the name call on, so `remaining` loses its register to `token`; retail
// keeps token_length in EDI and remaining in EBP and reloads token); with token read through
// its address 701 (identical code); clamp as >=, remaining <, remaining <= 433 each, with the
// plain result 701/701.
// Round 367: Inline length/copy/comparison/variadic-format declarations 433 each; ordinary result on the
// same four boundaries 701 each. All declarations, result storage and copy/format paths restored.
// Round 224: ordinary result completed on every no-output path: all failure tails 715;
// name/score/name+score entry defaults 713/707/692; timer/name+timer/score+timer/all entry
// defaults 716/712/712/710 (fast). Frame 0x204 recovers, copy tails still merge; all restored.
// Round 66: ordinary result with element-address name buffers, a shared name pointer,
// result-derived first terminator, a first-case pointer, a first-case exit, their combination,
// an independent first-case length and destination element addresses 701 each; all restored.
// This round: the team's local as short/long/int 421/403 (the long stays); the team loops as
// `!=`, `<=`, a while, the body in the if and a single score compare 421/403/390/390/390; the
// result local plain, without its initialiser, and with the text buffer first 701/665/433; the
// volatile result without its initialiser 449; every form restored.
// Round 272: entry declarations for cause/effect/both teams and both with opponent score,
// inherited/ordinary result 390/691/390/691/390/691/390/691. Original lifetimes restored.
// Round 393: Length/copy/comparison/format noinline declarations with inherited result 390/390/390/390;
// ordinary result 691/691/691/691. All declarations and original case bodies restored.
// Round 201 (fast): plain result re-measured 691 (1170 bytes); with the event's team field read at both
// uses instead of the team local 661 (1202 bytes; the event now in EBX as retail, but token_length in EBP
// and token in EDI where retail keeps remaining in EBP and reloads token); that with the clamp as
// `remaining <= result` 661; that with the side-time failure as `result = remaining` 701; the volatile
// result with the field reads 394. Retail's first case stores text[0] from SI (the zero result) before
// its `lea esi`, so its two player cases differ and are not merged. Team locals and result restored.
// Round 395: consumed name capacity with result after/before copy, result-fed capacity, and direct capacity exits
// 618/649/519/621; ordinary result with those four capacity lifetimes 692/692/692/692. All restored.
// Round 409: native cause/effect/both team-field addresses 390/390/394; formatter outcomes 414;
// with cause/effect/both 411/411/414 (411 is retail length); all with ordinary final result 713.
// Original result, team values, complete copy/format paths and body restored.
// Round 415: token-first zero-only player/team/other comparisons in seven subsets
// 391/390/391/391/392/391/392; complete comparison graph with ordinary result 692.
// Original comparison operands, result storage and copy/format tails restored.
// Round 218: Result/text/maximum lexical names, seven subsets, 390 each; complete opponent-scan
// role names 390. Frame 0x208 and copy/result homes remain; all original identities and body
// restored.
// Round 42: complete signed copy-count lifetimes with final result after copy,
// player names/team names/fallback token 460/484/568; player+team/player+fallback/
// team+fallback 620/450/457; all counts 674; all with ordinary final result 691.
// Original result storage and complete body restored; frame 0x208 versus 0x204,
// capacity/token/result allocation and separate name-copy tails remain.
// Round 181 (lane 3): the plain result keeps the token in EDI and leaves `remaining` on the stack
// (retail: remaining EBP, token_length EDI, event EBX, token reloaded per test), which is what lets
// the name/team tails merge. Plain result with the switch-side-time branch clamping into the result
// 732 (1129 bytes); the volatile result with that clamp 431 (1398 bytes); the plain result with
// every token test read through the parameter's address 691 (identical to the plain result); the
// event teams read at each use 394; a long opponent counter 421.
// Lane 18 round 6: retail's 4 frame bytes past the text buffer hold the inlined teams test's dead byte store (ours
// makes it too), so the volatile result is the extra slot. A plain result with one shared inline copy helper
// (length, clamp, copy; four callers) 691 (same code as the plain result); the opponent loop's team as long 421
// (team in EDI passed in ECX as retail, 1424 bytes); with `best_score <= function_1587f0(team)` 421 (loop body
// now retail's, missing only its `xor edi, edi` and padding); both with the plain result 688; and with the text
// buffer declared before the plain result 688. Original body restored.
// Round 466: Event/token-plus-length/output/all-four formal role names: 390 each;
// the same four with ordinary result storage 691 each, frame 0x204 but merged copy tails.
// Original formals, result storage, dispatch and copy/format paths restored;
// frame 0x208 versus 0x204 and independent name-copy/result/capacity homes remain.
// Lane 18 round 15 (fast): retail re-reads `token` from the stack at every compare, and that extra scratch temp
// per case shifts the EAX/ECX/EDX rotation, so the four name/team tails differ and only the copy call is shared;
// ours keeps `token` in a register and the plain result's tails merge. Plain result 691; with one read of
// `token` through its own address 691 (folded); plain result with the event's teams read at each use 661 (1202
// bytes); with the clamp as `remaining <= result` 691 (a listed form, now 691 not 652), both 661; volatile result
// with the teams at each use 394 (listed, re-measured); on the plain result with the teams at each use: the switch-
// side-time case setting `result = remaining` like the others 701 (1161 bytes), the clamps as the caller 0x19e0f0's
// `result <= remaining ? result : remaining` 661.
// Diagnostic (a volatile token read at every compare, reverted): 468, with retail's token_length EDI and per-test
// token reloads, but a hoisted zero then takes EBX (event to EBP) and `remaining` still stays on the stack.
// Round 54 (m3 lane 1, fast): a named length with retail's clamp shape `result = remaining <= length ? remaining :
// length` (cmp remaining, length; cmovle) in the four name/team cases: plain result 692, volatile result 412 (1444
// bytes); with the fallback clamp in the same shape, plain 694; every format failure as `return remaining;`
// (as the switch-side-time case): plain 696 (1167 bytes), volatile 439 (1416 bytes). With the plain result our
// allocator gives `token` EDI and leaves `remaining` on the stack (retail: remaining EBP, token reloaded per
// compare), so the name/team tails merge whatever the clamp's form.
// Round 66 (lane 9, fast): every vsnprintf failure as `return remaining;` with the volatile/plain result 439/696;
// each case its own `if` ending in `return result;` (no else chain) volatile/plain 436/630 (the plain form keeps
// token EDI, token_length EBX, event EBP). Diagnostic: the plain result with `|| result > remaining` added to every
// failure test (seven more uses of `remaining`) 688 with the same registers, so use counts do not decide it.
// Round 497: Complete 512-byte text initialization, entry aggregate/CRT and branch CRT/aggregate:
// inherited result 526/526/452/549; ordinary result 614/619/529/471. All regress; original buffer,
// result and body restored. Frame and copy tails remain.
// Round 46 (lane 14, fast): retail's per-compare `token` reloads take a scratch register each, so the case
// bodies' EAX/ECX/EDX rotation differs and only the copy and format calls are shared (0x19e74c, 0x19e3b8).
// Plain result 691 (control); with a `length` local and `length > remaining ? remaining : length` in the four
// name/team cases 692, as `remaining < length ? remaining : length` 692; the length local with the volatile
// result 412 (1444 bytes); plain result with the token fallback clamp as a ternary 693, with both 694.
// Round 231: case-owned copy/formatter/replacement results and their paired families
// 573/416/461/516/631/588; complete case results with original/remaining-first
// native copy clamps 540/540. Formatter-only gives retail length at 416; original
// result ownership, all cases, text buffer, loop and full body restored.
// MiniMax I (11-01): no new form, no lower score; the plain result still merges the four name/team copy tails (691)
// and a permuter run over it (27 tries in 301 s) produced only mechanical shuffles, none below the baseline.
// Round 118: plain result with every clamp as `result = result > remaining ? remaining : result;` 691 (1170 bytes);
// the same ternary clamps on the volatile result 415 (1446 bytes); the text buffer declared before a plain result
// with the clamps as `remaining < result` 691; token_length unsigned (definition, declaration and the caller's
// pointer type; 0x19e0f0 still matches) 390; plain result with the engine's token text in its own `written`
// local 691. Re-measured listed forms: long team with `best_score <=` 421, event teams read at each use 394.
// Retail never gives `token` a register (re-read from its home before every compare, the first one hoisted
// above `sub esp`), which is what keeps the four name/team tails apart.
// Round 304: unsigned library copy counts published before copying, player/team/both/fallback/
// player-fallback/team-fallback/all 499/511/649/401/508/532/647; all with ordinary result 693.
// Original result, signed clamps, copy paths and complete body restored; frame and tail merging remain.
// Round 449: Complete player/team/other token dispatch results, bool subsets 1..7
// 454/454/454/454/454/454/454; all with ordinary final result 736 (fast control 390, full 390).
// Original complete executable source restored; opening structural differences remain.
// Round 451: Separate positive/rejected maximum-score gates with short/long opponent
// counter and inherited/ordinary result storage: 388/388/421/421/691/691/688/688. Best two-point gain rejected; original
// scan, result and full body restored; frame and independent name-copy tails remain.
// Round 456: Ordinary result with bounded player/team name-copy contracts
// memcpy/original, memmove/original, original/memcpy, original/memmove,
// memcpy/memcpy, memcpy/memmove, memmove/memcpy, memmove/memmove:
// 636/553/632/518/670/502/448/441. Original result, wide copies and complete body restored; frame and separate copy tails remain.
// Round 458: Bounded word player/team/all-name and short all-name length producers,
// each with inherited/ordinary final result storage: 441/442/428/428/651/651/691/691.
// Original full-width producers, signed clamps, result storage and complete body
// restored; frame 0x208 versus 0x204 and independent copy tails remain.
// Round 77 (m3 lane 12, fast control 390): `remaining` copied into a capacity local used everywhere 390 (same
// code); `token` copied into a name local 390; capacity local with the plain result 691; plain result with a long
// opponent team and `best_score <=` 688; volatile result with those two and every format failure returning
// `remaining` 443. Compile-only diagnostics (the no-LTCG build reproduces ours: plain result gives `token` EBP and
// leaves `remaining` on the stack): the copies coalesce with the parameters; seven fewer `token` uses keep it in
// EBP; a `remaining` test added to each comparison (not a form, use-count probe) flips it to retail's homes, so
// retail's body uses `remaining` more, or `token` less, than ours. Original body restored.
// Round 464: repeated event input snapshots for cause/effect/both players and all player/team
// values before pure token comparisons: inherited result 418/433/441/452, ordinary result
// 632/632/643/574. All regress; original observations, result storage and full body restored.
// Existing admission result as conjunction/early failure/complete arms/conditional:
// inherited result 454/390/414/414; ordinary final result 736/691/698/698.
// Round 482: One shared final result read after independent dispatch prefixes of
// 1/2/4/6/8/10/12 arms, then every arm with ordinary result storage: 390/390/390/458/451/436/390/691.
// Original chain and result restored; frame/capacity homes and separate copy tails remain.
// AC489: First-word-only text initialization at entry for player/team/formatted families,
// subsets 1/2/4/3/5/6/7: 425/427/435/437/451/453/467; team-only with ordinary result 609.
// Original buffer, result and full body restored; frame/capacity homes and separate copy tails remain.
// Round 38 (m3 lane 18, fast): the twelve token compares through one inline predicate (token, length, name)
// 390, with the plain result 691 (both identical code); a block-scoped text buffer in each of the nine cases (one
// shared slot, frame unchanged) 390, plain 691 (identical); an inline copy helper (length, clamp, copy; four callers)
// with the volatile result 620 (1237 bytes). The plain result still merges even the name-call setup across cases.
// Round 79 (m3 lane 16, fast): plain result with the fallback as `if (result) return result;` 693; plain result
// with the local-team value long 693; plain result with switch-side-time `result = remaining` plus the early
// fallback return 734; long opponent team with `best_score <=` and both event teams read at each use, plain/volatile
// 667/404; a MIN(a, b) macro clamp (`a > b ? b : a`) in the four name/team cases 467 plain, also in the fallback 462
// plain, 493 volatile: the tails stop merging, but wcslen is called twice and compared unsigned (ja), not retail's
// one call and cmovle; retail's caller sets ECX then EDX (fastcall, /Gr), so no parameter reorder applies.
// Round 35 (m3 lane 17, fast): plain result with token_length unsigned (definition, declaration, caller's pointer)
// 691; plain result with `return result;` closing every copy arm 691 (same code), the volatile result with those
// returns 390 (same code); the engine's token text in a `written` local returned when nonzero, volatile/plain
// result 397/693. Retail: event EBX, token_length EDI, remaining EBP, result ESI from the top; the opponent loop
// runs on `cmp edi, 8` (no free register for a down counter), where ours counts EBX down from 8.
// Round 67 (m3 lane 10, fast): each case its own `if` ending in `return result;` with the plain result 630 (lane 9's
// form re-measured, 1232 bytes: every return gets its own epilogue where retail jumps to the shared copy tail), with
// every format failure returning `remaining` 623, with a long opponent team and `best_score <=` 625; on the volatile
// result: the time-mode test as `mode == 3 || mode == 4 || mode == 8` 397, the token fallback clamp as one ternary 395
// (1432 bytes), the mode field read at each use with no local 390 (same code).
// Round 143 (m3 lane 5, fast): every clamp as `if (remaining <= result)` 390 (same code); text
// declared before the result 390 (same code); `best_score <= function_1587f0(team)` with the short
// team 388; local-team value long 393 (1436 bytes); engine text returned early 390 (same code).
// AC512: Name-length/formatter/replacement results acquired in their consuming
// tests, seven family subsets 390 each; complete ordinary result 691. All restored.
// NOT kept: embedded assignments remain unmatched; native frame/capacity/result
// homes and separate name-copy tails remain.
// M3 lane 14 r59 (fast): on the plain result with the event teams read at each use and `remaining <=` clamps (661
// re-measured: event EBX and result ESI as retail, token EDI and token_length EBP where retail has token_length EDI
// and remaining EBP): every format failure as `return remaining;` 665 (1199 bytes); each case its own `if` closing on
// `return result;` (no else chain) 581 (1280 bytes), plus the failure returns 557 (1306 bytes; homes turn over again:
// token EBX, token_length ESI, event EBP, result EDI), with the text buffer declared first 581 (same code); the
// volatile result with the per-case returns and the teams at each use 429 (1427 bytes). Diagnostic (reverted): one
// `remaining &&` added to the second compare on the plain result 655 in the real build (homes unchanged), although
// a compile-only build (no LTCG) of the same source flips to retail's homes; that harness misleads here.
// AC526: Complete signed formatter-failure truths bool/byte/short/long, inherited
// result then native result: 418/418/427/424/691/691/691/691. NOT kept: native frame alone still
// merges name-copy tails; original full result, clamps, cases and body restored.
// F372: Entry declaration lifetimes for the opponent iterator, mode and local-team
// value, seven subsets then all with the actual local-player receiver: eight at 390.
// Original branch declarations, result storage and complete body restored.
// AC537: Complete absent-team eligibility at both name branches, bool/byte/short/long
// with inherited/native result: 390/390/390/390/691/691/691/691. NOT kept: no match; original admission,
// result qualifier, all name-copy/format paths and complete body restored.
// M3 lane 4 r167 (fast; one-file link with the callee files and ballast 413/693 for 390/691): `remaining` takes a register only
// when something reads it in the entry block, before the first compare. A plain result with one read of it there (a store to a
// probe global, or a volatile read into a local used everywhere; diagnostics, reverted) keeps the name/team tails apart, stores
// text[0] from SI, reloads token per compare and loads EBP at entry as retail: 455/444 (1472/1466 bytes), with team fields at
// each use, `remaining <= length` clamps, long opponent team 437 (1458 bytes, event EDI spilled where retail has EBX). Sixteen
// reads inside the first arm, reads in the first two arms, two at the function's end: no flip (534-684). Code-free reads at
// entry all fold before allocation: empty if, void ternary, unused local, += 0, `__assume(remaining > 0)`, a bound const
// reference through a conversion, a read through its address 691 each; a dead for 586 (1274 bytes). The natural entry
// reference is still unknown; it is the lead.
// AC548: Shared event-presence lifetime, bool/byte/short/long with inherited/ordinary result
// 454/454/446/441/671/671/630/626. NOT kept: no qualifying supported gain or byte match.
// Native frame/result/capacity/token homes and independent copy tails remain. Original complete body restored.
// M3 lane 16 r91 (fast; aligned unmatched in brackets, control 390 [214]): the teams test as one `&&` expression (no byte
// local) with the volatile result 454 (1394 B, frame 0x204 and text at retail's 0x14, but the code shortens) [192], with the
// plain result 736 (1128 B) [231]; the teams test as an early `return false` 390 (same code); switch-side-time failure as
// `result = remaining` on the volatile result 431 (1398 B, the round 181 code); a named player pointer for the local
// team's score case 390 (same code). Retail's local team read is `add eax, edx; mov al, [eax + 0xc0]` where ours folds
// the sum.
// M3 lane 12 r93 (fast): the twelve compares through an inline predicate taking `token` by const reference 390
// (identical: the reference folds, token stays in EDI), with the plain result 691 (identical to the plain result);
// the volatile result declared bare and set by a statement 390; plain result with both event teams read at each use
// and every copy clamp as `remaining <= result` 661 (listed combination re-measured); the teams test returned as one
// conjunction (no bool local) 454 (1394 bytes; listed as the conjunction, re-measured).
// Every format failure as `result = result < 0 ? remaining : result;` 514 (1524 bytes); the opponent score's
// number/time arms swapped (`mode < 3 || mode > 4 && mode != 8` takes the number format) 390 (same code).
// b2N (11-06) statement walk against retail 0x19e1c0-0x19e760, no source change: the twelve token literals
// (0x454898..0x454708, in order), the event field offsets (+0xc/+0x14 players, +0x10/+0x18 teams, type +0, subtype +4),
// the type==0 && (subtype 0x2d || 0x17) || team != NONE admission of both team-name arms, the opponent loop (0..7,
// team != cause_team, best <= score, INT_MIN start), the mode >= 3 && (<= 4 || == 8) time format, the player fields
// (+0xc0 team byte, +0x170 respawn dword, +0x1ac lives word, index masked 0xffff), the switch-side-time word at +0xe0 and the
// engine vtable slot 0xc8 (token, token_length, event, destination, remaining) with its token-copy fallback all agree
// with retail; no behaviour difference found. Only the `long volatile result` is unexplained by behaviour: it is
// there for the frame/ESI shape (390 against 691 plain) and would be undone in review unless the function matches.
// Round 63 (m3 lane 2, fast; control 390): plain result with the name/team clamps folded into the wcsncpy count
// (an embedded assignment, measured only) 691; plain result with `event != NULL` 691 (same code); the volatile
// result with the text buffer declared first and the switch-side-time failure as `result = remaining` 431
// (1398 B); re-measured listed forms: the fallback clamp as `remaining <= token_length ? remaining : token_length`
// on the volatile/plain result 395/693; plain result with every compare as `wcsncmp(...) == 0` 691; both team
// admissions with the team test first 401 (1438 B). Retail cross-jumps whole case tails (the local-team case enters the
// cause-team case's 0x1587f0 call with the team in EDI, the formatted cases share one vsnprintf/failure tail).
// M3 lane 8 r150 (fast; aligned 301, register-masked 199 on this body; retail reads the token into EAX ahead of the
// frame setup and from its slot at every later compare, and keeps remaining in EBP from entry): the four name/team
// copies through a shared four-caller copy helper (length, clamp, wcsncpy) 691 on the plain result (1170 bytes: the
// copies merge into one tail), 620 on the volatile result (1237 bytes), the same as forceinline; the switch-side-time
// failure as `result = remaining` on the plain result 732 (1129 bytes, aligned 288; the listed volatile-result form,
// re-measured on the plain result); the clamps as `remaining < result`
// on the plain result 691 (same code as the plain result); the four clamps as `result > remaining ? remaining : result`
// on the volatile result 415 (1446 bytes); the text buffer declared ahead of the volatile result 390 (same code).
// Round 84 (m3 lane 11, fast, control 390): every length clamp (four name/team cases and the token fallback) through
// an inline (length, capacity) function returning `capacity <= length ? capacity : length` (one wcslen, retail's
// cmp/cmovle shape): plain result 694 (1168 bytes, tails still merge), the volatile result 414 (1439 bytes); the
// function's parameters swapped (capacity, length) with the plain result 694; as `length < capacity ? length :
// capacity` plain 694, volatile 414. Retail's own tails: the result is a plain ESI variable (a dead `mov esi, eax`
// after the engine call at 0x19e726), `remaining` in EBP and the token reloaded into a scratch register (or ESI in
// the last compare, 0x19e6c1) for every compare.
// M3 lane 16 r100 (fast, 0x19e0f0 stays matched): our volatile-result build makes retail's local-team and spawn/lives joins; the
// switch-side-time case keeps its own `return remaining` tail, and both team-name cases join the fallback at its copy call (retail
// joins the cause-team case at the fallback's clamp). The plain result re-measured 691 (1170 B: token in EDI, token length in EBX,
// event in EBP, `remaining` on the stack); the fallback's engine through a `c_engine_peer *engine` local 390 / plain 691 (same code
// each); the format-failure ternary re-measured 514 / plain 691 (same code); a named `s_event_player *player` in the spawn-time and
// lives cases 387 (1443 B, retail's length), in the spawn-time case only 389 (1439 B; retail's spawn-time case has the pointer's dead
// `add eax, ebx`, its lives case does not); the cause-team score read into a named local before 0x159130 390 (same code). Body
// unchanged (gains under 4 are not kept).
// Round 73 (m3 lane 15, fast with 0x19e0f0): `long const capacity = remaining;` at entry used for every later read, with the
// plain/volatile result 691/390 (both fold to the existing code).
// MiniMax E (11-11): no new form tried, no lower score (390): the volatile result keeps our four name and
// team copy tails apart and our frame at 0x208 against retail's 0x204, while the plain result matches
// retail's frame but merges the tails; retail clamps the copy length with `cmp remaining, length` and a
// conditional move and keeps a copy of the length and the clamp in each case, so the clamp's own local is
// what is left; the forms listed above all score the same or worse, so the body stands.
// MiniMax E (11-05): no new form, no lower score.
// MiniMax h (11-04): no new form, no lower score; the volatile result keeps our case tails apart and our frame is 0x208 against retail's 0x204.
// M3 lane 1 r89 (fast with 0x19e0f0, which stays matched; control 390, aligned 197, masked 125; the two MiniMax lines above
// moved down so the score line leads): in the opponent scan the team-index test before the eligibility call 395; the score
// test nested inside the eligibility test 390 (same code); the local-player index as `g_4b9ed8 != NONE ? entries[...] : NONE`
// with one test 390 (same code); the opponent score read once per team into a `score` local 403 (1432 B); `best_score <=`
// with the long local-team value (two listed parts together) 391. Re-measured listed forms: long opponent team 421, mode read
// at each use 390, long local-team value 393, team test first in both team admissions 401, clamps as `remaining < result`
// 390, `best_score <= function_1587f0(team)` 388 (two points, under the bar).
// M3 lane 12 r110 (fast, 0x19e0f0 kept): const-qualified parameters on the plain result (control 691, 1170 bytes):
// `long const remaining` 691, `word const *const token` 691, `long const token_length` 691, every parameter const 691;
// every parameter const on the volatile result 390 (same code each: qualifiers do not move `remaining` into a register).
// The five `%s` format tails through one inline helper (vsnprintf, failure gives `remaining`; five callers): volatile
// result 465 (1487 bytes), plain result 594 (1269 bytes, aligned 202 against the volatile body's 214, masked 139);
// with the four name copies through a second inline helper (length, clamp, wcsncpy) plain 594 (same code), volatile 511.
// Constraint left: the remaining levers are the plain result (merges the name tails while `remaining` stays on the
// stack) and a natural entry reference to `remaining` (lane 4 r167); no untried natural form reaches either.
// M3 lane 13 r82 (fast with 0x19e0f0, which stays matched; control 390, aligned 371, masked 125): the named player pointer
// in the spawn-time and lives cases (listed 387) with `best_score <= function_1587f0(team)` (listed 388) together 385 at
// retail's 1443 bytes (aligned 309, masked 116; not kept: 5 points, one-use pointer); with a named player pointer in the
// local-team case too 385 (same code); with a `long` opponent counter 415; with both event teams at each use 387; those
// two on the plain result 697/667; the team admissions with the subtype in a named local 390 -> 397. Retail's opponent
// loop counts up (`inc edi; cmp edi, 8`); ours counts down in the free EBP, which retail gives to `remaining`.
// MiniMax A (11-11, second pass): no new form, no lower score (390); what still differs is unchanged: our build still merges the
// identical name-case tails that retail keeps apart, and its opponent loop still counts down in the free EBP that retail gives to the
// remaining count, so the per-case registers and the tail layout stay different (1443 bytes is not reached).
// MiniMax A (11-11): no new form, no lower score (390); our build still merges the identical name-case tails that retail keeps apart, and its opponent loop still counts down in the free EBP that retail gives to the remaining count, so the per-case registers and the tail layout stay different (1443 bytes is not reached).
// M3 lane 16 r114 (fast with 0x19e0f0; control 390, plain result 691): the body as a forced-inline function called from a thin
// 0x19e1c0, in the engine's parameter order or this one, plain result 691 (same code as the plain result), volatile result 390
// (same code); 0x19e0f0 calling 0x19e1c0 directly (no function-pointer local) with the plain/volatile result 663/449 (LTCG then
// takes token_length in EAX and leaves event on the stack; 0x19e0f0 10/9), so the pointer local's standard convention is retail's
// (retail holds no reference to 0x19e1c0 and no direct-call cycle runs through it); 0x19ea60 through an inline copy-and-terminate
// helper, plain or forced, and `inline` on 0x19ea60/0x19eae0: both still match and 0x19e1c0 is 390 (same code: ours still reuses
// ESI for the text address after the name calls where retail recomputes the lea). Diagnostics (a store of `remaining` to a probe
// global, reverted): at the function's top 455 (lane 4 r167's result; `remaining` EBP, but event EDI and spilled, token_length EBX,
// frame 0x20c); in the second compare's condition 588, in the first arm 621, before the return 687, in the last compare's condition
// 687, one in each of 2/4/5/9 arms 642/671/445/421: the homes never move. No count of reads in the arms gives `remaining` a
// register; only a read in the entry block does, and retail's entry block shows none.
// T106: Actual entry-owned capacity consumed by name/formatter/engine families,
// seven ordinary-result graphs then complete inherited-result graph:
// 458/701/458/701/458/701/458/434. Name forms recover token preload and
// early capacity EBP but retain frame 0x208 and merge name tails. NOT kept:
// no native frame/result/copy-tail equality; original full body restored.
// @retail 0x19e1c0
long function_19e1c0(s_event *event, long token_length, word const *token, word *destination, long remaining)
{
	long volatile result = 0;
	word text[0x100];

	if (!wcsncmp(L"#cause_player", token, token_length) && event && event->cause_player_index != NONE)
	{
		text[0] = 0;
		function_19ea60(event->cause_player_index, text);
		result = wcslen(text);
		if (result > remaining)
			result = remaining;
		wcsncpy(destination, text, result);
	}
	else if (!wcsncmp(L"#effect_player", token, token_length) && event && event->effect_player_index != NONE)
	{
		text[0] = 0;
		function_19ea60(event->effect_player_index, text);
		result = wcslen(text);
		if (result > remaining)
			result = remaining;
		wcsncpy(destination, text, result);
	}
	else if (!wcsncmp(L"#cause_team", token, token_length) && event)
	{
		long cause_team = event->cause_team;
		if (event->type == 0 && (event->subtype == 0x2d || event->subtype == 0x17) || cause_team != NONE)
		{
			text[0] = 0;
			function_19eae0(cause_team, text);
			result = wcslen(text);
			if (result > remaining)
				result = remaining;
			wcsncpy(destination, text, result);
		}
	}
	else if (!wcsncmp(L"#effect_team", token, token_length) && event)
	{
		long effect_team = event->effect_team;
		if (event->type == 0 && (event->subtype == 0x2d || event->subtype == 0x17) || effect_team != NONE)
		{
			text[0] = 0;
			function_19eae0(effect_team, text);
			result = wcslen(text);
			if (result > remaining)
				result = remaining;
			wcsncpy(destination, text, result);
		}
	}
	else if (!wcsncmp(L"#cause_team_score", token, token_length) && event && event->cause_team != NONE)
	{
		text[0] = 0;
		function_159130(function_1587f0(event->cause_team), text);
		result = unicode_string_vsnprintf(destination, remaining, L"%s", text);
		if (result < 0)
			result = remaining;
	}
	else if (!wcsncmp(L"#cause_team_opponent_score", token, token_length) && teams_enabled_19e1c0())
	{
		long best_score;
		short team;
		long mode;

		text[0] = 0;
		best_score = 0x80000000;
		for (team = 0; team < 8; team++)
		{
			if (function_161e10(team) && team != event->cause_team && function_1587f0(team) >= best_score)
				best_score = function_1587f0(team);
		}

		mode = g_4e6948->mode_180;
		if (mode >= 3 && (mode <= 4 || mode == 8))
			game_engine_format_time(best_score, text);
		else
			function_1630e0(text, L"%d", best_score);

		result = unicode_string_vsnprintf(destination, remaining, L"%s", text);
		if (result < 0)
			result = remaining;
	}
	else if (!wcsncmp(L"#local_player_score", token, token_length))
	{
		if (g_4b9ed8 != NONE)
		{
			long player_index = g_4e8c20->entries[g_4b9ed8];
			if (player_index != NONE)
			{
				text[0] = 0;
				function_159130(function_1587b0(player_index), text);
				result = unicode_string_vsnprintf(destination, remaining, L"%s", text);
				if (result < 0)
					result = remaining;
			}
		}
	}
	else if (!wcsncmp(L"#local_team_score", token, token_length) && function_15eaf0())
	{
		long player_index = function_14de70(g_4b9ed8);
		if (player_index != NONE)
		{
			char team = event_player_get(player_index)->team;
			if (team != NONE)
			{
				text[0] = 0;
				function_159130(function_1587f0(team), text);
				result = unicode_string_vsnprintf(destination, remaining, L"%s", text);
				if (result < 0)
					result = remaining;
			}
		}
	}
	else if (!wcsncmp(L"#score_to_win", token, token_length))
	{
		text[0] = 0;
		function_159130(g_4e6948->score_to_win, text);
		result = unicode_string_vsnprintf(destination, remaining, L"%s", text);
		if (result < 0)
			result = remaining;
	}
	else if (!wcsncmp(L"#local_spawn_time", token, token_length))
	{
		long player_index = function_14de70(g_4b9ed8);
		if (player_index != NONE)
		{
			result = unicode_string_vsnprintf(destination, remaining, L"%d", event_player_get(player_index)->respawn_ticks);
			if (result < 0)
				result = remaining;
		}
	}
	else if (!wcsncmp(L"#local_lives_remaining", token, token_length))
	{
		long player_index = function_14de70(g_4b9ed8);
		if (player_index != NONE)
		{
			result = unicode_string_vsnprintf(destination, remaining, L"%d", event_player_get(player_index)->lives);
			if (result < 0)
				result = remaining;
		}
	}
	else if (!wcsncmp(L"#switch_side_time", token, token_length))
	{
		result = unicode_string_vsnprintf(destination, remaining, L"%d", (short)g_4e9ae8->we0);
		if (result < 0)
			return remaining;
	}
	else
	{
		result = g_55e4d0[g_4e9ae8->engine_index]->get_event_token_text(token, token_length, event, destination, remaining);
		if (!result)
		{
			result = token_length;
			if (result > remaining)
				result = remaining;
			wcsncpy(destination, token, result);
		}
	}

	return result;
}

/* plays the response's sound for the event, or one of its random sounds */
// @retail 0x19e770
void function_19e770(s_event_response *response, s_event *event)
{
	long delay;
	long sound_index;
	bool flag = TEST_FIELD_BIT(response->sound_flags.bit0);
	long language = flag ? get_current_language() : 0;

	sound_index = response->sounds[language].index;
	if (response->sound_delay == 0.0f)
		delay = 0;
	else
		delay = game_seconds_to_ticks_round(response->sound_delay);

	if (response->permutation_count > 0)
	{
		real random = function_x82e52f(&g_4e7408->seed, __FILE__, __LINE__);
		real sum = 0.0f;
		long i;

		for (i = 0; i < response->permutation_count; i++)
		{
			s_event_sound_permutation *permutation = &response->permutations[i];

			sum += permutation->weight;
			if (random > sum)
			{
				flag = TEST_FIELD_BIT(permutation->flags.bit0);
				sound_index = permutation->sounds[language].index;
				break;
			}
		}
	}

	if (sound_index != NONE)
		function_23ef80(sound_index, delay, event, flag);
}

/* shows a player the response's text and timer and plays its sound */
// @retail 0x19e890
void function_19e890(long player_index, s_event_response *response, s_event *event)
{
	s_event_player *player = event_player_get(player_index);
	s_event_globals *globals = ((s_event_globals_definition *)g_4e3b44[g_4e034c->index & 0xffff].bytes)->globals;
	s_event *const *event_reference = &event;

	if (player->local_index != NONE && (!g_4e6948->flag1128 || (*event_reference)->type == 0 && event->subtype == 0x13))
	{
		if (response->string_handle != NONE && response->string_handle != 0)
		{
			word string[0x100];
			word plural_string[0x100];
			word text[0x100];
			word plural_text[0x100];

			string[0] = 0;
			function_1a0180(globals->strings, response->string_handle, string);
			function_19e0f0(string, 0x100, text, event);
			if (TEST_FIELD_BIT(response->flags.plural))
			{
				plural_string[0] = 0;
				function_1a0180(globals->strings, response->plural_string_id, plural_string);
				function_19e0f0(plural_string, 0x100, plural_text, event);
				function_24caac(player->local_index, text, plural_text, event->f);
			}
			else
			{
				function_24cbee(player->local_index, text);
			}
		}

		if (response->timer_string_id != NONE && response->timer_string_id != 0)
		{
			long seconds = response->field_18_3;
			word string[0x100];
			word text[0x100];

			string[0] = 0;
			function_1a0180(globals->strings, response->timer_string_id, string);
			function_19e0f0(string, 0x100, text, event);
			function_24cc73(player->local_index, text, game_seconds_to_ticks_round((real)seconds));
		}

		function_19e770(response, event);
	}
}

/* the player's name, as the text shows it */
// @retail 0x19ea60
void function_19ea60(long player_index, word *buffer)
{
	word text[0x100];

	wcsncpy(buffer, event_player_get(player_index)->name, 0xff);
	buffer[0xff] = 0;
	wcsncpy(text, buffer, 0xff);
	text[0xff] = 0;
	function_22d2ee(text, 0x100);
	wcsncpy(buffer, text, 0xff);
	buffer[0xff] = 0;
}

/* the team's name, as the text shows it */
// @retail 0x19eae0
void function_19eae0(long team, word *buffer)
{
	s_event_universal_globals *universal = ((s_event_globals_definition *)g_4e3b44[g_4e034c->index & 0xffff].bytes)->universal;
	long team_names;

	buffer[0] = 0;
	team_names = universal->team_names;
	if (team_names != NONE)
	{
		if (team == NONE)
			team = 8;
		function_1a0180(team_names, function_19fd00(team), buffer);
	}
}

// @retail 0x19eb30
void function_19eb30(s_event *event)
{
	if (event->a == NONE)
	{
		s_player_iterator iterator;

		iterator.data = g_4e8c24;
		iterator.absolute_index = NONE;
		iterator.index = NONE;
		while (player_iterator_next((long *)&iterator))
			function_19de80(event, iterator.index);
	}
	else
	{
		function_19de80(event, event->a);
	}
}

// @retail 0x19eb90
void function_19eb90(s_event *event)
{
	game_engine_event_send_inline(event);
}

// @retail 0x19ebb0
s_event *game_engine_event_initialize(long type, long subtype, s_event *event)
{
	game_engine_event_initialize_inline(event, type, subtype);
	return event;
}

// @retail 0x19ebe0
void game_engine_event_set_cause_player(long player_index, s_event *event)
{
	event->cause_player_index = player_index;
	event->cause_team = event_player_get(player_index)->team;
}

// @retail 0x19ec10
void game_engine_event_set_effect_player(s_event *event, long player_index)
{
	game_engine_event_set_effect_player_inline(event, player_index);
}
