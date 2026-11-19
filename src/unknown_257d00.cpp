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

/* the command a command script runs (0x2c bytes, at +0x4 of a command
   script): its type, a per-type selector, two values and two references
   (most types pack a point set in the high word and a point in the low word);
   the datum also names these fields directly */
struct s_cs_order
{
	short type;
	short selector;
	real unknown04;
	real unknown08;
	byte unknown0c[0x24 - 0xc];
	long reference;
	long second_reference;
};

/* a command script (0xd4 bytes) */
struct s_cs_datum
{
	short salt;
	byte unknown02[2];
	union
	{
		struct
		{
			short type;
			byte unknown06[2];
			real unknown08;
			byte unknown0c[0x28 - 0xc];
			long unknown28;
			long unknown2c;
		};
		s_cs_order command;
	};
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
// Matched in a full build with stand-in SDK headers (was 2): the thread index is read through a pointer taken
// before the three stores, so its load is ordered after them, and the second barrier is gone.
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
		long const *thread_index = &cs->thread_index;
		cs->type = NONE;
		cs->unknown28 = NONE;
		cs->unknown2c = NONE;
		g_502410 = cs_index;
		result = hs_thread_run_if_due(*thread_index);
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
// The command fields are read through `command` and the two reference validations through the datum
// itself: retail reloads the reference after each validation call instead of keeping it across.
// HELD (needs the owner's OK): the datum's command fields as one `s_cs_order command;` member read as
// `cs->command.x` everywhere (no union) also matches, but renames five field reads in 0x258880 (same code).
// @retail 0x258cf0
short __stdcall function_258cf0(long arg_0, long arg_1, s_cs_state *arg_2, long arg_3)
{
	s_actor_view *local_0 = actor_get(arg_0);
	s_cs_datum *local_1 = cs_get(arg_3);
	s_258cf0 *local_2 = (s_258cf0 *)arg_2;
	s_cs_order const *command = &local_1->command;
	bool success = false;
	bool allowed = true;
	long perception_index = ((s_handler_actor_view *)local_0)->perception_index;
	if (perception_index != NONE)
	{
		switch (command->type)
		{
		case 1: case 4: break;
		default: allowed = false; break;
		}
	}
	if (allowed)
	{
		switch (command->type)
		{
		case 1: case 2: case 3: case 4: case 6: case 7: case 8: case 9:
		case 14: case 15: case 16: case 17: case 18: case 19: case 21:
			function_259e70(arg_3);
			break;
		}
		switch (command->type)
		{
		case 0:
			local_2->field_0 = (short)command->unknown04;
			success = true;
			break;
		case 17:
			if (!function_25ab50(local_1->command.second_reference))
			{
				break;
			}
		case 15:
		case 16:
			if (!function_25ab50(local_1->command.reference))
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
			if (!function_25ab50(local_1->command.second_reference))
			{
				break;
			}
		case 1:
		case 19:
			if (!function_25ab50(local_1->command.reference))
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
			*(s_type_c3b527 *)((byte *)local_2 + 8) = cs_point_get(command->reference)->point;
			local_2->field_3 |= 0x40;
			success = true;
			break;
		case 4:
			{
				function_b9dd0(arg_1, &local_2->field_18);
				real local_5 = command->unknown08 * 0.01745329238474369f;
				local_2->field_c.k = 0.f;
				local_2->field_c.i = (real)cos(local_5);
				local_2->field_c.j = (real)sin(local_5);
				short local_6 = command->selector;
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
				local_2->field_c.i = (real)cos(command->unknown04 * 0.01745329238474369f) * command->unknown08;
				local_2->field_c.j = (real)sin(command->unknown04 * 0.01745329238474369f) * command->unknown08;
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
				local_2->field_c.i = command->unknown04;
				local_2->field_c.j = command->unknown08;
				local_2->field_0 = (short)real_to_long(g_510c54->field_2_3 * 2.f);
				success = true;
				break;
			}
		case 10:
			local_1->unknown8c = false;
			switch (command->selector)
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
				if (!local_14 || !function_25ab50(local_1->command.reference))
				{
					break;
				}
				s_cs_point *local_15 = cs_point_get(command->reference);
				function_cccd0(local_0->unknown018, *(short *)(local_14 + 4), 1);
				local_1->unknown99 = false;
				*(s_type_c3b527 *)local_1->unknown9c = local_15->point;
				local_1->unknown9a = 0;
				short local_16 = command->selector;
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
				long local_19 = command->reference;
				if (local_19 < 0 || local_19 >= *(long *)((byte *)g_4e0350 + 0x198))
				{
					break;
				}
				short local_21 = named_entry_find((s_named_entry_owner *)g_4e0350,
					*(char **)((byte *)g_4e0350 + 0x19c) + local_19 * 0x28);
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
				if (command->selector == 1)
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

// score 433 (was 433): The dispatch-default branch span, return placement and timer and request scheduling still
// differ. The 0x1c frame and entry register ownership agree.
// Earlier score 433 (was 433): The dispatch branch span and timer/exit layout still differ. The 0x1c frame and entry registers agree. Earlier 479.
// MiniMax E (11-16): no new form, no lower score (479): our body still runs eight bytes longer than retail's, so the jump table lands later; the two floats read from the actor view still reach the local through eax/ecx with the store before the lea, and the aggregate copy at +0x424 and case 4's direct facing reads are still not reproduced.
// MiniMax H (11-18): score 433 (was 479): the switch's cases now sit in retail's order and case 13 returns 1;
// what still differs is the shared return-1 block, which this build keeps at the end of the function where retail
// places it after case 0's body at the case-13 slot, the case-13 body, which returns through a 16-bit move
// where retail's is the shared 32-bit one, the turn-facing sum, which is added in the other order, the case-4
// dot product, which this build computes with another file's helper where retail inlines it, and the facing
// and colour copies, which choose the other registers. The turn-facing sum written with its terms in the other
// order measures 431, a gain of two, which is below the keep bar.
// m3 lane 14 r111 (fast with 0x110ab0; control 433 = full; aligned counts with the jump table cut off, control 126).
// What differs: the `if (local_3)` test sits after case 20 where retail places it after case 0, falling into case
// 13's return-1 epilogue (retail joins the two return-1 blocks there); case 20's else arm stores and reloads the flag
// where retail loads it into AL and jumps past the reload; case 5's `field_0 == 0` is one join where retail repeats it
// (with the EBX/EDI reloads) in each arm and shares case 0's `sete`; case 4 copies the facing; case 2 copies the turn
// facing as floats; case 6 folds its two flag sets into `or 0x69`; case 7/8's flag is normalised. Forms: case 4 reading
// the state's facing directly 611 (2400 B: actor EDI, state EBX); case 2's copy as an aggregate assignment 611; both
// 623 (2384 B); case 18's targets in nested two-use locals 583 (flipped too), with case 4 direct 609; those three
// together 616, with every form below 630 (2368 B; with EBX/EDI swapped back the rest is aligned 136); case 13 as the
// shared `local_30 = 1; goto` exit 481 (2404 B: the joined epilogue moves to the end with the test), as `break` 481,
// removed 481; case 0 moved after case 20 525, with case 13's goto 447; case 7/8's flag as `arg_1 == unknown018 ?
// unknown264 : true` 493 (case 7/8 then exactly retail's; match-only, as listed); case 5 as `!function_e70e0(arg_1) ? 0 :
// rate` 404 (retail's arm order, AX reused; NOT kept: arms swapped with the condition negated, match-only), as an
// if/else 592; the two together 494, with case 13's goto 474; the turn-facing products' operands commuted 433 (same
// code), the two terms swapped 431 (NOT kept: two lower); the 0x465 flag through a long cast 433 (2420 B); case 20's
// else as `&& !d1` / nested `if (!d1) local_3 = false;` 537/537 (2440 B); the flag as bool 433 (same code); case 21's
// target in a two-use local 433 (same code); case 20's iterator as data, index, datum_index 433; plain `return 1;` /
// `return 0;` exits 485 (2400 B), with the case 7/8 and case 5 forms 485.
// m3 lane 16 r120 (fast control 479 = full; all restored): every result goto as a plain `return 1;`
// frame 0x1c and entry agree. Dispatch span, planar copy and request flag stores differ.
// Earlier m3 lane 16 r120 (fast control 479 = full; all restored): every result goto as a plain `return 1;`
// / `return 0;` 483 (re-measured on this base); one `turn_facing` pointer for case 2's copy and publish (same code); case
// 2's aggregate copy, case 4's facing reads and case 18's and case 21's targets held across 0x25ab50, together 636
// (re-measured), with the plain returns 640; case 2 + case 4 only 639; cases 18 + 21 only 619. Each still gives the
// actor EDI and the state EBX (retail: actor EBX, state EDI, the state re-read from its argument slot after calls that
// take EDI, and that slot reused for case 18's first target and case 20's nearest distance).
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
// Earlier 479 (was 479): earlier 477, 610, 469; fast with 0x110ab0 listed. Case 1 now tests `!(d > 1.f)`, retail's compare
// MiniMax I (11-15): no new form, no lower score (479): case 2's publication of the turn vector still copies the twelve
// bytes at +0x4d8 as one dword and two floats where retail copies three dwords through `lea ecx, [ebx+0x424]`; with the
// aggregate copy the body is 2396 bytes like retail's, but the whole layout moves and the score rises to 610, so the
// component copy stays.
// Earlier 479 (was 479): earlier 477, 610, 469; fast with 0x110ab0 listed. Case 1 now tests `!(d > 1.f)`, retail's compare
// earlier 610, 469; fast with 0x110ab0 listed. Case 1 now tests `!(d > 1.f)`, retail's compare
// MiniMax R (11-10): no new form, no lower score.
// Earlier 610 (was 469; fast with 0x110ab0 listed): case 1 now tests `!(d > 1.f)`, retail's compare
// (0x2594b5 `comiss xmm0, [1.0]; jbe 0x2597fa`, true for a NaN distance); `d <= 1.f` compiled to
// `comiss 1.0, d; jae`, false for NaN. The body is now 2396 bytes like retail, but the positional
// count rises because retail reloads the state there (`mov eax, [esp+0x38]; add eax, 8`, 0x2594a7)
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
// m3 lane 3 r241 (fast with the file and 0x110ab0, which agrees with the full build here; all restored): case 6 with
// g_5023fc read at each use (no local copy) 478 (2400 B: `or byte ptr [eax+0x22], 0x69` in place); on it case 7/8's
// one-use flag folded into its `if` 473 (2396 B, retail's length; alone on main 478, but retail computes that flag as a
// value before the bit-8 test, `mov al, 1` / `mov al, [ebx+0x264]`, so main's local is retail's shape and its removal
// was not kept), with the listed `c ? true : byte` flag instead 474 (2396; alone 477, re-measured); on the 473 form the
// `local_21` block's `unknown456 = true` before the 0x458 copy 471 (a statement order: match-only lead; retail stores the
// byte between the copy's second and third dwords). On 471: the 0x454 store before the copy in the field_3 & 1 block 471,
// in the `local_21` block 480, the 0x465 flag before the 0x464 byte 471, the 0x470 pair before the 0x468 pair 479, the
// `field_3 |= 8` first 488 (2380 B). Also: case 6's flags through a named byte (`flags = x | 0x29`, two stores) 617
// (actor and state exchange registers), the 0x48 set after the state flag and the global store 482 (two `or byte
// ptr` RMWs, not retail's load / two stores), on the 478 form 482; case 5's tick reset as an if/else 618; the
// post-switch bit pair as `(field_3 & 3) == 3` 598; case 0 as `!field_0`, case 20 as `!(ac || d0)`, case 18's global
// at each use and the 0x1c/0x20 store order all the same code as their bases. A per-bit bitfield view for the 0x48
// set still folds to `or 0x69` (same code).
// Round 198 (m3 lane 4, fast with the file and 0x110ab0, control 479 / aligned 199; all restored): every arm through
// the existing local_2 for arg_2 479 (same code); the 0x456 flag stored after the 0x458/0x454 copy 480; g_502400
// typed `short *` 479 (same code). LEAD (match-only: case order and swapped arms): retail's jump table puts the case
// blocks in the order 1, 2/3, 4, 7/8, 0, 5, 9, 10, 12, 11, 6, 18, 21, 14, 20 with case 0's compare as the shared
// tail that falls into the post-switch `if (local_3)` test. Case 14 after case 21 481 (aligned 185); + case 7/8's
// flag as `arg_1 != actor->unknown018 ? true : unknown264` 477 (2400 B); as `arg_1 == actor->unknown018 ?
// unknown264 : true` 476 (retail's branch shape there, aligned 174); + case 5 as `!function_e70e0(arg_1) ? 0 :
// rate` 476 (aligned 167); + case 0 moved to the end of the switch 445 (2408 B, aligned 148: the post-switch test
// then sits after case 0 as in retail); + cases 9 and 11 as `local_0->unknown018 == arg_1` 445 (aligned 146). On
// that 445 form: case 5's result written in each arm 635; case 4's facing read from local_2->field_c 578 (2396 B,
// retail's length, but the actor takes EDI and the state EBX); with case 20's two distances written per component
// from the actor's +0x238 579 (still flipped); case 10 as `unknown8c == false` 445 (same code); case 0 back after
// case 7/8 with 7/8 breaking on a clear bit 4 and falling into it 476 (2400 B).
// M3 lane 15 r89 (fast with 0x110ab0; control 479; all restored): case 4 reading the state's facing directly (no `facing`
// copy, retail's form) 627 alone at 2384 B (first +0x9: the actor and state exchange EBX/EDI); that with the state alias
// `local_2` declared ahead of the actor 627 (same code); case 14's block moved after case 21 (retail's jump-table layout:
// 1, 2-group, 4, 7/8, 0, 5, 9, 10, 12, 11, 6, 18, 21, 14, 20) 481 (the case 14 block then sits where retail's does; case
// 18's registers rotate and an alignment pad appears), with cases 9 and 11 as `local_0->unknown018 == arg_1` 481; the result
// flag `local_3` as `bool` 479 (same code).
// M3 lane 15 r92 (plain unless marked; the fast image is not faithful here: fast control 474 at 2400 B against the full
// 479 at 2404 B): case 4 with the facing read from the field at each use, no aggregate copy, 627 (2384 B; fast 467 at
// retail's 2396 B: the copy is retail's missing 8 instructions, but the full build then gives the actor EDI and the
// state EBX from +0x9); the facing through a const pointer 627; that with `displacement` declared first 627; case 4 as
// `dot3f(&local_2->field_c, &displacement)` 627 (retail's k, j, i product order); dot3f with the operands swapped 627.
// M3 lane 16 r129 (fast with 0x110ab0, confirmed plain with the file; control 479, aligned 199, register-masked 146).
// Kept (behaviour, count unchanged): case 4's dot and case 20's two squared distances written i, j, k. VC emits these
// three-term float sums in the reverse of their written order, so the k, j, i spellings compiled to (i + j) + k
// where retail computes (k + j) + i (0x259771-0x2597ad, 0x259ae5-0x259b2b, 0x259b93-0x259bde); written i, j, k all
// three now round as retail's, and case 20's two blocks are retail's instructions and registers: 479 (aligned 172,
// register-masked 131; the case 20 change alone 168/133). The walk above that called these sums retail's order read
// the source, not the compiled code. On the case 2 aggregate copy (610, re-measured, actor EDI and state EBX): case 18's
// targets in nested two-use locals 612 (2400 B); case 21's target in a nested local 610 (same code); the +0x465 flag
// as `c ? 1 : 0` 610; cases 9 and 11 as `local_0->unknown018 == arg_1` 610; case 6 with g_5023fc read at each use 610
// (same code); case 1's point at function scope 610 (same code); case 4's facing read from the state's fields 639 (as
// listed). Every one still gives the actor EDI and the state EBX.
// Two-field planar initializers at the animation release, turning and fallback sites 501/488/494;
// release+turning/release+fallback/turning+fallback 502/509/503; all three 510; separate component assignments
// at all three 510. NOT kept: component transfers grow the body to 2412 to 2440 bytes against retail's 2396;
// the first difference is still the dispatch displacement at +0x51.
// cases 7/8 fall through to 0 479;
// cases 7/8 else-break then fall through 479;
// grouped timer cases 611;
// case 1 commits completion first 619;
// case 1 commits pending first 619;
// case 5 falls through to 0 479;
// cases 7/8 fall through with direct case 1 completion 619;
// case 5 falls through with direct case 1 completion 620;
// Original body restored.
// The 0x1c frame and entry ownership agree. The default branch span, animation
// copy registers and post-dispatch completion placement still differ.
// Case 20 inactive guard 488; existing target first 484; missing-target
// continue 479; missing-target and distance continues 479; for with a
// result-tested iterator and missing-target guard 479; inactive and missing
// guards 488; inactive guard with existing target first 487; clear completion
// from active-search or wait state 479. NOT kept: tied or longer forms.
// N408: Planar input transfers into animation/turn/fallback workspaces, memcpy 474/474/474,
// memmove 578/507/491. Animation pair through its named actor field 474. All three memcpy 474.
// Request bit setter for 0x29/0x48/both 453/445/452. NOT kept: copy forms tie or regress versus the
// reduced control 474. Setters increase length to 2416-2432/2396 bytes and give different store sequences.
// MiniMax R (11-17): no new form, no lower score (479): the switch's jump table still lays the case blocks out in the source's order rather than retail's (1, 2-group, 4, 7/8, 0, 5, 9, 10, 12, 11, 6, 18, 21, 14, 20), so the actor takes edi where retail keeps the state in ebx, and the rounds above record that no case order or arm spelling moves them.
// guarded pending tail 657; three facing branches 474; delayed facing first 578; negative delay admission
// 474; fallback point selection 478; carried turn admission 474; separate active timer paths 481; selected
// animation ticks 627. NOT kept: The delayed-facing branch forms tie the reduced control at 474. Selecting
// the fallback point by address reaches 2396 bytes at 478 in that image, but changes the load boundary and
// scores above the reduced control. The other variants regress.
// The dispatch span, state-bit completion layout and animation copy registers still
// differ. Shared subtype flags 479; subtype completion from the flag 618; incomplete
// subtype guard 479; signed tick-reset value 477; shared countdown/facing flags 479;
// retained selected object 479; facing scalars 627; playback admission guard 479;
// committed turn flags 475. NOT kept: the tick and turn forms shorten to 2400 bytes,
// but gain only two and four points and leave the dispatch span different.
// DH406: Direct completed returns for case 1 distance, case 4 facing, or the clear timer flag in cases
// 7/8 at 574/452/482. The three pairs 559/574/451; all three 559; all three with the default and case
// 13 sharing a return 619. NOT kept: every form regresses at 2416 bytes. The native full-width
// completion tail and dispatch span still differ.
// M3 lane 4 r215 (fast with the file and 0x110ab0, control 433, aligned/masked 111/75): kept, the selector read as the new typed
// command field (`local_1->command.selector`, three sites) and the 0x4d5 flag and the 0x6d4 vector through the actor view's typed
// fields, 433 each (same code). Measured: the command fields through an `s_cs_order const *` (0x258cf0's matching form), all of
// them 433 (same code), all but the two validation arguments 433 (masked 94); the result local as `long` 433 (same code: the
// returns stay `mov ax, 1` where retail has `mov eax, 1`); one to four extra referenced locals (diagnostic) 433 each (case 2's dot
// keeps its reassociated order); cases 9 and 11 as `local_0->unknown018 == arg_1` 432 (aligned 109, masked 73; one, not kept).
// Retail evidence for a lead: case 18 reads every value (unknown04, selector, both references, held across the two validation
// calls in [esp+0x38] and ESI) before the first store into the record, as an inlined setter taking them as arguments would;
// ours re-reads the references after the byte stores. Case 6's two ORs stay separate in retail (0x29, then 0x48 from the copy).
// M3 lane 4 r217 (fast; control 433, aligned 111, masked 75): case 2's turn vector as an aggregate copy with case 4's facing
// read from the state at each use 624 (2384 B, masked 61: retail's code there, actor EDI / state EBX); that with plain
// `return 1` / `return 0` exits and no result local 639 (2368 B, masked 75); with case 18's references in two named locals
// checked one at a time and case 6's flag byte named for two stores 624 (masked 54; case 6 still folds to `or 0x69`); case
// 18's named references alone 528 (2424 B, swapped); case 13 through the shared exit 478 (2400 B, masked 84).
// The frame and entry ownership hold, with a longer dispatch branch and different
// copy-carrier registers. Release-direction assignment 433; requested and settled
// direction snapshots 519/433; release and delayed workspace names 433; inverted
// turn decision 433; byte turn decision 433; fallback value selection 589; named
// delay mode 518; staged turn-direction test 433.
// NOT kept: the equal scores leave the copy schedule and dispatch span different;
// the regressing forms disturb the entry register roles or lengthen the tail.
// M3 lane 5 r204 (fast with 0x110ab0; control 433; all restored): case 6's second OR through a signed char view 433
// (same code: still `or 0x69`); case 4 reading the state's facing at each use 611 (2400 B, re-measured on this base:
// actor EDI, state EBX); on it one, two or three unused locals in the case 611 each (same code: the exchange is not a
// symbol-count tie), the case 2 vector written as three member copies from the actor 611; `default:` joined to case
// 13's `return 1` 613 (2416 B).
// score 433 (was 433):
// Select the observed zero zoom 433; explicit zoom branches 592; raw busy byte by identity branch 493;
// busy default with same-actor update 493; zero-zoom selection and raw busy byte 493; zero-countdown
// fallthrough 506; countdown result before fallthrough 504; fallthrough to sole return 448.
// The dispatch and timer paths still reach the late completion block.
// Selector case order 0-2-1 433, Selector case order 1-0-2 433, Selector case order 1-2-0 433, Selector case
// order 2-0-1 433, Selector case order 2-1-0 433, Selector acknowledgement exits directly 477, Reverse selector
// order with direct acknowledgement exit 477, Unsupported selector exits as completed 433. NOT kept: The selector
// orders and unsupported-selector return tie. The direct acknowledged return shortens the body to 2400 bytes at
// 477. The checked callee stays at 164.
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
				if (local_0->unknown4d5 && local_0->unknown270 == 0)
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
			local_3 = displacement.i * facing.i +
				facing.j * displacement.j +
				facing.k * displacement.k > local_1->unknown08;
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
	case 13:
		return 1;
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
		switch (local_1->command.selector)
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
	case 18:
		if (!(local_2->field_3 & 0x20) && function_25ab50(local_1->unknown28) && function_25ab50(local_1->unknown2c))
		{
			byte *local_11 = (byte *)g_5023fc;
			*(real *)(local_11 + 0x20) = local_1->unknown08;
			*(short *)(local_11 + 0x1e) = local_1->command.selector;
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
	case 14:
		if (!(local_2->field_3 & 0x20))
		{
			local_2->field_3 |= 0x20;
			*(short *)g_502400 = local_1->command.selector;
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
						real local_17 = local_16.i * local_16.i + local_16.j * local_16.j + local_16.k * local_16.k;
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
				if (*(real *)((byte *)local_1 + 0xb8) > local_19.i * local_19.i + local_19.j * local_19.j + local_19.k * local_19.k)
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
				point2f local_20 = *(point2f *)&local_0->unknown6d4;
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
