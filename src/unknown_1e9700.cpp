// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1E9700.CPP: a value of the difficulty table for the current
   campaign difficulty (lane M; called by the behaviors of 0x1a8000..0x1affff) */

#include "unknown_11c920.h"
#include "globals.h"
#include <string.h>
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
	if (destination->first != source[0]) { destination->first = source[0]; same = false; }
	if (destination->values[0] != source[1]) { destination->values[0] = source[1]; same = false; }
	if (destination->values[1] != source[2]) { destination->values[1] = source[2]; same = false; }
	if (destination->values[2] != source[3]) { destination->values[2] = source[3]; same = false; }
	if (destination->values[3] != source[4]) { destination->values[3] = source[4]; same = false; }
	if (destination->values[4] != source[5]) { destination->values[4] = source[5]; same = false; }
	if (destination->values[5] != source[6]) { destination->values[5] = source[6]; same = false; }
	if (destination->values[6] != source[7]) { destination->values[6] = source[7]; same = false; }
	if (destination->values[7] != source[8]) { destination->values[7] = source[8]; same = false; }
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
	for (long i = 0; i < 16; i++)
	{
		if (*mask_reference & (1 << i))
		{
			for (long column = 0; column < 9; column++)
			{
				if (column == 5 || column == 8)
				{
					long count = packet->values[i][column] - table->values[i][column];
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
				table->values[i][column] = packet->values[i][column];
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
	short difficulty;
	if (g_4e6948->state == 1)
		difficulty = g_4e6948->difficulty;
	else
		difficulty = 1;
	if (g_4e6948->state == 2)
		difficulty = 1;
	else if (g_4e6948->state == 1 && team != NONE && team >= 0 && team < 16)
	{
		bool enemy = !function_0bfe60(((s_allegiance_view *)g_4f55ec)->peace_bits, team + 16);
		if (!enemy)
		{
			short alternate = g_445580[row];
			if (alternate == NONE)
				difficulty = 1;
			else
				row = alternate;
		}
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
