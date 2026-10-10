// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1E9700.CPP: a value of the difficulty table for the current
   campaign difficulty (lane M; called by the behaviors of 0x1a8000..0x1affff) */

#include "unknown_11c920.h"
#include "globals.h"
#include <string.h>
#include <xmmintrin.h>
#include "slot_handler.h"

real function_1e96a0(short column, short row);

// @retail 0x1e9700
real function_1e9700(short row)
{
	long difficulty = 1;

	if (g_4e6948->state == 1)
		difficulty = g_4e6948->difficulty;
	return function_1e96a0(difficulty, row);
}

struct s_object_values
{
	long field_0;
	short values[16][14];
	short team_values[8][9];
};

void function_b58c0(long index, dword mask);

// @retail 0x1e9c90
void function_1e9c90(long object_index, long column, s_object_values *table)
{
	s_object_values *const *table_reference = &table;
	long index = object_index & 0xffff;
	(*table_reference)->values[index][column] = 0;
	if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value28 != NONE)
		function_b58c0(g_4e9ae8->value28, 1 << index);
}

// @retail 0x1e9f40
void function_1e9f40(long object_index, s_object_values *table)
{
	s_object_values *const *table_reference = &table;
	long index = object_index & 0xffff;
	memset((*table_reference)->values[index], 0, sizeof(table->values[index]));
	if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value28 != NONE)
		function_b58c0(g_4e9ae8->value28, 1 << index);
}

// @retail 0x1e97b0
void function_1e97b0(s_object_values *table)
{
	s_object_values *const *table_reference = &table;
	for (long i = 0; i < 16; i++)
	{
		short *row = (*table_reference)->values[i];
		row[9] = 0;
		*(long *)&row[12] = 0;
		row[10] = 0;
		if (row[0] != 0)
		{
			row[1] += row[0];
			if (!g_4e6948->flag1128)
				row[0] = 0;
			if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value28 != NONE)
				function_b58c0(g_4e9ae8->value28, 1 << i);
		}
	}
	long team = 0;
	do
	{
		short *row = (*table_reference)->team_values[team];
		if (row[0] != 0)
		{
			row[1] += row[0];
			if (!g_4e6948->flag1128)
				row[0] = 0;
			if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value28 != NONE)
				function_b58c0(g_4e9ae8->value28, 1 << (team + 16));
		}
		team++;
	} while (team < 8);
}

struct s_object_values_packet
{
	short values[16][9];
	short team_values[8][9];
};

struct s_team_value_row
{
	short first;
	short values[8];
};

PRIVATE __forceinline bool update_team_value_row(s_team_value_row *destination, const short *source)
{
	bool same = true;
    short *values = destination->values;
	if (destination->first != source[0]) { destination->first = source[0]; same = false; }
	if (values[0] != source[1]) { values[0] = source[1]; same = false; }
	if (values[1] != source[2]) { values[1] = source[2]; same = false; }
	if (values[2] != source[3]) { values[2] = source[3]; same = false; }
	if (values[3] != source[4]) { values[3] = source[4]; same = false; }
	if (values[4] != source[5]) { values[4] = source[5]; same = false; }
	if (values[5] != source[6]) { values[5] = source[6]; same = false; }
	if (values[6] != source[7]) { values[6] = source[7]; same = false; }
	if (values[7] != source[8]) { values[7] = source[8]; same = false; }
	return same;
}

PRIVATE __forceinline bool update_value_row(short *destination, const short *source)
{
	bool same = true;
	if (destination[0] != source[0]) { destination[0] = source[0]; same = false; }
	if (destination[1] != source[1]) { destination[1] = source[1]; same = false; }
	if (destination[2] != source[2]) { destination[2] = source[2]; same = false; }
	if (destination[3] != source[3]) { destination[3] = source[3]; same = false; }
	if (destination[4] != source[4]) { destination[4] = source[4]; same = false; }
	if (destination[5] != source[5]) { destination[5] = source[5]; same = false; }
	if (destination[6] != source[6]) { destination[6] = source[6]; same = false; }
	if (destination[7] != source[7]) { destination[7] = source[7]; same = false; }
	if (destination[8] != source[8]) { destination[8] = source[8]; same = false; }
	return same;
}

// @retail 0x1e98e0
void __stdcall function_1e98e0(void *state, long mask_address, void *data)
{
	void *const *state_reference = &state;
	const long *mask_reference = &mask_address;
	void *const *data_reference = &data;
	s_object_values *table = (s_object_values *)*state_reference;
	s_object_values_packet *packet = (s_object_values_packet *)*data_reference;
	dword *mask = (dword *)*mask_reference;
	dword changed = 0;
	for (long i = 0; i < 16; i++)
	{
		if ((1 << i) & *mask)
		{
			if (!update_value_row(packet->values[i], table->values[i]))
				changed |= 1 << i;
			else
				changed &= ~(1 << i);
		}
	}
	for (long team = 0; team < 8; team++)
	{
		if (*mask & (1 << (team + 16)))
		{
			if (!update_team_value_row((s_team_value_row *)packet->team_values[team], table->team_values[team]))
				changed |= 1 << (team + 16);
			else
				changed &= ~(1 << (team + 16));
		}
	}
	*mask = changed;
}

void function_225880(long player_index);
void function_225910(long player_index);

// @retail 0x1e9ad0
bool __stdcall function_1e9ad0(void *state, long mask, void *data)
{
	void *const *state_reference = &state;
	const long *mask_reference = &mask;
	void *const *data_reference = &data;
	s_object_values *table = (s_object_values *)*state_reference;
	s_object_values_packet *packet = (s_object_values_packet *)*data_reference;
	short *row = table->values[0];
 long row_offset = 0;
 for (long i = 0; i < 16; i++, row += sizeof(table->values[0]) / sizeof(short), row_offset += 9)
	{
		if (*mask_reference & (1 << i))
		{
			for (long column = 0; column < 9; column++)
			{
				if (column == 5 || column == 8)
				{
					long count = packet->values[0][row_offset + column] - row[column];
					if (count > 0)
					{
						long player_index = NONE;
						if (i != NONE && i >= 0 && i < g_4e8c24->high_water_index)
						{
							short salt = *(short *)(g_4e8c24->data + i * g_4e8c24->size);
							if (salt)
								player_index = (salt << 16) | i;
						}
						if (player_index != NONE)
						{
							byte *player = NULL;
							long index = player_index & 0xffff;
							if (index < g_4e8c24->high_water_index)
							{
								byte *entry = g_4e8c24->data + index * g_4e8c24->size;
								if (*(short *)entry && *(short *)entry == player_index >> 16)
									player = entry;
							}
							if (player && *(short *)(player + 0x28) != NONE)
							{
								do
								{
									if (column == 8)
										function_225910(player_index);
									else
										function_225880(player_index);
								} while (--count);
							}
						}
					}
				}
				row[column] = packet->values[0][row_offset + column];
			}
		}
	}
	for (long team = 0; team < 8; team++)
	{
		if (*mask_reference & (1 << (team + 16)))
		{
			for (long column = 0; column < 9; column++)
				table->team_values[team][column] = packet->team_values[team][column];
		}
	}
	return true;
}

typedef void (__stdcall *text_callback)(const char *text, long context);

struct s_text_callback_state
{
	text_callback begin;
	text_callback field_4;
	text_callback end;
	long context;
	text_callback override_begin;
	text_callback field_14;
	text_callback override_end;
	long override_context;
	bool disabled;
	bool pending;
	char text[0x100];
	long index;
	long value;
};

s_text_callback_state g_4f55f8;

// @retail 0x1e9650
void function_1e9650()
{
	if (g_4f55f8.pending)
	{
		if (!g_4f55f8.disabled)
		{
			if (g_4f55f8.override_end)
				g_4f55f8.override_end(g_4f55f8.text, g_4f55f8.override_context);
			else if (g_4f55f8.end)
				g_4f55f8.end(g_4f55f8.text, g_4f55f8.context);
		}
		g_4f55f8.pending = false;
	}
}

// @retail 0x1e95d0
void __stdcall function_1e95d0(const char *text)
{
	const char *const *text_reference = &text;
	function_1e9650();
	g_4f55f8.pending = true;
	strncpy(g_4f55f8.text, *text_reference, sizeof(g_4f55f8.text));
	g_4f55f8.text[0xff] = 0;
	g_4f55f8.index = NONE;
	g_4f55f8.value = 0;
	if (!g_4f55f8.disabled)
	{
		if (g_4f55f8.override_begin)
			g_4f55f8.override_begin(g_4f55f8.text, g_4f55f8.override_context);
		else if (g_4f55f8.begin)
			g_4f55f8.begin(g_4f55f8.text, g_4f55f8.context);
	}
}

// @retail 0x1e9e80
void function_1e9e80(long first, s_object_values *table, long second)
{
	long const *second_reference = &second;
	short temporary[14];
	if (g_4e6948->mode != 4)
	{
		memcpy(temporary, table->values[first], sizeof(temporary));
		memcpy(table->values[first], table->values[*second_reference], sizeof(temporary));
		memcpy(table->values[*second_reference], temporary, sizeof(temporary));
		if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value28 != NONE)
			function_b58c0(g_4e9ae8->value28, 1 << first);
		if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value28 != NONE)
			function_b58c0(g_4e9ae8->value28, 1 << *second_reference);
	}
}

/* The 35 short entries beginning at retail 0x445580. */
const short g_445580[35] =
{
	4, 5, 6, 7, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1
};

// @retail 0x1e9720
real function_1e9720(long kind, short team)
{
	short row = (short)kind;
	short difficulty = 1;
	switch (g_4e6948->state)
	{
	case 1:
		difficulty = g_4e6948->difficulty;
		if (team != NONE && team >= 0 && team < 16)
		{
			byte enemy = 1 - (byte)function_0bfe60(((s_allegiance_view *)g_4f55ec)->peace_bits, team + 16);
			if (!enemy)
			{
				short alternate = g_445580[row];
				if (alternate == NONE)
					difficulty = 1;
				else
					row = alternate;
			}
		}
		break;
	case 2:
		difficulty = 1;
		break;
	}
	return function_1e96a0(difficulty, row);
}

void function_225910(long player_index);
void function_162bf0(long player_index, long spectated_player_index);

struct s_counted_player
{
	byte field_0[0x28];
	short local_index;
	byte field_2a[0x21c - 0x2a];
};

// @retail 0x1ea880
void function_1ea880(long arg_1f407d, long player_index, s_object_values *table)
{
	s_object_values *const *table_reference = &table;
	if (player_index != NONE && arg_1f407d != player_index)
	{
		long index = player_index & 0xffff;
		if (((s_counted_player *)g_4e8c24->data)[index].local_index != NONE)
			function_225910(player_index);
		function_162bf0(arg_1f407d, player_index);
		short *entry = &(*table_reference)->values[index][8];
		long value = *entry + 1;
		if (value < -30000)
			value = -30000;
		else if (value > 30000)
			value = 30000;
		*entry = (short)value;
		if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value28 != NONE)
			function_b58c0(g_4e9ae8->value28, 1 << index);
	}
}

struct s_statborg;
void function_1968b0(long team, long index, long counter, long value);

struct s_request_screen_bounds
{
	short top, left, bottom, right;
};

void function_1e90b0(long first, point2f *points, s_request_screen_bounds const *bounds);

extern long g_4b9ed8;
extern short g_4b9dd0, g_4b9dd2;
struct s_bitmap_data;
struct D3DTexture;
struct s_widget_quad_2b11;
struct s_float_rect;
D3DTexture *function_3bcb0(s_bitmap_data *bitmap);
void function_22a664(s_widget_quad_2b11 const *quad, s_float_rect const *coordinates,
	long bitmap_index, long sequence, long color);

// @retail 0x1e9140
bool __stdcall function_1e9140(s_request_screen_bounds const *volatile bounds)
{
	// Retail keeps the bounds argument on the stack through the texture lookups.
	s_request_screen_bounds const *volatile const *bounds_reference = &bounds;
	byte *definition = (byte *)g_510c94;
	long color = *(long *)(definition + 0x444);
	bool result = true;
	long diagonal, corner, edge, center;
	bool alternate = false;
	if (g_4b9ed8 != NONE)
	{
		long player_index = g_4e8c20->entries[g_4b9ed8];
		if (player_index != NONE)
		{
			byte state = g_4e8c24->data[(player_index & 0xffff) * 0x21c + 0x88];
			alternate = state == 1 || state == 3;
		}
	}
	if (alternate)
	{
		corner = *(long *)(definition + 0x47c);
		diagonal = *(long *)(definition + 0x46c);
		edge = *(long *)(definition + 0x474);
		center = *(long *)(definition + 0x484);
	}
	else
	{
		corner = *(long *)(definition + 0x45c);
		diagonal = *(long *)(definition + 0x44c);
		edge = *(long *)(definition + 0x454);
		center = *(long *)(definition + 0x464);
	}
	if (color != NONE && corner != NONE && diagonal != NONE && edge != NONE && center != NONE)
	{
		s_bitmap_data *corner_bitmap = *(s_bitmap_data **)(g_4e3b44[corner & 0xffff].bytes + 0x48);
		s_bitmap_data *diagonal_bitmap = *(s_bitmap_data **)(g_4e3b44[diagonal & 0xffff].bytes + 0x48);
		s_bitmap_data *edge_bitmap = *(s_bitmap_data **)(g_4e3b44[edge & 0xffff].bytes + 0x48);
		s_bitmap_data *center_bitmap = *(s_bitmap_data **)(g_4e3b44[center & 0xffff].bytes + 0x48);
		if (!function_3bcb0(corner_bitmap))
			result = false;
		if (!function_3bcb0(diagonal_bitmap))
			result = false;
		if (!function_3bcb0(edge_bitmap))
			result = false;
		if (!function_3bcb0(center_bitmap))
			return false;
		if (result)
		{
			s_request_screen_bounds const *input = *bounds_reference;
			__declspec(align(8)) s_request_screen_bounds inner;
			inner.left = input->left + 8 + g_4b9dd2;
			inner.right = input->right - 8 + g_4b9dd2;
			inner.bottom = input->bottom - 8 + g_4b9dd0;
			inner.top = input->top + 8 + g_4b9dd0;
			s_request_screen_bounds piece = inner;
			real coordinates[4];
			_mm_store_ss(&coordinates[0], _mm_setzero_ps());
			_mm_store_ss(&coordinates[1], _mm_set_ss(1.0f));
			_mm_store_ss(&coordinates[2], _mm_setzero_ps());
			_mm_store_ss(&coordinates[3], _mm_set_ss(1.0f));
			point2f points[4];
			function_1e90b0(0, points, &piece);
			function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)coordinates, center, 0, color);
			short top = inner.top - 10;
			piece.top = top;
			piece.left = inner.left;
			piece.right = inner.right;
			piece.bottom = inner.top;
			function_1e90b0(0, points, &piece);
			function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)coordinates, edge, 0, color);
			short bottom = inner.bottom + 10;
			piece.top = inner.bottom;
			piece.bottom = bottom;
			piece.left = inner.left;
			piece.right = inner.right;
			function_1e90b0(2, points, &piece);
			function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)coordinates, edge, 0, color);
			short left = inner.left - 10;
			piece.left = left;
			piece.bottom = inner.bottom;
			piece.top = inner.top;
			piece.right = inner.left;
			function_1e90b0(1, points, &piece);
			function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)coordinates, edge, 0, color);
			short right = inner.right + 10;
			piece.right = right;
			piece.bottom = inner.bottom;
			piece.left = inner.right;
			piece.top = inner.top;
			function_1e90b0(3, points, &piece);
			function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)coordinates, edge, 0, color);
			piece.left = left;
			piece.top = top;
			piece.right = inner.left;
			piece.bottom = inner.top;
			function_1e90b0(0, points, &piece);
			function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)coordinates, corner, 0, color);
			piece.top = inner.bottom;
			piece.right = right;
			piece.bottom = bottom;
			piece.left = inner.right;
			function_1e90b0(2, points, &piece);
			function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)coordinates, corner, 0, color);
			piece.left = inner.right;
			piece.top = top;
			piece.right = right;
			piece.bottom = inner.top;
			function_1e90b0(0, points, &piece);
			function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)coordinates, diagonal, 0, color);
			piece.left = left;
			piece.top = inner.bottom;
			piece.bottom = bottom;
			piece.right = inner.left;
			function_1e90b0(2, points, &piece);
			function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)coordinates, diagonal, 0, color);
		}
	}
	return result;
}

#if 0
// Activating this body regresses 0x15ca10, 0x2bce70 and 0x2c0a90.
// Disabled retail draft 0x1e9df0
void function_1e9df0(long field, long counter, s_statborg *statistics, long team, long volatile delta)
{
 short *entry = &((s_object_values *)statistics)->team_values[team][field];
 long value = *entry + (short)delta;
 if (value < -30000)
  value = -30000;
 else if (value > 30000)
  value = 30000;
 *entry = (short)value;
 if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value28 != NONE)
  function_b58c0(g_4e9ae8->value28, 1 << (team + 16));
 if (counter != NONE)
  function_1968b0(NONE, team, counter, *entry);
}
#endif


#include "unknown_0259d0.h"
#include "unknown_030290.h"
#include <wchar.h>
struct s_510c4c;
extern s_510c4c *g_510c4c;
struct s_unknown_1e6a40;
extern s_unknown_1e6a40 *g_51e9c0;
struct s_33a0b_default;
extern s_33a0b_default *g_4686d4;
extern real g_4e69c0[4];
extern long g_4ba04c;
extern short_rectangle2d g_4b9dd8;
void function_13925f(long string_handle, word *buffer);
void function_1a0180(long tag_index, long string_handle, word *buffer);
void function_22d2ee(word *string, long maximum_length);
bool function_162c50(long player_index, long *spectated_player_index);
bool function_2259f0(long player_index);
bool function_22acb4(long local_player_index);
void function_13edb0(long font, long style, long justification, dword flags, color4f const *color, color4f const *field_24);
void function_13ec70(color4f const *color);
void function_13e9c0(word const *text, short_rectangle2d const *bounds, short_rectangle2d *a, short_rectangle2d *b, real scale);
long function_1e8cb0(long index);
struct s_entry_420;
s_entry_420 *function_1e8d50(long index);
class c_1fa50
{
public:
 void function_1fa50(short_rectangle2d const *bounds, void const *clip, void const *position,
  long line_gap, real scale, long color, void const *shadow) const;
 void function_1fb10(short_rectangle2d const *bounds, real scale) const;
};

// @retail 0x1e6fe0
void function_1e6fe0(long local_player_index)
{
 byte *globals = (byte *)g_51e9c0;
 if (!globals[0x6c5])
  return;
 word text[256];
 word temporary[256];
 text[0] = 0;
 bool spectating = false;
 if (local_player_index != NONE)
 {
  long player_index = g_4e8c20->entries[local_player_index];
  long spectated_player;
  if (player_index != NONE && function_162c50(player_index, &spectated_player))
  {
   *(long *)((byte *)g_510c4c + 0x1b8) = spectated_player;
   temporary[0] = 0;
   long string_handle = *(long *)((byte *)g_510c94 + (0x43c - function_2259f0(player_index) * 4));
   function_13925f(string_handle, temporary);
   wcsncpy((wchar_t *)text, (wchar_t const *)temporary, 255);
   text[255] = 0;
   function_22d2ee(text, 256);
   *(long *)((byte *)g_510c4c + 0x1b8) = NONE;
   spectating = true;
  }
 }
 if (!spectating)
 {
  long string_handle;
  if (globals[0x6c4] && *(long *)(globals + 0x6c0) != 0 && *(long *)(globals + 0x6c0) != NONE)
   string_handle = *(long *)(globals + 0x6c0);
  else
  {
   byte *state = globals + local_player_index * 0x1b0;
   long index = *(long *)(state + 0x80);
   if (index == NONE)
    return;
   short transition = *(short *)(globals + local_player_index * 0x1b0 + index * 4);
   if (transition != 2 && transition != 3)
    return;
   byte *entry = (byte *)function_1e8d50(index);
   if (!entry)
    return;
   word delay = *(word *)(entry + 4);
   if (delay && g_510c54->field_2_3 * delay <= *(long *)(state + 0x84))
    return;
   string_handle = function_1e8cb0(*(long *)(state + 0x80));
  }
  temporary[0] = 0;
  if (g_510c94 && g_510c94->string_list != NONE)
   function_1a0180(g_510c94->string_list, string_handle, temporary);
  wcsncpy((wchar_t *)text, (wchar_t const *)temporary, 255);
  text[255] = 0;
  function_22d2ee(text, 256);
 }
 if (!text[0])
  return;
 color4f default_color = *(color4f const *)g_4686d4;
 long font = g_4ba04c <= 1 ? 6 : 5;
 color4f color;
 bool alternate = function_22acb4(g_4b9ed8);
 if (alternate)
 {
 color.alpha = g_4b9ed8 >= 0 && g_4b9ed8 < 4 ? g_4e69c0[g_4b9ed8] : 1.0f;
  color.red = 0.9647058844566345f;
  color.green = 0.8627451062202454f;
 }
 else
 {
 color.alpha = g_4b9ed8 >= 0 && g_4b9ed8 < 4 ? g_4e69c0[g_4b9ed8] : 1.0f;
  color.red = 0.7137255072593689f;
  color.green = 0.8588235378265381f;
 }
 color.blue = 1.0f;
 default_color.alpha = g_4b9ed8 >= 0 && g_4b9ed8 < 4 ? g_4e69c0[g_4b9ed8] : 1.0f;
 function_13edb0(font, NONE, 2, 0, &color, &default_color);
 function_13ec70(&color);
 short_rectangle2d limits;
 limits.top = -32768; limits.left = -32768; limits.bottom = 32767; limits.right = 32767;
 short_rectangle2d bounds, measured;
 function_13e9c0(text, &limits, &bounds, &measured, 1.0f);
 long center_x = (g_4b9dd8.left + g_4b9dd8.right) / 2 - g_4b9dd2;
 long center_y = (g_4b9dd8.top + g_4b9dd8.bottom) / 2 - g_4b9dd0;
 long half_width = (short)(bounds.right - bounds.left) / 2 + 5;
 long half_height = (short)(bounds.bottom - bounds.top) / 2 + 1;
 limits.left = (short)(center_x - half_width);
 limits.right = (short)(center_x + half_width);
 limits.top = (short)(center_y - half_height);
 limits.bottom = (short)(center_y + half_height);
 if (function_1e9140((s_request_screen_bounds const *)&limits))
  ((c_1fa50 const *)text)->function_1fa50(&limits, NULL, NULL, 0, 1.0f, 0, NULL);
}

// @retail 0x1e90b0
void function_1e90b0(long first, point2f *points, const s_request_screen_bounds *bounds)
{
	real left = (real)bounds->left;
	real right = (real)bounds->right;
	real top = (real)bounds->top;
	real bottom = (real)bounds->bottom;
	points[first % 4].x = left;
	points[first % 4].y = top;
	points[(first + 1) % 4].x = right;
	points[(first + 1) % 4].y = top;
	points[(first + 2) % 4].x = right;
	points[(first + 2) % 4].y = bottom;
	points[(first + 3) % 4].x = left;
	points[(first + 3) % 4].y = bottom;
}
