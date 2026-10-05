#include "unknown_24fa12.h"
#include "unknown_11c920.h"

// @flags /O1 /Gr

// @retail 0x24fa12
c_session_user_state::c_session_user_state()
{
	index = NONE;
	active = false;
}

struct s_session_screen_state
{
	byte unknown00[0x814];
	c_session_user_state users[4];
};

// @retail 0x24fa23
long function_24fa23(s_session_screen_state const *screen)
{
	return screen->users[0].active || screen->users[1].active ||
		screen->users[2].active || screen->users[3].active;
}

c_class_1473c9 *__stdcall function_24fa4c(s_screen_parameters *parameters);

// @retail 0x24fa1d
screen_load_proc c_screen_24fd74::get_load_proc()
{
	return function_24fa4c;
}

// @retail 0x24fa4c
c_class_1473c9 *__stdcall function_24fa4c(s_screen_parameters *parameters)
{
	c_screen_24fd74 *screen = new c_screen_24fd74(parameters->a, parameters->b, parameters->user_flags);
	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x24fa88
c_screen_24fd74::c_screen_24fd74(long a, long b, word user_flags) :
	c_class_1473c9(0xe, a, b, user_flags),
	button0(0, user_flags),
	button1(1, user_flags),
	value810(true),
	value811(true),
	value812(0),
	value813(false),
	countdown_stage(NONE),
	countdown_time(0),
	mode(0),
	value142e(false),
	choice0(this, (session_choice_method)&c_screen_24fd74::handle_lobby_choice),
	choice1(this, (session_choice_method)&c_screen_24fd74::handle_lobby_choice),
	value1460(false),
	value1464(0),
	value1468(false),
	value146c(NONE),
	state_shown_time(0)
{
}

// @retail 0x24fba2 destructor c_screen_24fd74

// @retail 0x24fb86 deleting c_screen_24fd74

typedef char session_screen_size_check[sizeof(c_screen_24fd74) == 0x1474 ? 1 : -1];

extern bool g_51ec98;
bool function_199971(void);
bool function_592f0(void);
long function_148d30(void);
bool function_199e6d(long index);
void function_19a0af(long value);
long function_18fa4d(long mode);
bool function_19a78e(long controller, long countdown, long minimum);
void function_236299(long sound);
bool function_2510e1(void);

// @retail 0x24fbea
void function_24fbea(c_screen_24fd74 *screen)
{
	if (function_199971())
	{
		if (function_592f0() && g_51ec98 && function_199e6d(function_148d30()))
		{
			function_19a0af(function_148d30());
			if (!function_19a78e(function_18fa4d(0), 3, 3))
				function_236299(2);
		}
	}
	else if (!function_2510e1())
		screen->value142e = true;
}

long function_19989d(void);

// @retail 0x24ff0a
void function_24ff0a(c_screen_24fd74 *screen)
{
	short mode = 0;
	switch (function_19989d())
	{
	case 0: mode = 1; break;
	case 1: mode = 0; break;
	case 2: mode = 1; break;
	case 3: mode = 0; break;
	case 4: mode = 1; break;
	case 5: mode = 0; break;
	case 6: mode = 2; break;
	default: mode = screen->mode; break;
	}
	if (mode != screen->mode)
	{
		long pane = mode;
		screen->function_230427((short *)&pane);
		screen->mode = mode;
	}
}

struct s_session_player_view;
bool function_19a902(void);
long function_251364(s_session_player_view *player);

// @retail 0x24ff61
void function_24ff61(c_class_1a2c81 *screen, bool selected, s_session_player_view *player)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)screen->find_child(6, 1, false);
	if (text)
	{
		long string_handle;
		if (function_19a902())
			string_handle = selected ? 0x2700021b : 0x2000021a;
		else
		{
			byte active = (byte)function_251364(player);
			if (selected)
				string_handle = active ? 0x25000217 : 0x26000216;
			else
				string_handle = active ? 0x1d000219 : 0x1e000218;
		}
		text->function_253b1a(string_handle);
	}
}
