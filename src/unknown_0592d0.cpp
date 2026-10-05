// @flags /O2 /Gr
/* UNKNOWN_0592D0.CPP: a global getter */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_058ee0.h"

// @retail 0x592d0
dword function_0592d0(void)
{
	dword result = 0;
	if (g_527330.initialized)
	{
		result = g_527330.state;
	}
	return result;
}
