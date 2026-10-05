// @flags /O2 /Gr
/* UNKNOWN_218850.CPP: the sound cache: requesting, locking and loading the
   cache pages that hold sound data chunks */

#include "unknown_11c920.h"
#include <xtl.h>
#include "unknown_218850.h"
#include "physical_memory.h"
#include "unknown_12b400.h"
#include "data_array.h"
#include "async.h"

s_record_pool *g_502104;
dword g_502108;
s_sound_cache_allocator *g_50210c;

/* the cache page allocator is a physical memory allocator (function_13d370 is
   lane F's stub, src/stubs/lane_f.cpp) */
long function_213760(dword location, long size, void *buffer, dword *bytes_read, bool *done, long type, long priority);

void function_218a10(s_sound_chunk *chunk, long owner);

static inline void sound_cache_page_touch(s_sound_cache_allocator *allocator, long index)
{
	((s_sound_cache_page *)allocator->pages->data)[index & 0xffff].last_used = allocator->time;
}

/* the sound cache request (sound_cache_request_part.cpp): flags bit 0 blocks until
   the chunk is loaded, bit 1 starts loading it, bit 2 locks it. Returns bit 1
   when loaded, bit 2 when locked, bit 0 while still loading.
   Standard convention (see docs/DECOMPILING.md):
   1. Retail keeps it __stdcall (all three arguments on the stack, ret 0xc).
      With the marker this body matches byte for byte; without it LTCG moves
      the arguments into registers.
   2. No data or code in retail holds its address. Its seven callers
      (0x125e60 0x125f10 0x1268e0 0x129f20 0x12a450 0x16ea60 0x2ae500) are
      all LTCG code (each has register conventions of its own) and push all
      three arguments.
   3. Tried: taking each parameter's address keeps the arguments on the stack
      but changes the body (77 differences); /GL- on the file breaks the
      register convention of 0x218a10; declaring it __stdcall alone does
      nothing under LTCG. */
// @retail 0x218850 standard
dword __stdcall function_218850(long owner, s_sound_chunk *sound, dword flags)
{
	dword result = 0;
	bool block = (flags & 1) != 0;
	bool load = ((flags >> 1) & 1) != 0;
	bool lock = ((flags >> 2) & 1) != 0;

	if (sound->cache_index == NONE && owner != NONE && load)
	{
		function_218a10(sound, owner);
	}

	if (sound->cache_index != NONE)
	{
		sound_cache_page_touch(g_50210c, sound->cache_index);
		s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(sound->cache_index);

		if (block)
		{
			if (!entry->loaded)
			{
				while (!entry->loaded)
				{
					SwitchToThread();
				}
			}
		}

		if (entry->loaded)
		{
			if (!entry->used)
			{
				entry->used = 1;
			}
			result = 2;
		}

		if (lock)
		{
			entry->lock_count++;
			result |= 4;
		}
	}

	if (!(result & 2) && sound->cache_index != NONE)
	{
		result |= 1;
	}
	else
	{
		result &= ~1;
	}

	return result;
}

// @retail 0x218950
byte *sound_cache_chunk_get_data(s_sound_chunk *chunk)
{
	s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(chunk->cache_index);

	if (entry->reference_count < 0xff)
	{
		entry->reference_count++;
	}

	return (byte *)((SOUND_CACHE_PAGE(chunk->cache_index)->offset << g_50210c->page_shift) + g_502108);
}

// @retail 0x218a10
void function_218a10(s_sound_chunk *chunk, long owner)
{
	long index = function_13d370((s_physical_object *)g_50210c, SOUND_CHUNK_SIZE(chunk), 0);

	if (index != NONE)
	{
		byte *buffer = (byte *)((SOUND_CACHE_PAGE(index)->offset << g_50210c->page_shift) + g_502108);
		datum_new_at_index_with_salt(g_502104, index);
		s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(index);
		chunk->cache_index = index;
		entry->owner = owner;
		entry->chunk = chunk;
		entry->lock_count = 0;
		entry->reference_count = 0;
		entry->used = 0;

		dword size = SOUND_CHUNK_SIZE(chunk);
		if (size & 0x1ff)
		{
			size = (size | 0x1ff) + 1;
		}
		function_213760(chunk->file_offset, size, buffer, NULL, (bool *)&entry->loaded, 5, 4);
	}
}

/* ---- the cache's life: its entries (g_502104) and page allocator (g_50210c) ---- */

extern c_data_allocator *g_468758;
long function_120bf0(void);

PRIVATE void __stdcall sound_cache_entry_delete(long entry_index);
PRIVATE bool __stdcall sound_cache_entry_busy(long entry_index);

static inline s_physical_object *sound_cache_pages(void)
{
	return (s_physical_object *)g_50210c;
}

// @retail 0x2185d0
void sound_cache_initialize(void)
{
	s_physical_object *physical;

	g_502104 = data_new_inlined("xbox sound", 0x200, sizeof(s_sound_cache_entry), 0, g_468758);
	physical = physical_memory_new("xbox sound cache", 0xc0, 0xe, 0x200,
		sound_cache_entry_delete, sound_cache_entry_busy, NULL, g_468758);
	physical->state = 2;
	g_50210c = (s_sound_cache_allocator *)physical;
	g_502108 = (dword)physical_memory_malloc_fixed(0x300000, PAGE_READWRITE);
}

/* 0x2186b0, the cache's dispose, is in src/sound_cache_dispose.cpp (/Ob1) */

// @retail 0x2186f0
void function_2186f0(void)
{
	dword start_time = GetTickCount();
	s_record_pool_iterator iterator;
	s_sound_cache_entry *entry;

	iterator.data = g_502104;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while ((entry = (s_sound_cache_entry *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		while (SOUND_CACHE_ENTRY(iterator.datum_index)->lock_count ||
			SOUND_CACHE_ENTRY(iterator.datum_index)->reference_count ||
			!SOUND_CACHE_ENTRY(iterator.datum_index)->loaded)
		{
			async_globals.tasks_added = function_120bf0();
			if (GetTickCount() - start_time >= 5000)
			{
				break;
			}
		}

		long cache_index = entry->chunk->cache_index;
		if (cache_index != NONE)
		{
			sound_cache_pages()->block_delete(cache_index);
		}
	}
	g_502104->valid = false;
}

// @retail 0x218810
void sound_cache_new_frame(void)
{
	physical_memory_new_frame(sound_cache_pages());
}

// @retail 0x2189a0
PRIVATE bool __stdcall sound_cache_entry_busy(long entry_index)
{
	s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(entry_index);
	bool busy = false;

	if (entry->lock_count || entry->reference_count || !entry->loaded)
	{
		busy = true;
	}
	return busy;
}

// @retail 0x2189e0
PRIVATE void __stdcall sound_cache_entry_delete(long entry_index)
{
	SOUND_CACHE_ENTRY(entry_index)->chunk->cache_index = NONE;
	record_pool_release(g_502104, entry_index);
}
