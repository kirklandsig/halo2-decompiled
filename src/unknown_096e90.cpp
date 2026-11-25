#include "unknown_11c920.h"
#include <string.h>
#include "globals.h"
#include "unknown_08b110.h"

// @flags /O2 /Ob1 /arch:SSE /Gr

// @retail 0x96e90
void *function_96e90(long size)
{
	s_allocator_globals *globals = g_4d87f8;
	void *block = globals->allocator->allocate(size, 0, 0);
	if (block == 0)
	{
		globals->allocator->compact(0);
		block = globals->allocator->allocate(size, 0, 0);
	}
	if (block != 0)
		globals->count++;
	return block;
}

// @retail 0x970a0
bool c_vtable_450d1c::v0()
{
	return pending > 0;
}

static __forceinline long function_970b1(s_node_450d1c const *arg_0)
{
    bool local_0 = g_510548;
    long local_1 = arg_0->time;
    long local_2;
    if (local_0)
        local_2 = g_51054c;
    else
        local_2 = GetTickCount();
    return local_2 - local_1;
}

static __forceinline void function_970b3(c_vtable_450d1c *arg_0, s_node_450d1c *arg_1, long arg_2, real *arg_3, long *arg_4)
{
    s_owner_450d1c *local_0 = arg_0->owner;
    local_0->manager->v4(arg_1, arg_2, arg_3, arg_4);
}

// @retail 0x970b0
long c_vtable_450d1c::v1(long a1, long max_count, void *entries)
{
	long count = 0;
	s_node_450d1c *node = owner->head;
	s_node_450d1c *next;
	if (node != 0)
	{
		s_entry_450d1c *entry_pointer = (s_entry_450d1c *)entries;
		while (node != 0)
		{
			next = node->next;
			if (count >= max_count)
				break;
			dword mask = 1 << player;
			if ((node->active_mask & mask) != 0 && (node->done_mask & mask) == 0)
			{
				if (node->timeout != NONE && function_970b1(node) >= node->timeout)
				{
					pending--;
					s_owner_450d1c *current_owner = owner;
					node->active_mask &= ~(1 << player);
					if (node->active_mask == 0)
					{
						current_owner->manager->v3(node);
						function_89e70(current_owner, node);
					}
				}
				else
				{
					real fraction;
					function_970b3(this, node, a1, &fraction, (long *)&entries);
					long size = *(long *)&entries;
					if (fraction > 0.0f)
					{
						s_entry_450d1c *entry = entry_pointer;
						count++;
						entry_pointer++;
						size += 7;
						if (node->unknown10 != NONE)
							size += 14;
						size++;
						long local_0 = node->unknown14;
						*(volatile long *)&entries = size;
						if (local_0 != NONE)
						{
							size += 14;
							*(volatile long *)&entries = size;
						}
						entry->unknown00 = unknown04;
						entry->unknown04 = fraction;
						entry->node = node;
						entry->unknown0c = NONE;
						entry->size = size;
					}
					else
					{
						long start = node->time;
						if (time_now() - start >= g_4cf474 && node->timeout == NONE)
							node->timeout = 0;
					}
				}
			}
			node = next;
		}
	}
	return count;
}

void simulation_write_entity_identifier(long identifier, s_bitstream *stream);

static __forceinline void sender_write_type(s_bitstream *stream, dword type)
{
	if (type >= 32)
	{
		char message[256];
		message[0] = 0;
		csprintf_256(message, "%u exceeds max value of %u", type, 32);
	}
	function_195720(stream, type, 5);
}

static __forceinline void function_97241(c_vtable_450d1c *arg_0, s_node_450d1c *arg_1, s_bitstream *arg_2)
{
    s_owner_450d1c *local_0 = arg_0->owner;
    local_0->manager->v2(arg_1->unknown04, arg_1->size, arg_1->data, arg_2);
}

static __forceinline void function_97242(s_link_450d1c *arg_0)
{
	long local_0;
	if (!g_4d87f8->allocator->get_info(arg_0, &local_0))
		local_0 = NONE;
	s_allocator_globals *local_1 = g_4d87f8;
	local_1->allocator->release(arg_0, NONE);
	local_1->count--;
}

// @retail 0x97240
void c_vtable_450d1c::v3(s_node_450d1c *node, long a2, long a3, long key, s_bitstream *stream, long reserved_bits)
{
	s_request_450d1c *request = 0;
	long const *items;
	if (requests != 0 && requests->key == key)
		request = requests;
	if (!unknown09 && !request)
	{
		s_request_450d1c *block = (s_request_450d1c *)function_96e90(sizeof(s_request_450d1c));
		if (block)
		{
			block->key = NONE;
			block->links = 0;
			block->next = 0;
			block->next = requests;
			requests = block;
			request_count++;
			request = block;
			block->links = 0;
			block->key = key;
		}
		else
		{
			request = 0;
			unknown09 = 1;
		}
	}
	if (!unknown09 && request)
	{
		s_link_450d1c *link = (s_link_450d1c *)function_96e90(sizeof(s_link_450d1c));
		if (link)
		{
			link->node = 0;
			link->next = 0;
			stream->checkpoints[stream->checkpoint_count] = stream->bit_position;
			stream->checkpoint_count++;
			stream_write_bit(stream, true);
			sender_write_type(stream, node->unknown04);
			items = &node->unknown10;
			long remaining = 2;
			do
			{
				long item = *items;
				stream_write_bit(stream, item != NONE);
				if (item != NONE)
					simulation_write_entity_identifier(item, stream);
				items++;
			} while (--remaining);
			if (node->size > 0)
				function_97241(this, node, stream);
			if ((stream->size_in_bytes << 3) - stream->bit_position >= reserved_bits)
			{
				stream->checkpoint_count--;
				link->next = request->links;
				request->links = link;
				link->node = node;
				pending--;
				unknown24++;
				node->done_mask |= 1 << player;
			}
			else
			{
				stream->function_194710(true);
				function_97242(link);
			}
		}
		else
			unknown09 = 1;
	}
}

PRIVATE __forceinline long function_974c1(c_vtable_450d1c *arg_0, long arg_1, dword *arg_2, long *arg_3, dword *arg_4, s_bitstream *arg_5)
{
    s_owner_450d1c *local_0 = arg_0->owner;
    return local_0->manager->v0(arg_1, arg_2, 1, arg_3, arg_4, arg_5);
}

// @retail 0x974c0
long c_vtable_450d1c::v5(dword a1, s_bitstream *stream, long max_blocks, s_block_450c94 *blocks, long *count_out)
{
	long count = 0;
	long error = 0;
	if (unknown09)
		error = 2;
	else
	{
		s_bitstream *local_0 = stream;
		stream = (s_bitstream *)blocks;
		for (;;)
		{
			if (!stream_read_bit(local_0))
				break;
			if (count >= max_blocks)
			{
				error = 3;
				break;
			}

			long type = function_1959c0(local_0, 5);
			if (type < 0 || type >= 32)
			{
				error = 3;
				break;
			}

			long *local_1 = (long *)&blocks;
			*local_1 = 0;
			dword items[2];
			dword data[2];
			for (long i = 0; i < 2; i++)
			{
				if (stream_read_bit(local_0))
				{
					dword low = function_1959c0(local_0, 10);
					items[i] = low | ((byte)function_1959c0(local_0, 4) << 28);
				}
				else
					items[i] = NONE;
			}

			error = function_974c1(this, type, items, local_1, data, local_0);
			if (error != 0)
				break;

			s_block_450c94 *block = (s_block_450c94 *)stream;
			stream = (s_bitstream *)(block + 1);
			long produced = *local_1;
			(*(long volatile *)&count)++;
			block->index = NONE;
			block->unknown00 = type;
			block->count = produced;
			if (produced > 0)
				memcpy(block->data, data, produced * 8);
			*(dword *)&block->unknown08[0] = items[0];
			*(dword *)&block->unknown08[4] = items[1];
		}
	}
	*count_out = count;
	return error;
}
// @retail 0x97690
void c_vtable_450d1c::v6(s_block_450c94 *block)
{
	s_block_450c94 *volatile *local_0 = &block;
	s_block_450c94 *local_1 = *local_0;
	owner->manager->v1(local_1->unknown00, (dword *)&local_1->unknown08[0], local_1->count, (dword *)local_1->data);
}

PRIVATE __forceinline void function_976f1(s_request_450d1c *arg_0)
{
	long local_0;
	if (!g_4d87f8->allocator->get_info(arg_0, &local_0))
		local_0 = NONE;
	s_allocator_globals *local_1 = g_4d87f8;
	local_1->allocator->release(arg_0, NONE);
	local_1->count--;
}

// @retail 0x976f0
void c_vtable_450d1c::function_976f0(long key, bool flag)
{
	s_request_450d1c **link = &requests;
	s_request_450d1c *request = *link;
	while (request != 0 && request->key != key)
	{
		link = &request->next;
		request = *link;
	}
	if (request == 0)
		return;

	s_link_450d1c *current = request->links;
	while (current != 0)
	{
		s_node_450d1c *node = current->node;
		s_link_450d1c *next = current->next;
		node->done_mask &= ~(1 << player);
		unknown24--;
		if (!flag)
			pending++;
		else
		{
			unknown1c++;
			s_owner_450d1c *current_owner = owner;
			node->active_mask &= ~(1 << player);
			if (node->active_mask == 0)
			{
				current_owner->manager->v3(node);
				function_89e70(current_owner, node);
			}
		}

		long info;
		if (!g_4d87f8->allocator->get_info(current, &info))
			info = NONE;
		s_allocator_globals *globals = g_4d87f8;
		globals->allocator->release(current, NONE);
		globals->count--;
		current = next;
	}

	*link = request->next;
	request_count--;
	function_976f1(request);
}
// @retail 0x976c0
void c_vtable_450d1c::v7(void *a1)
{
	function_976f0((long)a1, true);
}

// @retail 0x976d0
void c_vtable_450d1c::v8(long a1, bool a2)
{
	function_976f0(a1, a2);
}

// @retail 0x97830
s_sub_450d14 *c_vtable_450d14::v0()
{
	s_world_450d14 *state = world;
	s_key_450d14 key = state->key;
	void *table = state->unknown0c;
	sub.owner = this;
	sub.count = 0;
	if (((1 << state->kind) & 0x14) != 0)
	{
		byte local_0 = *(byte volatile *)&state->unknown78;
		sub.flag = local_0 == 0;
		sub.value = state->unknown7c;
		s_game_options_view *options = g_4e6948;
		if (options != 0 && options->flag1120 != 0)
		{
			s_record_pool *local_1 = g_4e0300;
			s_sub_450d14 *local_2 = &sub;
			for (long i = 0; i < 4; i++)
			{
				s_match_450d14 *match = function_6a3b0(table, &key, i);
				if (match != 0)
				{
					long player_index = match->unknown04 & 0xffff;
					s_player_450d14 *player = &((s_player_450d14 *)g_4e8c24->data)[player_index];
					long handle = player->unknown2c;
					if (handle == NONE)
					{
						handle = player->unknown30;
						if (handle == NONE)
							continue;
					}
					s_object_450d14 *object = ((s_header_450d14 *)local_1->data)[handle & 0xffff].object;
					sub.entries[*(long volatile *)&sub.count].unknown04 = object->unknownd4;
					sub.entries[*(long volatile *)&sub.count].unknown08 = object->unknown64;
					sub.entries[*(long volatile *)&sub.count].unknown14 = object->unknown15c;
					sub.entries[*(long volatile *)&sub.count].unknown20 = object->unknown241;
					sub.entries[*(long volatile *)&sub.count].unknown00 = player_index;
					sub.count++;
				}
			}
		}
	}
	else
	{
		sub.flag = 0;
		sub.value = NONE;
	}
	return &sub;
}

// @retail 0x979b0
void c_vtable_450d14::v1(long a1, long a2, real *a3, long *a4)
{
	*a3 = 0.0f;
	*a4 = 0;
	long handle = function_699a0(world->unknown0c, a1);
	if (handle != NONE)
		function_82a40(handle, (s_relevance_observers const *)a2, a3, a4);
}
// @retail 0x979f0
bool function_979f0(s_world_450cb8 *world, long handle)
{
	s_table_450cb8 *table = &world->a->block->table;
	long index = handle & 0x3ff;
	bool result = false;
	if ((handle & 0x3ff) >= 0 && (dword)(handle & 0x3ff) < 0x400 && table->slots[index].handle == handle)
	{
		byte flags = table->flags->entries[index].flags;
		if (flags & 4)
			result = (flags >> 1) & 1;
	}
	return result;
}
// @retail 0x97a30
bool c_vtable_450cf4::v2(bool *a1)
{
	bool result = false;
	for (long i = 0; i < 3; i++)
	{
		c_interface_450c94 *child = children[i];
		if (child != 0 && child->v0())
		{
			result = true;
			*a1 = result;
		}
	}
	return result;
}

// @retail 0x97a70
long c_vtable_450cf4::v3(long a1, long a2)
{
	return unknown24;
}
