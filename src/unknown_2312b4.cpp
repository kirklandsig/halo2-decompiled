// @flags /O1 /arch:SSE /Gr
/* UNKNOWN_2312B4.CPP: the online Y menu, its three tabs (the friends,
   the players met and the recent players) and the user's pending online
   messages */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include "globals.h"
#include "screen_widgets.h"
#include "user_interface_lists.h"
#include "unknown_234c64.h"
#include "unknown_2312b4.h"

#pragma intrinsic(memcpy)

struct s_message;
void function_149f49(s_message *message, word a, dword *id, short b, long c, long d, long e);
void function_236299(long sound);
void function_148523();
void voice_initialize_menu_pool(void);
void voice_dispose_menu_pool(void);
void function_148995(s_window_manager_e94 *value);
void friends_list_reset(bool dispose);
struct s_friend_request
{
	byte data[0x6a2];
};
bool friend_request_get(s_friend_request *request);
bool friends_list_contains(XUID const *xuid);
bool players_list_contains(XUID const *xuid);
bool friends_list_task_running();
bool function_1a325a();
void function_1a31ff();
void function_1a303b(long controller_index);
void __stdcall online_messages_enumerate(DWORD controller_index, s_entry *entries, long *count);
void online_message_delete(DWORD controller_index, DWORD message_id, bool block_sender);
long function_1480ff(long screen_id);
void function_18ff47(long player, dword *out);
struct s_widget_view_2b0a;
void function_2b0a14(s_widget_view_2b0a *widget, short index);

static inline void widget_set_user_flags(c_class_1a2c81 *widget, word user_flags)
{
	widget->user_flags = user_flags;
}

/* a user's slot: set when the user's messages changed */
struct s_player_slot_messages_view
{
	byte unknown000[0x46d];
	bool messages_changed;
};

extern char g_54d5a8;

c_class_1473c9 *__stdcall function_2312c2(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2313a8(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_231995(s_screen_parameters *parameters);

/* ---- the tabs ---- */

// @retail 0x23148e
c_y_menu_tab_screen::c_y_menu_tab_screen(long a, long b, word user_flags) :
	c_class_1473c9(g_54d5a8 == 2 ? 0x1a : 0x1c, a, b, user_flags),
	value610(false)
{
}

/* a user's profile, as the tabs read it */
struct s_y_menu_profile_view
{
	byte unknown000[0x14c];
	long voice_state;
	bool voice_muted;
	byte unknown151[0x1e0 - 0x151];
};

/* a user's record: the gamertag first, a second name at +0x50 */
struct s_y_menu_record_view
{
	wchar_t gamertag[0x20];
	dword value40[4];
	word second_name[0x10];
	byte unknown70[0x7f - 0x70];
	char value7f;
	byte unknown80[0x90 - 0x80];
};

/* a user's online status */
struct s_y_menu_status_view
{
	byte unknown00[0x1c];
	long state;
	byte unknown20[0x92 - 0x20];
};

struct s_player_profile;
struct s_profile_record;
struct s_player_slot_blockb82;
struct s_name_buffer;
void player_slot_get_profile(long index, s_player_profile *profile, long *profile_index);
void function_18fd94(long index, dword *xuid, s_profile_record *record);
bool function_18ffc3(long index, s_player_slot_blockb82 *block);
void function_1a4714(long state, word *buffer);
void function_23620d(long string_handle, word *buffer);
word *function_1630e0(word *buffer, const word *format, ...);
void function_08cc20(s_name_buffer *buffer, const wchar_t *name);
bool voice_port_can_talk(long port);
void function_2b01a2(long value, s_widget_item *item);
void function_22f042(s_widget_item *items, c_class_1a2c81 *widget, long count);

// @retail 0x2314ce
void c_y_menu_tab_screen::v3()
{
	c_class_1a2c81 *voice_bitmap;
	c_class_1a2c81 *muted_bitmap;
	c_class_1a2c81 *bitmap4;
	s_y_menu_profile_view profile;
	long profile_index;
	s_y_menu_record_view record;
	s_y_menu_status_view status;
	word state[0x100];
	word format[0x100];

	c_class_1a2c81::v3();
	voice_bitmap = find_child(8, 5, false);
	muted_bitmap = find_child(8, 6, false);
	bitmap4 = find_child(8, 4, false);
	s_widget_item item;

	player_slot_get_profile(get_controller_index(), (s_player_profile *)&profile, &profile_index);
	function_18fd94(get_controller_index(), NULL, (s_profile_record *)&record);
	function_2b01a2(record.value7f, &item);
	memcpy(item.value48, record.value40, sizeof(item.value48));
	item.flags |= 2;
	if (value610 && function_18ffc3(get_controller_index(), (s_player_slot_blockb82 *)&status))
	{
		state[0] = 0;
		format[0] = 0;
		function_1a4714(status.state, state);
		function_23620d(0x280006be, format);
		function_1630e0(text.text, format, record.gamertag, state);
	}
	else if (record.second_name[0])
	{
		format[0] = 0;
		function_23620d(0x220006bd, format);
		function_1630e0(text.text, format, record.gamertag, record.second_name);
	}
	else
	{
		function_08cc20((s_name_buffer *)text.text, record.gamertag);
	}	item.flags |= 1;
	item.value4 = (long)text.text;
	function_22f042(&item, this, 1);
	if (voice_bitmap)
	{
		if (profile.voice_state == 1)
		{
			voice_bitmap->value6e = true;
			function_2b0a14((s_widget_view_2b0a *)voice_bitmap, 5);
		}
		else if (voice_port_can_talk(get_controller_index()))
		{
			voice_bitmap->value6e = true;
			if (profile.voice_state == 3)
			{
				function_2b0a14((s_widget_view_2b0a *)voice_bitmap, 3);
			}
			else
			{
				function_2b0a14((s_widget_view_2b0a *)voice_bitmap, 4);
			}
		}
		else
		{
			voice_bitmap->value6e = false;
		}
	}
	if (bitmap4)
	{
		bitmap4->value6e = false;
	}
	if (muted_bitmap)
	{
		if (profile.voice_muted)
		{
			muted_bitmap->value6e = false;
		}
		else
		{
			function_2b0a14((s_widget_view_2b0a *)muted_bitmap, 0);
			muted_bitmap->value6e = true;
		}
	}
}

// @retail 0x2312b4
void c_y_menu_tab_screen::v18(void *parameters)
{
	c_class_1a2c81::v1();
}

// @retail 0x23169c
c_y_menu_friends_screen::c_y_menu_friends_screen(long a, long b, word user_flags) :
	c_y_menu_tab_screen(a, b, user_flags),
	value814(NONE),
	list(user_flags)
{
}

/* the players tab's destructors are the friends tab's */
// @retail 0x2318a0 deleting c_y_menu_friends_screen
// @retail 0x2318bc destructor c_y_menu_friends_screen
// @retail 0x2316d0 destructor c_y_menu_friends_list

/* a press of X opens the friends options; back, B and start close the menu */
// @retail 0x231703
bool c_y_menu_friends_screen::v10(s_widget_event *event)
{
	if (event->type == 5)
	{
		switch (event->param)
		{
		case 1:
		case 3:
		case 0xd:
			function_148523();
			return true;
		case 2:
		{
			s_screen_parameters parameters;
			c_friends_options_screen *screen;

			parameters.field_c = 0;
			function_149f49((s_message *)&parameters, 0, 0, 1 << event->controller_index, 3, 4, (long)function_2313a8);
			screen = (c_friends_options_screen *)parameters.load(&parameters);
			if (screen)
			{
				list.name[0] = 0;
				screen->list.name = list.name;
				screen->list.entries = list.entries;
				screen->list.entry_count = 100;
				screen->list.source = list.data;
			}
			return true;
		}
		}
	}
	return c_class_1473c9::v10(event);
}

// @retail 0x231865
c_y_menu_players_screen::c_y_menu_players_screen(long a, long b, word user_flags) :
	c_y_menu_tab_screen(a, b, user_flags),
	value814(NONE),
	list(user_flags)
{
	value610 = true;
}

/* a press of X opens the clan options when a friend request is pending */
// @retail 0x2318d1
bool c_y_menu_players_screen::v10(s_widget_event *event)
{
	if (event->type == 5)
	{
		switch (event->param)
		{
		case 1:
		case 3:
		case 0xd:
			function_148523();
			return true;
		case 2:
		{
			s_friend_request request;

			if (friend_request_get(&request))
			{
				s_screen_parameters parameters;
				c_clan_options_screen *screen;

				parameters.field_c = 0;
				function_149f49((s_message *)&parameters, 0, 0, 1 << event->controller_index, 3, 4, (long)function_2312c2);
				screen = (c_clan_options_screen *)parameters.load(&parameters);
				if (screen)
				{
					list.name[0] = 0;
					screen->list.name = list.name;
					screen->list.entries = list.entries;
					screen->list.entry_count = 100;
					screen->list.source = list.data;
				}
			}
			else
			{
				function_236299(2);
			}
			return true;
		}
		}
	}
	return c_class_1473c9::v10(event);
}

// @retail 0x23179c
c_y_menu_recent_players_screen::c_y_menu_recent_players_screen(long a, long b, word user_flags) :
	c_y_menu_tab_screen(a, b, user_flags),
	value814(NONE),
	list(user_flags)
{
}

// @retail 0x2317d2 deleting c_y_menu_recent_players_screen
// @retail 0x231826 destructor c_y_menu_recent_players_screen
// @retail 0x2317f0 destructor c_y_menu_recent_players_list

// @retail 0x23183b
bool c_y_menu_recent_players_screen::v10(s_widget_event *event)
{
	if (event->type == 5)
	{
		switch (event->param)
		{
		case 1:
		case 3:
		case 0xd:
			function_148523();
			return true;
		}
	}
	return c_class_1473c9::v10(event);
}

/* ---- the menu ---- */

// @retail 0x231995
c_class_1473c9 *__stdcall function_231995(s_screen_parameters *parameters)
{
	c_online_y_menu_screen *screen = new c_online_y_menu_screen(parameters->a, parameters->b, parameters->user_flags);

	if (screen)
	{
		screen->m6c = true;
		screen->function_147f6d(parameters);
	}
	return screen;
}

// @retail 0x2319d6
c_online_y_menu_screen::c_online_y_menu_screen(long a, long b, word user_flags) :
	c_class_1473c9(g_54d5a8 == 2 ? 0x1a : 0x1c, a, b, user_flags),
	controller_index(NONE),
	tab_bar(user_flags),
	friends(a, b, user_flags),
	players(a, b, user_flags),
	recent_players(a, b, user_flags),
	message_count(0)
{
	voice_initialize_menu_pool();
}

// @retail 0x231a7b
c_online_y_menu_screen::~c_online_y_menu_screen()
{
	voice_dispose_menu_pool();
}

// @retail 0x231a5d deleting c_online_y_menu_screen

// @retail 0x2312bc
screen_load_proc c_online_y_menu_screen::get_load_proc()
{
	return function_231995;
}

// @retail 0x231c8c
void c_online_y_menu_screen::v2()
{
	c_class_1a2c81::v2();
	function_148995(0);
	friends_list_reset(true);
}

// @retail 0x231ac4
void c_online_y_menu_screen::v17()
{
	c_class_1473c9 *tab;

	tab = &friends;
	tab->v17();
	tab = &players;
	tab->v17();
	tab = &recent_players;
	tab->v17();
}

/* deletes the messages from players who are neither friends nor players met
   (once, when the friends list is ready) */
// @retail 0x231ca0
void function_231ca0(s_entry *messages, long count, c_online_y_menu_screen *screen)
{
	for (; count; count--, messages++)
	{
		XUID const *xuid = 0;
		if (messages)
			xuid = (XUID const *)&messages->unknown0;

		if ((messages->flags & 0x10000) && !friends_list_contains(xuid) && !players_list_contains(xuid))
		{
			online_message_delete(screen->controller_index, messages->unknown20, false);
		}
	}
}

/* the current tab's icon shows which tab it is */
// @retail 0x231cea
void c_online_y_menu_screen::v3()
{
	c_class_1a2c81 *current = tab_bar.focused;

	function_1a31ff();
	message_count = 0x7d;
	online_messages_enumerate(controller_index, messages, &message_count);
	if (!value55ac && friends_list_task_running() && function_1a325a())
	{
		function_231ca0(messages, message_count, this);
		value55ac = true;
	}
	if (current)
	{
		c_class_1a2c81 *bitmap = current->find_child(8, 1, false);

		if (bitmap)
		{
			short index;

			if (tab_is_current(&friends))
			{
				index = 0;
			}
			else if (tab_is_current(&players))
			{
				index = 1;
			}
			else if (tab_is_current(&recent_players))
			{
				index = 2;
			}
			else
			{
				goto done;
			}
			function_2b0a14((s_widget_view_2b0a *)bitmap, index);
		}
	}
done:
	c_class_1a2c81::v3();
}

/* the user's controller, the tabs and their lists, and the user's messages */
// @retail 0x231ae9
void c_online_y_menu_screen::v18(void *parameters)
{
	word user_flags = ((s_screen_parameters *)parameters)->user_flags;

	if (user_flags & 1)
	{
		controller_index = 0;
	}
	else if (user_flags & 2)
	{
		controller_index = 1;
	}
	else if (user_flags & 4)
	{
		controller_index = 2;
	}
	else if (user_flags & 8)
	{
		controller_index = 3;
	}
	else
	{
		controller_index = 0;
	}
	value55ac = false;
	long one = 1;
	friends_list_reset(one != 0);
	function_1a303b(controller_index);
	{
		long controller = *(volatile long *)&controller_index;
		friends.value814 = controller;
		friends.list.value88 = controller;
		*(word volatile *)&friends.list.user_flags = (word)(one << friends.list.value88);
	}
	{
		long controller = *(volatile long *)&controller_index;
		players.value814 = controller;
		players.list.value88 = controller;
		*(word volatile *)&players.list.user_flags = (word)(one << players.list.value88);
	}
	{
		long controller = *(volatile long *)&controller_index;
		recent_players.value814 = controller;
		recent_players.list.value88 = controller;
		*(word volatile *)&recent_players.list.user_flags = (word)(one << recent_players.list.value88);
	}
	/* Preserve the write before the layout reuses this parameter slot. */
	*(void *volatile *)&parameters = (void *)function_1480ff(screen_id);
	{
		s_screen_layout layout =
		{
			&tab_bar,
			3,
			{
				{ 0, 0, (c_class_1474e8 *)(parameters = &friends.list), 0 },
				{ 0, 0, &players.list, 0 },
				{ 0, 0, &recent_players.list, 0 }
			}
		};

		tab_bar.add_child(&friends);
		tab_bar.add_child(&players);
		tab_bar.add_child(&recent_players);
		build(&layout);
	}
	{
		s_window_manager_e94 user;

		function_18ff47(controller_index, user.data);
		function_148995(&user);
	}
	message_count = 0x7d;
	online_messages_enumerate(controller_index, messages, &message_count);
	((s_player_slot_messages_view *)&g_54e8e0[controller_index])->messages_changed = false;
	c_class_1a2c81::v1();
	friends.v7((c_class_1a2c81 *)parameters);
}
