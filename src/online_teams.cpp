#include "unknown_11c920.h"
#include <string.h>
#include <xtl.h>
#include <xonline.h>
#include "data_array.h"
#include "globals.h"
#include "online_tasks.h"

// @flags /O2 /Gr

/* ONLINE_TEAMS.CPP: the online tasks of the teams (clans) and their
   members: create a team (type 0x18), delete it (0x19), answer an invitation
   (0x1d), set a member's rank (0x1e), remove a member (0x1f) and list the
   members (0x20), and the results of the enumerations (UI lane; the open
   range next to 0xabc70) */

/* online_task_try_get with the salt taken first */
static inline s_type_9df9da *online_task_try_get_salted(long task_index)
{
	s_type_9df9da *result = 0;

	if (task_index != NONE)
	{
		s_record_pool *data = g_4cf78c;
		long index = task_index & 0xffff;
		long salt = task_index >> 16;

		if (index < data->high_water_index)
		{
			byte *datum = data->data + data->size * index;

			if (*(short *)datum != 0 && *(short *)datum == salt)
			{
				result = (s_type_9df9da *)datum;
			}
		}
	}

	return result;
}

// @retail 0xabf10
long online_team_delete(XUID const *team, long controller_index)
{
	long task_index = online_task_new_if_logged_on();

	if (task_index != NONE)
	{
		s_type_9df9da *task = online_task_try_get(task_index);

		if (task)
		{
			if (SUCCEEDED(XOnlineTeamDelete(controller_index, *team, NULL, (XONLINETASK_HANDLE *)&task->handle)))
			{
				task->flags = 1;
				task->type = 0x19;
				task->controller_index = controller_index;
			}
			else
			{
				function_6b640(task_index);
				task_index = NONE;
			}
		}
	}

	return task_index;
}

/* answer: 0 no, 1 yes, 2 never */
// @retail 0xabfa0
long online_team_answer_recruit(XUID const *team, long controller_index, long answer)
{
	long task_index = online_task_new_if_logged_on();

	if (task_index != NONE)
	{
		s_type_9df9da *task = online_task_try_get_salted(task_index);

		if (task)
		{
			XONLINE_PEER_ANSWER_TYPE type;

			switch (answer)
			{
			case 0:
				type = XONLINE_PEER_ANSWER_NO;
				break;
			case 1:
				type = XONLINE_PEER_ANSWER_YES;
				break;
			case 2:
				type = XONLINE_PEER_ANSWER_NEVER;
				break;
			default:
				__assume(0);
			}
			if (SUCCEEDED(XOnlineTeamMemberAnswerRecruit(controller_index, *team, type, NULL, (XONLINETASK_HANDLE *)&task->handle)))
			{
				task->flags = 1;
				task->type = 0x1d;
				task->controller_index = controller_index;
			}
			else
			{
				function_6b640(task_index);
				task_index = NONE;
			}
		}
	}

	return task_index;
}

/* the game keeps a member's rank (0..3) where Live keeps the privileges */
// @retail 0xac360
long online_team_member_set_rank(long controller_index, XUID const *team, XONLINE_TEAM_MEMBER const *member)
{
	long task_index = online_task_new_if_logged_on();

	if (task_index != NONE)
	{
		s_type_9df9da *task = online_task_try_get_salted(task_index);

		if (task)
		{
			XONLINE_TEAM_MEMBER_PROPERTIES properties = member->TeamMemberProperties;

			properties.dwPrivileges = 0;
			switch (member->TeamMemberProperties.dwPrivileges)
			{
			case 3:
				properties.dwPrivileges = 0xffffffff;
				break;
			case 2:
				properties.dwPrivileges |= 0xc;
			case 1:
				properties.dwPrivileges |= 0x10;
			case 0:
				break;
			default:
				__assume(0);
			}
			if (SUCCEEDED(XOnlineTeamMemberSetProperties(controller_index, *team, member->xuidTeamMember, &properties, NULL, (XONLINETASK_HANDLE *)&task->handle)))
			{
				task->flags = 1;
				task->type = 0x1e;
				task->controller_index = controller_index;
			}
			else
			{
				function_6b640(task_index);
				task_index = NONE;
			}
		}
	}

	return task_index;
}

// @retail 0xac050
long online_team_member_remove(long controller_index, XUID const *team, XUID const *member)
{
	long task_index = online_task_new_if_logged_on();

	if (task_index != NONE)
	{
		s_type_9df9da *task = online_task_try_get(task_index);

		if (task)
		{
			if (SUCCEEDED(XOnlineTeamMemberRemove(controller_index, *team, *member, NULL, (XONLINETASK_HANDLE *)&task->handle)))
			{
				task->flags = 1;
				task->type = 0x1f;
				task->controller_index = controller_index;
			}
			else
			{
				function_6b640(task_index);
				task_index = NONE;
			}
		}
	}

	return task_index;
}

/* the teams a user belongs to, once the enumeration has a result: count
   holds the room in teams and comes back with the number found */
// @retail 0xabd00
void online_teams_enumerate_get_results(long task_index, DWORD *count, XUID *teams)
{
	s_type_9df9da *task = online_task_try_get_salted(task_index);
	DWORD result = 0;

	if (task && online_logon_connected())
	{
		long status = online_task_poll(task_index);

		if (status == 1 || online_task_poll(task_index) == 2)
		{
			result = *count;
			XOnlineTeamEnumerateGetResults((XONLINETASK_HANDLE)task->handle, &result, teams);
		}
	}
	*count = result;
}

/* a team's details, once the enumeration has a result */
// @retail 0xabdb0
void online_team_get_details(long task_index, XUID const *team, XONLINE_TEAM *details)
{
	s_type_9df9da *task = online_task_try_get_salted(task_index);

	memset(details, 0, sizeof(XONLINE_TEAM));
	if (task && online_logon_connected())
	{
		long status = online_task_poll(task_index);

		if (status == 1 || online_task_poll(task_index) == 2)
		{
			XOnlineTeamGetDetails((XONLINETASK_HANDLE)task->handle, *team, details);
		}
	}
}

/* creates a team with the user as its first member, with every privilege */
// @retail 0xabe60
long online_team_create(long controller_index, XONLINE_TEAM_PROPERTIES const *properties)
{
	long task_index = online_task_new_if_logged_on();

	if (task_index != NONE)
	{
		s_type_9df9da *task = online_task_try_get_salted(task_index);

		if (task)
		{
			XONLINE_TEAM_MEMBER_PROPERTIES member;

			member.dwPrivileges = 0xffffffff;
			member.TeamMemberDataSize = 0;
			if (SUCCEEDED(XOnlineTeamCreate(controller_index, properties, &member, 100, NULL, (XONLINETASK_HANDLE *)&task->handle)))
			{
				task->flags = 1;
				task->type = 0x18;
				task->controller_index = controller_index;
			}
			else
			{
				function_6b640(task_index);
				task_index = NONE;
			}
		}
	}

	return task_index;
}

/* lists a team's members */
// @retail 0xac110
long online_team_members_enumerate(long controller_index, XUID const *team)
{
	long task_index = online_task_new_if_logged_on();

	if (task_index != NONE)
	{
		s_type_9df9da *task = online_task_try_get_salted(task_index);

		if (task)
		{
			if (SUCCEEDED(XOnlineTeamMembersEnumerate(controller_index, *team, 1, NULL, (XONLINETASK_HANDLE *)&task->handle)))
			{
				task->flags = 1;
				task->type = 0x20;
				task->controller_index = controller_index;
			}
			else
			{
				function_6b640(task_index);
				task_index = NONE;
			}
		}
	}

	return task_index;
}

/* a team's members, once the enumeration is done */
// @retail 0xac1b0
void online_team_members_enumerate_get_results(long task_index, DWORD *count, XUID *members)
{
	s_type_9df9da *task = online_task_try_get_salted(task_index);
	DWORD result = 0;

	if (task && online_logon_connected() && online_task_poll(task_index) == 2)
	{
		result = *count;
		XOnlineTeamMembersEnumerateGetResults((XONLINETASK_HANDLE)task->handle, &result, members);
	}
	*count = result;
}

/* a member's details, with the privileges turned back into the game's rank
   (online_team_member_set_rank's inverse) */
// @retail 0xac250
void online_team_member_get_details(long task_index, XUID const *arg_9da427, XONLINE_TEAM_MEMBER *member)
{
	s_type_9df9da *task = online_task_try_get_salted(task_index);

	memset(member, 0, sizeof(XONLINE_TEAM_MEMBER));
	if (task && online_logon_connected() && online_task_poll(task_index) == 2)
	{
		if (SUCCEEDED(XOnlineTeamMemberGetDetails((XONLINETASK_HANDLE)task->handle, *arg_9da427, member)))
		{
			DWORD privileges = member->TeamMemberProperties.dwPrivileges;

			if ((privileges & 1) && (privileges & 2) && (privileges & 4) && (privileges & 8) && (privileges & 0x10))
			{
				member->TeamMemberProperties.dwPrivileges = 3;
			}
			else if ((privileges & 4) && (privileges & 8) && (privileges & 0x10))
			{
				member->TeamMemberProperties.dwPrivileges = 2;
			}
			else
			{
				member->TeamMemberProperties.dwPrivileges = (privileges >> 4) & 1;
			}
		}
		else
		{
			memset(member, 0, sizeof(XONLINE_TEAM_MEMBER));
		}
	}
}
