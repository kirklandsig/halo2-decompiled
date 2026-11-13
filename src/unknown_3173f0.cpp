// @flags /O2 /GL- /GX
// Havok (third-party physics library): pairs that begin and end, passed to
// the world's dispatcher while an object collects what it hands back: its
// list of 16-byte entries is copied through as the dispatcher goes, into a
// stack array that then becomes the new list.
#include "stack_array_30d740.h"
#include "timer_mark_2ebc80.h"
#include "growable_array_2d91d0.h"

// orders each pair by the second words of its two objects, then the pairs
void __cdecl function_372b50(pair_30d870 *pairs, long count);

class pair_listener_315a10
{
public:
	virtual void slot_0() {}
};

// a reference-counted object that is told of the pairs through its second
// base
class pair_listening_315a10 : public referenced_2d88a0, public pair_listener_315a10
{
};

// what the dispatcher hands the pairs to while it runs
struct pair_handler_3173f0
{
	void const *table;
};

class dispatcher_3728c0
{
public:
	char unknown0[0x24];
	pair_handler_3173f0 *handler;
	char unknown28[0x100 - 0x28];
	// the dispatcher's own handler, put back when a caller is done
	pair_handler_3173f0 own_handler;

	void function_3728c0(pair_30d870 const *added, long added_count, pair_30d870 const *removed,
		long removed_count, pair_listener_315a10 *listener);
};

// the world's collision tolerance
struct tolerance_3173f0
{
	long field_0;
	long field_4;
	float distance;
};

struct world_3173f0
{
	char unknown0[0xcc];
	tolerance_3173f0 *tolerance;
	pair_listening_315a10 *pair_listener;
	char unknownd4[0x138 - 0xd4];
	dispatcher_3728c0 *dispatcher;
	char unknown13c[0x238 - 0x13c];
	long pair_capacity;
};

// one entry of the object's list
struct entry_3173f0
{
	long field_0;
	long field_4;
	long field_8;
	long field_c;
};

struct entries_3173f0
{
	entry_3173f0 *data;
	long size;
	unsigned long capacity_and_flags;
};

struct member_3175e0;
class sweep_3175e0;

class collector_3173f0
{
public:
	char unknown0[0x1c];
	pair_handler_3173f0 handler;
	world_3173f0 *world;
	char unknown24[0x30 - 0x24];
	bool examined;
	char unknown31[0x3c - 0x31];
	member_3175e0 **members;
	long member_count;
	char unknown44[0x68 - 0x44];
	entries_3173f0 entries;
	pair_30d870 *pending;
	long pending_count;
	char unknown7c[0x98 - 0x7c];
	tolerance_3173f0 *tolerance;
	// the entries still to be read, their end, and where the next one kept
	// is written, while the dispatcher runs
	entry_3173f0 *read;
	entry_3173f0 *end;
	entry_3173f0 *write;

	void function_3173f0(pair_30d870 *added, long added_count, pair_30d870 *removed, long removed_count);
};

// the same object as the island's narrow phase knows it
class narrow_phase_317c00 : public collector_3173f0
{
public:
	void function_3175e0(void const *step, void const *input);
};

// score 48 (was 82): Frame 0x14 agrees; pointer/count transfer reaches
// extent 487. First +0x49 differs in the allocation argument carrier, with
// further cursor publication and call setup differences. Pointer countdown 48;
// restored dispatcher 82; increasing pointer count 73; named capacity 76;
// end from saved read 82; combined empty test 82; direct listener 82;
// byte-copy helper 118. The pointer countdown is confirmed by a plain check.
// Round 94: sum reversed 48; array renamed 48; initial base named 48;
// temporary base saved 62; outer branch 48; destination advanced in store
// 48; source advanced in store 47; while countdown 48; source declared
// first 48; input/output pointer names 48. NOT kept: 47 is below the
// required gain; the allocation and call setup still differ.
// Serial search: 17 variants, best 42. NOT kept: gain under eight; a
// constant local, one-use count copies and type/operand changes.
// @retail 0x3173f0
void collector_3173f0::function_3173f0(pair_30d870 *added, long added_count, pair_30d870 *removed, long removed_count)
{
	if (added_count == 0 && removed_count == 0)
	{
		return;
	}
	function_372b50(added, added_count);
	function_372b50(removed, removed_count);
	vectors_30d9a0 kept(entries.size + added_count);
	read = entries.data;
	end = entries.data + entries.size;
	write = (entry_3173f0 *)kept.data;
	tolerance = world->tolerance;
	world->dispatcher->handler = &handler;
	pair_listener_315a10 *listener = world->pair_listener;
	world->dispatcher->function_3728c0(added, added_count, removed, removed_count, listener);
	world->dispatcher->handler = &world->dispatcher->own_handler;
	while (read < end)
	{
		*write = *read;
		write++;
		read++;
	}
	kept.size = write - (entry_3173f0 *)kept.data;
	if ((long)(entries.capacity_and_flags & 0x7fffffff) < kept.size)
	{
		if ((entries.capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(entries.data, (entries.capacity_and_flags & 0x7fffffff) * sizeof(entry_3173f0), 0x12);
		}
		entries.data = (entry_3173f0 *)g_memory_2d88a0->allocate_block(kept.size * sizeof(entry_3173f0), 0x12);
		entries.capacity_and_flags = kept.size;
	}
	entries.size = kept.size;
	entry_3173f0 *destination = entries.data;
	entry_3173f0 const *source = (entry_3173f0 const *)kept.data;
	for (long remaining = kept.size; remaining > 0; --remaining)
	{
		*destination = *source;
		destination++;
		source++;
	}
}

__forceinline void timer_add_3175e0(char const *label)
{
	bool const room = g_480154 < g_480158;
	if (room)
		timer_append_2ebc80(label);
}

// a box around a member's shape
struct bounds_3175e0
{
	float minimum[4];
	float maximum[4];
};

struct motion_3175e0
{
	char unknown0[0x20];
	char transform[0x40];
};

class shape_3175e0
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void get_bounds(void const *transform, float tolerance, bounds_3175e0 *bounds) {}
};

struct collidable_data_3175e0
{
	shape_3175e0 *shape;
	long field_4;
	motion_3175e0 const *motion;
	long field_c;
};

// what the broad phase knows a member by
struct handle_3175e0
{
	long field_0;
};

struct collidable_3175e0 : collidable_data_3175e0, handle_3175e0
{
};

struct member_3175e0
{
	char unknown0[0xc];
	collidable_3175e0 collidable;
};

// the broad phase: its sixth slot sweeps the members' boxes, giving the pairs
// that begin and end
class sweep_3175e0
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void sweep(handle_3175e0 **handles, bounds_3175e0 *bounds, long count, pairs_30d870 *added, pairs_30d870 *removed) {}
};

// the broad phase over the members (the first argument is the broad phase;
// the second is not used): each member's box, grown by half the
// world's tolerance, swept for the pairs that begin and end, the pairs kept
// waiting added to those that begin, all passed on to be dispatched
// @retail 0x3175e0
void narrow_phase_317c00::function_3175e0(void const *step, void const *input)
{
	sweep_3175e0 *sweep = (sweep_3175e0 *)step;
	timer_add_3175e0("LtBroadPhase");
	pairs_30d870 added(world->pair_capacity);
	pairs_30d870 removed(world->pair_capacity);
	long const count = member_count;
	long const extent_bytes = (count * sizeof(bounds_3175e0) + 15) & ~15;
	bounds_3175e0 *bounds = (bounds_3175e0 *)stack_allocate_30d740(extent_bytes);
	long const handles_size = (count * sizeof(handle_3175e0 *) + 15) & ~15;
	handle_3175e0 **handles = (handle_3175e0 **)stack_allocate_30d740(handles_size);
	tolerance_3173f0 const *world_tolerance = world->tolerance;
	float const tolerance = world_tolerance->distance * 0.5f;
	timer_add_3175e0("StCalcAabbs");
	{
		bounds_3175e0 *box = bounds;
		member_3175e0 **member = members;
		handle_3175e0 **handle = handles;
		for (int i = member_count - 1; i >= 0; i--)
		{
			collidable_3175e0 *collidable = &(*member)->collidable;
			*handle = collidable;
			collidable->shape->get_bounds(collidable->motion->transform, tolerance, box);
			box++;
			member++;
			handle++;
		}
	}
	timer_add_3175e0("St3AxisSweep");
	long const sweep_count = member_count;
	sweep->sweep(handles, bounds, sweep_count, &added, &removed);
	timer_add_3175e0("StExamine");
	long const pending_size = pending_count;
	if (pending_size > 0)
	{
		long const old = added.size;
		long const size = old + pending_size;
		long capacity = added.capacity_and_flags & 0x7fffffff;
		if (capacity < size)
		{
			capacity *= 2;
			function_2d9160(&added, size >= capacity ? size : capacity, sizeof(pair_30d870));
		}
		added.size = size;
		for (int j = 0; j < pending_count; j++)
		{
			added.data[old + j] = pending[j];
		}
		pending_count = 0;
	}
	g_memory_2d88a0->release_stack(handles, handles_size);
	g_memory_2d88a0->release_stack(bounds, extent_bytes);
	function_3173f0(added.data, added.size, removed.data, removed.size);
	examined = true;
	timer_add_3175e0("lt");
}
