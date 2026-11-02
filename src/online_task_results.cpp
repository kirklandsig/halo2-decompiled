// @flags /O2 /arch:SSE /Gr
/* ONLINE_TASK_RESULTS.CPP: what an online task reports to the interface:
   its progress, and the error string shown for the result it failed with */

#include "unknown_11c920.h"
#include "globals.h"
#include "online_tasks.h"
#include <xtl.h>

/* a Live result and the string of the error dialog that shows it */
struct s_online_error_string
{
	dword result;
	dword string;
};
typedef char assert_online_error_string_size[(sizeof(s_online_error_string) == 8) ? 1 : -1];

/* retail's table of the 40 results and their strings; it is the
   executable's data, so the declaration names its address */
s_online_error_string g_4672e0[40];

long __stdcall function_3ad1a6(long handle, long *out, long a, long b);

/* an online task's progress, from 0 to 1, by its task index; false when the
   task is gone or its progress cannot be read */
// @retail 0xb49a0
bool function_0b49a0(long task_index, real *progress)
{
	bool success = false;

	*progress = 0.0f;
	if (task_index != NONE)
	{
		s_record_pool *tasks = g_4cf78c;
		long absolute_index = task_index & 0xffff;
		if (absolute_index < tasks->high_water_index)
		{
			s_type_9df9da *task = (s_type_9df9da *)(tasks->data + tasks->size * absolute_index);
			if (task->salt != 0 && task->salt == (task_index >> 16))
			{
				long hundredths;
				if (function_3ad1a6((long)task->handle, &hundredths, 0, 0) >= 0)
				{
					*progress = (real)(dword)hundredths * 0.01f;
					success = true;
				}
			}
		}
	}

	return success;
}

/* the string of the error dialog for a Live result, 0x6f for one the table
   does not list */
// @retail 0xb4a20
dword online_result_get_error_string(dword result)
{
	dword string = 0x6f;

	for (dword i = 0; i < 40; i++)
	{
		if (g_4672e0[i].result == result)
		{
			string = g_4672e0[i].string;
			break;
		}
	}

	return string;
}
