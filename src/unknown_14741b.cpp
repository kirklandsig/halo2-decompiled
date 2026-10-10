// @flags /O1 /Oi /Gr
/* UNKNOWN_14741B.CPP: the legal notice, main menu and multiplayer pause
   screens, which the window manager loads itself */

#include "unknown_11c920.h"
#include "screen_widgets.h"
#include "user_interface_lists.h"
#include "unknown_24b5bc.h"
#include "unknown_19b510.h"
#include "unknown_19b516.h"

/* the legal notice screen (vtable 0x4537b8; its deleting destructor is
   folded with the appear offline screen's) */
class c_legalese_screen : public c_screen_with_menu
{
public:
	c_legalese_screen(long a, long b, word user_flags);

	virtual bool v10(s_widget_event *event);
	virtual screen_load_proc get_load_proc();

	c_legalese_acceptance_list list;
};

/* the main menu screen (vtable 0x453828; its deleting destructor is folded
   with the multiplayer controller settings screen's) */
class c_main_menu_screen : public c_screen_with_menu
{
public:
	c_main_menu_screen(long a, long b, word user_flags);

	/* finishes a sign in the screen was waiting for, and shows the feedback
	   dialog once */
	virtual void v3();
	virtual bool v10(s_widget_event *event);
	virtual void v19();
	virtual screen_load_proc get_load_proc();

	c_main_menu_list list;
	long value934;
};

/* the multiplayer pause screen (vtable 0x453898) */
class c_mp_pause_game_screen : public c_screen_with_menu
{
public:
	c_mp_pause_game_screen(long a, long b, word user_flags);

	virtual void v3();
	virtual screen_load_proc get_load_proc();

	c_mp_pause_game_list list;
};

// @retail 0x14741b
c_class_1473c9 *__stdcall function_14741b(s_screen_parameters *parameters)
{
	c_legalese_screen *screen = new c_legalese_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x147459
c_legalese_screen::c_legalese_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0xcb, a, b, user_flags, &list),
	list(user_flags)
{
}

// @retail 0x14748e
screen_load_proc c_legalese_screen::get_load_proc()
{
	return function_14741b;
}

// @retail 0x14752c
c_class_1473c9 *__stdcall function_14752c(s_screen_parameters *parameters)
{
	c_main_menu_screen *screen = new c_main_menu_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x14756a
c_main_menu_screen::c_main_menu_screen(long a, long b, word user_flags) :
	c_screen_with_menu(6, a, b, user_flags, &list),
	list(user_flags),
	value934(NONE)
{
}

// @retail 0x1475a3
screen_load_proc c_main_menu_screen::get_load_proc()
{
	return function_14752c;
}

// @retail 0x1475c7
c_class_1473c9 *__stdcall function_1475c7(s_screen_parameters *parameters)
{
	c_mp_pause_game_screen *screen = new c_mp_pause_game_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x147605
c_mp_pause_game_screen::c_mp_pause_game_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0xc3, a, b, user_flags, &list),
	list(user_flags)
{
}

// @retail 0x14763a
screen_load_proc c_mp_pause_game_screen::get_load_proc()
{
	return function_1475c7;
}

// @retail 0x232091 deleting c_mp_pause_game_screen
// @retail 0x2320af destructor c_mp_pause_game_screen
// @retail 0x231f36 destructor c_mp_pause_game_list
// @retail 0x147640 destructor c_mp_pause_game_list_item

/* the lists (in 0x2304d2..0x232708 in retail) */

// @retail 0x2304d2
c_legalese_acceptance_list::c_legalese_acceptance_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_legalese_acceptance_list::handle_item)
{
	data = user_interface_data_new("legalese acceptance list", 2, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
	accepted = false;
}

// @retail 0x230738
c_main_menu_list::c_main_menu_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_main_menu_list::handle_item)
{
	data = user_interface_data_new("main menu list", 5, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x231f18 deleting c_mp_pause_game_list

// @retail 0x230591
void c_legalese_acceptance_list::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);

	if (text)
	{
		long string_handle;

		switch (((short)widget_item(widget)->value70))
		{
		case 0:
			string_handle = 0xe000223;
			break;
		case 1:
			string_handle = 0x7000224;
			break;
		default:
			string_handle = 0;
			break;
		}
		text->function_253b1a(string_handle);
	}
}

void __stdcall function_1483c3(long reason);
void function_2238f4(long page, dword context, dword parameter1, dword parameter2);

// @retail 0x2305d0
void c_legalese_acceptance_list::handle_item(s_controller_reference **controller, long *item)
{
	switch ((short)*item)
	{
	case 0:
		function_2238f4(0, 0, 0, 0);
		break;
	case 1:
		function_1483c3(0);
		break;
	}
}

// @retail 0x2305f5
bool c_legalese_screen::v10(s_widget_event *event)
{
	bool event_consumed_flag;
	if (event->type == 6)
	{
		function_1483c3(0);
		event_consumed_flag = true;
	}
	else
	{
		event_consumed_flag = c_class_1473c9::v10(event);
	}
	return event_consumed_flag;
}

// @retail 0x2307c8
void c_main_menu_list::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);

	if (text)
	{
		long string_handle;

		switch (((short)widget_item(widget)->value70))
		{
		case 0:
			string_handle = 0x800010b;
			break;
		case 1:
			string_handle = 0x9000283;
			break;
		case 2:
			string_handle = 0xb000284;
			break;
		case 3:
			string_handle = 0xa000285;
			break;
		case 4:
			string_handle = 0x8000286;
			break;
		default:
			string_handle = NONE;
			break;
		}
		text->function_253b1a(string_handle);
	}
}

struct s_player_profile
{
	dword data[0x78];
};

bool function_19028d(void);
bool function_8d7c0(void);
bool function_6c7e0();
word function_1901fc(void);
void function_1906b4(void);
void __stdcall function_18f1c0(long a);
void player_slot_get_profile(long index, s_player_profile *profile, long *profile_index);
c_class_1473c9 *__stdcall function_230616(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_230691(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_24b4a9(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_25240c(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_252433(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_25245a(s_screen_parameters *parameters);
bool __stdcall function_236877(long controller_index);
bool __stdcall function_2368c1(long controller_index);
bool __stdcall function_236917(long controller_index);

extern bool g_54d5a0;

/* the main menu's last choice */
long g_510a14;
bool g_510819;
bool g_54e7cd;

/* the campaign */
// @retail 0x230888
void function_230888(s_controller_reference **controller)
{
	if (g_54d5a0)
	{
		dialog_ok_show(1, 0x32, 4, 1 << (*controller)->controller_index, 0, 0);
	}
	else
	{
		s_screen_parameters parameters;

		parameters.field_c = 0;
		function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)function_230616);
		parameters.load(&parameters);
		g_54e7cd = true;
	}
}

/* Xbox Live: signs the controller's player in first when it has a profile */
// @retail 0x2308e0
void __stdcall function_2308e0(c_main_menu_list *list, s_controller_reference **controller)
{
	s_player_profile profile;
	s_screen_parameters parameters;

	parameters.field_c = 0;
	if (function_19028d())
	{
		function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 5, 4, (long)function_25245a);
		parameters.load(&parameters);
	}
	else if (function_8d7c0())
	{
		c_main_menu_screen *screen = (c_main_menu_screen *)list->get_screen();
		s_controller_reference *reference = *controller;
		s_player_slot_profile *slot_profile = player_slot_profile_get(reference->controller_index);
		long profile_index;

		player_slot_get_profile(reference->controller_index, &profile, &profile_index);
		if (profile_index != NONE)
		{
			slot_profile->initialize(reference->controller_index);
			slot_profile->set_profile_index(profile_index);
			function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)function_24b4a9);
			parameters.load(&parameters);
			screen->value934 = (*controller)->controller_index;
		}
		else
		{
			function_1906b4();
			function_18f1c0(0);
		}
	}
	else
	{
		dialog_choice_show(1, 0x23, 4, 1 << (*controller)->controller_index, function_236917, 0, 0);
	}
}

/* split screen */
// @retail 0x2309ec
void function_2309ec(s_controller_reference **controller)
{
	if (g_54d5a0)
	{
		dialog_ok_show(1, 0x32, 4, 1 << (*controller)->controller_index, 0, 0);
	}
	else
	{
		s_screen_parameters parameters;

		parameters.field_c = 0;
		if (function_6c7e0())
		{
			dialog_choice_show(3, 0x78, 4, 1 << (*controller)->controller_index, function_236877, 0, 0);
		}
		else
		{
			function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 5, 4, (long)function_25240c);
			parameters.load(&parameters);
		}
	}
}

/* system link */
// @retail 0x230a64
void function_230a64(s_controller_reference **controller)
{
	bool connected = function_8d7c0();

	if (g_54d5a0)
	{
		dialog_ok_show(1, 0x32, 4, 1 << (*controller)->controller_index, 0, 0);
	}
	else
	{
		s_screen_parameters parameters;

		parameters.field_c = 0;
		if (function_6c7e0())
		{
			dialog_choice_show(3, 0x78, 4, 1 << (*controller)->controller_index, function_2368c1, 0, 0);
		}
		else if (connected)
		{
			function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 5, 4, (long)function_252433);
			parameters.load(&parameters);
		}
		else
		{
			dialog_ok_show(1, 0x3b, 4, function_1901fc(), 0, 0);
		}
	}
}

/* the settings */
// @retail 0x230aff
void function_230aff(s_controller_reference **controller)
{
	s_screen_parameters parameters;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 5, 4, (long)function_230691);
	parameters.load(&parameters);
}

// @retail 0x230827
void c_main_menu_list::handle_item(s_controller_reference **controller, long *item)
{
	long index = *item & 0xffff;

	g_510a14 = index;
	g_510819 = true;
	switch (index)
	{
	case 0:
		function_230888(controller);
		break;
	case 1:
		function_2308e0(this, controller);
		break;
	case 2:
		function_2309ec(controller);
		break;
	case 3:
		function_230a64(controller);
		break;
	case 4:
		function_230aff(controller);
		break;
	}
}

// @retail 0x23284e
void c_mp_pause_game_list::v20(c_class_1a2c81 *item, long unused)
{
	s_list_item_text table[6];

	table[0].item = 0;
	table[0].string_handle = 0xa0001b7;
	table[1].item = 1;
	table[1].string_handle = 0x8000286;
	table[2].item = 2;
	table[2].string_handle = 0xc0001b9;
	table[3].item = 3;
	table[3].string_handle = 0xc0001ba;
	table[4].item = 4;
	table[4].string_handle = 0x1c0001bb;
	table[5].item = 5;
	table[5].string_handle = 0x80001bd;
	function_24c75c(this, item, table, 0, 6);
}

long function_199ebc(void);
long function_199f34(void);
bool function_1999f9(void);
bool function_1900a5(long index);
short function_1900be(void);
short function_1900ff(long controller);
bool function_199971(void);
bool function_19a0c5(void);
long function_19989d(void);
void function_199e3c(long controller);
long function_199f6d(void);
bool function_1906da(long index);
bool function_199967(void);
bool function_19a1bd(void);
bool function_592f0(void);
bool function_1999b3(void);
byte *network_session_interface_get_data_4db0(void);
c_class_1473c9 *__stdcall function_231db5(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_23252e(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2323c3(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_23246a(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2325fb(s_screen_parameters *parameters);

bool g_4ee4e0;

/* quits the game: the player leaves, or the whole game ends */
// @retail 0x232c9e
bool __stdcall function_232c9e(long controller_index)
{
	bool alone = function_199f6d() == 1;

	if (!function_1900a5(controller_index) && (alone || function_1900be() == 1))
	{
		function_199e3c(controller_index);
		g_4ee4e0 = true;
	}
	else
	{
		s_player_slot_profile *profile = player_slot_profile_get(controller_index);

		profile->callback = 0;
		profile->sign_out();
	}
	return true;
}

/* ends the game */
// @retail 0x232cfc
bool __stdcall function_232cfc(long controller_index)
{
	function_19a1bd();
	return true;
}

/* asks whether to quit, in the words that fit the game */
// @retail 0x2329a7
void function_2329a7(c_class_1473c9 *screen, s_controller_reference **controller)
{
	bool host = function_199ebc() == 1;
	bool live = function_592f0();
	bool alone = function_199f34() == 1;
	long dialog_id;

	if (function_1999f9())
	{
		long controller_index = (*controller)->controller_index;

		if (function_1900a5(controller_index))
		{
			dialog_id = 0xba;
		}
		else if (function_1900be() != 1)
		{
			if (function_1900ff(controller_index) > 0)
			{
				dialog_ok_show(screen->v20(), 0x88, screen->v21(), 1 << controller_index, 0, 0);
				return;
			}
			dialog_id = 7;
		}
		else if (function_199971())
		{
			if (function_19a0c5())
			{
				if (host)
				{
					dialog_id = 0xbb;
				}
				else
				{
					dialog_id = live ? 0xb1 : 0xb2;
				}
			}
			else if (!host && !alone)
			{
				dialog_id = live ? 0xbd : 0xbe;
			}
			else
			{
				dialog_id = 0xbc;
			}
		}
		else if (!host && !alone)
		{
			dialog_id = live ? 0xb3 : 0xb5;
		}
		else
		{
			dialog_id = 0xb4;
		}
	}
	else if (host)
	{
		dialog_id = 0xb0;
	}
	else
	{
		dialog_id = live ? 0xa1 : 0x37;
	}
	dialog_choice_show_default(screen->v20(), screen->v21(), 1 << (*controller)->controller_index, function_232c9e, dialog_id);
	screen->start_animation(3);
}

/* whether the player may change its settings in this game */
// @retail 0x2b505c
bool __stdcall function_2b505c(long controller_index)
{
	bool result = true;

	if (!function_1900a5(controller_index))
	{
		if (function_1906da(controller_index))
		{
			result = false;
		}
		if (function_6c7e0())
		{
			result = false;
		}
	}
	return result;
}

/* the player settings */
// @retail 0x232aff
void function_232aff(c_class_1473c9 *screen, s_controller_reference **controller)
{
	s_screen_parameters parameters;
	long window;
	long channel;

	parameters.field_c = 0;
	if (function_2b505c((*controller)->controller_index))
	{
		window = screen->v21();
		channel = screen->v20();
		function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, channel, window, (long)function_231db5);
	}
	else
	{
		window = screen->v21();
		channel = screen->v20();
		function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, channel, window, (long)function_23252e);
	}
	parameters.load(&parameters);
}

/* the handicap */
// @retail 0x232b63
void function_232b63(c_class_1473c9 *screen, s_controller_reference **controller)
{
	s_screen_parameters parameters;
	long window;
	long channel;

	screen->start_animation(3);
	parameters.field_c = 0;
	window = screen->v21();
	channel = screen->v20();
	function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, channel, window, (long)function_2323c3);
	parameters.load(&parameters);
}

/* whether the teams may be changed */
// @retail 0x232d06
bool function_232d06()
{
	struct s_session_view
	{
		byte unknown00[0x48];
		dword flag0 : 1;
		dword flags1 : 4;
		dword flag5 : 1;
		dword flag6 : 1;
	};
	bool result = false;
	s_session_view *session = (s_session_view *)network_session_interface_get_data_4db0();

	if (session && function_199967())
	{
		if ((TEST_FIELD_BIT(session->flag0) || TEST_FIELD_BIT(session->flag5)) && TEST_FIELD_BIT(session->flag6))
		{
			result = true;
		}
	}
	return result;
}

/* change teams */
// @retail 0x232bb4
void function_232bb4(c_class_1473c9 *screen, s_controller_reference **controller)
{
	if (function_232d06())
	{
		s_screen_parameters parameters;
		long window;
		long channel;

		parameters.field_c = 0;
		window = screen->v21();
		channel = screen->v20();
		function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, channel, window, (long)function_23246a);
		parameters.load(&parameters);
	}
}

/* the controller settings */
// @retail 0x232c05
void function_232c05(c_class_1473c9 *screen, s_controller_reference **controller)
{
	s_screen_parameters parameters;
	long window;
	long channel;

	parameters.field_c = 0;
	window = screen->v21();
	channel = screen->v20();
	function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, channel, window, (long)function_2325fb);
	parameters.load(&parameters);
}

/* asks whether to end the game */
// @retail 0x232c4d
void function_232c4d(c_class_1473c9 *screen, s_controller_reference **controller)
{
	long state = function_19989d();

	if (function_592f0() && state != 6)
	{
		dialog_choice_show_default(screen->v20(), screen->v21(), 1 << (*controller)->controller_index, function_232cfc, 0xaf);
		screen->start_animation(3);
	}
}

// @retail 0x2326bc
c_mp_pause_game_list_item::c_mp_pause_game_list_item()
{
}

// @retail 0x2326ce
bool c_mp_pause_game_list_item::v10(s_widget_event *event)
{
	if (event->type == 5 && (event->param == 0xc || event->param == 0xd || event->param == 1))
	{
		get_screen()->start_animation(3);
		return true;
	}
	return c_class_14750b::v10(event);
}

// @retail 0x232708
c_mp_pause_game_list::c_mp_pause_game_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_mp_pause_game_list::handle_item)
{
	long state = function_19989d();
	bool can_end = function_592f0() && state != 6;

	data = user_interface_data_new("mp pause game list", 6, 4);
	function_16b790(data);
	((s_list_item_datum *)data->data)[record_pool_allocate(data) & 0xffff].item = 0;
	((s_list_item_datum *)data->data)[record_pool_allocate(data) & 0xffff].item = 1;
	((s_list_item_datum *)data->data)[record_pool_allocate(data) & 0xffff].item = 2;
	if (function_232d06())
	{
		((s_list_item_datum *)data->data)[record_pool_allocate(data) & 0xffff].item = 3;
	}
	if (function_592f0() && !function_1999b3())
	{
		((s_list_item_datum *)data->data)[record_pool_allocate(data) & 0xffff].item = 4;
	}
	if (can_end)
	{
		((s_list_item_datum *)data->data)[record_pool_allocate(data) & 0xffff].item = 5;
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2328b5
void c_mp_pause_game_list::handle_item(s_controller_reference **controller, long *item)
{
	if (*item != NONE)
	{
		c_class_1473c9 *screen = get_screen();
		s_list_item_datum *datum = &((s_list_item_datum *)data->data)[*item & 0xffff];

		s_list_item_datum *const *datum_reference = &datum;
		switch ((*datum_reference)->item)
		{
		case 0:
			function_2329a7(screen, controller);
			break;
		case 1:
			function_232aff(screen, controller);
			break;
		case 2:
			function_232b63(screen, controller);
			break;
		case 3:
			function_232bb4(screen, controller);
			break;
		case 4:
			function_232c05(screen, controller);
			break;
		default:
			function_232c4d(screen, controller);
			break;
		}
	}
}

/* a player slot's flag at +0x46d */
struct s_pause_player_slot_view
{
	byte unknown000[0x46d];
	bool value46d;
	byte unknown46e[0xc70 - 0x46e];
};

// @retail 0x23296e
void c_mp_pause_game_screen::v3()
{
	c_class_1a2c81 *bitmap = find_child(8, 4, false);

	if (bitmap)
	{
		long controller = get_controller_index();

		if (controller != NONE)
		{
			bitmap->value6e = ((s_pause_player_slot_view *)g_54e8e0)[controller].value46d;
		}
	}
	c_class_1a2c81::v3();
}

void __stdcall function_1483c3(long reason);

/* after the sign out, back to the main screen */
// @retail 0x230c7d
void __stdcall function_230c7d(long player, bool signed_in)
{
	function_1483c3(1);
}

/* the press the screen makes for a controller (a button event with its
   source) */
struct s_screen_press
{
	long type;
	long controller_index;
	long param;
	short source;
};

extern bool g_54e7f8;

void function_199a03(long mode);
short player_slot_count_active(void);
void function_148823();
extern long g_54e7c0;
extern long g_54e7c4;

// @retail 0x230b32
void c_main_menu_screen::v19()
{
	if (g_54e5d0.profile_index != NONE)
		profile_edit_end();
	function_199a03(NONE);
	g_54e7c0 = NONE;
	g_54e7c4 = NONE;
	set_user_flags(function_1901fc());
	c_class_1473c9::v19();
	if (player_slot_count_active() != 1)
	{
		function_148823();
		function_18f1c0(0);
	}
	if (g_510a14 >= 0 && g_510a14 < 5)
		list.select_item((short)g_510a14);
	g_54e7cd = false;
}

// @retail 0x230ba6
void c_main_menu_screen::v3()
{
	c_class_1a2c81::v3();
	long controller_index = value934;
	if (controller_index != NONE)
	{
		if (function_8d7c0())
		{
			if (!function_19028d())
			{
				goto done;
			}
			s_screen_press press;
			s_screen_press *reference = &press;

			press.type = 5;
			press.controller_index = controller_index;
			press.param = 0;
			press.source = 0xff;
			function_2308e0(&list, (s_controller_reference **)&reference);
		}
		value934 = NONE;
	}
done:
	if (g_54e7f8)
	{
		dialog_ok_show(3, 0x2c, 4, function_1901fc(), 0, 0);
		g_54e7f8 = false;
	}
}

/* B or back asks a signed in player whether to sign out */
// @retail 0x230c2b
bool c_main_menu_screen::v10(s_widget_event *event)
{
	long controller_index = event->controller_index;
	if (TEST_FIELD_BIT(((s_player_slot_sign_in_view *)g_54e8e0)[controller_index].signed_in) && event->type == 5)
	{
		switch (event->param)
		{
		case 1:
		case 13:
			((s_player_slot_sign_in_view *)g_54e8e0)[controller_index].profile.show_dialog(function_230c7d, 0x30);
			return true;
		}
	}
	return c_class_1473c9::v10(event);
}
