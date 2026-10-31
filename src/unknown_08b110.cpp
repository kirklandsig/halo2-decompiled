#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_08b110.h"
#include "unknown_096ed0.h"
#include "unknown_0662e0.h"

// @flags /O2 /arch:SSE /Gr

// @retail 0x81590
c_replication_view_storage::c_replication_view_storage()
{
}

struct s_z_input_packet;
struct s_z_transform_state;
void function_aaef0(s_bitstream *stream, s_z_input_packet const *state);
void function_ab7f0(s_bitstream *stream, s_z_transform_state const *state);
void function_194710(s_bitstream *stream, bool discard);

// @retail 0x8b9e0
void c_vtable_450c94::v3(s_node_450d1c *node, long a2, long a3, long key,
	s_bitstream *stream, long reserved_bits)
{
	long index = (long)node;
	stream_save_position(stream);
	stream_write_bit(stream, true);
	stream_write_checked(stream, index, 5);
	stream_write_bit(stream, (active_mask & (1 << index)) != 0);
	if (active_mask & (1 << index))
		function_aaef0(stream, (const s_z_input_packet *)&data18[index]);
	stream_write_bit(stream, (unknown718 & (1 << index)) != 0);
	if (unknown718 & (1 << index))
		function_ab7f0(stream, (const s_z_transform_state *)&data720[index]);
	if ((stream->size_in_bytes << 3) - stream->bit_position < reserved_bits)
		function_194710(stream, true);
	else
	{
		stream->checkpoint_count--;
		active_mask &= ~(1 << index);
		unknown718 &= ~(1 << index);
		times[index] = g_510548 ? g_51054c : GetTickCount();
	}
}

s_definition_4ced60 g_4ced60[32];
long g_4cf474;

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
long function_a5930(long index);
long function_a5980(long index);

struct s_object_relevance_source
{
	long object_index;
	long identifier;
	byte unknown08[0x1c - 8];
	real first;
	real second;
};

struct s_object_relevance_result
{
	real first;
	real second;
	long object_index;
	long identifier;
};

// @retail 0x82ac0
void function_82ac0(s_object_relevance_source *source, s_object_relevance_result *result)
{
	if (source->first > 0.0f || source->second > 0.0f)
	{
		long index = source->object_index;
		if (index != NONE && function_badc0(index, (dword)NONE))
		{
			long mode = ((long *)g_4cf77c)[2];
			long mapped;
			if (mode != 3 && mode != 5)
				mapped = function_a5930(index);
			else
				mapped = function_a5980(index);
			if (mapped != NONE)
			{
				result->object_index = mapped;
				result->identifier = source->identifier;
				result->first = source->first;
				result->second = source->second;
			}
		}
	}
}

// score 25 (was 25): retail places the allocation failure's `error = 2` arm after
// the release block's return (it jumps back to the null test); ours lays it out
// before the release block. Earlier at 25 (from 32): the error codes folded into the
// release test (32); the error set in an if/else with `result = error` before the
// release, the index test nested: 25.
// Earlier at 25 (from 25): cold allocation-error placement remains; validation-derived result 98;
// allocation/handler error defaults 63/64; valid-scope error 98; native payload word 25;
// validation plus payload word 98; size-first and type-first publication 25/25.
// Allocation failure remains before cleanup. Immutable
// allocation/handler/size 25/25/25; shared allocator receiver 29; indexed
// output 34; output element reference 25; signed-word error 41; direct failure
// result on the nested-index body 40.
// Earlier at 25 (from 25): Cold error-2 placement still differs. Success-first
// coded outcome 59; tested public outcome 75; path-defined outcome 81;
// function-scope size 25; registry record 25; produced-count reference 25;
// native publication on tested outcome 112; path-defined success 78.
// allocation error 2 still precedes the cleanup return.
// Reused size output 80; signed output 25; initialized output 45; globals/count/handler
// references 25/25/25; native item-size local 25; result after release 41.
// Earlier at 25 (from 25): allocation error 2 still precedes the cleanup return.
// Outcome at handler/size 48/50; invalid defaults 98/98;
// positive-size/allocation/success/default-success 56/58/75/72.
// Cold allocation error still precedes cleanup.
// record value 106; ordinal then record 104; field-filled record 106; owned cleanup 40; failure
// scope 40; absence-first ownership 69; byte error 41; unsigned result 25.
// cold error-2 arm still precedes release. Two-use allocation
// presence 76; complete index predicate 25; native pointer advancement 25;
// immutable post-info release owner 25; presence/owner 76; separated error
// enum 25; enum/presence 76; validation/publication/owner 25.
// Earlier at 25 (from 25): error-2 arm still inline before the release, retail sinks
// it past the cleanup return. Arms swapped 49; success/error returns split 73;
// handler-error default 64; separate post-hoc allocation test 25; function-scope
// error 25; const block 25; error declared after allocation 25; separate test with
// result after release 41.
// Round 41: error defaulting to 2 just after the allocation 60; result defaulting to 2
// there (3 on handler failure, 0 on success) 57; `block && handler` success test with
// `result = block ? 3 : 2` in an else/after it 39/39 (setne); the same test with an
// if/else of constants (3 then 2) 25 (jumps threaded, arms in source order), into result
// 40, arms 2 then 3 27; result 3 then `if (!block) result = 2` 51; the release through
// 0x8a6f0's force-inline release helper 25. No matched retail function outside a switch
// has this late `mov reg, imm; jmp back` block.
// @retail 0x8b110
long c_vtable_450cb8::v0(long index, long a2, long a3, long *count, s_item_450cb8 *items, void *a6)
{
	long result = 0;
	if (index >= 0 && index < handlers->count)
	{
		c_handler_450cb8 *handler = handlers->handlers[index];
		long size = handler->v2();
		if (size > 0)
		{
			long error;
			void *block = function_96e90(size);
			if (block != 0)
			{
				if (handler->v10(size, block, a6))
				{
					s_item_450cb8 * item = &items[(*count)++];
					item->data = block;
					item->size = (short)size;
					item->type = 0xe;
					goto local_0;
				}
				error = 3;
			}
			else
				error = 2;
			result = error;
			if (0 != block)
			{
				s_allocator_globals * globals;
				unsigned long info;
				g_4d87f8->allocator->get_info(block, &info);
				globals = g_4d87f8;
				globals->allocator->release(block, NONE);
				globals->count--;
			}
		}
	}
	else
		result = 3;
local_0:
	return result;
}

// @retail 0x8b1e0
void c_vtable_450cb8::v1(long index, long a2, long a3, s_item_450cb8 *item)
{
	c_handler_450cb8 *handler = handlers->handlers[index];
	long count = handler->v3();
	long value = count > 0 ? a2 : 0;
	long size;
	void *data;
	if (handler->v2() > 0)
	{
		data = item->data;
		size = item->size;
		item->data = 0;
	}
	else
	{
		size = 0;
		data = 0;
	}
	handler->v11(count, value, size, data);
	if (data != 0)
	{
		long result;
		g_4d87f8->allocator->get_info(data, &result);
		s_allocator_globals *globals = g_4d87f8;
		globals->allocator->release(data, NONE);
		globals->count--;
	}
}

// @retail 0x8b270
void c_vtable_450cb8::v2(long index, long a2, long a3, long a4)
{
	handlers->handlers[index]->v9(a2, a3, a4);
}

// @retail 0x8b2a0
void c_vtable_450cb8::v4(s_message_450cb8 *a1, s_context_450cb8 *a2, real *a3, long *a4)
{
	c_handler_450cb8 *handler = handlers->handlers[a1->type];
	long count = handler->v3();
	for (long i = 0; i < count; i++)
	{
		long handle = a1->handles[i];
		if (handle != NONE)
		{
			s_table_450cb8 *table = &a2->world->a->block->table;
			long index = handle & 0x3ff;
			if (index >= 0 && (dword)index < 0x400 && table->slots[index].handle == handle)
			{
				byte flags = table->flags->entries[index].flags;
				if (!(flags & 4))
					continue;
				if (table->slots[index].state == 3 && !(flags & 2))
					continue;
			}
			if (!handler->v4() || !function_979f0(a2->world, handle))
				goto fail;
		}
	}

	if (handler->v5(a1, a2))
	{
		handler->v6(a1, a2, a4);
		s_definition_4ced60 *definition = &g_4ced60[a1->type];
		real value;
		if (definition->unknown00 > 0.0f)
		{
			value = definition->unknown00;
		}
		else
		{
			real difference = definition->unknown10 - definition->unknown0c;
			real fraction = 0.0f;
			if (difference > 0.0f)
				fraction = handler->v7(a1, a2, definition->unknown08);
			value = fraction * difference + definition->unknown0c;
		}
		*a3 = value;
		return;
	}

fail:
	*a3 = 0.0f;
	*a4 = 0;
}
// @retail 0x8b450
void c_vtable_450cb8::v3(s_message_450cb8 *a1)
{
	if (handlers->handlers[a1->type]->v4())
	{
		long handle = a1->handles[0];
		if (handle != NONE)
		{
			s_datum_450cb8 *datum = datum_try_get_450cb8(datums, handle);
			if (datum != 0)
				datum->counter--;
		}
		handle = a1->handles[1];
		if (handle != NONE)
		{
			s_datum_450cb8 *datum = datum_try_get_450cb8(datums, handle);
			if (datum != 0)
				datum->counter--;
		}
	}
}

// @retail 0x8b410
void c_vtable_450cb8::v5(s_message_450cb8 *a1, long a2, long a3, long a4)
{
	c_handler_450cb8 *handler = handlers->handlers[a1->type];
	long index = handler->v0();
	handler->v8(a1, a2, g_4ced60[index].unknown08, a3, a4);
}

// @retail 0x8b870
bool c_vtable_450c94::v0()
{
	return active_mask != 0 || unknown718 != 0;
}

// @retail 0x8b890
long c_vtable_450c94::v1(long a1, long max_count, void *entries)
{
	long count = 0;
	s_entry_450c94 *entry_pointer = (s_entry_450c94 *)entries;
	long i;
	dword *time_pointer;
	for (i = 0, time_pointer = times; i < 32; i++, time_pointer++)
	{
		if (count >= max_count)
			break;

		bool flag_a = (active_mask & (1 << i)) != 0;
		bool flag_b = (unknown718 & (1 << i)) != 0;
		if (flag_a || flag_b)
		{
			s_entry_450c94 *entry = entry_pointer;
			
			count++;
			entry_pointer++;

			long size = 7;
			if (flag_a)
				size = 0x39;
			size++;
			if (flag_b)
				size += 0x71;

			entry->unknown00 = unknown04;
			real value = g_network_configuration.real1438;
			if (flag_a)
			{
				real x;
				long y;
				source->v1(i, a1, &x, &y);
				long start = *time_pointer;
				long elapsed = time_now() - start;
				if (elapsed >= g_network_configuration.value143c)
					value = g_network_configuration.real1440;
				else if (elapsed >= y)
					value = g_network_configuration.real1448 * x + g_network_configuration.real1444;
				else
					value = g_network_configuration.real1450 * x + g_network_configuration.real144c;
			}
			entry->size = size;
			entry->unknown04 = value;
			entry->index = i;
			entry->unknown0c = NONE;
		}
	}
	return count;
}

// @retail 0x8bf10
void c_vtable_450c94::v6(s_block_450c94 *block)
{
	long index = block->index;
	if (block->data[1] != 0)
	{
		data18[index] = *(s_dword34 *)block->data[1];
		mask14 |= 1 << index;
	}
	if (block->data[3] != 0)
	{
		data720[index] = *(s_dword40 *)block->data[3];
		mask71c |= 1 << index;
	}
}

// @retail 0x9c7c0
long c_interface_450c94::v2()
{
	return 1;
}

// @retail 0x8bbc0
void c_interface_450c94::v4(long a1, s_counter_450c94 *a2)
{
	a2->count++;
}

// @retail 0x8b760
void c_vtable_450c94::reset(c_source_450c94 *new_source)
{
	source = new_source;
	initialized = true;
	active_mask = 0;
	mask14 = 0;
	unknown718 = 0;
	mask71c = 0;
	for (long i = 0; i < 32; i++)
		times[i] = 0;
}

// @retail 0x8b7f0
bool c_vtable_450c94::take_data18(long index, s_dword34 *data)
{
	bool result = false;
	dword bit = 1 << index;
	if (mask14 & bit)
	{
		*data = data18[index];
		mask14 &= ~bit;
		result = true;
	}
	return result;
}

// @retail 0x8b830
void c_vtable_450c94::set_data720(long index, s_dword40 const *data)
{
	data720[index] = *data;
	unknown718 |= 1 << index;
}

bool function_ab2f0(s_bitstream *stream, s_z_input_packet *state);
bool function_ab960(s_bitstream *stream, s_z_transform_state *state);

static __forceinline void release_input_block(void *block, long *size)
{
 if (!g_4d87f8->allocator->get_info(block, size))
  *size = NONE;
 s_allocator_globals *globals = g_4d87f8;
 globals->allocator->release(block, NONE);
 globals->count--;
}

// @retail 0x8bbd0
long c_vtable_450c94::v5(dword a1, s_bitstream *stream, long max_blocks, s_block_450c94 *blocks, long *count)
{
 long result = 0;
 long used = 0;
 while (stream_read_bit(stream))
 {
  long index = function_1959c0(stream, 5);
  if (index < 0 || index >= 32)
  {
   result = 3;
   break;
  }
  if (used >= max_blocks)
  {
   result = 3;
   break;
  }
  s_z_input_packet *input = 0;
  s_z_transform_state *transform = 0;
  bool has_input = stream_read_bit(stream);
  if (has_input)
  {
   input = (s_z_input_packet *)handle_allocate(0x34);
   if (input)
   {
    if (!function_ab2f0(stream, input)) result = 3;
   }
   else result = 2;
  }
  bool has_transform = stream_read_bit(stream);
  if (has_transform)
  {
   transform = (s_z_transform_state *)handle_allocate(0x40);
   if (transform)
   {
    if (!function_ab960(stream, transform)) result = 3;
   }
   else result = 2;
  }
  if (!result && !has_input && !has_transform)
   result = 3;
  if (!result)
  {
   s_block_450c94 *block = &blocks[used++];
   block->index = index;
   ((s_item_450cb8 *)block->data)[0].size = input ? 0x34 : 0;
   ((s_item_450cb8 *)block->data)[0].data = input;
   block->unknown00 = NONE;
   block->count = 2;
   ((s_item_450cb8 *)block->data)[0].type = 9;
   ((s_item_450cb8 *)block->data)[1].type = 10;
   ((s_item_450cb8 *)block->data)[1].size = transform ? 0x40 : 0;
   ((s_item_450cb8 *)block->data)[1].data = transform;
  }
  else
  {
   if (input) { long size; release_input_block(input, &size); }
   if (transform) { long size; release_input_block(transform, &size); }
  }
  if (result)
   break;
 }
 *count = used;
 return result;
}

long function_a58d0(long index);

// @retail 0x82b30
void function_82b30(const s_object_relevance_result *source, s_object_relevance_source *result)
{
 if (source->first > 0.0f || source->second > 0.0f)
 {
  long index = source->object_index;
  if (index != NONE)
  {
   long object = function_a58d0(index);
   if (object != NONE)
   {
    result->object_index = object;
    result->identifier = source->identifier;
    result->first = source->first;
    result->second = source->second;
   }
  }
 }
}


#include "unknown_067e10.h"
#include <string.h>

struct s_view_iterator
{
 dword mask;
 long index;
};
bool world_next_view(c_class_6a600 *world, s_view_iterator *iterator, c_simulation_view **view);
struct s_weapon_activity_result;
struct s_unit_state_c6ef0;
void function_82740(const s_weapon_activity_result *input, s_unit_state_c6ef0 *state);
extern real g_4ced44;

// @retail 0x8b660
bool __stdcall function_8b660(s_simulation_world_actor *actor, long *object_index, s_unit_state_c6ef0 *output)
{
 bool result = false;
 s_simulation_world_actor *const *actor_reference = &actor;
 c_simulation_view *view = 0;
 s_view_iterator iterator = { 0xa, 0 };
 world_next_view((c_class_6a600 *)(*actor_reference)->unknown08, &iterator, &view);
 if (view)
 {
  c_vtable_450c94 *source = (c_vtable_450c94 *)((byte *)view->data + 0x5098);
  if (source)
  {
   long index = actor->actor_index + 16;
   dword bit = 1 << index;
   if (source->mask14 & bit)
   {
    s_dword34 activity = source->data18[index];
    source->mask14 &= ~bit;
    dword state[0x1f];
    function_82740((const s_weapon_activity_result *)&activity, (s_unit_state_c6ef0 *)state);
    memcpy(actor->state, state, sizeof(state));
    actor->time = g_510c54->game_time;
   }
  }
 }
 if (actor->time != NONE &&
  (g_510c54->game_time - actor->time) * g_510c54->rate < g_4ced44)
 {
  *object_index = actor->unknown04;
  memcpy(output, actor->state, sizeof(actor->state));
  result = true;
 }
 return result;
}
