// @flags /O2 /GL-
#include "lane_b_constraint_leaves.h"
#include "contact_constraint_315510.h"

// @retail 0x31bc60
lane_b_constraint_aggregate::lane_b_constraint_aggregate()
{
	function_31bc70(this);
}

// @retail 0x31bd90
void contact_default_31c000::info(lane_a_constraint_info *output) const
{
	lane_b_constraint_sizes *sizes = (lane_b_constraint_sizes *)output;
	sizes->total = 0;
	sizes->term_bytes = 0;
	sizes->results = 0;
	sizes->op_bytes = 0x18;
	long count = properties.size;
	sizes->total = 0x80 + count * 0x30;
	sizes->term_bytes = (count + 2) * 0x30;
	sizes->op_bytes = 0x2c + count * 4;
	sizes->results = count + 2;
	if (count >= 2)
	{
		sizes->term_bytes += 0x20;
		sizes->op_bytes += 4;
		sizes->results++;
	}
}

// @retail 0x31be20
long lane_b_contact_id_array::allocate(unsigned char value)
{
	for (long i = size - 1; i >= 0; --i)
	{
		if (data[i] == 0xff)
		{
			data[i] = value;
			return i;
		}
	}
	if (size == (capacity_and_flags & 0x7fffffff))
	{
		function_2d91d0(this, 1);
	}
	data[size] = value;
	size++;
	return size - 1;
}
