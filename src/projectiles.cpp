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
#include "object_iterator.h"
#include "effects.h"
#include "sound_sources.h"
#include <math.h>
#include <string.h>

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
		dword unknown6 : 2;
		dword unknown8 : 1;
		dword unknown9 : 2;
		dword unknown11 : 1;
		dword unknown12 : 2;
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
			dword unknown1 : 4;
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
	real magnitude = (real)sqrt(v->k * v->k + v->j * v->j + v->i * v->i);

	if (!(fabs(magnitude) < 0.0001f))
	{
		real inverse = 1.0f / magnitude;

		v->i = inverse * v->i;
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

	projectile->unknown160 = 1.0f;
	projectile->unknown158 = 1.0f;
	*(dword *)&projectile->flags &= ~8;
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

	vector.i = target->x - origin->x;
	vector.j = target->y - origin->y;
	vector.k = target->z - origin->z;

	real length = projectile_normalize(&vector);
	real seconds = 0.0f;

	if (length == 0.0f)
		vector = *g_4687b0;
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

	if (target->node_index == NONE)
	{
		s_object_marker marker;

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

	s_object_marker marker;

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
// @retail 0xf8de0
bool __stdcall function_f8de0(long projectile_index)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);
	vector3f *velocity = &projectile->linear_velocity;
	vector3f displacement = *velocity;
	vector3f delta;
	bool moved;

	if ((!(projectile->unknownc1 & 1) || TEST_FIELD_BIT(projectile->flags.unknown14)) &&
		function_109a00(projectile_index, &delta, TEST_FIELD_BIT(projectile->flags.unknown15), &displacement))
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
	short material_index)
{
	long response_count = definition->material_response_count;
	s_projectile_material_response *result = NULL;

	while (material_index != NONE && material_index >= 0 &&
		material_index < *(long *)((byte *)g_4e034c + 0x150))
	{
		byte *material = *(byte **)((byte *)g_4e034c + 0x154) + material_index * 0xb4;

		if (!material)
			break;

		s_projectile_material_response *response = (s_projectile_material_response *)definition->material_responses;

		for (long i = 0; i < response_count; i++, response++)
		{
			if (response->material_index == material_index)
			{
				result = response;
				break;
			}
		}
		material_index = *(short *)(material + 8);
		if (result)
			return result;
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
void __stdcall function_b93b0(long parent_index, long object_index, long node_index);
void __stdcall function_1e2930(long object_index, long actor_index);
void function_a83e0(long object_index, long parent_index, point3f const *point, long node_index,
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
	parameters->owner.unknown8 = NONE;
	parameters->unknown34 = 0;
	parameters->unknown38 = 0;
	parameters->scale_a = 1.0f;
	parameters->scale_b = 1.0f;
	parameters->unknown3c = 0;
	parameters->unknown30 = 0;
	parameters->color_a = 0xff808080;
	parameters->color_b = 0xff808080;
	parameters->source = 0;
}

/* pushes a projectile, and spins it at random by the push's strength */
// @retail 0xfa820
void function_fa820(long projectile_index, vector3f const *impulse)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);

	if (*(long *)((byte *)projectile + 0x14) == NONE)
	{
		projectile->linear_velocity.i += impulse->i;
		projectile->linear_velocity.j += impulse->j;
		projectile->linear_velocity.k += impulse->k;

		vector3f axis = g_4417f0[random_index(&g_4e7408->unknown0, 1026)];
		real spin = (real)sqrt(length_sq3f(impulse)) * function_x82e52f(&g_4e7408->unknown0, __FILE__, __LINE__) *
			1.5707964f;

		projectile->angular_velocity.i += spin * axis.i;
		projectile->angular_velocity.j += spin * axis.j;
		projectile->angular_velocity.k += axis.k * spin;
		function_fbd40(projectile_index);
		function_b9b90(projectile_index, false);
	}
}

/* the sound of a projectile's impact */
// @retail 0xfcdd0
void function_fcdd0(long definition_index, point3f const *point, vector3f const *forward)
{
	s_projectile_definition *definition = PROJECTILE_DEFINITION_GET(definition_index);
	long sound_index = *(long *)((byte *)definition + 0x124);

	if (sound_index != NONE)
	{
		s_location location = { 0 };
		s_sound_position position = { 0 };

		function_11bed0(&location, point);
		position.position = *point;
		position.compressed_forward = vector3d_compress(forward);
		position.velocity = *g_4687a4;
		position.location = location;
		function_1895f0(&position, 1.0f, sound_index);
	}
}

/* a projectile's detonation (or impact) effect at a point, facing out of
   the surface it hit */
// @retail 0xfcbc0
void function_fcbc0(point3f const *point, vector3f const *normal, long definition_index,
	s_effect_owner const *owner, bool attached, bool airburst)
{
	s_projectile_definition *definition = PROJECTILE_DEFINITION_GET(definition_index);
	s_effect_marker markers[4];
	s_effect_parameters parameters;
	long effect_index;

	markers[0].position = *point;
	markers[1].position = *point;
	markers[2].position = *point;
	markers[3].position = *point;
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
	parameters.markers = markers;
	parameters.marker_count = 4;
	if (owner)
		parameters.owner = *owner;
	effect_new_from_parameters(&parameters);
}

/* the effect values of an effects block's effect (+0x58), NONE without one */
// @retail 0xfd740
void function_fd740(point3f const *point, byte const *effects, long unused, long index,
	long *first_value04, long *second_value04, long *first_value, long *second_value, long *first_value0c,
	long *second_value0c)
{
	long effect_index = *(long const *)(effects + 0x58);

	if (effect_index != NONE)
	{
		function_1883e0(effect_index, false, point, g_47d8e0, unused, index, 0, first_value04, second_value04,
			first_value, second_value, first_value0c, second_value0c);
		return;
	}
	if (first_value04)
		*first_value04 = NONE;
	if (second_value04)
		*second_value04 = NONE;
	if (first_value)
		*first_value = NONE;
	if (second_value)
		*second_value = NONE;
	if (first_value0c)
		*first_value0c = NONE;
	if (second_value0c)
		*second_value0c = NONE;
}

/* the same for what a projectile hit: ignoring distance for multiplayer
   objects the network owns */
// @retail 0xfd7d0
void function_fd7d0(long object_index, point3f const *point, byte const *effects,
	long unused, long index, long *first_value04, long *second_value04, long *first_value, long *second_value,
	long *first_value0c, long *second_value0c)
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
		function_1883e0(effect_index, ignore_distance, point, g_47d8e0, unused, index, 0, first_value04, second_value04,
			first_value, second_value, first_value0c, second_value0c);
		return;
	}
	if (first_value04)
		*first_value04 = NONE;
	if (second_value04)
		*second_value04 = NONE;
	if (first_value)
		*first_value = NONE;
	if (second_value)
		*second_value = NONE;
	if (first_value0c)
		*first_value0c = NONE;
	if (second_value0c)
		*second_value0c = NONE;
}

/* sticks a projectile to what it hit */
// @retail 0xfd560
void function_fd560(long projectile_index, long object_index, long node_index, point3f const *point,
	vector3f const *forward)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);
	s_projectile_definition *definition = PROJECTILE_DEFINITION_GET(projectile->tag_index);

	if (object_index != NONE && (definition->flags & 8))
	{
		short attached_count = 0;

		for (long child_index = *(long *)((byte *)PROJECTILE_GET(object_index) + 0x10); child_index != NONE; )
		{
			s_projectile *child = PROJECTILE_GET(child_index);

			if (child->tag_index == projectile->tag_index && !((*(dword *)&child->flags >> 6) & 1))
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

	function_b75a0(projectile_index, point, forward, NULL, NULL, false);
	function_b77d0(projectile_index, g_4687a4, g_4687a4);
	*(dword *)&projectile->flags |= 8;
	function_b9b90(projectile_index, true);
	if (object_index != NONE)
	{
		function_b93b0(object_index, projectile_index, node_index);

		s_projectile *parent = PROJECTILE_GET(object_index);

		if (((1 << *((byte *)parent + 0xaa)) & 3) && *(long *)((byte *)parent + 0x12c) != NONE && g_4f55d0->active)
			function_1e2930(projectile_index, *(long *)((byte *)parent + 0x12c));
	}

	real seconds = 0.0f;

	if ((*(dword *)&projectile->flags >> 10) & 1)
		seconds = *(real *)((byte *)definition + 0x150);
	else if (definition->flags & 4)
		seconds = *(real *)((byte *)definition + 0xd8);

	real ticks = g_510c54->field_2_3 * seconds;

	if (ticks >= 1.0f)
		*(real *)((byte *)projectile + 0x15c) = 1.0f / ticks;
	function_a83e0(projectile_index, object_index, point, node_index, forward);
}

real function_30bf0(vector3f *v);

/* aims a falling projectile along an arc: the slowest speed that reaches
   the target, then the time to it at the speed used (high or low arc),
   falling back to the slowest arc when the speed can't reach it */
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
		if (forced_speed)
		{
			chosen_speed = *forced_speed;
		}
		else
		{
			chosen_speed = speed;
			if (time_scale && *time_scale > 0.0f)
			{
				real t = PIN(*time_scale * optimal_time, 0.001f, optimal_time);
				real x = -(distance_squared / (t * t) + t * t * a);
				real needed = (real)sqrt(b - x);

				if (speed > needed)
					chosen_speed = needed;
			}
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

		real inverse_time = 1.0f / time;
		vector3f velocity;

		velocity.i = inverse_time * delta.i;
		velocity.j = inverse_time * delta.j;

		real horizontal_speed_value = (real)sqrt(velocity.j * velocity.j + velocity.i * velocity.i);

		velocity.k = inverse_time * delta.k + time * gravity * 0.5f;

		real vertical = velocity.k;
		real path_distance = time * chosen_speed;

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

	if (speed_override)
		speed = *speed_override;
	else
		speed = definition->unknown17c *
			(TEST_FIELD_BIT(definition->flag_bits.difficulty_scaled) ? projectile_difficulty_scale() : 1.0f);

	if ((definition->flags & 2) && definition->gravity_scale > 0.0f)
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
// @retail 0xfc030
bool function_fc030(long projectile_index, point3f const *point, s_collision_result_1697c0 *collision)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);
	s_projectile_definition *definition = PROJECTILE_DEFINITION_GET(projectile->tag_index);
	point3f *origin = (point3f *)((byte *)projectile + 0x64);
	long ignore_object_index = *(long *)((byte *)projectile + 0x140);
	vector3f vector;

	vector.i = point->x - origin->x;
	vector.j = point->y - origin->y;
	vector.k = point->z - origin->z;
	if (function_1697c0(0x2480000f, origin, &vector, ignore_object_index, projectile_index, collision))
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
		vector3f offset_vector;

		start.x = side.i * radius + origin->x;
		start.y = side.j * radius + origin->y;
		start.z = side.k * radius + origin->z;
		offset_vector.i = side.i * radius + point->x - start.x;
		offset_vector.j = side.j * radius + point->y - start.y;
		offset_vector.k = side.k * radius + point->z - start.z;
		if (function_1697c0(0x4800008, &start, &offset_vector, ignore_object_index, NONE, collision))
			return true;

		start.x = (0.0f - radius) * side.i + origin->x;
		start.y = side.j * (0.0f - radius) + origin->y;
		start.z = side.k * (0.0f - radius) + origin->z;
		offset_vector.i = (0.0f - radius) * side.i + point->x - start.x;
		offset_vector.j = side.j * (0.0f - radius) + point->y - start.y;
		offset_vector.k = side.k * (0.0f - radius) + point->z - start.z;
		if (function_1697c0(0x4800008, &start, &offset_vector, ignore_object_index, NONE, collision))
			return true;
	}
	return false;
}

void function_1763a0(point3f const *point, vector3f const *direction, s_effect_marker *markers,
	vector3f const *normal);
long function_189fe0(s_sound_request const *request, long tag_index);

/* the effects and sounds of an effects block (a material's response) at
   a point */
// @retail 0xfcea0
void function_fcea0(long effects_index, point3f const *point, vector3f const *direction,
	s_effect_owner const *owner, long index, vector3f const *normal)
{
	byte *effects = g_4e3b44[effects_index & 0xffff].bytes;
	s_location location;
	long sounds[3];
	long effect_indices[3];
	long i;

	function_11bed0(&location, point);
	function_fd740(point, effects, 0xf, index, &effect_indices[0], &sounds[0], &effect_indices[1], &sounds[1],
		&effect_indices[2], &sounds[2]);
	for (i = 0; i < 3; i++)
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
			request.location.scale = 1.0f;
			request.location.spatial = position;
			request.location.audible = 1;
			request.location.requested_audible = 1;
			request.location.unknown08 = 0;
			request.platform_playback = NONE;
			request.object_index = NONE;
			request.source = NULL;
			request.marker = NULL;
			request.variant = NULL;
			function_189fe0(&request, sound_index);
		}
	}
}

/* a projectile's detonation effects and sounds at a point, attached to an
   object's node when it stuck to one */
// @retail 0xfd0e0
void function_fd0e0(long definition_index, real scale_a, real scale_b, vector3f const *direction,
	point3f const *point, vector3f const *normal, long index, bool attached, long object_index,
	short node_index, bool alternate)
{
	byte *definition = g_4e3b44[definition_index & 0xffff].bytes;
	long sounds[3];
	long effect_indices[3];
	s_location location;
	s_effect_marker markers[6];
	s_effect_parameters parameters;
	long i;

	effect_indices[0] = NONE;
	effect_indices[1] = NONE;
	effect_indices[2] = NONE;
	sounds[0] = NONE;
	sounds[1] = NONE;
	sounds[2] = NONE;
	function_11bed0(&location, point);
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
PRIVATE inline real projectile_random_range(real lower, real upper)
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
// @retail 0xf8200
bool __stdcall function_f8200(long projectile_index, byte const *data, long unused)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);
	s_projectile_definition *definition = PROJECTILE_DEFINITION_GET(projectile->tag_index);
	byte *definition_bytes = (byte *)definition;

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

	if (*(real *)(definition_bytes + 0x184) == *(real *)(definition_bytes + 0x188))
	{
		*(real *)((byte *)projectile + 0x18c) = *(real *)(definition_bytes + 0x184);
	}
	else
	{
		*(real *)((byte *)projectile + 0x18c) =
			projectile_random_range(*(real *)(definition_bytes + 0x184), *(real *)(definition_bytes + 0x188));
	}

	real ticks;

	if (definition->flags & 4)
		ticks = g_510c54->field_2_3 * *(real *)(definition_bytes + 0xd4);
	else if (*(real *)(definition_bytes + 0xd4) == *(real *)(definition_bytes + 0xd8))
		ticks = g_510c54->field_2_3 * *(real *)(definition_bytes + 0xd4);
	else
		ticks = projectile_random_range(*(real *)(definition_bytes + 0xd4), *(real *)(definition_bytes + 0xd8)) *
			g_510c54->field_2_3;
	if (ticks >= 1.0f)
		*(real *)((byte *)projectile + 0x15c) = 1.0f / ticks;

	real arming_ticks = g_510c54->field_2_3 * *(real *)(definition_bytes + 0xcc);

	if (arming_ticks >= 1.0f)
		*(real *)((byte *)projectile + 0x164) = 1.0f / arming_ticks;

	projectile->attachment_index = NONE;
	for (short i = 0; i < *(long *)(definition_bytes + 0x94); i++)
	{
		if (*(long *)(*(byte **)(definition_bytes + 0x98) + i * 0x18) == 'cont')
		{
			projectile->attachment_index = i;
			break;
		}
	}

	real speed = *(real *)((byte *)projectile + 0x16c) * definition->unknown17c;
	vector3f *forward = (vector3f *)((byte *)projectile + 0x70);

	projectile->linear_velocity.i += forward->i * speed;
	projectile->linear_velocity.j += forward->j * speed;
	projectile->linear_velocity.k += forward->k * speed;
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
// @retail 0xf87f0
void function_f87f0(long projectile_index, point3f *aim_point)
{
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

	if ((definition->flags & 0x200) && distance > 0.0001f)
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

		aim_point->x = (to_target.i * along + axis.i * cosine - (axis.j * to_target.k - axis.k * to_target.j) * sine) * amount +
			arg_0e6828.x;
		aim_point->y = (to_target.j * along + axis.j * cosine - (axis.k * to_target.i - to_target.k * axis.i) * sine) *
			amount + arg_0e6828.y;
		aim_point->z = (to_target.k * along + axis.k * cosine - (to_target.j * axis.i - axis.j * to_target.i) * sine) *
			amount + arg_0e6828.z;
		return;
	}

	if (!(definition->flags & 0x80))
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

		long salt = projectile_index >> 16;
		real time_a = (real)((g_510c54->game_time + salt * 3) & 0xffff) * g_510c54->rate;
		real time_b = (real)((g_510c54->game_time + salt * 7) & 0xffff) * g_510c54->rate;
		real yaw = function_17c900(10, time_b * (1.0f / 3.0f)) * 6.2831855f;
		real pitch = 3.1415927f - function_17c900(10, time_a * (1.0f / 3.0f)) * 1.5707964f;
		real cosine_pitch = (real)cos(pitch);

		aim_point->x = (real)cos(yaw) * cosine_pitch * amount + arg_0e6828.x;
		aim_point->y = (real)sin(yaw) * cosine_pitch * amount + arg_0e6828.y;
		aim_point->z = (real)sin(pitch) * amount + arg_0e6828.z;
		return;
	}

	*aim_point = arg_0e6828;
}

void function_b9fc0(long object_index, vector3f *forward, vector3f *up);
bool __stdcall function_bc1d0(long object_index, point3f *point);
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
void function_d7b80(s_type_1e6529 *data, long object_index, short node_index, short unknown0c, short region_entry_index,
	vector3f const *unknown14);
long function_d6c80(s_type_1e6529 *data, long ignore_object_index);

/* the down probe for a surface under a detonation (0x45326c) */
vector3f const g_45326c[1] = { { 0.0f, 0.0f, -1.0f } };

/* detonates a projectile: (once) its sound, contrail, direct and area
   damage, impact or airburst effects, and the effects of the material
   under it */
// @retail 0xfc330
void __stdcall function_fc330(long projectile_index, bool detach_contrail, real contrail_time)
{
	s_projectile *projectile = PROJECTILE_GET(projectile_index);

	if ((*(dword *)&projectile->flags >> 12) & 1)
		return;

	s_projectile_definition *definition = PROJECTILE_DEFINITION_GET(projectile->tag_index);
	byte *definition_bytes = (byte *)definition;
	real damage_scale = *(real *)((byte *)projectile + 0x188);
	bool attached = false;
	long attached_parent = NONE;
	bool airburst = true;
	long parent_index = *(long *)((byte *)projectile + 0x14);

	*(dword *)&projectile->flags |= 0x1000;
	if ((definition->flags & 8) && !((*(dword *)&projectile->flags >> 6) & 1) && parent_index != NONE)
	{
		byte *parent = (byte *)PROJECTILE_GET(parent_index);
		long stuck_count = 0;

		if (!((*(byte *)(parent + 0x10a) >> 2) & 1))
		{
			for (long child_index = *(long *)(parent + 0x10); child_index != NONE; )
			{
				s_projectile *child = PROJECTILE_GET(child_index);

				if (child->tag_index == projectile->tag_index && !((*(dword *)&child->flags >> 6) & 1))
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

				if (child->tag_index == projectile->tag_index && !((*(dword *)&child->flags >> 6) & 1))
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
			function_bc1d0(*(long *)((byte *)projectile + 0x14), &position);
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

		if ((*(dword *)&projectile->flags >> 10) & 1)
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
	if (material_index != NONE && !((*(dword *)&projectile->flags >> 9) & 1))
	{
		*(dword *)&projectile->flags |= 0x200;
		effect_flags = 4;
		function_fcea0(projectile->tag_index, &position, &forward, &owner, material_index,
			(vector3f const *)((byte *)projectile + 0x134));
	}
	if (effect_index != NONE)
	{
		effect_flags |= attached ? 2 : 1;
		function_fcbc0(&position, &forward, projectile->tag_index, &owner, attached, airburst);
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
		else if ((*(dword *)&projectile->flags >> 10) & 1)
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
				real along = (axis.i * velocity.i + axis.k * velocity.k + axis.j * velocity.j) * (1.0f - cosine);
				vector3f turned;

				turned.i = cosine * velocity.i + along * axis.i - (axis.k * velocity.j - axis.j * velocity.k) * sine;
				turned.j = cosine * velocity.j + axis.j * along - (axis.i * velocity.k - axis.k * velocity.i) * sine;
				turned.k = cosine * velocity.k + axis.k * along - (axis.j * velocity.i - axis.i * velocity.j) * sine;
				velocity = turned;

				if (TEST_FIELD_BIT(definition->flag_bits.unknown6))
				{
					vector3f forward = to_aim;
					vector3f up = projectile->up;
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
			vector3f forward = projectile->forward;
			vector3f up = projectile->up;

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

				real along = (forward.i * up.i + forward.k * up.k + up.j * forward.j) * (1.0f - projectile->rotation_cosine);
				vector3f spun;

				spun.i = along * forward.i + up.i * projectile->rotation_cosine -
					(forward.k * up.j - up.k * forward.j) * projectile->rotation_sine;
				spun.j = forward.j * along + up.j * projectile->rotation_cosine -
					(up.k * forward.i - forward.k * up.i) * projectile->rotation_sine;
				spun.k = forward.k * along + up.k * projectile->rotation_cosine -
					(forward.j * up.i - up.j * forward.i) * projectile->rotation_sine;
				up = spun;
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
void function_184060(long unknown3c, byte unknown59, s_type_1e6529 *data, long unknown50);
void function_a85c0(long projectile_index, s_collision_result_1697c0 const *collision, vector3f const *direction,
	bool unknown, real scale_a, real scale_b);

/* the effect parameters of a projectile's impact effects: its owner, and
   the object it hit */
PRIVATE inline void projectile_impact_effect_parameters(s_effect_parameters *parameters, long projectile_index,
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
	s_type_1e6529 damage;
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

	if ((definition->flags >> 4) & 1)
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
		TEST_FIELD_BIT(definition->flag_bits.drifts) && !((*(dword *)&projectile->flags >> 13) & 1) &&
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

				if (vitality > 0.0f && !((object[0x10a] >> 2) & 1) && *(long *)((byte *)projectile + 0xc8) != object_index)
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
