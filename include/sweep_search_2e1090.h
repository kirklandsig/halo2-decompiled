#ifndef SWEEP_SEARCH_2E1090_H
#define SWEEP_SEARCH_2E1090_H
// Havok (third-party physics library): the three-axis sweep's search for the
// boxes a box overlaps, shared inline by the search that reports pairs
// (src/unknown_2dfec0.cpp) and the one that collects the boxes
// (src/unknown_2e3370.cpp).
#include "sweep_quantize_2e1090.h"
#include "reference_counted_2d88a0.h"

// an array of longs whose storage comes from the allocator's stack area
// (src/unknown_2e18e0.cpp)
struct array_longs_2e1ab0
{
	long *data;
	long size;
	unsigned long capacity_and_flags;

	array_longs_2e1ab0() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~array_longs_2e1ab0()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(long), 0x12);
		}
	}
};

struct longs_2e1ab0 : array_longs_2e1ab0
{
	long *stack_block;
	long stack_capacity;

	longs_2e1ab0(long capacity);
	~longs_2e1ab0();
};

// a block from the top of the allocator's stack area, or from its overflow
// slot when the area is too small
__forceinline void *take_stack_2e1090(long size)
{
	memory_2d88a0 *memory = g_memory_2d88a0;
	long const free = memory->stack_free;
	if (size > free)
	{
		return memory->allocate_stack_overflow(size);
	}
	char *block = memory->stack_cursor;
	memory->stack_cursor = block + size;
	memory->stack_free = free - size;
	return block;
}

// what the search does with each box it finds
enum
{
	SEARCH_REPORT_PAIRS_2E1090,
	SEARCH_COLLECT_BOXES_2E1090
};

// of the boxes whose bits are set, each (not a grid cell's) that overlaps `range`
// on y and z: as a pair with `first_owner` appended to `pairs`, or its address
// appended to `found`
__forceinline void gather_overlaps_2e1090(class_2e1090 *sweep, long *first_owner, long *bits, element_2e1090 const &range,
	pairs_2e1090 *pairs, array_longs_2e1ab0 *found, int mode)
{
	pair_2e1090 pair;
	pair.first = first_owner;
	element_2e1090 *group = sweep->elements.data;
	long *word = bits;
	long *end = bits + (sweep->elements.size >> 5) + 1;
	for (; word < end; group += 32, word++)
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
					unsigned long apart; \
					apart_yz_2e1090(other[k], range, apart); \
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
__forceinline void search_boxes_2e1090(class_2e1090 *sweep, bounds_2e0020 const &bounds, long *first_owner,
	pairs_2e1090 *pairs, array_longs_2e1ab0 *found, int mode)
{
	int const count = sweep->elements.size;
	int const words = (count >> 5) + 4;
	long *bits = (long *)take_stack_2e1090((words * sizeof(long) + 15) & ~15);
	clear_blocks_2e1090(bits, count >> 3);
	quantized_box_2e1090 box;
	quantize_box_2e1090(sweep, bounds, box);

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
				cell_2e1090 *cell = &sweep->cells[slot - 1];
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
	gather_overlaps_2e1090(sweep, first_owner, bits, range, pairs, found, mode);
	g_memory_2d88a0->release_stack(bits, (words * sizeof(long) + 15) & ~15);
}

// sorts entries from low to high (both included) by key (src/unknown_2e0120.cpp)
struct entry_2e0250;
struct order_2e0250
{
};
void __cdecl function_2e0250(entry_2e0250 *entries, int low, int high, order_2e0250 order);

// sorts `count` entries by key
inline void sort_2e0250(entry_2e0250 *entries, int count)
{
	order_2e0250 order = {};
	if (count > 1)
	{
		int const last = count - 1;
		function_2e0250(entries, 0, last, order);
	}
}

#endif
