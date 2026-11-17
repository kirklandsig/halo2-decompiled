// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0E5300.CPP: the push a biped gives the vehicle it enters, leaves
   or boards (the unit actions call it), and whether a player's unit and a
   friendly AI unit can swap weapons (an outside function, 0x1515e0, calls
   it) */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_1cec30.h"
#include "unknown_1eb550.h"
#include "unknown_1e1f20.h"

/* an object header of g_4e0300 (12 bytes) */
struct s_e5300_object_header
{
	short salt;
	byte flags;
	byte type;
	byte unknown04[4];
	byte *object;
};

/* the object fields read here */
struct s_e5300_object
{
	long definition_index;
	byte unknown004[0x14 - 0x4];
	long parent_index;
	char parent_node;
	byte unknown019[0xb4 - 0x19];
	long havok_component_index;
	byte unknown0b8[0x12c - 0xb8];
	long actor_index;
};

#define E5300_OBJECT_HEADER_GET(index) (&((s_e5300_object_header *)g_4e0300->data)[(index) & 0xffff])
#define E5300_OBJECT_GET(index) ((s_e5300_object *)E5300_OBJECT_HEADER_GET(index)->object)

extern vector3f *g_4687bc;

point3f *function_b9dd0(long object_index, point3f *result);
long function_1cfe70(s_havok_component *component, long node_index);
real havok_component_rigid_body_mass_get(long rigid_body_index, s_havok_component *component);
void havok_component_rigid_body_point_impulse_apply(long rigid_body_index, s_havok_component *component,
	point3f const *point, vector3f const *velocity);

/* pushes the vehicle a biped is in downwards, at the biped's position: by the
   biped definition's +0x274 over the mass of the vehicle body at the biped's
   parent node, times the tick length, gravity and the vehicle definition's
   factor for entering (+0x274, entering 1: entering or boarding) or for
   leaving (+0x278, entering 0). Only a vehicle that is itself attached to
   another object is pushed */
// @retail 0xe5300
void function_e5300(long arg_159e6d_4, long entering)
{
	s_e5300_object *biped = E5300_OBJECT_GET(arg_159e6d_4);

	if (biped->parent_index != NONE)
	{
		s_e5300_object_header *vehicle_header = E5300_OBJECT_HEADER_GET(biped->parent_index);

		if ((1 << vehicle_header->type) & 2)
		{
			byte *local_0d520e = g_4e3b44[biped->definition_index & 0xffff].bytes;
			s_e5300_object *vehicle = (s_e5300_object *)vehicle_header->object;

			if (vehicle->parent_index != NONE)
			{
				byte *local_a2a045_2 = g_4e3b44[vehicle->definition_index & 0xffff].bytes;
				point3f position;
				real factor;

				function_b9dd0(arg_159e6d_4, &position);
				switch (entering)
				{
				case 0:
					factor = *(real *)(local_a2a045_2 + 0x278);
					break;
				case 1:
					factor = *(real *)(local_a2a045_2 + 0x274);
					break;
				default:
					__assume(0);
				}
				if (factor > 0.0f && vehicle->havok_component_index != NONE)
				{
					s_havok_component *component = havok_component_get(vehicle->havok_component_index);
					long rigid_body_index = function_1cfe70(component, biped->parent_node);

					if (rigid_body_index != NONE)
					{
						real mass = havok_component_rigid_body_mass_get(rigid_body_index, component);
						real weight = *(real *)(local_0d520e + 0x274) / mass;
						real scale = weight * g_510c54->rate * g_51e9c4->unknown0 * factor;
						vector3f velocity;

						velocity.i = g_4687bc->i * scale;
						velocity.j = g_4687bc->j * scale;
						velocity.k = g_4687bc->k * scale;
						havok_component_rigid_body_point_impulse_apply(rigid_body_index, component, &position,
							&velocity);
					}
				}
			}
		}
	}
}

/* a player (0x21c bytes; the fields read here) */
struct s_e5470_player
{
	byte unknown000[0x2c];
	long unit_index;
	byte unknown030[0x21c - 0x30];
};

void function_caf60(long unit_index, point3f *position);
void function_cb7e0(long unit_index, vector3f *vector);
void function_118e80(long object_index, vector3f *forward);
real function_30bf0(vector3f *vector);
bool __stdcall function_cd7b0(long unit_index, long weapon_index, bool *modes);

/* an object, with its header's address formed as well although nothing
   reads it: that keeps a later lookup of the same object from reusing this
   one, as in retail (see function_e5470) */
PRIVATE inline s_e5300_object *e5470_object_get(long object_index)
{
	s_e5300_object *object = E5300_OBJECT_GET(object_index);
	s_e5300_object_header *local_header = E5300_OBJECT_HEADER_GET(object_index);

	return object;
}

/* the weapon a unit holds (unit_get_current_weapon through
   e5470_object_get) */
PRIVATE inline long e5470_unit_current_weapon(long unit_index)
{
	s_unit_weapon_view *unit = (s_unit_weapon_view *)e5470_object_get(unit_index);
	short weapon = unit->current_weapon_index;
	long result = NONE;

	if (weapon != NONE)
	{
		result = unit->weapon_indices[weapon];
	}
	return result;
}

/* whether a player's unit and an AI unit next to it could swap the weapons
   they hold: both hold one, they stand less than 1.0 apart, the player's
   unit aims at the other and the other faces it, and each accepts the
   other's weapon (inferred).

   Near: the object lookups go through e5470_object_get, which reads
   g_4e0300's data pointer again for each object (0xe54bc, 0xe54e2) as
   retail does. Retail also reads the player's unit again for each use
   (0xe551f, 0xe5595, 0xe5638), which only a volatile player pointer
   reproduces here (11 instructions left with it); not kept. Re-measured on
   r809: adding `volatile` to the player pointer in the current body gives 121,
   so the 11 form above was a different body; the 47 body stays. */
// @retail 0xe5470
bool __stdcall function_e5470(long player_index, long unit_index)
{
	bool result = false;

	if (e5470_object_get(unit_index)->actor_index != NONE)
	{
		s_e5470_player *player = (s_e5470_player *)g_4e8c24->data + (player_index & 0xffff);
		long unit_weapon_index = e5470_unit_current_weapon(unit_index);
		long player_weapon_index = e5470_unit_current_weapon(player->unit_index);

		if (unit_weapon_index != NONE && player_weapon_index != NONE)
		{
			point3f player_position;
			point3f unit_position;
			vector3f direction;
			real distance;

			function_caf60(player->unit_index, &player_position);
			function_caf60(unit_index, &unit_position);
			direction.i = unit_position.x - player_position.x;
			direction.j = unit_position.y - player_position.y;
			direction.k = unit_position.z - player_position.z;
			distance = function_30bf0(&direction);
			if (distance > 0.0f && distance < 1.0f)
			{
				vector3f aim;

				function_cb7e0(player->unit_index, &aim);
				if (direction.i * aim.i + direction.j * aim.j + direction.k * aim.k > 0.8f)
				{
					vector3f forward;

					function_118e80(unit_index, &forward);
					if (forward.i * direction.i + forward.j * direction.j + forward.k * direction.k < 0.0f)
					{
						bool modes[4];

						if (function_cd7b0(unit_index, player_weapon_index, modes) && modes[2] &&
							function_cd7b0(player->unit_index, unit_weapon_index, modes) && modes[2])
						{
							result = true;
						}
					}
				}
			}
		}
	}
	return result;
}
