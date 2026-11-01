// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_19EC40.CPP: finds entries of the 4e0350 globals' second table by
   position and by three optional 16-bit keys */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_19ec40.h"
#include <math.h>

real distance_sq3f(point3f const *a, point3f const *b);
void marker_get_position_and_angle(long index, point3f *position, real *angle);

/* collects the indices of the marker entries whose three keys match (a key of -1
   matches any entry), up to the maximum count; with a point, only entries within
   the radius and the vertical distance count */
// @retail 0x19ec40
long function_19ec40(
	point3f const *point,
	real height,
	short key_a,
	short key_b,
	short key_c,
	long maximum_count,
	long *results,
	real radius)
{
	s_palette_source_globals *globals = g_4e0350;
	real radius_squared = radius * radius;
	real best = 0.0f;
	long count = 0;

	for (short i = 0; i < globals->marker_count; i++)
	{
		s_marker_entry *entry = &globals->marker_entries[i];

		if (key_a == -1 || key_a == entry->key_a)
		{
			if (key_b == -1 || key_b == entry->key_b)
			{
				if (key_c == -1 || key_c == entry->key_c)
				{
					real distance_squared = 0.0f;

					if (point)
					{
						point3f position;
						marker_get_position_and_angle(i, &position, NULL);
						distance_squared = distance_sq3f(&position, point);
						if (radius >= 0.0f && distance_squared > radius_squared)
							continue;
						if (height > 0.0f && fabs(entry->position.z - point->z) > height)
							continue;
					}

					if (count < maximum_count)
						results[count++] = i;
					else if (count == 1 && best > distance_squared)
					{
						results[0] = i;
						best = distance_squared;
					}
				}
			}
		}
	}

	return count;
}
