// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1DA540.CPP: the damage of havok collisions (0x1d96d0..) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"

/* an entry of the 16 collisions function_1d96d0 gathers on its stack (0x94
   bytes); it constructs them with the vector constructor iterator */
#include "unknown_1da540.h"

struct s_collision_damage_object
{
	byte unknown00[0x28];
	long value28;
	long value2c;
	byte unknown30[0xb4 - 0x30];
	long component_index;
};

struct s_collision_damage_surface
{
	byte unknown00[5];
	byte material_index;
	byte unknown06[2];
};

struct s_collision_damage_mesh
{
	byte unknown00[0x2c];
	s_collision_damage_surface *surfaces;
};

struct s_collision_damage_geometry
{
	byte unknown00[0x9c];
	s_collision_damage_surface *surfaces;
	byte unknowna0[0xc8 - 0xa0];
};

struct s_collision_damage_instance
{
	byte unknown00[0x34];
	short geometry_index;
	byte unknown36[0x58 - 0x36];
};

struct s_collision_damage_structure
{
	byte unknown00[0x13c];
	s_collision_damage_geometry *geometry;
	long unknown140;
	s_collision_damage_instance *instances;
};

struct s_collision_damage_definition
{
	long unknown00;
	long damage_index;
};

struct s_collision_damage_globals
{
	byte unknown00[0xbc];
	s_collision_damage_definition *definition;
};

struct s_slot_entry_list;
extern s_slot_entry_list *g_4e0340;
struct s_type_1e6529;
void function_d6660(s_type_1e6529 *data, long definition_index);
real function_1201a0(vector3f *vector, vector3f const *fallback);

struct s_collision_damage_location
{
	long instance;
	long material;
	long surface;
};

PRIVATE __forceinline s_havok_component_element48 *collision_damage_contact_get(s_havok_component *component, long index)
{
	return &component->unknown88.data[index];
}

PRIVATE __forceinline void collision_damage_location(dword key, long *instance, long *material, long *surface)
{
	long absolute_index = key & 0xffff;
	long part = (key >> 16) & 0x1fff;
	*instance = NONE;
	switch (key >> 29)
	{
	case 1:
		*material = ((s_collision_damage_mesh *)g_4e0340)->surfaces[absolute_index].material_index;
		*surface = absolute_index;
		break;
	default:
		s_collision_damage_structure *structure = (s_collision_damage_structure *)g_4e0348;
		*instance = absolute_index;
		*material = structure->geometry[structure->instances[absolute_index].geometry_index].surfaces[part].material_index;
		*surface = part;
		break;
	}
}

struct s_material_type_1da550
{
	short index;
};

/* fills a collision damage entry from one of an object's contacts when its
   impulse is above 0.4 */
// score 13 (was 95): the tail's loads of the location are scheduled
// differently (retail reloads the instance right after function_1201a0)
// Matched in a full build with stand-in SDK headers (was 13): the contact's material is copied through a one-member
// struct local (a struct-typed local is not folded into its use, so the 16-bit value stays a register value while its
// load stays after the kind store), and the surface index is stored after the material index.
// @retail 0x1da550
bool function_1da550(long object_index, long contact_index, s_collision_damage_entry *entry)
{
	s_collision_damage_object *object = (s_collision_damage_object *)havok_object_get(object_index);
	s_havok_component *component = havok_component_get(object->component_index);
	s_havok_component_element48 *contact = collision_damage_contact_get(component, contact_index);
	bool result = false;
	/* retail reads the impulse through the component again (it computes the
	   component pointer and then drops it) */
	if (component->unknown88.data[contact_index].impulse > 0.4f)
	{
		s_collision_damage_location location;
		collision_damage_location(*(dword *)&contact->unknown00[4], &location.instance, &location.material, &location.surface);
		function_d6660((s_type_1e6529 *)entry, ((s_collision_damage_globals *)g_4e034c)->definition->damage_index);
		entry->origin = contact->position;
		entry->position = contact->position;
		entry->direction = contact->normal;
		function_1201a0(&entry->direction, g_4687a8);
		entry->kind = 2;
		s_material_type_1da550 material = *(s_material_type_1da550 const *)&contact->material_b;
		*(s_material_type_1da550 *)&entry->unknown7c = material;
		entry->object_value28 = object->value28;
		entry->object_value2c = object->value2c;
		entry->flags |= 0x100;
		entry->instance_index = location.instance;
		entry->material_index = location.material;
		entry->surface_index = location.surface;
		result = true;
	}
	return result;
}


struct s_damage_owner;
void __stdcall object_get_damage_owner(long object_index, s_damage_owner *owner);
void __stdcall function_d7b80(s_type_1e6529 *data, long object_index, short node_index, short unknown0c, short region_index, vector3f const *direction);
long function_1dac00(long object_index);
bool havok_component_unknown10_recent(s_havok_component const *component);
void havok_component_unknown10_expire(s_havok_component *component);

struct s_collision_damage_limits
{
	byte unknown00[0x80];
	real minimum_speed;
	real maximum_speed;
	real minimum_damage;
	real maximum_damage;
};

struct s_collision_damage_event
{
	long definition_index;
	dword flags;
	byte owner[12];
	long owner_object;
	byte unknown18[0x24 - 0x18];
	point3f position;
	point3f origin;
	byte unknown3c[0x54 - 0x3c];
	real damage;
	byte unknown58[0x7c - 0x58];
	short material;
	byte unknown7e[6];
	byte kind;
	byte unknown85[3];

	s_collision_damage_event() : material(NONE) {}
};

PRIVATE inline bool collision_damage_recent_inlined(s_havok_component const *component)
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

// @retail 0x1da9e0
bool function_1da9e0(long object_index, point3f const *position, real speed_a, real speed_b,
	real impulse, bool use_owner, long owner_index)
{
	(void)&position;
	(void)&impulse;
	(void)&use_owner;
	(void)&owner_index;
	s_havok_object *object = havok_object_get(object_index);
	s_collision_damage_limits *limits = (s_collision_damage_limits *)g_4e3b44[*(long *)object & 0xffff].bytes;
	bool result = false;
	real speed = speed_a > speed_b ? speed_a : speed_b;
	if (object->havok_component_index != NONE)
	{
		s_havok_component *component = havok_component_get(object->havok_component_index);
		if (impulse > 4.0f || collision_damage_recent_inlined(component))
		{
			if (speed > limits->minimum_speed && limits->maximum_speed - limits->minimum_speed >= 0.001f && (byte)function_1dac00(object_index))
			{
				real lower = limits->minimum_damage;
				real upper = limits->maximum_damage;
				s_collision_damage_event event;
				function_d6660((s_type_1e6529 *)&event, ((s_collision_damage_globals *)g_4e034c)->definition->damage_index);
				real amount = (speed - limits->minimum_speed) * (upper - lower) / (limits->maximum_speed - limits->minimum_speed) + lower;
				event.kind = 2;
				event.damage = lower > amount ? lower : amount > upper ? upper : amount;
				event.origin = *position;
				event.position = *position;
				if (use_owner)
				{
					object_get_damage_owner(owner_index, (s_damage_owner *)event.owner);
					event.owner_object = owner_index;
				}
				else
					object_get_damage_owner(component->object_index, (s_damage_owner *)event.owner);
				event.flags |= 0x100;
				function_d7b80((s_type_1e6529 *)&event, component->object_index, NONE, NONE, NONE, NULL);
				if (havok_component_unknown10_recent(component))
					havok_component_unknown10_expire(component);
				result = true;
			}
		}
	}
	return result;
}

struct s_collision_force_definition
{
    short type;
    byte unknown02[0x38 - 2];
    long model_index;
    byte unknown3c[0x6c - 0x3c];
    real transfer_scale;
    real minimum_force, maximum_force;
    real minimum_damage, maximum_damage;
};

struct s_collision_force_material
{
    byte unknown00[0x10];
    byte material;
};

struct s_collision_force_model
{
    byte unknown00[0x60];
    long material_count;
    s_collision_force_material *materials;
};

real magnitude3d(vector3f const *vector);
long function_1dac40(long object_index, long other_object_index);
void function_e3f00(long object_index);

// score 16 (was 47): with magnitude3d `inline` (src/unknown_023540.cpp) retail's spill of the speed across the
// magnitude3d call comes from the inline callee, so the larger speed is a plain ternary (no volatile, no copy local,
// no reference locals). Left: the velocity copy's registers, the damage-index load order around the 0xd6660 call
// and the flags / owner stores before 0xd7b80 (the second havok_component_get's global load is retail's since the
// named materials pointer, round 151).
// M3 lane 8 r240 (fast screen with 0x1da550, 0x1da9e0, which stay matched): every difference is a scratch-register
// rotation or a store order. A const pointer to the first rigid-body element for the velocity copy 41 (the copy then
// retail's), a const reference to it 41, a pointer to the velocity member 45, `.data[0]` 47, the copy ahead of the
// other definition 54, after the magnitude 57. On the element pointer: the damage index read into a `long` local ahead
// of the event (the constructor's NONE store then follows the load, as retail) 7, with the owner-object store ahead of
// `flags |= 0x101` (0x1da9e0's order) 5, left: the index in EAX (retail EDX) and the materials pointer in EDX (retail
// reuses EAX). On that: a named materials pointer 23 (that block then retail's, the later call setups rotate by one),
// with the index `long const` 23; the model lookup at each use 5; a byte local for the material 5; the index declared
// at the rigid-body block's top 30, the ratio block's top 33, the function's top 188; a constructor taking the index 5,
// with the named materials pointer 23. Diagnostic (not keepable): the index as an expression temporary inside the call
// with the named materials pointer 8, everything after the 0xd6660 setup then retail's. Lead: retail reads the index as
// an expression temporary ahead of the NONE store (EDX by rotation) and names the materials pointer; every natural way
// to read it first makes it a named value, which takes EAX. All of these are one-use or order forms: match only.
// M3 lane 12 r192 (fast with 0x1da550 and 0x1da9e0, both MATCH): the element pointer + named index + owner store
// first rebuilt at 5; a forced-ranking search on it: the index forced into EDX gives 2 (the materials pointer then
// stays EDX), so two allocator picks remain and both follow from the index being an expression temporary in retail.
// The NONE store written as a plain field store (the other d6660 callers' form) in place of the constructor 47 (the
// store still precedes the index load: its dependence is real, not the inlined constructor's pointer), with the three
// match-only forms 5, with the index declared ahead of the damage limits 5; the store-form with the element pointer
// and the owner store first (index as a temporary) 41; the event declared ahead of the damage limits 47.
// M3 lane 15 r151 (H2M_FAST with 0x1da550 and 0x1da9e0, MATCH throughout). Kept: the materials pointer named under
// its own count test (`if (count) { materials = ...; if (materials->material) ... }`) 47 -> 16: the pointer then
// reuses the dead model register as retail does and every later scratch rotation is retail's. Reading: retail's
// damage index is an expression temporary (it takes EDX by rotation and is what advances the later rotations) that
// is loaded ahead of the event's NONE store; our temporary always follows the store, and a named index (EAX) leaves
// every later call setup rotated by one. On the kept form: the owner store ahead of `flags |= 0x101` 14; with that
// and the one-use element pointer for the velocity copy 8 (registers all retail; left only the index load after the
// NONE store, the span's movaps/subss and the push order); on that 8 form the event declared at the damage block's
// top 8, at the force block's top 18, a named index at the block's top 23 (a forced-ranking search putting it in EDX
// still leaves 20: the later rotations need a temporary); the velocity read through `rigid_bodies.data->` 16. On the
// earlier three-form 5 (element pointer, named index, owner store first): the index first in the block 5, between
// the limits and the span 5, after the event 41, as a `long const &` 41, through a `long const &` copy at the call 5;
// the count test nested with the materials read at each use 5, the named materials pointer 28. Diagnostic (removed):
// the NONE store moved into the call's first argument (index evaluated in the call) 41, the store still ahead.
// M3 lane 3 r316 (fast with 0x1da550 and 0x1da9e0, MATCH each time; control 16). Retail's own 0x1da9e0 stores the
// event's NONE before it loads the damage index, as this body does; retail here loads the index first as a scratch
// temporary (EDX), so its source reads the index ahead of the store without naming it. The two match-only forms
// together (element pointer for the velocity copy, owner store ahead of the flags) 8, the call setup alone left; on
// that, the index named ahead of the event 23, a forced-inline wrapper storing NONE and calling 0xd6660 with the index
// as its argument 23 (the inline parameter acts as a named value). On main's body: the force span named once for the
// test and the division 118 (764 B, retail subtracts twice); the speed test as `impulse >= 1.5f || body_speed >= 1.5f`
// 20.
// @retail 0x1da6d0
bool function_1da6d0(long object_index, long other_object_index, real speed_a, real speed_b, real impulse)
{
    s_havok_object *object = havok_object_get(object_index);
    s_havok_object *other = havok_object_get(other_object_index);
    s_collision_force_definition *definition = (s_collision_force_definition *)g_4e3b44[*(long *)object & 0xffff].bytes;
    real speed = speed_a > speed_b ? speed_a : speed_b;
    bool result = false;
    if (speed_b > speed_a * 0.25f && other->havok_component_index != NONE && (byte)function_1dac00(object_index))
    {
        s_havok_component *component = havok_component_get(other->havok_component_index);
        if ((byte)function_1dac40(object_index, component->object_index) && component->rigid_bodies.size > 0)
        {
            s_collision_force_definition *other_definition = (s_collision_force_definition *)g_4e3b44[*(long *)other & 0xffff].bytes;
            vector3f velocity = component->rigid_bodies[0].linear_velocity;
            real body_speed = magnitude3d(&velocity);
            real scale = 1.0f;
            if (havok_component_unknown10_recent(component))
            {
                body_speed += component->unknown14;
                if (speed >= 0.001f)
                    scale = (component->unknown14 + speed) / speed;
            }
            real maximum_speed = impulse > body_speed ? impulse : body_speed;
            if (!(1.5f > impulse && 1.5f > body_speed))
            {
                real speed_ratio = body_speed / maximum_speed;
                real force = speed_ratio * other_definition->transfer_scale * scale * speed;
                if (maximum_speed >= 0.001f && force > definition->minimum_force && definition->maximum_force - definition->minimum_force >= 0.001f)
                {
                    real lower = definition->minimum_damage;
                    real upper = definition->maximum_damage;
                    real damage_span = upper - lower;
                    s_collision_damage_event event;
                    function_d6660((s_type_1e6529 *)&event, ((s_collision_damage_globals *)g_4e034c)->definition->damage_index);
                    event.kind = 2;
                    if (other_definition->model_index != NONE)
                    {
                        s_collision_force_model *model = (s_collision_force_model *)g_4e3b44[other_definition->model_index & 0xffff].bytes;
                        if (model->material_count)
                        {
                            s_collision_force_material *materials = model->materials;
                            if (materials->material)
                                event.kind = materials->material | 0xc0;
                        }
                    }
                    real damage = (force - definition->minimum_force) * damage_span / (definition->maximum_force - definition->minimum_force) + lower;
                    event.damage = lower > damage ? lower : damage > upper ? upper : damage;
                    object_get_damage_owner(component->object_index, (s_damage_owner *)event.owner);
                    event.flags |= 0x101;
                    event.owner_object = component->object_index;
                    function_d7b80((s_type_1e6529 *)&event, object_index, NONE, NONE, NONE, NULL);
                    if (havok_component_unknown10_recent(component))
                        havok_component_unknown10_expire(component);
                    if (object->havok_component_index != NONE)
                    {
                        component = havok_component_get(object->havok_component_index);
                        if (havok_component_unknown10_recent(component))
                            havok_component_unknown10_expire(component);
                    }
                    if (definition->type == 0 && other_definition->type == 1)
                        function_e3f00(object_index);
                    result = true;
                }
            }
        }
    }
    return result;
}
