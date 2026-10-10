#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_19c1d0.h"
#include "data_array.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

// @flags /O2 /Gr

struct s_entry_b
{
	byte unknown00[4];
	long key;
	byte unknown08[0xb50 - 8];
};

struct s_entry_c
{
	long key;
	byte unknown04[0xc50 - 4];
	byte flags;
	byte unknownc51[0xc64 - 0xc51];
};

struct s_table_b
{
	long unknown00;
	long unknown04;
	long count;
	s_entry_b *data;
};

struct s_table_a
{
	long count;
	s_entry_a *data;
};

struct s_table_c
{
	long unknown00[4];
	long count;
	s_entry_c *data;
};

static s_table_b *get_table_b()
{
	s_tag_header_globals *globals = g_4e034c;
	s_table_b *table;
	if (globals->b_valid)
		table = globals->b;
	else
		table = 0;
	return table;
}

static s_entry_b *get_entry_b(s_table_b *table, long index)
{
	s_entry_b *entry = 0;
	if (table && table->count > 0)
		entry = table->data + index;
	return entry;
}

static s_table_a *get_table_a()
{
	s_table_a *table = 0;
	if (g_4e0350 && g_4e034c)
	{
		s_tag_header_globals *globals = g_4e034c;
		if (globals->a_valid)
			table = globals->a;
		else
			table = 0;
	}
	return table;
}

static s_entry_a *get_entry_a(s_table_a *table, long index)
{
	s_entry_a *entry = 0;
	if (table && table->count > 0)
		entry = table->data + index;
	return entry;
}

static s_table_c *get_table_c()
{
	s_tag_header_globals *globals = g_4e034c;
	s_table_c *table;
	if (globals->b_valid)
		table = (s_table_c *)globals->b;
	else
		table = 0;
	return table;
}

static s_entry_c *get_entry_c(s_table_c *table, long index)
{
	s_entry_c *entry = 0;
	if (table && table->count > 0)
		entry = table->data + index;
	return entry;
}

// @retail 0x19c1d0
void function_19c1d0()
{
	g_4ee4e8->valid = 0;
	g_4ee4e4->valid = 0;
}

// @retail 0x19c1f0
s_entry_b *function_19c1f0(long key)
{
	s_table_b *table = get_table_b();
	s_entry_b *result = 0;
	if (table)
	{
		long count = table->count;
		for (long i = 0; i < count; i++)
		{
			s_entry_b *entry = get_entry_b(get_table_b(), i);
			if (entry->key == key)
			{
				result = entry;
				break;
			}
		}
	}
	return result;
}

// @retail 0x19c270
s_entry_a *function_19c270(long key0, long key1)
{
	s_table_a *table = get_table_a();
	s_entry_a *result = 0;
	if (table)
	{
		long count = table->count;
		for (long i = 0; i < count; i++)
		{
			s_entry_a *entry = get_entry_a(get_table_a(), i);
			if (entry->key1 == key1 && entry->key0 == key0)
			{
				result = entry;
				break;
			}
		}
	}
	return result;
}

// @retail 0x19c320
s_entry_a *function_19c320(const char *name)
{
	s_table_a *table = get_table_a();
	s_entry_a *result = 0;
	if (table)
	{
		for (long i = 0; i < table->count; i++)
		{
			s_entry_a *entry = get_entry_a(get_table_a(), i);
			const char *a = strrchr(name, 0x5c);
			a = a ? a + 1 : name;
			const char *b = strrchr(entry->name, 0x5c);
			b = b ? b + 1 : entry->name;
			if (!strcmp(b, a))
			{
				result = entry;
				break;
			}
		}
	}
	return result;
}

// @retail 0x19c440
long function_19c440(long key0, long key1)
{
	s_table_a *table = get_table_a();
	long result = NONE;
	if (table)
	{
		long count = table->count;
		for (long i = 0; i < count; i++)
		{
			s_entry_a *entry = get_entry_a(get_table_a(), i);
			if (entry->key0 == key0 && entry->key1 > key1)
			{
				if (result == NONE || entry->key1 < result)
					result = entry->key1;
			}
		}
	}
	return result;
}

// @retail 0x19c4e0
long function_19c4e0(long key0)
{
	s_table_a *table = get_table_a();
	long result = NONE;
	if (table)
	{
		long count = table->count;
		for (long i = 0; i < count; i++)
		{
			s_entry_a *entry = get_entry_a(get_table_a(), i);
			if (entry->key0 == key0 && entry->key1 != NONE && result <= entry->key1)
				result = entry->key1;
		}
	}
	return result;
}

// @retail 0x19c580
long function_19c580()
{
	long result = NONE;
	s_table_c *table = get_table_c();
	if (table)
	{
		long count = table->count;
		for (long i = 0; i < count; i++)
		{
			s_entry_c *entry = get_entry_c(get_table_c(), i);
			long key = entry->key;
			if (key != NONE && (result == NONE || key < result))
				result = key;
		}
	}
	return result;
}

// @retail 0x19c5f0
s_entry_c *function_19c5f0(long key)
{
	s_table_c *table = get_table_c();
	s_entry_c *result = 0;
	if (table)
	{
		long count = table->count;
		for (long i = 0; i < count; i++)
		{
			s_entry_c *entry = get_entry_c(get_table_c(), i);
			if (entry->key == key)
			{
				result = entry;
				break;
			}
		}
	}
	return result;
}

// @retail 0x19c670
s_record_pool *function_19c670()
{
	s_table_b *table = get_table_b();
	s_record_pool *result = 0;
	if (table)
		result = g_4ee4e8;
	return result;
}

// @retail 0x19c6a0
s_record_pool *function_19c6a0()
{
	s_table_b *table = get_table_b();
	s_record_pool *result = 0;
	if (table)
		result = g_4ee4e4;
	return result;
}

// @retail 0x19c7c0
bool __stdcall function_19c7c0(long a, long b, const void *context)
{
	bool result = a > b;
	return result;
}

// @retail 0x19c7d0
long __stdcall function_19c7d0(long a, long b, const void *context)
{
	return a - b;
}

// @retail 0x19c970
const char *function_19c970(long campaign_id, long map_id)
{
	const char *path = 0;
	if (campaign_id == NONE)
	{
		s_entry_c *level = function_19c5f0(map_id);
		if (level)
			path = (const char *)level + 0xb4c;
	}
	else
	{
		s_entry_a *level = function_19c270(campaign_id, map_id);
		if (level)
			path = level->name;
	}
	return path;
}

// @retail 0x19c9a0
int __cdecl function_19c9a0(const void *a, const void *b)
{
	s_entry_c *x = function_19c5f0(*(const long *)a);
	s_entry_c *y = function_19c5f0(*(const long *)b);
	if (*(long *)((byte *)x + 0xc4c) > *(long *)((byte *)y + 0xc4c))
		return 1;
	return *(long *)((byte *)x + 0xc4c) < *(long *)((byte *)y + 0xc4c) ? -1 : 0;
}

/* the sort routines (0x13dcd0); sort_4byte's third parameter
   is never read */
typedef bool (__stdcall *t_sort_4byte_compare_function)(long, long, const void *);
typedef long (__stdcall *t_search_4byte_compare_function)(long, long, const void *);
void sort_4byte(long *elements, unsigned long count, void *unused, t_sort_4byte_compare_function compare, const void *context);
long function_13dd70(long key, const long *base, long count, t_search_4byte_compare_function compare, const long *context);

/* the elements of the data arrays these functions fill (8 bytes) */
struct s_level_datum
{
	short identifier;
	bool flag;
	byte unknown03;
	long value;
};

static inline s_level_datum *level_datum_get(s_record_pool *data, long index)
{
	return (s_level_datum *)(data->data + index * sizeof(s_level_datum));
}

// @retail 0x19c6d0
void function_19c6d0(s_record_pool *data, long key0)
{
	long values[20];
	s_table_a *table = get_table_a();
	long count = 0;

	if (table)
	{
		for (long i = 0; i < table->count; i++)
		{
			s_entry_a *entry = get_entry_a(get_table_a(), i);

			if (entry->key0 == key0 && entry->key1 != NONE)
				values[count++] = entry->key1;
		}
	}

	sort_4byte(values, count, &key0, function_19c7c0, 0);
	record_pool_release_all(data);
	for (long i = 0; i < count; i++)
	{
		long datum_index = record_pool_allocate(data);

		if (datum_index != NONE)
			level_datum_get(data, datum_index & 0xffff)->value = values[i];
	}
}

// @retail 0x19c7e0
void function_19c7e0(s_record_pool *data)
{
	long keys[50];
	long flagged[50];
	s_table_c *table = get_table_c();
	long count = 0;
	long flagged_count = 0;

	if (table)
	{
		for (long i = 0; i < table->count; i++)
		{
			s_entry_c *entry = get_entry_c(get_table_c(), i);

			if (entry->key != NONE)
			{
				keys[count++] = entry->key;
				if (entry->flags & 1)
					flagged[flagged_count++] = entry->key;
			}
		}
	}

	qsort(keys, count, sizeof(long), function_19c9a0);
	record_pool_release_all(data);
	for (long i = 0; i < count; i++)
	{
		long datum_index = record_pool_allocate(data);

		if (datum_index != NONE)
			level_datum_get(data, datum_index & 0xffff)->value = keys[i];
	}

	sort_4byte(flagged, flagged_count, &flagged_count, function_19c7c0, 0);
	long index = NONE;
	while (true)
	{
		s_level_datum *datum;

		index = data_next_absolute_index_inlined(data, index + 1);
		if (index == NONE)
			break;
		datum = (s_level_datum *)(data->data + data->size * index);
		if (!datum)
			break;
		if (function_13dd70(datum->value, flagged, flagged_count, function_19c7d0, 0) != NONE)
			datum->flag = true;
	}
}

/* a path of up to 259 characters */
struct s_level_path
{
	char string[0x104];
};

// @retail 0x19c9e0
char *level_path_print(s_level_path *path, char const *format, ...)
{
	va_list arguments;

	va_start(arguments, format);
	_vsnprintf(path->string, sizeof(path->string) - 1, format, arguments);
	path->string[sizeof(path->string) - 1] = 0;

	return path->string;
}

/* retail inlines function_16b790 here (unknown_16b570.cpp is /Ob1) */
static inline void data_make_valid_inlined(s_record_pool *data)
{
	data->valid = 1;
	record_pool_release_all(data);
}

/* the scenario's type at +0x10 */
struct s_level_scenario_view
{
	byte unknown00[0x10];
	short type;
};

/* a found content package: its directory and display name (online_feedback.cpp) */
struct s_content_item
{
	char directory[0x104];
	word display_name[0x80];
};

bool content_find_first(s_content_item *item, void **find_handle);
bool content_find_next(void *find_handle, s_content_item *item);
bool __stdcall function_19bfd0(s_content_item *item);

extern bool g_54e7f8;

/* reads the map files the disc holds; remembers whether one failed */
// @retail 0x19c120
void function_19c120(void)
{
	g_54e7f8 = false;
	if (((s_level_scenario_view *)g_4e0350)->type == 2)
	{
		void *handle;
		s_content_item item;

		item.directory[0] = 0;
		item.display_name[0] = 0;
		if (content_find_first(&item, &handle) != 0)
		{
			do
			{
				if (!function_19bfd0(&item))
					*(volatile bool *)&g_54e7f8 = true;
			}
			while (content_find_next((void *)handle, &item));
		}
	}
}

/* rebuilds both level lists */
// @retail 0x19c190
void function_19c190(void)
{
	data_make_valid_inlined(g_4ee4e8);
	data_make_valid_inlined(g_4ee4e4);
	function_19c120();
	function_19c6d0(g_4ee4e8, 1);
	function_19c7e0(g_4ee4e4);
}
