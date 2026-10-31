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
// score 390 (was 390): retail clamps the copy length with `cmp remaining, length` and a conditional
// move, so the comparison written as `remaining <= result` was tried: with the volatile result 466,
// with a plain result 652 (against 390 and 691). The plain result does match retail's frame 0x204
// and its ESI result register, but it merges the four name/team copy tails into one block, where
// retail keeps a copy of the length and clamp in each case and shares only the copy itself.
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
// Round 272: entry declarations for cause/effect/both teams and both with opponent
// score, inherited/ordinary result 390/691/390/691/390/691/390/691. Original lifetimes restored.
// Round 393: Length/copy/comparison/format noinline declarations with inherited result 390/390/390/390;
// ordinary result 691/691/691/691. All declarations and original case bodies restored.
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
// Lane 18 round 6: retail's 4 frame bytes past the text buffer hold the inlined teams test's dead byte store (ours
// makes it too), so the volatile result is the extra slot. A plain result with one shared inline copy helper
// (length, clamp, copy; four callers) 691 (same code as the plain result); the opponent loop's team as long 421
// (team in EDI passed in ECX as retail, 1424 bytes); with `best_score <= function_1587f0(team)` 421 (loop body
// now retail's, missing only its `xor edi, edi` and padding); both with the plain result 688; and with the text
// buffer declared before the plain result 688. Original body restored.
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
