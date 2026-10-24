// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0C8880.CPP: a unit's zoom magnification (units.cpp; an outside
   function the player control code in 0x187130 and 0x185be0 needs) */

#include "unknown_11c920.h"
#include "globals.h"

/* a unit, as this file reads it */
struct s_zoom_unit
{
	long definition_index;
	byte unknown04[0x212 - 4];
	char current_weapon_index;
	byte unknown213[0x218 - 0x213];
	long weapon_indices[4];
};

struct s_zoom_object_header
{
	byte unknown00[8];
	s_zoom_unit *object;
};

/* a weapon's tag */
struct s_zoom_weapon_definition
{
	byte unknown00[0x12e];
	byte flags12e;
};

/* the zoom levels of the globals tag (0xb8 of the block at +0x134) */
struct s_zoom_globals
{
	byte unknown00[0xb8];
	long zoom_level_count;
	real magnification_minimum;
	real magnification_maximum;
};

#define ZOOM_UNIT(index) (((s_zoom_object_header *)g_4e0300->data)[(index) & 0xffff].object)

bool function_100f70(long weapon_index);
real function_101090(long weapon_index, short field_240);

// @retail 0xc8880
real function_c8880(long unit_index, short field_240)
{
	s_zoom_unit *unit = ZOOM_UNIT(unit_index);
	real fraction = 1.0f;
	real result = fraction;
	short field_x11c898 = unit->current_weapon_index;

	if (field_x11c898 != NONE)
	{
		long weapon_index = unit->weapon_indices[field_x11c898];

		if (weapon_index != NONE)
		{
			if (function_100f70(weapon_index))
			{
				return function_101090(weapon_index, field_240);
			}

			bool unzoomed = (((s_zoom_weapon_definition *)g_4e3b44[ZOOM_UNIT(weapon_index)->definition_index & 0xffff].data)->flags12e & 1) != 0;
			/* retail stores the flag in its stack slot (and never reads it
			   back there): its address is taken */
			bool const *unzoomed_reference = &unzoomed;

			if (unzoomed)
			{
				return result;
			}
		}
	}

	s_zoom_globals *globals = *(s_zoom_globals **)((byte *)g_4e034c + 0x134);
	if (field_240 != NONE)
	{
		if (globals->zoom_level_count != 1)
		{
			fraction = (real)field_240 / (real)(globals->zoom_level_count - 1);
		}
		result = (globals->magnification_maximum - globals->magnification_minimum) * fraction + globals->magnification_minimum;
	}
	return result;
}
