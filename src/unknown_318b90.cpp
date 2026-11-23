// @flags /O2 /GL-
// Havok (third-party physics library): an object that keeps the object it
// was made for, with one reference.
#include "owned_reference_318b90.h"
#include "lane_a_contact_manager.h"
#include "contact_manager_318c10.h"

// @retail 0x318b90
class_318b90::class_318b90(void *owner)
	: owner(owner)
{
}

#include "lane_b_constraint_leaves.h"
#include "lane_a_contact_manager.h"

// @retail 0x318e60
void class_318e60::remove(unsigned short id)
{
	listener_world_315d20 *first = contacts().first_body;
	listener_world_315d20 *second = contacts().second_body;
	event_315d20 event;
	lane_a_contact_removed_payload payload;
	payload.id = id;
	payload.manager = this;
	payload.a = first;
	payload.b = second;
	memcpy(event.unknown0, &payload.id, sizeof(payload.id));
	memcpy(event.unknown0 + 4, &payload.manager, 12);
	function_3183b0((listener_world_318320 *)world, &event);
	if (first->listeners_60.size)
		function_315d90(first, &event);
	if (second->listeners_60.size)
		function_315d90(second, &event);
	contacts().remove(id);
	if (contacts().property_count == 0)
	{
		long previous_flag = world->field_12c;
		world->field_12c = 0;
		world->function_30ca30(&constraint);
		world->field_12c = previous_flag;
	}
}

// The two entities are read before the allocation, and the manager is built
// without a reference count store and with its fields assigned after the
// delay (see lane_a_contact_manager.h), as retail does.
// @retail 0x318bb0
lane_a_simple_contact_manager *class_318b90::function_318bb0(
    lane_a_contact_body const *first, lane_a_contact_body const *second, void const *input)
{
	lane_a_contact_entity *const a = first->entity;
	lane_a_contact_entity *const b = second->entity;
	return new lane_a_simple_contact_manager((listener_world_318320 *)owner, a, b);
}
