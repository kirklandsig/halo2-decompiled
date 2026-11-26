// @flags /O2 /Gr
/* SWARM.CPP: the ai's swarms: chains of creature objects that one actor
   drives (include/swarm.h). Creating and deleting a swarm, attaching its
   actor, walking its creatures and the five swarm spawn slots. */

#include "unknown_11c920.h"
#include "unknown_2551c0.h"
#include "globals.h"
#include "object_iterator.h"
#include "unknown_123b30.h"
#include "props.h"
#include "swarm.h"
#include <string.h>

typedef char assert_swarm_prop_size[(sizeof(s_type_76cf92) == 0xc4) ? 1 : -1];

/* starts an iterator over the swarms (while the ai is active) */
// @retail 0x290c50
void swarm_iterator_new(s_290c50_iterator *iterator)
{
	if (g_4f55d0->active)
	{
		iterator->records.data = g_5044c8;
		iterator->records.index = NONE;
		iterator->records.datum_index = NONE;
		iterator->object_index = NONE;
	}
}

/* clears a creature's swarm data */
// @retail 0x28da50
void swarm_creature_initialize(s_object_ai_data *creature)
{
	creature->flags = 0;
	creature->actor_index = NONE;
	creature->prop_index = NONE;
	creature->next_creature_index = NONE;
	creature->index20 = NONE;
	creature->swarm_handle = NONE;
	point3f &position = creature->position;
	position = *g_468788;
	creature->goal_prop_ref = NONE;
	creature->flag56 = false;
	creature->count66 = 0;
	creature->index64 = NONE;
	creature->position74 = *g_468788;
}

point3f *function_b9dd0(long object_index, point3f *result);

/* takes a creature's position from its object, and whether it has a parent */
// @retail 0x28f890
void swarm_creature_reset_position(long object_index, s_object_ai_data *creature)
{
	s_swarm_object *object = (s_swarm_object *)handler_object_get(object_index);
	function_b9dd0(object_index, &creature->position);
	creature->index20 = NONE;
	if (object->parent_index != NONE)
		creature->attached = true;
	else
		creature->attached = false;
}

inline s_object_ai_data *object_ai_data(s_handler_object_view *object)
{
	return object->flags134 ? NULL : (s_object_ai_data *)((byte *)object + object->ai_offset);
}

__forceinline s_object_ai_data *object_ai_data_checked(s_handler_object_view *object)
{
	s_object_ai_data *result;
	if (!object->flags134)
		result = (s_object_ai_data *)((byte *)object + object->ai_offset);
	else
		result = NULL;
	return result;
}

/* swarm_creature_iterator_next, inlined */
__forceinline s_handler_object_view *swarm_creature_iterator_step(s_ai_object_iterator *iterator)
{
	s_handler_object_view *object = NULL;
	if (iterator->next_index != NONE)
	{
		object = handler_object_get(iterator->next_index);
		iterator->index = iterator->next_index;
		s_object_ai_data *creature;
		if (!object->flags134 && (creature = (s_object_ai_data *)((byte *)object + object->ai_offset)) != NULL)
			iterator->next_index = creature->next_creature_index;
		else
			iterator->next_index = NONE;
	}
	return object;
}

/* clears index20 of each creature of a swarm */
// @retail 0x28fdf0
void swarm_creatures_clear_index20(long swarm_handle)
{
	long object_index = swarm_get(swarm_handle)->first_creature_index;
	while (object_index != NONE)
	{
		s_object_ai_data *creature = object_ai_data(handler_object_get(object_index));
		if (!creature)
			break;
		object_index = creature->next_creature_index;
		creature->index20 = NONE;
	}
}

/* gives each creature of a swarm the team */
// @retail 0x290bf0
void swarm_set_team(long swarm_handle, short team)
{
	long object_index = swarm_get(swarm_handle)->first_creature_index;
	while (object_index != NONE)
	{
		s_handler_object_view *object = handler_object_get(object_index);
		s_object_ai_data *creature = object_ai_data(object);
		if (!creature)
			break;
		((s_swarm_object *)object)->team = team;
		object_index = creature->next_creature_index;
	}
}

/* makes each creature of a swarm forget a prop_ref */
// @retail 0x28fd90
void swarm_forget_prop(long swarm_handle, long prop_index)
{
	long object_index = swarm_get(swarm_handle)->first_creature_index;
	while (object_index != NONE)
	{
		s_object_ai_data *creature = object_ai_data(handler_object_get(object_index));
		if (!creature)
			break;
		if (creature->prop_index == prop_index)
			creature->prop_index = NONE;
		if (creature->goal_prop_ref == prop_index)
			creature->goal_prop_ref = NONE;
		object_index = creature->next_creature_index;
	}
}

void function_b7360(long object_index);

/* sets flag 6 (or flag 5) at +0x10a of each creature object of a swarm */
// @retail 0x28e2b0
void swarm_creatures_set_object_flag(long swarm_handle, bool flag6)
{
	long object_index = swarm_get(swarm_handle)->first_creature_index;
	while (object_index != NONE)
	{
		s_handler_object_view *object = handler_object_get(object_index);
		s_object_ai_data *creature = object_ai_data(object);
		if (!creature)
			break;
		if (flag6)
			((s_swarm_object *)object)->flag6 = true;
		else
			((s_swarm_object *)object)->flag5 = true;
		function_b7360(object_index);
		object_index = creature->next_creature_index;
	}
}

/* hands out a creature's pending launch velocity once */
// @retail 0x290190
bool swarm_creature_get_pending_velocity(long object_index, point3f *velocity)
{
	bool result = false;
	s_handler_object_view *object = handler_object_get(object_index);
	s_object_ai_data *creature = object_ai_data_checked(object);
	if (creature && creature->velocity_pending)
	{
		*velocity = creature->pending_velocity;
		creature->velocity_pending = false;
		result = true;
	}
	return result;
}

s_swarm_spawn_slot *g_5044c4;

void function_26ae30(long object_index);
void __stdcall function_b8540(long object_index);
void __stdcall function_1e1a00(long actor_index, bool value);

/* deletes a swarm: takes its creatures out of the world and lets go of its
   actor and of the spawn slots that name it */
// @retail 0x28e200
void __stdcall swarm_delete(long swarm_handle)
{
	s_swarm *swarm = swarm_get(swarm_handle);
	if (swarm->creature_count > 0)
	{
		while (swarm->first_creature_index != NONE)
			swarm_remove_creature(swarm_handle, swarm->first_creature_index, true, false);
	}
	if (swarm->actor_index != NONE)
	{
		s_swarm_leader *actor = (s_swarm_leader *)actor_get(swarm->actor_index);
		if (actor->swarm_handle == swarm_handle)
			actor->swarm_handle = NONE;
	}
	if (swarm->spawn_slot_reference)
	{
		long slot_index = 0;
		do
		{
			if (g_5044c4[slot_index].swarm_handle == swarm_handle)
				g_5044c4[slot_index].swarm_handle = NONE;
			slot_index++;
		} while (slot_index < k_maximum_swarm_spawn_slots);
	}
	record_pool_release(g_5044c8, swarm_handle);
}

/* takes a creature out of a swarm's chain; the swarm is deleted when its last
   creature goes and delete_when_empty is set */
// @retail 0x28e390
bool __stdcall swarm_remove_creature(long swarm_handle, long object_index, bool remove_from_world, bool delete_when_empty)
{
	bool removed = false;
	s_swarm *swarm = swarm_get(swarm_handle);
	long *link = &swarm->first_creature_index;
	while (*link != NONE)
	{
		s_object_ai_data *creature = object_ai_data_checked(handler_object_get(*link));
		if (!creature)
			break;
		if (*link == object_index)
		{
			*link = creature->next_creature_index;
			((s_swarm_object *)handler_object_get(object_index))->orientation_pending = false;
			removed = true;
			creature = object_ai_data_checked(handler_object_get(object_index));
			if (remove_from_world)
			{
				function_26ae30(object_index);
				function_b8540(object_index);
			}
			if (creature)
			{
				creature->swarm_handle = NONE;
				creature->actor_index = NONE;
				creature->next_creature_index = NONE;
			}
			swarm->creature_count--;
			if (swarm->creature_count == 0 && delete_when_empty)
			{
				if (swarm->actor_index != NONE)
					function_1e1a00(swarm->actor_index, 0);
				swarm_delete(swarm_handle);
			}
			break;
		}
		link = &creature->next_creature_index;
	}
	return removed;
}

/* creates the swarm data array and the spawn slots */
// @retail 0x28d930
void function_28d930(void)
{
	g_5044c8 = data_new_inlined("swarm", 32, sizeof(s_swarm), 0, g_510c2c);
	g_5044c4 = (s_swarm_spawn_slot *)function_123d40(NULL, NULL, k_maximum_swarm_spawn_slots * sizeof(s_swarm_spawn_slot));
	for (long slot_index = 0; slot_index < k_maximum_swarm_spawn_slots; slot_index++)
	{
		g_5044c4[slot_index].count = 0;
		g_5044c4[slot_index].object_index = NONE;
	}
}

/* empties the spawn slots of an object that goes away */
// @retail 0x290b90
void swarm_spawn_slots_clear_object(long object_index)
{
	for (long slot_index = 0; slot_index < k_maximum_swarm_spawn_slots; slot_index++)
	{
		if (g_5044c4[slot_index].object_index == object_index)
		{
			if (g_5044c4[slot_index].swarm_handle != NONE)
				swarm_get(g_5044c4[slot_index].swarm_handle)->spawn_slot_reference = false;
			g_5044c4[slot_index].object_index = NONE;
			g_5044c4[slot_index].swarm_handle = NONE;
			g_5044c4[slot_index].count = 0;
		}
	}
}

/* detaches an actor from its swarm, and the swarm's creatures from the actor */
// @retail 0x28e160
void swarm_leader_detach(long actor_index)
{
	s_swarm_leader *actor = (s_swarm_leader *)actor_get(actor_index);
	if (actor->swarm_handle != NONE)
	{
		s_swarm *swarm = swarm_get(actor->swarm_handle);
		long swarm_handle = actor->swarm_handle;
		actor->swarm_handle = NONE;
		swarm->actor_index = NONE;
		s_ai_object_iterator iterator;
		iterator.next_index = swarm_get(swarm_handle)->first_creature_index;

		s_handler_object_view *object;
		while ((object = swarm_creature_iterator_step(&iterator)) != NULL)
		{
			s_object_ai_data *creature = object_ai_data_checked(object);
			if (creature)
				creature->actor_index = NONE;
		}
	}
}

/* makes an actor the one that drives a swarm, detaching both from any other */
// @retail 0x28e090
void __stdcall swarm_leader_attach(long actor_index, long swarm_handle)
{
	s_swarm_leader *actor = (s_swarm_leader *)actor_get(actor_index);
	s_swarm *swarm = swarm_get(swarm_handle);
	if (actor->swarm_handle != swarm_handle)
	{
		if (actor->swarm_handle != NONE)
			swarm_leader_detach(actor_index);
		long old_actor_index = swarm->actor_index;
		if (old_actor_index != NONE)
			swarm_leader_detach(old_actor_index);
		actor->swarm_handle = swarm_handle;
		swarm->actor_index = actor_index;
		s_ai_object_iterator iterator;
		iterator.next_index = swarm_get(swarm_handle)->first_creature_index;
		s_handler_object_view *object;
		while ((object = swarm_creature_iterator_step(&iterator)) != NULL)
		{
			s_object_ai_data *creature = object_ai_data_checked(object);
			if (creature)
				creature->actor_index = actor_index;
		}
	}
}

/* counts a creature's countdown down while it has no launch pending and its
   object is not paused */
// @retail 0x28e5c0
void swarm_creature_update_countdown(long object_index, s_object_ai_data *creature)
{
	if (!creature->velocity_pending)
	{
		s_swarm_object *object = (s_swarm_object *)object_get(object_index);
		if (!(bool)object->paused && creature->countdown > 0)
			creature->countdown--;
	}
}

/* steps an iterator along a swarm's chain of creatures */
// @retail 0x290c80
s_handler_object_view *swarm_creature_iterator_next(s_ai_object_iterator *iterator)
{
	s_handler_object_view *object = NULL;

	if (iterator->next_index != NONE)
	{
		object = handler_object_get(iterator->next_index);
		iterator->index = iterator->next_index;

		s_object_ai_data *creature = object_ai_data_checked(object);
		if (creature)
			iterator->next_index = creature->next_creature_index;
		else
			iterator->next_index = NONE;
	}

	return object;
}

/* unlinks every creature from its swarm and empties the swarm data array */
// @retail 0x28d9d0
void function_28d9d0(void)
{
	s_type_f1af8e iterator;
	iterator.signature = 0x86868686;
	iterator.type_mask = 1 << k_object_type_creature;
	iterator.flags = 0;
	iterator.index = 0;
	iterator.object_index = NONE;
	s_handler_object_view *object;
	while ((object = (s_handler_object_view *)function_baeb0(&iterator)) != NULL)
	{
		s_object_ai_data *creature = object_ai_data_checked(object);
		if (creature)
		{
			creature->swarm_handle = NONE;
			creature->actor_index = NONE;
			creature->next_creature_index = NONE;
		}
	}
	g_5044c8->valid = false;
}

void function_b9fc0(long object_index, vector3f *forward, vector3f *up);

/* gives a creature object the default orientation request (as 0x118f90
   builds it), turned to the object's own facing */
// @retail 0x28ff90
void swarm_creature_reset_orientation(long object_index)
{
	s_object_orientation_request request;
	memset(&request, 0, sizeof(request));
	request.forward = *g_4687a8;
	request.name0 = 0x6000086;
	request.name4 = 0x400000c;
	request.up = *g_4687b0;
	function_b9fc0(object_index, &request.forward, &request.up);
	swarm_object_header_get(object_index)->object->orientation = request;
	swarm_object_header_get(object_index)->object->orientation_pending = false;
}

/* resets the orientation of each creature of a swarm */
// @retail 0x290040
void swarm_reset_orientations(long swarm_handle)
{
	s_ai_object_iterator iterator;
	iterator.next_index = swarm_get(swarm_handle)->first_creature_index;
	while (swarm_creature_iterator_step(&iterator))
		swarm_creature_reset_orientation(iterator.index);
}

/* where a swarm's target is: an object, a prop or an actor, by type */
// @retail 0x28e4b0
bool swarm_target_get_location(long target_index, short target_type, long *object_index, point3f *position, vector3f *velocity)
{
	bool result = false;
	*object_index = NONE;
	if (target_index != NONE)
	{
		switch (target_type)
		{
		case _swarm_target_type_object:
		{
			s_swarm_object *object = (s_swarm_object *)handler_object_get(target_index);
			*object_index = target_index;
			function_b9dd0(target_index, position);
			*velocity = object->velocity;
			result = true;
			break;
		}
		case _swarm_target_type_prop:
		{
			s_prop_node_view *node = prop_node_get(target_index);
			s_swarm_prop_state *state = (s_swarm_prop_state *)prop_node_state(node);
			*object_index = node->object_index;
			*position = state->position;
			*velocity = state->velocity;
			result = true;
			break;
		}
		case _swarm_target_type_actor:
		{
			s_swarm_leader *actor = (s_swarm_leader *)actor_get(target_index);
			*object_index = actor->unit_index;
			*position = actor->position;
			*velocity = actor->velocity;
			result = true;
			break;
		}
		default:
			*object_index = NONE;
			break;
		}
	}
	return result;
}

void function_119020(long object_index, vector3f const *impulse);

/* applies an impulse to each creature of a swarm */
// @retail 0x2901e0
void swarm_apply_impulse(long swarm_handle, vector3f const *impulse)
{
	long object_index = swarm_get(swarm_handle)->first_creature_index;
	while (object_index != NONE)
	{
		s_object_ai_data *creature = object_ai_data_checked(handler_object_get(object_index));
		function_119020(object_index, impulse);
		if (!creature)
			break;
		object_index = creature->next_creature_index;
	}
}

/* a creature object goes away: it lets go of its target prop and leaves its
   swarm */
// @retail 0x28fd00
void swarm_creature_dispose(long object_index)
{
	s_handler_object_view *object = handler_object_get(object_index);
	if (object->type == k_object_type_creature)
	{
		s_object_ai_data *creature = object_ai_data_checked(object);
		if (creature && creature->swarm_handle != NONE)
		{
			if (creature->goal_prop_ref != NONE)
			{
				long prop_index = prop_ref_get(creature->goal_prop_ref)->prop_index;
				prop_get(prop_index)->unknown36 = false;
				creature->target_state = NONE;
			}
			swarm_remove_creature(creature->swarm_handle, object_index, false, true);
		}
	}
}

struct s_orientation_request_118f90;
void function_118f90(s_orientation_request_118f90 *request);

/* hands a creature's forward and up vectors and its pending animation to
   its object's orientation request */
// @retail 0x28f600
void swarm_creature_apply_orientation(long object_index)
{
	s_handler_object_view *object = handler_object_get(object_index);
	if (!object->flags134)
	{
		s_object_ai_data *creature = (s_object_ai_data *)((byte *)object + object->ai_offset);
		if (creature)
		{
			s_object_orientation_request request;
			function_118f90((s_orientation_request_118f90 *)&request);
			if (creature->velocity_pending)
				request.flag4 = true;
			else
				request.flag4 = false;
			request.flag3 = true;
			request.forward = creature->forward;
			request.up = creature->up;
			request.vector0c = creature->vector98;
			if (creature->animation_pending)
			{
				request.name0 = creature->animation_name0;
				request.name4 = creature->animation_name4;
				request.enabled = true;
				creature->animation_pending = false;
			}
			swarm_object_header_get(object_index)->object->orientation = request;
		}
	}
}
