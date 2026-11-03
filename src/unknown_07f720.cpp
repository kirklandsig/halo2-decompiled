// @flags /O2 /Gr
/* UNKNOWN_07F720.CPP: a player's colours from their appearance and team */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_07f720.h"

color3f function_1a06a0(long index);

/* the default team colour */
color3f const *g_468734;

struct s_team_colors
{
	byte unknown00[0x10];
	long team_color_count;
	color3f *team_colors;
};

/* the multiplayer globals tag (index at +0x16c of the globals) */
struct s_multiplayer_globals
{
	long universal_count;
	s_team_colors *universal;
};

// @retail 0x7f720
color3f function_7f720(short team)
{
	short team_index = (short)team;
	long tag_index = g_4e034c->index;
	color3f result = *g_468734;

	if (tag_index != NONE)
	{
		s_team_colors *universal = ((s_multiplayer_globals *)g_4e3b44[tag_index & 0xffff].bytes)->universal;

		if (team_index >= 0 && team_index < universal->team_color_count)
		{
			result = universal->team_colors[team_index];
		}
	}

	return result;
}

// @retail 0x7f790
void function_7f790(short team_index, bool use_default, s_player_appearance const *appearance, color3f *colors)
{
	colors[0] = function_1a06a0(appearance->colors[0]);
	colors[1] = function_1a06a0(appearance->colors[1]);
	colors[2] = function_1a06a0(appearance->colors[2]);
	colors[3] = function_1a06a0(appearance->colors[3]);

	if (use_default)
	{
		colors[0] = *(color3f const *)g_468710;
		colors[1] = *(color3f const *)g_468710;
	}
	else
	{
		long team = *(volatile long *)&team_index;
		if ((short)team != NONE)
		{
			colors[0] = function_7f720(team);
			colors[1] = function_1a06a0(appearance->colors[0]);
		}
	}
}

#include <wchar.h>
#include "main_messages.h"
long function_19fd00(long index);

// @retail 0x7f5b0
bool function_7f5b0(word *name, long capacity)
{
	bool result = false;
	word text[256];
	if (g_4e034c && g_4e034c->index != NONE)
	{
		s_multiplayer_globals *settings =
			(s_multiplayer_globals *)g_4e3b44[g_4e034c->index & 0xffff].bytes;
		if (settings->universal_count > 0)
		{
			s_team_colors *universal = settings->universal;
			if (*(long *)(universal->unknown00 + 4) != NONE)
			{
				g_4e7408->seed = 1664525 * g_4e7408->seed + 1013904223;
				short index = (short)(((g_4e7408->seed >> 16) * 100) >> 16);
				text[0] = 0;
				function_1a0180(*(long *)(universal->unknown00 + 4), function_19fd00(index), text);
				wcsncpy((wchar_t *)name, (const wchar_t *)text, capacity - 1);
				name[capacity - 1] = 0;
				result = true;
			}
		}
	}
	return result;
}

// @retail 0x7f660
bool __stdcall function_07f660(wchar_t *name, long length, const wchar_t *requested, long count, const wchar_t **names)
{
	long attempt = 0;
	bool result = false;
	wcsncpy(name, L"", length - 1);
	name[length - 1] = 0;
	for (; attempt < 100; attempt++)
	{
		if (attempt == 0 && requested && wcslen(requested) > 0)
		{
			wcsncpy(name, requested, length - 1);
			name[length - 1] = 0;
		}
		else if (!function_7f5b0((word *)name, length))
			break;
		result = true;
		for (long i = 0; i < count; i++)
		{
			const wchar_t *other = names[i];
			if (!wcscmp(other, name))
			{
				result = false;
				break;
			}
		}
		if (result)
			break;
	}
	return result;
}
