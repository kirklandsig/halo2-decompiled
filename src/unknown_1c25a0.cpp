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
	/* the shape at this level of the body */
	c_contact_shape_view *get_shape() const { return shape; }
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
	__forceinline c_contact_callback() {}
	__forceinline c_contact_callback(bool value) : found(value) {}
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
	__forceinline c_contact_exclusion() : c_contact_callback(false) {}
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
	if (body->get_shape()->type() == 0x18)
		return false;
	long metadata = body->get_shape()->metadata;
	if (!metadata)
		return false;
	if (contact_metadata_pin(body->get_shape()->metadata, 1, 16) == metadata)
		return false;
	return *(byte *)(metadata + 0x1e) != 0xff;
}

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
PRIVATE __forceinline long function_1d4884(long arg_0)
{
 return arg_0 < 1 ? 1 : arg_0 > 16 ? 16 : arg_0;
}

PRIVATE __forceinline bool function_1d4885(s_contact_body_view const *arg_0)
{
 if (arg_0->get_shape()->type() == 0x18)
  return false;
 long local_0 = arg_0->get_shape()->metadata;
 if (!local_0)
  return false;
 long bounded = function_1d4884(local_0);
 _ReadWriteBarrier();
 if (bounded == local_0)
  return false;
 return *(byte *)(local_0 + 0x1e) != 0xff;
}


#pragma function(_ReadWriteBarrier)
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
	s_havok_object *object = havok_object_get(object_index);
	if (object->havok_component_index != NONE)
	{
		s_havok_component *component = havok_component_get(object->havok_component_index);
		if ((byte)force || havok_component_any_rigid_body_active(component))
		{
			if ((1 << ((s_velocity_object_header *)g_4e0300->data)[object_index & 0xffff].type) & 2)
			{
				long index = havok_component_main_rigid_body_index_get(component);
				if (index != NONE && !havok_component_rigid_body_get(index, component)->m_fixed &&
					!component->rigid_body_keyframed(index))
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
					if (!body->m_fixed && body->m_motion->kind_code() != 6)
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


// @retail 0x1c29d0
void function_1c29d0(void)
{
	((c_physics_body_island_view *)g_47f048)->entity_observer_detach((c_havok_entity_observer *)g_51ecac);
	g_47f050--;
	g_51e9a4->entity_world_remove((hkEntity *)g_47f048);
	function_278f00();
}

// @retail 0x1c35f0
void __stdcall function_1c35f0(long object_index)
{
	s_physics_object_header *header = physics_object_header_get(object_index);

	if (((1 << header->type) & 0x1883) && !(header->flags & 0x10))
	{
		long root_index = function_baf80(object_index);
		s_physics_object_header *root = physics_object_header_get(root_index);

		function_0bfe40(&header->object->flags, 0xc, false);
		if (root->cluster_index != NONE)
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
#pragma inline_depth(0)
				function_0bfe40(&header->object->flags, 0xc,
					TEST_FIELD_BIT(component->bit14) || TEST_FIELD_BIT(component->bit20));
#pragma inline_depth()
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
   Retail keeps both arguments on the stack (ret 8). With the attachment
   helper call at 0xb93b0 -> 0xb7300 kept out of line, the code-reference
   cycle preserves this convention without an address-escape marker. */
// @retail 0x1c3770
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
void function_1c54b0(char const *variants, long object_index)
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
			s_model_region_view const *region = &model->regions[i];

			if (variants[i] == current[i])
				continue;
			if (variants[i] == NONE || current[i] == NONE)
			{
				if (region->permutations[variants[i] == NONE ? current[i] : variants[i]].region == NONE)
					continue;
			}
			else if (region->permutations[variants[i]].region == region->permutations[current[i]].region)
				continue;
			function_1c3770(object_index, 0);
			break;
		}
	}
}


// score 4 (was 4): two swapped pairs are left. After the entity loop ours reloads the
// island index before the world pointer and retail reloads the world pointer first;
// ours stores the removed flag before the discard call and retail stores it after the
// call (moving that store after the call always swaps ebx/ebp in the entity loop).
// Further trials: local names count to flagged_entities 4; local names island_index to
// group_index 4; local names entity_count to total_entities 4; local names i to
// entity_index 4; local names count to flagged_entities, island_index to group_index 4;
// local names island_index to island_ordinal, entity_count to entity_total, count to
// accepted_count, i to entity_index 4; all named loop state with flag after discard 18;
// named loop state, flag after discard, native int entity counts 18. Round 87, all with the flag
// after the discard call: entity_count renamed total/size/a_count, i renamed entity_index/j, count
// renamed matched, island renamed group, removed renamed discarded: 18 each (names never move the
// ebx/ebp swap).
// Round 42 (a compile-only build reproduces all four; 158 forms screened there): entity count read at each use 4,
// with the flag after the call 18; world local read at each iteration 4; entity local 4; nested tests 4.
// Round 89, with the flag after the discard call: entity-array pointer 75; island_index renamed
// active_island 18; increment in the outer for clause 18; scoped object, entity or flags locals 18 each;
// inner while 18; const entity count 18; function-scope flag cleared after the index update 63.
// Lane 10 round 42 (a compile-only harness reproduces ours and the 18 form exactly; every one of 60
// declaration orders and 27 name sets keeps EBP for the count with the flag after the call, and the island
// index is reloaded before the world in every form): `if (!removed) island_index++;` 91; flag set in an
// else arm without an initialiser 73; island-array local with the flag after the call 18; function-scope
// flag cleared at the top of each iteration 18; `entity_count == count` with the flag after the call 18;
// a 600 s permuter run (71 tries), none below 4.
// Lane 15 round 42 (fast): outer while with the flag after the call 18; a named next index 4; a named step
// with the flag after the call 18; an island-table local with the flag after the call 18; a for-scoped entity
// index with the flag after the call 18; a byte flag with the flag after the call 18. Compile-only, 26 forms
// of the index update, flag type and world read (`island_index = !removed + island_index`, a bool kept flag,
// a world local read per iteration with the discard through the global): the reload pair never swaps.
// Round 31 (lane 18, fast): a force-inline flagged-entity count helper compared with the island's count 6;
// a force-inline helper for the whole per-island body returning the flag 4 (flag after the call 18, its
// result added straight into the index 4); a world local read just before the count test 4; a world local
// at the iteration top for the island and the discard 4. Original body restored.
// Round 98, with the flag after discard and a world local before the count test except the pre-call control:
// that world-local form 18; a guarded do inner loop 79; component_index declared at island scope 18; an
// explicit walking entity pointer 20, with the flag before the call 8; a const island pointer 18; a per-element
// entity local 18; the post-call flag assigned from `count == entity_count` 18. Original body restored.
// Round 127 (lane 7, fast): the flag as the existing hkBool, after the call 25, before it 12; an inline island
// getter with the flag after the call 321 (12 bytes); an inverted kept flag before the call 24, after it 36; the
// flag declared at the count test 69, stored after the call 82; a world local reassigned on the back edge 37 (253
// bytes). Compile-only, 31 forms (world getter, for-scoped index, while, guarded do, ternary and `== false` steps):
// the reload pair never flips. Original body restored.
// Round 131 (lane 7, fast, all with the flag after the call): 1-3 unused locals at the top or inside the loop 18 each; the flag stored both before
// and after the call 18 (the first store dies before allocation); the flag declared then assigned 18; i declared before the flag 18; the flag
// declared before the count 20; the flag assigned from `count == entity_count` 18, from `!removed` 18; a two-use local for the index step 18.
// Diagnostics: an index offset in the island read 12, a count offset 64 (253 bytes), an entity index offset 69 (254 bytes): none moves the pair or
// the ebx/ebp choice. With the flag ahead of the call it crosses the call and retail's ebx/ebp follow; after the call, cursor takes ebx. An inline
// helper for the discard taking the world and island, or the island: flag first 4/4, flag after 18/18, forced inline after 18; one returning true 18;
// the whole per-island body as a plain inline helper is not inlined here 286/288 (45-47 bytes), four call forms with either flag order.
// Round 219 (lane 3, fast): the flag as char before/after the call 4/18; as long, int or dword 79 in either order; a long `kept` step (1,
// cleared on discard, added to the index) 36 in either order.
// Round 173 (m3 lane 4, fast): the island re-read from the world in the discard call's argument, flag after/before the call 18/4
// (same code as the island local); with the flag after the call: the entity index declared at function scope 18, the island
// index declared in the for statement 18, count and flag declared ahead of the island and entity count 18. Diagnostic: with the
// flag after the call the tail is retail's from the call on; only the entity count/cursor ebx/ebp swap and the reload pair remain.
// Round 137 (lane 7, fast; a one-file LTCG harness reproduces 4 and 18 exactly): with the flag after the call, the step as
// `removed ? 0 : 1` 18, as `removed == false` 18, as `island_index = island_index + !removed` 18; count declared before the
// entity count 18; the flag address-taken (harness only) 18, set through a force-inline discard helper taking its address
// (harness only) 18. The ternary step with the flag before the call 4 (same code).
// Round 162 (m3 lane 5, fast, all with the flag after the call): a 600 s permuter run from that form (40 tries) best 4, by
// moving the flag back before the call; count and entity index at function scope 20; the component test as `continue` then the
// flag test 18; a world local inside the discard block 18; the count test inverted with an empty then-arm 18; `while` with nested
// tests and `entity_count <= count` 55. Retail's zero-trip jump lands on the count test, past both reloads: the world pointer and
// the island index are both reloads on the entity loop's exit edge, world first in retail, index first here.
// Round 152 (m3 lane 7, fast; round 137's one-file harness re-pointed at this file, exact at 4 and 18): with the flag
// after the call, `removed++` 18, `removed |= true` 18 (harness), `removed = !removed` 18 (harness), an unsigned or signed
// char flag 18/18 (harness), the object test nested 18 (harness), `++count` 18 (harness); a world local declared at
// function scope and assigned at each iteration top 18 (flag first 4), assigned in the loop condition 37 (253 bytes; flag
// after 40), assigned once before the loop 59/58 (harness); the island declared at function scope 18 (flag first 4); the
// entity count declared at function scope 18 (flag first 4). Naming the world never moves the reload pair.
// Round 94 (m3 lane 11, fast; flag before/after the call): the component index through the existing out-of-line
// getter 0x1cf280 124/124 (208 bytes: the reduced image calls it); the per-entity test in an inline bool helper
// 172/172 (156 bytes, not inlined); the island's entity count and entity through inline accessors 4/18 (same code);
// the island through an inline (world, index) accessor 4/18 (same code). Compile-only diagnostic: the outer loop
// written as a guarded do-while, so the world is read ahead of the index's first assignment, keeps the pair in either
// flag order, so the pair does not follow which of the two values is defined first.
// Round 91 (m3 lane 14, fast; flag before/after the call): the outer loop as a guarded do-while 4/18; count and
// flag declared at function scope and assigned at the island top 4/18; the entity index declared in the inner for
// statement 4 (all the 4s the same code).
// @retail 0x1c50c0
void function_1c50c0(void)
{
	long island_index;

	for (island_index = 0; island_index < g_51e9a4->m_island_count; )
	{
		c_havok_body_group *island = g_51e9a4->m_islands[island_index];
		long entity_count = island->m_entity_count;
		long count = 0;
		bool removed = false;
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
			removed = true;
			g_51e9a4->world_island_discard(island);
		}
		island_index += !removed;
	}
}

// @retail 0x1c2a10
void function_1c2a10(void)
{
	struct
	{
		s_object *object;
		s_type_f1af8e iterator;
	} state;

	function_bae80(&state.iterator, 0, 0);
	while ((state.object = function_baeb0(&state.iterator)) != NULL)
	{
		physics_object_get(state.iterator.object_index)->havok_component_index = NONE;
	}
	function_bae80(&state.iterator, 0, 0);
	while ((state.object = function_baeb0(&state.iterator)) != NULL)
	{
		long object_index = state.iterator.object_index;

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

// score 1 (was 1): the only difference is the force flag handed to function_1c4040:
// ours goes in dl and retail passes it in al. That register belongs to
// function_1c4040's own convention (see the note there).
// Further trials: byte force, long attempt 24; signed char force, long attempt 24; int
// force, long attempt 31; bool force, unsigned long attempt 2; bool force, short attempt
// 11; bool force, unsigned short attempt 12; byte force, unsigned short attempt 26; int
// force, unsigned long attempt 31.
// Round 213: for-owned long retry 1; for-owned force 3; terminal equality 8; nonterminal
// equality 2; for-owned int retry 1; for-owned signed-byte retry 40; signed-byte terminal
// equality 40; for-owned long terminal equality with reset-first publication 8. Force argument
// convention remains.
// Round 88: defined after function_1c4040 1; the disabled 0xb8540 body enabled 1 (25 matched lost);
// the call-cycle entry moved through declaration order (function_146bf0 declared first in
// src/unknown_278f00.cpp, function_146da0 before function_1c4040 in src/unknown_146a20.cpp) 2, with
// function_1c4040 then in a custom convention (attempt EAX, active CL) and 15 matched lost.
// Round 32 (fast, each reproducing the baseline): function_146bf0 declared before function_278f00 here,
// with function_146da0 before function_1c4040 in src/unknown_146a20.cpp 1; plus function_1c3770
// declared first here and function_1c2690 before function_1c27a0 there 1; function_1d1540 declared
// before function_b8840 in src/bipeds.cpp 1; function_278f00 before function_146bf0 in
// src/unknown_0b8ca0.cpp 1; function_278f00 before function_1c51c0 in src/unknown_1c39c0.cpp 1.
// Round 106 (fast): our walk reaches function_1c4040 through 0x278f00 -> 0x1c3770 -> 0x1d1540 ->
// 0x1c39c0 -> here before 0x146bf0. This function moved ahead of the file's 0x146bf0/0x278f00 callers 1;
// moved to the end of the file 1; function_146da0 before function_1c4040 in src/unknown_146a20.cpp
// (alone) 1; function_1c4040 declared before function_278f00 here 1; function_146bf0 before
// function_1c3770 in src/unknown_278f00.cpp 1 (function_1c4040 25, 0x1c39c0 lost at 41).
// Round 61: attempt declared before force 3; callee declaration at the TU top 1; consistent inline
// declaration 1; byte callee result 1; the other caller's function_146da0 declaration first 1;
// callee declared before function_278f00/function_146bf0 1; function_146bf0/callee/function_278f00
// order 1; callee after both 1. Original declarations restored.
// Round 55 (lane 11, fast): 0x146bf0 declared first in this file 1; 0x278f00 declared first 1; 0x146da0 then
// 0x146bf0 declared first 1; this body moved into src/unknown_1c39c0.cpp (diagnostic) 1; 0x1c4040's definition
// moved to the end of this file 1. Its other caller 0x146bf0 stays built ahead of it; the declarations that move
// that sit in src/unknown_278f00.cpp and src/unknown_146a20.cpp, both other lanes' files this round.
// Round 204 (plain over the 720 cycle members, none lost): `force != false` at the call 24 (70 bytes, fast);
// this body alone in a new file of its own 1; function_278f00 declared before function_146bf0 in
// src/unknown_1c2b10.cpp 1, in src/unknown_1c3d30.cpp 1; function_1c3770 declared before function_e18d0 in
// src/unknown_1c38a0.cpp 1. Under the default /Gr convention the second argument is always DL, so only a
// custom convention for function_1c4040 can give AL.
// Round 102: function_1d1540 before function_1d1260 1; between function_1d1260 and function_1d01c0 1;
// function_146bf0 before function_278f00 1; that swap with function_1d1540 first of the three 1;
// function_1d1540 before 0x278f00/0x146bf0 1; between them 1; before the reversed 0x1d01c0/0x1d1260
// pair 1; the swapped pair with function_1d1540 between function_1d1260 and function_1d01c0 1.
// Every control stayed unchanged; original declarations restored.
// Round 89 (m3 lane 12, plain and full checks; every trial restored): `inline` on 0x146bf0's definition 1, on 0x146da0's 1; 0x1c4040 declared
// first or between its neighbours in src/unknown_146a20.cpp 1, 1. 0x1c4040 and 0x146bf0 call each other and 0x146bf0 takes no arguments, so
// retail reached 0x146bf0 first. Retail's graph has that path through a stub here: 0x1d1260 -> 0x2266a0 -> ... -> 0x17a8a0 -> 0xb7b40 -> 0x146bf0.
// 0xb7b40's draft enabled, nothing else: 2 (the custom regime, attempt EAX, active CL), 0x1c4040 13, 0x146bf0 3, 6 matched lost, all callers
// of 0x1d1540 going custom (215): 0xb8890, 0xe16b0, 0xf0fd0, 0x1c35f0, 0x1c3770, 0x1c39c0. With 0xb7430 too: the same plus 0x275380, 0xb7430
// matches. 0xb7b40 and 0xb8540: 1, 0x1d1540 back at 97, 0xb8540 matches, 28 lost (0x1c3770 goes custom, 41). All four drafts: 1, 29 lost.
// 0xb7b40, 0xb7430 and 0x2902b0: 1, 229 lost. Lead: with 0xb7b40 enabled 0x1d1540 needs a caller built before it, which takes 0x146bf0
// being reached before 0x278f00 is on the walk's stack (then 0x1c4040 -> 0x278f00 -> 0xb8890 builds that caller first).
// Round 94 (fast, all restored): 0xb7b40's draft enabled with 0x1c4040's array store without its cast and the fixed island
// read through g_51e9a4: this function, 0x1c4040 and 0x146bf0 match, 0x1d1540 custom (215), six lost (0xe16b0 114, 0xb8890
// 15, 0x1c35f0 40, 0x1c3770 21, 0x1c39c0 41, 0xf0fd0 27). Added to that: 0xb8890 declared before 0x1c3770 in
// src/unknown_278f00.cpp 1 (nothing lost, 0x1c4040 plain again, 17); 0x146bf0 first in src/unknown_0b7b40.cpp 1 (0x278f00
// lost 185); both 1; 0x1c2a10 declared first in src/unknown_1c2690.cpp no change. BLOCKED-BY 0x1c4040 (its convention).
// Round 137 (lane 7, fast): `0x34e < g_47f050` 1; the call's result compared `== false` 1; attempt reset before the force
// flag is set 3; `g_47f050 >= 0x34f` 5; the force step nested in a plain else 1.
// m3 lane 11 r83 (fast): the walk forms in 0x1c4040's note re-measured 1/1/1/1/1 (the two 34 forms lose eight matched).
// Round 175 (m3 lane 5): no form of this body can move the one differing instruction, so none was measured: under /Gr
// the second argument of a default-convention callee is DL in every spelling, and retail's AL needs function_1c4040 in
// the custom regime. Retail's graph gives the walk that does it: 0x146bf0 (with 0x146da0 inlined) calls 0x1c2690, which
// calls 0x1c39c0, which calls this function, so both callers of function_1c4040 are on the walk's stack when it is
// first reached, provided the cycle is entered at 0x146bf0 while 0x278f00 is still unvisited. Retail enters there
// through 0xb7b40, whose body is disabled here; every other caller of 0x146bf0 is reached only after 0x278f00.
// BLOCKED-BY 0xb7b40 (and the bodies its walk needs, see function_1c4040's note).
// Round 194 (m3 lane 4, fast over 19 cycle members): in src/unknown_0b7740.cpp, off limits in the earlier walk rounds, 0xb8890
// defined ahead of 0xb8840 1 and function_1d1540 declared before function_1d1260 1 (nothing moves); function_1d1540 declared
// ahead of function_1d1260 in src/unknown_1cec30.cpp (0x1d0cf0 visits 0x1d1540 first) with function_278f00 declared before
// function_1c51c0 in src/unknown_1c39c0.cpp 2 (0x1c4040 55; lost 0xe16b0 10, 0xb8840 15, 0x1c3770 41, 0x1c54b0 89, 0x1d1260
// 220, 0x278f00 245, 0xf0fd0 33); with either src/unknown_0b7740.cpp move added the same. All restored.
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
	c_havok_body_group **data;
	long count;
	dword capacity_and_flags;
};

struct s_physics_world_view
{
	byte unknown00[8];
	s_simulation_island_array active_islands;
	s_simulation_island_array inactive_islands;
	byte unknown20[0x2c - 0x20];
	c_havok_body_group *fixed_island;
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
	bool field_3;
	volatile long field_4;
	long field_8;
	s_simulation_island_array *field_c;
	long field_10;
};

// score 8 (was 8): only the convention differs: ours takes the active flag in DL
// (the default /Gr register: our LTCG reaches this function through 0x1c39c0 ->
// 0x1c51c0 before 0x146bf0 on their call cycle), retail takes it in AL with attempt
// in ECX. Kept: the existing function_a7670 for the mode test 66; the result field
// without volatile 8. Tried: plain locals 180; an island-scan helper 180; the priority
// field without volatile 172; the island-array store without its volatile cast 17;
// the effect read after the mode test 100; a separate result local 119; an else arm
// for the mode case 8. With 0x1c51c0's call removed (diagnostic) LTCG gives this
// function a custom convention, but attempt in EAX and active in CL (30), and the
// same with the parameters reordered (active, attempt).
// Round 378: Scan-owned success result with shared/fixed/dynamic/both entity ordinals,
// original/structured dispatch, all eight 111 at 537 bytes. Original result and body restored.
// Round 279: selected index separate through both scans; island cursor/entity ordinals/entry initialization
// combinations 35/36/35/36/35/36/35/36, all at 537 bytes. Original body restored;
// active DL versus AL and its array/count allocation remain.
// Round 88: our default convention comes from the call-cycle entry, which follows declaration order:
// function_146bf0 declared first in src/unknown_278f00.cpp plus function_146da0 declared before this
// function in src/unknown_146a20.cpp gives a custom convention, but attempt EAX and active CL (30; the
// first half loses 15 matched); with (active, attempt) reordered 30; with g_51e9a4 read at each use 32;
// 0xb8540's disabled body enabled 25 (25 matched lost). The custom regime still swaps EAX/ECX and AL/CL.
// Round 163: coordinated cdecl/stdcall with bool, byte, signed-char active and
// unsigned-long attempt transport 187/193 in every pair. All original declarations
// and bodies restored; frame 0x14 agrees, but active remains DL rather than AL.
// Round 42, in a diagnostic custom regime (0x1c51c0's call removed: 30, attempt EAX, active CL): world declared
// first 30; island-array store without the volatile cast 27; long active 30; fixed island read through g_51e9a4
// (world local only for the array choice) 31 with retail's AL/ECX ranking but the world load hoisted into EDX
// before sub esp; every world read through g_51e9a4 32.
// Round 90: declaration-first paths in three adjacent translation units, singly and combined, 8 each.
// In the diagnostic custom regime: fixed-island direct read with a two-use retry local 31; the
// complementary array-direct read 31; adding a two-use active local 31; world declared before the
// record 31; block-scoped world 32. Original body and declaration order restored.
// Lane 10 round 42: the convention follows where LTCG enters the object code's large call cycle, which
// this function, 0x146bf0 and 0x278f00 belong to through 0xb8540 (BLOCKED-BY 0xb8540: its body is disabled,
// see src/unknown_0b8540.cpp); the island-scan count in EDX rather than ECX follows from DL. Body trials:
// separate index for the island scan 8; island element read at each use 150; entity read through the
// priority view at both uses 143; fixed island read at each use 78; for-scoped indices 8.
// Lane 15 round 42 (fast): the active choice as if/else stores 20; a named copy of the active flag 8; `active
// == false` 8; a named array local before the volatile store 192 (553 bytes); the world pointer declared before
// the record 8. A static definition (diagnostic:
// the other caller in src/unknown_146a20.cpp no longer links) gives the custom convention, attempt EAX and
// active CL 13, with 0x1c51c0 at 2.
// m3 lane 9 r76 (fast): the world read through g_51e9a4 at each use 8; one physics_object_get shared by the detach
// steps 116 (521 B); the mode case and the detach path as if/else with one success store after 8 (kept: drops the
// goto and its label); object and unknown declared in the detach block 8; the three initial stores reversed 103.
// Round 127 (lane 7): `inline` on this definition 8, on 0x1c51c0's 8, on both 8 (fast); 0x1c3590 moved to src/unknown_1c2690.cpp
// (the same flags, ahead of nothing on the walk) 8. In the custom regime (0x146bf0 first in src/unknown_278f00.cpp, fast): the array
// store without its cast 27, as if/else 27, through a named array 55, `!active` 28; that store with the fixed island read through
// g_51e9a4 17 (one real difference, 538 bytes): this gives retail's AL/ECX and both callers match. With that body, plain, by walk:
// 0x1c3770 declared before havok_component_node_states_get in src/damage.cpp: this function, 0x1c51c0 and 0x146bf0 match, but 0xb8890,
// 0x1c35f0, 0x1c3770, 0x1c39c0 are lost (0x1d1540 goes custom, 215). Adding 0x1d1540 before 0x1d1260 here and 0x278f00 before
// 0x1c51c0 in src/unknown_1c39c0.cpp: 14, 0x1d1540 back at 97, 0x1d1260's callers lost. 0x1d1540 before 0x1d1260 here, 0x278f00 before
// 0x1c51c0 in src/unknown_1c39c0.cpp, function_ba6f0 before function_d9d60 in src/damage.cpp and 0x1d1260 declared ahead of the
// include in src/unknown_278f00.cpp: all three match with only 0xba6f0 and 0xba7f0 lost, because that entry passes through 0x1c54b0
// and 0xba7f0 is then built before it, so 0x1c54b0 goes default (81) where retail has it custom: BLOCKED-BY 0x1c54b0's place on the
// walk. Not kept (a matched function may not be lost); body and declarations restored. 0xba7f0 reached first through 0xd6800 and
// 0xd6a70 keeps 0x1c54b0 custom but this function default 17 (0xd6bc0, 0xd9640, 0x1039a0 lost).
// Round 103: forward declaration before/after the cycle callees 8 each; fixed-island direct global read 8;
// shared island/entity/priority locals 8 each; block-scoped world with the direct fixed read 8;
// effect pointer at function scope 8. Original body and declaration order restored.
// Round 137 (m3 lane 8, plain over the 149 cycle members): round 127's walk set without its src/damage.cpp step (array store
// without its cast, fixed island through g_51e9a4, 0x1d1540 before 0x1d1260 here, 0x278f00 before 0x1c51c0 in
// src/unknown_1c39c0.cpp, 0x1d1260 ahead of the include in src/unknown_278f00.cpp) 17 (still DL), none lost; plus 0x146bf0
// first in src/unknown_278f00.cpp 34 (still DL, 0x1d1540 custom 215, 15 lost); plus 0x2266a0 before 0x278f00 in
// src/unknown_1d1260.cpp 17; plus 0x146bf0 before 0x278f00 here 17; plus 0x1c51c0 defined after this function 17. Every
// path from 0x1d1260 or 0x1d1540 to 0x146bf0 passes 0xb7b40 or 0x2902b0 (both disabled): BLOCKED-BY 0xb7b40.
// Round 137 (lane 7, fast): `long i` declared first 8; the result byte cleared before the island-array store 101 (538 bytes);
// the selected-object store before the priority store 10; a `* const` world pointer 8; the array choice as an inverted
// ternary 9.
// m3 lane 11 r83: retail's 0x1c51c0 is custom too (its parameter in EDI, as ours). On the walk this function custom needs
// 0x146bf0 and 0x1c51c0 as its ancestors, and 0x1d1540 default needs one of its callers as its descendant. A rough walk
// model (retail call edges, our file order, first-mention callee order; it reproduces today's conventions) puts the
// cycle entry at 0x1d0cf0 -> 0x1d1260 -> 0x278f00 (src/unknown_1cec30.cpp); over every single callee-order move and pair
// of moves in 26 cycle callers (46 moves), none makes this function custom without flipping a matched function: the
// best, 0xdbc80 visiting 0x1c3770 first (round 127's src/damage.cpp lever), flips 0x1c39c0. Re-measured (fast): the array
// store without its cast and the fixed island through g_51e9a4 17; that with 0x146bf0 before 0x1c3770 in
// src/unknown_278f00.cpp 34 (0x1d1540 custom 215, eight matched lost); with 0x278f00 before 0x1c51c0 in
// src/unknown_1c39c0.cpp 17; both 34; with 0x1c2690 first in src/unknown_146a20.cpp 17.
// Round 172 (m3 lane 6, fast with 0x1c51c0, which stays 1): the priority tests as `local_0.field_4 < priority` in both
// scans 12; the effect read through the globals at its test and the call 106; the island index a plain for-scoped local
// in place of field_10 35; the mode test inverted with its arms swapped 73 (541 bytes); the fixed island's entity count
// read once before its loop 185 (555 bytes). The convention stays the only cause (BLOCKED-BY 0xb7b40, as above).
// m3 lane 10 r99 (fast with 0x1c51c0 1 and 0x1c3f30 matched): `if (object_index == NONE) continue;` in both scans 8; the
// success store in both arms of the mode test 8; the entity priority read at each use (no priority local) 170 (547 B); the
// island loop as a `while` with its increment at the end 8; the entity typed as the priority view (cast back for the call) 8.
// m3 lane 7 r159 (fast): the parameters as (active, attempt) at the definition, both declarations and both calls 11
// (active in CL, attempt in EDX); 0x1c3f30 defined after this function 8; 0x146bf0 defined first in
// src/unknown_146a20.cpp 8; 0x1c3770 defined after this function 8; this function moved to the end of its file 8.
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
		c_havok_body_group *island = world->fixed_island;

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
		c_havok_body_group *island = local_0.field_c->data[local_0.field_10];

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

		if (function_a7670(local_0.field_8))
		{
			physics_object_get(local_0.field_8)->flags |= 0x20;
			function_1c3770(local_0.field_8, 0);
		}
		else
		{
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
		}
		local_0.field_3 = true;
	}
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

/* the rigid body owning a collidable (0x1c4260): the owner sits 0x10 bytes
   before the collidable, and retail reads the type through the owner */
PRIVATE __forceinline hkRigidBody *collidable_rigid_body_1c4260(byte *arg_0)
{
 byte *local_1 = arg_0 - 0x10;
 long local_0 = *(long *)(local_1 + 0x18);
 return local_0 == 1 ? *(hkRigidBody **)(local_1 + 0x20) : NULL;
}

// @retail 0x1c4260
void function_1c4260()
{
 /* read through a const pointer, as retail keeps the flag store ahead of the
    island count load */
 if (g_51e9a4 && !*(bool const *)&g_47f05b)
 {
  g_47f05b = true;
  for (long local_0 = 0; local_0 < g_51e9a4->m_island_count; ++local_0)
  {
   c_havok_body_group *local_1 = g_51e9a4->m_islands[local_0];
   long local_2 = ((s_1c4261 *)local_1)->field_6c;
   if (local_2 > 24)
   {
    for (long local_3 = 0; local_3 < ((s_1c4261 *)local_1)->field_6c; ++local_3)
    {
     s_1c4260 *local_4 = &((s_1c4261 *)local_1)->field_68[local_3];
     hkRigidBody *local_5 = collidable_rigid_body_1c4260(local_4->field_0);
     hkRigidBody *local_6 = collidable_rigid_body_1c4260(local_4->field_4);
     if (local_5->m_motion->kind_code() == 7 || local_5->m_motion->kind_code() == 6 ||
         local_6->m_motion->kind_code() == 7 || local_6->m_motion->kind_code() == 6)
      --local_2;
    }
    if (local_2 > 24)
    {
     long local_7;
     for (long local_8 = 0; local_8 < 3; ++local_8)
     {
      long local_9 = 0x80000000;
      local_7 = NONE;
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
      if (local_7 != NONE)
       break;
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

#include "pair_filter_class.h"

class c_contact_rule_filter
{
public:
 virtual hkBool accepts(void const *query, s_contact_body_view const *a, s_contact_body_view const *b,
  c_contact_shape_container *container, long key);
};

/* the flag bit of a contact shape's material data; retail's dead stack byte
   is this helper's returned hkBool */
PRIVATE __forceinline hkBool contact_metadata_flag(long metadata)
{
 hkBool result;
 result.m_bool = *(byte *)(metadata + 0x1f) & 1;
 return result;
}

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
  c_contact_shape_view *shape = key != NONE ? container->child(key, buffer) : b->get_shape();
  if (shape->type() == 0x15)
   shape = *(c_contact_shape_view **)((byte *)shape + 0xc);
  if (shape->type() == 0x17)
   shape = *(c_contact_shape_view **)((byte *)shape + 0x30);
  if (shape->type() != 0x18 && function_182180((hkShape *)shape))
  {
   long metadata = shape->metadata;
   if (metadata && contact_metadata_pin(shape->metadata, 1, 16) != metadata)
   {
    if (contact_metadata_flag(metadata).m_bool)
    {
     root = a;
     while (root->parent)
      root = root->parent;
     hkRigidBody *rigid_body = root->type == 1 ? (hkRigidBody *)root->entity : NULL;
     if (rigid_body && rigid_body->m_motion->kind_code() == 7)
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

// score 3 (was 3): retail tests the loop count against zero at the loop entry and
// ours compares it with the index register; retail sets up the register arguments of
// the 0x1d10b0 call with ecx first and ours with edx first, an order that follows
// the callee's declared parameter order in src/unknown_1cec30.cpp (not editable here).
// Further trials: shared scale helper for both vectors 3; shared scale and output
// assignment helpers 65; shared scale plus named progress and body count 3; native int
// selected count 3; named saved iteration index 3; named selected and available counts 3;
// shared scale plus all named loop state 3; available count read at every use 3.
// Round 213: positive-guard exact-count long/int/short/word [4, 4, 132, 132]; positive-guard
// last-index long/int/short/word [156, 156, 164, 164]. Count test and matched-callee setup
// remain.
// Round 88: while loop 3; index shared with the fill loop 3; mirror initialised from the index 3;
// `count > i` 5; count initialised 3. This is 0x1d10b0's only caller, so its parameters can be
// reordered to (component, index, change) in src/unknown_1cec30.cpp for the register-argument order.
// Round 42 (0x1d10b0's file on the avoid list again): index declared before the mirror 3; mirror declared with
// the selection locals 3; index declared outside the for 3; int index 3; fill loop counting into count 139.
// Lane 10 round 42: the EBX compare comes from the mirror's zero (with no mirror at all the entry test is
// retail's `test eax, eax`, 125 diagnostic; no reload in the impulse path 106 diagnostic); index initialised
// before the selection 162; mirror zeroed before the selection 166; mirror stored at the top of each iteration
// 111; default-NONE selected index in place of the goto 3; selected index stored before the count 5.
// BLOCKED-BY 0x1d10b0 (its file on the avoid list; free in lane 15 round 42, form below, not kept).
// Lane 15 round 42: 0x1d10b0's parameters as (component, index, velocity) in a free function 1, but 0x1d10b0
// itself 1 (`[edx+ecx+0x40]`); as a member of s_havok_component (`component->...(index, &b)`, its siblings
// 0x1d0bb0/0x1d0c00/0x1d0b20 are members and retail addresses `[index*0x60+data+0x40]`) 1 with 0x1d10b0 still
// matched (plain check): the setup-order pair goes and only the entry `cmp eax, ebx` is left. Not kept (a gain
// of 2 without a match). With no mirror at all the body is retail's except the index spills: the two stores,
// the reload after the impulse call and the 4-byte frame slot (126 positional, 858 bytes, all three aligned).
// Entry test, compile-only: the zero of the mirror store is shared with the compare in that block, so mirror
// typed dword/int/unsigned, declared apart, set from the index, `i = local_0 = 0`, `local_0 = i = 0`, an
// int index, store-through-index before the test all keep `cmp eax, ebx`; `(local_0 = i) < count` re-reads the
// volatile; the mirror set in each selection arm gives `test eax, eax` but `mov [slot], 0` (+4 bytes); stored at
// the top of each iteration with a store after the loop `test eax, eax` but no head merge. Plain loop with
// `inline` on 0x1d1160's definition 126; with an unused fifth parameter on 0x1d1160 126. Fast, from 3: a positive
// guard around the for 3; the mirror set in each selection arm 36 (872 bytes); the mirror stored at the top of each
// iteration and after the loop 119 (870 bytes); `i = local_0 = 0` 161 (882 bytes); int index 3.
// Round 124: uninitialised loop state inside a positive do guard 39; `_count`, vector names and count
// declared first 39 each; reversed guard and initialised count 39 each; guarded for 39. Guard only around
// the mirror 5, braced 5, nonzero 143 (882 bytes), induction in the for initializer 147 (882), literal zero 5;
// the same-scope while with 0x1d10b0 as a component member 3 (the member stays matched). Restored.
// m3 lane 9 r76 (fast): the fill loop bounded by count itself 139 (872 B); count declared before the indices 3;
// `local_0 = ++i;` 3; the mirror declared at the top of the component block and zeroed at the label 3; the node
// guard as nested ifs 3. Restored.
// Round 127 (lane 7, fast): no mirror 125 (858 bytes), the same with `inline` on 0x1d1160's definition 125, with
// 0x1d1160's aligned locals in a force-inline helper 232 (0x1d1160 35); mirror declared at the function's top and
// zeroed at the label 3; `i = 0` then `if (i < count)` around a mirror set from i 5; a named count before the mirror
// 3; a named count with i declared before the mirror 3. Restored.
// Round 137 (m3 lane 8; plain unless fast): 0x1d10b0 as a member of s_havok_component (index, velocity), retail's ecx-then-edx
// setup, 1 plain with 0x1d10b0 still matched (not kept: 2 points without a match); on it: no mirror 126 (858 bytes, fast);
// `inline` on 0x1d1010's definition 249 (0x1d1010, 0x1d1160 and 0xe5300 lost); the goto as a default-NONE selected index 1
// (same code, fast); `local_0 = ++i` as the for increment 1; `local_0 = i = 0` with i declared before the for 1; a volatile
// copy of i saved just around the impulse call 100 (fast). Retail's +0x30 slot is a register spill of i (the entry test
// uses the constant, the store follows it), as if LTCG believed 0x1d1160 clobbers EBX; no source store reproduces that.
// Round 137 (lane 7, fast): the selected index stored before the count 5; the mirror declared then assigned 0 3; the index
// declared at the top of the component block 3; the rigid-body count initialised at its declaration 3; the mirror stored in the
// for statement's increment clause (`++i` at the end of the body) 3.
// Round 75 (m3 lane 14, fast): re-measured the while loop 3, a positive guard around a do/while 3 and the mirror set from i 3;
// an early `return result` for no rigid bodies 281 (902 bytes); the node index stored in the selection's condition 6; no mirror
// with i at function scope 125 (858 bytes, round 127's no-mirror code); the component flag in a named bool for both tests 240
// (894 bytes); the main-body comparison as `index == main index` 4; one rigid-body fetch shared by the linear and angular reads
// 67 (861 bytes). Retail's 0x1d1160 (ends at +0xc5) never touches EBX; its only direct call, 0x1d1010, saves it, so the index
// spill is no callee clobber in the bytes.
// Round 175 (m3 lane 5, fast with 0x1d10b0 0x1d1160 0x1d1010 0xe5300; plain agrees): the cause of the index spill. The two
// vectors and the loop index as the members of one local record (a, b, index, in that order: retail's frame) with a plain
// `for` and no mirror give retail's `test eax, eax` entry and the index store at each definition; positionally 105 (864
// bytes), aligned only the 4-byte reload after the 0x1d1160 call and the 0x1d10b0 setup pair are missing. The reload
// appears (868 bytes, 2: the pair alone) as soon as the compiler takes 0x1d1160 to write through its velocity argument,
// while 0x1d1010 and 0x1d10b0, given the same record's addresses, stay known not to. What makes it so in 0x1d1160
// (each with the record form here; 0x1d1160 loses its match in every one): a real store through the pointer
// (diagnostic); the vector conversion through a plain function of this file that stores through its output pointer
// (under /Ob1 it stays out of line, even with one call site: 0x1d1160 146-176 bytes); an SSE load intrinsic on the
// velocity's members (`_mm_load_ss(&velocity->i)`: 0x1d1160 204-240 bytes in six spellings, since the store intrinsic
// goes through a `lea` and the member read through a stack copy). What does not (105, no reload): a non-const velocity
// pointer; 0x1d1010 defined after 0x1d1160; `inline` on 0x1d1160; 0x1d1160 defined in this file; the conversion helper
// as `inline` or `__forceinline`; the conversion (vector alone, or point and vector) through plain functions defined
// in src/unknown_1cfb90.cpp, which is built without /Ob1 (they are inlined at link time, 0x1d1160 still matches, and
// the caller then sees no write); local copies of the point and the vector; an early `return` on the first path; the
// set intrinsics; a load intrinsic on the point only; the vector by value (copied to the stack, 252). So the callee
// must keep, when this function is compiled, a call or intrinsic that takes the velocity pointer and may store, yet
// end with retail's bytes: not found. With it the recipe is this record, 0x1d10b0 as a component member (index,
// velocity) and that callee form.
// Round 194 (m3 lane 4, fast with 0x1d10b0, 0x1d1160, 0x1d1010 and 0xe5300): 0x1d10b0 as an s_havok_component member (index,
// velocity) re-measured 1 with 0x1d10b0 matched (not kept: two points without a match). Diagnostic on it: no `i = local_0;`
// reload in the impulse arm still gives `cmp eax, ebx` (107, 864 B), so the entry compare comes from the mirror's zero store
// sharing EBX, not from the index's induction status. On the member form: no mirror with the two changes as one
// `vector3f changes[2]` 126 (858 B); the mirror kept with `changes[2]` 1 (same code); `long i = 0; while (i < count)` 1
// (re-measure of round 124's member-form while, 3); an
// `if (count > 0)` guard around the for 1; the mirror declared with count and zeroed in each selection arm 34 (872 B). Restored.
// @retail 0x1c4c50
bool __stdcall function_1c4c50(long object_index, long node_index, point3f const *point,
 vector3f const *change_a, vector3f const *change_b, long *result_object, vector3f *linear, vector3f *angular)
{
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
    if (!body->m_fixed && body->m_motion->kind_code() != 6)
    {
     if (point && change_a && !TEST_FIELD_BIT(component->flag1))
     {
      havok_component_rigid_body_point_impulse_apply(index, component, point, &a);
      i = local_0;
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
    __forceinline ~c_contact_query_bounds_info()
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
    virtual ~c_contact_query_transform_volume();
    byte unknown04[0xd0 - 4];
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
    void *add(void *volume);
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
    point3f center;
    contact_query_center(object, &center);
    real radius = g_47f05c * 2.0f + object->radius;
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
    c_contact_exclusion callback;
    callback.found_other = false;
    callback.excluded_component = excluded_component;
    info.shape = shape;
    info.transform.m_rotation.m_col0.set(matrix->forward.i, matrix->forward.j, matrix->forward.k);
    info.transform.m_rotation.m_col1.set(matrix->left.i, matrix->left.j, matrix->left.k);
    info.transform.m_rotation.m_col2.set(matrix->up.i, matrix->up.j, matrix->up.k);
    hkVector4 position;
    position.set(matrix->position.x, matrix->position.y, matrix->position.z);
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

class c_havok_abstract_allocator;
extern c_havok_abstract_allocator *g_479894;
extern c_havok_abstract_allocator *g_479898;
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
void function_22c390(c_havok_abstract_allocator *memory, s_fixed_memory_statistics_16 *statistics);
void function_22cb00(c_havok_abstract_allocator *memory, s_physics_pool_statistics *statistics);
long function_1d3880(s_havok_component const *component, long *constraints, long *bodies, long *contacts, long *other);

// @retail 0x1c4590 standard
void __stdcall function_1c4590(long flags)
{
    byte *world = (byte *)g_51e9a4;
    c_physics_statistics_allocator *allocator = (c_physics_statistics_allocator *)g_480118;
    long active_islands = *(long *)(world + 0xc);
    long inactive_islands = *(long *)(world + 0x18);
    long fixed_entities = *(long *)(*(byte **)(world + 0x2c) + 0x40);
    long allocator_statistics[4];
    long active_entities = 0, inactive_entities = 0;
    allocator->statistics(allocator_statistics);
    s_2797a0_iterator iterator;
    iterator.groups = (s_2797a0_groups *)g_51e9a4;
    iterator.mode = 0;
    iterator.second = false;
    iterator.group = NONE;
    iterator.index = NONE;
    iterator.unknown00 = 1;
    ++g_51ecb0;
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
    struct
    {
        s_physics_pool_statistics pool;
        s_fixed_memory_statistics_16 startup;
        s_fixed_memory_statistics_16 scratch;
        s_fixed_memory_statistics physical;
    } memory;
    fixed_memory_get_statistics(g_4798a0, &memory.physical);
    fixed_memory_get_statistics_16(g_47989c, &memory.scratch);
    function_22cb00(g_479898, &memory.pool);
    function_22c390(g_479894, &memory.startup);
    havok_printf("Havok Memory (total %.2f KB, debug %.2f KB, percent used %.3f)", 1045.109375, (memory.pool.metadata_size + memory.scratch.unknown0c + memory.startup.unknown0c) * (1.0f / 1024.0f), memory.pool.used_fraction * 100.0f);
    havok_printf("Startup pool: mem(%.2f KB/%.2f KB), allocs(%d)", memory.startup.allocated_size * (1.0f / 1024.0f), memory.startup.header_size * (1.0f / 1024.0f), memory.startup.allocation_count);
    havok_printf("Overflow pool: mem(%.2f KB/%.2f KB), allocs(%d))", memory.scratch.allocated_size * (1.0f / 1024.0f), memory.scratch.header_size * (1.0f / 1024.0f), memory.scratch.allocation_count);
    havok_printf("2nd Overflow pool: mem(%.2f KB/%.2f KB), allocs(%d))", memory.physical.allocated_size * (1.0f / 1024.0f), memory.physical.header_size * (1.0f / 1024.0f), memory.physical.allocation_count);
    havok_printf("Runtime Pool(%.2f KB): usage==pages(%d/%d)*size(%.2f KB)+last(%.2f KB)", (memory.pool.allocated - memory.pool.metadata_size) * (1.0f / 1024.0f), memory.pool.used_pages, memory.pool.pages, memory.pool.page_size * (1.0f / 1024.0f), (memory.pool.page_size - memory.pool.free_size) * (1.0f / 1024.0f));
    for (long i = 0; i < memory.pool.entry_count; ++i)
    {
        s_physics_pool_statistics::s_entry *entry = &memory.pool.entries[i];
        havok_printf("\t b%d(%d) (%d / %d (*%d))", i, entry->size, entry->used, entry->count, entry->stride);
    }
    long contacts_total = 0, bodies_total = 0, other_total = 0, constraints_total = 0;
    long contacts_maximum = 0, bodies_maximum = 0, other_maximum = 0, constraints_maximum = 0;
    struct
    {
        s_havok_component *component;
        s_record_pool_iterator iterator;
    } components;
    components.iterator.data = g_51e9b8;
    components.iterator.index = NONE;
    while ((components.component = (s_havok_component *)data_iterator_next_inlined(&components.iterator)) != NULL)
    {
        long contacts, constraints, other, bodies;
        function_1d3880(components.component, &contacts, &bodies, &constraints, &other);
        contacts_total += contacts;
        bodies_total += bodies;
        other_total += other;
        constraints_total += constraints;
        if (contacts_maximum <= contacts) contacts_maximum = contacts;
        if (bodies_maximum <= bodies) bodies_maximum = bodies;
        if (other_maximum <= other) other_maximum = other;
        if (constraints_maximum <= constraints) constraints_maximum = constraints;
    }
    havok_printf("Havok Component Meta Information Resource Stats");
    havok_printf("HEAP meta contact point memory %.2f KB (max %d), meta rigid body memory %.2f KB (max %d)", contacts_total * (1.0f / 1024.0f), contacts_maximum, bodies_total * (1.0f / 1024.0f), bodies_maximum);
    havok_printf("HEAP meta phantom memory %.2f KB (max %d), meta constaint memory %.2f KB (max %d)", other_total * (1.0f / 1024.0f), other_maximum, constraints_total * (1.0f / 1024.0f), constraints_maximum);
    havok_printf("TOAL HEAP meta memory %.2f KB", (contacts_total + bodies_total + constraints_total + other_total) * (1.0f / 1024.0f));
}
