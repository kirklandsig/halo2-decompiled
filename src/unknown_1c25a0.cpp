// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_1C25A0.CPP: the physics (Havok) system's lifecycle callbacks
   (0x441624..0x441638 in the lifecycle table) */

#include "unknown_11c920.h"
#include "unknown_123b30.h"
#include "data_array.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "object_iterator.h"
#include "havok_reference.h"
#include <xtl.h>

extern bool g_47f05b;

struct s_1c4260
{
 byte *field_0;
 byte *field_4;
 byte field_8[8];
};
struct s_1c4261
{
 byte field_0[0x68];
 s_1c4260 *field_68;
 long field_6c;
};

PRIVATE __forceinline hkRigidBody *function_1c4261(byte *arg_0)
{
 long local_0 = *(long *)(arg_0 + 8);
 byte *local_1 = arg_0 - 0x10;
 return local_0 == 1 ? *(hkRigidBody **)(local_1 + 0x20) : NULL;
}
#include <stdio.h>
#include <stdarg.h>

class c_contact_shape_view
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual void slot2() = 0;
	virtual void slot3() = 0;
	virtual void slot4() = 0;
	virtual long type() = 0;
	long unknown04;
	long metadata;
};

struct s_contact_body_view
{
	c_contact_shape_view *shape;
	long key;
	long unknown08;
	s_contact_body_view const *parent;
	byte unknown10[8];
	long type;
	long unknown1c;
	hkEntity *entity;
};

class c_world_contact_filter
{
public:
	virtual ~c_world_contact_filter() {}
	virtual real evaluate(void const *body, void const *query) = 0;
};

class c_world_contact_update : public c_world_contact_filter
{
public:
	virtual real evaluate(void const *body, void const *query);
};

// @retail 0x1c2580 deleting c_world_contact_update

class c_world_query_filter
{
public:
	virtual hkBool accepts(s_contact_body_view const *body, void const *query) = 0;
};

struct s_world_query_view
{
	byte unknown00[0xd0];
	byte *filter;
};

extern hkWorld *g_51e9a4;
byte *__fastcall function_30c170(hkWorld *world);
long havok_entity_component_index_get(hkEntity const *entity);
void __stdcall function_b9b90(long object_index, bool disable);

// @retail 0x1c5690
real c_world_contact_update::evaluate(void const *body, void const *query)
{
	s_contact_body_view const *root = body ? (s_contact_body_view const *)((byte const *)body - 0x10) : NULL;
	c_world_query_filter *filter = (c_world_query_filter *)(((s_world_query_view *)g_51e9a4)->filter + 8);
	byte *local_0 = function_30c170(g_51e9a4) + 0xc;
	if (filter->accepts(root, local_0).m_bool && root->type == 1 && root->entity)
	{
		long component_index = havok_entity_component_index_get(root->entity);
		if (component_index != NONE)
			function_b9b90(havok_component_get(component_index)->object_index, false);
	}
	return 1.0f;
}

class c_contact_callback
{
public:
	virtual ~c_contact_callback() {}
	virtual void hit(void const *query, s_contact_body_view const *body) = 0;
	bool found;
	byte unknown05[3];
	static void operator delete(void *block)
	{
		g_480118->allocate((long)block, 8, 0x1a);
	}
};

class c_contact_presence : public c_contact_callback
{
public:
	virtual void hit(void const *query, s_contact_body_view const *body);
};

// @retail 0x1c24f0 deleting c_contact_presence

class c_contact_exclusion : public c_contact_callback
{
public:
	virtual void hit(void const *query, s_contact_body_view const *body);
	bool found_other;
	byte unknown09[3];
	long excluded_component;
};

PRIVATE __forceinline long contact_metadata_pin(long value, long lower, long upper)
{
	return value < lower ? lower : value > upper ? upper : value;
}

PRIVATE __forceinline bool contact_shape_has_material(s_contact_body_view const *body)
{
	if (body->shape->type() == 0x18)
		return false;
	long metadata = body->shape->metadata;
	if (!metadata)
		return false;
	if (contact_metadata_pin(metadata, 1, 16) == metadata)
		return false;
	return *(byte *)(metadata + 0x1e) != 0xff;
}

PRIVATE __forceinline short function_1d4884(long arg_0)
{
 return (short)(arg_0 < 1 ? 1 : arg_0 > 16 ? 16 : arg_0);
}

PRIVATE __forceinline bool function_1d4885(s_contact_body_view const *arg_0)
{
 if (arg_0->shape->type() == 0x18)
  return false;
 long local_0 = arg_0->shape->metadata;
 if (!local_0 || function_1d4884(local_0) == local_0)
  return false;
 return *(byte *)(local_0 + 0x1e) != 0xff;
}

// @retail 0x1d4880
void c_contact_presence::hit(void const *query, s_contact_body_view const *body)
{
	s_contact_body_view const *root = body;
	while (root->parent)
		root = root->parent;
	if (root->type == 1 && root->entity && !function_1d4885(body))
		found = true;
}

// @retail 0x1c5400
void c_contact_exclusion::hit(void const *query, s_contact_body_view const *body)
{
	s_contact_body_view const *root = body;
	while (root->parent)
		root = root->parent;
	if (root->type == 1 && root->entity)
	{
		long component_index = havok_entity_property_get(root->entity, HAVOK_PROPERTY_COMPONENT_INDEX);
		if (!contact_shape_has_material(body) && (excluded_component == NONE || excluded_component != component_index))
		{
			found = true;
			found_other = true;
		}
	}
}

bool havok_component_any_rigid_body_active(s_havok_component *component);
bool havok_component_rigid_body_keyframed(long rigid_body_index, s_havok_component *component);
void havok_component_rigid_body_linear_velocity_set(long rigid_body_index, s_havok_component *component, vector3f const *velocity);
void havok_component_rigid_body_angular_velocity_set(long rigid_body_index, s_havok_component *component, vector3f const *velocity);
void havok_component_rigid_bodies_activate(s_havok_component *component);

struct s_velocity_object_header
{
	short identifier;
	byte flags;
	byte type;
	byte unknown04[4];
	s_havok_object *object;
};

// @retail 0x1c4b00
void function_1c4b00(long object_index, void *linear, void *angular, long force)
{
	long component_index = *(volatile long const *)&havok_object_get(object_index)->havok_component_index;
	if (component_index != NONE)
	{
		s_havok_component *component = havok_component_get(component_index);
		if ((byte)force || havok_component_any_rigid_body_active(component))
		{
			if ((1 << ((s_velocity_object_header *)g_4e0300->data)[object_index & 0xffff].type) & 2)
			{
				long index = havok_component_main_rigid_body_index_get(component);
				if (index != NONE && !havok_component_rigid_body_get(index, component)->m_fixed &&
					!havok_component_rigid_body_keyframed(index, component))
				{
					if (linear)
						havok_component_rigid_body_linear_velocity_set(index, component, (vector3f *)linear);
					if (angular)
						havok_component_rigid_body_angular_velocity_set(index, component, (vector3f *)angular);
				}
			}
			else
			{
				for (long index = 0; index < component->rigid_bodies.size; index++)
				{
					hkRigidBody *body = havok_component_rigid_body_get(index, component);
					if (!body->m_fixed && body->m_motion->getType() != 6)
					{
						if (linear)
							havok_component_rigid_body_linear_velocity_set(index, component, (vector3f *)linear);
						if (angular)
							havok_component_rigid_body_angular_velocity_set(index, component, (vector3f *)angular);
					}
				}
			}
			if ((byte)force && TEST_FIELD_BIT(component->flag5))
				havok_component_rigid_bodies_activate(component);
		}
	}
}

/* the havok components (unknown_1cec30.cpp) */
void havok_components_initialize(void);

void game_state_initialize_1edbc0(void);
void function_2263c0(void);
void function_146b30(void);
void function_146b80(void);
void function_146de0(void);
void function_226440(void);

/* an aligned block of physical memory: the offset back to the allocation
   sits before it */
void *g_479888;

// @retail 0x1c25a0
void function_1c25a0(void)
{
	g_51e9a0 = (long *)function_123d40("havok", "havok", sizeof(long));
	*g_51e9a0 = 0;
	game_state_initialize_1edbc0();
	function_2263c0();
	havok_components_initialize();
	function_146b30();
	function_146b80();
}

PRIVATE __forceinline byte *function_1c2601(void *arg_0)
{
	byte *local_0 = (byte *)arg_0;
	return local_0 - ((long *)local_0)[-1];
}

// @retail 0x1c2600
void function_1c2600(void)
{
	byte *block;

	function_146de0();
	block = function_1c2601(g_479888);
	if (!VirtualFree(block, 0, MEM_RELEASE))
	{
		GetLastError();
	}
	g_479888 = NULL;
	data_dispose(g_51e9b8);
	g_51e9b8 = NULL;
	function_226440();
}
/* the havok components (0xa0 bytes; unknown_1cec30.cpp) as the physics
   update sees them: a list of contacts at +0x70, each 0x60 bytes */
struct s_havok_contact_state
{
	byte unknown00[0xa8];
	long time;
	word flags;
};

struct s_havok_contact_owner
{
	byte unknown00[0x44];
	s_havok_contact_state *state;
};

struct s_havok_contact
{
	byte unknown00[0x40];
	s_havok_contact_owner *owner;
	byte unknown44[0x60 - 0x44];
};

/* an hkArray of contacts */
struct s_havok_contact_array
{
	s_havok_contact *data;
	long size;
	dword capacity_and_flags;

	s_havok_contact &operator[](long index)
	{
		return data[index];
	}
};

struct s_havok_component_contacts
{
	byte unknown00[0x70];
	s_havok_contact_array contacts;
	byte unknown7c[0xa0 - 0x7c];
};

inline s_havok_component_contacts *havok_component_contacts_get(long component_index)
{
	return &((s_havok_component_contacts *)g_51e9b8->data)[component_index & 0xffff];
}

// @retail 0x1c3930
bool havok_object_type_can_have_component(long definition_index)
{
	byte type = *g_4e3b44[definition_index & 0xffff].bytes;
	bool result = true;

	if ((1 << type) & 0x1883)
	{
		if (!g_47f058)
		{
			result = *g_51e9a0 < 0x200;
		}
		else
		{
			result = g_51e9b8->actual_count < g_51e9b8->maximum_count;
		}
	}
	return result;
}

// @retail 0x1c3980
void havok_object_count(long object_index)
{
	s_havok_object *object = havok_object_get(object_index);

	if (!TEST_FIELD_BIT(object->havok_flag))
	{
		object->havok_flag = 1;
		(*g_51e9a0)++;
	}
}

// @retail 0x1c4fc0
void havok_component_contacts_mark1(long component_index)
{
	if (component_index != NONE)
	{
		s_havok_component_contacts *component = havok_component_contacts_get(component_index);
		long i;

		for (i = 0; i < component->contacts.size; i++)
		{
			s_havok_contact *contact = &component->contacts[i];
			s_havok_contact_state *state = contact->owner->state;

			if (state)
			{
				if (state->time != g_510c54->game_time)
				{
					state->flags = 0;
				}
				state->flags |= 1;
				state->time = g_510c54->game_time;
			}
		}
	}
}

// @retail 0x1c5040
void havok_component_contacts_mark2(long component_index)
{
	if (component_index != NONE)
	{
		s_havok_component_contacts *component = havok_component_contacts_get(component_index);
		long i;

		for (i = 0; i < component->contacts.size; i++)
		{
			s_havok_contact *contact = &component->contacts[i];
			s_havok_contact_state *state = contact->owner->state;

			if (state)
			{
				if (state->time != g_510c54->game_time)
				{
					state->flags = 0;
				}
				state->flags |= 2;
				state->time = g_510c54->game_time;
			}
		}
	}
}
#define PIN(value, lower, upper) ((value) < (lower) ? (lower) : (value) > (upper) ? (upper) : (value))

/* the game time of the last ... (NONE when unset) */
long g_47f054 = NONE;

// @retail 0x1c58a0
bool function_1c58a0(void)
{
	long time = g_47f054;
	bool result = false;

	if (time != NONE)
	{
		long game_time = g_510c54->game_time;

		long lower = game_time - 3;

		result = (time < lower ? lower : game_time < time ? game_time : time) == time;
	}
	return result;
}

/* a Havok collision body: its shape, the shape key in its parent, and the
   parent body */
struct s_havok_shape_view
{
	byte unknown00[8];
	dword user_data;
};

struct s_havok_cd_body
{
	s_havok_shape_view *shape;
	long shape_key;
	byte unknown08[4];
	s_havok_cd_body *parent;
};

// @retail 0x1c55b0
long havok_cd_body_shape_key_get(s_havok_cd_body const *body)
{
	long result = NONE;

	while (body->parent)
	{
		if (body->parent->shape->user_data == 0xcabcabb0)
		{
			break;
		}
		body = body->parent;
	}
	if (body->parent)
	{
		result = body->shape_key;
	}
	return result;
}

// @retail 0x1c4560
void havok_printf(char const *format, ...)
{
	char buffer[0x104];
	va_list arguments;

	va_start(arguments, format);
	_vsnprintf(buffer, 0xfe, format, arguments);
}
void havok_component_delete(long component_index);

// @retail 0x1c37f0
void havok_object_detach(long object_index)
{
	s_havok_object *object = havok_object_get(object_index);

	if (object->havok_component_index != NONE)
	{
		havok_component_delete(object->havok_component_index);
		object->havok_component_index = NONE;
	}
	object = havok_object_get(object_index);
	if (TEST_FIELD_BIT(object->havok_flag))
	{
		object->havok_flag = 0;
		(*g_51e9a0)--;
	}
}
/* the physics world and its counters */
hkWorld *g_51e9a4;
long g_47f050;

struct s_47f048_object;
extern s_47f048_object *g_47f048;
extern void *g_51ecac;

void function_278f00(void);
void function_146bf0(void);
long function_baf80(long object_index);
void function_0bfe40(word *flags, long bit, bool value);
long havok_component_new(long object_index);
void function_1cf120(long component_index);
struct s_havok_component;
void function_1d1260(s_havok_component *component);
void __stdcall function_1d01c0(s_havok_component *component);
void function_1d1540(s_havok_component *component);

/* the object headers as the physics code reads them */
struct s_physics_object_header
{
	short identifier;
	byte flags;
	byte type;
	short cluster_index;
	byte unknown06[2];
	struct s_physics_object *object;
};

struct s_physics_object
{
	long tag_index;
	byte unknown004[0xc - 0x4];
	long next_object_index;
	long first_child_index;
	byte unknown014[0xb4 - 0x14];
	long havok_component_index;
	byte unknown0b8[0xc0 - 0xb8];
	union
	{
		word flags;
		struct
		{
			word unknownc0_0 : 6;
			word bit6 : 1;
			word unknownc0_7 : 5;
			word bit12 : 1;
			word unknownc0_13 : 3;
		};
	};
	byte unknown0c2[0x11a - 0xc2];
	short region_variants_offset;
};

/* the havok components (unknown_1cec30.cpp) as the physics code reads them */
struct s_havok_component_flags
{
	short identifier;
	byte unknown02[2];
	union
	{
		dword flags;
		struct
		{
			dword unknown0 : 5;
			dword bit5 : 1;
			dword unknown6 : 5;
			dword bit11 : 1;
			dword unknown12 : 2;
			dword bit14 : 1;
			dword unknown15 : 2;
			dword bit17 : 1;
			dword unknown18 : 2;
			dword bit20 : 1;
			dword unknown21 : 11;
		};
	};
	long object_index;
	byte unknown0c[0xa0 - 0xc];
};

/* the model's regions (the object definition's model, +0x70) */
struct s_model_permutation_view
{
	byte unknown0[6];
	char region;
	byte unknown7;
};

struct s_model_region_view
{
	byte unknown00[0xc];
	s_model_permutation_view *permutations;
};

struct s_model_view
{
	byte unknown00[0x70];
	long region_count;
	s_model_region_view *regions;
};

struct s_object_definition_view
{
	byte unknown00[0x38];
	long model_tag_index;
};

PRIVATE inline s_physics_object_header *physics_object_header_get(long object_index)
{
	return &((s_physics_object_header *)g_4e0300->data)[object_index & 0xffff];
}

PRIVATE inline s_physics_object *physics_object_get(long object_index)
{
	return physics_object_header_get(object_index)->object;
}

PRIVATE inline s_havok_component_flags *havok_component_flags_get(long component_index)
{
	return &((s_havok_component_flags *)g_51e9b8->data)[component_index & 0xffff];
}

/* an object 0x1c2910 makes */
c_havok_reference_counted *g_4f55b0;

/* drops the references to 0x1c2910's object and to g_47f048 */
PRIVATE __forceinline void function_1c2891(short *arg_0, c_havok_reference_counted *arg_1)
{
	--*arg_0;
	if (*arg_0 == 0)
	{
		if (function_279740((long)arg_1))
			*arg_0 = 1;
		else
			delete arg_1;
	}
}

// @retail 0x1c2890
void function_1c2890(void)
{
	if (g_4f55b0)
	{
		havok_reference_remove(g_4f55b0);
		g_4f55b0 = NULL;
	}
	c_havok_reference_counted *local_0 = (c_havok_reference_counted *)g_47f048;
	function_1c2891(&local_0->reference_count, local_0);
	g_47f048 = NULL;
}

// @retail 0x1c29d0
void function_1c29d0(void)
{
	((hkEntityApi *)g_47f048)->removeEntityListener((hkEntityListener *)g_51ecac);
	g_47f050--;
	g_51e9a4->removeEntity((hkEntity *)g_47f048);
	function_278f00();
}

// @retail 0x1c35f0
void __stdcall function_1c35f0(long object_index)
{
	s_physics_object_header *header = physics_object_header_get(object_index);

	if (((1 << header->type) & 0x1883) && !(header->flags & 0x10))
	{
		long root_index = function_baf80(object_index);

		header->object->bit12 = false;
		if (physics_object_header_get(root_index)->cluster_index != NONE)
		{
			s_physics_object *object = header->object;

			if (g_47f058)
			{
				s_havok_component_flags *component;

				object->havok_component_index = havok_component_new(object_index);
				function_1cf120(object->havok_component_index);
				component = havok_component_flags_get(object->havok_component_index);
				if (TEST_FIELD_BIT(object->bit6))
				{
					function_1d1540((s_havok_component *)component);
				}
				function_0bfe40(&header->object->flags, 0xc,
					TEST_FIELD_BIT(component->bit14) || TEST_FIELD_BIT(component->bit20));
			}
			havok_object_count(object_index);
		}
	}
}

// @retail 0x1c36f0
void __stdcall function_1c36f0(long parent_index, long object_index)
{
	s_physics_object *object = physics_object_get(object_index);
	long child_index = object->first_child_index;

	if (object->havok_component_index == NONE)
	{
		function_146bf0();
		function_1c35f0(object_index);
		function_278f00();
		function_146bf0();
	}
	while (child_index != NONE)
	{
		s_physics_object *child = physics_object_get(child_index);

		if (child_index == parent_index)
		{
			break;
		}
		function_1c36f0(parent_index, child_index);
		child_index = child->next_object_index;
	}
}

/* sets the flags of the object's havok component and rebuilds it, with the
   component's active state handled around the change.
   Standard convention (see docs/DECOMPILING.md):
   1. Retail keeps it __stdcall (both arguments on the stack, ret 8). With the
      marker this body matches byte for byte; without it LTCG passes the
      object index in eax.
   2. No data or code in retail holds its address. Its twelve callers
      (0xdbc80 0xe0c70 0xe0ef0 0xe16b0 0xe18d0 0xe24f0 0x1c38a0 0x1c4040
      0x1d2460 0x1e54d0 0x1ec690 0x278f00) all push both arguments; 0x1d2460
      itself takes its component in ecx, a register convention.
   3. Tried: declaring it __stdcall alone does nothing under LTCG; /GL- on
      the file would break the register convention of its callee 0x1cf120,
      which takes the component index in eax. */
// @retail 0x1c3770 standard
void __stdcall function_1c3770(long object_index, dword flags)
{
	s_physics_object *object = physics_object_get(object_index);

	if (object->havok_component_index != NONE)
	{
		s_havok_component_flags *component = havok_component_flags_get(object->havok_component_index);
		bool active = TEST_FIELD_BIT(component->bit5);

		if (active)
		{
			function_1d1260((s_havok_component *)component);
		}
		function_1d01c0((s_havok_component *)component);
		component->flags = flags;
		function_1cf120(object->havok_component_index);
		if (active)
		{
			function_1d1540((s_havok_component *)component);
		}
	}
}

// @retail 0x1c3850
void function_1c3850(long object_index)
{
	s_physics_object *object = physics_object_get(object_index);

	if (object->havok_component_index != NONE &&
		!TEST_FIELD_BIT(havok_component_flags_get(object->havok_component_index)->bit17))
	{
		function_1c3770(object_index, 0);
	}
}

PRIVATE inline char model_permutation_region_get(s_model_region_view const *region, char permutation)
{
	return permutation == NONE ? NONE : region->permutations[permutation].region;
}

// @retail 0x1c54b0
void function_1c54b0(long object_index, char const *variants)
{
	s_physics_object *object = physics_object_get(object_index);

	if (object->havok_component_index != NONE &&
		TEST_FIELD_BIT(havok_component_flags_get(object->havok_component_index)->bit11))
	{
		s_object_definition_view *definition = (s_object_definition_view *)g_4e3b44[object->tag_index & 0xffff].bytes;
		s_model_view *model = (s_model_view *)g_4e3b44[definition->model_tag_index & 0xffff].bytes;
		char const *current = (char const *)object + object->region_variants_offset;
		long i;

		for (i = 0; i < model->region_count; i++)
		{
			if (variants[i] != current[i] &&
				model_permutation_region_get(&model->regions[i], variants[i]) != model_permutation_region_get(&model->regions[i], current[i]))
			{
				function_1c3770(object_index, 0);
				break;
			}
		}
	}
}


// @retail 0x1c50c0
void function_1c50c0(void)
{
	long island_index;

	for (island_index = 0; island_index < g_51e9a4->m_island_count; )
	{
		hkSimulationIsland *island = g_51e9a4->m_islands[island_index];
		long entity_count = island->m_entity_count;
		long count = 0;
		volatile bool removed = false;
		long i;

		for (i = 0; i < entity_count; i++)
		{
			long component_index = havok_entity_property_get(island->m_entities[i], HAVOK_PROPERTY_COMPONENT_INDEX);

			if (component_index != NONE &&
				(physics_object_get(havok_component_flags_get(component_index)->object_index)->flags & 0x100))
			{
				count++;
			}
		}
		if (count == entity_count)
		{
			g_51e9a4->removeSimulationIsland(island);
			removed = true;
		}
		byte local_0 = *(volatile byte const *)&removed;
		island_index += local_0 == 0;
	}
}

// @retail 0x1c2a10
void function_1c2a10(void)
{
	s_type_f1af8e iterator;

	function_bae80(&iterator, 0, 0);
	while (function_baeb0(&iterator))
	{
		physics_object_get(iterator.object_index)->havok_component_index = NONE;
	}
	function_bae80(&iterator, 0, 0);
	while (function_baeb0(&iterator))
	{
		long object_index = iterator.object_index;

		if (physics_object_get(object_index)->havok_component_index == NONE)
		{
			function_146bf0();
			function_1c35f0(object_index);
			function_278f00();
			function_146bf0();
		}
	}
	function_1c50c0();
	g_47f054 = g_510c54->game_time;
}

bool function_1c4040(long attempt, bool active, bool any_object, bool even_if_unknown, long excluded_component_index);

// @retail 0x1c51c0
void function_1c51c0(long component_index)
{
	bool force = false;
	long attempt = 0;

	while (g_47f050 > 0x34e)
	{
		if (!function_1c4040(attempt, force, false, true, component_index))
		{
			if (attempt < 2)
			{
				attempt++;
			}
			else if (!force)
			{
				force = true;
				attempt = 0;
			}
		}
	}
}

/* the object as function_1c3f30 reads it */
struct s_physics_object_flags_view
{
	byte unknown000[4];
	dword unknown004_0 : 14;
	dword bit14 : 1;
	dword unknown004_15 : 17;
	byte unknown008[0x13c - 0x8];
	long unknown13c;
};

/* what function_1c4040 reads at +0xb4 of g_4e034c */
struct s_physics_effect_globals
{
	byte unknown0[4];
	long effect_index;
};

struct s_tag_header_globals_physics_view
{
	byte unknown00[0xb4];
	s_physics_effect_globals *effect;
};

/* an hkArray of simulation islands */
struct s_simulation_island_array
{
	hkSimulationIsland **data;
	long count;
	dword capacity_and_flags;
};

struct s_physics_world_view
{
	byte unknown00[8];
	s_simulation_island_array active_islands;
	s_simulation_island_array inactive_islands;
	byte unknown20[0x2c - 0x20];
	hkSimulationIsland *fixed_island;
};

struct s_physics_object_detach_view
{
	byte unknown000[0x30];
	point3f position;
	byte unknown03c[0x70 - 0x3c];
	vector3f velocity;
	byte unknown07c[0xd4 - 0x7c];
	long unknownd4;
};

struct s_physics_entity_view
{
	byte unknown00[0x8c];
	long priority;
};

bool function_a7670(long object_index);
#include "unknown_1765e0.h"
void __stdcall function_b8540(long object_index);
void havok_object_detach(long object_index);

PRIVATE __forceinline long function_1c3f31(hkEntity const *arg_0)
{
	long local_0;
	long local_1;
	for (local_1 = 0; local_1 < arg_0->m_property_count; local_1++)
	{
		if (arg_0->m_properties[local_1].m_key == HAVOK_PROPERTY_COMPONENT_INDEX)
		{
			local_0 = arg_0->m_properties[local_1].m_value.m_data;
			goto local_2;
		}
	}
	local_0 = 0;
local_2:
	return local_0;
}

// @retail 0x1c3f30
long function_1c3f30(hkEntity *entity, long attempt, bool any_object, bool even_if_unknown, long excluded_component_index)
{
	long local_0 = function_1c3f31(entity);
	long result = NONE;

	if (local_0 != NONE)
	{
		long component_index = function_1c3f31(entity);

		if (component_index != excluded_component_index)
		{
			long object_index = havok_component_flags_get(component_index)->object_index;

			if (!function_a7670(object_index) || even_if_unknown)
			{
				s_physics_object_header *header = physics_object_header_get(object_index);
				bool local_1 = *(volatile bool const *)&any_object;
				bool unknown = TEST_FIELD_BIT(((s_physics_object_flags_view *)header->object)->bit14);

				if (local_1 || (header->flags & 1))
				{
					if (unknown)
					{
						result = object_index;
					}
					else if ((1 << header->type) & 3)
					{
						if (attempt >= 2 && ((s_physics_object_flags_view *)*(s_physics_object *volatile const *)&header->object)->unknown13c == NONE)
						{
							result = object_index;
							goto local_3;
						}
					}
					else if (((1 << header->type) & 0x800) && attempt >= 1)
					{
						result = object_index;
					}
				}
			}
		}
	}
local_3:
	return result;
}

struct s_1c4040
{
	byte field_0[3];
	volatile bool field_3;
	volatile long field_4;
	long field_8;
	s_simulation_island_array *field_c;
	long field_10;
};

// @retail 0x1c4040
bool function_1c4040(long attempt, bool active, bool any_object, bool even_if_unknown, long excluded_component_index)
{
	s_1c4040 local_0;
	s_physics_world_view *world = (s_physics_world_view *)g_51e9a4;
	*(s_simulation_island_array *volatile *)&local_0.field_c = active ? &world->active_islands : &world->inactive_islands;
	local_0.field_4 = 0x80000000;
	local_0.field_8 = NONE;
	local_0.field_3 = false;
	long i;

	if (!active)
	{
		hkSimulationIsland *island = world->fixed_island;

		for (i = 0; i < island->m_entity_count; i++)
		{
			hkEntity *entity = island->m_entities[i];
			long priority = ((s_physics_entity_view *)entity)->priority;

			if (priority > local_0.field_4)
			{
				long object_index = function_1c3f30(entity, attempt, any_object, even_if_unknown, excluded_component_index);

				if (object_index != NONE)
				{
					local_0.field_4 = priority;
					local_0.field_8 = object_index;
				}
			}
		}
	}
	for (local_0.field_10 = 0; local_0.field_10 < local_0.field_c->count; local_0.field_10++)
	{
		hkSimulationIsland *island = local_0.field_c->data[local_0.field_10];

		for (i = 0; i < island->m_entity_count; i++)
		{
			hkEntity *entity = island->m_entities[i];
			long priority = ((s_physics_entity_view *)entity)->priority;

			if (priority > local_0.field_4)
			{
				long object_index = function_1c3f30(entity, attempt, any_object, even_if_unknown, excluded_component_index);

				if (object_index != NONE)
				{
					local_0.field_4 = priority;
					local_0.field_8 = object_index;
				}
			}
		}
	}
	if (local_0.field_8 != NONE)
	{
		s_physics_effect_globals *effect = ((s_tag_header_globals_physics_view *)g_4e034c)->effect;
		s_physics_object *object;
		bool unknown;

		if (g_4e6948->mode == 4 && ((s_physics_object_detach_view *)physics_object_get(local_0.field_8))->unknownd4 != NONE)
		{
			physics_object_get(local_0.field_8)->flags |= 0x20;
			function_1c3770(local_0.field_8, 0);
			local_0.field_3 = true;
			goto local_0;
		}
		object = physics_object_get(local_0.field_8);
		if (effect->effect_index != NONE)
		{
			function_1765e0(&((s_physics_object_detach_view *)object)->position, &((s_physics_object_detach_view *)object)->velocity,
				g_4687b0, effect->effect_index, 0, true);
		}
		object = physics_object_get(local_0.field_8);
		if (object->havok_component_index != NONE)
		{
			function_1d1260((s_havok_component *)havok_component_flags_get(object->havok_component_index));
		}
		object->bit6 = false;
		unknown = TEST_FIELD_BIT(physics_object_get(local_0.field_8)->bit6);
		if (unknown)
		{
			function_146bf0();
		}
		havok_object_detach(local_0.field_8);
		if (unknown)
		{
			function_278f00();
			function_146bf0();
		}
		function_b8540(local_0.field_8);
		local_0.field_3 = true;
	}
local_0:
	return local_0.field_3;
}


void function_bba20(long object_index);

PRIVATE __forceinline void function_1c4262(long arg_0)
{
 s_havok_object *local_0 = havok_object_get(arg_0);
 if (local_0->havok_component_index != NONE)
 {
  long local_1 = local_0->havok_component_index;
  s_havok_component *local_2 = havok_component_get(local_1);
  long local_3 = local_2->object_index;
  local_2->~s_havok_component();
  record_pool_release(g_51e9b8, local_1);
  s_havok_object *local_4 = havok_object_get(local_3);
  if (TEST_FIELD_BIT(local_4->havok_flag))
  {
   local_4->havok_flag = 0;
   --*g_51e9a0;
  }
  local_0->havok_component_index = NONE;
 }
 local_0 = havok_object_get(arg_0);
 if (TEST_FIELD_BIT(local_0->havok_flag))
 {
  local_0->havok_flag = 0;
  --*g_51e9a0;
 }
}

// @retail 0x1c4260
void function_1c4260()
{
 if (g_51e9a4 && !g_47f05b)
 {
  g_47f05b = true;
  for (long local_0 = 0; local_0 < g_51e9a4->m_island_count; ++local_0)
  {
   hkSimulationIsland *local_1 = g_51e9a4->m_islands[local_0];
   long local_2 = ((s_1c4261 *)local_1)->field_6c;
   if (local_2 > 24)
   {
    for (long local_3 = 0; local_3 < ((s_1c4261 *)local_1)->field_6c; ++local_3)
    {
     s_1c4260 *local_4 = &((s_1c4261 *)local_1)->field_68[local_3];
     hkRigidBody *local_5 = function_1c4261(local_4->field_0);
     hkRigidBody *local_6 = function_1c4261(local_4->field_4);
     if (local_5->m_motion->getType() == 7 || local_5->m_motion->getType() == 6 ||
         local_6->m_motion->getType() == 7 || local_6->m_motion->getType() == 6)
      --local_2;
    }
    if (local_2 > 24)
    {
     long local_7 = NONE;
     for (long local_8 = 0; local_8 < 3 && local_7 == NONE; ++local_8)
     {
      long local_9 = 0x80000000;
      for (long local_10 = 0; local_10 < local_1->m_entity_count; ++local_10)
      {
       hkEntity *local_11 = local_1->m_entities[local_10];
       long local_12 = ((s_physics_entity_view *)local_11)->priority;
       if (local_12 > local_9)
       {
        long local_13 = function_1c3f30(local_11, local_8, true, false, NONE);
        if (local_13 != NONE)
        {
         local_9 = local_12;
         local_7 = local_13;
        }
       }
      }
     }
     if (local_7 != NONE)
     {
      s_physics_effect_globals *local_14 = ((s_tag_header_globals_physics_view *)g_4e034c)->effect;
      s_physics_object *local_15 = physics_object_get(local_7);
      if (local_14->effect_index != NONE)
       function_1765e0(&((s_physics_object_detach_view *)local_15)->position,
        &((s_physics_object_detach_view *)local_15)->velocity, g_4687b0, local_14->effect_index, 0, true);
      local_15 = physics_object_get(local_7);
      if (local_15->havok_component_index != NONE)
       function_1d1260((s_havok_component *)havok_component_flags_get(local_15->havok_component_index));
      local_15->bit6 = false;
      bool local_16 = TEST_FIELD_BIT(physics_object_get(local_7)->bit6);
      if (local_16)
       function_146bf0();
      function_1c4262(local_7);
      if (local_16)
      {
       function_278f00();
       function_146bf0();
      }
      function_b8540(local_7);
     }
    }
   }
  }
  g_47f05b = false;
 }
}

// @retail 0x1c4a20
void function_1c4a20(hkEntity const *entity)
{
	long component_index = havok_entity_property_get(entity, HAVOK_PROPERTY_COMPONENT_INDEX);
	s_havok_component *component = havok_component_get(component_index);
	if (TEST_FIELD_BIT(component->flag5))
		function_bba20(component->object_index);
}


struct s_world_bounds_query
{
	hkVector4 lower;
	hkVector4 upper;
	hkVector4 half_extent;
	long flags;
};

class c_world_bounds_search
{
public:
	virtual void slot00() = 0;
	virtual void slot01() = 0;
	virtual void slot02() = 0;
	virtual void slot03() = 0;
	virtual void slot04() = 0;
	virtual void slot05() = 0;
	virtual void slot06() = 0;
	virtual void slot07() = 0;
	virtual void slot08() = 0;
	virtual void slot09() = 0;
	virtual void slot0a() = 0;
	virtual void slot0b() = 0;
	virtual void slot0c() = 0;
	virtual void slot0d() = 0;
	virtual void slot0e() = 0;
	virtual void search(s_world_bounds_query const *query, c_world_contact_filter *filter) = 0;
};

struct s_world_bounds_view
{
	byte unknown00[0xc4];
	c_world_bounds_search *search;
};

// @retail 0x1c55e0
void function_1c55e0(real const *bounds)
{
	s_world_bounds_query query;
	real *local_0 = (real *)&query.lower;
    real *local_1 = (real *)&query.upper;
    real local_2 = *(volatile real const *)&bounds[0];
    local_1[0] = *(volatile real const *)&bounds[1];
    real local_3 = *(volatile real const *)&bounds[3];
    local_0[0] = local_2;
    real local_4 = *(volatile real const *)&bounds[2];
    local_1[1] = local_3;
    real local_5 = *(volatile real const *)&bounds[5];
    local_0[1] = local_4;
    real local_6 = *(volatile real const *)&bounds[4];
    local_0[2] = local_6;
    local_1[2] = local_5;
    local_0[3] = 0.0f;
    local_1[3] = 0.0f;
	query.flags = 0;
	__m128 half = _mm_set_ss(0.5f);
	query.half_extent.m_quad = _mm_mul_ps(_mm_shuffle_ps(half, half, 0), _mm_sub_ps(query.upper.m_quad, query.lower.m_quad));
	c_world_contact_update filter;
	((s_world_bounds_view *)g_51e9a4)->search->search(&query, &filter);
}


class hkShape;
bool function_182180(hkShape const *shape);

class c_contact_shape_container
{
public:
 virtual void slot00() = 0;
 virtual void slot01() = 0;
 virtual void slot02() = 0;
 virtual void slot03() = 0;
 virtual void slot04() = 0;
 virtual void slot05() = 0;
 virtual void slot06() = 0;
 virtual void slot07() = 0;
 virtual void slot08() = 0;
 virtual void slot09() = 0;
 virtual void slot0a() = 0;
 virtual void slot0b() = 0;
 virtual void slot0c() = 0;
 virtual c_contact_shape_view *child(long key, void *buffer) = 0;
};

class c_contact_rule_fallback
{
public:
 hkBool accepts(long a, long b);
};

class c_contact_rule_filter
{
public:
 virtual hkBool accepts(void const *query, s_contact_body_view const *a, s_contact_body_view const *b,
  c_contact_shape_container *container, long key);
};

// @retail 0x1c32d0
hkBool c_contact_rule_filter::accepts(void const *query, s_contact_body_view const *a, s_contact_body_view const *b,
 c_contact_shape_container *container, long key)
{
 __m128 buffer[16];
 s_contact_body_view const *root = a;
 while (root->parent)
  root = root->parent;
 if ((hkEntity *)g_47f048 == root->entity)
 {
  c_contact_shape_view *shape = key != NONE ? container->child(key, buffer) : b->shape;
  if (shape->type() == 0x15)
   shape = *(c_contact_shape_view **)((byte *)shape + 0xc);
  if (shape->type() == 0x17)
   shape = *(c_contact_shape_view **)((byte *)shape + 0x30);
  if (shape->type() != 0x18 && function_182180((hkShape *)shape))
  {
   long metadata = shape->metadata;
   if (metadata && contact_metadata_pin(metadata, 1, 16) != metadata)
   {
    byte flag = *(byte *)(metadata + 0x1f) & 1;
    volatile byte saved_flag = flag;
    if (flag)
    {
     root = a;
     while (root->parent)
      root = root->parent;
     if (root->type == 1 && root->entity && ((hkRigidBody *)root->entity)->m_motion->getType() == 7)
     {
      hkBool result;
      result.m_bool = 0;
      return result;
     }
    }
   }
  }
 }
 s_contact_body_view const *root_b = b;
 while (root_b->parent)
  root_b = root_b->parent;
 long key_b = root_b->unknown1c;
 root = a;
 while (root->parent)
  root = root->parent;
 return ((c_contact_rule_fallback *)((byte *)this - 0xc))->accepts(root->unknown1c, key_b);
}


long function_183c60(long object_index, long node_index);
void havok_component_rigid_body_linear_velocity_change(long index, s_havok_component *component, vector3f const *change);
void havok_component_rigid_body_linear_velocity_add(long index, s_havok_component *component, vector3f const *change);
void havok_component_rigid_body_point_impulse_apply(long index, s_havok_component *component, point3f const *point, vector3f const *change);

struct s_contact_scale_definition
{
 byte unknown00[0x14];
 real scale;
};

PRIVATE inline void contact_vector_set(vector3f *out, real i, real j, real k)
{
 out->i = i;
 out->j = j;
 out->k = k;
}

PRIVATE inline void contact_motion_vector(vector3f *out, hkVector4 const *value)
{
 contact_vector_set(out, (*value)(0), (*value)(1), (*value)(2));
}

// @retail 0x1c4c50
bool __stdcall function_1c4c50(long object_index, long node_index, point3f const *point,
 vector3f const *change_a, vector3f const *change_b, long *result_object, vector3f *linear, vector3f *angular)
{
 // Retail keeps all eight inputs on the stack.
 (void)&object_index;
 (void)&node_index;
 (void)&point;
 (void)&change_a;
 (void)&change_b;
 (void)&result_object;
 (void)&linear;
 (void)&angular;
 s_velocity_object_header *headers = (s_velocity_object_header *)g_4e0300->data;
 s_havok_object *object = headers[object_index & 0xffff].object;
 bool result = false;
 if (object->havok_component_index != NONE)
 {
  long root_index = function_baf80(object_index);
  s_havok_object *root = headers[root_index & 0xffff].object;
  vector3f a;
  vector3f b;
  if (change_a)
   a = *change_a;
  if (change_b)
   b = *change_b;
  if (root->havok_component_index != NONE)
  {
   s_havok_component *component = havok_component_get(root->havok_component_index);
   if (root_index != object_index)
   {
    s_contact_scale_definition *definition = (s_contact_scale_definition *)g_4e3b44[*(long *)object & 0xffff].bytes;
    if (definition->scale > 0.001f)
    {
     s_contact_scale_definition *root_definition = (s_contact_scale_definition *)g_4e3b44[*(long *)root & 0xffff].bytes;
     real scale = root_definition->scale / definition->scale;
     if (change_a)
     {
      a.i *= scale;
      a.j *= scale;
      a.k *= scale;
     }
     if (change_b)
     {
      b.i *= scale;
      b.j *= scale;
      b.k *= scale;
     }
    }
   }
   if (!havok_component_any_rigid_body_active(component))
    havok_component_rigid_bodies_activate(component);
   long indices[64];
   long count;
   if (node_index != NONE && root_index == object_index)
   {
    long index = function_183c60(root_index, node_index);
    if (index != NONE)
    {
     count = 1;
     indices[0] = index;
     goto selected;
    }
   }
   long local_1;
   local_1 = component->rigid_bodies.size;
   for (long i = 0; i < local_1; ++i)
    indices[i] = i;
   count = local_1;
selected:
   volatile long local_0 = 0;
   for (long i = 0; i < count;)
   {
    long index = indices[i];
    hkRigidBody *body = havok_component_rigid_body_get(index, component);
    if (!body->m_fixed && body->m_motion->getType() != 6)
    {
     if (point)
     {
      if (change_a)
      {
       if (!TEST_FIELD_BIT(component->flag1))
       {
        havok_component_rigid_body_point_impulse_apply(index, component, point, &a);
        i = local_0;
       }
       else
        havok_component_rigid_body_linear_velocity_change(index, component, &a);
      }
     }
     else if (change_a)
      havok_component_rigid_body_linear_velocity_change(index, component, &a);
     if (change_b && !TEST_FIELD_BIT(component->flag1))
      havok_component_rigid_body_linear_velocity_add(index, component, &b);
     if (havok_component_main_rigid_body_index_get(component) == index)
     {
      *result_object = component->object_index;
      body = havok_component_rigid_body_get(index, component);
      if (!body->m_fixed)
       contact_motion_vector(linear, &body->m_motion->m_linear_velocity);
      else
       *linear = *g_4687a4;
      body = havok_component_rigid_body_get(index, component);
      if (!body->m_fixed)
       contact_motion_vector(angular, &body->m_motion->m_angular_velocity);
      else
       *angular = *g_4687a4;
      result = true;
     }
    }
    ++i;
    local_0 = i;
   }
  }
 }
 return result;
}

class c_world_callback_registration
{
public:
    void remove(void *callback);
};

c_havok_reference_counted *g_51e9a8;
c_havok_reference_counted *g_51e9ac;

// @retail 0x1c34a0
void function_1c34a0(void)
{
    if (g_51e9a8)
    {
        ((c_world_callback_registration *)g_51e9a4)->remove(g_51e9a8);
        delete g_51e9a8;
        g_51e9a8 = NULL;
    }
    havok_reference_remove((c_havok_reference_counted *)g_51e9a4);
    g_51e9a4 = NULL;
    havok_reference_remove(g_51e9ac);
    g_51e9ac = NULL;
}

#include <float.h>

class c_contact_query_allocator
{
public:
    virtual void slot0() = 0;
    virtual void slot1() = 0;
    virtual void slot2() = 0;
    virtual void slot3() = 0;
    virtual void *allocate(long size, long kind) = 0;
};

PRIVATE __forceinline void *contact_query_allocate(long size)
{
    void *block = ((c_contact_query_allocator *)g_480118)->allocate(size, 0x2c);
    ((c_havok_reference_counted *)block)->allocation_size = (word)size;
    return block;
}

struct c_contact_query_bounds_info
{
    long filter;
    void *shape;
    long unknown08;
    s_havok_array properties;
    byte unknown18[8];
    hkVector4 lower, upper;
    c_contact_query_bounds_info();
    ~c_contact_query_bounds_info()
    {
        if (!(properties.capacity_and_flags & 0x80000000))
            g_480118->allocate((long)properties.data, (properties.capacity_and_flags & 0x7fffffff) * 8, 0x12);
    }
};

struct c_contact_query_transform_info
{
    long filter;
    void *shape;
    long unknown08;
    s_havok_array properties;
    byte unknown18[8];
    hkTransform transform;
    c_contact_query_transform_info();
    ~c_contact_query_transform_info()
    {
        if (!(properties.capacity_and_flags & 0x80000000))
            g_480118->allocate((long)properties.data, (properties.capacity_and_flags & 0x7fffffff) * 8, 0x12);
    }
};

class c_contact_query_bounds_volume
{
public:
    c_contact_query_bounds_volume(c_contact_query_bounds_info const *info);
    byte unknown00[0x80];
    s_contact_body_view **bodies;
    long count;
    byte unknown88[8];
    static void *operator new(size_t size) { return contact_query_allocate(size); }
};

class c_contact_query_transform_volume
{
public:
    c_contact_query_transform_volume(c_contact_query_transform_info const *info);
    byte unknown00[0xd0];
    static void *operator new(size_t size) { return contact_query_allocate(size); }
};

class c_contact_query_dispatch
{
public:
    virtual void slot00() = 0;
    virtual void slot01() = 0;
    virtual void slot02() = 0;
    virtual void slot03() = 0;
    virtual void slot04() = 0;
    virtual void slot05() = 0;
    virtual void slot06() = 0;
    virtual void slot07() = 0;
    virtual void slot08() = 0;
    virtual void slot09() = 0;
    virtual void slot0a() = 0;
    virtual void slot0b() = 0;
    virtual void query(c_contact_callback *callback) = 0;
};

class c_contact_query_world
{
public:
    void add(void *volume);
    void remove(void *volume);
};

struct s_contact_query_object
{
    byte unknown00[0x30];
    point3f center;
    real radius;
};

extern real g_47f05c;

PRIVATE __forceinline void contact_query_center(s_contact_query_object const *object, point3f *center)
{
    *center = object->center;
}

// @retail 0x1c5710
void function_1c5710(long object_index)
{
    c_contact_query_bounds_info info;
    s_contact_query_object *object = (s_contact_query_object *)havok_object_get(object_index);
    real radius = g_47f05c * 2.0f + object->radius;
    point3f center;
    contact_query_center(object, &center);
    info.lower.set(center.x - radius, center.y - radius, center.z - radius);
    info.upper.set(center.x + radius, center.y + radius, center.z + radius);
    c_contact_query_bounds_volume *volume = new c_contact_query_bounds_volume(&info);
    ((c_contact_query_world *)g_51e9a4)->add(volume);
    for (long i = 0; i < volume->count; ++i)
    {
        s_contact_body_view *body = volume->bodies[i];
        if (body->type == 1 && body->entity && !((hkRigidBody *)body->entity)->m_fixed)
            ((hkRigidBody *)body->entity)->activate();
    }
    ((c_contact_query_world *)g_51e9a4)->remove(volume);
    havok_reference_remove((c_havok_reference_counted *)volume);
}

// @retail 0x1c5210
bool function_1c5210(transform4x3f const *matrix, void *shape, long excluded_component, long filter)
{
    (void)&shape;
    (void)&excluded_component;
    (void)&filter;
    c_contact_query_transform_info info;
    info.transform.m_rotation.m_col0.set(matrix->forward.i, matrix->forward.j, matrix->forward.k);
    info.transform.m_rotation.m_col1.set(matrix->left.i, matrix->left.j, matrix->left.k);
    info.transform.m_rotation.m_col2.set(matrix->up.i, matrix->up.j, matrix->up.k);
    hkVector4 position;
    position.set(matrix->position.x, matrix->position.y, matrix->position.z);
    info.shape = shape;
    c_contact_exclusion callback;
    callback.found = false;
    callback.found_other = false;
    callback.excluded_component = excluded_component;
    info.filter = filter;
    info.transform.m_translation = position;
    c_contact_query_transform_volume *volume = new c_contact_query_transform_volume(&info);
    _control87(0x9001f, 0x8001f);
    _mm_setcsr(_mm_getcsr() | 0x1f80);
    ((c_contact_query_world *)g_51e9a4)->add(volume);
    ((c_contact_query_dispatch *)volume)->query(&callback);
    bool result = callback.found_other;
    ((c_contact_query_world *)g_51e9a4)->remove(volume);
    _mm_setcsr(_mm_getcsr() & 0xffffffc0);
    _clearfp();
    _control87(0x9001f, 0xfffff);
    havok_reference_remove((c_havok_reference_counted *)volume);
    return result;
}

class hkMemory;
extern hkMemory *g_479894;
extern hkMemory *g_479898;
extern c_havok_fixed_memory *g_4798a0;
short g_51ecb0;

struct s_fixed_memory_statistics
{
    long header_size, allocated_size, unknown08, allocation_count, unknown10;
};
struct s_fixed_memory_statistics_16
{
    long header_size, allocated_size, allocation_count, unknown0c;
};
struct s_physics_pool_statistics
{
    long allocated, pages, used_pages, page_size, free_size;
    real used_fraction;
    struct s_entry { long size, stride, count, used; } entries[16];
    long metadata_size, entry_count;
};
struct s_2797a0_groups;
struct s_2797a0_iterator
{
    byte unknown00;
    bool second;
    byte unknown02[2];
    long group;
    long index;
    s_2797a0_groups *groups;
    long mode;
};

class c_physics_statistics_allocator
{
public:
    virtual void slot0() = 0;
    virtual void slot1() = 0;
    virtual void slot2() = 0;
    virtual void slot3() = 0;
    virtual void slot4() = 0;
    virtual void slot5() = 0;
    virtual void slot6() = 0;
    virtual void statistics(long *result) = 0;
};

bool function_2797a0(s_2797a0_iterator *iterator);
void fixed_memory_get_statistics(c_havok_fixed_memory *memory, s_fixed_memory_statistics *statistics);
void fixed_memory_get_statistics_16(c_havok_fixed_memory *memory, s_fixed_memory_statistics_16 *statistics);
void function_22c390(hkMemory *memory, s_fixed_memory_statistics_16 *statistics);
void function_22cb00(hkMemory *memory, s_physics_pool_statistics *statistics);
long function_1d3880(s_havok_component const *component, long *constraints, long *contacts, long *other, long *bodies);

// @retail 0x1c4590
void __stdcall function_1c4590(long flags)
{
    byte *world = (byte *)g_51e9a4;
    long active_islands = *(long *)(world + 0xc);
    long inactive_islands = *(long *)(world + 0x18);
    long fixed_entities = *(long *)(*(byte **)(world + 0x2c) + 0x40);
    long allocator_statistics[4];
    ((c_physics_statistics_allocator *)g_480118)->statistics(allocator_statistics);
    long active_entities = 0, inactive_entities = 0;
    s_2797a0_iterator iterator;
    ++g_51ecb0;
    iterator.groups = (s_2797a0_groups *)g_51e9a4;
    iterator.mode = 0;
    iterator.second = false;
    iterator.group = NONE;
    iterator.index = NONE;
    iterator.unknown00 = 1;
    while (function_2797a0(&iterator))
        ++active_entities;
    iterator.groups = (s_2797a0_groups *)g_51e9a4;
    iterator.mode = 1;
    iterator.second = false;
    iterator.group = NONE;
    iterator.index = NONE;
    iterator.unknown00 = 1;
    while (function_2797a0(&iterator))
        ++inactive_entities;
    --g_51ecb0;
    havok_printf("Havok Performance Stats");
    havok_printf("\t simulation islands (active/inactive):   %d/%d", active_islands, inactive_islands);
    havok_printf("\t fixed entities:                         %d", fixed_entities);
    havok_printf("\t dynamic entities (active/inactive):     %d/%d", active_entities, inactive_entities);
    s_fixed_memory_statistics physical;
    s_fixed_memory_statistics_16 scratch, startup;
    s_physics_pool_statistics pool;
    fixed_memory_get_statistics(g_4798a0, &physical);
    fixed_memory_get_statistics_16(g_47989c, &scratch);
    function_22cb00(g_479898, &pool);
    function_22c390(g_479894, &startup);
    havok_printf("Havok Memory (total %.2f KB, debug %.2f KB, percent used %.3f)", 1045.109375, (startup.unknown0c + scratch.unknown0c + pool.metadata_size) * (1.0f / 1024.0f), pool.used_fraction * 100.0f);
    havok_printf("Startup pool: mem(%.2f KB/%.2f KB), allocs(%d)", startup.allocated_size * (1.0f / 1024.0f), startup.header_size * (1.0f / 1024.0f), startup.allocation_count);
    havok_printf("Overflow pool: mem(%.2f KB/%.2f KB), allocs(%d))", scratch.allocated_size * (1.0f / 1024.0f), scratch.header_size * (1.0f / 1024.0f), scratch.allocation_count);
    havok_printf("2nd Overflow pool: mem(%.2f KB/%.2f KB), allocs(%d))", physical.allocated_size * (1.0f / 1024.0f), physical.header_size * (1.0f / 1024.0f), physical.allocation_count);
    havok_printf("Runtime Pool(%.2f KB): usage==pages(%d/%d)*size(%.2f KB)+last(%.2f KB)", (pool.allocated - pool.metadata_size) * (1.0f / 1024.0f), pool.used_pages, pool.pages, pool.page_size * (1.0f / 1024.0f), (pool.page_size - pool.free_size) * (1.0f / 1024.0f));
    for (long i = 0; i < pool.entry_count; ++i)
    {
        s_physics_pool_statistics::s_entry *entry = &pool.entries[i];
        havok_printf("\t b%d(%d) (%d / %d (*%d))", i, entry->size, entry->used, entry->count, entry->stride);
    }
    long contacts_total = 0, bodies_total = 0, constraints_total = 0, other_total = 0;
    long contacts_maximum = 0, bodies_maximum = 0, constraints_maximum = 0, other_maximum = 0;
    long index = NONE;
    for (;;)
    {
        index = data_next_absolute_index_inlined(g_51e9b8, index + 1);
        if (index == NONE)
            break;
        s_havok_component *component = (s_havok_component *)(g_51e9b8->data + g_51e9b8->size * index);
        if (!component)
            break;
        long contacts, constraints, other, bodies;
        function_1d3880(component, &contacts, &constraints, &other, &bodies);
        contacts_total += contacts;
        bodies_total += bodies;
        constraints_total += constraints;
        other_total += other;
        if (contacts_maximum <= contacts) contacts_maximum = contacts;
        if (bodies_maximum <= bodies) bodies_maximum = bodies;
        if (constraints_maximum <= constraints) constraints_maximum = constraints;
        if (other_maximum <= other) other_maximum = other;
    }
    havok_printf("Havok Component Meta Information Resource Stats");
    havok_printf("HEAP meta contact point memory %.2f KB (max %d), meta rigid body memory %.2f KB (max %d)", contacts_total * (1.0f / 1024.0f), contacts_maximum, bodies_total * (1.0f / 1024.0f), bodies_maximum);
    havok_printf("HEAP meta phantom memory %.2f KB (max %d), meta constaint memory %.2f KB (max %d)", other_total * (1.0f / 1024.0f), other_maximum, constraints_total * (1.0f / 1024.0f), constraints_maximum);
    havok_printf("TOAL HEAP meta memory %.2f KB", (contacts_total + bodies_total + constraints_total + other_total) * (1.0f / 1024.0f));
}
