// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1FB940.CPP: the clusters an event in one cluster reaches (with
   event_dispatch_group's 0x1fbac0..0x1fc210) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_249e20.h"
#include <string.h>

/* the clusters near the last cluster asked about, one bit each */
dword g_4f5728[0x10];
extern short g_4f5768; /* ai.cpp resets it */

#define MACRO_46A44D(count) (((count) + 31) >> 5)
#define BIT_VECTOR_SIZE_IN_BYTES(count) (4 * MACRO_46A44D(count))

/* the clusters that hear the cluster: not cut off from it, and nearer than
   40 world units */
// @retail 0x1fb940
dword *function_1fb940(short cluster_index)
{
	if (cluster_index != g_4f5768)
	{
		s_structure_bsp_view *bsp = (s_structure_bsp_view *)g_4e0348;
		short i;

		memset(g_4f5728, 0, BIT_VECTOR_SIZE_IN_BYTES(bsp->cluster_count));
		for (i = 0; i < bsp->cluster_count; i++)
		{
			if (!function_249c20(cluster_index, i, bsp) && function_249d60(cluster_index, i, bsp) < 40.0f)
				g_4f5728[i >> 5] |= 1 << (i & 31);
		}
		g_4f5768 = cluster_index;
	}

	return g_4f5728;
}
