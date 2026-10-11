// @flags /O2 /Gr
/* UNKNOWN_0E5280.CPP: the weapon a unit fires: the weapon of the vehicle it
   drives, else its own (an outside function lane I's firing position
   evaluators call) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1e1f20.h"

long __stdcall function_cbd80(long object_index, long *holder_index);

// @retail 0xe5280
long function_e5280(long unit_index)
{
	s_unit_weapon_view *unit = unit_weapon_view_get(unit_index);

	if (unit->parent_index != NONE)
	{
		s_unit_weapon_view *parent = unit_weapon_view_get(unit->parent_index);

		char parent_type = parent->type;
		if (((1 << parent_type) & 3) && parent->driver_index == unit_index)
		{
			long weapon_index = function_cbd80(unit->parent_index, 0);

			if (weapon_index != NONE)
			{
				return weapon_index;
			}
		}
	}
	return unit_get_current_weapon(unit_index);
}
