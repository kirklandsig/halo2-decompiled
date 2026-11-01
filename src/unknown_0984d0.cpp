#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_08b110.h"
#include "unknown_096ed0.h"
#include <string.h>

// @flags /O2 /Ob1 /Gr

/* src/unknown_096ed0.cpp, src/unknown_0b5650.cpp, src/unknown_195720.cpp */
void function_98ac0(c_handle_table_450cd0 *self, long handle);
void function_98b60(c_handle_table_450cd0 *self, long handle);
void function_98bf0(long handle, c_handle_table_450cd0 *self, dword mask);
void simulation_write_entity_identifier(long identifier, s_bitstream *stream);
void function_194710(s_bitstream *stream, bool discard);

/* the handle-table messages each start with a 3-bit kind; the stream keeps
   the position before it, so that a message without room can be taken back */
static inline void stream_push_position(s_bitstream *stream)
{
	stream->checkpoints[stream->checkpoint_count] = stream->bit_position;
	stream->checkpoint_count++;
}

static inline bool stream_has_room(s_bitstream *stream, long bits)
{
	return (stream->size_in_bytes << 3) - stream->bit_position >= bits;
}

/* kind 1: an entity's creation */
// @retail 0x98620
bool function_98620(c_handle_table_450cd0 *self, s_bitstream *stream, long index, long a3, long reserved_bits)
{
	long handle = self->entries[index].handle;
	long released = 0;
	stream_push_position(stream);
	function_195720(stream, 1, 3);
	simulation_write_entity_identifier(handle, stream);
	dword local_1 = self->entries[index].unknown04;
	if (self->table->owner->v0(handle, local_1, a3, stream, reserved_bits, &released) &&
		(stream->size_in_bytes << 3) - stream->bit_position >= reserved_bits)
	{
		stream->checkpoint_count--;
		function_98ac0(self, handle);
		if (released)
			function_98bf0(handle, self, released);
		return true;
	}
	function_194710(stream, true);
	return false;
}

/* kind 2: an entity's deletion */
// @retail 0x986d0
bool function_986d0(c_handle_table_450cd0 *self, long index, s_bitstream *stream, long reserved_bits)
{
	bool result = false;
	long handle = self->entries[index].handle;
	stream->checkpoints[stream->checkpoint_count] = stream->bit_position;
	stream->checkpoint_count++;
	function_195720(stream, 2, 3);
	simulation_write_entity_identifier(handle, stream);
	if (stream_has_room(stream, reserved_bits))
	{
		stream->checkpoint_count--;
		function_98b60(self, handle);
		result = true;
	}
	else
		function_194710(stream, true);
	return result;
}

/* kind 3: creation of an entity and the entities linked to it */
// score 18 (was 18): the callback setup still differs: our table/owner/vtable loads are hoisted
// above the argument pushes, where retail loads them after the entry value (all in ecx). The loop
// on `result && i < count` with the nested status test and its else spills the index around the
// call as retail does, with no volatile reads, goto or self reference; without the outer else the
// index stays in ebx (135 instructions). Round 49: entry value in a local first 18; retail's
// continue-shaped status update 14 and an argument-first forwarding helper 14 (each leaves only
// the receiver loads; neither keepable); owner/table/entry locals 16-18 (owner local in this form 18); status ternaries and
// `result = result && ...` 41-44; cleanup index, declaration order, handle local, peers receiver
// and loop shapes in a 100-form random search, 18 or worse.
// Earlier (22-131): unsigned handle/index storage, status role names, nested success admission,
// loop-proven status, inline/noinline declarations of the rollback, cleanup, mask release and
// identifier writer, index before outcome stores/admission, separate rollback tails, completed
// handle lifetime, status returns, final room guard, encoded count before/after the checkpoint,
// masked entry index, constant member dispatch, entry pointer, admitted mask/owner snapshots,
// peer receiver refresh, direct receiver, persistent entry-array base, separate callback handle,
// callback adapter, parallel cursors, status assignment, field references, grouped arrays, byte
// status, failure first, common rollback, element pointers, argument record, array names/order,
// append increment, array initialization, unsigned count, callback index name, status
// declaration order, inline depth zero.
// Round 318: on the natural index-spill body, long/short status 148/142;
// status after header 72; late long/short status 142/142; twice-used release mask 18;
// one status exit 18; late status with mask 72. Frame and callback setup remain. All restored.
// Round 5: named callback inputs with late and separated receiver acquisitions 18/18/18;
// bottom-increment for/while 18/18; success/failure presets 122/126 (414/402); shared checkpoint
// helper 18; release pointer arithmetic 18; assigned outcome branches 18; writer argument
// assignment 18; index declaration initializer 18. All restored.
// Round 136, natural spill graph: bool/long/short/byte callback outcomes 18,18,18,18;
// callback outcome with entry/owner/long-entry/both snapshots 18,18,18,18. All nonmatching exact-only forms restored.
// Round 44: retail's entry index uses ECX (`lea ecx, [eax+eax*4]`), which keeps its receiver loads
// after the entry value; ours uses EAX and hoists them. `if (result && call)` without the outer else
// 113; inner update false-first 18; receiver through `peers` 108; `handles[i]` at each use 18;
// permuter 56 tries, best 18.
// Round 184 (lane 3): `if (result && call) result = true; else result = false;` 113 (386 bytes);
// `if (!result || !call) result = false; else result = true;` 113; the inner update as
// `result = call ? true : false;` 113; the entry index as a local 18 (same code); the inner
// `if (result)` dropped (the loop condition implies it) 126 (370 bytes).
// @retail 0x98750
bool function_98750(c_handle_table_450cd0 *self, s_bitstream *stream, long index, long a3, long reserved_bits)
{
	bool result = true;
	long handles[4];
	long count = 0;
	long handle = self->entries[index].handle;
	s_handle_peers *peers = self->table;
	while (handle != NONE)
	{
		handles[count] = handle;
		handle = peers->peers[handle & 0x3ff].unknown04;
		count++;
	}
	long released[4];
	memset(released, 0, sizeof(released));
	stream_push_position(stream);
	function_195720(stream, 3, 3);
	stream_write_checked(stream, count - 2, 2);
	long i;
	for (i = 0; result && i < count; i++)
	{
		handle = handles[i];
		simulation_write_entity_identifier(handle, stream);
		if (result)
		{
			if (self->table->owner->v0(handle, self->entries[handle & 0x3ff].unknown04, a3, stream, reserved_bits, &released[i]))
				result = true;
			else
				result = false;
		}
		else
			result = false;
	}
	if (result && stream_has_room(stream, reserved_bits))
	{
		stream->checkpoint_count--;
		for (i = 0; i < count; i++)
		{
			function_98ac0(self, handles[i]);
			if (released[i])
				function_98bf0(handles[i], self, released[i]);
		}
		return true;
	}
	function_194710(stream, true);
	return false;
}

/* kind 4: the deletion of an entity and the entities linked to it */
// @retail 0x988f0
bool function_988f0(c_handle_table_450cd0 *self, long index, s_bitstream *stream, long reserved_bits)
{
	bool local_0 = false;
	long handles[4];
	long count = 0;
	s_handle_peers *peers = self->table;
	long handle = self->entries[index].handle;
	while (handle != NONE)
	{
		handles[count] = handle;
		handle = peers->peers[handle & 0x3ff].unknown04;
		count++;
	}
	stream->checkpoints[stream->checkpoint_count] = stream->bit_position;
	stream->checkpoint_count++;
	function_195720(stream, 4, 3);
	stream_write_checked(stream, count - 2, 2);
	long i;
	for (i = 0; i < count; i++)
		simulation_write_entity_identifier(handles[i], stream);
	if (stream_has_room(stream, reserved_bits))
	{
		stream->checkpoint_count--;
		for (i = 0; i < count; i++)
			function_98b60(self, handles[i]);
		local_0 = true;
	}
	else
		function_194710(stream, true);
	return local_0;
}

/* kind 5: an entity's update */
// @retail 0x989f0
bool function_989f0(c_handle_table_450cd0 *self, long index, s_bitstream *stream, long a3, long reserved_bits)
{
	long local_3 = stream->bit_position;
	long handle = self->entries[index].handle;
	long released = 0;
	stream->checkpoints[stream->checkpoint_count] = local_3;
	stream->checkpoint_count++;
	function_195720(stream, 5, 3);
	simulation_write_entity_identifier(handle, stream);
	dword local_1 = *(volatile dword *)&self->entries[index].unknown04;
	if (self->table->owner->v5(handle, local_1, a3, stream, reserved_bits, &released) &&
		(stream->size_in_bytes << 3) - stream->bit_position >= reserved_bits)
	{
		stream->checkpoint_count--;
		function_98bf0(handle, self, released);
		return true;
	}
	function_194710(stream, true);
	return false;
}

static __forceinline s_handle_node *function_984d1(s_handle_node *arg_0)
{
	if (arg_0)
	{
		arg_0->handle = NONE;
		arg_0->items = 0;
		arg_0->records = 0;
		arg_0->next = 0;
		return arg_0;
	}
	return 0;
}

// @retail 0x984d0
void c_handle_table_450cd0::v3(long a1, long a2, long a3, long a4, long a5, long a6)
{
	if (node == 0)
	{
		s_allocator_globals *globals = g_4d87f8;
		s_handle_node *block = (s_handle_node *)globals->allocator->allocate(0x10, 0, 0);
		if (block == 0)
		{
			globals->allocator->compact(0);
			block = (s_handle_node *)globals->allocator->allocate(0x10, 0, 0);
		}
		if (block != 0)
		{
			globals->count++;
		}
		block = function_984d1(block);
		node = block;
		if (node != 0)
		{
			node->handle = a4;
			node->items = 0;
			node->records = 0;
			node->next = 0;
			s_handle_node **link = &head;
			while (*link != 0)
				link = &(*link)->next;
			*link = node;
		}
		else
			unknown0a = 1;
	}

	if (!unknown0a)
	{
		switch (a2)
		{
		case 1:
			function_98620(this, (s_bitstream *)a5, a1, a3, a6);
			break;
		case 2:
			function_986d0(this, a1, (s_bitstream *)a5, a6);
			break;
		case 3:
			function_98750(this, (s_bitstream *)a5, a1, a3, a6);
			break;
		case 4:
			function_988f0(this, a1, (s_bitstream *)a5, a6);
			break;
		case 5:
			function_989f0(this, a1, (s_bitstream *)a5, a3, a6);
			break;
		default: __assume(0);
		}
	}
}

// @retail 0x98aa0
void c_handle_table_450cd0::v4(long a1, s_bitstream *stream)
{
	function_195720(stream, 0, 3);
	node = 0;
}

struct s_handle_creation;
bool function_991d0(c_handle_table_450cd0 *self, long handle, long size, void const *data);
void replication_table_add_chain(long const *values, s_handle_peers *peers, long count, long const *handles, long const *others, s_handle_creation *blocks);
long replication_table_get_chain(s_handle_peers *peers, long handle, long *handles);
void replication_table_release(s_handle_peers *peers, long handle);
void replication_table_release_chain(s_handle_peers *peers, long count, long const *handles);

class c_98fb0
{
public:
	virtual void v0() = 0;
	virtual void v1() = 0;
	virtual void v2() = 0;
	virtual void v3() = 0;
	virtual void v4() = 0;
	virtual void v5() = 0;
	virtual void v6() = 0;
	virtual void v7(long arg_0, long arg_1, long arg_2, void *arg_3) = 0;
};

PRIVATE __forceinline bool function_98fb1(c_handle_table_450cd0 *arg_0, long const *arg_1, byte const *arg_2)
{
	long local_0 = arg_1[4];
	long local_1 = arg_1[0];
	return function_991d0(arg_0, local_1, local_0, arg_2);
}
// score 115 (was 115): frame 0x1c agrees; validation items/data carriers and entry past the items reload differ.
// Earlier at 115 (was 115): the case 1/3 loop still differs in registers: retail keeps the items
// this round: no new form; the case 1/3 loop still keeps its cursors in
// registers where retail keeps them in memory, and the 0x991d0 argument order still differs.
// Earlier at 115: the case 1/3 loop still differs in registers: retail keeps the items
// Earlier (115): the case 1/3 loop still differs in registers: retail keeps the items
// Earlier: the case 1/3 loop still differs in registers: retail keeps the items
// pointer and the index in different registers than ours, and the argument order of the
// 0x991d0 call is not reproduced.
// pointer in eax and the data pointer in edx and enters the loop past the items reload; ours uses
// ecx for data and moves the handle into edx. The count == 1 branch now loads and spills like retail.
// The volatile reads keep the loop pointers on the stack as retail does; without them the handle
// is spilled instead (207). Tried: memcmp sized by info.count (179), state read into a local (115),
// loop counter by index without pointers (207), function_991d0 called directly (143), helper
// reading [0] first (134) or no helper locals (137), pointer locals or increments reordered (115).
// Tried additionally: call-first logical accumulation 191, compound accumulation 197, conditional
// accumulation 215, failure result 124, named cursors 115, named cursors/logical accumulation 191,
// named helper inputs 115, named cursors/failure result 124; original body restored at 115.
// Actual invalid counts: inherited/while/for/early cursor steps 171/212/213/211;
// actual valid counts with visited-count comparison 194/211/212/212. Items/data
// validation carriers and loop-entry reload remain; opening body and helper restored.
// Direct case scopes in validation/chain/notify, chain/validation/notify, notify/validation/chain,
// and notify/chain/validation orders 115/162/212/186; ordinary inline producer with those orders
// 115/162/212/186. Existing dispatch and producer restored.
// Consistent existing helper input orders 115 (five forms); entry validity 143;
// entry cursor declarations 115; both entry storage lifetimes 143. Original body restored.
// Plain pretested for with both/size/handle/no helper producers 195,210,212,212; corresponding while
// 195,210,212,212. Items/data carriers remain; full opening helper and body restored.
// Round 53: natural loops without the volatile reads (index or pointer cursors, direct call or
// helper, named size/handle/result values, for or while; 38 screened) keep edi for a cursor and
// spill the handle: indexed direct call 207f, indexed helper 208f, named size 207f, named size in
// the test 207f, pointer cursors with a direct call 212f.
// Generic data/unsigned item contracts/both 115/115/115; actual info record/generic data
// 229/229; native four-word data/native unsigned items 115/115; info plus native data 229.
// Real validator coordinated inline knowledge 115; with direct matched call 137; size/handle/both
// producer forwarding 138/134/137; immediate element-validity result 115 (exact-only); ordinary
// private-producer inline 115; conditional all-valid accumulation 215. Original validator declarations,
// private helper, both qualified pointer-slot reads and complete body restored.
// Lane 14 r11: retail keeps both cursors and the counter in memory (load, add, store, then the
// loop top reloads) with this/request/handle/index in EBX/EBP/EDI/ESI; every natural form splits
// the handle (spilled at entry, reloaded after the loop) and gives a cursor EDI. Plain pointer
// cursors 207; for loop over the parallel items/b arrays with the direct call 207; helper taking
// the cursors by const reference 207; the volatile read on the data cursor only 207, on the items
// cursor only 210; cursors grouped in a local struct (diagnostic) 210.
// Round 40: index loop with items[i], b[i], &data.v[i * 4] passed straight to 0x991d0 207 (data
// cursor and counter in memory as retail, but the items cursor takes EDI and the handle spills);
// through the helper 208; handle/index renamed three ways 207; handle/index declared in each
// case block 166 (with the index loop 189).
// Round 19 (lane 13), all without the volatile reads: the loop written with a label and a backward
// goto (diagnostic) 207, so the loop weighting is not syntactic; the items and data read by index in
// a for loop 208; the cursors passed to the helper by address 207, the helper also advancing them
// 206 (inlining promotes them back to registers); the result declared first in the function 218;
// the cursors and counter declared at the function top 207.
// Round 40: index loop with items[i], b[i], &data.v[i * 4] passed straight to 0x991d0 207 (data
// cursor and counter in memory as retail, but the items cursor takes EDI and the handle spills);
// through the helper 208; handle/index renamed three ways 207; handle/index declared in each
// case block 166 (with the index loop 189).
// Round 42 (lane 16): the case 5 owner call through a peers local 105 (the case 1 local declared
// for the whole function and assigned in both cases: the case 5 tail then matches, retail reuses
// EBX for the table; reverted at review: a one-use copy is kept only with a match); count read into one local for the ==1 test, chain and loop 176; memcmp
// sized by info.count 158; peers local also in case 2/4 for the flag tests and the chain query 126;
// 0x991d0 called directly from the loop 137; entry state tested first as an early return 105; loop
// cursors declared before the count test 195.
// Round 10 (m3 lane 2): the switch now dispatches with grouped case labels and breaks in place of
// gotos to labels (identical code, 115). The volatile cursor reads swapped 115; both passed straight
// to the helper 115; the case 2/4 entry through a named element pointer 115, its state read once into
// a named local 115 (same code). Natural loops still split the handle (harness: pointer cursors 184
// positional, index loops with the direct call or the helper 184/185).
// Round 30 (lane 13, fast): the cursors declared at the case top without the volatile reads 212 (528
// bytes), also shared (with the counter) by the notify loop 212; the notify loop reusing the validation
// counter 115 (same code); no early return after the validation 115; a `void *const *` data cursor over
// data.v 115. Compile-only, the natural loop still splits the handle with the loop in a forceinline
// helper, without the `__assume(0)` default, with extra cases, or with the handle used inside the loop.
// Round 214: shared phase ordinal 115; entry chain array 115; positive validity admission 115;
// ordinal/entry chain array 115; ordinal/positive admission 115; entry array/positive admission
// 115; all three lifetimes 115; all lifetimes/immutable handle and index 115.
// Round 106 (fast): after the loop retail loads the count into ECX (ours EAX), which shifts the count == 1
// block and the chain call. Count test inverted (chain call first) 109 at 544 bytes; result test as a scope
// instead of the early return 115; loop condition operands swapped 115; dword handle 115; case 2/4 written
// before case 1/3 162.
// N round 257: Complete case priorities 354/453 with bool, byte, signed-byte and word validity 179/179/179/204/185/185/185/205.
// Original dispatch, all validity storage, helper and both local-slot reads restored.
// Round 125 (lane 5): the slot index written out as `local_1 & 0x3ff` at each use 161 (diagnostic of
// the handle's weight); the handle typed dword 115; memcmp sized by the request's count with the count
// test reversed 178 (520 bytes); a plain for loop with the cursors set before it (no hand-inverted
// guard) 195; the case 2/4 element through a pointer local 115.
// Round 64 (lane 12; one-link LTCG env with the stand-in callers, code equal to the full listing; env positional/
// aligned 99/55 for this body): retail's loop cursors behave as ordinary spilled variables (the items cursor
// reloaded on the back edge only, the preheader's EAX used on entry; ESI/EDI untouched in the loop), not volatile
// reads. New: an index for loop through a forceinline helper taking the block and the index, without the
// volatile reads, 184/55; the items cursor typed as the field's dword (volatile reads kept): identical; the
// handle re-read from the request in the count-one tail (diagnostic, natural loop) 184. Re-measured in the env
// only (listed above): no volatile reads 184/63 (handle spilled before the switch, items cursor in EDI), the
// loop in a forceinline validator 184, a for loop stepping both cursors 193, an index loop passing element
// addresses 185.
// Round 60 (lane 16, fast): without the volatile cursor reads (207, 524 bytes) VC splits the handle around
// the loop (spilled at entry, reloaded after) and gives EDI to the items cursor; retail keeps this/request/
// index/handle in EBX/EBP/ESI/EDI and spills both cursors and the counter. On that base: the notify loop
// reusing the validation counter 207; 0x991d0 called from the helper with the items read in place 207
// (aligned 65); handle and index `long const` 207; the validity flag declared before the switch 217; the
// index declared first from the field (`index & 0x3ff`, then the handle) 207.
// Round 31 (m3 lane 2, fast): the request count read into one local for the == 1 test and the chain call
// 115 (same code); a peers local in case 2/4 for both flag tests and the table calls 182 (552 bytes); case
// 1's peers local dropped (the member read at each use) 204 (552 bytes); the chain-count comparison
// operands swapped 116; memcmp sized by the request's count (retail's `lea ecx, [edx*4]`) re-measured on
// this body 178 (520 bytes). 0x991d0's call setup follows its declared order (one register argument,
// the handle in EDX), so no parameter reorder has retail evidence.
// Round 147 (m3 lane 4, fast; volatile-read body): the validation counter declared inside the count test's
// block 203 (524 bytes); both cursors declared at case scope 195; the counter declared before the validity
// flag 117; the validity flag declared before the switch 142; the request type read once into a local for the
// switch and the type-2 test 115 (same code). Re-measured: plain cursors at case scope 212, the count test
// inverted 109 (544 bytes, branches laid out opposite to retail).
// Round 42 (m3 lane 13, fast): case 2/4's state test as an `if` of its own around the handle/state test 115 (same
// code); the chain-length test written `local_9 == info.count` with memcmp sized by info.count 178 (520 bytes); the
// validity flag declared before the switch 142; the validation loop as a plain `for` over the counter (volatile
// cursor reads kept) 195; the count-one branch reading `table` at each use (no peers local) 204. Independent
// tie-breaks left besides the loop: the count after the loop in EAX (retail ECX, so the chain call's register
// temporaries rotate), case 2/4's index*5 in ECX (retail EAX: state read through the scaled index before the entry
// address is formed), case 5's table in EDX (retail reloads it into EBX).
// @retail 0x98fb0
void c_handle_table_450cd0::v6(s_request_450cd0 *a1)
{
	s_handle_block *local_0 = (s_handle_block *)a1;
	long local_1 = local_0->index;
	long local_2 = local_1 & 0x3ff;
	switch (local_0->type)
	{
	case 1:
	case 3:
	{
		bool local_6 = true;
		long local_7 = 0;
		if (local_0->info.count > 0)
		{
			long const *local_10 = (long const *)local_0->info.items;
			byte const *local_11 = (byte const *)&local_0->data;
			do
			{
				long const *local_12 = *(long const *volatile *)&local_10;
				byte const *local_13 = *(byte const *volatile *)&local_11;
				if (!function_98fb1(this, local_12, local_13))
					local_6 = false;
				local_11 += 0x10;
				local_7++;
				local_10++;
			} while (local_7 < local_0->info.count);
		}
		if (!local_6)
			return;
		if (local_0->info.count == 1)
		{
			long local_19 = local_0->count;
			long local_18 = local_0->info.b[0];
			long local_17 = local_0->info.c[0];
			s_handle_peers *local_14 = table;
			local_14->peers[local_2].flags = 1;
			local_14->peers[local_2].unknown01 = (byte)((dword)local_1 >> 28);
			local_14->peers[local_2].mask = 0;
			local_14->owner->v2(local_1, local_18, local_17, local_19, &local_0->data);
		}
		else
			replication_table_add_chain((long const *)local_0->info.b, table, local_0->info.count, (long const *)local_0->info.items, (long const *)local_0->info.c, (s_handle_creation *)&local_0->data);
		for (long local_7 = 0; local_7 < local_0->info.count; local_7++)
			function_99690(this, local_0->info.items[local_7], 3);
		break;
	}
	case 2:
	case 4:
	{
		if (entries[local_2].state && entries[local_2].handle == local_1 && entries[local_2].state == 3)
		{
			if (local_0->type == 2)
			{
				if (!(table->peers[local_2].flags & 0x18))
				{
					function_99690(this, local_1, 0);
					replication_table_release(table, local_1);
				}
			}
			else if (table->peers[local_2].flags & 8)
			{
				long local_8[4];
				long local_9 = replication_table_get_chain(table, local_1, local_8);
				if (local_0->info.count == local_9 && memcmp(local_0->info.items, local_8, local_9 * sizeof(long)) == 0)
				{
					for (long local_7 = 0; local_7 < local_0->info.count; local_7++)
						function_99690(this, local_0->info.items[local_7], 0);
					replication_table_release_chain(table, local_0->info.count, (long const *)local_0->info.items);
				}
			}
		}
		break;
	}
	case 5:
		((c_98fb0 *)table->owner)->v7(local_1, local_0->info.c[0], local_0->count, &local_0->data);
		break;
	default:
		__assume(0);
	}
}
