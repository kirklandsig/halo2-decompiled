// @flags /O2 /Ob1 /Gr
/* ONLINE_MESSAGES.CPP: the Live messages: enumerating a user's messages,
   their details (task type 38), attachments (type 39) and deleting them
   (lane D) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include <wchar.h>
#include "online_tasks.h"
#include "online_message_entries.h"
#include "loop_allocator.h"
#include "globals.h"
#include "unknown_059ad0.h"
#include "unknown_058ee0.h"
#include "unknown_19b510.h"

void function_08ebd0(s_entry_source *source, s_entry *entry);
long function_8eda0(long state);

#define k_maximum_messages 125

// @retail 0x8e750 standard
void __stdcall online_messages_enumerate(DWORD controller_index, s_entry *entries, long *count)
{
	XONLINE_MSG_SUMMARY summaries[k_maximum_messages];
	DWORD summary_count;

	*count = 0;
	if (online_logon_connected() && SUCCEEDED(XOnlineMessageEnumerate(controller_index, summaries, &summary_count)))
	{
		for (DWORD i = 0; i < summary_count; i++)
			function_08ebd0((s_entry_source *)&summaries[i], &entries[i]);
		*count = summary_count;
	}
}

// @retail 0x8e7e0
bool online_messages_find_from(const XUID *sender, DWORD controller_index, long kind)
{
	bool result = false;
	s_entry entries[k_maximum_messages];
	long count = k_maximum_messages;

	online_messages_enumerate(controller_index, entries, &count);
	for (long i = 0; i < count; i++)
	{
		s_entry *entry = &entries[i];
		if (*(ULONGLONG *)entry == sender->qwUserID &&
			(kind == 1 || !TEST_FIELD_BIT(entry->flag_bits.flag10)) &&
			(kind == 2 || !TEST_FIELD_BIT(entry->flag_bits.flag12)) &&
			(kind != 3 && kind != 4 || TEST_FIELD_BIT(entry->flag_bits.flag11)))
		{
			result = true;
			break;
		}
	}
	return result;
}

// @retail 0x8e890
bool online_messages_find_flagged_from(const XUID *sender, DWORD controller_index)
{
	bool result = false;
	s_entry entries[k_maximum_messages];
	long count = k_maximum_messages;

	online_messages_enumerate(controller_index, entries, &count);
	for (long i = 0; i < count; i++)
	{
		if (*(ULONGLONG *)&entries[i] == sender->qwUserID && TEST_FIELD_BIT(entries[i].flag_bits.flag10))
		{
			result = true;
			break;
		}
	}
	return result;
}

// @retail 0x8e920
void online_message_delete(DWORD controller_index, DWORD message_id, bool block_sender)
{
	if (online_logon_connected())
		XOnlineMessageDelete(controller_index, message_id, block_sender);
}

// @retail 0x8e950
long online_message_details(DWORD controller_index, const s_entry *entry)
{
	long task_index = online_task_new_if_logged_on();

	if (task_index != NONE)
	{
		s_type_9df9da *task = online_task_try_get(task_index);
		if (task)
		{
			if (SUCCEEDED(XOnlineMessageDetails(controller_index, entry->unknown20, XONLINE_MSG_FLAG_READ, 0, NULL, (PXONLINETASK_HANDLE)&task->handle)))
			{
				task->flags = 1;
				task->type = 38;
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

// @retail 0x8e9e0
bool online_message_details_get_property(long task_index, long property, void *buffer, DWORD size, bool *too_small, DWORD *required_size)
{
	bool result = false;

	if (size > 0)
		memset(buffer, 0, size);
	*too_small = false;
	*required_size = 0;

	s_type_9df9da *task = online_task_try_get(task_index);
	if (task && online_logon_connected())
	{
		HRESULT hr = XOnlineMessageDetailsGetResultsProperty((XONLINETASK_HANDLE)task->handle, (WORD)function_8eda0(property), size, buffer, &size, NULL);
		result = SUCCEEDED(hr);
		if (!result && hr == 0x80155a02)
		{
			*too_small = true;
			*required_size = size;
		}
	}
	return result;
}

// @retail 0x8eab0
long online_message_download_attachment(long details_task_index, long property, void *buffer, DWORD size)
{
	long task_index = NONE;

	if (online_task_poll(details_task_index) == 2)
	{
		s_type_9df9da *local_c5a52a = function_6b910(details_task_index);
		if (local_c5a52a && online_logon_connected())
		{
			task_index = online_task_new_if_logged_on();
			s_type_9df9da *task = function_6b910(task_index);
			if (task)
			{
				if (SUCCEEDED(XOnlineMessageDownloadAttachmentToMemory((XONLINETASK_HANDLE)local_c5a52a->handle, (WORD)function_8eda0(property),
					(PBYTE)buffer, size, NULL, (PXONLINETASK_HANDLE)&task->handle)))
				{
					task->flags = 1;
					task->type = 39;
					task->controller_index = local_c5a52a->controller_index;
				}
				else
				{
					function_6b640(task_index);
					return NONE;
				}
			}
		}
	}
	return task_index;
}

// @retail 0x8eb50
bool online_message_download_get_results(long task_index, DWORD *received_size, BYTE **data, DWORD *total_size)
{
	*data = 0;
	*received_size = 0;
	*total_size = 0;
	if (online_task_poll(task_index) == 2)
	{
		s_type_9df9da *task = function_6b910(task_index);
		if (task && online_logon_connected() &&
			SUCCEEDED(XOnlineMessageDownloadAttachmentToMemoryGetResults((XONLINETASK_HANDLE)task->handle, data, received_size, total_size)))
		{
			return true;
		}
	}
	return false;
}

// @retail 0x8eee0
void online_message_block_reset(s_state_block *block)
{
	if (block->unknown218 != NONE)
	{
		function_6b640(block->unknown218);
		block->unknown218 = NONE;
	}
	XONLINE_MSG_HANDLE message = (XONLINE_MSG_HANDLE)block->unknown21c;
	if (message && message != (XONLINE_MSG_HANDLE)NONE && SUCCEEDED(XOnlineMessageDestroy(message)))
		block->unknown21c = 0;
	block->unknownc = 0;
	block->unknown20c = 0;
	block->active = false;
}

/* the text of a message being written */
// @retail 0x8ef40
void online_message_block_set_text(s_state_block *block, const wchar_t *text)
{
	wchar_t *block_text = (wchar_t *)&block->unknownc;
	wcsncpy(block_text, text, 255);
	block_text[255] = 0;
}

// @retail 0x8ef60
void online_message_block_set_values(s_state_block *block, long value210, long value214, long value20c)
{
	if (value210 < 0x499a)
	{
		block->unknown210 = value210;
		block->unknown20c = value20c;
		*(long *)block->unknown214 = value214;
	}
}

/* the message property tag of each of the game's message properties */
// @retail 0x8f3f0
long online_message_property_get_tag(long property)
{
	switch (property)
	{
	case 0:
		return 0x9c1;
	case 1:
		return 0x3c2;
	case 2:
		return 0x4c3;
	case 3:
		return 0x6c4;
	case 4:
		return 0x4c5;
	case 5:
		return 0x581;
	case 6:
		return 0x681;
	default:
		return NONE;
	}
}

// @retail 0x8f450
bool online_message_property_size_valid(long property, DWORD size)
{
	bool result;

	switch (property)
	{
	case 0:
		result = size > 0;
		break;
	case 1:
		result = size == 2;
		break;
	case 2:
		result = size == 4;
		break;
	case 3:
		result = size > 0 && !(size & 1);
		break;
	case 4:
		result = size == 4;
		break;
	case 5:
		result = size == 8;
		break;
	case 6:
		result = size > 0 && !(size & 1);
		break;
	default:
		result = false;
		break;
	}
	return result;
}

// @retail 0x8f4b0
void online_message_block_set_property(s_state_block *block, long property, DWORD size, const void *value)
{
	long tag = online_message_property_get_tag(property);

	if (tag == NONE)
		block->unknown8 = 4;
	else if (!online_message_property_size_valid(property, size))
		block->unknown8 = 4;
	else if (online_logon_connected())
		XOnlineMessageSetProperty((XONLINE_MSG_HANDLE)block->unknown21c, (WORD)tag, size, value, 0);
}

extern s_loop_allocator *g_51e998;

/* loop_free (0x18e430) and function_1a4826 (0x1a4826), inlined here */
static inline void loop_free_inline(s_loop_allocator *loop, void **pointer)
{
	s_loop_block *block = (s_loop_block *)*pointer - 1;

	loop->free += block->size;
	if (block->previous)
	{
		block->previous->next = block->next;
	}
	else
	{
		loop->first = block->next;
	}
	if (block->next)
	{
		block->next->previous = block->previous;
	}
	else
	{
		loop->last = block->previous;
	}
}

static inline void user_interface_free_inline(void *pointer)
{
	loop_free_inline(g_51e998, &pointer);
}

/* frees a message block (allocated from the user interface's pool) */
// @retail 0x8fa30
void function_08fa30(s_state_block *block)
{
	if (block->active)
		online_message_block_reset(block);
	user_interface_free_inline(block);
}

/* ---- sending a message block: create the Live message, set its
   properties, then send it as a friend request (kind 1), a team recruit (2),
   a game invite (3) or a message (4) ---- */

/* the identity a player slot holds at +0x4e0 (0x6a2 bytes; it starts with the
   team's id, then the team's name) */
#pragma pack(push, 2)
struct s_message_team_identity
{
	XUID xuid;
	wchar_t name[(0x6a2 - sizeof(XUID)) / sizeof(wchar_t)];
};
#pragma pack(pop)

/* a view of the player slots (g_54e8e0, 0xc70 bytes each) */
struct s_message_player_slot
{
	byte unknown000[0x4e0];
	s_message_team_identity identity;
	byte unknownb82[0xc70 - 0xb82];
};

/* a view of the online task screen (unknown_1a2ca7.cpp) */
class c_online_task_screen;
struct s_message_task_screen
{
	byte unknown000[8];
	word user_flags;
	byte unknown00a[0x610 - 0xa];
	long callback;
	long value;
	s_state_block *block;
	long task_index;
};

struct s_player_identity;

/* unknown_054fe0.cpp */
s_long_pair *network_session_interface_get_data_4999(void);

/* online_tasks.cpp */
HRESULT online_task_continue(s_type_9df9da *task);

/* unknown_0b49a0.cpp: the error string of a Live result */
dword function_0b4a20(dword key);

/* unknown_18f576.cpp */
bool player_slot_get_identity(long index, s_player_identity *identity);

/* unknown_147f6d.cpp */
void function_1487c3(long controller_index, long task_index, long callback, long value, long context);

/* unknown_19b510.cpp */
void dialog_ok_show(long a, long dialog_id, long b, word user_flags, dialog_choice_callback chosen, dialog_closed_callback closed);

/* unknown_1a2ca7.cpp: the pending friend request */
#include "unknown_x8d43e5.h"

void online_task_screen_dispose_task(c_online_task_screen *screen);

/* function_1a3294 and online_task_screen_finish (unknown_1a2ca7.cpp),
   inlined here */
static inline void friend_request_clear(void)
{
	g_global_4acf62.friend_request.valid = false;
	memset(&g_global_4acf62.friend_request.request, 0, sizeof(g_global_4acf62.friend_request.request));
}

static inline void online_task_screen_finish_inline(c_online_task_screen *screen)
{
	((s_message_task_screen *)screen)->callback = 0;
	online_task_screen_dispose_task(screen);
}

static inline s_message_player_slot *message_player_slots(void)
{
	return (s_message_player_slot *)g_54e8e0;
}

/* the message's context: the sender's team for a team recruit */
// @retail 0x8f390
ULONGLONG online_message_block_get_context(long controller_index, s_state_block *block)
{
	ULONGLONG context = 0;

	if (block->unknown4 == 2)
	{
		s_message_player_slot *slot = &message_player_slots()[controller_index];
		if (slot->identity.xuid.qwUserID != 0)
		{
			s_message_team_identity identity = slot->identity;
			context = identity.xuid.qwUserID;
		}
	}
	return context;
}

// @retail 0x8f070
void online_message_block_create(s_state_block *block, long controller_index)
{
	if (online_logon_connected())
	{
		byte message_type;
		DWORD flags;
		WORD property_count;

		switch (block->unknown4)
		{
		case 1:
			message_type = 2;
			break;
		case 3:
			message_type = 3;
			break;
		case 2:
			message_type = 4;
			break;
		case 4:
			message_type = 1;
			break;
		default:
			message_type = 1;
			break;
		}
		flags = 0;
		if (block->unknown210 > 0)
			flags = 4;
		if (wcslen((wchar_t *)&block->unknownc) > 0)
			flags |= 8;
		if (block->unknown4 == 2)
			flags |= 0x40;
		property_count = 0;
		if (block->unknown210 > 0)
			property_count = 3;
		if (wcslen((wchar_t *)&block->unknownc) > 0)
			property_count += 2;
		if (block->unknown4 == 3 || block->unknown4 == 2)
			property_count++;
		ULONGLONG context = online_message_block_get_context(controller_index, block);
		WORD expire = 0;
		if (block->unknown4 == 4)
			expire = 0x4ec0;
		if (SUCCEEDED(XOnlineMessageCreate(message_type, property_count, 0, context,
			flags, expire, (XONLINE_MSG_HANDLE *)&block->unknown21c)))
		{
			block->unknown8 = 1;
		}
		else
		{
			block->unknown21c = 0;
		}
	}
}

/* network_session_interface_get_data_4999 (0x64cf0), inlined here */
static inline s_long_pair *network_session_interface_get_data_4999_inline(void)
{
	s_long_pair *result = 0;
	c_class_58d20 *session = 0;

	if (g_527330.initialized)
	{
		c_class_58d20 *session_a = (c_class_58d20 *)g_527330.session_a;
		long state = session_a->state;
		if (state && state > 2 && state <= 8)
			session = session_a;
	}
	if (session && session->flag4998)
		result = &session->data4999;
	return result;
}

static inline c_class_58d20 *message_network_session_get_live(void)
{
	c_class_58d20 *result = 0;
	if (g_527330.initialized)
	{
		c_class_58d20 *session = (c_class_58d20 *)g_527330.session_a;
		long state = session->state;
		if (state && state > 2 && state <= 8)
			result = session;
	}
	return result;
}

static inline s_long_pair *message_session_get_data_4999(c_class_58d20 *session)
{
	s_long_pair *result = 0;
	if (session->flag4998)
		result = &session->data4999;
	return result;
}

static inline s_long_pair *network_session_interface_get_data_4999_inline2(void)
{
	s_long_pair *result = 0;
	c_class_58d20 *session = message_network_session_get_live();
	if (session)
		result = message_session_get_data_4999(session);
	return result;
}

// @retail 0x8f170
void online_message_block_set_properties(s_state_block *block, long controller_index)
{
	wchar_t *text = (wchar_t *)&block->unknownc;
	DWORD value;
	s_message_team_identity identity;

	if (wcslen(text) > 0)
	{
		long length = wcslen(text);
		value = XGetLanguage();
		online_message_block_set_property(block, 3, (length + 1) * sizeof(wchar_t), text);
		online_message_block_set_property(block, 4, sizeof(value), &value);
	}
	if (block->unknown210 > 0)
	{
		value = 1;
		online_message_block_set_property(block, 0, block->unknown210, (const void *)block->unknown20c);
		online_message_block_set_property(block, 1, sizeof(word), &value);
		online_message_block_set_property(block, 2, sizeof(long), block->unknown214);
	}
	if (block->unknown4 == 3)
	{
		s_long_pair *session_id;
		session_id = network_session_interface_get_data_4999_inline2();
		if (session_id)
			online_message_block_set_property(block, 5, 8, session_id);
		else
			block->unknown8 = 4;
	}
	else if (block->unknown4 == 2)
	{
		if (player_slot_get_identity(controller_index, (s_player_identity *)&identity))
			online_message_block_set_property(block, 6, (wcslen(identity.name) + 1) * sizeof(wchar_t), identity.name);
		else
			block->unknown8 = 4;
	}
	if (block->unknown8 != 4)
		block->unknown8 = 2;
}

// @retail 0x8f6e0
HRESULT online_message_block_send_friend_request(const XUID *xuid, s_state_block *block, long controller_index, const char *gamertag)
{
	HRESULT result = E_FAIL;
	long task_index = online_task_new_if_logged_on();

	block->unknown218 = task_index;
	s_type_9df9da *task = online_task_try_get(task_index);
	if (task)
	{
		bool by_xuid = true;

		if (xuid)
		{
			result = XOnlineFriendsRequestEx(controller_index, *xuid, (XONLINE_MSG_HANDLE)block->unknown21c, NULL, (PXONLINETASK_HANDLE)&task->handle);
		}
		else
		{
			if (!gamertag)
				goto failed;
			by_xuid = false;
			result = XOnlineFriendsRequestByNameEx(controller_index, gamertag, (XONLINE_MSG_HANDLE)block->unknown21c, NULL, (PXONLINETASK_HANDLE)&task->handle);
		}
		if (SUCCEEDED(result))
		{
			task->flags = 1;
			task->controller_index = controller_index;
			task->type = by_xuid ? 4 : 5;
			return result;
		}
	failed:
		function_6b640(block->unknown218);
	}
	block->unknown218 = NONE;
	return result;
}

// @retail 0x8f500
HRESULT online_message_block_send_team_recruit(s_state_block *block, long controller_index, const XUID *xuid, const char *gamertag)
{
	HRESULT result = E_FAIL;
	long task_index = online_task_new_if_logged_on();

	block->unknown218 = task_index;
	s_type_9df9da *task = online_task_try_get(task_index);
	if (task)
	{
		s_message_player_slot *slot = &message_player_slots()[controller_index];
		if (slot->identity.xuid.qwUserID != 0)
		{
			s_message_team_identity identity = slot->identity;
			bool by_xuid = true;
			XONLINE_TEAM_MEMBER_PROPERTIES properties;

			properties.dwPrivileges = 0x10;
			properties.TeamMemberDataSize = 0;
			if (xuid)
			{
				result = XOnlineTeamMemberRecruit(controller_index, identity.xuid, *xuid, &properties, (XONLINE_MSG_HANDLE)block->unknown21c, NULL, (PXONLINETASK_HANDLE)&task->handle);
			}
			else
			{
				if (!gamertag)
					goto failed;
				by_xuid = false;
				result = XOnlineTeamMemberRecruitByName(controller_index, identity.xuid, gamertag, &properties, (XONLINE_MSG_HANDLE)block->unknown21c, NULL, (PXONLINETASK_HANDLE)&task->handle);
			}
			if (SUCCEEDED(result))
			{
				task->flags = 1;
				task->controller_index = controller_index;
				task->type = by_xuid ? 0x1b : 0x1c;
				return result;
			}
		failed:
			function_6b640(block->unknown218);
			block->unknown218 = NONE;
			return result;
		}
		function_6b640(task_index);
	}
	block->unknown218 = NONE;
	return result;
}

// @retail 0x8f7e0
HRESULT online_message_block_send_game_invite(long controller_index, s_long_pair *session_id, s_state_block *block, const XUID *recipients, long recipient_count)
{
	long task_index = online_task_new_if_logged_on();

	block->unknown218 = task_index;
	s_type_9df9da *task = online_task_try_get(task_index);
	if (task)
	{
		HRESULT result = XOnlineGameInviteSend(controller_index, (word)recipient_count, recipients, *(XNKID *)session_id, 0,
			(XONLINE_MSG_HANDLE)block->unknown21c, NULL, (PXONLINETASK_HANDLE)&task->handle);
		if (SUCCEEDED(result))
		{
			task->controller_index = controller_index;
			task->flags = 1;
			task->type = 0x24;
			return result;
		}
		function_6b640(block->unknown218);
		block->unknown218 = NONE;
		return result;
	}
	block->unknown218 = NONE;
	return E_FAIL;
}

// @retail 0x8f8b0
HRESULT online_message_block_send_message(long controller_index, s_state_block *block, const XUID *recipients, long recipient_count)
{
	HRESULT result = E_FAIL;
	long task_index = online_task_new_if_logged_on();

	block->unknown218 = task_index;
	s_type_9df9da *task = online_task_try_get(task_index);
	if (task)
	{
		result = XOnlineMessageSend(controller_index, (XONLINE_MSG_HANDLE)block->unknown21c, recipient_count, recipients, NULL, (PXONLINETASK_HANDLE)&task->handle);
		if (SUCCEEDED(result))
		{
			task->flags = 1;
			task->type = 0x23;
			task->controller_index = controller_index;
			online_task_continue(task);
			return result;
		}
		function_6b640(block->unknown218);
	}
	block->unknown218 = NONE;
	return result;
}

/* the online task screen's callback once the send finishes: frees the block
   and shows the error if the task failed */
// @retail 0x8f960
void __stdcall online_message_block_send_finished(c_online_task_screen *screen)
{
	s_message_task_screen *view = (s_message_task_screen *)screen;
	bool failed = false;
	long error = 0x39;
	long task_index = view->task_index;

	if (task_index != NONE)
	{
		long status = online_task_poll(task_index);
		if (status != 2)
		{
			if (status <= 2 || status > 5)
				return;
			s_type_9df9da *task = function_6b910(task_index);
			if (task)
				error = function_0b4a20(online_task_continue(task));
			failed = true;
		}
	}
	s_state_block *block = view->block;
	if (block && block->unknown4 == 2)
		friend_request_clear();
	function_08fa30(block);
	online_task_screen_finish_inline(screen);
	if (failed && error != 0x90)
		dialog_ok_show(1, error, 4, view->user_flags, 0, 0);
}

// @retail 0x8f2c0
void online_message_block_send(s_state_block *block, long controller_index, const XUID *recipients, const char *gamertag, long recipient_count)
{
	HRESULT result = E_FAIL;

	switch (block->unknown4)
	{
	case 1:
		result = online_message_block_send_friend_request(recipients, block, controller_index, gamertag);
		break;
	case 3:
		{
			s_long_pair *session_id = network_session_interface_get_data_4999();
			result = online_message_block_send_game_invite(controller_index, session_id, block, recipients, recipient_count);
		}
		break;
	case 4:
		result = online_message_block_send_message(controller_index, block, recipients, recipient_count);
		break;
	case 2:
		result = online_message_block_send_team_recruit(block, controller_index, recipients, gamertag);
		break;
	}
	if (block->unknown218 == NONE)
	{
		dialog_ok_show(1, function_0b4a20(result), 4, 1 << controller_index, 0, 0);
	}
	else
	{
		block->unknown8 = 3;
		function_1487c3(controller_index, block->unknown218, (long)online_message_block_send_finished, 0, (long)block);
	}
}

/* sends a friend request or a team recruit to a gamertag; the task, or NONE
   (the block is freed then) */
// @retail 0x8ef90
long function_08ef90(s_state_block *block, long controller_index, const char *gamertag)
{
	if (block->unknown4 > 0 && block->unknown4 <= 2)
		online_message_block_create(block, controller_index);
	if (block->unknown8 == 1)
	{
		online_message_block_set_properties(block, controller_index);
		if (block->unknown8 == 2)
			online_message_block_send(block, controller_index, NULL, gamertag, 1);
	}
	long task_index = block->unknown218;
	if (task_index == NONE)
		function_08fa30(block);
	return task_index;
}

/* sends the block to users (a game invite or a message needs a text or an
   attachment); the task, or NONE (the block is freed then) */
// @retail 0x8eff0
long function_08eff0(s_state_block *block, long controller_index, const XUID *recipients, long recipient_count)
{
	long kind = block->unknown4;

	if (kind > 0 && (kind <= 3 || kind == 4 && (block->unknown210 > 0 || wcslen((wchar_t *)&block->unknownc) > 0)))
		online_message_block_create(block, controller_index);
	if (block->unknown8 == 1)
	{
		online_message_block_set_properties(block, controller_index);
		if (block->unknown8 == 2)
			online_message_block_send(block, controller_index, recipients, NULL, recipient_count);
	}
	long task_index = block->unknown218;
	if (task_index == NONE)
		function_08fa30(block);
	return task_index;
}
