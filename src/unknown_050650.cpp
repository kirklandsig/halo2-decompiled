// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_050650.CPP: actor direction selection and looking state. */

#include "unknown_11c920.h"
#include "data_array.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "props.h"
#include "unknown_0259a0.h"
#include "unknown_1e3920.h"
#include "unknown_11cc90.h"

/* A local view of the shared actor datum; other modules own the array. */
struct s_actor_looking_view
{
	byte unknown000[0x18];
	long unit_index;
	byte unknown01c[0x40 - 0x1c];
	bool attention_enabled;
	byte unknown041[0x54 - 0x41];
	long character_definition_index;
	byte unknown058[0x84 - 0x58];
	short field_xe428b1;
	short alert_state;
	byte unknown088[0x229 - 0x88];
	bool unknown229;
	byte unknown22a[2];
	point3f position;
	byte unknown238[0x264 - 0x238];
	bool direction_locked;
	byte unknown265;
	bool using_object;
	bool seated;
	byte unknown268[4];
	long object_index;
	short look_range_state;
	byte unknown272[0x27c - 0x272];
	s_location_view location;
	vector3f forward;
	byte unknown29c[0x328 - 0x29c];
	short unknown328;
	byte unknown32a[0x338 - 0x32a];
	long field_x50962e;
	long target_marker;
	bool target_marker_valid;
	byte unknown341[0x358 - 0x341];
	short attention_state;
	byte unknown35a[0x370 - 0x35a];
	point3f attention_point;
	byte unknown37c[0x41c - 0x37c];
	short aiming_mode;
	byte unknown41e[2];
	short aiming_direction_type;
	byte unknown422[2];
	long aiming_prop_index;
	byte unknown428[0x430 - 0x428];
	short looking_mode;
	byte unknown432[2];
	short looking_direction_type;
	byte unknown436[2];
	long looking_prop_index;
	byte unknown43c[0x48a - 0x43c];
	bool unknown48a;
	byte unknown48b[0x50c - 0x48b];
	bool path_active;
	byte unknown50d[0x539 - 0x50d];
	char path_count;
	char path_index;
	byte unknown53b[0x548 - 0x53b];
	s_actor_path_point path[4];
	byte unknown5b8[0x5d0 - 0x5b8];
	bool movement_aiming;
	bool movement_aiming_valid;
	byte unknown5d2[0x5ec - 0x5d2];
	vector3f field_x6d9d38;
	vector3f movement_aiming_direction;
	byte unknown604[0x684 - 0x604];
	short override_priority;
	short override_timer;
	short override_direction_type;
	byte unknown68a[2];
	vector3f field_x013329;
	long idle_aiming_timer;
	long idle_looking_timer;
	short idle_aiming_direction_type;
	byte unknown6a2[2];
	vector3f idle_aiming_direction;
	short idle_looking_direction_type;
	byte unknown6b2[2];
	vector3f idle_looking_direction;
	byte unknown6c0[0x6ce - 0x6c0];
	short animation_timer;
	byte unknown6d0;
	bool unrestricted_looking;
	bool attention_selected;
	byte unknown6d3;
	vector3f reference_vector;
	vector3f field_xe66477;
	vector3f field_x6f2625;
	bool aiming;
	byte unknown6f9[0x758 - 0x6f9];
	vector3f combat_direction;
	byte unknown764[0x80c - 0x764];
	short output_mode;
	byte unknown80e[2];
	dword output_flags;
	byte unknown814[0x828 - 0x814];
	vector3f output_reference;
	vector3f output_aiming;
	vector3f output_looking;
	point3f output_point;
	byte unknown858[0x888 - 0x858];
};

static inline s_actor_looking_view *actor_looking_get(long actor_index)
{
	return (s_actor_looking_view *)g_4f55f0->data + (actor_index & 0xffff);
}

// @retail 0x298b60
bool function_298b60(long actor_index)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	bool result = false;
	if (actor->aiming && actor->aiming_mode >= 2)
	{
		if (actor->aiming_direction_type == 2 ||
			(actor->aiming_direction_type == 1 && actor->aiming_prop_index == actor->field_x50962e))
			result = true;
	}
	return result;
}

// @retail 0x298bc0
bool function_298bc0(long actor_index)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	bool result = false;
	if (actor->looking_mode == 0)
	{
		if (actor->alert_state >= 3)
			return function_298b60(actor_index);
	}
	else if (actor->looking_mode == 2)
	{
		if (actor->looking_direction_type == 2 ||
			(actor->looking_direction_type == 1 && actor->looking_prop_index == actor->field_x50962e))
			result = true;
	}
	return result;
}

/* Name inferred from the timer and direction initialization. */
// @retail 0x297560
PRIVATE void function_297560(long actor_index)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	real ticks = (real)g_510c54->field_2_3;
	long rounded_ticks;
	/* Retail rounds using the current x87 rounding mode. */
	__asm
	{
		fld ticks
		fistp rounded_ticks
	}
	actor->idle_aiming_timer = (short)rounded_ticks;
	actor->idle_looking_timer = (short)rounded_ticks;
	actor->idle_aiming_direction_type = 4;
	actor->idle_looking_direction_type = 4;
	actor->idle_aiming_direction = actor->forward;
	actor->idle_looking_direction = actor->forward;
}

// @retail 0x297600
PRIVATE void function_297600(long actor_index)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	/* Retail clamps after converting each decremented integer timer to real. */
	real aiming = (real)(actor->idle_aiming_timer - 1);
	actor->idle_aiming_timer = (long)(aiming > 0.f ? aiming : 0.f);
	real looking = (real)(actor->idle_looking_timer - 1);
	actor->idle_looking_timer = (long)(looking > 0.f ? looking : 0.f);
}

real normalize2d(point2f *vector);

static inline real actor_looking_normalize2d(point2f *vector)
{
	real magnitude = (real)sqrt(vector->x * vector->x + vector->y * vector->y);
	if (!(fabs(magnitude) < 0.0001f))
	{
		real scale = 1.f / magnitude;
		vector->x = vector->x * scale;
		vector->y = scale * vector->y;
		return magnitude;
	}
	return 0.f;
}

/* A switch selects whether the vertical component is compared. */
// @retail 0x296d60
PRIVATE bool function_296d60(real cosine, vector3f const *aim, vector3f const *reference, bool three_dimensional)
{
	bool result = false;
	if (three_dimensional)
		return reference->k * aim->k + reference->j * aim->j + reference->i * aim->i > cosine;
	point2f aim_horizontal = { aim->i, aim->j };
	point2f reference_horizontal = { reference->i, reference->j };
	if (actor_looking_normalize2d(&aim_horizontal) > 0.f &&
		normalize2d(&reference_horizontal) > 0.f &&
		reference_horizontal.y * aim_horizontal.y + aim_horizontal.x * reference_horizontal.x > cosine)
		result = true;
	return result;
}

// @retail 0x296600
PRIVATE real function_296600(long actor_index, long prop_ref_index)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	s_prop_datum *reference = prop_ref_get(prop_ref_index);
	s_type_5cfb45 *state = function_25d690(reference);
	s_type_f95cd3 *view = NULL;
	if (reference->tracking_index != NONE)
	{
		s_type_e5ff81 *tracking = tracking_get(reference->tracking_index);
		if (tracking)
			view = &tracking->view;
	}
	real interest = 0.f;
	if (reference->state >= 1 && reference->state <= 2)
	{
		if (state->unknown5e)
		{
			short ticks = *(short *)((byte *)state + 0x5c);
			interest = 7.f > (real)ticks * g_510c54->rate ? 1.8f : 0.4f;
		}
		else
			interest = prop_get(reference->prop_index)->unknown23 ? 2.f : 1.f;
	}
	else if (prop_get(reference->prop_index)->unknown23 && reference->state >= 3)
		interest = 1.5f;
	if (reference->object_index == actor->object_index || state->unknown3c == actor->object_index)
		interest = 0.f;
	real scale = state->unknown3c == NONE ? 1.f : 1.5f;
	if (view)
	{
		switch (*(char *)((byte *)view + 0x3a))
		{
		case 1: interest += scale * 0.5f; break;
		case 2: interest += scale; break;
		case 3: interest += scale * 2.f; break;
		}
	}
	if (state->unknown63)
		interest += scale * 2.f;
	if (view)
	{
		switch (*(char *)((byte *)view + 0x38))
		{
		case 1: return interest * 0.6f;
		case 3: return interest * 0.4f;
		case 4: break;
		default: return interest;
		}
	}
	return interest * 0.2f;
}

struct s_actor_looking_properties
{
	real aiming_yaw;
	real aiming_pitch;
	real looking_yaw;
	real looking_pitch;
	real aiming_cosine;
	byte unknown14[4];
	real looking_cosine;
	byte unknown1c[4];
	real idle_aiming_angle;
	real idle_looking_angle;
	real moving_aiming_angle;
	real moving_looking_angle;
	real_bounds idle_time;
	real_bounds alternate_idle_time;
	real_bounds alert_idle_time;
	real_bounds alternate_alert_idle_time;
};

/* The original character-property lookup has not been recovered yet. */
s_actor_looking_properties *function_1e5160(long character_definition_index);

struct s_actor_looking_object
{
	long definition_index;
	byte unknown04[0x14 - 4];
	long parent_index;
	byte unknown18[0xaa - 0x18];
	byte type;
	byte unknownab[0x13c - 0xab];
	long player_index;
	byte unknown140[0x1fc - 0x140];
	short seat_index;
	byte unknown1fe[0x346 - 0x1fe];
	short animation_offset;
	byte unknown348[0x3dc - 0x348];
	byte movement_state;
};

struct s_actor_looking_object_header
{
	byte unknown00[8];
	s_actor_looking_object *object;
};

static inline s_actor_looking_object *actor_looking_object_get(long index)
{
	return ((s_actor_looking_object_header *)g_4e0300->data)[index & 0xffff].object;
}

struct s_actor_looking_seat
{
	byte unknown00[0x88];
	real minimum_yaw;
	real maximum_yaw;
	byte unknown90[0xb0 - 0x90];
};

struct s_actor_looking_unit_definition
{
	byte unknown00[0x1cc];
	s_actor_looking_seat *seats;
};

// @retail 0x2973f0
PRIVATE bool function_2973f0(long actor_index, real *looking_cosine, real *aiming_cosine, real *idle_aiming_cosine, real *idle_looking_cosine)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	s_actor_looking_properties *properties = function_1e5160(actor->character_definition_index);
	bool result = false;
	if (properties)
	{
		*aiming_cosine = properties->aiming_cosine;
		result = true;
		if (actor->using_object)
		{
			s_actor_looking_object *object = actor_looking_object_get(actor->object_index);
			s_tag_element *element = function_1e5450(actor_index, object->definition_index);
			if (element)
				*aiming_cosine = (real)cos(*(real *)((byte *)element + 0x70));
		}
		else if (actor->seated)
		{
			s_actor_looking_object *unit = actor_looking_object_get(actor->unit_index);
			if (unit->seat_index != NONE)
			{
				s_actor_looking_object *parent = actor_looking_object_get(unit->parent_index);
				s_actor_looking_unit_definition *definition = (s_actor_looking_unit_definition *)g_4e3b44[parent->definition_index & 0xffff].bytes;
				s_actor_looking_seat *seat = &definition->seats[unit->seat_index];
				real yaw = 0.f - seat->minimum_yaw;
				yaw = yaw > seat->maximum_yaw ? yaw : seat->maximum_yaw;
				*aiming_cosine = (real)cos(yaw);
			}
		}
		*looking_cosine = properties->looking_cosine;
		if (actor->field_xe428b1 == 4)
		{
			*idle_aiming_cosine = (real)cos(properties->moving_aiming_angle);
			*idle_looking_cosine = (real)cos(properties->moving_looking_angle);
		}
		else
		{
			*idle_aiming_cosine = (real)cos(properties->idle_aiming_angle);
			*idle_looking_cosine = (real)cos(properties->idle_looking_angle);
		}
	}
	return result;
}

/* Returns ticks. */
// @retail 0x297c10
PRIVATE long function_297c10(long actor_index, bool alternate_range, bool extended)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	s_game_time_globals *time = g_510c54;
	long result = time->field_2_3 * 3;
	s_actor_looking_properties *properties = function_1e5160(actor->character_definition_index);
	if (properties)
	{
		real lower, upper;
		if (actor->alert_state >= 2)
		{
			if (alternate_range)
			{
				lower = properties->alternate_alert_idle_time.lo;
				upper = properties->alternate_alert_idle_time.hi;
			}
			else
			{
				lower = properties->alert_idle_time.lo;
				upper = properties->alert_idle_time.hi;
			}
		}
		else if (alternate_range)
		{
			lower = properties->alternate_idle_time.lo;
			upper = properties->alternate_idle_time.hi;
		}
		else
		{
			lower = properties->idle_time.lo;
			upper = properties->idle_time.hi;
		}
		if (extended)
		{
			lower *= 1.5f;
			upper *= 1.5f;
		}
		/* Keep the sample and seconds-to-ticks product in x87 precision until
		   retail's store to real immediately before integer rounding. */
		real random = (real)random_next(&g_4e7408->unknown0) * (1.f / 65535.f);
		real ticks = (lower + (upper - lower) * random) * time->field_2_3;
		__asm
		{
			fld ticks
			fistp result
		}
	}
	return result;
}

/* Name inferred from the conditions that suppress direction selection. */
// @retail 0x2982f0
PRIVATE bool function_2982f0(long actor_index)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	bool result = true;
	if (actor->direction_locked)
		result = false;
	else
	{
		s_actor_looking_object *unit = actor_looking_object_get(actor->unit_index);
		if (unit->type == 0 && unit->movement_state == 5)
			result = false;
		else if (actor->field_xe428b1 >= 4 && actor->movement_aiming)
			result = false;
		else if (function_110ab0(actor->unit_index))
			result = false;
	}
	return result;
}

static inline real actor_looking_normalize3d(vector3f *vector)
{
	real magnitude = (real)sqrt(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
	if (!(fabs(magnitude) < 0.0001f))
	{
		real scale = 1.f / magnitude;
		vector->i = scale * vector->i;
		vector->j = vector->j * scale;
		vector->k = vector->k * scale;
		return magnitude;
	}
	return 0.f;
}

static inline void actor_looking_rotate(vector3f *vector, vector3f const *axis, real angle)
{
	real sine = (real)sin(angle);
	real cosine = (real)cos(angle);
	real parallel = (axis->i * vector->i + axis->j * vector->j + axis->k * vector->k) * (1.f - cosine);
	vector3f result;
	result.i = vector->i * cosine + axis->i * parallel - (vector->j * axis->k - vector->k * axis->j) * sine;
	result.j = vector->j * cosine + axis->j * parallel - (vector->k * axis->i - vector->i * axis->k) * sine;
	result.k = vector->k * cosine + axis->k * parallel - (vector->i * axis->j - vector->j * axis->i) * sine;
	*vector = result;
}

// @retail 0x296e60
PRIVATE bool function_296e60(point3f const *origin, vector3f const *forward, bool test_collision,
	real yaw_lower, real yaw_upper, real pitch_lower, real pitch_upper, vector3f *direction)
{
	vector3f pitch_axis = { -forward->j, forward->i, 0.f };
	if (actor_looking_normalize3d(&pitch_axis) == 0.f)
		pitch_axis = *g_4687ac;
	vector3f best_direction = *g_4687a8;
	real best_fraction = 0.f;
	real yaw_range = yaw_upper - yaw_lower;
	real arg_58ecd0 = pitch_upper - pitch_lower;
	bool clear = true;
	for (short attempt = 0; attempt < 10; ++attempt)
	{
		real yaw = (real)random_next(&g_4e7408->unknown0) * (1.f / 65535.f) * yaw_range + yaw_lower;
		real pitch = (real)random_next(&g_4e7408->unknown0) * (1.f / 65535.f) * arg_58ecd0 + pitch_lower;
		vector3f candidate = *forward;
		actor_looking_rotate(&candidate, &pitch_axis, pitch);
		actor_looking_rotate(&candidate, g_4687b0, yaw);
		clear = true;
		if (test_collision)
		{
			vector3f ray = { candidate.i * 3.f, candidate.j * 3.f, candidate.k * 3.f };
			s_collision_result_1697c0 collision;
			collision.unknown24 = NONE;
			clear = !function_1697c0(0x10800001, origin, &ray, NONE, NONE, &collision);
			if (clear)
			{
				actor_looking_normalize3d(&candidate);
				*direction = candidate;
				return true;
			}
			real fraction = *(real *)((byte *)&collision + 4);
			if (fraction > best_fraction)
			{
				best_fraction = fraction;
				best_direction = candidate;
			}
		}
	}
	if (!clear && best_fraction > 0.f)
	{
		actor_looking_normalize3d(&best_direction);
		*direction = best_direction;
		return true;
	}
	/* Retail still draws ten samples when collision testing is disabled,
	   then returns false without writing the output vector. */
	return false;
}

/* The direction decoder has aiming and optional target-point outputs.
   A specification is a type followed by a 12-byte union. */
struct s_type_952051
{
	short type;
	short padding;
	union
	{
		long index;
		point3f point;
		vector3f vector;
	};
};

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
long function_baf80(long object_index);
void function_cafc0(long unit_index, point3f *position);
void function_1caa40(long object_index, point3f *position);
long function_1e1f20(long actor_index);
real function_30bf0(vector3f *v);

/* Dependencies not yet recovered; signatures follow retail's call sites. */
void function_caf60(long unit_index, point3f *position);
void function_1e3b00(long object_index, long mode, point3f const *reference,
	void const *unknown0, void const *unknown1, point3f *position);
void function_1fc710(long actor_index, point3f *position);
bool function_1ffbd0(long actor_index, long weapon_index, short barrel_index,
	point3f const *origin, point3f const *target, bool flag, vector3f *direction,
	void *unknown0, void *unknown1, void *unknown2, void *unknown3);
bool __stdcall function_1ffe00(long actor_index, long object_index);

// @retail 0x2967b0
PRIVATE bool function_2967b0(long actor_index, s_type_952051 *specification,
	bool aiming, vector3f *direction, bool *has_target_point, point3f *arg_0e6828)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	bool result = false;
	bool point_valid = false;
	point3f point;
	s_prop_datum *reference;
	s_type_5cfb45 *state;
	long weapon_index;
	point3f origin;

	switch (specification->type)
	{
	case 0:
		if (!actor->movement_aiming)
			return false;
		*direction = actor->field_x6d9d38;
		if (function_1f8660(actor_index) && actor->object_index == NONE &&
			actor->field_xe428b1 == 4 && actor->path_index < actor->path_count - 1 &&
			magnitude3d(&actor->field_x6d9d38) < 1.f)
			function_210be0(&actor->path[actor->path_index].node,
				&actor->path[actor->path_index + 1].node, direction);
		goto normalize_direction;

	case 1:
		reference = (s_prop_datum *)record_pool_lookup(g_502418, specification->index);
		if (!reference)
			return false;
		state = function_25d690(reference);
		point_valid = true;
		if (reference->state < 1)
			return false;
		if (reference->state <= 2)
		{
			long object_index = reference->object_index;
			s_actor_looking_object *object = actor_looking_object_get(object_index);
			if (((1 << object->type) & 3) && !aiming)
			{
				if (object->player_index != NONE)
					function_cafc0(object_index, &point);
				else
					function_caf60(object_index, &point);
				goto direction_from_actor;
			}
			function_1caa40(object_index, &point);
			goto aim_at_point;
		}
		goto remembered_prop;

	case 2:
		if (actor->field_x50962e == NONE)
			return false;
		reference = prop_ref_get(actor->field_x50962e);
		state = function_25d690(reference);
		point_valid = true;
		if (reference->state >= 1 && reference->state <= 2)
		{
			if (aiming)
			{
				long object_index = function_baf80(reference->object_index);
				if (!actor->target_marker_valid)
				{
					function_1ffe00(actor_index, object_index);
					actor->target_marker_valid = true;
				}
				s_object_marker marker;
				if (actor->target_marker && function_b8d30(object_index, actor->target_marker, &marker, 1, false))
					point = marker.matrix.position;
				else
					point = *(point3f *)((byte *)state + 0x10);
				goto aim_at_point;
			}
			long object_index = reference->object_index;
			s_actor_looking_object *object = actor_looking_object_get(object_index);
			if (((1 << object->type) & 3) && object->player_index != NONE)
				function_cafc0(object_index, &point);
			else
				point = *(point3f *)((byte *)state + 0x30);
			goto direction_from_actor;
		}
		goto remembered_prop;

	case 3:
		point = specification->point;
		point_valid = true;
		goto aim_at_point;

	case 4:
		*direction = specification->vector;
		goto normalize_direction;

	case 5:
		if (actor->attention_state <= 0)
			return false;
		point = actor->attention_point;
		point_valid = true;
		goto aim_at_point;

	case 6:
		{
			long object_index = specification->index;
			if (object_index == actor->unit_index)
				return false;
			s_actor_looking_object *object = (s_actor_looking_object *)function_badc0(object_index, 0xffffffff);
			if (!object)
				return false;
			if (((1 << object->type) & 3) && !aiming)
			{
				if (object->player_index != NONE)
					function_cafc0(object_index, &point);
				else
					function_caf60(object_index, &point);
			}
			else
				function_1caa40(object_index, &point);
			point_valid = true;
			goto aim_at_point;
		}
	default:
		return false;
	}

remembered_prop:
	if (state->unknown00 == NONE)
		return false;
	function_210850((s_type_c3b527 const *)&state->unknown48, &point);
	if (aiming)
		goto local_254539;
	function_1e3b00(reference->object_index, 1, &point, NULL, NULL, &point);
	goto direction_from_actor;

aim_at_point:
	if (!aiming)
		goto direction_from_actor;
local_254539:
	weapon_index = function_1e1f20(actor_index);
	if (weapon_index == NONE)
		goto direction_from_actor;
	function_1fc710(actor_index, &origin);
	result = function_1ffbd0(actor_index, weapon_index, 0, &origin, &point, false, direction,
		NULL, NULL, NULL, NULL);
	if (!result)
	{
		vector3d_from_points3d(&origin, &point, direction);
		if (!(function_30bf0(direction) > 0.f))
			return false;
		result = true;
	}
	goto output_point;

direction_from_actor:
	vector3d_from_points3d(&actor->position, &point, direction);
normalize_direction:
	if (!(function_30bf0(direction) > 0.f))
		return false;
	result = true;
	if (!point_valid)
		return result;
output_point:
	if (has_target_point && arg_0e6828)
	{
		*has_target_point = true;
		*arg_0e6828 = point;
	}
	return result;
}

// @retail 0x296580
void function_296580(long actor_index)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	if (actor->aiming_direction_type == 0 && !actor->path_active)
	{
		actor->aiming_mode = 0;
		return;
	}
	if (actor->aiming_mode >= 3 &&
		function_2967b0(actor_index, (s_type_952051 *)&actor->aiming_direction_type,
			true, &actor->movement_aiming_direction, NULL, NULL))
		actor->movement_aiming_valid = true;
	else
		actor->movement_aiming_valid = false;
}

real function_11cc90(vector2f const *a, vector2f const *b);
/* Retail's clamped arccos helper; its implementation is outside this file. */
real function_50650(real cosine);

/* Name inferred from the yaw/pitch tests. Failure to decode or normalize
   leaves the direction accepted, as does the actor's bypass flag. */
// @retail 0x297a50
PRIVATE bool function_297a50(long actor_index, bool aiming, s_type_952051 *specification)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	bool result = true;
	s_actor_looking_properties *properties = function_1e5160(actor->character_definition_index);
	if (actor->unrestricted_looking)
		return result;
	vector3f direction;
	if (properties && function_2967b0(actor_index, specification, aiming, &direction, NULL, NULL))
	{
		vector3f forward = actor->forward;
		vector3f horizontal = direction;
		forward.k = 0.f;
		horizontal.k = 0.f;
		if (function_30bf0(&horizontal) != 0.f && function_30bf0(&forward) != 0.f)
		{
			real cosine = direction.j * horizontal.j + direction.i * horizontal.i + direction.k * horizontal.k;
			cosine = cosine < -1.f ? -1.f : (cosine > 1.f ? 1.f : cosine);
			real pitch = function_50650(cosine);
			if (direction.k < 0.f)
				pitch = 0.f - pitch;
			real yaw = function_11cc90((vector2f *)&horizontal, (vector2f *)&forward);
			if (aiming)
			{
				if (fabs(yaw) > properties->aiming_yaw + 0.01 || fabs(pitch) > properties->aiming_pitch + 0.01)
					result = false;
			}
			else if (fabs(yaw) > properties->looking_yaw + 0.01 || fabs(pitch) > properties->looking_pitch + 0.01)
				result = false;
		}
	}
	return result;
}

/* Values assigned by retail startup initializers 378620, 378680, 3786a0. */
real g_55e5bc = 0.17453292f;
real g_55e5c8 = 0.17453292f;
real g_55e5cc = 0.08726646f;

// @retail 0x297660
PRIVATE bool function_297660(long actor_index, bool use_aiming_direction, bool use_character_bounds,
	s_type_952051 *specification)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	bool result = false;
	real yaw_lower = 0.f, yaw_upper = 0.f, pitch_lower = 0.f, pitch_upper = 0.f;
	real scale = 1.f;
	s_actor_looking_properties *properties = function_1e5160(actor->character_definition_index);
	if (properties)
	{
		vector3f base;
		if (actor->look_range_state > 1)
			scale = 0.6f;
		if (use_character_bounds)
		{
			base = actor->forward;
			base.k = 0.f;
			if (function_30bf0(&base) == 0.f)
				base = *g_4687a8;
			yaw_upper = properties->aiming_yaw * scale;
			yaw_lower = 0.f - yaw_upper;
			real pitch = properties->aiming_pitch * scale;
			real minimum = 0.f - g_55e5bc;
			real lower = 0.f - pitch;
			pitch_lower = minimum > lower ? minimum : lower;
			pitch_upper = g_55e5bc > pitch ? pitch : g_55e5bc;
		}
		else
		{
			if (use_aiming_direction)
				base = actor->field_xe66477;
			else if (!function_2967b0(actor_index,
				(s_type_952051 *)&actor->idle_aiming_direction_type, false, &base, NULL, NULL))
				base = actor->forward;
			vector3f horizontal = base;
			horizontal.k = 0.f;
			if (function_30bf0(&horizontal) != 0.f)
			{
				vector3f aiming_horizontal = actor->field_xe66477;
				aiming_horizontal.k = 0.f;
				if (function_30bf0(&aiming_horizontal) != 0.f)
				{
					real *angles = actor->field_xe428b1 == 4 ? &properties->moving_aiming_angle : &properties->idle_aiming_angle;
					if (use_aiming_direction)
					{
						yaw_lower = 0.f - angles[0];
						yaw_upper = angles[1];
						pitch_lower = -0.17453292f;
						pitch_upper = 0.17453292f;
					}
					else
					{
						real yaw = function_11cc90((vector2f *)&horizontal, (vector2f *)&aiming_horizontal);
						real cosine = base.j * horizontal.j + base.i * horizontal.i + base.k * horizontal.k;
						cosine = cosine < -1.f ? -1.f : (cosine > 1.f ? 1.f : cosine);
						real pitch = function_50650(cosine);
						if (base.k < 0.f)
							pitch = 0.f - pitch;
						real yaw_minimum = yaw - g_55e5c8;
						real yaw_maximum = yaw + g_55e5c8;
						real pitch_minimum = pitch - g_55e5cc;
						real pitch_maximum = pitch + g_55e5cc;
						real yaw_limit = angles[0] * scale;
						real negative_yaw_limit = 0.f - yaw_limit;
						real pitch_lower_limit = scale * -0.17453292f;
						real pitch_upper_limit = scale * 0.17453292f;
						yaw_lower = (yaw_minimum > negative_yaw_limit ? yaw_minimum : negative_yaw_limit) - yaw;
						yaw_upper = (yaw_maximum > yaw_limit ? yaw_limit : yaw_maximum) - yaw;
						pitch_lower = (pitch_minimum > pitch_lower_limit ? pitch_minimum : pitch_lower_limit) - pitch;
						pitch_upper = (pitch_maximum > pitch_upper_limit ? pitch_upper_limit : pitch_maximum) - pitch;
					}
				}
			}
		}
		specification->type = 4;
		result = function_296e60(&actor->position, &base, true,
			yaw_lower, yaw_upper, pitch_lower, pitch_upper, &specification->vector);
	}
	if (!result)
	{
		specification->type = 4;
		specification->vector = actor->forward;
	}
	return result;
}

struct s_actor_looking_structure_view
{
	byte unknown000[0xc4];
	long pathfinding_count;
	s_pathfinding_data *pathfinding;
};

/* Name inferred from the path traces and the idle-direction caller. */
// @retail 0x297d30
PRIVATE bool function_297d30(long actor_index, s_type_952051 *specification)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	s_actor_looking_structure_view *structure = (s_actor_looking_structure_view *)g_4e0348;
	s_pathfinding_data *pathfinding = NULL;
	bool result = false;
	if (structure->pathfinding_count > 0)
		pathfinding = structure->pathfinding;
	if (actor->object_index == NONE && !actor->unknown229 && !actor->direction_locked)
	{
		function_26c180(actor_index);
		if (actor->location.unknown10 != NONE)
		{
			vector3f forward = actor->forward;
			forward.k = 0.f;
			if (function_30bf0(&forward) > 0.f)
			{
				function_210770(actor->location.point.output_index, &forward, &forward);
				s_path_trace_result trace;
				function_26c590(actor->location.unknown10, NULL, &trace, pathfinding,
					&actor->location.point.point, NONE, &forward, 1.25f, 0);
				/* Retail tests the first byte, not the helper's return value. */
				if (*(byte *)&trace.unknown00)
				{
					vector3f directions[8];
					short best_index = NONE;
					real best_score = 0.f;
					long rotation_index = 0;
					vector3f *direction = directions;
					for (short i = 0; i < 8; i++, rotation_index++, direction++)
					{
						*direction = actor->forward;
						real sine = (real)sin((real)rotation_index * 0.785398185f);
						real cosine = (real)cos((real)rotation_index * 0.785398185f);
						vector3f const *axis = g_4687b0;
						real parallel = (axis->i * direction->i + axis->j * direction->j + axis->k * direction->k) * (1.f - cosine);
						vector3f rotated;
						rotated.i = direction->i * cosine + axis->i * parallel - (direction->j * axis->k - direction->k * axis->j) * sine;
						rotated.j = direction->j * cosine + axis->j * parallel - (direction->k * axis->i - direction->i * axis->k) * sine;
						rotated.k = direction->k * cosine + axis->k * parallel - (direction->i * axis->j - direction->j * axis->i) * sine;
						*direction = rotated;
						function_26c590(actor->location.unknown10, NULL, &trace, pathfinding,
							&actor->location.point.point, NONE, direction, 5.f, 0);
						real fraction = *(byte *)&trace.unknown00 ? trace.distance * 0.2f : 1.f;
						real score = direction->i * forward.i + direction->k * forward.k + direction->j * forward.j;
						score = (0.6f > score ? 0.6f : score) * fraction;
						if (score > best_score)
						{
							best_score = score;
							best_index = i;
						}
					}
					if (best_index != NONE)
					{
						vector3f *best = &directions[best_index];
						if (0.7f > best->j * forward.j + best->k * forward.k + best->i * forward.i)
						{
							function_2105b0(actor->location.point.output_index, best, best);
							specification->type = 4;
							specification->vector = *best;
							result = true;
						}
					}
				}
			}
		}
	}
	return result;
}

// @retail 0x2980d0
PRIVATE bool function_2980d0(long actor_index, bool looking, bool use_character_bounds, vector3f *direction)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	s_type_952051 *aiming = (s_type_952051 *)&actor->idle_aiming_direction_type;
	s_type_952051 *look = (s_type_952051 *)&actor->idle_looking_direction_type;
	if (use_character_bounds && actor->unrestricted_looking && actor->attention_enabled &&
		function_297d30(actor_index, aiming))
	{
		actor->idle_aiming_timer = function_297c10(actor_index, use_character_bounds, true);
		actor->idle_looking_timer = function_297c10(actor_index, use_character_bounds, false);
		*look = *aiming;
		actor->attention_selected = true;
	}
	else
	{
		if (actor->idle_aiming_timer == 0 || !function_297a50(actor_index, use_character_bounds, aiming))
		{
			if (use_character_bounds && (random_next(&g_4e7408->unknown0) & 0x8000) != 0 &&
				actor->idle_looking_timer > g_510c54->field_2_3 &&
				function_297a50(actor_index, use_character_bounds, look))
				*aiming = *look;
			else
				function_297660(actor_index, true, use_character_bounds, aiming);
			actor->idle_aiming_timer = function_297c10(actor_index, use_character_bounds, true);
			actor->idle_looking_timer = function_297c10(actor_index, use_character_bounds, false);
			*look = *aiming;
		}
		if (looking && (actor->idle_looking_timer == 0 ||
			!function_297a50(actor_index, use_character_bounds, look)))
		{
			function_297660(actor_index, false, use_character_bounds, look);
			actor->idle_looking_timer = function_297c10(actor_index, use_character_bounds, false);
		}
	}
	if (!function_2967b0(actor_index, looking ? look : aiming, use_character_bounds, direction, NULL, NULL))
		*direction = actor->forward;
	return true;
}

bool function_1fdd90(long actor_index);
bool function_10f630(long object_index, long *first, long *second);
void function_10e9f0(long object_index, short channel, real value, real time);
point3f *function_b9dd0(long object_index, point3f *result);

struct s_actor_looking_animation
{
	byte unknown00[0x36];
	short state;
};

/* A local view of unit request 0x2d; keep the shared request declaration. */
struct s_actor_looking_turn_request
{
	long type;
	short side;
	byte unknown06[2];
	point3f position;
	vector3f forward;
};

// @retail 0x298370
void function_298370(long actor_index)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	s_actor_looking_object *unit = actor_looking_object_get(actor->unit_index);
	short animation_state = ((s_actor_looking_animation *)((byte *)unit + unit->animation_offset))->state;
	bool idle = false;
	bool allow_turn = false;
	bool has_target_point = false;
	point3f arg_0e6828;
	function_297600(actor_index);
	actor->field_xe66477 = actor->forward;
	actor->field_x6f2625 = actor->forward;
	real looking_cosine, aiming_cosine, idle_aiming_cosine, idle_looking_cosine;
	bool forced = false;
	if (function_2973f0(actor_index, &looking_cosine, &aiming_cosine, &idle_aiming_cosine, &idle_looking_cosine))
	{
		vector3f direction;
		bool valid = false;
		if (animation_state == 6)
		{
			s_object_marker marker;
			if (function_b8d30(actor->unit_index, 0x4000095, &marker, 1, false) > 0)
			{
				direction = marker.matrix.forward;
				direction.k = 0.f;
				valid = function_30bf0(&direction) > 0.f;
				aiming_cosine = -1.f;
			}
		}
		else if (function_1fdd90(actor_index) && !actor->unknown48a)
		{
			direction = actor->combat_direction;
			valid = function_30bf0(&direction) > 0.f;
		}
		else if (actor->field_xe428b1 < 4 && actor->override_timer > 0 &&
			(actor->aiming_mode < 4 || actor->override_priority >= 1))
		{
			valid = function_2967b0(actor_index, (s_type_952051 *)&actor->override_direction_type,
				true, &direction, NULL, NULL);
			if (valid)
				forced = true;
		}
		else
		{
			switch (actor->aiming_mode)
			{
			case 0:
				if (actor->field_xe428b1 >= 4 && actor->movement_aiming)
				{
					s_type_952051 movement;
					movement.type = 0;
					if (function_2967b0(actor_index, &movement, true, &direction, NULL, NULL))
					{
						valid = true;
						forced = true;
					}
				}
				/* fall through */
			case 1:
				if (function_2982f0(actor_index) && function_2980d0(actor_index, false, true, &direction))
				{
					valid = true;
					idle = true;
				}
				break;
			case 2:
			case 3:
			case 4:
				valid = function_2967b0(actor_index, (s_type_952051 *)&actor->aiming_direction_type,
					true, &direction, NULL, NULL);
				if (valid && animation_state == 7)
				{
					allow_turn = true;
					aiming_cosine = 0.707106769f;
				}
				break;
			}
		}
		if (valid && !actor->unrestricted_looking && !forced &&
			!function_296d60(aiming_cosine, &direction, &actor->forward, actor->unknown229))
		{
			if (allow_turn)
			{
				if (!function_110ab0(actor->unit_index))
					function_e68c0(0x27, actor->unit_index);
			}
			else
				valid = false;
		}
		if (valid)
		{
			actor->field_xe66477 = direction;
			actor->aiming = true;
		}
		else
		{
			actor->field_xe66477 = actor->reference_vector;
			actor->aiming = false;
		}

		bool looking_valid = false;
		if (animation_state == 6)
		{
			direction = actor->field_xe66477;
			looking_valid = true;
		}
		else if (actor->override_timer > 0 && (actor->aiming_mode < 4 || actor->override_priority >= 1))
			looking_valid = function_2967b0(actor_index, (s_type_952051 *)&actor->override_direction_type,
				false, &direction, &has_target_point, &arg_0e6828);
		else
		{
			switch (actor->looking_mode)
			{
			case 0:
			case 1:
				if (actor->aiming_mode < 4 && actor->unknown328 < 4 &&
					(actor->field_xe428b1 < 4 || !actor->movement_aiming) &&
					function_2982f0(actor_index) && (!idle || actor->alert_state < 3))
				{
					looking_valid = function_2980d0(actor_index, true, false, &direction);
					if (looking_valid)
						idle = true;
				}
				else
				{
					direction = actor->field_xe66477;
					looking_valid = true;
				}
				break;
			case 2:
				looking_valid = function_2967b0(actor_index, (s_type_952051 *)&actor->looking_direction_type,
					false, &direction, &has_target_point, &arg_0e6828);
				break;
			}
		}
		actor->field_x6f2625 = looking_valid ? direction : actor->forward;
		if (!idle)
			function_297560(actor_index);
		if (actor->unrestricted_looking)
		{
			actor->reference_vector = actor->field_xe66477;
			actor->unrestricted_looking = false;
		}
	}
	if (!actor->unknown229)
	{
		actor->reference_vector.k = 0.f;
		if (function_30bf0(&actor->reference_vector) == 0.f)
			actor->reference_vector = actor->forward;
	}
	if (actor->override_timer > 0 && --actor->override_timer <= 0)
		actor->override_priority = 0;
	if (actor->aiming && !function_110ab0(actor->unit_index) && !function_1f8660(actor_index) && animation_state == 5)
	{
		long unit_index = actor->unit_index;
		long first, second;
		if (function_10f630(unit_index, &first, &second) && (first == 0xf000545 || first == 0x10000546))
		{
			bool current_side = first == 0xf000545;
			point2f aiming = { actor->field_xe66477.i, actor->field_xe66477.j };
			point2f forward = { actor->forward.i, actor->forward.j };
			if (normalize2d(&aiming) > 0.f && normalize2d(&forward) > 0.f)
			{
				real yaw = function_11cc90((vector2f *)&aiming, (vector2f *)&forward);
				bool desired_side = yaw > 0.f;
				if (desired_side != current_side && fabs(yaw) > 0.34906584f)
				{
					s_actor_looking_turn_request request;
					request.type = 0x2d;
					request.side = !desired_side;
					function_b9dd0(unit_index, &request.position);
					request.forward = actor->forward;
					function_e6900(unit_index, (s_unit_request *)&request);
				}
			}
		}
	}
	if (actor->animation_timer > 0 && --actor->animation_timer <= 0)
		function_10e9f0(actor->unit_index, NONE, 1.f, 2.5f);
	actor->output_reference = actor->reference_vector;
	actor->output_aiming = actor->field_xe66477;
	actor->output_looking = actor->field_x6f2625;
	s_actor_looking_view *output = actor_looking_get(actor_index);
	if (actor->attention_selected)
		output->output_flags |= 8;
	else
		output->output_flags &= ~8;
	if (has_target_point)
	{
		actor->output_flags |= 0x8000;
		actor->output_point = arg_0e6828;
	}
	actor->output_mode = actor->alert_state < 2;
}
