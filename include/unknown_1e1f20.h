/* UNKNOWN_1E1F20.H: the weapons of a unit and of an actor's character
   (outside functions lane I's firing position evaluators call:
   unknown_0e5280.cpp, unknown_1e1f20.cpp, character_tag_accessors.cpp) */

#ifndef UNKNOWN_1E1F20_H
#define UNKNOWN_1E1F20_H

#include "unknown_11c920.h"
#include "globals.h"

/* an object header of g_4e0300 (12 bytes, the object at +8) */
struct s_object_header_weapon_view
{
	byte unknown00[8];
	byte *object;
};

/* the unit fields these functions read */
struct s_unit_weapon_view
{
	long tag_index;
	byte unknown004[0x14 - 0x4];
	long parent_index;
	byte unknown018[0xaa - 0x18];
	byte type;
	byte unknown0ab[0x212 - 0xab];
	char current_weapon_index;
	char next_weapon_index;
	byte unknown214[0x218 - 0x214];
	long weapon_indices[4];
	byte unknown228[0x24c - 0x228];
	long driver_index;
};

inline s_unit_weapon_view *unit_weapon_view_get(long object_index)
{
	return (s_unit_weapon_view *)((s_object_header_weapon_view *)g_4e0300->data)[object_index & 0xffff].object;
}

/* the weapon a unit holds */
inline long unit_get_current_weapon(long unit_index)
{
	s_unit_weapon_view *unit = unit_weapon_view_get(unit_index);
	short weapon = unit->current_weapon_index;
	long result = NONE;

	if (weapon != NONE)
	{
		result = unit->weapon_indices[weapon];
	}
	return result;
}

/* how a character uses a weapon (0xcc bytes) */
struct s_character_weapon
{
	byte unknown00[8];
	long weapon_tag_index;
	real unknown0c;
	real unknown10;
	real unknown14;
	real unknown18;
	byte unknown1c[0x24 - 0x1c];
	real unknown24;
	real unknown28;
	real unknown2c;
	real unknown30;
	byte unknown34[0xcc - 0x34];
};

long function_e5280(long unit_index);
/* lane M's definitions (unknown_1e1f20.cpp, character_tag_accessors.cpp; declared
   in ai_actor.h too): 0x1e5280 returns an s_character_weapon */
long function_1e1f20(long actor_index);
void *function_1e5280(long actor_index, long key);

#endif
