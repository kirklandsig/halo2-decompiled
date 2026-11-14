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
// MiniMax B (11-13): no new form, no lower score (270): retail's prologue loads the actor table global first and the
// cs global later, while the cs lookup is the first statement here; the two initializers swapped measure 487 and the
// output pointer declared first gives the same code, so the prologue is left as it is, and the table-first lookups in
// cases 7 and 13, case 10's clear after the selector load and case 5's store order are the differences that remain.
// score 270 (was 270; 270 before; 301 before r88; 301 before): 1848 bytes as retail, with retail's success-test layout (return 0 after the test, the failure block
// last). Left: arg_0 in ESI and the output in EBP where retail has EBP and ESI (ours holds the point reference across 0x25ab50 at the
// 1/19 site and case 5, retail reloads it), the prologue's pool order, the table-first lookups in cases 7 and 13, case 10's +0x8c
// clear after the selector load and case 5's store order.
// MiniMax G (11-12): no new form, no lower score (270): the two initializers swapped give 487 and the output pointer declared first the same code; the argument slots for the actor index and the cs index agree with retail.
// Earlier 301 (was 301): the object index stays in ESI and the output pointer in EBP (retail EBP and ESI), the first exit test falls into the failure return, and some bodies are 24 bytes short.
// MiniMax E (11-10): no new form, no lower score (301); retail reads the actor pool global before the datum pool and takes the actor index into ebp among the pushes, this build reads the datum pool first and takes the actor index into esi after the four pushes, and retail puts both return tails (the zero result and the shared 2) at the end of the function where this build leaves the 2 inline in the middle.
// Lead (round 91, m3 lane 9): at the 1/19 site, the `field_3 |= 0x40` store written before the point copy (a store through the
// output ahead of the reference re-read) gives retail's arg_0 EBP and output ESI and the reload `mov edi, [edi + 0x28]`, 297
// (fast, 1824 bytes, aligned 112); the store then sits ahead of the copy where retail has it after, and the join
// still falls into the shared `mov ax, 2` block where retail falls into `xor ax, ax` and keeps the failure block last.
// Earlier summary: (MiniMax E run 26) retail keeps the object index in EBP and the scaled actor base in EBX and reads the case word through EBX, ours keeps the index in ESI and the base in EDI, so the whole dispatch prologue and the case bodies are four bytes out; the forty-byte length deficit and the pool/actor/output ownership remain.
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
// Round 223: Native orientation/motion/direct planar values in subsets 1-7
// 502/470/508/459/495/469/500; all with explicit component construction 500.
// The direct-field temporary at 459 is exact-only; intrinsic/native direct copies
// without that temporary both 471. Original producers and complete body restored.
// Round 335: Whole countdown case zero before 17/4/7/8/10/5/9/13 gives
// 470/471/485/524/526/526/528/528 (fast control 471). The one-point reduction
// is rejected; original complete case order and body restored. Pool/actor/output
// ownership and the forty-byte length gap remain.
// Round 27 (m3 lane 18, fast, plain-confirmed; register-masked aligned 117 before): actor field at each use, no goto, group 17/15/16 before 3/2/1/19 with the wait
// written in both, output read at each use, flag-first case 10, and one success test after the switch: 471 (1848 bytes, retail's length) [69]; datum before actor with the
// output local first: 471 -> 306 [same shape, top of the function now close]; existing real_to_long and one object pointer in case 13: 300 (1840); wait arm before the point
// arm in 1/19: 301 (1824) [59] (kept). Tried: per-arm success returns 518 (1724), `return r ? 0 : 2` 554/734, assigned success with arm branches 563/567; output local at other
// positions 306-308 (same registers); only the 1/2/3 arms through the local 471; exit as `if (!success) return 2` 311/322, result short 476; failures as breaks 301 [59];
// case 7 rounding by hand 300-312, default-zero rounding 378; object flags as a dword or bitfield at 0x108 293 (1832, not retail's byte OR).
// Round 341: Consistent actor-object fields with a shared short/long/byte/word completion,
// failure-first 502/495/565/502, success-first 535/536/548/535. The long failure form reaches
// 1848 bytes but worsens allocation; original callback and observations restored.
// Round 286: Native actor/command/both array elements, command-first then actor-first, original
// elements actor-first, both with output at use: 301/301/301/487/487/487/487/487. Original recovered case/result graph
// and body restored; pool-first acquisition, actor/output carriers and 24-byte gap remain.
// Round 517: On the current case-result graph, direct admission/enclosed execution/named admission
// 332/332/334; repeated selector short/long 334/334; direct guard with short/long selector 466/500;
// enclosed short selector 466. Original admission, selector observations and complete body restored;
// entry carriers and the 24-byte deficit remain.
// Round 324: native record memmove boundaries, subsets 1-7 and all with direct output uses,
// 472/309/487/309/486/323/499/499. All regress; complete callback and transfers restored.
// Round 471: Completion before acquisitions/admitted, bool/byte/short/long,
// 301/301/465/449 and 301/301/465/449. Original completion lifetime and type restored.
// Round 75 (m3 lane 9, fast): 301 -> 246 at retail's 1848 bytes and 487 instructions (masked aligned 37). Retail's table sends cases 6/12/14/18/21 to the return-0 block right after the success
// test, and the failure return is one block at the very end: actor local first 487 (1836), first exit through `goto done` with actor first 298, word flags at +0x10a (retail's `or byte ptr`,
// alone 309, 1816), empty cases as `success = true; break;` 301 alone, `if (!success) return 2; return 0;` 311 (1820), if/else returns 317 (1820), inverted 322 (1808), `short result` if/else 269
// (1848; retail's join layout), `short result = 2` 475, long result with a cast 267, then every other failure as `break` 246, long result 244 (cast). Without actor first 278;
// without the empty cases 269; without word flags 282. No gain: `inline` on 0x25ab50 301 (same code), a byte bitfield 301 (same code), 0x25ab50 defined after this function 246, dot arguments
// swapped 248, by-value forceinline point getter 244, by-pointer 246, plain inline getters 561-582 (stay calls), a one-read named reference 298-301, point arm first 301 (1860), re-derived
// datum for the point read 483-506 (1864-1876). Diagnostics (volatile, reverted at once): a volatile point read at the 1/19 site alone 241 with retail's object index EBP and output ESI and the
// whole prologue exact (masked aligned 25); at case 5 alone 253 with no role change. So our CSE of the point reference across 0x25ab50 at the 1/19 site takes ESI from the object index; retail
// reloads there and at case 5, though 0x25a380 and 0x259430 keep such reads across the same call. Also left: `xor eax, eax` for retail's `xor ax, ax` in the join, the case 7 dot's i term sides, case 10's
// store before the selector load.
// NOT kept (coordinator, round 75 review): the 246 form needs its first exit as `goto done` (a goto is kept only with a match); the same form with that exit as `return 2` gives 485 with
// the actor local first and 493 with the datum local first, so the body is restored at 301 and +0x10a stays a byte cast. Lead: the goto form with the point reload at the 1/19 site above.
// Round 525: Shared case-seven/eight admission, either payload first, native bool/byte/short/long
// moving result: 473/473/473/473/476/476/476/476. Original separate admissions, result widths, cases and
// complete callback restored; pool/input/output ownership and the 24-byte deficit remain.
// F327: Complete single-exit zero/two storage before/after acquisitions,
// short/long/byte/dword 457/457/543/457 in either position. Original callback
// restored; actor/output entry carriers and the 24-byte deficit remain.
// Round 200 (m3 lane 3, fast control 301): fully single-exit form (every `return 0` as `success = true;
// break;`, every `return 2` as `break;`, the pre-switch exit the one goto to the only return, if/else
// short result) 467 (1852 bytes: retail's `je fail`/return-0 layout and arg_0 in EBP, but `xor eax, eax`
// for `xor ax, ax` and the cs index keeps ESI through the reference CSE across 0x25ab50); ending
// `return success ? 0 : 2;` 639 (1696); actor first 466; empty cases grouped 466; `short result = 2`
// then `if (success) result = 0;` 640 (1692). All restored.
// B384: Native packed-reference low/high/both indices at the initial point only,
// recording point only, both complete and crossed consumers: 464/459/464/295/290/307/479/474.
// NOT kept: the 290 recording high-word form uses narrower loads than retail and
// shortens away from 1848 bytes. Original reference observations and callback restored.
// Round 84 (m3 lane 12, fast): at the 1/19 site ours keeps the datum's point reference in ESI across the
// 0x25ab50 call where retail reloads it after the actor test (`mov edi, [edi + 0x28]`); the point arm
// first under `unknown018 == NONE` 300 (1840 bytes); case 4's slot as a ternary 414; case 13 with the
// shared 0xb7360 call after the if/else 301 (same code); case 5's slot as a ternary over 0 304; case 20
// with the unknownac store before the point copy 301.
// P290: Existing completion produced only by its actual case/default, bool/byte/short/long,
// 301/301/465/449; direct default failure at those widths 471/471/463/467.
// Original startup initialization, complete callback and inherited boundary restored;
// actor/output entry carriers, retained point reference and 24-byte deficit remain.
// F364: Native point-set/collection accessor ownership, pointer/reference returns and
// ordinary/forced boundaries: 301/301/301/301/553/303/553/303. NOT kept: shared users regress.
// Original accessor and callback restored; actor/output carriers and 24-byte deficit remain.
// Round 40 (m3 lane 18, fast): whole body inside `if (perception == NONE || type == 1 || type == 4)` with every failure as
// `break`, empty cases as `success = true; break;`, word flags at +0x10a and a `short result` if/else (lane 9's 246 form without
// its goto) 307 (1852 bytes; the `||` test is `cmp cx, 1/4`, not retail's `dec/sub 3`), actor first 305; case 13 word flags with one
// shared 0xb7360 call 309 (1816); success declared at the main switch 301; cs_point_get as an inline function 561 (breaks
// 0x259ec0 and 0x25a380), as forceinline 303 (0x259ec0 still breaks under inline_depth(0)); the wait result tested directly
// with `success = true` in its arm 630 (1700). Diagnostic, reverted: volatile reference reads at the 1/19 copy and case 5 289
// (arg_0 EBP, output ESI as retail).
// Y568: Positive recording/lookup/moving/direct payload-admission scopes, singly and
// paired/complete: 301/301/301/301/301/301/301/301. All tie at 1824/1848 bytes;
// original rejections, cases, observations and full body restored. Pool-first acquisition,
// actor/output ESI/EBP versus EBP/ESI and retained point-reference ownership remain.
// Y570: Full-width initial/recording point and set indices 303/303/301/303/303/301,
// both complete 301, both with one full reference capture 301. Original whole
// callback restored; pool/actor/output carriers and the 24-byte gap remain.
// B420: Existing completion receives secondary/primary/recording validations,
// subsets 1/2/4/3/5/6/7, then all as short: 301/301/301/479/301/301/479/526.
// NOT kept: no exact match; original complete body restored.
// B423: Both bounded orientation/recording slot admissions classified as valid/invalid
// bool/byte/short/long, 301 each at 1824/1848. NOT kept: no exact match;
// original word tests, slot publications, case graph and complete body restored.
// B426: Complete complementary completion state bool/byte/short/long, original/entry
// lifetime: 593/593/594/594/593/593/594/594. NOT kept: no byte match; original completion, all
// producers/consumers and callback restored. Pool/actor/output and retained reference remain.
// B428: Totalized callback result through the only return, no goto, positive admission,
// short/long/byte/word with default/explicit rejection: 590/479/590/479/569/596/590/479.
// NOT kept: no exact match; original callback restored. Actor/output entry ownership,
// retained reference and the 24-byte length deficit remain.
// B430: Selection-owned common validity publication, bool/byte/short/long, default
// invalid/admitted-arm production 358/358/487/487; default valid/explicit rejection
// 355/355/345/357. NOT kept: no byte match; original stores, result and callback restored.
// M3 lane 13 r63 (fast, restored): lead for a ruling. The reloads at the 1/19 and case 5 sites are 0x25ab50's IL effect on its callers: on lane 9's goto form (246, 1848 bytes, masked aligned 67), one store through a
// non-parameter pointer in 0x25ab50 (a pointer to a local, or a reference to a member of a local record; 0x25ab50 still matches) gives 91 at 1848 bytes, masked 10, with retail's arg_0 EBP and output ESI. A full check of
// that store loses only 0x1b13b0 and 0x25a380, which hold the reference across the call. An array element in 0x25ab50 or a by-value struct helper there have no effect (246; 0x25ab50 itself 47 with the helper). Left on 91:
// `xor eax, eax`/`mov eax, 2` for `xor ax, ax`/`mov ax, 2` from the short result, case 7's dot sides, and the 0x8c, 0x99 and 0x9a constant stores one line later than retail. On it: `if (success) return 0;` tails 159 (1816),
// if/else returns 177 (1820), `if (!success) return 2;` 169 (1812), unknown06 as a real short field 91 (same code). Without the store: the empty cases 301 (same code), with actor first 487, word flags 309, all three 497;
// per-arm returns with no join `if (success) return 0;` 582/478 with actor first (1724), `if (!success) return 2;` 581/478, ternary 528/538 (1800); all with actor first and word flags 491/491/544, with the store 632/632/542.
// Y590: Standard bounded byte-copy publications at initial/recording/current-state
// point records, seven subsets then complete backward copy: 454/309/309/481/472/319/489/489.
// NOT kept: native ownership is unrecovered and all extents shrink away from 1848;
// original complete callback and transfers restored.
// Y595: Shared secondary/primary reference-validation phase, positive/complementary bool/byte/short/long results:
// 472/472/465/461/500/500/499/498. NOT kept: no byte equality; original complete body restored.
// J254: Existing notification/reference/movement inline then forced contracts, one provider at a time
// then all: 301/507/301/919/301/1893/301/2376. Ordinary target code ties through the ret-16
// prefix; forced extents 1872/2200/3008/3380 versus 1848. NOT kept: shared-user regressions and
// native outlined-call/length mismatch. Original complete body and all declarations restored;
// actor/output carriers and packed-reference retention remain.
// B432: Original notification switch produces a positive/complementary classification
// in bool/byte/short/long: 303/303/303/303 in either polarity. NOT kept: no exact
// condition-storage form; original callback restored, pool/input/output homes remain.
// B443: Separate original byte clear/set producers at orientation/moving/directed
// payloads, seven subsets then all with set before clear: 301/301/301/301/301/301/301/301.
// NOT kept: no equality or gain; original byte expressions/full callback restored.
// Pool/input/output ownership, retained point-reference carrier and 24-byte gap remain.
// P339: Selected case-seven delay in short/word/int/dword, exhaustive branch
// then conditional producers: 301 each, all 1824/1848 bytes. NOT kept: no
// byte match; original full callback restored. Entry actor/output roles,
// reference retention and 24-byte deficit remain; all calibrated peers unchanged.
// Round 66 (m3 lane 14, fast): actor local first 487 (1836 bytes: our prologue then pushes three registers and inlines the
// first failure return); output local first, success first, both 301 each (same code); in the 1/19 point arm the 0x40 flag
// stored before the point copy 297 (not kept: four points and retail stores the flag after the copy), but the store makes
// ours reload the reference after the 0x25ab50 call as retail does and that arm then takes retail's output ESI: retail's
// reload there comes from a cause before the copy (retail reloads right after the call at case 5 too, before 0xcccd0).
// Diagnostic: inline depth one 301. On 301: 0x25ab50 splitting the reference through an inline out-parameter helper
// 301 (and 0x25ab50 itself 25, 82 bytes); case 5's two tests split 301; the 1/19 reference in a local used for the test
// and the point 303; the 1/19 point through a pointer local after the actor test 303; the empty-success cases as one
// label list 301.
// Round 91 (m3 lane 9, fast; plain for the 289): case 5's two failures as separate ifs 301 (same code); the first test as
// `perception != NONE && type != 1 && type != 4` 332 (1828 bytes); the empty cases listed before case 0 301 (same code);
// at the 1/19 site the flag store before the point copy 297 (roles as retail, see the lead above); the wait test inverted
// (`unknown018 == NONE` arm first) 300 (1840). On the 297 form: actor local first 476 (1836); empty cases as `success =
// true; break;` 297; the tail as `short result` if/else 289 plain (1860 bytes, `xor eax, eax`/`mov eax, 2` for retail's ax
// forms, case 9's failure gets its own epilogue; not kept: a tail-only result while the other returns stay); `if
// (!success) return 2; return 0;` 307 (1820); if/else returns 317 (1828); `short result = 2; if (success) result = 0;`
// 490; that result with the empty cases 289; case 9's/case 5's/both failures as `break` 297/297/297 (same code). The
// tail alone as the `short result` if/else 300 (1860). Case 13's flag RMW stays `mov dl; or dl; mov` where retail ORs
// the byte in memory (lead: a real byte field at +0x10a in the object view, a shared-header change).
// Also: the first switch's `default: return 2;` listed before `case 1: case 4:` 301 (same code), on the 297 form 297, that
// with `if (!success) return 2; return 0;` 307 (1820).
// M3 lane 11 r71 (fast; masked = aligned with registers and stack offsets ignored). The point-set getter as a real
// inline function in 0x25ab50, forceinline or static inline, 301, same code, no reload at the callers; with the point
// getter built on it 301, loses 0x259ec0 (238). Lane 9's goto form rebuilt 246 (1848); with the diagnostic store in
// 0x25ab50 91 (1848, 0x25a380 lost, not kept), where the real causes left are the returns' width, case 7's dot sides
// and the stores in cases 10, 5 and 9, the rest being jump offsets from the width.
// Tails on the 91: `if (success) return 0; return 2;` 159 (1816: the empty return-0 block merges into case 20's tail,
// so `mov ax, 2` follows the test), inverted 169 (1812), if/else 177 (1820), ternary 603 (1728), `short result = 2`
// 599 (1732), a goto to a `failed:` label 169/159, word result 91, an inline status helper 89 (1848, still `xor eax,
// eax`), a ternary into a short 603, the empty cases back to `return 0` 159/169, with the helper 91.
// Per-arm `if (success) return 0; break;` with `return 2;` after the switch, over actor order, flag width and store:
// 601, 609, 504, 513, 591, 605, 620, 628 (1696-1708). Per-arm `if (!success) return 2; return 0;`, empty cases last
// or falling through after case 15/16: 491 clean, 632 with the store. One shared result test with the empty cases
// falling through and other arms using `goto`: 485, 503, 494, 496 (1812-1824). The store lead still needs a ruling.
// B453: Complete motion pair before orientation, reversed motion pair, full orientation/motion group
// after cases 10/5/9/13/20, then motion pair after case 20: 410/380/450/451/449/449/448/419. NOT kept:
// no byte match; original complete body restored. Pool/input/output carriers and retained point
// reference ownership remain.
// Round 66 (m3 lane 14, fast): actor local first 487 (1836 bytes: our prologue then pushes three registers and inlines the
// first failure return); output local first, success first, both 301 each (same code); in the 1/19 point arm the 0x40 flag
// stored before the point copy 297 (not kept: four points and retail stores the flag after the copy), but the store makes
// ours reload the reference after the 0x25ab50 call as retail does and that arm then takes retail's output ESI: retail's
// reload there comes from a cause before the copy (retail reloads right after the call at case 5 too, before 0xcccd0).
// Diagnostic: inline depth one 301. On 301: 0x25ab50 splitting the reference through an inline out-parameter helper
// 301 (and 0x25ab50 itself 25, 82 bytes); case 5's two tests split 301; the 1/19 reference in a local used for the test
// and the point 303; the 1/19 point through a pointer local after the actor test 303; the empty-success cases as one
// label list 301.
// MiniMax B (11-06) round 2: no new form, no lower score (301); the permuter's rewrite of the
// field update as an explicit or of its own gives the same score (301), and its argument swap on
// the recording call lowers this function to 292 on its own (0x259430 stays at 469) but is not
// kept: retail's call site is `mov ecx, eax; mov eax, [esp + 0x18]; push 0; call`, which the
// original call reproduces byte for byte, so the swap would replace an exact call with one that
// passes the unit index as zero and truncates the recorded index; with the field update rewritten
// as well the score of 0x259430 rises to 485, so that pair is dropped and the body is unchanged.
// MiniMax B (11-06): no new form, no lower score (301); the entry still computes the cs datum
// pointer before the actor pointer (retail computes the actor pointer first, keeps the actor
// index in EBP and the actor pointer in EBX, and reads the cs index only after the four
// register pushes), and the case that returns 2 still spells the value `mov ax,2` where retail
// uses `xor ax,ax`; the actor local first stays at 487 and the function is unchanged.
// Walk b4J (11-06), every block against retail 0x258cf0-0x259380 (dispatch tables 0x259384 / 0x25938c /
// 0x2593a4 / 0x2593fc): the perception test (types 1 and 4 pass), the 0x259e70 byte table (types 1-4, 6-9,
// 14-19, 21), all 22 jump-table targets (type 11 and out-of-range go to the failure block, 6/12/14/18/21
// return 0), the 0x25ab50 checks and their order in 17 -> 15/16 and 3 -> 2 -> 1/19, the wait test and the 0x77/0x76
// updates, the point copy and the 0x40 flag, case 4 (0..3 range, 0xfd/1 flag, 0x1f86a0 on the wait object),
// cases 7 and 8 (0x26c test, 0xe7|4 and 0xf7|0x14 masks, delay 0.33 and 2.0 read from the XBE, the
// velocity.forward sum k+j+i and the `> 2.0` test, cos/sin of the unrounded product), the case 10 table,
// cases 5, 9, 13 and 20 (cccd0 arguments, point copy sizes, store order, the 0x10a bits): fields, widths,
// conditions, callees, constants and returns all agree with the draft; no behaviour difference found.
// Left as layout: ours puts `mov ax, 2` inline and `xor ax, ax` last where retail shares the success tail
// (0x258dc5 and 0x258e15 are reached from both the 15/16/17 and the 1/19 call sites; ours duplicates them) and ends
// on the failure block; case 13's flag OR is load/or/store (retail `or byte ptr [ecx+0x10a]`). Tried this walk:
// `bool success;` with `case 11: default: return 2;` 471 (1832, worse); `case 11: return 2;` with the initializer 301.
// Lane 3 r230 (fast, base 301 at 1824 bytes): case 13 through the existing swarm-object flag5/flag6 bitfields 309 (1816;
// retail's direct `or byte ptr`, a repeat of the listed word-flags result); arg_0 and the output renamed by role so the
// output's name sorts first 301 (byte-identical: names do not pick ESI/EBP here); `success` declared first 301 (identical);
// case 7's delay as a ternary 301 (identical); case 9 reading the scenario global at each use, no local, 301; case 13
// with the object looked up in each arm, no local, 313.
// m3 lane 8 r146 (fast; aligned/masked base 151/107): case 13's 0xb7360 call once after the if/else 301 (same code); case 4's
// range result as a ternary 437 (1820 bytes); case 5's clamp as a ternary 304; case 7's mask as `&= ~0x18; |= 4;` 301 (same code);
// case 13's OR value as a ternary with one store 319 (1808). Ours already cross-jumps the 0x259ec0 blocks as retail does.
// M3 lane 10 r89 (fast; positional, stack-masked/register-masked aligned 158/107): the `#pragma inline_depth(2)`
// that wrapped this function removed, 301 (same code; not kept: a compiler-setting change with no gain); the 1/19 point arm as an explicit else 301;
// its destination as the address of field_8 301; case 0 as `success = true; break;` 301 (same code); case 4's range
// as `local_6 < 4` 480 (1828 bytes); case 5's as `local_16 <= 2` 480 (1828). Re-measured: local_1 through the
// existing cs_get 301 (same code); the 1/19 point read through a re-derived datum, `cs_get(arg_3)->unknown28`
// (round 75's 483-506), 480 now at 1848 bytes, 146/76 (the reference is reloaded as in retail, but the scaled cs
// index then stays live in ESI from the entry). The wait block as a two-site helper is round 27's 513 form. Retail reloads the reference after 0x25ab50 at
// this site and at case 5, where 0x25a380 and 0x1b13b0 keep it across the same call, so the cause is in this caller.
// Round 68 (m3 lane 15, fast): a second datum pointer from the same lookup expression, used for the two cs_point_get
// reads, on this body 490 (prologue reorders); case 9 reading g_4e0350 at each use (no scenario local) 301 (case 9 then
// has retail's test/jl/load order). Lead (match-only forms): lane 9's round-75 246 form plus that second pointer at the
// 1/19 and case 5 point reads 107 at 1848 B with retail's arg_0 EBP/output ESI and both reference reloads after 0x25ab50
// (what lane 13 got from a store inside 0x25ab50, without touching it); with case 10's selector and case 5's selector
// read through it (loads and stores through different pointer variables keep their order: retail's +0x8c/+0x99/+0x9a
// clears before the selector loads) and case 9 as above 100. Left on 100: the prologue (the duplicated lookup leaves a
// shared offset temp, `lea edi, [ecx+edx]`, where retail computes the datum in EDI in place), the join's `xor eax, eax`/
// `mov eax, 2` from the `short result` tail (retail `xor ax, ax`/`mov ax, 2`; the 1-byte shift moves every jump-table
// entry) and case 7's dot sides. On 100: `if (success) return 0; return 2;` 169 (1816), `if (!success) return 2;` 179
// (1812), with a failure label 179, failures back to `return 2` 174 (with those tails 169/179), dot3f arguments swapped
// 102, the second pointer declared before the datum/before the actor 104/132, a plain copy of the datum pointer 244.
// m3 lane 11 r88 (fast, 270 plain-confirmed): on 301 the post-switch `return 0` and the empty cases jump into case 20's tail,
// so the success test falls into `mov ax, 2` mid-function and the jumps to it are short (the 24 missing bytes). Case 20
// ending `success = true; break;` 301 (they jump into case 13's tail instead), with case 13 too 305 (case 8's tail). Kept:
// every `return 0` in the main switch as `success = true; break;` and every `return 2` there as `break;`, with the guard's
// `return 2` and the `if (success) return 0; return 2;` tail, 290 (1860 bytes, masked 73; the guard as a goto to that tail
// gives the same code), then case 13 through s_swarm_object's flag6/flag5 bits with one 0xb7360 call (0x28e2b0's form,
// retail's `or byte ptr`) 270 (1848, masked 68). On it: actor local first 504; the lookups through object_header_get 270,
// the 1/19 point arm as an explicit else 270, case 13 with a call in each arm 270 (all the same code); case 9 reading
// g_4e0350 at each use 270 (masked 66); the guard as a goto to one `return success ? 0 : 2;` 651 (1684 bytes), to a
// `short result` if/else 465 (1840), to `short result = 2; if (success) result = 0;` 651 (1684); the tail as
// `if (!success) return 2; return 0;` 319 (1812).
// M3 lane 15 r76 (fast with 0x25ab50, its other callers and this file's functions; aligned/masked in brackets, control 270
// [133/68]): an inline helper returning a cs datum's target point (`cs_point_get(cs->unknown28)`) at the 1/19 copy and case 5
// 556 (1772 bytes: LTCG leaves it a call), at the 1/19 site only 517 (1812), at case 5 only 316 (1808); the same helper
// `__forceinline` at both, either one 270 each (same code: the inliner substitutes the parameter, so the read stays CSE'd).
// Diagnostics: 0x25ab50's indices through an out-parameter decode helper 270 (0x25ab50 itself 25); lane 13 r63's store through a
// pointer to a local in 0x25ab50 re-measured 107 (0x1b13b0 364, 0x25a380 364), and with named reference locals at 0x25a380's two
// and 0x1b13b0's two call sites 107 with 0x25a380 matched again but 0x1b13b0 398 (1448 bytes against 1463).
// After that: local_0 and local_1 through the existing actor_get and cs_get (their bodies were written out here) 270, byte-identical,
// kept; actor_get alone 270 (same code); case 13's object held as an s_swarm_object pointer (no casts at the two flag stores) 270 (same
// code); the datum's +0x9c record typed as s_type_c3b527 so case 5's copy needs no cast 270 (same code).
// T91: complete terminal tail orders 9/20/13, 13/9/20, 13/20/9, 20/9/13,
// 20/13/9 give 270/287/475/283/475; complete 9/13/20 block before cases
// 10/5/8 gives 318/287/339. Every entry prefix through notification dispatch
// is unchanged after addresses are masked. NOT kept: no equality or gain;
// original complete cases, fallthroughs, inputs, copies and callback restored.
// M3 lane 10 r97 (fast with the file's 28 other functions and 0x1b13b0, all unchanged; base 270): nothing kept. The parameters and
// locals renamed to the declaration's names (actor_index, object_index, state, cs_index; actor, cs, output) 270 (same code: names
// do not swap EBP/ESI); success declared first 270; the output pointer declared first 270 (same code). Round 68's second datum
// pointer (declared after the actor) for the 1/19 and case 5 point reads and both selector reads, with case 9 reading g_4e0350 at
// each use, re-measured on this body 495 (1856 bytes: arg_0 then takes EBP as retail, but the shared `lea edi, [ecx + edx]` and
// the prologue order follow); the second pointer for the case 10 and case 5 selector reads alone 490 (1856). Retail keeps the +0x8c
// and +0x9a clears ahead of the selector loads (ours sinks each store below its load) and reloads +0x28 after 0x25ab50 at both sites.
// M3 lane 5 r177 (fast, 0x259e70, 0x259ec0 and 0x25ab50 matched each time): case 7's dot as `dot3f(forward, velocity)` 276;
// case 7's actor unit index named for its two tests 333 (1852 B); case 7's moving test as one ternary with the object
// looked up in the arm 364 (1860 B); the five `success = true` cases under one label list 270 (same code); case 5's
// selector read at each use (no local_16) 270 (same code).
// P423: Complete case four before 0/17/3/8/10/5/9/13/20/6 on the 1848-byte graph:
// 344/337/335/367/390/425/427/428/428/427. Earlier core-only orders predated this result graph.
// NOT kept: no gain; original complete callback restored.
// P433: Independent complete case eight before 0/17/3/4/7/5/9/13:
// 437/426/424/394/358/328/332/335. NOT kept: all regress; original callback restored.
// Pool/input/output ownership and retained point references remain.
// T99: Both movement-query publications on the native-length completion graph:
// admitted query value / independent fallback / failure-first / reject-then-publish /
// fallback as completion / admission expression / separate stores with admission /
// negative fallback admission: 619/601/467/619/463/466/460/270.
// NOT kept: no equality or gain; original complete callback restored.
// P448: Original-site orientation/per-case movement/source/parent/property/lower-bound/lookup-result/final-mode decisions:
// 272/270/272/272/272/272/272/272. NOT kept: no eligible native-shape gain or byte equality; original complete body restored.
// P451: Independent recording/selection/current-point cases before zero 465/472/476;
// orientation default store 412; recording exhaustive stores 219; parent-owned moving result 270;
// direct branch delay stores 290; orientation selector at every use 270. NOT kept: the 219
// form moves the zero store away from retail and grows to 1852/1848; other forms tie or regress.
// M3 lane 12 r121 (one-file LTCG harness with the callee files, 1848 bytes, 257 for the plain 270; fast where given): the
// perception guard as one `&&` test with an early return 309 (1852 bytes); the 1/19 point arm through a named point pointer
// 259 harness; round 91's lead (`field_3 |= 0x40` before the point copy) re-measured on this body 261: every instruction
// from the prologue's end through the 1/19 arm is then retail's (arg_0 EBP, output ESI, +0x28 reloaded after 0x25ab50),
// but it reverses retail's store order (retail reads the reference before any store through the output), so not kept;
// that with the actor looked up first (retail's prologue order, as in 0x259430) 495 harness: ESI is then unused early and
// the guard's `return 2` gets its own shrink-wrapped epilogue where retail jumps to the shared failure tail; both pools
// read into locals first (0x259ec0's idiom) with the lead 495, without it 505. Diagnostics: the 1/19 reference read
// through a dword or as two word halves 446/432 harness. Left: retail treats 0x25ab50 as opaque at the 1/19 site and case 5.
// P467: Complete 17/15/16 validation-and-movement family before 4/7/8/10/5/9/13/20,
// on the native-length result graph: 318/350/413/437/471/471/474/470. NOT kept:
// all regress and grow to 1860 or 1856 bytes; original complete callback restored.
// Complete point-validation family before cases 8/10/5/9/13/20/6 on the native-length
// completion graph: 401/420/455/460/461/461/461; existing actual property-record
// input for recording 270. NOT kept: no equality or gain; placements grow to
// 1864 bytes. Original whole callback and includes restored; pool/actor/output homes remain.
// @retail 0x258cf0
short __stdcall function_258cf0(long arg_0, long arg_1, s_cs_state *arg_2, long arg_3)
{
	s_cs_datum *local_1 = cs_get(arg_3);
	s_actor_view *local_0 = actor_get(arg_0);
	s_258cf0 *local_2 = (s_258cf0 *)arg_2;
	bool success = false;
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
	switch (local_1->type)
	{
	case 0:
		local_2->field_0 = (short)local_1->unknown08;
		success = true;
		break;
	case 17:
		if (!function_25ab50(local_1->unknown2c))
		{
			break;
		}
	case 15:
	case 16:
		if (!function_25ab50(local_1->unknown28))
		{
			break;
		}
		{
			success = function_259ec0(arg_0, arg_3);
			if (success)
			{
				local_1->unknown77 = true;
			}
			else if (local_1->unknown76)
			{
				local_1->unknown77 = false;
				success = true;
			}
			break;
		}
	case 3:
		local_2->field_0 = 0;
	case 2:
		if (!function_25ab50(local_1->unknown2c))
		{
			break;
		}
	case 1:
	case 19:
		if (!function_25ab50(local_1->unknown28))
		{
			break;
		}
		if (local_0->unknown018 != NONE)
		{
			success = function_259ec0(arg_0, arg_3);
			if (success)
			{
				local_1->unknown77 = true;
			}
			else if (local_1->unknown76)
			{
				local_1->unknown77 = false;
				success = true;
			}
			break;
		}
		*(s_type_c3b527 *)((byte *)local_2 + 8) = cs_point_get(local_1->unknown28)->point;
		local_2->field_3 |= 0x40;
		success = true;
		break;
	case 4:
		{
			function_b9dd0(arg_1, &local_2->field_18);
			real local_5 = *(real *)&local_1->unknown0c[0] * 0.01745329238474369f;
			local_2->field_c.k = 0.f;
			local_2->field_c.i = (real)cos(local_5);
			local_2->field_c.j = (real)sin(local_5);
			short local_6 = *(short *)local_1->unknown06;
			if (local_6 >= 0 && local_6 <= 3)
			{
				local_2->field_8 = local_6;
			}
			else
			{
				local_2->field_8 = NONE;
			}
			if (local_0->unknown018 == arg_1)
			{
				function_1f86a0(arg_0);
			}
			local_2->field_3 = (local_2->field_3 & ~2) | 1;
			success = true;
			break;
		}
	case 7:
		{
			if (arg_1 == local_0->unknown018 && local_0->unknown26c != NONE)
			{
				break;
			}
			local_2->field_3 = (local_2->field_3 & ~0x18) | 4;
			bool local_7;
			if (arg_1 == local_0->unknown018)
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
				local_9 = real_to_long(g_510c54->field_2_3 * 0.33f);
			}
			local_2->field_8 = (short)local_9;
			local_2->field_0 = (short)real_to_long(g_510c54->field_2_3 * 2.f);
			local_2->field_c.i = (real)cos(local_1->unknown08 * 0.01745329238474369f) * *(real *)local_1->unknown0c;
			local_2->field_c.j = (real)sin(local_1->unknown08 * 0.01745329238474369f) * *(real *)local_1->unknown0c;
			success = true;
			break;
		}
	case 8:
		{
			if (arg_1 == local_0->unknown018 && local_0->unknown26c != NONE)
			{
				break;
			}
			local_2->field_3 = (local_2->field_3 & ~8) | 0x14;
			local_2->field_8 = 0;
			local_2->field_c.i = local_1->unknown08;
			local_2->field_c.j = *(real *)local_1->unknown0c;
			local_2->field_0 = (short)real_to_long(g_510c54->field_2_3 * 2.f);
			success = true;
			break;
		}
	case 10:
		local_1->unknown8c = false;
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
		}
		success = local_1->unknown8c;
		break;
	case 5:
		{
			byte *local_14 = (byte *)function_1e5380(arg_0);
			if (!local_14 || !function_25ab50(local_1->unknown28))
			{
				break;
			}
			s_cs_point *local_15 = cs_point_get(local_1->unknown28);
			function_cccd0(local_0->unknown018, *(short *)(local_14 + 4), 1);
			local_1->unknown99 = false;
			*(s_type_c3b527 *)local_1->unknown9c = local_15->point;
			local_1->unknown9a = 0;
			short local_16 = *(short *)local_1->unknown06;
			if (local_16 >= 0 && local_16 < 3)
			{
				local_1->unknown9a = local_16;
			}
			local_2->field_0 = (short)real_to_long(g_510c54->field_2_3 * 2.f);
			success = true;
			break;
		}
	case 9:
		{
			long local_19 = local_1->unknown28;
			byte *local_20 = (byte *)g_4e0350;
			if (local_19 < 0 || local_19 >= *(long *)(local_20 + 0x198))
			{
				break;
			}
			short local_21 = named_entry_find((s_named_entry_owner *)local_20,
				*(char **)(local_20 + 0x19c) + local_19 * 0x28);
			if (local_21 == NONE)
			{
				break;
			}
			success = function_1fb360(arg_1, local_21, 0);
			break;
		}
	case 13:
		{
			s_slot_object_view *local_object = object_get(arg_1);
			if (*(short *)local_1->unknown06 == 1)
			{
				((s_swarm_object *)local_object)->flag6 = true;
			}
			else
			{
				((s_swarm_object *)local_object)->flag5 = true;
			}
			function_b7360(arg_1);
			success = true;
			break;
		}
	case 20:
		function_26c180(arg_0);
		local_1->unknownd1 = false;
		((s_25a380 *)local_1)->field_c0 = local_0->unknown27c.point;
		local_1->unknownac = true;
		function_1f86a0(arg_0);
		success = true;
		break;
	case 6:
		success = true;
		break;
	case 12:
		success = true;
		break;
	case 14:
		success = true;
		break;
	case 18:
		success = true;
		break;
	case 21:
		success = true;
		break;
	}
	if (success)
	{
		return 0;
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

// MiniMax C (11-14): no new form, no lower score (479 in the full build, 474 in the reduced one): the two
// floats read from [actor+0x6d4] and [actor+0x6d8] go into [esp+0x10] and [esp+0x14] in ours through
// eax/ecx with the store before the lea of the destination, in retail through ecx/edx with the lea first;
// the twelve-byte copy from [edi+0xc] into [actor+0x458] rotates the same way (ours edx/ecx/eax, retail
// eax/edx/ecx), and the case-2 dot product's first product is formed as the stack value times the field
// in retail and as the field times the stack value here (that order measures 477 in the full build, below
// the gain threshold). The body runs eight bytes longer than retail's, so the jump table lands later and
// the branches after it differ.
// MiniMax A (11-12): the case 2 dot product's terms in retail's order, `local_6.x * local_0->unknown290.i +
// local_0->unknown290.j * local_6.y` (the x term first, the y term second): the full build goes 479 -> 477
// (2404 bytes); the reduced image reads 474 before the change and 477 after it, so the gain holds in the
// full build only; left: retail's aggregate copy at +0x424, case 4's direct facing reads, case 18's held
// references, case 14's block after 21, case 6's two flag stores and the post-switch test laid out after
// case 0.
// MiniMax S (11-11): no new form, no lower score (479; 474 when only this function's own units are in the
// reduced image); our body is longer than retail's (eight bytes in that image), so the jump table lands later
// and every branch after it differs.
// score 479 (was 479): earlier 477, 610, 469; fast with 0x110ab0 listed. Case 1 now tests `!(d > 1.f)`, retail's compare
// earlier 610, 469; fast with 0x110ab0 listed. Case 1 now tests `!(d > 1.f)`, retail's compare
// MiniMax R (11-10): no new form, no lower score.
// Earlier 610 (was 469; fast with 0x110ab0 listed): case 1 now tests `!(d > 1.f)`, retail's compare
// (0x2594b5 `comiss xmm0, [1.0]; jbe 0x2597fa`, true for a NaN distance); `d <= 1.f` compiled to
// `comiss 1.0, d; jae`, false for NaN. The body is now 2396 bytes like retail, but the positional
// score rises because retail reloads the state there (`mov eax, [esp+0x38]; add eax, 8`, 0x2594a7)
// where ours forms `lea eax, [ebx+8]`, which moves every later line by one; revert this line if
// the score matters more than the NaN edge.
// Walk b2I (11-06), every block against retail 0x259430-0x259d33 (types 0-21 through the jump table
// 0x259d34, countdown tail 0x2594c6-0x259d2f): conditions, fields and widths, callees and their
// registers/stack arguments, constants (1.0, 0.0, 0.984, 0.7, 0.5, 1000.0 read from the XBE), the
// copy sizes and the exits all agree with the draft. Only the case-1 compare above differed in
// behaviour. Layout-only differences left: retail places case 14 after 21 (0x259a5d), the retail
// case 6 flag byte gets two stores (`or 0x29` at 0x259989, then `or 0x48` at 0x2599a1) where ours
// folds them to `or 0x69`, and the sums in cases 4 and 20 are written in retail's order
// (k+j)+i / (z2+y2)+x2, which dot3f and length_sq3f (i first) would change for floats.
// The frame agrees at 0x1c; retail computes the actor base in EBX
// and spills it at [esp+0x10] before the type dispatch, ours computes it in EDI just
// before its use and reuses EBX for the state pointer.
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
// DH269: Independent timer/animation arm swaps 7/0, 9/10, 12/11, their pairs/all,
// 485,487,487,487,487,485,485; all with case 5 before the timer pair 502. Reduced control 485
// versus full 469; first +9 actor/state ownership persists. Original arms and entire body restored.
// Round 499: One candidate handle for query and publication, crossed with candidate/point scopes
// and both distance-comparison orientations: 490 each (fast control 485; full 469). Original
// observations, scopes, case order and body restored; actor/state and point homes remain.
// Round 223: Complete search arm before 1/4/7/0/5/9/10/12 gives
// 630/481/469/489/489/488/464/468; five-point gain rejected. On before-ten graph,
// first/both request receivers at uses and native bit updates alone/with both receivers
// 481/481/502/489. Original body restored; actor/state and first point homes remain.
// Round 335: Complete turning/alignment/both-distance product parts, subsets 1-7,
// 485 each; all parts const 485 (fast control 485; full 469). Actor/state allocation
// and first point home remain; all original products, cases and body restored.
// Round 305: Actor/command/state/completion initialization orders C-A-S-R, S-A-C-R, A-S-C-R, C-S-A-R,
// S-C-A-R, R-A-C-S, R-S-A-C, R-C-S-A: fast 614/485/485/614/614/502/502/616 against control 485.
// Original full 469 body restored; actor/state and point homes remain.
// M294: Query/count-state arm orders 9/10/11/12, 9/11/10/12, 9/11/12/10, 10/9/12/11,
// 10/12/9/11, 11/9/10/12, 12/9/10/11, 12/11/10/9:
// 469/469/469/469/469/472/469/472. Original complete executable source restored.
// Round 517: Complete facing fields at uses, existing dot and both squared-length helpers, subsets
// 1-7: 505/470/496/469/505/470/503; direct fields with threshold-first comparison 505. Original
// facing value, helper boundaries, all sums and body restored; actor/state ownership and first-point
// home remain.
// Round 359: Complete spatial/countdown arm priorities 4/0/7/5, 7/4/0/5, 0/4/7/5,
// 5/4/7/0, 0/5/4/7, 7/0/5/4, 5/7/0/4, 0/7/4/5: 469/473/469/478/478/492/476/473.
// Original arms and full body restored; actor/state ownership and first-point home remain.
// Round 361: Complete case-one/fallthrough group after arms 4/7/0/5/9/10/12/11:
// 625/618/618/624/620/628/618/613 (fast control 485; full 469). Original full
// case graph and body restored; actor/state ownership and first-point home remain.
// Round 363: Complete native CRT vector publications turning/state/current, subsets 1/2/4/3/5/6/7
// 485/485/485/485/485/485/485; all with facing input copy 485 (fast control 485; full baseline 469). Original
// transfers and body restored; actor/state registers and first-point home remain.
// Round 365: Native command target/range/readiness fields, subsets 1-7, and all with the
// existing actor perception view: 485 each (fast control 485; full 469). Original complete
// record layout, observations and executable body restored; actor/state and point homes remain.
// Round 200 (m3 lane 3): case-1 test as `if (d > 1.f) local_3 = false;` 610 plain (retail's
// comiss xmm0, [c]/jbe and retail length 2396, but the positional compare shifts a line); case-5
// ternary as if/else 499 fast (loses the movsx store); case 7/8 arm last (retail lays it before the
// shared sete tail) 492; that with the case-1 test 613; dot terms `x*i + j*y` 484, `i*x + j*y` 484
// (fast control 485). All restored.
// B384: Complete branch-produced spatial/countdown/scalar completion values, subsets 1-7,
// then all with canonical Boolean completion: 469/665/480/665/480/688/688/688. Original value producers,
// cases, iterator, facing copy and complete body restored; actor/state and first point homes remain.
// Round 40 (lane 17, fast; control 485, full 469): plain `return 1;`/`return 0;` for every result
// goto 500; the case 7/8 bool written in its test 515 (2372 bytes); case 1 as `!(distance > 1.f)` 622
// (2384 bytes); the last arm's two actor stores swapped 484 (same store order emitted); case 1's
// helper argument and the facing callback argument through the state local 485. All restored.
// Round 64 (lane 10, fast only, under a builds-differently CAVEAT, so these are not plain scores; control 485, full 469): retail's case table order is 1, 2.., 4, 7/8, 0, 13 (the exit), 5, 9, 10, 12, 11, 6, 18, 21, 14, 20, with the tail after case 0 and the sete
// shared by every `== 0` result. Case 14 moved before 20 491 (2400); with `!(distance > 1.f)` 615 (2396, masked aligned 101); that comparison alone or as an if 622 (2384, masked 94, retail's
// comiss with the memory constant); an explicit case 13 before 5 485 (same code); command before actor 614; the result first 502 (frame 0x20).
// F364: Native command-record lookup, global/explicit pool input, pointer/reference
// returns and ordinary/forced boundaries: 485/485/485/485/485/485/485/485 (fast control 485). Original shared
// lookup, record and callback restored; actor/state ownership and first-point home remain.
// Round 40 (m3 lane 18, fast; control 485, full 469): retail's case-block order 1, 2.., 4, 7/8, 0, 5, 9, 10, 12, 11, 6, 18, 21,
// 14, 20 (from its jump table) 491; case 20's point and vector declared outside the iterator loop 485; case 1's point declared
// at the case top 485; the result declared after the other locals 485; the state local for case 1's point and the facing
// call, declared after the flag 485. Retail's actor (EBX) is spilled to [esp+0x10] and that slot is shared with the case
// 1/4/20 points; ours keeps the actor in EDI, which 0x210b60's EDI argument clobbers in case 1, so its slot cannot be shared.
// B416: Completed search-scan/candidate-geometry/selected-target-geometry lifetimes,
// seven subsets then all with complete search role names: 485 each (fast control 485;
// full baseline 469). Original complete body restored; actor/state and first-point homes remain.
// B418: Real state bitfields at dispatch/tail/secondary observations, seven subsets
// and all with raw member tests: 485 each (fast control 485; full 469). Original
// type and body restored; actor/state registers and first-point home remain.
// B420: Complete structured outcome gates the post-dispatch continuation,
// completed/pending in bool/byte/short/long: 766/766/726/718/694/694/672/668.
// NOT kept: no exact match; original complete body restored.
// B423: Both nearest-candidate/selected-target range admissions classified as in/out
// bool/byte/short/long, 469 each at 2400/2396. NOT kept: no exact match;
// original thresholds, squared sums, iterator, publications and full body restored.
// B428: Native overlap-safe turning/animation/motion input transfers, seven subsets
// and all with the default direction transfer: 541/603/610/601/580/611/615/621 (fast control 485;
// full 469). NOT kept: all regress; original input copies and whole body restored.
// Actor/state ownership and first-point home remain.
// B430: Existing byte-flag writers at classification/request/motion publications,
// masks 1/2/3/4/5/6/7: 488/485/488/485/488/485/488; all plus pooled flags 513
// (fast control 485; full 469). NOT kept: no match or gain; original body restored.
// M3 lane 13 r63 (fast, control 485, masked aligned 167; restored): case 1 as `if (d > 1.f)` 622 (2384, masked 159); the facing fields written at each use 519 (2368, masked 142); the existing dot3f for case 4 508 (2368, masked 133); both with dot3f
// 656 (2356, masked 128); with field writes 639 (2368, masked 137); the state local first 485 (same code); case 14 before 20 491 (2400, masked 178), with `!(d > 1.f)` 615, with dot3f 508 (masked 124), all three 644 (masked 121). The callee store
// from 0x258cf0's note 485 (masked 215), with the fields 639 (masked 186): retail keeps the reference across 0x25ab50 at cases 18 and 21, so those would need a local. Retail lays the post-switch test right after the case 7/8 and case 0 `sete` tail.
// Y595: Complete turning/fallback normalization admissions, positive/complementary bool/byte/short/long results:
// 469/469/469/469/469/469/469/469. NOT kept: no byte equality; original complete body restored.
// B432: Native counted turning/state/current vector publications, seven subsets,
// 560/581/625/608/615/622/620; all with facing input loop 597. Turning-only recovers
// actor/state and point +0x10 but regresses. Original aggregate copies and full
// callback restored; actor/state/point homes, later geometry and tail remain.
// B434: Four untried nested subtype orders 0/2/1, 1/0/2, 1/2/0, 2/0/1; pending-first
// state arm; classification default refinements; combined graph: seven fast 485; shared
// classification phase 488 (fast control 485, full 469). NOT kept: no gain; original body restored.
// B436: Both live countdown computations in int/long/short/word storage, mutable/readonly:
// 615/615/615/615/469/469/469/469 on the retained request graph. NOT kept: no exact
// storage form or gain; signed admissions and gated observations preserved, original body restored.
// B438: Candidate-handle sentinel admission, positive/complementary bool/byte/short/long:
// eight fast 485 (unchanged reduced control; full 469). NOT kept: no exact lifetime
// form or gain; original body restored. Actor/state and first-point homes remain.
// B441: Complete state/actor arm after query/request arms 9/10/12/11/6/14/18/21:
// 500/501/502/499/497/497/500/507 (fast control 485; full 469). NOT kept: all regress and shorten
// away from retail; original complete body restored. Actor/state and first-point homes remain.
// B443: Native/CRT turning-anchor snapshots before/after the point query,
// with original/early facing capture: 502/502/517/517/500/500/517/517. NOT kept: all regress
// and grow to 2420 bytes; early facing grows frame 0x1c to 0x28.
// Original anchor observations/facing/full body restored; actor/state and first-point homes remain.
// Round 66 (m3 lane 14, fast with 0x110ab0 listed, which removes the reduced build's caveat and gives the full 469; alone
// the reduced image gives 485): case 1's point read through the state local 469 and the state read through the parameter at
// every use 469 (same code: retail reloads the parameter there, `mov eax, [esp+0x38]`, where ours forms `lea eax, [ebx+8]`);
// case 1 as `!(d > 1.f)` (retail's NaN-true `jbe`) 610; plain returns in place of the result/goto exits 493; the command
// script looked up before the actor 617. A register-masked aligned diff of the baseline gives 152 (case 4's facing copy, the
// countdown tail and retail's duplicated `if (result) return 1` epilogue).
// M3 lane 11 r71 (fast, control 485 at 2396 bytes; plain 469 at 2400). No state local, every use through the
// parameter 485 (same code); the result as bool 485 (same code); actor and datum written directly from the pools 485
// (same code); the two remaining parameter uses through the local 485 (same code); the case 7/8 bool written at its
// condition 515 (2372); case 1 as an if setting the result false 622 (2384). Diagnostics, not kept: extra actor or
// state references before the post-switch test 630-710, any such `if` moves the result into BL and leaves the actor
// and state registers swapped. Left: actor EBX and state EDI for ours EDI and EBX, and the post-switch test's layout.
// Y610: Paired countdown/state-byte/motion-direction lvalue identities,
// subsets 1/2/4/3/5/6/7, then all with actual classification-byte identities:
// 590/500/577/590/588/500/588/604 (fast control 469).
// NOT kept: no exact identity-form match; original complete body restored.
// Round 66 (m3 lane 14, fast with 0x110ab0 listed, which removes the reduced build's caveat and gives the full 469; alone
// the reduced image gives 485): case 1's point read through the state local 469 and the state read through the parameter at
// every use 469 (same code: retail reloads the parameter there, `mov eax, [esp+0x38]`, where ours forms `lea eax, [ebx+8]`);
// case 1 as `!(d > 1.f)` (retail's NaN-true `jbe`) 610; plain returns in place of the result/goto exits 493; the command
// script looked up before the actor 617. A register-masked aligned diff of the baseline gives 152 (case 4's facing copy, the
// countdown tail and retail's duplicated `if (result) return 1` epilogue).
// MiniMax B (11-06) round 2: no new form, no lower score (469); the only variant the permuter
// found in this file is the argument swap on the recording call, which belongs to 0x258cf0 and
// raises this function to 485 when it is applied together with the field rewrite, so it is not
// kept; the actor and state pointers still take the exchanged registers and the first point
// still sits at +0x1c where retail keeps it at +0x10; the body is unchanged.
// MiniMax B (11-06): no new form, no lower score (469); the actor and state pointers still take
// the exchanged registers (ours EDI for the actor and EBX for the state, retail the other way
// round), the first point still sits at +0x1c where retail keeps it at +0x10, and the post-switch
// test and the countdown tail keep our layout; the case-1 point read, the plain returns in place
// of the shared exit and the command script lookup before the actor all stay at 469 or worse,
// so the body is unchanged.
// Round 179 (m3 lane 4, fast with 0x110ab0, 0x258cf0, 0x25a130 and the file's matched functions, plain-confirmed):
// kept: case 2's turn-command vector published through the existing vector3f_assign into `unknown424.vector` from a
// `turn_facing` pointer (in place of the aggregate copy) 610 -> 479 (2404 bytes): the actor takes EBX and the
// state EDI as in retail, the first point moves to +0x10 and case 1 reloads the state from its home as retail, so the
// prologue, the dispatch and case 1 agree; first difference +0x51. Tried, not kept: the same three member stores
// written out 466 (2400 bytes; that is vector3f_assign's body by hand), with two pointers 466; case 2's planar copy by
// components 615, as two scalar stores 615. On the 466 form: case 4's facing read at each use (retail's form there)
// 636 and case 18's two point references held in locals across the 0x25ab50 calls (retail's command reads then agree
// one for one) 618, both 628: each gives the state EBX again; the tail's current-facing publication per component
// 576, with both 551; the case 2 dot's terms i-first in four operand orders 464, j-first in three 466; the case 7/8
// flag as `c ? true : byte` or an if/else 475; plain returns for the three goto exits 474. Left: retail's aggregate
// copy at +0x424 (`lea ecx`), case 4's direct facing reads, case 18's held references, case 14's block after 21, case
// 6's two flag stores and the post-switch test laid out after case 0.
// m3 lane 2 r70 (fast with the file and 0x110ab0; control 479, aligned 199, masked 135): case 6's `|= 0x29` moved ahead of the
// two 1000.f stores 479 (same code: ours still folds both into `or 0x69`, retail stores 0x29 and then 0x48); case 21's
// reference in a local across 0x25ab50 (retail's ESI) 617; with case 18's two references in locals as well 612; with case 2's
// aggregate copy and case 4's facing read from the state's fields too (retail's local forms in all four cases) 635 (2372
// bytes, masked 188); with case 7/8's flag as `arg_1 == actor ? byte : true` 632 (2368). Each flips the actor to EDI and the
// state to EBX. Lead: retail cross-jumps the `sete [esp+0x3c]` tails of cases 7/8, 0, 5 and 10 into one block that sits
// right before the post-switch test (case 9/12/20 jump past it to the test); ours keeps three setes and the test after case 20.
// m3 lane 16 r107 (fast with the file, 0x110ab0 and 0x25a130; control 479, aligned 146, masked 90; all restored): case 7/8
// ending with `break` when bit 4 is clear and falling into case 0's result 479 (same code); case 2's copy as an aggregate
// assignment through a named destination pointer 610 (retail's `lea ecx, [ebx+0x424]` copy, actor/state flip); case 5's
// result written in each arm 634 (2416 bytes); case 20's iterator set up as data, index, datum_index (the order of ai.cpp,
// game_loop.cpp and teleporters.cpp) 479 (retail's store order, aligned 145, masked 89; may repeat the listed "iterator
// initializer"); with the distance limit read after it 479 (aligned 146); cases 9 and 11 as `local_0->unknown018 == arg_1`
// 479 (retail's `cmp [ebx+0x18], reg` in both, aligned 144, masked 88). The last two leads are retail's forms but move no
// positional line while the body is 8 bytes long. Case 6's two flag stores (compile-only): `|= 0x29; |= 0x48`, bitfield
// sets and inline set-up helpers all fold to `or 0x69`; only a branch between the two sets keeps retail's second store.
// NOT kept (coordinator review, MiniMax A 11-12): 479 -> 477 is under the keep bar (8+ on a function still above 30); the body is main's again.
// P423: Complete case four after 9/10/12/11/6/14/18/21:
// 483/483/483/470/470/470/476/472. NOT kept: the 470 forms move the case away from its
// retail position; no byte match. Original complete callback restored.
// T95: Native entry and first point now agree; first +0x51 is a branch displacement.
// Motion/animation/both last-consumer scopes 479/479/479; paired final planar
// memcpy/memmove 479/479; state/current/both existing vector setters 565/592/594.
// NOT kept: no exact transfer-form repair; original complete body restored.
// P433: Explicit case-thirteen/default completion to the sole return before
// 1/4/7/0/5/9/12/20: 621/621/621/621/621/621/621/621. NOT kept:
// native length recovered but entry registers regress; original callback restored.
// T99: Real animation-to-countdown fallthrough group before outer cases
// 4/7/9/10/12/11/6/14: 475/483/479/483/483/483/468/474.
// NOT kept: the 468 form needs the selected group placement without byte
// equality; original complete case graph and countdown comparisons restored.
// P444: animation planar setter 501f; motion planar setter 494f; turning planar setter 490f; fallback planar setter 633f; case-four array difference 479f; nearest-candidate array difference 479f; retained-target array difference 479f; facing-input vector setter 627f.
// NOT kept: native entry remains, but planar/case/exit transport does not match; original complete body restored.
// P446: Actual case-five request, animation query, native mode, both subtype results, case-18/21 admissions and secondary-countdown producer;
// 479/479/479/479/479/479/479/479. NOT kept: no eligible native-shape gain or byte equality; original complete body restored.
// P449: Primary countdown, actor, animation release/index, facing, turn, delay and motion decisions:
// 479/479/479/479/479/479/483/479. NOT kept: no exact scratch-register or completion-tail repair.

// M3 lane 16 r114 (fast with the file and 0x110ab0; control 479, aligned 215; all restored): the +0x465 flag as `c ? 1 : 0` or
// through a long cast 482 (2408 bytes: retail's `mov eax, 1` / `xor eax, eax`, aligned 214, four bytes longer); case 18's two
// references in two-use locals under nested tests 619 (2400 bytes; actor EDI, state EBX); case 21's reference in a two-use local
// under a nested test 479 (same code); case 4's facing from the state's fields 627 (2384 bytes); case 2's vector as an aggregate
// assignment 610 (2396 bytes); cases 9 and 11 as `local_0->unknown018 == arg_1` 479 (aligned 213), the iterator set up data,
// index, datum_index 479 (aligned 214), both 479 (aligned 212); all of these together 636 (2372 bytes, aligned 296), cases 18,
// 4 and 2 alone 636; on that set the state pointer declared first 636, the script and state ahead of the actor 640, the result
// flag first 640; the state pointer declared first on this body 479 (same code). Each retail-shaped form of cases 2, 4 and 18
// still exchanges the actor and state registers, and no declaration order brings the actor back to EBX.
// P467: Sole-return success default with redundant success assignments removed,
// short/long/byte/bool at entry 604/604/630/630, after lookups 605/605/631/631.
// NOT kept: all regress; wide forms reach 2396 bytes but lose native entry allocation.
// Existing actor Boolean observations/control publications/spatial inputs, seven
// canonical-field subsets: 479/482/482/479/479/482/482; all with the existing
// perception accessor 625. NOT kept: no equality or gain; publication forms grow
// to 2408 bytes. Original complete callback restored; later copy and exit carriers remain.
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
			local_3 = !(function_210b60((s_type_c3b527 *)((byte *)arg_2 + 8), &local_4) > 1.f);
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
						vector3f const *turn_facing = (vector3f const *)((byte *)local_0 + 0x4d8);
						vector3f_assign(&local_0->unknown424.vector, turn_facing->i, turn_facing->j, turn_facing->k);
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
