// @flags /O2 /Ob1 /Gr
/* UNKNOWN_01E930.CPP: Bink movie playback

Bink allocates through function_156710 and
function_156810, which video_playback_setup (unknown_155ea0.cpp)
registers: the allocations come from a permanent block of physical memory
and are tracked in g_4e9148. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_01e930.h"
#include "unknown_03d380.h"
#include "physical_memory.h"
#include <xtl.h>
#include <d3d8.h>
#include <string.h>

enum
{
	k_maximum_bink_allocations = 16,
	k_bink_allocation_slack = 0x3000
};

/* the outstanding Bink allocations and their count */
void *g_4e9148[k_maximum_bink_allocations];
long g_510c64;
/* the available physical memory, in kilobytes */
long g_47ff80 = NONE;
dword g_51ebec;
extern dword g_54d5b8;
extern byte g_4e6388;

void __stdcall function_18f1c0(long a);
void function_12d520(long a);
bool function_12b3c0(void);
bool function_23e400(long button);
void __stdcall function_35b90(void *material);
void __stdcall function_363a0(void *vertices);
void __stdcall function_1e930(long a);
int __stdcall function_3e2330(void *movie);
int __stdcall function_3e2870(void *movie);
void __stdcall function_3e2e50(void *movie);
int __stdcall function_3e2830(void *movie, void *destination, long pitch, long height, long x, long y, dword flags);

/* the screen rectangles (unknown_0167a0.cpp) */
struct short_rect
{
	short v0, v1, v2, v3;
};

struct short_rect_pair
{
	short_rect a, b;
};

extern short_rect_pair g_485a8a;
byte g_4c1a18;

/* the head of a Bink movie (HBINK) */
struct s_bink_movie
{
	dword width;
	dword height;
	dword frames;
	dword frame_number;
	dword last_frame_number;
	dword frame_rate;
	dword frame_rate_divisor;
};

struct s_bink_vertex
{
	real x;
	real y;
	real u;
	real v;
	dword color;
};

/* per-track sound settings for the Bink sound callback */
struct s_bink_sound_track
{
	dword unknown00;
	real volume;
	byte unknown08[0x18];
};

s_bink_sound_track g_4e9300[8];

/* the texture cache's physical memory (unknown_12c0d0.cpp) */
extern s_physical_object *g_4e6464;
byte g_4e6389;

/* the sound settings Bink plays through (unknown_01e930.h) */
s_bink_sound_settings *g_51ebe4;

/* the bitmap the movie's texture is drawn as */
struct s_bink_bitmap
{
	dword signature;
	short width;
	short height;
	byte depth;
	byte unknown09;
	short unknown0a;
	short format;
	short flags;
	byte unknown10[0x28 - 0x10];
	long unknown28;
	byte unknown2c[0x34 - 0x2c];
	long size;
	byte unknown38[0x4c - 0x38];
	long unknown4c;
	void *texture;
	long unknown54;
};

s_bink_bitmap g_55ece8;

long __stdcall function_12d2f0(long a, long b, long c, long d);
void function_12c600(void);
double timing_ticks_to_seconds(__int64 ticks);
bool function_2148b0(long a);
struct D3DTexture *function_23e340(short width, short height, short format,
	void *(__stdcall *allocate)(long size, long alignment), long *size, void **data, struct D3DTexture *header);
short function_1358c0(short format);
void __stdcall function_3e0450(void *open, void *direct_sound);
void *__stdcall function_3e0af0(char const *name, dword flags);
int __stdcall function_3e2630(void *movie, dword track, dword *bins, dword count);
int __stdcall function_3e2680(void *movie, dword track, dword *bins, long *volumes, dword count);
void *__stdcall function_3e3c90(void *a);

/* the shared body of function_155e50, which retail inlines into
   the memory callbacks */
PRIVATE inline void bink_update_memory_available(void)
{
	MEMORYSTATUS status;

	memset(&status, 0, sizeof(status));
	status.dwLength = sizeof(status);
	GlobalMemoryStatus(&status);
	g_47ff80 = status.dwAvailPhys >> 10;
}

// @retail 0x155e50
void function_155e50(void)
{
	bink_update_memory_available();
}

// @retail 0x155f60
bool function_155f60(void)
{
	return g_4e9188.movie && g_4e9188.initialized;
}

// @retail 0x1565e0
void function_1565e0(void)
{
	if (g_4e9188.initialized && g_4e9188.permanent_memory)
	{
		function_12d520((long)g_4e9188.permanent_memory);
		g_4e9188.permanent_memory = NULL;
		g_4e9188.texture = NULL;
		g_4e9188.permanent_memory_size = 0;
	}
}

// @retail 0x156560
void function_156560(void)
{
	if (g_4e9188.initialized)
	{
		if (g_4e9188.movie)
		{
			function_3e2ff0(g_4e9188.movie);
			g_4e9188.movie = NULL;
		}
		function_1565e0();
		if (g_4e9188.flag1)
		{
			g_4e9188.flag1 = false;
			g_4e6388 = 0;
		}
	}
}

/* a lifecycle callback with the body of function_156560 */
// @retail 0x1566c0
void __stdcall function_1566c0(dword a, dword b)
{
	if (g_4e9188.initialized)
	{
		if (g_4e9188.movie)
		{
			function_3e2ff0(g_4e9188.movie);
			g_4e9188.movie = NULL;
		}
		function_1565e0();
		if (g_4e9188.flag1)
		{
			g_4e9188.flag1 = false;
			g_4e6388 = 0;
		}
	}
}

// @retail 0x1565a0
void bink_playback_end(void)
{
	if (g_4e9188.initialized)
	{
		function_156560();
		if (g_4e9188.flags & 0x20)
			function_18f1c0(0);
		g_4e9188.flags = 0;
		g_51ebec = g_54d5b8;
	}
}

// @retail 0x155ed0
void function_155ed0(void)
{
	if (g_4e9188.initialized)
	{
		function_156560();
		if (g_4e9188.flags & 0x20)
			function_18f1c0(0);
		g_51ebec = g_54d5b8;
		memset(&g_4e9188, 0, sizeof(g_4e9188));
	}
}

// @retail 0x156620
void *__stdcall function_156620(long size, long alignment)
{
	byte *result = g_4e9188.permanent_memory + g_4e9188.permanent_memory_size - size;

	if (alignment)
	{
		if ((long)result & (alignment - 1))
		{
			size += (alignment - (long)result) & (alignment - 1);
			result -= (alignment - 1) & (alignment - (long)result);
			g_4e9188.permanent_memory_size -= size;
			return result;
		}
		g_4e9188.permanent_memory_size -= size;
		return result;
	}
	g_4e9188.permanent_memory_size -= size;
	return result;
}

// @retail 0x156690
bool function_156690(void)
{
	bool result = true;

	for (short i = 0; i < g_510c64; i++)
	{
		if (g_4e9148[i])
			result = false;
	}
	return result;
}

// @retail 0x156710
void *__stdcall function_156710(unsigned long size)
{
	void *result = NULL;

	bink_update_memory_available();

	if (g_510c64 > 0 && !g_4e9148[0] && function_156690())
	{
		g_510c64 = 0;
		g_4e9188.permanent_memory_used = 0;
	}

	if (g_4e9188.permanent_memory_used + size <= (unsigned long)g_4e9188.permanent_memory_size &&
		g_510c64 < k_maximum_bink_allocations && g_4e9188.permanent_memory)
	{
		result = g_4e9188.permanent_memory + g_4e9188.permanent_memory_used;
		g_4e9188.permanent_memory_used += size;
		XPhysicalProtect(result, size, PAGE_READWRITE);
		g_4e9148[g_510c64++] = result;
		function_155e50();
		g_4e9188.permanent_memory_used += k_bink_allocation_slack;
		if (g_4e9188.permanent_memory_used > g_4e9188.permanent_memory_size)
			g_4e9188.permanent_memory_used = g_4e9188.permanent_memory_size;
	}
	return result;
}

// @retail 0x156810
void __stdcall function_156810(void *block)
{
	bink_update_memory_available();
	for (long i = 0; i < g_510c64; i++)
	{
		if (g_4e9148[i] == block)
		{
			g_4e9148[i] = NULL;
			break;
		}
	}
	bink_update_memory_available();
}

// @retail 0x1564e0
long bink_playback_ticks_remaining(void)
{
	s_bink_movie *movie = (s_bink_movie *)g_4e9188.movie;
	long result = 0;

	if (movie)
	{
		dword frames = movie->frames;
		dword remaining = frames - movie->frame_number;
		dword milliseconds = movie->frame_rate_divisor * frames * 1000 / movie->frame_rate;
		real seconds;

		milliseconds = milliseconds * remaining / (frames > 1 ? frames : 1);
		seconds = (long)milliseconds * 0.001f * 30.0f;
		__asm
		{
			fld seconds
			fistp result
		}
		result = result > 0 ? result : 0;
	}
	return result;
}

// @retail 0x156ab0
bool function_156ab0(void)
{
	long buttons[] = { 0, 1, 2, 3, 4, 5, 6, 7, 12, 13 };
	bool result = false;

	for (dword i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++)
	{
		if (function_23e400(buttons[i]))
		{
			result = true;
			break;
		}
	}
	return result;
}

/* the Bink sound callback: a track's volume */
// @retail 0x156b30
real __stdcall function_156b30(long track, long type)
{
	real result = 0.0f;

	if (type == 0x3000577)
		result = g_4e9300[track].volume;
	return result;
}

// @retail 0x1568d0
void function_1568d0(void)
{
	D3DLOCKED_RECT locked;

	function_3e2870(g_4e9188.movie);
	function_3e2e50(g_4e9188.movie);
	g_4e9188.texture->LockRect(0, &locked, NULL, 0);
	if (locked.pBits)
		function_3e2830(g_4e9188.movie, locked.pBits, locked.Pitch, g_4e9188.height, 0, 0, g_4e9188.copy_flags | 0x80000000);
}

// @retail 0x156960
void function_156960(void)
{
	short_rect screen = g_485a8a.a;
	short left, right, top, bottom;
	s_bink_vertex vertices[4];

	if (g_4e9188.flags & 0x10)
	{
		right = screen.v3;
		left = screen.v1;
		bottom = screen.v2;
		top = screen.v0;
	}
	else
	{
		short screen_width = screen.v3 - screen.v1;
		short screen_height = screen.v2 - screen.v0;

		left = (screen_width - g_4e9188.width) / 2;
		right = (screen_width + g_4e9188.width) / 2;
		top = (screen_height - g_4e9188.height) / 2;
		bottom = (screen_height + g_4e9188.height) / 2;
	}

	for (short i = 0; i < 4; i++)
	{
		long corner = i + 1;

		vertices[i].x = (corner & 2) ? right : left;
		vertices[i].y = i > 1 ? bottom : top;
		vertices[i].u = (corner & 2) ? g_4e9188.width : 0.0f;
		vertices[i].v = i > 1 ? g_4e9188.height : 0.0f;
		vertices[i].color = 0xffffffff;
	}

	function_35b90(g_4e9188.material);
	function_363a0(vertices);
	if (g_4c1a18)
	{
		function_1e930(1);
		g_4c1a18 = 0;
	}
}

// @retail 0x155f80
void function_155f80(bool synchronous)
{
	if (g_4e9188.initialized && g_4e9188.movie)
	{
		if (function_12b3c0())
		{
			if (!synchronous)
				g_4e9188.unknown02 = !function_3e2330(g_4e9188.movie);
		}
		else if (synchronous)
		{
			while (function_3e2330(g_4e9188.movie))
				;
			g_4e9188.unknown02 = true;
		}

		if ((g_4e9188.flags & 2) && (!(g_4e9188.flags & 0x100) || g_4e9188.unknown03) && function_156ab0() ||
			g_4e9188.finished)
			bink_playback_end();

		s_bink_movie *movie = (s_bink_movie *)g_4e9188.movie;
		if ((!movie || movie->frame_number == movie->frames) && !(g_4e9188.flags & 1))
			g_4e9188.finished = true;
	}
}

// @retail 0x156040
void function_156040(void)
{
	if (g_4e9188.initialized && g_4e9188.movie)
	{
		if (function_12b3c0())
			g_4e9188.unknown02 = true;
		if (g_4e9188.unknown02)
		{
			function_1568d0();
			g_4e9188.unknown02 = false;
		}
		function_156960();
		function_155f80(true);
	}
}

PRIVATE inline __int64 bink_read_tsc(void)
{
	volatile __int64 t = 0;
	__asm rdtsc
}

// @retail 0x156090
void function_156090(char const *name, dword flags)
{
	bink_update_memory_available();

	if (!g_4e9188.initialized)
		return;
	if (function_2148b0(0) && !(flags & 0x100))
		return;

	g_4e9188.finished = false;
	if (flags & 0x200)
	{
		g_4e9188.unknown03 = false;
	}
	else
	{
		bool skippable = true;

		if (function_2148b0(0))
			skippable = false;
		g_4e9188.unknown03 = skippable;
	}

	long large = flags & 0x100;
	long size = (large | 0x40) << 16;
	void *block = NULL;

	g_4e9188.permanent_memory_size = size;
	g_4e9188.permanent_memory_used = 0;
	g_4e9188.permanent_memory = NULL;

	__int64 start = bink_read_tsc();

	if (size > 0 && g_4e6464->page_count > 0)
	{
		long attempts = 0;

		while (!(block = (void *)function_12d2f0(size, 0, 0, (long)function_1566c0)))
		{
			if (attempts < 30)
			{
				attempts++;
				function_12c600();
				continue;
			}

			__int64 elapsed = bink_read_tsc() - start;
			if (elapsed < 0)
				elapsed = 0;
			if (timing_ticks_to_seconds(elapsed) >= 0.1f)
				break;
			D3DDevice_KickPushBuffer();
			D3DDevice_IsBusy();
			SwitchToThread();
		}
	}

	g_4e9188.permanent_memory = (byte *)block;
	if (!block)
	{
		function_1565e0();
		return;
	}

	XPhysicalProtect(block, g_4e9188.permanent_memory_size, PAGE_READONLY);
	function_155e50();
	if (g_51ebe4->direct_sound)
	{
		function_155e50();
		function_3e0450(function_3e3c90, g_51ebe4->direct_sound);
		function_155e50();
	}

	g_4e9188.movie = function_3e0af0(name, large ? 0x8002000 : 0);
	function_155e50();
	if (!g_4e9188.movie)
	{
		function_1565e0();
		return;
	}

	if (g_51ebe4->surround)
	{
		dword bins[4] = { 0, 1, 4, 5 };
		long volumes[4] = { 0x8000, 0x8000, 0x5472, 0x5472 };

		function_3e2630(g_4e9188.movie, 0, bins, 4);
		function_3e2680(g_4e9188.movie, 0, bins, volumes, 4);
	}
	else
	{
		dword bins[2] = { 0, 1 };
		long volumes[2] = { 0x8000, 0x8000 };

		function_3e2630(g_4e9188.movie, 0, bins, 2);
		function_3e2680(g_4e9188.movie, 0, bins, volumes, 2);
	}

	s_bink_movie *movie = (s_bink_movie *)g_4e9188.movie;
	long texture_size;
	void *texture_data;

	g_4e9188.width = (short)movie->width;
	g_4e9188.height = (short)movie->height;
	g_4e9188.copy_flags = 3;
	g_4e9188.texture = function_23e340(g_4e9188.width, g_4e9188.height, 10, function_156620, &texture_size, &texture_data,
		(struct D3DTexture *)g_4e9188.texture_header);
	if (!g_4e9188.texture)
	{
		function_1565e0();
		return;
	}

	function_155e50();
	XPhysicalProtect(texture_data, texture_size, PAGE_READWRITE | PAGE_WRITECOMBINE);
	function_155e50();
	if (!g_4e9188.texture)
	{
		function_1565e0();
		return;
	}

	memset(g_4e9188.material, 0, sizeof(g_4e9188.material));
	((real *)g_4e9188.material)[0x10] = 1.0f;
	((real *)g_4e9188.material)[0x11] = 1.0f;
	((real *)g_4e9188.material)[0xa] = 1.0f;
	((real *)g_4e9188.material)[0xb] = 1.0f;
	((long *)g_4e9188.material)[0] = 0;
	g_4e9188.material[0x96] = 0;
	*(short *)&g_4e9188.material[0x94] = 7;

	g_55ece8.signature = 'bitm';
	g_55ece8.width = g_4e9188.width;
	g_55ece8.height = g_4e9188.height;
	g_55ece8.depth = 1;
	g_55ece8.unknown0a = 0;
	g_55ece8.format = 10;
	g_55ece8.flags = 0x10;
	g_55ece8.size = function_1358c0(10) * g_4e9188.height * g_4e9188.width / 8;
	g_55ece8.unknown4c = NONE;
	g_55ece8.unknown28 = NONE;
	g_55ece8.unknown54 = NONE;
	g_55ece8.texture = g_4e9188.texture;
	*(s_bink_bitmap **)&g_4e9188.material[0xc] = &g_55ece8;
	g_4e9188.flags = flags;
	if (flags & 4)
	{
		function_155e50();
		function_155e50();
	}
	function_1568d0();
	g_4e6389 = 0;
	g_4e9188.flag1 = true;
	g_4e6388 = 1;
}
