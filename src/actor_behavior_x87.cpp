// @flags /O2 /Gr
/* ACTOR_BEHAVIOR_X87.CPP: the actor behavior functions built without SSE
   (actor_behavior.cpp has the rest): the level lookup, the command script
   behavior (type 4), behavior 0x81's update48 and the bit vector "and"
   the behavior type masks use. */

#include "unknown_11c920.h"
#include "unknown_26b230.h"
#include "globals.h"
#include "slot_owner.h"
#include "slot_handler.h"
#include "actor_behavior.h"

/* the level of the actor's behavior stack that runs the given type, or
   NONE */
/* 0x274470 keeps both calls to this lookup out of line. */
__declspec(noinline) short actor_behavior_find_level(long actor_index, short type);

// @retail 0x1a6fe0
short actor_behavior_find_level(long actor_index, short type)
{
	short result = NONE;
	s_slot_owner_entry *owner = (s_slot_owner_entry *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_slot_owner_entry));
	short current = owner->current;
	short level;

	for (level = 0; level <= current; level++)
	{
		if (owner->slots[level].type == type)
		{
			result = level;
			break;
		}
	}

	return result;
}

void __stdcall actor_behavior_command_script_stop(long actor_index, s_slot *slot);
short __stdcall actor_behavior_command_script_choose(long actor_index, short level, long active);
void function_258b20(long index, long actor_index);

short function_258a00(long actor_index, long unknown, long cs_index, short *result);

/* the command script behavior's choice: while the behavior the script
   started one level down still runs, it keeps running; otherwise the
   script runs on, and the behavior finishes with it */
/* (the volatile read of result is needed to match: round 39) */
// @retail 0x1b2e80
short __stdcall actor_behavior_command_script_choose(long actor_index, short level, long active)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 2;
	s_command_script_behavior_state *state = level < 4 ? (s_command_script_behavior_state *)&actor->slots[level] : NULL;
	if (state->child_active)
	{
		s_slot *child = level + 1 < 4 ? &actor->slots[level + 1] : NULL;
		short type = child->type;
		if (type >= 0 && type < k_slot_type_count &&
			g_46eeb8[type]->unknown8 != g_46f348 &&
			(g_46eeb8[type]->mask & g_4ee4ec) == g_4ee4ec &&
			(g_557c40[type >> 5] & (1 << (type & 31))) && child->state == 0 &&
			g_46eeb8[type]->evaluate(actor_index, child, *(bool *)&active) == g_46fbe8)
		{
			result = g_46fbe8;
		}
		else
			state->child_active = false;
	}
	if (!state->child_active)
	{
		short status = function_258a00(actor_index, active, actor->command_script_index, &result);
		if (status == 1 || status == 2)
		{
			state->finished = true;
			result = 2;
		}
		else if (*(short const volatile *)&result != 2)
			state->child_active = true;
	}
	if (result != 0 && *(long *)actor->unknown01c != NONE)
	{
		actor->unknown4a4 = 7;
		actor->unknown4a8 = NONE;
		actor->unknown4a5 = 0;
	}
	return result;
}

/* the command script behavior's priority: 3 while the actor has a command
   script */
// @retail 0x1b2da0
short __stdcall actor_behavior_command_script_priority(long actor_index)
{
	short result = 0;

	if (actor_get(actor_index)->command_script_index != NONE)
		result = 3;
	return result;
}

/* the command script behavior's start */
// @retail 0x1b2de0
bool __stdcall actor_behavior_command_script_start(long actor_index, s_slot *slot)
{
	s_command_script_behavior_state *state = (s_command_script_behavior_state *)slot;

	state->child_active = false;
	state->finished = false;
	return true;
}

/* the command script behavior's stop: a script that has not finished is
   stopped */
/* Keep the call to 0x258b20; the lookup stays in this body. */
#pragma inline_depth(0)
// @retail 0x1b2e00
void __stdcall actor_behavior_command_script_stop(long actor_index, s_slot *slot)
{
	s_actor_view *actor = (s_actor_view *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_actor_view));
	s_command_script_behavior_state *state = (s_command_script_behavior_state *)slot;

	if (!state->finished && actor->command_script_index != NONE)
		function_258b20(actor->command_script_index, actor_index);
}
#pragma inline_depth(255)

/* the command script behavior's evaluate: it finishes when the actor no
   longer has a command script */
// @retail 0x1b2e40
short __stdcall actor_behavior_command_script_evaluate(long actor_index, s_slot *slot, bool active)
{
	s_command_script_behavior_state *state = (s_command_script_behavior_state *)slot;
	short result = g_46fbe8;

	if (actor_get(actor_index)->command_script_index == NONE)
	{
		state->finished = true;
		result = g_46fbe4;
	}
	return result;
}

s_slot_handler_1 g_actor_behavior_command_script_handler =
{
	{
		4, 1, NONE, -2, 0,
		actor_behavior_command_script_priority, actor_behavior_command_script_evaluate,
		actor_behavior_command_script_start, actor_behavior_command_script_stop, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_choose)actor_behavior_command_script_choose, 0, 0
};

/* behavior 0x81's update48 (its handler, g_47eff8, is in
   unknown_1c20f0.cpp): when the actor's prop is of type 1 or 2 the actor
   faces it */
// @retail 0x1c22c0
void __stdcall actor_behavior_81_update48(long actor_index, long unused)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE)
	{
		s_prop_node_view *prop = prop_node_get(actor->prop_index);

		if (prop->unknown24 >= 1 && prop->unknown24 <= 2)
		{
			actor->unknown41c = 4;
			actor->unknown420 = 2;
			actor->unknown488 = 1;
		}
	}
}

/* result = a & b, over bit_count bits */
// @retail 0x1c2330
void bit_vector_intersect(dword *result, dword *a, dword *b, long bit_count)
{
	long count = (bit_count + 31) >> 5;
	long i;

	for (i = 0; i < count; i++)
	{
		result[i] = a[i] & b[i];
	}
}
