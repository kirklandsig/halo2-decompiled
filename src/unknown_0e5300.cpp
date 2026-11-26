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
#include "unknown_0259d0.h"

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
   other's weapon (inferred). */
// score 37 (was 47): vector3d_from_points3d + dot3f(&direction, &aim) + dot3f(&forward, &direction)
// match retail's 502 bytes and all FP instructions (+0x4a first diff). Earlier: e5470_object_get
// keeps g_4e0300->data reloads at 0xe54bc/0xe54e2; volatile player on r809 body gave 121 (on this
// 37 body, diagnostic volatile player matches 0/502B). Remaining: our compiler CSEs
// player->unit_index in eax across e5470_unit_current_weapon to 0xe551f (putting player in ebp and
// player_weapon_index in edi), where retail reloads [edi+0x2c] at 0xe551f (player in edi,
// player_weapon_index in ebp). Tried (fast): aim*dir 47;
// player macro each use 155; byte* player 47; __forceinline helpers 47; weapon ternary 127; nested
// weapon if 47; split cd7b0 modes 47; player_unit local 151; decl-then-assign weapons 47; clean
// header get 139; k+j+i dot 56; player_get/pool local/unit_weapon_in_slot/decl top/modes struct/
// unit local/player ref 37; unit_weapon before player 121; weapon if/else 128; header
// unit_get_current_weapon 99; non-inline helpers after 37 (clean 140); player_unit helper 33 (NOT
// kept: 1-caller helper); byte cast/ptr arg/two player ptrs/player_weapon helper/rename 37;
// player_unit ref/ptr 145; function_cbd50 forms 99-145; /Ob1, const player, nested weapon if 37;
// early return 134; player_weapon inside if 131; direct return weapon 128. GB20 (18 fast trials + 1 diag,
// 0xe5300 MATCH): const ref arg / both const ref 37; &player->unit_index at all 4 sites 145; s_player_datum
// type 37; dword unit_index 37; function_cbd50 inlined (direct or objget) 145 (519 B); two modes / nested
// weapon if / split pos decls / mask at call / player_get like objget / player method / actor local 37;
// char weapon 55; record pool lookup 121; header-first objget 140; inline_depth(1) diag 37.
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
			vector3d_from_points3d(&player_position, &unit_position, &direction);
			distance = function_30bf0(&direction);
			if (distance > 0.0f && distance < 1.0f)
			{
				vector3f aim;

				function_cb7e0(player->unit_index, &aim);
				if (dot3f(&direction, &aim) > 0.8f)
				{
					vector3f forward;

					function_118e80(unit_index, &forward);
					if (dot3f(&forward, &direction) < 0.0f)
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
