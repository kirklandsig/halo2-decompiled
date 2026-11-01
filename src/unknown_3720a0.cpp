// @flags /O2 /Ob1 /GL-
#include "joined_groups_3720a0.h"

inline void reserve_longs_372200(array_2d91d0 *array, long count)
{
	long capacity = array->capacity_and_flags & 0x7fffffff;
	if (capacity < count)
	{
		capacity *= 2;
		if (count >= capacity) capacity = count;
		function_2d9160(array, capacity, sizeof(long));
	}
}
inline void resize_longs_372200(array_2d91d0 *array, long count)
{
	long capacity = array->capacity_and_flags & 0x7fffffff;
	if (capacity < count)
	{
		capacity *= 2;
		function_2d9160(array, count >= capacity ? count : capacity, sizeof(long));
	}
	array->size = count;
}
inline members_372300 &append_group_372390(groups_372300 *groups)
{
	long count = groups->size + 1;
	long capacity = groups->capacity_and_flags & 0x7fffffff;
	if (capacity < count)
	{
		capacity *= 2;
		function_2d9160(groups, count >= capacity ? count : capacity, sizeof(members_372300));
	}
	groups->size = count;
	return groups->data[count - 1];
}

// The constructor records the arrays; it does not initialize the third word.
// @retail 0x3720a0
grouping_3720a0::grouping_3720a0(array_2d91d0 *parents_, array_2d91d0 *scratch_)
	: parents(parents_), scratch(scratch_) {}

inline long parent_value_3720c0(array_2d91d0 const *array, long index)
{
	return ((long const *)array->data)[index];
}
inline long &parent_slot_3720c0(array_2d91d0 *array, long index)
{
	return ((long *)array->data)[index];
}
__forceinline long grouping_3720a0::root(long index)
{
	long root = index;
	while (parent_value_3720c0(parents, root) >= 0)
		root = parent_value_3720c0(parents, root);
	while (parent_value_3720c0(parents, index) >= 0)
	{
		long &entry = parent_slot_3720c0(parents, index);
		index = entry;
		entry = root;
	}
	return index;
}

// Preserve the lower index as the root, rather than choosing by tree size.
// Group export relies on encountering each root before the rest of its group.
// score 75 (first implementation): out-of-line roots 184; value-walk
// expansion 74 (168 bytes); explicit candidates 95/79; cached parent 113.
// Slot/value accessors and array-reference variants 75 (185 bytes); local
// expansion leaves the same result. Root/compression lifetimes and layout differ.
// @retail 0x3720c0
long grouping_3720a0::join(long first, long second)
{
	long left = root(first);
	long right = root(second);
	if (left != right)
	{
		if (left < right)
		{
			parent_slot_3720c0(parents, left) = parent_value_3720c0(parents, left) + parent_value_3720c0(parents, right);
			parent_slot_3720c0(parents, right) = left;
		}
		else
		{
			parent_slot_3720c0(parents, right) = parent_value_3720c0(parents, right) + parent_value_3720c0(parents, left);
			parent_slot_3720c0(parents, left) = right;
		}
	}
	return left < right ? left : right;
}

inline long *begin_longs_372180(array_2d91d0 const &array) { return (long *)array.data; }
inline long *end_longs_372180(array_2d91d0 const &array) { return (long *)array.data + array.size; }

// @retail 0x372180
void grouping_3720a0::compress_paths()
{
	long *current = begin_longs_372180(*parents);
	long *end = end_longs_372180(*parents);
	for (; current != end; ++current)
	{
		if (*current >= 0)
			while (((long *)parents->data)[*current] >= 0)
				*current = ((long *)parents->data)[*current];
	}
	current = begin_longs_372180(*scratch);
	end = end_longs_372180(*scratch);
	for (; current != end; ++current)
		while (((long *)parents->data)[*current] >= 0)
			*current = ((long *)parents->data)[*current];
}

// @retail 0x372200
void grouping_3720a0::function_372200(array_2d91d0 const *pairs, long count)
{
	resize_longs_372200(parents, count);
	for (long i = 0; i < count; ++i)
		((long *)parents->data)[i] = -1;
	scratch->size = 0;
	reserve_longs_372200(scratch, pairs->size);
	resize_longs_372200(scratch, pairs->size);
	for (long i = 0; i < pairs->size; ++i)
	{
		long const *pair = (long const *)pairs->data + i * 2;
		long second = pair[1];
		long *result = (long *)scratch->data + i;
		long first = pair[0];
		if (first < second)
			*result = join(first, second);
		else
			*result = join(second, first);
	}
	compress_paths();
	group_count = 0;
}

// Export consumes the parent/scratch lists while retaining their capacity.
// @retail 0x372390
void grouping_3720a0::function_372390(groups_372300 *groups)
{
	for (long i = 0; i < parents->size; ++i)
	{
		if (((long *)parents->data)[i] < 0)
		{
			members_372300 &group = append_group_372390(groups);
			long count = -((long *)parents->data)[i];
			group.data = (long *)g_memory_2d88a0->allocate(count * sizeof(long), 1);
			group.size = -((long *)parents->data)[i];
			group.data[0] = i;
			group.written = 1;
			((long *)parents->data)[i] = groups->size - 1;
		}
		else
		{
			long root = ((long *)parents->data)[i];
			long group_index = ((long *)parents->data)[root];
			members_372300 &group = groups->data[group_index];
			group.data[group.written] = i;
			++group.written;
		}
	}
	resize_longs_372200(parents, 0);
	resize_longs_372200(scratch, 0);
}
