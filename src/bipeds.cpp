// @flags /O2 /arch:SSE /Gr
#include "orientations_storage.h"
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
#include "vector_products.h"
#include "normalize3d.h"
#include "unknown_1cec30.h"
#include "object_markers.h"
#include "slot_handler.h"
#include "unknown_11cc90.h"
#include "bit_vector.h"
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

/* whether a bit of a flags field is set */
static inline bool field_bit_test(dword flags, long bit)
{
	bool result = TEST_FIELD_BIT((flags >> bit) & 1);
	return result;
}

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

	function_ba160(arg_159e6d, &matrix);
	if (field_bit_test(*(dword *)(definition + 0xbc), 11))
	{
		facing = matrix.forward;
	}
	else
	{
		facing = BIPED_GET(arg_159e6d)->unknown168;
	}
	function_1e68a0((s_character_physics_update_input_datum_a *)input, (s_source_a *)&biped->physics_mode,
		(long)(definition + 0x264), biped->havok_component_index,
		field_bit_test(biped->flags_c0, 2) || field_bit_test(biped->flags_c0, 1) ? biped->unknown0b8 : NONE,
		field_bit_test(biped->flags_c0, 8), function_e4050(arg_159e6d),
		field_bit_test(*animation_flags, 2) && field_bit_test(*animation_flags, 4),
		0.16f > biped->unknown39c * g_510c54->rate, flag, &matrix.position, (point3f *)&facing,
		(point3f *)&biped->control);
	switch (biped->physics_mode)
	{
	case 1:
		input->valid = true;
		input->unknown44 = biped->unknown408;
		break;
	case 2:
	case 6:
		input->valid = true;
		break;
	case 3:
		input->dead = biped->unknown34b != 0;
		input->valid = true;
		break;
	case 4:
	case 5:
	{
		s_havok_component *component = havok_component_get(biped->havok_component_index);
		point3f position;

		havok_component_rigid_body_position_get(havok_component_main_rigid_body_index_get(component), component,
			&position);
		input->position = position;
		input->valid = true;
		break;
	}
	default:
		__assume(0);
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
		transform4x3f const *nodes = object_node_matrix(object_get(arg_159e6d), 0);
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
			for (long component = 0; component < 3; ++component)
			{
				real *current = &biped->unknown3cc.n[component];
				real difference = local.n[component] - *current;
				if (difference < -0.01f)
					difference = -0.01f;
				else if (difference > 0.01f)
					difference = 0.01f;
				biped->unknown3cc.n[component] += difference;
			}
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
// score 118 (was 118): flag-store scheduling and the final helper's unwanted expansion remain; BLOCKED-BY 0xe5930.
// Logic walk (2026-10-31): every store of 0xde4c0-0xde608 agrees with this source (offsets, sizes, the NONE/0/0x7f values,
// the 0xb9dd0 call with &+0x368, the 0x1f0 bit 1 choosing flags_348 |= 8 / &= ~8, +0x3d8/+0x3e0/+0x3dc/+0x3e4/+0x3e8, the
// final 0xe5930 call with the unit index); no behaviour difference. Only the inlined 0xe5930 body and the flag test differ.
// Boolean predicate 121; native word flags 114; reset zero/NONE/both chains 118;
// chains with Boolean predicate 121; counter/index/initial block library clear or fill
// 129/138/127; second receiver scope 118; native tag dword or bit member 121;
// native word stores with the tag in a named flag 113 (MiniMax B run 14; under the keep bar, restored).
// either tag form with native word stores 113 (subthreshold, restored).
// Native destination word bit: Boolean branch 113; direct bit 127; direct Boolean 139;
// negative Boolean admission 116. Byte destination bit: those four forms 121/131/131/121.
// Best five-point gain is below threshold; original type and body restored.
// Round 198: Complete destination flags in a local, stored once: byte 118, word 130,
// dword/long 122, with either branch or conditional-value form. Original body restored.
// Round 108: the definition test wrapped in TEST_FIELD_BIT 121 (417 bytes, the test becomes retail's
// shr/test); with the set/clear as word_flags_set(&biped->flags_348, 3, test) 113 at 415 bytes, and
// everything before the 0xe5930 call is then retail's; that plus an inline_depth(0) pragma (diagnostic)
// 20 at 343 bytes. Retail never inlines 0xe5930 (0xe5980 has no other caller); ours inlines it at the
// straight-line calls here and in 0xe4bd0, while 0xe24f0, 0xea1f0 and 0xec0d0 call it. Restored.
// Round 107 (lane 7): the test through the file's field_bit_test with the set/clear as word_flags_set 113 (415 bytes,
// everything before the 0xe5930 call retail's, as round 108's TEST_FIELD_BIT form; under the gate); `#pragma
// auto_inline(off)` around this function (diagnostic, not kept) still inlines 0xe5930 (118): LTCG ignores the caller
// file's inline settings. Our build calls 0xe5930 out of line only from 0xe2d80, 0xe24f0, 0xea1f0 and 0xec0d0.
// Also: the six NONE stores through `biped` instead of the fresh lookup 110 (396 bytes; retail does re-look the
// biped up after 0xb9dd0 at 0xde56e, so not kept); the flag set/clear through `current` 120; the definition looked
// up just before its test 147.
// Round 65 (lane 9): why 0xe5930 is inlined here: our LTCG inlines it at straight-line call sites and keeps it a
// call where the call sits under a condition (0xe2d80, 0xe24f0's switch, 0xec0d0), as it does with 0xe0dc0 (0xde9d0
// matched once that call moved into a switch default); retail never inlines it. Diagnostic `if (arg_159e6d != NONE)`
// around the call (not retail's, reverted): 21 at 346 bytes. The flag through word_flags_set and TEST_FIELD_BIT 113;
// TEST_FIELD_BIT with word-wide |= 8 / &= ~8 113; the mode stored before the index 114; word_flags_set with
// field_bit_test and the call's index read back from +0x3e0 131. BLOCKED-BY 0xe5930 (contributor file): the fix
// belongs to its definition.
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

	if (target_index != NONE)
	{
		if (biped->physics_mode != 6)
		{
			biped->flags_348 &= ~0x2000;
			biped->unknown3aa = 0;
			if (flag || biped->unknown13c == NONE || function_de9d0(arg_159e6d, target_index))
			{
				biped->unknown390 = target_index;
				function_1e54d0(&biped->physics_mode, 6);
				biped->unknown3f8 = flag;
				biped->unknown444 = biped->unknown168;
				if (flag && !TEST_FIELD_BIT((biped->flags_134 >> 23) & 1))
				{
					biped->flags_348 |= 0x2000;
				}
			}
			else
			{
				biped->unknown390 = NONE;
			}
		}
	}
	else
	{
		biped->unknown390 = NONE;
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
	byte unknownf4[0x150 - 0xf4];
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
/* the byte at +0x10a as bits */
struct s_biped_flags_10a_view
{
	word bit0 : 1;
	word bit1 : 1;
	word bit2 : 1;
	word unknown3 : 4;
	word bit7 : 1;
};

// score 241 (was 280): frame 0x16c agrees; retail computes the state pointer at the top and spills it (ours reads it at
// its one use, so the head is 25 bytes short), the 0x111650 call takes names in EAX where retail pushes both
// arguments (BLOCKED-BY 0x111650, unmatched, ret 8 in retail), and the type-0 bit is read as `and al, 4` (retail
// `shr al, 2; and al, 1`).
// Earlier (score 280 (was 281)): early EBX save, state/change homes and Boolean tail differed. (score 281 (was 305)):
// flag 2 of +0x10a through the word bitfield view and the definition flag as a field bit.
// Logic walk (2026-10-31): every callee and its argument order, the names triple, the type-0 names[0] choice
// (0xdd51f-0xdd524), each `changed` path, the signed compares (0xdd4ed) and the constants 0.55/0/0.01 agree with
// retail. Retail computes the state pointer before the Havok test (0xdd3b7-0xdd3c0) and spills it to [esp+0x20];
// any early state local takes EBX here. Tried: +0x10a/+0x348 bits through bitfield views at every use, the state at
// its one use and `changed |= function_111650(...) | action` 297 (1106 bytes; changed in BL and the late EBX save as
// retail, but the 0x111650 call takes names in EAX where retail pushes both arguments, 0xdd79d-0xdd7a2).
// Array zero initialization 300; long parent type 286; native byte flags/Boolean shifts 283;
// array/type 301; array/bits 299; type/bits 282; all 300; Boolean result 281;
// all/result 300; entry array with all/result 307; guarded exit 295; all/guard 300;
// array/guard 300; Boolean result/guard 295. Original array, field view, body and exits restored.
// Round 48, kept: names zeroed with memset before [0]/[1] are set (retail's three zero stores after the havok test)
// with the state pointer read at its one use (EBX is saved late after the early exit and `changed` lives in BL, as
// retail: 280 -> 242), and the parent type as a long (retail's movzx/cmp ecx, 1/test ecx; 241, plain). Tried: memset
// alone 299 (retail's zero stores; state keeps EBX); `changed` declared first 299; the type-0 bit as a Boolean cast
// 241 or through the bit view 293 (retail's shr/and, the listing shifts a line); `changed |= 0x111650(...) | action;
// return changed;` 241; the body inside `if (havok_component_index != NONE)` with one `return changed` 312 (head as
// retail: `xor al, al` before the test and the early exit shares the epilogue, but 29 bytes short), with the state at
// the top 303 (state takes EBX again), with `changed` declared first 321. Compile-only: all 120 orders of the five top
// declarations leave the state in a register and `changed` in memory.
// Round 65 (lane 9): one `return changed` with the body inside the Havok test and `changed |= 0x111650(...) | action`
// 312 (1092 bytes); that with the state at the top 303 (1126 bytes; aligned 75 against 91 for this form, but the
// state takes EBX at entry and `changed` a stack byte: the earlier-defined value gets the register); with
// `changed |= function_e2d80(...)` 303; the exit through a separate `result` with `changed` declared at the
// 0xe2d80 call 301. Retail keeps `changed` in BL and spills the state, so `changed` exists before the state there.
// @retail 0xdd360
bool __stdcall function_dd360(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);
	long parent_index = function_baf40(biped->parent_object_index);
	bool changed = false;
	long names[3];

	if (biped->havok_component_index == NONE)
	{
		return false;
	}
	memset(names, 0, sizeof(names));
	names[0] = 0x7000101;
	names[1] = 0x400000c;
	changed = function_e2d80(arg_159e6d);
	if (parent_index != NONE)
	{
		s_biped_header *header = &((s_biped_header *)g_4e0300->data)[parent_index & 0xffff];
		long parent_type = ((byte *)header)[3];

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

		if (TEST_FIELD_BIT(((s_biped_flags_10a_view *)&biped->flags_10a)->bit2) ||
			(biped->physics_mode != 2 && !(TEST_FIELD_BIT((*(dword *)(definition + 0x264) >> 3) & 1))))
		{
			biped->facing_goal.k = 0.0f;
			if (function_30bf0(&biped->facing_goal) == 0.0f)
			{
				biped->facing_goal = *g_4687a8;
			}
		}
		biped->unknown34a = function_dd300(*(long *)((byte *)biped + *(short *)((byte *)biped + 0x12a) + 0x7c));
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
void function_1e5a60(void *physics, s_biped_physics_result *result, s_biped_physics_input const *input);
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
// score 42 (was 310): walked against retail block by block (calls, argument order, offsets, constants, conditions,
// case bodies): the only behaviour difference was the final flags bit-7 clear, which retail also runs when the
// biped has no havok component (0xddf46 is the NONE branch's target, 0xdd9bf); it is now outside the if.
// What still differs: crouch sum loads the biped's crouch first (retail 0xddb25 loads crouch_change first; swapping
// operands, `crouch_change += ...` unchanged) and the crouch store comes after the flags `and` (retail before it,
// 0xddb4b/0xddb53; moving the `and` first 52); case 5 stores unknown35c before testing the surface flag (retail
// tests first, 0xddcc8-0xddcd0; a flag local 218, storing it after the if 197); case 4 holds the lookup's address
// in a lea instead of esi (0xddd8e, a holder pointer local unchanged) and orders the unknown364/edi reload
// differently (0xdddbd-0xdddc3); `or byte [0x349], 0x10` is scheduled after the unknown380 loads (retail 0xddeda).
// Fixed here: `bool physics` (long 291 -> bool 253), the permutation byte masked at its use (253 -> 47),
// unsigned key (47 -> 46), `PIN(contact...) == contact` (46 -> 45), i, k, j square sum (45 -> 42), word flag masks.
// Earlier rounds (350 and up): word flags, shared velocity norm, unsigned key, full-width physics result, case
// order (the compiler lays the cases out itself: 1/5/4 and 5/4/1 give the same bytes), clamp admission, owned crouch.
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
			biped->flags_348 |= 0x8000;
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
			/* retail sums i, k, j in that order (0xddad7-0xddaf7) */
			real speed_squared = velocity.i * velocity.i;

			speed_squared += velocity.k * velocity.k;
			speed_squared += velocity.j * velocity.j;
			if (speed_squared > 0.0001f)
			{
				function_b9b90(arg_159e6d, false);
				function_b7360(arg_159e6d);
				function_bba20(arg_159e6d);
			}
		}
		biped->crouch = PIN(crouch_change + biped->crouch, 0.0f, 1.0f);
		biped->flags_348 &= ~0x800;
		physics = function_e4680(arg_159e6d);
		if (havok_component_any_rigid_body_active(component) || physics || !(TEST_FIELD_BIT((biped->flags_c0 >> 6) & 1)))
		{
			biped->flags_348 &= ~0x1000;
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
				function_1e5a60(&biped->physics_mode, &result, &input);
				contact = result.contact_index;
				if (!(*((byte *)biped + 0xc1) & 1) && contact != NONE &&
					PIN(contact, 0, component->unknown88.size) == contact)
				{
					byte *entry = (byte *)component->unknown88.data + contact * 0x48;
					dword key = *(dword *)(entry + 4);
					bool found = false;

					if (key != NONE)
					{
						long index = key & 0xffff;
						long surface = (key >> 16) & 0x1fff;

						switch (key >> 29)
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
								long permutation = (char)(*(byte **)(region + 0xc))[(surface >> 5 & 0xff) * 8 + 5];
								dword position = (((char)region[4] << 8) | (permutation & 0xff)) << 16;
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
					biped->flags_348 |= 0x1000;
					biped->unknown368 = biped->unknown380;
				}
				function_e2fa0(arg_159e6d, result.unknown0c, *(byte **)((byte *)component + 0x70) + 0xc);
				if (result.flags & 1)
				{
					biped->flags_348 |= 1;
				}
				else
				{
					biped->flags_348 &= ~1;
				}
			}
		}
	}
	biped->flags_348 &= ~0x80;
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
real function_1e9700(short row);
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
PRIVATE __forceinline void biped_eye_raise(long arg_159e6d, s_biped *self, byte *self_definition, real crouch,
	point3f *eye)
{
	point3f head;
	vector3f forward;
	vector3f up;
	vector3f direction;
	s_biped_collision collision;

	eye->z += ((1.0f - crouch) * *(real *)(self_definition + 0x218) + *(real *)(self_definition + 0x21c) * crouch) *
		self->scale;
	head = *eye;
	collision.unknown24 = NONE;
	forward = self->unknown168;
	function_11d090(&forward, &up);
	function_df380(arg_159e6d, &head, &forward, &up);
	vector3d_from_points3d(eye, &head, &direction);
	if (function_30bf0(&direction) > 0.0f)
	{
		vector3f back;
		point3f start;

		vector3f_scale(&direction, -0.25f, &back);
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
		if (function_1697c0(0x4808c2d, &start, &direction, NONE, NONE, (s_collision_result_1697c0 *)&collision))
		{
			vector3f to_hit;
			vector3f to_head;

			vector3d_from_points3d(&start, &collision.point, &to_hit);
			vector3d_from_points3d(&start, &head, &to_head);
			if (dot3f(&to_hit, &direction) < dot3f(&to_head, &direction) + 0.05f)
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
		else
		{
			*eye = head;
		}
	}
}

/* a point on a biped: its eye (mode 0 at its origin, 1 standing, 2
   crouching, raised to its head marker unless something is in the way), or
   an offset from a point along a forward vector (mode 3) */
PRIVATE __forceinline void biped_eye_point(point3f *point, long arg_159e6d, short mode, point3f const *origin,
	vector3f const *forward, real const *offsets)
{
	s_biped *self = BIPED_GET(arg_159e6d);
	byte *self_definition = BIPED_DEFINITION_GET(self);
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
		crouch = function_e0dc0(arg_159e6d) ? 0.0f : self->crouch;
		break;
	}
	biped_eye_raise(arg_159e6d, self, self_definition, crouch, point);
}

/* fills the biped's character physics input for its physics mode: walking
   (1), flying (2), dead (3), animation driven (4, 5) or grabbing (6) */
// score 610 (was 612; 635; 1497 before biped_eye_raise was forced inline, 636 before the collision's NONE moved up in biped_eye_raise, 2026-10-31): biped_eye_raise is forced inline (below); the frame, flag homes and argument scheduling still differ.
// Outer-only inline control 1566; native case order 1562; full vector setter 1555;
// use-site animation flags 1578; consistent ground observations 1502; default boost 1563;
// conditional material 1566; workspace names 1566; case/setter/boost/names 1552.
// Ground observations with case order 1497, setter 1510, default boost 1535, case/setter 1509.
// Case order alone is below the eight-point gate; retain it only with the native ground observation cause, plain 1497.
// Logic walk (2026-10-31): every block, call argument and constant agrees with retail (ground velocity and facing,
// the speed_scale paths and 1/137, material, boost, the turning states, the 0x1e5bb0 arguments, cases 1-6 with
// 0x183670, 0xe5d50, the 0.9/0.05/-0.25 steps and the final 0x1e6850). The 880 missing bytes are biped_eye_raise,
// which retail expands here, in 0xde9d0 and in 0xdef60: as __forceinline this function is 651 full (1497), 0xde9d0
// 319 (931) and 0xdef60 287 (995). It is applied (2026-10-31) with biped_eye_raise taking the biped and its
// definition from its caller (the lookups hoisted as in retail's 0xdef60): 0xdc5c0 636, 0xde9d0 336, 0xdef60 279,
// 0xcb500 110 and 0xcb500's caller 0x1e3b00 matches. With the helper doing its own lookups 0xdef60 took its mode
// in DX and tested it before the lookup, which moved 0xcb500 to 129 and unmatched 0x1e3b00. Before the helper took the
// lookups, `default: __assume(0)` (retail's unchecked jump table at 0xdc9ae), `long players` (movsx at 0xdc7cc) and
// the 0xbc test in a bool local gave 633 (643 with the hoisted lookups); the five call arguments in named locals
// (retail's stack slots 0xac/0xa4/0xa0) 651. Without the forced inline those forms give 1497-1534.
// Round 107 (lane 7), kept 635 -> 612: the four definition/flag tests and the byte call arguments through the
// file's field_bit_test (as 0xdd0d0) with `default: __assume(0)` (retail's dead [esp+0x13] stores and 0xfc frame
// appear; alone 644); the rigid body's velocity through the typed hkRigidBody and vector3f_assign (retail's movss
// k/j/i loads; the out-of-line 0x1d09d0 call 693, with a forceinline declaration 693; without this form 647);
// 0xe5930's sibling 0x1e9700 called for the spread (its body was written out: movsx as retail) with the fraction
// multiplied first (fmul 1/137 before the spread); value fix: 0x1e67f0's points are (body, position, velocity)
// by its own bytes. Tried on top: 0x183670 with the biped's component first (retail's call puts biped+0xb4 in EAX,
// 0x183670's component_a; distance is symmetric) 612, not kept (0x183670 is unmatched); gravity through a new
// inline getter 614 (same as the local below); `material` declared last same slot; case 1's rate as
// count * (rate / speed) 612; gravity read into a local before the 0x1e5bb0 call 614 (retail hoists it to [esp+0xac] with the
// byte temps; the whole argument block then is retail's; a one-use local), control bit 3 through field_bit_test
// 644 (retail re-reads +0x148 after 0xe4050), the distance as `length > 0 ? length : 0` (retail's x87 temp and
// select) 629, biped_grab_eye doing its lookups before 0xb9dd0 and reading self->crouch (retail at 0xdcbb0 and
// 0xdeabb) 620 (0xde9d0 339 -> 359), both 635, 0xcb7e0 for the facing (retail inlines it) same code, material as a
// ternary same code, the squares as dy*dy + dx*dx same code, `material` renamed same slot. Still differ: stack slot
// order (material at 0x60 where retail has 0x8c, the case 6 target before position/facing), EDI/EBP swapped for
// the grab target and its root, 0xe0dc0 inlined (retail calls it), case 1's rate computed last (retail first),
// case 4/5 reuses `state` (retail re-derives it after 0x1e5bb0) and constructs c_type_709360 (retail does not:
// 0x1cd510 returns it by value there).
// Round 65 (lane 9): the grab eye through biped_eye_point (0xdef60's body, forced inline) with mode 0, which keeps
// 0xe0dc0 a call as retail (the old biped_grab_eye had it inlined) and does the self/definition lookups before
// 0xb9dd0 as retail: 612 -> 629 alone (self and definition in EBP/EDI where retail has EDI/EBP); with
// biped_eye_raise's hit-first test 632, its vector3d_from_points3d/dot3f test 625, the direction and back vector
// through vector3d_from_points3d and vector3f_scale 610 (0xde9d0 matches). On top: the eye block's locals declared
// radius/height/center/eye 610, center/height/radius/eye 610; the root distance through a `length` local (retail
// stores the root to its own x87 temp, [esp+0x38], before the > 0 select) as a ternary 616 (frame still 0xf8),
// as `distance = 0; if (length > 0) distance = length;` 611 (2828 bytes), with dy*dy + dx*dx 616.
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

	function_b9dd0(arg_159e6d, &position);
	hkRigidBody *rigid_body = havok_component_rigid_body_get(0, component);

	if (!rigid_body->m_fixed)
	{
		vector3f_assign(&ground_velocity, rigid_body->m_motion->m_linear_velocity(0),
			rigid_body->m_motion->m_linear_velocity(1), rigid_body->m_motion->m_linear_velocity(2));
	}
	else
	{
		ground_velocity = *g_4687a4;
	}
	if (field_bit_test(*(dword *)(definition + 0xbc), 11))
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
		if (biped->physics_mode == 1 && field_bit_test(*(dword *)(definition + 0x264), 2) && !airborne)
		{
			byte *globals = *(byte **)((byte *)g_4e034c + 0x134);

			speed_scale = (1.0f - biped->unknown2e4 * *(real *)(globals + 0x78)) * function_dfa60(arg_159e6d);
		}
		if (field_bit_test(*(dword *)(definition + 0x1f0), 6) && !biped->unknown1bc)
		{
			real spread = function_1e9700(8);

			speed_scale = spread * ((dword)arg_159e6d % 0x89 * 0.0072992700f) + 1.0f;
		}
	}
	if (!(biped->control_flags & 1) && field_bit_test(biped->flags_134, 23))
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
		definition + 0x264, 9, field_bit_test(biped->flags_c0, 6), turning, TEST_FIELD_BIT((biped->control_flags >> 3) & 1),
		field_bit_test(*animation_flags, 2), field_bit_test(biped->flags_c0, 8), function_e4050(arg_159e6d),
		field_bit_test(biped->flags_348, 1), biped->unknown39d != 0, g_51e9c4->unknown0, boost, &biped->control, &position,
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

					biped_eye_point(&eye, arg_159e6d, 0, NULL, NULL, NULL);
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
			(s_character_physics_component *)&velocity, animation_id, &body, &position, (point3f *)&velocity);
		break;
	}
	default:
		__assume(0);
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
// score 77 (was 77): Frame 0x28 and entry saves agree; the x87 squares (k, j, i; retail j, k, i), spin.i
// (axis.i loaded first; retail copies the angle and multiplies by axis.i) and the mode-3 test (retail keeps the byte
// in cl: mov cl; movzx eax, cl ... cmp cl, 3) still differ.
// Earlier (score 122 (was 223)): tag/flag carriers, strong-result home, spin scaling and late call/output scheduling.
// Direct up reads 245; test-produced strong 246, both 245; shared vector producers 241;
// complete away copy with direct up 247; linear result with direct up 223 (plain), all forms 253;
// workspace identities 245; linear result alone 241; long strong/spin/both 221/251/245;
// mode-overridden linear result 231; shared real random 223; linear plus vector helpers 218.
// The last five-point increment and long-strong two-point increment are not retained.
// Round 96, kept (942 bytes, retail's length): the flags_10a bit-2 tests through the existing bit view
// (retail's shr/test), the random draw named as spin_fraction (retail multiplies the scale, the magnitude
// and pi/2 separately) and the flag store through the existing word_flags_set. Steps: view on the first
// test only 195, first and third 193, second and third 208, all three 219; each view placement with the
// named draw 184/182/197/214; draw alone 217; `away = *impulse; away.k = 0.0f;` 214, with a long mode local
// in the final test 206 (byte mode 214; the test nested under the parent/+0x13c tests 206); then
// word_flags_set 122 (without the mode local 125, without the away copy 122: both dropped). Without the
// view 212, without the named draw 138. The existing 0xe4c10 call for the ragdoll block 242 (kept out of
// line); `byte *ragdoll` as 0xe4c10 writes it 201/193/220/128; bit-2 tests through a fresh lookup
// 254/284; spin products all angle-first or all axis-first 122/122. Still differ: the seed load and the
// magnitude's x87 interleave, spin product operands, +0x3ec clears through a pointer, mode byte/long loads.
// Logic walk against retail (block by block, 0xde620-0xde9c9): no behaviour difference found. Checked: the
// 0xbc bit-20 early return, strong = |impulse|^2 > 1.96 (0x45e5a8), the 0.5 scale (0x45dbbc) only when
// flags_10a bit 2 is clear, flags_348 |= 2, the cross product up x velocity (0xde763-0xde7a5, signs and
// operands), the 1/65535 and pi/2 constants (0x45dc50, 0x4123ec) and their multiply order, spin = axis * angle,
// spinning = strong || !bit2 (0xde84f-0xde865), the strong block (0xe57e0, then for mode 3 the two byte clears
// at +0x3f8/+0x3f9, 0x1ed340, 0xe18d0 with true), linear = mode != 3 || strong (0xde9c7 sets bl), the
// 0xb7880 argument order and its NULL masks (0xde8cd-0xde8e9), the away test order (parent, +0x13c, modes 4/5,
// +0x34b, strong || mode != 3, |impulse|^2 > 1.96) and 0xb75a0 (eax = 0, ecx = away, edx = up, location and
// flag 0) with its > 0.0 normalize test (0x45dbd8 is 0). All callers/callees still match (0xd9640 the only
// caller). Tried (2026-10-31): the away vector as a whole copy of the impulse then k = 0 (retail copies all three
// dwords at 0xde97c-0xde997 and overwrites the third with xorps) and the +0x3ec block goes through a local pointer
// (retail clears [ecx+0xd], [ecx+0xc] at 0xde885): both make the region identical to retail's, but
// the score reads 132 (not kept) because the metric is positional and the spin-product pair below shifts the
// rest by one line (the align edit blocks fall from 12 to 7). The remaining blocks are: the x87 square order
// (retail j, k, i; every source order of the three terms, a separate squared local, and the order i/j/k
// compile to k, j, i), the spin j/k products (retail loads axis.j/axis.k then mulss by the angle register;
// angle*axis, axis*angle, all-axis-first, vector3f_scale all compile to movaps+mulss mem), and the mode load
// at 0xde90e (mov cl; movzx eax, cl; cmp eax, 4/5; cmp cl, 3: long local 128, byte local 132, nested under the
// parent/+0x13c tests 132, nested long 129, switch on the byte 138 because it becomes a range compare).
// Round 48, kept: the existing cross3f for the axis (spin j/k load the axis first as retail; the spin block regains
// retail's length, 122 -> 95), the away vector as a whole copy of the impulse with k = 0 and the +0x3ec block through
// a pointer as 0xe4c10 writes it (95 -> 81), the long mode local nested under the parent/+0x13c tests as 0xddfb0
// writes it (81 -> 77, plain). Tried: velocity scaled i, k, j 131 (x87 squares as retail, scale/cross colouring
// moves); cross3f with velocity through vector3f_scale 95; angle declared first 122; the long mode local before
// the test 81 (load hoisted above the parent tests); the mode-3 test on the byte field 174 (mode block as retail,
// cross re-coloured; also with `strong ||` first in linear or away declared up front); spin_fraction folded into
// the angle 131 (the two constants fold). Compile-only screens: spin via vector3f_scale, angle initialised at its
// declaration, declaration and statement orders, `spin = axis; spin.X *= angle` never give retail's spin.i.
// Round 39 (fast; a compile-only screen of this file reproduces all three spots and the colour flip): the x87
// square order follows the creation order of velocity's fields (newest first), set by their first direct mention:
// the scale through the existing vector3f_scale 77 (same code), with the magnitude written i, k, j 73 (x87 squares
// as retail; every other term order screened: ijk/jik/jki/kij/kji give kji/kij/ikj/jik/ijk). spin.i follows a
// direct mention of axis.i before the angle (diagnostic `(void)axis.i;` 60, aligned 9: only the mode block left);
// natural forms: vector3f_scale(&axis, angle, &spin) 73, the angle passed as an expression to vector3f_scale 71
// (spin.i and spin.j angle-first), angle/magnitude/fraction declared or initialised in place, spin rows reordered,
// cross3f_staged (screens) keep spin.i. Mode block: retail's `mov cl; movzx eax, cl ... cmp cl, 3` comes only from
// the long local for 4/5 plus the field read for 3 (174; with the probe 178; with the spin expression 179), and on
// cross3f that always re-colours the velocity scale and the cross (xmm2/1/0 -> 0/2/1); the hand-written cross
// keeps the colours (screen) but turns spin to angle-first in all rows and the squares to i, j, k. A byte mode local
// with the field read for 3 90 (narrow compares); byte/char/word/short/dword locals (screens) narrow or keep eax.
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

		if (!(TEST_FIELD_BIT(((s_biped_flags_10a_view *)&biped->flags_10a)->bit2)))
		{
			velocity.i *= 0.5f;
			velocity.j *= 0.5f;
			velocity.k *= 0.5f;
		}
		word_flags_set(&biped->flags_348, 1, true);
		if ((TEST_FIELD_BIT(((s_biped_flags_10a_view *)&biped->flags_10a)->bit2)) || (TEST_FIELD_BIT((*(dword *)(definition + 0x264) >> 3) & 1)) || biped->physics_mode == 2)
		{
			vector3f axis;
			real magnitude;
			real spin_fraction;
			real angle;

			cross3f(g_4687b0, &velocity, &axis);
			function_30bf0(&axis);
			magnitude = (real)sqrt(velocity.j * velocity.j + velocity.k * velocity.k + velocity.i * velocity.i);
			spin_fraction = (real)random_next(&g_4e7408->unknown0) * 1.5259022e-05f;
			angle = spin_fraction * magnitude * 1.5707964f;
			spin.i = angle * axis.i;
			spin.j = axis.j * angle;
			spin.k = axis.k * angle;
			spinning = strong || !(TEST_FIELD_BIT(((s_biped_flags_10a_view *)&biped->flags_10a)->bit2));
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
				byte *ragdoll = (byte *)biped + 0x3ec;

				ragdoll[0xd] = 0;
				ragdoll[0xc] = 0;
				function_1ed340(ragdoll, arg_159e6d);
				function_e18d0(arg_159e6d, true);
			}
		}
apply:
		bool linear = biped->physics_mode != 3 || strong;
		if (linear || spinning)
		{
			point3f position;

			function_b9dd0(arg_159e6d, &position);
			function_b7880(arg_159e6d, NONE, &position, linear ? &velocity : NULL, spinning ? &spin : NULL);
		}
		if (biped->parent_object_index == NONE && biped->unknown13c == NONE)
		{
			long mode = biped->physics_mode;

			if (mode != 4 && mode != 5 && !biped->unknown34b && (strong || mode != 3) &&
				impulse->j * impulse->j + impulse->k * impulse->k + impulse->i * impulse->i > 1.4f * 1.4f)
			{
				vector3f away = *impulse;

				away.k = 0.0f;
				if (function_30bf0(&away) > 0.0f)
				{
					function_b75a0(arg_159e6d, NULL, &away, g_4687b0, NULL, false);
				}
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
	bool result = true;
	long const *self_reference = &arg_159e6d;
	long self_index = *self_reference;
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
		biped_eye_point(&eye, self_index, 0, NULL, NULL, NULL);
		result = false;

		for (i = 0; i < count; i++)
		{
			s_biped_collision collision;
			vector3f vector;

			collision.unknown24 = NONE;
			vector3d_from_points3d(&eye, &center, &vector);
			if (!function_1697c0(0x84000d, &eye, &vector, self_index, target_index,
				(s_collision_result_1697c0 *)&collision))
			{
				result = true;
				break;
			}
			if (count > 1)
			{
				center.z += height / (real)(count - 1);
			}
		}
	}
	return result;
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
		real sprint = 0.0f;
		byte *globals = *(byte **)((byte *)g_4e034c + 0xf4);
		byte *player = g_4e8c24->data + (biped->unknown13c & 0xffff) * 0x21c;
		long delay = vehicle_round_ticks(g_510c54->field_2_3 * *(real *)(globals + 0x20));
		long duration = vehicle_round_ticks(g_510c54->field_2_3 * *(real *)(globals + 0x24));
		long used = vehicle_round_ticks(*(short *)(player + 0x17e) * *(real *)(globals + 0x28));
		long remaining = *(short *)(player + 0x17c) - used;

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

/* a point on a biped (biped_eye_point) */
// score 248 (was 248): Frame 0x8c agrees; first global-load scheduling, the offsets in ECX rather than on
// the stack (ret 0x10 versus 0x14), and the forward/up workspace homes remain.
// Earlier: 248 (was 273; 995 before the helper was forced inline; 279 before the collision's NONE moved up in biped_eye_raise, 2026-10-31): the helper is forced inline; 0xdef60 still compiles with another convention than retail, whose
// own bytes take only `point` in EAX and the other five arguments on the stack (0xcb500's call at 0xcb56c-0xcb571,
// the mode read from [esp+0xa4] at 0xdef9e) with the biped in EDI, its definition in EBX and the index in EBP. Ours
// takes the mode in EBX, the forward vector in EDX and the offsets in ECX (ret 0xc); their registers force a scratch
// register for the point copy, which pushes the definition to a stack slot.
// Tried: the helper doing its own biped/definition lookups 287 (mode in DX, 0xcb500 129, 0x1e3b00 30); only `self`
// hoisted 271 (0xcb500 111, 0x1e3b00 matches); definition passed at the call 288 (0xcb500 122); mode 3 as a separate
// test after the point setup (arg in EBX, mode on the stack) 283 with 0xcb500 122; mode 3 as an else branch 264 with
// 0xcb500 122; offsets in locals 271. The earlier forms with the helper out of line (zero/default/reverse/if-else/
// origin-join/name/zero-default) scored 989-1003.
// Round 65 (lane 9): the body moved into the forced-inline biped_eye_point, which 0xde9d0 and 0xdc5c0 expand with
// mode 0 as retail does (their 0xe0dc0 stays a call only inside the mode switch's default; inside the old
// biped_grab_eye, where the call was unconditional, LTCG inlined it); in biped_eye_raise the second ray's test as
// `dot3f(&to_hit, &direction) < dot3f(&to_head, &direction) + 0.05f` through vector3d_from_points3d, the head
// direction through vector3d_from_points3d, the back vector through vector3f_scale and the collision's NONE before
// the forward copy: 273 -> 248 at retail's 1041 bytes (0xde9d0 matches, 0xcb500 108 =, 0x1e3b00 matches). Steps:
// the inline test written hit-first 275, the vector test 240, the direction too 253, the back vector 248. Still
// differs: retail falls through into the mode-0 0xb9dd0 call and then the crouch switch, and returns `ret 0x14`
// (only `point` in EAX) where ours returns `ret 0x10`; `if (mode == 0)` first 254.
// Round 289: view-forward, basis-up, both, head-direction, forward/direction, up/direction, all three,
// and complete head/hit frame identities all 248 at 1041 bytes. Original helper/body restored; every
// checked shared-helper user and matched caller retained its score.
// @retail 0xdef60
void __stdcall function_def60(point3f *point, long arg_159e6d, short mode, point3f const *origin,
	vector3f const *forward, real const *offsets)
{
	biped_eye_point(point, arg_159e6d, mode, origin, forward, offsets);
}

/* a biped's collision capsule: its bottom point, the vector to its top and
   its radius, from its two capsule nodes when its definition names them */
// @retail 0xdf6c0
void __stdcall function_df6c0(point3f *bottom, vector3f *axis, long arg_159e6d, real *radius)
{
	byte *definition = BIPED_DEFINITION_GET(((s_biped *)object_get(arg_159e6d)));

	if (*(short *)(definition + 0x254) != NONE && *(short *)(definition + 0x256) != NONE)
	{
		transform4x3f const *first_node = object_node_matrix(object_get(arg_159e6d), *(short *)(definition + 0x254));
		transform4x3f const *second_node = object_node_matrix(object_get(arg_159e6d), *(short *)(definition + 0x256));

		if (TEST_FIELD_BIT((*(dword *)(definition + 0x264) >> 1) & 1))
		{
			point3f const *second_point = &second_node->position;
			point3f const *first_point = &first_node->position;

			bottom->x = (first_point->x + second_point->x) * 0.5f;
			bottom->y = (first_point->y + second_point->y) * 0.5f;
			bottom->z = (first_point->z + second_point->z) * 0.5f;
			*axis = *g_4687a4;
		}
		else
		{
			*bottom = first_node->position;
			vector3d_from_points3d(&first_node->position, &second_node->position, axis);
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
	else if (function_ddf60(arg_159e6d))
	{
		hit = function_1697c0(0x800001, &start, &vector, NONE, NONE, (s_collision_result_1697c0 *)&collision);
	}
	else
	{
		hit = function_1697c0(0x84000d, &start, &vector, NONE, NONE, (s_collision_result_1697c0 *)&collision);
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
			bool object_valid = true;
			if (biped->unknown360 != NONE)
				object_valid = function_badc0(biped->unknown360, NONE) != NULL;
			if (!object_valid) function_dfd70(arg_159e6d);
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
	long a5, real radius, bool a7);

/* drops the biped out of a vehicle: level and upright, at a clear spot
   by the vehicle (or anywhere clear) */
// @retail 0xe0070
void function_e0070(long arg_159e6d, long vehicle_index)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	long root_index = function_baf40(vehicle_index);
	long attempt;
	bool success = false;

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
	for (attempt = vehicle_index == NONE; !success && attempt < 2; attempt++)
	{
		long ignore = attempt == 0 ? vehicle_index : NONE;
		point3f center;
		real height;
		real radius;

		function_df5f0(arg_159e6d, &center, &height, &radius);
		success = function_c5460(arg_159e6d, ignore, NULL, 0, 0, radius * 2.0f, 1);
		if (!success)
		{
			s_biped *vehicle = BIPED_GET(vehicle_index);
			point3f point = *(point3f *)((byte *)vehicle + 0x30);
			real vehicle_radius = *(real *)((byte *)vehicle + 0x3c);

			success = function_c5460(arg_159e6d, ignore, &point, 0, 0, vehicle_radius, 1);
		}
	}
}

bool function_1d5120(s_havok_component *component, long rigid_body_index, long a, long b, real rate, long *result,
	long value);
void __stdcall function_1c3770(long object_index, dword flags);

/* steps a dead biped's ragdoll settling (+0x3aa) toward rest */
// score 63 (was 63): EBX input agrees; the extra EBP save, body-index selection and flag-store scheduling remain.
// Native index 108; word flags 87; both 82; signed two-use count with word flags 73;
// native index plus signed count 67, promoted body input 63 kept. Shared short/long getter
// 118/122; explicit short 118; output scope, separate admissions and read-only tag 117;
// shared getter with word flags 89/99; common exit 97; unsigned/native count 87;
// word bitfields 87; named byte 117; byte/count forms 121/115/118; alternative result
// 83; component-first and handle name 67; byte-bitfield forms 103/117/115.
// Settling field at both uses 82; tag receiver at use 64; both 81; settling reads
// with shared promoted/native-short getter 99/89; both reads with those getters 102/100;
// both reads with direct valid-body admission 72. Entry count/convention and flag stores remain.
// Round 281: shared entry/query input-output count 68/73; entry with short body/getter
// 72/87; query with short body/getter 77/105; tag at use on entry/query graphs 69/79.
// All restored; extra EBP and body/flag scheduling remain.
// Round 396: Entry-zero query output with retained/at-use count, shared long/short getter,
// and entry/at-use tag: 91/109/84/101/97/111/93/113. All restored; entry EBP remains.
// Round 171: Paired flag field with retained/native body and retained/fresh count: word 63/73/82/87, byte 103/115/105/117. Extra EBP, validated-index transport and flag scheduling remain; original body and object declaration restored.
// Round 162: the count re-read at the call through a second biped lookup gives retail's byte test and re-read but
// keeps the extra EBP: with the shared getter/explicit body 91 (265 bytes)/68. Signed-char count field, bitfield or
// one-bit-at-a-time flag clears, settlement declared first, short index: no change or worse (harness). Restored.
// Round 312: admitted computed duration with explicit/promoted-shared/native-shared/native-explicit body,
// in-place/named computed offset 66/66/83/98/75/88/66/66. All restored; extra EBP, body transport and flag stores remain.
// Round 49: the extra EBP is the count held in EAX from the test to the call (retail tests the byte
// and re-reads it with movsx after pushing &settled). Screened in an LTCG mini-link (scores shifted
// to the checker's, which read one higher; the second-lookup form checked 68): two-bit word bitfield for bits 13-14 cleared
// with `= 0` 63 (0x9fff still hoisted into EDI on the success path); that with the count re-read
// through a second lookup 68; re-read from the same biped 82 (CSE'd into BL, index on the stack);
// char body with a short selection 73; the existing main-rigid-body getter into short/long 75/72;
// byte view of +0x349 or a two-bit byte bitfield 104/104 (mask hoisted into BL); re-read with char
// body/getter/byte view 76/79/108; a settled flag with one shared final clear 63; the rate named
// before the call (retail computes it first) 72, with the second-lookup re-read 74.
// Round 96: existing short main-rigid-body getter with the count tested as a byte and re-read through a
// second lookup at the call 91 at 265 bytes (EBX input, byte test, re-read and body select as retail; an
// early Havok data load takes EBP and 0x9fff is hoisted into EDI); with 0x1d5120's count declared before
// its result pointer (retail's setup: rate, &settled, count in EAX, rate push) 92 (aligned 21 of 72, from
// 36); that reorder alone 63; getter long/short without the re-read 99/89 (count cached in BL, input moves
// to the stack); re-read without the getter 82; short getter result with re-read 80; rate named first
// 97/105/95; clears through word_flags_set 130, as two masks 91, as a byte clear of +0x349 132/137;
// component re-fetched at the call 90; shared final clear through one goto 92; member-function getter 92.
// Round b6B (retail walk, no logic difference: same callee order 1d5120(component, body, 9, def+0x290+0x20, rate,
// &settled, count in EAX); body test byte >= 0 and < size then movsx ax; rate = [0x510c54]+4 * 0x4407b8 (= 0.15f);
// result stored as a byte; bits 13-14 cleared as `and byte [+0x349], 0x9f` before and after 0x1c3770, and on the
// failed path after the flags_134 |= 0x800000). Structure screen: byte test of the field, count read at the call
// from the same biped 99 (count cached in BL, input on the stack, ret 4; body and flag code as retail), from a second
// lookup 91 (re-read as retail, EBP = early Havok data load; 0x9fff in EDI); that with byte clears on the three
// sites 132, on one/two sites 93-123; char body with short selection 71 (only the failed-path clear is the byte
// form: a single clear narrows by itself, two on a path get the constant hoisted); main-rigid-body getter with a
// short if/else 80. Restored. Hypothesis (untested): retail had no spare callee-saved register, so no hoist and no EBP.
// Round 48 (retail walk: the prologue's EBX input and plain `ret` agree; the byte test, the re-read with movsx at the
// call and the short body select without jump threading are retail's): the existing short main-rigid-body getter with
// the byte test and `(char)biped->unknown3aa` at the call 89 (262 bytes; the byte is cached in DL, the input moves to
// the stack, ret 4); that with a long getter result 99; the count read into a local just before the call 89; one
// condition with the getter at each use and a shared final clear 135; a long count local just inside the first test
// 78. The hypothesis above does not hold: retail leaves EBP unused, so it had a spare register and still re-read.
// Round 65 (lane 9): byte test, short getter and the count re-read through a second lookup 80 (261 bytes), re-read
// from the same biped 89; that with the body in a long local 91 (265 bytes; the body select is then retail's:
// movsx ax / or eax,-1 / movsx edx,ax / cmp edx,-1 without threading); havok_component_get computing the masked
// index first (diagnostic) 91. Lead: retail pushes &settled, then loads the count into EAX, then pushes the rate,
// so 0x1d5120 declares its count before its result pointer; with that order (diagnostic, 0x1d5120 77 unchanged)
// the argument setup is retail's (92 with the long body form; EBP still holds the early Havok data load and 0x9fff
// is hoisted into EDI). 0x1d5120's file was another lane's this round.
// @retail 0xe0c70
void function_e0c70(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);

	long steps = (char)biped->unknown3aa;
	if (steps && biped->havok_component_index != NONE)
	{
		s_havok_component *component = havok_component_get(biped->havok_component_index);
		long body = component->unknown18;
		long rigid_body = body >= 0 && body < component->rigid_bodies.size ? body : NONE;
		long settled;

		if (rigid_body != NONE &&
			function_1d5120(component, rigid_body, 9, *(long *)(definition + 0x290) + 0x20, g_510c54->rate * 0.15f,
				&settled, steps))
		{
			biped->unknown3aa = settled;
			biped->flags_348 &= ~0x6000;
			function_1c3770(arg_159e6d, 0);
			biped->flags_348 &= ~0x6000;
			return;
		}
	}
	biped->flags_134 |= 0x800000;
	biped->flags_348 &= ~0x6000;
}

/* retail calls this out of line from 0x1d43d0 */
__declspec(noinline) bool __stdcall function_e0d70(long arg_159e6d, real *seconds);

/* how long the dead biped's ragdoll has been settling */
// @retail 0xe0d70
bool __stdcall function_e0d70(long arg_159e6d, real *seconds)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	bool result = false;

	if (biped->unknown3aa)
	{
		*seconds = (real)(char)biped->unknown3aa * (g_510c54->rate * 0.15f);
		result = true;
	}
	return result;
}

real function_1ec5f0(void *ragdoll);
void function_1d35d0(s_havok_component *component, long rigid_body_index, real scale);

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
				function_1d35d0(component, i, scale);
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
// score 2 (was 2): the extent call's argument setup is still emitted in the other order (ours
// mov ecx,esi then mov eax,ebx, retail mov eax,ebx then mov ecx,esi); 0x1d1230 itself matches
// retail and takes the index in EAX and the component in ECX, so only the caller's spelling is
// left; retail calls it as a member of s_havok_component, which needs the callee's own file
// (include/unknown_1cec30.h and src/unknown_1cec30.cpp), outside this lane's files; round 162
// found that the member form matches both the caller and the callee there, so the fix only
// waits for that file to be free
// score 2 (was 2): only argument preparation before the extent call differs: mov ecx,esi precedes mov eax,ebx.
// Shared main-index helper with short 34, with long 31; long ternary 76; shared point setter 52; long helper/setter 24;
// typed body getter 61; short helper/setter 27; both typed body helpers/setter 24; typed three-use point source 2, kept; earlier uninitialized index declaration 2.
// Setter result as point argument 2; returned pointer 2; returned reference 2; split extent declaration 2;
// point storage first 2; source pointer declared first 2; initialized point-first declarations 2; const extent result 2.
// Extent formal order reversed 0 but loses its matched callee (1, address base/index); separate noinline and inline
// extent declarations original/reversed 2/0 with the same callee loss; outer early exit, split inner guard, both early exits 2/2/2;
// explicit cdecl and stdcall original/reversed 2/0 with the same callee loss; signed int extent index original/reversed 2/0, same loss.
// Round 162 (harness, both files): the extent query as a member of the component (this first, index parameter),
// written with havok_component_rigid_body_get(index, this) or rigid_bodies[index]: 0 here and the callee unchanged.
// Not applied: the callee's file was held by another lane this round.
// @retail 0xe1a80
void function_e1a80(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);

	if (biped->unknown34b == 1 && biped->havok_component_index != NONE)
	{
		s_havok_component *component = havok_component_get(biped->havok_component_index);
		long rigid_body = havok_component_main_rigid_body_index_get(component);

		if ((TEST_FIELD_BIT((component->unknown04 >> 11) & 1)) && rigid_body != NONE)
		{
			real value = function_1d1230(rigid_body, component);
			point3f const *position = (point3f const *)((byte *)havok_component_rigid_body_get(rigid_body, component)->m_motion + 0x70);
			point3f point;
			real_point3d_set(&point, position->x, position->y, position->z);

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
// Matches with the list's address passed where retail passes it (lane 17 round 5). Earlier, at 6: frame 0x110,
// entry and length 224 agreed; six mask/Boolean/output stack offsets differed.
// Native one-word visited mask 91 kept; root clamp 143, with native mask 100; common result 123;
// declaration-time zero 100, with common result 123; result first 100, with common result 123.
// Lookup/output scope 86 (five-point incremental gain, 11 extra bytes; restored); with root clamp 101;
// mask initialized after admission 96; output-only Boolean 103; result after first placement 122;
// output-only after placement 103. Native mask body retained.
// Round 282: mask before admission, array/scalar with range/clamp and initialized/output-only
// result [89,85,91,70,86,82,80,84]; ending lookup scope on this graph [86,88,83,85].
// One-word mask before admission, root clamp and output-only result 70 plain, kept;
// output receiver is never read by its matched consumer. All original call roles retained.
// Round 396: Shared placement/traversal Boolean with native array/scalar and long/char root,
// direct/assigned return: 65/79/105/103 twice. Shared false-default result with nested/guard
// exits and late/entry initialization: array 61/98/118/121, scalar 49/96/111/119.
// Separate actual function-return default, shared/distinct output, mask names: scalar
// 25/32/25/32, entry defaults 104/106/104/106; original indexed mask 6/13/6/13.
// Unescaped default plus indexed mask and shared placement output 6 plain, kept.
// Consumed lookup scope with array/scalar and shared/distinct output 99/97/103/101.
// Round 402: placement destination before mask/list, after lookup or before root,
// started/placement-succeeded names, late mask declaration and combined native names
// 6 each; list scope through placement/mask with shared output 99/99, distinct output
// 103/103. All restored; six mask/Boolean/output homes remain.
// Round 435: Actual lookup-return receiver with shared/separate traversal output,
// long/char/dword root 77/79/88/90/81/83; long root with entry placement declaration
// 77/79. Original body and calls restored; six stack offsets remain.
// Round 32 (lane 1): MATCH (fast and plain) with 0xe23e0's fourth parameter retyped from bool * to
// s_list_rigid_bodies const * and &list passed (retail pushes the list's address; 0xe23e0 only passes it on).
// 0xe23e0 is a contributor's finished function, so held until its owner OKs the type-only change.
// @retail 0xe1b60
bool __stdcall function_e1b60(long arg_159e6d, point3f const *point, real value)
{
	s_list_rigid_bodies list;
	long count;
	dword visited[1];

	function_181bd0(arg_159e6d, &list);
	count = havok_component_get(BIPED_GET(arg_159e6d)->havok_component_index)->rigid_bodies.size;
	memset(visited, 0, sizeof(visited));
	bool result = false;
	if (PIN(count, 1, 32) == count && list.count > 0)
	{
		long root = list.lists[0];

		if (PIN(root, 0, count - 1) == root)
		{
			bool placement_result = function_e1c40(arg_159e6d, point, value, root, NONE, NONE, 0);
			visited[root >> 5] |= 1 << (root & 0x1f);
			/* retail passes the list's address here; 0xe23e0 only forwards this parameter, and
			   retyping it to the list (held for its owner) would drop the cast */
			result = function_e23e0(arg_159e6d, point, value, (bool *)&list, visited, root, placement_result);
		}
	}
	return result;
}

void function_b8840(long unit_index);
void function_1d1540(s_havok_component *component);

void function_bfa40(long object_index, long a);

/* sets or clears the object's bit 29 (+4) when it has node orientations
   (+0x112); clearing a set bit first rebuilds them from its node matrices */
static inline void biped_orientation_flag_set(long arg_159e6d, bool value)
{
	s_biped *biped = BIPED_GET(arg_159e6d);

	if (arg_159e6d != NONE && *(short *)((byte *)biped + 0x112) != NONE)
	{
		if (value)
		{
			biped->object_flags |= 0x20000000;
		}
		else
		{
			if (TEST_FIELD_BIT((biped->object_flags >> 29) & 1))
			{
				function_bfa40(arg_159e6d, 0);
			}
			biped->object_flags &= ~0x20000000;
		}
	}
}

/* turns the biped into a ragdoll: lifted a little when it fell through
   something, its nodes placed from its Havok bodies */
// Matches (lane 17 round 5) with both flag tests through TEST_FIELD_BIT, the node count as the signed size over
// sizeof(transform4x3f) and one lookup for the tail's flag set. Earlier, at 125: frame 0x18 agreed; header-offset
// spill, biped/component/variant roles and packed result homes differed.
// Boolean word conditions 154; result first 125; both 154; ragdoll/reach names 125;
// conditions/names 154; result/names 125; all three 154; all with indexed nodes 292.
// Shared vector sum 126; native variant record 125, with sum 126; read-only component 125, with
// record/sum/both 125/126/126; reversed sum inputs 125. All restored.
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
	if (TEST_FIELD_BIT((biped->flags_348 >> 9) & 1))
	{
		s_biped *object = BIPED_GET(arg_159e6d);
		dword node_count = *(short *)((byte *)object + 0x114) / sizeof(transform4x3f);
		transform4x3f *nodes = (transform4x3f *)((byte *)object + object->node_matrices_offset);

		*(real *)((byte *)biped + 0x6c) += 0.125f;
		for (; (long)node_count > 0; node_count--, nodes++)
		{
			nodes->position.z += 0.125f;
		}
	}
	function_1c3770(arg_159e6d, 0);
	if (TEST_FIELD_BIT((component->unknown04 >> 11) & 1))
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
		if (arg_159e6d != NONE)
		{
			s_biped *object = BIPED_GET(arg_159e6d);

			if (*(short *)((byte *)object + 0x112) != NONE)
			{
				object->object_flags |= 0x20000000;
			}
		}
	}
	return result;
}

void function_ba350(long object_index, real seconds);
bool __stdcall function_10f430(long unit_index, long field_7c, long state_name, long weapon_name, long action_name,
	real blend, bool flags, long mode);

/* gets a fallen biped up from its ragdoll: back to its getting-up
   animation, Havok asleep, at a clear spot facing along its slide */
// The ragdoll state (+0x34b) goes through a pointer local: written as biped->unknown34b, the compiler sinks its
// clearing store below the flags `and` (score 4). Also needed: the first flag test in a `bool` local (retail loads
// it with the fallen test), the second test re-read through BIPED_GET as `(bool)(x >> 29 & 1)` (a plain `>> 29 & 1`
// becomes `test dword [edi + 4], 0x20000000`), and `facing = velocity` then `facing.k = 0`.
// @retail 0xe18d0
void __stdcall function_e18d0(long arg_159e6d, bool place)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *ragdoll_state = &biped->unknown34b;
	bool fallen = *ragdoll_state == 1;
	bool flagged = (biped->object_flags >> 29) & 1;

	if (flagged)
	{
		real blend = 0.267f;

		biped_orientation_flag_set(arg_159e6d, false);
		function_ba350(arg_159e6d, blend);
		function_10f430(arg_159e6d, 0x7000101, 0x7000101, 0x7000101, 0xd000042, 0.0f, false, 0x210);
	}
	*ragdoll_state = 0;
	biped->flags_348 &= 0xf8ff;
	function_b9b90(arg_159e6d, false);
	function_1c3770(arg_159e6d, 0);
	if (place && *ragdoll_state)
	{
		point3f center;
		real height;
		real radius;
		vector3f velocity;

		function_df5f0(arg_159e6d, &center, &height, &radius);
		function_c5460(arg_159e6d, NONE, NULL, 0, 0, radius * 2.0f, 0);
		function_ba1d0(arg_159e6d, &velocity, NULL);
		if (velocity.j * velocity.j + velocity.i * velocity.i + velocity.k * velocity.k > 1.4f)
		{
			vector3f facing;

			facing = velocity;
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
// score 540 (was 568; the old 774 was stale): the logic was walked against retail block by block and agrees
// (conditions, field offsets and widths, constants 1.0/0.001/0.7/0.65/2.0/0.3, call order and arguments, three
// loops). The flag tests are TEST_FIELD_BIT (retail `shr; test r8, 1` at 0xe1079, 0xe109d, 0xe10a8, 0xe118c).
// Still differs: retail returns through one exit with result on the stack (init 0 at 0xe0f1c, stored 1 at
// 0xe108c/0xe0fa5/0xe0ff6, `mov al, [esp+0x5c]` at every tail, in the argument's home slot), keeps the argument on
// the stack (ebp = [esp+0x58], biped in ebx, frame 0x4c, `ret 4`) and loads +0x34b with movsx (0xe0f11). Retail also
// stores (flags_348 >> 9) & 1 to a byte local [esp+0x13] at 0xe1142 that nothing reads (a dead local or a dead second
// parameter of function_e16b0 is eliminated in ours). Retail walks a pointer in the mass and farthest loops and
// re-reads rigid_bodies.size after the velocity call.
// Tried: one `return result;` exit with `break`s and a `(char)` switch 552 (ours then takes the argument in eax,
// frame 0x50, and 0xdd360 goes 280 -> 281); flags_348 copied to a word local 552; hit position as three reals 568;
// result declared first (same). Earlier lane forms (names, order, exits): single result exit 861, complete
// reciprocal, workspace role names, body count at every use, separate scan ordinals, complete offset vectors.
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
		if (*(long *)(model + 0x24) == NONE || TEST_FIELD_BIT((biped->flags_348 >> 10) & 1))
		{
			return false;
		}
		result = true;
		if (g_5107ed)
		{
			*((byte *)&biped->flags_348 + 1) |= 1;
			return true;
		}
		if (TEST_FIELD_BIT((*(dword *)(definition + 0x1f0) >> 11) & 1))
		{
			return true;
		}
		held = TEST_FIELD_BIT((biped->flags_134 >> 27) & 1) && !TEST_FIELD_BIT((biped->flags_348 >> 9) & 1);
		if (!function_138fa0(!held))
		{
			*((byte *)&biped->flags_348 + 1) |= 2;
			return true;
		}
		{
			long primary = function_10f720(arg_159e6d, true);
			long secondary = function_10f720(arg_159e6d, false);

			if (!TEST_FIELD_BIT((biped->flags_134 >> 27) & 1) && (primary == 2 || secondary == 2))
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

			if (TEST_FIELD_BIT((component->unknown04 >> 11) & 1))
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
			biped_orientation_flag_set(arg_159e6d, true);
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
bool function_1faf80(long arg_159e6d, void const *definition_flight, vector3f const *control, real rate,
	vector3f *facing, real *turn);
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
// score 604 (was 654): the logic now follows retail (see below); frame is 0x78 like retail. Still differs: the
// `special` flag (retail stores 0 before the 0x6000084 test and 1 under the 0x7000039 test; the plain compare gives
// sete, and the flag-set-inside-a-test form scores 571 but is the form review rejects), register assignment and the
// scheduling of the wall-turn vector maths, `or byte ptr [edi+0x348], 0x20` (ours loads al first).
// Behaviour fixed against retail: returns true for physics_mode 6 without a target (0xe0228 stores 1 before the
// mode-6 test); `left` is retail's (sine = up . (facing x forward) > 0, 0xe03e3-0xe0441 and 0xe04c7-0xe04ef) so the 0xa00000f/0x900000e
// choices at 0xe0500-0xe05a7 are no longer swapped, with the turn rotating by -/+sin (0xe05e9-0xe0602, 0xe077b-0xe07ab) and the
// overshoot signs of 0xe0727-0xe0779 and 0xe07c5; the wall turn is a full axis rotation with the (up . forward)(1 - cos) term
// (0xe0654-0xe0708) not just forward*cos + up x forward*sin; the animation rotation is built from
// (turned, forward) (ecx = turned, edx = forward at 0xe0952-0xe0958) and the node count read after it (0xe096a);
// the planar clamp sets turned.k to 0 (0xe08f3), not facing.k.
// Tried: reading the definition at each use (654), split definition reads (667), wall products reordered (no
// change), unsigned `>> 5` for the node count (658-660), `changed = mode != 3` as an initialiser (663), shared
// cross products / unified workspaces / one exit (earlier rounds, 672-720). Retail's `shr eax, 5` on the movsx'd
// short at 0xe097f is not reproduced (ours is sar).
// @retail 0xe01d0
bool __stdcall function_e01d0(long arg_159e6d, long *names)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);
	byte *state = (byte *)biped + *(short *)((byte *)biped + 0x12a);
	bool changed = false;

	if (biped->physics_mode != 3)
	{
		changed = true;
	}
	if (biped->physics_mode != 3 && biped->physics_mode != 6)
	{
		if (biped->physics_mode == 2)
		{
			if (*(long *)((byte *)biped + 0x3ec) <= 0)
			{
				real rate = (biped->control_flags & 8) ? 0.99f : *(real *)(definition + 0x24c);

				function_1faf80(arg_159e6d, definition + 0x2cc, &biped->control, rate, &biped->facing_goal,
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
				sine = (facing.j * biped->forward.k - facing.k * biped->forward.j) * biped->up.i +
					(biped->forward.i * facing.k - facing.i * biped->forward.k) * biped->up.j +
					(facing.i * biped->forward.j - biped->forward.i * facing.j) * biped->up.k;
				cosine = biped->forward.j * facing.j + biped->forward.k * facing.k + biped->forward.i * facing.i;
			}
			else
			{
				facing = biped->facing_goal;
				facing.k = 0.0f;
				if (normalize2d((point2f *)&facing) == 0.0f)
				{
					facing = biped->forward;
				}
				sine = biped->forward.j * facing.i - biped->forward.i * facing.j;
				cosine = biped->forward.j * facing.j + biped->forward.i * facing.i;
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
				vector3f turned = biped->forward;
				real overshoot;

				if (left)
				{
					turn_sine = 0.0f - turn_sine;
				}
				if (wall)
				{
					real along = (turned.j * biped->up.j + turned.k * biped->up.k + turned.i * biped->up.i) *
						(1.0f - turn_cosine);
					real rotated_i = turned.i * turn_cosine + biped->up.i * along -
						(turned.j * biped->up.k - turned.k * biped->up.j) * turn_sine;
					real rotated_j = turned.j * turn_cosine + biped->up.j * along -
						(biped->up.i * turned.k - turned.i * biped->up.k) * turn_sine;
					real rotated_k = turned.k * turn_cosine + biped->up.k * along -
						(turned.i * biped->up.j - biped->up.i * turned.j) * turn_sine;

					turned.i = rotated_i;
					turned.j = rotated_j;
					turned.k = rotated_k;
					overshoot = (turned.j * facing.i - turned.i * facing.j) * biped->up.k +
						(turned.i * facing.k - turned.k * facing.i) * biped->up.j +
						(turned.k * facing.j - turned.j * facing.k) * biped->up.i;
				}
				else
				{
					real turned_i = turned.i * turn_cosine - turned.j * turn_sine;

					turned.j = turned.j * turn_cosine + turned.i * turn_sine;
					turned.i = turned_i;
					overshoot = turned.j * facing.i - turned.i * facing.j;
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
						turned.i = facing.i;
						turned.j = facing.j;
						turned.k = 0.0f;
					}
					function_30bf0(&turned);
				}
				if (biped->unknown13c != NONE && function_bf5d0(arg_159e6d) &&
					0.98f > biped->forward.j * turned.j + biped->forward.i * turned.i)
				{
					transform4x3f rotation;
					byte orientation[0x20];

					matrix4x3_rotation_between_vectors(&rotation, &turned, &biped->forward);
					orientation_from_matrix4x3(&rotation, (rigid_transform_scaled *)orientation);
					if (*(short *)((byte *)biped + 0x10c) >> 5 > 0)
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
matrix3x3 *function_142eb0(matrix3x3 const *right, matrix3x3 const *left, matrix3x3 *out);

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
// score 494 (was 488, after the logic walk of 2026-10-31; it adds 6 points but fixes behaviour): retail's final
// placement subtracts the pivot rotated by the turned axes alone (0xe2260-0xe2312: no scale, no position, with the
// out-alias test `cmp edi,[esp+0x10]` of function_2477b1), so it goes through function_2477b1 instead of a second
// scaled point transform (505). Which pivot is parent follows retail's registers (0xe1d33-0xe1d6f: esi/edi
// outputs of 0x1cff80, equal property -> parent is the edi one; it writes the same point to both, so values agree)
// (494). Verified against retail with no difference: 0x1417b0 call (0xe220c-0xe221c: ecx = delta, edx = back,
// matching `(&rotation, &delta, &back)`), 0x142eb0 register setup, both 0x30bf0 orders, 0x1697c0 argument order,
// 0x168f40 argument order, offset/target/placed/delta sign and operand order, constants (0.025, -1, 0, 1e-6),
// the branch holding each piece, callers 0xe1b60/0xe23e0 (still MATCH). Tried after: scalar x/y/z locals in the
// point transform (549), limit computed after the distance (512).
// Frame 0xfc versus 0x12c; origin/index carriers, pivot-transform
// expansion, native homes and final rotation/translation scheduling differ. Early collision material
// initialization/lifetime 499; with entry false result 499, point setter 504, workspace names 499;
// origin setter alone 486; existing point-transform calls 1108; guarded single result 514;
// limited-scope material initialization 500; full point-difference producers 504;
// late parameter-only travel limit 482 (frame 0x12c, below keep margin).
// Native origin source view 512, with guarded result 527; named limit result 507;
// native origin/limit result/differences 527; explicit existing-helper inline 488,
// with late limit 482; late limit/native origin 496; late limit/native origin/result 512.
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
			parent_pivot = &pivot_a;
			child_pivot = &pivot_b;
		}
		else
		{
			parent_pivot = &pivot_b;
			child_pivot = &pivot_a;
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
				function_142eb0(&rotation.rotation, &matrix.rotation, &matrix.rotation);
				function_30bf0(&matrix.forward);
				function_30bf0(&matrix.left);
				function_30bf0(&matrix.up);
			}
		}
		vector3f rotated_pivot;

		/* retail 0xe2227-0xe2312: only the turned axes carry the pivot (no scale, no position) */
		function_2477b1(&matrix.rotation, (vector3f const *)child_pivot, &rotated_pivot);
		matrix.position.x = target.x - rotated_pivot.i;
		matrix.position.y = target.y - rotated_pivot.j;
		matrix.position.z = target.z - rotated_pivot.k;
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
	bool result = false;

	if (biped->unknown39d > 0 && biped->physics_mode == 3 && (TEST_FIELD_BIT((*(dword *)(definition + 0x1f0) >> 10) & 1)) &&
		!TEST_FIELD_BIT(((s_biped_flags_10a_view *)&biped->flags_10a)->bit2) && biped->unknown39d == 1)
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
		result = true;
	}
	return result;
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
// matches retail: the halved landing time is tested as a byte (AL) and widened to 16
// bits (CX) only inside the branch, so retail's seed pointer load comes between the
// test and the widening.
// Earlier (score 4): the range was widened in CX and the test ran on CL, before the
// seed pointer load. A signed range also deriving the byte
// score 4 (was 4): the range is widened in CX, but ours tests CL before the seed pointer; retail
// tests AL, then loads the seed pointer before widening CX. A signed range also deriving the byte
// test 4; an independent byte 8; an unsigned range 4; explicit byte narrowing 4; a two-view union 4;
// range assigned inside/before the byte test 27/27; long/dword range 16/16; a low-byte mask 4.
// Earlier: retail widens the halved landing time to 16 bits (`movzx cx, al`) before the seed step and sign-extends it at the multiply, ours folds both into `movzx edx, al` (seed then in ecx, not eax); tried: `(short)half` as the argument 27, a named seed pointer 28, `x = x + random_index(...)` 27, a one-use `short` range local 27, a file-local `__forceinline` short-parameter copy of the helper 27.
// Shared bounded-range helper short/byte 60/59; consistent inline/forceinline short 60/4, forceinline byte 27;
// range divided by two 4; short read then halved 4; signed-byte test 4; short positive/nonzero tests 10/10;
// shared bounded-range helper with a short positive test 59.
// Mode receiver owned at guard/entry 77/77, in native admission scope 4;
// final-store narrowing 4, with mode receiver 77; early completion guards 4; staged guards 4;
// consistent object observations 129; role names 4; entry-scope ranges 4.
// One serial 600-second search: twelve attempts, eight scored, none below 4. Original body restored.
// @retail 0xe3f00
void function_e3f00(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);

	biped->unknown39d = (byte)MIN(vehicle_round_ticks(g_510c54->field_2_3 * *(real *)(definition + 0x214)), 0xff);
	function_114b60(NONE, 0x14, arg_159e6d, 0xd, 0);
	if (biped->unknown39d > 1 && TEST_FIELD_BIT((*(dword *)(definition + 0x1f0) >> 10) & 1) && biped->physics_mode != 3)
	{
		if ((biped->unknown39d >> 1) > 0)
		{
			short range = biped->unknown39d >> 1;

			biped->unknown39d += (byte)random_index(&g_4e7408->unknown0, range);
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
// score 91 (was 91): reset-mode dispatch expands instead of retail calling it out of line (113/61 bytes).
// Separate reset noinline declaration 91; Boolean flag test 98; both 98; noinline with
// field-use object lookup 94; Boolean lookup with/without noinline 101/101; both call-boundary
// declarations with Boolean test 98, with field-use lookup 101. Original body/declarations restored.
// Round 282: physical low-byte mask/Boolean/named/early guards 91/91/91/91;
// native word bitfield with Boolean/early guards 98/98; native byte bitfield 91/91.
// Original physical word declaration and caller body restored.
// Round 89: a noinline declaration of 0xe5930 here is ignored (it is defined in another file); one next to
// its definition (src/unknown_0e57e0.cpp, contributor) gives 10 (61 bytes, unit in ESI as retail), and with
// the flag test as TEST_FIELD_BIT this matches (alone that test is 98). A noinline declaration of this
// function then keeps it out of 0xcaf00, which matches too.
// Round 237: reset-inline/first-call-inline/both declarations 91/91/91; queued caller
// noinline alone/with reset/with first/with both 91/91/91/91; all three inline 91.
// All temporary declarations restored; the reset dispatch still expands.
// Round 312: actual post-reset predicate bool/byte/short/long with mutable/read-only object
// receiver 101/101/101/101/104/104/104/104. All restored; reset expansion, entry saves and input home remain.
// Round 49: BLOCKED-BY 0xe5930 (src/unknown_0e57e0.cpp, contributor): our link inlines it here and
// into 0xde4c0 but not into 0xe2d80; retail never does. The known fix (round 89) is a noinline
// declaration beside its definition, which new rules forbid.
// Logic walk (2026-10-31): 0xe4bd0-0xe4c0c agrees with this source: object read before the calls, 0xe18d0(unit, 1),
// 0xe5930(unit), then byte +0xc0 bit 6 (flags_c0 bit 6) clear -> 0xb8890(unit); no behaviour difference.
// Round 107 (lane 7): the test through the file's field_bit_test 98; `#pragma auto_inline(off)` around this function
// (diagnostic, not kept) 91: 0xe5930 is still inlined, the caller file's inline settings do not reach LTCG.
// Also: the biped looked up after the two calls 94; the test as `(flags_c0 & 0x40) == 0` 91; the flag read through a
// fresh lookup at the test 94.
// Round 65 (lane 9): the call is straight-line here, and our LTCG inlines 0xe5930 only at such sites (a call under a
// condition stays a call, as in 0xe2d80, 0xe24f0 and 0xec0d0); diagnostic `if (arg_159e6d != NONE)` around it (not
// retail's, reverted) 7 at 62 bytes, the rest then being the TEST_FIELD_BIT flag test (98 alone). A named
// `asleep` bool read after the calls 98; field_bit_test 98. Still BLOCKED-BY 0xe5930.
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
		byte *ragdoll = (byte *)biped + 0x3ec;

		ragdoll[0xd] = 0;
		ragdoll[0xc] = 0;
		function_1ed340(ragdoll, arg_159e6d);
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
	/* retail keeps this unused argument on the stack */
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
	bool stuck = false;
	long count;

	if (!TEST_FIELD_BIT((biped->flags_348 >> 7) & 1) && biped->physics_mode == 1 && !TEST_FIELD_BIT((biped->flags_134 >> 9) & 1) &&
		!TEST_FIELD_BIT((*(dword *)(definition + 0x1f0) >> 2) & 1) && biped->actor_index != NONE &&
		*(long *)(state + 0x7c) != 0xe0000c3 && biped->unknown399 > function_1469f0(0.5f))
	{
		byte *globals = *(byte **)((byte *)g_4e034c + 0x144);
		point3f ground;

		/* probes 6 units down from the biped for ground */
		stuck = true;
		if (function_df880(NULL, g_4687bc, NONE, arg_159e6d, NONE, 6.0f, &ground, NULL, NULL, NULL, NULL, NULL))
		{
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
				if ((position.z - ground.z) * g_51e9c4->unknown0 * 2.0f + velocity.k * velocity.k < jump * jump)
				{
					stuck = false;
				}
			}
		}
	}
	if (stuck)
	{
		count = biped->unknown3ab + 1;
		if (count > 0xfe)
		{
			count = 0xfe;
		}
	}
	else
		count = 0;
	biped->unknown3ab = (byte)count;
	if (biped->unknown3ab >= vehicle_round_ticks((real)g_510c54->field_2_3))
	{
		long last = biped->unknown38c;

		if (last - 1 != 0 || g_510c54->field_2_3 * 0.5f + (real)last < (real)g_510c54->game_time)
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
__declspec(noinline) long function_10f8f0(long object_index);
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
		TEST_FIELD_BIT(((s_biped_flags_10a_view *)&biped->flags_10a)->bit7);
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
		if (!immune && !TEST_FIELD_BIT(((s_biped_flags_10a_view *)&biped->flags_10a)->bit2))
		{
			s_type_1e6529 data;

			word_flags_set(&biped->flags_348, 15, true);
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
			real spin_fraction = (real)random_next(&g_4e7408->unknown0) * 1.5259022e-05f;
			real spin = spin_fraction * 1.0471976f + 1.5707964f;
			vector3f axis;
			bool found = false;

			if (0.8f > biped->up.k)
			{
				real cross_i = biped->up.j * g_4687b0->k - g_4687b0->j * biped->up.k;
				real cross_j = g_4687b0->i * biped->up.k - biped->up.i * g_4687b0->k;
				real cross_k = g_4687b0->j * biped->up.i - biped->up.j * g_4687b0->i;

				vector3f_assign(&axis, cross_i, cross_j, cross_k);
				found = function_30bf0(&axis) > 0.0f;
			}
			if (!found)
			{
				real angle_fraction = (real)random_next(&g_4e7408->unknown0) * 1.5259022e-05f;
				real angle = angle_fraction * 6.2831855f;

				vector3f_assign(&axis, (real)cos(angle), (real)sin(angle), 0.0f);
			}
			vector3f_add_scaled(&biped->angular_velocity, &axis, spin * g_510c54->rate, &biped->angular_velocity);
		}
		function_e3910(arg_159e6d);
	}
	name = *(long *)(state + 0x7c);
	if (name == 0xa00003e || name == 0xd00003f)
		names[1] = 0xd00003f;
	else
		names[1] = 0x800001e;
}

/* turns a vector about a unit axis (Rodrigues) */
PRIVATE inline void biped_rotate_about(vector3f *v, vector3f const *axis, real sine, real cosine)
{
	real dot = (v->i * axis->i + v->k * axis->k + v->j * axis->j) * (1.0f - cosine);
	vector3f cross;

	cross3f(axis, v, &cross);
	v->i = v->i * cosine + axis->i * dot + cross.i * sine;
	v->j = v->j * cosine + axis->j * dot + cross.j * sine;
	v->k = v->k * cosine + axis->k * dot + cross.k * sine;
}

/* turns a tumbling biped by its angular velocity over a tick, keeping its
   facing and up square */
// score 171 (was 171): the normalize block now agrees with retail but for its xmm numbering (j/k in xmm5/xmm4,
// retail xmm4/xmm3); the frame is 0x30 (retail 0x24: the rotate helper's cross spills) and the rotation/basis SSE
// carriers still differ.
// Earlier (score 184 (was 184)): frame 0x24 and entry saves agreed; axis copy/norm order, early normalization loads.
// Direct rotation output 235; scalar cross terms 246; all shared cross producers 184 (plain);
// shared normalizer 240 (restored grouping); explicit rotation output 251; weighted producers 265;
// scaled-first normalization 234; native axis value 288; vector identities 235;
// opposite cross/subtraction 235 (restored); on shared crosses: norm squares 210,
// scaled-first 184, shared normalization scale 213, all three 208, sine first 220, complement 209.
// Round 408: plain component-index normalization, complete vector role identities, both, native CRT
// axis copy, copy/loop, copy/identities, all three 184/184/184/184/184/184/184; existing complete
// vector assignment producer 210. First copy/norm scheduling and early SSE carriers remain; all restored.
// Round 96: retail inverts the near-zero test (constant-first fcompi, `ja` to the zero arm, normalize as
// the fall-through): `!(0.0001f > fabs)`/`!(fabs < 0.0001f)` give that layout 184/184, `fabs >= 0.0001f`
// 204. Every reassociated sum runs in reverse of retail (function-global order): the length written i, j, k
// gives retail's k, j, i x87 order 181 (j, k, i 187); the rotate helper's dot reversed 210 (214 alone);
// sqrtf/fabsf/cosf+sinf 185/185/184, with the i, j, k length 182 (sqrtf) and 180 (cosf+sinf, 887 bytes).
// Logic walk (2026-10-31), no behaviour difference: against retail 0xe3910-0xe3c86 the offsets (forward 0x70,
// up 0x7c, angular velocity 0x94, game_time_globals rate +4), the constants (1.0, 1e-4, 0.0 at 0x45dbc0/dc/d8),
// the near-zero test (0xe3986 `ja` to speed = 0 with the axis left unnormalized, 0xe39b6), cos/sin/(1 - cos)
// homes, both Rodrigues rotations (0xe3a00-0xe3abe forward; 0xe3ade-0xe3bb2 up, signs and terms), the two
// 0x30bf0 calls (eax = forward at 0xe3ac3, eax = up at 0xe3c44), up = forward x (up x forward) at
// 0xe3bb6-0xe3c3f and the reset of both vectors only when the length == 0.0 (0xe3c49-0xe3c7e) all agree. Dot sum
// order is i,k,j (forward) then k,j,i (up) in retail; any of i,k,j / k,i,j / k,j,i gives the same 184 and
// i,j,k / j,i,k / j,k,i the same 220, also with a separate helper copy for the up rotation.
// Round 48, kept: the existing normalize3d (include/normalize3d.h) in place of its body written out by hand gives
// retail's k, j, i squares, constant-first compare and scaled-first layout (184 -> 176), with the one-use speed local
// folded into the angle 171 at retail's 887 bytes (plain). Tried: speed declared before the angle 176; angle, cosine
// and sine initialised at their declarations 171 (same code); in the rotate helper the cross computed before the dot
// 180 (frame 0x24 as retail, carriers move), scalar cross terms 194, dot written k, j, i or i, j, k 171. Compile-only:
// all 720 orders of the six locals keep frame 0x30.
// Round 39 (fast): retail's rotation is v * cos + axis * dot(1 - cos) - (v x axis) * sin with the cross terms as
// scalars (no cross vector in its 0x24 frame) and keeps the rotated up in registers for the two crosses. In the
// helper: 0x146320's rotate shape (projection written k, j, i, cross j/k locals, cross i inline) 162 (frame 0x24, the
// first rotation nearly retail's; aligned 163 -> 157; not kept: cross3f's body as scalars); 0x170d70's shape (cross i
// local first, projection, cross j/k) 211 (919 bytes), projection first 211; cross3f(v, axis) subtracted 171 (same
// code), computed before the dot 180 (frame 0x24), dot-first products 171. In the body: up and side declared at the
// copy 171; up copied before the first rotation 229 (874 bytes). Lead: the rotate helper with scalar cross terms
// is shared by 0x146320, 0x170d70 and 0x50650's files (each has its own copy, all held this round); one shared
// header helper would let this body use it.
// Round 289: staged rotation/side/up cross subsets 1/2/4/3/5/6/7 give 171/171/180/171/180/180/180;
// all staged plus a native input snapshot 215. Original helper/body restored; frame and SIMD carriers remain.
// @retail 0xe3910
void function_e3910(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	vector3f axis = biped->angular_velocity;
	real angle;
	real sine;
	real cosine;
	vector3f up;
	vector3f side;

	angle = normalize3d(&axis) * g_510c54->rate;
	cosine = (real)cos(angle);
	sine = (real)sin(angle);
	biped_rotate_about(&biped->forward, &axis, sine, cosine);
	function_30bf0(&biped->forward);
	up = biped->up;
	biped_rotate_about(&up, &axis, sine, cosine);
	cross3f(&up, &biped->forward, &side);
	cross3f(&biped->forward, &side, &biped->up);
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
// score 19 (was 19): Logic walk (2026-10-31) against retail 0xe3380-0xe36fd: callees (0xb9dd0, 0xddf60 + 0x1697c0 = the
// inlined 0xdf880 with offset 0.4 (0x45dd30) and length 8.0 (0x45dc18) at 0xe347a-0xe352f, 0x24590, 0x30bf0, 0x1fb7e0 with
// eax type/ecx actor/edx NULL and two pushed NONEs at 0xe36e4-0xe36ec), offsets (0xbc bit 6, 0x12c, 0x12a, 0x7c, 0x244, 0x34c byte,
// 0x38c, 0x88-0x90, 0x84, 0x94), constants (0.5, 2.0, 0.0, 0.3, 0.6, 1.5707964), the sum < time test (comiss/jbe 0xe3459),
// the normal copy over the direction slot (0xe366f-0xe3694) and the 0x54/0x55/0x56 selection all agree; no behaviour
// difference. Frame 0x84 agrees; the actor index for 0x1fb7e0 stays in ECX across the
// 0x24590 call (retail moves it to EDI and back: its callers do not trust 0x24590 to keep ECX), and
// that 2-byte tail difference shifts six jumps to the exit. Round 35: both probes through 0xdf880
// (as 0xe30c0 does) 28 at retail length; the vehicle's 0x34c byte 27; the time comparison written
// sum < time 21; the actor index as a named local 21 (same code); `falling` set before the flight
// vector 19.
// Earlier: byte/word read, float scheduling and normal-copy homes differed.
// First/second/both branch-selected collision calls 215/225/105 fast; both retained.
// On both calls: state at test 105; shared vector/point/both setters 107/110/111; carrier name 105.
// Complete branch-selected calls 105 plain; original operands and helper roles preserved.
// Native short ticks 209; time at each use 122; both 211; entry flight workspace 105,
// with short ticks 209; fallback event result 119, with ticks/workspace 207.
// One-use normal copy 61 at retail length (exact-match-only, restored).
// Round 435: Word/long event carriers 19/19; complete short/long conditionals 19/19;
// fallback defaults 60/60; complementary nested selection 20/20. Original body restored;
// the actor carrier across the magnitude call still differs.
// Round 32: magnitude3d (0x24590) marked inline in src/unknown_023540.cpp 2 (fast and plain; only the
// order of the 0x1fb7e0 argument setup left), but six of its unmatched callers rise (0xf8eb0 946->1030,
// 0xd1e10 761->791, 0x185be0 731->918, 0x1aa0d0 375->402, 0x1d24a0 920->942, 0x2c0d60 899->953), so
// restored; actor index local 19; magnitude compared as `magnitude3d(...) < 1.5707964f` 19; long type 19;
// the type as one ternary 19.
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

		if (time - *(long *)((byte *)biped + 0x244) > ticks * 4 && *((byte *)vehicle + 0x34c) > ticks &&
			(biped->unknown38c == NONE || ticks * 0.5f + (real)biped->unknown38c < (real)time))
		{
			bool falling = false;

			biped->unknown38c = time;
			if (!function_df880(NULL, g_4687bc, NONE, arg_159e6d, NONE, 8.0f, NULL, NULL, NULL, NULL, NULL, NULL))
			{
				vector3f flight;

				falling = true;
				flight.i = vehicle->linear_velocity.i * 2.0f;
				flight.j = vehicle->linear_velocity.j * 2.0f;
				flight.k = vehicle->linear_velocity.k * 2.0f - g_51e9c4->unknown0 * 2.0f;
				if (function_30bf0(&flight) > 0.0f)
				{
					vector3f normal;

					if (function_df880(NULL, &flight, NONE, arg_159e6d, NONE, 8.0f, NULL, &normal, NULL, NULL, NULL,
						NULL) && normal.k > 0.3f)
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
// score 13 (was 13): Frame 0xc agrees; three object lookups read [data + index] where retail reads [index + data],
// the 0x1e5af0 push temp is ecx (retail edx) and the not-found arm keeps names in edx and copies it to ecx (retail
// loads it twice, into eax for the store and ecx for the join).
// Earlier (score 31 (was 158)): retail reloads the result byte at the second block's and the 0x34b block's false
// returns (ours folded them to xor/sete).
// Result first 174; explicit status exits 173; current/primary/tag receivers at uses 182/177/206;
// initial success joined to result 201; current observations/status exits 185; result first too 186.
// Original body and every callee contract restored.
// Round 408: real object ordinal long/dword/word 169/169/179; actual later query result alone/with
// those carriers 201/201/201/203, sequential result 203. Declared animation flags 164; with existing
// biped bit view 158 plain at 617/617, kept. Native ordinal coupled 156/156: additional two-point
// gain rejected; retain the confirmed field graph and all original lookups, calls and result lifetimes.
// Round 96, kept: bit-29 set/clear through the shared inline helper (its own lookup gives retail's re-test
// and read-modify-write), the outer bit-29 tests read through a fresh lookup, the state flag tested as a
// TEST_FIELD_BIT, `if (unknown34b == 1) return true; return result;`. Tried: fresh lookup at the inner
// test only 149; at block 2's test 101, at its store 159; fresh lookup at every use 178/168/176/172;
// result set in the 0x34b block 151/105; state flag through a fresh state read 118; single exit through
// goto for block 2 and/or the 0x34b block 187/195/176/186; result declared first/second/third 34/34/33;
// state declared before the definition 33; `return unknown34b == 1 || result;` 91.
// Logic walk (2026-10-31): every block agrees with retail 0xe3c90-0xe3ef6 (callees and argument order, 0x7000101/
// 0xd000042/0xc000043/0x6000087/0x400000c, the 0x3e88b439 blend, +0xc1 bit 0, +0x11 mask 0xa, +0x6c bit 0, +0x10a bit 2, +0x39d
// unsigned > 0, the 0x1e5af0 order: output in eax, physics in edx, +0x7c in ecx, +0x70 pushed); no behaviour difference.
// Retail returns the result byte at [esp+0x13] reloaded at the 0xe3dc2 tail shared by the block-2 return and the
// 0x34b != 1 return; an address-taken `bool const *result_reference = &result;` local changes nothing (31).
// Round 48, kept: one if/else-if chain with a single `return result` (the first block sets result, the second
// block, the 0x34b block and the rest are its arms): the block-2 and 0x34b != 1 exits share retail's result-byte
// reload (31 -> 13, plain). Tried: an index-first getter (`index = i & 0xffff;` before the table pointer) for the
// four lookups 10 (all three [esi + data] as retail; a new one-caller helper, not kept); the existing object_get
// 13; a named absolute-index local at each lookup 213 (argument moves to the stack); the two zero stores swapped
// 15; `result = true` before the names store 15; the inner if/else inverted 13; result declared first 16; a 600 s
// permuter run (44 tries), none below 13.
// Round 39 (fast): only the top lookup decides the order of all three: the index formed before the table pointer
// there gives retail's [index + data] in all three (13 -> 10): a header pointer local (`header = &headers[i]`,
// `biped = header->biped`) 10, a named absolute index 10 (both one-use, kept only with a match), the index-first
// getter on the inner tests or in the flag helper only 13; BIPED_GET file-wide as an index-first inline getter 10
// (0xdd0d0 and 0xe4450 stop matching). On the header local: the names store in both inner arms 199, `result = true`
// first in the found arm 12, the output zero store first 12, `state->flags & 1` unwrapped 67, physics as a byte
// offset 10, output as `((real *)output)[10]` 10, result declared before state 12, inner if/else inverted 10, the
// blend constant passed directly 13 (10 on the header local; same code). Left
// (10): the push temp and the names registers, a one-step scratch rotation; 0xdd360's 0x1e5af0 call also takes edx
// for the push temp in retail where ours takes the first free register (lead: 0x1e5af0's parameter order; its file
// is held this round).
// @retail 0xe3c90
bool __stdcall function_e3c90(long arg_159e6d, long *names, s_biped_physics_output *output)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);
	s_animation_state *state = (s_animation_state *)((byte *)biped + *(short *)((byte *)biped + 0x12a));
	bool result = false;

	if (function_e4050(arg_159e6d) && !(*((byte *)biped + 0xc1) & 1) &&
		TEST_FIELD_BIT((BIPED_GET(arg_159e6d)->object_flags >> 29) & 1))
	{
		real blend = 0.267f;

		biped_orientation_flag_set(arg_159e6d, false);
		function_ba350(arg_159e6d, blend);
		function_10f430(arg_159e6d, 0x7000101, 0x7000101, 0x7000101, 0xd000042, 0.0f, false, 0x210);
		result = true;
	}
	else if ((state->channels[0].graph_tag_index == NONE || state->channels[0].animation_id.index == NONE ||
		(*((byte *)state + 0x11) & 0xa)) &&
		!(TEST_FIELD_BIT((BIPED_GET(arg_159e6d)->object_flags >> 29) & 1)) && state->unknown7c != 0xd000042)
	{
		biped_orientation_flag_set(arg_159e6d, true);
	}
	else if (biped->unknown34b)
	{
		names[0] = 0x7000101;
		names[1] = 0xc000043;
		((byte *)names)[8] = 0;
		((byte *)names)[9] = 0;
		if (biped->unknown34b == 1)
		{
			result = true;
		}
	}
	else
	{
		if (function_e4050(arg_159e6d) && function_10f340(arg_159e6d, 0x7000101, 0xd000042))
		{
			if (state->unknown7c == 0xd000042)
			{
				function_e3910(arg_159e6d);
			}
			if (TEST_FIELD_BIT(state->flags & 1))
			{
				state->flags &= ~1;
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
		if (!(TEST_FIELD_BIT(((s_biped_flags_10a_view *)&biped->flags_10a)->bit2)) && (TEST_FIELD_BIT((*(dword *)(definition + 0x1f0) >> 10) & 1)) &&
			BIPED_GET(arg_159e6d)->unknown39d > 0)
		{
			names[0] = 0x6000087;
			names[1] = 0x400000c;
			((byte *)names)[8] = 1;
			((byte *)names)[9] = 1;
			result = true;
		}
		else
		{
			((byte *)names)[9] = 0;
		}
	}
	return result;
}

/* starts the biped's landing recovery (+0x34c: 0 soft, 1 hard) from how
   far it fell (its definition's soft and hard landing heights) */
// @retail 0xe4330
void function_e4330(long arg_159e6d, real height)
{
	/* how hard the landing is: the height, less the soft landing height on a
	   soft landing, then as a fraction of the landing's range */
	real severity = height;
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);
	real soft = *(real *)(definition + 0x204);
	real hard = *(real *)(definition + 0x208);
	real maximum = *(real *)(definition + 0x20c);
	real range;
	real time;
	short kind;
	bool forced = TEST_FIELD_BIT((biped->flags_348 >> 6) & 1);
	/* retail keeps forced on the stack */
	bool const *forced_reference = &forced;

	if (!forced && soft > severity)
	{
		return;
	}
	if (forced)
	{
		severity = maximum;
		range = maximum - hard;
		time = *(real *)(definition + 0x200);
		*(byte *)&biped->flags_348 &= ~0x40;
		kind = 1;
	}
	else if (hard > severity)
	{
		range = hard - soft;
		time = *(real *)(definition + 0x1fc);
		severity -= soft;
		kind = 0;
	}
	else
	{
		range = maximum - hard;
		time = *(real *)(definition + 0x200);
		kind = 1;
	}
	if (range > 0.0f)
	{
		severity = PIN(severity / range, 0.0f, 1.0f);
		long ticks = vehicle_round_ticks(g_510c54->field_2_3 * (time * severity));

		if (ticks > 0)
		{
			biped->unknown34c = kind;
			biped->unknown34e = 0;
			biped->unknown34f = (byte)MIN(ticks, 0x7f);
		}
	}
}

bool function_13cb40(void);
void __stdcall function_e4d90(long arg_159e6d, long type, real scale, long a);

/* steps the biped's landing recovery and picks its landing animation */
// @retail 0xe4450
void __stdcall function_e4450(long arg_159e6d, long *names)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	char ticks = (char)++biped->unknown34e;
	char duration = (char)biped->unknown34f;
	bool first = (ticks < duration ? duration : (ticks > 1 ? 1 : ticks)) == ticks;

	if (ticks >= duration)
	{
		function_e4300(arg_159e6d);
	}
	if (biped->unknown34c == 0 &&
		(names[1] == 0xa000014 || names[1] == 0x9000015 || names[1] == 0x9000016 || names[1] == 0xa000017) &&
		(char)biped->unknown34e >= (char)biped->unknown34f >> 1)
	{
		function_e4300(arg_159e6d);
	}
	if (!function_13cb40() && (biped->unknown34e == 1 || first))
	{
		function_e4d90(arg_159e6d, 5, 1.0f, 2);
	}
	if (biped->unknown34c != NONE)
	{
		byte *state = (byte *)biped + *(short *)((byte *)biped + 0x12a);

		if ((*(long *)(state + 0x7c) == 0xd00003f || *(long *)(state + 0x7c) == 0x90006b2) &&
			function_10f340(arg_159e6d, 0x7000101, 0x90006b2))
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
   player), relative to what the biped stands on; an actor may refuse. The
   biped index is read once from the stack argument (ret 4) */
// score 210 (was 210): 640 bytes against retail 683, frame 0x24 versus 0x28. Walked against retail block by block
// (callees, argument order, offsets, constants, condition directions, return values, store paths): behaviour agrees.
// Remaining difference: on the actor path retail (0xe490c-0xe4929) reads the animation state name at
// biped + *(short *)(biped + 0x12a) + 0x7c (the base is computed at entry into EBX, 0xe4799/0xe47af, and kept
// across the whole function) and stores name == 0xa00003e || name == 0xd00003f into the result slot [esp+0x3c]
// (the stack argument's own slot) before 0x1f5560, whose AL overwrites it at 0xe4934 with nothing reading it in
// between: dead in effect, so the 55-byte gap and EBX taken by that base (riding then spills to [esp+0x13], EDI is
// the definition) are not a behaviour difference. Plain forms (expression, if/else, via a local pointer) are compiled
// away (224/229); only `volatile bool result` (174) or writing through the parameter's address (166) keeps it, both
// refused for not matching. The ba1d0 output is its own local `raw`, copied to `velocity` (kicked and passed on, as
// retail's [esp+0x14] copy) and `relative` (retail's re-stored ba1d0 slot [esp+0x20]); earlier: kicking the ba1d0
// output itself with relative a copy (224). Natural parameter (no `reference`) 228; relative as scalars 207
// (without the shared slot, restored). Tried earlier: late success 221 (subthreshold), shared vector producers 224;
// CRT relative copy 224; common result exit 222 (subthreshold); all velocity names 224; late/shared 221;
// natural parameter/common result/shared/CRT 234. All executable forms restored.
// Round 289: long result/contact/both 211/212/211; native up/scalar up/actor observations 202/219/210;
// native up plus long contact 193 (ordinary; rejected while depending on the inherited parameter-address
// receiver), full scalar graph 222. Direct parameter on up/contact 213; direct up with bool/short/byte
// contact 215/210/215; one result exit with bool/long/short contact 210/209/206 (four-point gain rejected).
// All executable forms restored; the 0x24/0x28 frame and overwritten state-base lifetime remain.
// @retail 0xe4770
bool __stdcall function_e4770(long arg_159e6d)
{
	long const *reference = &arg_159e6d;
	s_biped *biped = BIPED_GET(*reference);
	byte *definition = BIPED_DEFINITION_GET(biped);
	real jump;
	vector3f ground;
	vector3f raw;
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
	function_ba1d0(arg_159e6d, &raw, NULL);
	velocity = raw;
	relative = raw;
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
extern real g_47f05c;

/* lands or is knocked to the ground: a crouching biped standing up into a
   ceiling is lifted clear, the hit is remembered (+0x44c..), and a flag
   plays the knocked-down animation */
// score 104 (was 124): frame 0xc and length 422 agree; only the point/handle input registers remain (retail takes
// the point in EAX and the handle in ECX). Earlier: point/index input registers, upward-sum scheduling and cap loading.
// Native word flags, entry point/velocity workspaces, explicit lift arms, separate contact
// admissions, contact-index name, state scope, library point copy and immutable receiver
// all 124; lift initialized from room 121 (subthreshold, restored). Complete upward sum
// and its first pair 124; native overlapping contact fields, native state offset, both,
// both with the upward pair and both with word flags 124. All executable forms restored.
// Handle-first interface orders 97/97/97/97 (retail takes the point in EAX and the handle in ECX); other
// interface orders 124/124/124/124. HELD: the handle-first order (handle, point, contact, knocked) changes the
// call in 0xcaa60, which is in an open contributor PR (#858); apply it with both declarations and both calls
// once #858 has landed or closed.
// Round 198: Complete upward products, doubled extent and actual position-getter result:
// seven subsets and immutable full form all 124; original body, interface and cap source restored.
// Lane 17 round 5: the lift cap read from the global g_47f05c (retail loads it from .data and doubles it at run
// time) 104 at 422 bytes, kept; the upward sum k-first (retail's j*j then k*k then i) 104, kept with it; on that,
// the handle-first order (handle, point, contact, knocked, both declarations and both calls) MATCHES fast and
// plain: still HELD for #858, restored. Named cap local 104; clamp as an if 104.
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
			if (g_4687b0->k * velocity.k + g_4687b0->j * velocity.j + g_4687b0->i * velocity.i > 0.001f)
			{
				byte *definition = BIPED_DEFINITION_GET(biped);
				real room = *(real *)(definition + 0x26c) - *(real *)(definition + 0x270) * 2.0f;

				if (room > 0.0f)
				{
					real lift = room > g_47f05c * 2.0f ? g_47f05c * 2.0f : room;
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
			scale = (char)biped->unknown39f * 0.007874016f;
			scale = PIN(scale, 0.0f, 1.0f);
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
// score 154 (was 168): frame 0xec versus 0x94; material homes, model-count test and effect-call transport remain.
// Logic walk (2026-10-31) against retail 0xe4d90-0xe503b: every callee, argument order (188180 pushes at
// 0xe4eb2-0xe4ee5 and 0xe4ff1-0xe5021, point in eax at 0xe5028 and forward in ecx at 0xe4edf/0xe5022), field offset (0x3dc, 0x58,
// 0x2f8/0x2fc, 0xc0 bit 6, 0x44c, 0x450, 0x28, 0x14, 0x38, 0x78/0x7c/0x5c, 0x1a), constant (0.0328 at 0x47f05c, 0.1
// at 0x45dc68, tag 0x60005ba, query flags 0x84000d), condition direction and the 3-stack-arg/edx-marker callers
// (0xe51c3-0xe5209, 0xe4d6d) agrees with ours; no behaviour difference. Only fix this round: collision.unknown24 = NONE
// before the model scan (retail stores it at 0xe4f43 before the loop test) 168 -> 154. Callers 0xe4450/0xe45e0/0xe4c50/0xe5040 stay MATCH,
// 0xe4770 stays 210.
// BLOCKED-BY 0x188180 (evidence): its 5th stack parameter (+0x6c on entry, 0x1881c0) is pushed unchanged as a dword to 0x1883e0
// (0x1881ed), whose short-index class parameters are already known; here retail writes the material as a WORD (0xe4e70,
// 0xe4eaa, 0xe4f43 and 0x1697c0's +0x24) and pushes a DWORD read of that slot (0xe4ebc, 0xe4ffe), i.e. the parameter is the
// c_type_47f957 class (include/impacts.h), not long. Fast-image test (bipeds.cpp only, the 0x188180 call then unresolved, so
// its register setup is not retail's): material as c_type_47f957 and its prototype changed 686 bytes = retail, score 137;
// with the marker in its own block, the collision NONE early and `down` assigned componentwise after the point offset
// (movss like retail) 121 at frame 0x94; only the unresolved call setup and register homes remain. Applying it needs the
// 188180 definition (unknown_187ec0.cpp) and the declarations in items.cpp, unit_object_type.cpp and unknown_0a76b0.cpp
// changed together (mangled names differ), with 0x1883e0's callers: not done here. In the long form: marker scope 177
// (A), componentwise down 162 (C), B+C 167, A+C 171, A+B+C 175, down struct-copied after the offset with B 179, A+B 173.
// Marker scope/direct location/full-width material combinations 190/168/177/183/189/169/178;
// early collision initialization with all three 169. Query-branch location pointer 168 plain;
// with marker scope/full-width material/both 177/169/178. Keep only query-branch location ownership.
// Round 408: query-time ray with retained/per-use count and indexed/node-pointer scan 174/181/174/181;
// with marker phase scope 183/190/183/190 (frame 0x90). Complete branch-selected material-query
// effect calls on original/scoped-ray graphs 180/165; three-point gain at 701 bytes rejected. Original body restored.
// Round 96: retail keeps the material in a 16-bit stack slot (branch + word store, no cmov) and pushes it
// as a raw dword, so 0x188180's material parameter is 16-bit there (BLOCKED-BY 0x188180, whose file another
// lane holds this round); retail also overlaps the foot marker with the collision record (both at +0x24).
// Marker in its own block scope 177 (frame 0x90); count test as `count - 1 >= 0`/`count > 0`/
// `!(count - 1 < 0)` 173/169/173 (retail's lea/test/jge/jne not reproduced); down vector copied after
// the point offset (retail's place) 174, with the marker scope 183.
// @retail 0xe4d90
void __stdcall function_e4d90(long arg_159e6d, long type, real scale, long marker)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);

	if (biped->physics_mode == 1 && *(long *)(definition + 0x58) != NONE)
	{
		point3f point;
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
			s_location *location = (s_location *)((byte *)biped + 0x28);
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
				collision.unknown24 = NONE;
				for (i = 0; i < count; i++)
				{
					if (*(long *)(*(byte **)(model + 0x7c) + i * 0x5c) == 0x60005ba)
					{
						break;
					}
				}
				point.x += g_4687b0->i * 0.1f;
				point.y += g_4687b0->j * 0.1f;
				point.z += g_4687b0->k * 0.1f;
				if (function_1697c0(0x84000d, &point, &down, NONE, NONE, (s_collision_result_1697c0 *)&collision))
				{
					function_188180(&point, g_4687b0, *(long *)(definition + 0x58), arg_159e6d, type,
						*((byte *)definition + 0x1a), collision.unknown24, (long)g_4687bc, (s_location *)((byte *)biped + 0x28), scale);
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
void function_1c4a80(long object_index, bool a, bool b);
void __stdcall function_b8600(long object_index, long unknown);

/* applies the biped's character physics move: its facing and up, its
   animation names, its movement animation's rate, crouching and grabbing,
   and its new velocity (or, asleep, its new position) */
// score 648 (was 742): the logic was walked against retail block by block and agrees (callees, argument order,
// field offsets, condition directions, constants 0.1/0.15/0.5/0/+-32768/4/6/+-1). The later velocity is
// move.velocity itself: retail adds to it in place (0xe2a2c-0xe2a38, [esp+0x44]), stores it to linear_velocity and
// passes &move.velocity to 0x1c4b00 (0xe2b3b), so the second copy is gone and `velocity` lives only in the matrix
// block, as in retail's frame (move at +0x30, matrix at 0xac). The aim block stores each step first and then clamps
// on `<=`/`>=` (shared tail at 0xe2d4c), with bits 0x80 and 0x100 tested separately.
// Still differs: retail calls 0x1c4a80 at 0xe2c31 and ours inlines it (about 70 bytes; /Ob1 in that function's file
// fixes it, 641 -> 552 fast, but raises 0x1e55d0 319 -> 355 and is a flags change, so not applied); ours caches
// move.flags in ebx, so case 6/4/default each end with their own `call 0xe5930` where retail has one (0xe2869);
// retail keeps NONE in edi (0xe270f); animation_id/state/definition stack slots 0x2c/0x24/0x28 (retail 0x24/0x28/0x2c).
// Tried: the animation lookup as animation_id's initializer (before the names[1] store; retail stores first,
// 0xe26c9) 653; move.flags as a dword (retail reads it whole at 0xe28a9) same score; lean bits as `else if` with the
// crouch test repeated 719; nested with the decay block written twice 787; `do { ... break; } while (false)` 552
// with /Ob1 (554 with the plain `||` form); case order 6 before 4 same. Earlier rounds: default completion
// fallthrough 776; direct still-result ownership 773; common final result with fallthrough 789; coherent workspace
// names 800; declared flag stores 781; entry still Boolean 800; definition at every use 864.
// @retail 0xe24f0
bool __stdcall function_e24f0(long arg_159e6d, long *names, s_biped_physics_output *output)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	byte *definition = BIPED_DEFINITION_GET(biped);
	s_animation_state *state = (s_animation_state *)((byte *)biped + *(short *)((byte *)biped + 0x12a));
	word const *animation_flags = (word const *)((byte *)biped + biped->unknown33e);
	s_biped_physics_move move;
	bool still;
	bool grabbing;
	real change;

	*(byte *)&biped->flags_348 &= ~0x10;
	still = function_ddfb0(arg_159e6d);
	if (still)
	{
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
			vector3f velocity = move.velocity;
			transform4x3f frame;
			vector3f local;

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
		default:
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
	if (!(TEST_FIELD_BIT((*(dword *)(definition + 0x264) >> 1) & 1)) && (function_e4050(arg_159e6d) || grabbing))
	{
		move.velocity.k += (*(real *)(definition + 0x268) - *(real *)(definition + 0x26c)) * g_510c54->field_2_3 *
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

			BIPED_GET(arg_159e6d)->linear_velocity = move.velocity;
			function_1c4b00(arg_159e6d, &move.velocity, NULL, resting);
		}
	}
	else
	{
		s_biped *current = BIPED_GET(arg_159e6d);
		bool sleeping = TEST_FIELD_BIT((current->object_flags >> 8) & 1);
		point3f position;
		point3f *origin = (point3f *)((byte *)biped + 0x64);

		position.x = move.velocity.i * g_510c54->rate + origin->x;
		position.y = move.velocity.j * g_510c54->rate + origin->y;
		position.z = move.velocity.k * g_510c54->rate + origin->z;
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
	if (biped->crouch == 0.0f && *(long *)((byte *)g_4e6948 + 8) != 2 &&
		((biped->control_flags & 0x80) || (biped->control_flags & 0x100)))
	{
		if (biped->control_flags & 0x80)
		{
			biped->unknown3a4 -= g_510c54->rate * 4.0f;
			if (biped->unknown3a4 <= -1.0f)
			{
				biped->unknown3a4 = -1.0f;
			}
		}
		else
		{
			biped->unknown3a4 += g_510c54->rate * 4.0f;
			if (biped->unknown3a4 >= 1.0f)
			{
				biped->unknown3a4 = 1.0f;
			}
		}
	}
	else if (biped->unknown3a4 > 0.0f)
	{
		biped->unknown3a4 -= g_510c54->rate * 6.0f;
		if (biped->unknown3a4 <= 0.0f)
		{
			biped->unknown3a4 = 0.0f;
		}
	}
	else if (0.0f > biped->unknown3a4)
	{
		biped->unknown3a4 += g_510c54->rate * 6.0f;
		if (biped->unknown3a4 >= 0.0f)
		{
			biped->unknown3a4 = 0.0f;
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

/* whether a grab measurement (yaw, pitch, distance; function_e5d50) is
   inside the grab window: closer than 2.0 (flag) or 1.2, the yaw within a
   limit that narrows from 50 to 15 degrees with the distance, the pitch
   within 30 degrees */
// @retail 0xe63b0
bool function_e63b0(vector3f const *aim, bool flag)
{
	real distance = aim->k < 0.0f ? 0.0f : aim->k;
	real range = 2.0f;
	bool result = false;

	if (!flag)
	{
		range = 1.2f;
	}

	if (distance < range)
	{
		real t = distance * 0.8333333f;
		real limit = (1.0f - t) * 0.87266463f + t * 0.2617994f;

		result = PIN(aim->i, 0.0f - limit, limit) == aim->i &&
			PIN(aim->j, -0.5235988f, 0.5235988f) == aim->j;
	}
	return result;
}

/* measures a target from the biped's grab eye, against a reference
   direction: the yaw and pitch (radians) of the direction to the target, and
   the gap between the two bodies, not less than 0 */
// score 406 (was 449): The logic matches retail (walked block by block, constants read from the XBE, no behaviour
// difference found). The reference frame is a matrix3x3 (forward, left, up): retail's frame is 0xc8, the 36 bytes
// being that 36-byte object at +0x58 (a 12-byte local per axis overlaps the eye's locals and gives 0xa4). Still
// differs: the other locals' slot order (retail head +0x10, to_head +0x1c, eye +0x28; ours packs the 4-byte
// definition and distance slot in front) and the operand order of the first ray's start (retail loads back.i, then
// multiplies by t). Tried, no change: locals at function scope (449), the cross product as an inline helper (452),
// permuted declaration order (406), t * back.i (406); biped_grab_eye as the helper stays out of line (1214).
// @retail 0xe5d50
void __stdcall function_e5d50(long arg_159e6d, long target_index, vector3f *offset, point3f *point)
{
	point3f center;
	real height;
	real radius;
	point3f eye;
	point3f target;
	real target_height;
	real target_radius;
	vector3f direction;
	real distance;
	s_biped_header *header;

	function_df5f0(arg_159e6d, &center, &height, &radius);

	/* biped_grab_eye, written out: the build keeps that helper out of line,
	   and retail has it inline here. Retail still calls function_e0dc0 here,
	   where the build inlines it */
	{
		s_biped *self = BIPED_GET(arg_159e6d);
		byte *self_definition = BIPED_DEFINITION_GET(self);
		real crouch;
		point3f head;
		vector3f forward;
		vector3f up;
		vector3f to_head;
		s_biped_collision collision;

		function_b9dd0(arg_159e6d, &eye);
#pragma inline_depth(0)
		crouch = function_e0dc0(arg_159e6d) ? 0.0f : self->crouch;
#pragma inline_depth()
		eye.z += ((1.0f - crouch) * *(real *)(self_definition + 0x218) + *(real *)(self_definition + 0x21c) * crouch) *
			self->scale;
		head = eye;
		forward = self->unknown168;
		collision.unknown24 = NONE;
		function_11d090(&forward, &up);
		function_df380(arg_159e6d, &head, &forward, &up);
		to_head.i = head.x - eye.x;
		to_head.j = head.y - eye.y;
		to_head.k = head.z - eye.z;
		if (function_30bf0(&to_head) > 0.0f)
		{
			vector3f back;
			point3f start;

			back.i = to_head.i * -0.25f;
			back.j = to_head.j * -0.25f;
			back.k = to_head.k * -0.25f;
			if (function_1697c0(0x4808c2d, &eye, &back, NONE, NONE, (s_collision_result_1697c0 *)&collision))
			{
				real t = collision.t * 0.9f;

				start.x = back.i * t + eye.x;
				start.y = back.j * t + eye.y;
				start.z = back.k * t + eye.z;
			}
			else
			{
				start = collision.point;
			}
			if (function_1697c0(0x4808c2d, &start, &to_head, NONE, NONE, (s_collision_result_1697c0 *)&collision) &&
				(head.z - start.z) * to_head.k + (head.y - start.y) * to_head.j + (head.x - start.x) * to_head.i +
				0.05f > (collision.point.z - start.z) * to_head.k + (collision.point.y - start.y) * to_head.j +
				(collision.point.x - start.x) * to_head.i)
			{
				eye.x = collision.point.x - to_head.i * 0.05f;
				eye.y = collision.point.y - to_head.j * 0.05f;
				eye.z = collision.point.z - to_head.k * 0.05f;
			}
			else
			{
				eye = head;
			}
		}
	}

	header = &((s_biped_header *)g_4e0300->data)[target_index & 0xffff];
	if ((1 << ((byte *)header)[3]) & 1)
	{
		function_df5f0(target_index, &target, &target_height, &target_radius);
	}
	else
	{
		point3f nearest_a;
		point3f nearest_b;
		real gap;

		if (function_183670(BIPED_GET(arg_159e6d)->havok_component_index, header->biped->havok_component_index,
			&nearest_a, &nearest_b, &gap))
		{
			target = nearest_b;
		}
		else
		{
			function_b9dd0(target_index, &target);
		}
		target_height = 0.0f;
		target_radius = 0.0f;
	}
	target.z = PIN(eye.z, target.z, target.z + target_height);
	direction.i = target.x - eye.x;
	direction.j = target.y - eye.y;
	direction.k = target.z - eye.z;
	distance = (real)sqrt(direction.k * direction.k + direction.j * direction.j + direction.i * direction.i);
	if (!(fabs(distance) < 0.0001f))
	{
		real inverse = 1.0f / distance;

		direction.i = inverse * direction.i;
		direction.j = direction.j * inverse;
		direction.k = direction.k * inverse;
	}
	else
	{
		distance = 0.0f;
	}
	point->z = distance;
	if (distance == 0.0f)
	{
		point->y = 0.0f;
		point->x = 3.1415927f;
	}
	else
	{
		matrix3x3 frame;
		real forward_amount;
		real up_amount;
		real left_amount;

		point->z = distance - (target_radius + radius);
		point->z = point->z < 0.0f ? 0.0f : point->z;
		frame.forward = *offset;
		function_11d090(&frame.forward, &frame.up);
		frame.left.i = frame.up.j * frame.forward.k - frame.up.k * frame.forward.j;
		frame.left.j = frame.up.k * frame.forward.i - frame.up.i * frame.forward.k;
		frame.left.k = frame.up.i * frame.forward.j - frame.up.j * frame.forward.i;
		forward_amount = direction.k * frame.forward.k + direction.j * frame.forward.j + direction.i * frame.forward.i;
		up_amount = direction.k * frame.up.k + direction.j * frame.up.j + direction.i * frame.up.i;
		left_amount = direction.k * frame.left.k + direction.j * frame.left.j + direction.i * frame.left.i;
		point->x = (real)atan2(left_amount, forward_amount);
		point->y = (real)atan2(up_amount, sqrt(left_amount * left_amount + forward_amount * forward_amount));
	}
}

/* chooses the unit's grab target between a candidate and the current one
   (NONE for none): the one inside the grab window (function_e63b0), and when
   both are, the one with the smaller yaw when both are close, else the
   nearer; NONE when neither is */
// score 112 (was 112): walked against retail block by block, no behaviour difference: the weapon slot byte at
// +0x212 and the +0x218 table (0xe5be1-0xe5bfd), the flag inlined from function_cbd50 and function_1012c0
// (definition byte +0x12f, bit 0; 0xe5c04-0xe5c34), previous == NONE at 0xe5c37, both function_e5d50 calls (object
// first, then previous, offset = unit + 0x168, out pushed first; 0xe5c4d, 0xe5c83, 0xe5c95), function_e63b0 on
// candidate then current (0xe5ca0, 0xe5cad), the 0.6 constant at 0x44ae8c, the gap test (+8, 0xe5cc6-0xe5cda), the
// yaw magnitudes (+0, 0xe5cdc-0xe5d0a; the candidate wins only when |candidate yaw| < |current yaw|, a tie
// keeps previous via 0xe5d36) and the three returns (candidate: 0xe5cbc, 0xe5d0f, 0xe5d23; previous: 0xe5d36; NONE:
// 0xe5d3a) all agree; the caller 0x1515e0 (0x15165a-0x15166c) passes (unit, object, previous) and stores the
// result. Remaining difference is allocation: retail keeps object_index in EBP and the result in the argument's
// home slot ([esp+0x28], stored as imm -1 at 0xe5bf0), reloads [esi+0x44] for the weapon lookup (0xe5c0b) and tests
// the first function_e63b0 result from its spill (0xe5cb2); ours holds -1 in EBP as the result, caches the data
// pointer in ESI and calls function_e63b0 on current first. Tried: result as a reassigned parameter with the flag
// before the unit lookup (391 bytes, 120), flat && conditions (127/132), inverted nesting (136/140), calls inside
// the conditions (114, wrong: the second call becomes conditional).
// @retail 0xe5bc0
long __stdcall function_e5bc0(long unit_index, long object_index, long previous_index)
{
	s_biped *unit = BIPED_GET(unit_index);
	long result = NONE;
	bool flag = function_1012c0(function_cbd50(unit_index, *((char *)unit + 0x212)));

	if (previous_index == NONE)
	{
		vector3f measurement;

		function_e5d50(unit_index, object_index, &unit->unknown168, (point3f *)&measurement);
		if (function_e63b0(&measurement, flag))
		{
			result = object_index;
		}
	}
	else
	{
		vector3f candidate;
		vector3f current;
		bool candidate_inside;
		bool current_inside;

		function_e5d50(unit_index, object_index, &unit->unknown168, (point3f *)&candidate);
		function_e5d50(unit_index, previous_index, &unit->unknown168, (point3f *)&current);
		candidate_inside = function_e63b0(&candidate, flag);
		current_inside = function_e63b0(&current, flag);
		if (candidate_inside)
		{
			if (current_inside)
			{
				if (candidate.k < 0.6f && current.k < 0.6f)
				{
					real candidate_yaw = candidate.i >= 0.0f ? candidate.i : 0.0f - candidate.i;
					real current_yaw = current.i >= 0.0f ? current.i : 0.0f - current.i;

					if (candidate_yaw < current_yaw)
					{
						result = object_index;
					}
					else
					{
						result = previous_index;
					}
				}
				else if (candidate.k < current.k)
				{
					result = object_index;
				}
				else
				{
					result = previous_index;
				}
			}
			else
			{
				result = object_index;
			}
		}
		else if (current_inside)
		{
			result = previous_index;
		}
	}
	return result;
}
