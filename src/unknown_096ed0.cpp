#include "unknown_11c920.h"
#include <xtl.h>
#include "globals.h"
#include "unknown_08b110.h"
#include "unknown_096ed0.h"
#include <stdlib.h>
#include <string.h>

// @flags /O2 /arch:SSE /Gr

long replication_table_get_chain(s_handle_peers *peers, long handle, long *handles);
long replication_table_find_in_chains(s_handle_peers *peers, long *handles, long handle);
void replication_table_release(s_handle_peers *peers, long handle);
void replication_table_release_chain(s_handle_peers *peers, long count, long const *handles);

// @retail 0x98150
long c_handle_table_450cd0::v1(long a1, long max_count, void *output)
{
	long count = 0;
	if (unknown09)
	{
		long index = unknown5024;
		if (index != NONE)
		{
			s_entry_450c94 *cursor = (s_entry_450c94 *)output;
			for (; index != NONE && count < max_count; index = entries[index].unknown0a)
			{
				if (entries[index].state)
				{
					long handle = entries[index].handle;
					s_handle_peers *peers = table;
					s_handle_peer *peer = &peers->peers[handle & 0x3ff];
					long type;
					long handles_count;
					real priority;
					long size;
					if (entries[index].state == 1)
					{
						if (peer->flags & 8)
						{
							long handles[4];
							bool blocked = false;
							type = 3;
							handles_count = replication_table_get_chain(table, handle, handles);
							priority = 0.0f;
							size = 0;
							for (long i = 0; i < handles_count; i++)
							{
								real item_priority;
								long item_size;
								s_handle_peers *chain_peers = table;

								chain_peers->owner->v3(handles[i], entries[handles[i] & 0x3ff].unknown04, a1, &item_priority, &item_size);
								if (item_priority > 0.0f)
									priority = priority > item_priority ? priority : item_priority;
								else
									blocked = true;
								size += item_size;
							}
							if (blocked)
								priority = 0.0f;
						}
						else
						{
							if (peer->flags & 0x10)
								continue;
							type = 1;
							handles_count = 1;
							peers->owner->v3(handle, entries[index].unknown04, a1, &priority, &size);
						}
					}
					else if (entries[index].state == 3)
					{
						if (peer->mask & (1 << shift))
						{
							if (peer->flags & 8)
							{
								long handles[4];
								bool blocked = false;
								type = 4;
								handles_count = replication_table_get_chain(table, handle, handles);
								priority = 0.0f;
								size = 0;
								for (long i = 0; i < handles_count; i++)
								{
									real item_priority;
									s_handle_peers *chain_peers = table;

									chain_peers->owner->v9(handles[i], a1, &item_priority);
									if (item_priority > 0.0f)
										priority = priority > item_priority ? priority : item_priority;
									else
										blocked = true;
								}
								if (blocked)
									priority = 0.0f;
							}
							else
							{
								if (peer->flags & 0x10)
									continue;
								type = 2;
								handles_count = 1;
								peers->owner->v9(handle, a1, &priority);
								size = 0;
							}
						}
						else
						{
							if (!entries[index].unknown04)
								continue;
							type = 5;
							handles_count = 1;
							peers->owner->v8(handle, entries[index].unknown04, entries[index].unknown0c, a1, &priority,
								&size);
						}
					}
					else
						continue;
					if (priority > 0.0f)
					{
						s_entry_450c94 *out = cursor;
						count++;
						cursor++;
						out->unknown00 = *(long *)unknown04;
						out->index = index;
						out->unknown04 = priority;
						out->unknown0c = type;
						out->size = size + 14 * handles_count + 3 + (handles_count > 1 ? 2 : 0);
					}
				}
			}
		}
	}
	return count;
}

// @retail 0x991d0
bool function_991d0(c_handle_table_450cd0 *self, long handle, long size, void const *data)
{
	long index = handle & 0x3ff;
	bool result = false;
	if (self->entries[index].state)
	{
		long old_handle = self->entries[index].handle;
		long difference = ((dword)handle >> 28) - ((dword)old_handle >> 28) + 16;
		if (difference > 8)
			difference -= 16;
		if ((difference >= 0 && difference < 4) || difference <= -4)
		{
			self->table->owner->v11(old_handle, handle, size, data);
			s_handle_peers *peers = self->table;
			byte flags = peers->peers[old_handle & 0x3ff].flags;
			if ((bool)((flags >> 3) & 1) || (flags & 0x10))
			{
				long handles[4];
				long count;
				if ((bool)((flags >> 3) & 1))
					count = replication_table_get_chain(peers, old_handle, handles);
				else
					count = replication_table_find_in_chains(peers, handles, old_handle);
				for (long i = 0; i < count; i++)
					function_99690(self, handles[i], 0);
				replication_table_release_chain(self->table, count, handles);
			}
			else
			{
				function_99690(self, old_handle, 0);
				replication_table_release(self->table, old_handle);
			}
			result = true;
		}
	}
	else
		result = true;
	return result;
}

/* the release routine retail inlines here (allocation is handle_allocate in the header) */
static inline void free_block(void *block)
{
	long info;
	if (!g_4d87f8->allocator->get_info(block, &info))
		info = NONE;
	allocator_globals_release(g_4d87f8, block);
}

// @retail 0x99820
const char *function_99820()
{
	return "slayer-engine-globals";
}

__declspec(noinline) long function_98480(long handle, c_handle_table_450cd0 *self);

/* the next live handle after `handle` (NONE starts at the first), or NONE;
   retail keeps it out of line in has_pending_entity, which otherwise inlines it */
// @retail 0x98480
long function_98480(long handle, c_handle_table_450cd0 *self)
{
	long i;
	if (handle == NONE)
		i = 0;
	else
		i = (handle & 0x3ff) + 1;
	long result = NONE;
	for (; i < 0x400; i++)
	{
		if (self->entries[i].state == 1 || self->entries[i].state == 2)
		{
			result = self->entries[i].handle;
			break;
		}
	}
	return result;
}

__declspec(noinline) bool function_99640(c_handle_table_450cd0 *self, long handle);

// @retail 0x99640
bool function_99640(c_handle_table_450cd0 *self, long handle)
{
	long index = handle & 0x3ff;
	bool result = false;
	if (index >= 0 && (dword)index < 0x400)
	{
		if (self->entries[index].handle == handle)
			result = !self->table->peers[index].flag2 || (self->entries[index].state == 3 && !self->table->peers[index].flag1);
	}
	return result;
}

// @retail 0x995c0
void function_995c0(c_handle_table_450cd0 *self, long handle)
{
	long index = handle & 0x3ff;
	s_handle_peer *peer = &self->table->peers[index];
	s_handle_entry *entry = &self->entries[index];
	if (self->entries[index].state == 1 && !(self->entries[index].unknown10 & self->entries[index].state))
	{
		function_99690(self, handle, 0);
		self->unknown5038++;
		return;
	}
	if (self->entries[index].state == 1)
		function_99690(self, handle, 3);
	peer->mask |= (word)(1 << self->shift);
	if (entry->state == 3)
	{
		if (entry->unknown04 != 0)
			self->unknown5034--;
		self->unknown503c++;
	}
}
// @retail 0x98ac0
void function_98ac0(c_handle_table_450cd0 *self, long handle)
{
	s_handle_item *item = new s_handle_item;
	long index = handle & 0x3ff;
	if (item != 0)
	{
		item->next = self->node->items;
		self->node->items = item;
		item->handle = handle;
		item->kind = 1;
	}
	else
		self->unknown0a = 1;
	self->entries[index].unknown10 |= 1;
	function_99690(self, handle, 2);
}

// @retail 0x98b60
void function_98b60(c_handle_table_450cd0 *self, long handle)
{
	s_handle_item *item = new s_handle_item;
	if (item != 0)
	{
		item->next = self->node->items;
		self->node->items = item;
		item->handle = handle;
		item->kind = 2;
	}
	else
		self->unknown0a = 1;
	function_99690(self, handle, 4);
}

// @retail 0x98bf0
void function_98bf0(long handle, c_handle_table_450cd0 *self, dword mask)
{
	s_handle_record *record = new s_handle_record;
	long index = handle & 0x3ff;
	if (record != 0)
	{
		record->next = self->node->records;
		self->node->records = record;
		s_handle_record *found = 0;
		for (s_handle_node *node = self->head; node != 0; node = node->next)
		{
			for (s_handle_record *other = node->records; other != 0; other = other->next)
			{
				if (other->handle == handle)
				{
					found = other;
					break;
				}
			}
		}
		if (found != 0)
			found->link = record;
		record->handle = handle;
		record->mask = mask;
		record->link = 0;
	}
	else
		self->unknown0a = 1;
	self->entries[index].unknown04 &= ~mask;
	self->entries[index].unknown0c = time_now();
	if (self->entries[index].state == 3 && self->entries[index].unknown04 == 0)
		self->unknown5034--;
}
/* ---- the sender state of 0x450d1c (see c_vtable_450d1c in unknown_08b110.h) ---- */
struct s_sender_node;

class c_sender_manager
{
public:
	virtual void v0() = 0;
	virtual void v1() = 0;
	virtual void v2() = 0;
	virtual void v3(s_sender_node *node) = 0;
};

struct s_sender_node
{
	byte unknown00[0x18];
	void *block;
	byte unknown1c[4];
	dword active_mask;
	dword done_mask;
	s_sender_node *next;
};

struct s_sender_owner
{
	byte unknown00[4];
	c_sender_manager *manager;
	byte unknown08[0x40];
	long count;
	s_sender_node *head;
};

struct s_sender_link
{
	s_sender_node *node;
	s_sender_link *next;
};

struct s_sender_request
{
	long key;
	s_sender_link *links;
	s_sender_request *next;
};

struct s_sender
{
	byte unknown00[0xc];
	long player;
	s_sender_request *requests;
	long request_count;
	s_sender_owner *owner;
	long unknown1c;
	long pending;
	long unknown24;
};

static __forceinline void function_96ed1(void *arg_0)
{
	long local_0;
	if (!g_4d87f8->allocator->get_info(arg_0, &local_0))
		local_0 = NONE;
	c_allocator *local_1 = g_4d87f8->allocator;
	s_allocator_globals *local_2 = g_4d87f8;
	local_1->release(arg_0, NONE);
	if (arg_0 != 0)
		local_2->count--;
}

// @retail 0x96ed0
void function_96ed0(s_sender *self)
{
	s_sender_request *request = self->requests;
	self->requests = 0;
	if (request != 0)
	{
		do
		{
			s_sender_link *link = request->links;
			s_sender_request *next_request = request->next;
			if (link != 0)
			{
				do
				{
					s_sender_node *node = link->node;
					s_sender_link *next = link->next;
					node->done_mask &= ~(1 << self->player);
					free_block(link);
					link = next;
				}
				while (link != 0);
			}
			free_block(request);
			request = next_request;
		}
		while (request != 0);
	}

	s_sender_node *node = self->owner->head;
	if (node != 0)
	{
		do
		{
			s_sender_node *next = node->next;
			dword mask = 1 << self->player;
			if (node->active_mask & mask)
			{
				s_sender_owner *owner = self->owner;
				node->active_mask &= ~mask;
				if (node->active_mask == 0)
				{
					owner->manager->v3(node);

					/* 0x89e70 and 0x89eb0, which retail inlines here */
					s_sender_node **link = &owner->head;
					if (*link != 0)
					{
						do
						{
							s_sender_node *current = *link;
							if (current == node)
							{
								*link = node->next;
								break;
							}
							link = &current->next;
						}
						while (*link != 0);
					}
					owner->count--;
					if (node->block != 0)
						function_96ed1(node->block);
					free_block(node);
				}
			}
			node = next;
		}
		while (node != 0);
	}
	self->pending = 0;
	self->unknown1c = 0;
	self->unknown24 = 0;
}
/* ---- the sorting of 0x97ab0 ---- */
struct s_priority_entry_raw { long v[5]; };

struct s_priority_entry
{
	long child;
	real priority;
	long size;
	long unknown0c;
	long index;
};

// @retail 0x97a80
int __cdecl function_97a80(const void *a, const void *b)
{
	real difference = ((const s_priority_entry *)b)->priority - ((const s_priority_entry *)a)->priority;
	if (difference > 0.0f)
		return 1;
	if (difference < 0.0f)
		return -1;
	return 0;
}
/* retail inlines the stream initialization here */
static inline void stream_initialize(s_bitstream *stream, void *buffer, long size, long mode)
{
	stream->mode = mode;
	stream->data = (byte *)buffer;
	stream->size_in_bytes = size;
	stream->unknown08 = 1;
	memset(stream->data, 0, stream->size_in_bytes);
	stream->bit_position = 0;
	stream->checkpoint_count = 0;
	stream->error = false;
	if (stream->mode == 1)
	{
		stream->unknown2c = 0;
		stream->unknown30 = 0;
	}
	else if (stream->mode == 3 || stream->mode == 4)
	{
		if (function_1959c0(stream, 0x20) == 0x64656267)
			stream->error = true;
		else
		{
			stream->bit_position = 0;
			stream->error = false;
		}
	}
}

/* retail inlines the end of a stream here: its size is rounded up to the
   alignment, and its bits are appended to the output */
static __forceinline void function_97ab1(s_bitstream *arg_0, s_bitstream *arg_1)
{
	long *local_0 = &arg_0->size_in_bytes;
	long local_1 = local_0[3];
	long local_2 = local_0[1];
	long local_3 = (local_1 + 7) / 8;
	local_0[0] = local_3;
	long local_4 = local_3 % local_2;
	if (local_4)
		local_0[0] = local_3 - local_4 + local_2;
	local_0[2] = 2;
	function_1955d0(arg_1, (void *)local_0[-1], local_1);
	arg_1->unknown2c += local_0[10];
	arg_1->unknown30 += local_0[11];
}

// @retail 0x97ab0
bool c_aggregate_450cf4::v4(long a1, s_bitstream *out, long a3, long reserved_bits)
{
	s_priority_entry_raw list_raw[2000];
	s_priority_entry *list = (s_priority_entry *)list_raw;
	dword buffers[3][0x180];
	s_bitstream streams[3];
	long total = 0;
	bool overflowed = false;
	long source_value = source->v0();
	for (long i = 0; i < 3; i++)
	{
		c_handle_table_450cd0 *child = children[i];
		if (child != 0 && child->v0())
			total += child->v1(source_value, 2000 - total, &list[total]);
	}
	qsort(list, total, sizeof(s_priority_entry), function_97a80);
	if (total > 0x100)
		total = 0x100;

	long budget = (out->size_in_bytes << 3) - out->bit_position - reserved_bits;
	long k = 0;
	do
	{
		streams[k].mode = 0;
		streams[k].bit_position = 0;
		streams[k].checkpoint_count = 0;
		streams[k].error = false;
		k++;
	}
	while (k < 3);
	budget -= unknown24;
	for (long i = 0; i < 3; i++)
		stream_initialize(&streams[i], buffers[i], 0x600, 1);

	if (a1 != NONE)
	{
		for (long i = 0; i < total; i++)
		{
			long *local_0 = &list_raw[i].v[1];
			long which = local_0[-1];
			if (local_0[1] <= budget)
			{
				c_handle_table_450cd0 *child = children[which];
				s_bitstream *stream = &streams[which];
				if (child != 0)
				{
					long before = stream->bit_position;
					child->v3(local_0[3], local_0[2], source_value, a1, (long)stream, (stream->size_in_bytes << 3) - before - budget);
					budget += before - stream->bit_position;
					if (budget <= 0)
						break;
				}
			}
			else
				overflowed = true;
		}
	}

	for (long i = 0; i < 3; i++)
	{
		c_handle_table_450cd0 *local_1 = children[i];
		if (local_1)
			local_1->v4(a1, &streams[i]);
	}

	for (long i = 0; i < 3; i++)
	{
		if (children[i] != 0)
			function_97ab1(&streams[i], out);
	}	return overflowed;
}
/* the part of 0x992d0 that points the records of earlier nodes, which referred
   to a record about to be freed, at what that record referred to */
static inline void records_relink(s_handle_node *head, s_handle_node *stop, long handle, s_handle_record *record)
{
	s_handle_node *other = head;
	while (other != 0 && other != stop)
	{
		s_handle_record *candidate = other->records;
		other = other->next;
		for (; candidate != 0; candidate = candidate->next)
		{
			if (candidate->handle == handle && candidate->link == record)
			{
				candidate->link = record->link;
				return;
			}
		}
	}
}

// @retail 0x992d0
void c_handle_table_450cd0::v8(long handle, bool flag)
{
	s_handle_node **head_link = &head;
	s_handle_node **link = &head;
	s_handle_node *node = *link;
	while (node != 0)
	{
		if (node->handle == handle)
		{
			s_handle_record *record = node->records;
			if (record != 0)
			{
				do
				{
					long record_handle = record->handle;
					s_handle_record *next_record = record->next;
					long index = record_handle & 0x3ff;
					s_handle_peer *peer = &table->peers[index];
					if (!flag)
					{
						dword mask = record->mask;
						for (s_handle_node *other = node->next; other != 0; other = other->next)
						{
							for (s_handle_record *candidate = other->records; candidate != 0; candidate = candidate->next)
							{
								if (candidate->handle == record_handle)
									mask &= ~candidate->mask;
							}
						}
						if (mask != 0 && entries[index].unknown04 == 0 && entries[index].state == 3)
						{
							if (!(peer->mask & (1 << shift)))
								unknown5034++;
						}
						entries[index].unknown04 |= mask;
					}
					else
						unknown5030++;
					records_relink(*head_link, node, record_handle, record);
					free_block(record);
					record = next_record;
				}
				while (record != 0);
			}

			s_handle_item *item = node->items;
			if (item != 0)
			{
				do
				{
					long item_handle = item->handle;
					s_handle_item *next_item = item->next;
					long offset = (item_handle & 0x3ff) * sizeof(s_handle_peer);
					s_handle_peer *peer = (s_handle_peer *)((byte *)table->peers + offset);
					if (item->kind == 1)
					{
						if (!flag)
						{
							if (peer->mask & (1 << shift))
								function_99690(this, item_handle, 3);
							else
								function_99690(this, item_handle, 1);
						}
						else
						{
							function_99690(this, item_handle, 3);
							unknown5028++;
						}
					}
					else if (!flag)
						function_99690(this, item_handle, 3);
					else
					{
						function_99690(this, item_handle, 0);
						peer->mask &= ~(1 << shift);
						s_handle_peers *peers = table;
						if (((s_handle_peer *)((byte *)peers->peers + offset))->mask == 0)
						{
							peers->owner->v12(item_handle);
							byte *flags = (byte *)peers->peers + offset;
							*flags &= 0xfe;
						}
						unknown5038++;
					}
					free_block(item);
					item = next_item;
				}
				while (item != 0);
			}

			*link = node->next;
			free_block(node);
			return;
		}
		link = &node->next;
		node = *link;
	}
}
// @retail 0x98cf0
long c_handle_table_450cd0::v5(dword a1, s_bitstream *stream, long max_blocks, s_handle_block *blocks, long *count_out)
{
	long error = 0;
	long count = 0;
	if (unknown0a)
		error = 2;
	else
	{
		s_handle_block *cursor = blocks;
		do
		{
			dword type = function_1959c0(stream, 3);
			long n;
			if (type >= 6)
			{
				error = 3;
				break;
			}
			if (type != 0)
			{
				if (type > 2 && type <= 4)
				{
					n = function_1959c0(stream, 2) + 2;
					if (n < 1 || n > 4)
					{
						error = 3;
						break;
					}
				}
				else
					n = 1;
			}
			else
				n = 0;
			if (error != 0)
				break;
			if (count >= max_blocks && n > 0)
			{
				error = 3;
				break;
			}
			if (n == 0)
				break;

			s_handle_block_info info;
			s_handle_block_data data;
			memset(&info, 0, sizeof(info));
			info.count = n;
			long produced = 0;
			for (long i = 0; i < n; i++)
			{
				if (error != 0)
					break;
				dword low = function_1959c0(stream, 10);
				dword item = low | ((byte)function_1959c0(stream, 4) << 28);
				long index = item & 0x3ff;
				info.items[i] = item;
				switch (type)
				{
				case 1:
				case 3:
					error = table->owner->v1(item, &info.b[i], &info.c[i], 8, &produced, data.v, stream);
					break;
				case 2:
				case 4:
					break;
				case 5:
					if (entries[index].handle == (long)item && entries[index].state == 3)
					{
						s_handle_peers *peers = table;
						error = peers->owner->v6(item, info.c, 8, &produced, data.v, stream);
					}
					else
						error = 2;
					break;
				default:
					error = 3;
					break;
				}
			}

			if (error == 0)
			{
				count++;
				s_handle_block *block = cursor;
				cursor++;
				block->type = type;
				block->index = info.items[0];
				block->info = info;
				block->count = produced;
				block->data = data;
			}
			else
			{
				for (long k = 0; k < produced; k++)
				{
					if (data.v[k * 2 + 1] != 0)
					{
						free_block(data.v[k * 2 + 1]);
						data.v[k * 2 + 1] = 0;
					}
				}
			}
		}
		while (error == 0);
	}
	*count_out = count;
	return error;
}
