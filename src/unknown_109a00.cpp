#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "object_list.h"
#include "unknown_1428b0.h"
#include "vector_products.h"
#include "normalize3d.h"
#include "object_tag_definitions.h"

// @flags /O2 /arch:SSE /Gr

struct s_carried_object
{
	long definition_index;
	byte unknown004[0x14 - 4];
	long parent_index;
	byte unknown018[0x88 - 0x18];
	vector3f velocity;
	byte unknown094[0xb4 - 0x94];
	long component_index;
	long platform_index;
	byte unknown0bc[4];
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word : 13;
};

struct s_carried_object_header
{
	short identifier;
	byte flags;
	byte type;
	byte unknown04[4];
	s_carried_object *object;
};

struct s_carrier_definition
{
	word unknown00;
	word : 7;
	word flag7 : 1;
	word : 8;
};

struct s_carrier_motion
{
	point3f previous_center;
	point3f center;
	vector3f previous_velocity;
	vector3f velocity;
	vector3f previous_angular_velocity;
	vector3f angular_velocity;
	transform4x3f previous_transform;
	transform4x3f current_transform;
	transform4x3f transform;
	bool valid;
	bool active;
	byte unknown0e6[2];
};

struct s_small_index;
struct s_location;
short function_0b67a0(s_small_index const *data);
transform4x3f *function_ba160(long object_index, transform4x3f *matrix);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
void function_b75a0(long object_index, point3f const *point, vector3f const *forward,
	vector3f const *up, s_location const *location, bool unknown);


bool function_109a00(long object_index, vector3f *delta, bool rotate, vector3f *velocity);

/* the time globals' tick count as a real */
static inline real game_time_ticks_as_real(void)
{
	s_game_time_globals *globals = g_510c54;
	short ticks = globals->field_2_3;
	real rate = (real)ticks;
	return rate;
}

static __forceinline bool carried_object_motion_apply(long object_index, vector3f *delta, bool rotate, vector3f *velocity)
{
	s_carried_object_header *headers = (s_carried_object_header *)g_4e0300->data;
	s_carried_object *object = headers[object_index & 0xffff].object;
	bool result = false;

	if (TEST_FIELD_BIT(object->flag2))
	{
		long platform_index = object->platform_index;
		for (long i = 0; i < g_5107f0->object_count; i++)
		{
			if (g_5107f0->object_indices[i] == platform_index)
			{
				s_carrier_motion *motion = (s_carrier_motion *)&g_5107f0->entries[i];
				if (motion->valid && motion->active && object->parent_index == NONE)
				{
					s_carried_object *platform = headers[platform_index & 0xffff].object;
					s_carrier_definition *definition = (s_carrier_definition *)g_4e3b44[platform->definition_index & 0xffff].bytes;
					long component_index = platform->component_index;
					if (component_index != NONE &&
						function_0b67a0((s_small_index *)(g_51e9b8->data + (component_index & 0xffff) * 0xa0)) != NONE)
					{
						transform4x3f matrix;
						function_ba160(object_index, &matrix);
						if (rotate && TEST_FIELD_BIT(definition->flag7))
						{
							transform4x3f moved;
							vector3f previous_velocity;
							function_142a60(&motion->transform, &matrix, &moved);
							vector3f previous_offset;
							vector3d_from_points3d(&motion->previous_center, &matrix.position, &previous_offset);
							cross3f_staged(&motion->previous_angular_velocity, &previous_offset, &previous_velocity);
							vector3f_add(&motion->previous_velocity, &previous_velocity, &previous_velocity);
							vector3f *object_velocity = &object->velocity;
							object_velocity->i -= previous_velocity.i;
							object_velocity->j -= previous_velocity.j;
							object_velocity->k -= previous_velocity.k;
							function_142640(&motion->transform, object_velocity, object_velocity);
							carrier_point_velocity(&motion->center, &motion->velocity,
								&motion->angular_velocity, &moved.position, delta);
							function_b75a0(object_index, &moved.position, &moved.forward, &moved.up, NULL, false);
							*velocity = *object_velocity;
						}
						else
						{
							point3f moved;
							transform4x3f_apply_point(&motion->transform, &matrix.position, &moved);
							vector3f displacement;
							displacement.i = matrix.position.x - moved.x;
							real scale = game_time_ticks_as_real();
							displacement.j = matrix.position.y - moved.y;
							displacement.k = matrix.position.z - moved.z;
							vector3f change = {
								displacement.i * scale,
								displacement.j * scale,
								displacement.k * scale
							};
							function_b75a0(object_index, &moved, NULL, NULL, NULL, false);
							*velocity = object->velocity;
							vector3f_add(&object->velocity, &change, &object->velocity);
							delta->i = change.i * -1.0f;
							delta->j = change.j * -1.0f;
							delta->k = change.k * -1.0f;
						}
						result = true;
					}
				}
				break;
			}
		}
	}
	return result;
}

// @retail 0x109a00
bool function_109a00(long object_index, vector3f *delta, bool rotate, vector3f *velocity)
{
	return carried_object_motion_apply(object_index, delta, rotate, velocity);
}

struct s_carrier_search_view
{
	byte unknown00[0x28];
	byte location[8];
	point3f center;
	real radius;
};

short __stdcall function_bb050(long mask, dword type_mask, void const *location, point3f const *position, real radius, long *objects, short maximum_count);
void function_b7360(long object_index);
void function_bba20(long object_index);

// @retail 0x109580
void function_109580(long object_index)
{
	(void)&object_index;
	long objects[1024];
	s_record_pool *pool = g_4e0300;
	s_carrier_search_view *object = (s_carrier_search_view *)((s_carried_object_header *)pool->data)[object_index & 0xffff].object;
	long count = function_bb050(0, 0x3f, object->location, &object->center, object->radius, objects, 1024);
	for (long i = 0; i < count; i++)
	{
		long index = objects[i];
		s_carried_object_header *header = &((s_carried_object_header *)pool->data)[index & 0xffff];
		if ((header->flags & 1) && !(header->flags & 0x18))
		{
			s_carried_object *child = header->object;
			if (!TEST_FIELD_BIT(child->flag2))
			{
				s_carried_object *attached = ((s_carried_object_header *)pool->data)[index & 0xffff].object;
				attached->flag2 = true;
				attached->platform_index = object_index;
				function_b7360(index);
				if ((1 << header->type) & 0x3c)
				{
					function_bba20(index);
					header->flags |= 8;
				}
			}
		}
	}
}

struct s_carrier_vector4
{
	real n[4];

	real const &operator()(long i) const { return n[i]; }
};

struct s_carrier_body_motion
{
	byte unknown00[0x40];
	s_carrier_vector4 velocity;
	s_carrier_vector4 angular_velocity;
	byte unknown60[0x70 - 0x60];
	s_carrier_vector4 center;
};

struct s_carrier_body
{
	byte unknown00[0x3c];
	s_carrier_body_motion *motion;
	byte inactive;
};

struct s_carrier_body_entry
{
	byte unknown00[0x40];
	s_carrier_body *body;
	byte unknown44[4];
	char *indices;
	long count;
	byte unknown50[0x60 - 0x50];
};

struct s_carrier_component
{
	byte unknown00[0x18];
	char default_body;
	byte unknown19[0x70 - 0x19];
	s_carrier_body_entry *bodies;
	long count;
};

struct s_carrier_physics_body
{
	byte unknown00[0x18];
	byte flags;
	byte unknown19[0x90 - 0x19];
};

struct s_carrier_physics_definition
{
	byte unknown00[0x3c];
	s_carrier_physics_body *bodies;
};

struct s_havok_component;
real function_30bf0(vector3f *vector);
void function_141590(transform4x3f const *in, transform4x3f *out);

PRIVATE inline short carrier_component_default_body(s_carrier_component const *component)
{
	short body_index;
	if (component->default_body >= 0 && component->default_body < component->count)
		body_index = component->default_body;
	else
		body_index = NONE;
	return body_index;
}

PRIVATE inline void carrier_make_vector3f(vector3f *vector, real i, real j, real k)
{
	vector->i = i;
	vector->j = j;
	vector->k = k;
}

PRIVATE inline void carrier_vector_from_havok(vector3f *vector, s_carrier_vector4 const *havok)
{
	carrier_make_vector3f(vector, (*havok)(0), (*havok)(1), (*havok)(2));
}

PRIVATE inline void carrier_body_velocity_get(s_carrier_body const *body, vector3f *velocity)
{
	if (!body->inactive)
	{
		carrier_vector_from_havok(velocity, &body->motion->velocity);
	}
	else
	{
		*velocity = *g_4687a4;
	}
}

PRIVATE inline void carrier_body_angular_velocity_get(s_carrier_body const *body, vector3f *velocity)
{
	if (!body->inactive)
	{
		carrier_vector_from_havok(velocity, &body->motion->angular_velocity);
	}
	else
	{
		*velocity = *g_4687a4;
	}
}

/* a - b; result may be either input */
static __forceinline void vector3f_subtract(vector3f const *a, vector3f const *b, vector3f *result)
{
	result->i = a->i - b->i;
	result->j = a->j - b->j;
	result->k = a->k - b->k;
}

/* the entry's motion since its previous sample: the current transform composed with the previous
   one's inverse, its axes made orthonormal again */
static __forceinline void carrier_motion_delta_update(s_carrier_motion *entry)
{
	transform4x3f inverse;
	function_141590(&entry->previous_transform, &inverse);
	function_142a60(&entry->current_transform, &inverse, &entry->transform);
	function_30bf0(&entry->transform.forward);
	function_30bf0(&entry->transform.left);
	function_30bf0(&entry->transform.up);
	vector3f scaled;
	vector3f_scale(&entry->transform.forward, dot3f(&entry->transform.up, &entry->transform.forward), &scaled);
	vector3f_subtract(&entry->transform.up, &scaled, &entry->transform.up);
	function_30bf0(&entry->transform.up);
	cross3f_staged(&entry->transform.up, &entry->transform.forward, &entry->transform.left);
	function_30bf0(&entry->transform.left);
}

/* the component's rigid body, through the shared getter, as this file's view of it */
PRIVATE inline s_carrier_body *carrier_component_body(s_carrier_component const *component, long body_index)
{
	return (s_carrier_body *)havok_component_rigid_body_get(body_index, (s_havok_component const *)component);
}

// @retail 0x109660
void function_109660(long object_index, long entry_index)
{
	s_carried_object_header *headers = (s_carried_object_header *)g_4e0300->data;
	s_carried_object *object = headers[object_index & 0xffff].object;
	s_carrier_motion *entry = (s_carrier_motion *)&g_5107f0->entries[entry_index];
	long component_index = object->component_index;
	long body_index = NONE;
	s_carrier_component *component;
	if (component_index != NONE)
	{
		component = (s_carrier_component *)(g_51e9b8->data + (component_index & 0xffff) * 0xa0);
		long model_index = ((s_object_definition const *)g_4e3b44[object->definition_index & 0xffff].bytes)->model_tag_index;
		byte *model = g_4e3b44[model_index & 0xffff].bytes;
		if (*(long *)(model + 0x24) != NONE)
		{
			s_carrier_physics_definition *definition = (s_carrier_physics_definition *)g_4e3b44[*(long *)(model + 0x24) & 0xffff].bytes;
			body_index = carrier_component_default_body(component);
			bool found = false;
			for (long i = 0; i < component->count && !found; i++)
			{
				for (long j = 0; j < component->bodies[i].count; j++)
				{
					char physics_body_index = component->bodies[i].indices[j];
					if (definition->bodies[physics_body_index].flags & 0x10)
					{
						body_index = i;
						found = true;
						break;
					}
				}
			}
		}
	}
	bool valid;
	if (body_index != NONE)
	{
		if (entry->active)
		{
			entry->previous_transform = entry->current_transform;
			entry->previous_velocity = entry->velocity;
			entry->previous_angular_velocity = entry->angular_velocity;
			entry->previous_center = entry->center;
			entry->valid = true;
		}
		((s_havok_component *)component)->rigid_body_matrix_get(body_index, &entry->current_transform);
		carrier_body_velocity_get(carrier_component_body(component, body_index), &entry->velocity);
		carrier_body_angular_velocity_get(carrier_component_body(component, body_index), &entry->angular_velocity);
		carrier_vector_from_havok((vector3f *)&entry->center, &carrier_component_body(component, body_index)->motion->center);
		entry->active = true;
		valid = entry->valid;
	}
	else
	{
		entry->active = false;
		entry->valid = false;
		valid = false;
	}
	if (valid)
	{
		carrier_motion_delta_update(entry);
	}
}
