// @flags /O2 /arch:SSE /Gr
/* BIPEDS.CPP: bipeds

The biped object type's callbacks (its definition at 0x467a78) and the
helpers only they call (movement, physics, landing, melee and the biped's
animation). The callbacks upstream already has (unknown_0dc450.cpp,
unknown_0de080.cpp, unknown_0dc3a0.cpp, unknown_0e4050.cpp) stay in their
files; 0xdc370, at the start of the range, is c_a's destructor
(unknown_1efac0.h), which the linker placed here. */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_0259d0.h"
#include "unknown_1cec30.h"
#include "object_markers.h"
#include "unknown_11cc90.h"
#include "unknown_1efac0.h"
#include "unknown_1eb550.h"
#include "unknown_1cafc0.h"
#include <math.h>
#include <string.h>

#ifndef PIN
#define PIN(n,floor,ceiling) ((n)<(floor) ? (floor) : ((n)>(ceiling)?(ceiling):(n)))
#endif

#ifndef MIN
#define MIN(a,b) ((a)>(b)?(b):(a))
#endif

/* the biped (the unit fields, then the biped's own from +0x334; the fields
   read here) */
struct s_biped
{
	long definition_index;
	dword object_flags;
	byte unknown008[0xc];
	long parent_object_index;
	byte unknown018[0x70 - 0x18];
	vector3f forward;
	vector3f up;
	vector3f linear_velocity;
	vector3f angular_velocity;
	real scale;
	byte unknown0a4[0xb4 - 0xa4];
	long havok_component_index;
	long unknown0b8;
	long unknown0bc;
	word flags_c0;
	byte unknown0c2[0xd4 - 0xc2];
	long unknown0d4;
	byte unknown0d8[0x10a - 0xd8];
	byte flags_10a;
	byte unknown10b[0x116 - 0x10b];
	short node_matrices_offset;
	byte unknown118[0x12c - 0x118];
	long actor_index;
	byte unknown130[0x134 - 0x130];
	dword flags_134;
	byte unknown138[0x13c - 0x138];
	long unknown13c;
	byte unknown140[0x148 - 0x140];
	dword control_flags;
	byte unknown14c[0x150 - 0x14c];
	vector3f facing_goal;
	byte unknown15c[0x168 - 0x15c];
	vector3f unknown168;
	byte unknown174[0x1b0 - 0x174];
	vector3f control;
	bool unknown1bc;
	byte unknown1bd[0x1fc - 0x1bd];
	short seat_index;
	byte unknown1fe[0x2c4 - 0x1fe];
	real crouch;
	byte unknown2c8[0x2e4 - 0x2c8];
	real unknown2e4;
	byte unknown2e8[0x33e - 0x2e8];
	short unknown33e;
	byte unknown340[0x348 - 0x340];
	word flags_348;
	byte unknown34a;
	byte unknown34b;
	short unknown34c;
	byte unknown34e;
	byte unknown34f;
	long unknown350;
	long unknown354;
	long unknown358;
	long unknown35c;
	long unknown360;
	long unknown364;
	point3f unknown368;
	vector3f unknown374;
	point3f unknown380;
	long unknown38c;
	long unknown390;
	long unknown394;
	byte unknown398;
	char unknown399;
	byte unknown39a;
	byte unknown39b;
	char unknown39c;
	byte unknown39d;
	byte unknown39e;
	byte unknown39f;
	real unknown3a0;
	real unknown3a4;
	byte unknown3a8[2];
	byte unknown3aa;
	byte unknown3ab;
	byte unknown3ac[0x3c8 - 0x3ac];
	bool unknown3c8;
	byte unknown3c9[3];
	point3f unknown3cc;
	long unknown3d8;
	byte physics_mode;
	byte unknown3dd[3];
	long unknown3e0;
	long unknown3e4;
	long unknown3e8;
	byte unknown3ec[0x3f8 - 0x3ec];
	byte unknown3f8;
	byte unknown3f9;
	byte unknown3fa[0x404 - 0x3fa];
	long unknown404;
	long unknown408;
	byte unknown40c[0x434 - 0x40c];
	real unknown434;
	byte unknown438[0x444 - 0x438];
	vector3f unknown444;
	byte unknown450[0x45c - 0x450];
	real unknown45c;
};

struct s_biped_header
{
	byte unknown00[8];
	s_biped *biped;
};

/* a float rounded to an integer (fld, fistp) */
__forceinline long vehicle_round_ticks(real value)
{
	long result;

	__asm
	{
		fld value
		fistp result
	}
	return result;
}

#define BIPED_GET(index) (((s_biped_header *)g_4e0300->data)[(index) & 0xffff].biped)
#define BIPED_DEFINITION_GET(biped) (g_4e3b44[(biped)->definition_index & 0xffff].bytes)

void __stdcall function_c42e0(long unit_index, void const *placement);
void __stdcall function_de4c0(long arg_159e6d);
bool function_e4050(long object_index);
bool function_e0dc0(long unit_index);
bool function_25aad0(long actor_index, real *value);

/* places a biped from the scenario: the unit placement */
// @retail 0xdc4a0
void __stdcall biped_place(long arg_159e6d, byte const *placement)
{
	function_c42e0(arg_159e6d, placement + 0x4c);
}

/* the biped object type's callback that resets its physics state (+0x58) */
// @retail 0xdc540
bool __stdcall function_dc540(long arg_159e6d, long a, long b)
{
	function_de4c0(arg_159e6d);
	return true;
}

/* how a unit animation name moves a biped: 0 not at all (idle and turns),
   1 in place (the airborne and landing names), 2 otherwise */
// @retail 0xdd300
char function_dd300(long name)
{
	char result = 2;

	switch (name)
	{
	case 0xa00000f:
	case 0x900000e:
	case 0x400000c:
		result = 0;
		break;
	case 0x800001e:
	case 0x9000015:
	case 0x9000016:
	case 0xa000014:
	case 0xa000017:
	case 0xd000042:
		result = 1;
		break;
	}
	return result;
}

/* the change in the biped's crouch this tick: toward crouching while the
   crouch control is held, else toward standing, by the definition's
   crouch speed (an actor's own when it has one) */
// @retail 0xdd7d0
real function_dd7d0(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);
	real speed = *(real *)(definition + 0x250);
	real change;

	if (biped->actor_index != NONE &&
		*(long *)(g_4f55f0->data + (biped->actor_index & 0xffff) * 0x888 + 0x858) != NONE)
	{
		function_25aad0(biped->actor_index, &speed);
	}
	change = g_510c54->rate * speed;
	if (TEST_FIELD_BIT((biped->flags_134 >> 23) & 1))
	{
		real remaining = 1.0f - biped->crouch;

		if (change > remaining)
		{
			change = remaining;
		}
	}
	else
	{
		change = 0.0f - (change > biped->crouch ? biped->crouch : change);
	}
	if (biped->physics_mode == 6 && biped->unknown3f8 && (TEST_FIELD_BIT((biped->flags_348 >> 13) & 1)))
	{
		change = 1.0f - biped->crouch;
		if (!(1.0f > change))
		{
			change = 1.0f;
		}
	}
	return change;
}

/* the height the biped's crouching moves it this tick, when it crouches on
   the ground */
// @retail 0xdd8e0
bool __stdcall function_dd8e0(long arg_159e6d, real *height_change)
{
	byte *definition = BIPED_DEFINITION_GET(BIPED_GET(arg_159e6d));
	real change = function_dd7d0(arg_159e6d);
	bool result = false;

	if (!TEST_FIELD_BIT((*(dword *)(definition + 0x264) >> 1) & 1) &&
		(function_e4050(arg_159e6d) || function_e0dc0(arg_159e6d)) && change != 0.0f)
	{
		*height_change = (*(real *)(definition + 0x268) - *(real *)(definition + 0x26c)) *
			g_510c54->field_2_3 * change;
		result = true;
	}
	return result;
}

struct s_slot_entry_list;
extern s_slot_entry_list *g_4e0340;

/* whether the surface the biped stands on (with no object under it) has its
   first flag set */
// @retail 0xddf60
bool function_ddf60(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	bool result = false;

	if (biped->unknown35c == NONE && biped->unknown354 != NONE && biped->unknown360 == NONE)
	{
		result = *(*(byte **)((byte *)g_4e0340 + 0x2c) + biped->unknown354 * 8 + 4) & 1;
	}
	return result;
}

/* whether the biped is still: dead ones are not, ragdolls are when their
   physics rest, and the rest are when no control is pushed or they stand
   on the ground */
// @retail 0xddfb0
bool function_ddfb0(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	bool resting = false;

	if (!biped->unknown34b)
	{
		if (biped->havok_component_index != NONE)
		{
			s_havok_component *component = havok_component_get(biped->havok_component_index);

			if (*(byte *)(*(byte **)((byte *)component + 0x70) + 0x44) && !(TEST_FIELD_BIT((component->unknown04 >> 18) & 1)))
			{
				resting = true;
			}
		}
		if (biped->physics_mode == 6)
		{
			return true;
		}
		if (biped->physics_mode == 3)
		{
			return resting;
		}
		if (!(0.0001f > (real)fabs(biped->control.i * biped->control.i + biped->control.j * biped->control.j +
			biped->control.k * biped->control.k)))
		{
			return true;
		}
		long mode = biped->physics_mode;

		if (mode == 4 || mode == 5 || function_e4050(arg_159e6d))
		{
			return true;
		}
	}
	return resting;
}

/* the biped's character physics input (its fields past what 0x1e68a0
   fills) */
struct s_character_physics_update_input_datum_a;
struct s_source_a;
struct s_biped_physics_input
{
	byte unknown00[0x14];
	bool valid;
	byte unknown15[0x44 - 0x15];
	long unknown44;
	bool dead;
	byte unknown49[3];
	point3f position;
};

transform4x3f *function_ba160(long object_index, transform4x3f *matrix);
void function_1e68a0(s_character_physics_update_input_datum_a *datum, s_source_a *source, long a1, long a2, long a3,
	bool b0, bool b1, bool b2, bool b3, bool b4, point3f *p1, point3f *p2, point3f *p3);
void havok_component_rigid_body_position_get(long rigid_body_index, s_havok_component *component,
	point3f *position);

/* fills the biped's character physics input for its current physics mode
   (+0x3dc): its node matrix, facing, controls and Havok body */
// @retail 0xdd0d0
void __stdcall function_dd0d0(long arg_159e6d, s_biped_physics_input *input, bool flag)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);
	word const *animation_flags = (word const *)((byte *)biped + biped->unknown33e);
	transform4x3f matrix;
	vector3f facing;
	bool airborne;
	bool landing;
	long seat;

	function_ba160(arg_159e6d, &matrix);
	if (TEST_FIELD_BIT((*(dword *)(definition + 0xbc) >> 11) & 1))
	{
		facing = matrix.forward;
	}
	else
	{
		facing = BIPED_GET(arg_159e6d)->unknown168;
	}
	airborne = 0.16f > biped->unknown39c * g_510c54->rate;
	landing = (TEST_FIELD_BIT((*animation_flags >> 2) & 1)) && (TEST_FIELD_BIT((*animation_flags >> 4) & 1));
	if ((TEST_FIELD_BIT((biped->flags_c0 >> 2) & 1)) || (TEST_FIELD_BIT((biped->flags_c0 >> 1) & 1)))
	{
		seat = biped->unknown0b8;
	}
	else
	{
		seat = NONE;
	}
	function_1e68a0((s_character_physics_update_input_datum_a *)input, (s_source_a *)&biped->physics_mode,
		(long)(definition + 0x264), biped->havok_component_index, seat, TEST_FIELD_BIT((biped->flags_c0 >> 8) & 1),
		function_e4050(arg_159e6d), landing, airborne, flag, &matrix.position, (point3f *)&facing,
		(point3f *)&biped->control);
	switch (biped->physics_mode)
	{
	case 1:
		input->valid = true;
		input->unknown44 = biped->unknown408;
		break;
	case 2:
		input->valid = true;
		break;
	case 3:
		input->dead = biped->unknown34b != 0;
		input->valid = true;
		break;
	case 4:
	{
		s_havok_component *component = havok_component_get(biped->havok_component_index);
		char body = *((char *)component + 0x18);
		point3f position;

		havok_component_rigid_body_position_get(body >= 0 && body < component->rigid_bodies.size ? body : NONE,
			component, &position);
		input->position = position;
		input->valid = true;
		break;
	}
	}
}

bool __stdcall function_cba50(long unit_index, vector3f *direction, bool looking);
bool function_10f820(long object_index);
void function_de080(vector3f const *forward, vector3f const *up, vector2f *forward2d,
	vector2f *left2d);

/* moves the biped's camera offset (+0x3cc) toward its head marker,
   relative to its root node, a little each tick */
// @retail 0xde190
void function_de190(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);

	if (biped->unknown13c != NONE && !function_10f820(arg_159e6d))
	{
		transform4x3f const *nodes = (transform4x3f const *)((byte *)biped + biped->node_matrices_offset);
		s_object_marker marker;
		vector3f offset;
		vector3f aim;
		vector2f forward;
		vector2f left;
		point3f local;

		if (!function_b8d30(arg_159e6d, 0xb0000dd, &marker, 1, false))
		{
			function_b8d30(arg_159e6d, 0x4000095, &marker, 1, false);
		}
		offset.i = marker.matrix.position.x - nodes->position.x;
		offset.j = marker.matrix.position.y - nodes->position.y;
		offset.k = marker.matrix.position.z - nodes->position.z;
		aim = biped->unknown168;
		function_cba50(arg_159e6d, &aim, 0);
		function_de080(&aim, &biped->up, &forward, &left);
		local.x = forward.j * offset.j + forward.i * offset.i;
		local.y = left.j * offset.j + left.i * offset.i;
		local.z = offset.k;
		if (biped->unknown3c8)
		{
			biped->unknown3cc.x += PIN(local.x - biped->unknown3cc.x, -0.01f, 0.01f);
			biped->unknown3cc.y += PIN(local.y - biped->unknown3cc.y, -0.01f, 0.01f);
			biped->unknown3cc.z += PIN(local.z - biped->unknown3cc.z, -0.01f, 0.01f);
		}
		else
		{
			biped->unknown3cc = local;
			biped->unknown3c8 = true;
		}
	}
}

point3f *function_b9dd0(long object_index, point3f *result);
void function_e5930(long unit_index);

/* resets the biped's state: its flags, landing and melee timers, its ground
   contact and its physics mode */
// @retail 0xde4c0
void __stdcall function_de4c0(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);
	s_biped *current;

	biped->flags_348 = 0;
	biped->unknown34a = 0;
	biped->unknown34b = 0;
	biped->unknown34e = 0;
	biped->unknown34f = 0;
	biped->unknown34c = NONE;
	biped->unknown38c = NONE;
	biped->unknown390 = NONE;
	biped->unknown394 = NONE;
	biped->unknown398 = 0;
	biped->unknown399 = 0;
	biped->unknown39a = 0;
	biped->unknown39b = 0;
	biped->unknown39c = 0x7f;
	biped->unknown39d = 0;
	biped->unknown39e = 0;
	biped->crouch = 0.0f;
	biped->unknown3a0 = 0.0f;
	biped->unknown3a4 = 0.0f;
	function_b9dd0(arg_159e6d, &biped->unknown368);
	current = BIPED_GET(arg_159e6d);
	current->unknown354 = NONE;
	current->unknown358 = NONE;
	current->unknown35c = NONE;
	current->unknown360 = NONE;
	current->unknown364 = NONE;
	current->unknown350 = NONE;
	if ((*(dword *)(definition + 0x1f0) >> 1) & 1)
	{
		*(byte *)&biped->flags_348 |= 8;
	}
	else
	{
		*(byte *)&biped->flags_348 &= ~8;
	}
	biped->unknown3d8 = NONE;
	biped->unknown3e0 = arg_159e6d;
	biped->physics_mode = 0;
	biped->unknown3e4 = NONE;
	biped->unknown3e8 = NONE;
	function_e5930(arg_159e6d);
}

bool __stdcall function_de9d0(long arg_159e6d, long target_index);
void function_1e54d0(void *state, long a);

/* starts the biped toward an object to grab (+0x390), switching its
   physics to mode 6; NONE clears it */
// @retail 0xdee60
void function_dee60(long arg_159e6d, long target_index, bool flag)
{
	s_biped *biped = BIPED_GET(arg_159e6d);

	if (target_index == NONE)
	{
		biped->unknown390 = NONE;
		return;
	}
	if (biped->physics_mode != 6)
	{
		*((byte *)&biped->flags_348 + 1) &= ~0x20;
		biped->unknown3aa = 0;
		if (!flag && biped->unknown13c != NONE && !function_de9d0(arg_159e6d, target_index))
		{
			biped->unknown390 = NONE;
			return;
		}
		biped->unknown390 = target_index;
		function_1e54d0(&biped->physics_mode, 6);
		biped->unknown444 = biped->unknown168;
		biped->unknown3f8 = flag;
		if (flag && !((biped->flags_134 >> 23) & 1))
		{
			*((byte *)&biped->flags_348 + 1) |= 0x20;
		}
	}
}

/* the biped's physics output, filled by its physics mode (0x140 bytes) */
struct s_biped_physics_output
{
	byte unknown00[0x50];
	short unknown50;
	short unknown52;
	byte unknown54[0xf0 - 0x54];
	real unknownf0;
	byte unknownf4[0x140 - 0xf4];
};

long function_baf40(long object_index);
void __stdcall function_e3380(long arg_159e6d);
void function_e3700(long arg_159e6d);
long function_1469f0(real seconds);
bool function_e68c0(long type, long unit_index);
bool __stdcall function_e2d80(long arg_159e6d);
bool __stdcall function_e01d0(long arg_159e6d, long *names);
void __stdcall function_dc5c0(long arg_159e6d, s_biped_physics_output *output);
void function_1e5af0(void *physics, s_biped_physics_output *output, vector3f const *up, vector3f const *forward);
bool __stdcall function_e24f0(long arg_159e6d, long *names, s_biped_physics_output *output);
bool __stdcall function_e0ef0(long arg_159e6d);
bool __stdcall function_e3c90(long arg_159e6d, long *names, s_biped_physics_output *output);
void __stdcall function_e40d0(long arg_159e6d, long *names);
void __stdcall function_e4450(long arg_159e6d, long *names);
void function_e45e0(long arg_159e6d);
void function_e4c50(long arg_159e6d);
real function_30bf0(vector3f *v);
bool function_113e40(long unit_index);
void function_bba20(long object_index);
void function_b58c0(long index, dword mask);
bool __stdcall function_e6830(long unit_index);
bool __stdcall function_111650(long unit_index, long *names);
extern vector3f *g_4687a4;

/* the biped object type's update (+0x40): riding in a vehicle or biped,
   else its controls, its physics mode's movement, its animation, its
   camera and the unit update */
// @retail 0xdd360
bool __stdcall function_dd360(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);
	long parent_index = function_baf40(biped->parent_object_index);
	byte *state = (byte *)biped + *(short *)((byte *)biped + 0x12a);
	bool changed = false;
	long names[3];

	if (biped->havok_component_index == NONE)
	{
		return false;
	}
	names[0] = 0x7000101;
	names[1] = 0x400000c;
	names[2] = 0;
	changed = function_e2d80(arg_159e6d);
	if (parent_index != NONE)
	{
		s_biped_header *header = &((s_biped_header *)g_4e0300->data)[parent_index & 0xffff];
		byte parent_type = ((byte *)header)[3];

		if (parent_type == 1)
		{
			s_biped *vehicle = header->biped;

			function_e3380(arg_159e6d);
			if (*((byte *)g_4e6948 + 0xc) != 4)
			{
				function_e3700(arg_159e6d);
			}
			if (*((byte *)g_4e6948 + 0xc) != 4 && vehicle->unknown34b > 0)
			{
				long delay = vehicle_round_ticks(g_510c54->field_2_3 * 0.55f);

				if (biped->seat_index != NONE)
				{
					byte *local_a2a045 = BIPED_DEFINITION_GET(BIPED_GET(biped->parent_object_index));
					byte *seat = *(byte **)(local_a2a045 + 0x1cc) + biped->seat_index * 0xb0;

					if (*(real *)(seat + 0x20) > 0.0f)
					{
						delay = function_1469f0(*(real *)(seat + 0x20));
					}
				}
				if (vehicle->unknown34b > delay)
				{
					function_e68c0(0x1e, arg_159e6d);
				}
			}
			changed = true;
		}
		else if (parent_type == 0)
		{
			names[0] = ((header->biped->flags_10a >> 2) & 1) ? 0x700003d : 0x1000003c;
			changed = true;
		}
	}
	else
	{
		s_biped_physics_output output;

		if (((biped->flags_10a >> 2) & 1) ||
			(biped->physics_mode != 2 && !((*(dword *)(definition + 0x264) >> 3) & 1)))
		{
			biped->facing_goal.k = 0.0f;
			if (function_30bf0(&biped->facing_goal) == 0.0f)
			{
				biped->facing_goal = *g_4687a8;
			}
		}
		biped->unknown34a = function_dd300(*(long *)(state + 0x7c));
		if (0.1f * 0.1f > biped->control.i * biped->control.i + biped->control.j * biped->control.j +
			biped->control.k * biped->control.k)
		{
			biped->control = *g_4687a4;
		}
		if (biped->flags_348 & 1)
		{
			if (biped->unknown399 < 0x7f)
			{
				biped->unknown399++;
			}
			changed = true;
		}
		else
		{
			biped->unknown399 = 0;
		}
		if ((biped->flags_348 >> 1) & 1)
		{
			if ((char)biped->unknown39a < 0x7f)
			{
				biped->unknown39a++;
			}
			changed = true;
		}
		else
		{
			biped->unknown39a = 0;
		}
		output.unknown50 = NONE;
		output.unknown52 = NONE;
		changed |= function_e01d0(arg_159e6d, names);
		function_dc5c0(arg_159e6d, &output);
		function_1e5af0(&biped->physics_mode, &output, &biped->up, &biped->forward);
		changed |= function_e24f0(arg_159e6d, names, &output);
		switch (biped->physics_mode)
		{
		case 1:
			if (function_e4050(arg_159e6d))
			{
				function_e40d0(arg_159e6d, names);
			}
			else if (biped->unknown34c != NONE)
			{
				function_e4450(arg_159e6d, names);
			}
			else if ((biped->flags_348 >> 1) & 1)
			{
				function_e45e0(arg_159e6d);
			}
			changed = true;
			break;
		case 3:
			changed |= function_e0ef0(arg_159e6d);
			changed |= function_e3c90(arg_159e6d, names, &output);
			break;
		}
		function_e4c50(arg_159e6d);
	}
	function_de190(arg_159e6d);
	if (((biped->flags_10a >> 2) & 1) && (*((byte *)biped + 0xc1) & 1))
	{
		if (function_113e40(arg_159e6d))
		{
			function_bba20(arg_159e6d);
			changed = true;
		}
		else
		{
			*(byte *)&biped->flags_348 &= ~3;
		}
	}
	else
	{
		biped->unknown0bc = g_510c54->game_time;
		function_bba20(arg_159e6d);
		if (BIPED_GET(arg_159e6d)->unknown0d4 != NONE)
		{
			function_b58c0(BIPED_GET(arg_159e6d)->unknown0d4, 4);
		}
		changed = true;
	}

	bool action = function_e6830(arg_159e6d);

	return function_111650(arg_159e6d, names) | action | changed;
}

/* the result of the biped's character physics move (0x1e5a60) */
struct s_biped_physics_result
{
	byte unknown00[4];
	byte flags;
	byte unknown05[3];
	long contact_index;
	long unknown0c;
};

struct s_type_1e6529
{
	long definition_index;
	dword flags;
	byte unknown08[0x7c - 0x8];
	short material_index;
	byte unknown7e[0x84 - 0x7e];
	bool unknown84;
	byte unknown85[0x88 - 0x85];
};

struct s_damage_owner;
struct s_table_holder;
void function_e30c0(long arg_159e6d);
bool function_1ec500(long arg_159e6d);
void function_d6660(s_type_1e6529 *data, long definition_index);
void __stdcall object_get_damage_owner(long object_index, s_damage_owner *owner);
void __stdcall function_d7b80(s_type_1e6529 *data, long object_index, short node_index, short unknown0c, short region_entry_index,
	vector3f const *unknown14);
void function_ba1d0(long object_index, vector3f *linear_velocity, vector3f *angular_velocity);
void function_b7740(long object_index, vector3f const *linear_velocity, vector3f const *angular_velocity,
	bool unknown);
void function_1c4b00(long object_index, void *a, void *b, long c);
void __stdcall function_b9b90(long object_index, bool disable);
void function_b7360(long object_index);
bool function_e4680(long arg_159e6d);
bool havok_component_any_rigid_body_active(s_havok_component *component);
void function_1e5a60(s_biped_physics_result *result, s_biped_physics_input const *input, void *physics);
bool havok_component_unknown10_recent(s_havok_component const *component);
void function_e4330(long arg_159e6d, real height);
real function_1201a0(vector3f *v, vector3f const *fallback);
void __stdcall function_e2fa0(long arg_159e6d, long unknown, void *a);
void *function_1efd80(s_table_holder *holder, dword position);
long function_baf80(long object_index);
extern long *g_51e9cc;

/* whether the first flag of a surface in a collision bsp's surfaces is set */
#define BIPED_SURFACE_FLAG(bsp, surface) (*(*(byte **)((byte *)(bsp) + 0x2c) + (surface) * 8 + 4) & 1)

/* the biped's physics callback: its crouch, kill volumes, its character
   physics move, and the ground (structure, instanced geometry or object)
   its contact rests on */
// @retail 0xdd990
void __stdcall function_dd990(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);

	if (biped->havok_component_index != NONE)
	{
		s_havok_component *component = havok_component_get(biped->havok_component_index);
		s_biped *root = BIPED_GET(function_baf80(arg_159e6d));
		real crouch_change = function_dd7d0(arg_159e6d);
		real height_change;
		bool physics;

		function_e30c0(arg_159e6d);
		if (!(*((byte *)root + 0xc1) & 1) && function_1ec500(arg_159e6d))
		{
			*((byte *)&biped->flags_348 + 1) |= 0x80;
			if (*((byte *)g_4e6948 + 0xc) != 4)
			{
				s_type_1e6529 data;

				data.material_index = NONE;
				function_d6660(&data, *(long *)(*(byte **)((byte *)g_4e034c + 0x144) + 0x28));
				data.unknown84 = true;
				data.flags |= 4;
				object_get_damage_owner(arg_159e6d, (s_damage_owner *)data.unknown08);
				function_d7b80(&data, arg_159e6d, NONE, NONE, NONE, NULL);
			}
		}
		if (function_dd8e0(arg_159e6d, &height_change))
		{
			vector3f velocity;

			function_ba1d0(arg_159e6d, &velocity, NULL);
			velocity.k -= height_change;
			function_b7740(arg_159e6d, &velocity, NULL, false);
			function_1c4b00(arg_159e6d, &velocity, NULL, 1);
			if (velocity.i * velocity.i + velocity.j * velocity.j + velocity.k * velocity.k > 0.0001f)
			{
				function_b9b90(arg_159e6d, false);
				function_b7360(arg_159e6d);
				function_bba20(arg_159e6d);
			}
		}
		biped->crouch = PIN(crouch_change + biped->crouch, 0.0f, 1.0f);
		*((byte *)&biped->flags_348 + 1) &= ~8;
		physics = function_e4680(arg_159e6d);
		if (havok_component_any_rigid_body_active(component) || physics || !(TEST_FIELD_BIT((biped->flags_c0 >> 6) & 1)))
		{
			*((byte *)&biped->flags_348 + 1) &= ~0x10;
			function_b9dd0(arg_159e6d, &biped->unknown368);
			if (!(TEST_FIELD_BIT((biped->flags_c0 >> 8) & 1)))
			{
				biped->unknown354 = NONE;
				biped->unknown358 = NONE;
				biped->unknown35c = NONE;
				biped->unknown360 = NONE;
				biped->unknown364 = NONE;
			}
			if ((biped->parent_object_index == NONE && (TEST_FIELD_BIT((biped->flags_c0 >> 6) & 1))) ||
				(biped->physics_mode == 3 && biped->unknown404 != NONE))
			{
				s_biped_physics_input input;
				s_biped_physics_result result;
				long contact;

				function_dd0d0(arg_159e6d, &input, physics);
				function_1e5a60(&result, &input, &biped->physics_mode);
				contact = result.contact_index;
				if (!(*((byte *)biped + 0xc1) & 1) && contact != NONE &&
					contact >= 0 && contact <= component->unknown88.size)
				{
					byte *entry = (byte *)component->unknown88.data + contact * 0x48;
					long key = *(long *)(entry + 4);
					bool found = false;

					if (key != NONE)
					{
						long index = key & 0xffff;
						long surface = (key >> 16) & 0x1fff;

						switch ((dword)key >> 29)
						{
						case 1:
							biped->unknown35c = NONE;
							biped->unknown354 = index;
							biped->unknown360 = NONE;
							biped->unknown364 = NONE;
							biped->unknown358 = NONE;
							found = true;
							break;
						case 5:
						{
							byte *structure = (byte *)g_4e0348;
							byte *instance = *(byte **)(structure + 0x144) + index * 0x58;
							byte *definition = *(byte **)(structure + 0x13c) + *(short *)(instance + 0x34) * 0xc8;
							byte *bsp = *(byte **)(*(byte **)(definition + 0xb4) + 0xc);

							biped->unknown35c = index;
							if (!BIPED_SURFACE_FLAG(bsp, surface))
							{
								biped->unknown354 = surface;
								biped->unknown360 = NONE;
								biped->unknown364 = NONE;
								biped->unknown358 = NONE;
							}
							found = true;
							break;
						}
						case 4:
						{
							long object_index = g_51e9cc[index];

							if (object_index != NONE && (TEST_FIELD_BIT((BIPED_GET(object_index)->object_flags >> 22) & 1)))
							{
								s_biped *object = BIPED_GET(object_index);
								byte *model = g_4e3b44[*(long *)(BIPED_DEFINITION_GET(object) + 0x38) & 0xffff].bytes;
								byte *region = *(byte **)(model + 0x74) + (surface & 0x1f) * 0x10;
								long permutation = (char)(*(byte **)(region + 0xc))[(surface >> 5 & 0xff) * 8 + 5] & 0xff;
								dword position = (((char)region[4] << 8) | permutation) << 16;
								s_lookup lookup;

								if (lookup.initialize(object_index))
								{
									byte *bsp = (byte *)function_1efd80((s_table_holder *)&lookup, position);

									if (!BIPED_SURFACE_FLAG(bsp, *(long *)(entry + 0xc)))
									{
										biped->unknown35c = NONE;
										biped->unknown360 = object_index;
										biped->unknown364 = position;
										biped->unknown354 = *(long *)(entry + 0xc);
										biped->unknown358 = NONE;
										found = true;
									}
								}
							}
							break;
						}
						}
					}
					if (!found && *(long *)(entry + 0x18) != NONE &&
						(TEST_FIELD_BIT((BIPED_GET(*(long *)(entry + 0x18))->object_flags >> 22) & 1)))
					{
						biped->unknown35c = NONE;
						biped->unknown360 = *(long *)(entry + 0x18);
						biped->unknown354 = NONE;
						biped->unknown364 = NONE;
						biped->unknown358 = NONE;
					}
				}
				if (biped->physics_mode == 1 && (result.flags & 2))
				{
					real height = biped->unknown45c;

					if (havok_component_unknown10_recent(component))
					{
						height = component->unknown14 + height;
					}
					function_e4330(arg_159e6d, height);
				}
				if (contact != NONE)
				{
					byte *entry = (byte *)component->unknown88.data + contact * 0x48;

					biped->unknown380 = *(point3f *)(entry + 0x1c);
					biped->unknown374 = *(vector3f *)(entry + 0x28);
					function_1201a0(&biped->unknown374, g_4687b0);
					*((byte *)&biped->flags_348 + 1) |= 0x10;
					biped->unknown368 = biped->unknown380;
				}
				function_e2fa0(arg_159e6d, result.unknown0c, *(byte **)((byte *)component + 0x70) + 0xc);
				if (result.flags & 1)
				{
					biped->flags_348 |= 1;
				}
				else
				{
					*(byte *)&biped->flags_348 &= ~1;
				}
			}
		}
		*(byte *)&biped->flags_348 &= 0x7f;
	}
}

struct s_type_94656b;
struct s_character_physics_component;
void function_1e5bb0(s_biped_physics_output *output, void *physics, void *state, real speed_scale, long havok_component_index,
	long arg_159e6d, void const *definition_physics, long a, bool b, bool turning, bool c, bool landing, bool d,
	bool grounded, bool e, bool f, real gravity, real boost, vector3f const *control, point3f const *position,
	vector3f const *forward, vector3f const *up, vector3f const *facing_goal,
	vector3f const *facing, vector3f const *ground_velocity, long material);
void function_1e6120(s_biped_physics_output *output, void *physics, real rate, bool airborne, bool b, bool c, real crouch);
void function_1e6360(s_biped_physics_output *output, real height, long arg_159e6d, real crouch);
void function_1e67a0(s_character_physics_component *component, s_type_94656b *datum, byte a,
	byte b);
void function_1e67f0(s_type_94656b *datum,
	s_character_physics_component *component, long animation_id, point3f *p1, point3f *p2, point3f *p3);
void function_1e6850(s_type_94656b *datum, byte a, point3f *p1, point3f *p2, real v);
real function_1e96a0(short column, short row);
real function_dfa60(long arg_159e6d);
real function_d1210(long object_index);
bool __stdcall function_183670(long component_a, long component_b, point3f *a, point3f *b, real *distance);
void __stdcall function_e5d50(long arg_159e6d, long target_index, vector3f *offset, point3f *point);
vector3f *function_11d090(vector3f const *v, vector3f *out);
void function_df380(long unit_index, point3f *position, vector3f *forward, vector3f *up);
void __stdcall function_df5f0(long object_index, point3f *center, real *height, real *radius);
void function_e59e0(long arg_159e6d, vector3f *velocity, point3f *position);
struct s_collision_result_1697c0;
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);

/* the collision result as the biped's grab reads it */
struct s_biped_collision
{
	long type;
	real t;
	point3f point;
	byte unknown14[0x24 - 0x14];
	short unknown24;
	byte unknown26[0x5c - 0x26];
};

/* the point a biped grabs from: its eye (its standing or crouching height
   up from its origin), moved to its head marker unless something is in
   the way */
PRIVATE inline void biped_eye_raise(long arg_159e6d, real crouch, point3f *eye)
{
	s_biped *self = BIPED_GET(arg_159e6d);
	byte *self_definition = BIPED_DEFINITION_GET(self);
	point3f head;
	vector3f forward;
	vector3f up;
	vector3f direction;

	eye->z += ((1.0f - crouch) * *(real *)(self_definition + 0x218) + *(real *)(self_definition + 0x21c) * crouch) *
		self->scale;
	head = *eye;
	forward = self->unknown168;
	function_11d090(&forward, &up);
	function_df380(arg_159e6d, &head, &forward, &up);
	direction.i = head.x - eye->x;
	direction.j = head.y - eye->y;
	direction.k = head.z - eye->z;
	if (function_30bf0(&direction) > 0.0f)
	{
		s_biped_collision collision;
		vector3f back;
		point3f start;

		collision.unknown24 = NONE;
		back.i = direction.i * -0.25f;
		back.j = direction.j * -0.25f;
		back.k = direction.k * -0.25f;
		if (function_1697c0(0x4808c2d, eye, &back, NONE, NONE, (s_collision_result_1697c0 *)&collision))
		{
			real t = collision.t * 0.9f;

			start.x = back.i * t + eye->x;
			start.y = back.j * t + eye->y;
			start.z = back.k * t + eye->z;
		}
		else
		{
			start = collision.point;
		}
		if (function_1697c0(0x4808c2d, &start, &direction, NONE, NONE, (s_collision_result_1697c0 *)&collision) &&
			(head.z - start.z) * direction.k + (head.y - start.y) * direction.j + (head.x - start.x) * direction.i +
			0.05f > (collision.point.z - start.z) * direction.k + (collision.point.y - start.y) * direction.j +
			(collision.point.x - start.x) * direction.i)
		{
			eye->x = collision.point.x - direction.i * 0.05f;
			eye->y = collision.point.y - direction.j * 0.05f;
			eye->z = collision.point.z - direction.k * 0.05f;
		}
		else
		{
			*eye = head;
		}
	}
}

PRIVATE inline void biped_grab_eye(long arg_159e6d, point3f *eye)
{
	function_b9dd0(arg_159e6d, eye);
	biped_eye_raise(arg_159e6d, function_e0dc0(arg_159e6d) ? 0.0f : BIPED_GET(arg_159e6d)->crouch, eye);
}

/* fills the biped's character physics input for its physics mode: walking
   (1), flying (2), dead (3), animation driven (4, 5) or grabbing (6) */
// @retail 0xdc5c0
void __stdcall function_dc5c0(long arg_159e6d, s_biped_physics_output *output)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);
	s_havok_component *component = havok_component_get(biped->havok_component_index);
	byte *state = (byte *)biped + *(short *)((byte *)biped + 0x12a);
	word const *animation_flags = (word const *)((byte *)biped + biped->unknown33e);
	bool airborne = *(long *)(state + 0x7c) == 0xe0000c2;
	real speed_scale = 1.0f;
	long material = 0;
	real boost;
	point3f position;
	vector3f ground_velocity;
	vector3f facing;
	byte *ground = *(byte **)(*(byte **)((byte *)component + 0x70) + 0x40);

	function_b9dd0(arg_159e6d, &position);
	if (!ground[0x40])
	{
		ground_velocity = *(vector3f *)(*(byte **)(ground + 0x3c) + 0x40);
	}
	else
	{
		ground_velocity = *g_4687a4;
	}
	if (TEST_FIELD_BIT((*(dword *)(definition + 0xbc) >> 11) & 1))
	{
		facing = biped->forward;
	}
	else
	{
		facing = biped->unknown168;
	}
	if (biped->unknown34c == 1)
	{
		speed_scale = 0.0f;
	}
	else
	{
		if (biped->physics_mode == 1 && (TEST_FIELD_BIT((*(dword *)(definition + 0x264) >> 2) & 1)) && !airborne)
		{
			byte *globals = *(byte **)((byte *)g_4e034c + 0x134);

			speed_scale = (1.0f - biped->unknown2e4 * *(real *)(globals + 0x78)) * function_dfa60(arg_159e6d);
		}
		if ((TEST_FIELD_BIT((*(dword *)(definition + 0x1f0) >> 6) & 1)) && !biped->unknown1bc)
		{
			short players = 1;
			real spread;

			if (*(long *)((byte *)g_4e6948 + 8) == 1)
			{
				players = *(short *)((byte *)g_4e6948 + 0x132);
			}
			spread = function_1e96a0(players, 8);
			speed_scale = (dword)arg_159e6d % 0x89 * 0.0072992700f * spread + 1.0f;
		}
	}
	if (!(biped->control_flags & 1) && (TEST_FIELD_BIT((biped->flags_134 >> 23) & 1)))
	{
		if (*(long *)(definition + 0x28c))
		{
			material = *(long *)(definition + 0x290) + 0x20;
		}
		else
		{
			material = *(long *)(definition + 0x298) + 0x30;
		}
	}
	if (biped->control_flags & 0x800)
	{
		boost = function_d1210(arg_159e6d) * (*(real *)(definition + 0x1d0) - 1.0f) + 1.0f;
	}
	else
	{
		boost = 1.0f;
	}

	bool turning = *(long *)(state + 0x7c) == 0x900000e || *(long *)(state + 0x7c) == 0xa00000f;

	function_1e5bb0(output, &biped->physics_mode, state, speed_scale, biped->havok_component_index, arg_159e6d,
		definition + 0x264, 9, TEST_FIELD_BIT((biped->flags_c0 >> 6) & 1), turning, TEST_FIELD_BIT((biped->control_flags >> 3) & 1),
		TEST_FIELD_BIT((*animation_flags >> 2) & 1), TEST_FIELD_BIT((biped->flags_c0 >> 8) & 1), function_e4050(arg_159e6d),
		TEST_FIELD_BIT((biped->flags_348 >> 1) & 1), biped->unknown39d != 0, g_51e9c4->unknown0, boost, &biped->control, &position,
		&biped->forward, &biped->up, &biped->facing_goal, &facing, &ground_velocity, material);
	switch (biped->physics_mode)
	{
	case 1:
		function_1e6120(output, &biped->physics_mode,
			g_510c54->rate / *(real *)(definition + 0x214) * (real)biped->unknown39d, airborne,
			*(long *)(state + 0x70) == 0x6000085, TEST_FIELD_BIT((biped->flags_348 >> 7) & 1), biped->crouch);
		break;
	case 2:
		function_1e6360(output, biped->unknown3a0, arg_159e6d, biped->crouch);
		break;
	case 3:
		function_1e67a0((s_character_physics_component *)&biped->physics_mode,
			(s_type_94656b *)output, 1, biped->unknown34b != 0);
		break;
	case 4:
	case 5:
	{
		vector3f velocity;
		point3f body;
		c_type_709360 current;
		long animation_id;

		function_e59e0(arg_159e6d, &velocity, &position);
		havok_component_rigid_body_position_get(0, component, &body);
		animation_id = *(long *)((s_animation_state *)state)->current_animation_get(&current);
		function_1e67f0((s_type_94656b *)output,
			(s_character_physics_component *)&velocity, animation_id, &position, &body, &body);
		break;
	}
	case 6:
	{
		point3f target_position = position;
		bool moved = false;
		real distance = 0.0f;

		ground_velocity = *g_4687a4;
		if (biped->unknown390 != NONE)
		{
			long root_index = function_baf80(biped->unknown390);
			s_biped *root = BIPED_GET(root_index);

			if (root->havok_component_index != NONE)
			{
				point3f nearest_a;
				point3f nearest_b;
				s_biped_header *header;

				function_b9dd0(biped->unknown390, &target_position);
				function_ba1d0(root_index, &ground_velocity, NULL);
				if (function_183670(root->havok_component_index, biped->havok_component_index, &nearest_a, &nearest_b,
					&distance))
				{
					point3f point;
					real dx;
					real dy;

					function_e5d50(arg_159e6d, biped->unknown390, &biped->unknown444, &point);
					dx = nearest_b.x - nearest_a.x;
					dy = nearest_b.y - nearest_a.y;
					distance = (real)sqrt(dx * dx + dy * dy);
					if (!(distance > 0.0f))
					{
						distance = 0.0f;
					}
					moved = true;
				}
				header = &((s_biped_header *)g_4e0300->data)[biped->unknown390 & 0xffff];
				if (((1 << ((byte *)header)[3]) & 1) && header->biped->physics_mode == 2)
				{
					point3f eye;
					real radius;
					real height;
					point3f center;

					biped_grab_eye(arg_159e6d, &eye);
					function_df5f0(biped->unknown390, &center, &height, &radius);
					radius *= 2.0f;
					if (!(radius > height))
					{
						radius = height;
					}
					target_position.z -= radius * 0.5f + center.z - (eye.z - output->unknownf0);
				}
			}
		}
		if (!moved && !biped->unknown3f9)
		{
			target_position.x = facing.i * 0.3f + position.x;
			target_position.y = facing.j * 0.3f + position.y;
			target_position.z = facing.k * 0.3f + position.z;
			biped->unknown434 = 0.3f;
			distance = 0.3f;
			ground_velocity = *g_4687a4;
			moved = true;
		}
		function_1e6850((s_type_94656b *)output, moved, (point3f *)&ground_velocity,
			&target_position, distance);
		break;
	}
	}
}

struct s_location;
void __stdcall function_e57e0(long arg_159e6d);
void __stdcall function_1ed340(void *physics, long arg_159e6d);
void __stdcall function_e18d0(long arg_159e6d, bool flag);
void __stdcall function_b7880(long object_index, long node_index, point3f const *point,
	vector3f const *impulse, vector3f const *angular_impulse);
void function_b75a0(long object_index, point3f const *point, vector3f const *forward,
	vector3f const *up, s_location const *location, bool unknown);

/* a biped's response to a damage impulse: it starts flinching or falling,
   a hard one knocks it over (and a ragdoll gets a random spin), and on
   the ground it turns to face away from the hit */
// @retail 0xde620
void __stdcall function_de620(long arg_159e6d, vector3f const *impulse)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);

	if (!(TEST_FIELD_BIT((*(dword *)(definition + 0xbc) >> 20) & 1)))
	{
		bool strong = impulse->i * impulse->i + impulse->j * impulse->j + impulse->k * impulse->k > 1.4f * 1.4f;
		bool spinning = false;
		vector3f velocity = *impulse;
		vector3f spin = *g_4687a4;
		vector3f const *up = g_4687b0;

		if (!(TEST_FIELD_BIT((biped->flags_10a >> 2) & 1)))
		{
			velocity.i *= 0.5f;
			velocity.j *= 0.5f;
			velocity.k *= 0.5f;
		}
		biped->flags_348 |= 2;
		if ((TEST_FIELD_BIT((biped->flags_10a >> 2) & 1)) || (TEST_FIELD_BIT((*(dword *)(definition + 0x264) >> 3) & 1)) || biped->physics_mode == 2)
		{
			vector3f axis;
			real magnitude;
			real angle;

			axis.i = up->j * velocity.k - up->k * velocity.j;
			axis.j = up->k * velocity.i - up->i * velocity.k;
			axis.k = up->i * velocity.j - up->j * velocity.i;
			function_30bf0(&axis);
			magnitude = (real)sqrt(velocity.j * velocity.j + velocity.k * velocity.k + velocity.i * velocity.i);
			angle = (real)random_next(&g_4e7408->unknown0) * 1.5259022e-05f * magnitude * 1.5707964f;
			spin.i = angle * axis.i;
			spin.j = axis.j * angle;
			spin.k = axis.k * angle;
			spinning = strong || !(TEST_FIELD_BIT((biped->flags_10a >> 2) & 1));
			if (!spinning)
			{
				goto apply;
			}
		}
		if (strong)
		{
			function_e57e0(arg_159e6d);
			if (biped->physics_mode == 3)
			{
				biped->unknown3f9 = 0;
				biped->unknown3f8 = 0;
				function_1ed340((byte *)biped + 0x3ec, arg_159e6d);
				function_e18d0(arg_159e6d, true);
			}
		}
apply:
		if (biped->physics_mode != 3 || strong || spinning)
		{
			point3f position;
			bool linear = biped->physics_mode != 3 || strong;

			function_b9dd0(arg_159e6d, &position);
			function_b7880(arg_159e6d, NONE, &position, linear ? &velocity : NULL, spinning ? &spin : NULL);
		}
		if (biped->parent_object_index == NONE && biped->unknown13c == NONE && biped->physics_mode != 4 &&
			biped->physics_mode != 5 && !biped->unknown34b && (strong || biped->physics_mode != 3) &&
			impulse->j * impulse->j + impulse->k * impulse->k + impulse->i * impulse->i > 1.4f * 1.4f)
		{
			vector3f away;

			away.i = impulse->i;
			away.j = impulse->j;
			away.k = 0.0f;
			if (function_30bf0(&away) > 0.0f)
			{
				function_b75a0(arg_159e6d, NULL, &away, up, NULL, false);
			}
		}
	}
}

/* whether the biped can see a biped it would grab: a ray from its eye to
   the target's center (or to three points up the target's height) gets
   through */
// @retail 0xde9d0
bool __stdcall function_de9d0(long arg_159e6d, long target_index)
{
	s_biped_header *header = &((s_biped_header *)g_4e0300->data)[target_index & 0xffff];

	if ((1 << ((byte *)header)[3]) & 1)
	{
		s_biped *target = header->biped;
		byte *target_definition = BIPED_DEFINITION_GET(target);
		point3f center;
		point3f eye;
		real height;
		long count;
		long i;

		if (*(dword *)(target_definition + 0x264) & 1)
		{
			center = *(point3f *)((byte *)target + 0x30);
			count = 1;
			height = 0.0f;
		}
		else
		{
			real span;

			function_df5f0(target_index, &center, &height, &span);
			height += *(real *)(target_definition + 0x270);
			center.z -= *(real *)(target_definition + 0x270) * 0.5f;
			count = 3;
		}
		biped_grab_eye(arg_159e6d, &eye);
		for (i = 0; i < count; i++)
		{
			s_biped_collision collision;
			vector3f vector;

			vector.i = center.x - eye.x;
			vector.j = center.y - eye.y;
			vector.k = center.z - eye.z;
			collision.unknown24 = NONE;
			if (!function_1697c0(0x84000d, &eye, &vector, arg_159e6d, target_index,
				(s_collision_result_1697c0 *)&collision))
			{
				return true;
			}
			if (count > 1)
			{
				center.z += height / (real)(count - 1);
			}
		}
		return false;
	}
	return true;
}

/* sets the biped's flag 7 (+0x348) */
// @retail 0xdef30
word *function_def30(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);

	biped->flags_348 |= 0x80;
	return &biped->flags_348;
}

/* a biped's center (raised by its radius unless it stands on its origin),
   its height (standing to crouching, less its radius at both ends) and its
   radius */
// @retail 0xdf5f0
void __stdcall function_df5f0(long object_index, point3f *center, real *height, real *radius)
{
	s_biped *biped = BIPED_GET(object_index);
	byte *definition = BIPED_DEFINITION_GET(biped);

	function_b9dd0(object_index, center);
	if (!(*(dword *)(definition + 0x264) & 1))
	{
		center->z += *(real *)(definition + 0x270);
	}
	if (TEST_FIELD_BIT((*(dword *)(definition + 0x264) >> 1) & 1))
	{
		*height = 0.0f;
		*radius = *(real *)(definition + 0x270);
	}
	else
	{
		*height = (*(real *)(definition + 0x26c) - *(real *)(definition + 0x268)) * biped->crouch +
			*(real *)(definition + 0x268) - *(real *)(definition + 0x270) * 2.0f;
		*radius = *(real *)(definition + 0x270);
	}
}

real function_17ca10(real x, short curve);

/* the speed a player's biped moves at: its player's speed, plus the sprint
   boost while its sprint meter lasts */
// @retail 0xdfa60
real function_dfa60(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	real speed = 1.0f;

	if (biped->unknown13c != NONE)
	{
		byte *player = g_4e8c24->data + (biped->unknown13c & 0xffff) * 0x21c;
		byte *globals = *(byte **)((byte *)g_4e034c + 0xf4);
		long delay = vehicle_round_ticks(g_510c54->field_2_3 * *(real *)(globals + 0x20));
		long duration = vehicle_round_ticks(g_510c54->field_2_3 * *(real *)(globals + 0x24));
		long used = vehicle_round_ticks(*(short *)(player + 0x17e) * *(real *)(globals + 0x28));
		long remaining = *(short *)(player + 0x17c) - used;
		real sprint = 0.0f;

		speed = *(real *)(player + 0x19c);
		if (remaining > delay)
		{
			real fraction = (real)(remaining - delay) / (real)(duration - 1);

			sprint = function_17ca10(PIN(fraction, 0.0f, 1.0f), 1);
			if (sprint > 0.0f)
			{
				*(byte *)&biped->flags_348 |= 0x10;
			}
		}
		speed = (*(real *)(globals + 0x2c) - 1.0f) * sprint + speed;
	}
	return speed;
}

/* forgets the ground the biped stands on */
// @retail 0xdfd70
void function_dfd70(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);

	biped->unknown354 = NONE;
	biped->unknown358 = NONE;
	biped->unknown35c = NONE;
	biped->unknown360 = NONE;
	biped->unknown364 = NONE;
	biped->unknown350 = NONE;
}

/* a point on a biped: its eye (mode 0 at its origin, 1 standing, 2
   crouching, raised to its head marker unless something is in the way), or
   an offset from a point along a forward vector (mode 3) */
// @retail 0xdef60
void __stdcall function_def60(point3f *point, long arg_159e6d, short mode, point3f const *origin,
	vector3f const *forward, real const *offsets)
{
	real crouch;

	if (mode)
	{
		*point = *origin;
		if (mode == 3)
		{
			point->x += forward->i * offsets[0];
			point->y += forward->j * offsets[0];
			point->z += forward->k * offsets[0];
			point->x += offsets[1] * (0.0f - forward->j);
			point->y += offsets[1] * forward->i;
			point->z += offsets[1] * 0.0f;
			point->z = offsets[2] + point->z;
			return;
		}
	}
	else
	{
		function_b9dd0(arg_159e6d, point);
	}
	switch (mode)
	{
	case 1:
		crouch = 0.0f;
		break;
	case 2:
		crouch = 1.0f;
		break;
	default:
		crouch = function_e0dc0(arg_159e6d) ? 0.0f : BIPED_GET(arg_159e6d)->crouch;
		break;
	}
	biped_eye_raise(arg_159e6d, crouch, point);
}

/* a biped's collision capsule: its bottom point, the vector to its top and
   its radius, from its two capsule nodes when its definition names them */
// @retail 0xdf6c0
void __stdcall function_df6c0(point3f *bottom, vector3f *axis, long arg_159e6d, real *radius)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);
	short node_a = *(short *)(definition + 0x254);
	short node_b = *(short *)(definition + 0x256);

	if (node_a != NONE && node_b != NONE)
	{
		transform4x3f const *nodes = (transform4x3f const *)((byte *)biped + biped->node_matrices_offset);
		point3f const *a = &nodes[node_a].position;
		point3f const *b = &nodes[node_b].position;

		if (TEST_FIELD_BIT((*(dword *)(definition + 0x264) >> 1) & 1))
		{
			bottom->x = (a->x + b->x) * 0.5f;
			bottom->y = (a->y + b->y) * 0.5f;
			bottom->z = (a->z + b->z) * 0.5f;
			*axis = *g_4687a4;
		}
		else
		{
			*bottom = *a;
			axis->i = b->x - a->x;
			axis->j = b->y - a->y;
			axis->k = b->z - a->z;
		}
		*radius = *(real *)(definition + 0x23c);
	}
	else
	{
		real height;
		real capsule_radius;
		real half;

		function_df5f0(arg_159e6d, bottom, &height, &capsule_radius);
		half = height * 0.5f;
		bottom->z += half;
		axis->i = g_4687b0->i * half;
		axis->j = g_4687b0->j * half;
		axis->k = g_4687b0->k * half;
		*radius = *(real *)(definition + 0x23c);
	}
}

/* the collision result as the biped's ground probe reads it */
struct s_biped_ground_collision
{
	long type;
	real t;
	point3f point;
	byte unknown14[0x24 - 0x14];
	short unknown24;
	byte unknown26[2];
	vector3f normal;
	byte unknown34[0x3c - 0x34];
	long unknown3c;
	long unknown40;
	byte unknown44[4];
	long unknown48;
	byte unknown4c[4];
	long unknown50;
	byte unknown54[0x5c - 0x54];
};

bool collision_test_vector_object(dword flags, s_collision_result_1697c0 *collision, long object_index,
	point3f const *point, vector3f const *vector);
bool function_1696d0(long flags, s_biped_ground_collision *collision, long object_index, point3f const *point,
	vector3f const *vector, long a, long b);

/* probes along a direction from the biped (or a point) for ground: against
   one object, against another, or against the world */
// @retail 0xdf880
bool __stdcall function_df880(point3f const *point, vector3f const *direction, long object_index,
	long arg_159e6d, long other_index, real length, point3f *hit_point, vector3f *normal, long *a, long *b,
	long *c, long *d)
{
	s_biped_ground_collision collision;
	point3f start;
	vector3f vector;
	bool hit;

	if (point)
	{
		start = *point;
	}
	else
	{
		function_b9dd0(arg_159e6d, &start);
	}
	start.x -= direction->i * 0.4f;
	start.y -= direction->j * 0.4f;
	start.z -= direction->k * 0.4f;
	vector.i = direction->i * length;
	vector.j = direction->j * length;
	vector.k = direction->k * length;
	collision.unknown24 = NONE;
	if (object_index != NONE &&
		collision_test_vector_object(0x84000d, (s_collision_result_1697c0 *)&collision, object_index, &start, &vector))
	{
		hit = true;
	}
	else if (other_index != NONE && function_1696d0(0x84000d, &collision, other_index, &start, &vector, NONE, NONE))
	{
		hit = true;
	}
	else
	{
		hit = function_1697c0(function_ddf60(arg_159e6d) ? 0x800001 : 0x84000d, &start, &vector, NONE, NONE,
			(s_collision_result_1697c0 *)&collision);
	}
	if (hit)
	{
		if (a)
		{
			*a = collision.unknown50;
		}
		if (c)
		{
			*c = collision.unknown40;
		}
		if (d)
		{
			*d = collision.unknown48;
		}
		if (hit_point)
		{
			*hit_point = collision.point;
		}
		if (normal)
		{
			*normal = collision.normal;
		}
		if (b)
		{
			*b = collision.unknown3c;
		}
	}
	return hit;
}

bool function_1f55e0(long actor_index, point3f *point, vector3f *direction);
extern vector3f *g_4687bc;

/* probes for the ground under the biped: down from its last ground point,
   back along its facing while it swims (mode 5), into the surface it
   clings to, or where its actor says */
// @retail 0xdfba0
void __stdcall function_dfba0(long arg_159e6d, long other_index, long object_index, point3f *point, long *a,
	long *b, long *c, long *d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	point3f start = biped->unknown368;
	vector3f direction;

	if (biped->actor_index == NONE || !function_1f55e0(biped->actor_index, &start, &direction))
	{
		if (biped->physics_mode == 5)
		{
			direction.i = biped->forward.i * -1.0f;
			direction.j = biped->forward.j * -1.0f;
			direction.k = biped->forward.k * -1.0f;
		}
		else
		{
			if (TEST_FIELD_BIT((biped->flags_348 >> 12) & 1))
			{
				direction.i = biped->unknown374.i * -1.0f;
				direction.j = biped->unknown374.j * -1.0f;
				direction.k = biped->unknown374.k * -1.0f;
				start = biped->unknown380;
			}
			else
			{
				direction = *g_4687bc;
			}
			start.x += g_4687b0->i * 0.05f;
			start.y += g_4687b0->j * 0.05f;
			start.z += g_4687b0->k * 0.05f;
		}
	}
	if (!function_df880(&start, &direction, object_index, arg_159e6d, other_index, 2.0f, point, NULL, a, b, c, d))
	{
		*a = NONE;
		*c = NONE;
		*d = NONE;
		*b = NONE;
	}
}

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
long function_1fa3a0(long a, long b, long object_index, long c, point3f const *point);

/* the ground the biped stands on: its surface, its location, the point and
   the object (with that object's surface) under it, probed again at most
   once a tick */
// @retail 0xdfdb0
void function_dfdb0(long arg_159e6d, long *surface, long *location, point3f *point, long *object,
	long *object_surface)
{
	s_biped *biped = BIPED_GET(arg_159e6d);

	if (biped->physics_mode == 2)
	{
		biped->unknown354 = NONE;
		biped->unknown358 = NONE;
		biped->unknown35c = NONE;
		biped->unknown360 = NONE;
		biped->unknown364 = NONE;
		function_b9dd0(arg_159e6d, point);
	}
	else
	{
		if (biped->unknown354 != NONE && !function_ddf60(arg_159e6d))
		{
			if (biped->unknown360 != NONE && !function_badc0(biped->unknown360, NONE))
			{
				function_dfd70(arg_159e6d);
			}
		}
		else if (g_510c54->game_time > biped->unknown350)
		{
			point3f ground = biped->unknown368;

			biped->unknown350 = g_510c54->game_time;
			if (biped->unknown360 != NONE && !function_badc0(biped->unknown360, NONE))
			{
				biped->unknown360 = NONE;
			}
			function_dfba0(arg_159e6d, biped->unknown360, biped->unknown35c, &ground, &biped->unknown354,
				&biped->unknown35c, &biped->unknown360, &biped->unknown364);
			biped->unknown358 = NONE;
			if (biped->unknown354 != NONE)
			{
				biped->unknown368 = ground;
			}
		}
		if (biped->unknown358 == NONE)
		{
			if (biped->unknown354 != NONE)
			{
				biped->unknown358 = function_1fa3a0(biped->unknown35c, biped->unknown354, biped->unknown360, biped->unknown364,
					&biped->unknown368);
			}
			else
			{
				biped->unknown358 = NONE;
			}
		}
	}
	if (point)
	{
		*point = biped->unknown368;
	}
	if (surface)
	{
		*surface = biped->unknown354;
	}
	if (object)
	{
		*object = biped->unknown360;
	}
	if (object_surface)
	{
		*object_surface = biped->unknown364;
	}
	if (location)
	{
		*location = biped->unknown358;
	}
}

bool __stdcall function_c5460(long object_index, long ignore_index, point3f const *position, point3f *result,
	long a5, real radius, long a7);

/* drops the biped out of a vehicle: level and upright, at a clear spot
   by the vehicle (or anywhere clear) */
// @retail 0xe0070
void function_e0070(long arg_159e6d, long vehicle_index)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	long root_index = function_baf40(vehicle_index);
	long attempt;

	if (root_index != NONE && ((1 << ((byte *)&((s_biped_header *)g_4e0300->data)[root_index & 0xffff])[3]) & 2))
	{
		vehicle_index = root_index;
	}
	biped->forward.k = 0.0f;
	if (function_30bf0(&biped->forward) == 0.0f)
	{
		biped->forward = *g_4687a8;
	}
	biped->up = *g_4687b0;
	function_e57e0(arg_159e6d);
	for (attempt = vehicle_index == NONE; attempt < 2; attempt++)
	{
		long ignore = attempt == 0 ? vehicle_index : NONE;
		point3f center;
		real height;
		real radius;
		s_biped *vehicle;
		point3f point;

		function_df5f0(arg_159e6d, &center, &height, &radius);
		if (function_c5460(arg_159e6d, ignore, NULL, 0, 0, radius * 2.0f, 1))
		{
			break;
		}
		vehicle = BIPED_GET(vehicle_index);
		point = *(point3f *)((byte *)vehicle + 0x30);
		if (function_c5460(arg_159e6d, ignore, &point, 0, 0, *(real *)((byte *)vehicle + 0x3c), 1))
		{
			break;
		}
	}
}

bool function_1d5120(s_havok_component *component, long rigid_body_index, long a, long b, real rate, long *result,
	long value);
void __stdcall function_1c3770(long object_index, dword flags);

/* steps a dead biped's ragdoll settling (+0x3aa) toward rest */
// @retail 0xe0c70
void function_e0c70(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);

	if (biped->unknown3aa && biped->havok_component_index != NONE)
	{
		s_havok_component *component = havok_component_get(biped->havok_component_index);
		char body = *((char *)component + 0x18);
		short rigid_body = body >= 0 && body < component->rigid_bodies.size ? body : NONE;
		long settled;

		if (rigid_body != NONE &&
			function_1d5120(component, rigid_body, 9, *(long *)(definition + 0x290) + 0x20, g_510c54->rate * 0.15f,
				&settled, (long)(char)biped->unknown3aa))
		{
			biped->unknown3aa = settled;
			*((byte *)&biped->flags_348 + 1) &= 0x9f;
			function_1c3770(arg_159e6d, 0);
			*((byte *)&biped->flags_348 + 1) &= 0x9f;
			return;
		}
	}
	biped->flags_134 |= 0x800000;
	*((byte *)&biped->flags_348 + 1) &= 0x9f;
}

/* how long the dead biped's ragdoll has been settling */
// @retail 0xe0d70
bool __stdcall function_e0d70(long arg_159e6d, real *seconds)
{
	s_biped *biped = BIPED_GET(arg_159e6d);

	if (biped->unknown3aa)
	{
		*seconds = g_510c54->rate * 0.15f * (real)(char)biped->unknown3aa;
		return true;
	}
	return false;
}

real function_1ec5f0(void *ragdoll);
void function_1d35d0(long rigid_body_index, s_havok_component *component, real scale);

/* scales a dead biped's ragdoll bodies by how far it has settled, a little
   differently for each biped */
// @retail 0xe0e00
void function_e0e00(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);

	if (biped->physics_mode == 3 && biped->havok_component_index != NONE)
	{
		s_havok_component *component = havok_component_get(biped->havok_component_index);

		if (TEST_FIELD_BIT((component->unknown04 >> 11) & 1))
		{
			real scales[4];
			real scale;
			long i;

			scales[0] = 2.0f;
			scales[1] = 1.0f;
			scales[2] = 1.333f;
			scales[3] = 1.666f;
			scale = (1.0f - function_1ec5f0((byte *)biped + 0x3ec)) * scales[arg_159e6d & 3] + 1.0f;
			for (i = 0; i < *(long *)((byte *)component + 0x80); i++)
			{
				function_1d35d0(i, component, scale);
			}
		}
	}
}

real function_1ec640(void *ragdoll);

/* how fast a dead biped's ragdoll falls, while it has fallen over */
// @retail 0xe1670
real function_e1670(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	real result = 0.0f;

	if (biped->unknown34b == 1 && biped->physics_mode == 3)
	{
		result = function_1ec640((byte *)biped + 0x3ec) * -0.125f;
	}
	return result;
}

real function_1d1230(long rigid_body_index, s_havok_component *component);
bool __stdcall function_e1b60(long arg_159e6d, point3f const *point, real value);

/* starts a fallen biped's ragdoll from its main rigid body */
// @retail 0xe1a80
void function_e1a80(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);

	if (biped->unknown34b == 1 && biped->havok_component_index != NONE)
	{
		s_havok_component *component = havok_component_get(biped->havok_component_index);
		char body = *((char *)component + 0x18);
		short rigid_body = body >= 0 && body < component->rigid_bodies.size ? body : NONE;

		if ((TEST_FIELD_BIT((component->unknown04 >> 11) & 1)) && rigid_body != NONE)
		{
			real value = function_1d1230(rigid_body, component);
			byte *motion = *(byte **)(*(byte **)((byte *)component->rigid_bodies.data + rigid_body * 0x60 + 0x40) +
				0x3c);
			point3f point = *(point3f *)(motion + 0x70);

			function_e1b60(arg_159e6d, &point, value);
		}
	}
}

struct s_list_rigid_bodies
{
	long count;
	char lists[256];
	char unlisted;
};

s_list_rigid_bodies *function_181bd0(long object_index, s_list_rigid_bodies *rigid_bodies);
bool __stdcall function_e1c40(long arg_159e6d, point3f const *point, real value, long rigid_body_index, long a,
	long b, bool c);
bool __stdcall function_e23e0(long arg_159e6d, point3f const *point, real value, bool *result, dword *visited,
	long rigid_body_index, bool started);

/* places the biped's ragdoll bodies, from its root rigid body outward */
// @retail 0xe1b60
bool __stdcall function_e1b60(long arg_159e6d, point3f const *point, real value)
{
	s_list_rigid_bodies list;
	long count;
	bool result = false;

	function_181bd0(arg_159e6d, &list);
	count = havok_component_get(BIPED_GET(arg_159e6d)->havok_component_index)->rigid_bodies.size;
	if (PIN(count, 1, 32) == count && list.count > 0)
	{
		long root = list.lists[0];

		if (root >= 0 && root <= count - 1)
		{
			dword visited[8];
			bool started;

			memset(visited, 0, sizeof(visited));
			started = function_e1c40(arg_159e6d, point, value, root, NONE, NONE, 0);
			visited[root >> 5] |= 1 << (root & 0x1f);
			return function_e23e0(arg_159e6d, point, value, &result, visited, root, started);
		}
	}
	return result;
}

void function_b8840(long unit_index);
void function_1d1540(s_havok_component *component);

/* turns the biped into a ragdoll: lifted a little when it fell through
   something, its nodes placed from its Havok bodies */
// @retail 0xe16b0
bool __stdcall function_e16b0(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);
	byte *variant = *(byte **)(definition + 0x288) + *((byte *)biped + 0x3ff) * 0x80;
	s_havok_component *component = havok_component_get(biped->havok_component_index);
	real value = *(real *)(variant + 0x2c);
	bool asleep = (biped->flags_c0 >> 6) & 1;
	bool result = false;
	point3f point;

	if (asleep)
	{
		function_b8840(arg_159e6d);
	}
	function_b9dd0(arg_159e6d, &point);
	point.x += *(real *)(variant + 0x70);
	point.y += *(real *)(variant + 0x74);
	point.z += *(real *)(variant + 0x78);
	if ((biped->flags_348 >> 9) & 1)
	{
		s_biped *object = BIPED_GET(arg_159e6d);
		dword node_count = (word)*(short *)((byte *)object + 0x114) / 0x34;
		transform4x3f *nodes = (transform4x3f *)((byte *)object + object->node_matrices_offset);

		*(real *)((byte *)biped + 0x6c) += 0.125f;
		for (; (long)node_count > 0; node_count--, nodes++)
		{
			nodes->position.z += 0.125f;
		}
	}
	function_1c3770(arg_159e6d, 0);
	if ((component->unknown04 >> 11) & 1)
	{
		result = function_e1b60(arg_159e6d, &point, value);
	}
	if (asleep)
	{
		s_biped *current = BIPED_GET(arg_159e6d);

		if (current->havok_component_index != NONE)
		{
			function_1d1540(havok_component_get(current->havok_component_index));
		}
		*(byte *)&current->flags_c0 |= 0x40;
	}
	if (result)
	{
		if (biped->physics_mode == 3)
		{
			*(long *)((byte *)biped + 0x448) = g_510c54->game_time;
			function_e0e00(arg_159e6d);
		}
		if (arg_159e6d != NONE && *(short *)((byte *)BIPED_GET(arg_159e6d) + 0x112) != NONE)
		{
			BIPED_GET(arg_159e6d)->object_flags |= 0x20000000;
		}
	}
	return result;
}

void function_bfa40(long object_index, long a);
void function_ba350(long object_index, real seconds);
bool __stdcall function_10f430(long unit_index, long field_7c, long state_name, long weapon_name, long action_name,
	real blend, bool flags, long mode);

/* gets a fallen biped up from its ragdoll: back to its getting-up
   animation, Havok asleep, at a clear spot facing along its slide */
// @retail 0xe18d0
void __stdcall function_e18d0(long arg_159e6d, bool place)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	bool fallen = biped->unknown34b == 1;

	if ((biped->object_flags >> 29) & 1)
	{
		real blend = 0.267f;

		if (arg_159e6d != NONE && *(short *)((byte *)biped + 0x112) != NONE)
		{
			if ((biped->object_flags >> 29) & 1)
			{
				function_bfa40(arg_159e6d, 0);
			}
			biped->object_flags &= ~0x20000000;
		}
		function_ba350(arg_159e6d, blend);
		function_10f430(arg_159e6d, 0x7000101, 0x7000101, 0x7000101, 0xd000042, 0.0f, false, 0x210);
	}
	biped->unknown34b = 0;
	*((byte *)&biped->flags_348 + 1) &= 0xf8;
	function_b9b90(arg_159e6d, false);
	function_1c3770(arg_159e6d, 0);
	if (place && biped->unknown34b)
	{
		point3f center;
		real height;
		real radius;
		vector3f velocity;

		function_df5f0(arg_159e6d, &center, &height, &radius);
		function_c5460(arg_159e6d, NONE, NULL, 0, 0, radius * 2.0f, 0);
		function_ba1d0(arg_159e6d, &velocity, NULL);
		if (velocity.k * velocity.k + velocity.i * velocity.i + velocity.j * velocity.j > 1.4f)
		{
			vector3f facing;

			facing.i = velocity.i;
			facing.j = velocity.j;
			facing.k = 0.0f;
			if (function_30bf0(&facing) != 0.0f)
			{
				function_b75a0(arg_159e6d, NULL, &facing, g_4687b0, NULL, false);
			}
		}
	}
	if (fallen)
	{
		(*(long *)((byte *)g_4e6948 + 0x1130))--;
	}
}

/* the script flags at 0x5107ec (ragdolls stay asleep) and 0x5107ed (no
   ragdolls); 0x5107ee is in unknown_29f5b0.cpp */
byte g_5107ec;
byte g_5107ed;

struct s_small_index;
bool function_1ecef0(void *ragdoll);
bool function_138fa0(long type);
long function_10f720(long object_index, bool first);
bool function_1ed430(void *ragdoll, vector3f *direction, long *value);
long function_183c60(long object_index, long maximum_value);
real havok_component_rigid_body_mass_get(long rigid_body_index, s_havok_component *component);
void havok_component_rigid_body_linear_velocity_change(long rigid_body_index, s_havok_component *component,
	vector3f const *change);
short function_0b67a0(const s_small_index *data);
void __stdcall function_b77d0(long object_index, vector3f const *linear_velocity,
	vector3f const *angular_velocity);

/* the dead biped's ragdoll: when it falls over (+0x34b 0 to 1) its bodies
   get a push along the killing blow, spread from the hit body; a fallen
   one settles and goes to sleep (2); one that gets up again stands */
// @retail 0xe0ef0
bool __stdcall function_e0ef0(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	bool result = false;

	switch (biped->unknown34b)
	{
	case 0:
	{
		byte *ragdoll = (byte *)biped + 0x3ec;
		byte *definition;
		byte *model;
		bool held;

		if (!(ragdoll[0x11] && !ragdoll[0x12] && (!function_e4050(arg_159e6d) || (*((byte *)biped + 0xc1) & 1))) &&
			!function_1ecef0(ragdoll))
		{
			return false;
		}
		definition = BIPED_DEFINITION_GET(biped);
		model = g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes;
		if (*(long *)(model + 0x24) == NONE || ((biped->flags_348 >> 10) & 1))
		{
			return false;
		}
		result = true;
		if (g_5107ed)
		{
			*((byte *)&biped->flags_348 + 1) |= 1;
			return true;
		}
		if ((*(dword *)(definition + 0x1f0) >> 11) & 1)
		{
			return true;
		}
		held = ((biped->flags_134 >> 27) & 1) && !((biped->flags_348 >> 9) & 1);
		if (!function_138fa0(!held))
		{
			*((byte *)&biped->flags_348 + 1) |= 2;
			return true;
		}
		{
			long primary = function_10f720(arg_159e6d, true);
			long secondary = function_10f720(arg_159e6d, false);

			if (!((biped->flags_134 >> 27) & 1) && (primary == 2 || secondary == 2))
			{
				return true;
			}
		}
		biped->unknown34b = 1;
		function_b9b90(arg_159e6d, false);
		if (g_5107ec)
		{
			biped->flags_c0 |= 0x100;
		}
		if (!function_e16b0(arg_159e6d))
		{
			biped->unknown34b = 0;
			biped->flags_348 |= 0x100;
			return true;
		}
		(*(long *)((byte *)g_4e6948 + 0x1130))++;
		if (biped->havok_component_index != NONE)
		{
			s_havok_component *component = havok_component_get(biped->havok_component_index);

			if ((component->unknown04 >> 11) & 1)
			{
				vector3f direction;
				long value;
				long hit_body;
				bool pushed = false;

				if (function_1ed430(ragdoll, &direction, &value) &&
					(hit_body = function_183c60(arg_159e6d, value)) != NONE)
				{
					real strength = function_259d0(&g_4e7408->unknown0, NULL, 0, 1.5f, 3.25f);
					real lift = function_259d0(&g_4e7408->unknown0, NULL, 0, 0.75f, 1.5f);
					point3f center = *g_468788;
					point3f hit;
					vector3f along;
					real total = 0.0f;
					long count = component->rigid_bodies.size;
					long i;

					hit = *(point3f *)(*(byte **)(*(byte **)((byte *)component->rigid_bodies.data + hit_body * 0x60 +
						0x40) + 0x3c) + 0x70);
					for (i = 0; i < count; i++)
					{
						point3f const *position = (point3f const *)(*(byte **)(*(byte **)(
							(byte *)component->rigid_bodies.data + i * 0x60 + 0x40) + 0x3c) + 0x70);
						real mass = (real)(long)havok_component_rigid_body_mass_get(i, component);

						center.x += position->x * mass;
						center.y += position->y * mass;
						center.z += position->z * mass;
						total += mass;
					}
					center.x *= 1.0f / total;
					center.y *= 1.0f / total;
					center.z *= 1.0f / total;
					along.i = hit.x - center.x;
					along.j = hit.y - center.y;
					along.k = hit.z - center.z;
					if (function_30bf0(&along) > 0.001f)
					{
						real dot = along.i * direction.i + along.k * direction.k + along.j * direction.j;

						direction.i -= along.i * dot;
						direction.j -= along.j * dot;
						direction.k -= along.k * dot;
						if (function_30bf0(&direction) != 0.0f)
						{
							real push = *(real *)(definition + 0x258) > 0.7f ? 0.7f : *(real *)(definition + 0x258);
							real farthest = 0.0f;

							push *= strength;
							direction.i *= push;
							direction.j *= push;
							direction.k *= push;
							for (i = 0; i < count; i++)
							{
								point3f const *position = (point3f const *)(*(byte **)(*(byte **)(
									(byte *)component->rigid_bodies.data + i * 0x60 + 0x40) + 0x3c) + 0x70);
								real distance = (position->x - center.x) * along.i + (position->z - center.z) * along.k +
									(position->y - center.y) * along.j;

								if (!(farthest > distance))
								{
									farthest = distance;
								}
							}
							for (i = 0; i < count; i++)
							{
								point3f const *position = (point3f const *)(*(byte **)(*(byte **)(
									(byte *)component->rigid_bodies.data + i * 0x60 + 0x40) + 0x3c) + 0x70);
								real distance = (position->x - center.x) * along.i + (position->z - center.z) * along.k +
									(position->y - center.y) * along.j;
								real scale = (0.0f > distance ? 0.65f : 1.0f) * (1.0f / farthest) * distance;
								vector3f change;

								if (i == hit_body)
								{
									scale *= 2.0f;
								}
								change.i = direction.i * scale;
								change.j = direction.j * scale;
								change.k = direction.k * scale + lift;
								havok_component_rigid_body_linear_velocity_change(i, component, &change);
							}
							pushed = true;
						}
					}
				}
				if (!pushed && component->rigid_bodies.size > 0)
				{
					vector3f change;

					change.i = g_4687bc->i * 2.0f;
					change.j = g_4687bc->j * 2.0f;
					change.k = g_4687bc->k * 2.0f;
					havok_component_rigid_body_linear_velocity_change(
						function_0b67a0((s_small_index const *)component), component, &change);
				}
			}
		}
		*((byte *)&biped->flags_348 + 1) &= ~1;
		return result;
	}
	case 1:
		if (!g_5107ec && (*((byte *)biped + 0xc1) & 1))
		{
			if (arg_159e6d != NONE && *(short *)((byte *)biped + 0x112) != NONE)
			{
				biped->object_flags |= 0x20000000;
			}
			biped->unknown34b = 2;
			function_b77d0(arg_159e6d, g_4687a4, NULL);
			function_1c3770(arg_159e6d, 0);
			(*(long *)((byte *)g_4e6948 + 0x1130))--;
			return true;
		}
		if ((g_510c54->game_time + (arg_159e6d & 0xffff)) % vehicle_round_ticks(g_510c54->field_2_3 * 0.3f) == 0)
		{
			function_e1a80(arg_159e6d);
		}
		function_e0e00(arg_159e6d);
		return true;
	default:
		if (!(*((byte *)biped + 0xc1) & 1))
		{
			function_e18d0(arg_159e6d, true);
		}
		return false;
	}
}

struct rigid_transform_scaled;
struct real_quaternion_transform;
bool function_1faf80(vector3f *facing, long arg_159e6d, void const *definition_flight, vector3f const *control,
	real rate, real *turn);
bool function_bf5d0(long object_index);
void matrix4x3_rotation_between_vectors(transform4x3f *matrix, vector3f const *arg_5f338b,
	vector3f const *arg_bc44c6);
void orientation_from_matrix4x3(transform4x3f const *matrix, rigid_transform_scaled *out);
void function_11dbb0(real_quaternion_transform *out, real_quaternion_transform const *a,
	real_quaternion_transform const *b);
bool function_e63b0(vector3f const *aim, bool flag);
bool function_1012c0(long weapon_index);
long function_cbd50(long unit_index, short weapon_index);
real normalize2d(point2f *v);

/* turns a vector about an axis by an angle (the axis perpendicular to it) */
PRIVATE inline void biped_rotate(vector3f *v, vector3f const *axis, real sine, real cosine)
{
	vector3f cross;

	cross.i = axis->j * v->k - axis->k * v->j;
	cross.j = axis->k * v->i - axis->i * v->k;
	cross.k = axis->i * v->j - axis->j * v->i;
	v->i = v->i * cosine + cross.i * sine;
	v->j = v->j * cosine + cross.j * sine;
	v->k = v->k * cosine + cross.k * sine;
}

/* turns the biped toward its desired facing: flying ones by their flight
   controls, walking ones at the definition's turn rate (or by a turning
   animation when they stand), and a grabbing one toward its target */
// @retail 0xe01d0
bool __stdcall function_e01d0(long arg_159e6d, long *names)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);
	byte *state = (byte *)biped + *(short *)((byte *)biped + 0x12a);
	bool changed = false;

	if (biped->physics_mode != 3 && biped->physics_mode != 6)
	{
		changed = true;
		if (biped->physics_mode == 2)
		{
			if (*(long *)((byte *)biped + 0x3ec) <= 0)
			{
				real rate = (biped->control_flags & 8) ? 0.99f : *(real *)(definition + 0x24c);

				function_1faf80(&biped->facing_goal, arg_159e6d, definition + 0x2cc, &biped->control, rate,
					&biped->unknown3a0);
			}
			else
			{
				biped->unknown3a0 = 0.0f;
			}
		}
		else if (biped->physics_mode != 5 && *(long *)(state + 0x70) != 0x6000084)
		{
			bool special = *(long *)(state + 0x70) == 0x7000039;
			bool wall = TEST_FIELD_BIT((*(dword *)(definition + 0x264) >> 3) & 1);
			vector3f facing;
			real sine;
			real cosine;
			bool left;

			if (wall)
			{
				vector3f side;

				side.i = biped->up.j * biped->facing_goal.k - biped->up.k * biped->facing_goal.j;
				side.j = biped->up.k * biped->facing_goal.i - biped->up.i * biped->facing_goal.k;
				side.k = biped->up.i * biped->facing_goal.j - biped->up.j * biped->facing_goal.i;
				facing.i = side.j * biped->up.k - side.k * biped->up.j;
				facing.j = side.k * biped->up.i - side.i * biped->up.k;
				facing.k = side.i * biped->up.j - side.j * biped->up.i;
				if (function_30bf0(&facing) == 0.0f)
				{
					facing = biped->forward;
				}
				sine = (biped->forward.j * facing.k - biped->forward.k * facing.j) * biped->up.i +
					(biped->forward.k * facing.i - biped->forward.i * facing.k) * biped->up.j +
					(biped->forward.i * facing.j - biped->forward.j * facing.i) * biped->up.k;
				cosine = biped->forward.j * facing.j + biped->forward.k * facing.k + biped->forward.i * facing.i;
			}
			else
			{
				point2f flat;

				flat.x = biped->facing_goal.i;
				flat.y = biped->facing_goal.j;
				if (normalize2d(&flat) == 0.0f)
				{
					flat.x = biped->forward.i;
					flat.y = biped->forward.j;
				}
				facing.i = flat.x;
				facing.j = flat.y;
				facing.k = 0.0f;
				sine = biped->forward.i * flat.y - biped->forward.j * flat.x;
				cosine = biped->forward.j * flat.y + biped->forward.i * flat.x;
			}
			left = sine > 0.0f;
			if (-0.9f > cosine)
			{
				if (*(long *)(state + 0x7c) == 0xa00000f)
				{
					left = true;
				}
				else if (*(long *)(state + 0x7c) == 0x900000e)
				{
					left = false;
				}
			}
			if (biped->unknown34a == 1 || (*(dword *)(definition + 0x1f0) & 1))
			{
				real angle = g_510c54->rate * *(real *)(definition + 0x1ec);
				real turn_cosine = (real)cos(angle);
				real turn_sine = (real)sin(angle);
				vector3f turned;
				real overshoot;

				if (left)
				{
					turn_sine = 0.0f - turn_sine;
				}
				if (wall)
				{
					turned = biped->forward;
					biped_rotate(&turned, &biped->up, 0.0f - turn_sine, turn_cosine);
					overshoot = (turned.j * facing.k - turned.k * facing.j) * biped->up.i +
						(turned.k * facing.i - turned.i * facing.k) * biped->up.j +
						(turned.i * facing.j - turned.j * facing.i) * biped->up.k;
				}
				else
				{
					turned.i = biped->forward.i * turn_cosine + biped->forward.j * turn_sine;
					turned.j = biped->forward.j * turn_cosine - biped->forward.i * turn_sine;
					turned.k = biped->forward.k;
					overshoot = turned.i * facing.j - turned.j * facing.i;
				}
				if (left ? 0.0f > overshoot : overshoot > 0.0f)
				{
					if (wall)
					{
						vector3f side;

						side.i = biped->up.j * facing.k - biped->up.k * facing.j;
						side.j = biped->up.k * facing.i - biped->up.i * facing.k;
						side.k = biped->up.i * facing.j - biped->up.j * facing.i;
						if (function_30bf0(&side) > 0.0f)
						{
							turned.i = side.j * biped->up.k - side.k * biped->up.j;
							turned.j = side.k * biped->up.i - side.i * biped->up.k;
							turned.k = side.i * biped->up.j - side.j * biped->up.i;
						}
					}
					else
					{
						biped->up = *g_4687b0;
						turned = facing;
					}
					function_30bf0(&turned);
				}
				if (biped->unknown13c != NONE && function_bf5d0(arg_159e6d) &&
					0.98f > biped->forward.j * turned.j + biped->forward.i * turned.i)
				{
					transform4x3f rotation;
					byte orientation[0x20];
					long count = *(short *)((byte *)biped + 0x10c) >> 5;

					matrix4x3_rotation_between_vectors(&rotation, &biped->forward, &turned);
					orientation_from_matrix4x3(&rotation, (rigid_transform_scaled *)orientation);
					if (count > 0)
					{
						real_quaternion_transform *a = (real_quaternion_transform *)((byte *)biped +
							*(short *)((byte *)biped + 0x112));
						real_quaternion_transform *b = (real_quaternion_transform *)((byte *)biped +
							*(short *)((byte *)biped + 0x10e));

						function_11dbb0(a, (real_quaternion_transform *)orientation, a);
						function_11dbb0(b, (real_quaternion_transform *)orientation, b);
					}
				}
				biped->forward = turned;
			}
			else if (!biped->unknown34a && !special && !(TEST_FIELD_BIT((biped->flags_134 >> 11) & 1)))
			{
				real limit = (biped->control_flags & 8) ? 0.99f : *(real *)(definition + 0x24c) + 0.0001f;

				if (limit >= cosine && !(TEST_FIELD_BIT((*(dword *)(definition + 0xbc) >> 20) & 1)))
				{
					names[1] = left ? 0xa00000f : 0x900000e;
				}
			}
		}
	}
	if (biped->unknown390 != NONE && biped->physics_mode == 6)
	{
		real limit = g_510c54->rate * 9.424778f;
		vector3f aim;
		char weapon = *((char *)BIPED_GET(arg_159e6d) + 0x212);
		long weapon_index = weapon != NONE ? *(long *)((byte *)BIPED_GET(arg_159e6d) + 0x218 + weapon * 4) : NONE;

		function_e5d50(arg_159e6d, biped->unknown390, &biped->unknown444, (point3f *)&aim);
		if (function_e63b0(&aim, function_1012c0(weapon_index)))
		{
			vector3f look;

			function_e5d50(arg_159e6d, biped->unknown390, &biped->unknown168, (point3f *)&look);
			if (function_e63b0(&look, function_1012c0(function_cbd50(arg_159e6d,
				*((char *)BIPED_GET(arg_159e6d) + 0x212)))))
			{
				real yaw = PIN(look.i, 0.0f - limit, limit);
				vector3f axis;

				function_11d090(&biped->unknown168, &axis);
				biped_rotate(&biped->unknown168, &axis, (real)sin(yaw), (real)cos(yaw));
				if (biped->unknown3f8)
				{
					real pitch = PIN(look.j, 0.0f - limit, limit);
					real sine = (real)sin(pitch);
					real cosine = (real)cos(pitch);

					biped->unknown168.i = cosine * biped->unknown168.i + axis.i * sine;
					biped->unknown168.j = cosine * biped->unknown168.j + axis.j * sine;
					biped->unknown168.k = cosine * biped->unknown168.k + axis.k * sine;
				}
				*(vector3f *)((byte *)biped + 0x15c) = biped->unknown168;
				*(vector3f *)((byte *)biped + 0x18c) = biped->unknown168;
				*(vector3f *)((byte *)biped + 0x180) = biped->unknown168;
				biped->facing_goal = biped->unknown168;
				*(byte *)&biped->flags_348 |= 0x20;
			}
		}
		return true;
	}
	return changed;
}

struct s_vehicle_ray;
bool __stdcall function_168f40(long flags, s_vehicle_ray const *ray, long ignore_object_index, long ignore_unit_index);
void havok_component_rigid_body_matrix_get(long rigid_body_index, s_havok_component *component, transform4x3f *matrix);
void havok_component_rigid_body_matrix_set(long rigid_body_index, s_havok_component *component,
	transform4x3f const *matrix);
void havok_component_contact_properties_get(s_havok_component const *component, long contact_index, long *property_a,
	long *property_b);
void function_1cff80(s_havok_component_element0c const *constraint, point3f *pivot_a, point3f *pivot_b);
matrix3x3 *function_142eb0(matrix3x3 const *a, matrix3x3 const *b, matrix3x3 *out);

/* a point in a matrix's frame, scaled */
PRIVATE inline void biped_matrix_transform(transform4x3f const *matrix, point3f const *point, point3f *result)
{
	point3f scaled = *point;

	if (matrix->scale != 1.0f)
	{
		scaled.x *= matrix->scale;
		scaled.y *= matrix->scale;
		scaled.z *= matrix->scale;
	}
	result->x = matrix->up.i * scaled.z + matrix->left.i * scaled.y + matrix->forward.i * scaled.x + matrix->position.x;
	result->y = matrix->up.j * scaled.z + matrix->left.j * scaled.y + matrix->forward.j * scaled.x + matrix->position.y;
	result->z = matrix->up.k * scaled.z + matrix->left.k * scaled.y + matrix->forward.k * scaled.x + matrix->position.z;
}

PRIVATE inline point3f *biped_rigid_body_position(s_havok_component *component, long rigid_body_index)
{
	return (point3f *)(*(byte **)(*(byte **)((byte *)component->rigid_bodies.data + rigid_body_index * 0x60 + 0x40) +
		0x3c) + 0x70);
}

/* places one of the ragdoll's bodies: hanging from its constraint's pivot on
   the body placed before it (kept within reach of the ragdoll's center and
   out of the world, turned to follow), or the first body at the point */
// @retail 0xe1c40
bool __stdcall function_e1c40(long arg_159e6d, point3f const *point, real value, long rigid_body_index,
	long contact_index, long parent_body_index, bool started)
{
	s_havok_component *component = havok_component_get(BIPED_GET(arg_159e6d)->havok_component_index);
	point3f origin = *biped_rigid_body_position(component, rigid_body_index);
	transform4x3f matrix;

	havok_component_rigid_body_matrix_get(rigid_body_index, component, &matrix);
	if (!started && !function_168f40(0x80040d, (s_vehicle_ray const *)&origin, arg_159e6d, NONE))
	{
		return false;
	}
	if (parent_body_index != NONE)
	{
		transform4x3f parent;
		point3f pivot_a;
		point3f pivot_b;
		point3f const *parent_pivot;
		point3f const *child_pivot;
		point3f world_pivot;
		point3f child_world;
		vector3f offset;
		point3f target;
		point3f placed;
		vector3f delta;
		long property_a;
		long property_b;
		real distance;
		real limit = value - 0.025f;

		havok_component_rigid_body_matrix_get(parent_body_index, component, &parent);
		function_1cff80(&component->unknown7c.data[contact_index], &pivot_a, &pivot_b);
		havok_component_contact_properties_get(component, contact_index, &property_a, &property_b);
		if (property_a == parent_body_index)
		{
			parent_pivot = &pivot_b;
			child_pivot = &pivot_a;
		}
		else
		{
			parent_pivot = &pivot_a;
			child_pivot = &pivot_b;
		}
		biped_matrix_transform(&parent, parent_pivot, &world_pivot);
		biped_matrix_transform(&matrix, child_pivot, &child_world);
		offset.i = world_pivot.x - point->x;
		offset.j = world_pivot.y - point->y;
		offset.k = world_pivot.z - point->z;
		distance = (real)sqrt(offset.k * offset.k + offset.j * offset.j + offset.i * offset.i);
		target = world_pivot;
		if (!(limit > distance))
		{
			s_biped_collision collision;

			if (function_1697c0(0x80040d, point, &offset, arg_159e6d, NONE, (s_collision_result_1697c0 *)&collision))
			{
				real t = limit > collision.t * distance ? collision.t : collision.t - 0.025f / distance;

				target.x = offset.i * t + point->x;
				target.y = offset.j * t + point->y;
				target.z = offset.k * t + point->z;
			}
		}
		placed.x = target.x - child_world.x + origin.x;
		placed.y = target.y - child_world.y + origin.y;
		placed.z = target.z - child_world.z + origin.z;
		delta.i = placed.x - target.x;
		delta.j = placed.y - target.y;
		delta.k = placed.z - target.z;
		if (delta.k * delta.k + delta.j * delta.j + delta.i * delta.i > 0.001f * 0.001f &&
			function_168f40(0x80040d, (s_vehicle_ray const *)&placed, arg_159e6d, NONE))
		{
			vector3f back;

			back.i = offset.i * -1.0f;
			back.j = offset.j * -1.0f;
			back.k = offset.k * -1.0f;
			if (function_30bf0(&back) != 0.0f && function_30bf0(&delta) != 0.0f)
			{
				transform4x3f rotation;

				matrix4x3_rotation_between_vectors(&rotation, &delta, &back);
				function_142eb0(&matrix.rotation, &rotation.rotation, &matrix.rotation);
				function_30bf0(&matrix.forward);
				function_30bf0(&matrix.left);
				function_30bf0(&matrix.up);
			}
		}
		biped_matrix_transform(&matrix, child_pivot, &child_world);
		matrix.position.x = target.x - (child_world.x - matrix.position.x);
		matrix.position.y = target.y - (child_world.y - matrix.position.y);
		matrix.position.z = target.z - (child_world.z - matrix.position.z);
	}
	else
	{
		real lowered = value - 0.025f;

		matrix.position.x = point->x - (origin.x - matrix.position.x);
		matrix.position.y = point->y - (origin.y - matrix.position.y);
		matrix.position.z = point->z - lowered - (origin.z - matrix.position.z);
	}
	havok_component_rigid_body_matrix_set(rigid_body_index, component, &matrix);
	return true;
}

/* places the ragdoll's bodies joined to one body, then theirs in turn */
// @retail 0xe23e0
bool __stdcall function_e23e0(long arg_159e6d, point3f const *point, real value, bool *result, dword *visited,
	long rigid_body_index, bool started)
{
	s_havok_component *component = havok_component_get(BIPED_GET(arg_159e6d)->havok_component_index);
	long i;

	for (i = 0; i < component->unknown7c.size; i++)
	{
		long a;
		long b;
		long other = NONE;

		havok_component_contact_properties_get(component, i, &a, &b);
		if (a == rigid_body_index)
		{
			other = b;
		}
		if (b == rigid_body_index)
		{
			other = a;
		}
		if (other != NONE && !(visited[other >> 5] & (1 << (other & 0x1f))))
		{
			bool placed = function_e1c40(arg_159e6d, point, value, other, i, rigid_body_index, started);

			visited[other >> 5] |= 1 << (other & 0x1f);
			function_e23e0(arg_159e6d, point, value, result, visited, other, placed);
		}
	}
	return true;
}

struct s_1faf30_timer;
void function_1faf30(s_1faf30_timer *timer);
bool function_1cb920(void *data, long mode);
bool function_114b60(short entry_index, short fallback_index, long unit_index, long priority, void const *extra);
void __stdcall function_b8890(long unit_index);

/* counts down the biped's landing (+0x39d); a ragdolling biped that can
   recover gets up when it reaches its last tick */
// @retail 0xe2d80
bool __stdcall function_e2d80(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);

	if (biped->unknown39d > 0 && biped->physics_mode == 3 && (TEST_FIELD_BIT((*(dword *)(definition + 0x1f0) >> 10) & 1)) &&
		!(TEST_FIELD_BIT((biped->flags_10a >> 2) & 1)) && biped->unknown39d == 1)
	{
		function_e5930(arg_159e6d);
		function_10f430(arg_159e6d, 0x7000001, 0x7000101, 0x7000101, 0x7000001, 0.0f, false, 2);
		if (arg_159e6d != NONE)
		{
			s_biped *current = BIPED_GET(arg_159e6d);

			if (*(short *)((byte *)current + 0x112) != NONE)
			{
				if (TEST_FIELD_BIT((current->object_flags >> 29) & 1))
				{
					function_bfa40(arg_159e6d, 0);
				}
				current->object_flags &= ~0x20000000;
			}
		}
		if (biped->physics_mode == 2)
		{
			function_1faf30((s_1faf30_timer *)((byte *)biped + 0x3ec));
		}
	}
	if (biped->unknown39d > 0)
	{
		biped->unknown39d--;
		return true;
	}
	return false;
}

/* the biped's movement animation names: hovering, swimming, and for an
   idle one the stepping name for its direction (1 to 11) */
// @retail 0xe2e90
void __stdcall function_e2e90(long arg_159e6d, long *field_7c, long *state_name, long direction, byte value,
	byte *out)
{
	s_biped *biped = BIPED_GET(arg_159e6d);

	if (biped->unknown34c != 1)
	{
		switch (biped->physics_mode)
		{
		case 2:
			if (function_1cb920((byte *)biped + *(short *)((byte *)biped + 0x12a), 0x6000542))
			{
				*field_7c = 0x6000542;
			}
			break;
		case 5:
			*field_7c = 0x50000cb;
			break;
		}
		if (*state_name == 0x400000c)
		{
			switch (direction)
			{
			case 1:
				*state_name = 0x400000c;
				break;
			case 2:
				*state_name = 0xa000017;
				break;
			case 3:
				*state_name = 0x9000016;
				break;
			case 4:
				*state_name = 0xa000014;
				break;
			case 5:
				*state_name = 0x9000015;
				break;
			case 6:
				*state_name = 0xd000035;
				break;
			case 7:
				*state_name = 0xc000034;
				break;
			case 8:
				*state_name = 0xd000032;
				break;
			case 9:
				*state_name = 0xc000033;
				break;
			case 10:
				*state_name = 0x800001c;
				break;
			case 11:
				*state_name = 0xa00001d;
				break;
			}
			if (direction)
			{
				*out = value;
			}
		}
	}
}

/* starts the biped's landing: its landing time, and for one that can
   collapse, a ragdoll for a random part of it */
// @retail 0xe3f00
void function_e3f00(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);
	long ticks = vehicle_round_ticks(g_510c54->field_2_3 * *(real *)(definition + 0x214));

	if (ticks > 0xff)
	{
		ticks = 0xff;
	}
	else
	{
		ticks = vehicle_round_ticks(g_510c54->field_2_3 * *(real *)(definition + 0x214));
	}
	biped->unknown39d = (byte)ticks;
	function_114b60(NONE, 0x14, arg_159e6d, 0xd, 0);
	if (biped->unknown39d > 1 && ((*(dword *)(definition + 0x1f0) >> 10) & 1) && biped->physics_mode != 3)
	{
		byte half = biped->unknown39d >> 1;

		if (half)
		{
			biped->unknown39d += (byte)((random_next(&g_4e7408->unknown0) * (short)half) >> 16);
		}
		function_b9b90(arg_159e6d, false);
		function_1e54d0(&biped->physics_mode, 3);
	}
}

/* whether the biped is landing */
// @retail 0xe4020
bool function_e4020(long arg_159e6d)
{
	return BIPED_GET(arg_159e6d)->unknown39d > 0;
}

/* forgets the biped's melee (+0x34c) */
// @retail 0xe4300
void function_e4300(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);

	biped->unknown34c = NONE;
	biped->unknown34e = 0;
	biped->unknown34f = 0;
}

/* gets the biped up and puts its Havok to sleep, unless it already sleeps */
// @retail 0xe4bd0
void function_e4bd0(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);

	function_e18d0(arg_159e6d, true);
	function_e5930(arg_159e6d);
	if (!((biped->flags_c0 >> 6) & 1))
	{
		function_b8890(arg_159e6d);
	}
}

/* gets a ragdolling biped up */
// @retail 0xe4c10
void function_e4c10(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);

	if (biped->physics_mode == 3)
	{
		biped->unknown3f9 = 0;
		biped->unknown3f8 = 0;
		function_1ed340((byte *)biped + 0x3ec, arg_159e6d);
		function_e18d0(arg_159e6d, true);
	}
}

void function_1c9c00(long object_index, long arg_1);
bool recorded_animation_playing(long object_index);
real function_1e20b0(long actor_index);

/* times how long the biped has stood on the same object (+0x398), until a
   tenth of a second, then waits half a second (negative) before again */
// @retail 0xe2fa0
void __stdcall function_e2fa0(long arg_159e6d, long object_index, void *unused)
{
	void *const *reference = &unused;
	s_biped *biped = BIPED_GET(arg_159e6d);
	char ticks = (char)biped->unknown398;

	if (0 > ticks)
	{
		if (object_index == NONE)
		{
			biped->unknown398 = ticks + 1;
		}
		else
		{
			biped->unknown398 = (byte)-(char)vehicle_round_ticks(g_510c54->field_2_3 * 0.5f);
		}
		return;
	}
	if (object_index != NONE && function_badc0(object_index, NONE))
	{
		function_1c9c00(object_index, arg_159e6d);
		if (biped->unknown13c != NONE || recorded_animation_playing(arg_159e6d))
		{
			if (biped->unknown394 != object_index)
			{
				biped->unknown394 = object_index;
				biped->unknown398 = 0;
			}
			else
			{
				biped->unknown398++;
				if ((char)biped->unknown398 * g_510c54->rate > 0.1f)
				{
					biped->unknown398 = (byte)-(char)vehicle_round_ticks(g_510c54->field_2_3 * 0.5f);
				}
			}
		}
	}
}

/* counts the ticks an actor's biped has been falling too far to jump back
   (+0x3ab), and reports it once a second */
// @retail 0xe30c0
void function_e30c0(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);
	byte *state = (byte *)biped + *(short *)((byte *)biped + 0x12a);
	long count = 0;

	if (!((biped->flags_348 >> 7) & 1) && biped->physics_mode == 1 && !((biped->flags_134 >> 9) & 1) &&
		!((*(dword *)(definition + 0x1f0) >> 2) & 1) && biped->actor_index != NONE &&
		*(long *)(state + 0x7c) != 0xe0000c3 && biped->unknown399 > function_1469f0(0.5f))
	{
		byte *globals = *(byte **)((byte *)g_4e034c + 0x144);
		s_biped_ground_collision collision;
		point3f start;
		vector3f vector;
		bool stuck = true;

		function_b9dd0(arg_159e6d, &start);
		start.x -= g_4687bc->i * 0.4f;
		start.y -= g_4687bc->j * 0.4f;
		start.z -= g_4687bc->k * 0.4f;
		vector.i = g_4687bc->i * 6.0f;
		vector.j = g_4687bc->j * 6.0f;
		vector.k = g_4687bc->k * 6.0f;
		collision.unknown24 = NONE;
		if (function_1697c0(function_ddf60(arg_159e6d) ? 0x800001 : 0x84000d, &start, &vector, NONE, NONE,
			(s_collision_result_1697c0 *)&collision))
		{
			point3f ground = collision.point;
			point3f position;
			vector3f velocity;

			function_b9dd0(arg_159e6d, &position);
			function_ba1d0(arg_159e6d, &velocity, NULL);
			if (velocity.k > 0.0f)
			{
				stuck = false;
			}
			else
			{
				real jump = *(real *)(globals + 0x64);

				if (biped->actor_index != NONE)
				{
					jump = function_1e20b0(biped->actor_index);
				}
				jump *= 1.5f;
				if (jump * jump > (position.z - ground.z) * g_51e9c4->unknown0 * 2.0f + velocity.k * velocity.k)
				{
					stuck = false;
				}
			}
		}
		if (stuck)
		{
			count = MIN(biped->unknown3ab + 1, 0xfe);
		}
	}
	biped->unknown3ab = (byte)count;
	if (biped->unknown3ab >= vehicle_round_ticks((real)g_510c54->field_2_3))
	{
		long last = biped->unknown38c;

		if (last != 1 || (real)g_510c54->game_time > g_510c54->field_2_3 * 0.5f + (real)last)
		{
			biped->unknown38c = g_510c54->game_time;
			function_114b60(NONE, 0xe, arg_159e6d, 0xd, 0);
		}
	}
}

bool function_109e00(long object_index, vector3f *velocity, bool definition_flag_required);
void function_155b60(long unit_index);
bool function_a7670(long object_index);
void __stdcall function_b8540(long a);
long function_10f8f0(long object_index);
void function_e3910(long arg_159e6d);

/* a walking biped falling faster than the globals' falling speed takes
   falling damage, unless it rides a vehicle that protects it; a falling
   object leaves its cluster */
// @retail 0xe3700
void function_e3700(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *globals = *(byte **)((byte *)g_4e034c + 0x144);
	byte *definition = BIPED_DEFINITION_GET(biped);
	bool immune = (TEST_FIELD_BIT((biped->flags_134 >> 9) & 1)) || (TEST_FIELD_BIT((*(dword *)(definition + 0x1f0) >> 2) & 1)) ||
		(TEST_FIELD_BIT((biped->flags_10a >> 7) & 1));
	vector3f velocity;
	real falling;
	real height_change;

	if (*(long *)((byte *)g_4e6948 + 8) == 2)
	{
		return;
	}
	function_ba1d0(arg_159e6d, &velocity, NULL);
	falling = velocity.k;
	if ((TEST_FIELD_BIT((biped->flags_348 >> 11) & 1)) && function_dd8e0(arg_159e6d, &height_change))
	{
		falling -= height_change;
	}
	if (function_109e00(arg_159e6d, &velocity, false))
	{
		falling -= velocity.k;
	}
	if (!immune && biped->parent_object_index != NONE)
	{
		s_biped *parent = BIPED_GET(biped->parent_object_index);

		if ((1 << *((byte *)parent + 0xaa)) & 2)
		{
			if (!(TEST_FIELD_BIT((*(dword *)(BIPED_DEFINITION_GET(parent) + 0x1ec) >> 6) & 1)))
			{
				return;
			}
			function_ba1d0(arg_159e6d, &velocity, NULL);
			falling = velocity.k;
		}
	}
	if (biped->physics_mode == 1 && 0.0f - *(real *)(globals + 0x5c) > falling)
	{
		if (!immune && !(TEST_FIELD_BIT((biped->flags_10a >> 2) & 1)))
		{
			s_type_1e6529 data;

			*((byte *)&biped->flags_348 + 1) |= 0x80;
			if (biped->unknown13c != NONE)
			{
				function_155b60(arg_159e6d);
			}
			data.material_index = NONE;
			function_d6660(&data, *(long *)(globals + 0x28));
			data.unknown84 = true;
			data.flags |= 4;
			object_get_damage_owner(arg_159e6d, (s_damage_owner *)data.unknown08);
			function_d7b80(&data, arg_159e6d, NONE, NONE, NONE, NULL);
		}
		if (*(long *)((byte *)g_4e6948 + 8) != 2 && (TEST_FIELD_BIT((biped->object_flags >> 18) & 1)) && biped->unknown13c == NONE &&
			!function_a7670(arg_159e6d))
		{
			function_b8540(arg_159e6d);
		}
	}
}

/* the airborne biped: falling damage, a random spin for one that tumbles,
   and its airborne animation */
// @retail 0xe40d0
void __stdcall function_e40d0(long arg_159e6d, long *names)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);
	byte *state = (byte *)biped + *(short *)((byte *)biped + 0x12a);
	long name;

	if (*((byte *)g_4e6948 + 0xc) != 4)
	{
		function_e3700(arg_159e6d);
	}
	if (TEST_FIELD_BIT((*(dword *)(definition + 0x1f0) >> 3) & 1))
	{
		long current = function_10f8f0(arg_159e6d);

		if (current == 0xe000038 || current == 0xa000040)
		{
			real spin = (real)random_next(&g_4e7408->unknown0) * 1.5259022e-05f * 1.0471976f + 1.5707964f;
			vector3f axis;
			bool found = false;

			if (0.8f > biped->up.k)
			{
				axis.i = biped->up.j * g_4687b0->k - g_4687b0->j * biped->up.k;
				axis.j = g_4687b0->i * biped->up.k - biped->up.i * g_4687b0->k;
				axis.k = g_4687b0->j * biped->up.i - biped->up.j * g_4687b0->i;
				found = function_30bf0(&axis) > 0.0f;
			}
			if (!found)
			{
				real angle = (real)random_next(&g_4e7408->unknown0) * 1.5259022e-05f * 6.2831855f;

				axis.i = (real)cos(angle);
				axis.j = (real)sin(angle);
				axis.k = 0.0f;
			}
			spin *= g_510c54->rate;
			biped->angular_velocity.i += spin * axis.i;
			biped->angular_velocity.j += axis.j * spin;
			biped->angular_velocity.k += axis.k * spin;
		}
		function_e3910(arg_159e6d);
	}
	name = *(long *)(state + 0x7c);
	names[1] = name == 0xa00003e || name == 0xd00003f ? 0xd00003f : 0x800001e;
}

/* turns a vector about a unit axis (Rodrigues) */
PRIVATE inline void biped_rotate_about(vector3f *v, vector3f const *axis, real sine, real cosine)
{
	real dot = (v->i * axis->i + v->k * axis->k + v->j * axis->j) * (1.0f - cosine);
	vector3f cross;
	vector3f result;

	cross.i = axis->j * v->k - axis->k * v->j;
	cross.j = axis->k * v->i - axis->i * v->k;
	cross.k = axis->i * v->j - axis->j * v->i;
	result.i = v->i * cosine + axis->i * dot + cross.i * sine;
	result.j = v->j * cosine + axis->j * dot + cross.j * sine;
	result.k = v->k * cosine + axis->k * dot + cross.k * sine;
	*v = result;
}

/* turns a tumbling biped by its angular velocity over a tick, keeping its
   facing and up square */
// @retail 0xe3910
void function_e3910(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	vector3f axis = biped->angular_velocity;
	real speed = (real)sqrt(axis.k * axis.k + axis.j * axis.j + axis.i * axis.i);
	real angle;
	real sine;
	real cosine;
	vector3f up;
	vector3f side;

	if (0.0001f > (real)fabs(speed))
	{
		speed = 0.0f;
	}
	else
	{
		real inverse = 1.0f / speed;

		axis.i *= inverse;
		axis.j *= inverse;
		axis.k *= inverse;
	}
	angle = speed * g_510c54->rate;
	cosine = (real)cos(angle);
	sine = (real)sin(angle);
	biped_rotate_about(&biped->forward, &axis, sine, cosine);
	function_30bf0(&biped->forward);
	up = biped->up;
	biped_rotate_about(&up, &axis, sine, cosine);
	side.i = up.j * biped->forward.k - up.k * biped->forward.j;
	side.j = up.k * biped->forward.i - up.i * biped->forward.k;
	side.k = up.i * biped->forward.j - up.j * biped->forward.i;
	biped->up.i = biped->forward.j * side.k - biped->forward.k * side.j;
	biped->up.j = biped->forward.k * side.i - biped->forward.i * side.k;
	biped->up.k = biped->forward.i * side.j - biped->forward.j * side.i;
	if (function_30bf0(&biped->up) == 0.0f)
	{
		biped->forward = *g_4687a8;
		biped->up = *g_4687b0;
	}
}

struct s_1fb7e0_data;
bool function_1fb7e0(short type, long actor_index, s_1fb7e0_data const *data, long target_index, long unknown);
bool function_10f340(long unit_index, long mode, long set);
real magnitude3d(vector3f const *v);

/* an actor riding a fast vehicle that is about to leave the ground (no
   ground under it, or none along its flight) says so, at most once every
   half second: falling (0x54), flipping over (0x56) or a jump (0x55) */
// @retail 0xe3380
void __stdcall function_e3380(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	s_biped *vehicle = BIPED_GET(biped->parent_object_index);
	byte *state = (byte *)biped + *(short *)((byte *)biped + 0x12a);

	if ((TEST_FIELD_BIT((*(dword *)(BIPED_DEFINITION_GET(vehicle) + 0xbc) >> 6) & 1)) && biped->actor_index != NONE &&
		*(long *)(state + 0x7c) != 0xe0000c3)
	{
		long time = g_510c54->game_time;
		long ticks = g_510c54->field_2_3;

		if (time - *(long *)((byte *)biped + 0x244) > ticks * 4 && vehicle->unknown34c > ticks &&
			(biped->unknown38c == NONE || (real)time > ticks * 0.5f + (real)biped->unknown38c))
		{
			s_biped_ground_collision collision;
			point3f start;
			vector3f vector;
			bool falling = false;

			biped->unknown38c = time;
			function_b9dd0(arg_159e6d, &start);
			start.x -= g_4687bc->i * 0.4f;
			start.y -= g_4687bc->j * 0.4f;
			start.z -= g_4687bc->k * 0.4f;
			vector.i = g_4687bc->i * 8.0f;
			vector.j = g_4687bc->j * 8.0f;
			vector.k = g_4687bc->k * 8.0f;
			collision.unknown24 = NONE;
			if (!function_1697c0(function_ddf60(arg_159e6d) ? 0x800001 : 0x84000d, &start, &vector, NONE, NONE,
				(s_collision_result_1697c0 *)&collision))
			{
				vector3f flight;

				flight.i = vehicle->linear_velocity.i * 2.0f;
				flight.j = vehicle->linear_velocity.j * 2.0f;
				flight.k = vehicle->linear_velocity.k * 2.0f - g_51e9c4->unknown0 * 2.0f;
				falling = true;
				if (function_30bf0(&flight) > 0.0f)
				{
					function_b9dd0(arg_159e6d, &start);
					start.x -= flight.i * 0.4f;
					start.y -= flight.j * 0.4f;
					start.z -= flight.k * 0.4f;
					vector.i = flight.i * 8.0f;
					vector.j = flight.j * 8.0f;
					vector.k = flight.k * 8.0f;
					collision.unknown24 = NONE;
					if (function_1697c0(function_ddf60(arg_159e6d) ? 0x800001 : 0x84000d, &start, &vector, NONE, NONE,
						(s_collision_result_1697c0 *)&collision) && collision.normal.k > 0.3f)
					{
						falling = false;
					}
				}
			}
			if (biped->actor_index != NONE)
			{
				short type;

				if (falling)
				{
					type = 0x54;
				}
				else if (vehicle->up.k > 0.6f && 1.5707964f > magnitude3d(&vehicle->angular_velocity))
				{
					type = 0x55;
				}
				else
				{
					type = 0x56;
				}
				function_1fb7e0(type, biped->actor_index, NULL, NONE, NONE);
			}
		}
	}
}

/* the dead biped's animation: its getting-up animation once it can stand,
   the ragdoll's pose while it lies, or its landing */
// @retail 0xe3c90
bool __stdcall function_e3c90(long arg_159e6d, long *names, s_biped_physics_output *output)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);
	s_animation_state *state = (s_animation_state *)((byte *)biped + *(short *)((byte *)biped + 0x12a));
	bool result = false;

	if (function_e4050(arg_159e6d) && !(*((byte *)biped + 0xc1) & 1))
	{
		s_biped *current = BIPED_GET(arg_159e6d);

		if (TEST_FIELD_BIT((current->object_flags >> 29) & 1))
		{
			real blend = 0.267f;

			if (arg_159e6d != NONE && *(short *)((byte *)current + 0x112) != NONE)
			{
				if (TEST_FIELD_BIT((current->object_flags >> 29) & 1))
				{
					function_bfa40(arg_159e6d, 0);
				}
				current->object_flags &= ~0x20000000;
			}
			function_ba350(arg_159e6d, blend);
			function_10f430(arg_159e6d, 0x7000101, 0x7000101, 0x7000101, 0xd000042, 0.0f, false, 0x210);
			return true;
		}
	}
	if (state->channels[0].graph_tag_index == NONE || state->channels[0].animation_id.index == NONE ||
		(*((byte *)state + 0x11) & 0xa))
	{
		s_biped *current = BIPED_GET(arg_159e6d);

		if (!(TEST_FIELD_BIT((current->object_flags >> 29) & 1)) && state->unknown7c != 0xd000042)
		{
			if (arg_159e6d != NONE && *(short *)((byte *)current + 0x112) != NONE)
			{
				current->object_flags |= 0x20000000;
			}
			return result;
		}
	}
	if (biped->unknown34b)
	{
		names[0] = 0x7000101;
		names[1] = 0xc000043;
		((byte *)names)[8] = 0;
		((byte *)names)[9] = 0;
		return biped->unknown34b == 1;
	}
	if (function_e4050(arg_159e6d) && function_10f340(arg_159e6d, 0x7000101, 0xd000042))
	{
		if (state->unknown7c == 0xd000042)
		{
			function_e3910(arg_159e6d);
		}
		if (*((byte *)state + 0x6c) & 1)
		{
			*((byte *)state + 0x6c) &= ~1;
		}
		names[1] = 0xd000042;
		result = true;
	}
	else
	{
		if (state->unknown7c == 0xd000042)
		{
			biped->unknown3a0 = 0.0f;
			*(real *)((byte *)output + 0x28) = 0.0f;
			function_1e5af0(&biped->physics_mode, output, &biped->up, &biped->forward);
		}
		names[1] = 0xc000043;
	}
	if (!(TEST_FIELD_BIT((biped->flags_10a >> 2) & 1)) && (TEST_FIELD_BIT((*(dword *)(definition + 0x1f0) >> 10) & 1)) &&
		BIPED_GET(arg_159e6d)->unknown39d > 0)
	{
		names[0] = 0x6000087;
		names[1] = 0x400000c;
		((byte *)names)[8] = 1;
		((byte *)names)[9] = 1;
		return true;
	}
	((byte *)names)[9] = 0;
	return result;
}

/* starts the biped's landing recovery (+0x34c: 0 soft, 1 hard) from how
   far it fell (its definition's soft and hard landing heights) */
// @retail 0xe4330
void function_e4330(long arg_159e6d, real height)
{
	real fall = height;
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);
	real soft = *(real *)(definition + 0x204);
	real hard = *(real *)(definition + 0x208);
	real maximum = *(real *)(definition + 0x20c);
	real range;
	real time;
	short kind;
	bool forced = TEST_FIELD_BIT((biped->flags_348 >> 6) & 1);
	bool const *forced_reference = &forced;

	if (!forced)
	{
		if (soft > fall)
		{
			return;
		}
		if (hard > fall)
		{
			range = hard - soft;
			time = *(real *)(definition + 0x1fc);
			fall -= soft;
			kind = 0;
		}
		else
		{
			range = maximum - hard;
			time = *(real *)(definition + 0x200);
			kind = 1;
		}
	}
	else
	{
		fall = maximum;
		range = maximum - hard;
		time = *(real *)(definition + 0x200);
		*(byte *)&biped->flags_348 &= ~0x40;
		kind = 1;
	}
	if (range > 0.0f)
	{
		long ticks = vehicle_round_ticks(time * PIN(fall / range, 0.0f, 1.0f) * g_510c54->field_2_3);

		if (ticks > 0)
		{
			biped->unknown34c = kind;
			biped->unknown34e = 0;
			biped->unknown34f = (byte)MIN(ticks, 0x7f);
		}
	}
}

struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;
void __stdcall function_e4d90(long arg_159e6d, long type, real scale, long a);

/* steps the biped's landing recovery and picks its landing animation */
// @retail 0xe4450
void __stdcall function_e4450(long arg_159e6d, long *names)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	char ticks = (char)++biped->unknown34e;
	char duration = (char)biped->unknown34f;
	bool first = (ticks < duration ? duration : (ticks > 1 ? 1 : ticks)) == ticks;
	byte *state;

	if (ticks >= duration)
	{
		s_biped *current = BIPED_GET(arg_159e6d);

		current->unknown34c = NONE;
		current->unknown34e = 0;
		current->unknown34f = 0;
	}
	if (biped->unknown34c == 0 &&
		(names[1] == 0xa000014 || names[1] == 0x9000015 || names[1] == 0x9000016 || names[1] == 0xa000017) &&
		(char)biped->unknown34e >= (char)biped->unknown34f >> 1)
	{
		s_biped *current = BIPED_GET(arg_159e6d);

		current->unknown34c = NONE;
		current->unknown34e = 0;
		current->unknown34f = 0;
	}
	if ((!g_510c50 || !((byte *)g_510c50)[5]) && (biped->unknown34e == 1 || first))
	{
		function_e4d90(arg_159e6d, 5, 1.0f, 2);
	}
	if (biped->unknown34c != NONE)
	{
		long name;

		state = (byte *)biped + *(short *)((byte *)biped + 0x12a);
		name = *(long *)(state + 0x7c);
		if ((name == 0xd00003f || name == 0x90006b2) && function_10f340(arg_159e6d, 0x7000101, 0x90006b2))
		{
			names[1] = 0x90006b2;
			return;
		}
		if (biped->unknown34c == 1)
		{
			names[1] = 0x9000020;
		}
		else if (biped->unknown34c == 0)
		{
			names[1] = 0x900001f;
		}
	}
}

/* a biped knocked down for a tenth of a second and still moving tumbles */
// @retail 0xe45e0
void function_e45e0(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);

	if ((char)biped->unknown39a * g_510c54->rate > 0.1f)
	{
		vector3f velocity;

		function_ba1d0(arg_159e6d, &velocity, NULL);
		if (velocity.i * velocity.i + velocity.j * velocity.j + velocity.k * velocity.k > 1.0f)
		{
			function_e4d90(arg_159e6d, 2, 1.0f, 2);
		}
	}
}

bool __stdcall function_e4770(long arg_159e6d);

/* jumps when the jump control is held and the biped can: on the ground a
   moment, not landing, and (for a player) not jumped too recently */
// @retail 0xe4680
bool function_e4680(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	bool jumping = (biped->control_flags >> 1) & 1;
	bool jumped = false;

	if (biped->unknown39c < 0x7f)
	{
		biped->unknown39c++;
	}
	if (jumping)
	{
		long delay = vehicle_round_ticks(g_510c54->field_2_3 * 0.3f);

		if ((biped->unknown13c == NONE || (char)biped->unknown39e < delay) &&
			biped->unknown39c * g_510c54->rate > 0.16f && !function_e4050(arg_159e6d) && biped->unknown34c != 1 &&
			function_e4770(arg_159e6d))
		{
			biped->unknown39e = (byte)delay;
			jumped = true;
		}
		if ((char)biped->unknown39e < 0x7f)
		{
			biped->unknown39e++;
		}
	}
	else
	{
		biped->unknown39e = 0;
	}
	return jumped;
}

bool function_1f5560(long actor_index, vector3f *facing);

/* jumps: an upward kick to the definition's jump speed (less for a tired
   player), relative to what the biped stands on; an actor may refuse.
   Retail keeps the biped on the stack (ret 4): its address is taken */
// @retail 0xe4770
bool __stdcall function_e4770(long arg_159e6d)
{
	long const *reference = &arg_159e6d;
	s_biped *biped = BIPED_GET(*reference);
	byte *definition = BIPED_DEFINITION_GET(biped);
	real jump;
	vector3f ground;
	vector3f velocity;
	vector3f relative;
	bool riding;
	bool result = true;
	real along;

	if (function_e4050(arg_159e6d) || biped->unknown34c == 1)
	{
		return false;
	}
	jump = *(real *)(definition + 0x1f8);
	riding = function_109e00(arg_159e6d, &ground, false);
	if (biped->unknown13c != NONE)
	{
		byte *globals = *(byte **)((byte *)g_4e034c + 0x134);

		jump = (1.0f - *(real *)(globals + 0x7c) * biped->unknown2e4) * jump;
	}
	function_ba1d0(arg_159e6d, &velocity, NULL);
	relative = velocity;
	if (riding)
	{
		relative.i -= ground.i;
		relative.j -= ground.j;
		relative.k -= ground.k;
	}
	along = biped->up.k * relative.k + biped->up.j * relative.j + biped->up.i * relative.i;
	if (jump > along)
	{
		real kick = jump - along;

		velocity.i += biped->up.i * kick;
		velocity.j += biped->up.j * kick;
		velocity.k += biped->up.k * kick;
	}
	if (biped->actor_index != NONE)
	{
		result = function_1f5560(biped->actor_index, &velocity);
		if (!result)
		{
			return false;
		}
		if (riding)
		{
			velocity.i += ground.i;
			velocity.j += ground.j;
			velocity.k += ground.k;
		}
	}
	function_e4d90(arg_159e6d, 4, 1.0f, 2);
	function_e57e0(arg_159e6d);
	biped->unknown39c = 0;
	function_b7740(arg_159e6d, &velocity, NULL, false);
	function_1c4b00(arg_159e6d, &velocity, NULL, 1);
	if (velocity.k * velocity.k + velocity.j * velocity.j + velocity.i * velocity.i > 0.0001f)
	{
		function_b9b90(arg_159e6d, false);
		function_b7360(arg_159e6d);
		function_bba20(arg_159e6d);
	}
	return result;
}

void function_e5790(long arg_159e6d);

/* lands or is knocked to the ground: a crouching biped standing up into a
   ceiling is lifted clear, the hit is remembered (+0x44c..), and a flag
   plays the knocked-down animation */
// @retail 0xe4a20
void __stdcall function_e4a20(point3f const *point, long arg_159e6d, long object_index, bool knocked)
{
	s_biped *biped = BIPED_GET(arg_159e6d);

	if (biped->physics_mode != 3)
	{
		if (biped->parent_object_index == NONE && function_e4050(arg_159e6d))
		{
			vector3f velocity;

			function_ba1d0(arg_159e6d, &velocity, NULL);
			if (g_4687b0->j * velocity.j + g_4687b0->k * velocity.k + g_4687b0->i * velocity.i > 0.001f)
			{
				byte *definition = BIPED_DEFINITION_GET(biped);
				real room = *(real *)(definition + 0x26c) - *(real *)(definition + 0x270) * 2.0f;

				if (room > 0.0f)
				{
					real lift = room > 0.0328f * 2.0f ? 0.0328f * 2.0f : room;
					point3f position;

					function_b9dd0(arg_159e6d, &position);
					position.z += lift;
					function_b75a0(arg_159e6d, &position, NULL, NULL, NULL, false);
				}
			}
		}
		function_e5790(arg_159e6d);
		if (point && object_index != NONE)
		{
			*(long *)((byte *)biped + 0x450) = object_index;
			*(long *)((byte *)biped + 0x44c) = g_510c54->game_time;
			*(point3f *)((byte *)biped + 0x454) = *point;
		}
	}
	if (knocked)
	{
		s_animation_state *state = (s_animation_state *)((byte *)biped + *(short *)((byte *)biped + 0x12a));

		function_10f430(arg_159e6d, 0x7000101, 0x7000101, 0x7000101, 0xc000043, 0.0f, false, 0x210);
		state->channels_finish();
		*((byte *)state + 0x6c) |= 1;
		*((byte *)state + 0x65) = 0;
		*((byte *)state + 0x64) = 0;
		*((byte *)state + 0x67) = 0;
		*((byte *)&biped->flags_348 + 1) |= 4;
	}
}

/* plays the biped's turning footstep: when its turn starts (+0x34a 1),
   a tenth of a second into turning in place, or as a turning animation
   loops, scaled by how far it turned */
// @retail 0xe4c50
void function_e4c50(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *state;
	long name;
	bool timed = false;

	switch ((char)biped->unknown34a)
	{
	case 0:
		if ((char)biped->unknown39b > 0)
		{
			biped->unknown39b++;
			if ((char)biped->unknown39b * g_510c54->rate > 0.1f)
			{
				biped->unknown39b = 0;
				timed = true;
			}
		}
		break;
	case 1:
		biped->unknown39b = 1;
		break;
	default:
		biped->unknown39b = 0;
		break;
	}
	if (!timed)
	{
		state = (byte *)biped + *(short *)((byte *)biped + 0x12a);
		name = *(long *)(state + 0x7c);
		if (name != 0x900000e && name != 0xa00000f)
		{
			return;
		}
		if (*(long *)state != NONE && *(short *)(state + 6) != NONE &&
			*(real *)(state + 0x1c) * 0.033333335f != 0.0f)
		{
			return;
		}
	}
	{
		real scale = 1.0f;

		if (timed)
		{
			scale = PIN((char)biped->unknown39f * 0.007874016f, 0.0f, 1.0f);
		}
		function_e4d90(arg_159e6d, 3, scale, 2);
		biped->unknown39f = 0;
	}
}

transform4x3f *function_b8bd0(long object_index, short node_index);
bool function_11c120(s_location const *location, point3f const *point, short *zone_index);
void function_188180(point3f const *point, vector3f const *forward, long tag_index, long object_index, long index,
	long variant, long unused, long effect_value, s_location const *location, real scale);

/* a walking biped's footstep effect: at its foot marker (or its origin),
   with the material it stands on */
// @retail 0xe4d90
void __stdcall function_e4d90(long arg_159e6d, long type, real scale, long marker)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);

	if (biped->physics_mode == 1 && *(long *)(definition + 0x58) != NONE)
	{
		point3f point;
		s_location *location = (s_location *)((byte *)biped + 0x28);
		s_object_marker foot;

		if (marker < *(long *)(definition + 0x2f8) &&
			function_b8d30(arg_159e6d, (*(long **)(definition + 0x2fc))[marker], &foot, 1, false))
		{
			point = foot.matrix.position;
		}
		else
		{
			function_b9dd0(arg_159e6d, &point);
		}
		if (TEST_FIELD_BIT((biped->flags_c0 >> 6) & 1))
		{
			short material = *(short *)((byte *)biped + 0x44c);
			short zone = NONE;
			point3f probe;

			function_b9dd0(arg_159e6d, &probe);
			probe.z += 0.0328f;
			if (function_11c120(location, &probe, &zone))
			{
				material = zone;
			}
			function_188180(&point, (vector3f *)((byte *)biped + 0x450), *(long *)(definition + 0x58), arg_159e6d,
				type, *((byte *)definition + 0x1a), material, (long)g_4687bc, location, scale);
		}
		else if (biped->parent_object_index == NONE)
		{
			byte *model = g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes;
			long count = *(long *)(model + 0x78);

			if (count >= 1)
			{
				s_biped_ground_collision collision;
				vector3f down = *g_4687bc;
				long i;

				point = function_b8bd0(arg_159e6d, 0)->position;
				for (i = 0; i < count; i++)
				{
					if (*(long *)(*(byte **)(model + 0x7c) + i * 0x5c) == 0x60005ba)
					{
						break;
					}
				}
				collision.unknown24 = NONE;
				point.x += g_4687b0->i * 0.1f;
				point.y += g_4687b0->j * 0.1f;
				point.z += g_4687b0->k * 0.1f;
				if (function_1697c0(0x84000d, &point, &down, NONE, NONE, (s_collision_result_1697c0 *)&collision))
				{
					function_188180(&point, g_4687b0, *(long *)(definition + 0x58), arg_159e6d, type,
						*((byte *)definition + 0x1a), collision.unknown24, (long)g_4687bc, location, scale);
				}
			}
		}
	}
}

/* the result of the biped's character physics update (0x1e55d0) */
struct s_biped_physics_move
{
	byte flags;
	byte unknown01[3];
	long direction;
	byte value;
	byte unknown09[3];
	vector3f velocity;
	vector3f forward;
	vector3f up;
	byte unknown30[0x54 - 0x30];
	bool placed;
	byte unknown55[0x64 - 0x55];
	point3f position;
};

void __stdcall function_1e55d0(s_biped_physics_move *move, void *physics, s_biped_physics_output *output);
void matrix4x3_from_forward_and_up(transform4x3f *out, vector3f const *forward, vector3f const *up);
vector3f *function_1427f0(transform4x3f const *matrix, vector3f const *vector,
	vector3f *out);
bool __stdcall function_1cd8a0(s_animation_state *state, long arg_159e6d, vector3f const *velocity);
bool __stdcall function_1cdb00(s_animation_state *state, long arg_159e6d, vector3f const *control);
void function_e6f90(long unit_index);
void __stdcall function_b87b0(long object_index);
void function_1c4a80(long object_index, long a, long b);
void __stdcall function_b8600(long object_index, long unknown);

/* applies the biped's character physics move: its facing and up, its
   animation names, its movement animation's rate, crouching and grabbing,
   and its new velocity (or, asleep, its new position) */
// @retail 0xe24f0
bool __stdcall function_e24f0(long arg_159e6d, long *names, s_biped_physics_output *output)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);
	s_animation_state *state = (s_animation_state *)((byte *)biped + *(short *)((byte *)biped + 0x12a));
	word const *animation_flags = (word const *)((byte *)biped + biped->unknown33e);
	s_biped_physics_move move;
	vector3f velocity;
	bool still = false;
	bool grabbing;
	real change;

	*(byte *)&biped->flags_348 &= ~0x10;
	if (function_ddfb0(arg_159e6d))
	{
		still = true;
		if (*((byte *)biped + 0xc1) & 1)
		{
			function_b9b90(arg_159e6d, false);
		}
	}
	if ((*((byte *)biped + 0xc1) & 1) && (TEST_FIELD_BIT((biped->flags_10a >> 2) & 1)) &&
		((*((byte *)state + 0x6c) & 1) || (TEST_FIELD_BIT((*animation_flags >> 2) & 1))))
	{
		return still;
	}
	function_1e55d0(&move, &biped->physics_mode, output);
	biped->forward = move.forward;
	*(vector3f *)((byte *)output + 0xf4) = move.forward;
	biped->up = move.up;
	function_1e5af0(&biped->physics_mode, output, &biped->up, &biped->forward);
	function_e2e90(arg_159e6d, &names[0], &names[1], move.direction, move.value, (byte *)&names[2]);
	if ((biped->physics_mode == 4 || biped->physics_mode == 5) && move.placed)
	{
		function_b75a0(arg_159e6d, &move.position, NULL, NULL, NULL, false);
	}
	if (names[1] == 0x400000c && biped->unknown34a == 1)
	{
		byte *idle = (byte *)biped + 0x3a8;

		if (idle[1])
		{
			idle[0] = 0;
		}
		else
		{
			idle[0]++;
		}
		if (0.1f > idle[0] * g_510c54->rate)
		{
			c_type_709360 animation_id;

			names[1] = state->unknown7c;
			animation_id = state->animation_get(0x400000c, 0x7000101, 0x7000101);
			if (animation_id.index != NONE)
			{
				state->animation_touch(animation_id);
			}
		}
	}
	*((byte *)biped + 0x3a9) = function_dd300(names[1]);
	if (state->graph_tag_index != NONE && state->channels[0].graph_tag_index != NONE &&
		state->channels[0].animation_id.index != NONE && state->unknown7c != 0xe0000c2 &&
		(arg_159e6d == NONE || !(*((byte *)BIPED_GET(arg_159e6d) + 0xb3) > 0)))
	{
		if (TEST_FIELD_BIT((*(dword *)(definition + 0x264) >> 2) & 1))
		{
			transform4x3f frame;
			vector3f local;

			velocity = move.velocity;
			if (biped->physics_mode == 1)
			{
				velocity.i -= move.position.x;
				velocity.j -= move.position.y;
				velocity.k -= move.position.z;
			}
			matrix4x3_from_forward_and_up(&frame, &biped->forward, &biped->up);
			function_1427f0(&frame, &velocity, &local);
			function_1cd8a0(state, arg_159e6d, &local);
		}
		else
		{
			function_1cdb00(state, arg_159e6d, &biped->control);
		}
	}
	if (move.flags & 4)
	{
		switch (biped->physics_mode)
		{
		case 4:
			function_e6f90(arg_159e6d);
			function_e5930(arg_159e6d);
			break;
		case 6:
		{
			byte *grab = (byte *)biped + *(short *)((byte *)biped + 0x346);

			if (*(long *)(grab + 0x2c) != NONE && (char)grab[0x32] <= (char)grab[0x33])
			{
				break;
			}
			if (function_e0dc0(arg_159e6d))
			{
				function_e0c70(arg_159e6d);
			}
			function_e5930(arg_159e6d);
			break;
		}
		default:
			function_e5930(arg_159e6d);
			break;
		}
	}
	if (biped->unknown3aa && !function_e0dc0(arg_159e6d))
	{
		biped->unknown3aa += 0 >= (char)biped->unknown3aa ? 1 : -1;
	}
	{
		s_biped *current = BIPED_GET(arg_159e6d);
		bool crouched = TEST_FIELD_BIT((biped->flags_134 >> 23) & 1);
		bool crouching = biped->control_flags & 1;
		bool released = TEST_FIELD_BIT((move.flags >> 1) & 1);

		grabbing = current->physics_mode == 6 && current->unknown3f8 && (TEST_FIELD_BIT((current->flags_348 >> 13) & 1));
		if (biped->physics_mode != 4 && !grabbing)
		{
			if (!crouched ? crouching : (!crouching && released))
			{
				if (crouching)
				{
					biped->flags_134 |= 0x800000;
				}
				else
				{
					biped->flags_134 &= ~0x800000;
				}
				function_1c3770(arg_159e6d, 0);
			}
		}
	}
	if (!(TEST_FIELD_BIT((biped->flags_348 >> 14) & 1)) && grabbing && !(TEST_FIELD_BIT((biped->flags_134 >> 23) & 1)))
	{
		long ticks = (long)((0.0f - (*(real *)(definition + 0x268) - *(real *)(definition + 0x26c))) /
			(g_510c54->rate * 0.15f) + 0.5f);

		function_1c3770(arg_159e6d, 0);
		*((byte *)&biped->flags_348 + 1) |= 0x20;
		biped->unknown3aa = (byte)ticks;
		*((byte *)&biped->flags_348 + 1) |= 0x40;
	}
	change = function_dd7d0(arg_159e6d);
	((byte *)names)[9] = TEST_FIELD_BIT((biped->flags_134 >> 23) & 1);
	velocity = move.velocity;
	if (!(TEST_FIELD_BIT((*(dword *)(definition + 0x264) >> 1) & 1)) && (function_e4050(arg_159e6d) || grabbing))
	{
		velocity.k += (*(real *)(definition + 0x268) - *(real *)(definition + 0x26c)) * g_510c54->field_2_3 *
			change;
	}
	*((byte *)&biped->flags_348 + 1) |= 8;
	if (move.flags & 1)
	{
		biped->flags_348 |= 2;
	}
	else
	{
		*(byte *)&biped->flags_348 &= ~2;
	}
	if (TEST_FIELD_BIT((biped->flags_c0 >> 6) & 1))
	{
		s_havok_component *component = havok_component_get(biped->havok_component_index);

		if (!(TEST_FIELD_BIT((component->unknown04 >> 11) & 1)))
		{
			vector3f const *push = (vector3f const *)((byte *)output + 0x138);
			bool pushed = !biped->unknown34b && push->i * push->i + push->j * push->j + push->k * push->k > 0.0f;
			bool resting = function_ddfb0(arg_159e6d) || pushed;

			BIPED_GET(arg_159e6d)->linear_velocity = velocity;
			function_1c4b00(arg_159e6d, &velocity, NULL, resting);
		}
	}
	else
	{
		s_biped *current = BIPED_GET(arg_159e6d);
		bool sleeping = TEST_FIELD_BIT((current->object_flags >> 8) & 1);
		point3f position;
		point3f *origin = (point3f *)((byte *)biped + 0x64);

		position.x = velocity.i * g_510c54->rate + origin->x;
		position.y = velocity.j * g_510c54->rate + origin->y;
		position.z = velocity.k * g_510c54->rate + origin->z;
		if (sleeping)
		{
			function_b87b0(arg_159e6d);
		}
		if (current->parent_object_index != NONE ||
			(position.x >= -32768.0f && 32768.0f >= position.x && position.y >= -32768.0f && 32768.0f >= position.y &&
			position.z >= -32768.0f && 32768.0f >= position.z))
		{
			*(point3f *)((byte *)current + 0x64) = position;
			if (BIPED_GET(arg_159e6d)->unknown0d4 != NONE)
			{
				function_b58c0(BIPED_GET(arg_159e6d)->unknown0d4, 2);
			}
		}
		function_1c4a80(arg_159e6d, 0, 0);
		if (sleeping)
		{
			function_b8600(arg_159e6d, 0);
		}
		function_bba20(arg_159e6d);
		if (g_4687a4)
		{
			BIPED_GET(arg_159e6d)->linear_velocity = *g_4687a4;
		}
		function_1c4b00(arg_159e6d, g_4687a4, NULL, 0);
	}
	{
		real aim = biped->unknown3a4;

		if (biped->crouch == 0.0f && *(long *)((byte *)g_4e6948 + 8) != 2 && (biped->control_flags & 0x180))
		{
			if (biped->control_flags & 0x80)
			{
				aim -= g_510c54->rate * 4.0f;
				biped->unknown3a4 = -1.0f > aim ? -1.0f : aim;
			}
			else
			{
				aim += g_510c54->rate * 4.0f;
				biped->unknown3a4 = aim > 1.0f ? 1.0f : aim;
			}
		}
		else if (aim > 0.0f)
		{
			aim -= g_510c54->rate * 6.0f;
			biped->unknown3a4 = 0.0f > aim ? 0.0f : aim;
		}
		else if (0.0f > aim)
		{
			aim += g_510c54->rate * 6.0f;
			biped->unknown3a4 = aim > 0.0f ? 0.0f : aim;
		}
	}
	return true;
}

struct s_e5980_object_flags
{
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word flag3 : 1;
	word flag4 : 1;
	word flag5 : 1;
	word flag6 : 1;
	word flag7 : 1;
	word flag8 : 1;
	word flag9 : 1;
	word flag10 : 1;
	word flag11 : 1;
	word flag12 : 1;
	word flag13 : 1;
	word flag14 : 1;
	word flag15 : 1;
};

struct s_e5980_definition
{
	byte unknown00[0x264];
	union
	{
		byte flags_byte;
		struct
		{
			dword flag0 : 1;
			dword flag1 : 1;
			dword flag2 : 1;
			dword flag3 : 1;
			dword flag4 : 1;
		};
	};

};

// @retail 0xe5980
long function_e5980(long arg_159e6d_3)
{
	s_biped *biped = BIPED_GET(arg_159e6d_3);
	s_e5980_object_flags *flags =
		(s_e5980_object_flags *)&biped->flags_10a;

	if (TEST_FIELD_BIT(flags->flag2))
		return 3;

	s_e5980_definition *definition =
		(s_e5980_definition *)BIPED_DEFINITION_GET(biped);

	return TEST_FIELD_BIT(definition->flag4) ? 2 : 1;
}

/* the biped object type's definition: its callbacks are reached only through
   it, so their addresses escape and they keep the standard convention. The
   callbacks other files define are left NULL here. */
struct s_biped_type_definition_view
{
	char const *name;
	long group_tag;
	short datum_size;
	short unknown0a;
	short unknown0c;
	short unknown0e;
	void *functions[29];
	void *types[3];
	byte unknown90[0xc8 - 0x90];
};

extern s_biped_type_definition_view g_467a78;

s_biped_type_definition_view g_467a78 =
{
	"biped",
	'bipd',
	0x464,
	0x60,
	0x68,
	0x54,
	{
		NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, (void *)function_dc540,
		(void *)biped_place, NULL, NULL, NULL,
		(void *)function_dd360, (void *)function_dd990, NULL, NULL,
		NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL,
		NULL, NULL, (void *)function_de4c0, NULL,
		NULL
	},
	{ NULL, NULL, &g_467a78 }
};

PRIVATE inline void transform_vector_e59e0(transform4x3f const *matrix, vector3f *vector)
{
	real x = vector->i;
	real y = vector->j;
	real z = vector->k;
	if (matrix->scale != 1.0f)
	{
		x *= matrix->scale;
		y *= matrix->scale;
		z *= matrix->scale;
	}
	vector->i = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x;
	vector->j = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x;
	vector->k = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x;
}

PRIVATE inline void transform_point_e59e0(transform4x3f const *matrix, point3f *point)
{
	real x = point->x;
	real y = point->y;
	real z = point->z;
	if (matrix->scale != 1.0f)
	{
		x = matrix->scale * x;
		y = matrix->scale * y;
		z = matrix->scale * z;
	}
	point->x = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x + matrix->position.x;
	point->y = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x + matrix->position.y;
	point->z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z;
}

// @retail 0xe59e0
void function_e59e0(long arg_159e6d, vector3f *velocity, point3f *position)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	s_animation_state *state =
		(s_animation_state *)((byte *)biped + *(short *)((byte *)biped + 0x12a));
	transform4x3f matrix;

	function_ba160(arg_159e6d, &matrix);
	if (state->velocity_get((vector3f *)position, velocity))
	{
		transform_point_e59e0(&matrix, position);
		transform_vector_e59e0(&matrix, velocity);
	}
	else
	{
		*position = matrix.position;
		*velocity = *g_4687a4;
	}
}
