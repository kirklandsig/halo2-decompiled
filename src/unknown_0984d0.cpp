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
			dword value = self->entries[handle & 0x3ff].unknown04;
			s_handle_peers *table = self->table;
			if (table->owner->v0(handle, value, a3, stream, reserved_bits, &released[i]))
			{
				result = true;
				continue;
			}
		}
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
void replication_table_add_chain(s_handle_peers *peers, long count, long const *handles, long const *values, long const *others, s_handle_creation *blocks);
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
		for (long validation_index = 0; validation_index < local_0->info.count; validation_index++)
		{
			if (!function_991d0(this, local_0->info.items[validation_index], local_0->info.b[validation_index], (byte const *)&local_0->data + validation_index * 0x10))
				local_6 = false;
		}
		if (!local_6)
			return;
		if (local_0->info.count == 1)
		{
			long local_19 = local_0->count;
			long local_17 = local_0->info.c[0];
			long local_18 = local_0->info.b[0];
			s_handle_peers *local_14 = table;
			local_14->peers[local_2].flags = 1;
			local_14->peers[local_2].unknown01 = (byte)((dword)local_1 >> 28);
			local_14->peers[local_2].mask = 0;
			local_14->owner->v2(local_1, local_18, local_17, local_19, &local_0->data);
		}
		else
			replication_table_add_chain(table, local_0->info.count, (long const *)local_0->info.items, (long const *)local_0->info.b, (long const *)local_0->info.c, (s_handle_creation *)&local_0->data);
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
				/* a chain never outgrows its four-entry buffer */
				__assume(local_9 <= 4);
				if (local_0->info.count == local_9 && memcmp(local_0->info.items, local_8, local_0->info.count * sizeof(long)) == 0)
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
	{
		s_handle_peers *local_14 = table;
		((c_98fb0 *)local_14->owner)->v7(local_1, local_0->info.c[0], local_0->count, &local_0->data);
		break;
	}
	default:
		__assume(0);
	}
}
