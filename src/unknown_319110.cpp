// @flags /O2 /Ob1 /GL- /GX
#include "contact_manager_318c10.h"
#include "owned_reference_318b90.h"
#include <stddef.h>

typedef char contact_storage_size[sizeof(contact_default_31c000) == 0x110 ? 1 : -1];
typedef char contact_inline_offset[offsetof(contact_default_31c000, inline_properties) == 0x9c ? 1 : -1];
typedef char manager_storage_offset[offsetof(class_318e60, constraint) == 0x10 ? 1 : -1];
typedef char manager_size[sizeof(class_318e60) == 0x120 ? 1 : -1];

// Empty arrays borrow their inline buffers until they grow.
static __forceinline void initialize_contact_array(array_2d91d0 *array, void *buffer, long capacity)
{
	array->data = buffer;
	array->size = 0;
	array->capacity_and_flags = capacity | 0x80000000;
}
// @retail 0x31c000
contact_default_31c000::contact_default_31c000()
{
	owner = 0;
	body_b = 0;
	body_a = 0;
	priority = 0;
	initialize_contact_array((array_2d91d0 *)&ids, unknown2c, 8);
	points.data = 0;
	points.size = 0;
	points.capacity_and_flags = 0x80000000;
	unsigned long *state = (unsigned long *)&solver_data;
	for (long i = 0; i < 8; ++i) state[i] = 0;
	state[8] = 0x3e88d677;
	state[9] = 0x3f08d677;
	state[10] = 0x3f4d41b3;
	state[11] = 0x3f4d41b3;
	initialize_contact_array((array_2d91d0 *)&properties, inline_properties, 4);
	lane_b_contact_property *p = inline_properties;
	p->impulse = 0; p->bias = 0; ++p;
	p->impulse = 0; p->bias = 0; ++p;
	p->impulse = 0; p->bias = 0; ++p;
	p->impulse = 0; p->bias = 0;
}
// @retail 0x31c0d0 deleting contact_default_31c000
// @retail 0x31c0c0 adjustor contact_default_31c000 8

// @retail 0x319110
class_318e60::class_318e60(c_1d1261 *owner, hkEntity *first, hkEntity *second)
	: referenced_2d88a0(preserve_reference())
{
	world = owner;
	constraint.set_entity_a(first);
	constraint.set_entity_b(second);
	unknownc = 1;
}

// @retail 0x3190a0
class_318e60::~class_318e60()
{
	if (constraint.properties.size) world->function_30ca30(&constraint);
}
// @retail 0x319190 deleting class_318e60

// @retail 0x3191c0
class_318e60 *class_319080::create(lane_a_contact_body const *first,
	lane_a_contact_body const *second, void const *input)
{
	hkEntity *a = (hkEntity *)first->entity;
	hkEntity *b = (hkEntity *)second->entity;
	return new class_318e60((c_1d1261 *)owner, a, b);
}
