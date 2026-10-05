// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_213890.CPP: waits for the cache file reads queued on the
   asynchronous task queue to finish. Decompiled by lane L for the cache file
   code (unknown_122870.cpp). */

#include "unknown_11c920.h"
#include "async.h"
#include <xtl.h>

#define NUMBEROF(array) (sizeof(array) / sizeof((array)[0]))

// @retail 0x213890
void function_213890(void)
{
	long categories[] = { 2, 3, 4, 5, 6 };

	for (long i = 0; i < NUMBEROF(categories); i++)
	{
		while (function_120b50(categories[i]))
			SwitchToThread();
	}
}

extern char const *g_46880c;
char *function_11c9c0(char *buffer, long maximum_count, const char *format, ...);

// @retail 0x2138f0
void __stdcall function_2138f0(long first, long last)
{
	(void)&last;
	char path[0x100];
	for (long index = first; index < last; index++)
	{
		function_11c9c0(path, sizeof(path), "%scache%03d%s", "z:\\", index, g_46880c);
		DeleteFileA(path);
		function_11c9c0(path, sizeof(path), "%scache%03d%s", "n:\\", index, g_46880c);
		DeleteFileA(path);
	}
	SetLastError(0);
}
