// @flags /O2 /GL- /GX
// Havok (third-party physics library): a query of a shape holding a tree of
// parts against another shape, the two sides swapped for the tree query,
// into a collector of the nearest result.
#include "pair_agents_2eab70.h"
#include "side_swap_collectors_2eb1d0.h"
#include "swapped_query_2ed2a0.h"
#include "lane_b_flip_collectors.h"

void __cdecl function_2ea640(void const *a, void const *b, void const *input, void *collector, void *second_collector);

struct body_2ea940;
struct input_2ea940;

void __cdecl function_2ea940(body_2ea940 const *a, body_2ea940 const *b, input_2ea940 const *input, void *collector);

// a collector that starts with no result: the largest float as its distance
class nearest_2eb290
{
public:
	nearest_2eb290() : distance(3.40282347e+38f) {}
	virtual ~nearest_2eb290() {}

	float distance;
};

class nearest_value_2eb290 : public nearest_2eb290
{
public:
	nearest_value_2eb290(long value_) : value(value_) {}

	long value;
};

// @retail 0x2eb290
void __cdecl function_2eb290(body_2ea940 const *a, body_2ea940 const *b, input_2ea940 const *input, long value)
{
	/* the caller's collector, wrapped in the one that turns what the
	   swapped query finds back (retail table 0x412970) */
	lane_b_flip_point_collector collector((lane_b_cd_point_collector *)value);
	function_2ea940(b, a, input, &collector);
}

// the query with the two sides the other way round: its direction turned
// around, and each collector wrapped in one that turns its results back
// @retail 0x2eb2f0
void __cdecl function_2eb2f0(void const *a, void const *b, void const *input, void *collector, void *second_collector)
{
	query_2ed2a0 const *query = (query_2ed2a0 const *)input;
	query_2ed2a0 swapped;
	swap_query_2ed2a0(&swapped, query);
	swapped_collector_2ed2a0 first(query->direction, collector);
	if (second_collector)
	{
		swapped_collector_2ed2a0 second(query->direction, second_collector);
		function_2ea640(b, a, &swapped, &first, &second);
	}
	else
	{
		function_2ea640(b, a, &swapped, &first, 0);
	}
}

// the query with the two sides the other way round: its direction turned
// around, and each collector wrapped in one that turns its results back
// @retail 0x2eb3f0
void class_2ea530::function_2eb3f0(void const *a, void const *b, void const *input, void *collector, void *second_collector)
{
	query_2ed2a0 const *query = (query_2ed2a0 const *)input;
	query_2ed2a0 swapped;
	swap_query_2ed2a0(&swapped, query);
	swapped_collector_2ed2a0 first(query->direction, collector);
	if (second_collector)
	{
		swapped_collector_2ed2a0 second(query->direction, second_collector);
		function_2ea530(b, a, &swapped, &first, &second);
	}
	else
	{
		function_2ea530(b, a, &swapped, &first, 0);
	}
}
