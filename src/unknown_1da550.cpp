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

// @retail 0x1da550
bool function_1da550(long object_index, long contact_index, s_collision_damage_entry *entry)
{
	s_collision_damage_object *object = (s_collision_damage_object *)havok_object_get(object_index);
	s_havok_component *component = havok_component_get(object->component_index);
	s_havok_component_element48 *contact = collision_damage_contact_get(component, contact_index);
	bool result = false;
	if (contact->impulse > 0.4f)
	{
		s_collision_damage_location location;
		collision_damage_location(*(dword *)&contact->unknown00[4], &location.instance, &location.material, &location.surface);
		function_d6660((s_type_1e6529 *)entry, ((s_collision_damage_globals *)g_4e034c)->definition->damage_index);
		entry->origin = contact->position;
		entry->position = contact->position;
		entry->direction = contact->normal;
		function_1201a0(&entry->direction, g_4687a8);
		entry->kind = 2;
		entry->unknown7c = contact->material_b;
		entry->object_value28 = object->value28;
		entry->object_value2c = object->value2c;
		entry->flags |= 0x100;
		entry->surface_index = location.surface;
		entry->instance_index = location.instance;
		entry->material_index = location.material;
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

// @retail 0x1da6d0
bool function_1da6d0(long object_index, long other_object_index, real speed_a, real speed_b, real impulse)
{
    long const *object_index_reference = &object_index;
    real const *impulse_reference = &impulse;
    s_havok_object *object = havok_object_get(*object_index_reference);
    s_havok_object *other = havok_object_get(other_object_index);
    s_collision_force_definition *definition = (s_collision_force_definition *)g_4e3b44[*(long *)object & 0xffff].bytes;
    volatile real speed = speed_a > speed_b ? speed_a : speed_b;
    volatile bool result = false;
    if (speed_b > speed_a * 0.25f && other->havok_component_index != NONE && (byte)function_1dac00(*object_index_reference))
    {
        s_havok_component *component = havok_component_get(other->havok_component_index);
        if ((byte)function_1dac40(component->object_index, *object_index_reference) && component->rigid_bodies.size > 0)
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
            real maximum_speed = *impulse_reference > body_speed ? *impulse_reference : body_speed;
            if (!(1.5f > *impulse_reference && 1.5f > body_speed))
            {
                real force = body_speed / maximum_speed * other_definition->transfer_scale * scale * speed;
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
                        if (model->material_count && model->materials->material)
                            event.kind = model->materials->material | 0xc0;
                    }
                    real damage = (force - definition->minimum_force) * damage_span / (definition->maximum_force - definition->minimum_force) + lower;
                    event.damage = lower > damage ? lower : damage > upper ? upper : damage;
                    object_get_damage_owner(component->object_index, (s_damage_owner *)event.owner);
                    event.flags |= 0x101;
                    event.owner_object = component->object_index;
                    function_d7b80((s_type_1e6529 *)&event, *object_index_reference, NONE, NONE, NONE, NULL);
                    if (havok_component_unknown10_recent(component))
                        havok_component_unknown10_expire(component);
                    if (object->havok_component_index != NONE)
                    {
                        component = havok_component_get(object->havok_component_index);
                        if (havok_component_unknown10_recent(component))
                            havok_component_unknown10_expire(component);
                    }
                    if (definition->type == 0 && other_definition->type == 1)
                        function_e3f00(*object_index_reference);
                    result = true;
                }
            }
        }
    }
    return result;
}
