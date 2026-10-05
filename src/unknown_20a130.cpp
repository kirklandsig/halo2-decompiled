#include "unknown_11c920.h"
#include "hs.h"

// @flags /O2 /Gr

extern long *g_4de2d0;
long function_1ded60(void);
void function_1dedb0(long list_index, long object_index);

// @retail 0x20a130
long __stdcall function_20a130(script_value value)
{
	long result = NONE;
	if (value.s >= 0 && value.s < 0x280)
	{
		long object_index = g_4de2d0[value.s];
		if (object_index != NONE)
		{
			result = function_1ded60();
			function_1dedb0(result, object_index);
		}
	}
	return result;
}

// @retail 0x20a170
long __stdcall function_20a170(long object_index)
{
	long result = NONE;
	if (object_index != NONE)
	{
		result = function_1ded60();
		function_1dedb0(result, object_index);
	}
	return result;
}

// The initialization at 0x20a1a0 installs these casts in the runtime table.
long (__stdcall *g_4f7640)(long) = function_20a170;
long (__stdcall *g_4f7658)(script_value) = function_20a130;
