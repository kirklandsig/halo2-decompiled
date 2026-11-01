#include "unknown_4cf790.h"
// @flags /O1 /Ob1 /Oi /arch:SSE /Gr
/* UNKNOWN_1A2CA7.CPP: the screen that waits for an online task (vtable
   0x4549c0: it shows the task's title and description and calls back when
   the task finishes or is cancelled), and the friends list globals */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include "data_array.h"
#include "screen_widgets.h"
#include "unknown_19b516.h"
#include "unknown_19b510.h"
#include "online_tasks.h"
#include "online_friends.h"
#include "unknown_2b116a.h"
#include "unknown_18f576.h"
#include "online_message_entries.h"
#include "unknown_2312b4.h"
#include "unknown_x8d43e5.h"
#include "globals.h"

#include "online_task_screen.h"

/* the last friend request (0x6a2 bytes) */
struct s_friend_request
{
	byte data[0x6a2];
};

/* a friend's details (12 bytes) */
struct s_friend_details
{
	dword unknown0;
	dword unknown4;
	dword unknown8;
};

/* a friend of the friends list (0xa4 bytes) */
struct s_friend
{
	byte unknown00[2];
	short unknown02;
	union
	{
		XUID xuid;
		struct
		{
			dword unknown04;
			dword unknown08;
			dword unknown0c;
		};
	};
	union
	{
		char gamertag[16];
		bool unknown10;
	};
	union
	{
		dword flags20;
		struct
		{
			byte unknown20;
			byte : 2;
			byte flags21_2 : 1;
			byte flags21_3 : 1;
			byte : 4;
		};
	};
	XNKID session_id;
	DWORD title_id;
	s_friend_details details;
	word name[48];
	byte state_data_size;
	byte state_data[4];
	byte unknowna1[0xa4 - 0xa1];
};

/* a player of the players list (0xac bytes) */
struct s_friend_player : s_friend
{
	long state;
	dword flagsa8;
};

/* a player as the online service reports it (0x92 bytes) */
#pragma pack(push, 1)
struct s_online_player
{
	XUID xuid;
	char gamertag[16];
	long state;
	byte unknown20[0x86 - 0x20];
	union
	{
		dword flags;
		struct
		{
			dword flags_0 : 1;
			dword : 31;
		};
		struct
		{
			dword : 2;
			dword flags_2 : 1;
			dword : 29;
		};
	};
	byte unknown8a[0x92 - 0x8a];
};
#pragma pack(pop)

/* an entry of the list of player references (8 bytes) */
struct s_friend_player_reference
{
	byte unknown0[4];
	long player_index;
};

s_online_player_data_globals g_global_4acf62 =
{
	NONE, NULL, NULL, NULL, NULL, NULL, NONE, NONE, NONE, 0, NONE, NONE, NONE
};
byte g_54eae8[4][0xc70];

void function_18fe9e(long gamepad_index);
void function_190728(long index);
void function_24bac5(void *a);
bool xuid_equal(XUID const *a, XUID const *b, bool compare_guest_number);
void online_get_title_name(DWORD title_id, WCHAR *name, long name_length);
HRESULT online_task_continue(s_type_9df9da *task);
void function_6b640(long task_index);
bool function_6c7e0();
dword online_result_get_error_string(dword key);
void unicode_string_copy(word *destination, const word *source, long maximum_count);
c_class_1473c9 *__stdcall online_task_screen_load(s_screen_parameters *parameters);
void function_1a3294();

/* ---- the screen ---- */

// @retail 0x1a2f70
void online_task_screen_dispose_task(c_online_task_screen *screen)
{
	if (screen->task_index != NONE)
	{
		s_type_9df9da *task = function_6b910(screen->task_index);

		if (task)
			screen->result = online_task_continue(task);
		else
			screen->result = E_FAIL;
		function_6b640(screen->task_index);
		screen->task_index = NONE;
	}
}

// @retail 0x1a2fb4
void online_task_screen_finish(c_online_task_screen *screen)
{
	screen->finished = NULL;
	online_task_screen_dispose_task(screen);
}

// @retail 0x1a2d8a
void online_task_screen_end(c_online_task_screen *screen, bool show_error)
{
	bool failed = false;

	if (screen->task_index != NONE)
	{
		switch (online_task_poll(screen->task_index))
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
	if (failed && show_error)
		dialog_ok_show(1, online_result_get_error_string(screen->result), 4, screen->user_flags, 0, 0);
}

// @retail 0x1a2ca7
void __stdcall online_task_screen_end_with_error(c_online_task_screen *screen)
{
	online_task_screen_end(screen, true);
}

// @retail 0x1a2cb7
void __stdcall function_1a2cb7(c_online_task_screen *screen)
{
	bool failed = false;

	if (screen->task_index != NONE)
	{
		switch (online_task_poll(screen->task_index))
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
		dialog_ok_show(1, online_result_get_error_string(screen->result), 4, screen->user_flags, 0, 0);
	function_190728(screen->get_controller_index());
	function_1a3294();
	function_18fe9e(screen->get_controller_index());
}

// @retail 0x1a2d2f
void __stdcall function_1a2d2f(c_online_task_screen *screen)
{
	bool failed = false;

	if (screen->task_index != NONE)
	{
		switch (online_task_poll(screen->task_index))
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
		dialog_ok_show(1, online_result_get_error_string(screen->result), 4, screen->user_flags, 0, 0);
	function_1a3294();
}

// @retail 0x1a2de1
screen_load_proc c_online_task_screen::get_load_proc()
{
	return online_task_screen_load;
}

// @retail 0x1a2de7
c_class_1473c9 *__stdcall online_task_screen_load(s_screen_parameters *parameters)
{
	c_online_task_screen *screen = new c_online_task_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x1a2e23
c_online_task_screen::c_online_task_screen(long a, long b, word user_flags) :
	c_class_1473c9(0xb6, a, b, user_flags)
{
	task_index = NONE;
	finished = NULL;
	cancelled = NULL;
	result = S_OK;
}

// @retail 0x1a2e63 deleting c_online_task_screen

// @retail 0x1a2e7f
c_online_task_screen::~c_online_task_screen()
{
	if (function_6c7e0() && task_index != NONE)
	{
		s_type_9df9da *task = function_6b910(task_index);

		if (task && task->type == 1)
		{
			long index = get_controller_index();

			if (index >= 0 && index <= 3)
				function_24bac5(g_54eae8[index]);
		}
	}
	online_task_screen_dispose_task(this);
}

// @retail 0x1a2edb
bool c_online_task_screen::v10(s_widget_event *event)
{
	if (event->type == 5 && (event->param == 1 || event->param == 13) && cancelled)
	{
		cancelled(this);
		online_task_screen_finish(this);
		return true;
	}
	return false;
}

// @retail 0x1a2f12
void c_online_task_screen::v3()
{
	if (task_index != NONE)
	{
		set_title(online_task_get_title(task_index));
		set_description(online_task_get_description(task_index));
	}
	if (finished)
	{
		finished(this);
	}
	else if (!((bool)(((dword)animation.valuee >> 1) & 1)))
	{
		online_task_screen_dispose_task(this);
		function_22e957(3);
	}
	((c_widget *)this)->function_22e391();
}

// @retail 0x1a2fc5
void c_online_task_screen::set_title(long string_handle)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)find_child(6, 0, false);

	if (text)
		text->function_253b1a(string_handle);
}

// @retail 0x1a2fe4
void c_online_task_screen::set_description(long string_handle)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)find_child(6, 2, false);

	if (text)
		text->function_253b1a(string_handle);
}

void function_253c3a(long block_index, long index, c_text_widget_45a5e0 *widget);

// @retail 0x1a3003
void c_online_task_screen::set_title_from_block(long block_index, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)find_child(6, 0, false);

	if (text)
		function_253c3a(block_index, index, text);
}

// @retail 0x1a301f
void c_online_task_screen::set_description_from_block(long block_index, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)find_child(6, 2, false);

	if (text)
		function_253c3a(block_index, index, text);
}

/* ---- the friends list ---- */

// @retail 0x1a3230
bool friends_list_task_running()
{
	bool result = false;

	if (g_global_4acf62.friends_task_index != NONE && g_global_4acf62.field_4_4)
	{
		long status = online_task_poll(g_global_4acf62.friends_task_index);

		if (status == 1 || status == 2)
			result = true;
	}
	return result;
}

// @retail 0x1a325a
bool function_1a325a()
{
	return g_global_4acf62.friend_request.valid ? g_global_4acf62.friend_request.unknown6a3 : true;
}

// @retail 0x1a3269
bool friend_request_get(s_friend_request *request)
{
	if (g_global_4acf62.friend_request.valid)
		*request = *(s_friend_request *)&g_global_4acf62.friend_request.request;
	else
		memset(request, 0, sizeof(s_friend_request));
	return g_global_4acf62.friend_request.valid;
}

// @retail 0x1a3294
void function_1a3294()
{
	g_global_4acf62.friend_request.valid = false;
	memset(&g_global_4acf62.friend_request.request, 0, sizeof(s_friend_request));
}

// @retail 0x1a32ae
bool friends_list_contains(XUID const *xuid)
{
	bool result = false;

	if (g_global_4acf62.field_4_4 && xuid->qwUserID)
	{
		s_list_item_iterator iterator;

		iterator.iterator.index = NONE;
		iterator.iterator.datum_index = NONE;
		iterator.iterator.data = g_global_4acf62.field_4_4;
		while (function_2b2327(&iterator) && !result)
			result = xuid_equal((XUID const *)(iterator.item + 4), xuid, false);
	}
	return result;
}

// @retail 0x1a32fc
bool players_list_contains(XUID const *xuid)
{
	bool result = false;

	if (g_global_4acf62.field_8_2 && xuid->qwUserID)
	{
		s_list_item_iterator iterator;

		iterator.iterator.index = NONE;
		iterator.iterator.datum_index = NONE;
		iterator.iterator.data = g_global_4acf62.field_8_2;
		while (function_2b2327(&iterator) && !result)
			result = xuid_equal((XUID const *)(iterator.item + 4), xuid, false);
	}
	return result;
}

// @retail 0x1a334a
bool function_1a334a(long index, XUID const *xuid)
{
	s_player_slot_blockb82 block;
	bool result = false;

	if (function_18ffc3(index, &block) && *(long *)&block.data[0x1c] == 3 && g_global_4acf62.field_8_2)
	{
		s_list_item_iterator iterator;

		result = true;
		iterator.iterator.index = NONE;
		iterator.iterator.datum_index = NONE;
		iterator.iterator.data = g_global_4acf62.field_8_2;
		while (function_2b2327(&iterator))
		{
			byte *player = iterator.item;

			if (((XUID *)(player + 4))->qwUserID && *(long *)(player + 0xa4) == 3 &&
				xuid_equal((XUID const *)(player + 0x30), xuid, false))
			{
				result = false;
				break;
			}
		}
	}
	return result;
}

// @retail 0x1a353a
void title_name_get(WCHAR *name, long name_length, DWORD title_id)
{
	name[0] = 0;
	if (title_id)
		online_get_title_name(title_id, name, name_length);
}

// @retail 0x1a35c8
void friends_list_reset(bool dispose)
{
	g_global_4acf62.controller_index = NONE;
	if (dispose)
	{
		if (g_global_4acf62.field_4_4)
		{
			data_dispose(g_global_4acf62.field_4_4);
			g_global_4acf62.field_4_4 = NULL;
		}
		if (g_global_4acf62.field_8_2)
		{
			data_dispose(g_global_4acf62.field_8_2);
			g_global_4acf62.field_8_2 = NULL;
		}
		if (g_global_4acf62.clan_member_reference_data)
		{
			data_dispose(g_global_4acf62.clan_member_reference_data);
			g_global_4acf62.clan_member_reference_data = NULL;
		}
		if (g_global_4acf62.field_10_3)
		{
			data_dispose(g_global_4acf62.field_10_3);
			g_global_4acf62.field_10_3 = NULL;
		}
		if (g_global_4acf62.field_14)
		{
			data_dispose(g_global_4acf62.field_14);
			g_global_4acf62.field_14 = NULL;
			g_global_4acf62.friend_request.unknown6a3 = false;
		}
	}
	else
	{
		record_pool_release_all(g_global_4acf62.field_4_4);
		record_pool_release_all(g_global_4acf62.field_8_2);
		record_pool_release_all(g_global_4acf62.clan_member_reference_data);
		g_global_4acf62.friend_request.unknown6a3 = false;
	}
	if (g_global_4acf62.presence_task_index != NONE)
	{
		function_6b640(g_global_4acf62.presence_task_index);
		g_global_4acf62.presence_task_index = NONE;
	}
	if (g_global_4acf62.friends_task_index != NONE)
	{
		function_6b640(g_global_4acf62.friends_task_index);
		g_global_4acf62.friends_task_index = NONE;
	}
	if (g_global_4acf62.clan_members_task_index != NONE)
	{
		function_6b640(g_global_4acf62.clan_members_task_index);
		g_global_4acf62.clan_members_task_index = NONE;
	}
	if (g_global_4acf62.task_index_e0 != NONE)
	{
		function_6b640(g_global_4acf62.task_index_e0);
		g_global_4acf62.task_index_e0 = NONE;
	}
	if (g_global_4acf62.task_index_e4 != NONE)
	{
		function_6b640(g_global_4acf62.task_index_e4);
		g_global_4acf62.task_index_e4 = NONE;
	}
	if (g_global_4acf62.task_index_e8 != NONE)
	{
		function_6b640(g_global_4acf62.task_index_e8);
		g_global_4acf62.task_index_e8 = NONE;
	}
	memset(&g_global_4acf62.xuid_ec, 0, sizeof(g_global_4acf62.xuid_ec));
	memset(&g_global_4acf62.xuid_f8, 0, sizeof(g_global_4acf62.xuid_f8));
	memset(&g_global_4acf62.recent_player_xuid, 0, sizeof(g_global_4acf62.recent_player_xuid));
	memset(&g_global_4acf62.friend_request.request, 0, sizeof(s_friend_request));
	g_global_4acf62.start_time = 0;
	g_global_4acf62.friend_request.valid = false;
}

// @retail 0x1a43eb
void friends_player_new()
{
	long player_index = record_pool_allocate(g_global_4acf62.field_8_2);
	long reference_index;
	s_friend_player *player = (s_friend_player *)(g_global_4acf62.field_8_2->data + (player_index & 0xffff) * sizeof(s_friend_player));

	reference_index = record_pool_allocate(g_global_4acf62.clan_member_reference_data);
	s_friend_player_reference *reference = (s_friend_player_reference *)(g_global_4acf62.clan_member_reference_data->data + (reference_index & 0xffff) * sizeof(s_friend_player_reference));

	player->unknown04 = 0;
	player->unknown08 = 0;
	player->unknown10 = false;
	player->name[0] = 0;
	reference->player_index = player_index;
}

// @retail 0x1a460f
bool friend_details_get(XUID const *xuid, s_friend_details *details)
{
	bool result = false;

	if (xuid && xuid->qwUserID && details && g_global_4acf62.field_10_3)
	{
		s_list_item_iterator iterator;

		iterator.iterator.index = NONE;
		iterator.iterator.datum_index = NONE;
		iterator.iterator.data = g_global_4acf62.field_10_3;
		while (function_2b2327(&iterator))
		{
			XUID entry_xuid;

			entry_xuid.qwUserID = *(ULONGLONG *)iterator.item;
			entry_xuid.dwUserFlags = 0;
			if (xuid_equal(xuid, &entry_xuid, false))
			{
				*details = *(s_friend_details *)(iterator.item + 8);
				result = true;
				break;
			}
		}
	}
	return result;
}

// @retail 0x1a4692
bool friend_name_get(XUID const *xuid, word *name)
{
	bool result = false;

	if (xuid && xuid->qwUserID && g_global_4acf62.field_14)
	{
		s_list_item_iterator iterator;

		iterator.iterator.index = NONE;
		iterator.iterator.datum_index = NONE;
		iterator.iterator.data = g_global_4acf62.field_14;
		while (function_2b2327(&iterator))
		{
			XUID entry_xuid;

			entry_xuid.qwUserID = *(ULONGLONG *)iterator.item;
			entry_xuid.dwUserFlags = 0;
			if (xuid_equal(xuid, &entry_xuid, false))
			{
				unicode_string_copy(name, (word *)(iterator.item + 8), 16);
				result = true;
				break;
			}
		}
	}
	return result;
}
/* copies at most count characters and terminates the copy */
static inline char *string_copy(char *destination, char const *source, long count)
{
	strncpy(destination, source, count);
	destination[count - 1] = 0;
	return destination;
}

// @retail 0x1a4336
void friends_player_set(s_friend_player *player, s_online_player const *source, long value)
{
	player->unknown02 = value;
	player->xuid = source->xuid;
	string_copy(player->gamertag, source->gamertag, 16);
	player->flags20 = 0;
	player->name[0] = 0;
	if (TEST_FIELD_BIT(source->flags_0))
		player->flags20 = 0x400;
	else
		player->flags20 = 0;
	if (TEST_FIELD_BIT(source->flags_2))
		player->flags21_3 = true;
	else
		player->flags21_3 = false;
	player->flagsa8 = source->flags;
	memset(&player->session_id, 0, sizeof(player->session_id));
	player->title_id = 0;
	if (!friend_details_get(&player->xuid, &player->details))
		memset(&player->details, 0, sizeof(s_friend_details));
	player->state = source->state;
	player->flagsa8 = source->flags;
}


/* the text of a friend's state */
// @retail 0x1a4714
void function_1a4714(long state, word *buffer)
{
	long string_handle = 0;

	switch (state)
	{
	case 0:
		string_handle = 0x40002c6;
		break;
	case 1:
		string_handle = 0x60002c7;
		break;
	case 2:
		string_handle = 0xd0002c8;
		break;
	case 3:
		string_handle = 0x90002c9;
		break;
	}
	function_23620d(string_handle, buffer);
}

DWORD online_friend_flags_get_state(dword flags);

/* a friend or player of the lists as the online service's friend */
// @retail 0x1a3555
void friend_get_online_friend(s_friend const *player, XONLINE_FRIEND *result)
{
	memset(result, 0, sizeof(XONLINE_FRIEND));
	result->xuid = player->xuid;
	string_copy(result->szGamertag, player->gamertag, XONLINE_GAMERTAG_SIZE);
	result->dwFriendState = online_friend_flags_get_state(player->flags20);
	result->sessionID = player->session_id;
	result->dwTitleID = player->title_id;
	result->StateDataSize = player->state_data_size > sizeof(player->state_data) ? sizeof(player->state_data) : player->state_data_size;
	memcpy(result->StateData, player->state_data, sizeof(player->state_data));
}

/* what the friends and players lists know of a user */
// @retail 0x1a33c4
void friends_lists_get_user(XUID const *xuid, bool *arg_a721be, bool *is_player, dword *flags, DWORD *title_id, bool *in_session, XONLINE_FRIEND *field_xb3bdcf)
{
	XNKID no_session = {0};

	*arg_a721be = false;
	*is_player = false;
	*flags = 0;
	*title_id = 0;
	*in_session = false;
	if (field_xb3bdcf)
		memset(field_xb3bdcf, 0, sizeof(XONLINE_FRIEND));

	if (g_global_4acf62.field_4_4 && xuid->qwUserID)
	{
		s_list_item_iterator iterator;

		iterator.iterator.index = NONE;
		iterator.iterator.datum_index = NONE;
		iterator.iterator.data = g_global_4acf62.field_4_4;
		while (function_2b2327(&iterator) && !*arg_a721be)
		{
			s_friend_player *player = (s_friend_player *)iterator.item;

			*arg_a721be = xuid_equal(&player->xuid, xuid, false);
			if (*arg_a721be)
			{
				*flags |= player->flags20;
				*title_id = player->title_id;
				*in_session = memcmp(&player->session_id, &no_session, sizeof(XNKID)) != 0;
				if (field_xb3bdcf)
					friend_get_online_friend(player, field_xb3bdcf);
			}
		}
	}

	if (g_global_4acf62.field_8_2 && xuid->qwUserID)
	{
		s_list_item_iterator iterator;
		bool keep_flags = *arg_a721be && !(*flags & 0x30);

		iterator.iterator.index = NONE;
		iterator.iterator.datum_index = NONE;
		iterator.iterator.data = g_global_4acf62.field_8_2;
		while (function_2b2327(&iterator) && !*is_player)
		{
			s_friend_player *player = (s_friend_player *)iterator.item;

			*is_player = xuid_equal(&player->xuid, xuid, false);
			if (*is_player)
			{
				if (!keep_flags)
					*flags |= player->flags20;
				if (!*arg_a721be)
				{
					*title_id = player->title_id;
					*in_session = memcmp(&player->session_id, &no_session, sizeof(XNKID)) != 0;
					if (field_xb3bdcf)
						friend_get_online_friend(player, field_xb3bdcf);
				}
			}
		}
	}
}

#define NUMBEROF(array) (sizeof(array) / sizeof((array)[0]))

long online_presence_task_new(DWORD controller_index);
void online_presence_add(long task_index, DWORD group_id, DWORD user_count, XUID *users);
void online_presence_submit(long task_index);
void online_presence_task_clear(long task_index);

/* asks for the presence of everyone on the friends and players lists */
// @retail 0x1a4141
void friends_lists_request_presence()
{
	bool submit;

	if (g_global_4acf62.presence_task_index != NONE)
		function_6b640(g_global_4acf62.presence_task_index);
	g_global_4acf62.presence_task_index = online_presence_task_new(g_global_4acf62.controller_index);
	if (g_global_4acf62.presence_task_index == NONE)
		return;

	submit = false;
	online_presence_task_clear(g_global_4acf62.presence_task_index);
	if (g_global_4acf62.field_4_4)
	{
		s_list_item_iterator iterator;

		{
			XUID users[100];
			long user_count = 0;

			iterator.iterator.data = g_global_4acf62.field_4_4;
			iterator.iterator.index = NONE;
			iterator.iterator.datum_index = NONE;
			while (function_2b2327(&iterator) && user_count < NUMBEROF(users))
			{
				s_friend_player *player = (s_friend_player *)iterator.item;

				if (player->xuid.qwUserID)
					users[user_count++] = player->xuid;
			}
			if (user_count > 0)
			{
				online_presence_add(g_global_4acf62.presence_task_index, 'frnd', user_count, users);
				submit = true;
			}
		}
		if (!g_global_4acf62.xuid_ec.qwUserID)
		{
			iterator.iterator.index = NONE;
			iterator.iterator.datum_index = NONE;
			iterator.iterator.data = g_global_4acf62.field_4_4;
			if (function_2b2327(&iterator))
			{
				s_friend_player *player;

				do
				{
					player = (s_friend_player *)iterator.item;
					if (player->xuid.qwUserID)
						break;
				}
				while (function_2b2327(&iterator));
				g_global_4acf62.xuid_ec = player->xuid;
			}
		}
	}
	if (g_global_4acf62.field_8_2)
	{
		s_list_item_iterator iterator;

		{
			XUID users[120];
			long user_count = 0;

			iterator.iterator.data = g_global_4acf62.field_8_2;
			iterator.iterator.index = NONE;
			iterator.iterator.datum_index = NONE;
			while (function_2b2327(&iterator) && user_count < NUMBEROF(users))
			{
				s_friend_player *player = (s_friend_player *)iterator.item;

				if (player->xuid.qwUserID)
					users[user_count++] = player->xuid;
			}
			if (user_count > 0)
			{
				online_presence_add(g_global_4acf62.presence_task_index, 'clan', user_count, users);
				submit = true;
			}
		}
		if (!g_global_4acf62.xuid_f8.qwUserID)
		{
			iterator.iterator.index = NONE;
			iterator.iterator.datum_index = NONE;
			iterator.iterator.data = g_global_4acf62.field_8_2;
			if (function_2b2327(&iterator))
			{
				s_friend_player *player;

				do
				{
					player = (s_friend_player *)iterator.item;
					if (player->xuid.qwUserID)
						break;
				}
				while (function_2b2327(&iterator));
				g_global_4acf62.xuid_f8 = player->xuid;
			}
		}
	}
	if (submit)
		online_presence_submit(g_global_4acf62.presence_task_index);
}

struct s_named_entry;

void __stdcall online_messages_enumerate(DWORD controller_index, s_entry *entries, long *count);
const char *function_08ebc0(s_named_entry *entry);
void ascii_string_to_unicode(const char *source, word *destination, long maximum_count);
void unicode_string_snprintf(word *buffer, long maximum_count, const word *format, ...);

static inline XUID *message_entry_get_xuid(s_entry *entry)
{
	XUID *xuid = NULL;

	if (entry)
		xuid = (XUID *)entry;
	return xuid;
}

/* adds the senders of the player's messages (at most 20) to the players
   list, except those given and those already on it */
// score 81 (was 81): BLOCKED-BY 0x63d00 (Round 113 below). An earlier round re-read the listing
// only. Retail keeps the excluded-XUID
// MiniMax R (11-01): no new form, no lower score; retail keeps the excluded-XUID cursor in EDI
// and passes it to 0x63d00 in EDX, while our LTCG keeps the cursor in EDX and passes it in ECX,
// so the difference is still 0x63d00's parameter order and clobber set in another lane's file.
// score 81 (was 81): this round: the listing was re-read only. Retail keeps the excluded-XUID
// cursor in EDI and spills the sender to [ebp+0x6c]; ours keeps the cursor in EDX and the sender
// in EDI, and retail passes the cursor to 0x63d00 in EDX where ours passes it in ECX.
// Still BLOCKED-BY 0x63d00's parameter order and clobber set, whose file belongs to another lane.
// Earlier, 81 (was 81): this round's five declaration-order trials for the swapped count and
// sender homes: the count after the loop index, the sender held at function scope, the
// exclusion flag declared before the sender, the exclusion flag held at function scope and
// both of them at function scope; four of them score 81 and the flag-first one 80, too small
// a gain to keep. Still the count and sender homes reversed, the sender set by a branch and
// the exclusion cursor kept in EDX across the 0x63d00 call.
// Round 19: unchanged; the added count and the message pointer keep their swapped homes
// (+0x68/+0x6c) and the sender stays in edi because our LTCG keeps the exclusion cursor in edx
// across 0x63d00; still BLOCKED-BY 0x63d00.
// Earlier, 81 (was 81): frame 0x2268 agrees; count/sender homes, nullable sender spill and exclusion cursor remain.
// Earlier: count/sender homes +0x6c/+0x68 remain reversed; nullable getter and exclusion cursor differ.
// Exclusion pointer at use 82; native exclusion result 132; enumeration-count lifetime 82; format buffers swapped 81; getter force-inline declaration 81.
// Byte added count 103; signed/unsigned word exclusion result 128/128; cumulative conditional 113; rejection guards 81.
// Current cumulative-result trials: plain getter 145; result-first plus plain getter 145;
// read-only sender 81; read-only sender plus plain getter 145; native string buffer 75
// (458 bytes, plain; rejected single-array wrapper); buffer plus plain getter 141;
// loop-scoped index 81; indexed cumulative exclusions 81.
// retail keeps the sender in a stack slot (set by a branch, later reused for the
// player) and keeps the getter's null test; without the qualified read the test is folded away.
// Plain message read 145; that with an if-set exclusion 150; if-set exclusion alone 113; the count
// renamed result/sender_count/added 81 each; a message pointer stepped at the body's end 157.
// Shared sender across allocation on cumulative exclusions 135; plain getter 145; literal success 138, plain/literal 150; library copy 135, plain/library 145; readonly/named shared sender 135 each.
// Round 292: Existing equality/membership declarations, ordinary/inline/separate-noinline in all eight
// nonoriginal pairs, 81 each; every provider declaration and body restored.
// BLOCKED-BY 0x63d00: our LTCG knows xuid_equal never writes EDX and keeps the exclusion
// cursor there across the call (sender in EDI); retail reloads `mov edx, edi` each pass and
// spills the sender, as at all 21 retail calls of 0x63d00 (retail does rely on a preserved
// EDX for other callees). Taking 0x63d00's address does not change ours. Round 14: stepped
// message pointer 157; getter through the field at +0 145; ternary getter 145 (VC folds
// `p ? p : NULL` to p, so only the qualified read keeps retail's test); sender reused for
// the copy 135.
// Round 50: account-entry, added player and account-id role names 81 each; split message and
// player declarations 81 each; inline reference allocation 81; account-copy buffer name 81;
// exclusion declarations swapped 81. Opening body restored.
// Round 113 (lane 8, fast with 0x63d00): `if (xuid_equal(...)) is_excluded = true;` (listed, 113) gives
// retail's exclusion-loop body exactly, so the rest is allocation: retail treats 0x63d00 as clobbering EDX
// (cursor in EDI, `mov edx, edi` per call, sender spilled to +0x6c and reloaded into ECX), ours keeps the
// cursor in EDX. 0x63d00 takes ECX, EDX and one stack byte, the /Gr fastcall shape, and every retail caller
// reloads EDX after it: it looks like a /GL- object in retail (a flags lead for the user; its file is
// another lane's). Forms: `is_excluded |= xuid_equal(...)` 140 (446 bytes); the loop as a while with the
// steps in its body 81 (same code); the if-set form with the flag declared after the cursor locals 113, with
// the sender at function scope 113; the excluded id as the first argument 81.
// Round 25 (lane 17, fast): a message pointer stepped at the body's end without the qualified read 157 (the
// getter's null test still folds); the getter defined out of line in another file and inlined by the link
// (diagnostic) 145, test still folded: ours knows the message cursor is non-null, retail tests it at both
// getter uses (`test ecx, ecx; cmovne esi, ecx` before the copy); an indexed if-set exclusion loop 135 (448
// bytes); the cursor, remaining and flag declared in reverse 81; `messages + i` 81.
// @retail 0x1a4441
long players_list_add_message_senders(XUID const *excluded, long excluded_count)
{
	s_entry messages[125];
	long message_count = NUMBEROF(messages);
	long added_count = 0;
	long i;

	online_messages_enumerate(g_global_4acf62.controller_index, messages, &message_count);
	for (i = 0; i < message_count && added_count < 20; i++)
	{
		s_entry *message = &messages[i];

		if (TEST_FIELD_BIT(message->flag_bits.flag12))
		{
			XUID *xuid = message_entry_get_xuid(*(s_entry *volatile *)&message);
			bool is_excluded = false;
			long remaining = excluded_count;
			XUID const *excluded_xuid = excluded;

			for (; remaining > 0; --remaining, ++excluded_xuid)
			{
				is_excluded = xuid_equal(xuid, excluded_xuid, false) || is_excluded;
			}
			if (!is_excluded && !players_list_contains(xuid))
			{
				long player_index = record_pool_allocate(g_global_4acf62.field_8_2);
				s_friend_player *player = (s_friend_player *)(g_global_4acf62.field_8_2->data + (player_index & 0xffff) * sizeof(s_friend_player));
				long reference_index = record_pool_allocate(g_global_4acf62.clan_member_reference_data);
				s_online_player local_446bcb;
				word name[16];

				((s_friend_player_reference *)(g_global_4acf62.clan_member_reference_data->data + (reference_index & 0xffff) * sizeof(s_friend_player_reference)))->player_index = player_index;
				memset(&local_446bcb, 0, sizeof(local_446bcb));
				local_446bcb.xuid = *message_entry_get_xuid(message);
				string_copy(local_446bcb.gamertag, function_08ebc0((s_named_entry *)message), 16);
				local_446bcb.flags = 1;
				friends_player_set(player, &local_446bcb, added_count);
				if (friend_name_get((XUID const *)&player->details, name))
				{
					word format[256];
					word gamertag[48];

					format[0] = 0;
					ascii_string_to_unicode(player->gamertag, gamertag, NUMBEROF(gamertag));
					function_23620d(0x220006bd, format);
					unicode_string_snprintf(player->name, NUMBEROF(player->name), format, gamertag, name);
				}
				else
				{
					ascii_string_to_unicode(player->gamertag, player->name, NUMBEROF(player->name));
				}
				player->flags21_2 = true;
				added_count++;
			}
		}
	}
	return added_count;
}

short online_friends_get_latest(long task_index, XONLINE_FRIEND *friends);

static inline XUID const *online_friend_get_xuid(XONLINE_FRIEND const *field_xb3bdcf)
{
	XUID const *xuid = NULL;

	if (field_xb3bdcf)
		xuid = &field_xb3bdcf->xuid;
	return xuid;
}

static inline char const *online_friend_get_gamertag(XONLINE_FRIEND const *field_xb3bdcf)
{
	char const *gamertag = "";

	if (field_xb3bdcf)
		gamertag = field_xb3bdcf->szGamertag;
	return gamertag;
}

/* rebuilds the friends list from the friends task's latest results */
// @retail 0x1a3728
void friends_list_update()
{
	if (g_global_4acf62.friends_task_index == NONE || !g_global_4acf62.field_4_4)
		return;

	long status = online_task_poll(g_global_4acf62.friends_task_index);

	if (status == 1 || status == 2)
	{
		XONLINE_FRIEND online_friends[MAX_FRIENDS];
		long friend_count = (word)online_friends_get_latest(g_global_4acf62.friends_task_index, online_friends);
		long previous_count = g_global_4acf62.field_4_4->actual_count - 1;
		long i;

		record_pool_release_all(g_global_4acf62.field_4_4);
		if (friend_count > 0)
		{
			for (i = 0; i < friend_count; i++)
			{
				XONLINE_FRIEND *field_xb3bdcf = &online_friends[i];
				s_friend *friend_ = (s_friend *)(g_global_4acf62.field_4_4->data + (record_pool_allocate(g_global_4acf62.field_4_4) & 0xffff) * sizeof(s_friend));
				s_online_friend copy;
				word name[16];
	
				online_friend_copy(field_xb3bdcf, &copy);
				friend_->unknown02 = (short)i;
				friend_->xuid = *online_friend_get_xuid(field_xb3bdcf);
				string_copy(friend_->gamertag, online_friend_get_gamertag(field_xb3bdcf), 16);
				ascii_string_to_unicode(friend_->gamertag, friend_->name, NUMBEROF(friend_->name));
				friend_->flags20 = copy.flags;
				friend_->session_id = copy.session_id;
				friend_->title_id = copy.title_id;
				friend_->state_data_size = field_xb3bdcf->StateDataSize;
				memcpy(friend_->state_data, field_xb3bdcf->StateData, sizeof(friend_->state_data));
				if (!friend_details_get(&friend_->xuid, &friend_->details))
				{
					memset(&friend_->details, 0, sizeof(friend_->details));
				}
				else if (friend_name_get((XUID const *)&friend_->details, name))
				{
					s_global_string_buffer format;
					word gamertag[48];
	
					format.text[0] = 0;
					ascii_string_to_unicode(friend_->gamertag, gamertag, NUMBEROF(gamertag));
					format.load(0x220006bd);
					unicode_string_snprintf(friend_->name, NUMBEROF(friend_->name), format.text, gamertag, name);
				}
			}
			if (friend_count != previous_count)
				friends_lists_request_presence();
		}

		long last_index = record_pool_allocate(g_global_4acf62.field_4_4);
		s_friend *last = (s_friend *)(g_global_4acf62.field_4_4->data + (last_index & 0xffff) * sizeof(s_friend));

		last->unknown04 = 0;
		last->unknown08 = 0;
		last->unknown10 = false;
		last->name[0] = 0;
	}
	else if (status != 0)
	{
		function_6b640(g_global_4acf62.friends_task_index);
		g_global_4acf62.friends_task_index = NONE;
	}
}

struct s_player_identity;
bool player_slot_get_identity(long index, s_player_identity *identity);
long online_friends_enumerate(DWORD controller_index);
long online_team_members_enumerate(long controller_index, XUID const *team);
long function_75870(void);
bool player_configuration_cache_next_recent_player(s_recent_player *player, long *iterator);
extern dword g_54d5b8;


/* creates the lists of the controller's friends and team members and starts
   the tasks that fill them */
// @retail 0x1a303b
void function_1a303b(long controller_index)
{
	s_recent_player player;
	long iterator;

	g_global_4acf62.friend_request.valid = player_slot_get_identity(controller_index, (s_player_identity *)&g_global_4acf62.friend_request.request);
	g_global_4acf62.controller_index = controller_index;
	if (!g_global_4acf62.field_4_4)
	{
		g_global_4acf62.field_4_4 = user_interface_data_new("friends list", 0x65, sizeof(s_friend));
		if (g_global_4acf62.field_4_4)
			function_16b790(g_global_4acf62.field_4_4);
	}

	long last_index = record_pool_allocate(g_global_4acf62.field_4_4);
	s_friend *last = (s_friend *)(g_global_4acf62.field_4_4->data + (last_index & 0xffff) * sizeof(s_friend));

	last->unknown04 = 0;
	last->unknown08 = 0;
	last->unknown10 = false;
	last->name[0] = 0;
	if (!g_global_4acf62.field_8_2)
	{
		g_global_4acf62.field_8_2 = user_interface_data_new("clan members list", 0x79, sizeof(s_friend_player));
		if (g_global_4acf62.field_8_2)
			function_16b790(g_global_4acf62.field_8_2);
	}
	if (!g_global_4acf62.clan_member_reference_data)
	{
		g_global_4acf62.clan_member_reference_data = user_interface_data_new("clan members list", 0x79, sizeof(s_friend_player_reference));
		if (g_global_4acf62.clan_member_reference_data)
			function_16b790(g_global_4acf62.clan_member_reference_data);
	}
	if (g_global_4acf62.field_8_2 && g_global_4acf62.clan_member_reference_data)
		friends_player_new();
	if (!g_global_4acf62.field_10_3)
	{
		g_global_4acf62.field_10_3 = user_interface_data_new("player xuid - clan xuid mapping", 300, 0x18);
		if (g_global_4acf62.field_10_3)
			function_16b790(g_global_4acf62.field_10_3);
	}
	if (!g_global_4acf62.field_14)
	{
		g_global_4acf62.field_14 = user_interface_data_new("clan display data", 300, 0x28);
		if (g_global_4acf62.field_14)
			function_16b790(g_global_4acf62.field_14);
	}
	g_global_4acf62.presence_task_index = online_presence_task_new(controller_index);
	g_global_4acf62.friends_task_index = online_friends_enumerate(controller_index);
	if (g_global_4acf62.friend_request.valid)
	{
		g_global_4acf62.clan_members_time = function_75870();
		g_global_4acf62.clan_members_task_index = online_team_members_enumerate(controller_index, (XUID const *)&g_global_4acf62.friend_request.request);
	}
	else
	{
		g_global_4acf62.clan_members_time = 0;
	}
	g_global_4acf62.start_time = g_54d5b8;
	iterator = g_4cf984;
	while (player_configuration_cache_next_recent_player(&player, &iterator))
	{
		XUID const *xuid = (XUID const *)&player;

		if (xuid->qwUserID && !XOnlineIsUserGuest(xuid->dwUserFlags) && (xuid->qwUserID >> 48) != 0xfefe &&
			*(word const *)&player.unknown08[4])
		{
			g_global_4acf62.recent_player_xuid = *xuid;
			break;
		}
	}
}

typedef bool (__stdcall *t_compare_function)(const void *, const void *, const void *);
void function_13da70(void *elements, unsigned long count, unsigned long element_size, t_compare_function compare, const void *context);

static inline s_friend_player *clan_member_reference_get_player(void const *reference)
{
	return (s_friend_player *)(g_global_4acf62.field_8_2->data +
		(((s_friend_player_reference const *)reference)->player_index & 0xffff) * sizeof(s_friend_player));
}

/* the order of the team members list: whether a goes after b */
// score 79 (was 79): retail holds the first flag word in EDX and spills the
// extracted bits to [ebp-2]..[ebp-7] with the result at [ebp-8]; ours keeps
// the word in EBX, uses other slots and reads the low bytes again.
// @retail 0x1a391f
bool __stdcall clan_member_compare(void const *a, void const *b, void const *context)
{
	s_friend_player *player_a = clan_member_reference_get_player(a);
	s_friend_player *player_b = clan_member_reference_get_player(b);
	bool result = false;
	dword flags_a = player_a->flags20;
	bool a9 = (bool)((flags_a >> 9) & 1);
	dword flags_b = player_b->flags20;
	bool b9 = (bool)((flags_b >> 9) & 1);
	bool a_c0 = false;
	bool b_c0;

	if (flags_a & 0xc0)
		a_c0 = true;
	b_c0 = false;
	if (flags_b & 0xc0)
		b_c0 = true;

	bool a11 = (bool)((flags_a >> 11) & 1);
	bool b11 = (bool)((flags_b >> 11) & 1);
	bool a10 = (bool)((flags_a >> 10) & 1);
	bool b10 = (bool)((flags_b >> 10) & 1);
	bool a0 = (bool)(player_a->unknown20 & 1);
	bool b0 = (bool)(player_b->unknown20 & 1);

	if (a10 || b10)
	{
		if (b10 && !a10)
		{
			result = true;
			goto done;
		}
		if (a10 && !b10)
		{
			result = false;
			goto done;
		}
	}
	else if (a11 || b11)
	{
		if (a11 && !b11)
		{
			result = true;
			goto done;
		}
		if (b11 && !a11)
		{
			result = false;
			goto done;
		}
	}
	else
	{
		if (b0 && !a0)
		{
			result = true;
			goto done;
		}
		if (a0 && !b0)
		{
			result = false;
			goto done;
		}
		if (b_c0 && !a_c0)
		{
			result = true;
			goto done;
		}
		if (a_c0 && !b_c0)
		{
			result = false;
			goto done;
		}
		if (b9 && !a9)
		{
			result = true;
			goto done;
		}
		if (a9 && !b9)
		{
			result = false;
			goto done;
		}
	}
	if (strncmp(player_a->gamertag, player_b->gamertag, 16) > 0)
		result = true;
done:
	return result;
}

/* sorts the team members list, keeping each datum's salt in place */
// @retail 0x1a3a57
void clan_members_sort()
{
	word salts[121];

	if (g_global_4acf62.field_8_2 && g_global_4acf62.clan_member_reference_data &&
		g_global_4acf62.field_8_2->high_water_index > 2)
	{
		s_record_pool *references = g_global_4acf62.clan_member_reference_data;
		long count = references->high_water_index - 1;
		long i;

		for (i = 0; i < count; i++)
		{
			word *datum = (word *)datum_get_absolute(references, i);

			if (datum)
				salts[i] = *datum;
		}
		function_13da70(references->data, references->high_water_index - 1, sizeof(s_friend_player_reference), clan_member_compare, NULL);
		for (i = 0; i < g_global_4acf62.clan_member_reference_data->high_water_index - 1; i++)
		{
			word *datum = (word *)datum_get_absolute(g_global_4acf62.clan_member_reference_data, i);

			if (datum)
				*datum = salts[i];
		}
	}
}

void function_18ff47(long player, dword *out);
bool player_slot_live_tasks_pending(long index);
long function_75890(long time);
void online_team_members_enumerate_get_results(long task_index, DWORD *count, XUID *members);
void online_team_member_get_details(long task_index, XUID const *arg_9da427, XONLINE_TEAM_MEMBER *member);

/* the seconds between refreshes of the team members list (0x4cf76c, lane D's
   region; not decompiled yet) */
long g_4cf76c;

/* refills the team members list from the team members task (and the senders
   of the user's messages), or starts the task again */
// @retail 0x1a3af1
void clan_members_update()
{
	if (!g_global_4acf62.field_8_2 || !g_global_4acf62.clan_member_reference_data)
		return;

	long previous_count = g_global_4acf62.field_8_2->actual_count;

	if (g_global_4acf62.clan_members_task_index != NONE)
	{
		long status = online_task_poll(g_global_4acf62.clan_members_task_index);

		if (status == 1 || status == 2)
		{
			XONLINE_USER user;
			XUID members[100];
			DWORD member_count = NUMBEROF(members);
			long added_count;

			function_18ff47(g_global_4acf62.controller_index, (dword *)&user);
			online_team_members_enumerate_get_results(g_global_4acf62.clan_members_task_index, &member_count, members);
			record_pool_release_all(g_global_4acf62.field_8_2);
			record_pool_release_all(g_global_4acf62.clan_member_reference_data);
			g_global_4acf62.friend_request.unknown6a3 = false;
			added_count = players_list_add_message_senders(members, member_count);
			if ((long)member_count > 0)
			{
				for (long i = 0; i < (long)member_count; i++)
				{
					s_friend_request request;
					bool valid = friend_request_get(&request);
					XONLINE_TEAM_MEMBER member;

					online_team_member_get_details(g_global_4acf62.clan_members_task_index, &members[i], &member);
					if (!xuid_equal(&member.xuidTeamMember, &user.xuid, false))
					{
						long player_index = record_pool_allocate(g_global_4acf62.field_8_2);
						s_friend_player *player = (s_friend_player *)(g_global_4acf62.field_8_2->data + (player_index & 0xffff) * sizeof(s_friend_player));
						long reference_index = record_pool_allocate(g_global_4acf62.clan_member_reference_data);

						((s_friend_player_reference *)(g_global_4acf62.clan_member_reference_data->data + (reference_index & 0xffff) * sizeof(s_friend_player_reference)))->player_index = player_index;
						friends_player_set(player, (s_online_player const *)&member, i + added_count);
						if (valid && !(player->flags20 & 0xc00))
						{
							word state[256];
							s_global_string_buffer format;
							word gamertag[48];

							state[0] = 0;
							format.text[0] = 0;
							*(XUID *)&player->details = *(XUID *)&request;
							ascii_string_to_unicode(player->gamertag, gamertag, NUMBEROF(gamertag));
							function_1a4714(player->state, state);
							format.load(0x280006be);
							unicode_string_snprintf(player->name, NUMBEROF(player->name), format.text, gamertag, state);
						}
						else
						{
							ascii_string_to_unicode(player->gamertag, player->name, NUMBEROF(player->name));
							memset(&player->details, 0, sizeof(player->details));
						}
					}
				}
			}
			g_global_4acf62.friend_request.unknown6a3 = true;
			friends_player_new();
			clan_members_sort();
			if (status == 2)
			{
				function_6b640(g_global_4acf62.clan_members_task_index);
				g_global_4acf62.clan_members_task_index = NONE;
			}
		}
		else if (status != 0)
		{
			function_6b640(g_global_4acf62.clan_members_task_index);
			g_global_4acf62.clan_members_task_index = NONE;
		}
	}
	else if (!g_global_4acf62.friend_request.valid)
	{
		if (!player_slot_live_tasks_pending(g_global_4acf62.controller_index))
		{
			g_global_4acf62.friend_request.valid = player_slot_get_identity(g_global_4acf62.controller_index, (s_player_identity *)&g_global_4acf62.friend_request.request);
			if (g_global_4acf62.friend_request.valid)
			{
				g_global_4acf62.clan_members_time = function_75870();
				g_global_4acf62.clan_members_task_index = online_team_members_enumerate(g_global_4acf62.controller_index, (XUID const *)&g_global_4acf62.friend_request.request);
			}
			else
			{
				record_pool_release_all(g_global_4acf62.field_8_2);
				record_pool_release_all(g_global_4acf62.clan_member_reference_data);
				g_global_4acf62.friend_request.unknown6a3 = false;
				players_list_add_message_senders(NULL, 0);
				friends_player_new();
			}
		}
	}
	else
	{
		long timeout = g_4cf76c * 1000;

		if (!player_slot_live_tasks_pending(g_global_4acf62.controller_index) &&
			function_75890(g_global_4acf62.clan_members_time) > timeout)
		{
			g_global_4acf62.friend_request.valid = player_slot_get_identity(g_global_4acf62.controller_index, (s_player_identity *)&g_global_4acf62.friend_request.request);
			if (g_global_4acf62.friend_request.valid)
			{
				g_global_4acf62.clan_members_time = function_75870();
				g_global_4acf62.clan_members_task_index = online_team_members_enumerate(g_global_4acf62.controller_index, (XUID const *)&g_global_4acf62.friend_request.request);
			}
		}
	}
	if (g_global_4acf62.field_8_2->actual_count != previous_count)
		friends_lists_request_presence();
}

void online_presence_get_latest(long task_index, DWORD group_id, DWORD count, XONLINE_PRESENCE *presences);

/* copies the presence task's results into the friends and team members
   lists */
// score 117 (was 117): BLOCKED-BY 0x63d00. Retail keeps &player->xuid in ESI across 0x63d00 in the
// clan-list loop (it treats 0x63d00 as clobbering EDX), so j takes EDI and the presence cursor spills to
// [ebp-4], the friend count's earlier home; the early count homes follow from that.
// MiniMax R (11-01): no new form, no lower score; the member count is still read into ESI and
// pushed where retail keeps it in EAX, and the friend/presence pointers still stay in EDX across
// the 0x63d00 calls; still BLOCKED-BY 0x63d00 and 0x2b642e in other lanes' files.
// Round 19: unchanged; the count homes, the delayed count load and the post-membership flag
// reloads remain; still BLOCKED-BY 0x63d00.
// Earlier, 117 (was 117): frame agrees; count homes, early count lifetimes and flag reloads differ. Global optimization
// off 1206; bounded friend search 230; bounded member search 219; both 211; indexed merge-record pointer
// 162.
// Completed friend/member/both flag results 159/218/218; signed friend/member results
// 159/218; complete friend flag accumulation 229; both results with friend-first count 219;
// merged field result 164. Count homes, early zero and post-call flags still differ.
// Native session library copies member/friend/both 162/159/162; complete friend/member/both iterator
// initializers 231/227/211; native friend/member presence values 256/239. Count homes, early zero and post-
// call flag reloads remain.
// Friend/member/both low-bit subtraction 210/218/215; friend/member/both self-bit
// results 229/164/231; friend-field merge 164; inherited member receiver 159.
// Count homes, early zero and memory flag updates still differ.
// Empty friend/member/both row guards 159/159/159; friend/member/both identity values 265/248/287;
// friend-first/member-first merged-state accumulation 162/162. Original count homes and flag reloads remain.
// Frame and early EBX zero agree; count homes, early count lifetimes and membership flag reloads
// differ. friend self equality inputs 160; member self equality inputs 161; presence merge
// equality inputs 188; friend presence equality inputs 164; member presence equality inputs 214;
// nested merge equality inputs 161; friend membership result 159; member membership result 159;
// merge friend count first 195; merge member count home 188; merge count order and home 195; merge
// counts member row 129; merge counts friend row 190; merge counts both rows 124.
// Frame and early EBX zero agree; count homes, early count lifetimes and membership flag reloads
// differ. immutable count snapshots 124; native list record references 124; original count name
// 124; original count observation order 117; original count names and order 117; row pair without
// initial merge 219.
// count homes, early count retention and membership flag reloads remain.
// On the recovered comparison carriers: outer/inner/both merge cursors 231/117/231;
// friend/member/both search cursors 186/219/222; outer-plus-friend 236, inner-plus-member 217.
// Count homes, early saved-count lifetime and memory flag updates remain. friend field binding 120; member
// field binding 117; both field bindings 120; native count results 117; merge ordinal scope 117; friend
// count arms 117; member count arms 117; both count arms 117.
// On the recovered row/zero context: early count storage 117; matched member pointer/reference
// 151/151; existing member-bit/friend-byte/both eligibility 186/117/189; independent
// merge/friend ordinals 117; immutable count storage 117. Count homes, delayed count loads
// and memory flag updates still differ.
// clan count renamed to sort after the friend count 117; friend count read
// first 124 (its home stays ebp-8, loaded into esi at once); both 124; both counts before the
// arrays 124; both list-contains helpers declared inline 117. Ours keeps flags20 in a register
// across those helpers (they write no memory here); retail tests and updates it in memory.
// friend-side count rename 117; friend-first with that rename 124;
// retail equality call roles with the rename 159/844; nested friend/member/both membership
// guards 117/117/117; counts before the user record 117; shared user pointer 117.
// Count homes, the delayed count load and post-membership flag reloads remain.
// BLOCKED-BY 0x63d00 (round 14): our LTCG knows xuid_equal never writes EDX and keeps
// friend/presence pointers there across the calls (`mov esi, edx` after the call); retail
// recomputes them, as at all 21 retail calls of 0x63d00. The flags20 register across
// players_list_contains/friends_list_contains is the same gap (retail treats them as
// writing memory). Separate noinline inline declarations of those two change nothing.
// Round 14: retail's argument roles in the merge, friend-presence and player-presence calls
// 159; that plus the friend count first 160; merge and player roles only 164; friend count
// first 124; loop-scoped merge index 117.
// Round 418: member count after user copy 124; friend count after user copy 118; both counts after user
// copy 126; friend-first after user copy 126; named member after user 124; named friend after user 118;
// named both after user 126; named friend-first after user 126. count homes, early count retention and
// membership flag reloads remain.
// Earlier, 117 (was 117): this round: the two member counts in a local two-member struct (both kept
// in memory) give 124, the same as exchanging the two declarations; the score stays 117. The
// member count is still read into ESI and pushed where retail keeps it in EAX, and the count
// homes, the delayed count loads and the membership flag reloads remain. Still BLOCKED-BY
// 0x63d00 and 0x2b642e, whose files belong to other lanes.
// Earlier, 117 (was 117): this round: the listing was re-read to place the first difference; the
// member count is still read into ESI and pushed where retail keeps it in EAX, and the count
// homes, the delayed count loads and the membership flag reloads remain. Still BLOCKED-BY
// 0x63d00 and 0x2b642e, whose files belong to other lanes.
// Round 25 (lane 17, fast): friend count first with the clan count named member_count 124 (friend count
// still [ebp-8] and in ESI at once); counts zeroed then set by two ifs 242 (839 bytes); counts as if/else
// arms 117 (same code); inverted ternaries `!pool ? 0 : count` 121; friend-first if/else arms 124.
// @retail 0x1a3dfe
void friends_lists_update_presence()
{
	if (g_global_4acf62.presence_task_index == NONE)
		return;

	long status = online_task_poll(g_global_4acf62.presence_task_index);

	if (status == 1 || status == 2)
	{
		XONLINE_PRESENCE friend_presences[101];
		XONLINE_PRESENCE clan_presences[121];
		XONLINE_USER user;
		long clan_count = g_global_4acf62.field_8_2 ? g_global_4acf62.field_8_2->actual_count : 0;
		long friend_count = g_global_4acf62.field_4_4 ? g_global_4acf62.field_4_4->actual_count : 0;
		long i;

		function_18ff47(g_global_4acf62.controller_index, (dword *)&user);
		if (friend_count > 0)
			online_presence_get_latest(g_global_4acf62.presence_task_index, 'frnd', friend_count, friend_presences);
		if (clan_count > 0)
			online_presence_get_latest(g_global_4acf62.presence_task_index, 'clan', clan_count, clan_presences);
		for (i = 0; i < friend_count; i++)
		{
			for (long j = 0; j < clan_count; j++)
			{
				if (xuid_equal(&friend_presences[i].xuid, &clan_presences[j].xuid, false))
				{
					DWORD state = friend_presences[i].dwUserState | clan_presences[j].dwUserState;

					clan_presences[j].dwUserState = state;
					friend_presences[i].dwUserState = state;
				}
			}
		}

		if (g_global_4acf62.field_4_4)
		{
			s_list_item_iterator iterator;

			iterator.iterator.index = NONE;
			iterator.iterator.datum_index = NONE;
			iterator.iterator.data = g_global_4acf62.field_4_4;
			while (function_2b2327(&iterator))
			{
				s_friend *friend_ = (s_friend *)iterator.item;

				if (friend_->xuid.qwUserID)
				{
					if (xuid_equal(&user.xuid, &friend_->xuid, false))
						friend_->flags20 |= 1;
					for (long j = 0; j < friend_count; j++)
					{
						if (xuid_equal(&friend_presences[j].xuid, &friend_->xuid, false))
						{
							friend_->flags20 |= friend_presences[j].dwUserState & 0xffffff20;
							if ((friend_->flags20 & 0x30) && !players_list_contains(&friend_->xuid))
								friend_->flags20 &= ~0xf;
							break;
						}
					}
				}
			}
		}

		if (g_global_4acf62.field_8_2)
		{
			s_list_item_iterator local_51cbf5;

			local_51cbf5.iterator.index = NONE;
			local_51cbf5.iterator.datum_index = NONE;
			local_51cbf5.iterator.data = g_global_4acf62.field_8_2;
			while (function_2b2327(&local_51cbf5))
			{
				s_friend_player *player = (s_friend_player *)local_51cbf5.item;

				if (player->xuid.qwUserID)
				{
					if (xuid_equal(&user.xuid, &player->xuid, false))
						player->flags20 |= 1;
					for (long j = 0; j < clan_count; j++)
					{

						if (xuid_equal(&clan_presences[j].xuid, &player->xuid, false))
						{
							player->flags20 |= clan_presences[j].dwUserState;
							player->session_id = clan_presences[j].SessionID;
							player->title_id = clan_presences[j].dwTitleID;
							player->state_data_size = clan_presences[j].StateDataSize > sizeof(player->state_data) ? sizeof(player->state_data) : clan_presences[j].StateDataSize;
							memcpy(player->state_data, clan_presences[j].StateData, player->state_data_size);
							if ((player->flags20 & 0xc00) && !friends_list_contains(&player->xuid))
								player->flags20 &= ~0xf;
							break;
						}
					}
					if (g_global_4acf62.field_4_4)
					{
						s_list_item_iterator iterator;

						iterator.iterator.index = NONE;
						iterator.iterator.datum_index = NONE;
						iterator.iterator.data = g_global_4acf62.field_4_4;
						while (function_2b2327(&iterator))
						{
							s_friend *friend_ = (s_friend *)iterator.item;

							if (xuid_equal(&player->xuid, &friend_->xuid, false))
							{
								player->flags20 = (player->flags20 & 0xffffff20) | friend_->flags20;
								friend_->flags20 = player->flags20;
								player->session_id = friend_->session_id;
								player->title_id = friend_->title_id;
								player->state_data_size = friend_->state_data_size > sizeof(player->state_data) ? sizeof(player->state_data) : friend_->state_data_size;
								memcpy(player->state_data, friend_->state_data, player->state_data_size);
							}
						}
					}
				}
			}
			clan_members_sort();
		}
	}
	else if (status != 0)
	{
		function_6b640(g_global_4acf62.presence_task_index);
		g_global_4acf62.presence_task_index = NONE;
	}
}

/* updates the friends and team members lists while the controller is signed
   in to Xbox Live */
// @retail 0x1a31ff
void function_1a31ff()
{
	long controller_index = g_global_4acf62.controller_index;

	if (controller_index >= 0 && controller_index < 4 && TEST_FIELD_BIT(g_54e8e0[controller_index].flag5))
	{
		friends_list_update();
		clan_members_update();
		friends_lists_update_presence();
	}
}
