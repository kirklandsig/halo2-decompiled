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
// Round 94 (m3 lane 13, fast with 0x12e210, which stays matched): round 85's 14 form re-measured 14 (266 bytes). On it: the
// release address in a local ahead of the linked test 14 (same code: sunk into the arm); round 163's count named and stored
// after the flag read 18 (266 bytes, the store stays after the load); the field pointer declared inside the loop 14; the
// pointer typed `s_fixup_group **` for the argument with no cast at the call 14; the group read through the pointer into a
// local ahead of the linked test 14 (sunk); the time adjusted through the physical block with no field pointer 70 (268
// bytes). In every 14 form the arm's own `mov esi, [edi + 8]` comes after the pool data load into EDX; retail's `mov esi,
// edx` comes before it, as if the argument were set up first and the scheduler then hoisted the pool and shift loads
// above the copy (the data load into EDX cannot cross it). On the base form the address local ahead of the block read
// re-measured 27 (round 80's form).
// Round 98 (m3 lane 14, fast with 0x12e210, which stays matched): round 85's 14 form re-measured 14. To set the group up
// before the address, the release step as an inline helper taking (group, physical index): on the 14 form 14, forced
// inline 14 (both the same code: the arm still reloads ESI after the offset load); on the base body with the field read
// directly 27; a helper taking the block, testing its linked flag and releasing 27; the 14 form with the test and the
// arm each through its own reference to the field 14 (same code).
// Round 130 (m3 lane 12, fast with 0x12e210, which stays matched): the cache block as a const pointer 27; the loop's
// block local as a const pointer 27; the loop test as `busy(...) != false` 27 (all same code); the time test with its
// operands swapped 28 (first difference +0x41); the tail's pointer offset read through the geometry block's own
// section-offset field (no fixup-group cast) 27.
// Round 100 (m3 lane 13, fast with 0x12e210, which stays matched): the task count stored to the file-level counter
// g_4e28f0 (0x12b450's spelling of the same address, never address-taken) in place of the async aggregate's field 27
// (same code: the store's alias status does not move the arm's block load), with the loop read at each use and the
// tail's block local 51; the physical object's time read once into a named local for the compare and the store 27;
// the release address as a named dword inside the arm 27; the wait as a guarded do/while 27 (all 27 the same code).
// Round 128 (m3 lane 16, fast with 0x12e210, which stays matched): the loop's block local as a reference to the field 27
// (same code); one reference to the block field at the guard's top for every later read 51 (EDI/EBP swap, the loop block
// still straight to ESI); the tail's pointer clear through an inline helper taking the group with the loop read at each
// use 51 (tail exact, EDI/EBP swap); both linked tests through an inline accessor taking the cache block 27 (same code);
// the cache block bound by reference for every access 27 (same code).
// Round 174 (m3 lane 7, fast with 0x12e210, which stays matched): the count named, the block read, then the count stored
// to 0x12b450's never-address-taken spelling g_4e28f0 73 (266 B: the count takes EDI, the cache block EBP); round 163's
// named count and flag with the store to g_4e28f0 and a tail block local 51 (the store now moves above the flag read, as in
// retail, but the flag and argument reads merge into ESI again and EDI/EBP swap: with async_globals the store stays below);
// the time decrement as `physical_block->time--` with no field pointer 55; the arm's group read through a second field overlaid
// on `block` (an anonymous union) 27 (same code: the two reads still merge); the time field bound as `long &` 27
// (same code).
// Round 138 (m3 lane 12, fast with 0x12e210, which stays matched; control 27): the loop's linked test through a new
// trivial inline accessor taking the block 27 (same code); the group passed as the address of the block's first field on
// this body 27 (same code); the tail's test and pointer through a block local 51 (tail exact, EDI/EBP swap); the group
// through a fresh `geometry_cache_block_get(block_index)->block` lookup (0x12e210's spelling) 115 (293 B); the arm as a new
// inline helper taking the block by const reference, the group named first inside it, with the test read straight from
// the field 27 (the reference read merges with the test's before allocation).
// Round 140 (m3 lane 12, fast with 0x12e210, which stays matched; control 27): round 85's field-reference form with the
// count stored through g_4e28f0 18 (266 B: the store then sinks below the linked test, the arm still loads ESI late); that
// form with the roles reversed (the test through the reference, the arm reading the field) 14 (266 B, the same code as the
// original 14 form), with g_4e28f0 18, with the physical block local dropped 14; the arm's lookup through the block's own
// `cache_block_index` (0x12de70's apply spelling, a second use of the block in the arm) 71 (the arm forms its own index
// product, EBP unused); the one-use physical block local dropped (time pointer straight from the lookup) 27 and a named
// `linked` flag in the real build 27 (both the same code).
// M3 lane 9 r141 (fast with 0x12e210, which stays matched; control 27, round 85's field-pointer form re-measured 14):
// the group read through the field pointer into a local ahead of the loop's linked test (test direct) 14 (266 B: the
// read still sinks into the arm and loads `[edi + 8]` after the pool data); the test through the field pointer with the
// group read directly ahead of it 14 (same code); that first form with the tail's block local 18; the field-pointer form
// with the tail's block local 18; one field pointer declared in the loop for both the test and the arm 27 (merged, base
// code), with the tail's block local 51 (EDI/EBP swap). Redundant parens, 1-3 deep, around the release address's shifted
// offset or the time decrement 27 each (6 forms, the same code).
// Round 142 (m3 lane 12, fast with every function of this file listed, all 15 others stay matched; control 27): the
// arm's address through a new inline accessor that takes the block table as a parameter 27; the physical object, its
// block table and its page shift named in the arm ahead of the call 27; the address through a new inline
// `physical_block_address(index)` helper 27 (all three the same code: how the base is spelled never moves the pool or
// shift loads). Same-role locals merged or split (after one shared loop index matched 0x12eb00): the loop read at each
// use with a tail block local, re-measured 51, with the physical block local dropped 51, with the time pointer dropped
// (the field written through the block local) 65; ONE block variable declared at the top of the guard and assigned in
// the loop and again before the tail 51, with the physical block local dropped 51, with the time pointer dropped 65;
// the loop read at each use with that shared variable for the tail only 51; the base body with the time pointer
// dropped 45 (17 aligned against the control's 18: the tail's copy goes, the time test reorders). Grid on the
// each-use loop: arm base as accessor-with-table / three named locals / the shift named first, tail block local or
// direct, time pointer kept or dropped: 51/65/51/65/51/65 and 27/45/27/45/27/45 (the arm spelling never matters).
// Diagnostic: 1-6, 8, 10 and 12 dead initialised locals at the top, on the base body 27 each and on the each-use loop
// with a tail block local 51 each (the register ties do not follow the symbol count).
// M3 lane 5 r207 (fast with 0x12e210, which stays matched; control 27): the loop's block local typed as the fixup group
// it is released as, the test casting back 27 (same code); the tail's pointer record cleared by assigning a value-
// initialised record 75 (252 B); a group local for the tail's pointer with the loop's arm reading the field at each use
// 27 (same code); the loop's block local kept for the argument with the test reading the field again, plus a tail block
// local, 51; a tail block local with the loop's arm through a new inline release step taking the cache block 51.
// Diagnostic: an unused block read after the release call 27, with a tail block local 51 (a read after the call does not
// move the tie; the r177/r87 reads stood before it).
// Round 116 (m3 lane 13, fast with 0x12e210, which stays matched; control 27): the physical object and its page shift named
// in the loop's arm ahead of the block lookup 27 (same code), with a tail block local 51 (EDI/EBP swap); the tail block
// local with the loop's argument re-read from the field 51; the loop read at each use with the argument through
// `geometry_cache_block_get(block_index)->block` and the tail block local 115 (293 bytes); the arm's physical block named
// before the call 27 (same code).
// M3 lane 10 r130 (fast with 0x12e210, which stays matched; control 27): the task count named, the block local read, then
// the count stored to the async field 73 (266 bytes: the count takes EDI, the cache block EBP, the 24-byte index EBX); a
// trivial forceinline identity (diagnostic) around the task count, the block read, the busy call's index, the release
// address's index or the time compare 27 each (same code: identities fold before allocation, no rotation step).
// M3 lane 16 r158 (fast with 0x12e210, which stays matched; control 27): the loop's block local passed to the release
// through a `const &` bound after its read 27, bound inside the linked arm 27, the linked flag read through a `bool const
// &` to the field 27 (all the same code); the tail's block read once into a local with a `const &` to it for the pointer
// record 51 (EDI/EBP swap), with the loop's reference as well 51; a second cache-block pointer looked up before the wait
// loop and read in the arm 102 (281 B).
// Round 203 (m3 lane 8, fast with 0x12e210, which stays matched; control 27): READING. With the field read at each use
// (no block local), any second use of the block in the loop's arm before the call gives retail's arm and tail line for
// line (the test's load in EDX, `mov esi, edx` after the page-shift load, the tail's ECX with no copy), and only that
// use's own code differs: `+ block->cache_block_index` on the address 41 (272 B), `+ (dword)block` 36, `+ block->
// block_offset` 40, a probe store of the block ahead of the call 40 (273 B). With a block local none of these split the
// value (42/46/47: it keeps ESI from the test), and a use after the call is a reload (54). So retail's block is the common
// read of the test and the arm with two uses in the arm, the second leaving no code. Forms that add no use, 27 each (same
// code): a `const &` binding of the block local for the test or the argument (four forms), two block locals both read from
// the field, a const reference to the field in the arm, a reference bound to the cast pointer (a temporary), the cast
// through `dword`, the first field's address, the block local assigned again in the arm, the block local declared before
// the loop, `p ? p : NULL` and `linked ? p : NULL` as the argument; a select local (`linked ? block : NULL`, then `if
// (selected)`) 44 (269 B, its own null test); `__assume(block != NULL)` or `__assume(block->runtime_linked)` in the arm 45
// (they move the time test's registers, no split); the count named and stored to g_4e28f0 after a flag read 27 (the store
// moves above the load before allocation); a block local read between the named count and its store 73. Re-measured: the
// count named and stored to the async field after a flag read 18, the arm through a field pointer 14 (the listed forms).
// Round 129 (m3 lane 14, fast with 0x12e210, which stays matched; control 27): the release address from the entry's
// physical block local (no second lookup) 87 (248 bytes: the block pointer is kept, retail re-derives it from the pool);
// the loop's block local bound to a const reference used for the argument 27, used for the linked test 27; the loop's
// block local declared `register` 27; the arm's linked test as `!= false` 27 (all four the same code).
// M3 lane 6 r215 (fast with 0x12e210, which stays matched; control 27): the release through a new inline wrapper taking
// (block, base) 27; the release's group re-read from the field with the test on the loop's block local (no tail local)
// 27; the tail's pointer formed from a second named block read inside its arm 27; `&& local_1` after the arm's linked
// test 27 (all four the same code: the null test after the dereference folds before allocation and is not a second
// use); a group local read before the tail's test and used for its pointer 51 (EDI/EBP swap).
// M3 lane 8 r209 (fast with 0x12e210, which stays matched; control 27): the release call's group re-read from the field
// with the test through the block local 27 (same code); the test reading the field and the block local declared inside
// the linked arm for the argument 27; that first form with a tail block local for the test and pointer 51 (EDI/EBP
// swap, re-measured on the each-use loop as listed); the release address named in the arm ahead of a call that re-reads
// the group from the field 27; the loop read at each use with a tail block local used for every tail access (test,
// pointer, both clears) 55. The texture twin 0x12d200 spells its loop the same way (field test at each use).
// M3 lane 3 r288 (fast with 0x12e210, which stays matched; control 27): the each-use loop with a tail block local
// re-measured 51 (the loop's block still loads straight into ESI, the cache block takes EBP and is pushed at entry), with
// the cache block renamed 51 (names do not move the EDI/EBP tie); the time step-back through a new inline helper taking
// the time field by reference (retail forms the field's address and reads through it) 27 (same code); the each-use loop
// with the release through a new inline helper that tests the linked flag again 44 (269 bytes: the repeated test is
// not folded); the block written back to its field after the loop's read (diagnostic) 51; the guard's block read into a
// named local 27 (same code).
// M3 lane 8 r211: NOT KEPT (M3 review 11-21: a parameter the body never reads and no retail call shows is a steering
// device, banned at review even with the owner's OK; 0x223e00 is also a contributor's function): MATCH (h2m fast, check.py --fast and
// the plain check; all 25 functions of this file and src/unknown_223b60.cpp matched, nothing lost) with the release
// callee taking one more parameter that its body never reads, fed the block, and the loop's test reading the field (no
// block local): `if (cache_block->block->runtime_linked) fixup_group_release_resources((s_fixup_group *)cache_block->
// block, address, cache_block->block);`. The link drops the unread argument, so it leaves no code of its own, but the
// block then has two uses in the arm: the test's load stays in EDX (0x12e31a), the group becomes `mov esi, edx` after
// the page-shift load (0x12e330) and the tail reads ECX with no copy. This is the wanted second use of round 177.
// 0x223e00 keeps its bytes (group ESI, base EAX). The unread parameter declared first instead of last matches as well;
// with main's block local for the test and both arguments 27 (same code as main: a local's two uses do not split).
// Patch: build/r211/held-12e290.patch (definition, header declaration and this call). Code left as on main.
// M3 lane 1 r145 (h2m fast with 0x12e210, which stays matched; control 27): the guard as an early return for an empty
// cache block 103 (279 B, the release duplicated); the arm's physical object named once for its block table and page
// shift 27 (same code); that with the field read at each use and a tail block local read through the geometry block's
// own section-offset field 51 (EDI/EBP swap); the page shift named first in the arm 27 (same code); the loop's block read
// through a fresh pool lookup of the cache block 114 (291 B).
// M3 lane 8 r216 (h2m fast with 0x12e210, which stays matched; control 27, the each-use loop re-measured 27): the cache
// block's field bound once as `s_geometry_block_info *&` at the top and used for the guard, the loop and the tail 81 (first
// difference +0x9); that reference bound only in the loop's arm with the field read through it 27 (same code); the loop's
// release step as a new forceinline helper taking (cache block, block index) and reading the field at each use 27 (same
// code); the loop as `if (busy) do { ... } while (busy);` 27 (same code); the tail's root clear through a new forceinline
// helper taking the group (offset read, count and pointer cleared) 51 (+0x9), with the each-use loop 51.
// M3 lane 4 r243 (h2m fast with 0x12e210, which stays matched; control 27; the reduced image builds 0x223e00 differently,
// so these are screens): round 89's form (the count named, the linked flag read from the field ahead of the count's store,
// the arm re-reading the field) re-measured 18 (266 B: the block loads into EDX as retail, but the store stays after the
// test and the arm reloads ESI where retail copies EDX); the group named from the field ahead of the linked test, the test
// reading the field, 27 (same code); a group local for the tail's pointer with the tail's test on the field 51, that with
// the loop's group form 51, with the loop read at each use 51 (EDI/EBP swap in all three). Diagnostics (a probe global,
// removed): the count stored to a never-address-taken global 27 on main's body and 27 on round 89's form (the store is then
// scheduled ahead of the load and the block goes straight to ESI), so the count's store order is not kept by the async
// aggregate's taken address.
// M3 lane 1 r150 (h2m fast with 0x12e210, which stays matched; control 27): the arm as `if (!local_1->runtime_linked)
// continue;` then the release 27 (same code); the block read ahead of the task-count store 71 (first difference +0x9);
// the busy result in a named flag re-tested at the loop's end 27 (same code); a second named pointer `group` copied from
// the block local and passed to the release 27 (same code: the copy folds, no second use); the loop test as `busy(...)
// == true` 28.
// M3 lane 6 r226 (h2m fast with 0x12e210, which stays matched; control 27): `__assume(local_1 == cache_block->block);`
// in the loop's arm 45 (first difference +0x23: the time test's registers move, the block still goes straight to ESI);
// `__assume(local_1->cache_block_index == block_index);` there (diagnostic) 45; the page shift named ahead of the
// linked test 27, the arm's physical offset named there 27 (both sink into the arm, same code); the block bound as an
// object reference for the test and `&block` 27 (same code).
// M3 lane 7 r213 (h2m fast with 0x12e210, which stays matched; control 27; screened on a multi-object LTCG harness that
// reproduces this function exactly): 540 combinations of the pre-loop time fix, the loop's block spelling (named, per
// use, a group local, `continue`, declared ahead of the task store), the release address and the tail spelling. Every
// loop spelling gives the same code; the tail comes out retail's (the block straight into ECX, no copy) whenever the
// time pointer local is gone, but retail's pre-loop `lea eax, [edx+ebp+0x14]` needs that pointer (or a reference).
// Forms: the time fix through the field 45 (tail retail's, pre-loop and scratch rotation shift); that with a group local
// in the tail 45; the time fix as `physical_block->time--` 55; the loop's offset read through the physical block local
// 87 (248 B); the tail's offset through the geometry block's own +0x1c field 27 (same code, the copy stays).
// M3 lane 2 r137 (h2m fast with 0x12e210, which stays matched; control 27): the time fix as `*local_0 = *local_0 - 1;`
// 57 and as `*local_0 -= 1;` 57 (first difference +0x23: the field is reloaded); the release address summed base first
// 27, as one integer sum cast once (0x12e3a0's spelling) 27, base first with the loop reading the field at each use 27
// (the three the same code).
// M3 lane 14 r144 (h2m fast with 0x12e210, which stays matched; control 27, round 85's field-pointer form re-measured 14): the
// release address as one integer sum cast at the call 27, as `(byte *)g_4e6494 + (offset << shift)` 27, that on the 14 form
// 14; on the 14 form a new forced-inline release step taking (physical index, group) 14 (the arm's own load still comes
// after the pool data and offset loads); the group or the linked test through a `const &` bound to the block local 27/27
// (all the same code as their bases).
// M3 lane 8 r227 (h2m fast with 0x12e210, which stays matched; control 27): a null test ahead of the arm's linked test
// (`local_1 && local_1->runtime_linked`) 45 (269 B: the calls in the loop keep the test); a group pointer named from the
// field ahead of the test, the test on the field and the release base named in the arm 27; the release base named ahead
// of the linked test 27 (both the same code: the base's loads sink into the arm); the busy function's spelling (test on
// the local, the group through a fresh `geometry_cache_block_get(block_index)->block`) 115 (293 B); 0x12de70's apply
// spelling for the base (`physical_block_get(local_1->cache_block_index)`) 71.
// M3 lane 4 r250 (h2m fast with 0x12e210, which stays matched; control 27, round 85's field-pointer form re-measured 14
// at 266 B): the arm's block through a `s_geometry_block_info *const &` bound to the field before the loop 14 (the same
// code as the field pointer, match-only); the field pointer declared inside the loop for the arm only 27; the loop's
// block local declared `*const` 27; the linked test as `!= false` 27; the loop's block read through the cache-block
// getter 114 (291 B).
// M3 lane 13 r146 (h2m fast with 0x12e210, which stays matched; control 27): the release callee's two parameters declared
// (base, group) at its definition, declaration and this call (retail sets the group register up first; diagnostic only, the
// callee is a contributor's) 27 (the same code; the callee 0x223e00 and 0x12de70/0x223c70/0x223d80 stay matched); the time
// fix through a `now` local read once, with the field pointer 27 (same code), with the field written at both uses and no
// pointer 45; an identity forceinline pointer view (no code) around the arm's group argument 48 (272 B), around the arm's
// linked test 48 (272 B).
// M3 lane 1 r169 (h2m fast with 0x12e210, which stays matched; control 27): a forced-ranking search over 183 single
// allocator decisions on this body and 60 on round 85's field-pointer form (14) finds nothing below either count, so
// what is left is not a ranking tie but the arm's missing second use of the block. Forms: the task count named and
// stored at the end of the loop body 68 (266 B); the release's physical block named inside the arm 27; the arm's
// release reading the guard's physical block local re-fetched in place 27; one physical-object local at the guard's
// top for the time fix and the arm's page shift 84 (266 B); the loop's block local as a const pointer to const 27.
// M3 lane 4 r262 (h2m fast with 0x12e210, which stays matched; control 27, round 85's field-pointer form re-measured 14
// at 266 B). Tracer reading: on this body the loop's block (one value for the test and the release) is coloured after
// the task count (56 against 44) and takes ESI by its call-argument preference, as the count is dead by then. On the 14
// form the arm's second load is a value of its own, and the code generator emits it after the address chain (last in
// its scheduler region), when EDX already holds the pool data, so it stays a load; retail's `mov esi, edx` before
// `mov edx, [eax + 0x44]` reads as a copy made before that chain (it delays the data load by an anti-dependence).
// Forms: the count named and stored after the block read (the count then live across it) 73 (266 B: the block's ESI
// preference pushes the count to EDI and the cache block to EBP), that with the flag read into a named bool ahead of
// the store 73; the count stored after the read with the arm on the field pointer 39 (268 B: count ESI as retail, the
// test's block in EAX and its flag in CL, the store still after the read), with the arm reading the field directly 39;
// the arm's block through the cache-block getter (0x12e210's spelling) 115 (293 B), with the count stored after the
// read 115. The release callee is a contributor's /Ob1 function: an inlined callee taking the block (whose parameter
// copy would be retail's `mov esi, edx`) is not available.
// M3 lane 2 r153 (h2m fast with 0x12e210, which stays matched; control 27): the release base from the guard's physical
// block local 87 (248 B, the pointer is held across the loop); the time fix reading the physical block getter at each use
// with no local 27 (same code); a physical block local inside the release arm 27 (same code); the loop reading the block
// field at each use with the base named in the arm and the tail through one block local 51 (EDI/EBP swap); that with a
// group named in the arm instead 51.
// M3 lane 12 r194 (h2m fast with 0x12e210, which stays matched; control 27): the arm's group and base both named inside
// it, group first, then the call on the two names 27; the loop's test and argument both read from the field (no block
// local) with the tail unchanged 27; one physical object local at the guard's top for the time test and its store 27;
// the arm's page shift read from the physical object global directly 27; the block local declared bare at the loop
// top and assigned after the count's store 27 (all five the same code).
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
