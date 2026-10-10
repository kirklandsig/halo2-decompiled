// @flags /O2 /Ob1 /arch:SSE /Gr
/* LOOP_ALLOCATOR.CPP: the "loop" allocators (the voice pool and the ui
   memory pool): a header, then a pool of blocks kept in address order,
   allocated at the end, at the start or in a gap, and compacted on demand */

#include "unknown_11c920.h"
#include "globals.h"
#include "loop_allocator.h"
#include <xtl.h>
#include <string.h>

#define LOOP_SIGNATURE 0x706f6f6c
#define LOOP_BLOCK_SIGNATURE 0x68656164

static inline byte *loop_block_get_address(s_loop_allocator *loop, s_loop_block *block)
{
	byte *address = (byte *)block;

	if (loop->debug_headers)
	{
		address -= sizeof(s_loop_block_debug_header);
	}
	return address;
}

static inline long loop_block_header_size(s_loop_allocator *loop)
{
	long size = sizeof(s_loop_block);

	if (loop->debug_headers)
	{
		size = sizeof(s_loop_block) + sizeof(s_loop_block_debug_header);
	}
	return size;
}

static inline char *function_x91aa57(char *destination, char const *source, dword size)
{
	strncpy(destination, source, size);
	destination[size - 1] = 0;
	return destination;
}

// @retail 0x18eea0
bool function_18eea0(long stage)
{
	if (stage > 1 && stage <= 3)
	{
		return true;
	}
	return false;
}

// @retail 0x18e250
void function_18e250(s_loop_allocator *loop, long size, const char *name, c_memory_source *source)
{
	memset(loop, 0, sizeof(*loop));
	loop->signature = LOOP_SIGNATURE;
	function_x91aa57(loop->name, name, sizeof(loop->name));
	loop->source = source;
	loop->base = (byte *)(((dword)loop + sizeof(*loop) + 15) & ~15);
	loop->size = size;
	loop->free = size;
	loop->first = NULL;
	loop->last = NULL;
	loop->field3c = false;
	loop->field3d = false;
	loop->debug_headers = false;
}

// @retail 0x18e1f0
s_loop_allocator *function_18e1f0(c_memory_source *source, long size, const char *name)
{

	if (!source)
	{
		source = (c_memory_source *)g_468758;
	}

	long allocation_size = size + 0x50;
	s_loop_allocator *loop = (s_loop_allocator *)source->allocate(allocation_size);
	if (loop)
	{
		function_18e250(loop, size, name, source);
	}
	return loop;
}

// @retail 0x18e230
void function_18e230(s_loop_allocator *loop)
{
	c_memory_source *source = loop->source;

	memset(loop, 0, sizeof(*loop));
	source->release(loop);
}

// @retail 0x18e4f0
inline s_loop_block *loop_block_insert(s_loop_allocator *loop, byte *address, long size, char const *file, long line, s_loop_block *previous, s_loop_block *next)
{
	s_loop_block *block = (s_loop_block *)(loop->debug_headers ? address + sizeof(s_loop_block_debug_header) : address);
	s_loop_block_debug_header *header = loop->debug_headers ? (s_loop_block_debug_header *)block - 1 : NULL;

	if (header)
	{
		header->signature = LOOP_BLOCK_SIGNATURE;
		header->file = file;
		header->line = line;
		header->time = GetTickCount();
	}

	block->size = size;
	block->next = next;
	block->previous = previous;
	if (!previous)
	{
		loop->first = block;
	}
	else
	{
		previous->next = block;
	}
	if (!next)
	{
		loop->last = block;
	}
	else
	{
		next->previous = block;
	}
	return block;
}

// @retail 0x18e560
s_loop_block *loop_allocate_at_end(s_loop_allocator *loop, long size, char const *file, long line)
{
	s_loop_block *last = loop->last;
	s_loop_block *result = NULL;
	byte *address;

	if (!last)
	{
		address = loop->base;
	}
	else
	{
		address = loop_block_get_address(loop, last) + last->size;
	}

	if (address + size <= loop->base + loop->size)
	{
		result = loop_block_insert(loop, address, size, file, line, last, NULL);
	}
	return result;
}

// @retail 0x18e600
s_loop_block *loop_allocate_at_start(s_loop_allocator *loop, long size, char const *file, long line)
{
	s_loop_block *result = NULL;
	byte *end;

	if (!loop->first)
	{
		dword tmp0 = loop->size;
		end = loop->base + tmp0;
	}
	else
	{
		end = (byte *)loop->first;
		if (loop->debug_headers)
		{
			end -= sizeof(s_loop_block_debug_header);
		}
	}

	byte *address = loop->base;
	if (address + size <= end)
	{
		result = loop_block_insert(loop, address, size, file, line, NULL, loop->first);
	}
	return result;
}

// @retail 0x18e690
s_loop_block *loop_allocate_in_gap(s_loop_allocator *loop, long size, char const *file, long line)
{
	s_loop_allocator *const *loop_reference = &loop;
	s_loop_block *block = (*loop_reference)->first;
	s_loop_block *local_0 = NULL;
	s_loop_block *next = NULL;

	if (block)
	{
		do
		{
			next = block->next;
			if (next)
			{
				byte *next_address = loop_block_get_address(loop, next);
				byte *address = loop_block_get_address(loop, block) + block->size;
				if (address + size <= next_address)
				{
					local_0 = loop_block_insert(loop, address, size, file, line, block, next);
				goto local_1;
				}
			}
			block = next;
		} while (next);
		goto local_1;
	}
	local_1:
	return local_0;
}

// @retail 0x18e2b0
bool loop_allocate(s_loop_allocator *loop, void **pointer, long size, char const *file, long line)
{
	long block_size = loop_block_header_size(loop) + size;
	s_loop_block *block;

	if (block_size & 15)
	{
		block_size = (block_size | 15) + 1;
	}

	if (loop->field3d)
	{
		block = loop_allocate_in_gap(loop, block_size, file, line);
		if (!block)
		{
			block = loop_allocate_at_start(loop, block_size, file, line);
		}
		if (!block)
		{
			block = loop_allocate_at_end(loop, block_size, file, line);
		}
	}
	else
	{
		block = loop_allocate_at_end(loop, block_size, file, line);
	}

	if (block)
	{
		block->owner = loop->field3c ? NULL : pointer;
		loop->free -= block->size;
		*pointer = block + 1;
		return true;
	}
	return false;
}

// @retail 0x18e430
void loop_free(s_loop_allocator *loop, void **pointer)
{
	s_loop_block *block = (s_loop_block *)*pointer - 1;

	loop->free += block->size;
	if (block->previous)
	{
		block->previous->next = block->next;
	}
	else
	{
		loop->first = block->next;
	}
	if (block->next)
	{
		block->next->previous = block->previous;
	}
	else
	{
		loop->last = block->previous;
	}
}

// @retail 0x18e340
bool loop_reallocate(s_loop_allocator *loop, void **pointer, long size, char const *file, long line)
{
	bool local_0 = false;
	s_loop_allocator *const *local_2 = &loop;
	s_loop_block *block = (s_loop_block *)*pointer - 1;
	long block_size = loop_block_header_size(loop) + size;

	if (block_size & 15)
	{
		block_size = (block_size | 15) + 1;
	}

	byte *end = (byte *)block->next;
	if (!end)
	{
		end = loop->base + loop->size;
	}

	if ((byte *)block + block_size <= end)
	{
		loop->free += block->size - block_size;
		block->size = block_size;
		if (loop->debug_headers)
		{
			s_loop_block_debug_header *header = (s_loop_block_debug_header *)block - 1;
			if (header)
			{
				header->file = file;
				header->line = line;
				header->time = GetTickCount();
			}
		}
		local_0 = true;
		goto local_1;
	}

	void *volatile *local_3 = (void *volatile *)&file;
	if (loop_allocate(loop, (void **)local_3, size, file, line))
	{
		void *const volatile *local_4 = pointer;
		memcpy((*local_3), *local_4, block->size - loop_block_header_size(loop));
		loop_free(loop, pointer);
		((s_loop_block *)(*local_3) - 1)->owner = (*local_2)->field3c ? NULL : pointer;
		*pointer = (*local_3);
		local_0 = true;
	}
	local_1:
	return local_0;
}

// @retail 0x18e470
void loop_compact(s_loop_allocator *loop)
{
	if (!loop->field3c && loop->first)
	{
		byte *address = loop->base;
		s_loop_block *previous = NULL;
		s_loop_block *block = loop->first;

		do
		{
			byte *block_address = loop_block_get_address(loop, block);
			if (block_address > address)
			{
				memmove(address, block_address, block->size);
				block_address = address;
				block = (s_loop_block *)address;
				if (loop->debug_headers)
				{
					block = (s_loop_block *)(address + sizeof(s_loop_block_debug_header));
				}
				if (loop->field3c)
				{
					block->owner = NULL;
				}
				else
				{
					*block->owner = block + 1;
				}
			}
			block->previous = previous;
			if (previous)
			{
				previous->next = block;
			}
			else
			{
				loop->first = block;
			}
			address = block_address + block->size;
			previous = block;
			block = block->next;
		} while (block);

		previous->next = block;
		loop->last = previous;
	}
}
