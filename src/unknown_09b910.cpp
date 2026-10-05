// @flags /O2 /Gr
/* UNKNOWN_09B910.CPP: the item, projectile, weapon and device object types
   and the small projectile, weapon and game engine event definitions */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0a58d0.h"
#include "object_types_21_2.h"
#include <string.h>

#define OBJECT_FROM_INDEX(index) \
	((byte *)((s_object_header *)g_4e0300->data)[(index) & 0xffff].object)

/* the projectile object, as seen by v26 */
struct s_projectile_object
{
	long definition_index;
	byte unknown04[0x16];
	short field1a;
	byte unknown1c[8];
	long field24;
	byte unknown28[0xaf - 0x28];
	byte field_af;
	byte unknownb0[0xc4 - 0xb0];
	long field_c4;
	byte unknownc8[0xd4 - 0xc8];
	long field_d4;
	byte unknownd8[0x12c - 0xd8];
	dword flags_12c;
	short field_130;
	byte unknown132[0x144 - 0x132];
	long field_144;
	short field_148;
};

/* the projectile's state: 0x20 bytes */
struct s_projectile_state
{
	long field0;
	long field4;
	byte field8;
	long fieldc;
	long field10;
	long field14;
	long field18;
	byte field1c;
	byte field1d;
};

/* the item, weapon and device state: 0x10 bytes */
struct s_simple_state
{
	long field0;
	long field4;
	union
	{
		byte field8;
		long field8_long;
	};
	long fieldc;
};

static void simple_state_clear(s_simple_state *state)
{
	memset(state, 0, sizeof(s_simple_state));
}

/* the object, as seen by the small v26 methods */
struct s_simple_object
{
	long definition_index;
	byte unknown04[0x16];
	short field1a;
	byte unknown1c[8];
	long field24;
	byte unknown28[0xaf - 0x28];
	byte field_af;
};

/* the entity info of a projectile: the identifier is at +0x14 */
struct s_projectile_entity_info
{
	byte unknown00[0x10];
	long field10;
	long identifier;
	long field18;
};

struct s_projectile_entity
{
	long field0;
	byte unknown04[4];
	long object_index;
	long field8;
	byte unknown10[4];
	s_projectile_entity_info *info;
};

/* the projectile entity: the creation data for v9 */
struct s_projectile_creation
{
	byte unknown00[0x14];
	s_projectile_entity_info *info;
};

/* the game engine event's definition data */
struct s_event_data
{
	byte unknown00[4];
	long field4;
	long field8;
	long fieldc;
	long field10;
	long field14;
	long field18;
	long field1c;
};

struct s_event_holder
{
	byte unknown00[0x18];
	s_event_data *data;
};

struct s_event_mask
{
	byte unknown00[8];
	dword mask;
};

struct s_boot_event_data
{
	byte unknown00[0xc];
	bool field_c;
	byte unknownd[0x38 - 0xd];
	bool field_38;
};

struct s_boot_event_holder
{
	byte unknown00[0x18];
	s_boot_event_data *data;
};

struct s_flag_event_data
{
	byte unknown00[0x1c];
	byte flags;
};

struct s_flag_event_holder
{
	byte unknown00[0x18];
	s_flag_event_data *data;
};

// ---- item ----

// @retail 0xa0260
const char *c_item_type::v1()
{
	return "item";
}

// @retail 0xa0270
long c_item_type::v2()
{
	return 0x94;
}

// @retail 0xa0280
long c_item_type::v3()
{
	return 0x10;
}

// @retail 0xa0290
long c_item_type::v5()
{
	return 0x43f;
}

// @retail 0xa0430
void c_item_type::v9(long a, long b, long *size)
{
	*size = 0x65;
}

// @retail 0xa3d60
void c_item_type::v26(long index, long b, s_entity_state *state)
{
	s_simple_state *s = (s_simple_state *)state;
	simple_state_clear(s);
	s_simple_object *object = (s_simple_object *)OBJECT_FROM_INDEX(index);
	s->field4 = object->definition_index;
	s->field0 = object->field1a;
	s->field8 = object->field_af;
	s->fieldc = object->field24;
}

// ---- projectile ----

// @retail 0xa0ad0
long c_projectile_type::v0()
{
	return 0xd;
}

// @retail 0xa0ae0
const char *c_projectile_type::v1()
{
	return "projectile";
}

// @retail 0xa0af0
long c_projectile_type::v3()
{
	return 0x20;
}

// @retail 0xa0e90
void c_projectile_type::v9(long a, long b, long *size)
{
	s_projectile_entity_info *info = ((s_projectile_creation *)a)->info;
	long extra = 0;
	if (info->identifier != NONE)
	{
		extra = 1 + (info->field18 == NONE ? 0 : 5);
	}
	*size = 0x69 + extra + (info->field10 == NONE ? 0 : 4) + (info->identifier == NONE ? 0 : 0xe);
}

// @retail 0xa11f0
void c_projectile_type::v21(s_entity *entity)
{
	s_projectile_entity_info *info = ((s_projectile_entity *)entity)->info;
	if (entity->object_index != NONE)
	{
		((s_projectile_object *)OBJECT_FROM_INDEX(entity->object_index))->field_d4 = entity->field0;
	}
	if (info->identifier != NONE)
	{
		long identifier = info->identifier;
		byte salt = (byte)(((long)((dword)identifier >> 28) + 1) % 16);
		info->identifier = (salt << 28) | (identifier & 0x3ff);
	}
}

// @retail 0xa0b00
void c_projectile_type::v26(long index, long b, s_entity_state *state)
{
	s_projectile_state *s = (s_projectile_state *)state;
	s_projectile_object *definition = (s_projectile_object *)OBJECT_FROM_INDEX(index);
	memset(s, 0, sizeof(s_projectile_state));
	s_projectile_object *object = (s_projectile_object *)OBJECT_FROM_INDEX(index);
	s->field4 = object->definition_index;
	s->field0 = object->field1a;
	s->field8 = object->field_af;
	s->fieldc = object->field24;
	if (definition->field_c4 != NONE)
		s->field10 = definition->field_c4 & 0xffff;
	else
		s->field10 = NONE;
	s->field14 = NONE;
	s->field18 = NONE;
	if (definition->field_144 != NONE)
	{
		s->field14 = ((s_projectile_object *)OBJECT_FROM_INDEX(definition->field_144))->field_d4;
		s->field18 = definition->field_148;
	}
	s->field1c = (definition->flags_12c >> 1) & 1;
	s->field1d = (definition->flags_12c >> 8) & 1;
}

// @retail 0xa0dd0
bool c_projectile_type::v30(long index)
{
	s_projectile_object *object = (s_projectile_object *)OBJECT_FROM_INDEX(index);
	if (object->field_130 < 2)
		object->field_130 = 2;
	return false;
}

// ---- weapon ----

// @retail 0xa1e80
const char *c_weapon_type::v1()
{
	return "weapon";
}

// @retail 0xa1e90
long c_weapon_type::v2()
{
	return 0xa0;
}

// @retail 0xa1ea0
long c_weapon_type::v5()
{
	return 0x3c3f;
}

// @retail 0xa1eb0
void c_weapon_type::v26(long index, long b, s_entity_state *state)
{
	s_simple_state *s = (s_simple_state *)state;
	simple_state_clear(s);
	simple_state_clear(s);
	s_simple_object *object = (s_simple_object *)OBJECT_FROM_INDEX(index);
	s->field4 = object->definition_index;
	s->field0 = object->field1a;
	s->field8 = object->field_af;
	s->fieldc = object->field24;
}

// ---- device ----

// @retail 0xa3d30
const char *c_device_type::v1()
{
	return "device";
}

// @retail 0xa3d40
long c_device_type::v2()
{
	return 0x98;
}

// @retail 0xa3d50
long c_device_type::v5()
{
	return 0xc3f;
}

// @retail 0xa4090
bool c_device_type::v34(long a, s_entity_data *source, long *block)
{
	return false;
}

// ---- projectile events ----

// @retail 0xa1860
const char *c_projectile_impact_effect_event::v1()
{
	return "projectile-impact-effect";
}

// @retail 0xa1870
void c_projectile_impact_effect_event::v6(void *a, long b, long *size)
{
	*size = 0x4d;
}

// @retail 0xa15c0
const char *c_projectile_effect_event::v1()
{
	return "projectile-effect";
}

// @retail 0xa15d0
long c_projectile_effect_event::v2()
{
	return 0x30;
}

// @retail 0xa15e0
void c_projectile_effect_event::v6(void *a, long b, long *size)
{
	s_flag_event_data *data = ((s_flag_event_holder *)a)->data;
	*size = 0x3e;
	if (data->flags & 4)
		*size = 0x5b;
}

// @retail 0xa1b00
const char *c_projectile_object_impact_effect_event::v1()
{
	return "projectile-object-impact-effect";
}

// @retail 0xa1b10
long c_projectile_object_impact_effect_event::v2()
{
	return 0x3c;
}

// @retail 0xa1b20
void c_projectile_object_impact_effect_event::v6(void *a, long b, long *size)
{
	*size = 0x55;
}

// @retail 0xa13b0
const char *c_projectile_attached_event::v1()
{
	return "projectile-attached";
}

// @retail 0xa13c0
void c_projectile_attached_event::v6(void *a, long b, long *size)
{
	*size = 0x6f;
}

// ---- weapon events ----

// @retail 0xa3170
const char *c_weapon_put_away_event::v1()
{
	return "weapon-put-away";
}

// @retail 0xa2f30
void c_weapon_put_away_event::v6(void *a, long b, long *size)
{
	*size = 0xd;
}

// @retail 0xa2850
const char *c_weapon_fire_event::v1()
{
	return "weapon-fire";
}

// @retail 0xa2860
long c_weapon_fire_event::v2()
{
	return 0x44;
}

// @retail 0xa2870
void c_weapon_fire_event::v6(void *a, long b, long *size)
{
	s_boot_event_data *data = ((s_boot_event_holder *)a)->data;
	*size = 0xf;
	if (data->field_c)
		*size = 0x3b;
	if (data->field_38)
		*size += 0x13;
}

// @retail 0xa3270
const char *c_weapon_pickup_event::v1()
{
	return "weapon-pickup";
}

// @retail 0xa3280
void c_weapon_pickup_event::v6(void *a, long b, long *size)
{
	*size = 0xe;
}

// @retail 0xa35a0
const char *c_weapon_effect_event::v1()
{
	return "weapon-effect";
}

// @retail 0xa35b0
void c_weapon_effect_event::v6(void *a, long b, long *size)
{
	*size = 0x25;
}

// @retail 0xa2f10
long c_weapon_drop_event::v0()
{
	return 0x12;
}

// @retail 0xa2f20
const char *c_weapon_drop_event::v1()
{
	return "weapon-drop";
}

// @retail 0xa2e40
const char *c_weapon_reload_event::v1()
{
	return "weapon-reload";
}

// ---- game engine events ----

// @retail 0xa5730
long c_game_engine_request_boot_player_event::v0()
{
	return 0x19;
}

// @retail 0xa5740
const char *c_game_engine_request_boot_player_event::v1()
{
	return "game-engine-request-boot-player-event";
}

// @retail 0xa5760
void c_game_engine_request_boot_player_event::v6(void *a, long b, long *size)
{
	*size = 8;
}

// @retail 0xa5770
real c_game_engine_request_boot_player_event::v7(long a, long b, long c)
{
	return 0.8f;
}

// @retail 0x9b910
long c_game_engine_event::v0()
{
	return 7;
}

// @retail 0xa50c0
const char *c_game_engine_event::v1()
{
	return "game-engine-event";
}

// @retail 0xa50d0
bool c_game_engine_event::v5(s_event_holder *a, s_event_mask *b)
{
	bool result = true;
	if (a->data->field8 != NONE)
		result = (b->mask & (1 << a->data->field8)) != 0;
	if (a->data->fieldc != NONE)
		result = result && (b->mask & (1 << a->data->fieldc));
	if (a->data->field14 != NONE)
		result = result && (b->mask & (1 << a->data->field14));
	return result;
}

// @retail 0xa5140
void c_game_engine_event::v6(void *a, long b, long *size)
{
	s_event_data *data = ((s_event_holder *)a)->data;
	long base;
	if (data->field1c)
		base = (data->field1c != 1) ? 0x21 : 1;
	else
		base = 0;
	long t18 = data->field18 == NONE ? 0 : 4;
	long t14 = data->field14 == NONE ? 0 : 4;
	long t10 = data->field10 == NONE ? 0 : 4;
	long t0c = data->fieldc == NONE ? 0 : 4;
	long t08 = data->field8 == NONE ? 0 : 4;
	long result = t08 + ((t0c + base) + (t18 + t14 + t10)) + 0x10;
	if (data->field4 == 9)
		result += 4;
	*size = result;
}
