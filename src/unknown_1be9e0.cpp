// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_26e370.h"

/* slot type 0x36 */

short __stdcall function_1beb70(long actor_index, s_slot *slot, bool active);
long __stdcall function_1bead0(long actor_index, s_slot *slot);
short __stdcall function_1bee40(long actor_index, long leader_index, long a, long b);
short function_1b6e50(long actor_index);

/* the prop's view as slot type 0x36 reads it */
struct s_prop_view_36
{
	byte unknown00[0x2a];
	bool unknown2a;
};

struct s_slot_36
{
	s_slot_header header;
	bool unknown0c;
	byte unknown0d[3];
	long element_index;
	byte unknown14[0x1c - 0x14];
	short unknown1c;
	byte unknown1e[0x40 - 0x1e];
};

/* a joint (an element of g_502424) as slot type 0x36 reads it */
struct s_joint_participant_36
{
	long actor_index;
	short status;
	short priority;
	real score;
};

struct s_joint_36
{
	short salt;
	short state;
	s_joint_participant_36 participants[10];
	short participant_count;
	byte unknown7e[0xbc - 0x7e];
};

void function_26edb0(long joint_index, short participant_index);
bool function_1be9e0(long actor_index, long other_index, real *distance, short *ticks);


PRIVATE __forceinline real function_1be9e2(point3f const *arg_1, point3f const *arg_0)
{
	vector3f v;

	vector3d_from_points3d(arg_1, arg_0, &v);
	return (real)sqrt(v.k * v.k + v.j * v.j + v.i * v.i);
}

// @retail 0x1be9e0
bool function_1be9e0(long actor_index, long other_index, real *distance, short *ticks)
{
	bool result = false;
	short slot_ticks = 0x7fff;
	s_actor_view *actor = actor_get(actor_index);
	s_actor_view *other = actor_get(other_index);

	real magnitude = function_1be9e2(&actor->position, &other->position);

	if (magnitude < 4.0f)
	{
		slot_ticks = function_1b6e50(other_index);
		if (slot_ticks * g_510c54->rate < 4.0f)
			result = true;
	}
	if (distance)
		*distance = magnitude;
	if (ticks)
		*ticks = slot_ticks;
	return result;
}

// @retail 0x1bead0
long __stdcall function_1bead0(long actor_index, s_slot *slot)
{
	s_slot_36 *state = (s_slot_36 *)slot;
	long result = NONE;
	s_slot_entry_iterator iterator;

	iterator.actor_index = actor_index;
	iterator.reference.unknown2 = 0x36;
	iterator.reference.unknown0 = NONE;
	for (s_slot_memory_entry *entry = function_26f0c0(&iterator); entry; entry = function_26f0c0(&iterator))
	{
		if (result == NONE)
		{
			if (function_26ecc0(actor_index, iterator.reference.unknown0, (s_joint_behavior_state *)slot))
				result = entry->unknown4;
		}
		else
		{
			joint_decline(actor_index, iterator.reference.unknown0);
		}
	}
	if (result == NONE)
	{
		long element_index = function_26e940(actor_index);

		if (element_index != NONE)
		{
			state->unknown0c = true;
			state->element_index = element_index;
		}
		result = element_index;
	}
	return result;
}

/* invites the actor's clump members near enough, the nearest first */
static __forceinline long real_to_long(real value);

/* The status is g_46fbe8 while the invitation is still running and g_46fbe4 once it is over. `waiting` is set only
   where the block is left without a return and tested once at the end: retail's layout (each g_46fbe4 return at its
   own test, the missing prop's next, the g_46fbe8 return last) comes from that one tail test. */
// @retail 0x1beb70
short __stdcall function_1beb70(long actor_index, s_slot *slot, bool active)
{
	s_slot_36 *state = (s_slot_36 *)slot;
	s_joint_36 *joint = (s_joint_36 *)element_502424_get(state->element_index);
	short accepted = 0;
	s_actor_view *actor = actor_get(actor_index);
	bool waiting = false;

	if (actor->prop_index != NONE)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_type_f95cd3 *view = function_25d740((s_prop_node *)node);

		if (node->unknown27 < 1 && (!view || !((s_prop_view_36 *)view)->unknown2a))
		{
			if (state->unknown1c > 0)
			{
				if (--state->unknown1c <= 0)
					return g_46fbe4;
			}
			else
			{
				short declined = 0;
				short i;

				for (i = 0; i < 10; i++)
				{
					s_joint_participant_36 *participant = &joint->participants[i];

					if (participant->actor_index != NONE)
					{
						if (participant->status == 0)
						{
							if (function_1be9e0(actor_index, participant->actor_index, NULL, NULL))
								accepted++;
							else
								function_26edb0(state->element_index, i);
						}
						else if (participant->status == 3)
						{
							declined++;
						}
					}
				}

				if (accepted == 0)
				{
					short result = g_46fbe8;
					real seconds = g_510c54->field_2_3 * 1.5f;
					long ticks;

					ticks = real_to_long(seconds);
					state->unknown1c = (short)ticks;
					if (declined > 1)
						function_1fb7e0(0x86, actor_index, NULL, NONE, NONE);
					return result;
				}

				short participants = joint->participant_count + accepted;

				if (participants < 2)
					return g_46fbe4;
			}
		}
		else
			return g_46fbe4;
		waiting = true;
	}
	if (!waiting)
		return g_46fbe4;
	return g_46fbe8;
}

// @retail 0x1bee40
short __stdcall function_1bee40(long actor_index, long leader_index, long a, long b)
{
	s_actor_view *actor = actor_get(actor_index);
	long count = 0;

	if (actor->unknown07c != NONE)
	{
		s_element_502420_member_iterator iterator;
		iterator.next_index = element_502420_get(actor->unknown07c)->first_actor_index;
		s_actor_view *other;
		while ((other = element_502420_member_next(&iterator)) != NULL)
		{
			real distance;
			short ticks;
			if (other != actor && function_1be9e0(actor_index, iterator.index, &distance, &ticks) &&
				function_26eae0(leader_index, iterator.index, 3, 1.0f / (distance * 10.0f + (real)ticks)))
			{
				count++;
			}
		}
	}
	return (short)count;
}
// @retail 0x1bed40
void __stdcall function_1bed40(long actor_index, s_slot *slot, s_slot_target_list *list)
{
	s_actor_view *actor = actor_get(actor_index);
	s_prop_view_fields *view = prop_node_view(prop_node_get(actor->prop_index));

	if (view->unknown8c <= 0 && !view->unknown88)
	{
		actor->unknown41c = 3;
		actor->unknown420 = 2;
		actor->unknown4a1 = true;
		return;
	}

	long target = NONE;
	short i;

	for (i = 0; i < 10; i++)
	{
		long other_index = list->entries[i].actor_index;

		if (other_index != NONE && other_index != actor_index)
			target = actor_get(other_index)->unknown018;
	}
	if (target != NONE)
	{
		actor->unknown430 = 2;
		actor->unknown434 = 6;
		actor->unknown438.object_index = target;
		actor->unknown444 = NONE;
	}
	actor->unknown4a1 = true;
}

s_slot_handler_2x g_47eda8 =
{
	{
		{
			0x36, 2, 0, -2, 0,
			function_1adcd0, function_1beb70, joint_initiate, joint_leave, NONE, {0},
			0, 0, 0, 0, 0, 0, 1
		},
		(t_slot_proc)joint_update, joint_activate, joint_deactivate
	},
	(t_slot_proc)function_1bead0, 0, (t_slot_release)slot_release_true, slot_release_nothing, function_1bed40, (t_slot_proc4)function_1bee40,
	2, 3, 4.0f, 0
};
