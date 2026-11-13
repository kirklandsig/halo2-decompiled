// @flags /O2 /GL- /GX
// Havok (third-party physics library): the three-axis sweep
// (axis_sweep_2e1090.h) adding many boxes at once (their endpoints written
// at the end of each axis, sorted and merged into place) and taking many out
// at once (the element array compacted and each axis renumbered).
#include "sweep_batches_2e50d0.h"
#include "sweep_search_2e1090.h"

// the indices of the endpoints to remove (src/unknown_2e2190.cpp)
struct removed_2e2370
{
	unsigned short *indices;
	long count;
};

// each object's new index, by its old one
struct renumbering_2e2370
{
	unsigned short *new_index;
};

// an axis's endpoints: merging a sorted run at its end into place, and
// taking endpoints out (src/unknown_2e2190.cpp)
struct endpoint_2e2190;

class endpoint_list_2e2190
{
public:
	endpoint_2e2190 *endpoints;
	long size;
	unsigned long capacity_and_flags;

	void function_2e2190(char *objects, long first_count, long second_count, long axis);
	void function_2e2370(char *objects, removed_2e2370 *removed, long axis, renumbering_2e2370 const *renumbering);
};

// an array of 16-bit values whose storage comes from the allocator's stack
// area (src/unknown_2e18e0.cpp)
struct array_shorts_2e1850
{
	unsigned short *data;
	long size;
	unsigned long capacity_and_flags;

	array_shorts_2e1850() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~array_shorts_2e1850()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(short), 0x12);
		}
	}
};

struct shorts_2e1850 : array_shorts_2e1850
{
	unsigned short *stack_block;
	long stack_capacity;

	shorts_2e1850(long capacity);
	~shorts_2e1850();
};

// `count` more elements of `element_size` bytes, the array grown to at least
// twice its room when full: where the new ones start
__forceinline void *grow_by_2e50d0(void *array, long count, long element_size)
{
	array_2d91d0 *header = (array_2d91d0 *)array;
	long const old_size = header->size;
	long const new_size = old_size + count;
	long capacity = header->capacity_and_flags & 0x7fffffff;
	if (capacity < new_size)
	{
		capacity *= 2;
		function_2d9160(array, new_size >= capacity ? new_size : capacity, element_size);
	}
	header->size = new_size;
	return (char *)header->data + old_size * element_size;
}

// the boxes of `bounds`, one for each owner, added together: each
// axis's new endpoints are sorted and merged into place, the grid cells'
// boxes keep spanning the axes, and the pairs the new boxes make are
// reported
// @retail 0x2e50d0
void sweep_batches_2e50d0::function_2e50d0(owners_2e50d0 const *owners, bounds_array_2e1020 const *bounds, pairs_2e1090 *pairs)
{
	int const bounds_count = bounds->size;
	if (bounds_count < 1)
	{
		return;
	}
	int const first_index = elements.size;
	int const count = owners->size;
	grow_by_2e50d0(&elements, count, sizeof(element_2e1090));
	{
		int old_sizes[3];
		old_sizes[0] = axes[0].size;
		old_sizes[1] = axes[1].size;
		old_sizes[2] = axes[2].size;
		endpoint_2e0f80 *room[3];
		room[0] = (endpoint_2e0f80 *)grow_by_2e50d0(&axes[0], count * 2, sizeof(endpoint_2e0f80));
		room[1] = (endpoint_2e0f80 *)grow_by_2e50d0(&axes[1], count * 2, sizeof(endpoint_2e0f80));
		room[2] = (endpoint_2e0f80 *)grow_by_2e50d0(&axes[2], count * 2, sizeof(endpoint_2e0f80));
		for (int i = 0; i < count; i++)
		{
			quantized_box_2e1090 box;
			quantize_box_2e1090(this, bounds->data[i], box);
			int const index = first_index + i;
			element_2e1090 &element = elements.data[index];
			long *owner = owners->data[i];
			element.owner = (long)owner;
			*owner = index;
			room[0][i * 2].value = (unsigned short)box.low[0];
			room[0][i * 2].id = (unsigned short)index;
			room[0][i * 2 + 1].value = (unsigned short)box.high[0];
			room[0][i * 2 + 1].id = (unsigned short)index;
			room[1][i * 2].value = (unsigned short)box.low[1];
			room[1][i * 2].id = (unsigned short)index;
			room[1][i * 2 + 1].value = (unsigned short)box.high[1];
			room[1][i * 2 + 1].id = (unsigned short)index;
			room[2][i * 2].value = (unsigned short)box.low[2];
			room[2][i * 2].id = (unsigned short)index;
			room[2][i * 2 + 1].value = (unsigned short)box.high[2];
			room[2][i * 2 + 1].id = (unsigned short)index;
		}
		element_2e1090 *data = elements.data;
		for (int a = 0; a < 3; a++)
		{
			sort_2e0250((entry_2e0250 *)room[a], count * 2);
		}
		for (int a = 0; a < 3; a++)
		{
			((endpoint_list_2e2190 *)&axes[a])->function_2e2190((char *)data, old_sizes[a], count * 2, a);
		}
		if (cells_used)
		{
			int const added = count * 2;
			for (int k = 0; k < cells_used; k++)
			{
				cell_2e1090 const &cell = cells[k];
				element_2e1090 &cell_box = elements.data[cell.box];
				cell_box.slots[2] += added;
				cell_box.slots[3] += added;
			}
		}
	}
	{
		longs_2e1ab0 lower_ends(count);
		lower_ends.size = count;
		for (int i = 0; i < count; i++)
		{
			((endpoint_2e0f80 *)lower_ends.data)[i].id = (unsigned short)(first_index + i);
			((endpoint_2e0f80 *)lower_ends.data)[i].value = elements.data[first_index + i].slots[4];
		}
		function_2e2f70((endpoint_2e0f80 *)lower_ends.data, count, pairs, 1);
	}
}

// the boxes of the owners taken out together: the pairs they ended are
// reported, the last boxes move into their places, each axis drops their
// endpoints and renumbers the rest, and the grid cells follow
// score 162 (was 169): this/count now use retail's ESI/EDI; scalar
// frame homes, cached tail-owner handling, the cell-address store and the
// cell remap still differ. an explicit kind flag 265; the flag with a cell snapshot 284;
// memcpy 284; an end-pointer loop with and without memcpy 193/193; single-use tail
// owner 141 (discarded); an element pointer / narrowed id 162/162 with a
// cell snapshot (discarded); shared tail 164, cell-address local 60 (not
// exact, discarded); reused tail owner 162; shared tail without snapshot
// and with entry reference 173/173; other local names and separate
// size lifetimes 162; reused owner with entry reference 162; explicit cell
// address 173 (not exact, discarded). Keep the twice-used owner value and
// original direct remap; slot cursor advances in the body.
// Fresh scalar read 172; pointer local 171; while cursor steps 169; direct
// test 67; head cursor 214; short snapshot 56; entry pointer 67;
// postincrement store 56; wider scope, rename and narrower scope 56/56/56;
// indexed read 224; direct store 169; early reference 161. New bodies restored.
// Persistent tail/both element cursors 299/298; retained table base 242;
// three output cursors 283; endpoint base 284; outer cell cursor 284;
// cell-list entry cursor 155 (NOT kept: seven-point gain above 30);
// owner-slot cursor 316. Other new cursor forms regress; body restored.
// @retail 0x2e5590
void sweep_batches_2e50d0::function_2e5590(owners_2e50d0 const *owners, pairs_2e1090 *pairs)
{
	int const count = owners->size;
	int box_count = elements.size;
	{
		longs_2e1ab0 lower_ends(count);
		lower_ends.size = count;
		for (int i = 0; i < count; i++)
		{
			long *owner = owners->data[i];
			((endpoint_2e0f80 *)lower_ends.data)[i].id = *(unsigned short *)owner;
			((endpoint_2e0f80 *)lower_ends.data)[i].value = elements.data[*owner].slots[4];
		}
		function_2e2f70((endpoint_2e0f80 *)lower_ends.data, count, pairs, 0);
	}
	int const removed = count * 2;
	shorts_2e1850 x_slots(removed);
	shorts_2e1850 y_slots(removed);
	shorts_2e1850 z_slots(removed);
	x_slots.size = removed;
	y_slots.size = removed;
	z_slots.size = removed;
	long removed_owner = 0;
	{
		for (int i = 0, j = 0; i < count; i++)
		{
			int const index = *owners->data[i];
			element_2e1090 &box = elements.data[index];
			x_slots.data[j] = box.slots[4];
			x_slots.data[j + 1] = box.slots[5];
			y_slots.data[j] = box.slots[0];
			y_slots.data[j + 1] = box.slots[2];
			z_slots.data[j] = box.slots[1];
			z_slots.data[j + 1] = box.slots[3];
			box.owner = (long)&removed_owner;
			*owners->data[i] = 0;
			j += 2;
		}
	}
	shorts_2e1850 new_index(box_count);
	new_index.size = box_count;
	{
		int last = box_count - 1;
		for (int i = 0; i <= last;)
		{
			long const tail_owner = elements.data[last].owner;
			if (tail_owner == (long)&removed_owner)
			{
				last--;
				continue;
			}
			element_2e1090 &box = elements.data[i];
			if (box.owner == (long)&removed_owner)
			{
				box = elements.data[last];
				new_index.data[last] = (unsigned short)i;
				long const owner = tail_owner;
				if ((owner & 1) == 0)
				{
					*(long *)owner = i;
				}
				else
				{
					((cell_2e1090 *)((char *)cells + (owner & ~1)))->box = (unsigned short)i;
				}
				last--;
			}
			else
			{
				new_index.data[i] = (unsigned short)i;
				i++;
			}
		}
		box_count = last + 1;
	}
	elements.resize(box_count);
	((endpoint_list_2e2190 *)&axes[0])->function_2e2370((char *)elements.data, (removed_2e2370 *)&x_slots, 0, (renumbering_2e2370 const *)&new_index);
	((endpoint_list_2e2190 *)&axes[1])->function_2e2370((char *)elements.data, (removed_2e2370 *)&y_slots, 1, (renumbering_2e2370 const *)&new_index);
	((endpoint_list_2e2190 *)&axes[2])->function_2e2370((char *)elements.data, (removed_2e2370 *)&z_slots, 2, (renumbering_2e2370 const *)&new_index);
	if (cells_used)
	{
		for (int k = 0; k < cells_used; k++)
		{
			cell_2e1090 &cell = cells[k];
			element_2e1090 &cell_box = elements.data[cell.box];
			cell_box.slots[2] -= removed;
			cell_box.slots[3] -= removed;
			for (int i = 0; i < cell.boxes.size; i++)
			{
				cell.boxes.data[i] = new_index.data[cell.boxes.data[i]];
			}
		}
	}
}
