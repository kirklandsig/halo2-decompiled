// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0CC2B0.CPP: whether a point is within a cone around a unit's
   facing, seen from its head (units.cpp; an outside function lane A's script
   functions need) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "object_markers.h"
#include <math.h>

/* the unit's facing vector (+0x18c) */
struct s_unit_0cc2b0
{
	byte unknown000[0x18c];
	vector3f facing;
};

struct s_object_header_0cc2b0
{
	byte unknown00[8];
	s_unit_0cc2b0 *object;
};

real function_30bf0(vector3f *v);

// @retail 0xcc2b0
bool function_cc2b0(long unit_index, point3f const *point, real angle)
{
	bool result = false;

	if (unit_index != NONE)
	{
		s_unit_0cc2b0 *unit = ((s_object_header_0cc2b0 *)g_4e0300->data)[unit_index & 0xffff].object;
		s_object_marker marker;
		point3f head;
		vector3f vector;

		function_b8d30(unit_index, 0x4000095, &marker, 1, false);
		head = marker.matrix.position;
		vector.i = point->x - head.x;
		vector.j = point->y - head.y;
		vector.k = point->z - head.z;
		function_30bf0(&vector);
		if (vector.k * unit->facing.k + vector.j * unit->facing.j + vector.i * unit->facing.i > cos(angle))
			result = true;
	}
	return result;
}

/* the unit's weapons (+0x218) and a weapon's definition */
struct s_unit_weapons_0cd5c0
{
	byte unknown000[0x218];
	long weapon_object_indices[4];
};

struct s_object_0cd5c0
{
	long definition_index;
};

struct s_object_header_0cd5c0
{
	byte unknown00[8];
	void *object;
};

/* whether a unit holds a weapon of the given definition */
// @retail 0xcd5c0
bool unit_has_weapon_definition(long unit_index, long definition_index)
{
	s_object_header_0cd5c0 *headers = (s_object_header_0cd5c0 *)g_4e0300->data;
	s_unit_weapons_0cd5c0 *unit = (s_unit_weapons_0cd5c0 *)headers[unit_index & 0xffff].object;
	bool result = false;

	for (long i = 0; i < 4; i++)
	{
		long weapon_index = unit->weapon_object_indices[i];
		if (weapon_index != NONE && ((s_object_0cd5c0 *)headers[weapon_index & 0xffff].object)->definition_index == definition_index)
		{
			result = true;
			break;
		}
	}
	return result;
}
