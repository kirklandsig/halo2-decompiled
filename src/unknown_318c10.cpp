// @flags /O2 /GL-
// Contact storage insertion and its material/notification path.
#include "contact_manager_318c10.h"
#include "lane_a_motion_leaves.h"
#include <math.h>

struct s_1d1541 { void *function_30d2b0(void *); };
void __cdecl function_318320(listener_world_318320 *, event_315d20 *);
void __cdecl function_315d20(listener_world_315d20 *, event_315d20 *);

struct contact_added_318c10
{
	unsigned short id;
	class_318e60 *manager;
	lane_a_contact_body const *first;
	lane_a_contact_body const *second;
	void *world;
	lane_b_contact_point *point;
	lane_b_contact_property *properties;
	float speed;
	long rejected;
};

struct contact_body_318c10
{
	unsigned char unknown0[0xc];
	contact_body_318c10 const *parent;
	unsigned char unknown10[0x10];
	lane_a_contact_entity *entity;
};

static __forceinline contact_body_318c10 const *contact_root_318c10(lane_a_contact_body const *start)
{
	contact_body_318c10 const *body = (contact_body_318c10 const *)start;
	while (body->parent) body = body->parent;
	return body;
}

static __forceinline class_3193c0 *contact_motion_318c10(lane_a_contact_entity const *entity)
{
	return *(class_3193c0 *const *)((unsigned char const *)entity + 0x3c);
}

static __forceinline float &contact_property_318c10(lane_b_contact_property *property, long offset)
{
	// Material and separation values occupy the otherwise opaque payload.
	return *(float *)((unsigned char *)property + offset);
}

static __forceinline void copy_contact_point_318c10(lane_b_contact_point *to, lane_b_contact_point const *from)
{
	float separation = from->position.m128_f32[3];
	to->position = from->position;
	to->normal = from->normal;
	float &slot = to->position.m128_f32[3];
	slot = separation;
}

// 0x31be80 is in its own file (src/unknown_31be80.cpp). The event is built once,
// after the point copy, field by field in member order, and it is declared in its
// own block together with the listener calls and the rejection test. With the
// event at function scope the compiler keeps its stores behind the material reads;
// inside the block it moves the id, second and rejected stores up into the first
// square-root statement and the first store into the second, as retail does.
// The root walk is a plain while, and the copied point's separation is stored
// through a reference so that it stays behind the normal's store.
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
	copy_contact_point_318c10(added_point, point);
	{
		contact_added_318c10 event;
		event.id = (unsigned short)id;
		event.manager = this;
		event.first = first;
		event.second = second;
		event.point = added_point;
		event.properties = property;
		event.speed = speed;
		event.rejected = 0;
		function_318320((listener_world_318320 *)world, (event_315d20 *)&event);
		if (entity_a->listeners_60.size) function_315d20(entity_a, (event_315d20 *)&event);
		if (entity_b->listeners_60.size) function_315d20(entity_b, (event_315d20 *)&event);
		if (event.rejected == 1)
		{
			remove((unsigned short)id);
			return 0xffff;
		}
	}
	float time_scale = *(float const *)((unsigned char const *)input + 0x30);
	float bias = contact_property_318c10(property, 12) * time_scale * speed * 1.3f;
	property->bias = bias;
	contact_property_318c10(property, 20) = bias + point->position.m128_f32[3];
	float inverse = 1.0f / (contact_motion_318c10(entity_a)->field_20.m128_f32[3]
		+ contact_motion_318c10(entity_b)->field_20.m128_f32[3] + 1.0e-10f);
	property->impulse = inverse * (contact_property_318c10(property, 12) + 1.0f) * speed * 0.2f;
	return (unsigned short)id;
}
