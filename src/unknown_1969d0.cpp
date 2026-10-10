#include "unknown_11c920.h"
#include "globals.h"
#include "input_record.h"

// @flags /O2 /Gr

s_input_entry g_511a74[16];

// @retail 0x001969d0
long function_1969d0(long a, long b, long c)
{
	long lookup_hit_value = -1;
	if (g_510cb0 && g_510cb1)
	{
		lookup_hit_value = g_511c90[a * 0x1b5 + b + c * 8].value;
	}
	return lookup_hit_value;
}

// @retail 0x00196a10
long function_196a10(long a, long b)
{
	long result = -1;
	if (g_510cb0 && g_510cb1)
	{
		result = g_511c4e[a * 0x1b5 + b].value;
	}
	return result;
}

// @retail 0x00196a50
long function_196a50(long a, long b, long c)
{
	long result = -1;
	if (g_510cb0 && g_510cb1)
	{
		result = g_515294[(a * 16 + b) * 2 + c].value;
	}
	return result;
}

// @retail 0x00196a90
long function_196a90()
{
	long result = 0;
	if (g_510cb0 && g_510cb1)
	{
		result = g_510e2c;
	}
	return result;
}

// @retail 0x00196ab0
void function_196ab0(long index, short axis, short button)
{
	if (g_510ca0 && !g_510cb1 && index != NONE && input_device(index)->active)
	{
		input_device(index)->axis = axis;
		input_device(index)->button = button;
	}
}

// @retail 0x00196b00
void function_196b00(long index, long value, char button)
{
	if (g_510ca0 && !g_510cb1 && index >= 0 && index < 8 && g_511a74[index].active)
	{
		if (value < -0x8000)
		{
			value = -0x8000;
		}
		else if (value > 0x7fff)
		{
			value = 0x7fff;
		}
		g_511a74[index].value = (word)value;
		g_511a74[index].unknown01 = button;
	}
}

// @retail 0x00196b70
long function_196b70(long index)
{
	long result = 0;
	if (g_510cb0 && g_510cb1 && index != NONE && input_device(index)->active)
	{
		result = input_device(index)->axis;
	}
	return result;
}

// @retail 0x00196bb0
long function_196bb0(long index)
{
	long result = 0;
	if (g_510cb0 && g_510cb1 && index >= 0 && index < 8 && g_511a74[index].active)
	{
		result = (short)g_511a74[index].value;
	}
	return result;
}

// @retail 0x00196bf0
long function_196bf0(long index)
{
	long result = -1;
	if (g_510cb0 && g_510cb1 && index != NONE && input_device(index)->active)
	{
		if (g_510e30 & 1)
		{
			long slot = input_device(index)->slot;
			if (slot == NONE)
			{
				return NONE;
			}
			if (slot >= 0 && slot < 16 && g_511a74[slot].active)
			{
				result = (char)g_511a74[slot].unknown01;
			}
		}
		else
		{
			result = input_device(index)->button;
		}
	}
	return result;
}

// @retail 0x00196c60
long function_196c60(long index)
{
	long result = 0;
	if (g_510cb0 && g_510cb1 && index >= 0 && index < 8 && g_511a74[index].active)
	{
		result = (char)g_511a74[index].unknown01;
	}
	return result;
}

// @retail 0x00196ca0
void *function_196ca0(long index)
{
	void *result = (void *)L"";  /* the empty string at 0x450aac */
	if (g_510cb0 && g_510cb1 && index != NONE && input_device(index)->active)
	{
		result = input_device(index)->data;
	}
	return result;
}

// @retail 0x00196ce0
void *function_196ce0(long index)
{
	void *result = 0;
	if (g_510cb0 && g_510cb1 && index != NONE && input_device(index)->active)
	{
		result = input_device(index)->unknown02;
	}
	return result;
}
