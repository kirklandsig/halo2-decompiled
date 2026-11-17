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
	// a copy of `from`'s endpoint at `index`, after the last one
	void append_from(endpoints_2e0f80 const &from, element_2e1090 const &box, int slot)
	{
		append(from.data[box.slots[slot]]);
	}
};

// the three lists, and the storage that follows them
struct overlap_endpoints_2e3370
{
	endpoint_list_2e3370 axes[3];
	endpoint_2e0f80 storage[1];
};

// a box quantized: its lower words (the low bit cleared) and its upper words
// (the low bit set), widened to 32 bits
struct __declspec(align(16)) quantized_box_2e3370
{
	unsigned long low[4];
	unsigned long high[4];
};

// both corners of a box, the lower one rounded down to an even word and the
// upper one up to an odd word
__forceinline void quantize_box_2e3370(class_2e1090 const *sweep, bounds_2e0020 const &box, quantized_box_2e3370 &out)
{
	corner_words_2e1090 lower;
	corner_words_2e1090 upper;
	quantize_corner_2e1090(box.lower, sweep->lower_offset, sweep->scale,
		*(__m128 const *)g_4125e0, *(__m128 const *)g_4125d0, lower);
	quantize_corner_2e1090(box.upper, sweep->upper_offset, sweep->scale,
		*(__m128 const *)g_4125e0, *(__m128 const *)g_4125d0, upper);
	out.low[0] = lower.words[0] & 0xfffe;
	out.low[1] = lower.words[1] & 0xfffe;
	out.low[2] = lower.words[2] & 0xfffe;
	out.high[0] = upper.words[0] | 1;
	out.high[1] = upper.words[1] | 1;
	out.high[2] = upper.words[2] | 1;
}

// of the boxes whose bits are set, each (not a grid cell's) that overlaps `range`
// on y and z: as a pair with `first_owner` appended to `pairs`, or its address
// appended to `found`
__forceinline void gather_overlaps_2e3370(class_2e1090 *sweep, long *first_owner, long *bits, element_2e1090 const *range,
	pairs_2e1090 *pairs, array_longs_2e1ab0 *found, int mode)
{
	pair_2e1090 pair;
	pair.first = first_owner;
	element_2e1090 *group = sweep->elements.data;
	long const words = (sweep->elements.size >> 5);
	long *word = bits;
	for (; word < bits + words + 1; group += 32, word++)
	{
		unsigned long found_bits = *word;
		if (found_bits == 0)
		{
			continue;
		}
		element_2e1090 *other = group;
		do
		{
			if (found_bits & 0xf)
			{
#define SEARCH_TEST_2E1090(k) \
				if (found_bits & (1 << k)) \
				{ \
					unsigned long const apart = apart_yz_2e1090(*range, other[k]); \
					if (!apart && (other[k].owner & 1) == 0) \
					{ \
						if (mode == SEARCH_REPORT_PAIRS_2E1090) \
						{ \
							pair.second = other[k].owner; \
							pairs->append(pair); \
						} \
						else \
						{ \
							found->data[found->size] = (long)&other[k]; \
							found->size++; \
						} \
					} \
				}
				SEARCH_TEST_2E1090(0)
				SEARCH_TEST_2E1090(1)
				SEARCH_TEST_2E1090(2)
				SEARCH_TEST_2E1090(3)
#undef SEARCH_TEST_2E1090
			}
			other += 4;
			found_bits >>= 4;
		} while (found_bits);
	}
}

// every box (not a grid cell's) the bounds overlap: as a pair with
// `first_owner` appended to `pairs`, or its address appended to `found`
__forceinline void search_boxes_2e3370(class_2e1090 *sweep, bounds_2e0020 const &bounds, long *first_owner,
	pairs_2e1090 *pairs, array_longs_2e1ab0 *found, int mode)
{
	int const count = sweep->elements.size;
	int const words = (count >> 5) + 4;
	long *bits = (long *)take_stack_2e1090((words * sizeof(long) + 15) & ~15);
	clear_blocks_2e1090(bits, count >> 3);
	quantized_box_2e3370 box;
	quantize_box_2e3370(sweep, bounds, box);

	// one bit per box: set for the boxes whose x interval overlaps
	{
		endpoint_2e0f80 *endpoint = sweep->axes[0].data + 1;
		if (sweep->cells_used)
		{
			// start from the grid cell's box: the boxes it holds, less those
			// that start inside it
			int const low_x = box.low[0];
			int const slot = low_x >> (16 - sweep->cell_shift);
			if (slot > 0)
			{
				cell_2e1090 *cells = sweep->cells;
				cell_2e1090 *cell = &cells[slot - 1];
				flip_bit_2e1090(bits, cell->box);
				unsigned short *held = cell->boxes.data;
				for (int remaining = cell->boxes.size - 1; remaining >= 0; remaining--)
				{
					flip_bit_2e1090(bits, *held);
					held++;
				}
				element_2e1090 &cell_box = sweep->elements.data[cell->box];
				endpoint = sweep->axes[0].data + cell_box.slots[4] + 1;
				{
					endpoint_2e0f80 *end = sweep->axes[0].data + cell_box.slots[5];
					while (endpoint < end)
					{
						if ((endpoint->value & 1) == 0)
						{
							drop_bit_2e1090(bits, endpoint->id);
						}
						endpoint++;
					}
				}
				endpoint = sweep->axes[0].data + cell_box.slots[4] + 1;
			}
		}
		while (endpoint->value < box.low[0])
		{
			flip_bit_2e1090(bits, endpoint->id);
			endpoint++;
		}
		while (endpoint->value < box.high[0])
		{
			if ((endpoint->value & 1) == 0)
			{
				flip_bit_2e1090(bits, endpoint->id);
			}
			endpoint++;
		}
	}

	// where the bounds' y and z ends would sit
	element_2e1090 range;
	{
		endpoints_2e0f80 &axis = sweep->axes[1];
		endpoint_2e0f80 *base = axis.data;
		entry_2dfec0 *last = (entry_2dfec0 *)(base + axis.size - 2);
		range.slots[0] = (unsigned short)((endpoint_2e0f80 *)((sorted_2dfec0 *)&axis)->first_not_below(
			(entry_2dfec0 *)(base + 1), last, (unsigned short)box.low[1]) - base);
		range.slots[2] = (unsigned short)((endpoint_2e0f80 *)((sorted_2dfec0 *)&axis)->first_not_below(
			(entry_2dfec0 *)(base + 1), last, (unsigned short)box.high[1]) - (base + 1));
	}
	{
		endpoints_2e0f80 &axis = sweep->axes[2];
		endpoint_2e0f80 *base = axis.data;
		entry_2dfec0 *last = (entry_2dfec0 *)(base + axis.size - 2);
		range.slots[1] = (unsigned short)((endpoint_2e0f80 *)((sorted_2dfec0 *)&axis)->first_not_below(
			(entry_2dfec0 *)(base + 1), last, (unsigned short)box.low[2]) - base);
		range.slots[3] = (unsigned short)((endpoint_2e0f80 *)((sorted_2dfec0 *)&axis)->first_not_below(
			(entry_2dfec0 *)(base + 1), last, (unsigned short)box.high[2]) - (base + 1));
	}

	// of those, the ones that overlap on y and z too
	gather_overlaps_2e3370(sweep, first_owner, bits, &range, pairs, found, mode);
	g_memory_2d88a0->release_stack(bits, (words * sizeof(long) + 15) & ~15);
}

// sorts the entries after the first by key
__forceinline void sort_after_first_2e3370(endpoint_2e0f80 *entries, int count)
{
	order_2e0250 order = {};
	if (count > 1)
	{
		int const last = count - 1;
		function_2e0250((entry_2e0250 *)(entries + 1), 0, last, order);
	}
}

// the endpoints of every box the bounds overlap (not the grid cells'), one
// list per axis, each sorted between a sentinel at each end
// score 51 (was 414): the box is quantized and the overlaps gathered by
// file-local inline helpers (copies of the header's search, specialized here),
// and each kept endpoint is appended from the box and the slot number.
// Before, 414 (was 438): each kept endpoint is copied by a list member that takes
// the source list and the slot (retail forms the list's address, then reads its
// data, after the slot). Frame size agrees; the array and vector slots differ.
// Earlier: array/cursor/sentinel renames and narrower scopes 518, in-place
// loop 436, value append 544, member copies 706. New: range by value 479,
// range pointer 438, first bound by value 475, later word declaration 438,
// reversed test inputs 491, nested search scope 438, scan-word scope 438,
// output postincrement 438. On reversed inputs: early group step 623,
// read-only walk, nested tests, reordered pointers, const output cursor,
// postincrement and cell reference 491; range by value 479. Shared-query
// losses exclude the range-value, bound-value and group-step forms.
// New: earlier low stores 526; separate corner scopes 526; one intrinsic
// expression 476; indexed clear 534; sentinel loops
// 481/444, both 487; ascending collection 464. Shared forms NOT kept:
// other users regress.
// P453: construction block removed 438; sort end-pointer bound 440;
// box snapshot 452; all axis bases retained 595; all output header
// references 439; separate ending endpoint 435; initial memset 455;
// collected count retained 510. NOT kept: best gain three; array home
// remains displaced. Original body and shared helpers retained.
// @retail 0x2e3370
void class_2e1090::function_2e3370(bounds_2e0020 const *bounds, overlap_endpoints_2e3370 *lists)
{
	longs_2e1ab0 found(elements.size);
	search_boxes_2e3370(this, *bounds, 0, 0, &found, SEARCH_COLLECT_BOXES_2E1090);

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
		lists->axes[0].append_from(axes[0], **kept, 4);
		lists->axes[0].append_from(axes[0], **kept, 5);
		lists->axes[1].append_from(axes[1], **kept, 0);
		lists->axes[1].append_from(axes[1], **kept, 2);
		lists->axes[2].append_from(axes[2], **kept, 1);
		lists->axes[2].append_from(axes[2], **kept, 3);
		kept++;
	}
	endpoint_list_2e3370 *list = lists->axes;
	for (int axis = 3; axis != 0; axis--)
	{
		sort_after_first_2e3370(list->data, list->size - 1);
		list++;
	}
	sentinel.value = 0xfffc;
	sentinel.id = 0;
	lists->axes[0].append(sentinel);
	lists->axes[1].append(sentinel);
	lists->axes[2].append(sentinel);
}

inline void remap_boxes_2e3b00(shorts_2e1090 &boxes, long const *remap)
{
	for (long j = boxes.size - 1; j >= 0; j--)
	{
		boxes.data[j] = (unsigned short)remap[boxes.data[j]];
	}
}
inline void set_index_2e3b00(element_2e1090 const &element, cell_2e1090 *cells, long index)
{
	long owner = element.owner;
	if ((owner & 1) == 0)
	{
		*(long *)owner = index;
	}
	else
	{
		((cell_2e1090 *)((char *)cells + (owner & ~1)))->box = (unsigned short)index;
	}
}
inline void set_index2_2e3b00(long const &owner_ref, cell_2e1090 *cells, long index)
{
	long owner = owner_ref;
	if ((owner & 1) == 0)
	{
		*(long *)owner = index;
	}
	else
	{
		((cell_2e1090 *)((char *)cells + (owner & ~1)))->box = (unsigned short)index;
	}
}
// sorts `count` entries by key
inline void sort_2e02d0(entry_2e0250 *entries, int count)
{
	order_2e0250 order = {};
	if (count > 1)
	{
		int const last = count - 1;
		function_2e02d0(entries, 0, last, order);
	}
}

// score 100 (was 111): the sort is set up by a helper that takes the entries and
// their count (retail loads the list and steps past the sentinel before the
// count test). Owner-loop copy, cell-loop registers and axis step order remain.
// Before, 111 (was 171): first written form 171; frame now agrees. Sort setup
// and loop registers differ. Object argument 141; its scope 141; cell
// reference 130; named count 101 (NOT kept: repeated array pointer reads);
// endpoint cursor 111; range pointer 100 (NOT kept: one use); element
// reference 99; owner pointer 100; shared index 100 (NOT kept: shared local);
// owner mask 40 (NOT kept: extra mask); separate pointers 99 (NOT kept:
// one use); index first 100; int indices 100; while 100; axis reference 100;
// early owner pointer 100 (NOT kept: one use); cell pointer 100; unscoped
// indexed loop 101 (NOT kept: repeated array pointer reads); unscoped cursor 111.
// New: record copies 111; tagged branch first 111; axis pointer/count 110
// (NOT kept: gain 1); box decrement in body 111; captured endpoint count 115;
// direct size test 147; inclusive last ordinal 147; unsigned owner 111.
// P419: Context before count 111; branch-local context and direct empty value
// 110/110; paired memcpy 111; once-loaded indexed endpoints 111; head-tested
// box ordinal 111; distinct loop names 111; initial bulk copy 189.
// NOT kept: best gain one; sort setup and later loop registers remain.
// P428: order base shared across sort/remap 186; empty object without
// initialization 194; whole sort scope 111; original count for box loops
// 216; saved-record base shared across both passes 164; forward cell
// indices 111; cell pointer/countdown 199; endpoint-id reference 111.
// NOT kept: no gain; sort-base setup and later loop carriers still differ.
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
	sort_2e02d0((entry_2e0250 *)(order.data + 1), order.size - 1);
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
