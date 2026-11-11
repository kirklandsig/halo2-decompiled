// @flags /O2 /GL-
#include "lane_a_contact_manager.h"
#include "lane_a_motion_leaves.h"

struct lane_a_contact_point { __m128 position, normal; };
struct contact_added_318a90 : event_315d20
{
	lane_a_contact_point *point;
	void *properties;
	float speed;
	// The retail frame reserves four trailing bytes beyond the speed.
	unsigned long unknown20;
};
void __cdecl function_318320(listener_world_318320 *, event_315d20 *);
void __cdecl function_315d20(listener_world_315d20 *, event_315d20 *);

static __forceinline class_3193c0 *contact_motion_318a90(lane_a_contact_entity const *entity)
{
	class_3193c0 *motion;
	memcpy(&motion, (unsigned char const *)entity + 0x3c, sizeof(motion));
	return motion;
}

// score 2: the normal and first-velocity loads trade places.
// @retail 0x318a90
unsigned short lane_a_simple_contact_manager::add(lane_a_contact_body const *first,
	lane_a_contact_body const *second, void const *, lane_a_contact_point const *point)
{
	lane_a_contact_entity *entity_a = a;
	lane_a_contact_entity *entity_b = b;
	__m128 velocity_a, velocity_b;
	contact_motion_318a90(entity_a)->velocity_at(&point->position, &velocity_a);
	contact_motion_318a90(entity_b)->velocity_at(&point->position, &velocity_b);
	__m128 relative = _mm_sub_ps(velocity_b, velocity_a);
	__m128 products = _mm_mul_ps(relative, point->normal);
	float speed;
	_mm_store_ss(&speed, _mm_add_ss(_mm_shuffle_ps(products, products, 0xaa),
		_mm_add_ss(_mm_shuffle_ps(products, products, 0x55), products)));
	lane_a_contact_point copy;
	copy.position = point->position;
	copy.normal = point->normal;
	contact_added_318a90 event;
	lane_a_contact_removed_payload payload;
	payload.id = 0;
	payload.manager = this;
	memcpy(event.unknown0, &payload.id, sizeof(payload.id));
	memcpy(event.unknown0 + 4, &payload.manager, 4);
	memcpy(event.unknown0 + 8, &first, 4);
	memcpy(event.unknown0 + 12, &second, 4);
	event.point = &copy;
	event.properties = 0;
	event.speed = speed;
	function_318320(world, &event);
	if (entity_a->listeners_60.size)
		function_315d20(entity_a, &event);
	if (entity_b->listeners_60.size)
		function_315d20(entity_b, &event);
	return 0;
}
