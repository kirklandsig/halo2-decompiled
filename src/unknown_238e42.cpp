#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include "screen_widgets.h"
#include "unknown_19b510.h"
#include "unknown_234c64.h"
#include "unknown_18f576.h"
#include "online_tasks.h"
#include "online_message_entries.h"
#include <string.h>
#include "unknown_18f576.h"
#include "online_tasks.h"
#include "online_message_entries.h"
#include <string.h>

// @flags /O1 /Oi /arch:SSE /Gr

/* UNKNOWN_238E42.CPP: what the online screens do with the player or
   friend the user chose (friend requests, game invites, messages) and the
   checks before a squad is made */

struct s_message;
void function_149f49(s_message *message, word a, dword *id, word b, long c, long d, long e);
void function_14887e(s_screen_settings_54dc6c *settings);
void function_1487c3(long controller_index, long task_index, long callback, long value, long context);
class c_online_task_screen;
void __stdcall function_1a2cb7(c_online_task_screen *screen);
void __stdcall function_1a2d2f(c_online_task_screen *screen);

void online_friend_from_user(XONLINE_FRIEND *friend_, const XONLINE_USER *user);
void online_friends_remove(DWORD controller_index, const XONLINE_FRIEND *friend_);
void online_friends_answer_request(DWORD controller_index, const XONLINE_FRIEND *friend_, long answer);
void online_friends_game_invite(DWORD controller_index, XONLINE_FRIEND *friend_);

bool network_session_manager_session_unready(void);
bool network_session_manager_session_a_established(void);
dword function_0592d0(void);
short network_session_interface_get_value_5dd0(void);
bool function_19a935(void);
long function_199fd6(void);
long function_19a161(void);

typedef bool (__stdcall *multiple_choice_callback)(long controller_index, long item);
void function_2b8c05(long a, long b, word user_flags, multiple_choice_callback callback, long title, long count, long *string_ids);
void __stdcall function_2395dc(XONLINE_FRIEND *friend_, long controller_index, long mode);

c_class_1473c9 *__stdcall function_2b8add(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b8099(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b80a9(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b80d9(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b80e9(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b8aed(s_screen_parameters *parameters);

/* the player the online screens act on, as function_14887e copies it out:
   a user (type 1) or a friend (type 2) */
#pragma pack(push, 1)
struct s_online_selection
{
	long type;
	union
	{
		XONLINE_USER user;
		XONLINE_FRIEND friend_;
	};
	/* set when the friend's game is to be joined through Live */
	bool join_pending;
	byte unknown75[0x78 - 5 - sizeof(XONLINE_USER)];
};
#pragma pack(pop)

/* the selection as a friend */
static __forceinline void online_selection_get_friend(s_online_selection *selection, XONLINE_FRIEND *friend_)
{
	switch (selection->type)
	{
	case 1:
		online_friend_from_user(friend_, &selection->user);
		break;
	default:
		*friend_ = selection->friend_;
		break;
	}
}

/* opens the message send screen for a message of this type */
// @retail 0x238e42
void function_238e42(long controller_index, long type)
{
	s_screen_parameters parameters;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, 1 << controller_index, 3, 4, 0);
	switch (type)
	{
	case 1:
		parameters.load = function_2b8099;
		break;
	case 2:
		parameters.load = function_2b80a9;
		break;
	case 3:
		parameters.load = function_2b80d9;
		break;
	case 4:
		parameters.load = function_2b80e9;
		break;
	}
	if (parameters.load)
	{
		parameters.load(&parameters);
	}
}

// @retail 0x238ee3
void function_238ee3(long controller_index)
{
	s_screen_parameters parameters;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, 1 << controller_index, 3, 4, (long)function_2b8add);
	parameters.load(&parameters);
}

// @retail 0x238f11
void function_238f11(long controller_index)
{
	s_screen_parameters parameters;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, 1 << controller_index, 3, 4, (long)function_2b8aed);
	parameters.load(&parameters);
}

struct s_online_game_invite;
void online_friends_answer_game_invite(DWORD controller_index, const XONLINE_FRIEND *friend_, long answer);
void online_game_invite_answer(DWORD controller_index, const s_online_game_invite *invite, long answer);

/* the game invitation of a message */
struct s_game_invite_message_view
{
	byte unknown00[0x24];
	dword title_id;
	byte unknown28[8];
	FILETIME time;
};

/* declines the game invitation of the message from the selected player;
   without the message's session it declines the friend's own invitation */
// @retail 0x23902b
void __stdcall function_23902b(void *message, long controller_index, unsigned __int64 session_id)
{
	s_game_invite_message_view *invite = (s_game_invite_message_view *)message;
	s_online_selection selection;

	function_14887e((s_screen_settings_54dc6c *)&selection);
	switch (selection.type)
	{
	case 1:
		if (invite && session_id)
		{
			XONLINE_FRIEND friend_;

			online_friend_from_user(&friend_, &selection.user);
			friend_.dwFriendState |= XONLINE_FRIENDSTATE_FLAG_ONLINE | XONLINE_FRIENDSTATE_FLAG_RECEIVEDINVITE;
			friend_.dwTitleID = invite->title_id;
			friend_.gameinviteTime = invite->time;
			*(unsigned __int64 *)&friend_.sessionID = session_id;
			selection.friend_ = friend_;
			selection.type = 2;
			online_game_invite_answer(controller_index, (s_online_game_invite *)&selection.friend_, 1);
		}
		break;
	default:
		if (invite && session_id)
		{
			selection.friend_.dwFriendState |= XONLINE_FRIENDSTATE_FLAG_RECEIVEDINVITE;
			*(unsigned __int64 *)&selection.friend_.sessionID = session_id;
			selection.friend_.dwTitleID = invite->title_id;
			selection.friend_.gameinviteTime = invite->time;
			online_game_invite_answer(controller_index, (s_online_game_invite *)&selection.friend_, 1);
		}
		else
		{
			online_friends_answer_game_invite(controller_index, &selection.friend_, 1);
		}
		break;
	}
}

/* invites the selected player to the game */
// @retail 0x2390f8
void function_2390f8(long controller_index)
{
	s_online_selection selection;

	function_14887e((s_screen_settings_54dc6c *)&selection);
	switch (selection.type)
	{
	case 1:
	{
		XONLINE_FRIEND friend_;

		online_friend_from_user(&friend_, &selection.user);
		friend_.dwFriendState |= 0x4000000;
		selection.friend_ = friend_;
		selection.type = 2;
		break;
	}
	}
	online_friends_game_invite(controller_index, &selection.friend_);
}

/* accepts the selected player's friend request */
// @retail 0x23914b
void function_23914b(long controller_index)
{
	s_online_selection selection;
	XONLINE_FRIEND friend_;

	function_14887e((s_screen_settings_54dc6c *)&selection);
	online_selection_get_friend(&selection, &friend_);
	online_friends_answer_request(controller_index, &friend_, 1);
}

/* declines it */
// @retail 0x239197
void function_239197(long controller_index)
{
	s_online_selection selection;
	XONLINE_FRIEND friend_;

	function_14887e((s_screen_settings_54dc6c *)&selection);
	online_selection_get_friend(&selection, &friend_);
	online_friends_answer_request(controller_index, &friend_, 0);
}

/* blocks the player */
// @retail 0x2391e2
void function_2391e2(long controller_index)
{
	s_online_selection selection;
	XONLINE_FRIEND friend_;

	function_14887e((s_screen_settings_54dc6c *)&selection);
	online_selection_get_friend(&selection, &friend_);
	online_friends_answer_request(controller_index, &friend_, 2);
}

/* removes the friend */
// @retail 0x23922e
void function_23922e(long controller_index)
{
	s_online_selection selection;
	XONLINE_FRIEND friend_;

	function_14887e((s_screen_settings_54dc6c *)&selection);
	online_selection_get_friend(&selection, &friend_);
	online_friends_remove(controller_index, &friend_);
}

// @retail 0x2397b9
void function_2397b9(long controller_index)
{
	dialog_ok_show(3, 0x7f, 4, 1 << controller_index, 0, 0);
}

// @retail 0x23982a
void function_23982a(long controller_index)
{
	dialog_ok_show(1, 0xbf, 4, 1 << controller_index, 0, 0);
}

// @retail 0x239877
bool __stdcall function_239877(long controller_index, long item)
{
	s_online_selection selection;

	function_14887e((s_screen_settings_54dc6c *)&selection);
	switch ((short)item)
	{
	case 1:
		function_2395dc(&selection.friend_, controller_index, 1);
		break;
	}
	return true;
}

/* asks how to join the friend */
// @retail 0x239843
void function_239843(long controller_index)
{
	long string_ids[2];

	string_ids[0] = 0x1200068b;
	string_ids[1] = 0x1300068c;
	function_2b8c05(3, 4, 1 << controller_index, function_239877, 0x1000068a, 2, string_ids);
}

// @retail 0x2398dc
bool __stdcall function_2398dc(long controller_index, long item)
{
	s_online_selection selection;

	function_14887e((s_screen_settings_54dc6c *)&selection);
	switch ((short)item)
	{
	case 1:
		function_2395dc(&selection.friend_, controller_index, 2);
		break;
	case 2:
		function_2395dc(&selection.friend_, controller_index, 3);
		break;
	}
	return true;
}

// @retail 0x2398a0
void function_2398a0(long controller_index)
{
	long string_ids[3];

	string_ids[0] = 0x1200068e;
	string_ids[1] = 0x1300068f;
	string_ids[2] = 0x10000690;
	function_2b8c05(3, 4, 1 << controller_index, function_2398dc, 0x1000068d, 3, string_ids);
}

dword online_friend_get_flags(const XONLINE_FRIEND *friend_);
bool online_friend_is_in_this_title(const XONLINE_FRIEND *friend_);
void online_friends_answer_game_invite(DWORD controller_index, const XONLINE_FRIEND *friend_, long answer);
bool function_592f0(void);
long function_199f34(void);
void network_session_manager_leave_session_a(bool close);
void network_session_manager_leave_session_b(bool close);
bool network_session_interface_has_user(const XUID *xuid);
void function_199c94(const void *target, long controller, bool flag);
bool function_19a1bd(void);
void function_199e2e(bool close);
void function_148523();
extern bool g_4ee4e0;
c_class_1473c9 *function_149ef3(word user_flags, long load);
c_class_1473c9 *__stdcall function_2b8536(s_screen_parameters *parameters);

/* the screen that joins a friend's game, set to come along with the squad */
struct s_join_screen_view
{
	byte unknown000[0x898];
	bool value898;
};

extern long g_51ec08;
extern long g_51ec0c;

/* joins the friend's game (mode 1 to 4: how a squad already formed comes
   along) */
// @retail 0x2395dc
void __stdcall function_2395dc(XONLINE_FRIEND *friend_, long controller_index, long mode)
{
	dword flags = online_friend_get_flags(friend_);
	bool in_session = function_592f0();
	long players = function_199f34();
	long state = function_0592d0();
	bool in_this_title = online_friend_is_in_this_title(friend_);
	long join_type = 1;

	if (!(flags & 1))
	{
		dialog_ok_show(1, 0x3e, 4, 1 << controller_index, 0, 0);
		return;
	}
	switch (state)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 9:
	{
		if (!in_session && mode != 4)
		{
			if (players > 1)
			{
				if (mode != 1)
				{
					function_239843(controller_index);
					return;
				}
				join_type = 2;
				goto leave;
			}
		}
		else if (players > 1)
		{
			if (mode == 2)
			{
				join_type = 3;
			}
			else if (mode == 4)
			{
			leave:
				g_4ee4e0 = true;
				if (in_this_title)
				{
					network_session_manager_leave_session_a(true);
					network_session_manager_leave_session_b(true);
				}
			}
			else if (mode == 3)
			{
				((s_join_screen_view *)function_149ef3((word)(1 << controller_index), (long)function_2b8536))->value898 = true;
				return;
			}
			else
			{
				function_2398a0(controller_index);
				return;
			}
		}

		bool join_pending = ((s_online_selection *)&g_54d598.settings)->join_pending;

		if (flags & 0xc0)
		{
			online_friends_answer_game_invite(controller_index, friend_, 0);
		}
		if (!in_this_title)
		{
			if (join_pending)
			{
				((s_online_selection *)&g_54d598.settings)->join_pending = false;
				XOnlineFriendsJoinGame(controller_index, friend_);
			}
			function_2397b9(controller_index);
			return;
		}

		FILETIME none;
		bool invited;

		memset(&none, 0, sizeof(none));
		invited = memcmp(&friend_->gameinviteTime, &none, sizeof(none)) != 0;
		function_148523();
		if (state == 3)
		{
			g_54d598.m0c = join_type;
			if (in_session && join_type == 3)
			{
				function_19a1bd();
			}
			else
			{
				g_4ee4e0 = true;
				function_199e2e(true);
			}
		}
		else
		{
			if (!network_session_interface_has_user(&friend_->xuid))
			{
				g_51ec08 = 0;
				g_51ec0c = 0;
			}
			function_199c94(friend_, controller_index, invited);
		}
		break;
	}
	case 6:
	case 7:
	case 8:
		function_23982a(controller_index);
		break;
	}
}
/* joins the selected friend's game through Live */
// @retail 0x238eb5
void __stdcall function_238eb5(long controller_index, long mode)
{
	s_online_selection selection;

	function_14887e((s_screen_settings_54dc6c *)&selection);
	if (selection.type == 2)
	{
		((s_online_selection *)&g_54d598.settings)->join_pending = true;
		function_2395dc(&selection.friend_, controller_index, mode);
	}
}

// @retail 0x238ea7
void __stdcall function_238ea7(long controller_index)
{
	function_238eb5(controller_index, 0);
}
struct s_name_request;
void __stdcall function_148893(s_name_request *request, long mode);
struct s_online_game_invite;
void online_game_invite_answer(DWORD controller_index, const s_online_game_invite *invite, long answer);

/* a game invite message: its title and when it was sent */
struct s_invite_message_view
{
	byte unknown00[0x24];
	DWORD title_id;
	byte unknown28[0x30 - 0x28];
	FILETIME time;
};

/* joins the game a message invites the selected player to */
// @retail 0x238f3f
void __stdcall function_238f3f(long controller_index, void *message, unsigned __int64 session_id)
{
	void **message_reference = &message;
	s_online_selection selection;
	s_invite_message_view *invite;

	((s_online_selection *)&g_54d598.settings)->join_pending = false;
	function_14887e((s_screen_settings_54dc6c *)&selection);
	invite = (s_invite_message_view *)message;
	if (selection.type == 1)
	{
		XONLINE_FRIEND friend_;

		if (!invite || !session_id)
		{
			return;
		}
		online_friend_from_user(&friend_, &selection.user);
		friend_.dwFriendState |= 0x8000001;
		selection.friend_ = friend_;
		*(unsigned __int64 *)&selection.friend_.sessionID = session_id;
		selection.friend_.dwTitleID = invite->title_id;
		selection.friend_.gameinviteTime = invite->time;
		selection.type = 2;
		invite = NULL;
		online_game_invite_answer(controller_index, (s_online_game_invite *)&selection.friend_, 0);
		selection.friend_.dwFriendState = 1;
		function_148893((s_name_request *)&selection, 1);
	}
	if (invite && session_id)
	{
		*(unsigned __int64 *)&selection.friend_.sessionID = session_id;
		selection.friend_.dwTitleID = invite->title_id;
		selection.friend_.gameinviteTime = invite->time;
		function_148893((s_name_request *)&selection, 1);
	}
	function_2395dc(&selection.friend_, controller_index, 0);
}
/* the player a clan task acts on: the id and what follows it */
#pragma pack(push, 4)
struct s_clan_task_target
{
	unsigned __int64 xuid;
	long unknown8;
};
#pragma pack(pop)

s_clan_task_target g_54e420;

long online_team_answer_recruit(XUID const *team, long controller_index, long answer);

/* the clan tasks on the target: invite (0x239300), accept (0x23933a) and
   decline (0x239374) */
// @retail 0x239300
void __stdcall function_239300(long controller_index)
{
	if (g_54e420.xuid)
	{
		long task_index = online_team_answer_recruit((XUID const *)&g_54e420, controller_index, 1);

		if (task_index != NONE)
		{
			function_1487c3(controller_index, task_index, (long)function_1a2cb7, 0, 0);
		}
	}
}

// @retail 0x23933a
void __stdcall function_23933a(long controller_index)
{
	if (g_54e420.xuid)
	{
		long task_index = online_team_answer_recruit((XUID const *)&g_54e420, controller_index, 0);

		if (task_index != NONE)
		{
			function_1487c3(controller_index, task_index, (long)function_1a2d2f, 0, 0);
		}
	}
}

// @retail 0x239374
void __stdcall function_239374(long controller_index)
{
	if (g_54e420.xuid)
	{
		long task_index = online_team_answer_recruit((XUID const *)&g_54e420, controller_index, 2);

		if (task_index != NONE)
		{
			function_1487c3(controller_index, task_index, (long)function_1a2d2f, 0, 0);
		}
	}
}

/* whether a squad can be made now; tells the user why not */
// @retail 0x239abe
bool function_239abe(long controller_index)
{
	bool result = false;

	if (network_session_manager_session_unready() &&
		function_19a935() &&
		!network_session_manager_session_a_established() &&
		network_session_interface_get_value_5dd0() == NONE)
	{
		if (function_0592d0() == 6 || function_0592d0() == 7 || function_0592d0() == 8)
		{
			dialog_ok_show(3, 0x86, 4, 1 << controller_index, 0, 0);
		}
		else if (!function_199fd6())
		{
			dialog_ok_show(3, 0x85, 4, 1 << controller_index, 0, 0);
		}
		else if (function_19a161() == 2)
		{
			dialog_ok_show(3, 0x83, 4, 1 << controller_index, 0, 0);
		}
		else
		{
			result = true;
		}
	}
	else
	{
		dialog_ok_show(3, 0x84, 4, 1 << controller_index, 0, 0);
	}
	return result;
}

/* the clan code's views: the clan (the friend request globals of
   unknown_1a2ca7.cpp hold it) and its members' count, a member */
#pragma pack(push, 2)
struct s_friend_request
{
	XUID xuid;
	byte data[0x69e - sizeof(XUID)];
	dword type;
};
#pragma pack(pop)

struct s_player_identity;
struct s_window_manager_text;
bool friend_request_get(s_friend_request *request);
bool function_1a334a(long index, XUID const *xuid);
bool player_slot_get_identity(long index, s_player_identity *identity);
void function_18ff47(long player, dword *out);
void online_task_screen_finish(c_online_task_screen *screen);
dword function_0b4a20(dword key);
void function_190728(long index);
const char *function_148956(s_window_manager_text *text);
void function_14896e(s_window_manager_754 *a, s_window_manager_df6 *b);
void friends_lists_get_user(XUID const *xuid, bool *arg_a721be, bool *is_player, dword *flags, DWORD *title_id, bool *in_session, XONLINE_FRIEND *field_xb3bdcf);
bool xuid_equal(XUID const *a, XUID const *b, bool compare_guest_number);
void __stdcall online_messages_enumerate(DWORD controller_index, s_entry *entries, long *count);
void online_message_delete(DWORD controller_index, DWORD message_id, bool block_sender);
long online_team_delete(XUID const *team, long controller_index);
long online_team_member_remove(long controller_index, XUID const *team, XUID const *member);
long online_team_member_set_rank(long controller_index, XUID const *team, XONLINE_TEAM_MEMBER const *member);

/* the selection's id */
static inline XUID const *online_selection_get_xuid(s_online_selection const *selection)
{
	XUID const *result = NULL;

	switch (selection->type)
	{
	case 1:
		result = &selection->user.xuid;
		break;
	case 2:
		result = &selection->friend_.xuid;
		break;
	}
	return result;
}

/* the online task screen (unknown_1a2ca7.cpp) as its callbacks read it */
struct s_online_task_screen_view
{
	byte unknown000[0x61c];
	long task_index;
	HRESULT result;
};

/* deletes the messages from the selected player */
// @retail 0x239a3d
void function_239a3d(long controller_index)
{
	s_entry entries[0x7d];
	s_online_selection selection;
	long count = 0x7d;
	XUID const *sender;
	s_entry *entry;

	function_14887e((s_screen_settings_54dc6c *)&selection);
	sender = online_selection_get_xuid(&selection);
	online_messages_enumerate(controller_index, entries, &count);
	for (entry = entries; count != 0; count--, entry++)
	{
		XUID const *entry_sender = entry ? (XUID const *)entry : NULL;

		if ((entry->flags & 0x10000) && xuid_equal(entry_sender, sender, true))
		{
			online_message_delete(controller_index, entry->unknown20, false);
		}
	}
}

/* removes the friend, once the user confirms */
// @retail 0x2397cf
bool __stdcall function_2397cf(long controller_index)
{
	s_online_selection selection;
	XONLINE_FRIEND friend_;

	function_14887e((s_screen_settings_54dc6c *)&selection);
	online_selection_get_friend(&selection, &friend_);
	function_14887e((s_screen_settings_54dc6c *)&selection);
	online_friends_remove(controller_index, &friend_);
	function_239a3d(controller_index);
	return true;
}

// @retail 0x239277
void function_239277(long controller_index)
{
	dialog_choice_show(3, 0x74, 4, 1 << controller_index, function_2397cf, 0, 0);
}

/* when the leave-clan task ends */
// @retail 0x239968
void __stdcall function_239968(c_online_task_screen *screen)
{
	s_online_task_screen_view *view = (s_online_task_screen_view *)screen;
	c_class_1473c9 *widget = (c_class_1473c9 *)screen;
	bool failed = false;

	if (view->task_index != NONE)
	{
		switch (online_task_poll(view->task_index))
		{
		case 2:
			break;
		case 3:
		case 4:
		case 5:
			failed = true;
			break;
		default:
			return;
		}
	}
	online_task_screen_finish(screen);
	if (failed)
	{
		dialog_ok_show(1, function_0b4a20(view->result), 4, widget->user_flags, 0, 0);
	}
	else
	{
		function_239300(widget->get_controller_index());
	}
	function_190728(widget->get_controller_index());
}

/* leaves the clan: removes the user, or deletes the clan when the user is
   its last member */
// @retail 0x23955e
void function_23955e(long controller_index)
{
	XONLINE_USER user;
	s_friend_request identity;

	function_18ff47(controller_index, (dword *)&user);
	if (user.xuid.qwUserID && user.szGamertag[0] &&
		player_slot_get_identity(controller_index, (s_player_identity *)&identity) && identity.xuid.qwUserID)
	{
		long task_index;

		if (identity.type > 1)
		{
			task_index = online_team_member_remove(controller_index, &identity.xuid, &user.xuid);
		}
		else
		{
			task_index = online_team_delete(&identity.xuid, controller_index);
		}
		if (task_index != NONE)
		{
			function_1487c3(controller_index, task_index, (long)function_239968, 0, 0);
		}
	}
}

// @retail 0x23990c
bool __stdcall function_23990c(long controller_index)
{
	s_friend_request request;

	if (friend_request_get(&request))
	{
		bool allowed = function_1a334a(controller_index, &request.xuid);

		if (request.type != 1 && allowed)
		{
			dialog_ok_show(1, 0x96, 4, 1 << controller_index, 0, 0);
			return true;
		}
	}
	function_23955e(controller_index);
	return true;
}

// @retail 0x239292
void function_239292(long controller_index)
{
	s_friend_request request;

	if (friend_request_get(&request))
	{
		bool allowed = function_1a334a(controller_index, &request.xuid);

		if (request.type != 1 && allowed)
		{
			dialog_ok_show(1, 0x96, 4, 1 << controller_index, 0, 0);
		}
		else
		{
			dialog_choice_show(1, 0x7a, 4, 1 << controller_index, function_23990c, 0, 0);
		}
	}
	else
	{
		function_239300(controller_index);
	}
}

/* makes the selected member the clan's leader, once the user confirms */
// @retail 0x2399da
bool __stdcall function_2399da(long controller_index)
{
	s_friend_request team;
	XONLINE_TEAM_MEMBER member;
	long task_index;

	function_14896e((s_window_manager_754 *)&team, (s_window_manager_df6 *)&member);
	player_slot_get_identity(controller_index, (s_player_identity *)&team);
	member.TeamMemberProperties.dwPrivileges = 3;
	task_index = online_team_member_set_rank(controller_index, &team.xuid, &member);
	if (task_index != NONE)
	{
		function_1487c3(controller_index, task_index, (long)function_1a2d2f, 0, 0);
	}
	return true;
}

/* sets the selected player's rank in the clan (NONE removes them) */
// @retail 0x2393ae
void __stdcall function_2393ae(long controller_index, long rank)
{
	s_friend_request team;
	s_window_manager_754 selected_team;
	s_online_selection selection;
	XONLINE_TEAM_MEMBER member;
	XONLINE_TEAM_MEMBER membership;
	XUID const *xuid;
	const char *name;
	bool arg_a721be;
	bool is_player;
	dword flags;
	DWORD title_id;
	bool in_session;
	bool have_member;
	bool may_change;

	if (friend_request_get(&team))
	{
		function_14887e((s_screen_settings_54dc6c *)&selection);
		xuid = online_selection_get_xuid(&selection);
		name = function_148956((s_window_manager_text *)&selection);
		function_14896e(&selected_team, (s_window_manager_df6 *)&member);
		have_member = true;
		if (!member.xuidTeamMember.qwUserID)
		{
			have_member = false;
		}
		friends_lists_get_user(xuid, &arg_a721be, &is_player, &flags, &title_id, &in_session, NULL);
		may_change = (bool)((flags >> 11) & 1);
		if (!have_member)
		{
			if (may_change)
			{
				member.xuidTeamMember = *xuid;
				strncpy(member.szGamertag, name, 0x10);
				member.szGamertag[0xf] = 0;
			}
			else
			{
				dialog_ok_show(1, 0x6e, 4, 1 << controller_index, 0, 0);
				return;
			}
		}
		{
			function_18ffc3(controller_index, (s_player_slot_blockb82 *)&membership);
			if (xuid && xuid->qwUserID)
			{
				long own_rank = membership.TeamMemberProperties.dwPrivileges;

				if (may_change || own_rank >= 2 && own_rank > (long)member.TeamMemberProperties.dwPrivileges)
				{
					long task_index;

					if (rank == NONE)
					{
						task_index = online_team_member_remove(controller_index, &team.xuid, xuid);
					}
					else if (rank > own_rank)
					{
						dialog_ok_show(1, 0x5d, 4, 1 << controller_index, 0, 0);
						return;
					}
					else if (rank == 3)
					{
						dialog_choice_show(3, 0x75, 4, 1 << controller_index, function_2399da, 0, 0);
						return;
					}
					else
					{
						member.TeamMemberProperties.dwPrivileges = rank;
						task_index = online_team_member_set_rank(controller_index, &team.xuid, &member);
					}
					if (task_index != NONE)
					{
						function_1487c3(controller_index, task_index, (long)function_1a2d2f, 0, 0);
					}
				}
				else
				{
					dialog_ok_show(1, 0x5d, 4, 1 << controller_index, 0, 0);
				}
			}
			else
			{
				dialog_ok_show(1, 0x5c, 4, 1 << controller_index, 0, 0);
			}
		}
	}
	else
	{
		dialog_ok_show(1, 0x5c, 4, 1 << controller_index, 0, 0);
	}
}
