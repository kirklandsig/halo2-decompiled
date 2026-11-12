// @flags /O2 /GL- /GX
// Havok (third-party physics library): a three-axis sweep of boxes
// (axis_sweep_2e1090.h): the endpoints of every box a box overlaps, in three
// sorted lists, one per axis.
#include "sweep_search_2e1090.h"
#include "reference_counted_2d88a0.h"
#include <new>

struct array_vectors_2e1980
{
	__m128 *data;
	long size;
	unsigned long capacity_and_flags;
	array_vectors_2e1980();
	~array_vectors_2e1980();
};

struct vectors_2e1980 : array_vectors_2e1980
{
	__m128 *stack_block;
	long stack_capacity;
	vectors_2e1980(long capacity);
	~vectors_2e1980();
};

void __cdecl function_2e02d0(entry_2e0250 *entries, int low, int high, order_2e0250 context);


// a list of endpoints over storage it does not own
struct endpoint_list_2e3370
{
	endpoint_2e0f80 *data;
	long size;
	unsigned long capacity_and_flags;

	endpoint_list_2e3370(endpoint_2e0f80 *storage, long capacity) : data(storage), size(0), capacity_and_flags(capacity | 0x80000000) {}

	void append(endpoint_2e0f80 const &endpoint)
	{
		data[size] = endpoint;
		size++;
	}
};

// the three lists, and the storage that follows them
struct overlap_endpoints_2e3370
{
	endpoint_list_2e3370 axes[3];
	endpoint_2e0f80 storage[1];
};

// the endpoints of every box the bounds overlap (not the grid cells'), one
// list per axis, each sorted between a sentinel at each end
// score 438 (was 438): frame size agrees; the array and vector slots differ.
// Earlier: array/cursor/sentinel renames and narrower scopes 518, in-place
// loop 436, value append 544, member copies 706. New: range by value 479,
// range pointer 438, first bound by value 475, later word declaration 438,
// reversed test inputs 491, nested search scope 438, scan-word scope 438,
// output postincrement 438. On reversed inputs: early group step 623,
// read-only walk, nested tests, reordered pointers, const output cursor,
// postincrement and cell reference 491; range by value 479. Shared-query
// losses exclude the range-value, bound-value and group-step forms.
// @retail 0x2e3370
void class_2e1090::function_2e3370(bounds_2e0020 const *bounds, overlap_endpoints_2e3370 *lists)
{
	longs_2e1ab0 found(elements.size);
	search_boxes_2e1090(this, *bounds, 0, 0, &found, SEARCH_COLLECT_BOXES_2E1090);

	// two endpoints for each box, and a sentinel at each end
	{
		endpoint_2e0f80 *storage = lists->storage;
		long const capacity = found.size * 2 + 2;
		for (int axis = 0; axis < 3; axis++)
		{
			new (&lists->axes[axis]) endpoint_list_2e3370(storage + axis * capacity, capacity);
		}
	}
	endpoint_2e0f80 sentinel;
	sentinel.value = 0;
	sentinel.id = 0;
	lists->axes[0].append(sentinel);
	lists->axes[1].append(sentinel);
	lists->axes[2].append(sentinel);
	element_2e1090 **kept = (element_2e1090 **)found.data;
	for (long remaining = found.size - 1; remaining >= 0; remaining--)
	{
		lists->axes[0].append(axes[0].data[(*kept)->slots[4]]);
		lists->axes[0].append(axes[0].data[(*kept)->slots[5]]);
		lists->axes[1].append(axes[1].data[(*kept)->slots[0]]);
		lists->axes[1].append(axes[1].data[(*kept)->slots[2]]);
		lists->axes[2].append(axes[2].data[(*kept)->slots[1]]);
		lists->axes[2].append(axes[2].data[(*kept)->slots[3]]);
		kept++;
	}
	endpoint_list_2e3370 *list = lists->axes;
	for (int axis = 3; axis != 0; axis--)
	{
		sort_2e0250((entry_2e0250 *)(list->data + 1), list->size - 1);
		list++;
	}
	sentinel.value = 0xfffc;
	sentinel.id = 0;
	lists->axes[0].append(sentinel);
	lists->axes[1].append(sentinel);
	lists->axes[2].append(sentinel);
}

// score 111 (was 171): first written form 171; frame now agrees. Sort setup
// and loop registers differ. Object argument 141; its scope 141; cell
// reference 130; named count 101 (NOT kept: repeated array pointer reads);
// endpoint cursor 111; range pointer 100 (NOT kept: one use); element
// reference 99; owner pointer 100; shared index 100 (NOT kept: shared local);
// owner mask 40 (NOT kept: extra mask); separate pointers 99 (NOT kept:
// one use); index first 100; int indices 100; while 100; axis reference 100;
// early owner pointer 100 (NOT kept: one use); cell pointer 100; unscoped
// indexed loop 101 (NOT kept: repeated array pointer reads); unscoped cursor 111.
// @retail 0x2e3b00
void class_2e1090::function_2e3b00()
{
	long const count = elements.size;
	vectors_2e1980 saved(count);
	saved.size = count;
	longs_2e1ab0 order(count);
	order.size = count;
	longs_2e1ab0 remap(count);
	remap.size = count;
	for (long i = 0; i < elements.size; i++)
	{
		((element_2e1090 *)saved.data)[i] = elements.data[i];
		((endpoint_2e0f80 *)order.data)[i].value = axes[0].data[elements.data[i].slots[4]].value;
		((endpoint_2e0f80 *)order.data)[i].id = (unsigned short)i;
	}
	long const sortable_count = order.size - 1;
	order_2e0250 context = {};
	if (sortable_count > 1)
	{
		function_2e02d0((entry_2e0250 *)(order.data + 1), 0, sortable_count - 1, context);
	}
	for (long i = 0; i < elements.size; i++)
	{
		remap.data[((endpoint_2e0f80 *)order.data)[i].id] = i;
		elements.data[i] = ((element_2e1090 *)saved.data)[((endpoint_2e0f80 *)order.data)[i].id];
	}
	for (long i = 1; i < elements.size; i++)
	{
		long owner = elements.data[i].owner;
		if ((owner & 1) == 0)
		{
			*(long *)owner = i;
		}
		else
		{
			((cell_2e1090 *)((char *)cells + (owner & ~1)))->box = (unsigned short)i;
		}
	}
	for (long i = 0; i < cells_used; i++)
	{
		cell_2e1090 &cell = cells[i];
		for (long j = cell.boxes.size - 1; j >= 0; j--)
		{
			cell.boxes.data[j] = (unsigned short)remap.data[cell.boxes.data[j]];
		}
	}
	for (long axis = 0; axis < 3; axis++)
	{
		endpoint_2e0f80 *endpoint = axes[axis].data;
		for (long i = 0; i < axes[axis].size; i++)
		{
			endpoint->id = (unsigned short)remap.data[endpoint->id];
			endpoint++;
		}
	}
}
