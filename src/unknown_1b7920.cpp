// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_20fe20.h"
#include "unknown_1e1f20.h"

static __forceinline long real_to_long(real value);

/* slot type 0x1e: the actor goes to its prop's point */

struct s_slot_1e
{
	s_slot_header header;
	long start_time;
	s_type_c3b527 point;
	short ticks;
	short delay;
	byte unknown24[0x40 - 0x24];
};

/* the block of the actor's character tag function_1e4e50 returns */
struct s_character_e50
{
	byte unknown00[8];
	real unknown08;
	byte unknown0c[0x14 - 0xc];
	real distance;
	byte unknown18[4];
	real unknown1c;
	real unknown20;
};

void *function_1e4e50(long actor_index);
point3f *function_b9dd0(long object_index, point3f *result);
void function_1f86a0(long index);

short __stdcall function_1b7920(long actor_index);
short __stdcall function_1b7b90(long actor_index, s_slot *slot, bool active);
bool __stdcall function_1b79d0(long actor_index, s_slot *slot);
void __stdcall function_1b7c70(long actor_index, s_slot *slot);
void __stdcall function_1b7cc0(long actor_index, s_slot *slot);

/* whether the actor's prop is in state 3 within 3 of where it should be */
PRIVATE __forceinline real function_1b7921(s_prop_node_view *arg_0, s_prop_view_fields *arg_1)
{
    s_prop_state_view *local_0 = prop_node_state(arg_0);
    return function_210970(&local_0->unknown48, &arg_1->unknown18);
}

// @retail 0x1b7920
short __stdcall function_1b7920(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (actor->prop_index != NONE)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_prop_view_fields *view = prop_node_view(node);

		if (view && view->unknown06 == 3 &&
			function_1b7921(node, view) < 3.0f)
		{
			result = 1;
		}
	}
	return result;
}

/* the delay is function_259d0's interval (bounds upper first, then the
   draw, rounded by its (real) cast), kept on the x87 stack as retail does;
   a call to function_259d0 itself is computed in SSE after the call to
   function_1f86a0 here (74 differences) */
// @retail 0x1b79d0
bool __stdcall function_1b79d0(long actor_index, s_slot *slot)
{
	bool result = false;
	s_character_e50 *character = (s_character_e50 *)function_1e4e50(actor_index);
	s_prop_node_view *node = prop_node_get(actor_get(actor_index)->prop_index);
	s_prop_state_view *s_type_5cfb45 = prop_node_state(node);
	s_prop_view_fields *view = prop_node_view(node);

	if (view && view->unknown10 != NONE && character)
	{
		s_slot_1e *state = (s_slot_1e *)slot;
		real sample = function_x82e52f(&g_4e7408->unknown0, __FILE__, __LINE__);
		real ticks = character->unknown08 + sample * 2.0f;
		real upper = character->unknown20;
		real lower = character->unknown1c;
		real sample2 = function_x82e52f(&g_4e7408->unknown0, __FILE__, __LINE__);
		real delay = (real)(lower + (upper - lower) * sample2);

		state->point = view->unknown18;
		state->point.point.z += s_type_5cfb45->unknown38 - s_type_5cfb45->position.z;
		function_1f86a0(actor_index);
		state->ticks = (short)real_to_long(g_510c54->field_2_3 * ticks);
		state->start_time = g_510c54->game_time;
		state->delay = (short)real_to_long(g_510c54->field_2_3 * delay);
		result = true;
	}
	return result;
}

#pragma inline_depth(0)
/* the slot's status: continue (g_46fbe8) while the delay runs, or once it is
   over while the prop's point is within the character's distance of the
   prop's object; finished (g_46fbe4) otherwise. Retail calls function_25d740 out of line here, so
   inlining is off for this function and the actor and prop node lookups are
   written out; the result is one local, as retail reloads g_46fbe8 for it
   after the calls. */
// @retail 0x1b7b90
short __stdcall function_1b7b90(long actor_index, s_slot *slot, bool active)
{
	s_actor_view *actor = (s_actor_view *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_actor_view));
	s_character_e50 *character = (s_character_e50 *)function_1e4e50(actor_index);
	short result = g_46fbe4;

	if (character && actor->prop_index != NONE)
	{
		s_slot_1e *state = (s_slot_1e *)slot;

		result = g_46fbe8;
		if (g_510c54->game_time - state->start_time > state->delay)
		{
			s_prop_node_view *node = (s_prop_node_view *)(g_502418->data + (actor->prop_index & 0xffff) * sizeof(s_prop_node_view));
			s_prop_view_fields *view = (s_prop_view_fields *)function_25d740((s_prop_node *)node);

			if (view)
			{
				point3f position;

				function_b9dd0(node->object_index, &position);
				if (function_210ac0(&view->unknown18, &position) > character->distance)
					result = g_46fbe4;
			}
			else
			{
				result = g_46fbe4;
			}
		}
	}
	return result;
}
#pragma inline_depth()

// @retail 0x1b7c70
void __stdcall function_1b7c70(long actor_index, s_slot *slot)
{
	s_slot_1e *state = (s_slot_1e *)slot;

	if (state->ticks > 0 && --state->ticks == 0)
	{
		long unit_index = actor_get(actor_index)->unknown018;

		if (unit_index != NONE)
			function_20ba60(0x20, unit_index, NONE, NONE, NONE, NULL);
	}
}

s_slot_handler_2 g_47e848 =
{
	{
		0x1e, 2, 0, -2, 0,
		function_1b7920, function_1b7b90, function_1b79d0, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	0, function_1b7c70, function_1b7cc0
};

/* how the actor's character uses its weapon, by difficulty */
struct s_weapon_difficulty_entry
{
	long unknown0;
	long unknown4;
	long unknown8;
};

struct s_character_weapon_1e
{
	byte unknown00[0x98];
	s_weapon_difficulty_entry difficulty[3];
};

PRIVATE __forceinline point3f *function_1b7cc1(s_type_c3b527 const *arg_0, point3f *arg_1)
{
	if (arg_0->output_index != NONE)
	{
		if (!function_2104b0(arg_0->output_index, &arg_0->point, arg_1))
			*arg_1 = arg_0->point;
	}
	else
	{
		*arg_1 = arg_0->point;
	}

	return arg_1;
}

// @retail 0x1b7cc0
void __stdcall function_1b7cc0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	long weapon_index = function_1e1f20(actor_index);
	s_slot_1e *state = (s_slot_1e *)slot;
	point3f point;

	function_1b7cc1(&state->point, &point);
	actor->unknown41c = 3;
	actor->unknown420 = 3;
	actor->unknown424.point = point;
	actor->unknown430 = 2;
	actor->unknown434 = 3;
	actor->unknown438.point = point;
	actor->unknown444 = NONE;
	actor->unknown488 = true;
	actor->unknown4a0 = true;
	actor->unknown48c = true;
	actor->unknown490_point = state->point;
	if (weapon_index != NONE)
	{
		s_character_weapon_1e *weapon = (s_character_weapon_1e *)function_1e5280(actor_index, object_get(weapon_index)->tag_index);

		if (weapon)
		{
			s_weapon_difficulty_entry *entry;
			short local_0 = g_4e6948->state == 1 ? g_4e6948->difficulty : 1;

			switch (local_0)
			{
			case 2:
				entry = &weapon->difficulty[1];
				break;
			case 3:
				entry = &weapon->difficulty[2];
				break;
			default:
				entry = &weapon->difficulty[0];
				break;
			}
			actor->unknown710 = entry->unknown4;
		}
	}
}