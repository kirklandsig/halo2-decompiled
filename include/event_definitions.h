#pragma once

/* EVENT_DEFINITIONS.H: the simulation event definitions, one 12-slot vtable
   each, deriving from c_event_definition: the unit, vehicle, damage and
   breakable-surface events (src/unknown_09a5e0.cpp) and the projectile,
   weapon and game engine events (src/unknown_09b910.cpp). */

#include "unknown_11c920.h"

/* the size and shape of a "damage section response" event's data */
struct s_event_section_data
{
	byte unknown00[0x18];
	word *kind;
};

struct s_bitstream;

/* the event definition base: 12 slots (the type id, the name, a number, three
   shared slots, the size of the event's data, the relevance, its description,
   the encoding and decoding of the event's data, and the event's handling).
   Slots that no decompiled function owns keep placeholder bodies. */
class c_event_definition
{
public:
	virtual long v0() { return 0; }
	virtual const char *v1() { return 0; }
	virtual long v2() { return 0; }
	virtual long v3() { return 0; }
	virtual bool v4() { return true; }
	virtual bool v5(long a, long b) { return false; }
	virtual void v6(void *a, long b, long *size) {}
	virtual real v7(long a, long b, long c) { return 1.0f; }
	virtual void v8(long a, long b, long c, long size, char *buffer) {}
	virtual void v9(long a, void const *data, s_bitstream *stream) {}
	virtual bool v10(long a, void *data, s_bitstream *stream) { return false; }
	virtual bool v11(long a, long const *entities, long c, void const *data) { return false; }
};

/* the unit, vehicle and damage events (src/unknown_09a5e0.cpp); the encoding
   of the board, exit and enter vehicle events is one folded function */
class c_unit_melee_initiate_event_definition : public c_event_definition
{
public:
	virtual const char *v1();
	virtual void v6(void *a, long b, long *size);
	virtual void v8(long a, long b, long c, long size, char *buffer);
	virtual void v9(long a, void const *data, s_bitstream *stream);
	virtual bool v10(long a, void *data, s_bitstream *stream);
	virtual bool v11(long a, long const *entities, long c, void const *data);
};

class c_unit_pickup_event_definition : public c_event_definition
{
public:
	virtual const char *v1();
	virtual void v6(void *a, long b, long *size);
	virtual void v8(long a, long b, long c, long size, char *buffer);
	virtual void v9(long a, void const *data, s_bitstream *stream);
	virtual bool v10(long a, void *data, s_bitstream *stream);
	virtual bool v11(long arg_0, long const *arg_1, long arg_2, void const *arg_3);
};

class c_unit_grenade_release_event_definition : public c_event_definition
{
public:
	virtual long v0();
	virtual const char *v1();
	virtual long v2();
	virtual void v6(void *a, long b, long *size);
	virtual void v8(long a, long b, long c, long size, char *buffer);
	virtual void v9(long a, void const *data, s_bitstream *stream);
	virtual bool v10(long a, void *data, s_bitstream *stream);
	virtual bool v11(long a, long const *entities, long c, void const *data);
};

class c_vehicle_trick_event_definition : public c_event_definition
{
public:
	virtual const char *v1();
	virtual void v6(void *a, long b, long *size);
	virtual void v8(long a, long b, long c, long size, char *buffer);
	virtual void v9(long a, void const *data, s_bitstream *stream);
	virtual bool v10(long a, void *data, s_bitstream *stream);
	virtual bool v11(long a, long const *entities, long c, void const *data);
};

class c_vehicle_flip_event_definition : public c_event_definition
{
public:
	virtual long v0();
	virtual const char *v1();
	virtual void v8(long a, long b, long c, long size, char *buffer);
	virtual bool v11(long a, long const *entities, long c, void const *data);
};

class c_unit_grenade_initiate_event_definition : public c_event_definition
{
public:
	virtual long v0();
	virtual const char *v1();
	virtual void v6(void *a, long b, long *size);
	virtual void v8(long a, long b, long c, long size, char *buffer);
	virtual void v9(long a, void const *data, s_bitstream *stream);
	virtual bool v10(long a, void *data, s_bitstream *stream);
	virtual bool v11(long a, long const *entities, long c, void const *data);
};

class c_unit_board_vehicle_event_definition : public c_event_definition
{
public:
	virtual const char *v1();
	virtual void v6(void *a, long b, long *size);
	virtual void v8(long a, long b, long c, long size, char *buffer);
	virtual void v9(long a, void const *data, s_bitstream *stream);
	virtual bool v10(long a, void *data, s_bitstream *stream);
	virtual bool v11(long a, long const *entities, long c, void const *data);
};

class c_unit_exit_vehicle_event_definition : public c_event_definition
{
public:
	virtual const char *v1();
	virtual void v8(long a, long b, long c, long size, char *buffer);
	virtual bool v11(long a, long const *entities, long c, void const *data);
};

class c_unit_melee_damage_event_definition : public c_event_definition
{
public:
	virtual long v0();
	virtual const char *v1();
	virtual void v6(void *a, long b, long *size);
	virtual void v8(long a, long b, long c, long size, char *buffer);
	virtual void v9(long a, void const *data, s_bitstream *stream);
	virtual bool v10(long a, void *data, s_bitstream *stream);
	virtual bool v11(long arg_0, long const *arg_1, long arg_2, void const *arg_3);
};

class c_unit_enter_vehicle_event_definition : public c_event_definition
{
public:
	virtual const char *v1();
	virtual void v8(long a, long b, long c, long size, char *buffer);
	virtual bool v11(long a, long const *entities, long c, void const *data);
};

class c_breakable_surface_damage_event_definition : public c_event_definition
{
public:
	virtual const char *v1();
	virtual long v2();
	virtual void v6(void *a, long b, long *size);
	virtual real v7(long a, long b, long c);
	virtual void v8(long a, long b, long c, long size, char *buffer);
	virtual void v9(long a, void const *data, s_bitstream *stream);
	virtual bool v10(long a, void *data, s_bitstream *stream);
	virtual bool v11(long arg_0, long const *arg_1, long arg_2, void const *arg_3);
};

class c_damage_section_response_event_definition : public c_event_definition
{
public:
	virtual const char *v1();
	virtual void v6(void *a, long b, long *size);
	virtual void v8(long a, long b, long c, long size, char *buffer);
	virtual void v9(long a, void const *data, s_bitstream *stream);
	virtual bool v10(long a, void *data, s_bitstream *stream);
	virtual bool v11(long arg_0, long const *arg_1, long arg_2, void const *arg_3);
};

class c_damage_aftermath_event_definition : public c_event_definition
{
public:
	virtual const char *v1();
	virtual void v6(void *a, long b, long *size);
	virtual void v8(long a, long b, long c, long size, char *buffer);
	virtual void v9(long a, void const *data, s_bitstream *stream);
	virtual bool v10(long a, void *data, s_bitstream *stream);
	virtual bool v11(long arg_0, long const *arg_1, long arg_2, void const *arg_3);
};

class c_projectile_impact_effect_event : public c_event_definition
{
public:
	virtual bool v11(long a, long const *entities, long c, void const *data);
	virtual void v9(long a, void const *data, s_bitstream *stream);
	virtual bool v10(long a, void *data, s_bitstream *stream);
	virtual const char *v1();
	virtual void v6(void *a, long b, long *size);
	virtual void v8(long a, long b, long c, long size, char *buffer);
};

class c_projectile_effect_event : public c_event_definition
{
public:
	virtual real v7(long a, long b, long c);
	virtual bool v10(long a, void *data, s_bitstream *stream);
	virtual bool v11(long a, long const *entities, long c, void const *data);
	virtual void v9(long a, void const *data, s_bitstream *stream);
	virtual const char *v1();
	virtual long v2();
	virtual void v6(void *a, long b, long *size);
	virtual void v8(long a, long b, long c, long size, char *buffer);
};

class c_projectile_object_impact_effect_event : public c_event_definition
{
public:
	virtual bool v11(long a, long const *entities, long c, void const *data);
	virtual real v7(long a, long b, long c);
	virtual void v9(long a, void const *data, s_bitstream *stream);
	virtual bool v10(long a, void *data, s_bitstream *stream);
	virtual const char *v1();
	virtual long v2();
	virtual void v6(void *a, long b, long *size);
	virtual void v8(long a, long b, long c, long size, char *buffer);
};

class c_projectile_attached_event : public c_event_definition
{
public:
	virtual bool v11(long a, long const *entities, long c, void const *data);
	virtual const char *v1();
	virtual void v6(void *a, long b, long *size);
	virtual void v8(long a, long b, long c, long size, char *buffer);
	virtual void v9(long a, void const *data, s_bitstream *stream);
	virtual bool v10(long a, void *data, s_bitstream *stream);
};

class c_weapon_put_away_event : public c_event_definition
{
public:
	virtual bool v11(long a, long const *entities, long c, void const *data);
	virtual const char *v1();
	virtual void v6(void *a, long b, long *size);
	virtual void v8(long a, long b, long c, long size, char *buffer);
};

class c_weapon_fire_event : public c_event_definition
{
public:
	virtual bool v11(long a, long const *entities, long c, void const *data);
	virtual void v9(long a, void const *data, s_bitstream *stream);
	virtual bool v10(long a, void *data, s_bitstream *stream);
	virtual const char *v1();
	virtual long v2();
	virtual void v6(void *a, long b, long *size);
	virtual void v8(long a, long b, long c, long size, char *buffer);
};

class c_weapon_pickup_event : public c_event_definition
{
public:
	virtual bool v11(long a, long const *entities, long c, void const *data);
	virtual const char *v1();
	virtual void v6(void *a, long b, long *size);
	virtual void v8(long a, long b, long c, long size, char *buffer);
	virtual void v9(long a, void const *data, s_bitstream *stream);
	virtual bool v10(long a, void *data, s_bitstream *stream);
};

class c_weapon_effect_event : public c_event_definition
{
public:
	virtual bool v11(long a, long const *entities, long c, void const *data);
	virtual const char *v1();
	virtual void v6(void *a, long b, long *size);
	virtual void v8(long a, long b, long c, long size, char *buffer);
	virtual void v9(long a, void const *data, s_bitstream *stream);
	virtual bool v10(long a, void *data, s_bitstream *stream);
};

class c_weapon_drop_event : public c_event_definition
{
public:
	virtual bool v11(long a, long const *entities, long c, void const *data);
	virtual long v0();
	virtual const char *v1();
	virtual void v8(long a, long b, long c, long size, char *buffer);
	virtual void v9(long a, void const *data, s_bitstream *stream);
	virtual bool v10(long a, void *data, s_bitstream *stream);
};

class c_weapon_reload_event : public c_event_definition
{
public:
	virtual bool v11(long a, long const *entities, long c, void const *data);
	virtual const char *v1();
	virtual void v8(long a, long b, long c, long size, char *buffer);
};

class c_game_engine_request_boot_player_event : public c_event_definition
{
public:
	virtual bool v11(long a, long const *entities, long c, void const *data);
	virtual long v0();
	virtual const char *v1();
	virtual void v6(void *a, long b, long *size);
	virtual real v7(long a, long b, long c);
	virtual void v8(long a, long b, long c, long size, char *buffer);
	virtual void v9(long a, void const *data, s_bitstream *stream);
	virtual bool v10(long a, void *data, s_bitstream *stream);
};

/* the game engine event: its own slot 5 and 6 differ from the base event */
class c_game_engine_event
{
public:
	virtual long v0();
	virtual const char *v1();
	virtual long v2() { return 0; }
	virtual void v3() {}
	virtual void v4() {}
	virtual bool v5(struct s_event_holder *a, struct s_event_mask *b);
	virtual void v6(void *a, long b, long *size);
	virtual void v7() {}
	virtual void v8(long a, long b, long c, long size, char *buffer);
	virtual void v9(long a, void const *data, s_bitstream *stream);
	virtual bool v10(long a, void *data, s_bitstream *stream);
	virtual bool v11(long a, long const *entities, long c, void const *data);
};
