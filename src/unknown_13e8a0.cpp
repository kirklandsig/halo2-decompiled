// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_13E8A0.CPP: the text drawing state: font, colours, shadow,
   justification and tab stops */

#include "unknown_11c920.h"
#include "unknown_13eeb0.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "font_loading.h"
#include "unknown_030290.h"
#include "text_style_clear.h"
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
	text_style_clear();
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
bool unicode_line_break_allowed(utf32 previous, utf32 character);

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
				iterator->can_break = unicode_line_break_allowed(previous, next);
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
			word local_1 = *next;
			character = unicode_escape_character_lookup(local_1, &found);
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
struct s_font_cache_flags
{
	dword wait : 1;
	dword create : 1;
};
long font_cache_get_character(long font_index, long character, s_font_cache_flags flags);
bool font_cache_character_load_pixels(long datum_index, s_font_cache_flags flags);
s_font_character_header *font_cache_get_character_header(long font_index, long character, s_font_cache_flags flags);
short function_122570(s_font_header const *header, dword first_character, dword second_character);

struct s_text_line_measurement
{
	real width;
	long index;
};

// MiniMax S (11-16): no new form, no lower score (77): the remaining differences are still the XMM0/XMM3 colouring (this build gives XMM0 to the scale and XMM3 to the advance, retail the other way round) and the update's adds, which retail keeps inside the update arm while ours run ahead of the test; the summed-width spelling that reached 65 is not kept, a one-use local being kept only with a byte match.
// MiniMax C (11-15): score 65 (was 77): the new running width is summed once after the can_break block and before the
// test (`real next_width = advance + offset + kerning + width;`) and stored in the update arm ahead of the count's
// increment, which is the order retail stores them in. The advance is still live across that sum, so the update
// accumulates in a copy of the advance (retail's `movaps xmm3, xmm0`) and the running width loads early into XMM4
// as retail does; the earlier spelling that named the same sum before the can_break block scored 68, and the sum
// named inside the update arm never produces the copy (77). Still open: the XMM0/XMM3 colouring (ours gives XMM0
// to the scale and XMM3 to the advance, retail the other way round) and the update's adds, which retail keeps in
// the update arm while ours run ahead of the test. The new width is read once, in the update arm.
// score 77 (was 77): M3 lane 8 r170 (fast with 0x13f0e0): the update accumulated in the advance (`advance += offset;
// advance += kerning; advance += width; width = advance;`) 77; `advance += offset + kerning; width += advance; count++;`
// 77; with the test reading the advance product written out again (diagnostic) 77, still no copy; 1-7 dead long locals at
// the top 77 each (no phase effect); 0x13f700's `real x = position->x + advance + offset + kerning;` named before the
// can_break block, test `x + width` 75 (the width loads early into XMM4 as retail because the offset holds XMM3, but the
// sum runs ahead of the can_break branch and there is still no advance copy; a one-use part moved ahead, not kept).
// Earlier first line: round 116 (m3 lane 9, fast): kerning/offset/advance declared at function scope 77, the loop as
// `while (!done)` 77 (both same code); re-measured (listed dimensions combined, same code): `advance = raw; advance =
// scale * advance;` 77, all three metrics raw first and then `x = scale * x` 77. The cause stays the advance copy
// (`movaps xmm3, xmm0`) that gives the width XMM4. No untried natural form is left.
// Earlier at 77 (was 77): retail loads the width into xmm4 before the multiply
// unchanged this round: retail copies the advance product into XMM3 and accumulates
// the update in that copy, loads the running width into XMM4 early and reuses the freed XMM0 for the
// bound; ours keeps the advance in XMM0, accumulates the update there and reloads the width late into
// XMM3, with the bound in XMM5. The permuter found no keepable form (its best, 72, adds a flag set by
// one test and read by the next).
// Earlier note (77): retail loads the width into xmm4 before the multiply
// chain and accumulates the four terms in xmm3; ours reloads the width into
// xmm3 after the multiplies and accumulates in xmm0 (register allocation only).
// MiniMax C (11-11): no new form, no lower score (77): the one difference left is the running width's reload —
// retail loads it into xmm4 right after the scale and copies the advance into xmm3 (movaps) so the bound can
// reuse xmm0, ours reloads it into xmm3 after the three multiplies and accumulates the update in xmm0; the
// width's load placement follows the source order of its read, so moving the can_break block between the
// metric definitions gives 111 and the block's own position is already the one retail uses.
// MiniMax D (11-11): no lower score (77): naming the update's sum once before the test reaches 74 but computes the terms
// twice, so it was reverted; with the sum written in the update arm the copy never appears and the allocation stays as it is.
// M3 lane 13 r75 (fast; masked aligned count in brackets, base [16]): the end sum named and computed before the can_break block 75 [21]
// (the width load then moves to retail's XMM4 right after the scale, but the adds move up with it; match-only); the else arm's
// width update before `count++` 77 [16]; advance computed before offset 78 [18]; break_width stored before break_index 76 [16]; both
// of those 75 [22].
// Tried (77, same code): the test written as sum < x1; width += advance + offset + kerning;
// a copy of the width taken after the kerning call. Worse: advance or offset written
// inline at both uses (111), the can_break block ahead of offset/advance (111).
// m3 lane 7 r146 (fast): the one cause is retail's `movaps xmm3, xmm0` after the advance product. The update's
// advance is a second register copy, so the width takes xmm4 and loads early and the compare reuses xmm0 for x1.
// Matched 0x142640 and 0x17c040 give that copy from `x = scale * x` on an already-defined variable. Tried (77, same
// code): the advance product written out in the update only, or in the compare only (CSE merges it); a two-step
// `advance = raw; advance *= scale`; the kerning call result named before its product; the update summed as
// w+a+o+k, k+o+a+w, (a+o)+(k+w) (77) or a+(o+k)+w (76); the compare as w+x+a+o+k (77) or x+(a+o+k)+w (83).
// Definition orders (kerning/offset/advance) kao 78, oka 82, ako 82, oak and aok 133 (393 bytes); break_width
// declared first 77; the can_break stores swapped 76; the restore swapped 77; unsigned break_index 77. Permuter
// (600 s) best 71: those swaps plus an unsigned index, a byte flag and a flag set from the count test (not kept).
// Round 99 (lane 16): the cause is a copy, not the width load: retail copies the advance (`movaps xmm3,
// xmm0`) right after its multiply and accumulates the width in that copy, so the bound reuses XMM0 and
// the width goes to XMM4 early; ours accumulates in place in XMM0. Same code (77) for the accumulation as
// width + advance + offset + kerning, the test as width + x + ... or x + width + ..., both of those;
// the advance written out at both uses 111 (aligned 36 against 16); written out only in the
// accumulation 77, only in the test 77; the new width named before the test (`real next_width = advance +
// offset + kerning + width;`, used once in the else arm) 74 (width loaded early into XMM4 as retail, but
// the copy runs the other way and the adds move ahead of the test; one-use local, not kept); the same
// started from the advance and completed before the test 74; a copy of the advance at its definition,
// accumulated in the else arm (as `+` or `+=`) 77 (copy propagated away).
// Round 172 (m3 lane 5, fast): the bound test positive with the arms in natural order (`x1 > sum || !count`, or
// `count == 0`) 77 and the products written scale first 77 (identical code); width and index accumulated in the
// returned measurement aggregate 96 (400 bytes, XMM pattern unchanged); `advance + offset + kerning` named once for
// both the test and the update 84; the token test negated with the done arm first 104. Diagnostic: `(real)(scale *
// 1.f)` chains 1-3 deep in the kerning product fold away (identical code).
// Round 116 (m3 lane 12, fast; control 77): the bound read once into a local before the loop 123 (425 bytes); the start
// x read once into a local before the loop 116 (416 bytes); the test written `x1 <= sum && count` 80 (the compare then
// takes x1 as a memory operand, `comiss xmm4, [edx+4]`). Retail's bound reuses XMM0 after the advance's last add in the
// test, so its advance has two registers (XMM0 for the test, the XMM3 copy for the update); ours keeps one.
// Round 175 (m3 lane 6, fast; control 77): the bound test summed kerning first (`x + kerning + offset + advance + width`)
// 77; the test's sum named right before the test 77; a copy of the width taken at the top of the character block, before
// the kerning call, used for the break store, the test and the update 77 (variant of the listed copy after the call); the
// font-cache flags declared once before the loop 77; `wrapped` declared at function scope and cleared at the loop top 77;
// the returned aggregate filled by member assignment 77.
// All the same code: retail's early width load into XMM4 (and with it the advance copy) did not move in any spelling.
// m3 lane 3 r239 (fast with the file's functions): `width` renamed to sort ahead of `advance`, or `advance` renamed to sort
// after `width`, 77 each (same code: names do not move the width/advance-copy XMM coloring); `width` declared last among the
// entry locals 81 (the entry stores reorder); kerning/offset/advance declared at the top of the loop body and assigned in
// place 77 (same code). Retail's width loads into XMM4 ahead of the products because the advance copy takes XMM3.
// P485: One continue/stop/restore-break completion result, byte/char/signed-char
// 123/123/123, short/word/int/long/dword 117/117/117/117/117. NOT kept: no byte
// match; original two flags, float trees, loop and complete body restored. Native advance
// copy and early width load remain.
// M3 lane 15 r83 (fast): `__assume` non-NULL on bounds, position, iterator, all three 77 each (same code); on the header
// inside `if (header)` 122 (399 B, the inlined lookup's NULL test folds). Compile-only screen (0.6 s a variant; it reproduces
// this body's colouring: advance XMM0 for the test and the update, width XMM3, sum XMM4, bound XMM5), none with retail's
// `movaps xmm3, xmm0` advance copy: `width += advance + offset + kerning`; the product written at the test or the update;
// advance accumulated in place (`advance += offset; advance += kerning; width += advance` and `advance += offset + kerning +
// width; width = advance`); `count &&` first; the test as `sum >= x1`; a named fits bool; the arms swapped; width-first and
// advance-first test sums; a width-first update; 120 of the 720 orders of the six entry declarations; 1-12 dead long and
// 1-8 dead real locals; 90 explicit-parenthesis shapes of the test and the update (the copy appears only where the test
// and the update share a parenthesized partial sum, which then moves the update ahead of the test).
// m3 lane 1 r103 (fast with 0x13f700/0x13f0e0): the bound `real right = bounds->x1;`, the start `real left = position->x;`, or
// both, named right before the test 77 each (same code). Re-measured as a diagnostic: the advance product written at both uses
// 111; its listing shows why the width loads late here: the width takes the scale's XMM3 once the scale dies, so the load cannot
// be hoisted; retail gives XMM3 to a second copy of the advance (`movaps xmm3, xmm0`) and the width XMM4. With the product at
// both uses the width does load early (the scale is still live), but the update reuses the test's product in place, so there
// is still one advance range. No untried natural form found.
// P492: Original two flags produced by token/setting/break-index tests,
// seven bool subsets then all byte: 111/134/116/115/108/119/123/123.
// NOT kept: no native advance copy or early width-load recovery; original
// flags, lazy reads, arithmetic groups and complete body restored at 77.
// m3 lane 9 r123 (fast with 0x13f700/0x13f0e0): the three metrics `real const` 77; the update through an inline helper that
// accumulates into its advance parameter 77 (the parameter copy is propagated away like a named copy); the three metrics in one
// declaration 77; `width += advance + offset + kerning;` before `count++` 77; the bound test as `sum >= x1 && count != 0` 80.
// P504: Original-site immutable kerning/offset/advance/all-metric producers, cache flags, header pointer binding,
// header pointee and incoming scale: 77/77/77/77/77/77/77/77. NOT kept: All eight tie 77 at 415 bytes. Native
// independent advance copy and early running-width load are unchanged. Original producer and input mutability
// restored; all calibrated file/provider measurements unchanged. Original complete executable body restored.
// M3 lane 15 r87 (fast with 0x13f700/0x13f0e0; base 77, register-masked aligned 10): the can_break block between the offset
// and the advance 111, between the kerning and the offset 111, the first of those with the width update before `count++`
// 111 (all three move the advance conversion late and lose the hoisted 4 in EBX); `width += advance + offset + kerning;`
// before `count++` 77 (masked 7: the update arm's adds, `inc eax` and the width-then-count stores are retail's; the early
// width load into XMM4 and the advance copy still missing); the header's two metrics read into shorts before the kerning
// call 77 (same code).
// P537: Original-site break-admission truth in positive/complementary bool/byte/short/long:
// eight 77s at 415 bytes. NOT kept: native independent advance copy and early width load
// unchanged; original field observation, flag writes, float trees and full body restored.
// Round 129 (m3 lane 12, fast): the bound test with the sum on the left (`!(sum < bounds->x1) && count`) 77 (same
// code: x1 still loads first); the header's advance and offsets typed as short fields (no casts; 0x13f700 and
// 0x13f0e0 still match) 77; the three metrics through an inline `pixels * scale` helper 77; `real right =
// position->x + advance;` named after the metrics and used in the test 74 (the width then loads early into XMM4, but
// the advance gets no copy; a one-use part, not kept). Diagnostics: without the width restore 101 (394 bytes), without
// the width's zero initialiser 53; neither gives the advance copy. Retail's colouring gives the bound XMM0 ahead of the
// advance (whose update-path value then needs the XMM3 copy); ours colours the advance XMM0 and the width XMM3. Also:
// the test and the update sharing a parenthesized `(advance + offset + kerning)` 83 (computed once, no copy).
// NOT kept (coordinator review, MiniMax C 11-15): 77 -> 65 rested on a one-use local computed ahead of the test
// (real next_width = advance + offset + kerning + width; then width = next_width in the else arm), a form kept
// only with a byte match.
// @retail 0x13ef40
s_text_line_measurement function_13ef40(s_text_iterator *iterator, point2f const *position, box2f const *bounds, real scale)
{
	long index = 0;
	real width = 0.0f;
	long break_index = 0;
	long count = 0;
	real break_width;
	bool done = false;
	do
	{
		bool wrapped = false;
		function_13f5a0(iterator);
		if (iterator->token == _text_token_character)
		{
			s_font_cache_flags flags = { true, true };
			s_font_character_header *header = font_cache_get_character_header(iterator->font, iterator->character, flags);
			if (header)
			{
				real kerning = function_122570(iterator->field_4_3, iterator->previous_character, iterator->character) * scale;
				real offset = *(short *)header->unknown08 * scale;
				real advance = (short)header->unknown00 * scale;
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
					s_font_cache_flags flags = { true, true };
					s_font_character_header *header = font_cache_get_character_header(iterator.font, iterator.character, flags);
					if (header)
					{
						real source_x = 0.0f;
						real source_y = 0.0f;
						real width = header->width;
						real height = header->height;
						real kerning = function_122570(iterator.field_4_3, iterator.previous_character, iterator.character) * scale;
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
	long tab = 0;
	long line = 0;
	long maximum_lines = 0;
	long wrapped_line = 0;
	point2f position = { (real)bounds->left, (real)bounds->top };
	s_text_iterator iterator;
	if (!function_13f470(&iterator, g_4e73a0.font, (short)g_4e73a0.justification, text,
		(short)g_4e73a0.style, &g_4e73a0.color, &g_4e73a0.shadow, &g_4e73a0.field_24))
		goto done;
	do
	{
		long first = iterator.index;
		long justification = iterator.justification;
		box2f line_bounds = { (real)bounds->left, (real)bounds->right, (real)bounds->top, (real)bounds->bottom };
		if (g_4e73a0.tab_stop_count > 0)
		{
			if ((short)tab != 0)
				line_bounds.x0 = g_4e73a0.tab_stops[(short)tab - 1] * scale;
			else
			{
				if ((short)line != 0)
					line_bounds.x0 += g_4e73a0.unknown60 * scale;
				else
					line_bounds.x0 += g_4e73a0.unknown5e * scale;
			}
			if ((short)tab < g_4e73a0.tab_stop_count)
				line_bounds.x1 = g_4e73a0.tab_stops[(short)tab] * scale;
		}
		else
		{
			if ((short)line != 0)
				line_bounds.x0 += g_4e73a0.unknown60 * scale;
			else
				line_bounds.x0 += g_4e73a0.unknown5e * scale;
		}
		s_font_header *header = iterator.field_4_3;
		position.x = *(short *)header->unknown0a * scale + line_bounds.x0;
		long height = header->descending_height + line_gap;
		height += header->leading_height;
		long ascent = header->ascending_height;
		height += ascent;
		position.y = (ascent + height * ((short)wrapped_line + (short)line)) * scale + line_bounds.y0;
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
			box2f const *clipping;
			if (clip)
			{
				clip_bounds.x0 = clip->left;
				clip_bounds.x1 = clip->right;
				clip_bounds.y0 = clip->top;
				clip_bounds.y1 = clip->bottom;
				clipping = &clip_bounds;
			}
			else
				clipping = NULL;
			function_13f700(&line_bounds, clipping, (short)first, callback, &position, iterator.color, iterator.field_24, text, (short)measured.index, scale);
		}
		iterator.index = (short)measured.index;
		switch (iterator.token)
		{
		case _text_token_character:
			wrapped_line++;
			if ((short)wrapped_line > (short)maximum_lines)
				maximum_lines = wrapped_line;
			break;
		case _text_token_tab:
			if ((short)tab < g_4e73a0.tab_stop_count)
			{
				tab++;
				wrapped_line = 0;
			}
			break;
		case _text_token_newline:
			tab = 0;
			line += maximum_lines + 1;
			maximum_lines = 0;
			wrapped_line = 0;
			break;
		case _text_token_justification:
			wrapped_line = 0;
			break;
		case _text_token_end:
			goto done;
		}
	} while (iterator.token != _text_token_end);
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
