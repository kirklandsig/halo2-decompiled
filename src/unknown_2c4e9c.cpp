#include <string.h>
#include "unknown_11c920.h"
#include "globals.h"
#include <xtl.h>
#include <xonline.h>
#include "screen_widgets.h"
#include "unknown_19b510.h"
#include "user_interface_lists.h"
#include "unknown_19b516.h"
#include "unknown_234c64.h"

// @flags /O1 /Oi /Gr

/* UNKNOWN_2C4E9C.CPP: the small virtual methods of the screens and lists
   built in 0x2c4e00..0x2cb8c0 (the settings, variant, and custom game
   screens). Each class is named by its list's debug name where its
   constructor gives one, else by its retail vtable. */

/* the load procedures (not decompiled yet: stubs in src/stubs/lane_e.cpp) */
c_class_1473c9 *__stdcall function_2b54b2(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2c7dc3(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2c7e0f(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2c8362(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2c83a4(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2c8858(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2c8896(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2c8998(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2c89a8(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2c89b9(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2c89ca(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2c8a8f(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2bbacb(s_screen_parameters *parameters);

/* ---- opening screens ---- */

struct s_message;
void function_149f49(s_message *message, word a, dword *id, word b, long c, long d, long e);

/* the screens 0x2c7dc3/0x2c7e0f and 0x2c8362/0x2c83a4 load */
struct s_screen_view_2c83
{
	byte unknown00[0x9b4];
	long value9b4;
	byte unknown9b8[0xc9c - 0x9b8];
	long valuec9c;
};

// @retail 0x2c83e6
void function_2c83e6(long type, long a, long b, word user_flags)
{
	long types[9] = { 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e };
	s_screen_parameters parameters;
	screen_load_proc load;
	s_screen_view_2c83 *screen;
	long const *type_reference = &type;

	parameters.field_c = 0;
	load = function_2c7dc3;
	for (dword i = 0; i < 9; i++)
	{
		if (*type_reference == types[i])
		{
			load = function_2c7e0f;
			break;
		}
	}
	function_149f49((s_message *)&parameters, 0, 0, user_flags, a, b, (long)load);
	screen = (s_screen_view_2c83 *)parameters.load(&parameters);
	screen->valuec9c = *type_reference;
}

// @retail 0x2c8474
void function_2c8474(long value, long a, long b, word user_flags, bool alternate)
{
	s_screen_parameters parameters;
	s_screen_view_2c83 *screen;
	screen_load_proc load = alternate ? function_2c83a4 : function_2c8362;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, user_flags, a, b, (long)load);
	screen = (s_screen_view_2c83 *)parameters.load(&parameters);
	screen->value9b4 = value;
}

/* ---- bit vectors ---- */

// @retail 0x2cb220
void function_2cb220(long count, dword *bits)
{
	memset(bits, 0xff, ((count + 31) >> 5) * 4);
}

// @retail 0x2cb200
void function_2cb200(dword *bits, long small)
{
	long count;

	memset(bits, 0, 8);
	switch (small)
	{
	case 0:
		count = 64;
		break;
	case 1:
		count = 32;
		break;
	default:
		__assume(0);
	}
	function_2cb220(count, bits);
}

/* ---- lists ---- */

// @retail 0x2c4e9c
long c_campaign_level_handles_list::get_item_count()
{
	return 15;
}

// @retail 0x2c55f4
long c_game_engine_variant_category_list::get_item_count()
{
	return 9;
}

// @retail 0x2c6a7d
long c_xbox_live_message_list::get_item_count()
{
	return 14;
}

class c_list_45d078 : public c_class_1474e8
{
public:
	c_list_45d078(word user_flags);

	virtual void v17();
	virtual long get_item_count();
	/* shows the setting's name and its current value */
	virtual void v20(c_class_1a2c81 *item, long unused);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[12];
	long value688;
	long value68c;
	c_list_item_handler handler;
	bool alternate;
};

// @retail 0x2c7ac7
c_list_45d078::c_list_45d078(word user_flags) :
	c_class_1474e8(user_flags),
	value688(NONE),
	value68c(0),
	handler(this, (list_item_method)&c_list_45d078::handle_item)
{
	alternate = false;
}

/* the item's datum holds the value the screen it opens is loaded with */
struct s_list_45d078_datum
{
	byte unknown00[4];
	long *value;
};

// @retail 0x2c7d86
void c_list_45d078::handle_item(s_controller_reference **controller, long *item)
{
	s_list_45d078_datum *datum = (s_list_45d078_datum *)record_pool_lookup(data, *item);

	if (datum && datum->value)
	{
		function_2c8474(*datum->value, 3, 4, user_flags, alternate);
	}
}

/* the tag references a category's options are read from (the list
   function_236a3a finds by its id) */
struct s_option_reference
{
	long group;
	long index;
};

struct s_option_reference_list
{
	long id;
	byte unknown04[4];
	long count;
	s_option_reference *references;
};

struct s_reference_list;
s_reference_list *function_236a3a(long id);

/* one item per option of the category: each holds its option's tag */
// @retail 0x2c7b7d
void c_list_45d078::v17()
{
	value68c = (long)(s_option_reference_list *)function_236a3a(value688);
	if (value68c && ((s_option_reference_list *)value68c)->count > 0)
	{
		data = user_interface_data_new("variant category options list", ((s_option_reference_list *)value68c)->count, 8);
		function_16b790(data);
		for (long i = 0; i < data->maximum_count; i++)
		{
			s_option_reference *reference = &((s_option_reference_list *)value68c)->references[i];
			long *value = 0;
			s_list_45d078_datum *datum = &((s_list_45d078_datum *)data->data)[record_pool_allocate(data) & 0xffff];

			if (reference->index != NONE)
			{
				value = (long *)g_4e3b44[reference->index & 0xffff].data;
			}
			datum->value = value;
		}
		delegate_register(&item_handlers, &handler);
	}
	else
	{
		data = user_interface_data_new("EMPTY variant category options list", 1, 8);
		function_16b790(data);
		((s_list_45d078_datum *)data->data)[record_pool_allocate(data) & 0xffff].value = 0;
	}
	((c_widget *)this)->c_widget::v1();
}

// @retail 0x2c7b29 deleting c_list_45d078
// @retail 0x2c7b47 destructor c_list_45d078

// @retail 0x2c7a9b
long c_list_45d078::get_item_count()
{
	return 12;
}

/* ---- screens ---- */

#pragma pack(push, 4)
struct s_clan_task_target
{
	unsigned __int64 xuid;
	long unknown8;
};
#pragma pack(pop)

/* a message as the screen keeps it (online_message_entries.h's s_entry) */
#pragma pack(push, 4)
struct s_online_message_view
{
	s_clan_task_target xuid;
	byte unknown0c[0x1c - 0x0c];
	union
	{
		dword flags;
		struct
		{
			dword unknown_bits0 : 10;
			dword flag10 : 1;
			dword flag11 : 1;
			dword flag12 : 1;
			dword flag13 : 1;
			dword flag14 : 1;
			dword flag15 : 1;
			dword flag16 : 1;
		} flag_bits;
	};
	dword message_id;
	dword title_id;
	union
	{
		s_clan_task_target sender;
		struct
		{
			byte unknown28[8];
			FILETIME sent_time;
		} times;
	};
	byte unknown38[0x40 - 0x38];
};
#pragma pack(pop)

/* the xbox live message display screen (vtable 0x45cf98) */
class c_xbox_live_message_display_screen : public c_screen_with_menu
{
public:
	c_xbox_live_message_display_screen(long a, long b, word user_flags);

	/* stops the voice mail and the online tasks */
	virtual void v2();
	/* shows the message once its details are read, or opens the screen
	   the message list goes back to */
	virtual void v3();
	/* reads the user's messages, sorts them and shows the first */
	virtual void v19();
	virtual screen_load_proc get_load_proc();

	/* reads the message's details when their task is done, and shows them */
	void update_details();
	void read_details();
	/* checks the voice attachment once its download is done */
	void update_attachment();
	void play_voice_mail(long controller_index);
	/* shows the next message the screen's mode lists, or marks the screen to
	   go back when none is left */
	void show_next_message();

	c_xbox_live_message_list list;
	long value_db8;
	byte unknowndbc[4];
	s_online_message_view messages[0x7d];
	long message_count;
	long value2d04;
	long task_2d08;
	long task_2d0c;
	word message_text[0x400];
	long value3510;
	byte attachment[0x499a];
	byte unknown7eae[2];
	dword voice_mail_length;
	bool value7eb4;
	word value7eb6;
	dword voice_mail_duration;
	byte unknown7ebc[4];
	unsigned __int64 value7ec0;
	long voice_port_mode;
	bool value7ecc;
};

void voice_mail_stop(long port);
void voice_set_port_mode(long port, long mode);
void function_6b640(long task_index);

/* the list waiting for a dialog's answer */
c_xbox_live_message_list *g_51ecd4;

// @retail 0x2c6aa5
c_xbox_live_message_list::c_xbox_live_message_list(word user_flags) :
	c_class_1474e8(user_flags),
	message(0),
	handler(this, (list_item_method)&c_xbox_live_message_list::handle_item)
{
	data = user_interface_data_new("xbox live message list", 14, 4);
	function_16b790(data);
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2c6a3a
c_xbox_live_message_list::~c_xbox_live_message_list()
{
	g_51ecd4 = 0;
}

// @retail 0x2c6a81 deleting c_xbox_live_message_list

// @retail 0x2c6dd8
void c_xbox_live_message_list::v20(c_class_1a2c81 *item, long unused)
{
	s_list_item_text table[14];

	table[0].item = 0;
	table[0].string_handle = 0x150002b5;
	table[1].item = 1;
	table[1].string_handle = 0x160002b6;
	table[2].item = 2;
	table[2].string_handle = 0x200002b7;
	table[3].item = 3;
	table[3].string_handle = 0x130002bb;
	table[4].item = 4;
	table[4].string_handle = 0x140002bc;
	table[5].item = 5;
	table[5].string_handle = 0x1e0002bd;
	table[6].item = 6;
	table[6].string_handle = 0x120002b0;
	table[7].item = 7;
	table[7].string_handle = 0x130002b1;
	table[8].item = 8;
	table[8].string_handle = 0x130002b2;
	table[9].item = 9;
	table[9].string_handle = 0x140002b3;
	table[10].item = 10;
	table[10].string_handle = 0xc0002e4;
	table[11].item = 12;
	table[11].string_handle = 0xe0002e5;
	table[12].item = 13;
	table[12].string_handle = 0xd0002ac;
	table[13].item = 11;
	table[13].string_handle = 0x100002ad;
	function_24c75c(this, item, table, 0, 14);
}

bool function_19acc6(XUID const *xuid);
bool online_title_is_this_title(DWORD title_id);

// @retail 0x2c6b65
void c_xbox_live_message_list::set_message(s_online_message_view *message)
{
	bool item0 = false;
	bool item1 = false;
	bool item2 = false;
	bool item3 = false;
	bool item4 = false;
	bool item5 = false;
	bool item6 = false;
	bool item7 = false;
	bool item11 = false;
	bool item12;

	this->message = message;
	record_pool_release_all(data);
	if (this->message)
	{
		bool not_in_session = !function_19acc6((XUID const *)&message->xuid);
		bool other_title;

		item12 = false;
		if (TEST_FIELD_BIT(this->message->flag_bits.flag10))
		{
			item2 = true;
			item1 = true;
			item0 = true;
		}
		else if (TEST_FIELD_BIT(this->message->flag_bits.flag11))
		{
			item6 = not_in_session;
			item7 = true;
		}
		else if (TEST_FIELD_BIT(this->message->flag_bits.flag12))
		{
			item5 = true;
			item4 = true;
			item3 = true;
		}
		else
		{
			if (!TEST_FIELD_BIT(this->message->flag_bits.flag13) &&
				!TEST_FIELD_BIT(this->message->flag_bits.flag14) &&
				!TEST_FIELD_BIT(this->message->flag_bits.flag15) &&
				TEST_FIELD_BIT(this->message->flag_bits.flag16))
			{
				item11 = true;
			}
			item12 = true;
		}
		other_title = !online_title_is_this_title(this->message->title_id);
		if (item0)
			list_item_add(this, 0);
		if (item1)
			list_item_add(this, 1);
		if (item2)
			list_item_add(this, 2);
		if (item3)
			list_item_add(this, 3);
		if (item4)
			list_item_add(this, 4);
		if (item5)
			list_item_add(this, 5);
		if (item6)
			list_item_add(this, other_title ? 6 : 8);
		if (item7)
			list_item_add(this, other_title ? 7 : 9);
		list_item_add(this, 10);
		if (item11)
			list_item_add(this, 11);
		if (item12)
			list_item_add(this, 12);
		list_item_add(this, 13);
	}
	function_24c0c4((c_widget *)this);
	v7(child);
}

void online_message_delete(unsigned long controller_index, unsigned long message_id, bool block_sender);
void function_238e42(long controller_index, long type);
void function_239197(long controller_index);
void function_2391e2(long controller_index);

/* replies to the message, then deletes it */
// @retail 0x2c6ea7
void __stdcall function_2c6ea7(c_xbox_live_message_list *list, s_controller_reference **controller)
{
	function_238e42((*controller)->controller_index, 4);
	online_message_delete((*controller)->controller_index, list->message->message_id, false);
}

/* declines the friend request */
// @retail 0x2c6fe5
void function_2c6fe5(c_xbox_live_message_list *list, s_controller_reference **controller)
{
	c_class_1473c9 *screen = list->get_screen();

	function_239197((*controller)->controller_index);
	screen->start_animation(3);
}

/* blocks the sender, once the user confirms */
// @retail 0x2c7008
bool __stdcall function_2c7008(long controller_index)
{
	function_2391e2(controller_index);
	if (g_51ecd4)
	{
		g_51ecd4->get_screen()->start_animation(3);
	}
	return true;
}

// @retail 0x2c702e
void function_2c702e(c_xbox_live_message_list *list, s_controller_reference **controller)
{
	g_51ecd4 = list;
	dialog_choice_show(1, 0xa4, 4, 1 << (*controller)->controller_index, function_2c7008, 0, 0);
}


extern s_clan_task_target g_54e420;
void __stdcall function_23933a(long controller_index);
void __stdcall function_239374(long controller_index);

/* accepts the clan invitation */
// @retail 0x2c7089
void function_2c7089(c_xbox_live_message_list *list, s_controller_reference **controller)
{
	c_class_1473c9 *screen = list->get_screen();
	s_online_message_view *message = list->message;

	g_54e420 = message->sender;
	function_23933a((*controller)->controller_index);
	screen->start_animation(3);
}

// @retail 0x2c70c1
bool __stdcall function_2c70c1(long controller_index)
{
	function_239374(controller_index);
	if (g_51ecd4)
	{
		g_51ecd4->get_screen()->start_animation(3);
	}
	return true;
}

/* declines the clan invitation, once the user confirms */
// @retail 0x2c70e7
void function_2c70e7(c_xbox_live_message_list *list, s_controller_reference **controller)
{
	g_51ecd4 = list;
	s_online_message_view *message = list->message;

	g_54e420 = message->sender;
	dialog_choice_show(1, 0xa4, 4, 1 << (*controller)->controller_index, function_2c70c1, 0, 0);
}

// @retail 0x2b54b2
c_class_1473c9 *__stdcall function_2b54b2(s_screen_parameters *parameters)
{
	c_xbox_live_message_display_screen *screen = new c_xbox_live_message_display_screen(parameters->a, parameters->b, parameters->user_flags);

	if (screen)
	{
		screen->m6c = true;
		screen->function_147f6d(parameters);
	}
	return screen;
}

// @retail 0x2c7224
c_xbox_live_message_display_screen::c_xbox_live_message_display_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0xd3, a, b, user_flags, &list),
	list(user_flags)
{
	task_2d08 = NONE;
	task_2d0c = NONE;
	value_db8 = 0;
	value2d04 = 0;
	value3510 = 0;
	voice_mail_length = 0;
	value7eb4 = false;
	value7eb6 = 0;
	voice_mail_duration = 0;
	value7ec0 = 0;
	voice_port_mode = 0;
	value7ecc = false;
	message_text[0] = 0;
	message_count = 0x7d;
}

// @retail 0x2c72bd deleting c_xbox_live_message_display_screen
// @retail 0x2c72db destructor c_xbox_live_message_display_screen

// @retail 0x2c73ac
void c_xbox_live_message_display_screen::v2()
{
	if (voice_mail_length > 0)
	{
		voice_mail_stop(get_controller_index());
	}
	voice_set_port_mode(get_controller_index(), voice_port_mode);
	if (task_2d0c != NONE)
	{
		function_6b640(task_2d0c);
		task_2d0c = NONE;
	}
	if (task_2d08 != NONE)
	{
		function_6b640(task_2d08);
		task_2d08 = NONE;
	}
	c_class_1a2c81::v2();
}

// @retail 0x2c6a9f
screen_load_proc c_xbox_live_message_display_screen::get_load_proc()
{
	return function_2b54b2;
}

long online_task_poll(long task_index);
bool online_message_details_get_property(long task_index, long property, void *buffer, DWORD size, bool *too_small, DWORD *required_size);
long online_message_download_attachment(long details_task_index, long property, void *buffer, DWORD size);
bool online_message_download_get_results(long task_index, DWORD *received_size, BYTE **data, DWORD *total_size);
bool online_title_is_this_title(DWORD title_id);
bool function_1906da(long index);
void parse_replace_missing_characters(word *string, long count);
void function_253bc9(c_text_widget_458940 *widget, long subtitle_type);
bool function_230265(c_class_1473c9 *screen);
void voice_play_voice_mail(long port, const long *data, long size);
void function_236299(long sound);
c_class_1473c9 *__stdcall function_2b7152(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b7162(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b7173(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b7184(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b7195(s_screen_parameters *parameters);

/* the time the shown message was sent */
SYSTEMTIME g_54e7ce;

// @retail 0x2c74f9
void c_xbox_live_message_display_screen::update_details()
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)find_child(6, 3, false);
	c_text_widget_45a5e0 *time_text = (c_text_widget_45a5e0 *)find_child(6, 2, false);
	c_text_widget_45a5e0 *voice_text = (c_text_widget_45a5e0 *)find_child(6, 5, false);

	read_details();
	if (text)
	{
		if (message_text[0] == 0)
		{
			s_online_message_view *message = list.message;

			if (TEST_FIELD_BIT(message->flag_bits.flag11))
			{
				if (!online_title_is_this_title(message->title_id))
				{
					text->function_253b1a(0x110002db);
				}
				else
				{
					text->function_253b1a(0x120002da);
				}
			}
			else if (TEST_FIELD_BIT(message->flag_bits.flag10))
			{
				text->function_253b1a(0x130002dc);
			}
			else if (TEST_FIELD_BIT(message->flag_bits.flag12))
			{
				text->function_253b1a(0x110002dd);
			}
			else
			{
				text->function_253b1a(0x140002de);
			}
		}
		else
		{
			text->function_22f52e()->set_text(message_text);
		}
	}
	if (time_text)
	{
		s_online_message_view *message = list.message;
		FILETIME local_time;
		SYSTEMTIME system_time;

		if (FileTimeToLocalFileTime(&message->times.sent_time, &local_time) &&
			FileTimeToSystemTime(&local_time, &system_time))
		{
			g_54e7ce = system_time;
			if (system_time.wHour < 12)
			{
				time_text->function_253b1a(0x70002df);
			}
			else
			{
				time_text->function_253b1a(0x70002e0);
			}
		}
	}
	if (voice_text)
	{
		if (value7eb4)
		{
			voice_text->value6e = true;
			if (voice_mail_duration > 0)
			{
				voice_text->function_253b1a(0x120002e1);
			}
			else
			{
				voice_text->function_253b1a(0x110002e2);
			}
		}
		else
		{
			voice_text->value6e = false;
		}
	}
	if (value7eb4 && voice_mail_duration > 0)
	{
		subtitle.function_253b1a(0xb0002e3);
	}
	else
	{
		function_253bc9(&subtitle, 1);
	}
}

__forceinline void c_xbox_live_message_display_screen::read_details()
{
	if (task_2d08 != NONE)
	{
		switch (online_task_poll(task_2d08))
		{
		case 0:
			return;
		case 1:
			return;
		case 2:
		{
			bool too_small = false;
			DWORD required_size;

			if (function_1906da(get_controller_index()) &&
				online_message_details_get_property(task_2d08, 3, message_text, sizeof(message_text), &too_small, &required_size))
			{
				parse_replace_missing_characters(message_text, 0x400);
				if (online_message_details_get_property(task_2d08, 4, &value3510, sizeof(value3510), &too_small, &required_size))
				{
					XGetLanguage();
				}
			}
			voice_mail_length = 0;
			value7eb6 = 0;
			voice_mail_duration = 0;
			if (!function_1906da(get_controller_index()))
			{
				value7eb4 = true;
			}
			else if (online_message_details_get_property(task_2d08, 1, &value7eb6, sizeof(value7eb6), &too_small, &required_size) &&
				value7eb6 == 1 &&
				online_message_details_get_property(task_2d08, 2, &voice_mail_duration, sizeof(voice_mail_duration), &too_small, &required_size))
			{
				if (online_message_details_get_property(task_2d08, 0, attachment, sizeof(attachment), &too_small, &required_size) || too_small)
				{
					value7eb4 = true;
					if (too_small)
					{
						task_2d0c = online_message_download_attachment(task_2d08, 0, attachment, sizeof(attachment));
					}
				}
			}
			value7ec0 = 0;
			online_message_details_get_property(task_2d08, 5, &value7ec0, 8, &too_small, &required_size);
		}
		}
		function_6b640(task_2d08);
		task_2d08 = NONE;
	}
}

// @retail 0x2c77fe
void c_xbox_live_message_display_screen::update_attachment()
{
	if (task_2d0c != NONE)
	{
		switch (online_task_poll(task_2d0c))
		{
		case 0:
			return;
		case 1:
			return;
		case 2:
		{
			BYTE *data;
			DWORD total_size;
			bool success = online_message_download_get_results(task_2d0c, &voice_mail_length, &data, &total_size);

			if (success)
			{
				if (voice_mail_length == total_size)
				{
					play_voice_mail(get_controller_index());
				}
				else
				{
					success = false;
				}
			}
			if (!success)
			{
				voice_mail_length = 0;
				value7eb6 = 0;
				voice_mail_duration = 0;
			}
		}
		}
		function_6b640(task_2d0c);
		task_2d0c = NONE;
	}
}

// @retail 0x2c7a6a
void c_xbox_live_message_display_screen::play_voice_mail(long controller_index)
{
	voice_mail_stop(controller_index);
	if (voice_mail_length > 0)
	{
		voice_play_voice_mail(controller_index, (const long *)attachment, voice_mail_length);
	}
	else
	{
		function_236299(2);
	}
}

bool xuid_equal(XUID const *a, XUID const *b, bool compare_guest_number);
long online_message_details(DWORD controller_index, const s_entry *entry);
void function_14887e(s_screen_settings_54dc6c *settings);

/* the settings function_14887e copies out: the mode, then the user's xuid */
struct s_screen_settings_view
{
	long mode;
	XUID xuid;
	byte unknown0c[0x78 - 0x0c];
};

// @retail 0x2c788a
void c_xbox_live_message_display_screen::show_next_message()
{
	s_screen_settings_view settings;

	function_14887e((s_screen_settings_54dc6c *)&settings);
	message_text[0] = 0;
	value3510 = 0;
	voice_mail_length = 0;
	value7eb6 = 0;
	voice_mail_duration = 0;
	value7eb4 = false;
	for (; value2d04 < message_count; value2d04++)
	{
		c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)find_child(6, 0, false);
		long controller_index = get_controller_index();
		s_online_message_view *message = &messages[value2d04];
		XUID const *xuid = NULL;
		bool other_title;

		if (message)
			xuid = (XUID const *)&message->xuid;
		if (task_2d08 != NONE)
		{
			function_6b640(task_2d08);
			task_2d08 = NONE;
		}
		if (task_2d0c != NONE)
		{
			function_6b640(task_2d0c);
			task_2d0c = NONE;
		}
		other_title = !online_title_is_this_title(message->title_id);
		if (xuid_equal(xuid, &settings.xuid, false) &&
			(!TEST_FIELD_BIT(message->flag_bits.flag10) || value_db8 == 1) &&
			(!TEST_FIELD_BIT(message->flag_bits.flag12) || value_db8 == 2))
		{
			if (text)
			{
				long string_handle;

				if (TEST_FIELD_BIT(message->flag_bits.flag10))
					string_handle = 0x1d0002cc;
				else if (TEST_FIELD_BIT(message->flag_bits.flag11))
				{
					if (other_title)
						string_handle = 0x1a0002cd;
					else
						string_handle = 0x1b0002ce;
				}
				else if (TEST_FIELD_BIT(message->flag_bits.flag12))
					string_handle = 0x1b0002d1;
				else if (TEST_FIELD_BIT(message->flag_bits.flag13))
					string_handle = 0x220002d2;
				else if (TEST_FIELD_BIT(message->flag_bits.flag14))
					string_handle = 0x180002d3;
				else if (TEST_FIELD_BIT(message->flag_bits.flag15))
					string_handle = 0x150002d4;
				else
					string_handle = TEST_FIELD_BIT(message->flag_bits.flag16) ? 0x140002d5 : 0x160002d8;
				text->function_253b1a(string_handle);
			}
			task_2d08 = online_message_details(controller_index, (const s_entry *)message);
			list.set_message(message);
			value2d04++;
			return;
		}
	}
	value7ecc = true;
}

/* shows the next message, after the list's item was handled */
// @retail 0x2c7215
void function_2c7215(c_xbox_live_message_list *list)
{
	((c_xbox_live_message_display_screen *)list->get_screen())->show_next_message();
}

void function_23914b(long controller_index);
void function_239292(long controller_index);

// @retail 0x2c6fcf
void function_2c6fcf(s_controller_reference **controller, c_xbox_live_message_list *list)
{
	function_23914b((*controller)->controller_index);
	function_2c7215(list);
}

// @retail 0x2c705c
void function_2c705c(c_xbox_live_message_list *list, s_controller_reference **controller)
{
	s_online_message_view *message = list->message;

	g_54e420 = message->sender;
	function_239292((*controller)->controller_index);
	function_2c7215(list);
}

void __stdcall function_238f3f(long controller_index, void *message, unsigned __int64 value);
void __stdcall function_23902b(void *message, long controller_index, unsigned __int64 value);
void function_238f11(long controller_index);

/* answers the game invitation in the message */
// @retail 0x2c7142
void function_2c7142(c_xbox_live_message_list *list, long controller_index)
{
	c_xbox_live_message_display_screen *screen = (c_xbox_live_message_display_screen *)list->get_screen();

	function_238f3f(controller_index, list->message, screen->value7ec0);
}

// @retail 0x2c712a
bool __stdcall function_2c712a(long controller_index)
{
	if (g_51ecd4)
	{
		function_2c7142(g_51ecd4, controller_index);
	}
	return true;
}

/* accepts the game invitation, once the user confirms when a game is on */
// @retail 0x2c7165
void function_2c7165(c_xbox_live_message_list *list, s_controller_reference **controller)
{
	if (g_4e6948->state == 1)
	{
		g_51ecd4 = list;
		dialog_choice_show(1, 9, 4, 1 << (*controller)->controller_index, function_2c712a, 0, 0);
	}
	else
	{
		function_2c7142(list, (*controller)->controller_index);
	}
}

/* declines what the message asks, by its kind, then shows the next message */
// @retail 0x2c71a0
void function_2c71a0(c_xbox_live_message_list *list, s_controller_reference **controller)
{
	long controller_index = list->get_controller_index();
	s_online_message_view *message = list->message;

	if (TEST_FIELD_BIT(message->flag_bits.flag11))
	{
		c_xbox_live_message_display_screen *screen = (c_xbox_live_message_display_screen *)list->get_screen();

		function_23902b(message, (*controller)->controller_index, screen->value7ec0);
	}
	else if (TEST_FIELD_BIT(message->flag_bits.flag12))
	{
		function_23933a((*controller)->controller_index);
	}
	else if (TEST_FIELD_BIT(message->flag_bits.flag10))
	{
		function_239197(controller_index);
	}
	else
	{
		online_message_delete(controller_index, message->message_id, false);
	}
	function_2c7215(list);
}

/* plays the voice mail again */
// @retail 0x2c6fbc
void function_2c6fbc(c_xbox_live_message_list *list, s_controller_reference **controller)
{
	((c_xbox_live_message_display_screen *)list->get_screen())->play_voice_mail((*controller)->controller_index);
}

// @retail 0x2c6b20
bool c_xbox_live_message_list::v10(s_widget_event *event)
{
	c_xbox_live_message_display_screen *screen = (c_xbox_live_message_display_screen *)get_screen();
	bool handled = false;

	if (event->type == 5)
	{
		switch (event->param)
		{
		case 1:
			if (screen)
			{
				handled = true;
				screen->value7ecc = handled;
			}
			break;
		case 2:
			function_2c6fbc(this, (s_controller_reference **)&event);
			handled = true;
			break;
		}
	}
	if (!handled)
	{
		handled = ((c_widget *)this)->c_widget::v18((s_event *)event);
	}
	return handled;
}

// @retail 0x2c7410
void c_xbox_live_message_display_screen::v3()
{
	c_class_2b01eb *meter = (c_class_2b01eb *)find_child(8, 6, false);

	if (meter)
	{
		meter->value84 = (real)voice_mail_duration * (1.0f / 15000.0f);
	}
	if (!value7ecc && list.message)
	{
		update_details();
		update_attachment();
	}
	else if (!function_230265(this))
	{
		s_screen_parameters parameters;

		parameters.field_c = 0;
		function_149f49((s_message *)&parameters, 0, 0, (word)(1 << get_controller_index()), 3, 4, (long)function_2b7152);
		switch (value_db8)
		{
		case 0:
			parameters.load = function_2b7184;
			break;
		case 1:
			parameters.load = function_2b7162;
			break;
		case 2:
			parameters.load = function_2b7173;
			break;
		case 3:
			parameters.load = function_2b7184;
			break;
		case 4:
			parameters.load = function_2b7195;
			break;
		}
		parameters.load(&parameters);
		start_animation(3);
		value7ecc = false;
	}
	c_class_1a2c81::v3();
}

/* friend requests first, then the older messages first */
// @retail 0x2c72f0
bool __stdcall message_compare(const void *a, const void *b, const void *context)
{
	s_online_message_view const *message_a = (s_online_message_view const *)a;
	s_online_message_view const *message_b = (s_online_message_view const *)b;
	bool result = false;

	if (TEST_FIELD_BIT(message_b->flag_bits.flag11) && !TEST_FIELD_BIT(message_a->flag_bits.flag11))
	{
		result = true;
	}
	else if (TEST_FIELD_BIT(message_a->flag_bits.flag11) && !TEST_FIELD_BIT(message_b->flag_bits.flag11))
	{
		result = false;
	}
	else if (message_a->times.sent_time.dwHighDateTime < message_b->times.sent_time.dwHighDateTime ||
		(message_a->times.sent_time.dwHighDateTime == message_b->times.sent_time.dwHighDateTime &&
		message_a->times.sent_time.dwLowDateTime < message_b->times.sent_time.dwLowDateTime))
	{
		result = true;
	}
	return result;
}

void __stdcall online_messages_enumerate(DWORD controller_index, s_entry *entries, long *count);
typedef bool (__stdcall *t_compare_function)(const void *, const void *, const void *);
void function_13da70(void *elements, unsigned long count, unsigned long element_size, t_compare_function compare, const void *context);
long voice_get_port_mode(long port);

// @retail 0x2c7346
void c_xbox_live_message_display_screen::v19()
{
	online_messages_enumerate(get_controller_index(), (s_entry *)messages, &message_count);
	function_13da70(messages, message_count, sizeof(s_online_message_view), message_compare, 0);
	show_next_message();
	c_class_1473c9::v19();
	voice_port_mode = voice_get_port_mode(get_controller_index());
	voice_set_port_mode(get_controller_index(), 3);
}

// @retail 0x2c6ecf
void c_xbox_live_message_list::handle_item(s_controller_reference **controller, long *item)
{
	if (*item != NONE)
	{
		switch (((s_list_item_datum *)data->data)[*item & 0xffff].item)
		{
		case 0:
			function_2c6fcf(controller, this);
			break;
		case 1:
			function_2c6fe5(this, controller);
			break;
		case 2:
			function_2c702e(this, controller);
			break;
		case 3:
			function_2c705c(this, controller);
			break;
		case 4:
			function_2c7089(this, controller);
			break;
		case 5:
			function_2c70e7(this, controller);
			break;
		case 6:
			function_2c7165(this, controller);
			break;
		case 8:
			function_2c7165(this, controller);
			break;
		case 10:
			function_2c7215(this);
			break;
		case 7:
			function_2c71a0(this, controller);
			break;
		case 9:
			function_2c71a0(this, controller);
			break;
		case 12:
			function_2c71a0(this, controller);
			break;
		case 13:
			function_238f11((*controller)->controller_index);
			break;
		case 11:
			function_2c6ea7(this, controller);
			break;
		}
	}
}

/* the screen of the list of 0x45d078 (vtable 0x45d140): its screen id is
   set by the create function */
class c_screen_45d140 : public c_screen_with_menu
{
public:
	c_screen_45d140(long a, long b, word user_flags);

	virtual bool v10(s_widget_event *event);
	virtual screen_load_proc get_load_proc();

	c_list_45d078 list;
};

// @retail 0x2c7dc3
c_class_1473c9 *__stdcall function_2c7dc3(s_screen_parameters *parameters)
{
	c_screen_45d140 *screen = new c_screen_45d140(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->screen_id = 0xdd;
	screen->list.alternate = false;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2c7e0f
c_class_1473c9 *__stdcall function_2c7e0f(s_screen_parameters *parameters)
{
	c_screen_45d140 *screen = new c_screen_45d140(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->screen_id = 0xde;
	screen->list.alternate = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2c7e5b
c_screen_45d140::c_screen_45d140(long a, long b, word user_flags) :
	c_screen_with_menu(NONE, a, b, user_flags, &list),
	list(user_flags)
{
}

// @retail 0x2c7e8d deleting c_screen_45d140
// @retail 0x2c7eab destructor c_screen_45d140

// @retail 0x2c7ec0
bool c_screen_45d140::v10(s_widget_event *event)
{
	if (event->type == 5)
	{
		switch (event->param)
		{
		case 1:
		case 13:
			if (list.alternate)
			{
				s_screen_parameters parameters;

				parameters.field_c = 0;
				function_149f49((s_message *)&parameters, 0, 0, 1 << event->controller_index, 3, 4, (long)function_2bbacb);
				parameters.load(&parameters);
			}
			break;
		}
	}
	return c_class_1473c9::v10(event);
}

// @retail 0x2c7a9f
screen_load_proc c_screen_45d140::get_load_proc()
{
	return list.alternate ? function_2c7e0f : function_2c7dc3;
}

/* ---- the variant parameter setting screen (vtable 0x45d0d0): the values
   one setting of the variant can take ---- */

void function_1a0180(long tag_index, long string_handle, word *buffer);
long *function_2369b3(long id);
struct s_indexed_block;
struct s_indexed_entry;
s_indexed_entry *function_236aa9(s_indexed_block *block, long id);
long function_2374f0(void *base, long index);
void function_2373be(long index, void *base, long value);
bool __stdcall function_19a728(s_game_variant *variant);
byte *network_session_interface_get_data_4db0(void);

struct s_list_item_iterator
{
	byte *item;
	s_record_pool_iterator iterator;
};

bool function_2b2327(s_list_item_iterator *iterator);

/* a value a setting can take */
struct s_variant_setting_option
{
	byte unknown00[4];
	long value;
	long string_handle;
};

/* a setting of the variant (a block of the user interface globals tag):
   the field it edits, its strings and its values */
struct s_variant_setting_definition
{
	long field_index;
	byte unknown04[8];
	long string_list_index;
	long name_string_id;
	long title_string_id;
	long description_string_id;
	long option_count;
	s_variant_setting_option *options;
};

/* the item's datum: the value it stands for */
struct s_variant_setting_datum
{
	byte unknown00[4];
	s_variant_setting_option *option;
};

/* the variant being edited, or (alternate) a copy of the session's */
static __forceinline s_game_variant *variant_settings_get_variant(bool alternate, s_game_variant *buffer)
{
	s_game_variant *variant = alternate ? buffer : &g_54e4a0;

	if (alternate)
	{
		byte *data = network_session_interface_get_data_4db0();

		if (data)
		{
			memcpy(variant, data, sizeof(s_game_variant));
		}
		else
		{
			variant = 0;
		}
	}
	return variant;
}

/* "variant parameter setting list" (vtable 0x45d020) */
class c_variant_parameter_setting_list : public c_class_1474e8
{
public:
	c_variant_parameter_setting_list(word user_flags);

	/* builds the items from the setting's values */
	virtual void v17();
	/* folded with c_list_45af88's */
	virtual long get_item_count() { return 6; }
	virtual void v20(c_class_1a2c81 *item, long unused);

	void handle_item(s_controller_reference **controller, long *item);
	/* focuses the item of the setting's current value */
	void select_current_value();

	c_class_14750b items[6];
	c_list_item_handler handler;
	long setting_index;
	s_variant_setting_definition *definition;
	bool alternate;
};

class c_variant_parameter_setting_screen : public c_screen_with_menu
{
public:
	c_variant_parameter_setting_screen(long a, long b, word user_flags);

	/* shows the setting's title and description */
	virtual void v19();
	virtual screen_load_proc get_load_proc();

	c_variant_parameter_setting_list list;
};

// @retail 0x2c7c56
void c_list_45d078::v20(c_class_1a2c81 *item, long unused)
{
	long datum_index = widget_item(item)->value70;

	if (datum_index != NONE)
	{
		s_list_45d078_datum *datum = (s_list_45d078_datum *)record_pool_lookup(data, datum_index);
		c_class_1a2c81 *name_text = item->find_child(6, 0, false);
		c_class_1a2c81 *value_text = item->find_child(6, 1, false);

		if (datum && datum->value && ((s_variant_setting_definition *)datum->value)->string_list_index != NONE)
		{
			if (name_text)
			{
				s_variant_setting_definition *definition = (s_variant_setting_definition *)datum->value;
				word name[0x100];

				name[0] = 0;
				function_1a0180(definition->string_list_index, definition->name_string_id, name);
				name_text->function_22f52e()->set_text(name);
			}
			if (value_text)
			{
				s_game_variant buffer;
				s_game_variant *variant = variant_settings_get_variant(alternate, &buffer);
				long value = function_2374f0(variant, *datum->value);
				s_variant_setting_definition *definition = (s_variant_setting_definition *)datum->value;
				s_variant_setting_option *option = (s_variant_setting_option *)function_236aa9((s_indexed_block *)definition, value);
				word text[0x100];

				text[0] = 0;
				if (option)
				{
					function_1a0180(definition->string_list_index, option->string_handle, text);
				}
				value_text->function_22f52e()->set_text(text);
			}
		}
	}
}

// @retail 0x2c7f1d
c_variant_parameter_setting_list::c_variant_parameter_setting_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_variant_parameter_setting_list::handle_item)
{
	setting_index = NONE;
	definition = 0;
	alternate = false;
}

// @retail 0x2c7f7f
void c_variant_parameter_setting_list::v17()
{
	definition = (s_variant_setting_definition *)function_2369b3(setting_index);
	if (definition && definition->option_count > 0)
	{
		data = user_interface_data_new("variant parameter setting list", definition->option_count, 8);
		function_16b790(data);
		for (long i = 0; i < data->maximum_count; i++)
		{
			s_variant_setting_option *option = &definition->options[i];

			((s_variant_setting_datum *)data->data)[record_pool_allocate(data) & 0xffff].option = option;
		}
		delegate_register(&item_handlers, &handler);
	}
	else
	{
		data = user_interface_data_new("EMPTY variant parameter setting list", 1, 8);
		function_16b790(data);
		((s_variant_setting_datum *)data->data)[record_pool_allocate(data) & 0xffff].option = 0;
	}
	((c_widget *)this)->c_widget::v1();
}

// @retail 0x2c803b
void c_variant_parameter_setting_list::v20(c_class_1a2c81 *item, long unused)
{
	long datum_index = widget_item(item)->value70;

	if (datum_index != NONE)
	{
		s_variant_setting_datum *datum = (s_variant_setting_datum *)record_pool_lookup(data, datum_index);
		c_class_1a2c81 *text = item->find_child(6, 0, false);

		if (definition && definition->string_list_index != NONE && datum && text && datum->option)
		{
			word buffer[0x100];

			buffer[0] = 0;
			function_1a0180(definition->string_list_index, datum->option->string_handle, buffer);
			text->function_22f52e()->set_text(buffer);
		}
	}
}

// @retail 0x2c80c6
void c_variant_parameter_setting_list::select_current_value()
{
	s_game_variant buffer;
	s_game_variant *variant = variant_settings_get_variant(alternate, &buffer);

	if (variant)
	{
		long value = function_2374f0(variant, setting_index);
		s_list_item_iterator iterator;

		iterator.iterator.index = NONE;
		iterator.iterator.datum_index = NONE;
		iterator.iterator.data = data;
		while (function_2b2327(&iterator))
		{
			s_variant_setting_option *option = ((s_variant_setting_datum *)iterator.item)->option;

			if (option && option->value == value)
			{
				select_datum(iterator.iterator.datum_index);
				break;
			}
		}
	}
}

/* 0x2c8159, the list's handler, is written below but left out: as a third
   caller of function_2c83e6 it moves that function's first two arguments
   into registers (retail keeps all four on the stack), which breaks it and
   function_2bb978. The placeholder keeps the handler's slot. */
void c_variant_parameter_setting_list::handle_item(s_controller_reference **controller, long *item)
{
	get_screen()->start_animation(3);
}

#if 0
/* opens the settings of the game engine (unknown_2bb295.cpp's
   0x2bb978 does the same) */
static __forceinline void open_game_engine_settings(long field_xcb8724, s_controller_reference **controller)
{
	long type;

	switch (field_xcb8724)
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

// @retail 0x2c8159
void c_variant_parameter_setting_list::handle_item(s_controller_reference **controller, long *item)
{
	s_variant_setting_datum *datum = (s_variant_setting_datum *)record_pool_lookup(data, *item);

	if (datum && datum->option)
	{
		s_game_variant buffer;
		s_game_variant *variant = variant_settings_get_variant(alternate, &buffer);

		if (variant)
		{
			function_2373be(setting_index, variant, datum->option->value);
			if (alternate)
			{
				function_19a728(variant);
				open_game_engine_settings(variant->field_xcb8724, controller);
			}
		}
	}
	get_screen()->start_animation(3);
}
#endif

// @retail 0x2c8252
c_variant_parameter_setting_screen::c_variant_parameter_setting_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0xdf, a, b, user_flags, &list),
	list(user_flags)
{
}

// @retail 0x2c8287
void c_variant_parameter_setting_screen::v19()
{
	word buffer[0x100];
	c_class_1a2c81 *title = find_child(6, 0, false);
	c_class_1a2c81 *description = find_child(6, 2, false);
	s_variant_setting_definition *definition = (s_variant_setting_definition *)function_2369b3(list.setting_index);

	if (definition && definition->string_list_index != NONE)
	{
		if (title)
		{
			buffer[0] = 0;
			function_1a0180(definition->string_list_index, definition->title_string_id, buffer);
			title->function_22f52e()->set_text(buffer);
		}
		if (description)
		{
			buffer[0] = 0;
			function_1a0180(definition->string_list_index, definition->description_string_id, buffer);
			description->function_22f52e()->set_text(buffer);
		}
	}
	list.select_current_value();
	c_class_1473c9::v19();
}

// @retail 0x2c8349
void function_2c8349(c_variant_parameter_setting_screen *screen, bool m6c, s_screen_parameters *parameters, bool alternate)
{
	screen->m6c = m6c;
	screen->list.alternate = alternate;
	screen->function_147f6d(parameters);
}

// @retail 0x2c8362
c_class_1473c9 *__stdcall function_2c8362(s_screen_parameters *parameters)
{
	c_variant_parameter_setting_screen *screen = new c_variant_parameter_setting_screen(parameters->a, parameters->b, parameters->user_flags);

	function_2c8349(screen, true, parameters, false);
	return screen;
}

// @retail 0x2c83a4
c_class_1473c9 *__stdcall function_2c83a4(s_screen_parameters *parameters)
{
	c_variant_parameter_setting_screen *screen = new c_variant_parameter_setting_screen(parameters->a, parameters->b, parameters->user_flags);

	function_2c8349(screen, true, parameters, true);
	return screen;
}

/* the deleting destructor is folded with c_variant_editing_screen's (0x2b778a) */

// @retail 0x2c7ab3
screen_load_proc c_variant_parameter_setting_screen::get_load_proc()
{
	return list.alternate ? function_2c83a4 : function_2c8362;
}

bool function_153850(byte *model);

/* the screen at 0x45d2b8 and the ones that derive from it (0x45d328,
   0x45d398, 0x45d408): a press of B or back copies its settings out */

/* a screen that edits the profile's settings, keeping a copy to restore */
class c_screen_45d2b8 : public c_screen_with_menu
{
public:
	c_screen_45d2b8(long screen_id, long a, long b, word user_flags, void *list);

	virtual bool v10(s_widget_event *event);

	bool changed;
	byte unknown615[3];
	dword settings[0x78];
};

// @retail 0x2c87be
c_screen_45d2b8::c_screen_45d2b8(long screen_id, long a, long b, word user_flags, void *list) :
	c_screen_with_menu(screen_id, a, b, user_flags, list),
	changed(true)
{
	memcpy(settings, &g_54e5d0.settings, sizeof(settings));
}

// @retail 0x2c87fe
bool c_screen_45d2b8::v10(s_widget_event *event)
{
	if (event->type == 5 && (event->param == 1 || event->param == 13) && changed)
	{
		memcpy(&g_54e5d0.settings, settings, sizeof(g_54e5d0.settings));
	}
	return c_class_1473c9::v10(event);
}

/* the emblem screen (vtable 0x45d328) */
class c_screen_45d328 : public c_screen_45d2b8
{
public:
	c_screen_45d328(long a, long b, word user_flags, long mode);

	/* keeps the focused emblem and shows it */
	virtual void v3();
	virtual bool v10(s_widget_event *event);
	/* focuses the emblem the profile has */
	virtual void v19();
	virtual screen_load_proc get_load_proc();

	c_choose_emblem_list list;
	long mode;
};

// @retail 0x2c8858
c_class_1473c9 *__stdcall function_2c8858(s_screen_parameters *parameters)
{
	c_screen_45d328 *screen = new c_screen_45d328(parameters->a, parameters->b, parameters->user_flags, 0);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2c8896
c_class_1473c9 *__stdcall function_2c8896(s_screen_parameters *parameters)
{
	c_screen_45d328 *screen = new c_screen_45d328(parameters->a, parameters->b, parameters->user_flags, 1);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

/* the screen id of each kind of emblem */
static long emblem_screen_id(long mode)
{
	long screen_id;

	switch (mode)
	{
	case 0:
		screen_id = 0x31;
		break;
	default:
		screen_id = 0xe9;
		break;
	}
	return screen_id;
}

// @retail 0x2c88d5
c_screen_45d328::c_screen_45d328(long a, long b, word user_flags, long mode) :
	c_screen_45d2b8(emblem_screen_id(mode), a, b, user_flags, &list),
	list(user_flags, mode),
	mode(mode)
{
}

// @retail 0x2cb1d4
bool c_screen_45d328::v10(s_widget_event *event)
{
	switch (event->type)
	{
	case 5:
		switch (event->param)
		{
		case 2:
			if (mode == 0)
			{
				g_54e5d0.settings.flag ^= true;
				return true;
			}
			break;
		}
		break;
	}
	return c_screen_45d2b8::v10(event);
}

// @retail 0x2c8920
screen_load_proc c_screen_45d328::get_load_proc()
{
	screen_load_proc result;

	switch (mode)
	{
	case 0:
		result = function_2c8858;
		break;
	case 1:
		result = function_2c8896;
		break;
	default:
		__assume(0);
	}
	return result;
}

// @retail 0x2c8936 deleting c_screen_45d328
// @retail 0x2c8a7a destructor c_screen_45d328

struct s_widget_view_2b0a;
void function_2b12ca(s_widget_view_2b0a *widget, short b, short a, void const *bounds);

// @retail 0x2cb174
void c_screen_45d328::v3()
{
	if (!ANIMATION_FLAG(animation, 1))
	{
		long datum = list.get_focused_datum();
		s_widget_view_2b0a *emblem;

		if (datum != NONE)
		{
			switch (mode)
			{
			case 0:
				g_54e5d0.settings.unknown11d[0] = (byte)datum;
				break;
			case 1:
				g_54e5d0.settings.unknown11d[1] = (byte)datum;
				break;
			default:
				__assume(0);
			}
		}
		emblem = (s_widget_view_2b0a *)find_child(9, 0, false);
		if (emblem)
		{
			function_2b12ca(emblem, NONE, NONE, g_54e5d0.settings.colors);
		}
	}
	c_class_1a2c81::v3();
}

// @retail 0x2cb144
void c_screen_45d328::v19()
{
	byte emblem;

	switch (mode)
	{
	case 0:
		emblem = g_54e5d0.settings.unknown11d[0];
		break;
	case 1:
		emblem = g_54e5d0.settings.unknown11d[1];
		break;
	default:
		__assume(0);
	}
	list.select_item(emblem);
	c_class_1473c9::v19();
}

// @retail 0x2c89db
screen_load_proc function_2c89db(long index)
{
	screen_load_proc result = 0;

	switch (index)
	{
	case 0:
		result = function_2c8998;
		break;
	case 1:
		result = function_2c89a8;
		break;
	case 2:
		result = function_2c89b9;
		break;
	case 3:
		result = function_2c89ca;
		break;
	}
	return result;
}

/* the player color screen (vtable 0x45d398): one screen for each of the
   four colors */
class c_screen_45d398 : public c_screen_45d2b8
{
public:
	c_screen_45d398(long a, long b, word user_flags);

	/* focuses the color the profile has */
	virtual void v19();
	virtual screen_load_proc get_load_proc();

	c_choose_player_color_list list;
	long index;
};

// @retail 0x2c8954
c_class_1473c9 *function_2c8954(s_screen_parameters *parameters, long index)
{
	c_screen_45d398 *screen = new c_screen_45d398(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->index = index;
	screen->list.value2a0 = index;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2c8998
c_class_1473c9 *__stdcall function_2c8998(s_screen_parameters *parameters)
{
	return function_2c8954(parameters, 0);
}

// @retail 0x2c89a8
c_class_1473c9 *__stdcall function_2c89a8(s_screen_parameters *parameters)
{
	return function_2c8954(parameters, 1);
}

// @retail 0x2c89b9
c_class_1473c9 *__stdcall function_2c89b9(s_screen_parameters *parameters)
{
	return function_2c8954(parameters, 2);
}

// @retail 0x2c89ca
c_class_1473c9 *__stdcall function_2c89ca(s_screen_parameters *parameters)
{
	return function_2c8954(parameters, 3);
}

// @retail 0x2c8a02
c_screen_45d398::c_screen_45d398(long a, long b, word user_flags) :
	c_screen_45d2b8(0x32, a, b, user_flags, &list),
	list(user_flags)
{
	list.value2a0 = 0;
}

// @retail 0x2c8a6f
screen_load_proc c_screen_45d398::get_load_proc()
{
	return function_2c89db(index);
}

// @retail 0x2c8a3d
void c_screen_45d398::v19()
{
	char color = (char)g_54e5d0.settings.colors[0];

	if (color >= -1 && color < 18)
	{
		list.select_item((char)g_54e5d0.settings.colors[index]);
	}
	c_class_1473c9::v19();
}

/* the model screen (vtable 0x45d408) */
class c_screen_45d408 : public c_screen_45d2b8
{
public:
	c_screen_45d408(long a, long b, word user_flags);

	/* keeps the focused model */
	virtual void v3();
	/* focuses the model the profile has */
	virtual void v19();
	virtual screen_load_proc get_load_proc();

	c_choose_model_list list;
};

// @retail 0x2c8a8f
c_class_1473c9 *__stdcall function_2c8a8f(s_screen_parameters *parameters)
{
	c_screen_45d408 *screen = new c_screen_45d408(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2c8acb
c_screen_45d408::c_screen_45d408(long a, long b, word user_flags) :
	c_screen_45d2b8(0x34, a, b, user_flags, &list),
	list(user_flags)
{
}

// @retail 0x2c8aff
screen_load_proc c_screen_45d408::get_load_proc()
{
	return function_2c8a8f;
}

// @retail 0x2c8b32 deleting c_screen_45d408
// @retail 0x2c8b50 destructor c_screen_45d408

// @retail 0x2cb4e2
void c_screen_45d408::v3()
{
	if (!ANIMATION_FLAG(animation, 1))
	{
		g_54e5d0.settings.model = (byte)list.get_focused_datum();
	}
	c_class_1a2c81::v3();
}

// @retail 0x2c8b05
void c_screen_45d408::v19()
{
	if (function_153850(&g_54e5d0.settings.model))
	{
		list.select_item((char)g_54e5d0.settings.model);
	}
	c_class_1473c9::v19();
}

/* ---- the settings menus ---- */

/* a player's profile (unknown_18f576.cpp) */
struct s_player_profile
{
	dword data[0x78];
};

void player_slot_get_profile(long index, s_player_profile *profile, long *profile_index);

/* the controller settings screens (unknown_2b116a.cpp) */
c_class_1473c9 *__stdcall function_2b4274(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b4397(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b4485(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b4565(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b46a6(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b47a7(s_screen_parameters *parameters);

/* loads a screen for these controllers (not decompiled yet) */
c_class_1473c9 *function_149ef3(word user_flags, long load); /* unknown_1490ec.cpp */

// @retail 0x2c5b35
c_xbox_live_appear_offline_list::c_xbox_live_appear_offline_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_xbox_live_appear_offline_list::handle_item)
{
	data = user_interface_data_new("xbox live appear offline list", 2, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2c5bc5
void c_xbox_live_appear_offline_list::v1()
{
	((c_widget *)this)->c_widget::v9();
	if (g_54e5d0.profile_index == NONE)
	{
		s_player_profile profile;
		long profile_index;

		player_slot_get_profile(get_controller_index(), &profile, &profile_index);
		select_item(((s_player_profile_settings *)&profile)->unknown150 == 0);
	}
	else
	{
		select_item(g_54e5d0.settings.unknown150 == 0);
	}
}

// @retail 0x2c5c23
void c_xbox_live_appear_offline_list::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);

	if (text)
	{
		long string_handle;

		switch (((short)widget_item(widget)->value70))
		{
		case 0:
			string_handle = 0x7000183;
			break;
		case 1:
			string_handle = 0x8000184;
			break;
		default:
			string_handle = NONE;
			break;
		}
		text->function_253b1a(string_handle);
	}
}

// @retail 0x2c5c63
void c_xbox_live_appear_offline_list::handle_item(s_controller_reference **controller, long *item)
{
	short index = *(short *)item;
	long controller_index = (*controller)->controller_index;
	c_xbox_live_appear_offline_list *list = this;
	bool end_edit = true;
	bool begin_edit = true;
	s_player_profile profile;
	long profile_index;

	player_slot_get_profile(controller_index, &profile, &profile_index);
	if (g_54e5d0.profile_index == NONE)
	{
		profile_edit_begin(controller_index, (s_player_profile_settings *)&profile, profile_index);
		begin_edit = false;
	}
	else if (g_54e5d0.profile_index != profile_index)
	{
		end_edit = false;
	}
	switch (index)
	{
	case 0:
		g_54e5d0.settings.unknown150 = true;
		break;
	case 1:
		g_54e5d0.settings.unknown150 = false;
		break;
	}
	if (end_edit)
	{
		profile_edit_end();
		if (begin_edit)
		{
			controller_index = (*controller)->controller_index;
			player_slot_get_profile(controller_index, &profile, &profile_index);
			profile_edit_begin(controller_index, (s_player_profile_settings *)&profile, profile_index);
		}
	}
	function_14800c(list->v11(), list->v12());
}

// @retail 0x230573 deleting c_xbox_live_appear_offline_list
// @retail 0x1474b2 destructor c_xbox_live_appear_offline_list

// @retail 0x2c84b8
c_controller_settings_edit_list::c_controller_settings_edit_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_controller_settings_edit_list::handle_item)
{
	data = user_interface_data_new("controller settings edit list", 6, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2c8719
void c_controller_settings_edit_list::handle_item(s_controller_reference **controller, long *item)
{
	if (record_pool_lookup(data, *item))
	{
		short index = *(short *)item;
		s_screen_parameters parameters;

		parameters.field_c = 0;
		function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 5, 4, 0);
		switch (index)
		{
		case 0:
			parameters.load = function_2b4274;
			break;
		case 1:
			parameters.load = function_2b4565;
			break;
		case 2:
			parameters.a = 3;
			parameters.load = function_2b4397;
			break;
		case 3:
			parameters.a = 3;
			parameters.load = function_2b4485;
			break;
		case 4:
			parameters.a = 3;
			parameters.load = function_2b46a6;
			break;
		default:
			parameters.a = 3;
			parameters.load = function_2b47a7;
			break;
		}
		if (parameters.load)
		{
			parameters.load(&parameters);
		}
	}
}

// @retail 0x2c8b65
c_multiplayer_settings_edit_list::c_multiplayer_settings_edit_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_multiplayer_settings_edit_list::handle_item)
{
	data = user_interface_data_new("multiplayer settings edit list", 7, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2c8cfc
void c_multiplayer_settings_edit_list::handle_item(s_controller_reference **controller, long *item)
{
	if (record_pool_lookup(data, *item))
	{
		switch (*(short *)item)
		{
		case 0:
			function_149ef3(1 << (*controller)->controller_index, (long)function_2c8a8f);
			break;
		case 1:
			function_149ef3(1 << (*controller)->controller_index, (long)function_2c8998);
			break;
		case 2:
			function_149ef3(1 << (*controller)->controller_index, (long)function_2c89a8);
			break;
		case 3:
			function_149ef3(1 << (*controller)->controller_index, (long)function_2c89b9);
			break;
		case 4:
			function_149ef3(1 << (*controller)->controller_index, (long)function_2c89ca);
			break;
		case 5:
			function_149ef3(1 << (*controller)->controller_index, (long)function_2c8858);
			break;
		default:
			function_149ef3(1 << (*controller)->controller_index, (long)function_2c8896);
			break;
		}
	}
}

// @retail 0x2bb731 deleting c_multiplayer_settings_edit_list
// @retail 0x2b783c destructor c_multiplayer_settings_edit_list

/* ---- the settings edit lists: each item sets one of the edited profile's
   settings, then the list's window goes back ---- */

void function_53810(long voice_mask, long controller_index);
void function_54fc0(long controller_index, long voice_through_tv);
bool function_153850(byte *model);

// @retail 0x2c5dca deleting c_voice_mask_list

// @retail 0x2c5d34
c_voice_mask_list::c_voice_mask_list(word user_flags) :
	c_class_1474e8(user_flags),
	value188(false),
	handler(this, (list_item_method)&c_voice_mask_list::handle_item)
{
	data = user_interface_data_new("voice mask list", 2, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2c5de8
void c_voice_mask_list::v1()
{
	((c_widget *)this)->c_widget::v9();
	select_item((short)g_54e5d0.settings.voice_mask);
}

// @retail 0x2c5e02
void c_voice_mask_list::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);

	if (text)
	{
		long string_handle;

		switch (((short)widget_item(widget)->value70))
		{
		case 0:
			string_handle = 0x400021c;
			break;
		case 1:
			string_handle = 0x9000303;
			break;
		default:
			string_handle = NONE;
			break;
		}
		text->function_253b1a(string_handle);
	}
}

// @retail 0x2c5e42
void c_voice_mask_list::handle_item(s_controller_reference **controller, long *item)
{
	switch (*(short *)item)
	{
	case 0:
		g_54e5d0.settings.voice_mask = 0;
		break;
	case 1:
		g_54e5d0.settings.voice_mask = 1;
		break;
	}
	if (value188)
	{
		profile_edit_save();
	}
	function_53810(g_54e5d0.settings.voice_mask, (*controller)->controller_index);
	function_14800c(v11(), v12());
}

// @retail 0x2b8cb1 deleting c_voice_through_tv_list

// @retail 0x2c5ea7
c_voice_through_tv_list::c_voice_through_tv_list(word user_flags) :
	c_class_1474e8(user_flags),
	value288(false),
	handler(this, (list_item_method)&c_voice_through_tv_list::handle_item)
{
	data = user_interface_data_new("voice through tv list", 4, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2c5f3c
void c_voice_through_tv_list::v1()
{
	((c_widget *)this)->c_widget::v9();
	select_item((short)g_54e5d0.settings.voice_through_tv);
}

// @retail 0x2c5f56
void c_voice_through_tv_list::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);

	if (text)
	{
		long string_handle;

		switch (((short)widget_item(widget)->value70))
		{
		case 0:
			string_handle = 0xd000304;
			break;
		case 1:
			string_handle = 0x25000305;
			break;
		case 2:
			string_handle = 0x21000306;
			break;
		case 3:
			string_handle = 0xe000307;
			break;
		default:
			string_handle = NONE;
			break;
		}
		text->function_253b1a(string_handle);
	}
}

// @retail 0x2c5fab
void c_voice_through_tv_list::handle_item(s_controller_reference **controller, long *item)
{
	switch (*(short *)item)
	{
	case 0:
		g_54e5d0.settings.voice_through_tv = 0;
		break;
	case 1:
		g_54e5d0.settings.voice_through_tv = 1;
		break;
	case 2:
		g_54e5d0.settings.voice_through_tv = 2;
		break;
	case 3:
		g_54e5d0.settings.voice_through_tv = 3;
		break;
	}
	if (value288)
	{
		profile_edit_save();
	}
	function_54fc0((*controller)->controller_index, g_54e5d0.settings.voice_through_tv);
	function_14800c(v11(), v12());
}

// @retail 0x2c602e
c_thumbstick_settings_edit_list::c_thumbstick_settings_edit_list(word user_flags) :
	c_class_1474e8(user_flags),
	value288(false),
	handler(this, (list_item_method)&c_thumbstick_settings_edit_list::handle_item)
{
	data = user_interface_data_new("thumbstick settings edit list", 4, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2c60c3
void c_thumbstick_settings_edit_list::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);

	if (text)
	{
		long string_handle;

		switch (((short)widget_item(widget)->value70))
		{
		case 0:
			string_handle = 0x7000001;
			break;
		case 1:
			string_handle = 0x80002f5;
			break;
		case 2:
			string_handle = 0x60002f6;
			break;
		case 3:
			string_handle = 0xf0002f7;
			break;
		default:
			string_handle = NONE;
			break;
		}
		text->function_253b1a(string_handle);
	}
}

// @retail 0x2c6118
void c_thumbstick_settings_edit_list::handle_item(s_controller_reference **controller, long *item)
{
	byte layout;

	switch (*(short *)item)
	{
	case 0:
		layout = 0;
		break;
	case 1:
		layout = 1;
		break;
	case 2:
		layout = 2;
		break;
	case 3:
		layout = 3;
		break;
	default:
		layout = 0;
		break;
	}
	g_54e5d0.settings.thumbstick_layout = layout;
	if (value288)
	{
		profile_edit_save();
	}
	function_14800c(v11(), v12());
}

// @retail 0x2c61ce
c_look_sensitivity_settings_edit_list::c_look_sensitivity_settings_edit_list(word user_flags) :
	c_class_1474e8(user_flags),
	value288(false),
	handler(this, (list_item_method)&c_look_sensitivity_settings_edit_list::handle_item)
{
	data = user_interface_data_new("look sensitivity settings edit list", 10, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2c6264
void c_look_sensitivity_settings_edit_list::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);

	if (text)
	{
		long string_handle;

		switch ((short)(widget_item(widget)->value70 + 1))
		{
		case 1:
			string_handle = 0x120003cf;
			break;
		case 2:
			string_handle = 0x120003d0;
			break;
		case 3:
			string_handle = 0x120003d1;
			break;
		case 4:
			string_handle = 0x120003d2;
			break;
		case 5:
			string_handle = 0x120003d3;
			break;
		case 6:
			string_handle = 0x120003d4;
			break;
		case 7:
			string_handle = 0x120003d5;
			break;
		case 8:
			string_handle = 0x120003d6;
			break;
		case 9:
			string_handle = 0x120003d7;
			break;
		case 10:
			string_handle = 0x130003d8;
			break;
		default:
			string_handle = 0;
			break;
		}
		text->function_253b1a(string_handle);
	}
}

// @retail 0x2c630e
void c_look_sensitivity_settings_edit_list::handle_item(s_controller_reference **controller, long *item)
{
	long sensitivity = *(short *)item + 1;
	byte value;

	if (sensitivity < 1)
	{
		value = 1;
	}
	else if (sensitivity > 10)
	{
		value = 10;
	}
	else
	{
		value = (byte)sensitivity;
	}
	g_54e5d0.settings.look_sensitivity = value;
	if (value288)
	{
		profile_edit_save();
	}
	function_14800c(v11(), v12());
}

// @retail 0x2c635b
c_invert_look_settings_edit_list::c_invert_look_settings_edit_list(word user_flags) :
	c_class_1474e8(user_flags),
	value188(false),
	handler(this, (list_item_method)&c_invert_look_settings_edit_list::handle_item)
{
	data = user_interface_data_new("invert look settings edit list", 2, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2c63f1
void c_invert_look_settings_edit_list::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);

	if (text)
	{
		long string_handle;

		switch (((short)widget_item(widget)->value70))
		{
		case 0:
			string_handle = 0x6000308;
			break;
		case 1:
			string_handle = 0x7000309;
			break;
		default:
			string_handle = NONE;
			break;
		}
		text->function_253b1a(string_handle);
	}
}

// @retail 0x2c6431
void c_invert_look_settings_edit_list::handle_item(s_controller_reference **controller, long *item)
{
	switch (*(short *)item)
	{
	case 1:
		g_54e5d0.settings.controller_flags.invert_look = false;
		break;
	case 0:
		g_54e5d0.settings.controller_flags.invert_look = true;
		break;
	default:
		__assume(0);
	}
	if (value188)
	{
		profile_edit_save();
	}
	function_14800c(v11(), v12());
}

// @retail 0x2c647d
c_button_settings_edit_list::c_button_settings_edit_list(word user_flags) :
	c_class_1474e8(user_flags),
	value288(false),
	handler(this, (list_item_method)&c_button_settings_edit_list::handle_item)
{
	data = user_interface_data_new("button settings edit list", 4, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2c6512
void c_button_settings_edit_list::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);

	if (text)
	{
		long string_handle;

		switch (((short)widget_item(widget)->value70))
		{
		case 0:
			string_handle = 0x7000001;
			break;
		case 1:
			string_handle = 0x90003e4;
			break;
		case 2:
			string_handle = 0x50003e5;
			break;
		case 3:
			string_handle = 0xb0003e6;
			break;
		default:
			string_handle = NONE;
			break;
		}
		text->function_253b1a(string_handle);
	}
}

// @retail 0x2c6567
void c_button_settings_edit_list::handle_item(s_controller_reference **controller, long *item)
{
	byte layout;

	switch (*(short *)item)
	{
	case 0:
		layout = 0;
		break;
	case 1:
		layout = 1;
		break;
	case 2:
		layout = 2;
		break;
	case 3:
		layout = 3;
		break;
	default:
		layout = 0;
		break;
	}
	g_54e5d0.settings.button_layout = layout;
	if (value288)
	{
		profile_edit_save();
	}
	function_14800c(v11(), v12());
}

// @retail 0x2c661f
c_auto_level_settings_edit_list::c_auto_level_settings_edit_list(word user_flags) :
	c_class_1474e8(user_flags),
	value188(false),
	handler(this, (list_item_method)&c_auto_level_settings_edit_list::handle_item)
{
	data = user_interface_data_new("auto level settings edit list", 2, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2c66b5
void c_auto_level_settings_edit_list::handle_item(s_controller_reference **controller, long *item)
{
	switch (*(short *)item)
	{
	case 1:
		g_54e5d0.settings.controller_flags.auto_level = false;
		break;
	case 0:
		g_54e5d0.settings.controller_flags.auto_level = true;
		break;
	default:
		__assume(0);
	}
	if (value188)
	{
		profile_edit_save();
	}
	function_14800c(v11(), v12());
}

// @retail 0x2c6701
c_vibration_settings_edit_list::c_vibration_settings_edit_list(word user_flags) :
	c_class_1474e8(user_flags),
	value188(false),
	handler(this, (list_item_method)&c_vibration_settings_edit_list::handle_item)
{
	data = user_interface_data_new("vibration settings edit list", 2, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2c6797
void c_vibration_settings_edit_list::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);

	if (text)
	{
		long string_handle;

		switch (((short)widget_item(widget)->value70))
		{
		case 0:
			string_handle = 0x2000181;
			break;
		case 1:
			string_handle = 0x3000182;
			break;
		default:
			string_handle = NONE;
			break;
		}
		text->function_253b1a(string_handle);
	}
}

// @retail 0x2c67d7
void c_vibration_settings_edit_list::handle_item(s_controller_reference **controller, long *item)
{
	switch (*(short *)item)
	{
	case 0:
		g_54e5d0.settings.controller_flags.vibration = false;
		break;
	case 1:
		g_54e5d0.settings.controller_flags.vibration = true;
		break;
	default:
		g_54e5d0.settings.controller_flags.vibration = true;
		break;
	}
	if (value188)
	{
		profile_edit_save();
	}
	function_14800c(v11(), v12());
}

// @retail 0x2b52b7 deleting c_subtitle_setting_list

// @retail 0x2c8ded
c_subtitle_setting_list::c_subtitle_setting_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_subtitle_setting_list::handle_item)
{
	data = user_interface_data_new("subtitle setting list", 3, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2c8e7d
void c_subtitle_setting_list::v1()
{
	short item;

	((c_widget *)this)->c_widget::v9();
	item = 0;
	switch (g_54e5d0.settings.subtitles)
	{
	case 0:
		item = 0;
		break;
	case 1:
		item = 1;
		break;
	case 2:
		item = 2;
		break;
	}
	select_item(item);
}

// @retail 0x2c8eaa
void c_subtitle_setting_list::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);

	if (text)
	{
		long string_handle;

		switch (((short)widget_item(widget)->value70))
		{
		case 0:
			string_handle = 0x7000001;
			break;
		case 1:
			string_handle = 0x2000181;
			break;
		case 2:
			string_handle = 0x3000182;
			break;
		default:
			string_handle = NONE;
			break;
		}
		text->function_253b1a(string_handle);
	}
}

// @retail 0x2c8ef5
void c_subtitle_setting_list::handle_item(s_controller_reference **controller, long *item)
{
	byte subtitles = 0;

	switch (*(short *)item)
	{
	case 0:
		subtitles = 0;
		break;
	case 1:
		subtitles = 1;
		break;
	case 2:
		subtitles = 2;
		break;
	}
	g_54e5d0.settings.subtitles = subtitles;
	function_14800c(v11(), v12());
}

// @retail 0x2b4bd2 deleting c_choose_player_color_list

bool function_0bfe60(const dword *flags, long bit);

// @retail 0x2cb018
c_choose_emblem_list::c_choose_emblem_list(word user_flags, long mode) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_choose_emblem_list::handle_item),
	mode(mode)
{
	dword available[2];

	data = user_interface_data_new("choose emblem list", 64, 4);
	function_16b790(data);
	function_2cb200(available, this->mode);
	for (long i = 0; i < data->maximum_count; i++)
	{
		if (function_0bfe60(available, i))
		{
			function_16b990(data, i);
		}
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2cb102
void c_choose_emblem_list::handle_item(s_controller_reference **controller, long *item)
{
	short emblem = *(short *)item;

	switch (mode)
	{
	case 0:
		g_54e5d0.settings.unknown11d[0] = (byte)emblem;
		break;
	case 1:
		g_54e5d0.settings.unknown11d[1] = (byte)emblem;
		break;
	default:
		__assume(0);
	}
	function_14800c(v11(), v12());
}

/* ---- the custom game maps list ---- */

s_record_pool *function_19c6a0();
long function_19c4e0(long key0);
long function_190565();
struct s_entry_c;
s_entry_c *function_19c5f0(long key);
struct s_localized_short_name;
wchar_t *localized_short_name_get(s_localized_short_name *definition);
void function_23625d(long tag_index);
bool function_19a6f2(long campaign_id, long map_id);
bool data_datum_iterator_next(s_data_datum_iterator *iterator);

/* the multiplayer maps' table (unknown_19c1d0.cpp): a map's id and whether
   it is downloaded content */
struct s_map_entry
{
	short salt;
	bool downloaded;
	byte unknown03;
	long map_id;
};

/* a map's definition: its id and bitmap */
struct s_map_definition
{
	long map_id;
	byte unknown04[4];
	long bitmap_tag_index;
};

/* the map chosen last */
long g_51098c;

// @retail 0x2c9928
c_custom_game_maps_list::c_custom_game_maps_list(word user_flags) :
	c_class_1474e8(user_flags),
	coop(false),
	handler(this, (list_item_method)&c_custom_game_maps_list::handle_item)
{
	s_record_pool *maps = function_19c6a0();
	bool all_maps;
	s_list_item_iterator iterator;

	data = user_interface_data_new("custom game maps", maps->high_water_index, 8);
	function_16b790(data);
	all_maps = false;
	if (function_190565() >= function_19c4e0(1))
	{
		all_maps = true;
	}
	iterator.iterator.index = NONE;
	iterator.iterator.datum_index = NONE;
	iterator.iterator.data = maps;
	while (function_2b2327(&iterator))
	{
		s_map_entry *entry = (s_map_entry *)iterator.item;

		if (!entry->downloaded || all_maps)
		{
			s_map_entry *item = &((s_map_entry *)data->data)[record_pool_allocate(data) & 0xffff];
			s_map_definition *map = (s_map_definition *)function_19c5f0(entry->map_id);

			item->downloaded = entry->downloaded;
			item->map_id = entry->map_id;
			if (map && map->bitmap_tag_index != NONE)
			{
				function_23625d(map->bitmap_tag_index);
			}
		}
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2c9a3a deleting c_custom_game_maps_list
// @retail 0x2bb3a2 destructor c_custom_game_maps_list

/* selects the map chosen last (v1 reaches it by a tail jump) */
// @retail 0x2c9a73
void c_custom_game_maps_list::select_last_map()
{
	long map_id = g_51098c;

	if (map_id != NONE)
	{
		s_data_datum_iterator iterator;

		iterator.index = NONE;
		iterator.datum_index = NONE;
		iterator.data = data;
		while (data_datum_iterator_next(&iterator))
		{
			if (((s_map_definition *)function_19c5f0(((s_map_entry *)iterator.datum)->map_id))->map_id == map_id)
			{
				select_datum(iterator.datum_index);
				break;
			}
		}
	}
}

// @retail 0x2c9a58
void c_custom_game_maps_list::v1()
{
	((c_widget *)this)->c_widget::v9();
	if (!coop)
	{
		select_last_map();
	}
}

/* shows the map's name */
// @retail 0x2c9ac4
void c_custom_game_maps_list::v20(c_class_1a2c81 *widget, long index)
{
	long datum = widget_item(widget)->value70;

	if (datum != NONE)
	{
		s_entry_c *map = function_19c5f0(((s_map_entry *)data->data)[datum & 0xffff].map_id);
		c_class_1a2c81 *text = widget->find_child(6, 0, false);

		if (text)
		{
			wchar_t *name = localized_short_name_get((s_localized_short_name *)map);

			text->function_22f52e()->set_text(name);
		}
	}
}

// @retail 0x2c9b17
void c_custom_game_maps_list::handle_item(s_controller_reference **controller, long *item)
{
	if (*item != NONE)
	{
		s_map_entry *map = &((s_map_entry *)data->data)[*item & 0xffff];

		if (coop)
		{
			function_148044(v11(), v12(), 0xb4);
		}
		else
		{
			function_19a6f2(NONE, map->map_id);
			get_screen()->start_animation(3);
		}
	}
}

// @retail 0x2c9c12
c_difficulty_list::c_difficulty_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_difficulty_list::handle_item),
	alternate(false),
	value2a1(false)
{
	data = user_interface_data_new("difficulty list", 4, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
}

void function_121100(long *value);
void function_1210a0(long *value);
void function_24c1c5(c_widget *widget, char direction);

/* focuses the difficulty last played */
// @retail 0x2c9cad
void c_difficulty_list::v1()
{
	long difficulty;
	long i;

	((c_widget *)this)->c_widget::v9();
	if (alternate)
	{
		function_121100(&difficulty);
	}
	else
	{
		function_1210a0(&difficulty);
	}
	for (i = difficulty; i > 0; i--)
	{
		function_24c1c5((c_widget *)this, 1);
	}
}

// @retail 0x2c9ce7
void c_difficulty_list::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);
	long string_handle;

	switch (((short)widget_item(widget)->value70))
	{
	case 0:
		string_handle = 0x400028e;
		break;
	case 1:
		string_handle = 0x60000b8;
		break;
	case 2:
		string_handle = 0x600028f;
		break;
	case 3:
		string_handle = 0x9000290;
		break;
	default:
		string_handle = NONE;
		break;
	}
	text->function_253b1a(string_handle);
}

void function_148d42(long value);
bool function_19a76d(short index);
void function_1210c0(long value);
void function_121060(long value);
c_class_1473c9 *__stdcall function_2b1467(s_screen_parameters *parameters);
extern bool g_54e7cc;
bool function_592f0(void);

/* the screen the level select loads (its +0x610) */
struct s_screen_view_610
{
	byte unknown000[0x610];
	long value610;
};

/* the campaign's difficulty: starts the level, or goes on to choose it */
// @retail 0x2c9d38
void c_difficulty_list::handle_item(s_controller_reference **controller, long *item)
{
	long index = *item & 0xffff;
	long difficulty = index < 0 ? 0 : (index > 3 ? 3 : index);

	function_148d42(difficulty);
	g_54e7cc = false;
	if (function_592f0())
	{
		function_19a76d((short)difficulty);
	}
	if (alternate)
	{
		function_1210c0(difficulty);
		get_screen()->start_animation(3);
	}
	else
	{
		s_screen_parameters parameters;
		s_screen_view_610 *screen;

		function_121060(difficulty);
		parameters.field_c = 0;
		function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 5, 4, (long)function_2b1467);
		screen = (s_screen_view_610 *)parameters.load(&parameters);
		screen->value610 = 0;
	}
}

// @retail 0x2cb23f
c_choose_player_color_list::c_choose_player_color_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_choose_player_color_list::handle_item)
{
	value2a0 = 0;
	data = user_interface_data_new("choose player color list", 18, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2cb30a
void c_choose_player_color_list::handle_item(s_controller_reference **controller, long *item)
{
	short color = *(short *)item;
	long value = 0;

	if (color >= -1 && color < 18)
	{
		value = color;
	}
	g_54e5d0.settings.colors[value2a0] = (byte)value;
	function_14800c(v11(), v12());
}

// @retail 0x2cb3e0
c_choose_model_list::c_choose_model_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_choose_model_list::handle_item)
{
	data = user_interface_data_new("choose model list", 2, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2cb4a2
void c_choose_model_list::handle_item(s_controller_reference **controller, long *item)
{
	g_54e5d0.settings.model = *(byte *)item;
	if (!function_153850(&g_54e5d0.settings.model))
	{
		g_54e5d0.settings.model = 0;
	}
	function_14800c(v11(), v12());
}

/* ---- the campaign level select screen and its list ---- */

struct s_entry_b;
struct s_entry_a;
s_entry_b *function_19c1f0(long key);
s_record_pool *function_19c670();
s_entry_a *function_19c270(long key0, long key1);
long function_1910d9(void);
long player_slot_get_single_profile_index(void);
bool function_124770(long profile_index);
bool function_592f0(void);
void function_19040d(long value);
bool __stdcall function_163890(char const *field_8_5, long a);
c_class_1473c9 *__stdcall function_2bb3ed(s_screen_parameters *parameters);
struct s_localized_name;
struct s_localized_description;
wchar_t *localized_name_get(s_localized_name *definition);
wchar_t *localized_description_get(s_localized_description *definition);
struct s_type_7ba8e9;
s_type_7ba8e9 *function_137550(long group_index, short bitmap_index);
struct s_widget_view_2b0a;
void function_2b0a7b(s_widget_view_2b0a *widget, s_type_7ba8e9 *bitmap);

extern long g_54e7c0;
extern long g_54e7c4;

/* a campaign level's definition: its map id, its picture and its scenario */
struct s_campaign_level
{
	long campaign_id;
	long map_id;
	char field_8_5[4];
	long bitmap_tag_index;
};

// @retail 0x2c4ea0
c_campaign_level_handles_list::c_campaign_level_handles_list(word user_flags, bool alternate) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_campaign_level_handles_list::handle_item)
{
	this->alternate = alternate;
	unlocked = false;

	s_record_pool *levels = function_19c670();
	long last_level = function_190565();
	s_data_datum_iterator iterator;

	if (alternate && last_level <= 0x68)
	{
		last_level = 0x68;
	}
	g_54e7c0 = 1;
	data = user_interface_data_new("campaign level handles", levels->high_water_index, 8);
	function_16b790(data);
	iterator.index = NONE;
	iterator.datum_index = NONE;
	iterator.data = levels;
	while (data_datum_iterator_next(&iterator))
	{
		s_map_entry *level = (s_map_entry *)iterator.datum;

		if (!this->alternate || level->map_id >= 0x69)
		{
			s_map_entry *item = &((s_map_entry *)data->data)[record_pool_allocate(data) & 0xffff];
			s_campaign_level *definition = (s_campaign_level *)function_19c1f0(level->map_id);

			item->downloaded = level->downloaded;
			item->map_id = level->map_id;
			last_map_id = level->map_id;
			if (definition && definition->bitmap_tag_index != NONE)
			{
				function_23625d(definition->bitmap_tag_index);
			}
			if (level->map_id > last_level)
			{
				break;
			}
		}
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2c4fd7 deleting c_campaign_level_handles_list

// @retail 0x2c4ff5
void c_campaign_level_handles_list::v1()
{
	long map_id = function_1910d9();

	((c_widget *)this)->c_widget::v9();
	if (map_id <= last_map_id)
	{
		s_data_datum_iterator iterator;

		iterator.index = NONE;
		iterator.datum_index = NONE;
		iterator.data = data;
		while (data_datum_iterator_next(&iterator))
		{
			if (map_id == ((s_map_entry *)iterator.datum)->map_id)
			{
				select_datum(iterator.datum_index);
				break;
			}
		}
	}
}

/* shows the level's name */
// @retail 0x2c5049
void c_campaign_level_handles_list::v20(c_class_1a2c81 *widget, long index)
{
	long datum = widget_item(widget)->value70;

	if (datum != NONE)
	{
		s_entry_b *level = function_19c1f0(((s_map_entry *)data->data)[datum & 0xffff].map_id);

		if (level)
		{
			c_class_1a2c81 *text = widget->find_child(6, 0, false);
			wchar_t *name = localized_name_get((s_localized_name *)level);

			text->function_22f52e()->set_text((word *)name);
		}
	}
}

/* plays the level */
// @retail 0x2c509c
void c_campaign_level_handles_list::handle_item(s_controller_reference **controller, long *item)
{
	if (*item != NONE)
	{
		long profile_index = player_slot_get_single_profile_index();

		if (profile_index != NONE)
		{
			if (function_124770(profile_index))
			{
				unlocked = true;
			}
			else
			{
				dialog_ok_show(1, 0x80, 4, 1 << (*controller)->controller_index, 0, 0);
			}
		}
		if (unlocked || alternate)
		{
			s_campaign_level *level = (s_campaign_level *)function_19c270(1, ((s_map_entry *)data->data)[*item & 0xffff].map_id);

			g_54e7c4 = level->map_id;
			if (function_592f0())
			{
				function_19a6f2(1, level->map_id);
			}
			function_19040d(level->map_id);
			function_163890(level->field_8_5, 0);
			if (alternate)
			{
				get_screen()->start_animation(3);
			}
			else
			{
				s_screen_parameters parameters;

				parameters.field_c = 0;
				function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 5, 4, (long)function_2bb3ed);
				parameters.load(&parameters);
			}
		}
	}
}

// @retail 0x2c51a2
void c_level_select_screen::v3()
{
	s_map_entry *datum = (s_map_entry *)record_pool_lookup(list.data, list.get_focused_datum());

	if (datum)
	{
		s_campaign_level *level = (s_campaign_level *)function_19c1f0(datum->map_id);

		if (level)
		{
			c_class_1a2c81 *bitmap = find_child(8, 0, false);
			c_class_1a2c81 *text = find_child(6, 2, false);

			if (bitmap)
			{
				function_2b0a7b((s_widget_view_2b0a *)bitmap, function_137550(level->bitmap_tag_index, 0));
			}
			text->function_22f52e()->set_text((word *)localized_description_get((s_localized_description *)level));
		}
	}
	c_class_1a2c81::v3();
}

/* ---- the game engine variant category list ---- */

long __stdcall function_120e70(byte *buffer);
bool __stdcall function_215f40(long arg_9db745, byte *buffer);
long function_212380(long arg_9db745, long controller_index, byte *buffer);
bool function_212bc0(long file_index, s_game_variant *variant);
void function_148aa3(long error, dword controller_flags);
void function_238c21(long controller, long type, word *name, long maximum_count);
c_class_1473c9 *__stdcall function_2ca4cd(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca525(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca580(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca5d8(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca630(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca68b(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca6e3(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca73b(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca796(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca7ee(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca846(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca8a1(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca8f9(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca951(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ca9ac(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2caa04(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2caa5c(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2caab7(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2cab0f(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2cab67(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2cabc2(s_screen_parameters *parameters);

/* the error of the last saved game file operation */
long g_55c154;

// @retail 0x2c55f8
c_game_engine_variant_category_list::c_game_engine_variant_category_list(word user_flags) :
	c_class_1474e8(user_flags),
	edit_settings(false),
	create(false),
	edit_alternate(false),
	handler(this, (list_item_method)&c_game_engine_variant_category_list::handle_item)
{
	data = user_interface_data_new("game engine variant category list", 9, 4);
	function_16b790(data);
	list_item_add(this, 0);
	list_item_add(this, 1);
	list_item_add(this, 3);
	if (!g_54d598.value08)
	{
		list_item_add(this, 4);
	}
	list_item_add(this, 6);
	list_item_add(this, 7);
	if (!g_54d598.value08)
	{
		list_item_add(this, 8);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2c573b deleting c_game_engine_variant_category_list

// @retail 0x2c5759
void c_game_engine_variant_category_list::v1()
{
	((c_widget *)this)->c_widget::v9();
	if (!edit_settings && !create && !edit_alternate)
	{
		select_variant_engine();
	}
}

/* whether an item stands for this game engine */
static __forceinline bool variant_category_is_game_engine(short item, long arg_9db745)
{
	bool result;

	switch (item)
	{
	case 0:
		result = arg_9db745 == 2;
		break;
	case 1:
		result = arg_9db745 == 4;
		break;
	case 3:
		result = arg_9db745 == 3;
		break;
	case 4:
		result = arg_9db745 == 7;
		break;
	case 6:
		result = arg_9db745 == 1;
		break;
	case 7:
		result = arg_9db745 == 9;
		break;
	default:
		result = arg_9db745 == 8;
		break;
	}
	return result;
}

// @retail 0x2c5782
void c_game_engine_variant_category_list::select_variant_engine()
{
	s_game_variant variant;

	if (function_120e70((byte *)&variant) != NONE)
	{
		s_list_item_iterator iterator;

		iterator.iterator.index = NONE;
		iterator.iterator.datum_index = NONE;
		iterator.iterator.data = data;
		while (function_2b2327(&iterator))
		{
			if (variant_category_is_game_engine(((s_list_item_datum *)iterator.item)->item, variant.field_xcb8724))
			{
				select_datum(iterator.iterator.datum_index);
				break;
			}
		}
	}
}

// @retail 0x2c5823
void c_game_engine_variant_category_list::v20(c_class_1a2c81 *widget, long index)
{
	s_list_item_text table[7];

	table[0].item = 0;
	table[0].string_handle = 0x600010f;
	table[1].item = 1;
	table[1].string_handle = 0x4000237;
	table[2].item = 3;
	table[2].string_handle = 0x7000110;
	table[3].item = 4;
	table[3].string_handle = 0xa000113;
	table[4].item = 6;
	table[4].string_handle = 0x3000439;
	table[5].item = 7;
	table[5].string_handle = 0x700010e;
	table[6].item = 8;
	table[6].string_handle = 0xb000115;
	function_24c75c(this, widget, table, 0, 7);
}

// @retail 0x2c5897
void c_game_engine_variant_category_list::handle_item(s_controller_reference **controller, long *item)
{
	s_list_item_datum *datum = (s_list_item_datum *)record_pool_lookup(data, *item);

	if (datum)
	{
		short type = datum->item;

		if (create)
		{
			long arg_9db745;
			byte buffer[0x100];
			s_game_variant variant;
			long file_index;

			switch (type)
			{
			case 0:
				arg_9db745 = 1;
				break;
			case 1:
				arg_9db745 = 2;
				break;
			case 3:
				arg_9db745 = 4;
				break;
			case 4:
				arg_9db745 = 5;
				break;
			case 6:
				arg_9db745 = 7;
				break;
			case 7:
				arg_9db745 = 8;
				break;
			default:
				arg_9db745 = 9;
				break;
			}
			g_55c154 = 0;
			if (function_215f40(arg_9db745, buffer) &&
				(file_index = function_212380(arg_9db745, (*controller)->controller_index, buffer)) != NONE &&
				function_212bc0(file_index, &variant))
			{
				g_54e49c = file_index;
				memcpy(&g_54e4a0, &variant, sizeof(g_54e4a0));
				function_238c21((*controller)->controller_index, 5, g_54e4a0.name, 0x20);
			}
			else
			{
				function_148aa3(g_55c154, user_flags);
			}
		}
		else
		{
			s_screen_parameters parameters;

			parameters.field_c = 0;
			function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 5, 4, 0);
			switch (type)
			{
			case 0:
				if (edit_settings)
					parameters.load = function_2ca4cd;
				else if (edit_alternate)
					parameters.load = function_2ca580;
				else
				{
					parameters.load = function_2ca525;
					parameters.a = 3;
				}
				break;
			case 1:
				if (edit_settings)
					parameters.load = function_2ca5d8;
				else if (edit_alternate)
					parameters.load = function_2ca68b;
				else
				{
					parameters.load = function_2ca630;
					parameters.a = 3;
				}
				break;
			case 3:
				if (edit_settings)
					parameters.load = function_2ca6e3;
				else if (edit_alternate)
					parameters.load = function_2ca796;
				else
				{
					parameters.load = function_2ca73b;
					parameters.a = 3;
				}
				break;
			case 4:
				if (edit_settings)
					parameters.load = function_2ca7ee;
				else if (edit_alternate)
					parameters.load = function_2ca8a1;
				else
				{
					parameters.load = function_2ca846;
					parameters.a = 3;
				}
				break;
			case 6:
				if (edit_settings)
					parameters.load = function_2ca8f9;
				else if (edit_alternate)
					parameters.load = function_2ca9ac;
				else
				{
					parameters.load = function_2ca951;
					parameters.a = 3;
				}
				break;
			case 7:
				if (edit_settings)
					parameters.load = function_2caa04;
				else if (edit_alternate)
					parameters.load = function_2caab7;
				else
				{
					parameters.load = function_2caa5c;
					parameters.a = 3;
				}
				break;
			default:
				if (edit_settings)
					parameters.load = function_2cab0f;
				else if (edit_alternate)
					parameters.load = function_2cabc2;
				else
				{
					parameters.load = function_2cab67;
					parameters.a = 3;
				}
				break;
			}
			parameters.load(&parameters);
		}
	}
}

/* ---- the clan member privileges list ---- */

struct s_window_manager_754;
struct s_window_manager_df6;
void function_14896e(s_window_manager_754 *a, s_window_manager_df6 *b);
void __stdcall function_2393ae(long controller, long privilege);

/* the list waiting for the dialog's answer */
c_clan_member_privileges_list *g_51ecd0;

// @retail 0x2c6824
c_clan_member_privileges_list::c_clan_member_privileges_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_clan_member_privileges_list::handle_item)
{
	privilege = 1;
	data = user_interface_data_new("clan member privileges", 4, 4);
	function_16b790(data);
	list_item_add(this, 0);
	list_item_add(this, 1);
	list_item_add(this, 2);
	list_item_add(this, 3);
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2b53a7
c_clan_member_privileges_list::~c_clan_member_privileges_list()
{
	g_51ecd0 = 0;
}

/* the clan member the screens are about (the window manager keeps it at a
   two-byte aligned offset, so it is packed) */
#pragma pack(push, 4)
struct s_clan_member
{
	unsigned __int64 xuid;
	byte unknown08[0x1c - 0x08];
	long privilege;
	byte unknown20[0x94 - 0x20];
};
#pragma pack(pop)

// @retail 0x2c6905
void c_clan_member_privileges_list::v1()
{
	((c_widget *)this)->c_widget::v9();
	select_current_privilege();
}

// @retail 0x2c6915
void c_clan_member_privileges_list::select_current_privilege()
{
	byte clan[0x6a4];
	s_clan_member member;

	function_14896e((s_window_manager_754 *)clan, (s_window_manager_df6 *)&member);
	if (member.xuid)
	{
		switch (member.privilege)
		{
		case 0:
			select_item(0);
			break;
		case 1:
			select_item(1);
			break;
		case 2:
			select_item(2);
			break;
		case 3:
			select_item(3);
			break;
		}
	}
}

// @retail 0x2c6966
void c_clan_member_privileges_list::v20(c_class_1a2c81 *widget, long index)
{
	s_list_item_text table[4];

	table[0].item = 0;
	table[0].string_handle = 0x40002c6;
	table[1].item = 1;
	table[1].string_handle = 0x60002c7;
	table[2].item = 2;
	table[2].string_handle = 0xd0002c8;
	table[3].item = 3;
	table[3].string_handle = 0x90002c9;
	function_24c75c(this, widget, table, 0, 4);
}

/* the dialog's answer: gives the member the privilege */
// @retail 0x2c69b3
bool __stdcall function_2c69b3(long controller)
{
	if (g_51ecd0)
	{
		function_2393ae(controller, g_51ecd0->privilege);
		g_51ecd0->get_screen()->start_animation(3);
	}
	return true;
}

// @retail 0x2c69e4
void c_clan_member_privileges_list::handle_item(s_controller_reference **controller, long *item)
{
	if (*item != NONE)
	{
		s_list_item_datum *datum = (s_list_item_datum *)record_pool_lookup(data, *item);

		if (datum)
		{
			privilege = datum->item;
			g_51ecd0 = this;
			dialog_choice_show(1, 0xa6, 4, 1 << (*controller)->controller_index, function_2c69b3, 0, 0);
		}
	}
}

/* a player color, passed by value */
struct s_player_color
{
	char index;
};

long function_14986f(s_player_color color);

/* shows the color's name */
// @retail 0x2cb2d5
void c_choose_player_color_list::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);

	if (text)
	{
		s_player_color color;

		color.index = (char)widget_item(widget)->value70;
		text->function_253b1a(function_14986f(color));
	}
}

long function_14990f(byte index);
long function_2365e0(long type);

// @retail 0x2cb0d3
void c_choose_emblem_list::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);

	if (text)
	{
		text->function_253b1a(function_14990f((byte)widget_item(widget)->value70));
	}
}

// @retail 0x2cb470
void c_choose_model_list::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);

	if (text)
	{
		text->function_253b1a(function_2365e0(widget_item(widget)->value70 & 0xffff));
	}
}

/* shows the setting's name and the edited profile's value of it */
// @retail 0x2c8548
void c_controller_settings_edit_list::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *name = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);
	c_text_widget_45a5e0 *value = (c_text_widget_45a5e0 *)widget->find_child(6, 1, false);
	long name_handle;
	long value_handle;

	switch ((short)widget_item(widget)->value70)
	{
	case 0:
		name_handle = 0x130003dd;
		switch (g_54e5d0.settings.thumbstick_layout)
		{
		case 0:
			value_handle = 0x7000001;
			break;
		case 1:
			value_handle = 0x80002f5;
			break;
		case 2:
			value_handle = 0x60002f6;
			break;
		case 3:
			value_handle = 0xf0002f7;
			break;
		default:
			value_handle = 0;
			break;
		}
		break;
	case 1:
		name_handle = 0xf0003de;
		switch (g_54e5d0.settings.button_layout)
		{
		case 0:
			value_handle = 0x7000001;
			break;
		case 1:
			value_handle = 0x90003e4;
			break;
		case 2:
			value_handle = 0x50003e5;
			break;
		case 3:
			value_handle = 0xb0003e6;
			break;
		default:
			value_handle = 0;
			break;
		}
		break;
	case 2:
	{
		byte sensitivity = g_54e5d0.settings.look_sensitivity;

		name_handle = 0x100003df;
		switch (sensitivity < 1 ? (short)1 : (sensitivity > 10 ? (short)10 : (short)sensitivity))
		{
		case 1:
			value_handle = 0x120003cf;
			break;
		case 2:
			value_handle = 0x120003d0;
			break;
		case 3:
			value_handle = 0x120003d1;
			break;
		case 4:
			value_handle = 0x120003d2;
			break;
		case 5:
			value_handle = 0x120003d3;
			break;
		case 6:
			value_handle = 0x120003d4;
			break;
		case 7:
			value_handle = 0x120003d5;
			break;
		case 8:
			value_handle = 0x120003d6;
			break;
		case 9:
			value_handle = 0x120003d7;
			break;
		case 10:
			value_handle = 0x130003d8;
			break;
		default:
			value_handle = 0;
			break;
		}
		break;
	}
	case 3:
		name_handle = 0xb0003e0;
		value_handle = TEST_FIELD_BIT(g_54e5d0.settings.controller_flags.invert_look) ? 0x6000308 : 0x7000309;
		break;
	case 4:
		name_handle = 0xb0003e1;
		value_handle = TEST_FIELD_BIT(g_54e5d0.settings.controller_flags.auto_level) ? 0x6000308 : 0x7000309;
		break;
	case 5:
		name_handle = 0x90003e2;
		value_handle = TEST_FIELD_BIT(g_54e5d0.settings.controller_flags.vibration) ? 0x3000182 : 0x2000181;
		break;
	default:
		name_handle = NONE;
		value_handle = 0;
		break;
	}
	if (name)
	{
		name->function_253b1a(name_handle);
	}
	if (value)
	{
		value->function_253b1a(value_handle);
	}
}

long function_149b5a(byte index);
long function_149da5(byte index);

/* shows the setting's name and the edited profile's value of it */
// @retail 0x2c8bf5
void c_multiplayer_settings_edit_list::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *name = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);
	c_text_widget_45a5e0 *value = (c_text_widget_45a5e0 *)widget->find_child(6, 1, false);

	if (name && value)
	{
		long string_handle;

		switch ((short)widget_item(widget)->value70)
		{
		case 0:
			name->function_253b1a(0xc0002f8);
			string_handle = function_2365e0((char)g_54e5d0.settings.model);
			break;
		case 4:
			name->function_253b1a(0x120002fc);
			string_handle = function_14986f(*(s_player_color *)&g_54e5d0.settings.colors[3]);
			break;
		case 3:
			name->function_253b1a(0x120002fb);
			string_handle = function_14986f(*(s_player_color *)&g_54e5d0.settings.colors[2]);
			break;
		case 2:
			name->function_253b1a(0x120002fa);
			string_handle = function_14986f(*(s_player_color *)&g_54e5d0.settings.colors[1]);
			break;
		case 1:
			name->function_253b1a(0x120002f9);
			string_handle = function_14986f(*(s_player_color *)&g_54e5d0.settings.colors[0]);
			break;
		case 5:
			name->function_253b1a(0x110002fd);
			string_handle = function_149b5a(g_54e5d0.settings.unknown11d[0]);
			break;
		case 6:
			name->function_253b1a(0x110002fe);
			string_handle = function_149da5(g_54e5d0.settings.unknown11d[1]);
			break;
		default:
			return;
		}
		value->function_253b1a(string_handle);
	}
}
