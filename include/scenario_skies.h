/* SCENARIO_SKIES.H: the scenario's sky block as read by the cluster sky lookup
   (src/unknown_14a280.cpp, 0x14b360) and the sky lighting query
   (src/unknown_0d0690.cpp, 0xd1a30): each entry is a tag reference whose tag
   index names a sky tag. */

#ifndef SCENARIO_SKIES_H
#define SCENARIO_SKIES_H

#include "unknown_11c920.h"
#include "globals.h"
#include <stddef.h>

/* one entry of the scenario's sky block: a tag reference, 8 bytes */
struct s_scenario_sky_reference
{
	byte unknown00[4];
	long sky_index;
};
typedef char assert_scenario_sky_reference_size[(sizeof(s_scenario_sky_reference) == 8) ? 1 : -1];

/* the scenario (g_4e0350) as the sky lookups see it: its sky block and flags */
struct s_scenario_skies_view
{
	byte unknown00[8];
	long sky_count;
	s_scenario_sky_reference *skies;
	byte unknown10[2];
	byte flags;
};
typedef char assert_scenario_skies_view_count[(offsetof(s_scenario_skies_view, sky_count) == 8) ? 1 : -1];
typedef char assert_scenario_skies_view_flags[(offsetof(s_scenario_skies_view, flags) == 0x12) ? 1 : -1];

/* the tag data of a scenario sky, or NULL when the index or its tag is NONE */
__forceinline byte *sky_surface_data(short index)
{
	s_scenario_skies_view *scenario = (s_scenario_skies_view *)g_4e0350;
	long tag_index = NONE;
	if (index >= 0)
	{
		long count = scenario->sky_count;
		if (index < count)
			tag_index = scenario->skies[index].sky_index;
	}
	byte *result = NULL;
	if (tag_index != NONE)
		result = g_4e3b44[tag_index & 0xffff].bytes;
	return result;
}

#endif
