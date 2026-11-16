// @flags /O2 /GL-
#include "lane_b_constraint_leaves.h"
// Havok (third-party physics library): a function that gives back the object
// it is passed (the constructor at 0x31bc60 calls it, and game code).

// @retail 0x31bc70
void *__fastcall function_31bc70(void *arg_0)
{
	return arg_0;
}

class query_31bc80
{
public:
	void function_31bc80(bool *result) const;
};

// @retail 0x31bc80
void query_31bc80::function_31bc80(bool *result) const
{
	*result = true;
}

// sizes that start at zero
struct zeroed_sizes_31bc90 : lane_b_constraint_sizes
{
	zeroed_sizes_31bc90()
	{
		total = 0;
		term_bytes = 0;
		op_bytes = 0;
		results = 0;
	}
};

// score 0 (was 36): matched with sizes that zero themselves in a constructor
// (one zero register, as retail), the point count decremented through a
// reference (retail's in-place dec, as in 0x30ccd0) and the property array
// read once per pass (retail's base + offset order). Before: the early zero stores, copy address and count stores
// differ. Direct stores 44; field count 95; copy pointer 46; size add order 44;
// postfix point count 44; field decrement 74; zero list 36; map byte 44;
// element memcpy 44; loop names 44; decrement in test 44; map while 76;
// shared counter 44; zero before count 36; compound subtract 44; old count 74;
// int counters 44; four zeros 44; memset 37; constant new count 79.
// Two/three aggregate zeros 80/44; chained zeros in member/reverse/op-first/
// total-results order 48/44/47/47; empty aggregate 37; entry zero list 94.
// Pair predicate 36; full branch sizes 98; source ordinal 96, with full
// sizes 100; source cursor 133, with full sizes 122. All restored.
// Repeated id-array pointer/reference, const or mutable, 36; those views
// with value-initialized sizes 86. Initial byte-slot pointer/reference
// with zeros before/after the count 86. Early zero carrier still differs.
// Named pair predicate 36; complete branch sizes 98; source ordinal 96, with predicate 96;
// source cursor 133, with predicate 133; source ordinal/cursor with branch sizes 100/122.
// All restored; early zero/count stores and copy-address allocation remain.
// @retail 0x31bc90
void lane_b_contact_constraint_prefix::remove(unsigned short id)
{
	long removed_index = ids.data[id];
	ids.data[id] = 0xff;
	long new_count = property_count;
	zeroed_sizes_31bc90 sizes;
	if (new_count == 2)
	{
		sizes.op_bytes = 4;
		sizes.term_bytes = 0x20;
		sizes.results = 1;
	}
	--new_count;
	for (long i = removed_index; i < new_count; ++i)
	{
		lane_b_contact_property *base = properties;
		base[i] = base[i + 1];
	}
	properties[removed_index].flags = 0;
	property_count = new_count;
	long &size = points.size;
	--size;
	for (long i = ids.size - 1; i >= 0; --i)
	{
		if (ids.data[i] != 0xff && ids.data[i] > removed_index)
		{
			--ids.data[i];
		}
	}
	sizes.op_bytes += 4;
	sizes.term_bytes += 0x30;
	++sizes.results;
	owner->remove(this, &sizes);
	solver_data.flags |= 5;
}
