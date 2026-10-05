#include <string.h>
#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include "screen_widgets.h"
#include "unknown_19b510.h"
#include "unknown_19b516.h"
#include "unknown_234c64.h"
#include "online_message_entries.h"

// @flags /O1 /Oi /Gr

/* UNKNOWN_2B7B48.CPP: the screen that writes a message (text
   and voice) and sends it to the chosen players */

struct s_message;
void function_149f49(s_message *message, word a, dword *id, word b, long c, long d, long e);
void function_14887e(s_screen_settings_54dc6c *settings);
struct s_window_manager_text;
const char *function_148956(s_window_manager_text *text);
void function_236299(long sound);
void function_238c21(long controller, long type, word *name, long maximum_count);
void unicode_string_copy(word *destination, const word *source, long maximum_count);
void function_22d2ee(word *string, long maximum_length);
bool function_6c7e0();
bool function_1906da(long index);
bool voice_data_is_wave(const long *data, long size);
bool voice_port_can_talk(long port);
long online_task_poll(long task_index);
void function_08eeb0(s_state_block *block);
void online_message_block_set_text(s_state_block *block, const wchar_t *text);
void online_message_block_set_values(s_state_block *block, long value210, long value214, long value20c);
void function_08fa30(s_state_block *block);
long function_08eff0(s_state_block *block, long controller_index, XUID const *recipients, long recipient_count);
long function_08ef90(s_state_block *block, long controller_index, const char *gamertag);
c_class_1473c9 *__stdcall function_2c9012(s_screen_parameters *parameters);

/* the voice message being recorded: its data, size and length */
dword g_504940;
byte g_504948[0x499a];
dword g_5092e4;

/* the player the online screens act on (function_14887e) */
struct s_message_send_selection
{
	long type;
	union
	{
		XONLINE_USER user;
		XONLINE_FRIEND friend_;
	};
	byte unknown[0x78 - 4 - sizeof(XONLINE_USER)];
};

/* the voice recording screen that 0x2c9012 loads: where it records */
struct s_voice_record_screen_view
{
	byte unknown000[0x61c];
	byte *data;
	long maximum_size;
	dword *size;
	dword *length;
};

/* "xbox live message send list" (vtable 0x45c068): send, edit the text,
   record a voice message */
class c_xbox_live_message_send_list : public c_class_1474e8
{
public:
	c_xbox_live_message_send_list(word user_flags);
	~c_xbox_live_message_send_list();

	/* makes the message block */
	virtual void v1();
	/* folded with c_squad_privacy_setting_list's */
	virtual long get_item_count() { return 3; }
	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);
	void send(s_controller_reference **controller);
	void function_2b7da6(s_controller_reference **controller);
	void set_default_text();

	c_class_14750b items[3];
	/* 1 and 2: a reply, 3: a friend request, 4: a message */
	long type;
	s_state_block *block;
	word text[0x100];
	XUID const *recipients;
	long recipient_count;
	long task_index;
	/* send to the gamertag of the settings (function_14887e) */
	bool to_gamertag;
	byte unknown41d[3];
	c_list_item_handler handler;
};

/* the screen (vtable 0x45bff8) */
class c_xbox_live_message_send_screen : public c_screen_with_menu
{
public:
	c_xbox_live_message_send_screen(long a, long b, word user_flags);

	/* shows the title, the text and the voice message's length */
	virtual void v3();
	/* ignores input once the message is sent */
	virtual bool v10(s_widget_event *event);
	virtual screen_load_proc get_load_proc();

	c_xbox_live_message_send_list list;
	bool sent;
	byte unknowna4d[3];
	long valuea50;
};

void function_2b7de4(s_controller_reference **controller);

// @retail 0x2b8045
c_class_1473c9 *function_2b8045(s_screen_parameters *parameters, long type, bool to_gamertag)
{
	c_xbox_live_message_send_screen *screen = new c_xbox_live_message_send_screen(parameters->a, parameters->b, parameters->user_flags);

	if (screen)
	{
		screen->list.type = type;
		screen->m6c = true;
		screen->list.to_gamertag = to_gamertag;
		screen->function_147f6d(parameters);
	}
	return screen;
}

// @retail 0x2b8099
c_class_1473c9 *__stdcall function_2b8099(s_screen_parameters *parameters)
{
	return function_2b8045(parameters, 1, false);
}

// @retail 0x2b80a9
c_class_1473c9 *__stdcall function_2b80a9(s_screen_parameters *parameters)
{
	return function_2b8045(parameters, 2, false);
}

// @retail 0x2b80b9
c_class_1473c9 *__stdcall function_2b80b9(s_screen_parameters *parameters)
{
	return function_2b8045(parameters, 1, true);
}

// @retail 0x2b80c9
c_class_1473c9 *__stdcall function_2b80c9(s_screen_parameters *parameters)
{
	return function_2b8045(parameters, 2, true);
}

// @retail 0x2b80d9
c_class_1473c9 *__stdcall function_2b80d9(s_screen_parameters *parameters)
{
	return function_2b8045(parameters, 3, false);
}

// @retail 0x2b80e9
c_class_1473c9 *__stdcall function_2b80e9(s_screen_parameters *parameters)
{
	return function_2b8045(parameters, 4, false);
}

// @retail 0x2b80f9
screen_load_proc c_xbox_live_message_send_screen::get_load_proc()
{
	switch (list.type)
	{
	case 1:
		return list.to_gamertag ? function_2b80b9 : function_2b8099;
	case 2:
		if (list.to_gamertag)
		{
			return function_2b80c9;
		}
		else
		{
			return function_2b80a9;
		}
	case 3:
		return function_2b80d9;
	case 4:
		return function_2b80e9;
	}
	return 0;
}

// @retail 0x2b8143
c_xbox_live_message_send_screen::c_xbox_live_message_send_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0xd7, a, b, user_flags, &list),
	list(user_flags)
{
	valuea50 = 0;
	sent = false;
}

// @retail 0x2b8186 deleting c_xbox_live_message_send_screen
// @retail 0x2b81a4 destructor c_xbox_live_message_send_screen

// @retail 0x2b81b9
bool c_xbox_live_message_send_screen::v10(s_widget_event *event)
{
	if (sent)
		return sent;
	return c_class_1473c9::v10(event);
}

// @retail 0x2b81cb
void c_xbox_live_message_send_screen::v3()
{
	c_class_1a2c81 *text = find_child(6, 2, false);
	c_class_2b01eb *meter = (c_class_2b01eb *)find_child(8, 6, false);
	c_text_widget_45a5e0 *length_text = (c_text_widget_45a5e0 *)find_child(6, 4, false);

	switch (list.type)
	{
	case 1:
		((c_text_widget_45a5e0 *)&title)->function_253b1a(0x1d0002cc);
		break;
	case 2:
		((c_text_widget_45a5e0 *)&title)->function_253b1a(0x1b0002d1);
		break;
	case 3:
		if (valuea50 == 1)
			((c_text_widget_45a5e0 *)&title)->function_253b1a(0x260002cf);
		else if (valuea50 == 2)
			((c_text_widget_45a5e0 *)&title)->function_253b1a(0x280002d0);
		else
			((c_text_widget_45a5e0 *)&title)->function_253b1a(0x1b0002ce);
		break;
	default:
		if (valuea50 == 1)
			((c_text_widget_45a5e0 *)&title)->function_253b1a(0x1f0002d6);
		else if (valuea50 == 2)
			((c_text_widget_45a5e0 *)&title)->function_253b1a(0x210002d7);
		else
			((c_text_widget_45a5e0 *)&title)->function_253b1a(0x140002d5);
		break;
	}
	if (text)
	{
		text->function_22f52e()->set_text(list.text);
	}
	if (meter)
	{
		meter->value84 = (real)g_5092e4 * 6.6666667e-5f;
	}
	if (length_text)
	{
		length_text->function_253b1a(g_5092e4 > 0 ? 0x120002e1 : 0x110002e2);
	}
	c_class_1a2c81::v3();
	if (sent && !ANIMATION_FLAG(animation, 1))
	{
		if (list.task_index != NONE)
		{
			switch (online_task_poll(list.task_index))
			{
			case 0:
			case 1:
				return;
			}
		}
		start_animation(3);
	}
}

// @retail 0x2b7b48
c_xbox_live_message_send_list::c_xbox_live_message_send_list(word user_flags) :
	c_class_1474e8(user_flags),
	type(4),
	block(0),
	recipients(0),
	recipient_count(0),
	task_index(NONE),
	to_gamertag(false),
	handler(this, (list_item_method)&c_xbox_live_message_send_list::handle_item)
{
	bool online;

	text[0] = 0;
	memset(g_504948, 0, sizeof(g_504948));
	g_504940 = 0;
	g_5092e4 = 0;
	data = user_interface_data_new("xbox live message send list", 3, 4);
	function_16b790(data);
	online = function_1906da(get_controller_index());
	list_item_add(this, 0);
	if (online)
	{
		list_item_add(this, 1);
		list_item_add(this, 2);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2b7c6a deleting c_xbox_live_message_send_list

// @retail 0x2b7c86
c_xbox_live_message_send_list::~c_xbox_live_message_send_list()
{
	if (block)
	{
		function_08fa30(block);
		block = 0;
	}
}

// @retail 0x2b7cd8
void c_xbox_live_message_send_list::v1()
{
	s_state_block *memory = (s_state_block *)function_1a47fd(sizeof(s_state_block));
	s_state_block *local_8dd4ff;

	if (memory)
	{
		function_08eeb0(memory);
		local_8dd4ff = memory;
	}
	else
	{
		local_8dd4ff = 0;
	}
	block = local_8dd4ff;
	((c_widget *)this)->c_widget::v9();
}

// @retail 0x2b7d00
void c_xbox_live_message_send_list::v20(c_class_1a2c81 *widget, long index)
{
	s_list_item_text table[3];

	table[0].item = 0;
	table[0].string_handle = 0xc0002a6;
	table[1].item = 1;
	table[1].string_handle = 0x13000601;
	table[2].item = 2;
	table[2].string_handle = 0x14000602;
	function_24c75c(this, widget, table, 0, 3);
}

// @retail 0x2b7d40
void c_xbox_live_message_send_list::handle_item(s_controller_reference **controller, long *item)
{
	if (!((c_xbox_live_message_send_screen *)get_screen())->sent && function_6c7e0())
	{
		if (*item != NONE)
		{
			switch (*item & 0xffff)
			{
			case 0:
				send(controller);
				break;
			case 1:
				function_2b7da6(controller);
				break;
			case 2:
				function_2b7de4(controller);
				break;
			}
		}
	}
	else
	{
		function_236299(2);
	}
}

/* lets the user write the text, starting from the default one */
// @retail 0x2b7da6
void c_xbox_live_message_send_list::function_2b7da6(s_controller_reference **controller)
{
	unsigned long length = 0;

	while (length < 0x100 && text[length])
	{
		length++;
	}
	if (!length)
	{
		set_default_text();
	}
	function_238c21((*controller)->controller_index, 0xe, text, 0x100);
}

/* opens the voice recording screen */
// @retail 0x2b7de4
void function_2b7de4(s_controller_reference **controller)
{
	if (voice_port_can_talk((*controller)->controller_index))
	{
		s_screen_parameters parameters;
		s_voice_record_screen_view *screen;

		parameters.field_c = 0;
		function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 1, 4, (long)function_2c9012);
		screen = (s_voice_record_screen_view *)parameters.load(&parameters);
		g_504940 = 0;
		g_5092e4 = 0;
		if (screen)
		{
			screen->data = g_504948;
			screen->maximum_size = sizeof(g_504948);
			screen->size = &g_504940;
			screen->length = &g_5092e4;
		}
	}
	else
	{
		function_236299(2);
	}
}

/* sends the message */
// @retail 0x2b7e69
void c_xbox_live_message_send_list::send(s_controller_reference **controller)
{
	XUID const *message_recipients;
	long count;
	bool voice;
	s_message_send_selection selection;
	s_screen_settings_54dc6c settings;

	if (recipients && recipient_count > 0)
	{
		message_recipients = recipients;
		count = recipient_count;
	}
	else if (!to_gamertag)
	{
		XUID *xuid;

		function_14887e((s_screen_settings_54dc6c *)&selection);
		xuid = 0;
		switch (selection.type)
		{
		case 1:
			xuid = &selection.user.xuid;
			break;
		case 2:
			xuid = &selection.friend_.xuid;
			break;
		}
		message_recipients = xuid;
		if (xuid && *(unsigned __int64 *)xuid)
			count = 1;
		else
			count = 0;
	}
	voice = false;
	if (g_504940 > 0 && g_5092e4 > 0 && voice_data_is_wave((const long *)g_504948, g_504940))
	{
		voice = true;
	}
	if (block)
	{
		long task;

		if (type == 4 && !text[0] && !voice)
		{
			function_236299(2);
			return;
		}
		online_message_block_set_text(block, (const wchar_t *)text);
		if (voice)
		{
			online_message_block_set_values(block, (long)g_504940, (long)g_504948, g_5092e4);
		}
		block->unknown4 = type;
		if (!to_gamertag)
		{
			task = function_08eff0(block, (*controller)->controller_index, message_recipients, count);
		}
		else
		{
			function_14887e(&settings);
			task = function_08ef90(block, (*controller)->controller_index, function_148956((s_window_manager_text *)&settings));
		}
		block = 0;
		task_index = task;
	}
	((c_xbox_live_message_send_screen *)get_screen())->sent = true;
}

/* the text a message of this type starts with */
// @retail 0x2b7fc5
void c_xbox_live_message_send_list::set_default_text()
{
	word buffer[0x100];

	buffer[0] = 0;
	switch (type)
	{
	case 1:
		((c_widget *)get_screen())->function_230134(0x130002dc, buffer);
		break;
	case 2:
		((c_widget *)get_screen())->function_230134(0x110002dd, buffer);
		break;
	case 3:
		((c_widget *)get_screen())->function_230134(0x120002da, buffer);
		break;
	default:
		((c_widget *)get_screen())->function_230134(0x140002de, buffer);
		break;
	}
	unicode_string_copy(text, buffer, 0x100);
	function_22d2ee(text, 0x100);
}
