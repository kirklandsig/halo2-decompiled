#include "unknown_11c920.h"
#include "cache_copy_globals.h"
// @flags /O2 /Ob1 /arch:SSE /Gr





#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

// @retail 0x2148b0
bool function_2148b0(real *progress)
{
	bool result = false;
	real fraction = 0.0f;

	if (g_55b4f0.state)
	{
		if (g_55b4f0.state < 11)
		{
			fraction = 0.0f;
		}
		else if (g_55b4f0.state > 12)
		{
			fraction = 1.0f;
		}
		else if (g_55b4f0.progress.total > 0)
		{
			// + 0.0f: kept only with the match (it loads the count before the total is reread, as retail)
			fraction = ((real)g_55b4f0.progress.completed + 0.0f) / (real)g_55b4f0.progress.total;
		}
		result = true;
	}
	else if (g_55b4f0.requests[1].map_name[0])
	{
		fraction = 0.0f;
		result = true;
	}
	fraction = PIN(fraction, 0.0f, 1.0f);
	if (progress)
	{
		*progress = fraction;
	}
	return result;
}

long map_location_get(char const *map_name);
bool map_names_equal(char const *map_name, char const *other_map_name);

// @retail 0x214940
real cache_copy_progress_for_map(char const *map_name)
{
	real result = 0.0f;
	long location = map_location_get(map_name);

	if (location == 3)
	{
		return 1.0f;
	}
	if (location == 2)
	{
		if (map_names_equal(g_55b4f0.path, map_name))
		{
			function_2148b0(&result);
		}
		return result;
	}
	return 0.0f;
}
