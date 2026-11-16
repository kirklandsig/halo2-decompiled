// @flags /O2 /GL-
/* UNKNOWN_22C720.CPP: Havok's pool memory (c_havok_page_pool_allocator,
   vtable 0x457620; unknown_146a20.h). Blocks up to 0x2000 bytes come from
   per-size-class free lists, refilled from 0x2000-byte pages; each block
   handed out by allocate or allocate_aligned carries a 16-byte header */

#include "unknown_11c920.h"
#include "unknown_146a20.h"
#include "stream_holder_2d8e70.h"

/* frees a block too large for the pool (holds 0x2d6fb0) */
void __cdecl function_2d6fb0(void *address);
void (__cdecl *g_461ddc)(void *address) = function_2d6fb0;

/* the pool keeps no statistics: says so on the stream */
// @retail 0x22c720
void c_havok_page_pool_allocator::method6(long unknown)
{
	((stream_holder_2d8e70 *)unknown)->function_2d9000(
		"Statistics are disabled, please enable them in hkbase/config/hkConfigMemoryStats.h\n");
}

/* a block with a header: its size, its memory class and a marker */
// @retail 0x22c740
void *c_havok_page_pool_allocator::allocate(long size, long memory_class)
{
	long *block = (long *)allocate16(size + 16, memory_class);
	if (block)
	{
		block[1] = size;
		block[2] = memory_class;
		block[0] = 0x2345656;
		return block + 4;
	}
	return 0;
}

// @retail 0x22c780
void c_havok_page_pool_allocator::deallocate(void *address)
{
	long *header = (long *)address - 4;
	header[0] = 0xdeadbeef;
	deallocate16(header, header[1] + 16, header[2]);
}

/* as allocate, aligned; the header's last word is the block's offset */
// @retail 0x22c7b0
void *c_havok_page_pool_allocator::allocate_aligned(long alignment, long size, long memory_class)
{
	long total = alignment + size;
	char *block = (char *)allocate16(total + 16, memory_class);
	if (block)
	{
		long *aligned = (long *)((unsigned long)(block + alignment + 15) & ~(alignment - 1));
		aligned[-3] = total;
		aligned[-2] = memory_class;
		aligned[-4] = 0x2345656;
		aligned[-1] = (char *)aligned - block;
		return aligned;
	}
	return 0;
}

// @retail 0x22c800
void c_havok_page_pool_allocator::deallocate_aligned(void *address)
{
	long *header = (long *)address;
	header[-4] = 0xdeadbeef;
	deallocate16((char *)address - header[-1], header[-3] + 16, header[-2]);
}

/* takes a block from its size class's free list; an empty list gets the
   rest of the current page (up to 0x100 bytes of blocks) or, for a large
   class, a whole new page */
// a block of a size class whose free list is empty: cut from the current
// page (the rest of a small class's share going to its free list) or from a
// fresh page; none when the pages are used up
static __forceinline void *refill_small_22c830(c_havok_page_pool_allocator *pool, long size_class)
{
	long class_size;
	char *result;
	long in_use;
	class_size = pool->class_sizes[size_class];
	long offset;
	offset = class_size;
	if ((unsigned long)(pool->unknown20 + class_size) > (unsigned long)pool->unknown1c)
	{
		if (pool->page_count >= 0x76)
		{
			return 0;
		}
		unsigned long page = (long)&pool->pages[0x4010 + (pool->page_count << 13)];
		pool->unknown18 = page;
		pool->unknown20 = page;
		++pool->page_count;
		long page_limit = page + 0x1fc0;
		pool->unknown1c = page_limit;
		pool->unknown2d0[0]++;
	}
	result = (char *)pool->unknown20;
	pool->unknown20 = pool->unknown20 + class_size;
	in_use = pool->unknown2e8[size_class].first;
	for (; 0x100 > offset; offset += class_size)
	{
		char *block = (char *)pool->unknown20;
		if ((unsigned long)((long)block + class_size) >= (unsigned long)pool->unknown1c)
		{
			break;
		}
		pool->unknownf0344[size_class]++;
		pool->unknownf0388[size_class]++;
		pool->unknown2e8[size_class].first--;
		*(long *)block = pool->unknown24[size_class];
		pool->unknown24[size_class] = (long)block;
		pool->unknown20 += class_size;
	}
	pool->unknown2e8[size_class].first = in_use;
	pool->unknownf0344[size_class]++;
	return result;
}

static __forceinline void *refill_large_22c830(c_havok_page_pool_allocator *pool, long size_class)
{
	long class_size;
	char *result;
	long in_use;
	class_size = pool->class_sizes[size_class];
	if (pool->page_count < 0x76)
	{
		result = (char *)&pool->pages[0x4010 + (pool->page_count << 13)];
		++pool->page_count;
		pool->unknown2d0[0]++;
		in_use = pool->unknown2e8[size_class].first;
		char *block = result + class_size;
		for (long offset = class_size; offset < 0x2000; offset += class_size, block += class_size)
		{
			pool->unknownf0344[size_class]++;
			pool->unknownf0388[size_class]++;
			pool->unknown2e8[size_class].first--;
			*(long *)block = pool->unknown24[size_class];
			pool->unknown24[size_class] = (long)block;
		}
		pool->unknown2e8[size_class].first = in_use;
		pool->unknownf0344[size_class]++;
		return result;
	}
	return 0;
}

// score 0 (was 78): matched with the small and the large refill each written as its own inline
// helper over the pool (retail places the large one after the shared exit and
// returns zero through the result register).
// Before, 78 (was 114): retail keeps the page count and the class size in ebx
// where ours keeps them in edi, and does the size compare before the frame
// is set up; ours still reads the class size into a local of its own in each
// branch
// @retail 0x22c830
void *c_havok_page_pool_allocator::allocate16(long size, long memory_class)
{
	if (0x2000 >= size)
	{
		long size_class;
		if (0x200 >= size)
		{
			size_class = (signed char)small_classes[size];
		}
		else
		{
			size_class = large_classes[(size - 1) >> 10];
		}
		long *free_block = (long *)unknown24[size_class];
		unknown2e8[size_class].first++;
		if (free_block)
		{
			unknown24[size_class] = *free_block;
			unknownf0388[size_class]--;
			return free_block;
		}
		return 0x200 >= size ? refill_small_22c830(this, size_class) : refill_large_22c830(this, size_class);
	}
	return 0;
}
/* returns a block to its size class's free list */
// @retail 0x22ca00
void c_havok_page_pool_allocator::deallocate16(void *address, long size, long memory_class)
{
	if (size <= 0x2000)
	{
		long size_class;
		if (size <= 0x200)
		{
			size_class = (signed char)small_classes[size];
		}
		else
		{
			size_class = large_classes[(size - 1) >> 10];
		}
		unknownf0388[size_class]++;
		unknown2e8[size_class].first--;
		*(long *)address = unknown24[size_class];
		unknown24[size_class] = (long)address;
	}
	else
	{
		g_461ddc(address);
	}
}

/* fills four longs: the pages' size, the bytes in use, and two counters */
// @retail 0x22ca70
void c_havok_page_pool_allocator::method7(long unknown)
{
	long *statistics = (long *)unknown;
	statistics[2] = unknown2d0[2];
	statistics[3] = unknown2d0[3];
	long used = 0;
	for (long i = 1; i < 17; i++)
	{
		used += unknown2e8[i].first * class_sizes[i];
	}
	statistics[0] = (page_size + unknown2e4) * unknown2d0[0];
	statistics[1] = used;
}
