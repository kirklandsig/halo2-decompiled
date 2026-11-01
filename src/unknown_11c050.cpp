// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_11C050.CPP: queries on the cluster of a location (s_location).
   Decompiled by lane R for the effects (0x178020, 0x179730). */

#include "unknown_11c920.h"
#include "globals.h"
#include "object_queries.h"
#include "location_zones.h"
#include <string.h>

struct s_bsp3d;
extern s_bsp3d *g_4e033c;
long function_14a280(s_bsp3d *bsp, long index, point3f *point);

// @retail 0x11bf90
long function_11bf90(long object_index, point3f *point)
{
	short attempts = 0;
	while (function_14a280(g_4e033c, 0, point) == NONE && attempts++ < 150)
		point->z += 0.05f;
	return attempts == 0;
}

struct s_entry_11bf30
{
	byte unknown00[0x24];
	long tag_index;
	byte unknown28[0x64 - 0x28];
};

// @retail 0x11bf30
bool function_11bf30(s_location const *location)
{
	s_11c120_bsp *bsp = (s_11c120_bsp *)g_4e0348;
	s_11c120_cluster *cluster = &bsp->clusters[location->cluster_index];
	bool result = false;
	if (cluster->entry_index != NONE && cluster->entry_index < bsp->entry_count)
	{
		s_entry_11bf30 *entry = (s_entry_11bf30 *)bsp->entries + cluster->entry_index;
		if (entry->tag_index != NONE)
			result = (*g_4e3b44[entry->tag_index & 0xffff].bytes & 1) != 0;
	}
	return result;
}

struct s_named_entry_11c0b0
{
	char name[0x20];
	long value;
};

struct s_name_table_11c0b0
{
	byte unknown00[0x48];
	long count;
	s_named_entry_11c0b0 *entries;
};

// @retail 0x11c0b0
short function_11c0b0(char const *name, s_name_table_11c0b0 *table)
{
	s_named_entry_11c0b0 *entries;
	for (short i = 0; i < table->count; i++)
	{
		entries = table->entries;
		if (!strcmp(entries[i].name, name))
			return i;
	}
	return NONE;
}

#define CLUSTER_BITS(offset) ((dword *)((byte *)g_4e6948 + (offset)))

__declspec(noinline)
// @retail 0x11c050
bool function_11c050(s_location const *location)
{
	long cluster_index = location->cluster_index;

	return (CLUSTER_BITS(0x1178)[cluster_index >> 5] & (1 << (cluster_index & 31))) != 0;
}

__declspec(noinline)
// @retail 0x11c080
bool function_11c080(s_location const *location)
{
	long cluster_index = location->cluster_index;

	return (CLUSTER_BITS(0x1138)[cluster_index >> 5] & (1 << (cluster_index & 31))) != 0;
}

/* retail calls this out of line, except where it inlines the body
   (location_point_in_zone_inline in location_zones.h) */
__declspec(noinline) bool function_11c120(s_location const *location, point3f const *point, short *zone_index);

// @retail 0x11c120
bool function_11c120(s_location const *location, point3f const *point, short *zone_index)
{
	return location_point_in_zone_inline(location, point, zone_index);
}
