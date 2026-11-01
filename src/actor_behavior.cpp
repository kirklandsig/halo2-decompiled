// @flags /O2 /arch:SSE /Gr
/* ACTOR_BEHAVIOR.CPP: the actor behavior stack and the callbacks of several
   behavior types (the SSE half; actor_behavior_x87.cpp has the rest).

   An actor runs up to four behavior slots, one per level (s_slot,
   slot_owner.h). g_46eeb8 holds one handler per behavior type
   (slot_handler.h); the stack functions here find where a behavior goes,
   replace the levels from there down, and run the current behavior's
   updates through its handler. */

#include "unknown_11c920.h"
#include "unknown_26b230.h"
#include "globals.h"
#include "slot_owner.h"
#include "slot_handler.h"
#include "unknown_2626b0.h"
#include "data_array.h"
#include "unknown_1fb7e0.h"
#include "unknown_1f4460.h"
#include <string.h>
#include "unknown_2605d0.h"
#include "unknown_0259a0.h"
#include "actor_behavior.h"

/* the handlers by behavior type (slot_handler.h) */
s_slot_handler *g_46eeb8[k_slot_type_count];

#define ACTOR_BEHAVIOR_OWNER(actor_index) ((s_slot_owner_entry *)(g_4f55f0->data + ((actor_index) & 0xffff) * sizeof(s_slot_owner_entry)))

/* the level a behavior of the given type goes to below the given level: one
   below the nearest level that holds the type's parent (or type 4), or
   NONE when that is past the last level */
/* (the plain loop with a break does not match: round 39) */
// @retail 0x1a8080
short actor_behavior_get_insert_level(short type, s_slot_owner_entry *owner, short level)
{
	short result = -1;
	short parent_type = g_46eeb8[type]->wanted_type;

	if (parent_type != -1)
	{
		while (level >= 0)
		{
			short level_type = owner->slots[level].type;
			if (level_type == parent_type || level_type == 4)
				goto found;
			level--;
		}
		level = 0;
		goto done;
	found:
		if (level == -1)
			level = 0;
	}
done:
	if (level < 3)
		result = level + 1;
	return result;
}

/* starts a behavior of the given type at the given level: stops the
   behaviors at that level and below, then fills the level from the given
   state (or just the type) */
// @retail 0x1a80e0
bool function_1a80e0(long actor_index, short type, s_slot *state, short level)
{
	s_slot_owner_entry *owner = ACTOR_BEHAVIOR_OWNER(actor_index);
	s_slot_handler *handler = g_46eeb8[type];
	bool started;

	if (state)
		state->type = type;
	if (handler->start)
		started = handler->start(actor_index, state);
	else
		started = true;
	if (started)
	{
		short i;
		for (i = owner->current; i >= level; i--)
		{
			s_slot *slot = &owner->slots[i];
			t_slot_proc stop = g_46eeb8[slot->type]->stop;
			if (stop)
				stop(actor_index, i < 4 ? slot : 0);
			slot->type = -1;
			slot->state = 1;
		}
		owner->current = level;
		if (level < 4)
		{
			s_slot *slot = &owner->slots[level];
			if (slot)
			{
				if (state)
				{
					if (handler->kind == 1)
						state->unknown4 = -1;
					*slot = *state;
				}
				else
				{
					slot->type = type;
				}
				slot->state = 0;
				slot->time = g_510c54->game_time;
			}
		}
	}
	return started;
}

/* records a request in the first free of the actor's three entries, or in
   place of the one with the lowest priority below the new one */
// @retail 0x1a8220
bool function_1a8220(long actor_index, short type, short field8, long priority, short field4, short field6, short field2)
{
	short chosen = -1;
	bool result = false;
	s_slot_owner_entry *owner = ACTOR_BEHAVIOR_OWNER(actor_index);
	short lowest_priority = (short)priority;
	short i = 0;

	do
	{
		s_slot_entry *entry = &owner->entries[i];
		if (entry->type == -1)
		{
			chosen = i;
			break;
		}
		if (entry->priority < lowest_priority)
		{
			lowest_priority = entry->priority;
			chosen = i;
		}
		i++;
	}
	while (i < 3);

	if (chosen != -1)
	{
		s_slot_entry *entry = &owner->entries[chosen];
		entry->type = type;
		entry->field8 = field8;
		entry->field4 = field4;
		entry->priority = (short)priority;
		entry->field6 = field6;
		entry->field2 = field2;
		result = true;
	}
	return result;
}

/* runs the current behavior's update44, if it has one */
// @retail 0x1a82d0
void actor_behavior_current_update44(long actor_index)
{
	s_slot_owner_entry *owner = ACTOR_BEHAVIOR_OWNER(actor_index);
	s_slot *slot = &owner->slots[owner->current];
	s_slot_handler_2 *handler = (s_slot_handler_2 *)g_46eeb8[slot->type];

	if (handler->head.kind == 2 && handler->update44)
		handler->update44(actor_index, slot);
}

/* runs the current behavior's update48, if it has one */
// @retail 0x1a8320
void actor_behavior_current_update48(long actor_index)
{
	s_slot_owner_entry *owner = ACTOR_BEHAVIOR_OWNER(actor_index);
	s_slot *slot = &owner->slots[owner->current];
	s_slot_handler_2 *handler = (s_slot_handler_2 *)g_46eeb8[slot->type];

	if (handler->head.kind == 2 && handler->update48)
		handler->update48(actor_index, slot);
}

/* the priority of behaviors that may always run */
// @retail 0x1a8370
short __stdcall actor_behavior_priority_minimum(long)
{
	return 1;
}

/* the default behavior's start */
// @retail 0x1a8380
bool __stdcall actor_behavior_default_start(long actor_index, s_slot *)
{
	ACTOR_BEHAVIOR_OWNER(actor_index)->unknown84 = 4;
	return true;
}

/* the default behavior (group 0x68) and its children */
s_slot_child g_actor_behavior_default_children[3] =
{
	{0x5f, 0, NONE, {0}, 0, 0, 0},
	{0x60, 1, NONE, {0}, 0, 0, 0},
	{5, 0, 0, {0}, 0, 0, 0},
};

s_slot_handler_1 g_actor_behavior_default_handler =
{
	{
		0x68, 1, 0, -2, 0,
		actor_behavior_priority_minimum, function_1bced0, actor_behavior_default_start, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	function_1a79e0, 3, g_actor_behavior_default_children
};

bool __stdcall function_1f4280(long actor_index);
bool __stdcall function_1f46f0(long actor_index, short type, s_reference reference, byte *scratch, bool unknown);
void __stdcall actor_behavior_3a_update48(long actor_index, s_slot *slot);

/* behavior 0x3a's update48 (its handler is in unknown_1b0020.cpp): the
   actor moves to the slot's reference until it has one it can follow, or
   gives it up when its unit has fought too long or the player's team is
   its enemy */
// @retail 0x1b0110
void __stdcall actor_behavior_3a_update48(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_actor_behavior_3a_data *state = (s_actor_behavior_3a_data *)slot;

	actor->unknown41c = 4;
	actor->unknown420 = 0;
	actor->unknown489[1] = true;
	actor->unknown44b[0] = true;
	actor->unknown449 = true;
	actor->unknown446[2] = true;
	actor->unknown481 = true;
	s_reference reference = state->reference;

	if (REFERENCE_EQUAL(reference, g_470fa0))
	{
		function_1f4280(actor_index);
	}
	else if (actor->unknown040)
	{
		if (function_1f46f0(actor_index, 4, reference, NULL, false))
		{
			function_2628f0(actor_index, state->reference);
			actor->unknown3f0 = state->reference_flag;
		}
		else if (actor->unknown5b4 > 4 || team_is_enemy(actor->unknown024, 1))
		{
			if (!REFERENCE_EQUAL(actor->unknown418, g_470fa0))
			{
				function_262800(actor_index, actor->unknown418, false);
				function_1f4280(actor_index);
				function_2628f0(actor_index, g_470fa0);
			}
			state->reference = g_470fa0;
			state->gave_up = true;
		}
	}
}

/* the squared distance from b to a */
static inline real actor_behavior_distance_squared3d(point3f const *a, point3f const *b)
{
	real dx = a->x - b->x;
	real dy = a->y - b->y;
	real dz = a->z - b->z;

	return dz * dz + dy * dy + dx * dx;
}

void __stdcall actor_behavior_64_update48(long actor_index, s_slot *slot);

/* behavior 0x64's update48 (its handler is in unknown_1b3590.cpp): the
   actor walks to the slot's point and, once there, faces its prop */
// @retail 0x1b3c60
void __stdcall actor_behavior_64_update48(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_actor_behavior_64_data *state = (s_actor_behavior_64_data *)slot;
	s_type_c3b527 *destination = &state->destination;
	real distance_squared;

	if (destination->output_index == NONE)
	{
		vector3f offset;

		offset.i = actor->position.x - destination->point.x;
		offset.j = actor->position.y - destination->point.y;
		offset.k = actor->position.z - destination->point.z;

		real sum = offset.i * offset.i;
		sum += offset.k * offset.k;
		sum += offset.j * offset.j;
		distance_squared = sum;
	}
	else
	{
		point3f point;

		function_210850(destination, &point);
		distance_squared = actor_behavior_distance_squared3d(&actor->position, &point);
	}
	if (state->radius * state->radius > distance_squared)
		function_1f4280(actor_index);
	if (state->prop_index != NONE)
	{
		if (distance_squared < 4.0f)
		{
			if (!state->vocalized)
			{
				function_1fb7e0(0x37, actor_index, NULL, prop_node_get(state->prop_index)->object_index, NONE);
				state->vocalized = true;
			}
			state->arrived = true;
			actor->unknown41c = 3;
			actor->unknown420 = 1;
			actor->unknown424.object_index = state->prop_index;
			actor->unknown430 = 2;
			actor->unknown434 = 1;
			actor->unknown438.object_index = state->prop_index;
			actor->unknown444 = NONE;
			actor->unknown449 = true;
			actor->unknown44a = false;
			actor->unknown488 = false;
		}
		else
		{
			actor->unknown41c = 2;
			actor->unknown420 = 2;
			actor->unknown488 = false;
		}
	}
}

void function_1f86a0(long index);
void *function_1e5030(long actor_index);

bool __stdcall actor_behavior_6c_start(long actor_index, s_slot *slot);
bool __stdcall actor_behavior_6c_update40(long actor_index, s_slot *slot);
bool __stdcall actor_behavior_wait_start(long actor_index, s_slot *slot);
short __stdcall actor_behavior_wait_choose(long actor_index, short level, bool active);

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
/* behavior 0x6c's start: the unit gets request 0x2c, the point counter and
   the actor's movement state start over */
/* (without the barrier the stores are reordered: round 39) */
// @retail 0x1bcee0
bool __stdcall actor_behavior_6c_start(long actor_index, s_slot *slot)
{
	s_unit_request request;

	memset(&request, 0, sizeof(request));
	long unit_index = actor_get(actor_index)->unknown018;
	request.type = 0x2c;
	function_e6900(unit_index, &request);
	((s_counter_behavior_state *)slot)->counter = 0;
	s_actor_view *actor = actor_get(actor_index);
	actor->unknown50c = false;
	actor->unknown5ac = NONE;
	actor->unknown5b0 = NONE;
	actor->unknown5b4 = 0;
	actor->unknown5b6 = 0;
	_ReadWriteBarrier();
	actor->unknown4ac = 0;
	actor->unknown504 = 0;
	return true;
}

#pragma function(_ReadWriteBarrier)

/* behavior 0x6c's evaluate: when the actor reaches a point it stops and
   heads for the next; it never finishes by itself */
// @retail 0x1bcf90
short __stdcall actor_behavior_6c_evaluate(long actor_index, s_slot *slot, bool active)
{
	if (actor_get(actor_index)->unknown504 == 2)
	{
		s_counter_behavior_state *state = (s_counter_behavior_state *)slot;

		function_1f86a0(actor_index);
		state->counter++;
	}
	return g_46fbe8;
}

/* behavior 0x6c's update48: the actor looks along g_4687a8 */
// @retail 0x1bd1e0
void __stdcall actor_behavior_6c_update48(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	actor->unknown41c = 4;
	actor->unknown420 = 4;
	actor->unknown424.vector = *g_4687a8;
}

/* behavior group 7's evaluate: it finishes when its ticks run out */
// @retail 0x1bd330
short __stdcall actor_behavior_wait_evaluate(long actor_index, s_slot *slot, bool active)
{
	s_counter_behavior_state *state = (s_counter_behavior_state *)slot;
	short result = g_46fbe8;

	if (--state->counter <= 0)
		result = g_46fbe4;
	return result;
}

/* behavior group 7's choice: the default choice, and the actor stops (and
   faces its prop if it has one) */
// @retail 0x1bd350
short __stdcall actor_behavior_wait_choose(long actor_index, short level, bool active)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = function_1a79e0(actor_index, level, active);

	function_1f4280(actor_index);
	if (actor->prop_index != NONE)
	{
		actor->unknown41c = 4;
		actor->unknown420 = 2;
	}
	return result;
}

s_slot_handler_2 g_actor_behavior_6c_handler =
{
	{
		0x6c, 2, NONE, -2, 0,
		function_1adcd0, actor_behavior_6c_evaluate, actor_behavior_6c_start, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)actor_behavior_6c_update40, 0, actor_behavior_6c_update48
};

/* the children of behavior group 7 */
s_slot_child g_actor_behavior_wait_children[1] =
{
	{2, 0, NONE, {0}, 0.0f, 0, 0},
};

s_slot_handler_1 g_actor_behavior_wait_handler =
{
	{
		7, 1, 0, -2, 0,
		function_1adcd0, actor_behavior_wait_evaluate, actor_behavior_wait_start, slot_proc_nothing, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	actor_behavior_wait_choose, 1, g_actor_behavior_wait_children
};

/* behavior group 7's start: unless the actor is at +0x328 level 12 or more,
   it waits a random time from its character's range (half of it from level
   10), and starts only when that is at least a tick */
// @retail 0x1bd230
bool __stdcall actor_behavior_wait_start(long actor_index, s_slot *slot)
{
	bool result = false;
	s_actor_view *actor = actor_get(actor_index);
	s_counter_behavior_state *state = (s_counter_behavior_state *)slot;

	if (actor->unknown328 < 12)
	{
		s_character_wait_range *range = (s_character_wait_range *)function_1e5030(actor_index);

		if (range && range->upper > 0.0f)
		{
			real seconds = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, range->lower, range->upper) * g_510c54->field_2_3;
			long ticks;

			__asm
			{
				fld seconds
				fistp ticks
			}

			state->counter = (short)ticks;
			if (state->counter > 0)
			{
				if (actor->unknown328 >= 10)
					state->counter /= 2;
				result = true;
			}
		}
	}

	return result;
}


void __stdcall function_2628f0(long actor_index, s_reference reference);
bool __stdcall function_1f8a70(long actor_index, long unknown);

/* clears the actor's movement command */
PRIVATE __forceinline void actor_move_command_clear(s_actor_move_command *command)
{
	command->mode = 0;
	command->mode_changed = false;
	command->value04 = 0.0f;
	command->value08 = 0.0f;
	command->value20 = 0.0f;
	command->value24 = 0.0f;
	command->flag28 = false;
	command->flag29 = false;
	command->index38 = NONE;
	command->flag3c = false;
}

/* orders the actor to move to the point, unless it already moves to within
   0.1 of it */
PRIVATE __forceinline void actor_walk_toward_point(long actor_index, s_type_c3b527 const *destination)
{
	s_actor_view *actor = actor_get(actor_index);
	if (!actor->unknown229)
		function_1f86a0(actor_index);
	else
	{
		function_2628f0(actor_index, g_470fa0);
		if (actor->unknown4ac == 2 && actor->unknown4c8 == NONE &&
			!(function_210a30(destination, &actor->unknown4b8) > 0.1f * 0.1f))
		{
			if (actor->unknown040 && !actor->unknown506)
				function_1f8a70(actor_index, 0);
		}
		else
		{
			function_1f86a0(actor_index);
			actor_move_command_clear((s_actor_move_command *)&actor->unknown4ac);
			actor->unknown4b8 = *destination;
			actor->unknown4ac = 2;
			actor->unknown4ae = true;
			actor->unknown4c8 = NONE;
			actor->unknown4e4 = NONE;
			if (actor->unknown229)
				actor->unknown656 = 0;
			function_1f8a70(actor_index, 0);
		}
	}
}

/* behavior 0x6c's update40: unless the actor is moving, it heads for the
   point the counter picks among those function_261d20 finds */
// @retail 0x1bcfd0
bool __stdcall actor_behavior_6c_update40(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	if (!actor->unknown50c || actor->unknown504 != 1)
	{
		s_2605d0_request request;
		s_261d20_entry points[512];
		memset(&request, 0, sizeof(request));
		request.unknown056 = true;
		short point_count = function_261d20(actor_index, points, 512, &request);
		if (point_count > 0)
		{
			s_counter_behavior_state *state = (s_counter_behavior_state *)slot;
			if (state->counter >= point_count)
				state->counter = 0;
			point3f position;
			s_type_c3b527 destination;
			function_210850(*(s_type_c3b527 const **)points[state->counter].unknown00, &position);
			destination.point = position;
			destination.output_index = NONE;
			actor_walk_toward_point(actor_index, &destination);
		}
	}
	return true;
}

/* behavior 0x50's update48 (its handler is in unknown_1bf990.cpp): the
   actor looks at the slot's point, when it has one */
// @retail 0x1c0230
void __stdcall actor_behavior_50_update48(long actor_index, s_slot *slot)
{
	s_actor_behavior_50_data *state = (s_actor_behavior_50_data *)slot;

	if (state->has_look_point)
	{
		s_actor_view *actor = actor_get(actor_index);

		actor->unknown41c = 4;
		actor->unknown420 = 4;
		actor->unknown424.point = state->look_point;
	}
}

short actor_behavior_find_level(long actor_index, short type);
void function_1f86a0(long index);

/* sets the value of the actor's behavior 0x82, if it runs one, and clears
   the actor's flag at +0x22a */
// @retail 0x1c1540
void function_1c1540(long actor_index, long value)
{
	s_actor_view *actor = actor_get(actor_index);
	long level = actor_behavior_find_level(actor_index, 0x82);

	if (level != NONE)
	{
		((s_actor_behavior_82_data *)&actor->slots[level])->value = value;
	}
	actor->unknown22a = false;
}

/* behavior 0x15's priority: 3 while the actor's flag at +0x223 is set */
// @retail 0x1c1590
short __stdcall actor_behavior_15_priority(long actor_index)
{
	short result = 0;

	if (actor_get(actor_index)->unknown223)
	{
		result = 3;
	}
	return result;
}

/* behavior 0x15's start: the actor's movement state starts over and the
   behavior takes the actor's prop */
/* (the plain stores do not match: round 39) */
// @retail 0x1c15c0
bool __stdcall actor_behavior_15_start(long actor_index, s_slot *slot)
{
	s_actor_behavior_15_data *state = (s_actor_behavior_15_data *)slot;
	s_actor_view *actor = actor_get(actor_index);

	*(bool volatile *)&actor->unknown50c = false;
	*(long volatile *)&actor->unknown5ac = NONE;
	actor->unknown5b0 = NONE;
	actor->unknown5b4 = 0;
	actor->unknown5b6 = 0;
	actor->unknown4ac = 0;
	actor->unknown504 = 0;
	state->repositioned = false;
	state->prop_index = actor->prop_index;
	actor->unknown354 = true;
	function_265cb0(actor_index);
	return true;
}

/* behavior 0x15's stop */
// @retail 0x1c1640
void __stdcall actor_behavior_15_stop(long actor_index, s_slot *slot)
{
	actor_get(actor_index)->unknown354 = false;
}

/* behavior 0x15's evaluate: it finishes when the actor's flag at +0x223
   clears; it starts over when the actor's prop changes, and stops the
   actor when the prop comes within 2.5 (or, after a search, goes past
   3.75) */
// @retail 0x1c1670
short __stdcall actor_behavior_15_evaluate(long actor_index, s_slot *slot, bool active)
{
	s_actor_behavior_15_data *state = (s_actor_behavior_15_data *)slot;
	short result = g_46fbe8;
	s_actor_view *actor = actor_get(actor_index);
	s_prop_node_view *prop;

	if (!actor->unknown223)
	{
		result = g_46fbe4;
	}
	else
	{
	prop = prop_node_get(actor->prop_index);
	if (actor->prop_index != state->prop_index)
	{
		state->prop_index = actor->prop_index;
		state->repositioned = false;
		state->settled = false;
		function_1f86a0(actor_index);
	}
	if (state->repositioned)
	{
		if (prop->unknown28 > 3.75f)
		{
			function_1f86a0(actor_index);
			state->repositioned = false;
			state->settled = false;
		}
	}
	else if (2.5f > prop->unknown28)
	{
		function_1f86a0(actor_index);
	}
	}
	return result;
}

/* steps a prop index along its list */
static inline s_prop_node_view *actor_behavior_prop_next(long *prop_index)
{
	s_prop_node_view *prop = 0;

	if (*prop_index != NONE)
	{
		prop = prop_node_get(*prop_index);
		*prop_index = prop->next_index;
	}
	return prop;
}

/* behavior 0x15's update40: past 5 from its prop the actor moves toward
   it; once close (or after a search) it searches for a position kept 3.75
   from the positions of all its props of type 1, and moves there */
// @retail 0x1c1730
bool __stdcall actor_behavior_15_update40(long actor_index, s_slot *slot)
{
	s_actor_behavior_15_data *state = (s_actor_behavior_15_data *)slot;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown040)
	{
		s_prop_node_view *prop = prop_node_get(actor->prop_index);

		if (prop->unknown28 > 5.0f)
		{
			if (function_1f4810(actor_index, actor->prop_index, 0.0f, 0))
			{
				actor->unknown4cc = 2.5f;
			}
		}
		else if (state->repositioned || 1.75f > prop->unknown28)
		{
			byte *buffer = ai_scratch_buffer_get();
			s_prop_search search;
			long prop_index;
			long other_actor_index;
			bool flag;
			s_261d20_entry entry;
			s_reference reference;

			memset(&search, 0, sizeof(search));
			search.type = 0;
			s_prop_node_view *other;

			prop_index = actor_get(actor_index)->first_prop_index;
			while ((other = actor_behavior_prop_next(&prop_index)) != 0 && search.point_count < 32)
			{
				if (other->type == 1)
				{
					s_prop_position_state *other_state = (s_prop_position_state *)prop_node_state(other);

					search.points[search.point_count].position = other_state->position;
					search.points[search.point_count].weight = 3.75f;
					search.point_count++;
				}
			}
			reference = function_261280(&search, actor_index, &entry, &other_actor_index, buffer, &flag);
			if (!REFERENCE_EQUAL(reference, g_470fa0))
			{
				function_2626b0(actor_index, reference, other_actor_index, buffer, flag, true);
			}
			state->repositioned = true;
			ai_scratch_buffer_release(buffer);
		}
	}
	return true;
}

/* behavior 0x15's update48: the actor faces its prop, and sets its flags
   at +0x449 and +0x483 (and +0x44a within 2.5) until it has settled */
// @retail 0x1c18f0
void __stdcall actor_behavior_15_update48(long actor_index, s_slot *slot)
{
	s_actor_behavior_15_data *state = (s_actor_behavior_15_data *)slot;
	s_actor_view *actor = actor_get(actor_index);
	s_prop_node_view *prop = prop_node_get(actor->prop_index);

	actor->unknown41c = 4;
	actor->unknown420 = 2;
	if (!state->settled && (!state->repositioned || !(1.25f > prop->unknown28)))
	{
		if (2.5f > prop->unknown28)
		{
			actor->unknown44a = true;
		}
		actor->unknown483 = true;
		actor->unknown449 = true;
	}
	else
	{
		state->settled = true;
		actor->unknown449 = true;
	}
}

/* the release callback (release28) of behaviors 0x15, 0x50, 0x64 and
   others: a prop is going away, and the slot forgets it if it is its own */
// @retail 0x1c1990
void __stdcall actor_behavior_release_prop(long actor_index, s_slot *slot, long prop_index)
{
	s_actor_behavior_15_data *state = (s_actor_behavior_15_data *)slot;

	if (state->prop_index == prop_index)
	{
		state->prop_index = NONE;
	}
}

s_slot_handler_2 g_actor_behavior_15_handler =
{
	{
		0x15, 2, 0, -2, 0,
		actor_behavior_15_priority, actor_behavior_15_evaluate, actor_behavior_15_start, actor_behavior_15_stop, NONE, {0},
		0, actor_behavior_release_prop, 0, 0, 0, 0, 0
	},
	(t_slot_proc)actor_behavior_15_update40, slot_proc_nothing, actor_behavior_15_update48
};
