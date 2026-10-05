#include "unknown_11c920.h"
// @flags /O2 /Gr
#include <string.h>
#include "async.h"
#include "unknown_122870.h"

struct s_cache_file
{
	void *handle;
	byte unknown04[0x800];
};

extern s_cache_file g_557c90[3];

// @retail 0x214c70
long cache_file_slot_from_type(long type)
{
	long result = NONE;

	switch (type)
	{
	case 0:
		result = 2;
		break;
	case 1:
		result = 0;
		break;
	case 2:
		result = 1;
		break;
	}
	return result;
}

// @retail 0x214c30
s_cache_file *cache_file_from_type(long type)
{
	s_cache_file *result = NULL;
	long slot = cache_file_slot_from_type(type);

	if (slot != NONE)
	{
		result = &g_557c90[slot];
	}
	return result;
}

struct s_cache_file_limits
{
	long first;
	long last;
	long size;
};

const s_cache_file_limits g_448f98[5] =
{
	{2, 2, 0x5000000},
	{0, 0, 0xb400000},
	{1, 1, 0x20800000},
	{3, 3, 0x5000000},
	{4, 5, 0x11800000}
};

static inline s_cache_file_limits const *cache_file_limits_find(long index)
{
	s_cache_file_limits const *result = NULL;
	dword i;

	for (i = 0; i < 5; i++)
	{
		s_cache_file_limits const *limits = &g_448f98[i];

		if (index >= limits->first && index <= limits->last)
		{
			result = limits;
			break;
		}
	}
	return result;
}

// @retail 0x214c90
long cache_file_size_limit(long index)
{
	long result = 0;
	s_cache_file_limits const *limits = cache_file_limits_find(index);

	if (limits)
	{
		result = limits->size;
	}
	return result;
}

char const *g_46880c = ".map";

char *function_11c9c0(char *buffer, long maximum_count, char const *format, ...);

// @retail 0x214cc0
void cache_file_path_from_slot(long index, char *path, long path_size)
{
	s_cache_file_limits const *limits = cache_file_limits_find(index);

	strncpy(path, "", path_size);
	path[path_size - 1] = 0;
	if (limits)
	{
		char const *drive = NULL;

		if (index >= 0 && index <= 1)
		{
			drive = "z:\\";
		}
		else if (index >= 2 && index <= 5)
		{
			drive = "n:\\";
		}
		function_11c9c0(path, path_size, "%scache%03d%s", drive, index, g_46880c);
	}
}

bool cache_header_verify(s_cache_header const *header);

__forceinline s_cache_header *function_xafcc06(s_cache_file *file)
{
	return (s_cache_header *)file->unknown04;
}

// @retail 0x2149c0
bool cache_file_header_read(long index)
{
	s_cache_file *file = &g_557c90[index];
	bool volatile done = false;
	dword volatile bytes_read = 0;
	bool volatile result = false;
	char path[256];

	cache_file_path_from_slot(index, path, sizeof(path));
	s_cache_header *header = function_xafcc06(file);
	function_1a0f10(*(s_file_handle *)&file->handle, header, 0x800, 0, 0, 6, (dword *)&bytes_read, &done);
	if (!done)
	{
		while (!done)
		{
			SwitchToThread();
		}
	}
	if (bytes_read == 0x800 && cache_header_verify(header))
	{
		return true;
	}
	return result;
}

__forceinline FILETIME const *cache_file_time(s_cache_file const *file)
{
	return (FILETIME const *)((byte const *)file + 0x150);
}

__forceinline FILETIME cache_file_current_time(void)
{
	FILETIME result;
	GetSystemTimeAsFileTime(&result);
	return result;
}

// @retail 0x214ac0
bool cache_file_slot_precedes(long first_index, long second_index)
{
	bool result = false;
	FILETIME now = cache_file_current_time();

	if (first_index == NONE)
	{
		result = true;
		goto done;
	}
	if (second_index == NONE)
	{
		result = false;
		goto done;
	}
	s_cache_file const *first = &g_557c90[first_index];
	s_cache_file const *second = &g_557c90[second_index];

	if (CompareFileTime(cache_file_time(first), &now) > 0)
	{
		goto done;
	}
	if (CompareFileTime(cache_file_time(second), &now) <= 0 &&
		(dword)cache_file_size_limit(first_index) >= (dword)cache_file_size_limit(second_index) &&
		CompareFileTime(cache_file_time(second), cache_file_time(first)) >= 0)
	{
		goto done;
	}
	result = true;
done:
	return result;
}

extern long g_55aca8;

// @retail 0x214b80
long cache_file_choose_slot(short type, dword size)
{
	long result = NONE;
	long first;
	long last;

	switch (type)
	{
	case 3:
		first = 0;
		last = 0;
		break;
	case 4:
		first = 1;
		last = 1;
		break;
	case 1:
		first = 3;
		last = 3;
		break;
	case 0:
		first = 4;
		last = 5;
		break;
	case 2:
		first = 2;
		last = 2;
		break;
	default:
		__assume(0);
	}
	for (long index = first; index <= last; index++)
	{
		if (g_55aca8 != index && (dword)cache_file_size_limit(index) >= size && cache_file_slot_precedes(result, index))
		{
			result = index;
		}
	}
	return result;
}
