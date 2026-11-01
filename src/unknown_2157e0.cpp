// @flags /O2 /Gr
#include "unknown_11c920.h"
#include "loop_allocator.h"
#include "saved_game_storage.h"
#include <xtl.h>
#include <string.h>

// The allocator methods are shared with the memory source at 0x476fbc.
class c_physical_memory_source : public c_memory_source
{
public:
	virtual void *allocate(long size);
	virtual void release(void *block) {}

	long unknown04;
};

struct s_profile_location_bytes
{
	byte data[0x40];
};

struct s_profile_location_table
{
	long count;
	s_profile_location_bytes entries[0x1000];
};


void *g_51ea14;
byte g_55c14e;
byte g_55c14f;
s_55c160 g_55c160_table;
s_saved_game_file_lock g_saved_game_file_lock;
#define g_55c274 g_saved_game_file_lock.free
#define g_55c278 g_saved_game_file_lock.file_index
#define g_55c27c g_saved_game_file_lock.thread

void function_215880(void *ref);
bool function_216800(long b, void *a);

bool function_1249f0(long memory_unit, char *drive_letter);
void function_2172a0(long handle);

/* a file handle: generation (9 bits), a flag (bit 13 of the 22), the
   index (13 bits), then the type in the low byte's low nibble */
static inline long file_handle_build_2170e0(long generation, short index, bool flag, long type)
{
	long handle = ((generation & 0x1ff) << 14) | (index & 0x1fff);

	if (flag)
		handle |= 0x2000;
	return (handle << 8) | (type & 15);
}

// @retail 0x2170e0
long function_2170e0(long file_index)
{
	(void)&file_index;
	long result = NONE;
	if (g_55c14f)
	{
		s_55c164 *cached = NULL;
		for (long i = 0; i < g_55c160_table.count; i++)
		{
			long index = (long)g_55c160_table.entries[i].field0;
			if (index != NONE && index == file_index)
			{
				cached = &g_55c160_table.entries[i];
				break;
			}
		}
		long type = file_index & 15;
		if ((bool)(((dword)file_index >> 21) & 1))
		{
			result = file_handle_build_2170e0(saved_game_storage_get_cache()->generation, (short)(file_index >> 8), true, type);
		}
		else
		{
			char drive;
			if (g_51ea14 && function_1249f0(0, &drive))
			{
				s_saved_game_location_iterator iterator;
				iterator.table = &saved_game_storage_get_cache()->locations;
				iterator.index = 0;
				s_saved_game_file_location location;
				location.name[0] = 0;
				location.display_name[0] = 0;
				while (saved_game_location_iterator_get(&iterator, &location))
				{
					long current = iterator.index++;

					if (location.type != type)
						continue;
					if (strncmp((char const *)cached->unknown04, location.name, 20))
						continue;
					result = file_handle_build_2170e0(saved_game_storage_get_cache()->generation, (short)current, false, type);
					break;
				}
			}
		}
		function_2172a0(file_index);
		if (result != NONE)
		{
			g_55c160_table.count++;
			g_55c160_table.entries[g_55c160_table.count - 1].field0 = (void *)result;
		}
	}
	else
	{
		long generation = saved_game_storage_get_cache()->generation;
		if (generation == ((file_index >> 22) & 0x1ff))
		{
			if ((bool)(((dword)file_index >> 21) & 1))
				result = file_handle_build_2170e0(generation, (short)(file_index >> 8), true, file_index);
			else
				result = file_index;
		}
	}
	return result;
}

// @retail 0x2157e0
void __stdcall function_2157e0(long stage)
{
	switch (stage)
	{
	case 1:
		{
			c_physical_memory_source ref;
			ref.unknown04 = 2;
			function_215880(&ref);
		}
		break;
	}
	g_55c14e = 1;
}

// @retail 0x215810
void function_215810(void)
{
	void *table = g_51ea14;

	if (table)
	{
		long i;
		long count = g_55c160_table.count;
		for (i = 0; i < count; i++)
		{
			s_55c164 *entry = &g_55c160_table.entries[i];
			long file_index = (long)entry->field0;
			function_216800(file_index, entry->unknown04);
		}
		g_55c14f = 1;
	}

	c_physical_memory_source ref;
	ref.unknown04 = 2;
	c_physical_memory_source *reference = &ref;
	if (table)
	{
		reference->release(table);
		g_51ea14 = 0;
	}
	g_55c14e = 0;
}

// @retail 0x216800
bool function_216800(long file_index, void *location)
{
	bool result = false;
	void *files = g_51ea14;

	if (files)
	{
		if (!((bool)(((dword)file_index >> 21) & 1)))
		{
			long unit = (file_index >> 4) & 0xf;
			long index = (file_index >> 8) & 0x1fff;
			s_profile_location_table *table = (s_profile_location_table *)((byte *)files + 0xbef8) + unit;
			long count = table->count;
			long bounded_index = index < 0 ? 0 : index > count - 1 ? count - 1 : index;
			if (bounded_index == index)
			{
				*(s_profile_location_bytes *)location = table->entries[index];
				result = true;
			}
		}
	}
	else if (g_55c14f)
	{
		long count = g_55c160_table.count;
		for (long i = 0; i < count; i++)
		{
			long cached_index = (long)g_55c160_table.entries[i].field0;
			if (cached_index != NONE && cached_index == file_index)
			{
				*(s_profile_location_bytes *)location = *(s_profile_location_bytes *)g_55c160_table.entries[i].unknown04;
				result = true;
				break;
			}
		}
	}
	return result;
}

struct s_saved_game_file_location;

// @retail 0x2168b0
bool function_2168b0(s_saved_game_file_location *location, long file_index)
{
	long const *index_reference = &file_index;
	bool result = false;
	dword thread = GetCurrentThreadId();
	if (g_55c274 || (g_55c278 == *index_reference && g_55c27c != thread))
	{
		void *files = g_51ea14;
		if (files)
		{
			long unit = (*index_reference >> 4) & 0xf;
			long index = (*index_reference >> 8) & 0x1fff;
			s_profile_location_table *table = (s_profile_location_table *)((byte *)files + 0xbef8) + unit;
			long count = table->count;
			long bounded_index = index < 0 ? 0 : index > count - 1 ? count - 1 : index;
			if (bounded_index == index)
			{
				table->entries[index] = *(s_profile_location_bytes const *)location;
				result = true;
			}
		}
		else if (g_55c14f)
		{
			long count = g_55c160_table.count;
			for (long i = 0; i < count; i++)
			{
				long cached_index = (long)g_55c160_table.entries[i].field0;
				if (cached_index != NONE && cached_index == *index_reference)
				{
					*(s_profile_location_bytes *)g_55c160_table.entries[i].unknown04 = *(s_profile_location_bytes const *)location;
					break;
				}
			}
		}
	}
	return result;
}


bool g_55c14d;
