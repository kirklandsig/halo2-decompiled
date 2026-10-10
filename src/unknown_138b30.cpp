// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include <string.h>

struct s_138b30
{
	byte field_0[0x58];
	dword *field_58;
	byte field_5c[0x9c - 0x5c];
	long field_9c;
};

struct s_138b31
{
	byte field_0[0x10];
	short field_10;
};

void function_150820(dword *bits, bool local_only);
void function_13e3e0(dword const *a, dword const *b, dword *destination, long bit_count);
long camera_scripting_cluster_get(void);

#pragma inline_depth(0)
// @retail 0x138b30
void function_138b30(dword *arg_1, bool arg_2, long arg_3)
{
	s_138b30 *local_1 = (s_138b30 *)g_4e0348;
	memset(arg_1, 0, 0x40);
	s_game_options_view *local_2 = g_4e6948;
	if (local_2->state != 3)
	{
		function_150820(arg_1, arg_2);
	}
	else
	{
		long local_3 = NONE;
		for (long local_4 = 0; local_4 < 4; local_4++)
		{
			if (g_4e8c20->entries[local_4] != NONE)
			{
				local_3 = local_4;
				break;
			}
		}
        short local_7;
        if (local_3 != NONE && g_4686c4 != NONE)
        {
            local_7 = (short)(g_4e9bd4[local_3].state.unknown0c[4]
                | (g_4e9bd4[local_3].state.unknown0c[5] << 8));
        }
        else
        {
            s_138b31 const *local_6 = 0;
            local_7 = local_6->field_10;
        }
		if (local_7 != NONE)
		{
			function_13e3e0(arg_1,
				local_1->field_58 + ((local_1->field_9c + 31) >> 5) * local_7, arg_1, local_1->field_9c);
		}
	}
	if (local_2->flag11f8 && *g_4e8c34)
	{
		short local_8 = (short)camera_scripting_cluster_get();
		if (local_8 != NONE)
		{
			function_13e3e0(arg_1,
				local_1->field_58 + ((local_1->field_9c + 31) >> 5) * local_8, arg_1, local_1->field_9c);
		}
	}
}
#pragma optimize("", on)
#pragma inline_depth(255)
