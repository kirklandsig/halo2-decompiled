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
				result = function_176780((s_effect_owner const *)NULL, owner_index, scale_a, tag_index, scale_b, NULL, NULL);
			break;
		case 'snd!':
			function_189060(unit_index, NONE, scale_a, g_468788, g_4687a8, tag_index);
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
// Shared data with result-first selection 43; typed holder headers 43; pool local 43;
// existing weapon getter at every lookup with result-first selection 41; result first 41;
// split comparison declaration 41; nested holder index 41; mixed pool/data locals 41;
// function-scope holder pointer 41; existing object-header getter 41.
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
// @retail 0x100b80
bool function_100b80(long magazine_index, long weapon_index, long count)
{
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	long available = function_1008f0(magazine_index, weapon_index, false);
	long definition_index = weapon->definition_index;
	bool result = false;
	long taken = 0;

	function_106030(weapon_index);
	if (available >= count)
	{

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
					WEAPON_GET(other_index)->definition_index == definition_index)
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

// score 3 (was 3): the word is stored at +0x254 where retail stores it at +0x256, so the two
// stores are in the wrong order. That is `pop esi`: ours pops it right after edi, before the
// inlined reset's two word stores; retail after them. Tried: the reset storing value250 before
// value258 3 (0x105be0 2); the reset's animation as one default object 18; 0x105be0 defined
// after this function 3; one short index for all three loops 3; the definition read after the
// field resets 110. A true result declared first 3, or just before the reset 3; loop indices
// declared at the top 3; the +0x17e word as a named field 3; magazine index declared in its loop 3.
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

PRIVATE __forceinline real weapon_activity_value(long weapon_index, s_weapon *weapon, s_weapon_definition *definition)
{
	real result = 0.0f;
	long i;
	for (i = 0; i < definition->barrel_count; i++)
	{
		real value = *(real *)((byte *)&weapon->barrels[i] + 0x18);
		result = result > value ? result : value;
	}
	for (i = 0; i < definition->trigger_count; i++)
	{
		s_weapon_trigger_definition *trigger_entry = &definition->triggers[i];
		real scale = *(real *)((byte *)trigger_entry + 0x28);
		if (trigger_entry->value_1c > 0.0f)
		{
			real value = function_102070(weapon_index, (short)i) * scale;
			result = result > value ? result : value;
		}
		if (weapon->triggers[i].state == 2)
		{
			real value = scale + (1.0f - scale) * *(real *)((byte *)weapon + 0x188);
			result = result > value ? result : value;
		}
	}
	real value = *(real *)((byte *)weapon + 0x180) * *(real *)((byte *)definition + 0x164);
	if (!(result > value)) result = value;
	return result;
}

// score 398 (was 398): one 27-label switch whose tree retail lays out inverted on the path to 0x1d0007ba
// (root, 0x1500056e and 0x1700056c fall through to the high side; no other function in the image has that node
// shape); frame eight versus four and object/tag/activity homes remain.
// Early activity plus final maximum 398; barrel read 422; scale reads 422; property bits 420; branch maxima 404;
// all maxima 398; joined scale 423; wide selector 424. Direct barrel maximum 413; helper maximum branches 409;
// independent loop indices 407; final accumulated maximum 409; size optimization 672. Helper input order 407;
// bool selector 408; tag-only helper lookup 407; object/tag homes 407; barrel bound sample 407 (fast).
// Published output test 407; scalar clamp 407; unsigned word selector 407; shared progress guard 413; shared
// barrel read 414; typed flags field 407. Descending progress classifier 407; direct short selectors 407;
// shared threshold 407; normalization range 407; shifted property flags 428.
// Early/default activity classification 404/406; suppressed admission 421;
// identifier partitions 0xbffffff/0x10000573/0x11ffffff/0x13ffffff/0x16ffffff 459/418/435/418/434.
// readonly activity inputs 407; normalization parts 407; elapsed tick expression 407; inventory case exit 407;
// progress field admission 407; progress case exits 407; magazine case exits 407; decimal digit expressions
// 407.
// Round 361: caller-seeded activity accumulator 398; entry result 399; barrel/trigger/both
// while traversals 398/398/398; entry result with barrel/trigger/both while traversals 399/399/399
// (fast). Existing helper and body restored.
// Separate activity/sparse tag lifetimes with result/helper variants 411 each;
// direct unknown-name exits 434 each (four of each). Entry/activity tag lookups
// with early/native activity dispatch 398/409; full object lookups 411/419.
// Four-byte frames regress; opening early-activity/final-maximum body restored at 398.
// Trigger fields at use/table pointer/table field reference 398/422/398; selected magazine pointer 398;
// with each trigger form 398/422/398; barrel array and direct maximum 423. All restored.
// Actual trigger-scale/threshold/capacity field references, pair/all combinations and readonly activity
// receivers 423/398/423/396/420/396/420/420. Original body and helpers restored.
// Round 163: Native trigger-state cursor with metadata/bound/tag-lookup combinations
// none/metadata/bound/metadata+bound/tag/metadata+tag/bound+tag/all
// 423/422/423/422/423/422/423/422. Each is 1623/1642 bytes and regresses.
// Original helper, field observations, maxima and tag ownership restored.
// Trigger metadata cursor 422; with barrel cursor, native bounds, both 422/422/422;
// cursor with field scale, both cursors with field scale 427/427; cursor with complete maxima 422;
// both cursors/bounds/field scale/maxima 426. All restored.
// Negative unknown-name bool/byte/short/long with original exits 398 each;
// single-exit guarded publication 471/471/462/466. All restored.
// Scalar accumulator/contribution/output names, alone and combined, 398 each;
// combined names with late constant found declaration 398. All eight restored.
// Round 251: Case-scoped barrel/trigger/both/magazine/barrel+magazine/trigger+magazine/all selectors 398/398/398/398/398/398/398; all with full-width selectors 424. Original shared word selector restored.
// Round 228: existing activity helper as final-only/direct output producer,
// original/native dispatch, supplied/helper-owned tag: 398/415/409/425/398/415/409/425. All restored.
// Round 202: Full name switch with unknown-name exit/classifier/upper-first output subsets
// 430/412/409/414/411/415/403; all with direct activity publication 411.
// Original activity admission, dispatch, classifier and output normalization restored.
// Round 172: existing activity operation as weapon/tag member, mutable/readonly receiver and
// other object by pointer/native reference, all eight 398. Original free helper, record declarations
// and marked body restored; frame eight/four and object/tag/activity homes remain.
// Round 381: double activity accumulator 554; double helper return 398; double caller result 710;
// accumulator/return 554, accumulator/caller 810, return/caller 710; all 810,
// all with readonly helper receivers 810. Original real helper/caller carriers restored.
// Round 32: native predicate-to-real producers for complete inventory/heat, progress,
// property-bit, inventory/progress, inventory/bits, progress/bits, all families
// 410/412/419/426/426/431/442; all with native final activity publication 441. All restored.
// Round 168: Matched inventory/admission/progress query inline subsets 1/2/3/4/5/6/7 score 455/453/451/434/451/453/451; all with separate noinline declarations 451. Original six declaring files restored.
// Round 404: Complete property dispatch and direct barrel-field maximum with metadata/state/scale
// combinations 415/429/429/430/429/433/421/435. Original helper and dispatcher restored; frame eight/four
// and tag/activity homes remain.
// Round 406: Frame eight versus four and object/tag/activity homes remain. Native unsigned property
// carrier with early or complete dispatch, consistently per-use tag reads and machine-width
// selector combinations; per-use tags recover frame four but grow the body. Scores
// 398/409/602/617/424/417/558/568. All restored.
// Round 410: Complete callback object observations with complete dispatch, early zero-result
// lifetime and full-width selector subsets 398/409/399/410/424/417/425/418. Original object
// receiver, result lifetime, signed-word selector, dispatch and activity helper restored.
// Round 429: One candidate shared by all four activity maxima, with candidate priority, scale
// scope and independent ordinals: all eight 398. Original helper/body restored.
// Round 300: Actual barrel maximum field reference, both callback scalar field references,
// both, interpolation complement, barrel/complement, fields/complement, all and all with readonly
// receivers 423/398/423/398/423/398/423/423. Original helper and body restored at 398.
// Round 87: one switch with the activity names inside (if-assignment forms, long selectors) 410; reversed case
// order 410 (same code); separate activity cases 845; helper-owned weapon and definition lookups 435 (frame four);
// helper-owned definition lookup 410; merged activity labels with a name test 410/410; default first 410; found
// flag with `if (!found) return false` 408 (places the default and activity bodies as retail does), with
// helper-owned lookups 432; plain inline/static helper not inlined 704/694. Compile-only screens: the root stays
// `jg` for 30 dispatch/result/helper combinations, nested switches on the name, trivialised cases, /O1, /Os, /Ox.
// Round 17: retail's 0x0c000569 is an ordinary case jumping into the middle of 0x1d0007ba's body, so
// the source is `case 0x1d0007ba: if (flag) break;` falling through into `case 0x0c000569:`; that form
// 409 (aligned diff 406 -> 371, the root still `jg`); on it the 0x16e flags as `(byte >> k) & 1` (the
// form retail tests) 409, a long selector 417, helper-owned definition lookup 409, both activity
// cases last 409. Compile-only: fallthrough first or last, or the flag case in an `if` before the
// switch, all keep the `jg` root.
// Round 94: progress tests ordered 0x0f0006e5/0x0e0006e6/0x0d0006e7 with no trailing else plus
// `*active = result > 0.0f;` 420 (result in XMM2, tail as retail); plus guard/stale-time ternaries 422;
// plus the fallthrough switch 429; plus barrel maximum and trigger scale read at each use in the activity
// helper 407 (1642 bytes; activity body as retail except EBP holds the trigger count, not the weapon, and
// XMM2 is re-zeroed); that with the fallthrough switch 405. Retail recomputes the definition only on the
// 0x1d0007ba path. A 27-value LTCG test switch keeps the `jg` root with loop, fallthrough, goto and return
// cases.
// @retail 0xff5f0
bool __stdcall function_ff5f0(long weapon_index, long name, real *value, bool *active)
{
	bool found = true;
	s_weapon *weapon = WEAPON_GET(weapon_index);
	s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
	real result = 0.0f;
	short index;
	if (name == 0x1d0007ba || name == 0x0c000569)
	{
		if (name != 0x1d0007ba || !((byte *)g_4de2f4)[0x82])
			result = weapon_activity_value(weapon_index, weapon, definition);
	}
	else switch (name)
	{
	case 0x03000577: result = weapon->heat; break;
	case 0x04000567: result = *(real *)((byte *)weapon + 0x180); break;
	case 0x05000024: result = function_10cf50(weapon_index) ? 1.0f : 0.0f; break;
	case 0x05000081: result = *(real *)((byte *)weapon + 0x18c); break;
	case 0x09000568:
		if (!function_10cf50(weapon_index) || ((bool)(((dword)*(word *)((byte *)weapon + 0x16c) >> 5) & 1)))
			result = weapon->heat < 1.0f ? 1.0f : 0.0f;
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
		if (g_4e6948->state == 2 && *(short *)((byte *)weapon + 0x17e) != NONE && function_162b10(weapon_index))
		{
			real progress = function_162b70(weapon_index);
			if (name == 0x0d0006e7)
				result = progress == 1.0f ? 1.0f : 0.0f;
			else if (name == 0x0e0006e6)
				result = progress > 0.0f && progress < 1.0f ? 1.0f : 0.0f;
			else if (name == 0x0f0006e5)
				result = progress == 0.0f ? 1.0f : 0.0f;
			else
				result = progress;
		}
		break;
	case 0x120006e4:
		if (g_4e6948->state == 2 && *(short *)((byte *)weapon + 0x17e) != NONE && function_162b10(weapon_index))
			result = function_162b70(weapon_index);
		break;
	case 0x0e000572:
	case 0x10000573:
		index = name == 0x0e000572 ? 0 : 1;
		if (index < definition->barrel_count && (long)(g_510c54->game_time - *(long *)((byte *)weapon + 0x248)) <= 1)
			result = *(real *)((byte *)&weapon->barrels[index] + 0x10);
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
		if (index < definition->magazine_count && definition->magazines[index].rounds_loaded_maximum != 0)
			result = (real)weapon->magazines[index].rounds_loaded / (real)definition->magazines[index].rounds_loaded_maximum;
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
	case 0x14000578: result = (weapon->value_16e & 2) ? 1.0f : 0.0f; break;
	case 0x16000579: result = (weapon->value_16e & 4) ? 1.0f : 0.0f; break;
	case 0x1700056b:
		if (definition->magazine_count > 0)
			result = (real)(weapon->magazines[0].rounds_loaded % 10) * 0.1f;
		break;
	case 0x1700056c:
		if (definition->magazine_count > 0)
			result = (real)((weapon->magazines[0].rounds_loaded / 10) % 10) * 0.1f;
		break;
	default: found = false; break;
	}
	if (!found) return false;
	if (result < 0.0f)
		result = 0.0f;
	else if (result > 1.0f)
		result = 1.0f;
	*value = result;
	if (result > 0.0f) *active = true;
	else *active = false;
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
void function_a88a0(long player_index, long definition_index, long other_definition_index, long count);

struct s_ammo_conversion_entry
{
    short count;
    byte unknown02[6];
    long definition_index;
};

// score 190 (was 190): Frame 0x20 agrees; destination/tag homes and conversion staging remain.
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
        function_a88a0(player_index, weapon->definition_index, other->definition_index, *rounds);
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

#define WEAPON_WORD_AT_102FB0(base, offset) (*(word *)((byte *)(base) + (offset)))

PRIVATE __forceinline long unit_player_index_102fb0(long unit_index)
{
    s_weapon_unit *unit = (s_weapon_unit *)function_badc0(unit_index, 3);
    return unit ? unit->player_index : NONE;
}

// score 604 (was 604): Frame 0x1c64 versus 0x1c60; weapon/barrel and definition/holder homes remain.
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
// @retail 0x102fb0
void __stdcall function_102fb0(long weapon_index, short barrel_index)
{
    s_weapon *weapon = WEAPON_GET(weapon_index);
    s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
    s_weapon_barrel_definition *barrel_definition = &definition->barrels[barrel_index];
    s_weapon_barrel *barrel = &weapon->barrels[barrel_index];
    volatile long unit_index;
    if (TEST_FIELD_BIT(weapon->in_inventory)) unit_index = weapon->unit_index;
    else unit_index = NONE;
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
            (!(bool)(((dword)WEAPON_LONG_AT(definition, 0x12c) >> 11) & 1) || !(weapon->heat >= 1.0f) ||
                function_106030(weapon_index)) && magazine->rounds_loaded >= barrel_definition->rounds_per_shot &&
            (!(bool)(((dword)WEAPON_LONG_AT(definition, 0x12c) >> 19) & 1) || unit_index == NONE ||
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
    else if (!(bool)(((dword)WEAPON_LONG_AT(definition, 0x12c) >> 11) & 1) || !(weapon->heat >= 1.0f))
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
        function_105fa0(weapon_index, failure ? (barrel_index != 0) + 3 : (barrel_index != 0) + 1);
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
        if (!(bool)(((dword)WEAPON_LONG_AT(definition, 0x12c) >> 26) & 1))
            function_101db0(weapon_index, barrel_definition->value_a8);
        function_1058b0(weapon_index, (barrel_index != 0) + 1, false);
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
            if (unit_player_index_102fb0(unit_index) != NONE)
            {
                long player_slot = *(short *)(g_4e8c24->data + (unit_player_index_102fb0(unit_index) & 0xffff) * 0x21c + 0x28);
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
    if ((bool)(((dword)WEAPON_LONG_AT(barrel_definition, 0) >> 12) & 1))
    {
        s_object_marker markers[64];
        long marker_name = function_101b10(weapon_index, barrel_index);
        long owner_index = function_101ec0(weapon_index);
        short count = function_b8d30(owner_index, marker_name, markers, 64, false);
        if (count >= 1)
        {
            long selected = WEAPON_BYTE_AT(barrel, 0x2e) % count;
            long model_owner_102fb0 = function_101ec0(weapon_index);
            s_effect_marker marker;
            marker.name = marker_name;
            marker.position = markers[selected].matrix.position;
            marker.forward = markers[selected].node_matrix.forward;
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
// score 1420 (was 1420): Frame 0x3c/0x38 and pool/offset/dual/scalar homes remain.
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
// @retail 0xfdfb0
bool __stdcall function_fdfb0(long weapon_index)
{
    s_weapon *weapon = WEAPON_GET(weapon_index);
    s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
    bool dual = function_101690(weapon_index);
    long unit_index = weapon_get_owner_unit_index(weapon_index);
    real scale = function_d1490(unit_index, weapon_index);
    long player_index = NONE;
    if ((bool)((WEAPON_LONG_AT(definition, 0x12c) >> 27) & 1)) scale = dual ? 1.0f : 0.0f;
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
    if ((bool)((WEAPON_LONG_AT(definition, 0x12c) >> 10) & 1) && weapon->parent_index == NONE &&
        (bool)((WEAPON_LONG_AT(weapon, 4) >> 8) & 1)) function_10d4e0(weapon_index);
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
    short blocked = (updated->value_16e & 0x20) || updated->state_ticks > 0 || (WEAPON_BYTE_AT(updated, 0x16c) & 2);
    if (blocked) { weapon->value_174 = 0; result = true; }
    for (short i = 0; i < definition->magazine_count; ++i) result = result | function_fdd50(weapon_index, i);
    for (short i = 0; i < definition->trigger_count; ++i)
    {
        s_weapon_trigger *trigger = &weapon->triggers[i];
        s_weapon_trigger_definition *trigger_entry = &definition->triggers[i];
        if (trigger->timer > 0) { --trigger->timer; result = true; }
        short input = WEAPON_SHORT_AT(trigger_entry, 4);
        short down = !blocked && (pressed[input] || (signed char)WEAPON_BYTE_AT(trigger, 4) < 0);
        short up = !blocked && released[input];
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
            WEAPON_BYTE_AT(weapon, 0x175) = (byte)(long)(((double)increment + old) * 255.0);
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

struct s_entry_pair { long object_index; long entry_index; };
struct s_damage_owner;
struct s_type_1e6529;
struct s_location;
void __stdcall function_c8290(vector3f *aim, point3f *point, long unit_index, vector3f *velocity,
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
void random_vector_in_cone(vector3f const *forward, vector3f *result, dword *seed, real min_angle, real max_angle);
real function_30bf0(vector3f *vector);
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

// @retail 0x104150
void __stdcall function_104150(long weapon_index, short barrel_index)
{
    long const *weapon_reference = &weapon_index;
    short const *barrel_reference = &barrel_index;
    s_weapon *weapon = WEAPON_GET(*weapon_reference);
    s_weapon_barrel *barrel = &weapon->barrels[*barrel_reference];
    s_weapon_definition *definition = WEAPON_DEFINITION(weapon);
    byte *barrel_definition = (byte *)&definition->barrels[*barrel_reference];
    long unit_index = NONE;
    if (weapon->item_flags & 1) unit_index = weapon->unit_index;
    long owner_index = function_101fb0(*weapon_reference);
    dword seed = (dword)*weapon_reference;
    long special_marker = NONE;
    bool create_projectile = true;
    bool network_projectile = false;
    bool synchronize = false;
    short projectile_code = NONE;
    bool dual = function_101690(*weapon_reference);
    real scale = function_d1490(unit_index, *weapon_reference);
    long synchronized_target = NONE;
    short synchronized_node = NONE;
    point3f synchronized_point = *g_468788;
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
    short marker_count = function_b8d30(function_101ec0(*weapon_reference),
        function_101b10(*barrel_reference, *weapon_reference), markers, 64, false);
    if (!marker_count) marker_count = 1;
    if (!(WEAPON_LONG_AT(barrel_definition, 0) & 8)) marker_count = 1;
    point3f average;
    if (WEAPON_LONG_AT(barrel_definition, 0) & 0x100)
    {
        average = *g_468788;
        for (short i = 0; i < marker_count; ++i)
        {
            average.x += markers[i].matrix.position.x;
            average.y += markers[i].matrix.position.y;
            average.z += markers[i].matrix.position.z;
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
            byte code = (WEAPON_BYTE_AT(g_4e3b44[projectile_tag & 0xffff].bytes, 0x128) & 0x3f) |
                (network_projectile ? 0x40 : 0);
            projectile_code = function_15cca0(player_index, code);
        }
    }
    vector3f repeated_direction;
    for (long marker_index = 0; marker_index < marker_count; ++marker_index)
    {
        s_object_marker const *marker = &markers[marker_index];
        if (marker_count > 1 && (bool)(((dword)WEAPON_LONG_AT(barrel_definition, 0) >> 12) & 1) &&
            WEAPON_BYTE_AT(barrel, 0x2e) % marker_count != marker_index) continue;
        point3f origin = marker->matrix.position;
        vector3f direction = marker->matrix.forward;
        vector3f motion = *g_4687a4;
        real inherited_speed = 0.0f, spread = 0.0f;
        real actor_speed = 0.0f;
        bool has_motion = false, player_controlled = false;
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
                bool project = (bool)((WEAPON_LONG_AT(g_4e3b44[WEAPON_LONG_AT(unit, 0) & 0xffff].bytes, 0xbc) >> 3) & 1);
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
                function_c8290(&direction, &origin, active_index, &motion,
                    player_index != NONE ? (real *)(barrel_definition + 0x7c) : NULL, project, use_aim, true);
                has_motion = function_109e00(active_index, &motion, true);
                inherited_speed = direction.i * motion.i + direction.j * motion.j + direction.k * motion.k;
                if (inherited_speed < 0.0f) inherited_speed = 0.0f;
                if (WEAPON_LONG_AT(barrel_definition, 0) & 8)
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
                    bool primary = primary_index == *weapon_reference;
                    if ((WEAPON_LONG_AT(barrel_definition, 0) & 1) && aim->object_index != NONE &&
                        WEAPON_REAL_AT(aim, 0x1c) > 0.0f)
                    {
                        if (!(function_106400(*weapon_reference, true) && *barrel_reference == 1 && !function_106320(aim)))
                            target = *aim;
                    }
                    else if ((bool)(((dword)WEAPON_LONG_AT(barrel_definition, 0) >> 13) & 1) && weapon->object_index_194 != NONE)
                    {
                        target.object_index = weapon->object_index_194;
                        target.entry_index = weapon->entry_177;
                    }
                    else function_1a4c00(player_index, aim, primary, &direction, &origin);
                }
                else if (actor_index != NONE)
                {
                    bool aimed = function_1fddd0(actor_index, &origin,
                        (WEAPON_LONG_AT(barrel_definition, 0) & 0x100) ? (vector3f *)&average : &direction,
                        (long *)&actor_speed, &spread, &target.object_index);
                    if ((bool)(((dword)WEAPON_LONG_AT(barrel_definition, 0) >> 13) & 1) && target.object_index != NONE)
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
        if (*barrel_reference == 0 && WEAPON_SHORT_AT(weapon, 0x1a2) > 0)
        {
            projectile_index = WEAPON_LONG_AT(definition->barrels, 0x17c);
            projectile_count *= WEAPON_SHORT_AT(weapon, 0x1a2);
            WEAPON_SHORT_AT(weapon, 0x1a2) = 0;
        }
        word delayed = (word)WEAPON_SHORT_AT(barrel, 0x2c);
        if (delayed) { projectile_count = (short)(projectile_count * (delayed + 1)); WEAPON_SHORT_AT(barrel, 0x2c) = 0; }
        owner_index = function_101fb0(*weapon_reference);
        for (short i = 0; projectile_index != NONE && i < projectile_count; ++i)
        {
            s_effect_owner owner;
            owner.unknown0 = owner_index != NONE ? WEAPON_LONG_AT(WEAPON_GET(owner_index), 0x13c) : NONE;
            owner.unknown4 = owner_index;
            owner.unknown8 = owner_index != NONE ? WEAPON_SHORT_AT(WEAPON_GET(owner_index), 0x138) : NONE;
            byte creation[0xc4];
            function_b7930(creation, projectile_index, owner_index, &owner);
            *(point3f *)(creation + 0x1c) = origin;
            *(vector3f *)(creation + 0x28) = direction;
            if (player_controlled) WEAPON_LONG_AT(creation, 0x18) |= 4;
            /* retail: a barrel whose +0x10 is zero always fires a tracer; otherwise the
               signed shot counter at +0xe counts up to the definition's +0x2e */
            bool tracer = WEAPON_REAL_AT(barrel, 0x10) == 0.0f;
            if (!tracer)
            {
                short previous = WEAPON_SHORT_AT(barrel, 0xe);
                WEAPON_SHORT_AT(barrel, 0xe) = (short)(previous + 1);
                if (previous >= WEAPON_SHORT_AT(barrel_definition, 0x2e)) tracer = true;
            }
            if (tracer) WEAPON_SHORT_AT(barrel, 0xe) = 0;
            vector3f *shot_direction = (vector3f *)(creation + 0x28);
            real shot_spread = spread;
            if (shot_spread != 0.0f)
            {
                real low = WEAPON_REAL_AT(barrel_definition, 0x5c);
                real high = WEAPON_REAL_AT(barrel_definition, 0x60);
                if (low != 0.0f) low = WEAPON_REAL_AT(barrel_definition, 0x74);
                if (high != 0.0f) high = WEAPON_REAL_AT(barrel_definition, 0x78);
                low = low * scale + WEAPON_REAL_AT(barrel_definition, 0x74) * (1.0f - scale);
                high = high * scale + WEAPON_REAL_AT(barrel_definition, 0x78) * (1.0f - scale);
                shot_spread = low + (high - low) * WEAPON_REAL_AT(barrel, 0x1c);
            }
            if (!((WEAPON_LONG_AT(barrel_definition, 0) & 0x20) && (WEAPON_BYTE_AT(weapon, 0x16e) & 0x40)))
            {
                real minimum = WEAPON_REAL_AT(barrel_definition, 0x58) * scale +
                    WEAPON_REAL_AT(barrel_definition, 0x70) * (1.0f - scale);
                random_vector_in_cone(shot_direction, shot_direction, &g_4e7408->unknown0, minimum, shot_spread);
            }
            if (WEAPON_LONG_AT(barrel_definition, 0) & 0x80)
            {
                if (!i) repeated_direction = *shot_direction;
                else *shot_direction = repeated_direction;
            }
            vector3f up;
            real ax = (real)fabs(shot_direction->i), ay = (real)fabs(shot_direction->j), az = (real)fabs(shot_direction->k);
            if (ax <= ay && ax <= az) { up.i = 0.0f; up.j = shot_direction->k; up.k = -shot_direction->j; }
            else if (!(ay <= az)) { up.i = shot_direction->j; up.j = -shot_direction->i; up.k = 0.0f; }
            else { up.i = -shot_direction->k; up.j = 0.0f; up.k = shot_direction->i; }
            function_30bf0(&up);
            *(vector3f *)(creation + 0x34) = up;
            function_1055b0(shot_direction, &up, i, WEAPON_SHORT_AT(barrel_definition, 0x68),
                WEAPON_REAL_AT(barrel_definition, 0x6c), projectile_count);
            vector3f velocity;
            velocity.i = shot_direction->i * inherited_speed;
            velocity.j = shot_direction->j * inherited_speed;
            velocity.k = shot_direction->k * inherited_speed;
            if (has_motion)
            {
                real along = shot_direction->i * motion.i + shot_direction->j * motion.j + shot_direction->k * motion.k;
                velocity.i += motion.i - along * shot_direction->i;
                velocity.j += motion.j - along * shot_direction->j;
                velocity.k += motion.k - along * shot_direction->k;
            }
            if (actor_speed > 0)
            {
                real speed = WEAPON_REAL_AT(g_4e3b44[WEAPON_LONG_AT(barrel_definition, 0x90) & 0xffff].bytes, 0x17c);
                real desired = actor_speed;
                if (speed > desired)
                {
                    velocity.i += (desired - speed) * shot_direction->i;
                    velocity.j += (desired - speed) * shot_direction->j;
                    velocity.k += (desired - speed) * shot_direction->k;
                }
            }
            *(vector3f *)(creation + 0x40) = velocity;
            if (create_projectile)
            {
                long index = function_b7b40(creation);
                if (index != NONE)
                {
                    byte *projectile = (byte *)WEAPON_GET(index);
                    WEAPON_REAL_AT(projectile, 0x188) = (1.0f - scale) + WEAPON_REAL_AT(barrel_definition, 0x64) * scale;
                    WEAPON_SHORT_AT(projectile, 0x1a8) = projectile_code;
                    if (actor_speed > 0) WEAPON_LONG_AT(projectile, 0x12c) |= 0x100;
                    WEAPON_LONG_AT(projectile, 0x144) = target.object_index;
                    WEAPON_LONG_AT(projectile, 0x148) = target.entry_index;
                    if (!tracer) WEAPON_LONG_AT(projectile, 0x12c) &= ~2;
                    if (network_projectile) function_a7870(index);
                    WEAPON_LONG_AT(projectile, 0x1a4) = weapon->definition_index;
                    function_fa100(index);
                    byte *projectile_tag_bytes = g_4e3b44[WEAPON_LONG_AT(projectile, 0) & 0xffff].bytes;
                    if (synchronize && WEAPON_SHORT_AT(barrel_definition, 0x34) == 2 && (WEAPON_LONG_AT(projectile, 0x12c) & 0x800) &&
                        (WEAPON_LONG_AT(projectile_tag_bytes, 0xbc) & 0x20) && (WEAPON_LONG_AT(projectile, 0x12c) & 0x2000))
                    {
                        long parent_index = WEAPON_LONG_AT(projectile, 0x150);
                        if (WEAPON_LONG_AT(WEAPON_GET(parent_index), 0xd4) != NONE)
                        {
                            long node_count;
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
            object_get_root_location(*weapon_reference, (s_location *)(damage + 0x1c));
            if (unit_index != NONE) object_get_damage_owner(unit_index, (s_damage_owner *)(damage + 8));
            function_d6c80((s_type_1e6529 *)damage, unit_index);
        }
    }
    if (synchronize) function_a7d50(*weapon_reference, *barrel_reference,
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
