// @flags /O1 /Gr
/* UNKNOWN_230612.CPP: small virtual methods of the lists and widgets in
   0x230000..0x239b80 (one placeholder class per vtable until the classes are
   written) and two flag setting callbacks */

#include "unknown_11c920.h"
#include "main_globals.h"
#include "unknown_19b516.h"
#include "unknown_19b510.h"
#include "screen_widgets.h"
#include "user_interface_lists.h"

short player_slot_count_active(void);
c_class_1473c9 *__stdcall function_231db5(s_screen_parameters *parameters);

/* the list widgets' 22 slot vtables (0x458a74, 0x458b48 ...) */
class c_list_vtable
{
public:
	virtual ~c_list_vtable() {}
	virtual void v1() {}
	virtual void v2() {}
	virtual void v3() {}
	virtual void v4() {}
	virtual void v5() {}
	virtual void v6() {}
	virtual void v7() {}
	virtual void v8() {}
	virtual void v9() {}
	virtual bool v10(s_event *event) { return false; }
	virtual void v11() {}
	virtual void v12() {}
	virtual void v13() {}
	virtual void v14() {}
	virtual void v15() {}
	virtual void v16() {}
	virtual void v17() {}
	virtual void v18() {}
	virtual long v19() { return 0; }
	virtual void v20() {}
	virtual void v21() {}
};

/* the list of the screen at 0x458a00 (vtable 0x458a74) */
class c_list_458a74 : public c_list_vtable
{
public:
	virtual long v19();
};

/* the lists whose slot 10 is 0x230f31 (vtable 0x458b48 and others) */
class c_list_458b48 : public c_list_vtable
{
public:
	virtual bool v10(s_event *event);
};

// @retail 0x230612
long c_list_458a74::v19()
{
	return 5;
}

// @retail 0x230f31
bool c_list_458b48::v10(s_event *event)
{
	return ((c_widget *)this)->c_widget::v18(event);
}


/* the dialogs' choices: quit the game, restart the level */
// @retail 0x2323ab
bool __stdcall function_2323ab(long controller_index)
{
	main_globals.quit_game = true;
	return true;
}

// @retail 0x2323b7
bool __stdcall function_2323b7(long controller_index)
{
	main_globals.reset_map = true;
	return true;
}

/* ---- callbacks of the pause screens ---- */


/* closes the pause menu */
// @retail 0x23216c
void function_23216c(c_class_1a2c81 *screen)
{
	main_globals.unknown6f = true;
	screen->start_animation(3);
}

/* asks whether to restart the level */
// @retail 0x23217b
void function_23217b(c_class_1a2c81 *screen, s_controller_reference **controller)
{
	dialog_choice_show(3, 0xaa, 4, 1 << (*controller)->controller_index, function_2323b7, 0, 0);
	screen->start_animation(3);
}

/* opens the controller settings */
// @retail 0x2321ab
void function_2321ab(c_class_1473c9 *screen, s_controller_reference **controller)
{
	s_screen_parameters parameters;
	long window;
	long channel;

	screen->start_animation(3);
	parameters.field_c = 0;
	window = screen->v21();
	channel = screen->v20();
	function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, channel, window, (long)function_231db5);
	parameters.load(&parameters);
}

/* asks whether to quit the game (another dialog when several players are in it) */
// @retail 0x2321fc
void function_2321fc(c_class_1a2c81 *screen, s_controller_reference **controller)
{
	long dialog_id = 0xa8;

	if (player_slot_count_active() > 1)
	{
		dialog_id += 0xf;
	}
	dialog_choice_show_default(3, 4, 1 << (*controller)->controller_index, function_2323ab, dialog_id);
	screen->start_animation(3);
}

// @retail 0x231fe3
void c_pause_game_list::handle_item(s_controller_reference **controller, long *item)
{
	c_class_1473c9 *screen = get_screen();

	switch (*item & 0xffff)
	{
	case 0:
		screen->start_animation(3);
		break;
	case 1:
		function_23216c(screen);
		break;
	case 2:
		function_23217b(screen, controller);
		break;
	case 3:
		function_2321ab(screen, controller);
		break;
	default:
		function_2321fc(screen, controller);
		break;
	}
}

/* the string of a pause screen's item */
// @retail 0x232371
long function_232371(long item)
{
	long pause_item_string_id;

	switch (item)
	{
	case 0:
		pause_item_string_id = 0x100030b;
		break;
	case 1:
		pause_item_string_id = 0x100030c;
		break;
	case 2:
		pause_item_string_id = 0x100030d;
		break;
	case 3:
		pause_item_string_id = 0x100030e;
		break;
	case 4:
		pause_item_string_id = 0x100030f;
		break;
	}
	return pause_item_string_id;
}
