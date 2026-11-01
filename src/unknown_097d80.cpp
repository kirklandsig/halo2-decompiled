#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_096ed0.h"

// @flags /O2 /Gr

/* ---- class A: an aggregate of three child objects ---- */
struct c_child;

struct s_block_item
{
	c_child *object;
	long unknown04;
};

struct s_block
{
	byte unknown00[0x3c];
	long count;
	long unknown40;
	s_block_item items[7];
	long unknown7c;
};

struct c_child
{
	virtual void v0(long, long) {}
	virtual bool v1(c_child *, long *) { return false; }
	virtual void v2() {}
	virtual void v3() {}
	virtual void v4() {}
	virtual long v5(dword, void *, long, s_block *, long *) { return 0; }
	virtual void v6(s_block *) {}
	virtual void v7(void *) {}
	virtual void v8(long, long) {}
};

struct s_request
{
	byte unknown00[0xc];
	long type;
};

struct c_aggregate : c_child
{
	byte unknown04[8];
	c_child *children[3];

	void v7(void *a);
	void v8(long a, long b);

};


// score 31 (was 31): the release reads the allocator global again (allocator_globals_release, retail's
// reload into eax then edi) at retail's length; the dispatch loop reads its child by index as the collection
// loop does, which gives retail's request/child cursor registers (EBX/EBP), and declares both bounds in its
// for-init, which gives retail's starts[i]/starts[i + 1] load order; left: only the frame, 0x8024 against
// 0x8020 (`produced` keeps its own slot where retail packs the loop-2 cursor and loop-3 count into it, and the
// two temporary slots are swapped). This body built without /GL is byte-exact (BLOCKED by the build flag). Compiled without /GL (screen only) the body
// gets retail's 0x8020 frame and every slot, 20 left (EBX/EBP and the boundary loads); every /GL form
// screened keeps 0x8024.
// Earlier (74): the dispatch loop walks the child cursor and the starts[] boundaries as retail does
// (index in ebp, child cursor and inner count spilled), so the two children are visited in retail order.
// Tried: ordinal/child-cursor dispatch 115; explicit inner length on ordinal/range forms 102/99;
// guarded count-down block traversal 130; shared output/output as count/result 84/84/84; produced count
// reused as cleanup total 130; cumulative boundaries with carried total/direct table/local output/table
// cursor, original and ordinal dispatch, 134/155, 140/157, 132/141, 142/157 (direct boundary outputs
// recover frame 0x8020 but lengthen dispatch); release through the file-local release helper of
// unknown_096ed0.cpp (global read again after the query) 102f, with an indexed dispatch 119f, with a
// child cursor 119f; completion output at loop scope 113f; helper with an object local 102f; output/error
// names and scopes (36 screened) don't move the frame. Kept: item test on the field and the allocator
// global held across the release 85. Global held across the release only 100; indexed dispatch 104, with
// the hold 115; class operator delete 100, with indexed dispatch 115; release through a local helper
// taking the globals 103; field test with indexed dispatch 119; completion output renamed 92 (frame
// unchanged); held global with the item test on the local 106. On that body: indexed dispatch 115,
// reading the child at each use 128; completion output at loop or function scope 96/96; total declared
// before error 85. Cleanup block/item/both cursors 104/104/104; child-boundary cursor 99; indexed
// child/range 106; shared child base 133, with range 153; collection rejection 116; read-only range 99;
// range-array bound 92; child-array bound 139; read-only range bound 92; repaired-entry completion,
// release and both output names 92/92/92. Completion/information/both output lifetimes 105/99/110;
// cleanup block/item cursors in the boundary-array context 92/92; actual dispatch block range 103;
// nested error scope and early error/total results 92/92. count name 104; release-output name 104; both
// names 104; result-first 104; arrays swapped 104; collection cursor 105; indexed dispatch 104; allocator
// capture 115. Error identity 92; total before error 92; error-first gate 98; cleanup null exit 92;
// proven nonempty count 92; cleanup ordinal names 92; unsigned fixed collection 93; exact boundary-end
// test 92. produced after arrays 105; produced after counters 105; information after arrays 99; both
// after arrays 110; collection boundary cursor 148; shared boundary cursor 148; paired collection
// cursors 132; boundary lifetime 92. Concrete collection/dispatch class and allocation-pointer
// combinations 92 each; shared typed table 150; common child-interface collection/dispatch/both/both
// with allocation pointer 92 each.
// Round 186: Native stream contract alone/allocation pointer 85/85; iteration output/both 96/96; native
// contract with ordinal dispatch alone/allocation 115/115, with output/both 125/125. Existing shared
// release helper with default/force-inline linkage and local/direct-field input 102 each; real starts
// initialized on those four forms 151 each. Per-child counts with ordinal/direct child, ordinal/child
// cursor, count-table end and child-table end 159/159/142/181; aggregate-zero equivalents
// 183/183/148/206 (ordinal forms recover frame 0x8020 but lengthen dispatch).
// Round 292: Allocator information output initialized to zero or NONE at its producer, with
// query/block/after-array/before-array declaration lifetimes: eight at 100/377.
// Round 86 (on the release form): produced at function scope 55, outside the test 59, renamed 45;
// starts store first 45; indexed test 55; error/total and blocks/starts declaration swaps 45/45;
// boundaries read in the inner loop's test 121; boundaries swapped 45; child-cursor walk 45; the
// query and release through an inline free-block helper 45.
// N227: Published absolute child origin at capacity/output-address/completion consumers,
// seven subsets 45 each; complete graph with final boundary as cleanup limit 45. Original
// produced output, cumulative publications, dispatch and release graph restored.
// Round 43: produced renamed count/added/block_count and the dispatch cursor renamed cursor/slot/entry
// 45 each (frame 0x8024 unchanged); one function-scope child cursor walked in both loops 56.
// seven subsets 45 each; complete graph with final boundary as cleanup limit 45.
// Round 27: the child read from the member at each use in the collection loop 62; the dispatch loop
// started from starts[i] with only the end named 45 (same code); an item pointer in the cleanup loop 45;
// one child cursor at function scope walked by the collection and dispatch loops 56; the error test
// before the child test 50.
// Round 3 (lane 18): retail's slots are produced/loop-2 cursor/loop-3 count at 0x10, children base/loop-2
// count/loop-3 block at 0x14, error 0x18, result 0x1c; ours give the loop temps two slots of their own
// (0x10/0x14) and keep produced (0x1c) apart. produced assigned in its own statement 45; declared in the
// loop body ahead of the child 55; `blocks + total` 45; `!error` gate 45; result declared ahead of the
// item test 45; produced in a block closed around the collection loop 55; produced declared and zeroed at
// the top of every loop pass 143 (389 bytes). The frame stays 0x8024 in each.
// Round 48: under LTCG no form lets the address-taken `produced` share a slot (an LTCG mini-link
// keeps it apart in every variant; dropping `&result` alone gives 0x8020 with produced still apart),
// where a /GL- build packs it with the loop-2 cursor and loop-3 count as retail does. Diagnostic, at
// once reverted: the file built /GL- gives this function 20 but costs 0x97f60, 0x97fe0 and 0x980d0
// their matches, so it needs a non-LTCG object of its own. Child cursor walked in loop 1 45; children
// read without the pointer cast 45; source pointer copied first 45; get_info output at function
// scope 45; produced declared at loop scope 143.
// Round 22 (lane 13; aligned difflib count 38, nearly all the 4-byte frame offset, plus request/child
// cursor EBP/EBX and the loop-2 cursor temp EDX/ECX): total declared before error 45; the starts
// store before the child read 45; the dispatch gate as nested ifs 45; a const dispatch cursor 45;
// the cleanup item read into its local before the test 89 (368 bytes). BLOCKED by the build flag:
// the retail frame needs `produced` to share a slot, which only a non-LTCG object gives.
// Lane 16 r48 (fast): the collection loop in a block of its own 45 (frame 0x8024); the cleanup bound
// named from the block 65; the cleanup loop bounded by starts[3] 45; `*source` read once before the
// collection loop 145 (389 B); the item cleared before its release 59. Retail's slot 0x10 holds
// produced, then the dispatch cursor and the cleanup countdown (strength-reduced loop temporaries),
// which our LTCG build gives slots of their own.
// Round 33 (lane 10): loop 1's child and error tests nested 45 (identical); loop 2's inner walk over
// a block pointer range 128 (389 bytes); v5's count output as a `long &` 45 (identical); loop 2's
// cursor declared at function scope 45 (identical); produced declared at the top of loop 1's body
// and zeroed in the call's block 55. A compile-only /FAs build without /GL gives retail's 0x8020
// frame (produced, loop 2's cursor and loop 3's count in one slot) and leaves request in EBP (retail
// EBX) and the end bound loaded first; the starts store first, nested tests, a source copy, the
// bounds declared end first, a block-pointer walk and a `!error` gate don't move either there.
// Round 7 (m3 lane 18, fast): produced renamed written (a name sorting after the compiler's
// temporaries) 45 (frame 0x8024); the dispatch bounds declared end first 45 (same code); the
// dispatch child read after the bounds 45; the cleanup indexing blocks[k].items[j] directly 45; the
// collection loop guarded by `if (child == 0 || error != 0) continue;` 55.
// Lane 16 r50 (fast): the cleanup item test as a declaration in the condition 89 (368 B); blocks
// declared after starts/error/total 45; one function-scope index for all three loops 45 (frame
// 0x8024); the get_info output declared at the inner loop top 45; the cleanup through blocks[k]
// directly 45. BLOCKED by the build flag (frame 0x8020 needs a non-LTCG object).
// Round 138 (lane 4; a one-file LTCG harness of this file reproduces the 0x8024 frame): the dispatch gate
// with the type test first 49; the collection test with the error test first 50; each boundary stored after
// its child (`starts[i + 1] = total;` in the loop, `starts[0] = 0;` first) 129; the release output initialized
// to NONE at its declaration 78 (376 bytes); a named `room` argument 45 (same code). Harness only: renaming the
// request, source, error, total, starts, blocks or child locals: the same code each (names do not move the
// LTCG frame); nested gate/collection tests, a while dispatch loop, a block pointer argument same code.
// Round 27 (m3 lane 15, fast): `if (child && !error)` 45; `blocks + j` in the dispatch call 45; the
// dispatch gate with the type test first 49; the dispatch inner loop as `while (first < end)` with a
// post-increment 45; the capacity named before the call 45. Still BLOCKED by the build flag (frame 0x8020
// needs `produced` to share a slot, which only a non-LTCG object gives).
// Round 23 (m3 lane 2, fast): the request type read into a local at the top 150 (389 bytes); `total = total +
// produced` 45; the dispatch start read in the inner loop's init 45; the get_info result named as a bool 45; the
// dispatch bounds through `long const *bounds = &starts[i]` (bounds[0], bounds[1]) 45: it gives retail's
// boundary load order (starts[i] into ECX first), but those lines still differ by the 4-byte frame offset.
// Round 32 (m3 lane 14, fast): loop 2 walking a `starts` pointer (`start[0]`/`start[1]`) 120 (389 bytes);
// loop 2 over a block pointer with a countdown from `starts[i + 1] - starts[i]` 50; `*source` read into a
// local in loop 1 147 (389 bytes); the type test first in the dispatch gate 49; `blocks + total` in loop
// 1's call 45 (same code). Still BLOCKED by the build flag.
// Round 34 (m3 lane 14, fast): the count output as a local of a forceinline collection helper, error or count
// returned, 147/147 (389 bytes, frame 0x8024); as a one-member struct 45 and a one-element array 45
// (diagnostics, frame unchanged); the cleanup loop walking an item pointer 87 (376 bytes); the dispatch child
// read as `((c_child **)children)[i]` (no walking cursor) 31, kept. On 31: produced declared at function scope
// 42, at the collection loop's top 141 (389 bytes); `blocks + k` 31; starts declared before blocks 31; the
// dispatch bounds read in the inner loop's test 121 (388 bytes); the cleanup loop over a block pointer 100
// (391 bytes). Diagnostic (reverted at once): the file built /GL- leaves 2 (the bound loads only). Compile-only
// (/O2 /Gr, no /GL) bound spellings: end declared first, `end_block > j`, j declared outside the for, the end
// named before a loop-variable j: end loaded first (as ours); a bounds pointer, a while loop, `for (; j < end;)`
// and `for (long j = starts[i], end_block = starts[i + 1]; ...)` load starts[i] first as retail; the last
// kept (31 under LTCG: those two loads still differ by the frame offset; without /GL nothing else changes, so
// that build is byte-exact). Still BLOCKED by the build flag (frame 0x8020 needs `produced` to share a slot).
// Round 36 (m3 lane 14, fast; all 31 differences are the 4-byte frame offset and the swapped temporary slots):
// produced declared at the loop-body top and zeroed in the call's block 42; the cleanup block as `blocks + k`
// 31; total declared before error 31; `total = total + produced` 31; a named room argument 31; the cleanup item
// tested without `!= 0` 31 (all same code).
// Retail's slot 0x10 holds produced, the dispatch child cursor and the cleanup countdown, slot 0x14 the
// children base, the inner count and the block pointer; ours groups the temporaries the same way but keeps
// produced apart. Still BLOCKED by the build flag.
// Round 285: Shared callback output with fixed short/unsigned-short collection and dispatch ordinals,
// scratch after counters/before arrays: eight at 161 (406 bytes). Frame and callback/loop temporary homes
// remain; complete body and original signed ordinals restored.
// Round 287: One count across callback and dispatch-end/cleanup/both phases,
// independent/function-scope information output, count before arrays: 138/157/176/138/157/176/176/176.
// Escaped count and later loop homes remain; original signed traversal and outputs restored.
// Round 31 (m3 lane 2, fast): produced as a dword (passed through a long pointer cast) 31; total as a
// dword 32; the room argument as `sizeof(blocks) / sizeof(blocks[0]) - total` 31; the collection child
// declared before loop 1 31; error and total declared before starts 31 (frame 0x8024 in each).
// Round 147 (m3 lane 4, fast; a compile-only /GL- listing names the shared lowest slot produced plus two
// compiler temporaries, the next slot three more): error renamed status (a name sorting after produced)
// 31 (same code); produced declared and zeroed at the loop-body top 125 (389 bytes); the boundary stored
// before the child read 31; the cleanup through blocks[k] directly 31; both loops calling the concrete
// child class (children[i] without the cast) 31. Still BLOCKED by the build flag.
// @retail 0x97d80
long c_aggregate_450cf4::function_97d80(dword *source, s_request *request)
{
	s_block blocks[256];
	long starts[4];
	long error = 0;
	long total = 0;

	for (long i = 0; i < 3; i++)
	{
		c_child *child = ((c_child **)children)[i];
		starts[i] = total;
		if (child != 0 && error == 0)
		{
			long produced = 0;
			error = child->v5(*source, request, 256 - total, &blocks[total], &produced);
			total += produced;
		}
	}
	starts[3] = total;

	if (error == 0 && request->type != 4)
	{
		for (long i = 0; i < 3; i++)
		{
			c_child *object = ((c_child **)children)[i];
			for (long j = starts[i], end_block = starts[i + 1]; j < end_block; j++)
				object->v6(&blocks[j]);
		}
	}

	for (long k = 0; k < total; k++)
	{
		s_block *block = &blocks[k];
		for (long j = 0; j < block->count; j++)
		{
			if (block->items[j].object != 0)
			{
				c_child *object = block->items[j].object;
				long result;
				if (!g_4d87f8->allocator->get_info(object, &result))
					result = NONE;
				allocator_globals_release(g_4d87f8, object);
				block->items[j].object = 0;
			}
		}
	}

	return error;
}

// @retail 0x97f00
void c_aggregate::v7(void *a)
{
	for (long i = 0; i < 3; i++)
	{
		c_child *child = children[i];
		if (child != 0)
			child->v7(a);
	}
}

// @retail 0x97f30
void c_aggregate::v8(long a, long b)
{
	long local_0 = *(volatile long *)&a;
	for (long i = 0; i < 3; i++)
	{
		c_child *child = children[i];
		if (child != 0)
			child->v8(local_0, b);
	}
}

/* ---- class B: a handle table ---- */
// @retail 0x97f60
void function_97f60(s_handle_peers *t, long a, c_handle_table_450cd0 *self)
{
	self->table = t;
	long local_0 = 1 << a;
	self->head = 0;
	self->shift = a;
	self->bit = local_0;
	self->node = 0;
	for (long i = 0; i < 1024; i++)
	{
		self->entries[i].handle = NONE;
		self->entries[i].unknown04 = 0;
		self->entries[i].state = 0;
		self->entries[i].unknown0a = NONE;
		self->entries[i].unknown0c = 0;
		self->entries[i].unknown10 = 0;
	}
	self->unknown5024 = NONE;
	self->unknown502c = 0;
	self->unknown5028 = 0;
	self->unknown503c = 0;
	self->unknown5038 = 0;
	self->unknown5034 = 0;
	self->unknown5030 = 0;
	self->unknown09 = 0;
	self->unknown0a = 0;
	self->unknown08 = 1;
}

// @retail 0x97fe0
void c_handle_table_450cd0::function_97fe0()
{
	while (head != 0)
		v8(head->handle, false);

	long local_0 = 0;
	s_handle_entry *entry = entries;
	for (long i = 0; i < 1024; i++, entry++)
	{
		if (entry->state != local_0)
		{
			long handle = entry->handle;
			s_handle_peer *item = &table->peers[handle & 0x3ff];
			word mask = item->mask;
			if (mask & (1 << shift))
			{
				item->mask = mask & ~(1 << shift);
				s_handle_peers *t = table;
				if (t->peers[handle & 0x3ff].mask == 0)
				{
					t->owner->v12(handle);
					byte *local_1 = &t->peers[handle & 0x3ff].flags;
					*local_1 &= 0xfe;
				}
			}
			entry->state = (short)local_0;
			entry->handle = NONE;
			entry->unknown0a = NONE;
			entry->unknown10 = (word)local_0;
		}
	}

	unknown502c = 0;
	unknown5028 = 0;
	unknown5034 = 0;
	unknown5030 = 0;
	unknown5024 = NONE;
}

// @retail 0x980d0
void c_handle_table_450cd0::function_980d0(long handle, dword mask)
{
	long index = handle & 0x3ff;
	s_handle_entry *entry = &entries[index];
	if (entry->state == 3)
	{
		if (!((1 << shift) & table->peers[index].mask) && entry->unknown04 == 0)
			unknown5034++;
	}
	entry->unknown04 |= mask;
}

// @retail 0x98120
bool c_handle_table_450cd0::function_98120()
{
	bool result = false;
	if (unknown09)
		result = (unknown503c != 0) | (unknown5034 != 0) | (unknown502c != 0);
	return result;
}
