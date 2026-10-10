// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_173B90.CPP: the particle systems effects start (g_510c74) and
   their particles, emitters and locations */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "effects.h"
#include <xtl.h>
#include "unknown_246cd0.h"

struct s_bsp3d;
extern s_bsp3d *g_4e033c;
long function_14a280(s_bsp3d *bsp, long index, point3f *point);

struct s_structure_leaf_173b90
{
	short cluster_index;
	byte unknown02[6];
};

struct s_structure_bsp_173b90
{
	byte unknown00[0x30];
	s_structure_leaf_173b90 *leaves;
};

void __stdcall function_2486e0(long particle_location_index);
long function_248620(void); /* unknown_2483f0.cpp */
void function_248d90(s_particle_location_datum *particle_location, long *first_index, long *last_index);
long function_248d50(s_particle_location_datum *particle_location);
void function_248970(s_particle_location_datum *particle_location, bool field_b4, real unknown, s_particle_system_datum *particle_system, real *values, transform4x3f const *matrix);
real function_248df0(long index, void *a, void *b, void const *c);
bool function_178af0(long effect_index);
void function_178b30(long effect_index, long unknown0, long unknown4);
color3f *unpack_color3f(dword pixel, color3f *color);
dword __cdecl pack_color3f(const color3f *color);

/* the last time particle systems ran out */
long g_47ff88;
void effect_remove_event_slot(long effect_index, long value);
void function_1753f0(s_particle_system_datum *particle_system);
bool function_174a30(s_particle_system_datum *particle_system, real dt);
void function_17af80(long effect_index, long particle_system_index);

struct s_particle_callback_output_173cb0
{
	long field_0;
	byte field_4;
	byte field_5;
	short field_6;
	short field_8;
	short field_a;
	long field_c;
	long field_10;
	long field_14;
};

// @retail 0x173cb0
bool __stdcall function_173cb0(long tag_index, long unused, long first, long second, long third, short value, s_particle_callback_output_173cb0 *output)
{
	/* This callback is passed at 0x1748f2 with all seven arguments on the stack. */
	long const *tag_argument = &tag_index;
	long const *unused_argument = &unused;
	long const *first_argument = &first;
	long const *second_argument = &second;
	long const *third_argument = &third;
	short const *value_argument = &value;
	s_particle_callback_output_173cb0 *const *output_argument = &output;
	tag_index = *tag_argument;
	unused = *unused_argument;
	first = *first_argument;
	second = *second_argument;
	third = *third_argument;
	value = *value_argument;
	output = *output_argument;
	long const *reference;
	dword group = TAG_GROUP(tag_index);
	if (group == 0x5052544d || group == 'prt3')
		reference = function_137bd0(tag_index)->function_x947334();
	else
		reference = *(long const **)(g_4e3b44[tag_index & 0xffff].bytes + 0x24);
	byte *definition = g_4e3b44[*reference & 0xffff].bytes;
	byte *table = *(byte **)(definition + 0x5c);
	word *entries = *(word **)(table + 4);
	long index = (entries[first * 5] & 0x1ff) + second;
	word *indices = *(word **)(table + 0xc);
	index = (indices[index] & 0x1ff) + third;
	byte *items = *(byte **)(table + 0x14);
	long item_tag = *(long *)(items + index * 10 + 4);
	byte *item = g_4e3b44[item_tag & 0xffff].bytes;
	output->field_10 = (*(long **)(item + 0x20))[1];
	output->field_5 = 3;
	output->field_4 = 0;
	output->field_8 = 7;
	output->field_14 = 0;
	output->field_6 = value;
	output->field_c = NONE;
	return true;
}

extern long g_4b9f8c;
extern real g_4b9fa4, g_4b9ff8, g_4b9f18, g_4b9f9c;
bool function_16e210(long cluster_index, long value);
void function_25ca0(bool enabled);

// @retail 0x174f80
void function_174f80(s_particle_system_datum *system, void *a, void *b, void *c)
{
	c_type_4e7709 *definition = function_137bd0(system->function_1751d0()->tag_index);
	bool enabled = function_16e210(system->location.cluster_index, g_4b9f8c);
	if (!enabled)
		function_25ca0(false);
	definition->render(system, a, b, c);
	if (!enabled)
	{
		real divisor = g_4b9fa4;
		if (0.0001f > divisor)
			divisor = 0.0001f;
		real value = g_4b9ff8;
		value *= 1.0f / divisor;
		value = 0.0f - value;
		real constants[4];
		constants[0] = value < 0.0f ? 0.0f : value > 1.0f ? 1.0f : value;
		constants[1] = 0.0f;
		constants[2] = g_4b9f18;
		constants[3] = g_4b9f9c;
		D3DDevice_SetVertexShaderConstant(-81, constants, 1);
	}
}

struct s_particle_render_request_173c90
{
	s_particle_system_datum *system;
	void *location;
};

// @retail 0x173c90
void __stdcall function_173c90(s_particle_render_request_173c90 const *request)
{
	s_particle_render_request_173c90 const *const *argument = &request;
	request = *argument;
	function_174f80(request->system, request->location, (void *)15, 0);
}

// @retail 0x173d80
void __stdcall function_173d80(long a, long b, long c, void *first, void *second, long system_index, void *unused)
{
	long const *a_argument = &a;
	long const *b_argument = &b;
	long const *c_argument = &c;
	void *const *unused_argument = &unused;
	a = *a_argument;
	b = *b_argument;
	c = *c_argument;
	unused = *unused_argument;
	void *const *first_argument = &first;
	void *const *second_argument = &second;
	long const *index_argument = &system_index;
	first = *first_argument;
	second = *second_argument;
	system_index = *index_argument;
	s_particle_system_datum *system = DATUM(g_510c74, s_particle_system_datum, system_index);
	long index = system->location_index;
	while (index != NONE)
	{
		s_particle_location_datum *location = DATUM(g_51ec8c, s_particle_location_datum, index);
		function_174f80(system, location, first, second);
		index = location->next_index;
	}
}

// @retail 0x173b90
long function_173b90(s_particle_system_datum *particle_system)
{
	return particle_system->flag10;
}

// @retail 0x173de0
void function_173de0(void)
{
	g_510c74 = data_new_inlined("particle_system", 0x80, sizeof(s_particle_system_datum), 0, g_510c2c);
	g_51ec84 = data_new_inlined("particles", 0x400, 0x40, 0, g_510c2c);
	g_51ec88 = data_new_inlined("particle_emitter", 0x100, 0x4c, 0, g_510c2c);
	g_51ec8c = data_new_inlined("particle_location", 0x100, sizeof(s_particle_location_datum), 0, g_510c2c);
}

// @retail 0x173ee0
void particle_systems_update_locations(void)
{
	s_record_pool_iterator iterator;
	short bsp_index = g_4686c4;
	s_particle_system_datum *particle_system;

	iterator.data = g_510c74;
	iterator.index = NONE;
	while ((particle_system = (s_particle_system_datum *)data_iterator_next_inlined(&iterator)) != 0)
	{
		if (particle_system->location.bsp_index != bsp_index)
		{
			s_location location;

			if (particle_system->location_index != NONE)
			{
				s_particle_location_datum *particle_location = DATUM(g_51ec8c, s_particle_location_datum, particle_system->location_index);

				if (bsp_index != NONE)
				{
					location.leaf_index = function_14a280(g_4e033c, 0, &particle_location->position);
					if (location.leaf_index != NONE)
						location.cluster_index = ((s_structure_bsp_173b90 *)g_4e0348)->leaves[location.leaf_index].cluster_index;
					else
						location.cluster_index = NONE;
				}
				else
				{
					location.leaf_index = NONE;
					location.cluster_index = NONE;
				}
			}
			else
			{
				location.leaf_index = NONE;
				location.cluster_index = NONE;
			}
			location.bsp_index = bsp_index;
			particle_system->set_location(&location);
		}
	}
}

// @retail 0x174180
void __stdcall function_174180(long particle_system_index)
{
	s_particle_system_datum *particle_system = DATUM(g_510c74, s_particle_system_datum, particle_system_index);
	long child_index;

	function_1753f0(particle_system);
	child_index = particle_system->first_child_index;
	while (child_index != NONE)
	{
		s_particle_system_datum *child = DATUM(g_510c74, s_particle_system_datum, child_index);
		long next_index = child->next_index;

		particle_system_unlink(child, &particle_system->first_child_index, &particle_system->last_child_index);
		function_174180(child_index);
		child_index = next_index;
	}
	if (particle_system->effect_index != NONE && TAG_GROUP(particle_system->tag_index) == 'effe')
		effect_remove_event_slot(particle_system->effect_index, particle_system_index);
	record_pool_release(g_510c74, particle_system_index);
}

PRIVATE __forceinline long data_scan_174990(s_record_pool *data, long index)
{
	if (index >= 0)
	{
		long count = data->high_water_index;
		if (index < count)
		{
			dword const *bits = data->bitmap;
			do
			{
				if (bits[index >> 5] & (1 << (index & 31)))
					return index;
				++index;
			} while (index < count);
		}
	}
	return NONE;
}

// @retail 0x174990
void __stdcall function_174990(real dt)
{
	struct
	{
		s_particle_system_datum *current;
		s_record_pool_iterator cursor;
	} iterator;
	iterator.cursor.data = g_510c74;
	iterator.cursor.index = NONE;

	for (;;)
	{
		long index = data_scan_174990(iterator.cursor.data, iterator.cursor.index + 1);
		if (index == NONE)
			break;

		iterator.current = (s_particle_system_datum *)(iterator.cursor.data->data + iterator.cursor.data->size * index);
		s_particle_system_datum *particle_system = iterator.current;
		iterator.cursor.datum_index = (particle_system->salt << 16) | index;
		iterator.cursor.index = index;

		if (particle_system->unknown4c == NONE && !function_174a30(particle_system, dt))
		{
			if (particle_system->effect_index != NONE)
			{
				function_17af80(particle_system->effect_index, iterator.cursor.datum_index);
				particle_system->effect_index = NONE;
			}
			function_174180(iterator.cursor.datum_index);
		}
	}
}

// @retail 0x175070
void particle_system_link(s_particle_system_datum *particle_system, long *last_index, long *first_index)
{
	s_record_pool *data = g_510c74;
	long particle_system_index = (particle_system->salt << 16) | (particle_system - (s_particle_system_datum *)data->data);

	particle_system->next_index = NONE;
	if (*first_index == NONE)
		*first_index = particle_system_index;
	if (*last_index != NONE)
	{
		s_particle_system_datum *last = DATUM(data, s_particle_system_datum, *last_index);

		last->next_index = particle_system_index;
		particle_system->previous_index = (last->salt << 16) | (last - (s_particle_system_datum *)data->data);
		*last_index = particle_system_index;
	}
	else
	{
		*last_index = particle_system_index;
		particle_system->previous_index = NONE;
	}
}

// @retail 0x175100
void particle_system_unlink(s_particle_system_datum *particle_system, long *first_index, long *last_index)
{
	s_record_pool *data = g_510c74;
	s_particle_system_datum *elements = (s_particle_system_datum *)data->data;
	long particle_system_index = (particle_system - elements) | (particle_system->salt << 16);

	if (particle_system->next_index != NONE)
		DATUM(data, s_particle_system_datum, particle_system->next_index)->previous_index = particle_system->previous_index;
	if (particle_system->previous_index != NONE)
		DATUM(data, s_particle_system_datum, particle_system->previous_index)->next_index = particle_system->next_index;
	if (*first_index == particle_system_index)
		*first_index = particle_system->next_index;
	if (*last_index == particle_system_index)
		*last_index = particle_system->previous_index;
	particle_system->next_index = NONE;
	particle_system->previous_index = NONE;
}

// @retail 0x175180
void s_particle_system_datum::set_location(s_location const *location)
{
	long child_index = first_child_index;

	this->location = *location;
	while (child_index != NONE)
	{
		s_particle_system_datum *child = DATUM(g_510c74, s_particle_system_datum, child_index);

		child->set_location(location);
		child_index = child->next_index;
	}
}

// @retail 0x1753a0
long function_1753a0(s_particle_system_datum *particle_system)
{
	long location_index = particle_system->location_index;
	long count = 0;

	if (location_index != NONE)
	{
		s_particle_location_datum *locations = (s_particle_location_datum *)g_51ec8c->data;

		do
		{
			s_particle_location_datum *location = &locations[location_index & 0xffff];

			count += function_248d50(location);
			location_index = location->next_index;
		} while (location_index != NONE);
	}
	return count;
}

// @retail 0x1753f0
void function_1753f0(s_particle_system_datum *particle_system)
{
	long index = particle_system->location_index;

	while (index != NONE)
	{
		long next_index = DATUM(g_51ec8c, s_particle_location_datum, index)->next_index;

		function_2486e0(index);
		index = next_index;
	}
	particle_system->location_index = NONE;
	particle_system->unknown34 = NONE;
}

// @retail 0x173ba0
void __stdcall function_173ba0(dword mask, void *a, void *b, void const *c, real *values)
{
	long index = 0;
	while (mask)
	{
		__asm
		{
			bsf ecx, mask
			mov index, ecx
		}
		mask &= ~(1 << index);
		values[index] = function_248df0(index, a, b, c);
	}
}

struct s_particle_value_cache
{
	real values[17];
	dword mask;
	void *field_48;
	void *field_4c;
	void const *field_50;
};

struct s_particle_emitter_175610
{
	short salt;
	word count;
	long first_particle_index;
	long next_index;
	byte unknown0c[0x40];
};

struct s_particle_175610
{
	short salt;
	word flags;
	long next_index;
	byte unknown08[0x14];
	point3f position;
	vector3f velocity;
	byte unknown34[0xc];
};

struct s_emitter_definition_175610
{
	byte unknown00[0x48];
	s_particle_property scale;
	s_particle_property color;
	s_particle_property alpha;
	byte unknown78[0x38];
	dword flags;
	dword mask;
};

struct s_particle_frame;
void function_248450(matrix3x3 *rotation, s_particle_frame const *frame, point3f *position, matrix3x3 const **rotation_result, point3f const **position_result, bool field_b4);
void function_246d80(s_particle_property const *property, real const *values, color3f *color);
real function_3eb70(void);
typedef void (__stdcall *particle_callback_175610)(s_particle_175610 *, s_particle_value_cache *, point3f const *, vector3f const *, real, color4f const *, void *);

PRIVATE inline void particle_rotate_175610(matrix3x3 const *matrix, vector3f const *input, vector3f *output)
{
	vector3f value = *input;
	output->i = matrix->left.i * value.j + matrix->forward.i * value.i + matrix->up.i * value.k;
	output->j = matrix->forward.j * value.i + matrix->up.j * value.k + matrix->left.j * value.j;
	output->k = matrix->forward.k * value.i + matrix->up.k * value.k + matrix->left.k * value.j;
}

// @retail 0x175610
void function_175610(s_particle_value_cache *cache, s_particle_system_datum *system, s_particle_location_datum *location, particle_callback_175610 callback, void *context)
{
	s_particle_system_datum * *system_reference = &system;
	s_particle_value_cache * *cache_reference = &cache;
	s_effect_particle_system_definition *definition = (*system_reference)->function_1751d0();
	if ((*system_reference) != (*cache_reference)->field_48)
	{
		(*cache_reference)->field_48 = (*system_reference);
		(*cache_reference)->mask &= 0xfffff98f;
	}
	if (location != (*cache_reference)->field_4c)
	{
		(*cache_reference)->field_4c = location;
		(*cache_reference)->mask &= 0xfffecf7f;
	}
	long emitter_index = *(long *)((byte *)location + 4);
	s_emitter_definition_175610 *emitter_definition = *(s_emitter_definition_175610 **)((byte *)definition + 0x34);
	while (emitter_index != NONE)
	{
		s_particle_emitter_175610 *emitter = DATUM(g_51ec88, s_particle_emitter_175610, emitter_index);
		long particle_index = emitter->first_particle_index;
		if (particle_index != NONE)
		{
			real multiplier = 1.0f;
			real scale = 1.0f;
			if (TEST_FIELD_BIT((*system_reference)->flag10))
				multiplier = function_3eb70();
			matrix3x3 rotation;
			point3f origin;
			matrix3x3 const *rotation_result;
			point3f const *origin_result;
			color4f color;
			if (definition->unknown0c)
				function_248450(&rotation, (s_particle_frame const *)emitter, &origin, &rotation_result, &origin_result, *((byte *)location + 2) != 0);
			dword mask = emitter_definition->mask & 0x107f0;
			function_173ba0(~(*cache_reference)->mask & mask, (*cache_reference)->field_48, (*cache_reference)->field_4c, (*cache_reference)->field_50, (*cache_reference)->values);
			(*cache_reference)->mask |= mask;
			if (emitter_definition->flags & 1)
				color.alpha = function_246cd0(&emitter_definition->alpha, (*cache_reference)->values);
			if (emitter_definition->flags & 2)
				function_246d80(&emitter_definition->color, (*cache_reference)->values, (color3f *)&color.red);
			if ((bool)((emitter_definition->flags >> 2) & 1))
				scale = function_246cd0(&emitter_definition->scale, (*cache_reference)->values) * multiplier;
			mask = emitter_definition->mask & 0xf80f;
			while (particle_index != NONE)
			{
				s_particle_175610 *particle = DATUM(g_51ec84, s_particle_175610, particle_index);
				point3f position = particle->position;
				vector3f velocity = particle->velocity;
				if (particle != (*cache_reference)->field_50)
				{
					(*cache_reference)->field_50 = particle;
					(*cache_reference)->mask &= 0xffff07f0;
				}
				function_173ba0(~(*cache_reference)->mask & mask, (*cache_reference)->field_48, (*cache_reference)->field_4c, (*cache_reference)->field_50, (*cache_reference)->values);
				(*cache_reference)->mask |= mask;
				particle_index = particle->next_index;
				if (!(emitter_definition->flags & 1))
					color.alpha = function_246cd0(&emitter_definition->alpha, (*cache_reference)->values);
				if (!(emitter_definition->flags & 2))
					function_246d80(&emitter_definition->color, (*cache_reference)->values, (color3f *)&color.red);
				if (!(bool)((emitter_definition->flags >> 2) & 1))
					scale = function_246cd0(&emitter_definition->scale, (*cache_reference)->values) * multiplier;
				if (definition->unknown0c)
				{
					particle_rotate_175610(rotation_result, (vector3f const *)&position, (vector3f *)&position);
					position.x = origin_result->x + position.x;
					position.y = origin_result->y + position.y;
					position.z = origin_result->z + position.z;
					particle_rotate_175610(rotation_result, &velocity, &velocity);
				}
				callback(particle, (*cache_reference), &position, &velocity, scale, &color, context);
			}
		}
		emitter_index = emitter->next_index;
		++emitter_definition;
	}
}

// @retail 0x173c10
void function_173c10(dword mask, s_particle_value_cache *cache)
{
	function_173ba0(~cache->mask & mask, cache->field_48, cache->field_4c, cache->field_50, cache->values);
	cache->mask |= mask;
}

struct s_248cc0;
struct s_247fd0;
void function_248cc0(s_248cc0 *location, s_247fd0 *values, s_particle_system_datum *system,
	point3f const *position, vector3f const *velocity);

PRIVATE inline real particle_clamp_175430(real const &value, real minimum, real maximum)
{
	return value < minimum ? minimum : value > maximum ? maximum : value;
}

// @retail 0x175430
void __stdcall function_175430(s_particle_system_datum *system, point3f const *position, vector3f const *velocity)
{
	if (particle_clamp_175430(position->x, -100000.0f, 100000.0f) != position->x ||
		particle_clamp_175430(position->y, -100000.0f, 100000.0f) != position->y ||
		particle_clamp_175430(position->z, -100000.0f, 100000.0f) != position->z ||
		particle_clamp_175430(velocity->i, -100.0f, 100.0f) != velocity->i ||
		particle_clamp_175430(velocity->j, -100.0f, 100.0f) != velocity->j ||
		particle_clamp_175430(velocity->k, -100.0f, 100.0f) != velocity->k)
		return;
	s_record_pool *locations = g_51ec8c;
	if (system->location_index == NONE)
	{
		long index = function_248620();
		if (index == NONE)
			return;
		s_particle_location_datum *location = DATUM(locations, s_particle_location_datum, index);
		function_248d90(location, &system->location_index, &system->unknown34);
		location->position = *position;
	}
	if (system->location_index != NONE)
	{
		s_particle_location_datum *location = DATUM(locations, s_particle_location_datum, system->location_index);
		s_particle_value_cache cache;
		cache.field_48 = system;
		cache.field_4c = 0;
		cache.field_50 = 0;
		cache.mask = 0;
		if (location != cache.field_4c)
		{
			cache.field_4c = location;
			cache.mask = 0;
		}
		function_173c10(0x1ffff, &cache);
		function_248cc0((s_248cc0 *)location, (s_247fd0 *)&cache, system, position, velocity);
	}
}

// @retail 0x173fd0
long function_173fd0(s_effect_particle_system_definition *definition, long effect_index, long tag_index, short definition_index, long event_index)
{
	long particle_system_index = NONE;

	if ((bool)((~(((byte *)definition)[0x16] >> 3)) & 1) && definition->tag_index != NONE && function_137bd0(definition->tag_index) && definition->unknown30 > 0)
	{
		if (effect_index == NONE || function_178af0(effect_index))
		{
			particle_system_index = record_pool_allocate(g_510c74);
			if (particle_system_index != NONE)
			{
				s_particle_system_datum *particle_system = DATUM(g_510c74, s_particle_system_datum, particle_system_index);

				*((word *)particle_system + 6) = 9;
				particle_system->flag4 = definition->flag0;
				byte &flags = ((byte *)particle_system)[0xc];
				if (definition->flag1)
					flags |= 0x20;
				else
					flags &= ~0x20;
				if (definition->flag2)
					flags |= 0x40;
				else
					flags &= ~0x40;
				g_4e7408->seed = g_4e7408->seed * 1664525 + 1013904223;
				particle_system->random_a = (real)(g_4e7408->seed >> 16) * (1.0f / 65535.0f);
				g_4e7408->seed = g_4e7408->seed * 1664525 + 1013904223;
				particle_system->random_b = (real)(g_4e7408->seed >> 16) * (1.0f / 65535.0f);
				particle_system->tag_index = tag_index;
				particle_system->effect_index = effect_index;
				particle_system->next_index = NONE;
				particle_system->previous_index = NONE;
				particle_system->location_index = NONE;
				particle_system->unknown34 = NONE;
				particle_system->definition_index = definition_index;
				particle_system->event_index = event_index;
				particle_system->first_child_index = NONE;
				particle_system->last_child_index = NONE;
				particle_system->parent = 0;
				particle_system->unknown4c = NONE;
				particle_system->color = 0xff808080;
			}
			else
			{
				long game_time = g_510c54->game_time;

				if (game_time - g_47ff88 > g_510c54->field_2_3 * 60)
					g_47ff88 = game_time;
			}
			if (particle_system_index != NONE && effect_index != NONE && TAG_GROUP(tag_index) == 'effe')
				function_178b30(effect_index, (long)definition, particle_system_index);
		}
	}
	return particle_system_index;
}

// @retail 0x1751d0
s_effect_particle_system_definition *s_particle_system_datum::function_1751d0()
{
	long index = tag_index;
	switch (TAG_GROUP(index))
	{
	case 'effe':
		return &TAG_GET(s_effect_definition, index)->events[event_index].particle_systems[definition_index];
	case 'bsdt':
		return (s_effect_particle_system_definition *)(*(byte **)(g_4e3b44[index & 0xffff].bytes + 0x18) + definition_index * sizeof(s_effect_particle_system_definition));
	case 'PRTM':
	case 'prt3':
		{
		c_type_4e7709 *definition = function_137bd0(parent->function_1751d0()->tag_index);
		long element = definition_index;
		return definition->function_1751d0((word)element);
	}
	}
	return 0;
}

// @retail 0x175270
void function_175270(s_particle_system_datum *particle_system, s_particle_system_spawn *spawn, transform4x3f const *matrix, bool field_b4)
{
	struct
	{
		dword mask;
		s_particle_system_datum *particle_system;
		s_particle_location_datum *particle_location;
		void *unknown0c;
	} query;
	real values[17];
	s_particle_location_datum *particle_location;

	*((word *)particle_system + 6) |= 0x101;
	query.mask = 0;
	query.particle_system = 0;
	query.particle_location = 0;
	query.unknown0c = 0;
	if (spawn->location_index == NONE)
	{
		spawn->location_index = function_248620();
		if (spawn->location_index == NONE)
			return;
		particle_location = DATUM(g_51ec8c, s_particle_location_datum, spawn->location_index);
		function_248d90(particle_location, &particle_system->location_index, &particle_system->unknown34);
		particle_location->position = matrix->position;
	}
	particle_system->unknown04 = (spawn->unknown + spawn->scale) * particle_system->unknown08;
	particle_location = DATUM(g_51ec8c, s_particle_location_datum, spawn->location_index);
	if (particle_system != query.particle_system)
	{
		query.particle_system = particle_system;
		query.mask &= 0xfffff98f;
	}
	if (particle_location != query.particle_location)
	{
		query.particle_location = particle_location;
		query.mask &= 0xfffecf7f;
	}
	function_173ba0(~query.mask & 0x1ffff, query.particle_system, query.particle_location, matrix, values);
	query.mask |= 0x1ffff;
	function_248970(particle_location, field_b4, spawn->unknown, particle_system, values, matrix);
	spawn->location_index = particle_location->next_index;
}

// @retail 0x175a80
void function_175a80(bool tinted, dword color_a, dword color_b, s_particle_system_datum *particle_system, bool multiplied)
{
	if (!multiplied)
	{
		if (tinted)
		{
			color3f color;

			unpack_color3f(color_b, &color);
			color.red *= 0.5f;
			color.green *= 0.5f;
			color.blue *= 0.5f;
			particle_system->color = pack_color3f(&color);
		}
	}
	else if (tinted)
	{
		color3f color;
		color3f other;

		unpack_color3f(color_a, &color);
		unpack_color3f(color_b, &other);
		color.red *= other.red;
		color.green *= other.green;
		color.blue *= other.blue;
		particle_system->color = pack_color3f(&color);
	}
	else
	{
		particle_system->color = color_a;
	}
}

extern dword g_4c56c0[64];
extern vector3f *g_4687bc;
extern vector3f *g_4687b4;
real function_30bf0(vector3f *vector);
vector3f *function_11d000(vector3f const *vector, vector3f *out);
long function_1753a0(s_particle_system_datum *system);

class c_248767 : public s_effect_particle_system_definition
{
public:
	void function_248750(s_particle_system_datum *system, real dt, byte *location);
};

PRIVATE inline void particle_cross_174a30(vector3f const *forward, vector3f const *up, vector3f *out)
{
	out->i = up->k * forward->j - up->j * forward->k;
	out->j = up->i * forward->k - up->k * forward->i;
	out->k = up->j * forward->i - forward->j * up->i;
}

class c_particle_tag_view_174a30
{
public:
	virtual void v0() = 0;
	virtual void v1() = 0;
	virtual void v2() = 0;
	virtual void v3() = 0;
	virtual void v4() = 0;
	virtual void v5() = 0;
	virtual void v6() = 0;
	virtual void v7() = 0;
	virtual long count() = 0;
	virtual long const *names() = 0;
};

// @retail 0x174a30
bool function_174a30(s_particle_system_datum *particle_system, real dt)
{
	(void)&dt;
	word flags = *((word *)particle_system + 6);
	if (flags & 0x100)
		flags |= 0x200;
	else
		flags &= ~0x200;
	*((volatile word *)particle_system + 6) = flags;
	particle_system->flag8 = false;
	flags = *((volatile word *)particle_system + 6);
	if (!(flags & 0x20) &&
		!(g_4c56c0[particle_system->location.cluster_index >> 5] &
		(1 << (particle_system->location.cluster_index & 31))))
		function_1753f0(particle_system);
	s_effect_particle_system_definition *definition = particle_system->function_1751d0();
	long location_index = particle_system->location_index;
	while (location_index != NONE)
	{
		s_particle_location_datum *location = DATUM(g_51ec8c, s_particle_location_datum, location_index);
		((c_248767 *)definition)->function_248750(particle_system, dt, (byte *)location);
		location_index = location->next_index;
	}
	c_particle_tag_view_174a30 *tag = (c_particle_tag_view_174a30 *)function_137bd0(definition->tag_index);
	long const *names = tag->names();
	long child_index = particle_system->first_child_index;
	while (child_index != NONE)
	{
		s_particle_system_datum *child = DATUM(g_510c74, s_particle_system_datum, child_index);
		long next_index = child->next_index;
		byte *particle = record_pool_lookup_checked(g_51ec84, child->unknown4c);
		if (particle)
		{
			transform4x3f matrix;
			long index = child->unknown24;
			if (index != NONE && index < tag->count())
			{
				long name = names[index];
				switch (name)
				{
				case 0x20000ca:
					matrix.forward = *g_4687b0;
					matrix.up = *g_4687a8;
					matrix.left = *g_4687ac;
					break;
				case 0x70000c0:
					matrix.forward = *g_4687bc;
					matrix.up = *g_4687b4;
					matrix.left = *g_4687ac;
					break;
				default:
				{
					vector3f const *velocity = (vector3f *)(particle + 0x28);
					matrix.forward.i = velocity->i * -1.0f;
					matrix.forward.j = velocity->j * -1.0f;
					matrix.forward.k = velocity->k * -1.0f;
					function_30bf0(&matrix.forward);
					function_11d000(&matrix.forward, &matrix.up);
					particle_cross_174a30(&matrix.forward, &matrix.up, &matrix.left);
					break;
				}
				}
			}
			else
			{
				vector3f const *velocity = (vector3f *)(particle + 0x28);
				matrix.forward.i = velocity->i * -1.0f;
				matrix.forward.j = velocity->j * -1.0f;
				matrix.forward.k = velocity->k * -1.0f;
				real magnitude = (real)sqrt(matrix.forward.j * matrix.forward.j +
					matrix.forward.i * matrix.forward.i + matrix.forward.k * matrix.forward.k);
				if (!(fabs(magnitude) < 0.0001f))
				{
					real inverse = 1.0f / magnitude;
					matrix.forward.i *= inverse;
					matrix.forward.j *= inverse;
					matrix.forward.k *= inverse;
				}
				real x = (real)fabs(matrix.forward.i);
				real y = (real)fabs(matrix.forward.j);
				real z = (real)fabs(matrix.forward.k);
				if (y >= x && z >= x)
				{
					matrix.up.i = 0.0f;
					matrix.up.j = matrix.forward.k;
					matrix.up.k = 0.0f - matrix.forward.j;
				}
				else if (z >= y)
				{
					matrix.up.i = 0.0f - matrix.forward.k;
					matrix.up.j = 0.0f;
					matrix.up.k = matrix.forward.i;
				}
				else
				{
					matrix.up.i = matrix.forward.j;
					matrix.up.j = 0.0f - matrix.forward.i;
					matrix.up.k = 0.0f;
				}
				particle_cross_174a30(&matrix.forward, &matrix.up, &matrix.left);
			}
			matrix.scale = 1.0f;
			matrix.position = *(point3f *)(particle + 0x1c);
			dword color = particle_system->color;
			s_particle_system_spawn spawn;
			spawn.scale = *(real *)(particle + 8) / *(real *)(particle + 0xc) - dt;
			spawn.unknown = dt;
			spawn.location_index = child->location_index;
			c_type_4e7709 *child_tag = function_137bd0(child->function_1751d0()->tag_index);
			bool tinted = child_tag->tinted();
			function_175a80(tinted, color, 0xffffffff, child, child_tag->multiplied());
			function_175270(child, &spawn, &matrix, false);
		}
		if (!function_174a30(child, dt))
		{
			s_particle_system_datum *volatile parent = particle_system;
			particle_system_unlink(child, &parent->first_child_index, &parent->last_child_index);
			function_174180(child_index);
		}
		child_index = next_index;
	}
	particle_system->flag0 = false;
	if (function_1753a0(particle_system) > 0 && TEST_FIELD_BIT(particle_system->flag3))
		return true;
	if (particle_system->flag0 || particle_system->first_child_index != NONE || particle_system->flag9)
		return true;
	return false;
}
