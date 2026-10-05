// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1A83B0.CPP: the slot handlers of types 0x46, 0x2f, 0x2d and 0x58
   (0x47d980..0x47da7c), which watch a target moving towards the actor */

#include "unknown_11c920.h"
#include "slot_handler.h"
#include "ai_actor.h"
#include "unknown_2626b0.h"
#include "unknown_0259a0.h"

/* the state of a slot of type 0x58 */
struct s_slot_58
{
	s_slot_header header;
	short ticks;
	byte unknown0e[0x40 - 0xe];
};

short __stdcall function_1a8c30(long actor_index, s_slot *slot);
bool __stdcall function_1a9400(long actor_index, s_slot *slot);
void __stdcall function_1a9760(long actor_index, s_slot *slot);
real function_30bf0(vector3f *v);
real function_11e000(point3f const *b, point3f const *a, vector3f const *d);
real function_11e130(point3f const *a0, vector3f const *a, point3f const *b0, vector3f const *b);

/* where the target (actor +0x360) stands relative to the line the actor
   watches (from +0x370 along +0x37c): whether the actor's own position, the
   point at +0x510 and the segment along +0x5ec come within reach */
// @retail 0x1a8fa0
void function_1a8fa0(long actor_index, real distance, bool *near_point, bool *near_actor, bool *near_segment)
{
	s_actor_view *actor = actor_get(actor_index);
	s_ai_object *target = ai_object_get(actor->unknown360);
	bool segment = false;
	bool point;
	bool within;

	if ((real)sqrt(target->velocity.i * target->velocity.i + target->velocity.j * target->velocity.j + target->velocity.k * target->velocity.k) > 0.1f)
	{
		vector3f direction;
		direction.i = actor->unknown370.x - actor->position.x;
		direction.j = actor->unknown370.y - actor->position.y;
		direction.k = actor->unknown370.z - actor->position.z;
		function_30bf0(&direction);
		if (target->velocity.k * direction.k + target->velocity.j * direction.j + target->velocity.i * direction.i > 0.1f)
		{
			point = false;
			within = false;
			segment = false;
			goto done;
		}
	}
	{
		point3f *start = &actor->unknown370;
		vector3f line;
		line.i = (actor->unknown37c.i * 1.5f + start->x) - start->x;
		line.j = (actor->unknown37c.j * 1.5f + start->y) - start->y;
		line.k = (actor->unknown37c.k * 1.5f + start->z) - start->z;
		point3f *position = &actor->position;
		distance += actor->unknown36c;
		within = distance * distance > function_11e000(position, start, &line);
		if (actor->unknown50c && (near_point || near_segment))
		{
			point3f other;
			function_210850(&actor->unknown510, &other);
			real reach = actor->unknown36c;
			point = reach * reach > function_11e000(&other, start, &line);
			if (near_segment && actor->unknown5d0 && !within && !point)
			{
				vector3f segment_vector;
				segment_vector.i = actor->unknown5ec.i * 3.0f;
				segment_vector.j = actor->unknown5ec.j * 3.0f;
				segment_vector.k = actor->unknown5ec.k * 3.0f;
				reach = actor->unknown36c;
				if (reach * reach > function_11e130(start, &line, position, &segment_vector))
					segment = true;
				else
					segment = false;
			}
		}
		else
		{
			point = within;
			segment = false;
		}
	}
done:
	if (near_point)
		*near_point = point;
	if (near_actor)
		*near_actor = within;
	if (near_segment)
		*near_segment = segment;
}

// @retail 0x1a83b0
short __stdcall function_1a83b0(long actor_index, short level, bool active)
{
	short result = function_1a79e0(actor_index, level, active);

	if (result == g_46fbe4)
		result = 0xe;
	return result;
}

// @retail 0x1a92b0
short __stdcall function_1a92b0(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (actor->unknown358 != 0 &&
		prop_node_get(actor->unknown368)->unknown24 >= 1 &&
		!actor->unknown35e)
	{
		real range = actor->unknown39c + 3.0f;
		if (!(actor->unknown398 > range * range) && !actor->unknown225)
		{
			bool near_point;
			bool near_actor;
			bool near_segment;

			result = 1;
			function_1a8fa0(actor_index, 0.0f, &near_point, &near_actor, &near_segment);
			if (near_point || near_segment && !near_actor)
			{
				if (!REFERENCE_EQUAL(actor->unknown418, g_470fa0))
					function_262800(actor_index, actor->unknown418, false);
			}
			if (near_actor || near_segment)
				result = 3;
		}
	}
	return result;
}

// @retail 0x1a93c0
short __stdcall function_1a93c0(long actor_index, s_slot *slot, bool active)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe8;

	if (actor->unknown358 == 0 || actor->unknown35e)
		result = g_46fbe4;
	return result;
}

// @retail 0x1a9400
bool __stdcall function_1a9400(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown040 && actor->unknown358)
	{
		byte *buffer = ai_scratch_buffer_get();
		s_prop_search search;
		long a;
		bool b;
		s_reference reference;

		memset(&search, 0, sizeof(search));
		search.type = 6;
		search.unknown14 = true;
		search.unknown59 = true;
		reference = function_261280(&search, actor_index, NULL, &a, buffer, &b);
		function_2626b0(actor_index, reference, a, buffer, b, true);
		ai_scratch_buffer_release(buffer);
	}
	return true;
}
// @retail 0x1a94b0
void __stdcall function_1a94b0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE)
	{
		short *view = (short *)function_25d700(actor->prop_index);
		if (view && *view >= 6)
		{
			actor->unknown488 = true;
			actor->unknown41c = 4;
			actor->unknown420 = 2;
			return;
		}
	}
	if (actor->unknown358 > 0)
	{
		actor->unknown41c = 2;
		actor->unknown420 = 5;
	}
	else
	{
		actor->unknown41c = 2;
		actor->unknown420 = 2;
	}
}

// @retail 0x1a9540
short __stdcall function_1a9540(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (actor->unknown270 == 3 &&
		actor->unknown358 != 0 &&
		prop_node_get(actor->unknown368)->unknown24 >= 1 &&
		!actor->unknown35e)
	{
		real range = actor->unknown39c + 3.0f;
		if (!(actor->unknown398 > range * range) && !actor->unknown225)
		{
			bool near_point;
			bool near_actor;
			bool near_segment;

			result = 1;
			function_1a8fa0(actor_index, 0.5f, &near_point, &near_actor, &near_segment);
			if (near_point || near_segment && !near_actor)
			{
				if (!REFERENCE_EQUAL(actor->unknown418, g_470fa0))
					function_262800(actor_index, actor->unknown418, false);
			}
			if (near_actor || near_segment)
				result = 3;
		}
	}
	return result;
}

// @retail 0x1a9660
bool __stdcall function_1a9660(long actor_index, s_slot *slot)
{
	s_slot_58 *state = (s_slot_58 *)slot;

	state->ticks = 0;
	long unit_index = actor_get(actor_index)->unknown018;
	if (unit_index != NONE)
		function_20ba60(0x26, unit_index, NONE, NONE, NONE, NULL);
	return true;
}

// @retail 0x1a96b0
short __stdcall function_1a96b0(long actor_index, s_slot *slot, bool active)
{
	s_slot_58 *state = (s_slot_58 *)slot;
	short result = g_46fbe8;

	if (actor_get(actor_index)->unknown358 == 0)
	{
		result = g_46fbe4;
	}
	else
	{
		bool near_actor;
		bool near_segment;
		function_1a8fa0(actor_index, 0.5f, NULL, &near_actor, &near_segment);
		if (!near_actor && !near_segment)
		{
			if ((real)state->ticks * g_510c54->rate > 1.0f)
				result = 0x54;
			else
				state->ticks++;
		}
		else
		{
			state->ticks = 0;
		}
	}
	return result;
}

/* ---- the handlers ---- */

s_slot_child g_46f390[1] =
{
	{2, 0, NONE, {0}, 0.0f, 0, 0},
};

s_slot_handler_1 g_47d980 =
{
	{
		0x46, 1, 0, -2, 0,
		0, function_1b2d90, slot_start_true, slot_proc_nothing, 1, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	function_1a83b0, 1, g_46f390
};

s_slot_handler_0 g_47d9cc =
{
	0x2f, 0, 0, -2, 0, function_1a8c30
};

s_slot_handler_2 g_47d9e0 =
{
	{
		0x2d, 2, 0, -2, 0,
		function_1a92b0, function_1a93c0, 0, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1a9400, 0, function_1a94b0
};

s_slot_handler_2 g_47da30 =
{
	{
		0x58, 2, 0, -2, 0,
		function_1a9540, function_1a96b0, function_1a9660, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)slot_start_true, 0, function_1a9760
};