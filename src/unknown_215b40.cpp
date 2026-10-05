#include "unknown_11c920.h"
#include "unknown_19d220.h"
#include <string.h>

// @flags /O2 /Ob1 /Gr

class c_location_record
{
public:
	c_location_record();
	char path[0x14];
	word name[0x16];
};

struct s_location_record_table
{
	long count;
	c_location_record entries[0x1000];
};

extern void *g_51ea14;
bool function_216800(void *location, long file_index);

// @retail 0x215b40
c_location_record::c_location_record()
{
	path[0] = 0;
	name[0] = 0;
}

// @retail 0x216060
bool function_216060(long file_index, word const *name)
{
	bool result = false;
	struct
	{
		char path[0x14];
		word name[0x16];
	} location;
	location.path[0] = 0;
	location.name[0] = 0;
	if (function_216800(&location, file_index) && name && name[0])
	{
		if (wcsncmp(location.name, name, 0x80))
			result = true;
	}
	return result;
}

// @retail 0x216990
bool function_216990(long unit, c_location_record const *record, long *index)
{
	bool result = false;
	if (g_51ea14)
	{
		s_location_record_table *table = (s_location_record_table *)((byte *)g_51ea14 + 0xbef8) + unit;
		if (table->count != 0x1000)
		{
			long slot = table->count++;
			table->entries[slot] = *record;
			*index = table->count - 1;
			result = true;
		}
	}
	return result;
}

// @retail 0x2169e0
void function_2169e0(long file_index)
{
	if (g_51ea14)
	{
		long unit = (file_index >> 4) & 0xf;
		long index = (file_index >> 8) & 0x1fff;
		s_location_record_table *table = (s_location_record_table *)((byte *)g_51ea14 + 0xbef8) + unit;
		long count = table->count;
		long bounded_index = index < 0 ? 0 : index > count - 1 ? count - 1 : index;
		if (bounded_index == index)
		{
			if (index < count - 1)
			{
				c_location_record *record = &table->entries[index];
				memmove(record, record + 1, (count - index - 1) * sizeof(c_location_record));
			}
			table->count--;
		}
	}
}

// @retail 0x217300
long function_217300(long file_index, long removed_index)
{
	if (!((bool)(((dword)file_index >> 21) & 1)))
	{
		void *storage = g_51ea14;
		if (storage)
		{
			long unit = (file_index >> 4) & 0xf;
			long removed_unit = (removed_index >> 4) & 0xf;
			if (removed_unit == unit)
			{
				long index = (file_index >> 8) & 0x1fff;
				long removed = (removed_index >> 8) & 0x1fff;
				if (index > removed)
				{
					long generation = *(long *)((byte *)storage + 0x4befc);
					file_index = (file_index & 0xf) | ((unit & 0xf) << 4) |
						(((index - 1) & 0x1fff) << 8) | ((generation & 0x1ff) << 22);
				}
			}
		}
	}
	return file_index;
}

struct s_cached_location_entry
{
	long type;
	byte data[0x1e0];
};

struct s_cached_location_table
{
	long count;
	s_cached_location_entry entries[1];
};

PRIVATE __forceinline word *copy_location_name(word *destination, word const *source)
{
	wcsncpy(destination, source, 0x7f);
	destination[0x7f] = 0;
	return destination;
}

// @retail 0x215b50
word *function_215b50(long file_index, word *name)
{
	word *result = name;
	s_cached_location_table *files = (s_cached_location_table *)g_51ea14;
	name[0] = 0;
	if (files && (bool)(((dword)file_index >> 21) & 1))
	{
		long type = file_index & 0xf;
		long index = (file_index >> 8) & 0x1fff;
		long bounded_index = index < 0 ? 0 : index > files->count - 1 ? files->count - 1 : index;
		if (bounded_index == index)
		{
			s_cached_location_entry *entry = &files->entries[index];
			if (type == 0)
			{
				return copy_location_name(result, (word *)(entry->data + 8));
			}
			else
			{
				long bounded_type = type < 1 ? 1 : type > 9 ? 9 : type;
				if (bounded_type == type)
				{
					return copy_location_name(result, (word *)(entry->data + 4));
				}
			}
		}
	}
	else
	{
		struct
		{
			char path[0x14];
			word name[0x16];
		} location;
		location.path[0] = 0;
		location.name[0] = 0;
		if (function_216800(&location, file_index))
		{
			return copy_location_name(result, location.name);
		}
	}
	return result;
}

struct s_player_profile;
void function_1a0750(s_player_profile *profile);
const word *function_217a20(long language);
long g_55c280;

// @retail 0x216760
void function_216760(void)
{
	if (g_51ea14 && ((s_cached_location_table *)g_51ea14)->count != 0x65)
	{
		s_cached_location_entry entry;
		word name[0x100];
		entry.type = 0;
		name[0] = 0;
		word const *default_name = function_217a20(g_55c280);
		word const *const *name_reference = &default_name;
		wcsncpy(name, *name_reference, 0xff);
		name[0xff] = 0;
		function_1a0750((s_player_profile *)entry.data);
		wcsncpy((word *)(entry.data + 8), name, 0x1f);
		((word *)(entry.data + 8))[0x1f] = 0;
		s_cached_location_table *files = (s_cached_location_table *)g_51ea14;
		long index = files->count++;
		files->entries[index] = entry;
	}
}

long saved_game_file_type_from_variant(s_game_variant *variant);

// @retail 0x217080
void __stdcall function_217080(s_game_variant const *variant)
{
	s_game_variant const *const *variant_reference = &variant;
	if (g_51ea14 && ((s_cached_location_table *)g_51ea14)->count != 0x65)
	{
		union
		{
			s_cached_location_entry entry;
			unsigned __int64 words[0x1e8 / 8];
		} value;
		*(s_game_variant *)value.entry.data = **variant_reference;
		value.entry.type = saved_game_file_type_from_variant((s_game_variant *)value.entry.data);
		s_cached_location_table *files = (s_cached_location_table *)g_51ea14;
		s_cached_location_entry *destination = &files->entries[files->count++];
		*destination = value.entry;
	}
}

typedef bool (__stdcall *t_location_compare)(long, long, void const *);
void sort_4byte(long *elements, unsigned long count, void *unused, t_location_compare compare, void const *context);
bool function_1249f0(long memory_unit, char *drive_letter);

// @retail 0x2163d0
bool __stdcall function_2163d0(long first, long second, void const *context)
{
	bool result;
	dword first_cached = (dword)first >> 21;
	dword second_cached = (dword)second >> 21;
	if (!((first_cached ^ second_cached) & 1))
		result = first > second;
	else
		result = (bool)((byte)first_cached & 1);
	return result;
}

// @retail 0x215900
void __stdcall function_215900(long controller, long type, word *capacity, long *indices, long include_cached)
{
	(void)&controller;
	(void)&type;
	(void)&capacity;
	(void)&indices;
	(void)&include_cached;
	long count = 0;
	if (g_51ea14)
	{
		long units = controller == NONE || controller == 0xff ? 1 : 3;
		long unit = 0;
		for (long pass = 0; pass < units && count < *capacity; pass++)
		{
			char drive_letter;
			if (function_1249f0(unit, &drive_letter))
			{
				s_location_record_table *table = (s_location_record_table *)((byte *)g_51ea14 + 0xbef8) + unit;
				long index = 0;
				do
				{
					if (index >= table->count)
						break;
					struct
					{
						char path[0x14];
						word name[0x12];
						long type;
						byte unknown3c[4];
					} entry;
					memcpy(&entry, &table->entries[index], sizeof(entry));
					long old_index = index++;
					if (entry.type == type)
					{
						long generation = *(long *)((byte *)g_51ea14 + 0x4befc);
						indices[count++] = (type & 0xf) | ((unit & 0xf) << 4) |
							((old_index & 0x1fff) << 8) | ((generation & 0x1ff) << 22);
					}
				} while (count < *capacity);
			}
			if (controller == 0xff)
				unit++;
			else
			{
				switch (controller)
				{
				case NONE: unit++; break;
				case 0: unit = pass + 1; break;
				case 1: unit = pass + 3; break;
				case 2: unit = pass + 5; break;
				case 3: unit = pass + 7; break;
				default: __assume(0); break;
				}
			}
		}
		if ((byte)include_cached)
		{
			s_cached_location_table *files = (s_cached_location_table *)g_51ea14;
			long cached_count = files->count;
			for (long index = 0; count < *capacity && index < cached_count; index++)
			{
				if (files->entries[index].type == type)
				{
					long generation = *(long *)((byte *)files + 0x4befc);
					indices[count++] = (type & 0xf) | ((index & 0x1fff) << 8) |
						(1 << 21) | ((generation & 0x1ff) << 22);
				}
			}
			sort_4byte(indices, count, &cached_count, function_2163d0, NULL);
		}
	}
	*capacity = (word)count;
}

bool function_2161d0(long file_index, void *buffer, long size);
bool function_19d650(s_game_variant *variant);

// @retail 0x212bc0
bool function_212bc0(long file_index, s_game_variant *variant)
{
	bool result = false;
	if (file_index != NONE && function_2161d0(file_index, variant, sizeof(*variant)))
	{
		union
		{
			s_game_variant value;
			unsigned __int64 words[0x130 / 8];
		} copy;
		copy.value = *variant;
		result = function_19d650(&copy.value);
	}
	return result;
}

// @retail 0x212c20
bool __stdcall function_212c20(word const *name, s_game_variant *variant, long *file_index)
{
	(void)&name;
	(void)&variant;
	(void)&file_index;
	bool result = false;
	if (file_index)
		*file_index = NONE;
	if (name[0])
	{
		for (long type = 1; type <= 9 && !result; type++)
		{
			long indices[0x1000];
			long capacity = 0x1000;
			word candidate[0x100];
			function_215900(0xff, type, (word *)&capacity, indices, true);
			for (long index = 0; index < (word)capacity; index++)
			{
				if (!_wcsicmp(function_215b50(indices[index], candidate), name))
				{
					result = true;
					if (variant)
						result = function_212bc0(indices[index], variant);
					if (result && file_index)
						*file_index = indices[index];
					break;
				}
			}
		}
	}
	else if (variant)
	{
		function_19d220(variant, 0);
		result = true;
	}
	return result;
}
