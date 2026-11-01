/* UNKNOWN_1CEC30.H: the parts of Havok's rigid body (hkEntity) that the
   game's havok components (src/unknown_1cec30.cpp) read: the property list
   at +0x30, whose keys 0x2001..0x2003 the game uses for its own indices. */

#ifndef UNKNOWN_1CEC30_H
#define UNKNOWN_1CEC30_H

#include "unknown_11c920.h"
#include "globals.h"
#include <xmmintrin.h>

class s_havok_property_word
{
public:
	s_havok_property_word() {}
	s_havok_property_word(long value) : m_data(value) {}

	long m_data;
};

struct s_havok_entity_property
{
	dword m_key;
	s_havok_property_word m_value;
};

/* an hkArray: data, size, capacity (the top bit set when the array does
   not own its storage) */
struct s_havok_array
{
	void *data;
	long size;
	dword capacity_and_flags;
};

class hkEntity
{
public:
	s_havok_property_word entity_property_erase(dword key);
	void entity_property_put(dword key, s_havok_property_word value);

	byte unknown00[0x30];
	s_havok_entity_property *m_properties;
	long m_property_count;
	dword m_property_capacity;
};

/* a property of the entity, 0 when it has none */
inline long havok_entity_property_get(hkEntity const *entity, dword key)
{
	long i;

	for (i = 0; i < entity->m_property_count; i++)
	{
		if (entity->m_properties[i].m_key == key)
		{
			return entity->m_properties[i].m_value.m_data;
		}
	}
	return 0;
}

/* Havok's vector: four reals, 16 byte aligned */
struct hkVector4
{
	__m128 m_quad;

	void operator=(hkVector4 const &v) { m_quad = v.m_quad; }
	real const &operator()(long i) const { return m_quad.m128_f32[i]; }

	void set(real x, real y, real z, real w = 0.0f)
	{
		real *components = (real *)&m_quad;

		components[0] = x;
		components[1] = y;
		components[2] = z;
		components[3] = w;
	}
};

struct hkRotation
{
	hkVector4 m_col0;
	hkVector4 m_col1;
	hkVector4 m_col2;
};

struct hkTransform
{
	void set(hkTransform const &t)
	{
		m_rotation.m_col0 = t.m_rotation.m_col0;
		m_rotation.m_col1 = t.m_rotation.m_col1;
		m_rotation.m_col2 = t.m_rotation.m_col2;
		m_translation = t.m_translation;
	}

	/* this = this * b */
	void transform_compose_in_place(hkTransform const &b);
	void transform_set_to_inverse(hkTransform const &t);

	hkRotation const &rotation_part() const { return m_rotation; }

	hkRotation m_rotation;
	hkVector4 m_translation;
};

/* Havok's boolean, returned through a hidden pointer */
class hkBool
{
public:
	hkBool() {}

	char m_bool;
};

/* Havok's motion of a rigid body: the virtual slots the game calls (the
   others are placeholders), its inverse mass, velocities and transform */
class c_havok_body_motion_state
{
public:
	virtual void slot00(void) {}
	virtual void slot01(void) {}
	virtual void slot02(void) {}
	virtual void slot03(void) {}
	virtual void slot04(void) {}
	virtual void slot05(void) {}
	virtual long kind_code(void) const { return 0; }
	virtual void slot07(void) {}
	virtual void slot08(void) {}
	virtual void getInertiaWorld(hkRotation &inertia) const {}
	virtual void slot0a(void) {}
	virtual void slot0b(void) {}
	virtual void slot0c(void) {}
	virtual void slot0d(void) {}
	virtual void slot0e(void) {}
	virtual void slot0f(void) {}
	virtual void slot10(void) {}
	virtual void slot11(void) {}
	virtual void slot12(void) {}
	virtual void pose_write(hkTransform const &transform) {}
	virtual void setLinearVelocity(hkVector4 const &velocity) {}
	virtual void setAngularVelocity(hkVector4 const &velocity) {}
	virtual void getPointVelocity(hkVector4 const &point, hkVector4 &velocity) const {}
	virtual void slot17(void) {}
	virtual void applyPointImpulse(hkVector4 const &impulse, hkVector4 const &point) {}
	virtual void slot19(void) {}
	virtual void slot1a(void) {}
	virtual void slot1b(void) {}
	virtual void applyLinearImpulse(hkVector4 const &impulse) {}

	real getMass(void) const
	{
		real mass_inverse = ((real const *)&m_inertia_and_mass_inverse)[3];

		return mass_inverse == 0.0f ? 0.0f : 1.0f / mass_inverse;
	}

	/* the vtable pointer is padded to the class's alignment, 0x10 */
	byte unknown10[0x20 - 0x10];
	hkVector4 m_inertia_and_mass_inverse;
	byte unknown30[0x40 - 0x30];
	hkVector4 m_linear_velocity;
	hkVector4 m_angular_velocity;
	byte unknown60[0x80 - 0x60];
	hkTransform m_transform;
};

class c_havok_body_group;

class hkRigidBody
{
public:
	void pose_write(hkTransform const &transform);
	hkBool body_is_simulating(void) const;
	void activate(void);
	inline void motion_transform_set(hkTransform const &transform);

	byte unknown00[0x8];
	void *m_world;
	byte unknown0c[0x3c - 0xc];
	c_havok_body_motion_state *m_motion;
	bool m_fixed;
	byte unknown41[3];
	c_havok_body_group *m_simulation_island;
};

class c_havok_entity_observer;

/* the parts of Havok's entity, simulation island and world the game's
   physics code calls */
class c_physics_body_island_view
{
public:
	void entity_observer_detach(c_havok_entity_observer *listener);
	void activate(void);

	byte unknown00[0x44];
	struct s_havok_motion_type_view
	{
		byte unknown00[0x40];
		long m_type;
	} *m_motion;
};

class c_havok_body_group
{
public:
	byte unknown00[0x3c];
	hkEntity **m_entities;
	long m_entity_count;
};

class hkWorld
{
public:
	hkBool entity_world_remove(hkEntity *entity);
	hkEntity *entity_world_insert(hkEntity *entity);
	void world_island_discard(c_havok_body_group *island);

	byte unknown00[8];
	c_havok_body_group **m_islands;
	long m_island_count;
};

/* the objects as the physics code sees them: the flag at +0xc0 marks an
   object counted in g_51e9a0 */
struct s_havok_object
{
	byte unknown000[0xb4];
	long havok_component_index;
	byte unknown0b8[0xc0 - 0xb8];
	word havok_flag : 1;
	word unknownc0 : 15;
};

struct s_havok_object_header
{
	byte unknown00[8];
	s_havok_object *object;
};

inline s_havok_object *havok_object_get(long object_index)
{
	return ((s_havok_object_header *)g_4e0300->data)[object_index & 0xffff].object;
}

/* counts the object in g_51e9a0, once (0x1c3980) */
void havok_object_count(long object_index);

/* the havok components (src/unknown_1cec30.cpp): a data array of 0x200
   components of 0xa0 bytes, one per object with Havok rigid bodies.
   Their arrays: data, size, capacity (the top bit set when the array does
   not own its storage); Havok's thread memory (g_480118) frees the storage */
struct s_havok_component_rigid_body
{
	point3f position;
	vector3f linear_velocity;
	vector3f angular_velocity;
	byte unknown24[0x30 - 0x24];
	/* the velocity of the surfaces the body rests on (function_1d87b0) */
	hkVector4 unknown30;
	hkRigidBody *rigid_body;
	byte unknown44;
	byte flags45;
	byte unknown46[0x48 - 0x46];
	/* the object's nodes the body drives */
	char *nodes;
	long node_count;
	byte unknown50[0x60 - 0x50];
};

/* Havok's contact between two entities, as the impacts see it */
struct s_havok_contact_entities
{
	byte unknown00[0xc];
	hkEntity *entity_a;
	hkEntity *entity_b;
};

/* a contact (unknown7c): its impact (src/impacts.cpp) */
struct s_havok_component_element0c
{
	short unknown00;
	short unknown02;
	long impact_index;
	s_havok_contact_entities *contact;
};

/* a constraint (unknown88): its impact and the rigid bodies it joins */
struct s_havok_component_element48
{
	byte unknown00[0x8];
	long impact_index;
	byte unknown0c[0x10 - 0xc];
	short material_a;
	short material_b;
	/* the other havok component, NONE when there is none */
	long component_b;
	byte unknown18[0x1c - 0x18];
	point3f position;
	vector3f normal;
	byte unknown34[0x38 - 0x34];
	real impulse;
	byte unknown3c[0x45 - 0x3c];
	char rigid_body_index_a;
	char rigid_body_index_b;
	byte unknown47;
};

struct s_havok_component_element08
{
	byte unknown[0x8];
};

struct s_havok_array60
{
	s_havok_component_rigid_body *data;
	long size;
	long capacity_and_flags;

	s_havok_component_rigid_body const &operator[](long i) const { return data[i]; }

	~s_havok_array60()
	{
		if (!(capacity_and_flags & 0x80000000))
		{
			g_480118->allocate((long)data, (capacity_and_flags & 0x7fffffff) * sizeof(s_havok_component_rigid_body), 0x12);
		}
	}
};

struct s_havok_array0c
{
	s_havok_component_element0c *data;
	long size;
	long capacity_and_flags;

	~s_havok_array0c()
	{
		if (!(capacity_and_flags & 0x80000000))
		{
			g_480118->allocate((long)data, (capacity_and_flags & 0x7fffffff) * sizeof(s_havok_component_element0c), 0x12);
		}
	}
};

struct s_havok_array48
{
	s_havok_component_element48 *data;
	long size;
	long capacity_and_flags;

	~s_havok_array48()
	{
		if (!(capacity_and_flags & 0x80000000))
		{
			g_480118->allocate((long)data, (capacity_and_flags & 0x7fffffff) * sizeof(s_havok_component_element48), 0x12);
		}
	}
};

/* allocated on its own from Havok's thread memory */
struct s_havok_array08
{
	s_havok_component_element08 *data;
	long size;
	long capacity_and_flags;

	~s_havok_array08()
	{
		if (!(capacity_and_flags & 0x80000000))
		{
			g_480118->allocate((long)data, (capacity_and_flags & 0x7fffffff) * sizeof(s_havok_component_element08), 0x12);
		}
	}

	static void operator delete(void *block)
	{
		g_480118->allocate((long)block, sizeof(s_havok_array08), 0x12);
	}
};

struct rigid_transform_scaled;

struct s_havok_component
{
	short identifier;
	byte unknown02[2];
	union
	{
		dword unknown04;
		struct
		{
			dword flag0 : 1;
			dword flag1 : 1;
			dword flag2 : 1;
			dword flag3 : 1;
			dword flag4 : 1;
			dword flag5 : 1;
			dword transformed : 1;
			dword flag7 : 1;
			dword flag8 : 1;
			dword flag9 : 1;
			dword flag10 : 1;
			dword flag11 : 1;
			dword flags12 : 20;
		};
		struct
		{
			dword : 18;
			/* no surface-velocity push (function_1d87b0, function_1cf520) */
			dword flag18 : 1;
		};
	};
	long object_index;
	long unknown0c;
	long unknown10;
	real unknown14;
	char unknown18;
	char unknown19;
	bool unknown1a;
	bool unknown1b;
	signed char unknown1c;
	byte unknown1d[3];
	long unknown20;
	byte unknown24[0x30 - 0x24];
	/* applied to the rigid bodies' transforms when transformed is set */
	hkTransform transform;
	s_havok_array60 rigid_bodies;
	s_havok_array0c unknown7c;
	s_havok_array48 unknown88;
	s_havok_array08 *unknown94;
	hkRigidBody *rigid_body;
	long unknown9c;

	~s_havok_component();
	void initialize(long object_index);
	void function_1d3d20(long node_index, dword *updated, dword const *restore,
		rigid_transform_scaled *orientations, long unknown);

	bool rigid_body_keyframed(long rigid_body_index);
	void rigid_body_inertia_get(long rigid_body_index, struct matrix3x3 *inertia);
	real rigid_body_mass_get(long rigid_body_index);
	void rigid_body_matrix_get(long rigid_body_index, struct transform4x3f *matrix);
};

inline s_havok_component *havok_component_get(long component_index)
{
	return &((s_havok_component *)g_51e9b8->data)[component_index & 0xffff];
}

inline hkRigidBody *havok_component_rigid_body_get(long rigid_body_index, s_havok_component const *component)
{
	return component->rigid_bodies[rigid_body_index].rigid_body;
}

/* the free-function spelling of s_havok_component::rigid_body_matrix_get */
inline void havok_component_rigid_body_matrix_get(long rigid_body_index, s_havok_component *component, struct transform4x3f *matrix)
{
	component->rigid_body_matrix_get(rigid_body_index, matrix);
}

/* the free-function spelling of s_havok_component::rigid_body_mass_get */
inline real havok_component_rigid_body_mass_get(long rigid_body_index, s_havok_component *component)
{
	return component->rigid_body_mass_get(rigid_body_index);
}

/* the free-function spelling of s_havok_component::rigid_body_keyframed */
inline bool havok_component_rigid_body_keyframed(long rigid_body_index, s_havok_component *component)
{
	return component->rigid_body_keyframed(rigid_body_index);
}

/* the component's main rigid body (unknown18), NONE when it has none */
inline short havok_component_main_rigid_body_index_get(s_havok_component const *component)
{
	short rigid_body_index;

	if (component->unknown18 >= 0 && component->unknown18 < component->rigid_bodies.size)
	{
		rigid_body_index = component->unknown18;
	}
	else
	{
		rigid_body_index = NONE;
	}
	return rigid_body_index;
}

#define HAVOK_PROPERTY_COMPONENT_INDEX 0x2001
#define HAVOK_PROPERTY_2002 0x2002
#define HAVOK_PROPERTY_2003 0x2003

#endif
