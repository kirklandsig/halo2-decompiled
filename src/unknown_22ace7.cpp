// @flags /O1 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_07f720.h"
#include <stddef.h>

struct s_4e6950
{
	byte unknown00[0x18];
	real value18;
	real value1c;
	real value20;
	real value24;
	byte unknown28[8];
	real value30;
	real value34;
	real value38;
	real value3c;
	byte unknown40[0x30];
	real user_values[4];
};

extern s_4e6950 g_4e6950;
extern long g_4b9ed8;
struct s_510c4c;
extern s_510c4c *g_510c4c;

struct s_interface_function_state
{
	byte unknown00[0x1b4];
	long player_index;
	byte unknown1b8[8];
	s_player_appearance appearance;
	short quantity;
};

typedef char check_interface_value_view_size[sizeof(s_4e6950) == 0x80 ? 1 : -1];
typedef char check_interface_quantity_offset[offsetof(s_interface_function_state, quantity) == 0x1d0 ? 1 : -1];

bool function_22acb4(long local_player_index);
bool player_get_function_value(long player_index, long name, real *value);
bool player_appearance_get_function_value(long name, s_player_appearance const *appearance, real *value);

// @retail 0x22ace7
real __stdcall function_22ace7(long context, long name)
{
	real result = 1.0f;
	switch (name)
	{
	case 0x090005ce:
		result = function_22acb4(g_4b9ed8) ? 1.0f : 0.0f;
		break;
	case 0x0a0005cf:
		result = g_4e6950.value30;
		break;
	case 0x0a0005d0:
		result = g_4e6950.value34;
		break;
	case 0x0a0005d1:
		result = g_4e6950.value38;
		break;
	case 0x0a0005d2:
		result = g_4e6950.value3c;
		break;
	case 0x0c0005cb:
		result = g_4e6950.value1c;
		break;
	case 0x0c0005cc:
		result = g_4e6950.value20;
		break;
	case 0x0d0005ca:
		result = g_4e6950.value18;
		break;
	case 0x0e0005cd:
		result = g_4e6950.value24;
		break;
	case 0x11000600:
		result = ((s_interface_function_state *)g_510c4c)->quantity * (1.0f / 64.0f);
		break;
	case 0x0d0005ff:
	case 0x170005fa:
	case 0x170005fb:
	case 0x180005fc:
	case 0x1d0005fd:
	case 0x1d0005fe:
		{
			s_interface_function_state *state = (s_interface_function_state *)g_510c4c;
			if (state->player_index != NONE)
				player_get_function_value(state->player_index, name, &result);
			else
				player_appearance_get_function_value(name, &state->appearance, &result);
		}
		break;
	}
	return result;
}

struct s_interface_function_context
{
	long index;
	real (__stdcall *evaluate)(long context, long name);
	byte unknown08[0x70];
};

s_interface_function_context g_5021d0;
void function_2c3b0(s_interface_function_context *context);

// @retail 0x22a648
void function_22a648(void)
{
	function_2c3b0(&g_5021d0);
	g_5021d0.index = NONE;
	g_5021d0.evaluate = function_22ace7;
}
