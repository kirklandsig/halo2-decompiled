// @flags /O2 /Gr
/* UNKNOWN_1A06A0.CPP: a color from the scenario's table, the best of four
   entries, and the player profile defaults */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
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

/* the four keys at +0xec of the object 0x1a06f0 reads */
struct s_key_set
{
	byte unknown00[0xec];
	long keys[4];
};

/* a player profile (0x1e0 bytes) */
struct s_player_profile
{
	byte unknown00[4];
	dword flags_0 : 1;
	dword : 31;
	wchar_t name[32];
	byte unknown48[0xfc - 0x48];
	union
	{
		dword unknownfc;
		struct
		{
			dword unknownfc_0 : 1;
			dword : 31;
		};
	};
	byte unknown100;
	byte unknown101;
	byte unknown102;
	byte unknown103[0x118 - 0x103];
	byte unknown118;
	byte unknown119;
	byte unknown11a;
	byte unknown11b;
	byte unknown11c;
	byte unknown11d;
	byte unknown11e;
	byte unknown11f[0x1e0 - 0x11f];
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

// @retail 0x1a06a0
color3f *function_1a06a0(long index, color3f *color)
{
	color3f result = *(color3f *)g_468710;

	if (g_4e0350)
	{
		s_color_table_view *table = (s_color_table_view *)g_4e034c;

		if (index >= 0 && index < table->color_count)
			result = table->colors[index];
	}
	*color = result;
	return color;
}

// @retail 0x1a06f0
void function_1a06f0(s_key_set *set, long *best_key, long *best_index)
{
	long i;

	*best_key = NONE;
	*best_index = 1;
	for (i = 0; i < 4; i++)
	{
		long key = set->keys[i];

		if (key != NONE && function_19c1f0(key) && *best_key <= key)
		{
			*best_key = key;
			*best_index = i;
		}
	}
}

// @retail 0x1a0750
void function_1a0750(s_player_profile *profile)
{
	memset(profile, 0, sizeof(s_player_profile));
	profile->unknown118 = 10;
	profile->unknown119 = 0;
	profile->unknown11a = 0;
	profile->unknown11b = 10;
	profile->unknown11d = 0;
	profile->unknown11e = 0;
	profile->flags_0 = true;
	profile->unknown102 = 3;
	profile->unknownfc = 0;
	profile->unknown100 = 0;
	profile->unknown101 = 0;
}

// @retail 0x1a07b0
void function_1a07b0(s_player_profile *profile, long type)
{
	memset(profile, 0, sizeof(s_player_profile));
	wide_string_copy(profile->name, L"Guest", 31);
	profile->unknown118 = 10;
	profile->unknown119 = 0;
	profile->unknown11a = 0;
	profile->unknown11b = 10;
	profile->unknown11d = 0;
	profile->unknown11e = 0;
	profile->flags_0 = true;
	profile->unknown102 = 3;
	profile->unknownfc = 0;
	switch (type)
	{
	case 0:
		profile->unknown100 = 0;
		profile->unknown101 = 0;
		break;
	case 1:
		profile->unknown100 = 0;
		profile->unknown101 = 0;
		profile->unknownfc_0 = true;
		break;
	}
}

/* the saved game files (save_file_group.cpp, not decompiled yet) */
bool function_2161d0(long file_index, void *buffer, long size);
bool function_216240(long file_index, void *buffer, long size, wchar_t *name);

struct s_player_appearance;
bool function_153750(s_player_appearance *appearance);

/* what the sign-in screens call a player profile */
struct s_player_profile_settings;

/* reads a player profile from its saved game file, then repairs it */
// @retail 0x1a0850
bool function_1a0850(long file_index, s_player_profile *profile)
{
	s_player_profile file_profile;
	bool result = function_2161d0(file_index, &file_profile, sizeof(file_profile));

	if (result)
		*profile = file_profile;
	profile->name[31] = 0;
	if (profile->unknown11f[0x151 - 0x11f] >= 3)
		profile->unknown11f[0x151 - 0x11f] = 0;
	if (!function_153750((s_player_appearance *)&profile->unknown118))
		memset(&profile->unknown118, 0, 0x10);
	return result;
}

/* a player profile: the guest's (no file), or the one saved in the file */
// @retail 0x1a0540
bool function_1a0540(s_player_profile_settings *settings, long file_index)
{
	s_player_profile *profile = (s_player_profile *)settings;

	if (file_index == NONE)
	{
		function_1a07b0(profile, 0);
		return true;
	}
	return function_1a0850(file_index, profile);
}

/* reads the name and profile of a saved player profile */
// @retail 0x1a0660
bool function_1a0660(long file_index, s_player_profile *profile)
{
	if (file_index == NONE)
		return false;
	return function_216240(file_index, profile, sizeof(s_player_profile), profile->name);
}
