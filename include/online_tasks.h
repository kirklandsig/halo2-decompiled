/* ONLINE_TASKS.H: the online tasks (g_4cf78c) and the online account code
   that drives them (lane D) */

#ifndef ONLINE_TASKS_H
#define ONLINE_TASKS_H

#include "unknown_11c920.h"
#include "data_array.h"

/* a machine's network address */
struct s_online_address
{
	byte bytes[6];
};

/* one online task (0x14 bytes): flags bit 0 started, bit 1 running, bit 2
   finished, bit 5 failed */
struct s_type_9df9da
{
	short salt;
	union
	{
		word flags;
		struct
		{
			word started : 1;
			word running : 1;
			word finished : 1;
			word unknown3 : 2;
			word failed : 1;
		} flag_bits;
	};
	long type;
	long controller_index;
	void *handle;
	long result;
};

/* the tasks */
extern s_record_pool *g_4cf78c;

/* retail inlines record_pool_lookup into the online code (unknown_16b570.cpp is
   built /Ob1) */
static inline s_type_9df9da *online_task_try_get(long task_index)
{
	s_type_9df9da *result = 0;

	if (task_index != NONE)
	{
		s_record_pool *data = g_4cf78c;
		long index = task_index & 0xffff;

		if (index < data->high_water_index)
		{
			byte *datum = data->data + data->size * index;
			short salt = *(short *)datum;

			if (salt != 0 && salt == (task_index >> 16))
			{
				result = (s_type_9df9da *)datum;
			}
		}
	}

	return result;
}

/* a task by datum index, without the salt check */
static inline s_type_9df9da *online_task_get_unchecked(long task_index)
{
	return (s_type_9df9da *)g_4cf78c->data + (task_index & 0xffff);
}


void online_tasks_initialize(void);
long online_task_poll(long task_index);
inline long online_task_new(void);
long online_task_get_type(long task_index);
long online_task_find(long type, long controller_index);
long online_task_exists(long type, long controller_index);
s_type_9df9da *function_6b910(long task_index);
void function_6b640(long task_index);
long online_task_new_if_logged_on(void);
void online_task_restart(long task_index);
long online_task_get_title(long task_index);
long online_task_get_description(long task_index);
void online_check_development_address(void);

/* the Live logon task (online_tasks.cpp) */
extern long g_467214;
long online_task_get_logon_status(long task_index);

static inline bool online_logon_connected(void)
{
	bool connected = false;
	if (g_467214 != NONE)
	{
		switch (online_task_get_logon_status(g_467214))
		{
		case 1:
			connected = true;
			break;
		}
	}
	return connected;
}

#endif
