// @flags /O2 /Gr
#include "object_masks_storage.h"
#include "partition_colliding_storage.h"
#include "partition_noncolliding_storage.h"
/* UNKNOWN_0B68C0.CPP: function_b68c0 (entry 27, dispose) */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "object_list.h"
#include "effects.h"
#include "unknown_0b68c0.h"
#include <string.h>

struct s_callback_entry
{
	void (*callback)(void);
	byte unknown04[0x34];
};

struct s_data_header_40
{
	byte unknown00[0x24];
	c_data_allocator *allocator;
	byte unknown28[0x18];
};

s_callback_entry g_4674ac[3];
s_callback_node *g_4e0330;
s_cluster_partition g_4e0310_partition;
bool g_4de2f0;
s_data_header_40 *g_4de2ec;

// @retail 0xb68c0
void function_b68c0(void)
{
	s_callback_entry *entry = g_4674ac;
	long count = 3;
	do
	{
		if (entry->callback)
		{
			entry->callback();
		}
		entry++;
		count--;
	} while (count);

	for (s_callback_node *node = g_4e0330; node; node = node->next)
	{
		if (node->callback)
		{
			node->callback();
		}
	}

	if (g_4e0310)
	{
		g_4e0310 = 0;
	}
	if (g_4e0318)
	{
		g_4e0318 = 0;
	}
	if (g_4e0314)
	{
		g_4e0314 = 0;
	}

	if (g_4de2f0)
	{
		s_data_header_40 *data = g_4de2ec;
		c_data_allocator *allocator = data->allocator;

		memset(data, 0, sizeof(*data));
		allocator->deallocate(data);

		data_dispose(g_4e0300);
	}

	g_4e0300 = 0;
	g_4de2ec = 0;
	if (g_4de2e0)
	{
		g_4de2e0 = 0;
	}
	if (g_4de2e8)
	{
		g_4de2e8 = 0;
	}
	if (g_4de2e4)
	{
		g_4de2e4 = 0;
	}
	if (g_4de2d4)
	{
		g_4de2d4 = 0;
	}
	if (g_4de2dc)
	{
		g_4de2dc = 0;
	}
	if (g_4de2d8)
	{
		g_4de2d8 = 0;
	}
}


struct s_object_visibility_header_ab
{
    short salt;
    byte flags;
    byte type;
    volatile short cluster;
    byte unknown06[2];
    byte *object;
};
bool object_dispatch_test_cluster_visibility(long object_index, long a, long b);

static __forceinline bool object_cluster_contains_ab(dword const *clusters, long index)
{
    long word_index = index >> 5;
    dword mask = 1 << (index & 31);
    dword flags = clusters[word_index];
    bool result = (flags & mask) != 0;
    return result;
}

// @retail 0xb6d60
bool function_b6d60(long object_index, dword const *clusters)
{
    s_object_visibility_header_ab *header = &((s_object_visibility_header_ab *)g_4e0300->data)[object_index & 0xffff];
    byte *object = header->object;
    bool result = false;
    if ((bool)((*(dword *)(object + 4) >> 1) & 1))
        return true;
    if (((1 << header->type) & 0x80) && object_dispatch_test_cluster_visibility(object_index, (long)clusters, (long)&result))
        return result;
    if (header->cluster != NONE)
        return object_cluster_contains_ab(clusters, header->cluster);
    return result;
}


void function_d4890(void);
void function_d48d0(void);
void function_c0040(void);
void __stdcall function_bc300(long object_index);
extern long *g_4de2d0;
extern s_record_pool *g_4e030c;
struct s_object_list;
extern s_object_list *g_4de2f4;

// @retail 0xb69d0
void function_b69d0(void)
{
    function_d4890();
    for (s_callback_node *node = g_4e0330; node; node = node->next)
    {
        void (*callback)(void) = *(void (**)(void))((byte *)node + 0x18);
        if (callback) callback();
    }
    function_c0040();
    s_record_pool *pool = g_4e0300;
    pool->valid = true;
    record_pool_release_all(pool);
    memset(g_4de2d0, 0xff, 0x280 * sizeof(long));
    memset(g_4de2e0, 0xff, 0x200 * sizeof(long));
    data_make_valid_inlined((s_record_pool *)g_4de2e8);
    data_make_valid_inlined((s_record_pool *)g_4de2e4);
    memset(g_4de2d4, 0xff, 0x200 * sizeof(long));
    data_make_valid_inlined((s_record_pool *)g_4de2dc);
    data_make_valid_inlined((s_record_pool *)g_4de2d8);
    byte *state = (byte *)g_4de2f4;
    state[3] = 0;
    *(short *)(state + 4) = 0;
    *(long *)(state + 0xc) = 0;
    *(long *)(state + 0x10) = 0;
    *(long *)(state + 0x14) = 0;
    state[0x81] = 0;
    state[0x80] = 0;
    *(long *)(state + 8) = NONE;
}

static inline void pool_invalidate_ab(void *data)
{
    s_record_pool *pool = (s_record_pool *)data;
    if (pool->valid) pool->valid = false;
}

// @retail 0xb6ab0
void function_b6ab0(void)
{
    function_d48d0();
    for (s_callback_node *node = g_4e0330; node; node = node->next)
    {
        void (*callback)(void) = *(void (**)(void))((byte *)node + 0x1c);
        if (callback) callback();
    }
    g_4e030c->valid = false;
    pool_invalidate_ab(g_4e0318);
    pool_invalidate_ab(g_4e0314);
    s_record_pool *pool = g_4e0300;
    if (pool->valid)
    {
        long index = data_datum_index(pool, function_16bc00(pool, 0));
        while (index != NONE)
        {
            function_bc300(index);
            long next = index == NONE ? 0 : (index & 0xffff) + 1;
            index = data_datum_index(pool, function_16bc00(pool, next));
        }
        pool->valid = false;
    }
    pool_invalidate_ab(g_4de2e8);
    pool_invalidate_ab(g_4de2e4);
    pool_invalidate_ab(g_4de2dc);
    pool_invalidate_ab(g_4de2d8);
}

struct s_object_tick_flags_ab
{
    byte unknown00[0xc0];
    word : 3;
    word in_list : 1;
    word : 12;
};

void __stdcall function_bc820(long object_index);
void function_109660(long object_index, long entry_index);
void function_fd910();

/* the data iterator's step over the object headers (g_4e0300) as retail
   inlines it in this file: the next-used-index scan in its do/while form */
static __forceinline s_object_visibility_header_ab *visibility_header_step(s_record_pool_iterator *iterator)
{
    s_record_pool *data = iterator->data;
    long index = data_find_index(data, iterator->index + 1);
    s_object_visibility_header_ab *result;
    if (index != NONE)
    {
        result = (s_object_visibility_header_ab *)(data->data + data->size * index);
        iterator->index = index;
        iterator->datum_index = (result->salt << 16) | index;
    }
    else
    {
        iterator->index = data->maximum_count;
        iterator->datum_index = NONE;
        result = 0;
    }
    return result;
}

// score 2 (was 2): the two values are copied into EAX and ECX in the opposite order.
// Unsigned callee parameter declarations 2 each; reversed snapshot declarations 2;
// predeclared loop index 2; header before iterator 2; read-only headers 2; inline depth zero 91.
// BLOCKED-BY 0x109660: its body takes the entry index in EAX and the object in ECX whatever
// the declared order. Callee forms: pool local 2; entries + index 2; header pointer 2;
// entry before object 2; that plus both parameters swapped at all three places 2.
// Round 37 (0x109660 forms): entry pointer computed before the object 2; entry as `entries + index` 2; component
// index read before the entry pointer 2; entry by pointer arithmetic on the cast array 2; entry as an element of
// the cast array 2. Our 0x109660 copies the entry index from EAX (retail ECX into EBX) whatever its body order.
// @retail 0xb7060
void function_b7060()
{
    long *indices = g_5107f0->object_indices;
    long count = g_5107f0->object_count;
    ((byte *)g_4de2f4)[3] = 1;
    for (long i = 0; i < count; i++)
    {
        long object_index = indices[i];
        s_object_visibility_header_ab *header = &((s_object_visibility_header_ab *)g_4e0300->data)[object_index & 0xffff];
        if ((header->flags & 4) && !(header->flags & 8))
        {
            function_bc820(object_index);
            function_109660(object_index, i);
        }
    }
    s_record_pool_iterator iterator;
    iterator.data = g_4e0300;
    iterator.index = NONE;
    iterator.datum_index = NONE;
    s_object_visibility_header_ab *header;
    while ((header = visibility_header_step(&iterator)) != 0)
    {
        if ((header->flags & 4) && !(header->flags & 8) &&
            !TEST_FIELD_BIT(((s_object_tick_flags_ab *)header->object)->in_list))
            function_bc820(iterator.datum_index);
    }
    function_fd910();
    ((byte *)g_4de2f4)[3] = 0;
}

bool __stdcall function_bc470(long object_index);
void function_109580(long object_index);
void __stdcall function_b83b0(long object_index, bool flag);
void __stdcall function_b8460(long object_index, bool flag);
void function_bf380();

// @retail 0xb6f10
void function_b6f10()
{
    long *indices = g_5107f0->object_indices;
    long count = g_5107f0->object_count;
    ((byte *)g_4de2f4)[3] = 1;
    for (long i = 0; i < count; i++)
    {
        long object_index = indices[i];
        s_object_visibility_header_ab *header = &((s_object_visibility_header_ab *)g_4e0300->data)[object_index & 0xffff];
        if ((header->flags & 1) && (header->flags & 2) && !(header->flags & 8))
        {
            function_bc470(object_index);
            if ((header->flags & 4) && !(header->flags & 8))
                function_109580(object_index);
        }
    }
    s_record_pool_iterator iterator;
    iterator.data = g_4e0300;
    iterator.index = NONE;
    iterator.datum_index = NONE;
    s_object_visibility_header_ab *header;
    while ((header = visibility_header_step(&iterator)) != 0)
    {
        if ((header->flags & 1) && (header->flags & 2) && !(header->flags & 8))
        {
            s_object_tick_flags_ab *object = (s_object_tick_flags_ab *)((s_object_visibility_header_ab *)g_4e0300->data)[iterator.datum_index & 0xffff].object;
            if (!TEST_FIELD_BIT(object->in_list))
                function_bc470(iterator.datum_index);
        }
    }
    ((byte *)g_4de2f4)[3] = 0;
}

// @retail 0xb7150
void function_b7150()
{
    ((byte *)g_4de2f4)[3] = 1;
    s_record_pool_iterator iterator;
    iterator.data = g_4e0300;
    iterator.index = NONE;
    iterator.datum_index = NONE;
    s_object_visibility_header_ab *header;
    while ((header = visibility_header_step(&iterator)) != 0)
    {
        header->flags &= ~0x20;
        if ((header->flags & 8) && (header->flags & 1) && (header->flags & 2) && !(header->flags & 0x10))
        {
            header->flags &= ~8;
            function_bc470(iterator.datum_index);
            if (header->flags & 4)
                function_bc820(iterator.datum_index);
        }
    }
    function_fd910();
    iterator.data = g_4e0300;
    iterator.index = NONE;
    iterator.datum_index = NONE;
    while ((header = visibility_header_step(&iterator)) != 0)
    {
        if (header->flags & 0x10)
        {
            function_b83b0(iterator.datum_index, true);
            function_b8460(iterator.datum_index, true);
        }
    }
    ((byte *)g_4de2f4)[3] = 0;
    function_bf380();
}

void function_b7290(long object_index);
void function_b7300(long object_index);
void __stdcall function_b8540(long object_index);
bool function_a7670(long object_index);
void havok_component_contacts_mark1(long component_index);
void havok_component_contacts_mark2(long component_index);

// @retail 0xb6df0
void __stdcall function_b6df0(dword const *previous, dword const *current, long count)
{
    s_record_pool_iterator iterator;
    iterator.data = g_4e0300;
    iterator.index = NONE;
    iterator.datum_index = NONE;
    s_object_visibility_header_ab *header;
    while ((header = visibility_header_step(&iterator)) != 0)
    {
        if (header->flags & 0x40)
        {
            long object_index = iterator.datum_index;
            bool active = function_b6d60(object_index, current);
            if (header->flags & 1)
            {
                if (!active)
                {
                    byte *object = ((s_object_visibility_header_ab *)g_4e0300->data)[object_index & 0xffff].object;
                    if (!function_a7670(object_index))
                    {
                        if ((bool)((*(dword *)(object + 4) >> 17) & 1))
                            function_b8540(object_index);
                        else if (!(bool)((*(dword *)(object + 4) >> 1) & 1))
                        {
                            function_b7300(object_index);
                            havok_component_contacts_mark2(*(long *)(object + 0xb4));
                        }
                    }
                }
            }
            else if (active)
            {
                byte *object = ((s_object_visibility_header_ab *)g_4e0300->data)[object_index & 0xffff].object;
                function_b7290(object_index);
                havok_component_contacts_mark1(*(long *)(object + 0xb4));
            }
        }
    }
}

void function_b67c0();
void function_b6bb0();
void function_b6c50();

struct s_object_lifecycle_ab
{
    void (*events[8])();
    void (__stdcall *clusters_changed)(dword const *previous, dword const *current, long count);
};

// The object's nine lifecycle slots, including the two unused slots.
s_object_lifecycle_ab g_4411a4 =
{
    { function_b67c0, function_b68c0, function_b69d0, function_b6ab0,
      function_b6bb0, function_b6c50, 0, 0 },
    function_b6df0
};


bool object_or_parent_hidden(long object_index);
bool function_b9d20(long object_index);
void __stdcall function_bef30(long object_index, long remove, long add, long siblings, bool own_flags);
void function_b8b70(long object_index);
void function_bf090(long object_index);
void __stdcall function_10a250(long object_index);
void function_15b220(long object_index, long index);
void __stdcall object_list_erase(long object_index);
void function_bb950(long object_index, bool add, long delta);
void function_a7a60(long object_index);
void object_interpolated_values_clear_all(long object_index);

void function_1ca130(long unit_index, long priority, long value);

// @retail 0xbfc30
void function_bfc30(long unit_index, long value, long priority)
{
    long const *priority_reference = &priority;
    if (unit_index != NONE)
    {
        struct s_header { word identifier; byte flags; byte type; dword unknown04; byte *object; };
        s_header *header = &((s_header *)g_4e0300->data)[unit_index & 0xffff];
        long type_mask = 1;
        if ((type_mask << header->type) & 3)
            function_1ca130(unit_index, *priority_reference, value);
    }
    if (value == 2)
        *(long *)((byte *)g_4de2f4 + 0x14) = g_510c54->game_time;
}

struct s_remove_object_header
{
    word identifier;
    byte flags;
    byte type;
    dword unknown04;
    byte *object;
};



void function_d4830();
void function_1088e0();
void function_bffa0();
struct s_loop_allocator;
__declspec(noinline) s_loop_allocator *game_state_loop_allocator_new(long size, char const *name);
void *function_123d40(char const *name, char const *type, long size);
void function_1cabc0(s_cluster_partition *partition, char const *name, long payload_size);

s_record_pool *data_new(char const *name, long maximum_count, long size, long alignment_bits, c_data_allocator *allocator);

// @retail 0xb67c0
void function_b67c0()
{
    function_d4830();
    function_1088e0();
    function_bffa0();
    g_4e0300 = data_new("object", 0x800, 12, 0, g_510c2c);
    g_4de2ec = (s_data_header_40 *)game_state_loop_allocator_new(0x100000, "objects");
    g_4de2f0 = false;
    byte *state = (byte *)function_123d40(NULL, NULL, 0x84);
    memset(state, 0, 0x84);
    memset(&g_object_masks_storage, 0, sizeof(g_object_masks_storage));
    g_4de2f4 = (s_object_list *)state;
    g_4de2d0 = (long *)function_123d40(NULL, NULL, 0xa00);
    function_1cabc0(&g_partition_colliding_storage, "collideable object", 0x14);
    function_1cabc0(&g_partition_noncolliding_storage, "noncollideable object", 0x14);
}




s_partition_noncolliding_storage g_partition_noncolliding_storage;

s_partition_colliding_storage g_partition_colliding_storage;
