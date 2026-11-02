// @flags /O2 /GL-
// Contact storage insertion and its material/notification path.
#include "contact_manager_318c10.h"
#include "lane_a_motion_leaves.h"
#include <math.h>

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

static __forceinline lane_b_contact_property *insert_contact_property_31be80(
	array_2d91d0 *array, long index)
{
	long moving = array->size - index;
	long count = array->size + 1;
	long capacity = array->capacity_and_flags & 0x7fffffff;
	if (capacity < count)
	{
		capacity *= 2;
		function_2d9160(array, count >= capacity ? count : capacity, sizeof(lane_b_contact_property));
	}
	lane_b_contact_property const *source = (lane_b_contact_property *)array->data + index;
	lane_b_contact_property *destination = (lane_b_contact_property *)array->data + index + 1;
	for (long i = moving - 1; i >= 0; --i)
		destination[i] = source[i];
	array->size = count;
	return (lane_b_contact_property *)array->data + index;
}

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
	lane_b_contact_property *added_property = insert_contact_property_31be80(
		(array_2d91d0 *)&properties, index);
	added_property->impulse = 0;
	added_property->bias = 0;
	added_property->flags = 0;
	if (index > 0 && added_property[-1].flags == 0)
		added_property->flags = 1;
	*point = added_point;
	*property = added_property;
	return ids.allocate((unsigned char)index);
}

struct s_1d1541 { void *function_30d2b0(void *); };
void __cdecl function_318320(listener_world_318320 *, event_315d20 *);
void __cdecl function_315d20(listener_world_315d20 *, event_315d20 *);

struct contact_added_318c10 : event_315d20
{
	lane_b_contact_point *point;
	lane_b_contact_property *properties;
	float speed;
	long rejected;
};

static __forceinline lane_a_contact_body const *contact_root_318c10(lane_a_contact_body const *body)
{
	lane_a_contact_body const *parent;
	memcpy(&parent, (unsigned char const *)body + 0xc, sizeof(parent));
	while (parent)
	{
		body = parent;
		memcpy(&parent, (unsigned char const *)body + 0xc, sizeof(parent));
	}
	return body;
}

static __forceinline class_3193c0 *contact_motion_318c10(lane_a_contact_entity const *entity)
{
	class_3193c0 *motion;
	memcpy(&motion, (unsigned char const *)entity + 0x3c, sizeof(motion));
	return motion;
}

static __forceinline float &contact_property_318c10(lane_b_contact_property *property, long offset)
{
	// Material and separation values occupy the otherwise opaque payload.
	return *(float *)((unsigned char *)property + offset);
}

// @retail 0x318c10
unsigned short class_318e60::add(lane_a_contact_body const *first,
	lane_a_contact_body const *second, void const *input, lane_b_contact_point const *point)
{
	long count = contacts().property_count;
	if (count > 250) return 0xffff;
	lane_a_contact_entity *entity_a = contact_root_318c10(first)->entity;
	lane_a_contact_entity *entity_b = contact_root_318c10(second)->entity;
	if (count == 0)
		((s_1d1541 *)world)->function_30d2b0(&constraint);
	lane_b_contact_point *added_point;
	lane_b_contact_property *property;
	long id = contacts().add(&added_point, &property);
	__m128 velocity_a, velocity_b;
	contact_motion_318c10(entity_a)->velocity_at(&point->position, &velocity_a);
	contact_motion_318c10(entity_b)->velocity_at(&point->position, &velocity_b);
	__m128 relative = _mm_sub_ps(velocity_b, velocity_a);
	__m128 products = _mm_mul_ps(relative, point->normal);
	float speed;
	_mm_store_ss(&speed, _mm_add_ss(_mm_shuffle_ps(products, products, 0xaa),
		_mm_add_ss(_mm_shuffle_ps(products, products, 0x55), products)));
	float const *material_a = (float const *)((unsigned char const *)contact_root_318c10(first)->entity + 0x48);
	float const *material_b = (float const *)((unsigned char const *)contact_root_318c10(second)->entity + 0x48);
	contact_property_318c10(property, 8) = sqrtf(material_b[1] * material_a[1]);
	contact_property_318c10(property, 12) = sqrtf(material_b[2] * material_a[2]);
	float separation = point->position.m128_f32[3];
	added_point->position = point->position;
	added_point->normal = point->normal;
	added_point->position.m128_f32[3] = separation;
	contact_added_318c10 event;
	unsigned short event_id = (unsigned short)id;
	class_318e60 *manager = this;
	memcpy(event.unknown0, &event_id, 2);
	memcpy(event.unknown0 + 4, &manager, 4);
	memcpy(event.unknown0 + 8, &first, 4);
	memcpy(event.unknown0 + 12, &second, 4);
	event.point = added_point;
	event.properties = property;
	event.speed = speed;
	event.rejected = 0;
	function_318320((listener_world_318320 *)world, &event);
	if (entity_a->listeners_60.size) function_315d20(entity_a, &event);
	if (entity_b->listeners_60.size) function_315d20(entity_b, &event);
	if (event.rejected == 1)
	{
		remove((unsigned short)id);
		return 0xffff;
	}
	float time_scale = *(float const *)((unsigned char const *)input + 0x30);
	float bias = contact_property_318c10(property, 12) * time_scale * speed * 1.3f;
	property->bias = bias;
	contact_property_318c10(property, 20) = bias + point->position.m128_f32[3];
	float inverse = 1.0f / (contact_motion_318c10(entity_b)->field_20.m128_f32[3]
		+ contact_motion_318c10(entity_a)->field_20.m128_f32[3] + 1.0e-10f);
	property->impulse = inverse * (contact_property_318c10(property, 12) + 1.0f) * speed * 0.2f;
	return (unsigned short)id;
}
