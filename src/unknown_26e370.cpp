// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_26E370.CPP: AI joint behaviors

Actors join a joint behavior through invitations: the joint's leader invites
participants, each invited actor keeps the invitation in one of its four
invitation slots until it accepts, declines or the invitation expires. */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "slot_owner.h"
#include "slot_handler.h"
#include "unknown_26e370.h"

enum
{
	k_maximum_joint_participants = 10,
	k_maximum_joint_invitations = 4,
	k_lowest_participant_priority = 8,

	_participant_invited = 0,
	_participant_withdrawn,
	_participant_declined,
	_participant_accepted
};

struct joint_participant
{
	long actor_index;
	short status;
	short priority;
	real score;
};

struct joint_state
{
	short salt;
	short state;
	joint_participant participants[k_maximum_joint_participants];
	short participant_count;
	byte unknown7e[0xbc - 0x7e];
};

/* what an invitation carries: the head of a joint behavior state */
struct s_joint_header
{
	short type;
	byte unknown02[2];
	long joint_index;
	long expiration_time;
};

/* the joint behavior's part of an actor's behavior state (an s_slot) */
struct s_joint_behavior_state
{
	short type;
	byte unknown02[2];
	long joint_index;
	long expiration_time;
	bool waiting;
	byte unknown0d[3];
	long state_joint_index;
	short state;
	short timer;
	short participant_index;
};

/* the behavior definitions (g_46eeb8, unknown_1a8080.cpp's s_slot_handler):
   joint behaviors come in two layouts past +0x4c */
typedef long (__stdcall *t_joint_create)(long actor_index, s_joint_behavior_state *behavior);
typedef void (__stdcall *t_joint_proc)(long actor_index, s_joint_behavior_state *behavior, joint_state *joint);
typedef bool (__stdcall *t_joint_test)(long actor_index, s_joint_behavior_state *behavior, joint_state *joint);
typedef short (__stdcall *t_joint_gather)(long actor_index, long joint_index, s_joint_behavior_state *behavior, struct s_joint_invitation_request *request);
typedef short (__stdcall *t_joint_slot_proc)(long actor_index, short slot_index, bool active, joint_state *joint);

struct s_joint_behavior_definition
{
	byte unknown00[0x4c];
	t_joint_create create;
	t_joint_proc leave;
	t_joint_test update;
	t_joint_proc activate;
	t_joint_proc deactivate;
	t_joint_gather gather;
	short minimum_participants;
	short maximum_participants;
	real invitation_seconds;
};

struct s_joint_behavior_definition_b
{
	byte unknown00[0x4c];
	t_joint_create create;
	byte unknown50[4];
	t_joint_slot_proc update;
	t_joint_gather gather;
	short minimum_participants;
	short maximum_participants;
	real invitation_seconds;
};

#define JOINT_DEFINITION(type) ((s_joint_behavior_definition *)g_46eeb8[(type)])
#define JOINT_DEFINITION_B(type) ((s_joint_behavior_definition_b *)g_46eeb8[(type)])

/* an invitation the leader builds for function_26ec20 */
struct s_joint_invitation_request
{
	long unknown00;
	long unknown04;
	s_joint_header invitation;
};

/* elements of g_51eca4 hold a joint index at +4 */
struct s_joint_reference
{
	short salt;
	byte unknown02[2];
	long joint_index;
};

s_record_pool *g_502424;
s_record_pool *g_51eca4;
short g_470fdc = NONE;

#define JOINT_STATE(index) ((joint_state *)(g_502424->data + ((index) & 0xffff) * sizeof(joint_state)))
#define ACTOR_ENTRY(index) ((s_slot_owner_entry *)(g_4f55f0->data + ((index) & 0xffff) * sizeof(s_slot_owner_entry)))

// @retail 0x26e940
long function_26e940(long actor_index)
{
	long joint_index = NONE;

	if (ACTOR_ENTRY(actor_index)->joint_index != NONE)
	{
		joint_index = record_pool_allocate(g_502424);
		if (joint_index != NONE)
		{
			joint_state *joint = JOINT_STATE(joint_index);

			for (short i = 1; i < k_maximum_joint_participants; i++)
			{
				joint->participants[i].actor_index = NONE;
				joint->participants[i].status = _participant_invited;
			}
			joint->participants[0].actor_index = actor_index;
			joint->participants[0].status = _participant_accepted;
			joint->participant_count = 1;
		}
	}
	return joint_index;
}

// @retail 0x26e9c0
short function_26e9c0(long actor_index, long joint_index)
{
	s_slot_owner_entry *actor = ACTOR_ENTRY(actor_index);

	for (short i = 0; i < k_maximum_joint_invitations; i++)
	{
		if (actor->joint_invitations[i].type != NONE && actor->joint_invitations[i].joint_index == joint_index)
			return i;
	}
	return NONE;
}

// @retail 0x26ed40
bool joint_decline(long actor_index, short invitation_index)
{
	short const *reference = &invitation_index;
	s_joint_invitation *invitation = &ACTOR_ENTRY(actor_index)->joint_invitations[*reference];
	joint_state *joint = JOINT_STATE(invitation->joint_index);

	joint->participants[invitation->participant_index].actor_index = NONE;
	joint->participants[invitation->participant_index].status = _participant_withdrawn;
	invitation->type = NONE;
	return true;
}

// @retail 0x26ea10
short function_26ea10(long actor_index, short priority)
{
	s_slot_owner_entry *actor = ACTOR_ENTRY(actor_index);
	short result = NONE;
	short lowest_priority = k_lowest_participant_priority;
	short lowest_index = NONE;

	for (short i = 0; i < k_maximum_joint_invitations; i++)
	{
		s_joint_invitation *invitation = &actor->joint_invitations[i];

		if (invitation->type == NONE)
		{
			result = i;
			break;
		}
		if (invitation->expiration_time < g_510c54->game_time)
		{
			joint_decline(actor_index, i);
			result = i;
			break;
		}

		short participant_priority = JOINT_STATE(invitation->joint_index)->participants[invitation->participant_index].priority;
		if (participant_priority < lowest_priority)
		{
			lowest_priority = participant_priority;
			lowest_index = i;
		}
	}

	if (result == NONE && lowest_priority < priority && lowest_index != NONE)
	{
		joint_decline(actor_index, lowest_index);
		result = lowest_index;
	}
	return result;
}

// @retail 0x26eae0
bool function_26eae0(long joint_index, long actor_index, short priority, real score)
{
	bool result = false;

	if (function_26ea10(actor_index, priority) != NONE)
	{
		joint_state *joint = JOINT_STATE(joint_index);
		short i = 0;

		while (joint->participants[i].actor_index != NONE && i < k_maximum_joint_participants)
			i++;
		if (i < k_maximum_joint_participants)
		{
			joint->participants[i].actor_index = actor_index;
			joint->participants[i].score = score;
			joint->participants[i].priority = priority;
			result = true;
		}
	}
	return result;
}

// @retail 0x26eb70
void function_26eb70(long actor_index, s_joint_header const *behavior)
{
	s_slot_owner_entry *actor = ACTOR_ENTRY(actor_index);
	joint_state *joint = JOINT_STATE(behavior->joint_index);

	for (short i = 0; i < k_maximum_joint_participants; i++)
	{
		if (joint->participants[i].actor_index == actor_index)
		{
			short invitation_index = function_26ea10(actor_index, joint->participants[i].priority);

			if (invitation_index != NONE)
			{
				actor->joint_invitations[invitation_index].joint_index = behavior->joint_index;
				actor->joint_invitations[invitation_index].type = behavior->type;
				actor->joint_invitations[invitation_index].expiration_time = behavior->expiration_time;
				actor->joint_invitations[invitation_index].participant_index = i;
				return;
			}
		}
	}
}

// @retail 0x26ec20
void function_26ec20(s_joint_header const *behavior, long leader_index)
{
	joint_state *joint = JOINT_STATE(behavior->joint_index);

	for (short i = 0; i < k_maximum_joint_participants; i++)
	{
		long actor_index = joint->participants[i].actor_index;

		if (actor_index != NONE && actor_index != leader_index)
		{
			s_slot_owner_entry *actor = ACTOR_ENTRY(actor_index);
			short invitation_index = function_26ea10(actor_index, joint->participants[i].priority);

			if (invitation_index != NONE)
			{
				actor->joint_invitations[invitation_index].joint_index = behavior->joint_index;
				actor->joint_invitations[invitation_index].type = behavior->type;
				actor->joint_invitations[invitation_index].expiration_time = behavior->expiration_time;
				actor->joint_invitations[invitation_index].participant_index = i;
			}
		}
	}
}

// @retail 0x26ecc0
bool function_26ecc0(long actor_index, short invitation_index, s_joint_behavior_state *behavior)
{
	short const *reference = &invitation_index;
	s_joint_invitation *invitation = &ACTOR_ENTRY(actor_index)->joint_invitations[*reference];
	short participant_index = invitation->participant_index;
	joint_state *joint = JOINT_STATE(invitation->joint_index);

	joint->participants[participant_index].status = _participant_accepted;
	behavior->participant_index = participant_index;
	behavior->waiting = false;
	behavior->state_joint_index = invitation->joint_index;
	joint->participant_count++;
	invitation->type = NONE;
	return true;
}

// @retail 0x26edb0
void function_26edb0(long joint_index, short participant_index)
{
	joint_participant *participant = &JOINT_STATE(joint_index)->participants[participant_index];

	if (participant->status == _participant_invited)
	{
		s_slot_owner_entry *actor = ACTOR_ENTRY(participant->actor_index);

		for (short i = 0; i < k_maximum_joint_invitations; i++)
		{
			s_joint_invitation *invitation = &actor->joint_invitations[i];

			if (invitation->type != NONE && invitation->joint_index == joint_index)
			{
				joint_decline(participant->actor_index, i);
				break;
			}
		}
	}
	participant->status = _participant_declined;
}

// @retail 0x26ee40
void function_26ee40(s_joint_behavior_state *behavior)
{
	joint_state *joint = JOINT_STATE(behavior->state_joint_index);

	joint->participants[behavior->participant_index].actor_index = NONE;
	joint->participants[behavior->participant_index].status = _participant_withdrawn;
	if (--joint->participant_count == 0)
	{
		for (short i = 0; i < k_maximum_joint_participants; i++)
		{
			joint_participant *participant = &joint->participants[i];

			if (participant->actor_index != NONE && participant->status == _participant_invited)
			{
				s_slot_owner_entry *actor = (s_slot_owner_entry *)datum_get_inlined(g_4f55f0, participant->actor_index);

				if (actor)
				{
					short invitation_index = function_26e9c0(participant->actor_index, behavior->state_joint_index);

					if (invitation_index != NONE)
					{
						actor->joint_invitations[invitation_index].type = NONE;
						actor->joint_invitations[invitation_index].joint_index = NONE;
						actor->joint_invitations[invitation_index].participant_index = NONE;
					}
				}
			}
		}
		record_pool_release(g_502424, behavior->state_joint_index);
	}
}

// @retail 0x26ef40
long joint_count_invited_participants(joint_state const *joint)
{
	long count = 0;

	for (short i = 0; i < k_maximum_joint_participants; i++)
	{
		if (joint->participants[i].actor_index != NONE && joint->participants[i].status == _participant_invited)
			count++;
	}
	return count;
}

// @retail 0x26e370
void joint_clear_references(long joint_index)
{
	long index = NONE;

	while ((index = data_next_absolute_index_inlined(g_51eca4, index + 1)) != NONE)
	{
		s_joint_reference *reference = (s_joint_reference *)(g_51eca4->data + g_51eca4->size * index);

		if (!reference)
			break;
		if (reference->joint_index == joint_index)
			reference->joint_index = NONE;
	}
}

// @retail 0x26ef70
void function_26ef70(long joint_index, short maximum_participants)
{
	joint_state *joint = JOINT_STATE(joint_index);

	for (short i = 0; i < k_maximum_joint_participants; i++)
	{
		joint_participant *participant = &joint->participants[i];

		if (participant->actor_index != NONE && participant->status == _participant_invited)
		{
			s_slot_owner_entry *actor = ACTOR_ENTRY(participant->actor_index);

			for (short j = 0; j < k_maximum_joint_invitations; j++)
			{
				if (actor->joint_invitations[j].joint_index == joint_index)
				{
					joint_decline(participant->actor_index, j);
					break;
				}
			}
		}
	}

	short count = joint->participant_count;
	if (count > maximum_participants)
	{
		do
		{
			real lowest_score = 3.4028235e38f;
			short lowest_index = NONE;

			for (short j = 1; j < k_maximum_joint_participants; j++)
			{
				if (joint->participants[j].actor_index != NONE && joint->participants[j].status == _participant_accepted &&
					lowest_score > joint->participants[j].score)
				{
					lowest_score = joint->participants[j].score;
					lowest_index = j;
				}
			}
			if (lowest_index == NONE)
				break;
			function_26edb0(joint_index, lowest_index);
		}
		while (--count > maximum_participants);
	}
}

// @retail 0x26e3d0
bool function_26e3d0(long joint_index, short minimum_participants, short maximum_participants, s_joint_behavior_state *behavior)
{
	joint_state *joint = JOINT_STATE(joint_index);
	bool result = true;

	switch (joint->state)
	{
	case 0:
		if (behavior->waiting)
		{
			short invited = (short)joint_count_invited_participants(joint);

			if (--behavior->timer <= 0 || invited == 0)
			{
				if (joint->participant_count >= minimum_participants)
				{
					if (joint->participant_count + invited >= maximum_participants)
						function_26ef70(joint_index, maximum_participants);
					joint->state = 1;
				}
				else
				{
					joint->state = 3;
					result = false;
				}
			}
		}
		else if (joint->participants[behavior->participant_index].status == _participant_declined)
		{
			result = false;
		}
	case 1:
		if (behavior->state == 0 && !behavior->waiting &&
			joint->participants[behavior->participant_index].status == _participant_declined)
			result = false;
		break;
	case 2:
	case 3:
		result = false;
		break;
	}

	behavior->state = joint->state;
	return result;
}

// @retail 0x26e600
void __stdcall joint_leave(long actor_index, s_slot *slot)
{
	s_joint_behavior_state *behavior = (s_joint_behavior_state *)slot;
	joint_state *joint = JOINT_STATE(behavior->state_joint_index);

	if (joint)
	{
		if (JOINT_DEFINITION(behavior->type)->leave)
			JOINT_DEFINITION(behavior->type)->leave(actor_index, behavior, joint);
		function_26ee40(behavior);
	}
}

// @retail 0x26e650
bool __stdcall joint_update(long actor_index, s_slot *slot)
{
	s_joint_behavior_state *behavior = (s_joint_behavior_state *)slot;
	s_joint_behavior_definition *definition = JOINT_DEFINITION(behavior->type);
	long joint_index = behavior->state_joint_index;
	joint_state *joint = JOINT_STATE(joint_index);
	bool result = false;

	if (function_26e3d0(joint_index, definition->minimum_participants, definition->maximum_participants, behavior))
	{
		if (definition->update)
			return definition->update(actor_index, behavior, joint);
		return true;
	}
	return result;
}

// @retail 0x26e6d0
void __stdcall joint_activate(long actor_index, s_slot *slot)
{
	if (JOINT_DEFINITION(((s_joint_behavior_state *)slot)->type)->activate)
		JOINT_DEFINITION(((s_joint_behavior_state *)slot)->type)->activate(actor_index, (s_joint_behavior_state *)slot,
			JOINT_STATE(((s_joint_behavior_state *)slot)->state_joint_index));
}

// @retail 0x26e710
void __stdcall joint_deactivate(long actor_index, s_slot *slot)
{
	if (JOINT_DEFINITION(((s_joint_behavior_state *)slot)->type)->deactivate)
		JOINT_DEFINITION(((s_joint_behavior_state *)slot)->type)->deactivate(actor_index, (s_joint_behavior_state *)slot,
			JOINT_STATE(((s_joint_behavior_state *)slot)->state_joint_index));
}

PRIVATE inline long joint_invitation_ticks(real seconds)
{
	real value = g_510c54->field_2_3 * seconds;
	long ticks;

	__asm
	{
		fld value
		fistp ticks
	}
	return ticks;
}

// @retail 0x26e4b0
bool __stdcall joint_initiate(long actor_index, s_slot *slot)
{
	s_joint_behavior_state *behavior = (s_joint_behavior_state *)slot;
	s_joint_behavior_definition *definition = JOINT_DEFINITION(behavior->type);
	bool result = true;
	long joint_index;

	if (!definition->create || (joint_index = definition->create(actor_index, behavior)) == NONE)
		return false;

	if (behavior->waiting)
	{
		s_joint_invitation_request request = { 5, NONE };
		long ticks;

		ticks = joint_invitation_ticks(definition->invitation_seconds) > 2 ? joint_invitation_ticks(definition->invitation_seconds) : 2;
		request.invitation.type = behavior->type;
		request.invitation.joint_index = joint_index;
		request.invitation.expiration_time = g_510c54->game_time + ticks;

		result = definition->gather(actor_index, joint_index, behavior, &request) + 1 >= definition->minimum_participants;
		if (result)
		{
			behavior->timer = (short)ticks;
			function_26ec20(&request.invitation, actor_index);
		}
		else
		{
			joint_leave(actor_index, slot);
			return result;
		}
	}

	behavior->state = JOINT_STATE(joint_index)->state;
	return result;
}

// @retail 0x26e750
bool __stdcall joint_initiate_b(long actor_index, s_slot *slot)
{
	s_joint_behavior_state *behavior = (s_joint_behavior_state *)slot;
	s_joint_behavior_definition_b *definition = JOINT_DEFINITION_B(behavior->type);
	bool result = true;
	long joint_index;

	if (!definition->create || (joint_index = definition->create(actor_index, behavior)) == NONE)
		return false;

	if (behavior->waiting)
	{
		s_joint_invitation_request request = { 5, NONE };
		long ticks;

		ticks = joint_invitation_ticks(definition->invitation_seconds) > 2 ? joint_invitation_ticks(definition->invitation_seconds) : 2;
		request.invitation.type = behavior->type;
		request.invitation.joint_index = joint_index;
		request.invitation.expiration_time = g_510c54->game_time + ticks;

		result = definition->gather(actor_index, joint_index, behavior, &request) + 1 >= definition->minimum_participants;
		if (result)
		{
			behavior->timer = (short)ticks;
			function_26ec20(&request.invitation, actor_index);
		}
		else
		{
			joint_leave(actor_index, slot);
			return result;
		}
	}

	behavior->state = JOINT_STATE(joint_index)->state;
	return result;
}

// @retail 0x26e8a0
short __stdcall function_26e8a0(long actor_index, short slot_index, bool active)
{
	s_slot *slot = &ACTOR_ENTRY(actor_index)->slots[slot_index];
	s_joint_behavior_definition_b *definition = JOINT_DEFINITION_B(slot->type);
	s_joint_behavior_state *behavior = (s_joint_behavior_state *)slot;

	if (function_26e3d0(behavior->state_joint_index, definition->minimum_participants, definition->maximum_participants, behavior))
		return definition->update(actor_index, slot_index, active, JOINT_STATE(behavior->state_joint_index));
	return g_470fdc;
}

// @retail 0x26f060
bool actor_has_joint_invitation(long actor_index, short type)
{
	s_slot_owner_entry *actor = ACTOR_ENTRY(actor_index);
	bool result = false;

	for (short i = 0; i < k_maximum_joint_invitations; i++)
	{
		if (actor->joint_invitations[i].type == type && actor->joint_invitations[i].expiration_time > g_510c54->game_time)
		{
			result = true;
			break;
		}
	}
	return result;
}
