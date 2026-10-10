// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_1CEC30.CPP: the havok components (0x1cec30..0x1cffxx): a data
   array of 0x200 components of 0xa0 bytes, one per object with a Havok
   rigid body, and the properties the game keeps on the rigid bodies */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_1cec30.h"
#include <string.h>
#include <math.h>
#include <new>
#include <float.h>
#include "havok_reference.h"
#include "unknown_122870.h"

void __cdecl function_2d91d0(void *array, long element_size);

// @retail 0x1d0150
void function_1d0150(s_havok_component *component, s_havok_contact_entities *contact, short a, short b)
{
	short const *a_reference = &a;
	short const *b_reference = &b;
	s_havok_component_element0c entry;
	entry.unknown00 = *a_reference;
	entry.unknown02 = *b_reference;
	entry.impact_index = NONE;
	entry.contact = contact;
	s_havok_array0c *array = &component->unknown7c;
	if (array->size == (array->capacity_and_flags & 0x7fffffff))
		function_2d91d0(array, sizeof(entry));
	s_havok_component_element0c *destination = &array->data[array->size++];
	*destination = entry;
}

struct s_component_object_transform_view
{
	long definition_index;
};

struct s_component_transform_header
{
	short identifier;
	byte flags;
	byte type;
	byte unknown04[4];
	s_component_object_transform_view *object;
};

struct s_component_transform_definition
{
	byte unknown00[0x38];
	long model_index;
};

struct s_component_transform_model
{
	byte unknown00[4];
	long render_model_index;
};

struct s_component_transform_node
{
	byte unknown00[0x28];
	transform4x3f transform;
	byte unknown5c[4];
};

struct s_component_transform_render_model
{
	byte unknown00[0x4c];
	s_component_transform_node *nodes;
};

bool __stdcall function_e0d70(long object_index, real *offset);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
transform4x3f *function_ba160(long object_index, transform4x3f *matrix);
void havok_component_rigid_body_matrix_get(long rigid_body_index, s_havok_component *component, transform4x3f *matrix);
void function_1d43d0(s_havok_component *component, transform4x3f const *matrix, transform4x3f *result);

// @retail 0x1d4360
void function_1d4360(s_havok_component *component, transform4x3f *volatile result)
{
	transform4x3f matrix;
	short index = havok_component_main_rigid_body_index_get(component);
	if (index != NONE)
	{
		havok_component_rigid_body_matrix_get(index, component, &matrix);
	}
	else
	{
		function_ba160(component->object_index, &matrix);
	}
	function_1d43d0(component, &matrix, result);
}

// @retail 0x1d43d0
void function_1d43d0(s_havok_component *component, transform4x3f const *matrix, transform4x3f *result)
{
	transform4x3f *const *result_reference = &result;
	result = *result_reference;
	if (TEST_FIELD_BIT(component->flag9))
	{
		*result = *matrix;
		long object_index = component->object_index;
		if ((1 << ((s_component_transform_header *)g_4e0300->data)[object_index & 0xffff].type) & 1)
		{
			real offset;
			if (function_e0d70(object_index, &offset))
				result->position.z += offset;
		}
	}
	else if (havok_component_main_rigid_body_index_get(component) != NONE)
	{
		*result = *matrix;
		if (component->unknown19 != NONE)
		{
			s_component_object_transform_view *object = ((s_component_transform_header *)g_4e0300->data)[component->object_index & 0xffff].object;
			s_component_transform_definition *definition = (s_component_transform_definition *)g_4e3b44[object->definition_index & 0xffff].bytes;
			s_component_transform_model *model = (s_component_transform_model *)g_4e3b44[definition->model_index & 0xffff].bytes;
			s_component_transform_render_model *render_model = (s_component_transform_render_model *)g_4e3b44[model->render_model_index & 0xffff].bytes;
			function_142a60(result, &render_model->nodes[component->unknown19].transform, result);
		}
	}
	else
	{
		*result = *matrix;
	}
}

struct s_component_constraint_view
{
	short index;
	byte unknown02[2];
	long key;
	long impact_index;
	long value0c;
	short material_a;
	short material_b;
	long component_b;
	long object_index;
	byte unknown1c[0x38 - 0x1c];
	real impulse;
	real value3c;
	real value40;
	char value44;
	char unknown45;
	char unknown46;
	byte unknown47;
};

extern long *g_51e9cc;

// @retail 0x1cf400
void function_1cf400(s_component_constraint_view *entry, short index, long value0c, long key,
	hkEntity *entity_a, hkEntity *entity_b, long object_index, char value44, real value3c, real value40,
	short material_a, short material_b)
{
	entry->index = index;
	entry->key = key;
	entry->impact_index = NONE;
	entry->value0c = value0c;
	entry->material_a = material_a;
	entry->material_b = material_b;
	entry->component_b = havok_entity_property_get(entity_b, HAVOK_PROPERTY_COMPONENT_INDEX);
	long owner_index;
	if (key != NONE)
	{
		long mapped_index = NONE;
		long key_index = key & 0xffff;
		long type = (dword)key >> 29;
		if (type == 3 || type == 4)
			mapped_index = g_51e9cc[key_index];
		owner_index = mapped_index;
	}
	else
		owner_index = object_index;
	entry->object_index = owner_index;
	entry->impulse = 0.0f;
	entry->value3c = value3c;
	entry->value40 = value40;
	entry->value44 = value44;
	entry->unknown45 = (char)havok_entity_property_get(entity_a, HAVOK_PROPERTY_2002);
	entry->unknown46 = (char)havok_entity_property_get(entity_b, HAVOK_PROPERTY_2002);
}

struct s_component_property_view
{
	byte unknown00[4];
	dword flags;
	byte unknown08[0x1a - 8];
	byte value1a;
	byte value1b;
	bool function_1d3550(long key, bool *positive, real *value) const;
};

// @retail 0x1d3550
bool s_component_property_view::function_1d3550(long key, bool *positive, real *value) const
{
	bool found = true;
	real result;
	switch (key)
	{
	case 0xa000693:
		result = (flags & 0x2000) ? 1.0f : 0.0f;
		*value = result;
		break;
	case 0xd000691:
		result = value1b * (1.0f / 255.0f);
		*value = result;
		break;
	case 0xd000692:
		result = value1a * (1.0f / 255.0f);
		*value = result;
		break;
	default:
		found = false;
		break;
	}
	if (found)
	{
		*positive = *value > 0.0f;
	}
	return found;
}

// @retail 0x1d3880
long function_1d3880(s_havok_component const *component, long *constraints, long *contacts, long *other, long *bodies)
{
	*constraints = component->unknown88.size * sizeof(s_havok_component_element48);
	*contacts = (component->unknown7c.capacity_and_flags & 0x7fffffff) * sizeof(s_havok_component_element0c);
	s_havok_array08 *local_0 = component->unknown94;
	long capacity = (long)local_0;
	if (local_0)
		capacity = local_0->capacity_and_flags & 0x7fffffff;
	*other = capacity * sizeof(s_havok_component_element08);
	*bodies = (component->rigid_bodies.capacity_and_flags & 0x7fffffff) * sizeof(s_havok_component_rigid_body);
	for (long i = 0; i < component->rigid_bodies.size; i++)
	{
		long count = component->rigid_bodies.data[i].node_count;
		if (count > 4)
			*bodies += ((count + 3) >> 2) * 4;
	}
	long constraints_size = *constraints;
	long bodies_size = *bodies;
	long contacts_size = *contacts;
	long other_size = *other;
	return constraints_size + bodies_size + contacts_size + other_size;
}

inline bool havok_entity_property_exists(hkEntity const *entity, dword key)
{
	long i;

	for (i = 0; i < entity->m_property_count; i++)
	{
		if (entity->m_properties[i].m_key == key)
		{
			return true;
		}
	}
	return false;
}


// @retail 0x1cec30
void havok_components_initialize(void)
{
	g_51e9b8 = data_new_inlined("havok components", 0x200, sizeof(s_havok_component), 4, g_468758);
}

// @retail 0x1cf280
long havok_entity_component_index_get(hkEntity const *entity)
{
	return havok_entity_property_get(entity, HAVOK_PROPERTY_COMPONENT_INDEX);
}

// @retail 0x1cf2b0
void havok_entity_component_index_set(hkEntity *entity, hkPropertyValue component_index)
{
	if (havok_entity_property_exists(entity, HAVOK_PROPERTY_COMPONENT_INDEX))
	{
		entity->removeProperty(HAVOK_PROPERTY_COMPONENT_INDEX);
	}
	entity->addProperty(HAVOK_PROPERTY_COMPONENT_INDEX, component_index);
}

// @retail 0x1cf300
long havok_entity_property_2002_get(hkEntity const *entity)
{
	return havok_entity_property_get(entity, HAVOK_PROPERTY_2002);
}

// @retail 0x1cf330
void havok_entity_property_2002_set(hkEntity *entity, hkPropertyValue value)
{
	if (havok_entity_property_exists(entity, HAVOK_PROPERTY_2002))
	{
		entity->removeProperty(HAVOK_PROPERTY_2002);
	}
	entity->addProperty(HAVOK_PROPERTY_2002, value);
}

// @retail 0x1cf380
long __cdecl havok_entity_property_2003_get(hkEntity const *entity)
{
	return havok_entity_property_get(entity, HAVOK_PROPERTY_2003);
}

// @retail 0x1cf3b0
void havok_entity_property_2003_set(hkEntity *entity, hkPropertyValue value)
{
	if (havok_entity_property_exists(entity, HAVOK_PROPERTY_2003))
	{
		entity->removeProperty(HAVOK_PROPERTY_2003);
	}
	entity->addProperty(HAVOK_PROPERTY_2003, value);
}

// @retail 0x1cf950
void s_havok_component::initialize(long object_index)
{
	s_havok_component *component = this;
	real seconds;
	long ticks;

	component->unknown04 = 0;
	component->object_index = object_index;
	component->unknown0c = NONE;
	seconds = g_510c54->field_2_3 * 0.35f;
	__asm
	{
		fld seconds
		fistp ticks
	}
	component->unknown18 = NONE;
	component->unknown19 = NONE;
	component->unknown20 = NONE;
	component->unknown14 = 0.0f;
	component->unknown1a = false;
	component->unknown1b = false;
	component->unknown1c = false;
	component->unknown10 = -ticks;
	component->rigid_bodies.data = NULL;
	component->rigid_bodies.size = 0;
	component->rigid_bodies.capacity_and_flags = 0x80000000;
	component->unknown7c.data = NULL;
	component->unknown7c.size = 0;
	component->unknown7c.capacity_and_flags = 0x80000000;
	component->unknown88.data = NULL;
	component->unknown88.size = 0;
	component->unknown88.capacity_and_flags = 0x80000000;
	component->unknown94 = NULL;
	component->rigid_body = NULL;
	component->unknown9c = 0;
}

// @retail 0x1cfdf0
bool havok_component_unknown10_recent(s_havok_component const *component)
{
	long game_time = g_510c54->game_time;
	real seconds = g_510c54->field_2_3 * 0.35f;
	long ticks;

	__asm
	{
		fld seconds
		fistp ticks
	}
	return game_time - component->unknown10 < ticks;
}

// @retail 0x1cfe30
void havok_component_unknown10_expire(s_havok_component *component)
{
	long game_time = g_510c54->game_time;
	real seconds = g_510c54->field_2_3 * 0.35f;
	long ticks;

	__asm
	{
		fld seconds
		fistp ticks
	}
	component->unknown10 = game_time - ticks + 1;
}

PRIVATE __forceinline void function_1cf0b1(long arg_0)
{
    s_havok_object *local_0 = havok_object_get(arg_0);
    if (!TEST_FIELD_BIT(local_0->havok_flag))
    {
        local_0->havok_flag = 1;
        (*g_51e9a0)++;
    }
}

// @retail 0x1cf0b0
long havok_component_new(long object_index)
{
	s_record_pool *components = g_51e9b8;
	long component_index = record_pool_allocate(components);
	s_havok_component *component = &((s_havok_component *)components->data)[component_index & 0xffff];
	if (component)
	{
		component->initialize(object_index);
	}
	function_1cf0b1(object_index);
	return component_index;
}
/* the object, its header and definition as 0x1cf8b0 reads them */
struct s_havok_vehicle_definition
{
	byte unknown000[0x1ec];
	dword unknown1ec_0 : 19;
	dword unknown1ec_19 : 1;
	dword unknown1ec_20 : 12;
};

struct s_havok_vehicle
{
	long definition_index;
	byte unknown004[0x134 - 0x4];
	dword unknown134_0 : 1;
	dword unknown134_1 : 1;
	dword unknown134_2 : 30;
	byte unknown138[0x248 - 0x138];
	long unknown248;
};

struct s_havok_component_object_header
{
	short identifier;
	byte flags;
	byte type;
	byte unknown04[4];
	s_havok_vehicle *object;
};

struct s_havok_friction
{
	byte unknown00[8];
	real friction;
	long unknown0c;
};

struct s_havok_material
{
	byte unknown00[0x3c];
	real friction;
	long unknown40;
};

// @retail 0x1cf8b0
void havok_component_friction_get(long component_index, s_havok_friction *result, s_havok_material const *material)
{
	s_havok_component *component = &((s_havok_component *)g_51e9b8->data)[component_index & 0xffff];
	s_havok_component_object_header *header = &((s_havok_component_object_header *)g_4e0300->data)[component->object_index & 0xffff];
	real friction;

	if (((1 << header->type) & 2) && TEST_FIELD_BIT(((s_havok_vehicle_definition *)g_4e3b44[header->object->definition_index & 0xffff].bytes)->unknown1ec_19) &&
		(header->object->unknown248 != NONE || TEST_FIELD_BIT(header->object->unknown134_1)))
	{
		friction = 0.0f;
	}
	else
	{
		friction = material->friction;
	}
	result->friction = friction;
	result->unknown0c = material->unknown40;
}

void function_1d1260(s_havok_component *component);
void __stdcall function_1d01c0(s_havok_component *component);

// @retail 0x1cf9f0
s_havok_component::~s_havok_component()
{
	if (unknown04 & 0x20)
	{
		function_1d1260(this);
	}
	function_1d01c0(this);
	if (unknown94)
	{
		delete unknown94;
		unknown94 = NULL;
	}
}

// @retail 0x1cec80
void havok_components_dispose(void)
{
	s_record_pool_iterator iterator;

	iterator.data = g_51e9b8;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while (data_iterator_next_inlined(&iterator))
	{
		havok_component_get(iterator.datum_index)->~s_havok_component();
	}
	g_51e9b8->valid = false;
}

// @retail 0x1cf220
void havok_component_delete(long component_index)
{
	s_havok_component *component = havok_component_get(component_index);
	long object_index = component->object_index;
	s_havok_object *object;

	component->~s_havok_component();
	record_pool_release(g_51e9b8, component_index);
	object = havok_object_get(object_index);
	if (TEST_FIELD_BIT(object->havok_flag))
	{
		object->havok_flag = 0;
		(*g_51e9a0)--;
	}
}

// @retail 0x1cfac0
void havok_component_transform_set(s_havok_component *component, transform4x3f const *matrix)
{
	hkRigidBody *rigid_body = component->rigid_body;

	if (rigid_body)
	{
		hkTransform transform;
		hkVector4 translation;

		transform.m_rotation.m_col0.set(matrix->forward.i, matrix->forward.j, matrix->forward.k);
		transform.m_rotation.m_col1.set(matrix->left.i, matrix->left.j, matrix->left.k);
		transform.m_rotation.m_col2.set(matrix->up.i, matrix->up.j, matrix->up.k);
		translation.set(matrix->position.x, matrix->position.y, matrix->position.z);
		transform.m_translation = translation;
		rigid_body->setTransform(transform);
	}
}
/* the object as function_1cf120 reads it: its type, and what it passes to
   function_1c4b00 */
struct s_havok_component_owner
{
	byte unknown000[0x88];
	byte unknown088[0x94 - 0x88];
	byte unknown094[0xaa - 0x94];
	char type;
};

struct s_havok_component_owner_header
{
	short identifier;
	byte flag0 : 1;
	byte flags1 : 7;
	byte type;
	byte unknown04[4];
	s_havok_component_owner *object;
};

void function_1d56a0(s_havok_component *component);
void function_1d56f0(s_havok_component *component);
signed char __stdcall function_1d5940(s_havok_component *component, long a, long b, long c);
void function_1d6b80(s_havok_component *component);
void function_1d6ca0(s_havok_component *component);
void function_1c4b00(long object_index, void *a, void *b, long c);

// @retail 0x1cf120
void function_1cf120(long component_index)
{
	s_havok_component *component = havok_component_get(component_index);
	s_havok_component_owner_header *header = &((s_havok_component_owner_header *)g_4e0300->data)[component->object_index & 0xffff];
	s_havok_component_owner *object = header->object;
	bool flag = TEST_FIELD_BIT(header->flag0);

	switch (object->type)
	{
	case 0:
		function_1d6b80(component);
		break;
	case 1:
		function_1d56f0(component);
		break;
	case 7:
		component->unknown04 |= 0x80;
		component->unknown1c = function_1d5940(component, 0, 1, 0);
		break;
	case 11:
		function_1d56a0(component);
		break;
	case 12:
		function_1d6ca0(component);
		break;
	default:
		__assume(0);
	}
	function_1c4b00(component->object_index, object->unknown088, object->unknown094, 0);
	component->unknown04 |= 1;
	if (flag)
	{
		component->unknown04 |= 0x20000;
	}
	else
	{
		component->unknown04 &= ~0x20000;
	}
}

/* the rigid bodies of a component (0x1d0870..0x1d1ca0) */

static inline void make_vector3f(vector3f *vector, real i, real j, real k)
{
	vector->i = i;
	vector->j = j;
	vector->k = k;
}

static inline void vector3d_from_havok(vector3f *vector, hkVector4 const *havok)
{
	make_vector3f(vector, (*havok)(0), (*havok)(1), (*havok)(2));
}

static inline void havok_from_vector3d(hkVector4 *havok, vector3f const *vector)
{
	havok->set(vector->i, vector->j, vector->k);
}

static inline void havok_rigid_body_activate(hkRigidBody *rigid_body)
{
	if (!rigid_body->isActive().m_bool && rigid_body->m_simulation_island)
	{
		rigid_body->activate();
	}
}

// @retail 0x1d08e0
void havok_component_rigid_body_matrix_get(long rigid_body_index, s_havok_component *component, transform4x3f *matrix)
{
	hkTransform transform;

	transform.set(havok_component_rigid_body_get(rigid_body_index, component)->m_motion->m_transform);

	if (TEST_FIELD_BIT(component->transformed))
	{
		transform.setMulEq(component->transform);
	}
	matrix->scale = 1.0f;
	vector3d_from_havok(&matrix->forward, &transform.m_rotation.m_col0);
	vector3d_from_havok(&matrix->left, &transform.m_rotation.m_col1);
	vector3d_from_havok(&matrix->up, &transform.m_rotation.m_col2);
	vector3d_from_havok((vector3f *)&matrix->position, &transform.m_translation);
}

// @retail 0x1d0870
void havok_component_rigid_body_position_get(long rigid_body_index, s_havok_component *component, point3f *position)
{
	if (TEST_FIELD_BIT(component->transformed))
	{
		transform4x3f matrix;

		havok_component_rigid_body_matrix_get(rigid_body_index, component, &matrix);
		*position = matrix.position;
	}
	else
	{
		vector3d_from_havok((vector3f *)position, &havok_component_rigid_body_get(rigid_body_index, component)->m_motion->m_transform.m_translation);
	}
}

/* retail inlines the velocity getters into this file's callers */
static inline void rigid_body_linear_velocity_get(long rigid_body_index, s_havok_component *component, vector3f *velocity)
{
	hkRigidBody *rigid_body = havok_component_rigid_body_get(rigid_body_index, component);

	if (!rigid_body->m_fixed)
	{
		vector3d_from_havok(velocity, &rigid_body->m_motion->m_linear_velocity);
	}
	else
	{
		*velocity = *g_4687a4;
	}
}

static inline void rigid_body_angular_velocity_get(long rigid_body_index, s_havok_component *component, vector3f *velocity)
{
	hkRigidBody *rigid_body = havok_component_rigid_body_get(rigid_body_index, component);

	if (!rigid_body->m_fixed)
	{
		vector3d_from_havok(velocity, &rigid_body->m_motion->m_angular_velocity);
	}
	else
	{
		*velocity = *g_4687a4;
	}
}

// @retail 0x1d09d0
void havok_component_rigid_body_linear_velocity_get(long rigid_body_index, s_havok_component *component, vector3f *velocity)
{
	rigid_body_linear_velocity_get(rigid_body_index, component, velocity);
}

// @retail 0x1d0ad0
void havok_component_rigid_body_angular_velocity_get(long rigid_body_index, s_havok_component *component, vector3f *velocity)
{
	rigid_body_angular_velocity_get(rigid_body_index, component, velocity);
}

// @retail 0x1d0a20
void havok_component_rigid_body_point_velocity_get(long rigid_body_index, s_havok_component *component, point3f const *point, vector3f *velocity)
{
	if (!havok_component_rigid_body_get(rigid_body_index, component)->m_fixed)
	{
		hkVector4 havok_point;
		hkVector4 havok_velocity;

		havok_from_vector3d(&havok_point, (vector3f const *)point);
		havok_component_rigid_body_get(rigid_body_index, component)->m_motion->getPointVelocity(havok_point, havok_velocity);
		vector3d_from_havok(velocity, &havok_velocity);
	}
	else
	{
		*velocity = *g_4687a4;
	}
}

// @retail 0x1d0b20
void havok_component_rigid_body_inertia_get(long rigid_body_index, s_havok_component *component, matrix3x3 *inertia)
{
	hkRotation havok_inertia;

	havok_component_rigid_body_get(rigid_body_index, component)->m_motion->getInertiaWorld(havok_inertia);
	vector3d_from_havok(&inertia->forward, &havok_inertia.m_col0);
	vector3d_from_havok(&inertia->left, &havok_inertia.m_col1);
	vector3d_from_havok(&inertia->up, &havok_inertia.m_col2);
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))

// @retail 0x1d0bb0
real havok_component_rigid_body_mass_get(long rigid_body_index, s_havok_component *component)
{
	return MAX(1.0f, havok_component_rigid_body_get(rigid_body_index, component)->m_motion->getMass());
}

// @retail 0x1d0c00
bool havok_component_rigid_body_keyframed(long rigid_body_index, s_havok_component *component)
{
	return havok_component_rigid_body_get(rigid_body_index, component)->m_motion->getType() == 6;
}

// @retail 0x1d0dd0
void havok_component_rigid_body_linear_velocity_set(long rigid_body_index, s_havok_component *component, vector3f const *velocity)
{
	hkRigidBody *rigid_body = havok_component_rigid_body_get(rigid_body_index, component);

	if (!rigid_body->m_fixed)
	{
		hkVector4 havok_velocity;

		havok_from_vector3d(&havok_velocity, velocity);
		havok_rigid_body_activate(rigid_body);
		rigid_body->m_motion->setLinearVelocity(havok_velocity);
	}
}

// @retail 0x1d0e50
void havok_component_rigid_body_angular_velocity_set(long rigid_body_index, s_havok_component *component, vector3f const *velocity)
{
	s_havok_component *const *local_0 = &component;
	component = *local_0;
	if (!TEST_FIELD_BIT(component->flag1))
	{
		hkRigidBody *const *local_1 = &component->rigid_bodies.data[rigid_body_index].rigid_body;
		if (!*(volatile byte const *)&(*local_1)->m_fixed)
		{
			hkVector4 havok_velocity;
			hkRigidBody *rigid_body;

			havok_from_vector3d(&havok_velocity, velocity);
			rigid_body = *local_1;
			havok_rigid_body_activate(rigid_body);
			rigid_body->m_motion->setAngularVelocity(havok_velocity);
		}
	}
}

// @retail 0x1d10b0
void havok_component_rigid_body_linear_velocity_add(long rigid_body_index, s_havok_component *component, vector3f const *velocity)
{
	hkRigidBody *rigid_body = havok_component_rigid_body_get(rigid_body_index, component);
	hkVector4 impulse;
	real mass;

	havok_from_vector3d(&impulse, velocity);
	mass = rigid_body->m_motion->getMass();
	__m128 mass4 = _mm_set_ss(mass);
	impulse.m_quad = _mm_mul_ps(_mm_shuffle_ps(mass4, mass4, 0), impulse.m_quad);
	havok_rigid_body_activate(rigid_body);
	rigid_body->m_motion->applyLinearImpulse(impulse);
}

// @retail 0x1d1c10
bool havok_component_any_rigid_body_active(s_havok_component *component)
{
	bool result = false;
	long rigid_body_index;

	for (rigid_body_index = 0; rigid_body_index < component->rigid_bodies.size; rigid_body_index++)
	{
		if (havok_component_rigid_body_get(rigid_body_index, component)->isActive().m_bool)
		{
			result = true;
			break;
		}
	}
	return result;
}

// @retail 0x1d1c60
void havok_component_rigid_bodies_activate(s_havok_component *component)
{
	if (TEST_FIELD_BIT(component->flag5))
	{
		long rigid_body_index;

		for (rigid_body_index = 0; rigid_body_index < component->rigid_bodies.size; rigid_body_index++)
		{
			havok_component_rigid_body_get(rigid_body_index, component)->activate();
		}
	}
}

// @retail 0x1d1ca0
bool havok_component_main_rigid_body_movable(s_havok_component *component)
{
	long rigid_body_index = havok_component_main_rigid_body_index_get(component);
	bool result = false;

	if (rigid_body_index != NONE)
	{
		result = havok_component_rigid_body_get(rigid_body_index, component)->m_motion->getType() != 6 &&
			!havok_component_rigid_body_get(rigid_body_index, component)->m_fixed;
	}
	return result;
}

/* the motion's transform, unless the body is fixed in a world (an inline of
   hkRigidBody's, which retail inlines into 0x1d0cf0 once) */
// @retail 0x1d8e80
void hkRigidBody::motion_transform_set(hkTransform const &transform)
{
	havok_rigid_body_activate(this);
	if (!m_fixed || !m_world)
	{
		m_motion->setTransform(transform);
	}
}

void function_1d1260(s_havok_component *component);
void function_1d1540(s_havok_component *component);

// @retail 0x1d0cf0
void havok_component_rigid_body_transform_set(long rigid_body_index, s_havok_component *component, hkTransform const *transform)
{
	hkTransform local;

	local.set(*transform);
	if (TEST_FIELD_BIT(component->transformed))
	{
		hkTransform inverse;

		inverse.setInverse(component->transform);
		local.setMulEq(inverse);
	}
	if (havok_component_rigid_body_get(rigid_body_index, component)->m_fixed && TEST_FIELD_BIT(component->flag5))
	{
		function_1d1260(component);
		havok_component_rigid_body_get(rigid_body_index, component)->motion_transform_set(local);
		function_1d1540(component);
	}
	else
	{
		havok_component_rigid_body_get(rigid_body_index, component)->motion_transform_set(local);
	}
}

// @retail 0x1d0c20
void havok_component_rigid_body_matrix_set(long rigid_body_index, s_havok_component *component, transform4x3f const *matrix)
{
	hkTransform transform;
	hkVector4 translation;

	transform.m_rotation.m_col0.set(matrix->forward.i, matrix->forward.j, matrix->forward.k);
	transform.m_rotation.m_col1.set(matrix->left.i, matrix->left.j, matrix->left.k);
	transform.m_rotation.m_col2.set(matrix->up.i, matrix->up.j, matrix->up.k);
	translation.set(matrix->position.x, matrix->position.y, matrix->position.z);
	transform.m_translation = translation;
	havok_component_rigid_body_transform_set(rigid_body_index, component, &transform);
}

// @retail 0x1d1010
void havok_component_rigid_body_linear_velocity_change(long rigid_body_index, s_havok_component *component, vector3f const *change)
{
	vector3f velocity;

	rigid_body_linear_velocity_get(rigid_body_index, component, &velocity);
	velocity.i += change->i;
	velocity.j += change->j;
	velocity.k += change->k;
	havok_component_rigid_body_linear_velocity_set(rigid_body_index, component, &velocity);
}

// @retail 0x1d1160
void havok_component_rigid_body_point_impulse_apply(long rigid_body_index, s_havok_component *component, point3f const *point, vector3f const *velocity)
{
	if (TEST_FIELD_BIT(component->flag1))
	{
		havok_component_rigid_body_linear_velocity_change(rigid_body_index, component, velocity);
	}
	else
	{
		hkRigidBody *rigid_body = havok_component_rigid_body_get(rigid_body_index, component);
		hkVector4 havok_point;
		hkVector4 impulse;
		real mass;
		__m128 mass4;

		havok_from_vector3d(&havok_point, (vector3f const *)point);
		havok_from_vector3d(&impulse, velocity);
		mass = rigid_body->m_motion->getMass();
		mass4 = _mm_set_ss(mass);
		impulse.m_quad = _mm_mul_ps(_mm_shuffle_ps(mass4, mass4, 0), impulse.m_quad);
		rigid_body->m_motion->applyPointImpulse(impulse, havok_point);
	}
}

/* the state of an object's nodes the rigid bodies drive (0x1d1a20 reads it,
   0x1d1b60 applies it) */
#define MAXIMUM_HAVOK_NODES 64

struct s_havok_node_states
{
	dword valid[MAXIMUM_HAVOK_NODES / 32];
	transform4x3f matrices[MAXIMUM_HAVOK_NODES];
	vector3f linear_velocities[MAXIMUM_HAVOK_NODES];
	vector3f angular_velocities[MAXIMUM_HAVOK_NODES];
};

// @retail 0x1d1a20
void havok_component_node_states_get(s_havok_component *component, s_havok_node_states *states)
{
	long rigid_body_index;

	memset(states->valid, 0, sizeof(states->valid));
	for (rigid_body_index = 0; rigid_body_index < component->rigid_bodies.size; rigid_body_index++)
	{
		s_havok_component_rigid_body *rigid_body = &component->rigid_bodies.data[rigid_body_index];

		if (rigid_body->node_count > 0)
		{
			long node_index = rigid_body->nodes[0];

			if (node_index >= 0 && node_index < MAXIMUM_HAVOK_NODES)
			{
				states->valid[node_index >> 5] |= 1 << (node_index & 0x1f);
				havok_component_rigid_body_matrix_get(rigid_body_index, component, &states->matrices[node_index]);
				rigid_body_linear_velocity_get(rigid_body_index, component, &states->linear_velocities[node_index]);
				rigid_body_angular_velocity_get(rigid_body_index, component, &states->angular_velocities[node_index]);
			}
		}
	}
}

// @retail 0x1d1b60
void havok_component_node_states_set(s_havok_component *component, s_havok_node_states const *states)
{
	long rigid_body_index;

	for (rigid_body_index = 0; rigid_body_index < component->rigid_bodies.size; rigid_body_index++)
	{
		s_havok_component_rigid_body *rigid_body = &component->rigid_bodies.data[rigid_body_index];

		if (rigid_body->node_count > 0)
		{
			long node_index = rigid_body->nodes[0];

			if (node_index >= 0 && node_index < MAXIMUM_HAVOK_NODES &&
				(states->valid[node_index >> 5] & (1 << (node_index & 0x1f))))
			{
				havok_component_rigid_body_matrix_set(rigid_body_index, component, &states->matrices[node_index]);
				havok_component_rigid_body_linear_velocity_set(rigid_body_index, component, &states->linear_velocities[node_index]);
				havok_component_rigid_body_angular_velocity_set(rigid_body_index, component, &states->angular_velocities[node_index]);
			}
		}
	}
}

// @retail 0x1cefb0
void havok_component_rigid_body_state_update(long rigid_body_index, s_havok_component *component)
{
	s_havok_component_rigid_body *rigid_body = &component->rigid_bodies.data[rigid_body_index];
	point3f position;
	vector3f linear_velocity;
	vector3f angular_velocity;

	havok_component_rigid_body_position_get(rigid_body_index, component, &position);
	hkRigidBody *local_0 = havok_component_rigid_body_get(rigid_body_index, component);
	bool local_1 = local_0->m_fixed;
	if (!local_1)
		vector3d_from_havok(&linear_velocity, &local_0->m_motion->m_linear_velocity);
	else
		linear_velocity = *g_4687a4;
	vector3f const *local_2 = g_4687a4;
	long const volatile *local_3 = (long const volatile *)local_2;
	((long *)&angular_velocity)[0] = local_3[0];
	((long *)&angular_velocity)[1] = local_3[1];
	((long *)&angular_velocity)[2] = local_3[2];
	if (!local_1)
		vector3d_from_havok(&angular_velocity, &local_0->m_motion->m_angular_velocity);
	rigid_body->position = position;
	rigid_body->linear_velocity = linear_velocity;
	rigid_body->angular_velocity = angular_velocity;
}

void __stdcall function_1c3770(long object_index, dword flags);

/* puts the component's object back in the motion state its flags ask for */
// @retail 0x1d2460
void function_1d2460(s_havok_component *component)
{
	dword flags = component->unknown04;

	if (flags & 0x104000)
	{
		if (!(flags & 0x2000))
		{
			if ((flags & 0x80000) && !(flags & 0x100000))
			{
				function_1c3770(component->object_index, 0);
			}
		}
		else if (flags & 0x100000)
		{
			function_1c3770(component->object_index, 0x2000);
		}
	}
}


class c_material_shape;
void function_182b90(c_material_shape *shape, hkEntity const *entity,
	real *friction, real *restitution, short *material);

struct s_component_contact_body
{
	c_material_shape *shape;
	long key;
	long unknown08;
	s_component_contact_body *parent;
	byte unknown10[8];
	long type;
	long unknown1c;
	hkEntity *entity;
};

struct s_component_contact_pair
{
	byte unknown00[8];
	s_component_contact_body *bodies[2];
};

PRIVATE __forceinline real function_1cfd11(real const *arg_0)
{
	real local_0 = *(real const volatile *)&arg_0[0];
	return (real)sqrt(local_0 * arg_0[1]);
}

// @retail 0x1cfd10
void function_1cfd10(s_component_contact_pair const *contact, s_havok_component *component, real scale)
{
	(void)&component;
	(void)&scale;
	real friction[2];
	short material;
	real restitution;
	for (long i = 0; i < 2; ++i)
	{
		s_component_contact_body *body = i == 0 ? contact->bodies[0] : contact->bodies[1];
		s_component_contact_body *root = body;
		while (root->parent)
			root = root->parent;
		hkEntity *entity = root->type == 1 ? root->entity : NULL;
		function_182b90(body->shape, entity, &restitution, &friction[i], &material);
	}
	scale *= function_1cfd11(friction);
	s_game_time_globals *time = g_510c54;
	if (!(scale > component->unknown14))
	{
		real seconds = time->field_2_3 * 0.35f;
		long local_0 = time->game_time;
		long ticks;
		__asm
		{
			fld seconds
			fistp ticks
		}
		if (local_0 - component->unknown10 < ticks)
			goto done;
	}
	component->unknown14 = scale;
done:
	component->unknown10 = time->game_time;
}


struct s_component_collision_rule
{
	dword flags;
	char lower;
	char upper;
	byte unknown06[0x68 - 6];
};

struct s_component_collision_shape
{
	byte unknown00[8];
	short rule_index;
	byte unknown0a[2];
};

struct s_component_collision_model
{
	byte unknown00[0x2c];
	s_component_collision_rule *rules;
	byte unknown30[0x14];
	s_component_collision_shape *shapes;
};

struct s_component_collision_model_link
{
	byte unknown00[0x24];
	long physics_model_index;
};

struct s_component_collision_object
{
	long definition_index;
	byte unknown04[0x108 - 4];
	dword flags0 : 18;
	dword flag18 : 1;
	dword flags19 : 13;
	byte unknown10c[0x13c - 0x10c];
	long attached_index;
};

struct s_component_collision_header
{
	short identifier;
	byte flags;
	byte type;
	long unknown04;
	s_component_collision_object *object;
};

// @retail 0x1d1d00
bool function_1d1d00(long other_component_index, long shape_index, long component_index)
{
	bool result = false;
	if (other_component_index != NONE)
	{
		s_havok_component *component = havok_component_get(component_index);
		s_component_collision_object *object = ((s_component_collision_header *)g_4e0300->data)[component->object_index & 0xffff].object;
		s_component_transform_definition *definition = (s_component_transform_definition *)g_4e3b44[object->definition_index & 0xffff].bytes;
		s_component_collision_model_link *model = (s_component_collision_model_link *)g_4e3b44[definition->model_index & 0xffff].bytes;
		s_component_collision_model *physics = (s_component_collision_model *)g_4e3b44[model->physics_model_index & 0xffff].bytes;
		s_component_collision_rule *rule = &physics->rules[physics->shapes[shape_index].rule_index];
		s_havok_component *other = havok_component_get(other_component_index);
		s_component_collision_header *header = &((s_component_collision_header *)g_4e0300->data)[other->object_index & 0xffff];
		s_component_collision_object *volatile other_object = header->object;
		long mode = *(char *)&other->unknown1c;
		long type = header->type;
		long type_mask = 1 << type;
		bool attached = (type_mask & 3) && header->object->attached_index != NONE;
		bool flagged = (type_mask & 1) && TEST_FIELD_BIT(other_object->flag18);
		if (mode)
		{
			long lower = rule->lower;
			long pinned = mode < lower ? lower : rule->upper < mode ? rule->upper : mode;
			if (pinned != mode)
				goto done;
		}
		dword flags = rule->flags;
		if (attached)
		{
			if (flags & 8)
				goto done;
		}
		else if (flags & 16)
			goto done;
		if ((flagged && (flags & 0x08000000)) || (flags & (1 << (type + 5))))
			goto done;
		result = true;
	}
done:
	return result;
}


void __cdecl function_2d9160(void *array, long capacity, long element_size);

struct s_component_small_bytes
{
	byte *data;
	long size;
	dword capacity_and_flags;
	byte embedded[4];

	s_component_small_bytes()
	{
		data = embedded;
		size = 0;
		capacity_and_flags = 0x80000004;
	}
	void resize(long count)
	{
		long capacity = capacity_and_flags & 0x7fffffff;
		if (capacity < count)
		{
			capacity *= 2;
			function_2d9160(this, count >= capacity ? count : capacity, 1);
		}
		size = count;
	}
};

class c_component_body_state
{
public:
	c_component_body_state(hkEntity *body, byte const *values, long count, bool enabled);
	point3f position;
	vector3f velocity;
	vector3f angular_velocity;
	hkVector4 vector;
	hkEntity *body;
	byte active;
	byte flags;
	s_component_small_bytes values;
};

// @retail 0x1d86d0
c_component_body_state::c_component_body_state(hkEntity *body, byte const *values, long count, bool enabled)
	: body(body), active(0), flags(0)
{
	(void)&body;
	(void)&values;
	(void)&count;
	(void)&enabled;
	velocity = *g_4687a4;
	angular_velocity = *g_4687a4;
	position = *g_468788;
	vector.m_quad = _mm_setzero_ps();
	hkPropertyValue property;
	memset(&property, 0, sizeof(property));
	havok_entity_property_2003_set(body, property);
	this->values.resize(count);
	if (enabled)
		flags |= 1;
	else
		flags &= ~1;
	for (long i = 0; i < count; ++i)
		this->values.data[i] = values[i];
}


// @retail 0x1d0080
void function_1d0080(hkRigidBody *body, s_havok_component *component, byte const *values, long count, bool enabled)
{
	(void)&component;
	(void)&values;
	(void)&count;
	(void)&enabled;
	s_havok_object *object = havok_object_get(component->object_index);
	long body_index = component->rigid_bodies.size;
	s_havok_array60 *array = &component->rigid_bodies;
	if (array->size == (array->capacity_and_flags & 0x7fffffff))
		function_2d91d0(array, sizeof(s_havok_component_rigid_body));
	new (&array->data[array->size++]) c_component_body_state((hkEntity *)body, values, count, enabled);
	havok_component_rigid_body_state_update(body_index, component);
	hkPropertyValue component_property;
	component_property.m_data = object->havok_component_index;
	havok_entity_component_index_set((hkEntity *)body, component_property);
	hkPropertyValue body_property;
	body_property.m_data = body_index;
	havok_entity_property_2002_set((hkEntity *)body, body_property);
	if (body->m_motion->getType() != 7 && body->m_motion->getType() != 6)
		component->unknown04 |= 0x8000;
}

class c_component_rotation
{
public:
	hkVector4 value;
	void set(hkRotation const &rotation);
};

void __cdecl function_2db880(hkVector4 const *position, c_component_rotation const *rotation, real frequency, hkRigidBody *body);

// @retail 0x1d0ee0
void function_1d0ee0(long rigid_body_index, s_havok_component *component, transform4x3f const *matrix)
{
	hkTransform transform;
	hkVector4 translation;
	real frequency = (real)g_510c54->field_2_3;
	transform.m_rotation.m_col0.set(matrix->forward.i, matrix->forward.j, matrix->forward.k);
	transform.m_rotation.m_col1.set(matrix->left.i, matrix->left.j, matrix->left.k);
	transform.m_rotation.m_col2.set(matrix->up.i, matrix->up.j, matrix->up.k);
	translation.set(matrix->position.x, matrix->position.y, matrix->position.z);
	transform.m_translation = translation;
	if (TEST_FIELD_BIT(component->transformed))
	{
		hkTransform inverse;
		inverse.setInverse(component->transform);
		transform.setMulEq(inverse);
	}
	c_component_rotation rotation;
	rotation.set(transform.m_rotation);
	function_2db880(&transform.m_translation, &rotation, frequency,
		havok_component_rigid_body_get(rigid_body_index, component));
}

PRIVATE __forceinline real component_scalar_magnitude(real value)
{
	return value >= 0.0f ? value : 0.0f - value;
}

#define COMPONENT_SCALAR_PIN(value, lower, upper) ((value) < (lower) ? (lower) : (value) > (upper) ? (upper) : (value))

PRIVATE __forceinline real component_acceleration_scale(bool scale_by_mass, real mass, real target, real acceleration, real exponent)
{
	real result;
	if (exponent > 0.001f)
		result = (real)pow(component_scalar_magnitude(target), exponent) * acceleration;
	else
		result = acceleration;
	if (scale_by_mass && mass > 0.001f)
		result /= mass;
	return result;
}

PRIVATE __forceinline real const &component_time_step()
{
	return g_510c54->rate;
}

// @retail 0x1d2280
real function_1d2280(bool allow_reverse, bool scale_by_mass, real mass, real target, real acceleration,
	real velocity, real limit, real position, real exponent, bool force_acceleration, real *ratio)
{
	(void)&scale_by_mass;
	(void)&mass;
	(void)&target;
	(void)&acceleration;
	(void)&velocity;
	(void)&limit;
	(void)&position;
	(void)&exponent;
	(void)&force_acceleration;
	(void)&ratio;
	real scaled_acceleration = component_acceleration_scale(scale_by_mass, mass, target, acceleration, exponent);
	real const &delta_time = component_time_step();
	real step = delta_time * scaled_acceleration;
	real magnitude = component_scalar_magnitude(step);
	target -= position;
	real next_velocity;
	if (step > 0.001f)
	{
		if (force_acceleration)
			next_velocity = velocity + step;
		else
		{
			real stop_distance = (velocity / step + 1.0f) * (delta_time * velocity) * 0.5f;
			if (target > stop_distance && target > step)
				next_velocity = velocity + step;
			else if (target < 0.0f)
				next_velocity = velocity;
			else
				next_velocity = (real)sqrt(2.0f * (step * target));
		}
		next_velocity = COMPONENT_SCALAR_PIN(next_velocity, allow_reverse ? -FLT_MAX : velocity, limit);
	}
	else
		next_velocity = COMPONENT_SCALAR_PIN(velocity + step, 0.0f - limit, allow_reverse ? FLT_MAX : velocity);
	real fraction = component_scalar_magnitude(limit) > 0.001f ? component_scalar_magnitude(velocity / limit) : 0.0f;
	*ratio = fraction;
	return COMPONENT_SCALAR_PIN(next_velocity - velocity, 0.0f - magnitude, magnitude);
}

#undef COMPONENT_SCALAR_PIN

struct s_component_contact_link
{
	char shape_index;
	char body_index;
	byte count;
	char kind;
	long object_index;
};

struct s_component_linked_object
{
	byte unknown00[0x3e4];
	long linked_object;
};

// @retail 0x1d2070
void function_1d2070(hkEntity const *entity, s_havok_component *component, long component_index, long kind, long shape_index)
{
	(void)&component;
	(void)&component_index;
	(void)&kind;
	(void)&shape_index;
	long other_component = havok_entity_property_get(entity, HAVOK_PROPERTY_COMPONENT_INDEX);
	long body_index = havok_entity_property_get(entity, HAVOK_PROPERTY_2002);
	if (other_component != NONE && other_component != component_index && component->unknown94)
	{
		s_havok_array08 *array = component->unknown94;
		long other_object_index = havok_component_get(other_component)->object_index;
		for (long i = 0; i < array->size; ++i)
		{
			s_component_contact_link *link = (s_component_contact_link *)&array->data[i];
			if (link->object_index == other_object_index && link->kind == kind &&
				link->body_index == body_index && link->shape_index == shape_index)
			{
				if (--link->count == 0)
				{
					s_component_collision_object *object = ((s_component_collision_header *)g_4e0300->data)[component->object_index & 0xffff].object;
					s_component_transform_definition *definition = (s_component_transform_definition *)g_4e3b44[object->definition_index & 0xffff].bytes;
					s_component_collision_model_link *model = (s_component_collision_model_link *)g_4e3b44[definition->model_index & 0xffff].bytes;
					s_component_collision_model *physics = (s_component_collision_model *)g_4e3b44[model->physics_model_index & 0xffff].bytes;
					s_component_collision_rule *rule = &physics->rules[physics->shapes[shape_index].rule_index];
					array->data[i] = array->data[--array->size];
					if (rule->flags & 0x01000000)
					{
						s_component_collision_header *header = &((s_component_collision_header *)g_4e0300->data)[other_object_index & 0xffff];
						if ((1 << header->type) & 1)
						{
							s_component_linked_object *other_object = (s_component_linked_object *)header->object;
							if (other_object->linked_object == component->object_index)
								other_object->linked_object = NONE;
						}
					}
				}
				break;
			}
		}
		if (array->size == 0)
		{
			delete component->unknown94;
			component->unknown94 = NULL;
		}
	}
}


class c_contact_link_allocator
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual void slot2() = 0;
	virtual void slot3() = 0;
	virtual void *allocate(long size, long category) = 0;
};

struct s_contact_link_array : s_havok_array08
{
 s_contact_link_array()
 {
  data = NULL;
  size = 0;
  capacity_and_flags = 0x80000000;
 }
 static void *operator new(size_t size)
 {
  return ((c_contact_link_allocator *)g_480118)->allocate(size, 0x12);
 }
};

// @retail 0x1d1e40
void function_1d1e40(long component_index, s_havok_component *component, hkEntity const *entity, long kind, long shape_index)
{
	(void)&component;
	(void)&kind;
	(void)&shape_index;
	long other_component = havok_entity_property_get(entity, HAVOK_PROPERTY_COMPONENT_INDEX);
	long body_index = havok_entity_property_get(entity, HAVOK_PROPERTY_2002);
	if (other_component != NONE && other_component != component_index &&
		function_1d1d00(other_component, shape_index, component_index))
	{
		long other_object_index = havok_component_get(other_component)->object_index;
		if (!component->unknown94)
		{
			component->unknown94 = new s_contact_link_array;
		}
		s_havok_array08 *array = component->unknown94;
		s_component_contact_link *link;
		for (long i = 0; i < array->size; ++i)
		{
			link = (s_component_contact_link *)&array->data[i];
			if (link->object_index == other_object_index && link->kind == kind &&
				link->body_index == body_index && link->shape_index == shape_index)
				goto found;
		}
		{
			s_component_collision_object *object = ((s_component_collision_header *)g_4e0300->data)[component->object_index & 0xffff].object;
			s_component_transform_definition *definition = (s_component_transform_definition *)g_4e3b44[object->definition_index & 0xffff].bytes;
			s_component_collision_model_link *model = (s_component_collision_model_link *)g_4e3b44[definition->model_index & 0xffff].bytes;
			s_component_collision_model *physics = (s_component_collision_model *)g_4e3b44[model->physics_model_index & 0xffff].bytes;
			s_component_collision_rule *rule = &physics->rules[physics->shapes[shape_index].rule_index];
			if (array->size == (array->capacity_and_flags & 0x7fffffff))
				function_2d91d0(array, 8);
			link = (s_component_contact_link *)&array->data[array->size++];
			link->kind = (char)kind;
			link->shape_index = (char)shape_index;
			link->body_index = (char)body_index;
			link->count = 0;
			link->object_index = other_object_index;
			if (rule->flags & 0x01000000)
			{
				s_component_collision_header *header = &((s_component_collision_header *)g_4e0300->data)[other_object_index & 0xffff];
				if ((1 << header->type) & 1)
				{
					s_component_linked_object *other_object = (s_component_linked_object *)header->object;
					if (other_object->linked_object == NONE)
						other_object->linked_object = component->object_index;
				}
			}
		}
found:
		++link->count;
	}
}


PRIVATE inline void component_property_replace(hkEntity *entity, dword key, hkPropertyValue value)
{
 if (havok_entity_property_exists(entity, key))
  entity->removeProperty(key);
 entity->addProperty(key, value);
}

PRIVATE inline void component_body_release(c_havok_reference_counted *body)
{
 --body->reference_count;
 if (body->reference_count == 0)
 {
  long value = (long)body;
#define COMPONENT_MEMORY_LIMIT ((long)0x80061000 + (cache_file_globals.loaded ? cache_file_globals.header.unknown1c : 0))
  if ((value < (long)0x80061000 ? (long)0x80061000 : value > COMPONENT_MEMORY_LIMIT ? COMPONENT_MEMORY_LIMIT : value) == value)
   body->reference_count = 1;
  else
   delete body;
#undef COMPONENT_MEMORY_LIMIT
 }
}

// @retail 0x1d01c0
void __stdcall function_1d01c0(s_havok_component *component)
{
 (void)&component;
 for (long i = 0; i < component->unknown7c.size; ++i)
 {
  s_havok_component_element0c *entry = &component->unknown7c.data[i];
  havok_reference_remove((c_havok_reference_counted *)entry->contact);
  entry->contact = NULL;
 }
 for (long i = 0; i < component->rigid_bodies.size; ++i)
 {
  s_havok_component_rigid_body *entry = &component->rigid_bodies.data[i];
  hkRigidBody *body = entry->rigid_body;
  component_property_replace((hkEntity *)body, HAVOK_PROPERTY_COMPONENT_INDEX, NONE);
  component_property_replace((hkEntity *)body, HAVOK_PROPERTY_2002, NONE);
  component_body_release((c_havok_reference_counted *)body);
  entry->rigid_body = NULL;
  long capacity = *(long *)entry->unknown50;
  if (!(capacity & 0x80000000))
   g_480118->allocate((long)entry->nodes, capacity & 0x7fffffff, 0x12);
 }
 component->unknown04 = 0;
 component->unknown1c = false;
 component->unknown18 = (char)0xff;
 component->unknown19 = (char)0xff;
 component->rigid_bodies.size = 0;
 component->unknown7c.size = 0;
 if (component->rigid_body)
 {
  havok_reference_remove((c_havok_reference_counted *)component->rigid_body);
  component->rigid_body = NULL;
 }
 if (component->unknown9c)
 {
  havok_reference_remove((c_havok_reference_counted *)component->unknown9c);
  component->unknown9c = 0;
 }
}


struct rigid_transform_scaled
{
 quaternionf rotation;
 point3f position;
 real scale;
};

struct s_component_node_object
{
 long definition_index;
 byte unknown04[0x116 - 4];
 short matrices_offset;
};

struct s_component_node_link
{
 byte unknown00[6];
 short parent;
 short sibling;
 short child;
};

struct s_component_node_physics
{
 byte unknown00[0xcc];
 s_component_node_link *nodes;
};

struct s_component_node_default
{
 byte unknown00[0xc];
 point3f position;
 quaternionf rotation;
 byte unknown28[0x60 - 0x28];
};

struct s_component_node_render_model
{
 byte unknown00[0x4c];
 s_component_node_default *nodes;
};

void function_141590(transform4x3f const *in, transform4x3f *out);
matrix3x3 *function_141e10(matrix3x3 *out, quaternionf const *rotation);
void orientation_from_matrix4x3(transform4x3f const *matrix, rigid_transform_scaled *out);

PRIVATE inline transform4x3f *component_node_matrices(long object_index)
{
 s_component_node_object *object = (s_component_node_object *)havok_object_get(object_index);
 return (transform4x3f *)((byte *)object + object->matrices_offset);
}

// @retail 0x1d3d20
void function_1d3d20(s_havok_component *component, long node_index, dword *updated, dword const *restore,
 rigid_transform_scaled *orientations, long unknown)
{
 (void)&node_index;
 long current_node = node_index;
 (void)&updated;
 (void)&restore;
 (void)&orientations;
 (void)&unknown;
 s_component_node_object *object = (s_component_node_object *)havok_object_get(component->object_index);
 s_component_transform_definition *definition = (s_component_transform_definition *)g_4e3b44[object->definition_index & 0xffff].bytes;
 s_component_transform_model *model = (s_component_transform_model *)g_4e3b44[definition->model_index & 0xffff].bytes;
 long physics_index = ((s_component_collision_model_link *)model)->physics_model_index;
 s_component_node_physics *physics = (s_component_node_physics *)g_4e3b44[physics_index & 0xffff].bytes;
 s_component_node_render_model *render_model = (s_component_node_render_model *)g_4e3b44[model->render_model_index & 0xffff].bytes;
 s_component_node_link *node = &physics->nodes[current_node];
 short parent = node->parent;
 if (parent != NONE)
 {
  dword mask = 1 << (current_node & 31);
  if (updated[current_node >> 5] & mask)
  {
   if (orientations)
   {
    transform4x3f *matrices = component_node_matrices(component->object_index);
    transform4x3f inverse;
    transform4x3f relative;
    function_141590(&matrices[parent], &inverse);
    function_142a60(&inverse, &matrices[(short)current_node], &relative);
    orientation_from_matrix4x3(&relative, &orientations[current_node]);
   }
  }
  else if (restore[current_node >> 5] & mask)
  {
   s_component_node_default *initial = &render_model->nodes[current_node];
   transform4x3f *matrices = component_node_matrices(component->object_index);
   transform4x3f *matrix = &matrices[(short)current_node];
   function_141e10(&matrix->rotation, &initial->rotation);
   matrix->position.x = 0.0f;
   matrix->position.y = 0.0f;
   matrix->position.z = 0.0f;
   matrix->scale = 1.0f;
   matrix->position = initial->position;
   function_142a60(&matrices[parent], matrix, matrix);
   updated[current_node >> 5] |= mask;
   if (orientations)
   {
    orientations[current_node].rotation = initial->rotation;
    orientations[current_node].position = initial->position;
    orientations[current_node].scale = 1.0f;
   }
  }
 }
 long child = node->child;
 while (child != NONE)
 {
  s_component_node_link *next = &physics->nodes[child];
  function_1d3d20(component, child, updated, restore, orientations, unknown);
  child = next->sibling;
 }
}

class c_extent_shape;
real __stdcall function_182aa0(c_extent_shape *shape, real *minimum, real *maximum);

// @retail 0x1d1230
real function_1d1230(long rigid_body_index, s_havok_component *component)
{
    real minimum, maximum;
    hkRigidBody *body = component->rigid_bodies[rigid_body_index].rigid_body;
    return function_182aa0(*(c_extent_shape **)((byte *)body + 0xc), &minimum, &maximum);
}

struct s_component_joint_override
{
    short type;
    short index;
    long unknown04;
    real strength;
};

struct s_component_joint_override_group
{
    byte unknown00[8];
    long count;
    s_component_joint_override *entries;
    byte unknown10[8];
};

struct s_component_joint_angles
{
    byte unknown00[0x78];
    real twist_minimum, twist_maximum;
    real cone_minimum, cone_maximum;
    real plane_minimum, plane_maximum;
    real strength;
};

struct s_component_joint_hinge
{
    byte unknown00[0x78];
    real strength;
    real minimum, maximum;
};

struct s_component_joint_model
{
    byte unknown00[0x30];
    long override_count;
    s_component_joint_override_group *overrides;
    byte unknown38[0xbc - 0x38];
    s_component_joint_angles *angles;
    byte unknownc0[0xec - 0xc0];
    s_component_joint_hinge *hinges;
};

class c_component_joint_strength
{
public:
    void set_strength(real strength);
};

// @retail 0x1d35d0
void function_1d35d0(long constraint_index, s_havok_component *component, real scale)
{
    real const *scale_reference = &scale;
    s_havok_component_element0c *constraint = &component->unknown7c.data[constraint_index];
    s_component_object_transform_view *object = (s_component_object_transform_view *)havok_object_get(component->object_index);
    s_component_transform_definition *definition = (s_component_transform_definition *)g_4e3b44[object->definition_index & 0xffff].bytes;
    s_component_collision_model_link *model = (s_component_collision_model_link *)g_4e3b44[definition->model_index & 0xffff].bytes;
    s_component_joint_model *physics = (s_component_joint_model *)g_4e3b44[model->physics_model_index & 0xffff].bytes;
    real strength = 0.0f;
    for (long i = 0; i < physics->override_count; ++i)
    {
        s_component_joint_override_group *group = &physics->overrides[i];
        for (long j = 0; j < group->count; ++j)
        {
            s_component_joint_override *entry = &group->entries[j];
            if (entry->type == constraint->unknown00 && entry->index == constraint->unknown02)
            {
                strength = entry->strength;
                break;
            }
        }
    }
    switch (constraint->unknown00)
    {
    case 2:
        {
            s_component_joint_angles *angles = &physics->angles[constraint->unknown02];
            real *joint = (real *)constraint->contact;
            joint[0x8c / 4] = (angles->twist_minimum > angles->twist_maximum ? angles->twist_maximum : angles->twist_minimum) * *scale_reference * 0.01745329238474369f;
            joint[0x90 / 4] = (angles->twist_minimum > angles->twist_maximum ? angles->twist_minimum : angles->twist_maximum) * *scale_reference * 0.01745329238474369f;
            joint[0x84 / 4] = (real)sin((angles->plane_minimum > angles->plane_maximum ? angles->plane_maximum : angles->plane_minimum) * *scale_reference * 0.01745329238474369f);
            joint[0x88 / 4] = (real)sin((angles->plane_minimum > angles->plane_maximum ? angles->plane_minimum : angles->plane_maximum) * *scale_reference * 0.01745329238474369f);
            joint[0x80 / 4] = (real)cos((angles->cone_minimum > angles->cone_maximum ? angles->cone_minimum : angles->cone_maximum) * *scale_reference * 0.01745329238474369f);
            ((c_component_joint_strength *)joint)->set_strength((strength != 0.0f ? strength : angles->strength) * *scale_reference);
        }
        break;
    case 1:
        {
            s_component_joint_hinge *hinge = &physics->hinges[constraint->unknown02];
            real *joint = (real *)constraint->contact;
            if (strength == 0.0f)
                strength = hinge->strength;
            joint[0xd0 / 4] = strength * *scale_reference;
            real lower = 0.0f - hinge->minimum;
            real upper = 0.0f - hinge->maximum;
            joint[0xc8 / 4] = (lower > upper ? upper : lower) * *scale_reference * 0.01745329238474369f;
            lower = 0.0f - hinge->minimum;
            upper = 0.0f - hinge->maximum;
            joint[0xcc / 4] = (lower > upper ? lower : upper) * *scale_reference * 0.01745329238474369f;
        }
        break;
    }
}

struct c_component_joint_snapshot_0
{
    __m128 data[6];
    c_component_joint_snapshot_0();
};
struct c_component_joint_reader_0
{
    void read(c_component_joint_snapshot_0 *output);
};

struct c_component_joint_snapshot_1
{
    __m128 data[9];
    c_component_joint_snapshot_1();
};
struct c_component_joint_reader_1
{
    void read(c_component_joint_snapshot_1 *output);
};

struct c_component_joint_snapshot_2
{
    __m128 data[9];
    c_component_joint_snapshot_2();
};
struct c_component_joint_reader_2
{
    void read(c_component_joint_snapshot_2 *output);
};

struct c_component_joint_snapshot_4
{
    __m128 data[3];
    c_component_joint_snapshot_4();
};
struct c_component_joint_reader_4
{
    void read(c_component_joint_snapshot_4 *output);
};

// @retail 0x1cff80
void function_1cff80(s_havok_component_element0c const *constraint, point3f *pivot_a, point3f *pivot_b)
{
    switch (constraint->unknown00)
    {
    case 2:
        {
            c_component_joint_reader_2 *joint = (c_component_joint_reader_2 *)constraint->contact;
            c_component_joint_snapshot_2 snapshot;
            joint->read(&snapshot);
            real x = snapshot.data[1].m128_f32[0];
            real y = snapshot.data[1].m128_f32[1];
            real z = snapshot.data[1].m128_f32[2];
            pivot_a->x = x;
            pivot_a->y = y;
            pivot_a->z = z;
            pivot_b->x = x;
            pivot_b->y = y;
            pivot_b->z = z;
        }
        break;
    case 1:
        {
            c_component_joint_reader_1 *joint = (c_component_joint_reader_1 *)constraint->contact;
            c_component_joint_snapshot_1 snapshot;
            joint->read(&snapshot);
            real x = snapshot.data[1].m128_f32[0];
            real y = snapshot.data[1].m128_f32[1];
            real z = snapshot.data[1].m128_f32[2];
            pivot_a->x = x;
            pivot_a->y = y;
            pivot_a->z = z;
            pivot_b->x = x;
            pivot_b->y = y;
            pivot_b->z = z;
        }
        break;
    case 0:
        {
            c_component_joint_reader_0 *joint = (c_component_joint_reader_0 *)constraint->contact;
            c_component_joint_snapshot_0 snapshot;
            joint->read(&snapshot);
            real x = snapshot.data[1].m128_f32[0];
            real y = snapshot.data[1].m128_f32[1];
            real z = snapshot.data[1].m128_f32[2];
            pivot_a->x = x;
            pivot_a->y = y;
            pivot_a->z = z;
            pivot_b->x = x;
            pivot_b->y = y;
            pivot_b->z = z;
        }
        break;
    case 4:
        {
            c_component_joint_reader_4 *joint = (c_component_joint_reader_4 *)constraint->contact;
            c_component_joint_snapshot_4 snapshot;
            joint->read(&snapshot);
            real x = snapshot.data[1].m128_f32[0];
            real y = snapshot.data[1].m128_f32[1];
            real z = snapshot.data[1].m128_f32[2];
            pivot_a->x = x;
            pivot_a->y = y;
            pivot_a->z = z;
            pivot_b->x = x;
            pivot_b->y = y;
            pivot_b->z = z;
        }
        break;
    default:
        *pivot_a = *g_468788;
        *pivot_b = *g_468788;
        break;
    }
}

struct s_component_bounds
{
    __m128 lower;
    __m128 upper;
};

class c_component_bounds_lookup
{
public:
    virtual void slot0() = 0;
    virtual void slot1() = 0;
    virtual void slot2() = 0;
    virtual void slot3() = 0;
    virtual void slot4() = 0;
    virtual void slot5() = 0;
    virtual void slot6() = 0;
    virtual void slot7() = 0;
    virtual void bounds(void *handle, s_component_bounds *output) = 0;
};

struct s_component_bounds_world
{
    byte unknown00[0xc4];
    c_component_bounds_lookup *lookup;
};

struct s_component_bounds_body
{
    byte unknown00[0x80];
    real padding;
    byte unknown84[0xc];
};

struct s_component_bounds_model
{
    byte unknown00[0x3c];
    s_component_bounds_body *bodies;
};

extern hkWorld *g_51e9a4;

// @retail 0x1d44f0
bool __stdcall function_1d44f0(s_havok_component *component, point3f *center, real *radius)
{
    s_havok_component *const *component_reference = &component;
    point3f *const *center_reference = &center;
    real *const *radius_reference = &radius;
    bool result = false;
    if ((*component_reference)->rigid_bodies.size > 0 && TEST_FIELD_BIT((*component_reference)->flag5) && TEST_FIELD_BIT((*component_reference)->flag11))
    {
        s_component_object_transform_view *object = (s_component_object_transform_view *)havok_object_get((*component_reference)->object_index);
        byte *definition_bytes = g_4e3b44[object->definition_index & 0xffff].bytes;
        if (!(definition_bytes[0x1c] & 2))
        {
            s_component_transform_definition *definition = (s_component_transform_definition *)definition_bytes;
            s_component_collision_model_link *model = (s_component_collision_model_link *)g_4e3b44[definition->model_index & 0xffff].bytes;
            s_component_bounds_model *physics = (s_component_bounds_model *)g_4e3b44[model->physics_model_index & 0xffff].bytes;
            s_component_bounds combined;
            volatile long local_0 = 0;
            for (long i = 0; i < (*component_reference)->rigid_bodies.size;)
            {
                s_havok_component_rigid_body *body = &(*component_reference)->rigid_bodies.data[i];
                void *shape = (byte *)body->rigid_body + 0xc;
                real padding = 0.0f;
                for (long j = 0; j < body->node_count; ++j)
                {
                    padding = padding > physics->bodies[body->nodes[j]].padding ? padding : physics->bodies[body->nodes[j]].padding;
                }
                s_component_bounds bounds;
                ((s_component_bounds_world *)g_51e9a4)->lookup->bounds(shape ? (byte *)shape + 0x10 : NULL, &bounds);
                if (padding != 0.0f)
                {
                    bounds.lower.m128_f32[0] -= padding;
                    bounds.upper.m128_f32[0] += padding;
                    bounds.lower.m128_f32[1] -= padding;
                    bounds.upper.m128_f32[1] += padding;
                    bounds.lower.m128_f32[2] -= padding;
                    bounds.upper.m128_f32[2] += padding;
                }
                i = local_0;
                if (i == 0)
                {
                    combined.lower = bounds.lower;
                    combined.upper = bounds.upper;
                }
                else
                {
                    combined.lower = _mm_min_ps(combined.lower, bounds.lower);
                    combined.upper = _mm_max_ps(combined.upper, bounds.upper);
                }
                ++i;
                local_0 = i;
            }
            __m128 half = _mm_set1_ps(0.5f);
            __m128 middle = _mm_mul_ps(half, _mm_add_ps(combined.lower, combined.upper));
            __m128 extent = _mm_mul_ps(half, _mm_sub_ps(combined.upper, combined.lower));
            (*center_reference)->x = middle.m128_f32[0];
            (*center_reference)->y = middle.m128_f32[1];
            (*center_reference)->z = middle.m128_f32[2];
            __m128 square = _mm_mul_ps(extent, extent);
            __m128 sum = _mm_add_ss(_mm_shuffle_ps(square, square, 0xaa), _mm_add_ss(_mm_shuffle_ps(square, square, 0x55), square));
            __m128 length = _mm_sqrt_ss(sum);
            real value;
            _mm_store_ss(&value, length);
            **radius_reference = value + 0.001f;
            result = true;
        }
    }
    return result;
}

struct s_component_surface
{
    byte unknown00[6];
    short material;
};

struct s_component_surface_mesh
{
    byte unknown00[0x2c];
    s_component_surface *surfaces;
};

struct s_slot_entry_list;
extern s_slot_entry_list *g_4e0340;
bool function_181db0(long index, vector3f *result);

PRIVATE __forceinline real component_surface_length(vector3f const *v)
{
    return (real)sqrt(v->i * v->i + v->j * v->j + v->k * v->k);
}

PRIVATE __forceinline void component_surface_property_set(hkEntity *entity, long value)
{
    if (havok_entity_property_exists(entity, HAVOK_PROPERTY_2003))
        entity->removeProperty(HAVOK_PROPERTY_2003);
    entity->addProperty(HAVOK_PROPERTY_2003, hkPropertyValue(value));
}

// @retail 0x1d87b0
void __stdcall function_1d87b0(s_havok_component_rigid_body *body, long body_index, s_havok_component *component)
{
    (void)&body;
    (void)&body_index;
    (void)&component;
    vector3f normal = *g_4687a4;
    long materials[3];
    long material_count = 0;
    long other_contacts = 0;
    long contact_count = component->unknown88.size;
    for (long i = 0; i < contact_count; ++i)
    {
        s_havok_component_element48 *contact = &component->unknown88.data[i];
        if (contact->rigid_body_index_a == body_index && (((byte *)contact)[0x44] & 0x20) && material_count < 3)
        {
            s_component_surface *surface = &((s_component_surface_mesh *)g_4e0340)->surfaces[*(long *)((byte *)contact + 0xc)];
            long j;
            for (j = 0; j < material_count; ++j)
                if (materials[j] == surface->material)
                    break;
            if (j == material_count)
            {
                normal.i = contact->normal.i + normal.i;
                normal.j = contact->normal.j + normal.j;
                normal.k = contact->normal.k + normal.k;
                materials[material_count++] = surface->material;
            }
        }
    }
    if (material_count == 0)
    {
        havok_entity_property_2003_set((hkEntity *)body->rigid_body, hkPropertyValue(0));
        *( __m128 *)((byte *)body + 0x30) = _mm_setzero_ps();
        body->unknown44 = 0;
        return;
    }
    real length = component_surface_length(&normal);
    if (!(fabs(length) < 0.0001f))
    {
        real inverse = 1.0f / length;
        normal.i = inverse * normal.i;
        normal.j = normal.j * inverse;
        normal.k = normal.k * inverse;
    }
    else
        normal = *g_4687b0;
    for (long i = 0; i < contact_count; ++i)
    {
        s_havok_component_element48 *contact = &component->unknown88.data[i];
        if (contact->rigid_body_index_a == body_index && !(((byte *)contact)[0x44] & 0x20) &&
            dot3f(&contact->normal, &normal) > 0.25f)
            ++other_contacts;
    }
    vector3f velocity = *g_4687a4;
    real speed_sum = 0.0f;
    for (long i = 0; i < material_count; ++i)
    {
        vector3f local_c7c2ff;
        if (!function_181db0(materials[i], &local_c7c2ff))
            local_c7c2ff = *g_4687a4;
        velocity.i = local_c7c2ff.i + velocity.i;
        velocity.j = local_c7c2ff.j + velocity.j;
        velocity.k = local_c7c2ff.k + velocity.k;
        speed_sum += component_surface_length(&local_c7c2ff);
    }
    length = component_surface_length(&velocity);
    if (!(fabs(length) < 0.0001f))
    {
        real inverse = 1.0f / length;
        velocity.i = inverse * velocity.i;
        velocity.j = velocity.j * inverse;
        velocity.k = velocity.k * inverse;
    }
    real speed = ((bool)((component->unknown04 >> 18) & 1) ? 1.0f : 2.5f) * speed_sum / material_count;
    hkVector4 *result = (hkVector4 *)((byte *)body + 0x30);
    result->set(speed * velocity.i, velocity.j * speed, velocity.k * speed);
    if (other_contacts == 0)
    {
        body->unknown44 = 1;
        if ((bool)((component->unknown04 >> 18) & 1))
            component_surface_property_set((hkEntity *)body->rigid_body, 0);
        else
            component_surface_property_set((hkEntity *)body->rigid_body, (long)result);
    }
    else
    {
        body->unknown44 = 2;
        component_surface_property_set((hkEntity *)body->rigid_body, 0);
    }
}

struct s_impact;
extern s_record_pool *g_51ebfc;
void impact_release(long impact_index, s_impact *impact);

// @retail 0x1d06c0
void __stdcall function_1d06c0(s_havok_component *component, long contact_index)
{
    // Retail keeps both arguments on the stack and reuses the index slot.
    s_havok_component *const *component_reference = &component;
    long const *index_reference = &contact_index;
    long removed_index = contact_index;
    s_havok_component_element48 *contact = &component->unknown88.data[contact_index];
    bool update_surface = (bool)((((byte *)contact)[0x44] >> 5) & 1);
    contact_index = contact->rigid_body_index_a;
    long impact_index = contact->impact_index;
    if (impact_index != NONE)
    {
        impact_release(impact_index, (s_impact *)(g_51ebfc->data + (impact_index & 0xffff) * 0xa0));
        contact->impact_index = NONE;
    }
    component->unknown88.data[removed_index] = component->unknown88.data[--component->unknown88.size];
    if (update_surface)
        function_1d87b0(&component->rigid_bodies.data[contact_index], contact_index, component);
}


// @retail 0x1d05d0
void function_1d05d0(hkEntity const *entity_a, hkEntity const *entity_b,
    s_havok_component *component, short const *contact_id)
{
    long component_b = havok_entity_property_get(entity_b, HAVOK_PROPERTY_COMPONENT_INDEX);
    long body_a = havok_entity_property_get(entity_a, HAVOK_PROPERTY_2002);
    long body_b = havok_entity_property_get(entity_b, HAVOK_PROPERTY_2002);
    for (long i = 0; i < component->unknown88.size; i++)
    {
        s_havok_component_element48 *contact = &component->unknown88.data[i];
        if (*(short *)contact == *contact_id && contact->component_b == component_b &&
            body_a == contact->rigid_body_index_a && body_b == contact->rigid_body_index_b)
        {
            function_1d06c0(component, i);
            return;
        }
    }
}


// @retail 0x1d0400
long __stdcall function_1d0400(s_havok_component *component, short index,
    void const *volatile contact_data, long key, long value0c, long object_index, char flags,
    hkEntity *entity_a, hkEntity *entity_b, real value3c, real value40,
    short material_a, short material_b)
{
    (void)&component;
    (void)&contact_data;
    long result = NONE;
    bool room = component->unknown88.size < 28;
    if (!room && ((1 << ((s_component_transform_header *)g_4e0300->data)[component->object_index & 0xffff].type) & 1))
    {
        real minimum = 3.402823466e+38f;
        long remove_index = NONE;
        for (long i = 0; i < component->unknown88.size; i++)
        {
            if (component->unknown88.data[i].normal.k < minimum)
            {
                minimum = component->unknown88.data[i].normal.k;
                remove_index = i;
            }
        }
        if (remove_index != NONE)
        {
            function_1d06c0(component, remove_index);
            room = true;
        }
    }
    if (room)
    {
        result = component->unknown88.size;
        s_havok_array48 *array = &component->unknown88;
        long old_size = array->size;
        long new_size = old_size + 1;
        long capacity = array->capacity_and_flags & 0x7fffffff;
        if (capacity < new_size)
        {
            capacity *= 2;
            function_2d9160(array, new_size >= capacity ? new_size : capacity, 0x48);
        }
        s_component_constraint_view *entry = (s_component_constraint_view *)&array->data[old_size];
        array->size = new_size;
        if (entry)
            function_1cf400(entry, index, value0c, key, entity_a, entity_b, object_index,
                flags, value3c, value40, material_a, material_b);
        if (((byte *)array->data)[result * 0x48 + 0x44] & 0x20)
        {
            long body_index = havok_entity_property_get(entity_a, HAVOK_PROPERTY_2002);
            function_1d87b0(&component->rigid_bodies.data[body_index], body_index, component);
        }
    }
    return result;
}


struct s_component_node_body_link
{
    short node;
    byte unknown02[0x90 - 2];
};

struct s_component_node_parent
{
    byte unknown00[6];
    short parent;
    byte unknown08[4];
};

struct s_component_node_body_model
{
    byte unknown00[0x38];
    long body_count;
    s_component_node_body_link *bodies;
    byte unknown40[0xc8 - 0x40];
    long node_count;
    s_component_node_parent *nodes;
};

// @retail 0x1cfe70
long function_1cfe70(s_havok_component *component, long node_index)
{
    s_component_transform_definition *definition = (s_component_transform_definition *)g_4e3b44[
        ((s_component_transform_header *)g_4e0300->data)[component->object_index & 0xffff].object->definition_index & 0xffff].bytes;
    s_component_collision_model_link *model = (s_component_collision_model_link *)g_4e3b44[definition->model_index & 0xffff].bytes;
    s_component_node_body_model *physics = (s_component_node_body_model *)g_4e3b44[model->physics_model_index & 0xffff].bytes;
    long body = NONE;
    if (node_index < physics->node_count)
    {
        short parent = physics->nodes[node_index].parent;
        do
        {
            for (long i = 0; i < physics->body_count; i++)
            {
                if (physics->bodies[i].node == node_index)
                {
                    body = i;
                    break;
                }
            }
            node_index = parent;
        } while (parent != NONE && body == NONE);
    }
    if (body == NONE)
    {
        for (long i = 0; i < physics->body_count; i++)
        {
            if (physics->bodies[i].node == NONE)
                break;
        }
    }
    long result = NONE;
    for (long i = 0; i < component->rigid_bodies.size; i++)
    {
        if (component->rigid_bodies.data[i].node_count > 0)
            result = i;
    }
    return result;
}


struct s_component_child_frame
{
    byte unknown00[0x10];
    hkTransform transform;
};

class c_component_child_frames
{
public:
    virtual void slot00() {}
    virtual void slot01() {}
    virtual void slot02() {}
    virtual void slot03() {}
    virtual void slot04() {}
    virtual void slot05() {}
    virtual void slot06() {}
    virtual void slot07() {}
    virtual void slot08() {}
    virtual void slot09() {}
    virtual void slot0a() {}
    virtual void slot0b() {}
    virtual void slot0c() {}
    virtual s_component_child_frame *get_frame(long index, void *buffer) { return NULL; }
};

#include "unknown_182d90.h"

PRIVATE inline void component_frame_matrix(hkTransform const *frame, transform4x3f *matrix)
{
    matrix->scale = 1.0f;
    vector3d_from_havok(&matrix->forward, &frame->m_rotation.m_col0);
    vector3d_from_havok(&matrix->left, &frame->m_rotation.m_col1);
    vector3d_from_havok(&matrix->up, &frame->m_rotation.m_col2);
    vector3d_from_havok((vector3f *)&matrix->position, &frame->m_translation);
}

// @retail 0x1d83e0
void function_1d83e0(long body_index, s_havok_component *component, long node_a, long node_b,
    transform4x3f *matrix)
{
    s_component_transform_definition *definition = (s_component_transform_definition *)g_4e3b44[
        ((s_component_transform_header *)g_4e0300->data)[component->object_index & 0xffff].object->definition_index & 0xffff].bytes;
    s_component_collision_model_link *model = (s_component_collision_model_link *)g_4e3b44[definition->model_index & 0xffff].bytes;
    s_component_node_body_model *physics = (s_component_node_body_model *)g_4e3b44[model->physics_model_index & 0xffff].bytes;
    s_havok_component_rigid_body *body = &component->rigid_bodies.data[body_index];
    long child_a = NONE;
    long child_b = NONE;
    for (long i = 0; i < body->node_count && (child_a == NONE || child_b == NONE); i++)
    {
        long node = physics->bodies[body->nodes[i]].node;
        if (node == node_a && child_a == NONE)
            child_a = i;
        if (node == node_b && child_b == NONE)
            child_b = i;
    }
    c_component_child_frames *shape = *(c_component_child_frames **)((byte *)body->rigid_body + 0xc);
    hkVector4 buffer_a[16];
    hkVector4 buffer_b[16];
    if (child_a == 0)
    {
        s_component_child_frame *frame = shape->get_frame(child_b, buffer_a);
        component_frame_matrix(&frame->transform, matrix);
    }
    else if (child_b == 0)
    {
        s_component_child_frame *frame = shape->get_frame(child_a, buffer_a);
        component_frame_matrix(&frame->transform, matrix);
        function_141590(matrix, matrix);
    }
    else
    {
        s_component_child_frame *frame_a = shape->get_frame(child_a, buffer_a);
        s_component_child_frame *frame_b = shape->get_frame(child_b, buffer_b);
        hkTransform inverse;
        s_extent_transform relative;
        inverse.setInverse(frame_a->transform);
        relative.compose((s_extent_transform const *)&frame_b->transform, (s_extent_transform const *)&inverse);
        component_frame_matrix((hkTransform const *)&relative, matrix);
    }
}


#include "unknown_1eb550.h"

void function_bba20(long object_index);
void function_f7ed0(long vehicle_index, vector3f *impulse);

// @retail 0x1ced00
void function_1ced00(s_havok_component *component)
{
    if (TEST_FIELD_BIT(component->flag5) && havok_component_any_rigid_body_active(component))
    {
        vector3f change;
        change.i = 0.0f;
        change.j = 0.0f;
        change.k = 0.0f - g_510c54->rate * g_51e9c4->unknown0;
        function_bba20(component->object_index);
        if (!TEST_FIELD_BIT(component->flag2) &&
            ((1 << ((s_component_transform_header *)g_4e0300->data)[component->object_index & 0xffff].type) & 2))
            function_f7ed0(component->object_index, &change);
        for (long i = 0; i < component->rigid_bodies.size; i++)
        {
            if (havok_component_rigid_body_get(i, component)->m_motion->getType() != 6)
            {
                vector3f linear, angular;
                rigid_body_linear_velocity_get(i, component, &linear);
                rigid_body_angular_velocity_get(i, component, &angular);
                real speed_squared = linear.k * linear.k + linear.j * linear.j + linear.i * linear.i;
                if (speed_squared > 10000.0f)
                {
                    double scale = 100.0 / sqrt(speed_squared);
                    linear.i = (real)(linear.i * scale);
                    linear.j = (real)(linear.j * scale);
                    linear.k = (real)(linear.k * scale);
                    havok_component_rigid_body_linear_velocity_set(i, component, &linear);
                }
                real angular_squared = angular.k * angular.k + angular.j * angular.j + angular.i * angular.i;
                if (angular_squared > 2500.0f)
                {
                    double scale = 50.0 / sqrt(angular_squared);
                    angular.i = (real)(angular.i * scale);
                    angular.j = (real)(angular.j * scale);
                    angular.k = (real)(angular.k * scale);
                    havok_component_rigid_body_angular_velocity_set(i, component, &angular);
                }
            }
            havok_component_rigid_body_state_update(i, component);
            if (havok_component_rigid_body_get(i, component)->isActive().m_bool &&
                !TEST_FIELD_BIT(component->flag2) &&
                havok_component_rigid_body_get(i, component)->m_motion->getType() != 6 &&
                !havok_component_rigid_body_get(i, component)->m_fixed)
                havok_component_rigid_body_linear_velocity_change(i, component, &change);
        }
    }
}


struct s_component_frame_pair
{
    long unknown00;
    short node_a;
    short node_b;
    transform4x3f frame_a;
    transform4x3f frame_b;
};

struct s_component_frame_owner
{
    byte unknown00[0x48];
    s_component_node_body_model *physics;
};

struct s_component_frame_map
{
    long unknown00;
    char bodies[256];
    char fallback;
};

byte *__fastcall function_30c170(hkWorld *world);

PRIVATE inline void component_matrix_frame(transform4x3f const *matrix, hkTransform *frame)
{
    havok_from_vector3d(&frame->m_rotation.m_col0, &matrix->forward);
    havok_from_vector3d(&frame->m_rotation.m_col1, &matrix->left);
    havok_from_vector3d(&frame->m_rotation.m_col2, &matrix->up);
    hkVector4 position;
    havok_from_vector3d(&position, (vector3f const *)&matrix->position);
    frame->m_translation = position;
}

// @retail 0x1d7ef0
void function_1d7ef0(hkTransform *frame_a, s_havok_component *component,
    s_component_frame_owner *owner, s_component_frame_map const *mapping, hkTransform *frame_b,
    hkRigidBody **body_a, hkRigidBody **body_b, s_component_frame_pair const *pair)
{
    long node_a = pair->node_a;
    long index_a = node_a == NONE ? mapping->fallback : mapping->bodies[node_a];
    s_havok_component_rigid_body *body = &component->rigid_bodies.data[index_a];
    s_component_node_body_link *first = &owner->physics->bodies[body->nodes[0]];
    long child = 0;
    for (; child < body->node_count; child++)
    {
        if (owner->physics->bodies[body->nodes[child]].node == pair->node_a)
            break;
    }
    if (first->node != pair->node_a)
    {
        transform4x3f relative, matrix;
        function_1d83e0(index_a, component, first->node, node_a, &relative);
        function_142a60(&relative, &pair->frame_a, &matrix);
        component_matrix_frame(&matrix, frame_a);
    }
    else
        component_matrix_frame(&pair->frame_a, frame_a);
    *body_a = component->rigid_bodies.data[index_a].rigid_body;
    long node_b = pair->node_b;
    long index_b;
    if (pair->node_b != NONE &&
        (index_b = node_b == NONE ? mapping->fallback : mapping->bodies[node_b]) != NONE)
    {
        s_havok_component_rigid_body *other = &component->rigid_bodies.data[index_b];
        s_component_node_body_link *other_first = &owner->physics->bodies[other->nodes[0]];
        long other_child = 0;
        for (; other_child < other->node_count; other_child++)
        {
            if (owner->physics->bodies[other->nodes[other_child]].node == pair->node_b)
                break;
        }
        if (other_first->node != pair->node_b)
        {
            transform4x3f relative, matrix;
            function_1d83e0(index_b, component, other_first->node, node_b, &relative);
            function_142a60(&relative, &pair->frame_b, &matrix);
            component_matrix_frame(&matrix, frame_b);
        }
        else
            component_matrix_frame(&pair->frame_b, frame_b);
        *body_b = component->rigid_bodies.data[index_b].rigid_body;
    }
    else
    {
        ((s_extent_transform *)frame_b)->compose(
            (s_extent_transform const *)&(*body_a)->m_motion->m_transform,
            (s_extent_transform const *)frame_a);
        *body_b = (hkRigidBody *)function_30c170(g_51e9a4);
    }
}

struct s_1cf520
{
 word field_0;
 byte field_2[0x1a];
 point3f field_1c;
 vector3f field_28;
 real field_34;
 real field_38;
 byte field_3c[8];
 byte field_44;
 signed char field_45;
};
struct s_1cf521
{
 hkVector4 field_0;
 hkVector4 field_10;
};
class c_1cf520
{
public:
 virtual void function_1cf520() = 0;
 virtual void function_1cf521() = 0;
 virtual void function_1cf522() = 0;
 virtual void function_1cf523() = 0;
 virtual void function_1cf524() = 0;
 virtual s_havok_friction *function_1cf525(word arg_0) = 0;
 virtual s_1cf521 *function_1cf526(word arg_0) = 0;
};
struct s_scale_definition
{
 byte unknown00[0x2c];
 real scale;
};
struct s_scale_owner
{
 byte unknown00[0x3c];
 s_scale_definition *definition;
 real get_inverse_scale() const;
};
struct s_1d8e40 : hkRigidBody
{
 void function_1d8e40(hkVector4 const &arg_0);
 void function_1d8ec0(hkVector4 const &arg_0, hkVector4 const &arg_1);
};

PRIVATE __forceinline __m128 function_1cf527(__m128 arg_0)
{
 __m128 local_0 = _mm_add_ss(_mm_shuffle_ps(arg_0, arg_0, 0x55), arg_0);
 return _mm_add_ss(_mm_shuffle_ps(arg_0, arg_0, 0xaa), local_0);
}

PRIVATE __forceinline bool function_1cf528(s_havok_component const *arg_0)
{
 long local_0 = g_510c54->game_time;
 real local_1 = g_510c54->field_2_3 * 0.35f;
 long local_2;
 __asm { fld local_1 }
 __asm { fistp local_2 }
 return local_0 - arg_0->unknown10 < local_2;
}

PRIVATE __forceinline void function_1cf529(hkVector4 const *arg_0, real *arg_1)
{
 real local_0 = (*arg_0)(0), local_2 = (*arg_0)(2), local_1 = (*arg_0)(1);
 arg_1[0] = local_0;
 arg_1[1] = local_1;
 arg_1[2] = local_2;
}

struct s_1cf530
{
 dword field_0;
 dword field_4 : 18;
 dword field_5 : 1;
 dword field_6 : 13;
};

struct s_1cf531
{
 word field_0 : 11;
 word field_1 : 1;
 word field_2 : 4;
};

// @retail 0x1cf520
void function_1cf520(s_1cf520 *arg_1, c_1cf520 *arg_0, bool arg_2, bool arg_3, long arg_4, long arg_5)
{
 (void)&arg_2; (void)&arg_3; (void)&arg_4; (void)&arg_5;
 s_1cf521 *local_0 = arg_0->function_1cf526(arg_1->field_0);
 s_havok_friction *local_1 = arg_0->function_1cf525(arg_1->field_0);
 s_havok_component *local_2 = havok_component_get(arg_4);
 arg_1->field_34 = local_0->field_0(3);
 function_1cf529(&local_0->field_0, (real *)&arg_1->field_1c);
 function_1cf529(&local_0->field_10, (real *)&arg_1->field_28);
 local_2->rigid_bodies.data[arg_5].rigid_body->m_motion->getType();
 real local_3 = 1.0f > local_2->rigid_bodies.data[arg_5].rigid_body->m_motion->getMass()
  ? 1.0f : local_2->rigid_bodies.data[arg_5].rigid_body->m_motion->getMass();
 real local_4 = *(real *)local_1 >= 0.0f ? *(real *)local_1 : 0.0f - *(real *)local_1;
 if ((*(dword *)&local_4 & 0x7f800000) == 0x7f800000) local_4 = 0.0f;
 arg_1->field_38 = local_4 / local_3;
 if (arg_3)
 {
  arg_1->field_28.i *= -1.0f;
  arg_1->field_28.j *= -1.0f;
  arg_1->field_28.k *= -1.0f;
 }
 else havok_component_friction_get(arg_4, local_1, (s_havok_material *)arg_1);
 if (arg_1->field_38 > 0.4f || (arg_1->field_44 & 8) || function_1cf528(local_2))
 {
  byte *local_5 = *(byte **)(g_4e0300->data + (local_2->object_index & 0xffff) * 12 + 8);
  s_1cf531 *local_24 = (s_1cf531 *)(local_5 + 0xc0);
  local_24->field_1 = true;
 }
 if (!arg_2 && (arg_1->field_44 & 0x20))
 {
  s_havok_component_rigid_body *local_6 = &local_2->rigid_bodies.data[arg_1->field_45];
  if (local_6->unknown44 == 2 && !TEST_FIELD_BIT(((s_1cf530 *)local_2)->field_5))
  {
   hkVector4 local_7;
   const real *local_8 = (const real *)((byte *)local_6 + 0x30);
   local_7.set(local_8[0], local_8[1], local_8[2]);
   hkRigidBody *local_9 = local_6->rigid_body;
   __m128 local_10 = _mm_mul_ps(_mm_set1_ps(0.6f), local_7.m_quad);
   volatile __m128 local_11 = _mm_mul_ps(local_10, local_10);
   real local_12;
   _mm_store_ss(&local_12, function_1cf527(_mm_load_ps((const real *)&local_11)));
   local_7.m_quad = local_10;
   if (local_12 > 0.0000010000001f)
   {
    hkVector4 local_13, local_14;
    local_13.m_quad = local_10;
    local_9->m_motion->getPointVelocity(local_0->field_0, local_14);
    __m128 local_15 = function_1cf527(_mm_load_ps((const real *)&local_11));
    __m128 local_16 = _mm_rsqrt_ss(local_15);
    __m128 local_17 = _mm_sub_ss(_mm_set_ss(3.0f), _mm_mul_ss(_mm_mul_ss(local_15, local_16), local_16));
    local_17 = _mm_mul_ss(_mm_mul_ss(_mm_set_ss(0.5f), local_16), local_17);
    __m128 local_18 = _mm_mul_ps(_mm_shuffle_ps(local_17, local_17, 0), local_7.m_quad);
    __m128 local_19 = _mm_mul_ps(local_14.m_quad, local_18);
    _mm_store_ss(&local_12, function_1cf527(local_19));
    local_13.m_quad = _mm_sub_ps(local_13.m_quad, _mm_mul_ps(_mm_set1_ps(local_12), local_18));
    real local_20 = ((s_scale_owner *)local_9)->get_inverse_scale() * 0.2f;
    local_13.m_quad = _mm_mul_ps(_mm_set1_ps(local_20), local_13.m_quad);
    ((s_1d8e40 *)local_9)->function_1d8ec0(local_13, local_0->field_0);
   }
  }
 }
}


struct s_1d0771
{
 byte field_0[0x20];
 word field_20;
 byte field_22[0xe];
};
struct s_1d0772
{
 s_1d0771 *field_0;
 long field_4;
};
struct s_1d0773
{
 void *field_0;
 byte field_4[0x14];
 long field_18;
 long field_1c;
 void *field_20;
};
struct s_1d0770
{
 c_1cf520 *field_0;
 s_1d0773 *field_4;
 void **field_8;
 s_1d0772 **field_c;
 void *field_10;
};

// @retail 0x1d0770
void function_1d0770(s_1d0770 *arg_0, s_havok_component *arg_1, long arg_2,
 long arg_3, long arg_4, long arg_5)
{
 (void)&arg_1; (void)&arg_2; (void)&arg_3; (void)&arg_4; (void)&arg_5;
 for (long local_0 = 0; local_0 < (*arg_0->field_c)->field_4; ++local_0)
 {
  s_1d0771 *local_1 = &(*arg_0->field_c)->field_0[local_0];
  for (long local_2 = 0; local_2 < arg_1->unknown88.size; ++local_2)
  {
   byte *local_3 = (byte *)&arg_1->unknown88.data[local_2];
   if (*(word *)local_3 == local_1->field_20 && *(long *)(local_3 + 0x14) == arg_3 &&
    arg_4 == *(signed char *)(local_3 + 0x45) && arg_5 == *(signed char *)(local_3 + 0x46) &&
    arg_0->field_4->field_18 == 1 && arg_0->field_4->field_20)
   {
    struct { void *volatile field_0; bool field_4; } local_4;
    local_4.field_4 = arg_0->field_4->field_20 != arg_0->field_10;
    if (local_4.field_4) local_4.field_0 = arg_0->field_4->field_0;
    else local_4.field_0 = *arg_0->field_8;
    function_1cf520((s_1cf520 *)local_3, arg_0->field_0, false, local_4.field_4, arg_2, arg_4);
   }
  }
 }
}

struct s_1d56a0
{
 byte field_0[8];
 byte *field_8;
};

// @retail 0x1d56a0
void function_1d56a0(s_havok_component *arg_0)
{
 byte *local_0 = ((s_1d56a0 *)g_4e0300->data)[arg_0->object_index & 0xffff].field_8;
 arg_0->unknown1c = function_1d5940(arg_0, 0, 0, 1);
 if (arg_0->unknown1c <= 1)
  *(dword *)(local_0 + 0x12c) |= 1;
 else
  *(dword *)(local_0 + 0x12c) &= ~1;
}

void __stdcall function_1d6d00(s_havok_component *arg_0, void *arg_1, byte *arg_2,
 long arg_3, long arg_4, long arg_5, bool arg_6);

// @retail 0x1d6ca0
void function_1d6ca0(s_havok_component *arg_0)
{
 byte *local_0 = ((s_1d56a0 *)g_4e0300->data)[arg_0->object_index & 0xffff].field_8;
 function_1d6d00(arg_0, g_4e3b44[*(long *)local_0 & 0xffff].bytes + 0xd4,
  local_0 + 0x17c, 0xf, 0xf, 0, local_0[0x17c] == 3);
}
