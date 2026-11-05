/* OBJECT_LIST.H: g_5107f0, the list of moving objects whose velocity carries
   the objects attached to them (defined in src/object_list.cpp, queried in
   src/unknown_109e00.cpp), and the point velocity of such an object */

#ifndef OBJECT_LIST_H
#define OBJECT_LIST_H

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

/* an object of the list (0xe8 bytes) */
struct s_object_list_entry
{
	byte unknown00[0xc];
	point3f center;
	byte unknown18[0x24 - 0x18];
	vector3f linear_velocity;
	byte unknown30[0x3c - 0x30];
	vector3f angular_velocity;
	byte unknown48[0xe5 - 0x48];
	bool active;
	byte unknowne6[0xe8 - 0xe6];
};

struct s_object_list_state
{
	s_object_list_entry entries[32];
	long object_indices[32];
	long object_count;
	bool locked;
};

extern s_object_list_state *g_5107f0;

/* the velocity at point of a body moving about center with the given linear
   and angular velocities: linear + angular x (point - center); the cross
   product is computed in full before result is written */
static inline void carrier_point_velocity(point3f const *center, vector3f const *linear,
	vector3f const *angular, point3f const *point, vector3f *result)
{
	vector3f const *velocity = linear;
	vector3f offset;
	offset.i = point->x - center->x;
	offset.j = point->y - center->y;
	offset.k = point->z - center->z;
	real k = angular->i * offset.j - angular->j * offset.i;
	real j = angular->k * offset.i - angular->i * offset.k;
	real i = angular->j * offset.k - angular->k * offset.j;
	result->i = i;
	result->j = j;
	result->k = k;
	result->i = velocity->i + result->i;
	result->j = velocity->j + result->j;
	result->k = velocity->k + result->k;
}

#endif
