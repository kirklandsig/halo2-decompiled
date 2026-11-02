// @flags /O2 /Ob1 /arch:SSE /Gr
#include "orientations_storage.h"
#include "slot_handler.h"
static __forceinline long real_to_long(real value);
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
#include "normalize3d.h"
#include "vector_products.h"
#include "constraint_helpers_314090.h"
#include "joint_descriptions_312920.h"

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
void function_1d43d0(s_havok_component *component, transform4x3f const *matrix, transform4x3f *result);

// @retail 0x1d4360
void function_1d4360(s_havok_component *component, transform4x3f *volatile result)
{
	transform4x3f matrix;
	short index = havok_component_main_rigid_body_index_get(component);
	if (index != NONE)
	{
		component->rigid_body_matrix_get(index, &matrix);
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
long function_1d3880(s_havok_component const *component, long *constraints, long *bodies, long *contacts, long *other)
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
void havok_entity_component_index_set(hkEntity *entity, long component_index)
{
	if (havok_entity_property_exists(entity, HAVOK_PROPERTY_COMPONENT_INDEX))
	{
		entity->entity_property_erase(HAVOK_PROPERTY_COMPONENT_INDEX);
	}
	entity->entity_property_put(HAVOK_PROPERTY_COMPONENT_INDEX, *(s_havok_property_word const *)&component_index);
}

// @retail 0x1cf300
long havok_entity_property_2002_get(hkEntity const *entity)
{
	return havok_entity_property_get(entity, HAVOK_PROPERTY_2002);
}

// @retail 0x1cf330
void havok_entity_property_2002_set(hkEntity *entity, s_havok_property_word value)
{
	if (havok_entity_property_exists(entity, HAVOK_PROPERTY_2002))
	{
		entity->entity_property_erase(HAVOK_PROPERTY_2002);
	}
	entity->entity_property_put(HAVOK_PROPERTY_2002, value);
}

// @retail 0x1cf380
long __cdecl havok_entity_property_2003_get(hkEntity const *entity)
{
	return havok_entity_property_get(entity, HAVOK_PROPERTY_2003);
}

// @retail 0x1cf3b0
void havok_entity_property_2003_set(hkEntity *entity, long value)
{
	if (havok_entity_property_exists(entity, HAVOK_PROPERTY_2003))
	{
		entity->entity_property_erase(HAVOK_PROPERTY_2003);
	}
	entity->entity_property_put(HAVOK_PROPERTY_2003, *(s_havok_property_word const *)&value);
}

// @retail 0x1cf950
void s_havok_component::initialize(long object_index)
{
	s_havok_component *component = this;

	component->unknown04 = 0;
	component->object_index = object_index;
	component->unknown0c = NONE;
	component->unknown10 = -real_to_long(g_510c54->field_2_3 * 0.35f);
	component->unknown18 = NONE;
	component->unknown19 = NONE;
	component->unknown20 = NONE;
	component->unknown14 = 0.0f;
	component->unknown1a = false;
	component->unknown1b = false;
	component->unknown1c = false;
	s_havok_array60 *bodies = &component->rigid_bodies;
	bodies->data = NULL;
	bodies->size = 0;
	bodies->capacity_and_flags = 0x80000000;
	s_havok_array0c *contacts = &component->unknown7c;
	contacts->data = NULL;
	contacts->size = 0;
	contacts->capacity_and_flags = 0x80000000;
	s_havok_array48 *constraints = &component->unknown88;
	constraints->data = NULL;
	constraints->size = 0;
	constraints->capacity_and_flags = 0x80000000;
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
void havok_component_friction_get(s_havok_friction *result, long component_index, s_havok_material const *material)
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
		rigid_body->pose_write(transform);
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
long __stdcall function_1d5940(s_havok_component *component, bool a, bool b, bool c);
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

	long object_type = object->type;
	switch (object_type)
	{
	case 11:
		function_1d56a0(component);
		break;
	case 0:
		function_1d6b80(component);
		break;
	case 12:
		function_1d6ca0(component);
		break;
	case 7:
		component->unknown04 |= 0x80;
		component->unknown1c = function_1d5940(component, 0, 1, 0);
		break;
	case 1:
		function_1d56f0(component);
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
	if (!rigid_body->body_is_simulating().m_bool && rigid_body->m_simulation_island)
	{
		rigid_body->activate();
	}
}

// @retail 0x1d08e0
void s_havok_component::rigid_body_matrix_get(long rigid_body_index, transform4x3f *matrix)
{
	hkTransform transform;

	transform.set(havok_component_rigid_body_get(rigid_body_index, this)->m_motion->m_transform);

	if (TEST_FIELD_BIT(transformed))
	{
		transform.transform_compose_in_place(this->transform);
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

		component->rigid_body_matrix_get(rigid_body_index, &matrix);
		*position = matrix.position;
	}
	else
	{
		vector3d_from_havok((vector3f *)position, &havok_component_rigid_body_get(rigid_body_index, component)->m_motion->m_transform.m_translation);
	}
}

/* retail inlines the velocity getters into this file's callers */
static inline void rigid_body_linear_velocity_get(s_havok_component *component, long rigid_body_index, vector3f *velocity)
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

static inline void rigid_body_angular_velocity_get(s_havok_component *component, long rigid_body_index, vector3f *velocity)
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

/* both velocities of one rigid body (the zero vector for a fixed one) */
static __forceinline void rigid_body_velocities_get(hkRigidBody *rigid_body, vector3f *linear_velocity, vector3f *angular_velocity)
{
	if (!rigid_body->m_fixed)
		vector3d_from_havok(linear_velocity, &rigid_body->m_motion->m_linear_velocity);
	else
		*linear_velocity = *g_4687a4;
	if (!rigid_body->m_fixed)
		vector3d_from_havok(angular_velocity, &rigid_body->m_motion->m_angular_velocity);
	else
		*angular_velocity = *g_4687a4;
}

// @retail 0x1d09d0
void havok_component_rigid_body_linear_velocity_get(s_havok_component *component, long rigid_body_index, vector3f *velocity)
{
	/* the rigid body read through a pointer to the array at each use keeps
	   retail's [index + data] addressing with this parameter order */
	s_havok_array60 const *rigid_bodies = &component->rigid_bodies;

	if (!rigid_bodies->data[rigid_body_index].rigid_body->m_fixed)
	{
		vector3d_from_havok(velocity, &rigid_bodies->data[rigid_body_index].rigid_body->m_motion->m_linear_velocity);
	}
	else
	{
		*velocity = *g_4687a4;
	}
}

// @retail 0x1d0ad0
void havok_component_rigid_body_angular_velocity_get(s_havok_component *component, long rigid_body_index, vector3f *velocity)
{
	/* the rigid body read through a pointer to the array at each use keeps
	   retail's [index + data] addressing with this parameter order */
	s_havok_array60 const *rigid_bodies = &component->rigid_bodies;

	if (!rigid_bodies->data[rigid_body_index].rigid_body->m_fixed)
	{
		vector3d_from_havok(velocity, &rigid_bodies->data[rigid_body_index].rigid_body->m_motion->m_angular_velocity);
	}
	else
	{
		*velocity = *g_4687a4;
	}
}

// @retail 0x1d0a20
void havok_component_rigid_body_point_velocity_get(long rigid_body_index, s_havok_component *component, point3f const *point, vector3f *velocity)
{
    s_havok_component *const *local_0 = &component;
    s_havok_component *local_1 = *local_0;
	if (!havok_component_rigid_body_get(rigid_body_index, local_1)->m_fixed)
	{
		hkVector4 havok_point;
		hkVector4 havok_velocity;

		havok_from_vector3d(&havok_point, (vector3f const *)point);
		havok_component_rigid_body_get(rigid_body_index, local_1)->m_motion->getPointVelocity(havok_point, havok_velocity);
		vector3d_from_havok(velocity, &havok_velocity);
	}
	else
	{
		*velocity = *g_4687a4;
	}
}

// @retail 0x1d0b20
void s_havok_component::rigid_body_inertia_get(long rigid_body_index, matrix3x3 *inertia)
{
	hkRotation havok_inertia;

	havok_component_rigid_body_get(rigid_body_index, this)->m_motion->getInertiaWorld(havok_inertia);
	vector3d_from_havok(&inertia->forward, &havok_inertia.m_col0);
	vector3d_from_havok(&inertia->left, &havok_inertia.m_col1);
	vector3d_from_havok(&inertia->up, &havok_inertia.m_col2);
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))

// @retail 0x1d0bb0
real s_havok_component::rigid_body_mass_get(long rigid_body_index)
{
	return MAX(1.0f, havok_component_rigid_body_get(rigid_body_index, this)->m_motion->getMass());
}

// @retail 0x1d0c00
bool s_havok_component::rigid_body_keyframed(long rigid_body_index)
{
	return havok_component_rigid_body_get(rigid_body_index, this)->m_motion->kind_code() == 6;
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

// score 29 (was 29): retail keeps the slot address in eax, tests the fixed flag through edx/dl and reads the
// rigid body again after the vector's stores; ours reads it once into esi. Tried: the Havok vector declared
// at the top, in the outer block, with the body read at its use, with every local at the top, or with the
// body in a named local for the test: 46 each (retail's reload, but the flag byte goes to bl: push ebx).
// Round 89: slot as a `hkRigidBody *const &` reference with the vector at the top and no own-parameter read
// 44 (component in EDX); that reference with the own-parameter read 39 (reloads after activation too); slot
// pointer with the vector after/before it in the flag block, getter for the fixed test, `== false`, own
// parameter read into a new local: 48 each (retail's code but for BL); one `&&` test with the getter twice
// 53. Matched code has 263 `mov dl, [edx+N]` loads, so BL means our first body load stays live.
// Round 139: the getter into a body local with no own-parameter read 41 (component in EDX, callers
// lose matches); the slot pointer with the vector in the flag block and the byte in a named bool 48
// (BL); that without the own-parameter read 47; the slot computed before one `flag1 || fixed` test
// with the vector at the top 48; the body as a `hkRigidBody *const &` with the vector in the flag
// block 46 (index moves to ECX). Diagnostic: the vector stores through an opaque pointer (forcing
// the reload) still give BL 52, so a shared first body load is not what takes DL. In every form our
// build never loads the fixed byte into the dying base register (ECX base -> DL, EDX/EAX base -> BL),
// where retail does (`mov edx, [eax]; mov dl, [edx + 0x40]`).
// @retail 0x1d0e50
void havok_component_rigid_body_angular_velocity_set(long rigid_body_index, s_havok_component *component, vector3f const *velocity)
{
	s_havok_component *const *local_0 = &component;
	component = *local_0;
	if (!TEST_FIELD_BIT(component->flag1))
	{
		hkRigidBody *const *local_1 = &component->rigid_bodies.data[rigid_body_index].rigid_body;
		if (!(*local_1)->m_fixed)
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
		if (havok_component_rigid_body_get(rigid_body_index, component)->body_is_simulating().m_bool)
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
		result = havok_component_rigid_body_get(rigid_body_index, component)->m_motion->kind_code() != 6 &&
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
		m_motion->pose_write(transform);
	}
}

void function_1d1260(s_havok_component *component);
void function_1d1540(s_havok_component *component);

// score 81 (was 81): component arrives on the stack where retail passes it in ebx (no push ebx,
// plain ret, frame 0x88), the entry copy is rep movsd where retail uses movaps, and retail tests
// m_fixed before flag5. Tried: copy through hkTransform::set 77 at 237 bytes (0x1d1b60 lost);
// set + body pointer + m_fixed first 88 at 230 (retail's code except the ebx parameter); that with
// flag5 first 27 at 220 (component in edi, costs 0x1d0c20 and 0x1d1b60); as a member function, every
// parameter order, body pointer at the top, else arm first, early return, noinline: all as 88.
// The 88 body is retail's but for the component's stack home (allocated to ebx, loaded from the
// stack); the getter at each use 88; this function declared ahead of 0x1d1260 88; 0x1d0c20 defined
// first 88. Without the 0x1d1260 call the component arrives in edi (89); without the 0x1d1540 call
// it stays on the stack (77): the call to 0x1d1260 with the component live across it decides it.
// Reset/rebuild inline/noinline contracts, original then aligned-copy/fixed-or-flag-first graphs
// 81/81/81/81/81/81/81/81/88/88/88/88/27/27/27/27. Original executable source restored. All four 27
// forms still lose matched 0x1d1b60 at 59 in the reduced image.
// Round 28 (lane G): the declaration order does not decide the argument registers: with retail's
// order (transform, index, component) and the old order kept only for the caller in another file,
// the score is still 81 here and 38 in 0x1d0c20, and matched 0x1d1b60 stays matched, so the order
// was left as it is. What still differs: retail keeps the component in EBX, ours on the stack.
// Both-inline paired form: plain 27 at 220 bytes; matched 0x1d1b60 loses 59; rejected.
// Round 158: assigned copy + body pointer + m_fixed first 88 at 230 (retail's code; a /Gd first
// pass loads the component once into ebx, yet it stays a stack argument); same with set() 88; flag5
// first 27 (component edi); body fetched before the transformed block, either test order, 74 (edi).
// The edi forms take 0x1d5120 from 77 to 15 but lose 0x1d1b60 (59) and move 0x1d0c20 to 49.
// Round 33: the inverse step as a shared force-inline helper (with 0x1d0ee0) 81; that with the
// retail body 88 (index now in ECX, ret 4: only the component's stack home and the frame shift it
// causes differ); a volatile read of the component at either call or the second fetch 89/89/92;
// the whole body inside a force-inline helper (diagnostic) 88. The same 88 result without 0x1d5120
// in the image; a /Gd compile loads the component once into EBX. Matched 0x1c59a0, 0x1cbf50 and
// 0x136860 take EBX arguments with self-aligned frames, so the alignment alone is not the cause.
// Round 431: Library transform copies with original/fixed-first body receiver: memcpy and memmove, then
// both with the inverse reserved at entry: 81/85/77/76/81/85/77/76. The 76 forms are five-point gains
// only; both 77 forms lose matched 0x1d1b60 at 60. Original executable body restored.
// Round 89: the 88 body with each other parameter order 88 each (0x1d0c20 40); without 0x1d1b60's call to
// 0x1d0c20 86 (still a stack argument). A /Gd first pass of the 88 body uses EBX, ESI and EDI (component in
// EBX); without the 0x1d1540 call also all three (stack); without the 0x1d1260 call only ESI and EDI
// (component an EDI argument); matched 0x1cbf50's first pass uses ESI and EDI and LTCG passes its parameter
// in the unused EBX. Lead: a body whose first pass needs at most two callee-saved registers.
// Round 108 (on the 88 body: hkTransform::set copy, body pointer, m_fixed first): control 88 at 230; the
// fixed arm's refetch assigned back to the body pointer 88; the body pointer declared first, or declared
// after the copy target and assigned at the fetch, 88/88; the setter declared without `inline` (both arms
// call it, 182 bytes) 112, the component still loaded from its stack home into EBX, so the body held across
// the inlined setter's calls is not the cause; the same-file caller's call removed (diagnostic) 86, the
// component then loaded into ESI, still a stack argument. Original body restored.
// Lane 15 round 18: 0x1d8e80 moved to its address neighbours' file (src/unknown_1d8dd0.cpp): identical
// code (81); the 88 body with that move 88 (its /Gd first pass then uses only ESI+EDI, component in
// EDI, so the two-register rule does not decide it); flag5 first 27 again (component EDI, 0x1d1b60
// lost at 59, 0x1d3fc0 149 -> 179, 0x1d5120 77 -> 15); that with the move 27; flag5 first with the
// plain struct copy 85. Diagnostics: the 88 body's /Gd pass loads the component once into EBX (flag5
// first: EDI) and only the EDI form becomes a register argument; without 0x1d0c20's call LTCG gives the
// 88 body the component in ECX and the index on the stack, without 0x1d5120's call nothing changes, and
// 0x1d0c20's aligned transform moved into a forceinline helper changes nothing: 0x1d0c20's call decides it.
// Round 93 (lane 8): the 88 body (set copy, body pointer, m_fixed first) again 88 (230 bytes; its code is
// retail's but for the component loaded from [ebp + 8] into EBX with push/pop, ret 4); that keeping the
// first body pointer for the fixed arm's call (no second fetch) 77 at 220 (index ECX, component an EDI
// register argument, only ESI saved and EBX unused; 0x1d5120 77 -> 15, but matched 0x1d1b60 59 and
// retail fetches again after 0x1d1260); the transform by reference 88; inverse declared with local 88;
// the second fetch into the body local 88. The lead stands: LTCG made the component a register argument
// only when the first pass left a callee-saved register free.
// Round 250: One body receiver with conditional post-reset refetch: constructor copy, flag/fixed-
// first and bool/byte refresh 81/81/81/81 at 220 bytes; set copy 85/85/86/86 at 231. Frame/component
// convention remains; every matched control survives.
// Round 13 (lane 18, fast, with 0x1d0c20/0x1d1b60/0x1d5120/0x1d3fc0): retail's argument setup at both
// callers is ECX (index), then EBX (component), then EAX (transform), so the setup-order rule gives
// (transform, component, index): on this body 81 (0x1d1b60 lost at 2), on the 88 body (set copy, body
// pointer, m_fixed first) 88; the 88 body with a const body pointer 88; the transform by reference 88;
// the component by reference 88 (the 88 forms keep 0x1d5120 at 73, 0x1d0c20 40). A /Gd compile-only first
// pass of this body keeps the component in ESI and never touches EBX, yet LTCG still stacks it, so the
// first pass alone does not predict the EBX parameter.
// Round 41 (lane 11, fast, with 0x1d0c20/0x1d1b60/0x1d5120): the 88 body again 88 (0x1d0c20 40, 0x1d5120 73);
// on it the plain struct copy in place of set() 85 at 220 bytes (rep movsd, component still loaded from
// [ebp + 0xc] into EBX; 0x1d0c20 38, 0x1d5120 77); the fixed arm refetching into the body pointer and
// returning on its own 88. On the plain-copy body, parameters (transform, index, component) 85 (same code;
// definition, both declarations and both calls changed together) and (component, index, transform) 85 with
// matched 0x1d1b60 lost at 2.
// Round 60 (m3 lane 9, fast with 0x1d0c20/0x1d1b60/0x1d5120): on the 88 body (set copy, body pointer,
// m_fixed first; aligned 8, only the component's stack home): the definition marked `inline` 88; the
// reset test as a named bool 154 (278 bytes); the body read by direct rigid_bodies indexing 88; the fixed
// and flag tests nested with a return in the reset arm 88; the transformed flag read before the copy 88.
// It is on the object/havok call cycle (0x1d1260 and 0x1d1540 reach it again through 0x1d1b60 ->
// 0x1d0c20 without any stubbed function), so where LTCG enters that cycle may decide its convention.
// Round 26 (lane 17, plain): 0x1d1260/0x1d1540 declared at the top of this file 81 (same code); that with the
// 88 body 88 (0x1d0c20 40, 0x1d5120 73), so the cycle walk order is not what keeps the component on the stack;
// the 88 body with flag5 read into a named bool before the body fetch 88, with a member-wise copy 88 (fast).
// @retail 0x1d0cf0
void havok_component_rigid_body_transform_set(long rigid_body_index, s_havok_component *component, hkTransform const *transform)
{
	hkTransform local = *transform;
	if (TEST_FIELD_BIT(component->transformed))
	{
		hkTransform inverse;

		inverse.transform_set_to_inverse(component->transform);
		local.transform_compose_in_place(inverse);
	}
	if (TEST_FIELD_BIT(component->flag5) && havok_component_rigid_body_get(rigid_body_index, component)->m_fixed)
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

/* the Havok frame of a game matrix (its scale is not carried) */
PRIVATE inline void component_matrix_frame(transform4x3f const *matrix, hkTransform *frame)
{
	havok_from_vector3d(&frame->m_rotation.m_col0, &matrix->forward);
	havok_from_vector3d(&frame->m_rotation.m_col1, &matrix->left);
	havok_from_vector3d(&frame->m_rotation.m_col2, &matrix->up);
	hkVector4 position;
	havok_from_vector3d(&position, (vector3f const *)&matrix->position);
	frame->m_translation = position;
}

// score 38 (was 38): BLOCKED-BY 0x1d0cf0: retail passes it the component in EBX (loaded here from the
// stack after `push ebx`); ours pushes the component, so the frame and every store offset differ. The
// body is otherwise retail's instruction for instruction. Tried here: zero transform/translation/both
// 64/61/84; rotation workspace/initialized workspace/initialized transform with workspace 78/106/107;
// position input value 77; rotation input values 143; external matrix/component records 38/38;
// complete frame set/initialization/library copy 81/71/71; translation memcpy 70; aligned forward/up
// column values 49/49; defined ahead of 0x1d0cf0 38; native left/all/left-up/forward-left columns
// 48/69/58/59; SIMD translation/rotation/all/left-plus-translation 38/69/69/48. Through 0x1d0cf0
// (not kept): its retail body (vector copy, body pointer, m_fixed first) 40 (component stays on the
// stack there); flag5 first 49; body fetched first 49/49; every form that makes the component a
// register argument puts it in EDI, not EBX, and costs matched 0x1d1b60. Round 33: the shared
// world-to-local transform step as a force-inline helper (also used by 0x1d0ee0, which stays matched)
// 38, with 0x1d0cf0's retail body 40; that with a volatile read of 0x1d0cf0's component at the
// 0x1d1540 call/the 0x1d1260 call/the second body fetch 40/40/40. Round 89: through the existing
// component_matrix_frame converter 38 (same code, kept), with 0x1d0cf0's retail body 40.
// Round 28 (lane G): retail's arguments here are (matrix, index, component) as in 0x1d0cf0; the
// declaration order does not change the registers LTCG picks (0x1d0cf0 81 and this 38 either
// way), so the source keeps its order. Every remaining difference follows 0x1d0cf0's convention.
// @retail 0x1d0c20
void havok_component_rigid_body_matrix_set(long rigid_body_index, s_havok_component *component, transform4x3f const *matrix)
{
	hkTransform transform;

	component_matrix_frame(matrix, &transform);
	havok_component_rigid_body_transform_set(rigid_body_index, component, &transform);
}

// @retail 0x1d1010
void havok_component_rigid_body_linear_velocity_change(long rigid_body_index, s_havok_component *component, vector3f const *change)
{
	vector3f velocity;

	rigid_body_linear_velocity_get(component, rigid_body_index, &velocity);
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
		if (component->rigid_bodies.data[rigid_body_index].node_count > 0)
		{
			long node_index = component->rigid_bodies.data[rigid_body_index].nodes[0];

			if (node_index >= 0 && node_index < MAXIMUM_HAVOK_NODES)
			{
				states->valid[node_index >> 5] |= 1 << (node_index & 0x1f);
				component->rigid_body_matrix_get(rigid_body_index, &states->matrices[node_index]);
				rigid_body_linear_velocity_get(component, rigid_body_index, &states->linear_velocities[node_index]);
				rigid_body_angular_velocity_get(component, rigid_body_index, &states->angular_velocities[node_index]);
			}
		}
	}
}

// @retail 0x1d1b60
void havok_component_node_states_set(s_havok_component *component, s_havok_node_states const *states)
{
	long rigid_body_index;
	s_havok_component_rigid_body *bodies;

	for (rigid_body_index = 0; rigid_body_index < component->rigid_bodies.size; rigid_body_index++)
	{
		bodies = component->rigid_bodies.data;
		/* the pointer serves the test only; retail re-reads the array for the node */
		if (bodies[rigid_body_index].node_count > 0)
		{
			long node_index = component->rigid_bodies.data[rigid_body_index].nodes[0];

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
	rigid_body_velocities_get(havok_component_rigid_body_get(rigid_body_index, component), &linear_velocity, &angular_velocity);
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
	byte unknown04[0x10a - 4];
	word flags10a_0 : 2;
	word flag10a_2 : 1;
	word flags10a_3 : 13;
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
		s_component_collision_object *other_object = header->object;
		long mode = *(char *)&other->unknown1c;
		long type = header->type;
		long type_mask = 1 << type;
		bool attached = (type_mask & 3) && ((s_component_collision_object *)havok_object_get(other->object_index))->attached_index != NONE;
		bool flagged = (type_mask & 1) && TEST_FIELD_BIT(other_object->flag10a_2);
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
	havok_entity_property_2003_set(body, 0);
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
	havok_entity_component_index_set((hkEntity *)body, object->havok_component_index);
	havok_entity_property_2002_set((hkEntity *)body, *(s_havok_property_word const *)&body_index);
	if (body->m_motion->kind_code() != 7 && body->m_motion->kind_code() != 6)
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
		inverse.transform_set_to_inverse(component->transform);
		transform.transform_compose_in_place(inverse);
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

// score 26 (was 26): earlier 43; Acceleration XMM0/memory timestep prefix and stop-test schedule still differ;
// negative-target guard 50; stopping branch first 106; step predicate first 26 (kept, plain 26);
// negative guard with step predicate 32; independent/eager stop decisions 129/124;
// explicit fraction branches 43; velocity input/output lifetime 219.
// acceleration/timestep registers and stop scheduling remain; direct stopping decision 106;
// conditional/stopping-first results, stopping ticks, squared speed, limit magnitude, normalized speed
// and captured step decision 26 each. Original body retained.
// acceleration/timestep allocation remains; assigned correction and positive clamp 26;
// nonpositive/all assigned clamps 45; shared velocity clamp 148; shared bounds 148 (449/450 bytes).
// acceleration register, memory timestep multiply and stop-test schedule differ.
// Ordinary acceleration/magnitude/time inline contracts and paired forms 26;
// completed clamp before ratio 82; accelerated candidate default 114.
// the time step is still loaded before its product; retail multiplies from memory
// and keeps the step's address. Step on the left of both products 26; direct field reads 26;
// one-expression step 26; address-taken target 26; step pointer first 142; direct reads with the
// stopping test first 43; product inside the scale helper 26; per-arm mass step 135.
// acceleration/timestep allocation and stopping schedule remain.
// Combined forced/stopping step-first, distance-first, eager distance, conditional result,
// named decision and stopping guard 106 each; merged default/return scaling 140/110.
// isolated mass return 113; threshold-first return 121; returned
// selection/nested return/power selection 113 each; direct/default/assigned magnitude
// results with mass return 113/127/113. XMM0 prefix improves; memory timestep still differs.
// acceleration/timestep allocation and stopping order remain.
// Mass eligibility early scale/mass-first 164/166, late 26/120; selected divisor early/late 143/114;
// early eligibility with selected normalization scale/mass-first 164/166. Original helper retained.
// Round 15: stop distance tested before the step 43 (retail's test order, zero register rotated);
// the same with the step on the right of both products 43; time-step reference declared first 142,
// with the stop-first test 142; time step copied into a value local 99 (acceleration in xmm0 as
// retail, but the step stays in a register instead of the memory multiply).
// Round 115 (lane 6, fast): the stop-first test with the stop ticks and tick distance named 43; the time
// step bound inside the scale helper between the exponent result and the mass division, the helper
// returning the step 31 (the time-step global is then loaded before the mass test, still loaded before
// its product); the helper scaling its own acceleration parameter 132 (the acceleration load hoisted);
// the step as a value local at function scope 147 (479 bytes); `(velocity * delta_time)` as the first
// factor of the stop distance 26. One-file LTCG harness screens (exact here, ~45 forms): every product
// operand order, either test order, value/reference/pointer/struct-pointer reads of the step and the
// stop distance's groupings give the load-first products; an intervening float store (to a file global
// or through `ratio`) never folds the step either, and `(void)&local` changes nothing.
// Lead (harness diagnostics): the product's operand order follows where the step's value is first defined.
// A step value read before the exponent test and used only in the step product gives retail's in-place
// `mulss xmm0, [mem]` with the scaled value in xmm0, but it is spilled across the power call (`mulss xmm0,
// [esp+8]`, 160, 490 bytes); read after the exponent result 50; a constant multiplier also puts the scaled
// value in xmm0. Retail loads the global right at the product, so its step read must rank earlier in another way.
// @retail 0x1d2280
real function_1d2280(bool allow_reverse, bool scale_by_mass, real mass, real target, real acceleration,
	real velocity, real limit, real position, real exponent, bool force_acceleration, real *ratio)
{
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
			if (target > step && target > stop_distance)
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


PRIVATE inline void component_property_replace(hkEntity *entity, dword key, s_havok_property_word value)
{
 if (havok_entity_property_exists(entity, key))
  entity->entity_property_erase(key);
 entity->entity_property_put(key, value);
}

/* the size of the loaded cache file, 0 when none is loaded */
PRIVATE inline long cache_file_size_1d01c0()
{
 if (cache_file_globals.loaded)
  return cache_file_globals.header.unknown1c;
 return 0;
}

/* drops a reference to a body and frees it when none are left, unless it sits
   in the pinned memory range (from 0x80061000 to the end of the loaded cache
   file's size past it); the size helper is called twice on purpose, since a
   limit computed once is shared and changes the add/sub spelling of the sum */
PRIVATE inline void component_body_release(c_havok_reference_counted *body)
{
 --body->reference_count;
 if (body->reference_count == 0)
 {
  long value = (long)body;
  if ((value < (long)0x80061000 ? (long)0x80061000 : value > (long)0x80061000 + cache_file_size_1d01c0() ? (long)0x80061000 + cache_file_size_1d01c0() : value) == value)
   body->reference_count = 1;
  else
   delete body;
 }
}

/* frees an hkArray's storage unless the array does not own it */
PRIVATE inline void nodes_free_1d01c0(s_havok_array *array)
{
 if (!(array->capacity_and_flags & 0x80000000))
  g_480118->allocate((long)array->data, array->capacity_and_flags & 0x7fffffff, 0x12);
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
  nodes_free_1d01c0((s_havok_array *)&entry->nodes);
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

// score 29 (was 172): only the orientations branch differs: retail reads both matrix offsets
// before inverting the parent (the call to 0x141590) and keeps the node's matrix base (object
// plus offset) in EDI across that call, where ours keeps the object and reads the offset again
// after it. Tried at 172 (lane): stable handle 197; pointer names 172; in-place composition 212;
// default point setters 188; sibling induction 175. Tried at 29: reordering the locals and
// precomputing the matrix pointer (no change).
// @retail 0x1d3d20 standard
void s_havok_component::function_1d3d20(long node_index, dword *updated, dword const *restore,
 rigid_transform_scaled *orientations, long unknown)
{
 s_havok_component *component = this;
 s_component_node_object *object = (s_component_node_object *)havok_object_get(component->object_index);
 s_component_transform_definition *definition = (s_component_transform_definition *)g_4e3b44[object->definition_index & 0xffff].bytes;
 s_component_transform_model *model = (s_component_transform_model *)g_4e3b44[definition->model_index & 0xffff].bytes;
 long physics_index = ((s_component_collision_model_link *)model)->physics_model_index;
 s_component_node_physics *physics = (s_component_node_physics *)g_4e3b44[physics_index & 0xffff].bytes;
 s_component_node_link *node = &physics->nodes[node_index];
 short parent = node->parent;
 s_component_node_render_model *render_model = (s_component_node_render_model *)g_4e3b44[model->render_model_index & 0xffff].bytes;
 if (parent != NONE)
 {
  dword mask = 1 << (node_index & 31);
  if (updated[node_index >> 5] & mask)
  {
   if (orientations)
   {
    transform4x3f *matrices = component_node_matrices(component->object_index);
    transform4x3f *parent_matrix = object_node_matrix(object_get(component->object_index), parent);
    transform4x3f inverse;
    transform4x3f relative;
    function_141590(parent_matrix, &inverse);
    function_142a60(&inverse, &matrices[(short)node_index], &relative);
    orientation_from_matrix4x3(&relative, &orientations[node_index]);
   }
  }
  else if (restore[node_index >> 5] & mask)
  {
   s_component_node_default *initial = &render_model->nodes[node_index];
   transform4x3f *matrix = object_node_matrix(object_get(component->object_index), (short)node_index);
   transform4x3f *matrices = component_node_matrices(component->object_index);
   function_141e10(&matrix->rotation, &initial->rotation);
   matrix->position.x = 0.0f;
   matrix->position.y = 0.0f;
   matrix->position.z = 0.0f;
   matrix->scale = 1.0f;
   matrix->position = initial->position;
   function_142a60(&matrices[parent], matrix, matrix);
   updated[node_index >> 5] |= mask;
   if (orientations)
   {
    orientations[node_index].rotation = initial->rotation;
    orientations[node_index].position = initial->position;
    orientations[node_index].scale = 1.0f;
   }
  }
 }
 long child = node->child;
 while (child != NONE)
 {
  s_component_node_link *next = &physics->nodes[child];
  component->function_1d3d20(child, updated, restore, orientations, unknown);
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

// @retail 0x1d35d0
void function_1d35d0(s_havok_component *component, long constraint_index, real scale)
{
    s_havok_component_element0c *constraint = &component->unknown7c.data[constraint_index];
    s_component_object_transform_view *object = (s_component_object_transform_view *)havok_object_get(component->object_index);
    s_component_transform_definition *definition = (s_component_transform_definition *)g_4e3b44[object->definition_index & 0xffff].bytes;
    s_component_collision_model_link *model = (s_component_collision_model_link *)g_4e3b44[definition->model_index & 0xffff].bytes;
    s_component_joint_model *physics = (s_component_joint_model *)g_4e3b44[model->physics_model_index & 0xffff].bytes;
    real override_strength = 0.0f;
    for (long i = 0; i < physics->override_count; ++i)
    {
        s_component_joint_override_group *group = &physics->overrides[i];
        for (long j = 0; j < group->count; ++j)
        {
            s_component_joint_override *entry = &group->entries[j];
            if (entry->type == constraint->unknown00 && entry->index == constraint->unknown02)
            {
                override_strength = entry->strength;
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
            joint[0x8c / 4] = (angles->twist_minimum > angles->twist_maximum ? angles->twist_maximum : angles->twist_minimum) * scale * 0.01745329238474369f;
            joint[0x90 / 4] = (angles->twist_minimum > angles->twist_maximum ? angles->twist_minimum : angles->twist_maximum) * scale * 0.01745329238474369f;
            joint[0x84 / 4] = (real)sin((angles->plane_minimum > angles->plane_maximum ? angles->plane_maximum : angles->plane_minimum) * scale * 0.01745329238474369f);
            joint[0x88 / 4] = (real)sin((angles->plane_minimum > angles->plane_maximum ? angles->plane_minimum : angles->plane_maximum) * scale * 0.01745329238474369f);
            joint[0x80 / 4] = (real)cos((angles->cone_minimum > angles->cone_maximum ? angles->cone_minimum : angles->cone_maximum) * scale * 0.01745329238474369f);
            ((lane_a_ragdoll *)joint)->set_strength((override_strength != 0.0f ? override_strength : angles->strength) * scale);
        }
        break;
    case 1:
        {
            s_component_joint_hinge *hinge = &physics->hinges[constraint->unknown02];
            real *joint = (real *)constraint->contact;
            real strength = override_strength;
            if (strength == 0.0f)
                strength = hinge->strength;
            joint[0xd0 / 4] = strength * scale;
            real lower = 0.0f - hinge->minimum;
            real upper = 0.0f - hinge->maximum;
            joint[0xc8 / 4] = (lower > upper ? upper : lower) * scale * 0.01745329238474369f;
            lower = 0.0f - hinge->minimum;
            upper = 0.0f - hinge->maximum;
            joint[0xcc / 4] = (lower > upper ? lower : upper) * scale * 0.01745329238474369f;
        }
        break;
    }
}

// @retail 0x1cff80
void function_1cff80(s_havok_component_element0c const *constraint, point3f *pivot_a, point3f *pivot_b)
{
    switch (constraint->unknown00)
    {
    case 2:
        {
            lane_a_ragdoll *joint = (lane_a_ragdoll *)constraint->contact;
            c_component_joint_snapshot_2 snapshot;
            joint->export_description(&snapshot);
            real x = snapshot.pivot_a.m128_f32[0];
            real y = snapshot.pivot_a.m128_f32[1];
            real z = snapshot.pivot_a.m128_f32[2];
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
            lane_a_limited_hinge *joint = (lane_a_limited_hinge *)constraint->contact;
            c_component_joint_snapshot_1 snapshot;
            joint->export_description(&snapshot);
            real x = snapshot.pivot_a.m128_f32[0];
            real y = snapshot.pivot_a.m128_f32[1];
            real z = snapshot.pivot_a.m128_f32[2];
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
            lane_a_hinge *joint = (lane_a_hinge *)constraint->contact;
            c_component_joint_snapshot_0 snapshot;
            joint->export_description(&snapshot);
            real x = snapshot.pivot_a.m128_f32[0];
            real y = snapshot.pivot_a.m128_f32[1];
            real z = snapshot.pivot_a.m128_f32[2];
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
            lane_a_ball_socket *joint = (lane_a_ball_socket *)constraint->contact;
            c_component_joint_snapshot_4 snapshot;
            joint->export_description(&snapshot);
            real x = snapshot.pivot_a.m128_f32[0];
            real y = snapshot.pivot_a.m128_f32[1];
            real z = snapshot.pivot_a.m128_f32[2];
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

// a vector of four copies of a real
PRIVATE inline __m128 component_bounds_splat(real value)
{
    __m128 const single = _mm_set_ss(value);
    return _mm_shuffle_ps(single, single, 0);
}

// @retail 0x1d44f0
bool __stdcall function_1d44f0(s_havok_component *component, point3f *center, real *radius)
{
    bool result = false;
    if (component->rigid_bodies.size > 0 && TEST_FIELD_BIT(component->flag5) && TEST_FIELD_BIT(component->flag11))
    {
        s_component_object_transform_view *object = (s_component_object_transform_view *)havok_object_get(component->object_index);
        byte *definition_bytes = g_4e3b44[object->definition_index & 0xffff].bytes;
        if (!(definition_bytes[0x1c] & 2))
        {
            s_component_transform_definition *definition = (s_component_transform_definition *)definition_bytes;
            s_component_collision_model_link *model = (s_component_collision_model_link *)g_4e3b44[definition->model_index & 0xffff].bytes;
            s_component_bounds_model *physics = (s_component_bounds_model *)g_4e3b44[model->physics_model_index & 0xffff].bytes;
            s_component_bounds combined;
            for (long i = 0; i < component->rigid_bodies.size; i++)
            {
                long node_count = component->rigid_bodies.data[i].node_count;
                s_havok_component_rigid_body *body = &component->rigid_bodies.data[i];
                void *shape = (byte *)body->rigid_body + 0xc;
                real padding = 0.0f;
                for (long j = 0; j < node_count; j++)
                {
                    s_component_bounds_body *node_body = &physics->bodies[body->nodes[j]];
                    padding = padding > node_body->padding ? padding : node_body->padding;
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
            }
            __m128 middle = _mm_mul_ps(component_bounds_splat(0.5f), _mm_add_ps(combined.lower, combined.upper));
            __m128 extent = _mm_mul_ps(component_bounds_splat(0.5f), _mm_sub_ps(combined.upper, combined.lower));
            center->x = middle.m128_f32[0];
            center->y = middle.m128_f32[1];
            center->z = middle.m128_f32[2];
            __m128 square = _mm_mul_ps(extent, extent);
            __m128 sum = _mm_add_ss(_mm_shuffle_ps(square, square, 0xaa), _mm_add_ss(_mm_shuffle_ps(square, square, 0x55), square));
            __m128 length = _mm_sqrt_ss(sum);
            real value;
            _mm_store_ss(&value, length);
            *radius = value + 0.001f;
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
        entity->entity_property_erase(HAVOK_PROPERTY_2003);
    entity->entity_property_put(HAVOK_PROPERTY_2003, s_havok_property_word(value));
}

/* clears a vector; function_1d87b0 stores through this inline function:
   written in place, its store keeps function_181db0's result pointer off ebx
   (scores 69 for 0x181db0 and 211 for 0x1d87b0 that way) */
PRIVATE __forceinline void component_surface_vector_clear(hkVector4 *vector)
{
    vector->m_quad = _mm_setzero_ps();
}
// score 91 (was 91): aligned frame 0x34 agrees; the contact loop and the normal block are retail's.
// Left: velocity j/k loaded before its length test (retail reloads them into xmm2/xmm3 ahead of the
// branch and scales them in place, i through a copy of the inverse; ours copies the inverse for all
// three), the speed products the same way (retail scales j, k in place), and the property value 0 kept
// in EDI across the erase. At 109 the contact loop's normal sums took xmm4/xmm1/xmm3 (retail
// xmm3/xmm2/xmm4) and the normal block copied the inverse for all three (retail keeps it in xmm1 and
// scales j, k in place).
// Tried: velocity sums as running sum plus material vector 131 kept; speed sum declared before the
// velocity 127 kept; normal scaled j, k, i 109 kept (k, j, i 110; i, k, j 121; j, i, k 124; k, i, j 122;
// all in place 109); velocity scaled j, k, i as well 109; inlined property setter passing the value as
// the 0x1cf3b0 setter does 216; shared dot input order 137; consistent inverse-first/inverse-last
// products 137/137; dot with either product form 137/137; mode snapshot 192; snapshot plus dot 192;
// sample default before matched query 208; complete contact input/normal sum/both 220/267/463; complete
// velocity sum 283; symmetric scaled components, separate speed product, material count after store and
// corrected-boundary speed-first each 137; fallback-first 150; separate normal/velocity/both
// normalization inputs 443/232/478; native flag byte, signed index, both fields, native bit, bit/index,
// Boolean bit, Boolean bit/index and readonly native contacts all 137; corrected-boundary compound sums
// 137; plain axis accumulation 184; byte material count 451; unsigned other count 137; corrected-boundary
// final shared scale 214; existing squared-length normal/sample/both/velocity/normal-velocity/
// sample-velocity/all boundaries 109 each; shared self-dot at all magnitudes 109; shared normal/velocity
// length 123; shared inverse 162; both 168; outer sample vector 109; vector plus length/inverse/both
// 123/162/168; all with one scan ordinal 168; setter taking the property word by value 173 (the zero
// goes to EBX before the search, retail EDI); velocity through the existing normalize3d 109; one vector
// local for both sums 109; compound velocity scale 109; normal declared first 113;
// normal/velocity reciprocal identity 138/170, both 184; early normal/velocity/both length zero 109 each;
// reciprocals and lengths 184, with early speed sum 392; successive normal/velocity sum workspace 109,
// with shared length/inverse/both 115/109/123, the same with a persistent sample 109/115/109/123; native
// contact/material/both add boundaries 157/162/210; reversed complete inputs 157/162/210; existing vector
// assignment at contact/material sums 144/114; complete normal/velocity/final scaled-product staging, all
// seven subsets, 109 each; staged outputs through the existing vector setters 127; magnitude pointer inline
// and vector-reference forced/ordinary inline 109/109/109; vector-value forced/ordinary inline 490/490;
// scalar-value forced/ordinary inline 193/193; scalar-reference input 109; shared assignment at
// normal/velocity/both normalization 127/109/127; shared scale there 127/109/127; fallback assignment 355;
// normal plus fallback assignment 355. Round 89: retail's velocity j/k loads are hoisted ahead of the
// length (both arms need them in registers) where ours folds them into `inverse * [j]` inside the branch;
// its three inline property updates look like inlined copies of the matched 0x1cf3b0 setter (out of line
// on the cold no-material path). The existing 0x1cf3b0 setter at every site 355 (not inlined), with an
// `inline` declaration 436 (the cold call inlined too); velocity normalised into a separate direction vector
// with a struct/per-field copy in the else arm 204/149; plain `inline` property helper 109; typed surface
// velocity field 109 (kept, same code).
// Round 93 (lane 8): the contact sum through vector3f_add(&contact->normal, &normal, &normal) and the
// normal scaled through vector3f_scale(&normal, 1 / length, &normal) 109 -> 91 (kept, with the velocity's
// hand-written length/scale block as normalize3d(&velocity), the same code); either alone 157/127. On
// 109: velocity scale through vector3f_scale or `*=` 109/109; normal `*=` 127; the normal inverse declared
// before the loop 109; one inverse shared by both blocks 162 (1454 bytes; velocity's three loads hoisted
// like retail's j/k, but i too), with velocity and speed declared at the top as well 162; inverse or
// inverse plus speed declared after velocity 109/109; speed declared before the velocity loop 109;
// both sums through vector3f_add 210, plus helper scales 205/139 (the velocity length sums i first);
// velocity sum and scale through helpers 157. On 91: velocity sum through vector3f_add either order
// 396/396 (1426 bytes), with normalize3d or vector3f_scale 139/139; the result scaled into velocity then
// set 159; velocity declared at the top 407; speed summed and averaged in one `speed` local 138 (the
// speed products all in place), with velocity declared before it 142; velocity declared before
// speed_sum 95; both 413. Round 131 (lane 4) on 91: the setter taking the property word by value
// 155 (1467 bytes; the zero goes to EBX before the search, retail EDI), by const reference 155;
// speed computed before the velocity normalization 430 (1403 bytes); one setter call in the
// no-contact arm with a conditional value 267 (1371 bytes); the contact test inverted 207 (1498
// bytes). The matched normal block shows the shape retail's velocity block needs (i scaled through
// a copy of the inverse, j and k in place in registers loaded before the branch).
// Round 134 (m3 lane 4) on 91: the inline property setter building its property word before the search
// (`s_havok_property_word word(value);` first) 155 at 1467 bytes: the three setter sites then are
// retail's exactly (`xor edi, edi` before the search, `push edi` after the erase, shared tail), but with
// the velocity block still 5 bytes long the loop padding shifts; worth retrying once the velocity block
// fits. The velocity normalized through the normal block's length local and vector3f_scale 179; the
// existing normalize_inline 175 (x87 order k, i, j); a normalized copy of the velocity (diagnostic) 211
// (1474 bytes); compound `+=` velocity sums 97. All restored.
// Round 37 (lane 10, fast): retail's velocity scale and its speed scale both read as (s * i, j * s,
// k * s); no existing helper has that order. The velocity through the existing normalize_inline 175
// (its length order differs, first difference +0x2d5); through the function's `length` local and
// vector3f_scale like the normal block 179; the inline setter passing its value through its address
// (`*(s_havok_property_word const *)&value`, as the out-of-line setter does) 163 (1467 bytes); the speed
// sum before the velocity sums 91 (same code);
// the result written through `body->unknown30` with no pointer local 99.
// Round 464: success-boundary velocity/speed/other-contact default subsets
// 339/192/305/284/412/337/411; velocity and speed at material admission with
// late other-contact zero 411. Original default lifetimes and helpers restored.
// Round 60 (lane 16): a compile-only screen reproduces our velocity block exactly; over ~60 forms (four
// loop-sum forms, normalize3d / own length + vector3f_scale / an inverse shared with the normal block,
// three product orders, three else arms, three length expressions, declaration order, scaled set) only
// the shared inverse hoists the velocity loads, and then all three (retail: j and k only). LTCG: the
// setter building its word first 155 (aligned 25 -> 22: only the velocity block and loop padding
// remain); the shared inverse with an own velocity length 199 (the normal block changes from +0x53),
// with the word-first setter 192; vector3f_add velocity sums + shared inverse + word-first setter 404,
// with the length as sqrt(dot3f) 404.
// Round 14 (lane 17, fast): the length kept from normalize3d in a named local 91; the speed products'
// operand order (v * s for all three, s * v for all three) 91/91; the material count cast to real 91;
// the result pointer declared before the normalization 263 (1403 bytes). Diagnostics (written-out
// normalization, not kept): the scale as (s * i, j * s, k * s) 91 (identical code: operand order is
// not the lever), the length summed k, j, i with the scale k, j, i 99. Retail loads the velocity's j and
// k into xmm3/xmm2 at the length's x87 loads and uses those same values in both arms (in-place scale,
// unscaled else), reloading only i; ours reloads all three in each arm.
// @retail 0x1d87b0
void __stdcall function_1d87b0(s_havok_component_rigid_body *body, long body_index, s_havok_component *component)
{
    long material_count = 0;
    long other_contacts = 0;
    vector3f normal = *g_4687a4;
    long materials[3];
    long contact_count = component->unknown88.size;
    for (long i = 0; i < contact_count; ++i)
    {
        s_havok_component_element48 *contact = &component->unknown88.data[i];
        if (contact->rigid_body_index_a == body_index && (((byte *)contact)[0x44] & 0x20) && material_count < 3)
        {
            s_component_surface *surface = &((s_component_surface_mesh *)g_4e0340)->surfaces[*(long *)((byte *)contact + 0xc)];
            bool found = false;
            for (long j = 0; j < material_count; ++j)
            {
                if (surface->material == materials[j])
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                vector3f_add(&contact->normal, &normal, &normal);
                materials[material_count++] = surface->material;
            }
        }
    }
    if (material_count == 0)
    {
        havok_entity_property_2003_set((hkEntity *)body->rigid_body, 0);
        component_surface_vector_clear(&body->unknown30);
        body->unknown44 = 0;
        return;
    }
    real length = component_surface_length(&normal);
    if (!(fabs(length) < 0.0001f))
    {
        vector3f_scale(&normal, 1.0f / length, &normal);
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
    real speed_sum = 0.0f;
    vector3f velocity = *g_4687a4;
    for (long i = 0; i < material_count; ++i)
    {
        vector3f local_c7c2ff;
        if (!function_181db0(materials[i], &local_c7c2ff))
            local_c7c2ff = *g_4687a4;
        velocity.i = velocity.i + local_c7c2ff.i;
        velocity.j = velocity.j + local_c7c2ff.j;
        velocity.k = velocity.k + local_c7c2ff.k;
        speed_sum += component_surface_length(&local_c7c2ff);
    }
    normalize3d(&velocity);
    real speed = ((bool)((component->unknown04 >> 18) & 1) ? 1.0f : 2.5f) * speed_sum / material_count;
    hkVector4 *result = &body->unknown30;
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

/* moves the last element into the removed one's place */
static inline void array_remove_at_1d06c0(s_havok_array48 *array, long index)
{
    array->size--;
    array->data[index] = array->data[array->size];
}

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
    array_remove_at_1d06c0(&component->unknown88, removed_index);
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
    void const *const volatile *contact_data_reference = &contact_data;
    bool room = component->unknown88.size < 28;
    long result = NONE;
    if (!room && ((1 << ((s_component_transform_header *)g_4e0300->data)[component->object_index & 0xffff].type) & 1))
    {
        real minimum = 3.402823466e+38f;
        long remove_index = NONE;
        for (long i = 0; i < component->unknown88.size; i++)
        {
            s_havok_component_element48 *contact = &component->unknown88.data[i];

            if (contact->normal.k < minimum)
            {
                minimum = contact->normal.k;
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
        long current = node_index;
        s_component_node_parent *node = &physics->nodes[current];
        do
        {
            for (long i = 0; i < physics->body_count; i++)
            {
                if (physics->bodies[i].node == current)
                {
                    body = i;
                    break;
                }
            }
            current = node->parent;
        } while (node->parent != NONE && body == NONE);
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

// BLOCKED-BY match cost: enabling this body loses 10 matched functions that neither call it nor
// are called by it (0x82980, 0xbfa40, 0xebaa0, 0xec940, 0xec970, 0x141f60, 0x142360, 0x1424f0,
// 0x16c4f0, 0x16c5f0; maintainer's full check on #566), so `@stub 0x1d3920` in src/stubs/lane_s.cpp
// stays active. The body behaves like retail on 6/6 in-game snapshots (fecheck): retail keeps all
// three arguments on the stack (ret 0xc) and its callers 0xbc8dc and 0xbd1a7 push them.
#if 0
/* views of s_component_node_object and s_component_node_render_model with the fields
   this function reads named (same layouts) */
struct s_component_node_object_1d3920
{
    long definition_index;
    byte unknown04[0x110 - 4];
    short unknown110;
    short orientations_offset;
    byte unknown114[2];
    short matrices_offset;
};

struct s_component_node_render_model_1d3920
{
    byte unknown00[0x48];
    long node_count;
    s_component_node_default *nodes;
};

/* Moves the component's driven nodes onto their rigid bodies (raised by `offset`), marks
   them in `updated` and rebuilds the rest of the hierarchy from the moved nodes. */
// Retail 0x1d3920, disabled with its blocker above.
void __stdcall function_1d3920(byte *component_bytes, dword *updated, real offset)
{
    /* retail keeps all three arguments on the stack (ret 0xc); its callers push them */
    byte *const *component_reference = &component_bytes;
    dword *const *updated_reference = &updated;
    real const *offset_reference = &offset;
    s_havok_component *component = (s_havok_component *)*component_reference;
    if (TEST_FIELD_BIT(component->flag9) || component->rigid_bodies.size <= 0)
        return;
    s_component_node_object_1d3920 *object = (s_component_node_object_1d3920 *)havok_object_get(component->object_index);
    s_component_transform_definition *definition = (s_component_transform_definition *)g_4e3b44[object->definition_index & 0xffff].bytes;
    s_component_transform_model *model = (s_component_transform_model *)g_4e3b44[definition->model_index & 0xffff].bytes;
    s_component_node_body_model *physics = (s_component_node_body_model *)g_4e3b44[
        ((s_component_collision_model_link *)model)->physics_model_index & 0xffff].bytes;
    s_component_node_render_model_1d3920 *render_model = (s_component_node_render_model_1d3920 *)g_4e3b44[model->render_model_index & 0xffff].bytes;
    dword moved[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    long unknown = 0;
    rigid_transform_scaled *orientations = NULL;
    bool changed = false;
    if (object->orientations_offset != NONE)
    {
        unknown = (dword)object->unknown110 >> 5;
        orientations = (rigid_transform_scaled *)((byte *)object + object->orientations_offset);
    }
    for (long i = 0; i < component->rigid_bodies.size; i++)
    {
        s_component_node_body_link *first = &physics->bodies[component->rigid_bodies.data[i].nodes[0]];
        for (long j = 0; j < component->rigid_bodies.data[i].node_count; j++)
        {
            s_component_node_body_link *link = &physics->bodies[component->rigid_bodies.data[i].nodes[j]];
            if (link->node == NONE || havok_component_rigid_body_get(i, component)->m_motion->kind_code() == 6)
                continue;
            long node = link->node;
            if ((*updated_reference)[node >> 5] & (1 << (node & 31)))
                continue;
            /* a node below the body's first node follows that node */
            long ancestor = node;
            while (ancestor != NONE && ancestor != first->node)
                ancestor = physics->nodes[ancestor].parent;
            if (ancestor != NONE && link->node != first->node)
                continue;
            long parent = physics->nodes[node].parent;
            transform4x3f *matrix = &component_node_matrices(component->object_index)[node];
            component->rigid_body_matrix_get(i, matrix);
            matrix->position.z += *offset_reference;
            if (link->node != first->node)
            {
                hkVector4 buffer[16];
                transform4x3f frame_matrix;
                c_component_child_frames *shape = *(c_component_child_frames **)((byte *)component->rigid_bodies.data[i].rigid_body + 0xc);
                s_component_child_frame *frame = shape->get_frame(j, buffer);
                component_frame_matrix(&frame->transform, &frame_matrix);
                function_142a60(matrix, &frame_matrix, matrix);
            }
            (*updated_reference)[link->node >> 5] |= 1 << (link->node & 31);
            changed = true;
            moved[link->node >> 5] |= 1 << (link->node & 31);
            for (; parent != NONE; parent = physics->nodes[parent].parent)
                moved[parent >> 5] |= 1 << (parent & 31);
        }
    }
    if (changed && render_model->node_count > 0)
        component->function_1d3d20(0, *updated_reference, moved, orientations, unknown);
}
#endif

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
        s_component_node_body_link *link = &physics->bodies[body->nodes[i]];
        if (link->node == node_a && child_a == NONE)
            child_a = i;
        if (link->node == node_b && child_b == NONE)
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
        inverse.transform_set_to_inverse(frame_a->transform);
        relative.compose((s_extent_transform const *)&frame_b->transform, (s_extent_transform const *)&inverse);
        component_frame_matrix((hkTransform const *)&relative, matrix);
    }
}


#include "unknown_1eb550.h"
#include "vector_products.h"

void function_bba20(long object_index);
void function_f7ed0(long vehicle_index, vector3f *impulse);

/* caps a vector's length at maximum; true when it was longer */
PRIVATE __forceinline bool velocity_limit_length(vector3f *v, real maximum)
{
    real length_squared = v->k * v->k;
    length_squared += v->j * v->j;
    length_squared += v->i * v->i;
    bool limited = length_squared > maximum * maximum;
    if (limited)
        scale_vector_by_double(v, maximum / sqrt(length_squared), v);
    return limited;
}

// score 7 (was 7): the saves are staged as retail's; the five rigid-body reads address
// [offset + data] where retail has [data + offset], and the 0x1d1010 call copies the component to ESI
// before the index to ECX. The entry saves came from this function's own double-typed scale: with a
// named double here, any image holding 0x2056e0's double cap or unknown_0350e0.cpp pins the saves at
// entry. Round 17: both caps through the existing scale_vector_by_double 50; real scales 77 (folds the
// last multiply); quotient per component 77; the two-caller cap helper with the quotient as
// scale_vector_by_double's argument 7 (kept), with a named double scale 50; operator[] reads at the
// first two sites 7; negated change product 7; existing keyframed helper at both/the last test 151/29.
// Round 31: a compile-only build of this file reproduces the [offset + data] order; it turns to
// retail's [data + offset] at all sites only without the inline kind test at the loop head (or with
// the kind test as the out-of-line keyframed method); pre-loop code, the last block, loop/index forms,
// inline test helpers and a short index (no strength reduction) never flip it. Real trials: keyframed
// method/free wrapper at the first test 156/156; separate inline linear/angular getters 128; while
// loop 7; a body pointer for the test and the velocity read 152.
// Round 33: keyframed method marked inline at both tests, as member or free wrapper, 7/7 (same
// code); forceinline at both/the last test 7/7 (0x2273d0 and 0x1c4b00 lose); the matched siblings'
// index declared before the loop 7. Compile-only screens: the order turns to retail's only when the
// loop head reads the array's size field before the first rigid body; getter parameter order, a
// member function with `this`, direct data[] reads and index names never move it.
// Earlier rounds (the full image then pushed ebx/ebp/esi at entry):
// Earlier: Frame 0x30 agrees; admission save staging and paired fixed/zero velocity homes remain.
// staggered register saves and paired velocity homes differ;
// marked linear/angular/both getters 200/197/274; earlier angular norm 146; keyframe query 167 (fast base 135).
// Force-inline linear/angular/both getter declarations 175 each; initialized linear/angular outputs 199 each;
// initialized pair 219; initialized pair with getter declarations 219; live external array receiver 175
// (plain scores; reduced preview 135). Staggered saves and paired getter allocation still differ.
// Staggered saves and paired velocity homes remain; reversed getter
// order 178; linear norm before angular read 176; angular read/norm before linear read 186;
// native simulation result 180; post-update body receiver 187 (plain). Opening body retained.
// staggered saves and paired velocity homes remain;
// linear / angular / both component loops 175/175/175; horizontal initialization 173;
// linear / angular / both loops with horizontal initialization 173/173/175;
// direct component pointers 183 (plain; original double products and component order retained).
// Round 358: Paired named magnitudes 135; direct shared scale outputs 175; compound clamp components 135;
// original-order real accumulations 128 reduced, 168 plain; nested post-update guards 135 (control 135). Keep
// the seven-point full-image accumulation gain at retail length; frame 0x30, staggered saves and paired
// getter homes remain.
// Round 224: Iteration norm/vector/both declarations 128/128/128; inverse velocity/gravity lexical
// homes 128/128; complete immutable norms 133; chained horizontal stores 130; joint homes/norms/chain
// 135 (fast control 128, full 168). Save staging and paired fixed/zero homes remain.
// Round 323 (reduced control 128): common norm 139; common scale 128; real limit
// literals 128; common norm/scale 139; norm/limits 139; scale/limits 128; all 139;
// all with body-index name 139. Full-image save staging and paired getter homes remain.
// Round 71: the full image keeps the three saves at entry only when src/unknown_0350e0.cpp is linked in
// (any reduced image without it sinks them as retail does); float numerators 100.0f/50.0f give retail's
// fdivr dword 168 (tie); direct array / pointer-to-array rigid body reads 128f; index declared before the
// loop at function/block top 128f (address order stays [offset + data], retail [data + offset]).
// Round 137: active-query bool/byte/short/long on nested admission 168,168,168,168;
// on guarded exit 168,168,168,168 (controlled reduced scores).
// Round 332 (reduced control 128): active/notification/impulse inline boundaries and
// all their combinations 128 each; state/change inline boundaries 132/678. All restored.
// Joint active/notification/impulse inline boundaries plain 168/677; restored.
// Round 345 (controlled preview 168): complete linear/angular/both squared terms
// 173/168/173; iteration terms/paired first sums/immutable terms/shared terms/shared first sums
// 173/173/173/173/173. Full-image save staging and getter homes remain.
// Round 152: Shared angular inline/force/out-of-line declarations 168/168/168; shared angular call with those contracts 177/177/236; shared pair with angular force/inline 238/238. Forced forms lose a matched caller; all declarations and body restored.
// Round 147: admission mask 161; state masks 135; both masks 194; vertical product 168;
// admission/product 161; state/product 135; both/product 194; guarded type mask 168; state zero
// tests 135. Keep both native state zero tests; every other form restored.
// Round 197: Readonly private linear/angular/both getter inputs 135 each; native signed-int
// ordinal alone/with each input pair 135 each; readonly pair with common double scale 135.
// Original native state-zero graph, helpers and ordinal restored.
// Round 201: Private linear/angular/both per-use body receivers 135/135/196; readonly pair 135;
// mixed per-use/readonly receivers 135/135; fixed-first pair 151 and per-use fixed-first pair 202
// (both fixed-first forms lose matched users). Original helpers and body restored; frame/save homes remain.
// Round 63: kept the loop's bit-2 test as TEST_FIELD_BIT(flag2) (retail's shr/test) 135 -> 114 and the
// float numerators 100.0f/50.0f (retail's fdivr dword, aligned -2) 114; the first bit-2 test as the
// bitfield too (retail's form there as well) 168, alone 174, while the entry saves are staged
// differently (reduced image with both: aligned 37). Retail reloads g_4687a4 after the linear copy's
// stores, as if its address were taken somewhere; ours never takes it. The change vector as a brace
// initializer 114; its zeros chained 114. Round 103 (fast, reduced 92): nested entry tests 92; the
// kind test as an empty then-arm 92; the type mask as (1 << 1) 92; the flag test as !(x & 4) 92; the
// change's k component first 99. No function of the linked file is on this function's direct call chain
// in our image, and its frame-aligning callees align themselves, so the entry staging is not a callee's
// stack-alignment request.
// Round 150 (reduced scores marked f): both velocities through one helper on one body pointer, the
// matched 0x1cefb0's own sequence (retail's single body read, the flag in DL and the zero-vector
// reload on the fixed path) with the first bit-2 test as TEST_FIELD_BIT 50 (7f; kept, 0x1cefb0 uses
// the helper and still matches; the flag as a local at both tests 158f and loses 0x1cefb0); the entry tests as two early returns 50; loop index
// declared at block top as 0x1d1c10 7f; preincrement 7f; count local 160f; const component 7f;
// direct data[i] reads at the first site/all sites 7f/10f; a body pointer after the state update 7f.
// Retail's call graph from here holds 25 functions and no cycle.
// Round 195: Native final vector outputs linear/angular/both [241, 244, 314]; paired result before
// receiver 314; native component-set output linear/angular/both [241, 230, 205]; paired early component
// result 205. Original private getters restored; staged saves and fixed/zero homes remain.
// Round 29 (controlled full-image scores; reduced body 7): existing vector constructor 50;
// two-use change pointer 50; reversed velocity declarations 50; guarded do-while 53;
// reversed loop comparison 52; preincrement 50; outer-block index 50; per-iteration body index 50.
// Original body restored; entry saves, five address forms and the final argument moves remain.
// Round 71 (reduced scores): 0x1d1010 declared (component, index, change) with its callers 5f (retail's
// ECX-then-ESI setup at the call) but 0x1d1010 itself then reads [data + index] (1; callers in
// unknown_206090.cpp and unknown_1c25a0.cpp were held this round); with an array pointer in the
// velocity helper, 0x1d1010 still 1; array pointer at all five / the three late reads 10f/10f;
// separate component-first velocity helpers 128f. The address order stays [index + data].
// Round 383: retained pre-clamp body/owner handle/native repeated array receivers 190/190/53;
// body+owner/body+array/owner+array/all/all with plain while 193/189/190/192/192 (controlled
// preview 50). Original body, paired getter, observation graph and all interfaces restored.
// Round 244: Paired-getter graph with shared scale linear/angular/both 50/50/50; common scale workspace
// alone/with linear/angular/both 50/50/50/50; shared pair and single admission exit 50. Staged saves, five
// address orders and final call moves remain; original source restored.
// Round 183: target entry inline/noinline 50/50; state-update inline alone/with entry
// inline/noinline 50/50/50; state-update noinline alone/with entry inline/noinline 50/50/50.
// Paired getter unchanged; all contracts restored.
// Round 47: array operator[] at all five/first-only/late-three sites 10/7/10; array reference for the
// bound/all reads 7/10; size reference 7; shared data pointer 171; final call scope 7. Original body restored.
// Round 37 (lane 10): the final call's setup needs 0x1d1010 as (component, index, change) (7 -> 5 in round
// 15, not kept: 0x1d24a0 920 -> 966), whose call lines include contributor function 0xe0ef0 (BLOCKED-BY
// 0xe0ef0). The [offset + data] order never flips in the full build when the loop shrinks: without the
// angular cap 333 (492 bytes), without the head kind test 171 (653), without the last change block 185
// (600), without the pre-loop impulse block 227 (628), without the cap and the last block 425 (410); all
// keep [offset + data] (the reduced-harness size lead does not carry over). Plain compile-only builds keep
// it for direct data[i] reads and an explicit `data + i * 0x60` at all five sites as well. Candidate forms:
// the index declared before the change vector 7; the velocity pair declared before the head kind test 7;
// `angular, linear` declared in that order 7; the entry tests as one early return 7; the velocities through
// the existing out-of-line linear/angular getters 274 (551 bytes, calls kept; the head read then turns to
// [data + offset]).
// Round 15 (m3 lane 2): 0x1d1010 declared (component, index, change), as every retail call sets the
// component register last, 7 -> 5, but 0x1d24a0 920 -> 966 (and 0x1d1010 then needs 0x1d09d0's array-pointer
// getter form): not kept (coordinator). On 5: kind tests through an inline rigid-body method wrapping the
// virtual call 13; direct data[i] reads at all five sites 8; separate linear/angular inline getters 139;
// named cap results 5; index declared before the change vector 5; one body pointer for the head test
// and the velocity read 152. Compile-only: the order turns to
// retail's only without the head kind test or without the capped velocity block (or with only one capped
// velocity), so it follows the loop's size, not a spelling; 256 combinations of declaration, index,
// increment, direct-read and initializer forms and unused locals never flip it.
// Round 117 (fast; the matched 0x1d1c10 loop gives retail's [data + offset] with the same getter):
// the head kind code in a named local 7; a rigid-body local for the head test 8; the velocity pair
// declared with the change vector 7; the 0xbba20 call before the change vector 22; the index named
// rigid_body_index as in the siblings 7.
// Round 66: reversed subscript at all five reads 10; data+index and index+data pointer reads 10 each;
// direct data reads at the first two 7, the late three 10, the late kind/fixed pair 7; explicit array
// operator calls 10; one shared element reference 156 (680 bytes). Opening body restored.
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
            if (havok_component_rigid_body_get(i, component)->m_motion->kind_code() != 6)
            {
                vector3f linear, angular;
                rigid_body_velocities_get(havok_component_rigid_body_get(i, component), &linear, &angular);
                if (velocity_limit_length(&linear, 100.0f))
                    havok_component_rigid_body_linear_velocity_set(i, component, &linear);
                if (velocity_limit_length(&angular, 50.0f))
                    havok_component_rigid_body_angular_velocity_set(i, component, &angular);
            }
            havok_component_rigid_body_state_update(i, component);
            if (havok_component_rigid_body_get(i, component)->body_is_simulating().m_bool &&
                !TEST_FIELD_BIT(component->flag2) &&
                havok_component_rigid_body_get(i, component)->m_motion->kind_code() != 6 &&
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

// MiniMax D (11-01): no new form tried, no lower score; the round-41 merge rewrote this body,
// so its score is now 346 against the 206 the task started from.
// score 206 (was 206): Frame 0xb4 and extent agree; body base/stride and protected relative-call
// input registers remain. BLOCKED-BY 0x1d83e0 (contributor code): retail passes the component to it
// in EAX and loads the component into EAX at entry; ours passes it in ECX, and the EAX/ECX roles
// swap from the first body (+0x3d, still the first difference in round 68; the reduced build of the
// fast check gives another convention, 346, so only the plain check scores this function).
// Tried: body getters 206; first count after lookup 205 (below gate), both 275; node streams 336;
// streams with getters 336; count names 206; search names 206; transformed name 206; entry phase
// transforms 301; equal-node arm first 248; second selector word 226; promoted first nodes 290; entry
// matrix 206; entry relative transform 206; fallback first 206; first mapped byte 299; second mapped
// byte 213; first node at use 338; second node at use 226; loop-scoped child counters 206; late first
// link 370; late second 309; array accessor 206; mapped-index names 206; second count after lookup
// 224; reused first body 206; word first mapped index 220; word second mapped index 212; array header
// pointer 336; per-phase model pointers 209; reduced search operand order first/second/both
// 346/346/346; frame-test operands first/second/both 346/346/346; first mapped-index result 345, with
// search operands 345 (control 346; one-point reductions excluded); existing frame converter with
// matrix/output/both references 206/206/206; direct translation, with matrix/output/both references
// and actual output return 367/367/367/367/367; output-first converter, with matrix reference,
// output pointer return, and output-first return 206/206/206/206; split rotation/translation,
// output-first split, split return and output-first split return 206/206/206/206; shared sequential
// body index/count/first-link workspaces 299/208/302; index+count/index+link/count+link/all/all with
// child ordinal 217/213/302/300/300; consistent requested-node snapshots first/second/both
// 367/254/400; count fields at every use first/second/both 316/337/358; corresponding
// snapshot/bound pairs and all 369/369/419; index through if/else 313; a body-link table local 209;
// the child search as a while with the compound test 206 (same code); the second index assigned in
// plain statements (no embedded assignment, same code, kept) 206; the child counter declared first 208.
// Round 241: native requested-node shorts first/second/both 338/226/354; independent
// readonly model snapshots first/second 221/321; both snapshots with first/second/both
// shorts 267/325/283. Original frame, body and protected relative-call contract restored.
// First link before count first/second/both 205/224/275; loop link locals first/second 206/206;
// converter position declared first 206; translation read before rotations 254; converter force-inline 206 (plain).
// Round 33: Native transform-value converter input, with pointer/reference output, output order
// and actual output-address return combinations: 321/321/321/321/321/321/321/321. Original converter/body restored.
// Round 233: Call-free native body records first/second/both, then each with
// readonly first links, both with first-link/count order and both refinements:
// 317/203/332/317/203/332/374/374. Original records, output reloads and protected call tuple restored.
// Round 182: first/second/both actual link references 206/206/206; native body
// record references confined to count/node searches, alone/with either/both links
// and mutable body records, 332/332/332/332/332. Original records and body restored.
// Round 89: blocker confirmed in the full build: retail loads the component into EAX before each 0x1d83e0
// call and 0x1d83e0 reads it from EAX at entry; ours uses ECX on both sides.
// @retail 0x1d7ef0
void function_1d7ef0(hkTransform *frame_a, s_havok_component *component,
    s_component_frame_owner *owner, s_component_frame_map const *mapping, hkTransform *frame_b,
    hkRigidBody **body_a, hkRigidBody **body_b, s_component_frame_pair const *pair)
{
    long node_a = pair->node_a;
    long index_a = node_a == NONE ? mapping->fallback : mapping->bodies[node_a];
    long node_count = component->rigid_bodies.data[index_a].node_count;
    s_component_node_body_link *first = &owner->physics->bodies[component->rigid_bodies.data[index_a].nodes[0]];
    long child = 0;
    for (; child < node_count; child++)
    {
        if (owner->physics->bodies[component->rigid_bodies.data[index_a].nodes[child]].node == pair->node_a)
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
    long index_b = NONE;
    if (pair->node_b != NONE)
        index_b = node_b == NONE ? mapping->fallback : mapping->bodies[node_b];
    if (index_b != NONE)
    {
        long other_node_count = component->rigid_bodies.data[index_b].node_count;
        s_component_node_body_link *other_first = &owner->physics->bodies[component->rigid_bodies.data[index_b].nodes[0]];
        long other_child = 0;
        for (; other_child < other_node_count; other_child++)
        {
            if (owner->physics->bodies[component->rigid_bodies.data[index_b].nodes[other_child]].node == pair->node_b)
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

struct s_1cf531
{
 word field_0 : 11;
 word field_1 : 1;
 word field_2 : 4;
};

// score 27 (was 27): the friction magnitude takes another dead slot (retail shares the friction pointer's
// spill slot), the rigid-body element is formed offset-first in retail (data + offset, ours offset + data),
// retail loads the vector's y early and stores it after z, and the scratch registers rotate from there.
// Tried: the element through the array's operator[] 27; the vector through _mm_setr_ps or _mm_set_ps 104;
// the magnitude in a second local for the finiteness result 27; the bits read as long 27; the magnitude
// renamed either side of the other locals 27/27; the finiteness test through a by-value bit helper 25 (the
// magnitude then shares the friction pointer's slot, as in retail; a new one-caller helper, not kept). With
// that helper: element fields read at each use 105; the rigid body read before the vector 25; the corrected
// vector updated in place in the impulse 44; impulse/difference vectors renamed 25/25; `index + data` 25
// (same code); the 0.6 scale declared before the vector 25. Typed field at +0x30 and named flag18 bit
// (kept, same code). Still differ: (B) element address data + offset in retail; (C) y stored after z;
// (D) the corrected vector spills to 0x30 in retail (ours 0x40).
// Round 139: the corrected vector computed in place in the impulse vector 46 (908 bytes; the store
// goes to retail's 0x30, but the final product loads the impulse into a register where retail folds
// a spill reload as its memory operand); that with the final product impulse-first 43; the impulse
// as an __m128 with the product in a new vector 107 (frame 0x64, the copy forwarded from the scaled
// vector), also with the copy in its own block 107; the surface components in named x/y/z locals
// 27; the surface vector through havok_from_vector3d 27.
// @retail 0x1cf520
void function_1cf520(s_1cf520 *arg_1, c_1cf520 *arg_0, bool arg_2, bool arg_3, long arg_4, long arg_5)
{
 s_1cf521 *local_0 = arg_0->function_1cf526(arg_1->field_0);
 s_havok_friction *local_1 = arg_0->function_1cf525(arg_1->field_0);
 s_havok_component *local_2 = havok_component_get(arg_4);
 arg_1->field_34 = local_0->field_0(3);
 function_1cf529(&local_0->field_0, (real *)&arg_1->field_1c);
 function_1cf529(&local_0->field_10, (real *)&arg_1->field_28);
 local_2->rigid_bodies.data[arg_5].rigid_body->m_motion->kind_code();
 hkRigidBody *body = local_2->rigid_bodies.data[arg_5].rigid_body;
 real local_3 = 1.0f > body->m_motion->getMass()
  ? 1.0f : body->m_motion->getMass();
 real local_4 = *(real *)local_1 >= 0.0f ? *(real *)local_1 : 0.0f - *(real *)local_1;
 if ((*(dword *)&local_4 & 0x7f800000) == 0x7f800000) local_4 = 0.0f;
 arg_1->field_38 = local_4 / local_3;
 if (arg_3)
  vector3f_scale(&arg_1->field_28, -1.0f, &arg_1->field_28);
 else havok_component_friction_get(local_1, arg_4, (s_havok_material *)arg_1);
 if (arg_1->field_38 > 0.4f || (arg_1->field_44 & 8) || function_1cf528(local_2))
 {
  byte *local_5 = *(byte **)(g_4e0300->data + (local_2->object_index & 0xffff) * 12 + 8);
  s_1cf531 *local_24 = (s_1cf531 *)(local_5 + 0xc0);
  local_24->field_1 = true;
 }
 if (!arg_2 && (arg_1->field_44 & 0x20))
 {
  s_havok_component_rigid_body *local_6 = &local_2->rigid_bodies.data[arg_1->field_45];
  if (local_6->unknown44 == 2 && !TEST_FIELD_BIT(local_2->flag18))
  {
   hkVector4 local_7;
   local_7.set(local_6->unknown30(0), local_6->unknown30(1), local_6->unknown30(2));
   hkRigidBody *local_9 = local_6->rigid_body;
   __m128 scale = _mm_set_ss(0.6f);
   local_7.m_quad = _mm_mul_ps(_mm_shuffle_ps(scale, scale, 0), local_7.m_quad);
   __m128 local_11 = _mm_mul_ps(local_7.m_quad, local_7.m_quad);
   real local_12;
   _mm_store_ss(&local_12, function_1cf527(_mm_mul_ps(local_7.m_quad, local_7.m_quad)));
   if (local_12 > 0.0000010000001f)
   {
    hkVector4 local_13, local_14;
    local_13.m_quad = local_7.m_quad;
    local_9->m_motion->getPointVelocity(local_0->field_0, local_14);
    __m128 local_15 = function_1cf527(local_11);
    __m128 local_16 = _mm_rsqrt_ss(local_15);
    __m128 local_17 = _mm_sub_ss(_mm_set_ss(3.0f), _mm_mul_ss(_mm_mul_ss(local_15, local_16), local_16));
    local_17 = _mm_mul_ss(_mm_mul_ss(_mm_set_ss(0.5f), local_16), local_17);
    __m128 local_18 = _mm_mul_ps(_mm_shuffle_ps(local_17, local_17, 0), local_7.m_quad);
    __m128 local_19 = _mm_mul_ps(local_14.m_quad, local_18);
    real speed;
    _mm_store_ss(&speed, function_1cf527(local_19));
    __m128 along = _mm_set_ss(speed);
    __m128 corrected = _mm_sub_ps(local_13.m_quad, _mm_mul_ps(_mm_shuffle_ps(along, along, 0), local_18));
    real local_20 = ((s_scale_owner *)local_9)->get_inverse_scale() * 0.2f;
    __m128 factor = _mm_set_ss(local_20);
    local_13.m_quad = _mm_mul_ps(_mm_shuffle_ps(factor, factor, 0), corrected);
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
 long arg_3, long arg_4, bool arg_5, bool arg_6);

// @retail 0x1d6ca0
void function_1d6ca0(s_havok_component *arg_0)
{
 byte *local_0 = ((s_1d56a0 *)g_4e0300->data)[arg_0->object_index & 0xffff].field_8;
 function_1d6d00(arg_0, g_4e3b44[*(long *)local_0 & 0xffff].bytes + 0xd4,
  local_0 + 0x17c, 0xf, 0xf, 0, local_0[0x17c] == 3);
}
