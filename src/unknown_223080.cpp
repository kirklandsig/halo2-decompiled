// @flags /O2 /arch:SSE /Gr
#include "allocator_storage.h"
/* UNKNOWN_223080.CPP: the loading screen (the progress text in the
   language of the console) */

#include "unknown_11c920.h"
#include <xtl.h>
#include "language.h"
#include <stdio.h>
#include <stdarg.h>

/* the length of a wide string, at most maximum_count characters */
PRIVATE inline long ustrnlen(wchar_t const *string, long maximum_count)
{
	long length;

	for (length = 0; length < maximum_count && string[length]; length++)
	{
	}
	return length;
}

// @retail 0x223320
wchar_t const *loading_text_get(void)
{
	switch (get_current_language())
	{
	case 1:
		return L"\x8aad\x307f\x8fbc\x3093\x3067\x3044\x307e\x3059";
	case 2:
		return L"LADEN";
	case 3:
		return L"CHARGEMENT EN COURS";
	case 4:
		return L"CARGANDO";
	case 5:
		return L"CARICAMENTO";
	case 6:
		return L"\xbd88\xb7ec\xc624\xb294 \xc911";
	case 7:
		return L"\x8f09\x5165\x4e2d";
	default:
		return L"LOADING";
	}
}

// @retail 0x223720
wchar_t *ustrnzcatf(
	wchar_t *buffer,
	wchar_t const *format,
	...)
{
	wchar_t *local_0 = buffer;
	unsigned long length = 0;
	while (length < 62)
	{
		if (!buffer[length])
			break;
		if (!buffer[length + 1])
		{
			++length;
			break;
		}
		length += 2;
	}
	long remaining = 63 - length;
	wchar_t *local_1 = buffer + length;
	va_list arguments;

	va_start(arguments, format);
	_vsnwprintf(local_1, remaining - 1, format, arguments);
	local_1[remaining - 1] = 0;
	va_end(arguments);
	return local_0;
}

#include "globals.h"
#include <string.h>

struct s_grid_pair
{
	short x, y;
};

struct s_2f6b0_point
{
	short x, y;
};

struct short_rect
{
	short v0, v1, v2, v3;
};

struct short_rect_pair
{
	short_rect a, b;
};

struct s_2f800_source;
struct s_2f800_size;
struct s_2f800_view;

struct s_223080
{
	long field_0, field_4, field_8;
	byte field_c[0x74];
	byte field_80[0x30];
	short_rect field_b0, field_b8;
	byte field_c0[0x34];
};

struct s_223081
{
	point3f field_0;
	byte field_c[0x14];
	vector3f field_20, field_2c;
	byte field_38[0x1c];
};

extern vector3f *g_4687bc;
struct s_unknown_485a88
{
    byte flag;
    short_rect_pair rects;
};
extern s_unknown_485a88 g_unknown_485a88;
#define g_485a8a (g_unknown_485a88.rects)
extern long g_4e64a0;

long window_manager_away_from_main_menu(void);
void function_2f600(long arg_0, long arg_1, s_grid_pair *arg_2);
void function_2f640(long arg_0, s_grid_pair *arg_1, s_grid_pair const *arg_2,
	long arg_3, long arg_4, s_grid_pair *arg_5);
void function_2f6b0(s_2f6b0_point const *arg_0, s_2f6b0_point const *arg_1,
	s_2f6b0_point const *arg_2, short_rect *arg_3, short_rect *arg_4);
void function_2f800(s_2f800_view *arg_3, s_2f800_source const *arg_0, s_2f800_size const *arg_1, s_2f800_size const *arg_2);
void function_2c490(long arg_0);
void function_3f450(long arg_0);

// score 2 (was 71): the 0x2f640 call sets ecx before edx, retail edx first:
// BLOCKED-BY 0x2f640, whose parameters retail declares (index, dimensions,
// extent, ...): that order at its definition and this call matches this
// function with 0x2f640 still matched, but its file is machine 1's. At 71
// (since solved): the 0x2f6b0 convention, the 0x3f450 argument's pointer
// register and the else-branch copy's rotation. Else copies swapped 71; dimensions through a pointer local 71;
// menu guard split 71; grid locals declared separately 71; arg_6 through
// a local 71; local_4 at function scope 71; arg_0 == 0 71; else rects
// through a local pair 73. Struct-copy of the pair 58 but positional
// only; field-by-field 146; arm swap 122 (earlier rounds).
// @retail 0x223080
void function_223080(long arg_0, long arg_1, long arg_2, s_223080 *arg_3,
	long arg_4, long arg_5, s_223081 const *arg_6)
{
	s_223081 local_0;
	s_grid_pair local_1, local_2, local_3;
	arg_3->field_0 = arg_0;
	arg_3->field_4 = arg_1;
	arg_3->field_8 = arg_2;
	if (!arg_0)
	{
		s_223081 const *local_4 = arg_6;
		if (g_4e6948 && g_4e6948->flag1120 && g_4e6948->state == 3 && (byte)window_manager_away_from_main_menu())
		{
			local_0 = *arg_6;
			local_0.field_0 = *g_468788;
			local_0.field_20 = *g_4687bc;
			local_0.field_2c = *g_4687a8;
			local_4 = &local_0;
		}
		function_2f600(arg_4, arg_5, &local_1);
		function_2f640(arg_1, &local_2, &local_1, arg_4, arg_5, &local_3);
		function_2f6b0((s_2f6b0_point *)&local_3, (s_2f6b0_point *)&local_1,
			(s_2f6b0_point *)&local_2, &arg_3->field_b0, &arg_3->field_b8);
		function_2f800((s_2f800_view *)arg_3->field_80, (s_2f800_source *)local_4, (s_2f800_size *)&local_2, (s_2f800_size *)&local_1);
		function_2c490(arg_4);
		if (g_4e64a0 <= 0)
			g_4e6098 = false;
		else if (g_4e6098)
			function_3f450(*(short const *)((byte const *)arg_6 + 0x10));
	}
	else
	{
		arg_3->field_b0 = g_485a8a.a;
		arg_3->field_b8 = g_485a8a.b;
		function_2f800((s_2f800_view *)arg_3->field_80, (s_2f800_source *)arg_6, 0, 0);
	}
	memcpy(arg_3->field_c, arg_3->field_80, sizeof(arg_3->field_c));
}
