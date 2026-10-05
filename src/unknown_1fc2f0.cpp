// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1FC2F0.CPP: a tracked point (an aim or look target) and its
   updates */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "slot_handler.h"
#include <math.h>

/* where a point lies: two indices that the tracking copies together */
struct s_tracking_location
{
	short unknown0;
	short unknown2;
};

struct s_tracked_point
{
	point3f point;
	vector3f offset;
	vector3f velocity;
	s_tracking_location location;
	bool unknown28;
	bool unknown29;
};

// @retail 0x1fc2f0
void function_1fc2f0(s_tracked_point *tracked, point3f const *point, bool unknown)
{
	tracked->offset = *g_4687a4;
	tracked->point = *point;
	tracked->unknown29 = false;
	tracked->velocity = *g_4687a4;
	tracked->unknown28 = unknown;
	tracked->location.unknown0 = NONE;
	tracked->location.unknown2 = NONE;
}

struct s_tracking_source
{
	byte unknown00[0x28];
	bool unknown28;
};

struct s_tracking_target
{
	byte unknown00[0xe4];
	real unknowne4;
};

struct s_tracking_result
{
	long unknown0;
	long direction;
};

// @retail 0x1fc620
void function_1fc620(s_tracking_result *result, s_tracking_source const *source, s_tracking_target const *target)
{
	if (source->unknown28)
	{
		if (target->unknowne4 < 0.0f)
			result->direction = 0xb;
		else if (target->unknowne4 > 0.0f)
			result->direction = 0xa;
		else
			result->direction = 1;
	}
}

struct s_tracking_state
{
	byte unknown00[0x2c];
	point3f unknown2c;
	point3f unknown38;
	vector3f velocity;
	s_tracking_location location;
	byte unknown54[0xe8 - 0x54];
	vector3f unknowne8;
};

struct s_tracking_output
{
	dword flags;
	byte unknown04[0xc - 0x4];
	vector3f velocity;
	byte unknown18[0x54 - 0x18];
	bool unknown54;
	byte unknown55[3];
	point3f point;
};

// @retail 0x1fc660
void function_1fc660(s_tracked_point *tracked, s_tracking_state const *state, s_tracking_output *output)
{
	if (tracked->location.unknown0 == state->location.unknown0 && tracked->location.unknown2 == state->location.unknown2)
	{
		vector3f delta;

		vector3d_from_points3d(&state->unknown38, &state->unknown2c, &delta);
		if (length_sq3f(&delta) > 0.01f)
			output->flags |= 4;
	}
	if (tracked->unknown29)
	{
		output->point.x = state->unknowne8.i + tracked->offset.i;
		output->point.y = state->unknowne8.j + tracked->offset.j;
		output->point.z = state->unknowne8.k + tracked->offset.k;
		output->unknown54 = true;
		tracked->unknown29 = false;
	}
}

/* the tracked velocity, its change limited to half a unit per second */
// @retail 0x1fc4b0
void function_1fc4b0(s_tracked_point *tracked, s_tracking_state const *state, s_tracking_output *output)
{
	vector3f velocity;
	vector3f delta;
	real limit;

	function_1fc660(tracked, state, output);
	velocity.i = (state->velocity.i * g_510c54->rate + state->unknown38.x - state->unknown2c.x) * (real)g_510c54->field_2_3;
	velocity.j = (state->velocity.j * g_510c54->rate + state->unknown38.y - state->unknown2c.y) * (real)g_510c54->field_2_3;
	velocity.k = (state->velocity.k * g_510c54->rate + state->unknown38.z - state->unknown2c.z) * (real)g_510c54->field_2_3;
	delta.i = velocity.i - state->velocity.i;
	delta.j = velocity.j - state->velocity.j;
	delta.k = velocity.k - state->velocity.k;
	limit = g_510c54->rate * 0.5f;
	if (limit * limit <= length_sq3f(&delta) &&
		tracked->location.unknown0 == state->location.unknown0 &&
		tracked->location.unknown2 == state->location.unknown2)
	{
		real scale = limit / ((real)sqrt(length_sq3f(&delta)) * g_510c54->rate);

		output->velocity.i = state->velocity.i + scale * delta.i;
		output->velocity.j = state->velocity.j + scale * delta.j;
		output->velocity.k = state->velocity.k + scale * delta.k;
	}
	else
	{
		output->velocity = velocity;
	}
	tracked->velocity = state->velocity;
	tracked->location = state->location;
}

struct s_actor_aim_unit
{
	byte unknown000[0x212];
	char field_212;
	byte unknown213[0x218 - 0x213];
	long weapons[4];
};

bool function_101b80(long weapon_index, short barrel_index, point3f *point);
void function_cafc0(long unit_index, point3f *position);

PRIVATE inline bool actor_weapon_position(long unit_index, point3f *position)
{
	s_actor_aim_unit *unit = (s_actor_aim_unit *)object_get(unit_index);
	short slot = unit->field_212;
	bool result = false;

	if (slot != NONE)
	{
		long weapon_index = unit->weapons[slot];
		if (weapon_index != NONE)
			result = function_101b80(weapon_index, 0, position);
	}
	return result;
}

/* Uses a weapon marker when available, otherwise the unit or actor origin. */
// @retail 0x1fc710
void function_1fc710(long actor_index, point3f *position)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown274 != NONE)
	{
		if (!actor_weapon_position(actor->unknown274, position))
			function_cafc0(actor->unknown018, position);
	}
	else if (!actor_weapon_position(actor->unknown018, position))
	{
		*position = *(point3f *)((byte *)actor + 0x22c);
	}
}
