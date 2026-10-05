// @flags /O1 /Ob1 /arch:SSE /Gr
/* UNKNOWN_24C7C1.CPP: list walkers and the state of the first-person HUD
   (g_5023f4) */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_19b516.h"
#include "unknown_13fd90.h"
#include "unknown_030290.h"
#include "unknown_13927e.h"

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
	s_hud_globals_definition *view = g_510c94;

	return view ? (s_hud_globals_definition *)view->unknown00 : 0;
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
			hud->value_1394 = view->value_ea + view->value_e8;
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
		long delta;

		if (flag)
		{
			delta = (word)(g_5023f4->time_1398 - g_510c54->game_time);
		}
		else
		{
			delta = (short)(g_510c54->game_time - g_5023f4->time_1398);
		}
		g_5023f4->ticks_139c = delta + ticks;
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
		player->time_450 = g_510c54->game_time;
		player->flag_4d2 = true;
		player->sequence_4d3 = g_5023f4->next_sequence++;
		player->time_4d8 = g_510c54->game_time;
		player->value_4dc = value;
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

			vertex[0] = vertex[0] + (vertex[0] - center_x) * scale * 0.5f;
			vertex[1] = vertex[1] + (vertex[1] - center_y) * scale * 0.5f;
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
extern short g_4b9dd0;
extern short g_4b9dd2;

/* the frame the HUD draws in */
short_rectangle2d g_4b9dd8;

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
void function_24ce9d(word const *text, short left, short top, short_rectangle2d *bounds, short_rectangle2d *result)
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
		color4f color;
		color4f field_24;
		word text[0x100];

		function_1396c7(1, &origin);
		top = (short)((long)origin.y - g_4b9dd0 + 0x3c);
		left = (short)((long)origin.x - g_4b9dd2);
		if (split_screen)
			top -= 0x19;

		s_hud_player *player = hud_player_get(local_player_index);
		short count = 4 - split_screen;
		s_hud_state *hud = g_5023f4;
		bool timer_message = hud->element_1390 && hud->value_1394;
		bool tutorial_message = g_510c98->flag && hud->element_138c;
		bool scripted_message = player->flag_444 && (player->value_440 || player->text[0]);
		long height = 0;

		if (timer_message || tutorial_message || scripted_message)
		{
			if (timer_message)
			{
				s_hud_globals_definition *globals = g_510c94;
				real fraction;

				unpack_color4f(globals->color_cc, &color);
				fraction = (real)hud->value_1394 / (real)globals->value_ea;
				if (fraction > 1.0f)
					fraction = 1.0f;
				color.alpha = fraction * color.alpha;
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
			*(color3f *)&field_24.red = *(color3f *)g_468718;
			field_24.alpha = color.alpha;

			short ticks = player->ticks_446;
			long fade_ticks = g_510c54->field_2_3 / 4;
			if (ticks < fade_ticks)
			{
				real fraction = (real)ticks / (real)fade_ticks;
				if (fraction < 0.0f)
					fraction = 0.0f;
				else if (fraction > 1.0f)
					fraction = 1.0f;

				real flash = 1.0f - fraction;
				real rest = 1.0f - flash;

				color.red = color.red * rest + flash;
				color.green = color.red * rest + flash;
				color.blue = color.red * rest + flash;
				field_24.red = field_24.red * rest + 0.5f * flash;
				field_24.green = field_24.red * rest + 0.5f * flash;
				field_24.blue = field_24.red * rest + 0.5f * flash;
				g_4e73a0.vertex_proc = function_24cedf;
				g_4e73a0.vertex_proc_parameter = *(long *)&flash;
			}

			bounds.top = top;
			bounds.left = left;
			bounds.bottom = top + line_height * 5;
			bounds.right = g_4b9dd8.right - g_4b9dd2;
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

		short y = top;
		for (short i = 0; i < count; i++)
		{
			s_hud_message *message = &player->messages[i];

			if (message->active)
			{
				s_game_time_globals *game_time_globals = g_510c54;
				long game_time = game_time_globals->game_time;

				if (function_163040(local_player_index))
					message->time++;

				real age = (real)(game_time - message->time) * game_time_globals->rate;

				*(s_color_bits *)&color.red = *function_13927e(local_player_index);
				color.alpha = function_1392a9(local_player_index);
				field_24 = *(color4f *)g_4686d4;
				field_24.alpha = function_1392a9(local_player_index);

				if (age > view->fade_time)
				{
					real fade = 1.0f - (age - view->fade_time) / view->display_time;
					if (fade < 0.0f)
						fade = 0.0f;
					else if (fade > 1.0f)
						fade = 1.0f;

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
						function_24ce9d(text, left, y, &bounds, &text_bounds);
						message_height = (short)(bounds.bottom - bounds.top);
						function_1fa30(text, &bounds);
					}
				}
				else if (function_13ef30(message->text))
				{
					function_24ce9d(message->text, left, y, &bounds, &text_bounds);
					message_height = (short)(bounds.bottom - bounds.top);
					function_1fa30(message->text, &bounds);
				}

				y = (short)((real)message_height * view->line_spacing + (real)y);
			}
		}

		player = hud_player_get(local_player_index);
		field_24 = *(color4f *)g_4686d4;
		long game_time = g_510c54->game_time;
		top = g_4b9dd8.bottom - g_4b9dd0 - 0x28;
		*(s_color_bits *)&color.red = *function_13927e(local_player_index);
		bounds.left = g_4b9dd8.left - g_4b9dd2;
		bounds.right = g_4b9dd8.right - g_4b9dd2;
		bounds.bottom = top + line_height * 5;
		bounds.top = top;

		if (game_time >= player->time_4d8 && game_time < player->time_4d8 + player->value_4dc)
		{
			long duration = player->value_4dc ? player->value_4dc : 1;
			real fade = pow(1.0f - (real)(game_time - player->time_4d8) / (real)duration, 0.4f);

			color.alpha = function_1392a9(local_player_index) * fade;
			field_24.alpha = function_1392a9(local_player_index) * fade;
			function_13edb0(font, NONE, 2, 0, &color, &field_24);
			if (function_13ef30(player->text_454))
				function_1fa30(player->text_454, &bounds);
		}

		bounds.bottom += 0x12;
		top += 0x12;
		bounds.top = top;
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
