// @flags /O2 /Ob1 /Gr
/* UNKNOWN_209580.CPP: runs a script thread if it is due (script_thread_runner; lane
   I's outside function, kept out of line as in retail, where the command
   scripts' 0x258880 calls it) */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"

/* a script thread (g_4f9384, 0x418 bytes; unknown_209520.cpp) */
struct s_hs_due_thread
{
	byte unknown00[8];
	long sleep_until;
	byte unknown0c[0x418 - 0xc];
};

extern s_record_pool *g_4f9384;

void function_209850(long thread_index); /* unknown_209520.cpp */

/* runs the thread if it is due: 0 when it finished, 1 when it sleeps, 2 when
   it waits */
// @retail 0x209580
short function_209580(long thread_index)
{
	s_hs_due_thread *thread = (s_hs_due_thread *)(g_4f9384->data + (thread_index & 0xffff) * sizeof(s_hs_due_thread));
	short result = 2;

	if (thread->sleep_until == -3)
	{
		thread->sleep_until = 0;
	}
	if (thread->sleep_until >= 0 && thread->sleep_until <= g_510c54->game_time)
	{
		function_209850(thread_index);
		result = 0;
		if (thread->sleep_until != NONE)
		{
			result = (thread->sleep_until != -3) + 1;
		}
	}
	return result;
}
