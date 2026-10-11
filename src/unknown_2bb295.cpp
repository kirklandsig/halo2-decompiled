#include <string.h>
#include "unknown_11c920.h"
#include "globals.h"
#include "screen_widgets.h"
#include "unknown_19b510.h"
#include "user_interface_lists.h"
#include "unknown_19b516.h"

// @flags /O1 /Oi /Gr

/* UNKNOWN_2BB295.CPP: the squad settings screen (vtable 0x45c518)
   and its list of what the squad's leader can change */

struct s_message;
void function_149f49(s_message *message, word a, dword *id, short b, long c, long d, long e);
byte *network_session_interface_get_data_4db0(void);
long function_19989d(void);
long function_1480ff(long screen_id);
bool function_1999b3(void);
void function_199a57(void);
void function_199a03(long mode);
void function_19a864(void);
bool function_19a76d(short index);
void function_121100(long *value);
void function_148d42(long value);
void dialog_ok_show(long a, long dialog_id, long b, short user_flags, dialog_choice_callback chosen, dialog_closed_callback closed);
void function_2c83e6(long type, long a, long b, word user_flags);

/* the squad settings that open a screen (unknown_2b116a.cpp) */
void function_2bb8ac(s_controller_reference **controller);
void function_2bb8df(s_controller_reference **controller);
void function_2bb912(s_controller_reference **controller);
void function_2bb945(s_controller_reference **controller);
void function_2bb9ce(s_controller_reference **controller);
void function_2bbf06(long controller_index, bool alternate);

struct s_widget_view_2b0a;
struct s_type_7ba8e9;
void function_2b0a14(s_widget_view_2b0a *widget, short index);
void function_2b0a7b(s_widget_view_2b0a *widget, s_type_7ba8e9 *bitmap);
void function_2b0ad3(long index, s_widget_view_2b0a *widget, long bitmap_index);
s_type_7ba8e9 *function_137550(long group_index, short bitmap_index);
bool function_19a84e(long *a, long *b);
short network_session_interface_get_value_5dd0(void);
struct s_entry_b;
struct s_entry_c;
s_entry_b *function_19c1f0(long key);
s_entry_c *function_19c5f0(long key);

/* the maps' and the variants' definitions (unknown_19c1d0.cpp): their
   bitmaps */
struct s_map_definition_view
{
	long map_id;
	byte unknown04[4];
	long bitmap_tag_index;
};

struct s_variant_definition_view
{
	byte unknown00[0xc];
	long bitmap_tag_index;
};

extern bool g_54d5a0;
extern bool g_54e7cc;

/* "squad setting list" (vtable 0x45c5f8) */
class c_squad_setting_list : public c_class_1474e8
{
public:
	c_squad_setting_list(word user_flags);

	/* a press of X opens the variant's settings */
	virtual bool v10(s_widget_event *event);
	virtual long get_item_count();
	virtual void v20(c_class_1a2c81 *item, long unused);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[7];
	c_list_item_handler handler;
};

/* the squad settings screen (vtable 0x45c518) */
class c_squad_settings_screen : public c_class_1473c9
{
public:
	c_squad_settings_screen(long a, long b, word user_flags);

	/* shows the focused setting's name, description and value */
	virtual void v3();
	/* a press of Y opens the squad privacy setting */
	virtual bool v10(s_widget_event *event);
	virtual void v18(void *parameters);
	virtual screen_load_proc get_load_proc();

	c_squad_setting_list list;
};

long function_2bbaad(c_squad_setting_list *list);

// @retail 0x2bbacb
c_class_1473c9 *__stdcall function_2bbacb(s_screen_parameters *parameters)
{
	c_squad_settings_screen *screen = new c_squad_settings_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2bbb07
c_squad_settings_screen::c_squad_settings_screen(long a, long b, word user_flags) :
	c_class_1473c9(0x17, a, b, user_flags),
	list(user_flags)
{
}

// @retail 0x2bbb3a deleting c_squad_settings_screen
// @retail 0x2bbb58 destructor c_squad_settings_screen

/* builds the screen around its list; the text says whether the user leads
   the squad */
// @retail 0x2bbb6d
void c_squad_settings_screen::v18(void *parameters)
{
	volatile long definition_index = function_1480ff(screen_id);
	s_screen_layout layout =
	{
		0,
		1,
		{
			{ 0, 0, &list, 0 }
		}
	};
	c_text_widget_45a5e0 *text;

	build(&layout);
	v7(&list);
	c_class_1a2c81::v1();
	text = (c_text_widget_45a5e0 *)find_child(6, 1, false);
	if (text)
	{
		if (function_1999b3())
		{
			text->function_253b1a(0xb0005f9);
		}
		else
		{
			text->function_253b1a(0xa0005f8);
		}
	}
}

// @retail 0x2bb299
screen_load_proc c_squad_settings_screen::get_load_proc()
{
	return function_2bbacb;
}

// @retail 0x2bbbf6
void c_squad_settings_screen::v3()
{
	long setting = function_2bbaad(&list);
	c_text_widget_45a5e0 *name_text = (c_text_widget_45a5e0 *)find_child(6, 2, false);
	c_text_widget_45a5e0 *description_text = (c_text_widget_45a5e0 *)find_child(6, 3, false);
	c_text_widget_45a5e0 *value_text = (c_text_widget_45a5e0 *)find_child(6, 4, false);
	s_widget_view_2b0a *bitmap = (s_widget_view_2b0a *)find_child(8, 4, false);
	long name;
	long description;
	long value;
	long bitmap_index = 0;
	long map_id;
	long variant_id;
	bool valid = function_19a84e(&map_id, &variant_id);

	if (bitmap)
	{
		function_2b0a7b(bitmap, 0);
	}
	switch (setting)
	{
	case 0:
		name = 0xf0005e0;
		description = 0xb0005ec;
		value = 0x30005f4;
		if (valid && bitmap)
		{
			s_map_definition_view *map = (s_map_definition_view *)function_19c5f0(variant_id);

			if (map)
			{
				function_2b0a7b(bitmap, function_137550(map->bitmap_tag_index, 0));
			}
		}
		break;
	case 1:
	{
		s_game_variant *variant;

		name = 0x130005e1;
		description = 0xf0005ed;
		value = 0x700055d;
		variant = (s_game_variant *)network_session_interface_get_data_4db0();
		if (bitmap && variant && variant->field_xcb8724)
		{
			long index;

			switch (variant->field_xcb8724)
			{
			case 1:
				index = 6;
				break;
			case 2:
				index = 0;
				break;
			case 3:
				index = 3;
				break;
			case 4:
				index = 1;
				break;
			case 7:
				index = 4;
				break;
			case 8:
				index = 8;
				break;
			case 9:
				index = 7;
				break;
			default:
				goto done;
			}
			function_2b0ad3(1, bitmap, index);
		}
		break;
	}
	case 2:
		name = 0x110005e2;
		description = 0xd0005ee;
		value = 0x50005f5;
		if (valid && bitmap)
		{
			s_variant_definition_view *definition = (s_variant_definition_view *)function_19c1f0(variant_id);

			if (definition)
			{
				function_2b0a7b(bitmap, function_137550(definition->bitmap_tag_index, 0));
			}
		}
		break;
	case 3:
	{
		short privacy;

		name = 0x160005e3;
		description = 0x120005ef;
		value = 0xa0005f6;
		privacy = network_session_interface_get_value_5dd0();
		if (bitmap)
		{
			long index;

			switch (privacy)
			{
			case 0:
				index = 0;
				break;
			case 1:
				index = 1;
				break;
			case 2:
				index = 2;
				break;
			case 3:
				index = 3;
				break;
			default:
				goto done;
			}
			function_2b0ad3(0, bitmap, index);
		}
		break;
	}
	case 4:
		name = 0x120005df;
		description = 0xf0005ed;
		value = 0x700055d;
		bitmap_index = 2;
		break;
	case 6:
		name = 0x130005e8;
		description = 0xe0005db;
		bitmap_index = 7;
		value = 0;
		break;
	case 7:
		name = 0x1a0005e7;
		description = 0x120005dd;
		bitmap_index = 6;
		value = 0;
		break;
	case 8:
		name = 0x180005e9;
		description = 0x130005dc;
		bitmap_index = 0;
		value = 0;
		break;
	case 9:
		name = 0x120005e5;
		description = 0xe0005f1;
		value = 0x60005f7;
		bitmap_index = 4;
		break;
	default:
		name = 0;
		description = 0;
		bitmap_index = 0;
		value = 0;
		break;
	}
done:
	if (name_text)
	{
		name_text->function_253b1a(name);
	}
	if (description_text)
	{
		description_text->function_253b1a(description);
	}
	if (value_text)
	{
		value_text->function_253b1a(value);
	}
	if (bitmap)
	{
		function_2b0a14(bitmap, (short)bitmap_index);
	}
	c_class_1a2c81::v3();
}

// @retail 0x2bbed0
bool c_squad_settings_screen::v10(s_widget_event *event)
{
	if (event->type == 5 && event->param == 2 && !function_1999b3())
	{
		function_2bb9ce((s_controller_reference **)&event);
		return true;
	}
	return c_class_1473c9::v10(event);
}

/* the setting the list's focused item stands for, or NONE */
// @retail 0x2bbaad
long function_2bbaad(c_squad_setting_list *list)
{
	s_list_item_datum *datum = (s_list_item_datum *)record_pool_lookup(list->data, list->get_focused_datum());

	if (datum)
	{
		return datum->item;
	}
	return NONE;
}

// @retail 0x2bb4d2
c_squad_setting_list::c_squad_setting_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_squad_setting_list::handle_item)
{
	long state = function_19989d();

	data = user_interface_data_new("squad setting list", 10, 4);
	function_16b790(data);
	switch (state)
	{
	case 0:
	case 2:
		list_item_add(this, 2);
		list_item_add(this, 3);
		list_item_add(this, 7);
		break;
	case 1:
	case 3:
		list_item_add(this, 0);
		list_item_add(this, 1);
		list_item_add(this, 4);
		list_item_add(this, 6);
		break;
	case 4:
		list_item_add(this, 2);
		list_item_add(this, 3);
		list_item_add(this, 7);
		list_item_add(this, 8);
		break;
	case 5:
		list_item_add(this, 0);
		list_item_add(this, 1);
		list_item_add(this, 4);
		list_item_add(this, 6);
		list_item_add(this, 8);
		break;
	case 6:
		list_item_add(this, 9);
		list_item_add(this, 6);
		list_item_add(this, 7);
		break;
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2bb295
long c_squad_setting_list::get_item_count()
{
	return 7;
}

// @retail 0x2bb74f
void c_squad_setting_list::v20(c_class_1a2c81 *item, long unused)
{
	s_list_item_text table[9];

	table[0].item = 0;
	table[0].string_handle = 0xa0005d3;
	table[1].item = 1;
	table[1].string_handle = 0xe0005d4;
	table[2].item = 2;
	table[2].string_handle = 0xc0005d5;
	table[3].item = 3;
	table[3].string_handle = 0x110005d6;
	table[4].item = 4;
	table[4].string_handle = 0xd00042a;
	table[5].item = 6;
	table[5].string_handle = 0xe0005db;
	table[6].item = 7;
	table[6].string_handle = 0x120005dd;
	table[7].item = 8;
	table[7].string_handle = 0x130005dc;
	table[8].item = 9;
	table[8].string_handle = 0xd0005d8;
	function_24c75c(this, item, table, 0, 9);
}

/* opens the settings of the session's variant's game engine */
// @retail 0x2bb978
void function_2bb978(s_controller_reference **controller)
{
	s_game_variant *variant = (s_game_variant *)network_session_interface_get_data_4db0();
	long type;

	switch (variant->field_xcb8724)
	{
	case 1:
		type = 0x16;
		break;
	case 2:
		type = 0x17;
		break;
	case 3:
		type = 0x18;
		break;
	case 4:
		type = 0x19;
		break;
	case 7:
		type = 0x1c;
		break;
	case 8:
		type = 0x1d;
		break;
	case 9:
		type = 0x1e;
		break;
	default:
		return;
	}
	function_2c83e6(type, 3, 4, 1 << (*controller)->controller_index);
}

/* starts the game */
// @retail 0x2bba01
void __stdcall function_2bba01(c_squad_setting_list *list, s_controller_reference **controller)
{
	if (g_54d5a0)
	{
		dialog_ok_show(1, 0x32, 4, 1 << (*controller)->controller_index, 0, 0);
	}
	else
	{
		long value;

		function_199a57();
		function_199a03(0);
		function_121100(&value);
		function_148d42(value);
		g_54e7cc = false;
		function_19a76d((short)value);
		list->get_screen()->start_animation(3);
	}
}

// @retail 0x2bba66
void __stdcall function_2bba66(c_squad_setting_list *list)
{
	function_199a57();
	function_199a03(2);
	function_19a864();
	list->get_screen()->start_animation(3);
}

// @retail 0x2bba8c
void __stdcall function_2bba8c(c_squad_setting_list *list, s_controller_reference **controller)
{
	function_2bbf06((*controller)->controller_index, true);
	list->get_screen()->start_animation(3);
}

/* the same as function_2bba8c (retail folded the two) */
void __stdcall function_2bba8c_alternate(c_squad_setting_list *list, s_controller_reference **controller)
{
	function_2bbf06((*controller)->controller_index, true);
	list->get_screen()->start_animation(3);
}

// @retail 0x2bb7dd
bool c_squad_setting_list::v10(s_widget_event *event)
{
	if (event->type == 6)
	{
		function_2bba8c(this, (s_controller_reference **)&event);
	}
	return ((c_widget *)this)->c_widget::v18((s_event *)event);
}

// @retail 0x2bb801
void c_squad_setting_list::handle_item(s_controller_reference **controller, long *item)
{
	if (*item != NONE)
	{
		s_list_item_datum *datum = &((s_list_item_datum *)data->data)[*item & 0xffff];

		switch (datum->item)
		{
		case 0:
			function_2bb8ac(controller);
			break;
		case 1:
			function_2bb8df(controller);
			break;
		case 2:
			function_2bb912(controller);
			break;
		case 3:
			function_2bb945(controller);
			break;
		case 4:
			function_2bb978(controller);
			break;
		case 6:
			function_2bba01(this, controller);
			break;
		case 7:
			function_2bba66(this);
			break;
		case 8:
			function_2bba8c(this, controller);
			break;
		case 9:
			function_2bba8c_alternate(this, controller);
			break;
		default:
			__assume(0);
		}
	}
}


