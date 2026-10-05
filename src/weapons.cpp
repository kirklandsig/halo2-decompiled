// @flags /O2 /arch:SSE /Gr
/* WEAPONS.CPP: weapon state queries and updates (barrels, triggers,
   magazines, zoom). The functions of 0x101e80..0x102100 are in
   unknown_101e80.cpp. */

#include "unknown_11c920.h"
#include "globals.h"
#include "object_markers.h"
#include <math.h>
#include "unknown_1dacb0.h"
#include "effects.h"
#include "unknown_1cafc0.h"

/* the weapon definition (the tag data) */
struct s_weapon_magazine_definition
{
	byte flags;
	byte unknown01[0xa - 1];
	short rounds_loaded_maximum;
	short rounds_total_maximum;
	byte unknown0e[0x14 - 0xe];
	short rounds_reloaded;
	byte unknown16[0x18 - 0x16];
	real value_18;
	byte unknown1c[0x38 - 0x1c];
	long reloading_effect;
	byte unknown3c[0x5c - 0x3c];
};

struct s_weapon_trigger_definition
{
	byte unknown00[6];
	short behavior;
	byte unknown08[2];
	short primary_barrel;
	byte unknown0c[0x1c - 0xc];
	real value_1c;
	byte unknown20[0x2c - 0x20];
	real charging_time;
	byte unknown30[0x34 - 0x30];
	long effect_tag_index;
	byte unknown38[0x40 - 0x38];
};

struct s_weapon_barrel_definition
{
	dword flag0 : 1;
	dword flag1 : 1;
	dword flag2 : 1;
	dword flag3 : 1;
	dword flag4 : 1;
	dword flag5 : 1;
	dword flag6 : 1;
	dword flag7 : 1;
	dword flag8 : 1;
	dword flag9 : 1;
	dword flag10 : 1;
	dword flag11 : 1;
	dword : 20;
	byte unknown04[0x20 - 4];
	real value_20;
	real value_24;
	byte unknown28[0x30 - 0x28];
	long name;
	byte unknown34[0x90 - 0x34];
	long projectile_definition_index;
	byte unknown94[0x9c - 0x94];
	real value_9c;
	byte unknowna0[0xa8 - 0xa0];
	real value_a8;
	byte unknownac[0xe4 - 0xac];
	long firing_effect_count;
	struct s_weapon_firing_effect *firing_effects;
};

/* a firing effect of a barrel */
struct s_weapon_firing_effect
{
	byte unknown00[0x18];
	long effect_tag_index;
};

/* an animation graph per player character (16 bytes) */
struct s_weapon_player_animation
{
	byte unknown00[0xc];
	long graph_index;
};

struct s_weapon_definition
{
	byte unknown000[0x38];
	long model_index;
	byte unknown03c[0x12c - 0x3c];
	dword flag0 : 1;
	dword flag1 : 1;
	dword flag2 : 1;
	dword flag3 : 1;
	dword flag4 : 1;
	dword flag5 : 1;
	dword prevents_grenade_throwing : 1;
	dword flag7 : 1;
	dword flag8 : 1;
	dword flag9 : 1;
	dword flag10 : 1;
	dword flag11 : 1;
	dword flag12 : 1;
	dword flag13 : 1;
	dword flag14 : 1;
	dword flag15 : 1;
	dword flag16 : 1;
	dword flag17 : 1;
	dword flag18 : 1;
	dword flag19 : 1;
	dword flag20 : 1;
	dword flag21 : 1;
	dword flag22 : 1;
	dword flag23 : 1;
	dword flag24 : 1;
	dword flag25 : 1;
	dword flag26 : 1;
	dword flag27 : 1;
	dword flag28 : 1;
	dword flag29 : 1;
	dword flag30 : 1;
	dword flag31 : 1;
	byte unknown130[0x1fe - 0x130];
	short zoom_level_count;
	real zoom_magnification_minimum;
	real zoom_magnification_maximum;
	byte unknown208[0x288 - 0x208];
	long animation_weapon_class;
	long animation_weapon_type;
	short value_290;
	short reload_style;
	short value_294;
	byte unknown296[0x2a8 - 0x296];
	long player_animation_count;
	s_weapon_player_animation *player_animations;
	byte unknown2b0[0x2c0 - 0x2b0];
	long magazine_count;
	s_weapon_magazine_definition *magazines;
	long trigger_count;
	s_weapon_trigger_definition *triggers;
	long barrel_count;
	s_weapon_barrel_definition *barrels;
	byte unknown2d8[0x2fc - 0x2d8];
	long overheated_effect;
	byte unknown300[4];
	long overheated_definition_index;
};

/* the weapon (the object data) */
struct s_weapon_barrel
{
	char timer;
	byte state;
	short ticks;
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word flag3 : 1;
	word flag4 : 1;
	word flag5 : 1;
	word flag6 : 1;
	word flag7 : 1;
	word : 8;
	word value06;
	byte unknown08[0x14 - 8];
	real value14;
	byte unknown18[0x28 - 0x18];
	real accumulator;
	byte unknown2c[0x34 - 0x2c];
};

struct s_weapon_trigger
{
	byte state;
	byte unknown01;
	short timer;
	dword flag0 : 1;
	dword flag1 : 1;
	dword flag2 : 1;
	dword flag3 : 1;
	dword flag4 : 1;
	dword flag5 : 1;
	dword flag6 : 1;
	dword flag7 : 1;
	dword : 24;
	long effect_index;
};

struct s_weapon_magazine
{
	short state;
	short ticks;
	byte unknown04[2];
	short rounds_unloaded;
	short rounds_loaded;
	byte unknown0a[2];
	short ticks_0c;
	short ticks_0e;
};

struct s_weapon
{
	long definition_index;
	byte unknown004[0x14 - 4];
	long parent_index;
	byte unknown018[0x12a - 0x18];
	short animation_state_offset;
	union
	{
		byte item_flags;
		struct
		{
			byte in_inventory : 1;
			byte item_flag1 : 1;
			byte item_flag2 : 1;
			byte item_flag3 : 1;
			byte item_flag4 : 1;
			byte item_flag5 : 1;
			byte item_flag6 : 1;
			byte item_flag7 : 1;
		};
	};
	byte unknown12d[0x154 - 0x12d];
	long unit_index;
	byte unknown158[0x16e - 0x158];
	short value_16e;
	byte value_170;
	byte unknown171[0x177 - 0x171];
	byte entry_177;
	long state;
	short state_ticks;
	byte unknown17e[0x184 - 0x17e];
	real heat;
	byte unknown188[0x194 - 0x188];
	long object_index_194;
	byte unknown198[0x1a4 - 0x198];
	s_weapon_barrel barrels[2];
	s_weapon_trigger triggers[2];
	s_weapon_magazine magazines[2];
	byte unknown244[0x24c - 0x244];
	long time_24c;
	long value250;
	c_type_709360 animation_254;
	real value258;
};

struct s_weapon_header
{
	byte unknown00[8];
	s_weapon *weapon;
};

#define WEAPON_GET(index) (((s_weapon_header *)g_4e0300->data)[(index) & 0xffff].weapon)
#define WEAPON_DEFINITION(weapon) ((s_weapon_definition *)g_4e3b44[(weapon)->definition_index & 0xffff].bytes)

/* the unit holding a weapon (a view of the unit) */
struct s_weapon_unit
{
	byte unknown000[0xaa];
	byte object_type;
	byte unknown0ab[0x12a - 0xab];
	short animation_state_offset;
	byte unknown12c[0x13c - 0x12c];
	long player_index;
	byte unknown140[0x212 - 0x140];
	char current_weapon_slot;
	char other_weapon_slot;
	byte unknown214[0x218 - 0x214];
	long weapon_indices[4];
};

struct s_weapon_unit_header
{
	byte unknown00[8];
	s_weapon_unit *unit;
};

/* a sound event of a weapon's animation */
struct s_weapon_sound_event
{
	short type;
	byte unknown02[2];
	long sound_index;
	byte unknown08[8];
	long marker_name;
};

long function_189060(long object_index, short value, real scale, point3f const *position, vector3f const *direction, long tag_index);

#define WEAPON_UNIT_GET(index) (((s_weapon_unit_header *)g_4e0300->data)[(index) & 0xffff].unit)

bool function_100880(long weapon_index, long magazine_index);
bool function_159dd0(long player_index);
bool function_159d40(void);

// @retail 0x100390
bool function_100390(long weapon_index, long barrel_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	bool result = false;

	if (definition->barrel_count > barrel_index)
	{
		s_weapon_barrel *barrel = &weapon->barrels[barrel_index];
		barrel->flag2 = true;
		result = true;
	}
	return result;
}

// @retail 0x1003e0
bool function_1003e0(long weapon_index, long barrel_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	bool result = false;

	if (definition->barrel_count > barrel_index)
	{
		s_weapon_barrel *barrel = &weapon->barrels[barrel_index];
		barrel->flag5 = true;
		result = true;
	}
	return result;
}

// @retail 0x100f00
bool function_100f00(long weapon_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	bool result = true;

	if (weapon->heat >= 1.0f)
	{
		result = false;
	}
	else if (definition->magazine_count > 0 && definition->magazines[0].rounds_loaded_maximum > 0
		&& weapon->magazines[0].rounds_loaded == 0 && weapon->magazines[0].rounds_unloaded == 0)
	{
		result = false;
	}
	return result;
}

// @retail 0x101010
short function_101010(long weapon_index, short field_240)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);

	if (!function_100880(weapon_index, NONE))
	{
		if (field_240 >= 0 && field_240 < definition->zoom_level_count - 1)
		{
			field_240++;
		}
		else
		{
			field_240 = field_240 == definition->zoom_level_count - 1 ? NONE : 0;
		}
	}
	return field_240;
}

// @retail 0x100f70
bool function_100f70(long weapon_index)
{
	return function_101010(weapon_index, NONE) != NONE;
}

// @retail 0x100fd0
bool function_100fd0(long weapon_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);

	return definition->zoom_level_count > 0;
}

// @retail 0x101090
real function_101090(long weapon_index, short field_240)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	real result = 1.0f;

	if (field_240 >= 0 && field_240 < definition->zoom_level_count)
	{
		real fraction;
		real minimum;
		real maximum;

		if (definition->zoom_level_count > 1)
			fraction = (real)field_240 / (real)(definition->zoom_level_count - 1);
		else
			fraction = 0.0f;
		if (definition->zoom_magnification_minimum > 0.0f)
			minimum = definition->zoom_magnification_minimum;
		else
			minimum = 1.0f;
		if (definition->zoom_magnification_maximum > 0.0f)
			maximum = definition->zoom_magnification_maximum;
		else
			maximum = 1.0f;
		result = (real)pow(maximum / minimum, fraction) * minimum;
	}
	return result;
}

// @retail 0x101160
bool function_101160(long weapon_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	bool result = false;

	for (long i = 0; i < definition->trigger_count; i++)
	{
		byte state = weapon->triggers[i].state;
		if (state == 3 || state == 7 || state == 5 || state == 6 || state == 2 && definition->value_294)
			result = true;
	}
	return result;
}

// @retail 0x1011d0
bool function_1011d0(long weapon_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	bool result = false;

	for (long i = 0; i < definition->trigger_count; i++)
	{
		if (weapon->triggers[i].state == 0 && definition->triggers[i].behavior == 5)
			result = true;
	}
	return result;
}

// @retail 0x101240
bool function_101240(long weapon_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);

	bool result = TEST_FIELD_BIT(definition->flag3);
	return result;
}

// @retail 0x1012c0
bool function_1012c0(long weapon_index)
{
	bool result = false;

	if (weapon_index != NONE)
	{
		s_weapon *weapon = WEAPON_GET(weapon_index);
		s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
		result = TEST_FIELD_BIT(definition->flag24);
	}
	return result;
}

// @retail 0x101300
bool function_101300(long weapon_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);

	bool result = TEST_FIELD_BIT(definition->flag28);
	return result;
}

// @retail 0x101340
bool function_101340(long weapon_index)
{
	bool result = false;

	if (weapon_index != NONE)
	{
		s_weapon *weapon = WEAPON_GET(weapon_index);
		s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
		result = TEST_FIELD_BIT(definition->flag18);
	}
	return result;
}

// @retail 0x101380
bool function_101380(long weapon_index)
{
	bool result = false;

	if (weapon_index != NONE)
	{
		s_weapon *weapon = WEAPON_GET(weapon_index);
		s_weapon_definition *definition = WEAPON_DEFINITION(weapon);

		result = TEST_FIELD_BIT(definition->flag20);
		if (g_4e6948->state == 2 && !function_159d40())
			result = false;
	}
	return result;
}

// @retail 0x1013e0
bool function_1013e0(long weapon_index)
{
	bool result = false;

	if (weapon_index != NONE)
	{
		s_weapon *weapon = WEAPON_GET(weapon_index);
		s_weapon_definition *definition = WEAPON_DEFINITION(weapon);

		result = TEST_FIELD_BIT(definition->flag21);
		if (g_4e6948->state == 2 && !function_159d40())
			result = false;
	}
	return result;
}

// @retail 0x101440
bool function_101440(long weapon_index)
{
	bool result = false;

	if (weapon_index != NONE)
	{
		s_weapon *weapon = WEAPON_GET(weapon_index);
		s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
		result = TEST_FIELD_BIT(definition->prevents_grenade_throwing);
		if (weapon->state >= 5 && weapon->state <= 10)
			result = true;
	}
	return result;
}

// @retail 0x101640
bool function_101640(long weapon_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);

	if (TEST_FIELD_BIT(definition->flag22) || TEST_FIELD_BIT(definition->flag23))
		return true;
	return false;
}

// @retail 0x101980
void function_101980(long weapon_index, short rounds_loaded, short magazine_index, short rounds_total)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	short maximum = definition->magazines[magazine_index].rounds_loaded_maximum;
	short loaded = rounds_loaded < 0 ? 0 : rounds_loaded;
	short unloaded = rounds_total - loaded < 0 ? 0 : rounds_total - loaded;

	weapon->magazines[magazine_index].rounds_loaded = loaded <= maximum ? loaded : maximum;
	weapon->magazines[magazine_index].rounds_unloaded = unloaded;
}

// @retail 0x101b10
long function_101b10(long barrel_index, long weapon_index)
{
	long result = 0;

	if (barrel_index >= 0 && barrel_index < 2)
	{
		s_weapon *weapon = WEAPON_GET(weapon_index);
		s_weapon_definition *definition = WEAPON_DEFINITION(weapon);

		result = barrel_index ? 0x110000dc : 0xf0000db;
		if (barrel_index < definition->barrel_count)
		{
			s_weapon_barrel_definition *barrel = &definition->barrels[barrel_index];
			if (barrel->name && barrel->name != NONE)
				result = barrel->name;
		}
	}
	return result;
}

// @retail 0x101d20
bool function_101d20(long weapon_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);

	bool result = false;

	if (!definition->magazine_count)
	{
		result = true;
	}
	else
	{
		for (short i = 0; i < definition->barrel_count; i++)
		{
			if (definition->barrels[i].value_a8 > 0.0f)
			{
				result = true;
				break;
			}
		}
	}
	return result;
}

// @retail 0x1029d0
bool function_1029d0(long weapon_index, short magazine_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);

	return weapon->magazines[magazine_index].state == 2;
}

// @retail 0x102e10
void function_102e10(long weapon_index, short trigger_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	s_weapon_trigger_definition *local_172694 = &definition->triggers[trigger_index];

	if (local_172694->charging_time > 0.0f)
	{
		real ticks_real = (real)g_510c54->field_2_3 * local_172694->charging_time;
		long ticks;
		__asm
		{
			fld ticks_real
			fistp ticks
		}
		weapon = WEAPON_GET(weapon_index);
		weapon->triggers[trigger_index].state = 4;
		weapon->triggers[trigger_index].timer = (short)ticks;
	}
	else
	{
		short barrel_index = local_172694->primary_barrel;
		if (barrel_index != NONE)
		{
			s_weapon_barrel *barrel = &weapon->barrels[barrel_index];
			barrel->flag0 = true;
		}
		weapon = WEAPON_GET(weapon_index);
		weapon->triggers[trigger_index].state = 0;
		weapon->triggers[trigger_index].timer = 0;
		s_weapon_trigger *trigger = &WEAPON_GET(weapon_index)->triggers[trigger_index];
		trigger->flag3 = false;
		trigger->flag4 = false;
	}
}

// @retail 0x102f70
void function_102f70(long weapon_index, short barrel_index)
{
	if (barrel_index >= 0 && barrel_index < 2)
	{
		s_weapon_barrel *barrel = &WEAPON_GET(weapon_index)->barrels[barrel_index];
		if (barrel->state != 1)
			barrel->flag6 = false;
	}
}

// @retail 0x103a90
bool function_103a90(long weapon_index, short trigger_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	s_weapon_trigger_definition *trigger = &definition->triggers[trigger_index];
	short barrel_index = trigger->primary_barrel;
	bool result = true;

	if (barrel_index != NONE && barrel_index < definition->barrel_count && weapon->barrels[barrel_index].state)
		result = false;
	if (weapon->heat >= 1.0f)
		result = false;
	return result;
}

void function_b7360(long object_index);

// @retail 0x101740
void function_101740(long weapon_index, long other_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon *other = WEAPON_GET(other_index);

	function_b7360(weapon_index);
	function_b7360(other_index);
	if (weapon->definition_index == other->definition_index)
	{
		s_weapon_definition *definition = WEAPON_DEFINITION(weapon);

		for (long i = 0; i < definition->magazine_count; i++)
		{
			s_weapon_magazine_definition *magazine_definition = &definition->magazines[i];
			s_weapon_magazine *magazine = &weapon->magazines[i];
			s_weapon_magazine *other_magazine = &other->magazines[i];

			if (magazine->rounds_unloaded < magazine_definition->rounds_total_maximum && other_magazine->rounds_unloaded > 0)
			{
				long space = magazine_definition->rounds_total_maximum - magazine->rounds_unloaded;
				short count = (short)(other_magazine->rounds_unloaded <= space ? other_magazine->rounds_unloaded : space);
				magazine->rounds_unloaded += count;
				other_magazine->rounds_unloaded -= count;
			}
			if (magazine->rounds_unloaded < magazine_definition->rounds_total_maximum && other_magazine->rounds_loaded > 0)
			{
				long space = magazine_definition->rounds_total_maximum - magazine->rounds_unloaded;
				short count = (short)(other_magazine->rounds_loaded <= space ? other_magazine->rounds_loaded : space);
				magazine->rounds_unloaded += count;
				other_magazine->rounds_loaded -= count;
			}
		}
		if (weapon->heat > other->heat)
		{
			real heat = weapon->heat;
			weapon->heat = other->heat;
			other->heat = heat;
		}
	}
}

// @retail 0x102a00
void function_102a00(long weapon_index, long trigger_index, bool flag)
{
	if (weapon_index != NONE)
	{
		s_weapon *weapon = WEAPON_GET(weapon_index);

		if (trigger_index >= 0 && trigger_index < WEAPON_DEFINITION(weapon)->trigger_count)
		{
			s_weapon_trigger *trigger = &weapon->triggers[trigger_index];

			if (flag)
				trigger->flag7 = true;
			else
				trigger->flag7 = false;
			function_b7360(weapon_index);
		}
	}
}

// @retail 0x103dd0
void function_103dd0(long weapon_index, short barrel_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	s_weapon_barrel_definition *barrel_definition = &definition->barrels[barrel_index];
	s_weapon_barrel *barrel = &weapon->barrels[barrel_index];

	barrel->timer = 0;
	barrel->value06 = 0;
	barrel->state = 0;
	if (!TEST_FIELD_BIT(barrel_definition->flag11))
	{
		barrel = &WEAPON_GET(weapon_index)->barrels[barrel_index];
		barrel->flag0 = false;
	}
}

// @retail 0x105740
void function_105740(long weapon_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);

	switch (weapon->state)
	{
	case 3:
	{
		s_weapon_barrel_definition *barrel_definition = &WEAPON_DEFINITION(weapon)->barrels[0];
		if (barrel_definition->value_9c > 0.0f && TEST_FIELD_BIT(barrel_definition->flag4))
			weapon->barrels[0].value14 = 1.0f;
		break;
	}
	case 4:
	{
		s_weapon_barrel_definition *barrel_definition = &WEAPON_DEFINITION(weapon)->barrels[1];
		if (barrel_definition->value_9c > 0.0f && TEST_FIELD_BIT(barrel_definition->flag4))
			weapon->barrels[1].value14 = 1.0f;
		break;
	}
	}
}

// @retail 0x105ba0
long function_105ba0(short type, bool flag)
{
	long result = NONE;

	switch (type)
	{
	case 1:
		result = flag ? 7 : 8;
		break;
	case 2:
		result = 9;
		break;
	case 3:
		result = flag ? 10 : 11;
		break;
	case 4:
		break;
	default:
		__assume(0);
	}
	return result;
}

// @retail 0x105be0
void function_105be0(long weapon_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);

	weapon->animation_254.graph_index = NONE;
	weapon->animation_254.index = NONE;
	weapon->value258 = 0.0f;
	weapon->value250 = NONE;
}
// @retail 0x106030
bool function_106030(long weapon_index)
{
	bool result = false;

	if (g_4e6948->state == 2)
	{
		s_weapon *weapon = WEAPON_GET(weapon_index);

		if (weapon->item_flags & 1)
		{
			long unit_index = weapon->unit_index;

			if (unit_index != NONE)
			{
				s_weapon_unit *unit = WEAPON_UNIT_GET(unit_index);

				if (unit->player_index != NONE && function_159dd0(unit->player_index))
					result = true;
			}
		}
	}
	return result;
}

static inline long weapon_magazine_rounds_unloaded(s_weapon *weapon, long magazine_index, bool infinite)
{
	long rounds = 0;

	if (infinite)
	{
		s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
		if (definition->magazine_count > magazine_index)
			rounds = definition->magazines[magazine_index].rounds_total_maximum;
	}
	else
	{
		rounds = weapon->magazines[magazine_index].rounds_unloaded;
	}
	return rounds;
}

// @retail 0x1008f0
long function_1008f0(long magazine_index, long weapon_index, bool weapon_only)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	bool infinite = function_106030(weapon_index);
	long rounds = weapon_magazine_rounds_unloaded(weapon, magazine_index, infinite);

	if (weapon->unit_index != NONE && !weapon_only)
	{
		s_weapon_unit *unit = WEAPON_UNIT_GET(weapon->unit_index);

		for (long i = 0; i < 4; i++)
		{
			long other_index = unit->weapon_indices[i];

			if (other_index != NONE && other_index != weapon_index)
			{
				s_weapon *other = WEAPON_GET(other_index);
				if (other->definition_index == weapon->definition_index)
					rounds += weapon_magazine_rounds_unloaded(other, magazine_index, infinite);
			}
		}
	}
	return rounds;
}

// @retail 0x100b40
long function_100b40(long magazine_index, long weapon_index, bool weapon_only)
{
	s_weapon_magazine *magazine = &WEAPON_GET(weapon_index)->magazines[magazine_index];

	return function_1008f0(magazine_index, weapon_index, weapon_only) + magazine->rounds_loaded;
}

// @retail 0x103e60
void function_103e60(long weapon_index, short barrel_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	s_weapon_barrel_definition *barrel_definition = &definition->barrels[barrel_index];
	s_weapon_barrel *barrel = &weapon->barrels[barrel_index];
	real total = (real)g_510c54->field_2_3 * barrel_definition->value_20;
	real partial = (1.0f - barrel_definition->value_24) * total;
	long ticks = (long)floor(partial);
	real fraction = (real)(total - floor(total));

	barrel->accumulator += fraction;
	if (barrel->accumulator >= 1.0f)
	{
		real whole = (real)floor(barrel->accumulator);
		barrel->accumulator -= whole;
		ticks += (long)whole;
	}
	barrel->ticks = (short)ticks;
	barrel->state = 2;
}

// @retail 0x103f60
void function_103f60(long weapon_index, short barrel_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	s_weapon_barrel_definition *barrel_definition = &definition->barrels[barrel_index];
	s_weapon_barrel *barrel = &weapon->barrels[barrel_index];
	long ticks = (long)floor(g_510c54->field_2_3 * barrel_definition->value_20);

	ticks -= (long)floor((1.0f - barrel_definition->value_24) * g_510c54->field_2_3 * barrel_definition->value_20);
	barrel->state = 3;
	barrel->ticks = (short)ticks;
	if (!ticks)
		function_103dd0(weapon_index, barrel_index);
}

// @retail 0x105dd0
void __stdcall function_105dd0(long object_index, long unused, s_weapon_sound_event *event)
{
	if (event->type == 1 && event->sound_index != NONE)
	{
		s_object_marker marker;

		if (event->marker_name == NONE || event->marker_name == 0x600008a || function_b8d30(object_index, event->marker_name, &marker, 1, false) < 1)
		{
			marker.node_index = 0;
			marker.node_matrix.position = *g_468788;
			marker.node_matrix.forward = *g_4687a8;
		}
		function_189060(object_index, marker.node_index, 1.0f, &marker.node_matrix.position, &marker.node_matrix.forward, event->sound_index);
	}
}

long function_101f20(long object_index);
bool function_1061c0(long object_index, long *out_index, byte *out_entry);

// @retail 0x100430
void function_100430(long weapon_index, long value, real amount)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);

	weapon->value_16e = (short)value;
	weapon->value_170 = (byte)(long)(amount * 255.0f);
	if (definition->trigger_count > 0 && definition->triggers[0].behavior == 5 && weapon->triggers[0].state != 6)
	{
		long unit_index = function_101f20(weapon_index);

		if (unit_index != NONE && WEAPON_UNIT_GET(unit_index)->player_index != NONE)
		{
			long object_index;
			byte entry;

			if (function_1061c0(unit_index, &object_index, &entry))
			{
				weapon->object_index_194 = object_index;
				weapon->entry_177 = entry;
				weapon->time_24c = g_510c54->game_time;
			}
		}
	}
	if (value & 0x1df)
		function_b7360(weapon_index);
}

/* a view of the player (the player data, 0x21c bytes) */
struct s_weapon_player
{
	byte unknown000[0x28];
	short value_28;
	byte unknown02a[2];
	long unit_index;
	byte unknown030[0x88 - 0x30];
	char character_index;
};

#define WEAPON_PLAYER_GET(index) ((s_weapon_player *)(g_4e8c24->data + ((index) & 0xffff) * 0x21c))

struct s_model_definition_view
{
	byte unknown00[0x14];
	long animation_graph_index;
};


long function_101ec0(long object_index);

// @retail 0x101b80
bool function_101b80(long weapon_index, short barrel_index, point3f *point)
{
	s_object_marker markers[64];
	bool result = false;
	long object_index = function_101ec0(weapon_index);
	long marker_name = function_101b10(barrel_index, weapon_index);
	short count = function_b8d30(object_index, marker_name, markers, 64, false);

	if (count > 0)
	{
		*point = *g_468788;
		for (short i = 0; i < count; i++)
		{
			point3f *position = &markers[i].matrix.position;
			point->x = position->x + point->x;
			point->y = position->y + point->y;
			point->z = position->z + point->z;
		}

		real scale = 1.0f / (real)count;
		point->x *= scale;
		point->y *= scale;
		point->z *= scale;
		result = true;
	}
	return result;
}

// @retail 0x1060a0
void function_1060a0(long weapon_index, long unit_index)
{
	s_weapon_definition *definition = WEAPON_DEFINITION(WEAPON_GET(weapon_index));

	if (definition->model_index != NONE)
	{
		long graph_index = ((s_model_definition_view *)g_4e3b44[definition->model_index & 0xffff].bytes)->animation_graph_index;
		if (graph_index != NONE)
			function_1ddaf0((s_graph_tag *)g_4e3b44[graph_index & 0xffff].bytes);
	}
	if (unit_index != NONE)
	{
		s_weapon_unit *unit = WEAPON_UNIT_GET(unit_index);

		if (unit->animation_state_offset != NONE)
		{
			s_animation_state *state = (s_animation_state *)((byte *)unit + unit->animation_state_offset);

			state->resources_request(0x7000101, definition->animation_weapon_class, definition->animation_weapon_type, true, false);
			if (unit->player_index != NONE)
			{
				s_weapon_player *player = WEAPON_PLAYER_GET(unit->player_index);

				if (player->value_28 != NONE)
				{
					long character_index = player->character_index;

					if (character_index >= 0 && character_index < definition->player_animation_count)
					{
						long graph_index = definition->player_animations[character_index].graph_index;
						if (graph_index != NONE)
							function_1ddaf0((s_graph_tag *)g_4e3b44[graph_index & 0xffff].bytes);
					}
				}
			}
		}
	}
}

bool function_17b160(long effect_index, long tag_index);

/* the group tag of a tag instance */
struct s_tag_group_view
{
	dword group_tag;
};

// @retail 0x1039a0
long function_1039a0(long object_index, long tag_index, long effect_index, real scale_a, real scale_b)
{
	long result = NONE;

	if (tag_index != NONE)
	{
		long owner_index = function_101ec0(object_index);
		s_weapon *weapon = WEAPON_GET(object_index);
		long unit_index = NONE;

		if (TEST_FIELD_BIT(weapon->in_inventory))
			unit_index = weapon->unit_index;
		switch (((s_tag_group_view *)&g_4e3b44[(short)tag_index])->group_tag)
		{
		case 'effe':
			result = effect_index;
			if (!function_17b160(effect_index, tag_index))
				result = function_176780(owner_index, NULL, scale_a, tag_index, scale_b, NULL, NULL);
			break;
		case 'snd!':
			function_189060(unit_index, NONE, scale_a, g_468788, g_4687a8, tag_index);
			break;
		}
	}
	return result;
}

long function_1766b0(long object_index, long tag_index, long unknown34, long unknown38, short unknown3c);

// @retail 0x103a60
long function_103a60(long object_index, long tag_index)
{
	long result = NONE;

	if (tag_index != NONE)
	{
		long owner_index = function_101ec0(object_index);
		if (owner_index != NONE)
			result = function_1766b0(owner_index, tag_index, 0, 0, 0);
	}
	return result;
}

real function_fa7b0(long definition_index, real distance);

// @retail 0x100ea0
real weapon_barrel_estimate_time_to_target(long weapon_index, short barrel_index, real distance)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	real result = 0.0f;

	if (barrel_index >= 0 && barrel_index < definition->barrel_count)
	{
		s_weapon_barrel_definition *barrel = &definition->barrels[barrel_index];
		if (barrel->projectile_definition_index != NONE)
			result = function_fa7b0(barrel->projectile_definition_index, distance);
	}
	return result;
}

long function_166244(long key);
void function_ee680(long user_index, long state, bool secondary, long weapon_index);

static inline long user_get_unit_index(long user_index)
{
	long player_index = g_4e8c20->entries[user_index];
	bool valid = player_index != NONE;

	if (valid)
		return WEAPON_PLAYER_GET(player_index)->unit_index;
	return NONE;
}

// @retail 0x105fa0
void function_105fa0(long weapon_index, long state)
{
	long user_index = function_166244(weapon_index);
	long unit_index = user_index != NONE ? user_get_unit_index(user_index) : NONE;
	bool secondary = false;

	if (unit_index != NONE)
	{
		s_weapon_unit *unit = WEAPON_UNIT_GET(unit_index);
		short slot = unit->current_weapon_slot;
		long current_weapon_index = slot != NONE ? unit->weapon_indices[slot] : NONE;
		secondary = current_weapon_index != weapon_index;
	}
	function_ee680(user_index, state, secondary, weapon_index);
}

bool function_cd660(long unit_index);
long function_cbd50(long unit_index, short weapon_index);

// @retail 0x101690
bool function_101690(long weapon_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	bool result = false;

	if (weapon->parent_index != NONE && ((1 << WEAPON_UNIT_GET(weapon->parent_index)->object_type) & 3))
	{
		if (function_cd660(weapon->parent_index) &&
			(weapon_index == function_cbd50(weapon->parent_index, WEAPON_UNIT_GET(weapon->parent_index)->current_weapon_slot) ||
			weapon_index == function_cbd50(weapon->parent_index, WEAPON_UNIT_GET(weapon->parent_index)->other_weapon_slot)))
			result = true;
		else
			result = false;
	}
	return result;
}

bool function_fa580(real speed, point3f const *origin, point3f const *target, vector3f *direction,
	real *distance, real *speed_out, real *time);
bool function_fa6a0(long definition_index, real const *speed_override, point3f const *origin, point3f const *target,
	real *unknown2, real const *unknown3, real const *unknown4, bool unknown5, vector3f *direction, real *speed_out,
	real *time, real *distance, bool *linear);

// @retail 0x100dd0
bool weapon_barrel_aim(long weapon_index, short barrel_index, point3f const *origin, point3f const *target,
	real const *unknown3, bool unknown5, vector3f *direction, real *speed_out, real *time, real *distance, bool *linear)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	bool result = false;

	if (barrel_index >= 0 && barrel_index < definition->barrel_count)
	{
		s_weapon_barrel_definition *barrel = &definition->barrels[barrel_index];

		if (barrel->projectile_definition_index != NONE)
		{
			function_fa6a0(barrel->projectile_definition_index, NULL, origin, target, NULL, unknown3, NULL, unknown5,
				direction, speed_out, time, distance, linear);
			result = true;
		}
		else
		{
			result = function_fa580(1.0f, origin, target, direction, distance, speed_out, time);
			if (linear)
				*linear = true;
		}
	}
	return result;
}

long first_person_weapon_state_animation(short state);
short first_person_weapon_animation_ticks(long weapon_index, long animation_name, short type);

// @retail 0x105a80
void function_105a80(short state, long weapon_index, short magazine_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_magazine *magazine = &weapon->magazines[magazine_index];
	s_weapon_magazine_definition *magazine_definition = &WEAPON_DEFINITION(weapon)->magazines[magazine_index];
	bool empty = magazine->rounds_loaded == 0;
	long first_person_state = NONE;
	short secondary_ticks = NONE;
	short ticks = NONE;

	switch (state)
	{
	case 0:
		break;
	case 1:
	case 2:
		first_person_state = function_105ba0(state, empty);
		if (first_person_state != NONE)
		{
			long animation = first_person_weapon_state_animation((short)first_person_state);

			ticks = first_person_weapon_animation_ticks(weapon_index, animation, 0);
			if (state == 2)
				secondary_ticks = NONE;
			else
				secondary_ticks = first_person_weapon_animation_ticks(weapon_index, animation, 3);
		}
		break;
	case 3:
		break;
	case 4:
	{
		real time = (real)g_510c54->field_2_3 * magazine_definition->value_18;
		long rounded;

		__asm
		{
			fld time
			fistp rounded
		}
		ticks = (short)rounded;
		break;
	}
	default:
		__assume(0);
	}
	magazine->state = state;
	magazine->ticks = ticks;
	magazine->ticks_0c = secondary_ticks;
	magazine->ticks_0e = secondary_ticks;
	if (first_person_state != NONE)
		function_105fa0(weapon_index, first_person_state);
}

// @retail 0x101db0
void function_101db0(long weapon_index, real heat)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	bool overheat_disabled = false;

	if (TEST_FIELD_BIT(weapon->in_inventory) && weapon->unit_index != NONE)
	{
		long player_index = WEAPON_UNIT_GET(weapon->unit_index)->player_index;
		if (player_index != NONE)
			overheat_disabled = function_159dd0(player_index);
	}
	if (TEST_FIELD_BIT(weapon->item_flag3) && weapon->heat < 1.0f && !overheat_disabled)
	{
		weapon->heat = heat + weapon->heat;
		if (weapon->heat >= 1.0f)
		{
			weapon->heat = 1.0f;
			function_1039a0(weapon_index, definition->overheated_effect, NONE, 0.0f, 0.0f);
			if (definition->overheated_definition_index != NONE)
				weapon->definition_index = definition->overheated_definition_index;
		}
	}
}

// @retail 0x105840
void function_105840(long weapon_index)
{
	s_weapon_definition *definition = WEAPON_DEFINITION(WEAPON_GET(weapon_index));

	if (definition->barrel_count > 0)
	{
		s_weapon_barrel_definition *barrel = &definition->barrels[0];

		if (barrel->firing_effect_count > 0)
		{
			long effect_tag_index = barrel->firing_effects[0].effect_tag_index;

			if (effect_tag_index != NONE)
				function_1039a0(weapon_index, effect_tag_index, NONE, 1.0f, 0.0f);
		}
	}
}

void function_c86e0(long unit_index, bool keep_weapon_zoom);

/* the unit holding a weapon, if it is held */
static inline long weapon_get_owner_unit_index(long weapon_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	long result = NONE;

	if (TEST_FIELD_BIT(weapon->in_inventory))
		result = weapon->unit_index;
	return result;
}

// @retail 0x103b10
void function_103b10(long weapon_index, short trigger_index)
{
	long unit_index = weapon_get_owner_unit_index(weapon_index);
	s_weapon *weapon = WEAPON_GET(weapon_index);

	weapon->triggers[trigger_index].state = 3;
	weapon->triggers[trigger_index].timer = 0;
	if (unit_index != NONE)
		function_c86e0(unit_index, true);
}

// @retail 0x103b70
void function_103b70(long weapon_index, short trigger_index)
{
	long unit_index = weapon_get_owner_unit_index(weapon_index);
	s_weapon *weapon = WEAPON_GET(weapon_index);

	weapon->triggers[trigger_index].state = 7;
	weapon->triggers[trigger_index].timer = 0;
	if (unit_index != NONE)
		function_c86e0(unit_index, true);
}

// @retail 0x103bd0
void function_103bd0(long weapon_index, short trigger_index)
{
	long unit_index = weapon_get_owner_unit_index(weapon_index);
	long ticks = g_510c54->field_2_3;
	s_weapon *weapon = WEAPON_GET(weapon_index);

	weapon->triggers[trigger_index].timer = (short)ticks;
	weapon->triggers[trigger_index].state = 6;
	if (unit_index != NONE)
		function_c86e0(unit_index, true);
}

// @retail 0x103c40
void function_103c40(long weapon_index, short trigger_index)
{
	long unit_index = weapon_get_owner_unit_index(weapon_index);
	real time = (real)g_510c54->field_2_3 * 0.5f;
	long ticks;
	s_weapon *weapon;

	__asm
	{
		fld time
		fistp ticks
	}
	weapon = WEAPON_GET(weapon_index);
	weapon->triggers[trigger_index].state = 5;
	weapon->triggers[trigger_index].timer = (short)ticks;
	if (unit_index != NONE)
		function_c86e0(unit_index, true);
}

// @retail 0x103ce0
void function_103ce0(long weapon_index, short trigger_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_trigger_definition *local_172694 = &WEAPON_DEFINITION(weapon)->triggers[trigger_index];
	s_weapon_trigger *trigger = &weapon->triggers[trigger_index];
	long unit_index = weapon_get_owner_unit_index(weapon_index);
	real time;
	long ticks;

	trigger->effect_index = function_1039a0(weapon_index, local_172694->effect_tag_index, NONE, 0.0f, 0.0f);
	time = (real)g_510c54->field_2_3 * local_172694->value_1c;
	__asm
	{
		fld time
		fistp ticks
	}
	weapon = WEAPON_GET(weapon_index);
	weapon->triggers[trigger_index].state = 1;
	weapon->triggers[trigger_index].timer = (short)ticks;
	if (unit_index != NONE)
		function_c86e0(unit_index, true);
}

// @retail 0x102c60
void function_102c60(long weapon_index, short magazine_index, bool interrupted)
{
	s_weapon_magazine *magazine = &WEAPON_GET(weapon_index)->magazines[magazine_index];

	if (interrupted || magazine->state != 4)
		function_105fa0(weapon_index, !interrupted && magazine->state == 3 ? 12 : 0);
	function_105a80(5, weapon_index, magazine_index);
}

// @retail 0x104080
void __stdcall function_104080(long weapon_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	short index;

	for (index = 0; index < definition->trigger_count; index++)
	{
		s_weapon_trigger *trigger = &weapon->triggers[index];

		trigger->state = 0;
		trigger->timer = 0;
	}
	for (index = 0; index < definition->magazine_count; index++)
		function_105a80(0, weapon_index, index);
	function_105be0(weapon_index);
	function_b7360(weapon_index);
}

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);

/* a unit's definition: its campaign metagame type at +0xc0 */
struct s_weapon_unit_definition
{
	byte unknown000[0xc0];
	short metagame_type;
};

// @retail 0x101c80
void function_101c80(long weapon_index, long unit_index)
{
	s_weapon_definition *definition = WEAPON_DEFINITION(WEAPON_GET(weapon_index));

	if (TEST_FIELD_BIT(definition->flag26) && g_4e6948->state == 1)
	{
		long *unit = (long *)function_badc0(unit_index, 3);

		if (unit)
		{
			real scale = 1.0f;
			long type = ((s_weapon_unit_definition *)g_4e3b44[*unit & 0xffff].bytes)->metagame_type;

			if (type >= 4 && type <= 5)
				scale = 0.25f;
			function_101db0(weapon_index, scale * 0.1f);
		}
	}
}

void __stdcall function_c9d00(long unit_index, long weapon_index, long state);

/* whether a weapon in a state may change to another: anything may replace
   state 0, states 1 and 2 only by the same or a later state */
static inline bool weapon_state_replaceable(long current, long state)
{
	bool result = false;

	switch (current)
	{
	case 0:
		result = true;
		break;
	case 1:
	case 2:
		result = state >= current;
		break;
	}
	return result;
}

/* puts a weapon in a state and plays the state's animation */
// @retail 0x1058b0
bool function_1058b0(long weapon_index, long state, bool force)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	bool result = false;

	function_b7360(weapon_index);
	if (force || weapon_state_replaceable(weapon->state, state))
	{
		if (WEAPON_GET(weapon_index)->animation_state_offset != NONE)
		{
			s_animation_state *animation = (s_animation_state *)((byte *)weapon + weapon->animation_state_offset);
			long set;

			switch (state)
			{
			case 0:
				set = 0x400000c;
				break;
			case 1:
				set = 0x6000006;
				break;
			case 2:
				set = 0x6000007;
				break;
			case 3:
				set = 0x9000004;
				break;
			case 4:
				set = 0x9000005;
				break;
			case 5:
				set = 0x8000002;
				break;
			case 6:
				set = 0x8000002;
				break;
			case 7:
				set = 0x9000009;
				break;
			case 8:
				set = 0x9000009;
				break;
			case 9:
				set = 0x5000024;
				break;
			case 10:
				set = 0x8000025;
				break;
			default:
				__assume(0);
			}

			long state_flags = 0x82;

			if (set == 0x400000c)
				state_flags = 0x96;
			long weapon_class = 0x7000001;
			long unit_index = function_101f20(weapon_index);

			if (unit_index != NONE && function_cd660(unit_index))
				weapon_class = 0x400054b;
			if (animation->animation_set(0x7000101, weapon_class, 0x7000001, set, state_flags, 0x3f))
			{
				weapon->state = state;
			}
			else if (weapon_class == 0x400054b &&
				animation->animation_set(0x7000101, 0x7000001, 0x7000001, set, state_flags, 0x3f))
			{
				weapon->state = state;
			}
		}
		weapon = WEAPON_GET(weapon_index);
		result = true;
		if (TEST_FIELD_BIT(weapon->in_inventory) && weapon->unit_index != NONE)
			function_c9d00(weapon->unit_index, weapon_index, state);
	}
	return result;
}

/* puts a weapon in state 10 back to state 0 */
// @retail 0x100350
bool function_100350(long weapon_index)
{
	bool result = false;

	if (WEAPON_GET(weapon_index)->state == 10)
	{
		function_1058b0(weapon_index, 0, true);
		result = true;
	}
	return result;
}
// @retail 0x105800
void function_105800(long weapon_index)
{
	switch (WEAPON_GET(weapon_index)->state)
	{
	case 7:
	case 8:
	case 10:
		break;
	default:
		function_1058b0(weapon_index, 0, true);
	}
}

long function_101e80(long object_index);

/* starts reloading a magazine of a weapon, if it can */
// @retail 0x102a80
bool function_102a80(long weapon_index, short magazine_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_magazine *magazine = &weapon->magazines[magazine_index];
	s_weapon_magazine_definition *magazine_definition = &WEAPON_DEFINITION(weapon)->magazines[magazine_index];
	bool result = false;
	short state;

	if (WEAPON_DEFINITION(weapon)->reload_style == 1)
	{
		if (magazine->state == 3 || magazine->state == 4)
			state = 3;
		else
			state = 2;
	}
	else
	{
		state = 1;
	}
	switch (magazine->state)
	{
	case 0:
	case 5:
		break;
	case 4:
		if (state == 3)
			break;
	default:
		return result;
	}
	if (function_101e80(weapon_index) && function_1008f0(magazine_index, weapon_index, false) > 0 &&
		magazine->rounds_loaded < magazine_definition->rounds_loaded_maximum)
	{
		function_1058b0(weapon_index, magazine_index + 5, false);
		function_105a80(state, weapon_index, magazine_index);
		function_1039a0(weapon_index, magazine_definition->reloading_effect, NONE, 0.0f, 0.0f);
		return true;
	}
	return false;
}

static inline s_weapon_barrel *weapon_barrel_get(long weapon_index, short barrel_index)
{
	s_weapon_barrel *result = NULL;

	if (barrel_index >= 0 && barrel_index < 2)
		result = &WEAPON_GET(weapon_index)->barrels[barrel_index];
	return result;
}

/* reloads a magazine of a weapon: resets its triggers and barrels */
// @retail 0x101490
bool function_101490(long weapon_index, long magazine_index)
{
	s_weapon_definition *definition = WEAPON_DEFINITION(WEAPON_GET(weapon_index));
	bool result = false;

	if (magazine_index >= 0 && magazine_index < definition->magazine_count &&
		function_102a80(weapon_index, (short)magazine_index))
	{
		long i;

		function_b7360(weapon_index);
		for (i = 0; i < definition->trigger_count; i++)
		{
			s_weapon_trigger *trigger = &WEAPON_GET(weapon_index)->triggers[(short)i];

			trigger->state = 0;
			trigger->timer = 0;
		}
		for (i = 0; i < definition->barrel_count; i++)
		{
			s_weapon_barrel *barrel = weapon_barrel_get(weapon_index, (short)i);

			if (barrel && barrel->state != 1)
				barrel->flag6 = false;
			if (definition->reload_style != 1)
				function_103dd0(weapon_index, (short)i);
		}
		result = true;
	}
	return result;
}
/* finds a weapon's animation (its overlay, else the animation) in the graph
   for its holder's character, and makes it the weapon's current one */
// @retail 0x105c20
bool __stdcall function_105c20(long weapon_index, long animation_name)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	bool result = false;

	weapon->animation_254.graph_index = NONE;
	weapon->animation_254.index = NONE;
	weapon->value258 = 0.0f;
	weapon->value250 = NONE;
	if (weapon->unit_index != NONE)
	{
		long player_index = WEAPON_UNIT_GET(weapon->unit_index)->player_index;

		if (player_index != NONE)
		{
			long character_index = WEAPON_PLAYER_GET(player_index)->character_index;

			if (character_index >= 0 && character_index < definition->player_animation_count)
			{
				s_weapon_player_animation *player_animation = &definition->player_animations[character_index];

				if (player_animation->graph_index != NONE)
				{
					s_animation_state state;

					if (state.initialize(player_animation->graph_index, NONE, true))
					{
						c_type_709360 animation_id = state.overlay_find(animation_name, state.unknown74, state.unknown78);

						if (animation_id.index != NONE ||
							(animation_id = state.animation_get(animation_name, state.unknown74, state.unknown78)).index != NONE)
						{
							weapon->animation_254 = animation_id;
							weapon->value250 = player_animation->graph_index;
							weapon->value258 = 0.0f;
							function_b7360(weapon_index);
							result = true;
						}
					}
					state.channels_clear_partial();
				}
			}
		}
	}
	return result;
}
/* the unit holding a weapon in its inventory, and whether that unit holds
   two weapons (0x101f20 and 0xcd660, inlined) */
static inline long weapon_inventory_unit_get(long weapon_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	long result = NONE;

	if (TEST_FIELD_BIT(weapon->in_inventory) && weapon->unit_index != NONE)
		result = weapon->unit_index;
	return result;
}

static inline bool unit_dual_wielding(long unit_index)
{
	s_weapon_unit *unit = WEAPON_UNIT_GET(unit_index);

	return unit->current_weapon_slot != NONE && unit->other_weapon_slot != NONE;
}

/* puts a weapon away: resets its triggers and barrels, and plays its
   put-away animation (state 10) */
// @retail 0x100130
bool __stdcall function_100130(long weapon_index, bool immediate)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	long i;

	for (i = 0; i < definition->trigger_count; i++)
	{
		s_weapon_trigger *trigger = &WEAPON_GET(weapon_index)->triggers[(short)i];

		trigger->state = 0;
		trigger->timer = 0;
	}
	for (i = 0; i < definition->barrel_count; i++)
		function_103e60(weapon_index, (short)i);

	s_weapon *current = WEAPON_GET(weapon_index);

	function_b7360(weapon_index);
	if (!immediate && !weapon_state_replaceable(current->state, 10))
		return false;
	if (WEAPON_GET(weapon_index)->animation_state_offset != NONE)
	{
		s_animation_state *animation = (s_animation_state *)((byte *)current + current->animation_state_offset);
		long weapon_class = 0x7000001;
		long unit_index = weapon_inventory_unit_get(weapon_index);

		if (unit_index != NONE && unit_dual_wielding(unit_index))
			weapon_class = 0x400054b;
		if (animation->animation_set(0x7000101, weapon_class, 0x7000001, 0x8000025, 0x82, 0x3f))
		{
			current->state = 10;
		}
		else if (weapon_class == 0x400054b &&
			animation->animation_set(0x7000101, 0x7000001, 0x7000001, 0x8000025, 0x82, 0x3f))
		{
			current->state = 10;
		}
	}

	s_weapon *put_away = WEAPON_GET(weapon_index);

	if (TEST_FIELD_BIT(put_away->in_inventory) && put_away->unit_index != NONE)
	{
		s_weapon_unit *unit = WEAPON_UNIT_GET(put_away->unit_index);
		volatile bool is_current = weapon_index ==
			(unit->current_weapon_slot != NONE ? unit->weapon_indices[unit->current_weapon_slot] : NONE);
	}
	weapon->state_ticks = first_person_weapon_animation_ticks(weapon_index, 0x8000025, 1);
	return true;
}
/* takes rounds from a weapon's reserve for a magazine: first its own, then
   the reserves of the holder's other weapons of the same kind */
// @retail 0x100b80
bool function_100b80(long magazine_index, long weapon_index, long count)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	long available = function_1008f0(magazine_index, weapon_index, false);
	bool result = false;

	function_106030(weapon_index);
	if (available >= count)
	{
		long taken = 0;

		if (magazine_index >= 0 && magazine_index < definition->magazine_count)
		{
			s_weapon_magazine *magazine = &weapon->magazines[magazine_index];
			short unloaded = magazine->rounds_unloaded;

			taken = count <= unloaded ? count : unloaded;
			magazine->rounds_unloaded = unloaded - (short)taken;
			if (taken > 0)
				function_b7360(weapon_index);
		}
		if (weapon->unit_index != NONE && taken < count)
		{
			s_weapon_unit *unit = WEAPON_UNIT_GET(weapon->unit_index);

			for (long i = 0; i < 4; i++)
			{
				long other_index = unit->weapon_indices[i];

				if (other_index != NONE && other_index != weapon_index &&
					WEAPON_GET(other_index)->definition_index == weapon->definition_index)
				{
					s_weapon_magazine *magazine = &WEAPON_GET(other_index)->magazines[magazine_index];
					short unloaded = magazine->rounds_unloaded;
					long take = count - taken > unloaded ? unloaded : count - taken;

					if (take > 0)
					{
						magazine->rounds_unloaded = unloaded - (short)take;
						taken += take;
					}
				}
			}
		}
		result = true;
	}
	return result;
}

void function_a7cd0(long weapon_index);

/* finishes reloading a magazine: moves rounds from the reserve into it */
// @retail 0x102b90
void function_102b90(long weapon_index, short magazine_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_magazine *magazine = &weapon->magazines[magazine_index];
	s_weapon_magazine_definition *magazine_definition = &WEAPON_DEFINITION(weapon)->magazines[magazine_index];

	if (magazine_definition->flags & 1)
		magazine->rounds_loaded = 0;

	long available = function_1008f0(magazine_index, weapon_index, false);
	long reloaded = magazine_definition->rounds_reloaded > available ? available : magazine_definition->rounds_reloaded;
	word loaded = magazine->rounds_loaded;
	short rounds = (short)(loaded + reloaded);

	if (rounds > magazine_definition->rounds_loaded_maximum)
		rounds = magazine_definition->rounds_loaded_maximum;
	if (TEST_FIELD_BIT(weapon->item_flag3))
		function_100b80(magazine_index, weapon_index, rounds - (short)loaded);
	magazine->rounds_loaded = rounds;
	magazine->ticks_0c = NONE;
	magazine->ticks_0e = NONE;
	function_a7cd0(weapon_index);
}

void __stdcall function_104080(long weapon_index);

/* finishes the reloads of a weapon's magazines that are past half way */
// @retail 0x1015a0
void function_1015a0(long weapon_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);

	for (short i = 0; i < definition->magazine_count; i++)
	{
		s_weapon_magazine *magazine = &weapon->magazines[i];

		if (magazine->state == 1 && magazine->ticks_0c * 2 < magazine->ticks_0e)
		{
			function_102b90(weapon_index, i);
			function_105fa0(weapon_index, 0);
			function_105a80(5, weapon_index, i);
		}
	}
	function_104080(weapon_index);
}
