#ifndef PHYSICS_PAIRS_3728C0_H
#define PHYSICS_PAIRS_3728C0_H
#include "placed_shape_312800.h"
#include "holder_30dc80.h"
#include "slot_table_372a40.h"
#include "matrix_compare_2da7f0.h"

// A pair points at handles inside their owners. Their unsigned keys order
// the pairs; their kinds select one of the dispatcher's 8-by-8 handlers.
struct pair_handle_3728c0
{
	unsigned long unknown0;
	unsigned long key;
	unsigned long kind;
};
struct pair_order_372a80
{
	bool operator()(pair_30d870 const &a, pair_30d870 const &b) const
	{
		unsigned long first = ((pair_handle_3728c0 *)a.first)->key;
		unsigned long second = ((pair_handle_3728c0 *)b.first)->key;
		if (first < second) return true;
		if (first != second) return false;
		return ((pair_handle_3728c0 *)a.second)->key < ((pair_handle_3728c0 *)b.second)->key;
	}
};
// The listener prefix used by this call has a hidden-pointer bool result.
// Older game views keep that interface opaque; do not change their layout.
class pair_filter_3728c0
{
public:
	virtual bool_result_2da7f0 accept(void const *first, void const *second) = 0;
};
#endif
