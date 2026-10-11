// @flags /O2 /Gr /arch:SSE
#include "unknown_2551c0.h"
#include "globals.h"

struct s_28f700
{
	byte field_0[0x1c];
	long field_1c;
	byte field_20[0x888 - 0x20];
};

struct s_28f701
{
	byte field_0[0x18];
	long field_18;
	short field_1c;
	byte field_1e[2];
	long field_20;
	byte field_24[0x10];
};

struct s_28f702
{
	byte field_0[0xc];
	long field_c;
};

void __stdcall function_28e770(long arg_0, long arg_1);
void function_28f3b0(long arg_0, long arg_1);
void function_28f600(long arg_0);

__forceinline s_28f702 *function_28f701(s_handler_object_view *arg_0)
{
	s_28f702 *local_0;
	if (!arg_0->flags134)
		local_0 = (s_28f702 *)((byte *)arg_0 + arg_0->ai_offset);
	else
		local_0 = NULL;
	return local_0;
}

PRIVATE __forceinline s_handler_object_view *function_28f703(s_ai_object_iterator *arg_0)
{
	s_handler_object_view *local_0 = NULL;
	if (arg_0->next_index != NONE)
	{
		local_0 = handler_object_get(arg_0->next_index);
		arg_0->index = arg_0->next_index;
		s_28f702 *local_1 = function_28f701(local_0);
		arg_0->next_index = local_1 ? local_1->field_c : NONE;
	}
	return local_0;
}

// @retail 0x28f700
void __stdcall function_28f700(long arg_0)
{
	long const volatile *local_5 = &arg_0;
	s_28f700 *local_0 = &((s_28f700 *)g_4f55f0->data)[arg_0 & 0xffff];
	s_28f701 *local_1 = (s_28f701 *)perception_get(local_0->field_1c);
	if ((real)(g_510c54->game_time - local_1->field_20) * g_510c54->rate > 2.0f)
		local_1->field_1c = 0;
	s_ai_object_iterator local_6;
	local_6.next_index = ((s_28f701 *)perception_get(local_0->field_1c))->field_18;
	while (function_28f703(&local_6) != NULL)
	{
		long local_3 = local_6.index;
		function_28e770(arg_0, local_3);
	}
	local_6.next_index = ((s_28f701 *)perception_get(local_0->field_1c))->field_18;
	while (function_28f703(&local_6) != NULL)
	{
		long local_3 = local_6.index;
		function_28f3b0(*local_5, local_3);
	}
	local_6.next_index = ((s_28f701 *)perception_get(local_0->field_1c))->field_18;
	while (function_28f703(&local_6) != NULL)
	{
		long local_3 = local_6.index;
		function_28f600(local_3);
	}
}
