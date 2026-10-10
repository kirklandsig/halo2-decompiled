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

class c_online_task_screen;
typedef void (__stdcall *online_task_screen_callback)(c_online_task_screen *screen);

/* the screen (0x624 bytes, screen id 0xb6) */
class c_online_task_screen : public c_class_1473c9
{
public:
	c_online_task_screen(long a, long b, word user_flags);
	virtual ~c_online_task_screen();
	virtual void v3();
	virtual bool v10(s_widget_event *event);
	virtual screen_load_proc get_load_proc();

	void set_title(long string_handle);
	void set_description(long string_handle);
	void set_title_from_block(long block_index, long index);
	void set_description_from_block(long block_index, long index);

	online_task_screen_callback finished;
	online_task_screen_callback cancelled;
	byte unknown618[4];
	long task_index;
	HRESULT result;
};

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
			byte flags_0 : 1;
			byte : 7;
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
dword function_0b4a20(dword key);
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
		dialog_ok_show(1, function_0b4a20(screen->result), 4, screen->user_flags, 0, 0);
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
		dialog_ok_show(1, function_0b4a20(screen->result), 4, screen->user_flags, 0, 0);
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
		dialog_ok_show(1, function_0b4a20(screen->result), 4, screen->user_flags, 0, 0);
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
	player->flags20 = source->flags_0 << 10;
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

void function_23620d(long string_handle, word *buffer);

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
// @retail 0x1a4441
long players_list_add_message_senders(XUID const *excluded, long excluded_count)
{
	s_entry messages[125];
	long message_count = NUMBEROF(messages);
	long added_count = 0;
	long i;

	online_messages_enumerate(g_global_4acf62.controller_index, messages, &message_count);
	for (i = 0; i < message_count; i++)
	{
		s_entry *message = &messages[i];

		if (added_count >= 20)
			break;
		if (TEST_FIELD_BIT(message->flag_bits.flag12))
		{
			XUID *xuid = message_entry_get_xuid(*(s_entry *volatile *)&message);
			bool is_excluded = false;
			long j;

			for (j = 0; j < excluded_count; j++)
			{
				if (xuid_equal(xuid, &excluded[j], false))
					is_excluded = true;
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
					word format[256];
					word gamertag[48];
	
					format[0] = 0;
					ascii_string_to_unicode(friend_->gamertag, gamertag, NUMBEROF(gamertag));
					function_23620d(0x220006bd, format);
					unicode_string_snprintf(friend_->name, NUMBEROF(friend_->name), format, gamertag, name);
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
extern long g_4cf984;

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
// @retail 0x1a391f
bool __stdcall clan_member_compare(void const *a, void const *b, void const *context)
{
	s_friend_player *player_a = clan_member_reference_get_player(a);
	s_friend_player *player_b = clan_member_reference_get_player(b);
	dword flags_a = player_a->flags20;
	dword flags_b = player_b->flags20;
	bool a9 = (bool)((flags_a >> 9) & 1);
	bool b9 = (bool)((flags_b >> 9) & 1);
	volatile bool result = false;
	bool a_c0 = false;
	bool b_c0 = false;

	if (flags_a & 0xc0)
		a_c0 = true;
	if (flags_b & 0xc0)
		b_c0 = true;

	bool a11 = (bool)((flags_a >> 11) & 1);
	bool a10 = (bool)((flags_a >> 10) & 1);
	bool b11 = (bool)((flags_b >> 11) & 1);
	bool b10 = (bool)((flags_b >> 10) & 1);
	bool a0 = (bool)(player_a->unknown20 & 1);
	bool b0 = (bool)(player_b->unknown20 & 1);

	if (a10)
	{
		if (!b10)
			return false;
	}
	else if (b10)
	{
		return true;
	}
	else if (a11)
	{
		if (!b11)
			return true;
	}
	else if (b11)
	{
		return false;
	}
	else
	{
		if (b0 && !a0)
			return true;
		if (a0 && !b0)
			return false;
		if (b_c0 && !a_c0)
			return true;
		if (a_c0 && !b_c0)
			return false;
		if (b9 && !a9)
			return true;
		if (a9 && !b9)
			return false;
	}
	if (strncmp(player_a->gamertag, player_b->gamertag, 16) > 0)
		return true;
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
bool function_18ff64(long index);
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
			long i;
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
			i = 0;
			do
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
						word format[256];
						word gamertag[48];

						state[0] = 0;
						format[0] = 0;
						*(XUID *)&player->details = *(XUID *)&request;
						ascii_string_to_unicode(player->gamertag, gamertag, NUMBEROF(gamertag));
						function_1a4714(player->state, state);
						function_23620d(0x280006be, format);
						unicode_string_snprintf(player->name, NUMBEROF(player->name), format, gamertag, state);
					}
					else
					{
						ascii_string_to_unicode(player->gamertag, player->name, NUMBEROF(player->name));
						memset(&player->details, 0, sizeof(player->details));
					}
				}
				i++;
			}
			while (i < (long)member_count);
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
		if (!function_18ff64(g_global_4acf62.controller_index))
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

		if (!function_18ff64(g_global_4acf62.controller_index) &&
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
		long friend_count = g_global_4acf62.field_4_4 ? g_global_4acf62.field_4_4->actual_count : 0;
		long clan_count = g_global_4acf62.field_8_2 ? g_global_4acf62.field_8_2->actual_count : 0;
		long i;
		long j;

		function_18ff47(g_global_4acf62.controller_index, (dword *)&user);
		if (friend_count > 0)
			online_presence_get_latest(g_global_4acf62.presence_task_index, 'frnd', friend_count, friend_presences);
		if (clan_count > 0)
			online_presence_get_latest(g_global_4acf62.presence_task_index, 'clan', clan_count, clan_presences);
		for (i = 0; i < friend_count; i++)
		{
			for (j = 0; j < clan_count; j++)
			{
				if (xuid_equal(&clan_presences[j].xuid, &friend_presences[i].xuid, false))
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
					for (j = 0; j < friend_count; j++)
					{
						if (xuid_equal(&friend_->xuid, &friend_presences[j].xuid, false))
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
					for (j = 0; j < clan_count; j++)
					{
						XONLINE_PRESENCE *presence = &clan_presences[j];

						if (xuid_equal(&player->xuid, &presence->xuid, false))
						{
							player->flags20 |= presence->dwUserState;
							player->session_id = presence->SessionID;
							player->title_id = presence->dwTitleID;
							player->state_data_size = presence->StateDataSize > sizeof(player->state_data) ? sizeof(player->state_data) : presence->StateDataSize;
							memcpy(player->state_data, presence->StateData, player->state_data_size);
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
