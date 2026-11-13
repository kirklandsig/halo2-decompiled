// @flags /O2 /GL-
// Havok (third-party physics library): a three-axis sweep of boxes
// (axis_sweep_2e1090.h): moving boxes to new bounds, endpoint by endpoint
// along each axis, and listing the pairs that start and stop overlapping.
#include "sweep_quantize_2e1090.h"

// non-zero when the boxes are apart on x or z: a box's upper words less the
// other's lower words go negative
__forceinline unsigned long apart_xz_2e0350(element_2e1090 const &box, element_2e1090 const &other)
{
	short const a = box.slots[5] - other.slots[4];
	short const b = box.slots[3] - other.slots[1];
	short const c = other.slots[5] - box.slots[4];
	short const d = other.slots[3] - box.slots[1];
	return ((a | b) | (c | d)) & 0x8000;
}

// non-zero when the boxes are apart on x or y
__forceinline unsigned long apart_xy_2e0350(element_2e1090 const &box, element_2e1090 const &other)
{
	short const a = box.slots[2] - other.slots[0];
	short const b = box.slots[5] - other.slots[4];
	short const c = other.slots[2] - box.slots[0];
	short const d = other.slots[5] - box.slots[4];
	return ((a | b) | (c | d)) & 0x8000;
}

// whether the boxes are apart on the two axes other than `axis`
__forceinline bool apart_elsewhere_2e0350(int axis, element_2e1090 const &box, element_2e1090 const &other)
{
	return (axis == 0 && apart_yz_2e1090(box, other)) || (axis == 1 && apart_xz_2e0350(box, other))
		|| (axis == 2 && apart_xy_2e0350(box, other));
}

// where `value` sits in the cell's list of boxes, or -1
__forceinline long find_box_2e0350(cell_2e1090 const *cell, unsigned short value)
{
	for (long position = 0; position < cell->boxes.size; position++)
	{
		if (cell->boxes.data[position] == value)
		{
			return position;
		}
	}
	return -1;
}

// a pair of the two boxes' owners appended to `pairs`
__forceinline void add_pair_2e0350(pairs_2e1090 *pairs, element_2e1090 const &box, element_2e1090 const &other)
{
	pair_2e1090 &pair = pairs->next_slot();
	pair.first = (long *)box.owner;
	pair.second = other.owner;
}

// the boxes start overlapping: a pair, or for a grid cell's box, the box
// joins the cell's list
__forceinline void start_overlap_2e0350(cell_2e1090 *cells, element_2e1090 const &box, unsigned long index,
	element_2e1090 const &other, pairs_2e1090 *pairs, bool cell_mode)
{
	if (!cell_mode)
	{
		add_pair_2e0350(pairs, box, other);
	}
	else if ((other.owner & 1) == 0)
	{
		add_pair_2e0350(pairs, box, other);
	}
	else
	{
		((cell_2e1090 *)((char *)cells + (other.owner & ~1)))->boxes.append((unsigned short)index);
	}
}

// the boxes stop overlapping: a pair, or for a grid cell's box, the box
// leaves the cell's list
__forceinline void stop_overlap_2e0350(cell_2e1090 *cells, element_2e1090 const &box, unsigned long index,
	element_2e1090 const &other, pairs_2e1090 *pairs, bool cell_mode)
{
	if (!cell_mode)
	{
		add_pair_2e0350(pairs, box, other);
	}
	else if ((other.owner & 1) == 0)
	{
		add_pair_2e0350(pairs, box, other);
	}
	else
	{
		cell_2e1090 *const cell = (cell_2e1090 *)((char *)cells + (other.owner & ~1));
		long const found = find_box_2e0350(cell, (unsigned short)index);
		cell->boxes.size--;
		cell->boxes.data[found] = cell->boxes.data[cell->boxes.size];
	}
}

// a box's endpoints on one axis moved to their new values: each endpoint
// passed moves one slot over, and when the box starts (or stops) overlapping
// another on this axis while the other two overlap already, the pair is
// added (or removed); on the x axis a grid cell's box instead gains (or
// loses) the box
__forceinline void move_on_axis_2e0350(class_2e1090 *sweep, int axis, element_2e1090 *data, element_2e1090 &box,
	unsigned long index, int new_low, int new_high, bool cell_mode, pairs_2e1090 *added, pairs_2e1090 *removed)
{
	int const low_slot = axis == 0 ? 4 : axis == 1 ? 0 : 1;
	int const high_slot = axis == 0 ? 5 : axis == 1 ? 2 : 3;
	endpoints_2e0f80 &endpoints = sweep->axes[axis];
	// the lower end moves down
	{
		int position = box.slots[low_slot];
		endpoint_2e0f80 *endpoint = endpoints.data + position;
		int value;
		while (new_low < (value = endpoint[-1].value))
		{
			int const other_id = endpoint[-1].id;
			element_2e1090 &other = data[other_id];
			endpoint->value = (unsigned short)value;
			endpoint->id = other_id;
			if (value & 1)
			{
				other.slots[high_slot] = (unsigned short)position;
				if (!apart_elsewhere_2e0350(axis, box, other))
				{
					start_overlap_2e0350(sweep->cells, box, index, other, added, cell_mode);
				}
			}
			else
			{
				other.slots[low_slot] = (unsigned short)position;
			}
			endpoint--;
			position--;
			endpoint->id = (unsigned short)index;
			box.slots[low_slot] = (unsigned short)position;
		}
		endpoint->value = (unsigned short)new_low;
	}
	// the upper end moves up
	{
		endpoint_2e0f80 *endpoint = endpoints.data + box.slots[high_slot];
		int value;
		while (new_high > (value = endpoint[1].value))
		{
			endpoint++;
			box.slots[high_slot]++;
			element_2e1090 &other = data[endpoint->id];
			endpoint[-1] = endpoint[0];
			endpoint->id = (unsigned short)index;
			if ((value & 1) == 0)
			{
				other.slots[low_slot]--;
				if (!apart_elsewhere_2e0350(axis, box, other))
				{
					start_overlap_2e0350(sweep->cells, box, index, other, added, cell_mode);
				}
			}
			else
			{
				other.slots[high_slot]--;
			}
		}
	}
	// the upper end moves down
	{
		endpoint_2e0f80 *endpoint = endpoints.data + box.slots[high_slot];
		int value;
		while (new_high < (value = endpoint[-1].value))
		{
			endpoint--;
			box.slots[high_slot]--;
			element_2e1090 &other = data[endpoint->id];
			endpoint[1] = endpoint[0];
			endpoint->id = (unsigned short)index;
			if ((value & 1) == 0)
			{
				other.slots[low_slot]++;
				if (!apart_elsewhere_2e0350(axis, box, other))
				{
					stop_overlap_2e0350(sweep->cells, box, index, other, removed, cell_mode);
				}
			}
			else
			{
				other.slots[high_slot]++;
			}
		}
		endpoint->value = (unsigned short)new_high;
	}
	// the lower end moves up
	{
		endpoint_2e0f80 *endpoint = endpoints.data + box.slots[low_slot];
		int value;
		while (new_low > (value = endpoint[1].value))
		{
			endpoint++;
			box.slots[low_slot]++;
			element_2e1090 &other = data[endpoint->id];
			endpoint[-1] = endpoint[0];
			endpoint->id = (unsigned short)index;
			if (value & 1)
			{
				other.slots[high_slot]--;
				if (!apart_elsewhere_2e0350(axis, box, other))
				{
					stop_overlap_2e0350(sweep->cells, box, index, other, removed, cell_mode);
				}
			}
			else
			{
				other.slots[low_slot]--;
			}
		}
		endpoint->value = (unsigned short)new_low;
	}
}

// one box moved to its quantized bounds along x (where grid cells' boxes
// gain or lose it), then y, then z
__forceinline void move_box_2e0350(class_2e1090 *sweep, long *owner, quantized_box_2e1090 const &bounds,
	pairs_2e1090 *added, pairs_2e1090 *removed)
{
	unsigned long index = *owner;
	element_2e1090 *data = sweep->elements.data;
	element_2e1090 &box = data[index];
	move_on_axis_2e0350(sweep, 0, data, box, index, bounds.low[0], bounds.high[0], true, added, removed);
	move_on_axis_2e0350(sweep, 1, data, box, index, bounds.low[1], bounds.high[1], false, added, removed);
	move_on_axis_2e0350(sweep, 2, data, box, index, bounds.low[2], bounds.high[2], false, added, removed);
}

// score 18 (was 1044; 927 in this round's first build): the body matches
// retail instruction for instruction but for the scheduling at the end of the
// quantizing: retail reads `removed`, the cursor, the owner and its index
// before storing the second high word and the low words, ours after all six
// stores (passing the cursor to the per-box step, an owner local, the store
// order of the box struct and its alignment were tried).
// moves each box to its new bounds, adding the pairs that start overlapping
// to `added` and those that stop to `removed`
// Further history: the early stores still precede owner/cursor reads.
// Indexed arrays 983; owner index 981; bounds offset 1005; bounds range 979;
// count/cursors 965; indexed owner with bounds step 956; owner range with
// bounds step 949; bounds range with both pointer steps 974.
// Owner word reference 18; owner array-element reference 18; read-only
// owner input 18; returned output reference 18; staged shifted parts 30;
// nested vector expression 26; unsigned-int output fields 18; aligned
// dword array 952. All restored, including the shared operations.
// @retail 0x2e0350
void class_2e1090::function_2e0350(long *const *owners, bounds_2e0020 const *bounds, long count, pairs_2e1090 *added, pairs_2e1090 *removed)
{
	long *const *end = owners + count;
	for (long *const *owner = owners; owner < end; owner++)
	{
		quantized_box_2e1090 box;
		quantize_box_2e1090(this, *bounds, box);
		move_box_2e0350(this, *owner, box, added, removed);
		bounds++;
	}
}
