/* UNKNOWN_096ED0.H: the handle table class of the vtable at 0x450cd0 and the
   aggregate of three of them (vtable 0x450cf4). This is the one declaration of
   the handle table: unknown_097d80.cpp (init, release, mask), unknown_0984d0.cpp
   (v3, v4, v6) and unknown_096ed0.cpp (the rest) all define its members. */

#ifndef UNKNOWN_096ED0_H
#define UNKNOWN_096ED0_H

#include "unknown_11c920.h"
#include "bitstream.h"
#include "globals.h"
#include <stddef.h>

/* the allocation retail inlines into every place that makes a node, item or
   record (function_96e90, built /Ob1, is the out-of-line copy) */
inline void *handle_allocate(long size)
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

/* a 12-byte item of a node: a handle and what is pending for it (1 or 2) */
struct s_handle_item
{
	long handle;
	long kind;
	s_handle_item *next;

	s_handle_item() : kind(0), next(0) {}
	static void *operator new(size_t size) { return handle_allocate(size); }
};

/* a 16-byte record of a node: x08 links the record that supersedes it */
struct s_handle_record
{
	long handle;
	dword mask;
	s_handle_record *link;
	s_handle_record *next;

	s_handle_record() : link(0), next(0) {}
	static void *operator new(size_t size) { return handle_allocate(size); }
};

/* a 16-byte node: one per message in flight */
struct s_handle_node
{
	long handle;
	s_handle_item *items;
	s_handle_record *records;
	s_handle_node *next;
};

/* the owner of the peers table; slot 12 releases a handle */
class c_handle_owner
{
public:
	virtual bool v0(long handle, dword a2, long a3, s_bitstream *stream, long a5, long *released) = 0;
	virtual long v1(long handle, void *a2, void *a3, long a4, long *produced, void *a6, s_bitstream *stream) = 0;
	virtual void v2(long handle, long a2, long a3, long a4, void *a5) = 0;
	virtual void v3() = 0;
	virtual void v4() = 0;
	virtual bool v5(long handle, dword a2, long a3, s_bitstream *stream, long a5, long *released) = 0;
	virtual long v6(long handle, void *a2, long a3, long *produced, void *a5, s_bitstream *stream) = 0;
	virtual void v7() = 0;
	virtual void v8() = 0;
	virtual void v9() = 0;
	virtual void v10(long handle) = 0;
	virtual void v11() = 0;
	virtual void v12(long handle) = 0;
};

struct s_handle_peer
{
	union
	{
		byte flags;
		struct
		{
			byte flag0 : 1;
			byte flag1 : 1;
			byte flag2 : 1;
			byte flag3 : 5;
		};
	};
	byte unknown01;
	word mask;
	dword unknown04;
};

class c_handle_table_450cd0;

/* the replicated handles and the (up to 15) tables that send them
   (lane D's replication_entity_table.cpp manages it) */
struct s_handle_peers
{
	c_handle_owner *owner;
	c_handle_table_450cd0 *tables[15];
	dword table_mask;
	s_handle_peer peers[1024];
	long next_free;
};

struct s_handle_entry
{
	long handle;
	dword unknown04;
	word state;
	short unknown0a;
	dword unknown0c;
	word unknown10;
	word unknown12;
};

/* a block of v5 (the same 0x80 bytes as s_block_450c94 in unknown_08b110.h) */
struct s_handle_block_info
{
	long count;
	dword items[4];
	dword b[4];
	dword c[4];
};

struct s_handle_block_data
{
	void *v[16];
};

struct s_handle_block
{
	long type;
	long index;
	s_handle_block_info info;
	long count;
	s_handle_block_data data;
};

/* a request v6 is asked about */
struct s_request_450cd0
{
	long kind;
	long handle;
};

class c_handle_table_450cd0;

/* the object whose v0 gives the value every child is asked about */
class c_handle_source
{
public:
	virtual long v0() = 0;
};

/* the vtable at 0x450cd0 (9 slots): v3 is 0x984d0, v4 0x98aa0, v5 0x98cf0,
   v6 0x98fb0, v8 0x992d0 */
class c_handle_table_450cd0
{
public:
	virtual bool v0() { return false; }
	virtual long v1(long a1, long max_count, void *entries) { return 0; }
	virtual void v2() {}
	virtual void v3(long a1, long a2, long a3, long a4, long a5, long a6);
	virtual void v4(long a1, s_bitstream *stream);
	virtual long v5(dword a1, s_bitstream *stream, long max_blocks, s_handle_block *blocks, long *count_out);
	virtual void v6(s_request_450cd0 *a1);
	virtual void v7() {}
	virtual void v8(long handle, bool flag);

	void function_97fe0();
	void function_980d0(long handle, dword mask);
	bool function_98120();

	byte unknown04[4];
	byte unknown08;
	byte unknown09;
	byte unknown0a;
	byte unknown0b;
	long shift;
	long bit;
	s_handle_peers *table;
	s_handle_node *head;
	s_handle_node *node;	/* 0x1c */
	long unknown20;
	s_handle_entry entries[1024];
	long unknown5024;
	long unknown5028;
	long unknown502c;
	long unknown5030;
	long unknown5034;
	long unknown5038;
	long unknown503c;
};

/* the vtable at 0x450cf4 (8 slots): an aggregate of three handle tables.
   v2 is 0x97a30, v3 0x97a70 and v4 0x97ab0 */
class c_aggregate_450cf4
{
public:
	virtual void v0() {}
	virtual void v1() {}
	virtual bool v2(bool *a1) { return false; }
	virtual long v3(long a1, long a2) { return 0; }
	virtual bool v4(long a1, s_bitstream *stream, long a3, long reserved_bits);
	virtual void v5() {}
	virtual void v6() {}
	virtual void v7() {}

	byte unknown04[8];
	c_handle_table_450cd0 *children[3];
	byte unknown18[0xc];
	long unknown24;
	c_handle_source *source;
};

c_handle_table_450cd0 *function_99690(c_handle_table_450cd0 *self, long index, long new_state);

#endif