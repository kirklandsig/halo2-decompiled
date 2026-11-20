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


/* asks each child for its blocks (up to 256 in all), lets each child handle its own range unless the
   request is of type 4, then hands every block item back to the allocator */
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
