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
	*(volatile byte *)&profile->unknown118 = 10;
	*(volatile byte *)&profile->unknown119 = 0;
	*(volatile byte *)&profile->unknown11a = 0;
	*(volatile byte *)&profile->unknown11b = 10;
	*(volatile byte *)&profile->unknown11d = 0;
	*(volatile byte *)&profile->unknown11e = 0;
	dword *local_0 = (dword *)((byte *)profile + 4);
	*local_0 |= 1;
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
	*(volatile byte *)&profile->unknown118 = 10;
	*(volatile byte *)&profile->unknown119 = 0;
	*(volatile byte *)&profile->unknown11a = 0;
	*(volatile byte *)&profile->unknown11b = 10;
	*(volatile byte *)&profile->unknown11d = 0;
	*(volatile byte *)&profile->unknown11e = 0;
	((volatile s_player_profile *)profile)->flags_0 = true;
	profile->unknown102 = 3;
	profile->unknownfc = 0;
	switch (type)
	{
	case 0:
		*(volatile byte *)&profile->unknown100 = 0;
		*(volatile byte *)&profile->unknown101 = 0;
		break;
	case 1:
		*(volatile byte *)&profile->unknown100 = 0;
		*(volatile byte *)&profile->unknown101 = 0;
		profile->unknownfc |= 1;
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

void __stdcall function_215900(long controller, long type, word *capacity, long *indices, long include_cached);
word *function_215b50(long file_index, word *name);

// @retail 0x1a0560
bool function_1a0560(wchar_t const *name, s_player_profile_settings *settings, long *file_index)
{
	long count = 4096;
	wchar_t local_0[128];
	long indices[4096];
	bool result = false;
	if (file_index)
		*file_index = NONE;
	if (*name)
	{
		function_215900(255, 0, (word *)&count, indices, 1);
		for (long index = 0; index < (word)count; index++)
		{
			wchar_t const *local_1 = (wchar_t *)function_215b50(indices[index], (word *)local_0);
			if (!_wcsicmp(local_1, name))
			{
				result = true;
				if (settings)
					result = function_1a0540(settings, indices[index]);
				if (result && file_index)
					*file_index = indices[index];
				break;
			}
		}
	}
	else if (settings)
	{
		function_1a07b0((s_player_profile *)settings, 0);
		result = true;
	}
	return result;
}

long function_215c00(long arg_0, wchar_t const *arg_1);
void __stdcall function_215e60(long arg_0);

PRIVATE inline long function_1a03a1(long arg_0)
{
	g_4e7408->seed = g_4e7408->seed * 1664525 + 1013904223;
	return (long)(g_4e7408->seed >> 16) % arg_0;
}

/* Retail uses two stack arguments and ret 8. With the standard marker,
   this body matches all 413 bytes; without it the unused first argument
   is removed and three instructions differ. No retail data or code holds
   its address. Callers 0x238537, 0x2385d6, 0x238642 and 0x2386c5 use
   register conventions themselves and push both arguments. Declaring
   __stdcall alone and the earlier parameter-address trial did not retain
   the unused stack argument. */
// @retail 0x1a03a0 standard
long __stdcall function_1a03a0(long arg_0, word *arg_1)
{
	s_player_profile local_0;
	long local_1 = function_215c00(0, (wchar_t const *)arg_1);
	if (local_1 != NONE)
	{
		function_1a07b0(&local_0, 0);
		*(dword *)((byte *)&local_0 + 4) = 0;
		local_0.unknown118 = (byte)function_1a03a1(16);
		local_0.unknown119 = (byte)function_1a03a1(16);
		local_0.unknown11a = (byte)function_1a03a1(16);
		local_0.unknown11b = (byte)function_1a03a1(16);
		local_0.unknown11d = (byte)function_1a03a1(64);
		local_0.unknown11e = (byte)function_1a03a1(32);
		wide_string_copy(local_0.name, (wchar_t const *)arg_1, 31);
		*(long *)((byte *)&local_0 + 0xec) = NONE;
		*(long *)((byte *)&local_0 + 0xf0) = NONE;
		*(long *)((byte *)&local_0 + 0xf4) = NONE;
		*(long *)((byte *)&local_0 + 0xf8) = NONE;
		if (!function_216240(local_1, &local_0, sizeof(local_0), local_0.name))
		{
			function_215e60(local_1);
			return NONE;
		}
	}
	return local_1;
}
