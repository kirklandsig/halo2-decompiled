// @flags /O2 /Gr
/* UNKNOWN_213D20.CPP (part): which map file a map name stands for and
   where it is: open as a cache file, copied to the utility drive, or being
   copied (0x213d20..0x214e40). Decompiled by lane T for the map loading code
   of 0x163680..0x163b60. */

#include "unknown_11c920.h"
#include <xtl.h>
#include <string.h>
#include "map_file_path.h"

char *csprintf_256(char *buffer, const char *format, ...);
int function_11c920(char const *s1, char const *s2);

/* the open cache files (0x804 bytes each, unknown_213760.cpp); the name of
   each file's map is at +0x24 */
struct s_cache_file
{
	HANDLE handle;
	byte unknown04[0x800];
};

extern s_cache_file g_557c90[3];

#define MAXIMUM_CACHE_FILES 6
#define cache_file_map_name(index) ((char const *)&g_557c90[index] + 0x24)

/* the maps copied to the utility drive (0x108 bytes each) */
struct s_cache_copy
{
	char path[256];
	long last_used;
	long state;
};

#define MAXIMUM_CACHE_COPIES 8

s_cache_copy g_55acac[MAXIMUM_CACHE_COPIES];

/* the map being copied and the two maps waiting to be */
struct s_cache_copy_request
{
	char map_name[256];
	long priority;
};

extern long g_55bd04;
extern long g_55bd08;
bool g_55bd0c;
char g_55bd21[0x103];
s_cache_copy_request g_55be24[2];

enum
{
	_map_location_none = 0,
	_map_location_queued,
	_map_location_copying,
	_map_location_open,
	_map_location_copied
};

__declspec(noinline) void cache_copy_hurry(void);

// @retail 0x214d40
void map_name_from_path(char const *path, char *name)
{
	char *extension;
	char *c;
	dword count;

	strncpy(name, path, 0x100);
	name[0xff] = 0;
	extension = name + strspn(name, ".");
	if (*extension)
	{
		extension = strpbrk(extension, ".");
		if (extension)
		{
			*extension = 0;
		}
	}
	for (count = 0x100, c = name; *c && count-- > 0; c++)
	{
		char character = *c;

		if (character >= 'A' && character <= 'Z')
		{
			character += 'a' - 'A';
		}
		*c = character;
	}
}

// @retail 0x214db0
void s_map_name::map_file_path_get(char *path) const
{
	char name[256];

	if (text[0] == 't' && text[1] == ':' && text[2] == '\\' ||
		text[0] == 'w' && text[1] == ':' && text[2] == '\\')
	{
		map_name_from_path(text, name);
		csprintf_256(path, "%s.map", name);
	}
	else
	{
		char const *file_name = strrchr(text, '\\');

		if (file_name)
		{
			file_name++;
		}
		else
		{
			file_name = text;
		}
		map_name_from_path(file_name, name);
		csprintf_256(path, "d:\\maps\\%s.map", name);
	}
}

// @retail 0x214a60
long cache_file_find(char const *map_name)
{
	char path[256];
	long result = NONE;
	long index;

	path[0] = 0;
	((s_map_name const *)map_name)->map_file_path_get(path);
	for (index = 0; index < MAXIMUM_CACHE_FILES; index++)
	{
		if (!function_11c920(path, cache_file_map_name(index)))
		{
			result = index;
			break;
		}
	}
	return result;
}

// @retail 0x214060
s_cache_copy *cache_copy_find(char const *map_name)
{
	s_cache_copy *volatile result;
	char path[256];
	dword index;

	path[0] = 0;
	result = NULL;
	((s_map_name const *)map_name)->map_file_path_get(path);
	for (index = 0; index < MAXIMUM_CACHE_COPIES; index++)
	{
		s_cache_copy *copy = &g_55acac[index];

		if (!strcmp(path, copy->path))
		{
			return copy;
		}
	}
	return result;
}

// @retail 0x2141b0
void cache_copy_forget(char const *map_name)
{
	s_cache_copy *copy = cache_copy_find(map_name);

	if (copy)
	{
		memset(copy, 0, sizeof(*copy));
	}
}

// @retail 0x214100
void __stdcall cache_copy_record_use(char const *map_name)
{
	s_map_file_path path;
	s_cache_copy *copy;

	path.text[0] = 0;
	path.get(map_name);
	copy = cache_copy_find(path.text);
	if (!copy)
	{
		dword index;

		for (index = 0; index < MAXIMUM_CACHE_COPIES; index++)
		{
			s_cache_copy *candidate = &g_55acac[index];

			if (!candidate->path[0])
			{
				copy = candidate;
				break;
			}
			if (!copy || candidate->last_used < copy->last_used)
			{
				copy = candidate;
			}
		}
		if (copy)
		{
			memcpy(copy->path, path.text, sizeof(path.text));
			copy->state = 0;
		}
	}
	if (copy)
	{
		copy->last_used = GetTickCount();
		copy->state++;
		cache_copy_find(map_name);
	}
}

// @retail 0x2141d0
bool cache_copy_complete(char const *map_name)
{
	s_cache_copy *copy = cache_copy_find(map_name);
	long state = 0;

	if (copy)
	{
		state = copy->state;
	}
	return state >= 3 ? true : false;
}

long g_55bd18;
long g_55bd1c;
bool g_55bd20;

long function_12d400(long type, long size, long user_data, long update, long release);
void function_12d520(long address);
bool function_2141f0(void);

// @retail 0x214030
void cache_copy_buffer_release(void)
{
	if (g_55bd18)
	{
		function_12d520(g_55bd18);
		g_55bd18 = 0;
		g_55bd1c = 0;
	}
}

// @retail 0x213fa0
void __stdcall cache_copy_buffer_reclaim(long address, long user_data)
{
	while (g_55bd18)
	{
		g_55bd20 = true;
		if (g_55bd0c)
		{
			function_2141f0();
		}
	}
}

// @retail 0x213fe0
void cache_copy_buffer_resize(long size)
{
	if (g_55bd1c != size)
	{
		cache_copy_buffer_release();
		g_55bd18 = function_12d400(0, size, 0, 0, (long)cache_copy_buffer_reclaim);
		if (g_55bd18)
		{
			g_55bd1c = size;
		}
	}
}

// @retail 0x213e30
long cache_copy_current_priority(char const *map_name)
{
	long result = NONE;
	s_map_file_path path;
	s_map_file_path current_path;

	path.text[0] = 0;
	path.get(map_name);
	current_path.text[0] = 0;
	current_path.get(g_55bd21);
	if (!function_11c920(path.text, current_path.text))
	{
		result = g_55be24[0].priority;
	}
	return result;
}

// @retail 0x213e90
long cache_copy_queued_priority(char const *map_name)
{
	long result = NONE;
	s_map_file_path path;
	s_map_file_path queued_path;

	path.text[0] = 0;
	path.get(map_name);
	queued_path.text[0] = 0;
	queued_path.get(g_55be24[1].map_name);
	if (!function_11c920(path.text, queued_path.text))
	{
		result = g_55be24[1].priority;
	}
	return result;
}

/* retail calls this out of line from its one caller, 0x163680 */
__declspec(noinline) long cache_copy_priority(char const *map_name);

// @retail 0x213ef0
long cache_copy_priority(char const *map_name)
{
	long result = cache_copy_current_priority(map_name);

	if (result == NONE)
	{
		result = cache_copy_queued_priority(map_name);
	}
	return result;
}

// @retail 0x213f10
long map_location_get(char const *map_name)
{
	s_map_file_path path;
	s_cache_copy *copy;

	path.text[0] = 0;
	path.get(map_name);
	if (cache_file_find(path.text) != NONE)
	{
		return _map_location_open;
	}
	copy = cache_copy_find(map_name);
	if (copy && copy->state >= 3)
	{
		return _map_location_copied;
	}
	if (cache_copy_current_priority(map_name) != NONE)
	{
		return _map_location_copying;
	}
	return cache_copy_queued_priority(map_name) != NONE;
}

// @retail 0x214e40
bool map_names_equal(char const *map_name, char const *other_map_name)
{
	bool result = false;
	s_map_file_path path;
	s_map_file_path other_path;

	path.text[0] = 0;
	other_path.text[0] = 0;
	path.get(map_name);
	other_path.get(other_map_name);
	if (!strcmp(path.text, other_path.text))
	{
		result = true;
	}
	return result;
}

// @retail 0x214990
void cache_copy_hurry(void)
{
	if (g_55bd04 && g_55bd04 < 0x11)
	{
		g_55bd08 = 2;
		g_55bd04 = 0x11;
	}
}

// @retail 0x213d20
bool map_copy_request(char const *map_name, long priority)
{
	bool result = false;
	long location = map_location_get(map_name);
	char path[256];

	path[0] = 0;
	((s_map_name const *)map_name)->map_file_path_get(path);
	if (location == _map_location_none)
	{
		if (!g_55be24[1].map_name[0] || priority >= g_55be24[1].priority)
		{
			memset(&g_55be24[1], 0, sizeof(g_55be24[1]));
			strncpy(g_55be24[1].map_name, path, 0x100);
			g_55be24[1].map_name[0xff] = 0;
			g_55be24[1].priority = priority;
			result = true;
		}
	}
	else if (location == _map_location_copying)
	{
		if (map_names_equal(path, g_55be24[0].map_name))
		{
			g_55be24[0].priority = priority;
		}
		result = true;
	}
	else if (location == _map_location_queued)
	{
		if (map_names_equal(path, g_55be24[1].map_name))
		{
			g_55be24[1].priority = priority;
		}
		result = true;
	}
	else if (location == _map_location_open)
	{
		result = true;
	}
	if (g_55be24[0].map_name[0] && g_55be24[1].map_name[0] && g_55be24[1].priority >= g_55be24[0].priority)
	{
		cache_copy_hurry();
	}
	return result;
}
