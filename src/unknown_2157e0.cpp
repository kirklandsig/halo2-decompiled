// @flags /O2 /Gr
#include "unknown_11c920.h"
#include "loop_allocator.h"

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

struct s_55c164
{
	void *field0;
	byte unknown04[0x40];
};

void *g_51ea14;
byte g_55c14e;
byte g_55c14f;
long g_55c160;
s_55c164 g_55c164[16];

void function_215880(void *ref);
bool function_216800(void *a, long b);

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
		long count = g_55c160;
		for (i = 0; i < count; i++)
		{
			s_55c164 *entry = &g_55c164[i];
			long file_index = (long)entry->field0;
			function_216800(entry->unknown04, file_index);
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
bool function_216800(void *location, long file_index)
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
		long count = g_55c160;
		for (long i = 0; i < count; i++)
		{
			long cached_index = (long)g_55c164[i].field0;
			if (cached_index != NONE && cached_index == file_index)
			{
				*(s_profile_location_bytes *)location = *(s_profile_location_bytes *)g_55c164[i].unknown04;
				result = true;
				break;
			}
		}
	}
	return result;
}
