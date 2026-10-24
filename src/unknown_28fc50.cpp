// @flags /O2 /Gr /arch:SSE
#include "unknown_2551c0.h"
#include "globals.h"

struct s_28fc50
{
	byte field_0[4];
	long field_4;
	long field_8;
};

struct s_28fc51
{
	byte field_0[0x14];
	short field_14;
	byte field_16[6];
	short field_1c;
	byte field_1e[2];
	long field_20;
	byte field_24[0x10];
};

long function_1c9580(long arg_0, bool arg_1);
void __stdcall function_1e2570(long arg_0, word arg_1, long arg_2, real arg_3, long arg_4);

// @retail 0x28fc50
void function_28fc50(long arg_0, word arg_1, long arg_2)
{
	word const *local_4 = &arg_1;
	s_handler_object_view *local_0 = handler_object_get(arg_0);
	s_28fc50 *local_1 = local_0->flags134 ? NULL : (s_28fc50 *)((byte *)local_0 + local_0->ai_offset);
	if (local_1 && local_1->field_4 != NONE && local_1->field_8 != NONE)
	{
		s_28fc51 *local_2 = (s_28fc51 *)perception_get(local_1->field_8);
		long local_3 = function_1c9580(arg_2, *local_4 != 9);
		local_2->field_1c++;
		local_2->field_20 = g_510c54->game_time;
		function_1e2570(local_1->field_4, *local_4, local_3, 1.0f / local_2->field_14, (long)g_4687a4);
	}
}
