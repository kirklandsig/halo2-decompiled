// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_257D00.CPP: the ai's command scripts: the "command scripts"
   (g_502408) an actor runs, chained from the actor's +0x858, and the "joint
   command scripts" (g_502404) that run one script on several actors */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "slot_handler.h"
static __forceinline long real_to_long(real value);
#include "unknown_2551c0.h"
#include "swarm.h"
#include <string.h>

/* a command script (0xd4 bytes) */
struct s_cs_datum
{
	short salt;
	byte unknown02[2];
	short type;
	byte unknown06[2];
	real unknown08;
	byte unknown0c[0x28 - 0xc];
	long unknown28;
	long unknown2c;
	short state;
	byte unknown32[2];
	long script_index;
	long thread_index;
	long next_index;
	long joint_index;
	bool unknown44;
	bool unknown45;
	bool unknown46;
	byte unknown47;
	short unknown48;
	byte unknown4a[2];
	long unknown4c;
	byte unknown50;
	bool unknown51;
	bool unknown52;
	byte unknown53;
	short unknown54;
	byte unknown56[2];
	long unknown58;
	bool unknown5c;
	byte unknown5d[3];
	real unknown60;
	bool unknown64;
	byte unknown65[7];
	bool unknown6c;
	byte unknown6d[7];
	bool unknown74;
	bool unknown75;
	byte unknown76;
	bool unknown77;
	bool unknown78;
	bool unknown79;
	bool unknown7a;
	byte unknown7b;
	short unknown7c;
	bool unknown7e;
	bool unknown7f;
	bool unknown80;
	bool unknown81;
	bool unknown82;
	bool unknown83;
	short unknown84;
	bool unknown86;
	byte unknown87[5];
	bool unknown8c;
	byte unknown8d[3];
	long unknown90;
	long unknown94;
	byte unknown98;
	bool unknown99;
	short unknown9a;
	byte unknown9c[0xac - 0x9c];
	bool unknownac;
	byte unknownad[3];
	long unknownb0;
	byte unknownb4[0xd1 - 0xb4];
	bool unknownd1;
	byte unknownd2[2];
};

/* a participant of a joint command script */
struct s_joint_cs_participant
{
	long actor_index;
	long cs_index;
	short unknown8;
	byte unknowna[2];
};

/* a joint command script (0x8c bytes) */
struct s_joint_cs_datum
{
	short salt;
	byte unknown02[2];
	long script_index;
	short unknown08;
	short participant_count;
	long thread_index;
	s_joint_cs_participant participants[10];
	short leader;
	bool unknown8a;
	byte unknown8b;
};

/* the actor fields the command scripts keep */
struct s_actor_cs_view
{
	byte unknown000[0x1c4];
	struct
	{
		short type;
		byte unknown2[2];
		short unknown4;
		short unknown6;
		short timer;
		short unknowna;
	} entries[3];
	byte unknown1e8[0x3b8 - 0x1e8];
	bool unknown3b8;
	byte unknown3b9[0x858 - 0x3b9];
	long first_cs_index;
	long current_cs_index;
	byte unknown860[0x888 - 0x860];
};

/* the scenario's command script point sets (g_4e0350 +0x1d8) */
struct s_cs_point_set
{
	byte unknown00[0x20];
	long point_count;
	struct s_cs_point *points;
	byte unknown28[0x30 - 0x28];
};

struct s_cs_scenario_data
{
	long point_set_count;
	s_cs_point_set *point_sets;
};

struct s_cs_scenario_view
{
	byte unknown000[0x1d8];
	long script_data_count;
	s_cs_scenario_data *script_data;
};

/* the state a command script keeps per participant (0x24 bytes) */
struct s_cs_state
{
	byte unknown0[3];
	byte flags3;
	short unknown4;
	byte unknown6[2];
	short unknown8;
	byte unknowna[0x24 - 0xa];
};

typedef short (__stdcall *cs_iterate_proc)(long actor_index, long object_index, s_cs_state *state, long cs_index);

/* the scenario's squads (g_4e0350 +0x174, 0x18 bytes each) as the joint
   command scripts read them */
struct s_cs_squad
{
	byte unknown00[4];
	struct
	{
		dword flag0 : 1;
		dword flag1 : 1;
		dword unknown : 30;
	} flags;
	byte unknown08[0x18 - 0x8];
};

struct s_cs_squad_scenario_view
{
	byte unknown000[0x174];
	s_cs_squad *squads;
};

/* a point of a command script point set (0x3c bytes) */
struct s_cs_point
{
	byte unknown00[0x20];
	s_type_c3b527 point;
	long unknown30;
	byte unknown34[0x3c - 0x34];
};

/* the actor fields function_259ec0 sets */
struct s_actor_cs_move_view
{
	byte unknown000[0x478];
	bool unknown478;
	byte unknown479[0x4d8 - 0x479];
	vector3f unknown4d8;
};

extern s_record_pool *g_502404;
extern s_record_pool *g_4f9384;

/* the ai index whose actor gets command scripts queued (unknown_272b70.cpp) */
long g_502428;

long function_272b70(long ai_index);
real function_30bf0(vector3f *v);
long function_257d80(long script_index, long thread_index);
long function_257e70(long script_index);
long function_257fa0(long actor_index, short script_index, long thread_index);
long function_258040(long actor_index, short script_index, long thread_index);

bool __stdcall function_1f4280(long actor_index);
long function_209520(short script_index);
void function_267770(long prop_index, long actor_index);

short function_258b60(long actor_index, cs_iterate_proc proc, long cs_index);
short __stdcall function_258cc0(long actor_index, long object_index, s_cs_state *state, long cs_index);
short __stdcall function_258cf0(long actor_index, long object_index, s_cs_state *state, long cs_index);
short __stdcall function_259430(long actor_index, long object_index, s_cs_state *state, long cs_index);
short __stdcall function_259d90(long actor_index, long object_index, s_cs_state *state, long cs_index);
void function_259e70(long cs_index);
void function_258540(long actor_index, long cs_index);
void function_258480(long joint_index, long actor_index);
void function_2583e0(long joint_index);

inline s_cs_datum *cs_get(long cs_index)
{
	return (s_cs_datum *)(g_502408->data + (cs_index & 0xffff) * sizeof(s_cs_datum));
}

inline s_joint_cs_datum *joint_cs_get(long joint_index)
{
	return (s_joint_cs_datum *)(g_502404->data + (joint_index & 0xffff) * sizeof(s_joint_cs_datum));
}

inline s_actor_cs_view *actor_cs_get(long actor_index)
{
	return (s_actor_cs_view *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_actor_cs_view));
}

// @retail 0x257d00
void function_257d00(void)
{
	g_502408 = data_new_inlined("command scripts", 0x28, sizeof(s_cs_datum), 0, g_510c2c);
	g_502404 = data_new_inlined("joint command scripts", 10, sizeof(s_joint_cs_datum), 0, g_510c2c);
}

// @retail 0x257d80
long function_257d80(long script_index, long thread_index)
{
	long cs_index = record_pool_allocate(g_502408);

	if (cs_index != NONE)
	{
		s_cs_datum *cs = cs_get(cs_index);

		cs->script_index = script_index;
		if (thread_index != NONE)
		{
			cs->thread_index = thread_index;
		}
		else
		{
			cs->thread_index = function_209520(script_index);
		}

		cs->unknown45 = false;
		cs->unknown46 = false;
		cs->unknown51 = false;
		cs->unknown52 = false;
		cs->unknown5c = false;
		cs->unknown64 = false;
		cs->unknown6c = false;
		cs->unknown74 = false;
		cs->unknown75 = false;
		cs->unknown7e = false;
		cs->unknown79 = false;
		cs->unknown7a = false;
		cs->unknown7f = false;
		cs->unknown80 = false;
		cs->unknown81 = false;
		cs->unknown82 = false;
		cs->unknown83 = false;
		cs->unknown84 = 0;
		cs->unknown86 = false;
		cs->unknown8c = false;
		cs->unknown99 = false;
		cs->unknown77 = false;
		cs->unknown78 = false;
		cs->state = 1;
		cs->joint_index = NONE;
		cs->next_index = NONE;
		cs->unknown60 = 0.f;
		cs->unknown7c = 10;
		cs->unknown90 = NONE;
		cs->unknown94 = NONE;
		cs->unknown9a = NONE;
		function_259e70(cs_index);
	}

	return cs_index;
}

// @retail 0x257e70
long function_257e70(long script_index)
{
	long joint_index = record_pool_allocate(g_502404);

	if (joint_index != NONE)
	{
		s_joint_cs_datum *joint = joint_cs_get(joint_index);

		joint->participant_count = 0;
		joint->script_index = script_index;
		joint->thread_index = function_209520(script_index);
		if (joint->thread_index == NONE)
		{
			record_pool_release(g_502404, joint_index);
			joint_index = NONE;
		}
	}

	return joint_index;
}

// @retail 0x257ed0
long function_257ed0(long actor_index, short script_index, long thread_index)
{
	s_actor_cs_view *actor = actor_cs_get(actor_index);

	if (g_502428 != NONE && actor_index == function_272b70(g_502428))
	{
		for (long cs_index = actor->first_cs_index; cs_index != NONE; )
		{
			s_cs_datum *cs = cs_get(cs_index);
			cs->unknown7e = true;
			cs_index = cs->next_index;
		}
		return function_258040(actor_index, script_index, thread_index);
	}

	while (actor->first_cs_index != NONE)
	{
		function_258540(actor_index, actor->first_cs_index);
	}

	long cs_index = function_257d80(script_index, thread_index);

	actor->first_cs_index = cs_index;
	actor->current_cs_index = cs_index;
	if (cs_index != NONE)
	{
		function_258b60(actor_index, function_258cc0, cs_index);
	}
	return cs_index;
}

// @retail 0x257fa0
long function_257fa0(long actor_index, short script_index, long thread_index)
{
	s_actor_cs_view *actor = actor_cs_get(actor_index);
	long cs_index = function_257d80(script_index, thread_index);

	if (cs_index != NONE)
	{
		s_cs_datum *cs = cs_get(cs_index);

		if (actor->first_cs_index != NONE)
		{
			s_cs_datum *first = cs_get(actor->first_cs_index);
			if (first->state == 0)
			{
				first->state = 3;
			}
		}

		cs->next_index = actor->first_cs_index;
		actor->first_cs_index = cs_index;
		actor->current_cs_index = cs_index;
		function_258b60(actor_index, function_258cc0, cs_index);
	}

	return cs_index;
}

// @retail 0x258040
long function_258040(long actor_index, short script_index, long thread_index)
{
	s_actor_cs_view *actor = actor_cs_get(actor_index);
	long cs_index = function_257d80(script_index, thread_index);

	if (cs_index != NONE)
	{
		long *link = &actor->first_cs_index;

		while (*link != NONE)
		{
			link = &cs_get(*link)->next_index;
		}
		*link = cs_index;

		if (actor->first_cs_index == cs_index)
		{
			actor->current_cs_index = cs_index;
		}
	}

	return cs_index;
}

// @retail 0x2580c0
bool function_2580c0(short squad_index, short script_index, long *actor_indices, short count)
{
	bool result;
	s_joint_cs_datum *joint;
	bool first;
	s_cs_squad *squad;
	long joint_index = function_257e70(script_index);

	if (joint_index != NONE)
	{
		joint = joint_cs_get(joint_index);
		first = true;
		squad = NULL;

		if (squad_index != NONE)
		{
			squad = &((s_cs_squad_scenario_view *)g_4e0350)->squads[squad_index];
		}

		joint->participant_count = count > 10 ? (short)10 : count;
		joint->unknown08 = squad_index;
		joint->unknown8a = false;
		for (short i = 0; i < joint->participant_count; i++)
		{
			long cs_index = function_257fa0(actor_indices[i], script_index, joint->thread_index);

			if (cs_index != NONE)
			{
				s_cs_datum *cs = cs_get(cs_index);

				cs->joint_index = joint_index;
				if (squad)
				{
					cs->unknown83 = !TEST_FIELD_BIT(squad->flags.flag1);
				}
				if (first)
				{
					first = false;
					joint->leader = i;
				}
				else
				{
					cs->unknown44 = true;
				}
				joint->participants[i].actor_index = actor_indices[i];
				joint->participants[i].cs_index = cs_index;
				joint->participants[i].unknown8 = i;
			}
			else
			{
				actor_indices[i] = NONE;
			}
		}

		result = joint->leader != NONE;

		if (!result)
		{
			function_2583e0(joint_index);
		}
	}
	else
	{
		result = false;
	}
	return result;
}

// @retail 0x258230
bool function_258230(long cs_index, short mode, long actor_index, long new_actor_index)
{
	s_cs_datum *new_cs;
	s_cs_datum *cs = cs_get(cs_index);
	long new_cs_index;
	long thread_index = cs->thread_index;

	switch (mode)
	{
	case 0:
		new_cs_index = function_257ed0(new_actor_index, (short)cs->script_index, thread_index);
		break;
	case 1:
		new_cs_index = function_257fa0(new_actor_index, (short)cs->script_index, thread_index);
		break;
	case 2:
		new_cs_index = function_258040(new_actor_index, (short)cs->script_index, thread_index);
		break;
	default:
		return false;
	}

	if (new_cs_index != NONE)
	{
		new_cs = cs_get(new_cs_index);

		if (cs->joint_index != NONE)
		{
			s_joint_cs_datum *joint = joint_cs_get(cs->joint_index);

			for (short i = 0; i < joint->participant_count; i++)
			{
				if (joint->participants[i].actor_index == actor_index)
				{
					joint->participants[i].actor_index = new_actor_index;
					joint->participants[i].cs_index = new_cs_index;
					break;
				}
			}
		}

		new_cs->state = 1;
		new_cs->unknown44 = cs->unknown44;
		cs->joint_index = NONE;
		cs->thread_index = NONE;
		cs->type = 0x17;
		return true;
	}
	return false;
}

PRIVATE __forceinline s_joint_cs_datum *function_258341(long arg_0)
{
	return (s_joint_cs_datum *)(g_502404->data + (arg_0 & 0xffff) * sizeof(s_joint_cs_datum));
}

// @retail 0x258340
bool function_258340(short participant_index, long joint_index)
{
	bool local_0 = true;
	long const *local_1 = &joint_index;
	s_joint_cs_datum *joint = function_258341(*local_1);

	if (participant_index < joint->participant_count)
	{
		long cs_index = joint->participants[joint->leader].cs_index;
		if (cs_index != NONE)
		{
			cs_get(cs_index)->unknown44 = true;
		}

		cs_index = joint->participants[participant_index].cs_index;
		if (cs_index != NONE)
		{
			cs_get(cs_index)->unknown44 = false;
			joint->leader = participant_index;
			goto local_2;
		}

		local_0 = false;
		function_2583e0(joint_index);
	}

local_2:
	return local_0;
}

// @retail 0x2583e0
void function_2583e0(long joint_index)
{
	s_joint_cs_datum *joint = joint_cs_get(joint_index);

	for (short i = 0; i < joint->participant_count; i++)
	{
		if (joint->participants[i].actor_index != NONE)
		{
			long cs_index = joint->participants[i].cs_index;
			s_cs_datum *cs = cs_get(cs_index);

			cs->thread_index = NONE;
			cs->joint_index = NONE;
			function_258540(joint->participants[i].actor_index, cs_index);
		}
	}

	if (joint->thread_index != NONE)
	{
		record_pool_release(g_4f9384, joint->thread_index);
	}
	record_pool_release(g_502404, joint_index);
}

// @retail 0x258480
void function_258480(long joint_index, long actor_index)
{
	long const *local_0 = &joint_index;
	s_joint_cs_datum *joint = joint_cs_get(*local_0);
	byte local_1 = 0;

	for (short i = 0; i < joint->participant_count; i++)
	{
		s_joint_cs_participant *participant = &joint->participants[i];

		if (participant->cs_index != NONE)
		{
			s_cs_datum *cs = cs_get(participant->cs_index);

			if (joint->participants[i].actor_index == actor_index)
			{
				cs->joint_index = NONE;
				joint->participants[i].actor_index = NONE;
				joint->participants[i].cs_index = NONE;
			}
			else if (joint->participants[i].actor_index != NONE)
			{
				local_1 = 1;
				cs->unknown7e = (bool)local_1;
			}
		}
	}

	if (!local_1)
	{
		if (joint->thread_index != NONE)
		{
			record_pool_release(g_4f9384, joint->thread_index);
		}
		record_pool_release(g_502404, joint_index);
	}
}

// @retail 0x258540
void function_258540(long actor_index, long cs_index)
{
	s_cs_datum *cs = cs_get(cs_index);
	s_actor_cs_view *actor = actor_cs_get(actor_index);

	if (cs->joint_index != NONE)
	{
		function_258480(cs->joint_index, actor_index);
	}
	else if (cs->thread_index != NONE)
	{
		record_pool_release(g_4f9384, cs->thread_index);
	}

	long *link = &actor->first_cs_index;
	while (*link != NONE)
	{
		if (*link == cs_index)
		{
			*link = cs->next_index;
			break;
		}
		link = &cs_get(*link)->next_index;
	}

	if (actor->current_cs_index == cs_index)
	{
		actor->current_cs_index = NONE;
	}
	record_pool_release(g_502408, cs_index);
}

// @retail 0x258600
void function_258600(long actor_index)
{
	long cs_index = actor_cs_get(actor_index)->first_cs_index;

	while (cs_index != NONE)
	{
		long next_index = cs_get(cs_index)->next_index;
		function_258540(actor_index, cs_index);
		cs_index = next_index;
	}
}

/* the actor's field the running command script publishes in g_50242c */
struct s_actor_cs_run_view
{
	byte unknown00[0x30];
	long unknown30;
};

inline s_actor_cs_run_view *actor_cs_run_get(long actor_index)
{
	return (s_actor_cs_run_view *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_actor_cs_view));
}

extern long g_50240c;
extern long g_502410;
long g_50242c;
short hs_thread_run_if_due(long thread_index); /* unknown_209520.cpp */

/* runs the command script's thread for the actor, again while it ends in
   type 0x16: 0 when it is done, 1 while it sleeps, 2 while it waits, 3 when
   it has no thread */
extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x258880
short function_258880(long actor_index, long cs_index)
{
	short result;
	s_cs_datum *cs = cs_get(cs_index);
	s_actor_cs_run_view *actor = actor_cs_run_get(actor_index);

	while (cs->thread_index != NONE)
	{
		if (cs->unknown44)
		{
			cs->state = 1;
			result = 2;
			goto local_9;
		}
		g_502428 = (actor_index & 0xffff) | 0x80000000;
		_ReadWriteBarrier();
		if (actor->unknown30 != NONE)
		{
			g_50242c = actor->unknown30 & 0xffff;
		}
		else
		{
			g_50242c = 0xc3e703e7;
		}
		g_50240c = actor_index;
		cs->type = NONE;
		cs->unknown28 = NONE;
		cs->unknown2c = NONE;
		g_502410 = cs_index;
		_ReadWriteBarrier();
		result = hs_thread_run_if_due(cs->thread_index);
		g_502428 = NONE;
		_ReadWriteBarrier();
		g_50240c = NONE;
		g_502410 = NONE;
		if (cs->unknown44)
		{
			cs->state = 1;
			result = 2;
			goto local_9;
		}
		if (cs->type != 0x16)
		{
			if (cs->type == 0x17)
			{
				cs->state = 2;
				result = 0;
			}
			else if (result == 2)
			{
				cs->state = 1;
			}
			else if (result == 0)
			{
				cs->state = 2;
			}
			else if (result == 1)
			{
				cs->state = 0;
			}
			goto local_9;
		}
		cs = cs_get(cs_index);
		actor = actor_cs_run_get(actor_index);
	}
	cs->state = 4;
	result = 3;
	goto local_9;
local_9:
	return result;
}
#pragma function(_ReadWriteBarrier)

/* the actor fields the command scripts' update reads */
struct s_actor_cs_update_view
{
	byte unknown000[0x86];
	short unknown086;
	byte unknown088[0x2d8 - 0x88];
	real unknown2d8;
	real unknown2dc;
	byte unknown2e0[0x480 - 0x2e0];
	bool unknown480;
	byte unknown481[0x858 - 0x481];
	long first_cs_index;
	long current_cs_index;
	byte unknown860[0x888 - 0x860];
};

inline s_actor_cs_update_view *actor_cs_update_get(long actor_index)
{
	return (s_actor_cs_update_view *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_actor_cs_update_view));
}

/* updates the actor's command scripts: runs the first until one blocks */
// @retail 0x258660
void function_258660(long actor_index)
{
	s_actor_cs_update_view *actor = actor_cs_update_get(actor_index);
	byte again = true;

	while (actor->first_cs_index != NONE && again)
	{
		long cs_index = actor->first_cs_index;
		s_cs_datum *cs;
		byte finish;

		again = false;
		cs = cs_get(cs_index);
		finish = false;
		if (cs->unknown79)
		{
			finish = actor->unknown086 >= 2;
		}
		if (!finish && cs->unknown7a)
		{
			finish = actor->unknown2d8 > 0.f || actor->unknown2dc > 0.f;
		}
		if (!finish && cs->unknown7c < 10)
		{
			finish = actor->unknown086 >= cs->unknown7c;
		}
		if (cs->unknown7e || finish)
		{
			cs->state = 2;
		}

		switch (cs->state)
		{
		case 4:
			function_258540(actor_index, cs_index);
			actor->current_cs_index = NONE;
			function_258b60(actor_index, function_258cc0, cs_index);
			again = true;
			break;
		case 2:
			if (cs->joint_index != NONE)
			{
				function_2583e0(cs->joint_index);
			}
			else
			{
				function_258540(actor_index, cs_index);
			}
			actor->current_cs_index = NONE;
			function_258b60(actor_index, function_258cc0, cs_index);
			again = true;
			break;
		case 1:
			{
				short result = function_258880(actor_index, cs_index);

				if (result == 1)
				{
					short next = function_258b60(actor_index, function_258cf0, cs_index);

					if (next == 0)
					{
						actor->current_cs_index = cs_index;
						break;
					}
					if (next == 2)
					{
						again = true;
						cs->state = 1;
					}
					else
					{
						again = true;
						cs->state = 2;
					}
				}
				else if (result == 0 || result == 3)
				{
					again = true;
				}
				if (!cs->unknown78)
				{
					actor->current_cs_index = NONE;
				}
			}
			break;
		case 0:
		case 3:
			actor->current_cs_index = cs_index;
			break;
		}
	}
	if (actor->first_cs_index != NONE)
	{
		s_cs_datum *cs = cs_get(actor->first_cs_index);

		if (!cs->unknown7f)
		{
			function_267770(NONE, actor_index);
		}
		if (cs->state == 1 && !cs->unknown81 && !cs->unknown78)
		{
			actor->unknown480 = true;
		}
	}
}

long g_502400;
long g_5023fc;

/* runs the command script through its states for the actor: 1 when it is
   done, 2 when it has no thread; 0 with *result 2 while it waits */
// @retail 0x258a00
short function_258a00(long actor_index, long unknown, long cs_index, short *result)
{
	short local_0 = 0;
	s_cs_datum *cs = cs_get(cs_index);

	g_502400 = (long)result;
	g_5023fc = unknown;
	if (cs->unknown7e)
	{
		cs->state = 2;
	}
	for (;;)
	{
		short status;

		if (cs->state == 0)
		{
			status = function_258b60(actor_index, function_259430, cs_index);
			if (status == 1)
			{
				function_258b60(actor_index, function_259d90, cs_index);
				cs->state = 1;
			}
			else if (status == 2)
			{
				cs->state = 1;
			}
			else
			{
				return status;
			}
		}
		else if (cs->state == 3)
		{
			cs->state = function_258b60(actor_index, function_258cf0, cs_index) == 2;
		}
		else if (cs->state == 1)
		{
			status = function_258880(actor_index, cs_index);
			if (status == 2)
			{
				*result = 2;
				local_0 = 0;
				goto local_1;
			}
			if (status == 0)
			{
				local_0 = 1;
				goto local_1;
			}
			if (status == 1)
			{
				if (function_258b60(actor_index, function_258cf0, cs_index) == 2)
				{
					cs->state = 1;
				}
			}
			else if (status == 3)
			{
				local_0 = 2;
				goto local_1;
			}
		}
		else
		{
			return (cs->state != 2) + 1;
		}
	}
local_1:
	return local_0;
}

/* a command script's facing (mode at +8, the direction at +0xc) */
struct s_cs_facing
{
	byte unknown00[8];
	short mode;
	byte unknown0a[2];
	vector3f direction;
};

/* the actor's unit forward at +0x290 */
struct s_actor_cs_facing_view
{
	byte unknown000[0x18];
	long unit_index;
	byte unknown01c[0x290 - 0x1c];
	vector3f forward;
	byte unknown29c[0x888 - 0x29c];
};

/* the object's up vector at +0x7c */
struct s_cs_facing_object
{
	byte unknown00[0x7c];
	vector3f up;
};

struct s_cs_facing_object_header
{
	byte unknown0[8];
	s_cs_facing_object *object;
};

void function_118e80(long object_index, vector3f *forward); /* unknown_118e80.cpp */

static inline void cs_cross_product3d(vector3f const *a, vector3f const *b, vector3f *result)
{
	result->i = a->j * b->k - a->k * b->j;
	result->j = a->k * b->i - a->i * b->k;
	result->k = a->i * b->j - a->j * b->i;
}

PRIVATE __forceinline void function_25a1b3(vector3f const *arg_0, vector3f *arg_1)
{
	arg_1->i = 0.f - arg_0->i;
	arg_1->j = 0.f - arg_0->j;
	arg_1->k = 0.f - arg_0->k;
}

/* sets the facing from the object's forward: along it (0), against it (1),
   or to its side (2, 3 the other side) */
PRIVATE __forceinline void function_25a131(vector3f const *arg_0, vector3f const *arg_1, vector3f *arg_2)
{
	vector3f const &first = *arg_0;
	arg_2->i = arg_1->k * first.j;
	arg_2->i -= first.k * arg_1->j;
	arg_2->j = arg_1->i * first.k;
	arg_2->j -= first.i * arg_1->k;
	arg_2->k = arg_1->j * first.i;
	arg_2->k -= first.j * arg_1->i;
}

// @retail 0x25a130
void function_25a130(long actor_index, long object_index, s_cs_facing *facing)
{
	s_actor_cs_facing_view *actor = (s_actor_cs_facing_view *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_actor_cs_facing_view));
	vector3f forward;

	if (object_index == actor->unit_index)
	{
		forward = actor->forward;
	}
	else
	{
		function_118e80(object_index, &forward);
	}

	short mode = facing->mode;

	switch (mode)
	{
	case 0:
		facing->direction = forward;
		break;
	case 1:
		function_25a1b3(&forward, &facing->direction);
		break;
	case 2:
	case 3:
		{
			vector3f side;

			function_25a131(g_4687b0, &forward, &side);
			if (function_30bf0(&side) == 0.f)
			{
				s_cs_facing_object *object = ((s_cs_facing_object_header *)g_4e0300->data)[object_index & 0xffff].object;

				cs_cross_product3d(&object->up, &forward, &side);
				if (function_30bf0(&side) == 0.f)
				{
					side = *g_4687a8;
				}
			}
			if (mode == 2)
			{
				facing->direction = side;
			}
			else
			{
				function_25a1b3(&side, &facing->direction);
			}
		}
		break;
	}
}

// @retail 0x258b20
void function_258b20(long index, long actor_index)
{
	s_cs_datum *cs = cs_get(index);

	function_258b60(actor_index, function_259d90, index);
	cs->state = 3;
}

// @retail 0x258cc0
short __stdcall function_258cc0(long actor_index, long object_index, s_cs_state *state, long cs_index)
{
	memset(state, 0, sizeof(s_cs_state));
	state->unknown4 = 0;
	return 0;
}

// @retail 0x259e70
void function_259e70(long cs_index)
{
	s_cs_datum *cs = cs_get(cs_index);

	if (cs->unknownac || cs->unknownb0 != NONE)
	{
		cs->unknownac = false;
		cs->unknownb0 = NONE;
		cs->unknownd1 = false;
		cs->unknown52 = false;
		cs->unknown46 = false;
	}
}

/* the point a command script reference names: the set in the high word, the
   point in the low word */
#define cs_point_get(reference) \
	(&((s_cs_scenario_view *)g_4e0350)->script_data->point_sets[((reference) >> 16) & 0xffff].points[(reference) & 0xffff])

/* the point set a command script reference's high word names */
#define cs_point_set_get(set_index) \
	(&((s_cs_scenario_view *)g_4e0350)->script_data->point_sets[(set_index)])

#pragma inline_depth(0)
// @retail 0x259ec0
bool function_259ec0(long actor_index, long cs_index)
{
	s_record_pool *local_1 = g_502408;
	s_record_pool *local_2 = g_4f55f0;
	s_actor_view *actor = (s_actor_view *)(local_2->data + (actor_index & 0xffff) * sizeof(s_actor_view));
	s_cs_datum *cs = (s_cs_datum *)(local_1->data + (cs_index & 0xffff) * sizeof(s_cs_datum));
	volatile bool result = true;
	s_cs_point *point = cs_point_get(cs->unknown28);

	if (actor->unknown018 == NONE)
	{
		return result;
	}

	bool exact = cs->type == 2 || cs->type == 16;

	((s_actor_cs_move_view *)actor)->unknown478 = cs->unknown75;
	switch (cs->type)
	{
	case 15:
	case 16:
	case 17:
		{
			bool local_0 = function_1f4460(actor_index, &point->point, NONE, actor->unknown018, exact);
			result = local_0;
			if (!local_0)
			{
				return result;
			}
		}
		actor->unknown4cc = cs->unknown08;
		break;
	case 1:
	case 2:
	case 3:
	case 19:
		if (point->unknown30 != NONE)
		{
			exact |= cs->unknown08 != 0.f;
			{
				bool local_0 = function_1f4460(actor_index, &point->point, point->unknown30, NONE, exact);
				result = local_0;
				if (!local_0)
				{
					return result;
				}
			}
			if (exact)
			{
				actor->unknown4b0 = cs->unknown08;
			}
		}
		else
		{
			result = false;
			return result;
		}
		break;

	}

	if (cs->type == 17 || cs->type == 3)
	{
		vector3f *facing = &((s_actor_cs_move_view *)actor)->unknown4d8;

		function_210be0(&point->point, &cs_point_get(cs->unknown2c)->point, facing);
		if (function_30bf0(facing) != 0.f)
		{
			actor->unknown4d5 = true;
		}
	}
	else if (cs->type == 2)
	{
		vector3f *facing = &((s_actor_cs_move_view *)actor)->unknown4d8;

		function_210be0(&point->point, &cs_point_get(cs->unknown2c)->point, facing);
		if (function_30bf0(facing) != 0.f)
		{
			actor->unknown4d4 = true;
		}
	}
	return result;
}
#pragma inline_depth(255)

// @retail 0x25aa10
void function_25aa10(long actor_index, long object_index)
{
	long cs_index = actor_cs_get(actor_index)->first_cs_index;

	while (cs_index != NONE)
	{
		s_cs_datum *cs = cs_get(cs_index);

		if (cs->unknown46 && cs->unknown48 == 1 && cs->unknown4c == object_index)
		{
			cs->unknown46 = false;
			cs->unknown4c = NONE;
		}
		if (cs->unknown52 && cs->unknown54 == 1 && cs->unknown58 == object_index)
		{
			cs->unknown52 = false;
			cs->unknown58 = NONE;
		}
		if (cs->unknownb0 == object_index)
		{
			function_259e70(cs_index);
		}
		cs_index = cs->next_index;
	}
}

// @retail 0x25aad0
bool function_25aad0(long actor_index, real *value)
{
	s_actor_view *actor = actor_get(actor_index);
	long cs_index = ((s_actor_cs_view *)actor)->first_cs_index;
	bool result = false;

	if (cs_index != NONE && !actor->unknown007)
	{
		s_cs_datum *cs = cs_get(cs_index);

		if ((cs->state == 0 || cs->state == 1) && cs->unknown60 > 0.f)
		{
			*value = cs->unknown60;
			result = true;
		}
	}
	return result;
}

// @retail 0x25ab50
bool function_25ab50(long reference)
{
	bool result = false;

	if (reference != NONE)
	{
		s_cs_scenario_view *scenario = (s_cs_scenario_view *)g_4e0350;

		if (scenario->script_data_count > 0)
		{
			long set_index = (reference >> 16) & 0xffff;

			if (set_index >= 0 && set_index < scenario->script_data->point_set_count)
			{
				long point_index = reference & 0xffff;

				if (point_index >= 0 && point_index < cs_point_set_get(set_index)->point_count)
					result = true;
			}
		}
	}
	return result;
}

real function_210b60(s_type_c3b527 const *arg_0, point3f const *arg_1);
real normalize2d(point2f *v);
bool function_1f50e0(long actor_index);
bool function_1f57f0(long actor_index, long animation, long const *target);
bool function_1f86f0(long index);
bool function_1f8720(long index);
long function_e70e0(long unit_index);
bool recorded_animation_playing(long object_index);
bool __stdcall function_1ffa30(s_type_c3b527 const *arg_0, long arg_4, long arg_1, long arg_2, bool arg_3);
struct s_slot_82;
void function_1c1080(s_slot_82 *state, long reference);
extern point2f *g_468778;

struct s_25a380
{
	byte field_0[0xb4];
	real field_b4;
	real field_b8;
	real field_bc;
	s_type_c3b527 field_c0;
};

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
void function_1caa40(long object_index, point3f *position);
bool function_1f8660(long index);
void function_1f86a0(long index);
void function_1e4290(long actor_index, bool value);
void function_26bfa0(long object_index, long *location_index, s_location_view *location);
bool function_1a8220(long index, short a, short b, long unknown, short c, short d, short e);
point3f *function_b9dd0(long object_index, point3f *result);

// @retail 0x25a380
void function_25a380(long arg_0)
{
	s_actor_view *local_0 = actor_get(arg_0);
	long local_1 = ((s_actor_cs_view *)local_0)->first_cs_index;
	if (local_1 == NONE)
	{
		return;
	}
	s_cs_datum *local_2 = cs_get(local_1);
	if (local_2->state != 0 && local_2->state != 1)
	{
		return;
	}
	if (local_2->unknown45)
	{
		local_0->unknown488 = true;
		local_0->unknown4a2 = true;
		local_0->unknown4a1 = false;
	}
	else if (local_2->unknown52)
	{
		if (!local_2->unknown51)
		{
			local_0->unknown4a2 = true;
			local_0->unknown4a1 = false;
		}
		else
		{
			local_0->unknown4a2 = false;
			local_0->unknown4a1 = true;
		}
	}
	else if (!local_2->unknown7f)
	{
		local_0->unknown4a2 = false;
		local_0->unknown4a1 = true;
	}
	if (local_2->unknown46)
	{
		bool target_set = false;
		switch (local_2->unknown48)
		{
		case 1:
			local_0->unknown430 = 2;
			local_0->unknown434 = 6;
			local_0->unknown438.object_index = local_2->unknown4c;
			target_set = true;
			break;
		case 2:
			if (function_25ab50(local_2->unknown4c))
			{
				point3f local_3;
				function_210850(&cs_point_get(local_2->unknown4c)->point, &local_3);
				local_0->unknown434 = 3;
				local_0->unknown438.point = local_3;
				local_0->unknown430 = 2;
				target_set = true;
			}
			break;
		}
		if (!target_set)
		{
			local_0->unknown438.vector = local_0->unknown290;
			local_0->unknown434 = 4;
			local_0->unknown430 = 2;
		}
		local_0->unknown444 = NONE;
	}
	else if (local_2->state == 1 && !local_2->unknown80 && !local_2->unknown52)
	{
		local_0->unknown438.vector = local_0->unknown290;
		local_0->unknown434 = 4;
		local_0->unknown430 = 2;
		local_0->unknown444 = NONE;
	}
	if (local_2->unknown52)
	{
		s_type_c3b527 local_4;
		bool local_5 = false;
		switch (local_2->unknown54)
		{
		case 2:
			if (function_25ab50(local_2->unknown58))
			{
				s_cs_point *local_6 = cs_point_get(local_2->unknown58);
				point3f local_7;
				function_210850(&local_6->point, &local_7);
				local_0->unknown41c = 3;
				local_0->unknown420 = 3;
				local_0->unknown424.point = local_7;
				local_4 = local_6->point;
				local_5 = true;
			}
			break;
		case 1:
			if (function_badc0(local_2->unknown58, (dword)NONE))
			{
				local_0->unknown41c = 3;
				local_0->unknown420 = 6;
				local_0->unknown424.object_index = local_2->unknown58;
				function_1caa40(local_2->unknown58, &local_4.point);
				local_4.output_index = NONE;
				local_5 = true;
			}
			break;
		}
		if (local_5)
		{
			if (local_2->unknown45)
			{
				local_0->unknown48c = true;
				local_0->unknown490_point = local_4;
				local_0->unknown4a0 = true;
				local_0->unknown489[2] = true;
			}
		}
		else
		{
			local_0->unknown41c = 3;
			local_0->unknown420 = 4;
			local_0->unknown424.vector = local_0->unknown290;
		}
		local_0->unknown44d = *(bool *)&local_2->unknown50;
	}
	else if (local_2->state == 1 && !local_2->unknown80)
	{
		local_0->unknown41c = 3;
		local_0->unknown420 = 4;
		local_0->unknown424.vector = local_0->unknown290;
	}
	else if (local_2->unknown7f)
	{
		if (local_2->unknown45)
		{
			local_0->unknown41c = 3;
			local_0->unknown420 = 2;
		}
		else
		{
			local_0->unknown41c = 2;
			local_0->unknown420 = 2;
		}
	}
	((s_actor_cs_move_view *)local_0)->unknown478 = local_2->unknown75;
	if (local_2->unknown78)
	{
		if (function_1f8660(arg_0) || (actor_get(arg_0)->unknown504 == 3 && !local_2->unknown76))
		{
			local_2->unknown78 = true;
		}
		else
		{
			local_2->unknown78 = false;
		}
	}
	if (local_2->state == 1 && !local_2->unknown81 && !local_2->unknown78)
	{
		local_0->unknown480 = true;
	}
	if (local_2->unknown74)
	{
		function_1e4290(arg_0, true);
	}
	if (!local_2->unknownac && local_2->unknownb0 != NONE)
	{
		s_25a380 *local_8 = (s_25a380 *)local_2;
		point3f local_9;
		vector3f local_10;
		function_b9dd0(local_2->unknownb0, &local_9);
		vector3d_from_points3d(&local_0->position, &local_9, &local_10);
		if (local_8->field_b4 > length_sq3f(&local_10))
		{
			if (function_1f8660(arg_0))
			{
				function_1f86a0(arg_0);
			}
			if (!local_2->unknownd1)
			{
				function_26c180(arg_0);
				local_8->field_c0 = local_0->unknown27c.point;
				local_2->unknownd1 = true;
			}
		}
		else
		{
			real local_11 = local_2->unknownd1 ? local_8->field_bc : local_8->field_b8;
			point3f local_12;
			vector3f local_13;
			function_210850(&local_8->field_c0, &local_12);
			vector3d_from_points3d(&local_12, &local_9, &local_13);
			if (!(length_sq3f(&local_13) > local_11) && local_0->unknown040)
			{
				long local_14;
				s_type_c3b527 local_15; /* 0x26bfa0 fills only the point */
				function_26bfa0(local_2->unknownb0, &local_14, (s_location_view *)&local_15);
				if (function_1f4460(arg_0, &local_15, local_14, local_2->unknownb0, false) && local_8->field_b4 > 0.f)
				{
					local_0->unknown4cc = (real)sqrt(local_8->field_b4) * 0.75f;
				}
			}
		}
		function_1a8220(arg_0, 3, 1, 3, 1, 0x2f, 3);
		local_0->unknown480 = false;
	}
	if (local_2->unknown83)
	{
		long local_17 = real_to_long(g_510c54->field_2_3 * 2.f);
		if ((short)local_17 > g_4f55d0->unknown22)
		{
			g_4f55d0->unknown22 = (short)local_17;
		}
	}
	if (local_2->unknown6c)
	{
		*(real *)&local_0->unknown478[4] = *(real *)&local_2->unknown6d[3];
	}
	else
	{
		*(real *)&local_0->unknown478[4] = 0.f;
	}
	if (local_2->unknown64)
	{
		local_0->unknown4b4 = *(real *)&local_2->unknown65[3];
	}
	else
	{
		local_0->unknown4b4 = 0.f;
	}
	if (local_2->unknown94 != NONE)
	{
		local_0->unknown450 = local_2->unknown94;
	}
	local_0->unknown449 = local_2->unknown5c;
	local_0->unknown44a = local_2->unknown5c;
}

struct s_258cf0
{
	short field_0;
	byte field_2;
	byte field_3;
	short field_4;
	byte field_6[2];
	short field_8;
	byte field_a[2];
	vector3f field_c;
	point3f field_18;
};

struct s_named_entry_owner;
short named_entry_find(s_named_entry_owner const *owner, char const *name);
void *function_1e5380(long actor_index);
short function_cccd0(long unit_index, short grenade_type, short delta);
bool function_1fb360(long unit_index, short recording_index, long flags);
void function_b7360(long object_index);

#pragma inline_depth(2)
// score 471 (was 471): (MiniMax E run 26) retail keeps the object index in EBP and the scaled actor base in EBX and reads the case word through EBX, ours keeps the index in ESI and the base in EDI, so the whole dispatch prologue and the case bodies are four bytes out; the forty-byte length deficit and the pool/actor/output ownership remain.
// Earlier (471): No allocated frame; actor/pool/output acquisition, dispatch layout and forty missing bytes remain.
// the body is 40 bytes short; retail loads the actor and cs-datum
// table globals before the actor index and keeps the actor base in EBX, ours loads the
// index first and keeps the actor base in ESI.
// Actor/output registers, dispatch layout and rounding homes still differ.
// Actor/output registers, dispatch layout and rounding homes still differ.
// Actor/output registers, dispatch layout and rounding homes still differ.
// No frame; actor/output entry carriers, dispatch layout and rounding homes remain.
// Actor/output registers, dispatch layout and rounding homes still differ.
// Actor/pool/output entry carriers and rounding homes still differ; selection case result retained.
// Earlier: Actor/output registers, dispatch layout and rounding homes still differ.
// Actor/output acquisition, dispatch layout and forty missing bytes remain.
// Earlier 482 (was 482): Actor/output registers, dispatch layout and rounding homes still differ; eight source trials restored.
// Earlier: actor/output and dispatch registers still differ; multi-use actor handle kept.
// actor/output registers, branch layout and rounding homes differ;
// direct entry guard 631 fast; notification ranges 642 fast; selection-result field 597 plain
// (only three lower, rejected); actor pointer name 618 fast; tail case order 639 fast;
// observed selector with a case scope 611 fast, same as the selection-result form.
// Native orientation/point/entry/movement results 641/623/622/642 fast; invalidity in
// the real default case 572 fast, 547 plain (entry full 604, unchanged fast 620).
// Entry 550; formal role names/native initial conversion/sine first 572 fast; shared motion angle 592 fast.
// Size optimization 775 fast; depth two 561 fast, 532 plain, 1808/1848 bytes.
// Command type short/long 530/533; actor object snapshot 482 plain (kept);
// single-result completion 588; shared entry failure 515 (fast; existing depth two).
// Native actor/command/both references 482; state and state+actor/command/both 473,
// rejected without an exact match; output direction reference 534. Canonical state type
// and canonical direct output 482/482 (companion 478/478). All ten forms restored.
// Native unsigned object bits 482; initial/command/location point memcpy 482/482/482;
// both table/all native point copies 482/482; native clear/set updates 482; movement outcomes 530.
// Copies and bit updates preserve the complete executable prefix; original body restored.
// Existing actor/command/both lookups 482 each; direct output uses alone and with
// each/both lookups 482 each; actor-object value begun at its producer 524.
// Four explicit pool/data orders 482 each; native pool headers in either order 482/482; headers plus native
// offsets in either data order 480/480. Two-point gains and match-only memory snapshots rejected; original
// body restored. Pool/actor/output acquisition and forty missing bytes remain.
// Velocity-first native dot loads 476 plain (kept); two-use motion angle 502; radius 491;
// dot/angle 502; angle/radius and all motion values 498; separate duration 493;
// all motion values and separate duration 507. Native dispatch/register gap remains.
// Native state flag fields point/orientation/moving/directed and paired/all groups 476 each;
// all flags with native selector field 476. Pool/actor/output schedule and forty-byte gap remain.
// Shared duration conversions, moving/direct/combined/point/moving+point/direct+point/all 493/482/491/482/491/480/493;
// two-use rate at duration producer 540. Two-point combined conversion gain rejected; original conversions restored.
// Native command scalar, command point, state point variant and command location
// point 476 each; scalar+command point, scalar+state point, both point destinations
// and all four native payloads 476 each.
// Notification, point-test and movement inline/inline-noinline declarations, separately and combined, 476 each;
// all restored. Notification contracts lose reduced matched controls; restored-context losses distinguished.
// Results within movement/selection/both/recording/movement+recording/selection+recording/all cases
// 526/466/519/474/515/468/518; native-width outcomes in all three cases 518. Selection-only short result
// 466 plain (kept), without a shared function exit. Validity initialized before selection 536; native selection
// outcome 466; both 536; selection outcome begun after dispatch 467. Entry carriers and rounding homes remain.
// Round 161: movement/selection/recording completion 533/528/529; paired movement-selection/movement-recording/selection-recording 524/523/533; all outcome completion 528; orientation branch store 525. All restored.
// Round 145: Native counted character table, promoted lookup result, repeated command index reads
// and both 466 each; corresponding actual-record table forms 466/466/466/466. Original type and
// body restored.
// Round 151: Shared success return with explicit default failure, empty arms plus complete
// orientation/motion/point payload masks 0-7, all 466/1812. No result carrier added; original returns and
// complete dispatch restored.
// Round 240: complementary query result bool/byte/short/long 520/520/469/525;
// with complete complementary locomotion at the same widths 520/520/470/525. All restored.
// Round 138: shared rounded-tick outputs moving-direct/moving-point/direct-point/all
// 468/468/468/466; shared all tick products 474; all products and outputs 470;
// moving-only/other-case products 466/466. Original scratch declarations restored;
// pool/actor/output acquisition, dispatch layout and conversion homes remain.
// Consistent actor-object field reads 530; one point-selector publication 472; both 532;
// shared object-update notification 466; field reads/notification 530, point/notification 472,
// all three 532; field reads with positive point-production admission 530. Original body restored.
// Complete core-group orders ACB/BAC/BCA/CAB/CBA 474/544/534/478/532;
// B/A with point case before/between/after the groups 522/546/549. All original case bodies restored.
// Round 223: shared initial truncation 509; complete direction assignment helper 477;
// both 509; named-entry inline declaration 471; with truncation/direction/both
// 509/477/509; separate named-entry noinline declaration 471. All restored.
// Cross-case same-object comparison result bool/byte/short/long 536/536/527/532;
// complete complementary result at the same widths 536/536/527/532. Added storage
// regresses acquisition and dispatch; original comparison observations restored.
// Round 226: Per-use actor pool/getter 501/501; command pool/getter 786/787; both direct/getters 798/793;
// mixed direct/getter acquisitions 798/793. All original acquisitions and body restored.
// Round 159: Nine/eleven action tables with range admission 656/695 and switch dispatch 646/690;
// notification membership table 534; membership with eleven/nine ranged action tables 772/738 and eleven
// switched actions 771. All restored.
// Round 253: success-first selection result short/long/byte/word 472/472/482/472;
// actual-default failure production at those widths 488/488/508/488.
// Original selection test and full executable body restored.
// Round 245: First admission predicate in positive bool/byte/short/long and
// complementary bool/byte/short/long carriers, 471 each. All restored; pool/actor/output
// entry allocation and forty-byte deficit remain.
// Round 175: inner selector groups DNAB/NABD/BAND/ANBD/BNAD/NBAD/ABDN/DBAN
// 470/470/470/471/471/470/471/470; A=0..4, B=5..8, N=9/10, D=default.
// One-count reductions rejected; original inner and outer dispatch restored.
// Round 174: coordinated input-settings/playback inline and noinline contracts,
// each boundary and all four paired forms, 471 each; original declarations restored.
// Pool/actor/output acquisition and the forty-byte deficit remain.
// Round 392: Shared angle producer/direct point publication/native word range-result stores, paired/all,
// then all with the word destination defaulted first: 504/473/501/505/501/503/503/504. Original producer and dispatch lifetimes restored.
// Round 247: complete terminal cases 9/13/9+13/20/9+20/13+20/all before the core
// fallthrough groups 524/525/517/520/510/540/513; all in reverse order 513. All regress;
// original case order, actor/output acquisition, rounding and complete body restored.
// Round 280: Isolated duration producer scope 471; delay default 482; independent end ticks 483; both 482;
// coherent scratch names 471; names with delay/end/both 482/483/482. Original conversion destinations,
// case graph and actor-object observation restored.
// Round 188: Coordinated state/location/both inline contracts 471 each; producer-local motion completion alone/with state/with location/with both 527 each; both with literal recording completion 527; isolated recording completion 471. State-inline forms lose 22 matched reduced controls. All declarations, result producers and full body restored.
// Consistent first/second command references and actor-action observations, subsets 1-7,
// 544/514/532/520/532/526/530; all before notification 509. All original observations and
// complete body restored; pool/actor/output ownership and the forty-byte gap remain.
// Empty-success leaf group before outer cases 0/4/7/8/10/5/9/13: 471 each, 1808/1848 bytes.
// All eight have the original masked instruction prefix through the first table; full original case order, return graph and callback restored.
// Round 322: Coupled movement-query publication with positive/negative success-or-fallback admission, bool/byte/short/long: 539/531/533/534 each. Original stores, returns, dispatch and body restored; entry actor/output carriers and length deficit remain.
// Typed-array actor/command/both acquisitions, original/early actor-object observation, then both acquisition
// orders: 471/471/471/470/470/470/477/478. Original complete executable source restored.
// Round 260: Bounded notification bitset with entry/admitted short/long selector: mask shifts
// 543/559/539/552, bit shifts 542/555/542/549. Original membership, selector observations, cases and
// full body restored.
// Round 145: retail reads the actor object field at each use (actor base EBX, arg_0 EBP, datum EDI, output
// ESI), lays the 17/15/16 group out before 3/2/1/19 and repeats the wait call's head on the 1/19 path, and
// tests the wait result with branches. Actor object read at each use 525 (1812 bytes: actor EBX, datum EDI
// as retail, arg_0 ESI); `if (local_4) return 0; return 2;` 527 (1800); `if (!local_4) return 2; return 0;`
// 527; both 541 (1796); the wait block as a forceinline helper called from the 1/19 and 15/16 paths in
// place of the goto 513 (1872: the call is repeated as retail, but each copy keeps its own tail).
// Round 327: Shared movement-query tail after dispatch, success-first bool/byte/short/long
// 470/470/493/492; failure-first 473/473/490/494. Original case flow and body restored; entry
// carriers and length gap remain.
// Round 329: Independent movement/direct/point conversion workspaces declared at callback
// scope, subsets 1-7 and all with coherent product/tick names, 471 each. Original
// declaration scopes, conversion operations and complete executable body restored.
// Round 331: Common case-thirteen notification with branch/conditional flag values and
// orientation/point result stores: 471/483/512/521/471/493/519/528. Original terminal cases, output
// producers and complete body restored; actor/pool/output allocation and length gap remain.
// @retail 0x258cf0
short __stdcall function_258cf0(long arg_0, long arg_1, s_cs_state *arg_2, long arg_3)
{
	s_actor_view *local_0 = ((s_actor_view *)(g_4f55f0->data + (arg_0 & 0xffff) * sizeof(s_actor_view)));
	s_cs_datum *local_1 = ((s_cs_datum *)(g_502408->data + (arg_3 & 0xffff) * sizeof(s_cs_datum)));
	s_258cf0 *local_2 = (s_258cf0 *)arg_2;
	if (((s_handler_actor_view *)local_0)->perception_index != NONE)
	{
		switch (local_1->type)
		{
		case 1: case 4: break;
		default: return 2;
		}
	}
	switch (local_1->type)
	{
	case 1: case 2: case 3: case 4: case 6: case 7: case 8: case 9:
	case 14: case 15: case 16: case 17: case 18: case 19: case 21:
		function_259e70(arg_3);
		break;
	}
	long actor_object = local_0->unknown018;
	switch (local_1->type)
	{
	case 0:
		local_2->field_0 = (short)local_1->unknown08;
		return 0;
	case 3:
		local_2->field_0 = 0;
	case 2:
		if (!function_25ab50(local_1->unknown2c))
		{
			return 2;
		}
	case 1:
	case 19:
		if (!function_25ab50(local_1->unknown28))
		{
			return 2;
		}
		if (actor_object == NONE)
		{
			*(s_type_c3b527 *)((byte *)local_2 + 8) = cs_point_get(local_1->unknown28)->point;
			local_2->field_3 |= 0x40;
			return 0;
		}
		goto local_3;
	case 17:
		if (!function_25ab50(local_1->unknown2c))
		{
			return 2;
		}
	case 15:
	case 16:
		if (!function_25ab50(local_1->unknown28))
		{
			return 2;
		}
	local_3:
		{
			bool local_4 = function_259ec0(arg_0, arg_3);
			if (local_4)
			{
				local_1->unknown77 = true;
			}
			else if (local_1->unknown76)
			{
				local_1->unknown77 = false;
				return 0;
			}
			return local_4 ? 0 : 2;
		}
	case 4:
		{
			function_b9dd0(arg_1, &local_2->field_18);
			real local_5 = *(real *)&local_1->unknown0c[0] * 0.01745329238474369f;
			local_2->field_c.k = 0.f;
			local_2->field_c.i = (real)cos(local_5);
			local_2->field_c.j = (real)sin(local_5);
			short local_6 = *(short *)local_1->unknown06;
			local_2->field_8 = local_6 >= 0 && local_6 <= 3 ? local_6 : NONE;
			if (arg_1 == actor_object)
			{
				function_1f86a0(arg_0);
			}
			local_2->field_3 = (local_2->field_3 & ~2) | 1;
			return 0;
		}
	case 7:
		{
			if (arg_1 == actor_object && local_0->unknown26c != NONE)
			{
				return 2;
			}
			local_2->field_3 = (local_2->field_3 & ~0x18) | 4;
			bool local_7;
			if (arg_1 == actor_object)
			{
				local_7 = local_0->unknown5d0 && *(short *)((byte *)local_0 + 0x5d6) == 0;
			}
			else
			{
				s_slot_object_view *local_8 = object_get(arg_1);
				local_7 = local_8->parent_index == NONE && dot3f(&local_8->velocity, &local_8->forward) > 2.f;
			}
			long local_9;
			if (local_7)
			{
				local_9 = 0;
			}
			else
			{
				real local_10 = g_510c54->field_2_3 * 0.33f;
				__asm
				{
					fld local_10
					fistp local_9
				}
			}
			local_2->field_8 = (short)local_9;
			real local_11 = g_510c54->field_2_3 * 2.f;
			__asm
			{
				fld local_11
				fistp local_9
			}
			local_2->field_0 = (short)local_9;
			local_2->field_c.i = (real)cos(local_1->unknown08 * 0.01745329238474369f) * *(real *)local_1->unknown0c;
			local_2->field_c.j = (real)sin(local_1->unknown08 * 0.01745329238474369f) * *(real *)local_1->unknown0c;
			return 0;
		}
	case 8:
		{
			if (arg_1 == actor_object && local_0->unknown26c != NONE)
			{
				return 2;
			}
			local_2->field_3 = (local_2->field_3 & ~8) | 0x14;
			local_2->field_8 = 0;
			local_2->field_c.i = local_1->unknown08;
			local_2->field_c.j = *(real *)local_1->unknown0c;
			real local_12 = g_510c54->field_2_3 * 2.f;
			long local_13;
			__asm
			{
				fld local_12
				fistp local_13
			}
			local_2->field_0 = (short)local_13;
			return 0;
		}
	case 10:
	{
		short selection_outcome = 2;
		switch (*(short *)local_1->unknown06)
		{
		case 0: local_1->unknown8c = true; local_1->unknown90 = 0x700002c; break;
		case 1: local_1->unknown8c = true; local_1->unknown90 = 0xe00002a; break;
		case 2: local_1->unknown8c = true; local_1->unknown90 = 0xd00002b; break;
		case 3: local_1->unknown8c = true; local_1->unknown90 = 0xa00002d; break;
		case 4: local_1->unknown8c = true; local_1->unknown90 = 0xb00002e; break;
		case 5: local_1->unknown8c = true; local_1->unknown90 = 0xa000010; break;
		case 6: local_1->unknown8c = true; local_1->unknown90 = 0x9000011; break;
		case 7: local_1->unknown8c = true; local_1->unknown90 = 0x9000012; break;
		case 8: local_1->unknown8c = true; local_1->unknown90 = 0xa000013; break;
		case 9: case 10: local_1->unknown90 = NONE; local_1->unknown8c = true; break;
		default: local_1->unknown8c = false; break;
		}
		if (local_1->unknown8c)
			selection_outcome = 0;
		return selection_outcome;
	}
	case 5:
		{
			byte *local_14 = (byte *)function_1e5380(arg_0);
			if (!local_14 || !function_25ab50(local_1->unknown28))
			{
				return 2;
			}
			s_cs_point *local_15 = cs_point_get(local_1->unknown28);
			function_cccd0(actor_object, *(short *)(local_14 + 4), 1);
			local_1->unknown99 = false;
			*(s_type_c3b527 *)local_1->unknown9c = local_15->point;
			local_1->unknown9a = 0;
			short local_16 = *(short *)local_1->unknown06;
			if (local_16 >= 0 && local_16 < 3)
			{
				local_1->unknown9a = local_16;
			}
			real local_17 = g_510c54->field_2_3 * 2.f;
			long local_18;
			__asm
			{
				fld local_17
				fistp local_18
			}
			local_2->field_0 = (short)local_18;
			return 0;
		}
	case 9:
		{
			long local_19 = local_1->unknown28;
			byte *local_20 = (byte *)g_4e0350;
			if (local_19 < 0 || local_19 >= *(long *)(local_20 + 0x198))
			{
				return 2;
			}
			short local_21 = named_entry_find((s_named_entry_owner *)local_20,
				*(char **)(local_20 + 0x19c) + local_19 * 0x28);
			if (local_21 == NONE)
			{
				return 2;
			}
			return function_1fb360(arg_1, local_21, 0) ? 0 : 2;
		}
	case 13:
		if (*(short *)local_1->unknown06 == 1)
		{
			*((byte *)object_get(arg_1) + 0x10a) |= 0x40;
			function_b7360(arg_1);
		}
		else
		{
			*((byte *)object_get(arg_1) + 0x10a) |= 0x20;
			function_b7360(arg_1);
		}
		return 0;
	case 20:
		function_26c180(arg_0);
		local_1->unknownd1 = false;
		((s_25a380 *)local_1)->field_c0 = local_0->unknown27c.point;
		local_1->unknownac = true;
		function_1f86a0(arg_0);
		return 0;
	case 6: return 0;
	case 12: return 0;
	case 14: return 0;
	case 18: return 0;
	case 21: return 0;
	}
	return 2;
}
#pragma inline_depth(255)

// @retail 0x25aba0
void function_25aba0(long actor_index)
{
	s_actor_cs_view *actor = actor_cs_get(actor_index);

	actor->unknown3b8 = false;
	short *local_0 = &actor->entries[0].timer;
	long local_1 = 3;
	do
	{
		if (local_0[-4] != NONE)
		{
			(*local_0)--;
			if (*local_0 <= 0)
			{
				local_0[-4] = NONE;
				local_0[-2] = NONE;
				local_0[-1] = NONE;
				local_0[1] = 0;
			}
		}
		local_0 += 6;
	}
	while (--local_1);
}

/* swarm_creature_iterator_next, inlined; this copy reads the next creature
   before it records the current one, which keeps the current index in ebx
   and the actor index in ebp as retail does (with the shared one,
   function_258b60 takes the actor index in ebx and its callers stop
   matching) */
__forceinline s_handler_object_view *cs_creature_iterator_next(s_ai_object_iterator *iterator)
{
	s_handler_object_view *object = NULL;

	if (iterator->next_index != NONE)
	{
		s_object_ai_data *creature;
		long next_index;

		object = handler_object_get(iterator->next_index);
		next_index = NONE;
		if (!object->flags134 && (creature = (s_object_ai_data *)((byte *)object + object->ai_offset)) != NULL)
		{
			next_index = creature->next_creature_index;
		}
		iterator->index = iterator->next_index;
		iterator->next_index = next_index;
	}

	return object;
}

/* runs a command script step on an actor, or on each creature of its
   perception; with creatures, the result is 0 unless at least 80% of them
   finished or failed, then 2 when more finished than failed, else 1 */
// score 21 (was 100): the loop keeps the next index in esi and the creature data in edi where retail swaps them,
// and ours steps the next index through edx where retail writes edi directly.
// @retail 0x258b60
short function_258b60(long actor_index, cs_iterate_proc proc, long cs_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 2;

	if (actor->unknown007)
	{
		long perception_index = ((s_handler_actor_view *)actor)->perception_index;
		if (perception_index != NONE)
		{
			short succeeded = 0;
			short failed = 0;
			short count = 0;
			s_ai_object_iterator iterator;
			s_handler_object_view *object;

			iterator.next_index = perception_get(perception_index)->object_index;
			while ((object = cs_creature_iterator_next(&iterator)) != NULL)
			{
				byte *data;
				if (!object->flags134 && (data = (byte *)object + object->ai_offset) != NULL)
				{
					short outcome = proc(actor_index, iterator.index, (s_cs_state *)(data + 0x28), cs_index);
					*(short *)(data + 0x2c) = outcome;
					if (outcome == 2)
					{
						succeeded++;
					}
					else if (outcome == 1)
					{
						failed++;
					}
					count++;
				}
			}

			if ((real)(failed + succeeded) >= (real)count * 0.8f)
			{
				result = (succeeded > failed) + 1;
			}
			else
			{
				result = 0;
			}
		}
	}
	else
	{
		result = proc(actor_index, actor->unknown018, (s_cs_state *)((byte *)actor + 0x864), cs_index);
		*(short *)((byte *)actor + 0x868) = result;
	}
	return result;
}

// @retail 0x259d90
short __stdcall function_259d90(long actor_index, long object_index, s_cs_state *state, long cs_index)
{
	s_actor_view *actor = actor_get(actor_index);

	switch (cs_get(cs_index)->type)
	{
	case 1:
	case 2:
	case 3:
	case 15:
	case 16:
	case 17:
		if (actor->unknown018 != NONE)
		{
			function_1f4280(actor_index);
			return 1;
		}
		state->flags3 &= ~0x40;
		break;
	case 4:
		state->flags3 &= ~1;
		state->unknown8 = NONE;
		break;
	case 7:
	case 8:
		state->flags3 &= ~4;
		state->unknown8 = 0;
		break;
	case 6:
	case 14:
	case 18:
		state->flags3 &= ~0x20;
		break;
	}
	return 1;
}

// score 469 (was 469): the frame agrees at 0x1c; retail computes the actor base in EBX
// and spills it at [esp+0x10] before the type dispatch, ours computes it in EDI just
// before its use and reuses EBX for the state pointer. The body is 2400 bytes against
// retail's 2396.
// Earlier: retail keeps the object index in EBX and pushes EBP and ESI before EDI, ours keeps the index in EDI and pushes EBP, ESI and EDI in the other order, so the prologue, the jump table and every case body differ; the actor/output acquisition and the length deficit remain.
// Earlier 478 (was 478): Frame 0x1c agrees; actor/state registers and first point home still differ; eight scope trials restored.
// Earlier: frame 28 agrees; actor/state registers and first point home still differ.
// Short completion 531; iterator name 505; first point name 505; short countdown snapshots 505;
// deferred completion else block 505 (unchanged reduced preview 505).
// actor/state registers and the first point home differ;
// signed completion 502; direct point result 628; captured unit 503; direct state access 502; inline depth one 502; staged alignment 503; staged squared distance 502.
// Unsigned result 502; completion-test result 712; native unit predicate 490; iterator initializer 490;
// first/facing countdown values 636/621; planar memcpy 502; facing vector value 483;
// unit plus iterator 490; facing plus unit 486; facing plus iterator 486.
// Actor-position cursor 478; actor-facing cursor 574; facing-output cursor 573; state-flag cursor 510;
// actor handle 478; position/facing 574; position/flags 510; combined subobjects 597.
// Native actor/command/state record references, their pairs and all three 478 each;
// actor/command/state pointer role names 478. All eight restored. Canonical state type
// and canonical direct output in both callbacks 478/478, restored.
// Native actor scalar/vector/both members 484/478/484; shared short/native command selector 583/552;
// post-dispatch result 646; shared length producer 478 (grouping diagnostic restored); complete facing mask 609.
// Uninitialized first point, case-four point/vector, both groups, all spatial buffers,
// planar buffers, first plus planar, iterator and all workspaces at entry: 478 each.
// Complete facing CRT copy, twice-used selected target and consistent vector-before-enable publication: seven
// separate/composite forms 478 each; constant-first distance admission 478. Original body restored;
// actor/state registers and first point home remain.
// Position/distance/motion/alternate-motion helper inline and noinline declarations 478 each.
// Shared point/displacement/both workspaces 479/478/479; point or displacement
// with shared planar direction 479/478, all three 479; case-twenty point or
// displacement sharing 478/478. Opening workspaces and every call restored.
// Round 377: Complete pending-result bool/byte/short/long 603/603/637/632;
// entry-initialized pending results 605/605/636/631 (fast control 478). Boolean
// forms reach retail length but preserve actor/state reversal and regress.
// All result producers, iterator, facing value and original update graph restored.
// Single-bit equality tests across dispatch/clear/tail, separately and combined,
// and one shared case flag observation: eight forms all 478. Original body restored.
// Round 237: native state bit updates 488; actor/command/both at uses 1149/940/1417;
// actor-state/command-state/all receivers 1149/940/1417; both getters with bit updates 1438
// (fast control 491; full 478). Actor/state and first-point homes remain; all restored.
// Round 240: Internal turning-result byte/short/long 491/491/491; facing-completion
// byte/short/long 491/583/584; both short/long 583/584 (fast control 491, full 478).
// Actor/state ownership and first point home persist; original predicates restored.
// Round 175: consistent case-disjoint position/displacement/planar-direction names,
// seven family subsets and all with iterator/completion/result names, 491 each
// (unchanged reduced control 491; full 478). Original lexical identities restored.
// Round 31: conditional failure producers for spatial/countdown/both admissions 491/604/604;
// scalar-predicate admissions 631; spatial+scalar 631; countdown+scalar 611; all 611; all with Boolean completion 611.
// Reduced control 491 versus full 478; original default, all decision producers and complete body restored.
// Round 174: coordinated animation/state-query inline and noinline contracts,
// each boundary and four paired forms, 478 each; all provider declarations restored.
// Actor/state ownership and first point home remain.
// Round 259: separate inline/noinline declarations for the dispatch, facing, animation
// and notification callees 491/491/491/491/491/491/491/491 (fast control 491, full 478).
// All declarations and every call restored; actor/state and first point homes remain.
// Round 262: reference-check/state-publication/secondary-motion/request-admission
// inline and noinline declarations 491/491/491/491/491/491/491/491 (fast control 491; full 478).
// Every declaration and complete callback body restored; actor/state and point homes remain.
// Round 280: Entry-produced return result short/long with zero/one defaults 491 each; byte/bool with
// zero/one defaults 642 each (fast control 491, full 478). Original return storage, completion byte and
// actor/state acquisitions restored.
// Round 218: Nearest/candidate/distance scalar role names, seven subsets, 491 each; all three
// uninitialized at entry 491 (reduced control 491, full 478). Actor/state ownership and first point
// home remain; original scalar lifetimes restored.
// Round 312: first/second/both request receivers at uses 489/491/489; physical selector alone/first/second/both
// 491/489/491/489; both receivers and selector with the two-stage native flag value 491 (reduced control 491,
// full 478). Two-point reduced gains rejected; every receiver, physical declaration and original body restored.
// Round 322: Shared vector assignment at turning/state/current publication subsets 1-7: 566/586/624/618/632/639/639; all plus facing initialization 618 (fast control 491). All original vector copies, helper boundaries, case order and body restored; actor/state ownership remains.
// Round 238: complementary dispatch-ready/same-unit/facing-incomplete decisions, seven
// subsets 491/491/491/493/493/493/493; all as long 583 (fast control 491; full 478).
// Original internal decisions, byte completion, iterator and every vector/call restored.
// Complete actor-object/motion/position observations, seven subsets and case-owned position with both other
// values: 629/624/645/617/637/667/674/653. Original complete executable source restored.
// Round 469: Facing before pure displacement/all three planar aggregates/iterator object
// field uniformly at uses in seven subsets, then all with facing aggregate: 492/620/621/491/492/620/621/513.
// Isolated turning/animation/motion constructors and pairs, then turning/all with facing
// aggregate: 591/506/506/612/600/514/512/513. Turning construction restores actor/state ownership and point +0x10
// but regresses; constant-first distance/common result exit on turning/all graphs: 591/629/620/616.
// Original complete body restored; actor/state ownership and first-point home remain.
// Round 327: Complete pending-request arm orders 6/18/14/21, 6/21/18/14, 6/14/18/21, 18/6/21/14,
// 18/21/6/14, 21/6/18/14, 14/6/18/21, 14/21/18/6 give 471/474/469/480/476/480/472/480. Order
// 6/14/18/21 retained at 469 by ordinary full validation; all arm bodies remain unchanged.
// Round 331: Separate scalar initialization of turning/animation/motion planar copies, subsets 1-7:
// 591/502/612/490/600/510/620; all with Boolean completion 620 (fast control 485; full 469). Original
// copies and complete body restored; turning forms recover the entry but regress overall.
// Round 273: On the recovered turning-constructor register prefix, coherent
// admission/alignment/search workspace names in seven subsets and all with
// planar role names 591/591/591/591/591/591/591/591 (fast control 491; full 478).
// First point/prefix recover but later homes regress; original complete body restored.
// Round 499: One candidate handle for query and publication, crossed with candidate/point scopes
// and both distance-comparison orientations: 490 each (fast control 485; full 469). Original
// observations, scopes, case order and body restored; actor/state and point homes remain.
// @retail 0x259430
short __stdcall function_259430(long arg_0, long arg_1, s_cs_state *arg_2, long arg_3)
{
	short local_30;
	s_actor_view *local_0 = actor_get(arg_0);
	s_cs_datum *local_1 = cs_get(arg_3);
	s_258cf0 *local_2 = (s_258cf0 *)arg_2;
	byte local_3 = 1;
	switch (local_1->type)
	{
	case 1:
		if (*(long *)((byte *)local_0 + 0x1c) != NONE)
		{
			point3f local_4;
			function_b9dd0(arg_1, &local_4);
			local_3 = function_210b60((s_type_c3b527 *)((byte *)arg_2 + 8), &local_4) <= 1.f;
			break;
		}
	case 2: case 3: case 15: case 16: case 17: case 19:
		if (!local_1->unknown77)
		{
			local_3 = false;
			if (*((byte *)local_0 + 0x40))
			{
				if (function_259ec0(arg_0, arg_3))
					local_1->unknown77 = true;
				else if (!local_1->unknown76)
					local_3 = true;
			}
		}
		if (local_1->unknown77)
		{
			if (function_1f86f0(arg_0) || (function_1f8720(arg_0) && !local_1->unknown76) || local_1->type == 19)
			{
				bool local_5 = false;
				if (*((byte *)local_0 + 0x4d5) && local_0->unknown270 == 0)
				{
					point2f local_6 = *(point2f *)((byte *)local_0 + 0x4d8);
					if (normalize2d(&local_6) > 0.f &&
						local_0->unknown290.j * local_6.y + local_6.x * local_0->unknown290.i < 0.984f)
					{
						*(vector3f *)((byte *)local_0 + 0x424) = *(vector3f *)((byte *)local_0 + 0x4d8);
						*(short *)((byte *)local_0 + 0x41c) = 3;
						*(short *)((byte *)local_0 + 0x420) = 4;
						*((byte *)local_0 + 0x44d) = 1;
						local_5 = true;
					}
				}
				if (!local_5)
				{
					if (local_1->type == 19)
						local_1->unknown78 = true;
					else
						function_1f4280(arg_0);
					{ local_30 = 1; goto local_31; }
				}
			}
			if (!function_1f86f0(arg_0) && !function_1f8660(arg_0) && !function_1f8720(arg_0))
				local_1->unknown77 = false;
			local_3 = false;
		}
		break;
	case 4:
		{
			point3f local_7;
			function_b9dd0(arg_1, &local_7);
			vector3f displacement;
			vector3d_from_points3d(&local_2->field_18, &local_7, &displacement);
			vector3f facing = local_2->field_c;
			local_3 = facing.k * displacement.k +
				facing.j * displacement.j +
				displacement.i * facing.i > local_1->unknown08;
		}
		break;
	case 7: case 8:
		if (local_2->field_3 & 4)
		{
			bool local_9 = arg_1 != local_0->unknown018 || local_0->unknown264;
			if ((local_2->field_3 & 8) && local_9)
				local_2->field_0 = 0;
			local_3 = local_2->field_0 == 0;
		}
		break;
	case 0:
		local_3 = local_2->field_0 == 0;
		break;
	case 5:
		if (arg_1 == local_0->unknown018)
		{
			if (local_1->unknown99)
				local_2->field_0 = function_e70e0(arg_1) ? g_510c54->field_2_3 : 0;
			else if (!function_110ab0(arg_1) &&
				function_1ffa30((s_type_c3b527 *)((byte *)local_1 + 0x9c), arg_0, NONE, NONE, false))
				local_1->unknown99 = true;
			local_3 = local_2->field_0 == 0;
		}
		break;
	case 9:
		if (arg_1 == local_0->unknown018)
			local_3 = !recorded_animation_playing(arg_1);
		break;
	case 10:
		local_3 = !local_1->unknown8c;
		break;
	case 12:
		switch (*(short *)((byte *)local_1 + 6))
		{
		case 0: local_3 = local_0->unknown086 > 1; break;
		case 1: local_3 = local_0->unknown086 >= 7; break;
		case 2:
			if (local_2->field_2 & 8)
				local_2->field_2 &= ~0x18;
			else
			{
				local_2->field_2 |= 0x10;
				local_3 = false;
			}
			break;
		}
		break;
	case 11:
		if (arg_1 == local_0->unknown018)
		{
			byte *local_10 = (byte *)object_get(arg_1);
			local_3 = *(short *)(local_10 + *(short *)(local_10 + 0x342) + 0xc) != 14;
		}
		break;
	case 6:
		if (!(local_2->field_3 & 0x20) && arg_1 == local_0->unknown018)
		{
			byte *local_8 = (byte *)g_5023fc;
			*(long *)(local_8 + 0x1c) = local_1->unknown28;
			*(short *)(local_8 + 0x20) = NONE;
			*(real *)(local_8 + 0x28) = 1000.f;
			*(real *)(local_8 + 0x2c) = 1000.f;
			local_8[0x22] |= 0x29;
			local_8[0x22] |= 0x48;
			local_2->field_3 |= 0x20;
			*(short *)g_502400 = 0x4c;
			local_3 = false;
		}
		break;
	case 14:
		if (!(local_2->field_3 & 0x20))
		{
			local_2->field_3 |= 0x20;
			*(short *)g_502400 = *(short *)((byte *)local_1 + 6);
			local_3 = false;
		}
		break;
	case 18:
		if (!(local_2->field_3 & 0x20) && function_25ab50(local_1->unknown28) && function_25ab50(local_1->unknown2c))
		{
			byte *local_11 = (byte *)g_5023fc;
			*(real *)(local_11 + 0x20) = local_1->unknown08;
			*(short *)(local_11 + 0x1e) = *(short *)((byte *)local_1 + 6);
			local_11[0x24] = 1;
			*(long *)(local_11 + 0x28) = local_1->unknown28;
			*(long *)(local_11 + 0x2c) = local_1->unknown2c;
			local_2->field_3 |= 0x20;
			*(short *)g_502400 = 0x7e;
			local_3 = false;
		}
		break;
	case 21:
		if (!(local_2->field_3 & 0x20) && function_25ab50(local_1->unknown28))
		{
			function_1c1080((s_slot_82 *)g_5023fc, local_1->unknown28);
			local_2->field_3 |= 0x20;
			*(short *)g_502400 = 0x82;
			local_3 = false;
		}
		break;
	case 20:
		if (local_1->unknownac)
		{
			if (local_1->unknownb0 == NONE)
			{
				real local_12 = *(real *)((byte *)local_1 + 0xb8);
				s_data_datum_iterator local_13;
				local_13.data = g_4e8c24;
				local_13.datum_index = NONE;
				local_13.index = NONE;
				while (data_datum_iterator_next(&local_13))
				{
					long local_14 = *(long *)(local_13.datum + 0x2c);
					if (local_14 != NONE)
					{
						point3f local_15;
						function_b9dd0(local_14, &local_15);
						vector3f local_16;
						vector3d_from_points3d((point3f *)((byte *)local_0 + 0x238), &local_15, &local_16);
						real local_17 = local_16.k * local_16.k + local_16.j * local_16.j + local_16.i * local_16.i;
						if (local_12 > local_17)
						{
							local_1->unknownac = false;
							local_1->unknownb0 = *(long *)(local_13.datum + 0x2c);
							local_12 = local_17;
						}
					}
				}
				if (local_1->unknownb0 != NONE)
				{
					local_1->unknown52 = true;
					local_1->unknown54 = 1;
					local_1->unknown58 = local_1->unknownb0;
					local_1->unknown46 = true;
					local_1->unknown48 = 1;
					local_1->unknown4c = local_1->unknownb0;
				}
			}
			else
			{
				point3f local_18;
				vector3f local_19;
				function_b9dd0(local_1->unknownb0, &local_18);
				vector3d_from_points3d((point3f *)((byte *)local_0 + 0x238), &local_18, &local_19);
				if (*(real *)((byte *)local_1 + 0xb8) > local_19.k * local_19.k + local_19.j * local_19.j + local_19.i * local_19.i)
					local_1->unknownac = false;
			}
			local_3 = !local_1->unknownac && !*((byte *)local_1 + 0xd0);
		}
		else if (local_1->unknownb0 != NONE)
			local_3 = local_1->unknownd1;
		break;

	}
	if (local_3)
		{ local_30 = 1; goto local_31; }
	if (local_2->field_0 > 0)
		local_2->field_0--;
	if ((local_2->field_3 & 4) && local_2->field_8 > 0)
		local_2->field_8--;
	if ((local_2->field_3 & 1) && (local_2->field_3 & 2))
		function_25a130(arg_0, arg_1, (s_cs_facing *)arg_2);
	if (!*((byte *)local_0 + 7))
	{
		if (local_1->unknown8c && !function_1f50e0(arg_0))
		{
			if (local_1->unknown90 != NONE)
			{
				point2f local_20 = *(point2f *)((byte *)local_0 + 0x6d4);
				normalize2d(&local_20);
				function_1f57f0(arg_0, local_1->unknown90, (long const *)&local_20);
			}
			local_1->unknown8c = false;
		}
		if (local_2->field_3 & 1)
		{
			local_0->unknown456 = true;
			local_0->unknown458 = local_2->field_c;
			*(short *)((byte *)local_0 + 0x454) = local_2->field_8;
		}
		if (local_2->field_3 & 4)
		{
			bool local_21;
			if (local_2->field_3 & 8)
				local_21 = local_2->field_8 > 0;
			else
			{
				local_21 = local_2->field_8 != 0 || local_0->unknown264 || function_110ab0(local_0->unknown018);
				if (!local_21)
				{
					point2f local_22 = *(point2f *)&local_0->unknown290;
					if (normalize2d(&local_22) == 0.f)
						local_22 = *(point2f *)g_468778;
					*((byte *)local_0 + 0x464) = 1;
					*((byte *)local_0 + 0x465) = local_2->field_c.i * 0.7f > local_2->field_c.j;
					*((byte *)local_0 + 0x466) = 1;
					*(point2f *)((byte *)local_0 + 0x468) = local_22;
					*(point2f *)((byte *)local_0 + 0x470) = *(point2f *)&local_2->field_c;
					local_2->field_3 |= 8;
					if (!(local_2->field_3 & 0x10))
					{
						local_2->field_8 = (short)real_to_long(g_510c54->field_2_3 * 0.5f);
					}
				}
			}
			if (local_21)
			{
				local_0->unknown458 = local_0->unknown290;
				local_0->unknown456 = true;
				*(short *)((byte *)local_0 + 0x454) = 0;
			}
		}
	}
	{ local_30 = 0; goto local_31; }
local_31:
	return local_30;
}
