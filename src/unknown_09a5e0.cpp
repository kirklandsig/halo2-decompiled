// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_09A5E0.CPP: the unit object type, the "game-engine-player" and
   "breakable-surface-group" entity definitions, and the simulation event
   definitions of vtables 0x4514e8, 0x4517a8, 0x4519f8 and 0x451ae0 */

#include "unknown_11c920.h"
#include "globals.h"
#include "engine_peer.h"
#include "object_type_definitions.h"
#include "object_types_21_1.h"
#include "entity_relevance.h"
#include "flags_writer.h"
#include "unit_requests.h"
#include <math.h>
#include <string.h>

#define OBJECT_HEADER(index) (&((s_object_header *)g_4e0300->data)[(index) & 0xFFFF])
#define OBJECT(index) (OBJECT_HEADER(index)->object)
#define UNIT_OBJECT(index) ((s_unit_object_view *)OBJECT(index))

/* the unit object, as this code sees it */
struct s_unit_object_view
{
	long definition_index;
	byte unknown04[0x16];
	short field1a;
	byte unknown1c[8];
	long field24;
	byte unknown28[0xaf - 0x28];
	byte field_af;
	byte unknownb0[0x138 - 0xb0];
	short field138;
	byte unknown13a[2];
	long field13c;
	byte unknown140[0x2a0 - 0x140];
	long field2a0;
	short field2a4;
	byte unknown2a6[2];
	long field2a8;
	long field2ac;
};

/* the real a quantized value stands for */
static inline real event_dequantize_real(long value, long maximum, real minimum_value, real maximum_value)
{
	if (value == 0)
		return minimum_value;
	if (value >= maximum)
		return maximum_value;
	return (minimum_value * (maximum - value) + maximum_value * value) / maximum;
}

/* the identifier in a slot, or NONE when there is no manager */
static long slot_identifier(short index)
{
	long id = NONE;

	if (g_55e4d0[g_4e9ae8->engine_index])
		id = g_4e9ae8->slots[index];
	return id;
}

/* the identifier in a slot, or NONE when there is no manager */
static long slot_of(c_engine_peer *manager, short index)
{
	long id = NONE;

	if (manager)
		id = g_4e9ae8->slots[index];
	return id;
}

static bool slot_matches(c_engine_peer *manager, short index, s_entity_slot *entity)
{
	bool result = false;

	if (manager)
	{
		long id = g_4e9ae8->slots[index];

		if (id != NONE && ((entity->id ^ id) & 0x3ff) == 0)
			result = true;
	}
	return result;
}

static void long4_clear(s_long4 *value)
{
	value->a = 0;
	value->b = 0;
	value->c = 0;
	value->d = 0;
}

static void unit_state_clear(s_unit_entity_state *state)
{
	state->field0 = 0;
	state->field4 = 0;
	*(long *)&state->field8 = 0;
	state->fieldc = 0;
	long4_clear(&state->field10);
	*(long *)&state->field20 = 0;
}

// ---- the unit object type ----

// @retail 0x9d200
long c_unit_type::v0()
{
	return 9;
}

// @retail 0x9d210
const char *c_unit_type::v1()
{
	return "unit";
}

// @retail 0x9d220
long c_unit_type::v2()
{
	return 0xf8;
}

// @retail 0x9d230
long c_unit_type::v3()
{
	return 0x24;
}

// @retail 0x9b940
long c_unit_type::v4()
{
	return 0x18;
}

// @retail 0x9d240
long c_unit_type::v5()
{
	return 0x5dcc3f;
}

// @retail 0x9dcf0
void c_unit_type::v9(long a, long b, long *size)
{
	*size = 0x91;
}

// @retail 0x9dd00
void c_unit_type::v10(s_creation_request *request, long parameter, long size, char *buffer)
{
	real relevance = -1.0f;
	s_creation_weight *entry = &g_4cef68[request->definition_index];
	if (!(entry->weight > g_45dbd8))
		relevance = function_aa4d0(1, &request->entity_index, entry->maximum_distance, (s_relevance_observers const *)parameter, 0);
	function_11c9c0(buffer, size, "unit creation: relevance=%5.3f", relevance);
}

// @retail 0x9df10
void c_unit_type::v11(long a, long b, long c)
{
	s_flags_a6900 const *flags = (s_flags_a6900 const *)b;
	dword update_flags = flags->flags;
	long result;
	flags->function_a6900(&result, this);
	result += 0xe;
	if ((update_flags & 0x400) && result > 0x22)
		result = 0x22;
	if ((update_flags & 0x800) && result > 0x2c)
		result = 0x2c;
	if ((update_flags & 0x1000) && result > 0x29)
		result = 0x29;
	if ((update_flags & 0x2000) && result > 0x23)
		result = 0x23;
	if ((update_flags & 0x3c000) && result > 0x27)
		result = 0x27;
	if ((update_flags & 0x3c0000) && result > 0x32)
		result = 0x32;
	if ((update_flags & 0x16) && result > 0x28)
		result = 0x28;
	if (update_flags & 0x17)
	{
		if (result > 0x1f)
			result = 0x1f;
	}
	*(long *)c = result;
}

// @retail 0x9dea0
void c_unit_type::v21(s_entity *entity)
{
	s_unit_entity *unit = (s_unit_entity *)entity;
	s_unit_entity_data *data = unit->data;

	if (unit->object_index != NONE)
		*(long *)((byte *)OBJECT(unit->object_index) + 0xd4) = unit->field0;
	if (data->identifier != NONE)
	{
		long salt = (unsigned long)data->identifier >> 28;
		salt = (salt + 1) % 16;
		data->identifier = ((byte)salt << 28) | (data->identifier & 0x3ff);
	}
}

// @retail 0x9d280
void c_unit_type::v26(long index, long b, s_entity_state *state)
{
	s_unit_entity_state *unit_state = (s_unit_entity_state *)state;
	s_unit_object_view *first = UNIT_OBJECT(index);
	s_unit_object_view *object;

	unit_state_clear(unit_state);

	object = UNIT_OBJECT(index);
	unit_state->field4 = object->definition_index;
	unit_state->field0 = object->field1a;
	unit_state->field8 = object->field_af;
	unit_state->fieldc = object->field24;

	long4_clear(&unit_state->field10);
	if (first->field13c != NONE)
	{
		s_long4 *element = (s_long4 *)(g_4e8c24->data + (first->field13c & 0xffff) * 0x21c + 0x84);

		unit_state->field10 = *element;
	}
	unit_state->field20 = first->field138;
}

// @retail 0x9d250
bool c_unit_type::v28(long index)
{
	return !TEST_FIELD_BIT(OBJECT(index)->flag2);
}

// @retail 0x9dc90
bool c_unit_type::v32(long index)
{
	s_unit_object_view *object = UNIT_OBJECT(index);

	object->field2a0 = NONE;
	object->field2a4 = -1;
	object->field2a8 = NONE;
	object->field2ac = NONE;
	return true;
}

// ---- the "game-engine-player" entity definition ----

// @retail 0x9ac70
long c_game_engine_player_entity_definition::v0()
{
	return 6;
}

// @retail 0x9ac80
const char *c_game_engine_player_entity_definition::v1()
{
	return "game-engine-player";
}

// @retail 0x9beb0
long c_game_engine_player_entity_definition::v2()
{
	return 0x38;
}

// @retail 0x9bec0
long c_game_engine_player_entity_definition::v3()
{
	return 2;
}

// @retail 0x9ac90
long c_game_engine_player_entity_definition::v4()
{
	return 0xb;
}

// @retail 0x9edd0
void c_game_engine_player_entity_definition::v9(long a, long b, long *size)
{
	*size = 5;
}

// @retail 0x9aca0
void c_game_engine_player_entity_definition::v10(s_creation_request *request, long parameter, long size, char *buffer)
{
	real relevance = -1.0f;
	s_creation_weight *entry = &g_4cef68[request->definition_index];
	if (!(entry->weight > g_45dbd8))
		relevance = function_aa4d0(1, &request->entity_index, entry->maximum_distance, (s_relevance_observers const *)parameter, 0);
	function_11c9c0(buffer, size, "player creation: relevance=%5.3f", relevance);
}

// @retail 0x9ad20
void c_game_engine_player_entity_definition::v26(long a, dword *flags, long size, char *buffer)
{
	real relevance = 0.0f;
	long period = 0;
	function_abac0(&relevance, (s_creation_request const *)a, (s_update_state const *)flags, &period);
	function_11c9c0(buffer, size, "player update: relevance=%5.3f: period=%d", relevance, period);
}

// @retail 0x9ad10
void c_game_engine_player_entity_definition::v11(long a, long b, long *size)
{
	*size = 0xb;
}

// @retail 0x9b8e0
bool c_game_engine_player_entity_definition::v16(s_float_holder *a, s_float_holder *b, long c)
{
	bool result = false;

	if (fabs(b->value - a->value) < 3.0518044e-5f)
		result = true;
	b->value = 0.0f;
	a->value = 0.0f;
	return result;
}

// @retail 0x9ad80
void c_game_engine_player_entity_definition::v18(s_entity_slot *entity, long b, short *slot)
{
	long index;

	for (index = 0; index < 16; index++)
	{
		if (slot_identifier(index) == entity->id)
			break;
	}
	*slot = (short)index;
}

// @retail 0x9afb0
bool c_game_engine_player_entity_definition::v19(long a, short *b, long c, long d)
{
	g_55e4d0[g_4e9ae8->engine_index]->p47(*b, c, d);
	return true;
}

// @retail 0x9aff0
bool c_game_engine_player_entity_definition::v20(s_entity_slot *entity, long b, long c, long d)
{
	bool result = false;
	long id = entity->id;
	c_engine_peer *manager = g_55e4d0[g_4e9ae8->engine_index];
	long index;

	for (index = 0; index < 16; index++)
	{
		if (slot_of(manager, index) == id)
			break;
	}
	if (index != 16)
	{
		manager->p48(index, b, c, d);
		result = true;
	}
	return result;
}

// @retail 0x9b0a0
void c_game_engine_player_entity_definition::v21(s_entity_slot *entity)
{
	c_engine_peer *manager = g_55e4d0[g_4e9ae8->engine_index];
	long index;

	for (index = 0; index < 16; index++)
	{
		long id = slot_of(manager, index);

		if (id != NONE && ((entity->id ^ id) & 0x3ff) == 0)
			break;
	}
	if (index != 16 && manager)
	{
		long *slot = &g_4e9ae8->slots[(short)index];

		if (*slot != NONE)
		{
			long id = entity->id;

			if (((id ^ *slot) & 0x3ff) == 0)
				*slot = id;
		}
	}
}

// @retail 0x9aeb0
bool c_game_engine_player_entity_definition::v22(s_entity_slot *entity, long b, short *slot, long d, long e, long f)
{
	short index = *slot;
	c_engine_peer *manager = g_55e4d0[g_4e9ae8->engine_index];

	if (manager)
	{
		if (g_4e9ae8->slots[index] != NONE)
			g_4e9ae8->slots[index] = NONE;
	}
	g_4e9ae8->slots[index] = entity->id;
	return true;
}

// @retail 0x9b180
bool c_game_engine_player_entity_definition::v23(s_entity_slot *entity, long b, long c, long d)
{
	bool result = false;
	c_engine_peer *manager = g_55e4d0[g_4e9ae8->engine_index];
	long id = entity->id;
	long index;

	for (index = 0; index < 16; index++)
	{
		if (slot_of(manager, index) == id)
			break;
	}
	if (index >= 0 && index < 16)
	{
		if (manager->p49(index, b, c, d))
			result = true;
	}
	return result;
}

// @retail 0x9af00
bool c_game_engine_player_entity_definition::v24(s_entity_slot *entity)
{
	bool result = false;
	c_engine_peer *manager = g_55e4d0[g_4e9ae8->engine_index];
	long index;

	for (index = 0; index < 16; index++)
	{
		if (slot_of(manager, index) == entity->id)
			break;
	}
	if (index != 16 && entity->id == slot_of(manager, index))
	{
		g_4e9ae8->slots[(short)index] = NONE;
		result = true;
	}
	return result;
}

// ---- the "breakable-surface-group" entity definition ----

// @retail 0x9cd70
long c_breakable_surface_group_entity_definition::v0()
{
	return 8;
}

// @retail 0x9cd80
const char *c_breakable_surface_group_entity_definition::v1()
{
	return "breakable-surface-group";
}

// @retail 0x9cd90
long c_breakable_surface_group_entity_definition::v2()
{
	return 4;
}

// @retail 0x9cda0
void c_breakable_surface_group_entity_definition::v9(long a, long b, long *size)
{
	*size = 0x10;
}

// @retail 0x9cdb0
void c_breakable_surface_group_entity_definition::v10(s_creation_request *request, long parameter, long size, char *buffer)
{
	real relevance = -1.0f;
	s_creation_weight *entry = &g_4cef68[request->definition_index];
	if (!(entry->weight > g_45dbd8))
		relevance = function_aa4d0(1, &request->entity_index, entry->maximum_distance, (s_relevance_observers const *)parameter, 0);
	function_11c9c0(buffer, size, "breakable surface group creation: relevance=%5.3f", relevance);
}

// @retail 0x9ce30
void c_breakable_surface_group_entity_definition::v26(long a, dword *flags, long size, char *buffer)
{
	real relevance = 0.0f;
	long period = 0;
	function_abac0(&relevance, (s_creation_request const *)a, (s_update_state const *)flags, &period);
	function_11c9c0(buffer, size, "breakable surface group update:relevance=%5.3f: period=%d", relevance, period);
}

// @retail 0x9ce20
void c_breakable_surface_group_entity_definition::v11(long a, long b, long *size)
{
	*size = 0x21;
}

// @retail 0x9cfd0
void c_breakable_surface_group_entity_definition::v18(s_entity_slot *entity, long b, short *slot)
{
	*slot = (short)entity->slot;
}

// @retail 0x9cff0
bool c_breakable_surface_group_entity_definition::v19(long a, long b, long c, long *d)
{
	*d = 0;
	return true;
}

// @retail 0x9d0a0
void c_breakable_surface_group_entity_definition::v21(s_entity_slot *entity)
{
	long slot = entity->slot;

	if (slot >= 0 && slot < 8)
	{
		long id = g_4eca60[slot];

		if (id != NONE && ((entity->id ^ id) & 0x3ff) == 0)
			g_4eca60[slot] = entity->id;
	}
}

// @retail 0x9d0e0
bool c_breakable_surface_group_entity_definition::v22(s_entity_slot *entity, long b, short *slot, long d, long e, long f)
{
	entity->slot = *slot;
	if (d)
		v23(entity, d, e, f);
	return true;
}

// @retail 0x9d1b0
bool c_breakable_surface_group_entity_definition::v24(s_entity_slot *entity)
{
	entity->slot = NONE;
	return true;
}

// @retail 0x9d1c0
bool c_breakable_surface_group_entity_definition::v25(s_entity_slot *entity)
{
	long slot = entity->slot;

	if (slot >= 0 && slot < 8)
	{
		if (g_4eca60[slot] != NONE)
		{
			g_4eca60[slot] = NONE;
		}
		long id = entity->id;
		g_4eca60[slot] = id;
	}
	return true;
}

// ---- the event definitions ----

// @retail 0x9f9e0
const char *c_unit_melee_initiate_event_definition::v1()
{
	return "unit-melee-initiate";
}

// @retail 0x9f9f0
void c_unit_melee_initiate_event_definition::v6(void *a, long b, long *size)
{
	*size = 2;
}

// @retail 0x9fe20
const char *c_unit_pickup_event_definition::v1()
{
	return "unit-pickup";
}

// @retail 0x9fe40
void c_unit_pickup_event_definition::v6(void *a, long b, long *size)
{
	long result = 12;

	if (*((s_event_section_data *)a)->kind == 1)
		result = 0x1c;
	*size = result;
}

// @retail 0x9a5e0
long c_unit_grenade_release_event_definition::v0()
{
	return 0x16;
}

// @retail 0x9f330
const char *c_unit_grenade_release_event_definition::v1()
{
	return "unit-grenade-release";
}

// @retail 0x9f340
long c_unit_grenade_release_event_definition::v2()
{
	return 0x1c;
}

// @retail 0x9f350
void c_unit_grenade_release_event_definition::v6(void *a, long b, long *size)
{
	*size = 0x42;
}

// @retail 0x9fcf0
const char *c_vehicle_trick_event_definition::v1()
{
	return "vehicle-trick";
}

// @retail 0x9b950
void c_vehicle_trick_event_definition::v6(void *a, long b, long *size)
{
	*size = 0;
}

// @retail 0x9fb90
long c_vehicle_flip_event_definition::v0()
{
	return 0x13;
}

// @retail 0x9fba0
const char *c_vehicle_flip_event_definition::v1()
{
	return "vehicle-flip";
}

// @retail 0x9f180
long c_unit_grenade_initiate_event_definition::v0()
{
	return 0xe;
}

// @retail 0x9f190
const char *c_unit_grenade_initiate_event_definition::v1()
{
	return "unit-grenade-initiate";
}

// @retail 0x9f1a0
void c_unit_grenade_initiate_event_definition::v6(void *a, long b, long *size)
{
	*size = 1;
}

// @retail 0x9eef0
const char *c_unit_board_vehicle_event_definition::v1()
{
	return "unit-board-vehicle";
}

// @retail 0x9ef00
void c_unit_board_vehicle_event_definition::v6(void *a, long b, long *size)
{
	*size = 0x20;
}

// @retail 0x9edc0
const char *c_unit_exit_vehicle_event_definition::v1()
{
	return "unit-exit-vehicle";
}

/* the unit object, as the vehicle events see it */
struct s_vehicle_event_unit_view
{
	byte unknown000[0x14];
	long parent_index;
	byte unknown018[0x1fc - 0x18];
	short seat_index;
};

/* an object header with its object type */
struct s_typed_object_header
{
	byte unknown00[3];
	byte type;
	byte unknown04[4];
	void *object;
};

/* 0xe68c0, src/unknown_0e68c0.cpp */
bool function_e68c0(long type, long unit_index);

// @retail 0x9ee20
bool c_unit_exit_vehicle_event_definition::v11(long a, long const *entities, long c, void const *data)
{
	bool result = false;
	long unit_index = function_a58d0(entities[0]);
	long vehicle_index = function_a58d0(entities[1]);
	if (unit_index != NONE && vehicle_index != NONE)
	{
		s_typed_object_header *headers = (s_typed_object_header *)g_4e0300->data;
		if (((1 << headers[unit_index & 0xffff].type) & 3) && ((1 << headers[vehicle_index & 0xffff].type) & 3))
		{
			s_vehicle_event_unit_view *unit = (s_vehicle_event_unit_view *)headers[unit_index & 0xffff].object;
			if (unit->parent_index != NONE && unit->parent_index == vehicle_index &&
				unit->seat_index != NONE && unit->seat_index == *(long const *)data)
			{
				function_e68c0(0x1d, unit_index);
				result = true;
			}
		}
	}
	return result;
}

static inline long pin(long value, long lo, long hi)
{
	long result;
	if (value < lo)
		result = lo;
	else
	{
		result = hi;
		if (value <= hi)
			result = value;
	}
	return result;
}

/* 0xf47d0, src/unknown_0f47d0.cpp */
bool function_f47d0(long unit_index, long trick);

// @retail 0x9fdb0
bool c_vehicle_trick_event_definition::v11(long a, long const *entities, long c, void const *data)
{
	bool result = false;
	long unit_index = function_a58d0(entities[0]);
	if (unit_index != NONE && ((1 << ((s_typed_object_header *)g_4e0300->data)[unit_index & 0xffff].type) & 2))
	{
		long trick = *(long const *)data;
		if (pin(trick, 0, 3) == trick)
			result = function_f47d0(unit_index, trick);
	}
	return result;
}

/* the data of the unit melee initiate and grenade initiate events */
struct s_unit_action_event_data
{
	short type;
};

/* the unit (or vehicle) object, as the unit action events see it */
struct s_event_unit_view
{
	s_object_view object;
	byte unknown10c[0x1fc - 0x10c];
	short seat_index;
	byte unknown1fe[0x23c - 0x1fe];
	byte grenade_type;
	byte current_grenade_type;
};

/* a unit definition's seat (0xb0 bytes) */
struct s_event_unit_seat
{
	union
	{
		dword flags;
		struct
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
			dword boardable : 1;
		};
	};
	byte unknown04[0x3e - 4];
	short boarding_seat;
	byte unknown40[0xb0 - 0x40];
};

/* a unit definition, as the vehicle events see it */
struct s_event_unit_definition
{
	byte unknown000[0x1c8];
	long seat_count;
	s_event_unit_seat *seats;
};

#define TYPED_OBJECT_HEADER(index) ((s_typed_object_header *)(((index) & 0xffff) * sizeof(s_typed_object_header) + g_4e0300->data))
#define EVENT_UNIT_DEFINITION(object) ((s_event_unit_definition *)g_4e3b44[(object)->definition_index & 0xffff].bytes)

/* 0xe6fe0 (src/unknown_0e6fe0.cpp): whether a unit is performing an action */
bool unit_action_active(long unit_index, long action_type);
long function_c8f60(long unit_index, short seat_index);
/* 0xc92c0: whether a unit can enter a vehicle's seat */
bool __stdcall function_c92c0(long unit_index, long vehicle_index, short seat_index, long *a, bool *b);

/* the request to enter a vehicle's seat (type 0x1c) */
struct s_unit_enter_seat_request
{
	long type;
	long vehicle_index;
	short seat_index;
	bool unknowna;
	bool unknownb;
	byte unknown0c[0x20 - 0xc];
};

/* the request to throw a grenade (type 0x16) */
struct s_unit_grenade_request
{
	long type;
	bool initiate;
	bool release;
	byte unknown06[2];
	vector3f position;
	vector3f velocity;
};

/* the request to melee (type 0x1b) */
struct s_unit_melee_request
{
	long type;
	short melee_type;
	bool unknown6;
	byte unknown7;
	long target_index;
	byte unknown0c[0x20 - 0xc];
};

/* the request to flip a vehicle (type 0x21) */
struct s_unit_flip_request
{
	long type;
	long unit_index;
	byte unknown08[0x20 - 8];
};

/* the data of a grenade release event */
struct s_unit_grenade_release_event_data
{
	short type;
	vector3f position;
	vector3f velocity;
};

/* sets the grenade type a unit throws */
static inline void unit_set_grenade_type(long unit_index, short grenade_type)
{
	s_event_unit_view *unit = (s_event_unit_view *)OBJECT(unit_index);

	unit->current_grenade_type = (byte)grenade_type;
	unit->grenade_type = (byte)grenade_type;
}

// @retail 0x9ec50
bool c_unit_enter_vehicle_event_definition::v11(long a, long const *entities, long c, void const *data)
{
	bool result = false;
	long unit_index = function_a58d0(entities[0]);
	long vehicle_index = function_a58d0(entities[1]);
	if (unit_index != NONE && vehicle_index != NONE)
	{
		s_typed_object_header *unit_header = TYPED_OBJECT_HEADER(unit_index);
		if (((1 << unit_header->type) & 3) && ((1 << TYPED_OBJECT_HEADER(vehicle_index)->type) & 3))
		{
			s_event_unit_view *unit = (s_event_unit_view *)OBJECT(unit_index);
			s_event_unit_view *vehicle = (s_event_unit_view *)OBJECT(vehicle_index);
			if (!TEST_FIELD_BIT(unit->object.flag2) && unit->object.field14 == NONE && !TEST_FIELD_BIT(vehicle->object.flag2))
			{
				long const *seat_index = (long const *)data;
				if (*seat_index >= 0 && *seat_index < EVENT_UNIT_DEFINITION(&vehicle->object)->seat_count)
				{
					long unknown;
					bool unknown_flag;
					if (function_c92c0(unit_index, vehicle_index, (short)*seat_index, &unknown, &unknown_flag))
					{
						s_unit_enter_seat_request request;
						memset(&request, 0, sizeof(request));
						request.type = 0x1c;
						request.vehicle_index = vehicle_index;
						request.seat_index = (short)*seat_index;
						result = function_e6900(unit_index, (s_unit_request *)&request);
					}
				}
			}
		}
	}
	return result;
}

// @retail 0x9efa0
bool c_unit_board_vehicle_event_definition::v11(long a, long const *entities, long c, void const *data)
{
	bool result = false;
	long unit_index = function_a58d0(entities[0]);
	if (unit_index != NONE)
	{
		s_event_unit_view *unit = (s_event_unit_view *)OBJECT(unit_index);
		long vehicle_index = function_a58d0(entities[1]);
		if (!TEST_FIELD_BIT(unit->object.flag2) && vehicle_index != NONE)
		{
			s_event_unit_view *vehicle = (s_event_unit_view *)OBJECT(vehicle_index);
			if ((1 << vehicle->object.type) & 2)
			{
				long const *seat_index = (long const *)data;
				if (*seat_index != NONE && !unit_action_active(unit_index, 0x1f))
				{
					s_event_unit_definition *definition = EVENT_UNIT_DEFINITION(&vehicle->object);
					if (*seat_index < definition->seat_count)
					{
						s_event_unit_seat *seat = &definition->seats[*seat_index];
						if (TEST_FIELD_BIT(seat->boardable) && seat->boarding_seat != NONE)
						{
							bool enter = false;
							if (unit->object.field14 == NONE)
								enter = true;
							else if (unit->object.field14 != vehicle_index || unit->seat_index != *seat_index)
							{
								function_e68c0(0x1e, unit_index);
								enter = true;
							}
							if (!TEST_FIELD_BIT(vehicle->object.flag2) && enter)
							{
								long occupant = function_c8f60(vehicle_index, (short)*seat_index);
								if (occupant != NONE && occupant != unit_index)
									function_e68c0(0x1e, occupant);
								s_unit_enter_seat_request request = { 0 };
								request.type = 0x1c;
								request.vehicle_index = vehicle_index;
								request.seat_index = (short)*seat_index;
								request.unknowna = false;
								request.unknownb = false;
								function_e6900(unit_index, (s_unit_request *)&request);
							}
							if (unit->object.field14 == vehicle_index && unit->seat_index == *seat_index)
							{
								function_e68c0(0x1f, unit_index);
								result = true;
							}
						}
					}
				}
			}
		}
	}
	return result;
}

// @retail 0x9f270
bool c_unit_grenade_initiate_event_definition::v11(long a, long const *entities, long c, void const *data)
{
	bool result = false;
	long unit_index = function_a58d0(entities[0]);
	if (unit_index == NONE)
	{
	}
	else
	{
		s_event_unit_view *unit = (s_event_unit_view *)OBJECT(unit_index);
		if (unit->object.type != 0)
		{
		}
		else if (TEST_FIELD_BIT(unit->object.flag2))
		{
		}
		else
		{
			s_unit_grenade_request request;
			unit_set_grenade_type(unit_index, ((s_unit_action_event_data const *)data)->type);
			memset(&request, 0, sizeof(request));
			request.type = 0x16;
			request.initiate = true;
			function_e6900(unit_index, (s_unit_request *)&request);
			result = true;
		}
	}
	return result;
}

// @retail 0x9f460
bool c_unit_grenade_release_event_definition::v11(long a, long const *entities, long c, void const *data)
{
	bool result = false;
	long unit_index = function_a58d0(entities[0]);
	if (unit_index == NONE)
	{
	}
	else
	{
		s_event_unit_view *unit = (s_event_unit_view *)OBJECT(unit_index);
		if (unit->object.type != 0)
		{
		}
		else if (TEST_FIELD_BIT(unit->object.flag2))
		{
		}
		else
		{
			s_unit_grenade_release_event_data const *event = (s_unit_grenade_release_event_data const *)data;
			s_unit_grenade_request request;
			unit_set_grenade_type(unit_index, event->type);
			memset(&request, 0, sizeof(request));
			request.type = 0x16;
			request.release = true;
			request.position = event->position;
			request.velocity = event->velocity;
			function_e6900(unit_index, (s_unit_request *)&request);
			result = true;
		}
	}
	return result;
}

// @retail 0x9fac0
bool c_unit_melee_initiate_event_definition::v11(long a, long const *entities, long c, void const *data)
{
	long unit_index = function_a58d0(entities[0]);
	long target_index = function_a58d0(entities[1]);
	if (unit_index != NONE)
	{
		s_event_unit_view *unit = (s_event_unit_view *)OBJECT(unit_index);
		if (target_index != NONE && !((1 << ((s_typed_object_header *)g_4e0300->data)[target_index & 0xffff].type) & 3))
			target_index = NONE;
		if (unit->object.type == 0 && !TEST_FIELD_BIT(unit->object.flag2))
		{
			s_unit_melee_request request;
			memset(&request, 0, sizeof(request));
			request.type = 0x1b;
			request.melee_type = ((s_unit_action_event_data const *)data)->type;
			request.unknown6 = true;
			request.target_index = target_index;
			function_e6900(unit_index, (s_unit_request *)&request);
		}
	}
	return false;
}

// @retail 0x9fbf0
bool c_vehicle_flip_event_definition::v11(long a, long const *entities, long c, void const *data)
{
	bool result = false;
	long vehicle_index = function_a58d0(entities[0]);
	long unit_index = function_a58d0(entities[1]);
	if (vehicle_index != NONE && unit_index != NONE)
	{
		s_typed_object_header *headers = (s_typed_object_header *)g_4e0300->data;
		if ((1 << headers[vehicle_index & 0xffff].type) & 2)
		{
			if (!((1 << headers[unit_index & 0xffff].type) & 3))
			{
			}
			else if (TEST_FIELD_BIT(((s_object_view *)OBJECT(unit_index))->flag2))
			{
			}
			else if (TEST_FIELD_BIT(((s_object_view *)OBJECT(vehicle_index))->flag2))
			{
			}
			else
			{
				s_unit_flip_request request;
				request.type = 0x21;
				request.unit_index = unit_index;
				result = function_e6900(vehicle_index, (s_unit_request *)&request);
			}
		}
	}
	return result;
}

// @retail 0x9f550
long c_unit_melee_damage_event_definition::v0()
{
	return 0x17;
}

// @retail 0x9f560
const char *c_unit_melee_damage_event_definition::v1()
{
	return "unit-melee-damage";
}

// @retail 0x9f570
void c_unit_melee_damage_event_definition::v6(void *a, long b, long *size)
{
	*size = 0x2a;
}

// @retail 0x9ebe0
const char *c_unit_enter_vehicle_event_definition::v1()
{
	return "unit-enter-vehicle";
}

// @retail 0x9ca40
const char *c_breakable_surface_damage_event_definition::v1()
{
	return "breakable-surface-damage";
}

// @retail 0x9ca50
long c_breakable_surface_damage_event_definition::v2()
{
	return 0x34;
}

// @retail 0x9ca60
void c_breakable_surface_damage_event_definition::v6(void *a, long b, long *size)
{
	*size = 0xa0;
}

// @retail 0xa51e0
real c_breakable_surface_damage_event_definition::v7(long a, long b, long c)
{
	return g_45dbd8;
}

// @retail 0x9c7d0
const char *c_damage_section_response_event_definition::v1()
{
	return "damage-section-response";
}

// @retail 0x9c7e0
void c_damage_section_response_event_definition::v6(void *a, long b, long *size)
{
	*size = 9;
}

// @retail 0x9bea0
const char *c_damage_aftermath_event_definition::v1()
{
	return "damage-aftermath";
}

// @retail 0x9bee0
void c_damage_aftermath_event_definition::v6(void *a, long b, long *size)
{
	*size = 0x49;
}

// ---- the event descriptions, encodings and decodings ----

/* the data of the vehicle trick and the vehicle boarding events */
struct s_long_event_data
{
	long value;
};

/* the data of a damage section response event */
struct s_damage_section_response_event_data
{
	long section_index;
	long response_index;
	long kind;
};

// @retail 0x9fa00
void c_unit_melee_initiate_event_definition::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "unit-melee-initiate: relevance=%5.3f", v7(a, b, c));
}

// @retail 0x9fa40
void c_unit_melee_initiate_event_definition::v9(long a, void const *data, s_bitstream *stream)
{
	stream_write_checked(stream, ((s_unit_action_event_data const *)data)->type, 2);
}

// @retail 0x9fa90
bool c_unit_melee_initiate_event_definition::v10(long a, void *data, s_bitstream *stream)
{
	s_unit_action_event_data *event = (s_unit_action_event_data *)data;
	event->type = (short)function_1959c0(stream, 2);
	if (event->type > 0 && event->type < 3)
		return true;
	return false;
}

// @retail 0x9fe70
void c_unit_pickup_event_definition::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "unit-pickup : relevance=%5.3f", v7(a, b, c));
}

// @retail 0x9f360
void c_unit_grenade_release_event_definition::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "unit-grenade-release: relevance=%5.3f", v7(a, b, c));
}

// @retail 0x9fd00
void c_vehicle_trick_event_definition::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "vehicle-trick: relevance=%5.3f", v7(a, b, c));
}

// @retail 0x9fd40
void c_vehicle_trick_event_definition::v9(long a, void const *data, s_bitstream *stream)
{
	stream_write_checked(stream, ((s_long_event_data const *)data)->value, 2);
}

// @retail 0x9fd90
bool c_vehicle_trick_event_definition::v10(long a, void *data, s_bitstream *stream)
{
	((s_long_event_data *)data)->value = function_1959c0(stream, 2);
	return true;
}

// @retail 0x9fbb0
void c_vehicle_flip_event_definition::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "vehicle-flip relevance=%5.3f", v7(a, b, c));
}

// @retail 0x9f1b0
void c_unit_grenade_initiate_event_definition::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "unit-grenade-initiate: relevance=%5.3f", v7(a, b, c));
}

// @retail 0x9f1f0
void c_unit_grenade_initiate_event_definition::v9(long a, void const *data, s_bitstream *stream)
{
	stream_write_checked(stream, ((s_unit_action_event_data const *)data)->type, 1);
}

// @retail 0x9f240
bool c_unit_grenade_initiate_event_definition::v10(long a, void *data, s_bitstream *stream)
{
	s_unit_action_event_data *event = (s_unit_action_event_data *)data;
	event->type = (short)function_1959c0(stream, 1);
	if (event->type >= 0 && event->type < 2)
		return true;
	return false;
}

// @retail 0x9ef10
void c_unit_board_vehicle_event_definition::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "unit board vehicle: relevance=%5.3f", v7(a, b, c));
}

// @retail 0x9ef50
void c_unit_board_vehicle_event_definition::v9(long a, void const *data, s_bitstream *stream)
{
	stream_write_checked(stream, ((s_long_event_data const *)data)->value, 5);
}

// @retail 0x9ec30
bool c_unit_board_vehicle_event_definition::v10(long a, void *data, s_bitstream *stream)
{
	((s_long_event_data *)data)->value = function_1959c0(stream, 5);
	return true;
}

// @retail 0x9ede0
void c_unit_exit_vehicle_event_definition::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "unit exit vehicle: relevance=%5.3f", v7(a, b, c));
}

// @retail 0x9f580
void c_unit_melee_damage_event_definition::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "unit-melee-damage: relevance=%5.3f", v7(a, b, c));
}

// @retail 0x9ebf0
void c_unit_enter_vehicle_event_definition::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "unit enter vehicle: relevance=%5.3f", v7(a, b, c));
}

// @retail 0x9ca70
void c_breakable_surface_damage_event_definition::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "damage section response");
}

// @retail 0x9c7f0
void c_damage_section_response_event_definition::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "damage section response: relevance=%5.3f", v7(a, b, c));
}

// @retail 0x9c830
void c_damage_section_response_event_definition::v9(long a, void const *data, s_bitstream *stream)
{
	s_damage_section_response_event_data const *event = (s_damage_section_response_event_data const *)data;
	stream_write_checked(stream, event->section_index, 4);
	stream_write_checked(stream, event->response_index, 4);
	stream_write_checked(stream, event->kind, 1);
}

// @retail 0x9c8e0
bool c_damage_section_response_event_definition::v10(long a, void *data, s_bitstream *stream)
{
	s_damage_section_response_event_data *event = (s_damage_section_response_event_data *)data;
	event->section_index = function_1959c0(stream, 4);
	event->response_index = function_1959c0(stream, 4);
	event->kind = function_1959c0(stream, 1);
	return true;
}

// ---- the encodings of the "game-engine-player" and "breakable-surface-group" entities ----

/* the pairs of structure bsp and bit that map to the breakable surface slots
   (src/unknown_183ee0.cpp) */
struct s_slot_pair
{
	long a;
	long b;
};

extern s_slot_pair g_4eca80[0x100];
extern byte *g_4ed280;
byte *function_183fc0(long index);
long function_184000(long bit, long index);

// @retail 0x9ae30
void c_game_engine_player_entity_definition::v12(long a, void const *data, long c, s_bitstream *stream)
{
	stream_write_checked(stream, *(short const *)data, 5);
}

// @retail 0x9ae80
bool c_game_engine_player_entity_definition::v13(long a, void *data, s_bitstream *stream)
{
	short *index = (short *)data;
	*index = (short)function_1959c0(stream, 5);
	if (*index >= 0 && *index < 16)
		return true;
	return false;
}

/* a game engine player's update (src/unknown_09a5e0.cpp decodes it) */
struct s_game_engine_player_update
{
	byte team;
	byte unknown01[3];
	long unknown04;
	byte unknown08[12];
	real unknown14;
	bool unknown18;
	byte unknown19;
	short unknown1a;
	long unknown1c;
	short unknown20;
	bool unknown22;
	bool unknown23;
	bool unknown24;
	byte unknown25;
	struct
	{
		bool valid;
		byte unknown01;
		short a[3];
		short b[3];
		short c;
	} unknown26;
};

void function_194830(s_bitstream *stream, bool value);

// @retail 0x9b240
bool c_game_engine_player_entity_definition::v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8)
{
	s_game_engine_player_update const *update = (s_game_engine_player_update const *)a5;
	s_bitstream *stream = (s_bitstream *)a7;
	bool result = false;
	s_flags_writer writer;
	flags_writer_initialize(&writer, stream, 0, 0xb, a2, a8);
	if (writer.space)
	{
		if (flags_writer_begin(&writer, 0, "respawn-timer-exists"))
		{
			stream_write_checked(stream, update->unknown04, 16);
			function_1955d0(stream, update->unknown08, 96);
		}
		flags_writer_end(&writer);
		if (flags_writer_begin(&writer, 1, "speed-multiplier-exists"))
		{
			real scaled = update->unknown14 * 32767.5f;
			long quantized;
			__asm
			{
				fld scaled
				fistp quantized
			}
			function_195720(stream, quantized, 16);
		}
		flags_writer_end(&writer);
		if (flags_writer_begin(&writer, 2, "waypoint-action-exists"))
			stream_write_checked(stream, (char)update->team, 3);
		flags_writer_end(&writer);
		if (flags_writer_begin(&writer, 3, "blocking-teleporter-exists"))
			function_194830(stream, update->unknown18);
		flags_writer_end(&writer);
		if (flags_writer_begin(&writer, 4, "netdebug-exists"))
		{
			function_194830(stream, update->unknown26.valid);
			if (update->unknown26.valid)
			{
				stream_write_checked(stream, (word)update->unknown26.a[0], 16);
				stream_write_checked(stream, (word)update->unknown26.a[1], 16);
				stream_write_checked(stream, (word)update->unknown26.a[2], 16);
			}
			stream_write_checked(stream, (word)update->unknown26.b[0], 16);
			stream_write_checked(stream, (word)update->unknown26.b[1], 16);
			stream_write_checked(stream, (word)update->unknown26.b[2], 16);
			stream_write_checked(stream, (word)update->unknown26.c, 7);
		}
		flags_writer_end(&writer);
		if (flags_writer_begin(&writer, 5, "lives-remaining-exists"))
			stream_write_checked(stream, update->unknown1a + 1, 7);
		flags_writer_end(&writer);
		if (flags_writer_begin(&writer, 6, "last-betrayer-exists"))
			stream_write_checked(stream, update->unknown1c + 1, 5);
		flags_writer_end(&writer);
		if (flags_writer_begin(&writer, 7, "respawn-timer-exists"))
			stream_write_checked(stream, (word)update->unknown20, 10);
		flags_writer_end(&writer);
		if (flags_writer_begin(&writer, 8, "vehicle-entrance-ban-exists"))
			function_194830(stream, update->unknown22);
		flags_writer_end(&writer);
		if (flags_writer_begin(&writer, 9, "active-in-game-exists"))
			function_194830(stream, update->unknown23);
		flags_writer_end(&writer);
		if (flags_writer_begin(&writer, 10, "sitting-out-exists"))
			function_194830(stream, update->unknown24);
		flags_writer_end(&writer);
		*(dword *)a3 |= writer.written;
		result = true;
	}
	return result;
}

// @retail 0x9b710
bool c_game_engine_player_entity_definition::v15(long a, dword *flags, long c, void *data, s_bitstream *stream)
{
	s_game_engine_player_update *update = (s_game_engine_player_update *)data;
	dword update_flags = 0;
	if (function_1957d0(stream))
	{
		update->unknown04 = function_1959c0(stream, 16);
		function_195820(stream, update->unknown08, 96);
		update_flags = 1;
	}
	if (function_1957d0(stream))
	{
		update->unknown14 = event_dequantize_real(function_1959c0(stream, 16), 0xffff, 0.0f, 2.0f);
		update_flags |= 2;
	}
	if (function_1957d0(stream))
	{
		update->team = (byte)function_1959c0(stream, 3);
		update_flags |= 4;
	}
	if (function_1957d0(stream))
	{
		update->unknown18 = function_1957d0(stream);
		update_flags |= 8;
	}
	if (function_1957d0(stream))
	{
		memset(&update->unknown26, 0, sizeof(update->unknown26));
		update->unknown26.valid = function_1957d0(stream);
		if (update->unknown26.valid)
		{
			update->unknown26.a[0] = (short)function_1959c0(stream, 16);
			update->unknown26.a[1] = (short)function_1959c0(stream, 16);
			update->unknown26.a[2] = (short)function_1959c0(stream, 16);
		}
		update->unknown26.b[0] = (short)function_1959c0(stream, 16);
		update->unknown26.b[1] = (short)function_1959c0(stream, 16);
		update->unknown26.b[2] = (short)function_1959c0(stream, 16);
		update->unknown26.c = (short)function_1959c0(stream, 7);
		update_flags |= 0x10;
	}
	if (function_1957d0(stream))
	{
		update->unknown1a = (short)(function_1959c0(stream, 7) - 1);
		update_flags |= 0x20;
	}
	if (function_1957d0(stream))
	{
		update->unknown1c = function_1959c0(stream, 5) - 1;
		update_flags |= 0x40;
	}
	if (function_1957d0(stream))
	{
		update->unknown20 = (short)function_1959c0(stream, 10);
		update_flags |= 0x80;
	}
	if (function_1957d0(stream))
	{
		update->unknown22 = function_1957d0(stream);
		update_flags |= 0x100;
	}
	if (function_1957d0(stream))
	{
		update->unknown23 = function_1957d0(stream);
		update_flags |= 0x200;
	}
	if (function_1957d0(stream))
	{
		update->unknown24 = function_1957d0(stream);
		update_flags |= 0x400;
	}
	*flags = update_flags;
	return (bool)update_flags;
}

// @retail 0x9ce90
void c_breakable_surface_group_entity_definition::v12(long a, void const *data, long c, s_bitstream *stream)
{
	function_1955d0(stream, data, 16);
}

// @retail 0x9ceb0
bool c_breakable_surface_group_entity_definition::v13(long a, void *data, s_bitstream *stream)
{
	function_195820(stream, data, 16);
	if (stream->bit_position <= stream->size_in_bytes * 8 && *(short *)data != -1)
		return true;
	return false;
}

// @retail 0x9cef0
bool c_breakable_surface_group_entity_definition::v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8)
{
	s_bitstream *stream = (s_bitstream *)a7;
	s_flags_writer writer;
	flags_writer_initialize(&writer, stream, 0, 1, a2, a8);
	bool result = false;
	if (writer.space)
	{
		if (flags_writer_begin(&writer, 0, "surface-group-update-exists"))
			function_1955d0(stream, (void const *)a5, 32);
		flags_writer_end(&writer);
		*(dword *)a3 |= writer.written;
		result = true;
	}
	return result;
}

// @retail 0x9cfa0
bool c_breakable_surface_group_entity_definition::v15(long a, dword *flags, long c, void *data, s_bitstream *stream)
{
	if (function_1957d0(stream))
	{
		function_195820(stream, data, 32);
		*flags |= 1;
	}
	return true;
}

// @retail 0x9d000
bool c_breakable_surface_group_entity_definition::v20(s_entity_slot *entity, dword *flags, long c, dword *mask)
{
	if (*flags & 1)
	{
		dword old_mask = *mask;
		long index = entity->slot * 32;
		s_slot_pair *pair = &g_4eca80[index];
		for (long i = 0; i < 32; i++, pair++, index++)
		{
			if (index != NONE && index < 0x100)
			{
				if (pair->a != NONE || pair->b != pair->a)
				{
					if (!function_184000(pair->b, pair->a))
						*mask |= 1 << i;
					else
						*mask &= ~(1 << i);
				}
			}
		}
		if (old_mask != *mask)
			*flags |= 1;
	}
	return true;
}

// @retail 0x9d110
bool c_breakable_surface_group_entity_definition::v23(s_entity_slot *entity, long b, long c, long d)
{
	dword *mask = (dword *)d;
	for (long i = 0; i < 32; i++)
	{
		if (*mask & (1 << i))
		{
			long index = entity->slot * 32 + i;
			if (index != NONE && index < 0x100)
			{
				long bsp = g_4eca80[index].a;
				long bit = g_4eca80[index].b;
				if (bsp != NONE || bit != bsp)
				{
					if (function_184000(bit, bsp) && *g_4ed280)
					{
						dword *bits = (dword *)function_183fc0(bsp);
						bits[bit >> 5] &= ~(1 << (bit & 31));
					}
				}
			}
		}
	}
	return true;
}

/* the scenario's block of object names the events refer to by index */
struct s_event_scenario_view
{
	byte unknown000[0x3d8];
	long object_name_count;
	long *object_names;
};

/* reads the index of an object name, and returns that name or NONE */
__forceinline long event_read_scenario_object_name(s_bitstream *stream)
{
	long result = NONE;
	long index = function_1959c0(stream, 9) - 1;
	if (index != NONE)
	{
		s_event_scenario_view *scenario = (s_event_scenario_view *)g_4e0350;
		result = NONE;
		if (scenario && scenario->object_name_count > 0)
		{
			if ((index < 0 ? 0 : (index > scenario->object_name_count - 1 ? scenario->object_name_count - 1 : index)) == index)
				result = scenario->object_names[index];
		}
	}
	return result;
}

struct s_unit_pickup_event_data
{
	short type;
	long object_name;
	byte extra[2];
};

struct s_unit_melee_damage_event_data
{
	long object_name;
	long damage_type;
	long material;
	long response;
	byte location[2];
	real scale;
	byte region;
};

void scenario_object_name_encode(long object_name, s_bitstream *stream);

// @retail 0x9feb0
void c_unit_pickup_event_definition::v9(long a, void const *data, s_bitstream *stream)
{
	s_unit_pickup_event_data const *event = (s_unit_pickup_event_data const *)data;
	stream_write_checked(stream, event->type, 3);
	scenario_object_name_encode(event->object_name, stream);
	if (event->type == 1)
		function_1955d0(stream, event->extra, 16);
}

// @retail 0x9f5c0
void c_unit_melee_damage_event_definition::v9(long a, void const *data, s_bitstream *stream)
{
	s_unit_melee_damage_event_data const *event = (s_unit_melee_damage_event_data const *)data;
	scenario_object_name_encode(event->object_name, stream);
	stream_write_bit(stream, event->damage_type != NONE);
	if (event->damage_type != NONE)
	{
		stream_write_checked(stream, event->damage_type, 8);
		stream_write_bit(stream, event->material != NONE);
		if (event->material != NONE)
			stream_write_checked(stream, event->material, 10);
		stream_write_checked(stream, event->response, 17);
	}
	function_1955d0(stream, event->location, 16);
	real scale = event->scale * 255.0f;
	long quantized;
	__asm
	{
		fld scale
		fistp quantized
	}
	function_195720(stream, quantized, 8);
	stream_write_checked(stream, event->region, 8);
}

void __fastcall function_24f6b0(dword index, vector3f *direction);
void function_194bc0(vector3f const *direction, s_bitstream *stream);

static inline void event_write_direction(s_bitstream *stream, vector3f const *direction)
{
	function_194bc0(direction, stream);
}

#define SET_FLAG(flags, bit, value) ((value) ? ((flags) |= (1 << (bit))) : ((flags) &= ~(1 << (bit))))

/* the data of a damage aftermath event */
struct s_damage_aftermath_event_data
{
	long object_name;
	long unknown04;
	short damage_type;
	bool has_direction;
	byte unknown0b;
	vector3f direction;
	real unknown18;
	real unknown1c;
	union
	{
		dword flags;
		struct
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
		};
	};
	real unknown24;
	real unknown28;
	short unknown2c;
	short unknown2e;
	long unknown30;
	byte unknown34;
};

/* rounds as the x87 does (unknown_0259d0's fld/fistp idiom) */
#define EVENT_QUANTIZE(result, value) __asm { fld value } __asm { fistp result }

// @retail 0x9bf30
void c_damage_aftermath_event_definition::v9(long a, void const *data, s_bitstream *stream)
{
	s_damage_aftermath_event_data const *event = (s_damage_aftermath_event_data const *)data;
	scenario_object_name_encode(event->object_name, stream);
	stream_write_checked(stream, event->damage_type + 1, 5);
	stream_write_bit(stream, event->has_direction);
	if (event->has_direction)
		event_write_direction(stream, &event->direction);
	{
		long quantized;
		real scaled = event->unknown18 * 15.5f;
		EVENT_QUANTIZE(quantized, scaled);
		function_195720(stream, quantized, 5);
	}
	{
		real scaled = event->unknown1c * 15.5f;
		long quantized;
		EVENT_QUANTIZE(quantized, scaled);
		function_195720(stream, quantized, 5);
	}
	stream_write_bit(stream, TEST_FIELD_BIT(event->flag1));
	stream_write_bit(stream, TEST_FIELD_BIT(event->flag2));
	stream_write_bit(stream, TEST_FIELD_BIT(event->flag3));
	stream_write_bit(stream, TEST_FIELD_BIT(event->flag4));
	stream_write_bit(stream, TEST_FIELD_BIT(event->flag5));
	stream_write_bit(stream, TEST_FIELD_BIT(event->flag6));
	stream_write_bit(stream, TEST_FIELD_BIT(event->flag9));
	stream_write_bit(stream, TEST_FIELD_BIT(event->flag7));
	{
		real scaled = event->unknown28 * (127.0f / 9.0f);
		long quantized;
		EVENT_QUANTIZE(quantized, scaled);
		function_195720(stream, quantized, 7);
	}
	{
		real scaled = event->unknown24 * 21.0f;
		long quantized;
		EVENT_QUANTIZE(quantized, scaled);
		function_195720(stream, quantized, 6);
	}
	stream_write_checked(stream, event->unknown2c + 1, 4);
	stream_write_checked(stream, event->unknown2e + 1, 8);
	stream_write_checked(stream, event->unknown30, 3);
	stream_write_checked(stream, event->unknown34, 8);
}

// @retail 0x9c350
bool c_damage_aftermath_event_definition::v10(long a, void *data, s_bitstream *stream)
{
	s_damage_aftermath_event_data *event = (s_damage_aftermath_event_data *)data;
	event->object_name = event_read_scenario_object_name(stream);
	event->damage_type = (short)(function_1959c0(stream, 5) - 1);
	event->unknown04 = 0;
	event->has_direction = function_1957d0(stream);
	if (event->has_direction)
	{
		dword direction = function_1959c0(stream, 17);
		function_24f6b0(direction, &event->direction);
	}
	else
		event->direction = *g_4687a4;
	event->unknown18 = event_dequantize_real(function_1959c0(stream, 5), 31, 0.0f, 2.0f);
	event->unknown1c = event_dequantize_real(function_1959c0(stream, 5), 31, 0.0f, 2.0f);
	event->flags = 0;
	SET_FLAG(event->flags, 1, function_1957d0(stream));
	SET_FLAG(event->flags, 2, function_1957d0(stream));
	SET_FLAG(event->flags, 3, function_1957d0(stream));
	SET_FLAG(event->flags, 4, function_1957d0(stream));
	SET_FLAG(event->flags, 5, function_1957d0(stream));
	SET_FLAG(event->flags, 6, function_1957d0(stream));
	SET_FLAG(event->flags, 9, function_1957d0(stream));
	SET_FLAG(event->flags, 7, function_1957d0(stream));
	event->unknown28 = event_dequantize_real(function_1959c0(stream, 7), 127, 0.0f, 9.0f);
	event->unknown24 = event_dequantize_real(function_1959c0(stream, 6), 63, 0.0f, 3.0f);
	event->unknown2c = (short)(function_1959c0(stream, 4) - 1);
	event->unknown2e = (short)(function_1959c0(stream, 8) - 1);
	event->unknown30 = function_1959c0(stream, 3);
	event->unknown34 = (byte)function_1959c0(stream, 8);
	return true;
}

// @retail 0x9ff20
bool c_unit_pickup_event_definition::v10(long a, void *data, s_bitstream *stream)
{
	s_unit_pickup_event_data *event = (s_unit_pickup_event_data *)data;
	event->type = (short)function_1959c0(stream, 3);
	event->object_name = event_read_scenario_object_name(stream);
	if (event->type == 1)
		function_195820(stream, event->extra, 16);
	return true;
}

// @retail 0x9f780
bool c_unit_melee_damage_event_definition::v10(long a, void *data, s_bitstream *stream)
{
	s_unit_melee_damage_event_data *event = (s_unit_melee_damage_event_data *)data;
	event->object_name = event_read_scenario_object_name(stream);
	if (function_1957d0(stream))
	{
		event->damage_type = function_1959c0(stream, 8);
		if (function_1957d0(stream))
		{
			event->material = function_1959c0(stream, 10);
			event->response = function_1959c0(stream, 17);
		}
		else
		{
			event->material = NONE;
			event->response = function_1959c0(stream, 17);
		}
	}
	else
	{
		event->damage_type = NONE;
		event->material = NONE;
		event->response = NONE;
	}
	function_195820(stream, event->location, 16);
	event->scale = event_dequantize_real(function_1959c0(stream, 8), 255, 0.0f, 1.0f);
	event->region = (byte)function_1959c0(stream, 8);
	return true;
}

// @retail 0x9bef0
void c_damage_aftermath_event_definition::v8(long a, long b, long c, long size, char *buffer)
{
	function_11c9c0(buffer, size, "damage aftermath: relevance=%5.3f", v7(a, b, c));
}
