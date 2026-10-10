// @flags /O1 /arch:SSE /Gr
/* UNKNOWN_22A648.CPP: hud drawing (built for size; the file continues past
   0x22c000) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include <math.h>
#include <stddef.h>

// @retail 0x22a85b
long function_22a85b(
	char type)
{
	long result = NONE;

	switch (type)
	{
	case 0:
		result = 6;
		break;
	case 1:
		result = 8;
		break;
	}
	return result;
}

struct s_4e6950
{
	byte unknown00[0x30];
	color4f color;
	byte unknown40[0x40];
};

typedef char check_widget_color_offset[offsetof(s_4e6950, color) == 0x30 ? 1 : -1];
typedef char check_widget_interface_view_size[sizeof(s_4e6950) == 0x80 ? 1 : -1];

extern s_4e6950 g_4e6950;

struct s_text_widget
{
	byte unknown00[0x1c];
	word mode;
	word flags;
	byte unknown20[4];
	long font_tag;
	long text;
	byte unknown2c[4];
	char fonts[3];
};

struct s_text_widget_state
{
	byte unknown00[0x224];
	bool has_player;
	byte unknown225[3];
	long player_index;
};

long function_13a690(long mode);
real __stdcall function_22ace7(long context, long name);
word *function_1630e0(word *buffer, word const *format, ...);
void function_13925f(long string_handle, word *buffer);
byte *record_pool_lookup(s_record_pool *data, long datum_index);
void parse_text(word *string);

// @retail 0x22a871
void function_22a871(s_text_widget const *widget, s_text_widget_state const *state,
	color4f const *color, word *text, long *font)
{
	if (widget->text && widget->font_tag != NONE)
	{
		long mode = function_13a690(widget->mode);
		g_4e6950.color = *color;
		if (widget->flags & 1)
		{
			long value = (long)function_22ace7(NONE, widget->text);
			if (widget->flags & 2)
			{
				value = value < 0 ? 0 : value > 99 ? 99 : value;
				if (value < 10)
					function_1630e0(text, (word const *)L"0%d", value);
				else
					function_1630e0(text, (word const *)L"%d", value);
			}
			else if (widget->flags & 4)
			{
				value = value < 0 ? 0 : value > 999 ? 999 : value;
				if (value < 10)
					function_1630e0(text, (word const *)L"00%d", value);
				else if (value < 100)
					function_1630e0(text, (word const *)L"0%d", value);
				else
					function_1630e0(text, (word const *)L"%d", value);
			}
			else
				function_1630e0(text, (word const *)L"%d", value);
		}
		else if (widget->flags & 8)
		{
			long player_index = state->player_index;
			if (state->has_player && player_index != NONE)
			{
				byte *player = record_pool_lookup(g_4e8c24, player_index);
				if (player)
					function_1630e0(text, (word const *)L"%s", player + 0x44);
				else
					function_1630e0(text, (word const *)L"");
			}
			else
				function_1630e0(text, (word const *)L"");
		}
		else
		{
			function_13925f(widget->text, text);
			parse_text(text);
		}
		*font = NONE;
		switch (mode)
		{
		case 0: *font = function_22a85b(widget->fonts[0]); break;
		case 1: *font = function_22a85b(widget->fonts[1]); break;
		case 2: *font = function_22a85b(widget->fonts[2]); break;
		}
	}
}

bool function_13ee20(word const *string, long font);

// @retail 0x22a9bc
bool function_22a9bc(s_text_widget const *widget, long unused,
	s_text_widget_state const *state, color4f const *color)
{
	long const *local_0 = &unused;
	(void)local_0;
	volatile bool result = true;
	if (widget->text && widget->font_tag != NONE)
	{
		word text[256];
		long font;
		text[0] = 0;
		function_22a871(widget, state, color, text, &font);
		if (!function_13ee20(text, g_4e73a0.font))
			result = false;
	}
	return result;
}

struct s_tag_data
{
	long size;
	byte *address;
};

struct s_widget_transform_function
{
	long name;
	byte unknown04[8];
	s_tag_data function;
};

struct s_widget_transform
{
	dword flags;
	s_widget_transform_function scale_x;
	s_widget_transform_function scale_y;
	s_widget_transform_function rotation;
	s_widget_transform_function offset_x;
	s_widget_transform_function offset_y;
};

struct s_widget_transform_block
{
	long count;
	s_widget_transform *address;
};

typedef char check_widget_transform_size[sizeof(s_widget_transform_function) == 0x14 ? 1 : -1];
typedef char check_widget_rotation_offset[offsetof(s_widget_transform, rotation) == 0x2c ? 1 : -1];
typedef char check_widget_font_offset[offsetof(s_text_widget, fonts) == 0x30 ? 1 : -1];

real function_13b390(void const *function, real input, real range);
real function_13bb40(s_tag_data const *function, real value);

static __forceinline real widget_transform_value(s_widget_transform_function const *transform)
{
	real input = function_22ace7(NONE, transform->name);
	return function_13bb40(&transform->function, function_13b390(&transform->function, input, 1.0f));
}

static inline void rotate_widget_axis(point2f *axis, real sine, real cosine)
{
	real y = axis->x * sine + axis->y * cosine;
	axis->x = cosine * axis->x - sine * axis->y;
	axis->y = y;
}

static __forceinline void offset_widget_position(point2f *position, point2f const *axis, real distance)
{
	real x = axis->x * distance;
	real y = axis->y * distance;
	position->x += x;
	position->y += y;
}

// @retail 0x22bcaf
void function_22bcaf(s_widget_transform_block const *block, point2f *position,
	point2f *axis_x, point2f *axis_y)
{
	if (block->count)
	{
		s_widget_transform *transform = block->address;
		if (transform->flags & 2)
		{
			real angle = widget_transform_value(&transform->rotation);
			real sine = (real)sin(angle);
			real cosine = (real)cos(angle);
			rotate_widget_axis(axis_x, sine, cosine);
			rotate_widget_axis(axis_y, sine, cosine);
		}
		if (transform->flags & 4)
		{
			real x = widget_transform_value(&transform->offset_x);
			real y = widget_transform_value(&transform->offset_y);
			offset_widget_position(position, axis_x, x);
			offset_widget_position(position, axis_y, y);
		}
		if (transform->flags & 1)
		{
			real x = widget_transform_value(&transform->scale_x);
			real y = widget_transform_value(&transform->scale_y);
			axis_x->x *= x;
			axis_x->y *= x;
			axis_y->x *= y;
			axis_y->y *= y;
		}
	}
}
