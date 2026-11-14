// @flags /O2 /Ob1 /arch:SSE /Gr
/* PROJECTILES.CPP: projectiles

The projectile object type's callbacks (its
definition at 0x467f28), and its flight, collision, attachment and
detonation. */

#include "unknown_11c920.h"
#include "globals.h"
#include "impacts.h"
#include "data_array.h"
#include "unknown_0259d0.h"
#include "object_markers.h"
#include "normalize3d.h"
#include "vector_products.h"
#include "object_iterator.h"
#include "effects.h"
#include "sound_sources.h"
#include "impacts.h"
#include "bit_vector.h"
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

// MiniMax A (11-12): no new form, no lower score (3): the three differences are still the flag OR and its store around the fld st(0) and the axis.i load that retail makes after it; the angle declared after the flag store measures 10 and the axis through a pointer local measures 3 with the same code, so the body stands.
/* the rotation a spinning projectile makes each tick */
// MiniMax A (11-14): no new form, no lower score (3): only the rotating flag's OR and store and the first axis load
// are in the other order; the frame, the length and every other instruction agree.
// MiniMax H (11-14): no new form, no lower score (3): the flag's OR and store still complete after this build hoists the axis's first word above them; copying the axis with memcpy gives the same bytes, and storing the rotation axis one component at a time measures 71.
// MiniMax B (11-13): no new form, no lower score (3): retail still completes the rotating-flag OR and its store before the angle duplicate and the first axis load, while this build hoists that load above the flag OR and sinks the store below the duplicate; the shared normalizer takes its vector by pointer, so the axis copy's address escapes and the store cannot move, and letting the helper read its parameter through its address would give retail's tail but score 11 because the squared terms then come out in i, j, k order.
// MiniMax H (11-12): no new form, no lower score (3): the flag OR and store still cross the angle duplicate and the first axis load differently - ours hoists the axis load above the flag store and sinks the store below the duplicate, retail does the opposite.
// MiniMax G (11-11): no new form, no lower score (3); the three differences are one schedule: retail completes the rotating-flag
// OR and its store before the angle duplicate and the first axis load, while this build hoists that load above the flag store and
// sinks the store below the duplicate; the axis stored through a pointer local, the three components copied in the other orders,
// the flag set through the bit-vector setter and the angle declared after the flag store all score 3 or higher.
// MiniMax H (11-11): no new form, no lower score (3); retail completes the rotating-flag OR and store before the angle duplicate and the first axis load, while ours still hoists that load above the flag store, so the two sides differ in the order of those two instructions alone.
// MiniMax R (11-10): no new form, no lower score (3); retail completes the rotating-flag OR and store before the angle duplicate and the first axis load, ours still hoists that load above the flag store, and the sixteen source forms tried this round all compile to the same two differences.
// MiniMax A (11-11): no new form, no lower score (3); the three differences are all one schedule: retail completes the
// rotating-flag OR and its store before the angle duplicate (`fld st(0)`) and before the rate load, ours loads the
// rate first and sinks the store below the duplicate. The rate read at each use, the angle written at each use and
// the flag store as a bitfield all keep our order (3), and the forms that put the store first (the flag before the
// angle, 22) cost more. The original body is kept.
// MiniMax I (11-10): no new form, no lower score (3); letting the axis local's address escape puts retail's flag order back but squares the components in i, j, k order instead of retail's k, j, i (14), spelling the magnitude in i, j, k order (14) or naming the squares (14) does not change that, and the copy-in written component by component gives 63.
// MiniMax H (11-04): no new form, no lower score; retail keeps the rotating-flag OR and store ahead of the first axis load and ours hoists that load above the flag store.
// MiniMax A (11-03): no new form, no lower score.
// score 3 (was 3): this round: the rate computed after the flag write scores 10, and the axis stored
// after both trigonometry calls scores 21 with a shorter body (283 bytes); both were reverted.
// score 3 (was 3): retail completes the rotating-flag OR and store before the angle duplicate and
// the first axis load; ours hoists that load above the flag store and sinks the store below the
// duplicate. Everything else agrees; only a normalizer in another unit keeps retail's order.
// This round: the tail was re-ordered nine ways (the axis first, the axis last, the trigonometry
// first, the flag first, the flag as a bitfield set, the rate first, the angle written at each use,
// the axis between the trigonometry calls, the flag last) scoring 32/21/21/10/3/3/34/3/32; a
// helper taking the axis by reference compiles identically (3), and the permuter's 72 tries in
// 339 s found nothing below 3.
// This round: letting the axis local's address escape (a pointer local at the caller, or the
// helper reading its parameter through its address) puts retail's flag order back but squares the
// components in i, j, k order instead of retail's k, j, i (14 and 11); the magnitude expression's
// order does not change that (11 three ways, 12 with named squares), and a helper taking the
// vector through a pointer to the pointer does not compile against its other two callers.
// score 3 (was 3): this round: retail keeps the rotating-flag OR and store ahead of the
// angle duplicate and the first axis load; ours still hoists that load above the flag
// store and sinks the store below the duplicate. The axis copy-in and the normalizer
// re-read as an array (the helper's v->n[] terms, and a local real *axis_values) leave
// the load hoisted (3, 3, 3; component stores through that pointer 68); the helper
// without the inline keyword leaves real calls (168, and 0xfa580 stops matching);
// __forceinline and a const pointer parameter change nothing (3, 3); a reference to the
// axis (all, copy only, normalize only) and a helper taking a reference parameter give
// 14/3/14/14; the flag through a dword pointer is 3 and through a pointer local does
// not compile.
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
// Round 2 of #1061: still 3 (same on the 535-source image). The rotation axis stored through a
// pointer local 3; the three components copied one by one, i/j/k order 71, k/j/i order 81; the flag
// set through bit_vector_set 9; the angle declared after the flag store 10; the axis declared and
// assigned separately 3. The three differences are unchanged: retail's flag OR and store precede the
// angle duplicate (`fld st(0)`) and the first axis load.
// Permuter (25 min on #1061, about 1180 variants): nothing below 3.
// MiniMax A (11-13): no new form, no lower score (3): retail completes the rotating-flag OR and store before the angle duplicate
// and the first axis load, while this build hoists the axis load above the flag OR and sinks the flag store below the duplicate;
// the shared normalizer takes its vector by pointer, so the axis copy's address escapes and the store cannot be reordered, and
// making that helper take the vector by reference changes nothing (0xfa580 still matches, 0xfbd40 stays 3). This round: an extra
// pointer local to the axis, used by the normalizer call or left unused, and the normalizer reading its parameter through its
// address, all keep the same three differences (3 each, with 0xfa580 still matching). This round: the axis copy through the
// existing vector3f_assign scores 81; a pointer local to the axis handed to the normalizer scores 14 (the flag order is still
// ours and the squares come out in i, j, k order instead of retail's k, j, i); both are reverted.
// This round: the normalizer reading its parameter through its address reproduces retail's tail exactly but scores 11, because
// the squared terms then come out in i, j, k order instead of retail's k, j, i; the form is reverted. This round: the axis copy
// between the sine and the cosine gives the same bytes (3); the flag assignment before the angle computation scores 10; the axis
// copy written as three dword casts scores 28 (the copy loses its `lea` and the store stays after the flag OR); the escape in the
// normalizer keeps the squared terms in i, j, k whatever order the source uses, so the tail and the squares cannot both be had.
// address, all keep the same three differences (3 each, with 0xfa580 still matching).
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

// @retail 0xfd4c0
s_projectile_material_response *__stdcall projectile_get_material_response(s_projectile_definition const *definition,
	c_type_47f957 material_type)
{
	s_projectile_material_response *result = NULL;
	long response_count = definition->material_response_count;
	long i;

	while (!result && global_material_get(material_type.m_index))
	{
		for (i = 0; i < response_count; i++)
		{
			s_projectile_material_response *response = (s_projectile_material_response *)definition->material_responses + i;

			if (response->material_index == material_type.m_index)
			{
				result = response;
				break;
			}
		}
		material_type.m_index = *(short *)(global_material_get(material_type.m_index) + 8);
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
void function_1883e0(long tag_index, bool ignore_distance, point3f const *point, c_type_47f957 element_index, c_type_47f957 entry_element_index,
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

// MiniMax A (11-11): matches; the six selector outputs follow retail's order (effect 0, effect 1, effect 2,
// sound 0, sound 1, sound 2) in the parameters and in the NONE stores, and both call sites pass them in that
// order; retail reads them in eax, ecx, edx, esi, edi and ebp and null-tests them in that order.
/* the effects and sounds of an effects block's effect (+0x58), three of
   each, NONE without one:
   0x1883e0 gives sounds as its "first" values and effects as its "second"
   (retail passes the effect slots it reads back, the fifth argument, ecx
   and edx, as 0x1883e0's second_value04, second_value and second_value0c) */
// element_index is `short` (11-12, Fable lane 7 r153): its caller 0xfcea0 passes a 16-bit word and 0xa17d0 matches with it;
// this function still matches.
// @retail 0xfd740
void function_fd740(point3f const *point, byte const *effects, long index, short element_index,
	long *effect0, long *effect1, long *effect2, long *sound0, long *sound1, long *sound2)
{
	long effect_index = *(long const *)(effects + 0x58);

	if (effect_index != NONE)
	{
		function_1883e0(effect_index, false, point, c_type_47f957(g_47d8e0), c_type_47f957(element_index), index, 0, sound0, effect0,
			sound1, effect1, sound2, effect2);
		return;
	}
	if (effect0)
		*effect0 = NONE;
	if (effect1)
		*effect1 = NONE;
	if (effect2)
		*effect2 = NONE;
	if (sound0)
		*sound0 = NONE;
	if (sound1)
		*sound1 = NONE;
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
		function_1883e0(effect_index, ignore_distance, point, c_type_47f957(g_47d8e0), c_type_47f957((short)element_index), index, 0, sound0, effect0,
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

// MiniMax A (11-12): no new form, no lower score (232): the frame is still 0x28 against retail 0x30 and the two pointer parameters are still kept in edi and edx where retail loads each at its use, so the gravity pointer, the distance output, the rounded horizontal root and the two separate divisions still differ; the forms that move the pointers to their uses fold back to the same code, so the body stands.
/* aims a falling projectile along an arc: the slowest speed that reaches
   the target, then the time to it at the speed used (high or low arc),
   falling back to the slowest arc when the speed can't reach it */
// MiniMax A (11-14): no new form, no lower score (232): the frame is 0x28 against retail's 0x30, and the three
// components are subtracted into xmm1, xmm0 and xmm4 where retail takes them in xmm7 and xmm3.
// MiniMax H (11-14): no new form, no lower score (232): this round re-read the diff: retail holds the gravity pointer in EDX where this build holds it in EBX, and takes the horizontal root in SSE where we round through x87; the pointer's register belongs to the callers in other files, so the body cannot pick it.
// MiniMax B (11-13): no new form, no lower score (232): the three components are still subtracted into xmm1, xmm0 and xmm4 where retail takes them in xmm7 and xmm3, the frame is 0x28 against retail's 0x30, and the x87 and SSE halves are split differently, so the body has to be respelled as a whole rather than one form at a time.
// MiniMax H (11-12): no new form, no lower score (232): the x87 divisions, the register homes and the frame are unchanged; the two divisions cannot be had together with the callee convention, since the convention follows how two_a is produced.
// MiniMax G (11-11): no new form, no lower score (232); the frame is 0x28 against retail's 0x30, the two pointer parameters stay
// in EDI and EDX where retail loads each at its use, and the rounded horizontal root and the two separate divisions still differ;
// the two-element array reference and the horizontal length as an unsigned long both fold back to the same code or score higher.
// MiniMax H (11-11): no new form, no lower score (232); retail loads each of the two pointer parameters at its use while ours keeps them in EDI and EDX, and the frame, the rounded horizontal root and the two separate divisions still differ.
// MiniMax R (11-10): no new form, no lower score (232); the forced-speed and time-scale pointers are still enregistred in EDI and EDX where retail loads each at its use, and the frame, the rounded horizontal root and the two separate divisions still differ.
// MiniMax A (11-11): no new form, no lower score (232); the frame is still 0x28 against retail's 0x30 and the
// entry block still keeps the two pointer parameters in EDI and EDX where retail loads each at its use, so the
// whole register schedule (the gravity pointer, the distance output, the rounded horizontal root and the two
// separate divisions) differs; the forms that move the pointers to their uses, the reciprocal written as its own
// expression and the rounded root in a local all keep the same 232 differences. The original body is kept.
// MiniMax I (11-10): no new form, no lower score (232); the two pointer parameters are still enregistred in EDI and EDX where retail loads each at its use, and the frame, the rounded horizontal root and the two separate divisions still differ.
// MiniMax H (11-04): no new form, no lower score; the difference the earlier rounds describe is still there.
// MiniMax A (11-03): no new form, no lower score.
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
// Round 22 (m3 lane 15, --fast with the five callers, which alone builds another convention): t2 divided by
// `(a * 2.0f)` 248 (callers lost); optimal time from `-1.0f / (a * 2.0f)` 271; forced speed read through its
// reference at both uses 232 (same code); minimum speed as if/else 232 (same code); time-scale clamp as a stored
// product with if/else-if 253. The read through the address folds back to the parameter, which ours enregisters in
// EDI from entry, so the distance output is not kept in EDI from entry as retail keeps it for 0xfa580.
// Round 34 (m3 lane 11): delta through fa580's plain component loop 233; forced/time-scale pointers read
// plainly with `if (forced_speed)` first 288 and forced first with the address reads 287 (callers'
// convention lost in both); a clamp `if (gravity < 0.0f) gravity = 0.0f;` before `if (gravity > 0.0f &&
// ...)` in place of `!(0.0f > gravity) &&` 232 (986 bytes; plain 232, callers kept): the horizontal root
// moves into the entry block, rounded to a float in a dead parameter home and compared in SSE with the
// epsilon shared with delta.k as retail (aligned difference 227 -> 213), not kept as the clamp's store
// only shapes the code; on the clamp, two_a written out at both divisions 276 and the time scale read
// plainly 248 (callers lost in both). Diagnostic (reverted at once): both pointers read through volatile
// casts give retail's EDI distance and EDX gravity pointer (216; 212 with the clamp), so the remaining
// causes are those two pointers' entry registers, the reciprocal of two_a and the xmm homes of delta.
// Round 36 (m3 lane 11): the velocity's horizontal speed through magnitude2d 232 (same x87 order);
// optimal time as `sqrt(-root / two_a)` 236 (the SSE reciprocal stays); two_a written out at both divisions
// with plain pointer reads 247 (ret 0x28, callers lost); C++ references to the two pointer parameters 232
// (same code); `if (0.0f > gravity) gravity = 0.0f;` ahead of the vertical delta 247.
// Y488: horizontal admission first/positive gravity alone/both/positive test first
// 252/251/250/232; all four with consistent per-use gravity products 271/260/269/273.
// Original body restored; frame, rounded horizontal root and separate divisions remain.
// Round 189 (m3 lane 3, fast with the three matched callers): two_a from gravity 242 and two_a first with
// `a = two_a * 0.5f` 230 (both lose every caller's convention); `two_a = a + a` 232 (the SSE reciprocal stays);
// two_a before the discriminant base 238; delta.k computed with delta.i/j 233.
// Y492: native real quotient producers optimal/solved/both 232/232/232;
// double producers 246/273/264, both with entry result/double flight time 267/302.
// All restored; frame, rounded-root scheduling and separate divisions remain.
// Round 495: Whole fallback/default/publication CRT-copy subsets on the shared-scale/path
// graph 232 each (seven forms); complete copies with entry result 245. Original body
// restored; frame, rounded-root scheduling and separate divisions remain.
// Round 497: Entry chosen-speed/time/inverse-time storage, seven nonempty subsets, 232 each; all
// with path-distance storage 232. Every producer stays at its original use; original body restored.
// Frame 0x28/0x30, rounded horizontal root and separate divisions remain.
// Round 243: existing horizontal dot producer alone/both norms 232/232; with complete
// displacement helper 235/236; entry-result counterparts 245/245/228/228. The 228 forms
// recover frame 0x30 but gain only four and lose four matched callers. Original helpers,
// inputs, scalar trees, result lifetime and inherited optional-pointer contract restored.
// M287: Native double a/b/discriminant-base storage subsets and the complete
// solved-coefficient storage:
// 270/263/247/264/269/268/262/320. All precision and original pointer contracts restored;
// frame, rounded-root scheduling and separate divisions remain.
// Round 250: rounded solved-root receiver with conditional long/short/real sign 232/241/261;
// branch-produced long/short/real sign 262/254/261; default negative/positive integer sign 261/261.
// Exact-only receivers restored; latter six forms lose five matched callers. Original pointer
// contract, frame 0x28/0x30, rounded horizontal root and separate-division gap remain.
// M290: Independent/shared rounded polynomial-root storage with solver/entry declarations and original/shared horizontal-square storage:
// 232/232/232/232/232/232/232/232. Original executable body restored.
// Round 43 (lane 15, fast with 0xfa6a0, 0xfa580, 0x1f8100, 0x1f8160; ret 0x2c already agrees, 11 stack arguments; frame 0x28 against
// retail's 0x30 is two_a and the discriminant base in memory): t2 with no float cast on its sqrt 232 (reciprocal stays); that with the
// optimal time as `root * -1.0f / two_a` 232, `root / -two_a` 236, `-root / two_a` 236, `-1.0f * root / two_a` 232. CL-only: only a
// double dividend or a divisor written as its own expression gives two x87 divisions; a branch or call between keeps the reciprocal.
// Two_a written at both divisions 248 (970 bytes; gravity scale moves to an XMM argument, 0xfa6a0 44, 0x1f8100 8); with the clamp 276
// (1012), with plain pointer reads too 278; the gravity scale read through its address on those 246 and 295 (1018; callers kept), plain
// pointers 278 (0xfa6a0 34, 0x1f8100 5). The clamp alone 232 (986). Velocity written out instead of the scale helper 234 (983), with
// the clamp 234 (986).
// Round 2 of #1061: statement-by-statement logic walk against retail, no behaviour difference
// found: gravity from the global times the scale, `0 > gravity` / `gravity > 0` and the two 0.0001 tests
// (`ja`/`jbe`, same NaN senses), distance_squared = k*k + (j*j + i*i) order, a = g*g*0.25, discriminant base =
// d2*a*4, two_a = a*2, root = -sqrt(base) rounded to float, optimal time = sqrt((-1/two_a)*root), minimum =
// b - root clamped at 0 before its sqrt, minimum-speed store first, forced speed read once else speed with the
// time-scale clamp [0.001, optimal] and `speed > sqrt(b - x)`, `chosen >= minimum`, c = b - v*v, `0 > c` and
// `disc >= 0`, the sign via setne/lea and fimul, t2 unrounded in x87 with `t2 > 0`, the not-found exit (time =
// optimal, chosen = minimum, result false), inverse time scaling of delta, horizontal speed before g*t/2 is added
// to k, vertical stored before normalisation, the zero-length fallbacks (delta, then g_4687b0, result false) and
// the stores in retail's order (direction, distance, speed out, vertical, horizontal, time). Only register homes,
// the frame and the SSE reciprocal remain. New forms: two_a and the discriminant base as references to a
// two-element array 232 (folds back); `volatile real two_a` as a diagnostic gives the two x87 divisions but 277
// (1000 bytes) and loses every caller (0xfa6a0 44, 0x1f8100 8, 0x26f150 21, 0xecdc0 32, 0x2900b0 30): the
// convention follows how two_a is produced, so the divisions and the convention cannot yet be had together.
// Permuter (25 min on #1061, about 790 variants): 180 only with a volatile horizontal length and the found flag
// as an unsigned long; not kept (volatile without a match).
// MiniMax A (11-13): no new form, no lower score (232): our frame is 0x28 against retail's 0x30 and the SSE register allocation
// differs from the first instruction (xmm7 and xmm4 here where retail uses xmm5 and xmm7).
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

// MiniMax A (11-12): no new form, no lower score (163): our frame is still 0x48 against retail 0x34, and the last argument of the 0x1697c0 call rides in ebp here and in edx in retail, so the pushed arguments and the returned point slot differ; that callee lives in src/bipeds.cpp, which this lane may not edit, so its register convention cannot be moved from here. BLOCKED-BY 0x1697c0.
/* whether a projectile's path to a point hits anything: the line from it,
   and for projectiles with a radius the two lines either side of it */
// MiniMax A (11-14): no new form, no lower score (163): the frame is 0x48 against retail's 0x34, so the local homes do
// not fit retail's shape, and the model entry and the collision point are taken in the other order afterwards.
// MiniMax H (11-14): no new form, no lower score (163): this round re-read the diff: the zero test still compares against a memory constant where retail reuses a zeroed register for its negation, and the cross product's products are still multiplied in the other order, so the frame and the register plan have to move together.
// MiniMax B (11-13): no new form, no lower score (163): the frame is 0x48 against retail's 0x34, so this build's local homes do not fit retail's shape, and the model entry and the collision point are taken in the other order afterwards; the frame follows from the locals, so the body has to be respelled together.
// MiniMax H (11-12): no new form, no lower score (163): retail's return-true block still sits right after the first call and jumps back to it where ours puts the merged block last, and the collision pointer is still loaded into ebp.
// MiniMax G (11-11): no new form, no lower score (163); the frame is 0x48 against retail's 0x34 and the three ray queries still
// inline 0x16a040, whose defining file this lane may not edit; the radius at each use, the collision pointer read through its
// own address, the side queries joined as one expression and an early return for the radius test all score 163 or higher.
// MiniMax H (11-11): no new form, no lower score (163); the frame is 0x48 against retail's 0x34 and the three ray queries still expand 0x16a040, whose defining file is not this lane's.
// MiniMax R (11-10): no new form, no lower score (163); the frame stays 0x48 against retail's 0x34 and the three ray queries still inline 0x16a040, whose defining file is not this lane's.
// MiniMax A (11-11): no new form, no lower score (163); the frame is 0x48 against retail's 0x34 and the prologue
// order differs with it: retail reads the tag table's base, the projectile's radius and the object pool before the
// four endpoint pairs are formed, ours keeps the radius and the vector in the frame. The endpoint pairs computed
// together, the radius read at each use and the collision pointer read through its own address all leave the frame
// at 0x48 and the same 163 differences, and the forms with fewer live values (a shared offset vector, a side vector
// in the frame) score no lower; the three ray queries still inline 0x16a040, whose defining file is not this lane's.
// The original body is kept.
// MiniMax I (11-10): no new form, no lower score (163); the frame stays 0x40 against retail's 0x34 and the three ray queries still inline 0x16a040, whose defining file is not this lane's.
// MiniMax H (11-04): no new form, no lower score; the three ray queries have to inline 0x16a040, whose defining file is not this lane's.
// MiniMax A (11-03): no new form, no lower score.
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
// Round 22 (m3 lane 15, one-file harness exact at 163, radius form 181 in --fast too): radius field at each use
// 181 (definition in EBP as retail); single-exit result 184, with the radius at use 187; first query vector through
// vector3d_from_points3d 163, with the radius at use 188; side delta through it 163, with the radius 181, both 188;
// endpoint declarations at the block top 163; first query vector reused for both offset queries 163, with the
// radius 180; a separate opposite query vector 164; opposite endpoints computed first 186.
// Round 34 (m3 lane 11): BLOCKED-BY 0x16a040's noinline declaration (kept out of line for 0x1c8df0's calls): retail's
// three queries are that test inlined. The three queries as calls to it 328 (611 bytes, out of line); the query
// vectors renamed 163 and the endpoints renamed 163 (same code, so slot order here does not follow names); the side
// delta computed k, j, i (retail's load order) 166; the radius read from the field in the test and into a block
// local 184 (the collision pointer keeps EBP).
// Round 36 (m3 lane 11): the three queries as calls to 0x16a040 with a __forceinline redeclaration here 328
// (the defining file's noinline wins); with that noinline declaration dropped 187 (inlined, frame 0x3c;
// 0x1c8df0 170 -> 457, 0xcf040/0x24da18/0x24e59f stay matched); on that, endpoints through vector3f_add_scaled
// 173, radius compared from the field and read in the block 170, radius field at every use 158 (definition in
// EBP as retail; frame 0x40 vs 0x34 and the shared return-true block remain). All restored.
// Round 229: complete origin/target/both point values, native or library copies,
// 196/181/221 in each family; both with bounded initial ray 221, shared query vector 218.
// Every original input observation and executable body restored; native frame/tag/query homes remain.
// Round 2 of #1061: logic walk against retail, no behaviour difference found: first query
// (flags 0x2480000f, origin, point - origin, projectile+0x140, projectile index), radius test `0.0001 > radius`
// then `+0xc4 != NONE`, side = cross(up, delta) in retail's component order, normalise and the g_4687ac fallback,
// the four endpoints (side * radius, `0 - radius` for the negative pair; the negative products take radius first
// on x and the side first on y, z), flags 0x4800008, ignore NONE for both side queries, the offset vector as end
// - start. Retail's return-true block sits right after the first call and later hits jump back to it; ours puts
// the merged block last. New forms: collision pointer read through its own address 163 (the load is still hoisted
// into EBP), with the radius at each use 181 (745 bytes; EBP is then the definition and ESI reloads the
// collision for the side queries as retail, but the frame stays 0x48 and the end points are still stored);
// a local __forceinline helper with 0x16a040's body for the three queries 187 (735 bytes, frame 0x3c); side
// queries joined as one `||` expression 163; early `return false` for the radius/tag test 181 (767 bytes).
// Permuter (25 min on #1061, about 1140 variants): 160 only with vector3d_from_points3d's two points swapped; not kept.
// MiniMax A (11-13): no new form, no lower score (163): our frame is 0x48 against retail's 0x34, so every later offset is shifted;
// the permuter ran 78 tries in 600 s and kept the baseline, and this round's run (4 tries in 394 s) kept it again.
// This round: moving the opposite-point block after the first call scores 220, and re-reading the radius after the calls scores
// 184; both are reverted.
// the permuter ran 78 tries in 600 s and kept the baseline.
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
// MiniMax G (11-11): no new form, no lower score (4); only the four register arguments of the 0xfd740 call differ, and that
// callee's own parameter order belongs to another file, so the four instructions cannot be moved from this body.
// MiniMax H (11-11): no new form, no lower score (4); only the four register arguments of the 0xfd740 call differ, and that callee's own parameter order is in another file.
// MiniMax R (11-10): no new form, no lower score (4); only the four register arguments of the 0xfd740 call differ, and that callee's own parameter order is not this lane's to change.
// MiniMax A (11-11): no new form, no lower score (4); the only four differing instructions are the register
// arguments of the 0xfd740 call: retail passes the effect index 1 in ecx, the effect index 2 in edx, the sound 0 in
// esi and the sound 1 in edi, ours passes the same four addresses in ebx, edi, esi and edx. That choice is 0xfd740's
// own convention (it null-tests its outputs in the order eax, ecx, edx, esi, edi, ebp), so it cannot be moved from
// this file. BLOCKED-BY 0xfd740.
// MiniMax I (11-10): no new form, no lower score (4); only the four register arguments of the 0xfd740 call differ, and that callee's own parameter order is not this lane's to change.
// MiniMax H (11-04): no new form, no lower score; the four register arguments of the 0xfd740 call follow that callee's own parameter order, which this lane may not change.
// MiniMax A (11-03): no new form, no lower score.
// score 4 (was 4): only the four register arguments of the 0xfd740 call differ: retail passes
// effect index 1 in ecx, effect index 2 in edx, sound 0 in esi and sound 1 in edi, ours passes
// the same four addresses in ebx, edi, esi and edx; that choice comes from 0xfd740's own body.
// This round: nothing in this file changes it; the call site keeps retail's argument order and
// the callee's own parameter order is not this lane's to change. BLOCKED-BY 0xfd740.
// score 4 (was 4): this round: retail's 0xfd740 takes its six selector outputs
// in the order (effect index 0, sound 2, effect index 1, effect index 2,
// sound 0, sound 1), the first two on the stack and the last four in ecx, edx,
// esi and edi; our callee takes them in the order (effect index 0, sound 0,
// effect index 1, sound 1, effect index 2, sound 2) and reads the last four
// from esi, edi, ebx and edx. The four addresses are the same set, so only
// the registers and their order differ; matching them needs the callee's
// parameter order changed, which is not ours to change. BLOCKED-BY 0xfd740.
// score 0 (was 4): only the register arguments of the 0xfd740 call differ (retail
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
// Round 2 of #1061: re-measured 4 (the four lea lines before `call 0xfd740`; every other instruction, the two loops
// and the request fill agree). Retail 0xfd740 (a callee of this file, not touched here) takes its selector outputs in
// eax (a stack argument loaded at entry), ecx, edx, esi, edi and ebp and null-tests them in that order at 0xfd783-0xfd7b5;
// the caller's register choice (ecx = effect 1 at esp+0xa4, edx = effect 2 at +0xa8, esi = sound 0 at +0x94, edi = sound 1
// at +0x98) is that callee's own convention, so nothing in this body moves it. BLOCKED-BY 0xfd740.
// MiniMax A (11-11): matches; 0xfd740's six selector outputs now follow retail's order (effect 0, effect 1,
// effect 2, sound 0, sound 1, sound 2) in its parameters, in its NONE stores and at both of its call sites,
// which fixes the four register arguments of this call; 0xfd740 matches with it and 0xfd0e0 goes 29 -> 25.
// index is `short` (11-12, Fable lane 7 r153): 0xa17d0 loads the word field for it with `xor edx, edx; mov dx` and matches;
// this function still matches.
// @retail 0xfcea0
void function_fcea0(long effects_index, point3f const *point, vector3f const *direction,
	s_effect_owner const *owner, short index, vector3f const *normal)
{
	byte *effects = g_4e3b44[effects_index & 0xffff].bytes;
	long sounds[3];
	long effect_indices[3];
	s_location location;
	long i;

	function_11bed0(&location, point);
	function_fd740(point, effects, 0xf, index, &effect_indices[0], &effect_indices[1], &effect_indices[2], &sounds[0], &sounds[1],
		&sounds[2]);
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
// MiniMax R (11-10): no new form, no lower score (115); the frame and the extent agree, but the point's home and the selector call's registers still follow 0xfd740, which is not one of this lane's functions.
// MiniMax I (11-10): no new form, no lower score (115); the frame and the extent agree, and the point's home and the 0xfd740 selector call's registers still follow that callee, which is not one of this lane's functions.
// MiniMax H (11-04): no new form, no lower score; the selector call's registers and the point's lifetime follow the unmatched 0xfd740 in this file.
// MiniMax A (11-03): no new form, no lower score.
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
// Round 22 (m3 lane 15): NUMBEROF bounds 115 (retail's `jb` in both loops; positional count unchanged); dword
// counter 115; sound index read at each use 115; sibling 0xfcea0's request order with NUMBEROF 120; sound records
// at function scope 115. Retail pushes the 16-bit material raw to 0xfd740 where ours zero-extends it for that
// callee's `long` parameter: BLOCKED-BY 0xfd740 for its parameter width as well as its EBX argument.
// Round 34 (m3 lane 11): first difference (+0xd) is still that convention: retail passes 0xfd740 four register
// pointers (EDI, ESI, EDX, ECX) and keeps the point in EDI, ours five (EBX too) with the point in EBX; retail's
// caller 0xa1df0 zero-extends the word before the call, so retail's raw push fits a 16-bit callee parameter.
// No trial: the callee is contributor code.
// Round 36 (m3 lane 11): the alternate tag (+0xec) in a named local 149; the final tag (+0x144) in a named
// local 150. Root of the 0xfd740 convention: retail's 0xfd740 keeps the tag index in EBX for 0x1883e0, which
// takes it there only with its 4th/5th parameters as c_type_47f957 (0x1883e0's note). With that form, 0x188180's
// call and the 0xfd740/0xfd7d0 call sites converted (diagnostic, reverted) this is 29 (0x1883e0 matches, 0xfd740
// 49 -> 8): BLOCKED-BY 0x1883e0 (its class form is held by src/impacts.cpp and the 0xfd740/0xfd7d0 call sites).
// What then remains: the raw 16-bit index push, 0xfd740's output order, `jb` bounds, the zero-store order.
// Round 47 (m3 lane 13, fast): NUMBEROF bounds 115 again (both loops `jb`; the effect loop sits one line off);
// markers and parameters at function scope 153; the final tag stored into the parameters before its NONE test
// 116; the sound records cleared before the effect records 121; the attachment stores with the flag last 115.
// The sound loop's point and velocity copies hold their source pointer in ECX/EAX where retail uses EAX/ECX, so
// the copy stores come out in the other order; same allocation cause as the BLOCKED-BY 0xfd740 entry.
// Round 229: one done exit/exclusive arms with a twice-used selected effect result,
// native unsigned ordinal and aggregate table defaults: 115/177/115/177/121/183/121/183.
// Original exits, result reads, defaults and whole executable source restored; point/zero/selector homes remain.
// Round 2 of #1061: logic walk against retail, no behaviour difference found: location from the
// point, six NONE stores, markers from the direction/normal, default parameters, `attached` stores flags 1, the
// object index and the word node index (retail loads a word), scales, markers, count 6, the selector call (the
// index pushed as a raw dword where ours zero-extends the word), the alternate-tag early return with flags |= 4,
// the unsigned-compared effect loop (`jb`), the final tag after flags |= 4, and the sound loop (point copy,
// compressed forward, velocity, location, scale 1.0, bits 0x11, NONE playback, zero source/marker/variant).
// Everything left is allocation around 0xfd740's five register pointer arguments (ours) against retail's four
// (point in EDI, zero in EBX), so it stays BLOCKED-BY 0xfd740, which this lane does not touch. No new form tried.
// Permuter (25 min on #1061, about 340 variants): 111 only with the flag set through an int temporary and the
// sound index declared apart; the plain forms in it (tag before flag, unsigned index, declaration order) stay 115.
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
		function_fd740(point, definition, 0xf, index, &effect_indices[0], &effect_indices[1], &effect_indices[2], &sounds[0], &sounds[1],
			&sounds[2]);
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

// MiniMax A (11-12): no new form, no lower score (296): retail expands the speed update 0xfbfd0 inline after the 0xfbd40 call and holds index * 12 in ebp, a body the rules forbid writing out by hand, so the pool and tag homes, the speed update schedule and the seat-bit tests stand.
/* the projectile type's new callback: its target, lifetime, range,
   attachment, starting velocity and spin, whether it starts under water,
   and the seat flags of the unit that fired it */
// MiniMax A (11-14): no new form, no lower score (296): retail expands 0xfbfd0 into this body while this build calls
// it, so the body is 66 bytes short of retail's 1261 (BLOCKED-BY 0xfbfd0), and retail takes the model table's entry
// before the collision subtraction where this build takes the subtraction first.
// MiniMax H (11-14): no new form, no lower score (296): retail still expands the matched speed setter inline and this build still calls it; inlining it here was measured before at 313 with a longer body, and the model table's entry is still taken after the collision subtraction.
// MiniMax B (11-13): no new form, no lower score (296): retail takes the model table's entry before the collision subtraction where this build takes the subtraction first, and the two then push their arguments in the other order; the frame's locals differ as well, so the block would have to be respelled together.
// MiniMax H (11-12): no new form, no lower score (296); retail expands the speed update 0xfbfd0 inline after the
// 0xfbd40 call and keeps index * 12 in EBP from entry, and that body is another lane's finished code which this
// lane may not write out by hand, so the pool and tag homes, the speed update's schedule and the seat-bit tests stand.
// MiniMax G (11-11): no new form, no lower score (296); retail expands the speed update 0xfbfd0 inline after the
// 0xfbd40 call and keeps index * 12 in EBP from entry, and that body is another lane's finished code which this lane
// may not write out by hand, so the pool and tag homes, the speed update's schedule and the seat-bit tests stand.
// MiniMax I (11-11): no new form, no lower score (296): retail expands the speed update 0xfbfd0 after the 0xfbd40 call and keeps index * 12 in EBP, and that body is another lane's finished code here, so the pool and tag homes, the speed update's schedule and the seat-bit tests stand.
// MiniMax H (11-11): no new form, no lower score (296); retail expands 0xfbfd0 inline after the 0xfbd40 call and holds index * 12 in EBP, a body this lane may not write out by hand, so the pool and tag homes, the speed update's schedule and the seat-bit tests stand.
// MiniMax S (11-10): no new form, no lower score (296): retail expands 0xfbfd0 after the 0xfbd40 call and holds index * 12 in EBP, a body this lane may not write out by hand, so the pool and tag homes, the speed update's schedule and the seat-bit tests stand.
// MiniMax A (11-10): no new form, no lower score (296): retail expands 0xfbfd0 inline after the
// 0xfbd40 call and holds index * 12 in ebp, a body the rules forbid writing out by hand, so the
// pool and tag homes, the speed update's schedule and the seat-bit tests stand.
// MiniMax R (11-10): no new form, no lower score (296): retail expands 0xfbfd0 inline after the 0xfbd40 call and holds index * 12 in EBP, a body the rules forbid writing out by hand, so the pool and tag homes, the speed update's schedule and the seat-bit tests stand.
// MiniMax D (11-10): no new form, no lower score (296).
// score 296 (was 307; 535-source image; the 4-source reduced image allocates differently and scores
// higher, so measure this with a big caller such as 0xfaa60 in the check).
// Round 2 of #1061: statement-by-statement walk against retail: no behaviour difference (every
// store, test, call and constant agrees); the differences are structural. (1) retail's callee list is
// only 0xfbd40 and 0xfd8b0: it expands 0xfbfd0 after the 0xfbd40 call (pool and tag re-read, final speed
// when object_flags bit 3 else initial speed), which written out here gives 230 at retail's 1261 bytes but is not kept: it copies another function's body without a match, so the call stays. (2) retail
// reads the tag once at entry into EDI (definition local). (3) it reads the timer bounds straight from
// the definition (frame 4, not 0xc) and tests flag bit 2 with shr/test (TEST_FIELD_BIT). (4) under water
// is one `under_water = true` under `&& (!(water & 0x80) || 0 > dot)`, then bit_vector_set (the
// `mov bl, 1; test bl, bl` pair). Scores on the 535-source image: definition local + inline speed update
// reading the tag again 303; + single-store water 262; + timer/flag form 230 (all with the written-out speed update; with the call instead, 296, kept). Not kept: speed
// update through the definition local 328 (311 with the water form), a second pool fetch like an inline
// expansion (D,P) 314 / 310, seat tests as a word-bitfield view (retail's two shr/test) 246 at 1275
// bytes, 0x190 stored from `speed` 215 at 1243 bytes (retail recomputes the product); named
// object_flags bits 16/17, flags.unknown8 = false, a pool local with the root helper taking it, and
// operand swaps of the speed product and the plane dot product all leave 230.
// Left: retail loads the pool into EDX first and keeps index * 12 in EBP (ours keeps the pool in EBP, no
// index * 12 as the speed update reads through ESI); retail reloads the tag for 0xfd8b0 where ours keeps
// a copy; ours keeps NONE in a register for the 16-bit stores/compare where retail has immediates;
// the speed product's operand order and the 0x190 store before the `and [esi+0x12c], 0xfffffeff` RMW;
// the plane dot product's operand order; retail's two separate object_flags RMWs (0x10000, 0x20000)
// where ours merges to one `or 0x30000`; retail's two separate seat-bit tests where ours has `test dl, 6`.
// Earlier rounds (before the definition was read once):
// HELD (MiniMax C, 11-01): 0xfbfd0 marked `inline` (it is @Banshee64's finished code,
// #9, so it waits for their OK) with a definition local at entry gives 304: the speed update expands where
// retail expands it, the definition goes to EDI and the body is retail's 1261 bytes; the definition local
// without the inline is 380. On that 304 form: retail keeps the pool pointer in EDX from the entry to the
// owner loop and re-reads the projectile's tag for the 0xfd8b0 call, where ours keeps the tag in EAX and
// re-loads the pool global inside the loop (differs from +0x5). A pool local with the root helper taking
// the pool 302; typed field stores and a byte-cast tag read 304; the permuter's best (294 reduced, 293
// full) needs a `volatile` cast, which is not allowed.
// score 315 (was 315): retail expands the speed update inline after the 0xfbd40 call, keeping
// index * 12 in EBP from entry and the definition in EDI; ours calls it, so the offset is never
// kept and the entry differs at +0xb. Marking 0xfbfd0 `inline` expands it but scores 369.
// Earlier (315): frame 4 agrees; pool/input and tag receiver allocation differ at +0xb.
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
// Round 34 (m3 lane 11): the unit's seat bits copied with the existing word_flags_set on the +0xc0 word 315 (kept:
// the tail is now retail's `shr dl; test dl, 1` and unmerged `or/and byte ptr` exactly; byte_flags_set 332, same
// code as the if/else). On that: the unit word read at each use in the seat test 347 (still `test, 6`); both seat
// tests through TEST_FIELD_BIT 333 (retail's separate byte shifts appear, in BL/DL, 1243 bytes); a definition local
// at entry for every use 380; the definition flag through flag_bits.unknown2 347; as `(flags >> 2) & 1` 315 (same
// code). Still BLOCKED-BY 0xfbfd0 (contributor) for retail's inline copy and its EBP-held index * 12.
// Round 36 (m3 lane 11): the water test with one `under_water = true` under `&& (!(water & 0x80) || 0 > dot)`
// 315 (same code); the under-water flag through bit_vector_set(&object_flags, 3, under_water) 344 (retail's
// exact `mov bl, 1; test bl, bl` single-store block, but two fewer instructions ahead of the missing 0xfbfd0
// expansion); with a definition local at entry 364; with the single assignment 344.
// Y488: actual attachment result/random bounds/timer bounds subsets
// 344/328/328/307/328/330/309; all with shared initial speed 362.
// Keep only the twice-used timer-bound pair, plain/full 307; other forms restored.
// Y492: complete timer selectors combined/inverted/equality-first/conditional
// 339/339/305/339; each with completed attachment result 348/348/346/348.
// The two-point equality-first lead is below retention; all restored.
// Entry frame/slot/tag homes and speed-update expansion remain.
// MiniMax A (11-13): no new form, no lower score (296): retail expands 0xfbfd0 into this body while this build calls it, so the
// body is 66 bytes short of retail's (BLOCKED-BY 0xfbfd0).
// @retail 0xf8200
bool __stdcall function_f8200(long projectile_index, byte const *data, long unused)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);
	// retail reads the tag once at entry (the definition lives in EDI from there on)
	s_projectile_definition *definition = PROJECTILE_DEFINITION_GET(projectile->tag_index);

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

	if (*(real *)(((byte *)definition) + 0x184) == *(real *)(((byte *)definition) + 0x188))
	{
		*(real *)((byte *)projectile + 0x18c) = *(real *)(((byte *)definition) + 0x184);
	}
	else
	{
		*(real *)((byte *)projectile + 0x18c) =
			projectile_random_range(*(real *)(((byte *)definition) + 0x184), *(real *)(((byte *)definition) + 0x188));
	}

	real ticks;

	if (TEST_FIELD_BIT(definition->flag_bits.unknown2))
		ticks = g_510c54->field_2_3 * *(real *)(((byte *)definition) + 0xd4);
	else if (*(real *)(((byte *)definition) + 0xd4) == *(real *)(((byte *)definition) + 0xd8))
		ticks = g_510c54->field_2_3 * *(real *)(((byte *)definition) + 0xd4);
	else
		ticks = projectile_random_range(*(real *)(((byte *)definition) + 0xd4), *(real *)(((byte *)definition) + 0xd8)) *
			g_510c54->field_2_3;
	if (ticks >= 1.0f)
		*(real *)((byte *)projectile + 0x15c) = 1.0f / ticks;

	real arming_ticks = g_510c54->field_2_3 * *(real *)(((byte *)definition) + 0xcc);

	if (arming_ticks >= 1.0f)
		*(real *)((byte *)projectile + 0x164) = 1.0f / arming_ticks;

	projectile->attachment_index = NONE;
	for (short i = 0; i < *(long *)(((byte *)definition) + 0x94); i++)
	{
		if (*(long *)(*(byte **)(((byte *)definition) + 0x98) + i * 0x18) == 'cont')
		{
			projectile->attachment_index = i;
			break;
		}
	}

	real speed = *(real *)((byte *)projectile + 0x16c) * definition->unknown17c;
	vector3f *forward = (vector3f *)((byte *)projectile + 0x70);

	for (long component = 0; component < 3; ++component)
		projectile->linear_velocity.n[component] += forward->n[component] * speed;
	*(real *)((byte *)projectile + 0x190) = *(real *)((byte *)projectile + 0x16c) * definition->unknown17c;
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
			point3f *center = (point3f *)((byte *)projectile + 0x30);

			if (*(short *)(plane + 2) != NONE &&
				(!(water & 0x80) ||
				0.0f > *(real *)(plane + 0xc) * center->z + *(real *)(plane + 8) * center->y +
					*(real *)(plane + 4) * center->x - *(real *)(plane + 0x10)))
			{
				under_water = true;
			}
		}
	}
	bit_vector_set((dword *)&projectile->object_flags, 3, under_water);

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
				word_flags_set((word *)((byte *)projectile + 0xc0), 2, (*(word *)(*(byte **)(header + 8) + 0xc0) >> 2) & 1);
				word_flags_set((word *)((byte *)projectile + 0xc0), 1, (*(word *)(*(byte **)(header + 8) + 0xc0) >> 1) & 1);
				*(long *)((byte *)projectile + 0xb8) = *(long *)(*(byte **)(header + 8) + 0xb8);
			}
		}
	}
	return true;
}

real function_17c900(short function_type, real input);

// MiniMax A (11-12): no new form, no lower score (348): the frame of 0x30 agrees but retail takes the projectile index off the stack and forms the entry address itself, keeping the projectile in esi where we keep the index in esi and the projectile in ebx on the stack, so the aim point stack home sits four bytes higher; the declaration and the call contract may not change, so the body stands.
/* where a guided projectile aims: its target's point, led by the target's
   velocity, then wobbled around the line to it or wandering */
// MiniMax A (11-14): no new form, no lower score (348): the frame agrees (0x30) but the entry register allocation
// differs - retail takes the projectile index off the stack and forms the entry address itself, keeping the projectile
// in esi where this build keeps the index in esi and the projectile on the stack - and the body is four bytes longer.
// MiniMax H (11-14): no new form, no lower score (348): the shared cross-product helper's spelling still decides this block's shape and its other caller (0xfa580) is matched with the current one, so the helper cannot change; what is left is the product order and the stack slots inside it.
// MiniMax B (11-13): no new form, no lower score (348): the cross product's products are still multiplied in the other order and the results land in other stack slots, so the block's shape differs rather than a single instruction, and the rest of the body follows that shape.
// MiniMax H (11-12): no new form, no lower score (348); retail takes the projectile index off the stack and forms
// the entry address itself, keeping the projectile in ESI, where we keep the index in ESI and the projectile in EBX
// on the stack, so the aim point's stack home sits four bytes higher; the declaration and the call's contract may not
// change, so the body stands.
// MiniMax G (11-11): no new form, no lower score (348); retail takes the projectile index off the stack and forms
// the entry address itself, keeping the projectile in ESI, where we keep the index in ESI and the projectile in EBX
// on the stack, so the aim point's stack home sits four bytes higher here; the declaration and the call's contract
// may not change, and this round re-read the diff and the index's home is fixed by the caller, so the body stands.
// MiniMax I (11-11): no new form, no lower score (348): retail takes the projectile index off the stack and forms the entry address itself, keeping the projectile in ESI, where we keep the index in ESI and the projectile in EBX on the stack, so the aim point's stack home sits four bytes higher here; the declaration and the call's contract may not change.
// MiniMax H (11-11): no new form, no lower score (348); retail takes the projectile index off the stack and forms the entry address itself, keeping the projectile in ESI, where we keep the index in ESI, the projectile in EBX and save EBX on the stack, so the aim point's stack home sits four bytes higher here than in retail.
// MiniMax S (11-10): no new form, no lower score (348): retail takes the projectile index off the stack and forms the entry address itself, keeping the projectile in ESI, where we keep the index in ESI, the projectile in EBX and save EBX on the stack, so the aim point's stack home sits four bytes higher here than in retail.
// MiniMax A (11-10): no new form, no lower score (348): the frame of 0x30 agrees, but the aim
// point's stack home is [esp+0x24] in retail against [esp+0x28] here, and retail's sole caller
// passes the index on the stack and takes the aim point back in ebx while our declaration and
// call take the index in eax and pass the point's address; a declaration may not change.
// MiniMax R (11-10): no new form, no lower score (348): the aim point's stack home is [esp+0x24] in retail against [esp+0x28] here, and retail pushes EBP and ESI at a different point, so the rotation's register homes still differ.
// MiniMax D (11-10): no new form, no lower score (348): the aim point's stack home is [esp+0x24] in retail against [esp+0x28] here, and retail pushes EBP/ESI after the lookup where ours pushes EBX/EBP/ESI before it.
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
// Round 22 (m3 lane 15): the header normalize_inline 348; lead field read at each use 369; the file normalize
// helper with one exit 344 (1454 bytes, four points; 0xfa580 stays matched, 0xfbd40 3); that with the output
// written at three exits 358; plus the salt computed in the wander branch 345 (projectile in ESI as retail, index
// EAX to EBX, output EBP, frame 0x2c); plus a target pointer local 345 (same code).
// On the one-exit helper: salt in the wander branch alone 359; three exits + wander salt + lead read at each use
// 351 (1463 bytes); three exits + lead at each use 361; lead at each use 352 (1463 bytes).
// Round 34 (m3 lane 11): the salt written at both uses with the output written at three exits (two point setters
// on aim_point, the plain copy in an else) gives retail's whole convention (index on the stack re-read for the
// salt, output in EBX, definition spilled, frame 0x30; identical to +0x4f) at 328, aligned difference 162 (0xf8eb0
// 946 -> 969); left: retail's early SSE zero (xmm4) and to_target's components kept in xmm1-3 across the normalize
// merge, and the rotation's register homes. On that form: normalize3d 336; vector3d_from_points3d for to_target
// 350; lead field read at each use 341 (1463 bytes, index back in a register); wobble clock as if/else 328 (same);
// up vector read through the global at each use 328 (same).
// Round 36 (m3 lane 11): on round 34's form, the wander offset as a named direction vector before the scaled
// add 362 (its x87 tail is retail's exactly; that form is 19 bytes long elsewhere: retail keeps to_target in
// xmm3-xmm1 and a zero in xmm4 across the normalize merge and re-reads the lead field at its second use). On
// this form: that vector 365; the salt read late through the index's address 366 (index copied to EBX at
// entry); normalize3d in place of projectile_normalize 344 (1454 bytes); with the wander vector 343 (1470
// bytes); on that, the lead field at each use 349, salt named in the wander branch 361, cross3f for the axis
// 343 (same code), output written at three exits 378, to_target through vector3d_from_points3d 377, the lead
// and wander adds through vector3f_add_scaled 343/343 (same code).
// Round 126 (m3 lane 6, fast with 0xf8eb0/0xfa580/0xfbd40/0xf86f0/0xf8db0): the game time named once in the wander
// branch 348 (same code); that with the salt computed in the branch, after or before the game time, 365/365 (1469
// bytes); the wander amount as one nested ternary 348; the up pointer declared after the axis 348. Retail keeps the
// index on the stack (re-read and shifted in the wander branch, its home then reused for the yaw) and the output in
// EBX; the wobble path pops and writes the output itself.
// Round 109 (lane 8, fast with 0xf8eb0/0xf86f0; aligned = difflib count 218): `to_target` declared before the
// aim point 348; `point3f const *center` 348; the drift test as `distance > 0.0001f && flag` 354 (1471 bytes); the
// up-vector pointer declared after `axis` 348; the lead block's speed through the existing magnitude3d 348 (LTCG
// inlines it; identical code). The index EAX/output-stack convention (retail: index on the stack, re-read for
// the salt, output in EBX) still sets every register from the first instruction.
// Round 16 (m3 lane 17, fast): salt declared at entry and assigned in the wander branch 365 (1469 bytes), with
// the output written at the three exits 382 (1480 bytes); to_target declared first 348 (same code); salt
// products as `3 * salt` / `7 * salt` 348 (same code); the wander amount as one conditional expression 348
// (same code). Retail's caller sets the output with `lea ebx, [esp+0xc4]` and pushes only the index, and
// this body saves only EBP/ESI/EDI: the output arrives in EBX and is never written there.
// Round 22 (lane 17, fast): the target point local renamed 348 and the direction local renamed 348 (identical
// code: names do not order this frame's slots); the salt written at both uses re-measured 329 (retail's stack
// index, the rest offset by retail's 0x18 to_target slot and early xmm4 zero), with either rename 329. Retail
// scales to_target as `s * i` from memory (copy of s) but j and k in place in xmm2/xmm1, the mixed pattern
// of i written by name and j/k written through a pointer; ours copies s for all three.
// Y488: plain lead/turn/drift component loops, seven subsets, 347/358/358/362/362/355/355;
// all with twice-used drift tick 355. Lead-only reaches 1463 bytes but gains one; restored.
// Stacked index/output EBX transport and normalization/rotation homes remain.
// Y492: complete clamped time/lead/turn-amount producers in seven subsets
// 348/348/348/352/352/352/352; all with clamped drift producer 366.
// All restored; index/output transport and normalization/rotation homes remain.
// Round 495: Complete normalizer input vector/scalar snapshots with reciprocal/result
// variants 464 (eight forms); forced original graphs 410/348, forced shared-reciprocal
// single-exit graphs 398/344 (344 raises other users 0 to 8 and 3 to 10). All helper
// changes restored; stacked-index/output transport and component homes remain.
// Round 497: Uniform parallel/cosine-axis/sine-cross term subsets 366/348/348/348/366/348/348; all
// with per-use lead field 369. Original body restored; stacked-index/output-EBX transport and
// normalized component homes remain.
// Round 244: existing moving/havok object getters, each with early/drift-local salt
// and common/branch-final point output: 348/377/365/382, twice. Original macro,
// salt, point workspace and final publication restored; frame 0x30 agrees, stacked
// index/output-EBX contract and point/direction homes remain.
// M287: Complete multi-use normalized-component storage in six producer
// orders, then mutable/immutable turn-scope ownership:
// 348/348/348/348/348/348/348/348. Original vector reads restored;
// stacked-index/output transport and normalization homes remain.
// Round 43 (lane 15, fast with 0xf86f0; 0xf8eb0 946 throughout): normalize3d alone 344 (1454 bytes), a gain of 4, not kept; a
// single-exit normalizer local to this function (three orders of the sum and the reciprocal, diagnostic) 346/346/346 (1454);
// output at the three exits 377 (1477), with normalize3d 358 (1464); the salt in the wander branch 365 (1469), with normalize3d
// 359 (1456); both 382 (1480), all three 345 (1467). Retail tests `ja` to the zero arm with j and k already in xmm2/xmm1 and
// the zero in xmm4 from the entry; no plain form brings them there.
// Round 2 of #1061: full statement walk against retail found no behaviour difference. Checked: the object and tag
// lookups, the f86f0 call (target in ecx, output in edi), to_target i/j/k order, the normalizer's skip path (to_target
// untouched, distance 0), the lead block (+0x198 > 0, speed > 0.0001, ba1d0 and 30bf0 arguments, the two 0..2 pins, the
// lead field re-read, the i/j/k adds), the drift test (+0xbc bit 9 and distance > 0.0001; bit 7 tested separately for
// the wander arm), the up x to_target cross products, the clock (null and active tests, game_time * rate * 6.5), the
// 0.05 pin, the dot/cross/cos/sin terms of every component, the wander range, its 10/2 thresholds and 0.8 pin, the
// 3/7 salts with the 0xffff masks, the two 17c900 calls (7*salt first, then 3*salt) and the cos/sin of yaw/pitch, and
// every constant at its retail address (0.0001, 1, 2, 6.5, 0.05, 0.8, 0.1, 10, 1/3, 2pi, pi/2, pi). The only
// differences are the three output stores (retail stores straight to the output in each exit; ours copies the point
// once, same values) and the angle/cosine reusing the dead index slot (esp+0x40). The retail caller (call at 0xf91a6: `push
// index; lea ebx, [esp+0xc4]; call`) proves the callee convention (index on the stack, output in ebx); only the salt
// written at both uses plus the three exits gives it (328, caller 0xf8eb0 946 -> 969, so the pair rises by 3,
// 1294 -> 1297, and the form is not kept). On that form: the three wander components computed into locals before the point set 328
// (same code); `game_time` named in the wander branch 328 (same); wander direction vector 362 (1482 bytes, its tail is
// retail's); the drift rotation as a named vector then scaled and stored 330 (stores land last as retail, schedule still
// differs). Remaining as before: the early xmm4 zero, to_target kept in xmm1-3 across the lead merge, the rotation's
// register homes.
// MiniMax A (11-13): no new form, no lower score (348): the frame agrees (0x30) but the entry register allocation differs, and
// the body is four bytes longer than retail's.
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
// MiniMax I (11-11): no new form, no lower score (504): the flag word still goes through EAX where retail keeps it in EDI for the OR and the store, the flag test still comes before the definition lookup where retail makes it after, and the frame's slots are in another arrangement.
// MiniMax H (11-11): no new form, no lower score (504); the flag word still goes through EAX where retail keeps it in EDI for the OR and the store, the flag test comes before the definition lookup where retail makes it after, and the frame's slots are in another arrangement.
// MiniMax S (11-10): no new form, no lower score (504): the flag word still goes through EAX where retail keeps it in EDI for the OR and the store, the flag test comes before the definition lookup where retail makes it after, and the frame's slots are in another arrangement.
// MiniMax A (11-10): no new form, no lower score (504): the frame of 0x184 and the first lookup
// agree, but the header offset's stack home and the flag reload and store differ, and the
// 0xfcea0 index that retail loads as a word costs that callee 129 points if it is changed.
// MiniMax R (11-10): no new form, no lower score (504): the home of the parent and owner block, the probe's material term and the order of the two damage stores still differ; every declaration order of the Booleans, the parent and the probe measured the same code as the body above.
// MiniMax D (11-10): the parent index read from the projectile at each use instead of in a local (fast 519 -> 504, 2171 bytes);
// the flag word is still loaded once, so the parent lookup still reloads the table pointer and the tail stays one dword copy out.
// score 519 (was 535; 527 before round 1): Frame 0x184 agrees; the stack slots and the flag word differ, see below.
// Round 1 of #1061: full statement walk against retail found no behaviour difference (every offset, constant, condition,
// callee and argument order agrees). Kept: `switch (mode) { case 2: case 4: ... }` for the a84e0 test (retail's movsx/sub/sub
// chain; also settles the damage tail, 535 -> 527), native flag bits for the +0x12c tests and stores (child stores stay two
// separate or/store pairs), the definition bit 3 as `flag_bits.unknown3`, `if (attached) |= 2 else |= 1`, the
// random-seed pointer in a local (retail loads 0x4e7408 once into esi), the probe index `dword` with a `probe` pointer
// (retail's edi += 0xc / esi += 0xc / cmp edi, 0xc; 527 -> 518/519), and the area-damage object lookup after d6660 (same score).
// Remaining: retail re-reads [ebp+0x12c] for the first set, each test and each set (ours reuses the loaded word, 5 sites), the
// parent +0x10a bit 2 is `mov al; shr al, 2; test al, 1` (ours test byte, 4), the centre scale loads each component and
// multiplies by the inverse (ours scales the inverse), material_index lives in a 16-bit slot shared with attached_parent (ours
// stores it as a dword), and the home of attached (0x18)/attached_parent (0x14), owner/point/block position (all 0x30) and up
// (0x54). Tried: declaration orders of the Booleans/parent/probe/material 527 (no change), owner and current in their own
// block (no change), `inverse * center.x` (no change), unsigned index without the pointer 518, a word bit view for +0x10a not
// tried (layout-only struct).
// Earlier: native definition bit 533; projectile/child flag bits 527; definition bit plus parent field reads 527;
// common exit 527; saved-position role name 527; distinct direct/area damage names 527;
// probe-table pointer loop 530; probe declarations reversed 527; all native flag tests/stores 533;
// unsigned table index 539; material declared at entry 527; parent/Boolean declarations first 527.
// @retail 0xfc330
void __stdcall function_fc330(long projectile_index, bool detach_contrail, real contrail_time)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);

	if (TEST_FIELD_BIT(projectile->flags.unknown12))
		return;

	s_projectile_definition *definition = PROJECTILE_DEFINITION_GET(projectile->tag_index);
	byte *definition_bytes = (byte *)definition;
	real damage_scale = *(real *)((byte *)projectile + 0x188);
	bool attached = false;
	long attached_parent = NONE;
	bool airburst = true;

	projectile->flags.unknown12 = true;
	if (TEST_FIELD_BIT(definition->flag_bits.unknown3) && !(TEST_FIELD_BIT(projectile->flags.unknown6)) &&
		*(long *)((byte *)projectile + 0x14) != NONE)
	{
		byte *parent = (byte *)PROJECTILE_GET(*(long *)((byte *)projectile + 0x14));
		long stuck_count = 0;

		if (!(TEST_FIELD_BIT((*(byte *)(parent + 0x10a) >> 2) & 1)))
		{
			for (long child_index = *(long *)(parent + 0x10); child_index != NONE; )
			{
				s_projectile *child = PROJECTILE_GET(child_index);

				if (child->tag_index == projectile->tag_index && !(TEST_FIELD_BIT(child->flags.unknown6)))
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

				if (child->tag_index == projectile->tag_index && !(TEST_FIELD_BIT(child->flags.unknown6)))
				{
					if ((short)stuck_count <= *(short *)(definition_bytes + 0xe6))
					{
						point3f point;

						child->flags.unknown6 = true;
						child->flags.unknown12 = true;
						child->unknown158 = 0.0f;
						child->unknown160 = 0.0f;
						function_b9dd0(child_index, &point);
						center.x += point.x;
						center.y += point.y;
						center.z += point.z;
					}
					else
					{
						dword *seed = &g_4e7408->unknown0;

						child->unknown158 = function_259a0(seed) * child->unknown158;
						child->unknown160 = function_259a0(seed) * child->unknown160;
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
			/* the projectile leaves its parent: retail passes the projectile
			   index in ebx (0xfc5a6 loads it from the first argument) */
			function_b9a90(projectile_index);
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

		if (TEST_FIELD_BIT(projectile->flags.unknown10))
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
		for (dword i = 0; i < 1; i++)
		{
			s_collision_result_1697c0 collision;
			vector3f vector;
			vector3f const *probe = &g_45326c[i];

			vector.i = probe->i * probe_length;
			vector.j = probe->j * probe_length;
			vector.k = probe->k * probe_length;
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
	if (material_index != NONE && !(TEST_FIELD_BIT(projectile->flags.unknown9)))
	{
		projectile->flags.unknown9 = true;
		effect_flags = 4;
		function_fcea0(projectile->tag_index, &position, &forward, &owner, material_index,
			(vector3f const *)((byte *)projectile + 0x134));
	}
	if (effect_index != NONE)
	{
		if (attached)
			effect_flags |= 2;
		else
			effect_flags |= 1;
		function_fcbc0(projectile->tag_index, &position, &forward, &owner, attached, airburst);
	}
	if (*(long *)(definition_bytes + 0x124) != NONE)
		effect_flags |= 8;
	switch (g_4e6948->mode)
	{
	case 2:
	case 4:
		break;
	default:
		if (effect_flags)
			function_a84e0(projectile_index, &material_index, (vector3f const *)((byte *)projectile + 0x134), effect_flags);
		break;
	}

	if (g_4e6948->mode != 4)
	{
		long area_index;

		if (attached)
			area_index = *(long *)(definition_bytes + 0x11c);
		else if (TEST_FIELD_BIT(projectile->flags.unknown10))
			area_index = *(long *)(definition_bytes + 0x158);
		else
			area_index = *(long *)(definition_bytes + 0x104);

		if (area_index != NONE)
		{
			s_type_1e6529 data;
			data.material_index = NONE;
			function_d6660(&data, area_index);
			data.unknown84 = *(definition_bytes + 0x128) & 0x3f;

			s_projectile *object = PROJECTILE_GET(projectile_index);

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
// Round 1 of #1061: statement-by-statement walk of all 1055 retail instructions against this source (control flow, loop
// rotation, field offsets, 1.0/0.5/0.0/0.0001/0.3 constants read from the XBE, condition directions, callee argument
// registers and stack order, the end switch and its fall-through): no behaviour difference found. What differs is
// layout (retail frame 0x118, collision at +0xcc, own slots for guidance forward/up/aim point/movement; ours 0x110)
// and register/scheduling. Retail expands 0xfaa30 inline at two of its five uses (the 1.0 test before the loop and
// the predicted case at the end, both re-reading the data array with the saved index * 12 in [esp+0x7c]); it emits
// the range check's call twice (0xf97b4 and 0xf97ca, one per arm of the average_speed test), so its code has six
// sites for the same five statements as ours (actions 1, 2, 1, 1 at collision_count == 10, 2). A local inline copy
// at the first use 1040, at both 1055 (4750 bytes), so all five stay calls to 0xfaa30.
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
// MiniMax A (11-14): no new form, no lower score (1196): the frame is 0x220 against retail's 0x218, this build pushes
// ebp where retail keeps the state in esi, and the tag's home is at esp+0x228 where retail has it at esp+0x220, so
// every later offset is shifted.
// MiniMax H (11-14): no new form, no lower score (1196): this round re-read the diff: the frame stays 0x220 against retail's 0x218 and the tag's home stays four bytes higher, and the collision count's width change was measured before at 1190 only by regressing the caller from 946 to 1037.
// MiniMax B (11-13): no new form, no lower score (1196): the frame is 0x220 against retail's 0x218, this build pushes ebp where retail keeps the state in esi, and the tag's home is at esp+0x228 where retail has it at esp+0x220, so the body's register plan differs from the first instruction.
// MiniMax H (11-12): no new form, no lower score (1196): the frame is still 0x220 against retail's 0x218, the velocity argument still arrives on the stack where retail takes it in eax and keeps it in a callee-saved register, and 0xfd7d0's register arguments still differ.
// MiniMax S (11-11): no new form, no lower score (1220); retail's frame is eight bytes smaller and it keeps the biped in ebp where ours keeps it in ebx, and no form tried this round matches both, so the body stands as found.
// MiniMax G (11-11): the material index is a dword local here, so the call to 0xfd7d0 pushes it raw as retail
// does instead of sign-extending a word, and the store into the projectile keeps its 16-bit width.
// MiniMax S (11-10): no new form, no lower score (1190); the frame is 0x220 against retail's 0x218, the velocity argument arrives on the stack where retail takes it in eax and keeps it in a callee-saved register, and the collision, effect and sound homes still differ. The sound and effect records start at the same address as retail's, so the eight extra frame bytes sit in the small scalar homes; the velocity pointer is kept in esi on both sides, but only retail's callers hand it over in eax.
// MiniMax G (11-12): no new form tried this round, no lower score (1196): the frame is still 0x220 against
// retail's 0x218 and the velocity argument still arrives on the stack instead of in eax.
// score 1196 (was 1220): the frame is 0x220 against retail's 0x218, the velocity arrives on the stack where retail
// takes it in eax, and 0xfd7d0's register arguments (retail takes the projectile index in eax and three of the six
// out-pointers in ecx, edx and edi) still differ; that split is the compiler's choice for the callee.
// score 1190: Frame 0x220 versus 0x218, stacked velocity and normalization/effect scheduling remain.
// Round 1 of #1061 (logic walk against retail, all blocks): no statement-level behaviour difference found except the
// two below. collision_count is a dword in retail (`cmp dword ptr [esp+0x23c], 1`; the caller does `movsx edx, ax;
// push edx`): `long collision_count` 1190 -> 1180 (4848 -> 4836 bytes) and caller 0xf8eb0 then pushes it as retail
// does, but scores 946 -> 1037 (4691 -> 4718 bytes, positional shift), so it is not kept (the type changes no behaviour).
// Source order (round 2 of #1061): retail stores NONE to the damage record's material (+0x7c) BEFORE calling 0xd6660
// in both damage scopes, and 0xd6660 then overwrites it with g_47d8e0; the store now comes first, as retail's. No
// behaviour change: g_47d8e0 is a constant NONE (src/unknown_187ec0.cpp, never written), so either order leaves NONE
// (score unchanged). A goto to share the
// `*velocity = *g_4687a4` tail 1180 (same bytes). Retail inlines function_faa30 at three of its four sites (the
// unknowndc^2 test and both action-switch arms) and calls it only in the 0.09 block; ours calls all four.
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
// Round 22 (m3 lane 15): 1197 -> 1190 from 0xfd4c0 taking the material type (both calls wrap the index). The
// velocity stays on the stack because the unmatched 0xfd7d0 takes an out-pointer in ESI, so ours reloads the
// velocity after that call (BLOCKED-BY 0xfd7d0); retail also pushes the material to it raw (16-bit parameter).
// Material local as c_type_47f957 1210; second lookup from c_type_47f957(damage.material_index) 1196; the damage
// record view's material field typed c_type_47f957 1196 (0xfc330 527 -> 522); both 1210; one function-scope
// damage record 1197 (frame 0x278).
// Round 34 (m3 lane 11): still BLOCKED-BY 0xfd7d0 at entry (our build gives it register pointers in EBX/EDI/ESI/ECX and
// a `long` element where retail passes EAX/EDI/EDX/ECX and pushes the 16-bit value raw). The three projectile flag
// sets of the stop arm as three dword ORs 1190 (same code, VC merges them into one `or 0x814`); through the existing
// bit_vector_set 1279 (4904 bytes): retail's exact three-store sequence, frame 0x21c, the projectile in EBX and the
// response pointer spilled to [esp+0x14] as retail (aligned difference 850 -> 748), but longer. Reflection dot
// through dot3f 1190; grouped `k + (j + i)` 1190 (retail's j, i, k evaluation; masked difference 620 -> 613);
// `j + (i + k)` 1190; `k + (j + i)` with the j operands swapped 1190 (same code).
// Round 36 (m3 lane 11): the reflect arm's object-flag toggle through bit_vector_set(&object_flags, 3, !bit)
// 1192 (retail's two loads, plus a `not cl`); as a bitfield assignment 1277 (4900 bytes). 0xfd7d0 is the
// blocker: with 0x1883e0's class parameters (diagnostic) 0xfd7d0 62 -> 69 and this 1220. Retail's velocity in
// ESI also lets it tail-merge the two `*velocity = *g_4687a4` copies; ours keeps both.
// Round 215: complete native offset/perpendicular/reflected aggregate producers,
// seven nonempty subsets 1190 each; all three readonly completed vectors 1190.
// Original body, vector operands, arithmetic trees, damage scopes and interfaces restored;
// frame 0x220/0x218 and velocity/request homes remain.
// Round 229: full-width potential/settled/created carrier subsets 1213/1197/1222/1187/1216/1206/1229;
// all three short 1228. Created long gains only three and is rejected; all original Boolean
// carriers and executable source restored. Frame 0x220/0x218, velocity and request homes remain.
// MiniMax R (11-11): no new form tried this round, no lower score (1220): the body is 4856 bytes against retail's 4828, so 28 bytes are extra here, and the differences sit in many separate blocks rather than in one local's home.
// MiniMax A (11-13): no new form, no lower score (1196): our frame is 0x220 against retail's 0x218, so every later offset is shifted.
// @retail 0xfaa60
void function_faa60(vector3f *velocity, long projectile_index, s_collision_result_1697c0 const *collision,
	point3f *end_point, vector3f *displacement, short collision_count)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);
	s_projectile_definition *definition = PROJECTILE_DEFINITION_GET(projectile->tag_index);
	long material_index = collision->unknown24;
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

			damage.material_index = NONE; /* retail stores it first; 0xd6660 then writes its default */
			function_d6660(&damage, damage_definition_index);
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

	s_projectile_material_response *response = projectile_get_material_response(definition, c_type_47f957(material_index));
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

		damage.material_index = NONE; /* retail stores it first; 0xd6660 then writes its default */
		function_d6660(&damage, damage_definition_index);
		damage.flags |= 8;
		damage.origin = collision->point;
		damage.position = collision->point;
		damage.direction = *velocity;
		function_30bf0(&damage.direction);
		damage.unknown84 = *((byte *)definition + 0x128) & 0x3f;
		damage.material_index = collision->unknown24;
		damage.unknown80 = projectile_get_material_response(definition, c_type_47f957(collision->unknown24));
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
