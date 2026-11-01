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

// score 6 (was 6): the tick counter's address is kept in ECX where retail keeps it in EAX, so the
// counter is read into EAX and written back from there.
// register value/ticks/both 6 each; const value/ticks/player 6 each;
// existing player accessor 6; function-scope message/index 6 each; short index 6.
// Indexed player fields 26; tick address from state and index 16; tick address from global 56;
// tick pointer name 6; increment result name 6; both role names 6; minimum intrinsic 6;
// nested countdown guards 6; messages indexed at each use 6; the field read twice in the
// saturation 27; a short reference for the ticks 6; nested expiry tests 6; a named elapsed time 6.
// Round 19: the countdown as a forceinline helper 6; the tick and expiry update as a forceinline
// helper taking the player 6; early-return guards 6; the tick address from a forceinline accessor 6;
// the tick locals declared at the function top 6. Diagnostics: one pointer variable for both
// counters 18 (the pointer takes ECX in both); without the message loop the tick block keeps
// ECX/EAX; the value variable shared with the countdown 40 (split again, tick block unchanged).
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
		*ticks = (short)(value > 0x7fff ? 0x7fff : value);

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
// score 114 (was 115): frame 0x264 agrees; predicate packing, color/rectangle/scalar homes and the text-buffer call convention still differ.
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
// Round 40 (lane 11, fast; unchanged source 115 fast, 114 plain): retail stores `left` once with a word move
// (`mov word ptr [ebp-0x3c], ax`), so it is a memory local there, not a spilled register (ours spills it as a
// dword); with the bool timer that word store is retail's missing byte (the bool form is otherwise retail's
// flag code, bytes at -0xf/-0xe/-0xd but for the scripted flag at -2). Retail frame from EBP down: split
// screen, bounds/origin, the three flags, line height, loop index with the flash scalar, height, text bounds,
// colour, top (spilled in the loop), left, player, view, count, message height, font, field_24. Tried: one
// colour pair at function scope 133; the message height declared at the block top 115; the bool timer 191
// (1888 bytes); `(void)&left;` (diagnostic) 115 (optimized away, still a dword spill); the loop index at the
// block top / first 115/115; bool timer with the loop index at the block top 191.
// Round 16 (m3 lane 17, fast; unchanged source 115 fast): `left` renamed to sort first 115 (same code: it
// is a spill slot, so slot-name ordering does not reach it); `left` declared before `top` 115 (same); the
// HUD state global read at each use 115 (same); the text buffer declared first 115 (same); `top` as a long
// 499 (1882 bytes). In the bool-timer form the flags keep retail's relative order (split screen, scripted,
// tutorial, timer from EBP down); only the scripted flag back-fills the byte beside split screen (-2)
// where retail starts a new group after origin (-0xd).
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

		top += (short)height;
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

			if (message->counted && message->count)
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
