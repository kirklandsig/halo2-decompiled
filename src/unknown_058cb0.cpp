// @flags /O2 /Ob1 /Gr
/* UNKNOWN_058CB0.CPP: the Live mute lists (one per controller) and the
   session state queries */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include "globals.h"
#include "online_tasks.h"
#include "unknown_058dd0.h"
#include "unknown_058ee0.h"
#include <string.h>

/* the mute list startup task, and each controller's mute list task, users
   and user count (NONE until the list is read) */
struct s_online_mutelist_globals
{
	long startup_task;
	long tasks[4];
	XONLINE_MUTELISTUSER users[4][MAX_MUTELISTUSERS];
	long user_counts[4];
};

s_online_mutelist_globals g_4c99c0;

// @retail 0x58a50
void online_mutelist_reset(long controller_index)
{
	g_4c99c0.startup_task = NONE;
	g_4c99c0.tasks[controller_index] = NONE;
	memset(g_4c99c0.users[controller_index], 0, sizeof(g_4c99c0.users[controller_index]));
	g_4c99c0.user_counts[controller_index] = NONE;
}

// @retail 0x58a90
void online_mutelist_startup(long controller_index)
{
	if (g_4c99c0.startup_task == NONE)
	{
		long task_index = online_task_new_if_logged_on();
		if (task_index != NONE)
		{
			s_type_9df9da *task = function_6b910(task_index);
			if (task)
			{
				if (SUCCEEDED(XOnlineMutelistStartup(NULL, (PXONLINETASK_HANDLE)&task->handle)))
				{
					task->flags = 1;
					task->type = 11;
					task->controller_index = controller_index;
					g_4c99c0.startup_task = task_index;
				}
				else
				{
					function_6b640(g_4c99c0.startup_task);
					g_4c99c0.startup_task = NONE;
				}
			}
		}
	}
}

// @retail 0x58b00
void online_mutelist_dispose(long controller_index)
{
	long startup_task = g_4c99c0.startup_task;

	if (g_4c99c0.tasks[controller_index] != NONE)
	{
		function_6b640(g_4c99c0.tasks[controller_index]);
		g_4c99c0.tasks[controller_index] = NONE;
		g_4c99c0.user_counts[controller_index] = NONE;
		memset(g_4c99c0.users[controller_index], 0, sizeof(g_4c99c0.users[controller_index]));
	}
	if (startup_task != NONE && !online_task_exists(12, 0xff))
	{
		function_6b640(startup_task);
		g_4c99c0.startup_task = NONE;
	}
}

// @retail 0x58b70
void online_mutelist_get(long controller_index)
{
	if (g_4c99c0.tasks[controller_index] == NONE || !function_6b910(g_4c99c0.tasks[controller_index]))
	{
		long task_index = online_task_new_if_logged_on();
		if (task_index != NONE)
		{
			s_type_9df9da *task = function_6b910(task_index);
			if (task)
			{
				g_4c99c0.user_counts[controller_index] = NONE;
				if (SUCCEEDED(XOnlineMutelistGet(controller_index, MAX_MUTELISTUSERS, NULL, (PXONLINETASK_HANDLE)&task->handle,
					g_4c99c0.users[controller_index], (DWORD *)&g_4c99c0.user_counts[controller_index])))
				{
					task->controller_index = controller_index;
					task->flags = 9;
					task->type = 12;
					g_4c99c0.tasks[controller_index] = task_index;
				}
				else
				{
					function_6b640(g_4c99c0.tasks[controller_index]);
					g_4c99c0.tasks[controller_index] = NONE;
				}
			}
		}
	}
}

// @retail 0x58c10
void online_mutelist_add(long controller_index, const XUID *xuid)
{
	long count = g_4c99c0.user_counts[controller_index];

	if (online_logon_connected() && count != NONE && SUCCEEDED(XOnlineMutelistAdd(controller_index, *xuid)))
		online_mutelist_get(controller_index);
}

// @retail 0x58c60
void online_mutelist_remove(long controller_index, const XUID *xuid)
{
	long count = g_4c99c0.user_counts[controller_index];

	if (online_logon_connected() && count != NONE && SUCCEEDED(XOnlineMutelistRemove(controller_index, *xuid)))
		online_mutelist_get(controller_index);
}

// @retail 0x58cb0
bool online_mutelist_contains(long controller_index, const XUID *xuid)
{
	bool result = false;
	long count = g_4c99c0.user_counts[controller_index];
	if (count != NONE && TEST_FIELD_BIT(g_54e8e0[controller_index].flag5))
	{
		for (long i = 0; i < count; i++)
		{
			if (memcmp(xuid, &g_4c99c0.users[controller_index][i].xuid, sizeof(*xuid)) == 0)
			{
				result = true;
				break;
			}
		}
	}
	return result;
}

// @retail 0x58d20
inline bool c_class_58d20::function_058d20()
{
	bool result = false;
	long current = state;
	if (current == 5 || current == 6 || current == 7 || current == 8)
	{
		result = true;
	}
	else
	{
		/* retail keeps a dead stack store of the state on this path,
		   inlined into every caller: a volatile local reproduces it */
		volatile long unused = current;
	}
	return result;
}

// @retail 0x58d50
inline bool function_058d50(c_class_58d20 *s)
{
	bool result = false;
	if (s->state > 2 && s->state <= 8)
	{
		result = s->current_member == s->value50;
	}
	return result;
}

// @retail 0x58d70
inline bool function_058d70(c_class_58d20 *s)
{
	if (s->state > 2 && s->state <= 8)
	{
		return true;
	}
	return false;
}

/* the session manager's embedded states and client */
// @retail 0x58e70
s_session_states::s_session_states()
{
}
