// @flags /O2 /arch:SSE /Gr
#include "allocator_storage.h"
/* UNKNOWN_12DE70.CPP: geometry blocks streamed from the cache file: a request
   queues a read job for the block, and the block is linked (its fixups
   applied) once the read is done. Decompiled by lane F for the sound
   promotions (0x18c630, 0x18c720). */

#include "unknown_11c920.h"
#include <xtl.h>
#include "globals.h"
#include "data_array.h"
#include "geometry_cache.h"
#include "unknown_223b60.h"
#include "physical_memory.h"
#include "async.h"
#include "main_messages.h"

/* the cache file's physical memory: blocks of 24 bytes */
extern s_physical_object *g_4e649c;
extern dword g_4e6494;

/* a read in flight (12 bytes) */
struct s_cache_block
{
	short identifier;
	bool done;
	byte priority;
	long job;
	s_geometry_block_info *block;
};

s_record_pool *g_4e648c;
long g_4e64a0;
long g_4e64a4;
long g_4e64a8;
bool g_4e64b0;
bool g_468c4c;

long function_213760(dword location, long size, void *buffer, dword *bytes_read, bool *done, long type, long priority);
bool function_120ce0(long job, long priority);
void function_120d50(bool volatile *done, bool idle);
long function_120bf0(void);
void function_12e0c0(void);

static inline s_physical_object *physical_object_get(void)
{
	return g_4e649c;
}

static inline s_physical_block *physical_block_get(long index)
{
	return &((s_physical_block *)physical_object_get()->blocks->data)[index & 0xffff];
}

// @retail 0x12e3a0
bool function_12e3a0(bool wait, s_geometry_block_info *block, bool urgent, long priority)
{
	bool immediate = wait || urgent;
	long local_6 = block->block_size;
	long local_7 = immediate ? 1 : 5;
	long physical_index = function_13d370(g_4e649c, local_6, local_7);

	if (physical_index != NONE)
	{
		dword address = (physical_block_get(physical_index)->offset << physical_object_get()->page_shift) + g_4e6494;
		long cache_index = datum_new_at_index_with_salt(g_4e648c, physical_index);
		s_cache_block *cache_block = &((s_cache_block *)g_4e648c->data)[cache_index & 0xffff];
		long size;
	
		cache_block->block = block;
		block->cache_block_index = physical_index;
		cache_block->block->runtime_linked = false;
		size = block->block_size;
		if (size & 0x1ff)
		{
			size = (size | 0x1ff) + 1;
		}
		long local_2 = block->block_offset;
		volatile long local_0 = *(long *)((byte *)block + 0x18);
		cache_block->job = function_213760(local_2, size, (void *)address, NULL, &cache_block->done, 4, priority);
		cache_block->priority = (byte)priority;
		return true;
	}
	return false;
}

// @retail 0x12de70
bool function_12de70(s_geometry_block_info *block, dword flags)
{
	bool result = false;

	if (block->runtime_linked)
	{
		physical_block_get(block->cache_block_index)->time = physical_object_get()->time;
		result = true;
	}
	else if (block->section_data_size > 0 && block->resource_data_size >= 0)
	{
		bool wait = flags & 1;
		bool optional = (flags >> 2) & 1;
		bool load = (flags >> 1) & 1;
		bool background = (flags >> 3) & 1;
		bool urgent = false;
		bool structure = false;
		dword group_tag = *(dword *)&g_4e3b44[block->owner_tag_index];
		long priority;

		if (group_tag == 'ltmp' || group_tag == 'sbsp')
		{
			structure = true;
		}
		if (g_468c4c)
		{
			wait = false;
		}

		if (g_4e64a0 > 0 && load && !optional && !wait)
		{
			if (g_4e64a4 == 0 || g_4e64a4 == 1 && structure)
			{
				urgent = true;
			}
			if (g_4e64a0 == 1 && urgent)
			{
				wait = true;
			}
		}

		if (urgent)
		{
			priority = 2;
		}
		else if (wait)
		{
			priority = 7;
		}
		else if (background)
		{
			priority = 5;
		}
		else
		{
			priority = !optional + 1;
		}

		if (block->cache_block_index == NONE && load)
		{
			function_12e3a0(wait, block, urgent, priority);
		}

		long physical_index = block->cache_block_index;
		if (physical_index != NONE)
		{
			s_cache_block *cache_block = &((s_cache_block *)g_4e648c->data)[physical_index & 0xffff];

			bool done = cache_block->done;
			physical_block_get(physical_index)->time = physical_object_get()->time;
			if (!done)
			{
				if (priority > cache_block->priority)
				{
					function_120ce0(cache_block->job, 7);
					cache_block->priority = (byte)priority;
				}
				if (wait)
					function_120d50(&cache_block->done, true);
			}
			if (cache_block->done)
			{
				result = true;
				if (!block->runtime_linked)
				{
					fixup_group_apply((s_fixup_group *)block, (byte *)(physical_block_get(block->cache_block_index)->offset << physical_object_get()->page_shift) + g_4e6494);
					block->runtime_linked = true;
				}
			}
		}
		else if (!optional && load)
		{
			g_4e64b0 = true;
		}
		if (!result && structure)
		{
			g_4e64a8++;
		}
	}
	return result;
}

/* blocks waiting to be requested (8 bytes each) */
struct s_pending_block
{
	short identifier;
	byte unknown02[2];
	s_geometry_block_info *block;
};

s_record_pool *g_4e6490;
dword g_55e728;

// @retail 0x12dcb0
bool function_12dcb0(s_geometry_block_info *block)
{
	bool result = false;

	if (block && block->section_data_size > 0 && block->resource_data_size >= 0)
	{
		if (block->runtime_linked)
		{
			if (block->cache_block_index != NONE)
			{
				result = true;
				physical_block_get(block->cache_block_index)->time = physical_object_get()->time;
			}
			else
			{
				block->runtime_linked = false;
			}
		}
		else if (g_4e64a0 > 0 && g_4e6098)
		{
			function_12de70(block, 3);
		}
		else if (!(block->flags & 4))
		{
			long pending_index = record_pool_allocate(g_4e6490);

			if (pending_index != NONE)
			{
				((s_pending_block *)g_4e6490->data)[pending_index & 0xffff].block = block;
				block->flags |= 4;
			}
			else if (GetTickCount() > g_55e728)
			{
				g_55e728 = GetTickCount() + 30000;
			}
		}
	}
	return result;
}
long g_4e64ac;

void __stdcall function_12e290(long block_index);
bool __stdcall geometry_cache_block_busy(long block_index);

/* blocks of the geometry cache have no state of their own (identical to,
   and folded with, c_object_type_definition::v30) */
static byte __stdcall geometry_cache_block_state(long block_index)
{
	return 0;
}

// @retail 0x12d8b0
void geometry_cache_initialize(void)
{
	g_4e648c = data_new_inlined("xbox geometry", 0x200, sizeof(s_cache_block), 0, g_468758);
	g_4e6490 = data_new_inlined("xbox predicted geometry", 0x15e, sizeof(s_pending_block), 0, g_468758);
	g_4e649c = physical_memory_new("xbox geometry cache", 0, 0xc, 0x200, function_12e290, geometry_cache_block_busy, geometry_cache_block_state, g_468758);
}

// @retail 0x12d970
void geometry_cache_dispose(void)
{
	if (g_4e648c)
	{
		data_dispose(g_4e648c);
		g_4e648c = NULL;
	}
	if (g_4e6490)
	{
		data_dispose(g_4e6490);
		g_4e6490 = NULL;
	}
	if (g_4e649c)
	{
		g_4e649c->allocator->deallocate(g_4e649c);
		g_4e649c = NULL;
	}
}

// @retail 0x12daf0
void geometry_cache_initialize_for_new_map(void)
{
	g_4e64a0 = 0;
	g_4e64b0 = false;
	g_4e64ac = 0;
	if (g_4e648c)
	{
		g_4e648c->valid = true;
		record_pool_release_all(g_4e648c);
	}
	if (g_4e6490)
	{
		g_4e6490->valid = true;
		record_pool_release_all(g_4e6490);
	}
}

// @retail 0x12db40
void geometry_cache_dispose_from_old_map(void)
{
	physical_memory_new_frame(g_4e649c);
	function_12e0c0();
	if (g_4e648c)
	{
		g_4e648c->valid = false;
	}
	if (g_4e6490)
	{
		g_4e6490->valid = false;
	}
}

/* the reads still in flight */
// @retail 0x12dba0
long function_12dba0(void)
{
	long count = 0;

	if (g_4e6490->valid)
	{
		s_record_pool_iterator iterator;
		s_cache_block *cache_block;

		iterator.data = g_4e648c;
		iterator.index = NONE;
		iterator.datum_index = NONE;
		while ((cache_block = (s_cache_block *)data_iterator_next_inlined(&iterator)) != NULL)
		{
			if (!cache_block->done)
			{
				count++;
			}
		}
	}
	return count;
}

/* requests the predicted blocks, a few at a time */
// @retail 0x12dc10
void function_12dc10(void)
{
	s_record_pool *pending_blocks = g_4e6490;

	if (pending_blocks->valid && async_globals.tasks_added <= 25)
	{
		long requests = function_12dba0();
		struct
		{
			s_pending_block *field_0;
			s_record_pool_iterator field_4;
		} local_0;

		local_0.field_4.data = pending_blocks;
		local_0.field_4.index = NONE;
		local_0.field_4.datum_index = NONE;
		while ((local_0.field_0 = (s_pending_block *)data_iterator_next_calling(&local_0.field_4)) != NULL && requests < 5)
		{
			function_12de70(local_0.field_0->block, 6);
			byte *local_1 = &local_0.field_0->block->flags;
			*local_1 &= ~4;
			record_pool_release(g_4e6490, local_0.field_4.datum_index);
			requests++;
		}
	}
}

/* forgets a block: its prediction and its memory */
static __forceinline byte *function_12ddd1(s_record_pool_iterator *arg_0)
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

static __forceinline void function_12ddd2(byte *arg_0)
{
	*arg_0 &= ~4;
}

// @retail 0x12ddd0
void function_12ddd0(s_geometry_block_info *block)
{
	if (block->flags & 4)
	{
		s_record_pool_iterator iterator;
		s_pending_block *pending;

		iterator.data = g_4e6490;
		iterator.index = NONE;
		iterator.datum_index = NONE;
		while ((pending = (s_pending_block *)function_12ddd1(&iterator)) != NULL)
		{
			if (block == pending->block)
			{
				record_pool_release(g_4e6490, iterator.datum_index);
				break;
			}
		}
	}
	function_12ddd2(&block->flags);
	if (block->cache_block_index != NONE)
	{
		g_4e649c->block_delete(block->cache_block_index);
		block->cache_block_index = NONE;
		block->runtime_linked = false;
	}
}

// @retail 0x12e0c0
void function_12e0c0(void)
{
	if (g_4e649c)
	{
		D3DDevice_KickPushBuffer();
		D3DDevice_IsBusy();
		physical_memory_flush(g_4e649c);
		if (g_4e6490->valid)
		{
			s_record_pool_iterator iterator;
			s_pending_block *pending;

			iterator.data = g_4e6490;
			iterator.index = NONE;
			iterator.datum_index = NONE;
			while ((pending = (s_pending_block *)data_iterator_next_calling(&iterator)) != NULL)
			{
				byte *local_0 = &pending->block->flags;
				*local_0 &= ~4;
				record_pool_release(g_4e6490, iterator.datum_index);
			}
		}
	}
}

/* forgets the blocks of one tag */
// @retail 0x12e150
void function_12e150(long tag_index)
{
	D3DDevice_KickPushBuffer();
	D3DDevice_IsBusy();
	if (g_4e648c && g_4e648c->valid)
	{
		s_record_pool_iterator iterator;
		s_cache_block *cache_block;
		s_pending_block *pending;

		iterator.data = g_4e648c;
		iterator.index = NONE;
		iterator.datum_index = NONE;
		while ((cache_block = (s_cache_block *)data_iterator_next_calling(&iterator)) != NULL)
		{
			if (*(long *)&cache_block->block->owner_tag_index == tag_index)
			{
				function_12ddd0(cache_block->block);
			}
		}

		iterator.data = g_4e6490;
		iterator.index = NONE;
		iterator.datum_index = NONE;
		while ((pending = (s_pending_block *)data_iterator_next_calling(&iterator)) != NULL)
		{
			if (*(long *)&pending->block->owner_tag_index == tag_index)
			{
				record_pool_release(g_4e6490, iterator.datum_index);
			}
		}
	}
}

static inline s_cache_block *geometry_cache_block_get(long index)
{
	return &((s_cache_block *)g_4e648c->data)[index & 0xffff];
}

/* whether a cache block is still in use: read this tick, still loading, or read in the last 5 ticks with
   linked resources that are still in use (the call reads the block field again through the pool, as
   retail does) */
// @retail 0x12e210
bool __stdcall geometry_cache_block_busy(long block_index)
{
	s_cache_block *cache_block = geometry_cache_block_get(block_index);
	s_physical_block *physical_block = physical_block_get(block_index);
	long age = physical_object_get()->time - physical_block->time;
	bool result = false;

	if (age == 0)
	{
		return true;
	}
	if (!cache_block->done)
	{
		return true;
	}
	if (age < 5 && cache_block->block->runtime_linked)
	{
		result = fixup_group_has_resource((s_fixup_group *)geometry_cache_block_get(block_index)->block, (byte *)(physical_block->offset << physical_object_get()->page_shift) + g_4e6494);
	}
	return result;
}

// score 27 (was 27): ours loads the pointer into esi where retail uses edx or ecx, and the result fields are
// read in a different order. In the loop retail loads the block into edx and copies it to esi for the
// release call; after the loop ours loads it into edx and copies it to ecx. Tried: an inline helper taking
// the group for the pointer clear 51 (cache block moves to ebp); with a shared address helper 51; the
// count in a local read before the block 68; the index read through its address 51; the block re-read 51;
// alone (no other change): the loop's block read at each use 27, the physical block re-derived into
// its local in the loop 27, the page shift read into a local first 27, the address through an inline
// helper 27 (all identical code); the pointer record cleared by memset 82. Round 31: the release
// call's block re-read through the pool getter (as 0x12e210 does) 115 (the pool is reloaded,
// 293 bytes); a named block local for the test and the pointer after the loop 51 (cache block
// and physical index swap edi/ebp); the release address in a local before the call 27 and a
// group local for the pointer clear 27 (both identical code); the time adjusted through the
// physical block local without the field pointer 45 (the time is loaded, not addressed).
// Round 119 (lane 8, fast): the post-loop unlink as an inline helper taking the group 51 (the tail then
// matches retail, but the cache block and the index*24 product swap edi/ebp); one block local for
// the loop and the tail 51 (same); that local also holding the entry test's block 51 (same); the pool
// entry through the existing getter 27 (same code, kept); the tail's pointer offset through the
// block's own field 27 (same code). One-file LTCG harness (no edi/ebp swap there), each with the tail
// helper: `continue` when unlinked, a cache-block release helper, a group release helper, a named
// linked flag, a derived-struct group, the block through a cast of its field's address: aligned 8
// each, as the helper alone (the in-loop block still goes straight to esi; retail loads edx and
// copies it to esi); the count stored after the block read 32 (the store sinks past the test).
// Round 46 (fast): the loop's block local declared before the loop 27; declared ahead of the tasks
// store 27; the release under `if (!linked) continue;` 27; the address as `(byte *)g_4e6494 + (offset <<
// shift)` 27; the physical block looked up at entry, before the block test, 27 (all identical code). In-loop
// block still goes straight to ESI (retail EDX, copied late).
// Round 95: the cache block's declaration split from its lookup 27; the loop block declared
// before the loop 27; the physical block declared before the block guard 27; the time field
// pointer declared before its assignment 27; the loop block initialized before the loop and
// refreshed in it 27; the tail pointer declaration split 27; the pool release written in both
// block arms 75 at 277 bytes; the parameter renamed by its cache role 27 (all others 265 bytes).
// Round 61 (fast): the count named and stored after the block read 73 (266 bytes: the count moves to EDI, the group keeps ESI); a block local for the tail's test and
// pointer 51 (tail exact, cache block and index*24 swap EDI/EBP); a group local inside the tail's arm 27 (a temp plus a copy, identical), the pointer offset read first 27;
// a group local inside the loop's arm 27 (the copy coalesces into ESI, no EDX), with the tail's block local 51.
// Round 89 (fast): named count with the flag read straight from the block before the store and the block re-read in the arm 18 (266 bytes, aligned 6: the block goes to EDX as retail and the tail is
// exact with no helper, the store stays after the load and the arm reloads; one-use count and bool, so not kept without a match), with `!= 0` 48 (271); count and block named with the flag from it 73
// (266), block named then the store 73 (266); store first with the flag or block and flag named 27/27; loop read at each use 27, with the tail's named block 51; the tail's block named inside the arm 27,
// with the loop's read at each use 27; a group and base named in the arm 27, the base named with the block direct 27; the arm's read as `*(s_fixup_group **)&` 27, as long 27, the test's through a cast
// view 27 (a typed view does not stop the CSE). Diagnostic: the release callee as (base, group) 27 (same code; a contributor's function). Retail's block looks like a scratch temp with the arm's own load forwarded to `mov esi, edx`.
// Round 155 (m3 lane 6, fast): the loop's release through an inline helper taking (block, index) 27; the loop's test and release
// as an inline helper taking (cache block, index) 27 (real build; the harness only before); one physical-object local for the
// loop's blocks and page shift 27; the tail as an inline helper taking the cache block 27; a group local before the tail's test
// for the pointer 51 (edi/ebp swap). Diagnostics: with no release in the loop the tail still loads EDX and copies it to ECX, so
// the tail copy is the tail's own; the swap follows the number of `cache_block->block` reads (the tail's block local plus the
// loop read at each use and the tail's pointer base read through the cache block gives no swap, 38, but reloads the base).
// Round 80 (m3 lane 10, fast): round 89's 18 form rebuilt: once the loop's block load lands in EDX the tail is exact with no other change, so the
// tail's copy follows the loop. The release's block re-read with the address in a local first 27; the test re-reading the block field with the loop
// local kept for the argument 27; diagnostics: the block read before the wait 71 (not retail's order), the count store written in both arms of the
// linked test 18 (266 bytes: the stores merge after the test and the arm reloads the block into ESI). Retail's loop looks like two loads of the block
// field with the second turned into `mov esi, edx` after allocation, as our tail does with `mov ecx, edx`: the loop and the tail trade which pair
// is CSE'd. The release address named before the linked test (not in the arm) 27.
// Round 177 (m3 lane 4, fast): LEAD, what splits the loop's block into EDX and a copy. A block read used once in the arm merges
// with the test's read (one load, straight into ESI); a block value used twice in the arm stays apart from the test's read, and
// the arm's load then becomes `mov esi, edx`. Diagnostic (wrong code, reverted): the loop read at each use with a second arm use
// (`+ cache_block->block->cache_block_index` on the address) and a block local in the tail gives retail's loop and tail line for
// line with EDI/EBP as retail, 41 at 272 bytes, differing only by the added field's own load and add. So retail's arm uses the
// block twice and its tail once (a block local, or one read). A second use in the test's own block instead (`&& flags != 0xff`,
// diagnostic) 55 (269 bytes): no copy, so it has to be in the arm. New, each with the loop read at each use and the tail block
// local: the entry test through the pool getter 81; the last store through the pool getter 109 (286 bytes); the linked test
// repeated inside the loop's arm 51 and an arm block local beside the direct read 51 (both second uses fold away before they
// count); the group passed as the address of the block's first field 51; the tail's offset through the block's own field 51.
// Re-measured: loop read at each use with a tail block local 51 (EDI/EBP swap), with a group local in the tail's arm 27, that
// tail alone 27, a group local in the loop's arm with the tail block local 51. The matched sibling 0x12e210 reads the block
// twice through two lookups (two loads, no copy); the three fixup functions have no caller outside this file.
// Wanted: a second use of the block in the loop's arm that leaves no code of its own.
// Round 85 (m3 lane 14, fast): LEAD, the second use is a separate load. The arm reading the block through
// `&cache_block->block` taken before the loop (the test reading the field directly) gives 14 at 266 bytes: the two loads
// stay apart, as in matched 0x12e210, where the arm's re-read through the getter is its own `mov esi, [edi+8]`. The test's
// block then goes to EDX as retail and the tail is exact. Left: our arm loads ESI from [edi+8] after EDX has been reused
// for the pool data, where retail copies EDX into ESI before that load and reads the page shift first. Not kept (a field
// read through its own address, mixed with direct reads; match-only). On that form, the same code: the group in a local
// before the call 14, the pointer for the test and the direct read in the arm 14, group and base locals 14, the pointer
// taken at the top 14, the tail's two reads through it as well 14; the release callee as (base, group) (a contributor's
// function, measured and put back) 14, same code; the page shift named first 14 (same code); the tail reading the block
// once into a local for its test and pointer 18. On the original body: the block local initialised above the tasks store
// 71 (the block is loaded before the count and held in ESI across the lock calls); `__assume(local_1 != NULL)` in the arm
// 45. A 600 s permuter run on the 14 form found nothing (its reduced image scores that form 73, the full image 13).
// Round 87 (m3 lane 14, fast): the EDI/EBP choice and whether the loop test's block stays apart follow the number of
// reads of the cache block's field in the loop. With a block local for the tail's test and pointer: the loop read at each
// use 51; that with a dead `(void)cache_block->block;` in the arm 51 (same code); with an unused block local in the arm
// (diagnostic, a redundant statement) 18 at 266 bytes: EDI/EBP, the test's EDX and the tail are retail's, and only the
// arm's own `mov esi, [edi+8]` (after the pool data load) stands where retail has `mov esi, edx` before it; a group local
// in the arm 51; the loop's block local kept for the argument with the test reading the field 51; group and base as two
// statements in the arm 51; a block local in the arm with the linked test repeated on it 53 (267 bytes). Compile-only:
// the physical block getter with a pool local or an allocator local gives the same load order. Retail's arm load is a
// second read forwarded from the test's register: it needs a real second use of the block in the arm, ahead of the
// base's loads.
// Round 163 (m3 lane 8, fast with 0x12e210, which stays matched): LEAD, the loop arm reading the block at each use with
// the tail reading it once into a block local (re-measured 51: tail exact, cache block and the index product swap
// EDI/EBP), plus the task count named and stored after the arm's flag read (`long count = 120bf0(); bool linked =
// block->runtime_linked; tasks_added = count; if (linked) ...`) 22 at 266 bytes: EDI/EBP, the flag read into EDX and the
// tail test are retail's; left are the count store after that read (retail before it), the arm's own `mov esi, [edi +
// 8]` after the pool data load (retail `mov esi, edx` before it) and the tail's two stores one rotation step off. Not
// kept: one-use count and flag. On that form: the group read into a local ahead of the address 22, the store written in
// both arms of the flag test 22, the contributor's release callee as (base, group) (diagnostic, put back) 22, all the same
// code. Also: the store first with a named flag 51 (the two reads merge again); a block local in the arm with
// `__assume(block != NULL)` 65.
// @retail 0x12e290
void __stdcall function_12e290(long block_index)
{
	s_cache_block *cache_block = geometry_cache_block_get(block_index);

	if (cache_block->block)
	{
		s_physical_block *physical_block = physical_block_get(block_index);

		long *local_0 = &physical_block->time;
		if (*local_0 == physical_object_get()->time)
		{
			*local_0 = physical_object_get()->time - 1;
		}
		while (geometry_cache_block_busy(block_index))
		{
			async_globals.tasks_added = function_120bf0();
			s_geometry_block_info *local_1 = cache_block->block;
			if (local_1->runtime_linked)
			{
				fixup_group_release_resources((s_fixup_group *)local_1, (byte *)(physical_block_get(block_index)->offset << physical_object_get()->page_shift) + g_4e6494);
			}
		}
		if (cache_block->block->runtime_linked)
		{
			s_fixup_pointer *pointer = (s_fixup_pointer *)((byte *)cache_block->block + ((s_fixup_group *)cache_block->block)->pointer_offset);

			pointer->count = 0;
			pointer->pointer = NULL;
			cache_block->block->runtime_linked = false;
		}
		cache_block->block->cache_block_index = NONE;
	}
	record_pool_release(g_4e648c, block_index);
}

bool game_loop_running();
bool function_138840();
struct s_unknown_13bf00_flags_view
{
	byte unknown00[5];
	bool flag5;
};
struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;

bool g_47ff3c = true;

/* once the geometry cache has run low while playing a campaign, at most once
   a minute: tells the players and frees the cache */
// @retail 0x12e480
void function_12e480(void)
{
	if (g_47ff3c && g_4e64b0)
	{
		if (g_4e64a0 <= 0 && (!g_510c50 || !((s_unknown_13bf00_flags_view *)g_510c50)->flag5) && game_loop_running() &&
			g_4e6948->state == 1 && !function_138840())
		{
			dword time = GetTickCount();

			if (!g_4e64ac || time - g_4e64ac >= 60000)
			{
				long page_count = g_4e649c->page_count;

				if ((real)physical_memory_used_pages(g_4e649c, 5) / (real)page_count < 0.85f)
				{
					if (g_4e6948->state == 1)
					{
						function_24cdaf();
						main_print_message(local_player_first_index(), 0xf0006a3);
					}
					function_12e0c0();
					g_4e64a4 = 0;
					g_4e64a0 = 3;
					g_4e64ac = time;
				}
			}
		}
		g_4e64b0 = false;
	}
}

/* the geometry cache's frame: its memory's clock, the predicted blocks'
   requests and the low-memory message */
// @retail 0x12dd80
void geometry_cache_update(void)
{
	s_physical_object *physical = g_4e649c;

	if (physical)
	{
		if (physical->time == 0x7fffffff)
		{
			physical_memory_reset_time(physical);
		}
		else
		{
			physical->time++;
		}
		for (short i = 0; i < 8; i++)
		{
			physical->limits[i] = 0x7fffffff;
		}
		function_12dc10();
		function_12e480();
	}
}
