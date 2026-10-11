// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_13E8A0.CPP: the text drawing state: font, colours, shadow,
   justification and tab stops */

#include "unknown_11c920.h"
#include "unknown_13eeb0.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "font_loading.h"
#include "unknown_030290.h"
#include <string.h>


s_draw_string_globals g_4e73a0;

struct s_text_bounds
{
	short_rectangle2d rectangle;
	short ascending_height;
	short descending_height;
};

s_text_bounds g_4e7394;

typedef void (__stdcall *text_glyph_callback)(long, long, long, long, long,
	real, real, real, real, real, real, real, long, long);
void __stdcall function_13f0e0(text_glyph_callback callback, short_rectangle2d const *bounds,
	short *position, short_rectangle2d const *clip, short line_gap, real scale, dword const *text);

// @retail 0x13e8e0
void __stdcall function_13e8e0(long unknown0, long font, long unknown2, long unknown3, long unknown4,
	real x, real y, real unknown7, real unknown8, real width, real height, real scale, long unknown12, long unknown13)
{
	s_font_header *header = font_get(g_4e28f4[font]);
	real right = x + width * scale + 1.0f;
	real bottom = y + height * scale + 1.0f;
	if (g_4e7394.rectangle.left > x)
		g_4e7394.rectangle.left = (short)x;
	if (g_4e7394.rectangle.top > y)
		g_4e7394.rectangle.top = (short)y;
	if (right > g_4e7394.rectangle.right)
		g_4e7394.rectangle.right = (short)right;
	if (bottom > g_4e7394.rectangle.bottom)
		g_4e7394.rectangle.bottom = (short)bottom;
	if (header)
	{
		g_4e7394.ascending_height = header->ascending_height;
		g_4e7394.descending_height = header->descending_height;
	}
}

// @retail 0x13ea60
void function_13ea60(short_rectangle2d const *bounds, short_rectangle2d *ink_bounds,
	short_rectangle2d *line_bounds, dword const *text, real scale)
{
	short position[2];
	s_font_header *header = font_get(g_4e28f4[g_4e73a0.font]);
	g_4e7394.rectangle.top = 32767;
	g_4e7394.rectangle.left = 32767;
	g_4e7394.rectangle.bottom = -32768;
	g_4e7394.rectangle.right = -32768;
	if (header)
	{
		g_4e7394.ascending_height = header->ascending_height;
		g_4e7394.descending_height = header->descending_height;
	}
	function_13f0e0(function_13e8e0, bounds, position, NULL, 0, scale, text);
	line_bounds->left = position[0];
	line_bounds->right = position[0] + 1;
	line_bounds->top = position[1] - g_4e7394.ascending_height;
	line_bounds->bottom = position[1] + g_4e7394.descending_height;
	*ink_bounds = g_4e7394.rectangle;
}

void function_13eb60(color4f const *color);

/* the brightest colour text is drawn in: a colour with every channel above
   this is scaled down to it */
#define k_maximum_text_brightness 0.68f

// @retail 0x13ec70
void function_13ec70(color4f const *color)
{
	color4f c = *color;
	bool local_1 = true;
	local_1 &= c.red > k_maximum_text_brightness;
	local_1 &= c.green > k_maximum_text_brightness;
	local_1 &= c.blue > k_maximum_text_brightness;
	if (local_1)
	{
		real minimum = c.green > c.blue ? c.blue : c.green;
		minimum = c.red > minimum ? (c.green > c.blue ? c.blue : c.green) : c.red;
		if (k_maximum_text_brightness > minimum)
			minimum = k_maximum_text_brightness;
		else if (minimum > 1.0f)
			minimum = 1.0f;
		real scale = k_maximum_text_brightness / minimum;
		c.red *= scale;
		c.green *= scale;
		c.blue *= scale;
	}
	function_13eb60(&c);
}

// @retail 0x13e8a0
void function_13e8a0()
{
	memset(&g_4e73a0, 0, sizeof(g_4e73a0));
	g_4e73a0.font = 0;
	g_4e73a0.tab_stop_count = 0;
	g_4e73a0.flags = 0;
	g_4e73a0.justification = 0;
	g_4e73a0.unknown5e = 0;
	g_4e73a0.unknown60 = 0;
	function_13ec70(g_4686cc);
}

// @retail 0x13eb20
void function_13eb20(short const *tab_stops, short count)
{
	g_4e73a0.tab_stop_count = count > 16 ? 16 : count;
	if (g_4e73a0.tab_stop_count > 0)
	{
		memcpy(g_4e73a0.tab_stops, tab_stops, g_4e73a0.tab_stop_count * sizeof(short));
	}
}

// @retail 0x13eb60
void function_13eb60(color4f const *color)
{
	bool valid =
		color->alpha >= 0.0f && 1.0f >= color->alpha &&
		color->red >= 0.0f && 1.0f >= color->red &&
		color->green >= 0.0f && 1.0f >= color->green &&
		color->blue >= 0.0f && 1.0f >= color->blue;

	g_4e73a0.color = *color;
	if (!valid)
	{
		if (0.0f > g_4e73a0.color.alpha)
			g_4e73a0.color.alpha = 0.0f;
		else if (g_4e73a0.color.alpha > 1.0f)
			g_4e73a0.color.alpha = 1.0f;
		if (0.0f > g_4e73a0.color.red)
			g_4e73a0.color.red = 0.0f;
		else if (g_4e73a0.color.red > 1.0f)
			g_4e73a0.color.red = 1.0f;
		if (0.0f > g_4e73a0.color.green)
			g_4e73a0.color.green = 0.0f;
		else if (g_4e73a0.color.green > 1.0f)
			g_4e73a0.color.green = 1.0f;
		if (0.0f > g_4e73a0.color.blue)
			g_4e73a0.color.blue = 0.0f;
		else if (g_4e73a0.color.blue > 1.0f)
			g_4e73a0.color.blue = 1.0f;
	}
}

// @retail 0x13ed50
void function_13ed50(color4f const *field_24)
{
	if (field_24)
	{
		g_4e73a0.shadow = true;
		g_4e73a0.field_24 = *field_24;
	}
	else
	{
		g_4e73a0.shadow = false;
	}
}

// @retail 0x13ed90
void function_13ed90(long font)
{
	font_get(g_4e28f4[font]);
	g_4e73a0.font = font;
}

// @retail 0x13edb0
void function_13edb0(long font, long style, long justification, dword flags, color4f const *color, color4f const *field_24)
{
	font_get(g_4e28f4[font]);
	g_4e73a0.font = font;
	function_13ec70(color);
	if (field_24)
	{
		g_4e73a0.shadow = true;
		g_4e73a0.field_24 = *field_24;
	}
	else
	{
		g_4e73a0.shadow = false;
	}
	g_4e73a0.style = style;
	g_4e73a0.justification = justification;
	g_4e73a0.flags = flags;
}

/* ---- string iteration ---- */

#include "unknown_13fd90.h"

bool font_cache_predict_character(s_13eeb1 font_index, s_13eeb1 character);
long unicode_escape_character_lookup(word character, bool *found);
bool function_13fd20(utf32 previous, utf32 character);

enum
{
	_text_token_end = 0,
	_text_token_newline,
	_text_token_tab,
	_text_token_justification,
	_text_token_character,
};

enum
{
	_text_character_left = 0xe405,
	_text_character_right,
	_text_character_center
};

#pragma pack(push, 2)
/* walks a string of utf32 characters token by token */
struct s_text_iterator
{
	long font;
	s_font_header *field_4_3;
	dword const *string;
	short index;
	short style;
	short justification;
	dword character;
	dword previous_character;
	bool can_break;
	bool check_breaks;
	long token;
	long previous_token;
	dword color;
	dword field_24;
};
#pragma pack(pop)

static __forceinline dword alpha_rgb_to_pixel32(real alpha, real const *rgb)
{
	dword pixel = (long)(alpha * 255.0f);
	pixel = (pixel << 8) | (long)(rgb[0] * 255.0f);
	pixel = (pixel << 8) | (long)(rgb[1] * 255.0f);
	pixel = (pixel << 8) | (long)(rgb[2] * 255.0f);
	return pixel;
}

// @retail 0x13f470
bool function_13f470(s_text_iterator *iterator, long font, short justification, dword const *string, short style, color4f const *color, bool const *shadow, color4f const *field_24)
{
	memset(iterator, 0, sizeof(*iterator));
	iterator->string = string;
	iterator->style = style;
	iterator->justification = justification;
	iterator->font = font;
	iterator->index = 0;
	iterator->can_break = false;
	iterator->check_breaks = (bool)(g_4e73a0.flags & 1);
	iterator->color = alpha_rgb_to_pixel32(color->alpha, &color->red);
	if (*shadow)
	{
		real alpha = field_24->alpha > color->alpha ? color->alpha : field_24->alpha;
		iterator->field_24 = alpha_rgb_to_pixel32(alpha, &field_24->red);
	}
	else
	{
		iterator->field_24 = 0;
	}
	iterator->field_4_3 = font_get(g_4e28f4[font]);
	return iterator->field_4_3 != NULL;
}

// @retail 0x13f5a0
void function_13f5a0(s_text_iterator *iterator)
{
	long character;
	long token;

	do
	{
		character = iterator->string[iterator->index++];
		switch (character)
		{
		case 0:
			token = _text_token_end;
			break;
		case '\t':
			token = _text_token_tab;
			break;
		case '\n':
			token = NONE;
			break;
		case '\r':
			token = _text_token_newline;
			break;
		case _text_character_left:
			iterator->justification = 0;
			token = _text_token_justification;
			break;
		case _text_character_right:
			iterator->justification = 1;
			token = _text_token_justification;
			break;
		case _text_character_center:
			iterator->justification = 2;
			token = _text_token_justification;
			break;
		default:
			if (iterator->check_breaks)
			{
				utf32 previous = { iterator->character };
				utf32 next = { character };
				iterator->can_break = function_13fd20(previous, next);
			}
			else
			{
				iterator->can_break = false;
			}
			token = _text_token_character;
			break;
		}
	} while (token == NONE);

	if (iterator->previous_token != _text_token_character)
	{
		iterator->previous_character = 0;
	}
	else
	{
		iterator->previous_character = iterator->character;
	}
	iterator->character = character;
	iterator->previous_token = iterator->token;
	iterator->token = token;
}

// @retail 0x13eeb0
bool function_13eeb0(dword const *string, long font)
{
	bool result = true;
	s_text_iterator iterator;

	if (function_13f470(&iterator, font, (short)g_4e73a0.justification, string, (short)g_4e73a0.style, &g_4e73a0.color, &g_4e73a0.shadow, &g_4e73a0.field_24))
	{
		for (;;)
		{
			function_13f5a0(&iterator);
			if (iterator.token == _text_token_character)
			{
				s_13eeb1 local_1 = { iterator.character };
				s_13eeb1 local_2 = { font };
				if (!font_cache_predict_character(local_2, local_1))
				{
					result = false;
				}
			}
			else if (iterator.token == _text_token_end)
			{
				break;
			}
		}
	}

	return result;
}

/* copies a string into utf32 characters, decoding the '|' escapes */
static __forceinline void unicode_string_to_characters(long maximum_count, word const *source, dword *destination)
{
	while (maximum_count > 0)
	{
		if (maximum_count == 1)
		{
			*destination = 0;
			break;
		}

		dword character;
		word const *next = source + 1;
		word c = *source;
		if (c == '|')
		{
			bool found = false;
			character = unicode_escape_character_lookup(*next, &found);
			if (found)
			{
				next++;
			}
		}
		else
		{
			character = c;
		}

		source = next;
		*destination = character;
		if (!character)
		{
			break;
		}
		destination++;
		maximum_count--;
	}
}
// @retail 0x13ee20
bool function_13ee20(word const *string, long font)
{
	dword characters[0x800];
	unicode_string_to_characters(0x800, string, characters);
	return function_13eeb0(characters, font);
}

// @retail 0x13ef30
bool function_13ef30(word const *string)
{
	return function_13ee20(string, g_4e73a0.font);
}
/* whether a private use character is drawn as a glyph (rather than being a
   formatting code) */
// @retail 0x13f660
bool function_13f660(utf32 codepoint)
{
	long character = codepoint.value;
	bool result = false;

	if (character >= 0xe112 && character <= 0xe12b)
	{
		return false;
	}

	if (character >= 0xe000 && character <= 0xe3ff)
	{
		switch (character)
		{
		case 0xe000:
		case 0xe001:
		case 0xe002:
		case 0xe004:
		case 0xe008:
		case 0xe106:
		case 0xe107:
		case 0xe108:
		case 0xe109:
		case 0xe10a:
		case 0xe10b:
		case 0xe10c:
		case 0xe10d:
		case 0xe10e:
		case 0xe10f:
		case 0xe110:
		case 0xe111:
		case 0xe12c:
		case 0xe12d:
		case 0xe12e:
		case 0xe12f:
		case 0xe130:
		case 0xe131:
			return false;
		default:
			result = true;
			break;
		}
	}

	return result;
}

struct s_font_character_header
{
	word unknown00;
	word pixels_size;
	short width;
	short height;
	byte unknown08[4];
	dword pixels_offset;
};

struct s_text_cached_character
{
	byte unknown00[0x10];
	long state;
	byte unknown14[8];
	s_font_character_header header;
	byte unknown2c[0xc];
};

extern s_record_pool *g_54d574;
long font_cache_get_character(long font_index, long character, dword flags);
bool font_cache_character_load_pixels(long datum_index, dword flags);
s_font_character_header *font_cache_get_character_header(long font_index, long character, dword flags);
short function_122570(s_font_header const *header, dword first_character, dword second_character);

struct s_text_line_measurement
{
	real width;
	long index;
};

// @retail 0x13ef40
s_text_line_measurement function_13ef40(s_text_iterator *iterator, point2f const *position, box2f const *bounds, real scale)
{
	real width = 0.0f;
	volatile long index = 0;
	volatile long count = 0;
	volatile long break_index = 0;
	real break_width;
	bool done = false;
	do
	{
		bool wrapped = false;
		function_13f5a0(iterator);
		if (iterator->token == _text_token_character)
		{
			long glyph_index = font_cache_get_character(iterator->font, iterator->character, 3);
			s_font_character_header *header = NULL;
			if (glyph_index != NONE && font_cache_character_load_pixels(glyph_index, 3))
			{
				s_text_cached_character *glyph = &((s_text_cached_character *)g_54d574->data)[glyph_index & 0xffff];
				if (glyph->state == 4)
					header = &glyph->header;
			}
			if (header)
			{
				real kerning = function_122570(iterator->field_4_3, iterator->character, iterator->previous_character) * scale;
				real advance = (short)header->unknown00 * scale;
				real offset = *(short *)header->unknown08 * scale;
				if (iterator->can_break)
				{
					break_index = index;
					break_width = width;
				}
				if (!(bounds->x1 > position->x + advance + offset + kerning + width) && count)
				{
					if (g_4e73a0.flags & 1)
					{
						if (break_index > 0)
						{
							index = break_index;
							width = break_width;
							wrapped = true;
						}
						done = true;
					}
				}
				else
				{
					count++;
					width = advance + offset + kerning + width;
				}
			}
		}
		else
			done = true;
		if (!wrapped)
			index = iterator->index;
	} while (!done);
	s_text_line_measurement result = { width, index };
	return result;
}

// @retail 0x13f700
void function_13f700(box2f const *bounds, box2f const *clip, short first,
	text_glyph_callback callback, point2f *position, dword color, dword shadow,
	dword const *text, short last, real scale)
{
	real left = -32768.0f;
	real top = -32768.0f;
	real right = 32767.0f;
	real bottom = 32767.0f;
	if (bounds)
	{
		if (bounds->x0 > left) left = bounds->x0;
		if (bounds->x1 < right) right = bounds->x1;
		if (bounds->y0 > top) top = bounds->y0;
		if (bounds->y1 < bottom) bottom = bounds->y1;
	}
	if (clip)
	{
		if (clip->x0 > left) left = clip->x0;
		if (clip->x1 < right) right = clip->x1;
		if (clip->y0 > top) top = clip->y0;
		if (clip->y1 < bottom) bottom = clip->y1;
	}
	if (right > left && bottom > top)
	{
		s_text_iterator iterator;
		if (function_13f470(&iterator, g_4e73a0.font, (short)g_4e73a0.justification, text,
			(short)g_4e73a0.style, &g_4e73a0.color, &g_4e73a0.shadow, &g_4e73a0.field_24))
		{
			iterator.index = first;
			while (iterator.index < last)
			{
				function_13f5a0(&iterator);
				if (!iterator.character)
					break;
				if (iterator.token == _text_token_character && ((long)iterator.character < 0 || (long)iterator.character > 31))
				{
					s_font_character_header *header = font_cache_get_character_header(iterator.font, iterator.character, 3);
					if (header)
					{
						real width = header->width;
						real height = header->height;
						real source_x = 0.0f;
						real source_y = 0.0f;
						real kerning = function_122570(iterator.field_4_3, iterator.character, iterator.previous_character) * scale;
						real offset = *(short *)header->unknown08 * scale;
						real x = position->x + offset + kerning;
						real y = position->y - *(short *)(header->unknown08 + 2) * scale;
						if (x + width > right) width = right - x;
						if (x < left)
						{
							source_x = left - x;
							x = left;
							width -= source_x;
						}
						if (y + height > bottom) height = bottom - y;
						if (y < top)
						{
							source_y = top - y;
							y = top;
							height -= source_y;
						}
						if (width > 0.0f && height > 0.0f)
						{
							dword pixel = color;
							utf32 codepoint = { iterator.character };
							if (function_13f660(codepoint)) pixel |= 0xffffff;
							callback((long)&iterator, iterator.font, iterator.character, pixel, shadow,
								x, y, source_x, source_y, width, height, scale,
								(long)g_4e73a0.vertex_proc, g_4e73a0.vertex_proc_parameter);
						}
						position->x = (short)header->unknown00 * scale + position->x + offset + kerning;
					}
				}
			}
		}
	}
}

// @retail 0x13f0e0
void __stdcall function_13f0e0(text_glyph_callback callback, short_rectangle2d const *bounds,
	short *arg_c5cac5, short_rectangle2d const *clip, short line_gap, real scale, dword const *text)
{
	point2f position = { (real)bounds->left, (real)bounds->top };
	long tab = 0;
	long line = 0;
	long maximum_lines = 0;
	long wrapped_line = 0;
	s_text_iterator iterator;
	if (function_13f470(&iterator, g_4e73a0.font, (short)g_4e73a0.justification, text,
		(short)g_4e73a0.style, &g_4e73a0.color, &g_4e73a0.shadow, &g_4e73a0.field_24))
	{
		for (;;)
		{
			long first = iterator.index;
			long justification = iterator.justification;
			box2f line_bounds = { (real)bounds->left, (real)bounds->right, (real)bounds->top, (real)bounds->bottom };
			if (g_4e73a0.tab_stop_count > 0)
			{
				if ((short)tab != 0)
					line_bounds.x0 = g_4e73a0.tab_stops[(short)tab - 1] * scale;
				else
					line_bounds.x0 += ((short)line == 0 ? g_4e73a0.unknown5e : g_4e73a0.unknown60) * scale;
				if ((short)tab < g_4e73a0.tab_stop_count)
					line_bounds.x1 = g_4e73a0.tab_stops[(short)tab] * scale;
			}
			else
				line_bounds.x0 += ((short)line == 0 ? g_4e73a0.unknown5e : g_4e73a0.unknown60) * scale;
			s_font_header *header = iterator.field_4_3;
			position.x = *(short *)header->unknown0a * scale + line_bounds.x0;
			position.y = (header->ascending_height +
				(header->leading_height + line_gap + header->descending_height + header->ascending_height) * ((short)wrapped_line + (short)line)) * scale + line_bounds.y0;
			s_text_line_measurement measured = function_13ef40(&iterator, &position, &line_bounds, scale);
			header = iterator.field_4_3;
			switch (justification)
			{
			case 1:
				position.x = line_bounds.x1 - line_bounds.x0 + line_bounds.x0 - measured.width - *(short *)header->unknown0a;
				break;
			case 2:
				position.x = (line_bounds.x1 - line_bounds.x0 - measured.width) * 0.5f + line_bounds.x0;
				break;
			}
			if (line_bounds.y1 > position.y)
			{
				box2f clip_bounds;
				box2f const *clipping = NULL;
				if (clip)
				{
					clip_bounds.x0 = clip->left;
					clip_bounds.x1 = clip->right;
					clip_bounds.y0 = clip->top;
					clip_bounds.y1 = clip->bottom;
					clipping = &clip_bounds;
				}
				function_13f700(&line_bounds, clipping, (short)first, callback, &position, iterator.color, iterator.field_24, text, (short)measured.index, scale);
			}
			iterator.index = (short)measured.index;
			switch (iterator.token)
			{
			case _text_token_end:
				goto done;
			case _text_token_newline:
				line += maximum_lines + 1;
				tab = 0;
				maximum_lines = 0;
				wrapped_line = 0;
				break;
			case _text_token_tab:
				if ((short)tab < g_4e73a0.tab_stop_count)
				{
					tab++;
					wrapped_line = 0;
				}
				break;
			case _text_token_justification:
				wrapped_line = 0;
				break;
			case _text_token_character:
				wrapped_line++;
				if ((short)wrapped_line > (short)maximum_lines)
					maximum_lines = wrapped_line;
				break;
			}
		}
	}
done:
	if (arg_c5cac5)
	{
		arg_c5cac5[0] = (short)position.x;
		arg_c5cac5[1] = (short)position.y;
	}
}

// @retail 0x13e9c0
void function_13e9c0(word const *text, short_rectangle2d const *bounds, short_rectangle2d *a, short_rectangle2d *b, real scale)
{
	dword characters[0x800];
	unicode_string_to_characters(0x800, text, characters);
	function_13ea60(bounds, a, b, characters, scale);
}
