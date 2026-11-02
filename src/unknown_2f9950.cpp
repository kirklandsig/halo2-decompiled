// @flags /O2 /GL- /GX
// Havok (third-party physics library): a query of a shape holding a tree of
// parts against the child of a wrapping shape, into a collector of the
// nearest result; and a query with the child of a wrapping shape and the
// other side the other way round.
#include "child_query_2f9950.h"
#include "swapped_query_2ed2a0.h"
#include "lane_b_flip_collectors.h"

void __cdecl function_2ea640(void const *a, void const *b, void const *input, void *collector, void *second_collector);

// @retail 0x2f9950
void __cdecl function_2f9950(body_2ea940 const *a, body_2ea940 const *b, input_2ea940 const *input, long value)
{
	/* the caller's collector, wrapped in the one that turns what the
	   swapped query finds back (retail table 0x412970) */
	lane_b_flip_point_collector collector((lane_b_cd_point_collector *)value);
	body_2ea940 child(a, a->placement, (wrapper_2f9950 const *)a->shape);
	function_2ea940(b, &child, input, &collector);
}

// the query with the first side replaced by the child of the shape that wraps
// it, and the two sides the other way round: its direction turned around, and
// each collector wrapped in one that turns its results back
// @retail 0x2f99d0
void __cdecl function_2f99d0(void const *a, void const *b, void const *input, void *collector, void *second_collector)
{
	query_2ed2a0 const *query = (query_2ed2a0 const *)input;
	query_2ed2a0 swapped;
	swap_query_2ed2a0(&swapped, query);
	swapped_collector_2ed2a0 first(query->direction, collector);
	body_2ea940 const *side = (body_2ea940 const *)a;
	if (second_collector)
	{
		swapped_collector_2ed2a0 second(query->direction, second_collector);
		body_2ea940 child(side, side->placement, (wrapper_2f9950 const *)side->shape);
		function_2ea640(b, &child, &swapped, &first, &second);
	}
	else
	{
		body_2ea940 child(side, side->placement, (wrapper_2f9950 const *)side->shape);
		function_2ea640(b, &child, &swapped, &first, 0);
	}
}
