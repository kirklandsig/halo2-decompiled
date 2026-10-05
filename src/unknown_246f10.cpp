#include "effects.h"
#include "sound_sources.h"

// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_object_246eb0
{
	byte unknown00[0x28];
	vector3f vector;
};

struct s_particle_impact
{
	point3f position;
	vector3f direction;
	vector3f normal;
	short index;
};

extern vector3f *g_4687bc;
real function_1201a0(vector3f *v, vector3f const *fallback);
void function_1763a0(point3f const *point, vector3f const *direction, s_effect_marker *markers, vector3f const *normal);
real function_246eb0(s_object_246eb0 const *object);
dword vector3d_compress(vector3f const *vector);
void function_11bed0(s_location *location, point3f const *point);
long function_1895f0(s_sound_position const *position, real scale, long tag_index);

// @retail 0x246eb0
real function_246eb0(s_object_246eb0 const *object)
{
	real r = (real)sqrt(object->vector.i * object->vector.i + object->vector.j * object->vector.j + object->vector.k * object->vector.k) - 0.5f;
	if (r < 0.f)
	{
		return 0.f;
	}
	if (r > 1.f)
	{
		r = 1.f;
	}
	return r;
}

// @retail 0x246f10
void function_246f10(s_object_246eb0 const *particle, s_particle_impact const *impact, s_effect_marker *markers)
{
	if (impact)
		function_1763a0(&impact->position, &impact->direction, markers, &impact->normal);
	else
	{
		point3f position = *(point3f const *)((byte const *)particle + 0x1c);
		vector3f normal = *g_4687b0;
		vector3f direction = particle->vector;
		function_1201a0(&direction, g_4687bc);
		function_1763a0(&position, &direction, markers, &normal);
	}
}

// @retail 0x247000
void function_247000(s_object_246eb0 const *particle, volatile long tag_index)
{
	s_sound_position position;
	position.position = *(point3f const *)((byte const *)particle + 0x1c);
	position.compressed_forward = vector3d_compress(g_4687a8);
	position.velocity = *g_4687a4;
	function_11bed0(&position.location, &position.position);
	real scale = function_246eb0(particle);
	function_1895f0(&position, scale, tag_index);
}

void function_176a50(s_effect_parameters *parameters, long tag_index, long marker_count, s_effect_marker *markers, long mode);
long __stdcall effect_new_from_parameters(s_effect_parameters *parameters);

// @retail 0x246fa0
void function_246fa0(s_object_246eb0 const *particle, s_particle_impact const *impact, long tag_index)
{
	s_effect_marker markers[6];
	function_246f10(particle, impact, markers);
	real scale = function_246eb0(particle);
	s_effect_parameters parameters;
	parameters.flags = 0;
	function_176a50(&parameters, tag_index, 6, markers, 0);
	parameters.scale_a = scale;
	parameters.scale_b = scale;
	effect_new_from_parameters(&parameters);
}

/* The particle definition's material-selection interface. */
class c_particle_material_interface
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
	virtual void v8() = 0;
	virtual void v9() = 0;
	virtual void v10() = 0;
	virtual void v11() = 0;
	virtual bool v12() = 0;
};

void function_188180(point3f const *point, vector3f const *forward, long tag_index, long object_index, long index, long variant,
	long unused, long effect_value, s_location const *location, real scale);

// @retail 0x247070
void function_247070(s_object_246eb0 const *particle, s_particle_impact const *impact, long definition_index, long tag_index)
{
	s_location location;
	function_11bed0(&location, (point3f const *)((byte const *)particle + 0x1c));
	if (impact)
	{
		c_particle_material_interface *definition = (c_particle_material_interface *)function_137bd0(definition_index);
		long variant = definition->v12() ? 0 : NONE;
		short index = impact->index;
		real scale = function_246eb0(particle);
		function_188180(&impact->position, &impact->normal, tag_index, NONE, 10, variant,
			index, (long)&impact->direction, &location, scale);
	}
}

// @retail 0x246e60
void function_246e60(long tag_index, dword group, s_particle_impact const *impact, s_object_246eb0 const *particle, long definition_index)
{
	if (tag_index != NONE)
	{
		switch (group)
		{
		case 'effe': function_246fa0(particle, impact, tag_index); break;
		case 'foot': function_247070(particle, impact, definition_index, tag_index); break;
		case 'snd!': function_247000(particle, tag_index); break;
		}
	}
}
