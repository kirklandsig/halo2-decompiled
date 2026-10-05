// @flags /O2 /Gr
/* REPLICATION_ENTITY_TABLE.CPP: the table of replicated handles (1024 entries
   of 8 bytes, a 4-bit sequence in the handle's top bits) and the up to 15
   handle tables that send them (lane D) */

#include "unknown_11c920.h"
#include "unknown_096ed0.h"
#include <string.h>

#define HANDLE_INDEX(handle) ((handle) & 0x3ff)

// @retail 0x89d20
long replication_table_get_chain(s_handle_peers *peers, long handle, long *handles)
{
	long count;

	for (count = 0; handle != NONE; count++)
	{
		handles[count] = handle;
		handle = peers->peers[HANDLE_INDEX(handle)].unknown04;
	}
	return count;
}

// @retail 0x89d50
long replication_table_find_in_chains(s_handle_peers *peers, long *handles, long handle)
{
	long i;
	s_handle_peer *peer;
	for (i = 0, peer = peers->peers; i < 1024; i++, peer++)
	{
		byte flags = peer->flags;
		if ((flags & 1) && (flags & 8))
		{
			long count = replication_table_get_chain(peers, (peer->unknown01 << 28) | i, handles);
			for (long j = 0; j < count; j++)
			{
				if (handles[j] == handle)
					return count;
			}
		}
	}
	return 0;
}

// @retail 0x89c40
long replication_table_allocate(s_handle_peers *peers)
{
	long first = peers->next_free;
	long result = NONE;
	long i;

	for (i = first; i < 1024; i++)
	{
		if (!(peers->peers[i].flags & 1))
		{
			result = i;
			break;
		}
	}
	if (result == NONE)
	{
		for (i = 0; i < peers->next_free; i++)
		{
			if (!(peers->peers[i].flags & 1))
			{
				result = i;
				break;
			}
		}
	}
	if (result != NONE)
	{
		peers->peers[result].flags = 1;
		peers->next_free = (result + 1) % 1024;
	}
	return result;
}

// @retail 0x89cc0
long replication_table_create(s_handle_peers *peers, long index)
{
	peers->peers[index].flags |= 4;
	peers->peers[index].unknown04 = NONE;
	byte sequence = (peers->peers[index].unknown01 + 1) % 16;
	peers->peers[index].unknown01 = sequence;
	long handle = (sequence << 28) | index;
	for (short i = 0; i < 15; i++)
	{
		if (peers->tables[i])
			function_99690(peers->tables[i], handle, 1);
	}
	return handle;
}

/* makes a chain of count handles (at most four): the first is the chain's
   head, each links the next */
// @retail 0x89470
bool replication_table_create_chain(s_handle_peers *peers, long count, long *handles)
{
	long indices[4];
	long i;

	for (i = 0; i < count; i++)
	{
		indices[i] = replication_table_allocate(peers);
		if (indices[i] == NONE)
			return false;
	}
	for (i = count - 1; i >= 0; i--)
	{
		handles[i] = replication_table_create(peers, indices[i]);
		s_handle_peer *peer = &peers->peers[HANDLE_INDEX(handles[i])];
		if (i == 0)
			peer->flags |= 8;
		else
			peer->flags |= 0x10;
		peer->unknown04 = i + 1 < count ? handles[i + 1] : NONE;
	}
	return true;
}

// @retail 0x89660
void replication_table_release(s_handle_peers *peers, long handle)
{
	s_handle_peer *peer = &peers->peers[HANDLE_INDEX(handle)];
	peer->flag1 = true;
	peer->mask = 0;
	peers->owner->v10(handle);
	peers->owner->v12(handle);
	peer->flags &= 0xfe;
}

// @retail 0x89690
void replication_table_release_chain(s_handle_peers *peers, long count, const long *handles)
{
	long i;

	for (i = count - 1; i >= 0; i--)
	{
		s_handle_peer *peer = &peers->peers[HANDLE_INDEX(handles[i])];
		peer->flags &= 0xe7;
		peer->unknown04 = NONE;
	}
	for (i = count - 1; i >= 0; i--)
		replication_table_release(peers, handles[i]);
}

// @retail 0x89430
void replication_table_reset(s_handle_peers *peers)
{
	for (short i = 0; i < 15; i++)
	{
		if (peers->tables[i])
			peers->tables[i]->function_97fe0();
	}
	memset(peers->peers, 0, sizeof(peers->peers));
	peers->next_free = 0;
}

// @retail 0x893c0
void replication_table_initialize(s_handle_peers *peers)
{
	peers->owner = 0;
	peers->table_mask = 0;
	for (long i = 0; i < 15; i++)
		peers->tables[i] = 0;
	for (short j = 0; j < 15; j++)
	{
		if (peers->tables[j])
			peers->tables[j]->function_97fe0();
	}
	memset(peers->peers, 0, sizeof(peers->peers));
	peers->next_free = 0;
}

/* src/unknown_096ed0.cpp */
void function_995c0(c_handle_table_450cd0 *self, long handle);

/* a handle changed: the owner and every table that sends it hear of it, and
   the owner releases it when no machine holds it */
// @retail 0x89500
void replication_table_update(s_handle_peers *peers, long handle)
{
	s_handle_peer *peer = &peers->peers[HANDLE_INDEX(handle)];
	peer->flag1 = true;
	peers->owner->v10(handle);
	if (peers->table_mask)
	{
		for (long i = 0; i < 15; i++)
		{
			if (peers->table_mask & (1 << i))
				function_995c0(peers->tables[i], handle);
		}
	}
	if (peer->mask == 0)
	{
		peers->owner->v12(handle);
		peer->flags &= 0xfe;
	}
}

/* the same for a whole chain of handles */
// @retail 0x89570
void replication_table_update_chain(s_handle_peers *peers, long handle)
{
	s_handle_peer *head = &peers->peers[HANDLE_INDEX(handle)];
	long handles[4];
	long count = replication_table_get_chain(peers, handle, handles);
	long i;
	for (i = count - 1; i >= 0; i--)
	{
		peers->peers[HANDLE_INDEX(handles[i])].flag1 = true;
		peers->owner->v10(handles[i]);
	}
	if (peers->table_mask)
	{
		for (long table = 0; table < 15; table++)
		{
			if (peers->table_mask & (1 << table))
			{
				for (i = count - 1; i >= 0; i--)
					function_995c0(peers->tables[table], handles[i]);
			}
		}
	}
	if (head->mask == 0)
	{
		for (i = count - 1; i >= 0; i--)
		{
			s_handle_peer *peer = &peers->peers[HANDLE_INDEX(handles[i])];
			peers->owner->v12(handles[i]);
			peer->flags &= 0xfe;
		}
	}
}

/* marks a handle in every table that sends it (as the tables' own
   function_980d0, inlined) */
// @retail 0x89710
void replication_table_mark(s_handle_peers *peers, long handle, dword mask)
{
	for (long i = 0; i < 15; i++)
	{
		c_handle_table_450cd0 *table = peers->tables[i];
		if (table)
		{
			long index = HANDLE_INDEX(handle);
			s_handle_entry *entry = &table->entries[index];
			if (entry->state == 3 && !((1 << table->shift) & table->table->peers[index].mask) && entry->unknown04 == 0)
				table->unknown5034++;
			entry->unknown04 |= mask;
		}
	}
}

class c_handle_owner_with_mask : public c_handle_owner
{
public:
	virtual void slot13() = 0;
	virtual void slot14() = 0;
	virtual dword get_pending_mask(long handle) = 0;
};

// @retail 0x898c0
void replication_table_attach_sender(s_handle_peers *peers, long index, c_handle_table_450cd0 *sender)
{
	peers->tables[index] = sender;
	peers->table_mask |= 1 << index;
	for (long i = 0; i < 1024; i++)
	{
		s_handle_peer *peer = &peers->peers[i];
		if ((peer->flags & 1) && (peer->flags & 4) && !(peer->flags & 2))
		{
			long handle = (peer->unknown01 << 28) | i;
			function_99690(peers->tables[index], handle, 1);
			dword mask = ((c_handle_owner_with_mask *)peers->owner)->get_pending_mask(handle);
			if (mask)
				peers->tables[index]->function_980d0(handle, mask);
		}
	}
}

/* what the owner is given with each handle it is told of */
struct s_handle_creation
{
	dword data[4];
};

/* takes a chain of handles another machine created: the peers record them
   and the owner is told of each */
// @retail 0x89950
void replication_table_add_chain(const long *values, s_handle_peers *peers, long count, const long *handles, const long *others, s_handle_creation *blocks)
{
	long i;
	for (i = 0; i < count; i++)
	{
		long handle = handles[i];
		s_handle_peer *peer = &peers->peers[HANDLE_INDEX(handle)];
		peer->flags = 1;
		peer->unknown01 = (byte)((dword)handle >> 28);
		peer->mask = 0;
		peer = &peers->peers[HANDLE_INDEX(handles[i])];
		if (i == 0)
			peer->flags |= 8;
		else
			peer->flags |= 0x10;
		peer->unknown04 = i + 1 < count ? handles[i + 1] : NONE;
	}
	for (i = 0; i < count; i++)
		peers->owner->v2(handles[i], values[i], others[i], 2, &blocks[i]);
}
