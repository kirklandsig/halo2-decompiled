// @flags /O1 /Ob1 /arch:SSE /Gr
/* UNKNOWN_24C7C1.CPP: list walkers and the state of the first-person HUD
   (g_5023f4) */

#include <math.h>
#include "camera_storage.h"
#include <stdlib.h>
#include <string.h>
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_19b516.h"
#include "unknown_13fd90.h"
#include "unknown_030290.h"
#include "unknown_13927e.h"

/* retail calls the pow intrinsic (_CIpow) even though this file is size-optimised */
#pragma intrinsic(pow)

struct s_link
{
	long unknown00;
	s_link *next;
};

class c_vtable_base
{
public:
	virtual void method0(long a) = 0;
};

class c_list_item : public c_vtable_base, public s_link
{
};

class c_event_base
{
public:
	virtual void method0(s_event **event, long *key) = 0;
};

class c_event_item : public c_event_base, public s_link
{
};

struct s_item_list
{
	s_link *first;
};

struct s_hud_element
{
	byte unknown00[0x22];
	word flag_index;
	byte state;
	byte unknown25[0x1b];
};

struct s_hud_tag_data
{
	byte unknown00[0xc];
	word (*flags)[1];
	byte unknown10[4];
	s_hud_element *elements;
};

/* a message of the HUD (four for each local player) */
struct s_hud_message
{
	long time;
	word text[0x3f];
	bool active;
	byte sequence;
	bool counted;
	bool expired;
	short count;
};

struct s_hud_player
{
	s_hud_message messages[4];
	word text[0x110];
	long value_440;
	bool flag_444;
	byte unknown445;
	short ticks_446;
	long sound_448;
	byte unknown44c[2];
	bool flag_44e;
	bool flag_44f;
	long time_450;
	word text_454[0x3f];
	bool flag_4d2;
	byte sequence_4d3;
	byte unknown4d4[4];
	long time_4d8;
	long value_4dc;
};

struct s_hud_state
{
	s_hud_player players[4];
	byte unknown1380[4];
	bool flag_1384;
	byte next_sequence;
	byte unknown1386[6];
	s_hud_element *element_138c;
	s_hud_element *element_1390;
	short value_1394;
	byte unknown1396[2];
	long time_1398;
	short ticks_139c;
	byte unknown139e[6];
	short value_13a4;
	byte flag_13a6;
	byte flag_13a7;
};


struct s_view_globals
{
	byte unknown00;
	byte flag;
};

struct s_entry_globals_view
{
	byte unknown00[0x20c];
	long tag_index;
};


s_hud_state *g_5023f4;
s_view_globals *g_510c98;

// @retail 0x24c7c1
void function_24c7c1(s_item_list *list, long a)
{
	for (s_link *link = list->first; link; link = link->next)
	{
		static_cast<c_list_item *>(link)->method0(a);
	}
}

// @retail 0x24c7e4
void function_24c7e4(void *list_pointer, s_event **event, long *key)
{
	s_item_list *list = (s_item_list *)list_pointer;
	for (s_link *link = list->first; link; link = link->next)
	{
		static_cast<c_event_item *>(link)->method0(event, key);
	}
}

// @retail 0x24c80b
s_hud_globals_definition *function_24c80b(void)
{
	s_hud_globals_definition *local_0 = 0;
	s_hud_globals_definition *local_1 = g_510c94;
	if (local_1)
		local_0 = (s_hud_globals_definition *)local_1->unknown00;
	return local_0;
}

// @retail 0x24c831
void function_24c831(short index)
{
	s_entry_globals_view *globals = (s_entry_globals_view *)g_4e0350;

	if (g_510c98->flag && globals->tag_index != NONE)
	{
		long tag_index = globals->tag_index;		s_hud_tag_data *data = (s_hud_tag_data *)g_4e3b44[tag_index & 0xffff].bytes;
		g_5023f4->element_138c = data->elements + index;
	}
}

// @retail 0x24c878
void function_24c878(short index)
{
	long tag_index = ((s_entry_globals_view *)g_4e0350)->tag_index;

	if (tag_index != NONE)
	{
		s_hud_tag_data *data = (s_hud_tag_data *)g_4e3b44[tag_index & 0xffff].bytes;
		s_hud_element *element = data->elements + index;

		if (element->state == 1 && *(byte *)(data->flags + element->flag_index) == 0)
		{
			s_hud_globals_definition *view = g_510c94;
			s_hud_state *hud = g_5023f4;

			hud->element_1390 = element;
			hud->value_1394 = view->timed_message.value_1e + view->timed_message.value_1c;
		}
	}
}

// @retail 0x24c8e2
void function_24c8e2(short a, short b)
{
	short ticks = (short)(a * 0x3c + b) * g_510c54->field_2_3;

	g_5023f4->ticks_139c = ticks;
	g_5023f4->flag_13a6 = 0;
	g_5023f4->flag_13a7 = 1;
	g_5023f4->time_1398 = g_510c54->game_time;

	short value = g_5023f4->value_13a4;
	g_5023f4->value_13a4 = (value < 0) ? 0 : (value > 4) ? 4 : value;
}

// @retail 0x24c93f
void function_24c93f(bool flag)
{
	g_5023f4->flag_13a6 = flag;

	short ticks = g_5023f4->ticks_139c;
	if (ticks > 0)
	{
		if (flag)
		{
			word delta = (word)g_5023f4->time_1398;
			delta -= (word)g_510c54->game_time;
			g_5023f4->ticks_139c = delta + ticks;
		}
		else
		{
			short delta = (short)g_510c54->game_time;
			delta -= (short)g_5023f4->time_1398;
			g_5023f4->ticks_139c = delta + ticks;
		}
	}
}

// @retail 0x24c98c
void function_24c98c(long index, bool flag)
{
	s_hud_player *player = (s_hud_player *)g_5023f4 + index;

	player->flag_44e |= (player->flag_444 != flag);
	player->flag_444 = flag;
	player->value_440 = 0;

	if (flag)
	{
		player->value_440 = 0;
		unicode_string_copy(player->text, (const word *)L"", 0xff);
	}

	player->flag_44f = flag;
}

/* ---- the messages of the HUD (lane O) ---- */

void function_13925f(long string_handle, word *buffer);
bool function_13cb40();
long function_14de70(long user_index);
bool function_161b60(long player_index);
long function_1896c0(real scale, long tag_index);

static inline s_hud_player *hud_player_get(long index)
{
	return &g_5023f4->players[index];
}

// @retail 0x24ca1d
void function_24ca1d(long player_index, word const *text, long sound, long tag_index)
{
	s_hud_player *player = hud_player_get(player_index);

	unicode_string_copy(player->text, text, 0xff);
	player->text[0xff] = 0;
	player->flag_444 = true;
	player->value_440 = 0;

	if (player->sound_448 != sound)
	{
		if (player->sound_448 == NONE && player->ticks_446 > g_510c54->field_2_3 / 4 && tag_index != NONE)
			function_1896c0(1.0f, tag_index);
		player->sound_448 = sound;
		player->ticks_446 = 0;
	}
}

// @retail 0x24c9e9
void function_24c9e9(long player_index, long string_handle, long sound, long tag_index)
{
	word text[0x100];

	text[0] = 0;
	function_13925f(string_handle, text);
	function_24ca1d(player_index, text, sound, tag_index);
}

/* the message to reuse: a free one, the one showing the same text, or the
   oldest */
// @retail 0x24ccdb
s_hud_message *function_24ccdb(s_hud_player *player, word const *text, word const *plural_text)
{
	long oldest_time = 0x7fffffff;
	short oldest_index = 0;

	for (short i = 0; i < sizeof(player->messages) / sizeof(player->messages[0]); i++)
	{
		s_hud_message *message = &player->messages[i];

		if (!message->active)
			return message;
		if (message->counted && !message->expired && text && plural_text &&
			!wcsncmp((wchar_t const *)message->text, (wchar_t const *)(message->count > 1 ? plural_text : text), 0x3f))
			return message;
		if (!message->active)
			return message;
		if (oldest_time > message->time)
		{
			oldest_time = message->time;
			oldest_index = i;
		}
	}

	s_hud_message *oldest = &player->messages[oldest_index];

	oldest->active = false;
	return oldest;
}

// @retail 0x24caac
void function_24caac(long player_index, word const *text, word const *plural_text, long count)
{
	if (player_index != NONE)
	{
		s_hud_player *player = hud_player_get(player_index);
		s_hud_message *message = function_24ccdb(player, text, plural_text);

		if (!message->active)
			message->count = 0;
		message->count += count;
		unicode_string_copy(message->text, message->count > 1 ? plural_text : text, 0x3f);
		message->counted = true;
		message->expired = false;
		message->time = g_510c54->game_time;
		message->active = true;
		message->sequence = g_5023f4->next_sequence++;
		player->flag_44e = false;
	}
}

// @retail 0x24cb54
void function_24cb54(long player_index, word const *text, word const *plural_text)
{
	if (player_index != NONE)
	{
		s_hud_player *player = hud_player_get(player_index);

		for (long i = 0; i < 4; i++)
		{
			s_hud_message *message = &player->messages[i];

			if (message->active && message->counted && !message->expired && text && plural_text &&
				!wcsncmp((wchar_t const *)message->text, (wchar_t const *)(message->count > 1 ? plural_text : text), 0x3f))
				message->expired = true;
		}
	}
}

// @retail 0x24cbee
void function_24cbee(long player_index, word const *text)
{
	if (player_index != NONE && text && *text)
	{
		s_hud_player *player = hud_player_get(player_index);
		s_hud_message *message = function_24ccdb(player, NULL, NULL);

		unicode_string_copy(message->text, text, 0x3f);
		message->time = g_510c54->game_time;
		message->active = true;
		message->counted = false;
		message->expired = false;
		message->sequence = g_5023f4->next_sequence++;
		player->flag_44e = false;
	}
}

// @retail 0x24cbbf
void function_24cbbf(long player_index, long string_handle)
{
	word text[0x100];

	text[0] = 0;
	function_13925f(string_handle, text);
	function_24cbee(player_index, text);
}

// @retail 0x24cc73
void function_24cc73(long player_index, word const *text, long value)
{
	if (player_index != NONE)
	{
		s_hud_player *player = hud_player_get(player_index);

		unicode_string_copy(player->text_454, text, 0x3f);
		s_hud_player volatile *local_0 = player;
		local_0->time_450 = g_510c54->game_time;
		local_0->flag_4d2 = true;
		local_0->sequence_4d3 = g_5023f4->next_sequence++;
		local_0->time_4d8 = g_510c54->game_time;
		local_0->value_4dc = value;
	}
}

/* sorts the messages: active ones first, then the newest */
// @retail 0x24cd7a
int __cdecl hud_message_compare(void const *a, void const *b)
{
	s_hud_message const *message_a = (s_hud_message const *)a;
	s_hud_message const *message_b = (s_hud_message const *)b;
	int result;

	if (message_b->active && !message_a->active)
		return 1;

	result = message_b->time - message_a->time;
	if (!result)
		result = message_b->sequence - message_a->sequence;

	return result;
}

// @retail 0x24cdaf
void __fastcall function_24cdaf(void)
{
	for (long i = 0; i < 4; i++)
	{
		for (long j = 0; j < 4; j++)
			g_5023f4->players[i].messages[j].active = false;
	}
}

// @retail 0x24cdd8
void function_24cdd8(long player_index)
{
	s_hud_globals_definition *view = function_24c80b();

	if (view && player_index != NONE && !function_13cb40() && function_161b60(function_14de70(player_index)))
	{
		s_hud_state *hud = g_5023f4;
		s_hud_player *player = &hud->players[player_index];
		short *ticks;
		long value;

		if (hud->element_1390 && hud->value_1394 > 0)
			hud->value_1394--;

		ticks = &player->ticks_446;
		value = *ticks + 1;
		if (*ticks + 1 > 0x7fff)
			value = 0x7fff;
		*ticks = (short)value;

		for (long i = 0; i < 4; i++)
		{
			s_hud_message *message = &player->messages[i];

			if (message->active && (real)(g_510c54->game_time - message->time) * g_510c54->rate >= view->display_time + view->fade_time)
			{
				message->time = NONE;
				message->active = false;
			}
		}
	}
}

/* scales the four vertices of a quad about its center (a callback of the
   text drawing code, whose parameter holds the scale's bits) */
// @retail 0x24cedf
bool __stdcall function_24cedf(real *vertices, long parameter)
{
	real scale;

	*(long *)&scale = parameter;

	if (scale > 0.0f)
	{
		real center_x = (vertices[0] + vertices[10]) * 0.5f;
		real center_y = (vertices[1] + vertices[11]) * 0.5f;

		for (long i = 0; i < 4; i++)
		{
			real *vertex = vertices + i * 5;

			real local_0 = *(real const volatile *)vertex - center_x;
			real local_1 = *(real const volatile *)(vertex + 1) - center_y;
			local_0 *= scale;
			local_0 *= 0.5f;
			vertex[0] = *(real const volatile *)vertex + local_0;
			local_1 *= scale;
			local_1 *= 0.5f;
			vertex[1] = *(real const volatile *)(vertex + 1) + local_1;
		}
	}

	return true;
}

/* ---- drawing the messages of the HUD (lane O) ---- */

struct s_local_player_table_view
{
	byte unknown00[8];
	short count;
};

struct s_510c4c;
struct s_510c4c_view
{
	byte unknown000[0x1b0];
	long text_count;
};

struct s_33a0b_default;

extern s_510c4c *g_510c4c;
extern s_33a0b_default *g_4686d4;

/* the frame the HUD draws in */

long function_122540(long font);
color4f *unpack_color4f(dword pixel, color4f *color);
color3f *unpack_color3f(dword pixel, color3f *color);
dword __cdecl pack_color4f(const color4f *color);
void function_1396c7(long a, point2f *point);
real function_1392a9(long local_player_index);
void function_13edb0(long font, long style, long justification, dword flags, color4f const *color, color4f const *field_24);
void function_13eb60(color4f const *color);
bool function_13ee20(word const *text, long font);
bool function_13ef30(word const *text);
void function_1fa30(word const *text, short_rectangle2d *bounds);
void function_22d2ee(word *string, long maximum_length);
bool function_15f120(long player_index, word *text, long maximum_count, long a);
bool function_163040(long local_player_index);
void function_13e9c0(word const *text, short_rectangle2d const *bounds, short_rectangle2d *a, short_rectangle2d *b, real scale);

/* measures text placed at (top, left) */
// @retail 0x24ce9d
void function_24ce9d(word const *text, long left, long top, short_rectangle2d *bounds, short_rectangle2d *result)
{
	short_rectangle2d rect;

	rect.top = top;
	rect.left = left;
	rect.bottom = top + 1000;
	rect.right = left + 1000;
	function_13e9c0(text, &rect, bounds, result, 1.0f);
	*result = *bounds;
}

/* draws a local player's HUD messages: the scripted message at the top,
   the list of messages, then the timed message and the status text at the
   bottom */
// score 80 (was 115): earlier 114 (was 114), 115 (was 115), 115 (was 114), 114 (was 114), 114 (was 115); frame 0x264 agrees; the predicate bytes' packing, the left word and the colour, rectangle and scalar homes differ.
// MiniMax S (11-20): no new form, no lower score (114): retail writes its three message flags as consecutive bytes at -0xf, -0xe and -0xd and the left coordinate as a word at -0x3c, where this build writes a dword at -0x14, bytes at -0xd and -2 and a dword at -0x50; the tutorial colour's block order and 0x24ce9d's argument setup are the only other differences.
// The body is unchanged: 114 and 115 are the same source in two whole-image states (one adjacent reload pair swaps). One note, the rounds' entries below.
// M3 lane 3 r268 (fast with 0x24ce9d, 0x15f120, 0x24cedf, all MATCH; control 115): the bool timer with `left` bound to a
// `short const &` for the bounds store 191 (1888 bytes; the reference folds away and `left` is still stored as a dword);
// the text height clamped as `height = bottom - top; if (height < 0) height = 0;` 452 (1888 bytes); the timed colour as
// `alpha *= fraction > 1.0f ? 1.0f : fraction` 115 (same code); the scripted test as two nested ifs 115 (same code); a long
// loop index 420 (1877 bytes); a long count 197 (1883 bytes). Diagnostic, reverted at once: with `left`'s address taken
// the store is still a dword (395): this build widens a short store into its four-byte home whenever the upper half is
// free, so retail's word store means the two bytes above `left` belong to something it never touches (the two-short
// aggregate the earlier rounds found).
// MiniMax R (11-17): no new form, no lower score (114; 115 in the batch of all fourteen, whose reduced image is wider): retail's timer message, left coordinate and view pointer still sit in byte and word homes where this build gives them dwords, and the difference still rests on the text buffer's call convention in 0x15f120: BLOCKED-BY 0x15f120.
// MiniMax R (11-14, run 65): no new form, no lower score (114 in the reduced image, 115 in the full one): retail
// keeps the three message predicates as bytes at -0xf/-0xe/-0xd and `left` in a word at -0x3c, while this build
// packs the predicates at -0x14/-0xd/-2 and spills `left` as a dword at -0x50; the only spelling that produces
// retail's word store is an invented two-short aggregate whose other half the function never touches, which review
// rejects, and the best keepable form (const short left) measures 109, a gain of five, under the keep bar. The
// body stands.
// MiniMax B (11-14): no new form, no lower score (115): retail keeps the view at -0x44, the font at -0x50, the line
// height at -0x14, the player at -0x40, the three message predicates as bytes at -0xf/-0xe/-0xd and `left` in a word at
// -0x3c, while this build puts the view at -0x30, the font at -0x4c, the line height at -0x18, the player at -0x3c and
// `left` in a dword at -0x50; the word needs 0x24ce9d to take a short, and that declaration may not change here:
// BLOCKED-BY 0x24ce9d.
// MiniMax C (11-14): no new form, no lower score (114 in the full build, 115 in the reduced one): retail's
// homes are the view at -0x44, the font at -0x50, the line height at -0x14, the player at -0x40, the bounds
// at -0xc, the text bounds at -0x24, the colour at -0x34 and the three message predicates as bytes at
// -0xf/-0xe/-0xd with the split-screen byte at -1; ours puts the predicates at -0x14/-0xd/-2, and
// retail keeps `left` in a word at -0x3c, sign-extended at the call to 0x24ce9d, where ours spills it as a
// dword at -0x50; that word needs 0x24ce9d to take a short, and its declaration may not change:
// BLOCKED-BY 0x24ce9d.
// MiniMax A (11-13): no new form, no lower score (114): the timer predicate as a bool puts the byte
// stores where retail has them but moves every other home, and scores 191; the three message predicates
// are still packed in different homes (ours at -0x14/-0xd/-2 against retail's bytes at -0xf/-0xe/-0xd) and
// retail keeps `left` in a word at -0x3c where ours spills it as a dword at -0x50.
// MiniMax G (11-12): no new form, no lower score (114); the three message predicates are still packed in different homes (ours at -0x14/-0xd/-2 against retail's bytes at -0xf/-0xe/-0xd) and retail keeps `left` in a word at -0x3c where ours spills it as a dword at -0x50.
// Earlier score line (114, was 115): frame 0x264 agrees; predicate packing, the left word and color/rectangle/scalar homes differ.
// frame 0x264 agrees; predicate packing, the left word and color/rectangle/scalar homes differ.
// MiniMax E (11-11): no new form, no lower score (114); the three message predicates are still packed in different
// homes (ours at -0x14/-0xd/-2 against retail's bytes at -0xf/-0xe/-0xd) and retail keeps `left` in a word at
// -0x3c where ours spills it as a dword at -0x50; the text buffer is still pushed where retail passes it in esi,
// which the text helper in another file decides.
// MiniMax C (11-10): the score is 115, unchanged; the three message predicates are packed in different homes (ours at -0x14/-0xd/-2 against retail's bytes at -0xf/-0xe/-0xd) and retail keeps `left` in a word at -0x3c where ours spills it as a dword at -0x50.
// MiniMax R (11-10): no new form, no lower score (115 in the eight-function image); the view's home is
// still four bytes above retail's and the text buffer is still pushed where retail passes it in esi,
// which the contributor's text helper decides.
// MiniMax S (11-07): no new form, no lower score; the three message predicates as bytes give 191, so the
// body stays; the view, font, line-height, player, colour and rectangle homes and the left word still differ.
// MiniMax R (11-05): no new form, no lower score; the timer flag as a bool puts retail's byte stores at
// -0xe/-0xd with the scripted flag still at -2 beside split_screen, 191, so the body stays (file now 114).
// MiniMax G (11-04): no new form, no lower score; retail keeps `left` in a word-sized local at -0x3c
// beside `top` and homes the colour and text bounds at -0x34/-0x24, ours spills `left` as a dword at
// -0x50 with the colour at -0x2c.
// frame 0x264 agrees; predicate packing, left-word and color/rectangle/scalar homes differ; both text helpers match.
// frame 0x264 agrees; predicate packing and color/rectangle/scalar homes still differ.
// frame 0x264 agrees; predicate packing, color/rectangle/scalar homes and the text-buffer call convention still differ.
// Round 15 (m3 lane 2, LTCG env harness of this file + depth-4 callee closure, aligned counts; base 123 = plain 115):
// bool timer 121; long left 129; bool timer with long left 127; every local renamed to sort first or last
// (view, font, line_height, player, hud, split_screen, height, status_player, origin, the three flags) identical
// code; 80 orders of the player/count/hud/flag/height declarations, long and bool timer each, 125-219 (none
// below the current order). The instructions are retail's; only the spill-home permutation differs.
// Earlier: frame slots differ (the scripted flag packs beside the split-screen
// byte, shifting colour, view, left and the loop index); BLOCKED-BY 0x15f120, whose text
// buffer retail passes in esi (our push is the extra byte). Tutorial colour as three calls 233;
// nested ternary 142; if-assigned pixel 383; byte timer flag 203 (retail length); byte timer
// with nested ternary 203; named any-message test 523.
// Phase foreground/background/both identities 142/142/142; rectangle role names 142; initial rectangle memcpy 390; color/rectangle identities together 142; unsigned word count 342; read-only HUD receiver 142 (fast; unchanged preview 142). Restored.
// Shared aggregate presence on the retained long-timer graph: bool/long with inherited row decrement
// 526/529; precomputed reserved-row subtraction 561/544 (exact-only); direct conditional bound
// 512/547; result-first bound 514/522. Original predicates, conditional decrement and body restored.
// Round 85: byte timer 191; scripted rename/declaration-first 191/191; early left declaration 115;
// outer fade ticks with byte timer 191; shared flash/fade scalar with byte/long timer 190/109 fast;
// the long-timer form 107 plain (seven-point gain, restored); long scripted 208; inverted tutorial
// choice 109; const short left 109; early shared declaration 109; result-first scripted 453.
// Round 131 (lane 4): the 0x15f120 call now agrees (buffer in esi; that callee matches), so the
// rest is the frame: retail keeps the timer flag in a byte at -0xf, shares the flash scalar with the
// loop index (-0x18), gives the message height its own slot (-0x4c) and stores left as a word at
// -0x3c; ours shares one dword (-0x14) among the timer flag, flash and message height. Bool timer
// 191 (1888 bytes, aligned 111 against 113); message height declared at the loop top 309 (aligned
// 118); top/left declared at their first assignment 114 (the reduced image's control, same code);
// ticks read from the field at each use 115 (same code); scripted flag computed first 205; text_bounds
// declared before bounds 114 (control, same code).
// Round 51: retail keeps the three message flags as bytes at -0xf/-0xe/-0xd and `left` as a word memory
// local at -0x3c; the flags are spill slots (renaming them and split_screen in four sort orders: no
// change). Bool timer 191 (1888 bytes; structurally closer: flags -0xe/-0xd, scripted still at -2
// beside split_screen). On the bool timer (LTCG env link, aligned counts against 123 for the base):
// tutorial colour as if/else 131, tutorial ternary inverted 121, `height` declared before the flags
// 126 (1901 bytes), scripted tested first 235; long timer with the tutorial if/else 133.
// Round 35 (lane 11, fast; unchanged source 114 fast): the three message flags declared uninitialised
// before the font 114 (same code); as bools so declared 191 (aligned 112 -> 111: timer/tutorial at
// -0xe/-0xd, scripted still at -2 where retail has -0xf/-0xe/-0xd); tutorial colour as `flag ? a4 :
// (b6 & 1) ? a8 : a4` 114 (same code); count as `split_screen ? 3 : 4` 479; short height 114 (same code).
// Round 54 (lane 16, fast; base 114): the tutorial colour as `!flag_1384 && (b6 & 1) ? a8 : a4` 114 (same code);
// `left` declared `short const` at its assignment 114 (same code); both 114; both with a bool timer 191 (1888
// bytes); the tutorial colour reading g_510c94 at each use (no `globals` local) 114 (same code).
// Round 62 (lane 12, fast; base 114): the two colour locals declared once at function scope 133; the
// rectangles declared before origin 115; `field_24` declared before `color` in each phase 115; `color`
// renamed (diagnostic) 115 with its home unchanged at -0x2c (retail -0x34); the rectangles at function
// scope 115. The one byte the bool timer loses is `left`: retail stores it as a 16-bit memory local
// (`mov word ptr [ebp-0x3c], ax`) where ours spills a dword, so the bool timer needs that form too.
// Round 40 (lane 11, fast; unchanged source 115 fast, 114 plain): retail stores `left` once with a word move
// (`mov word ptr [ebp-0x3c], ax`), so it is a memory local there, not a spilled register (ours spills it as a
// dword); with the bool timer that word store is retail's missing byte (the bool form is otherwise retail's
// flag code, bytes at -0xf/-0xe/-0xd but for the scripted flag at -2). Retail frame from EBP down: split
// screen, bounds/origin, the three flags, line height, loop index with the flash scalar, height, text bounds,
// colour, top (spilled in the loop), left, player, view, count, message height, font, field_24. Tried: one
// colour pair at function scope 133; the message height declared at the block top 115; the bool timer 191
// (1888 bytes); `(void)&left;` (diagnostic) 115 (optimized away, still a dword spill); the loop index at the
// block top / first 115/115; bool timer with the loop index at the block top 191.
// Lane 18 round 16 (fast; unchanged source 114 fast, 115 plain; one-link env with 0x13cb40's file, exact but for the
// pow calls): the tutorial colour named in a dword local 114; text_bounds declared first 114; left declared before
// split_screen 114; text_bounds and color renamed to sort first/last (tip #104) 114; the text buffer declared first
// 114 (all the same code). Env: bounds, field_24 or color renamed, origin/bounds/text_bounds/text in any of three
// further orders, each alone and on the bool timer: identical code (bool timer: flags -0xe/-0xd/-2, retail
// -0xf/-0xe/-0xd). Names and declaration order never move these homes; retail places text_bounds (-0x24) above
// the first colour (-0x34) where ours places it below (-0x38 under -0x2c).
// Round 142 (lane 4, fast; control 114): with the bool timer (191, 1888 bytes) the flags land at
// -0xe/-0xd with scripted at -2 beside split_screen (retail -0xf/-0xe/-0xd, -2..-4 unused); there
// split_screen renamed to sort last 191, scripted renamed to sort first/last 191/191, split_screen declared
// at function scope 191 (114 with the long timer), the timer or all three flags as `byte` 191/191: names,
// scope and byte/bool type do not move the byte slots.
// Round 16 (m3 lane 17, fast; unchanged source 115 fast): `left` renamed to sort first 115 (same code: it
// is a spill slot, so slot-name ordering does not reach it); `left` declared before `top` 115 (same); the
// HUD state global read at each use 115 (same); the text buffer declared first 115 (same); `top` as a long
// 499 (1882 bytes). In the bool-timer form the flags keep retail's relative order (split screen, scripted,
// tutorial, timer from EBP down); only the scripted flag back-fills the byte beside split screen (-2)
// where retail starts a new group after origin (-0xd).
// Lane 9 round 67 (fast, which reproduces 114): `left` computed after the split-screen top adjustment 125; the
// address-taken rectangles: retail homes text_bounds at -0x24 just below the scalars and the colour at -0x34, ours the
// colour at -0x2c and text_bounds at -0x38; text_bounds declared in each block that uses it, after/before the colour
// locals 114/114 (homes unchanged); that with the bool timer 191 (1888 bytes); flash declared at the phase block top
// 114 plain (fast crashes on that source). Diagnostic: a volatile `left` scrambles the entry (468).
// Round 287: Shared drawing coefficients across flash/row/status/timed phases,
// three short-height then five byte-timer subsets: 261/266/259/191/191/190/214/195. Predicate packing,
// left-word and color/scalar homes remain; all original values and body restored.
// Round 128 (m3 lane 6, fast; control 114): on the bool timer (191, 1888 bytes) `left` is still stored as a
// dword (retail `mov word ptr [ebp-0x3c], ax`) and the scripted flag still packs at -2: `left` assigned in two
// statements (cast, then `-= g_4b9dd2`) 191; `left` initialised to 0 and reassigned (diagnostic) 191; the two
// rectangles and the text buffer declared before origin 191; origin declared at function scope 191 (all the same
// code: a second definition does not give `left` a home). The compile-only harness is far off here (276 aligned).
// Round 490: all returned RGB records by memcpy, long/bool timer, 479/477; with local rectangle
// memmove 522/516. Native assignments with text/color/both workspace initialization 547/560/598;
// both initialized with bool timer 598. All copies, defaults, predicates and body restored.
// Round 48 (m3 lane 11, fast; control 115 fast, 114 plain): a compile-only screen shows VC storing a lone short
// local's whole register (dword) even when it is volatile or address-taken; only a `left` that shares its dword
// with another 2-byte member gets retail's `mov word ptr [ebp-0x3c], ax`. Diagnostics with `left` as the first
// member of an invented two-short struct (not keepable): with the bool timer 109 at retail's 1889 bytes (left and
// the flags are then bytes/words as retail, only the slot order differs); with the long timer 194 (1890 bytes);
// with the bool timer and the struct declared first 109, the tutorial choice inverted 109, renamed to sort first
// 109. The lead: retail's `left` lives in a 4-byte aggregate whose other half this function never touches.
// Screen only: left/top as x/y of one two-short point struct stores top into it too (retail never writes beside left).
// Round 128 (m3 lane 6, fast; control 114): on the bool timer (191, 1888 bytes) `left` is still stored as a
// dword (retail `mov word ptr [ebp-0x3c], ax`) and the scripted flag still packs at -2: `left` assigned in two
// statements (cast, then `-= g_4b9dd2`) 191; `left` initialised to 0 and reassigned (diagnostic) 191; the two
// rectangles and the text buffer declared before origin 191; origin declared at function scope 191 (all the same
// code: a second definition does not give `left` a home). The compile-only harness is far off here (276 aligned).
// Round 40 (lane 13, fast): `left = (short)origin.x - g_4b9dd2` 114 (same code); with the bool timer 191
// (1888 bytes, `left` still a dword spill); `short line_height` 115; `short font` 486 (1892 bytes); `long
// count` 197 (1883 bytes).
// Round 27 (lane 17, fast; control 115): 0x24ce9d's parameters from retail's setup order, the text last (left, top,
// bounds, result, text) 110 with 0x24ce9d still matched, top or bounds moved around it 110 (a gain of 5, not kept);
// (top, left, ...) forms 113-115 with 0x24ce9d lost; (left, text, top, ...) 114. Tutorial colour as a nested ternary,
// `!flag && bit ? a8 : a4` and a named pixel from an if-chain or if/else 115 each (same code); on the reorder, two unpack
// calls 209 (1890 bytes), a default pixel with one override 398 (1887 bytes). The 0x1fa30 calls load the text before the
// bounds in retail (its parameters swapped); that definition is in a contributor's file, left alone.
// J228: rectangle-helper orders bounds/result/top/text/left and top/bounds/result/text/left
// each 114; swapping the output order each 116; each with shared flash/post-pow row scale
// 275/276. Every reordered helper loses its match (4/10), so all contracts and bodies restored.
// Round 328: Carried message pointer with short/long/unsigned-long/word/byte row counters
// 453/420/420/420/417; ordinary end-pointer for/while and direct active-range traversal
// 322/322/322. Original indexed traversal restored; scalar, predicate and point homes remain.
// Round 346: script/row/status text-buffer consumer scope subsets, then all phase scopes:
// 120/121/121/121/121/120/114/114.
// Frame 0x264 agrees; predicate packing, left word and color/rectangle/scalar homes remain. Fast control
// 114; plain 115. Original executable body restored.
// Round 54 (m3 lane 14, fast; control 115): message height declared before the three flags 128; the status
// line position declared before the game time 122; text height as a short 340 (1895 B); the status duration
// as `value ? value : 1` 116; timer alpha as a compound `*=` 115 (same code). Retail's tutorial colour is
// `flag ? a4 : (bit ? a8 : a4)` with the a4 arm first and the bit test jumping back to it.
// Round 131 (m3 lane 8, fast with 0x24ce9d/0x24c80b/0x24cedf; 0x24ce9d matched each time; base 115 in both checks):
// `short top, left;` in one declaration 115 (same code); the timer flag as short 195 (1890 bytes), as dword 115 (same
// code); the tutorial flag as long 227 (1890 bytes); `left` declared before `top` with the bool timer 191 (1888 bytes).
// Retail keeps `left` as a word memory local at -0x3c beside `top`'s dword spill at -0x38; ours spills `left` as a dword
// at -0x50 and shifts font (-0x4c against -0x50), line height (-0x18 against -0x14) and player (-0x3c against -0x40).
// J260: Timed ratio/row power/status power double storage singles 486/322/222; pairs 486/483/316; all 492; immutable all 492; frames 0x270 versus 0x264. No exact match; real producer widths restored.
// NOT kept: no eligible native repair; opening executable source restored. View, packed predicates, left word, color and rectangle homes remain.
// Round 224 (m3 lane 3, fast; control 115; counts in brackets are aligned with the frame offsets masked: 19 for
// this body, 16 with the bool timer): `left` as the x of the existing two-short widget point with y never written
// and the bool timer 109 at retail's 1889 bytes [15] (a six-point gain through a type chosen for its size, not
// kept). On the bool timer (191, 1888 bytes): an unused `short` declared beside `left` 191, one assigned from
// `left` and never read 191 (both still a dword store); a named `short right` for the two right edges 492 (1887
// bytes), for the first edge only 494 (1893). With the widget point the flags sit at -2/-0xd/-0xe against retail's
// -0xd/-0xe/-0xf: only the scripted flag is above the bounds/origin slot where retail has it below. A repeated
// `bounds.top = top` store there changes nothing (109); the scripted test written out from the player fields at its
// second use 375 (1892); the scripted flag set in two steps 454 (1891). What is left once the slots are masked:
// the tutorial colour's arm order (retail a4 first) and two `mov eax, edi; push eax` argument loads (ours EDX).
// M3 lane 8 r144 (fast with 0x24ce9d, 0x24c80b and 0x24cedf, matched each time; control 115). With the bool timer: left and
// top as x and y of a two-short screen point, y written first and top a copy of it 516 (1899 bytes); only x written 109
// (1889); x written first, then y, top a copy of y 109 (the same code as only x: the y store is removed, so converting the
// origin to a whole integer point is a source that gives retail's word store); that with 0x24ce9d's text parameter last
// (retail's setup order) 104, 0x24ce9d still matched (the diff is build/r144/lead-24cf66.patch; not kept: the point's y is
// dead storage, a locals-grouping form without a match); the reorder alone 110; the reorder with the bool timer and plain
// shorts 186 (1888). On the 109 form: the tutorial colour as `flag ? a4 : bit ? a8 : a4` 109 (same code); the total height
// renamed to sort last or first 109/109 (same code: names do not order these homes); the total height declared before the
// player 471 (1885); the two rectangles declared before origin 109 (same code). Left on the 104 form: retail's homes in
// order are split flag, origin, scripted/tutorial/timer flags, line height, flash (shared with the row counter), total
// height, bounds, colour, top, the point, player, view, count, row height (its own home), font; ours packs the scripted
// flag at -2, shares the row height with the total height and gives the row counter its own home; and the tutorial
// colour's block order.
// Round 112 (m3 lane 9, plain): 114 -> 103 with two parameter reorders, NOT kept (coordinator 11-11: helper parameter order
// picked by score, kept only with a match); the bodies stay at main's order.
// 0x1fa30 as (bounds, text): m3_owner prints it as ours, though this note called its file a contributor's. Retail
// sets ECX (text) before EAX (bounds) at every call, which gives (bounds, text), 0x1fa50's own order. 114 -> 108.
// On that, 0x24ce9d as (left, top, bounds, result, text), round 27's order: 108 -> 103; two of its three calls
// then set up their arguments as retail does. 0x1fa30, 0x1fa50 and 0x24ce9d still match; 0x24cf66 is their only
// retail caller. With frame offsets masked, 14 lines differ: the left word store, the timer flag as a byte, the
// tutorial colour's block order and the third 0x24ce9d call's setup; the rest is homes. On the 103 form: bool
// timer 180 at 1888 (re-measure); the loop's `view->fade_time` read once into a named local 290 at 1892; the
// message-text arm with a named `message_text` for its three uses 103 (same code); the scripted test as nested ifs
// 103 (same code); the flash fraction clamp as a ternary 103 (same code); the loop's fade clamp as if/else-if 110.
// M3 lane 13 r92 (fast with 0x24ce9d, 0x24c80b, 0x24cedf and 0x1fa30, matched each time; control 114): retail's row counter
// shares the flash scalar's home (-0x18; flash is address-taken in both builds) where ours spills the counter at -0x44. The
// rows loop in a block of its own (the counter's scope then a sibling of the flash block; this build has no for-scope
// conformance) 114 (same code: spill homes do not follow lexical scope); that with the bool timer 191 (1888 bytes); the row
// height as a short 261 (1903 bytes); the row height declared once at the loop block's top 114 (same code); the qsort call
// inside the loop block 114 (same code).
// MiniMax S (11-15): no new form, no lower score (114): retail keeps the timer message as a byte at -0xf and the left
// coordinate as a word at -0x3c where ours stores a dword, and the frame homes of the row counter and the flash scalar differ.
// MiniMax D (11-16): no new form, no lower score (115 in the fast check): retail keeps the view pointer at [ebp-0x44]
// where this build keeps it at [ebp-0x3c], and retail's three message flags are bytes at -0xf, -0xe and -0xd while this
// build gives the timer flag a long and the others bytes at -0x14, -0xd and -2; making the timer flag a bool so the
// three pack measures 191 (as the round above it found), so the frame is left as it is.
// b9E (11-17) walk against retail, fast build on r839: 114 fast, 1889 bytes both; 0x24cf66 starts exactly where
// 0x24cedf ends (0x24cf63 ret 8), no padding or fall-through. Every field offset, jcc sense (NaN included: 0x24d130,
// 0x24d3ce, 0x24d3ec, 0x24d3f8), callee argument, loop/qsort shape and constant agrees with the code; 0x45de40 and
// 0x45e570 hold 1.9f and 0.4f promoted to double, 0x45dbc0 1.0f, 0x45dbbc 0.5f. No behaviour difference found.
// Retail-only details that compile the same: the scripted path stores alpha 1.0 at 0x24d0c2 and 0x24d0ec overwrites it;
// the timer path's pack_color4f call at 0x24d189 keeps its result unused. `top += (short)height` lost its cast (retail
// adds the dword at 0x24d32d): 114 fast, 115 in the r841 full build. Left: homes (timer byte 0x24d03b, left word
// 0x24cffc, view 0x24cf76, font 0x24cfc9), the tutorial colour's block order (retail falls into the a4 load at
// 0x24d1a2, ours puts it last), and 0x24ce9d's setup (retail pushes the text between the left and top loads at
// 0x24d2d6-0x24d2dd; ours loads top first through edx; 0x1fa30's ecx/eax loads at 0x24d2eb swap likewise).
// M3 lane 7 r176 (fast, control 115): all on the byte timer flag (191, 1888 B, re-measure): the origin in a block of its own
// with the top and left computed inside it 191; the origin declared after top and left 191; the text rectangle declared
// after the flags 191 (all the same code: scope and declaration order move no home); the scripted flag as a test-set 453
// (1886 B); the scripted test written out at its second use 375 (1891 B; the flag then lives in DL and the timer and
// tutorial bytes take -0xd/-0xe). On the byte-timer form the three flags already sit in retail's relative order (scripted
// above tutorial above timer); the one home out of place is the origin/text-rectangle slot: retail allocates it straight
// after the split flag (-0xc, flags at -0xd/-0xe/-0xf), ours after the scripted flag (-2, then the slot, then -0xd/-0xe).
// 463: text measurer left/top/both as short 184/181/182; with byte timer 206/203/205.
// View scoped to the end of the message rows, mutable/read-only 115/115. NOT kept: shorter
// coordinate forms and tied scoped forms leave the view, predicate and coordinate homes different.
// Status rectangle 115; row draw/measurement/both 172/115/172; banner draw/measurement 115/115.
// All phase-owned rectangles 162; with byte timer 208. NOT kept: ties or regressions against the 115 preview control; no native view or left-coordinate repair.
// MiniMax A (11-18): no new form, no lower score (114; 115 on this build's reduced image); the tutorial colour's condition in
// De Morgan form (`!flag_1384 && (flags_b6 & 1) ? color_a8 : color_a4`) compiles exactly as before (115), and holding the
// colour in a local so that it is loaded first and only then overwritten (retail's order) scores 398; what still differs is
// the stack homes (the timer byte, the left word, the view and the font), the tutorial colour's block order and 0x24ce9d's
// argument setup; reordering 0x24ce9d's parameters reaches 103 to 110 but changes a function's parameter order, so it is not
// applied.
// M3 lane 15 r99 (fast with 0x24ce9d, 0x24c80b, 0x24cedf and 0x1fa30, matched each time; control 115 fast, 114 in the full
// build): the three message flags as one three-bool array (timer, tutorial, scripted; retail's -0xf/-0xe/-0xd are consecutive
// bytes in that order) 241 (1888 B): the array homes above the origin slot at -8/-7/-6 where retail's bytes sit below it; the
// three flags declared ahead of the origin with the byte timer 191 (the byte-timer code unchanged: scripted still back-fills
// -2); the tutorial colour as an if / else-if / else of three unpack calls (a4, a8, a4) 214 (1890 B); left and the first top
// as the two elements of a `short position[2]` (the sibling 0x13954b's integer origin) with top a copy of the second and the
// byte timer 109 (1889 B, the two-short lead's code; a locals-grouping form, not kept), the same without the byte timer 194
// (1890 B); a second view pointer copied after the sort for the row loop's three reads 115 (same code); the split-screen flag
// as a long with the byte timer 496 (1893 B). In every form retail's split-screen byte keeps -1 to itself (-2..-4 unused)
// while ours packs the scripted flag at -2; declaration order, an array and the other flags' widths do not change that.
// The full build reads 115 once src/unknown_1ee340.cpp's constructor stores its vertices through the component accessor
// (this round's 0x1edd00 match) and 114 with that file as before (plain checks of both trees): the reload pair before the
// flag setup (`mov edi, [ebp - 0x64]` against retail's `mov ecx, [ebp - 0x40]`) follows the whole-image link, as in round 97.
// M3 lane 4 r222 (fast with 0x24ce9d, 0x24c80b, 0x24cedf and 0x1fa30, matched each time; control 115): the tutorial colour as
// `if (flag_1384) unpack(a4) else unpack(bit ? a8 : a4)` 214 (1890 B: the a4 load then comes first as in retail, but the shared
// call follows it where retail's follows the a8 load); that with g_510c94 read at each use 446 (1903 B); negated with the arms
// swapped 214 (1890 B); the first on the bool timer 151 (1889 B); the timed message colour read through g_510c94 at each use
// (no `timed` local) 416 (1890 B). Same code: `left` as `short const` on the bool timer (191); 0 to 5 dead locals ahead of the
// bool-timer body (homes unchanged, so not a symbol-count state). A one-object LTCG screen names the homes: retail shares the
// flash scalar with the row counter and the row fade with the total height, and gives the row height a cell of its own; on the
// bool timer ours shares the row height with the total height and the flash scalar with the row fade, and the row counter has
// its own cell. A scan of every matched retail function finds no other lone word-sized frame local like `left` (only an /Od one).
// M3 lane 1 r124 (fast with the file's other 21 functions, all MATCH each run; control 115, difflib-aligned 113): nearly
// every difference is a frame home. 0x24ce9d (same file, matched, no longer held) taking `left` and `top` as `short`
// 182 (1883 B; 0x24ce9d stays matched); `left` as a `short const` initialised where it is computed 115 (same code); the
// timer flag as a bool 191 (1888 B: timer -0xe, tutorial -0xd, scripted still packed at -2 beside the split-screen byte,
// where retail has -0xf/-0xe/-0xd below `origin`), with `origin` declared ahead of the split-screen flag 191 (same
// code); `origin` scoped to its own block around the top/left computation 115, with the bool timer 191 (same code each).
// M3 lane 16 r150 (fast with 0x24ce9d, 0x24c80b, 0x24cedf, 0x1fa30 and 0x15f120, all MATCH each run; control 115, 31 lines
// with the frame offsets masked): the flash scalar passed by value to an inline setter of the vertex procedure and its
// parameter 383 (1892 bytes); the tutorial colour from an inline function with three returns (a4, a8, a4) 442 at 1859
// bytes when the compiler leaves it out of line, forced inline 115 (same code); the tutorial colour as an if/else of two
// unpack calls, the second holding the bit ternary 214 (1890 bytes; the 1.0 loads move); the nested ternary with its first
// arm read through the global and the others through the local 115 (same code); the player taken from the HUD state
// local declared ahead of it 246 (1891 bytes). With the offsets masked the body differs from retail only in the left word
// store, the timer byte, the tutorial colour's block order (retail: flag test, the a4 load, the bit test jumping back
// to it, the a8 load) and the argument order at the 0x24ce9d and 0x1fa30 calls; every other line is a home. Retail's
// homes from EBP down: split flag, origin and bounds, scripted, tutorial, timer, line height, flash with the row counter,
// total height, text bounds, colour, top, left, player, view, count, row height, font; ours: split flag, scripted, origin
// and bounds, tutorial, timer with flash and the row height, line height, total height, colour, view, text bounds,
// player, top, row counter, count, font, left.
// MiniMax A (11-19): no new form, no lower score (114; 115 on this build's reduced image): what is left is the
// frame homes and two widths - retail writes its three message flags as consecutive bytes at -0xf, -0xe and -0xd
// where this build writes the scripted flag as a dword at -0x14, the tutorial flag as a byte at -0xd and the
// split-screen flag as a byte at -2; retail writes the left coordinate as a word at -0x3c where this build writes
// a dword at -0x50 (the spelling that narrows it groups the two coordinates into one array, a banned
// locals-grouping form); the view pointer sits at -0x44 against -0x30 here and the font at -0x50 against -0x4c.
// The tutorial colour's block order and 0x24ce9d's argument setup are the only other differences.
// m3 lane 11 r130 (fast with 0x24ce9d, 0x24c80b, 0x24cedf and 0x1fa30, matched each time; control 115 fast, 114 full):
// text_bounds renamed to sort first 115 (identical code); the three message flags and the message block in an inner
// block of their own, with the bool timer 463 (1883 B: the flags leave memory) and with the long timer 128; the view
// pointer typed const 115 (same code); the timer flag as `char` 191 (1888 B, the byte timer's code). The sibling
// 0x13954b converts its origin into a `short origin[2]` it passes to a call; here no call takes the pair, so that
// spelling stays the locals-grouping form listed above.
// M3 lane 5 r215 (fast with 0x24ce9d, 0x24c80b, 0x24cedf and 0x1fa30, matched each time; control 115 fast, 114 in the
// baseline full check): the bool timer re-measured 191 (1888 bytes, aligned 111 against the control's 113: the flags
// then sit at -0xe, -0xd and -2 for retail's -0xf, -0xe, -0xd). Retail's word store means another two-byte local shares
// `left`'s cell, so with the bool timer the function's other short locals were declared beside it: the fade tick count
// declared after `left` (assigned where it was) 191; between `top` and `left` 191; the row index declared after `left`
// 191; the count declared after `left` 191; all three 191; `left`, the tick count, `top` in that order 191 (six forms,
// all the bool timer's code: `left` is still stored as a dword at -0x4c and no short is packed with it; the homes do not
// follow where the shorts are declared).
// M3 lane 12 r160 (fast with 0x24ce9d, 0x24c80b, 0x24cedf and 0x1fa30, all MATCH; control 114): use-count forms to move
// the spill homes: the fade time read once into a local used twice 293 (1892 bytes); the elapsed fade time named 114
// (same code); the status player taken from `player` 535 (1861 bytes); the timer flag read through the global 114 (same
// code); the tutorial colour as three arms each calling the unpack 215 (1890 bytes).
// M3 lane 7 r197 (fast with 0x24ce9d, 0x24c80b, 0x24cedf, 0x1fa30 and 0x15f120, matched each time; control 114, aligned
// 102). On the bool timer (191, 1888 B): the four flags renamed so that their names sort in each of four orders 191 each
// (same code: the byte homes do not follow names); the three message flags declared without initialisers after the
// split flag, after `origin`, after `text`, at the top of the block, and in reverse order after the split flag and at the
// top 191 each (same code). On the kept body: the message height declared ahead of the message loop 114, the row counter
// declared ahead of it 114, both 114, the flash value declared at the top of the timer arm 114 (same code each). Retail
// shares the flash value's home with the row counter (-0x18) and gives the message height its own (-0x4c); ours shares
// the timer, flash and message height (-0x14): the packing follows the lifetimes, not the declarations.
// MiniMax C (11-20): no lower score (114): the frame homes and the left coordinate word store differ; a word for left scores 482.
// M3 lane 6 r217 (fast with 0x24ce9d, 0x24c80b, 0x24cedf and 0x1fa30, matched each time; control 114): one local for
// g_510c94 across the timed, tutorial and default colour arms 480 (1868 B); the row loop's active test in the for
// condition 316 (1892 B); the split-screen test as `count >= 2` 116, with the bool timer 193 (flags still -0xe/-0xd/-2);
// the status block reusing `player` for its second lookup 535 (1861 B). Diagnostic: an extra read of origin.x 491, the
// scripted byte stays at -2 (the packing does not follow origin's reference count). Body unchanged.
// M3 lane 11 r141 (fast with 0x24ce9d, 0x24c80b, 0x24cedf, 0x1fa30 and 0x15f120, matched each time; control 114, aligned
// differing rows 102, 18 with the frame offsets masked, 12 with registers masked too; body unchanged). LEAD for the
// tutorial colour, the first spelling that gives retail's block (flag test, the a4 load, the bit test jumping back to
// it, the a8 load): the selection nested with the inner test negated, `flag ? a4 : (!(bit) ? a4 : a8)`, 108 (masked 15
// and 9; six points, under the keep bar by itself). Same code as the body: the inner arm's a4 read through the global
// 114, the whole selection inverted (`!flag ? (bit ? a8 : a4) : a4`) 114. Other instructions: the flag folded into a
// bool selection first, `(flag ? true : !(bit)) ? a4 : a8` 406 (1898 B), `(flag ? false : (bit) != 0) ? a8 : a4` 403
// (1896 B). On the 108 form: `left` as a `short const` initialised where it is computed 108 (same code); the bool timer
// 185 (1888 B, masked 13 and 6); the bool timer with left and the first top as the two elements of a `short
// position[2]` (round 99's locals-grouping lead) 103 at retail's 1889 B (masked 12 and 5: what is left with the offsets
// masked is the argument order at the three 0x24ce9d and four 0x1fa30 calls); that lead without the nested selection
// 109 (re-measure). The frame homes are still the open part: retail allocates split flag, origin, then the scripted,
// tutorial and timer bytes, where this build puts the scripted byte between the split flag and the origin.
// M3 lane 7 r205 (H2M fast; control 114): round 141's lead re-measured, the bool timer, `left` and the first top as the
// two elements of `short position[2]` (x written first, top a copy of y) and the nested tutorial selection 103 (1889 B; with
// y written first 516 at 1899 B); with it 0x1fa30 as (bounds, text) and 0x24ce9d as (left, top, bounds, result, text)
// (round 112's orders, both callees still matched) 92 at 1889 B, the lowest so far. With the frame offsets masked only six
// rows differ on the 92 form (the player/offset reload pair, which follows the whole image, and the third 0x24ce9d
// call's setup); everything else is homes: ours scripted -2, tutorial -0xd, timer -0xe, total height with the row height
// -0x18, colour -0x28, flash with the row fade -0x2c, text bounds -0x34, player -0x38, view -0x3c, font -0x40, row counter
// -0x44, the point -0x4c, top -0x50; retail scripted -0xd, tutorial -0xe, timer -0xf, flash with the row counter -0x18,
// total height with the row fade -0x1c, text bounds -0x24, colour -0x34, top -0x38, the point -0x3c, player -0x40, view
// -0x44, row height -0x4c, font -0x50. On the 92 form: a long row counter 419 (1877 B); the loop's game time read ahead of
// the active test 326 (1880 B); the HUD state read through the global at each use 92 (same code); the count as
// `split_screen ? 3 : 4` 479 (1886 B); one colour pair declared at the block top for all three draws 109 (1889 B). Not
// kept (steering forms without a match); the 92 form is saved as build/r205/lead-24cf66-92.patch.
// M3 lane 11 r145 (H2M fast with 0x24ce9d/0x24c80b/0x24cedf/0x1fa30/0x15f120, all MATCH each run; control 114): the
// tutorial colour as `flag ? a4 : (bit ? a8 : a4)` 114 (same code as the body); as `(bit) && !flag ? a8 : a4` 116; as
// `!flag && (bit) ? a8 : a4` 114 (same code); the timer fraction clamp as `fraction < 1.0f ? fraction : 1.0f` 116; the
// fade test as `fade_ticks > ticks` 116. Reading of lane 7 r205's 92 lead (re-applied, 92, no difference left but homes):
// retail, from EBP down: split -1, origin/bounds -0xc, scripted -0xd, tutorial -0xe, timer -0xf, line height -0x14, flash
// sharing with the row counter -0x18, total height sharing with the row fade -0x1c, text bounds -0x24, colour -0x34, top
// -0x38, left -0x3c, player -0x40, view -0x44, count -0x48, row height -0x4c, font -0x50, colour pair -0x60, player offset
// -0x64; the lead: split -1, scripted -2, origin/bounds -0xc, tutorial -0xd, timer -0xe, line height -0x14, total height
// sharing with the row height -0x18, colour -0x28, flash sharing with the row fade -0x2c, text bounds -0x34, player -0x38,
// view -0x3c, font -0x40, row counter -0x44, count -0x48, left -0x4c, top -0x50 (the rest as retail). So the open part is
// which loop scalar shares with flash and with the total height. Body unchanged.
// M3 lane 1 r149 (H2M fast; control 114): the timer as a bool 191 (1888 B: its byte lands at -0xe, the tutorial byte at
// -0xd and the scripted byte at -2, retail -0xf/-0xe/-0xd: the three keep retail's relative order but the scripted byte
// is placed ahead of origin, in the split flag's padding); that with the three flags declared scripted, tutorial, timer
// and assigned after 191 (same code); origin declared first of the locals 114 (same code); the three flags computed
// after `height` 463 (1883 B); the late scripted test as `!player->value_440 && scripted_message` 117; the count as
// `split_screen ? 3 : 4` 479 (1886 B, re-measured on main's body).
// M3 lane 13 r137 (h2m fast with 0x24ce9d, 0x24c80b, 0x24cedf, 0x1fa30, 0x15f120, all MATCH; control 114): the row's
// message height as a short 261 (1903 B); the status duration defaulted by `if (!duration) duration = 1;` 115; the
// row fade clamped by if/else on a copy of the remainder 121; the status bounds' top stored before the bottom 114 (same
// code); the text height tested and assigned without its local 114 (same code). Homes and predicate packing remain.
// M3 lane 7 r215 (H2M_FAST with 0x24ce9d, 0x15f120, 0x24cedf, all MATCH; control 114). The bool timer re-measured:
// 191 at 1888 B, `left` still a dword store. On it, going after the two bytes above retail's word `left` with a second
// short local that never needs memory: the right edge named in a short local declared after `left` 191, declared
// ahead of `left` 191, declared at its assignment 191, the bottom edge named the same way 191 (the same code each:
// a short that stays in a register reserves no home, so it cannot be what shares `left`'s cell); the scripted flag
// computed ahead of the tutorial flag 460 (1883 B). In both builds the byte flags keep one order from EBP down
// (split screen, scripted, tutorial, timer); retail's bounds/origin home sorts above the scripted flag where ours
// sorts below it, which is why ours back-fills the byte beside the split-screen flag. Body unchanged.
// M3 lane 8 r228 (H2M_FAST with 0x24ce9d/0x24c80b/0x24cedf/0x1fa30/0x15f120, all MATCH; control 114, 115 in the full
// baseline): the messages sorted ahead of `top += height` 122; the total height as a short 114 (same code); the row
// count as `4 - split_screen` 479 (1886 B); `keep = 1.0f - flash` named for all six colour rows 396 (1885 B); the fade
// tick count as a short 437 (1896 B). Body unchanged.
// M3 lane 11 r161 (H2M_FAST with 0x24ce9d/0x24c80b/0x24cedf, all MATCH; control 114): `origin` and the top/left
// computation in their own block 114 (same code), that with the bool timer 191 (1888 B); no `hud` local 115; the
// player lookup ahead of `split_screen` 504 (1895 B); the timer flag set by an `if` after a zero default 115. Body
// unchanged.
// M3 lane 15 r142 (H2M_FAST with 0x24ce9d/0x15f120/0x24cedf, all MATCH; control 114). Frame reading (this frame is over
// 0x80 bytes, so slots sort by references x 1000 / size): our colour slot (color + both field_24, 25 refs, 1562) sorts
// above the text-bounds slot (12 refs with three temporaries and the status game time, 1500), retail's the other way;
// retail's three flag bytes sit below the bounds/origin slot although its scripted flag also has four references.
// Forms: the first bottom from `bounds.top` 114; the status top stored first and the bottom from it 114; the status
// step as `bounds.top += 0x12; bounds.bottom += 0x12;` 114; the text height tested and assigned as an expression 114
// (all the same code); the total height as `height = text_height >= 0 ? text_height : 0;` 349 (1896 B).
// M3 lane 11 r165 (H2M_FAST with 0x24ce9d, 0x24c80b, 0x24cedf, 0x1fa30 and 0x15f120, all MATCH; control 114, aligned 102;
// plain check agrees). Frame reading (the allocator's packing view against retail's cell order): every home retail has
// follows from one more packing reference on `top`, `left`, the row height and the text bounds and four more on
// `bounds`, and all of those sit in the row's plain-text arm, so that arm existed twice when the frame was packed (the
// /O1 build merges the identical tails afterwards). Kept: the row written as `if (counted) { if (count) formatted; else if
// (has text) plain; } else if (has text) plain;` 80 (1889 B, aligned 68; same row logic). On round 205's 92 lead
// (bool timer, `short position[2]`, the nested tutorial selection, 0x1fa30 as (bounds, text) and 0x24ce9d as (left, top,
// bounds, result, text)) the same row form gives 2 at 1889 B: every home is retail's and only the two reloads before
// the split-screen test come in the other order (player, then the player offset in retail); saved as
// build/r165/lead-0x24cf66-2.patch, not kept (the array, the nested selection and the two reorders are match-only, the
// reorders were reverted at review before). Pieces on that lead: main's tutorial selection 8, main's parameter orders 15,
// the `long` timer 140 (1890 B), `short left` in place of the array 85 (1888 B). Kept form with the bool timer 104 (1888 B),
// the bool timer alone 191 (re-measured). For the last pair on the 2 lead: the count step ahead of the text height 64
// (1888 B), the sort ahead of `top += height` 12, `--count` 2, `count` declared ahead of the player 21, the HUD state
// local ahead of the player 2, `&player->messages[0]` 2, the row index declared outside the `for` 2, the status lookup
// through a copied index 2, the player taken from the HUD state local (`&hud->players[index]`) 139 (1891 B); 1 to 3
// unused locals ahead of the player 2 (diagnostic). The allocator's forced decisions (priorities, picks, costs, flags)
// leave the pair; only a forced scheduler height gives the match: the two reloads tie and keep code-generation order,
// which follows the candidates' numbers (the cut walks run in ascending number and the later walk's reload is emitted
// first; ours numbers the player before the offset). Permuter 600 s on the lead: nothing below 2 (51 tries).
// M3 lane 6 r242 (H2M_FAST with 0x24ce9d, 0x24c80b, 0x24cedf, 0x1fa30 and 0x15f120, all MATCH; control 80, aligned 70; body
// unchanged). Lane 11 r165's 2 lead rebuilt on this tree (build/r242/lead-0x24cf66-2.patch; the r165 patch no longer applies):
// 2 at 1889 B, the two reloads ahead of the split-screen test in the other order. They tie in the scheduler and keep the order
// in which the allocator's last pass created their two pieces: the player's first, then the `index * 0x4e0` offset that the
// first player lookup and the status lookup share (retail the other way round). On the lead: no `player` local with the
// getter at each of its eight uses 513 (1904 B, frame 0x25c: retail keeps a home for the player); the player defined after
// the timer and tutorial flags 487 (1888 B); the getter with a state-pointer local, with a result local, as `players + index`
// or forceinline 2 each (same code); the status lookup assigned to the same `player` local 120 (the homes move).
// M3 lane 2 r149 (H2M_FAST with 0x24ce9d, 0x24c80b, 0x24cedf, 0x1fa30 and 0x15f120, all MATCH; main's body 81 in the full
// baseline). Lane 11 r165's lead applied from lane 6 r242's patch: 2 at 1889 B (the player and player-offset reloads ahead
// of the split-screen test in the other order). On it: the count step ahead of the text height re-measured 64 (a listed
// form); the height as `height = text_height >= 0 ? text_height : 0;` 450 (1887 B); a named `messages` pointer for the sort
// 2; no HUD state local (the global read at each use) 2; the fade ticks read from the player at both uses (no `ticks`
// local) 2 (same code each); the sort through a second player lookup 455 (1890 B). The pair is a code-generation tie
// (lane 11 r165, lane 6 r242); none of these moves the two values' numbering. Body unchanged.
// GB9 (fast with 0x24ce9d, 0x24c80b, 0x24cedf, 0x1fa30, 0x1fa50, 0x15f120, 0x13edb0, all MATCH throughout; control 80;
// 111 fast trials + 1 plain check on the score-2 lead [bool timer_message, s_widget_point position, nested tutorial colour,
// 0x1fa30 as (bounds, text), 0x24ce9d as (left, top, bounds, result, text)], body unchanged):
// Plain full-image check on today's tree also reads score 2 at +0x3bd (1889/1889 B, all 7 peers MATCH).
// Probing what flips the +0x3bd reload pair (`mov edi, [ebp-0x64]; mov ecx, [ebp-0x40]` vs retail `mov ecx; mov edi`):
// sharing BOTH color and field_24 across all 3 blocks flips +0x3bd to retail (`65` at 1889 B, first 260 insns identical to
// sharing field_24 alone `60` which stays swapped); moving `message_height = 0` before `if (age > view->fade_time)` flips
// +0x3bd to retail (`34` at 1889 B, exact match through +0x45f) and moving it after 0x13edb0 also flips +0x3bd (`202` at
// 1890 B, exact through line 334); `font` via `if (split_screen) font = 5` (`181`) and `active` in `for` condition (`258`)
// also flip +0x3bd. Every home-and-code-preserving variation tested stayed at score 2 (+0x3bd swapped): sharing only field_24
// across blocks 2+3, swapping color/field_24 declarations, sharing game_time or fraction, dropping hud+ticks+text_height (up
// to 5 fewer locals), renaming player/status_player/flags/bounds, direct pointer/reference player lookups, split/nested/early-return
// outer guards, explicit `!= 0`/byte-ternary timer, hoisting declarations, 0x24ce9d parameter permutations or moving 0x24ce9d
// after 0x24cf66, `color.blue = color.green`, non-compound alpha, `message_text`/`status_text` locals, `short height`, inline
// `fraction` decl, dropping tutorial `globals` local, named `pixel`/`ratio` locals, commuted top add, `duration = 1; if (...)`,
// `fraction` ternary, `while`/`++i` loop, nested scripted/else blocks, and B3 bounds order. Explicit `player_offset` local
// used twice scored 9 (+0xb8, `lea ecx, [esi + edi]`, still swapped at +0x3bd), proving front-end symbol order does not
// control the tie; only IR live-range changes flip +0x3bd. Body and callees restored to main (score 80).
// The view and left coordinate homes still differ. Naming the status start, duration or end scores
// 80/81/80. Start plus duration, start plus end, duration plus end and all three score 80/80/81/80.
// Bounding the shared duration in place scores 108 at 1885 bytes. The view's initial spill and the 0x264
// frame stay fixed.
// M3 lane GA r6 (fast with 0x24ce9d, 0x24c80b, 0x24cedf, 0x1fa30, 0x15f120, all MATCH; control 80 at 1889/1889 B; 79
// compiled fast trials + 1 plain check). Existing `s_widget_point position` (include/screen_widgets.h) reproduces the 2-diff
// lead at 1889/1889 B (+0x3bd, `mov edi, [ebp-0x64]; mov ecx, [ebp-0x40]` vs retail `mov ecx; mov edi`) in both `--fast` and
// a plain full check (all 24 peers including 0x1fa30/0x1fa50/0x24ce9d MATCH; still 2, not kept as parameter reorders and
// `s_widget_point` grouping are match-only). A/B/C isolation on main's 80 base shows `bool timer_message` alone flips the
// +0x3bd reload pair to `mov edi; mov ecx`: `s_widget_point` alone 158, nested tutorial alone 74 (-6 pts, under bar), both
// 152 (all keep retail `mov ecx; mov edi`), while `bool timer_message` alone 104, + `s_widget_point` 21, + nested tutorial
// 98 (all flip to `mov edi; mov ecx`), and `short timer_message` on the lead 141 restores `mov ecx; mov edi`. On the
// `s_widget_point` lead: explicit `long player_offset` before `player` 9 (1889 B, still `mov edi; mov ecx`), `player` ref /
// `byte` flags / `s_hud_player` direct indexing / early return / nested `if (view)` / C89 top declarations / shared
// `game_time` / shared `field_24` across blocks 2+3 / dropping `globals`, `duration`, `ticks`, `text_height`, `hud`, inline
// clamp 2 each; `timer_message` via `if` 11, ternary 4; `position.y` before `.x` 516; `count = split_screen ? 3 : 4` 477;
// no `player` local 500; `status_player` dropped 386 / moved before `if` 408; sharing both `color` and `field_24` across all
// three blocks flips +0x3bd back to retail `mov ecx, [ebp-0x40]; mov edi, [ebp-0x64]` (65 at 1889 B) even with `bool
// timer_message`, while moving `color` above `text_bounds` (-0x2c vs -0x34); sharing only one or only blocks 2+3 leaves
// `mov edi; mov ecx` (60/67/12/2). Body unchanged.
// @retail 0x24cf66
void function_24cf66(long local_player_index)
{
	s_hud_globals_definition *view = function_24c80b();

	if (view && local_player_index != NONE && !function_13cb40() && function_161b60(function_14de70(local_player_index)))
	{
		bool split_screen = ((s_local_player_table_view *)g_4e8c20)->count > 1;
		long font = split_screen ? 5 : 6;
		long line_height = function_122540(font);
		point2f origin;
		short top;
		short left;
		short_rectangle2d bounds;
		short_rectangle2d text_bounds;
		word text[0x100];

		function_1396c7(1, &origin);
		top = (short)((long)origin.y - g_4b9dd0 + 0x3c);
		left = (short)((long)origin.x - g_4b9dd2);
		if (split_screen)
			top -= 0x19;

		s_hud_player *player = hud_player_get(local_player_index);
		short count = 4 - (split_screen != 0);
		s_hud_state *hud = g_5023f4;
		long timer_message = hud->element_1390 && hud->value_1394;
		bool tutorial_message = g_510c98->flag && hud->element_138c;
		bool scripted_message = player->flag_444 && (player->value_440 || player->text[0]);
		long height = 0;

		if (timer_message || tutorial_message || scripted_message)
		{
			color4f color;
			color4f field_24;
			if (timer_message)
			{
				s_hud_timed_message_definition const *timed = &g_510c94->timed_message;
				real fraction;

				unpack_color4f(timed->color, &color);
				fraction = (real)hud->value_1394 / (real)timed->value_1e;
				color.alpha = (fraction > 1.0f ? 1.0f : fraction) * color.alpha;
				pack_color4f(&color);
			}
			else if (tutorial_message)
			{
				s_hud_globals_definition *globals = g_510c94;

				unpack_color4f(hud->flag_1384 || !(globals->flags_b6 & 1) ? globals->color_a4 : globals->color_a8, &color);
			}
			else
			{
				unpack_color3f(g_510c94->color_a4, (color3f *)&color.red);
				color.alpha = 1.0f;
			}

			*(s_color_bits *)&color.red = *function_13927e(local_player_index);
			color.alpha = function_1392a9(local_player_index);
			long fade_ticks = g_510c54->field_2_3 / 4;
			*(color3f *)&field_24.red = *(color3f *)g_468718;
			field_24.alpha = color.alpha;

			short ticks = player->ticks_446;
			if (ticks < fade_ticks)
			{
				real fraction = (real)ticks / (real)fade_ticks;
				if (fraction < 0.0f)
					fraction = 0.0f;
				else if (fraction > 1.0f)
					fraction = 1.0f;

				real flash = 1.0f - fraction;

				color.red = (1.0f - flash) * color.red + flash;
				color.green = (1.0f - flash) * color.red + flash;
				color.blue = (1.0f - flash) * color.red + flash;
				field_24.red = (1.0f - flash) * field_24.red + 0.5f * flash;
				field_24.green = (1.0f - flash) * field_24.red + 0.5f * flash;
				field_24.blue = (1.0f - flash) * field_24.red + 0.5f * flash;
				g_4e73a0.vertex_proc = function_24cedf;
				g_4e73a0.vertex_proc_parameter = *(long *)&flash;
			}

			bounds.left = left;
			bounds.right = g_4b9dd8.right - g_4b9dd2;
			bounds.top = top;
			bounds.bottom = top + line_height * 5;
			text_bounds = bounds;
			function_13edb0(font, NONE, 0, 0, &color, &field_24);
			function_13eb60(&color);

			if (scripted_message && !player->value_440)
			{
				unicode_string_copy(text, player->text, 0x100);
				function_22d2ee(text, 0x100);
				if (function_13ee20(text, g_4e73a0.font))
				{
					function_24ce9d(text, left, top, &bounds, &text_bounds);
					function_1fa30(text, &bounds);
				}
			}

			g_4e73a0.unknown5e = 0;
			g_4e73a0.unknown60 = 0;
			g_4e73a0.vertex_proc = 0;
			g_4e73a0.vertex_proc_parameter = 0;

			long text_height = text_bounds.bottom - top;
			if (text_height >= 0)
				height = text_height;
			if (split_screen)
				count--;
		}

		top += height;
		qsort(player->messages, 4, sizeof(s_hud_message), hud_message_compare);
		for (short i = 0; i < count; i++)
		{
			color4f color;
			color4f field_24;
			s_hud_message *message = &player->messages[i];

			if (!message->active)
				break;
			long game_time = g_510c54->game_time;

			if (function_163040(local_player_index))
				message->time++;

			real age = (real)(game_time - message->time) * g_510c54->rate;

			*(s_color_bits *)&color.red = *function_13927e(local_player_index);
			color.alpha = function_1392a9(local_player_index);
			field_24 = *(color4f *)g_4686d4;
			field_24.alpha = function_1392a9(local_player_index);

			if (age > view->fade_time)
			{
				real remaining = 1.0f - (age - view->fade_time) / view->display_time;
				real fade = remaining < 0.0f ? 0.0f : (remaining > 1.0f ? 1.0f : remaining);

				real scale = pow(fade, 1.9f);
				color.alpha *= scale;
				field_24.alpha *= scale;
			}

			long message_height = 0;
			function_13edb0(font, NONE, 0, 0, &color, &field_24);

			if (message->counted)
			{
				if (message->count)
				{
					unicode_string_copy(text, message->text, 0x100);
					((s_510c4c_view *)g_510c4c)->text_count = message->count;
					function_22d2ee(text, 0x100);
					if (function_13ee20(text, g_4e73a0.font))
					{
						function_24ce9d(text, left, top, &bounds, &text_bounds);
						message_height = (short)(bounds.bottom - bounds.top);
						function_1fa30(text, &bounds);
					}
				}
				else if (function_13ef30(message->text))
				{
					function_24ce9d(message->text, left, top, &bounds, &text_bounds);
					message_height = (short)(bounds.bottom - bounds.top);
					function_1fa30(message->text, &bounds);
				}
			}
			else if (function_13ef30(message->text))
			{
				function_24ce9d(message->text, left, top, &bounds, &text_bounds);
				message_height = (short)(bounds.bottom - bounds.top);
				function_1fa30(message->text, &bounds);
			}

			top = (short)((real)message_height * view->line_spacing + (real)top);
		}

		color4f color;
		color4f field_24;
		s_hud_player *status_player = hud_player_get(local_player_index);
		field_24 = *(color4f *)g_4686d4;
		long game_time = g_510c54->game_time;
		long local_2 = g_4b9dd8.bottom - g_4b9dd0 - 0x28;
		*(s_color_bits *)&color.red = *function_13927e(local_player_index);
		bounds.left = g_4b9dd8.left - g_4b9dd2;
		bounds.right = g_4b9dd8.right - g_4b9dd2;
		bounds.bottom = local_2 + line_height * 5;
		bounds.top = local_2;

		if (game_time >= status_player->time_4d8 && game_time < status_player->time_4d8 + status_player->value_4dc)
		{
			long duration = !status_player->value_4dc ? 1 : status_player->value_4dc;
			real fade = pow(1.0f - (real)(game_time - status_player->time_4d8) / (real)duration, 0.4f);

			color.alpha = function_1392a9(local_player_index) * fade;
			field_24.alpha = function_1392a9(local_player_index) * fade;
			function_13edb0(font, NONE, 2, 0, &color, &field_24);
			if (function_13ef30(status_player->text_454))
				function_1fa30(status_player->text_454, &bounds);
		}

		bounds.bottom += 0x12;
		local_2 += 0x12;
		bounds.top = local_2;
		*(s_color_bits *)&color.red = *function_13927e(local_player_index);
		color.alpha = function_1392a9(local_player_index);
		field_24.alpha = function_1392a9(local_player_index);

		if (function_15f120(function_14de70(local_player_index), text, 0x100, 1))
		{
			function_13edb0(font, NONE, 2, 0, &color, &field_24);
			if (function_13ee20(text, g_4e73a0.font))
				function_1fa30(text, &bounds);
		}
	}
}
