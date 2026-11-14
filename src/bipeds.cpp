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
#include "object_tag_definitions.h"
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
struct s_biped_flags_348
{
	word field_0 : 1;
	word field_1 : 1;
	word field_2 : 1;
	word field_3 : 1;
	word field_4 : 1;
	word field_5 : 1;
	word field_6 : 1;
	word field_7 : 1;
	word field_8 : 1;
	word field_9 : 1;
	word field_a : 1;
	word field_b : 1;
	word field_c : 1;
	word field_d : 1;
	word field_e : 1;
	word field_f : 1;
};

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
	union
	{
		word flags_348;
		s_biped_flags_348 flags_348_bits;
	};
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
/* the same definition, typed (object_tag_definitions.h): a biped's, a unit's
   (the vehicle a biped rides) or any object's */
#define BIPED_DEFINITION(biped) ((s_biped_definition *)BIPED_DEFINITION_GET(biped))
#define UNIT_DEFINITION(unit) ((s_unit_definition *)BIPED_DEFINITION_GET(unit))
#define OBJECT_DEFINITION(object) ((s_object_definition *)BIPED_DEFINITION_GET(object))

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
// MiniMax G (11-14): no new form, no lower score (118): the reset dispatch still expands at this call site, so the
// body is 413 bytes against retail's 343, and the 0x1f0 bit test still folds to a byte test where retail shifts
// the dword first: BLOCKED-BY 0xe5930.
// MiniMax G (11-13): no new form, no lower score (118): the reset dispatch still expands at this
// call site (413 bytes against retail's 343) and the 0x1f0 bit test folds to a byte test where
// retail shifts the dword first: BLOCKED-BY 0xe5930.
// MiniMax A (11-13): no new form, no lower score (118): the reset dispatch still expands here where retail calls 0xe5930 out of line, so the body is 413 bytes against retail's 343; a switch or a do-while(0) wrapper around the call does not keep it out of line, and the callee's own file (src/unknown_0e57e0.cpp) is not this lane's: BLOCKED-BY 0xe5930.
// MiniMax B (11-12): no new form, no lower score (118): the definition flag test wrapped in TEST_FIELD_BIT or written
// as a ternary measures 121 (417 bytes) and the flag tested as a mask measures 118 (413 bytes), the same as here; this
// build inlines 0xe5930 (defined in src/unknown_0e57e0.cpp) where retail calls it, so the body is 413 bytes against
// retail's 343, and the noinline for 0xe5930 is a change in another file.
// MiniMax S (11-11): no new form, no lower score (118): the body still matches retail's first 0x10a bytes and is 70
// bytes longer, the extra bytes all coming from the reset dispatch expanding at this call site, and the 0x1f0 bit test
// is still folded to a byte test where retail shifts the dword first; the reset helper's own file (src/unknown_0e57e0.cpp)
// is not this lane's: BLOCKED-BY 0xe5930.
// MiniMax R (11-10): no new form, no lower score (118); the reset dispatch still expands at this straight-line call site and the 0x1f0 bit test is still folded to a byte test where retail shifts the dword first, and the reset helper's own file is not this lane's.
// MiniMax I (11-10): no new form, no lower score (118); the reset dispatch still expands here and the 0x1f0 bit test is still folded to a byte test where retail shifts the dword first; the reset helper's own file is not this lane's.
// MiniMax E (11-08): no new form, no lower score (118); the reset call still expands here instead of staying a call, and the 0x1f0 bit test is still folded to a byte test where retail shifts the dword first; the reset helper's own file is not this lane's.
// MiniMax G (11-07): `if (biped != NULL)` around the reset call (always true: biped is written just above) keeps
// 0xe5930 out of line, 9; not kept (an always-true test that only steers inlining).
// MiniMax D (11-06, run 49): no new form, no lower score (118); 0xe5930 (src/unknown_0e57e0.cpp, not this lane's file) still expands here, and the 0x1f0 bit test is folded to a byte test where retail shifts the dword first.
// MiniMax A (11-04): no new form, no lower score; the reset call still expands here.
// MiniMax A (11-03): no new form, no lower score: 0xe5930 (src/unknown_0e57e0.cpp, not my file) still
// expands here, so the reset dispatch keeps its extra instructions.
// MiniMax R (11-01): no new form, no lower score: 0xe5930 (src/unknown_0e57e0.cpp, not my file) still expands here, so the reset dispatch keeps its extra instructions.
// MiniMax E (11-11): no new form, no lower score (118): the flag store scheduling and the final helper's expansion remain; the helper 0xe5930 is defined in another file and a new noinline declaration is not allowed: BLOCKED-BY 0xe5930.
// MiniMax H (11-12): no new form, no lower score (118): the reset dispatch still expands here where retail calls it, and the dispatch's own file is not this lane's to edit.
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
// Round 192: the set/clear as word_flags_set with TEST_FIELD_BIT/field_bit_test 113/113 (415 bytes, all before the
// 0xe5930 call retail's); field_bit_test with the byte-cast stores 121, with plain word `|= 8`/`&= ~8` 113; the plain
// word stores alone 114, with TEST_FIELD_BIT 113. Best gain 5 (under the bar), restored; still BLOCKED-BY 0xe5930.
// Round 65 (lane 9): why 0xe5930 is inlined here: our LTCG inlines it at straight-line call sites and keeps it a
// call where the call sits under a condition (0xe2d80, 0xe24f0's switch, 0xec0d0), as it does with 0xe0dc0 (0xde9d0
// matched once that call moved into a switch default); retail never inlines it. Diagnostic `if (arg_159e6d != NONE)`
// around the call (not retail's, reverted): 21 at 346 bytes. The flag through word_flags_set and TEST_FIELD_BIT 113;
// TEST_FIELD_BIT with word-wide |= 8 / &= ~8 113; the mode stored before the index 114; word_flags_set with
// field_bit_test and the call's index read back from +0x3e0 131. BLOCKED-BY 0xe5930 (contributor file): the fix
// belongs to its definition.
// Round 214 (m3 lane 3, fast): on this function TEST_FIELD_BIT 121, the bit member assigned 139 (427 bytes), word_flags_set with TEST_FIELD_BIT 113 [28]
// at 415 (under the bar). Diagnostics on 0xe5930's body, restored: no mode local, switch cases with returns, six copies of the mode, the arm's biped or
// state named, a forced-inline getter one or two levels deep, a forced-inline arm: 118 each, 0xe5930 still matching and still inlined here; an if chain
// 121 (0xe5930 23), an explicit case 0 124 (27), the biped named at entry 110 (35). No spelling of its body keeps it out of line. Still BLOCKED-BY 0xe5930.
// MiniMax R (11-11): no new form tried this round, no lower score (118): the body matches retail's first 0x10a bytes and is 70 bytes longer overall, the extra bytes all coming from the reset helper expanding at this call site.
// @retail 0xde4c0
void __stdcall function_de4c0(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	s_biped_definition *definition = BIPED_DEFINITION(biped);
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
	if ((definition->biped.flags >> 1) & 1)
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
void function_1e5af0(void *physics, vector3f const *forward, vector3f const *up, s_biped_physics_output *output);
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

// MiniMax G (11-14): no new form, no lower score (222): retail computes the biped's data pointer before the early
// exit and keeps it in a frame slot while this build computes it at its use, and the state pointer's spill slot
// and the early exit's epilogue still differ: BLOCKED-BY 0x111650.
// MiniMax G (11-13): no new form, no lower score (222): the state pointer is spilled to
// [esp+0x20] where retail spills it to [esp+0x24], retail reads the parent's index before the
// 0xbaf40 call where this build reads it after, the early exit does not share retail's epilogue,
// and the 0x111650 call takes the names in eax where retail pushes both: BLOCKED-BY 0x111650.
// MiniMax B (11-13): no new form, no lower score (222): the 0x111650 call still takes the names in eax where retail pushes both arguments, and that callee (src/unknown_10f1e0.cpp) is not in this lane's files: BLOCKED-BY 0x111650.
// MiniMax A (11-13): no new form, no lower score (222): the first difference is the slot the state pointer is spilled to (this build's [esp+0x20] against retail's [esp+0x24], the same frame size), and retail reads the parent's index before the 0xbaf40 call where this build reads it after; moving a local to shape the frame is not allowed.
// MiniMax C (11-11): no new form, no lower score (222); retail computes the state pointer up front and spills it at [esp+0x20] with the biped index at [esp+0x24], ours spills the index there and reads the parent through the parent-object helper, and retail pushes EBX after its early-exit test and shares one epilogue where ours has its own.
// MiniMax E (11-11): no new form, no lower score (222); retail loads the biped's havok index into a register and
// compares it there, computes the biped-plus-short-offset pointer early and spills it at [esp+0x20], and jumps to
// the shared exit for its early return, where ours compares the field in memory, spills the object index at
// [esp+0x20] where retail uses [esp+0x24] and returns inline.
// MiniMax H (11-12): no new form, no lower score (222): the state pointer is still rematerialized instead of spilled to its stack home, and retail's early exit still jumps to a shared epilogue this build does not have.
// score 222 (was 224; 241; 280 before round 48): frame 0x16c agrees; retail computes the state pointer at the top and spills it (ours reads it at
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
// Round 192, kept (241 -> 224): the final +0x10a bit-2 test through the bit view (retail's `shr cl, 2; test cl, 1` at
// 0xdd725; 235 alone) and the case-1 `else if` bit-1 test through TEST_FIELD_BIT (retail's `shr al, 1; test al, 1` at
// 0xdd704; 231 alone). Tried: that bit-1 test through TEST_FIELD_BIT or field_bit_test at the first site 270, at both sites 266,
// both sites with the view 259; the type-0 names test through the view 288, through TEST_FIELD_BIT 235/224; the
// guarded body with one exit 312 again (with the view 306, aligned 68 of 306: the head is then retail's but for the
// state's three instructions), with the state local after the parent lookup/after `changed`/after `names`/after the
// biped 306/311/311/310 (state in EBX, `changed` in [esp+0x13]), that with `changed` declared first 337. Remaining:
// the first two flags_348 tests share a cached word in CX and stores of DL = 0 (retail tests memory, stores
// immediates), the early exit's own epilogue, the state spill and the 0x111650 call (BLOCKED-BY 0x111650).
// Round 65 (lane 9): one `return changed` with the body inside the Havok test and `changed |= 0x111650(...) | action`
// 312 (1092 bytes); that with the state at the top 303 (1126 bytes; aligned 75 against 91 for this form, but the
// state takes EBX at entry and `changed` a stack byte: the earlier-defined value gets the register); with
// `changed |= function_e2d80(...)` 303; the exit through a separate `result` with `changed` declared at the
// 0xe2d80 call 301. Retail keeps `changed` in BL and spills the state, so `changed` exists before the state there.
// M3 lane 13 r74 (fast with 0xe4c50/0xde190): on today's base `changed |= function_111650(...) | action; return
// changed;` 222 (masked aligned 65 -> 64); the bit-0, bit-1 and both flags_348 tests through the existing bitfield view
// 222/222/222 (same code); the early exit returning `changed` 222 (same code). Ours still caches flags_348 in CX and 0
// in DL for the counter clears where retail tests memory and stores immediates (the constant hoisting of 0xe0c70's
// 0x9fff).
// MiniMax B (11-11): no new form, no lower score (222): the one form that helps is the names array's second
// entry instead of the first, which scores 221 and is dropped as too small a gain; what still differs is that
// retail computes the state pointer and spills it to its stack home before the early-exit test while ours
// rematerializes it, retail's type-0 bit test shifts a byte and masks it where ours masks the bit directly,
// retail tests the flags word in memory where ours caches it in a register, and the tail's or-chain differs
// because of the 0x111650 return convention.
// MiniMax I (11-10): no lower score (222 kept); a state local computed at the top of the function scores 322 and a
// single-exit goto form 301, so both are worse than the base. What still differs: retail computes the state pointer
// (the `movsx`/`add` pair) up front and spills it at [esp+0x20] with the biped index at [esp+0x24], ours spills the
// index at [esp+0x20] and reads the parent through the parent-object helper where retail reads the parent's index
// field directly; retail also pushes EBX after its early-exit test and shares one epilogue, ours has its own.
// MiniMax B (11-11), NOT kept (coordinator): 222 -> 210 moves `changed = true;` above the other-object test, a statement
// order picked by score (match-only). The lead as written: the flag that says the biped changed is set before the test on
// the biped's other object, which is where retail sets it (retail's mov bl, 1 sits before that cmp/je pair), so
// the store no longer lands after the join; what still differs is that retail computes the state pointer and
// spills it to its stack home before the early-exit test where ours rematerializes it, retail's type-0 bit test
// shifts a byte and masks it where ours masks the bit directly, retail tests the flags word in memory where ours
// caches it in a register, and the tail's or-chain differs because of the 0x111650 return convention.
// MiniMax B (11-11): no new form, no lower score (210 on the moved-flag form, not kept): retail loads the biped's other-object field into a
// register before the early-exit test and zeroes the result register there, so the test reads a register and the
// early exit jumps to the shared epilogue; writing the body as one `if` with a result variable reproduces the zero
// and the register load (301) and adding the state pointer as a local of its own reproduces the spill at
// [esp+0x20] (291), but the two together leave the whole body one instruction out of step (309) because this build
// then keeps the state pointer in EBX and pushes EBX in the prologue where retail keeps it in its stack home.
// MiniMax A (11-12): no new form, no lower score (222); retail computes the state pointer up front and spills it at
// [esp+0x20] with the biped index at [esp+0x24], while this build spills the index at [esp+0x20] and reads the parent
// through the parent-object helper, and retail's early exit jumps to the shared epilogue where this build has an
// epilogue of its own; the forms tried (the state pointer as a local, the early exit as one return, the parent's
// field read directly) all score 222 or worse.
// @retail 0xdd360
bool __stdcall function_dd360(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	s_biped_definition *definition = BIPED_DEFINITION(biped);
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
					s_unit_definition *local_a2a045 = UNIT_DEFINITION(BIPED_GET(biped->parent_object_index));
					byte *seat = (byte *)local_a2a045->unit.seats.address + biped->seat_index * 0xb0;

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
			(biped->physics_mode != 2 && !(TEST_FIELD_BIT((definition->biped.physics.flags >> 3) & 1))))
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
		function_1e5af0(&biped->physics_mode, &biped->forward, &biped->up, &output);
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
			else if (TEST_FIELD_BIT((biped->flags_348 >> 1) & 1))
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
	if (TEST_FIELD_BIT(((s_biped_flags_10a_view *)&biped->flags_10a)->bit2) && (*((byte *)biped + 0xc1) & 1))
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
// score 10 (was 10): what still differs: the crouch sum loads the biped's crouch and adds the spilled change (retail
// loads the change and adds the field), case 5 stores unknown35c before the surface flag test (retail tests first),
// case 4 stores 0x364 before reading entry+0xc for 0x354 (retail reads it first). Notes below, oldest last.
// MiniMax S (11-09): 0xdd990 at 10 (was 17): every +0x348 flag write goes through the existing word_flags_set.
// Not kept (coordinator 11-09): case 5's surface flag array in a one-use pointer, so the flag test keeps its own
// register instead of taking the index's (9; a one-use pointer is match-only).
// Still differs in three places: the crouch sum loads the biped's own crouch and adds the spilled change (retail
// loads the change and adds the field), case 5 still stores unknown35c before the surface flag test (retail tests
// first), and case 4's stores are ordered differently.
// MiniMax S (11-09): no new form, no lower score (17); still differs in four places: the crouch sum loads the
// biped's own crouch into the register and adds the spilled change (retail loads the change and adds the field),
// the crouch store sits after the flags clear (retail before it), case 5 stores unknown35c before the surface flag
// test (retail tests the flag first) and the 0x10 flag or is scheduled after the three unknown380 loads (retail
// before them); case 4's stores are ordered differently as well.
// MiniMax S (11-08): 0xdd990 at 17 (was 42): the case 4 lookup's address is carried in one pointer value, used by
// both the initialize call and the 0x1efd80 call, the same spelling 0x14a8e0 uses; retail keeps that address in a
// register and our build rematerialised it, which cost the two extra bytes. The length now agrees with retail's 1481.
// Still differs: the crouch sum loads the field first (retail loads the spilled crouch change), the crouch store sits
// after the flags clear (retail before it), case 5 stores unknown35c before the surface flag test, case 4's stores are
// ordered differently and the 0x10 or is scheduled after the unknown380 loads.
// MiniMax R (11-06): no new form, no lower score (42); the flag store still sits after the three loads and the crouch add still folds the field instead of the spilled operand; the crouch sum's two operands the other way round changes no code at all, and the flag clear ahead of the crouch store gives 49.
// Earlier note (at 42 then): Frame 0xb8 agrees; lookup-address rematerialization adds two bytes;
// crouch operands and contact/flag publication schedules remain.
// Earlier: walked against retail block by block (calls, argument order, offsets, constants, conditions,
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
// Round 46 (lane 14, fast): retail inlines 0xb77d0 here (the velocity block). That block as a call to function_b77d0 498
// (stays a call; the crouch sum then loads crouch_change first as retail); with a __forceinline declaration 498
// (not honoured; 0xb77d0's file is /Ob1); 0xb77d0 defined `inline` 44 (inlined, the square sum back to i/j/k and the
// crouch order back; 0x2056e0 stops matching). crouch_change named crouch_delta 42, change 42,
// z_crouch_change 42; the sum named `crouch` before the PIN 42.
// Round 118: the crouch sum in a named local under PIN 42 and with an if/else clamp 42 (both identical code);
// case 5 `found = true` first 42; case 4's surface pointer passed straight to the flag test 42; the lookup declared
// with the contact block's locals 42. Diagnostic (not keepable, a pointer to a whole local): an `s_lookup *` local
// used for both the initialize call and 0x1efd80 gives retail's esi reuse (`mov edx, esi`) and length, 42 -> 17
// (1481 bytes; left: the crouch sum/store order, case 5's 0x35c store, case 4's store order, the 0x10 or); 0x14a8e0
// shows the same reuse, so the lead is a source form in which both calls take one pointer value.
// Round 253: Failed object/lookup/surface case admissions, subsets 1/2/4/3/5/6/7 and all with the
// lookup declared at case scope: 42 each. Original body restored; frame agrees, lookup
// address rematerialization and crouch/surface/store scheduling remain.
// P267: word flag writers (the existing word_flags_set) with case 5's 0x35c store in both arms of the
// surface test / case 4's contact (0x354) stored before the position (0x364) / native decoded widths,
// subsets 1/2/4/3/5/6/7: 32/30/211/27/225/213/225; all without the writers 229. Not kept (coordinator):
// retail stores 0x35c once, between the flag test and its branch (0xddcd0), so the both-arms form is a
// duplicated store, and retail stores 0x364 before 0x354 (0xdddbd, 0xdddc7), the source's present order,
// so the case 4 swap is a score-picked order. The writers alone stayed under the keep bar in earlier rounds.
// F321: complementary contact-discovery state in bool/byte/word/long storage,
// then each with lookup/surface tests owning that state: 42/42/42/42/200/200/170/168.
// Original executable body restored; lookup receiver retention and store scheduling remain.
// M306: Combined initialize/surface admission crossed with enclosing lookup, chained contact clears and branch-owned instance index: 42/39/46/43/42/39/46/43. All restored.
// M306: Direct crouch update then clamp with branch-owned index/contact setters/terminal setter: 280/296/310/318/280/296/310/318. All restored.
// Lookup aggregate initialization at lookup/case/admission/object phases 186/217/204/204;
// CRT clears at those phases 182/213/199/199. Shared word-bit publication for crouch,
// contact, other and all flag groups 39/38/42/35; seven-count lead above 30 rejected.
// N317: Current-crouch bounded state direct/successive 42/42; shared word flags
// 35/35; branch-owned instance/contact-first publication 27/27; actual shared lookup
// receiver 2/2 at 1481 bytes. Incoming field capture and receiver alias are exact-only;
// the two crouch-add operands still reverse. Earlier uninitialized delta declarations
// before object/gate/component/root, original/offset names, 2 each (eight forms).
// Common real lookup/getter record declarations 42, joint publication 27; all other
// lookup-user scores unchanged. Original body and all supporting declarations restored.
// Round 369: complete squared-speed expression and three-product staging, each
// crossed with shared word flag publication and outer velocity workspace ownership:
// 45/45/38/38/45/45/38/38. Four-point best above 30 rejected; original predicate,
// declarations, flag stores and complete body restored. Lookup and crouch schedules remain.
// N323: Upper-first complete crouch sum: ternary/direct-field branches/mutable else-if/successive bounds, each without/with shared word flags: 47/40/295/295/47/40/47/40. Original complete body restored; lookup receiver rematerialization and crouch/contact schedules remain.
// F340: Last-consumer scopes for root/input/height, seven subsets, then all with
// a role-named contact lookup: 42/42/42/42/42/42/42/42. Original complete body restored;
// frame 0xb8 agrees; receiver rematerialization and crouch/contact schedules remain.
// Lane 1 round 62: the existing word_flags_set for every +0x348 write 35 (7 points, under the gate); with a crouch
// field pointer (read and store) 35, the pointer alone 39 (store before the `and` as retail); with case 4's 0x354
// store ahead of 0x364 30 (score-picked, retail stores 0x364 first); case 5 without the bsp local 198, also without
// the definition local 198, the surface flag in a long local 194. Crouch operands, compile-only (they reproduce the
// listing): field pointer, staged `+=`, a capture, an if/else clamp, the sum through an inline pin or add, the sum
// assigned back to crouch_change: field first in all. The velocity block's conditional call is what flips it: the
// block ending in an unconditional call loads crouch_change first, any `if` around 0xb7360 loads the field first.
// P293: Owning word-flag crouch/contact/other publications, seven subsets then direct final
// response-bit production: 42/42/42/42/42/42/42/71. Original complete body and helpers restored; no
// qualifying gain.
// Y551: Sole final flag-clear/return ownership at complete component/activity/ground
// admissions, seven subsets then all with entry lookup storage: 42 each. Original
// nested callback body restored; lookup rematerialization and crouch/contact schedules remain.
// M338: Complete actual activity admission in bool/byte/short/long, positive/complementary channels:
// 42/42/42/42/42/42/42/42. Original body restored; lookup receiver and crouch/contact schedules remain.
// Nonmatching single-consumer truth carriers restored.
// N348: Packed index/surface/kind extraction before key admission, subsets 1/2/4/3/5/6/7:
// 79/51/42/245/79/51/245; all with short kind 245. Original decoder scopes and full
// body restored; lookup receiver retention and crouch/contact scheduling remain.
// F355: Complete velocity-threshold positive/complementary truth ownership in bool/byte/short/long:
// 42/42/42/42/42/42/42/42. NOT kept: no gain; original body restored.
// J246: Coordinated table-getter inline contract with native word flags,
// actual crouch field receiver and entry lookup storage: 42/35/39/35/42/35/39/35;
// coordinated initializer inline contract alone 42. All helper users unchanged.
// NOT kept: seven-point flag gain is below the gate; field receiver misses.
// Original body/contracts restored; lookup address and crouch/contact schedules remain.
// P322: Actual lookup initialization result bool/byte/short/long at consumer
// then lookup-scope declarations: 42 each. NOT kept: one-use result misses equality;
// lookup receiver rematerialization and crouch/contact schedules remain. Full body restored.
// DL259: Native contact key/surface/fallback-geometry field subsets 1/2/4/3/5/6/7:
// 42 each; real shared lookup/getter prefix base with implicit conversion 42.
// Original record views, declarations and complete body restored; lookup receiver
// retention and crouch/contact schedules remain.
// M372: Actual contact point/normal/final-position CRT memcpy transfers, seven site
// subsets: 42 each; complete memmove 108 at 1462/1481 bytes. Original aggregate
// transfers and complete body restored; lookup receiver and crouch/contact schedules remain.
// Y590: Complete switch-default discovery ownership, found/missing channels in
// bool/byte/short/long storage: 42 each. NOT kept: no receiver-retention or
// contact-schedule gain; original complete body restored.
// P340: Recent-component height output, exhaustive/conditional real/double, immutable real/double and
// explicit-query real/double results: 121/121/137/144/121/144/121/144. NOT kept: no equality; original
// height observation, branch and full body restored. Lookup rematerialization and crouch/contact
// publication schedules remain.
// P346: Complete query/decoder contact admission, valid/invalid bool/byte/short/long results:
// 42/42/42/42/42/42/42/42. NOT kept: no native lookup retention or equality; original callback restored.
// M383: Actual contiguous contact-field CRT fills at initial/object/instance phase subsets
// 1/2/4/3/5/6/7, then fallback too: 255/214/204/259/275/221/266/272. NOT kept:
// lookup receiver/store schedule not recovered; original scalar clears and complete body restored.
// J256: Sparse contact-kind exclusive comparisons in six orders, named kind and independent tests:
// 260/135/260/110/139/136/261/276. NOT kept: all sparse dispatch forms regress from 42; lookup address rematerialization persists and the independent tests also grow the frame
// Original executable source restored; native frame and allocation gaps remain.
// Lane 15 round 58 (fast): shared lookup receiver alone 17; with it the speed test as one expression in all six term
// orders 20/20/19/19/20/19 (orders with j first emit i,j,k, the others j,i,k; none emits retail's i,k,j), through
// length_sq3f/dot3f 20/19, the crouch field read at entry 374; the velocity block as a call to an inlined 0xb77d0
// (diagnostic) 19, the crouch pair unchanged; length_sq3f without the receiver 45; receiver plus word_flags_set for
// every +0x348 write 10 (the crouch store and the 0x10 or become retail's); that with case 5's surface array named
// with the case's other pointers 9, a typed surface element 9, the crouch operands swapped 9. Left at 9: the crouch
// operand pair, case 5's 0x35c store ahead of the test, case 4's store order.
// Round 70 (m3 lane 14, fast): 0xb77d0's file is now plain /O2, but the velocity block as a call to it stays out of line,
// with a plain or a force-inline declaration 498/498. The lookup receiver re-measured 17; with word_flags_set for every
// +0x348 write 10; plus case 4's 0x354 store ahead of 0x364 5 (diagnostic, the receiver is not keepable). Without the
// receiver: case 4's swap 37, with word_flags_set 30 (not kept: the P267 order ruling; lead: retail loads entry+0xc
// between the 0x360 and 0x364 stores, 0xdddc3). Case 5 with a one-use flag ahead of the 0x35c store 210, with 0x35c
// stored after the if 192 (1489 bytes; compile-only gives the test before the store but sinks it to the join).
// Compile-only (it reproduces the crouch pair): the add loads crouch_change first only when the outer
// `if (function_dd8e0(...))` is removed (not just the inner velocity `if`); a bool or goto for that test, a scoped
// height_change, the block as an inline helper (with an early return), a copy or helper parameter for crouch_change,
// a named sum, `+=`, an if/else clamp, the compare reversed and the speed as one expression all keep ours.
// Lane 1 round 79 (fast): every +0x348 write through the s_biped_flags_348 bitfields 42 (same code); with the bit-0
// result as `field_0 = result.flags & 1` 71; case 4's entry+0xc surface named for its test and store 147 (1481 bytes);
// `physics` declared at its assignment 42; `velocity.k = velocity.k - height_change` 42.
// P383: Complete fallback-owner admission in bool/char/signed-char/byte/short/word/long/dword:
// 42/42/42/42/42/42/42/42. NOT kept: no native lookup-receiver retention or equality; original direct
// admission and whole body restored.
// Round 81 (m3 lane 1, fast): case 5's surface flag in a bool ahead of the 0x35c store 210 (1487 bytes); case 5's
// 0x35c store in both arms of the flag test 7; that with case 4's 0x354 store ahead of 0x364 2 (only the crouch-add
// operand pair left; both forms are exact-only, so not kept). On that 2: the velocity push (0xb7740, 0x1c4b00, the speed
// test and its three calls) as a force-inline helper 2, the whole height-change block as a force-inline helper 2, the
// velocity declared at the component scope 2, the speed sum declared with the block's locals 2, the summed crouch in a
// block local 2. Retail inlines 0xb77d0 here while its 16 other retail callers keep it out of line, so it cannot be
// made inline; the crouch pair is the blocker. More probes on the 2-form: no clamp (diagnostic) still field first;
// `crouch_change * 1.0f` 2; crouch_change declared last in the block 2, or at function scope before the biped 2; a
// helper shaped like 0xb77d0 (NULL angular argument and its `||` test, one-expression sums) 4 (the sum comes out i, j, k).
// Round 87 (m3 lane 1, fast; the 2-form, case 5's 0x35c store in both arms plus case 4's 0x354 store ahead of 0x364, re-measured 2:
// only the crouch-add pair left). Retail evidence for both parts: retail reads entry+0xc after the 0x35c/0x360 stores but before
// the 0x364 store, and reads the surface flag before the 0x35c store; a load cannot move across a store that may alias it (stores
// to the biped may reorder among themselves), so retail's source reads them in that order. On the 2-form: the velocity block as a
// call to 0xb77d0 with that function's parameter-address read removed 502 (still a call, so the read does not keep it out of line;
// 0xb77d0 itself 56); 0xb77d0 defined `inline` 4 (re-measure of round 46 on this base: inlined, the speed sum comes out i, j, k
// and the crouch add keeps the field first; 0x2056e0 434); object_get in place of every BIPED_GET 2 (same code). Compile-only map
// of the crouch add (it reproduces the pair): retail's order comes only when the dd8e0 block holds a float operation followed by a
// call and no nested test (`velocity.k -= height_change` then any call, or either test removed: change first; both tests, with
// int, float or speed conditions: field first; the block removed, empty, ba1d0 alone or calls without the float operation: field
// first); the first block, e30c0 and the code after the crouch store do not move it. An inline copy of 0xb77d0's body (inline or
// forceinline, with or without the parameter-address read, the sum written out in all six orders, length_sq3f or dot3f) keeps the
// field first and sums i, j, k where retail's inlined copy sums i, k, j: retail's inlined velocity block is not what our compiler
// makes of that body. Diagnostics on the call form: with the havok test removed 500, with both tests removed 478: 0xb77d0 stays
// out of line wherever the call stands, so our link never inlines it by itself (retail inlines it here and not at 0x2056e0's
// top-level call; `inline` inlines it at both): 0xb77d0's file is /Ob1 again (its @flags line). Diagnostic, not keepable (a flags
// change): that /Ob1 dropped with the call form 5, inlined by the link itself as retail, but the sum is still i, j, k, the crouch add
// field first and one `mov eax, esi` differs (0x2056e0 inlines it too, 544). Permuter on the 2-form (600 s, fast, anneal): 14 tries, none below 2.
// The inner test's first call, 0xb9b90, is an unmatched body (contributor code), a possible factor in that block's allocation.
// No untried natural spelling of the add is left.
// M3 lane 13 r85 (fast; the compile-only harness gives the same field-first add for each): the crouch change as a `real const`
// 10, as a const reference to the call's result 10; the speed test as `0.0001f < speed_squared` 10, as
// `!(speed_squared <= 0.0001f)` 292 (1485 bytes); the crouch field read into a local before the sum 10.
// M3 lane 13 r88 (fast). The 2-form (case 5's 0x35c store in both arms, case 4's 0x354 store ahead of 0x364) re-measured 2, the
// crouch-add pair alone; on it: 1-8 dead initialised locals at the top 2 each (the pair never moves: no symbol-count lever here);
// 1-8 extern declarations or 1, 3, 8 prototypes ahead of the function 2 each; the crouch change's address stored to a global
// (diagnostic) still field first (388, the frame shifts); the two changes as one two-element array 4; the height change declared
// ahead of the crouch change 2; the sum assigned back to the crouch change and then pinned 2. The velocity push as a plain static
// function of (object, velocity) 504 (1361 bytes: it stays a call); as a plain static function shaped like 0xb77d0 (object, linear,
// angular, called with a NULL angular part) the link expands it by itself, 4 (the sum comes out i, j, k and the crouch add field
// first, as with 0xb77d0 itself expanded); in that function length_sq3f, dot3f, a `moving` flag set by two tests or the terms
// written k, j, i give the same code, 4 each, and 1-8 dead locals in the caller 4 each. So neither the sum's term order nor the
// crouch pair follows a spelling of that body or a symbol count; retail's copy of 0xb77d0 here is still not reproduced.
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
			word_flags_set(&biped->flags_348, 15, true);
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
		word_flags_set(&biped->flags_348, 11, false);
		physics = function_e4680(arg_159e6d);
		if (havok_component_any_rigid_body_active(component) || physics || !(TEST_FIELD_BIT((biped->flags_c0 >> 6) & 1)))
		{
			word_flags_set(&biped->flags_348, 12, false);
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
								byte *model = g_4e3b44[OBJECT_DEFINITION(object)->model.tag_index & 0xffff].bytes;
								byte *region = *(byte **)(model + 0x74) + (surface & 0x1f) * 0x10;
								long permutation = (char)(*(byte **)(region + 0xc))[(surface >> 5 & 0xff) * 8 + 5];
								dword position = (((char)region[4] << 8) | (permutation & 0xff)) << 16;
								s_lookup lookup;
								s_lookup *lookup_pointer = &lookup;

								if (lookup_pointer->initialize(object_index))
								{
									byte *bsp = (byte *)function_1efd80((s_table_holder *)lookup_pointer, position);

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
					word_flags_set(&biped->flags_348, 12, true);
					biped->unknown368 = biped->unknown380;
				}
				function_e2fa0(arg_159e6d, result.unknown0c, *(byte **)((byte *)component + 0x70) + 0xc);
				word_flags_set(&biped->flags_348, 0, (result.flags & 1) != 0);
			}
		}
	}
	word_flags_set(&biped->flags_348, 7, false);
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
	s_character_physics_component *component, c_type_709360 animation_id, point3f *p1, point3f *p2, point3f *p3);
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
PRIVATE __forceinline void biped_eye_raise(long arg_159e6d, s_biped *self, s_biped_definition *self_definition, real crouch,
	point3f *eye)
{
	point3f head;
	vector3f forward;
	vector3f up;
	vector3f direction;
	s_biped_collision collision;

	{
		point3f scratch;

		real_point3d_set(&scratch, eye->x, eye->y, eye->z);
	}
	eye->z += ((1.0f - crouch) * self_definition->biped.standing_camera_height + self_definition->biped.crouching_camera_height * crouch) *
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

			vector3f_add_scaled((vector3f const *)eye, &back, t, (vector3f *)&start);
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
	s_biped_definition *self_definition = BIPED_DEFINITION(self);
	real crouch;

	if (mode == 0)
	{
		function_b9dd0(arg_159e6d, point);
	}
	else
	{
		*point = *origin;
	}
	if (mode == 3)
	{
		vector3f side;

		vector3f_assign(&side, 0.0f - forward->j, forward->i, 0.0f);
		vector3f_add_scaled((vector3f const *)point, forward, offsets[0], (vector3f *)point);
		vector3f_add_scaled((vector3f const *)point, &side, offsets[1], (vector3f *)point);
		point->z = offsets[2] + point->z;
		return;
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
// MiniMax B (11-13): no new form, no lower score (566); the frame is still 0xf8 against retail's 0xfc, and the call to 0x1e5bb0 still passes 17 values where retail passes 21, which the callee's own declaration decides.
// score 566 (was 566): earlier 585, 588; frame 0xf8 versus 0xfc (the gravity argument's early frame dword); position, ground,
// earlier 588; frame 0xf8 versus 0xfc (the gravity argument's early frame dword); position, ground,
// MiniMax B (11-14): no new form, no lower score (566); retail's frame is 0xfc against our 0xf8 (one dword more) and its
// definition pointer sits at the top of the frame where ours sits mid-frame, so every stack home after the prologue is four
// bytes out; the speed-scale block also computes `1.0f - a*b` before the helper's multiply where ours multiplies first.
// score 566 (was 585): earlier 588; frame 0xf8 versus 0xfc (the gravity argument's early frame dword); position, ground,
// material and animation homes differ.
// MiniMax B (11-13): no new form, no lower score (566); the frame is still 0xf8 against retail's 0xfc, and the call to 0x1e5bb0 still passes 17 values where retail passes 21, which the callee's own declaration decides.
// MiniMax I (11-09): no new form, no lower score (588); our call to 0x1e5bb0 passes 17 values where retail passes 21
// (our callee reads fewer parameters, so link-time code generation drops the rest), and the frame stays 0xf8 against
// retail's 0xfc (the gravity argument).
// Earlier 588 (was 589): Frame 0xf8 versus 0xfc (the gravity argument's early frame dword); position, ground,
// material and animation homes differ.
// MiniMax R (11-06): no new form, no lower score; 589 (was 585) as a side effect of the mode 3 arm in biped_eye_point becoming the `mode == 3` then branch, which fixes 0xdef60's layout (233 -> 104): the eye arm's start/back homes move by eight bytes here.
// Earlier at 589 (was 585): Frame 0xf8 versus 0xfc; position, ground, material and animation homes differ.
// Earlier: (was 612; 635; 1497 before biped_eye_raise was forced inline, 636 before the collision's NONE moved up in biped_eye_raise, 2026-10-31): biped_eye_raise is forced inline (below); the frame, flag homes and argument scheduling still differ.
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
// Round 339: Full-width airborne result with paired facing setters, existing target-vector producer
// and material result in both branches, eight subsets 639/651/639/651/692/690/692/690.
// Original result types, producers and body restored; frame and workspace homes remain.
// P258: Per-use tag/state/both/component/tag+component/state+component/all
// receivers 666/698/676/681/695/692/683; all plus use-site animation flags
// 689. Original receivers, eye helpers and complete executable body restored.
// DL242: Phase velocity reuse 608; packed-output library copy 616 and paired reuse 596 (exact-
// only); getter inline alone 610, with reuse/copy/both 608/616/596; all plus facing library copies
// 600. Native packed setter parameter/field 616, with phase velocity reuse 596. Result-object
// rename, construction at producer and both each tie 596. The getter inline declaration is
// restored. Keep the native packed-value setter contract with the shared velocity workspace;
// ordinary whole-program confirmed. The sole call passes the existing four-byte object directly;
// every setter body statement is unchanged. Frame and material/animation homes still differ.
// Round 330: consumed nearest points for eye/center and shared target/body point,
// subsets 1-7 then all with long airborne 616/600/604/619/615/600/600/644. Original complete body restored;
// frame, material/animation homes and entry scheduling remain.
// N321: Actual animation-getter return used by packed setter with early/producer construction and
// current/selected homes: 608; each with native turning 628. All regress and leave frame 0xf8 versus
// 0xfc. Original getter use, value graph and complete body restored. Scores
// 608/608/608/608/628/628/628/628.
// Original executable source restored.
// Round 373: Complete walking-speed selection in positive/nested/excluded/split admissions,
// original material default 668/657/668/540; material default at its selection
// 699/694/699/589. NOT kept: 540 moves the frame farther from retail (0xf4 versus
// 0xfc); 589 gains only seven and moves length away. Original complete body restored.
// AC483: Discarded point output reused as eye/center/body/eye+body/center+body,
// then eye+nearest-center/center+nearest-eye/eye+body+nearest-center:
// 612/605/614/622/609/604/609/605. Several reach 2828 bytes; frame/material/animation
// homes still differ. Original point outputs, helpers and complete body restored.
// N327: Animation-case receiver reacquired after physics initialization, reused/typed/direct/byte
// receivers 585/585/604/585; those with actual packed getter return 599/599/612/599. Keep the existing
// state reassignment at case entry: plain 585, 2828 bytes as retail, no other measurement changes or
// match losses. Retail reloads this receiver before the case calls. No new local or helper. Frame stays
// 0xf8 versus 0xfc; original packed output observation and all other phases retained.
// N330: Existing three-dimensional/planar difference and planar magnitude producers,
// then each with named nonnegative length selection: 585/585/589/589/599/599/600/600.
// Frame stays 0xf8 versus 0xfc; original scalar deltas, helpers and complete body restored.
// F342: Initialization-phase last-use scopes for speed/material/boost/turning,
// speed+material/speed+boost/material+boost/all: 584/585/585/585/585/584/585/585. NOT kept: no byte
// match; original complete body and every helper restored. Frame and material/airborne/animation homes
// remain.
// M329: Complete airborne false-default/true-default/positive-if-else/negative-if-else
// producers 648/648/585/585; turning false-default/true-default/first-test/final-chain
// 585/585/556/556. NOT kept: 556 uses a distinct turning-result branch graph from retail
// and keeps frame 0xf8 versus 0xfc; original bool producers and complete body restored.
// P292: Called walk result, walk attenuation product/complement, called boost result,
// boost increment, spread fraction, walking-rate quotient and doubled-radius span:
// 550/550/584/585/585/581/585/585. NOT kept: 550 uses a one-use called result and
// shortens away from retail; 584 gains one, 581 gains four and shortens away.
// Original complete body restored; missing frame word and material/airborne/animation homes remain.
// AC500: Actual initialization state/tag/read-only domains, masks 1-7, then read-only rigid body:
// 585 each. Native fields retain every observation and key width; original real record, receivers,
// packed output, helper graph and complete body restored. Frame 0xf8 versus 0xfc and scalar homes remain.
// DH314: Individual airborne/speed/boost/turning/moved/distance/paired target-size scalar
// identities, then the complete scalar graph: 585 each. Every initializer and lifetime fixed;
// frame stays 0xf8 versus 0xfc. Original entire body and helpers restored.
// DH318: Separate called radius, computed diameter and selected span; height-default/diameter-default/if-
// else/ternary, mutable/const diameter: 585/585/585/585/585/585/585/585. Frame 0xf8 versus 0xfc and
// material/airborne/animation homes remain. All restored.
// P292: Called walk result, walk attenuation product/complement, called boost result,
// boost increment, spread fraction, walking-rate quotient and doubled-radius span:
// 550/550/584/585/585/581/585/585. NOT kept: 550 uses a one-use called result and
// shortens away from retail; 584 gains one, 581 gains four and shortens away.
// Original complete body restored; missing frame word and material/airborne/animation homes remain.
// Y565: Independent nearest-point pair, measured point, eye/center and animated-body
// output declarations at case/function entry, paired/all case outputs, all entry outputs,
// then all plus animated body: eight at 585. Original body restored; frame 0xf8/0xfc
// and material/airborne/animation homes remain. No input observation or helper changed.
// J245: Actual moved result in bool/byte/short/long with function-scope
// declaration, then case-scope declaration and separate false producer: 585/585/584/581/585/585/584/581.
// NOT kept: 584 and 581 gain only one and four; every form retains frame 0xf8 versus 0xfc. Original movement result and all workspace homes restored.
// P300: Once-read motion input, twice-consumed turning key and real header byte,
// graphs 1/2/4/3/5/6/7 plus entry motion declaration: 585 each. No gain; original
// receivers, real header layout and full body restored. Frame 0xf8 versus 0xfc
// and material/airborne/animation homes remain.
// F369: Existing speed/boost accumulators at walking/spread/boost producers, masks 1-7,
// then all with stepwise boost: 550/585/550/585/550/585/550/550. NOT kept: 550 shortens
// to 2816 versus 2828 bytes and keeps frame 0xf8 versus 0xfc; original complete body restored.
// P316: Optional block address in mutable/const byte/void pointer domains: entry-null
// forms 585 each; selection-join null forms 691 each at 2788 bytes, frame 0xf4.
// NOT kept: no gain, join forms move frame/length away, and one-use field pointers
// require equality. Original integer carrier and complete body restored; frame
// 0xf8/0xfc and material/airborne/animation homes remain.
// P318: Gravity captured before/after turning in real/const real/double/const double
// on the retained animation/output graph: 579/579/497/497/461/461/497/497.
// Native frame 0xfc recovers; real captures grow to 2832 bytes, double stays 2828.
// NOT kept: one-use field capture and type-dependent allocation require equality;
// the six-point 579 form also misses the gain threshold. Original full body restored.
// P322: Actual grounded-call result bool/byte/short/long, consumer then entry declarations:
// 456/567/610/611/456/567/610/611. Bool recovers frame 0xfc and 2828 bytes; NOT kept:
// one-use call-result transport needs equality. Original whole body restored; material,
// airborne and animation homes remain on the retained graph.
// P324: Plain actual-component loops for all velocity defaults, selected facing and
// target-position input, masks 1-7: 820/800/895/659/820/800/957; complete target
// advance loop 617. NOT kept: all regress and grow beyond retail length; original
// native copies, row expressions and complete body restored. Shared helpers unchanged.
// F391: Material constant-default lifetime before object/tag/component/state/animation/airborne,
// then after position and after velocity: 584/584/584/584/584/584/590/610, all 2828 bytes.
// NOT kept: best gains one and every frame stays 0xf8 versus native 0xfc. Original material
// initialization, all memory observations, animation refresh, packed output, shared eye helpers and
// complete body restored; material/airborne/animation homes and missing frame word remain.
// B437: Airborne char/signed-char/byte/short/word/int/dword/long domains on the
// retained animation graph 599/599/599/632/632/627/627/627. NOT kept: all regress;
// frame 0xf8 versus 0xfc and material/airborne/animation homes remain.
// P337: Default boost before tag/component/airborne/position, then after position/
// velocity/facing/speed: 659/659/659/660/651/631/613/603, 2816 bytes, frame 0xf8.
// NOT kept: all regress; original default branch and complete body restored.
// P339: Whole case orders 2/1/3/6/4-5, 3/1/2/6/4-5, 1/3/2/6/4-5,
// 1/2/3/4-5/6, 2/1/3/4-5/6, 3/1/2/4-5/6, 1/3/2/4-5/6, 3/2/1/4-5/6:
// 592/582/587/606/608/596/606/607, all 2828 bytes, frame 0xf8.
// NOT kept: best gains three and no native frame/layout match. Original case
// order and full body restored; shared helpers and all checked peers unchanged.
// P341: Actual selected-root/header/ground-body record lvalues, seven subsets then complete read-only
// graph: 585/585/585/585/585/585/585/585. NOT kept: missing native frame word and
// material/airborne/animation homes remain; original receivers, helper graph and whole body restored.
// P343: Initial speed-constant lifetime before
// object/tag/component/state/animation/airborne/ground/facing boundaries:
// 589/585/585/585/585/585/600/618. NOT kept: no equality; original whole body restored; frame 0xf8
// versus 0xfc and material/airborne/animation homes remain.
// F391: Material constant-default lifetime before object/tag/component/state/animation/airborne,
// then after position and after velocity: 584/584/584/584/584/584/590/610, all 2828 bytes.
// NOT kept: best gains one and every frame stays 0xf8 versus native 0xfc. Original material
// initialization, all memory observations, animation refresh, packed output, shared eye helpers and
// complete body restored; material/airborne/animation homes and missing frame word remain.
// N399: Native initial-motion/contact-query/eye-body vector and point lexical homes,
// families 1..7 then alternate complete prefix: fast 585/585/585/585/585/585/585/585. Every original executable
// statement and lifetime preserved. NOT kept: unmatched home allocation; original body restored.
// Lane 15 round 58, NOT kept (585 -> 439, plain): the missing frame word is the 0x1e5bb0 gravity argument. Retail
// loads g_51e9c4->unknown0 early and keeps it in its own dword ([esp+0xac]), as it does for the field_bit_test()
// arguments, which are inline calls; a direct read is loaded at its push (retail 0x118700 does that). Through an inline
// gravity accessor the frame is retail's 0xfc: 461 (fast), and with biped_eye_point's mode 3 arm in the origin copy's
// else (0xdef60's form) 439, aligned (LCS) differences 52. Then (fast): component declared after airborne 457,
// material declared before speed_scale 438, the vector locals reordered 439, the case 1 rate (and 0xe0c70's) through
// a file-local tick getter 439, the case 1 product's factors swapped 439. Not kept because matched 0xc2d00
// (src/unknown_0bffa0.cpp, unrelated callees) loses its match (2: two loads swap after its 0xb8d30 call) whenever
// such an accessor exists in this file: header and file-local, inline/__forceinline/const-reference (the last leaves
// this function at 589), two placements and three names all lose it; the 104 form alone keeps 0xc2d00 but raises
// this function to 589.
// Lane 15 round 61, NOT kept: the round 58 recipe again (0xc2d00 is unmatched on main now): the gravity argument
// through an inline accessor in include/unknown_1eb550.h (also used by 0xe3380, codegen-neutral there) plus
// biped_eye_point's mode 3 arm in the origin copy's else, 585 -> 439 (plain, frame 0xfc); then 0x1cd510 returning its
// animation id by value (retail evidence: retail's case 4/5 passes a slot in EAX, makes none of the constructor's two
// NONE stores and reads the result through the returned pointer, `mov eax, [eax]`; 0x1cd510's own bytes unchanged)
// 439 -> 397 at 2828 bytes (plain). Reverted because the accessor costs matched 0x54590 (src/network_voice.cpp, two
// loads swap) in the full build: with the accessor and the mode 3 arm alone, with real_point3d_set too, and with
// everything, 0x54590 is 2; without the accessor it matches. Without the accessor (fast): by-value alone 605
// (2804 bytes), the mode 3 arm 589, mode 3 + 0xdef60's real_point3d_set 589, all three 609. With the accessor (fast):
// alone 461, + real_point3d_set 461, + mode 3 439, + both 439, + by-value 397. On 397 (fast): case 6's distance as
// `length = sqrt(...); distance = length > 0.0f ? length : 0.0f;` 603 (retail's select, its temp in airborne's slot
// and the whole lower frame become retail's, but position moves 0x74 -> 0x90 and material stays low; aligned 60 ->
// 61), that through magnitude2d 603; the planar distance through magnitude2d alone 397 (same code: ours squares dy
// first and keeps dx, retail squares dx first in place); the walking speed_scale product's factors swapped 397 (same
// code); case 1's rate named ahead of its call 397 (same code).
// Lane 15 round 62: 589 -> 588 from 0xdef60's helper forms in biped_eye_raise/biped_eye_point (start through
// vector3f_add_scaled, mode 3's side through vector3f_assign). The gravity accessor re-measured (plain) on that state:
// 466 (2832 bytes), 0xc2d00 matches, but 0x54590 goes to 2 again, in include/unknown_1eb550.h and file-local here
// alike; not kept. Retail loads the gravity at 0xdc8a8 among the early-evaluated inline-call arguments (the
// field_bit_test temps, right to left) and stores the value in its own dword, so retail's argument is an inline
// call; the blocker is the whole-image tie that swaps 0x54590's (and 0xc2d00's) two reloads. 0x1cd510 by value
// without the accessor 608 (plain); the grab header declared at its assignment 588; the spread call inline in its
// product 588; the walking globals pointer read at its use 566 (2816 bytes: the pre-call load retail has is lost,
// aligned 308 -> 313, not kept); `turning` written inline in the call 588.
// P364: Existing moved result owned by the contact-query return before its branch,
// bool/char/signed-char/byte/short/word/long/dword: 589/589/589/589/588/588/583/583.
// NOT kept: best gains five; native success publication follows distance calculation.
// Original whole body restored; frame 0xf8 versus 0xfc and scalar/argument homes remain.
// P366: Actual case-6 target-entry CRT copy/fallback point setter/coherent zero-velocity CRT transfers, masks
// 1/2/3/4/5/6/7: 588/589/589/588/588/589/589; complete graph with existing scaled add 585. NOT kept: best
// gains only three; frame remains 0xf8/0xfc. Original complete body, call values and helpers restored;
// material/airborne/animation homes remain.
// Behaviour fix (11-08, from P366's finding): retail 0xdd022..0xdd03f passes the case-6 target position as
// 0x1e6850's first point (stack, stored to +0xc0) and the velocity as its second (ECX, +0xcc); ours had them swapped.
// P369: Existing settings-record member read, ordinary/forced inline, const/mutable receiver and value/reference
// return: 464/588/464/588 twice. Value results recover frame 0xfc but grow to 2832/2828; references retain frame
// 0xf8. NOT kept: unmatched single-caller helper and nonnative extent; complete original header/body restored.
// P371: Complete complementary movement outcome in bool/char/signed-char/byte/short/word/long/dword:
// 585/585/585/585/590/590/588/588. NOT kept: best gains three, wider forms grow
// to 2832/2828 bytes, and frame stays 0xf8/0xfc; original complete body restored.
// P373: Actual center-top/eye-height/correction arithmetic boundaries, seven subsets,
// then half-span with the complete correction: 588/588/588/588/588/588/585/588, 2828 bytes.
// NOT kept: best gains three, with no native frame equality; original whole body restored.
// P375: Fixed-first initial velocity, conditional facing source and actual multi-use motion receiver,
// masks 1..7, then complete read-only motion receiver: 604/661/671/588/604/661/671/671. NOT kept: no native frame/home
// equality; original full body restored, including every shared eye helper and gravity observation.
// P377: Native initialization bit-result producers for motion/animation/paired movement
// flags, seven subsets then complete immutable results: 584/591/557/599/583/587/557/557. NOT kept: one-use inputs
// require equality; original complete body, gravity observation and all shared helpers restored.
// P379: Case-6 zero-distance default at function entry, before/after position, before
// facing/speed/boost/turning/dispatch: 620/616/602/585/571/559/559/576. NOT kept: first seven recover frame
// 0xfc but grow to 2832/2828 bytes; final 576 keeps frame 0xf8/0xfc and stores zero before dispatch rather
// than inside case 6. All original constants, gravity observations and whole body restored.
// P381: Complete complementary airborne outcome in bool/char/signed-char/byte/short/word/long/dword:
// 599/599/599/599/631/631/626/626. NOT kept: narrow frame 0xf4, wider 0xf8 versus native 0xfc;
// gravity/material/airborne homes remain. Original state result, both consumers and full body restored.
// P383: Actual turning comparison in char/signed-char/byte/short/word/int/long/dword storage:
// 600/600/600/630/630/630/630/630. NOT kept: no native frame equality; every form shortens to 2808 versus
// 2828 bytes. Original predicate and whole body restored; early gravity/material/airborne/animation homes
// remain.
// P391: Entry component/state/animation/airborne declaration orders CASB/ACSB/ASCB/SCAB/
// SACB/SCBA/SBCA/SBAC: 572/591/604/574/604/585/607/596. C=component, S=state,
// A=animation flags, B=airborne. NOT kept: score-selected orders do not recover the native
// frame word or entry load schedule; every frame stays 0xf8. Original full body restored.
// M3 lane 13 r74 (plain unless marked): the gravity argument through a new inline accessor in include/unknown_1eb550.h
// (also at 0xe3380) 459 at 2832 bytes (masked+stack aligned 201 -> 72), but matched 0x54590 goes to 2 again; the same
// read into a plain `real` local after `turning` 459 (identical code) with 0x54590 still matched; the accessor as an
// ordinary function in src/unknown_1eb550.cpp (linked after network_voice.cpp), which LTCG inlines, 459 and 0x54590 2
// again: any new function in this call graph flips 0x54590 wherever it is defined (diff in build/r74 of m3-lane13).
// Fast: boost as one conditional expression 585 (same code), facing as one conditional expression 661 (2796 bytes).
// M3 lane 16 r104 (fast; fast agrees with the full build here, 585): `turning` through a named animation id read once and
// compared twice 585; case 1's flags_348 bit 7 through field_bit_test 585; the rigid body's motion pointer as a local used three
// times 585; the fallback's `biped->unknown434 = distance = 0.3f;` 585; the facing source chosen as a pointer 661 (2796 B).
// No existing function returns the gravity block or its field (grep), so the frame dword still has no keepable route. Retail
// puts the biped's component in EAX for 0x183670 (as 0xe5d50's first argument), ours the root's: a parameter-order lead whose
// definition sits in src/unknown_181a10.cpp (another lane's file this round) and is unmatched.
// M3 lane 2 r73 (plain, every function of this file checked each time; aligned 370, masked 108 at 585). KEPT, 585 -> 566
// (2816 B against 2828, aligned 376, masked 109): the walking speed's one-use `globals` pointer local removed, the tag field
// read at its use (lane 15 r62's form; retail loads that pointer into ESI before the 0xdfa60 call, ours after it now); the
// fallback target through vector3f_add_scaled 566 (same count, kept: an existing helper's body). On 566: the spread call
// inline in its product 585 (masked 105); `turning` written in the 0x1e5bb0 call 566 (aligned 389); 0x183670's components
// swapped at the call (diagnostic: retail's EAX setup, 0x183670's own entry takes component_a in EAX as retail's does, but
// a call-site swap alone is not keepable while it is unmatched) 566; the walking product written call-first 566 (same code);
// 0x1cd510 returning the animation id by value inside the 0x1e67f0 arguments 585 (2804 B, masked 103; 0x1cd510 and its
// file still match); the one-use animation-flags pointer read at its use 685 (2792 B); both flags_348 bit reads through
// flags_348_bits 570. No other untried natural form is left here: the frame dword still needs the gravity argument as an
// inline call, and any new function flips 0x54590.
// P436: Fixed existing biped/tag/component/animation receivers, airborne result,
// root handle/receiver and turning result: eight fast 566s, control 566. NOT kept:
// no native frame-word repair or byte change; original complete body restored.
// P439: Complete coordinate ranges for unit/alternate facing, target and fixed velocity,
// then backward target/unit-facing/alternate-facing/fixed-velocity: 624/624/587/633/587/624/624/633.
// NOT kept: every range adds a call (28 versus 27); facing/velocity recover frame
// 0xfc only at 624/633. Original material/airborne homes remain; original
// transfers, shared eye helpers, gravity observation and full body restored.
// MiniMax A (11-13): no new form tried this round, no lower score (566): the frame is 0xf8 here and 0xfc in
// retail, so the argument slots sit four bytes higher, this build loads the 1.0f constant before the first store
// where retail stores first, and the body is 2816 bytes against retail's 2828.
// M3 lane 4 r196 (fast, which agrees here): airborne declared after speed_scale and material 566 (same code); material
// declared first 565; speed_scale ahead of airborne 566 (same code); boost as `boost = 1.0f; if (control_flags & 0x800)
// boost = ...;` 579 (2812 B); the facing select with the negated test and the arms swapped 570.
// P471: Complete grab/animation workspace cases before the small cases, orders
// 6/1/2/3/45, 6/1/3/2/45, 6/2/1/3/45, 6/2/3/1/45, 6/3/1/2/45,
// 6/3/2/1/45, 6/45/1/2/3, 45/6/1/2/3: 557/557/566/557/551/557/584/562.
// NOT kept: workspace-first blocks differ from native initialization-before-grab
// layout, and the frame stays 0xf8/0xfc; original cases and full body restored.
// P473: Distance/radius/both standard maxima, comparator maxima, then reversed-order
// comparator minima at distance/radius: 553/547/579/562/568/588/562/568. NOT kept: no native frame-word
// equality; original complete body, unordered selections and includes restored.
// Apparent gains add out-of-line bound calls absent from the native inline selections.
// P476: Standard typed airborne/turning/walking-mode predicates, masks 1/2/4/3/5/6/7, then root sentinel producer. Scores 646/640/620/702/626/675/726/764.
// NOT kept: unmatched diagnostic source forms; original executable body restored.
// @retail 0xdc5c0
void __stdcall function_dc5c0(long arg_159e6d, s_biped_physics_output *output)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	s_biped_definition *definition = BIPED_DEFINITION(biped);
	s_havok_component *component = havok_component_get(biped->havok_component_index);
	byte *state = (byte *)biped + *(short *)((byte *)biped + 0x12a);
	word const *animation_flags = (word const *)((byte *)biped + biped->unknown33e);
	bool airborne = *(long *)(state + 0x7c) == 0xe0000c2;
	real speed_scale = 1.0f;
	long material = 0;
	real boost;
	point3f position;
	vector3f velocity;
	vector3f facing;

	function_b9dd0(arg_159e6d, &position);
	hkRigidBody *rigid_body = havok_component_rigid_body_get(0, component);

	if (!rigid_body->m_fixed)
	{
		vector3f_assign(&velocity, rigid_body->m_motion->m_linear_velocity(0),
			rigid_body->m_motion->m_linear_velocity(1), rigid_body->m_motion->m_linear_velocity(2));
	}
	else
	{
		velocity = *g_4687a4;
	}
	if (field_bit_test(definition->unit.flags, 11))
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
		if (biped->physics_mode == 1 && field_bit_test(definition->biped.physics.flags, 2) && !airborne)
		{
			speed_scale = (1.0f - biped->unknown2e4 * *(real *)(*(byte **)((byte *)g_4e034c + 0x134) + 0x78)) *
				function_dfa60(arg_159e6d);
		}
		if (field_bit_test(definition->biped.flags, 6) && !biped->unknown1bc)
		{
			real spread = function_1e9700(8);

			speed_scale = spread * ((dword)arg_159e6d % 0x89 * 0.0072992700f) + 1.0f;
		}
	}
	if (!(biped->control_flags & 1) && field_bit_test(biped->flags_134, 23))
	{
		if (definition->biped.physics.pill_shapes.count)
		{
			material = (long)definition->biped.physics.pill_shapes.address + 0x20;
		}
		else
		{
			material = (long)definition->biped.physics.sphere_shapes.address + 0x30;
		}
	}
	if (biped->control_flags & 0x800)
	{
		boost = function_d1210(arg_159e6d) * (definition->unit.boost.boost_peak_power - 1.0f) + 1.0f;
	}
	else
	{
		boost = 1.0f;
	}

	bool turning = *(long *)(state + 0x7c) == 0x900000e || *(long *)(state + 0x7c) == 0xa00000f;

	function_1e5bb0(output, &biped->physics_mode, state, speed_scale, biped->havok_component_index, arg_159e6d,
		&definition->biped.physics, 9, field_bit_test(biped->flags_c0, 6), turning, TEST_FIELD_BIT((biped->control_flags >> 3) & 1),
		field_bit_test(*animation_flags, 2), field_bit_test(biped->flags_c0, 8), function_e4050(arg_159e6d),
		field_bit_test(biped->flags_348, 1), biped->unknown39d != 0, g_51e9c4->unknown0, boost, &biped->control, &position,
		&biped->forward, &biped->up, &biped->facing_goal, &facing, &velocity, material);
	switch (biped->physics_mode)
	{
	case 1:
		function_1e6120(output, &biped->physics_mode,
			g_510c54->rate / definition->biped.stun_recovery_time * (real)biped->unknown39d, airborne,
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

		velocity = *g_4687a4;
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
				function_ba1d0(root_index, &velocity, NULL);
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
			vector3f_add_scaled((vector3f const *)&position, &facing, 0.3f, (vector3f *)&target_position);
			biped->unknown434 = 0.3f;
			distance = 0.3f;
			velocity = *g_4687a4;
			moved = true;
		}
		function_1e6850((s_type_94656b *)output, moved, &target_position,
			(point3f *)&velocity, distance);
		break;
	}
	case 4:
	case 5:
	{
		state = (byte *)biped + *(short *)((byte *)biped + 0x12a);
		point3f body;
		c_type_709360 current;

		function_e59e0(arg_159e6d, &velocity, &position);
		havok_component_rigid_body_position_get(0, component, &body);
		((s_animation_state *)state)->current_animation_get(&current);
		function_1e67f0((s_type_94656b *)output,
			(s_character_physics_component *)&velocity, current, &body, &position, (point3f *)&velocity);
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
// @retail 0xde620
void __stdcall function_de620(long arg_159e6d, vector3f const *impulse)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	s_biped_definition *definition = BIPED_DEFINITION(biped);

	if (!(TEST_FIELD_BIT((definition->unit.flags >> 20) & 1)))
	{
		bool strong = impulse->i * impulse->i + impulse->j * impulse->j + impulse->k * impulse->k > 1.4f * 1.4f;
		bool spinning = false;
		vector3f velocity = *impulse;
		vector3f spin = *g_4687a4;

		if (!(TEST_FIELD_BIT(((s_biped_flags_10a_view *)&biped->flags_10a)->bit2)))
		{
			vector3f_scale(&velocity, 0.5f, &velocity);
		}
		word_flags_set(&biped->flags_348, 1, true);
		if ((TEST_FIELD_BIT(((s_biped_flags_10a_view *)&biped->flags_10a)->bit2)) || (TEST_FIELD_BIT((definition->biped.physics.flags >> 3) & 1)) || biped->physics_mode == 2)
		{
			vector3f axis;
			real magnitude;
			real spin_fraction;
			real angle;

			cross3f(g_4687b0, &velocity, &axis);
			function_30bf0(&axis);
			magnitude = (real)sqrt(velocity.i * velocity.i + velocity.k * velocity.k + velocity.j * velocity.j);
			spin_fraction = (real)random_next(&g_4e7408->unknown0) * 1.5259022e-05f;
			/* the first row names the angle: retail copies it for this product and loads the other two axes first */
			spin.i = axis.i * (angle = spin_fraction * magnitude * 1.5707964f);
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
			byte mode_byte = biped->physics_mode;
			long mode = mode_byte;

			if (mode != 4 && mode != 5 && !biped->unknown34b && (strong || mode_byte != 3) &&
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
		s_biped_definition *target_definition = BIPED_DEFINITION(target);
		point3f center;
		point3f eye;
		real height;
		long count;
		long i;

		if (target_definition->biped.physics.flags & 1)
		{
			center = *(point3f *)((byte *)target + 0x30);
			count = 1;
			height = 0.0f;
		}
		else
		{
			real span;

			function_df5f0(target_index, &center, &height, &span);
			height += target_definition->biped.physics.radius;
			center.z -= target_definition->biped.physics.radius * 0.5f;
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
// MiniMax S (11-08): 0xdef60 matches byte for byte (1041 bytes, score 0). The raise takes one more local filled
// through the existing real_point3d_set ahead of the back scale, and the first-ray start's y product then takes
// retail's load-first form (back.j loaded, then the scale) instead of copying the scale first; the x and z
// products were already load-first. 0xde9d0, 0xcb500 and 0x1e3b00 still match, 0xdc5c0 588 -> 585, and 0xdd990
// (42) and 0xe01d0 (605) are unchanged.
// score 7 (was 7): 1040 bytes against 1041; only the first-ray start's y product differs: ours copies the scale
// first (`movaps xmm1, xmm0; mulss xmm1, [back.j]`), retail loads back.j first (the x and z products agree).
// MiniMax R (11-06): the mode 3 arm as the then branch of `mode == 3` puts retail's block layout in place (233 -> 104):
// the mode != 0 copy now sits after the shared arm, as in retail, and the first 110 instructions agree byte for byte; the
// forward/up homes stay the other way round (ours 0x1c/0x28, retail 0x28/0x1c) and swapping their declarations does not move them.
// Earlier at 104 (was 233): Frame 0x8c agrees; the mode arms are laid out as in retail, but the forward/up homes are swapped
// against retail (ours 0x1c/0x28, retail 0x28/0x1c) and the second ray's dot products take their colours from that.
// Earlier: first global-load scheduling, the offsets in ECX rather than on
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
// Round 314: Mode-three forward traversal 248; side-vector scaled add 240; both 240;
// forward scaled add 240; both scaled adds 203 (shared user regresses); traversal/CRT
// origin 248; scaled add/CRT origin 240; point origin setter/traversal 263. Original restored.
// Round 324: single complete mode dispatch orders 0123d/3012d/d0123/1203d/2103d/3201d/013d2/d3210
// 392/375/392/393/393/378/388/375; shared user 610 -> 614 in all. Original helper restored.
// Common origin setup once with mode orders 312d/123d/d312/1d23/2d31/321d/d213/231d:
// 254 each; direct helper users unchanged, dispatcher 108 -> 109. Original helper restored.
// Round 359: Forward/side/both scale observations 240/235/240; each with a complete
// direction input 282/278/279; both scales and the crouch if-chain, without/with direction,
// 251/277. Every form raises shared user 0xdc5c0 from 610 to 614; overlap-sensitive
// snapshots also need a match. Original shared helper and complete target restored.
// Round 525: Single-exit mode-three/raised-eye branch after common origin acquisition, four crouch
// result forms with native/memmove origin transfer: 248/248/251/251/273/273/274/274. Memmove loses a matched
// helper user; both original helpers and the complete target restored. Frame/convention homes remain.
// P269: existing native tag real fields +0x218/+0x21c, mutable/read-only tag and
// biped roots, initialized/separate pointer declarations, masks 0..7:
// 248/248/248/248/248/248/248/248. Original tag layout, helpers and body restored.
// AC481: match-only offsets parameter address read and forward/origin/mode home
// subsets 0..7: 222/242/242/242/242/242/242/245. On offsets-only, zero-mode first /
// forward-up names / native tag-table receiver, subsets 1/2/4/3/5/6/7, then all
// with head declared after collision: 228/222/222/228/228/222/228/228. The 222
// form restores ret 0x14 but needs exact bytes; all bodies and helpers restored.
// Round 537: real forward/up/direction scratch scope subsets 248/248/248/248;
// remaining subsets and head-at-copy 248/248/248/248. Original shared helper and
// complete target restored; offsets transport and forward/up homes remain.
// N323: Normalized-direction early admission, complete collision-result join and native point setter, seven subsets then complete height-delta producer: 249/249/248/249/249/249/249/249. All original helpers/body restored; shared users regress and offsets/forward/up homes remain.
// Round 377: Existing typed object getter and CRT transfers of already-copied head/
// forward inputs, seven subsets, then both head publications: 248 in all eight.
// Every shared-user score unchanged; original helpers and target restored.
// Object/tag acquisition, stacked offsets and forward/up homes remain.
// Lane 1 round 62, kept (248 -> 233 at 1041 bytes; 0xcb500 108 -> 24, the convention now retail's): `if (mode == 0)`
// first, then `if (mode != 3)` around the crouch switch and the raise, mode 3's side vector (0 - j, i, 0) with both
// offsets through the existing vector3f_add_scaled in its else, the offsets on the stack with no address read.
// Lead, not kept: the same arm written ahead of the raise with a return 104 (1058 bytes, first difference +0x1bd,
// mode 3 and the entry exactly retail's), but its symbols come before the raise's in 0xdc5c0 too, which goes
// 585 -> 589 (positional; aligned full 297 -> 271, its second ray block becomes retail's register for register).
// Tried: mode first alone 228 (with the offsets' address read), side as written-out adds 226, cached
// forward.i/along/across 151, scalar side parts 104/143, cached forward.i with along/zero locals 124, the side
// vector declared at the helper's top or in a block 104; every arm ahead of the raise 0xdc5c0 589. In biped_eye_raise
// on the 104 form: forward or up renamed 104, swapped declarations 104, dot3f(&direction, ...) 104; the test written
// head-first 80 and direction-first 80 (0xde9d0 stops matching, 20).
// M340: Actual head/forward/fallback coordinate-array transfers 1/2/4/3/5/6/7, all with returned-up consumption: 240/247/245/256/253/258/268/275. NOT kept: no complete native/shared-user agreement; helpers and body restored.
// Lane 18 round 34 (fast): `if (mode != 0)` copy first 252 (1038 bytes, 0xcb500 107); `if (mode == 3)` arm first as an
// if/else around the crouch switch and raise 104 (1058 bytes, +0x1bd; 0xdc5c0 585 -> 589, its second ray block then
// register for register retail's, so the rise is positional), that with crouch scoped to the else 104/589; crouch
// scoped to the mode != 3 block 233; switch cases 2, 1, default 233; default's ternary as if/else 233. In the 104 form
// the forward/up homes are swapped (forward [esp+0x1c], retail [esp+0x28]) and the to_head terms spill.
// Y553: Complete first-ray displacement/final-clearance/start-setter subsets
// 234/240/236/240/234/240/236; both point setters 236. Original helpers restored.
// NOT kept: no target gain; scaled-displacement/clearance forms also raise shared users.
// P290: Native nested nonzero-origin/mode-three admission with original switch,
// standing-first/crouching-first results, explicit default branches, zero/one initial
// crouch and default-owned zero: 104/261/261/104/104/245/242/247.
// NOT kept: every form raises shared user 0xdc5c0; some also lose 0xde9d0.
// Original helpers/body restored; mode-three layout and basis/ray homes remain.
// J245: Existing head/forward/fallback scalar-setter subsets 1/2/4/3/5/6/7,
// then first-ray miss setter: 235/229/253/241/255/258/256/245.
// NOT kept: all forms lose 0xde9d0; 229 gains only four and grows to 1047/1041 bytes. Original dispatch, basis and ray homes remain.
// P300: Sequential forward/back, up/ray and returned-up storage graphs 1/2/3/4/5/6/7:
// 233/233/233/240/240/233/233; forward/back at its first writer 233. NOT kept:
// no target gain, and several forms raise shared users. Original helpers/body restored;
// mode-three placement and basis/ray homes remain.
// N353: Mode-three predicate bool/byte/word/long before origin acquisition
// 270/270/261/261; after origin acquisition 233/233/233/233.
// NOT kept: no gain or native predicate storage match; original helpers and all user bodies restored.
// AC518: Complete normalized head-separation admission bool/byte/short/long
// at producer or before head workspace: 233/233/233/233/233/233/233/233.
// NOT kept: one-use admission result unmatched; bool forms also raise a shared user.
// B421: First-ray hit/clear bool/byte/short/long results 233/233/233/233/233/233/233/233.
// NOT kept: no match; Boolean clear also raises shared user 0xdc5c0 585 -> 589.
// Both original helpers and complete target restored; mode-three/ray homes remain.
// P316: Actual hit/head projection and clearance-endpoint ownership, six native scalar
// forms 233 each; both projections plus clearance or actual obstruction result 234/234.
// NOT kept: no gain. Original shared helpers restored; native mode-three placement
// and forward/up/ray homes remain; every checked shared user retains its score.
// F373: Height-interpolation complement/paired products/unscaled/scaled sum,
// paired products with sum, complement with products/unscaled/scaled sum:
// 233/233/233/233/233/233/233/233. NOT kept: no byte match; original
// shared helpers restored, mode-three placement and basis/ray homes remain.
// F375: Default stance-query standing/crouch returns, bool/byte/short/long:
// 233/233/233/233/233/233/233/233. Complementary forms also raise shared users.
// NOT kept: no byte match; original helpers restored, mode-three/basis/ray homes remain.
// P321: Initial zero/one crouch with ternary/complete/crouch-default/zero-default
// switch results: 233/233/252/233/233/233/264/234. NOT kept: no gain;
// crouch-default and zero-default also raise shared users. Helpers restored.
// P344: Actual first-ray fraction and hit/head projection declarations at the collision workspace,
// seven subsets then reversed declarations: 233/233/233/233/233/233/233/233. NOT kept: no byte match;
// both original helpers and complete target restored. Mode-three placement and basis/ray homes remain.
// P346: Unified adjusted-head result with one final eye publication, original/final setter,
// start setter and returned-up call ownership: 238/239/239/248/245/240/240/249.
// NOT kept: native output/frame schedule not recovered and shared users regress. Both helpers restored.
// F384: Actual side-vector setter/point-setter/mutable/immutable initializer,
// native or memmove origin transfer: 235/235/233/233/256/256/253/253. NOT kept:
// no gain; memmove with either initializer also raises a shared user. Original
// shared helpers and target restored; mode-three placement and basis/ray homes remain.
// F390: Complete joined final-point producer after the second query, hit in bool/byte/short/long,
// record then scalar-setter publication: 226/226/226/226/278/278/278/278. NOT kept: seven-point
// lead grows to 1044/1041 bytes; setters reach 1077. Every form raises shared users and loses
// exact 0xde9d0. Both original helpers and full target restored; mode-three/basis/ray homes remain.
// B439: Second-ray hit/clear result in bool/byte/short/long storage:
// 233/233/233/233/233/233/233/233. NOT kept: one-use query results do not
// match; both helpers restored, native mode-three and basis/ray homes remain.
// M386: Plain indexed/pointer start and clearance component producers, independent,
// paired and crossed: 237/240/240/257/468/454/468/257. NOT kept: no byte equality or
// qualifying native/shared-user gain; original shared helpers and complete body restored.
// N380: Standard complete head/forward/fallback object-copy boundaries, subsets 1..7,
// then complete backward ranges: 236/239/244/270/267/272/263/259. NOT kept: target and shared users
// regress; original helpers and complete wrapper restored. Native mode-three
// placement and forward/up/second-ray homes remain.
// P344: Actual first-ray fraction and hit/head projection declarations at the collision workspace,
// seven subsets then reversed declarations: 233/233/233/233/233/233/233/233. NOT kept: no byte match;
// both original helpers and complete target restored. Mode-three placement and basis/ray homes remain.
// J261: Complete forward/side displacement and double-factor backward-ray phases 246/248/246/245/238/236/238;
// both offset vectors through the double-factor provider 305. Backward or double-offset forms lose de9d0,
// several also c2d00; dc5c0 rises, and reduced-only 41640 also worsens.
// NOT kept: no eligible native repair; opening executable source restored. Mode-three placement and native
// basis/ray homes remain; both original helpers and wrapper restored.
// Lane 15 round 58, NOT kept (233 -> 104, plain): lane 1 r62's form, the mode 3 arm inside the origin copy's else
// with a return; with 0xdc5c0's gravity accessor 0xdc5c0 goes 461 -> 439 with it instead of rising, but that accessor
// costs matched 0xc2d00 (see 0xdc5c0), and alone the form raises 0xdc5c0 585 -> 589. On it (fast):
// forward/up renamed 104, start declared before back 104, up before forward 104, to_head before to_hit 117 (0xde9d0
// stops matching). The homes follow neither names nor declaration order.
// Lane 15 round 61, NOT kept: the 104 form plus, in biped_eye_raise, the first-ray start written through the existing
// real_point3d_set (the same component expressions) gives 55 (plain, 1047 bytes; 0xde9d0 still matches): with
// start's fields no longer written directly, the forward/back and up/start homes and the to_head dot order become
// retail's and the second epilogue merges; only the to_hit dot sums (k + i) + j where retail (and 0xde9d0) sum
// (k + j) + i. Not kept because without 0xdc5c0's gravity accessor (reverted: it costs matched 0x54590) it raises
// 0xdc5c0 585 -> 589; real_point3d_set alone 240 (1032 bytes). Coordinator lead: this pair (0xdef60 233 -> 55
// against 0xdc5c0 +4 positional) needs a ruling. Also tried (fast, on the accessor state): the two `*eye = head`
// arms as one after the block (adjust arm returns) 111 (0xde9d0 34); the raise's first test as an early return 129
// (0xde9d0 119); start renamed 104 (same code); start through vector3f_add_scaled(eye, &back, collision.t * 0.9f)
// 95 (homes and dots retail's, y/z interleave; 0xde9d0 87), with the named t 95 (0xde9d0 matches); on 55:
// dot3f(&direction, &to_hit) 55, both dots direction-first 55, dot3f(&direction, &to_head) 55, side assigned j first
// 61, the test written head-first 52 (0xde9d0 20), to_head computed first 92 (0xde9d0 9), the adjust arm through
// real_point3d_set 70 (0xde9d0 28), to_hit/to_head declarations swapped 55. The to_hit order follows field ranks
// (first direct mentions), not spelling; the adjust arm's direct collision.point/direction mentions rank it, and that
// arm is shared with 0xde9d0.
// Lane 15 round 62 (fast, plain for the kept steps): the start through real_point3d_set kept by ruling 104 -> 55; then
// the start through vector3f_add_scaled(eye, &back, t, &start) 95 (both dots retail's, y and z products copy the
// scale) and, on it, mode 3's side through vector3f_assign 7 (kept; 0xdc5c0 589 -> 588, 0xde9d0 matches).
// Diagnostics: biped_eye_raise on a local copy of the point gives retail's whole second-ray block (both dots and
// colours), so the stores through `point` set the dot order; without the adjust arm both dots go (i, k, j); start.y
// copies the scale only when the adjust arm stores both y and z. On 55: adjust products `0.05f * direction.i` 55,
// named dots 55, `0.05f + dot` 55, the adjusted point in a local 91 (0xde9d0 111), the adjust offset through
// vector3f_scale 22 at 1041 bytes (both dots (i, k, j), 0xde9d0 30), that scale in place on direction 55. On 7:
// `0.9f * collision.t` 7, start before back 7, back at the helper's top 54, t unnamed 7, `point->z += offsets[2]` 7,
// side at the top 7, `-forward->j` 7, `eye->z = eye->z + ...` 7, a `vector->i * t + point->x` helper in any operand
// order 7, sixteen orders of head/forward/up/direction/collision 7, the adjust arm through real_point3d_set 22
// (0xde9d0 28) or through vector3f_scale and real_point3d_set 68, the mode 3 arm inside the origin copy's else 7.
// Lane 1 round 79 (fast, with 0xde9d0/0xdc5c0): forward copied through vector3f_assign 169 (0xde9d0 212); direction
// through vector3f_assign of head - eye 95 (0xdc5c0 585); both `*eye = head` arms through real_point3d_set 96 (0xde9d0 46);
// head and forward declared at their copies 7 (same code); back scaled inside the first ray call's argument 7 (same
// code); start written as three direct rows 61. CL-only listing (it reproduces both 0xdef60's copy-first start.y and
// 0xde9d0's load-first one): start.y turns load-first, with nothing else in 0xdef60 changed and 0xde9d0 unchanged, only when
// one more local filled through an inline helper (real_point3d_set, vector3f_assign, vector3f_scale or
// vector3d_from_points3d, even dead) comes ahead of the back scale; after it, a scalar-only helper, direct stores or a
// pointer local do not move it (forward through vector3f_assign moves it but turns the copy into float moves).
// Lane 1 round 79, not kept: a collision-result type whose constructor sets unknown24 to NONE (declared after
// `head = *eye`, the explicit stores gone) matches 0xdef60 byte for byte (plain; 0xde9d0 still matches, 0xdc5c0 588)
// but loses matched 0x54590 in the full image (its mask/source reload pair swaps): on the base type, on a derived type,
// under two other names, only in the raise, forced inline: 0x54590 lost each time; an unused such type keeps it. On the
// base type it also takes contributor 0xe5d50 406 -> 435. A dead `{ point3f d; real_point3d_set(&d, ...); }` at the
// raise's top (diagnostic) matches 0xdef60 and keeps 0x54590: an existing helper's expansion leaves the image tie alone,
// a new inline function does not. Lead: a natural existing-helper form ahead of the back scale.
// P362: On the native 7-point graph, initial head setter/aggregate/memcpy/memmove
// 160/156/7/163; collision at its sentinel store 7; raised-point/backward-ray identities
// 7/7; immutable named first-ray fraction 7; sentinel memset 7. NOT kept: no byte
// equality; setter/aggregate/memmove also regress shared users. Both helpers restored.
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
	real a5, real radius, bool a7);

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
// score 63 (was 63): extra EBP (the Havok base loaded early), count transport and the hoisted 0x9fff flag clears differ.
// MiniMax H (11-12): no new form, no lower score (63): byte-typed flag bitfields (103), a byte view of +0x349 clearing through the high byte (103), the count read again at the call through BIPED_GET (76) or in place (87) and the char body with a short selection (76) all measure worse; the extra EBP, the widened count test and the hoisted 0x9fff word clears remain.
// MiniMax G (11-12): no new form, no lower score (63): nothing new this round; the extra EBP (the Havok base loaded early), the
// count transport and the hoisted 0x9fff flag clears still differ.
// MiniMax G (11-12): no new form, no lower score (63): the extra EBP (the Havok base loaded early), the count
// transport and the hoisted 0x9fff flag clears still differ.
// MiniMax B (11-12): no new form, no lower score (63): the +0x3aa byte tested in place with the cast value read again
// at the call, the count local as a byte, and the long local kept with the byte tested in place all measure 82 (253 bytes)
// against the current 63 (245 bytes, retail's length), so the sign-extended long local stays; what is left is retail's
// in-place byte test at +0x1a against this build's sign-extended long.
// MiniMax S (11-11): no new form, no lower score (63): five spellings of the settling count - tested in
// place without a local, a byte local, a short local, the count read again through BIPED_GET at the call and
// a byte cast there - score 82, 82, 85, 68 and 82, because without the long local this build keeps the byte
// in BL and adds a register save; retail's 0x1d5120 takes the count in EAX with the component pointer on
// the stack, and that parameter list is declared in another file, so the call site cannot be steered here.
// MiniMax E (11-11): no new form, no lower score (63): the settling count still rides in an extra EBP, the component index is still read into ECX where retail reads it into EAX, and the hoisted 0x9fff flag clears and the split or of +0x134 remain; a byte view of +0x349 beside the flags word (a plain mask 103, a byte-container bitfield 103) and the count re-read through BIPED_GET (94) are all worse.
// MiniMax E (11-08): no new form, no lower score (63); the settling count still rides in an extra EBP, the component index is still read into ECX where retail reads it into EAX, the component's array base still folds into the addressing mode where retail adds it into ECX first, and the body selection still jumps straight out where retail assigns -1 and tests it.
// MiniMax D (11-06, run 49): no new form, no lower score (63); the settling count still rides in an extra EBP across the body validation, and the native body and flag stores keep our schedule.
// MiniMax A (11-03): no new form, no lower score: the extra EBP that carries the settling count across the
// body validation, and the native body and flag store schedule, remain.
// MiniMax R (11-01): no new form, no lower score: the byte test of +0x3aa with the count re-read at the call gives 89 and an if/else-assigned short body 85; retail's `and byte [+0x349], 0x9f` needs a byte field at 0x349 that the header's word flags_348 cannot express.
// Earlier 63 (was 63): EBX input agrees; the extra EBP save, body-index selection and flag-store scheduling remain.
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
// Round 192: retail's call setup (`push &settled`, then the count into EAX, then the rate push) is right to left over
// 0x1d5120 declared (..., rate, count, result). That reorder (definition, declaration and call) alone 63; with the
// second-lookup re-read 68 (the whole call setup is then retail's; EBP still holds the Havok data, ours folding
// `[ecx+ebp+0x18]` where retail adds first and reloads EAX, and 0x9fff is hoisted into EDI); with the short getter
// into a short/long 80/92. Without the reorder: re-read 68, getter into long/short 91/80. All restored.
// Round 65 (lane 9): byte test, short getter and the count re-read through a second lookup 80 (261 bytes), re-read
// from the same biped 89; that with the body in a long local 91 (265 bytes; the body select is then retail's:
// movsx ax / or eax,-1 / movsx edx,ax / cmp edx,-1 without threading); havok_component_get computing the masked
// index first (diagnostic) 91. Lead: retail pushes &settled, then loads the count into EAX, then pushes the rate,
// so 0x1d5120 declares its count before its result pointer; with that order (diagnostic, 0x1d5120 77 unchanged)
// the argument setup is retail's (92 with the long body form; EBP still holds the early Havok data load and 0x9fff
// is hoisted into EDI). 0x1d5120's file was another lane's this round.
// Round 259: Native body field at every selection use with retained/at-use count, entry/at-use
// tag and local/entry-zero query output 67/82/74/85/77/91/83/94. All restored;
// extra EBP, native validation and flag-store schedule remain.
// DH295: Complete per-use component getters with retained/at-use settling count, promoted/native
// main-body result and entry/at-use tag: 74/99/74/89/86/103/87/101. All restored;
// extra EBP, native body transport and flag scheduling remain.
// M319: Full native component-index ownership at admission/getter, long/dword,
// crossed with original/promoted-shared/native-shared/native-explicit body:
// 63/73/75/73/63/73/75/73. All restored; extra EBP, body transport and flag stores remain.
// M325: Complete body-validity bool/byte/short/long before index selection 63/63/63/63;
// false-default bound results bool/long 85/75, true-default 63/63. All restored;
// extra EBP, native body transport and flag-store schedule remain.
// Lane 18 round 34 (fast): the call nested in its own if under the body test 63; char steps local 82 (253 bytes);
// steps read before the definition lookup 63; rate as `0.15f * rate` 63; body selection as an if/else 63.
// M338: Complete validated-query result in bool/byte/short/long, positive/complementary channels:
// 63/63/63/63/63/63/63/63. Original body restored; extra EBP, body transport and flag schedules remain.
// Nonmatching single-consumer query results restored.
// F355: Condition-declared signed settling count in char/short/long/read-only-long, original/entry query output:
// 82/84/63/63/82/84/63/63. NOT kept: no gain; original body restored.
// N344: Existing input/index/output role homes: settling_steps/current_steps, body_index/primary_index,
// validated_body/selected_body, settling_result/settled_steps: 63/63/63/63/63/63/63/63.
// NOT kept: no exact EBP/count/flag repair; original symbols, producers and complete body restored.
// F358: Complete sole-exit success/failure carriers in byte/short/long/dword:
// 63/63/63/63/63/63/63/63. NOT kept: no exact outcome-carrier repair; original count, both return paths and all stores restored.
// F360: Complete settling-period expression at four pre-validation phases, mutable/immutable:
// 68/68/68/71/68/68/68/71. NOT kept: no exact period-lifetime/EBP repair; retail computes the period after body validation.
// Original complete body and helpers restored.
// P298: Existing dword fallback setter with native word setters at pre-call/post-call/failure
// clear boundaries, masks 0..7: 73/78/78/100/73/78/78/100. The fallback memory OR agrees;
// extra EBP and count/body transport remain. Original complete body restored.
// Y569: Nested count/component, selected-body/query and signed bound admission scopes,
// six fresh subsets, complete native selected short and complete native getter graph:
// 63/63/63/63/63/63/67/75. NOT kept: no qualifying native repair; complete original body restored;
// extra EBP, count/body transport and flag stores remain.
// M366: Direct lower/upper body admission without selected sentinel, long/short/char/read-only-long,
// combined/nested query: 70/64/65/70/70/64/65/70. NOT kept: no exact/native-sentinel repair;
// original full body restored; extra EBP, count/body transport and flag schedules remain.
// M391: Removed-bit dependencies at the three flag-reset sites, seven subsets, then all with missing-bit fallback publication: 63/63/63/63/63/63/63/72.
// NOT kept: No exact native mask/count/EBP repair; original field widths, call roles and helpers retained. Original complete body restored.
// F384: Complete admission-time object getter graph, retained/at-use count and
// explicit long/short/shared-promoted/shared-short body contracts:
// 80/85/101/91/85/86/104/100. NOT kept: all regress and grow; original body restored.
// Extra EBP, native count/body transport and flag schedules remain.
// B442: Complementary signed body rejection, direct/default-true producers in
// bool/byte/short/long: 63/63/63/63/85/85/76/75. NOT kept: no equality or qualifying
// native count/EBP/flag repair. Original validation, count, stores and full body restored.
// B444: Explicit flag read/modify/write assignments at pre-call/post-call/failure
// boundaries, seven subsets then all mask-first: 63/63/63/63/63/63/63/63. NOT kept: no byte equality
// or native EBP/body/flag repair; original complete body and word fields restored.
// M391: Removed-bit dependencies at the three flag-reset sites, seven subsets, then all with missing-bit fallback publication: 63/63/63/63/63/63/63/72.
// NOT kept: No exact native mask/count/EBP repair; original field widths, call roles and helpers retained. Original complete body restored.
// Lane 15 round 58 (fast): the main-rigid-body getter at the test and again at the call, with the byte test and a
// (char) re-read 89, with the long count 75; the component index named for the test and the lookup 63; the valid arm
// as `(short)body` 66. The getter with the second-lookup re-read (91) frees EAX but EBP still takes the array base,
// folded into [ecx+ebp+0x18] where retail adds it into ECX first. 0x1d5120's own prologue and convention are retail's.
// The rate through a file-local tick getter 63.
// MiniMax G (11-07): the header getter alone 75 (its 16-bit body selection is retail's movsx ax,al / or eax,-1 /
// movsx edx,ax), with a char steps local the array lookup also matches retail's mov eax,[edx+0x44]; add ecx,eax;
// mov al,byte [ecx+0x18] 99, the byte mask written as a byte 103, the steps byte re-read at the call 82. NOT kept:
// every form that frees EAX moves the argument off EBX onto the stack and the frame becomes __stdcall, so 63 stands;
// original body restored.
// Lane 1 round 79 (fast): byte field test, short main-rigid-body getter and `(char)biped->unknown3aa` at the call 89
// (262 bytes: the byte cached in DL, the argument on the stack in EBP, ret 4); with a named rate 105; with 0x1d5120's count
// before its result pointer everywhere 89 (the call setup then retail's); field test and re-read with the long select 82;
// the select as a short ternary on the field at each use 73. All fold the havok array base into [ecx+base+0x18].
// P385: Initial settling-admission outcome bool/byte/short/long, positive then complementary:
// 63/63/63/63/63/63/63/63. NOT kept: no exact EBP/count/body or flag-store repair;
// original captured count, every observation and complete body restored.
// M3 lane 4 r176 (fast, with 0x1d5120; all on the byte test, the header main-rigid-body getter into a long, the count
// re-read through a second lookup at the call and 0x1d5120's count declared before its result pointer, which re-measures
// 92 at 265 bytes: call setup, body select and settled store are then retail's, and what is left is the Havok array base
// in EBP folded into [ecx+ebp+0x18], the body count and the tick pointer also in EBP, 0x9fff in EDI and the split
// `or` of +0x134): the component looked up once inside the getter call and again at the settle call, no component
// local, 90 (264 bytes: base in ECX, still folded); the whole admission as one condition with no locals, the
// settled arm in the if, the +0x134 flag in the else and one shared final clear 128 (295 bytes: the getter runs twice
// and the final clear is still duplicated and hoisted); the word clear before 0x1c3770 with a byte-view clear of
// +0x349 at both exits 103 (274 bytes: the word clear narrows by itself as retail's, the byte-view clears split into
// load, and, store around the pops); the getter at the test and again at the call, no body local, 80 (261 bytes);
// the first count test through its own lookup as well 126 (288 bytes, the input leaves EBX). All restored.
// M3 lane 5 r167 (fast). Located: the count read at the call as `(char)BIPED_GET(arg_159e6d)->unknown3aa` keeps
// retail's reload from ESI (the lookup folds into ESI, the field load does not; read through `biped` it folds into a
// register, 82); 68 on the base, re-measured. With the rate written as 0.0f (diagnostic) the entry is retail's through
// the body test (no EBP, base added into ECX first), so the extra EBP comes from the rate pointer: ours evaluates the
// rate after the count is pinned in EAX, retail before it. Left there: the header getter loads the size into a register
// (push/pop of EBP around it) where retail compares from memory, 0x9fff hoisted into EDI, the split `or` of +0x134. New
// forms: the success arm falling into one shared final clear with the +0x134 flag in both else arms 90 (273 bytes); a
// char body with a short ternary, the second-lookup count and 0x1d5120's count before its result pointer 76 (255 bytes,
// size compared from memory as retail, but the select is threaded); a named rate on the getter base 97 (re-measure of
// round 96's 97), on the char-body base 79; the call in its own if under the body test on the getter base 92 (same code); the rate through an inline
// helper on the getter base 92 (same code, diagnostic). The mask spelled 0x9fff at the second clear 63 (same code).
// A named rate inside the body test, the call nested under it, 91 (EBP still taken).
// M3 lane 13 r74 (fast): the three clears as two word-bitfield clears each (flags_348_bits field_d/field_e) 63 (same
// code: they merge into one hoisted 0x9fff); byte test of +0x3aa, the header getter into a short and the count re-read
// through a second lookup 80 (261 bytes: the argument stays in EBX, but the Havok base and body count load early into
// EBP); that with the getter into a long 91 (265 bytes, masked aligned 26 -> 21, body select retail's); the short form
// with 0x1d5120's count before its result pointer 80 (same code at the call); the short form with the +0x134 flag
// through bit_vector_set 70 (253 bytes, memory OR as retail). Both sides return with a plain `ret` now.
// M3 lane 9 r110 (fast, control 63): the success arm's first flag clear before the settled store 63 (same code: the
// stores keep our schedule); the failure arm's clear before the +0x134 flag 64; both 64. Permuter (600 s, 5 tries on
// this busy machine): best 62 only by swapping 0x1d5120's and 0x1c3770's arguments (a behaviour change), rejected.
// MiniMax R (11-11): no new form tried this round, no lower score (63): the body is retail's length and the first difference is at +0x1a, where retail tests the +0x3aa byte in place and this build sign-extends it into a long before the test.
// M3 lane 13 r85 (fast with 0x1d5120, which stays 77 in every form). New lead: 0x1d5120's count as its FIRST parameter (in EAX,
// the six stack arguments in their present order; definition, declaration and call together). With the byte test and the count
// read at the call through a second lookup the extra EBP is gone (pushes ecx/esi/edi as retail) 59 (246 bytes), plus the rate
// named just before the call (computed first as retail, but on x87) 57 at 245 bytes; not kept (under the keep bar). What is left on
// that form: the definition goes to EDX (retail EDI), so the pool pointer, component and body each take another register, the count
// loads after the last push (retail right after `&settled`), 0x9fff is hoisted into EDI and the +0x134 `or` is split. Other forms:
// count-first alone 63 (same code), with the count read through `biped` 64, with the header getter 80/82 (259/264 bytes, a push/pop
// of EBP around the size compare); the rate local as `real const` 57 (same code); the definition read at the call, no entry local,
// 73; the first two clears through word_flags_set 66; the +0x134 flag through bit_vector_set 65 (238 bytes), with count-first alone
// 73. With the count between the rate and the result pointer (round 192's order) and the named rate 65 (the tick pointer then takes
// EAX before `&settled` as retail, but the rate goes through x87 and is pushed from EBP). Re-measured: the second-lookup re-read
// alone 68; round 192's order with it and the getter 92, with `(char)biped->unknown3aa` 81, plus the getter 99.
// MiniMax A (11-13): no new form, no lower score (63): this build sign-extends the +0x3aa byte into a long before the test where
// retail tests the byte in place, and the extra EBP push stays.
// M3 lane 5 r179 (fast with 0x1d5120/0xe0d70/0x1c3770). Base re-measured: 0x1d5120 declared (..., rate, count, result),
// the +0x3aa byte tested in place and the count read at the call through a second lookup 68. New: one `word *` to
// flags_348 at the three clears 67 (the +0x134 `or` becomes retail's memory RMW and the settled store comes before the
// first clear as retail; 0x9fff stays hoisted into EDI); one byte pointer to +0x349 cleared with `&= ~0x60` 99 (267 bytes:
// load/and/store, 0x9f hoisted into BL, the argument moves to the stack); the word pointer on this body 73 (236 bytes);
// on the count-first form (lane 13 r85) 65 (238 bytes; its listed bit_vector_set form again, differing only in where the
// settled store sits), with the rate named before the call 65 (the rate on x87); on the word-pointer form the count through
// one `byte *` to +0x3aa at the test, the call and the store 82 (253 bytes: cached in BL, the argument on the stack), the
// clears through a pointer to the s_biped_flags_348 bitfield view 67 (same code as the word pointer). Every
// form keeps `mov edi, 0x9fff` on the success path: a 16-bit mask used twice is put in a register, where retail's two
// byte RMW clears share no constant. Body unchanged.
// This round: retail passes the settling count to 0x1d5120 in EAX and takes six stack arguments, while this build passes seven
// because 0x1d5120's declaration and definition live in another file, so the call site cannot be matched from here.
// P475: Standard signed settling/lower/upper admission contracts, seven subsets
// then all complementary: 111/131/126/155/149/153/174/174. NOT kept: no exact
// EBP/count/body/flag repair; original predicates and complete body restored.
// MiniMax H (11-14): no new form, no lower score (63): retail takes the settling count in EAX and passes six stack arguments where this build passes seven, and 0x1d5120's declaration and definition are in another file.
// @retail 0xe0c70
void function_e0c70(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	s_biped_definition *definition = BIPED_DEFINITION(biped);

	long steps = (char)biped->unknown3aa;
	if (steps && biped->havok_component_index != NONE)
	{
		s_havok_component *component = havok_component_get(biped->havok_component_index);
		long body = component->unknown18;
		long rigid_body = body >= 0 && body < component->rigid_bodies.size ? body : NONE;
		long settled;

		if (rigid_body != NONE &&
			function_1d5120(component, rigid_body, 9, (long)definition->biped.physics.pill_shapes.address + 0x20, g_510c54->rate * 0.15f,
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

bool __stdcall function_e1b60(long arg_159e6d, point3f const *point, real value);

/* starts a fallen biped's ragdoll from its main rigid body */
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
			real value = component->function_1d1230(rigid_body);
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

PRIVATE inline point3f *biped_rigid_body_position(s_havok_component *component, long rigid_body_index)
{
	return (point3f *)(*(byte **)(*(byte **)((byte *)component->rigid_bodies.data + rigid_body_index * 0x60 + 0x40) +
		0x3c) + 0x70);
}

/* the dead biped's ragdoll: when it falls over (+0x34b 0 to 1) its bodies
   get a push along the killing blow, spread from the hit body; a fallen
   one settles and goes to sleep (2); one that gets up again stands */
// score 362 (was 362): frame 0x48 against retail's 0x4c (retail's dead flag byte before the 0xe16b0 call, a HELD
// contributor form below), the +0x34b state byte zero-extended, scalar, centre and result homes. Round 85 (m3 lane 13) at
// the end of this note; the earlier first lines are kept below as "Earlier first line".
// MiniMax G (11-13): no new form, no lower score (362): the diff re-read this round puts the first difference at the state byte at +0x34b, read zero-extended here and sign-extended in retail, and the second at the flags word at +0x348 that retail reads and shifts into a byte local before the 0xe16b0 call, which has no counterpart here, so the body stands.
// MiniMax B (11-13): no new form, no lower score (362): our frame is 0x48 against retail's 0x4c, and this build reads the +0x34b byte unsigned where retail reads it signed; the signed form measures 361, under the eight-point bar for a function still above 30, so the body stands.
// MiniMax H (11-12): no new form, no lower score (362): the only form this round - the state byte at +0x34b read with a signed cast - measures 361, under the keep gate, so the body stands.
// MiniMax G (11-12): no new form, no lower score (362): nothing new this round; the frame 0x48 against retail's 0x4c (retail's
// dead flag byte before the 0xe16b0 call) and the held homes remain.
// MiniMax G (11-12): no new form, no lower score (362): frame 0x48 against retail's 0x4c (retail's dead flag byte
// before the 0xe16b0 call) and the held homes remain.
// MiniMax B (11-12): no new form, no lower score (362): the switch reads the +0x34b byte zero-extended where retail
// sign-extends it; typing the field char (or casting it in the switch) measures 361, one point and under the gate, and
// the field type also changes contributor code such as 0xe18d0, so the byte field stays.
// MiniMax E (11-11, run 80): no new form, no lower score (362): retail's frame is 0x4c and ours 0x48, retail sign-extends the ragdoll state byte at +0x34b where we zero-extend it, and retail keeps the flags bit 9 in a stack slot before the 0xe16b0 call, which we do not; a signed cast on that byte measures 361 (under the keep bar) and the held form that feeds the flags bit 9 to the matched 0xe16b0 as an unread parameter changes a matched function's signature and needs the owner's ruling.
// MiniMax H (11-11): no new form, no lower score (362); the headshot scale clamp written as an if-statement (362), with its test reversed (368), and the farthest-distance compare spelled the other way (362) all leave the code as it was, so the frame stays short of retail's 0x4c.
// MiniMax R (11-09): no new form, no lower score (539); the state byte read as a signed byte gives 538, under the keep bar, and the frame stays 0x44 here against 0x4c in retail.
// MiniMax R (11-06): no new form, no lower score (520); the frame is 0x44 here and 0x4c in retail, so the local area is two words short.
// MiniMax B (11-11): at 362 (from 490): the definition pointer read once at the top of the hit block (retail computes
// it at 0xe11cc and keeps it in a stack slot) and the push scale through vector3f_scale. Still differs: retail's frame is
// 0x4c and ours 0x48, retail sign-extends the ragdoll state byte at +0x34b where ours zero-extends it, and retail keeps
// the flags bit 9 in a stack slot before the 0xe16b0 call, which we do not.
// MiniMax B (11-11): no lower score (362 kept); a signed cast on the ragdoll state byte in the switch reproduces retail's
// sign-extending load but measures 361, under the keep bar, so it was reverted. The held form that adds an unread bool
// parameter to the matched 0xe16b0, fed from a named local holding the flags bit 9, reaches 342 at a 0x4c frame and keeps
// 0xe16b0 matched, but it changes a matched function's signature and needs the owner's ruling.
// Earlier first line: at 362 (from 490): round 101 (m3 lane 12), round 104 (m3 lane 16) and round 112 (m3 lane 12) notes are at the end of this note. Earlier at 490 (was 517). Earlier at 517 (was 539). Earlier at 539 (was 520): two value fixes this round raise it: case 1 passes g_4687a4 as 0xb77d0's angular velocity too
// MiniMax S (11-11): the definition pointer (BIPED_DEFINITION_GET(biped)) is now a local of the hit block, as retail computes it at 0xe11cc, and the direction scale goes through vector3f_scale; our entry spill of the biped is gone and the score falls to 361. Still differs: the frame is 0x48 here against 0x4c in retail, so every stack reference sits four bytes low, and retail keeps the centre sums in xmm4-xmm6 across the mass call with the total and the hit in memory.
// Earlier first line: at 362 (from 490; 361 with the biped's unknown34b typed char, not kept by the coordinator 11-11: 0xe18d0 is contributor code): round 101 (m3 lane 12), round 104 (m3 lane 16) and round 112 (m3 lane 12) notes are at the end of this note. Earlier at 490 (was 517). Earlier at 517 (was 539). Earlier at 539 (was 520): two value fixes this round raise it: case 1 passes g_4687a4 as 0xb77d0's angular velocity too
// (retail 0xe0f75: `mov eax, [0x4687a4]; push eax` with EAX still holding it, which matched 0xb77d0 takes as its third
// argument; ours passed NULL), and the single-body fallback push runs when the component's flag 11 is clear (retail
// 0xe118f jumps to the fallback at 0xe15b7; ours skipped it). Frame 0x44 versus 0x4c; result byte, scalar homes and
// native point loading remain.
// Earlier: 540 (was 568; the old 774 was stale): the logic was walked against retail block by block and agrees
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
// M288: Uniform per-use tag fields 530 (plain, kept); complete center/hit setters
// alone 571/569, tag+center/tag+hit/both setters/all three 562/562/556/546; all plus
// mass/push role names 546. Every caller and all other file measurements stay unchanged.
// P258: Advancing body receivers in mass/farthest/both scans 530/530/530;
// push/mass+push/farthest+push/all scans 534/534/534/534; all receivers
// with uniform per-use counts 544. Original indexed scans and body restored.
// Round 324: complete centroid scalar storage in both input-read orders, each with/without
// reciprocal and all weighted products beside their consumers: 562 each. Frontloaded
// weighted products also 562, reverted. Original aggregate centroid and body restored.
// M309: Existing position helper before/after mass query 530/548; native signed view
// 529/547. Common exit 527/509, with signed view 527/508 (redundant result stores,
// restored). Deduplicated exits 531; per-path result 591; false-default/result-first
// 542/542. Original complete body, owning field and helper declarations restored.
// Round 326: Existing complete centroid/direction/fallback scale producers,
// seven subsets, then all with the per-body impulse scale before lift: 530 each.
// Original component producers and body restored; frame 0x44 versus 0x4c,
// result byte, state width and centroid/impulse homes remain.
// Round 532: separate output-query/body admission, case-one-first dispatch and case-owned query
// workspaces, seven subsets 530/541/541/530/530/541/541; all with complete query-role names 541.
// All remain 1834/1908 bytes, frame 0x44 versus 0x4c. Original compound query, case order and body restored.
// F329: Shared push/fallback velocity workspace, hit snapshot at its direction consumer,
// and the existing complete along-difference producer, seven subsets:
// 530/548/530/548/530/563/563; all with velocity role name 563. All restored;
// frame 0x44 versus 0x4c, result byte and native scalar/vector homes remain.
// Round 371: Native per-body position values in mass/farthest/push scans,
// subsets 1/2/4/3/5/6/7: 546/513/506/515/524/494/499; all with immutable
// hit producer 499. Both distance-scan values (494 plain, 1874/1908, frame 0x50
// versus retail 0x4c) NOT kept: struct copies in two of the three scans, the subset
// picked by score (mass scan keeps its pointer). Body restored at 530.
// F341: Last-use scopes for admission inputs, the hit position and the
// projection scalar, seven subsets then all with complete role names: 530 each.
// All restored; frame 0x44 versus 0x4c, signed state/result byte and point homes remain.
// Y539: Test-produced ragdoll/model admission and the existing bit-test producer,
// subsets 1/2/4/3/5/6/7: 530/537/530/537/530/537/537; complete graph with
// natural shared completion 539 (1859 bytes, incoming index differs). Original
// guards, bit tests, exits and complete body restored; frame/result homes remain.
// M322: Plain three-axis centroid/projection/impulse-scale loops, subsets
// 1/2/4/3/5/6/7, then all with centroid-normalization loop:
// 567/562/556/588/598/598/615/631; 1732-1816 versus retail 1908 bytes.
// All regress; original complete body and arithmetic restored.
// P292: Actual computed-vector aggregate initializers for hit difference/body impulse/
// fallback impulse, subsets 1/2/4/3/5/6/7, then all with immutable hit at its producer:
// 530 each. Original declarations, component producers and entire body restored;
// frame 0x44 versus 0x4c and native result/state-byte, point and scalar homes remain.
// AC500: Real array-range mass/farthest/push masks 1-7: 541/530/519/516/532/514/529; native final size 539.
// NOT kept: 519 combines tying/regressing parts; 516/514 hold a cursor unlike native base/size reloads; 529 below gain gate.
// Original complete body restored; frame 0x44 versus 0x4c, result/state bytes and point/scalar homes remain.
// DH314: Weighted centroid via shared scaled-add/point setter/vector setter/weighted vector
// plus add: 530/530/530/546; each with a named three-use reciprocal: 530/530/530/546.
// The weighted vector shortens to 1818/1908 and regresses; frame/result homes persist.
// Original aggregate, complete body and every helper restored.
// N344: Entry-only uninitialized total/mass/farthest/push/scale/strength+lift/dot/query-value
// declarations, original producers fixed: 530/530/530/530/530/530/530/528.
// NOT kept: query-output entry scope gains only two; no exact scalar/frame repair. Original body restored.
// P292: Actual computed-vector aggregate initializers for hit difference/body impulse/
// fallback impulse, subsets 1/2/4/3/5/6/7, then all with immutable hit at its producer:
// 530 each. Original declarations, component producers and entire body restored;
// frame 0x44 versus 0x4c and native result/state-byte, point and scalar homes remain.
// B395: One actual result exit, byte/signed-byte/short/word accumulator, case-one true
// at completion/admission: 539/539/539/539/523/522/523/522. NOT kept: 522 reaches the
// gain bar but uses EAX input/ret instead of native stack input/ret 4 and a word result
// conversion. Original result graph, all input observations and complete body restored.
// AC504: Push-setting receiver at random/centroid/average/push boundaries,
// mutable then readonly: 561/562/530/530/561/562/530/530. Early receivers
// give frame 0x48 and 1821 bytes; later forms tie original 0x44/1834.
// Original whole body restored; result/state bytes and point/scalar homes remain.
// P301: Existing typed rigid-body getter/declared motion field at hit/mass/farthest/
// push sites, then hit+mass/both distance scans/all sites: 530 each; all with the
// position acquired after the pure mass query 548 (1816 bytes). Original raw access
// chain and complete body restored; native 0x4c frame and result/position homes remain.
// M346: Returned-result ownership across every existing exit, with actual case-one success
// at admission, bool/byte/char/signed-char/short/word/int/long: 530 each. No common-exit
// rewrite; all retain frame 0x44 and 1834/1908 bytes. Original body restored.
// M369: Existing centroid/hit transfers, native/memcpy/memmove pairs except native/native:
// 530/528/530/530/528/514/514/512. NOT kept: two-point leads fail the gate; seed memmove
// adds a CRT call absent from the native three-word inline copy; original transfers/body restored.
// DL259: Progressive actual impulse completion at both normalizers,
// bool/byte/char/signed-char/short/word/int/long: 530/530/530/530/530/530/530/530.
// NOT kept: no native result/frame repair; original result, scans and body restored.
// Native final-loop size observation remains a possible unretained correction.
// AC546: Existing owning-word writer for deferred/failure/notification publications,
// seven subsets then existing state bitfields: 520/545/535/530/520/545/535/534.
// Set-only 530; clear-only 520 plain, 1844/1908 bytes. Kept the clear-only writer:
// native direct byte AND in both completion paths; frame/result/state homes remain.
// B437: Actual body byte offsets stepped inside mass/farthest/push loop subsets
// 1/2/4/3/5/6/7 and complete unsigned offsets: 530 each. Original signed ordinals,
// base observations and body restored; frame/result/state/point homes remain.
// P337: Complementary returned outcome at original exits, bool/byte/char/signed-char/
// short/word/int/long: 530 each. NOT kept: no native frame/result repair; original
// positive result, all exits, observations and complete body restored.
// M386: Genuine hit-to-difference in-place vector, component/difference-helper/vector-setter/
// point-setter producers with native/library input transfers: 529/530/529/529/529/530/529/529.
// NOT kept: no native frame/result-home byte equality; original separate point and direction restored.
// M388: Actual bound-selected push, maximum-distance result and side-factor producers,
// masks 1/2/4/3/5/6/7, then named reciprocal: eight fast 530s, 1834/1908 bytes.
// NOT kept: no frame/result-home repair; original complete body restored.
// AC549: Independent lexical identities for direction/center/hit/along/change/query value/
// hit ordinal/completion result: eight 520s, 1844/1908 bytes. NOT kept: no native
// frame or stack-home repair; original names and complete body restored.
// AC546: Existing owning-word writer for deferred/failure/notification publications,
// seven subsets then existing state bitfields: 520/545/535/530/520/545/535/534.
// Set-only 530; clear-only 520 plain, 1844/1908 bytes. Kept the clear-only writer:
// native direct byte AND in both completion paths; frame/result/state homes remain.
// Y610: Actual state-byte/flag-word/body-array lvalue identities, subsets 1/2/4/3/5/6/7,
// then all with component-record identity: 520/520/520/520/520/520/520/520 (fast control 520).
// NOT kept: no byte equality;
// original memory observations, complete body and names restored; frame/result homes remain.
// Lane 15 round 58 (fast): case 0 as positive nested ifs with else arms for the three flag exits (retail's tail
// order: 0xe16b0 failure, 0x138fa0 failure, g_5107ed arm, all after the main body) and one result: 502; with case 1
// and default returning directly 445 (aligned 168 against 198), but the argument then moves to EAX and 0xdd360 goes
// 222 -> 224 (retail pushes it); with one read of the argument through its address (diagnostic) 461, 0xdd360 222, the
// result byte in the argument's home slot as retail. Bisect: only the false exits merging with the true paths (result
// a real variable) move the argument to EAX: 525/224; `break`/`return result` elsewhere 520/222. Every return as
// `break` with one `return result;` 523.
// Lane 1 round 79 (fast, on the fixed body unless noted): the value fixes 520 -> 539 (kept); the hand-written body
// position chains through the existing biped_rigid_body_position 539 (kept, same code); `(char)` switch 538 (520 before the
// fixes: also 538); `held` written into the call 537; the three loops on `component->rigid_bodies.size` (retail re-reads
// it after the velocity call in the third) 546 (1819 bytes). Lead: one `return result;` exit with breaks (retail's
// every exit reloads the result byte) 524 at retail's frame 0x4c, `(char)` switch the same, result declared first 524,
// `held` inline 531, with the size field 530; it moves the argument into EAX (`mov ebx, eax`, retail keeps it on the
// stack in EBP with the biped in EBX) and its only caller 0xdd360 goes 222 -> 224, so it is not kept. Also on the fixed
// body: along through vector3d_from_points3d 537; the fallback through vector3f_scale 539; hit initialised at its
// declaration 539.
// P363: Both projection scans through computed relative vectors, shared difference/aggregate/setter/dot
// producers: 535/535/535/535; each with mass query before the position lookup 576/576/576/576.
// NOT kept: four-point leads miss the margin; reordered forms regress. Original complete body,
// numeric trees, bounds, state/fallback values and call tuples restored; frame/result homes remain.
// P374: Plain pretested mass/farthest/push scans, seven subsets then independent ordinals:
// 539/539/539/539/539/539/539/539. NOT kept: no gain; original complete for-loop body restored.
// Native frame/result-byte and point/scalar homes remain; all bounds and input values preserved.
// P376: Direction projection via products/accumulator/two-term prefix/const products, each plain/uniform row parts:
// 539/539/539/539/539/539/539/539. NOT kept: no native frame/result equality.
// Original complete body, projection association, observations and scan bounds restored.
// P378: Full flag/query/body-selection-to-normalization impulse-success ownership in
// bool/byte/char/signed-char/short/word/int/long: 539/539/539/539/539/539/539/539. NOT kept:
// no native result/frame repair. Original complete body and all original outcomes restored.
// T71: Sole-return completion label, admission-owned
// bool/byte/char/signed-char/short/word/int/long results: 510/535/535/535/504/504/520/520. NOT
// kept: frame 0x4c recovers, but all take EAX input/plain ret and ordinary result homes instead
// of native stack input/ret 4 and argument-home byte. Original complete body restored.
// P389: Actual two-bit held predicate in byte/char/signed-char/short/word/int/long/dword
// storage at its existing producer, with original inverted query and all exits:
// 539/539/539/524/524/524/524/524. NOT kept: no exact native frame/home recovery;
// one-use width changes require equality (native AL, wider AX/EAX); original body restored.
// Round 101 (m3 lane 12; fast with the caller 0xdd360, plain where marked; "al" = aligned instruction differences, 338 for the
// old body, because retail's frame is 8 bytes larger and shifts every stack offset). Kept, 539 -> 517 (al 311, the index on the
// stack in EBP and the biped in EBX as retail, 0xdd360 222): case 0 as nested if/else arms (retail lays the 0xe16b0-failure,
// 0x138fa0-failure and g_5107ed arms out of line at the tail, 0xe1621/0xe163c/0xe1650) with one `return result;`, and the mass
// query ahead of the position read in the mass loop (retail calls 0x1d0bb0, then reads the body). The nested arms alone 487, but
// the index then moves to EAX and 0xdd360 goes 222 -> 224; the mass-first order is what keeps the biped in EBX unspilled.
// NEEDS A RULING, not kept (diff in build/r101/e0ef0-404.patch): on top of that, a definition pointer read before the two random
// scales (retail stores it at 0xe11eb and reads +0x258 from it after the normalizes) and the hit position through
// real_point3d_set (retail's movss loads) 404 plain (al 279, frame 0x48 against 0x4c, 0xdd360 222). The pointer is a local copy
// beside the macro reads earlier in case 0 (a mix); the uniform forms: per-use reads 538, one pointer for every use in case 0
// (declared at the case top) 547 (al 227, first difference +0x22). On the 404 form: the hit as an aggregate copy 457 (index to
// EAX, 0xdd360 224); mass query after the position 556 (0xdd360 224); the body count read from the field at each use 412 (al
// 255: retail does re-read it after the velocity call at 0xe158f); with that, a `(char)` switch 411 plain (retail's movsx).
// Other forms with the nested arms and mass first: field count 523 (al 294); field count + hit setter 538 (al 267); count local +
// hit setter 535 (al 287); nested + field count, mass after the position 520 (0xdd360 224).
// Singly on the old body: one result exit with breaks 524 (al 365, index to EAX, 0xdd360 224); mass query first 580 (al 324);
// with the field count 589 (al 316); plus the definition pointer 598 (al 344, index to EAX); plus the hit setter 614; plus the
// single exit 509 (al 299, index in EAX); that without the ragdoll pointer local 531; single exit + mass + field count + hit
// setter, no definition pointer 552 (al 283, index on the stack).
// On the 412 form: the hit stored straight into the direction vector with `-=` after the loop 412 (same code); `total = mass +
// total` same code; the total added first in the loop al 256; the centre through real_point3d_set 388 (al 258); the total declared
// ahead of the random scales 382 (al 200, frame 0x4c, but its zero store then precedes the calls where retail's follows them: a
// declaration order picked by score); five declaration/assignment orders of centre, direction and total 411 each (same code);
// the six term orders of the two projection sums 404 each on the 404 form (same code; retail sums x, z, y, ours z, y, x).
// Still differs: retail's frame holds a dead flag byte, `(flags_348 >> 9) & 1` stored at 0xe1142 just before the 0xe16b0 call and
// never read (an unread local of that value is removed: 509, same code). HELD (needs the owner's OK; 0xe16b0 is a contributor's
// matched function; measured and put back, diff in build/r101/held-e0ef0.patch): that value in a named local passed to an unread
// second bool parameter of 0xe16b0 reproduces the store with 0xe16b0 still matching (493 on the single-exit form, al 262; 415 on
// the 404 form, al 244, but there the index goes back to EAX and 0xdd360 to 224; on the kept 517 form 490 fast, al 216, 1893 bytes, frame 0x4c with the first difference at +0x22, the index still on
// the stack and 0xdd360 222). Retail also keeps the centre sums
// in xmm4-xmm6 across the mass call with the total and the hit in memory (ours keeps the hit and the total in registers and the
// centre in memory), loads the component pool before masking the index, and takes the hit body in EBP once the index is dead.
// Round 164 (m3 lane 6, fast with 0xdd360, which stays 222 unless noted): the mass as a `long` local 526; the hit-body lookup
// in its own nested if 517 (same code); a for-scoped index per scan 517 (same code); the difference vector loaded from the hit
// point and reduced by the centre in place 535 (the same code as the hit setter, re-measured 535: the hit then holds xmm5-xmm7
// across the mass loop and the centre goes to memory, retail the reverse); the total summed before the centre rows 514 (three
// points, an order picked by score); that with the three loops on the size field 523 (the push loop is then retail's, offset in
// EBP and index in memory, but our mass-loop cursor takes EBX and the biped spills).
// HELD (needs the owner's OK; 0xe16b0 is a contributor's matched function; measured and put back, diff in
// build/r164/held-e0ef0.patch): 0xe16b0 given an unread `bool` second parameter, passed a named local holding
// `(flags_348 >> 9) & 1`, with the three loops on the size field: 360 plain (aligned 259, masked 120, 0xe16b0 MATCH, 0xdd360
// 222); retail's dead byte store appears (retail stores that byte at 0xe1142, between the index push and the call). The
// measurement reproduces the store; it does not show that 0xe16b0's source had that parameter. It needs the owner's OK and a
// byte match: the named local is a one-use copy, and the value passed straight into the call gives 523 (no store). On the
// held form: the total
// summed first 352 fast (masked 125); a definition pointer at the hit block's top 350 (frame 0x4c as retail, but the index goes
// to EAX and 0xdd360 to 224), that with the hit setter 416, with the nested hit-body if 350, with a case-0 definition local as
// well 352; the hit setter alone 519.
// Round 104 (m3 lane 16, fast with 0xdd360 (222 throughout) and 0xe16b0; kept change confirmed plain): kept 517 -> 490 (1892 B),
// the push block through existing helpers: the projection dot through dot3f, the projection through vector3f_add_scaled (scale
// `0.0f - dot`; retail subtracts with subss, so that site is not retail's yet, and no subtract-scaled helper exists), both scans'
// distances through a per-body offset vector (vector3d_from_points3d, then dot3f). Singly: dot3f 517, scans 515, add_scaled 514;
// scans + add_scaled 498; with the along vector through vector3d_from_points3d 498. On 490: the push change through vector3f_scale
// 490; `direction *= push` through vector3f_scale 497; the fallback change through vector3f_scale 490; along through
// vector3d_from_points3d 490; the push limit as a two-use local 490; the hit setter re-measured 515; the size field at each loop test
// re-measured 518. The biped's +0x34b declared `char` (retail's movsx; needs 0xe18d0's pointer local retyped, contributor code)
// 517 -> 516 on the old body, 497 on scans + add_scaled; 0xe18d0 and the other users stay matched, 0xdd360 222.
// NEEDS A RULING (mix; diff against main in build/r104/held-e0ef0.patch): a definition pointer local at the top of the hit block
// (before the two random scales, where retail computes it at 0xe11eb) used for both +0x258 reads, beside the macro reads at the
// model lookup and the 0x1f0 test: 490 -> 368 fast; with `direction *= push` through vector3f_scale 362. It removes our entry spill
// of the biped (`mov [esp+0x10], ebx`): ours gives the third scan's counter EBX and reloads the biped after the loop, retail keeps
// that counter in memory and the biped in EBX, so the whole body realigns. On 362 with the held 0xe16b0 form (unread bool second
// parameter fed from a named local, see round 164) 342 at 1924 B, frame 0x4c, and with the `char` field too 341 (first difference
// +0x43: the hit point's movss copy and the mass loop's load order remain). Uniform forms of the pointer: declared at the top of
// case 0 for all four uses 496; declared at the model lookup 493 (frame 0x4c, masked aligned 90 against 155, but the entry spill
// stays); declared at the function top 490; on the model-lookup form `direction *= push` through vector3f_scale 486, the hit setter
// 528, the limit local 493, the change and fallback scales 493, the size field at each loop test 519 (521 with the push scale).
// Without the scans' offset vectors (dot3f + vector3f_add_scaled only) 501, so the offset vectors add 11. The 0x1f0 test and
// flags_348 bit 10 through field_bit_test: 490 on the kept body, 486 on the model-lookup pointer form (same code).
// Round 112 (m3 lane 12, fast with 0xdd360 (222 throughout), 0xe16b0 and 0xe18d0; masked = aligned count with registers and
// offsets masked, 89 on this body, aligned 226): every flags_348 bit write through word_flags_set (the 0x138fa0-failure arm's
// bit 9 and the g_5107ed arm's bit 8 for the byte-pointer `|=`s) 505 at 1885 bytes (masked 85: retail's fused `or byte ptr
// [ebx + 0x349], 2`); with the 0xe16b0-failure arm's `|= 0x100` through it too 505 (masked 83, aligned 215: that arm's
// stores then run in retail's order, `mov byte ptr [ebx + 0x34b], 0` before `or word ptr [ebx + 0x348], si`). Not kept (the
// count rises as the body shortens), but it is retail's shape at all three sites: a lead once the frame grows. On that form:
// the hit through real_point3d_set from a position pointer 522 (1869 B: the hit takes xmm5-xmm7 across the mass loop and the
// centre goes to memory); the three scans on the size field 526 (1864 B, frame 0x44, masked 77; the mass-loop cursor takes
// EBX and the biped still spills); both re-measure listed forms on the new form. On this body, re-measured: `along` loaded
// from the hit position and reduced by the centre in place, no hit local (round 164's form) 515 (1876 B; along takes
// xmm5-xmm7, the centre goes to memory). No other untried natural form was found. Retail keeps the
// centre in xmm4-xmm6 across 0x1d0bb0 (which writes only xmm0-xmm3) with the total and the hit (stored into along's slot by
// movss) in memory; each form here that copies the hit as floats gives it those registers instead.
// M3 lane 13 r85 (fast with 0xdd360 (222), 0xe16b0 and 0xe18d0, both still matched): retail adds the Havok pool base into
// the component first and tests `[edi + 4]`, ours folds `[edi + esi + 4]` (as in 0xe0c70); the component's flag 11 through
// the header's `flag11` bit 362 (same code); the component index in a named local for its NONE test and the lookup 362 (same
// code); `value` declared before `direction` 362 (same code); `held` through the file's field_bit_test 362 (same code); the
// fallback test as `size >= 1` 367 (1891 bytes).
// P438: Original biped/component/definition bindings, cached count and strength/lift/
// mass/projection bindings fixed independently: eight 362 at 1892/1908 bytes.
// NOT kept: no frame or native component-address repair; original entire body restored.
// MiniMax A (11-13): no new form, no lower score (362): our frame is 0x48 against retail's 0x4c, and this build reads the +0x34b
// byte unsigned where retail reads it signed (the signed form was measured at 361 and is not keepable on its own).
// M3 lane 5 r179 (fast with 0xdd360/0xe16b0/0xe18d0): case 0's flags_348 reads and writes through one `word *flags` 362
// (other code: the 0xe16b0-failure arm stores +0x34b before its `or` as retail; the `or byte [+0x349], 2` stays split).
// HELD (needs the owner's OK; 0xe16b0 is contributor code; measured and put back, diff in build/r179/held-e0ef0.patch):
// the unread `bool` second parameter of 0xe16b0 fed from a named local of `(flags_348 >> 9) & 1`, alone on this body,
// 342 (1924 bytes, frame 0x4c as retail; 0xe16b0 and 0xe18d0 still match, 0xdd360 222). No other untried natural form
// (lane 12 r112). Body unchanged.
// P471: Standard array normalization/difference, maximum/minimum reductions, impulse/
// fallback range products and hit/seed range copies: 527/396/381/518/519/503/346/349.
// NOT kept: range copies add a call absent from the native inline transfers; hit
// copy grows frame to 0x58 versus native 0x4c. Original body and includes restored.
// P473: Standard first-product-seeded direction/farthest/push reductions, masks 1/2/4/3/5/6/7,
// then all with explicit plus/multiply: 381/427/355/409/369/430/405/596. NOT kept: no native frame/result
// equality; original complete body, existing dots and include set restored.
// The 355 reduction gains seven above 30; all other reduction forms regress.
// P476: Current hit-tag/shared-direction graph: existing weighted-add/centroid-scale/impulse-scale producers, masks 1/2/4/3/5/6/7, then fallback scale. Scores 362/362/362/362/362/362/362/362.
// NOT kept: unmatched diagnostic source forms; original executable body restored.
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
		byte *model;

		if (!(ragdoll[0x11] && !ragdoll[0x12] && (!function_e4050(arg_159e6d) || (*((byte *)biped + 0xc1) & 1))) &&
			!function_1ecef0(ragdoll))
		{
			break;
		}
		model = g_4e3b44[BIPED_DEFINITION(biped)->object.model.tag_index & 0xffff].bytes;
		if (*(long *)(model + 0x24) == NONE || TEST_FIELD_BIT((biped->flags_348 >> 10) & 1))
		{
			break;
		}
		result = true;
		if (!g_5107ed)
		{
			if (!TEST_FIELD_BIT((BIPED_DEFINITION(biped)->biped.flags >> 11) & 1))
			{
				bool held = TEST_FIELD_BIT((biped->flags_134 >> 27) & 1) && !TEST_FIELD_BIT((biped->flags_348 >> 9) & 1);

				if (function_138fa0(!held))
				{
					long primary = function_10f720(arg_159e6d, true);
					long secondary = function_10f720(arg_159e6d, false);

					if (TEST_FIELD_BIT((biped->flags_134 >> 27) & 1) || (primary != 2 && secondary != 2))
					{
						biped->unknown34b = 1;
						function_b9b90(arg_159e6d, false);
						if (g_5107ec)
						{
							biped->flags_c0 |= 0x100;
						}
						if (function_e16b0(arg_159e6d))
						{
							(*(long *)((byte *)g_4e6948 + 0x1130))++;
							if (biped->havok_component_index != NONE)
							{
								s_havok_component *component = havok_component_get(biped->havok_component_index);
								vector3f direction;
								long value;
								long hit_body;
								bool pushed = false;

								if (TEST_FIELD_BIT((component->unknown04 >> 11) & 1) && function_1ed430(ragdoll, &direction, &value) &&
									(hit_body = function_183c60(arg_159e6d, value)) != NONE)
								{
									s_biped_definition *definition = BIPED_DEFINITION(biped);

									real strength = function_259d0(&g_4e7408->unknown0, NULL, 0, 1.5f, 3.25f);
									real lift = function_259d0(&g_4e7408->unknown0, NULL, 0, 0.75f, 1.5f);
									point3f center = *g_468788;
									point3f hit;
									vector3f along;
									real total = 0.0f;
									long count = component->rigid_bodies.size;
									long i;

									hit = *biped_rigid_body_position(component, hit_body);
									for (i = 0; i < count; i++)
									{
										real mass = (real)(long)havok_component_rigid_body_mass_get(i, component);
										point3f const *position = biped_rigid_body_position(component, i);

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
										real dot = dot3f(&along, &direction);

										vector3f_add_scaled(&direction, &along, 0.0f - dot, &direction);
										if (function_30bf0(&direction) != 0.0f)
										{
											real push = definition->biped.headshot_acceleration_scale > 0.7f ? 0.7f : definition->biped.headshot_acceleration_scale;
											real farthest = 0.0f;

											push *= strength;
											vector3f_scale(&direction, push, &direction);
											for (i = 0; i < count; i++)
											{
												point3f const *position = biped_rigid_body_position(component, i);
												vector3f offset;
												real distance;

												vector3d_from_points3d(&center, position, &offset);
												distance = dot3f(&offset, &along);

												if (!(farthest > distance))
												{
													farthest = distance;
												}
											}
											for (i = 0; i < count; i++)
											{
												point3f const *position = biped_rigid_body_position(component, i);
												vector3f offset;
												real distance;

												vector3d_from_points3d(&center, position, &offset);
												distance = dot3f(&offset, &along);
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
							word_flags_set(&biped->flags_348, 8, false);
						}
						else
						{
							biped->unknown34b = 0;
							biped->flags_348 |= 0x100;
						}
					}
				}
				else
				{
					*((byte *)&biped->flags_348 + 1) |= 2;
				}
			}
		}
		else
		{
			*((byte *)&biped->flags_348 + 1) |= 1;
		}
		break;
	}
	case 1:
		if (!g_5107ec && (*((byte *)biped + 0xc1) & 1))
		{
			biped_orientation_flag_set(arg_159e6d, true);
			biped->unknown34b = 2;
			function_b77d0(arg_159e6d, g_4687a4, g_4687a4);
			function_1c3770(arg_159e6d, 0);
			(*(long *)((byte *)g_4e6948 + 0x1130))--;
		}
		else
		{
			if ((g_510c54->game_time + (arg_159e6d & 0xffff)) % vehicle_round_ticks(g_510c54->field_2_3 * 0.3f) == 0)
			{
				function_e1a80(arg_159e6d);
			}
			function_e0e00(arg_159e6d);
		}
		result = true;
		break;
	default:
		if (!(*((byte *)biped + 0xc1) & 1))
		{
			function_e18d0(arg_159e6d, true);
		}
		break;
	}
	return result;
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

	cross3f(axis, v, &cross);
	v->i = v->i * cosine + cross.i * sine;
	v->j = v->j * cosine + cross.j * sine;
	v->k = v->k * cosine + cross.k * sine;
}

/* turns the biped toward its desired facing: flying ones by their flight
   controls, walking ones at the definition's turn rate (or by a turning
   animation when they stand), and a grabbing one toward its target */
// MiniMax B (11-13): no new form, no lower score (548); the frame agrees, but the flight-call homes, the state bytes and the vector scheduling still differ.
// score 548 (was 548): round 101 (m3 lane 12) and r73 notes are at the end of this note. Frame 0x78 agrees; flight-call homes, state bytes and vector scheduling remain.
// round 101 (m3 lane 12) and r73 notes are at the end of this note. Frame 0x78 agrees; flight-call homes, state bytes and vector scheduling remain.
// MiniMax B (11-14): no new form, no lower score (548); the prologue already differs - retail computes the pool offset in EBP
// (pushing EBP before the shift) and keeps it there, while ours computes it in EBX and pushes EBP later, so the first field load
// and every call argument after it are one register out.
// score 548 (was 561): round 101 (m3 lane 12) and r73 notes are at the end of this note. Frame 0x78 agrees; flight-call homes, state bytes and vector scheduling remain.
// MiniMax S (11-09): no new form, no lower score (605 in the reduced build); retail keeps the biped
// index in EBP and the definition pointer in ECX with a home at +0x18, ours keeps the definition in
// EBP and the index in EBX with a home at +0x1c, and retail's changed-result byte sits at +0x11
// against our +0x12, so retail's frame is 0x78 and ours is 0x7c. Retail's remaining byte homes need
// the special flag to live in memory, which is the form review rejects; a named index at the top
// measures 603 and the same name used at the tail 635, the special flag as a bool 649.
// MiniMax R (11-06): no new form, no lower score (605 in the reduced build); the frame is 0x7c here and 0x78 in retail, and the definition pointer is held in EBP here and in memory in retail.
// MiniMax S (11-10): no new form, no lower score (561); the frame 0x78 agrees and the flight-call homes, the state bytes and the
// vector scheduling are unchanged.
// Earlier: 604 (was 654): the logic now follows retail (see below); frame is 0x78 like retail. Still differs: the
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
// M288: Direct flight-rate argument 593 (seven-point gain, rejected); staged shared
// projections 633, both 616; complete facing/turn setters 624, with rate 624, with staged
// projections 616, all three 616, with role names 616. Original complete body restored.
// P258: Default flight threshold D, existing bit tests B, per-use state S:
// D/B/DB/S/DS/BS/DBS/direct-rate+BS fast 649/656/643/646/632/654/639/654
// (control 642); expanded 651/650/604/648/618/639/634/639 (control 649).
// DB plain 615; BS plain 639. Original threshold, tests and state receiver restored.
// F323: Frame 0x78 agrees; first +0x4e is the flight exit span, with scalar homes and vector scheduling still different.
// Full-width special/wall/left truth subsets 1/2/4/3/5/6/7 and all native bytes
// fast 600/639/652/631/615/633/622/642 (control 642); special alone plain 561,
// 2728/2712 bytes. Keep only long special; every producer, condition and call is original.
// F329: Shared wall side, shared angle sine/cosine and complete triple-product crosses,
// seven subsets: 611/604/669/598/643/669/669; all with direct cross producer 643
// (unchanged reduced control 600; full 561). The 598 form gains only two. All restored;
// frame 0x78, flight scalar/byte homes and wall-vector scheduling remain.
// M313: Native orientation/shared short-hand getter/entry scalar declaration subsets, then branch scalar declaration; original body restored;
// fast 600/616/616/600/600/616/616/616.
// N321: Common final result, bool/long/byte width and result-first lifetimes on the retained
// long-special graph tie or regress against reduced control 605, except byte common result 603
// (two-point lead, restored). Full baseline 561 retained; flight/result/vector homes remain. Scores
// 605/625/610/605/618/603/605/618.
// Original executable source restored.
// P274: Complete walking up/forward/desired-facing input values, subsets 1/2/4/3/5/6/7,
// fast 645/663/651/699/666/675/720 (control 605); all with entry declarations 720.
// Original body restored; flight/result-byte homes and vector scheduling remain.
// AC488: shared mode-result, separate walking-state admission and flight-zero branch
// subsets 1/2/4/3/5/6/7: 600/600/608/600/608/608/608; all with walking-only state receiver 611.
// Reduced control 600, full 561; original complete body restored. Flight scalar/result-byte
// homes and vector scheduling remain; existing matched flight contract preserved.
// P276: Actual typed animation-state/turn-tag/read-only receiver subsets 1/2/4/3/5/6/7,
// then full native unsigned state keys: fast 600 each (control 600; full 561). All actual
// data declarations and complete body restored; flight/result-byte homes and vector schedule remain.
// P278: Actual flight-clamp/rate, walking-limit/angle/cosine/sine/overshoot scalar lexical homes
// independently, then all: reduced 605 each, control 605/full 561. Original body restored;
// scalar/result homes and vector scheduling remain.
// Round 543: Existing complete vector setter at axial/clamp/pitch producers, seven
// subsets then all projections: fast 605/601/639/611/673/676/642/677 (control 600; full 561).
// All regress and grow; original producers, helpers and complete body restored.
// Round 379: Ordinary-state/planar/right-turn complementary truth subsets 1/2/4/3/5/6/7, then
// complementary unchanged result: 605/633/610/633/610/633/633/626 (fast control 605; full 561).
// Original full-width special and complete truth graph restored; result/flight scalar homes and
// vector scheduling remain.
// P292: Multi-use actual up/forward/goal field pointers, subsets 1/2/4/3/5/6/7,
// then all references: reduced 607/683/690/630/598/687/638/638 (control 600).
// NOT kept: 598 gains only two; first +7 is entry scheduling; original entire
// body restored. Full 561 retained; flight-result/scalar and vector homes remain.
// AC500: Admitted-mode tag ownership with producer-local/entry-only declaration, each with
// original/direct/if-else/default flight threshold: 607/617/607/680/607/617/607/680 (fast control 600, full 561).
// Original tag lifetime, threshold producer and complete body restored; flight/result homes remain.
// DH314: False-default special state before key exclusion, true from its native test,
// bool/byte/char/signed-char/short/word/long/dword carriers:
// 656/656/656/656/645/645/651/651 (reduced control 600; full 561).
// Exact-match-only flag forms regress; original producer, guard and complete body restored.
// N344: Existing result/special/wall/left lexical homes: result/handled, special_turn/turn_override,
// wall_turn/use_up_axis, turn_left/negative_turn: eight fast 600 (control 600; full 561).
// NOT kept: no exact home/copy repair. Original truth symbols, widths, helpers and complete body restored.
// P292: Multi-use actual up/forward/goal field pointers, subsets 1/2/4/3/5/6/7,
// then all references: reduced 607/683/690/630/598/687/638/638 (control 600).
// NOT kept: 598 gains only two; first +7 is entry scheduling; original entire
// body restored. Full 561 retained; flight-result/scalar and vector homes remain.
// F356: Existing complete staged projection outputs at first/second/pair/third/fourth/pair/all sites, then all direct shared outputs on the retained long-special graph:
// 599/613/597/612/612/604/636/605. NOT kept: no qualifying match or gain; original complete body restored.
// F356 ordinary confirmation of the initial pair: 604, 2720/2712 bytes versus baseline 561;
// NOT kept: full-image count regresses despite eight-byte shortening. Original body restored.
// M342: Both native double trig results at walking/yaw/pitch phase subsets 1/2/4/3/5/6/7,
// then all const: fast 623/605/601/602/623/623/602/602 (control 600, full 561). NOT kept:
// no exact native-result lifetime graph. Original complete body restored; flight/result/vector homes remain.
// M349: Actual special/wall/left entry-only declarations, seven subsets, then
// constant-false result first: eight fast 600s against control 600 (full 561).
// NOT kept: no byte match for declaration allocation. Original body restored;
// result/flight scalar homes and vector scheduling remain.
// DH324: Actual initial mode/current-state/prior-turn discriminator values, seven
// subsets then immutable values: fast 599/599/611/599/611/601/609/609 (control 600, full 561).
// NOT kept: 599 gains only one against reduced 600. Original field observations and
// complete body restored; result/flight/vector homes remain.
// P320: Parallel axial projection/cosine/sine-cross parts, P/C/S/PC/PS/CS/all/const-all:
// P320: fast 610/610/612/614/614/614/587/587 (control 605, full 561). NOT kept:
// P320: 587 lengthens 2733 to 2748 against retail 2712, and only the complete
// P320: combination gains while its parts regress. Original complete body restored;
// P320: result/flight scalar homes and vector scheduling remain.
// M362: Existing yaw cross ordinary/staged 600/600; complete indexed yaw
// publication with original/ordinary/staged cross 617/617/617; existing
// scale/add-scaled with ordinary/staged cross 600/600; weighted staged cross
// and existing sum 605 (reduced control 600, full 561). NOT kept: no gain;
// original yaw helper, flight/result homes and complete body restored.
// M364: Original early-return changed result in char/signed-
// char/byte/short/word/int/long/dword storage: 610/610/610/623/623/625/625/625. NOT kept: no
// native result-byte/flight-home match; original result domain and complete body restored.
// AC546: In-place first-wall/clamp/yaw output phases, seven subsets then coordinate
// publication: 624/622/585/672/737/643/695/623 (control 605). NOT kept: 585
// lengthens away and combines regressing parts; yaw adds an out-of-line call.
// Isolated forced native/setter yaw 635/605; 18 calls recovered, but frame 0x8c
// or a tie remains. Original complete body, helper and long special restored.
// Y590: Actual wall-projection/axial-scalar/matrix-conversion last-use scopes,
// seven subsets then all with planar temporary termination: 600 each (fast control
// 600, full 561). NOT kept: no byte change; complete original body restored.
// Changed-result/flight homes and vector scheduling remain.
// F388: Whole facing/turned/axis component-array views, subsets 1/2/4/3/5/6/7:
// fast 604/604/611/614/605/605/613; all with side/aim/look views 619
// (control 600, full 561). NOT kept: regressions and growth; original body
// restored, result/flight-scalar homes and vector scheduling remain.
// M383: Actual trig-input double carriers at walking/yaw/pitch phase subsets 1/2/4/3/5/6/7,
// then all const: 600/623/602/623/602/611/611/611 (reduced control 600; full 561). NOT kept:
// no native flight/result-home repair; original input/result domains and complete body restored.
// M388: Final four copies via memcpy/memmove/vector setter, native byte writer, then
// transfers plus writer and point setter plus writer: 600/593/659/593/593/593/659/659
// (fast control 600). NOT kept: seven-point leads below gate; memmove adds four calls.
// Original publications/body restored; result/flight homes and vector schedule remain.
// Direct flight-rate argument on the current full-width-special graph, alone/with byte writer:
// 610/603 (control 600); both regress. Original scalar producer and whole body restored.
// AC546: In-place first-wall/clamp/yaw output phases, seven subsets then coordinate
// publication: 624/622/585/672/737/643/695/623 (control 605). NOT kept: 585
// lengthens away and combines regressing parts; yaw adds an out-of-line call.
// Isolated forced native/setter yaw 635/605; 18 calls recovered, but frame 0x8c
// or a tie remains. Original complete body, helper and long special restored.
// Lane 15 round 58 (plain; aligned = LCS mnemonic differences, 216 for this body): the node test as
// `long node_count = *(short *)(+0x10c) / sizeof(real_quaternion_transform); if (node_count > 0)` 591 (retail's shr/jle),
// with the two transform pointers named before it 593 (that block then retail's exactly, aligned 190); the current
// weapon after the 0xe5d50 call through unit_get_current_weapon 610 (retail's movsx dx sequence, aligned 188); both
// 612 (aligned 176); with the flight rate passed directly 622 (aligned 167). All lengthen to 2738-2744 bytes and the
// positional count rises; they wait on the early misalignment (result byte, flight slot, wall-turn products).
// P383: Complete overshoot/within-turn admission in bool/byte/short/long (reduced control 605; full 561):
// 603/603/603/603/603/603/603/603. NOT kept: two-point gains below gate and no native result/flight-home
// equality; original direct geometric test, helper graph and whole body restored.
// P393: Complete normalization fallback/side-admission outcomes, positive then complementary
// bool/byte/short/long: 588/588/588/588/588/588/588/588 (fast control 605; full 561). NOT kept: one-use
// outcomes remain unmatched and grow 2733 to 2743 bytes against retail 2712. Original
// normalizer tests and whole body restored; result/flight homes and vector scheduling remain.
// Round 101 (m3 lane 12, plain checks; --fast builds this function differently, 605 with 595 aligned differences against the
// plain build's 381): the turn angle's product written definition-first 561 (same code); the facing limit as `limit = 0.99f;
// if (!(flags & 8)) limit = definition + 0.0001f;` (retail loads 0.99 first and jumps over the other arm) 614 (2735 bytes,
// aligned 369); the wall block's side and facing through the existing cross3f 598 (2714 bytes, aligned 398), through
// cross3f_staged 604 (aligned 417); the sine as cross3f(facing, forward) with dot3f(up, cross) 639, with dot3f(cross, up) 646.
// Round 164 (m3 lane 6, plain; --fast still builds this at 600-605 with every callee and 0x11dbb0's callers listed; masked =
// aligned count with registers and stack offsets masked, 250 for this body): the current weapon read after the first 0xe5d50
// call as retail does (a sign-extended short) 578 (masked 217); the animation value in a named two-use local with the 0x6000084
// test in a nested block 573 (masked 231); both 576 (masked 215); the wall turn through the existing rotate_direction (the same
// formula) 634 (2757 bytes, masked 214), with both 631; the pitch rows with double sine and cosine (retail computes them on the
// x87 stack with unrounded fsin/fcos and loads the pitch twice) 587 (masked 256). Body unchanged.
// M3 lane 16 r104 (plain, 0xdd360/0xe0ef0/0xdc5c0 unchanged): the mode-6 first weapon through the existing
// unit_get_current_weapon at its current place 594 (2743 B); the wall cosine through dot3f 563 (2742 B, masked aligned 154 ->
// 140); the wall turn's axial term through dot3f 610; both 573; the pitch rows through vector3f_scale then vector3f_add_scaled 571.
// M3 lane 2 r73 (plain, all 66 functions of this file unchanged; masked = aligned count with registers and esp offsets masked,
// 250 at 561): the mode-6 `|= 0x20` through flags_348_bits.field_5 568 (2721 B, masked 249; the `or byte ptr` is retail's but
// scheduled before the four copies), through word_flags_set(&flags_348, 5, true) 568 (2721 B, masked 247: retail's fused `or
// byte ptr` at retail's place); the mode tests nested, `if (mode != 3) { changed = true; if (mode != 6) ... }` 561 (same code);
// the overshoot clamp's wall arm through cross3f 611 (masked 245). LEAD 554 (2735 B, aligned 376, masked 228; seven points,
// under the gate): word_flags_set with the wall cosine through dot3f. On it: dot3f's operands swapped, the pitch rows as
// `v * cosine + axis * sine`, the pitch rows' addends swapped, biped_rotate's addends swapped 554 each (same code: the sum
// and product orders do not follow the source); the wall turn's cross part through cross3f 589 (masked 258). On that 554
// base the flight rate passed directly 554 (same code). KEPT, 561 -> 549 (2738 B, 650 lines as retail, aligned 442,
// masked 237): word_flags_set and both three-term dot products (the wall cosine and the wall turn's axial term) through
// dot3f; biped_rotate's cross through cross3f at the same bytes. On 549: the current weapon through unit_get_current_weapon
// after the first 0xe5d50 call 618 (masked 220); the first wall block's side and facing through cross3f 552 (2726 B,
// aligned 416, masked 216); the clamp's wall arm through cross3f 600 (2716 B, masked 213); both 624 (masked 204); every
// remaining cross product through cross3f (the triple products as cross3f + dot3f) 690 (2776 B, masked 197); the whole
// first wall block through helpers 636; every planar product through dot_product2d/cross_product2d 621; the three
// TEST_FIELD_BIT tests through the file's field_bit_test 647.
// P436: Baseline remeasured 548 (opening heading 549); frame 0x78 agrees, result byte
// +0x12 versus +0x11 and flight scalar +0x14 versus +0x18 remain. Fixed existing
// biped/tag/state receivers, special/wall results, native angle/cosine and phase limits:
// eight fast 614s (control 614). NOT kept: no byte change or eligible gain; original
// complete body restored. Native-width immutable inputs differ from earlier double-carrier forms.
// P439: Lower-first yaw then pitch producers: raw-result updates, lower-default selection,
// exhaustive arms and standard upper-bound selection: 604/603/604/627/610/610/612/604.
// NOT kept: no supported native result-byte/flight-home repair; every original clamp,
// unordered path, helper and complete body restored. Ordinary raw/default/exhaustive yaw
// and upper-selection pitch 607/601/611/629 regress against full 548.
// MiniMax A (11-13): no new form tried this round, no lower score (548): the flag byte is still at [esp+0x12]
// here and at [esp+0x11] in retail, the two cold exits still land at a different place in the body than
// retail's, and the body is 2738 bytes against retail's 2712, so the block layout after the flag test differs.
// M3 lane 4 r196 (plain): `bool special` (direct compare) 610 (2734 B); `bool wall` declared ahead of `long special` 575
// (2746 B); the overshoot test as `left ? overshoot < 0.0f : overshoot > 0.0f` 548 (same code); the 0.98 test's products
// turned-first 548 (same code); the flight rate as `rate = 0.99f; if (!(control_flags & 8)) rate = threshold;` (retail loads
// 0.99 first) 613 (2747 B).
// P471: Standard range transfers for paired facing fallbacks, planar goal, turn input,
// up reset, forward publication, final publications and planar clamp; existing consumer
// orientation record: 648/577/611/662/562/581/626/612. NOT kept: apparent gains
// add calls absent from native transfers or enlarge the frame; original body/includes restored.
// P473: Standard first-product-seeded wall-cosine/axial/planar reductions, masks 1/2/4/3/5/6/7,
// then explicit operations: 696/679/678/687/701/689/693/778. NOT kept: no native result/flight-home
// equality; original entire body, dots, planar tree and includes restored.
// P476: Standard typed changed/special/left predicate-result contracts, masks 1/2/4/3/5/6/7, then wall-bit contract. Reduced control 612 versus full 548. Scores 660/673/702/662/727/722/738/782.
// NOT kept: unmatched diagnostic source forms; original executable body restored.
// @retail 0xe01d0
bool __stdcall function_e01d0(long arg_159e6d, long *names)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	s_biped_definition *definition = BIPED_DEFINITION(biped);
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
				real rate = (biped->control_flags & 8) ? 0.99f : definition->biped.cosine_stationary_turning_threshold;

				function_1faf80(arg_159e6d, &definition->biped.physics.flying_physics, &biped->control, rate, &biped->facing_goal,
					&biped->unknown3a0);
			}
			else
			{
				biped->unknown3a0 = 0.0f;
			}
		}
		else if (biped->physics_mode != 5 && *(long *)(state + 0x70) != 0x6000084)
		{
			long special = *(long *)(state + 0x70) == 0x7000039;
			bool wall = TEST_FIELD_BIT((definition->biped.physics.flags >> 3) & 1);
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
				cosine = dot3f(&biped->forward, &facing);
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
			if (biped->unknown34a == 1 || (definition->biped.flags & 1))
			{
				real angle = g_510c54->rate * definition->biped.moving_turning_speed;
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
					real along = dot3f(&turned, &biped->up) * (1.0f - turn_cosine);
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
				real limit = (biped->control_flags & 8) ? 0.99f : definition->biped.cosine_stationary_turning_threshold + 0.0001f;

				if (limit >= cosine && !(TEST_FIELD_BIT((definition->unit.flags >> 20) & 1)))
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
				word_flags_set(&biped->flags_348, 5, true);
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
	real x = point->x;
	real y = point->y;
	real z = point->z;

	if (matrix->scale != 1.0f)
	{
		x = matrix->scale * x;
		y = matrix->scale * y;
		z = matrix->scale * z;
	}
	result->x = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x + matrix->position.x;
	result->y = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x + matrix->position.y;
	result->z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z;
}

/* places one of the ragdoll's bodies: hanging from its constraint's pivot on
   the body placed before it (kept within reach of the ragdoll's center and
   out of the world, turned to follow), or the first body at the point */
// MiniMax G (11-13): no new form, no lower score (356): the diff re-read this round puts the first difference at the pivot transform's output home (+0x60 here, +0x88 in retail) and then at the matrix rows, which retail reads through a register where this build copies them through the frame, with our two results at +0x20 and +0x24 against retail's +0x14 and +0x18, so the body stands.
// MiniMax B (11-13): no new form, no lower score (356): the frame agrees (0x12c) and the first difference is at +0x71, where the entry block's register choice differs; the body is one byte longer than retail's.
// MiniMax H (11-12): no new form, no lower score (356): the two pivot roles swapped measure 356, the same code, so the body stands.
// MiniMax G (11-12): no new form, no lower score (356): nothing new this round; the origin, the first property/distance, the
// matrix, the second property/limit and the placed homes are still in another order.
// MiniMax G (11-12): no new form, no lower score (356): the origin, the first property/distance, the matrix, the
// second property/limit and the placed homes are still in another order.
// score 336 (was 356): origin, first property/distance, matrix, second property/limit and placed homes in another order
// (round 196 of m3 lane 4: the world-pivot group and target also swap, see its note at the end)
// (retail 0x40/0x4c/0x50/0x84/0x88, ours 0x44/0x40/0x60/0x5c/0x50), the pivot select and the materialized travel test.
// Round 85 (m3 lane 13) at the end; the earlier first lines are kept below as "Earlier first line".
// MiniMax D (11-11): no new form, no lower score (371); the frame still agrees at 0x12c and the length still
// matches retail's 1937, but the stack homes are still ours: the origin sits at esp+0x44 against retail's 0x40,
// the matrix at 0x60 against 0x50 and the 16-byte placed object at 0x88 against 0x84, and every home from 0x94
// up agrees. The homes only move when the declaration order of the locals moves, which the earlier rounds
// measured as a placement trick and reverted; the pivot ternaries and the materialized travel test that would
// take the other homes are the same match-only forms.
// MiniMax E (11-11): no new form, no lower score (371); the frame's locals sit at [esp+0xc8] and [esp+0xd4] where
// retail uses [esp+0xd4] and [esp+0xc8], and the two tails cross-jump at the z store and the call.
// Earlier first line: at 356 (from 371): round 112 (m3 lane 12) note at the end. Earlier at 354 (was 354): origin, distance, matrix, limit and
// placed homes in another order; the pivot select and the materialized travel test differ.
// MiniMax B (11-07): no new form, no lower score (354); our origin, distance and matrix homes are 0x44, 0x34 and
// 0x60 against retail's 0x40, 0x4c and 0x50, while every home from 0x94 up agrees.
// MiniMax C (11-07): round 53: no new form, no lower score; the transform's matrix, origin and distance homes stay where the earlier rounds left them.
// Earlier at 371 (was 354): MiniMax D (11-05): no new form, no lower score; our origin, distance and matrix homes are 0x44, 0x34
// Earlier 354 (was 354): MiniMax D (11-05): no new form, no lower score; our origin, distance and matrix homes are 0x44, 0x34
// and 0x60 against retail's 0x40, 0x4c and 0x50, while every home from 0x94 up agrees.
// Earlier at 368: NOT kept (lane 1 r62): the pivot pair as two ternaries on the same property test gave 351
// frame 0x12c agrees, 1940 bytes against 1937, first +0x71. Earlier NOT kept (lane 1 r62): the pivot pair as two ternaries on the same property test gave 351
// (frame 0x12c and 1937 bytes agree, first +0x71 the body-matrix home, +0x60 versus +0x50), but retail compares once:
// a repeated test picked by score, match only; the if/else restored. Earlier: 368 (was 494; 488 before the logic walk of 2026-10-31, which adds 6 points but fixes behaviour): retail's final
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
// Round 192, kept together (494 -> 368; frame 0x12c and the entry now retail's, first difference +0x71): the
// collision's unknown24 = NONE stored at the top of the parent block (retail 0xe1d0b; ours never stored it) 492, the
// origin through real_point3d_set as sibling 0xe1a80 (retail's load-all movss copy) 479, single exit with `result`
// false at entry (retail's `xor bl, bl` / `mov al, bl`; the lane wrote it with one goto, the coordinator rewrote it as
// the plain `if (started || ...)` block, same bytes at the end: 368 both) 405, the limit computed after the distance 396,
// the rotation built from a copy of delta (`direction`; retail copies delta's home to 0x34 by dwords and normalizes
// the copy, back takes delta's home) 368. Tried on that: biped_matrix_transform as 0x142570's body (x/y/z locals,
// `scale * x`, retail's register shape) 421; pivot ternaries 398 (with that body 431); collision declared first
// 396; world_pivot/child_world swapped 368; `real limit;` at the block top 368; the collision target as written-out
// if/else arms 433, as two vector3f_add_scaled calls 434, with a named `reached` test 450, `limit > distance ||
// !hit` 447. Remaining: retail pairs world_pivot's home with delta/back and child_world's with the copy (ours the
// reverse), property_b's with limit; retail keeps 1.0f in xmm4 across both pivot transforms; retail materializes
// `limit > distance` in AL before the target copy; the pivot select (`mov ecx, edi; je` to an out-of-line lea).
// M288: On the retained exact-frame graph, pivot-result/body-transform/property-placement
// role names separately and all together 368/368/368/368; complete scaled-point setter
// 421, with all role names 421; setter plus default pivot selection 424, with all names 424.
// Original helper, selection and executable body restored; origin/matrix and paired point homes remain.
// Round 514: native input memcpy/memmove with direct output 368/383; aggregate
// input with output memcpy/memmove 895/875; paired input/output copies 895/895/875/875.
// Original helper and body restored; matrix/pivot homes and scalar transport remain.
// Round 324: last-use scopes for parent matrix/world pivot/child world, subsets 1-7,
// then all with matrix declared before origin: 368 each. Original scopes restored.
// Round 359: Independent pivot transform order, delayed direction copy and existing back scale,
// subsets 1/2/4/3/5/6/7, then all with entry limit: 381/388/369/407/382/388/408/431.
// Original producer order, scalar lifetimes and complete body restored; matrix/origin homes remain.
// Round 326: Cached scale/shared scale producer/branch-owned scaled point in
// the existing pivot helper, subsets 1-7: 369/369/369/366/367/367/367; all with
// the native point declaration first 367. Two-point lead rejected; original
// alias-safe helper and body restored. Matrix/origin and paired pivot homes remain.
// Round 365: Shared world-offset/placement-difference-plus-origin/final-displacement
// producers, subsets 1-7: 364/387/388/368/364/369/363; all with shared contact scaled
// sum 364. Exact-length 364/363 gains are below threshold. Original complete body
// restored; matrix/origin and paired pivot/property homes remain.
// Round 371: Native travel-limit decision before original target copy,
// byte/short/word/long 372/376/376/375; complementary truth representation
// 372/376/376/375. Original predicate and body restored; matrix/pivot homes remain.
// Round 373: Complete up/left/forward product lifetimes in every transform row,
// seven subsets then complete row projections: 370/370/370/370/369/370/895/895. NOT kept: product
// temporaries do not reproduce retail; original helper/body restored.
// M317: Sequential parent/rotation, origin/placed and pivot/target workspace reuse subsets 1/2/4/3/5/6/7, all
// with consumed-direction reuse; root relocation offset with aggregate copy/point setter:
// 369/401/395/401/395/428/428/428/396/379.
// Matrix/origin and paired point/property homes remain. Original executable body restored.
// Round 375: Collision fraction from candidate update/owned candidate/full branches/named distance,
// storage at producer/parent-block entry: 368/368/368/369/368/368/368/369. Original conditional and whole
// body restored; frame 0x12c, matrix/pivot/property homes and first +0x71 remain.
// M325: Nested normalization admissions, real/bool/long results, shared short-circuit/progressive/false-default
// outcome and early real-result declarations 368/369/369/369/369/369/369/369. All restored;
// matrix/origin and paired pivot/property homes remain; result carriers do not recover them.
// Lane 1 round 62, kept: the pivots as two ternaries 351 at 1937 bytes (retail compares once, its child pivot's lea
// out of line). Declaring the properties at the top in three orders 368 (frame unmoved). On the ternaries, each
// positional score against aligned mnemonic differences (base 135): x/y/z locals in the transform 414/106, with the
// target's two arms written out 382/93, plus a bool of limit > distance ahead of the target copy 396/60, plus retail's
// else arm (the offset from the position, origin overwritten by *point, z lowered; also through
// vector3d_from_points3d) 378/33. With the if/else select instead of the ternaries 329/42, with its defaults assigned
// first 326/42, with only the child default 372/42. Lead 326: retail's shape, but the bool is one-use (not kept
// without a match). Without it: arms 363/69, no arms 391/91, default select with arms 364, without 394.
// P293: Admission-produced placement result in bool/byte/short/long, entry-default then query-boundary
// lifetime: 973/961/954/956/973/961/954/956. Original complete body and helpers restored; no qualifying
// gain.
// M338: Native scale/transform-row/complete point-geometry ordinal loops, subsets 1/2/4/3/5/6/7 and final
// pivot translation: 369/369/364/369/364/364/364/364. Original helper and body restored;
// matrix/origin/pivot homes remain. NOT kept: four-point lead above 30 fails the eight-point gate.
// DH322: Final pivot in consumed offset, component/difference/setter/loop: 368/368/368/368;
// in final position: 363/363/363/363, 1937 bytes, matrix home 0x50 and first +0x7d; origin four bytes high.
// NOT kept: five-point nonmatch gain above 30 fails the eight-point threshold; original
// output workspace, helper and complete body restored, matrix/origin/pivot/property homes remain.
// AC504: Both pivots through existing alias-safe/scalar-input/axis rotations, with
// component/shared/in-place translation: 895 each (1412 bytes, helper outlined).
// Native inline expansion on four producer forms: 389/389/389/391; all regress.
// Original helper/body restored; matrix/origin, pivot/property homes remain.
// DL254: Two-use pivot output-pointer contract, direct/scalar/setter/aggregate producers, ordinary then forced inline:
// 895/421/895/895/368/421/369/367. Original complete body and helper contracts restored;
// NOT kept: unsupported storage/return/domain nonmatches; native homes remain.
// P302: Existing pivot-helper actual matrix/input/output references, seven subsets then forced-inline
// complete reference graph: 368/368/368/368/368/368/368/368. All tie; original pointer contracts/helper/body restored;
// native matrix/origin and paired pivot/property homes remain.
// M369: Completion-owned common matrix setter, bool/byte/short/long, original/entry default:
// 406/403/400/401/406/403/400/401. NOT kept: all regress and shorten away from retail;
// original result, admission and publication restored; matrix/origin and paired pivot homes remain.
// DL259: Existing pivot helper point/matrix/returned-point value ownership,
// seven subsets then immutable point input: 853/866/858/823/817/820/781/781.
// NOT kept: native aggregate contracts miss byte equality. Original helper and
// complete body restored; matrix/origin and paired pivot/property homes remain.
// M372: Origin/matrix/false-result declarations at function entry, seven
// subsets then all with the original collision declared at entry: 368 each.
// Original declarations and complete body restored; native matrix/origin and
// paired pivot/property homes remain; frame 0x12c still agrees.
// Round 588: Position/property/collision last-use scopes, seven subsets, then all with
// earlier inner rotation declaration: 368 each. NOT kept: no native-home repair; original
// complete body restored. Matrix/origin and paired pivot/property homes remain.
// F386: Static existing component-array observations of scaled pivot, transform rows
// and full geometry, seven subsets then all forced: 369/368/369/369/369/369/369/369.
// NOT kept: tie/regressions; original helper/body restored, matrix/origin/pivot homes remain.
// B442: Root-first parent/root order, bool/byte/short/long completion results,
// false-default 383/417/414/415; true-default 443/444/437/437. NOT kept: all regress;
// true-default frame grows to 0x130. Original order, result and full body restored.
// P342: Complete offset/delta squared products, alone/both, real-root overload and complete immutable
// products: 369/369/369/368/369/369/369/369. NOT kept: no byte match or eligible native allocation gain;
// original reductions, root expression and whole body restored. Matrix/origin and paired pivot/property
// homes remain.
// Round 214 (m3 lane 3), kept: the transform as 0x142570's x/y/z body, the target's two arms written out as retail has them, the pivot select
// with its defaults first: 368 -> 354 at 1940 bytes (masked aligned 106 -> 93). Steps: x/y/z 421 [100]; arms 361 [94]; plain select with both 361;
// arms alone 433; arms and select 428. Match-only, not kept (each a one-use flag or a repeated test): `reached` ahead of the target copy 329 [50],
// default select 326 [49], retail's else arm through origin 364 [70] alone and in those; two pivot ternaries 309 [40]: then the select is byte
// exact. Arms through vector3f_add_scaled, one or both, 326 [48/49]; collision.t in place 378; one row set after adjusting t 368 (1918 bytes);
// length_sq3f in both squared sums 309 (same code); limit named ahead of the pivots 406; `real limit;` with the other locals 309.
// Diagnostics: limit's second use spelled as the difference 309 [29], the rotated-pivot products become retail's memory-second shape; three fewer
// `placed` reads or nine more matrix references put the matrix at retail's +0x50 but leave limit and property_b above placed, where retail has
// them below it. Left on the 309 form: that slot order, the second transform's point loads, t's register in the arms and the else arm.
// On the kept 354: length_sq3f in both squared sums 354, `real limit;` with the other locals 354, the first arm through vector3f_add_scaled 354, the arms inverted 354 [99],
// the two pivot pointers declared the other way 354: five without a gain.
// Round 70 (m3 lane 14, fast with 0xe1b60/0xe23e0): renames to move same-size stack slots, the matrix as body_matrix
// 354, the origin as body_origin 354, the two properties as first/second_property 354, the distance as pivot_distance
// 354, all four 354 (identical code: names do not order this frame). Only 17 of the 351 differing lines are offset-only;
// the masked diff starts at the pivot select and retail's materialized `limit > distance` (AL) before the target copy.
// Lane 1 round 79 (fast): offset through vector3d_from_points3d 370 (1944 bytes); the no-parent arm as a shift vector
// from origin to the matrix position added to the lowered point 392 (1962 bytes); back through vector3f_scale 356; delta
// through vector3d_from_points3d 354; offset and delta both 370. With a collision-result constructor (unknown24 = NONE;
// it matches 0xdef60 but loses 0x54590, not kept) 354.
// P363: Current scalar-input pivot rows through two-term/first-term/projection/complete accumulators,
// ordinary then forced inline: 354/353/354/353/354/353/354/353. NOT kept: one-point leads
// miss the margin and native matrix/origin homes; original helper and complete body restored.
// P374: Complete parallel impact products, placement shifts and root-origin shifts,
// seven subsets then immutable full expression parts: 354/354/354/354/354/354/354/354. NOT kept: no gain;
// original complete body restored; matrix/origin/distance homes remain displaced.
// P376: Selected parent/child/both point values; both setter/memcpy/memmove/const; final child value:
// 371/383/403/386/403/407/403/366. NOT kept: regressions, native matrix/origin/pivot homes differ.
// Original selected addresses, scalar-input helper and complete body restored.
// P378: One parent/root clearance owner at entry/component/position/matrix boundaries,
// mutable/readonly: 463/463/463/463/463/463/433/433. NOT kept: all regress and shorten away
// from native; early owners also lose both exact callers. Original full body restored.
// P381: Terminal rotated output in consumed back/direction/delta/placed/world-pivot/
// child-world/origin/parent-position homes: 354/354/354/354/356/354/389/357.
// NOT kept: no native matrix/origin/alias-copy home repair; original independent
// output, every prior vector value and complete body restored.
// P385: Contact-selection outcome bool/byte/short/long, positive then complementary:
// 354/354/354/354/354/354/354/354. NOT kept: no native matrix/origin or paired property/pivot home repair;
// original single test, selected addresses and complete body restored.
// T73: Consumed selected parent-pivot and child-world/placement outputs, alone/both,
// complete placement setters and forced existing pivot expansion: 391/356/398/391/358/398/391/398.
// NOT kept: every form regresses; parent reuse grows frame to 0x130 versus native 0x12c.
// Original independent outputs, input captures, helper annotation and complete body restored.
// P395: Existing target/direction/both transfers through memcpy 354/354/354;
// origin transfer alone/with target/with direction/with both 381/381/381/381;
// all three transfers through memmove 504. NOT kept: no native home repair;
// original value producers, complete body and existing helper preserved.
// M3 lane 4 r176 (fast with 0xe1b60/0xe23e0, both stay matched; base 354 at 1940). Retail's frame: origin +0x40, the
// first property +0x4c alone (the travel distance later shares that slot), matrix +0x50, limit +0x84, placed +0x88 with
// the second property's address four bytes into it; ours has origin +0x44, the properties at +0x40/+0x60 and the matrix
// +0x60. The two properties declared in their own block around the pivot select 360 (1945 bytes, slots unmoved).
// Re-measured as diagnostic bases: two pivot ternaries 343 (1949), with the `reached` bool 331 (1959), the ternaries with
// retail's no-parent arm (shift from the position, origin overwritten by *point, z lowered) 331 (1944: origin +0x40 and
// the first property +0x4c as retail, matrix still +0x60); on that base placed through real_point3d_set 331, delta
// through vector3d_from_points3d 331, both 331 (same code). On the kept body, the no-parent arm as a shift local with
// origin overwritten 364 (1935), as a lowered copy of the point with the shift through vector3d_from_points3d 373
// (1929), as a lowered copy with the differences in place 369 (1933). All restored.
// M3 lane 5 r167 (fast with 0xe1b60/0xe23e0, both stay matched; base now 371 at 1931, aligned 229, masked 156): the two
// properties declared at function scope 371, the matrix declared before the origin 371, the collision at function scope
// 371 (M372's entry collision re-measured), the distance declared at its computation 371, back at the top of the parent
// block 371, the target declared at its copy 371, rotated_pivot and the rotation matrix at the top of the parent block
// 371/371 (the rotation one may repeat round 588's earlier rotation declaration), the collision call joined to the limit test with `&&` 371, the move test
// split into two nested ifs 371 (all the same code: scopes and declaration order move no slot). The limit computed
// before the distance (undoing round 192's order) 357 at 1935 bytes, but aligned 230 and masked 157: a length effect,
// and a moved declaration, so not kept. All restored.
// M3 lane 13 r74 (fast with 0xe1b60/0xe23e0): parent pivot as one conditional on the property and the child by a
// pointer test on it 376 (1947 bytes, masked aligned 224 -> 207); `limit <= distance` 370 (1944); distance declared
// at its assignment 354, the compare as `parent_body_index == property_a` 354, the pivot pointers declared with
// their defaults 354 (same code). Frame, low to high: retail origin 0x40, property_a/distance 0x4c, matrix 0x50,
// property_b/limit 0x84, placed 0x88; ours property_a/distance 0x40, origin 0x44, placed 0x50, property_b/limit
// 0x5c, matrix 0x60.
// M3 lane 12 r112 (fast with 0xe1b60/0xe23e0, both matched; base 371 at 1931, aligned 229, masked 91). Diagnostic, not
// kept (each part is match-only): the two pivot ternaries, a `reached` bool ahead of the target copy and retail's no-parent arm
// (shift from the position, origin overwritten by *point, z lowered) together 318 at 1980 bytes, masked 46 (the pivot select
// and the travel test are then retail's). What is left there: retail cross-jumps the two arms' tails (the position y/z stores
// and the 0x1d0c20 call, entered from the parent arm by `jmp 0xe2397`) because both arms leave y in xmm1 and z in xmm0; our
// parent arm loads the target into xmm1/xmm0/xmm5 (retail xmm2/xmm1/xmm0) with the rotated-pivot sums one register lower, so
// its tail stays a separate copy (the extra 43 bytes); the collision arms' products also take the other operand form (ours
// copy t and multiply by the offset in memory, retail loads the offset and multiplies by t in the first arm). Frame: retail
// packs the rotated pivot into the world-pivot/delta/back slot (+0x10) with the target at +0x1c; ours packs it into placed's
// slot with the target at +0x10. Trials on that cause. On this body: the target copied into matrix.position before the
// rotation and the rotated pivot taken off with `-=` 360 at 1927 bytes (aligned 268, masked 105: positional only, the matrix
// moves to +0x44 and the tails stay apart; not kept); that with retail's no-parent arm 349 at 1932 bytes (masked 80; the two
// tails now cross-jump at the z store and the call, but the arm reuses origin for the new point and computes the one-use
// shift ahead of the stores, so it is match-only). On the 318 form: the final rows written z, y, x 318 (one line moves); the
// target copied into matrix.position, then `-=` 340 at 1958 bytes (masked 53; the tails cross-jump at the z store and the
// matrix home becomes retail's +0x50, but the copy is ours, not retail's register loads); the copy through real_point3d_set
// 318 (same code as the 318 form: its stores fold away). Retail's tail needs the parent arm's target loads in xmm2/xmm1/xmm0.
// MiniMax B (11-11): at 356 (from 371): the pivot select now tests `property_a != parent_body_index` and takes
// pivot_b first, which is the order retail keeps the two pivots in; what still differs is the pairing of the stack
// slots (ours takes the two addresses at esp+0x48 and esp+0x88 where retail takes esp+0x4c and esp+0x60), our
// select compares in eax/esi and reads cmp [esp+0x40],ecx where retail compares in ecx/edi and reads the frame,
// retail's travel test is materialized as a bool where ours branches on the raw comparison, and the second
// transform puts the scale in xmm2/z where retail uses xmm3/xmm2.
// MiniMax B (11-11): no new form, no lower score (371): what still differs is the pairing of the stack slots
// (ours takes the two addresses at esp+0x48 and esp+0x88 where retail takes esp+0x4c and esp+0x60), the
// order of the two pivots (ours keeps the first in esi and the second in edi, retail the other way round),
// and the shape of the 0xdc3a0 call.
// M3 lane 13 r85 (fast with 0xe1b60/0xe23e0, both stay matched). Diagnostics: the two pivots' names swapped through the body,
// and the two properties' names swapped, 356 each (same code: names do not order these address-taken slots). Trials through
// existing helpers: delta through vector3d_from_points3d 356 (same code); back through vector3f_scale(&offset, -1.0f) 356 (same
// code); offset through vector3d_from_points3d 372 (1942 bytes); both collision arms' target through vector3f_add_scaled 356
// (same code); placed through real_point3d_set 356 (same code).
// MiniMax A (11-12): no new form, no lower score (356); the first difference is at +0x71, where this build's origin
// sits at [esp+0x44] and its matrix at [esp+0x60] while retail's origin sits at [esp+0x40] and its matrix at
// [esp+0x50], so every slot below the matrix sits four bytes higher although both frames are 0x12c bytes; moving a
// local to shape the frame is not allowed.
// P438: Original component/position bindings, limit/lowered/distance producers and
// the existing transform matrix/input/output bindings fixed independently: eight 356.
// NOT kept: matrix/origin/property homes unchanged; complete body and helper restored.
// MiniMax A (11-13): no new form, no lower score (356): the frame agrees (0x12c) and the first difference is at +0x71, where the
// register choice in the entry block differs; the body is one byte longer than retail's.
// M3 lane 4 r196 (fast with 0xe1b60/0xe23e0, both stay matched; plain agrees). KEPT 356 -> 336 (1924 B): the pivot select as
// defaults `parent_pivot = &pivot_a; child_pivot = &pivot_b;` then `if (property_a != parent_body_index)` swapping them (retail
// copies pivot_a's register into the parent register before the compare, `mov ecx, edi; je`); the pivots then take retail's
// +0xc8/+0xd4 (the first reference in the select orders them). Diagnostics: the select as `==` with both arms 364 (pivots
// right too), the target and world-pivot names swapped 336 (same code), the pivot declarations swapped 336 (same code), the
// rotated pivot named aaa_/zzz_ 336 (same code). On 336: rotated_pivot declared with the other parent-arm locals 336,
// back/direction declared at the parent-arm top 336, target declared at function scope 336 (all the same code), the first
// collision arm as `t * offset.i` 336 (same code), the first arm through vector3f_add_scaled 336 (same code). Cause found
// (diagnostic, not kept: a name reused): the rotation written into `delta` 331, the world-pivot group then takes retail's
// +0x10 and the matrix +0x54: retail packs the rotated pivot and the alias-safe helper's copy into the world-pivot/delta/
// back slot, ours into placed's slot (a compile-only TU harness, build/r196/h/cb.py of m3-lane4, shows the same packing;
// names, declaration places and scopes do not move it). Next: a natural form that ends placed's memory life after the
// rotation or starts the rotated pivot's earlier.
// P475: Whole-coordinate standard range origin/target/direction producers, seven subsets
// then all backward: 399/440/390/496/411/438/488/485. NOT kept: native
// matrix/origin/pivot homes not recovered; original transfers and complete body restored.
// @retail 0xe1c40
bool __stdcall function_e1c40(long arg_159e6d, point3f const *point, real value, long rigid_body_index,
	long contact_index, long parent_body_index, bool started)
{
	s_havok_component *component = havok_component_get(BIPED_GET(arg_159e6d)->havok_component_index);
	point3f const *position = biped_rigid_body_position(component, rigid_body_index);
	point3f origin;
	transform4x3f matrix;
	bool result = false;

	real_point3d_set(&origin, position->x, position->y, position->z);
	havok_component_rigid_body_matrix_get(rigid_body_index, component, &matrix);
	if (started || function_168f40(0x80040d, (s_vehicle_ray const *)&origin, arg_159e6d, NONE))
	{
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
			s_biped_collision collision;

			collision.unknown24 = NONE;
			havok_component_rigid_body_matrix_get(parent_body_index, component, &parent);
			function_1cff80(&component->unknown7c.data[contact_index], &pivot_a, &pivot_b);
			havok_component_contact_properties_get(component, contact_index, &property_a, &property_b);
			parent_pivot = &pivot_a;
			child_pivot = &pivot_b;
			if (property_a != parent_body_index)
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
			real limit = value - 0.025f;

			target = world_pivot;
			if (!(limit > distance))
			{
				if (function_1697c0(0x80040d, point, &offset, arg_159e6d, NONE, (s_collision_result_1697c0 *)&collision))
				{
					real t = collision.t;

					if (limit > t * distance)
					{
						target.x = offset.i * t + point->x;
						target.y = offset.j * t + point->y;
						target.z = offset.k * t + point->z;
					}
					else
					{
						t -= 0.025f / distance;
						target.x = offset.i * t + point->x;
						target.y = offset.j * t + point->y;
						target.z = offset.k * t + point->z;
					}
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
				vector3f direction = delta;

				back.i = offset.i * -1.0f;
				back.j = offset.j * -1.0f;
				back.k = offset.k * -1.0f;
				if (function_30bf0(&back) != 0.0f && function_30bf0(&direction) != 0.0f)
				{
					transform4x3f rotation;

					matrix4x3_rotation_between_vectors(&rotation, &direction, &back);
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
		result = true;
	}
	return result;
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
// MiniMax G (11-14): no new form, no lower score (91): the reset dispatch still expands at this call site, so the
// body is 113 bytes against retail's 61, and retail's own signature takes the biped in edi and the index in esi
// where this build takes one argument: BLOCKED-BY 0xe5930.
// MiniMax G (11-13): no new form, no lower score (91): the reset dispatch still expands here (113
// bytes against retail's 61) where retail calls 0xe5930 out of line, and that callee lives in
// another file: BLOCKED-BY 0xe5930.
// MiniMax B (11-13): no new form, no lower score (91): retail's own signature takes the biped in edi and the index in esi where ours takes one argument, and the caller of that signature is in src/unit_object_type.cpp, so the difference is reported and not patched here.
// MiniMax A (11-13): no new form, no lower score (91): the reset dispatch still expands at this straight-line call site, so the body is 113 bytes against retail's 61; a switch or a do-while(0) wrapper around the call does not keep it out of line, and the callee's own file (src/unknown_0e57e0.cpp) is not this lane's: BLOCKED-BY 0xe5930.
// MiniMax B (11-12): no new form, no lower score (91): this build inlines the body of 0xe5930 (defined in
// src/unknown_0e57e0.cpp) into the caller where retail calls it, so the body is 113 bytes against retail's 61 and the
// comparison pairs only the two calls; the noinline for 0xe5930 is a change in another file, which this lane may not make.
// MiniMax S (11-11): no new form, no lower score (91): the body is still 113 bytes against retail's 61 because the
// reset dispatch expands here instead of staying a call, and the unit index is still saved in EBX and pushed where retail
// keeps it in ESI; the reset helper's own file (src/unknown_0e57e0.cpp) is not this lane's: BLOCKED-BY 0xe5930.
// MiniMax R (11-10): no new form, no lower score (91); the reset dispatch still expands at this straight-line call site, so the body is 113 bytes against retail's 61, and the reset helper's own file is not this lane's.
// MiniMax I (11-10): no new form, no lower score (91); the reset dispatch still expands at this straight-line call site (113 bytes against retail's 61), and the reset helper's own file is not this lane's.
// MiniMax E (11-08): no new form, no lower score (91); the reset call still expands here instead of staying a call, so the body is 113 bytes against retail's 61 and the unit index rides in EAX and EBX where retail keeps it in ESI; the reset helper's own file is not this lane's.
// MiniMax G (11-07): `if (biped != NULL)` around the reset call (always true: biped's flags are read right after)
// keeps 0xe5930 out of line, 5, and a new noinline declaration of this function takes 0xcaf00 13 -> 5; neither kept
// (an always-true test that only steers inlining, and a new noinline).
// MiniMax D (11-06, run 49): no new form, no lower score (91); 0xe5930 (src/unknown_0e57e0.cpp, not this lane's file) still expands here, so the body is 113 bytes against retail's 61.
// MiniMax A (11-04): no new form, no lower score; the reset call still expands here.
// MiniMax A (11-03): no new form, no lower score: 0xe5930 (src/unknown_0e57e0.cpp, not my file) is still
// inlined at this straight-line call site, so the function is 113 bytes against retail's 61.
// MiniMax R (11-01): no new form, no lower score: 0xe5930 (src/unknown_0e57e0.cpp, not my file) is still inlined at this straight-line call site, where retail keeps the call.
// MiniMax E (11-11): no new form, no lower score (91): our LTCG still expands the reset-mode dispatch where retail calls 0xe5930 out of line (113 bytes against 61); the callee's file is not mine and a new noinline declaration is not allowed: BLOCKED-BY 0xe5930.
// MiniMax H (11-12): no new form, no lower score (91): the reset dispatch still expands at this straight-line call site, and retail takes the index from ESI where this build takes it from the argument register.
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
// Round 192: the biped looked up after the 0xe18d0 call 69 (96 bytes, but only because our inlined 0xe5930 then
// shares the lookup; retail looks it up first, so not kept); `!(*((byte *)biped + 0xc0) & 0x40)` 91; field_bit_test
// 98; an early return on the bit 91; TEST_FIELD_BIT through a fresh lookup with no biped local 101. BLOCKED-BY 0xe5930.
// Round 65 (lane 9): the call is straight-line here, and our LTCG inlines 0xe5930 only at such sites (a call under a
// condition stays a call, as in 0xe2d80, 0xe24f0 and 0xec0d0); diagnostic `if (arg_159e6d != NONE)` around it (not
// retail's, reverted) 7 at 62 bytes, the rest then being the TEST_FIELD_BIT flag test (98 alone). A named
// `asleep` bool read after the calls 98; field_bit_test 98. Still BLOCKED-BY 0xe5930.
// Round 214 (m3 lane 3, fast): TEST_FIELD_BIT on the flag test 98 (118 bytes); a named copy of the index 91. 0xe5930 body spellings as in 0xde4c0's note:
// 91 while it still matches, 94 and 98 otherwise. 0xcaf00 stays 13 throughout. Still BLOCKED-BY 0xe5930.
// MiniMax R (11-11): no new form tried this round, no lower score (91): retail keeps the index in ESI and pushes only the result pointer and the 1, this build saves the index in EBX and pushes it too, so the prologue saves three registers against retail's one.
// MiniMax A (11-14): no new form, no lower score (91): our LTCG still expands the 0xe5930 reset
// dispatch at this straight-line call site where retail keeps the call out of line (113 bytes
// against 61); the callee's file is not mine, so the boundary cannot be recovered here:
// BLOCKED-BY 0xe5930.
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

void rotate_direction(vector3f *vector, vector3f const *axis, real sine, real cosine); /* forced inline in src/unknown_1eb8a0.cpp */

/* turns a tumbling biped by its angular velocity over a tick, keeping its
   facing and up square */
// score 79 (was 79): first rotation and staged cross product operands (load-first where retail copies the axis
// register), 894 against 887 bytes. Round 112 (m3 lane 12) note at the end.
// MiniMax G (11-13): no new form, no lower score (79): the diff re-read this round finds the same first difference, inside the shared rotation helper whose own file this lane may not edit, and the operand orders listed above still measure at or above the body, so the body stands.
// MiniMax B (11-13): no new form, no lower score (79): the frame agrees (0x24) and the first difference is at +0x107, where retail sums the cross product's terms in the register before the multiply while this build spills the factor first; the body is seven bytes longer than retail's.
// MiniMax H (11-12): no new form, no lower score (79): the angle multiplied the other way round measures 79, the sine and the cosine taken the other way round measure 111 and the second rotation taking the up field measures 143, so the body stands.
// MiniMax G (11-12): no new form, no lower score (79): nothing new this round; the first rotation and the staged cross product
// operands (load-first where retail copies the axis) still differ.
// MiniMax G (11-12): no new form, no lower score (79): the first rotation and the staged cross product operands
// (load-first where retail copies the axis) still differ.
// MiniMax D (11-11): no new form, no lower score (79); the first difference is still at +0x107, inside the
// inlined rotation helper, whose body lives in another file and is not this lane's to change, so the bytes
// from there on cannot be moved from here. What is left in this file is the first rotation's cross terms,
// which read the forward x before the axis copy where retail copies the axis register first, and the
// normalization's SIMD numbering; both are the operand-order forms the earlier rounds already measured.
// MiniMax E (11-11): no new form, no lower score (79); the difference is inside the shared rotation helper
// rotate_direction, which is defined in src/unknown_1eb8a0.cpp, a file this lane may not edit.
// MiniMax S (11-10): no new form, no lower score (79); the reduced build of this function is not the
// full build (in the narrow three-source image it is 483 bytes against retail's 887 and two calls
// reach what the image leaves out, in the wide image it is 894 bytes), so the fast check cannot be
// used as the signal here and the body stays as it is: the shared rotation helper's bytes, the
// normalization SIMD numbering and the first rotation's cross terms still read the forward x
// load-first where retail multiplies by it in memory.
// MiniMax B (11-07): 63 (892 bytes), not kept: the first rotation and the normalize call take &biped->forward
// while the forward local declared above them serves the later uses; with the local in all of them 79 (main),
// with the field in all of them 154, with the local declared at its first use (the first cross) 77 at 887 bytes.
// A spelling and declaration point picked by score are kept only with a byte match. Left at 63: the first
// rotation's dot sum order (ours k, j, i against retail's i, k, j).
// MiniMax C (11-05): no new form, no lower score: the normalization SIMD numbering and the trigonometric
// and rotation homes remain.
// MiniMax A (11-03): no new form, no lower score: the normalization SIMD numbering and the trigonometric
// and basis homes remain.
// MiniMax C (11-07): round 53: no new form, no lower score; the shared rotation helper's bytes are the difference.
// Earlier 79 (was 79): frame 0x24 and the register homes agree, 894 bytes against 887, first +0x107. Left: the first rotation's cross terms read the
// forward x load-first where retail multiplies by it in memory, and the final cross reads forward y load-first where retail has it
// in memory (one extra load, the seven bytes). Earlier: 145: frame 0x24 and retail length agreed; first +0x4f was normalization SIMD numbering,
// followed by rotation/basis carriers and trigonometric homes. Earlier frame 0x30 had cross spills.
// Earlier, at 184 (from 184): frame 0x24 and entry saves agreed; axis copy/norm order, early normalization loads.
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
// Round 312: indexed rotation publication 179; complete dot products 167 (four-point lead, restored);
// both 145 plain, retained; indexed axis/cosine/both weighted vectors 150/207/198;
// indexed parallel component terms 145; cross-first named-dot indexed form 175.
// Existing twice-used helper only; original arithmetic association and in-place input safety preserved.
// Round 315: Complete sine-weighted cross on the retained indexed-dot graph, with no/cosine/axis/both
// weighted companions 149/206/175/210; complete vector-sum publication with input/output
// and native sum receivers 238/219/222/220. Original helper/body restored; normalization SIMD
// ranks, trigonometric homes and rotation/basis carriers remain.
// M306: Native scalar component accumulation at four prefix/publication boundaries, with all factors first/last: 173/173/170/170/173/173/170/170. All restored.
// M306: Existing rotation helper force-inline on the original retained expression 145; restored.
// Round 361: Existing rotation input/result by value, crossed with native axis
// value, staged cross and shared dot: 329/402/329/412/318/490/319/463. Original
// in-place helper and full body restored; normalization SIMD ranks and trig/rotation
// homes remain. Alternate dot grouping was exact-only and is restored.
// Round 470: Repeated forward-field receiver at first rotation/before normalization,
// each with no/forward/up/both fallback vector setters: 191/186/185/193/220/221/220/228. All restored;
// normalization SIMD ranks and trig/rotation/basis homes remain.
// Round 325: Actual sine/cosine/axis const-reference formals in the twice-used rotation,
// nonempty subsets then all with vector reference: 145/145/145/145/145/145/145/145. Original helper and body
// restored; normalization SIMD numbering and coefficient/rotation homes remain.
// P268: Native angle/sine/cosine lexical home subsets 1/2/4/3/5/6/7, then all with
// complete private projection/cross scalar identities: 145 each. Original helper/body restored;
// normalized-axis SIMD ranks, trigonometric homes and rotation/basis carriers remain.
// N321: Complete in-place vector accumulation through existing scale/add producers, with
// no/cosine/projection/sine/pair/all workspaces, all regress and grow the body. Original private
// helper and target body restored; normalized-axis SIMD and trig/rotation homes remain. Scores
// 183/197/263/200/217/201/285/218.
// Original executable source restored.
// Round 371: Rounded sine/cosine wider storage and named cosine-complement,
// subsets 1/2/4/3/5/6/7: 182/231/175/276/192/240/286; all with angle
// producer declaration 286. Original real coefficients, helper and body restored.
// Round 373: Native int/unsigned-int/dword/short/word/char/byte publication ordinals
// and plain output cursor: 145/145/145/145/145/145/145/175. Original loop/helper/body restored; normalized
// axis SIMD ranks and coefficient/rotation homes remain.
// N327: Complete native up-field rotation/reconstruction ownership, ordinary/staged side/up cross subsets
// and producer/entry side declarations: 170/170/171/171/170/170/171/171 at 887 bytes. All shrink the
// frame to 0x18 versus retail 0x24 and regress. Original native up snapshot, rotation helper and whole
// body restored; normalization SIMD/trig/rotation homes remain.
// Lane 1 round 62: retail's rotation written as dot (1 - cos), scalar cross j and k locals before the in-place rows,
// cross i inside its row, against this body's aligned 102 mnemonics / 166 full: dot written i, k, j 162 (33/152, first
// difference +0xc6: the normalization colours become retail's), through dot3f 159 (59/86, 899 bytes), named projection
// and dot locals 166 (46/110), rows cosine-first 162; dot orders ijk/jik/jki 177 (34/100), ikj/kij/kji 162, axis-first
// 177/162; three named projections k, i, j 170, k, j, i 170, i, j, k 168; one named projection and cross k ahead 184.
// Lead: retail's first rotation evaluates v.k * axis.k and v.j * axis.i ahead of the cosine; none of these gives it.
// All raise the positional score, so this body stays.
// J244: Existing staged rotation/side/up crosses on retained indexed publication,
// seven subsets then all with shared dot: 175/145/143/174/175/143/175/143.
// NOT kept: two-point lead below threshold; rotation-stage extents 879/887.
// Original helper/body and arithmetic restored; normalization SIMD and trig/basis homes remain.
// Y551: Actual axis assignment/component initializer/shared setter/library copy
// on the indexed-dot graph 145/224/211/145; with a separate magnitude result
// 137/224/228/137. NOT kept: 137 uses a one-use call-result local without a
// match. Original body/helper restored; normalization SIMD and coefficient homes remain.
// M342: Existing in-place rotation returns its output pointer, ignored/forward/up/both
// consumer handoffs with ordinary/forced inline: 145/145/145/145/145/145/145/145. NOT kept: no exact
// output-handoff graph. Original helper and complete body restored; normalization SIMD/trig homes remain.
// AC508: Reversed rotation cross with sine subtraction on retained indexed-dot graph,
// ordinary/staged producers, shared dot, cross-first capture and complete parallel parts:
// 179/156/182/158/168/168/178/179. NOT kept: no exact sign/scheduling match; original helper/body
// restored. Normalization SIMD ranks and trigonometric/basis homes remain.
// Y559: Full unrounded CRT cosine/sine/both double results, original then
// producer declarations; float trig pair with original/producer declarations:
// 223/182/276/223/182/276/191/191. NOT kept: no byte match; normalization SIMD,
// native cosine/sine and rotation/basis homes remain.
// M342: Existing in-place rotation returns its output pointer, ignored/forward/up/both
// consumer handoffs with ordinary/forced inline: 145/145/145/145/145/145/145/145. NOT kept: no exact
// output-handoff graph. Original helper and complete body restored; normalization SIMD/trig homes remain.
// F359: Complete projection reductions in four source phases, original/early cross capture:
// 181/175/188/169/190/179/192/177. NOT kept: no exact projection-phase repair.
// Original complete body and helpers restored.
// P320: Complete projection declaration orders ikj/jik/jki/kij/kji 145 each;
// P320: immutable ijk/kij and early dot storage on the indexed graph 145/145/145.
// P320: NOT kept: no exact producer/home repair. Original helper and complete body
// P320: restored; normalization SIMD ranks, trig homes and rotation/basis carriers remain.
// M362: Existing rotation owns cosine/sine/both from real then double angle formals,
// 227/200/227/237/190/236; both float trig producers in that graph 228/235.
// NOT kept: native once-computed coefficients and frame are lost; no exact match.
// Original helper contract, coefficient lifetimes and complete body restored.
// M364: Complete parallel projection scalar input captures, axis/vector/both in both
// declaration priorities, mutable then const: 175/190/177/177/175/190/177/177. NOT kept: one-
// use captures do not match; normalization SIMD ranks and trig/basis homes remain.
// M372: Row-local cosine/projection/cross product parts inside the existing
// publication loop, seven subsets then complete immutable parts:
// 179/176/196/154/181/170/190/190. NOT kept: all regress; original twice-used helper
// and body restored; normalization SIMD and trigonometric/basis homes remain.
// Round 588: Angle/rotation/basis last-use scopes, seven subsets, then all with double angle:
// 145/145/145/145/145/145/145/151. NOT kept: no exact scope/domain repair; original
// complete body restored. Normalization SIMD ranks and coefficient/basis homes remain.
// F386: Existing vector/axis projection component-array observations, three subsets
// ordinary/forced inline, then complete readonly projections ordinary/forced: 145 each.
// NOT kept: no gain; original helper/body restored, normalization SIMD/trig homes remain.
// P340: Tick-rate scalar ownership at the normalization product, mutable/immutable real rate and
// magnitude carriers at two producer boundaries: 210/210/210/210/157/157/157/157. NOT kept: unmatched
// one-use carrier graph; original product, coefficients, helper and complete body restored. Native 0x24
// frame, normalization SIMD and trig/basis homes remain.
// J256: Twice-used cosine complement at cosine/sine boundaries, mutable/immutable and ordinary/forced rotation:
// 163/163/163/163/163/163/163/163. NOT kept: all eight common-factor forms regress from 145 to 163; the added inline formal and hoisted factor miss required byte equality
// Original executable source restored; native frame and allocation gaps remain.
// Round 214 (m3 lane 3), kept: the existing rotate_direction in place of the private indexed helper, side = forward x up through
// cross3f and up = side x forward through cross3f_staged (retail's signs and its k, j, i products with i, j, k stores), a named
// `forward` at every use: 145 -> 79. Steps (masked aligned counts in brackets, base 61): shared scalar helper with the staged cross 154 [19]
// at 879 bytes, with `forward` on the rotation and the normalize only 72 [17] at 887, at every use 79 [13] at 894. NOT kept: the first rotation
// on the field and `forward` for the rest 63 [11] at 892, for the crosses only 60 [15] at 887 (a mix of the field and the pointer).
// Tried on the helper: its dot and cross statements in every order 74+; accumulated dot in six orders 60+ [10 at best]; named dot terms 74+;
// separate read paths for dot, cross and rows 60+. On the body: an up field pointer before or after the first rotation, seven use patterns,
// 172+/72+; the pointer at the top of the body 130 [20]; a copy-taking rotation helper 139+; both crosses plain 203 [39], with a local
// result or into the up copy 242 [50]; side through scalars 143 [28]. Diagnostics, not kept: identity pointer chains in the body and helper:
// one dead up pointer plus one helper alias has all after the first call exact (57 at 887) with the first rotation 13 off; two dead pointers
// have the first rotation 3 off and the rest 12 off; chains saturate at four. The operand state of the two blocks has to change separately.
// Round 70 (m3 lane 14, fast with 0x1ecf50/0x1e5bb0/0x30bf0; 0x1ecf50 must be listed, alone the reduced image gives 621):
// in the shared rotate helper, the v.i products of cross j and k written axis-first, one or both, 79/79/79 (same code:
// product operands are canonical; 0x1ecf50 unchanged); the trig values inline in both rotate calls, no sine/cosine
// locals 250 (929 bytes); cosine declared before sine 79; side through cross3f_staged and up through cross3f 100; up
// and side declared at their first use 79.
// Lane 1 round 79 (fast, same companions): `angle = rate * normalize3d(&axis)` 79; sine before cosine 111; axis declared,
// assigned before the normalize 79; angle/cosine/sine declared at their assignments 79; one up field pointer for the
// staged cross, the length test and the reset 225 (913 bytes). A CL-only copy with rotate_direction's body pasted in
// reproduces our listing; 150 random mixes of local declaration orders, forward-pointer placement, trig order, axis
// assignment, up at its copy and rate-first never went below the base: the operand sides sit in the shared helper.
// P363: On the shared-rotation/native-prefix graph, up snapshot through memcpy/memmove/setter
// 79/145/215; both fallback copies through memcpy/memmove/setters 79/73/94;
// final nonzero-length return 79; both input snapshots through memcpy 79.
// NOT kept: memmove fallback gains only six; transfer/allocation forms miss equality.
// Original complete body restored; first rotation i operands and final cross j reload remain.
// P374: Independent angular memmove/setter/aggregate/memcpy producers on the shared rotation graph,
// then complete rotation scopes: 204/239/217/79/146/212/225/130. NOT kept: no exact input/lifetime repair;
// original complete body restored, native frame and first rotation operand gap preserved.
// P376: Shared-rotation forward value owning rotation/rotation+normalization, aggregate/setter/memcpy/memmove:
// 246/207/246/245/250/250/250/245. NOT kept: regressions, no native operand/home equality.
// Original full body restored; first rotation and final-cross memory operands remain.
// P378: Reconstructed-up output in consumed up/side workspaces, aggregate/setter/memcpy/memmove
// publication before the field normalizer: 252/154/252/273/255/130/255/273. NOT kept: no exact
// output-home repair; all extents grow beyond native. Original complete shared-rotation body restored.
// P381: Complete final normalized/degenerate outcomes in bool/byte/int/long,
// positive then complementary: 79/79/79/79/79/79/79/79. NOT kept: no native
// first-rotation/final-cross operand repair; original direct final test and whole body restored.
// P385: Complete forward/biped member references and readonly pointer identities,
// then readonly input reference/pointer across both snapshots: 79/79/79/79/79/79/153/153.
// NOT kept: no exact first-rotation/final-cross operand repair; all aliases restored.
// T73: Consumed rotated-up/rotation-axis side-cross outputs, ordinary/staged final crosses,
// then complete final-up publication through either consumed workspace:
// 92/201/144/79/186/200/254/285. NOT kept: tie/regressions; native first-rotation and final-cross
// memory operands remain. Original shared rotation, independent outputs and full body restored.
// P395: Float cosine/sine APIs on the shared-rotation graph 93/93/158; separate
// normalization result alone/with cosine/with sine/with both 153/158/158/159;
// both float APIs with forced caller inline declaration 158. NOT kept: all regress;
// original coefficient producers, rotation declaration and complete body restored.
// M3 lane 4 r176 (fast with 0x1ecf50/0x30bf0; base 79 at 894). What is left is operand order only: retail copies the axis
// register and multiplies by forward->i in the first rotation's cross j and k rows, and by forward->j in the staged cross's
// k and i rows (`movaps reg, reg; mulss reg, [esi + n]`, 7 bytes shorter in all), where ours loads the field first. The
// first rotation through a private copy of the helper with its cross through cross3f into a local 167 (901 bytes, every
// product memory-first). Mixes of the field and the pointer, not kept (match-only): the field for the first rotation and
// the staged cross 60 (887 bytes: the first rotation's cross rows are then retail's, but its dot adds k, j, i with j read
// early where retail adds i, k, j with k read early, and the staged cross has the copied form on forward->k), that with the
// reset on the field 60, with the normalize and the reset too 60; the staged cross alone 83 (887), with the reset 83, with
// the normalize 83; the first cross alone 79 (same code); the normalize alone 79 (same code); the first rotation and the
// first cross 84 (876), plus the staged cross 89 (874), plus the normalize instead 84 (876); the first rotation and the
// normalize 63 (892). All restored.
// M3 lane 5 r167 (fast with 0x1ecf50/0x30bf0; base 79 at 894, masked aligned 13): the angle as `angle =
// normalize3d(&axis); angle *= g_510c54->rate;` 79 (same code); a named `speed` for the length 153 (877 bytes, the listed
// speed local re-measured); a const pointer to the angular velocity for the axis copy 208 (880 bytes); that with the forward
// pointer declared first 178 (897 bytes); a const pointer to the old up for its copy 79 (same code); a pointer to the new up
// for the length test and the reset 79 (same code); re-measured: one up pointer for the copy, the staged cross, the test and
// the reset 225 (913 bytes). The
// lever left is the per-site operand state inside rotate_direction (src/unknown_1eb8a0.cpp), which was on this
// round's Avoid list, so no helper-side form was tried. All restored.
// M3 lane 13 r74 (fast with 0x1ecf50/0x30bf0): `angle = normalize3d(&axis); angle *= rate;` 79 (same code); the final
// test as `!function_30bf0(&biped->up)` 79 (same code); the axis copied componentwise 217 (912 bytes); `forward`
// through a second biped lookup 79 (same code); the up copy through a second lookup 246 (909 bytes: redone after the
// first rotation's stores).
// M3 lane 12 r112 (fast with 0x1ecf50/0x30bf0; masked aligned 13): the forward pointer declared and set right after the biped,
// ahead of the axis copy, 130 (first difference +0x5a). No other untried body form: the 13 masked lines are the operand form
// of the forward.i products in the first rotation's cross j and k (retail `movaps reg, axis; mulss reg, [esi]`, ours `movss
// reg, [esi]; mulss reg, axis`) and of the forward.j products in the staged cross, plus where the (1 - cos) spill is
// scheduled. They come from inside rotate_direction (src/unknown_1eb8a0.cpp, on this round's Avoid list) and the shared
// cross3f_staged (include/vector_products.h, used by matched functions), so no helper-side form was tried.
// M3 lane 12 r115 (fast with 0x1ecf50/0x30bf0), helper side: rotate_direction as 0x2551f0's helper (a named vector_k, the
// projection in one expression) 134 (895 bytes, 0x1ecf50 lost); as 0x1055b0's spelling (`(1 - cos) * dot3f`, rows
// axis-first) 144 (905, 0x1ecf50 lost); its dot as dot3f(axis, vector) 79 (same code); with named axis_i/j/k 130 (904,
// 0x1ecf50 lost; close to M364's captures). Body: a snapshot of forward taken after the normalize and read by the side
// cross 162 (913), by the final cross 164 (913). Re-measured: up and side declared at their first use (Round 70) with the
// side through cross3f_staged 79 (same code); the final cross into the up copy, then stored (P378) 252 (937). Compile-only screens (the helper pasted into a copy of this file reproduces this listing): 48 dot/cross statement
// orders, 250 helper spellings (operand orders, the dot in each order, row term orders, indexed access) and named
// components in every order never beat this body; 300 caller mixes of the field and the pointer per use, declaration
// points and an up pointer beat it only with the field in the first rotation (the 63 mix above). A copy of forward read by
// the side cross gives retail's final-cross operand sides, so retail's final cross reads forward apart from the side cross.
// M3 lane 13 r85 (fast with 0x1ecf50/0x30bf0, both stay matched): the forward field read at every use, no pointer local, with
// the present helpers 154 (879 bytes); the pointer declared at the top, among the locals or after them and assigned where it
// is now 79/79/79 (same code); a `vector3f *const` pointer 79 (same code). Re-measured: axis declared, then assigned before
// the normalize 79; sine before cosine 111. The five matched functions that inline the same rotation (0x1055b0, 0x177d10,
// 0x23d260, 0x2551f0, 0x29c860) each spell it privately, so none gives a shared helper form to copy.
// P438: Existing fallback coordinate ranges through native/forward/backward library
// copies, excluding native/native: 65/77/69/74/80/77/68/79. NOT kept: the 65/69/68
// leads add out-of-line copy calls absent in retail; other gains miss the gate.
// Original complete body and include set restored; first rotation/cross operands remain.
// MiniMax A (11-13): no new form, no lower score (79): the frame agrees (0x24) and the first difference is at +0x107; the body is
// seven bytes longer than retail's.
// M3 lane 5 r179 (fast with 0x1ecf50/0x30bf0/0xe3c90): the axis local renamed (name order as a lever on the cross
// products' operand order) 79 (same code). No further trial: the caller-side forms are exhausted (lane 12 r112/r115,
// lane 1 r79, lane 13 r85) and rotate_direction's file was on this round's Avoid list again. Body unchanged.
// P475: Original axis/up snapshot ranges through native/forward/backward transfers,
// excluding native/native: 204/204/218/248/248/218/248/248. NOT kept: no exact
// first-rotation/final-cross operand repair; original snapshots and full body restored.
// MiniMax H (11-14): no new form, no lower score (79): the frame agrees (0x24) and the first difference is still at +0x107, inside the shared rotation helper, whose own file is not this lane's.
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
	vector3f *forward = &biped->forward;
	rotate_direction(forward, &axis, sine, cosine);
	function_30bf0(forward);
	up = biped->up;
	rotate_direction(&up, &axis, sine, cosine);
	cross3f(forward, &up, &side);
	cross3f_staged(&side, forward, &biped->up);
	if (function_30bf0(&biped->up) == 0.0f)
	{
		*forward = *g_4687a8;
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
// MiniMax B (11-12): no new form, no lower score (2): the two points are still the 0x1fb7e0 call's argument setup -
// retail pushes the first NONE, moves the actor into ECX, zeroes EDX and pushes the second NONE, this build pushes both
// NONEs before it touches the registers; the actor index in a named local, the two constants written as -1, the data
// argument cast to its own type and the type local as a long all measure the same 2 (894 bytes), so the order follows the
// callee's parameter list, which is declared in another file and cannot be steered from here.
// MiniMax S (11-11): no new form tried this round, no lower score (2): the two points are still the 0x1fb7e0
// call's argument setup - retail pushes the first NONE, moves the actor into ECX, zeroes EDX and pushes the
// second NONE, this build pushes both NONEs before it touches the registers; the order follows the callee's
// parameter list, which is declared in another file, so the call site cannot be steered from here.
// MiniMax R (11-10): no new form, no lower score (2); the event call still pushes both NONEs before it moves the actor into ECX and zeroes EDX, retail pushes one NONE, moves the actor, zeroes EDX and pushes the other NONE.
// MiniMax I (11-10): no new form, no lower score (2); the event call still pushes both NONEs before it moves the actor into ECX and zeroes EDX; the actor index in a local gives the same two differences, and the permuter's 41 tries kept 2.
// MiniMax E (11-08): no new form, no lower score (2); the event call still pushes both NONEs before it moves the actor into ECX and zeroes EDX, retail pushes one NONE, moves the actor, zeroes EDX and pushes the other NONE; the data argument as 0 or as a cast null, the actor as a long cast or as a local, the type as an int, a comma expression and a short cast on a NONE all leave the same two differences.
// MiniMax D (11-06, run 49): no new form, no lower score (2); the 0x1fb7e0 call still pushes the fourth argument before it sets ecx and edx, retail pushes it after.
// MiniMax A (11-03): no new form, no lower score: the 0x1fb7e0 call still pushes the fourth argument
// before it sets edx and ecx, while retail pushes it after; 0 and -1 literals for NULL and NONE keep 2.
// MiniMax R (11-01): earlier, the actor rode in ECX because LTCG knew magnitude3d preserved it; the helper file was closed.
// MiniMax E (11-11): no new form, no lower score (2): the fourth argument in a local and the fifth argument in a local both keep the same two differences (the event call still pushes both NONEs before it moves the actor into ECX and zeroes EDX), and the whole type selection as one ternary keeps the same two differences; the permuter (900 s, annealing) found no match.
// MiniMax H (11-11): no new form, no lower score (2): the two points are still the 0x1fb7e0 call's setup, where retail pushes the first NONE, moves the actor into ecx, zeroes edx and only then pushes the second NONE, and ours pushes both NONEs before the registers are set; that order comes from the callee's declaration and the argument list, which this lane may not edit.
// MiniMax H (11-12): no new form, no lower score (2): the two remaining points are the 0x1fb7e0 call's argument setup, where retail pushes the first NONE, moves the actor into ECX, zeroes EDX and only then pushes the second NONE, while this build pushes both NONEs before it touches the registers; that order follows the callee's declaration, which this lane must keep as it is.
// score 2 (was 2): Logic walk (2026-10-31) against retail 0xe3380-0xe36fd: callees (0xb9dd0, 0xddf60 + 0x1697c0 = the
// inlined 0xdf880 with offset 0.4 (0x45dd30) and length 8.0 (0x45dc18) at 0xe347a-0xe352f, 0x24590, 0x30bf0, 0x1fb7e0 with
// eax type/ecx actor/edx NULL and two pushed NONEs at 0xe36e4-0xe36ec), offsets (0xbc bit 6, 0x12c, 0x12a, 0x7c, 0x244, 0x34c byte,
// 0x38c, 0x88-0x90, 0x84, 0x94), constants (0.5, 2.0, 0.0, 0.3, 0.6, 1.5707964), the sum < time test (comiss/jbe 0xe3459),
// the normal copy over the direction slot (0xe366f-0xe3694) and the 0x54/0x55/0x56 selection all agree; no behaviour
// difference. Frame 0x84 agrees. Only the event-call setup remains: ours pushes both NONEs before `mov ecx, edi`; retail
// pushes the first, moves the actor, zeroes EDX, then pushes the second. R75: the magnitude tested inside the upright arm,
// selecting 0x55/0x56 there (19 -> 2 plain; the same with a named one-use angular_speed, not kept): the actor now has
// retail's EDI home and the six shifted exits agree. Caller-local inline
// declaration 19; split calls without it 20; one-use/two-use actor locals 2/2; split calls with it 20; inner conditional
// expression 2; fallback event with it 60. Earlier actor-in-ECX description and Round 35: both probes through 0xdf880
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
// Round 514: long falling/byte event/both 214/33/222; shared complete flight scale 2,
// with long falling/byte event/both 214/33/222; uninitialized event at entry 2.
// Original body restored; final actor move versus second argument push remains.
// Round 259: Bounded event enum with retained/at-use time, long/native-word ticks and
// original/shared complete flight scale 2/30/2/184/2/30/2/184. All restored;
// actor move versus second argument push remains the only two differences.
// MiniMax R (11-11): no new form tried this round, no lower score (2): the two points are still the 0x1fb7e0 call setup - the first NONE has to be pushed after the actor moves into ECX and before EDX is zeroed - and that push order comes from the callee's declaration and the argument list, which this lane may not edit.
// Round 98 (m3 lane 10, fast with 0x1fb7e0, 0x24590, 0xe3c90, which stay matched; control 2). Cause found: retail's order
// is not the callee's parameter order (the other 35 callers push both NONEs first) but one event call per arm, merged by the
// compiler: the merged block then sets the registers before the pushes and the first push is hoisted above them. With one
// call per arm (three, or two with the type local in the else arm) this build gives `push -1; xor edx, edx; push -1` but
// keeps the actor in ECX across 0x24590 (20 each; with a named actor local 20/20); retail holds it in EDI and copies it
// (`mov ecx, edi`), so retail's callers have no register information for 0x24590 (no retail caller keeps ECX or EDX
// across it). Diagnostic, reverted: a named actor local, three calls and 0x30bf0 (already `inline`) in 0x24590's place give
// retail's tail exactly (`mov edi, [edi + 0x12c]` ... `push -1; mov ecx, edi; xor edx, edx; push -1`; 27, all from the
// stand-in). BLOCKED-BY 0x24590: needs round 32's `inline` on magnitude3d's definition (src/unknown_023540.cpp, on this
// round's Avoid list, and six unmatched callers rose with it); the body for it is build/r98/lead-e3380.patch.
// Also tried: the call through a file-local forceinline wrapper with its parameters in five orders 2 each (same code); the
// type selection written as a conditional expression in the argument list 2 (same code), with a short cast 2, with `&&`
// 19, inverted 2; a named actor local with the single call 2 (same code); four calls 45; two calls with a 0x56 default 207.
// Round 416: Direct admission/timing/final-actor rejections and complete admission/actor guards
// 2/2/2/2; falling-default event 32; baseline outcomes plus one admitted event 38 (897 bytes);
// angular-velocity input pointer 2; final rejection with falling-default event 32. All restored;
// the final actor move versus second stack push remains.
// Round 101 (m3 lane 1): `inline` on magnitude3d's definition plus one event call per arm (the actor read at each
// use, or a named actor local) MATCH (fast; full check 11995, nothing lost), but seven unmatched magnitude3d callers
// rise: 0xd1e10 542->596, 0xf6d10 749->759, 0xf8eb0 947->1030, 0x185be0 590->942, 0x1aa0d0 375->402, 0x1d24a0
// 902->935, 0x2c0d60 920->963 (around the call they now take retail's xmm registers; the rises are positional), so it
// is not kept: BLOCKED-BY 0x24590. Without it (fast): per-arm calls with a `short type` local in the upright arm 20;
// two calls (falling, then `up.k > 0.6f && 1.5707964f > magnitude ? 0x55 : 0x56`) 20; `if (actor == NONE) return;`
// then per-arm calls 20; three calls with the upright and magnitude tests joined by `&&` 20.
// @retail 0xe3380
void __stdcall function_e3380(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	s_biped *vehicle = BIPED_GET(biped->parent_object_index);
	byte *state = (byte *)biped + *(short *)((byte *)biped + 0x12a);

	if ((TEST_FIELD_BIT((UNIT_DEFINITION(vehicle)->unit.flags >> 6) & 1)) && biped->actor_index != NONE &&
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
				else if (vehicle->up.k > 0.6f)
				{
					type = 1.5707964f > magnitude3d(&vehicle->angular_velocity) ? 0x55 : 0x56;
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
	s_object_header_view *header = object_header_get(arg_159e6d);
	s_biped *biped = (s_biped *)header->object;
	s_biped_definition *definition = BIPED_DEFINITION(biped);
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
				function_1e5af0(&biped->physics_mode, &biped->forward, &biped->up, output);
			}
			names[1] = 0xc000043;
		}
		if (!(TEST_FIELD_BIT(((s_biped_flags_10a_view *)&biped->flags_10a)->bit2)) && (TEST_FIELD_BIT((definition->biped.flags >> 10) & 1)) &&
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

bool function_1f5560(long actor_index, vector3f *facing, bool unused);

/* jumps: an upward kick to the definition's jump speed (less for a tired
   player), relative to what the biped stands on; an actor may refuse. */
// Matches with a false result, success after the support query and one return.
// MiniMax A (11-03): no new form, no lower score: retail keeps the riding flag at [ebp-0x25] and writes a
// dead 1 into the 0x109e00 argument slot, while ours keeps the flag in that slot.
// score 68 (was 68): 683 bytes and frame 0x28 agree; only result=true's position and result/riding's swapped slots left. Earlier (640 bytes against retail 683, frame 0x24 versus 0x28): Walked against retail block by block
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
// Round 313: Natural-parameter component loops for relative/kick/actor-ground subsets
// 240/228/176/228/240/228/167; complete dot/norm products 228.
// All three loops reach 167 plain at 683 bytes, but lose matched callers; restored.
// The 176 form also uses a register argument and ret instead of retail ret 4; restored.
// DL239: Natural parameter with native complete relative/kick/actor-ground vector producer subsets
// 1..7, all plus common result exit: 174/204/191/204/191/178/222/222. Original complete body
// restored; frame 0x24/0x28, state-base/result and vector homes remain.
// Ordinary lower-score forms lose matched 0xe4680 and 0xecfc0; all restored.
// Round 359: Natural input with all three twice-used ground components, reversed raw copies,
// existing kick producer and bool/short contact: 191/196/191/195/191/196/191/195.
// Actual admission/actor result with one exit and bool/short contact 190/192. All lose matched
// callers 0xe4680 and 0xecfc0 through the input convention; complete original body restored.
// Round 361: Natural-input velocity setter/direct actor test/separate impulse,
// subsets 1-7 and all with relative setter: 216/241/229/229/217/241/229/262.
// Every corrected form loses matched callers through the input convention; original
// body restored. An unused-address preparation 198 is excluded, not a gain.
// M313: Natural input/direct relative-query output with CRT copy/direct success/short contact subsets; callers lose 61/14; restored;
// fast 239/239/253/253/238/238/254/254.
// Round 535: natural parameter with complementary actor-rejection result, bool/byte/short/long
// storage and original/short contact: 232/232/235/233/230/230/235/231. Original body restored;
// frame/state-result lifetime and input convention remain.
// Complementary bool/byte results recover frame 0x28 at 232, but lose the two
// matched callers at 61/14 through the natural register input; restored.
// Round 537: natural input with complementary support result bool/byte/short/long,
// entry success 229/229/217/219; query-bound success 225/225/213/215. Original complete body
// restored; native stack input, state/result lifetime and 0x28 frame remain unresolved.
// NOT kept: natural-input forms lose matched callers through the input convention.
// M317: Natural input/existing velocity-wake helper; complete jump/up/square parts 1/2/4/3/5/7; all with
// long/short contact truth: 317/228/228/228/228/228/228/231/228.
// State-base/result lifetime, frame 0x24/0x28 and input convention remain. Original executable body restored.
// F340: Natural-input last-consumer scopes for definition/raw/relative, seven subsets,
// then all with admitted success: 228/228/228/228/228/228/228/224. Frame 0x28 recovers, but
// length is 626/683 and exact callers lose 61/14; original complete body restored.
// Lane 1 round 62, kept (210 -> 68 at 683 bytes): retail's name test is the argument of an unused third parameter of
// 0x1f5560 (LTCG drops the parameter and keeps the argument's stores, retail's dead [esp+0x3c] store) with the state
// base at entry 162, which also removes the parameter-address read (the natural parameter, callers still match);
// `result = true` just before the actor test 145; dot3f(&relative, &biped->up) 68. Tried: 0x1f5560 taking &result 210
// (compiled away); state before definition 162; long riding 174; no name local (two reads) 162; `result = true`
// after 0x109e00 143 (slot and store as retail, but riding takes BL and the state spills), ahead of it 143, ahead of
// 0xba1d0 133 (686 bytes), at its declaration 160; on the after-0x109e00 form two reads, declaration order and riding
// at its assignment 143 each; dot3f(&biped->up, &relative) 144; written orders 145; vector3f_add_scaled kick 144;
// velocity/relative copies swapped 145; ba1d0 into relative 154; relative from raw 161.
// Review: the state base stays a local at entry: retail loads it there and keeps it in EBX across the calls (a load
// the compiler cannot move above calls), and inlined at the use it gives 167 and loses matched 0xe4680 (61) and 0xecfc0 (14).
// M340: Positive support ownership const bool/const byte/test/conditional, original/query-bound success: 68/143/68/143/146/182/146/182. NOT kept: no exact result/slot graph; original body restored.
// Lane 18 round 34 (fast): riding renamed to sort after result 68; result renamed to sort first 68; `result = true`
// right after 0x109e00 on the dot3f body 143 (686 bytes); result declared before riding 68; no result local, `return
// true` 151 (672 bytes); riding declared at its assignment 68. The result/riding slot swap (result in the argument slot) remains.
// Y553: Explicit state criterion with success before/after support or velocity query
// 143/143/133/133; complementary support with late/query-bound success and branched
// criterion 156/155/155; sole actual result exit 92. All preserve both exact callers.
// NOT kept: result/support lifetime forms regress; original complete body restored.
// P290: Tested actor-result return 68; explicit state criterion in result and branched criterion
// 62/62; one actual result exit 158; byte/short support 68/148; long result 153;
// actual admission result 83. NOT kept: six-point criterion gain is below eight; all
// executable forms restored. Native support/result byte homes and early success store remain.
// N346: Independent state-criterion argument in bool/byte/short/long, mutable/const,
// 144/144/144/144/146/146/146/146. NOT kept: one-use argument producers remain unmatched;
// original support/result lifetimes and complete body restored. Native byte homes remain.
// Y557: State criterion assigned to the returned result within the existing
// call argument, with success at actor/support/velocity/entry boundaries:
// OR criterion 62/143/133/160; conditional criterion 141/145/135/159.
// NOT kept: the six-point best is below eight and an embedded assignment
// requires byte equality. Original result/support lifetimes and full body restored.
// N350: Separate positive actor authorization in bool/const bool/byte/const byte,
// original and support-bound success: 155 each at 668/683 bytes. NOT kept: result
// lifetime loses native extent and byte homes; both exact callers survive; body restored.
// B395: Byte/signed-byte success accumulators, actor/support/velocity/entry producers:
// 88/150/140/149 in either domain. NOT kept: all regress; canonical return conversion
// and native support/result homes remain. Original Boolean result and complete body restored.
// Native state/offset/jump field-domain subsets 1/2/4/3/5/6/7:
// 68/68/68/68/68/68/68; complete graph with native early success 143.
// All native record views and original result timing restored; support/result
// byte homes and early success remain unresolved.
// Y572: Joint original handle/support/result lexical owners, eight complete
// behavioral-name triples: 68 each. NOT kept: support/result byte homes and
// early success store remain; original complete body and parameter name restored.
// P316: Plain relative/kick/actor-ground coordinate loops on the recovered state-call
// graph, masks 1/2/4/3/5/6/7 then all with support-bound success:
// 143/144/144/236/143/70/233/211. NOT kept: no gain; final early-success form
// also loses both exact callers. Original full body restored; support/result homes remain.
// P318: Positive support in signed byte/unsigned short/long/unsigned long on the
// recovered state-call/dot graph, original then support-bound success:
// 68/148/151/151/143/136/142/142. NOT kept: no byte match; native support/result
// homes remain. Original complete body restored; both exact callers survive.
// F373: Ground/raw/velocity/relative declarations at their actual producers,
// individual, paired and complete lifetimes, then support at its producer:
// 68/68/68/68/68/68/68/68. NOT kept: byte homes and early success remain;
// original body restored, both exact callers survive.
// F375: Existing relative/kick/actor-ground producers, seven subsets then complete raw library copies:
// 143/68/68/68/143/70/143/143 on the state-call/dot graph. NOT kept: no byte match.
// Original body restored; native support/result byte homes remain, exact callers survive.
// P321: Query directly into velocity/relative, record/CRT copy, actor/query-bound
// success on the recovered state-call/dot graph: 150/150/148/148/136/136/135/135.
// NOT kept: all regress and shorten to 671/683; full body restored.
// AC538: Complete upward-kick admission, positive/complementary bool/byte/short/long:
// 144/144/144/144/144/144/144/144. NOT kept: unmatched decision/storage forms; original complete body
// restored.
// Native support/result byte homes and early success store remain.
// F390: Support query through the existing result before its riding copy, bool/byte/short/long,
// original then support-bound success: 68/68/148/151/143/143/136/142. NOT kept: no gain;
// the early Boolean graphs grow to 686/683. Original complete body restored, both exact callers
// survive; native support/success byte homes and early success publication remain.
// P339: Structured actor-success restoration/completion tail with original/native
// success timing, bool/byte/word/long result: 68/88/154/153/92/95/147/146.
// NOT kept: no byte match; original body restored. Support/result byte homes and
// early success store remain; both exact callers and providers survive.
// N380: Complementary state criterion bool/byte/short/long, original/support-bound
// success: 144/144/146/146/160/160/167/167. NOT kept: no equality; original state observation,
// call criterion and complete body restored. Support/result byte homes and
// native early success remain.
// @retail 0xe4770
bool __stdcall function_e4770(long arg_159e6d)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	s_biped_definition *definition = BIPED_DEFINITION(biped);
	byte *state = (byte *)biped + *(short *)((byte *)biped + 0x12a);
	real jump;
	vector3f ground;
	vector3f raw;
	vector3f velocity;
	vector3f relative;
	bool riding;
	bool result = false;
	real along;

	if (function_e4050(arg_159e6d) || biped->unknown34c == 1)
	{
		goto done;
	}
	jump = definition->biped.jump_velocity;
	riding = function_109e00(arg_159e6d, &ground, false);
	result = true;
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
	along = dot3f(&relative, &biped->up);
	if (jump > along)
	{
		real kick = jump - along;

		velocity.i += biped->up.i * kick;
		velocity.j += biped->up.j * kick;
		velocity.k += biped->up.k * kick;
	}
	if (biped->actor_index != NONE)
	{
		long name = *(long *)(state + 0x7c);

		result = function_1f5560(biped->actor_index, &velocity, name == 0xa00003e || name == 0xd00003f);
		if (!result)
		{
			goto done;
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
done:
	return result;
}

void function_e5790(long arg_159e6d);
extern real g_47f05c;

/* lands or is knocked to the ground: a crouching biped standing up into a
   ceiling is lifted clear, the hit is remembered (+0x44c..), and a flag
   plays the knocked-down animation */
// Matches with the handle first, the order Claude lane 17 found in round 5: retail takes the point in EAX and the
// handle in ECX, and its callers 0xcaa60 and 0x9c610 set them up that way. The contact comes before the point
// (Claude lane 16 round 99): this function matches either way, and 0x9c610's call matches only with this order.
// The lift cap is read from the global g_47f05c (retail loads it from .data and doubles it at run time), and the
// upward sum is k-first (retail's j*j, then k*k, then i).
// @retail 0xe4a20
void __stdcall function_e4a20(long arg_159e6d, long object_index, point3f const *point, bool knocked)
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
				s_biped_definition *definition = BIPED_DEFINITION(biped);
				real room = definition->biped.physics.height_crouching - definition->biped.physics.radius * 2.0f;

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
#include "impacts.h"
void function_188180(point3f const *point, vector3f const *forward, long tag_index, long object_index, long index,
	long variant, c_type_47f957 material, long effect_value, s_location const *location, real scale);

/* a walking biped's footstep effect: at its foot marker (or its origin),
   with the material it stands on */
// MiniMax H (11-14), NOT kept (coordinator: a 1-point gain on a function still above 30 is under the keep bar; code
// back to main, 80): score 79 (was 80): the probe offset now reads the global g_47f05c first, as retail does (the constant loaded first, then the point added); `short zone = -1` compiles to the same bytes, and `word zone` does not fit the callee's `short *`.
// MiniMax B (11-13): no new form, no lower score (80): retail keeps the three incoming stack arguments in ecx, edx and eax where this build takes them in eax, ecx and edx, retail reads the flag byte at ebp+0xc0 into cl where this build uses al, and retail stores the NONE zone as a word where this build reuses the dword esi; the argument slots belong to the caller, so no source form reaches them.
// MiniMax H (11-12): no new form, no lower score (138): the foot marker in its own block measures 146 and the point copied componentwise on top of that measures 138, the same code, so the body stands.
// MiniMax G (11-12): no new form, no lower score (138): nothing new this round; the frame 0xec against retail's 0x94 (the marker
// and the collision do not share a home) and the remaining homes differ.
// MiniMax G (11-12): no new form, no lower score (138): frame 0xec against retail's 0x94 (the marker and the
// collision do not share a home), the material carrier and the remaining homes differ.
// MiniMax B (11-12): no new form, no lower score (168): the frame is 0xec against retail's 0x94 and the entry argument
// sits at [esp+0xf0] against retail's [esp+0x98], so this build's locals take more room than retail's; the 0x188180
// call's convention is set by the callee's definition in src/unknown_187ec0.cpp, which this lane may not edit.
// MiniMax S (11-11): no new form, no lower score (168): the frame is still 0xec against retail's 0x94 because the foot
// marker and the collision record do not share a home, and the material is still carried as a dword where retail keeps a
// 16-bit value; the effect helper's parameter convention (src/unknown_187ec0.cpp) is not this lane's:
// BLOCKED-BY 0x188180.
// score 80 (was 138; earlier 168, 154): the frame is retail's 0x94 now (the foot marker in its own block, the down vector copied
// after the point offset through vector3f_assign, and the count test as count - 1 >= 0); what is left is register choice — the
// contact point load lands in edx against retail's eax, the marker position copy rotates eax/ecx/edx, the flag byte test uses al
// against retail's cl, the material word comes from cx against retail's dx, the zone is stored as a dword against retail's
// word, and the down vector's three components are loaded in reverse and stored after the argument pushes.
// MiniMax A (11-13): no new form, no lower score (80): the point offset through the shared global g_47f05c (retail's operand
// order), the zone typed as c_type_47f957 (retail's word store) and the count test as PIN(0, 0, count - 1) == 0 (retail's
// lea/test/jge/jne) together score 80, the same as the baseline, so all three are reverted; the global alone scores 79 and the
// typed zone alone 125 (its one byte less shifts the loop's padding).
// Earlier first line: at 168 (from 154): frame 0xec versus 0x94 (marker and collision don't share a home), material carrier
// (BLOCKED-BY 0xceee0 since round 85, see the end of this note; earlier BLOCKED-BY 0x188180).
// score 168 (was 168; earlier 154): frame 0xec versus 0x94 (marker and collision don't share a home), material carrier
// (BLOCKED-BY 0xceee0 since round 85, see the end of this note; earlier BLOCKED-BY 0x188180). Earlier first line: at 168
// (from 154): frame 0xec versus 0x94 (marker and collision don't share a home), material carrier.
// MiniMax E (11-08): no new form, no lower score (154); 0x188180 (src/unknown_187ec0.cpp, not this lane's file) is still unmatched, so its argument order and the material homes stay as they are.
// MiniMax D (11-06, run 49): no new form, no lower score (154); 0x188180 (src/unknown_187ec0.cpp, not this lane's file) is still unmatched, so its argument order and the material homes stay as they are.
// MiniMax A (11-04): the material as a four-byte carrier scores 151, under the retention margin and not kept;
// the material class still needs 0x188180's own file.
// MiniMax A (11-03): no new form, no lower score: 0x188180 (src/unknown_187ec0.cpp, not my file) is still
// unmatched, so its argument order and the material homes stay as they are.
// MiniMax R (11-01): no new form, no lower score: 0x188180 is still unmatched, so its argument order and the material homes stay as they are.
// Earlier 154 (was 168): frame 0xec versus 0x94; material homes, model-count test and effect-call transport remain.
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
// Round 214 (m3 lane 3, fast; masked aligned in brackets, base 45), nothing kept: 0x188180's material as c_type_47f957 by value at its definition, every
// declaration and call 153 [31] at 669 bytes, but 0x188180 77 -> 95, 0xceee0 94 -> 103, 0xec4f0 45 -> 49 (0x10c880 65, 0x247070 15). The same with
// 0x188180 passing the dword on through a cast (diagnostic) 152 [31], 0x188180 77, 0xceee0 101, 0xec4f0 49; marker scope 153 [30] at 664; `down`
// written after the point offset 136 [25] at 686 = retail's length; both 147 [23]. BLOCKED-BY 0x1883e0's long fifth parameter, which the matched
// 0xfd740, 0xfd7d0 and 0x508d0 (contributor) feed from long values. On the long form: the count test as `PIN(0, 0, count - 1) == 0` 174 [41] at 657
// (retail's `lea eax, [esi - 1]; test; jge; jne`, a raise, so restored); `0.0328f + probe.z` 154 (same code); both 174; with marker scope 180 [40].
// M3 lane 13 r74 (fast with 0xe4450/0xe45e0/0xe4c50/0xe5040): the zone declared before the material 154, the foot
// marker before the point 154, the scan counter declared in its `for` 154 (same code); the location pointer with
// the material and zone 158 (671 bytes); the zone declared just before its query 195. Retail stores the zone as a
// word as well (ours a dword).
// M3 lane 2 r73 (plain and fast both 168 at 664 bytes; 154 was not reproduced, lane 4 r111 already saw 168; masked = aligned
// count with registers and esp offsets masked, 49): the 0.1 point offset through vector3f_add_scaled and the flags_c0 bit 6
// test through the file's field_bit_test 168 each (same code, kept: existing helpers for their bodies); the material as a
// long carrier 168 (663 B, masked 41; a width picked by score, not kept). No other untried natural form: the rest waits on
// 0x188180's material parameter as c_type_47f957, whose declaration and call in src/unknown_0a76b0.cpp belong to another
// lane this round.
// M3 lane 9 r110 (fast, control 168 = the plain baseline): the physics-mode/effect test as an early return 168 (same code); the
// point offset through the existing vector3f_add_scaled 168 (same code); the material read after the probe 168 (its load moves
// past the call); the model count tested as `if (count)` 169. Diagnostic, not kept (only this file's declaration changed, so the
// reduced image leaves the call unresolved): the material as c_type_47f957 163, with the marker in its own block and `down`
// componentwise after the offset 170. Lane 5 r169's measured patch (that parameter as c_type_47f957 at the definition, every
// declaration and call) gave 168 -> 163 plain but needs src/unit_object_type.cpp's declaration and raises 0xceee0 90 -> 97 there,
// so BLOCKED-BY 0x188180 (and 0xceee0's own material parameter).
// M3 lane 13 r85 (plain where marked, else fast; 0x1883e0 now takes the material type, so its blocker above is stale). NEEDS A
// RULING, not kept (diff in build/r85/held-188180.patch): 0x188180's seventh parameter as the material type (c_type_47f957) by
// value at its definition (passed straight on to 0x1883e0), all five declarations and the five calls, with the item impact's
// +0x24 field declared as that type in src/items.cpp and impacts.h included where the type was missing: plain 168 -> 138 at
// 686 bytes (retail's length), 0x10c880 69 -> 39, 0xec4f0 23 -> 8, 0x247070 30 -> 6, 0x188180 165 and 0xcf3d0 493 unchanged,
// 0xcf040, 0x1883e0 and this function's callers still match; but 0xceee0 90 -> 97 (it passes its own `short` material to the
// call; retail reads that parameter's slot as a dword, so its parameter is the same type, and that needs the call line in the
// contributor's matched 0xcf040; with it retyped and both of its calls adjusted, fast, 0xceee0 114). So BLOCKED-BY 0xceee0.
// Without the item field typed (a constructed value at 0x10c880's call) 0x10c880 82 (fast). Here the material local as that
// type 138 (fast), passed as a constructed value 163 (fast). On the 138 form, fast, all match-only (build/r85/
// e4d90-extras-on-held.patch): the zone as that type, default-constructed (retail's word store of NONE), read through
// `&zone.m_index` 138 (same count; its slot and the material's stay swapped against retail's, and renaming it to sort before
// `material` moves nothing); declared before the material 138; the marker in its own block 145 (669 bytes; frame 0x94 as
// retail); `down` copied after the point offset 136; all four 136; with `down` through vector3f_assign after the offset (retail's
// movss copy) 119, with the count test as `PIN(0, 0, count - 1) == 0` (the in-range idiom 0xdd990 uses; retail's lea/test/jge/jne)
// 88, both 81 at 686 bytes. Left at 81: eax/ecx/edx rotations at the marker index, the point copies, the flag test, the material
// word, both 0x188180 call setups and the model lookup; the probe offset's constant loaded first in retail (`0.0328f + probe.z`
// 136, same code); the `down` loads interleaved with the pushes in retail.
// @retail 0xe4d90
void __stdcall function_e4d90(long arg_159e6d, long type, real scale, long marker)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	s_biped_definition *definition = BIPED_DEFINITION(biped);

	if (biped->physics_mode == 1 && definition->object.footstep_effects.tag_index != NONE)
	{
		point3f point;

		{
			s_object_marker foot;

			if (marker < definition->biped.contact_points.count &&
				function_b8d30(arg_159e6d, ((long *)definition->biped.contact_points.address)[marker], &foot, 1, false))
			{
				point = foot.matrix.position;
			}
			else
			{
				function_b9dd0(arg_159e6d, &point);
			}
		}
		if (field_bit_test(biped->flags_c0, 6))
		{
			c_type_47f957 material(*(short *)((byte *)biped + 0x44c));
			short zone = NONE;
			point3f probe;

			function_b9dd0(arg_159e6d, &probe);
			probe.z += 0.0328f;
			s_location *location = (s_location *)((byte *)biped + 0x28);
			if (function_11c120(location, &probe, &zone))
			{
				material = c_type_47f957(zone);
			}
			function_188180(&point, (vector3f *)((byte *)biped + 0x450), definition->object.footstep_effects.tag_index, arg_159e6d,
				type, definition->object.impact_sound_size, material, (long)g_4687bc, location, scale);
		}
		else if (biped->parent_object_index == NONE)
		{
			byte *model = g_4e3b44[definition->object.model.tag_index & 0xffff].bytes;
			long count = *(long *)(model + 0x78);

			if (count - 1 >= 0)
			{
				vector3f down;
				s_biped_ground_collision collision;
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
				vector3f_add_scaled((vector3f const *)&point, g_4687b0, 0.1f, (vector3f *)&point);
				vector3f_assign(&down, g_4687bc->i, g_4687bc->j, g_4687bc->k);
				if (function_1697c0(0x84000d, &point, &down, NONE, NONE, (s_collision_result_1697c0 *)&collision))
				{
					function_188180(&point, g_4687b0, definition->object.footstep_effects.tag_index, arg_159e6d, type,
						definition->object.impact_sound_size, c_type_47f957(collision.unknown24), (long)g_4687bc, (s_location *)((byte *)biped + 0x28), scale);
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
// MiniMax R (11-09): no new form, no lower score (594 in the reduced build); the definition pointer and the state pointer take the next stack slot round from retail's, and the body is still 87 bytes longer than retail's.
// MiniMax R (11-06): no new form, no lower score (594 in the reduced build); the frame agrees and the tag, state and animation homes of the note below are unchanged.
// score 579 (was 588): round 101 (m3 lane 12) notes are at the end of this note. Earlier at 588 (was 648): frame 0xd0 agrees; tag/state/animation homes, byte/result residency
// and completion-call expansion remain.
// Earlier: the logic was walked against retail block by block and agrees (callees, argument order,
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
// Round 192 (fast control 655): retail stores `still` as a stack byte, 0 before the 0xddfb0 call and 1 inside its
// branch ([esp+0x13], later shared with `grabbing`, filled as 0xe0dc0's false-then-true body), places the early
// `return still` after the final return, and keeps +0x3aa in BL across the 0xe0dc0 call. Tried: that `still` form 686;
// the +0x10a test through the bit view 674; both 694 (the head is then retail's up to the early return); the count read
// once into a `char` local with `0 >= x`/`x <= 0`/`+=` forms 655 each (identical code: ours re-reads after the call);
// the early return as one goto to an exit after the final return 655, with the `still` form 686, with both 694.
// Round 259: Direct animation-result initialization after the names store, complete shared orientation
// setters and state-before-tag acquisition in seven subsets 681/678/720/655/681/678/720;
// all three with shared squared length 720 (unchanged reduced control 655). Original body restored.
// P271: Complete orientation memcpy at biped-forward/output-forward/up, subsets
// 1/2/3/4/5/6/7, then complete memmove: 655/655/655/655/655/655/655/628.
// Complete memmove 621 plain at 2264/2190 bytes NOT kept: it adds three out-of-line CRT copy
// calls retail does not make (a score-picked copy form). Struct assignments restored at 648.
// M311: Actual animation-result/tag/state lexical-home subsets 1-7, all with late uninitialized motion:
// 655/655/655/655/655/655/655/655 (fast control 655); restored.
// M313: Uniform motion flags/placement mode/entry animation output subsets, all plus entry matrix declaration; restored;
// fast 682/658/685/655/682/658/685/685.
// DH299: Branch-produced grabbing/transition comparison/merged guard, seven subsets
// 714/648/655/640/714/648/640; all with native byte decisions 640 (fast control 655).
// 640 forms need an exact match for the branch-set flag; seven-point eligible forms fail the keep margin.
// Original body restored; frame 0xd0 and animation/state/tag homes plus completion expansion remain.
// DH301: Complete repeated tag flags/standing-height/crouched-height ownership, subsets
// 1/2/4/3/5/6/7, then all with read-only tag receiver: 729/770/741/773/744/780/775/775
// (fast control 655). All regress; complete original body restored. Frame 0xd0 agrees;
// animation/state/tag homes and completion expansion remain.
// F341: Existing complete setters for every velocity copy, relative velocity
// and integrated point, seven subsets then all with complete workspace roles:
// 653/655/655/653/653/655/653/653 versus fast control 655. Velocity-copy forms
// lengthen to 2297; two-point gains rejected. All restored; full score stays 648.
// Y539: One completion call after the complete mode dispatch, with test-produced
// admitted/blocked eligibility in bool/byte/word/long: fast 655 each against control
// 655 (full 648). Compiler completion cloning and frame/homes remain; original
// switch, repeated calls and complete body restored. Every checked peer is unchanged.
// M322: Native velocity-difference/position-integration axis loops and complete
// lean-result ownership, subsets 1/2/4/3/5/6/7, then consistent timestep:
// fast 684/667/646/676/675/658/667/656 against control 655.
// NOT kept: lean-local 646 hoists the field read and merges the native
// step/clamp field stores to one final store. Original body restored at full 648.
// M340: Native orientation coordinate copies/returned-rotation consumption/scaled integration 1/2/4/3/5/6/7, all with local-before-matrix: 1601/652/653/1502/1590/649/1497/1497. NOT kept: rotation/integration gains 3/2, both 6, below gate; coordinate copies enlarge the frame; original body restored.
// Y551: Late resting result as false/success, call-result/push, true/failure and
// test-owned arms, original/existing squared length: 665/649/665/652 twice
// versus fast 655. NOT kept: flag-set forms require a match; six-point best
// also fails the margin. Original body restored; frame/homes/call expansion remain.
// P291: Actual early skip/apply movement decision separated from the still result,
// bool/byte/short/long in both truth domains: fast 655 each against control 655,
// 2285/2190 bytes. Original body restored at full 648; frame 0xd0, tag/state/animation
// homes and completion expansion remain.
// F356: Actual persistent low-byte/high-byte/word-bitfield views, seven subsets, then all with read-only tag receiver:
// 663/655/655/663/663/655/663/663. NOT kept: no qualifying match or gain; original complete body restored.
// AC504: Constant-default animation-result construction at entry/pre-motion/post-motion/
// pre-timer scopes, original then animation-flags-first acquisitions: 655 each
// against reduced control 655 (full 648). Original complete body restored; tag/state/
// animation homes and completion expansion remain.
// M349: Producer-owned still/grabbing/change declarations, seven subsets, then
// admitted motion declaration: eight fast 655s against control 655 (full 648).
// NOT kept: no byte match for declaration allocation. Original body restored;
// tag/state/animation homes and completion expansion remain.
// DH324: Native flag-word/timer-array/output forward-push fields, seven subsets,
// then direct native push reads: fast 655/655/655/655/655/655/655/655 (control 655, full 648).
// Original actual record declaration, field accesses and complete body restored.
// P320: Native flag-member entry/motion/high stores, combinations and high reads:
// P320: fast 594/655/666/594/602/662/599/599 (control 655, opening full 648). Keep entry bit 4 only:
// P320: 588 plain at 2271/2190 bytes; native direct byte mask and +0x5e prefix
// P320: agree. No other measurement changes. Other forms restored; tag/state/
// P320: animation homes, byte results and completion-call expansion remain.
// M362: Existing grabbing predicate helper with bool/byte/char/short/word/int/long/dword
// result domains 715/715/715/710/710/706/706/706 (fast control 655, full 648).
// NOT kept: helper-result storage lengthens the body and regresses; native homes
// and completion expansion remain. Original predicate and complete body restored.
// M364: Original multi-use still call result in char/signed-
// char/byte/short/word/int/long/dword storage: 683/683/683/688/688/685/685/685. NOT kept: no
// native truth-byte/animation-home match; original result domain and complete body restored.
// F374: Actual inline grabbing char/signed-char/byte/short/word/int/long/dword
// carriers: 594/594/594/581/581/578/578/578 (fast control 594, full 588).
// NOT kept: wider carriers retain the result in EDI/DI rather than retail
// byte storage; original bool, predicate and complete body restored.
// DH350: Common returned result, call-owned/test-owned bool/byte/short/long
// on the native entry-bit graph: 594/645/608/604/611/602/623/624 (fast control
// 594, full 588). NOT kept: no match or gain; original early/final returns restored.
// DH354: Native branch-produced early still, original direct exits and entry-bit graph;
// bool/char/signed-char/byte/short/word/int/long: 660/667/667/667/674/674/677/677. NOT kept: no match.
// Body restored; truth-byte, tag/state/animation homes and completion expansion remain.
// P364: Existing flag-test producer at entry/tag/crouch/grab/movement/body/sleep/complete
// consumer boundaries: 594/594/594/594/594/594/594/594. NOT kept: no native frame/home repair or exact
// equality; fast control 594, full 588. Original whole body and shared helper restored.
// P366: Completion provider inline at caller/definition/both 594/594/594; complete four-branch step producers
// mutable/const 594/594, branch-local updated values mutable/const 594/594, both 594. Fast control 594, full
// 588. NOT kept: all eight are byte-neutral in the reduced image; every shared user retains its calibrated
// score. Original target and provider metadata restored; native truth byte, scalar homes and completion
// expansion remain.
// P368: Existing animation lookup/touch inline declaration/definition/both contracts and combined
// header/all graphs: eight fast 594s (control 594, full 588). Direct result initialization
// after the name store, mutable/const on this entry graph: 651/651, 2301/2190 bytes. NOT kept:
// neutral contracts or regressing result transfer; original target, header and providers restored.
// P371: Complete complementary still-query result, bool/char/signed-char/byte/short/word/long/dword:
// 685/685/685/685/685/685/677/677; fast control 594, full 588. NOT kept: no native truth-byte/home recovery;
// all regress; original result, early/final returns and complete body restored.
// P375: Complete retained forward/up/physics-mode field addresses, masks 1..7, then complete entry
// address lifetimes: 603/640/645/594/603/640/645/627. NOT kept: no native home/producer match; all original direct
// field observations, copies, helper boundaries and complete body restored.
// P379: Fast control 594/full 588; idle-tick admission bool/byte/short/long, original timer pointer then
// complete declared timer-byte receivers: 707/707/707/707/707/707/707/707. NOT kept: no native
// truth-byte/tag/state/animation-home equality; one-use admissions require equality; original complete body
// restored.
// T71: Native-entry tag/state/animation lexical-home subsets and complete motion roles: eight
// 594s (unchanged reduced control 594, full 588). NOT kept: native home/truth-byte repair
// absent; original names and whole body restored.
// P389: Complete standing/request/release crouch truth producers in uniform
// byte/char/signed-char/short/word/int/long/dword at their original observations:
// 594/594/594/586/586/611/611/611. NOT kept: no exact native frame/home recovery;
// wider one-use truths need equality; native byte tests/homes remain. Body restored.
// Round 101 (m3 lane 12; fast control 594, plain where marked). Kept: the two high flag stores beside the settle count through
// the existing flags_348_bits members (retail's two separate `or byte ptr [ebp + 0x349]`) 586 fast, 579 plain (was 588).
// BLOCKED-BY 0xb7430: retail's unparented position update (0xe2bbc-0xe2c41: wake, bounds test, position copy, 0xb58c0,
// `push 0; push 0; call 0x1c4a80`, 0xb8600) is the inlined body of 0xb7430(index, &position, NULL, NULL, NULL, false, true,
// false, false), inside which 0x1c4a80 stays out of line; our hand-written copy inlines 0x1c4a80 (22 more instructions), which
// is most of the 81 extra bytes. 0xb7430's body is disabled in src/unknown_0b7430.cpp (it closes a call cycle through 0xb75a0).
// DIAGNOSTIC, restored: with that body enabled and called here, 2202 bytes against retail's 2190 and 517 fast (control 611 in
// that build; 0xb7430 109 and 0xb75a0 73 there).
// Behaviour difference found, NOT changed (it raises the count): retail reads names[1] once into EBX at 0xe266e, tests it
// against 0x400000c, and passes that old value to 0xdd300 at 0xe26fb even after the idle block has stored the state's name
// into names[1] (0xe26c9); ours re-reads names[1] for the 0xdd300 call. A `long name = names[1];` local for both uses 617 fast
// (2279 bytes, aligned 305 against 308), with the bitfield stores 609.
// Singly (fast): the flags_10a test through the file's s_biped_flags_10a_view bit (retail's `shr al, 2; test al, 1`) 646;
// `output forward = biped->forward = move.forward` chained 594 (same code); the animation id initialised at its declaration
// 651 (2301 bytes); the settle byte in a char local across the 0xe0dc0 call (retail keeps it in BL) 594 (same code); a long
// copy of the physics mode for the 4/5 test (retail's movzx and dword compares) 589, a mix with the direct reads, not kept;
// `still = false` with `still = true` under the query (retail's stores at 0xe253b and 0xe2551) 660. Combined with the name
// local and the bitfield stores: settle local 609, chained copy + bit view 632, mode copy 608.
// @retail 0xe24f0
bool __stdcall function_e24f0(long arg_159e6d, long *names, s_biped_physics_output *output)
{
	s_biped *biped = BIPED_GET(arg_159e6d);
	s_biped_definition *definition = BIPED_DEFINITION(biped);
	s_animation_state *state = (s_animation_state *)((byte *)biped + *(short *)((byte *)biped + 0x12a));
	word const *animation_flags = (word const *)((byte *)biped + biped->unknown33e);
	s_biped_physics_move move;
	bool still;
	bool grabbing;
	real change;

	biped->flags_348_bits.field_4 = false;
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
	function_1e5af0(&biped->physics_mode, &biped->forward, &biped->up, output);
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
		if (TEST_FIELD_BIT((definition->biped.physics.flags >> 2) & 1))
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
		long ticks = (long)((0.0f - (definition->biped.physics.height_standing - definition->biped.physics.height_crouching)) /
			(g_510c54->rate * 0.15f) + 0.5f);

		function_1c3770(arg_159e6d, 0);
		biped->flags_348_bits.field_d = true;
		biped->unknown3aa = (byte)ticks;
		biped->flags_348_bits.field_e = true;
	}
	change = function_dd7d0(arg_159e6d);
	((byte *)names)[9] = TEST_FIELD_BIT((biped->flags_134 >> 23) & 1);
	if (!(TEST_FIELD_BIT((definition->biped.physics.flags >> 1) & 1)) && (function_e4050(arg_159e6d) || grabbing))
	{
		move.velocity.k += (definition->biped.physics.height_standing - definition->biped.physics.height_crouching) * g_510c54->field_2_3 *
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
