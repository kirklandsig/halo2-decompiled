// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_146A20.CPP: the memory Havok allocates from. One block of virtual
   memory (g_479888) holds the allocators, built in place: a fixed buffer, a
   Havok pool, and two fixed buffers made when the pool runs short, one in
   the ai's scratch buffers (g_47989c) and one in physical memory
   (g_4798a0). The game's own hkMemory (vtable 0x45378c, 0x147110 and on)
   allocates from them. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_146a20.h"
#include "physical_memory.h"
#include "unknown_223b60.h"
#include <xtl.h>
#include <new.h>

/* the texture cache's physical memory (unknown_12c0d0.cpp) */
extern s_physical_object *g_4e6464;

long __stdcall function_12d2f0(long a, long b, long c, long d);
void function_12c600(void);
void function_12d520(long a);
double timing_ticks_to_seconds(__int64 ticks);

/* the havok system (unknown_1c25a0.cpp and later) */
void function_1c27a0(void);
void function_1c2690(void);
void __stdcall function_1c4590(long unknown);
bool function_1c4040(long attempt, bool active, bool any_object, bool even_if_unknown, long excluded_component_index);

/* the block the allocators live in (unknown_1c25a0.cpp) */
extern void *g_479888;

/* the ai's scratch buffers (ai.cpp) */
extern byte *g_510c44;
extern bool g_510c48;

/* the fixed buffer in physical memory (unknown_147090.cpp) */
extern c_havok_fixed_memory *g_4798a0;

#define HAVOK_SCRATCH_MEMORY_SIZE 0x452e8

#define MAX(a, b) ((a) > (b) ? (a) : (b))

void function_146b80(void);
void function_146da0(void);
void function_146de0(void);
long function_146fd0(void);

bool g_479880;
long g_479884;
long g_47988c;
long g_479890;
hkMemory *g_479894;
hkMemory *g_479898;
long g_4798b4;
bool g_47f05a;
bool g_47f05b;

static __int64 read_tsc(void)
{
	volatile __int64 t = 0;
	__asm rdtsc
}

// @retail 0x146a20
void function_146a20(void)
{
	g_479884 = NONE;
	g_479890 = 1;
	g_479880 = true;
	g_479894 = new (g_479888) c_havok_fixed_memory((byte *)g_479888 + 0x30, 0x15000);
	g_479898 = new ((byte *)g_479888 + 0x15040) hkPoolMemory;
	g_4798b4++;
}

// @retail 0x146ac0
void function_146ac0(void)
{
	if (g_47989c)
	{
		function_146de0();
	}
	if (g_4798a0)
	{
		((hkMemory *)g_4798a0)->~hkMemory();
		g_4798a0 = NULL;
		function_12d520(g_47988c);
		g_47988c = 0;
	}
	g_479894->~hkMemory();
	g_479894 = NULL;
	g_479898->~hkMemory();
	g_479898 = NULL;
	g_479890 = 0;
	g_479880 = false;
}

/* a page aligned block of virtual memory: its size and the offset back to
   the allocation sit before it */
static void *virtual_memory_allocate_aligned(long size)
{
	void *result = NULL;
	byte *block = (byte *)VirtualAlloc(result, size + 0x1000, MEM_COMMIT | MEM_TOP_DOWN, PAGE_READWRITE);

	if (!block)
	{
		GetLastError();
	}
	else
	{
		byte *aligned = (byte *)(((dword)block + 0x1000) & ~0xfff);

		((long *)aligned)[-2] = size;
		((long *)aligned)[-1] = aligned - block;
		result = aligned;
	}
	return result;
}

// @retail 0x146b30
void function_146b30(void)
{
	g_479888 = virtual_memory_allocate_aligned(0x105470);
}

// @retail 0x146b80
void function_146b80(void)
{
	byte *buffer = g_510c44;
	long size = HAVOK_SCRATCH_MEMORY_SIZE;

	g_510c48 = true;
	if ((dword)buffer & 0xf)
	{
		long adjustment = -(long)buffer & 0xf;

		buffer += adjustment;
		size -= adjustment;
	}
	g_47989c = new ((byte *)g_479888 + 0x105410) c_havok_fixed_memory(buffer, size);
}

// @retail 0x146bf0
void function_146bf0(void)
{
	if (!g_47f05b)
	{
		long level = function_146fd0();
		long attempt;

		g_47f05b = true;
		for (attempt = 0; attempt < 3 && level >= 2; attempt++)
		{
			long count = 0;
			long pass;

			if (level >= 3)
			{
				function_1c4590(1);
			}
			level = function_146fd0();
			for (pass = 0; pass < 3 && level >= 2; pass++)
			{
				long tries = 0;

				do
				{
					bool recent = g_479884 != NONE && g_510c54->game_time - g_479884 < 10;

					if (!function_1c4040(attempt, pass == 2, pass > 1, recent, NONE))
					{
						if (!attempt && !recent)
						{
							function_146da0();
							level = function_146fd0();
							g_479884 = g_510c54->game_time;
						}
						break;
					}
					level = function_146fd0();
					if (level < 2)
					{
						break;
					}
					tries++;
					if (++count % 8 == 0)
					{
						function_146da0();
						level = function_146fd0();
						g_479884 = g_510c54->game_time;
					}
				}
				while (tries <= 50);
			}
		}
		g_47f05b = false;
	}
}

// @retail 0x146da0
void function_146da0(void)
{
	bool scratch = g_47989c != NULL;

	function_1c27a0();
	g_47f05a = true;
	function_1c2690();
	g_47f05a = false;
	if (scratch && !g_47989c)
	{
		function_146b80();
	}
}

// @retail 0x146de0
void function_146de0(void)
{
	if (g_47989c)
	{
		((hkMemory *)g_47989c)->~hkMemory();
		g_47989c = NULL;
		g_510c48 = false;
	}
	if (g_4798a0)
	{
		((hkMemory *)g_4798a0)->~hkMemory();
		g_4798a0 = NULL;
		function_12d520(g_47988c);
		g_47988c = 0;
	}
}

// @retail 0x146e30
hkMemory *function_146e30(void)
{
	if (!g_4798a0)
	{
		long volatile size = 0x800000;
		byte *block;
		long adjustment;

		do
		{
			__int64 start = read_tsc();

			block = NULL;
			if (size > 0 && g_4e6464->page_count > 0)
			{
				long retries = 0;

				for (;;)
				{
					block = (byte *)function_12d2f0(size, 0, 0, 0);
					if (block)
					{
						break;
					}
					if (retries < 90)
					{
						retries++;
						function_12c600();
						continue;
					}

					__int64 elapsed = read_tsc() - start;
					if (elapsed < 0)
						elapsed = 0;
					if (!((real)timing_ticks_to_seconds(elapsed) < 1.0f))
						break;
					D3DDevice_KickPushBuffer();
					D3DDevice_IsBusy();
					SwitchToThread();
				}
			}
			g_47988c = (long)block;
		}
		while (!block && ((size = size / 2) >= 0x100000));

		adjustment = (((dword)block + 0xf) & ~0xf) - (dword)block;
		g_4798a0 = new ((byte *)g_479888 + 0x105440) c_havok_fixed_memory(block, size - adjustment);
		function_1c4590(1);
	}
	return g_4798a0;
}

// @retail 0x146fd0
long function_146fd0(void)
{
	long physical_level = 0;
	long scratch_level = 0;
	long pool_level;

	if (g_4798a0 && g_4798a0->m_count > 0)
	{
		physical_level = 3;
	}
	if (g_47989c && g_47989c->m_count > 0)
	{
		scratch_level = 3;
	}
	if (g_479898)
	{
		hkPoolMemory *pool = (hkPoolMemory *)g_479898;

		pool_level = 3;
		if ((real)pool->page_count * (1.0f / 118.0f) < 0.8f)
		{
			real used = pool->get_used_fraction();

			if (used < 0.85f)
			{
				pool_level = 2;
				if (used < 0.75f)
				{
					pool_level = 1;
					if (used < 0.65f)
					{
						pool_level = 0;
					}
				}
			}
		}
	}
	else
	{
		pool_level = 0;
	}
	return MAX(scratch_level, MAX(pool_level, physical_level));
}
