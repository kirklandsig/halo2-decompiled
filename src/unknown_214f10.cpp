// @flags /O2 /Gr
/* UNKNOWN_214F10.CPP: the game state's memory and its two copies on the
   utility drive: unknown_123b30.cpp allocates the memory here, and saves it to
   (and reads it back from) the cache file of one of two slots, through a
   block of the texture cache when one is free. Decompiled by lane L for
   unknown_123b30.cpp (0x123b30..0x123e20). */

#include "unknown_11c920.h"
#include "async.h"
#include "unknown_12b400.h"
#include <xtl.h>
#include <string.h>

char *function_11c9c0(char *buffer, long size, const char *format, ...);
long function_12d400(long type, long size, long user_data, long update, long release);
void function_12d520(long address);
void function_125d60(void);

/* the name of a slot's cache file */
char const *g_470050;

bool g_5020d8;
dword g_5020dc;
long g_5020e0;
long g_5020e4;
bool volatile g_5020e8;
dword g_5020ec;
short g_5020f0;
bool g_5020f2[2];
bool g_5020f4;
s_file_handle g_5020f8[2];

// @retail 0x214f10
void *game_state_cache_allocate(long cpu_size, long gpu_size)
{
	long size = cpu_size + gpu_size;
	void *memory = physical_memory_malloc_fixed(size, PAGE_READWRITE);

	g_5020dc = (dword)memory;
	g_5020e0 = size;
	g_5020d8 = true;
	return memory;
}

// @retail 0x214f80
void game_state_cache_files_open(void)
{
	for (short i = 0; i < 2; i++)
	{
		char path[200];
		bool volatile done;
		bool success;

		function_11c9c0(path, sizeof(path), g_470050, i);
		function_1a0b40(path, 3, 3, 0, 8, 6, &g_5020f8[i], &done);
		if (!done)
		{
			while (!done)
			{
				SwitchToThread();
			}
		}
		s_file_handle file = g_5020f8[i];

		success = file.handle != INVALID_HANDLE_VALUE;
		if (success)
		{
			function_1a1310(file, 0x40b000, 8, 6, &success, &done);
			if (!done)
			{
				while (!done)
				{
					SwitchToThread();
				}
			}
		}
	}
	g_5020f4 = true;
}

// @retail 0x2150f0
void game_state_cache_files_close(void)
{
	for (short i = 0; i < 2; i++)
	{
		bool volatile done;

		function_1a1550(g_5020f8[i], 8, 6, &done);
		if (!done)
		{
			while (!done)
			{
				SwitchToThread();
			}
		}
	}
	g_5020f4 = false;
}

/* the write from the texture cache block has finished: give the block back */
static __forceinline void game_state_cache_write_finish(void)
{
	g_5020f2[g_5020f0] = g_5020ec == g_5020e0;
	function_12d520(g_5020e4);
	g_5020e4 = 0;
}

// @retail 0x215140
void __stdcall game_state_cache_lock_update(void *address, long user_data)
{
	if (g_5020e8)
	{
		game_state_cache_write_finish();
	}
}

#pragma optimize("g", off)
// @retail 0x215180
void __stdcall game_state_cache_lock_release(void *address, long user_data)
{
	while (g_5020e4)
	{
		if (g_5020e8)
		{
			game_state_cache_write_finish();
		}
		else
		{
			SwitchToThread();
		}
	}
}

#pragma optimize("g", on)

// @retail 0x2151f0
void game_state_cache_write(short slot)
{
	void const *source;
	long priority;

	if (g_5020e4)
	{
		game_state_cache_lock_release((void *)g_5020e4, 0);
	}
	g_5020f2[slot] = false;
	g_5020e4 = function_12d400(0, g_5020e0, 0, (long)game_state_cache_lock_update, (long)game_state_cache_lock_release);
	g_5020f0 = slot;
	source = (void const *)g_5020dc;
	priority = 3;
	if (g_5020e4)
	{
		memcpy((void *)g_5020e4, (void const *)g_5020dc, g_5020e0);
		source = (void const *)g_5020e4;
		priority = 2;
	}
	function_1a1050(g_5020f8[slot], source, g_5020e0, 0, 0, 8, priority, &g_5020ec, &g_5020e8);
	if (!g_5020e4)
	{
		if (!g_5020e8)
		{
			while (!g_5020e8)
			{
				SwitchToThread();
				function_125d60();
			}
		}
		g_5020f2[slot] = g_5020ec == g_5020e0;
	}
}

// @retail 0x2152e0
bool game_state_cache_read(short slot)
{
	bool result = false;

	if (g_5020e4)
	{
		game_state_cache_lock_release((void *)g_5020e4, 0);
	}
	if (g_5020f2[slot])
	{
		dword bytes_read;
		bool volatile done;

		function_1a0f10(g_5020f8[slot], (void *)g_5020dc, g_5020e0, 0, 8, 6, &bytes_read, &done);
		if (!done)
		{
			while (!done)
			{
				SwitchToThread();
			}
		}
		result = bytes_read == g_5020e0;
	}
	return result;
}
