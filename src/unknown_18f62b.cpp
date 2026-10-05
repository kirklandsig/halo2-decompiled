// @flags /O1 /Oi /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>

struct s_controller_event
{
	long type;
	long controller;
	long value;
	short amount;
};

s_controller_event g_551aa0[4];
bool g_551ae4;
extern bool g_551ae0[4];
extern dword g_54d5b8;

struct s_slot_update_view
{
	union
	{
		dword flags;
		struct { dword field_0_0 : 5; dword field_0_5 : 1; };
	};
	byte field_4[0x470 - 4];
	XONLINE_USER user;
	XONLINE_TEAM team;
	XONLINE_TEAM_MEMBER member;
	long team_task;
	long member_task;
	byte field_c1c[0xc70 - 0xc1c];
};
C_ASSERT(sizeof(s_slot_update_view) == 0xc70);

void function_147dbe(s_controller_event *event);
void function_1907d6(dword time);
void function_191234(long index);
bool function_1249f0(long memory_unit, char *drive_letter);
bool voice_port_can_talk(long port);
void function_190f9b(long index);
long online_task_poll(long task_index);
void online_teams_enumerate_get_results(long task_index, DWORD *count, XUID *teams);
void online_team_get_details(long task_index, XUID const *team, XONLINE_TEAM *details);
long online_team_members_enumerate(long controller_index, XUID const *team);
void function_6b640(long task_index);
void online_team_members_enumerate_get_results(long task_index, DWORD *count, XUID *members);
bool xuid_equal(XUID const *a, XUID const *b, bool compare_guest_number);
void online_team_member_get_details(long task_index, XUID const *id, XONLINE_TEAM_MEMBER *member);
void function_18fe9e(long index);
void function_18f9be();

PRIVATE inline long slot_update_next(long index)
{
	long next = NONE;
	if (index >= 0 && index < 3) next = index + 1;
	return next;
}

PRIVATE inline XUID const *slot_update_id(XONLINE_USER const *user)
{
	XUID const *result = 0;
	if (user) result = &user->xuid;
	return result;
}

PRIVATE inline bool slot_update_connected(short index)
{
	return g_4e61cc[index] != 0;
}

// @retail 0x18f62b
void function_18f62b()
{
	long memory_units[4][2] = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 } };
	if (g_551aa0[0].type)
	{
		s_controller_event *event = g_551aa0;
		function_147dbe(event);
		for (long i = 0; i < 3; i++)
			event[i] = event[i + 1];
		memset(g_551aa0 + 3, 0, sizeof(s_controller_event));
	}
	else if (!g_551ae4)
		function_1907d6(g_54d5b8);
	long index = 0;
	do
	{
		s_slot_update_view *slot = &((s_slot_update_view *)g_54e8e0)[index];
		bool changed = false;
		bool connected = slot_update_connected((short)index);
		if (connected)
		{
			g_551ae0[index] = false;
			slot->flags |= 1;
		}
		else
		{
			function_191234(index);
			slot->flags &= ~1;
		}
		if (connected)
		{
			char drive;
			if (function_1249f0(memory_units[index][0], &drive)) slot->flags |= 2;
			else slot->flags &= ~2;
			if (function_1249f0(memory_units[index][1], &drive)) slot->flags |= 4;
			else slot->flags &= ~4;
			if (voice_port_can_talk(index)) slot->flags |= 8;
			else slot->flags &= ~8;
		}
		else slot->flags &= ~14;
		if (TEST_FIELD_BIT(slot->field_0_5)) function_190f9b(index);
		if (slot->team_task != NONE)
		{
			long status = online_task_poll(slot->team_task);
			bool done = status != 0;
			if (status > 0 && status <= 2)
			{
				XUID teams[8];
				DWORD count = 8;
				online_teams_enumerate_get_results(slot->team_task, &count, teams);
				if ((long)count > 0)
				{
					online_team_get_details(slot->team_task, teams, &slot->team);
					changed = true;
					slot->member_task = online_team_members_enumerate(index, teams);
				}
			}
			if (done)
			{
				function_6b640(slot->team_task);
				slot->team_task = NONE;
			}
		}
		if (slot->member_task != NONE)
		{
			long status = online_task_poll(slot->member_task);
			bool done = status != 0 && status != 1;
			switch (status)
			{
			case 2:
			{
				XUID const *id = slot_update_id(&slot->user);
				XUID members[100];
				DWORD count = 100;
				online_team_members_enumerate_get_results(slot->member_task, &count, members);
				for (long i = 0; i < (long)count; i++)
				{
					if (xuid_equal(&members[i], id, false))
					{
						online_team_member_get_details(slot->member_task, id, &slot->member);
						break;
					}
				}
			}
				break;
			}
			if (done)
			{
				function_6b640(slot->member_task);
				slot->member_task = NONE;
			}
		}
		if (changed) function_18fe9e(index);
		index = slot_update_next(index);
	} while (index != NONE);
	function_18f9be();
}
