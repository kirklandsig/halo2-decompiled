// @flags /O2 /Gr
/* UNKNOWN_208CC0.CPP: value-to-text formatters */

#include "unknown_11c920.h"
#include <string.h>
#include "unknown_0a58d0.h"

struct s_name_table
{
	const char **names;
	long count;
};

s_name_table g_4456fc[1];

// @retail 0x208cc0
void __stdcall function_208cc0(long unused, bool value, char *buffer, long maximum_count)
{
	if (value)
	{
		strncpy(buffer, "true", maximum_count);
	}
	else
	{
		strncpy(buffer, "false", maximum_count);
	}
	buffer[maximum_count - 1] = 0;
}

// @retail 0x208d10
void __stdcall function_208d10(long unused, real value, char *buffer, long maximum_count)
{
	function_11c9c0(buffer, maximum_count, "%f", (double)value);
}

// @retail 0x208d40
void __stdcall function_208d40(long unused, short value, char *buffer, long maximum_count)
{
	function_11c9c0(buffer, maximum_count, "%d", (long)value);
}

// @retail 0x208d70
void __stdcall function_208d70(long unused, long value, char *buffer, long maximum_count)
{
	function_11c9c0(buffer, maximum_count, "%ld", value);
}

// @retail 0x208da0
void __stdcall function_208da0(long unused, const char *value, char *buffer, long maximum_count)
{
	strncpy(buffer, value, maximum_count);
	buffer[maximum_count - 1] = 0;
}

// @retail 0x208dd0
void __stdcall function_208dd0(short table_index, short name_index, char *buffer, long maximum_count)
{
	strncpy(buffer, g_4456fc[table_index].names[name_index], maximum_count);
	buffer[maximum_count - 1] = 0;
}

typedef void (__stdcall *t_value_to_text)(long, long, char *, long);

t_value_to_text g_46fd84[5] =
{
	(t_value_to_text)function_208cc0,
	(t_value_to_text)function_208d10,
	(t_value_to_text)function_208d40,
	(t_value_to_text)function_208d70,
	(t_value_to_text)function_208da0,
};

t_value_to_text g_46fe20[6] =
{
	(t_value_to_text)function_208dd0,
	(t_value_to_text)function_208dd0,
	(t_value_to_text)function_208dd0,
	(t_value_to_text)function_208dd0,
	(t_value_to_text)function_208dd0,
	(t_value_to_text)function_208dd0,
};
