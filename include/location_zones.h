/* LOCATION_ZONES.H: the zones of the structure clusters (unknown_11c050.cpp) */

#ifndef LOCATION_ZONES_H
#define LOCATION_ZONES_H

#include "unknown_11c920.h"
#include "globals.h"
#include "object_queries.h"

struct s_11c120_cluster
{
	byte unknown00[0x70];
	byte zone;
	byte unknown71;
	short entry_index;
	byte unknown74[0xb0 - 0x74];
};

struct s_11c120_zone
{
	byte unknown00[2];
	short index;
	plane3f plane;
	byte unknown14[4];
};

struct s_11c120_bsp
{
	byte unknown00[0x68];
	s_11c120_zone *zones;
	byte unknown6c[0xa0 - 0x6c];
	s_11c120_cluster *clusters;
	byte unknowna4[0xd4 - 0xa4];
	long entry_count;
	byte *entries;
};

/* whether a point lies in the zone of its location's cluster: anywhere in the cluster when the
   zone byte's top bit is clear, otherwise below the zone's plane; the zone's index goes to
   zone_index. function_11c120 is this out of line; callers retail inlines it into use it directly */
__forceinline bool location_point_in_zone_inline(s_location const *location, point3f const *point, short *zone_index)
{
	bool result = false;
	short cluster_index = location->cluster_index;

	if (cluster_index != NONE)
	{
		s_11c120_bsp *bsp = (s_11c120_bsp *)g_4e0348;
		s_11c120_cluster *cluster = &bsp->clusters[cluster_index];

		if (cluster->zone != 0xff)
		{
			s_11c120_zone *entry = &bsp->zones[cluster->zone & 0x7f];
			short index = entry->index;

			if (index != NONE)
			{
				if (!(cluster->zone & 0x80) || 0.0f > entry->plane.k * point->z + entry->plane.j * point->y + entry->plane.i * point->x - entry->plane.d)
				{
					result = true;
					if (zone_index)
						*zone_index = index;
				}
			}
		}
	}
	return result;
}

#endif
