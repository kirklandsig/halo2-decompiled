// @flags /O2 /GL-
// Contact storage insertion and its material/notification path.
#include "contact_manager_318c10.h"

static __forceinline lane_b_contact_point *append_contact_point_31be80(
	lane_b_contact_constraint_prefix::point_array &points)
{
	long index = points.size;
	long count = index + 1;
	long capacity = points.capacity_and_flags & 0x7fffffff;
	if (capacity < count)
	{
		capacity *= 2;
		function_2d9160(&points, count >= capacity ? count : capacity, sizeof(lane_b_contact_point));
	}
	points.size = count;
	return points.data + index;
}

// room for at least `request` properties, at least twice the old room
static __forceinline void reserve_properties_31be80(contact_properties_315510 *elements, long request)
{
	long const capacity = elements->capacity_and_flags & 0x7fffffff;
	if (capacity < request)
	{
		long const doubled = capacity * 2;
		function_2d9160(elements, request < doubled ? doubled : request, sizeof(lane_b_contact_property));
	}
}

// copies `count` properties, the last first
static __forceinline void copy_properties_down_31be80(lane_b_contact_property *to,
	lane_b_contact_property const *from, long count)
{
	for (long i = count - 1; i >= 0; i--)
	{
		to[i] = from[i];
	}
}

// the property `offset` bytes into the storage
static __forceinline lane_b_contact_property *property_at_31be80(contact_properties_315510 *elements, long offset)
{
	return (lane_b_contact_property *)((char *)elements->data + offset);
}

// makes room for `count` properties at `index`, the later ones moving up;
// gives the first of them (the same insertion as 0x2e96a0's: the outer test
// against the capacity and the reserve's own test are both kept)
static __forceinline lane_b_contact_property *open_gap_31be80(contact_properties_315510 *elements,
	long index, long count)
{
	long const new_size = count + elements->size;
	long const tail = elements->size - index;
	if (new_size > (elements->capacity_and_flags & 0x7fffffff))
	{
		reserve_properties_31be80(elements, new_size);
	}
	long const offset = index * (long)sizeof(lane_b_contact_property);
	copy_properties_down_31be80(property_at_31be80(elements, offset) + count,
		property_at_31be80(elements, offset), tail);
	elements->size = new_size;
	return property_at_31be80(elements, offset);
}

// the property insert is written like 0x2e96a0's (a capacity test around the
// reserve helper, which tests again), which keeps the count in ecx across the
// grow call as in retail; the gap is addressed by its byte offset, which is
// reloaded after the move loop as in retail.
// @retail 0x31be80
long lane_b_contact_constraint_prefix::add(lane_b_contact_point **point,
	lane_b_contact_property **property)
{
	lane_b_constraint_sizes sizes;
	sizes.term_bytes = 0;
	sizes.op_bytes = 0;
	sizes.results = 0;
	sizes.total = property_count * 0x30 + 0xb0;
	if (property_count == 1)
	{
		sizes.op_bytes = 4;
		sizes.term_bytes = 0x20;
		sizes.results = 1;
	}
	sizes.op_bytes += 4;
	sizes.term_bytes += 0x30;
	++sizes.results;
	owner->add(this, &sizes);
	solver_data.flags |= 6;
	long index = points.size;
	lane_b_contact_point *added_point = append_contact_point_31be80(points);
	lane_b_contact_property *added_property = open_gap_31be80(
		(contact_properties_315510 *)&properties, index, 1);
	added_property->impulse = 0;
	added_property->bias = 0;
	added_property->flags = 0;
	if (index > 0 && added_property[-1].flags == 0)
		added_property->flags = 1;
	*point = added_point;
	*property = added_property;
	return ids.allocate((unsigned char)index);
}
