// @flags /O2 /Ob1 /Gr
/* ONLINE_TASKS.CPP: the online tasks (the data array g_4cf78c, 24 tasks of
   0x14 bytes) (lane D) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include "data_array.h"
#include "globals.h"
#include "online_tasks.h"
#include "online_message_entries.h"
#include "unknown_234c64.h"

byte g_4771c8[0x2580];
long g_479748 = NONE;

/* the two development addresses the local machine's address (g_4cf7cc,
   globals.h) is compared with */
const s_online_address g_43ff84[2] =
{
	{ 0x00, 0x0d, 0x3a, 0x5d, 0xd1, 0xf1 },
	{ 0x00, 0x50, 0xf2, 0x10, 0x52, 0x20 },
};
bool g_50944e;

// @retail 0x6b3e0
void online_tasks_initialize(void)
{
	g_4cf78c = data_new_inlined("online tasks", 24, sizeof(s_type_9df9da), 0, g_468758);
	g_4cf78c->valid = true;
	record_pool_release_all(g_4cf78c);
	online_check_development_address();
	memset(g_4771c8, 0, sizeof(g_4771c8));
	g_479748 = NONE;
}

// @retail 0x6b5d0
long online_task_poll(long task_index)
{
	s_type_9df9da *task = online_task_try_get(task_index);

	long status;

	if (!task)
		status = 5;
	else if (task->flag_bits.finished)
		status = task->flag_bits.failed ? 3 : 2;
	else if (task->flag_bits.running)
		status = 1;
	else
		status = task->flag_bits.started ? 0 : 4;
	return status;
}

// @retail 0x6b6f0
inline long online_task_new(void)
{
	long task_index = record_pool_allocate(g_4cf78c);

	if (task_index != NONE)
	{
		s_type_9df9da *task = (s_type_9df9da *)g_4cf78c->data + (task_index & 0xffff);
		task->handle = 0;
		task->type = NONE;
		task->controller_index = NONE;
		task->flags = 0;
	}
	return task_index;
}

// @retail 0x6b7e0
long online_task_get_type(long task_index)
{
	return ((s_type_9df9da *)g_4cf78c->data)[task_index & 0xffff].type;
}

/* retail inlines the data iterator here (unknown_16b570.cpp is built /Ob1) */
static inline void online_task_iterator_new(s_record_pool_iterator *iterator)
{
	iterator->data = g_4cf78c;
	iterator->index = NONE;
	iterator->datum_index = NONE;
}

// @retail 0x6b800
long online_task_find(long type, long controller_index)
{
	s_record_pool_iterator iterator;
	s_type_9df9da *task;

	online_task_iterator_new(&iterator);
	while ((task = (s_type_9df9da *)data_iterator_next_inlined(&iterator)) != 0)
	{
		if (task->type == type && (task->controller_index == controller_index || controller_index == NONE || controller_index == 0xff))
			return iterator.datum_index;
	}
	return NONE;
}

/* counts the matching tasks, but stops at the first (callers compare the
   count with 2) */
PRIVATE __forceinline long function_6b892(s_record_pool *arg_0, long arg_1)
{
 long local_0 = NONE;
 if (arg_1 >= 0)
 {
  for (; arg_1 < *(const volatile long *)&arg_0->high_water_index; arg_1++)
  {
   if (arg_0->bitmap[arg_1 >> 5] & (1 << (arg_1 & 0x1f)))
   { local_0 = arg_1; break; }
  }
 }
 return local_0;
}

PRIVATE __forceinline byte *function_6b891(s_record_pool_iterator *arg_0)
{
 s_record_pool *local_0 = arg_0->data;
 long local_1 = function_6b892(local_0, arg_0->index + 1);
 byte *local_2;
 if (local_1 != NONE)
 {
  local_2 = local_0->data + local_0->size * local_1;
  arg_0->index = local_1;
  arg_0->datum_index = (*(short *)local_2 << 16) | local_1;
 }
 else
 {
  arg_0->index = local_0->maximum_count;
  arg_0->datum_index = NONE;
  local_2 = 0;
 }
 return local_2;
}

// @retail 0x6b890
long online_task_exists(long type, long controller_index)
{
	s_record_pool_iterator iterator;
	s_type_9df9da *task;
	long count = 0;

	online_task_iterator_new(&iterator);
	while (count == 0 && (task = (s_type_9df9da *)function_6b891(&iterator)) != 0)
	{
		if (task->type == type && (task->controller_index == controller_index || controller_index == NONE || controller_index == 0xff))
			count++;
	}
	return count;
}

/* disposes every task, the logon (0), change-logon (1), friends (2), mute
   list startup (11) and 33 tasks only once the tasks that need them are gone */
// @retail 0x6b950
void online_tasks_dispose_all(void)
{
	while (g_4cf78c->actual_count > 0)
	{
		s_record_pool_iterator iterator;
		s_type_9df9da *task;

		online_task_iterator_new(&iterator);
		while ((task = (s_type_9df9da *)data_iterator_next_inlined(&iterator)) != 0)
		{
			bool dispose;

			switch (task->type)
			{
			case 0:
				dispose = g_4cf78c->actual_count == 1;
				break;
			case 1:
				dispose = g_4cf78c->actual_count <= 2;
				break;
			case 2:
				dispose = g_4cf78c->actual_count <= 3;
				break;
			case 11:
				dispose = !online_task_exists(12, 0xff);
				break;
			case 33:
				dispose = g_4cf78c->actual_count <= 4;
				break;
			case 44:
				dispose = true;
				break;
			default:
				dispose = true;
				break;
			}
			if (dispose)
				function_6b640(iterator.datum_index);
		}
	}
}

// @retail 0x6b450
void online_tasks_dispose(void)
{
	online_tasks_dispose_all();
	data_dispose(g_4cf78c);
	if (g_479748 != NONE)
	{
		function_6b640(g_479748);
		g_479748 = NONE;
	}
}

// @retail 0x6b910
s_type_9df9da *function_6b910(long task_index)
{
	return online_task_try_get(task_index);
}

// @retail 0x6ba80
long online_task_get_title(long task_index)
{
	long result = 0;
	s_type_9df9da *task = online_task_try_get(task_index);

	if (task)
	{
		switch (task->type)
		{
		case 0:
			result = 0x700061f;
			break;
		case 1:
			result = 0x14000620;
			break;
		case 2:
			result = 0x11000621;
			break;
		case 3:
			result = 0x15000622;
			break;
		case 4:
			result = 0x10000623;
			break;
		case 5:
			result = 0x1c000624;
			break;
		case 6:
			result = 0xe000625;
			break;
		case 7:
			result = 0x10000626;
			break;
		case 8:
			result = 0x10000627;
			break;
		case 9:
			result = 0x16000628;
			break;
		case 10:
			result = 0x10000629;
			break;
		case 11:
			result = 0x1200062a;
			break;
		case 12:
			result = 0x1600062b;
			break;
		case 13:
			result = 0x1900062c;
			break;
		case 14:
			result = 0x1400062d;
			break;
		case 15:
			result = 0x1600062e;
			break;
		case 16:
			result = 0x1200062f;
			break;
		case 17:
			result = 0x11000630;
			break;
		case 18:
			result = 0xb000631;
			break;
		case 19:
			result = 0xb000632;
			break;
		case 20:
			result = 0xd000633;
			break;
		case 21:
			result = 0x13000634;
			break;
		case 22:
			result = 0x15000635;
			break;
		case 23:
			result = 0x11000636;
			break;
		case 24:
			result = 0xd000637;
			break;
		case 25:
			result = 0xd000638;
			break;
		case 26:
			result = 0xd000639;
			break;
		case 27:
			result = 0x1400063a;
			break;
		case 28:
			result = 0x2100063b;
			break;
		case 29:
			result = 0x1500063c;
			break;
		case 30:
			result = 0x1f00063d;
			break;
		case 31:
			result = 0x1400063e;
			break;
		case 32:
			result = 0x1800063f;
			break;
		case 33:
			result = 0xa000640;
			break;
		case 34:
			result = 0x10000641;
			break;
		case 35:
			result = 0xe000642;
			break;
		case 36:
			result = 0x1a000643;
			break;
		case 37:
			result = 0x1c000644;
			break;
		case 38:
			result = 0x1a000645;
			break;
		case 39:
			result = 0x1d000646;
			break;
		case 41:
			result = 0x1f000647;
			break;
		case 42:
			result = 0x21000648;
			break;
		case 43:
			result = 0x20000649;
			break;
		case 44:
			result = 0x2200064a;
			break;
		case 45:
			result = 0xf00064b;
			break;
		default:
			result = NONE;
			break;
		}
	}
	return result;
}

// @retail 0x6bd10
long online_task_get_description(long task_index)
{
	long result = 0;
	s_type_9df9da *task = online_task_try_get(task_index);

	if (task)
	{
		switch (task->type)
		{
		case 0:
			result = 0x700064c;
			break;
		case 1:
			result = 0x1400064d;
			break;
		case 2:
			result = 0x1100064e;
			break;
		case 3:
			result = 0x1500064f;
			break;
		case 4:
			result = 0x10000650;
			break;
		case 5:
			result = 0x1c000651;
			break;
		case 6:
			result = 0xe000652;
			break;
		case 7:
			result = 0x10000653;
			break;
		case 8:
			result = 0x10000654;
			break;
		case 9:
			result = 0x16000655;
			break;
		case 10:
			result = 0x10000656;
			break;
		case 11:
			result = 0x12000657;
			break;
		case 12:
			result = 0x16000658;
			break;
		case 13:
			result = 0x19000659;
			break;
		case 14:
			result = 0x1400065a;
			break;
		case 15:
			result = 0x1600065b;
			break;
		case 16:
			result = 0x1200065c;
			break;
		case 17:
			result = 0x1100065d;
			break;
		case 18:
			result = 0xb00065e;
			break;
		case 19:
			result = 0xb00065f;
			break;
		case 20:
			result = 0xd000660;
			break;
		case 21:
			result = 0x13000661;
			break;
		case 22:
			result = 0x15000662;
			break;
		case 23:
			result = 0x11000663;
			break;
		case 24:
			result = 0xd000664;
			break;
		case 25:
			result = 0xd000665;
			break;
		case 26:
			result = 0xd000666;
			break;
		case 27:
			result = 0x14000667;
			break;
		case 28:
			result = 0x21000668;
			break;
		case 29:
			result = 0x15000669;
			break;
		case 30:
			result = 0x1f00066a;
			break;
		case 31:
			result = 0x1400066b;
			break;
		case 32:
			result = 0x1800066c;
			break;
		case 33:
			result = 0xa00066d;
			break;
		case 34:
			result = 0x1000066e;
			break;
		case 35:
			result = 0xe00066f;
			break;
		case 36:
			result = 0x1a000670;
			break;
		case 37:
			result = 0x1c000671;
			break;
		case 38:
			result = 0x1a000672;
			break;
		case 39:
			result = 0x1d000673;
			break;
		case 41:
			result = 0x1f000674;
			break;
		case 42:
			result = 0x21000675;
			break;
		case 43:
			result = 0x20000676;
			break;
		case 44:
			result = 0x22000677;
			break;
		case 45:
			result = 0xf000678;
			break;
		default:
			result = NONE;
			break;
		}
	}
	return result;
}

// @retail 0x6bfa0
void online_check_development_address(void)
{
	bool development = false;

	for (dword i = 0; i < sizeof(g_43ff84) / sizeof(g_43ff84[0]); i++)
	{
		if (!memcmp(g_4cf7cc, &g_43ff84[i], sizeof(s_online_address)))
		{
			development = true;
			break;
		}
	}
	g_50944e = development;
}

/* src/unknown_08d7c0.cpp, src/online_presence.cpp */
bool function_8d7c0(void);
void online_presence_task_clear(long task_index);


long g_467214 = NONE;
long g_467218 = 10;
bool g_50944f;

// @retail 0x6cd50
long online_task_get_logon_status(long task_index)
{
	s_type_9df9da *task;
	if (task_index == NONE || (task = function_6b910(task_index)) == 0)
		return g_467218;
	if (task->type == 1 || task->flag_bits.failed)
		return task->result;

	void *handle = task->handle;
	if (!handle || handle == (void *)NONE)
	{
		g_467218 = 10;
		return 10;
	}
	if (!function_8d7c0())
	{
		task->flag_bits.failed = true;
		task->result = 5;
		g_467218 = 5;
		return 5;
	}

	long result;

	switch (XOnlineLogonTaskGetResults((XONLINETASK_HANDLE)handle))
	{
	case XONLINE_E_LOGON_CANNOT_ACCESS_SERVICE:
		result = 4;
		break;
	case XONLINE_E_LOGON_CONNECTION_LOST:
		result = 5;
		break;
	case XONLINE_E_LOGON_INVALID_USER:
		result = 6;
		break;
	case XONLINE_E_LOGON_KICKED_BY_DUPLICATE_LOGON:
		result = 7;
		break;
	case XONLINE_E_LOGON_SERVERS_TOO_BUSY:
		result = 8;
		break;
	case XONLINE_E_LOGON_UPDATE_REQUIRED:
		result = 3;
		break;
	case XONLINE_E_LOGON_USER_ACCOUNT_REQUIRES_MANAGEMENT:
		result = 2;
		break;
	case XONLINE_S_LOGON_USER_HAS_MESSAGE:
		g_50944f = true;
	default:
		result = 10;
		break;
	case XONLINE_S_LOGON_CONNECTION_ESTABLISHED:
		result = 1;
		g_467218 = result;
		return result;
	case S_OK:
		result = 0;
		g_467218 = result;
		return result;
	}
	task->flag_bits.failed = true;
	task->result = result;
	g_467218 = result;
	return result;
}

// @retail 0x6c670
HRESULT online_task_continue(s_type_9df9da *task)
{
	HRESULT result = E_FAIL;
	if (task && task->handle && task->handle != (void *)NONE)
	{
		if (task->type == 0 && function_8d7c0() || online_logon_connected())
			result = XOnlineTaskContinue((XONLINETASK_HANDLE)task->handle);
		else
			result = 0x80151000;
	}
	return result;
}

// @retail 0x6c450
HRESULT online_task_update(s_type_9df9da *task)
{
	HRESULT result = online_task_continue(task);
	if (SUCCEEDED(result))
	{
		switch (result)
		{
		case XONLINETASK_S_RUNNING:
		case XONLINETASK_S_RUNNING_IDLE:
			break;
		case XONLINETASK_S_RESULTS_AVAIL:
			task->flags |= 2;
			break;
		default:
			task->flags = (task->flags & ~1) | 4;
			break;
		}
	}
	else
	{
		task->flags = (task->flags & ~1) | 0x24;
	}
	return result;
}

// @retail 0x6b640
void function_6b640(long task_index)
{
	s_type_9df9da *task = online_task_try_get(task_index);
	if (task)
	{
		void *handle = task->handle;
		if (handle && handle != (void *)NONE)
		{
			switch (task->type)
			{
			case 2:
				{
					HRESULT result = S_OK;
					do
					{
						if (result == XONLINETASK_S_RUNNING_IDLE)
							break;
						result = online_task_continue(task);
					} while (SUCCEEDED(result));
				}
				break;
			case 3:
				if (SUCCEEDED(XOnlineFriendsEnumerateFinish((XONLINETASK_HANDLE)handle)))
					online_task_continue(task);
				break;
			case 0x21:
				online_presence_task_clear(task_index);
				break;
			}
			XOnlineTaskClose((XONLINETASK_HANDLE)task->handle);
			task->handle = 0;
		}
		record_pool_release(g_4cf78c, task_index);
	}
}

// @retail 0x6b730
long online_task_new_if_logged_on(void)
{
	if (online_logon_connected())
		return online_task_new();
	return NONE;
}

// @retail 0x6b780
void online_task_restart(long task_index)
{
	s_type_9df9da *task = (s_type_9df9da *)g_4cf78c->data + (task_index & 0xffff);
	long type = task->type;
	long controller_index = task->controller_index;
	function_6b640(task_index);
	long new_index = datum_new_at_index_with_salt(g_4cf78c, task_index);
	s_type_9df9da *new_task = (s_type_9df9da *)g_4cf78c->data + (new_index & 0xffff);
	new_task->type = type;
	new_task->controller_index = controller_index;
	new_task->flags = 0;
}

// @retail 0x6c7c0
bool online_get_logon_status(long *status)
{
	bool valid = g_467214 != NONE;
	*status = online_task_get_logon_status(g_467214);
	return valid;
}

// @retail 0x6c7e0
bool function_6c7e0()
{
	return online_logon_connected();
}

// @retail 0x6b550
long online_get_nat_type(void)
{
	long nat_type = 0;
	if (online_logon_connected())
	{
		switch (XOnlineGetNatType())
		{
		case XONLINE_NAT_OPEN:
			nat_type = 1;
			break;
		case XONLINE_NAT_MODERATE:
			nat_type = 2;
			break;
		case XONLINE_NAT_STRICT:
			nat_type = 3;
			break;
		}
	}
	return nat_type;
}

// @retail 0x6d040
void online_set_notification_state(const XNKID *session_id, DWORD user_index, BYTE *state_data, DWORD state_flags)
{
	if (online_logon_connected())
	{
		XNKID id;
		if (session_id)
			id = *session_id;
		else
			memset(&id, 0, sizeof(id));
		XOnlineNotificationSetState(user_index, state_flags, id, 4, state_data);
	}
}

void __stdcall online_messages_enumerate(DWORD controller_index, s_entry *entries, long *count);

// @retail 0x6d080
bool function_6d080(long controller, s_channel_message *result)
{
	bool found = false;
	s_player_slot_flags *slot = &g_54e8e0[controller];
	if (!(slot->flags & 3) && online_logon_connected())
	{
		XONLINE_NOTIFICATION_EX_INFO notification;
		DWORD state_flags;
		found = XOnlineGetNotificationEx(controller, &notification, &state_flags) != FALSE;
		if (found)
		{
			switch (notification.bMessageType)
			{
			case 2: result->type = 1; break;
			case 3: result->type = 2; break;
			case 4: result->type = 3; break;
			case 5: result->type = 4; break;
			case 6: result->type = 5; break;
			case 7: result->type = 6; break;
			default: result->type = 0; break;
			}
			result->id = notification.dwMessageID;
			memset(&result->entry, 0, sizeof(result->entry));
			result->flags[0] = (notification.dwNotifyFlags & 8) != 0;
			result->flags[1] = (notification.dwNotifyFlags & 4) != 0;
			result->flags[2] = (notification.dwNotifyFlags & 0x100) != 0;
			result->flags[3] = (notification.dwNotifyFlags & 0xff000000) != 0;
			s_entry entries[125];
			long count = 125;
			online_messages_enumerate(controller, entries, &count);
			for (long i = 0; i < count; i++)
			{
				if (entries[i].unknown20 == result->id)
				{
					result->entry = entries[i];
					break;
				}
			}
		}
	}
	return found;
}

#include "unknown_075870.h"

extern s_network_observer *g_4cf8e4;
extern bool g_4cf95c;
void function_7f410(void);
void transport_reset(void);
long online_task_get_change_logon_status(s_type_9df9da *task);
bool __stdcall function_6bff0(s_type_9df9da *task);

static __forceinline void online_task_logon_failed(s_type_9df9da *task)
{
	task->flag_bits.finished = true;
	task->flag_bits.failed = true;
	if (task->result == 0 || task->result == 1)
		task->result = 10;
}

// @retail 0x6c2a0
bool function_6c2a0(s_type_9df9da *task, long task_index)
{
	if (!g_transport_globals.initialized || !g_transport_globals.started)
		task->flag_bits.finished = true;
	bool result = !task->flag_bits.finished;
	if (result)
	{
		online_task_continue(task);
		switch (online_task_get_logon_status(task_index))
		{
		case 0:
			break;
		case 1:
			if (!task->flag_bits.running)
			{
				g_4cf8e4->flag4e00 = true;
				g_4cf95c = true;
				function_7f410();
				transport_reset();
				if (function_6bff0(task))
					task->flag_bits.running = true;
				else
					online_task_logon_failed(task);
			}
			break;
		default:
			online_task_logon_failed(task);
			result = false;
			break;
		}
	}
	return result;
}

// @retail 0x6c360
bool function_6c360(s_type_9df9da *task)
{
	bool result = false;
	switch (online_task_continue(task))
	{
	case (long)0x80151001:
		task->result = 4;
		task->flag_bits.finished = true;
		task->flag_bits.failed = true;
		break;
	case (long)0x80151003:
		task->flag_bits.finished = true;
		task->flag_bits.failed = true;
		task->result = 8;
		break;
	case (long)0x80151002:
		task->flag_bits.finished = true;
		task->flag_bits.failed = true;
		task->result = 3;
		break;
	case (long)0x80151006:
		task->flag_bits.finished = true;
		task->flag_bits.failed = true;
		task->result = 6;
		break;
	case (long)0x80151300:
		{
			long status = online_task_get_change_logon_status(task);
			task->result = 10;
			if (status == 2)
			{
				task->flag_bits.finished = true;
				task->flag_bits.failed = true;
				task->result = status;
			}
			else
			{
				task->flag_bits.finished = true;
				task->flag_bits.failed = true;
			}
		}
		break;
	case 0:
	case 0x1513f0:
		task->result = 0;
		return true;
	case 0x1513f1:
		task->flag_bits.finished = true;
		task->result = 1;
		if (function_6bff0(task))
			return true;
		else
		{
			task->flag_bits.failed = true;
			if (task->result == 0 || task->result == 1)
				task->result = 10;
		}
		break;
	default:
		task->flag_bits.finished = true;
		task->flag_bits.failed = true;
		task->result = 10;
		break;
	}
	return result;
}


extern bool g_51055d;
long online_friends_startup(void);
void function_6c4a0(long result, long task_index);

// @retail 0x6c560
bool function_6c560(s_type_9df9da *task, long task_index)
{
 bool result = true;
 if ((task->flags & 1) && !(task->flags & 4))
 {
  switch (task->type)
  {
  case 0:
   result = function_6c2a0(task, task_index);
   break;
  case 1:
   function_6c360(task);
   break;
  case 2:
   {
    HRESULT status = online_task_update(task);
    if (FAILED(status) && status != (HRESULT)0x80151000)
     online_friends_startup();
   }
   break;
  case 41:
  case 42:
  case 43:
  case 44:
   {
    HRESULT status = online_task_update(task);
    if (!(task->flags & 1))
     function_6c4a0(status, task_index);
   }
   break;
  case 13:
   {
    HRESULT status = online_task_update(task);
    if ((task->flags & 4) && status == 0x153101)
     g_51055d = true;
   }
   break;
  default:
   online_task_update(task);
   break;
  }
 }
 if (task->flags & 4)
 {
  if (task->flags & 8)
   task->flags = (task->flags & ~8) | 16;
  else if (task->flags & 16)
   function_6b640(task_index);
 }
 return result;
}


struct s_presence_name_cache;
void function_8bf70(s_presence_name_cache *cache);

static __forceinline long online_task_next_absolute_index(s_record_pool *data, long index)
{
 long result = NONE;
 if (index >= 0 && index < data->high_water_index)
 {
  long count = data->high_water_index;
  dword *bits = data->bitmap;
  do
  {
   if (bits[index >> 5] & (1 << (index & 0x1f)))
   {
    result = index;
    break;
   }
   index++;
  } while (index < count);
 }
 return result;
}

static __forceinline s_type_9df9da *online_task_next(s_record_pool_iterator *iterator)
{
 s_record_pool *data = iterator->data;
 long index = online_task_next_absolute_index(data, iterator->index + 1);
 s_type_9df9da *result;
 if (index != NONE)
 {
  result = (s_type_9df9da *)(data->data + data->size * index);
  iterator->index = index;
  iterator->datum_index = (result->salt << 16) | index;
 }
 else
 {
  iterator->index = data->maximum_count;
  iterator->datum_index = NONE;
  result = 0;
 }
 return result;
}

// @retail 0x6b4a0
void function_6b4a0(void)
{
 if (g_467214 != NONE)
 {
  long status = online_task_get_logon_status(g_467214);
  if (status >= 0 && status <= 1)
  {
   s_record_pool_iterator iterator;
   s_type_9df9da *task;
   online_task_iterator_new(&iterator);
   while ((task = online_task_next(&iterator)) != 0)
   {
    if (!function_6c560(task, iterator.datum_index))
     break;
   }
   function_8bf70((s_presence_name_cache *)g_4771c8);
  }
 }
}
