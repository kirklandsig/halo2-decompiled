#include "unit_requests.h"
#include "effects.h"
// @flags /O2 /Gr
/* UNKNOWN_09B910.CPP: the item, projectile, weapon and device object types
   and the small projectile, weapon and game engine event definitions */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0a58d0.h"
#include "object_types_21_2.h"
#include <string.h>
#include <xmmintrin.h>

/* Retail keeps these definition objects in data. Their vtables retain all
   overrides even when a particular method has no indirect caller yet. */
c_item_type g_47980c;
c_projectile_type g_479818;
c_projectile_attached_event g_47981c;
c_projectile_effect_event g_479820;
c_projectile_impact_effect_event g_479824;
c_weapon_type g_47982c;
c_weapon_fire_event g_479830;
c_weapon_pickup_event g_479838;
c_weapon_put_away_event g_47983c;
c_weapon_drop_event g_479840;
c_weapon_effect_event g_479844;
c_device_type g_47984c;
c_game_engine_event g_479854;
c_game_engine_request_boot_player_event g_479858;

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

// @retail 0xa13d0
void c_projectile_attached_event::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "projectile attached: relevance=%5.3f", v7(a, b, c));
}

// @retail 0xa1630
void c_projectile_effect_event::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "projectile effect: relevance=%5.3f", v7(a, b, c));
}

// @retail 0xa1880
void c_projectile_impact_effect_event::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "projectile impact effect: relevance=%5.3f", v7(a, b, c));
}

// @retail 0xa1b60
void c_projectile_object_impact_effect_event::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "projectile object impact effect: relevance=%5.3f", v7(a, b, c));
}

// @retail 0xa28a0
void c_weapon_fire_event::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "weapon fire : relevance=%5.3f", v7(a, b, c));
}

// @retail 0xa2e50
void c_weapon_reload_event::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "weapon-reload: relevance=%5.3f", v7(a, b, c));
}

// @retail 0xa2f40
void c_weapon_drop_event::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "weapon-drop : relevance=%5.3f", v7(a, b, c));
}

// @retail 0xa3180
void c_weapon_put_away_event::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "weapon-put-away : relevance=%5.3f", v7(a, b, c));
}

// @retail 0xa3290
void c_weapon_pickup_event::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "weapon-pickup : relevance=%5.3f", v7(a, b, c));
}

// @retail 0xa35c0
void c_weapon_effect_event::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "weapon-effect : relevance=%5.3f", v7(a, b, c));
}

// @retail 0xa5780
void c_game_engine_request_boot_player_event::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "game engine request boot player");
}

// @retail 0xa51f0
void c_game_engine_event::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "game engine");
}

class c_device_touch_event : public c_event_definition
{
public:
	virtual bool v11(long a, long const *entities, long c, void const *data);
	virtual bool v10(long, void *, s_bitstream *) { return true; }
	virtual real v7(long a, long b, long c);
	virtual const char *v1();
	virtual void v8(long a, long b, long c, long size, char *buffer);
};

c_device_touch_event g_479850;

// @retail 0xa44a0
const char *c_device_touch_event::v1()
{
	return "device-touch";
}

// @retail 0xa4500
void c_device_touch_event::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "device-touch: relevance %5.3f", v7(a, b, c));
}

// @retail 0xa57a0
void c_game_engine_request_boot_player_event::v9(long a, void const *data, s_bitstream *stream)
{
	long const *values = (long const *)data;
	stream_write_checked(stream, values[0], 4);
	stream_write_checked(stream, values[1], 4);
}

// @retail 0xa5820
bool c_game_engine_request_boot_player_event::v10(long a, void *data, s_bitstream *stream)
{
	long *values = (long *)data;
	values[0] = function_1959c0(stream, 4);
	values[1] = function_1959c0(stream, 4);
	return true;
}

// @retail 0xa04b0
void c_item_type::v11(long a, long b, long c)
{
	long const volatile *flags_parameter = &b;
	long flags_address = *flags_parameter;
	((s_flags_a6900 const *)flags_address)->function_a6900(&b, this);
	long result = b + 1;
	if (*(dword const *)flags_address & 0x400)
		result = 12 < result ? 12 : result;
	*(long *)c = result;
}

// @retail 0xa4110
void c_device_type::v11(long a, long b, long c)
{
	dword flags = ((s_flags_a6900 const *)b)->flags;
	((s_flags_a6900 const *)b)->function_a6900(&b, this);
	long result = b + 2;
	if ((flags & 0x400) && result > 26)
		result = 26;
	if (flags & 0x800)
		result = 26 < result ? 26 : result;
	*(long *)c = result;
}

// @retail 0xa23f0
void c_weapon_type::v11(long a, long b, long c)
{
	dword flags = ((s_flags_a6900 const *)b)->flags;
	((s_flags_a6900 const *)b)->function_a6900(&b, this);
	long result = b + 1;
	if ((flags & 0x400) && result > 12)
		result = 12;
	result += 3;
	if ((flags & 0x800) && result > 18)
		result = 18;
	if ((flags & 0x1000) && result > 19)
		result = 19;
	if (flags & 0x2000)
		result = 40 < result ? 40 : result;
	*(long *)c = result;
}


void simulation_write_position(s_bitstream *stream, real const *position, long bits, bool keep_inside);
void simulation_read_position(s_bitstream *stream, real *position, long bits);
void scenario_object_name_encode(long object_name, s_bitstream *stream);

struct s_weapon_effect_payload
{
	dword kind;
	real position[3];
};

// @retail 0xa3600
void c_weapon_effect_event::v9(long a, void const *data, s_bitstream *stream)
{
	s_weapon_effect_payload const *event = (s_weapon_effect_payload const *)data;
	stream_write_checked(stream, event->kind, 1);
	simulation_write_position(stream, event->position, 12, true);
}

// @retail 0xa3660
bool c_weapon_effect_event::v10(long a, void *data, s_bitstream *stream)
{
	s_weapon_effect_payload *event = (s_weapon_effect_payload *)data;
	event->kind = function_1959c0(stream, 1);
	simulation_read_position(stream, event->position, 12);
	if ((event->kind > 1 ? 1 : event->kind) == event->kind)
		return true;
	return false;
}

struct s_weapon_pickup_payload
{
	short field_0;
	char field_2;
	char field_3;
	long field_4;
};

// @retail 0xa32d0
void c_weapon_pickup_event::v9(long a, void const *data, s_bitstream *stream)
{
	s_weapon_pickup_payload const *event = (s_weapon_pickup_payload const *)data;
	stream_write_checked(stream, event->field_0 + 1, 5);
	stream_write_checked(stream, event->field_2 + 1, 3);
	stream_write_checked(stream, event->field_3 + 1, 3);
	stream_write_checked(stream, event->field_4, 3);
}

// @retail 0xa33b0
bool c_weapon_pickup_event::v10(long a, void *data, s_bitstream *stream)
{
	s_weapon_pickup_payload *event = (s_weapon_pickup_payload *)data;
	event->field_0 = (short)(function_1959c0(stream, 5) - 1);
	event->field_2 = (char)(function_1959c0(stream, 3) - 1);
	event->field_3 = (char)(function_1959c0(stream, 3) - 1);
	event->field_4 = function_1959c0(stream, 3);
	if ((event->field_2 == NONE || (event->field_2 >= 0 && event->field_2 < 4)) &&
		(event->field_3 == NONE || (event->field_3 >= 0 && event->field_3 < 4)) &&
		event->field_4 >= 0 && event->field_4 < 8)
		return true;
	return false;
}

struct s_weapon_drop_payload
{
	short field_0;
	short field_2;
	long object_name;
};

struct s_z_event_scenario_view
{
	byte unknown000[0x3d8];
	long object_name_count;
	long *object_names;
};

PRIVATE __forceinline long z_event_read_object_name(s_bitstream *stream)
{
	long result = NONE;
	long index = function_1959c0(stream, 9) - 1;
	if (index != NONE)
	{
		s_z_event_scenario_view *scenario = (s_z_event_scenario_view *)g_4e0350;
		long name = NONE;
		if (scenario && scenario->object_name_count > 0)
			if ((index < 0 ? 0 : (index > scenario->object_name_count - 1 ? scenario->object_name_count - 1 : index)) == index)
				name = scenario->object_names[index];
		result = name;
	}
	return result;
}

// @retail 0xa2f80
void c_weapon_drop_event::v9(long a, void const *data, s_bitstream *stream)
{
	s_weapon_drop_payload const *event = (s_weapon_drop_payload const *)data;
	stream_write_checked(stream, event->field_0, 1);
	stream_write_checked(stream, event->field_2, 3);
	scenario_object_name_encode(event->object_name, stream);
}

// @retail 0xa3010
bool c_weapon_object_event::v10(long a, void *data, s_bitstream *stream)
{
	s_weapon_drop_payload *event = (s_weapon_drop_payload *)data;
	event->field_0 = (short)function_1959c0(stream, 1);
	event->field_2 = (short)function_1959c0(stream, 3);
	event->object_name = z_event_read_object_name(stream);
	if (event->field_0 >= 0 && event->field_0 < 2 && event->field_2 >= 0 && event->field_2 < 4)
		return true;
	return false;
}

struct s_projectile_attached_payload
{
	bool field_0;
	short field_2;
	real position[3];
	byte payload[8];
};

// @retail 0xa1410
void c_projectile_attached_event::v9(long a, void const *data, s_bitstream *stream)
{
	s_projectile_attached_payload const *event = (s_projectile_attached_payload const *)data;
	stream_write_bit(stream, event->field_0);
	if (event->field_0)
		stream_write_checked(stream, event->field_2, 8);
	simulation_write_position(stream, event->position, 13, false);
	function_1955d0(stream, event->payload, 64);
}

// @retail 0xa14d0
bool c_projectile_attached_event::v10(long a, void *data, s_bitstream *stream)
{
	s_projectile_attached_payload *event = (s_projectile_attached_payload *)data;
	event->field_0 = function_1957d0(stream);
	if (event->field_0)
		event->field_2 = (short)function_1959c0(stream, 8);
	else
		event->field_2 = NONE;
	simulation_read_position(stream, event->position, 13);
	function_195820(stream, event->payload, 64);
	return true;
}


// @retail 0xa44b0
real c_device_touch_event::v7(long a, long b, long c)
{
	long indices[2];
	long count = v3();
	if (count > 0)
		memcpy(indices, (byte *)a + 0x10, count * sizeof(long));
	return function_aa4d0(count, indices, *(real *)&c, (s_relevance_observers const *)b, 0);
}


struct s_z_projectile_effect_payload
{
 long object_name;
 real position[3];
 real direction[3];
 dword flags;
 real secondary_direction[3];
 word field_2c;
};
void function_194bc0(s_bitstream *stream, vector3f const *direction);

// @retail 0xa1670
void c_projectile_effect_event::v9(long a, void const *data, s_bitstream *stream)
{
 s_z_projectile_effect_payload const *event = (s_z_projectile_effect_payload const *)data;
 stream_write_checked(stream, event->flags, 4);
 scenario_object_name_encode(event->object_name, stream);
 simulation_write_position(stream, event->position, 12, true);
 function_194bc0(stream, (vector3f const *)event->direction);
 if (event->flags & 4)
 {
  function_1955d0(stream, &event->field_2c, 16);
  function_194bc0(stream, (vector3f const *)event->secondary_direction);
 }
}

// @retail 0xa5570
bool c_game_engine_event::v10(long a, void *data, s_bitstream *stream)
{
 long *values = (long *)data;
 values[0] = function_1959c0(stream, 4);
 values[1] = function_1959c0(stream, 6);
 values[2] = NONE;
 if (!function_1957d0(stream)) values[2] = function_1959c0(stream, 4);
 values[3] = NONE;
 if (!function_1957d0(stream)) values[3] = function_1959c0(stream, 4);
 values[4] = NONE;
 if (!function_1957d0(stream)) values[4] = function_1959c0(stream, 4);
 values[5] = NONE;
 if (!function_1957d0(stream)) values[5] = function_1959c0(stream, 4);
 values[6] = NONE;
 if (!function_1957d0(stream)) values[6] = function_1959c0(stream, 4);
 values[7] = 0;
 if (!function_1957d0(stream))
 {
  values[7] = 1;
  if (!function_1957d0(stream)) values[7] = function_1959c0(stream, 32);
 }
 if (values[0] == 9)
  *(short *)(values + 8) = (short)(function_1959c0(stream, 4) - 1);
 else
  *(short *)(values + 8) = 0;
 return true;
}

real __fastcall function_24f6b0(dword index, vector3f *direction);
extern short g_47d8e0;

// @retail 0xa1700
bool c_projectile_effect_event::v10(long a, void *data, s_bitstream *stream)
{
 s_z_projectile_effect_payload *event = (s_z_projectile_effect_payload *)data;
 event->flags = function_1959c0(stream, 4);
 event->object_name = z_event_read_object_name(stream);
 simulation_read_position(stream, event->position, 12);
 long packed_direction_1 = function_1959c0(stream, 17);
 function_24f6b0(packed_direction_1, (vector3f *)event->direction);
 if (event->flags & 4)
 {
  function_195820(stream, &event->field_2c, 16);
  long packed_direction_2 = function_1959c0(stream, 17);
 function_24f6b0(packed_direction_2, (vector3f *)event->secondary_direction);
 }
 else
 {
  event->field_2c = g_47d8e0;
  *(vector3f *)event->secondary_direction = *g_4687b0;
 }
 return true;
}

struct s_effect_owner;
__declspec(noinline) void *function_122c10(long group_tag, long tag_index);
void function_fcbc0(long definition_index, point3f const *point, vector3f const *normal,
 s_effect_owner const *owner, bool attached, bool airburst);
void function_fcdd0(long definition_index, point3f const *point, vector3f const *forward);
void function_fcea0(long effects_index, point3f const *point, vector3f const *direction,
 s_effect_owner const *owner, long index, vector3f const *normal);

// score 20 (was 20): only the 0xfcea0 call differs: retail passes the word field as a 16-bit argument
// (`xor edx, edx; mov dx, [ebp + 0x2c]`, then its own argument order), ours zero-extends it with movzx for
// 0xfcea0's long index. 0xfcea0's index as short matches this, but 0xfcea0 then passes it on to 0xfd740's
// long element index (4 -> 133); with 0xfd740's as short too 0xfcea0 stays 4 and 0xfd0e0 goes 115 -> 148
// (0xfd740 then extends it for 0x1883e0, whose matching parameter retail also passes unextended):
// BLOCKED-BY 0xfd740, 0x1883e0. Earlier: 0xfcbc0 with its normal and definition parameters swapped
// (since kept, 22 -> 20); position and direction pointer locals for the three calls 50
// Round 103: the payload's word field declared short 20 (`movsx` for `movzx`); 0xfcea0's index as short
// (definition, declarations and both calls) matches this function, but 0xfcea0 goes 4 -> 133 (579 bytes: its
// `movsx` for 0xfd740's long index pads its loop head) and 0xfc330 527 -> 533; the same with word 133; with
// the payload field short too 133; with 0xfcea0 widening the index into a long local at entry 133.
// @retail 0xa17d0
bool c_projectile_effect_event::v11(long a, long const *entities, long c, void const *data)
{
 s_z_projectile_effect_payload const *event = (s_z_projectile_effect_payload const *)data;
 if (event->flags && event->object_name != NONE && function_122c10(0x70726f6a, event->object_name))
 {
  if (event->flags & 3)
   function_fcbc0(event->object_name, (point3f const *)event->position,
    (vector3f const *)event->direction, 0, (event->flags & 2) != 0, false);
  if (event->flags & 8)
   function_fcdd0(event->object_name, (point3f const *)event->position, (vector3f const *)event->direction);
  if (event->flags & 4)
   function_fcea0(event->object_name, (point3f const *)event->position, (vector3f const *)event->direction,
    0, event->field_2c, (vector3f const *)event->secondary_direction);
 }
 return true;
}

void game_engine_boot_player(long player_index);

/* the datum index of the player at an absolute index; false when the index
   is out of range or that player slot is free */
static inline bool event_absolute_player_to_datum_index(long index, long *player_index)
{
 if (index == NONE || index < 0 || index >= g_4e8c24->high_water_index)
  return false;
 byte *player = g_4e8c24->data + g_4e8c24->size * index;
 if (!*(short *)player)
  return false;
 *player_index = (*(short *)player << 16) | index;
 return true;
}

/* the datum at an absolute index, or null when the index is out of range or
   the datum is free (the same test as datum_get_absolute in unknown_16b570.cpp,
   which retail inlines here) */
static inline byte *event_used_datum_at(s_record_pool *data, long index)
{
 byte *result = 0;
 if (index != NONE && index >= 0 && index < data->high_water_index)
 {
  byte *datum = data->data + data->size * index;
  if (*(short *)datum != 0)
   result = datum;
 }
 return result;
}

// @retail 0xa5850
bool c_game_engine_request_boot_player_event::v11(long a, long const *entities, long c, void const *data)
{
 long const *values = (long const *)data;
 bool result = false;
 long player_index;
 if (event_absolute_player_to_datum_index(values[0], &player_index) &&
  event_used_datum_at(g_4e8c24, values[1]))
 {
  game_engine_boot_player(player_index);
  result = true;
 }
 return result;
}

// @retail 0xa5210
void c_game_engine_event::v9(long a, void const *data, s_bitstream *stream)
{
 long const *values = (long const *)data;
 stream_write_checked(stream, values[0], 4);
 stream_write_checked(stream, values[1], 6);
 stream_write_bit(stream, values[2] == NONE);
 if (values[2] != NONE) stream_write_checked(stream, values[2], 4);
 stream_write_bit(stream, values[3] == NONE);
 if (values[3] != NONE) stream_write_checked(stream, values[3], 4);
 stream_write_bit(stream, values[4] == NONE);
 if (values[4] != NONE) stream_write_checked(stream, values[4], 4);
 stream_write_bit(stream, values[5] == NONE);
 if (values[5] != NONE) stream_write_checked(stream, values[5], 4);
 stream_write_bit(stream, values[6] == NONE);
 if (values[6] != NONE) stream_write_checked(stream, values[6], 4);
 stream_write_bit(stream, values[7] == 0);
 if (values[7] != 0)
 {
  stream_write_bit(stream, values[7] == 1);
  if (values[7] != 1) function_195720(stream, values[7], 32);
 }
 if (values[0] == 9)
  stream_write_checked(stream, *(short const *)(values + 8) + 1, 4);
}

#include "game_engine_events.h"

PRIVATE __forceinline bool z_event_restore_player(long *identifier, long index)
{
 if (index != NONE)
 {
  if (index < 0 || index >= g_4e8c24->high_water_index)
   return false;
  if (!*(short *)(g_4e8c24->data + g_4e8c24->size * index))
   return false;
  *identifier = data_datum_index(g_4e8c24, index);
 }
 return true;
}

// @retail 0xa5650
bool c_game_engine_event::v11(long a, long const *entities, long c, void const *data)
{
 struct s_player_restore
 {
  __forceinline bool restore(long *identifier, long index)
  {
   if (index != NONE)
   {
    if (index < 0 || index >= g_4e8c24->high_water_index) return false;
    byte *player = g_4e8c24->data + g_4e8c24->size * index;
    if (!*(short *)player) return false;
    *identifier = (*(short *)player << 16) | index;
   }
   return true;
  }
 } restore;
 bool result = true;
 s_event const *source = (s_event const *)data;
 s_event event = *source;
 if (restore.restore(&event.a, source->a) &&
  restore.restore(&event.cause_player_index, source->cause_player_index) &&
  restore.restore(&event.effect_player_index, source->effect_player_index))
  function_19eb30(&event);
 else
  result = false;
 return result;
}


real function_aa7c0(s_relevance_observers const *observers, point3f const *position, real maximum_distance);

// @retail 0xa1600
real c_projectile_effect_event::v7(long a, long b, long c)
{
 byte const *payload = *(byte const **)(a + 0x18);
 return function_aa7c0((s_relevance_observers const *)b, (point3f const *)(payload + 4), *(real *)&c);
}

// @retail 0xa1b30
real c_projectile_object_impact_effect_event::v7(long a, long b, long c)
{
 byte const *payload = *(byte const **)(a + 0x18);
 return function_aa7c0((s_relevance_observers const *)b, (point3f const *)(payload + 0x18), *(real *)&c);
}


struct s_z_blend_state
{
 real first;
 real second;
 long identifier;
 long slot;
};
struct s_z_fire_payload
{
 long slot;
 long object_name;
 long mode;
 bool relative;
 short node;
 real position[3];
 vector3f direction;
 s_z_blend_state blend;
 bool attached;
 long identifier;
 long attachment;
};
void simulation_write_relative_position(s_bitstream *stream, long bits, real const *position);
void simulation_read_relative_position(s_bitstream *stream, long bits, real *position);
void function_ab5e0(s_z_blend_state const *state, s_bitstream *stream);
bool function_ab700(s_bitstream *stream, s_z_blend_state *state);
void simulation_write_entity_identifier(long identifier, s_bitstream *stream);

// @retail 0xa28e0
void c_weapon_fire_event::v9(long a, void const *data, s_bitstream *stream)
{
 s_z_fire_payload const *event = (s_z_fire_payload const *)data;
 stream_write_checked(stream, event->slot + 1, 3);
 scenario_object_name_encode(event->object_name, stream);
 stream_write_checked(stream, event->mode, 1);
 stream_write_bit(stream, event->relative);
 if (event->relative)
 {
  stream_write_checked(stream, event->node, 8);
  simulation_write_relative_position(stream, 12, event->position);
 }
 function_ab5e0(&event->blend, stream);
 function_194bc0(stream, &event->direction);
 stream_write_bit(stream, event->attached);
 if (event->attached)
 {
  stream_write_checked(stream, event->attachment, 5);
  simulation_write_entity_identifier(event->identifier, stream);
 }
}

// @retail 0xa2a80
bool c_weapon_fire_event::v10(long a, void *data, s_bitstream *stream)
{
 bool result = true;
 s_z_fire_payload *event = (s_z_fire_payload *)data;
 event->slot = function_1959c0(stream, 3) - 1;
 if (event->slot != NONE) result = event->slot >= 0 && event->slot < 4;
 event->object_name = z_event_read_object_name(stream);
 event->mode = function_1959c0(stream, 1);
 result = result && event->mode >= 0 && event->mode < 2;
 event->relative = function_1957d0(stream);
 if (event->relative)
 {
  event->node = (short)function_1959c0(stream, 8);
  simulation_read_relative_position(stream, 12, event->position);
 }
 bool blend_valid = function_ab700(stream, &event->blend);
 result = result && blend_valid;
 long direction = function_1959c0(stream, 17);
 function_24f6b0(direction, &event->direction);
 event->attached = function_1957d0(stream);
 if (event->attached)
 {
  event->attachment = function_1959c0(stream, 5);
  long index = function_1959c0(stream, 10);
  byte salt = (byte)function_1959c0(stream, 4);
  event->identifier = index | ((dword)salt << 28);
 }
 return result;
}




bool function_e68c0(long type, long unit_index);
void function_a8dd0(long unit_index);
void function_107840(long device_index, long unit_index);
bool __stdcall function_cd4e0(long unit_index, short hand, bool flag);
__declspec(noinline) void *function_122c10(long group_tag, long tag_index);
void function_fd0e0(long definition_index, real scale_a, real scale_b, vector3f const *direction,
    point3f const *point, vector3f const *normal, word index, bool attached, long object_index,
    long node_index, bool alternate);
void function_fd560(long projectile_index, long object_index, short node_index,
    point3f const *point, vector3f const *forward);

struct s_z_event_header
{
    word identifier;
    byte flags, type;
    dword unknown04;
    byte *object;
};

struct s_z_event_unit_bits { byte unknown000[0x10a]; byte flags; };

// @retail 0xa2e90
bool c_weapon_reload_event::v11(long a, long const *entities, long c, void const *data)
{
    struct s_unit_flags
    {
        byte unknown000[0x10a];
        word unknown0 : 2;
        word suppressed : 1;
        word unknown3 : 13;
    };
    bool result = false;
    long index = function_a58d0(entities[0]);
    if (index != NONE)
    {
        s_z_event_header *header = (s_z_event_header *)g_4e0300->data + (index & 0xffff);
        if (((1 << header->type) & 3) && !((bool)((s_unit_flags *)header->object)->suppressed))
        {
            function_e68c0(0, index);
            function_e68c0(10, index);
            function_a8dd0(index);
            result = true;
        }
    }
    return result;
}

// @retail 0xa4540
bool c_device_touch_event::v11(long a, long const *entities, long c, void const *data)
{
    struct s_unit_flags
    {
        byte unknown000[0x10a];
        word unknown0 : 2;
        word suppressed : 1;
        word unknown3 : 13;
    };
    long device = function_a58d0(entities[0]);
    long unit = function_a58d0(entities[1]);
    if (device != NONE && unit != NONE)
    {
        s_record_pool *pool = g_4e0300;
        s_z_event_header *headers = (s_z_event_header *)pool->data;
        if ((1 << headers[device & 0xffff].type) & 0x380)
        {
            if ((1 << ((s_z_event_header *)pool->data)[unit & 0xffff].type) & 3)
            {
                byte *unit_object = ((s_z_event_header *)g_4e0300->data)[unit & 0xffff].object;
                if (!((bool)((s_unit_flags *)unit_object)->suppressed))
                    function_107840(device, unit);
            }
        }
    }
    return false;
}

// @retail 0xa1520
bool c_projectile_attached_event::v11(long a, long const *entities, long c, void const *data)
{
    bool result = false;
    long projectile = function_a58d0(entities[0]);
    long target = function_a58d0(entities[1]);
    if (projectile != NONE)
    {
        s_z_event_header *header = (s_z_event_header *)g_4e0300->data + (projectile & 0xffff);
        if (header->type == 5 && *(long *)(header->object + 0x14) == NONE)
        {
            byte const *event = (byte const *)data;
            vector3f const *forward = (vector3f const *)(event + 0x10);
            if (*(short const *)(event + 0x14) == NONE && *(long const *)(event + 0x10) == NONE) forward = 0;
            if ((event[0] && *(short const *)(event + 2) != NONE) || (!event[0] && *(short const *)(event + 2) == NONE))
            {
                function_fd560(projectile, target, *(short const *)(event + 2), (point3f const *)(event + 4), forward);
                result = true;
            }
        }
    }
    return result;
}

// @retail 0xa31c0
bool c_weapon_put_away_event::v11(long a, long const *entities, long c, void const *data)
{
    struct s_unit_flags
    {
        byte unknown000[0x10a];
        word unknown0 : 2;
        word suppressed : 1;
        word unknown3 : 13;
    };
    long unit = function_a58d0(entities[0]);
    if (unit != NONE)
    {
        s_z_event_header *headers = (s_z_event_header *)g_4e0300->data;
        s_z_event_header *header = headers + (unit & 0xffff);
        if (((1 << header->type) & 3) && !((bool)((s_unit_flags *)header->object)->suppressed))
        {
            byte const *event = (byte const *)data;
            short hand = *(short const *)event, slot = *(short const *)(event + 2);
            if (slot == ((signed char *)header->object)[0x212 + hand])
            {
                long weapon = *(long *)(header->object + 0x218 + slot * 4);
                if (weapon != NONE && *(long *)(event + 4) == *(long *)headers[weapon & 0xffff].object)
                    function_cd4e0(unit, hand, false);
            }
        }
    }
    return false;
}

// @retail 0xa30b0
bool c_weapon_drop_event::v11(long a, long const *entities, long c, void const *data)
{
    struct s_unit_flags
    {
        byte unknown000[0x10a];
        word unknown0 : 2;
        word suppressed : 1;
        word unknown3 : 13;
    };
    long unit = function_a58d0(entities[0]);
    if (unit != NONE)
    {
        s_z_event_header *headers = (s_z_event_header *)g_4e0300->data;
        s_z_event_header *header = headers + (unit & 0xffff);
        if (((1 << header->type) & 3) && !((bool)((s_unit_flags *)header->object)->suppressed))
        {
            byte const *event = (byte const *)data;
            short hand = *(short const *)event, slot = *(short const *)(event + 2);
            if (slot == ((signed char *)header->object)[0x212 + hand])
            {
                long weapon = *(long *)(header->object + 0x218 + slot * 4);
                if (weapon != NONE && *(long *)(event + 4) == *(long *)headers[weapon & 0xffff].object)
                    function_e68c0(hand ? 19 : 9, unit);
            }
        }
    }
    return false;
}



bool __stdcall function_cd7b0(long unit_index, long weapon_index, bool *modes);
void function_d0930(long unit_index, vector3f *direction);
void __stdcall function_102fb0(long weapon_index, short barrel);
void function_103e60(long weapon_index, short barrel);
void __stdcall function_fd980(long weapon_index);
transform4x3f *function_b8c00(long object_index, long *count);
point3f *transform4x3f_apply_point(transform4x3f const *matrix, point3f const *point, point3f *result);
long __stdcall effect_new_from_parameters(s_effect_parameters *parameters);
extern vector3f *g_4687b0;
extern vector3f *g_4687bc;
struct s_object_relevance_source
{
    long object_index, identifier;
    byte unknown08[0x1c - 8];
    real first, second;
};
struct s_object_relevance_result
{
    real first, second;
    long object_index, identifier;
};
void function_82b30(s_object_relevance_result const *source, s_object_relevance_source *result);

// @retail 0xa3420
bool c_weapon_pickup_event::v11(long a, long const *entities, long c, void const *data)
{
    struct s_unit_flags
    {
        byte unknown000[0x10a];
        word unknown0 : 2;
        word suppressed : 1;
        word unknown3 : 13;
    };
    bool result = false;
    long unit = function_a58d0(entities[0]);
    long weapon = function_a58d0(entities[1]);
    if (unit != NONE && weapon != NONE)
    {
        s_z_event_header *headers = (s_z_event_header *)g_4e0300->data;
        if (((1 << headers[unit & 0xffff].type) & 3) && ((1 << headers[weapon & 0xffff].type) & 4))
        {
            byte *unit_object = ((s_z_event_header *)g_4e0300->data)[unit & 0xffff].object;
            byte *weapon_object = ((s_z_event_header *)g_4e0300->data)[weapon & 0xffff].object;
            if (!((bool)((s_unit_flags *)unit_object)->suppressed) && *(long *)(weapon_object + 0x14c) != unit &&
                !(weapon_object[0x12c] & 1))
            {
                // the event's data, read from the parameter's stack slot once the unit and weapon pass
                long const *event = (long const *)*(void const *const volatile *)&data;
                if (*(long *)(unit_object + 0x210) == event[0])
                {
                    bool modes[4];
                    if (function_cd7b0(unit, weapon, modes))
                    {
                        bool allowed = false;
                        switch (event[1])
                        {
                        case 3: allowed = modes[0]; break;
                        case 4: allowed = modes[2]; break;
                        case 5: allowed = modes[1]; break;
                        case 6: allowed = modes[3]; break;
                        }
                        if (allowed)
                        {
                            s_unit_request request;
                            memset(&request, 0, sizeof(request));
                            request.type = 0x14;
                            *(long *)request.arguments = weapon;
                            *(short *)(request.arguments + 4) = (short)event[1];
                            if (function_e6900(unit, &request)) result = true;
                        }
                    }
                }
            }
        }
    }
    return result;
}
