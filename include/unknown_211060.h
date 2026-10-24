/* UNKNOWN_211060.H: moving a point through a medium, with bounces
   (src/unknown_211060.cpp, docs/unknown_211060.md) */

#ifndef UNKNOWN_211060_H
#define UNKNOWN_211060_H

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

struct s_location;

long function_211060(vector3f *velocity, dword flags, void const *mover_settings, s_location *location,
	long cell_slot, point3f *position, vector3f const *force, vector3f *normal, short *material_type, real radius,
	real dt);

#endif
