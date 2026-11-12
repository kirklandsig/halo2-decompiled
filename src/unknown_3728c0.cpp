// @flags /O2 /GL-
#include "physics_pairs_3728c0.h"

inline bool accept_pair_3728c0(pair_filter_3728c0 *filter, pair_30d870 const *pair)
{
	void const *first = pair->first ? (char const *)pair->first - 0x10 : 0;
	void const *second = pair->second ? (char const *)pair->second - 0x10 : 0;
	return filter->accept(first, second).value;
}
inline pair_handler_30dc80 *handler_3728c0(dispatcher_3728c0 *dispatcher, pair_30d870 const *pair)
{
	return (pair_handler_30dc80 *)((slot_table_372a40 *)dispatcher)->slots[
		((pair_handle_3728c0 *)pair->first)->kind][((pair_handle_3728c0 *)pair->second)->kind];
}

// Equal keys cancel before either callback. Removal needs no admission test.
// @retail 0x3728c0
void dispatcher_3728c0::function_3728c0(pair_30d870 const *added, long added_count,
	pair_30d870 const *removed, long removed_count, pair_listener_315a10 *listener)
{
	pair_30d870 const *added_pair = added;
	pair_30d870 const *added_end = added + added_count;
	pair_30d870 const *removed_pair = removed;
	pair_30d870 const *removed_end = removed + removed_count;
	pair_filter_3728c0 *filter = (pair_filter_3728c0 *)listener;
	pair_order_372a80 less;
	while (added_pair != added_end && removed_pair != removed_end)
	{
		if (((pair_handle_3728c0 *)added_pair->first)->key == ((pair_handle_3728c0 *)removed_pair->first)->key &&
			((pair_handle_3728c0 *)removed_pair->second)->key == ((pair_handle_3728c0 *)added_pair->second)->key)
		{
			++added_pair;
			++removed_pair;
		}
		else if (less(*removed_pair, *added_pair))
		{
			handler_3728c0(this, removed_pair)->slot_2(removed_pair);
			++removed_pair;
		}
		else
		{
			if (accept_pair_3728c0(filter, added_pair))
				handler_3728c0(this, added_pair)->slot_1(added_pair);
			++added_pair;
		}
	}
	for (; added_pair < added_end; ++added_pair)
	{
		if (accept_pair_3728c0(filter, added_pair))
			handler_3728c0(this, added_pair)->slot_1(added_pair);
	}
	for (; removed_pair < removed_end; ++removed_pair)
		handler_3728c0(this, removed_pair)->slot_2(removed_pair);
}

// @retail 0x372a80
void __cdecl sort_pair_range_372a80(pair_30d870 *pairs, long first, long last, pair_order_372a80 less)
{
	long right = last;
	long left = first;
	// The retail pivot slot is 16-byte aligned although a pair occupies eight bytes.
	__declspec(align(16)) pair_30d870 pivot = pairs[(first + last) >> 1];
	do
	{
		while (less(pairs[left], pivot)) ++left;
		while (less(pivot, pairs[right])) --right;
		if (right < left) break;
		if (right != left)
		{
			pair_30d870 temporary = pairs[right];
			pairs[right] = pairs[left];
			pairs[left] = temporary;
		}
		--right;
		++left;
	} while (left <= right);
	if (first < right)
		sort_pair_range_372a80(pairs, first, right, less);
	if (left < last)
		sort_pair_range_372a80(pairs, left, last, less);
}

inline void sort_pairs_372b50(pair_30d870 *pairs, long count, pair_order_372a80 less)
{
	if (count > 1)
		sort_pair_range_372a80(pairs, 0, count - 1, less);
}

// @retail 0x372b50
void __cdecl function_372b50(pair_30d870 *pairs, long count)
{
	if (count)
	{
		for (long i = 0; i < count; ++i)
		{
			if (((pair_handle_3728c0 *)pairs[i].first)->key > ((pair_handle_3728c0 *)pairs[i].second)->key)
			{
				long temporary = pairs[i].first;
				pairs[i].first = pairs[i].second;
				pairs[i].second = temporary;
			}
		}
		sort_pairs_372b50(pairs, count, pair_order_372a80());
	}
}
