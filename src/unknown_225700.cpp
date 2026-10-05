// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_225700.CPP: two decaying per-console levels raised by the players
   (g_510a0c, a real level, and g_510a10, a count), lowered over time, and
   the times of their last changes (g_502124); a lifecycle callback (entry
   15, initialize) resets the times */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_120d80.h"
#include <xtl.h>
#include <string.h>

struct s_unknown_225700
{
	dword level_time;
	dword count_time;
	long unknown8;
	dword update_time;
};

/* a player (g_4e8c24) as seen here: its gamepad */
struct s_unknown_225700_player
{
	byte unknown00[0x24];
	long gamepad_index;
};

/* the player slots (g_54e8e0) as seen here */
struct s_unknown_225700_slot
{
	byte unknown000[0x46c];
	bool level_reached;
	byte unknown46d[0xc70 - 0x46d];
};

s_unknown_225700 g_502124;
real g_510a0c;
long g_510a10;

/* the tuning values */
dword g_4cf74c;
dword g_4cf750;
real g_4cf754;
long g_4cf758;
real g_4cf75c;

void function_18fe9e(long gamepad_index);

PRIVATE inline s_unknown_225700_player *unknown_225700_player_get(long player_index)
{
	return (s_unknown_225700_player *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
}

PRIVATE inline s_unknown_225700 *unknown_225700_get(long player_index)
{
	s_unknown_225700 *result = NULL;

	if (player_index != NONE)
	{
		s_unknown_225700_player *player = unknown_225700_player_get(player_index);

		if (player->gamepad_index != NONE)
			result = &g_502124;
	}
	return result;
}

// @retail 0x225700
void function_225700(void)
{
	memset(&g_502124, 0, sizeof(g_502124));
	g_502124.unknown8 = NONE;
}

// @retail 0x225730
void function_225730(void)
{
	dword time = GetTickCount();
	long index;
	s_unknown_225700_player *player;

	for (index = 0; index < 4 && g_4e8c20->entries[index] == NONE; index++)
	{
	}
	if (index >= 4)
		index = NONE;

	while (index != NONE)
	{
		long player_index = index != NONE ? g_4e8c20->entries[index] : NONE;

		player = unknown_225700_player_get(player_index);
		if (player_index != NONE && player->gamepad_index != NONE)
		{
			bool changed = true;

			if (g_510a0c > 0.0f && time - g_502124.level_time > g_4cf74c)
			{
				real level = g_510a0c - 1.0f;

				if (!(level > 0.0f))
					level = 0.0f;
				g_502124.level_time = time;
				g_510a0c = level;
				global_preferences_globals.dirty = changed;
				if (g_4cf754 > level)
				{
					((s_unknown_225700_slot *)g_54e8e0)[player->gamepad_index].level_reached = false;
					function_18fe9e(player->gamepad_index);
				}
			}
			if (g_510a10 > 0 && time - g_502124.count_time > g_4cf750)
			{
				long count = g_510a10 - 1;

				g_502124.count_time = time;
				global_preferences_globals.dirty = changed;
				g_510a10 = count > 0 ? count : 0;
			}
			g_502124.update_time = time;
			return;
		}

		{
			long next = index != NONE ? index + 1 : 0;

			index = NONE;
			for (; next < 4; next++)
			{
				if (g_4e8c20->entries[next] != NONE)
				{
					index = next;
					break;
				}
			}
		}
	}
}

// @retail 0x225880
void function_225880(
	long player_index)
{
	if (player_index != NONE)
	{
		s_unknown_225700_player *player = unknown_225700_player_get(player_index);

		if (player->gamepad_index != NONE)
		{
			s_unknown_225700 *state = &g_502124;
			real level;

			state->level_time = GetTickCount();
			level = (real)(long)((long)g_510a0c + 1.0f);
			if (level >= g_4cf754)
			{
				((s_unknown_225700_slot *)g_54e8e0)[player->gamepad_index].level_reached = true;
				function_18fe9e(player->gamepad_index);
			}
			g_510a0c = level;
			global_preferences_globals.dirty = true;
		}
	}
}

// @retail 0x225910
void function_225910(
	long player_index)
{
	if (player_index != NONE)
	{
		s_unknown_225700_player *player = unknown_225700_player_get(player_index);

		if (player->gamepad_index != NONE)
		{
			s_unknown_225700 *state = &g_502124;
			real level;

			state->level_time = GetTickCount();
			level = (real)(long)((long)g_510a0c + g_4cf75c);
			if (level >= g_4cf754)
			{
				((s_unknown_225700_slot *)g_54e8e0)[player->gamepad_index].level_reached = true;
				function_18fe9e(player->gamepad_index);
			}
			g_510a0c = level;
			global_preferences_globals.dirty = true;
		}
	}
}

// @retail 0x2259a0
void function_2259a0(
	long player_index)
{
	s_unknown_225700 *state = unknown_225700_get(player_index);

	if (state)
	{
		long count = g_510a10;

		state->count_time = GetTickCount();
		g_510a10 = count + 1;
		global_preferences_globals.dirty = true;
	}
}

// @retail 0x2259f0
bool function_2259f0(
	long player_index)
{
	s_unknown_225700 *state = unknown_225700_get(player_index);
	bool result = false;

	if (state && g_510a10 <= g_4cf758)
		result = true;
	return result;
}
