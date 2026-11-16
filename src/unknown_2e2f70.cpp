// @flags /O2 /GL- /GX
// Havok (third-party physics library): the three-axis sweep (axis_sweep_2e1090.h)
// reporting the pairs a batch of boxes starts or stops, from each box's lower x
// endpoint: the endpoints are sorted, then a sweep along x keeps the boxes whose
// x intervals are open, and each box meets the open ones and those that start
// inside its own interval (src/unknown_2e50d0.cpp calls it).
#include "sweep_batches_2e50d0.h"
#include "sweep_search_2e1090.h"

// sorts the entries from low to high (both included) by key (src/unknown_2dfec0.cpp)
void __cdecl function_2dff20(entry_2dfec0 *entries, int low, int high, sorted_2dfec0 order);

// the long after the last one, the array grown first when full
__forceinline void append_2e2f70(array_longs_2e1ab0 &array, long value)
{
	if (array.size == (long)(array.capacity_and_flags & 0x7fffffff))
	{
		function_2d91d0(&array, sizeof(long));
	}
	array.data[array.size] = value;
	array.size++;
}

// where the cell's list holds the box, or -1
__forceinline long find_box_2e2f70(cell_2e1090 const *cell, unsigned short id)
{
	for (long index = 0; index < cell->boxes.size; index++)
	{
		if (cell->boxes.data[index] == id)
		{
			return index;
		}
	}
	return -1;
}

// one pair the boxes make: two boxes go to `pairs`; a box and a grid cell put
// the box into the cell's list, or take it out when the pairs stop
__forceinline void report_pair_2e2f70(class_2e1090 *sweep, pair_2e1090 &pair, long other, unsigned short id,
	pairs_2e1090 *pairs, pair_change_2e2f70 change)
{
	if ((other & 1) == 0)
	{
		pair.second = other;
		pairs->append(pair);
	}
	else
	{
		cell_2e1090 *cell = (cell_2e1090 *)((char *)sweep->cells + (other & ~1));
		if (change.starting)
		{
			cell->boxes.append(id);
		}
		else
		{
			cell->boxes.remove_at(find_box_2e2f70(cell, id));
		}
	}
}

// score 126 (was 272): retail stores each overlap test's result although it
// never reads it again, which the one result declared before the loop gives;
// the pair, its first box set once per box, gives retail's frame and slots;
// the endpoint pointer stays in edi once it steps after the append, the walk
// rereads the axis, and the cell's search returns from inside its loop.
// Still: ecx and edx are taken the other way round through the reports, and
// the first test loads the box's lower words first. A result per loop
// (50 placements), the owner as a plain local, the test's statements in six
// orders, and an index loop over the endpoints measured no better.
// @retail 0x2e2f70
void sweep_batches_2e50d0::function_2e2f70(endpoint_2e0f80 *endpoints, long count, pairs_2e1090 *pairs,
	pair_change_2e2f70 change)
{
	sorted_2dfec0 order = {};
	if (count > 1)
	{
		function_2dff20((entry_2dfec0 *)endpoints, 0, count - 1, order);
	}
	longs_2e1ab0 open(0x100);
	long cursor = 1;
	unsigned long apart;
	for (long remaining = count; remaining > 0; remaining--)
	{
		long const id = endpoints->id;
		element_2e1090 const &box = elements.data[id];
		endpoint_2e0f80 own = axes[0].data[box.slots[4]];
		// the endpoints before the box's own: lower ends open, upper ends close
		if (cursor != box.slots[4])
		{
			do
			{
				endpoint_2e0f80 entry = axes[0].data[cursor];
				if ((entry.value & 1) == 0)
				{
					entry.value |= 2;
					append_2e2f70(open, *(long *)&entry);
				}
				else
				{
					long *found = open.data;
					while (((endpoint_2e0f80 *)found)->id != entry.id)
					{
						found++;
					}
					long const index = found - open.data;
					open.size--;
					open.data[index] = open.data[open.size];
				}
				cursor++;
			} while (cursor != box.slots[4]);
		}
		pair_2e1090 pair;
		pair.first = (long *)box.owner;
		// the open boxes the box meets on y and z
		long const open_count = open.size;
		for (long i = 0; i < open_count; i++)
		{
			endpoint_2e0f80 const &entry = *(endpoint_2e0f80 const *)&open.data[i];
			if (entry.value & 2)
			{
				element_2e1090 const &other = elements.data[entry.id];
				apart = apart_yz_2e1090(box, other);
				if (!apart)
				{
					report_pair_2e2f70(this, pair, other.owner, id, pairs, change);
				}
			}
		}
		// the boxes that start inside the box's x interval
		for (long k = ++cursor; k < box.slots[5]; k++)
		{
			endpoint_2e0f80 const &entry = axes[0].data[k];
			if ((entry.value & 1) == 0)
			{
				element_2e1090 const &other = elements.data[entry.id];
				apart = apart_yz_2e1090(box, other);
				if (!apart)
				{
					report_pair_2e2f70(this, pair, other.owner, id, pairs, change);
				}
			}
		}
		own.value &= ~2;
		append_2e2f70(open, *(long *)&own);
		endpoints++;
	}
}
