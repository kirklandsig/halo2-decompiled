#include "unknown_11c920.h"
// @flags /O2 /Ob1 /arch:SSE /Gr

extern long g_55bd04;
struct s_cache_copy_progress
{
	long completed;
	volatile dword total;
};

s_cache_copy_progress g_55bcf8;

struct s_cache_copy_request
{
	char map_name[256];
	long priority;
};

extern s_cache_copy_request g_55be24[2];

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

// @retail 0x2148b0
bool function_2148b0(real *progress)
{
	bool result = false;
	real fraction = 0.0f;

	if (g_55bd04)
	{
		if (g_55bd04 < 11)
		{
			fraction = 0.0f;
		}
		else if (g_55bd04 > 12)
		{
			fraction = 1.0f;
		}
		else if (g_55bcf8.total > 0)
		{
			// + 0.0f: kept only with the match (it loads the count before the total is reread, as retail)
			fraction = ((real)g_55bcf8.completed + 0.0f) / (real)g_55bcf8.total;
		}
		result = true;
	}
	else if (g_55be24[1].map_name[0])
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
extern char g_55bd21[0x103];

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
		if (map_names_equal(g_55bd21, map_name))
		{
			function_2148b0(&result);
		}
		return result;
	}
	return 0.0f;
}
