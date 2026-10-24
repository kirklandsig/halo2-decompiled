// @flags /O2 /Gr
/* UNKNOWN_081780.CPP: builds the network memory (released by 0x81f80): the
   connections, their two kinds of stream and the pools, each taken from the
   top of the physical memory map, with the lowest and highest address and the
   total size kept in g_4d87c8, g_4d87cc and g_4d87c4; then the page heap and
   the allocator over it at g_4d87f8. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_12b400.h"
#include "unknown_0820f0.h"
#include "data_array.h"
#include <new>
#include <string.h>

/* src/unknown_081f80.cpp (layouts as declared there) */
struct s_a0c4_object
{
	byte unknown00[0x2098];
	void *vtable2098;
	byte unknown209c[0xa0ac - 0x209c];
	void *vtable_a0ac;
};

struct s_flagged
{
	byte unknown00[0x29];
	bool field29;
};

extern byte g_5291a0[4];
extern s_a0c4_object *g_4d87e8;
extern s_flagged *g_4d87f0;
extern s_flagged *g_4d87ec;
extern void *g_4d87fc;
extern long g_4d87e0;
extern byte g_4d87e4;
extern long g_4d87c4;
extern long g_4d87c8;
extern long g_4d87cc;

/* the sizes of the page heap for the two kinds of stage */
long g_4cf760;
long g_4cf764;
/* the size of the page heap at g_4d87fc */
long g_4d87f4;

/* tables passed to the allocator's library base */
/* placeholders: retail fills these three tables with data not recovered yet; 0x81780 only passes
   their addresses, so their contents do not affect its code */
dword g_46e490[0x80];
dword g_46e690[0x44];
dword g_46e7a0[4];

struct s_network_observer;
void network_observer_recreate_connections(s_network_observer *observer);

/* a window over sequence numbers, as the streams keep it */
struct s_stream_window_storage
{
	bool valid;
	long capacity;
	long newest;
	long oldest;
	long size;
	long head;
	long count;
};

static __forceinline void stream_window_construct(s_stream_window_storage *window, long capacity)
{
	window->size = capacity;
	window->head = window->count = 0;
	window->valid = false;
	window->capacity = capacity;
}

/* the reliable stream (0x97c bytes, vtable 0x450dd8; src/network_streams.cpp) */
class c_reliable_stream_storage
{
public:
	virtual void v0() {}

	__forceinline c_reliable_stream_storage()
	{
		stream_window_construct(&m_acknowledgement_window, 0x80);
		stream_window_construct(&m_message_window, 0x80);
		m_active = false;
		m_owner = 0;
	}

	bool m_active;
	bool m_unknown05;
	long m_owner;
	s_stream_window_storage m_acknowledgement_window;
	s_stream_window_storage m_message_window;
	byte m_unknown44[0x97c - 0x44];
};

/* the unreliable stream (0x2850 bytes, vtable 0x450db8; src/network_streams.cpp) */
class c_unreliable_stream_storage
{
public:
	virtual void v0() {}

	__forceinline c_unreliable_stream_storage()
	{
		stream_window_construct(&m_message_window, 0x200);
		stream_window_construct(&m_fragment_window, 0x200);
		m_active = false;
		m_owner = 0;
		m_unknown0c = 0;
	}

	bool m_active;
	bool m_unknown05;
	long m_owner;
	void *m_unknown0c;
	s_stream_window_storage m_message_window;
	byte m_messages[0x1800];
	s_stream_window_storage m_fragment_window;
	byte m_fragments[0x1000];
	long m_message_bytes;
	long m_fragment_bytes;
};

/* the two dispatchers of the 0xa0c4 byte object at g_4d87e8 (vtables
   0x450d60 and 0x450cb8) */
class c_dispatcher_450d60_storage
{
public:
	virtual void v0() {}

	__forceinline c_dispatcher_450d60_storage()
	{
		m_unknown04 = 0;
		m_unknown08 = 0;
		m_unknown0c = 0;
		m_unknown10 = 0;
	}

	byte m_unknown04;
	long m_unknown08;
	long m_unknown0c;
	long m_unknown10;
};

class c_dispatcher_450cb8_storage
{
public:
	virtual void v0() {}

	__forceinline c_dispatcher_450cb8_storage()
	{
		m_unknown04 = 0;
		m_unknown08 = 0;
		m_unknown0c = 0;
		m_handlers = 0;
		m_datums = 0;
	}

	byte m_unknown04;
	long m_unknown08;
	long m_unknown0c;
	void *m_handlers;
	void *m_datums;
};

struct s_a0c4_storage
{
	byte unknown00[0x2098];
	c_dispatcher_450d60_storage first;
	byte unknown20ac[0xa0ac - 0x20ac];
	c_dispatcher_450cb8_storage second;
};

/* library bases, not decompiled (src/stubs/memory.cpp) */
class c_library_329d20
{
public:
	c_library_329d20();
	virtual void v0() {}
};

class c_library_32a850
{
public:
	c_library_32a850(void *a1, void *a2, long a3, long a4, long a5, long a6, void *a7, void *heap, long a9, long a10, long a11, long a12, long a13);
	virtual void v0() {}

	byte unknown04[0x30];
};

/* the page heap over g_4d87fc (vtable 0x454970): a 0x1000 byte header, then
   the pages */
class c_network_page_heap : public c_library_329d20
{
public:
	virtual void v0() {}

	__forceinline c_network_page_heap(long size, long *counter)
	{
		m_begin = (byte *)this + 0x1000;
		m_end = (byte *)this + size;
		m_counter = counter;
		memset(m_pages, 0, sizeof(m_pages));
		m_page_count = size / 0x1000 - 1;
		*m_counter = 0;
	}

	byte *m_begin;
	byte *m_end;
	long m_page_count;
	dword m_pages[0x3fb];
	long *m_counter;
};

/* the allocator over the page heap (vtable 0x454980) */
class c_network_page_allocator : public c_library_32a850
{
public:
	virtual void v0() {}

	__forceinline c_network_page_allocator(void *heap) :
		c_library_32a850(g_46e490, g_46e690, 0x800, 0, 0x400, 0x100000, g_46e7a0, heap, 1, 0, 4, 0x400, 0)
	{
	}
};

/* g_4d87f8 (s_allocator_globals, with the heap's block count after it) */
struct s_allocator_holder
{
	c_network_page_allocator *allocator;
	long count;
	byte unknown08;
	long blocks;

	__forceinline s_allocator_holder(void *memory, long size)
	{
		allocator = new c_network_page_allocator(new (memory) c_network_page_heap(size, &blocks));
		count = 0;
		unknown08 = 0;
	}
};

/* the data array set up in place, as function_16b5f0 and function_16b790
   (src/unknown_16b570.cpp) do it; retail inlines both here */
static __forceinline void network_data_array_new(s_record_pool *data, const char *name, long maximum_count, long size, c_data_allocator *allocator)
{
	byte *elements = (byte *)data + sizeof(s_record_pool);

	memset(data, 0, sizeof(s_record_pool));
	strncpy(data->name, name, 0x20);
	data->flag0 = 1;
	data->flag1 = 1;
	data->name[0x1f] = 0;
	data->maximum_count = maximum_count;
	data->size = size;
	data->alignment_bits = 0;
	data->data = 0;
	data->signature = DATA_ARRAY_SIGNATURE;
	data->valid = 0;
	data->bitmap = (dword *)(elements + maximum_count * size);
	data->allocator = allocator;
	data->flag0 = 0;
	data->flag1 = 0;
	data->data = elements;
	memset(data->bitmap, 0, ((maximum_count + 31) >> 5) * 4);
}

static __forceinline void network_memory_track(void *block, long size)
{
	dword end = (dword)block + size;

	if (!g_4d87c4)
	{
		g_4d87c8 = (long)block;
		g_4d87cc = end;
	}
	else
	{
		if ((dword)g_4d87c8 > (dword)block)
		{
			g_4d87c8 = (long)block;
		}
		if ((dword)g_4d87cc <= end)
		{
			g_4d87cc = end;
		}
	}
	g_4d87c4 += size;
}

// @retail 0x81780
// score 22 (was 74): in the first allocation the size and the block take edi
// and esi the other way round, and the two stream constructors store each
// window's head before its count where ours stores the count first
void __stdcall function_81780(long stage)
{
	long size;
	long i;

	if (stage <= 1)
	{
		g_4d87d0 = 15;
		g_4d87f4 = g_4cf760;
		g_4d87e4 = 1;
	}
	else
	{
		g_4d87f4 = g_4cf764;
		g_4d87d0 = 1;
		g_4d87e4 = 0;
	}
	g_4d87e0 = g_4d87d0;

	size = g_4d87d0 * sizeof(s_link_connection);
	g_4d87d4 = physical_memory_malloc_fixed(size, PAGE_READWRITE);
	network_memory_track(g_4d87d4, size);
	for (i = 0; i < g_4d87d0; i++)
	{
		new ((s_link_connection *)g_4d87d4 + i) s_link_connection;
	}

	size = g_4d87d0 * sizeof(c_reliable_stream_storage);
	g_4d87d8 = physical_memory_malloc_fixed(size, PAGE_READWRITE);
	network_memory_track(g_4d87d8, size);
	for (i = 0; i < g_4d87d0; i++)
	{
		new ((c_reliable_stream_storage *)g_4d87d8 + i) c_reliable_stream_storage;
	}

	size = g_4d87d0 * sizeof(c_unreliable_stream_storage);
	g_4d87dc = physical_memory_malloc_fixed(size, PAGE_READWRITE);
	network_memory_track(g_4d87dc, size);
	for (i = 0; i < g_4d87d0; i++)
	{
		new ((c_unreliable_stream_storage *)g_4d87dc + i) c_unreliable_stream_storage;
	}

	size = sizeof(s_record_pool) + g_4d87e0 * 0xb4 + ((g_4d87e0 + 31) >> 5) * 4;
	g_4d87ec = (s_flagged *)physical_memory_malloc_fixed(size, PAGE_READWRITE);
	network_memory_track(g_4d87ec, size);
	network_data_array_new((s_record_pool *)g_4d87ec, "simulation view", g_4d87e0, 0xb4, g_46875c);
	((s_record_pool *)g_4d87ec)->valid = 1;
	record_pool_release_all((s_record_pool *)g_4d87ec);

	if (g_4d87e4)
	{
		g_4d87e8 = (s_a0c4_object *)physical_memory_malloc_fixed(sizeof(s_a0c4_storage), PAGE_READWRITE);
		network_memory_track(g_4d87e8, sizeof(s_a0c4_storage));
		new (g_4d87e8) s_a0c4_storage;

		size = sizeof(s_record_pool) + g_4d87e0 * 0xad30 + ((g_4d87e0 + 31) >> 5) * 4;
		g_4d87f0 = (s_flagged *)physical_memory_malloc_fixed(size, PAGE_READWRITE);
		network_memory_track(g_4d87f0, size);
		network_data_array_new((s_record_pool *)g_4d87f0, "simulation distributed view", g_4d87e0, 0xad30, g_46875c);
		((s_record_pool *)g_4d87f0)->valid = 1;
		record_pool_release_all((s_record_pool *)g_4d87f0);
	}

	g_4d87fc = physical_memory_malloc_fixed(g_4d87f4, PAGE_READWRITE);
	network_memory_track(g_4d87fc, g_4d87f4);
	g_4d87f8 = (s_allocator_globals *)new s_allocator_holder(g_4d87fc, g_4d87f4);

	network_observer_recreate_connections((s_network_observer *)g_5291a0);
}
