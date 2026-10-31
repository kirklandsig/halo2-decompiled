// @flags /O2 /Ob1 /arch:SSE /Gr
/* PROJECTILES.CPP: projectiles

The projectile object type's callbacks (its
definition at 0x467f28), and its flight, collision, attachment and
detonation. */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_0259d0.h"
#include "object_markers.h"
#include "normalize3d.h"
#include "vector_products.h"
#include "object_iterator.h"
#include "effects.h"
#include "sound_sources.h"
#include <math.h>
#include <string.h>

#ifndef NUMBEROF
#define NUMBEROF(array) (sizeof(array) / sizeof((array)[0]))
#endif

#ifndef PIN
#define PIN(n,floor,ceiling) ((n)<(floor) ? (floor) : ((n)>(ceiling)?(ceiling):(n)))
#endif

/* a projectile's target: an object, and the model node aimed at */
struct s_projectile_target
{
	long object_index;
	short node_index;
	byte unknown06[2];
};

/* the projectile object (the object fields, then the projectile's own from
   +0x12c) */
struct s_projectile
{
	long tag_index;
	struct
	{
		dword unknown0 : 1;
		dword unknown1 : 2;
		dword unknown3 : 1;
		dword : 28;
	} object_flags;
	byte unknown08[0x14 - 0x8];
	long parent_object_index;
	byte unknown18[0x64 - 0x18];
	point3f position;
	vector3f forward;
	vector3f up;
	vector3f linear_velocity;
	vector3f angular_velocity;
	byte unknowna0[0xc1 - 0xa0];
	byte unknownc1;
	byte unknownc2[0xc4 - 0xc2];
	long unknownc4;
	byte unknownc8[0xd4 - 0xc8];
	long unknownd4;
	byte unknownd8[0x11c - 0xd8];
	short attachment_states_size;
	short attachment_states_offset;
	byte unknown120[0x12c - 0x120];
	struct
	{
		dword rotating : 1;
		dword unknown1 : 1;
		dword unknown2 : 1;
		dword unknown3 : 1;
		dword unknown4 : 1;
		dword unknown5 : 1;
		dword unknown6 : 1;
		dword unknown7 : 1;
		dword unknown8 : 1;
		dword unknown9 : 1;
		dword unknown10 : 1;
		dword unknown11 : 1;
		dword unknown12 : 1;
		dword unknown13 : 1;
		dword unknown14 : 1;
		dword unknown15 : 1;
		dword : 16;
	} flags;
	short action;
	byte unknown132[0x140 - 0x132];
	long ignore_object_index;
	s_projectile_target target;
	long attachment_index;
	byte unknown150[0x158 - 0x150];
	real unknown158;
	real unknown15c;
	real unknown160;
	real unknown164;
	real distance_traveled;
	real unknown16c;
	real speed;
	vector3f rotation_axis;
	real rotation_sine;
	real rotation_cosine;
	real unknown188;
	real unknown18c;
	real unknown190;
	real unknown194;
	byte unknown198[0x1aa - 0x198];
	short unknown1aa;
};

/* an object's attachment state (8 bytes) */
struct s_projectile_attachment_state
{
	byte type;
	byte unknown01[3];
	long value;
};

/* the projectile definition tag (proj) fields read here */
struct s_projectile_definition
{
	byte unknown00[0xbc];
	union
	{
		dword flags;
		struct
		{
			dword unknown0 : 1;
			dword gravity : 1;
			dword unknown2 : 1;
			dword unknown3 : 1;
			dword unknown4 : 1;
			dword drifts : 1;
			dword unknown6 : 1;
			dword unknown7 : 1;
			dword unknown8 : 1;
			dword unknown9 : 1;
			dword difficulty_scaled : 1;
			dword : 21;
		} flag_bits;
	};
	short unknownc0;
	word unknownc2;
	byte unknownc4[0xd8 - 0xc4];
	real unknownd8;
	real unknowndc;
	real maximum_range;
	byte unknowne4[0x13c - 0xe4];
	long unknown13c;
	byte unknown140[0x164 - 0x140];
	real gravity_scale;
	byte unknown168[4];
	real initial_speed;
	real unknown170;
	byte unknown174[0x178 - 0x174];
	real final_speed;
	real unknown17c;
	real unknown180;
	byte unknown184[0x18c - 0x184];
	real unknown18c;
	byte unknown190[4];
	real unknown194;
	byte unknown198[4];
	long material_response_count;
	byte *material_responses;
};

struct s_projectile_header
{
	byte unknown00[8];
	s_projectile *object;
};

#define PROJECTILE_GET(index) (((s_projectile_header *)g_4e0300->data)[(index) & 0xffff].object)
#define PROJECTILE_DEFINITION_GET(tag) ((s_projectile_definition *)g_4e3b44[(tag) & 0xffff].bytes)

struct s_unknown_1eb550;
extern s_unknown_1eb550 *g_51e9c4;
real function_1e96a0(short column, short row);
void function_b9a90(long object_index);
void function_f8eb0(long projectile_index, vector3f *displacement);
bool function_109a00(long projectile_index, vector3f *delta, bool unknown, vector3f *velocity);

/* the difficulty scale of a projectile row (row 10) */
PRIVATE inline real projectile_difficulty_scale()
{
	return function_1e96a0(g_4e6948->state == 1 ? g_4e6948->difficulty : 1, 10);
}

/* a vector's length, and the vector made unit length (left alone when it's
   too short) */
PRIVATE inline real projectile_normalize(vector3f *v)
{
	real magnitude = (real)sqrt(v->j * v->j + v->k * v->k + v->i * v->i);

	if (!(fabs(magnitude) < 0.0001f))
	{
		v->i = (1.0f / magnitude) * v->i;
		real inverse = 1.0f / magnitude;
		v->j = v->j * inverse;
		v->k = v->k * inverse;
		return magnitude;
	}
	return 0.0f;
}

/* sets the object and node a projectile is guided to */
// @retail 0xf8db0
void projectile_set_target(long projectile_index, s_projectile_target const *target)
{
	PROJECTILE_GET(projectile_index)->target = *target;
}

// @retail 0xfa170
real function_fa170(long definition_index)
{
	return 0.0f - PROJECTILE_DEFINITION_GET(definition_index)->gravity_scale * *(real *)g_51e9c4;
}

/* forgets a target that is going away */
// @retail 0xfa9f0
void __stdcall projectile_clear_target(long projectile_index, long object_index)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);

	if (projectile->target.object_index == object_index)
	{
		projectile->target.object_index = NONE;
		projectile->target.node_index = NONE;
	}
}

// @retail 0xfaa30
void function_faa30(long projectile_index, short action)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);

	if (action > projectile->action)
		projectile->action = action;
}

/* a projectile's speed: its definition's final speed once it has one */
// @retail 0xfbfd0
void function_fbfd0(long projectile_index)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);
	s_projectile_definition *definition = PROJECTILE_DEFINITION_GET(projectile->tag_index);

	projectile->speed = TEST_FIELD_BIT(projectile->object_flags.unknown3) ? definition->final_speed :
		definition->initial_speed;
}

/* the object a projectile is attached to (its attachment state of type 3) */
// @retail 0xfd410
long function_fd410(long projectile_index)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);
	long attachment_index = projectile->attachment_index;
	long result = NONE;

	if (attachment_index != NONE)
	{
		s_projectile_attachment_state *states =
			(s_projectile_attachment_state *)((byte *)projectile + projectile->attachment_states_offset);

		if (attachment_index >= 0 &&
			attachment_index < (long)(projectile->attachment_states_size / sizeof(s_projectile_attachment_state)) &&
			states[attachment_index].type == 3)
		{
			result = states[attachment_index].value;
		}
	}
	return result;
}

// @retail 0xfd460
void function_fd460(long projectile_index, long value)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);
	long attachment_index = projectile->attachment_index;

	if (attachment_index != NONE)
	{
		s_projectile_attachment_state *states =
			(s_projectile_attachment_state *)((byte *)projectile + projectile->attachment_states_offset);

		if (attachment_index >= 0 &&
			attachment_index < (long)(projectile->attachment_states_size / sizeof(s_projectile_attachment_state)))
		{
			states[attachment_index].type = 3;
			states[projectile->attachment_index].value = value;
		}
	}
}

/* the difficulty scale of a projectile's speed */
// @retail 0xfd8b0
real function_fd8b0(long definition_index, long owner_index)
{
	if (TEST_FIELD_BIT(PROJECTILE_DEFINITION_GET(definition_index)->flag_bits.difficulty_scaled))
	{
		if (owner_index == NONE)
			return projectile_difficulty_scale();
		return 1.5f;
	}
	return 1.0f;
}

// @retail 0xfa7b0
real function_fa7b0(long definition_index, real distance)
{
	s_projectile_definition *definition = PROJECTILE_DEFINITION_GET(definition_index);
	real scale = TEST_FIELD_BIT(definition->flag_bits.difficulty_scaled) ? projectile_difficulty_scale() : 1.0f;
	real speed = definition->unknown17c * scale;

	if (speed > 0.0f)
		return distance / speed;
	return 0.0f;
}

// @retail 0xfd3c0
bool __stdcall function_fd3c0(long projectile_index)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);
	dword *flags = (dword *)&projectile->flags;

	projectile->unknown160 = 1.0f;
	projectile->unknown158 = 1.0f;
	*flags &= ~8;
	function_b9a90(projectile_index);
	return true;
}

/* an iterator over the projectiles (object_list_iterator<projectile_datum>):
   the current one, then the object iterator */
struct s_projectile_iterator
{
	s_projectile *datum;
	s_type_f1af8e iterator;
};

/* the first projectile there is */
// @retail 0xfa9a0
bool __stdcall function_fa9a0(long *value)
{
	s_projectile_iterator iterator;

	function_bae80(&iterator.iterator, 0x20, 0);
	if (iterator.datum = (s_projectile *)function_baeb0(&iterator.iterator))
	{
		*value = iterator.iterator.object_index;
		return true;
	}
	return false;
}

/* the direction, distance and time to a target in a straight line */
// @retail 0xfa580
bool function_fa580(real speed, point3f const *origin, point3f const *target, vector3f *direction,
	real *distance, real *speed_out, real *time)
{
	vector3f vector;

	for (long component = 0; component < 3; ++component)
		vector.n[component] = target->n[component] - origin->n[component];

	real length = projectile_normalize(&vector);

	if (length == 0.0f)
		vector = *g_4687b0;

	real seconds = 0.0f;
	if (speed > 0.0001f)
		seconds = length / speed;
	*direction = vector;
	if (distance)
		*distance = length;
	if (speed_out)
		*speed_out = speed;
	if (time)
		*time = seconds;
	return true;
}

/* the rotation a spinning projectile makes each tick */
// score 3 (was 3): the flag OR/store crosses the angle duplicate and first
// axis load differently. Forced inline, speed/axis/angle declarations, the
// first-product order and *= no change; shared normalize 14; j+(k+i) 14;
// k+(j+i) 18; j+k+i 10; direct first reciprocal with inverse before 18,
// without inverse 40, after first 3; flag before angle 22; sine before axis 3;
// axis before flag 32; axis memcpy 3; named flags 3. Retail's order is VC's
// when some local's address escapes (the axis load then waits for the flag
// store). Axis after sine/cosine 21; axis by components 71; double angle 3;
// named sine/cosine 3; dword OR 3; helper reading its parameter through its
// address 11 (flag order right, copy-in wrong).
// Further forms: shared output setter 81; shared input setter 68; both setters 90;
// complete normalizer input snapshots 163, with output/both setters 147/125;
// all squared terms named 3; plain array-index input copy 68.
// @retail 0xfbd40
void function_fbd40(long projectile_index)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);
	vector3f axis = projectile->angular_velocity;
	real speed = projectile_normalize(&axis);

	if (speed > 0.0001f)
	{
		real angle = speed * g_510c54->rate;

		projectile->flags.rotating = true;
		projectile->rotation_axis = axis;
		projectile->rotation_sine = (real)sin(angle);
		projectile->rotation_cosine = (real)cos(angle);
	}
	else
	{
		projectile->flags.rotating = false;
		projectile->rotation_sine = 0.0f;
		projectile->rotation_cosine = 1.0f;
	}
}

/* a projectile's exported function values */
// @retail 0xfbe70
bool __stdcall function_fbe70(long projectile_index, long name, real *value, bool *active)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);
	s_projectile_definition *definition = PROJECTILE_DEFINITION_GET(projectile->tag_index);
	bool result = true;
	real output;

	switch (name)
	{
	case 0xe000562:
		output = projectile->unknown158;
		break;
	case 0x6000564:
		output = TEST_FIELD_BIT(projectile->flags.unknown1) ? 1.0f : 0.0f;
		break;
	case 0xf000563:
		if (definition->maximum_range != 0.0f)
			output = projectile->distance_traveled / definition->maximum_range;
		else
			output = 0.0f;
		break;
	case 0x12000565:
		if (TEST_FIELD_BIT(projectile->flags.unknown8))
			output = 1.0f;
		else if (definition->unknown17c == 0.0f || definition->unknown180 == 0.0f)
			output = 1.0f;
		else
			output = PIN((projectile->distance_traveled - definition->unknown18c) * definition->unknown194, 0.0f, 1.0f);
		break;
	case 0x11000566:
		output = TEST_FIELD_BIT(projectile->flags.unknown3) ? 1.0f : 0.0f;
		break;
	default:
		result = false;
		break;
	}
	if (result)
	{
		*value = output;
		*active = output > 0.0f;
	}
	return result;
}

/* the point a projectile's target is aimed at: its node's marker, or the
   object's default marker */
// @retail 0xf86f0
void function_f86f0(s_projectile_target const *target, point3f *point)
{
	long object_index = target->object_index;
	s_projectile *object = PROJECTILE_GET(object_index);
	s_object_marker marker;

	if (target->node_index == NONE)
	{
		function_b8d30(object_index, 0x40000bd, &marker, 1, false);
		*point = marker.matrix.position;
		return;
	}

	byte *model = g_4e3b44[*(long *)(g_4e3b44[object->tag_index & 0xffff].bytes + 0x38) & 0xffff].bytes;
	long marker_name = *(long *)(*(byte **)(model + 0x6c) + target->node_index * 0x1c);
	s_object_marker markers[2];

	if (function_b8d30(object_index, marker_name, markers, 2, false) > 0)
	{
		*point = markers[0].matrix.position;
		return;
	}

	function_b8d30(object_index, 0x40000bd, &marker, 1, false);
	*point = marker.matrix.position;
}

/* moves a projectile that drifts with its velocity */
// @retail 0xfa100
void function_fa100(long projectile_index)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);

	if (TEST_FIELD_BIT(PROJECTILE_DEFINITION_GET(projectile->tag_index)->flag_bits.drifts))
	{
		vector3f displacement = projectile->linear_velocity;

		function_f8eb0(projectile_index, &displacement);
	}
}

/* the projectile type's per-tick update: its movement, plus what moved it */
// score 38 (was 38): Boolean argument byte spill and final call/add schedule differ; frame and vector homes agree.
// Direct moved expression 38; implicit flag conversion 38; double negation 38;
// guarded bool/byte flag 46/47; direct velocity fields 65; vector memcpy 38; vector names 38.
// @retail 0xf8de0
bool __stdcall function_f8de0(long projectile_index)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);
	vector3f *velocity = &projectile->linear_velocity;
	vector3f displacement = *velocity;
	vector3f delta;
	bool moved;
	bool rotate;

	/* retail passes the rotate flag from a byte local's stack home */
	if ((!(projectile->unknownc1 & 1) || TEST_FIELD_BIT(projectile->flags.unknown14)) &&
		(rotate = TEST_FIELD_BIT(projectile->flags.unknown15),
			function_109a00(projectile_index, &delta, rotate, &displacement)))
	{
		moved = true;
	}
	else
	{
		moved = false;
	}
	function_f8eb0(projectile_index, &displacement);
	if (moved)
	{
		velocity->i += delta.i;
		velocity->j += delta.j;
		velocity->k += delta.k;
	}
	return true;
}

/* the response of a projectile to a material, or its parents' */
struct s_projectile_material_response
{
	word flags;
	short response;
	byte unknown04[0x10 - 0x4];
	short material_index;
	byte unknown12[2];
	short potential_response;
	word response_flags;
	real chance_fraction;
	real angle_lower_bound;
	real angle_upper_bound;
	real velocity_lower_bound;
	real velocity_upper_bound;
	byte unknown2c[0x34 - 0x2c];
	short scale_effects_by;
	byte unknown36[2];
	real angular_noise;
	real velocity_noise;
	byte unknown40[0x48 - 0x40];
	real initial_friction;
	byte unknown4c[4];
	real parallel_friction;
	real perpendicular_friction;
};

s_projectile_material_response g_547660;

// score 34 (was 34): word input, material-pointer formation and ordinal/loop entries still differ.
// Prior: Input index load width, loop entry alignment and response cursor calculation differ.
// Indexed responses 69; pointer stepped in body 69; entry result 67; direct count 75;
// direct material global 72; body range validation 69; single exit 69; entry result/indexed 67;
// native current ancestry index 69; entry result/current index 67.
// Native table-end traversal: cached count / result-gated 54/36, relational 54/36;
// per-use count 60/65/60/65; result-gated ordinal 57/55, nullable material 57/55;
// indexed producer/result-first/current-short combinations 58/56/58/56;
// direct table bound 36, with result first 34; without unused parameter reference 49/49.
// Kept direct table bound/result-first 34 (147/145 bytes), plain confirmed;
// input index width, loop entries and cursor arithmetic remain.
// Short ancestry with direct endpoint 34; positive ordinal/body cursor 49; unguarded ordinal 55;
// positive while ordinal 49; short ancestry/ordinal 48; per-use count/ordinal 66;
// multiply-used response base 34; response base/short ancestry 34. Word input and cursor/loop entries remain.
// Typed material table: cached header endpoint/count once or per-use 34/65; cached header ordinal 49/66;
// per-use header endpoint 34/71 and ordinal 69/66. Typed parent member does not repair the word input.
// Result-owned response cursor: endpoint 40, ordinal 39, relational endpoint 40;
// direct ordinal/endpoint success returns 79/67; result cursor with per-use count
// ordinal/endpoint 70/73; short ancestry with result ordinal 39. Restored.
// Ordinal reset before material lookup, ancestry/entry lifetime with cursor/indexed candidates
// 58/56/58/56; immutable current-material values with those four forms 58/56/58/56.
// Native word input and response scan entry remain; opening body restored.
// Round 6: counted response loop 56 (retail's single guard and counter, registers rotate, inner
// loop unaligned), with a count guard 47, with a pointer cursor 55; the NULL-default material
// getter shape around those 34/56/47; short walk local without/with the reference 51/34, counted
// 65/56. Compiled alone with every argument on the stack, each form (short, word or long
// parameter, walk local, read through the reference) loads the index as a dword, never retail's
// word load that keeps it off the register-argument list.
// Round 8: compiled alone, the index loads as a word only when nothing reassigns it in a loop (the
// loop-carried copy widens the entry load), so the walk is not a loop over the parameter as written;
// counted response loop 56 (alone: retail's guard/counter shape, but index EDI, material ECX and the
// response ESI where retail has ESI/EDI/EDX), NULL-default getter with it 56, while-counted 56,
// tail-recursive call on the parent 72 (made a loop, result dropped), counted without the reference
// 64, walk local read through the reference 34. Round 101: a word parameter with short-cast range
// tests 56 (146 bytes, movzx); `return` of the call on the parent when nothing matched 69 (VC makes
// a rotated loop, still a dword load); the counted loop reading the element through the index at
// each use 56 (140 bytes); the counted loop testing the definition's count at each pass 70; the
// parent read only when nothing matched 74. Screened alone (synthetic shapes too): every loop form
// over a short index, with the index read through the parameter's address or not, loads a dword;
// the read through the address gives the word load only when the value is not loop-carried
// (0x1ed70). A scan of the matched functions finds no word-loaded parameter that a loop reassigns.
// 64, walk local read through the reference 34.
// Round 10: retail's entry jmp over the loop pad follows from its 5-byte word load. A walk local
// initialised from the parameter 34 (copies coalesce), with the counted response loop 55, either
// without the reference 51/65 (the index then arrives in EDX); a pointer to the index walked from
// the parameter to each parent field 49 (word loads, but at the loop head), with the counted loop
// 47; the existing global_material_get (inline in damage.cpp) for the lookup 34 (same code).
// Round 12: a counted scan gives retail's shape but colours index/material/counter/pointer EDI/ECX/
// EDX/ESI (retail ESI/EDI/ECX/EDX) with the dword entry load: a named response base indexed 54,
// the element indexed off the definition 56, a walk local read through the reference 56, the
// material declared before the loop 56; the pointer scan with a walk local read through the
// reference 34 (same code). A read through the reference loads a word only when no loop carries it.
// Round 440: Material-pointer ancestry, live/cached table and full/short identity, endpoint/ordinal scans
// 173/150/168/140/162/131/150/126. Existing native material-value parameter endpoint/count graphs
// 40/40/40/40/21/21/21/21; the counted form reaches 145 bytes but rotates index/material/scan homes.
// That aggregate parameter is exact-only and restored. Its ordinal zero before admission/lookup,
// direct/NULL lookup 50/50/51/54. Original scalar parameter, unused reference, callers and body restored.
// Round 31 (lane 10): with the existing c_type_47f957 by value (callers wrap the index), retail's
// structure is the unguarded counted for (zero, one count test, base load after it); ours then
// coalesces the material with the imul temporary in ECX (retail `lea edi,[ecx+edx]; test edi,edi`),
// which moves index/counter/response to DI/EDX/ESI: pointer cursor 50, indexed 50 (137 bytes), with
// `if (count > 0)` around it 23 (145, repeated test), count in the loop test 70, named base 53/55;
// harness (exact for all of these): the existing global_material_get in the condition or body with
// six loop forms, both compare orders, member/class parent update and declaration orders 21-52 (21
// only with the repeated guard); short/class walk copies 21-71; counter/count widths, operator==,
// &a[i], fallback ternary, counter declared outside 34-58.
// More harness forms: do/while on the result 21-33; a NULL-initialised function-scope material 21-50;
// `while ((material = get) != NULL)` with `if (result) break;` before or after the parent read
// 21-68 (gives retail's counter ECX and material EDI, but a second parent read); lookup tested
// before the result 46.
// @retail 0xfd4c0
s_projectile_material_response *__stdcall projectile_get_material_response(s_projectile_definition const *definition,
	short material_index)
{
	s_projectile_material_response *result = NULL;
	/* retail keeps material_index on the stack */
	short const *reference = &material_index;
	long response_count = definition->material_response_count;
	byte *materials = (byte *)g_4e034c;

	while (!result && material_index != NONE && material_index >= 0 &&
		material_index < *(long *)(materials + 0x150))
	{
		byte *material = *(byte **)(materials + 0x154) + material_index * 0xb4;

		if (!material)
			break;

		if (response_count > 0)
		{
			s_projectile_material_response *response = (s_projectile_material_response *)definition->material_responses;

			while (response != (s_projectile_material_response *)definition->material_responses + response_count)
			{
				if (response->material_index == material_index)
				{
					result = response;
					break;
				}
				++response;
			}
		}
		material_index = *(short *)(material + 8);
	}
	if (!result)
		result = &g_547660;
	return result;
}



extern vector3f *g_4687bc;
extern vector3f *g_4687a4;
extern short g_47d8e0;
long __stdcall effect_new_from_parameters(s_effect_parameters *parameters);
void __stdcall function_b9b90(long object_index, bool disable);
void function_1883e0(long tag_index, bool ignore_distance, point3f const *point, short element_index, long unused,
	long index, long variant, long *first_value04, long *second_value04, long *first_value, long *second_value,
	long *first_value0c, long *second_value0c);
long function_1895f0(s_sound_position const *position, real scale, long tag_index);
void function_11bed0(s_location *location, point3f const *point);
dword vector3d_compress(vector3f const *vector);
void function_b75a0(long object_index, point3f const *point, vector3f const *forward, vector3f const *up,
	s_location const *location, bool unknown);
void __stdcall function_b77d0(long object_index, vector3f const *linear_velocity, vector3f const *angular_velocity);
void __stdcall function_b93b0(long parent_index, long object_index, short node_index);
vector3f *random_unit_vector(vector3f *result, dword *seed);
void __stdcall function_1e2930(long object_index, long actor_index);
void function_a83e0(long object_index, long parent_index, point3f const *point, short node_index,
	vector3f const *forward);

/* a copy of the effect parameter defaults (unknown_175bd0.cpp keeps its
   own inline) */
PRIVATE inline void projectile_effect_parameters_initialize(s_effect_parameters *parameters)
{
	memset(parameters, 0, sizeof(*parameters));
	parameters->tag_index = NONE;
	parameters->unknown18 = NONE;
	parameters->object_index = NONE;
	parameters->owner.unknown4 = NONE;
	parameters->owner.unknown0 = NONE;
	parameters->unknown34 = 0;
	parameters->owner.unknown8 = NONE;
	parameters->unknown38 = 0;
	parameters->scale_a = 1.0f;
	parameters->scale_b = 1.0f;
	parameters->unknown3c = 0;
	parameters->unknown30 = 0;
	parameters->color_a = 0xff808080;
	parameters->color_b = 0xff808080;
	parameters->source = 0;
	parameters->flags = 0;
}

/* pushes a projectile, and spins it at random by the push's strength */
// @retail 0xfa820
void function_fa820(long projectile_index, vector3f const *impulse)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);

	if (*(long *)((byte *)projectile + 0x14) == NONE)
	{
		vector3f_add(&projectile->linear_velocity, impulse, &projectile->linear_velocity);

		vector3f axis;
		random_unit_vector(&axis, &g_4e7408->unknown0);
		real spin = function_x82e52f(&g_4e7408->unknown0, __FILE__, __LINE__);
		spin *= (real)sqrt(length_sq3f(impulse));

		vector3f rotation;
		vector3f_scale(&axis, spin * 1.5707964f, &rotation);
		vector3f *angular_velocity = &projectile->angular_velocity;
		angular_velocity->i = projectile->angular_velocity.i + rotation.i;
		angular_velocity->j = projectile->angular_velocity.j + rotation.j;
		angular_velocity->k = projectile->angular_velocity.k + rotation.k;
		function_fbd40(projectile_index);
		function_b9b90(projectile_index, false);
	}
}

/* the sound of a projectile's impact */
// @retail 0xfcdd0
void function_fcdd0(long definition_index, point3f const *point, vector3f const *forward)
{
	s_projectile_definition *definition = PROJECTILE_DEFINITION_GET(definition_index);
	if (*(long *)((byte *)definition + 0x124) != NONE)
	{
		s_sound_position position = { 0 };
		s_location location = { 0 };

		function_11bed0(&location, point);
		position.position = *point;
		position.compressed_forward = vector3d_compress(forward);
		position.velocity = *g_4687a4;
		position.location = location;
		function_1895f0(&position, 1.0f, *(long *)((byte *)definition + 0x124));
	}
}

/* a projectile's detonation (or impact) effect at a point, facing out of
   the surface it hit */
// @retail 0xfcbc0
void function_fcbc0(long definition_index, point3f const *point, vector3f const *normal,
	s_effect_owner const *owner, bool attached, bool airburst)
{
	s_projectile_definition *definition = PROJECTILE_DEFINITION_GET(definition_index);
	s_effect_marker markers[4];
	s_effect_parameters parameters;
	long effect_index;

	markers[3].position = *point;
	markers[2].position = *point;
	markers[1].position = *point;
	markers[0].position = *point;
	markers[0].forward = *normal;
	markers[1].forward = *g_4687bc;
	markers[2].forward = *g_4687b0;
	markers[3].forward.i = 0.0f - normal->i;
	markers[3].forward.j = 0.0f - normal->j;
	markers[3].forward.k = 0.0f - normal->k;
	markers[0].name = 0x700054c;
	markers[1].name = 0x70000c0;
	markers[2].name = 0x20000ca;
	markers[3].name = 0x8000550;
	if (attached)
		effect_index = *(long *)((byte *)definition + 0x114);
	else if (airburst)
		effect_index = *(long *)((byte *)definition + 0xf4);
	else
		effect_index = *(long *)((byte *)definition + 0xfc);

	projectile_effect_parameters_initialize(&parameters);
	parameters.tag_index = effect_index;
	parameters.marker_count = 4;
	parameters.markers = markers;
	parameters.flags |= 4;
	if (owner)
		parameters.owner = *owner;
	effect_new_from_parameters(&parameters);
}

/* the effects and sounds of an effects block's effect (+0x58), three of
   each, NONE without one:
   0x1883e0 gives sounds as its "first" values and effects as its "second"
   (retail passes the effect slots it reads back, the fifth argument, ecx
   and edx, as 0x1883e0's second_value04, second_value and second_value0c) */
// @retail 0xfd740
void function_fd740(point3f const *point, byte const *effects, long index, long element_index,
	long *effect0, long *sound0, long *effect1, long *sound1, long *effect2, long *sound2)
{
	long effect_index = *(long const *)(effects + 0x58);

	if (effect_index != NONE)
	{
		function_1883e0(effect_index, false, point, g_47d8e0, element_index, index, 0, sound0, effect0,
			sound1, effect1, sound2, effect2);
		return;
	}
	if (effect0)
		*effect0 = NONE;
	if (sound0)
		*sound0 = NONE;
	if (effect1)
		*effect1 = NONE;
	if (sound1)
		*sound1 = NONE;
	if (effect2)
		*effect2 = NONE;
	if (sound2)
		*sound2 = NONE;
}

/* the same for what a projectile hit: ignoring distance for multiplayer
   objects the network owns. The three sounds come first (the stack
   arguments) and the three effects after them (ecx, edx and edi in retail) */
// @retail 0xfd7d0
void function_fd7d0(long object_index, point3f const *point, byte const *effects,
	long index, long element_index, long *sound0, long *sound1, long *sound2, long *effect0,
	long *effect1, long *effect2)
{
	long effect_index = *(long const *)(effects + 0x58);

	if (effect_index != NONE)
	{
		bool ignore_distance = false;

		if (object_index != NONE)
		{
			s_projectile *object = PROJECTILE_GET(object_index);

			if (g_4e6948->state == 2 && g_4e6948->mode != 4 && *(long *)((byte *)object + 0xd4) != NONE)
				ignore_distance = true;
		}
		function_1883e0(effect_index, ignore_distance, point, g_47d8e0, element_index, index, 0, sound0, effect0,
			sound1, effect1, sound2, effect2);
		return;
	}
	if (sound0)
		*sound0 = NONE;
	if (effect0)
		*effect0 = NONE;
	if (sound1)
		*sound1 = NONE;
	if (effect1)
		*effect1 = NONE;
	if (sound2)
		*sound2 = NONE;
	if (effect2)
		*effect2 = NONE;
}

/* sticks a projectile to what it hit; location is the hit's s_location, which the
   callers' declarations still type as a vector */
// @retail 0xfd560
void function_fd560(long projectile_index, long object_index, short node_index, point3f const *point,
	vector3f const *location)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);
	s_projectile_definition *definition = PROJECTILE_DEFINITION_GET(projectile->tag_index);

	if (object_index != NONE && TEST_FIELD_BIT(definition->flag_bits.unknown3))
	{
		short attached_count = 0;

		for (long child_index = *(long *)((byte *)PROJECTILE_GET(object_index) + 0x10); child_index != NONE; )
		{
			s_projectile *child = PROJECTILE_GET(child_index);

			if (child->tag_index == projectile->tag_index && !TEST_FIELD_BIT(child->flags.unknown6))
			{
				child->unknown160 = 0.0f;
				child->unknown158 = 0.0f;
				attached_count++;
			}

			short maximum = *(short *)((byte *)definition + 0xe6);

			if (maximum && attached_count >= maximum)
			{
				*(dword *)&projectile->flags |= 0x80;
				break;
			}
			child_index = *(long *)((byte *)child + 0xc);
		}
	}

	function_b75a0(projectile_index, point, NULL, NULL, (s_location const *)location, false);
	function_b77d0(projectile_index, g_4687a4, g_4687a4);
	*(dword *)&projectile->flags |= 8;
	function_b9b90(projectile_index, true);
	if (object_index != NONE)
	{
		function_b93b0(object_index, projectile_index, (short)(node_index));

		s_projectile_header *header = &((s_projectile_header *)g_4e0300->data)[object_index & 0xffff];

		if ((1 << header->unknown00[3]) & 3)
		{
			s_projectile *parent = header->object;

			if (*(long *)((byte *)parent + 0x12c) != NONE && g_4f55d0->active)
				function_1e2930(projectile_index, *(long *)((byte *)parent + 0x12c));
		}
	}

	real seconds = 0.0f;

	if (TEST_FIELD_BIT(projectile->flags.unknown10))
		seconds = *(real *)((byte *)definition + 0x150);
	else if (TEST_FIELD_BIT(definition->flag_bits.unknown2))
		seconds = *(real *)((byte *)definition + 0xd8);

	real ticks = g_510c54->field_2_3 * seconds;

	if (ticks >= 1.0f)
		*(real *)((byte *)projectile + 0x15c) = 1.0f / ticks;
	function_a83e0(projectile_index, object_index, point, node_index, location);
}

real function_30bf0(vector3f *v);

/* aims a falling projectile along an arc: the slowest speed that reaches
   the target, then the time to it at the speed used (high or low arc),
   falling back to the slowest arc when the speed can't reach it */
// score 232 (was 232): the convention now agrees with retail (target EAX, origin ECX, direction
// ESI, the rest on the stack, `ret 0x2c`): the forced-speed and time-scale pointers are each read
// once through their addresses, which keeps them on the stack as retail reads them at their uses
// and matches the callers 0xfa6a0, 0x1f8100, 0x26f150 and 0x2900b0 (each passed the time scale in
// EDX before). Still differs: frame 0x28 vs 0x30; retail loads the gravity pointer into EDX at
// entry and keeps the distance output in EDI from entry (ours: gravity in EBX), rounds the
// horizontal length to a float before the gravity tests (ours keeps it on the x87 stack and
// computes it after those tests), evaluates the horizontal root in the entry block and divides by
// two_a twice in x87 (ours multiplies by one SSE reciprocal of two_a).
// Tried: the high-arc flag or the time-scale pointer read once through its address 266/269 (round
// 67: the time-scale pointer alone 244, the forced-speed pointer then takes EDX); horizontal tests with the constant first 266; the
// horizontal length computed after the gravity value 266; the gravity operands swapped 266;
// result flag declared first 275 (frame 0x30); optimal-time root with a double -1.0 244 (a final
// qword multiply where retail loads a dword -1.0 first); t2 root without its float cast 266;
// horizontal through magnitude2d 272; gravity computed first 266; the forced-speed test as
// `if (!forced_speed)` first 237 (kept, 983 bytes); with the forced speed as a ternary 266;
// delta.k after the gravity product 236; the time scale read once into a value (`time_scale ?
// *time_scale : 0.0f`) 262; both pointers through their addresses 244 (kept, see above), with
// the forced-speed read before the minimum-speed store 244.
// Entry velocity workspace 244; reversed entry vector declarations 244; output-first
// vector names 244/244; shared complete delta producer 245; common result exit 258;
// that with entry velocity 258; result-first with entry velocity 266 (frame 0x2c). Restored.
// Actual squared-time producer with found test 244; test-derived trajectory result
// 245; repeated time square alone/with found/result 244/244/245; result with native
// minimum clamp 246; coefficient home names 245; all scalar producer forms 246. Restored.
// Round 266: six lexical orders of early gravity/horizontal-root/squared-length
// scalars and two uninitialized entry declaration orders all 244. Every
// initialization, input observation and complete original body restored.
// Round 173: existing native gravity field 244; whole origin/target/both input points
// 298/299/327; those with complete shared delta 298/299/327; both with entry velocity
// 327. Original inputs, gravity observation, float trees and pointer contracts restored.
// Round 226: separate displacement/gravity admission results and complete horizontal products,
// seven subsets 282/283/244/298/282/283/298; combined admission on the full graph 298.
// Original body and pointer contract restored; frame 0x28/0x30 and rounded-root scheduling remain.
// Round 397: shared velocity scale 261; earlier path product 246; vertical stage first 244;
// scale/path 232 (983 bytes, plain kept); scale/vertical, path/vertical, all stages and
// all with library vector copies 246 each. Frame 0x28/0x30 and rounded-root/division scheduling remain.
// Round 402: native double denominator/optimal-time/horizontal lifetimes and their pairs/all
// 257/285/232/295/257/285/295; both native float division numerators named 232.
// All restored; frame 0x28/0x30, rounded horizontal root and separate divisions remain.
// Round 269: squared-distance workspace 232; root workspace 232; both 232; default flight time 266;
// distance/default 266; root/default 266; all 266; all with the twice-used time square 266. Original body restored.
// Round 294: Entry a/two_a/discriminant-base storage, seven nonempty subsets, all 232;
// all with b storage 232. Every original producer and complete body restored;
// frame 0x28/0x30, rounded-root scheduling and separate divisions remain.
// Round 222: found byte/short/long/dword with result first at entry, all 245; arc-only result,
// all 257. Original body restored; frame 0x28/0x30, rounded horizontal root and separate
// divisions remain.
// Lane 14 r16: double -1.0 with an uncast t2 root together 273; two_a written out at both
// divisions 248 (no reciprocal: both become x87 fdiv as in retail, frame 0x2c), that with
// `root * (-1.0f / two_a)` 248 (retail's fld -1/fdiv/fmul order); the named two_a with that
// order 232 (the SSE reciprocal returns); sqrtf for the horizontal length 232. Body restored.
// Round 10 (m3 lane 2, one-file harness identical to the real build): three causes remain, none
// with a natural form: retail never enregisters the forced-speed and time-scale pointers (it loads
// each at its use, so EDI is free for the distance output that 0xfa580 takes in EDI), it keeps the
// horizontal root in the entry block (stored as a float, compared in SSE), and it divides by two_a
// twice without a reciprocal. Two_a written out with the forced-speed test first 237 (ret 0x28, loses
// 0xfa6a0, 0x26f150, 0x1f8100 and 0x2900b0); gravity and horizontal tests as nested ifs 232; chosen
// speed defaulted at its declaration with the forced test first 238; the horizontal variable reused
// for the velocity's horizontal length 232; the time-scale speed chosen by a ternary with an else 261
// (971 bytes); six spilled-scalar renames 232 (harness).
// @retail 0xfa1a0
bool function_fa1a0(real speed, real gravity_scale, point3f const *origin, point3f const *target,
	real *minimum_speed, real const *time_scale, real const *forced_speed, bool high_arc, vector3f *direction,
	real *speed_out, real *time_out, real *distance, real *vertical_speed, real *horizontal_speed)
{
	vector3f delta;

	delta.i = target->x - origin->x;
	delta.j = target->y - origin->y;

	real horizontal_squared = delta.j * delta.j + delta.i * delta.i;
	real horizontal = (real)sqrt(horizontal_squared);
	real gravity = *(real *)g_51e9c4 * gravity_scale;
	bool result = true;

	delta.k = target->z - origin->z;
	if (!(0.0f > gravity) && gravity > 0.0f && (horizontal > 0.0001f || delta.k > 0.0001f))
	{
		real distance_squared = delta.k * delta.k + horizontal_squared;
		real a = gravity * gravity * 0.25f;
		real discriminant_base = distance_squared * a * 4.0f;
		real two_a = a * 2.0f;
		real b = gravity * delta.k;
		real root = -(real)sqrt(discriminant_base);
		real optimal_time = (real)sqrt(-1.0f / two_a * root);
		real minimum = b - root;
		real minimum_speed_value = 0.0f > minimum ? 0.0f : (real)sqrt(minimum);
		real chosen_speed;
		real time;
		bool found = false;

		if (minimum_speed)
			*minimum_speed = minimum_speed_value;

		real const *const *forced_speed_reference = &forced_speed;
		real const *forced = *forced_speed_reference;

		if (!forced)
		{
			real const *const *time_scale_reference = &time_scale;
			real const *scale = *time_scale_reference;

			chosen_speed = speed;
			if (scale && *scale > 0.0f)
			{
				real t = PIN(*scale * optimal_time, 0.001f, optimal_time);
				real x = -(distance_squared / (t * t) + t * t * a);
				real needed = (real)sqrt(b - x);

				if (speed > needed)
					chosen_speed = needed;
			}
		}
		else
		{
			chosen_speed = *forced;
		}

		if (chosen_speed >= minimum_speed_value)
		{
			real c = b - chosen_speed * chosen_speed;
			real discriminant = c * c - discriminant_base;

			if (0.0f > c && discriminant >= 0.0f)
			{
				real t2 = ((real)sqrt(discriminant) * (high_arc ? 1 : -1) - c) / two_a;

				if (t2 > 0.0f)
				{
					time = (real)sqrt(t2);
					found = true;
				}
			}
		}
		if (!found)
		{
			time = optimal_time;
			result = false;
			chosen_speed = minimum_speed_value;
		}

		real path_distance = time * chosen_speed;
		real inverse_time = 1.0f / time;
		vector3f velocity;

		vector3f_scale(&delta, inverse_time, &velocity);

		real horizontal_speed_value = (real)sqrt(velocity.j * velocity.j + velocity.i * velocity.i);

		velocity.k += time * gravity * 0.5f;

		real vertical = velocity.k;

		if (function_30bf0(&velocity) == 0.0f)
		{
			result = false;
			velocity = delta;
			if (function_30bf0(&velocity) == 0.0f)
				velocity = *g_4687b0;
		}
		*direction = velocity;
		if (distance)
			*distance = path_distance;
		if (speed_out)
			*speed_out = chosen_speed;
		if (vertical_speed)
			*vertical_speed = vertical;
		if (horizontal_speed)
			*horizontal_speed = horizontal_speed_value;
		if (time_out)
			*time_out = time;
		return result;
	}
	return function_fa580(speed, origin, target, direction, distance, speed_out, time_out);
}

/* aims a projectile of a definition at a target: along an arc for
   projectiles that fall, in a line otherwise */
// @retail 0xfa6a0
bool function_fa6a0(long definition_index, real const *speed_override, point3f const *origin, point3f const *target,
	real *unknown2, real const *unknown3, real const *unknown4, bool unknown5, vector3f *direction, real *speed_out,
	real *time, real *distance, bool *linear)
{
	s_projectile_definition *definition = PROJECTILE_DEFINITION_GET(definition_index);
	real speed;
	bool result;

	if (!speed_override)
	{
		real scale = TEST_FIELD_BIT(definition->flag_bits.difficulty_scaled) ? projectile_difficulty_scale() : 1.0f;

		speed = definition->unknown17c * scale;
	}
	else
	{
		speed = *speed_override;
	}

	if (TEST_FIELD_BIT(definition->flag_bits.gravity) && definition->gravity_scale > 0.0f)
	{
		result = function_fa1a0(speed, definition->gravity_scale, origin, target, unknown2, unknown3, unknown4,
			unknown5, direction, speed_out, time, distance, NULL, NULL);
		if (linear)
			*linear = false;
		return result;
	}
	result = function_fa580(speed, origin, target, direction, distance, speed_out, time);
	if (linear)
		*linear = true;
	return result;
}

struct s_collision_result_1697c0
{
	long type;
	real t;
	point3f point;
	byte unknown14[0x1c - 0x14];
	s_location location;
	short unknown24;
	byte unknown26[2];
	vector3f normal;
	byte unknown34[0x3c - 0x34];
	long unknown3c;
	long object_index;
	short unknown44;
	short node_index;
	byte unknown48[0x50 - 0x48];
	long unknown50;
	byte unknown54[0x58 - 0x54];
	byte unknown58;
	byte unknown59;
	short unknown5a;
};

bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);
real function_30bf0(vector3f *v);
extern vector3f *g_4687ac;

/* whether a projectile's path to a point hits anything: the line from it,
   and for projectiles with a radius the two lines either side of it */
// score 163 (was 163): simultaneous offset endpoints recover 757/756 bytes; frame 0x48
// versus 0x34 and endpoint/query homes still differ. Shared input differences 220;
// radius at use 224; both 233; simultaneous endpoints 163 plain (kept); endpoints with
// radius reads 181, plus shared differences 188; shared ray displacement 189, with
// shared cross product 189; first-ray scope 165, with radius reads 173.
// Round 212: shared negative radius 173; plain endpoint loop 204; side producer
// scope 163; negative/loop 204; negative/scope 173; loop/scope and all three 204.
// Negative with initial-ray scope 175 (frame 0x3c); consistent radius at use with
// initial scope alone/side scope/endpoint loop 173/173/201. Frame and ray homes remain.
// Original-producer all-ray vector reuse 163; side/query workspace reuse 163; complete
// vector reuse 166 (756 bytes, frame 0x3c); with shared inputs 165, radius reads 179,
// both 189; complete reuse with early admission 183, plus shared inputs 186. Restored.
// Round 279: all four endpoints through existing point setters, with query-vector reuse
// and result-first exit combinations 163/184/163/183; existing scaled-vector sums on the
// same graphs 180/177/181/178. Original body/include set restored; frame/tag/query homes remain.
// Round 266: native aggregate producers for all four endpoints, initial ray,
// lateral displacement/cross output and all seven subsets 163 each; complete
// endpoint aggregates with the negative pair first 186. Original body restored.
// Round 173: native position/ignore/radius fields with external object/tag pointer or reference
// receivers, mutable or read-only, all eight combinations 163. Original layout and body restored.
// Round 226: positive/negative endpoint names, reversed declarations and full displacement roles 163/163/163;
// with bounded direct ray 165, live radius 181, both 173; actual call results 172, all together 179.
// Original body restored; frame 0x48/0x34 and tag/query ownership remain.
// Round 397: immutable positive/negative/both endpoint pairs 163 each; each with
// bounded initial ray 165; positive/both with original query reuse 163 each. Actual
// initial displacement retained after the read-only query 180; with shared cross,
// later query reuse or both 180 each. Original body restored; frame/tag/query homes remain.
// DL176: Whole actual query-result lifetime in bool/byte/short/long 182/177/172/168; with
// consistent radius-field reads 198/193/179/185. Full-width result reaches 756 bytes but
// regresses. Original result exits, cached radius, simultaneous endpoints and body restored;
// frame/tag/query homes remain.
// Round 430: complete repeated positive/negative offsets, native products or existing scale producer,
// with actual long query result and bounded first-ray combinations 168/170/167/169 each.
// All reach 756 bytes; unbounded/bounded frames 0x3c/0x30 still miss 0x34. Original body restored.
// Round 213: existing staged cross with original/shared input differences and original/bounded
// initial ray 163/163/165/165; consistent radius-field counterparts 181/188/173/180.
// Original body restored; tag EBP and native frame 0x34 endpoint/query overlap remain.
// Round 42: normalized side consumed as final negative endpoint 164 (frame 0x3c);
// complete scalar cross input, direct up fields and both 163 each; endpoint reuse with
// scalar input/direct up/both 164 each, with bounded first ray 166. Uniform radius
// fields on endpoint reuse alone/all-input graph 181, with bounded first ray 173 each.
// All original executable source restored; frame 0x34 and tag/query allocation remain.
// Round 10 (m3 lane 2): retail computes each endpoint product twice (no common side * radius between
// the start and end points: separate radius copies, as an inlined two-point ray test's parameters
// give); block scopes give the slot sharing but not the products. Each query vector in its own block
// 166 (frame 0x30); the two side query vectors only 164; all three with the radius read again inside
// the radius block 171; endpoints through vector3f_add_scaled with 0 - radius / -radius 180/180 (769
// bytes). Body restored.
// Lane 15 round 18: retail inlines the two-point ray test 0x16a040 for all three queries (its
// vector homes give retail's sharing: first query/start, both offset queries). With 0x16a040
// made `inline` (noinline declaration dropped): three calls 187 (frame 0x3c); radius field at
// each use 166 (integer registers and opening as retail; end.z spilled into end's home); block
// radius local 188/170; origin-side endpoints first 174 (frame 0x30); endpoints through
// vector3f_add_scaled 158 (F0 spill as retail); with a named negative radius 159; offset queries
// joined by || 158. Not kept: 0x1c8df0 then inlines three of its 0x16a040 calls (170 -> 457).
// @retail 0xfc030
bool function_fc030(long projectile_index, point3f const *point, s_collision_result_1697c0 *collision)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);
	s_projectile_definition *definition = PROJECTILE_DEFINITION_GET(projectile->tag_index);
	point3f *origin = (point3f *)((byte *)projectile + 0x64);
	vector3f vector;

	vector.i = point->x - origin->x;
	vector.j = point->y - origin->y;
	vector.k = point->z - origin->z;
	if (function_1697c0(0x2480000f, origin, &vector, *(long *)((byte *)projectile + 0x140), projectile_index, collision))
		return true;

	real radius = *(real *)((byte *)definition + 0xc8);

	if (!(0.0001f > radius) && *(long *)((byte *)projectile + 0xc4) != NONE)
	{
		vector3f side;
		vector3f const *up = g_4687b0;
		vector3f delta;

		delta.i = point->x - origin->x;
		delta.j = point->y - origin->y;
		delta.k = point->z - origin->z;
		side.i = delta.k * up->j - up->k * delta.j;
		side.j = up->k * delta.i - up->i * delta.k;
		side.k = up->i * delta.j - delta.i * up->j;
		if (function_30bf0(&side) == 0.0f)
			side = *g_4687ac;

		point3f start;
		point3f end;
		point3f opposite_start;
		point3f opposite_end;
		vector3f offset_vector;

		start.x = side.i * radius + origin->x;
		start.y = side.j * radius + origin->y;
		start.z = side.k * radius + origin->z;
		end.x = side.i * radius + point->x;
		end.y = side.j * radius + point->y;
		end.z = side.k * radius + point->z;
		opposite_start.x = (0.0f - radius) * side.i + origin->x;
		opposite_start.y = side.j * (0.0f - radius) + origin->y;
		opposite_start.z = side.k * (0.0f - radius) + origin->z;
		opposite_end.x = (0.0f - radius) * side.i + point->x;
		opposite_end.y = side.j * (0.0f - radius) + point->y;
		opposite_end.z = side.k * (0.0f - radius) + point->z;
		vector3d_from_points3d(&start, &end, &offset_vector);
		if (function_1697c0(0x4800008, &start, &offset_vector, *(long *)((byte *)projectile + 0x140), NONE, collision))
			return true;
		vector3d_from_points3d(&opposite_start, &opposite_end, &offset_vector);
		if (function_1697c0(0x4800008, &opposite_start, &offset_vector, *(long *)((byte *)projectile + 0x140), NONE, collision))
			return true;
	}
	return false;
}

void function_1763a0(point3f const *point, vector3f const *direction, s_effect_marker *markers,
	vector3f const *normal);
long function_189fe0(s_sound_request const *request, long tag_index);

/* the effects and sounds of an effects block (a material's response) at
   a point */
// score 4 (was 4): only the register arguments of the 0xfd740 call differ (retail
// passes the first sound in esi, the second effect in ecx, the second sound in edi
// and the third effect in edx; ours ebx/edi/esi/edx), set by that callee's own body.
// BLOCKED-BY 0xfd740. NUMBEROF loop bounds 17; parameter defaults in the effects
// file's order 14; request filled in 0x1895f0's order 10; scale and audible bits
// before the rest 4.
// Coordinated selector output-parameter orders (original roles 0..5): 024135/042135 3;
// 024315/042315/014325/032145 4; 041235/021435 3. Definition and both
// callers changed together; every order restored because gains are at most one.
// Caller body: separate unsigned ordinals 4; array names/order combinations 4/4/4;
// both plain pointer walks with array names/order combinations 114/114/114/114.
// @retail 0xfcea0
void function_fcea0(long effects_index, point3f const *point, vector3f const *direction,
	s_effect_owner const *owner, long index, vector3f const *normal)
{
	byte *effects = g_4e3b44[effects_index & 0xffff].bytes;
	long sounds[3];
	long effect_indices[3];
	s_location location;
	long i;

	function_11bed0(&location, point);
	function_fd740(point, effects, 0xf, index, &effect_indices[0], &sounds[0], &effect_indices[1], &sounds[1],
		&effect_indices[2], &sounds[2]);
	for (i = 0; i < NUMBEROF(effect_indices); i++)
	{
		long effect_index = effect_indices[i];

		if (effect_index != NONE)
		{
			s_effect_marker markers[6];
			s_effect_parameters parameters;

			function_1763a0(point, direction, markers, normal);
			projectile_effect_parameters_initialize(&parameters);
			parameters.markers = markers;
			parameters.tag_index = effect_index;
			parameters.marker_count = 6;
			parameters.flags = 4;
			if (owner)
				parameters.owner = *owner;
			effect_new_from_parameters(&parameters);
		}
	}
	for (i = 0; i < NUMBEROF(sounds); i++)
	{
		long sound_index = sounds[i];

		if (sound_index != NONE)
		{
			s_sound_position position;
			s_sound_request request;

			position.position = *point;
			position.compressed_forward = vector3d_compress(direction);
			position.velocity = *g_4687a4;
			position.location = location;
			request.location.unknown02 = 0;
			request.location.flags = 0;
			request.location.spatial = position;
			request.location.scale = 1.0f;
			request.location.audible = 1;
			request.location.requested_audible = 1;
			request.location.unknown08 = 0;
			request.marker = NULL;
			request.source = NULL;
			request.variant = NULL;
			request.object_index = NONE;
			request.platform_playback = NONE;
			function_189fe0(&request, sound_index);
		}
	}
}

/* a projectile's detonation effects and sounds at a point, attached to an
   object's node when it stuck to one */
// score 115 (was 115): frame 0x150 and extent agree; point EBX/EDI, zero lifetime and selector call registers differ.
// Round 196: array-size bounds, unsigned shared ordinal and distinct unsigned ordinals 148 each;
// word index 115; long node 148; both widths 115 (kept); both widths with array bounds
// or separate ordinals 115 each. Every declaration coordinated; original loop bodies retained.
// Round 212: chained defaults 119; early loop ordinal 188; outer effect storage 153;
// defaults/ordinal 187; defaults/storage 157; ordinal/storage and all three 206;
// aggregate defaults with ordinal/storage 206. Native zero and point lifetimes remain.
// Existing request builder 251; direct spatial output 201; builder with point snapshot 250;
// point/direction/both snapshots 195/210/235; defaults before markers 189;
// matched sibling request-store order 120. Original body and all call roles restored.
// Attachment bitfield/word/complete conditional results 190/188/192; attachment/all
// overrides after selector 121/176; bitfield/complete conditional attachment after
// selector 163/179; all overrides with word result after selector 175. Restored.
// Coherent workspace names 115; paired-table initialization loop 249; sound-table
// reads at use 115; names with loop/reads 249/115; loop with reads and all three 249;
// separate direct-counter table initialization loops 249. Original body restored.
// Round 408: native definition pointer/reference with neither, fallback, alternate
// or both tag snapshots 115 each. Original record layout and body restored.
// Round 30: BLOCKED-BY 0xfd740: its unmatched body takes a pointer argument in EBX, so ours
// cannot keep retail's zero in EBX across that call (the point then takes EBX). Flag-bit
// member for both `|= 4` 115 (same code); tag before the flag 115 (same code); markers
// before the scales 115; compression before the point copy 117; velocity copy first 178.
// @retail 0xfd0e0
void function_fd0e0(long definition_index, real scale_a, real scale_b, vector3f const *direction,
	point3f const *point, vector3f const *normal, word index, bool attached, long object_index,
	long node_index, bool alternate)
{
	byte *definition = g_4e3b44[definition_index & 0xffff].bytes;
	long sounds[3];
	long effect_indices[3];
	s_location location;
	long i;

	effect_indices[0] = NONE;
	effect_indices[1] = NONE;
	effect_indices[2] = NONE;
	sounds[0] = NONE;
	sounds[1] = NONE;
	sounds[2] = NONE;
	function_11bed0(&location, point);
	{
		s_effect_marker markers[6];
		s_effect_parameters parameters;

		function_1763a0(point, direction, markers, normal);
		projectile_effect_parameters_initialize(&parameters);
		if (attached)
		{
			parameters.flags = 1;
			parameters.object_index = object_index;
			parameters.unknown18 = node_index;
		}
		parameters.scale_a = scale_a;
		parameters.markers = markers;
		parameters.scale_b = scale_b;
		parameters.marker_count = 6;
		function_fd740(point, definition, 0xf, index, &effect_indices[0], &sounds[0], &effect_indices[1], &sounds[1],
			&effect_indices[2], &sounds[2]);
		if (alternate && *(long *)(definition + 0xec) != NONE)
		{
			parameters.flags |= 4;
			parameters.tag_index = *(long *)(definition + 0xec);
			effect_new_from_parameters(&parameters);
			return;
		}
		for (i = 0; i < 3; i++)
		{
			if (effect_indices[i] != NONE)
			{
				parameters.tag_index = effect_indices[i];
				effect_new_from_parameters(&parameters);
			}
		}
		parameters.flags |= 4;
		if (*(long *)(definition + 0x144) != NONE)
		{
			parameters.tag_index = *(long *)(definition + 0x144);
			effect_new_from_parameters(&parameters);
		}
	}
	for (i = 0; i < 3; i++)
	{
		long sound_index = sounds[i];

		if (sound_index != NONE)
		{
			s_sound_position position;
			s_sound_request request;

			position.position = *point;
			position.compressed_forward = vector3d_compress(direction);
			position.velocity = *g_4687a4;
			position.location = location;
			request.location.unknown02 = 0;
			request.location.flags = 0;
			request.location.spatial = position;
			request.location.scale = 1.0f;
			request.location.audible = 1;
			request.location.requested_audible = 1;
			request.location.unknown08 = 0;
			request.object_index = NONE;
			request.platform_playback = NONE;
			request.marker = NULL;
			request.source = NULL;
			request.variant = NULL;
			function_189fe0(&request, sound_index);
		}
	}
}

struct s_match_globals;
extern s_match_globals *g_4e0348;

/* a random real between two bounds */
PRIVATE __forceinline real projectile_random_range(real lower, real upper)
{
	real random = function_x82e52f(&g_4e7408->unknown0, __FILE__, __LINE__);

	return (upper - lower) * random + lower;
}

/* the root of an object's parents */
PRIVATE inline long projectile_object_root(long object_index)
{
	long root = NONE;

	while (object_index != NONE)
	{
		root = object_index;
		object_index = *(long *)((byte *)PROJECTILE_GET(object_index) + 0x14);
	}
	return root;
}

/* the projectile type's new callback: its target, lifetime, range,
   attachment, starting velocity and spin, whether it starts under water,
   and the seat flags of the unit that fired it */
// score 332 (was 332): frame 4 agrees; pool/input and tag receiver allocation differ at +0xb.
// Speed update is still called instead of expanded; 1226/1261 bytes.
// Native flag tests 380; complete water result 375; both 380; plain velocity-component
// loop 364 fast, 360 plain (kept); loop/flags 381; loop/water 364; all three 381;
// chained target initialization with all three 381. Unchanged reduced control 375.
// Complete object/tag/both observations 550/332/735; shared scaled addition 364;
// with tag/object/both observations 344/523/715; initial setter on complete graph 713;
// retained entry tag identity 396. Tag observations alone kept, plain 332; others restored.
// Round 266: actual current-root parent result with live/cached tags, native
// water expression and inline/forced combinations 334/334/375/375 twice;
// live-tag root graph with shared initial-speed publication 348. Original
// parent helper, callback, speed observations and every call restored.
// Round 173: actual input-record fields with object pointer/reference and ordinary/forced
// root expansion 332 each; consistently cached tag on those four graphs 360 each.
// Four earlier slot-observation explorations score 484 and are excluded for overlap risk.
// Original input accesses, receiver graph, tag observations and root declaration restored.
// Round 226: native object-header/tag-instance entry observations and shared speed publication,
// seven subsets 484/383/383/358/447/411/339; full graph with native tag reference 339.
// Original body restored; slot/input allocation and speed-update expansion remain.
// Round 397: full unit-word observations 354; retained unit receiver 352; both 352;
// word/speed, receiver/speed and all three 388/392/387; both unit forms with shared
// scaled addition 351, with reused speed 364. Original callback restored; entry slot/input
// allocation and speed-update expansion remain.
// Round 402: native projectile timer/ownership/forward field observations, seven subsets,
// all 332; complete native fields with both twelve-byte library copies 332. Restored;
// slot/input allocation and expanded speed update remain, frame four agrees.
// Round 294: Entry ticks/arming-ticks/speed storage, seven nonempty subsets, all 332;
// all with owner-index storage 332. All producers and the complete callback restored;
// frame four agrees, slot/input allocation and speed-update expansion remain.
// Round 222: shared zone predicate 346, with shared scaled sum 332; both library copies tie each
// form. Shared zone with cached tag, with/without scaled sum or copies, 381 each. Original
// body/include restored; slot/input allocation and speed-update expansion remain.
// Lane 14 r16: BLOCKED-BY 0xfbfd0 (contributor code): retail expands it inline after 0xfbd40, which
// keeps index * 12 in EBP and the definition in EDI from entry. A definition local at entry 364
// (EDI as retail, 1176 bytes), with the bitfield test 395, with `(flags >> 2) & 1` 363; that last
// with `inline` on 0xfbfd0 (diagnostic only, not kept) 313 at 1248 bytes. Body restored.
// @retail 0xf8200
bool __stdcall function_f8200(long projectile_index, byte const *data, long unused)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);

	*(dword *)&projectile->flags |= 2;
	projectile->target.object_index = NONE;
	projectile->target.node_index = NONE;
	projectile->action = 0;
	*(short *)((byte *)projectile + 0x132) = g_47d8e0;
	*(vector3f *)((byte *)projectile + 0x134) = *g_4687b0;
	*(long *)((byte *)projectile + 0x140) = projectile_object_root(*(long const *)(data + 0x60));
	*(real *)((byte *)projectile + 0x188) = 1.0f;
	*(short *)((byte *)projectile + 0x1a8) = NONE;
	*(short *)((byte *)projectile + 0x1aa) = 0;
	*(real *)((byte *)projectile + 0x16c) = function_fd8b0(projectile->tag_index, *(long const *)(data + 0x5c));

	if (*(real *)(((byte *)PROJECTILE_DEFINITION_GET(projectile->tag_index)) + 0x184) == *(real *)(((byte *)PROJECTILE_DEFINITION_GET(projectile->tag_index)) + 0x188))
	{
		*(real *)((byte *)projectile + 0x18c) = *(real *)(((byte *)PROJECTILE_DEFINITION_GET(projectile->tag_index)) + 0x184);
	}
	else
	{
		*(real *)((byte *)projectile + 0x18c) =
			projectile_random_range(*(real *)(((byte *)PROJECTILE_DEFINITION_GET(projectile->tag_index)) + 0x184), *(real *)(((byte *)PROJECTILE_DEFINITION_GET(projectile->tag_index)) + 0x188));
	}

	real ticks;

	if ((PROJECTILE_DEFINITION_GET(projectile->tag_index))->flags & 4)
		ticks = g_510c54->field_2_3 * *(real *)(((byte *)PROJECTILE_DEFINITION_GET(projectile->tag_index)) + 0xd4);
	else if (*(real *)(((byte *)PROJECTILE_DEFINITION_GET(projectile->tag_index)) + 0xd4) == *(real *)(((byte *)PROJECTILE_DEFINITION_GET(projectile->tag_index)) + 0xd8))
		ticks = g_510c54->field_2_3 * *(real *)(((byte *)PROJECTILE_DEFINITION_GET(projectile->tag_index)) + 0xd4);
	else
		ticks = projectile_random_range(*(real *)(((byte *)PROJECTILE_DEFINITION_GET(projectile->tag_index)) + 0xd4), *(real *)(((byte *)PROJECTILE_DEFINITION_GET(projectile->tag_index)) + 0xd8)) *
			g_510c54->field_2_3;
	if (ticks >= 1.0f)
		*(real *)((byte *)projectile + 0x15c) = 1.0f / ticks;

	real arming_ticks = g_510c54->field_2_3 * *(real *)(((byte *)PROJECTILE_DEFINITION_GET(projectile->tag_index)) + 0xcc);

	if (arming_ticks >= 1.0f)
		*(real *)((byte *)projectile + 0x164) = 1.0f / arming_ticks;

	projectile->attachment_index = NONE;
	for (short i = 0; i < *(long *)(((byte *)PROJECTILE_DEFINITION_GET(projectile->tag_index)) + 0x94); i++)
	{
		if (*(long *)(*(byte **)(((byte *)PROJECTILE_DEFINITION_GET(projectile->tag_index)) + 0x98) + i * 0x18) == 'cont')
		{
			projectile->attachment_index = i;
			break;
		}
	}

	real speed = *(real *)((byte *)projectile + 0x16c) * (PROJECTILE_DEFINITION_GET(projectile->tag_index))->unknown17c;
	vector3f *forward = (vector3f *)((byte *)projectile + 0x70);

	for (long component = 0; component < 3; ++component)
		projectile->linear_velocity.n[component] += forward->n[component] * speed;
	*(real *)((byte *)projectile + 0x190) = *(real *)((byte *)projectile + 0x16c) * (PROJECTILE_DEFINITION_GET(projectile->tag_index))->unknown17c;
	*(dword *)&projectile->flags &= ~0x100;

	bool under_water = false;
	short cluster_index = *(short *)((byte *)projectile + 0x2c);

	if (cluster_index != NONE)
	{
		byte *bsp = (byte *)g_4e0348;
		byte water = *(*(byte **)(bsp + 0xa0) + cluster_index * 0xb0 + 0x70);

		if (water != 0xff)
		{
			byte *plane = *(byte **)(bsp + 0x68) + (water & 0x7f) * 0x18;

			if (*(short *)(plane + 2) != NONE)
			{
				if (!(water & 0x80))
				{
					under_water = true;
				}
				else
				{
					point3f *center = (point3f *)((byte *)projectile + 0x30);

					if (0.0f > *(real *)(plane + 0xc) * center->z + *(real *)(plane + 8) * center->y +
						*(real *)(plane + 4) * center->x - *(real *)(plane + 0x10))
					{
						under_water = true;
					}
				}
			}
		}
	}
	if (under_water)
		*(dword *)&projectile->object_flags |= 8;
	else
		*(dword *)&projectile->object_flags &= ~8;

	function_fbd40(projectile_index);
	function_fbfd0(projectile_index);
	*(dword *)&projectile->object_flags |= 0x10000;
	*(dword *)&projectile->object_flags |= 0x20000;
	*(point3f *)((byte *)projectile + 0x198) = *(point3f const *)(data + 0x1c);
	*(long *)((byte *)projectile + 0x1a4) = NONE;

	long owner_index = *(long const *)(data + 0x60);

	if (owner_index != NONE)
	{
		long root = projectile_object_root(owner_index);
		byte *header = g_4e0300->data + (root & 0xffff) * 12;

		if ((1 << header[3]) & 1)
		{
			byte *unit = *(byte **)(header + 8);
			long weapon_index = *(long *)(unit + 0x3e8);
			word unit_flags = *(word *)(unit + 0xc0);

			if (weapon_index != NONE &&
				(((unit_flags >> 1) & 1) || ((unit_flags >> 2) & 1) || weapon_index == *(long *)(unit + 0x3e4)))
			{
				*(dword *)&projectile->flags |= 0x8000;
				if ((*(byte *)(*(byte **)(header + 8) + 0xc0) >> 2) & 1)
					*((byte *)projectile + 0xc0) |= 4;
				else
					*((byte *)projectile + 0xc0) &= ~4;
				if ((*(byte *)(*(byte **)(header + 8) + 0xc0) >> 1) & 1)
					*((byte *)projectile + 0xc0) |= 2;
				else
					*((byte *)projectile + 0xc0) &= ~2;
				*(long *)((byte *)projectile + 0xb8) = *(long *)(*(byte **)(header + 8) + 0xb8);
			}
		}
	}
	return true;
}

real function_17c900(short function_type, real input);

/* where a guided projectile aims: its target's point, led by the target's
   velocity, then wobbled around the line to it or wandering */
// score 348 (was 348): Frame 0x30 recovered; index EAX/output-stack contract and normalization/rotation scheduling remain, four bytes long.
// Earlier 382 (was 382): Frame 0x2c versus 0x30 and index EAX versus stacked index remain.
// Consistent parameter reorder 382; common exit 382; lead field at every use 376 (six-point gain dropped);
// shared target pointer 382; point/direction role names 382; shared cross 382; shared point differences 383;
// complete spherical direction 401; early index-only salt 377 (five-point gain dropped); target-distance
// name 382; shared normalizer 382; dot after trigonometry 382; early salt plus field reads 387.
// DH round 218: separate final aim point/common output 393, both point setters 382, both 393;
// early salt with separate result/setters/both 375/377/375 (seven-/five-point leads restored);
// readonly center with separate result, with/without setters 393/393. Reuse the existing target
// point and publish once, scalar/shared setter 365/365; with early index-only salt 348/348.
// Shared setter plus early salt kept, plain 348; all game-call arguments unchanged.
// DH round 232: Complete rotation cross/output/both 352/353/345 (345 at retail length,
// three-point lead rejected); lead-speed product alone/with cross/with rotation/with both
// 361/374/365/368; all with complete wandering direction 369. All restored; index/output
// contract and normalization/rotation homes remain. Original arithmetic grouping retained.
// Round 248: real center field with pointer/direct uses 348/348; native signed-word salt
// with either receiver 360/360; both existing point-difference producers 368/368, with
// native salt 367/367. Declared lead field read at each use 369, with native salt 372.
// All bodies and real-field trials restored; frame agrees, stacked index/output EBX still differ.
// Parallel lead/turn/drift scalar entry reservations, seven subsets, and complete scalar role
// names all 348; every original declaration and assignment restored.
// DH round 234: caller output promoted before final branches with target/output reads, after
// target-copy acquisition or at the target producer 361/371/395/411; readonly input objects
// with those four boundaries 361/371/395/411. All restored; input contract and vector homes remain.
// DL176: Ret-4 stdcall, inline target, both 348/348/348; with salt at its drift expression
// 365/365/365; both with native signed-word salt late/early 366/360. Original convention, summary,
// salt and complete body restored; stacked-index/output-EBX transport and normalization homes
// remain.
// DH round 236: lead/turn/drift vector storage promoted without moving their input reads,
// all seven subsets 348 each; complete promoted graph with role-based point/vector names 348.
// Original branch storage and complete body restored; input transport and vector homes remain.
// Round 430: complete lead-offset/staged turn-cross/clock receiver subsets
// 344/348/344/362/365/362/365; complete graph with point/direction names and declaration order 365.
// Four-point lead-offset forms reach retail length but are below the keep margin; original body restored.
// Round 294: Unsigned word/dword salt 360/371; paired word/dword phase counters 354/371;
// word-word/dword-word/word-dword/dword-dword 354/354/371/371. Complete body
// restored; stacked-index/output-EBX transport and vector homes remain.
// Lane 14 r16: retail writes the output in each of three exits (two point setters and a plain copy)
// and reads the index again for the salt: late named salt with the three exits 382 (the index stays
// an EAX argument copied to EBX, the output goes to EBP); parameters swapped 382 (same code); a
// target pointer local 382; `(void)&projectile_index` 382; the salt written at both uses 328 (retail's
// convention: index on the stack, frame 0x30, first difference +0x4f; aligned difference 263 -> 162,
// not kept as it is a value written out at each use); without the target local 328; that with
// normalize_inline 326 (aligned 168); with a wander direction vector 362 (aligned 150). Remaining:
// retail's zero register in XMM4 from the to_target block and the normalized components kept in
// XMM1-3. Body restored.
// Round 10 (m3 lane 2): output written in each of the three exits with the entry salt 377 (1477
// bytes); salt named after the center pointer 347; game time named in the wander branch 348; that
// with the salt named there too 365; the wobble clock as an if/else in place of the ternary 348.
// The salt written at both uses again gives retail's convention, 329, and with the three exits 328
// (aligned difference 140, 70 with registers masked: the rotation's products and the normalized
// components' homes remain). Body restored.
// @retail 0xf87f0
void function_f87f0(long projectile_index, point3f *aim_point)
{
	long salt = projectile_index >> 16;
	s_projectile *projectile = PROJECTILE_GET(projectile_index);
	s_projectile_definition *definition = PROJECTILE_DEFINITION_GET(projectile->tag_index);
	point3f *center = (point3f *)((byte *)projectile + 0x30);
	point3f arg_0e6828;
	vector3f to_target;

	function_f86f0(&projectile->target, &arg_0e6828);
	to_target.i = arg_0e6828.x - center->x;
	to_target.j = arg_0e6828.y - center->y;
	to_target.k = arg_0e6828.z - center->z;

	real distance = projectile_normalize(&to_target);
	real lead_factor = *(real *)((byte *)definition + 0x198);

	if (lead_factor > 0.0f)
	{
		real speed = (real)sqrt(length_sq3f(&projectile->linear_velocity));

		if (speed > 0.0001f)
		{
			vector3f target_velocity;

			function_ba1d0(projectile->target.object_index, &target_velocity, NULL);

			real target_speed = function_30bf0(&target_velocity);
			real time = PIN(distance / speed, 0.0f, 2.0f);
			real lead = PIN(lead_factor * target_speed * time, 0.0f, 2.0f);

			arg_0e6828.x += target_velocity.i * lead;
			arg_0e6828.y += target_velocity.j * lead;
			arg_0e6828.z += target_velocity.k * lead;
		}
	}

	if (TEST_FIELD_BIT(definition->flag_bits.unknown9) && distance > 0.0001f)
	{
		vector3f const *up = g_4687b0;
		vector3f axis;

		axis.i = to_target.k * up->j - up->k * to_target.j;
		axis.j = up->k * to_target.i - to_target.k * up->i;
		axis.k = up->i * to_target.j - to_target.i * up->j;
		function_30bf0(&axis);

		real time = (g_510c54 && g_510c54->active) ? (real)g_510c54->game_time * g_510c54->rate : 0.0f;
		real angle = time * 6.5f;
		real amount = PIN(distance * 0.05f, 0.0f, 2.0f);
		real dot = axis.k * to_target.k + axis.j * to_target.j + axis.i * to_target.i;
		real sine = (real)sin(angle);
		real cosine = (real)cos(angle);
		real along = dot * (1.0f - cosine);

		real_point3d_set(&arg_0e6828, (to_target.i * along + axis.i * cosine - (axis.j * to_target.k - axis.k * to_target.j) * sine) * amount + arg_0e6828.x,
			(to_target.j * along + axis.j * cosine - (axis.k * to_target.i - to_target.k * axis.i) * sine) * amount + arg_0e6828.y,
			(to_target.k * along + axis.k * cosine - (to_target.j * axis.i - axis.j * to_target.i) * sine) * amount + arg_0e6828.z);
	}
	else if (!TEST_FIELD_BIT(definition->flag_bits.unknown7))
	{
		vector3f offset;

		offset.i = center->x - arg_0e6828.x;
		offset.j = center->y - arg_0e6828.y;
		offset.k = center->z - arg_0e6828.z;

		real range = (real)sqrt(length_sq3f(&offset));
		real amount;

		if (range > 10.0f)
			amount = 0.8f;
		else if (range > 2.0f)
			amount = PIN((range - 2.0f) * 0.1f, 0.0f, 0.8f);
		else
			amount = 0.0f;

		real time_a = (real)((g_510c54->game_time + salt * 3) & 0xffff) * g_510c54->rate;
		real time_b = (real)((g_510c54->game_time + salt * 7) & 0xffff) * g_510c54->rate;
		real yaw = function_17c900(10, time_b * (1.0f / 3.0f)) * 6.2831855f;
		real pitch = 3.1415927f - function_17c900(10, time_a * (1.0f / 3.0f)) * 1.5707964f;
		real cosine_pitch = (real)cos(pitch);

		real_point3d_set(&arg_0e6828, (real)cos(yaw) * cosine_pitch * amount + arg_0e6828.x,
			(real)sin(yaw) * cosine_pitch * amount + arg_0e6828.y,
			(real)sin(pitch) * amount + arg_0e6828.z);
	}
	*aim_point = arg_0e6828;
}

void function_b9fc0(long object_index, vector3f *forward, vector3f *up);
bool __stdcall function_bc1d0(long object_index, point3f const *point, long ignore_unit_index);
struct s_damage_owner;
void function_bc190(long object_index, s_damage_owner *owner);
void __stdcall function_a84e0(long projectile_index, short *material_index, vector3f const *vector, dword flags);
void __stdcall function_1ca690(long object_index, void const *data, long a, long b, long c);
point3f *function_b9dd0(long object_index, point3f *result);
real function_259a0(dword *seed);
void contrail_update(long contrail_index, bool detach, real dt);
void object_widgets_new(long object_index);
extern point3f *g_468788;

/* damage.cpp's damage event and its owner (the fields read here) */
struct s_damage_owner
{
	long player_index;
	long object_index;
	short team;
};

struct s_type_1e6529
{
	long definition_index;
	dword flags;
	s_damage_owner owner;
	long unknown14;
	long unknown18;
	s_location location;
	point3f position;
	point3f origin;
	vector3f direction;
	vector3f node_direction;
	real scale;
	byte unknown58[0x78 - 0x58];
	real unknown78;
	short material_index;
	short unknown7e;
	void const *unknown80;
	byte unknown84;
	byte unknown85[3];
};

void function_d6660(s_type_1e6529 *data, long definition_index);
void __stdcall function_d7b80(s_type_1e6529 *data, long object_index, short node_index, short unknown0c, short region_entry_index,
	vector3f const *unknown14);
long __stdcall function_d6c80(s_type_1e6529 *data, long ignore_object_index);

/* the down probe for a surface under a detonation (0x45326c) */
vector3f const g_45326c[1] = { { 0.0f, 0.0f, -1.0f } };

/* detonates a projectile: (once) its sound, contrail, direct and area
   damage, impact or airburst effects, and the effects of the material
   under it */
// score 527 (was 527): Frame 0x184 agrees; header-offset home, cached flags and probe/material homes differ.
// Native definition bit 533; projectile/child flag bits 527; definition bit plus parent field reads 527;
// common exit 527; saved-position role name 527; distinct direct/area damage names 527;
// probe-table pointer loop 530; probe declarations reversed 527; all native flag tests/stores 533;
// unsigned table index 539; material declared at entry 527; parent/Boolean declarations first 527.
// @retail 0xfc330
void __stdcall function_fc330(long projectile_index, bool detach_contrail, real contrail_time)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);

	if (TEST_FIELD_BIT((*(dword *)&projectile->flags >> 12) & 1))
		return;

	s_projectile_definition *definition = PROJECTILE_DEFINITION_GET(projectile->tag_index);
	byte *definition_bytes = (byte *)definition;
	real damage_scale = *(real *)((byte *)projectile + 0x188);
	bool attached = false;
	long attached_parent = NONE;
	bool airburst = true;
	long parent_index = *(long *)((byte *)projectile + 0x14);

	*(dword *)&projectile->flags |= 0x1000;
	if ((definition->flags & 8) && !(TEST_FIELD_BIT((*(dword *)&projectile->flags >> 6) & 1)) && parent_index != NONE)
	{
		byte *parent = (byte *)PROJECTILE_GET(parent_index);
		long stuck_count = 0;

		if (!(TEST_FIELD_BIT((*(byte *)(parent + 0x10a) >> 2) & 1)))
		{
			for (long child_index = *(long *)(parent + 0x10); child_index != NONE; )
			{
				s_projectile *child = PROJECTILE_GET(child_index);

				if (child->tag_index == projectile->tag_index && !(TEST_FIELD_BIT((*(dword *)&child->flags >> 6) & 1)))
					stuck_count++;
				child_index = *(long *)((byte *)child + 0xc);
			}
		}

		short maximum = *(short *)(definition_bytes + 0xe6);

		if (*(parent + 0xaa) == 0 && maximum && (short)stuck_count > maximum &&
			!(*(long *)(parent + 0x13c) != NONE && g_4e6948->state == 1))
		{
			point3f center = *g_468788;

			for (long child_index = *(long *)(parent + 0x10); child_index != NONE; )
			{
				s_projectile *child = PROJECTILE_GET(child_index);

				if (child->tag_index == projectile->tag_index && !(TEST_FIELD_BIT((*(dword *)&child->flags >> 6) & 1)))
				{
					if ((short)stuck_count <= *(short *)(definition_bytes + 0xe6))
					{
						point3f point;

						*(dword *)&child->flags |= 0x40;
						*(dword *)&child->flags |= 0x1000;
						child->unknown158 = 0.0f;
						child->unknown160 = 0.0f;
						function_b9dd0(child_index, &point);
						center.x += point.x;
						center.y += point.y;
						center.z += point.z;
					}
					else
					{
						child->unknown158 = function_259a0(&g_4e7408->unknown0) * child->unknown158;
						child->unknown160 = function_259a0(&g_4e7408->unknown0) * child->unknown160;
					}
					stuck_count--;
				}
				child_index = *(long *)((byte *)child + 0xc);
			}

			real inverse = 1.0f / (real)*(short *)(definition_bytes + 0xe6);
			point3f position;

			center.x *= inverse;
			center.y *= inverse;
			center.z *= inverse;
			attached_parent = *(long *)((byte *)projectile + 0x14);
			function_b9a90(attached_parent);
			position = *(point3f *)((byte *)projectile + 0x64);
			function_b75a0(projectile_index, &center, NULL, NULL, NULL, false);
			function_bc1d0(projectile_index, &position, *(long *)((byte *)projectile + 0x14));
			attached = true;
		}
	}

	point3f position;
	vector3f forward;
	vector3f up;

	function_b9dd0(projectile_index, &position);
	function_b9fc0(projectile_index, &forward, &up);
	if (*(long *)(definition_bytes + 0x124) != NONE)
	{
		s_sound_position sound_position;

		sound_position.position = position;
		sound_position.compressed_forward = vector3d_compress(&forward);
		sound_position.velocity = *g_4687a4;
		sound_position.location = *(s_location *)((byte *)projectile + 0x28);
		function_1895f0(&sound_position, 1.0f, *(long *)(definition_bytes + 0x124));
	}
	if (detach_contrail)
	{
		long contrail_index = function_fd410(projectile_index);

		if (contrail_index != NONE)
			contrail_update(contrail_index, false, g_510c54->rate - contrail_time);
	}

	if (g_4e6948->mode != 4)
	{
		long damage_index;

		if (TEST_FIELD_BIT((*(dword *)&projectile->flags >> 10) & 1))
			damage_index = *(long *)(definition_bytes + 0x160);
		else if (attached)
			damage_index = *(long *)(definition_bytes + 0x130);
		else
			damage_index = *(long *)(definition_bytes + 0x10c);

		long object_index = attached ? attached_parent : *(long *)((byte *)projectile + 0x14);

		if (object_index != NONE && damage_index != NONE)
		{
			s_type_1e6529 data;

			data.material_index = NONE;
			function_d6660(&data, damage_index);
			data.unknown84 = (*(definition_bytes + 0x128) & 0x3f) | 0x80;
			data.flags |= 0x1008;
			data.scale = damage_scale;
			function_b9fc0(projectile_index, &data.direction, NULL);
			function_b9dd0(projectile_index, &data.position);
			data.origin = data.position;
			data.unknown7e = *(short *)((byte *)projectile + 0x1a8);
			function_bc190(projectile_index, &data.owner);
			function_d7b80(&data, object_index, NONE, NONE, NONE, NULL);
		}
	}

	short material_index = *(short *)((byte *)projectile + 0x132);
	real probe_length = *(real *)(definition_bytes + 0x134);
	dword effect_flags = 0;

	if (probe_length == 0.0f || *(long *)((byte *)projectile + 0x14) != NONE || (projectile->unknownc1 & 1))
	{
		airburst = false;
	}
	else
	{
		material_index = g_47d8e0;
		for (long i = 0; i < 1; i++)
		{
			s_collision_result_1697c0 collision;
			vector3f vector;

			vector.i = g_45326c[i].i * probe_length;
			vector.j = g_45326c[i].j * probe_length;
			vector.k = g_45326c[i].k * probe_length;
			collision.unknown24 = NONE;
			if (function_1697c0(0x2480000f, &position, &vector, projectile_index, NONE, &collision))
			{
				material_index = collision.unknown24;
				airburst = false;
				break;
			}
		}
	}

	long effect_index;

	if (attached)
		effect_index = *(long *)(definition_bytes + 0x114);
	else if (airburst)
		effect_index = *(long *)(definition_bytes + 0xf4);
	else
		effect_index = *(long *)(definition_bytes + 0xfc);

	s_projectile *current = PROJECTILE_GET(projectile_index);
	s_effect_owner owner;

	owner.unknown4 = *(long *)((byte *)current + 0xc8);
	owner.unknown0 = *(long *)((byte *)current + 0xc4);
	owner.unknown8 = *(short *)((byte *)current + 0xc2);
	if (material_index != NONE && !(TEST_FIELD_BIT((*(dword *)&projectile->flags >> 9) & 1)))
	{
		*(dword *)&projectile->flags |= 0x200;
		effect_flags = 4;
		function_fcea0(projectile->tag_index, &position, &forward, &owner, material_index,
			(vector3f const *)((byte *)projectile + 0x134));
	}
	if (effect_index != NONE)
	{
		effect_flags |= attached ? 2 : 1;
		function_fcbc0(projectile->tag_index, &position, &forward, &owner, attached, airburst);
	}
	if (*(long *)(definition_bytes + 0x124) != NONE)
		effect_flags |= 8;
	if (g_4e6948->mode != 2 && g_4e6948->mode != 4 && effect_flags)
		function_a84e0(projectile_index, &material_index, (vector3f const *)((byte *)projectile + 0x134), effect_flags);

	if (g_4e6948->mode != 4)
	{
		long area_index;

		if (attached)
			area_index = *(long *)(definition_bytes + 0x11c);
		else if (TEST_FIELD_BIT((*(dword *)&projectile->flags >> 10) & 1))
			area_index = *(long *)(definition_bytes + 0x158);
		else
			area_index = *(long *)(definition_bytes + 0x104);

		if (area_index != NONE)
		{
			s_type_1e6529 data;
			s_projectile *object = PROJECTILE_GET(projectile_index);

			data.material_index = NONE;
			function_d6660(&data, area_index);
			data.unknown84 = *(definition_bytes + 0x128) & 0x3f;
			data.owner.object_index = *(long *)((byte *)object + 0xc8);
			data.owner.player_index = *(long *)((byte *)object + 0xc4);
			data.owner.team = *(short *)((byte *)object + 0xc2);
			data.scale = damage_scale;
			object_get_root_location(projectile_index, &data.location);
			data.origin = position;
			data.position = position;
			data.direction = forward;
			data.unknown7e = *(short *)((byte *)projectile + 0x1a8);
			function_d6c80(&data, NONE);
		}
	}
	object_widgets_new(projectile_index);
	function_1ca690(projectile_index, &position, 3, *(word *)(definition_bytes + 0xe4), 1);
}

/* the projectile object type (0x467f28): its name, group tag, datum size,
   its callbacks, the base object type (0x4678e8) and itself. The four slots
   that hold 0x175f40 (an empty function folded with c_game_engine::v10) and
   the base type are left NULL. */
void function_17c880(long contrail_index);
extern s_record_pool *g_4ea944;
s_object *function_badc0(long object_index, dword type_mask);
real function_1e9700(short row);
real magnitude3d(vector3f const *v);
vector3f *function_11d000(vector3f const *v, vector3f *out);
bool function_1897c0(long local_player_index, long unit_index, long tag_index, s_location const *location,
	point3f const *origin, vector3f const *direction);
void __stdcall function_1ca9f0(long object_index, long unknown);
void function_b7740(long object_index, vector3f const *linear_velocity, vector3f const *angular_velocity,
	bool unknown);
void function_1c4b00(long object_index, void *a, void *b, long c);
void function_b7360(long object_index);
void function_bba20(long object_index);
bool function_b9d20(long object_index);
void __stdcall function_bef30(long object_index, long a, long b, long c, bool d);
void function_b8b70(long object_index);
void __stdcall function_b8540(long object_index);
void function_faa60(vector3f *velocity, long projectile_index, s_collision_result_1697c0 const *collision,
	point3f *end_point, vector3f *displacement, short collision_count);

/* whether a projectile's arming is done: it has no arming time, or the
   time has passed */
PRIVATE inline bool projectile_armed(s_projectile const *projectile)
{
	return projectile->unknown164 == 0.0f || !(1.0f > projectile->unknown160);
}

/* whether an object is a predicted copy in a networked game */
PRIVATE inline bool projectile_object_predicted(long object_index)
{
	return g_4e6948->mode == 4 && PROJECTILE_GET(object_index)->unknownd4 != NONE;
}

/* a projectile's movement through a tick: guidance turning it toward its
   target, its speed curve and gravity, its range, then the line it moves
   along and what that line hits (up to 10 times), its orientation, and at
   the end its detonation or deletion */
// score 946 (was 1131): frame 0x110 versus 0x118, input/working registers, action-setter expansion and x87/SSE scheduling remain.
// Shared guidance producers 1149; shared basis crosses 1152; point producers 1131; common forward/up 1050; plus crosses 1046;
// role names 1131/1131; direct current field 1149; shared magnitude 1131; shared rotation crosses 1180; common left 1096;
// common rotated output 1096; both common 1069; existing state helper 1050/1131; complete guidance/roll inputs 994,
// with separate bases 1099; native count 1050, plus complete inputs 990 (four-point tail restored); caller noinline magnitude
// 1050/1131; all rotation inputs 1099; free-axis inputs 1011; complete inputs plus shared basis crosses 997;
// consistent dot/output inputs 946 (kept); consistent free-axis/all rotations 1011/1101; inputs before sine/cosine 1006.
// @retail 0xf8eb0
void function_f8eb0(long projectile_index, vector3f *displacement)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);
	s_projectile_definition *definition = PROJECTILE_DEFINITION_GET(projectile->tag_index);
	real remaining = g_510c54->rate;
	bool hit_player = false;
	short collision_count = 0;
	s_collision_result_1697c0 collision;

	if (!TEST_FIELD_BIT(projectile->flags.unknown1))
	{
		long contrail_index = function_fd410(projectile_index);

		if (contrail_index != NONE)
		{
			function_17c880(contrail_index);
			record_pool_release(g_4ea944, contrail_index);
		}
		function_fd460(projectile_index, NONE);
		projectile->attachment_index = NONE;
	}

	projectile->unknown160 += projectile->unknown164;
	projectile->unknown1aa++;

	bool armed;

	switch (definition->unknownc0)
	{
	case 1:
		armed = TEST_FIELD_BIT(projectile->flags.unknown2);
		break;
	case 2:
		armed = TEST_FIELD_BIT(projectile->flags.unknown4);
		break;
	case 3:
		armed = TEST_FIELD_BIT(projectile->flags.unknown11);
		break;
	default:
		armed = true;
		break;
	}
	if (TEST_FIELD_BIT(projectile->flags.unknown5) || TEST_FIELD_BIT(projectile->flags.unknown3) || armed)
	{
		if (!TEST_FIELD_BIT(projectile->flags.unknown5))
			projectile->flags.unknown5 = true;
		projectile->unknown158 += projectile->unknown15c;
		if (projectile->unknown158 >= 1.0f)
			function_faa30(projectile_index, 1);
	}

	while (remaining > 0.0f &&
		(projectile->action == 0 || projectile->action == 1 && !projectile_armed(projectile)) &&
		!TEST_FIELD_BIT(projectile->flags.unknown3) && !(projectile->unknownc1 & 1) &&
		!TEST_FIELD_BIT(projectile->flags.unknown14) && projectile->parent_object_index == NONE)
	{
		vector3f velocity = projectile->linear_velocity;
		vector3f average_velocity = projectile->linear_velocity;
		real speed = (real)sqrt(length_sq3f(&projectile->linear_velocity));
		real average_speed = speed;
		real starting_remaining = remaining;
		point3f end_point;
		vector3f forward;
		vector3f up;
		bool moved = false;

		collision.unknown24 = NONE;

		if (projectile->target.object_index != NONE && function_badc0(projectile->target.object_index, NONE) &&
			projectile->unknown18c > 0.0f)
		{
			byte *target = (byte *)PROJECTILE_GET(projectile->target.object_index);
			real turn_rate = projectile->unknown18c;

			if (((1 << target[0xaa]) & 3) && *(long *)(target + 0x13c) != NONE)
				turn_rate *= function_1e9700(19);

			point3f aim_point;
			vector3f to_aim;
			vector3f axis;
			vector3f const *current = &projectile->linear_velocity;

			function_f87f0(projectile_index, &aim_point);
			to_aim.i = aim_point.x - projectile->position.x;
			to_aim.j = aim_point.y - projectile->position.y;
			to_aim.k = aim_point.z - projectile->position.z;
			axis.i = current->j * to_aim.k - current->k * to_aim.j;
			axis.j = current->k * to_aim.i - to_aim.k * current->i;
			axis.k = to_aim.j * current->i - current->j * to_aim.i;
			if ((TEST_FIELD_BIT(definition->flag_bits.unknown8) ||
				current->k * to_aim.k + current->j * to_aim.j + current->i * to_aim.i > 0.0f) &&
				function_30bf0(&axis) > 0.0f)
			{
				real angle = turn_rate * remaining;
				real sine = (real)sin(angle);
				real cosine = (real)cos(angle);
				real incoming_i = velocity.i;
				real incoming_j = velocity.j;
				real incoming_k = velocity.k;

				real along = (axis.i * incoming_i + axis.k * incoming_k + axis.j * incoming_j) * (1.0f - cosine);

				velocity.i = cosine * incoming_i + along * axis.i - (axis.k * incoming_j - axis.j * incoming_k) * sine;
				velocity.j = cosine * incoming_j + axis.j * along - (axis.i * incoming_k - axis.k * incoming_i) * sine;
				velocity.k = cosine * incoming_k + axis.k * along - (axis.j * incoming_i - axis.i * incoming_j) * sine;

				if (TEST_FIELD_BIT(definition->flag_bits.unknown6))
				{
					forward = to_aim;
					up = projectile->up;
					vector3f left;

					function_30bf0(&forward);
					left.i = up.k * forward.j - forward.k * up.j;
					left.j = forward.k * up.i - up.k * forward.i;
					left.k = up.j * forward.i - forward.j * up.i;
					up.i = left.j * forward.k - left.k * forward.j;
					up.j = left.k * forward.i - forward.k * left.i;
					up.k = forward.j * left.i - left.j * forward.i;
					function_30bf0(&up);
					function_b75a0(projectile_index, NULL, &forward, &up, NULL, false);
				}
			}
		}

		if (!TEST_FIELD_BIT(projectile->flags.unknown8))
		{
			if (definition->unknown17c != 0.0f && definition->unknown180 != 0.0f)
			{
				real t = PIN((projectile->unknown194 - definition->unknown18c * projectile->unknown16c) *
					definition->unknown194, 0.0f, 1.0f);
				vector3f forward = projectile->forward;
				real new_speed = ((definition->unknown180 - definition->unknown17c) * t + definition->unknown17c) *
					projectile->unknown16c;
				real delta = new_speed - projectile->unknown190;

				projectile->unknown190 = new_speed;
				projectile->unknown194 += new_speed * remaining;
				velocity.i = forward.i * delta + velocity.i;
				velocity.j = forward.j * delta + velocity.j;
				velocity.k = forward.k * delta + velocity.k;
				average_velocity.i = (projectile->linear_velocity.i + velocity.i) * 0.5f;
				average_velocity.j = (projectile->linear_velocity.j + velocity.j) * 0.5f;
				average_velocity.k = (projectile->linear_velocity.k + velocity.k) * 0.5f;
				average_speed = (magnitude3d(&velocity) + speed) * 0.5f;
				if (definition->maximum_range == 0.0f && definition->unknownd8 == 0.0f &&
					projectile->distance_traveled >= projectile->speed)
				{
					function_faa30(projectile_index, 2);
				}
			}
			else
			{
				velocity = projectile->linear_velocity;
				average_velocity = projectile->linear_velocity;
			}
		}

		real gravity;

		if (TEST_FIELD_BIT(projectile->object_flags.unknown3))
			gravity = definition->unknown170 * *(real *)g_51e9c4;
		else
			gravity = definition->gravity_scale * *(real *)g_51e9c4;

		real fall = gravity * remaining;

		velocity.k -= fall;
		average_velocity.k -= fall * 0.5f;

		real fraction;

		if (definition->maximum_range != 0.0f &&
			projectile->distance_traveled + average_speed * remaining > definition->maximum_range)
		{
			if (average_speed != 0.0f)
				fraction = (definition->maximum_range - projectile->distance_traveled) / (average_speed * remaining);
			else
				fraction = 0.0f;
			function_faa30(projectile_index, 1);
		}
		else
		{
			fraction = 1.0f;
		}

		real step = fraction * remaining;

		end_point.x = step * average_velocity.i + projectile->position.x;
		end_point.y = step * average_velocity.j + projectile->position.y;
		end_point.z = step * average_velocity.k + projectile->position.z;

		if (collision_count == 10)
		{
			function_faa30(projectile_index, 1);
			remaining = 0.0f;
		}
		else if (projectile->action == 2)
		{
			remaining = 0.0f;
		}
		else
		{
			moved = true;
			if (function_fc030(projectile_index, &end_point, &collision))
			{
				real t = collision.t;

				remaining = (1.0f - t) * remaining;
				velocity.i = (velocity.i - projectile->linear_velocity.i) * t + projectile->linear_velocity.i;
				velocity.j = (velocity.j - projectile->linear_velocity.j) * t + projectile->linear_velocity.j;
				velocity.k = (velocity.k - projectile->linear_velocity.k) * t + projectile->linear_velocity.k;
				projectile->flags.unknown11 = true;
				if (collision.normal.k > 0.3f)
					projectile->flags.unknown2 = true;
				function_1ca9f0(projectile_index, collision.object_index);
				collision_count++;
				projectile->ignore_object_index = NONE;
				function_faa60(&velocity, projectile_index, &collision, &end_point, displacement, collision_count);
				function_1ca690(projectile_index, &collision, 2, definition->unknownc2, 1);
			}
			else
			{
				remaining = 0.0f;
			}
		}

		if (!TEST_FIELD_BIT(projectile->flags.unknown3) && moved)
		{
			vector3f movement;
			forward = projectile->forward;
			up = projectile->up;

			movement.i = end_point.x - projectile->position.x;
			movement.j = end_point.y - projectile->position.y;
			movement.k = end_point.z - projectile->position.z;
			projectile->distance_traveled += magnitude3d(&movement);

			if (!hit_player && definition->unknown13c != NONE)
			{
				for (long local_player_index = 0; local_player_index < 4; local_player_index++)
				{
					if (function_1897c0(local_player_index, projectile->unknownc4, definition->unknown13c,
						&collision.location, &projectile->position, &movement))
					{
						hit_player = true;
					}
				}
			}

			if ((definition->flags & 1) && (velocity.i != 0.0f || velocity.j != 0.0f || velocity.k != 0.0f))
			{
				forward = velocity;
				if (function_30bf0(&forward) > 0.0f)
				{
					vector3f left;

					left.i = up.j * forward.k - up.k * forward.j;
					left.j = up.k * forward.i - forward.k * up.i;
					left.k = forward.j * up.i - up.j * forward.i;
					up.i = left.k * forward.j - left.j * forward.k;
					up.j = forward.k * left.i - left.k * forward.i;
					up.k = left.j * forward.i - forward.j * left.i;
					if (function_30bf0(&up) == 0.0f)
						function_30bf0(function_11d000(&forward, &up));
				}
				else
				{
					forward = projectile->forward;
				}

				real incoming_i = up.i;
				real incoming_j = up.j;
				real incoming_k = up.k;

				real along = (forward.i * incoming_i + forward.k * incoming_k + incoming_j * forward.j) * (1.0f - projectile->rotation_cosine);

				up.i = along * forward.i + incoming_i * projectile->rotation_cosine -
					(forward.k * incoming_j - incoming_k * forward.j) * projectile->rotation_sine;
				up.j = forward.j * along + incoming_j * projectile->rotation_cosine -
					(incoming_k * forward.i - forward.k * incoming_i) * projectile->rotation_sine;
				up.k = forward.k * along + incoming_k * projectile->rotation_cosine -
					(forward.j * incoming_i - incoming_j * forward.i) * projectile->rotation_sine;
			}
			else if (TEST_FIELD_BIT(projectile->flags.rotating))
			{
				vector3f const *axis = &projectile->rotation_axis;
				vector3f rotated;
				real along;

				along = (forward.k * axis->k + forward.j * axis->j + axis->i * forward.i) * (1.0f - projectile->rotation_cosine);
				rotated.i = axis->i * along + projectile->rotation_cosine * forward.i -
					(forward.j * axis->k - forward.k * axis->j) * projectile->rotation_sine;
				rotated.j = forward.j * projectile->rotation_cosine + along * axis->j -
					(forward.k * axis->i - axis->k * forward.i) * projectile->rotation_sine;
				rotated.k = forward.k * projectile->rotation_cosine + along * axis->k -
					(axis->j * forward.i - forward.j * axis->i) * projectile->rotation_sine;
				forward = rotated;

				along = (up.k * axis->k + up.j * axis->j + axis->i * up.i) * (1.0f - projectile->rotation_cosine);
				rotated.i = axis->i * along + projectile->rotation_cosine * up.i -
					(up.j * axis->k - up.k * axis->j) * projectile->rotation_sine;
				rotated.j = up.j * projectile->rotation_cosine + along * axis->j -
					(up.k * axis->i - axis->k * up.i) * projectile->rotation_sine;
				rotated.k = up.k * projectile->rotation_cosine + along * axis->k -
					(axis->j * up.i - up.j * axis->i) * projectile->rotation_sine;
				up = rotated;
			}

			function_b75a0(projectile_index, &end_point, &forward, &up, &collision.location, false);
			function_b7740(projectile_index, &velocity, NULL, false);
			function_1c4b00(projectile_index, &velocity, NULL, 1);
			if (velocity.k * velocity.k + velocity.j * velocity.j + velocity.i * velocity.i > 0.0001f)
			{
				function_b9b90(projectile_index, false);
				function_b7360(projectile_index);
				function_bba20(projectile_index);
			}
			if (remaining > 0.0f && collision_count != 0)
			{
				long contrail_index = function_fd410(projectile_index);

				if (contrail_index != NONE)
					contrail_update(contrail_index, false, starting_remaining - remaining);
			}
		}
	}

	switch (projectile->action)
	{
	case 1:
		if (!projectile_armed(projectile))
			return;
		if (projectile_object_predicted(projectile_index))
			function_faa30(projectile_index, 2);
		else
			function_fc330(projectile_index, collision_count == 0, remaining);
		// fall through
	case 2:
		if (projectile_object_predicted(projectile_index))
		{
			s_projectile *object = PROJECTILE_GET(projectile_index);

			if (!(*(byte *)&object->object_flags & 1))
			{
				if (function_b9d20(projectile_index))
					function_bef30(projectile_index, 1, 0, 0, false);
				*(dword *)&object->object_flags |= 1;
				function_b8b70(projectile_index);
			}
		}
		else
		{
			function_b8540(projectile_index);
		}
		break;
	}
}

real function_11ce20(vector3f const *a, vector3f const *b);
real function_1201a0(vector3f *v, vector3f const *fallback);
void random_vector_in_cone(vector3f const *forward, vector3f *result, dword *seed, real min_angle, real max_angle);
struct s_globals_element;
s_globals_element *function_188690(short index);
void function_184060(long unknown3c, long unknown59, s_type_1e6529 *data, long unknown50);
void function_a85c0(long projectile_index, s_collision_result_1697c0 const *collision, vector3f const *direction,
	bool unknown, real scale_a, real scale_b);

/* the effect parameters of a projectile's impact effects: its owner, and
   the object it hit */
PRIVATE __forceinline void projectile_impact_effect_parameters(s_effect_parameters *parameters, long projectile_index,
	long tag_index, s_collision_result_1697c0 const *collision)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);

	parameters->tag_index = tag_index;
	parameters->owner.unknown4 = *(long *)((byte *)projectile + 0xc8);
	parameters->owner.unknown8 = *(short *)((byte *)projectile + 0xc2);
	parameters->owner.unknown0 = projectile->unknownc4;
	if (collision->type == 4)
	{
		parameters->flags |= 1;
		parameters->object_index = collision->object_index;
		parameters->unknown18 = collision->node_index;
	}
}

/* a projectile's collision: damage to what it hit, the response of the
   material hit (detonating, bouncing, reflecting, attaching), the impact
   effects and sounds, then the projectile's action */
// score 1197 (was 1197): Frame 0x220 versus 0x218, stacked velocity and normalization/effect scheduling remain.
// Earlier 1162 (was 1185): frame 0x220 versus 0x218, velocity on the stack versus eax, input normalization and effect scheduling remain.
// Independent damage scopes 1162 (kept); direct sound/late effects/both/scoped both 1223/1185/1223/1199; role names/chains/external normalization 1185/1185/1323.
// Full count/potential conjunction/input aggregate/assignment helper/uninitialized direction 1201/1216/1198/1198/1185; array aggregates/plain loop/unsigned index 1166/1348/1185.
// Those eight forms on damage scopes 1169/1199/1199/1194/1162/1162/1348/1162; coordinated velocity positions 1185/1185/1185/1201/1201.
// Input copy/full normalization/intermediate normalization first 1186/1186/1198; on damage scopes 1163/1159/1170 (1159 three-point tail restored).
// DH round 218: existing sound producer 1272; with complete position aggregate, readonly aggregate,
// sound record name, aggregate/name, both sound/effect array names, aggregate/array names and
// caller inline declaration all 1272. Every form 4728/4828 bytes, frame unchanged; all restored.
// DH round 232: Producer-phase effect/sound/both array defaults 1191/1191/1157;
// primary-scale default at selection 1189, with effect/sound/both defaults 1205/1205/1211;
// both arrays and both scale defaults at their real phases 1201. Five-point 1157 at
// 4832/4828 bytes rejected; primary-scale lifetime recovers frame 0x21c but regresses.
// All executable forms restored; frame 0x220/0x218 and velocity transport remain.
// Round 248: effect-result case orders 1162/1161/1162/1161; terminal action orders 1174/1162;
// paired dispatch/action orders 1174/1161. Native tag-array, direction-workspace, both,
// damage-record, array/record and all paired role names 1162 each. All restored; frame,
// velocity contract, native request homes and setter expansion still differ.
// DH round 234: shared reflection scaler/live material flags/readonly response combinations
// 1179/1162/1179/1162/1179/1162/1179; all with the real normal receiver 1194 at retail
// length. All restored; frame 0x220/0x218 and velocity/request homes remain.
// Round 402: distance fraction/path/shared difference, seven subsets,
// 1207/1207/1162/1207/1207/1207/1207; complete graph with native product bounds 1199.
// Damage tag at every use with live mode/response flags, four subsets, 1203/1201/1203/1201;
// those with native dword collision count 1201/1181/1201/1181. All restored;
// frame 0x220/0x218, velocity contract and request homes remain.
// Round 284: Independent effect/sound long or dword counters at their producers, four
// combinations 1162 each; those with the collision routine declared inline 1162 each.
// Original shared counter and declaration restored; frame and velocity contract remain.
// Round 430: marker/request producer declarations and complete effect/sound phase scope,
// seven subsets 1162/1162/1162/1162/1162/1208/1208; complete graph with request before spatial input 1208.
// Last three recover frame 0x218 but shrink to 4768/4828 bytes; velocity stays stacked. Original restored.
// Round 213: existing final-velocity friction/noise/rest helper producers, seven subsets,
// 1162 each; complete graph with reflected receiver setter 1162. Original body restored;
// frame 0x220/0x218 and velocity/request homes remain.
// Round 440: Native normal/point/both geometry values 1329/1262/1362, producer and entry storage alike;
// both library-copy forms 1362. Frames 0x228/0x234 regress; original geometry reads and every body statement restored.
// Native material-value callee and consistent call construction 1190; restored with the exact-only callee.
// @retail 0xfaa60
void function_faa60(vector3f *velocity, long projectile_index, s_collision_result_1697c0 const *collision,
	point3f *end_point, vector3f *displacement, short collision_count)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);
	s_projectile_definition *definition = PROJECTILE_DEFINITION_GET(projectile->tag_index);
	short material_index = collision->unknown24;
	vector3f direction = *velocity;
	real speed = normalize_inline(&direction);
	real scale_a = 1.0f;
	real scale_b = 0.0f;
	long sounds[3];
	long effects[3];
	s_effect_marker markers[6];
	s_effect_parameters parameters;
	long i;

	effects[0] = NONE;
	effects[1] = NONE;
	effects[2] = NONE;
	sounds[0] = NONE;
	sounds[1] = NONE;
	sounds[2] = NONE;
	if (speed == 0.0f)
		direction = *g_4687b0;

	real damage_scale = projectile->unknown188;

	if (TEST_FIELD_BIT((definition->flags >> 4) & 1))
	{
		vector3f offset;

		offset.i = collision->point.x - projectile->position.x;
		offset.j = collision->point.y - projectile->position.y;
		offset.k = collision->point.z - projectile->position.z;
		damage_scale = PIN(damage_scale * (1.0f - ((real)sqrt(length_sq3f(&offset)) +
			projectile->distance_traveled) / projectile->speed), 0.0f, 1.0f);
	}

	long mode = g_4e6948->mode;

	if (mode >= 4 && mode <= 5 && collision_count == 1 && collision->type == 4 &&
		TEST_FIELD_BIT(definition->flag_bits.drifts) && !(TEST_FIELD_BIT((*(dword *)&projectile->flags >> 13) & 1)) &&
		collision->object_index != NONE && PROJECTILE_GET(collision->object_index)->unknownd4 != NONE)
	{
		*(long *)((byte *)projectile + 0x150) = collision->object_index;
		*(short *)((byte *)projectile + 0x154) = collision->node_index;
		*(dword *)&projectile->flags |= 0x2000;
	}

	long damage_definition_index = *(long *)((byte *)definition + 0x14c);

	if (collision->type == 4 && damage_definition_index != NONE)
	{
		if (g_4e6948->mode == 4)
		{
			long object_index = collision->object_index;

			if (object_index != NONE)
			{
				byte *object = (byte *)PROJECTILE_GET(object_index);
				real vitality = *(real *)(object + 0xf0);

				if (vitality > 0.0f && !(TEST_FIELD_BIT((object[0x10a] >> 2) & 1)) && *(long *)((byte *)projectile + 0xc8) != object_index)
				{
					long model_index = *(long *)(g_4e3b44[*(long *)object & 0xffff].bytes + 0x38);

					if (model_index != NONE)
					{
						byte *model = g_4e3b44[model_index & 0xffff].bytes;

						if (*(long *)(model + 0x60) > 0)
						{
							material_index = *(short *)(*(byte **)(model + 0x64) + 0xcc);
							scale_b = vitality;
						}
					}
				}
			}
		}
		else
		{
			s_type_1e6529 damage;

			function_d6660(&damage, damage_definition_index);
			damage.material_index = NONE;
			damage.flags |= 8;
			damage.unknown84 = *((byte *)definition + 0x128) & 0x3f;
			damage.scale = damage_scale;
			damage.unknown7e = *(short *)((byte *)projectile + 0x1a8);
			function_bc190(projectile_index, &damage.owner);
			damage.origin = collision->point;
			damage.position = collision->point;
			damage.direction = *velocity;
			function_30bf0(&damage.direction);
			function_d7b80(&damage, collision->object_index, collision->node_index, collision->unknown44,
				collision->unknown5a, &collision->normal);
			if (function_188690(damage.material_index))
				material_index = damage.material_index;
			scale_b = damage.unknown78;
		}
	}

	*(short *)((byte *)projectile + 0x132) = material_index;
	*(vector3f *)((byte *)projectile + 0x134) = collision->normal;

	s_projectile_material_response *response = projectile_get_material_response(definition, material_index);
	real impact_cosine = 0.0f - (collision->normal.k * velocity->k + collision->normal.j * velocity->j +
		velocity->i * collision->normal.i);

	if (response->velocity_noise > 0.0f)
		impact_cosine -= function_x82e52f(&g_4e7408->unknown0, __FILE__, __LINE__) * response->velocity_noise;

	real angle = function_11ce20(&collision->normal, &direction) - 1.5707964f;

	if (response->angular_noise > 0.0f)
		angle += projectile_random_range(-response->angular_noise, response->angular_noise);

	bool potential = response->chance_fraction > 0.0f;

	if (response->angle_upper_bound != 0.0f)
	{
		potential = potential && angle >= response->angle_lower_bound && response->angle_upper_bound >= angle;
	}
	if (response->velocity_upper_bound != 0.0f)
	{
		potential = potential && speed >= response->velocity_lower_bound && response->velocity_upper_bound >= speed;
	}

	word response_flags = response->response_flags;

	if (response_flags & 2)
	{
		potential = potential && (collision->type != 4 ||
			!((1 << ((s_projectile_header *)g_4e0300->data)[collision->object_index & 0xffff].unknown00[3]) & 3));
	}
	if (response_flags & 1)
	{
		potential = potential && collision->type == 4 &&
			((1 << ((s_projectile_header *)g_4e0300->data)[collision->object_index & 0xffff].unknown00[3]) & 3);
	}

	short response_type;

	if (1.0f > response->chance_fraction ?
		potential && response->chance_fraction > function_x82e52f(&g_4e7408->unknown0, __FILE__, __LINE__) : potential)
	{
		response_type = response->potential_response;
	}
	else
	{
		response_type = response->response;
	}

	long effect_type;

	switch (response_type)
	{
	case 0:
		*(dword *)&projectile->flags |= 0x200;
		effect_type = 0xf;
		break;
	case 1:
		effect_type = 0x10;
		break;
	case 2:
		effect_type = 0x11;
		break;
	case 3:
		effect_type = 0x12;
		break;
	case 4:
		effect_type = 0x13;
		break;
	case 5:
	case 6:
		effect_type = 0x14;
		break;
	default:
		goto no_effects;
	}
	function_fd7d0(projectile_index, &collision->point, (byte const *)definition, effect_type, material_index,
		&sounds[0], &sounds[1], &sounds[2], &effects[0], &effects[1], &effects[2]);
no_effects:

	if (g_4e6948->mode != 4 && (collision->type == 1 || collision->type == 3) && (collision->unknown58 & 8))
	{
		s_type_1e6529 damage;

		function_d6660(&damage, damage_definition_index);
		damage.material_index = NONE;
		damage.flags |= 8;
		damage.origin = collision->point;
		damage.position = collision->point;
		damage.direction = *velocity;
		function_30bf0(&damage.direction);
		damage.unknown84 = *((byte *)definition + 0x128) & 0x3f;
		damage.material_index = collision->unknown24;
		damage.unknown80 = projectile_get_material_response(definition, collision->unknown24);
		damage.location = collision->location;
		damage.unknown7e = *(short *)((byte *)projectile + 0x1a8);
		damage.scale = damage_scale;
		function_184060(collision->unknown3c, collision->unknown59, &damage, collision->unknown50);
	}

	*end_point = collision->point;
	if (response_type == 2)
	{
		if (collision->type == 2)
		{
			if (!TEST_FIELD_BIT(projectile->object_flags.unknown3))
				*(dword *)&projectile->object_flags |= 8;
			else
				*(dword *)&projectile->object_flags &= ~8;
			function_fbfd0(projectile_index);
			end_point->x -= collision->normal.i * 0.001f;
			end_point->y -= collision->normal.j * 0.001f;
			end_point->z -= collision->normal.k * 0.001f;
		}
		else if (collision->type == 4)
		{
			real friction = 1.0f - response->initial_friction;

			velocity->i = velocity->i * friction;
			velocity->j = friction * velocity->j;
			velocity->k = friction * velocity->k;
			projectile->ignore_object_index = collision->object_index;
		}
		else
		{
			if (definition->unknownd8 != 0.0f)
			{
				projectile->flags.unknown2 = true;
				projectile->flags.unknown11 = true;
				projectile->flags.unknown4 = true;
				response_type = 3;
			}
			else
			{
				response_type = 0;
			}
			*velocity = *g_4687a4;
		}
	}
	else if (response_type == 5 || response_type == 4)
	{
		if (response_type == 5)
			projectile->unknown188 = 0.0f;

		real dot = displacement->j * collision->normal.j + collision->normal.i * displacement->i +
			displacement->k * collision->normal.k;
		vector3f perpendicular;
		real parallel_scale = 1.0f - response->parallel_friction;
		real perpendicular_scale = 1.0f - response->perpendicular_friction;
		vector3f reflected;

		perpendicular.i = dot * collision->normal.i;
		perpendicular.j = dot * collision->normal.j;
		perpendicular.k = dot * collision->normal.k;
		reflected.i = (displacement->i - perpendicular.i) * parallel_scale - perpendicular.i * perpendicular_scale;
		reflected.j = (displacement->j - perpendicular.j) * parallel_scale - perpendicular.j * perpendicular_scale;
		reflected.k = (displacement->k - perpendicular.k) * parallel_scale - perpendicular.k * perpendicular_scale;
		velocity->i = reflected.i - displacement->i + velocity->i;
		velocity->j = reflected.j - displacement->j + velocity->j;
		velocity->k = reflected.k - displacement->k + velocity->k;
		*displacement = reflected;
	}
	else
	{
		*velocity = *g_4687a4;
	}

	if (response->angular_noise > 0.0f)
		random_vector_in_cone(velocity, velocity, &g_4e7408->unknown0, 0.0f, response->angular_noise);
	if (response->velocity_noise != 0.0f)
	{
		real length = function_30bf0(velocity);

		if (length != 0.0f)
		{
			length = projectile_random_range(-response->velocity_noise, response->velocity_noise) + length;
			velocity->i = length * velocity->i;
			velocity->j = length * velocity->j;
			velocity->k = length * velocity->k;
		}
	}

	real speed_squared = length_sq3f(velocity);

	if (response_type != 3 && definition->unknowndc * definition->unknowndc > speed_squared)
		function_faa30(projectile_index, 1);
	if (0.09f > speed_squared)
	{
		projectile->flags.unknown4 = true;
		if (collision->normal.k > 0.3f &&
			(collision->object_index == NONE || collision->object_index == *(long *)((byte *)projectile + 0xb8)))
		{
			velocity->i = 0.0f;
			velocity->j = 0.0f;
			velocity->k = 0.0f;
			if (collision->object_index == *(long *)((byte *)projectile + 0xb8))
				projectile->flags.unknown14 = true;
			else
				function_b9b90(projectile_index, true);
			if (definition->unknownd8 == 0.0f)
				function_faa30(projectile_index, 1);
		}
	}

	switch (response->scale_effects_by)
	{
	case 0:
		scale_a = damage_scale;
		break;
	case 1:
		scale_a = PIN(angle * 0.63661975f, 0.0f, 1.0f);
		break;
	}

	vector3f local_cfa056 = direction;
	bool not_predicted = !projectile_object_predicted(projectile_index);
	bool detonated = response_type == 0;
	bool stopped = response_type == 3;
	bool settled = !TEST_FIELD_BIT(projectile->flags.unknown5) && (TEST_FIELD_BIT(projectile->flags.unknown4) || stopped);

	function_1201a0(&local_cfa056, g_4687bc);
	function_1763a0(&collision->point, &local_cfa056, markers, &collision->normal);
	scale_a = PIN(scale_a, 0.0f, 1.0f);
	scale_b = PIN(scale_b, 0.0f, 1.0f);
	projectile_effect_parameters_initialize(&parameters);
	parameters.scale_a = scale_a;
	parameters.scale_b = scale_b;
	parameters.markers = markers;
	parameters.marker_count = 6;
	parameters.source = (s_effect_source *)collision;

	if (g_4e6948->mode != 4 || not_predicted || !detonated && !stopped)
	{
		if (impact_cosine > 0.25f)
		{
			bool created = false;

			for (i = 0; i < 3; i++)
			{
				if (effects[i] != NONE)
				{
					parameters.flags &= ~4;
					projectile_impact_effect_parameters(&parameters, projectile_index, effects[i], collision);
					effect_new_from_parameters(&parameters);
					created = true;
				}
			}
			if (*(long *)((byte *)definition + 0x144) != NONE && material_index != g_47d8e0)
			{
				parameters.flags |= 4;
				projectile_impact_effect_parameters(&parameters, projectile_index,
					*(long *)((byte *)definition + 0x144), collision);
				effect_new_from_parameters(&parameters);
				created = true;
			}
			for (i = 0; i < 3; i++)
			{
				long sound_index = sounds[i];

				if (sound_index != NONE)
				{
					s_sound_position position;
					s_sound_request request;

					position.position = collision->point;
					position.compressed_forward = vector3d_compress(&direction);
					position.velocity = *g_4687a4;
					position.location = collision->location;
					request.location.unknown02 = 0;
					request.location.flags = 0;
					request.location.spatial = position;
					request.location.scale = 1.0f;
					request.location.audible = 1;
					request.location.requested_audible = 1;
					request.location.unknown08 = 0;
					request.object_index = NONE;
					request.platform_playback = NONE;
					request.marker = NULL;
					request.source = NULL;
					request.variant = NULL;
					function_189fe0(&request, sound_index);
					created = true;
				}
			}
			if ((settled || created) && g_4e6948->mode != 4 && projectile->unknownd4 != NONE && (detonated || stopped))
				function_a85c0(projectile_index, collision, &local_cfa056, settled, scale_a, scale_b);
		}

		if (!TEST_FIELD_BIT(projectile->flags.unknown5) && (TEST_FIELD_BIT(projectile->flags.unknown4) || response_type == 3))
		{
			parameters.flags |= 4;
			projectile_impact_effect_parameters(&parameters, projectile_index, *(long *)((byte *)definition + 0xec),
				collision);
			effect_new_from_parameters(&parameters);
		}
	}

	switch (response_type)
	{
	case 0:
		function_faa30(projectile_index, 1);
		break;
	case 1:
	case 6:
		function_faa30(projectile_index, 2);
		break;
	case 3:
		if (!projectile_object_predicted(projectile_index))
		{
			function_fd560(projectile_index, collision->type == 4 ? collision->object_index : NONE,
				collision->node_index, end_point, (vector3f const *)&collision->location);
		}
		break;
	}
}

struct s_object_type_definition_view
{
	char const *name;
	long group_tag;
	short datum_size;
	short unknown0a;
	long unknown0c;
	void *functions[29];
	void *parent;
	void *self;
	byte unknown8c[0xc8 - 0x8c];
};

extern s_object_type_definition_view g_467f28;

s_object_type_definition_view g_467f28 =
{
	"projectile",
	'proj',
	0x1ac,
	NONE,
	NONE,
	{
		NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, (void *)function_f8200,
		NULL, NULL, NULL, NULL,
		(void *)function_f8de0, NULL, NULL, (void *)function_fbe70,
		NULL, NULL, (void *)projectile_clear_target, NULL,
		NULL, (void *)function_fd3c0, NULL, NULL,
		NULL, NULL, NULL, NULL,
		NULL
	},
	NULL,
	&g_467f28
};
