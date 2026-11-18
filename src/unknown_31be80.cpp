// @flags /O2 /GL-
// Contact storage insertion and its material/notification path.
#include "contact_manager_318c10.h"

static __forceinline long append_contact_point_31be80(
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
	return index;
}

static __forceinline lane_b_contact_property *insert_contact_property_31be80(
	array_2d91d0 *array, long index, lane_b_contact_point *&added_point,
	lane_b_contact_point *const &point_data, long point_index)
{
	long count = array->size + 1;
	long moving = array->size - index;
	added_point = point_data + point_index;
	long capacity = array->capacity_and_flags & 0x7fffffff;
	if (capacity < count)
	{
		capacity *= 2;
		function_2d9160(array, count >= capacity ? count : capacity, sizeof(lane_b_contact_property));
	}
	lane_b_contact_property *data = (lane_b_contact_property *)array->data;
	lane_b_contact_property const *source = data + index;
	lane_b_contact_property *destination = data + index + 1;
	for (long i = moving - 1; i >= 0; --i)
		destination[i] = source[i];
	array->size = count;
	return (lane_b_contact_property *)array->data + index;
}

// score 13 (was 27): the point append returns its index, and the property insert
// forms the added point (`point_data + point_index`) after the count and the move
// length, so the index and the point array are still live where the count is
// defined: the count then stays in ecx, saved before the grow call and reloaded
// after it, as in retail. Left: retail loads the point array into ebp (its
// properties pointer is formed after the add).
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
	long point_index = append_contact_point_31be80(points);
	lane_b_contact_point *added_point;
	lane_b_contact_property *added_property = insert_contact_property_31be80(
		(array_2d91d0 *)&properties, index, added_point, points.data, point_index);
	added_property->impulse = 0;
	added_property->bias = 0;
	added_property->flags = 0;
	if (index > 0 && added_property[-1].flags == 0)
		added_property->flags = 1;
	*point = added_point;
	*property = added_property;
	return ids.allocate((unsigned char)index);
}

