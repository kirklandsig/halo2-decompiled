#include "unknown_11c920.h"
#include <string.h>
#include <xmmintrin.h>
#include "globals.h"
#include "data_array.h"
#include "physical_memory.h"

// @flags /O2 /arch:SSE /Gr

/* the physical memory allocator (physical_memory.h): a named, doubly linked
   list of blocks kept in a data array that follows the object */

// @retail 0x13d170
void function_13d170(s_physical_object *manager, const char *name, long page_count, long page_shift, long maximum_count, physical_block_delete_proc delete_proc, physical_block_busy_proc busy_proc, physical_block_state_proc state_proc, c_data_allocator *allocator)
{
	s_record_pool *data = (s_record_pool *)(manager + 1);

	function_16b5f0(data, name, maximum_count, sizeof(s_physical_block), 0, g_46875c);
	data->valid = 1;
	record_pool_release_all(data);

	memset(manager, 0, sizeof(s_physical_object));
	strncpy(manager->name, name, 0x20);
	manager->name[0x1f] = 0;
	manager->delete_proc = delete_proc;
	manager->busy_proc = busy_proc;
	manager->state_proc = state_proc;
	manager->page_count = page_count;
	manager->page_shift = page_shift;
	manager->first = NONE;
	manager->blocks = data;
	manager->time = 1;
	manager->signature = 0x77656565;
	manager->state = 0;
	manager->allocator = allocator;
	manager->last = NONE;
	for (long i = 0; i < 8; i++)
		manager->limits[i] = 0x7fffffff;
}

// @retail 0x13d230
void physical_memory_reset_time(s_physical_object *physical)
{
	s_record_pool_iterator iterator;
	s_physical_block *block;

	iterator.data = physical->blocks;
	physical->time = 0x101;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while ((block = (s_physical_block *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		dword age = 0x7fffffff - block->time;

		if (age > 0x100)
		{
			block->time = 0;
		}
		else
		{
			block->time = 0x100 - age;
		}
	}
}

// @retail 0x13d2b0
void physical_memory_flush(s_physical_object *physical)
{
	s_record_pool_iterator iterator;

	iterator.data = physical->blocks;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while (data_iterator_next_inlined(&iterator))
	{
		physical->block_delete(iterator.datum_index);
	}
}

/* frees a block: the owner's delete callback, then unlinks it */
// @retail 0x13d830
void s_physical_object::block_delete(long handle)
{
	s_physical_block *entry = &((s_physical_block *)blocks->data)[handle & 0xffff];

	if (delete_proc)
		delete_proc(handle);

	if (entry->previous != NONE)
		((s_physical_block *)blocks->data)[entry->previous & 0xffff].next = entry->next;
	else
		first = entry->next;

	if (entry->next != NONE)
		((s_physical_block *)blocks->data)[entry->next & 0xffff].previous = entry->previous;
	else
		last = entry->previous;

	record_pool_release(blocks, handle);
}

PRIVATE __forceinline byte *function_13d8b1(s_record_pool_iterator *arg_1)
{
	s_record_pool *local_1 = arg_1->data;
	long local_2 = data_find_index(local_1, arg_1->index + 1);
	byte *local_3;
	if (local_2 != NONE)
	{
		local_3 = local_1->data + local_1->size * local_2;
		arg_1->index = local_2;
		arg_1->datum_index = (*(short *)local_3 << 16) | local_2;
	}
	else
	{
		arg_1->index = local_1->maximum_count;
		arg_1->datum_index = NONE;
		local_3 = 0;
	}
	return local_3;
}

/* resizes the allocator to a number of pages, freeing the blocks past its end */
// @retail 0x13d8b0
void s_physical_object::method_13d8b0(long pages)
{
	s_record_pool_iterator iterator;
	s_physical_block *block;

	iterator.data = blocks;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while ((block = (s_physical_block *)function_13d8b1(&iterator)) != NULL)
	{
		if (block->offset + block->pages > pages)
		{
			block_delete(iterator.datum_index);
		}
	}
	page_count = pages;
}

// @retail 0x13d950
long physical_memory_used_pages(s_physical_object *physical, long age)
{
	long result = 0;
	s_record_pool_iterator iterator;
	s_physical_block *block;

	iterator.data = physical->blocks;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while ((block = (s_physical_block *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		if ((dword)(block->time + age) >= (dword)physical->time || physical->busy_proc && physical->busy_proc(iterator.datum_index))
		{
			result += block->pages;
		}
	}
	return result;
}

struct s_physical_block_view;
bool physical_memory_candidate_is_better(s_physical_object const *physical, s_physical_block_view const *a, s_physical_block_view const *b);

struct s_block_candidate
{
	long previous;
	dword time;
	long offset;
	long pages;
};

// score 370 (was 370): Frame 0x1050 versus 0x1054; page-offset/ring-tail allocation and policy blocks differ.
// Round 32: retail lays the segment arms out in source order (none, block, gap, then the scan), but
// ours moves the else arm past the scan; retail also uses the page offset (EBX) as its zero from
// the limit clamp on, so EBX is pushed first. Candidate stores offset before time 370; busy as
// `proc && proc(current)` 376; offset declared with the result at entry 400 (EBX/ESI prologue as
// retail, offset in EDI); arms inverted (`current != NONE` first) 367 at 1213 bytes (three-point
// gain dropped; the none arm then moves past the scan); inverted with early offset 382.
// tail and keeps page offset in EBX. Limit-first guard 385; oldest timestamp
// initialised 381; enclosing inner-next scope 370; short ring endpoints 392;
// candidate copy through memcpy 370.
// Bounded ring masks 387; unsigned-word endpoints 392; frame pointer 475;
// zero inline depth 418; size preference 643 (fast).
// Native busy result 370; native selection result 373; gap endpoint snapshot 370;
// page cursor published first 372; byte ring tail 432 (fast).
// Round 210: Word best-present/busy/policy states 372/373/374; byte ring head 393; function-
// scope deletion iterator 370. Frame 0x1050 versus 0x1054 and page/tail allocation remain.
// Round 212: Shared page shift 370; byte limit category 418; unsigned-word inner/insertion-next/successor cursors 374/375/383. Frame and page/tail homes remain.
// Frame 0x1050 versus 0x1054; retail spills the ring tail and keeps page offset in EBX.
// Limit-first guard 385; oldest timestamp initialised 381; enclosing inner-next scope 370; short
// ring endpoints 392; candidate copy through memcpy 370. Bounded ring masks 387; unsigned-word
// endpoints 392; frame pointer 475; zero inline depth 418; size preference 643 (fast). Native busy
// result 370; native selection result 373; gap endpoint snapshot 370; page cursor published first
// 372; byte ring tail 432 (fast). Tail reset 375; candidate while 370; short inner cursor 374;
// timestamp maximum 367 (three-point gain dropped); word limit index 386; maximum plus tail reset
// 373; maximum plus word limit 381; maximum plus all ring resets 368 (fast).
// Round 214: byte insertion successor 375; byte inner successor 383; byte inner ordinal 389; byte best-present state 370; byte policy state 370.
// Native pointer ring: indexed-source-equivalent for/while, mutable/immutable successor and branch/function
// array lifetimes 382/382/380/380/382/382/380/380. Original body and helpers restored.
// Negative best-candidate bool/byte/short/long in admitted scope 371/371/373/373;
// the same carriers from entry 379/379/380/380. Original positive best state and body restored.
// Cursor declaration orders 102435/032145/042315/052341 382/370/369/377
// (head, tail, offset, previous, current, oldest); four combined cursor names 370 each.
// Round 176: Unsigned previous/current/oldest, every pair/all three, and all
// three plus unsigned allocation result: 370 each (eight forms). Equality-first
// time policy bool/byte/short/long 385/385/381/383; non-strict admission
// 370/370/374/373. Frame/page-tail homes persist; all original code restored.
// Round 235: Complementary availability bool/byte/short/long at segment entry
// 376/376/387/385; constant default before ring insertion 385/385/399/397.
// Frame and page/ring allocation remain; original busy state and complete body restored.
// Round 237: Negative policy comparison bool/byte/short/long inside admission
// 376/376/374/375; defaults before admission 376/376/374/375. Original policy state restored.
// Round 194: Occupied-first short-circuit callback with initial/segment-owned defaults
// 379/379; conditional callback 366/366. Comparator inline/noinline on original
// 370/370, on occupied-first conditional form 366/366. Original-order callback
// forms 376/376/371/371. Four-point gain at retail length rejected; original code restored.
// Round 148: insertion candidate reference 370; scanned candidate reference 370; allocated block
// reference 370; both candidate references 370; insertion/allocated references 370;
// scan/allocated references 370; all object references 370; all references/readonly block/policy
// inline 370; isolated policy inline 370. All executable forms restored.
// Round 226: Native inserted candidate initializer 387; zero-initialized best 398; both 398;
// native deletion iterator initializer 370; insertion/iterator 387; best/iterator 398;
// all three 398; all with entry best initialization 417. Frame/page-tail homes remain.
// Round 162: Coordinated policy parameter orders 021/102/120/201/210 370 each; individual
// physical/candidate/best mutable-pointer input declarations 370 each. Policy body and all matched
// controls stay exact; original declarations and call restored. Frame 0x1050 versus 0x1054 and
// page/ring homes remain.
// Round 111: arms inverted 367 at 1213 B again (retail keeps the none arm first, so only a length
// coincidence); current and offset initialised at entry 399; 61 orders of the six cursor
// declarations (fast) 369-383; default case first 370 (our case 0/1 bodies stay at the function's
// end, retail keeps them in the loop); case 1 as `if (...) better = true;` 382. Retail keeps the
// offset in EBX through the scan and spills `next` (+0x4c); ours keeps `next` and the tail.
// DL190: Existing calling iterator with fields/aggregate 424/424; direct step 418/418; native datum iterator 409/404 (frame 0x1054, but out-of-line). Allocated lookup/absolute getter 367/367 at retail length (three-point gains rejected). Original iteration, allocation and body restored; page/ring carriers remain.
// Round 228: earlier head retirement/tail commitment/both 369/370/369; too-small
// candidate continue alone/with head/with tail/with both 370/381/370/386; all plus timestamp
// maximum 367 at retail length (three-point lead rejected). Original body restored.
// Round 237: Computed page-size long/dword, each with named-mask expression, original/entry constant result lifetime: 370/370/370/370/413/413/413/413.
// Original executable body restored.
// Round 27 (lane 14), aligned counts in brackets (control 370 [355]): busy handled at the end of the block arm
// (`if (busy) { head = tail; continue; }`, scan unconditional) 377 [252] at retail length 1213, retail's
// none/block/gap arm order; with busy = proc && proc(current) 377 [239]; that with the policy cases in
// default/1/0 source order 377 [239] (same code); with offset = 0 declared at entry 380 [257] (EBX pushed
// first as retail); with the allocation success returning directly 381 [244]. On the 239 form, retail still
// lays out the no-candidate limit update + return right after the loop guard and keeps the case 0/1 bodies in
// the loop (ours puts both at the end), and uses its zero EBX as offset (frame 0x1050 versus 0x1054).
// Round 242: Actual queued-candidate count long/dword/short/word, capacity-bound and
// not-full admission 407/407/406/418/407/407/406/418. All recover frame 0x1054 but
// grow code; original ring state and body restored, page-offset/tail homes remain.
// Round 8 (lane 17): the segment arms as an else-if chain 370 (same code); busy as a ternary on the callback 371;
// segment pages/time declared before the while 370; `if (!busy) scan else head = tail` 397 (1181 bytes); busy
// assigned in each arm without an initializer 369.
// Round 49 (m3 lane 1, fast): lane 14 round 27's busy form rebuilt (busy decided in the in-block arm,
// `if (busy) { head = tail; continue; }`, the scan unconditional) 377 at 1213 bytes (aligned 355 -> 240);
// with busy as `proc && proc(current)` 377 (aligned 238); on that: the new candidate's fields stored
// previous/offset/time/pages (retail's store order) 377 (on the original 370, same code), segment pages/
// time declared before the while 377, the scan as `for (...; i = next)` 377 (all aligned 238). Left there:
// retail holds offset in EBX from entry (its zero for the type clamp, so EBX is pushed first and `next` is
// spilled at +0x4c: frame 0x1054), stores the zero segment time in each arm and keeps current in EDI.
// Round 258: Native policy if/else dispatch 0/1/default and 1/0/default, each with
// bool/byte/short/long results, 385/385/384/381/400/400/396/394. Original switch, result domain and body
// restored; frame 0x1050 versus 0x1054 and page/ring homes remain.
// Round 35 (m3 lane 2, fast): lane 14's busy-in-arm form (`if (busy) { head = tail; continue; }` in the block
// arm) rebuilt 377 at 1213 bytes; on it, the allocation as a single exit (`if (result != NONE) link; else if (type
// == limit_index) clamp;`, no goto) 377 (same code; 370 on the original, same code), with the clamp arm first
// (`if (result == NONE)`) 393 (1197 bytes); on the original: the new candidate's fields written through
// `candidates[tail]` 371, the scan's candidate read through `candidates[i]` 397 (1244 bytes), the loop test as
// `page_count > offset` 370 (same code). Retail places the zero-trip limit update right after the loop guard
// (`jg` into the body, the update falling through); every form here keeps it at the end.
// Round 275: Reused candidate pages/time/both/best fields/pages-best/time-best/all 369/367/367/370/367/367/367; all with immutable successor 367. One/three-point leads rejected.
// Original executable source restored; frame 0x1050 versus 0x1054, page/ring homes and policy blocks remain.
// Round 277: Existing private iterator with field/aggregate initialization and candidate
// assignment/memcpy 370 each; each with native successful-datum lookup 367 at 1213 bytes
// (three-point gains rejected). Original helper use, iterator, lookups and body restored;
// frame/page-tail homes and policy blocks remain.
// Round 131 (m3 lane 6, fast): `long offset = 0;` as the first statement 373 (push/pop order and the pages
// register become retail's, the zero moves to EDI, frame still 0x1050); offset declared after the result,
// before the clamp 400 (1231 bytes); the clamp as an if/else-if chain 370 (same code); both 373; offset
// first with the candidate stored previous/offset/time/pages 372.
// @retail 0x13d370
long __stdcall function_13d370(s_physical_object *physical, long size, long type)
{
	long pages = size >> physical->page_shift;
	if (size & ((1 << physical->page_shift) - 1))
		pages++;
	long result = NONE;
	long limit_index = type < 0 ? 0 : (type > 8 ? 8 : type);
	if (pages < physical->limits[limit_index] || limit_index <= 0)
	{
		s_block_candidate candidates[256];
		s_block_candidate best;
		bool have_best = false;
		long head = 0;
		long tail = 0;
		long offset = 0;
		long previous = NONE;
		long current = physical->first;
		long oldest = NONE;
		dword oldest_time;
		while (offset < physical->page_count)
		{
			long next_tail = (tail + 1) % 256;
			if (next_tail != head)
			{
				s_block_candidate *candidate = &candidates[tail];
				candidate->previous = previous;
				candidate->time = 0;
				candidate->offset = offset;
				candidate->pages = 0;
				tail = next_tail;
			}
			long segment_pages;
			dword segment_time;
			bool busy = false;
			if (current == NONE)
			{
				segment_pages = physical->page_count - offset;
				segment_time = 0;
				offset = physical->page_count;
			}
			else
			{
				s_physical_block *block = &((s_physical_block *)physical->blocks->data)[current & 0xffff];
				if (offset == block->offset)
				{
					segment_pages = block->pages;
					segment_time = block->time;
					if (physical->busy_proc)
						busy = physical->busy_proc(current);
					if ((dword)(block->time + type) >= (dword)physical->time)
						busy = true;
					else if (!busy && (oldest == NONE || (dword)block->time < oldest_time))
					{
						oldest = current;
						oldest_time = block->time;
					}
					previous = current;
					current = block->next;
					offset = block->pages + block->offset;
					_mm_prefetch((char *)&((s_physical_block *)physical->blocks->data)[current & 0xffff], _MM_HINT_T0);
				}
				else
				{
					segment_pages = block->offset - offset;
					segment_time = 0;
					offset = block->offset;
				}
			}
			if (busy)
				head = tail;
			else
			{
				for (long i = head; i != tail; )
				{
					s_block_candidate *candidate = &candidates[i];
					long next = (i + 1) % 256;
					_mm_prefetch((char *)&candidates[next], _MM_HINT_T0);
					if (segment_time > candidate->time)
						candidate->time = segment_time;
					candidate->pages += segment_pages;
					if (candidate->pages >= pages)
					{
						bool better = false;
						if (have_best)
						{
							switch (physical->state)
							{
							case 0:
								better = candidate->time < best.time || (candidate->time == best.time && candidate->pages < best.pages);
								break;
							case 1:
								better = physical_memory_candidate_is_better(physical, (s_physical_block_view *)candidate, (s_physical_block_view *)&best);
								break;
							default:
								{
									dword age = physical->time - candidate->time;
									dword best_age = physical->time - best.time;
									age = age > 0 ? age : 0;
									best_age = best_age > 0 ? best_age : 0;
									better = (long)(best.pages * age) > (long)(candidate->pages * best_age);
								}
								break;
							}
						}
						if (!have_best || better)
						{
							best = *candidate;
							have_best = true;
						}
						head = (head + 1) % 256;
					}
					i = next;
				}
			}
		}
		if (have_best)
		{
			s_record_pool_iterator iterator;
			iterator.data = physical->blocks;
			iterator.index = NONE;
			iterator.datum_index = NONE;
			s_physical_block *block;
			while ((block = (s_physical_block *)data_iterator_next_inlined(&iterator)) != NULL)
			{
				if (block->offset < best.offset + pages && block->offset + block->pages > best.offset)
					physical->block_delete(iterator.datum_index);
			}
			if (physical->blocks->actual_count == physical->blocks->maximum_count && oldest != NONE)
			{
				if (best.previous == oldest)
					best.previous = ((s_physical_block *)physical->blocks->data)[oldest & 0xffff].previous;
				physical->block_delete(oldest);
			}
			result = record_pool_allocate(physical->blocks);
			if (result != NONE)
			{
				s_physical_block *blocks = (s_physical_block *)physical->blocks->data;
				s_physical_block *entry = &blocks[result & 0xffff];
				if (best.previous == NONE)
				{
					entry->previous = NONE;
					if (physical->first == NONE)
						physical->last = result;
					else
						blocks[physical->first & 0xffff].previous = result;
					entry->next = physical->first;
					physical->first = result;
				}
				else
				{
					s_physical_block *before = &blocks[best.previous & 0xffff];
					if (before->next == NONE)
					{
						entry->previous = physical->last;
						physical->last = result;
					}
					else
					{
						s_physical_block *after = &blocks[before->next & 0xffff];
						entry->previous = after->previous;
						after->previous = result;
					}
					entry->next = before->next;
					before->next = result;
				}
				entry->offset = best.offset;
				entry->pages = pages;
				entry->time = physical->time;
				goto done;
			}
		}
		if (type == limit_index)
			physical->limits[limit_index] = physical->limits[limit_index] > pages ? pages : physical->limits[limit_index];
	}
done:
	return result;
}

