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
void havok_component_rigid_body_point_velocity_get(vector3f *velocity, long rigid_body_index, s_havok_component *component, point3f const *point)
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

// Matched in a full build with stand-in SDK headers (was 29): the body slot and its fixed flag are read through
// reference-bound locals.
// Before:
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
// Round 140 (c1C): the slot as a `hkRigidBody *volatile *` (diagnostic, not kept) gives retail's two separate body
// loads and the late reload, but the byte still goes to BL (`mov edx,[eax]; mov bl,[edx+0x40]`, push ebx) 46,
// 141 bytes: the byte register is not chosen by the base register dying, so source cannot reach DL here. Callers
// pass the same registers as retail (index EAX, velocity ECX), so their setup is not the cause.
// Permuter (25 min on #1044 round 2, about 1400 variants): nothing below 29.
// b13C (11-17) walk against retail, fast build on r846: logic matches (flag1 bit, +0x40 slot address kept in eax, body
// byte at +0x40, x/y/z/0 stores, activate inline, vtable +0x54 as the setter; 0x1d0dd0's +0x50 is the linear one), so
// only allocation differs. Callers 0x115b10 0x115c70 0x1c4b00 0x1d1b60 stay MATCH. New forms: early returns for
// flag1 and m_fixed with the slot pointer and the late reload: retail's two body loads and `mov esi,[eax]` at 0x1d0ea6
// but BL again (push ebx, sub esp 0x18, 141 bytes) 48; second body read through havok_component_rigid_body_get 53.
// Matched 0x1d0dd0 puts the byte in CL because the dead index register is free; here retail's EDX holds the dying
// base (0x1d0e74 -> 0x1d0e76), and every reloading form of ours treats EDX as busy, so BL is the remaining cause.
// M3 lane 2 r100 (fast with callers 0x115b10 0x115c70 0x1c4b00 0x1d1b60 0x1ced00; a one-file LTCG harness of this file
// gives the same code): the slot pointer read at each of its three uses, no body local, 39 (143 B, the third read is
// reloaded after the activation calls); the getter at each of the three uses 67 (155 B, callers lose); no own-parameter
// read, slot pointer, body local assigned after the conversion 32 (component in EDX, callers lose); 0x1d09d0's array
// pointer (`rigid_bodies->data[index]` at the test and after the conversion) 46 (141 B); a by-value conversion helper
// (harness 37: the vector is copied); a 60-form random harness screen (own-parameter read x early/nested exits x slot
// pointer/reference/element/array/getter x vector and body-local placement x set()/helper) best is the current form.
// Diagnostics (harness, reverted): stores through a laundered pointer (float- or pointer-typed) still leave the body
// load shared (no reload), and the fixed byte read as char or byte stays in BL under a volatile reload, so in our build
// the vector stores never kill the slot load: retail's reload after them needs a cause other than aliasing.
// M3 lane 9 r139 (fast with the callers above): as a component member (index, velocity; the siblings' form, measured
// through a temporary header bridge) 41 (this/index/velocity arrive in EDX/ECX/EAX, callers lose; two callers are
// contributor code); the fixed test through a new inline fixed-flag accessor 29 (same code), with the vector at
// function scope 48 (141 B, the listed reload code with the byte in BL); `(void)&component;` in place of the
// own-parameter read 32 (component in EDX, callers lose), with the vector at function scope 47 (139 B: the byte still
// goes to BL with the component in EDX, so BL does not follow the component's range). Diagnostic: a non-const
// `hkRigidBody **` slot with the body read after the conversion 29 (same code as this body).
// M3 lane 2 r106 (fast, and a one-file LTCG harness that gives 28 for this body): the slot as an element pointer
// (`slot->rigid_body` at the test and after the conversion) 31 (a dead `add eax, edx`, the load still shared). Harness:
// early returns for flag1 and m_fixed with the slot pointer and the body read after the conversion 46 (141 B: retail's
// reload with no volatile, the byte in BL), with 0-9 dead initialised locals 46 each (no symbol-count tie); the fixed
// test read directly or through the getter with the slot pointer formed after it 46/46; nested blocks with the test
// read directly 34 (135 B); the body for the test in its own const local, or the byte in a named bool, 46/46.
// @retail 0x1d0e50
void havok_component_rigid_body_angular_velocity_set(long rigid_body_index, s_havok_component *component, vector3f const *velocity)
{
	s_havok_component *const *local_0 = &component;
	component = *local_0;
	if (!TEST_FIELD_BIT(component->flag1))
	{
		hkRigidBody *const *local_1 = &component->rigid_bodies.data[rigid_body_index].rigid_body;
		hkRigidBody *const *const &kept_slot = local_1;
		bool fixed = (*kept_slot)->m_fixed;
		bool const &kept_fixed = fixed;
		if (!kept_fixed)
		{
			hkVector4 havok_velocity;
			hkRigidBody *rigid_body;

			havok_from_vector3d(&havok_velocity, velocity);
			rigid_body = *kept_slot;
			havok_rigid_body_activate(rigid_body);
			rigid_body->m_motion->setAngularVelocity(havok_velocity);
		}
	}
}

/* a member: its one caller (0x1c4c50) sets up the index (ECX) before the component (EDX), as for a member
   whose `this` comes first */
// @retail 0x1d10b0
void s_havok_component::rigid_body_linear_velocity_add(long rigid_body_index, vector3f const *velocity)
{
	hkRigidBody *rigid_body = havok_component_rigid_body_get(rigid_body_index, this);
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

/* Retail passes the transform in EAX, the index in ECX and the component in EBX (a plain `ret`). Link-time code
   generation gives this function that convention only while no out-of-line function calls it directly: its two
   callers, 0x1d0c20 and 0x1d5120, reach it through s_havok_component::rigid_body_transform_set, the inline member
   spelling in include/unknown_1cec30.h. With a direct call from either of them the component arrives on the stack. */
// MiniMax A (11-19): no new form, no lower score (65): the component still arrives on the stack, so the frame is still 0x84 against retail's 0x88 and the copy of the transform is still a block copy against retail's four aligned moves; the argument registers are still chosen by LTCG over the 0x1d0cf0 -> 0x1d1260 -> 0x278f00 cycle, whose bodies are in other files.
// MiniMax H (11-19): no new form, no lower score (65): the transform copy is still a block copy where retail stores the four members separately, and the component still arrives on the stack where retail hands it over in EBX; the frame stays 0x84 against retail's 0x88.
// MiniMax B (11-18): no new form, no lower score (65): the transform copy is still a block copy here where retail
// stores the four members separately, and the component still arrives on the stack where retail hands it over in EBX;
// the memberwise set form measures 90 (worse), and the component's home is decided across the
// 0x1d0cf0 -> 0x1d1260 -> 0x278f00 -> 0xb8540 call cycle, whose bodies are in other files: BLOCKED-BY 0x1d1260.
// MiniMax S (11-17): no new form, no lower score (65): the frame is 0x84 against retail's 0x88, the index and the component arrive on the stack where retail keeps them in ecx and ebx, and the entry copy is a block copy where retail stores the four members separately; the component's home is decided across the 0x1d0cf0 -> 0x1d1260 -> 0x278f00 -> 0xb8540 call cycle, whose bodies are in other files.
// MiniMax A (11-12): no new form, no lower score (65); the component still arrives on the stack
// where retail hands it over in EBX, so the frame is 0x84 against retail's 0x88 and the transform
// copy is still a rep movsd where retail uses four movaps; the component's home is decided across
// the 0x1d0cf0 -> 0x1d1260 -> 0x278f00 -> 0xb8540 call cycle, whose bodies are in other files.
// MiniMax S (11-11): no new form, no lower score (65): retail hands this function its component in ebx while this build keeps it on the stack, so the frame is 0x84 against retail's 0x88 and the transform copy is a rep movsd where retail uses four movaps.
// MiniMax I (11-10): no new form, no lower score (65); the component and the index still arrive on the
// stack where retail passes the transform in EAX, the index in ESI and the component in EBX and returns
// with a plain ret, and the transform copy still uses rep movsd in a 0x84 frame where retail copies with
// four movaps in 0x88; the component's EBX home is decided across the call cycle in other files.
// MiniMax E (11-11): no new form, no lower score (65): frame 0x84 against 0x88, the component still arrives on the stack where retail has it in EBX, and the entry copy is rep movsd where retail uses four movaps; every form that makes the component a register argument puts it in EDI and costs matched 0x1d1b60: BLOCKED-BY 0x278f00 and the 0xb8540/0xb7430 drafts.
// MiniMax G (11-13): no new form, no lower score (65): retail's frame is 0x88 with the transform local at
// esp+8 and the inverse at esp+0x48, both 16-byte aligned, and esp is 16-byte aligned at the call to
// 0x2da3f0 (esi and edi pushed); this build's extra push ebx alone makes the frame 0x84 with the local at
// esp+4, so the component's EBX home is the one lever for the frame, the offsets and the plain ret.
// Retail's entry copy is the member-wise one (four movaps, as the matched 0x1d08e0 in this file), and
// rep movsd clobbers ECX, which is why the index cannot travel in ECX while the copy is a block copy.
// Matched precedents for a register argument in an aligned frame exist (0x1c59a0, 0x39780, 0x5b650,
// 0x62300, 0x70280, 0x93100, 0x93fa0, 0x136df0, 0x1523c0, 0x19d650, 0x1ce010), but none of them also
// stores an aligned SSE value, which is the shape 0x1d0cf0 needs.
// MiniMax I (11-14): no new form, no lower score (65): the frame is still 0x84 against retail's 0x88, the component still arrives on the stack where retail keeps it in EBX, and the entry copy is still rep movsd where retail uses four movaps; the component's EBX home is decided by LTCG over the whole 0x1d0cf0 -> 0x1d1260 -> 0x278f00 -> 0xb8540 cycle, whose bodies are in other files.
// MiniMax C (11-14): no new form, no lower score (65): retail's own argument order is (transform, index,
// component), the transform in EAX and the index in ECX, while this build takes the index and the component
// on the stack, and the parameter list may not be changed here; the frame is still 0x84 against retail's
// 0x88, the entry copy is still rep movsd where retail uses four movaps, and that convention is decided
// across the call cycle whose bodies are in other files.
// MiniMax D (11-16): no new form, no lower score (65): retail's own argument order is (transform in EAX, index in
// ECX, component in EBX) and this build takes the transform in EAX and pushes the index and the component, so the
// frame is 0x84 against retail's 0x88 and the entry copy is a rep movsd where retail uses four movaps; the copy
// through hkTransform::set is 90 at 231 bytes, so both differences have to be fixed together.
// MiniMax D (11-16): no new form, no lower score (65): with the copy through hkTransform::set (four movaps, as retail
// has it) the index does travel in ECX as retail has it - the block copy's clobbered ECX was what kept it on the stack -
// but the component still arrives on the stack where retail keeps it in EBX, the frame stays 0x84 against retail's 0x88
// and the length grows to 231 bytes (90); the component's EBX home is still decided by LTCG over the 0x1d0cf0 ->
// 0x1d1260 -> 0x278f00 -> 0xb8540 cycle, whose bodies are in other files: BLOCKED-BY 0x278f00 and the 0xb8540/0xb7430
// drafts.
// MiniMax R (11-17): no new form, no lower score (65): the frame is still 0x84 against retail's 0x88, the index and
// the component still arrive on the stack where retail keeps them in ecx and ebx, and the entry copy is still a
// rep movsd where retail uses four movaps; the component's ebx home is decided over the whole
// 0x1d0cf0 -> 0x1d1260 -> 0x278f00 -> 0xb8540 cycle, whose bodies are in other files (0x1d1260 is in
// src/unknown_1d1260.cpp): BLOCKED-BY 0x278f00 and the 0xb8540/0xb7430 drafts.
// MiniMax H (11-18): no new form, no lower score (65): the copy written as an assignment instead of a construction
// (a default hkTransform and then `local = *transform`, which copies the four members one by one) gives 90 at 231
// bytes, where the copy through hkTransform::set gave 77 at 237, so the member-wise copy does not pay for the frame
// and the index; the frame is still 0x84 against retail's 0x88, the index and the component still arrive on the stack
// where retail keeps them in ecx and ebx, and the entry copy is still a block copy where retail stores the four
// members separately: BLOCKED-BY 0x1d1260 and 0x278f00, whose bodies are in other files.
// score 65 (was 65): MiniMax R (11-15): no new form, no lower score; retail takes the transform in EAX, the index
// in ECX and the component in EBX (frame 0x5c) where this build pushes the index and the component (frame 0x50), and
// LTCG picks that split across the whole 0x1d0cf0 -> 0x1d1260 -> 0x278f00 -> 0xb8540 cycle, whose bodies are in other
// files: BLOCKED-BY 0x278f00 and the 0xb8540/0xb7430 drafts.
// score 65 (was 65): frame 0x84 versus 0x88; the index/component still arrive on the stack rather
// than ECX/EBX, and the constructor copy uses rep movsd rather than aligned SIMD. The two-use body
// slot receiver recovers the fixed-first guard and ordinary-path body lifetime; the post-reset
// body is fetched again. The convention still follows the call cycle outside this file.
// Earlier: component arrives on the stack where retail passes it in ebx (no push ebx,
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
// Round 357: Common setter with byte/word/dword/long refresh results, constructor then aligned set copy 77/83/82/82/56/59/58/58. Original body restored; frame and component transport remain.
// DL242: Actual body-array slot pointer/reference with constructor flag/fixed guards 83/65,
// aligned set 60/90 (60 loses matched 0x1d1b60); fixed-first whole-transform memcpy/memmove 65/78.
// Keep constructor copy with two-use slot pointer and fixed-first guard, ordinary whole-program
// confirmed; post-reset lookup retained. Component/index stack transport, 0x84/0x88 frame and
// rep/SIMD copy remain.
// Round 534: complete transform workspace roles and inverse entry/use storage on the
// two-use slot receiver, four name pairs: 65/65/65/65/65/65/65/65. Frame 0x84/0x88,
// constructor copy and stacked component/index remain; original complete body restored.
// Round 330: early native slot pointer/reference, constructor/set copy, before-copy then
// after-copy-before-inverse acquisition 86/85/86/85/88/85/88/85. Original body restored;
// frame/component transport remains; two early-constructor forms lose matched 0x1d1b60.
// DH298: Copy after inversion, assignment/set/memcpy/memmove with inverse at use
// 140/140/84/99; inverse at entry 140/140/84/99. All regress and are restored; fresh
// post-reset lookup and matched caller survive; frame and argument transport remain.
// N324: Native slot retained through common setter with fresh post-reset slot lookup; bool/byte/word/dword
// reset results, aggregate then aligned set copy 81/81/85/87/56/56/55/54. NOT kept: aligned forms lose
// matched 0x1d1b60 at 59; reset-result form remains unmatched. Frame and incoming argument transport remain.
// P281: Whole rotation/translation or SIMD-member aggregate initialization, inverse entry scope and
// actual slot before inversion: 106/90/106/90/101/85/101/85. All regress; early rotation/translation
// forms also lose the matched caller at 1. Original constructor, slot timing and complete body restored.
// M322: Slot-preserving nested/early/named reset guards with constructor
// 65/65/133; assignment 90/90/157; aligned setter nested/early 90/90.
// Assignment recovers SIMD/ECX index but frame 0x84 and stacked component
// remain, at 231/220 bytes. All regress or tie; original complete body restored.
// N328: Actual twice-used rigid-body reference, direct/getter acquisition and fixed-first/ordinary-return
// admission, original then aligned input copy: 85/85/85/85/88/88/88/88. Original slot graph restored;
// frame and incoming component/index remain.
// Round 380: Actual transformed-frame predicate, bool/byte/short/long relative truth
// then world complement: 65/65/65/65/80/80/80/80. Frame/copy and stacked index/component remain;
// original flag observation, native slot, fresh refetch and complete body restored.
// Round 382: Native fixed-body predicate alone on constructor/two-use-slot graph,
// bool/byte/short/long fixed truth then movable complement: 65/65/65/65/65/65/65/65. Original
// byte test, fresh refetch and full body restored; frame and component/index transport remain.
// DH309: Actual whole-array data receiver as pointer/field reference, original/aligned copy,
// with inverse at use/entry: 86/86/85/85/87/87/88/88. Original element receiver restored;
// frame, input copy and component/index transport remain.
// Round 548: Existing four-component setters for rotation/translation/both, inverse
// at use then entry, and both setters with early slot/use-entry inverse: 215/130/254/215/130/254/250/250.
// NOT kept: scalar-copy/frame and transport shapes miss; original constructor, fresh lookup and full body restored.
// N342: Fixed-path reset flag as bool/byte/short/long enabled truth 93/93/93/93,
// disabled complement 94/94/94/94. NOT kept: one-use predicates do not match;
// original constructor, two-use slot, fresh post-reset lookup and full body restored.
// Round 550: Sole-return done exit with constructor/assignment/set/memcpy/memmove
// copies 65/90/90/65/78; constructor/assignment/set with inverse at entry 65/90/90.
// NOT kept: exit/copy forms miss; original implicit exit, constructor, inverse scope,
// two-use slot and fresh post-reset lookup restored. Frame and component/index transport remain.
// F350: Native transform/reset word masks 89/90/94; shifted Boolean word
// forms 65/65/65; direct bitfields 94; both masks with the existing aligned
// copy 40 at 223/220 bytes. NOT kept: only the combined mask/copy graph gains,
// retail shifts the flags, and the length moves away. Original body restored;
// aligned frame, constructor copy and component/index transport remain.
// AC505: Native body-pointer member receiver, pointer/reference and fresh reset receiver,
// constructor copy 60/60/60/60; aligned set copy 93/93/93/93 (234/220 bytes).
// NOT kept: gain 5 below the gate; member receivers and all copy forms restored.
// N344: Actual two-use slot lexical homes array_slot/body_slot/current_slot/entry_slot,
// constructor then existing aligned setter: 65/65/65/65/90/90/90/90.
// NOT kept: aligned setters grow to 231/220 and miss. Original constructor, slot and fresh lookup restored.
// Round 3 of #1044: statement-by-statement walk of retail against the 88 body (set copy, body pointer,
// m_fixed first): offsets (+4 flags bit 6/bit 5, +0x30 transform, +0x70 data, stride 0x60, +0x40 body), shifts,
// both callees with ECX = component, the refetch after 0x1d1260, 0x1d8e80 called in the fixed arm and inlined
// in the other, and the local at esp+0x10 all agree, so there is no behaviour difference; only the component's
// transport differs (retail: EAX transform, ECX index, EBX component, plain ret and no push ebx). Retail's
// register ranks are index offset ESI, body EDI, component EBX; the flag5-first 27 form swaps body and component
// (body EBX, component EDI argument), the m_fixed-first forms keep retail's ranks but never convert. New: the
// local as aliased float storage (no aligned type; frame no longer realigned) 93 with the component still on
// the stack, so the aligned frame is not what blocks EBX here; flag5 first with direct rigid_bodies indexing at
// every use 77 (0x1d1b60 lost at 60); plain struct copy with flag5 first 85 (rep movsd takes ESI/EDI and the
// component still stays on the stack, so forcing ESI+EDI busy does not free EBX); parameter order (transform,
// component, index) on the 27 form 27 (same). Of 31 matched game functions with aligned SSE stack stores none
// takes an EBX argument, so there is no matched precedent for this shape. Callees 0x1d1260/0x1d1540 each save
// EBX, ESI and EDI, so their save sets do not explain it. Original body restored.
// Permuter (25 min on #1044 round 3, about 470 variants): only a volatile short copy of m_fixed with a char
// cast in the test and the arms swapped gave 60 (frame 0x94); not kept (volatile without a match).
// Round 153 (m3 lane 5, fast with 0x1d5120/0x1d0c20/0x1d1b60): set copy, body pointer, m_fixed first, direct-index refetch
// 88 (230 B); set copy, slot pointer fetched before the transformed block 85 (228 B); set copy, slot, direct refetch 90
// (231 B); constructor copy, body pointer, getter/direct refetch 85/85 (220 B). /Gd first pass: component EBX in every
// m_fixed-first form, EDI in flag5-first forms; only EDI ever becomes a register argument here.
// MiniMax D (11-17): no new form, no lower score (65): the copy of the transform is still retail's four
// memberwise SSE stores against our block copy, and the component still arrives on the stack instead of in
// the register retail uses for it, so our frame is four bytes smaller and our return still pops an
// argument; the memberwise copy through the existing hkTransform setter (65 -> 90 here and 38 -> 40 in
// 0x1d0c20) and the slot-hoist, direct-index and outer-scope inverse forms (85, 88, 90) were all worse and
// were restored. The component's transport is set by the call cycle 0x1d0cf0 -> 0x1d1260 -> 0x1d1540 ->
// 0x2da3f0/0x2da5d0, whose bodies are in other files.
// M3 lane 6 r203: LEAD (see 0x1d5120's note, build/r203/lead-1d0cf0-d8.patch): this body with the copy through
// hkTransform::set and one body pointer (`hkRigidBody *body = component->rigid_bodies[index].rigid_body;`) matches
// once 0x1d5120 calls the matrix setter 0x1d0c20 and 0x1d0c20's definition is `inline`: the component then
// arrives in EBX and the index in ECX. Not kept: matched 0x1d3fc0 then expands 0x1d0c20 and 0x1d5120 rises to 113.
// M3 lane 12 r150 (fast with 0x1d0c20, 0x1d1b60 and 0x1d5120; control 65, 0x1d0c20 38, 0x1d5120 77): the 88 body (set copy,
// body pointer, m_fixed first) re-measured 88 at 230 B, 0x1d0c20 40, 0x1d5120 73: line for line retail's except `push ebx;
// mov ebx, [ebp + 8]` and `ret 4` for retail's EBX argument. On it, every declared order of the three parameters (definition,
// the declaration in src/unknown_1d5120.cpp and both calls changed together) 88/88/88/88/88 (same code). Diagnostics on it
// (none a keepable form; each restored): the flag5 test first 27 at 220 B (component arrives in EDI, 0x1d1b60 lost at 59);
// 0x1d1540 called in place of 0x1d1260 88 (same code, so the callee's identity and its call cycle do not decide it); with
// either caller's call removed 88 (the callers do not decide it); without the two component calls 95 (EDI argument);
// without the inverse block 106 (EBX from the stack); no aligned locals at all 161 (EBX from the stack, the transform in
// EBP); the transform unused 102 (index in EAX, component still EBX from the stack); the index unused 84 (EDI argument);
// no else arm 138 / no second body lookup 77 at 220 B / no setter call in the fixed arm 92 (EDI argument each time); no
// flag5 test 140, the inverse block replaced by a call 87, the else arm through a second lookup 88, an extra early
// component test and call 111 (EBX from the stack each time); two more component calls in the fixed arm 63 (EDI argument,
// the body pointer in EBX). In nine shapes where the component is the third callee-saved value handed out (EBX) it stays
// on the stack, in seven where it is the second (EDI) it becomes a register argument; other matched functions do take an
// EBX argument that is live across calls (0x1d1a20 in this file, 0x860b0 with an aligned frame and a copied second
// argument), so the register itself is not barred. Left: what makes retail hand the component over in EBX with the body
// pointer still in EDI. In retail this function is on the object code's call cycle (0x1d1260 -> 0x278f00 -> 0xb8540 ->
// 0x109400 -> 0xe4c10 -> 0xe18d0 -> 0xb75a0 -> 0xb7430 -> 0x1c4a80 -> 0x1d3fc0 -> 0x1d0c20 -> here; 0x1d1540 reaches it
// too through 0x1c39c0), and in this build the cycle is open because 0xb8540 and 0xb7430 are disabled bodies behind
// empty stand-ins. Locate-only probe (plain check, every file restored): both bodies enabled and their stand-ins masked,
// with the 88 body here: still 88 (same code), 0xb8540 and 0xb7430 match, and matched 0xe18d0, 0xe4c10, 0xb75a0, 0x1d3fc0
// and 0x278f00 flip (33, 10, 73, 123, 185), as those two files' own notes say: the walk then enters the cycle at another
// member than retail's. BLOCKED-BY the cycle's entry (0xb8540, 0xb7430 and 0xb7b40 enabled together, as 0xb8540's note
// asks).
// @retail 0x1d0cf0
void havok_component_rigid_body_transform_set(long rigid_body_index, s_havok_component *component, hkTransform const *transform)
{
	hkTransform local;

	local.set(*transform);
	if (TEST_FIELD_BIT(component->transformed))
	{
		hkTransform inverse;

		inverse.transform_set_to_inverse(component->transform);
		local.transform_compose_in_place(inverse);
	}
	hkRigidBody *body = component->rigid_bodies[rigid_body_index].rigid_body;
	if (body->m_fixed && TEST_FIELD_BIT(component->flag5))
	{
		function_1d1260(component);
		havok_component_rigid_body_get(rigid_body_index, component)->motion_transform_set(local);
		function_1d1540(component);
	}
	else
	{
		body->motion_transform_set(local);
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

// @retail 0x1d0c20
void havok_component_rigid_body_matrix_set(long rigid_body_index, s_havok_component *component, transform4x3f const *matrix)
{
	hkTransform transform;
	/* read through its address: retail takes the component on the stack and loads it into EBX at the call */
	s_havok_component *const *home = &component;

	component_matrix_frame(matrix, &transform);
	(*home)->rigid_body_transform_set(rigid_body_index, &transform);
}

/* a member: every retail caller sets up the component (ESI) last, as for a member whose `this` comes first */
// @retail 0x1d1010
void s_havok_component::rigid_body_linear_velocity_change(long rigid_body_index, vector3f const *change)
{
	vector3f velocity;

	rigid_body_linear_velocity_get(this, rigid_body_index, &velocity);
	velocity.i += change->i;
	velocity.j += change->j;
	velocity.k += change->k;
	havok_component_rigid_body_linear_velocity_set(rigid_body_index, this, &velocity);
}

// @retail 0x1d1160
void havok_component_rigid_body_point_impulse_apply(long rigid_body_index, s_havok_component *component, point3f const *point, vector3f const *velocity)
{
	if (TEST_FIELD_BIT(component->flag1))
	{
		component->rigid_body_linear_velocity_change(rigid_body_index, velocity);
	}
	else
	{
		hkRigidBody *rigid_body = havok_component_rigid_body_get(rigid_body_index, component);
		hkVector4 havok_point;
		hkVector4 impulse;
		real mass;
		__m128 mass4;

		havok_from_vector3d(&havok_point, (vector3f const *)point);
		/* the change bound by reference: callers then count it as written through (0x1c4c50 reloads
		   its loop counter, kept beside the change, after this call) */
		vector3f const &change = *velocity;
		havok_from_vector3d(&impulse, &change);
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

PRIVATE __forceinline real component_acceleration_scale(real target, real acceleration, real exponent)
{
	real result;
	if (exponent > 0.001f)
		result = (real)pow(component_scalar_magnitude(target), exponent) * acceleration;
	else
		result = acceleration;
	return result;
}

PRIVATE __forceinline real component_time_step()
{
	return g_510c54->rate;
}

/* one tick of a component's speed toward a target position: the acceleration (scaled by a power of the distance
   and, when asked, divided by the mass) is applied until the stopping distance is reached, after which the speed
   follows sqrt(2 a d); returns the change in speed pinned to one tick's acceleration and stores |velocity / limit|
   in *ratio */
// The tick length is read at each use, once in each arm of the mass test and again for the stop distance: retail
// forms the rate's address in both arms and keeps it (`mulss xmm0, [eax + 4]`, `add eax, 4`, later `[eax]`).
// @retail 0x1d2280
real function_1d2280(bool allow_reverse, bool scale_by_mass, real mass, real target, real acceleration,
	real velocity, real limit, real position, real exponent, bool force_acceleration, real *ratio)
{
	real scaled_acceleration = component_acceleration_scale(target, acceleration, exponent);
	real step;
	if (scale_by_mass && mass > 0.001f)
		step = scaled_acceleration / mass * component_time_step();
	else
		step = scaled_acceleration * component_time_step();
	real magnitude = component_scalar_magnitude(step);
	target -= position;
	real next_velocity;
	if (step > 0.001f)
	{
		if (force_acceleration)
			next_velocity = velocity + step;
		else
		{
			real stop_distance = (velocity / step + 1.0f) * (velocity * component_time_step()) * 0.5f;
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

// MATCH (was 20; 29 and 172 before). Retail adds the parent's matrix offset last, (object + parent * 0x34)
// + offset, which changes how the compiler schedules the index arithmetic ahead of the inversion call; the
// parent matrix is therefore spelled that way instead of object_node_matrix(), whose (object + offset) +
// node sum the compiler keeps in its own order (20 -> 11). The restore arm then wants `initial`, the node
// matrices and the node's own matrix in that order (11 -> 0; the other five orders 16..58, the same byte
// sum for the restore arm's matrix 185). Changed nothing or lost earlier: stable handle 197; pointer
// names 172; in-place composition 212; default point setters 188; sibling induction 175; reordered locals
// and a precomputed matrix pointer. The full build also matches without the `standard` marker (the image
// is otherwise the same; the callers are itself and 0x1d3920, still unwritten); the marker is left as it was.
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
    s_slot_object_view *anchor_object = object_get(component->object_index);
    transform4x3f *parent_matrix = (transform4x3f *)((byte *)anchor_object + parent * sizeof(transform4x3f) + anchor_object->node_matrices_offset);
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
   transform4x3f *matrices = component_node_matrices(component->object_index);
   transform4x3f *matrix = object_node_matrix(object_get(component->object_index), (short)node_index);
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
real s_havok_component::function_1d1230(long rigid_body_index)
{
    real minimum, maximum;
    hkRigidBody *body = rigid_bodies[rigid_body_index].rigid_body;
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

/* the word is copied before the search: function_1d87b0 then keeps its zero in edi
   across the erase, as retail does (byte match; the parameter used directly keeps it in ebx) */
PRIVATE __forceinline void component_surface_property_set(hkEntity *entity, s_havok_property_word property)
{
    s_havok_property_word word = property;
    if (havok_entity_property_exists(entity, HAVOK_PROPERTY_2003))
        entity->entity_property_erase(HAVOK_PROPERTY_2003);
    entity->entity_property_put(HAVOK_PROPERTY_2003, word);
}

/* clears a vector; function_1d87b0 stores through this inline function:
   written in place, its store keeps function_181db0's result pointer off ebx
   (scores 69 for 0x181db0 and 211 for 0x1d87b0 that way) */
PRIVATE __forceinline void component_surface_vector_clear(hkVector4 *vector)
{
    vector->m_quad = _mm_setzero_ps();
}
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
        vector3f_add(&velocity, &local_c7c2ff, &velocity);
        speed_sum += component_surface_length(&local_c7c2ff);
    }
    normalize3d(&velocity);
    vector3f const *scaled = vector3f_scale(&velocity,
        ((bool)((component->unknown04 >> 18) & 1) ? 1.0f : 2.5f) * speed_sum / material_count,
        &velocity);
    hkVector4 *result = &body->unknown30;
    result->set(scaled->i, scaled->j, scaled->k);
    if (other_contacts == 0)
    {
        body->unknown44 = 1;
        if ((bool)((component->unknown04 >> 18) & 1))
            component_surface_property_set((hkEntity *)body->rigid_body, s_havok_property_word(0));
        else
            component_surface_property_set((hkEntity *)body->rigid_body, s_havok_property_word((long)result));
    }
    else
    {
        body->unknown44 = 2;
        component_surface_property_set((hkEntity *)body->rigid_body, s_havok_property_word(0));
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

// The rigid body is read through the component at each use: a local body pointer makes LTCG pass the
// component in ECX instead of retail's EAX (194). Retail takes the component in EAX and the body index
// in EDX (0x1d83f7 `mov edi, eax; mov eax, [edi + 8]` reads the component's object index, 0x1d8438
// `lea edx, [edx + edx*2]` scales the index by the rigid-body stride), so the component comes first:
// with the index first LTCG swaps the two registers (4). The matrix's position changes no bytes here
// or in 0x1d7ef0's calls, so it stays last.
// @retail 0x1d83e0
void function_1d83e0(s_havok_component *component, long body_index, long node_a, long node_b,
    transform4x3f *matrix)
{
    s_component_transform_definition *definition = (s_component_transform_definition *)g_4e3b44[
        ((s_component_transform_header *)g_4e0300->data)[component->object_index & 0xffff].object->definition_index & 0xffff].bytes;
    s_component_collision_model_link *model = (s_component_collision_model_link *)g_4e3b44[definition->model_index & 0xffff].bytes;
    s_component_node_body_model *physics = (s_component_node_body_model *)g_4e3b44[model->physics_model_index & 0xffff].bytes;
    long child_a = NONE;
    long child_b = NONE;
    for (long i = 0; i < component->rigid_bodies.data[body_index].node_count && (child_a == NONE || child_b == NONE); i++)
    {
        s_component_node_body_link *link = &physics->bodies[component->rigid_bodies.data[body_index].nodes[i]];
        if (link->node == node_a && child_a == NONE)
            child_a = i;
        if (link->node == node_b && child_b == NONE)
            child_b = i;
    }
    c_component_child_frames *shape = *(c_component_child_frames **)((byte *)component->rigid_bodies.data[body_index].rigid_body + 0xc);
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

/* caps each rigid body's velocities and adds one step of gravity to the simulating ones */
// @retail 0x1ced00
void function_1ced00(s_havok_component *component)
{
    if (TEST_FIELD_BIT(component->flag5) && havok_component_any_rigid_body_active(component))
    {
        vector3f change;
        real dt = g_510c54->rate;
        real gravity = g_51e9c4->unknown0;
        change.i = 0.0f;
        change.j = 0.0f;
        change.k = 0.0f - dt * gravity;
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
                component->rigid_body_linear_velocity_change(i, &change);
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
        function_1d83e0(component, index_a, first->node, node_a, &relative);
        function_142a60(&relative, &pair->frame_a, &matrix);
        component_matrix_frame(&matrix, frame_a);
    }
    else
        component_matrix_frame(&pair->frame_a, frame_a);
    *body_a = component->rigid_bodies.data[index_a].rigid_body;
    long node_b = NONE;
    long index_b = NONE;
    if (pair->node_b != NONE)
    {
        node_b = pair->node_b;
        index_b = node_b == NONE ? mapping->fallback : mapping->bodies[node_b];
    }
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
            function_1d83e0(component, index_b, other_first->node, node_b, &relative);
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

// score 2 (was 11): The corrected vector spills to 0x40 and reloads from that slot in the final mulps. Retail
// uses the saved impulse slot at 0x30.
// MiniMax H (11-20): no new form, no lower score (2): the corrected vector still spills to 0x40 where retail keeps it in
// the argument vector's own slot (0x30). The corrected vector copied into the argument vector before the scale call
// measures 23 and the final product in its own temporary 65, so it stays a plain temporary.
// MiniMax C (11-20): no new form, no lower score (2): retail stores the corrected vector into local_13's slot at 0x30 and folds its reload into the final mulps while this build stores it at 0x40 and reloads it there; the in-place form measures 23 and the store can only be written as an hkVector4 temporary, which the header does not allow here.
// MiniMax G (11-19): no new form, no lower score (2): only the corrected vector's spill slot differs (retail stores it to
// local_13's slot at 0x30 and folds its reload into the final mulps, this build stores it to 0x40 and reloads it there);
// the correction in place in local_13 measures 23 (the store goes to 0x30 but the reload after 0x183dc0 is a separate
// movaps, never folded), the correction as a named hkVector4 and the correction declared earlier both give the same 2,
// the final product's operands swapped 19, and a plain correction sharing local_13's slot grows the frame to 0x64 (94).
// MiniMax B (11-20): no new form, no lower score (2): the corrected vector still spills to 0x40 and the final
// mulps folds that slot, where retail keeps the corrected vector in the argument vector's own slot (0x30) and
// folds its reload. The cause is now pinned: in retail the copy of local_7, the corrected vector and the scaled
// vector all live in the one slot at 0x30, so the argument vector is corrected in place and MSVC folds the
// reload of that slot into the mulps; in this build the in-place form (23, and 19 with the operands swapped)
// makes MSVC reload the vector right after the get_inverse_scale call instead, because the argument's address
// escapes and the x87 store of the scale factor may then alias it. A plain __m128 for the corrected vector folds
// the way retail does but takes its own slot (94 with a named argument, 93 with a materialized temporary whose
// slot MSVC does not share with the plain one), and the copy-propagation spelling (94) grows the frame. The two
// remaining differences are the spill address and the folded operand address.
// Earlier score 2 (was 11): only the corrected vector's spill differs: retail stores it to local_13's slot (0x30) and
// folds its reload into the final `mulps`, ours stores it to 0x40. (Earlier first lines: the friction magnitude in
// another dead slot, the element formed offset-first in retail, retail's y loaded early and stored after z; the
// round 139 forms below fixed the first and the last.)
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
// Round 140 (c1C): the corrected vector computed in place in the impulse, with the final product splat-first, in a
// named splat, or after a no-op self-assignment: 46 (908 bytes; the reload after get_inverse_scale is a separate
// hoisted load, retail folds it as `mulps xmm1,[esp+0x34]`); the early copy as an __m128 snapshot and the impulse
// declared late: frame grows to 0x64+. Element as `data + index`, pointer arithmetic on bytes, or a long index
// local: 27 (same code).
// Permuter (25 min on #1044 round 2, about 3100 variants): nothing below 27.
// b13D (11-17) walk against retail, fast build on r846: no logic difference (fields, jcc senses, constants, the three
// vcalls and callees agree; 905 bytes). Kept: the rigid-body type test read from data[index] before the element pointer
// is formed (retail 0x1cf715 reads [eax+edx+0x44] with eax = data, then 0x1cf719 adds), 27 to 11; it fixes the element
// order named in the first lines and the register rotation after it. Not kept: the finiteness test through a copy of
// the by-value helper that src/unknown_0a0190.cpp and src/unknown_14b560.cpp each keep privately (a third copy would be
// a twin) puts the magnitude in the friction pointer's slot as retail does (0x1cf61e, esp+0x18): 9 with the read above.
// Still differ (9 with the helper): y (retail loads it at 0x1cf738, stores it at 0x1cf759 after z and the 0.6 load at
// 0x1cf751) and the corrected vector (retail 0x1cf86a stores it to esp+0x30 and 0x1cf890 folds it; ours esp+0x40).
// Tried on this base: corrected in place in the impulse 30 (908 bytes, the load is a separate movaps ahead of
// 0x1cf87a); early copy as an __m128 snapshot with the impulse declared late 99 (frame 0x64); impulse as an inline temp
// 98; the vector through _mm_setr_ps or _mm_set_ps 99; component stores in x, z, y order, y or x/y/z in locals,
// havok_from_vector3d 9 (same code, the scheduler orders the loads); the element's data pointer in a local 25 (old
// base).
// M3 lane 1 r117 (fast, base 11): corrected in place in local_13 re-measured 32 (908 B, the reload after 0x183dc0 a
// separate movaps); that with the final scale through a pointer helper 32; local_13 or corrected renamed to sort
// first/last 11/11/11/11 (the spill slot ignores names); the friction magnitude renamed magnitude/value/sorting
// first/last 11/11/11/11 (slot 0x14 unchanged); a separate final impulse vector 101 (frame 0x64), with the copy and
// corrected in an inner block 100; the subtraction reading the scaled vector with no copy 101 (frame 0x64).
// M3 lane 2 r100 (a one-file LTCG harness of this file with src/unknown_277b80.cpp, src/unknown_1d8dd0.cpp and
// src/unknown_183da0.cpp gives the full build's 11 exactly): corrected in place in local_13 re-measured 24 (the store
// goes to retail's 0x30, but the final product reads local_13 with its own movaps after 0x183dc0, never folded); on
// that form the final product reading local_13 through a const reference 24, through a const pointer 24, local_13
// copied with hkVector4's operator= 24; with the by-value finiteness test (as the private copies in
// src/unknown_0a0190.cpp and src/unknown_14b560.cpp) 22; the by-value test alone re-measured 9; local_7 written
// through m_quad.m128_f32[0..3] in place of set() 11 (same code), with the by-value test 9. Retail's fold of the
// final operand marks it as a spill reload of a temp packed into local_13's slot, not a read of local_13.
// M3 lane 9 r139 (fast): 1-7 dead initialised locals 11 each, with the by-value test 9 each (no symbol-count tie).
// Kept: the surface vector through a `vector3f` and the file's two converters (vector3d_from_havok, then
// havok_from_vector3d) with the finiteness test through the shared by-value helper: 11 -> 2 (y now loaded early
// into xmm1 and stored after z, the magnitude in the friction pointer's 0x18 slot); without the helper 28 (the zero
// and 1.0 constants swap xmm1/xmm2); through the top's x/z/y converter into a real[3] or vector3f 2/2. On that
// base: corrected in place in local_13 23 (908 B, its reload after the scale call is a separate movaps, never
// folded); through new hkVector4 subMul4/mul4 members 23, through file-local forceinline helpers taking
// `hkVector4 &` 23, the final scale alone through mul4 23 (all the in-place code); corrected as a named hkVector4 2
// (same code); local_13 by `local_13 = local_7;` 2, copy-initialised from local_7 124/124 (932 B); a 1500 s
// permuter run (608 tries) none below 2.
// Impulse snapshot aggregate 2; separate final aggregate 94; read-only snapshot and final aggregates 94;
// immutable correction 2; immutable square, projection and correction 2; correction declared before
// the length test 2; impulse and velocity declarations with the surface 7; named scaled SIMD product 2.
// Projected velocity vector named before subtraction 2, Intrinsic broadcast of the final scale 7, Intrinsic
// broadcasts of projection and final scale 16, Intrinsic broadcasts of all three scalar scales 87, Initialize
// corrected register from the saved impulse before subtracting 2, Velocity and normalization workspace ends at
// correction 2, Newton product named separately from the reciprocal-length correction 2, Final scale splatted in
// its own statement 2, Corrected vector and final scale in one intrinsic expression 7, Corrected vector formed at
// the final product with the original scale splat 50. NOT kept: The projection names, velocity scope and split
// splat leave both spill operands unchanged. The intrinsic broadcasts preserve the extent at higher scores.
// Forming the correction at the final product extends the body to 915 bytes.
// M3 lane 12 r180 (h2m fast, 2): local_14 declared before local_13 2, on its own line first 2 (same code); the copy
// into local_13 after the velocity call 62; the subtraction reading local_7 instead of the copy 57 (915 B, frame
// 0x64); local_13 declared and copied ahead of the length test 8.
// M3 lane 12 r181 (h2m fast, 2): the copy as an __m128 value with local_13 written only by the final product, with
// the correction named 94, in place in that value 94, local_13 declared just before the final product 94 (all frame
// 0x64: the value and local_13 never share a slot); local_13 corrected in place and read back into a named __m128
// for the final product 29; in place with the factor through _mm_load_ss 28; the factor through _mm_load_ss alone
// 23 (907 B). Diagnostic: the final product into local_7 and local_7 passed (local_13 no longer address-taken) 100
// either way, the subtraction then moves after 0x183dc0 and the copy gets its own slot.
// M3 lane 4 r255 (h2m fast, 2): corrected in place in local_13 with the final product `local_13 * splat` 19 (908 B; the
// store goes to retail's 0x30 and only the last product differs), on it a named splat first/last 23/19, the factor
// splatted in place first/last 23/19, splat first unnamed 23; the splat through `__m128 &` bound to the factor 6 or a
// named splat read through `__m128 const &` 6 (both 905 B: only the hoisted local_13 load differs); local_13's value or
// local_13 through a const reference 23/23; corrected also stored into local_13 ahead of the scale call 30 (910 B);
// local_13 copied back into an `__m128` for the final product 29 (910 B). Compile-only (CL without /GL, /FAs; it gives
// this build's frame exactly: local_7 -64, local_13 -48, two temps and corrected -32, local_14 -16): an address-taken
// local is never folded into `mulps` (a read of local_13 not just written is a separate load too), and it never shares
// its slot with a temp unless it is block-scoped (a block-local final vector shares with the spilled local_18); corrected
// declared ahead of local_13, after it, or in the outer block, and local_14 declared first: corrected stays at -32. A
// block-local copy (also via operator=) is forwarded from local_7 and never stored. Reading: retail's 0x30 is written by
// the copy, the correction and the final vector and folded once, so its folded read is a non-address-taken value packed
// with the address-taken final vector; no spelling here gives that packing.
// M3 lane 1 r164 (h2m fast, 2): the frame packing: local_13 is address-taken, so it interferes with every
// value live across a call in its scope (the copy, the corrected vector across the scale call), and the corrected
// vector joins the newest free 16-byte slot (the 0x40 temps). Forms: local_13 corrected in place with the final in an
// inner block 93 (910 B; local_13 is then forwarded and the subtraction sinks past the scale call); the copy as an
// in-place __m128 with the final in an inner block 93 (the copy is forwarded from local_7); a separate copy with the
// final declared after the scale call 94 (copy forwarded, the final interferes with everything in its block); the copy
// as an in-place __m128, the direction normalized in place in local_7 and the final in an inner block 61 (915 B: the
// final then shares the copy's home, as in retail, but the subtraction sinks past the scale call); that with the speed
// splat written into the subtraction 37 (905 B, the speed load sinks too). Lead: retail's 0x30 group is one register
// value with four references (copy spill, reload, corrected spill, folded reload) plus a block-local final; nothing
// found keeps the subtraction ahead of the scale call without an address-taken operand.
// M3 lane 14 r153 (fast screen, 2): the impulse bound as `hkVector4 &impulse = local_13` and corrected in place through
// it, with a separate final vector passed to the call: 89 (905 B; the subtraction stays ahead of the scale call and the
// final product folds the 0x30 reload as retail, but the final takes a new slot); that final in an inner block 90 (the
// copy is then forwarded from local_7, frame 0x44); that with the direction normalized in place in local_7 (no local_18)
// 7 (905 B: copy, corrected and the folded reload all at 0x30; left: the copy store sits after `push ebx` where retail
// stores it before the vtable load, and the block-local final takes 0x40, retail 0x30); the same without the reference
// 61 (the subtraction sinks past the scale call). On the 7 form: the final in local_13 itself through the reference,
// passed to the call, 23 (908 B: the copy store moves to retail's place, the reload is no longer folded); the final
// initialised from the impulse, the reference declared at the copy, the final as a splat then an in-place product, the
// final passed through a const reference: 7 each (same code); the copy in a block ending at the subtraction with the
// corrected value an `__m128` declared outside it 61, without the reference 61. Reading: the final (2 references) is
// packed after the rsqrt temporary (3) and joins that temporary's 0x40 slot; the copy store keeps retail's place only
// when the copy's address reaches a call, and the fold only when it does not.
// M3 lane 4 r260 (h2m fast, 2; body unchanged): the frame packer's counts on lane 14's 7 form (impulse by reference,
// direction normalized in place, block-local final): the impulse has four references and makes the 0x30 slot, the rsqrt
// temporary three and makes 0x40, the final two, so it joins the newest slot it does not interfere with, 0x40. The final
// lands in 0x30 only with exactly four references (diagnostics: a second in-place product and call on it give five and it
// joins local_7's 0x20, 53 at 932 B; the final passed as both arguments, or to two calls, three and still 0x40; bare
// address expressions add none). On the 7 form, all 7 with the same code: the final as the by-value result of a
// file-local inline; the final set to the splat and then multiplied in place; the final copied from the impulse by
// member or by operator= and scaled in place; the final by an aggregate initialiser; the final passed through a const
// reference declared ahead of its store; one component zeroed ahead of the product. Diagnostic: the scale getter as a
// function the link can see keeps the corrected vector in XMM2 across the call and takes the result in XMM0 (34), so
// retail's getter is outside the link's view as it is here and retail's reload is a real one. Compile-only: a read of a
// vector whose address reaches a call is never a memory operand of `mulps` (with or without the x87 store between), a
// read of one only bound by reference is. So retail's folded 0x30 read is of an object whose address reaches no call,
// and the final vector shares its slot; what gives the final four packing references with one store and one `lea`
// emitted is not found.
// M3 lane 4 r261 (fast screen, 2): the frame packer's order for the 16-byte objects: local_7 (7 references), local_13
// (4), the rsqrt temporary (3), then the 2-reference objects, each joining the newest slot it does not interfere with;
// the named corrected vector interferes with local_13 and joins the temporaries' 0x40. Forms: the final scale through
// the file's existing component_bounds_splat with corrected named 2 (same code); with the correction unnamed inside
// the final product 2 (same code: the spilled temporary also joins 0x40); the copy as a separate `__m128` or a
// non-address-taken hkVector4 with local_13 written only by the final product, at the block top or in an inner block,
// 94/94/94/94 (910 B, the copy forwarded from local_7); those four with the direction normalized in place in local_7,
// the correction named or unnamed, 62 each of six (915 B, the subtraction sinks past the scale call); re-measured:
// corrected in place in local_13 with `splat * local_13` 23 (908 B), with the splat bound through `__m128 &` 5 (905 B,
// the local_13 load hoisted above the fstp). All restored.
// M3 lane 15 r147 (fast screen, 2; the 7 form re-measured 7): on the 7 form, the final declared first in the
// length-test block and written only at the end 8 (frame 0x64, the final takes a new 0x60 slot); the final product
// named outside the final's block, the block holding only the store and the call 7 (same code); local_13 declared
// with local_7 in the outer block 7; local_14 declared ahead of local_13 7. On the kept body: the copy written through
// a reference to local_13 with the corrected vector a value 2 (same code). All restored.
// M3 lane 2 r152 (H2M_FAST, 2): the contact point named once as `hkVector4 const &point` for both calls 2; the
// direction as an hkVector4 object 2; the velocity product as an hkVector4 object 2; the velocity local_14 in a block
// that ends after the speed is taken 2 (all the same code); lane 14's 7 form with that velocity block 7 (905 B: the
// block-local final then joins the freed velocity slot 0x50 in place of 0x40, so it takes the newest slot free of
// interference, and 0x30 needs every newer slot busy while the final is live). All restored.
// M3 lane 7 r232 (h2m fast, 2): the frame packer's 16-byte list on the kept body is local_7 (7), local_13 (4), the rsqrt
// temporary (3), the square's spill (2), local_14 (2), corrected (2); local_13 interferes with corrected over its whole
// first-to-last range, so corrected joins the rsqrt slot. Forms: the product inside the subtraction written
// `local_18 * splat` 4 (905 B, its two registers swap); `__m128 const corrected` 2; the speed, correction, scale and final
// call in an inner block 2; the rsqrt steps in an inner block with local_18 declared outside 2; the scale as `0.2f *
// get_inverse_scale()` 2 (same code). Diagnostic: the correction stored back into local_18 2 (same slot). Reading: retail's
// layout follows if the vector passed to the call is its own object with four references (placed after the 4-reference
// impulse, ahead of the rsqrt temporary, and joining the impulse's slot); with two it comes after every 3-reference object.
// M3 lane 2 r156 (h2m fast, 2): the frame packer's 16-byte order on lane 14's 7 form is local_7 (7 references),
// local_13 (copy and correction, 4), the rsqrt temporary (3, opens 0x40), the square's spill (2), the velocity (2, its
// address reaches a call, so it interferes to the end of its block and opens 0x50) and the final (2), which joins the
// newest slot it does not interfere with, 0x40; to land in 0x30 it has to be placed ahead of the rsqrt temporary, so it
// needs exactly four references. Forms: the scaled impulse as its own `__m128` normalized in place with the copy and
// correction one `__m128` and a block-local final 62 (915 B, the subtraction sinks past the scale call); lane 14's 7
// form re-measured 7; on it the final stored by `_mm_store_ps(&final(0), ...)` 23 (907 B, the store goes through ECX),
// the copy into local_13 moved after the velocity call 82 (frame 0x44), the reference spelled as a pointer to local_13
// 7 (same code); on main the correction bound as `__m128 const &corrected` 2 (same code). All restored.
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
 if (!function_x41b793(local_4)) local_4 = 0.0f;
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
  if (local_2->rigid_bodies.data[arg_1->field_45].unknown44 == 2 && !TEST_FIELD_BIT(local_2->flag18))
  {
   s_havok_component_rigid_body *local_6 = &local_2->rigid_bodies.data[arg_1->field_45];
   hkVector4 local_7;
   vector3f surface;
 vector3d_from_havok(&surface, &local_6->unknown30);
 havok_from_vector3d(&local_7, &surface);
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
