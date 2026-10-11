// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_12C0D0.CPP: the texture cache: a data array of entries (one per
   bitmap level in memory) whose memory is a block of the physical memory
   allocator g_4e6464, a data array of predicted bitmaps waiting to be loaded,
   and the cache's scale, raised and lowered with how full it is. Its memory
   is set up in unknown_12d9f0.cpp's neighbour 0x12c1e0. */

#include "unknown_11c920.h"
#include "data_array.h"
#include "globals.h"
#include "physical_memory.h"
#include "unknown_12b400.h"
#include "async.h"
#include <xtl.h>
#include <string.h>

/* the part of a bitmap's data block the cache keeps track of */
struct s_bitmap_data
{
	byte unknown00[0xa];
	short type;
	byte unknown0c[2];
	word flags;
	byte unknown10[6];
	byte level_bias;
	byte cache_format;
	byte unknown18[4];
	long data_offsets[3];
	long block_indices[3];
	long field_34[3];
	long unknown40[4];
	D3DTexture *texture;
	long unknown54;
	real minimum_scale;
	/* the hardware texture header, built in place over the bitmap's
	   description of it (an offset into the shared pixel data, its format
	   and its size) */
	long hardware_common;
	dword hardware_data;
	long hardware_lock;
	union
	{
		dword hardware_format;
		struct
		{
			short format;
			short width;
		};
	};
	union
	{
		dword hardware_size;
		struct
		{
			short height;
			short depth;
		};
	};
	long unknown70;
};

/* an entry of the texture cache (0x28 bytes) */
struct s_texture_cache_entry
{
	byte unknown00[2];
	byte flags;
	bool resident;
	long pending;
	byte unknown08[4];
	long hardware_format;
	s_bitmap_data *bitmap;
	D3DResource resource;
	byte unknown20[8];
};

/* a bitmap predicted to be needed soon (8 bytes) */
struct s_texture_cache_request
{
	short salt;
	short unknown02;
	s_bitmap_data *bitmap;
};

/* a block of the cache's memory lent out (bink movies, the simulation world):
   the header sits just below the memory it describes */
typedef void (__stdcall *texture_cache_lock_proc)(void *address, long user_data);

struct s_texture_cache_lock
{
	long block_index;
	dword signature;
	void *address;
	long size;
	long user_data;
	texture_cache_lock_proc update;
	texture_cache_lock_proc release;
	s_texture_cache_lock *previous;
	s_texture_cache_lock *next;
};

s_record_pool *g_4e6454;
s_record_pool *g_4e6458;
s_texture_cache_lock *g_4e645c;
long g_4e646c;
dword g_4e6460;
s_physical_object *g_4e6464;
bool g_4e6479;
bool g_4e647a;
real g_4e647c;
dword g_4e6480;
long g_4e6484;
long g_4e6488;
dword g_55e724;
bool g_468841 = true;

long function_120bf0(void);
void __stdcall texture_cache_block_delete(long datum_index);
byte __stdcall texture_cache_entry_state(long datum_index);
bool __stdcall texture_cache_entry_busy(long datum_index);

static inline s_texture_cache_entry *texture_cache_entry_get(long datum_index)
{
	return (s_texture_cache_entry *)g_4e6454->data + (datum_index & 0xffff);
}

// @retail 0x12c0d0
void texture_cache_initialize(void)
{
	g_4e6454 = data_new_inlined("xbox texture", 0x200, sizeof(s_texture_cache_entry), 0, g_468758);
	g_4e6458 = data_new_inlined("xbox predicted texture", 0xc8, sizeof(s_texture_cache_request), 0, g_468758);
	g_4e6464 = physical_memory_new("xbox texture cache", 0, 0xc, 0x200, texture_cache_block_delete, texture_cache_entry_busy, texture_cache_entry_state, g_468758);
	g_4e6464->state = 2;
}

// @retail 0x12c190
void texture_cache_dispose(void)
{
	if (g_4e6454)
	{
		data_dispose(g_4e6454);
		g_4e6454 = NULL;
	}
	if (g_4e6464)
	{
		g_4e6464->allocator->deallocate(g_4e6464);
		g_4e6464 = NULL;
	}
}

// @retail 0x12c1e0
void texture_cache_initialize_for_new_map(void)
{
	long available = physical_memory_available();
	long pages = available / 4096;

	g_4e6460 = (dword)physical_memory_malloc_fixed(pages * 4096, PAGE_READWRITE | PAGE_WRITECOMBINE);
	g_4e6464->method_13d8b0(pages);
	g_4e6454->valid = true;
	record_pool_release_all(g_4e6454);
	g_4e6458->valid = true;
	record_pool_release_all(g_4e6458);
}

/* raises the cache's scale while it is nearly full, lowers it while it is not */
// @retail 0x12c2e0
void texture_cache_update_scale(void)
{
	real usage = 0.0f;
	long page_count = g_4e6464->page_count;

	if (page_count > 0)
	{
		usage = (real)physical_memory_used_pages(g_4e6464, 5) / (real)page_count;
	}
	g_4e6464->state = 2;
	if (g_468841 && page_count > 0)
	{
		if (usage >= 0.95f || g_4e647a)
		{
			g_4e647c += 0.05f;
		}
		else if (usage >= 0.85f)
		{
			g_4e647c += 0.02f;
		}
		else if (usage <= 0.5f)
		{
			g_4e647c -= 0.05f;
		}
		else if (usage <= 0.6f)
		{
			g_4e647c -= 0.02f;
		}
		else if (usage <= 0.7f)
		{
			g_4e647c -= 0.01f;
		}
		if (g_4e647c < 0.0f)
		{
			g_4e647c = 0.0f;
		}
		else if (g_4e647c > 2.0f)
		{
			g_4e647c = 2.0f;
		}
	}
	else
	{
		g_4e647c = 0.0f;
	}
	g_4e647a = false;
}

static inline bool texture_cache_lock_exists(s_texture_cache_lock *lock)
{
	s_texture_cache_lock *other;

	for (other = g_4e645c; other; other = other->next)
	{
		if (other == lock)
		{
			return true;
		}
	}
	return false;
}

// @retail 0x12c400
void texture_cache_update_locks(void)
{
	s_texture_cache_lock *lock = g_4e645c;

	while (lock)
	{
		if (lock->update)
		{
			lock->update(lock->address, lock->user_data);
		}
		if (!texture_cache_lock_exists(lock))
		{
			lock = g_4e645c;
		}
		else
		{
			lock = lock->next;
		}
	}
}

static __forceinline byte *function_12c531(s_record_pool_iterator *arg_0)
{
	s_record_pool *local_0 = arg_0->data;
	long local_1 = arg_0->index + 1;
	long local_2 = NONE;
	if (local_1 >= 0)
	{
		for (; local_1 < *(long volatile *)&local_0->high_water_index; local_1++)
		{
			if (local_0->bitmap[local_1 >> 5] & (1 << (local_1 & 0x1f)))
			{
				local_2 = local_1;
				break;
			}
		}
	}
	byte *local_3;
	if (local_2 != NONE)
	{
		local_3 = local_0->data + local_0->size * local_2;
		arg_0->index = local_2;
		arg_0->datum_index = (*(short *)local_3 << 16) | local_2;
	}
	else
	{
		arg_0->index = local_0->maximum_count;
		arg_0->datum_index = NONE;
		local_3 = NULL;
	}
	return local_3;
}

// @retail 0x12c530
void function_12c530(void)
{
	if (g_4e6454->valid)
	{
		s_record_pool_iterator iterator;
		s_texture_cache_entry *entry;

		iterator.data = g_4e6454;
		iterator.index = NONE;
		iterator.datum_index = NONE;
		while ((entry = (s_texture_cache_entry *)function_12c531(&iterator)) != NULL)
		{
			if (entry->bitmap)
			{
				entry->bitmap->unknown54 = 0;
				entry->bitmap->texture = NULL;
				entry->bitmap->unknown70 = 0;
			}
		}
	}
	g_4e6488 = 4;
}

// @retail 0x12c5b0
void function_12c5b0(void)
{
	if (++g_4e6488 == 0)
	{
		function_12c530();
	}
	physical_memory_new_frame(g_4e6464);
}

// @retail 0x12d160
byte __stdcall texture_cache_entry_state(long datum_index)
{
	s_texture_cache_entry *entry = texture_cache_entry_get(datum_index);
	byte result = 0;

	if (entry->flags & 1)
		return 0x10;
	if (entry->pending)
		result = 0x20;
	return result;
}

union s_12d1a1
{
    long field_0;
    bool field_4;
};

static __forceinline s_12d1a1 function_12d1a1(D3DResource *arg_0)
{
    s_12d1a1 local_0;
    local_0.field_0 = D3DResource_IsBusy(arg_0);
    return local_0;
}

// @retail 0x12d1a0
bool __stdcall texture_cache_entry_busy(long datum_index)
{
	s_texture_cache_entry *entry = texture_cache_entry_get(datum_index);

	if (!(entry->flags & 1) && (!(entry->flags & 2) || g_4e6479))
	{
		if (entry->hardware_format == NONE)
			return false;
		if (!entry->resident)
			return true;
		return function_12d1a1(&entry->resource).field_4;
	}
	return true;
}

/* the physical memory's callback when a block is freed: waits for the GPU to
   be done with the texture, then forgets it */
static __forceinline bool __stdcall function_12d201(long arg_0)
{
    s_texture_cache_entry *local_0 = texture_cache_entry_get(arg_0);
    if (!(local_0->flags & 1) && (!(local_0->flags & 2) || g_4e6479))
    {
        if (local_0->hardware_format == NONE)
            return false;
        if (!local_0->resident || D3DResource_IsBusy(&local_0->resource))
            return true;
        return false;
    }
    return true;
}

// @retail 0x12d200
void __stdcall texture_cache_block_delete(long datum_index)
{
	s_texture_cache_entry *entry = texture_cache_entry_get(datum_index);

	if (entry->bitmap)
	{
		while (function_12d201(datum_index))
		{
			async_globals.tasks_added = function_120bf0();
			if (entry->resident)
			{
				D3DResource_BlockUntilNotBusy(&entry->resource);
			}
		}
		entry->bitmap->block_indices[entry->pending] = NONE;
		entry->bitmap->unknown40[entry->pending] = 0;
		if (entry->pending == 0)
		{
			entry->bitmap->unknown54 = 0;
			entry->bitmap->texture = NULL;
			entry->bitmap->unknown70 = 0;
		}
	}
	record_pool_release(g_4e6454, datum_index);
}

/* whether a bitmap format is one the cache scales down (not the compressed
   formats 12-16) */
// @retail 0x12ccb0
bool texture_cache_format_scalable(long format)
{
	bool result = false;

	switch (format)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 17:
	case 18:
		result = true;
		break;
	}
	return result;
}
/* asks for a bitmap: touches its block when it is in the cache, else queues
   a request for it */
// @retail 0x12cc10
bool texture_cache_bitmap_request(s_bitmap_data *bitmap)
{
	bool result = false;

	if (g_4e6458->valid)
	{
		if (bitmap->block_indices[0] != NONE)
		{
			s_texture_cache_entry *entry = texture_cache_entry_get(bitmap->block_indices[0]);

			((s_physical_block *)g_4e6464->blocks->data)[bitmap->block_indices[0] & 0xffff].time = g_4e6464->time;
			if (entry->resident)
			{
				result = true;
			}
		}
		else if (!(bitmap->flags & 0x400))
		{
			long request_index = record_pool_allocate(g_4e6458);

			if (request_index != NONE)
			{
				((s_texture_cache_request *)g_4e6458->data)[request_index & 0xffff].bitmap = bitmap;
				bitmap->flags |= 0x400;
			}
			else if (GetTickCount() > g_55e724)
			{
				g_55e724 = GetTickCount() + 30000;
			}
		}
	}
	return result;
}

/* forgets a bitmap: its request and its blocks */
// @retail 0x12c770
void texture_cache_bitmap_unload(s_bitmap_data *bitmap)
{
	if (bitmap->flags & 0x200)
	{
		long *block_index;
		long count;

		if (bitmap->flags & 0x400)
		{
			s_record_pool_iterator iterator;
			s_texture_cache_request *request;

			iterator.data = g_4e6458;
			iterator.index = NONE;
			iterator.datum_index = NONE;
			while ((request = (s_texture_cache_request *)data_iterator_next_calling(&iterator)) != NULL)
			{
				if (request->bitmap == bitmap)
				{
					record_pool_release(g_4e6458, iterator.datum_index);
					break;
				}
			}
		}
		block_index = bitmap->block_indices;
		count = 3;
		do
		{
			if (*block_index != NONE)
			{
				g_4e6464->block_delete(*block_index);
				*block_index = NONE;
			}
			block_index[6] = 0;
			block_index++;
		}
		while (--count);
		bitmap->flags &= ~0x600;
		bitmap->unknown54 = 0;
	}
}

/* lends out size bytes of the cache's memory, page aligned, or NULL */
// @retail 0x12d2f0
long __stdcall function_12d2f0(long size, long user_data, long update, long release)
{
	long result = 0;
	long block_index = function_13d370(g_4e6464, size + 0x2023, 1);

	if (block_index != NONE)
	{
		byte *address = (byte *)((((((s_physical_block *)g_4e6464->blocks->data)[block_index & 0xffff].offset << g_4e6464->page_shift) + g_4e6460 + 0x1023)) & 0xfffff000);
		s_texture_cache_lock *lock = (s_texture_cache_lock *)address - 1;
		s_texture_cache_entry *entry = texture_cache_entry_get(datum_new_at_index_with_salt(g_4e6454, block_index));

		lock->signature = (dword)address ^ 0x2281972;
		lock->block_index = block_index;
		lock->address = address;
		lock->size = size;
		lock->update = (texture_cache_lock_proc)update;
		lock->release = (texture_cache_lock_proc)release;
		lock->user_data = user_data;
		lock->previous = NULL;
		lock->next = g_4e645c;
		if (g_4e645c)
		{
			g_4e645c->previous = lock;
		}
		g_4e645c = lock;
		XPhysicalProtect(lock->address, lock->size, PAGE_READWRITE);
		memset(&entry->flags, 0, sizeof(s_texture_cache_entry) - 2);
		entry->flags |= 1;
		result = (long)address;
	}
	return result;
}

double timing_ticks_to_seconds(__int64 ticks);

static __int64 read_tsc(void)
{
	volatile __int64 t = 0;
	__asm rdtsc
}

/* 0x12c600 (xbox_texture_cache_update.cpp): the cache's per-frame update */
void function_12c600(void);

static __forceinline long texture_cache_next_used_index(s_record_pool *data, long index)
{
	if (index >= 0 && index < *(long volatile *)&data->high_water_index)
	{
		do
		{
			if (data->bitmap[index >> 5] & (1 << (index & 0x1f)))
			{
				return index;
			}
			index++;
		}
		while (index < *(long volatile *)&data->high_water_index);
	}
	return NONE;
}

/* takes back every lent block, waits for the GPU, and forgets the predicted
   bitmaps */
// @retail 0x12d0a0
void function_12d0a0(void)
{
	while (g_4e645c)
	{
		g_4e645c->release(g_4e645c->address, g_4e645c->user_data);
	}
	D3DDevice_KickPushBuffer();
	D3DDevice_IsBusy();
	physical_memory_flush(g_4e6464);
	if (g_4e6458->valid)
	{
		s_record_pool *data = g_4e6458;
		long index = NONE;

		while ((index = texture_cache_next_used_index(data, index + 1)) != NONE)
		{
			s_texture_cache_request *request = (s_texture_cache_request *)(data->data + data->size * index);

			long local_0 = (request->salt << 16) | index;
			request->bitmap->flags &= ~0x400;
			record_pool_release(data, local_0);
		}
	}
}

// @retail 0x12c290
void texture_cache_dispose_from_old_map(void)
{
	g_4e6479 = true;
	function_12d0a0();
	g_4e6454->valid = false;
	g_4e6458->valid = false;
	if (g_4e646c)
	{
		g_4e646c = 0;
	}
	g_4e6464->method_13d8b0(0);
	g_4e6460 = 0;
}

/* an iteration over the tags of one group (unknown_122870.cpp) */
struct s_tag_iterator
{
	long unknown00;
	long unknown04;
	long datum_index;
	long next_index;
	long group_tag;
};

long function_122c70(s_tag_iterator *iterator);
long function_213760(dword location, long size, void *buffer, dword *bytes_read, bool *done, long category, long priority);

/* where the cache file keeps the bitmaps' shared pixel data, and its size */
dword g_547858;
long g_54785c;
bool g_4e6468;

struct s_bitmap_group_view
{
	byte unknown00[0x44];
	long bitmap_count;
};

/* walks the bitmap tags, then reads the shared pixel data into the top of
   the physical memory */
// @retail 0x12c640
void texture_cache_load_shared_data(void)
{
	s_tag_iterator iterator;
	long tag_index;

	iterator.next_index = 0;
	iterator.group_tag = 'bitm';
	while ((tag_index = function_122c70(&iterator)) != NONE)
	{
		s_bitmap_group_view *bitmap = (s_bitmap_group_view *)g_4e3b44[tag_index & 0xffff].bytes;

		for (short i = 0; i < bitmap->bitmap_count; i++)
		{
		}
	}
	if (g_547858 && g_54785c)
	{
		long size = g_54785c;
		long aligned_size = (size + 0xfff) & 0xfffff000;
		long read_size = size;
		void *memory;
		bool volatile done;

		if (size & 0x1ff)
		{
			read_size = (size | 0x1ff) + 1;
		}
		memory = physical_memory_malloc_fixed(aligned_size, PAGE_READWRITE | PAGE_WRITECOMBINE);
		g_4e646c = (long)memory;
		g_4e6468 = true;
		function_213760(g_547858, read_size, memory, NULL, (bool *)&done, 3, 7);
		if (!done)
		{
			while (!done)
			{
				SwitchToThread();
			}
		}
	}
	else
	{
		g_4e6468 = false;
	}
}

extern long g_450768[8][24];
short bitmap_get_mipmap_count(short width, short height, short depth, short format, bool linear, short maximum_levels);
long log2_floor(dword value);

/* builds a bitmap's hardware texture header over its description, pointing
   into the shared pixel data */
// @retail 0x12c820
void texture_cache_bitmap_build_texture(s_bitmap_data *bitmap)
{
	short depth;
	short height;
	short width;
	D3DTexture *texture = (D3DTexture *)&bitmap->hardware_common;
	dword data = bitmap->hardware_data + g_4e646c;
	short format = bitmap->format;
	long hardware_format = g_450768[0][format];
	short levels;

	if ((bitmap->flags & 0x20) && (format == 10 || format == 11))
	{
		hardware_format = 0x33;
	}
	depth = bitmap->depth;
	height = bitmap->height;
	width = bitmap->width;
	levels = bitmap_get_mipmap_count(width, height, depth, format, (bitmap->flags >> 4) & 1, 0);
	texture->Data = 0;
	texture->Lock = 0;
	texture->Common = 0x40001;
	texture->Format = 9;
	if (bitmap->type == 2)
	{
		texture->Format = 0xd;
	}
	texture->Format |= ((((bitmap->type != 1) ? 2 : 3) | (hardware_format << 4)) << 4) | ((levels + 1) << 16);
	texture->Format |= (short)log2_floor(width) << 20;
	texture->Format |= (short)log2_floor(height) << 24;
	texture->Format |= (short)log2_floor(depth) << 28;
	texture->Size = 0;
	texture->Data = data & 0xfffffff;
}

/* a bitmap's hardware texture in the shared pixel data, built the first time
   it is asked for */
// @retail 0x12c960
D3DTexture *texture_cache_bitmap_get_shared_texture(s_bitmap_data *bitmap)
{
	D3DTexture *result = NULL;

	if (g_4e6468)
	{
		D3DTexture *texture = (D3DTexture *)&bitmap->hardware_common;

		if (texture->Common || bitmap->hardware_lock > 0)
		{
			if ((long)texture->Common <= 0)
			{
				texture_cache_bitmap_build_texture(bitmap);
			}
			result = texture;
		}
	}
	return result;
}

bool g_4e647b;

/* the texture to draw a bitmap with at a scale: its own when it is not
   cached, the shared one for scalable formats, else the cached level the
   scale (raised by the cache's own bias) asks for when it is resident; a
   texture remembered for this frame or the next wins */
// @retail 0x12ccf0
D3DTexture *texture_cache_bitmap_get_texture(s_bitmap_data *bitmap, dword flags, real bias)
{
	D3DTexture *result = NULL;
	real scale = bias;
	bool unscaled;
	long level;

	if (g_468841)
	{
		scale = (real)bitmap->level_bias * 0.01f;
		scale += bias;
		scale += g_4e647c;
	}
	unscaled = (bool)((flags >> 2) & 1);
	if (unscaled)
	{
		scale = 0.0f;
	}
	if (!(bitmap->flags & 0x200))
	{
		result = bitmap->texture;
	}
	else if (g_4e647b && texture_cache_format_scalable(bitmap->cache_format))
	{
		if (unscaled)
		{
			result = NULL;
		}
		else
		{
			result = texture_cache_bitmap_get_shared_texture(bitmap);
		}
	}
	else if (bitmap->minimum_scale > scale)
	{
		long block_index;

		level = 0;

		if (scale >= 2.0f)
		{
			level = 2;
		}
		else if (scale >= 1.0f)
		{
			level = 1;
		}
		block_index = bitmap->block_indices[level];
		if (block_index != NONE)
		{
			s_texture_cache_entry *entry = texture_cache_entry_get(block_index);

			((s_physical_block *)g_4e6464->blocks->data)[block_index & 0xffff].time = g_4e6464->time;
			if (entry->resident)
			{
				result = (D3DTexture *)&entry->resource;
				if (!level)
				{
					bitmap->unknown70 = g_4e6488 + 2;
					bitmap->texture = result;
				}
			}
		}
	}
	if (bitmap->unknown70 >= g_4e6488 && bitmap->texture)
	{
		return bitmap->texture;
	}
	return result;
}

/* the highest level (of three) worth loading at a scale: level 1 needs a
   scale of 1, level 2 of 2, and only levels over 1 KB count after the first */
// @retail 0x12cb10
long texture_cache_bitmap_level(s_bitmap_data const *bitmap, real scale)
{
	long result = 0;

	for (long i = 0; i < 3; i++)
	{
		if (bitmap->data_offsets[i] != NONE)
		{
			long size = bitmap->field_34[i];

			if (size && (!i || size > 0x400))
			{
				real thresholds[3] = { 0.0f, 1.0f, 2.0f };

				if (thresholds[i] > scale)
				{
					break;
				}
				result = i;
			}
		}
	}
	return result;
}

/* function_12d2f0, pumping the cache until a block is free: not at all
   (type 0), or for up to 30 pumps and 0.1 seconds (type 1) or 90 pumps and
   one second (type 2) */
// @retail 0x12d400
long function_12d400(long type, long size, long user_data, long update, long release)
{
	__int64 start = read_tsc();
	long result = 0;
	real timeout = 0.0f;
	long maximum_pumps = 0;
	volatile long attempts = 5;
	long pumps;

	switch (type)
	{
	case 1:
		timeout = 0.1f;
		maximum_pumps = 30;
		attempts = 2;
		break;
	case 2:
		timeout = 1.0f;
		maximum_pumps = 90;
		attempts = 2;
		break;
	}

	if (size > 0 && g_4e6464->page_count > 0)
	{
		pumps = 0;
		do
		{
			result = function_12d2f0(size, user_data, update, release);
			if (result == 0)
			{
				if (pumps < maximum_pumps)
				{
					pumps++;
					function_12c600();
				}
				else
				{
					__int64 elapsed = read_tsc() - start;
					if (elapsed < 0)
					{
						elapsed = 0;
					}
					if (!((real)timing_ticks_to_seconds(elapsed) < timeout))
					{
						break;
					}
					D3DDevice_KickPushBuffer();
					D3DDevice_IsBusy();
					SwitchToThread();
				}
			}
		} while (result == 0);
	}
	return result;
}

/* takes back memory lent out by function_12d2f0 */
// @retail 0x12d520
void function_12d520(long address)
{
	s_texture_cache_lock *lock = (s_texture_cache_lock *)address - 1;
	s_texture_cache_entry *entry = texture_cache_entry_get(lock->block_index);
	s_texture_cache_lock *next;
	s_texture_cache_lock *previous;

	XPhysicalProtect(lock->address, lock->size, PAGE_READWRITE | PAGE_WRITECOMBINE);
	entry->flags &= ~1;
	next = lock->next;
	previous = lock->previous;
	lock->signature = 0;
	if (next)
	{
		next->previous = previous;
	}
	if (previous)
	{
		previous->next = next;
	}
	else
	{
		g_4e645c = next;
	}
	g_4e6464->block_delete(lock->block_index);
}

struct s_type_7ba8e9;
long function_1365a0(s_type_7ba8e9 const *bitmap, short mipmap_index);

/* dimensions and resident level addresses used to build a texture header */
struct s_bitmap_texture_view
{
	dword signature;
	short width;
	short height;
	char depth;
	byte unknown09;
	short type;
	short format;
	word flags;
	byte unknown10[4];
	short mipmap_count;
	byte unknown16[0x40 - 0x16];
	dword level_addresses[3];
};

// @retail 0x12d6d0
void __stdcall function_12d6d0(s_bitmap_texture_view const *bitmap, long level, D3DTexture *texture)
{
	texture->Data = 0;
	texture->Lock = 0;
	texture->Common = 0x40001;
	if (bitmap->flags & 0x10)
	{
		long format = g_450768[1][bitmap->format];
		if ((bitmap->flags & 0x20) && (bitmap->format == 10 || bitmap->format == 11))
			format = 0x36;
		texture->Format = (format << 8) | 0x10029;
		long pitch = function_1365a0((s_type_7ba8e9 const *)bitmap, 0);
		texture->Size = ((((pitch - 1) / 64) << 12 | (bitmap->height - 1)) << 12) | (bitmap->width - 1);
	}
	else
	{
		real reductions[3] = { 0.0f, 1.0f, 2.0f };
		short reduction = (short)reductions[level];
		long width = bitmap->width >> reduction;
		long height = bitmap->height >> reduction;
		long depth = bitmap->depth >> reduction;
		long levels = bitmap_get_mipmap_count((short)width, (short)height, (short)depth, bitmap->format, false, bitmap->mipmap_count - reduction);
		long format = g_450768[0][bitmap->format];
		if ((bitmap->flags & 0x20) && (bitmap->format == 10 || bitmap->format == 11))
			format = 0x33;
		short width_bits = (short)log2_floor(width);
		short height_bits = (short)log2_floor(height);
		short depth_bits = (short)log2_floor(depth);

		texture->Format = (((((((depth_bits << 4) | height_bits) << 4) | width_bits) << 12 | format) << 4 |
			(bitmap->type == 1 ? 3 : 2)) << 4) | (bitmap->type == 2 ? 4 : 0) | ((levels + 1) << 16) | 9;
		texture->Size = 0;
	}
	texture->Data = bitmap->level_addresses[level] & 0xfffffff;
}

// @retail 0x12d5a0
bool function_12d5a0(s_bitmap_data *arg_0, long arg_1, bool arg_2, bool arg_3, long arg_4)
{
	long local_0 = function_13d370(g_4e6464, arg_0->field_34[arg_1], (arg_2 || arg_3) ? 2 : 5);
	if (arg_2 && !arg_3)
		arg_4 = 7;
	if (local_0 == NONE)
		return false;
	long local_1 = (((s_physical_block *)g_4e6464->blocks->data)[local_0 & 0xffff].offset << g_4e6464->page_shift) + g_4e6460;
	datum_new_at_index_with_salt(g_4e6454, local_0);
	s_texture_cache_entry *local_2 = texture_cache_entry_get(local_0);
	local_2->resident = false;
	local_2->flags &= ~1;
	local_2->pending = arg_1;
	local_2->bitmap = arg_0;
	memset(&local_2->resource, 0, sizeof(D3DTexture));
	arg_0->block_indices[arg_1] = local_0;
	arg_0->unknown40[arg_1] = local_1;
	if (!arg_1)
		arg_0->unknown54 = local_1;
	function_12d6d0((s_bitmap_texture_view *)arg_0, arg_1, (D3DTexture *)&local_2->resource);
	long local_3 = arg_0->field_34[arg_1];
	if (local_3 & 0x1ff)
		local_3 = (local_3 | 0x1ff) + 1;
	volatile long local_4 = arg_0->unknown40[3];
	local_2->hardware_format = function_213760(arg_0->data_offsets[arg_1], local_3, (void *)local_1, NULL, &local_2->resident, 3, arg_4);
	*(long *)local_2->unknown08 = arg_4;
	g_4e6484++;
	return true;
}

bool function_120ce0(long arg_0, long arg_1);
void function_125d60(void);

// @retail 0x12c990
D3DTexture *function_12c990(s_bitmap_data *arg_0, long arg_1, bool arg_2, bool arg_3, bool arg_4, long arg_5, bool *arg_6, bool *arg_7)
{
	D3DTexture *local_0 = NULL;
	bool local_1 = async_globals.tasks_added > 25 || g_4e6484 >= 10;
	if (arg_2 && (arg_3 || arg_5 > 2 || !local_1))
	{
		if (arg_0->data_offsets[arg_1] != NONE && arg_0->field_34[arg_1] && (!arg_1 || arg_0->field_34[arg_1] > 0x400))
		{
			if (arg_0->block_indices[arg_1] == NONE)
			{
				if (!function_12d5a0(arg_0, arg_1, arg_3, arg_4, arg_5))
					*arg_7 = true;
			}
			if (arg_0->block_indices[arg_1] != NONE)
			{
				long local_2 = arg_0->block_indices[arg_1];
				s_texture_cache_entry *local_3 = texture_cache_entry_get(local_2);
				((s_physical_block *)g_4e6464->blocks->data)[local_2 & 0xffff].time = g_4e6464->time;
				if (!*(volatile bool *)&local_3->resident)
				{
					if (arg_3)
					{
						*arg_6 = true;
						function_120ce0(local_3->hardware_format, 7);
						function_120d50(&local_3->resident, true);
					}
					else if (arg_5 > *(long *)local_3->unknown08)
					{
						*(long *)local_3->unknown08 = arg_5;
						function_120ce0(local_3->hardware_format, arg_5);
					}
				}
				if (*(volatile bool *)&local_3->resident)
					local_0 = (D3DTexture *)&local_3->resource;
			}
		}
	}
	else if (arg_0->block_indices[arg_1] != NONE)
	{
		long local_4 = arg_0->block_indices[arg_1];
		s_texture_cache_entry *local_5 = texture_cache_entry_get(local_4);
		((s_physical_block *)g_4e6464->blocks->data)[local_4 & 0xffff].time = g_4e6464->time;
		if (*(volatile bool *)&local_5->resident)
			local_0 = (D3DTexture *)&local_5->resource;
	}
	return local_0;
}

// @retail 0x12cb80
bool function_12cb80(s_bitmap_data *arg_0)
{
	bool local_0 = true;
	for (long local_1 = 0; local_1 < 3; local_1++)
	{
		if (arg_0->data_offsets[local_1] != NONE && arg_0->field_34[local_1] && (!local_1 || arg_0->field_34[local_1] > 0x400))
		{
			bool local_2;
			bool local_3;
			function_12c990(arg_0, local_1, true, true, false, 7, &local_2, &local_3);
			long local_4 = arg_0->block_indices[local_1];
			if (local_4 != NONE)
			{
				byte *local_5 = &texture_cache_entry_get(local_4)->flags;
				*local_5 |= 2;
			}
			else
				local_0 = false;
		}
	}
	return local_0;
}

#include <xmmintrin.h>
D3DTexture *function_12ce00(s_bitmap_data *arg_0, dword arg_1, real arg_2);

// @retail 0x12c450
void function_12c450(void)
{
	if (g_4e6458->valid && async_globals.tasks_added <= 25)
	{
		long local_0 = 0;
		s_record_pool_iterator local_1;
		local_1.data = g_4e6458;
		local_1.index = NONE;
		local_1.datum_index = NONE;
		s_texture_cache_request *local_2;
		while ((local_2 = (s_texture_cache_request *)data_iterator_next_calling(&local_1)) != NULL && local_0 < 5)
		{
			s_bitmap_data *local_3 = local_2->bitmap;
			if (local_3->unknown70 <= g_4e6488 || !local_3->texture)
			{
				_mm_prefetch((char const *)&local_3->flags, _MM_HINT_T0);
				_mm_prefetch((char const *)&local_3->block_indices[0], _MM_HINT_T0);
				_mm_prefetch((char const *)&local_3->block_indices[1], _MM_HINT_T0);
				_mm_prefetch((char const *)&local_3->block_indices[2], _MM_HINT_T0);
				_mm_prefetch((char const *)local_3->texture, _MM_HINT_T0);
				if (!texture_cache_bitmap_get_texture(local_3, 2, 0.0f))
					function_12ce00(local_3, 2, 0.0f);
			}
			local_2->bitmap->flags &= ~0x400;
			record_pool_release(g_4e6458, local_1.datum_index);
			local_0++;
		}
	}
}


bool g_46883c;
bool g_46883d;
extern long g_4e6470, g_4e6474;
extern D3DResource *g_485ae4, *g_485ae8, *g_485aec;

// @retail 0x12ce00
D3DTexture *function_12ce00(s_bitmap_data *bitmap, dword flags, real bias)
{
    s_bitmap_data *const *bitmap_reference = &bitmap;
    s_bitmap_data *view = *bitmap_reference;
    bool wait = (flags & 1) != 0;
    bool request = (flags & 2) != 0;
    bool unscaled = (flags & 4) != 0;
    D3DTexture *result = NULL;
    real scale = bias;
    if (g_468841)
    {
        scale = (real)view->level_bias * 0.01f;
        scale += bias;
        scale += g_4e647c;
    }
    bool restricted = false;
    bool failed = false;
    if (g_46883d)
        scale = 0.0f;
    if (g_46883c)
        scale = 1.0f;
    if (!(view->flags & 0x200))
        return view->texture;

    if (!unscaled)
    {
        result = texture_cache_bitmap_get_shared_texture(view);
        if (g_4e6470 > 0 && (g_4e6474 == 0 ||
            (g_4e6474 == 1 && view->cache_format == 3)))
        {
            restricted = true;
            wait = g_4e6470 == 1;
        }
        else if (result && !(view->flags & 0x80))
            wait = false;
    }
    if (((g_4e647b && texture_cache_format_scalable(view->cache_format)) ||
        bias >= view->minimum_scale) && result)
        return result;

    long level = unscaled ? 0 : texture_cache_bitmap_level(view, scale);
    long priority = 2;
    if (!restricted)
    {
        if (wait)
            priority = 7;
        else if (flags & 8)
            priority = 5;
        else if (flags & 16)
            priority = 1;
    }
    bool waited;
    D3DTexture *loaded = function_12c990(view, level, request, wait,
        restricted, priority, &waited, &failed);
    if (loaded)
        result = loaded;
    else if (!unscaled)
    {
        for (long i = 0; i < 3; ++i)
        {
            long block_index = view->block_indices[i];
            if (block_index != NONE)
            {
                s_texture_cache_entry *entry = texture_cache_entry_get(block_index);
                ((s_physical_block *)g_4e6464->blocks->data)[block_index & 0xffff].time = g_4e6464->time;
                if (entry->resident)
                {
                    D3DTexture *candidate = (D3DTexture *)&entry->resource;
                    if (candidate)
                    {
                        result = candidate;
                        wait = false;
                    }
                }
            }
        }
        if (wait && !result)
        {
            if (view->type == 0)
                result = (D3DTexture *)g_485ae4;
            else if (view->type == 1)
                result = (D3DTexture *)g_485ae8;
            else
                result = (D3DTexture *)g_485aec;
            g_4e647a = true;
            return result;
        }
    }
    if (failed)
        g_4e647a = true;
    return result;
}
