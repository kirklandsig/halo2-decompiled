// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_290E20.CPP: the scenario's named ai triggers (0x30 bytes each at
   +0x248 of g_4e0350): a lookup by name and the test whether enough of a
   trigger's conditions hold for a squad or squad group (outside functions
   lane A's ai script query 0x275d10 needs) */

#include "unknown_11c920.h"
#include "globals.h"
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

bool __stdcall function_2912c0(s_ai_trigger_condition *condition, long squad_group_index, bool *result);
bool function_290f60(s_ai_trigger_condition *condition, bool *result, long squad_index);

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

// @retail 0x290e20
bool function_290e20(short trigger_index, long squad_index, long squad_group_index)
{
	s_ai_trigger *trigger = &((s_scenario_ai_triggers_view *)g_4e0350)->triggers[trigger_index];
	short count;
	short needed;
	short i;
	bool result;

	if ((trigger->flags & 1) && (((dword *)g_5044cc)[trigger_index >> 5] & (1 << (trigger_index & 0x1f))))
		return true;

	count = 0;
	result = false;
	if (trigger->combine_mode == 0)
		needed = 1;
	else
		needed = (short)trigger->condition_count;

	for (i = 0; i < trigger->condition_count; i++)
	{
		s_ai_trigger_condition *condition = &trigger->conditions[i];
		bool holds;

		if (squad_group_index != NONE && function_2912c0(condition, squad_group_index, &result))
			holds = true;
		else if (squad_index != NONE)
			holds = function_290f60(condition, &result, squad_index);
		else if (squad_group_index == NONE)
			holds = function_290f60(condition, &result, NONE);
		else
			holds = false;

		if (holds)
		{
			count++;
			if (count >= needed)
				break;
		}
	}

	if (count < needed)
		return false;
	if (trigger->flags & 1)
		((dword *)g_5044cc)[trigger_index >> 5] |= 1 << (trigger_index & 0x1f);
	return true;
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

struct s_ai_scene_unit_header
{
	byte unknown00[8];
	s_ai_scene_unit *unit;
};

/* the scenes that have started once (ai.cpp) */
extern void *g_5044d0;

long function_272b70(long ai_index);
bool function_2580c0(short squad_index, short script_index, long *actor_indices, short count);
void function_2919e0(s_ai_scene *scene, s_ai_scene_assignment *assignments, short *assignment_count, short maximum_count,
	short role_index, short role_count, long ai_index, long ai_index2, long ai_index3);

/* starts a scene (by name) with the actors the ai indices name when its
   trigger conditions hold */
// @retail 0x291b40
bool function_291b40(long name, short command_script_index, long ai_index, long ai_index2, long ai_index3)
{
	volatile bool result = false;
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
				return result;

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
						return result;
					}
				}
				if (!all)
					return result;
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
						s_ai_scene_unit *unit = ((s_ai_scene_unit_header *)g_4e0300->data)[actor->unit_index & 0xffff].unit;
						short variant_index = assignment->variant_indices[k];
						long local_dbe893 = assignment->variant_names[k];
						if (((s_ai_scene_unit_variant *)((byte *)unit + unit->variant_offset))->local_dbe893 != local_dbe893 &&
							local_dbe893 && variant_index != NONE)
						{
							s_ai_scene_model *model = (s_ai_scene_model *)g_4e3b44[*(long *)(g_4e3b44[unit->definition_index & 0xffff].bytes + 0x38) & 0xffff].bytes;
							if (variant_index >= 0 && variant_index < model->variant_count)
							{
								s_ai_scene_unit_variant *variant = (s_ai_scene_unit_variant *)((byte *)unit + unit->variant_offset);
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
			return result;
		}
	}

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
			result = true;
			break;
		}
	}
	return result;
}
