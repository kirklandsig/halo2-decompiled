#include "unknown_11c920.h"
#include "globals.h"
#include "game_engine_events.h"
#include <string.h>

// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_score_player_entry
{
	short values[14];
};

struct s_score_team_entry
{
	short values[9];
};

struct s_score_table
{
	s_score_player_entry players[16];
	dword field_1c0;
	s_score_team_entry teams[8];
};

struct s_score_display
{
	long players[16];
	short teams[8];
	char player_ranks[16];
	char team_ranks[8];
	short player_count;
	short team_count;
};

struct s_score_sort_context
{
	const byte *criteria;
	bool fallback;
};

bool function_15db80(long team);
bool function_161e10(long team);
bool function_15db30(long player_index);
bool function_15eaf0();

// @retail 0x23f990
long function_23f990(long first, long second, char criterion, bool fallback)
{
	c_engine_peer *engine = g_55e4d0[g_4e9ae8->engine_index];
	s_score_table *table = 0;
	if (engine)
		table = (s_score_table *)((byte *)g_4e9ae8 + 0x304);
	long result = 0;
	if (first != NONE && second != NONE && engine)
	{
		byte teams = ((byte *)g_4e6948)[0x184] & 1;
		volatile byte observed_teams = teams;
		if (teams && first != second)
		{
		switch (criterion)
		{
		case 10:
			result = (function_161e10(second) ? 1L : 0L) - (function_161e10(first) ? 1L : 0L);
			break;
		case 11:
			result = (long)!function_15db80(second) - (long)!function_15db80(first);
			break;
		case 12:
			result = table->teams[second].values[7] - table->teams[first].values[7];
			break;
		case 13:
			result = table->teams[second].values[0] - table->teams[first].values[0];
			break;
		case 14:
			result = table->teams[second].values[1] - table->teams[first].values[1]
				- table->teams[first].values[0] + table->teams[second].values[0];
			break;
		case 15:
			result = table->teams[second].values[2] - table->teams[first].values[2];
			break;
		case 16:
			result = table->teams[first].values[3] - table->teams[second].values[3];
			break;
		case 17:
			result = table->teams[second].values[6] - table->teams[first].values[6];
			break;
		case 18:
			if (fallback)
				result = first - second;
			break;
		}
		}
	}
	return result;
}

// @retail 0x23fba0
long function_23fba0(const byte *criteria, long first, long second, bool fallback)
{
	long result = 0;
	if (*(const volatile byte *)criteria != 0xff)
	{
		do
		{
			result = function_23f990(first, second, (char)(dword)*criteria, fallback);
			if (result)
				break;
			criteria++;
		} while (*(const char *)criteria != -1);
	}
	return result;
}

// @retail 0x23fc10
bool __stdcall function_23fc10(word first, word second, const void *context)
{
	const s_score_sort_context *sort = (const s_score_sort_context *)context;
	return function_23fba0(sort->criteria, (short)first, (short)second, sort->fallback) > 0;
}

struct s_score_player_view
{
	word identifier;
	byte flags;
	byte unknown03[0xc0 - 3];
	char team;
	byte unknownc1[0x21c - 0xc1];
};

PRIVATE inline long score_inactive_player_order(const s_score_player_view *first, const s_score_player_view *second)
{
	return (long)!((dword)second->flags >> 1 & 1) - (long)!((dword)first->flags >> 1 & 1);
}

PRIVATE inline s_score_player_view *score_player_at(long identifier)
{
	return (s_score_player_view *)g_4e8c24->data + (identifier & 0xffff);
}

// @retail 0x23f760
long __stdcall function_23f760(long first, long second, char criterion, bool fallback)
{
	s_score_table *table = 0;
	if (g_55e4d0[g_4e9ae8->engine_index])
		table = (s_score_table *)((byte *)g_4e9ae8 + 0x304);
	s_score_player_view *a = score_player_at(first);
	s_score_player_view *b = score_player_at(second);
	long result = 0;
	switch (criterion)
	{
	case 0:
		if (!function_15eaf0())
			result = score_inactive_player_order(a, b);
		break;
	case 1:
		{
			long first_flag = (dword)a->flags & 1;
			long second_flag = (dword)b->flags & 1;
			result = second_flag - first_flag;
		}
		break;
	case 2:
		result = (long)!function_15db30(second) - (long)!function_15db30(first);
		break;
	case 3:
		{
			long second_valid = b->team != -1;
			long first_valid = a->team != -1;
			result = second_valid - first_valid;
		}
		break;
	case 4:
		result = table->players[second & 0xffff].values[9] - table->players[first & 0xffff].values[9];
		break;
	case 5:
		result = table->players[second & 0xffff].values[2] - table->players[first & 0xffff].values[2];
		break;
	case 6:
		result = table->players[second & 0xffff].values[3] - table->players[first & 0xffff].values[3]
			- table->players[first & 0xffff].values[2] + table->players[second & 0xffff].values[2];
		break;
	case 7:
		result = table->players[second & 0xffff].values[4] - table->players[first & 0xffff].values[4];
		break;
	case 8:
		result = table->players[first & 0xffff].values[5] - table->players[second & 0xffff].values[5];
		break;
	case 9:
		result = table->players[second & 0xffff].values[8] - table->players[first & 0xffff].values[8];
		break;
	case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18:
		result = function_23f990(a->team, b->team, criterion, fallback);
		break;
	}
	return result;
}

// @retail 0x23fb60
long function_23fb60(const byte *criteria, long first, long second, bool fallback)
{
	long result = 0;
	if (*(const volatile byte *)criteria != 0xff)
	{
		do
		{
			result = function_23f760(first, second, (char)(dword)*criteria, fallback);
			if (result)
				break;
			criteria++;
		} while (*(const char *)criteria != -1);
	}
	return result;
}

// @retail 0x23fbe0
bool __stdcall function_23fbe0(long first, long second, const void *context)
{
	const s_score_sort_context *sort = (const s_score_sort_context *)context;
	return function_23fb60(sort->criteria, first, second, sort->fallback) > 0;
}

PRIVATE const byte g_459cec[] = {0, 2, 3, 10, 12, 14, 18, 4, 6, 0xff};
PRIVATE const byte g_459cf8[] = {0, 2, 3, 10, 12, 14, 15, 16, 17, 18, 4, 6, 7, 8, 9, 0xff};
PRIVATE const byte g_459d08[] = {0, 2, 3, 10, 13, 18, 5, 0xff};
PRIVATE const byte g_459d10[] = {0xff};

PRIVATE inline const byte *score_criteria(long mode)
{
	const byte *criteria;
	switch (mode)
	{
	case 0:
		criteria = g_459d08;
		break;
	case 1:
		if ((bool)((*(dword *)((byte *)g_4e6948 + 0x184) >> 4) & 1))
			criteria = g_459cf8;
		else
			criteria = g_459cec;
		break;
	default:
		criteria = g_459d10;
		break;
	}
	return criteria;
}

// @retail 0x23f360
long function_23f360(long team, long mode)
{
	long tied = 0;
	long better = 0;
	const byte *criteria = score_criteria(mode);
	long i = 0;
	do
	{
		if (team != i)
		{
			long order = function_23fba0(criteria, team, i, false);
			if (order > 0)
				better++;
			else if (order == 0)
				tied++;
		}
		i++;
	} while (i < 8);
	return (tied > 0) + 2 * better;
}

// @retail 0x23fd10
long function_23fd10(long mode, long team, const s_score_display *display)
{
	long better = 0;
	long tied = 0;
	const byte *criteria = score_criteria(mode);
	for (long i = 0; i < display->team_count; i++)
	{
		long other = display->teams[i];
		if (team != other)
		{
			long order = function_23fba0(criteria, team, other, false);
			if (order > 0)
				better++;
			else if (order == 0)
				tied++;
			else
				break;
		}
	}
	return (tied > 0) + 2 * better;
}

// @retail 0x23fc40
long function_23fc40(long mode, long player, const s_score_display *display)
{
	long better = 0;
	long tied = 0;
	const byte *criteria = score_criteria(mode);
	for (long i = 0; i < display->player_count; i++)
	{
		long other = display->players[i];
		if (player != other)
		{
			long order = 0;
			const byte *criterion = criteria;
			while (*criterion != 0xff)
			{
				order = function_23f760(player, other, *criterion, false);
				if (order)
					break;
				criterion++;
			}
			if (order > 0)
				better++;
			else if (order == 0)
				tied++;
			else
				break;
		}
	}
	return (tied > 0) + 2 * better;
}

PRIVATE inline long score_next_player(s_record_pool *data, long index)
{
	if (index >= 0)
	{
		for (; index < data->high_water_index; index++)
		{
			if (data->bitmap[index >> 5] & (1 << (index & 0x1f)))
				return index;
		}
	}
	return NONE;
}

/* the next used player entry after the cursor, with its datum index */
PRIVATE __forceinline byte *score_next_entry(s_record_pool *data, long *cursor, long *datum)
{
	long i = score_next_player(data, *cursor + 1);
	if (i == NONE)
		return NULL;
	byte *entry = data->data + data->size * i;
	*cursor = i;
	*datum = (*(short *)entry << 16) | i;
	return entry;
}

// @retail 0x23f260
long function_23f260(long mode, long player, long excluded_team)
{
	long better = 0;
	long tied = 0;
	const byte *criteria = score_criteria(mode);
	s_record_pool *data = g_4e8c24;
	long cursor = NONE;
	long other;
	byte *entry;
	while ((entry = score_next_entry(data, &cursor, &other)) != NULL)
	{
		if (player != other && ((s_score_player_view *)entry)->team != (short)excluded_team)
		{
			long order = function_23fb60(criteria, player, other, false);
			if (order > 0)
				better++;
			else if (order == 0)
				tied++;
		}
	}
	return (tied > 0) + 2 * better;
}

typedef bool (__stdcall *score_compare_player)(long, long, const void *);
typedef bool (__stdcall *score_compare_team)(word, word, const void *);
void sort_4byte(long *elements, unsigned long count, void *unused, score_compare_player compare, const void *context);
void sort_2byte(word *elements, unsigned long count, void *unused, score_compare_team compare, const void *context);

/* whether the current game engine plays in teams */
PRIVATE inline bool score_has_teams(void)
{
	bool result = false;
	if (g_55e4d0[g_4e9ae8->engine_index])
		result = ((byte *)g_4e6948)[0x184] & 1;
	return result;
}

PRIVATE inline bool score_team_present(long team)
{
	bool result = false;
	if (score_has_teams() && team >= 0 && team < 8)
		result = (g_4e9ae8->we & (1 << team)) != 0;
	return result;
}

// score 4: only the load order of criteria and fallback in the two sort
// contexts differs (retail loads criteria first). Aggregate initialization and
// the statement order keep the same code. Permuter: 372 tries in 600 s, no
// lower form.
// @retail 0x23f3e0
void function_23f3e0(s_score_display *display, long mode, bool fallback)
{
	const byte *criteria = score_criteria(mode);
	s_record_pool *data = g_4e8c24;
	display->player_count = 0;
	display->team_count = 0;
	long cursor = NONE;
	long player;
	while (score_next_entry(data, &cursor, &player) != NULL)
	{
		display->players[display->player_count++] = player;
	}
	s_score_sort_context player_context;
	player_context.criteria = criteria;
	player_context.fallback = fallback;
	sort_4byte(display->players, display->player_count, &fallback, function_23fbe0, &player_context);
	for (long i = 0; i < display->player_count; i++)
		display->player_ranks[i] = (char)function_23fc40(mode, display->players[i], display);
	if (score_has_teams())
	{
		for (long team = 0; team < 8; team += 4)
		{
			if (score_team_present(team))
				display->teams[display->team_count++] = (short)team;
			if (score_team_present(team + 1))
				display->teams[display->team_count++] = (short)(team + 1);
			if (score_team_present(team + 2))
				display->teams[display->team_count++] = (short)(team + 2);
			if (score_team_present(team + 3))
				display->teams[display->team_count++] = (short)(team + 3);
		}
		s_score_sort_context team_context;
		team_context.criteria = criteria;
		team_context.fallback = fallback;
		sort_2byte((word *)display->teams, display->team_count, &fallback, function_23fc10, &team_context);
		for (long j = 0; j < display->team_count; j++)
			display->team_ranks[j] = (char)function_23fd10(mode, display->teams[j], display);
	}
}

PRIVATE inline char score_team_rank(const s_score_display *display, short identifier)
{
	long rank = NONE;
	for (long i = 0; i < 8; i++)
	{
		if (identifier == display->teams[i])
		{
			rank = display->team_ranks[i];
			break;
		}
	}
	return (char)rank;
}

PRIVATE inline char score_player_rank(const s_score_display *display, long identifier)
{
	long rank = NONE;
	for (long i = 0; i < 16; i++)
	{
		if (identifier == display->players[i])
		{
			rank = display->player_ranks[i];
			break;
		}
	}
	return (char)rank;
}

// @retail 0x23fdd0
void __stdcall function_23fdd0(const s_score_display *previous, const s_score_display *current)
{
	bool teams = false;
	if (g_55e4d0[g_4e9ae8->engine_index])
		teams = TEST_FIELD_BIT(g_4e6948->flags184.bit0);
	long previous_count = teams ? previous->team_count : previous->player_count;
	for (long i = 0; i < previous_count; i++)
	{
		char previous_rank = teams ? previous->team_ranks[i] : previous->player_ranks[i];
		if (previous_rank / 2 > 0)
			break;
		char current_rank;
		if (teams)
			current_rank = score_team_rank(current, previous->teams[i]);
		else
			current_rank = score_player_rank(current, previous->players[i]);
		bool previous_tied = (previous_rank & 1) != 0;
		long event_code = NONE;
		if (current_rank != -1)
		{
			bool current_top = current_rank < 2;
			bool current_tied = current_top && current_rank == 1;
			if (current_top)
			{
				if (previous_tied && !current_tied)
					event_code = 25 + (teams != false);
			}
			else
				event_code = 27 + (teams != false);
		}
		if (event_code != NONE)
		{
			s_event event;
			game_engine_event_initialize_inline(&event, 0, event_code);
			if (teams)
				event.cause_team = previous->teams[i];
			else
				event.cause_player_index = previous->players[i];
			game_engine_event_send_inline(&event);
		}
	}
	long current_count = teams ? current->team_count : current->player_count;
	for (long j = 0; j < current_count; j++)
	{
		long current_rank = teams ? current->team_ranks[j] : current->player_ranks[j];
		if (current_rank >= 2)
			break;
		char previous_rank;
		if (teams)
			previous_rank = score_team_rank(previous, current->teams[j]);
		else
			previous_rank = score_player_rank(previous, current->players[j]);
		long event_code = NONE;
		if (previous_rank != -1 && previous_rank >= 2)
		{
			if (current_rank == 1)
			{
				long mode = g_4e6948->mode_180;
				if (mode != 3 && mode != 4 && mode != 8)
					event_code = 29 + (teams != false);
			}
			else
				event_code = 25 + (teams != false);
		}
		if (event_code != NONE)
		{
			s_event event;
			game_engine_event_initialize_inline(&event, 0, event_code);
			if (teams)
				event.cause_team = current->teams[j];
			else
				event.cause_player_index = current->players[j];
			game_engine_event_send_inline(&event);
		}
	}
}

// @retail 0x23f6e0
void function_23f6e0()
{
	s_score_display display;
	s_mp_globals *globals = g_4e9ae8;
	if (g_55e4d0[globals->engine_index] && globals->w6c == 1 &&
		(g_4e6948->mode == 4 || globals->lc04 == 1) && (*(dword *)globals & 0x20))
	{
		function_23f3e0(&display, 0, false);
		function_23fdd0((s_score_display *)((byte *)g_4e9ae8 + 0x74), &display);
		memcpy((byte *)g_4e9ae8 + 0x74, &display, sizeof(display));
		*(dword *)g_4e9ae8 &= ~0x20;
	}
}
