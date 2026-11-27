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
// score 390 (was 390): The token preload and 0x204 frame still differ. With an ordinary result the name-copy tails merge.
// MiniMax D (11-20, round 132): no new form, no lower score (390): the body is unchanged. With an ordinary result the
// four name and team copy tails still merge into one block (691) and the volatile result keeps them apart but frames
// 0x208 against retail's 0x204, so retail's ranking (the result in ESI, the token length in EDI, the event in EBX, the
// remaining count in EBP, with the token re-read from its stack slot for every compare) and its token preload are not
// reproduced here.
// M3 lane GB r3 (fast with 0x19e0f0, 0x19ea60, 0x19eae0, 0x19df10, 0x19de80, 0x19e770, 0x19e890, all MATCH; control 390, 1437/1443 B, +0x0; 12 source trials + 2 flag diagnostics, body unchanged): flag diagnostic dropping /Ob1 gives identical code (390 volatile / 691 plain). On plain result (frame 0x204): retail-faithful body (direct event teams, switch_side_time result=remaining, best_score<=, long team, remaining<=result clamps, player ptr in local_team/spawn_time) 703 (1161 B, al 423, mask 257); s_wchar_buf class for text 710 (1152 B, mask 212); s_out_buf {dst, rem} with inline methods 587 (1274 B, frame 0x20c), with forceinline methods 672 (1189 B, al 381), constructor-only 703; s_token {ptr, len} struct 703; single shared `if (result < 0) result = remaining;` at exit 632 (1233 B: splits arm 1 `mov word [esp+0x14], si; lea esi` from arm 2 `lea esi; mov word [esp+0x14], 0` with event=EBX, result=ESI, token still EDI); rem_ptr / tok_ptr inline helpers 703 each. On volatile result: single shared negative-exit clamp 477 (1506 B); s_out_buf 484 (1376 B, frame 0x210); best_score<= + direct event teams 392 (1437 B).
// MiniMax H (11-19, round 95): no new form, no lower score (390): retail's register ranking is the result in ESI, the token
// length in EDI, the event in EBX and the remaining count in EBP, with the token re-read from its stack slot for every
// compare, while this build keeps the result in a volatile slot, so the constant zero takes EBP and the frame is 0x208.
// An ordinary result does give retail's 0x204 frame and its ESI result, but it merges the name and team copy tails (691),
// and no reordering of the four name arms avoids that merge, so the body stands.
// MiniMax B (11-18, round 2): no new form, no lower score (390): the frame is still 0x208 against retail's 0x204 and
// the token preload still differs; the permuter over the body (one job, --fast, 300 s) tried 24 forms and its best is
// 384 differing instructions against the 385 baseline - one instruction, which the eight-point gate leaves out - and
// with an ordinary result the four name and team copy tails still merge (691 at 1170 bytes), so the body stands.
// retail clamps the copy length with `cmp remaining, length` and a conditional
// move, so the comparison written as `remaining <= result` was tried: with the volatile result 466,
// with a plain result 652 (against 390 and 691). The plain result does match retail's frame 0x204
// and its ESI result register, but it merges the four name/team copy tails into one block, where
// retail keeps a copy of the length and clamp in each case and shares only the copy itself.
// MiniMax b (11-11): no new form, no lower score (390); our frame is still 0x208 against retail's 0x204, the
// volatile result keeps our four name and team copy tails apart where retail keeps a copy of the length and
// clamp in each case, and retail's prologue forms the first token compare with the string, the stack argument
// and the token in the other registers.
// MiniMax G (11-12): no new form, no lower score (390): the volatile result still keeps our four copy tails apart but frames 0x208 against retail's 0x204, and a plain result reaches that frame and retail's ESI result register only by merging the four tails into one block (691 this round); the permuter's 319 seconds over the body found nothing below its 385 baseline.
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
// M3 lane 11 r99 (fast with 0x19e0f0, which stays matched): retail passes `remaining` to the formatter in EDX from EBP and
// shares one formatter call site among the `%s`/`%d` cases. The four name/team copies through a named `long length` (clamp,
// copy, then `result = length;`) on the volatile result 620 (1237 B: the tails merge), on the plain result 691 (same code
// as the plain result); every formatter failure as `return remaining;` (seven sites) on the volatile result 439 (1416 B);
// the named lengths with those returns on the plain result 696 (1167 B); the four copies ending in `return result;` on the
// plain result 691. Body unchanged.
// T106: Actual entry-owned capacity consumed by name/formatter/engine families,
// seven ordinary-result graphs then complete inherited-result graph:
// 458/701/458/701/458/701/458/434. Name forms recover token preload and
// early capacity EBP but retain frame 0x208 and merge name tails. NOT kept:
// no native frame/result/copy-tail equality; original full body restored.
// M3 lane 2 r82 (fast with 0x19e0f0/0x19ea60/0x19eae0, matched throughout; control 390, plain result 691): retail's name
// tails 1 and 4 are byte-identical from the wcslen call to their jump yet unmerged; only the wcsncpy call is shared, and
// the #cause_team tail joins the token fallback's clamp. On the plain result: each name/team case ending in `return
// result;` 691 (same code); the text buffer declared in each case's block 691 (same code, so the merge follows the slot,
// not the symbol); both 691. On the volatile result: per-case buffers 390, per-case returns 390 (both the same code).
// Diagnostic: one to three unused `long` locals on the plain result 691 each (no phase effect).
// M3 lane 5 r184 (fast with 0x19e0f0/0x19ea60/0x19eae0, matched throughout; control 390, aligned 199): every clamp as
// `if (remaining < result)` on the volatile result 390 (same score); the two team-name cases reading the event's team at
// each use (no team locals) 394 (aligned 192); that with the text buffer declared in each case's block 394 (aligned 190);
// the opponent iterator `long` 421 (1424 B, aligned 160); `__forceinline` on teams_enabled_19e1c0 390. Plain result built for reading: retail's name tails 2 and 1/4
// differ in their scratch registers (ECX/EDX against EDX/EAX), the cause-team tail joins the fallback's clamp with the
// flags live across the jump, and the other name tails share only the copy call, so retail's cross-jumping leaves
// byte-identical tails 1 and 4 apart. Body unchanged.
// M3 lane 12 r124 (fast with 0x19e0f0/0x19ea60/0x19eae0, matched each time; base 390, plain result 691 at 1170 B): the four
// name/team clamps as `result >= remaining` 691 plain, 390 volatile; the text buffer declared ahead of the result 691 plain,
// 390 volatile; both team admissions with the team test first on the plain result 688 (1172 B). On the plain result the two
// player-name bodies are one block (the second jumps into the first), and its `text[0]` store takes an immediate after the
// buffer's `lea esi`; retail's first body stores it from the zero result register (si) ahead of the lea, so its two bodies
// differ and never cross-jump. Retail also keeps `remaining` in EBP and the token on the stack, where ours keeps the token.
// M3 lane 2 r85 (fast with 0x19e0f0/0x19ea60/0x19eae0, matched throughout; control 390, plain result 691 at 1170 B): the
// name/team clamps spelled differently per case (second `remaining < result`, fourth `result >= remaining`) on the
// volatile result 466 (1485 B), on the plain result 542 (1314 B: fewer tails merge, but the two player cases still share
// their post-call code and token stays in EDI); all four spelled differently (`>`, `<`, `>=`, `<=`) plain 509 (1349 B),
// volatile 466 (1485 B); the plain result with `token` read through `word const *const *` to itself at every use 691 (the
// reference folds: token still EDI, the same code). Retail's tails differ only by its per-compare token reloads.
// M3 lane 1 r107 (fast with 0x19e0f0/0x19ea60/0x19eae0, matched throughout; control 390): on lane 13 r82's 385 form (named player
// pointer in the spawn-time and lives cases with `best_score <= function_1587f0(team)`, re-measured 385 at 1443 B): every clamp
// as `if (remaining < result)` 385; a block-scope text buffer in each name/team case (function-scope buffer kept for the formatted
// cases) 419 (1473 B); both team admissions team test first 396; a named player pointer in the local-team case too 385; the
// scoring mode read at each use 385. With the plain result ours enregisters token, token_length, event and result and leaves
// `remaining` on the stack; retail gives `remaining` EBP and reloads the token for every compare.
// P539: Complete signed copy-capacity outcomes bool/byte/short/long, inherited/ordinary result:
// 432/432/523/522/691/691/691/691. NOT kept: all regress; ordinary results still merge name-copy tails.
// Original result qualifier, direct signed clamps, text workspace and complete body restored.
// M3 lane 13 r98 (fast with 0x19e0f0/0x19ea60/0x19eae0, matched throughout; control 390, plain result 691): one shared inline
// copy helper (clamp, wcsncpy, return the length) for the four name cases and the token fallback, plain result 682 (1178 B),
// on the volatile result 654 (1201 B: the expansions merge); the helper comparing `remaining <= length` 682 (same code); the
// plain result with the clamps as `result = remaining < result ? remaining : result;` 691; the plain result with every token
// comparison through one shared inline predicate 691 (same code: the token keeps EDI, `remaining` stays on the stack).
// MiniMax E (11-15): no new form, no lower score (390): the extra dword in our frame is the length temporary at +0x14
// beside the volatile result at +0x10, and retail's first name case stores the buffer's zero word before taking its address
// while its second case takes the address first, so retail's two copy tails stay apart where ours merge.
// M3 lane 10 r113 (fast with 0x19e0f0/0x19ea60/0x19eae0, matched throughout; control 390, plain result 691 at 1170 B, frame
// 0x204 as retail): retail's result is a register (ESI, clamped with cmovle), `remaining` is EBP for the whole function and
// the token is reloaded from its slot for each compare; the plain result enregisters the token (EDI) instead and its case
// bodies come out identical and merge. On the plain result: the cause team read once into a local ahead of the opponent
// scan 693 (1169 B); the scoring mode read at each use (no `mode` local) 691 (same code); the local-team value as `long` 693
// (1169 B). On the volatile result: the cause-team local ahead of the scan 415 (1436 B); the local-team value as `long` 393
// (1436 B). Body unchanged.
// MiniMax I (11-16): no new form, no lower score (390): the plain result still merges the four name and team copy tails where retail keeps them apart - its first name case stores the buffer's zero word before taking its address and its second case takes the address first - and retail keeps `remaining` in EBP and reloads the token for every compare where ours keeps the token in a register; the clamps and the copy helper of the earlier rounds come to 385 at best, a gain of 5 that is below the keep bar.
// M3 lane 12 r135 (fast with 0x19e0f0/0x19ea60/0x19eae0, matched throughout; control 390, plain result 691 at 1170 B):
// the volatile result makes ours hold a constant zero in EBP for the whole function (every `text[0] = 0` stores `bp`, the
// event tests compare against EBP), the register retail gives to `remaining`; on the plain result ours sets up 0x19ea60's
// ESI argument ahead of the first case's `text[0] = 0` (an immediate store) where retail stores the zero result register SI
// first. Forms: the player index read into a local ahead of `text[0] = 0` in both player cases, plain/volatile 691/390; the
// player cases passing `&text[0]`, volatile/plain 390/691 (all the same code as their bases).
// M3 lane 13 r105 (fast with 0x19e0f0, which stays matched; control 390, plain result 691): with the plain result ours has one
// call each to 0x19ea60, 0x19eae0 and wcslen (the name tails cross-jump into one) where retail has two, two and four; retail's
// per-compare token reloads shift the scratch rotation so its tails differ. Forms: plain result with one `text[0] = 0;` at the
// function's top for every case 644 (1212 B, token still EDI); that with the event teams read at each use 694 (1161 B); plain
// result with the teams at each use and a named player pointer in the spawn-time and lives cases 667 (1196 B), with `best_score
// <= function_1587f0(team)` too 667; the volatile result with those three together 387 (1443 B, retail's length; 3 points and a
// one-use pointer, not kept).
// ordinary rejection assignments 701;
// qualified rejection assignments 437;
// ordinary rejection-first branches 657;
// qualified rejection-first branches 415;
// ordinary arm-local defaults 695;
// qualified arm-local defaults 420;
// ordinary result with reject returns 662;
// qualified result with reject returns 420;
// Original body restored.
// NOT kept: complete string-format returns 643, integer-format returns 655, all formatted returns
// 707 with plain copy result and 458 with existing result; shared signed name/literal clamp and
// copy with per-name lengths 557 plain or 519 with existing result; shared name clamp/copy 548;
// shared name length/clamp/copy 596. The plain result recovers frame 0x204 but merges the name tails.
// The entry token preload and separate length/clamp blocks still differ. Shared final copy only, with branch-local
// length and clamp, for player names, team names, all names, or names plus fallback: existing result 422, 478, 527,
// 539. Ordinary result 605, 569, 550, 568. Ordinary results reach frame 0x204 and shorten the body. Body restored.
// Token preload, ordinary result and independent copy tails still differ. Capacity-initialized result
// with zero on rejected cases, through name clamp, formats, both, or both plus engine fallback:
// ordinary 458/701/458/458, existing qualified result 427/497/431/434.
// m3 lane 5 r196 (fast with 0x19e0f0/0x19ea60/0x19eae0, matched throughout; control 390, plain result 691): re-measured
// without knowing they were listed, the teams-enabled helper as one `&&` return plain/volatile 736/454, as if/else arms or a
// ternary 414/414, the ternary on the plain result 698 (the listed admission forms). New: `remaining` bound by `long const &`
// for every use, plain/volatile 691/390; `token` bound by `word const *const &` for every use, plain 691 (all fold). Lever,
// diagnostic only: the plain result with a `remaining > 0 &&` test in every token compare 440 (1423 B): `remaining` then
// takes EDI and `token` is re-read from its home for every compare as in retail; the same test in the first case only 655
// (token keeps EDI). A compile-only copy of the file (no /GL; build/r196/cc19e.py) flips already with one early `remaining`
// use (first or second case's condition, not a later one, not a token_length or destination test): token preload above
// `sub esp`, the first buffer zero stored from SI and retail's four wcslen, two 0x19ea60 and two 0x19eae0 calls, frame
// 0x208 only for the opponent loop's spilled down-counter. 121 random combinations of listed spellings (clamp, teams at
// each use, failure returns, counter/team widths, mode local, declaration order, fallback test) never move token there.
// So retail's source weights `remaining` above `token` on the dispatch path; no natural spelling found that does.
// AC571: Teams predicate as unsigned byte/plain byte/word/long 390/390/409/410, and each with ordinary
// returned count 691. NOT kept: the word predicate reaches frame 0x204 at 1437 bytes but replaces the
// retail predicate spill with a word compare and misses the token preload. Ordinary results merge name tails.
// M3 lane 3 r259 (fast with 0x19e0f0, matched each time; control 390): every later read of `remaining` through a `long
// const &capacity = remaining;` binding at entry, plain result 691 (1170 B, folds to the plain result), volatile result
// 390 (same code); the two player-name cases through a two-caller forced-inline helper (clear, name call, length, clamp,
// copy) 460 volatile (1392 B) / 691 plain; the two team-name cases through the same kind of helper 484 volatile (1368 B);
// the spawn-time and lives `%d` prints with their failure fallback through a two-caller helper 473 volatile (1513 B).
// The token preload and 0x204 frame with separate name lengths and clamps remain unresolved.
// ordinary result and early player-name returns 691;
// ordinary result and returns through cause-team name 691; ordinary result and early name returns 691;
// ordinary result and returns through cause-team score 665;
// existing result and early player-name returns 390;
// existing result and returns through cause-team name 390; existing result and early name returns 390;
// existing result and returns through cause-team score 390.
// NOT kept: the ordinary result reaches frame 0x204 but moves farther from retail's length and merges the name tails.
// M3 lane 16 r137 (fast with 0x19e0f0; control 390): the switch-side failure as `result = remaining;` (single exit)
// 431 (1398 B), with a plain result 732 (1129 B); the engine's token text in its own `written` local with the fallback
// copy in the else 391 (1450 B), with a plain result 678 (1183 B), plus the single switch-side exit 719 (1142 B).
// Token comparisons supply the selected arm's zero count for player names,
// team names, local scores and timers, or all tokens. Existing count storage scores
// 453/443/489/579. Ordinary count scores 695/684/689/688. NOT kept: the
// ordinary count gives frame 0x204 but still merges the name producers.
// M3 lane 6 r199 (fast with 0x19e0f0/0x19ea60/0x19eae0, matched throughout; control 390, plain result 691): the four
// name/team clamps written into `remaining` itself (`if (remaining > length) remaining = length;`, then the copy and
// `result = remaining;`) plain 692 (1170 B), volatile 618 (1235 B); that with the token fallback clamped the same way
// plain 686 (1176 B), volatile 626 (1226 B); the clear-and-name calls through two two-caller inline helpers (`buffer[0] = 0;`
// then 0x19ea60 / 0x19eae0) plain 691, volatile 390 (same code each). Compile-only (no /GL) diagnostic: a store of
// `remaining` to a probe global after the if/else chain gives retail's shape there (token re-read for every compare,
// `remaining` in EBP, four wcslen and two calls to each name function); a store of `token` there keeps token in memory but
// EBP then holds a constant zero. The LTCG build does not follow that screen (lane 4 r167's probe before the return 687).
// Round 140 (m3 lane 9, fast with 0x19e0f0/0x19ea60/0x19eae0, matched each time; control 390): retail's frame cell past
// the buffer is the teams-enabled predicate byte (ours spills it too, one cell higher, under the volatile result). On
// the plain result the fourth callee-saved register goes to `token` (retail: `remaining`), and the compile-only
// harness reproduces that exactly; 32 combinations of five spellings (teams read at each use, `best_score <=
// function_1587f0(team)`, a `long` team, the first clamp as `remaining < result`, the switch-side failure as `result =
// remaining`) never move it. Real: volatile result with the teams read at each use and the reversed compare 388 (1437
// B); the compare with a `long` team 421; teams at use, compare, `long` team and the single switch-side exit 461; teams
// at use with the first clamp `remaining < result` 466; the plain result with all five 652 (1211 B, tails merged).
// M3 lane 11 r120 (a one-file LTCG harness of this file with its callees' objects; positional counts; control 396 at
// 1437 B, plain result 416 at 1170 B with token EDI, token_length EBX, event EBP, result ESI, `remaining` on the stack):
// plain result with the switch-side failure as `result = remaining;` 416 (1129 B, token EDI); plain result with the four
// name clamps spelled `>`, `<`, `>=`, `<=` 407 (1353 B, token EDI); `token` as a one-pointer aggregate parameter (with
// 0x19e0f0's pointer type to match) 416 (scalarised, same code as the plain result). Diagnostics (reverted): one to eight
// extra uses of `remaining` inside the name cases leave token in EDI; dropping the fallback's two token uses leaves it in
// EDI; only an entry test of `remaining` gives it EBP. So uses inside the cases weigh far less than the thirteen
// compares on the dispatch path, and retail's token in memory needs fewer or lighter token uses on that path.
// Standard signed minimum after a single name-length call. Player/team/all names/all
// names plus fallback 505/504/440/525 with the existing result, 634/636/673/415
// with an ordinary result. NOT kept: the minimum stays out of line and needs
// a larger frame. The 415 form restores the opening extent, with the preload still missing.
// First comparison before both locals 390/691; native comparison count 390/691; signed name
// length published after the clamp 438/692; missing-index sentinel in dispatch 454/736
// (existing result and ordinary result, respectively). The ordinary count recovers the
// 0x204 frame while merging the name-copy paths. Token preload and capacity home still differ.
// M3 lane 1 r125 (fast with 0x19e0f0/0x19ea60/0x19eae0, matched throughout; control 390, plain result 691 at 1170 B):
// `remaining` copied into a named `long capacity` used for every clamp, format and the fallback, on the plain result 691,
// on the volatile result 390 (both the same code: the copy propagates); on the plain result the token parameter declared
// `word const *const` 691, `long const remaining` 691, `__forceinline` on teams_enabled_19e1c0 691 (all the same code).
// Reading (plain result): ours ranks result ESI, token EDI, token_length EBX, event EBP and leaves `remaining` in its home;
// retail ranks result ESI, token_length EDI, event EBX, `remaining` EBP and reloads the token for each compare, whose
// rotating reload temps are what keep its name/team tails from cross-jumping.
// M3 lane 14 r117 (fast with 0x19e0f0, 0x19ea60, 0x19eae0, matched throughout; control 390, plain result 691 at 1170 B:
// result ESI, token EDI, token_length EBX, event EBP, `remaining` on the stack, where retail has event EBX, token_length
// EDI, remaining EBP and token on the stack). New: the plain result with an entry `__assume(remaining >= 0)`
// (diagnostic, not keepable) 691 (same code: the assumption adds no weight); `token` declared `word const *const` 691 (same
// code); the fallback's clamp as `remaining <= token_length ? remaining : token_length` 693 plain (1168 B), 395 on the
// volatile result (1432 B). Body unchanged.
// M3 lane 2 r113 (fast with 0x19e0f0/0x19ea60/0x19eae0, matched throughout; control 390, aligned/masked in brackets):
// combinations of listed partial forms: the event teams read at each use with every clamp as `result = remaining <= result
// ? remaining : result` (retail's cmp/cmovle order) 425 (1457 B) [209/120]; that with `best_score <= function_1587f0(team)`
// 425 [207/119]; the plain result with those two and the switch-side failure as `result = remaining` 701 (1161 B)
// [183/131]; the ternary clamps with `best_score <=` 415 (1446 B) [212/124]; teams at each use, `best_score <=` and the
// switch-side `result = remaining` 431 (1398 B) [205/119]. Reading (retail): the token is never held in a register (a
// scratch reload per compare and again for the fallback copy, the last compare's reload landing in ESI), `remaining` holds
// EBP from the entry, the team cases jump to the shared return when the team is NONE and re-read it for the call.
// Shared integer format call for the two local-player tokens, lives and the side timer,
// all three, or all three with a direct normalized return. Existing result 450/488/522/471.
// Ordinary result 682/702/746/694.
// NOT kept: the ordinary result gets frame 0x204 and merges the name paths. The other
// forms still reserve 0x208. None recovers the token preload.
// M3 lane 13 r119 (fast with 0x19e0f0; control 390): the plain result with `remaining` renamed through a `long const &`
// local 691, through a local copy 691, `token` through a local copy 691, both copies 691 (1170 B each, same code as the
// plain result); the volatile result with the `remaining` copy 390 (same code). Copies of a parameter fold away before
// the allocator weighs token against remaining.
// Zero-result terminators for player names, team names, scores, or all 467/413/433/487.
// Ordinary result for player, team, score, or cause names 691/691/691/691.
// NOT kept: the team-name form reaches retail length but keeps the 0x208 frame.
// Ordinary results recover the 0x204 frame and merge the name-copy tails.
// M3 lane 7 r195 (fast with 0x19e0f0/0x19ea60/0x19eae0, matched unless said; control 390, plain result 691): parameter
// names as the tie-break on the plain result (`token` as string / a / z / name, `remaining` as a / z / count /
// maximum_length / buffer_size, 8 builds) 691 each (same code: names do not move the registers); 0x19e0f0 calling this
// function directly, volatile / plain result 449 / 663 (0x19e0f0 9 / 10: LTCG then takes the token length in EAX, keeps
// the token in EBP and re-reads the event from its slot at every use, the per-use reload retail shows for the token);
// the token as a by-value one-pointer class-type parameter (it has a constructor), plain result 691 (scalarised, the same code; 0x19e0f0
// 54); the `register` keyword on `remaining` 691 (ignored). Diagnostic (reverted): the opponent scan removed, or without
// its event read, moves the event to EDI and the token to EBP on the plain result, `remaining` still in memory: with the
// scan's read the event takes the last register (EBP) and without it the first (EDI), so the registers go out lowest
// weight first, and retail's order (token length EDI, event EBX, `remaining` EBP) puts `remaining` above the event,
// which has the scan's loop read. No natural form found that weighs `remaining` that high.
// M3 lane 4 r234 (fast with 0x19e0f0/0x19ea60/0x19eae0; control 390, plain result 691): the parameters in the caller's
// setup order (token, destination, remaining, token_length, event) with a direct call from 0x19e0f0 449 (1477 B: LTCG
// then passes token_length in EAX and token in ECX, the two heaviest values in our body, so the register parameters
// follow the body's ranking and retail's ECX/EDX pair means event and token_length outrank token there; 0x19e0f0 8);
// every NONE test NONE-first 390 / 691 plain (same code); the twelve token literals as named file-scope arrays 390 / 691
// (same code); the opponent scan's team declared ahead of best_score 390 / 691 (same code). Body unchanged.
// Round 203 (m3 lane 8, fast with 0x19e0f0/0x19ea60/0x19eae0, matched throughout; control 390, plain result 691 at 1170 B):
// diagnostics (a volatile probe local, reverted): a read of `remaining` ahead of the first compare flips the plain result
// to retail's homes (remaining EBP from the entry, the token preloaded into EAX and reloaded per compare) 453 (1470 B,
// token_length EBX, event EDI spilled); the same read inside the opponent loop leaves the token in EDI 708 (1157 B), so loop
// weight is not the lever, only an entry-block reference is. Natural entry references, each with its own test: `if
// (remaining <= 0) return 0;` at the top on the plain result 473 (1392 B, remaining EBP as retail), on the volatile result
// 428 (1436 B); `remaining > 0 &&` leading the first compare (plain) 479 (1487 B). The first player case clearing its
// buffer with the zero result (`text[0] = (word)result;`, retail's SI store) on the plain result 691 (same code: the zero
// propagates), with the early return 473. Body unchanged.
// M3 lane 14 r129 (fast with 0x19e0f0, which stays matched; control 390, plain result 691): the plain result with a
// redundant `result = 0;` at the top of the first name case 691 (same code); `token_length` typed `unsigned long` (with the
// caller's pointer type) on the volatile/plain result 390/691 (same code each); an entry `if (remaining <= 0) return 0;`
// (diagnostic: it adds a test retail lacks) on the plain result 473 (1392 B: `remaining` in EBP and the token re-read from
// its slot for every compare as in retail, token_length EBX and event EDI spilled where retail has EDI/EBX), on the
// volatile result 428 (1436 B).
// M3 lane 2 r122 (fast with 0x19e0f0/0x19ea60/0x19eae0, matched throughout; control 390, plain result 691 at 1170 B). Reading:
// both builds hand out ESI, EDI, EBX, EBP in rank order; ours ranks result, token, token length, event, retail result, token
// length, event, `remaining`: the same order with the token left out. Every callee's convention agrees (0x1587f0 takes its team
// in EDI in both). On the plain result: the token read through its own address (`word const *const *reference = &token;`) at
// the virtual call, at the fallback copy, at both, or at the first compare 691 each (same code: the reference folds away, so it
// does not take the token out of the ranking); the four name copies through a file-local inline bounded copy 691 (same code);
// the "%s" formats through a file-local inline bounded format 594 (1269 B, token still EDI); both helpers 594.
// M3 lane 2 r124 (fast with 0x19e0f0/0x19ea60/0x19eae0, matched throughout; control 390, plain result re-measured 691 at
// 1170 B: token EDI, token length EBX, event EBP, result ESI, `remaining` in memory). Loop shapes of the opponent scan,
// untried above (a bottom-tested helper loop is what moved the register ranking in 0x2689d0): on the plain result, `long
// team` with `do { ... } while (++team < 8);` 688 (1172 B), a `short` team with that loop 691 (same code as the plain
// result), `long team` with `team++` inside the body 688; on this body, a `short` team bottom-tested 390 (same code), `long
// team` bottom-tested 421 (1424 B). The token keeps EDI in all five, so the scan's shape does not decide the ranking.
// Fallback copy bound computed at entry, defaulting to token length or capacity in
// branches or ternaries: ordinary result 669/480/669/669, existing qualified result
// 563 each. NOT kept: the early bound adds a stack slot, growing the frame to 0x20c.
// The token is still loaded after the prologue and the 0x204 frame remains unresolved.
// M3 lane 4 r239 (fast with 0x19e0f0/0x19ea60/0x19eae0, MATCH unless said; control 390, plain result 691 at 1170 B): `token`
// and `destination` as one by-value two-pointer aggregate parameter (the caller builds it, same stack layout) plain 691,
// volatile 390 (same code each: the aggregate is scalarised); the three stack parameters as one by-value aggregate plain 691
// (same code, 0x19e0f0 then differs); the twelve compares through an inline predicate taking `&token` plain 691 (folded).
// Diagnostics (reverted): every failure tail made unique so that no two arms can share code, plain 717 (1669 B: the token
// still takes EDI and `remaining` stays on the stack with about 27 unshared uses, so shared tails are not what keeps it
// there); a volatile token parameter with the plain result 469 (1386 B: result ESI, token length EDI, event EBX and the token
// preload as retail, but the fourth register goes to a hoisted zero and `remaining` still stays on the stack). So retail's
// `remaining` outranks both the token and the zero constant, which only a reference on the dispatch path gives it here.
// M3 lane 5 r225 (h2m fast with 0x19e0f0 MATCH; control 390, plain result 691 at 1170 B). Reading: retail's two player arms
// do not share their copy tails because the token is re-read into the next scratch register before every compare, so each
// arm starts at another point of the scratch rotation and the arms' rows differ in registers; with the token in a
// callee-saved register every arm gets the same registers and the tails merge. On the plain result: the name clamps as
// `result < remaining ? result : remaining` 691, through a named length 692, as `if (remaining <= result)` 691; `remaining`
// copied to a capacity local at the top and used throughout 691 (same code). On the volatile result: the clamps through
// a named length 412 (1444 B). Diagnostics on the plain result (restored): an empty inline taking `remaining`, or the
// destination and `remaining`, called at the top 691; a discarded inline predicate on `remaining` at the top 691;
// `remaining = remaining;` at the top 691 (same code: a reference that emits nothing does not count as the entry-block
// reference round 203 found); the empty inline on the volatile result 390. Body unchanged.
// Complete signed clamp assignments on both branches for player/team/all names/all plus literal fallback
// 432/433/438/436 with the existing result, 685/684/692/694 with an ordinary result.
// The ordinary result recovers frame 0x204 and merges the length tails. The entry token preload remains absent.
// M3 lane 15 r129 (H2M_FAST with 0x19df10, 0x19de80, 0x19e0f0, 0x19e770, 0x19e890, all MATCH; control 390): on the plain
// result, the five clamps as `if (remaining <= result)` 691 and as `if (remaining < result)` 691 (1170 B, both the same code
// as the plain control); the plain result with `__assume(remaining >= 0)` as the first statement 691 (diagnostic: a
// code-free entry reference is not a use, `remaining` stays on the stack); the plain result with the twelve compares
// reading a `name` copy of the token 691 (the copy coalesces with the parameter); on this body the opponent loop's team
// counter typed `long` 421 (1424 B).
// M3 lane 7 r209 (h2m fast with 0x19e0f0/0x19ea60/0x19eae0, MATCH; control 390). Reading: retail's player and team
// arms end in byte-identical 13-instruction runs (wcslen, clamp, three pushes) that each jump to one shared wcsncpy
// call, so only the call is cross-jumped; the plain result merges the whole runs. Tried: the plain result with the
// switch-side-time error path as `result = remaining` 732 (1129 B); every format error path returning `remaining`
// directly 439 (1416 B), with the plain result 696 (1167 B); the plain result with `return result;` after each name and
// team copy 691 (same code as the plain result); the plain result with both team arms reading the event's team field at
// each use (retail's `mov ecx, [ebx + 0x18]` at the call) 661 (1202 B).
// Capacity return on formatter failure for the event-team scores, local/target
// scores, local timers, or opponent score only, 438/432/434/389. With ordinary
// result storage, 666/642/646/691. NOT kept: opponent-only gains one but
// keeps the 0x208 frame. Ordinary results merge name preparation.
// M3 lane 1 r152 (h2m fast with 0x19e0f0, MATCH each time; control 390, plain result 691): every formatter failure as
// `result = result < 0 ? remaining : result;` 573 (1570 B), on the plain result 691; the name clamps as a named length and
// `length < remaining ? length : remaining` on the plain result 692; the two player cases with the index in a named
// `player_index` local 390 (same code), on the plain result 691. Reading (plain result): the merge starts at the player
// cases' `lea esi` / zero store / 0x19ea60 call, which both cases emit in one order; retail's first player case stores the
// zero from SI (the zero result) ahead of the `lea esi` and its second case stores an immediate after it, and the
// following wcslen/cmovle tails take different scratch registers, so only the final copy call is shared.
// M3 lane 5 r230 (h2m fast with 0x19e0f0, MATCH; control 390, plain result 691 at 1170 B). Reading: the plain result
// ranks result ESI, token EDI, token length EBX, event EBP with `remaining` on the stack (retail: result ESI, token
// length EDI, event EBX, remaining EBP, token on the stack), and its opponent loop counts down an extra EBX counter where
// retail runs `inc edi; cmp edi, 8; jl`. On the plain result: `remaining` renamed with an `a_` prefix 691, `token`
// renamed with a `z_` prefix 691, `long const remaining` 691, the text buffer declared ahead of the result 691 (same code
// each: no name or declaration tie); the loop counter typed long 688 (1172 B); the score test written
// `best_score <= score` 691, with the long counter 688. On the volatile result: `best_score <= score` 388.
// M3 lane 6 r230 (H2M_FAST with 0x19e0f0, MATCH; control 390, plain result 691): a text buffer declared in each arm, on
// the plain result 691 and on the kept result 390 (same code each). Diagnostics, not keepable: the plain result with
// the token read through a volatile view of its own address at every use 469 (1386 B: result ESI, token length EDI,
// event EBX and the per-compare token reloads as retail, but EBP goes to a hoisted zero used by the event NULL tests
// and the `text[0]` stores, and `remaining` stays on the stack); those reads at the twelve compares only 473 (1381 B);
// with a long team counter 469 (same ranking). Body unchanged.
// M3 lane 8 r223 (h2m fast with 0x19e0f0, MATCH; control 390): the engine-text arm as `if (result) return result;` ahead of
// the token copy (retail returns there with a dead `mov esi, eax`) 390 (same code), on the plain result 693 (1168 B); the
// switch-side-time failure as `result = remaining;` on the volatile result 431 (1398 B); the text buffer declared ahead of
// the volatile result 390; the opponent arm's mode read from the global at each of its three uses 390 (same code each).
// M3 lane 4 r252 (H2M_FAST with 0x19e0f0, MATCH; control 390, all on the volatile result): the opponent loop counter
// typed long 421 (1424 B), with the score test written `best_score <= score` 421 (that test alone re-measured 388); both
// team arms reading the event's team field at each use 394; the local team typed long 393 (1436 B); the token
// fallback's clamp as one ternary 395 (1432 B). Retail reads the token from its argument slot ahead of the prologue
// (`mov eax, [esp + 4]` before `sub esp`), so the token is its lightest value. Body unchanged.
// M3 lane 14 r146 (H2M_FAST with 0x19e0f0, MATCH; control 390, aligned 324, masked 203): the opponent test as
// `best_score <= function_1587f0(team)` 388 (retail's `cmp esi, eax; jg`; under the bar); that with a long team counter
// 421 (1424 B); the opponent text cleared after the scan instead of before it 408 (1432 B); the time-mode test inverted
// with the arms swapped 390 (same code); the local player lookup as one conditional initialiser 390 (same code). Retail
// re-reads the zero of `wcsncmp`'s result for the score-to-win buffer clear (`mov word [text], ax`).
// M3 lane 2 r142 (H2M_FAST with 0x19e0f0, 0x19ea60 and 0x19eae0, all MATCH each run; control 390, difflib-aligned 229, 145
// with registers and offsets masked; plain result re-measured 691 at 1170 B, aligned 232; body unchanged). Reading of
// retail's merges, arm by arm: the four name arms are compiled apart (their scratch registers rotate: the destination in
// EAX, ECX, EDX, EAX and the text address in EDX, EAX, ECX, EDX) and each is joined only to the token fallback's tail, the
// first, second and fourth at its `call` and the third six instructions earlier, where its registers happen to agree;
// the first and fourth arms end in the same 13 instructions and are still not joined to each other. Of the formatted
// arms, #cause_team_score keeps the full tail with its own epilogue, #local_team_score, #score_to_win, #local_spawn_time,
// #local_lives_remaining and #switch_side_time jump into it at four different depths, and #cause_team_opponent_score and
// #local_player_score each keep a full copy with their own epilogue although their last eight instructions are that
// tail's. With the plain result this build joins every arm whose code is identical: the second player arm jumps to the
// first one's start, the team arms share one tail, the opponent arm's tail is copied into both of its format branches
// and each copy joins #cause_team_score's. Tried, all on the plain result: `return result;` closing the opponent and the
// local-player arms 704 (1160 B: those two join each other, through `cmovl eax, esi`, and leave the first score arm);
// those two arms with their own `count` local returned 704 (same code); the switch-side failure as `result = remaining;`
// 732 (1129 B, re-measured); that with the two returns 744 (1119 B); the four name copies through one file-local inline
// helper (length, clamp, copy, return the length) 732 (same code as without it: instances of one helper still join);
// each name arm with its own differently named length local, assigned to the result 732 or returned 732 (same code).
// So neither a separate variable, a separate helper instance nor a return keeps identical arms apart here; retail's
// arms were never identical when its merge ran, and what kept them apart is not found.
// M3 lane 8 r231 (h2m fast with 0x19e0f0, MATCH each run; control 390, plain result 691). Reading: each of retail's name
// arms makes four rotating scratch loads (the token, the wcslen argument, the copy's text address, the destination), so
// the next arm starts one register on and no two arms are the same code; with the token in a callee-saved register ours
// makes three, every arm starts alike and the arms join. On the plain result, all 691 (same code): `(void)remaining;` as
// the first statement; the token read through an aggregate view of its own address at every use (folded); the token as
// a by-value one-member aggregate parameter with 0x19e0f0's pointer type and call changed to suit (diagnostic: the member
// is enregistered like the pointer; 390 on the kept result); `remaining` typed dword with a `long capacity` copy used
// everywhere; a `capacity` copy reduced by the result ahead of the return. Body unchanged.
// Ordinary player-name count published after copying 460; team-name count 484; all names 620;
// all names with ordinary return count 691; capacity-default per-name counts 620, with ordinary
// return count 692; single fallback count publication 564; shared ordinary name count 620.
// NOT kept: the ordinary result recovers frame 0x204 while merging name preparation paths.
// M3 lane 12 r184 (h2m fast with 0x19e0f0, MATCH throughout; control 390, plain result re-measured 691): on the plain
// result the allocator splits the stack parameter `remaining` into thirteen small candidates, one per arm that reads it,
// because nothing on the compare chain references it, so the result, the token length, the event and the token take the
// four callee-saved registers; retail's one `remaining` web from entry needs a reference there. New forms: the whole
// dispatch in a file-local `static inline` helper called from a one-line definition 1839 (not expanded), the same
// helper `__forceinline` 691 (same code as the plain result); a dead `remaining -= result;` before the return 691 (same
// code); one redundant clamp of the result against `remaining` at the exit 661 (1201 B, still thirteen candidates).
// Diagnostic lever (a test retail lacks): `remaining > 0 &&` in front of the first compare only 479 (1487 B): `remaining`
// loads into EBP at retail's entry instruction, but the event takes EDI and the token length EBX (retail EBX / EDI).
// M3 lane 16 r185 (h2m fast with 0x19e0f0/0x19ea60/0x19eae0, MATCH each run; control 390, plain result re-measured 691 at
// 1170 B). Reading: on the plain result the allocator ranks the result 223, the token length 75, the event 63 and the
// token 52 (the token is one candidate from the entry block, where the first compare reads it), while `remaining` has no
// candidate of its own: each arm that clamps makes its own two-use load of it (thirteen of them, 8 to 12 points each).
// Retail's one load of `remaining` at entry therefore needs a reference to it in the entry block. Retail's first and
// fourth name arms end in the same twelve instructions and are not joined, so its joins only take the tail of the block
// ahead of the shared copy call; on our plain result whole identical arm blocks are joined (the second player arm jumps
// back into the first). On the plain result, all 691 (same code): `__assume(remaining >= 0)`, `> 0` or `!= 0` at the
// top (diagnostic); `do {} while (false);` at the top; that statement after or ahead of the copy call in each of the
// four name arms; `remaining` read through a `long const &` bound to it; through a one-member aggregate local
// initialised from it; through a `long` copy read by const reference. Body unchanged.
// Direct format exits, unconditional name/fallback copy with per-arm/common bounds 552/488;
// name-only copy with per-arm/common bounds 513/513; ordinary result with the same four forms 605/561/547/548.
// NOT kept: all shorten the body and still miss the entry token load.
// M3 lane 8 r237 (h2m fast with 0x19e0f0, MATCH; control 390, plain result 691). Ledger on the plain result: the token
// is one candidate (52: the entry compare plus twelve chain compares, minus twelve unreferenced chain blocks), while
// `remaining`'s only whole candidate is the fallback's (26, three references): no use of it dominates the arms. Forms:
// the first compare reading the token through a `word const *const *` bound to its address, plain result 691, kept
// result 390 (same code each: the indirection folds before the candidates form); a `long` copy of `remaining` made at
// the top and used everywhere 691 (copy-propagated); the dispatch in a forceinline helper 691 (re-measure of r184's).
// Diagnostic, a test retail lacks: `if (remaining < 0) return 0;` first on the plain result 473 (1392 B: `remaining`
// in EBP from entry, the token read from its slot per compare and the name arms no longer joined, as retail; the event
// takes EDI and the token length EBX where retail has EBX / EDI, and a frame word returns). Body unchanged.
// Lane GA r4 (fast with 0x19e0f0/0x19ea60/0x19eae0, all MATCH each run; control 390 at 1437/1443 B, 22 distinct trials):
// capacity-seeded plain result (`long result = remaining;` consumed by the 4 name clamps) with direct event->cause_team/effect_team,
// long team + `best_score <=` and player locals in local_team/spawn_time recovers retail's token preload (`mov eax, [esp+4]`),
// exact 0x204 frame, unmerged team tails, retail's `inc edi; cmp edi, 8; jl` scan and 1440/1443 B at +0xb (426; without player
// locals 439 at 1454 B; with `return 0;` on rejected paths 432 at 1453 B; ternary clamps 439; format arms also consuming result
// 439/432; shared negative-exit clamp 521), while neither direct teams alone (604) nor long team alone (671) nor the base seeded
// form (661) flips it; `result` then dies after arm 4 so `token` re-enters EDI for arms 5..12. Also tried: plain result with
// mutated entry `limit` local 589/589, shared negative-exit clamp on retail arms 632 (on volatile base 477), per-arm buffer struct
// with constructor/copy_to/length plain 709/709 / volatile 625, inline `%s`/`%d` format helpers plain 588 / capacity-seeded 429,
// split `return remaining;` on arms 5..7 plain 602, volatile base with `best_score <=` + player locals in local_team/spawn_time
// 387 (-3, under the bar and one-use pointers), plain retail arms control 709. Original body restored.
// M3 lane 8 r237 (h2m fast with 0x19e0f0, MATCH; control 390, plain result 691). Ledger on the plain result: the token
// is one candidate (52: the entry compare plus twelve chain compares, minus twelve unreferenced chain blocks), while
// `remaining`'s only whole candidate is the fallback's (26, three references): no use of it dominates the arms. Forms:
// the first compare reading the token through a `word const *const *` bound to its address, plain result 691, kept
// result 390 (same code each: the indirection folds before the candidates form); a `long` copy of `remaining` made at
// the top and used everywhere 691 (copy-propagated); the dispatch in a forceinline helper 691 (re-measure of r184's).
// Diagnostic, a test retail lacks: `if (remaining < 0) return 0;` first on the plain result 473 (1392 B: `remaining`
// in EBP from entry, the token read from its slot per compare and the name arms no longer joined, as retail; the event
// takes EDI and the token length EBX where retail has EBX / EDI, and a frame word returns). Body unchanged.
// Successful token comparison supplies the buffer clear for score-to-win,
// cause-player plus score-to-win, all names plus score-to-win, or every clear:
// 390/391/449/417 with the existing result and 691 each with an ordinary result.
// NOT kept: the entry preload still differs and the ordinary forms merge copy tails.
// The inherited result slot still makes the frame 0x208. Publish a selected formatter count after the
// success test with ordinary result storage: score text 640, local counters 650, side-switch timer 693,
// pairs 692/700/650, all 690. All with inherited result storage 467. NOT kept: the 0x204 forms merge name-
// length tails and shorten to 1162–1223 bytes.
// M3 lane GB r7 (fast with 0x19e0f0, 0x19ea60, 0x19eae0, 0x19df10, 0x19de80, 0x19e770 and 0x19e890, all MATCH; control
// 390 at 1437/1443 B, al=320, mk=207; 14 trials; body unchanged): capacity-seeded plain result (`long result = remaining;`)
// consumed across all 13 arms with `long written` and `return 0;` on rejects 579 (1274 B, frame 0x208, al=413, mk=147),
// with `result = 0;` on rejects 644 (1211 B, frame 0x208); `int` parameter/result types (matching `expand_token`) 691
// plain / 390 volatile (same code); four-caller `format_score_to_buffer` + three-caller `format_int_to_buffer` inline
// helpers 617 plain (1243 B, frame 0x204, al=321) / `format_score_to_buffer` on volatile 466 (1379 B); two-caller
// `event_has_team_name` helper 661 plain (1202 B, al=253) / 394 volatile (1437 B, al=298); `return remaining;` on arms
// 5..7 + `result = remaining;` on arm 12 + `best_score <=` on volatile 429 (1420 B); `size_t length = wcslen(text);` 691
// plain / 390 volatile (same code); `do { ... } while (false);` wrapper on plain 425 (1435 B, frame 0x204, al=449, mk=358:
// raises loop depth and inlines 0x19eae0 and 0x1587b0 while keeping token in EDI).
// Separate text buffers in every token arm from the first write or the outer arm, with shared
// or role-specific names: 390 each with the qualified result and 691 each with ordinary result.
// The ordinary result restores 0x204 but still merges the name-copy tails.
// M3 lane 11 r169 (h2m fast with 0x19e0f0, 0x19ea60 and 0x19eae0, all MATCH; control 390). Ranking on the plain result:
// the result 223, the token length 75, the event 63 and the token 52 take the four callee-saved registers; `remaining`'s
// whole candidate is -45 (its arm references add about 36, 37 blocks where it is live but unread take 81). Forcing that
// candidate to the top (a forced-ranking search, not source) gives 499 against the plain 691: `remaining` then holds a
// register from entry and the token is re-read per compare as retail, but a zero constant takes EBX, the event goes to
// EBP and the frame grows a word, so the ranking alone does not give retail's body. Forms: the chain inside
// `do { ... } while (false);` on the kept result 584 (1586 B); on the plain result 425 (listed, re-measured), with the
// event teams read at each use 589 (1586 B), with a `long` opponent team and `best_score <=` 429 (1432 B), with both
// 576 (1576 B); the kept result with the wrapper and the scan pair 486 (1511 B). Body unchanged.
// Initial result clipped from capacity or token length. Zero at optional-arm
// entry or on rejection, with engine success publication or miss-length
// selection: 663/659/490/626/664/659/490/626. NOT kept: count rises, and the 0x204
// form merges name tails. Token preload and EBP capacity still differ.
// M3 lane 2 r154 (fast with 0x19e0f0/0x19ea60/0x19eae0, MATCH each run; control 390 at 1437 B): the side-switch arm's
// negative result as `result = remaining;` like the other format arms 431 (1398 B: its tail then joins theirs); the local
// team as a `long` (retail sign-extends it into EDI and jumps into the cause-team score arm's tail) 393; the fallback
// clamp as one ternary 395; both 398; the event teams read at each use 394 (re-measured). Body unchanged.
// M3 lane 1 r173 (fast with 0x19e0f0, 0x19ea60, 0x19eae0; control 390): with the plain result our build also merges the
// two player-name arms' calls (arm 2 jumps into arm 1 with its index in EAX); retail keeps every arm whole, sharing only
// the final wcsncpy call, and its arms differ in scratch choice (arm 1 stores text[0] from SI before `lea esi`, arm 2
// stores an immediate after it). Forms: the clamps as `if (remaining < result)` 390 (same code), on the plain result 691;
// the thirteen token compares through one file-local inline helper 390, on the plain result 691; the plain result with
// the name arms' length in a named local and a one-ternary clamp 692.
// M3 lane 16 r196 (fast with 0x19e0f0, 0x19df10, 0x19de80, 0x19ea60 and 0x19eae0, all MATCH; control 390, plain
// result 691). Reading from the register allocator on the plain result: `remaining` totals -45 (references +36, live but
// unread in 37 blocks -81) against the token's 52, so the token keeps the fourth callee-saved register; a forced-ranking
// search over 14 single decisions finds nothing under 685, so retail's shape needs two moves at once (the token out of
// the race and `remaining` above the zero constant). Forms on the plain result: every formatted arm returning
// `remaining` on failure (the side-switch arm's form) 696 (1167 B); each name and team arm returning its result after
// the copy 691; the text buffer declared ahead of the result 691; the teams test as a direct `return a && b` 736
// (1128 B: retail's stored teams byte goes); the opponent arm's teams test and the engine-text arm through the
// existing header helper and engine getter in place of the file-local copy 390 (same code; plain result 691), but
// including that header here lost two matched functions elsewhere in the full check, so it is not kept. Body unchanged.
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

/* the team's name, as the text shows it; inline because retail expands it in
   0x1608e0's team rows (the name buffer cleared twice) while its callers here and in
   the postgame statistics keep the call */
// @retail 0x19eae0
inline void function_19eae0(long team, word *buffer)
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
