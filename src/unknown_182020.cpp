// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"

void havok_component_rigid_body_point_velocity_get(vector3f *velocity, long rigid_body_index, s_havok_component *component,
	point3f const *point);
void function_141590(transform4x3f const *in, transform4x3f *out);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
extern transform4x3f *g_4687d0;

// @retail 0x182020
bool function_182020(long component_index, vector3f *previous_velocity, point3f const *point,
	transform4x3f const *transform, transform4x3f *matrix, vector3f *velocity,
	vector3f *delta_velocity, matrix3x3 *rotation)
{
	bool result = false;
	if (component_index != NONE)
	{
		s_havok_component *component = havok_component_get(component_index);
		long body_index = havok_component_main_rigid_body_index_get(component);
		long valid_index;
		if (body_index < 0)
			valid_index = 0;
		else
		{
			valid_index = component->rigid_bodies.size - 1;
			if (body_index <= valid_index)
				valid_index = body_index;
		}
		if (valid_index == body_index)
		{
			transform4x3f body_matrix;
			transform4x3f inverse;
			transform4x3f relative;
			vector3f velocity_sample;
			component->rigid_body_matrix_get(body_index, &body_matrix);
			function_141590(&body_matrix, &inverse);
			function_142a60(transform, &inverse, &relative);
			function_141590(&relative, &relative);
			*rotation = relative.rotation;
			havok_component_rigid_body_point_velocity_get(&velocity_sample, body_index, component, point);
			*velocity = velocity_sample;
			*rotation = g_4687d0->rotation;
			delta_velocity->i = velocity_sample.i - previous_velocity->i;
			delta_velocity->j = velocity_sample.j - previous_velocity->j;
			delta_velocity->k = velocity_sample.k - previous_velocity->k;
			*previous_velocity = velocity_sample;
			*matrix = body_matrix;
			result = true;
		}
	}
	return result;
}

struct s_shape_material
{
	byte unknown00[6];
	short material;
	long unknown08;
	real friction;
	real restitution;
};

class c_material_shape
{
public:
	virtual void slot0() {}
	virtual void slot1() {}
	virtual void slot2() {}
	virtual void slot3() {}
	virtual void slot4() {}
	virtual long shape_kind() { return 0; }
	long unknown04;
	s_shape_material *material;
	byte unknown0c[0x30 - 0xc];
	c_material_shape *child;
};

struct s_material_properties
{
	byte unknown00[0x1c];
	real friction;
	real restitution;
	byte unknown24[0xb4 - 0x24];
};

struct s_material_globals_view
{
	byte unknown00[0x150];
	long count;
	s_material_properties *materials;
};

struct s_material_object
{
	long definition_index;
	byte unknown04[0x10a - 4];
	byte flags;
};

struct s_material_object_header
{
	byte unknown00[3];
	byte type;
	long unknown04;
	s_material_object *object;
};

static inline s_material_object_header *material_object_header_get(long object_index)
{
	return &((s_material_object_header *)g_4e0300->data)[object_index & 0xffff];
}

struct s_entity_material_view
{
	byte unknown00[0x4c];
	real friction;
	real restitution;
};

struct s_lookup_source;
short *function_1ee410(s_lookup_source const *source, short *result);
long havok_entity_component_index_get(hkEntity const *entity);
extern short g_54e898;

/* the material index of a shape with its own material table */
static inline short shape_material_index_get(c_material_shape *shape)
{
	short index;
	return *function_1ee410((s_lookup_source *)shape, &index);
}

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

// @retail 0x182b90
void function_182b90(c_material_shape *shape, hkEntity const *entity,
	real *friction, real *restitution, short *material)
{
	if (shape->shape_kind() == 0x17)
		shape = shape->child;
	if (shape->shape_kind() == 0x18)
	{
		short index = shape_material_index_get(shape);
		s_material_properties *properties = 0;
		if (index != NONE && index >= 0)
		{
			s_material_globals_view *globals = (s_material_globals_view *)g_4e034c;
			if (index < globals->count)
				properties = &globals->materials[index];
		}
		/* the material index of the shape, looked up into the upper half of a
		   pair of shorts */
		short material_indices[2];
		*material = *function_1ee410((s_lookup_source *)shape, &material_indices[1]);
		*friction = properties->friction;
		*restitution = properties->restitution;
		return;
	}
	if (shape->material && PIN((long)shape->material, 1, 16) != (long)shape->material)
	{
		s_shape_material *properties = shape->material;
		*material = properties->material;
		*friction = properties->friction;
		*restitution = properties->restitution;
		return;
	}
	if (!entity)
	{
		*material = g_54e898;
		*friction = 1.0f;
		*restitution = 0.0f;
		return;
	}
	long component_index = havok_entity_component_index_get(entity);
	if (component_index != NONE)
	{
		s_havok_component *component = havok_component_get(component_index);
		long type = material_object_header_get(component->object_index)->type;
		if (type == 0)
		{
			s_material_object *object = material_object_header_get(component->object_index)->object;
			byte *definition = g_4e3b44[object->definition_index & 0xffff].bytes;
			bool alternate = (bool)((object->flags >> 2) & 1);
			*friction = ((s_entity_material_view const *)entity)->friction;
			*restitution = ((s_entity_material_view const *)entity)->restitution;
			short *selected = alternate ? (short *)(definition + 0x282) : (short *)(definition + 0x280);
			*material = *selected;
			return;
		}
		else if (type == 12)
		{
			s_material_object *object = material_object_header_get(component->object_index)->object;
			byte *definition = g_4e3b44[object->definition_index & 0xffff].bytes;
			bool alternate = (bool)((object->flags >> 2) & 1);
			*friction = ((s_entity_material_view const *)entity)->friction;
			*restitution = ((s_entity_material_view const *)entity)->restitution;
			short *selected = alternate ? (short *)(definition + 0xf2) : (short *)(definition + 0xf0);
			*material = *selected;
			return;
		}
	}
	else
	{
		*material = g_54e898;
		*friction = 1.0f;
		*restitution = 0.0f;
		return;
	}
	*material = g_54e898;
	*friction = 1.0f;
	*restitution = 0.0f;
}
