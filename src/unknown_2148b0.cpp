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

// score 2 (was 2): Frame agrees; numerator conversion follows the denominator reload instead of preceding it.
// Tried accumulate fraction 29; real numerator 29; integer numerator 40; extended numerator 29; converted
// operands 32; integral and real 32; fraction and total 32; integral operands 28. Plain total 37;
// double fraction 86; double operands 3; branch-assigned fraction 27; fraction first 2;
// state snapshot 2; range selection 42; high range first 43; mixed precision division 2.
// Implicit numerator 2; implicit denominator 2; nested total guard 2; low state uses initial fraction 39;
// active result first 39; denominator at least one 41; outer branches reversed 31; unit numerator factor 2;
// entry completed local 51.
// nonzero unsigned total 3; upper clamp first 20; optional output clamp 39; inactive entry return 63;
// availability from tests 49; both results by branch 45; inclusive low bound 4; inclusive high bound 4.
// Clamp inside the active branch 40; numerator assigned then divided 29; named denominator 32;
// clamp as two ifs 2; numerator named in the division arm 29. A non-volatile total converts the
// numerator first but keeps total in EAX (37); a volatile numerator goes through EAX (40).
// Round 39: the else arm as one ternary 42; a real numerator before the total test 50, at the top 58;
// the middle range before the high range 15.
// No other unsigned conversion in retail follows an fild (byte scan), so no matched model for the order.
// Round 57 (exact one-file harness): `(double)` casts on both operands, on the denominator, on the
// numerator, a long double numerator 3/3/3/3 (same order); implicit numerator 2; completed and total
// as two separate globals 2; numerator assigned then `/=` and a reciprocal product both longer.
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
			fraction = (real)g_55bcf8.completed / (real)g_55bcf8.total;
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
