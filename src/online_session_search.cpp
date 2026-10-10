// @flags /O2 /arch:SSE /Gr
/* ONLINE_SESSION_SEARCH.CPP: the Xbox Live matchmaking session searches: the
   online tasks that create, find and delete match sessions, the results of a
   search, and the session identifiers already seen. */

#include "unknown_11c920.h"
#include "globals.h"
#include "online_tasks.h"
#include "online_attributes.h"
#include "unknown_0662e0.h"
#include "network_qos.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include <stdlib.h>

#define MAXIMUM_SEARCH_RESULTS 50

void function_6b640(long task_index);
long online_task_new_if_logged_on(void);
void qos_release(long handle);

/* the release routine retail inlines here */
static inline void free_block(void *block)
{
	long info;
	g_4d87f8->allocator->get_info(block, &info);
	s_allocator_globals *globals = g_4d87f8;
	globals->allocator->release(block, NONE);
	if (block != 0)
		globals->count--;
}

static inline s_type_9df9da *online_task_try_and_get(long task_index)
{
	s_type_9df9da *task = 0;
	if (task_index != NONE)
	{
		s_record_pool *data = g_4cf78c;
		long absolute_index = task_index & 0xffff;
		if (absolute_index < data->high_water_index)
		{
			s_type_9df9da *candidate = (s_type_9df9da *)(data->data + data->size * absolute_index);
			if (candidate->salt != 0 && candidate->salt == (task_index >> 16))
				task = candidate;
		}
	}
	return task;
}

/* a session a search found (0x204 bytes) */
struct s_search_result
{
	bool valid;
	bool unknown01;
	bool seen;
	byte unknown03;
	long unknown04;
	bool unknown08;
	byte unknown09[3];
	byte search_result[0x68];
	bool unknown74;
	bool unknown75;
	byte unknown76[2];
	long unknown78;
	long unknown7c;
	long unknown80;
	byte unknown84[0x90 - 0x84];
	long field90;
	byte unknown94[0xaa - 0x94];
	short session_kind;
	byte unknownac[0xbc - 0xac];
	XNKID session_id;
	XNKEY session_key;
	XNADDR session_address;
	byte unknownf8[0x10c - 0xf8];
	long field10c;
	byte unknown110[0x1f8 - 0x110];
	long field1f8;
	long field1fc;
	long field200;
};

/* a session search */
struct s_session_search
{
	bool active;
	byte unknown01[3];
	dword start_time;
	long state;
	byte unknown0c[8];
	bool flag14;
	byte unknown15[7];
	long unknown1c;
	long unknown20;
	byte unknown24[0x34 - 0x24];
	long minimum_score;
	byte unknown38[0x70 - 0x38];
	long task_index;
	long qos_handles[2];
	long qos_counts[2];
	long result_count;
	long unknown88;
	s_search_result *results;
	void *unknown90;
	long seen_count;
	long seen_capacity;
	long unknown9c;
	XNKID *seen;
};

/* the parts of a session a search compares */
struct s_search_session
{
	long kind;
	XNKID id;
	XNKEY key;
	XNADDR address;
	long value;
};

long g_4ce394;
long g_4ce118;
long g_4ce11c;
long g_4ce200[10];
long g_4ce230;
long g_4ce234;
long g_4ce238;
long g_4ce23c[9][9];
long g_4ce380;
long g_4ce384;
double g_4ce388;
long g_4ce390;

extern bool g_4cf792;
extern XNADDR g_4cf793;
bool function_07a9b0(void);

#define SEARCH_LONG(object, offset) (*(long *)((byte *)(object) + (offset)))
#define SEARCH_FLAG(object, offset) (*(bool *)((byte *)(object) + (offset)))

typedef bool (__stdcall *t_search_compare)(void const *, void const *, void const *);
void function_13da70(void *elements, unsigned long count, unsigned long element_size, t_search_compare compare, void const *context);

struct s_long7
{
	long v[7];
};

struct s_match_result_data
{
	XNKEY key;
	XNKID id;
	XNADDR address;
	DWORD public_filled;
	DWORD public_open;
	DWORD private_filled;
	DWORD private_open;
	s_long7 properties;
};

void function_0b4a40(s_long7 *out, const __int64 *in);
long online_match_search(s_range_input const *input);

// @retail 0x90160
bool function_90160(long task_index, s_match_result_data *result)
{
	s_type_9df9da *task = online_task_try_and_get(task_index);
	bool success = false;
	if (task && online_logon_connected() && (task->flags & 4))
	{
		PXONLINE_MATCH_SEARCHRESULT *results;
		DWORD count;
		if (SUCCEEDED(XOnlineMatchSearchGetResults((XONLINETASK_HANDLE)task->handle, &results, &count)) && count > 0)
		{
			memset(result, 0, sizeof(*result));
			result->address = results[0]->HostAddress;
			result->key = results[0]->KeyExchangeKey;
			result->id = results[0]->SessionID;
			result->public_filled = results[0]->dwPublicFilled;
			result->public_open = results[0]->dwPublicOpen;
			result->private_filled = results[0]->dwPrivateFilled;
			result->private_open = results[0]->dwPrivateOpen;
			success = true;
			if (results[0]->dwNumAttributes == 7)
			{
				__int64 properties[7];
				if (SUCCEEDED(XOnlineMatchSearchParse(results[0], results[0]->dwNumAttributes, g_44050c, properties)))
					function_0b4a40(&result->properties, properties);
			}
		}
	}
	return success;
}

// @retail 0x90b30
void function_90b30(s_session_search *search)
{
	long current = search->unknown1c;
	if (current != NONE)
	{
		long minimum = SEARCH_LONG(search, 0x68) - current;
		if (minimum < SEARCH_LONG(search, 0x60))
			minimum = SEARCH_LONG(search, 0x60);
		SEARCH_LONG(search, 0x30) = minimum;
		long maximum = SEARCH_LONG(search, 0x50) + current;
		if (maximum > SEARCH_LONG(search, 0x64))
			maximum = SEARCH_LONG(search, 0x64);
		SEARCH_FLAG(search, 0x2c) = true;
		SEARCH_FLAG(search, 0x24) = true;
		SEARCH_LONG(search, 0x28) = maximum;
	}
	else
	{
		SEARCH_FLAG(search, 0x2c) = false;
		SEARCH_FLAG(search, 0x24) = false;
	}
	if (current != NONE && search->unknown20 > 0)
		search->minimum_score = (long)((real)g_network_configuration.value1e8 + ((real)current / (real)search->unknown20) * ((real)g_network_configuration.value1ec - (real)g_network_configuration.value1e8));
	else
		search->minimum_score = g_network_configuration.value1ec;
	s_range_input input;
	memset(&input, 0, sizeof(input));
	input.x = SEARCH_LONG(search, 0x10);
	input.y = SEARCH_LONG(search, 0x4c);
	input.has_min = SEARCH_FLAG(search, 0x2c);
	input.min = SEARCH_LONG(search, 0x30);
	input.has_max = SEARCH_FLAG(search, 0x24);
	input.max = SEARCH_LONG(search, 0x28);
	input.count = SEARCH_LONG(search, 0x38);
	memset(search->results, 0, search->unknown88);
	memset(search->unknown90, 0, search->unknown88);
	input.has_count = true;
	search->task_index = online_match_search(&input);
	if (search->task_index == NONE)
		search->state = 4;
	else
		search->start_time = g_510548 ? g_51054c : GetTickCount();
}

struct s_qos_target
{
	XNKID kid;
	XNKEY key;
	XNADDR xna;
};

long qos_lookup(long kind, long count, long bits_per_second, s_qos_target *targets);

// @retail 0x90020
long online_match_session_delete(s_search_session const *session, bool *unavailable)
{
	bool service_unavailable = false;
	online_task_exists(8, 0xff);
	long task_index = online_task_new_if_logged_on();
	if (task_index != NONE)
	{
		s_type_9df9da *task = online_task_try_and_get(task_index);
		if (task)
		{
			HRESULT result = XOnlineMatchSessionDelete(session->id, NULL, (PXONLINETASK_HANDLE)&task->handle);
			if (SUCCEEDED(result))
			{
				task->flags = 1;
				task->type = 8;
				task->controller_index = NONE;
				*unavailable = service_unavailable;
				return task_index;
			}
			if (result == 0x80155100)
				service_unavailable = true;
			function_6b640(task_index);
			task_index = NONE;
		}
	}
	*unavailable = service_unavailable;
	return task_index;
}

// @retail 0x900e0
long online_match_session_find(XNKID const *session_id)
{
	if (online_task_exists(9, 0xff) > 2)
		return NONE;
	long task_index = online_task_new_if_logged_on();
	s_type_9df9da *task = function_6b910(task_index);
	if (task)
	{
		if (FAILED(XOnlineMatchSessionFindFromID(*session_id, NULL, (PXONLINETASK_HANDLE)&task->handle)))
		{
			function_6b640(task_index);
			return NONE;
		}
		task->flags = 1;
		task->type = 9;
		task->controller_index = NONE;
	}
	return task_index;
}

// @retail 0x90c80
void __stdcall function_090c80(byte *p)
{
	byte *const *reference = &p;
	s_session_search *search = (s_session_search *)*reference;
	if (search->state == 1)
		search->state = 2;
	if (search->task_index != NONE)
	{
		function_6b640(search->task_index);
		search->task_index = NONE;
	}
	if (search->qos_handles[0] != NONE)
	{
		qos_release(search->qos_handles[0]);
		search->qos_handles[0] = NONE;
	}
	if (search->qos_handles[1] != NONE)
	{
		qos_release(search->qos_handles[1]);
		search->qos_handles[1] = NONE;
	}
	if (search->unknown90)
	{
		free_block(search->unknown90);
		search->unknown90 = 0;
	}
	if (search->results)
	{
		free_block(search->results);
		search->results = 0;
	}
	if (search->seen)
	{
		free_block(search->seen);
		search->seen = 0;
	}
	search->active = false;
}


// @retail 0x91290
bool session_search_seen(s_session_search *search, XNKID const *id)
{
	bool seen = false;
	for (long i = 0; i < search->seen_count && !seen; i++)
	{
		if (memcmp(&search->seen[i], id, sizeof(XNKID)) == 0)
			seen = true;
	}
	return seen;
}

struct s_online_match_result;
void function_8fb30(long task_index, s_online_match_result *output, word *capacity);

// @retail 0x902b0
void function_902b0(s_session_search *arg_0)
{
	s_match_result_data local_0[50];
	long local_1 = arg_0->task_index;
	if (local_1 != NONE)
	{
		long local_2 = online_task_poll(local_1);
		if (local_2 >= 0 && local_2 <= 1)
			return;
		if (local_2 == 2)
		{
			word local_3 = 50;
			function_8fb30(local_1, (s_online_match_result *)local_0, &local_3);
			arg_0->result_count = local_3;
			long local_4 = 0;
			for (; local_4 < arg_0->result_count; local_4++)
			{
				arg_0->results[local_4].valid = true;
				arg_0->results[local_4].seen = session_search_seen(arg_0, &local_0[local_4].id);
				arg_0->results[local_4].unknown01 = !arg_0->results[local_4].seen;
				arg_0->results[local_4].unknown08 = true;
				arg_0->results[local_4].unknown04 = local_4;
				memcpy(arg_0->results[local_4].search_result, &local_0[local_4], sizeof(local_0[local_4]));
				arg_0->results[local_4].unknown7c = NONE;
				*(word *)&arg_0->results[local_4].unknown74 = 0;
			}
			for (; local_4 < 50; local_4++)
				arg_0->results[local_4].valid = false;
		}
		if (arg_0->task_index != NONE)
		{
			function_6b640(arg_0->task_index);
			arg_0->task_index = NONE;
		}
	}
}


/* the session identifier in a result */
static inline XNKID const *search_result_id(s_search_result const *result)
{
	return (XNKID const *)(result->search_result + 0x10);
}

// @retail 0x912d0
void session_search_mark_seen(s_session_search *search, s_search_result *result)
{
	if (!result->seen)
	{
		XNKID const *id = search_result_id(result);
		result->seen = true;
		result->unknown01 = false;
		if (!session_search_seen(search, id) && search->seen_count < search->seen_capacity)
		{
			search->seen[search->seen_count] = *id;
			search->seen_count++;
		}
	}
}

struct s_session_message_values;
bool function_7e080(byte const *data, long size, s_session_message_values *values);
long function_75870(void);
bool qos_is_complete(long handle);
long qos_target_status(long handle, long index);

// @retail 0x905a0
void __stdcall function_905a0(s_session_search *search, long kind)
{
	if (search->qos_handles[kind] != NONE)
	{
		bool complete = qos_is_complete(search->qos_handles[kind]);
		for (long i = 0; i < MAXIMUM_SEARCH_RESULTS; i++)
		{
			s_search_result *entry = &search->results[i];
			if (entry->valid && entry->unknown01 && entry->unknown7c != NONE && entry->unknown78 == kind)
			{
				long state = qos_target_status(search->qos_handles[kind], entry->unknown7c);
				if (state != entry->unknown80)
				{
					entry->unknown80 = state;
					switch (state)
					{
					case 0: break;
					case 1: break;
					case 2: break;
					case 3: session_search_mark_seen(search, entry); break;
					case 4: session_search_mark_seen(search, entry); break;
					case 5:
						{
							s_qos_result *result = (s_qos_result *)((byte *)entry + 0x84);
							if (!qos_target_result(search->qos_handles[kind], result, entry->unknown7c))
								session_search_mark_seen(search, entry);
							else if (!result->data_size)
								session_search_mark_seen(search, entry);
							else
							{
								dword values[0x53];
								if (function_7e080(result->data, result->data_size, (s_session_message_values *)values))
								{
									memcpy((byte *)entry + 0xa8, values, sizeof(values));
									SEARCH_FLAG(entry, 0xa4) = true;
									SEARCH_LONG(entry, 0x1f4) = function_75870();
									*((bool *)entry + 0x74 + kind) = true;
									entry->unknown7c = NONE;
								}
								else
									session_search_mark_seen(search, entry);
								result->data_size = 0;
								result->data = 0;
							}
						}
						break;
					default: __assume(0);
					}
				}
			}
		}
		if (complete)
		{
			for (long i = 0; i < MAXIMUM_SEARCH_RESULTS; i++)
			{
				s_search_result *entry = &search->results[i];
				if (entry->valid && entry->unknown01 && entry->unknown7c != NONE && entry->unknown78 == kind)
					entry->unknown7c = NONE;
			}
			if (search->qos_handles[kind] != NONE)
			{
				qos_release(search->qos_handles[kind]);
				search->qos_handles[kind] = NONE;
			}
		}
	}
}

// @retail 0x91320
void session_search_mark_session(s_session_search *search, s_search_session const *session)
{
	if (search->active)
	{
		for (s_search_result *result = search->results; result < search->results + MAXIMUM_SEARCH_RESULTS; result++)
		{
			if (result->valid && result->unknown74 && memcmp(&session->id, (byte *)result + 0xbc, sizeof(XNKID)) == 0)
			{
				session_search_mark_seen(search, result);
				return;
			}
		}
	}
}

// @retail 0x91380
void session_search_get_progress(s_session_search *search, long *first, long *last, long *count, long *total)
{
	if (search->active)
	{
		long start;
		long end;
		if (search->flag14)
		{
			end = search->unknown20;
			start = search->unknown1c;
		}
		else
		{
			long current = search->unknown20;
			long previous = search->unknown1c;
			end = current + 1;
			if (previous != NONE)
				start = previous;
			else
				start = current;
		}
		long result_count = 0;
		long result_total = 0;
		for (s_search_result *result = search->results; result < search->results + MAXIMUM_SEARCH_RESULTS; result++)
		{
			if (result->valid)
			{
				result_count++;
				result_total += *(long *)((byte *)result + 0x48);
			}
		}
		*first = start;
		*last = end;
		*count = result_count;
		*total = result_total;
	}
}

// @retail 0x90f10
bool __stdcall session_search_result_precedes(void const *a, void const *b, void const *context)
{
	s_search_result const *first = (s_search_result const *)a;
	s_search_result const *second = (s_search_result const *)b;
	if (first->field1f8 < second->field1f8)
		return true;
	if (first->field1f8 > second->field1f8)
		goto failed;
	if (first->unknown74)
	{
		if (first->field90 > second->field90)
			return true;
		if (first->field90 < second->field90)
			goto failed;
	}
	if (first->field1fc > second->field1fc)
		return true;
	if (first->field1fc < second->field1fc)
		goto failed;
	if (first->field10c > second->field10c)
		return true;
failed:
	return false;
}

static inline bool session_search_version_allowed(long type, long lower, long upper)
{
	return type == 4 && lower >= 0x2651 && upper <= 0x2651;
}

// @retail 0x90d90
bool __stdcall session_search_result_allowed(s_session_search *search, s_search_result *entry)
{
	bool result = true;
	long minimum;
	long mask;
	volatile long maximum;
	if (SEARCH_FLAG(entry, 0xa4))
	{
		mask = SEARCH_LONG(entry, 0x110);
		minimum = SEARCH_LONG(entry, 0x12c);
		maximum = SEARCH_LONG(entry, 0x124);
	}
	else
	{
		mask = SEARCH_LONG(entry, 0x70);
		minimum = SEARCH_LONG(entry, 0x64);
		maximum = SEARCH_LONG(entry, 0x68);
	}
	if (SEARCH_FLAG(entry, 0xa4))
	{
		long local_0 = SEARCH_LONG(entry, 0xac);
		long upper = *(volatile long *)((byte *)entry + 0xb4);
		dword lower = *(volatile long *)((byte *)entry + 0xb0);
		if (!session_search_version_allowed(local_0, lower, upper))
			return false;
	}
	if (g_transport_globals.initialized && g_transport_globals.started)
		function_07a9b0();
	bool address_valid = g_4cf792;
	XNADDR address = g_4cf793;
	if (address_valid && memcmp(&address, (byte *)entry + 0x24, sizeof(address)) == 0)
		return false;
	if (SEARCH_FLAG(entry, 0xa4) && SEARCH_FLAG(search, 0x15) != 0)
	{
		long count = SEARCH_LONG(entry, 0x130);
		if (count > 0)
		{
			for (long i = 0; i < count; i++)
			{
				if (memcmp((byte *)entry + 0x134 + i * 12, 0x3d + (byte *)search, 12) == 0)
					result = false;
			}
			if (!result)
				goto done;
		}
	}
	if (!(mask & (1 << (SEARCH_LONG(search, 0x38) - 1))))
		result = false;
	if (SEARCH_FLAG(search, 0x2c) && minimum < SEARCH_LONG(search, 0x30))
		result = false;
	if (SEARCH_FLAG(search, 0x24) && maximum > SEARCH_LONG(search, 0x28))
		return false;
done:
	return result;
}

// @retail 0x91000
void __stdcall session_search_score_and_sort(s_session_search *search)
{
	if (memcmp(search->unknown90, search->results, search->unknown88) == 0)
		return;
	for (s_search_result *entry = search->results; entry < search->results + MAXIMUM_SEARCH_RESULTS; entry++)
	{
		entry->field1f8 = 0;
		if (entry->valid && entry->unknown01 && !session_search_result_allowed(search, entry))
			entry->unknown01 = false;
		if (entry->valid && entry->unknown01)
		{
			bool has_details = SEARCH_FLAG(entry, 0xa4);
			long kind;
			long value;
			long count;
			if (has_details)
			{
				kind = entry->field10c;
				value = SEARCH_LONG(entry, 0x120);
				count = SEARCH_LONG(entry, 0x11c);
			}
			else
			{
				kind = SEARCH_LONG(entry, 0x60);
				value = SEARCH_LONG(entry, 0x5c);
				count = SEARCH_LONG(entry, 0x6c);
			}
			entry->field1fc = abs(value - SEARCH_LONG(search, 0x4c));
			if (entry->field1fc <= g_4ce230)
				entry->field1f8 += g_4ce234 - ((g_4ce234 - g_4ce238) / (g_4ce230 + 1)) * entry->field1fc;
			if (kind == SEARCH_LONG(search, 0x38))
				entry->field1f8 += g_4ce380;
			long bounded = count < 0 ? 0 : (count > g_4ce384 ? g_4ce384 : count);
			entry->field200 = bounded;
			entry->field1f8 = (long)(entry->field1f8 + bounded * g_4ce388);
			if (has_details)
			{
				if ((SEARCH_LONG(entry, 0xf8) < g_4ce11c || SEARCH_LONG(entry, 0xfc) < g_4ce118) &&
					SEARCH_LONG(search, 0x58) >= g_4ce11c && SEARCH_LONG(search, 0x5c) >= g_4ce118)
					entry->field1f8 += g_4ce390;
				entry->field1f8 += g_4ce23c[SEARCH_LONG(search, 0x6c)][SEARCH_LONG(entry, 0xb8)];
			}
			if (entry->unknown74)
			{
				real value = entry->field90 * 0.01f;
				value = value < 0.0f ? 0.0f : (value > 9.0f ? 9.0f : value);
				long index = (long)value;
				long score;
				if (index < 9)
				{
					real lower = (real)g_4ce200[index];
					real upper = (real)g_4ce200[index + 1];
					score = (long)(lower + (value - index) * (upper - lower));
				}
				else
					score = g_4ce200[9];
				entry->field1f8 += score;
			}
		}
	}
	function_13da70(search->results, MAXIMUM_SEARCH_RESULTS, sizeof(s_search_result), session_search_result_precedes, 0);
	memcpy(search->unknown90, search->results, search->unknown88);
}

static inline long session_search_elapsed(dword start_time)
{
	return (long)((g_510548 ? g_51054c : GetTickCount()) - start_time);
}

// @retail 0x91400
bool session_search_select(s_session_search *search, s_search_session *session)
{
	bool result = false;
	s_search_result *selected = 0;
	long candidate_count = 0;
	long ready_count = 0;
	if (search->active)
	{
		for (s_search_result *entry = search->results; entry < search->results + MAXIMUM_SEARCH_RESULTS; entry++)
		{
			if (entry->valid)
			{
				result = entry->unknown01;
				if (result)
				{
					candidate_count++;
					result = entry->unknown74;
					if (result)
					{
						ready_count++;
						if (entry->field1f8 >= search->minimum_score && selected == 0)
							selected = entry;
					}
				}
			}
		}
		if (selected && (ready_count == candidate_count ||
			(ready_count >= (candidate_count >> 2) &&
			session_search_elapsed(search->start_time) > g_4ce394)))
		{
			if (session)
			{
				memset(session, 0, sizeof(*session));
				session->kind = selected->session_kind;
				session->id = selected->session_id;
				session->key = selected->session_key;
				session->address = selected->session_address;
				session->value = selected->field90;
			}
			return true;
		}
		return false;
	}
	return result;
}

// @retail 0x90420
void __stdcall function_90420(s_session_search *arg_0, long arg_1)
{
	if (arg_0->qos_handles[arg_1] == NONE)
	{
		long local_0 = 0;
		s_qos_target local_1[MAXIMUM_SEARCH_RESULTS];
		for (long local_2 = 0; local_2 < MAXIMUM_SEARCH_RESULTS; local_2++)
		{
			s_search_result *local_3 = &arg_0->results[local_2];
			if (local_3->valid && local_3->unknown01)
			{
				bool local_4 = false;
				if (!((bool *)&local_3->unknown74)[arg_1])
					local_4 = true;
				if (arg_1 == 0)
				{
					if (local_3->unknown74 && session_search_elapsed(SEARCH_LONG(local_3, 0x1f4)) > g_network_configuration.value35c)
						local_4 = true;
					local_4 = local_4 && local_3->unknown75;
				}
				if (local_4)
				{
					local_1[local_0].kid = *(XNKID *)(local_3->search_result + 0x10);
					local_1[local_0].key = *(XNKEY *)local_3->search_result;
					local_1[local_0].xna = *(XNADDR *)(local_3->search_result + 0x18);
					local_3->unknown78 = arg_1;
					local_3->unknown7c = local_0;
					local_0++;
					local_3->unknown80 = 0;
				}
			}
		}
		if (local_0 > 0)
		{
			arg_0->qos_handles[arg_1] = qos_lookup(arg_1, local_0, g_network_configuration.value358, local_1);
			if (arg_0->qos_handles[arg_1])
				arg_0->qos_counts[arg_1] = local_0;
			else
				arg_0->state = 5;
		}
	}
}

// @retail 0x90f70
void function_90f70(s_session_search *arg_0)
{
	if (arg_0->state != 0 && arg_0->task_index == NONE && arg_0->qos_handles[0] == NONE &&
		arg_0->qos_handles[1] == NONE && !session_search_select(arg_0, 0))
	{
		long local_0 = arg_0->result_count;
		long local_2 = SEARCH_LONG(arg_0, 0xc) + local_0;
		SEARCH_LONG(arg_0, 0xc) = local_2;
		bool local_1 = false;
		if (SEARCH_LONG(arg_0, 0xc) < g_network_configuration.value1b8)
		{
			if (arg_0->unknown1c != NONE)
			{
				if (arg_0->unknown1c < arg_0->unknown20)
				{
					arg_0->unknown1c++;
					local_1 = true;
				}
				else if (arg_0->unknown1c == arg_0->unknown20 && !arg_0->flag14)
				{
					arg_0->unknown1c = NONE;
					local_1 = true;
				}
			}
			if (local_0 != MAXIMUM_SEARCH_RESULTS && !local_1)
				arg_0->state = 0;
			else
				function_90b30(arg_0);
		}
		else
			arg_0->state = 0;
	}
}


struct s_surface_description;
s_surface_description *function_192e60(long arg_0);
long function_193250(s_surface_description *arg_0);
long function_1931a0(long arg_0, s_surface_description *arg_1);

struct s_90880
{
	long field_0;
	byte field_4[0x518];
	char field_51c[128];
};

struct s_909ff
{
	dword field_0[3];
};

PRIVATE inline void *function_90891(long arg_0)
{
	s_allocator_globals *local_0 = g_4d87f8;
	void *local_1 = local_0->allocator->allocate(arg_0, 0, 0);
	if (!local_1)
	{
		local_0->allocator->compact(0);
		local_1 = local_0->allocator->allocate(arg_0, 0, 0);
	}
	if (local_1)
		local_0->count++;
	return local_1;
}

// @retail 0x90880
bool function_90880(s_session_search *arg_0, long arg_1, long arg_2, long arg_3, long arg_4, long arg_5, long arg_6, long arg_7, s_909ff const *arg_8, long arg_9)
{
	s_surface_description *local_0 = function_192e60(arg_1);
	arg_0->unknown88 = 50 * sizeof(s_search_result);
	arg_0->results = (s_search_result *)function_90891(arg_0->unknown88);
	arg_0->unknown90 = function_90891(arg_0->unknown88);
	arg_0->seen_capacity = g_network_configuration.value1f8;
	arg_0->unknown9c = arg_0->seen_capacity * sizeof(XNKID);
	arg_0->seen = (XNKID *)function_90891(arg_0->unknown9c);
	if (arg_0->results && arg_0->unknown90 && arg_0->seen)
	{
		SEARCH_LONG(arg_0, 0x6c) = arg_9;
		SEARCH_LONG(arg_0, 0x4c) = arg_3;
		SEARCH_LONG(arg_0, 0x50) = arg_4;
		SEARCH_LONG(arg_0, 0x54) = arg_5;
		SEARCH_LONG(arg_0, 0x10) = arg_1;
		s_90880 *local_1 = (s_90880 *)local_0;
		arg_0->flag14 = local_1->field_0 == 5 || local_1->field_0 == 2 || local_1->field_0 == 4;
		SEARCH_FLAG(arg_0, 0x15) = local_1->field_0 == 5;
		SEARCH_LONG(arg_0, 0x18) = function_193250(local_0);
		long local_2 = function_1931a0(arg_4, local_0);
		SEARCH_LONG(arg_0, 0x60) = local_2;
		SEARCH_LONG(arg_0, 0x68) = local_2 > arg_5 ? local_2 : arg_5;
		SEARCH_LONG(arg_0, 0x64) = local_1->field_51c[SEARCH_LONG(arg_0, 0x68)];
		long local_3 = SEARCH_LONG(arg_0, 0x68) - SEARCH_LONG(arg_0, 0x60);
		long local_4 = SEARCH_LONG(arg_0, 0x64) - arg_4;
		arg_0->unknown20 = local_3 > local_4 ? local_3 : local_4;
		SEARCH_LONG(arg_0, 0x38) = arg_2;
		arg_0->unknown1c = 0;
		SEARCH_FLAG(arg_0, 0x3c) = arg_8 != 0;
		if (arg_8)
			*(s_909ff *)((byte *)arg_0 + 0x3d) = *arg_8;
		SEARCH_LONG(arg_0, 0x58) = arg_6;
		SEARCH_LONG(arg_0, 0x5c) = arg_7;
		SEARCH_LONG(arg_0, 0xc) = 0;
		arg_0->seen_count = 0;
		arg_0->active = true;
		arg_0->state = 1;
		function_90b30(arg_0);
	}
	else
	{
		if (arg_0->results)
		{
			free_block(arg_0->results);
			arg_0->results = 0;
		}
		if (arg_0->unknown90)
		{
			free_block(arg_0->unknown90);
			arg_0->unknown90 = 0;
		}
		if (arg_0->seen)
		{
			free_block(arg_0->seen);
			arg_0->seen = 0;
		}
	}
	return arg_0->active;
}

// @retail 0x90840
void function_90840(s_session_search *arg_0)
{
	if (arg_0->active)
	{
		function_902b0(arg_0);
		function_905a0(arg_0, 0);
		function_905a0(arg_0, 1);
		session_search_score_and_sort(arg_0);
		function_90420(arg_0, 0);
		function_90420(arg_0, 1);
		function_90f70(arg_0);
	}
}
