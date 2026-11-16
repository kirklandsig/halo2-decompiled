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

// one pair the boxes make: two boxes go to `pairs`; a box and a grid cell put
// the box into the cell's list, or take it out when the pairs stop
__forceinline void report_pair_2e2f70(class_2e1090 *sweep, long owner, long other, unsigned short id,
	pairs_2e1090 *pairs, pair_change_2e2f70 change)
{
	if ((other & 1) == 0)
	{
		pair_2e1090 pair;
		pair.first = (long *)owner;
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
			long index;
			for (index = 0; index < cell->boxes.size; index++)
			{
				if (cell->boxes.data[index] == id)
				{
					break;
				}
			}
			if (index == cell->boxes.size)
			{
				index = -1;
			}
			cell->boxes.remove_at(index);
		}
	}
}

// score 272 (was a stub): the body follows retail's control flow and calls
// instruction for instruction; retail's frame is 8 bytes larger (0x34): it
// keeps the box's owner and each overlap test's result in stack slots of their
// own, so most stack offsets differ by 8. A named overlap result (297), the
// owner read at each report (309), an index loop over the endpoints and a
// named sort order (285) all measured worse; a permuter run (7746 variants)
// reached 206 only with volatile locals.
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
	for (long remaining = count; remaining > 0; remaining--)
	{
		unsigned short const id = endpoints->id;
		element_2e1090 const &box = elements.data[id];
		endpoint_2e0f80 *const axis = axes[0].data;
		endpoint_2e0f80 own = axis[box.slots[4]];
		// the endpoints before the box's own: lower ends open, upper ends close
		if (cursor != box.slots[4])
		{
			do
			{
				endpoint_2e0f80 entry = axis[cursor];
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
					open.size--;
					open.data[found - open.data] = open.data[open.size];
				}
				cursor++;
			} while (cursor != box.slots[4]);
		}
		long const owner = box.owner;
		// the open boxes the box meets on y and z
		long const open_count = open.size;
		for (long i = 0; i < open_count; i++)
		{
			endpoint_2e0f80 const &entry = *(endpoint_2e0f80 const *)&open.data[i];
			if (entry.value & 2)
			{
				element_2e1090 const &other = elements.data[entry.id];
				if (!apart_yz_2e1090(box, other))
				{
					report_pair_2e2f70(this, owner, other.owner, id, pairs, change);
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
				if (!apart_yz_2e1090(box, other))
				{
					report_pair_2e2f70(this, owner, other.owner, id, pairs, change);
				}
			}
		}
		endpoints++;
		own.value &= ~2;
		append_2e2f70(open, *(long *)&own);
	}
}
