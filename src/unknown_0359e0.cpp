// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0359E0.CPP: bitmap dimensions and coordinate interleaving */

#include "unknown_11c920.h"

#define MAXIMUM(a, b) ((a) > (b) ? (a) : (b))
#define IS_POWER_OF_TWO(x) (!((x) & ((x) - 1)))

long log2_floor(dword value);

// @retail 0x1d6f0
dword function_1d6f0(bool linear, bool alternate, dword format, long width, long height)
{
	/* Both dimensions occupy stack slots in retail. */
	long const *width_reference = &width;
	long const *height_reference = &height;
	dword type = linear ? (alternate ? 0x2e : 0x12) : (alternate ? 0x2e : 6);
	format = (((format << 8) | type) << 8) | 0x29;
	if (!linear)
	{
		long width_bits = 1;
		long height_bits = 1;
		while ((1 << width_bits) < *width_reference)
			++width_bits;
		while ((1 << height_bits) < *height_reference)
			++height_bits;
		format |= (width_bits | (height_bits << 4)) << 20;
	}
	return format;
}

word const g_450840[64] =
{
	0x000, 0x001, 0x004, 0x005, 0x010, 0x011, 0x014, 0x015,
	0x040, 0x041, 0x044, 0x045, 0x050, 0x051, 0x054, 0x055,
	0x100, 0x101, 0x104, 0x105, 0x110, 0x111, 0x114, 0x115,
	0x140, 0x141, 0x144, 0x145, 0x150, 0x151, 0x154, 0x155,
	0x400, 0x401, 0x404, 0x405, 0x410, 0x411, 0x414, 0x415,
	0x440, 0x441, 0x444, 0x445, 0x450, 0x451, 0x454, 0x455,
	0x500, 0x501, 0x504, 0x505, 0x510, 0x511, 0x514, 0x515,
	0x540, 0x541, 0x544, 0x545, 0x550, 0x551, 0x554, 0x555,
};

PRIVATE inline long swizzle_log2(dword value)
{
	long result = 0;
	if (value > 0)
	{
		while (value != 1)
		{
			value >>= 1;
			++result;
		}
	}
	return result;
}

// @retail 0x358d0
void function_358d0(short width, short x, short y, short height, dword *out)
{
	long x_value = x;
	long y_value = y;
	short width_bits = (short)swizzle_log2(width);
	short height_bits = (short)swizzle_log2(height);
	short common_bits = width_bits > height_bits ? height_bits : width_bits;
	short mask = (short)((1 << common_bits) - 1);
	dword x_bits;
	dword y_bits;
	if (mask <= 63)
	{
		x_bits = g_450840[x_value & mask];
		y_bits = g_450840[y_value & mask];
	}
	else
	{
		x_bits = (g_450840[(x_value >> 6) & (mask >> 6)] << 12) | g_450840[x_value & 63];
		y_bits = (g_450840[(y_value >> 6) & (mask >> 6)] << 12) | g_450840[y_value & 63];
	}
	y_bits <<= 1;
	if (width_bits > common_bits)
		x_bits |= (x_value >> common_bits) << (common_bits * 2);
	else if (height_bits > common_bits)
		y_bits |= (y_value >> common_bits) << (common_bits * 2);
	out[0] = x_bits;
	out[1] = y_bits;
}

static inline bool function_x9955e3(long format)
{
	return format >= 14 && format <= 16;
}

/* the levels down to 1x1 (compressed formats stop at 4x4), none for a bitmap
   whose sides are not powers of two, at most maximum_levels */
// @retail 0x359e0
short bitmap_get_mipmap_count(short width, short height, short depth, short format, bool linear, short maximum_levels)
{
	short result = 0;

	if (IS_POWER_OF_TWO(width) && IS_POWER_OF_TWO(height) && IS_POWER_OF_TWO(depth))
	{
		if (linear)
		{
			result = 0;
		}
		else if (function_x9955e3(format))
		{
			result = (short)log2_floor(MAXIMUM(width / 4, MAXIMUM(height / 4, depth)));
		}
		else
		{
			result = (short)log2_floor(MAXIMUM(width, MAXIMUM(height, depth)));
		}
	}
	return maximum_levels > result ? result : maximum_levels;
}
