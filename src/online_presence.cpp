// @flags /O2 /Gr
/* ONLINE_PRESENCE.CPP: the Live presence task (online task type 33): the
   users it watches and the state they publish (lane D) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include "online_tasks.h"

/* a user's presence state as the game's friend flags */
// @retail 0x8c280
dword online_presence_get_flags(DWORD state, DWORD title_id)
{
	dword flags = 0;
	bool same_title = false;

	if (state & XONLINE_PRESENCE_FLAG_ONLINE)
	{
		if (XOnlineTitleIdIsSameTitle(title_id) && online_logon_connected())
			same_title = true;
		else
			same_title = false;
		flags = 1;
	}
	if (state & XONLINE_PRESENCE_FLAG_JOINABLE)
		flags |= 8;
	if (state & XONLINE_PRESENCE_FLAG_RECEIVEDREQUEST)
		flags |= 0x10;
	if (state & XONLINE_PRESENCE_FLAG_PLAYING)
		flags |= 4;
	if (state & XONLINE_PRESENCE_FLAG_RECEIVEDINVITE)
	{
		flags |= 0x80;
		if (same_title)
			flags |= 0x40;
		else
			flags &= ~0x40;
	}
	if (state & XONLINE_PRESENCE_FLAG_VOICE)
		flags |= 2;
	if (state & XONLINE_PRESENCE_FLAG_RECEIVEDTEAMRECRUIT)
		flags |= 0x400;
	if (state & XONLINE_PRESENCE_FLAG_RECEIVEDCOMPREMINDER)
		flags |= 0x1000;
	if (state & XONLINE_PRESENCE_FLAG_RECEIVEDCOMPREQUEST)
		flags |= 0x2000;
	if (state & XONLINE_PRESENCE_FLAG_RECEIVEDTITLECUSTOM)
		flags |= 0x200;
	return flags;
}

// @retail 0x8c320
long online_presence_task_new(DWORD controller_index)
{
	long task_index = online_task_new_if_logged_on();

	if (task_index != NONE)
	{
		s_type_9df9da *task = online_task_try_get(task_index);
		if (task)
		{
			if (SUCCEEDED(XOnlinePresenceInit(controller_index, NULL, (PXONLINETASK_HANDLE)&task->handle)))
			{
				task->flags = 1;
				task->type = 33;
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

// @retail 0x8c3a0
void online_presence_add(long task_index, DWORD group_id, DWORD user_count, XUID *users)
{
	s_type_9df9da *task = online_task_try_get(task_index);

	if (task && online_logon_connected())
		XOnlinePresenceAdd((XONLINETASK_HANDLE)task->handle, group_id, user_count, users);
}

// @retail 0x8c410
void online_presence_submit(long task_index)
{
	s_type_9df9da *task = online_task_try_get(task_index);

	if (task && online_logon_connected())
		XOnlinePresenceSubmit((XONLINETASK_HANDLE)task->handle);
}

// @retail 0x8c470
void online_presence_get_latest(long task_index, DWORD group_id, DWORD count, XONLINE_PRESENCE *presences)
{
	s_type_9df9da *task = online_task_try_get(task_index);

	memset(presences, 0, count * sizeof(XONLINE_PRESENCE));
	if (task && online_logon_connected())
	{
		long status = online_task_poll(task_index);
		if ((status == 1 || status == 2) && SUCCEEDED(XOnlinePresenceGetLatest((XONLINETASK_HANDLE)task->handle, group_id, count, presences)))
		{
			for (DWORD i = 0; i < count; i++)
				presences[i].dwUserState = online_presence_get_flags(presences[i].dwUserState, presences[i].dwTitleID);
		}
	}
}

// @retail 0x8c540
bool online_title_is_this_title(DWORD title_id)
{
	return (bool)XOnlineTitleIdIsSameTitle(title_id);
}

// @retail 0x8c550
void online_presence_task_clear(long task_index)
{
	s_type_9df9da *task = online_task_try_get(task_index);

	if (task && online_logon_connected())
		XOnlinePresenceClear((XONLINETASK_HANDLE)task->handle);
}
