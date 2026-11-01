// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_290E20.CPP: the scenario's named ai triggers (0x30 bytes each at
   +0x248 of g_4e0350): a lookup by name and the test whether enough of a
   trigger's conditions hold for a squad or squad group (outside functions
   lane A's ai script query 0x275d10 needs) */

#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_272b70.h"
#include "bit_vector.h"
#include <string.h>

/* a condition of a trigger (0x38 bytes) */
struct s_ai_trigger_condition
{
	byte unknown00[0x38];
};

struct s_ai_trigger
{
	char name[0x20];
	byte flags;
	byte unknown21[3];
	short combine_mode;
	byte unknown26[2];
	long condition_count;
	s_ai_trigger_condition *conditions;
};

struct s_scenario_ai_triggers_view
{
	byte unknown000[0x248];
	long trigger_count;
	s_ai_trigger *triggers;
};

/* the triggers that have fired once (ai.cpp) */
extern void *g_5044cc;

extern long g_50242c;
long __stdcall function_2093d0(long arg_0, bool arg_1);

struct s_291740
{
	byte field_0[0x4c];
	short field_4c;
	byte field_4e[0x7c - 0x4e];
};

struct s_291769
{
	byte field_0[0x24];
	long field_24;
};

struct s_291746
{
	byte field_0[0x1bc];
	s_291769 *field_1bc;
	byte field_1c0[0x244 - 0x1c0];
	s_291740 *field_244;
};

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x291740
void function_291740(short arg_0, long arg_1)
{
	long const *local_2 = &arg_1;
	s_291746 *local_0 = (s_291746 *)g_4e0350;
	s_291740 *local_1 = &local_0->field_244[arg_0];
	switch (local_1->field_4c)
	{
	case NONE:
		break;
	default:
		g_50242c = *local_2;
		_ReadWriteBarrier();
		function_2093d0(local_0->field_1bc[local_1->field_4c].field_24, false);
		break;
	}
}
#pragma function(_ReadWriteBarrier)

bool __stdcall function_2912c0(s_ai_trigger_condition *condition, long squad_group_index, bool *result);
bool function_290f60(s_ai_trigger_condition *condition, bool *result, long squad_index);

struct s_291800
{
	byte field_0[8];
	long field_8;
	long *field_c;
};

struct s_291801
{
	long field_0[20];
	long field_50[20];
};

struct s_291802
{
	long field_0;
	short field_4;
	short field_6;
	long field_8;
};

struct s_291803
{
	byte field_0[0x2c];
	long field_2c;
	s_291802 *field_30;
};

real function_20e190(long arg_0);

// @retail 0x291800
real function_291800(long arg_0, s_291800 const *arg_1, s_291801 const *arg_2,
	short arg_3, long *arg_4, short *arg_5)
{
	real local_0 = 0.0f;
	s_actor_view *local_2;
	bool local_3;
	*arg_4 = 0;
	*arg_5 = NONE;
	for (short local_1 = 0; local_1 < arg_3; local_1++)
	{
		if (arg_2->field_0[local_1] == arg_0)
			goto local_16;
	}
	local_2 = actor_get(arg_0);
	if (local_2->unknown018 == NONE)
		goto local_16;
	long local_17 = arg_1->field_8;
	if (local_17 > 0)
	{
		byte *local_4 = (byte *)object_get(local_2->unknown018);
		long *local_5 = (long *)(local_4 + *(short *)(local_4 + 0x342));
		if (local_5[0] == NONE)
			goto local_16;
		for (short local_6 = 0; local_6 < arg_3; local_6++)
		{
			if (arg_2->field_50[local_6] == local_5[1])
				goto local_8;
		}
		for (short local_7 = 0; local_7 < local_17; local_7++)
		{
			if (arg_1->field_c[local_7] == local_5[1])
			{
				*arg_4 = local_5[1];
				goto local_14;
			}
		}
local_8:
		{
			s_291803 *local_9 = (s_291803 *)g_4e3b44[(*(long *)((byte *)local_2 + 0x54)) & 0xffff].bytes;
			long local_20 = local_9->field_2c;
			if (local_20 <= 0)
				goto local_16;
			for (short local_10 = 0; local_10 < local_17; local_10++)
			{
				long const *local_21 = &arg_1->field_c[local_10];
				for (short local_11 = 0; local_11 < arg_3; local_11++)
				{
					if (arg_2->field_50[local_11] == *local_21)
						goto local_13;
				}
				for (short local_12 = 0; local_12 < local_20; local_12++)
				{
					s_291802 const *local_19 = &local_9->field_30[local_12];
					if (local_19->field_8 == *local_21)
					{
						*arg_4 = local_19->field_8;
						*arg_5 = local_19->field_4;
						local_3 = false;
						goto local_15;
					}
				}
local_13:;
			}
			goto local_16;
		}
	}
local_14:
	local_3 = true;
local_15:
	local_0 = function_20e190(local_2->unknown018);
	if (local_3)
		local_0 += 1.0f;
local_16:
	return local_0;
}

struct s_290d90_entry
{
	long inverted : 1;
	dword others : 31;
	short trigger_index;
	short unknown06;
};

struct s_290d90_group
{
	short result;
	short combine_mode;
	real delay;
	short value;
	short unknown0a;
	long count;
	s_290d90_entry *entries;
};

bool function_290e20(short trigger_index, long squad_index, long squad_group_index);

// @retail 0x290d90
bool function_290d90(s_290d90_group *group, long squad_index, long squad_group_index)
{
	short count = 0;
	short needed;
	if (group->combine_mode == 0)
		needed = 1;
	else
		needed = (short)group->count;
	for (short i = 0; i < group->count; i++)
	{
		s_290d90_entry *entry = &group->entries[i];
		if (entry->trigger_index != NONE &&
			function_290e20(entry->trigger_index, squad_index, squad_group_index) != (bool)entry->inverted)
		{
			count++;
			if (count >= needed)
				break;
		}
	}
	return count >= needed;
}

struct s_290cd0_definition
{
	byte unknown00[0x74];
	long count;
	s_290d90_group *groups;
};

struct s_290cd0_scenario
{
	byte unknown00[0x244];
	s_290cd0_definition *definitions;
};

// @retail 0x290cd0
short function_290cd0(short index, long squad_index, long squad_group_index, short *ticks, short *value)
{
	s_290cd0_definition *definition = &((s_290cd0_scenario *)g_4e0350)->definitions[index];
	long result = NONE;
	for (short i = 0; i < definition->count; i++)
	{
		s_290d90_group *group = &definition->groups[i];
		if (function_290d90(group, squad_index, squad_group_index))
		{
			result = group->result;
			if (value)
			{
				short adjusted = group->value;
				*value = adjusted == 0 ? (short)NONE : (short)(adjusted + 0x6e);
			}
			if (ticks)
			{
				real scaled = g_510c54->field_2_3 * group->delay;
				long rounded;
				__asm
				{
					fld scaled
					fistp rounded
				}
				*ticks = (short)rounded;
			}
		}
	}
	return (short)result;
}

// @retail 0x291790
short ai_trigger_find_by_name(char const *name)
{
	short result = NONE;
	s_scenario_ai_triggers_view *scenario = (s_scenario_ai_triggers_view *)g_4e0350;

	if (scenario)
	{
		short i;
		for (i = 0; i < scenario->trigger_count; i++)
		{
			s_ai_trigger *trigger = &((s_scenario_ai_triggers_view *)g_4e0350)->triggers[i];
			if (!_strnicmp(trigger->name, name, 0x20))
			{
				result = i;
				break;
			}
		}
	}
	return result;
}

// score 24 (was 24): retail makes the zero in EAX before the flag test but stores it
// (count, then result from al) only after the early return, and so loads the global into
// ECX; ours stores the count in the entry block and loads the global into EAX. Tried:
// count and result zeroed (or declared) after the early return, count first 30 (the zero
// is then made there and the global goes to EAX), result first 97; also with i zeroed at
// its declaration 30; result initialized at its declaration 102 or 111; count declared
// first with i zeroed at its declaration 33; result = (bool)count 24; the two early tests
// as nested ifs 24; the latch test in a named bool 26; result declared at its assignment 26.
// @retail 0x290e20
bool function_290e20(short trigger_index, long squad_index, long squad_group_index)
{
	s_ai_trigger *trigger = &((s_scenario_ai_triggers_view *)g_4e0350)->triggers[trigger_index];
	short count = 0;
	short needed;
	short i;
	bool result;

	if ((trigger->flags & 1) && bit_vector_test((dword const *)g_5044cc, trigger_index))
		return true;

	result = false;
	if (trigger->combine_mode == 0)
		needed = 1;
	else
		needed = (short)trigger->condition_count;

	for (i = 0; i < trigger->condition_count; i++)
	{
		s_ai_trigger_condition *condition = &trigger->conditions[i];

		if ((squad_group_index != NONE && function_2912c0(condition, squad_group_index, &result)) ||
			(squad_index != NONE && function_290f60(condition, &result, squad_index)) ||
			(squad_index == NONE && squad_group_index == NONE && function_290f60(condition, &result, NONE)))
		{
			count++;
			if (count >= needed)
				break;
		}
	}

	bool triggered;
	if (count >= needed)
	{
		triggered = true;
		if (trigger->flags & 1)
			bit_vector_set((dword *)g_5044cc, trigger_index, true);
	}
	else
		triggered = false;
	return triggered;
}

/* the scenario's scenes (0x18 bytes each at +0x174 of g_4e0350): a name, the
   trigger conditions that start one and its roles */
struct s_ai_scene_condition
{
	byte flags;
	byte unknown01[3];
	word trigger_index;
	byte unknown06[2];
};

struct s_ai_scene_trigger
{
	short combine_mode;
	byte unknown02[2];
	long condition_count;
	s_ai_scene_condition *conditions;
};

struct s_ai_scene
{
	long name;
	byte flags;
	byte unknown05[3];
	long trigger_count;
	s_ai_scene_trigger *triggers;
	long role_count;
	void *roles;
};

struct s_scenario_ai_scenes_view
{
	byte unknown000[0x170];
	long scene_count;
	s_ai_scene *scenes;
};

/* one assignment of actors to a scene's roles (0x8c bytes) */
struct s_ai_scene_assignment
{
	long actor_indices[10];
	real scores[10];
	long variant_names[10];
	short variant_indices[10];
};

// score 12 (was 12): the maximum count lives in esi through the loop as
// retail has it, the role index is reloaded from its argument slot at each use
// as retail does, the assignment copy reads the previous assignment by index
// and keeps retail's temporary register, and the recursion's arguments are
// loaded and pushed in retail's order and registers.
// Ours still reads the maximum-count slot with movsx (16 bits) where retail
// reads a dword, so the maximum-count parameter is wider in retail than our
// declaration allows; that one byte makes our body a byte longer and shifts
// every exit displacement. The assignment stores also load their two operands
// in the other order.
// and reloads the role index at each use; ours keeps the role index in esi.
// Retail reads the maximum-count argument slot as a dword, so a 32-bit
// parameter scores 70 plain, 56 with a 16-bit view of it and 91 with the
// role index in a local; the role index in a local alone scores 99.
// The choice follows the role index's uses (dropping its recursion or call use
// flips it) and the iterator callees' file being linked in. Parameters used
// directly (address forms removed) 56; next role as a long local 61 (347 bytes);
// `while (next && count < maximum)` 56; pre-increment test 56; flipped compares 56;
// outputs/score declared outside the loop 56; local_0 recomputed each pass worse;
// parameter names in either alphabetical order 56; score stored first 57; the
// recursion arm first 64; a short next-role local 88. Screened (copies in one LTCG
// build): the count in a local, the copy by index or by pointer step, the last-role
// test at its use, the role pointer or assignment pointer later, the iterator at
// the top, a NONE default with one exit: identical code or worse.
// Round 242: default-selected result with early rejection/positive enumeration 56/56;
// initialized selected result with positive enumeration 56; native-width completion result 123.
// Actual short role advanced/restored around recursion with retained/live completion and
// assignment/recursion first 96/119/102/120; native-width role state 243/248/236/244.
// Original recursive body restored; capacity/role allocation remains.
// Four parallel output-slot pointers before scoring or on success, crossed with the
// actual previous-assignment source and selector-output store priority: eight at 56;
// previous-source copies reach 347 bytes. All one-use element-pointer forms restored.
// Round 292: Entry-capacity and completed-copy capacity regions, each terminal return or positive
// scope and all eight nonoriginal combinations, 56 each; original exits restored.
// Lane 13 r14: only a long next-role local passed to the recursion gives retail's maximum count in
// ESI (61 at 347 bytes, aligned 21 against 24; the recursion then adds 1 to the sign-extended store
// index), so the raw `role_index + 1` argument is what gives the role index the register; that
// local in the flag test alone 56; the iterator's actor index in a named local 56; the assignment
// pointer recomputed once after the score test 123 (or with the copy through it 120); return for
// the inner break 56; `if (local_7 <= 0.0f) continue;` 59, `if (!(local_7 > 0.0f)) continue;` 56;
// `0.0f < local_7` 56.
// Round 38 (m3 lane 10, fast): the movsx is the int copy's; every form without it leaves the maximum a
// parameter and the role index then takes ESI (59 at 350 bytes): a short copy 59 (propagated), a 32-bit
// parameter with a short copy 59, a long role copy for the four stores 59 (12 on the int form, same code),
// that copy also passed to the scorer 100, a next-role local declared at the recursion 59, `if
// (++*assignment_count >= maximum_count) break;` 59, `return` for the leaf break 59, the count test in the
// while condition 59 (12 on the int form). The long next-role local from the entry (the 61 form, now 59 at
// 347 bytes) is the only one that gives the maximum ESI and retail's dword load; the recursion then adds 1 to
// the sign-extended store index.
// Round 47 (m3 lane 13, fast): maximum as an unsigned word parameter with a short copy 59 (copy propagated,
// compares read the slot as a 16-bit operand); the role index read once through its address (recursion
// `*role_reference + 1`) on the direct form 59 (the read folds, same code); the actor index in a local used by
// the scorer and the first store 12 (same code); the int copy declared at the top 25; the recursion passing the
// parameter while the compares use the int copy 76. Ours reloads the role index before the actor index in the
// store block (a spilled candidate's reload at the block head); retail loads it after, in expression order.
// @retail 0x2919e0
void __stdcall function_2919e0(s_ai_scene *scene, s_ai_scene_assignment *assignments, short *assignment_count, short maximum_count,
	short role_index, short role_count, long ai_index, long ai_index2, long ai_index3)
{
	s_ai_scene_assignment *local_0 = &assignments[*assignment_count];
	s_291800 *local_1 = &((s_291800 *)scene->roles)[role_index];
	bool local_2 = role_index + 1 >= role_count;
	long local_3;
	switch (*(short *)((byte *)local_1 + 4))
	{
	case 0: local_3 = ai_index; break;
	case 1: local_3 = ai_index2; break;
	case 2: local_3 = ai_index3; break;
	default: return;
	}
	if (local_3 == NONE)
		return;
	int limit = maximum_count;
	s_ai_actor_iterator local_4;
	ai_actor_iterator_new(local_3, &local_4);
	while (ai_actor_iterator_next(&local_4))
	{
		if (*assignment_count >= (short)limit)
			break;
		long local_5;
		short local_6;
		real local_7 = function_291800(local_4.actor_index, local_1, (s_291801 *)local_0, role_index, &local_5, &local_6);
		if (local_7 > 0.0f)
		{
			local_0->actor_indices[role_index] = local_4.actor_index;
			local_0->variant_names[role_index] = local_5;
			local_0->scores[role_index] = local_7;
			local_0->variant_indices[role_index] = local_6;
			if (local_2)
			{
				++*assignment_count;
				if (*assignment_count >= (short)limit)
					break;
				local_0 = &assignments[*assignment_count];
				*local_0 = assignments[*assignment_count - 1];
			}
			else
			{
				function_2919e0(scene, assignments, assignment_count, limit, role_index + 1, role_count,
					ai_index, ai_index2, ai_index3);
				local_0 = &assignments[*assignment_count];
			}
		}
	}
}

/* the actors and the units (local views) */
struct s_ai_scene_actor
{
	byte unknown000[0x18];
	long unit_index;
	byte unknown01c[0x30 - 0x1c];
	short squad_index;
	byte unknown032[0x888 - 0x32];
};

struct s_ai_scene_unit
{
	long definition_index;
	byte unknown004[0x342 - 4];
	short variant_offset;
};

struct s_ai_scene_unit_variant
{
	long name;
	long local_dbe893;
};

struct s_ai_scene_model_variant
{
	byte unknown00[0x34];
	long name;
};

struct s_ai_scene_model
{
	byte unknown00[0x50];
	long variant_count;
	s_ai_scene_model_variant *variants;
};

/* the scenes that have started once (ai.cpp) */
extern void *g_5044d0;

long function_272b70(long ai_index);
bool function_2580c0(short squad_index, short script_index, long *actor_indices, short count);
void __stdcall function_2919e0(s_ai_scene *scene, s_ai_scene_assignment *assignments, short *assignment_count, short maximum_count,
	short role_index, short role_count, long ai_index, long ai_index2, long ai_index3);

/* starts a scene (by name) with the actors the ai indices name when its
   trigger conditions hold */
// @retail 0x291b40
bool function_291b40(long name, short command_script_index, long ai_index, long ai_index2, long ai_index3)
{
	bool result = false;
	s_scenario_ai_scenes_view *scenario = (s_scenario_ai_scenes_view *)g_4e0350;
	short scene_index;

	for (scene_index = 0; scene_index < scenario->scene_count; scene_index++)
	{
		s_ai_scene *scene = &scenario->scenes[scene_index];
		if (scene->name == name)
		{
			s_ai_scene_assignment assignments[12];
			short assignment_count = 0;

			if (!(scene->flags & 1) && (((dword *)g_5044d0)[scene_index >> 5] & (1 << (scene_index & 0x1f))))
				goto done;

			if (scene->trigger_count > 0 && scene->triggers->condition_count > 0)
			{
				s_ai_scene_trigger *trigger = scene->triggers;
				short squad_index = NONE;
				short squad_group_index = NONE;
				bool any = trigger->combine_mode == 0;
				bool all;
				short i;

				switch ((dword)ai_index >> 30)
				{
				case 0:
					squad_index = (short)ai_index;
					break;
				case 1:
					squad_group_index = (short)ai_index;
					break;
				case 2:
				case 3:
				{
					long actor_index = function_272b70(ai_index);
					if (actor_index != NONE)
						squad_index = ((s_ai_scene_actor *)g_4f55f0->data)[actor_index & 0xffff].squad_index;
					break;
				}
				}

				all = !any;
				for (i = 0; i < trigger->condition_count; i++)
				{
					s_ai_scene_condition *condition = &trigger->conditions[i];
					bool holds = function_290e20(condition->trigger_index, squad_index, squad_group_index);
					if (condition->flags & 1)
						holds = !holds;
					if (holds)
					{
						if (any)
							goto assign;
					}
					else if (!any)
					{
						goto done;
					}
				}
				if (!all)
					goto done;
			}

		assign:
			function_2919e0(scene, assignments, &assignment_count, 12, 0, (short)scene->role_count, ai_index, ai_index2, ai_index3);
			if (assignment_count > 0)
			{
				real best_score = 0.0f;
				short best_index = NONE;
				short j;

				for (j = 0; j < assignment_count; j++)
				{
					real score = 0.0f;
					short k;
					for (k = 0; k < scene->role_count; k++)
						score += assignments[j].scores[k];
					if (score > best_score)
					{
						best_score = score;
						best_index = j;
					}
				}

				if (best_index != NONE)
				{
					s_ai_scene_assignment *assignment = &assignments[best_index];
					short k;

					for (k = 0; k < scene->role_count; k++)
					{
						s_ai_scene_actor *actor = &((s_ai_scene_actor *)g_4f55f0->data)[assignment->actor_indices[k] & 0xffff];
						s_ai_scene_unit *unit = (s_ai_scene_unit *)object_get(actor->unit_index);
						short variant_index = assignment->variant_indices[k];
						long local_dbe893 = assignment->variant_names[k];
						if (((s_ai_scene_unit_variant *)((byte *)unit + unit->variant_offset))->local_dbe893 != local_dbe893 &&
							local_dbe893 && variant_index != NONE)
						{
							s_ai_scene_model *model = (s_ai_scene_model *)g_4e3b44[*(long *)(g_4e3b44[unit->definition_index & 0xffff].bytes + 0x38) & 0xffff].bytes;
							if (variant_index >= 0 && variant_index < model->variant_count)
							{
								s_ai_scene_unit *written_unit = (s_ai_scene_unit *)object_get(actor->unit_index);
								s_ai_scene_unit_variant *variant = (s_ai_scene_unit_variant *)((byte *)written_unit + written_unit->variant_offset);
								variant->name = model->variants[variant_index].name;
								variant->local_dbe893 = local_dbe893;
							}
						}
					}

					bool started = function_2580c0(scene_index, command_script_index, assignment->actor_indices, (short)scene->role_count);
					result = started;
					if (started)
						((dword *)g_5044d0)[scene_index >> 5] |= 1 << (scene_index & 0x1f);
				}
			}
			goto done;
		}
	}

	done:
	return result;
}

struct s_player_291670
{
	byte unknown00[0x2c];
	long object_index;
};

struct s_object_291670
{
	byte unknown00[0x30];
	point3f position;
};

struct s_object_header_291670
{
	byte unknown00[8];
	s_object_291670 *object;
};

bool function_11c470(long trigger_volume_index, point3f const *point);

/* tests the player units against a trigger volume */
// @retail 0x291670
bool function_291670(short trigger_volume_index, bool all_players)
{
	bool result = false;
	s_record_pool_iterator iterator;
	iterator.data = g_4e8c24;
	iterator.index = NONE;
	s_player_291670 *player;
	while ((player = (s_player_291670 *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		bool inside = false;
		if (player->object_index != NONE)
		{
			if (function_11c470(trigger_volume_index, &((s_object_header_291670 *)g_4e0300->data)[player->object_index & 0xffff].object->position))
				inside = true;
		}
		if (all_players)
			result &= inside;
		else if (inside)
		{
			result = inside;
			goto local_0;
		}
	}
local_0:
	return result;
}

struct s_290f60
{
	short field_0;
	short field_2;
	short field_4;
	short field_6;
	real field_8;
	short field_c;
	byte field_e[0x22];
	short field_30;
	short field_32;
	byte field_34;
};

struct s_290f61
{
	byte field_0[2];
	short field_2;
	byte field_4[6];
	short field_a;
	byte field_c[4];
	real field_10;
	short field_14;
	short field_16;
	byte field_18[0xe];
	signed char field_26;
	byte field_27[5];
	long field_2c;
};

bool function_203240(long arg_0);
bool function_2032b0(long arg_0);
long function_209490(long arg_0);
bool function_2765e0(long arg_0, real arg_1);
bool function_226190(void);
long function_1469f0(real arg_0);

// @retail 0x290f60
bool function_290f60(s_ai_trigger_condition *arg_0, bool *arg_1, long arg_2)
{
	s_290f60 *local_0 = (s_290f60 *)arg_0;
	bool local_1 = false;
	if (local_0->field_2 != NONE)
		arg_2 = local_0->field_2;
	else if (local_0->field_4 != NONE)
		return function_2912c0(arg_0, NONE, arg_1);
	s_290f61 *local_2 = arg_2 != NONE ? (s_290f61 *)(g_51e9d8->data + (arg_2 & 0xffff) * 0x98) : NULL;
	switch (local_0->field_0)
	{
	case 0: case 1: case 2: case 3: case 4: case 6:
	case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18:
		if (!local_2 || !(local_2->field_2 & 4))
			goto local_6;
		break;
	}
	switch (local_0->field_0)
	{
	case 0:
		if (local_2 && local_2->field_a >= local_0->field_6)
			local_1 = true;
		else
			local_1 = false;
		break;
	case 1:
		if (local_2 && local_2->field_a <= local_0->field_6)
			local_1 = true;
		else
			local_1 = false;
		break;
	case 2:
		if (local_2 && local_2->field_10 >= local_0->field_8)
			local_1 = true;
		else
			local_1 = false;
		break;
	case 3:
		if (local_2 && local_2->field_10 <= local_0->field_8)
			local_1 = true;
		else
			local_1 = false;
		break;
	case 4:
		if (local_2 && (local_2->field_2 & 0x10))
			local_1 = true;
		else
			local_1 = false;
		break;
	case 5:
		if (local_2 && g_510c54->game_time > local_2->field_2c + local_0->field_6)
			local_1 = true;
		else
			local_1 = false;
		break;
	case 6:
		break;
	case 7:
		if (local_2 && local_0->field_30 != NONE)
		{
			s_291769 *local_3 = &((s_291746 *)g_4e0350)->field_1bc[local_0->field_30];
			g_50242c = arg_2 & 0xffff;
			long local_4 = function_209490(local_3->field_24);
			local_1 = *(bool *)&local_4;
		}
		else
			local_1 = false;
		break;
	case 8:
		if (local_2 && local_0->field_30 != NONE)
		{
			s_291769 *local_3 = &((s_291746 *)g_4e0350)->field_1bc[local_0->field_30];
			g_50242c = arg_2 & 0xffff;
			long local_4 = function_209490(local_3->field_24);
			local_1 = !*(bool *)&local_4;
		}
		else
			local_1 = false;
		break;
	case 9:
		if (local_0->field_c != NONE)
			local_1 = function_291670(local_0->field_c, false);
		else
			*arg_1 = true;
		break;
	case 10:
		if (local_0->field_c != NONE)
			local_1 = function_291670(local_0->field_c, true);
		else
			*arg_1 = true;
		break;
	case 11:
		if (local_2)
			local_1 = local_2->field_26 >= local_0->field_6;
		break;
	case 12:
		if (local_2)
			local_1 = local_2->field_26 <= local_0->field_6;
		break;
	case 13:
		if (arg_2 != NONE)
			local_1 = function_203240(arg_2);
		break;
	case 14:
		if (arg_2 != NONE)
			local_1 = function_2032b0(arg_2);
		break;
	case 15:
		if (local_2)
			local_1 = (local_2->field_2 & 8) != 0;
		break;
	case 16:
		if (local_2)
			local_1 = local_2->field_16 >= local_0->field_6;
		break;
	case 17:
		if (local_2)
			local_1 = local_2->field_16 <= local_0->field_6;
		break;
	case 18:
		if (local_2)
			local_1 = function_2765e0(arg_2 & 0xffff, local_0->field_8);
		break;
	case 19:
	{
		long local_5;
		if (g_4f55d0->unknown36c == NONE || (local_5 = g_510c54->game_time, local_5 - g_4f55d0->unknown36c > function_1469f0(local_0->field_8)))
			local_1 = true;
		else
			local_1 = false;
		break;
	}
	case 20:
		local_1 = function_226190();
		break;
	default:
		local_1 = false;
		break;
	}
	if (local_0->field_34 & 1)
		local_1 = !local_1;
local_6:
	return local_1;
}

struct s_2912c0
{
	byte field_0[0x18];
	long field_18;
	byte field_1c[7];
	bool field_23;
	short field_24;
	byte field_26[2];
	bool field_28;
	byte field_29[3];
	short field_2c;
	byte field_2e[2];
	real field_30;
	short field_34;
	byte field_36[2];
};

// @retail 0x2912c0
bool __stdcall function_2912c0(s_ai_trigger_condition *arg_0, long arg_1, bool *arg_2)
{
	bool local_0 = false;
	s_ai_trigger_condition *const *local_8 = &arg_0;
	long const *local_9 = &arg_1;
	bool *const *local_10 = &arg_2;
	s_290f60 *local_1 = (s_290f60 *)*local_8;
	if (local_1->field_2 != NONE)
		return function_290f60(*local_8, *local_10, NONE);
	long local_2 = local_1->field_4 != NONE ? local_1->field_4 : *local_9;
	s_2912c0 *local_3 = (s_2912c0 *)squad_group_get(local_2);
	switch (local_1->field_0)
	{
	case 0: case 1: case 2: case 3: case 4: case 6:
	case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18:
		if (!local_3 || !local_3->field_23)
			goto local_11;
		break;
	}
	s_squad_group_iterator local_4;
	switch (local_1->field_0)
	{
	case 0: local_0 = local_3->field_24 >= local_1->field_6; break;
	case 1: local_0 = local_3->field_24 <= local_1->field_6; break;
	case 2: local_0 = local_3->field_30 >= local_1->field_8; break;
	case 3: local_0 = local_3->field_30 <= local_1->field_8; break;
	case 4: local_0 = local_3->field_28; break;
	case 5: local_0 = g_510c54->game_time > local_3->field_18 + local_1->field_6; break;
	case 6: break;
	case 7:
		if (local_1->field_30 != NONE)
		{
			s_291769 *local_12 = &((s_291746 *)g_4e0350)->field_1bc[local_1->field_30];
			g_50242c = (local_2 & 0xffff) | 0x40000000;
			long local_5 = function_209490(local_12->field_24);
			local_0 = *(bool *)&local_5;
		}
		else
			local_0 = false;
		break;
	case 8:
		if (local_1->field_30 != NONE)
		{
			s_291769 *local_12 = &((s_291746 *)g_4e0350)->field_1bc[local_1->field_30];
			g_50242c = (local_2 & 0xffff) | 0x40000000;
			long local_6 = function_209490(local_12->field_24);
			local_0 = !*(bool *)&local_6;
		}
		else
			local_0 = false;
		break;
	case 9:
		if (local_1->field_c != NONE) local_0 = function_291670(local_1->field_c, false);
		else **local_10 = true;
		break;
	case 10:
		if (local_1->field_c != NONE) local_0 = function_291670(local_1->field_c, true);
		else **local_10 = true;
		break;
	case 11: if (local_3) local_0 = local_3->field_34 >= local_1->field_6; break;
	case 12: if (local_3) local_0 = local_3->field_34 <= local_1->field_6; break;
	case 13:
		if (local_2 != NONE)
		{
			function_204db0(&local_4, local_2);
			local_0 = true;
			while (local_0 && function_204e10(&local_4))
				local_0 &= function_203240(local_4.squad_index);
		}
		break;
	case 14:
		if (local_2 != NONE)
		{
			function_204db0(&local_4, local_2);
			local_0 = true;
			while (local_0 && function_204e10(&local_4))
				local_0 &= function_2032b0(local_4.squad_index);
		}
		break;
	case 15:
		if (local_2 != NONE)
		{
			function_204db0(&local_4, local_2);
			local_0 = false;
			s_squad_datum *local_7;
			while ((local_7 = function_204e10(&local_4)) != NULL)
				if (*((byte *)local_7 + 2) & 8) { local_0 = true; break; }
		}
		break;
	case 16: if (local_3) local_0 = local_3->field_2c >= local_1->field_6; break;
	case 17: if (local_3) local_0 = local_3->field_2c <= local_1->field_6; break;
	case 18:
		if (local_3) local_0 = function_2765e0((local_2 & 0xffff) | 0x40000000, local_1->field_8);
		break;
	case 19:
	{
		long local_13;
		local_0 = g_4f55d0->unknown36c == NONE || (local_13 = g_510c54->game_time, local_13 - g_4f55d0->unknown36c > function_1469f0(local_1->field_8));
		break;
	}
	case 20: local_0 = function_226190(); break;
	default: local_0 = false; break;
	}
	if (local_1->field_34 & 1) local_0 = !local_0;
local_11:
	return local_0;
}
