// @flags /O2 /Ob1 /Gr
/* UNKNOWN_217370.CPP: taking the saved-game file lock (in the subsystem's
   state, include/saved_game_storage.h) */

#include "unknown_11c920.h"
#include "saved_game_storage.h"
#include <xtl.h>

void function_125d60(void);

/* waits until the lock is free, takes it for this thread and the given file,
   and hands out the lock (its free flag, as src/unknown_2173d0.cpp declares it) */
// @retail 0x217370
void __stdcall function_217370(long file_index, bool **lock)
{
	dword thread = GetCurrentThreadId();
	s_saved_game_file_lock volatile *waiting = &g_saved_game_file_lock;
	if (!waiting->free)
	{
		while (!waiting->free)
		{
			SwitchToThread();
			function_125d60();
		}
	}
	g_saved_game_file_lock.thread = thread;
	g_saved_game_file_lock.free = false;
	g_saved_game_file_lock.file_index = file_index;
	*lock = &g_saved_game_file_lock.free;
}
