// @flags /O2 /Gr
/* PLAYER_PROFILE_STORAGE.CPP: player profiles, each kept in a saved game file of
   type 0 under the player's name: the defaults (the guest's, named "Guest"),
   creating a profile with a random appearance, finding one by name, reading
   and repairing one, and saving one now or as a saved game file task; and a
   color from the scenario's color table, which the appearance's colors
   index. Moved here from unknown_1a06a0.cpp and unknown_1a0680.cpp. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_2accd0.h"
#include "player_profile_storage.h"
#include <string.h>
#include <wchar.h>

/* the tag header globals (g_4e034c) as this file reads them: a table of
   colors */
struct s_color_table_view
{
	byte unknown00[0x160];
	long color_count;
	color3f *colors;
};

/* copies at most count characters and terminates the copy */
static inline wchar_t *wide_string_copy(wchar_t *destination, wchar_t const *source, long count)
{
	wcsncpy(destination, source, count);
	destination[count] = 0;
	return destination;
}

struct s_entry_b;
s_entry_b *function_19c1f0(long key);

/* a color of the scenario's color table, or the default color when the
   index is out of range or no scenario is loaded */
// @retail 0x1a06a0
color3f function_1a06a0(long index)
{
	color3f result = *(color3f *)g_468710;

	if (g_4e0350)
	{
		s_color_table_view *table = (s_color_table_view *)g_4e034c;

		if (index >= 0 && index < table->color_count)
			result = table->colors[index];
	}
	return result;
}

/* the profile's highest key that function_19c1f0 finds, and its index
   (NONE and 1 when none is found) */
// @retail 0x1a06f0
void player_profile_get_highest_key(s_player_profile *profile, long *best_key, long *best_index)
{
	long i;

	*best_key = NONE;
	*best_index = 1;
	for (i = 0; i < 4; i++)
	{
		long key = profile->keys[i];

		if (key != NONE && function_19c1f0(key) && *best_key <= key)
		{
			*best_key = key;
			*best_index = i;
		}
	}
}

/* clears a profile to the defaults, with no name */
// @retail 0x1a0750
void function_1a0750(s_player_profile *profile)
{
	memset(profile, 0, sizeof(s_player_profile));
	*(volatile char *)&profile->appearance.colors[0] = 10;
	*(volatile char *)&profile->appearance.colors[1] = 0;
	*(volatile char *)&profile->appearance.colors[2] = 0;
	*(volatile char *)&profile->appearance.colors[3] = 10;
	*(volatile byte *)&profile->appearance.emblem_foreground = 0;
	*(volatile byte *)&profile->appearance.emblem_background = 0;
	dword *flags = &profile->flags;
	*flags |= 1;
	profile->value102 = 3;
	profile->type_flags = 0;
	profile->value100 = 0;
	profile->value101 = 0;
}

/* clears a profile to the defaults of a type (0 or 1) and names it "Guest" */
// @retail 0x1a07b0
void player_profile_initialize(s_player_profile *profile, long type)
{
	memset(profile, 0, sizeof(s_player_profile));
	wide_string_copy(profile->name, L"Guest", 31);
	profile->appearance.colors[0] = 10;
	profile->appearance.colors[1] = 0;
	profile->appearance.colors[2] = 0;
	char *color3 = &profile->appearance.colors[3];
	*color3 = 10;
	profile->appearance.emblem_foreground = 0;
	profile->appearance.emblem_background = 0;
	dword *flags = &profile->flags;
	*flags |= 1;
	profile->value102 = 3;
	profile->type_flags = 0;
	switch (type)
	{
	case 0:
		profile->value100 = 0;
		profile->value101 = 0;
		break;
	case 1:
		profile->value100 = 0;
		profile->value101 = 0;
		{
			dword *type_flags = &profile->type_flags;
			*type_flags |= 1;
		}
		break;
	}
}

/* the saved game files (save_file_group.cpp, not decompiled yet) */
bool function_2161d0(long file_index, void *buffer, long size);
bool saved_game_file_read_synchronous(long file_index, void *buffer, long size, wchar_t *name);

struct s_player_appearance;
bool function_153750(s_player_appearance *appearance);

/* what the sign-in screens call a player profile */
struct s_player_profile_settings;

/* reads a player profile from its saved game file, then repairs it */
// @retail 0x1a0850
bool player_profile_fetch(long file_index, s_player_profile *profile)
{
	s_player_profile file_profile;
	bool result = function_2161d0(file_index, &file_profile, sizeof(file_profile));

	if (result)
		*profile = file_profile;
	wchar_t *name = profile->name;
	name[31] = 0;
	if (profile->value151 >= 3)
		profile->value151 = 0;
	if (!function_153750((s_player_appearance *)&profile->appearance))
		memset(&profile->appearance, 0, sizeof(profile->appearance));
	return result;
}

/* a player profile: the guest's (no file), or the one saved in the file */
// @retail 0x1a0540
bool player_profile_load(long file_index, s_player_profile_settings *settings)
{
	s_player_profile *profile = (s_player_profile *)settings;

	if (file_index == NONE)
	{
		player_profile_initialize(profile, 0);
		return true;
	}
	return player_profile_fetch(file_index, profile);
}

/* writes a profile to its saved game file under the profile's name and
   waits for the write */
// @retail 0x1a0660
bool player_profile_store(long file_index, s_player_profile *profile)
{
	if (file_index != NONE)
	{
		return saved_game_file_read_synchronous(file_index, profile, sizeof(s_player_profile), profile->name);
	}
	else
	{
		return false;
	}
}

void __stdcall function_215900(long controller, long type, word *capacity, long *indices, bool include_cached);
word *function_215b50(long file_index, word *name);

/* finds the profile file whose name matches, ignoring case, and loads it;
   an empty name gives the guest's profile */
// @retail 0x1a0560
bool player_profile_find_by_name(wchar_t const *name, s_player_profile_settings *settings, long *file_index)
{
	long count = 4096;
	wchar_t file_name_buffer[128];
	long indices[4096];
	bool result = false;
	if (file_index)
		*file_index = NONE;
	if (*name)
	{
		function_215900(255, 0, (word *)&count, indices, true);
		for (long index = 0; index < (word)count; index++)
		{
			wchar_t const *file_name = (wchar_t *)function_215b50(indices[index], (word *)file_name_buffer);
			if (!_wcsicmp(file_name, name))
			{
				result = true;
				if (settings)
				{
					bool loaded = player_profile_load(indices[index], settings);
					result = loaded;
				}
				if (result && file_index)
					*file_index = indices[index];
				break;
			}
		}
	}
	else if (settings)
	{
		player_profile_initialize((s_player_profile *)settings, 0);
		result = true;
	}
	return result;
}

long function_215c00(long type, wchar_t const *name);
void __stdcall function_215e60(long file_index);

/* the next value of g_4e7408's seed, modulo a range */
PRIVATE inline long player_profile_random(long range)
{
	g_4e7408->seed = g_4e7408->seed * 1664525 + 1013904223;
	return (long)(g_4e7408->seed >> 16) % range;
}

/* creates a profile file under a name, with the defaults and a random
   appearance; returns its file index, or NONE.
   Retail uses two stack arguments and ret 8. With the standard marker,
   this body matches all 413 bytes; without it the unused first argument
   is removed and three instructions differ. No retail data or code holds
   its address. Callers 0x238537, 0x2385d6, 0x238642 and 0x2386c5 use
   register conventions themselves and push both arguments. Declaring
   __stdcall alone and the earlier parameter-address trial did not retain
   the unused stack argument. */
// @retail 0x1a03a0 standard
long __stdcall player_profile_make(long controller_index, word *name)
{
	s_player_profile profile;
	long file_index = function_215c00(0, (wchar_t const *)name);
	if (file_index != NONE)
	{
		player_profile_initialize(&profile, 0);
		profile.flags = 0;
		profile.appearance.colors[0] = (byte)player_profile_random(16);
		profile.appearance.colors[1] = (byte)player_profile_random(16);
		profile.appearance.colors[2] = (byte)player_profile_random(16);
		profile.appearance.colors[3] = (byte)player_profile_random(16);
		profile.appearance.emblem_foreground = (byte)player_profile_random(64);
		profile.appearance.emblem_background = (byte)player_profile_random(32);
		wide_string_copy(profile.name, (wchar_t const *)name, 31);
		profile.keys[0] = NONE;
		profile.keys[1] = NONE;
		profile.keys[2] = NONE;
		profile.keys[3] = NONE;
		if (!saved_game_file_read_synchronous(file_index, &profile, sizeof(profile), profile.name))
		{
			function_215e60(file_index);
			return NONE;
		}
	}
	return file_index;
}

bool function_2173d0(long file_index, wchar_t const *name, void *buffer, dword size, s_saved_game_file_task *task);

/* starts writing a profile to its saved game file as a task */
// @retail 0x1a0680
bool player_profile_store_async(s_player_profile *profile, long file_index, s_saved_game_file_task *task)
{
	bool result;
	if (file_index != NONE)
		result = function_2173d0(file_index, profile->name, profile, sizeof(s_player_profile), task);
	else
		result = false;
	return result;
}
