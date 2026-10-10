// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_122870.CPP: the map cache file: its header (0x800 bytes between 'head'
   and 'foot'), the tag index read from it, tag lookups by group and the
   structure bsp it loads. */

#include "unknown_11c920.h"
#include "globals.h"
#include "async.h"
#include "unknown_122870.h"
#include <xtl.h>
#include <string.h>

struct s_cache_tag_instance
{
	long group_tag;
	long datum_index;
	void *address;
	long size;
};

struct s_cache_tag_group
{
	long group_tag;
	long parent_group_tags[2];
};

struct s_structure_bsp_reference
{
	dword offset;
	long size;
	s_structure_bsp_header *address;
	byte unknown0c[8];
	long bsp_tag_index;
	byte unknown18[4];
	long lightmap_tag_index;
};

struct s_tag_iterator
{
	long unknown00;
	long unknown04;
	long datum_index;
	long next_index;
	long group_tag;
};

long function_13ddd0(const void *key, const void *base, long count, long element_size, long (__stdcall *compare)(const void *, const void *, const void *), const void *context);
long function_213760(dword location, long size, void *buffer, dword *bytes_read, bool *done, long category, long priority);
void function_213890(void);
long cache_file_get_maximum_size(long type);

extern long g_55aca8;

s_cache_file_globals cache_file_globals;

#define CACHE_TAG_INSTANCES ((s_cache_tag_instance *)g_4e3b44)

// @retail 0x122870
bool cache_header_verify(s_cache_header const *header)
{
	bool result = false;
	FILETIME time;

	GetSystemTimeAsFileTime(&time);
	if (header->header_signature == 'head' &&
		header->footer_signature == 'foot' &&
		header->size >= 0 && header->size <= 0x20800000 &&
		header->type >= 0 && header->type < 5 &&
		header->size <= cache_file_get_maximum_size(header->type) &&
		strlen(header->name) < sizeof(header->name) &&
		header->version == 8)
	{
		result = true;
	}
	return result;
}

/* the open cache files (0x804 bytes each, unknown_213760.cpp): a handle and
   the file's header */
struct s_cache_file
{
	HANDLE handle;
	byte unknown04[0x800];
};

extern s_cache_file g_557c90[3];

long cache_file_find(char const *map_name);
bool function_122d60(s_cache_file_location location, long size, void *buffer);
bool __stdcall version_is_compatible(char const *version);

/* set when a map fails to load (the main loop shows the error) */
long g_510a08;
extern bool g_510819;

/* the scenario tag of the loaded map */
extern long g_4686c0;

/* takes size bytes, rounded up to whole pages, from the bottom of the
   current physical memory stage; NULL when it is full */
static __forceinline void *physical_memory_malloc_low(long size, dword protect)
{
	void *result = NULL;
	long stage = g_global_f9ae07.field_0;
	long *bottom = &g_global_f9ae07.field_c_6[stage];
	long address = g_global_f9ae07.field_c_6[stage];
	long aligned_size = (size + 0xfff) & 0xfffff000;
	long top = address + aligned_size;

	if (top <= g_global_f9ae07.field_20[stage])
	{
		*bottom = top;
		result = (void *)address;
		if (address)
		{
			result = (void *)(address | 0x80000000);
			if (result)
				XPhysicalProtect(result, aligned_size, protect);
		}
	}
	return result;
}

static inline long cache_file_sector_align(long size)
{
	if (size & 0x1ff)
		size = (size | 0x1ff) + 1;
	return size;
}

/* loads a map's cache file: its header, then its tag data (the tags header
   and the tag instances) into physical memory; points the tag instances, the
   scenario and the globals at what it read */
// @retail 0x1228f0
bool cache_files_load_map(char const *map_name)
{
	bool result = false;
	long scenario_index = NONE;

	g_55aca8 = cache_file_find(map_name);
	cache_file_globals.header = *(s_cache_header *)g_557c90[g_55aca8].unknown04;
	if (cache_header_verify(&cache_file_globals.header) && version_is_compatible(cache_file_globals.header.build_version))
	{
		cache_file_globals.field_4_7 = physical_memory_malloc_low(cache_file_globals.header.unknown1c, PAGE_READWRITE);
		result = cache_file_globals.field_4_7 != NULL;
		if (result)
		{
			s_cache_file_location location;

			location.file_index = NONE;
			location.offset = cache_file_globals.header.tag_data_offset;
			result = function_122d60(location, cache_file_sector_align(cache_file_globals.header.tag_data_size), cache_file_globals.field_4_7);
			if (!result)
			{
				g_510a08 = 0;
				g_510819 = true;
			}
			else
			{
				location.file_index = NONE;
				location.offset = cache_file_globals.header.tag_data_offset + cache_file_globals.header.tag_data_size;
				result = function_122d60(location, cache_file_sector_align(cache_file_globals.header.unknown18),
					(byte *)cache_file_globals.field_4_7 - cache_file_globals.header.unknown18 + cache_file_globals.header.unknown1c);
				if (result)
				{
					s_cache_tags_header *tags = (s_cache_tags_header *)cache_file_globals.field_4_7;

					if (tags->instances && tags->instance_count > 0 && tags->signature == 'tags')
					{
						cache_file_globals.tags = tags;
						result = true;
						cache_file_globals.loaded = result;
						scenario_index = tags->scenario_index;
						g_4e3b44 = (s_tag_instance *)tags->instances;
					}
					else
					{
						result = false;
					}
				}
				if (!result)
				{
					g_510a08 = 0;
					g_510819 = true;
				}
			}
		}
	}
	if (!result)
	{
		if (cache_file_globals.field_4_7)
			cache_file_globals.field_4_7 = NULL;
		if (g_55aca8 != NONE)
		{
			function_213890();
			g_55aca8 = NONE;
		}
	}
	g_4686c0 = scenario_index;
	if (scenario_index != NONE)
	{
		g_4e0350 = (s_palette_source_globals *)CACHE_TAG_INSTANCES[scenario_index & 0xffff].address;
		g_4e034c = (s_tag_header_globals *)CACHE_TAG_INSTANCES[cache_file_globals.tags->globals_index & 0xffff].address;
	}
	return result;
}

// @retail 0x122af0
void cache_files_dispose_map(void)
{
	if (g_55aca8 != NONE)
	{
		function_213890();
		g_55aca8 = NONE;
	}
	cache_file_globals.loaded = false;
	cache_file_globals.field_4_7 = NULL;
	memset(&cache_file_globals.header, 0, sizeof(cache_file_globals.header));
	g_4e3b44 = NULL;
	cache_file_globals.tags = NULL;
	cache_file_globals.bsp = NULL;
}

bool function_122d60(s_cache_file_location location, long size, void *buffer);

// @retail 0x122b40
bool cache_files_load_structure_bsp(s_structure_bsp_reference *bsp)
{
	bool result = false;
	long size = bsp->size;

	if (size & 0x1ff)
		size = (size | 0x1ff) + 1;
	s_cache_file_location location;

	location.file_index = NONE;
	location.offset = bsp->offset;
	if (function_122d60(location, size, bsp->address) && bsp->address->signature == 'sbsp')
	{
		cache_file_globals.bsp = bsp->address;
		CACHE_TAG_INSTANCES[(short)bsp->bsp_tag_index].address = cache_file_globals.bsp->bsp_address;
		if (bsp->lightmap_tag_index != NONE)
			CACHE_TAG_INSTANCES[(short)bsp->lightmap_tag_index].address = cache_file_globals.bsp->lightmap_address;
		result = true;
	}
	return result;
}

/* geometry_cache: forgets the streamed blocks of a tag (unknown_12de70.cpp) */
void function_12e150(long tag_index);

static inline void cache_files_unload_tag(long tag_index)
{
	s_cache_tag_instance *instance = &CACHE_TAG_INSTANCES[(short)tag_index];

	function_12e150(tag_index);
	instance->address = NULL;
}

// @retail 0x122bc0
void cache_files_unload_structure_bsp(s_structure_bsp_reference *bsp)
{
	cache_files_unload_tag(bsp->bsp_tag_index);
	if (*(long const volatile *)&bsp->lightmap_tag_index != NONE)
	{
		cache_files_unload_tag(bsp->lightmap_tag_index);
	}
	cache_file_globals.bsp = NULL;
}
s_cache_tag_group *cache_tag_group_get(long group_tag);

s_cache_tag_instance *cache_tag_instance_get(long tag_index);

static inline bool cache_tag_group_is(s_cache_tag_group const *group, long group_tag)
{
	return group->group_tag == group_tag || group->parent_group_tags[0] == group_tag || group->parent_group_tags[1] == group_tag;
}

// @retail 0x122c10
void *function_122c10(long group_tag, long tag_index)
{
	void *result = NULL;
	s_cache_tag_instance *instance = cache_tag_instance_get(tag_index);

	if (instance && cache_tag_group_is(cache_tag_group_get(instance->group_tag), group_tag))
		result = instance->address;
	return result;
}

// @retail 0x122c70
long function_122c70(s_tag_iterator *iterator)
{

	long local_0 = NONE;
	if (iterator->next_index < cache_file_globals.tags->instance_count)
	{
		do
		{
			long local_1 = *(long volatile *)&iterator->next_index;
			s_cache_tag_instance *instance = &CACHE_TAG_INSTANCES[local_1];
			iterator->next_index = local_1 + 1;
			if (instance && instance->group_tag != NONE && instance->datum_index != NONE)
			{
				if (iterator->group_tag != NONE)
				{
					s_cache_tag_group *group = cache_tag_group_get(instance->group_tag);
					if (iterator->group_tag != group->group_tag && iterator->group_tag != group->parent_group_tags[0] && iterator->group_tag != group->parent_group_tags[1])
						continue;
				}
				iterator->datum_index = instance->datum_index;
				local_0 = instance->datum_index;
				goto local_2;
			}
		} while (iterator->next_index < cache_file_globals.tags->instance_count);
	}
local_2:
	return local_0;
}

// @retail 0x122cf0
long __stdcall cache_tag_group_compare(void const *a, void const *b, void const *context)
{
	return *(long const *)a - *(long const *)b;
}

// @retail 0x122d00
s_cache_tag_group *cache_tag_group_get(long group_tag)
{
	s_cache_tag_group *result = NULL;

	if (cache_file_globals.tags)
	{
		s_cache_tag_group key = { group_tag, NONE, NONE };
		long index = function_13ddd0(&key, cache_file_globals.tags->groups, cache_file_globals.tags->group_count, sizeof(s_cache_tag_group), cache_tag_group_compare, NULL);
		if (index != NONE)
			result = &cache_file_globals.tags->groups[index];
	}
	return result;
}

// @retail 0x122d60
bool function_122d60(s_cache_file_location location, long size, void *buffer)
{

	bool result = false;
	dword bytes_read;
	function_213760(location.offset, size, buffer, &bytes_read, (bool *)&location.offset, 2, 6);
	function_120d50((bool volatile *)&location.offset, false);
	if (bytes_read == size)
		result = true;
	return result;
}

long map_location_get(char const *map_name);
bool map_names_equal(char const *map_name, char const *other_map_name);
bool function_2148b0(long progress);
extern char g_55bd21[0x103];
extern char const *g_4687fc;
extern char const *g_468800;
extern char const *g_468804;
extern char const *g_468808;

static __forceinline real cache_map_progress(char const *name)
{
	real progress = 0.0f;
	long location = map_location_get(name);
	switch (location)
	{
	case 3:
		progress = 1.0f;
		break;
	case 2:
		if (map_names_equal(g_55bd21, name))
			function_2148b0((long)&progress);
		break;
	default:
		progress = 0.0f;
	}
	return progress;
}

// @retail 0x122dd0
real __stdcall function_122dd0(byte *map_name, long mode, long type)
{
	(void)&map_name;
	(void)&mode;
	real map_weight = 100.0f;
	real progress = 0.0f;
	long location = map_location_get((char const *)map_name);
	real map_progress = cache_map_progress((char const *)map_name);
	real menu_progress = cache_map_progress(g_468804);
	real shared_progress = cache_map_progress(g_4687fc);
	real campaign_progress = cache_map_progress(g_468800);
	if (!strcmp((char const *)map_name, g_468808))
		map_weight = 90.0f;
	else if (type == 0)
		map_weight = 280.0f;
	else if (type == 1)
		map_weight = 100.0f;
	long menu_state = map_location_get(g_468804);
	long shared_state = map_location_get(g_4687fc);
	long campaign_state = map_location_get(g_468800);
	if (menu_state == 4) menu_progress = 1.0f;
	if (shared_state == 4) shared_progress = 1.0f;
	if (campaign_state == 4) campaign_progress = 1.0f;
	if (!strcmp((char const *)map_name, g_468808)) campaign_progress = 1.0f;
	real weight = 0.0f;
	switch (type)
	{
	case 0:
		weight = 500.0f;
		progress = campaign_progress * weight;
	case 1:
	case 4:
		progress = shared_progress * 200.0f + progress;
		weight += 200.0f;
	case 3:
		progress = menu_progress * 80.0f + progress;
		weight += 80.0f;
	case 2:
		weight += map_weight;
		progress = map_progress * map_weight + progress;
		if (!(weight >= 0.0f))
		{
			progress = 1.0f;
			goto done;
		}
	}
	switch (mode)
	{
	case 0:
		progress /= weight;
		if (progress < 0.0f) progress = 0.0f;
		else if (progress > 1.0f) progress = 1.0f;
		break;
	default:
		progress = weight - progress;
	}
done:
	if (location == 4) progress = 0.0f;
	return progress;
}

// @retail 0x122db0
long function_122db0(char const *map_name, long type)
{
	return ((long)function_122dd0((byte *)map_name, 1, type) / 4) * 1000;
}
