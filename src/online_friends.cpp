// @flags /O2 /Gr
/* ONLINE_FRIENDS.CPP: the Live friends list (online task types 2 and 3),
   friend requests and game invites, and the friend state as the game's
   flags (lane D) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include "globals.h"
#include "unknown_059ad0.h"
#include "unknown_058ee0.h"
#include "online_tasks.h"
#include "online_friends.h"

// @retail 0x8cc40
long online_friends_startup(void)
{
	long task_index = online_task_find(2, 0xff);
	s_type_9df9da *task;

	if (task_index != NONE)
	{
		task = online_task_get_unchecked(task_index);
		if (!task || !(task->flags & 0x24))
			return task_index;
		online_task_restart(task_index);
		task = online_task_get_unchecked(task_index);
	}
	else
	{
		task_index = online_task_new_if_logged_on();
		if (task_index == NONE)
			return NONE;
		task = online_task_get_unchecked(task_index);
	}

	if (task)
	{
		if (SUCCEEDED(XOnlineFriendsStartup(NULL, (PXONLINETASK_HANDLE)&task->handle)))
		{
			task->flags = 1;
			task->type = 2;
			task->controller_index = 0xff;
		}
		else
		{
			function_6b640(task_index);
			return NONE;
		}
	}
	return task_index;
}

// @retail 0x8cce0
long online_friends_enumerate(DWORD controller_index)
{
	long task_index = online_task_new_if_logged_on();

	if (task_index != NONE)
	{
		s_type_9df9da *task = online_task_try_get(task_index);
		if (task)
		{
			if (SUCCEEDED(XOnlineFriendsEnumerate(controller_index, NULL, (PXONLINETASK_HANDLE)&task->handle)))
			{
				task->flags = 1;
				task->type = 3;
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

// @retail 0x8cd60
short online_friends_get_latest(long task_index, XONLINE_FRIEND *friends)
{
	s_type_9df9da *task = online_task_try_get(task_index);
	short count = 0;

	if (task && online_logon_connected())
	{
		if (online_task_poll(task_index) == 1 || online_task_poll(task_index) == 2)
			count = (short)XOnlineFriendsGetLatest(task->controller_index, MAX_FRIENDS, friends);
	}
	return count;
}

// @retail 0x8cdf0
void online_friends_remove(DWORD controller_index, const XONLINE_FRIEND *friend_)
{
	XONLINE_FRIEND copy = *friend_;

	copy.dwFriendState &= XONLINE_FRIENDSTATE_FLAG_RECEIVEDREQUEST;
	if (online_logon_connected())
		XOnlineFriendsRemove(controller_index, &copy);
}

// @retail 0x8ce40
void online_friends_answer_request(DWORD controller_index, const XONLINE_FRIEND *friend_, long answer)
{
	XONLINE_FRIEND copy = *friend_;
	XONLINE_REQUEST_ANSWER_TYPE type;

	copy.dwFriendState &= XONLINE_FRIENDSTATE_FLAG_RECEIVEDREQUEST;
	switch (answer)
	{
	case 0:
		type = XONLINE_REQUEST_NO;
		break;
	case 1:
		type = XONLINE_REQUEST_YES;
		break;
	default:
		type = XONLINE_REQUEST_BLOCK;
		break;
	}
	if (online_logon_connected())
		XOnlineFriendsAnswerRequest(controller_index, &copy, type);
}

dword online_friend_state_get_flags(DWORD state, DWORD title_id);
bool online_title_is_this_title(DWORD title_id);

// @retail 0x8ceb0
dword online_friend_get_flags(const XONLINE_FRIEND *friend_)
{
	return online_friend_state_get_flags(friend_->dwFriendState, friend_->dwTitleID);
}

// @retail 0x8cec0
void online_friend_copy(const XONLINE_FRIEND *friend_, s_online_friend *result)
{
	result->xuid = friend_->xuid;
	result->flags = online_friend_get_flags(friend_);
	result->session_id = friend_->sessionID;
	result->title_id = friend_->dwTitleID;
	result->unknown1c = false;
}

/* a friend's Live state as the game's friend flags */
// @retail 0x8cf00
dword online_friend_state_get_flags(DWORD state, DWORD title_id)
{
	dword flags = 0;
	bool same_title = false;

	if (state & XONLINE_FRIENDSTATE_FLAG_ONLINE)
	{
		same_title = XOnlineTitleIdIsSameTitle(title_id) != 0;
		flags = 1;
	}
	if (state & XONLINE_FRIENDSTATE_FLAG_JOINABLE)
		flags |= 8;
	if (state & XONLINE_FRIENDSTATE_FLAG_RECEIVEDREQUEST)
		flags |= 0x10;
	if (state & XONLINE_FRIENDSTATE_FLAG_SENTREQUEST)
		flags |= 0x20;
	if (state & XONLINE_FRIENDSTATE_FLAG_PLAYING)
		flags |= 4;
	if (state & XONLINE_FRIENDSTATE_FLAG_RECEIVEDINVITE)
	{
		flags |= 0x80;
		if (same_title)
			flags |= 0x40;
		else
			flags &= ~0x40;
	}
	if ((state & XONLINE_FRIENDSTATE_FLAG_SENTINVITE) && !(state & XONLINE_FRIENDSTATE_FLAG_INVITEREJECTED))
		flags |= 0x100;
	if (state & XONLINE_FRIENDSTATE_FLAG_VOICE)
		flags |= 2;
	return flags;
}

/* the game's friend flags as a Live friend state */
// @retail 0x8cf80
DWORD online_friend_flags_get_state(dword flags)
{
	DWORD state = 0;

	if (flags & 1)
		state = XONLINE_FRIENDSTATE_FLAG_ONLINE;
	if (flags & 8)
		state |= XONLINE_FRIENDSTATE_FLAG_JOINABLE;
	if ((flags & 0x10) || (flags & 0x400))
		state |= XONLINE_FRIENDSTATE_FLAG_RECEIVEDREQUEST;
	if (flags & 0x20)
		state |= XONLINE_FRIENDSTATE_FLAG_SENTREQUEST;
	if (flags & 4)
		state |= XONLINE_FRIENDSTATE_FLAG_PLAYING;
	if (flags & 0x80)
		state |= XONLINE_FRIENDSTATE_FLAG_RECEIVEDINVITE;
	if (flags & 0x100)
		state |= XONLINE_FRIENDSTATE_FLAG_SENTINVITE;
	if (flags & 2)
		state |= XONLINE_FRIENDSTATE_FLAG_VOICE;
	return state;
}

#define SESSION_STATE_IS_LIVE(state) ((state) > 2 && (state) <= 8)

/* retail inlines network_session_interface_get_data_4999 (0x64cf0; its file
   is built /Ob1) */
static inline c_class_58d20 *online_session_get_live(void)
{
	c_class_58d20 *result = 0;
	if (g_527330.initialized)
	{
		c_class_58d20 *session = (c_class_58d20 *)g_527330.session_a;
		long state = session->state;
		if (state && SESSION_STATE_IS_LIVE(state))
			result = session;
	}
	return result;
}

static inline s_long_pair *online_session_get_data_4999_of(c_class_58d20 *session)
{
	s_long_pair *result = 0;
	if (session->flag4998)
		result = &session->data4999;
	return result;
}

static inline s_long_pair *online_session_get_data_4999(void)
{
	s_long_pair *result = 0;
	c_class_58d20 *session = online_session_get_live();
	if (session)
		result = online_session_get_data_4999_of(session);
	return result;
}

/* invites a friend to the current game session (retail reads the session
   id even when there is no session) */
// @retail 0x8cfe0
void online_friends_game_invite(DWORD controller_index, XONLINE_FRIEND *friend_)
{
	if (online_logon_connected())
	{
		XNKID *session_id = (XNKID *)online_session_get_data_4999();
		XOnlineFriendsGameInvite(controller_index, *session_id, 1, friend_);
	}
}

// @retail 0x8d050
void online_friends_answer_game_invite(DWORD controller_index, const XONLINE_FRIEND *friend_, long answer)
{
	XONLINE_GAMEINVITE_ANSWER_TYPE answers[3] = { XONLINE_GAMEINVITE_YES, XONLINE_GAMEINVITE_NO, XONLINE_GAMEINVITE_REMOVE };

	if (online_logon_connected() && (friend_->dwFriendState & XONLINE_FRIENDSTATE_FLAG_RECEIVEDINVITE) && (friend_->dwFriendState & XONLINE_FRIENDSTATE_FLAG_ONLINE))
		XOnlineFriendsAnswerGameInvite(controller_index, (XONLINE_FRIEND *)friend_, answers[answer]);
}

/* a game invite as the game keeps it (0x34 bytes) */
#pragma pack(push, 1)
struct s_online_game_invite
{
	XUID xuid;
	char gamertag[XONLINE_GAMERTAG_SIZE];
	dword unknown1c;
	FILETIME time;
	XNKID session_id;
	DWORD title_id;
};
#pragma pack(pop)

/* the online task screens (src/unknown_147f6d.cpp, src/unknown_1a2ca7.cpp) */
class c_online_task_screen;
void function_1487c3(long controller_index, long task_index, long callback, long value, long context);
void __stdcall online_task_screen_end_with_error(c_online_task_screen *screen);

/* answers a game invite through an online task (type 37) that a task
   screen waits on */
// @retail 0x8d0b0
void online_game_invite_answer(DWORD controller_index, const s_online_game_invite *invite, long answer)
{
	XONLINE_PEER_ANSWER_TYPE answers[3] = { XONLINE_PEER_ANSWER_YES, XONLINE_PEER_ANSWER_NO, XONLINE_PEER_ANSWER_NEVER };

	if (online_logon_connected())
	{
		long task_index = online_task_new_if_logged_on();
		s_type_9df9da *task = function_6b910(task_index);
		if (task)
		{
			XONLINE_GAMEINVITE_ANSWER_INFO info;
			info.dwTitleID = invite->title_id;
			info.GameInviteTime = invite->time;
			info.SessionID = invite->session_id;
			strncpy(info.szInvitingUserGamertag, invite->gamertag, XONLINE_GAMERTAG_SIZE);
			info.xuidInvitingUser = invite->xuid;
			if (SUCCEEDED(XOnlineGameInviteAnswer(controller_index, &info, answers[answer], NULL, (PXONLINETASK_HANDLE)&task->handle)))
			{
				task->flags = 1;
				task->type = 37;
				task->controller_index = controller_index;
				function_1487c3(controller_index, task_index, (long)online_task_screen_end_with_error, 0, 0);
			}
		}
	}
}

// @retail 0x8d190
bool online_friend_is_in_this_title(const XONLINE_FRIEND *friend_)
{
	return online_title_is_this_title(friend_->dwTitleID);
}

// @retail 0x8d1a0
void online_friend_get_title_name(const XONLINE_FRIEND *friend_, wchar_t *name, short name_length)
{
	name[0] = 0;
	if ((online_friend_get_flags(friend_) & 1) && online_logon_connected())
	{
		if (FAILED(XOnlineFriendsGetTitleName(friend_->dwTitleID, XGetLanguage(), name_length, name)))
			name[0] = 0;
	}
}

// @retail 0x8d1f0
void online_friend_from_user(XONLINE_FRIEND *friend_, const XONLINE_USER *user)
{
	memset(friend_, 0, sizeof(*friend_));
	friend_->xuid = user->xuid;
	char *gamertag = friend_->szGamertag;
	strncpy(gamertag, user->szGamertag, XONLINE_GAMERTAG_SIZE);
	gamertag[XONLINE_GAMERTAG_SIZE - 1] = 0;
}
