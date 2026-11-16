// @flags /O2 /GL-
// Havok (third-party physics library): the three-axis sweep's sorted
// endpoint helpers (axis_sweep_2e1090.h): the first entry of a sorted range
// whose 16-bit key is not below a given key (halving while the range is
// longer than 16 entries, then stepping) and a sort by key; then the search
// for every box a bounds overlaps, which calls the first one and follows it
// here, so that its compile sees that the call leaves memory alone.
#include "sweep_search_2e1090.h"

// @retail 0x2dfec0
entry_2dfec0 *sorted_2dfec0::first_not_below(entry_2dfec0 *begin, entry_2dfec0 *end, unsigned short key) const
{
	while (end - begin > 16)
	{
		entry_2dfec0 *middle = begin + ((end - begin) >> 1);
		if (middle->key < key)
		{
			begin = middle;
		}
		else
		{
			end = middle;
		}
	}
	while (begin->key < key)
	{
		begin++;
	}
	return begin;
}

// sorts the entries from low to high (both included) by key
// @retail 0x2dff20
void __cdecl function_2dff20(entry_2dfec0 *entries, int low, int high, sorted_2dfec0 order)
{
	int j = high;
	int i = low;
	entry_2dfec0 pivot = entries[(low + high) >> 1];
	do
	{
		while (entries[i].key < pivot.key)
		{
			i++;
		}
		while (pivot.key < entries[j].key)
		{
			j--;
		}
		if (j >= i)
		{
			if (j != i)
			{
				__declspec(align(16)) entry_2dfec0 swap = entries[j];
				entries[j] = entries[i];
				entries[i] = swap;
			}
			j--;
			i++;
		}
	} while (i <= j);
	if (low < j)
	{
		function_2dff20(entries, low, j, order);
	}
	if (i < high)
	{
		function_2dff20(entries, i, high, order);
	}
}

// every box the bounds overlap, as pairs with no first owner
// @retail 0x2e10b0
void class_2e1090::function_2e10b0(bounds_2e0020 const *bounds, pairs_2e1090 *pairs)
{
	long *first_owner = 0;
	search_boxes_2e1090(this, *bounds, first_owner, pairs, 0, SEARCH_REPORT_PAIRS_2E1090);
}