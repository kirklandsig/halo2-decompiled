#ifndef UNKNOWN_246CD0_H
#define UNKNOWN_246CD0_H

#include "unknown_11c920.h"

struct s_tag_data
{
	long size;
	byte *address;
};

struct s_particle_property
{
	short input_index;
	short range_index;
	word modifier;
	short modifier_index;
	s_tag_data function;
};

real function_246cd0(s_particle_property const *property, real const *values);

#endif
