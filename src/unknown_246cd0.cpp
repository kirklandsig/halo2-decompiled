#include "unknown_246cd0.h"
#include "unknown_11c920.h"
#include "globals.h"

// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_particle_property_header
{
	byte type;
	byte flags;
	byte unknown02[2];
	real lower;
	real upper;
};

real function_13b390(void const *function, real input, real range);
void function_13be80(s_tag_data const *function, real input, color3f *color);

PRIVATE inline real particle_property_input(real const *values, short index)
{
	return values[index];
}

PRIVATE __forceinline void particle_property_evaluate(s_tag_data const *function, real input, real range, real &result)
{
	result = function_13b390(function, input, range);
	s_particle_property_header const *header = (s_particle_property_header const *)function->address;
	if (!(header->flags & 0xf0))
	{
		real lower = header->lower;
		real upper = header->upper;
		if (result < 0.f)
			result = 0.f;
		else if (result > 1.f)
			result = 1.f;
		result = lower + (upper - lower) * result;
	}
}

// @retail 0x246cd0
real function_246cd0(s_particle_property const *property, real const *values)
{
	real result;
	particle_property_evaluate(&property->function,
		particle_property_input(values, property->input_index), particle_property_input(values, property->range_index), result);
	short index = property->modifier_index;
	if (index != NONE && index >= 0)
	{
		switch (property->modifier)
		{
		case 1: result = values[index] + result; break;
		case 2: result = values[index] * result; break;
		}
	}
	return result;
}

// @retail 0x246d80
void function_246d80(s_particle_property const *property, real const *values, color3f *color)
{
	s_particle_property_header const *header = (s_particle_property_header const *)property->function.address;
	real input = 0.f;
	real range;
	if (header->flags & 1)
		range = values[property->range_index];
	else
		range = input;
	if (header->type != 1)
		input = values[property->input_index];
	real result = function_13b390(&property->function, input, range);
	short index = property->modifier_index;
	if (index != NONE && index >= 0)
	{
		switch (property->modifier)
		{
		case 1: result = values[index] + result; break;
		case 2: result = values[index] * result; break;
		}
	}
	function_13be80(&property->function, result, color);
}

struct s_particle_rate_definition
{
	byte unknown00[8];
	s_particle_property rate;
};

// @retail 0x247100
real function_247100(s_particle_rate_definition const *definition, real const *values)
{
	return function_246cd0(&definition->rate, values) * 30.f;
}
