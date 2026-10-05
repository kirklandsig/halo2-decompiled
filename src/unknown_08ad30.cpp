// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_08AD30.CPP: the simulation entity table (1024 entries of 0x20
   bytes, each owned by one of the entity definitions) and its interface to
   the replication code, the vtable at 0x450d4c */

#include "unknown_11c920.h"
#include "globals.h"
#include "bitstream.h"
#include "unknown_096ed0.h"
#include "unknown_067e10.h"
#include <string.h>

struct s_entry;

/* what an entity's update was last sent with (as src/unknown_0aa4d0.cpp
   defines it) */
struct s_relevance_observers;
struct s_update_state
{
	dword flags;
	long time;
	s_relevance_observers const *observers;
};

/* the observers an update is weighed for, as this code sees them */
struct s_update_observers
{
	byte unknown00[4];
	bool none;
};

/* a block an update carries: the creation data (type 0xc) or the state
   (type 0xd) of an entity */
struct s_update_block
{
	short size;
	short type;
	void *block;
};

/* the entity definitions, as this code calls them */
class c_entry_handler
{
public:
	virtual void slot0() {}
	virtual void slot1() {}
	virtual long get_state_size() { return 0; }
	virtual long get_data_size() { return 0; }
	virtual long get_bit_count() { return 0; }
	virtual dword get_update_mask() { return 0; }
	virtual bool v6(s_entry *entry) { return false; }
	virtual void slot7() {}
	virtual bool v8(s_entry *entry, s_update_observers const *observers) { return false; }
	virtual void v9(s_entry *entry, s_update_observers const *observers, long *bits) {}
	virtual void v10(s_entry *entry, long a, long b, long c) {}
	virtual void v11(s_entry *entry, s_update_state const *state, long *bits) {}
	virtual void v12(long data_size, void *data, long a, s_bitstream *stream) {}
	virtual bool v13(long data_size, void *data, s_bitstream *stream) { return false; }
	virtual bool v14(bool writing, dword mask, dword *written_mask, long state_size, void *state, long a, s_bitstream *stream, long b) { return false; }
	virtual bool v15(bool reading, dword *read_mask, long state_size, void *state, s_bitstream *stream) { return false; }
	virtual void slot16() {}
	virtual void slot17() {}
	virtual void v18(s_entry *entry, long data_size, void *data) {}
	virtual bool save(long, long, long, void *) { return false; }
	virtual bool load(void *entry, long *out, long size, void *buffer) { return false; }
	virtual void advance(void *entry) {}
	virtual bool v22(s_entry *entry, long data_size, void *data, long count, long state_size, void *state) { return false; }
	virtual bool v23(s_entry *entry, long a, long state_size, void *state) { return false; }
	virtual bool v24(s_entry *entry) { return false; }
	virtual bool is_valid(void *entry) { return false; }
};


struct s_entry
{
	dword identifier;
	short handler_index;
	byte unknown06;
	signed char unknown07;
	long unknown08;
	long unknown0c;
	long data_size;
	void *data;
	long state_size;
	void *state;
};

/* an update's state, as v12 of the replication interface reads it */
struct s_update_source
{
	long unknown00;
	void *state;
};

struct c_entry_table
{
	virtual bool slot0(long, long) { return true; }
	virtual bool write_busy(long a, s_bitstream *stream, long c, long d);
	virtual long read_busy(long a, s_bitstream *stream);
	virtual void slot3() {}
	virtual void slot4() {}
	virtual bool write_creation(dword identifier, dword flags, long a, s_bitstream *stream, long b, dword *written_mask);
	virtual long read_creation(long a, long *handler_index_out, dword *mask_out, long b, long *count, s_update_block *blocks, s_bitstream *stream);
	virtual void create(dword identifier, long handler_index, long a, long count, s_update_block *blocks);
	virtual void creation_relevance(dword identifier, dword flags, s_update_observers const *observers, real *relevance, long *bits);
	virtual void v9(dword identifier, long a, long b, long c);
	virtual bool write_update(dword identifier, dword flags, long a, s_bitstream *stream, long b, dword *written_mask);
	virtual long read_update(dword identifier, long *size_out, long a, long *count, s_update_block *blocks, s_bitstream *stream);
	virtual void apply_update(dword identifier, long a, long b, s_update_source const *source);
	virtual void update_relevance(dword identifier, dword flags, long time, s_update_observers const *observers, real *relevance, long *bits);
	virtual void deletion_relevance(dword identifier, long a, real *relevance);
	virtual void v15(dword identifier);
	virtual void slot16(long, long, long, long) {}
	virtual void v17(dword identifier);
	virtual bool function_08ad30(dword identifier);
	virtual void function_08ad70();
	virtual dword function_08add0(dword identifier);
	virtual bool slot21(long) { return true; }
	virtual bool slot22(long) { return true; }
	virtual bool function_0843e0(long a, long b);
	bool initialized;
	bool busy;
	byte unknown06[2];
	long unknown08;
	long unknown0c;
	struct { long count; c_entry_handler *handlers[1]; } *handlers;
	s_entry entries[1024];

	void function_08ae80(dword identifier, short handler_index, long a, long b, long c, long d);
	void function_08aed0(dword identifier);
	bool function_08af70(long handler_index, long *size, void **data);
	bool function_08b010(long handler_index, long *size, void **data);
	void function_08a030();
	void function_08a400(dword identifier);
};

#define ENTRY_INDEX(identifier) ((identifier) & 0x3ff)

struct s_sender_tables;
void replication_table_reset(s_handle_peers *peers);
void replication_table_clear_senders(s_sender_tables *senders);

// @retail 0x68dc0
void simulation_world_reset_replication(c_class_6a600 *world)
{
	world->flag25 = false;
	world->flag2c = false;
	world->flag2e = false;
	if (world->state == 4 || world->state == 5)
	{
		replication_table_reset((s_handle_peers *)world->distribution->peers);
		replication_table_clear_senders((s_sender_tables *)world->distribution->unknown2048);
		((c_entry_table *)&world->distribution->field_2098)->function_08a030();
		world->function_6a6f0();
	}
	if (world->state == 3)
		function_6ab10(world);
}

void replication_table_mark(s_handle_peers *peers, long handle, dword mask);

// @retail 0x8a090
void entry_table_flush_updates(c_entry_table *table)
{
	// The table argument remains on the stack across the handler calls.
	c_entry_table *const *table_reference = &table;
	for (long i = 0; i < 1024; i++)
	{
		s_entry *entry = &table->entries[i];
		if (entry->identifier != NONE && entry->unknown0c)
		{
			if (entry->unknown06)
			{
				c_entry_handler *handler = table->handlers->handlers[entry->handler_index];
				long mask = entry->unknown0c;
				if (handler->load(entry, &mask, entry->state_size, entry->state) && mask)
					replication_table_mark((s_handle_peers *)table->unknown0c, entry->identifier, mask);
			}
			entry->unknown0c = 0;
		}
	}
}

// @retail 0x843e0
bool c_entry_table::function_0843e0(long a, long b)
{
	return false;
}

// @retail 0x8ad30
bool c_entry_table::function_08ad30(dword identifier)
{
	s_entry *entry = &entries[ENTRY_INDEX(identifier)];
	c_entry_handler *handler = handlers->handlers[entry->handler_index];
	bool result = false;
	if (handler->is_valid(entry))
	{
		result = true;
	}
	return result;
}

// @retail 0x8ad70
void c_entry_table::function_08ad70()
{
	for (long i = 0; i < 1024; i++)
	{
		s_entry *entry = &entries[i];
		if (entry->identifier != NONE)
		{
			c_entry_handler *handler = handlers->handlers[entry->handler_index];
			byte salt = (byte)(((long)(entry->identifier >> 28) + 1) % 16);
			entry->identifier = (entry->identifier & 0x3ff) | ((dword)salt << 28);
			handler->advance(entry);
		}
	}
}

// @retail 0x8add0
dword c_entry_table::function_08add0(dword identifier)
{
	s_entry *entry = &entries[ENTRY_INDEX(identifier)];
	c_entry_handler *handler = handlers->handlers[entry->handler_index];
	dword result;
	long size = handler->get_state_size();
	byte buffer[1024];
	memset(buffer, 0, size);
	handler->save(entry->data_size, (long)entry->data, size, buffer);
	long bits = handler->get_bit_count();
	result = (1 << bits) - 1;
	if (!handler->load(entry, (long *)&result, size, buffer))
	{
		result = 0;
	}
	return result;
}

PRIVATE void free_block(void *block)
{
	long dummy;
	g_4d87f8->allocator->get_info(block, &dummy);
	s_allocator_globals *globals = g_4d87f8;
	globals->allocator->release(block, -1);
	if (block)
	{
		globals->count--;
	}
}

// @retail 0x8aed0
void c_entry_table::function_08aed0(dword identifier)
{
	s_entry *entry = &entries[ENTRY_INDEX(identifier)];
	if (entry->data)
	{
		free_block(entry->data);
		entry->data = 0;
		entry->data_size = 0;
	}
	if (entry->state)
	{
		free_block(entry->state);
		entry->state = 0;
		entry->state_size = 0;
	}
	entry->identifier = NONE;
	entry->handler_index = -1;
}

// @retail 0x8ae80
void c_entry_table::function_08ae80(dword identifier, short handler_index, long a, long b, long c, long d)
{
	s_entry *entry = &entries[ENTRY_INDEX(identifier)];
	entry->identifier = identifier;
	entry->handler_index = handler_index;
	entry->unknown06 = 0;
	entry->unknown0c = 0;
	entry->unknown07 = 0;
	entry->unknown08 = NONE;
	entry->data_size = a;
	entry->data = (void *)b;
	entry->state_size = c;
	entry->state = (void *)d;
}

PRIVATE void *allocate_block(long block_size)
{
	s_allocator_globals *globals = g_4d87f8;
	void *block = globals->allocator->allocate(block_size, 0, 0);
	if (!block)
	{
		globals->allocator->compact(0);
		block = globals->allocator->allocate(block_size, 0, 0);
	}
	if (block)
	{
		globals->count++;
	}
	return block;
}

// @retail 0x8af70
bool c_entry_table::function_08af70(long handler_index, long *size, void **data)
{
	bool result = true;
	void *block = 0;
	long block_size = handlers->handlers[handler_index]->get_data_size();
	if (block_size > 0)
	{
		s_allocator_globals *globals = g_4d87f8;
		block = globals->allocator->allocate(block_size, 0, 0);
		if (!block)
		{
			globals->allocator->compact(0);
			block = globals->allocator->allocate(block_size, 0, 0);
		}
		if (block)
		{
			globals->count++;
		}
		if (block)
		{
			memset(block, 0, block_size);
		}
		else
		{
			result = false;
		}
	}
	*size = block_size;
	*data = block;
	return result;
}

// @retail 0x8b010
bool c_entry_table::function_08b010(long handler_index, long *size, void **data)
{
	bool result = true;
	long block_size = handlers->handlers[handler_index]->get_state_size();
	void *block = allocate_block(block_size);
	if (block)
	{
		memset(block, 0, block_size);
	}
	else
	{
		result = false;
	}
	*size = block_size;
	*data = block;
	return result;
}

/* src/unknown_096e90.cpp: the allocation, out of line */
void *function_96e90(long size);

/* src/unknown_0aa4d0.cpp */
real function_aa4d0(long count, long const *entity_indices, real maximum_distance,
	s_relevance_observers const *observers, bool *exact);
real function_abac0(real *relevance_out, struct s_creation_request const *request, s_update_state const *state, long *period_out);

/* the creation weights of the entity types (globals.h) */
struct s_creation_weight_view
{
	real weight;
	real maximum_distance;
	real relevance_bounds[2];
	byte update[0x3c];
};

// @retail 0x89340
bool c_entry_table::write_busy(long a, s_bitstream *stream, long c, long d)
{
	stream_write_bit(stream, initialized);
	return false;
}

// @retail 0x89390
long c_entry_table::read_busy(long a, s_bitstream *stream)
{
	long result = 0;
	if (function_1957d0(stream) != busy)
		result = 1;
	return result;
}

// @retail 0x8a5c0
bool c_entry_table::write_creation(dword identifier, dword flags, long a, s_bitstream *stream, long b, dword *written_mask)
{
	s_entry *entry = &entries[ENTRY_INDEX(identifier)];
	c_entry_handler *handler = handlers->handlers[entry->handler_index];
	*written_mask = 0;
	stream_write_checked(stream, entry->handler_index, 5);
	handler->v12(entry->data_size, entry->data, a, stream);
	dword mask = handler->get_update_mask();
	if (mask)
	{
		mask &= flags;
		stream_write_bit(stream, mask != 0);
		if (mask)
		{
			if (!handler->v14(true, mask, written_mask, entry->state_size, entry->state, a, stream, b) || *written_mask != mask)
				return false;
		}
	}
	return true;
}

static __forceinline void release_block(void *block, long *info)
{
	s_allocator_globals *globals = g_4d87f8;
	globals->allocator->get_info(block, info);
	globals = g_4d87f8;
	globals->allocator->release(block, -1);
	globals->count--;
}

// @retail 0x8a6f0
long c_entry_table::read_creation(long a, long *handler_index_out, dword *mask_out, long b, long *count, s_update_block *blocks, s_bitstream *stream)
{
	long result = 3;
	long handler_index = function_1959c0(stream, 5);
	c_entry_handler *handler = handlers->handlers[handler_index];
	if (handler)
	{
		long data_size = handler->get_data_size();
		long state_size = handler->get_state_size();
		void *data = 0;
		void *state;
		if (data_size > 0)
		{
			data = function_96e90(data_size);
			if (!data)
				result = 2;
		}
		state = handle_allocate(state_size);
		if (!state)
			result = 2;
		if ((!data_size || data) && state)
		{
			if (data_size > 0)
				memset(data, 0, data_size);
			if (handler->v13(data_size, data, stream) && handler->save(data_size, (long)data, state_size, state))
			{
				dword mask = handler->get_update_mask();
				result = 0;
				if (!mask || !function_1957d0(stream) || (handler->v15(true, (dword *)&result, state_size, state, stream) && !(result & ~mask)))
				{
					*handler_index_out = handler_index;
					*mask_out = result;
					blocks[*count].type = 0xc;
					blocks[*count].size = (short)data_size;
					blocks[*count].block = data;
					blocks[*count + 1].type = 0xd;
					blocks[*count + 1].size = (short)state_size;
					blocks[*count + 1].block = state;
					*count += 2;
					return 0;
				}
			}
			result = 3;
		}
		if (data)
			release_block(data, (long *)&stream);
		if (state)
			release_block(state, (long *)&stream);
		return result;
	}
	return 3;
}

// @retail 0x8a920
void c_entry_table::create(dword identifier, long handler_index, long a, long count, s_update_block *blocks)
{
	long data_size = blocks[0].size;
	void *data = blocks[0].block;
	long state_size = blocks[1].size;
	void *state = blocks[1].block;
	memset(blocks, 0, count * sizeof(s_update_block));
	c_entry_handler *handler = handlers->handlers[handler_index];
	function_08ae80(identifier, (short)handler_index, data_size, (long)data, state_size, (long)state);
	s_entry *entry = &entries[ENTRY_INDEX(identifier)];
	entry->unknown06 = handler->v22(entry, entry->data_size, entry->data, a, entry->state_size, entry->state) != false;
}

// @retail 0x8a460
void c_entry_table::creation_relevance(dword identifier, dword flags, s_update_observers const *observers, real *relevance, long *bits)
{
	s_entry *entry = &entries[ENTRY_INDEX(identifier)];
	c_entry_handler *handler = handlers->handlers[entry->handler_index];
	if ((observers->none && !handler->v6(entry)) || !handler->v8(entry, observers))
	{
		*relevance = 0.0f;
		*bits = 0;
	}
	else
	{
		s_creation_weight_view const *weight = (s_creation_weight_view const *)&g_4cef68[entry->handler_index];
		real value;
		if (weight->weight > 0.0f)
		{
			value = weight->weight;
		}
		else
		{
			value = weight->relevance_bounds[1] - weight->relevance_bounds[0];
			value *= function_aa4d0(1, (long const *)&entry->identifier, weight->maximum_distance, (s_relevance_observers const *)observers, 0);
			value += weight->relevance_bounds[0];
		}
		*relevance = value;
		handler->v9(entry, observers, bits);
		dword mask = handler->get_update_mask();
		if (mask)
		{
			(*bits)++;
			s_update_state state;
			state.flags = mask & flags;
			state.time = 0;
			state.observers = (s_relevance_observers const *)observers;
			long update_bits;
			handler->v11(entry, &state, &update_bits);
			*bits += update_bits;
		}
	}
}

// @retail 0x8a580
void c_entry_table::v9(dword identifier, long a, long b, long c)
{
	s_entry *entry = &entries[ENTRY_INDEX(identifier)];
	handlers->handlers[entry->handler_index]->v10(entry, a, b, c);
}

// @retail 0x8aa50
bool c_entry_table::write_update(dword identifier, dword flags, long a, s_bitstream *stream, long b, dword *written_mask)
{
	s_entry *entry = &entries[ENTRY_INDEX(identifier)];
	c_entry_handler *handler = handlers->handlers[entry->handler_index];
	*written_mask = 0;
	if (handler->v14(false, flags, written_mask, entry->state_size, entry->state, a, stream, b) && *written_mask != 0)
		return true;
	return false;
}

/* src/unknown_0a58d0.cpp */
struct s_simulation_entity_table;
struct s_simulation_entity;
s_simulation_entity *simulation_entity_try_get(s_simulation_entity_table *table, long entity_index);

// @retail 0x8aac0
long c_entry_table::read_update(dword identifier, long *size_out, long a, long *count, s_update_block *blocks, s_bitstream *stream)
{
	long result;
	s_entry *entry = 0;
	if (identifier != NONE)
	{
		s_entry *candidate = &entries[ENTRY_INDEX(identifier)];
		if (candidate->identifier == identifier)
			entry = candidate;
	}
	if (!entry)
		return 3;
	c_entry_handler *handler = handlers->handlers[entry->handler_index];
	long state_size = entry->state_size;
	void *state = handle_allocate(state_size);
	if (!state)
		return 3;
	if (stream->mode == 4)
	{
		memset(state, 0, entry->state_size);
		if (!handler->save(entry->data_size, (long)entry->data, entry->state_size, state))
			goto failed;
	}
	else
	{
		memcpy(state, entry->state, entry->state_size);
	}
	long read_size = 0;
	if (!handler->v15(false, (dword *)&read_size, entry->state_size, state, stream))
		goto failed;
	*size_out = read_size;
	blocks[*count].type = 0xd;
	blocks[*count].size = (short)entry->state_size;
	blocks[*count].block = state;
	(*count)++;
	return 0;
failed:
	result = 3;
	release_block(state, (long *)&stream);
	return result;
}

// @retail 0x8ac40
void c_entry_table::apply_update(dword identifier, long a, long b, s_update_source const *source)
{
	s_entry *entry = &entries[ENTRY_INDEX(identifier)];
	c_entry_handler *handler = handlers->handlers[entry->handler_index];
	memcpy(entry->state, source->state, entry->state_size);
	handler->v23(entry, a, entry->state_size, entry->state);
}

// @retail 0x8a9c0
void c_entry_table::update_relevance(dword identifier, dword flags, long time, s_update_observers const *observers, real *relevance, long *bits)
{
	s_entry *entry = &entries[ENTRY_INDEX(identifier)];
	c_entry_handler *handler = handlers->handlers[entry->handler_index];
	if (observers->none)
	{
		*relevance = 0.0f;
		*bits = 0;
	}
	else
	{
		s_update_state state;
		state.flags = flags;
		state.time = time;
		state.observers = (s_relevance_observers const *)observers;
		handler->v11(entry, &state, bits);
		*relevance = function_abac0(0, (struct s_creation_request const *)entry, &state, 0);
	}
}

// @retail 0x8aca0
void c_entry_table::deletion_relevance(dword identifier, long a, real *relevance)
{
	if (entries[ENTRY_INDEX(identifier)].unknown07 > 0)
		*relevance = 0.0f;
	else
		*relevance = 0.85f;
}

// @retail 0x8ace0
void c_entry_table::v15(dword identifier)
{
	s_entry *entry = &entries[ENTRY_INDEX(identifier)];
	if (entry->unknown06)
	{
		handlers->handlers[entry->handler_index]->v24(entry);
		entry->unknown06 = 0;
		entry->unknown0c = 0;
	}
}

// @retail 0x8ad20
void c_entry_table::v17(dword identifier)
{
	function_08aed0(identifier);
}

// @retail 0x8a030
void c_entry_table::function_08a030()
{
	busy = true;
	if (!initialized)
		memset(entries, 0, sizeof(entries));
	for (long i = 0; i < 1024; i++)
	{
		s_entry *entry = &entries[i];
		if (initialized)
		{
			if (entry->identifier != NONE)
				function_08aed0(entry->identifier);
		}
		else
		{
			entry->identifier = NONE;
			entry->handler_index = -1;
		}
	}
	busy = false;
}

// @retail 0x8a400
void c_entry_table::function_08a400(dword identifier)
{
	s_entry *entry = &entries[ENTRY_INDEX(identifier)];
	c_entry_handler *handler = handlers->handlers[entry->handler_index];
	handler->v18(entry, entry->data_size, entry->data);
	handler->save(entry->data_size, (long)entry->data, entry->state_size, entry->state);
	entry->unknown06 = true;
	entry->unknown0c = (1 << handler->get_bit_count()) - 1;
}

/* src/replication_entity_table.cpp */
long replication_table_allocate(s_handle_peers *peers);
long replication_table_create(s_handle_peers *peers, long index);
bool replication_table_create_chain(s_handle_peers *peers, long count, long *handles);

/* frees a block the way the failed creations do */
static __forceinline void discard_block(void *block, long *size)
{
	s_allocator_globals *globals = g_4d87f8;
	if (!globals->allocator->get_info(block, size))
		*size = NONE;
	globals = g_4d87f8;
	globals->allocator->release(block, NONE);
	globals->count--;
}

/* makes a new entity of a handler, with its data and state; the identifier,
   or NONE */
// @retail 0x8a110
long entity_table_new_entity(c_entry_table *table, long handler_index)
{
	long result = NONE;
	long data_size;
	void *data = 0;
	long state_size;
	void *state = 0;

	if (table->function_08af70(handler_index, &data_size, &data) && table->function_08b010(handler_index, &state_size, &state))
	{
		s_handle_peers *peers = (s_handle_peers *)table->unknown0c;
		long identifier = NONE;
		long index = replication_table_allocate(peers);
		if (index != NONE)
			identifier = replication_table_create(peers, index);
		result = identifier;
		if (identifier != NONE)
		{
			table->function_08ae80(identifier, (short)handler_index, data_size, (long)data, state_size, (long)state);
			return result;
		}
	}
	if (data)
		release_block(data, (long *)&table);
	if (state)
		release_block(state, (long *)&table);
	return result;
}

/* makes count (at most four) new entities, chained, of the given handlers */
// @retail 0x8a210
bool entity_table_new_entities(long *identifiers, c_entry_table *table, long count, long const *handler_indices)
{
	void *datas[4] = { 0 };
	void *states[4] = { 0 };
	long field_34[4] = { 0 };
	long state_sizes[4] = { 0 };
	bool result = true;
	long i;

	for (i = 0; i < count; i++)
	{
		if (!table->function_08af70(handler_indices[i], &field_34[i], &datas[i]) ||
			!table->function_08b010(handler_indices[i], &state_sizes[i], &states[i]))
		{
			goto failed;
		}
	}
	if (!replication_table_create_chain((s_handle_peers *)table->unknown0c, count, identifiers))
		goto failed;
	for (i = 0; i < count; i++)
		table->function_08ae80(identifiers[i], (short)handler_indices[i], field_34[i], (long)datas[i], state_sizes[i], (long)states[i]);
	return result;

failed:
	for (i = 0; i < count; i++)
	{
		if (datas[i])
			discard_block(datas[i], (long *)&handler_indices);
		if (states[i])
			discard_block(states[i], (long *)&table);
	}
	return false;
}
