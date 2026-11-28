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
#include "unit_requests.h"
#include "object_tag_definitions.h"

#define MIN(a, b) ((a) > (b) ? (b) : (a))

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
	byte unknown3c[0x48 - 0x3c];
	long effect_48;
	byte unknown4c[0x5c - 0x4c];
};

struct s_weapon_trigger_definition
{
	byte unknown00[6];
	short behavior;
	byte unknown08[2];
	short primary_barrel;
	byte unknown0c[4];
	real value_10;
	real value_14;
	byte unknown18[4];
	real value_1c;
	real value_20;
	byte unknown24[0x2c - 0x24];
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
	short magazine_index;
	short rounds_minimum;
	short rounds_per_shot;
	byte unknown2e[2];
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
	short rounds_fraction;
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
	byte value_171;
	byte value_172;
	char value_173;
	char value_174;
	byte unknown175[2];
	char entry_177;
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

long function_189060(long object_index, short value, point3f const *position, vector3f const *direction, real scale, long tag_index);

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
	s_weapon_magazine_definition *magazine_definition = &definition->magazines[magazine_index];
	short loaded = rounds_loaded < 0 ? 0 : rounds_loaded;
	short unloaded = rounds_total - loaded < 0 ? 0 : rounds_total - loaded;

	weapon->magazines[magazine_index].rounds_loaded = loaded > magazine_definition->rounds_loaded_maximum ? magazine_definition->rounds_loaded_maximum : loaded;
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
	bool result = false;
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);

	if (!definition->magazine_count)
	{
		result = true;
	}
	else
	{
		short i = 0;
		long count = definition->barrel_count;
		if (i < count)
		{
			s_weapon_barrel_definition *barrels = definition->barrels;
			do
			{
				if (barrels[i].value_a8 > 0.0f)
				{
					result = true;
					break;
				}
			}
			while (++i < count);
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

static inline s_weapon *weapon_get_102e10(long weapon_index)
{
	long index = weapon_index & 0xffff;
	s_weapon_header *headers = (s_weapon_header *)g_4e0300->data;
	return headers[index].weapon;
}

// @retail 0x102e10
void function_102e10(long weapon_index, short trigger_index)
{
	s_weapon *weapon = weapon_get_102e10(weapon_index);
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
		weapon = weapon_get_102e10(weapon_index);
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
		weapon = weapon_get_102e10(weapon_index);
		weapon->triggers[trigger_index].state = 0;
		weapon->triggers[trigger_index].timer = 0;
		s_weapon_trigger *trigger = &weapon_get_102e10(weapon_index)->triggers[trigger_index];
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
	bool result = true;

	if (trigger->primary_barrel != NONE)
	{
		short barrel_index = trigger->primary_barrel;

		if (barrel_index < definition->barrel_count && weapon->barrels[barrel_index].state)
			result = false;
	}
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

// Clears the weapon's animation; the pair is cleared through a reference, the only form with which 0xfdad0 (which inlines this) matches.
// @retail 0x105be0
void function_105be0(long weapon_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	c_type_709360 &animation = weapon->animation_254;

	animation.graph_index = NONE;
	animation.index = NONE;
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

static __forceinline void weapon_magazine_rounds_unloaded(s_weapon *weapon, long magazine_index, bool infinite, long &rounds)
{
	if (infinite)
	{
		s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
		if (definition->magazine_count > magazine_index)
			rounds += definition->magazines[magazine_index].rounds_total_maximum;
	}
	else
	{
		rounds += weapon->magazines[magazine_index].rounds_unloaded;
	}
}

// @retail 0x1008f0
long function_1008f0(long magazine_index, long weapon_index, bool weapon_only)
{
	long rounds = 0;
	s_weapon *weapon = WEAPON_GET(weapon_index);
	long magazine_offset = magazine_index * sizeof(s_weapon_magazine);
	bool infinite = function_106030(weapon_index);
	{
		if (infinite)
		{
			s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
			if (definition->magazine_count > magazine_index)
				rounds += definition->magazines[magazine_index].rounds_total_maximum;
		}
		else
		{
			s_weapon_magazine const *magazine = (s_weapon_magazine const *)((byte const *)weapon->magazines + magazine_offset);
			rounds += magazine->rounds_unloaded;
		}
	}

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
				{
					weapon_magazine_rounds_unloaded(other, magazine_index, infinite, rounds);
				}
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
	long total_ticks = (long)floor(g_510c54->field_2_3 * barrel_definition->value_20);
	long skipped_ticks = (long)floor((1.0f - barrel_definition->value_24) * g_510c54->field_2_3 * barrel_definition->value_20);
	long ticks = total_ticks - skipped_ticks;

	barrel->state = 3;
	barrel->ticks = (short)ticks;
	if (!ticks)
		function_103dd0(weapon_index, barrel_index);
}

// @retail 0x105dd0
void __stdcall function_105dd0(long object_index, real frame, s_weapon_sound_event *event)
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
		function_189060(object_index, marker.node_index, &marker.node_matrix.position, &marker.node_matrix.forward, 1.0f, event->sound_index);
	}
}

long function_101f20(long object_index);
bool function_1061c0(long object_index, long *out_index, byte *out_entry);

/* the weapon's control flags from its holder: a word, passed by value */
struct s_weapon_control_flags
{
	word bits;
};

// @retail 0x100430
void function_100430(long weapon_index, s_weapon_control_flags value, real amount)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);

	weapon->value_16e = (short)value.bits;
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
	if (value.bits & 0x1df)
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

		if (WEAPON_UNIT_GET(unit_index)->animation_state_offset != NONE)
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
						s_weapon_player_animation *player_animation = &definition->player_animations[character_index];

						if (player_animation->graph_index != NONE)
							function_1ddaf0((s_graph_tag *)g_4e3b44[player_animation->graph_index & 0xffff].bytes);
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
				result = function_176780(tag_index, (s_effect_owner const *)NULL, owner_index, scale_a, scale_b, NULL, NULL);
			break;
		case 'snd!':
			function_189060(unit_index, NONE, g_468788, g_4687a8, scale_a, tag_index);
			break;
		}
	}
	return result;
}

long function_1766b0(long object_index, long tag_index, long unknown34, long unknown38, short unknown3c);

__declspec(noinline) long function_103a60(long object_index, long tag_index);

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
void function_ee680(long user_index, long state, long secondary, long weapon_index);

static inline long user_get_unit_index(long user_index)
{
	bool valid = false;
	long player_index;

	if (user_index != NONE)
	{
		player_index = g_4e8c20->entries[user_index];
		valid = player_index != NONE;
	}
	if (valid)
		return WEAPON_PLAYER_GET(player_index)->unit_index;
	return NONE;
}

// @retail 0x105fa0
void function_105fa0(long weapon_index, long state)
{
	long user_index = function_166244(weapon_index);
	long unit_index = user_get_unit_index(user_index);
	long secondary = 0;

	if (unit_index != NONE)
	{
		s_weapon_unit *unit = WEAPON_UNIT_GET(unit_index);
		short slot = unit->current_weapon_slot;
		long current_weapon_index = NONE;

		if (slot != NONE)
			current_weapon_index = unit->weapon_indices[slot];
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

/* distance comes before speed_out: retail's caller (0x1ffbd0) loads distance (edx) before
   speed_out (eax), which our build does only in this declared order */
// @retail 0x100dd0
bool weapon_barrel_aim(long weapon_index, short barrel_index, point3f const *origin, point3f const *target,
	real const *unknown3, bool unknown5, vector3f *direction, real *time, real *distance, real *speed_out, bool *linear)
{
	bool result = false;
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);

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
	short secondary_ticks = NONE;
	short ticks = NONE;
	long first_person_state = NONE;

	switch (state)
	{
	case 6:
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
	case 1:
	case 2:
	case 3:
	case 4:
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
	case 0:
		break;
	case 5:
		break;
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

/* the item flags of a weapon as the item code declares them */
struct s_weapon_item_flags_view
{
	byte unknown000[0x12c];
	union
	{
		byte flags;
		struct
		{
			word in_inventory : 1;
			word flag1 : 1;
			word flag2 : 1;
			word flag3 : 1;
			word flag4 : 1;
			word flag5 : 1;
			word flag6 : 1;
			word flag7 : 1;
			word : 8;
		};
	};
};

static inline long weapon_get_owner_unit_index(long weapon_index);

// @retail 0x101db0
void function_101db0(long weapon_index, real heat)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	bool overheat_disabled = false;
	long unit_index = weapon_get_owner_unit_index(weapon_index);

	if (unit_index != NONE)
	{
		s_weapon_unit *unit = WEAPON_UNIT_GET(unit_index);

		if (unit->player_index != NONE)
			overheat_disabled = function_159dd0(unit->player_index);
	}
	if (TEST_FIELD_BIT(((s_weapon_item_flags_view *)weapon)->flag3) && weapon->heat < 1.0f && !overheat_disabled)
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
	s_weapon *weapon = WEAPON_GET(weapon_index);
	long ticks = ((s_game_time_globals const volatile *)g_510c54)->field_2_3;

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

	if (!interrupted && magazine->state == 3)
		function_105fa0(weapon_index, 12);
	else if (interrupted || magazine->state != 4)
		function_105fa0(weapon_index, 0);
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
		s_weapon *current = WEAPON_GET(weapon_index);
		if (current->animation_state_offset != NONE)
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
		if (TEST_FIELD_BIT(weapon->in_inventory) && weapon->unit_index != NONE)
			function_c9d00(weapon->unit_index, weapon_index, state);
		result = true;
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
	long const *weapon_reference = &weapon_index;
	s_weapon *weapon = WEAPON_GET(*weapon_reference);
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
	case 4:
		if (state != 3)
			break;
	case 0:
	case 5:
		if ((byte)function_101e80(weapon_index) && function_1008f0(magazine_index, weapon_index, false) > 0 &&
			magazine->rounds_loaded < magazine_definition->rounds_loaded_maximum)
		{
			function_1058b0(weapon_index, magazine_index + 5, false);
			function_105a80(state, weapon_index, magazine_index);
			function_1039a0(weapon_index, magazine_definition->reloading_effect, NONE, 0.0f, 0.0f);
			result = true;
		}
		break;
	}
	return result;
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
	long const volatile *weapon_reference = &weapon_index;
	s_weapon_definition *definition = WEAPON_DEFINITION(WEAPON_GET(weapon_index));
	bool result = false;

	if (magazine_index >= 0 && magazine_index < definition->magazine_count &&
		function_102a80(weapon_index, (short)magazine_index))
	{
		long i;

		result = true;
		function_b7360(weapon_index);
		for (i = 0; i < definition->trigger_count; i++)
		{
			s_weapon_header *headers = (s_weapon_header *)((s_record_pool volatile *)g_4e0300)->data;
			s_weapon *weapon = headers[weapon_index & 0xffff].weapon;
			weapon->triggers[(short)i].state = 0;
			weapon->triggers[(short)i].timer = 0;
		}
		for (i = 0; i < definition->barrel_count; i++)
		{
			if ((short)i >= 0 && (short)i < 2)
			{
				s_weapon_barrel *barrel = &WEAPON_GET(weapon_index)->barrels[(short)i];
				if (barrel->state != 1)
					barrel->flag6 = false;
			}
			if (definition->reload_style != 1)
				function_103dd0(*weapon_reference, (short)i);
		}
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

	function_105be0(weapon_index);
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
						c_type_709360 animation_id = state.overlay_or_animation_get(NONE, animation_name, NONE);

						if (animation_id.index != NONE)
						{
							long graph_index = player_animation->graph_index;

							weapon->value250 = graph_index;
							weapon->animation_254 = animation_id;
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
// score 17 (was 17): three early lookup operand orders; tail keeps header data in EBP and
// uses ECX for the holder offset with a late default and AL result, retail ECX data and EDX/DL selection.
// Round 2 (w2A): retail's lookups 1-3 and the tail are offset-first (`[esi+eax+8]`) and lookup 4 (the animation/inventory
// test) is data-first; ours is data-first at 1-3 and offset-first at the tail. All of 1-4 share one class: a
// `long weapon_offset = (weapon_index & 0xffff) * 12; ... *(s_weapon **)(weapon_offset + (byte *)g_4e0300->data + 8)`
// at lookup 1 flips 1-3 and the tail to retail's order but also lookup 4 (15); writing lookup 4 data-first flips
// 1-3 back (59, 43 with every lookup spelled out); lookup 4 through an own offset local, a separate getter or
// `& 0xffff` twice did not split it (all 15). The tail's slot read written retail's way (`long w = NONE; if (slot
// != NONE) w = unit->weapon_indices[slot];`, also via the weapon_inventory_unit_get helper) gives retail's
// branchless `or edx,-1` shape but shows 41, because retail also keeps data in ECX (ours reuses EBP, pool) and
// copies it (`mov eax,ecx`) before the unit lookup. Without the `volatile` the whole compare block is dropped (176,
// 489 bytes): retail keeps a dead byte store into the `immediate` slot (esp+0x1c). Assigning `immediate` itself 175.
// Shared data with result-first selection 43; typed holder headers 43; pool local 43;
// existing weapon getter at every lookup with result-first selection 41; result first 41;
// split comparison declaration 41; nested holder index 41; mixed pool/data locals 41;
// function-scope holder pointer 41; existing object-header getter 41.
// Permuter, 25 minutes (11-05): nothing below 17.
// M3 lane 6 r196 (fast with 0xc9d00, which matched each time; plain where noted): the block after the barrel loop is
// 0x1058b0's body for state 10, and the `volatile` compare is 0xc9d00 expanded for type 10 (its `first` flag is the dead
// byte store into the `immediate` slot). `function_c9d00(put_away->unit_index, weapon_index, 10)` in place of the
// volatile block 27 (plain 27, 532 B): every line from the b7360 call on is retail's (`mov eax, ecx`, `or edx, -1`,
// `sete dl`), but the weapon offset and the pool pointer swap ESI and EBP and the animation pointer and the weapon-kind name EDI and ESI. Not
// kept (above 17). On it: weapon_get_102e10 at lookups 1, 2, 3, every pair, all three, the put-away and the animation
// lookups 27 each; 0xcd660 called for the dual-wield test 106 (511 B: its /Ob1 file is never expanded); 0x1058b0's text
// mirrored (the lookup-3 pointer reassigned for the re-fetch) 27, with the animation lookup named 27; the holder through
// weapon_inventory_unit_get 27; a trigger pointer from a fresh lookup 137 (533 B); `result` declared first 27; the
// animation pointer after the weapon-kind name 185 (570 B); the dual-wield helper with a result flag 27; the holder helper without
// its NONE test 27; per-loop counters 27; `result = true` ahead of the ticks store 49. The whole block as a call to
// 0x1058b0(weapon, 10, immediate) 117 (503 B: LTCG expands it with retail's tail, 0x101f20 and 0xcd660 stay calls). On
// this body, weapon_get_102e10 at lookups 1-3 15 (they take retail's offset-first order, lookup 4 flips). The permuter
// cannot search the 0xc9d00 form (its reduced image scores it 80, the full image 27).
// M3 lane 9 r143 (fast, plain where noted; a reduced-link LTCG screen of this file reproduced the fast build): the 0xc9d00
// call in place of the `volatile` block re-measured 27; on it `bool result = false;` declared first in the body 4 (every
// register then retail's; left: lookups 1, 2, 3 and the put-away lookup data-first, retail offset-first); with
// weapon_get_102e10 at the first lookup as well 2 (plain 2: lookups 1-3 take retail's order, the animation-test lookup
// flips with them, the put-away lookup stays data-first). Not kept: the declaration move carries the gain (without it 27).
// On the 2 form, no change: the getter at every other subset of lookups (31), the holder and dual-wield helpers with a
// pool copy, the getter, a result flag or no NONE test, the tail through the holder helper, the put-away pointer reusing
// `current`, the animation test named, through a pool copy or through the getter; dead initialised locals 1-16 change
// nothing on either form. Diagnostics: only a named `weapon_index & 0xffff` local at the first lookup flips the order
// (the getter's local order is irrelevant); 0xc9d00 written with UNIT_GET at both uses or a header-array local in place
// of its pool copy (scratch copies of its file, which is not ours this round) gives the put-away lookup retail's
// offset-first order but drops retail's `mov eax, ecx` copy (13). The permuter cannot search this form (reduced 48, full 2).
// M3 lane 11 r129 (fast with 0xc9d00, MATCH each time), on lane 9 r143's 2 form re-measured (2: the animation-test lookup
// offset-first where retail is data-first, the put-away lookup data-first where retail is offset-first; the two always
// flip together): the put-away written out at each use 2; the tail through the holder helper's result 2; the holder
// helper through a header pointer without its NONE test 2, with a named index and headers local 2, through the getter 2;
// the dual-wield helper with a result flag 2, a headers local 2, a named index 2, a header pointer 2; the animation test
// through a header pointer 2, through the getter 2; the put-away through the getter 2; the put-away pointer declared at
// the top (either side of `result`) 2, at the top of the gate block 2; `long i` declared first 2. Retail's 0xc9d00 is
// data-first standalone and the put-away flip follows its pool-copy form (r143), so the lead stays in its file. Also on
// the 2 form: the put-away read through a pool copy local 2, through a headers local 2; one pool copy for the animation
// test and the put-away 219 (593 bytes).
// M3 lane 3 r271 (fast with 0xc9d00, 0x1058b0 and 0xfff40 in the image, all matched each time; the five weapon lookups'
// address orders are written O for offset-first and D for data-first: retail O O O D O with the `mov eax, ecx` copy): the
// 0xc9d00-call form with `result` at the top re-measured 4 (D D D D D), with weapon_get_102e10 at the first lookup 2
// (O O O O D). The flip of lookups 1-4 comes from any one-use local in the first lookup's statement, not from the masked
// index: a one-use pool copy there (`s_record_pool *objects = g_4e0300;` read once) 2, a one-use offset local written on
// either side of the data 2/2. The same locals used at two lookups 4 (D D D D D): the pool copy at lookups 1 and 2, that
// with own copies at lookups 3 and 5, the offset local reused at lookup 2 or 5; the offset local at the animation test or
// at every lookup 34 (534 B). Written operand order never counts: a lookup spelled offset-first with no local at lookup
// 1 or 5 of the 4 form, or at the animation test or lookup 5 of the 2 form, same code. On the 2 form, no change (2): the
// getter at lookups 1 and 5, at all five; lookup 5 or the animation test through a one-use pool copy; the animation test
// through a one-use header-array local; `result` declared after the definition (the order of 0x1058b0 and 0x105c20),
// `i` declared first, the put-away pointer, `current` or the definition declared bare at the top. Worse: one pool copy at
// the top for lookups 1, 2, 3 and 5 182 (564 B: the copy takes EBX and its data address is strength-reduced); the trigger
// loop through a trigger pointer from a fresh lookup 131 on the 4 form, 130 on the 2 form (533 B); a pool copy shared by
// the animation test and a written-out holder lookup 200 (579 B); the dual-wield helper through a twice-used pool copy
// 200 (579 B, diagnostic); the animation test through `current` 185 (576 B). The permuter on the 2 form with 0xc9d00,
// 0x1058b0 and 0xfff40 as context: 19 tries in 600 s (its reduced image is 542 files), nothing below 2. Matched 0x101490
// has retail's offset-first trigger loop beside data-first lookups only through its `volatile` pool read, so that
// lookup is a separate expression there. Left: what keeps the animation test's lookup data-first and the put-away lookup
// offset-first (with the copy for 0xc9d00's pool copy) once the others are offset-first.
// M3 lane 16 r163 (fast with 0xc9d00, MATCH each time), on the 2 form re-measured (2): the holder unit declared ahead of
// the weapon class 121 (540 B); the existing 0x101f20 for the holder lookup 51 (533 B, it stays a call); lookup 3 after
// the b7360 call 119 (525 B); per-loop counters in the two loops 2 (same code); a trigger pointer in the reset loop 130.
// M3 lane 14 r133 (fast with 0xc9d00 and 0x1058b0, both matched each time; control 17): the 0xc9d00-call forms rebuilt
// 27 / 4 (`result` first) / 2 (and weapon_get_102e10 at lookup 1, O O O O D). On the 2 form: the trigger loop through an
// inline (weapon index, trigger index) clear helper 2 (same code); both reset loops through one inline helper taking the
// definition 253 (425 B); the trigger loop through a trigger element pointer 130 (533 B); the unit index declared first
// in the animation block 121 (540 B); the weapon-kind value declared before the animation pointer 2 (same code); that
// value as one ternary initialiser on the dual-wield test 39. In our build lookups 1-4 always share one order and the
// put-away lookup takes its own (main D D D D O, the 2 form O O O O D); retail's lookup 4 is the odd one.
// M3 lane 2 r126 (fast with 0xc9d00, MATCH each time), on lane 9 r143's 2 form rebuilt (2): the put-away lookup through a
// reference-bound header array 14 (533 B: the lookup takes retail's offset-first order, but 0xc9d00's data load is then
// no longer shared and the `mov eax, ecx` copy is lost), through a named header local re-bound by a const reference 14;
// the put-away pointer bound to a const reference 49 (537 B), re-bound by a const reference 2; the animation test
// through a reference-bound header array 31 (534 B, still offset-first), through a named header local re-bound by a
// const reference 31, through a const-reference-bound pointer 2, through a named pointer re-bound by a const reference 2;
// the 0xb7360 call ahead of the lookup-3 pointer 119 (525 B); a reference-bound header array at lookup 1 in place of the
// getter 21; both helpers `__forceinline` 2; the existing weapon_get_owner_unit_index in place of the holder helper 2
// (same code; on main's body 17, same code); redundant parens around the animation test, the put-away NONE test, the
// gate's state read, the 0xc9d00 argument, the holder call or the animation-pointer sum 2 each (same code).
// M3 lane 3 r296 (H2M_FAST with 0xc9d00 and 0x1058b0, both MATCH each time): the 2 form rebuilt (2: lookup 4
// `[esi+ecx+8]` where retail has `[ecx+esi+8]`, the put-away lookup the reverse). On the 0xc9d00-call form: early returns
// in place of `result` (gate `if (!immediate && !replaceable) return false;`, `return true`) 121 (520 B); `result`
// declared bare and assigned false in an else arm 121 (520 B), with weapon_get_102e10 at lookup 1 121. On the 2 form:
// the animation test and the holder helper both through weapon_get_102e10 2; the put-away pointer `s_weapon const *` 2;
// renames of the put-away pointer, `current` and `weapon` 2 each (inert).
// M3 lane 4 r249 (h2m fast with 0xc9d00 and 0x1058b0, MATCH each time), on the 2 form rebuilt (2): the holder helper's
// weapon read as `(&headers[index])->weapon` (0x101f20's spelling) 2; the weapon-kind value set in an if/else with no
// initialiser 39; the holder and dual-wield tests in one inline helper taking the weapon index 2; the definition read
// through a second weapon lookup 2 (the 2s the same code); the animation test's index cast to short before the mask 104
// (545 B, the offset is no longer shared).
// M3 lane 1 r157 (h2m fast with 0xc9d00 and 0x1058b0, both MATCH each time; control 17). Rebuilt: the 2 form (0xc9d00 call,
// `result` first, weapon_get_102e10 at lookup 1) 2, the two rows left being base/index swaps (animation-test lookup `[esi +
// ecx + 8]` for retail's `[ecx + esi + 8]`, put-away lookup the reverse), and the plain 0xc9d00-call form 27. Retail's own
// 0x1058b0 and 0xc9d00 are data-first at every lookup, and retail's `mov eax, ecx` here is 0xc9d00's `mov eax, [pool +
// 0x44]` forwarded from the put-away lookup's register, so retail's put-away data load is a single-use load when the
// lookup is formed. On the 2 form, a file-local inline accessor returning the pool's header array as a one-pointer
// aggregate by value (the form that matched 0x151b80): at the put-away lookup 14 (533 B: that lookup takes retail's `[esi
// + eax + 8]`, but its data goes to EAX and 0xc9d00's load stays a second load, no copy); at the animation-test lookup
// 206 (583 B, an 8-byte frame); at both 207 (584 B); at lookup 3 19 (533 B); at lookup 1 in place of the getter 20; at all
// five 135 (551 B); in the dual-wield helper 201 (579 B). Pool-copy spellings (`s_record_pool *objects = g_4e0300;`
// read once) in the dual-wield helper 2, that with a result flag as 0xcd660 has it 2, in the holder helper 2, in both 2
// (all the same code). The state block as a call to the existing 0x1058b0 (`if (function_1058b0(weapon_index, 10,
// immediate))`) re-measured 117 (503 B: 0x101f20 and 0xcd660 stay calls, every lookup data-first), with
// weapon_get_102e10 at lookup 1 151 (525 B), that with `result` first 151. Diagnostic (a forced-inline file-local twin
// of 0x1058b0's body with its own weapon index, state and force parameters, reverted): with the getter and `result`
// first 2 (the 2 form's code), `result` after the loops 27, the result taken straight from the twin 49 (534 B), no
// getter 27; so inlined formals do not move the two lookups either. Diagnostic on 0xc9d00's own lookups (a scratch edit of
// its file, restored at once; the file is another lane's this round), with the 2 form here: its first unit read, both
// reads, or both with no pool copy through a by-value header-array accessor 78 (540 B) each, and 0xc9d00 itself 83 and
// 0x1058b0 49 with them, so 0xc9d00's reads are plain and the by-value form is not what separates its data load.
// M3 lane 13 r145 (h2m fast with 0xc9d00 and 0x1058b0, MATCH each time; control 17; the 0xc9d00-call form rebuilt 27, with
// `result` first 4, with weapon_get_102e10 at lookup 1 as well 2): on the 2 form one, two or three redundant parenthesis
// pairs around the b7360 argument, the lookup-3 initialiser, the gate's state read or the animation test's lookup 2 each
// (twelve forms, same code: the address operand orders ignore parens). Diagnostics on the 2 form: the dual-wield test
// removed 116 (501 B) or the whole holder test removed 239 (421 B) leave lookup 4 offset-first and the put-away lookup
// data-first, so lookup 4's order does not come from its data load being shared with the unit lookups. On the
// 0xc9d00-call form the put-away pointer as `current` reassigned 27 (same code); the 0xc9d00 call's unit argument read
// again through the getter 67 (on the 2 form 42).
// M3 lane 14 r149 (h2m fast with 0xc9d00 and 0x1058b0, MATCH each time; control 17; the 2 form rebuilt 2): on the 2 form,
// lookup 4 indexed by `(word)weapon_index` 104 (545 B, the offset is no longer shared); the put-away lookup indexed so 58
// (536 B); the animation test as `NONE != ...` 2; the put-away NONE test as `NONE != put_away->unit_index` 2; the
// dual-wield test written in the condition through WEAPON_UNIT_GET with no helper 2 (the 2s the same code).
// M3 lane 14 r150 (h2m fast with 0xc9d00 and 0x1058b0, MATCH each time; control 17; the 2 form rebuilt 2, O O O O D against
// retail's O O O D O). A two-use pool local (`s_record_pool *objects = g_4e0300;` in the gate block) is the first form
// that gives the animation-test lookup retail's `[ecx + esi + 8]` at 532 B: with a named animation-test pointer through it
// (the holder's two fields read through that pointer) and the dual-wield unit looked up through it as well 2, but the two
// rows left are then lookup 3 and the put-away lookup, both data-first (O O D D D): a macro lookup whose pool load is
// followed by a pool local's own load of the global takes that local as its pool and goes data-first, which is also what
// 0xc9d00's pool copy does to the put-away lookup in every 0xc9d00-call form. On that form: the local declared after or
// ahead of the b7360 call 2 (same code), at the function's top 197 (576 B); `result` after lookup 3 27, no getter at lookup
// 1 4 (D D D D D), both 27; the getter at lookup 3, at the put-away lookup, at both 2 (same code); a second macro lookup
// beside lookup 3 for the gate's state read 2 (same code: equal lookups fold to one), a second one at the put-away test or
// for 0xc9d00's unit argument 42. Lookup 3 keeps its order only when another macro lookup follows the local: the test
// through the local with the holder helper as is 206 (583 B: lookup 3 O, lookup 4 D, but the helper's lookup is repeated
// through a `mov edx, ecx` copy and the frame grows), the test and the holder each through the local with the dual-wield
// helper as is 200 (579 B). The local used once (the dual-wield helper as is) 2, and a two-use header-array local in its
// place 2 (both the 2 form's code: they are forwarded). Diagnostics: the three helpers as functions of another unit that
// the link-time inliner expands 2 (the 2 form's code, so the order is not set per unit); the existing 0xbf5a0 for the
// animation test 106 (547 B, it stays a call). Left: a form that puts only the animation-test group on a pool local
// without that local, or 0xc9d00's, taking over the lookups ahead of them.
// M3 lane 7 r223 (h2m fast; control 17): the reassociation order reads each lookup as ADD(offset, pool data, 8), a
// CSE temp ranked by its value-table number and a single-use data load ranked above every temp; on every 0xc9d00-call
// form the put-away lookup's reloaded data is used once (0xc9d00's own load stays a separate value, later forwarded as
// `mov eax, ecx`), so it stays data-first. Tried: weapon_get_102e10 at every subset of the six weapon lookups (63 forms
// on this body) 15 with lookup 1 in the subset, 17 without; that with `result` declared first 15; that with the put-away
// re-fetch assigned to `weapon` 152 (526 B). Diagnostics (reverted): on the 2 form a dead holder-unit lookup ahead of
// the 0xc9d00 call 42; 0xc9d00's second read through UNIT_GET (its own file) 41, with 0xc9d00 at 131.
// M3 lane 14 r152 (h2m fast with 0xc9d00 and 0x1058b0, MATCH unless said; control 17; rebuilt: the 2 form 2, the plain
// 0xc9d00-call form 27, that with `result` first 4). What sets the five address orders, read from the operand ranking:
// each lookup is one sum (offset, pool data, 8) laid out in descending rank. The shared offset is an optimizer
// temporary ranked by where its entry falls in the function's intermediate-code storage. A data load whose address
// `pool + 0x44` is shared by two loads with a kill between them (lookups 1 and 2 round the trigger loop, 3 and 4 round
// the 0xb7360 call) takes a fixed low rank; a data value used twice (lookup 4 and the dual-wield unit) is a temporary
// of its own; a single-use load through the pool global (the put-away lookup of every 0xc9d00-call form) ranks above
// every temporary, so that lookup is data-first whatever the others do. On the `result`-first form the offset
// temporary is made ahead of lookup 4's data (lookup 4 data-first as retail) but ranks just below the fixed rank
// (lookups 1-3 data-first); any one-use local in the first lookup's statement (the 2 form's getter, a pool copy, an
// offset local: 2 each, the same ranks) lowers the fixed rank but makes the offset temporary after lookup 4's data.
// Diagnostic, not source: 128 or more dead initialised locals (here or in an inlined helper) move the temporaries
// 0x100 entries up and leave 1 (only the put-away lookup's order; 256 on the plain form 27); 1-112 change nothing
// (entries come in blocks of 0x40), nor do uninitialised locals, the sizes of other functions, of the function
// compiled before this one or of a called callee, or 300 file-level globals. So retail's body carries about 230 or
// more further entries of intermediate code than ours when its temporaries are made, and its put-away data load shares
// its address with another load. Forms on the 2 form unless said: the tail through the holder helper's result 2 (the
// put-away offset is still the shared temporary: inline parameters are substituted); the getter at all five lookups 2,
// with the 128 locals 2; an unused holder-unit lookup ahead of the 0xc9d00 call 42 (re-measured: no shared data value,
// the code shifts); the real 0x101f20 and 0xcd660 in place of the two file-local twins with their /Ob1 files made
// expandable (diagnostic on two contributor files, restored at once) 27, the same code as the twins' plain form, and
// the whole state block as `if (function_1058b0(weapon_index, 10, immediate))` 27, with `result` first 4 (the same
// code again, at retail's 532 B: retail's block is that call expanded, and what keeps it from being written so is
// only that the two helpers' /Ob1 files are never expanded; with them expandable 0x101690 58 and 0x1058b0 78 expand
// them where retail calls). 0xc9d00's own reads (its file, ours): the first hand through the existing 0xcbd50 with
// plain unit lookups 41 (0xc9d00 131, 348 B), both hands through its pool copy 2 (0xc9d00 120), the first plain and
// the second through the pool copy 41 (131), the second-hand test computed ahead of the first 33 (84): only the
// present spelling keeps 0xc9d00. The pool global compared before and after the animation call survives (fast and
// plain): that call counts as writing the pool global, so the put-away lookup's address is not shared with lookup 4's.
// Also on the 2 form: the put-away lookup through a const reference bound to the pool's data field itself 2 (same
// code: the reference is folded back and the load stays single-use); 0xc9d00's unreachable tail shortened or lengthened
// (scratch copies of its file) 2, and with the 128-local probe 4 instead of 1: an inlined callee's dead code counts
// toward the entries, so the probe sits right at a block boundary. Diagnostic: with 0xc9d00 called for type 3 (its
// second-hand reads then live) the put-away data becomes a shared temporary and that lookup turns offset-first; for
// type 10 those reads are removed before the sharing pass, so they cannot be what gives retail its order.
// M3 lane 12 r189 (reduced-image screen with 0xc9d00, 0x1058b0 and 0xfff40, MATCH each time; control 17; rebuilt: the 2
// form 2, the `result`-first 0xc9d00-call form 4). Operand ranks read per form: every expression node is numbered in
// program order, two apart. The pool data load takes a number X at its first reading and X+4 for the reading lookup 4
// shares with the dual-wield unit; an unshared reading ranks by X's low three bits alone. With the macro at lookup 1 the
// mask and the offset are numbered just ahead of the data (offset X-2, under the shared X+4: lookup 4 D as retail) but
// X is 4 mod 8, above the offset (lookups 1-3 D); with a one-use local at lookup 1 the data is numbered first (X 0 mod
// 8: lookups 1-3 O) and the offset last (X+8: lookup 4 O). Lookup 1 on the `result`-first form through a one-use
// header-array local, a one-use masked index, a one-use header pointer or a one-use pool copy 2 each (the 2 form's
// code). Diagnostic (reverted): on the `result`-first form with the macro at lookup 1, a one-use local at the top
// holding two expression nodes of the weapon index and read at the very end gives O O O D D, all of lookups 1-4
// retail's; one node D D D D D (X 6 mod 8), four nodes X 4 mod 8 again, and the two nodes placed after lookup 1 change
// nothing. An unused local at the top initialised with a shift of the index, that narrowed, or that masked 4 each
// (unused expressions are dropped before the numbering). So retail evaluates two (or three) more live expression nodes
// ahead of the first lookup's data load; the put-away lookup's data is a load seen once (the pool global is read again
// after the animation call) and stays D in every form here.
// M3 lane 3 r310 (fast screen with 0xc9d00, 0x1058b0, 0xfff40, 0x103e60 and 0xb7360, MATCH each time; control 17). On
// main's body, the `volatile` compare's tail: the compare's sides swapped 17; the slot ternary negated (`slot == NONE
// ? NONE : ...`) 17; the holder's unit index named for the unit lookup 17 (all the same code); the slot read at both
// uses with no local 26; the slot as `long` 47 (527 B).
// M3 lane 6 r246 (h2m fast with 0xc9d00, 0xfff40 and 0x1058b0, MATCH each time; control 17), on main's body: the
// definition read through its own weapon lookup 17; `bool result = false;` declared first 17 (both the same code); the
// replaceable test reading the state through a fresh lookup 206 (581 B); the volatile compare's slot selection named
// as a `long` with the slot field read at each use 26; the barrel loop on its own `short` counter passed straight 110
// (535 B). BLOCKED-BY 0x101f20 and 0xcd660 (retail expands both inside its copy of 0x1058b0 here; their files are
// built /Ob1, so the call form keeps them as calls).
// M3 lane 14 r160 (fast screen with 0xc9d00, 0x1058b0 and 0xfff40, MATCH each time; control 17), on main's body: the trigger
// reset through the outer `weapon` pointer 156 (521 B); the trigger index without its `(short)` cast 46 (535 B); the
// ticks store through a fresh lookup 148 (539 B), through `current` 150 (542 B); the gate's state read through a fresh
// lookup 206 (581 B); the barrel call's index without its cast 17 (same code).
// M3 lane 3 r316 (fast screen with 0xc9d00, 0x1058b0 and 0xfff40, MATCH each time; control 17), on main's body: the
// trigger reset through the file's existing weapon_clear_trigger (its definition moved ahead of this function) 129,
// with its own `short` counter 131; the `volatile` block as the 0xc9d00 call 27 (listed, re-measured), with the
// helper 137 (533 B); the animation pointer formed from the first `weapon` pointer as 0x1058b0 does 188 (578 B).
// M3 lane 13 r167 (fast screen with 0xc9d00, 0x1058b0 and 0xfff40, MATCH each time; control 17), on main's body: the two
// animation_set tests merged into one `||` condition 17 (same code); the `volatile` compare through the existing 0xcbd50
// with the unit's current slot 43; weapon_get_102e10 at lookup 1 15 (lookups 1-3 retail's, the animation-test lookup
// flips, the tail unchanged), at lookups 1-3 15 (not kept: two points); that with the `volatile` block as the 0xc9d00
// call 27. Retail's tail is 0x1058b0's expansion with 0x101f20 and 0xcd660 inside: BLOCKED-BY 0x101f20 and 0xcd660.
// M3 lane 11 r178 (fast screen with 0xc9d00, 0x1058b0 and 0xfff40, MATCH each time; control 17; the 2 form rebuilt 2):
// the state block as `if (function_1058b0(weapon_index, 10, immediate)) { ticks; result = true; }` 117 (503 B: the
// two helpers stay calls), as `bool result = function_1058b0(...); if (result) ticks;` 112 (508 B), as an early return
// on that call 117 (503 B); on main's body `bool result = false;` first 17 (same code), that with weapon_get_102e10 at
// lookup 1 15 (the getter's listed 15). Body unchanged.
// M3 lane 15 r157 (fast screen with 0xc9d00, 0x1058b0 and 0xfff40, MATCH each time; control 17), on main's body: the
// put-away guard as nested ifs 17; the definition pointer to const 17; the holder's unit index named for the unit
// lookup and its two reads 17 (the three the same code); the weapon class chosen by a conditional expression 52 (first
// +0xc: the class constant is set up ahead of the trigger loop), with the state arms unbraced as well 52. Body
// unchanged; still BLOCKED-BY 0x101f20 and 0xcd660 (retail expands both here; their files are built /Ob1).
// @retail 0x100130
bool __stdcall function_100130(long weapon_index, bool immediate)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	long i;

	for (i = 0; i < definition->trigger_count; i++)
	{
		s_weapon *owner = WEAPON_GET(weapon_index);

		owner->triggers[(short)i].state = 0;
		owner->triggers[(short)i].timer = 0;
	}
	for (i = 0; i < definition->barrel_count; i++)
		function_103e60(weapon_index, (short)i);

	s_weapon *current = WEAPON_GET(weapon_index);
	bool result = false;

	function_b7360(weapon_index);
	if (immediate || weapon_state_replaceable(current->state, 10))
	{
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
			short slot = unit->current_weapon_slot;
			volatile bool is_current = weapon_index ==
				(slot != NONE ? unit->weapon_indices[slot] : NONE);
		}
		weapon->state_ticks = first_person_weapon_animation_ticks(weapon_index, 0x8000025, 1);
		result = true;
	}
	return result;
}
/* takes rounds from a weapon's reserve for a magazine: first its own, then
   the reserves of the holder's other weapons of the same kind */
// MiniMax B (11-12): no new form, no lower score (83): the difference is register roles only (retail keeps
// the magazine index in esi and the available/taken value in edi, ours the other way round, and retail holds
// the pool global in ebp where ours reloads it per slot); the pool-in-ebp shape needs the pool global to be
// known not to change across 0x106030 -> 0x159dd0, which lives in another lane's file.
// MiniMax G (11-13): matches. The unit comes out of the pool's data through the same expression as the other
// weapons' lookups, so the pool's data pointer stays in one register across the first slot, and the slot test
// compares the other weapon's definition index with the weapon's, the other way round, so that both sides of
// the comparison are materialised the way retail has them. The first block's `unloaded` is a short, which loads
// the rounds with a 16-bit move and sign-extends it, and its minimum goes through a `take` local, which gives
// retail's in-place `cmovle` on the loaded value.
// MiniMax G (11-10): no new form, no lower score (83).
// score 83 (was 107, fast): the slot test reads `weapon->definition_index == other->definition_index`, the slot take
// reads `magazine->rounds_unloaded` at each use (no short local), and the first block's `unloaded` is a long. Found
// from a permuter run (11-05) whose best form (81) also needed a volatile read, and kept without it; the operand order
// alone gives 110, the inline reads alone 163, the two together 85, with the long 83. 0x102b90 still matches.
// MiniMax A (11-04): no new form, no lower score; the result flag declared before the availability query
// scores 110 and after it 193.
// MiniMax H (11-03): no new form, no lower score; retail keeps the magazine index in esi and the available count in edi where ours has them the other way round, and retail holds the pool in ebp where ours reloads the pool global for each slot.
// score 107 (was 124): the pool pointer is read once into a local that lives across the whole body, the
// definition is read through the live weapon, `taken` is declared inside the availability gate and the
// other-weapon receiver is a single local reused by the two reserve accesses. Still open: the magazine,
// available and taken registers and the per-slot pool reload differ from retail.
// score 124 (was 124): frame eight bytes agrees; magazine, available and taken registers and pool reloads differ;
// retail holds the object pool in ebp across the four unrolled slots, the full image reloads it per slot.
// tried all lookups pool value 124; needed result first 171; positive take continue 124; availability before update 124; first reserve field reference 123.
// tried return from test 181; other reserve reference 123; weapon record reference 124.
// tried read-only settings 124; reserve publication 124; storage names 124.
// tried size policy 451; inline depth zero 124.
// Success before transfers 124; delayed tag 197; remaining need 224;
// need before holder 125; request-first clamp 182; upper bound first 124;
// actual tag reference 124; unsigned slot counter 124. Magazine/transfer
// registers and pooled receiver homes still differ.
// Original getter, per-slot getters, all object getters and header getters 124 each; first/other/both
// magazine references 124 each; unit reference 124.
// Round 127: single unavailable-reserve exit 124; signed-byte result 200;
// signed/unsigned halfword results 204/204; int slot index 124;
// state query, reserve notification and transfer noinline 124 each.
// Round 50: no definition-index local with taken declared in the transfer block 183 (574 bytes; the
// reduced image builds retail's shape at 585 bytes, the full image reloads the pool per slot);
// the same with a pool local for the holder loop 183; with an other-weapon local and continue 165
// (fast, as the previous); with `return true` on success 179 (fast); `return true` on success alone 124.
// Round 194: first/other/both magazine array roots 123/123/122 (586/586/585 bytes);
// first/other/both roots with unit observations 231/227/226; both roots with live handle
// 180, also with unit observations 211. The two-point retail-length form is restored;
// magazine, availability/transfer and pooled receiver homes remain.
// Round 271: consistent header-data snapshot with retained/live handle, entry/gate
// counter and short/long reserve values: 182/221/191/217/201/234/184/211.
// All restored; magazine/availability and receiver homes still differ.
// Round 423: consistent first/other/both reserve-field reads 124/127/127; each with
// a real per-slot weapon receiver 124/127/127; both fields with short slot counter 430,
// also with the receiver 430. Reduced previews only; complete original body restored.
// Round 87: retail-shaped body (no definition-index local, taken scoped in the success block, other-weapon
// pointer) 183 (frame four, 574 bytes; the reduced image gives retail's BL result and pool in EBP, 165); MIN-macro
// minimums same code; no definition-index local 198; taken in the success block 161; result declared first 124;
// if-form slot minimum 171; other-weapon local with continue 124. Our full image keeps the unit in EBP and
// reloads the pool global per slot where retail keeps the pool and spills the unit.
// Round 221: complete success/failure result assignments 124; first/other/both magazine
// array roots on that graph 123/123/122; entry-first result declaration with no/first/other/both
// roots 124/123/123/122. One/two-point gains rejected; original body restored.
// Round 17: no definition-index local 175 (fast, 585 bytes, aligned 72); plus taken scoped in the
// success block 165 fast (585 bytes, aligned 58: BL result and pool in EBP as retail, only ESI/EDI
// swapped between the magazine index and available/taken) but 183 in the full image (574 bytes,
// frame four: the unit takes EBP and the pool is reloaded per slot; no non-LTCG object references the
// pool global); on that, result declared first 168, other-weapon pointer 165, available renamed 165.
// Round 2 of lane 18 (plain checks; the count stays in EDX across the slots where retail rereads it
// from its slot, which leaves no register for the pool): volatile count reads in the loop 416 (loop
// no longer unrolled, 348 bytes); count's address taken 124; count read through its address in the
// loop 124 (same code); taken starting at the count with an if clamp 182; long first-magazine
// rounds 123 (586 bytes). Original body restored.
// Round 94 (LTCG test link that gives the full image's listing): no definition-index local with taken in the
// success block 183 plain (574 bytes; the reduced image's 165 shape needs 0x159dd0's callee left unresolved);
// with other-weapon pointer and continue, `count > taken`, result first or MIN-macro take: same shape; short slot
// index leaves the loop rolled (352 bytes). Original body with other-weapon pointer or unit pointer declared
// first: same code; definition index read in the success block 580 bytes. Retail's pool-in-EBP slots appear
// only when the pool global is treated as escaped, which changes 732 other functions.
// Round 241: complementary outcome alone/unsigned tag/first long reserve/other long reserve/
// both long reserves/tag-first/tag-other/tag-both 212/212/211/183/182/211/183/182.
// Original body restored; magazine/transfer and pooled receiver homes remain.
// Round 251: Remaining-expression / counter-first / both / compound reserve store / remaining-compound /
// counter-compound / all / all with magazine roots 180/130/180/124/180/130/180/180. Original restored;
// magazine/transfer and pooled receiver homes remain.
// Round 123 (lane 6; one-link LTCG harness of this file with globals, 0x157450, juggernaut and 0x0b58c0, which gives
// the full image's listing): the retail-shaped body (no definition-index local, taken in the success block) has every
// block and exit as retail; left are the unit kept in EBP (retail spills it, pool in EBP) and the magazine index /
// taken in EDI/ESI (retail ESI/EDI). Retail's pool-in-EBP and frame eight appear here only when the 0x1008f0 or
// 0x106030 call may write the pool global (an impossible-path store of it in either, or a juggernaut test left
// unresolved or built without /GL); a store in 0xb7360 or of another global does not. Retail's 0x1008f0 holds the
// pool global in EBP across its own 0x106030 call and 0xb9b90/0x100130/0x1058b0 hold it across 0xb7360, so retail
// did not see those calls as writing it: the cause is still unknown. Even with that store the ESI/EDI swap and the
// count held in EDX remain (harness 159 positional). Plain: long first-magazine rounds 123 (586 bytes, as before);
// slot take tested `>= 1` 139; reserve count added before the store 130; definition compare operands swapped 170;
// slot tests weapon-first 176. Harness only, on the retail shape: other-weapon pointer, unit-index local, slot-array
// pointer, pool local, unit/taken declared first, int/dword slot index, unloaded-first take, compound store, `count >
// taken`, `count <= available`: same code; if-form slot/magazine take, named needed, long unloaded, early false
// return, the other eleven orders of weapon/definition/available/result and 1-8 unused locals at the top or before the
// unit: none closer.
// Round 35 (fast): a pool local declared in the holder block for the slot lookups, the unit through the
// macro 170 (586 bytes; 0x102b90 then differs), the unit through the local too 124 (same code: the local
// is folded back into per-slot global reloads); the slot lookups through the existing weapon_get_102e10
// 124, every weapon lookup through it 124, a two-use other-weapon local from it with `continue` 124.
// Ours keeps the count in EDX through the slots (its stack home reused for the unit) where retail rereads
// it from the stack and keeps the pool in EBP.
// Round 62: nested other-weapon pointer as in 0x1008f0 with the slot test on weapon->definition_index
// (no definition-index local) 198 (571 bytes; fast 175 at 585); magazine-offset local 177 (fast);
// definition read in the range test 198 (fast, 568 bytes); available before weapon/definition 182 (fast);
// unit-index local 124; first-block take without the short local 124. One-link diagnostic: with the
// entry weapon read not touching the pool global, the slots take retail's pool-in-EBP/unit-spilled
// shape (aligned 83), so the entry read joins the loop's pool range; no source form found that splits it.
// Round 62, later: `taken` as a short 209 (fast, 611 bytes).
// Round 51 (lane 10, plain checks): `result = true` at the top of the success block 124; result declared after
// taken 124; taken assigned (not initialised) at the top of the success block (the check tool raised before
// scoring); the definition index read after the 0x106030 call 122 (587 bytes; a one-use field read moved past a
// call, not kept); one magazine pointer declared at the top for both blocks 124; the slot's weapon index read from
// the unit at each use 124. Retail keeps a false result in BL across the 0x106030 call and returns it on the
// failure exit (constant 1 on success), so its result is not folded at the join; ours folds both exits.
// Round 52 (lane 1, plain): `long taken = 0;` after the 0x106030 call 126 (585 bytes); the first block's
// minimum as its own `take` local added to `taken` 178 (589 bytes); `definition` read after the 0x1008f0 call
// 174; the taken placement without the definition-index local 196 (569 bytes, 0x102b90 5); the taken placement
// with `return true` in the success block and `return result` at the end 130 (0x102b90 5).
// Round 19 (lane 17, fast; the reduced image builds 0x1008f0 differently, so these are previews): the
// definition index read in the holder block 195 (571 bytes); definition index and taken both in the
// success block 160; the other magazine as `->magazines + magazine_index` 124 (same code); the holder
// test as `taken < count && unit_index != NONE` 125; the slot take as `unloaded < count - taken ? ...`
// 126. The pool-global lead stands (0x106030 reaches 0x159dd0 in a file another lane holds this round).
// Round 52 (lane 1, plain): `long taken = 0;` after the 0x106030 call 126 (585 bytes); the first block's
// minimum as its own `take` local added to `taken` 178 (589 bytes); `definition` read after the 0x1008f0 call
// 174; the taken placement without the definition-index local 196 (569 bytes, 0x102b90 5); the taken placement
// with `return true` in the success block and `return result` at the end 130 (0x102b90 5).
// w1D (11-05): walked against retail (magazine and unit slot loops, minimum, guards, exits): no logic difference;
// the score is register roles only (retail keeps the magazine index in ESI and the available/taken value in EDI,
// ours the reverse, and it pushes EDI later). One variable for available and taken 107 (same code); taken and/or
// result declared at the top 180/110/110.
// MiniMax E (11-10): no lower score (83). The remaining difference is register roles only: retail copies
// the magazine index out of EAX into ESI, which is callee-saved and so survives the 0x1008f0 and 0x106030
// calls, and keeps the available/taken value in EDI; it re-reads the count from its stack slot at every use
// instead of holding it in EDX, and it keeps the pool global in EBP across the 0x106030 call. Retail also
// saves EDI late, just before the 0x1008f0 call, because the value is dead before it; ours saves it in the
// prologue because the magazine index already lives there. Ours keeps the magazine index in EDI and the
// available value in ESI, the other way round, holds the count in EDX and reloads the pool global per slot.
// The pool-in-EBP shape needs the pool global to be known not to change across 0x106030 -> 0x159dd0, which
// lives in another lane's file. The parameter layout is the same as retail's (the magazine index in EAX, the
// weapon index and the count on the stack, the callee popping them), so nothing here is blocked by the
// declaration. A permuter over this body (25 tries, 310 s) found no variant below the baseline, and an
// earlier run was stopped by the harness before it produced a result, so no permuter form is recorded here.
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
			long take = count <= unloaded ? count : unloaded;

			taken = take;
			magazine->rounds_unloaded = unloaded - (short)taken;
			if (taken > 0)
				function_b7360(weapon_index);
		}
		if (weapon->unit_index != NONE && taken < count)
		{
			s_record_pool *pool = g_4e0300;
			s_weapon_unit *unit = ((s_weapon_unit_header *)pool->data)[weapon->unit_index & 0xffff].unit;

			for (long i = 0; i < 4; i++)
			{
				long other_index = unit->weapon_indices[i];

				if (other_index != NONE && other_index != weapon_index)
				{
					s_weapon *other = ((s_weapon_header *)pool->data)[other_index & 0xffff].weapon;
					if (other->definition_index == weapon->definition_index)
					{
						s_weapon_magazine *magazine = &other->magazines[magazine_index];
						long take = count - taken > magazine->rounds_unloaded ? magazine->rounds_unloaded : count - taken;

						if (take > 0)
						{
							magazine->rounds_unloaded = magazine->rounds_unloaded - (short)take;
							taken += take;
						}
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
	short rounds = (short)(magazine->rounds_loaded + reloaded);

	if (rounds > magazine_definition->rounds_loaded_maximum)
		rounds = magazine_definition->rounds_loaded_maximum;
	if (TEST_FIELD_BIT(((s_weapon_item_flags_view *)weapon)->flag3))
		function_100b80(magazine_index, weapon_index, rounds - magazine->rounds_loaded);
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

// @retail 0x102cc0
void function_102cc0(long weapon_index, short magazine_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	switch (weapon->magazines[magazine_index].state)
	{
	case 0:
	case 5:
		if ((byte)function_101e80(weapon_index))
		{
			function_1039a0(weapon_index, WEAPON_DEFINITION(weapon)->magazines[magazine_index].effect_48, NONE, 0.0f, 0.0f);
			function_1058b0(weapon_index, magazine_index + 3, false);
			function_105a80(6, weapon_index, magazine_index);
		}
		break;
	case 4:
		return;
	default:
		break;
	}
}

// @retail 0x102d60
void function_102d60(long weapon_index, short trigger_index)
{
	s_weapon_definition *definition = WEAPON_DEFINITION(WEAPON_GET(weapon_index));
	real duration = g_510c54->field_2_3 * definition->triggers[trigger_index].value_20;
	long ticks;
	__asm
	{
		fld duration
		fistp ticks
	}
	s_weapon *weapon = WEAPON_GET(weapon_index);
	weapon->triggers[trigger_index].state = 2;
	weapon->triggers[trigger_index].timer = (short)ticks;
	function_1058b0(weapon_index, trigger_index + 7, true);
	function_105fa0(weapon_index, 13);
}

// @retail 0x1018b0
void function_1018b0(long weapon_index, short const *rounds)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	for (short i = 0; i < definition->magazine_count; i++)
	{
		s_weapon_magazine *magazine = &weapon->magazines[i];
		s_weapon_magazine_definition *magazine_definition = &definition->magazines[i];
		magazine->rounds_loaded = magazine->rounds_loaded > rounds[i] ? rounds[i] : magazine->rounds_loaded;
		long unloaded = rounds[i] - magazine->rounds_loaded;
		magazine->rounds_unloaded = (short)(unloaded > magazine_definition->rounds_total_maximum ? magazine_definition->rounds_total_maximum : unloaded);
	}
	function_a7cd0(weapon_index);
	function_b7360(weapon_index);
}

// @retail 0x101a10
void function_101a10(long weapon_index, real fraction)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	bool heat = function_101d20(weapon_index);
	fraction = fraction < 0.0f ? 0.0f : (fraction > 1.0f ? 1.0f : fraction);
	if (heat)
		weapon->heat = 1.0f - fraction;
	else if (definition->magazine_count > 0)
	{
		s_weapon_magazine_definition *magazine_definition = &definition->magazines[0];
		real count = magazine_definition->rounds_loaded_maximum * fraction;
		long loaded;
		__asm
		{
			fld count
			fistp loaded
		}
		short unloaded = weapon->magazines[0].rounds_unloaded - (short)loaded + weapon->magazines[0].rounds_loaded;
		weapon->magazines[0].rounds_unloaded = unloaded < 0 ? 0 : (unloaded > magazine_definition->rounds_total_maximum ? magazine_definition->rounds_total_maximum : unloaded);
		weapon->magazines[0].rounds_loaded = (short)loaded;
	}
	function_b7360(weapon_index);
	function_a7cd0(weapon_index);
}

// @retail 0x105e80
bool function_105e80(volatile long weapon_index)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	bool result = false;
	if (weapon->value250 != NONE && weapon->animation_254.index != NONE)
	{
		s_animation_state state;
		if (state.initialize(weapon->value250, NONE, true))
		{
			c_animation_channel channel;
			if (state.channel_play(&channel, weapon->animation_254, 0x4201))
			{
				channel.set_frame_position(weapon->value258 * 30.0f);
				state.channel_update(&channel, (animation_event_callback)function_105dd0, weapon_index);
				weapon->value258 = channel.frame_position * (1.0f / 30.0f);
				result = !((bool)((channel.unknown11 >> 3) & 1));
			}
		}
		if (!result)
			function_105be0(weapon_index);
		state.channels_clear_partial();
	}
	return result;
}

// @retail 0x1055b0
void function_1055b0(vector3f *direction, vector3f const *axis, short index, short mode, real spacing, short flags)
{
	real offset;
	if (flags & 1)
	{
		if (!index)
			offset = 0.0f;
		else
		{
			short value = index - 1;

			if (value & 1)
				value >>= 1;
			else
				value = -(value >> 1);
			offset = (real)value;
		}
	}
	else
	{
		offset = (real)(index >> 1) - 0.5f;
		if (index & 1)
			offset = -offset;
	}
	real cosine = (real)cos(offset * spacing);
	real sine = (real)sin(offset * spacing);
	switch (mode)
	{
	case 1:
	{
		real dot = (1.0f - cosine) * dot3f(direction, axis);
		real cross_i = direction->j * axis->k - direction->k * axis->j;
		real cross_j = direction->k * axis->i - direction->i * axis->k;
		real cross_k = direction->i * axis->j - direction->j * axis->i;

		direction->i = direction->i * cosine + axis->i * dot - cross_i * sine;
		direction->j = direction->j * cosine + axis->j * dot - cross_j * sine;
		direction->k = direction->k * cosine + axis->k * dot - cross_k * sine;
	}
	}
}

#include "slot_handler.h"
static __forceinline long real_to_long(real value);

/* counts how long a trigger has been held and reports a press or a hold
   once it has been held for the given ticks */
PRIVATE __forceinline void weapon_trigger_hold(s_weapon *weapon, long ticks, long threshold, bool *pressed, bool *held)
{
	long delta = weapon->value_170 - weapon->value_172;
	weapon->value_174 = (char)MIN(weapon->value_174 + 1, 255);
	if (weapon->value_174 >= ticks)
	{
		if (delta < threshold && weapon->value_170 < 250)
			*pressed = true;
		else
			*held = true;
	}
}

/* updates a trigger's press/hold/release state from its press counters */
// @retail 0x1027b0
void function_1027b0(long weapon_index, long trigger_index, bool *pressed, bool *held, bool *released)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_trigger *trigger = &weapon->triggers[trigger_index];
	s_weapon_trigger_definition *definition = &WEAPON_DEFINITION(weapon)->triggers[trigger_index];
	real duration = g_510c54->field_2_3 * definition->value_10;
	long ticks = real_to_long(duration);
	long threshold = (byte)(long)(definition->value_14 * 255.0f);
	*pressed = false;
	*held = false;
	*released = false;
	if (weapon->value_170 < weapon->value_171)
	{
		if (weapon->value_173 == 1)
		{
			if (weapon->value_171 - weapon->value_172 > 20)
				*pressed = true;
			weapon->value_173 = -1;
			weapon->value_172 = weapon->value_171;
			weapon->value_174 = 0;
		}
	}
	else if (weapon->value_170 == weapon->value_171)
	{
		if (weapon->value_173 == 1)
			weapon_trigger_hold(weapon, ticks, threshold, pressed, held);
	}
	else if (weapon->value_173 == -1)
	{
		if (weapon->value_172 - weapon->value_171 > 20)
			*released = true;
		weapon->value_172 = weapon->value_171;
		weapon->value_173 = 1;
		weapon->value_174 = 0;
	}
	else if (!weapon->value_173)
	{
		weapon->value_172 = 0;
		weapon->value_173 = 1;
		weapon->value_174 = 0;
	}
	else
		weapon_trigger_hold(weapon, ticks, threshold, pressed, held);
	if (!weapon->value_170 && !*pressed && !*held)
		*released = true;
	else if (!*released)
	{
		if (*pressed && ((*(dword *)((byte *)trigger + 4)) & 0x18))
			*pressed = false;
		if (!*released && *held)
		{
			if (((*(dword *)((byte *)trigger + 4)) & 0x10) ||
				((*(byte *)definition & 1) && ((*(dword *)((byte *)trigger + 4)) & 8)))
				*held = false;
		}
	}
	weapon->value_171 = weapon->value_170;
	weapon->value_170 = 0;
}

point3f *function_b9dd0(long object_index, point3f *result);
bool function_11c120(s_location const *location, point3f const *point, short *zone_index);
real function_d1210(long object_index);

struct s_weapon_bits_102540
{
	byte unknown000[0x10a];
	word : 2;
	word flag10a_2 : 1;
	word : 13;
	byte unknown10c[0x16c - 0x10c];
	word : 1;
	word flag16c_1 : 1;
	word : 14;
};

// @retail 0x102540
bool function_102540(long weapon_index, short barrel_index, bool ignore_magazine_state)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	volatile bool result = false;
	if (barrel_index >= 0 && barrel_index < definition->barrel_count)
	{
		s_weapon_barrel *barrel = &weapon->barrels[barrel_index];
		s_weapon_barrel_definition *barrel_definition = &definition->barrels[barrel_index];
		long unit_index = NONE;
		if (weapon->item_flags & 1)
			unit_index = weapon->unit_index;
		if (TEST_FIELD_BIT(definition->flag11) && weapon->heat >= 1.0f && barrel->flag6 && !function_106030(weapon_index))
			goto done;
		if (TEST_FIELD_BIT(definition->flag25) && weapon->parent_index != NONE &&
			TEST_FIELD_BIT(((s_weapon_bits_102540 *)WEAPON_GET(weapon->parent_index))->flag10a_2))
			goto done;
		word flags = *(word *)((byte *)barrel + 4);
		if ((flags & 4) || barrel->state == 2)
			goto done;
		if (TEST_FIELD_BIT(definition->flag19) && unit_index != NONE &&
			((1 << g_4e0300->data[(unit_index & 0xffff) * 12 + 3]) & 2) && function_d1210(unit_index) > 0.0f)
			goto done;
		if (TEST_FIELD_BIT(((s_weapon_bits_102540 *)weapon)->flag16c_1))
			goto done;
		if (definition->trigger_count == 1 && definition->barrel_count == 2 && definition->triggers[0].behavior == 5 &&
			weapon->barrels[barrel_index == 0].state == 2)
			goto done;
		short magazine_index = barrel_definition->magazine_index;
		if (magazine_index != NONE)
		{
			s_weapon_magazine *magazine = &weapon->magazines[magazine_index];
			if (magazine->rounds_loaded < barrel_definition->rounds_minimum && !TEST_FIELD_BIT(barrel_definition->flag2) &&
				(definition->reload_style == 1 || (flags & 0x40)))
				goto done;
			if (magazine->rounds_loaded < barrel_definition->rounds_per_shot && (flags & 0x40))
				goto done;
			if (magazine->state && !ignore_magazine_state)
			{
				if (definition->reload_style != 1)
					goto done;
				function_105fa0(weapon_index, 0);
				function_105a80(5, weapon_index, magazine_index);
			}
		}
		point3f point;
		result = true;
		if (function_11c120((s_location *)((byte *)weapon + 0x28), function_b9dd0(weapon_index, &point), NULL))
			result = false;
	done:;
	}
	return result;
}

#include <string.h>

struct s_weapon_status_magazine
{
	bool active;
	bool idle;
	short loaded;
	short loaded_maximum;
	short unloaded;
	short total_maximum;
};

struct s_weapon_status
{
	real value;
	real heat;
	bool flag8;
	bool flag9;
	byte unknown0a[2];
	real fraction;
	bool charging;
	bool target_available;
	bool target_charging;
	bool target_ready;
	real target_fraction;
	point3f target_position;
	short magazine_count;
	s_weapon_status_magazine magazines[2];
};

struct s_weapon_target_marker
{
	long name;
	byte unknown04[0x1c - 4];
};

struct s_weapon_target_model
{
	byte unknown00[0x68];
	long marker_count;
	s_weapon_target_marker *markers;
};

bool function_106280(long object_index, long *out_index, byte *out_entry);

// score 83 (was 130): the loop now reads the pool state before the magazine definition
// pointer, which matches retail's frame, prologue and loop register roles (definition in edx,
// entry weapon in ebx, magazine pointer in edi); the entry weapon and definition still swap their
// spill slots ([esp+0x18]/[esp+0x1c]), the scaled weapon index stays in esi instead of the stack
// slot retail reloads it from, and the target-position copy uses edx/eax/ecx where retail uses
// ecx/edx/eax.
// score 130 (was 130): frame 0x88 versus 0x84 and current-object spill remain; loop register
// roles and the post-loop scratch registers (ours eax/ecx/edx where retail uses edx/eax/ecx) differ.
// Retail tests `idle` through the entry weapon pointer (ebx), not the reloaded one: that form
// alone gives retail's 0x84 frame and prologue but 171, as the loop reloads its index offset at
// the latch where retail reloads it at the loop head (and again after the loop).
// Tried: current magazine view 180; unsigned active range 207; marker position memcpy 130;
// signed trigger state 130; charging result lifetime 129; settings-record reference, query-result
// and elapsed-fraction locals 130; native output initialization 218; promoted target state 128
// (rejected); true-default active result alone/with settings reference 194; settings reference
// plus final receiver 130; query output names 130; output record reference 219; current receiver
// first 132; nested range tests 130; staged active result 206; natural while 130; read-only tags
// 130; float consistency 130; object/header getters 130/130; entry weapon/tag references 130/130;
// output magazine reference 219; byte/long active carriers 192/194; target-model reference 130;
// one-use marker identifier local 125 (reverted in review); halfword active 169; signed-byte
// active 192; loaded/total/both capacity values 130/130/130; target query/status noinline 130/130;
// idle through the entry weapon 171; with the magazine pointer declared first 171; with the
// magazine fields read in full 225; with a short state 170; with new locals for the final reread 171.
// w1D (11-05): walked against retail (field offsets, magazine entries at 10-byte stride, trigger and target
// paths, charging test, marker copy): no logic difference. One byte shared by the loop's active flag and the
// target query's entry 178; magazine pointer declared before the state read 171.
// Permuter, 25 minutes (11-05): 79 (full image 78) only with function_b8d30's first and last arguments swapped; not kept.
// @retail 0x100520
void function_100520(long weapon_index, s_weapon_status *status)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	memset(status, 0, sizeof(*status));
	status->value = *(real *)((byte *)weapon + 0x180);
	status->heat = weapon->heat;
	status->fraction = (real)*((byte *)weapon + 0x175) * (1.0f / 255.0f);
	status->flag8 = (bool)((*((byte *)weapon + 0x16c) >> 1) & 1);
	status->flag9 = (bool)((((byte)weapon->value_16e) >> 1) & 1);
	status->magazine_count = (short)definition->magazine_count;
	for (short i = 0; i < definition->magazine_count; i++)
	{
		bool active = false;
		long state = WEAPON_GET(weapon_index)->magazines[i].state;
		if (state > 0 && state <= 6)
			active = true;
		s_weapon_magazine_definition *magazine = &definition->magazines[i];
		status->magazines[i].active = active;
		status->magazines[i].idle = weapon->magazines[i].state == 0;
		status->magazines[i].loaded = WEAPON_GET(weapon_index)->magazines[i].rounds_loaded;
		status->magazines[i].loaded_maximum = magazine->rounds_loaded_maximum;
		status->magazines[i].unloaded = (short)function_1008f0(i, weapon_index, false);
		status->magazines[i].total_maximum = magazine->rounds_total_maximum;
	}
	if (definition->trigger_count > 0 && definition->triggers[0].behavior == 5 && weapon->magazines[0].rounds_loaded > 0)
	{
		byte state = weapon->triggers[0].state;
		if (!state)
		{
			long index;
			byte entry;
			if (function_106280(weapon_index, &index, &entry))
				status->target_available = true;
		}
		else if (state == 5 || state == 6)
		{
			long index = weapon->object_index_194;
			if (index != NONE)
			{
				s_weapon *object = (s_weapon *)function_badc0(index, NONE);
				if (object && weapon->entry_177 != NONE)
				{
					long model_index = WEAPON_DEFINITION(object)->model_index;
					if (model_index != NONE)
					{
						s_weapon_target_model *model = (s_weapon_target_model *)g_4e3b44[model_index & 0xffff].bytes;
						char entry = weapon->entry_177;
						if (entry >= 0 && entry < model->marker_count)
						{
							s_object_marker marker;
							if (function_b8d30(index, model->markers[entry].name, &marker, 1, false) == 1)
								status->target_position = marker.matrix.position;
						}
					}
				}
			}
			if (weapon->triggers[0].state == 5)
			{
				real duration = g_510c54->field_2_3 * 0.5f;
				long ticks = real_to_long(duration);
				status->target_charging = true;
				status->target_fraction = 1.0f - (real)weapon->triggers[0].timer / ticks;
			}
			else
				status->target_ready = true;
		}
	}
	weapon = WEAPON_GET(weapon_index);
	definition = WEAPON_DEFINITION(weapon);
	if (definition->trigger_count > 0)
	{
		s_weapon_trigger_definition *trigger = definition->triggers;
		if (weapon->triggers[0].state == 2 && trigger->behavior == 2 && *(short *)((byte *)trigger + 0x18) == 1)
			status->charging = true;
	}
}

extern bool g_4f55dc[16];
void __stdcall function_b9b90(long object_index, bool disable);

struct s_scenario_weapon_view
{
	byte unknown00[0x4c];
	short rounds_total;
	short rounds_loaded;
	struct
	{
		dword at_rest : 1;
		dword unknown1 : 1;
		dword accelerates : 1;
		dword unknown3 : 29;
	} flags;
};

// @retail 0xfda00
long __stdcall function_fda00(long weapon_index, s_scenario_weapon_view *placement)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	if (definition->magazine_count > 0)
	{
		short loaded = placement->rounds_loaded;
		short total = placement->rounds_total;
		if (g_4e6948->state == 1 && g_4f55dc[4])
		{
			loaded /= 2;
			total /= 2;
		}
		function_101980(weapon_index, loaded, 0, total);
	}
	function_b9b90(weapon_index, TEST_FIELD_BIT(placement->flags.at_rest));
	dword *flags = (dword *)((byte *)weapon + 4);
	*flags |= 0x8000;
	if (!TEST_FIELD_BIT(placement->flags.accelerates))
		((s_weapon_item_flags_view *)weapon)->flag7 = true;
	else
		((s_weapon_item_flags_view *)weapon)->flag7 = false;
	if (!TEST_FIELD_BIT(placement->flags.at_rest))
		*(real *)((byte *)weapon + 0x6c) += 0.05f;
	return weapon_index;
}

// @retail 0xfdad0
bool __stdcall function_fdad0(long weapon_index, void *placement, bool *flag)
{
    s_weapon *weapon = WEAPON_GET(weapon_index);
    s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
    weapon->state = 0;
    weapon->object_index_194 = NONE;
    *(long *)((byte *)weapon + 0x244) = NONE;
    *(real *)((byte *)weapon + 0x190) = 0.0f;
    *(real *)((byte *)weapon + 0x18c) = 0.0f;
    function_1058b0(weapon_index, 0, true);
    weapon->object_index_194 = NONE;
    weapon->entry_177 = NONE;
    weapon->time_24c = 0;
    weapon->value_173 = 0;
    weapon->value_174 = 0;
    weapon->value_172 = 0;
    weapon->value_171 = 0;
    short magazine_index;
    short i;
    for (magazine_index = 0; magazine_index < definition->magazine_count; magazine_index++)
    {
        s_weapon_magazine *magazine = &weapon->magazines[magazine_index];
        s_weapon_magazine_definition *magazine_definition = &definition->magazines[magazine_index];
        magazine->rounds_loaded = *(short *)((byte *)magazine_definition + 6) > magazine_definition->rounds_loaded_maximum ? magazine_definition->rounds_loaded_maximum : *(short *)((byte *)magazine_definition + 6);
        long remaining = *(short *)((byte *)magazine_definition + 6) - magazine->rounds_loaded;
        magazine->rounds_unloaded = remaining > magazine_definition->rounds_total_maximum ? magazine_definition->rounds_total_maximum : remaining;
    }
    for (i = 0; i < definition->trigger_count; i++)
        weapon->triggers[i].effect_index = NONE;
    for (i = 0; i < definition->barrel_count; i++)
    {
        s_weapon_barrel *barrel = &weapon->barrels[i];
        barrel->timer = 0x7f;
        *(long *)((byte *)barrel + 0x30) = NONE;
    }
    *(short *)((byte *)weapon + 0x17e) = NONE;
    function_105be0(weapon_index);
    return true;
}

// @retail 0xfdc70
bool __stdcall function_fdc70(long weapon_index, short magazine_index)
{
    (void)&weapon_index;
    (void)&magazine_index;
    long unit_index = NONE;
    s_record_pool *objects = g_4e0300;
    byte *volatile *headers = &objects->data;
    s_weapon *weapon = WEAPON_GET(weapon_index);
    if (TEST_FIELD_BIT(weapon->in_inventory))
        unit_index = weapon->unit_index;
    bool result = false;
    if (unit_index != NONE)
    {
        s_weapon_unit *unit = ((s_weapon_unit_header *)*headers)[unit_index & 0xffff].unit;
        short current_slot = unit->current_weapon_slot;
        long current_weapon = NONE;
        if (current_slot != NONE)
            current_weapon = unit->weapon_indices[current_slot];
        bool current = weapon_index == current_weapon;
        unit = ((s_weapon_unit_header *)*headers)[unit_index & 0xffff].unit;
        short other_slot = unit->other_weapon_slot;
        long other_weapon = NONE;
        if (other_slot != NONE)
            other_weapon = unit->weapon_indices[other_slot];
        if (current || weapon_index == other_weapon)
        {
            if (magazine_index == 0)
                result = function_e68c0(current ? 0 : 10, unit_index);
            if (magazine_index == 1)
                result = function_e68c0(current ? 1 : 11, unit_index);
        }
    }
    return result;
}

/* the weapon's flag word at 0x16e, as bits */
struct s_weapon_flags_view_1023d0
{
    word flag0 : 1;
    word flag1 : 1;
    word flag2 : 1;
    word flag3 : 1;
    word flag4 : 1;
    word flag5 : 1;
    word : 10;
};

// @retail 0xfdd50
bool __stdcall function_fdd50(long weapon_index, long magazine_index)
{
    s_weapon *weapon = WEAPON_GET(weapon_index);
    s_weapon_magazine *magazine = &weapon->magazines[magazine_index];
    s_weapon_magazine_definition *definition = &WEAPON_DEFINITION(weapon)->magazines[magazine_index];
    bool result = false;
    if (*(short *)((byte *)definition + 4) > 0 && magazine->rounds_loaded < definition->rounds_loaded_maximum)
    {
        long recharge_divisor = g_510c54->field_2_3;
        long rounds = *(short *)((byte *)definition + 4) / recharge_divisor;
        long remainder = *(short *)((byte *)definition + 4) % recharge_divisor;
        magazine->rounds_loaded += (short)rounds;
        magazine->rounds_fraction += (short)remainder;
        if (magazine->rounds_fraction >= g_510c54->field_2_3)
        {
            magazine->rounds_loaded++;
            magazine->rounds_fraction -= g_510c54->field_2_3;
        }
        if (magazine->rounds_loaded > definition->rounds_loaded_maximum)
            magazine->rounds_loaded = definition->rounds_loaded_maximum;
        function_a7cd0(weapon_index);
        result = true;
    }
    if (magazine->ticks > 0)
    {
        magazine->ticks--;
        result = true;
    }
    if (magazine->ticks_0c > 0)
    {
        magazine->ticks_0c--;
        result = true;
    }
    if (magazine->state != 0)
        result = true;
    switch (magazine->state)
    {
    case 1:
    case 2:
    case 3:
        if (magazine->ticks_0c > magazine->ticks)
            magazine->ticks_0c = magazine->ticks;
        if (magazine->ticks_0c == 0)
            function_102b90(weapon_index, (short)magazine_index);
        if (magazine->ticks == 0)
        {
            long reserve = function_1008f0(magazine_index, weapon_index, false);
            if (magazine->state == 1)
                function_105a80(0, weapon_index, (short)magazine_index);
            else
                function_105a80(4, weapon_index, (short)magazine_index);
            bool first = TEST_FIELD_BIT(((s_weapon_flags_view_1023d0 const *)&weapon->value_16e)->flag1) && function_102540(weapon_index, 0, true);
            bool second = TEST_FIELD_BIT(((s_weapon_flags_view_1023d0 const *)&weapon->value_16e)->flag2) && function_102540(weapon_index, 1, true);
            if (!(reserve > 0 && magazine->rounds_loaded < definition->rounds_loaded_maximum &&
                !(definition->flags & 1) && !first && !second && function_fdc70(weapon_index, (short)magazine_index)))
                function_102c60(weapon_index, (short)magazine_index, first | second);
        }
        break;
    case 4:
        break;
    case 5:
        function_102cc0(weapon_index, (short)magazine_index);
        break;
    case 6:
        if (magazine->ticks == 0)
            function_105a80(0, weapon_index, (short)magazine_index);
        break;
    default:
        break;
    }
    return result;
}

struct s_object_list;
extern s_object_list *g_4de2f4;
real function_102070(long object_index, short slot_index);
bool function_10cf50(long item_index);
bool function_162b10(long object_index);
real function_162b70(long object_index);
bool __stdcall function_fdfb0(long weapon_index);

/* the weapon's value for a function name, clamped to [0, 1], and whether it is above zero */
// @retail 0xff5f0
bool __stdcall function_ff5f0(long weapon_index, long name, real *value, bool *active)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	real result = 0.0f;
	long index;
	switch (name)
	{
	case 0x1d0007ba:
		/* the 0x0c000569 value unless the list's byte at 0x82 is set: retail loops back to the top with that
		   name (the path to this case falls through the tree, and the definition is looked up again) */
		if (((byte *)g_4de2f4)[0x82])
			result = 0.0f;
		else
			return function_ff5f0(weapon_index, 0x0c000569, value, active);
		break;
	case 0x0c000569:
	{
		long i;
		for (i = 0; i < definition->barrel_count; i++)
			result = result > *(real *)((byte *)&weapon->barrels[i] + 0x18) ? result : *(real *)((byte *)&weapon->barrels[i] + 0x18);
		for (i = 0; i < definition->trigger_count; i++)
		{
			s_weapon_trigger_definition *trigger_entry = &definition->triggers[i];
			if (trigger_entry->value_1c > 0.0f)
			{
				real candidate = function_102070(weapon_index, (short)i) * *(real *)((byte *)trigger_entry + 0x28);
				result = result > candidate ? result : candidate;
			}
			if (weapon->triggers[i].state == 2)
			{
				real candidate = *(real *)((byte *)trigger_entry + 0x28) +
					(1.0f - *(real *)((byte *)trigger_entry + 0x28)) * *(real *)((byte *)weapon + 0x188);
				result = result > candidate ? result : candidate;
			}
		}
		real candidate = *(real *)((byte *)weapon + 0x180) * *(real *)((byte *)definition + 0x164);
		if (!(result > candidate)) result = candidate;
		break;
	}
	case 0x03000577: result = weapon->heat; break;
	case 0x04000567: result = *(real *)((byte *)weapon + 0x180); break;
	case 0x05000024:
		if (function_10cf50(weapon_index))
			result = 1.0f;
		break;
	case 0x05000081: result = *(real *)((byte *)weapon + 0x18c); break;
	case 0x09000568:
		if (!function_10cf50(weapon_index) || ((bool)(((dword)*(word *)((byte *)weapon + 0x16c) >> 5) & 1)))
		{
			if (weapon->heat < 1.0f)
				result = 1.0f;
		}
		break;
	case 0x0a000066:
		if (((bool)(((dword)*(word *)((byte *)weapon + 0x16c) >> 1) & 1)) && *(real *)((byte *)definition + 0x150) != 1.0f)
			result = (*(real *)((byte *)weapon + 0x180) - *(real *)((byte *)definition + 0x150)) /
				(1.0f - *(real *)((byte *)definition + 0x150));
		break;
	case 0x0b0005a9: result = (real)((byte *)weapon)[0x175] * (1.0f / 255.0f); break;
	case 0x0d0006e7:
	case 0x0e0006e6:
	case 0x0f0006e5:
		result = 0.0f; /* retail zeroes the result again here and in the next case */
		if (g_4e6948->state == 2 && *(short *)((byte *)weapon + 0x17e) != NONE && function_162b10(weapon_index))
		{
			real progress = function_162b70(weapon_index);
			if (name == 0x0f0006e5)
			{
				if (progress == 0.0f)
					result = 1.0f;
			}
			else if (name == 0x0e0006e6)
			{
				if (progress > 0.0f && progress < 1.0f)
					result = 1.0f;
			}
			else if (name == 0x0d0006e7)
			{
				if (progress == 1.0f)
					result = 1.0f;
			}
		}
		break;
	case 0x120006e4:
		result = 0.0f;
		if (g_4e6948->state == 2 && *(short *)((byte *)weapon + 0x17e) != NONE && function_162b10(weapon_index))
			result = function_162b70(weapon_index);
		break;
	case 0x0e000572:
	case 0x10000573:
		index = name == 0x0e000572 ? 0 : 1;
		if (index < definition->barrel_count)
		{
			result = *(real *)((byte *)&weapon->barrels[index] + 0x10);
			if ((long)(g_510c54->game_time - *(long *)((byte *)weapon + 0x248)) > 1)
				result = 0.0f;
		}
		break;
	case 0x0f000574:
	case 0x11000575:
		index = name == 0x0f000574 ? 0 : 1;
		if (index < definition->trigger_count)
			result = function_102070(weapon_index, (short)index);
		break;
	case 0x1200056a:
	case 0x1400056d:
		index = name == 0x1200056a ? 0 : 1;
		if (index < definition->magazine_count)
		{
			s_weapon_magazine_definition *magazine_definition = &definition->magazines[index];
			if (magazine_definition->rounds_loaded_maximum != 0)
				result = (real)weapon->magazines[index].rounds_loaded / (real)magazine_definition->rounds_loaded_maximum;
		}
		break;
	case 0x14000570:
	case 0x16000571:
		index = name == 0x14000570 ? 0 : 1;
		if (index < definition->barrel_count)
			result = *(real *)((byte *)&weapon->barrels[index] + 0x10);
		break;
	case 0x1500056e:
	case 0x1700056f:
		index = name == 0x1500056e ? 0 : 1;
		if (index < definition->barrel_count)
			result = weapon->barrels[index].value14;
		break;
	case 0x14000578:
		if (TEST_FIELD_BIT(((s_weapon_flags_view_1023d0 const *)&weapon->value_16e)->flag1))
			result = 1.0f;
		break;
	case 0x16000579:
		if (TEST_FIELD_BIT(((s_weapon_flags_view_1023d0 const *)&weapon->value_16e)->flag2))
			result = 1.0f;
		break;
	case 0x1700056b:
		if (definition->magazine_count > 0)
			result = (real)(weapon->magazines[0].rounds_loaded % 10) * 0.1f;
		break;
	case 0x1700056c:
		if (definition->magazine_count > 0)
			result = (real)((weapon->magazines[0].rounds_loaded / 10) % 10) * 0.1f;
		break;
	default:
		return false;
	}
	if (result < 0.0f)
		result = 0.0f;
	else if (result > 1.0f)
		result = 1.0f;
	*value = result;
	*active = result > 0.0f;
	return true;
}

/* Prefix of the retail weapon type definition through its export callback. */
struct s_weapon_type_definition_view
{
	char const *name;
	dword group_tag;
	short datum_size;
	short unknown0a;
	short unknown0c;
	short scenario_size;
	void *unknown10[7];
	bool (__stdcall *create)(long, void *, bool *);
	long (__stdcall *place)(long, s_scenario_weapon_view *);
	void *unknown34[3];
	bool (__stdcall *update)(long);
	void *unknown44[2];
	bool (__stdcall *export_value)(long, long, real *, bool *);
};

s_weapon_type_definition_view g_467cd0 =
{
	"weapon", 'weap', 0x25c, 0x90, 0x98, 0x54,
	{ NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	function_fdad0, function_fda00,
	{ NULL, NULL, NULL }, function_fdfb0, { NULL, NULL }, function_ff5f0
};


// @retail 0x1023d0
bool function_1023d0(long weapon_index, long barrel_index)
{
    s_weapon *weapon = WEAPON_GET(weapon_index);
    s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
    s_weapon_barrel_definition *barrel_definition = &definition->barrels[barrel_index];
    bool result = false;
    if (barrel_definition->magazine_index != NONE)
    {
        s_weapon_magazine_definition *magazine_definition = &definition->magazines[barrel_definition->magazine_index];
        s_weapon_magazine *magazine = &weapon->magazines[barrel_definition->magazine_index];
        if ((magazine->rounds_loaded < barrel_definition->rounds_minimum && !TEST_FIELD_BIT(barrel_definition->flag2)) ||
            magazine->rounds_loaded < barrel_definition->rounds_per_shot || magazine->rounds_loaded == 0)
        {
            long magazine_index = barrel_definition->magazine_index;
            if (function_1008f0(magazine_index, weapon_index, false) > 0 &&
                magazine->rounds_loaded < magazine_definition->rounds_loaded_maximum)
            {
                if (!TEST_FIELD_BIT(((s_weapon_flags_view_1023d0 const *)&weapon->value_16e)->flag5) && weapon->parent_index != NONE)
                    function_fdc70(weapon_index, barrel_definition->magazine_index);
                result = true;
            }
            else
            {
                for (long i = 0; i < definition->trigger_count; ++i)
                {
                    s_weapon *current = WEAPON_GET(weapon_index);
                    current->triggers[(short)i].state = 0;
                    current->triggers[(short)i].timer = 0;
                }
                for (long i = 0; i < definition->barrel_count; ++i)
                    if (weapon->barrels[i].state == 1)
                        function_103e60(weapon_index, (short)i);
            }
        }
    }
    return result;
}


void __stdcall function_b8540(long object_index);

// @retail 0x104030
void function_104030(long weapon_index)
{
    if (g_4e6948->mode != 4)
    {
        s_weapon *weapon = WEAPON_GET(weapon_index);
        s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
        function_1039a0(weapon_index, *(long *)((byte *)definition + 0x180), NONE, 0.0f, 0.0f);
        function_b8540(weapon_index);
    }
}

// @retail 0x102f10
void function_102f10(long weapon_index, short trigger_index)
{
    s_weapon *weapon = WEAPON_GET(weapon_index);
    s_weapon_trigger_definition *trigger = &WEAPON_DEFINITION(weapon)->triggers[trigger_index];
    switch (*(short *)((byte *)trigger + 0x24))
    {
    case 1: function_104030(weapon_index); break;
    case 2: function_102e10(weapon_index, trigger_index); break;
    }
}


void function_b7360(long object_index);
void function_b58c0(long index, dword mask);
long function_1896c0(real scale, long sound_index);
void function_a88a0(long player_index, long first_definition, long second_definition, long count);

struct s_ammo_conversion_entry
{
    short count;
    byte unknown02[6];
    long definition_index;
};

// score 190 (was 190): Frame 0x20 agrees; destination/tag homes and conversion staging remain.
// MiniMax A (11-18, run 94): no new form, no lower score (190): the permuter's annealed run (300 s) again stops at
// 190 with the prologue's saved registers as its first difference (retail saves ebx, ebp, esi and holds the
// destination weapon in ebp and the other weapon's definition index in edi, ours saves ebx, esi, edi, holds the
// destination in edi and spills the other); the acquisition flag in retail's sense gives its `setg`, its `mov byte,
// 1` and its `mov al` tail but measures 202, the type byte read again at each of its three tests 212, the
// transfer's second minimum as an int 191, the type byte read again with that minimum as an int 215, the
// destination's definition index in a local inside the available block 190 and at the top of the loop 210, the
// conversion scan without the entry's null test 199, and the sum's two terms swapped 190 (which only moves that
// null test); the body stands.
// MiniMax B (11-18): no new form, no lower score (190): the conversion scan written as a while loop with the count in
// its condition measures 198, and the permuter's annealed run (900 s) over this body finds nothing below 190, so the
// conversion entry keeps its null test and the destination weapon stays in edi where retail holds it in ebp.
// MiniMax I (11-17, run 103): no new form, no lower score (190): retail still holds the destination weapon in EBP and
// stores the acquired flag with setg, where this build holds the destination in EDI and stores the not-acquired flag
// with setle, and the conversion scan still needs its entry tested for null; the entry kept by its index measures 194
// and the acquisition flag with retail's polarity 211, so the body stands.
// MiniMax I (11-17): no new form, no lower score (190): the conversion scan still needs its entry tested for null, and the
// destination weapon's register and the acquisition flag's sense still differ (this build holds the destination in edi and
// stores the not-acquired flag with `setle`, retail holds it in ebp and stores the acquired flag with `setg`); the forms
// measured around it (a found flag 206, the entry kept by its index 194, clearing the available count when the scan finds
// nothing 206, the scan counter as a long 210) all rise, so the body stands;
// MiniMax S (11-16): no new form, no lower score (190): the conversion scan still needs its entry tested for null - using the scan counter for the entry with a local found flag scores 206, keeping the entry by its index 194, clearing the available count when the scan finds nothing 206, and the pointer form with the skip as the else of the found test stays 190; the scan counter as a long instead of a short gives 210, and the destination weapon's register and the acquisition flag's sense still differ.
// MiniMax G (11-14): no new form, no lower score (190): the type byte re-read at each of its three tests 212, the acquisition flag with retail's polarity 211 (715 bytes), the same with a found flag in the conversion scan 216, the destination magazine without its own pointer 207, both of those together 211 (743 bytes), the capacity minimum without its cast 190 (unchanged code, different allocation); a 300 s permuter run found nothing below 190.
// MiniMax B (11-12): no new form, no lower score (190): the difference is in the homes and the
// carriers only (retail holds the destination weapon in ebp and spills it around the conversion
// scan, keeps the other weapon's definition index in edi, and stores the acquisition flag with
// retail's polarity); flipping the flag's sense compiles worse, and the one lower permuter form
// swaps two calls' arguments, which contradicts their declarations.
// MiniMax G (11-10): no new form, no lower score (190).
// MiniMax A (11-04): no new form, no lower score; the acquisition flag with retail's polarity 202, the
// conversion path's flag before its minimum 205, the transfer's second minimum as a long 191.
// MiniMax H (11-03): no new form, no lower score; retail saves ebx and ebp and keeps the weapon in ebp where ours saves ebx, esi and edi and keeps it in edi, so the stack slots and the loop's registers differ.
// MiniMax I (11-14): no new form, no lower score (190): the destination weapon is in edi where retail has ebp and the loop
// index in edi where retail has eax, and that one-register swap shifts the whole stream; the conversion entry used in place
// (a goto out of the scan, so no null test) measures 206, that same body with retail's acquisition polarity and the
// transfer's two accumulators as long 214, the goto body as it stands 206, `int` for the loop index 278 (785 bytes), the
// transfer's accumulators as int 191 and the acquisition flag with retail's polarity 211; a 300 s permuter run found
// nothing below 190, and the frame, the length and every other function in this file are unchanged.
// MiniMax A (11-19) remained at 190. Retail opens with `push ebp` and keeps the
// definition pointer in EBP, without a frame pointer, where this build pushes esi and edi, keeps EBP as the frame
// pointer and leaves the definition pointer in EDI, and retail reads the definition table after the weapon lookup
// where this build reads it before, so the saved-register set and the destination home differ before the first
// case body; the frame size and the length still agree.
// MiniMax R (11-20): no new form, no lower score (190, 731 bytes, retail 731): the frame and the length agree, and what
// is left is the register colouring and one test - retail keeps the destination weapon in EBP and the other weapon in
// EBX, sets the acquisition flag with setg on the clamped count and reaches the conversion entry without a nullable
// test, while this build holds the destination in EDI, spills the other and tests the entry for null.
// Earlier measurement 190 (was 190): Frame 0x20 agrees; destination/tag homes and conversion staging remain.
// Earlier 190 (was 190): Frame 0x20 agrees; destination/tag homes and conversion staging remain.
// Earlier score 190 (was 190): Frame 0x20 agrees; destination/tag homes and conversion staging remain.
// Earlier: Frame 0x24 versus 0x20; destination/tag homes and conversion staging remain.
// Earlier: frame 0x24 versus 0x20; destination/tag saved roles and conversion staging differ.
// Frame 0x24 versus 0x20, destination/tag homes and conversion staging remain.
// Earlier: Frame 0x24 versus 0x20, destination/tag homes and conversion staging remain.
// Earlier: frame 0x24 versus 0x20; destination/tag saved roles and conversion staging differ.
// Fast long loaded 203, unloaded 204, retained-rounds flag 205, acquired flag 209; shared return 205.
// Earlier: frame agrees; destination/tag and search carriers remain.
// Wide truncated capacity 207; search-key snapshot 203; unloaded value clamp 203;
// direct retention predicate 213; loaded value clamp 203. Original body restored.
// Earlier: Frame 0x20 agrees; destination EDI/EBP and conversion staging remain.
// Conversion ordinal 203; same-definition carrier 224; destination object reference 203; magazine
// reference 203; source sum order 203; native conversion fields 203.
// Round 132: word acquisition result 198 plain kept (731/731 bytes), frame 0x24/0x20;
// destination/tag and conversion staging remain. Byte result/retention/both 200/203/200; word
// retention/both 207/199; result/both at entry 200/202; word/byte result with type rereads
// 238/215; word result/both at entry 225/226.
// Frame 0x20 agrees; destination EDI/EBP, source/tag allocation and conversion entry staging remain.
// Earlier: destination/tag saved roles and conversion-entry address staging differ.
// Earlier: frame agrees; destination/tag and search carriers remain.
// Earlier: Frame 0x20 agrees; destination EDI/EBP and conversion staging remain.
// Round 109: Entry-result/ordinal join 206; ordinal-only scan 219; remaining transfer accumulator 203; both objects before tags 215; retained-rounds truth accumulation 203 (731 bytes).
// frame 0x20 agrees; destination/tag roles and conversion-entry staging remain.
// Reusable magazine count 211; read-only tag receivers/result names/outer ordinal/conversion
// scope 203/203/203/203; result-conditioned search 202 (below gate); plain while 203;
// source-count store order 203. Opening body restored.
// Round 356: Unsigned-word acquisition 198; complete acquisition arms 198; early uninitialized tag declarations 198; table declaration before count 198; unsigned word with arms 198.
// Success-path return 209; source from each path/after conversion 204/219; conversion count/table/both at use 197/198/198; short current/successor 205; combined ownership 205. One-point gain rejected.
// Existing object getter for destination/source/both/all lookups 198/198/198/198;
// both objects before tags with word acquisition 208; source-first pairs 211;
// source then destination then tags 209; source-first shared getters 210.
// Round 318: actual unloaded/loaded/both source-count values 207/201/202;
// native source state 236; capacity arm 197 (gain 1, rejected); values/state with capacity arm
// 202/236; both values with Boolean acquisition 220. Frame and saved roles remain; all restored.
// Round 159: Native leading tag byte/conversion fields/both 198/198/198; native fields with machine-width
// loaded/unloaded/both transfers 198/205/205; native fields with common acquisition result 212 (740
// bytes); native kind at every use 236 (747 bytes). Original word acquisition, raw observations, transfer
// types and return flow restored.
// Early unloaded publication after/before remaining calculation 198/198; with candidate entry 217/217;
// early publication with conversion quantity 198; numeric home names 198; wide loaded quantity 198;
// numeric names/conversion quantity/earliest publication 198. Original body restored.
// Round 183 complementary acquisition word/byte/long 212/194/215; source depletion
// bool/byte/short 197/197/212; both word-plus-bool/bytes 215/200 (fast). Byte acquisition
// recovers frame 0x20 but gains only four, so all restored; destination/tag and conversion homes remain.
// Complementary byte acquisition with source counts/capacity branch/source scope/
// table at every use 210/193/207/195; source counts with capacity/source scope 210/210;
// twice-used offered count 194, with source scope 207. Capacity form has frame 0x20
// and retail length but gains only five; all combinations restored.
// State declaration order, acquisition/retention names, source declaration order, capacity and
// conversion comparisons 198 each; explicit return truth 202; source minimum branch 197.
// Destination round-field pointer with short/bool/byte/complementary-byte acquisition 201/218/213/210;
// with result-controlled first-match search 216/211/208/212. All restored; frame and saved homes remain.
// Complementary byte acquisition plus destination/source/tag/magazine/transfer
// and index home names, individually and together, 194 each (fast; four-point gains dropped).
// Round 337: actual published round total short/word/long 225/225/229;
// mode-test return producer 197 (gain 1, rejected); with short/word/long totals 231/231/235;
// short total and mode producer with complementary byte acquisition 208. All restored.
// Round 351: persistent tag/magazine/both/conversion/tag-conversion/magazine-conversion/all
// cursors 205/222/228/228/224/217/232; all cursors with Boolean acquisition 236. All restored.
// Complementary bool/byte/short/long acquisition with the tag byte at every type test
// 216/216/234/232; with result-controlled first-match scan 246/246/261/259. All restored.
// Round 204: Shared three-use type-bit result bool/byte/word/dword with word acquisition
// 218/215/214/221; with complementary byte acquisition 212/204/219/224. Original type
// observation and result restored; extra frame word, destination/tag and conversion homes remain.
// Round 196: Ordinary/inline/noinline post-transfer updater and retention-query declaration
// pairs, excluding the original pair, 198 each. Original declarations and body restored.
// Lane 10 r11: source transfer clamp computed in int (retail's movsx/sub/cmp/cmovle shape) 197;
// with bool acquisition 202 (718 bytes, frame 0x20); plus conversion work inside the search
// loop with an index test after it 202; found-flag search (retail's lea-before-je, no NULL test)
// 211; bool/int clamp with the tag byte read at each of its three uses 213 (731); that with the
// found flag 207; source sum operands swapped 207 (no change); short acquisition with clamp and
// rereads 237; short with clamp and found flag 216. Retail keeps the other weapon in EBX
// throughout and spills the weapon and loop index instead. Original body restored.
// Round 14: byte result 200 (frame 0x20); early conversion publication 206; candidate entry 210;
// declaration order, capacity, table at use and offered count 200 each; source scope 211;
// complementary early publication 199; complementary early publication with offered count 204.
// Original word result and body restored.
// Lane 10 r13: retail keeps the other weapon in EBX across the inner scan and spills the scan
// count instead (all seven registers busy there). Ternary retention result (retail's je/call/jmp/
// xor al,al and other index in EDI) 203, with bool acquisition 208; that with the type byte at
// each use 239; bool, ternary, int loaded clamp, type byte at each use and scan count/table read
// at use 215 (731 bytes, frame 0x20; the count still gets a register); same with count/table
// locals 215; short acquisition with those 240; short with int clamp and ternary 202.
// Lead: retail sign-extends the passed round total (movsx) where ours zero-extends it; 0xa88a0's
// count taken as long would give the movsx (its file was not edited this round).
// Round 404: native signed notification count with acquisition/retention/kind combinations
// 208/191/214/204/233/213/236/213; capacity updates both/source/conversion 190/190/191. Kept source minimum
// branch and complementary byte result with long notification formal; full 190 at 731/731, first +0x18, all
// matches and callee preserved. Seven-point 191 lead alone rejected.
// Round 406: Frame 0x20 agrees; destination/tag homes and conversion staging remain. Branch-owned
// source magazines with direct definition equality at transfer, complementary byte/bool and
// positive bool/word acquisition, each with cached or complete per-use type observations. Scores
// 231/231/226/235/248/248/224/242. All restored.
// Round 410: Complete destination/source/both object observations 243/228/300;
// source tag after transfers 213, with destination/source/both observations 243/221/317;
// complete graph with consistent type-byte reads 328. Original object/tag graph restored.
// Round 429: Ordinal-owned first match: post-search entry 194/218, candidate entry 207/207,
// in-search capacity 199/198, offered quantity 202/216 (once/live count). Original body
// restored.
// Real notification/updater ordinary, inline and noinline declaration pairs, excluding the
// original pair: 190 each. Both complete defining files, all declarations and the transfer body restored;
// destination/tag/search homes remain with the correct 0x20 frame and signed notification count.
// Round 87: retail keeps the weapon in EBP (spilled around the conversion scan) and the other weapon in EBX
// throughout; ours puts the weapon in EDI and spills the other. Positive bool/byte acquisition 211/202; type byte
// read at each of its three tests 212 (pushes all four saved registers up front); source pointer declared inside
// the available block 210 (715 bytes); positive result with per-use type reads 214.
// Round 43: positive byte acquisition 202; ordinal conversion scan on it 215; direct destination,
// both-element and definition-only observations 207/235/228; unsigned conversion quantity 204,
// with conditional minimum 196, plus direct destination 205. Original body and field type restored.
// Round 94: positive returned acquisition, found-flag conversion search and the type byte at each test,
// together 198 (731 bytes; `other` in EBP and spilled, the conversion count in a register where retail
// spills the count and keeps `other` in EBX); plus the entry pointer taken before the test 219; that with the
// count read in the loop condition 211; with the unloaded term read through the other weapon 230; the
// first form with the acquisition declared first 198.
// Round 319: Capacity-failure continue with cached count, conversion-first arm and
// twice-used offered quantity subsets 210/207/196/207/210/207/196/207. All
// restored; frame 0x20 agrees but destination/tag and conversion homes remain.
// Round 455: Actual persistent source header pointer/reference, with acquisition declaration priority
// and retained-rounds constant lifetime: 209/211/209/211/209/211/209/211, 713/720 bytes.
// Original lookup graph, declarations, transfer expressions and complete body restored.
// Walk against retail (w1C, 11-05): all blocks of 0xffc60 match retail's logic (fields, conditions, callees and
// arguments, loop bounds). What differs is only homes (retail weapon EBP spilled around the scan, other EBX, other's
// definition index EDI) and the conversion scan's shape: retail computes &entries[j] at the top of each pass and jumps
// from the match straight to the shared `available > 0` tail, with no test after the loop. Tried: entry pointer at the
// top of the pass with `if (j >= count) continue;` after the loop 207 (713 bytes); a goto from the match to the tail 214
// (712 bytes) (both fast, control 190).
// Permuter, 25 minutes (11-05): 182 only with a volatile other_has_rounds and the same-definition test inverted; not kept.
// MiniMax E (11-10): no lower score (190). Retail holds the destination weapon in EBP and spills it to
// [esp+0x14], keeps the other weapon's definition index in EDI across the conversion scan, and stores the
// acquisition flag the other way round (`setg` on "acquired", initialised to 0 in the prologue). Ours holds
// the destination in EDI, keeps the other definition index in EBX (the other weapon is spilled instead) and
// stores "not acquired" (`setle`). Flipping the flag to retail's sense compiles worse (202 at 720 bytes), so it
// was reverted; the frame also differs by one slot. A permuter over this body (10 tries, 302 s) found only one
// lower form, 186 against 190, and it swaps the arguments of two calls (function_1896c0's scale and tag, and
// function_a88a0's player index and definition indices), which contradicts those declarations and is a
// parameter-order change, so it was dropped. The parameter layout is the same as retail's (the weapon
// index at [esp+4], the other index at [esp+8], the player index at [esp+12], the event index at [esp+16] and
// the rounds pointer at [esp+20], the callee popping twenty bytes), so nothing here is blocked by the
// declaration.
// MiniMax G (11-13): no new form, no lower score (190): both streams are 215 instructions long and the
// divergence starts in the prologue, where retail holds the destination weapon in EBP and the other
// weapon in EBX while ours holds the destination in EDI; the acquisition flag is stored the other way
// round as well (`setg` against our `setle`). An annealed permuter run over this body (300 s) again
// found only the argument swaps of `function_1896c0` and `function_a88a0`, which contradict those
// declarations, so it was dropped.
// MiniMax I (11-14): no new form, no lower score (190): both streams stay 215 instructions long and
// the divergence starts in the prologue, where retail holds the destination weapon in EBP and the
// other weapon in EBX while ours holds the destination in EDI and spills the other; the acquisition
// flag is stored the other way round as well, and flipping it to retail's sense compiles worse.
// MiniMax G (11-20): no new form, no lower score (190): retail keeps the destination weapon in EBP and the other weapon in EBX, stores the acquisition flag the other way round (a byte zeroed with the loop counter's dword and set with setg on acquired) where this build keeps a byte of its own that starts at one, and retail keeps the conversion entry without a nullable test; the flag's polarity, the two products' operand orders and the entry's null test were re-measured and give the same or a worse score.
// MiniMax S (11-15): no new form, no lower score (190): retail keeps the destination weapon in EBP and the other weapon in
// EBX, stores the acquisition flag the other way round (a byte zeroed in the prologue and set with `setg` on "acquired") and
// keeps the conversion entry without a nullable test, while ours holds the destination in EDI, spills the other and tests the
// entry for null.
// Lane N round 411 (fast control 190).
// Shared source definition index 190.
// Capacity and transfer 192.
// Transfer join result 204.
// Conversion block 190.
// Updated source rounds 209.
// Retained kind expression 207.
// Initial transfer flags 196.
// Both definition indices 190.
// m3 lane 2 r105 (fast): the other weapon's definition index in a named local at function scope 199, also passed
// to 0xa88a0 218, declared in the `available > 0` block 190 (same code), at loop scope 210; the source pointer
// declared inside the `available > 0` block 210 (715 bytes); the magazine read through the weapon at each use 207.
// Round 103 (m3 lane 15, fast): the other definition's type byte read at each of its three tests (retail reloads it
// after each call) 212; the conversion scan ended on `j >= count` with the entry picked by index 202 (718 bytes; the
// re-test is not threaded); a separate transfer count started at 0, set in the same-definition arm and in the scan's
// found arm (retail's shape: no null test, source cleared just before the definition compare, the count tested again
// after the same-definition `setg`) 202 (716 bytes); that with retail's flag polarity (acquired false at entry,
// `= transfer > 0`, `= true` in the scan, returned) 208 (709 bytes); plus the total's operands swapped 208 (same
// code); plus `retained` as a ternary (retail's `xor al, al` in the else arm) 215 (702 bytes); plus the type byte
// at each use 205 (719 bytes). The last form matches retail's opcode shape in 180 of 215 instructions (158 before);
// what is left: retail keeps the weapon in EBP and pushes EDI only after the loop guard (the early exit returns the
// cleared flag in AL), the scan count is spilled with the index in EBP, and the found entry is reached through the
// pointer formed in the scan. Not kept: no count below 190. Original body restored.
// M3 lane 2 r110 (fast; control 190; aligned = LCS line count, 135 for this body). Behaviour fix kept: the call to 0xa88a0 passed
// its two definition indices the other way round. Retail loads EDI from the weapon's index and ESI from the other weapon's
// (0xffed6-0xffee0) and the matched callee tests the type byte of the one in ESI, its first definition parameter. Count stays 190.
// Lead, match-only (build/r110/lead-0xffc60-52.patch in m3-lane2): 52 at 731 bytes, first difference +0xa1, every remaining line
// the ECX/EDX choice from the loop head on. It is: one exit (`if (mode == 4) acquired = false; else { ... }` for the early
// `return false;`, which puts the result in AL through the first block: `xor al, al`, the store from AL, EDI pushed after the
// loop guard, the count loaded for the guard); the two flags declared ahead of `*rounds = 0;` (retail stores other_has_rounds as
// an immediate and the result from AL); a transfer count started at 0; the same-definition clamp as a long ternary (`movsx esi,
// si` in place); the conversion scan reading its count and table from the magazine definition at each use with the entry pointer
// taken in the loop (the scan is then retail's instruction for instruction, count spilled); the loaded clamp in long; `retained`
// as a ternary; the type byte read at each use; the magazine pointer declared ahead of the definition's; the unloaded count read
// through `other` beside the source pointer. The last two make it match-only: with the definition's pointer first the loop index
// takes ECX for retail's EAX (207 at 713 bytes, aligned 33), with both sum operands through the source pointer `other` loses EBX
// to the weapon (204). Without those and the flags-first order the retail-shaped body measures 204-210.
// Steps (fast): retail-shaped transfer body 218; with ternary `retained` 205; count and table at use 229; that with no magazine
// local 159 (161 with the call fix); 36 declaration combinations with the fix 161-231; explicit returns after the loop
// 233/233/235; single exit with the magazine pointer first and unloaded through `other` 58, through the source 157, magazine
// pointer last 210; flags-first order and long ternary 52. Clean steps alone on this body: call fix + single exit 203, with the
// positive flag 203, flags first 204, long ternary 203, long loaded clamp 202, type byte at use + ternary `retained` 209.
// Same code or worse (compile-only screen, about 1500 forms): loop condition reversed, post-increment, index declared at
// function scope, a while loop, compare operands reversed, a named other-definition index, transfer and source declaration
// places, MIN or if forms of the three clamps, the existing game-mode helper, inline accessors for the two magazine pointers or
// the count, a short pointer or reference to the unloaded count, `acquired = true` ahead of the conversion clamp (the cmov
// becomes a branch), the leftover-rounds flag as an `||` chain (gives retail's loop-head registers but reads the flag).
// Permuter on the 52 form (600 s, 43 tries): best 52.
// Residues captured after both stores or one at a time 190/190 with sequential tests,
// 191/191 with both predicates evaluated, 189/189 with sticky accumulation. Calculated
// residues carried into both stores or published as calculated 209/209. NOT kept: one-point
// gain at 189, with destination and conversion search register homes unchanged.
// Paired weapon references, source tag reference and magazine tag reference 190 each.
// Source selected at capacity 208, reserve field reference 207, removal predicate 188,
// shared weapon-kind predicate 214, shared type mask 190. NOT kept: the removal predicate
// gains two with the same extent and saved-register divergence.
// Initial object-table base, tag-table base, both, or both scoped to the
// lookup phase 190 each. Source slot decoded as word/long, with the original
// lookups or the paired bases 203/217 and 203/217. NOT kept: the base reads
// tie, and naming the source ordinal regresses. Destination EBP and delayed
// EDI save remain.
// Rounds clear before the lookups or after the destination object/tag or source object 215/221/220/211.
// Early declarations for the retained/acquisition/both flags 190 each. Source tag deferred until acquisition 211.
// NOT kept: the early clear changes load order, the declaration forms keep EDI, and the deferred tag shrinks the frame to 0x1c.
// Shared absolute source slot 190, word slot 203, typed conversion block 190, read-only entries 190, entry
// formed before comparison 214, shared slot with all type tests reread 212, shared slot and typed block 190,
// also full-width total selection 191. NOT kept: the forms tie or regress and retain the saved-register
// divergence.
// Transfer amount and mode guards 190, Transfer amount and nested mode admission 190, Positive
// acquisition with transfer guards 202, Positive acquisition with nested mode admission 202, Final mode
// selects complementary acquisition outcome 203, Final mode selects positive acquisition outcome 199,
// Final mode outcome with three kind reads 218, Positive final mode outcome with three kind reads 215.
// NOT kept: The separated transfer gates tie. Positive acquisition shortens the body, and the final mode branches and kind reloads raise the score.
// The 0x20 frame agrees. The destination home, delayed save and conversion-entry join still differ.
// Bound and key in the search test 202, with the key carried 202;
// short search ordinal and offered count 202; key shared across both arms 202;
// single exit through negative acquisition 207; source reserve or loaded member lvalues 205 each;
// transfer cap inside the successful search 199. NOT kept: the ordinal searches shorten the body
// to 718 bytes, and the forms at retail length still miss the destination and key registers.
// Cached conversion membership as shifted bool, masked bool, result of a test or shifted byte 214/214/209/214.
// Byte or word type mask 204/190. Boolean membership with conditional retention or ternary retention 197/188.
// NOT kept: the two-point gain leaves the source and destination register roles unresolved.
// Destination magazine bank 207, source bank 190, both runtime banks 207, both runtime
// arrays by reference 207, persistent tag bank 208, destination and tag banks 209, source
// and tag banks 208, all three banks 209. NOT kept: the source bank ties, the destination
// bank shortens the body, and retaining the tag bank does not recover the destination save.
// The destination save and conversion-scan homes still differ in the 0x20 frame.
// Separate short transfer, arm-selected or capacity-initialized, with negative/positive
// acquisition 192/195 and 190/202. Long transfer equivalents 205/202 and 205/206.
// NOT kept: the short form ties and the positive or widened forms tie no lower or shorten the body.
// Conversion candidate as mutable or const reference before the match test 214/214;
// with an unequal-entry continue 214/214; the same four forms as a pointer 214/214/214/214.
// NOT kept: all shorten the body to 712 bytes and leave the first saved-register difference.
// Promoted ordinal for both destination rows 211, both runtime rows 190, definition and source 190,
// all three 190, all three with runtime row declared first 197, and source branch only 190.
// Same-definition predicate with separate source observations 234, also with shared source key 234.
// NOT kept: The four ties preserve the exchanged saved roles. Both separate-source forms grow to 760 bytes.
// Retail saves EBP for the destination and defers EDI until the magazine loop. The frame is 0x20 in both.
// Conversion miss joined as zero, success/failure first 190/190; arm-owned source with both joins 218/218.
// Conditional conversion cap at the join 190; signed pickup amount short/long 200/207; short with joined miss 200.
// NOT kept: the joined conversion paths tie or shorten the body, and holding the pickup amount adds a frame word. The caller and both callee controls retain their scores.
// M3 lane 11 r157 (H2M_FAST; control 190): lane 2 r110's 52 lead rebuilt 52; on it the conversion clamp written like the
// unloaded clamp (`count > available ? available : count`) 51 at 731 B, register-masked 2 (retail's 16-bit count load and
// flag store order then agree); left only the loop's ECX/EDX pair (offset 16 in ECX for retail's EDX, base and source in
// EDX for retail's ECX) and the tail's scratch phase from the 0xa88a0 call on. Compile-only screens on the 51 form (the
// loop's pair reproduces there): the four sum forms, source/transfer declaration places and order, the magazine
// definition first (pair right, index in ECX and base in EDI), references to the magazine, definition or source, a long
// index or available, a redundant loop guard, flag initialisation orders, compare operand orders, `continue` for the
// capacity and mode tests, the clamp as if/MIN/swapped ternaries: the pair never flips; with no clamp in the same-
// definition arm (diagnostic) it does. Available clamped in place with a `continue` on a scan miss 165 (fast; offset 16
// right, available and source then swap ECX/ESI). Tail: the call's count in a named long or short local, the sound
// store as a block, the existing-object pointer for the last lookup 51 each (same phase). On main's body the gt clamp
// alone 190 (same code). Lead saved as build/r157/lead-0xffc60-51.patch. Permuter on the 51 form (900 s, 66 tries): best 51.
// More tail forms on the 51 form (H2M_FAST): the sound picked by if/else 51, both NONE compares written constant first 51
// (same phase each), the other's definition re-derived through the pool for the sound 110 (747 B), `retained` as false
// plus an if 136 (741 B).
// M3 lane 11 r164 (H2M_FAST with 0xa88a0, 0x1896c0, both MATCH; round 157's 51 lead rebuilt, 51). Reading on the lead:
// the loop's ECX/EDX pair is a colouring order: the `i * 16` magazine offset (priority 166) is coloured before `source`
// (156) and takes ECX first; retail's pair follows when `source` outranks it. `source` loses 96 for being live but
// unreferenced across the conversion scan (16 blocks); the offset gains 128 in the loop head (eight candidates
// referenced there). With the offset forced into EDX (a diagnostic, not source) the lead measures 14, the loop exact and
// only the tail's scratch phase left (from the 0xa88a0 argument setup on the cursor starts one register later than
// retail's). Tried on the lead: `source` set to NULL in the conversion arm after the scan (not at its declaration) 162;
// set to NULL at the start of the conversion arm 173 (732 B); the other definition's type byte read once into a local
// (with the forced offset) 130 (an extra frame cell); the magazine definition pointer formed only in the conversion arm,
// the loop head reading the field through the definition, 208 (713 B); the same-definition sum read through `source`
// for both operands (re-measured: it does flip the loop pair) 204 (718 B: the weapon and the other weapon swap
// EBX/EBP and the loop entry changes).
// Generic source view mutable/const, generic destination view, and paired mutable/const views 190 each.
// Existing header accessors for the destination, source with generic refresh, and both with generic refresh
// 190 each. NOT kept: all preserve the exchanged saved-register roles and 731-byte extent.
// Destination, source and tag-array lvalues, single and paired loop-entry bindings 190 each;
// all three bound together at loop entry or per iteration 190 each. Saved destination and
// conversion-key registers remain different.
// The 0x20 frame agrees. The destination weapon and conversion key still use different saved registers.
// separate source-magazine selections 234, separate selections with positive acquisition 226.
// separate selections with one exit 227, conditional source pointer with positive acquisition 216.
// shared source tag key and scoped magazine 234, full-magazine continue with scoped magazine 226.
// transfer-gate continue with scoped magazine 226, source pointer assigned by the selected arms 242.
// NOT kept: every form raises the score. The conditional source pointer changes the save prefix but shortens the body to 698 bytes.
// Shared tag key by value or field reference 190/190. Keyed scan with hit-owned
// cap 205, hit-owned offer 197, joined comparison 202 or delayed table read 202.
// Positive acquisition with hit-owned cap 212; named key and ordinal with
// hit-owned offer 197. NOT kept: the key binds as before, and the scan rewrites
// raise the score. The 0x20 frame and destination/scan register difference remain.
// Shared long minimum for same-definition with conversion/unloaded/loaded 205/205/191;
// conversion with unloaded/loaded 197/204, both debits 204, all four 203;
// shared short minimum for conversion and both debits 204.
// NOT kept: all retain the 0x20 frame and 731 bytes, but the saved-register discrepancy persists.
// The 0x20 frame agrees; destination, source and conversion search register homes still differ.
// Declared source kind, destination sound, source sound, both sounds and simulation index 190 each.
// Declared kind at each use 212; all declared fields with one kind read or per-use reads 190/212.
// Both source counts shared with the limits or carried into the debit stores 211/221.
// NOT kept: one kind read ties; per-use reads grow and the count snapshots change the frame and raise the score.
// Existing reserve accumulator for the same-definition offer, capacity and unloaded debit
// 190/197/217. Offer/capacity 197, offer/debit 244, capacity/debit 202, all three 223.
// Accumulator after debit for the remaining reserve 236.
// NOT kept: offer ties and the other forms raise the score. Destination and search register
// homes still differ, and the debit-only and post-debit forms also grow.
// Immutable capacity carried to a mutable offer 225. Separate capacity and selected offer 192.
// Immutable tag/object pointers, conversion bound/entries/key, named ordinals and named conversion
// offer 190 each. Existing reserve query with loaded first or unloaded first 190/190, with a
// separate capacity 192. NOT kept: the immutable-capacity handoff grows to frame 0x24, the
// separate offers raise the score, and the rest tie. The destination and conversion-search homes differ.
// M3 lane 13 r159 (H2M_FAST; round 157's 51 lead rebuilt, 51). Reading on the lead: the magazine offset `i * 16` ranks
// 166 (128 of it from the loop head, where eight candidates are referenced) against `source` 156 (96 lost across the
// conversion scan); one candidate fewer in the loop head, or one more in the debit block (where `source` has six
// references), would put `source` first. Forms on the lead: `if (source != NULL)` 51, the loaded clamp as a ternary 51,
// the unloaded clamp as an if 51, the total's operands swapped 51, the definition compare written other-first 52,
// `transfer` declared ahead of `source` 51, `available >= 1` 57, the magazine definition pointer first 208 (713 B), the
// conversion compare written other-first 51. With the offset forced into EDX (a forced-ranking search, 14 left: the tail's
// scratch rotation from the 0xa88a0 setup on, one register late): the sound as an if/else, the last lookup's index not
// named, `!(other_has_rounds || retained)`, the sound call braced 14 each; `retained` as main's if 102; the last lookup
// through `other` 232.
// M3 lane 8 r243 (H2M_FAST with 0xa88a0; round 157's 51 lead rebuilt, 51). Reading on the lead (allocator priorities):
// the magazine offset 166 = 128 from the loop head (eight candidates referenced there: the counter, the offset, the
// weapon definition, the magazine definition, the weapon, the magazine address, `available` and the zero) + 48 from the
// same-definition arm - 10; `source` 156 = 252 referenced (144 of it from the debit block, six candidates) - 96 live
// across the conversion scan unreferenced; so one more candidate in the debit block (+24) or one fewer in the loop head
// (-16) puts `source` first. Forms on the lead: `loaded` as a short 74; `unloaded` as a long 53; the 0xa88a0 count read
// into a named local 51 (same code); the other weapon's type-2 test held in one bool for the sound and the tail 145;
// the debit's destination store through `weapon->magazines[i]` 214 (737 B). Body unchanged.
// M3 lane 7 r236 (H2M_FAST with 0xa88a0 and 0x1896c0; round 157's 51 lead rebuilt, 51). Per-block ledger on the lead:
// the offset = loop head +128 (eight candidates: the counter chain, the offset, the definition, the magazine-definition
// copy, the weapon, the magazine pointer, `available` and the zero of `available > 0`) + 48 in the same-definition arm
// - 10 = 166; `source` = 252 referenced (144 in the debit block, six candidates) - 96 live across the conversion scan =
// 156. Forms on the lead: no magazine-definition local (`definition->magazines[i]` at its three uses) 208 (713 B);
// `source` uninitialised and set NULL at the start of the conversion arm 173 (732 B); in the debit, `remaining` as its
// own local with `loaded` its clamp 162 (733 B), `short loaded` 74, `remaining` copied into `loaded` then clamped 51
// (same code), the clamp as `loaded >= source->rounds_loaded` 53.
// @retail 0xffc60
bool __stdcall function_ffc60(long weapon_index, long other_index, long player_index, long event_index, short *rounds)
{
    s_weapon *weapon = WEAPON_GET(weapon_index);
    s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
    s_weapon *other = WEAPON_GET(other_index);
    s_weapon_definition *other_definition = WEAPON_DEFINITION(other);
    *rounds = 0;
    bool other_has_rounds = false;
    byte unacquired = true;
    for (short i = 0; i < definition->magazine_count; ++i)
    {
        s_weapon_magazine_definition *magazine_definition = &definition->magazines[i];
        s_weapon_magazine *magazine = &weapon->magazines[i];
        short available = magazine_definition->rounds_total_maximum - magazine->rounds_unloaded;
        s_weapon_magazine *source = NULL;
        if (available > 0)
        {
            if (weapon->definition_index == other->definition_index)
            {
                source = &other->magazines[i];
                long total = source->rounds_unloaded + source->rounds_loaded;
                if (total <= available) available = (short)total;
                unacquired = available <= 0;
            }
            else
            {
                long count = *(long *)((byte *)magazine_definition + 0x54);
                s_ammo_conversion_entry *entries = *(s_ammo_conversion_entry **)((byte *)magazine_definition + 0x58);
                s_ammo_conversion_entry *conversion = NULL;
                for (short j = 0; j < count; ++j)
                {
                    if (entries[j].definition_index == other->definition_index)
                    {
                        conversion = &entries[j];
                        break;
                    }
                }
                if (!conversion) continue;
                available = conversion->count <= available ? conversion->count : available;
                unacquired = false;
            }
            if (available > 0 && g_4e6948->mode != 4)
            {
                if (source)
                {
                    short unloaded = source->rounds_unloaded > available ? available : source->rounds_unloaded;
                    short loaded = (short)(available - unloaded);
                    loaded = source->rounds_loaded <= loaded ? source->rounds_loaded : loaded;
                    source->rounds_unloaded -= unloaded;
                    source->rounds_loaded -= loaded;
                    if (source->rounds_unloaded > 0 || source->rounds_loaded > 0)
                        other_has_rounds = true;
                }
                magazine->rounds_unloaded += available;
                *rounds += available;
            }
        }
    }
    if (!unacquired)
    {
        function_b7360(weapon_index);
        function_b7360(other_index);
        if (g_4e6948->mode == 4) return false;
        byte other_type = *(byte *)other_definition;
        bool retained = false;
        if ((bool)((((dword)1 << other_type) >> 2) & 1)) retained = function_101640(other_index);
        if (!other_has_rounds && !retained) function_b8540(other_index);
        if (event_index != NONE)
        {
            long sound = ((1 << other_type) & 4) ? *(long *)((byte *)definition + 0x268) :
                *(long *)((byte *)WEAPON_DEFINITION(other) + 0x138);
            if (sound != NONE) function_1896c0(1.0f, sound);
        }
        function_a88a0(player_index, other->definition_index, weapon->definition_index, *rounds);
        function_a7cd0(weapon_index);
        if ((1 << other_type) & 4)
        {
            long index = *(long *)((byte *)WEAPON_GET(other_index) + 0xd4);
            if (index != NONE) function_b58c0(index, 0x2000);
        }
    }
    return !unacquired;
}


long function_101fb0(long object_index);
void function_1628f0(long player_index, char state);
void function_d0e60(long unit_index, real amount, real limit);
void function_bfc30(long unit_index, long value, long priority);
long function_c7100(long unit_index);
void function_d03e0(long unit_index, long definition_index);
real function_17ca10(real value, short curve);
void __stdcall function_187510(long player_index, real yaw_delta, real pitch_delta);
void function_176970(s_effect_owner const *owner, real scale_a, long tag_index, long object_index,
    short node_index, short marker_count, s_effect_marker *markers, real scale_b,
    point3f const *origin, vector3f const *direction, bool flag);

#define WEAPON_REAL_AT(base, offset) (*(real *)((byte *)(base) + (offset)))
#define WEAPON_SHORT_AT(base, offset) (*(short *)((byte *)(base) + (offset)))
#define WEAPON_LONG_AT(base, offset) (*(long *)((byte *)(base) + (offset)))
#define WEAPON_BYTE_AT(base, offset) (*((byte *)(base) + (offset)))
/* one bit of a long, as retail's `shr; test 1` */
#define WEAPON_BIT_AT(base, offset, bit) ((bool)(((dword)WEAPON_LONG_AT(base, offset) >> (bit)) & 1))

#define WEAPON_WORD_AT_102FB0(base, offset) (*(word *)((byte *)(base) + (offset)))

long unit_get_player_index(long unit_index);

// GA r9 lead, held as a patch (not kept: the forms are kept only with a byte match): 2 against 593: Only two commuted SIB base/index encodings differ at +0x5cd (`[eax + edx + 8]` vs `[edx + eax + 8]`)
// and +0x616 (`[edx + ecx + 0x1b8]` vs `[ecx + edx + 0x1b8]`) in the post-0x105fa0 recoil lookup; all 634 instructions,
// 2533 bytes, frame 0x1c60, registers, and stack slots match retail.
// score 593 (was 603): Frame 0x1c64 versus 0x1c60; retail keeps the unit index in EDI through the fired block (`cmp edi, -1`)
// where the volatile home is read at each use here; weapon/barrel and definition/holder homes remain.
// MiniMax I (11-20): no new form, no lower score (636): the prologue keeps the barrel index in EBX where retail keeps the weapon index, reads the barrel's flag byte into CL where retail tests it in memory, and the definition sits at [esp+0x28] where retail has it at [esp+0x14], so the copy-out loop and the rest of the body differ as well.
// MiniMax B (11-18): no new form, no lower score (643 in the fast check, 607 in the plain one): retail keeps the weapon index in ebx, the barrel offset in edi and the definition spilled to [esp+0x14], where this build keeps the barrel offset in ebx, the definition in edi and the weapon index in a stack slot, so the prologue and every later home differ; dropping the volatile on the unit index measures 657.
// MiniMax I (11-17, run 103): no new form, no lower score (607 plain, 643 fast): retail still keeps the weapon index in
// EBX and the barrel index in EDI and reads the first argument after its first push, where this build keeps the barrel
// index in EBX, the definition in EDI and reads the argument before the pushes; the frame still agrees at 0x1c60.
// MiniMax I (11-17): no new form, no lower score (607 plain, 643 fast): the frame still agrees with retail at 0x1c60 but
// ours is 2545 bytes against retail's 2533, retail keeps the weapon index in ebx, the weapon pointer in ebp and the barrel
// offset in edi and spills the definition to [esp+0x14] where this build keeps the weapon index in a stack slot, the
// barrel offset in ebx and the definition in edi, so the prologue and every later home differ; the permuter cannot search
// this function on the reduced image and its full-image baseline is 604 differing instructions, so the body stands;
// MiniMax H (11-14): no new form, no lower score (607 plain, 643 fast): the frame agrees with retail at 0x1c60, but
// the length is 2545 bytes against retail's 2533 and the homes do not: retail keeps the weapon index in ebx, the weapon
// pointer in ebp and the barrel offset in edi and spills the definition to [esp+0x14], where this build keeps the weapon
// index in a stack slot, the barrel offset in ebx and the definition in edi, and its stack slots sit four bytes off
// retail's, so the prologue and every later home differ.
// The permuter cannot search this function on the reduced image (it reports that the reduced image builds it
// differently); on the full image its baseline is 604 differing instructions, so only a plain-image search can try forms.
// MiniMax H (11-13): no new form, no lower score (607 plain, 643 fast); the frame agrees at 0x1c64 against
// 0x1c60 and the weapon, barrel, definition and holder still live in different homes, so the prologue and the
// register allocation of the barrel walk differ. Nothing here can be reshaped without touching the declarations
// that other files own.
// MiniMax R (11-14): no lower score (643 fast): the frame 0x1c64 differs from retail's 0x1c60 by four
// bytes and the weapon, barrel and definition homes remain; the unit index without the volatile
// measured 657, with it 637 and with the NONE at the declaration 640, so the kept form stands.
// MiniMax G (11-10): no new form, no lower score (607).
// MiniMax R (11-10): no new form, no lower score; the unit index without the volatile scores 657, with the volatile and the NONE at the declaration 637 and without the volatile and the NONE at the declaration 640, so the kept form stays the best.
// MiniMax G (11-04): no new form, no lower score; the barrel pointer before the definition gives fast 638 (was 640), below the retention threshold.
// MiniMax C (11-01): no new form, no lower score.
// Earlier 607 (was 604): Frame 0x1c64 versus 0x1c60; weapon/barrel and definition/holder homes remain.
// MiniMax H (11-18): score 643 (was 643): no new form, no lower score; the frame stays 0x1c64 against retail's
// 0x1c60 and the weapon, barrel, definition and holder still live in other homes, so the prologue and the
// barrel walk's register allocation differ; the plain image reads 607 for the kept body.
// Earlier 604 (was 604): Frame 0x1c64 versus 0x1c60; weapon/barrel and definition/holder homes remain.
// Earlier score 604 (was 604): Frame 0x1c64 versus 0x1c60; weapon/barrel and definition/holder homes remain.
// Earlier: round 87 measured 606 plain (was 565; fast 637 -> 640); the three behaviour fixes
// below are applied by the coordinator (round 87 review) and kept although the count rises. Frame 0x1c64 versus
// 0x1c60; weapon/barrel and scalar homes remain.
// frame 0x1c64 versus 0x1c60; weapon/barrel and scalar homes remain.
// Earlier: the three behaviour fixes below are applied by the
// coordinator (round 87 review) and kept although the count rises. Frame 0x1c64 versus 0x1c60; weapon/barrel and
// scalar homes remain.
// Plain single-store unit 637; later effect locals 659; long marker count 607;
// CRT marker copies 565; long fired 582.
// Owner getter 563; scale names 565; direct barrel uses 641; const barrel tag 565;
// effect cursor before snapshot 565 (all plain). Original body restored.
// copy differ. Trials: word fired 627; word failure 648; native original index 641; native interval
// bounds 623; word selected marker 565; later fired lifetime 573.
// Native effect entry 565; byte fired 565; byte failure 565; unsigned selected marker 565; barrel element
// reference 565. Existing call inputs and random sequence retained.
// Magazine selector 624/622; accumulated-shot snapshot 629/583 (fast/plain);
// shared player lookup 642; later tag outputs 639; later scale outputs 653 (fast, control 637).
// Admission scope 637f; rounds before independent owner/default stores 632f/607 plain;
// owner lifetime at entry 637f; entry marker workspaces 637f; unsigned-word mode 637f
// (control 637f). Original body restored.
// Round 115: Later failure lifetime 636; reversed tag defaults 637; interval field reads 634; positive magazine arm first 637; byte selected marker 638 (fast, control 637). Three-point preview gain rejected; opening body restored.
// Round 337 (fast control 637): direct short/promoted barrel values 637/634;
// direct weapon value/both short values 637/637; both promoted values 634;
// first-use promoted barrel/both/immutable values 634/634/634. Gain 3 rejected; all restored.
// Round 367: Test-produced shot result 642; holder scalar name 637; both 642; shared notification on two-
// branch holder graph 637; shot/shared 642; holder/shared 637; all 642; all with shot-result name 642 (fast
// control 637). Original body restored.
// Complete native tag-reference arrays with short/long/byte/word mode, raw/native interval
// fields: 637/637, 639/639, 632/632, 637/637 (fast control 637). Five-point gains rejected;
// original effect type, fields, body, parameter aliases and local qualification restored.
// Chained tag/scale/both defaults 637/637/637; chained admission-failure alone/with
// tags/with scales/with both 636 each; all reverse chains 637 (fast control 637).
// One-point gain rejected; original initialization graph and body restored.
// Round 172: real weapon/tag/barrel-record fields in seven nonempty family subsets, then all
// with native tag bits, all eight 637 (fast control 637; full baseline 565). Original padding,
// macros, inherited aliases/qualification and body restored; frame and receiver/scalar homes remain.
// Round 381 initial producer orders (definition D, barrel metadata M, barrel B, holder H), preview:
// DBMH 635; BDMH 566; DBHM 610; BHDM 613; HDMB 624; HBDM 634; DHMB 620; DHBM 633.
// Ordinary t02 620, t03 595, t04 626, t07 630, t05 632. All original producers and body restored.
// Round 32: shared RNG for selection/count/both 637/637/637; complete effect-count
// capture alone/selection/count/both/const both 621 each (fast control 637). Capture
// plain 621 at 2533/2533 bytes versus baseline 565 at 2526/2533; rejected. Original body restored.
// Round 168: Failure-threshold/result/recoil subsets 579/567/596/638/605/593/627/627 fast (control 637); ordinary t01/t02/t03/t05/t06/t07 635/573/626/636/600/627. Original 565 restored; preview-only frame recovery rejected.
// Round 186 (fast control 637): ordinary holder with four initial producer orders
// 652/655/570/637; the four direct-parameter forms give the same scores.
// Barrel/holder before definition, original/direct inputs, ordinary 619/619.
// Original holder qualification, producer order, parameter aliases and body restored.
// Round 300: Existing point/vector marker setters, both, workspace lexical names, point/names,
// vector/names, both/names, and complete graph with entry workspaces:
// 660/657/674/637/660/657/674/674 (fast control 637; full 565). Original body restored.
// Round 87: three source errors against retail (now FIXED in the body), each raising the positional count: retail picks the random
// effect once before the do loop (604), passes the barrel scale (1 when not fired) as the first scale and the
// heat ratio as the second at both effect calls (571, 2533/2533 bytes, aligned count 277 -> 271), and clamps the
// heat as `!flag3 && heat > max` else `heat > 1`; all three 606. Single-store volatile unit 629 (630 with the
// fixes). A plain (non-volatile) unit index set to NONE then overwritten gives retail's 0x1c60 frame and aligned
// count 229 (219 with the scale order, 227 with all fixes) but 2503-2509 bytes, positional 645-649: the next lead.
// The two parameter-address references were code-identical no-ops and are removed.
// Round 94 (plain): plain unit index 651 (2503 bytes, frame 0x1c60, aligned 348 -> 276); with the definition
// declared after the barrel pointers 651; second barrel-side argument as `barrel_index ? 2 : 1` 605 (retail
// recomputes the test there); barrel pointer declared before the definition 610 (retail's prologue order; the
// definition keeps EDI where retail spills it); that with the plain unit index 655.
// Round 239: complementary shot/failure/both results 570/570/571 fast; ordinary
// holder counterparts 660 each; both byte results 571, ordinary holder 660. Fast
// control 640; four promising forms all 604 plain. Original body restored.
// Round 246: live magazine/metadata/both/both with ordinal 653/602/581/634;
// ordinary-holder counterparts 641/666/659/672 (fast control 640). Ordinary
// metadata/both 643/649 versus full 604. Original receiver graph and body restored.
// Walk against retail (w1C, 11-05): every block of 0x102fb0 was compared with retail statement by statement; the only
// logic difference was the effect marker's direction, now read from markers[].matrix.forward (retail 0x1038f0-0x103914
// copies element+0x3c, the matrix forward, not element+0x8 of the node matrix); fast 640 -> 643, 2542 -> 2545 bytes (retail 2533).
// Dropping the `volatile` on unit_index gives fast 657 / 2501 bytes (retail keeps it in [esp+0x24], stored at entry and
// reloaded into EDI after each call). What remains differs only in homes: retail EBX = weapon_index, EDI = barrel offset
// (index*0x34, the barrel is never a pointer), definition spilled to [esp+0x14]; ours EDI = definition, EBX = offset.
// Permuter, 40 minutes (11-05): full image 590 only with temporaries for call arguments and indices and casts that change
// no value. Its plain parts (barrel before barrel_definition, function_b7360 before the rounds read, the clamp as nested ifs,
// the fired test written positively, burst declared after the unit) give 639 fast, from 643. Not kept.
// MiniMax E (11-10): no lower score (643 in the fast check, 607 in the plain one). Retail keeps the weapon
// index in EBX, the barrel offset in EDI and spills the definition to [esp+0x14]; ours assigns those registers
// the other way round and reserves four bytes more frame (mov eax, 0x1c64 against retail's 0x1c60). The fast
// check warns that this function builds differently in the reduced image, so its plain-check score differs
// from the fast one. The parameter layout is the same as retail's (both arguments on the stack, the callee
// popping eight bytes), so what is left is register allocation and the frame.
// MiniMax A (11-12, run 62): no new form, no lower score (607 in the plain check, 643 in the fast one); the first
// difference is at +0xb, where ours loads the actor argument and the definition table before the weapon and retail
// loads the pool pointer and takes the argument into EBX, so the prologue's registers and the four extra frame bytes
// differ before any case body is reached.
// MiniMax A (11-14): no new form, no lower score (607 in the plain check, 643 in the fast one); retail's frame is four bytes shorter than ours and its homes for the weapon index, the definition and the two barrel offsets differ, so the prologue and the copy-out loop stay laid out differently.
// MiniMax S (11-15): no new form, no lower score (607): retail keeps the weapon index in EBX and the barrel offset in EDI with
// a frame four bytes shorter than ours, and the copy-out loop and the effect marker's block order differ.
// MiniMax D (11-16): no new form, no lower score (643 in the fast check; ours is 2545 bytes against retail's 2533):
// retail's frame is four bytes shorter and it keeps the weapon index in ebx, the barrel's offset in edi and the definition
// spilled to [esp+0x14], while this build gives ebx the barrel's offset, edi the definition and esp+0x14 the weapon; the
// barrel itself is reached as the weapon's base plus the index scaled by 0x34 in retail, and the direct barrel uses this
// lane measured worse, so the body stands.
// Round 605. The full build still uses 0x1c64, compared with 0x1c60 in retail. Reduced control 643. Default
// holder before the inventory test 637. Shared absolute slot 643. Read-only weapon, barrel and both definition
// views 643 each. Declared definition flags 643. Named barrel scale store 643. Weapon item bitfield 636. NOT
// kept: the holder default and item bitfield each gain fewer than eight in the preview.
// Round 103 (m3 lane 15, all plain, control 607): the plain unit index set to NONE first (retail caches it in EDI and
// reloads it from its frame home only after the redirect, an ordinary spill) 648 (2506 bytes, frame 0x1c60); the
// firing-effect entry formed before the two barrel stores (retail loads the effect table before them) 636 (2522
// bytes; the effect loop then lines up with retail); both 654 (2506 bytes; opcode shape 570 of 634, against 535);
// that entry form with the redirected unit as a ternary argument 637; the `scale_a = 0.0f` store retail makes in
// the not-fired arm (movss [esp+0x20] at 0x103462) 609 (2543 bytes); the plain unit, the entry and the store
// together 646 (2512 bytes, shape 571). Left over: the per-call instruction counts are within five of retail's
// everywhere, so the positional count comes from drift: retail keeps the weapon index in EBX up to the first
// fire call, the magazine definition in EBX across the 0x106030/0xd1210 calls and the barrel offset in EDI.
// Not kept: no count below 607. Original body restored.
// Round 145 (m3 lane 16, plain): the unit index from the file's existing weapon_get_owner_unit_index (main wrote its
// body out as an if/else), the volatile kept, 603 (kept); without the volatile 648 (2506 bytes); the helper's own shape
// written in place (NONE stored first, then overwritten, as retail stores [esp+0x24] before its je) 596, not kept (a
// hand-written helper body). Fast reads 636/640/637 for the three (fast control 643).
// On 603 (plain): the second barrel-side argument as `barrel_index ? 2 : 1` 600 (retail recomputes it, no cached
// slot; frame stays 0x1c64; a three-point gain, not kept); the first as `(barrel_index != 0) + (failure ? 3 : 1)` 644.
// The four definition flag tests read from the definition's named bits instead of shifted raw words: 603 (same code, kept).
// The extra frame word: retail keeps the magazine definition in EBX across the 0x106030/0xd1210 calls and reuses its
// [esp+0x2c] for the rounds_loaded temporary; ours spills the magazine definition there and gives the temporary [esp+0x50].
// The magazine definition read at its one use instead of the local 640 (2518 bytes); with the ternary above 641.
// Left: ours caches the sign-extended rounds_loaded and the constant 1 of the unit-type test in frame slots where retail
// keeps them in ECX/EDX, and retail holds the magazine pointer in [esp+0x34] with its definition in EBX.
// Typed unit player fields 643, ordinary holder query 640, inventory-selected holder with
// typed player fields 640, typed header table base 643, magazine record references 643,
// magazine tag resolved at notification 601, indexed barrel tag fields 836, named replacement
// barrel field 643. Reduced control 643. NOT kept: late magazine lookup reaches retail length
// with frame 0x1c58 and delays an early retail lookup. The ordinary holder forms shorten the body.
// MiniMax B (11-19): no new form, no lower score (607 plain, 643 fast): retail still keeps the weapon index in EBX and
// the barrel's offset in EDI, spills the definition to [esp+0x14] and the unit index to [esp+0x24], where this build
// gives EBX the barrel's offset, EDI the definition, [esp+0x14] the unit index and [esp+0x28] the definition, and
// the full build still uses a 0x1c64 frame against retail's 0x1c60; a local copy of the weapon index, a computed
// barrel offset, the definition declared after the barrel pointers and a non-volatile unit index all score the same
// or more, and the fast check warns that this function builds differently in the reduced image.
// Round 149 (m3 lane 16, plain): main's volatile on the unit index dropped 648 (2506 bytes), but aligned exact 287 -> 232
// and register-masked 110 -> 76: retail's NONE-first store to [esp+0x24], the 0x1c60 frame and the EDI unit index of the
// fired block all appear, so the plain local is retail's form. On it: the magazine definition declared before the magazine
// pointer (the order 0x1023d0 uses) 571 (2538 bytes; a declaration order, kept only with a match); `scale_a = 0.0f` in the
// not-fired arm 644; the 0x1058b0 side argument as `barrel_index ? 2 : 1` 646; the barrel pointer before the definition 651;
// both side arguments as ternaries 646. On 571: the not-fired `scale_a` store 611; the side ternary 606; rounds declared
// before burst 571 (same code). The magazine definition first with the volatile kept 616. Left on 571: EDI holds the
// definition and EBX the barrel offset in the prologue and the effect loop (retail: EBX the weapon index up to the 0xa7cd0
// call, EDI the barrel offset, the definition never in a register), and the magazine pointer is not materialized (retail
// spills it to [esp+0x34]).
// Holder retained through recoil, notification, effects and player 571 fast/608 plain;
// through notification 570; through redirected effects 636; notification and redirect only 637.
// Redirected holder restored after the effect 640; with ordinary owner query 572;
// ordinary owner query with separate effect target 640. Nested magazine admissions 639/639.
// NOT kept: the shared holder reaches retail length on the plain check but retains frame 0x1c64 and raises the opening score.
// Round 444: Reused initial loaded short/promoted count 636/639. Initial short with the fallback
// minimum read once 636. Post-heat short/promoted debit 639/639. Both loads with signed shorts,
// with promoted debit, or with named fallback minimum 639/639/639. Reduced control 636.
// NOT kept: ties and regressions. The early magazine tag and post-heat rounds still occupy separate
// spill homes in the full build, with the 0x1c64 frame and swapped weapon/barrel registers.
// Round 611. signed post-debit remainder 627, positive-rounds branch 636, saturated debit store 643, prefetched
// magazine flags 597, prefetched notification predicate 635, late notification bit 639, positive branch with signed
// remainder 627, shot result before debit 633. NOT kept: the flag snapshot moves the flag read and shortens the
// frame to 0x1c5c in the reduced image. The full frame and register roles still differ.
// Magazine record or tag names 636 each, immutable receiver bindings 636,
// separate record or tag references 636 each, positive notification branch 636,
// fallback rounds before burst reset 636, and nested fallback thresholds 636.
// Reduced control 636. The full frame and separate magazine-tag and round spills still differ.
// Typed holder-header accessor 636; unsigned type mask 636; typed Boolean type bit 632;
// conjunctive holder rejection 636; negated ammunition shortage 636; short debit store 636;
// negated heat exemption 636; ammunition/heat gate before the loaded-minimum gate 639.
// NOT kept: four-point best reduced gain against 636, with no length improvement. The full build
// still gives the magazine definition and post-heat rounds separate homes in frame 0x1c64.
// Magazine record or tag names 636 each, immutable receiver bindings 636,
// separate record or tag references 636 each, positive notification branch 636,
// fallback rounds before burst reset 636, and nested fallback thresholds 636.
// Reduced control 636. The full frame and separate magazine-tag and round spills still differ.
// M3 lane 5 r220 (fast with 0x101db0 and 0x159dd0 listed, which makes the screen agree with the full build at 603):
// ours keeps `barrel_index != 0` from the 0x105fa0 call in a stack temp for the 0x1058b0 call where retail recomputes the
// setne there (0x1036f0); the event arguments as constant selects (`failure ? (barrel_index ? 4 : 3) : (barrel_index ?
// 2 : 1)` and `barrel_index ? 2 : 1`) drop that temp, 600 (2538 B, frame still 0x1c64); with the effect target as a
// select in the 0xd03e0 call (retail's cmovne) 599 (2537 B); the target select alone 602. Retail's frame has no home
// for the magazine's rounds (ours spills it to frame +0x24 because EDX is needed for `mov dx, [esi+0x2a]`; retail keeps
// it in ECX, the magazine in EAX); the rounds named once for the opening tests 599 (same code). The unit index plain,
// read by const reference in the player helper 646 (2507 B). None reaches the bar; body unchanged.
// Shot/holder/effect, magazine/effects, result/scale and ammunition/result names 603 each.
// Paired tag/scale defaults 604, burst published after multiplication 638, signed minimum carried into fallback 625,
// shared player lookup at both queries 600. NOT kept: three-point best gain and the 0x1c64 frame remains.
// Shared shot notification 603; admission predicates producing the fired value 577;
// plain holder plus nested feedback tests 648; named short fallback minimum 603;
// pre-heat round predicate 603; separate magazine tag base 603; scoped owner rejection 603;
// short selector with tag before magazine 666. NOT kept: the 577 form stores fired before
// the magazine debit where retail stores it after admission and the magazine update.
// The short selector recovers the 0x1c60 frame but grows the body to 2566 bytes.
// Post-heat loaded short retained through the holder test, with native compound debit, signed balance, short
// difference, or word balance. The shared and separate threshold and holder gates all score 603. Frame 0x1c64
// and separate magazine-tag and round spills remain.
// Exhaustive burst assignments 577; shared post-wake accumulator 630; selected burst before round
// multiplication 639; separate round requirements 651; selected burst with short minimum 620;
// named shot multiplier or debit conversion 603/603; short pre-wake minimum 608.
// NOT kept: 577 defers the default burst store until after the wake call, which retail places before it.
// Split admission after balance, heat, the per-shot minimum or the holder test, then
// with separate balance and heat blocks, balance and minimum blocks, heat and minimum
// blocks, and separate heat, minimum and holder blocks: 603 each. NOT kept: all keep
// the extra frame word and the early magazine tag spill.
// The magazine tag and admitted round count still compete for frame homes.
// Late shot result with full/balance/heat/minimum/holder admission split 652/597/634/600/660,
// explicit no-magazine arms 652, explicit magazine result arms 646, default no-magazine success 648.
// NOT kept: the 597 form gains six, grows to 2541 bytes and still publishes fired ahead of the debit.
// Reduced control 636. Captured tag array indexed throughout 752; tag element reference 636;
// runtime barrel indexed throughout 598; runtime and tag indexed 732; runtime indexed with tag reference 598;
// runtime element reference 636, with indexed tag 752, with tag reference 636.
// NOT kept: full check of the indexed runtime form scores 645 and shortens to 2518 bytes.
// The 0x1c60 stack probe returns, but the barrel offset is formed after the wake call and still takes EBX.
// Zero-extended loaded count with signed threshold and separate or combined
// gate, signed or unsigned cost: word 603 each, dword 561 each.
// NOT kept: dword shortens to 2525 bytes, replaces retail's xor and word load
// with movzx and leaves the count at +0x50 and the frame at 0x1c64.
// Required rounds scoped to ammunition 603. Magazine selector by field
// reference 568. Loaded balance by field reference, both field references,
// or both scoped to ammunition 647/647/647. Late tag defaults 630, late scale
// defaults 674, both after scoped ammunition 665. Selector reference after
// the sentinel test 603. NOT kept: the 568 form shares the first selector load
// with sign extension where retail reads twice. The later binding restores
// those reads and ties. Required rounds and the original field reads restored.
// M3 lane 3 r301 (fast with 0x101db0 and 0x159dd0 listed, which agrees with the plain check; control 603). Reached 566
// (2538 bytes): `scale_a = 0.0f` in the not-fired arm (retail stores it there at 0x103462, as in the other two arms)
// 574; with the player-index helper's result preset and set under its test (retail's `or ecx, -1` at 0x10379c) 570;
// with the marker count as a long (retail sign-extends the call result once at 0x1038c3, then compares and divides a
// dword) 570; the three 566. On the kept form with the target select: the effect target as a select in the 0xd03e0
// call 566 (2537 bytes, no gain, not kept); the event arguments as constant selects 591; the notification written in
// each arm of the rounds test 603; the unit index plain 648 (2508 bytes, frame 0x1c60), with the magazine definition
// declared first 605 (2540 bytes).
// Review (r301): not kept. The not-fired `scale_a = 0.0f` repeats the declaration's zero (a redundant statement counts
// only with a match); without it the preset helper result and the long count read 598, under the 8-point bar, so
// main's body stays at 603. The 574/566 steps and the 555 set above are leads for a match.
// The closest structure reached (aligned difference count 285 against main's 349, register-masked 105 against 184,
// 634 instructions as retail): the unit index plain, the magazine definition declared before the magazine pointer, the
// barrel pointer declared right after the weapon (0x104150's order: the prologue then computes in retail's order and
// the homes are retail's, [esp+0x14] definition, [esp+0x18] barrel offset, [esp+0x24] unit index), the constant
// selects, the not-fired store, the target select, the notification in each arm, the preset helper and the long count:
// 555 (2525 bytes, frame 0x1c5c). It needs both declaration orders, which count only with a match; without them the
// same set reads 627 (645 without the notification form, and 638 / 626 / 632 / 625 / 625 less one form each).
// Steps toward it, all on the plain unit index: alone 648 (2506 bytes); the magazine definition first 571 (2538
// bytes; the fired block from 0x10355f is then retail's nearly instruction for instruction); the barrel pointer first
// on that 617, without the magazine order 651; on the barrel-first form the constant selects 627, the not-fired store
// 595, the target select 617, the three 632 (2545 bytes); on those the notification in each arm 629 (2533 bytes,
// frame 0x1c5c), the preset helper 630, the long count 628.
// What is left there is one entry role: retail loads the weapon index into EBX right after `push ebx`, keeps the
// barrel offset in EDI and never gives the definition a register; this build keeps the definition in EDI and the
// offset in EBX and reads the weapon index from its home. A second 0xb7360 call at the entry (a diagnostic: one more
// unconditional use of the weapon index) gives retail's set, the definition computed in ECX and stored, with the
// weapon index in EDI and the offset in EBX, 631; the same extra use in the fired block 623, at the tail 604 or as
// the notification in each arm does not move it, nor do one or two fewer definition uses in the fired block (645,
// 667) or the owner lookup written in place (same code; with fresh weapon lookups 626). The same call at the top of
// the effect block (a diagnostic) 507 (2541 bytes, frame 0x1c60): it stops the random-state pointer being loaded
// ahead of the effect tests, where retail loads it at each use.
// On the kept 566 form, none lower: the effect entry pointer formed ahead of the two barrel stores in the selection
// loop (retail loads the effect table before them) 631 (2524 bytes, frame 0x1c5c, register-masked 148 against 180);
// the shake amount of the 0x187510 call named before the call 566 (same code); `burst + 1` named before the two
// barrel stores 566 (same code); the effect mode as a long 611; the barrel pointer declared right after the weapon
// 568; the magazine definition declared first 649 (2551 bytes, frame 0x1c60).
// M3 lane 11 r157 (plain; control 603): ours keeps `barrel_index != 0` from the 0x105fa0 argument in a stack cell for the
// 0x1058b0 call (the extra frame word: `mov [esp+0x24], eax` in both arms, `inc` of the reload later); retail recomputes it
// there with xor/cmp/setne/inc. The second call's trigger as `barrel_index ? 2 : 1` 600 at 2538 B (aligned 349 -> 320; the
// reduced image then has retail's 0x1c60 frame, the full one still 0x1c64: the magazine pointer and the movsx'd loaded
// count still take two cells where retail keeps the magazine definition in EBX); that with the unit index not volatile 646
// at 2507 B (aligned 255, register-masked 153 against 238: structurally the closest form so far); both trigger arguments
// as ternaries 636 fast (same code as the first); the first as `(barrel_index != 0) + (failure ? 3 : 1)` 631 fast at 2512 B.
// The effect loop: retail hoists `movzx eax, word` for the mask test and reloads the mask (xor/mov) for the `|=`, ours
// hoists a 16-bit load and reuses it for the `|=`; compile-only, the test as dword/long/word-masked/operands swapped, the
// `|=` as an assignment or without its cast, and named mask/index/ticks fields in the barrel record: same code each (with
// no `|=` at all, diagnostic, the hoist becomes retail's movzx and the bit goes to EBX as retail). Body unchanged.
// M3 lane 11 r164 (H2M_FAST with 0x101db0, 0x159dd0, 0x101b10, 0x101b80, all MATCH; control 603). Behaviour fix: the
// marker-name call passed the weapon and barrel indices to 0x101b10 the other way round; retail sets up ECX = the barrel
// index and EDX = the weapon index there, the definition's order, as the matched 0x101b80 does (603, same count). Kept
// (593 at retail's 2533 B): the marker count as a `long` (retail sign-extends 0xb8d30's short result once and compares
// and divides the dword) 599 alone; with the second event call's trigger as `barrel_index ? 2 : 1` (retail computes
// `setne; inc` again there; ours kept the first call's `barrel_index != 0` in a stack cell) 596; with the first call's
// arguments as selects too 596; with the player-index helper as a single-exit result preset to NONE (retail's `or ecx,
// -1` there) 593 (both calls written as selects, so one spelling). Measured without parts: without the long count 597,
// without the second call's select 598; the effect target as a select in the 0xd03e0 call on the kept set 594, on the set
// without the long count 596. Reading (frame and colouring traces): ours has 17 dword cells against retail's 16, and
// the weapon index (a parameter) is never coloured (live across 93 unreferenced blocks to the tail's marker calls), where
// retail holds it in EBX from the entry to the fire call; no single forced allocator decision reaches retail.
// M3 lane 2 r147 (H2M_FAST with 0x101db0 and 0x159dd0, control 593; al = aligned differing rows exact/register-masked,
// 261/150 on this body): the fire call once after the if/else on `fired` 593 (same code; on the plain unit index 649, same
// code); the owner-unit lookup declared first 631 (2497 B); the effect's remaining count stored in each arm of
// `lower == upper` (retail stores in both arms and re-reads the field for the loop test at 0x10338b) 641 (2518 B, frame
// 0x1c5c, al 284/163), with the plain unit index 657 (2502 B, al 227/129); the plain unit index alone re-measured 649
// (2503 B, al 208/95, the closest structure: the definition and the barrel offset trade both register and home, EDI and
// [esp+0x18] against retail's [esp+0x14] and EDI). Body unchanged.
// Named accumulated heat 593; shared tag cap 612; merged cap publication 604; native barrel accumulator 593.
// Native item flag 629; chained failure flags 593; nested destruction test 593; named no-magazine heat rejection 593.
// NOT kept: ties or regressions. The 0x1c64 frame and early weapon/tag register assignments remain.
// Holder, magazine, effect tags, selected entries, owner and barrel role-name trials 630 each (reduced control 630).
// Shared effect-history word setter 627, pending weapon byte setters 630, both 627. NOT kept: a three-point
// preview change. The full probe still differs by four bytes and the weapon index and barrel occupy different registers.
// Fallback-produced balance, admission or initial balance 621/623/640. Conditional fallback cost,
// burst or both 593/639/639. Separate admitted cost 547, or 545 with path-produced admission.
// NOT kept: both forms have retail length but carry the required rounds in EDI before the magazine check.
// Retail keeps one stack home through debit. The 547 form keeps frame 0x1c64 and the 545 form grows it to 0x1c68.
// The 0x1c64 frame and weapon/barrel register assignments still differ. NOT kept: cached history drops
// the publication reload, the local duration keeps a joined timer store and register test, and both
// shorten the body. The positive timer exit restores the frame but regresses. history word for the free-
// effect scan 593; promoted history for the free-effect scan 593; exhausted history as a shared value
// 536; positive effect-timer exit 635; single bounded next effect 631; scan history and fresh
// publication word 593; interval result initialized from lower bound 526; active effect index before
// timer debit 593.
// Typed barrel history, slot/countdown, complete selection state, three scales, extra shots,
// shot count/sequence, continuation handle and all runtime fields: 630 each in the reduced image.
// NOT kept: all tie the reduced control, which has a 0x1c60 probe and 2542-byte extent. The full
// 0x1c64 probe, weapon/barrel register roles and separate magazine spills remain.
// Typed weapon-tag heat cap, destruction, misfire and holder feedback; typed barrel-tag recovery/heat,
// pitch curve and event kind; all fields together: 593 each. NOT kept: all tie. The 0x1c64 probe
// and separate magazine-tag and loaded-round spills remain.
// Positive debit balance, explicit empty result, staged reload decision, and short-circuit reload
// after debit: 593/593/590/593. With the ordinary owner index: 649/649/607/629.
// NOT kept: the staged reload decision gains three at retail length but retains the 0x1c64 probe.
// The ordinary-owner forms recover 0x1c60, shorten the body, and raise the score.
// GA round 9 (lead held as a patch, not kept; the allowed forms alone, the volatile dropped, the existing
// unit_get_player_index, one `if (fired)` call and the field names, give 667, so the body stands; plain verified 593 -> 2, 2533/2533 B, frame 0x1c60, 0 masked diffs, 0 peer losses): dropped volatile on
// unit_index; replaced unit_player_index_102fb0 with existing unit_get_player_index (0x14de90); declared barrel right
// after weapon, unit_tag_index before tag_index, rounds before burst, and magazine_definition before magazine; moved
// `if (fired) function_a7cd0(weapon_index);` once after the magazine if/else (557 -> 302); formed effect entry before
// barrel stores with lower == upper arms; named misfire `chance` (130 -> 117); set scale_a = 0.0f in !fired and mode = 2
// before barrel flag6/7 (76 -> 35, killing g_4e7408 hoists); incremented value06 before storing burst (115 -> 76, restoring
// `lea edx, [ecx + 1]`); typed weapon+0x16c as a word bitfield union on s_weapon and owner_unit via WEAPON_UNIT_GET
// (35 -> 2); passed the 0x187510 kick through point2f (recovering frame 0x1c60 and [esp+0x44] spill; without it 159).
// Tried for the last 2 SIB bytes (all score 2): offset+ptr, idx[arr], dword cast, header local, barrel inside if,
// forceinline recoil helper before/after, function-scope updated/object_data, shadow block, no updated_definition local,
// objects local; non-inline/deferred inline helpers stayed out of line (846-850); without chance 18.
// Pre-heat loaded snapshot short/long 593/621; separate pre- and post-heat snapshots short/long 593/621;
// post-heat short for the minimum and debit 593; one short refreshed across both samples 593; debit
// through the post-heat short 593, with its clamp also 593 at 2521 bytes. The 0x1c64 probe and magazine spill remain.
// M3 lane 13 r159: GA round 9's held lead rebuilt from its recorded edits, plain check 2 (2533 B, +0x5cd); the reduced
// image reproduces it only with the file's callees listed beside it (alone it reads 539 at 2552 B). Reading (operand
// ranks): after the 0x105fa0 call the re-lookup's scaled index (an entry temporary) outranks the pool data load, and the
// barrel offset temporary outranks the `updated` pointer, so both addresses come out index-first. On the lead: no
// `updated` local (two lookups) 2, no `updated` or definition local 2, a barrel pointer formed from `updated` 2, the
// definition read twice through `updated` 2, `updated` const 2, the weapon definition named before the barrel's 2;
// diagnostics (reverted at once) with 1-8 dead initialised locals at the top 2 each (neither address flips). Main's
// body restored.
// M3 lane 8 r243 (H2M_FAST with 0x101db0, 0x159dd0, 0x101b10 and 0x101b80, MATCH each time; control 593): GA round 9's
// held lead rebuilt on this base reads 2 (the two post-0x105fa0 addresses index-first). Operand ranks read per form:
// each address is one sum sorted by descending rank after the global optimiser; the weapon-index offset and the barrel
// offset shared with the entry are optimizer temporaries numbered by the distinct intermediate-code expressions met
// before the re-lookup (here 0x361 and 0x36a), and retail's two orders both need that number about 150 higher.
// Diagnostic, not source: N initialised dead locals on the lead, at the top, before 0xb7360, before the effect block,
// before 0x105fa0 or right before the re-lookup: N = 8, 128, 144 give 2, N = 160, 176, 192, 208, 224, 240, 256, 320,
// 384 MATCH (2533 B); 256 after the fired block 1. Natural forms on the lead: the random draws with file and line
// arguments 2; the existing weapon_get_102e10 at both weapon lookups 2, at the re-lookup alone 2; the 0x105fa0 state
// named as a local 2. So retail's body holds about 150 or more further distinct expressions ahead of the re-lookup that
// fold away (inlined helpers whose code is removed are the likely source; 0x100130 above needs the same kind of
// surplus). Body unchanged.
// M3 lane 7 r236 (H2M_FAST with the same four callees, MATCH each time; control 593). The one-caller hand copy of the
// player-index lookup replaced by the existing unit_get_player_index (0x14de90): 593, identical code (kept). Operand
// ranks on GA round 9's 2 form: index temporary 0x1d840 against the pool data's 0x18008 (the data load only takes even
// levels, at most 0x1c008), barrel temporary 0x1da80 against `updated` 0x10380, so both need the temporaries' numbers to
// wrap past 0x400: 159 to 163 more distinct expressions ahead of the re-lookup with no new own locals (each own local
// declared ahead of `updated` widens the top by half). Measured on the lead: the existing weapon_get_102e10 at the first
// lookup +3 (and the data load drops to level 0), file and line arguments in the random draws +0, `updated` renamed
// (three names) +0. On main's body: the unit-index volatile dropped 649 (2503 B); the +0x16c status bits as a word
// bitfield 594; the owner's player through WEAPON_UNIT_GET 593; one `if (fired)` call after the magazine if/else 593;
// the four allowed forms together 656 (2495 B).
// M3 lane 7 r237 (h2m fast with 0x101db0, 0x159dd0, 0x101b10 and 0x101b80, MATCH each time; control 593): single
// pieces of GA round 9's held lead on main's body, each a declaration order or a named part (kept only with a match):
// unit_tag_index declared before tag_index 592; rounds declared before burst 593; the barrel pointer right after the
// weapon 595; the magazine definition pointer before the magazine pointer 570 (2546 B, first difference +0x13); the
// misfire threshold named `chance` 592.
// @retail 0x102fb0
void __stdcall function_102fb0(long weapon_index, short barrel_index)
{
    s_weapon *weapon = WEAPON_GET(weapon_index);
    s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
    s_weapon_barrel_definition *barrel_definition = &definition->barrels[barrel_index];
    s_weapon_barrel *barrel = &weapon->barrels[barrel_index];
    volatile long unit_index = weapon_get_owner_unit_index(weapon_index);
    long tag_index = NONE, unit_tag_index = NONE;
    real scale_a = 0.0f, scale_b = 0.0f;
    bool fired = false, failure = false;
    long burst = 0;
    long rounds = barrel_definition->rounds_minimum;
    function_b7360(weapon_index);
    if (WEAPON_REAL_AT(barrel, 0x24) >= 1.0f)
    {
        burst = (long)floor(WEAPON_REAL_AT(barrel, 0x24));
        rounds *= burst + 1;
    }
    if (barrel_definition->magazine_index != NONE)
    {
        s_weapon_magazine *magazine = &weapon->magazines[barrel_definition->magazine_index];
        s_weapon_magazine_definition *magazine_definition = &definition->magazines[barrel_definition->magazine_index];
        if (magazine->rounds_loaded < rounds && magazine->rounds_loaded >= barrel_definition->rounds_minimum)
        {
            burst = 0;
            rounds = barrel_definition->rounds_minimum;
        }
        if ((magazine->rounds_loaded >= rounds || TEST_FIELD_BIT(barrel_definition->flag2)) &&
            (!TEST_FIELD_BIT(definition->flag11) || !(weapon->heat >= 1.0f) ||
                function_106030(weapon_index)) && magazine->rounds_loaded >= barrel_definition->rounds_per_shot &&
            (!TEST_FIELD_BIT(definition->flag19) || unit_index == NONE ||
                !(((1 << ((byte *)g_4e0300->data)[(unit_index & 0xffff) * 12 + 3]) & 2)) ||
                !(function_d1210(unit_index) > 0.0f)) && !barrel->flag2)
        {
            if (TEST_FIELD_BIT(((s_weapon_item_flags_view *)weapon)->flag3)) magazine->rounds_loaded -= (short)rounds;
            if (magazine->rounds_loaded <= 0)
                magazine->rounds_loaded = 0;
            else if (magazine_definition->flags & 2)
                function_105a80(5, weapon_index, barrel_definition->magazine_index);
            fired = true;
            function_a7cd0(weapon_index);
        }
    }
    else if (!TEST_FIELD_BIT(definition->flag11) || !(weapon->heat >= 1.0f))
    {
        fired = true;
        function_a7cd0(weapon_index);
    }

    if (barrel_definition->firing_effect_count > 0)
    {
        if (WEAPON_SHORT_AT(barrel, 0xc) <= 0)
        {
            short original = WEAPON_SHORT_AT(barrel, 0xa);
            short effect_index = original;
            if (TEST_FIELD_BIT(barrel_definition->flag1))
                effect_index = (short)((long)_random(&g_4e7408->unknown0, NULL, 0) % barrel_definition->firing_effect_count);
            do
            {
                if (WEAPON_WORD_AT_102FB0(barrel, 8) == ((1 << barrel_definition->firing_effect_count) - 1))
                    WEAPON_WORD_AT_102FB0(barrel, 8) = 0;
                do
                {
                    ++effect_index;
                    if (effect_index >= barrel_definition->firing_effect_count) effect_index = 0;
                } while (WEAPON_WORD_AT_102FB0(barrel, 8) & (1 << effect_index));
                WEAPON_SHORT_AT(barrel, 0xa) = effect_index;
                WEAPON_WORD_AT_102FB0(barrel, 8) |= (word)(1 << effect_index);
                byte *entry = (byte *)barrel_definition->firing_effects + effect_index * 0x34;
                short lower = WEAPON_SHORT_AT(entry, 0), upper = WEAPON_SHORT_AT(entry, 2);
                WEAPON_SHORT_AT(barrel, 0xc) = lower == upper ? lower :
                    (short)(lower + ((_random(&g_4e7408->unknown0, NULL, 0) * (upper - lower)) >> 16));
            } while (WEAPON_SHORT_AT(barrel, 0xc) <= 0 && effect_index != original);
        }
        --WEAPON_SHORT_AT(barrel, 0xc);
        byte *entry = (byte *)((byte *)barrel_definition->firing_effects + WEAPON_SHORT_AT(barrel, 0xa) * 0x34);
        if (WEAPON_REAL_AT(definition, 0x25c) > 0.0f && WEAPON_REAL_AT(definition, 0x25c) < 1.0f && weapon->heat > WEAPON_REAL_AT(definition, 0x25c))
            if ((weapon->heat - WEAPON_REAL_AT(definition, 0x25c)) * WEAPON_REAL_AT(definition, 0x260) / (1.0f - WEAPON_REAL_AT(definition, 0x25c)) >
                function_x82e52f(&g_4e7408->unknown0, NULL, 0))
                failure = true;
        short mode;
        if (!fired) {
            barrel->flag6 = true;
            barrel->flag7 = true;
            mode = 2;
            scale_b = 1.0f;
        } else {
            scale_b = WEAPON_REAL_AT(barrel, 0x10);
            if (failure) {
                mode = 1;
                scale_a = 0.0f;
            } else {
                mode = 0;
                if (WEAPON_REAL_AT(definition, 0x154) == 0.0f)
                    scale_a = 0.0f;
                else
                    scale_a = WEAPON_REAL_AT(weapon, 0x180) / WEAPON_REAL_AT(definition, 0x154);
            }
        }
        tag_index = WEAPON_LONG_AT(entry, 8 + mode * 8);
        unit_tag_index = WEAPON_LONG_AT(entry, 0x20 + mode * 8);
    }

    if (fired)
    {
        if (burst) WEAPON_REAL_AT(barrel, 0x24) -= (real)burst;
        WEAPON_SHORT_AT(barrel, 0x2c) = (short)burst;
        barrel->value06 += (short)(burst + 1);
        ++WEAPON_BYTE_AT(barrel, 0x2e);
        WEAPON_LONG_AT(weapon, 0x248) = g_510c54->game_time;
        function_105fa0(weapon_index, failure ? (barrel_index ? 4 : 3) : (barrel_index ? 2 : 1));
        s_weapon *updated = WEAPON_GET(weapon_index);
        s_weapon_barrel_definition *updated_definition = &WEAPON_DEFINITION(updated)->barrels[barrel_index];
        if (updated_definition->value_9c > 0.0f && !TEST_FIELD_BIT(updated_definition->flag4))
            WEAPON_REAL_AT(&updated->barrels[barrel_index], 0x14) = 1.0f;
        if (WEAPON_REAL_AT(barrel_definition, 0xa0) > 0.0f) WEAPON_REAL_AT(barrel, 0x18) = 1.0f;
        if (g_4e6948->state == 2)
        {
            long owner = function_101fb0(weapon_index);
            if (owner != NONE)
            {
                long player = WEAPON_LONG_AT(WEAPON_GET(owner), 0x13c);
                if (player != NONE) function_1628f0(player, 1);
            }
        }
        if (unit_index != NONE && WEAPON_LONG_AT(WEAPON_GET(unit_index), 0x13c) != NONE)
            function_d0e60(unit_index, WEAPON_REAL_AT(definition, 0x27c), WEAPON_REAL_AT(definition, 0x280));
        WEAPON_REAL_AT(weapon, 0x180) += WEAPON_REAL_AT(barrel_definition, 0xa4);
        if (!TEST_FIELD_BIT(((s_weapon_item_flags_view *)weapon)->flag3) && WEAPON_REAL_AT(weapon, 0x180) > WEAPON_REAL_AT(definition, 0x154))
            WEAPON_REAL_AT(weapon, 0x180) = WEAPON_REAL_AT(definition, 0x154);
        else if (WEAPON_REAL_AT(weapon, 0x180) > 1.0f)
            WEAPON_REAL_AT(weapon, 0x180) = 1.0f;
        if (!TEST_FIELD_BIT(definition->flag26))
            function_101db0(weapon_index, barrel_definition->value_a8);
        function_1058b0(weapon_index, barrel_index ? 2 : 1, false);
        if (!failure)
        {
            barrel->flag1 = true;
            WEAPON_BYTE_AT(weapon, 0x16c) |= 1;
            function_bfc30(unit_index, WEAPON_SHORT_AT(barrel_definition, 0x36), true);
        }
        if (unit_index != NONE && unit_tag_index != NONE)
        {
            long redirected = function_c7100(unit_index);
            long target_102fb0 = unit_index;
            if (redirected != NONE) target_102fb0 = redirected;
            function_d03e0(target_102fb0, unit_tag_index);
        }
        if (definition->reload_style == 3 && barrel_index == 1) WEAPON_BYTE_AT(weapon, 0x16c) |= 8;
        if (unit_index != NONE)
        {
            if (unit_get_player_index(unit_index) != NONE)
            {
                long player_slot = *(short *)(g_4e8c24->data + (unit_get_player_index(unit_index) & 0xffff) * 0x21c + 0x28);
                if (player_slot != NONE)
                {
                    real value = function_17ca10(WEAPON_REAL_AT(barrel, 0x20), WEAPON_SHORT_AT(barrel_definition, 0xc0));
                    function_187510(player_slot, 0.0f, WEAPON_REAL_AT(barrel_definition, 0xb0) +
                        (WEAPON_REAL_AT(barrel_definition, 0xb4) - WEAPON_REAL_AT(barrel_definition, 0xb0)) * value);
                }
            }
            function_c86e0(unit_index, true);
        }
    }

    if (WEAPON_REAL_AT(weapon, 0x180) > WEAPON_REAL_AT(definition, 0x158) &&
        WEAPON_REAL_AT(definition, 0x15c) > function_x82e52f(&g_4e7408->unknown0, NULL, 0))
        function_104030(weapon_index);
    barrel->timer = 0;
    if (WEAPON_BIT_AT(barrel_definition, 0, 12))
    {
        s_object_marker markers[64];
        long marker_name = function_101b10(barrel_index, weapon_index);
        long owner_index = function_101ec0(weapon_index);
        long count = function_b8d30(owner_index, marker_name, markers, 64, false);
        if (count >= 1)
        {
            long selected = WEAPON_BYTE_AT(barrel, 0x2e) % count;
            long model_owner_102fb0 = function_101ec0(weapon_index);
            s_effect_marker marker;
            marker.name = marker_name;
            marker.position = markers[selected].matrix.position;
            marker.forward = markers[selected].matrix.forward;
            function_176970(NULL, scale_b, tag_index, model_owner_102fb0,
                markers[selected].node_index, 1, &marker, scale_a, NULL, NULL, false);
        }
    }
    else
        WEAPON_LONG_AT(barrel, 0x30) = function_1039a0(weapon_index, tag_index, WEAPON_LONG_AT(barrel, 0x30), scale_b, scale_a);
}


real function_d1490(long object_index, long child_index);
short first_person_weapon_animation_ticks(long weapon_index, long animation_name, short type);
void function_10d4e0(long item_index);
void function_177260(long effect_index, bool flag);
bool function_177610(long effect_index);
bool function_102100(short barrel_index, long weapon_index, bool *finished, bool *blocked);
void __stdcall function_bf600(long user, real frame, s_animation_frame_event const *event);
void __stdcall function_ba7f0(long object_index, long region_index, long bit_index, long bit_mask);

PRIVATE __forceinline void weapon_request_barrel(long weapon_index, short barrel_index, bool test_ready)
{
    if (barrel_index != NONE && (!test_ready || function_102540(weapon_index, barrel_index, false)))
        WEAPON_GET(weapon_index)->barrels[barrel_index].flag0 = true;
}

PRIVATE __forceinline void weapon_clear_trigger(long weapon_index, short trigger_index)
{
    s_weapon_trigger *trigger = &WEAPON_GET(weapon_index)->triggers[trigger_index];
    trigger->state = 0;
    trigger->timer = 0;
}

PRIVATE __forceinline long weapon_visible_model_owner(long weapon_index)
{
    s_weapon *weapon = WEAPON_GET(weapon_index);
    s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
    byte header_flags = ((byte *)g_4e0300->data)[(weapon_index & 0xffff) * 12 + 2];
    if ((header_flags & 0x10) || (WEAPON_LONG_AT(weapon, 4) & 1) || definition->model_index == NONE)
        if (weapon->parent_index != NONE) weapon_index = weapon->parent_index;
    return weapon_index;
}
// score 1412 (was 1412): Frame 0x34 versus 0x38. Retail keeps the pool and header offset in EDI and ESI, and computes the scale complement in the two barrel arms.
// Round 145 (m3 lane 16): blocked/down/up as bool (retail's byte homes at [esp+0x14..0x16]; no 16-bit stack homes in retail)
// with the definition flag 27/10 tests read from the definition's named bits and the object flag 8 test through the file's
// WEAPON_BIT_AT (all unsigned: retail's shr; the named bits and WEAPON_BIT_AT give the same code) 1412 plain at 5684
// bytes (kept; frame 0x3c -> 0x34, register-masked aligned 680 -> 518); bool alone 1414, the bit tests alone 1418,
// down alone 1424, up alone 1430, blocked alone 1414, down/up 1431, flag 27/10 only 1413, flag 8 only 1413 (all fast).
// On 1412: word bitfields for the 0x16c status and 0x16e control bits 1444 (status only 1438, control only 1434), the same
// read through TEST_FIELD_BIT 1510/1504/1448; control bit 7 through the 0x16e flag view or a word read 1412 each (same code);
// a bool result 1494 (5616 bytes; retail keeps it in BL with the home at [esp+0x13]); down/up started false before the
// timer test and set under `if (!blocked)` as at retail 0xfe633 1422 (5668 bytes); primary/secondary read from the
// trigger definition at each use 1544 (5800 bytes); both 1519 (register-masked 444/488 against 518/523).
// Earlier score 1413 (was 1420): the two definition flag tests read the bit through the field (flag27 at 0xfe02c, flag10 at
// 0xff128), the blocked and down/up carriers are bytes and the pressed and released bits are taken with retail's shift
// form, so the function has retail's length (5684 bytes). What still differs is the frame (0x34 against retail's 0x38),
// the prologue's register homes (the pool pointer in EBX where retail keeps it in EDI, the index in EDI where retail keeps
// it in ESI, and dual spilled where retail holds it in BL) and, with them, every later frame offset.
// MiniMax B (11-14): lane B pass over 0xfdfb0; the byte carriers follow retail's own byte slots (result 0x13, down 0x14,
// up 0x15, blocked 0x16, pressed 0x1c-0x1e, released 0x20-0x22). Not kept: the result as a bool (1490), the retail
// shaped down/up split with the two carriers zeroed before the timer test (1426), the pressed and released arrays as
// bytes (1413, unchanged), the complement inlined at each use site (1436).
// Earlier score 1420: walked statement by statement against retail 11-05 (w1A), with the jump tables at 0xff57c/0xff59c/
// 0xff5b4/0xff5c4/0xff5d4 mapped. No behaviour difference: the setup, animation state, ramp, heat and overheat, trigger
// and barrel state machines, barrel scale/ramp and the final heat/byte/x87 update agree, and all 77 call sites exist in
// both. The code is unchanged here, since the forms retail shows together score 1485 (5724 bytes), although the
// opcode shape (registers ignored) then matches 950 of 1471 instructions instead of 789: result a bool in a byte
// (0xfe0b5, 0xff56b); no hoisted `1.0f - scale` (0xff479); the +0x20 ramp clamps against double constants (fld qword
// 0x45dcc0/0x45dbe0 at 0xff42d/0xff4c8; the +0x1c clamps stay float); blocked a bool cleared at entry and set inside its
// test (0xfe038, 0xfe5bf); down/up bool bytes zeroed first (0xfe633, 0xfe64d-0xfe674); press/hold/release not cleared
// before 0x1027b0 (0xfe996); primary/secondary re-read from the trigger entry at each use (0xfe6b9, 0xfe6eb movsx);
// trigger fields indexed off the weapon ([ebx+ebp+0x210]); flag words unsigned (shr at 0xfe02c); behaviour cases laid
// out 0 1 4 5 2 3 and trigger states 0 1 2 3/7 5 6; the request helper keeps the barrel pointer (lea +0x1a4 after the or).
// With those, what differs is homes (retail keeps the weapon in EBP and the trigger offset in EBX; that form folds &timer
// into EBP and spills the weapon; retail's loop index lives at [esp+0x28]), retail's single tail for the flag3/flag4
// clear and the two 0x102f70 calls (emitted twice there), `or byte [ebp+0x16c], n` flag stores (load/or/store there) and
// the frame (0x38 against 0x30). Also tried on that form: a word-typed 0x16c flag view for retail's shr tests 1535; case
// 4/5 as separate primary/secondary paths 1580. Retail reads the trigger and barrel state bytes signed (movsx at 0xfe680,
// 0xff0af), and the trigger definition's barrel selectors are the words at +0x8 (read at 0xfe6b9) and +0xa (the struct
// names only +0xa, as primary_barrel).
// Earlier score 1420: Frame 0x3c/0x38 and pool/offset/dual/scalar homes remain.
// Earlier score 1420 (was 1420; round 449 reached 1408 but is not kept, see the review line): Frame 0x3c versus 0x38 and pool/dual/scalar homes remain.
// Earlier: frame 0x3c versus 0x38; pool/offset registers and dual/scalar homes remain.
// Long blocked fast 1518, barrel active 1513, down/up 1506, overheated 1520; depth one 1521;
// combined blocked/down/up 1470 plain (fast 1500).
// Byte result fast 1505; long result 1475 fast/1476 plain (5684 bytes); long dual 1507;
// input-array names 1507; barrel completion case order 1507. Full-width result kept.
// Own plain 1476; later table relocation 1450 with identical normalized code.
// Trials:
// Frame 0x3c versus 0x38; pool/offset ownership and dual/scalar homes remain.
// Earlier 1500 (was 1496): frame now 0x38; entry header/index roles and scalar homes still differ.
// Earlier 1475 (was 1500): frame 0x38 agrees; pool/index roles and scalar homes remain.
// Earlier 1436 (was 1475): frame 0x3c versus 0x38; pool/offset roles and scalar homes remain. Trials:
// word result 1474; word dual 1464; word inputs 1447; cooling arms 1481; shared complement 1442;
// long ticks 1477; long direction 1476; complement plus bits 1441; complement plus tag arms 1441;
// double tests 1480; float multiplier 1442; upper ternary 1478; complement plus word inputs 1436;
// packed bits/tag arms 1438; packed bool result 1477; packed word result 1436.
// Coefficient lifetimes: before magazines 1446; before barrels 1447; per barrel 1447; before blend 1447.
// Native trigger input 1447; native barrel selectors 1630; word input tables 1439; call-free blend tick sample 1440;
// weapon object reference 1436.
// Pool retained 1460; readonly tags 1436; long overheat on word-input/coefficient frame 1439;
// word/long barrel activity on that frame 1445/1445. Wide loop counters 1439/1680/1442 rejected (input range).
// Earlier 1436 (was 1475): barrel/input setup still differs. Coefficient lifetime trials: before
// magazines 1446; before barrels 1447; per barrel 1447; before blend 1447.
// Earlier 1436 (was 1436): Frame 0x3c versus 0x38 and pool/offset roles remain. Native trigger input
// 1447; native barrel selectors 1630; word input tables 1439; call-free blend tick sample 1440;
// Round 109: Dual query before lookups 1466; tag after dual query 1448; query outputs without defaults 1455; state-one outputs before blocked test 1436; native flag27 only 1435, flag10 only 1436, both 1435; long result declared first 1436.
// Input/trigger phase scope, phase-scoped target outputs, function-scoped target outputs,
// signed-byte input tables and persistent role names each 1436. Original body restored.
// Round 115: Native status fields 1470; native input fields 1455; both 1466; magazine activity from its test 1455; dual/owner setup scope 1436; signed trigger/barrel/both state bytes 1436/1436/1436 (fast). Opening body/type declarations restored.
// Existing header getter initially/throughout 1436/1436; header-data receiver 1447;
// long masked slot 1470; header receiver plus slot 1459; getter plus slot 1470;
// dword/word masked slot 1470/1434. Two-point word gain rejected; original lookups restored.
// Branch-owned three query outputs 1436; direct barrel selectors 1537; direct input selector 1448;
// all direct tag selectors 1542; integer input tables 1457; branch outputs/all selectors 1540;
// unsigned-byte effect decisions 1436; branch outputs/integer tables 1457. Original body restored.
// Round P208: ordinary inline request/reset/visible-owner helpers 1503/1436/1436;
// request-reset/request-owner/reset-owner/all inline 1503/1503/1436/1503;
// direct native input-table initialization 1436. Original helper declarations and body restored.
// Existing visible-owner result, with request guard/reset chain/both 1436/1436/1436/1436;
// each graph with Boolean activity 1477/1477/1477/1477. All helpers and body restored.
// Round 228: unsigned-word blocked/down/up carrier subsets 1436 each; all three
// with unsigned native activity 1436. Frame 0x3c and initial spills remain; all restored.
// Round 202: Solver-owned finished/stopped outputs, barrel/function scope, output role names,
// reversed declaration order in both scopes and native byte activity each 1448; both queries
// without output defaults 1465. Original defaults, lifetimes and all game-call tuples restored.
// Round 243: Player default before scale override, direct/valid candidate 1439/1420;
// before unit lookup 1442/1421; before dual query 1447/1428; before first lookup 1447/1428.
// Kept scale-override default plus valid candidate, plain 1420/5680; frame and pool carriers remain.
// Round 165: Dual/activity/owner inline-boundary subsets and native dual byte all 1420; positive
// result bool/byte/word 1469/1465/1422. Unsigned real input word alone 1416, with bool/byte/word
// result 1469/1465/1414; four/six-point gains rejected. Inner behavior/press/both/hold/behavior-
// hold/press-hold/all orders 1447/1450/1430/1421/1427/1465/1447; word-input/result with hold/all
// and word-input hold 1433/1449/1425. Original fields, result, cases and every inline
// specification restored; frame 0x3c versus 0x38 and pool/dual homes remain.
// Round 246: complementary activity result bool/byte/short/long 1440/1440/1459/1459; the same
// with entry declaration 1440/1440/1459/1459. Original persistent result restored.
// Round 280: complete four-way ramp/blend producer forms: named rates 1420;
// named steps 1420; next values with immediate stores 1420; bounded next values
// 1432; next values with rates/steps 1420/1420; bounded values with rates/steps
// 1432/1432. Original producer expressions and stores restored.
// Round 212: native animation/trigger/barrel activity-test accumulation, seven subsets and
// complete entry-zero result [1421, 1425, 1432, 1432, 1431, 1438, 1450, 1450]. Original
// producers restored; pool, dual and frame homes remain.
// Round 216: native word object slot plus flag27, with word activity/unsigned input/both,
// 1459/1459/1456/1470; flag10 counterparts 1459/1459/1456/1470. Complete original
// slot, flag reads, signed input field and long activity restored; frame 0x3c/0x38 remains.
// Native animation/channel fields, time receiver and existing channel getter subsets:
// 1420/1461/1461/1432/1432/1461/1461; complete graph with entry result workspace 1461.
// Every original input observation, helper and body restored; frame and pool/dual homes remain.
// Round 163: independent admission/tracked-target/owner-target query output
// scopes, seven subsets 1420 each; complete branch ownership with native event
// role names 1420. All original output scopes and names restored; frame 0x3c
// versus 0x38 and initial pool/offset/dual homes remain.
// Round 154 (fast): trigger fields written in full (no trigger pointer) 1577, alignment 1260 -> 1011;
// with the clear-trigger helper writing state/timer through one weapon local 1449 (5716 bytes,
// alignment 983); plus bool blocked 1470; plus bool down/up 1453 (alignment 940); plus `1.0f -
// scale` at each use 1483 (frame 0x40); switch cases in retail's jump-table order 1501. Retail's
// barrel request leaves a dead lea of the barrel base (+0x1a4, ours +0x1a8) and keeps blocked/down/up
// as stack bytes initialized to 0.
// Round 387: Frame 0x3c versus 0x38 and pool/dual homes remain. Shared magazine-trigger/trigger-
// barrel/magazine-barrel/all short ordinals 1420/1416/1416/1420; all at entry or with transient
// float names 1420 each; independent ordinal names with/without transient names 1420 each. The two
// 1416 forms reach 5684 bytes but are below the gain gate. Each paired graph with bool/byte/word
// activity 1469/1465/1426. Original body restored.
// Round 389: native reset pointer/reference, with original/readiness-first request formals,
// 1420 each; native working request index on those four graphs 1704 each. Original repeated
// helper contracts and all call arguments restored; frame and pool/dual homes remain.
// Round 404: Unsigned native input field with shared magazine-trigger/trigger-barrel/magazine-barrel/all
// ordinals, long/word activity pairs 1420/1417/1420/1417/1420/1417/1420/1417. Three-point leads rejected;
// complete original type, loops and body restored. Frame and pool/dual homes remain.
// Round 406: Frame 0x3c versus 0x38 and pool/dual/scalar homes remain. Complete tag reads at every
// use, crossed with barrel-phase complement lifetime, word activity and byte dual, grow to
// 6052-6068 bytes; both native 0x20 double-bound selections give 1479 at 5748 bytes. Scores
// 1842/1855/1865/1873/1842/1855/1865/1873/1479. All restored.
// Round 410: Complementary dual-query bool/byte/short/long 1454/1454/1446/1447;
// each with the adjacent owner query used directly 1454/1454/1446/1447. Original
// dual producer and owner-result lifetime restored; frame and pool/dual homes remain.
// Round 449: Boolean shift-and-bit observations for status/control/trigger,
// seven nonempty subsets 1416/1437/1437/1420/1420/1437/1441; all with byte
// blocked/down/up 1426. Status-only reaches retail length (5684) but gains
// only four, below the gain gate; original masks, signed test and carriers
// restored. Frame 0x3c versus 0x38 and initial pool/dual homes remain.
// Round 449 follow-up on status-only Boolean bits: byte blocked/down/both/up/
// blocked-up/down-up/all 1418/1420/1421/1427/1408/1433/1414; all Boolean
// carriers 1414. Byte blocked/up kept after plain confirmation at 1408,
// 5692/5684 bytes, frame 0x38 and first +8; down stays short. Both byte
// carriers remain canonical 0/1; input masks, signed trigger test, every field
// observation, helper, call tuple and float expression stay unchanged.
// Review of round 449 (m3): not kept: the 1408 form needs every flag bit read as `(bool)((byte >> n) & 1)`,
// a cast that changes no value, and blocked/up retyped short -> byte by score; both are kept only with a byte
// match. Code back to the masks and short carriers; the subset scores above still stand.
// Round 455: Actual persistent header element as pointer/reference, with entry activity declaration
// and readonly tag subsets: 1545 each, 5780/5684 bytes, frame 0x3c. Original complete body,
// lookup graph, activity declaration and metadata restored.
// Permuter, 40 minutes (11-05): 1369 only with volatile locals and casts that change no value. Not kept.
// MiniMax B (11-15), NOT kept: 1413 (was 1420), a 7-point gain, under the 8-point bar for a function above 30.
// Lane N round 411 (fast control 1420).
// Firing blend publication 1416 (NOT kept: four-point gain with the frame still larger).
// Recovery blend publication 1416 (NOT kept: four-point gain with the frame still larger).
// Both blend publications 1416 (NOT kept: four-point gain with the frame still larger).
// Both rate publications 1420.
// Tracked datum 1420.
// Owner player field 1420.
// Dual and blend homes 1420.
// Pool after animation 1420.
// Round 103 (m3 lane 15): both definition flag tests through WEAPON_BIT_AT (retail's shr) 1419; with blocked/down/up
// as bool 1413 plain at 5684 bytes, frame 0x34 against 0x38 (a gain of seven, and a retype); plus a bool result
// 1494; plus blocked cleared at entry and set in its test 1433; the owner-unit call nested in the scale call 1413.
// Not kept: none reaches the gate. Original body restored.
// Shared input word 1420. Shared input word with releases first 1420. Shared input word with shifted bits and
// releases first 1414. Shared cooling flags word 1432. Shared cooling flags with field-value rate selection
// 1420. Shared cooling flags with explicit rate branches 1420. One state-timer read for test and decrement
// 1420. Shared cooling and input words 1432. NOT kept: The shifted input word improves by six at 5684 bytes.
// The threshold is eight. The shared cooling flags enlarge the body to 5696 bytes, and neither rate
// selection improves the opening score.
// Round 149 (m3 lane 16, fast): retail has 13 dead barrel-base leas (`lea [..+0x1a4]` after `or byte [..+0x1a8], 1`)
// where this build has one; the inline request helper storing through a barrel element pointer gives 10 of them at 1412
// (same length; a one-use element pointer, kept only with a match). press/hold/release declared without initializers
// (0x1027b0 clears all three; retail stores nothing before that call) 1419 (5668 bytes); both 1419; the pointer with
// blocked started false at entry (retail's `mov byte [esp+0x16], 0`) and set in its test 1433; that with the
// uninitialized outputs 1440.
// Round 444. Magazine-only seeded accumulator as bool OR, byte OR, bool with an updater test,
// and bool logical OR 1415/1415/1414/1409. Independent contribution in the same four forms
// 1416/1416/1414/1420 (reduced control 1412). NOT kept: the three-point seeded logical gain
// leaves frame 0x34 and the opening pool, offset and dual-state homes.
// M3 lane 4 r229 (fast, control 1412 = plain; aligned = difflib count, masked = registers and stack slots masked too;
// control aligned 910, masked 521). Small forms on this body: the animation state's byte 0x11 read into a long ahead of
// the 0x14 test (retail's `movzx eax` at 0xfe121) 1429; the ramp sound tag as `increasing ? [+0x248] : [+0x250]` (retail
// loads each field in its own arm) 1412 (masked 517); both 0x18c ramp clamps as value selects (`x = x > t ? t : x` and
// `x = x > t ? x : t`: retail's shared `movss` copy at 0xfe2e0, that section then exact) 1427 (5668 B), with the tag
// arms 1411. A whole body in retail's forms (bool result; blocked false at entry and set in the block that clears +0x174;
// no complement local, `1.0f - scale` in the two barrel arms; the 0x16c and 0x16e words as word bitfields; down/up false
// first and filled under `if (!blocked)`; primary/secondary read from the trigger definition at each use through two
// by-value inline helpers (ready test, then the barrel's flag0 through an element pointer); the barrel selects of
// behaviours 4 and 5 as two arms each; behaviour cases 0 1 4 5 2 3, both action switches 0 3 1 2, states 0 1 2 3/7 4 5 6;
// `value_170 <= 0 && value_171 <= 0`; the barrel's +0x10 as the field at each use; finished/stopped ahead of the blocked
// test with the four 0x103e60 calls as an else-if chain; the +0x20 clamps against double 1.0 / 0.0 and the +0x1c clamps
// as value selects) with the trigger fields written `weapon->triggers[i].x` 1682 (5888 B, aligned 705, masked 306: the
// entry is then retail's, pool EDI, offset ESI, dual BL, but the flag word is addressed `[weapon + (i*3 + 0x84)*4]`
// because 0x210 divides by 12, so i takes ESI and the weapon leaves EBP); that with the trigger flags as a dword and mask
// tests 1709; with the trigger pointer local kept (this body's form) 1661 (5872 B, aligned 724, masked 293: retail's
// `[i*12 + weapon + 0x20e]` addressing, saved as build/r229/lead-fdfb0-reconstruction.patch). What is left on that form:
// the magazine loop hoists weapon_index into EBP (retail loads it from its home in the loop and keeps the weapon in EBP
// to the end of the trigger loop), so every trigger access reloads the weapon (about 180 bytes); retail's result sits in
// BL with a store to its home at each definition, here in memory only. Diagnostics on the 1661 form: without the magazine
// loop 1471 (5704 B, masked 222: the trigger loop is then retail's, weapon EBP, i*12 EBX, the definition entry ESI); the
// loop index `volatile` 1833; one function-scope index for the three loops 1661 (same code); `bool result = false;` as
// the first declaration 1661 (result then BL with retail's stores, weapon_index EBP from the entry); the eight scalars
// declared uninitialised at the top in seven orders 1667 each (same code: declaration order alone is no lever); the
// charge branch of behaviour 2 as an inline helper 1856. `(void)&weapon_index;` 1412 here and 1682 on the fields-in-full
// form, the magazine call's index read through a pointer to the parameter 1682 on that form (same code each: the read
// is forwarded). Re-measured: bool result alone 1494
// (5616 B, memory only, no BL), with its loop stores through a pointer to it 1490. Retail's frame, from the listing:
// dwords offset 0x24, loop index 0x28, definition 0x2c, player index 0x30, weapon 0x34, scale 0x38, target 0x3c, the
// x87 sum's old value 0x40 and an unreferenced cell at 0x44; pressed 0x1c, released 0x20; bytes result 0x13, down and
// finished 0x14, up and the barrel's active 0x15, blocked 0x16, target entry and stopped 0x17, press/hold/release
// 0x18-0x1a, and behaviour 4's control bit 6 stored (never read) at 0x1b. Two readings from compile-only screens of other
// bodies this round: a bitfield returned through a bool-returning inline function gives retail's `mov; shr; test 1` (a
// direct test of a narrow bitfield gives `test byte [mem], mask`), and a bool with two definitions that is tested at once
// keeps its store (retail's `shr al, 6; and al, 1; mov [esp + 0x1b], al; jne` at 0xfe7cf needs no qualifier). Body unchanged.
// Round 611. channel lookup helper 1417, typed animation state 1412, typed state with channel getter 1417, typed
// graph getter 1417, conditional animation lookup 1434, primary channel reference 1417, typed frame count 1417,
// nested animation-end test 1417. NOT kept: these animation-state forms leave the 0x34 frame, pool registers and
// spilled dual result in place.
// Blocked control or status shift tests 1415/1412, both shifts 1415,
// both shifts with branch assignments 1415, original tests with branch assignments 1412,
// the unblocked path first 1412, and scoped or read-only blocking receiver 1412 each.
// The frame remains 0x34 and the dual result still spills.
// Tracked-target snapshot 1412; typed owner player read 1412; separate magazine activity
// 1416, or seeded from prior activity 1415; call-first logical OR 1425; wide magazine
// accumulator 1412; narrow pre-magazine activity 1449; named animation receiver 1412.
// NOT kept: the entry pool/offset roles and the dual spill remain. The Boolean
// activity forms give no gain.
// The fixed-scale producer still does not recover the dual byte in BL.
// Nested dual branches or one scale selection 1412/1412, zero or unity default
// within the override 1438/1438, numeric Boolean scale 1442, distinct holder scale
// with conditional or exhaustive output 1435/1435, holder-first branches 1462.
// NOT kept: all retain frame 0x34 and the two branches and selection tie.
// Trigger/trigger tag/barrel/barrel tag lvalues 1412 each, paired trigger or barrel lvalues 1412 each;
// trigger cursor with separate ordinal 1584, barrel cursor with separate ordinal 1413.
// NOT kept: the trigger cursor recovers frame 0x38 but shortens the code by 168 bytes and still
// spills the dual-wield result. The barrel cursor leaves the entry allocation and frame different.
// Blocked control or status shift tests 1415/1412, both shifts 1415,
// both shifts with branch assignments 1415, original tests with branch assignments 1412,
// the unblocked path first 1412, and scoped or read-only blocking receiver 1412 each.
// The frame remains 0x34 and the dual result still spills.
// M3 lane 5 r220 (fast, control 1412): the 0x175 heat byte's product with the float constant 255.0f, no double cast
// (retail 0xff3fc `fmul dword ptr [0x45dc38]` reads a float 255.0) 1412 (kept: retail's constant, same count). On that:
// behaviour cases in retail's block order 0 1 4 5 2 3 1437 (5656 B); released declared before pressed (retail computes
// released[0..1] first) 1412 (same code); the +0x20 ramp clamps as selects against double literals (`x > 1.0 ? 1.0 :
// x`, `x <= 0.0 ? 0.0 : x`) 1483 (5748 B: the increasing clamp is then retail's x87 sequence at 0xff42d instruction for
// instruction and the frame is retail's 0x38), the decreasing one as `x > 0.0 ? x : 0.0` (retail's compare order) 1483;
// with a bool result (`|=` in the magazine loop) 1436 (5680 B, frame 0x34 again, the result memory-only where retail
// keeps BL with a home at 0x13; masked aligned rows 887 -> 855); also without the complement local 1427; also with the
// +0x1c clamps as float value selects 1432. The 64 bytes the double clamps add are about what the dword result stores
// cost over retail's byte ones, so the double clamps, a bool result and the arm-local complement belong together.
// Desired-state and handle-first transitions 1482/1477, named presence and absence 1412 each,
// shifted flag and eager flag join 1412/1409, Boolean XOR 1412, conditional transition 1450.
// NOT kept: three-point best gain. The pool, offset and entry result homes still differ.
// Opening-only pool 1412; retained header slot 1449; direct owner player result 1438;
// early blocking state 1433; Boolean activity set from magazine calls 1494 or call-first OR 1464;
// arm-owned blends 1436, with opening pool 1440. NOT kept: the pool-only form ties;
// Boolean activity and arm-owned blends shrink the frame to 0x30 and leave the dual spill.
// First +0x2 reserves 0x34 against retail's 0x38. The pool and header offset take EBX and EDI. Retail uses EDI and ESI, leaving BL available for the dual query and, later, the animation activity. Hold the early work result as a byte through each actual activity phase before widening to the existing long accumulator.
// Boolean activity through animation 1410.
// Byte activity through animation 1410.
// Boolean activity through initial timer 1413.
// Byte activity through initial timer 1413.
// Boolean activity through ramp 1428.
// Byte activity through ramp 1428.
// Boolean activity through heat 1431.
// Byte activity through heat 1431.
// NOT kept: The animation-only forms gain two at retail length, below the keep threshold. The wider spans raise the score. All retain the 0x34 frame, the pool and offset registers and the early dual spill. The original activity accumulator is restored.
// The pool and header displacement still occupy EBX and EDI. Dual remains spilled.
// Stable magazine, trigger and barrel count values 1418/1416/1417. The trigger
// count gives frame 0x38 and first difference +0x8 at 5680 bytes. Live count
// references individually, beside each walk or acquired together 1412 each.
// NOT kept: retail reloads the bounds. Stable values raise the score and the
// live references retain the 0x34 frame.
// Persistent weapon/tag names, player/weapon names, weapon/scale names, and tag/player/weapon/scale names,
// target/heat names, trigger outputs/heat names, barrel stop outputs/activity names, and all phase names: 1412 each.
// Flag-first selected barrel value 1423, tag field reference 1578, reference with shifted control bit 1577,
// and separate selected-barrel assignments 1426. NOT kept: the names leave the 0x34 frame and early dual spill.
// The flag-first forms grow the body and allocate 0x34 with a value or 0x3c with a reference.
// Shared idle/charged trigger release, seeded from the input edge, joined tests,
// selected source or early positive edge: 1559/1399/1411/1404, also after the
// blocked exit with the same scores. NOT kept: the 1399 form shortens to 5680
// bytes and retains frame 0x34. The 1559 form recovers 0x38 but grows to 5824.
// Player presence from an owner-path assignment 1429, short-circuit
// tests 1422, absence 1422, explicit owner arms 1422, or typed owner lookup
// for presence/absence 1422/1422. Shared trigger test from the signed player
// index 1419 or from a selected index 1442. NOT kept: all regress and
// hold a 0x30 frame against retail's 0x38. The signed player index is restored.
// M3 lane 3 r301 (fast, control 1412): lane 4's saved reconstruction re-measured on today's file 1661 (5872 bytes).
// On it, diagnostics for the EBP contest between the weapon index and the weapon pointer: one fewer unconditional use
// of the index at the entry 1686, another 1661, both 1675; the animation offset read from the weapon pointer instead
// of a fresh lookup 1664 (5856 bytes), with both 1670: the weapon index keeps EBP from the entry in all of them. On
// this body: the barrel's stop tests as an else-if chain with finished/stopped declared ahead of the blocked test
// 1437 (5704 bytes), the chain alone 1436, the animation offset from the weapon pointer 1477 (5652 bytes; retail
// looks the weapon up again there through the saved header offset), both 1466, the owner lookup nested in the scale
// call 1412 (same code).
// M3 lane 11 r157 (H2M_FAST; control 1412 at 5684 B, aligned 1450, register-masked 1158): retail holds the 0x105e80
// result as a byte (BL, stored to its byte cell at each set and read back at the return); `bool result` 1494 (5616 B).
// The flags word at weapon+0x16c: retail tests its bits with a byte load and shr/test and sets/clears them with byte
// memory RMWs (one word load at the overheated read); a word bitfield record there (eight one-bit fields overlaid on the word)
// with every 0x16c access in this body through it 1419 (same length; the clears become retail's memory RMW, the sets a
// word RMW); byte bitfields 1412 (same code as the byte macro); word bitfields with the tests through TEST_FIELD_BIT 1487
// (5716 B); the 0xf9 clear as a word `&=` or two bit clears 1447 each. Retail branches with a load in each arm for the
// state-change tag and the cooling rate: `increasing ? WEAPON_LONG_AT(definition, 0x248) : WEAPON_LONG_AT(definition,
// 0x250)` 1412 (aligned 1447, masked 1153; retail's shape), the rate the same way 1417, both 1416 (aligned 1443, masked
// 1146). Body unchanged.
// M3 lane 11 r164 (H2M_FAST with 0x105e80; control 1412). Reading of the frame: it is under 0x80 bytes, so its cells keep
// the order they were made in (reference count, then the order each reached its count), and the dead argument home is
// tried first: our barrel-loop `active` byte takes the weapon index's argument home (ours holds the index in a register
// through the barrel loop, so its home is dead there); retail reads the index from its home in that loop and gives
// `active` the byte cell it shares with `up`. Retail's frame also has a byte result, a byte for behaviour 4's control
// bit and a cell for the heat byte's old value that ours does not, so the 0x34/0x38 gap is several cells, not one.
// Tried: the player default stored after the scale override (retail's NONE store follows the override's test) 1437;
// the heat byte's sum written `old + increment` 1412; the old value and the increment declared the other way round
// 1412; the tick clamp as an if before the store 1412; the 0x194 validity test as nested ifs 1412 (the 1412s the same
// code).
// M3 lane 2 r147 (H2M_FAST, control 1412; al = aligned rows exact/register-masked/stack-masked/both, 909/708/703/520): a
// bool result with down/up started false at the loop top and filled under `if (!blocked)` (pressed, released, then the
// signed state byte) 1506 (5600 B); with blocked started false at entry and set in its own test 1517 (5600 B); the +0x20
// ramp clamp tests against double literals 1453 (5724 B), with double stores too 1453 (same code); the first form with
// the ramp clamps as double selects, the +0x1c clamps as float value selects and the complement written in place 1444
// (5668 B, al 889/679/667/478). Body unchanged.
// Conditional decay result 1413; named cooling adjustment 1412; one cooling coefficient read 1416; retained cooled heat 1412.
// Single clamped write 1412; named recovery time or excess heat 1412 each; nested recovery guard 1412.
// NOT kept: ties and regressions. The 0x34 frame, entry roles and dual spill remain.
// Trigger/barrel runtime-array lvalues, tag pointer fields, paired lvalues, and array bases with
// tag field pointers: 1413/1412/1413/1413 before their loops, 1412 each inside. The loop-entry
// runtime array forms shorten the barrel phase by four bytes. The 0x34 frame and dual spill remain.
// Retail block order for trigger states, press actions and hold actions 1555/1462/1427.
// Trigger plus press, trigger plus hold, or both actions 1610/1579/1466. All three
// 1619, and all three with the retail behavior order 1582.
// NOT kept: all grow and retain frame 0x34, with the early dual byte still spilled.
// Typed ordinal snapshots, signed input slot or action selectors 1412 each. First ordinal read at
// every use 1549, second 1657, both 1544; paired tag references or all native selectors 1544 each.
// NOT kept: the first-ordinal and paired forms recover frame 0x38, EDI/ESI and BL, but grow
// to 5812 or 5800 bytes and raise the score. The typed snapshots retain frame 0x34.
// Trigger/barrel runtime-array lvalues, tag pointer fields, paired lvalues, and array bases with
// tag field pointers: 1413/1412/1413/1413 before their loops, 1412 each inside. The loop-entry
// runtime array forms shorten the barrel phase by four bytes. The 0x34 frame and dual spill remain.
// Named raw and clamped ramp rise/fall/both 1437/1441/1483. Blend rise/fall/both
// 1411/1412/1412. Both rising updates 1448. All four 1484. NOT kept: the two
// ramp updates recover frame 0x38 but grow to 5748 bytes and still spill the dual result.
// The 0x34 frame, pool and header registers and spilled dual result remain.
// Typed runtime barrel, barrel tag, thermal tag and ramp tag fields 1412/1412/1417/1412.
// Typed barrel and tag together, both weapon tag phases, runtime weapon, all fields 1412/1416/1412/1416.
// NOT kept: ties and regressions at retail length; every form retains frame 0x34.
// Explicit count refresh at the magazine, trigger, or barrel loop latch 1412 each.
// Magazine/trigger, magazine/barrel, trigger/barrel and all three 1412 each.
// Magazine while loop with the count refreshed after advancing the ordinal 1412.
// NOT kept: the latch reads remain, but the 0x34 frame, EBX/EDI and the dual spill persist.
// M3 lane 13 r159 (H2M_FAST with the file's callees listed, which reproduces the plain 1412): the owner unit index passed
// straight to 0xd1490 (no local) 1412; the player default declared after the flag-27 select 1437; the owner's player
// index tested and copied in one `&&` test 1412; the flag-27 select as an if/else 1412; the unit index declared ahead
// of `dual` and assigned after it 1412 (the 1412s the same code).
// M3 lane 8 r243 (H2M_FAST alone reproduces 1412): the complement computed once just ahead of the barrel loop 1440
// (5688 B); the player default declared first 1422; the owner's player index read through WEAPON_UNIT_GET's typed field
// 1412 (same code), tested and copied at each use 1412; the magazine loop as `if (function_fdd50(weapon_index, i))
// result = true;` 1407 (a branch where retail ORs the byte result in BL: `or bl, al`, home stored after the loop; under
// the bar, not kept). Body unchanged.
// M3 lane 7 r236 (H2M_FAST alone; control 1412): the existing weapon_get_102e10 for the first lookup, for `updated`,
// for both, or for the animation-state test 1412 each (same code); `scale = (real)dual` for the flag-27 select 1442
// (5668 B). Frame reading (stack packer): ours packs seven one-byte slots (down, up, blocked, target entry with
// finished, press, hold, release with stopped and dual), the two input arrays at -0x2c/-0x28 (retail's offsets too)
// and ten dword slots including `result` and `complement`; retail has nine one-byte slots ([esp+0x13] the bool result
// homed from BL, [esp+0x14..0x16], [esp+0x17], press/hold/release at [esp+0x18..0x1a], the target entry at [esp+0x1b],
// which also holds the behaviour-4 secondary-select bit), so the missing four bytes are the byte result plus one more
// byte slot. A forced-ranking search over ten single decisions finds nothing below 1411.
// GA34 (49 fast trials with 27 matched peers all MATCH, control 1412): direct barrel+0x10 (no `value` local) 1443;
// direct bitfield reads on barrel flag3/trigger flag6/all 1425/1412/1421 (+no `value` 1453); signed `char` trigger/barrel
// state or `word` `value_16e` 1412 each (+all 1448); ramp/cooling ternary arms 1469/1463; double clamps + unhoisted
// `1.0f - scale` 1454 (+bool `result`/`blocked` 1465); down/up without `primary`/`secondary` locals 1466 (snaps retail
// entry BL + EDI/ESI and magazine `or bl, al`); plus double clamps 1453, plus ramp/cooling arms 1383 (fr=0x30, 5652 B;
// not kept: uses a one-use request pointer and entry `blocked = false` set in test — dropping the pointer gives 1411);
// stripping entry `blocked = false` snaps EBP=weapon across the whole trigger loop (1623, mk=639; retail behavior order
// 1620, al=672, mk=510; all retail case orders 1585, al=618, mk=409); direct barrel flag0 instead of `weapon_request_barrel`
// where ready was already checked recovers EBP=weapon, EBX=i*12, ESI=trigger_entry (1469; with bitfield view 1463, 5696 B,
// fr=0x28); plus unmerged press/hold case 0/3 barrel reads and primary-first case 4 cuts masked diffs to mk=376 (1672,
// 5856 B; without split45 1490/1533, mk=406/384, ret[600:800] 186/200 masked-exact), but fr=0x28 vs retail 0x38 shifts
// ~180 stack offsets (retail also tests bytes +0x170/+0x171 as `> 0` (`ja`) and shares state 6/8 barrel-request tail).
// Body unchanged.
// @retail 0xfdfb0
bool __stdcall function_fdfb0(long weapon_index)
{
    s_weapon *weapon = WEAPON_GET(weapon_index);
    s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
    bool dual = function_101690(weapon_index);
    long unit_index = weapon_get_owner_unit_index(weapon_index);
    real scale = function_d1490(unit_index, weapon_index);
    long player_index = NONE;
    if (TEST_FIELD_BIT(definition->flag27)) scale = dual ? 1.0f : 0.0f;
    real complement = 1.0f - scale;
    long owner_index = function_101fb0(weapon_index);
    if (owner_index != NONE)
    {
        long resolved_player_index = WEAPON_LONG_AT(WEAPON_GET(owner_index), 0x13c);
        if (resolved_player_index != NONE)
            player_index = resolved_player_index;
    }
    if (weapon->object_index_194 != NONE && !function_badc0(weapon->object_index_194, NONE)) weapon->object_index_194 = NONE;
    long result = function_105e80(weapon_index);
    if (WEAPON_GET(weapon_index)->animation_state_offset != NONE)
    {
        byte *state = (byte *)weapon + weapon->animation_state_offset;
        s_animation *animation = NULL;
        if (WEAPON_SHORT_AT(state, 6) != NONE)
            animation = function_1daea0((s_graph_tag *)g_4e3b44[WEAPON_LONG_AT(state, 0) & 0xffff].bytes,
                *(c_type_709360 *)(state + 4));
        ((s_animation_state *)state)->update(function_bf600, weapon_index, 0, NULL, NULL);
        if (state[0x14] & 1) function_105740(weapon_index);
        else if ((state[0x11] & 2) || (animation && WEAPON_SHORT_AT(animation, 0x14) == 1)) function_105800(weapon_index);
        if (WEAPON_LONG_AT(state, 0) != NONE && WEAPON_SHORT_AT(state, 6) != NONE && (state[0x12] & 1) &&
            !(state[0x11] & 9) && WEAPON_SHORT_AT(animation, 0x14) > 1) result = true;
    }
    if (!(bool)((WEAPON_BYTE_AT(weapon, 0x16c) >> 5) & 1))
    {
        --WEAPON_SHORT_AT(weapon, 0x19e);
        if (WEAPON_SHORT_AT(weapon, 0x19e) <= 0) WEAPON_BYTE_AT(weapon, 0x16c) |= 0x20;
        result = true;
    }
    if (TEST_FIELD_BIT(definition->flag10) && weapon->parent_index == NONE &&
        WEAPON_BIT_AT(weapon, 4, 8)) function_10d4e0(weapon_index);
    if ((bool)((weapon->value_16e >> 7) & 1)) function_105840(weapon_index);
    if (weapon->value_16e & 1)
    {
        WEAPON_BYTE_AT(weapon, 0x16c) &= 0xef;
        WEAPON_REAL_AT(weapon, 0x190) = WEAPON_REAL_AT(weapon, 0x190) == 0.0f ? 1.0f : 0.0f;
    }
    if (WEAPON_REAL_AT(weapon, 0x18c) != WEAPON_REAL_AT(weapon, 0x190))
    {
        bool increasing = WEAPON_REAL_AT(weapon, 0x190) > WEAPON_REAL_AT(weapon, 0x18c);
        if (!(WEAPON_BYTE_AT(weapon, 0x16c) & 0x10))
        {
            long tag = WEAPON_LONG_AT(definition, increasing ? 0x248 : 0x250);
            if (tag != NONE) function_1039a0(weapon_index, tag, NONE, 0.0f, 0.0f);
            WEAPON_BYTE_AT(weapon, 0x16c) |= 0x10;
        }
        if (increasing)
        {
            WEAPON_REAL_AT(weapon, 0x18c) += WEAPON_REAL_AT(definition, 0x2d8) * g_510c54->rate;
            if (WEAPON_REAL_AT(weapon, 0x18c) > WEAPON_REAL_AT(weapon, 0x190))
                WEAPON_REAL_AT(weapon, 0x18c) = WEAPON_REAL_AT(weapon, 0x190);
        }
        else
        {
            WEAPON_REAL_AT(weapon, 0x18c) -= WEAPON_REAL_AT(definition, 0x2dc) * g_510c54->rate;
            if (WEAPON_REAL_AT(weapon, 0x18c) <= WEAPON_REAL_AT(weapon, 0x190))
                WEAPON_REAL_AT(weapon, 0x18c) = WEAPON_REAL_AT(weapon, 0x190);
        }
        result = true;
    }
    if (WEAPON_REAL_AT(weapon, 0x180) > 0.0f)
    {
        if (WEAPON_REAL_AT(weapon, 0x180) >= WEAPON_REAL_AT(definition, 0x154) && !(WEAPON_BYTE_AT(weapon, 0x16c) & 2))
        {
            WEAPON_BYTE_AT(weapon, 0x16c) |= 2;
            long owner = function_101f20(weapon_index);
            if (owner != NONE) function_c86e0(owner, false);
            if (definition->reload_style == 3 && (WEAPON_BYTE_AT(weapon, 0x16c) & 8)) WEAPON_BYTE_AT(weapon, 0x16c) &= 0xf7;
            else function_105fa0(weapon_index, 14);
        }
        if (WEAPON_REAL_AT(weapon, 0x188) == 0.0f)
        {
            bool overheated = (WEAPON_BYTE_AT(weapon, 0x16c) & 2) != 0;
            real rate = WEAPON_REAL_AT(definition, overheated && WEAPON_REAL_AT(definition, 0x168) > 0.0f ? 0x168 : 0x160);
            real delta = g_510c54->rate * rate;
            if (WEAPON_REAL_AT(definition, 0x254) > 0.0f) delta *= 1.0f - weapon->heat * WEAPON_REAL_AT(definition, 0x254);
            WEAPON_REAL_AT(weapon, 0x180) -= delta;
            if (WEAPON_REAL_AT(weapon, 0x180) < 0.0f) WEAPON_REAL_AT(weapon, 0x180) = 0.0f;
            if (overheated && !(WEAPON_BYTE_AT(weapon, 0x16c) & 4))
            {
                short ticks = first_person_weapon_animation_ticks(weapon_index, 0x08000071, 1);
                if (ticks <= 1) ticks = 1;
                if ((real)ticks >= (WEAPON_REAL_AT(weapon, 0x180) - WEAPON_REAL_AT(definition, 0x150)) / delta)
                {
                    WEAPON_BYTE_AT(weapon, 0x16c) |= 4;
                    function_105fa0(weapon_index, 15);
                }
            }
        }
        if ((WEAPON_BYTE_AT(weapon, 0x16c) & 2) && WEAPON_REAL_AT(definition, 0x150) > WEAPON_REAL_AT(weapon, 0x180))
            WEAPON_BYTE_AT(weapon, 0x16c) &= 0xf9;
        result = true;
    }
    long overheated_tag = WEAPON_LONG_AT(definition, 0x170);
    if (overheated_tag != NONE)
    {
        bool item_flag = TEST_FIELD_BIT(weapon->item_flag1);
        bool active = (WEAPON_BYTE_AT(weapon, 0x16c) & 2) && !item_flag;
        long effect = WEAPON_LONG_AT(weapon, 0x244);
        if (active != (effect != NONE))
        {
            if (active) WEAPON_LONG_AT(weapon, 0x244) = function_103a60(weapon_index, overheated_tag);
            else
            {
                if (item_flag) function_177610(effect);
                else function_177260(effect, true);
                WEAPON_LONG_AT(weapon, 0x244) = NONE;
            }
        }
    }
    WEAPON_REAL_AT(weapon, 0x188) = 0.0f;
    if (weapon->state_ticks > 0) { --weapon->state_ticks; result = true; }
    bool pressed[3] = { (weapon->value_16e & 2) != 0, (weapon->value_16e & 4) != 0, (weapon->value_16e & 0x100) != 0 };
    bool released[3] = { (weapon->value_16e & 8) != 0, (weapon->value_16e & 0x10) != 0, false };
    s_weapon *updated = WEAPON_GET(weapon_index);
    bool blocked = (updated->value_16e & 0x20) || updated->state_ticks > 0 || (WEAPON_BYTE_AT(updated, 0x16c) & 2);
    if (blocked) { weapon->value_174 = 0; result = true; }
    for (short i = 0; i < definition->magazine_count; ++i) result = result | function_fdd50(weapon_index, i);
    for (short i = 0; i < definition->trigger_count; ++i)
    {
        s_weapon_trigger *trigger = &weapon->triggers[i];
        s_weapon_trigger_definition *trigger_entry = &definition->triggers[i];
        if (trigger->timer > 0) { --trigger->timer; result = true; }
        short input = WEAPON_SHORT_AT(trigger_entry, 4);
        bool down = !blocked && (pressed[input] || (signed char)WEAPON_BYTE_AT(trigger, 4) < 0);
        bool up = !blocked && released[input];
        short primary = WEAPON_SHORT_AT(trigger_entry, 8), secondary = WEAPON_SHORT_AT(trigger_entry, 0xa);
        long target;
        byte target_entry;
        switch (trigger->state)
        {
        case 0:
            switch (trigger_entry->behavior)
            {
            case 0:
                if ((down || up) && primary != NONE && function_102540(weapon_index, primary, false))
                { trigger->flag5 = true; weapon_request_barrel(weapon_index, primary, false); }
                else trigger->flag5 = false;
                if (!down || up) function_102f70(weapon_index, primary);
                break;
            case 1:
                if (down)
                {
                    if (TEST_FIELD_BIT(trigger->flag0) && primary != NONE && function_102540(weapon_index, primary, false))
                    { weapon_request_barrel(weapon_index, primary, false); trigger->flag0 = false; }
                }
                else { trigger->flag0 = true; function_102f70(weapon_index, primary); }
                break;
            case 2:
                if (blocked) { weapon->value_171 = 0; weapon->value_170 = 0; weapon->value_173 = 0; }
                else
                {
                    bool press = false, hold = false, release = false;
                    if (!weapon->value_170 && !weapon->value_171)
                    {
                        if (up)
                        {
                            if (WEAPON_SHORT_AT(trigger_entry, 0x18) == 1 && function_103a90(weapon_index, i))
                            { function_103ce0(weapon_index, i); trigger->flag6 = true; }
                        }
                        else
                        { trigger->flag3 = false; trigger->flag4 = false; function_102f70(weapon_index, primary); function_102f70(weapon_index, secondary); }
                    }
                    else
                    {
                        function_1027b0(weapon_index, i, &press, &hold, &release);
                        if (press)
                        {
                            trigger->flag3 = true;
                            switch (WEAPON_SHORT_AT(trigger_entry, 0x1a))
                            {
                            case 0: weapon_request_barrel(weapon_index, primary, true); break;
                            case 1: if (function_103a90(weapon_index, i)) function_103ce0(weapon_index, i); break;
                            case 2: function_103b10(weapon_index, i); break;
                            case 3: weapon_request_barrel(weapon_index, secondary, true); break;
                            }
                        }
                        if (hold)
                        {
                            trigger->flag4 = true;
                            switch (WEAPON_SHORT_AT(trigger_entry, 0x18))
                            {
                            case 0: weapon_request_barrel(weapon_index, secondary, true); break;
                            case 1: if (function_103a90(weapon_index, i)) function_103ce0(weapon_index, i); break;
                            case 2: if (player_index != NONE) function_103b10(weapon_index, i); else function_103bd0(weapon_index, i); break;
                            case 3: weapon_request_barrel(weapon_index, primary, true); break;
                            }
                        }
                        if (release)
                        { trigger->flag3 = false; trigger->flag4 = false; function_102f70(weapon_index, primary); function_102f70(weapon_index, secondary); }
                    }
                }
                break;
            case 3:
                if ((down || up) && function_103a90(weapon_index, i))
                { if (up) trigger->flag6 = true; function_103ce0(weapon_index, i); }
                else function_102f70(weapon_index, primary);
                break;
            case 4:
                if (down)
                {
                    short selected = (weapon->value_16e & 0x40) ? secondary : primary;
                    if (TEST_FIELD_BIT(trigger->flag0) && selected != NONE && function_102540(weapon_index, selected, false))
                    { weapon_request_barrel(weapon_index, selected, false); trigger->flag0 = false; }
                }
                else { trigger->flag0 = true; function_102f70(weapon_index, primary); function_102f70(weapon_index, secondary); }
                break;
            case 5:
                if (down)
                {
                    if (TEST_FIELD_BIT(trigger->flag0) && function_102540(weapon_index, 0, false))
                    {
                        if (player_index != NONE && function_106280(weapon_index, &target, &target_entry)) function_103c40(weapon_index, i);
                        else
                        {
                            short selected = player_index != NONE ? primary : secondary;
                            if (selected != NONE && function_102540(weapon_index, selected, false))
                            { weapon_request_barrel(weapon_index, selected, false); trigger->flag0 = false; }
                        }
                    }
                }
                else { trigger->flag0 = true; function_102f70(weapon_index, primary); function_102f70(weapon_index, secondary); }
                break;
            }
            break;
        case 1:
            if (!trigger->timer) function_102d60(weapon_index, i);
            else if (!down && !up)
            {
                if (TEST_FIELD_BIT(trigger->flag6)) trigger->flag6 = false;
                else weapon_request_barrel(weapon_index, primary, false);
                weapon_clear_trigger(weapon_index, i);
                if (trigger->effect_index != NONE) { function_177260(trigger->effect_index, true); trigger->effect_index = NONE; }
            }
            break;
        case 2:
            if (!TEST_FIELD_BIT(trigger->flag6))
            {
                if (!down && !blocked) function_102e10(weapon_index, i);
                if (!trigger->timer) function_102f10(weapon_index, i);
            }
            else if (!up)
            {
                weapon_clear_trigger(weapon_index, i);
                if (trigger->effect_index != NONE) { function_177260(trigger->effect_index, true); trigger->effect_index = NONE; }
                trigger->flag6 = false;
            }
            break;
        case 4: break;
        case 5:
            if (down || blocked)
            {
                if (function_106280(weapon_index, &target, &target_entry))
                {
                    if (weapon->object_index_194 == target)
                    { weapon->entry_177 = target_entry; if (!trigger->timer) function_103bd0(weapon_index, i); }
                    else { function_103c40(weapon_index, i); weapon->object_index_194 = target; weapon->entry_177 = target_entry; }
                }
                else function_103b70(weapon_index, i);
            }
            else
            {
                if (primary != NONE && function_102540(weapon_index, primary, false))
                { weapon_request_barrel(weapon_index, primary, false); trigger->flag0 = false; }
                weapon_clear_trigger(weapon_index, i);
            }
            break;
        case 6:
            if (!down && !blocked)
            { weapon_request_barrel(weapon_index, secondary, true); weapon_clear_trigger(weapon_index, i); }
            else
            {
                long owner = weapon_get_owner_unit_index(weapon_index);
                if (function_1061c0(owner, &target, &target_entry) && weapon->object_index_194 == target) function_103bd0(weapon_index, i);
                if (!trigger->timer && player_index != NONE) function_103b70(weapon_index, i);
            }
            break;
        case 3:
        case 7:
            if (down || blocked)
            { if (function_106280(weapon_index, &target, &target_entry)) function_103c40(weapon_index, i); }
            else
            {
                if (trigger->state != 7 && primary != NONE && function_102540(weapon_index, primary, false))
                { weapon_request_barrel(weapon_index, primary, false); trigger->flag0 = false; }
                weapon_clear_trigger(weapon_index, i);
            }
            break;
        }
        if (trigger->state || down || up) result = true;
    }
    for (short i = 0; i < definition->barrel_count; ++i)
    {
        s_weapon_barrel *barrel = &weapon->barrels[i];
        s_weapon_barrel_definition *barrel_definition = &definition->barrels[i];
        bool active = false;
        if (barrel->value14 > 0.0f)
        { barrel->value14 -= WEAPON_REAL_AT(barrel_definition, 0xd0) * g_510c54->rate; if (barrel->value14 <= 0.0f) barrel->value14 = 0.0f; result = true; }
        if (WEAPON_REAL_AT(barrel, 0x18) > 0.0f)
        { WEAPON_REAL_AT(barrel, 0x18) -= WEAPON_REAL_AT(barrel_definition, 0xcc) * g_510c54->rate; if (WEAPON_REAL_AT(barrel, 0x18) <= 0.0f) WEAPON_REAL_AT(barrel, 0x18) = 0.0f; result = true; }
        if (barrel->ticks > 0) { --barrel->ticks; result = true; }
        switch (barrel->state)
        {
        case 0:
            if (blocked) barrel->flag0 = false;
            else if (TEST_FIELD_BIT(barrel->flag0))
            {
                bool wait = false;
                if ((TEST_FIELD_BIT(barrel_definition->flag9) || TEST_FIELD_BIT(barrel_definition->flag10)) && definition->barrel_count > 1)
                {
                    byte state = weapon->barrels[i == 0].state;
                    wait = (state == 1 && TEST_FIELD_BIT(barrel_definition->flag9)) || (state == 3 && TEST_FIELD_BIT(barrel_definition->flag10));
                }
                if (!wait) barrel->state = 1;
                result = true;
            }
            else if (function_1023d0(weapon_index, i)) result = true;
            if (barrel->state != 1) break;
        case 1:
            if (blocked) function_103e60(weapon_index, i);
            else
            {
                bool finished = false, stopped = false;
                if (function_102100(i, weapon_index, &finished, &stopped))
                {
                    if (finished && !TEST_FIELD_BIT(barrel->flag0)) function_103e60(weapon_index, i);
                    else { barrel->flag0 = false; active = true; function_102fb0(weapon_index, i); }
                }
                else if (stopped || (finished && !TEST_FIELD_BIT(barrel->flag0))) function_103e60(weapon_index, i);
                else
                { long ticks = barrel->timer + 1; barrel->timer = (char)(ticks > 127 ? 127 : ticks); active = true; }
            }
            break;
        case 2: if (!barrel->ticks) function_103f60(weapon_index, i); break;
        case 3: if (!barrel->ticks) function_103dd0(weapon_index, i); break;
        }
        if (barrel->state || barrel->ticks > 0) result = true;
        real value = WEAPON_REAL_AT(barrel, 0x10);
        if (active)
        {
            value += WEAPON_REAL_AT(barrel_definition, 0xd4) * g_510c54->rate;
            if (value > 1.0f) value = 1.0f;
            WEAPON_REAL_AT(barrel, 0x10) = value;
            if (WEAPON_REAL_AT(barrel_definition, 0x18) != 0.0f && !TEST_FIELD_BIT(barrel->flag3) && value > WEAPON_REAL_AT(barrel_definition, 0x18))
            { function_ba7f0(weapon_visible_model_owner(weapon_index), NONE, 0, NONE); barrel->flag3 = true; }
        }
        else
        {
            if (value > 0.0f)
            { value -= WEAPON_REAL_AT(barrel_definition, 0xd8) * g_510c54->rate; if (value < 0.0f) value = 0.0f; WEAPON_REAL_AT(barrel, 0x10) = value; result = true; }
            if (TEST_FIELD_BIT(barrel->flag3) && WEAPON_REAL_AT(barrel_definition, 0x18) > value)
            { function_ba7f0(weapon_visible_model_owner(weapon_index), NONE, 0, 0); barrel->flag3 = false; }
        }
        if (value > 0.0f)
        {
            real old = (real)WEAPON_BYTE_AT(weapon, 0x175) * (1.0f / 255.0f);
            real increment = value * (1.0f / 7.0f);
            if (WEAPON_REAL_AT(barrel_definition, 0x14) > 0.0f) increment *= WEAPON_REAL_AT(barrel_definition, 0x14);
            WEAPON_BYTE_AT(weapon, 0x175) = (byte)(long)((increment + old) * 255.0f);
        }
        if (active)
        {
            WEAPON_REAL_AT(barrel, 0x20) += WEAPON_REAL_AT(barrel_definition, 0xc4) * g_510c54->rate;
            if (WEAPON_REAL_AT(barrel, 0x20) > 1.0f) WEAPON_REAL_AT(barrel, 0x20) = 1.0f;
            WEAPON_REAL_AT(barrel, 0x1c) += (complement * WEAPON_REAL_AT(barrel_definition, 0xdc) + scale * WEAPON_REAL_AT(barrel_definition, 0x50)) * g_510c54->rate;
            if (WEAPON_REAL_AT(barrel, 0x1c) > 1.0f) WEAPON_REAL_AT(barrel, 0x1c) = 1.0f;
            result = true;
        }
        else if (WEAPON_REAL_AT(barrel, 0x1c) > 0.0f)
        {
            WEAPON_REAL_AT(barrel, 0x20) -= WEAPON_REAL_AT(barrel_definition, 0xc8) * g_510c54->rate;
            if (WEAPON_REAL_AT(barrel, 0x20) <= 0.0f) WEAPON_REAL_AT(barrel, 0x20) = 0.0f;
            WEAPON_REAL_AT(barrel, 0x1c) -= (complement * WEAPON_REAL_AT(barrel_definition, 0xe0) + scale * WEAPON_REAL_AT(barrel_definition, 0x54)) * g_510c54->rate;
            if (WEAPON_REAL_AT(barrel, 0x1c) <= 0.0f) WEAPON_REAL_AT(barrel, 0x1c) = 0.0f;
            result = true;
        }
    }
    return result;
}


void __stdcall function_104150(long weapon_index, short barrel_index);

struct s_entry_pair { long object_index; short entry_index; };
struct s_damage_owner;
struct s_type_1e6529;
struct s_location;
void __stdcall function_c8290(long unit_index, point3f *point, vector3f *aim, vector3f *velocity,
    real const *offsets, bool project, bool use_aim, bool clip);
bool function_109e00(long object_index, vector3f *velocity, bool required);
void function_cafc0(long unit_index, point3f *position);
long function_baf40(long object_index);
bool function_16a7c0(point3f const *from, point3f const *to, long ignore_object_index,
    long ignore_unit_index, point3f *clipped);
bool function_106400(long object_index, bool flag);
bool function_106320(s_entry_pair *pair);
bool function_1a4c00(long player_index, s_entry_pair *pair, bool primary, vector3f *direction, point3f *point);
bool function_1fddd0(long actor_index, point3f const *origin, vector3f *direction,
    long *out_index, real *spread, long *out_object);
short function_15cca0(long player_index, byte code);
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner);
long __stdcall function_b7b40(void *data);
void random_vector_in_cone(dword *seed, vector3f const *forward, vector3f *result, real min_angle, real max_angle);
void __stdcall function_a7870(long object_index);
void function_fa100(long projectile_index);
transform4x3f *function_b8c00(long object_index, long *node_count);
point3f *function_b9dd0(long object_index, point3f *point);
void function_141590(transform4x3f const *in, transform4x3f *out);
point3f *transform4x3f_apply_point(transform4x3f const *matrix, point3f const *point, point3f *out);
s_location *object_get_root_location(long object_index, s_location *location);
void __stdcall object_get_damage_owner(long object_index, s_damage_owner *owner);
long __stdcall function_d6c80(s_type_1e6529 *data, long ignore_object_index);
void function_a7d50(long index, long group, long target, short node, point3f const *point);
extern short g_47d8e0;
extern short g_4686c4;

PRIVATE __forceinline real weapon_emission_random(dword *seed)
{
    *seed = *seed * 0x19660d + 0x3c6ef35f;
    return (real)(*seed >> 16) * 1.5259021893143654e-05f;
}

PRIVATE __forceinline void weapon_emission_rotate(vector3f *direction, vector3f const *axis, real angle)
{
    real sine = (real)sin(angle), cosine = (real)cos(angle);
    real amount = (direction->i * axis->i + direction->j * axis->j + direction->k * axis->k) * (1.0f - cosine);
    vector3f result;
    result.i = direction->i * cosine + axis->i * amount - (direction->j * axis->k - direction->k * axis->j) * sine;
    result.j = direction->j * cosine + axis->j * amount - (direction->k * axis->i - direction->i * axis->k) * sine;
    result.k = direction->k * cosine + axis->k * amount - (direction->i * axis->j - direction->j * axis->i) * sine;
    *direction = result;
}

/* whether the game runs in mode 4 */
static inline bool weapon_game_mode_is_4(void)
{
    bool result = false;

    if (g_4e6948->mode == 4)
        result = true;
    return result;
}

/* what function_b7930 builds and function_b7b40 makes a projectile from */
struct s_weapon_projectile_creation
{
	byte unknown00[0x18];
	dword flags;
	point3f position;
	vector3f forward;
	vector3f up;
	vector3f linear_velocity;
	byte unknown4c[0xc4 - 0x4c];
};

// score 1121 (was 1121): The first difference at +0x1 is the 0x1ea4-byte stack probe against retail’s 0x1e98.
// Entry lookup and indexing agree before the first spills. Retail’s 0x88-byte damage event sits above the
// distinct clipping output and the creation request. The trials replace the raw damage buffer with the existing
// event layout and test whether direct member publication changes the workspace allocation while preserving the
// values and call tuples.
// Earlier at 1121 (was 1121): the projectile code is kept as a dword and narrowed to a short where it is stored (retail
// writes the code as a dword at 0x104450 and reads a word back from the slot at 0x105233), which gives the function
// retail's length (5212 bytes). What still differs is the frame (0x1ea4 against retail's 0x1e98, twelve bytes bigger)
// and the parameter offsets that follow from it, the game mode read (a byte load plus a sign extension where retail
// reads it with one move at 0x10428d) and the layout of the locals below the creation block.
// MiniMax B (11-14): lane B pass over 0x104150; the two parameters are read directly instead of through a pointer to
// them, and the projectile code is a dword narrowed at the store. Not kept: a creation block twelve bytes smaller (the
// frame stays over), the complement and the owner result held in variables, the average loop rewritten, the game mode
// helper inlined, the marker select spelled as a bool.
// Earlier 1143 from 1161 after comparing each statement with retail (11-05, w1B). The shot spread is
// computed only while it is still 0 (0x104dbe-0x104dcb: ucomiss, lahf, test ah 0x44, jp past the block) and low/high fall
// back to +0x74/+0x78 when +0x5c/+0x60 are 0, writing `spread` itself (0x104e4a; the cone call at 0x104e8b reads it);
// function_1fddd0 gets (average-or-origin, direction) (0x1048a9-0x1048da; the source passed the average as the direction);
// function_109e00 fills its own vector (0x1046a1: [esp+0xec]) for the has_motion blend (0x1050c8) while the speed dot
// product reads the function_c8290 vector (0x1046b8-0x1046e0; the source shared one vector, which differed whenever
// function_109e00 wrote it); the mark code passed to function_15cca0 is tag+0x128 & 0x3f with no 0x40 bit (0x104437-0x104444).
// The normalize is inline as in retail (0x104f1d-0x105044), summing i*i + k*k + j*j where function_30bf0 sums i, j, k
// (last-ulp only). Same results, retail's shape: the second function_101fb0 call (a lookup) only when the projectile tag is
// set (0x104c8b); the target entry is a short (0x104835, 0x104866) copied with the object index as 8 bytes (0x105276-0x105284);
// marker index and shot counter are shorts; node_count starts at 0 (0x10535b); the synchronized point starts as {0.0f}
// (0x104260, the value g_468788 points at); up/forward/velocity live in the creation block; the projectile is re-fetched
// for the target store and `&= ~2` (0x10526f, 0x105292).
// Left: retail lays the player-controlled/actor `else if` straight after the player branch (0x104890-0x104935) but ours
// outlines it after the spread block. Round 3 (w3A, fast check): the cause is the `if (!aimed) continue;` edge to the
// loop latch; with the continue replaced by a store (a diagnostic only, as it changes behaviour) the actor block stays inline
// as in retail. Not kept: `aimed` as a loop-scope bool preset true and tested after the whole unit block 1118 (retail tests
// it only inside the actor branch, 0x10492d, and the other paths jump past it to 0x10493b; a preset then overridden value);
// `if (player_index != NONE) player_controlled = true;` as at retail 0x1045df 1136, alone, at retail's 5212 bytes (a flag set
// inside a test). On top of both: the rest of the body wrapped in `if (aimed)` (identical code), the actor tested first with
// the player branch as the `else` 1123, the marker-select `continue` as a bool in three spellings (identical code;
// retail 0x1044e5-0x1044ef materialises it with sete/test/je, ours emits jne). The mark-code expression (retail 0x104437:
// `xor/and/xor` merge of the byte at [esp+0x13], the network flag's slot, with the low six bits, then `and 0xffffff3f`) has
// no natural C spelling: plain `& 0x3f` here. The average loop is retail's (0x104360): retail's `jmp`/`lea ebx,[ebx]`
// before it is the loop-head alignment pad (also at 0x104480); only its pointer anchor differs ([eax-8..eax] against ours
// [eax-4..eax+4]). Frame 0x1ea4 against retail 0x1e98: the locals below the creation block are 12 bytes bigger (retail:
// synchronized point 0xf8, creation 0x104, clipped above it at 0x1c8; ours: clipped between them); moving
// `repeated_direction` into the projectile block changed nothing.
// Steps (fast check): behaviour fixes 1221, creation block 1314, normalize_inline instead of the function_30bf0 call 1274,
// short target + loop-top initialisers 1252, shift-style flag tests + projectile refetches 1143.
// Permuter, 40 minutes (11-05): 1082 only with the `unit_index != NONE` test dropped (a behaviour change), temporaries and
// casts. Not kept.
// Lane N round 411 (fast control 1121).
// Muzzle trace point 1121.
// Ballistic placement 1121.
// Hit transform homes 1121.
// Synchronized node 1137.
// Placement owner scope 1121.
// Placement owner once 1158.
// Damage mode gate 1121.
// Shot player slot 1121.
// Unit flag field 1121 (kept after plain check).
// m3 lane 2 r105 (fast): `owner_index` renamed to sort first or last, `special_marker` renamed to sort first 1121
// each (the same homes: these are spill slots); `motion` declared just before the 0xc8290 call 1135 (5197 bytes), at
// the top of the unit block 1121; `repeated_direction` at loop scope 1121; `center` and `clipped` declared apart 1121.
// The frame stays 0x1ea4 in all of them.
// M3 lane 6 r196 (fast): retail's frame below the markers, from the top: inverse 0x274, center 0x268, point 0x25c, damage
// 0x1d4, clipped 0x1c8, creation 0x104, synchronized point 0xf8, unit velocity 0xec, motion 0xb4, average 0x78, axis 0x6c,
// origin 0x48, direction 0x28 (offsets after the four pushes). Ours has creation above clipped and 12 more bytes of small
// locals. Retail's two rotations keep no 12-byte result: the axis and the direction are written back in place, with
// single floats spilled (0xc8, 0xd0, 0xd4) and the dot product summed i, k, j; ours keeps two 12-byte results (24 bytes).
// Rotation through the shared in-place helper (axis_rotation.h, sine and cosine computed first) 1190 (5150 B, frame
// 0x1ea0); the file's helper with three scalar results written back 1239 (5115 B, frame 0x1e94); the axis read as
// `markers[marker_index].matrix.up` (retail reads it by index, not through the marker pointer) 1121 (same code), with the
// shared helper 1185 (5155 B, 0x1ea0); `point3f clipped, center;` 1121 (same code).
// Round 145 (m3 lane 16, fast, control 1121): retail rotates in place (cross terms precomputed, one spilled, result.i
// stored straight back, j/k kept in registers into the second rotation; no 12-byte result temps): the file's
// weapon_emission_rotate body written in place with the three cross terms named 1211 (5146 bytes, frame 0x1ea0,
// register-masked aligned 286 -> 212); the same with the dot summed i, k, j 1211; with only cross_j/cross_k named 1211.
// Retail's spread fallbacks are selects that re-read the field (`x != 0 ? x : y`, 0x104dd6-0x104e0a): that form 1121
// (other code, register-masked 275); control bit 6 through the 0x16e flag view (retail's shr at 0x104e66) 1120; both
// 1125 (5217 bytes); both on the in-place rotation 1194 (5162 bytes); with the axis read by marker index 1190 (5166
// bytes, register-masked 210/229). The in-place forms are 46-66 bytes short of retail, so the length gap is elsewhere
// (retail duplicates the 0x1fddd0 argument pushes in both arms of the average/origin select at 0x1048c3).
// Muzzle trace with origin as input and output 1181. Muzzle trace with center as input and output 1136.
// Marker input with the origin as trace output 1185. Clipped position transferred by point setter 1140.
// Clipping position read directly by marker index 1121. Clipped output declared after filling the center
// 1121. Creation position and forward through component setters 1122. Motion vector scoped to the admitted
// unit phase 1121. NOT kept: The three shared clipping outputs recover the 0x1e98 frame and move the first
// difference to +0x75, but they shorten the body and regress. Retail passes a separate clipped point. The
// copy and motion-scope forms tie or regress.
// M3 lane 8 r200 (fast; raw score / order-insensitive masked difference, control 1121/111). Forms that follow retail's
// code, each on top of the one before: the marker name and the marker object looked up into locals before the marker
// call (retail calls 0x101b10, then 0x101ec0, then pushes), markers read by index with no marker pointer (retail keeps
// index * 0x70), `if (player_index != NONE) player_controlled = true;`, the primary index preset NONE and then set, the
// trace's ignore index named before the call 1115/96; the rotation helper in place with the three cross terms named
// (retail computes the i row's cross term at its use and the j and k cross terms before the first store), a short
// projectile code, the unit flag as bit 18 of the dword at 0x108 1187/72 (frame 0x1e98, first difference at +0x53);
// projectile index and count as if/else arms that each read +0x6a, the owner filled in if/else arms, the spread
// fallbacks as `field == 0.0f ? fallback : field`, bit 6 of 0x16e shifted 1176/67; the actor call written in both arms
// of the bit 8 test (retail pushes the three out pointers in each arm), the target type as a long (retail movzx), the
// shot counter post-incremented inside its comparison (retail loads the limit before the store), projectile flag bit 1
// cleared through a pointer to the flags field, the unit flag from the word at 0x10a (retail shifts a byte by 2) 1122/46
// (5197 bytes; the frame is 0x1e9c from this step on); the parent walk tested through the unit pointer with the index
// read through a pointer to the parent field advanced each pass 1037/41 at 5212 bytes (retail's `add eax, 0x24c`, reload),
// against the value walk 1122, a guarded do-while 1122 and a field pointer with a guarded do-while 1091; the spread
// angle through the existing 0x259d0 range function and the turn as the same function over [0, 2 pi] (retail multiplies
// by 1/65535 and then by 2 pi), the axis copied before the angles, the dot terms written j, k, i (retail sums i, k, j)
// 1039/38, the random block then retail's instruction for instruction. The marker select and the actor's aim result as
// one flag tested after the unit block 1219/90 (the actor block stays in line as in retail, but the flag takes a stack
// byte and the frame is 0x1e94); as two preset flags 1172/71 (retail's sete/test for the select, the actor block out of
// line again); nested ifs in place of both continues: the same code.
// What still differs on the 1037 form (frame 0x1e9c): (1) this build keeps a float zero in xmm7 across the marker loop and
// direction.j in memory, where retail keeps direction i/k/j in xmm5/xmm3/xmm2 from the inherited-speed dot product to
// the end of the rotation and has a zero register only inside the projectile loop (set after the 0xb7930 call; the
// loop-top resets use xmm0); that also decides whether the actor block is laid out in line. Against it: chained reset
// `a = b = c = 0.0f` 1095; double literals in the loop's four zero comparisons 1115 (the outer zero register goes and
// the actor block comes back in line, but the loop then has no zero register); integer stores for every zero outside
// the loop 1132 (a diagnostic); a clobbering call beside 0xb7b40 and beside 0xd6c80, 0xb7b40's real body in place of
// its stand-in, and the weapons file compiled ahead of 0xb7930's file all change nothing; 0xb7930 declared inline in
// its own file gives retail's zero placement (1146, ordered masked difference 458 -> 227) but costs 0x293130 and is not
// retail's form, since retail's 0x293130 keeps a zero in xmm2 across that call. (2) The mark code: retail merges the
// tag's six bits into an uninitialised byte with xor/and/xor and clears bits 6 and 7 with a 32-bit and before the
// push; a bit-field aggregate filled in full folds to `and 0x3f` plus a store and reload (as the byte does now), and
// only the six-bit field written with `& ~0xc0` at the call gives retail's instructions in a compile-only test, which
// needs 0x15cca0's code parameter typed that way. (3) The clipped point sits above the creation block in retail
// (0x1c8) and below it here (0x104): the large locals sort by reference count over size; three references (the
// address and two fields) put it in retail's place, the address alone puts it above the damage block, and the natural
// four leave it below the creation block, so retail's creation block carries more references than ours. (4) The marker
// count in ebp (ours ebx), the average loop's pointer anchored at z (ours y), and `average * reciprocal` computed in
// place (ours copies the reciprocal; the operand order makes no difference).
// Not kept: the forms that are acceptable without a match taken alone (in-place rotation, existing random functions,
// if/else arms, field re-reads, the actor call in both arms, markers by index, long target type, short code) measure
// 1191 (5156 bytes, frame 0x1e9c), a rise, and the rest are match-only forms; the body is main's.
// Shared rotation for the axis only 1140, direction only 1143. In-place rotation with all direction
// components captured or just the first two 1205/1205, with both vectors captured 1229. Projection
// before trig 1121, with in-place direction snapshots 1204. Separate range and turn scopes 1121.
// The in-place snapshots recover the 0x1e98 frame and move the first difference to +0x79,
// but finish 93-97 bytes short. The original separate clipping workspace remains.
// Ordinary-case exclusion 1138, synchronized-only first 1143, exhaustive ordinary-first conditional
// 1140, default network case first 1153; independently computed synchronization 1143 and independent
// synchronization/network results 1157. Switch with case 2 first or default first 1121 each.
// NOT kept: the conditionals lengthen the body, and neither switch order changes the 0x1ea4 frame.
// Typed barrel tag, named projectile field, native barrel flags and aggregate placement owner 1121
// each. Owner presence branch 1122 at 5199 bytes, admitted motion 1121, tracer from the two tests
// 1119 at 5212 bytes, complete damage initializer 1121. NOT kept: tracer gains two and retains the
// 0x1ea4 frame. The owner branch shortens the body and regresses.
// Rotation through complete setter 1248, aggregate result 1121; shared dot
// with setter 1216, staged cross scalars with setter 1244. Shared cross,
// alias-safe cross, and shared cross with direct component stores 1186 each;
// shared dot/cross with setter 1165. NOT kept: the setter alone drops the frame
// to 0x1e94 and shortens the body by 107 bytes. The shared cross forms use
// 0x1eac. Separate clipping and the native frame gap remain.
// Shared marker position 1154, marker origin/direction setters 1122/1149, i-k-j inherited/perpendicular
// dot products 1144/1121, actor speed scale-add 1121, synchronized point setter 1136, inherited velocity setter 1121.
// NOT kept: the marker direction setter grows the frame to 0x1eac and the remaining forms keep 0x1ea4.
// Round 452: Creation up through a setter or an aggregate in each case 1136/1299.
// Join component outcomes before a setter, field stores, or an aggregate 1143/1143/1171.
// Repeated-direction setter on capture, restore, or both 1142/1139/1137.
// The grouped result assignments expand the 0x1ea4 frame, and neither separate
// component publication nor repeat transfer recovers retail's 0x1e98.
// M3 lane 5 r220 (fast, control 1121): retail aligns the average, marker and projectile loop heads (jmp over a pad
// where 6 to 10 bytes are needed) and not the parent walk (12 needed); ours pads only the parent walk (9 needed), so
// the pads follow the code length ahead of each loop, not a source form. The up vector's `else if (ay <= az)` with its
// arms in retail's order (the az >= ay arm falls through at 0x104f79) 1122; the mode helper's test on a long local
// 1121 (same code: the compare is still narrowed to `cmp al, 4`); the helper given the caller's long mode as a
// parameter 1135 (retail's `movsx eax` / `cmp eax, 4` section then exact; a parameter picked by score); the rotation
// dot summed i, k, j 1121 (same code); the three loop-top floats reset by one chained assignment 1154 (5197 B); the
// inherited-speed dot written j, k, i (retail computes i, k, j from direction registers) 1145 (5239 B).
// Rotation through the shared setter 1248, aggregate rotation result 1121, rotation returned by value from
// pointers 1121, fixed axis by value 1150, staged and direct shared cross 1186 each, shared i-k-j projection
// 1131, changing vector passed and returned by value 1185. NOT kept: no gain. The setter reduces the frame to
// 0x1e94 but shortens the body to 5105 bytes.
// Inherited velocity aggregate 1168, named perpendicular components and staged correction 1121 each,
// shared vector addition 1132, named actor correction, staged actor result and both staged results 1121 each,
// named cone minimum terms 1154. NOT kept: ties and regressions. The 0x1ea4 frame still exceeds 0x1e98.
// Typed damage event with member stores or value initialization 1145/1145. Typed owner aggregate 1122.
// Origin/axis/both setters 1128/1168/1140. Chained origin stores 1145. Owner aggregate with chained origins
// 1158. NOT kept: none improve the opening frame or score.
// Marker-average scalar sums, indexed with direct seeds or initial point copy and component/point stores
// 1194/1139/1135/1142. The same four forms with a marker cursor 1171/1141/1130/1204. NOT kept: the 0x1ea4 frame
// remains and the scores rise.
// Fixed axis components with complete result or direct output 1154/1190; fixed axis
// with projected products and direct output 1228. Projected products before direct
// output or complete result 1215/1138. Named cosine complement with direct output
// 1215. Shared dot and projected products before complete result or direct output
// 1138/1215. NOT kept: the 0x1e9c frame forms shorten the body to 5125–5141 bytes
// and raise the score. The separate clipping point and opening rotation remain.
// Rotation result by value, explicit destination, returned destination, or staged component stores:
// saved axis 1121/1121/1121/1239, direct marker axis 1141/1141/1141/1273.
// NOT kept: the saved-axis result forms tie. The separate clipped point and retail frame remain.
// The rotation still needs the retail scalar homes at the 0x1e98 frame.
// Dot before cosine, cross before cosine, dot then cross, or cross then dot:
// vector result 1121 each, staged scalar result stores 1239 each.
// NOT kept: the vector forms tie and the scalar forms reserve 0x1e94 and shorten the body to 5115 bytes.
// Caller-owned rotation output, component stores or setter: one result for both saved inputs 1132/1132,
// separate results 1214/1210, direct marker input with separate results 1273/1273, or direct marker input
// with a shared result 1142/1142. NOT kept: one result for the saved inputs recovers 0x1e98 and first +0x75
// at 5195 bytes, but raises the score. The separate trace point and original rotations remain.
// M3 lane 15 r135 (the coordinator's rotation-family screen, h2m fast): both weapon_emission_rotate calls through include/axis_rotation.h's helper 1190 (5150 B). Restored.
// M3 lane 3 r301 (fast, control 1121; lane 8's saved lead re-measured 1039): each marker's position read through a
// pointer inside the averaging loop (three uses) 1121: the loop is then retail's instruction for instruction, the
// pointer anchored at z (1040 on the lead); a lead, no gain here. The scaling after it as `reciprocal * average.x`,
// `average.x *= 1.0f / (real)marker_count`, `average.x /= count`, through a pointer to the average or with (real)
// casts on the products: the same code each in a compile-only screen (retail multiplies the sums in place). The game
// mode through a long-returning inline accessor used by the function and the mode-4 helper 1121 (same code).
// Diagnostics on the lead: compiled without link-time code generation it shows retail's seven zero sites exactly
// (xmm0 at the marker loop top, xmm1 at the speed clamp, a memory compare for the aim test, xmm7 after the 0xb7930
// call, after the cone call and in the unit-velocity block, a memory compare after the creation call); an
// unsummarised call right after the 0xb7930 call gives the same seven sites in the linked image and loads all three
// direction components into registers ahead of the speed dot product, 1096 (5216 bytes); the three loop-top resets as
// integer stores 962 (5199 bytes, frame 0x1ea0; a cast that only steers). So the open cause is one fact: retail sets
// the zero again after the 0xb7930 call in the projectile loop (0x104d91) where this build keeps xmm7 across that
// call. The callee writes only xmm0 and retail's 0x293130 keeps xmm2 across it, and 307 of the 311 retail call sites
// that keep an xmm register across a call fit exact kill sets of the callee, so retail does know the callee's set
// and still does not carry the zero into the loop.
// M3 lane 2 r147 (H2M_FAST, control 1121; al = aligned rows exact/register-masked/register-and-stack-masked, 771/553/287):
// the projectile tag and count assigned in both arms of the barrel-0 override (retail reads +0x6a in each arm and
// multiplies with a 16-bit imul, 0x104c2a) 1116 (5214 B; that arm is then retail's instruction for instruction); with the
// delayed-shot test as `delayed > 0` (retail jbe) 1116; with the rejected target reset as two statements, object first
// (retail's store order) 1116; with the target type as a long 1155 (5197 B); the third form with the projectile owner
// filled in if/else arms (retail 0x104cd5..0x104d15) 1131 (5195 B, al 683/533/316), with the actor call in both arms of
// the bit-8 test 1128 (5215 B, al 718/560/266); the third form with only the actor call in both arms 1159 (5245 B). The
// 1116 forms gain 5, under the bar. Lead: retail builds the mark code at 0x104437 as a six-bit field inserted into an
// uninitialised stack byte with bits 6-7 cleared, a one-byte bit-field record passed by value to 0x15cca0, which hands it
// to 0x196ef0; typing that parameter reaches 0x197160 and its caller in another lane's file. Body unchanged.
// Shared spread weight, named blended endpoints or separate endpoint products 1121 each.
// Captured default endpoints 1134; early spread position 1140; cone products 1154; shot-wide default weight 1158; selected shot scale 1154.
// NOT kept: the local blend ties; the shot-wide form grows the frame. The 0x1ea4 probe remains.
// Whole rotation result with direction, axis or cross component locals 1154/1154/1121.
// Direction/axis, direction/cross, axis/cross or all three 1121/1154/1154/1121.
// Both inputs and in-place projection 1154. NOT kept: ties or shorter bodies with higher scores.
// The separate clipping output and 0x1ea4 frame remain in the opening body.
// The 0x1ea4 frame and holder-walk register assignments still differ. NOT kept: native player fields
// tie, final actor and player reads expand the frame, and the other holder forms shorten the body and
// raise the score. first parent retained across the walk 1154; aim permission from both final tests
// 1153; aim permission seeded by the original parent 1152; native player fields in the holder walk 1121;
// actor observation from the resolved holder 1147; player observation from the resolved holder 1123; aim
// address from the resolved holder 1169; three values from the resolved holder 1152; shared typed holder
// pool during traversal 1143.
// Staged cosine contributions, additive subtotals and sine-weighted cross terms before rotation publication:
// complete vectors 1177/1154/1154, direct component stores 1245/1216/1239. Result-vector updates in two
// or three phases 1250/1257. NOT kept: all raise the score and move away from retail length. The
// 0x1ea4 frame and separate clipping output remain.
// The 0x1ea4 probe and separate point homes remain.
// Separate world point using the getter result 1120; synchronization output in place 1141 or 1140 with getter result.
// Muzzle center or clipped point reused for the world point 1136/1136, both with getter result 1134/1134.
// World point transformed in place and copied 1155.
// NOT kept: the first form gains one. Sharing a point reaches frame 0x1e98 but shortens the body; retail keeps distinct clipping and synchronization points.
// Shared setters for clip-to-origin, marker-to-origin, request position and request direction
// 1140/1125/1139/1157. Both origins 1136, both request fields 1122,
// clip and request position 1128, all four 1151.
// NOT kept: the request pair retains retail length but the 0x1ea4 probe and distinct
// point homes persist. The other transfers also raise the score.
// The 0x1ea4 probe and the clipping and request homes still differ. Standard clip copy 1121,
// named owner and wielder homes 1121, named speed and spread homes 1121, named first projectile
// direction 1121, shared special-marker draw 1121, separate target outputs 1139 (0x1eac frame),
// standard request copies 1121, interleaved request fields 1125 at 5215 B, standard motion copy
// 1121, standard synchronization point clear 1130.
// M3 lane 13 r161 (fast, control 1121): the inherited speed through the existing dot3f 1138 (5196 B); the unit-velocity
// projection through dot3f 1121 (same code); both 1138; the three loop-top resets as one chained `= 0.0f` 1154
// (5197 B); the inherited-speed clamp compared with a double zero 1168 (5165 B). A forced-ranking search over the
// zero's web (ours one xmm7 web across both loops, ranked 202/272) found no single ranking or cut that moves the
// count: the frame (0x1ea4 against 0x1e98) still decides every stack offset.
// @retail 0x104150
void __stdcall function_104150(long weapon_index, short barrel_index)
{
    s_weapon *weapon = WEAPON_GET(weapon_index);
    s_weapon_barrel *barrel = &weapon->barrels[barrel_index];
    s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
    byte *barrel_definition = (byte *)&definition->barrels[barrel_index];
    long unit_index = NONE;
    if (weapon->item_flags & 1) unit_index = weapon->unit_index;
    long owner_index = function_101fb0(weapon_index);
    dword seed = (dword)weapon_index;
    long special_marker = NONE;
    bool create_projectile = true;
    bool network_projectile = false;
    bool synchronize = false;
    dword projectile_code = NONE;
    bool dual = function_101690(weapon_index);
    real scale = function_d1490(unit_index, weapon_index);
    long synchronized_target = NONE;
    short synchronized_node = NONE;
    point3f synchronized_point = { 0.0f };
    if (TEST_FIELD_BIT(definition->flag27)) scale = dual ? 1.0f : 0.0f;
    long mode = g_4e6948->mode;
    if (mode >= 4 && mode <= 5)
    {
        switch (WEAPON_SHORT_AT(barrel_definition, 0x34))
        {
        case 1:
            break;
        case 2:
            synchronize = true;
            break;
        default:
            synchronize = true;
            network_projectile = true;
            create_projectile = !weapon_game_mode_is_4();
            break;
        }
    }
    s_object_marker markers[64];
    short marker_count = function_b8d30(function_101ec0(weapon_index),
        function_101b10(barrel_index, weapon_index), markers, 64, false);
    if (!marker_count) marker_count = 1;
    if (!WEAPON_BIT_AT(barrel_definition, 0, 3)) marker_count = 1;
    point3f average;
    if (WEAPON_BIT_AT(barrel_definition, 0, 8))
    {
        average = *g_468788;
        for (short i = 0; i < marker_count; ++i)
        {
            average.x = markers[i].matrix.position.x + average.x;
            average.y = markers[i].matrix.position.y + average.y;
            average.z = markers[i].matrix.position.z + average.z;
        }
        real reciprocal = 1.0f / (real)marker_count;
        average.x *= reciprocal; average.y *= reciprocal; average.z *= reciprocal;
    }
    if (WEAPON_BYTE_AT(barrel, 4) & 0x20)
    {
        seed = seed * 0x19660d + 0x3c6ef35f;
        special_marker = (long)(seed >> 16) % marker_count;
    }
    long projectile_tag = WEAPON_LONG_AT(barrel_definition, 0x90);
    if (projectile_tag != NONE && owner_index != NONE)
    {
        long player_index = WEAPON_LONG_AT(WEAPON_GET(owner_index), 0x13c);
        if (player_index != NONE)
        {
            byte code = WEAPON_BYTE_AT(g_4e3b44[projectile_tag & 0xffff].bytes, 0x128) & 0x3f;
            projectile_code = function_15cca0(player_index, code);
        }
    }
    vector3f repeated_direction;
    for (short marker_index = 0; marker_index < marker_count; ++marker_index)
    {
        vector3f motion = *g_4687a4;
        vector3f unit_velocity;
        bool has_motion = false;
        real inherited_speed = 0.0f;
        real spread = 0.0f;
        real actor_speed = 0.0f;
        bool player_controlled = false;
        if (marker_count > 1 && WEAPON_BIT_AT(barrel_definition, 0, 12) &&
            WEAPON_BYTE_AT(barrel, 0x2e) % marker_count != marker_index) continue;
        s_object_marker const *marker = &markers[marker_index];
        point3f origin = marker->matrix.position;
        vector3f direction = marker->matrix.forward;
        s_entry_pair target = { NONE, NONE };
        if (!(TEST_FIELD_BIT(((s_weapon_barrel_definition *)barrel_definition)->flag6)) && unit_index != NONE)
        {
            byte *unit = (byte *)WEAPON_GET(unit_index);
            if (!(bool)((WEAPON_BYTE_AT(unit, 0x10a) >> 2) & 1))
            {
                long active_index = unit_index;
                long player_index = WEAPON_LONG_AT(unit, 0x13c);
                long actor_index = WEAPON_LONG_AT(unit, 0x12c);
                s_entry_pair *aim = (s_entry_pair *)(unit + 0x1c8);
                s_unit_definition const *unit_tag = (s_unit_definition const *)g_4e3b44[WEAPON_LONG_AT(unit, 0) & 0xffff].bytes;
                bool project = (bool)((unit_tag->unit.flags >> 3) & 1);
                bool use_aim = true;
                player_controlled = player_index != NONE;
                long parent = WEAPON_LONG_AT(unit, 0x24c);
                while (parent != NONE)
                {
                    active_index = parent;
                    byte *parent_bytes = (byte *)WEAPON_GET(parent);
                    actor_index = WEAPON_LONG_AT(parent_bytes, 0x12c);
                    player_index = WEAPON_LONG_AT(parent_bytes, 0x13c);
                    aim = (s_entry_pair *)(parent_bytes + 0x1c8);
                    parent = WEAPON_LONG_AT(parent_bytes, 0x24c);
                }
                if (actor_index != NONE && WEAPON_SHORT_AT(g_4f55f0->data + (actor_index & 0xffff) * 0x888, 0x6fe) == 4)
                    use_aim = false;
                if (WEAPON_LONG_AT(unit, 0x24c) != NONE) use_aim = false;
                function_c8290(active_index, &origin, &direction, &motion,
                    player_index != NONE ? (real *)(barrel_definition + 0x7c) : NULL, project, use_aim, true);
                has_motion = function_109e00(active_index, &unit_velocity, true);
                inherited_speed = direction.i * motion.i + direction.j * motion.j + direction.k * motion.k;
                if (inherited_speed < 0.0f) inherited_speed = 0.0f;
                if (WEAPON_BIT_AT(barrel_definition, 0, 3))
                {
                    origin = marker->matrix.position;
                    point3f center, clipped;
                    function_cafc0(unit_index, &center);
                    if (function_16a7c0(&center, &origin, function_baf40(unit_index), NONE, &clipped)) origin = clipped;
                }
                if (player_index != NONE)
                {
                    s_weapon_unit *original_unit = WEAPON_UNIT_GET(unit_index);
                    short primary_slot = (signed char)*((byte *)original_unit + 0x212);
                    long primary_index = primary_slot != NONE ? original_unit->weapon_indices[primary_slot] : NONE;
                    bool primary = primary_index == weapon_index;
                    if ((WEAPON_LONG_AT(barrel_definition, 0) & 1) && aim->object_index != NONE &&
                        WEAPON_REAL_AT(aim, 0x1c) > 0.0f)
                    {
                        if (!(function_106400(weapon_index, true) && barrel_index == 1 && !function_106320(aim)))
                            { target.object_index = aim->object_index; target.entry_index = aim->entry_index; }
                    }
                    else if (WEAPON_BIT_AT(barrel_definition, 0, 13) && weapon->object_index_194 != NONE)
                    {
                        target.object_index = weapon->object_index_194;
                        target.entry_index = weapon->entry_177;
                    }
                    else function_1a4c00(player_index, aim, primary, &direction, &origin);
                }
                else if (actor_index != NONE)
                {
                    bool aimed = function_1fddd0(actor_index,
                        WEAPON_BIT_AT(barrel_definition, 0, 8) ? &average : &origin, &direction,
                        (long *)&actor_speed, &spread, &target.object_index);
                    if (WEAPON_BIT_AT(barrel_definition, 0, 13) && target.object_index != NONE)
                    {
                        byte *header = (byte *)g_4e0300->data + (target.object_index & 0xffff) * 12;
                        byte type = header[3];
                        if ((type == 0 && WEAPON_SHORT_AT(WEAPON_GET(target.object_index), 0x1fc) == NONE) || (type != 0 && type != 1))
                            target.object_index = target.entry_index = NONE;
                    }
                    if (!aimed) continue;
                }
            }
        }
        if ((WEAPON_BYTE_AT(barrel, 4) & 0x20) && marker_index == special_marker)
        {
            real lower = WEAPON_REAL_AT(barrel_definition, 0x40) * 0.01745329238474369f;
            real upper = WEAPON_REAL_AT(barrel_definition, 0x44) * 0.01745329238474369f;
            real angle = lower + weapon_emission_random(&seed) * (upper - lower);
            real turn = weapon_emission_random(&seed) * 6.2831854820251465f;
            vector3f axis = marker->matrix.up;
            weapon_emission_rotate(&axis, &direction, turn);
            weapon_emission_rotate(&direction, &axis, angle);
        }
        long projectile_index = WEAPON_LONG_AT(barrel_definition, 0x90);
        short projectile_count = WEAPON_SHORT_AT(barrel_definition, 0x6a);
        if (barrel_index == 0 && WEAPON_SHORT_AT(weapon, 0x1a2) > 0)
        {
            projectile_index = WEAPON_LONG_AT(definition->barrels, 0x17c);
            projectile_count *= WEAPON_SHORT_AT(weapon, 0x1a2);
            WEAPON_SHORT_AT(weapon, 0x1a2) = 0;
        }
        word delayed = (word)WEAPON_SHORT_AT(barrel, 0x2c);
        if (delayed) { projectile_count = (short)(projectile_count * (delayed + 1)); WEAPON_SHORT_AT(barrel, 0x2c) = 0; }
        if (projectile_index != NONE)
        {
            long projectile_owner_index = function_101fb0(weapon_index);
            for (short i = 0; i < projectile_count; ++i)
            {
                bool tracer = false;
                s_effect_owner owner;
                owner.unknown0 = projectile_owner_index != NONE ? WEAPON_LONG_AT(WEAPON_GET(projectile_owner_index), 0x13c) : NONE;
                owner.unknown4 = projectile_owner_index;
                owner.unknown8 = projectile_owner_index != NONE ? WEAPON_SHORT_AT(WEAPON_GET(projectile_owner_index), 0x138) : NONE;
                s_weapon_projectile_creation creation;
                function_b7930(&creation, projectile_index, projectile_owner_index, &owner);
                creation.position = origin;
                creation.forward = direction;
                if (player_controlled) creation.flags |= 4;
                /* retail: a barrel whose +0x10 is zero always fires a tracer; otherwise the
                   signed shot counter at +0xe counts up to the definition's +0x2e */
                if (WEAPON_REAL_AT(barrel, 0x10) == 0.0f) tracer = true;
                else
                {
                    short previous = WEAPON_SHORT_AT(barrel, 0xe);
                    WEAPON_SHORT_AT(barrel, 0xe) = (short)(previous + 1);
                    if (previous >= WEAPON_SHORT_AT(barrel_definition, 0x2e)) tracer = true;
                }
                if (tracer) WEAPON_SHORT_AT(barrel, 0xe) = 0;
                if (spread == 0.0f)
                {
                    real low = WEAPON_REAL_AT(barrel_definition, 0x5c);
                    real high = WEAPON_REAL_AT(barrel_definition, 0x60);
                    if (low == 0.0f) low = WEAPON_REAL_AT(barrel_definition, 0x74);
                    if (high == 0.0f) high = WEAPON_REAL_AT(barrel_definition, 0x78);
                    low = low * scale + WEAPON_REAL_AT(barrel_definition, 0x74) * (1.0f - scale);
                    high = high * scale + WEAPON_REAL_AT(barrel_definition, 0x78) * (1.0f - scale);
                    spread = low + (high - low) * WEAPON_REAL_AT(barrel, 0x1c);
                }
                if (!(WEAPON_BIT_AT(barrel_definition, 0, 5) && ((WEAPON_BYTE_AT(weapon, 0x16e) >> 6) & 1)))
                {
                    real minimum = WEAPON_REAL_AT(barrel_definition, 0x58) * scale +
                        WEAPON_REAL_AT(barrel_definition, 0x70) * (1.0f - scale);
                    random_vector_in_cone(&g_4e7408->unknown0, &creation.forward, &creation.forward, minimum, spread);
                }
                if (WEAPON_BIT_AT(barrel_definition, 0, 7))
                {
                    if (!i) repeated_direction = creation.forward;
                    else creation.forward = repeated_direction;
                }
                real ax = (real)fabs(creation.forward.i), ay = (real)fabs(creation.forward.j), az = (real)fabs(creation.forward.k);
                if (ax <= ay && ax <= az) { creation.up.i = 0.0f; creation.up.j = creation.forward.k; creation.up.k = -creation.forward.j; }
                else if (!(ay <= az)) { creation.up.i = creation.forward.j; creation.up.j = -creation.forward.i; creation.up.k = 0.0f; }
                else { creation.up.i = -creation.forward.k; creation.up.j = 0.0f; creation.up.k = creation.forward.i; }
                normalize_inline(&creation.up);
                function_1055b0(&creation.forward, &creation.up, i, WEAPON_SHORT_AT(barrel_definition, 0x68),
                    WEAPON_REAL_AT(barrel_definition, 0x6c), projectile_count);
                creation.linear_velocity.i = creation.forward.i * inherited_speed;
                creation.linear_velocity.j = creation.forward.j * inherited_speed;
                creation.linear_velocity.k = creation.forward.k * inherited_speed;
                if (has_motion)
                {
                    real along = creation.forward.i * unit_velocity.i + creation.forward.j * unit_velocity.j +
                        creation.forward.k * unit_velocity.k;
                    creation.linear_velocity.i += unit_velocity.i - along * creation.forward.i;
                    creation.linear_velocity.j += unit_velocity.j - along * creation.forward.j;
                    creation.linear_velocity.k += unit_velocity.k - along * creation.forward.k;
                }
                if (actor_speed > 0)
                {
                    real speed = WEAPON_REAL_AT(g_4e3b44[WEAPON_LONG_AT(barrel_definition, 0x90) & 0xffff].bytes, 0x17c);
                    real desired = actor_speed;
                    if (speed > desired)
                    {
                        creation.linear_velocity.i += (desired - speed) * creation.forward.i;
                        creation.linear_velocity.j += (desired - speed) * creation.forward.j;
                        creation.linear_velocity.k += (desired - speed) * creation.forward.k;
                    }
                }
                if (create_projectile)
                {
                    long index = function_b7b40(&creation);
                    if (index != NONE)
                    {
                        byte *projectile = (byte *)WEAPON_GET(index);
                        WEAPON_REAL_AT(projectile, 0x188) = (1.0f - scale) + WEAPON_REAL_AT(barrel_definition, 0x64) * scale;
                        WEAPON_SHORT_AT(projectile, 0x1a8) = (short)projectile_code;
                        if (actor_speed > 0) WEAPON_LONG_AT(projectile, 0x12c) |= 0x100;
                        *(s_entry_pair *)((byte *)WEAPON_GET(index) + 0x144) = target;
                        if (!tracer) WEAPON_LONG_AT(WEAPON_GET(index), 0x12c) &= ~2;
                        if (network_projectile) function_a7870(index);
                        WEAPON_LONG_AT(projectile, 0x1a4) = weapon->definition_index;
                        function_fa100(index);
                        byte *projectile_tag_bytes = g_4e3b44[WEAPON_LONG_AT(projectile, 0) & 0xffff].bytes;
                        if (synchronize && WEAPON_SHORT_AT(barrel_definition, 0x34) == 2 && WEAPON_BIT_AT(projectile, 0x12c, 11) &&
                            WEAPON_BIT_AT(projectile_tag_bytes, 0xbc, 5) && WEAPON_BIT_AT(projectile, 0x12c, 13))
                        {
                            long parent_index = WEAPON_LONG_AT(projectile, 0x150);
                            if (WEAPON_LONG_AT(WEAPON_GET(parent_index), 0xd4) != NONE)
                            {
                                long node_count = 0;
                                transform4x3f *matrices = function_b8c00(parent_index, &node_count);
                                short node = WEAPON_SHORT_AT(projectile, 0x154);
                                if (node >= 0 && node < node_count)
                                {
                                    point3f point;
                                    transform4x3f inverse;
                                    function_b9dd0(index, &point);
                                    function_141590(&matrices[node], &inverse);
                                    transform4x3f_apply_point(&inverse, &point, &synchronized_point);
                                    synchronized_target = parent_index;
                                    synchronized_node = node;
                                }
                            }
                        }
                    }
                }
            }
        }
        long damage_tag = WEAPON_LONG_AT(barrel_definition, 0x98);
        if (g_4e6948->mode != 4 && damage_tag != NONE)
        {
            byte damage[0x88];
            memset(damage, 0, sizeof(damage));
            WEAPON_LONG_AT(damage, 0) = damage_tag;
            WEAPON_LONG_AT(damage, 4) |= 8;
            WEAPON_LONG_AT(damage, 8) = WEAPON_LONG_AT(damage, 0xc) = NONE;
            WEAPON_SHORT_AT(damage, 0x10) = NONE;
            WEAPON_LONG_AT(damage, 0x14) = NONE;
            WEAPON_LONG_AT(damage, 0x18) = unit_index;
            WEAPON_LONG_AT(damage, 0x1c) = NONE;
            WEAPON_SHORT_AT(damage, 0x20) = NONE;
            WEAPON_SHORT_AT(damage, 0x22) = g_4686c4;
            *(point3f *)(damage + 0x24) = origin;
            *(point3f *)(damage + 0x30) = origin;
            WEAPON_REAL_AT(damage, 0x54) = WEAPON_REAL_AT(damage, 0x58) = WEAPON_REAL_AT(damage, 0x5c) = 1.0f;
            *(vector3f *)(damage + 0x6c) = direction;
            WEAPON_SHORT_AT(damage, 0x7c) = g_47d8e0;
            WEAPON_SHORT_AT(damage, 0x7e) = NONE;
            damage[0x84] = barrel_definition[0x88] & 0x3f;
            object_get_root_location(weapon_index, (s_location *)(damage + 0x1c));
            if (unit_index != NONE) object_get_damage_owner(unit_index, (s_damage_owner *)(damage + 8));
            function_d6c80((s_type_1e6529 *)damage, unit_index);
        }
    }
    if (synchronize) function_a7d50(weapon_index, barrel_index,
        synchronized_target, synchronized_node, &synchronized_point);
}

// @retail 0xfd980
void __stdcall function_fd980(long weapon_index)
{
    long const *index_reference = &weapon_index;
    s_weapon *weapon = WEAPON_GET(weapon_index);
    s_weapon *const *weapon_reference = &weapon;
    s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
    for (long i = 0; i < definition->barrel_count; ++i)
    {
        s_weapon_barrel *barrel = &weapon->barrels[i];
        if (*(byte *)((byte *)barrel + 4) & 2)
        {
            function_104150(*index_reference, (short)i);
            *(byte *)((byte *)barrel + 4) &= 0xfd;
        }
    }
    struct s_flag_view { word enabled : 1; word other : 15; };
    s_flag_view *flag = (s_flag_view *)((byte *)*weapon_reference + 0x16c);
    flag->enabled = false;
}

#include "object_iterator.h"

// @retail 0xfd910
void function_fd910()
{
    struct
    {
        s_weapon *weapon;
        s_type_f1af8e iterator;
    } state;
    state.iterator.signature = 0x86868686;
    state.iterator.type_mask = 4;
    state.iterator.flags = 2;
    state.iterator.index = 0;
    state.iterator.object_index = NONE;
    while ((state.weapon = (s_weapon *)function_baeb0(&state.iterator)) != NULL)
        if (WEAPON_BYTE_AT(state.weapon, 0x16c) & 1)
            function_fd980(state.iterator.object_index);
}
