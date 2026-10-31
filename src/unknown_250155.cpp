// @flags /O1 /Gr
/* UNKNOWN_250155.CPP: the screens and lists of 0x250155..0x2541b2 (the
   matchmaking screens) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <stdlib.h>
#include <string.h>
#include "data_array.h"
#include "screen_widgets.h"
#include "unknown_19b510.h"
#include "unknown_19b516.h"
#include "unknown_234c64.h"
#include "unknown_24b5bc.h"
#include "unknown_2b116a.h"
#include "unknown_07f720.h"
#include "globals.h"

#pragma intrinsic(memset, memcpy, memcmp)

bool function_1999b3(void);
long function_19a161(void);

#include "unknown_24fa12.h"

struct s_session_screen_state;
long session_lobby_screen_any_team_valid(s_session_screen_state const *screen);
bool function_592f0(void);
bool function_19a902(void);
bool network_session_interface_local_machine_is_host(void);

// @retail 0x250dae
void function_250dae(c_session_lobby_screen *screen)
{
    if (ANIMATION_FLAG(screen->animation, 1))
        return;
    bool connected = function_592f0();
    bool enabled = function_592f0() && !(byte)session_lobby_screen_any_team_valid((s_session_screen_state *)screen) &&
        !function_19a902() && network_session_interface_local_machine_is_host();
    c_class_1a2c81 *text = 0;
    switch (screen->mode)
    {
    case 0:
    case 2:
        text = screen->find_child(6, 45, false);
        break;
    case 1:
        text = screen->find_child(6, 40, false);
        break;
    }
    if (connected)
    {
        screen->button0.value6e = true;
        screen->button1.value6e = true;
        if (text) text->value6e = false;
    }
    else
    {
        screen->button0.value6e = false;
        screen->button1.value6e = false;
        if (text) text->value6e = true;
    }
    if (enabled != screen->value810)
    {
        if (function_592f0())
        {
            if (enabled)
            {
                if (screen->value811)
                    screen->v7(&screen->button0);
                else
                    screen->v7(&screen->button1);
            }
            else
            {
                screen->value811 = screen->button0.is_in_window();
                screen->function_22ecb4_2(true);
            }
        }
        else
            screen->function_22ecb4_2(true);
        screen->value810 = enabled;
    }
}

struct s_game_variant_setting_category;
struct s_game_variant_setting_definition;
struct s_game_variant_setting_option;
s_game_variant_setting_category *game_variant_settings_find_category(long category_id);
s_game_variant_setting_option *game_variant_setting_find_option(s_game_variant_setting_definition *setting, long value);
long function_2374f0(void *base, long index);
void function_1a0180(long tag_index, long string_handle, word *buffer);
dword function_217b70(word const *text);

struct s_setting_reference_list_view
{
    long id;
    byte unknown04[4];
    long count;
    struct s_reference { long group; long index; } *references;
};

struct s_setting_definition_view
{
    long field_index;
    byte unknown04[8];
    long string_list;
    long title;
};

// @retail 0x25092c
void function_25092c(s_game_variant *variant, long index, c_class_1a2c81 *widget, word const *separator, word *buffer)
{
    word title[0x100];
    word value[0x100];
    title[0] = 0;
    value[0] = 0;
    long group;
    switch (*(long *)((byte *)variant + 0x44))
    {
    case 1: group = 22; break;
    case 2: group = 23; break;
    case 3: group = 24; break;
    case 4: group = 25; break;
    case 7: group = 28; break;
    case 8: group = 29; break;
    case 9: group = 30; break;
    default: group = NONE; break;
    }
    s_setting_reference_list_view *list = (s_setting_reference_list_view *)game_variant_settings_find_category(group);
    if (list && index >= 0 && index < list->count)
    {
        s_setting_reference_list_view::s_reference const *reference = &list->references[index];
        if (reference->index != NONE)
        {
            s_setting_definition_view *definition = (s_setting_definition_view *)g_4e3b44[reference->index & 0xffff].bytes;
            s_game_variant_setting_option *option = game_variant_setting_find_option((s_game_variant_setting_definition *)definition, function_2374f0(variant, definition->field_index));
            function_1a0180(definition->string_list, definition->title, title);
            if (option)
                function_1a0180(definition->string_list, *(long *)((byte *)option + 8), value);
        }
    }
    if (widget && (long)function_217b70(title) > 0)
    {
        function_1630e0(buffer, (word const *)L"%s%s%s", title, separator, value);
        widget->function_22f52e()->set_text(buffer);
        widget->value6e = true;
    }
}

/* shows the network connection's state in the mode's text */
// @retail 0x250155
void c_session_lobby_screen::function_250155()
{
	long index;

	switch (mode)
	{
	case 0:
		index = 12;
		break;
	case 1:
		index = 7;
		break;
	case 2:
		index = 12;
		break;
	default:
		index = NONE;
		break;
	}

	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)find_text((short)index);
	if (text)
	{
		if (function_1999b3())
		{
			text->value6e = false;
		}
		else
		{
			text->value6e = true;
			switch (function_19a161())
			{
			case 0:
				text->function_253b1a(0x4000201);
				break;
			case 1:
				text->function_253b1a(0xf000202);
				break;
			case 2:
				text->function_253b1a(0x6000203);
				break;
			}
		}
	}
}

/* "matchmaking list" (vtable 0x45a340): sixteen items, sorted by slot 3 */
class c_matchmaking_list : public c_class_1474e8
{
public:
	c_matchmaking_list(word user_flags);

	virtual void v1();
	/* fills the list with the session's players */
	virtual void v3();
	virtual void v20(c_class_1a2c81 *item, long unused);


	c_class_14750b items[16];
};

// @retail 0x251469
c_matchmaking_list::c_matchmaking_list(word user_flags) :
	c_class_1474e8(user_flags)
{
}

// @retail 0x2514d7
void c_matchmaking_list::v1()
{
	data = user_interface_data_new("matchmaking list", 16, 4);
	if (data)
	{
		function_16b790(data);
	}
	((c_widget *)this)->c_widget::v9();
}

// @retail 0x251497 deleting c_matchmaking_list
// @retail 0x2514b5 destructor c_matchmaking_list

c_class_1473c9 *__stdcall function_2519bb(s_screen_parameters *parameters);
long function_75870(void); /* 0x75870, unknown_075870.cpp */

bool g_51ec99;

/* the matchmaking screen (vtable 0x45a398) */
class c_matchmaking_screen : public c_class_1473c9
{
public:
	c_matchmaking_screen(long a, long b, word user_flags);

	virtual bool v10(s_widget_event *event);
	virtual void v3();
	virtual void v18(void *parameters);
	virtual screen_load_proc get_load_proc();

	long get_title();

	long value610;
	long value614;
	long value618;
	long value61c;
	bool value620;
	s_text_256 texts[6];
	short value1222;
	byte unknown1224[0x1424 - 0x1224];
	c_matchmaking_list list;
};

c_class_1473c9 *__stdcall function_233395(s_screen_parameters *parameters);
/* the count of the postgame statistics' players (unknown_232d43.cpp) */
extern long g_51ec08;

/* opens the saved film's postgame statistics, when there are players */
// @retail 0x2510aa
void function_2510aa(long controller_index)
{
	if (g_51ec08 > 0)
	{
		s_screen_parameters parameters;

		parameters.field_c = 0;
		function_149f49((s_message *)&parameters, 0, 0, 1 << controller_index, 3, 4, (long)function_233395);
		parameters.load(&parameters);
	}
}

// @retail 0x25137e
screen_load_proc c_matchmaking_screen::get_load_proc()
{
	return function_2519bb;
}

struct s_widget_view_2b0a;
void function_2b0a14(s_widget_view_2b0a *widget, short index);
bool voice_port_flag0_only(long port);
bool function_53750(long player_index);

/* the item's voice icon hides */
// @retail 0x251963
void function_251963(c_class_1a2c81 *item)
{
	c_class_1a2c81 *bitmap = item->find_child(8, 0, false);

	if (bitmap)
	{
		bitmap->value6e = false;
	}
}

/* the item's voice icon shows the player's voice state */
// @retail 0x251977
void function_251977(c_class_1a2c81 *item, long player)
{
	c_class_1a2c81 *bitmap = item->find_child(8, 0, false);

	if (bitmap && player != NONE)
	{
		short index;

		if (voice_port_flag0_only(player))
		{
			index = !function_53750(player);
		}
		else
		{
			index = 2;
		}
		function_2b0a14((s_widget_view_2b0a *)bitmap, index);
		bitmap->value6e = true;
	}
}

// @retail 0x2519bb
c_class_1473c9 *__stdcall function_2519bb(s_screen_parameters *parameters)
{
	c_matchmaking_screen *screen = new c_matchmaking_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2519f7
c_matchmaking_screen::c_matchmaking_screen(long a, long b, word user_flags) :
	c_class_1473c9(0xd2, a, b, user_flags),
	value610(0),
	value618(0),
	value61c(function_75870()),
	value620(false),
	value1222(0),
	list(user_flags)
{
	g_51ec99 = false;
}

// @retail 0x251a73 deleting c_matchmaking_screen
// @retail 0x251a91 destructor c_matchmaking_screen

/* a message box callback that accepts */
// @retail 0x2523b7
bool __stdcall function_2523b7(long controller)
{
	return true;
}

/* ---- the network squad browser ---- */

/* "network squad list" (vtable 0x45a500): the squads found on the system
   link */
class c_class_252b72 : public c_class_1474e8
{
public:
	c_class_252b72(word user_flags, bool alternate);

	/* forgets the focused squad */
	virtual void v2();
	/* refills the list, then remembers the focused squad */
	virtual void v3();
	/* shows a squad's name and game, or the item that creates one */
	virtual void v20(c_class_1a2c81 *item, long unused);
	/* folded with c_widget's v2 */
	virtual void *get_item_data() { return items; }

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[5];
	long value308;
	c_list_item_handler handler;
	bool searching;
	bool alternate;
};

/* the network squad browser screen (vtable 0x45a558) */
class c_network_squad_browser_screen : public c_screen_with_menu
{
public:
	c_network_squad_browser_screen(long a, long b, word user_flags, bool alternate);

	/* stops the search */
	virtual void v2();
	/* A or start opens the four way sign in */
	virtual bool v10(s_widget_event *event);
	/* shows the focused squad: its players, game and map */
	virtual void v3();
	/* starts the search and loads the maps' bitmaps */
	virtual void v19();
	virtual screen_load_proc get_load_proc();

	c_class_252b72 list;
	bool alternate;
	bool value93d;
	bool value93e;
};

void __stdcall function_252ed8(c_class_252b72 *list);
bool function_199df9(bool offline, bool system_link);
void function_199a03(long mode);
void function_199c47(long index);
bool function_199ba5(long index);
byte *function_199bbf(long index);
word function_1901fc(void);
void function_236299(long sound);

/* a datum of the list: a squad, or the item that creates one */
struct s_network_squad_datum
{
	short salt;
	short unknown02;
	long index;
	bool create;
	byte unknown09[3];
};

/* a squad found on the system link */
struct s_network_squad
{
	byte unknown00[0xa0];
	/* its game's category (0 when it has none) */
	long category;
	byte unknowna4[4];
	long valuea8;
	/* the key of its map's entry */
	long map_key;
	bool has_teams;
	byte unknownb1[0xbc - 0xb1];
	short player_count;
	byte unknownbe[0x17e - 0xbe];
	word player_names[16][0x20];
	byte unknown57e[0x5c0 - 0x57e];
	short player_teams[16];
	dword player_appearances[16][4];
};

/* the squad focused last */
long g_470b18 = NONE;

// @retail 0x252b72
c_class_252b72::c_class_252b72(word user_flags, bool alternate) :
	c_class_1474e8(user_flags),
	value308(0),
	handler(this, (list_item_method)&c_class_252b72::handle_item)
{
	g_470b18 = NONE;
	searching = false;
	this->alternate = alternate;
	data = user_interface_data_new("network squad list", 0x21, 0xc);
	function_16b790(data);
	function_252ed8(this);
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2530ec
void c_class_252b72::handle_item(s_controller_reference **controller, long *item)
{
	if (data && *item != NONE)
	{
		s_network_squad_datum *datum = &((s_network_squad_datum *)data->data)[*item & 0xffff];

		if (datum->create)
		{
			if (function_199df9(0, alternate))
			{
				function_199a03(2);
			}
		}
		else
		{
			long index = datum->index;
			s_network_squad *squad;

			if (index != NONE && function_199ba5(index) && (squad = (s_network_squad *)function_199bbf(index)) != 0)
			{
				if (squad->player_count >= 16)
				{
					dialog_ok_show(1, 0x41, 4, function_1901fc(), 0, 0);
				}
				else
				{
					function_199c47(index);
					return;
				}
			}
			function_236299(2);
		}
	}
}

// @retail 0x252ea0
void c_class_252b72::v3()
{
	long datum_index;

	function_252ed8(this);
	datum_index = get_focused_datum();
	if (datum_index != NONE && data)
	{
		g_470b18 = ((s_network_squad_datum *)data->data)[datum_index & 0xffff].index;
	}
	((c_widget *)this)->c_widget::v11();
}

long function_199b6a(long start);
void function_199b5b(bool start);
struct s_0b35e0_entry
{
	byte unknown00[0x44];
	bool value44;
};
s_0b35e0_entry *session_search_results_get(long index);
struct s_type_99af70;
bool function_07aaa0(XNADDR *xnaddr, s_type_99af70 *address);

/* the squads found on the system link: their xnet address at +0x70 */
struct s_network_squad_address_view
{
	byte unknown00[0x70];
	XNADDR address;
};

/* refills the list with the squads found (not the title's own), after the
   item that creates one; the items are reassigned when a datum went away */
// @retail 0x252ed8
void __stdcall function_252ed8(c_class_252b72 *list)
{
	c_class_252b72 **list_reference = &list;

	if (list->data)
	{
		function_199b5b(list->searching);
		if (list->searching)
		{
			XNADDR address;
			bool have_address = function_07aaa0(&address, NULL);
			long focused = list->get_focused_datum();
			long old_count;
			long datum_index;
			s_network_squad_datum *datum;
			long index;
			bool changed;
			unsigned long i;

			if (focused == NONE)
			{
				focused = record_pool_next_used(list->data, focused);
			}
			old_count = list->data->actual_count;
			record_pool_release_all(list->data);
			datum_index = record_pool_allocate(list->data);
			datum = &((s_network_squad_datum *)list->data->data)[datum_index & 0xffff];
			datum->index = NONE;
			datum->create = true;
			for (index = function_199b6a(NONE); index != NONE; index = function_199b6a(index))
			{
				bool own = false;
				s_network_squad_address_view *squad;
				s_0b35e0_entry *entry;

				if (function_199ba5(index))
				{
					squad = (s_network_squad_address_view *)function_199bbf(index);
					entry = session_search_results_get(index);
				}
				else
				{
					squad = NULL;
					entry = NULL;
				}
				if (have_address && squad)
				{
					own = memcmp(&squad->address, &address, sizeof(address)) == 0;
				}
				if (entry && entry->value44 && !own)
				{
					datum_index = record_pool_allocate(list->data);
					if (datum_index == NONE)
					{
						break;
					}
					datum = &((s_network_squad_datum *)list->data->data)[datum_index & 0xffff];
					datum->create = false;
					datum->index = index;
				}
			}
			changed = list->items[0].value70 == NONE || old_count != list->data->actual_count;
			for (i = 0; i < 5; i++)
			{
				if (changed)
				{
					break;
				}
				if (list->items[i].value70 == NONE)
				{
					return;
				}
				if (!record_pool_lookup(list->data, list->items[i].value70))
				{
					changed = true;
				}
			}
			if (changed)
			{
				for (i = 0; i < 5; i++)
				{
					list->items[i].value70 = NONE;
				}
				datum_index = record_pool_next_used(list->data, NONE);
				for (i = 0; i < 5 && datum_index != NONE; i++)
				{
					list->items[i].value70 = datum_index;
					datum_index = record_pool_next_used(list->data, datum_index);
				}
				list->select_datum(focused);
			}
		}
	}
}
/* a squad found on the system link, as its item shows it */
struct s_network_squad_item_view
{
	byte unknown00[0x14];
	short state;
	byte unknown16[2];
	word name[0x40];
	byte unknown98[0x9e - 0x98];
	short game_type;
};

// @retail 0x252c6a
void c_class_252b72::v20(c_class_1a2c81 *item, long unused)
{
	c_class_1a2c81 *name_text = item->find_child(6, 0, false);
	c_class_1a2c81 *game_text = item->find_child(6, 1, false);
	long datum_index = widget_item(item)->value70;

	if (datum_index != NONE)
	{
		s_network_squad_datum *datum = &((s_network_squad_datum *)data->data)[datum_index & 0xffff];

		if (name_text)
		{
			if (datum->create)
			{
				((c_text_widget_45a5e0 *)name_text)->function_253b1a(0xf000236);
			}
			else
			{
				long index = datum->index;
				word *name = L"";
				s_network_squad_item_view *squad;

				if (index != NONE && function_199ba5(index) && (squad = (s_network_squad_item_view *)function_199bbf(index)) != 0)
				{
					name = squad->name;
				}
				name_text->function_22f52e()->set_text(name);
			}
		}
		if (game_text)
		{
			game_text->function_22f52e()->set_text(L"");
			if (!datum->create)
			{
				long index = datum->index;
				s_network_squad_item_view *squad;

				if (index != NONE && function_199ba5(index) && (squad = (s_network_squad_item_view *)function_199bbf(index)) != 0)
				{
					long string_handle = 0;
					bool state0 = squad->state == 0;
					bool state1 = squad->state == 1;
					bool state2 = squad->state == 2;

					switch (squad->game_type)
					{
					case 0:
						if (state0)
						{
							string_handle = 0x9000238;
						}
						else if (state1)
						{
							string_handle = 0xb00023f;
						}
						else if (state2)
						{
							string_handle = 0xb000246;
						}
						break;
					case 1:
						if (state0)
						{
							string_handle = 0x9000239;
						}
						else if (state1)
						{
							string_handle = 0xb000240;
						}
						else if (state2)
						{
							string_handle = 0xb000247;
						}
						break;
					case 6:
						if (state0)
						{
							string_handle = 0x1000023a;
						}
						else if (state1)
						{
							string_handle = 0x12000241;
						}
						else if (state2)
						{
							string_handle = 0x12000248;
						}
						break;
					case 2:
					case 7:
						if (state0)
						{
							string_handle = 0x1200023c;
						}
						else if (state1)
						{
							string_handle = 0x14000243;
						}
						else if (state2)
						{
							string_handle = 0x1400024a;
						}
						break;
					case 3:
					case 8:
						if (state0)
						{
							string_handle = 0xb00023d;
						}
						else if (state1)
						{
							string_handle = 0xd000244;
						}
						else if (state2)
						{
							string_handle = 0xd00024b;
						}
						break;
					case 4:
					case 9:
						if (state0)
						{
							string_handle = 0xd00023e;
						}
						else if (state1)
						{
							string_handle = 0xf000245;
						}
						else if (state2)
						{
							string_handle = 0xf00024c;
						}
						break;
					}
					((c_text_widget_45a5e0 *)game_text)->function_253b1a(string_handle);
				}
			}
		}
	}
}
// @retail 0x252c0a deleting c_class_252b72
// @retail 0x252c28 destructor c_class_252b72

// @retail 0x252c5e
void c_class_252b72::v2()
{
	g_470b18 = NONE;
	((c_widget *)this)->c_widget::v10();
}

c_class_1473c9 *__stdcall function_253185(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2531c9(s_screen_parameters *parameters);

// @retail 0x253185
c_class_1473c9 *__stdcall function_253185(s_screen_parameters *parameters)
{
	c_network_squad_browser_screen *screen = new c_network_squad_browser_screen(parameters->a, parameters->b, parameters->user_flags, false);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2531c9
c_class_1473c9 *__stdcall function_2531c9(s_screen_parameters *parameters)
{
	c_network_squad_browser_screen *screen = new c_network_squad_browser_screen(parameters->a, parameters->b, parameters->user_flags, true);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x253221
c_network_squad_browser_screen::c_network_squad_browser_screen(long a, long b, word user_flags, bool alternate) :
	c_screen_with_menu(0xd, a, b, user_flags, &list),
	list(user_flags, alternate)
{
	this->alternate = alternate;
	value93d = false;
	value93e = false;
}

// @retail 0x253268 deleting c_network_squad_browser_screen
// @retail 0x253286 destructor c_network_squad_browser_screen

// @retail 0x25320d
screen_load_proc c_network_squad_browser_screen::get_load_proc()
{
	return alternate ? function_2531c9 : function_253185;
}


long function_1480ff(long screen_id);

// @retail 0x251aa6
void c_matchmaking_screen::v18(void *parameters)
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

	build(&layout);
	c_class_1a2c81::v1();
}

bool function_592f0(void);
byte function_199eaa(void);
bool function_199e7e(byte value);
/* retail inlined the window test of
   channel 3, index 4 */
// @retail 0x2507dc
void function_2507dc(void)
{
	if (function_592f0() && function_199eaa() && !function_1473b6(&g_54d598.windows_3[4]))
	{
		function_199e7e(0);
	}
}

/* ---- player slot and session helpers (0x2510e1..0x25142f) ---- */

byte *network_session_interface_get_data_4db0(void);
bool function_19a84e(long *a, long *b);
long function_19989d(void);
void function_19a864(void);
bool function_199967(void);
long function_199d7c(void);
long network_session_manager_get_value49ac(void);
short player_slot_count_signed_in_non_guests(void);
short player_slot_count_active(void);
bool function_1900a5(long player);
void function_199e3c(long controller);
short function_1900ff(long controller);
long function_199f34(void);
bool function_19a179(long player_index);
bool function_19a7e9(long controller, long value);
c_class_1473c9 *window_manager_open_channel_3_screen(word user_flags, long load);
c_class_1473c9 *__stdcall function_2b8536(s_screen_parameters *parameters);

/* a session entry: per-mode counts at +0xc54 */
struct s_session_entry_view
{
	byte unknown00[0xc54];
	byte counts[16];
};

struct s_entry_c;
s_entry_c *function_19c5f0(long key);

/* whether the session's settings are valid */
// @retail 0x2510e1
bool function_2510e1(void)
{
	bool result = false;
	byte *data = network_session_interface_get_data_4db0();
	long a;
	long b;

	if (function_19a84e(&a, &b) && data)
	{
		if (a != NONE)
		{
			result = true;
		}
		else
		{
			long index = *(long *)(data + 0x44);

			if (index >= 1 && index <= 9)
			{
				result = true;
			}
		}
	}
	return result;
}

// @retail 0x251121
bool function_251121(void)
{
	switch (function_19989d())
	{
	case 1:
	case 3:
	case 5:
		function_19a864();
		break;
	}
	return true;
}

/* the count of the session's current entry, at least one */
// @retail 0x251139
long function_251139(void)
{
	if (function_2510e1())
	{
		byte *data = network_session_interface_get_data_4db0();
		long a;
		long b;
		s_session_entry_view *entry;

		function_19a84e(&b, &a);
		entry = (s_session_entry_view *)function_19c5f0(a);
		if (entry)
		{
			long index = *(long *)(data + 0x44);

			if (index >= 1 && index <= 9)
			{
				long count = entry->counts[index];

				if (count > 1)
				{
					return count;
				}
			}
		}
	}
	return 1;
}

// @retail 0x251188
bool function_251188(long value)
{
	bool result = false;

	switch (value)
	{
	case 2:
	case 5:
	case 6:
	case 7:
	case 11:
	case 12:
	case 13:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 25:
		result = true;
		break;
	}
	return result;
}

/* asks the player to sign in */
// @retail 0x2512d3
void function_2512d3(long controller)
{
	dialog_ok_show(3, 0x88, 4, (word)(1 << controller), 0, 0);
}

// @retail 0x25122f
void function_25122f(long controller)
{
	function_199f34();
	function_19a7e9(controller, 10);
}

// @retail 0x2512ec
void __stdcall function_2512ec(long player, bool signed_in)
{
	function_25122f(player);
}

// @retail 0x25132e
bool __stdcall function_25132e(long controller)
{
	function_25122f(controller);
	function_199e3c(controller);
	return true;
}

// @retail 0x251345
bool __stdcall function_251345(long controller)
{
	function_25122f(controller);
	function_19a179(NONE);
	function_199e3c(controller);
	return true;
}

// @retail 0x251242
void function_251242(long controller)
{
	if (player_slot_count_signed_in_non_guests() == 1 && !function_1900a5(controller))
	{
		function_199e3c(controller);
	}
	else
	{
		s_player_slot_profile *profile = player_slot_profile_get(controller);

		profile->callback = function_2512ec;
		profile->sign_out();
	}
}

// @retail 0x251284
void function_251284(long controller)
{
	if (player_slot_count_active() == 1)
	{
		dialog_choice_show(3, 0x8b, 4, (1 << controller), function_25132e, 0, 0);
	}
	else
	{
		player_slot_profile_get(controller)->show_dialog(function_2512ec, 0x8b);
	}
}

// @retail 0x2512f8
void function_2512f8(long controller, bool choice)
{
	if (choice)
	{
		dialog_choice_show(3, 0x8b, 4, (1 << controller), function_251345, 0, 0);
	}
	else
	{
		window_manager_open_channel_3_screen((word)(1 << controller), (long)function_2b8536);
	}
}

// @retail 0x2511b6
void function_2511b6(long controller)
{
	long count = player_slot_count_signed_in_non_guests();
	long players = function_199f34();

	if (function_1900a5(controller))
	{
		function_251242(controller);
	}
	else if (count == 1)
	{
		if (players > count)
		{
			if (function_592f0())
			{
				function_2512f8(controller, players == 2);
			}
			else
			{
				function_25122f(controller);
				if (function_1900ff(controller) > 0)
				{
					function_2512d3(controller);
				}
				else
				{
					function_251284(controller);
				}
			}
		}
		else
		{
			function_251242(controller);
		}
	}
	else if (function_1900ff(controller) > 0)
	{
		function_2512d3(controller);
	}
	else
	{
		function_251242(controller);
	}
}

struct s_session_player_view
{
	byte unknown00[0x48];
	byte flags;
};

// @retail 0x251364
long function_251364(s_session_player_view *player)
{
	if (player && (player->flags & 1) && function_199967())
	{
		return 1;
	}
	return 0;
}

struct s_name_buffer;
bool __stdcall function_8ca70(long mode, s_name_buffer *output);
void function_08cc20(s_name_buffer *buffer, wchar_t const *name);
bool function_199dc9(long a, long b, long c);
long function_75890(long time);

// @retail 0x251f8d
void function_251f8d(c_matchmaking_screen *screen)
{
    long maximum = 0;
    long player_count = 0;
    bool capacity;
    {
        long unused;
        capacity = function_199dc9((long)&player_count, (long)&unused, (long)&maximum);
    }
    c_text_widget_45a5e0 *title = (c_text_widget_45a5e0 *)screen->find_child(6, 2, false);
    c_text_widget_45a5e0 *status = (c_text_widget_45a5e0 *)screen->find_child(6, 5, false);
    c_text_widget_45a5e0 *message = (c_text_widget_45a5e0 *)screen->find_child(6, 7, false);
    if (title)
        title->function_253b1a(screen->get_title());
    if (function_75890(screen->value61c) > 15000)
    {
        screen->value620 = false;
        screen->value61c = function_75870();
    }
    if (!screen->value620)
    {
        word buffer[0x100];
        buffer[0] = 0;
        if (function_8ca70(1, (s_name_buffer *)buffer))
            function_08cc20((s_name_buffer *)&screen->value1222, (wchar_t const *)buffer);
        screen->value620 = true;
    }
    if (message && (long)function_217b70((word *)&screen->value1222) > 0)
        message->function_22f52e()->set_text((word *)&screen->value1222);
    if (status)
    {
        if (capacity)
        {
            long limit = maximum;
            if (player_count >= limit)
            {
                status->function_253b1a(0x11000762);
                status->value6e = true;
            }
            else
            {
                struct s_string_buffer
                {
                    word text[0x100];
                    void load(c_widget *screen, long handle) { screen->function_230134(handle, text); }
                } format;
                word buffer[0x100];
                buffer[0] = 0;
                format.text[0] = 0;
                format.load((c_widget *)screen, 0xe000761);
                function_1630e0(buffer, format.text, limit - player_count);
                status->function_22f52e()->set_text(buffer);
                status->value6e = true;
            }
        }
        else
            status->value6e = false;
    }
}

// @retail 0x25142f
long function_25142f(void)
{
	long result = 0;

	switch (function_199d7c())
	{
	case 4:
		result = 1;
		break;
	case 6:
		result = 2;
		break;
	case 7:
		result = (network_session_manager_get_value49ac() != NONE) + 2;
		break;
	case 8:
		result = 3;
		break;
	}
	return result;
}

/* ---- the matchmaking list's items (0x251384..0x2517e2) ---- */

bool function_199dc9(long a, long b, long c);
long function_199ebc(void);
long function_199ef8(void);
bool function_19a0c5(void);
bool function_19a127(void);
bool function_19a951(long player_index);
bool function_19a9b4(long player_index);
byte *function_19aaa5(long player_index);
byte *function_19ab0e(long player_index);
void function_24c0c4(c_widget *widget);
void function_2b01a2(long value, s_widget_item *item);
void function_2b01b5(s_widget_item *item, short value);
long ui_definition_find_range(long value);
void function_22f042(s_widget_item *items, c_class_1a2c81 *widget, long count);

/* a player as the matchmaking list shows it */
struct s_matchmaking_player
{
	byte unknown00[0x40];
	dword value40[4];
	byte unknown50[0x7c - 0x50];
	char team;
	byte unknown7d;
	char value7e;
	byte unknown7f[0x81 - 0x7f];
	char value81;
};

/* a datum of the list: a player, 16 for an empty slot or NONE */
struct s_matchmaking_datum
{
	short salt;
	short player;
};

word *unicode_string_append(word *destination, const word *source, long maximum_count);
s_screen_definition *function_22f871(c_class_1473c9 *screen);
void function_1a0180(long tag_index, long string_handle, word *buffer);

/* the names of the session's teams (each once), joined by the screen's
   separator string */
// @retail 0x251c91
void function_251c91(c_class_1473c9 *screen, word *buffer)
{
	word separator[0x100];
	bool team_listed[16] = { false };
	bool first = true;

	separator[0] = 0;
	function_1a0180(function_22f871(screen)->string_list_index, 0x6000442, separator);
	for (long i = 0; i < 16; i++)
	{
		if (function_19a9b4(i))
		{
			s_matchmaking_player *player = (s_matchmaking_player *)function_19ab0e(i);

			if (!team_listed[player->team])
			{
				if (first)
				{
					unicode_string_append(buffer, (word const *)player->unknown50, 0x100);
					first = false;
				}
				else
				{
					unicode_string_append(buffer, (word const *)L" ", 0x100);
					buffer[0xff] = 0;
					unicode_string_append(buffer, separator, 0x100);
					buffer[0xff] = 0;
					unicode_string_append(buffer, (word const *)L" ", 0x100);
					buffer[0xff] = 0;
					unicode_string_append(buffer, (word const *)player->unknown50, 0x100);
				}
				buffer[0xff] = 0;
				team_listed[player->team] = true;
			}
		}
	}
}

/* by team, then by value7e (the larger first) */
// @retail 0x251384
int __cdecl matchmaking_compare_team_and_value(void const *a, void const *b)
{
	s_matchmaking_player *player_a = (s_matchmaking_player *)function_19ab0e(*(long const *)a);
	s_matchmaking_player *player_b = (s_matchmaking_player *)function_19ab0e(*(long const *)b);

	if (player_a->team > player_b->team)
	{
		return 1;
	}
	if (player_a->team < player_b->team)
	{
		return -1;
	}
	if (player_a->value7e < player_b->value7e)
	{
		return 1;
	}
	return player_a->value7e > player_b->value7e ? -1 : 0;
}

/* by team */
// @retail 0x2513c9
int __cdecl matchmaking_compare_team(void const *a, void const *b)
{
	s_matchmaking_player *player_a = (s_matchmaking_player *)function_19ab0e(*(long const *)a);
	s_matchmaking_player *player_b = (s_matchmaking_player *)function_19ab0e(*(long const *)b);

	if (player_a->team > player_b->team)
	{
		return 1;
	}
	return player_a->team < player_b->team ? -1 : 0;
}

/* by value7e (the larger first) */
// @retail 0x2513fc
int __cdecl matchmaking_compare_value(void const *a, void const *b)
{
	s_matchmaking_player *player_a = (s_matchmaking_player *)function_19ab0e(*(long const *)a);
	s_matchmaking_player *player_b = (s_matchmaking_player *)function_19ab0e(*(long const *)b);

	if (player_a->value7e < player_b->value7e)
	{
		return 1;
	}
	return player_a->value7e > player_b->value7e ? -1 : 0;
}

/* fills the list with the session's players, then the empty slots */
// score 83 (was 83): the fills are pointer loops (retail: guarded rep stosd from an index loop) and the
// index_count/item_count slots are swapped; branch two gives EDI to the 0x199ef8 result and keeps
// index_count in memory (`inc dword ptr`), where retail keeps index_count in EDI through both scans and
// reloads player_count from its slot; that asymmetry is the lead for the slot swap and the fills.
// Earlier first line: extra frame dword, count homes and counted tail stores remain;
// common guarded fill 172; common fill/shared query 166; definite comparator 124;
// byte selected count 145; signed-char predicates 116.
// frame 0x58 versus 0x54; query/count homes, scalar tail stores and final row registers differ.
// Initial slot memset 187; unsigned selected count 116; query failure guard 136; comparator default 124; native secondary result 140.
// Guarded fills with current cursor/sort 197, with shared query 195; short mode 125; byte mode 121; selected-end tests 158.
// Earlier: extra count home and scalar tail fills still differ.
// Standard range/count fills 166/164; first/second counted fill 164/166;
// first/second/both maxima 135/124/159; counted fill with maxima 143.
// extra query frame dword, count homes and scalar tail fills remain.
// Row-total lexical rank 116; independent query storage and renamed combination 160;
// actual row reference, short row result and native index array 116; native predicates and query-success local 136.
// Earlier: query/count homes and scalar tails remain. Shared query with
// first/second/both standard maxima 143/117/158; indexed rows 135, plus first/both maxima
// 150/145; shared query default at entry 142, plus both maxima 151.
// Extra query dword, count homes and scalar tail fills remain. Branch
// player/selection/both initialization 130/164/164f; first/second/row while loops 116f; retained row pool
// 143f; shared checked row accessor 118f. Branch selection restores frame 0x54 but regresses.
// Earlier: query/count homes and scalar tail fills still differ.
// First/second/both maximum equality 116/116/116; values-first comparator 115 (dropped);
// comparator branches/storage 116/116; factored mode 160; pre-allocation row value 136.
// Earlier: query/count homes and tail-fill schedule still differ.
// first word scan 164f; second word scan 135f.
// both word scans 155f; byte scans 155f.
// signed byte scans 155f; early comparator 164f.
// early comparator/word scans 166f; early comparator/byte scans 166f.
// extra query home, selected-count homes and scalar tail fills remain.
// Completed first/second/both query totals 145/138/159; rejected first/second/both scans 116;
// open-row default 115 (dropped); completed totals with open-row default 158.
// Earlier: query/count homes and scalar tail fills still differ.
// Branch-shared session count 138; with library/range tails 201/160;
// selection initialization 162; completed total 134; word scans 160; comparator/open-row forms 139/137.
// Earlier: index fills as `for (i = index_count; i < 16; i++)` give retail's guarded
// rep stosd 162; with item_count shared by the mode-one query (frame 0x54) 161; plus indices[i]
// rows without the cursor 171; block-scoped fill index 161/162. Those keep index_count in EBX for
// the whole body and move mode to the stack, where retail keeps mode in EBX, index_count in EDI
// only in the scans, and reloads player_count after calls (its address escapes in retail).
// J185: existing query noinline boundary, alone/shared output/indexed tails/both 116/123/162/161;
// native byte query return at all declarations, same four forms 116/123/162/161.
// All query declarations, provider source and original target body restored; no new checked loss beyond the preview.
// Round 256: Compaction candidate before predicate in first/second/both scans 119/118/121; with shared
// mode-one query 127/125/129; both scans with indexed tails, separate/shared query 160/159. All
// restored.
// Round 86: mode-one query through item_count (retail's [ebp-0xc] in both branches) gives frame 0x54 but
// item_count/index_count slots swap and scores 123; renames of either 123, declaration orders 123/123/123;
// with index fills as for loops (retail's guarded rep stosd) 161, indices[i] rows 133/135, all three 171;
// maximum spelled `q >= count ? q : count` 116 (same code) / 123 with the shared query; mode-one maximum
// as an if 123.
// Round 303: branch-owned first/second/both working collection counts 153/153/158;
// both with definite selected result 158, indexed tails 159, shared query 164,
// indexed tails/shared query 162, all three 162. Original counts and complete body restored.
// Round 447: Native query output-pointer contracts, seven parameter subsets; all with shared mode-one output storage
// 116/116/116/116/116/116/116/123. Extra query frame dword, count homes and scalar tail fills remain.
// Original executable source and all trial helper/interface changes restored.
// Round 32 (lane 11): the tail rows through a pool array pointer read once per datum, indexed in each arm
// (retail's three word stores, hoisted 0xffff, `or word, -1`) with `indices[i]` and no cursor 88; plus the
// query result in its own local and an if/else maximum (retail stores count first) in branch one 86, in
// both branches through item_count (frame 0x54) 83 (kept). On the earlier forms: index for-loop fills,
// mode-one query through item_count, indices[i] tail 171 (index_count EBX all through, mode on the stack);
// that with the fill index scoped/per-loop i/mode first/i first 171 each, `else if (i < player_count)` 173;
// scan+fill as a force-inline helper taking the count by address 171 (inline: out of line, 243); rows
// written in full in each arm on the cursor/indexed/t1 forms 144/145/172 (172 with five declaration
// orders). On the kept form: index for-loop fills 172; the three qsort calls written out (retail's
// tail-merged pushes) 119 (one extra reload shifts the rest); both 174; index_count/item_count renamed
// (aaa/zzz) 83/83/83 (slots do not move). Compile-only, the index fill always gives index_count a
// whole-body register and homes mode (216 combinations of fill/qsort/scan/maximum/loop-index/qsort
// placement/declaration order); only a pointer `<` fill keeps retail's homes, but as a pointer rep stosd.
// Round 136 (lane 4): memset fills 164 (stosd + stosb, index_count in EBX); the mode-3 qsort block inside branch two 131
// (430); `indices[index_count] = i; index_count++;` 83; declarations index/player/indices/item 84, item/player/index/
// indices, indices/player/index/item, player/item/index/indices 83 each; pointer `<` fills 154 (index_count in EDI in
// both scans as retail, but the 0x199ef8 result takes EBX). A one-link harness with the callee files is not exact here.
// Round 35 (lane 11, fast): the second branch's fill alone as `for (long j = index_count; j < 16; j++)` 107
// (that branch then matches retail: index_count in EDI, guarded rep stosd, EDI reloaded; but the
// 0x199ef8 result takes EBX there and mode moves to the stack, frame 0x58); with mode declared first 110;
// the first branch's fill alone 169; both with the three qsort calls written out 169/163 (420 bytes).
// @retail 0x2514fd
void c_matchmaking_list::v3()
{
	long player_count = 0;
	long index_count = 0;
	long indices[16];
	long item_count;
	long mode = function_25142f();
	bool teams = function_19a0c5();
	bool values = function_19a127();
	long i;

	if (mode != 3 && (mode < 1 || teams))
	{
		long count = function_199ebc();
		long maximum;

		item_count = 0;
		maximum = function_199dc9((long)&player_count, (long)&item_count, 0) ? item_count : 0;
		if (count > maximum)
		{
			item_count = count;
		}
		else
		{
			item_count = maximum;
		}
		for (i = 0; i < 16; i++)
		{
			if (function_19a951(i))
			{
				indices[index_count++] = i;
			}
		}
		for (long *empty = indices + index_count; empty != indices + 16; ++empty)
			*empty = NONE;
	}
	else
	{
		player_count = function_199ef8();
		for (i = 0; i < 16; i++)
		{
			if (function_19a9b4(i))
			{
				indices[index_count++] = i;
			}
		}
		for (long *empty = indices + index_count; empty != indices + 16; ++empty)
			*empty = NONE;
		if (mode == 1)
		{
			long maximum;

			item_count = 0;
			maximum = function_199dc9(0, (long)&item_count, 0) ? item_count : 0;
			if (player_count > maximum)
			{
				item_count = player_count;
			}
			else
			{
				item_count = maximum;
			}
		}
		else
		{
			item_count = player_count;
		}
	}
	if (mode == 3)
	{
		int (__cdecl *compare)(void const *, void const *) = NULL;
		if (teams)
			compare = values ? matchmaking_compare_team_and_value : matchmaking_compare_value;
		else if (values)
			compare = matchmaking_compare_team;
		if (compare)
			qsort(indices, index_count, sizeof(long), compare);
	}
	record_pool_release_all(data);
	for (i = 0; i < item_count; i++)
	{
		long datum_index = record_pool_allocate(data);

		if (datum_index != NONE)
		{
			s_matchmaking_datum *datums = (s_matchmaking_datum *)data->data;

			if (i < index_count)
			{
				datums[datum_index & 0xffff].player = (short)indices[i];
			}
			else if (i >= player_count)
			{
				datums[datum_index & 0xffff].player = NONE;
			}
			else
			{
				datums[datum_index & 0xffff].player = 16;
			}
		}
	}
	function_24c0c4((c_widget *)this);
	((c_widget *)this)->c_widget::v11();
}

/* an empty slot that a player may still fill */
// @retail 0x251703
void show_open_slot(c_matchmaking_list *list, c_class_1a2c81 *item)
{
	c_matchmaking_list **list_reference = &list;

	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)item->find_child(6, 0, false);
	c_class_1a2c81 *bitmap = item->find_child(10, 0, false)->find_child(8, 1, false);

	function_2b0a14((s_widget_view_2b0a *)bitmap, 1);
	text->value6e = true;
	text->function_253b1a(0xe00075e);
	function_251963(item);
	s_widget_item definition;
	definition.value5e = true;
	definition.flags = 0x20;
	function_2b01a2(NONE, &definition);
	function_22f042(&definition, item, 1);
}

/* an empty slot */
// @retail 0x251778
void show_empty_slot(c_matchmaking_list *list, c_class_1a2c81 *item)
{
	c_matchmaking_list **list_reference = &list;
	c_class_1a2c81 *text = item->find_child(6, 0, false);
	c_class_1a2c81 *bitmap = item->find_child(10, 0, false)->find_child(8, 1, false);

	function_2b0a14((s_widget_view_2b0a *)bitmap, 0);
	text->value6e = false;
	function_251963(item);
	s_widget_item definition;
	definition.value5e = true;
	definition.flags = 0x20;
	function_2b01a2(NONE, &definition);
	function_22f042(&definition, item, 1);
}

/* a player's slot: name, emblem, team and voice */
// @retail 0x2517e2
void show_player(c_matchmaking_list *list, c_class_1a2c81 *item, long player_index)
{
	c_matchmaking_list **list_reference = &list;
	c_class_1a2c81 *text = item->find_child(6, 0, false);
	c_class_1a2c81 *models = item->find_child(10, 0, false);
	c_class_1a2c81 *bitmap1 = models->find_child(8, 1, false);
	c_class_1a2c81 *bitmap2 = models->find_child(8, 2, false);
	c_class_1a2c81 *bitmap3 = models->find_child(8, 3, false);
	long mode = function_25142f();
	s_matchmaking_player *player;

	text->value6e = false;
	bitmap1->value6e = false;
	bitmap3->value6e = false;
	bitmap2->value6e = true;
	if (mode != 3 && (mode < 1 || function_19a0c5()))
	{
		if (!function_19a951(player_index))
		{
			goto empty;
		}
		player = (s_matchmaking_player *)function_19aaa5(player_index);
	}
	else
	{
		if (!function_19a9b4(player_index))
		{
			goto empty;
		}
		player = (s_matchmaking_player *)function_19ab0e(player_index);
	}
	if (player)
	{
		s_widget_item definition;

		definition.flags = 0;
		if (function_19a0c5())
		{
			function_2b01a2(player->value7e, &definition);
		}
		else
		{
			function_2b01b5(&definition, (short)ui_definition_find_range(NONE));
		}
		memcpy(definition.value48, player->value40, sizeof(definition.value48));
		definition.flags |= 2;
		definition.value4 = (long)player;
		definition.flags |= 1;
		definition.value64 = player->value81;
		definition.flags |= 0x100;
		if (network_session_manager_get_value49ac() != NONE && function_19a127())
		{
			char team = player->team;

			if (team >= 0 && team < 8)
			{
				definition.value5c = team;
				definition.flags |= 4;
				definition.value5f = false;
				definition.flags |= 0x80;
			}
			else if (team == NONE)
			{
				definition.value5c = NONE;
				definition.flags |= 4;
				definition.value5f = true;
				definition.flags |= 0x80;
			}
		}
build:
		function_22f042(&definition, item, 1);
		function_251977(item, player_index);
		return;
	}
empty:
	show_empty_slot(list, item);
}

// @retail 0x2516b3
void c_matchmaking_list::v20(c_class_1a2c81 *item, long unused)
{
	long datum_index = widget_item(item)->value70;

	if (datum_index != NONE)
	{
		long player = ((s_matchmaking_datum *)data->data)[datum_index & 0xffff].player;

		if (player >= 0 && player < 16)
		{
			show_player(this, item, player);
		}
		else if (player == 16)
		{
			show_open_slot(this, item);
		}
		else if (player == NONE)
		{
			show_empty_slot(this, item);
		}
	}
}

/* ---- the matchmaking screen (0x251afa..0x25217b) ---- */

long function_75890(long time); /* 0x75890, unknown_075870.cpp */
long function_199f6d(void);
void network_session_manager_request_mode_acknowledge(void);
void function_19a942(void);

/* the title of the matchmaking state */
// @retail 0x251bff
long c_matchmaking_screen::get_title()
{
	switch (value610)
	{
	case 0:
		return 0x1400043a;
	case 1:
		return 0x1200043b;
	case 2:
		return 0x1300043c;
	case 3:
		return 0x1300043d;
	case 4:
		return 0xe00043e;
	case 5:
		return 0xc00043f;
	case 6:
		return 0xd000440;
	case 7:
		return 0xd000440;
	case 8:
		return 0x16000441;
	}
	return 0;
}

// @retail 0x251c68
void function_251c68(c_matchmaking_screen *screen)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)screen->find_child(6, 2, false);

	if (text)
	{
		text->function_253b1a(screen->get_title());
	}
}

/* the dialog's first choice leaves the matchmaking */
// @retail 0x252150
bool __stdcall function_252150(long controller)
{
	g_51ec99 = true;
	network_session_manager_request_mode_acknowledge();
	function_19a942();
	return true;
}

/* the dialog closes by itself once the matchmaking is past its search */
// @retail 0x252166
bool __stdcall function_252166(c_class_1473c9 *screen, long dialog_id)
{
	bool result = false;

	if (function_25142f() >= 2)
	{
		result = true;
	}
	return result;
}

/* asks whether to leave the matchmaking (not for the first minute once it
   has found a game) */
// @retail 0x25217b
void function_25217b(c_matchmaking_screen *screen, s_widget_event *event)
{
	bool show_dialog = function_25142f() < 2 || function_75890(screen->value614) >= 60000;
	if (show_dialog)
	{
		long dialog_id = (function_199ebc() <= function_199f6d()) + 0x8c;
		long user_flags = event ? 1 << event->controller_index : (short)function_1901fc();

		dialog_choice_show(3, dialog_id, 4, user_flags, function_252150, 0, function_252166);
	}
	else
	{
		function_236299(2);
	}
}

// @retail 0x251afa
bool c_matchmaking_screen::v10(s_widget_event *event)
{
	if (event->type == 5)
	{
		switch (event->param)
		{
		case 1:
		case 13:
			function_25217b(this, event);
			return true;
		case 3:
			if (function_25142f() >= 2)
			{
				return true;
			}
			break;
		}
	}
	return c_class_1473c9::v10(event);
}

/* ---- screen 0x24fd74's teams (0x250a43..0x250f3a) ---- */

long function_148222(long channel, long index, long screen_id);
void function_148241(long channel, long index, long screen_id);
bool function_19a902(void);
long player_slot_get_team(long index);
long player_slot_iterate_next(long value);
long function_19aa17(long value);
void function_18fe9e(long index);

/* a player slot's team (+0x1fc) */
struct s_player_slot_team_view
{
	byte unknown000[0x1fc];
	long team;
	byte unknown200[0xc70 - 0x200];
};

/* a bitmap index for the session's value at +0x44 */
// @retail 0x250a43
void function_250a43(byte *data, c_class_1a2c81 *bitmap)
{
	short index;

	switch (*(long *)(data + 0x44))
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
		index = 10;
		break;
	}
	if (bitmap)
	{
		function_2b0a14((s_widget_view_2b0a *)bitmap, index);
		bitmap->value6e = true;
	}
}

/* closes channel 3's settings screens (10, 0x14, 0x15) while 0x19a902 holds */
// @retail 0x250eb7
void c_session_lobby_screen::function_250eb7()
{
	if (function_19a902() && ((byte)function_148222(3, 4, 10) || (byte)function_148222(3, 4, 0x14) || (byte)function_148222(3, 4, 0x15)))
	{
		if (!value813)
		{
			function_148241(3, 4, 10);
			function_148241(3, 4, 0x14);
			function_148241(3, 4, 0x15);
			function_236299(2);
			value813 = true;
		}
	}
	else
	{
		value813 = false;
	}
}

// @retail 0x250cda
void c_session_lobby_screen::function_250cda(long index, bool update)
{
	byte *data = network_session_interface_get_data_4db0();

	if (update)
	{
		if ((byte)function_251364((s_session_player_view *)data))
		{
			long player = function_19aa17(index);

			if (player != NONE)
			{
				teams[index].team = ((s_matchmaking_player *)function_19aaa5(player))->team;
				teams[index].valid = true;
			}
			else
			{
				teams[index].team = NONE;
				teams[index].valid = false;
			}
		}
	}
	else
	{
		teams[index].valid = false;
		if ((byte)function_251364((s_session_player_view *)data))
		{
			((s_player_slot_team_view *)g_54e8e0)[index].team = teams[index].team;
			function_18fe9e(index);
		}
	}
}

long function_251139(void);

/* the session's flags, as change_team reads them */
struct s_session_flags_view
{
	byte unknown00[0x48];
	dword flag0 : 1;
	dword flag1 : 1;
	dword flag2 : 1;
	dword flag3 : 1;
	dword flag4 : 1;
	dword flag5 : 1;
};

/* steps a valid slot's team by delta, wrapping around the team count (to no
   team as well when the session allows it) */
// @retail 0x250d5c
void c_session_lobby_screen::change_team(long index, long delta)
{
	if (teams[index].valid)
	{
		short *team = &teams[index].team;
		long value = *team + delta;
		long count = function_251139();
		s_session_flags_view *data = (s_session_flags_view *)network_session_interface_get_data_4db0();
		long minimum = 0;

		if (data && TEST_FIELD_BIT(data->flag5))
		{
			minimum = NONE;
		}
		if (value < minimum)
		{
			value = count - 1;
		}
		else if (value >= count)
		{
			value = minimum;
		}
		*team = (short)value;
	}
}

// @retail 0x250f3a
void c_session_lobby_screen::function_250f3a(byte *data)
{
	if (value812 != (byte)function_251364((s_session_player_view *)data))
	{
		long index = 0;

		do
		{
			long team_count = 1;

			if (data && (((s_session_player_view *)data)->flags & 1))
			{
				team_count = 8;
				if (*(long *)(data + 0xb4))
				{
					team_count = *(long *)(data + 0xb4);
				}
			}
			teams[index].team = (short)(player_slot_get_team(index) % team_count);
			function_250cda(index, false);
			index = player_slot_iterate_next(index);
		}
		while (index != NONE);
	}
	value812 = (byte)function_251364((s_session_player_view *)data);
}

long network_session_manager_get_value49f8(void);
void network_session_manager_set_value49f8(long value);
bool function_592f0(void);
extern dword g_54d5b8;

/* shows the session's state in a dialog once; a live session leaves it three
   seconds later */
// score 36 (was 36): the cases' blocks sit in the other order, ours tests the
// state's value before the call while retail calls first and tests after, and
// the argument of the call is built in the other registers
// Our compiler lays the shared call out after the highest case (case blocks go in
// value order); only a separate call in case 4 puts it after case 4 as retail does,
// but that copy pushes 0xad and the state register instead of eax and 4: 81 (176
// bytes). Tried: that copy with the dialog set before the switch, inside the guard,
// or with a goto to the store 81; case 4 as `if (!g_51ec99) {...; break;} goto
// shown;` 37; `case 1:` with the default 36.
// Lane 11 r12: a NONE dialog for the flagged case 4 with `if (dialog != NONE)`
// before the call (assigned then cleared, a ternary, an if/else) 36/62/37: VC
// threads the test away and keeps the same layout; case 4 first or the default
// last 36 (identical code: case blocks are laid out the same whatever their
// source order).
// Round 186: dialog result at entry long/int/dword/unsigned-int, native-scope int/dword/unsigned-int
// 36 each; full unsigned state 36. Original body restored; shared tail placement remains.
// Round 76: case 4 outside 55; its timestamp copy 48; case 4 after the switch 57; case 4 with case 5
// or the high cases outside 55/55; early guarded case 4 71; entry dialog with case 4 outside 55;
// case 4 outside with its timestamp 60. Original body restored.
// Round 13 (lane 13): all 120 case orders lay out the same (compile-only); the call follows
// case 4 only when case 4 has its own call that VC tail-merges (compile-only, fastcall), but
// here case 4's call pushes a constant: 81 with `dialog` or 0xad. Also: the switch only picks
// the dialog and `state != 4 || !g_51ec99` guards the call 55; a forced-inline dialog helper
// returning NONE 85; case 4 `if (!g_51ec99) break;` then store and return 48.
// Round 24 (lane 15): cases 0/2/3 `goto done` to a single exit 36; the outer test inverted (the timer
// arm first) 58; the dialog declared at function scope 36; case 4 `if (!g_51ec99) break; goto shown;`
// 36; the call written in every case with the dialog local (case 4 under `if (!g_51ec99)`) 117 (the
// constants propagate into each call; also costs caller 0x24fd74). A compile-only model reproduces our
// layout for all of these and for an if/else-if chain (VC builds no compare tree from it): retail's
// layout looks like case 4 hosting the whole call/store/return with the other cases as stubs that
// jump back into it.
// Round 136 (lane 4): case 4 `if (g_51ec99) { store; return; } dialog = 0xad;` 53 (165); its own call under
// `if (!g_51ec99)` then the store and return 81 (call after case 4 as retail, pushes 0xad and the state register); the
// flag test before `dialog = 0xad` 37; `dialog = 0xad; if (!g_51ec99) break;` then its own store and return 48 (163);
// the call under the flag then a jump to the store, with `case 1:` 83 (178).
// @retail 0x250804
void c_session_lobby_screen::show_session_state()
{
	long state = network_session_manager_get_value49f8();
	dword shown_time = state_shown_time;

	if (!shown_time)
	{
		long dialog;

		switch (state)
		{
		case 0:
		case 2:
		case 3:
			return;
		case 5:
			dialog = 0xac;
			break;
		case 6:
		case 14:
		case 19:
			dialog = 0xae;
			break;
		default:
			dialog = 0xab;
			break;
		case 4:
			dialog = 0xad;
			if (g_51ec99)
			{
				goto shown;
			}
			break;
		}
		dialog_ok_show(3, dialog, 4, user_flags, 0, 0);
	shown:
		state_shown_time = g_54d5b8;
	}
	else if (function_592f0() && state && g_54d5b8 - shown_time >= 3000)
	{
		network_session_manager_set_value49f8(0);
	}
}

long function_19a8d0(void);
long function_19a2ce(real *progress);
bool function_19a78e(long controller, long countdown, long minimum);

/* follows the countdown's stage (with a sound at each new one) and, when
   asked, signs the first player with a slot in */
// @retail 0x250a8b
void c_session_lobby_screen::update_countdown(bool signed_in_needed)
{
	if (function_19a902())
	{
		long previous_stage = countdown_stage;

		countdown_stage = function_19a8d0();
		if (countdown_stage >= 0 && countdown_stage <= 3 && countdown_stage != previous_stage)
		{
			function_236299(10);
			if (countdown_stage == 3)
			{
				countdown_time = g_54d5b8;
			}
		}
	}
	else
	{
		countdown_stage = NONE;
	}
	if (signed_in_needed)
	{
		long state = function_19a2ce(NULL);

		if (state != 4 && (state <= 8 || state > 10))
		{
			for (long controller = 0; controller != NONE; controller = player_slot_iterate_next(controller))
			{
				if (((s_player_slot_view_04 *)g_54e8e0)[controller].value04 != NONE)
				{
					function_25122f(controller);
					break;
				}
			}
		}
	}
}

/* starts the countdown for the controller's player (with a sound when it
   cannot start) */
// @retail 0x25106c
void function_25106c(long controller)
{
	if (function_19a902())
	{
		function_199f34();
		function_19a78e(controller, 10, 3);
	}
	else
	{
		function_199f34();
		if (!function_19a78e(controller, 10, 3))
		{
			function_236299(2);
		}
	}
}

bool network_session_manager_session_unready(void);
bool network_session_interface_local_machine_is_host(void);
c_class_1473c9 *__stdcall function_2bbacb(s_screen_parameters *parameters);

// @retail 0x250fb3
void c_session_lobby_screen::handle_lobby_choice(s_controller_reference **controller, long *item)
{
	/* the callback's controller reference stays on the stack */
	s_controller_reference ***controller_reference = &controller;
	long player = (*controller)->controller_index;
	if (TEST_FIELD_BIT(((s_player_slot_sign_in_view *)g_54e8e0)[player].signed_in) && function_592f0())
	{
		if (teams[player].valid)
			function_250cda(player, false);
		if (network_session_interface_local_machine_is_host())
		{
			switch ((short)*item)
			{
			case 0:
				if (function_19a2ce(NULL) == 9)
					function_25106c((*controller)->controller_index);
				break;
			case 1:
			{
				s_screen_parameters parameters;
				parameters.field_c = 0;
				function_199e7e(1);
				function_25122f((*controller)->controller_index);
				function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)function_2bbacb);
				parameters.load(&parameters);
				break;
			}
			}
		}
	}
}

bool window_manager_channel_in_use(long channel);
long function_199fd6(void);
long function_19a279(void);
void function_1905bf(long controller, bool flag);

// @retail 0x250b1e
bool c_session_lobby_screen::v10(s_widget_event *event)
{
	bool result = false;

	if (!network_session_manager_session_unready() || window_manager_channel_in_use(1))
	{
		result = true;
		goto done;
	}
	else
	{
		long controller = event->controller_index;

		if (TEST_FIELD_BIT(((s_player_slot_sign_in_view *)g_54e8e0)[controller].signed_in))
		{
			switch (event->type)
			{
			case 2:
				if (teams[controller].valid)
				{
					change_team(controller, NONE);
					result = true;
				}
				break;
			case 4:
				if (teams[controller].valid)
				{
					change_team(controller, 1);
					result = true;
				}
				break;
			case 5:
				switch (event->param)
				{
				case 0:
				case 0xc:
					if (teams[controller].valid)
					{
						function_250cda(controller, false);
						result = true;
					}
					else if (function_19a902())
					{
						function_25106c(event->controller_index);
					}
					break;
				case 1:
				case 0xd:
					if (teams[controller].valid)
					{
						teams[event->controller_index].valid = false;
						teams[event->controller_index].team = NONE;
					}
					else if (function_592f0() && function_19a902())
					{
						function_25122f(event->controller_index);
					}
					else
					{
						function_2511b6(event->controller_index);
					}
					result = true;
					break;
				case 2:
					if (teams[controller].valid)
					{
						result = true;
						function_250cda(controller, false);
					}
					else
					{
						if (function_19a902())
						{
							function_199f34();
							if (function_19a7e9(event->controller_index, 10))
							{
								result = true;
								break;
							}
						}
						function_250cda(event->controller_index, true);
						result = true;
					}
					break;
				case 5:
					function_2510aa(controller);
					break;
				}
				break;
			}
		}
		else
		{
			if (event->type == 5 && (event->param == 0 || event->param == 0xc))
			{
				if (function_199fd6() > 0 && !function_19a902() && function_19a279() == 3)
				{
					bool online = function_6c7e0();

					function_1905bf(event->controller_index, online);
				}
				else
				{
					function_236299(2);
				}
			}
			result = true;
		}
	}
	if (!result)
	{
		result = c_class_1473c9::v10(event);
	}
done:
	return result;
}
/* ---- screen 0x24fd74's texts and bitmaps (0x250332..0x2508a8) ---- */

short network_session_interface_get_value_5dd0(void);
bool function_199971(void);
bool function_199994(void);
bool function_19a935(void);
struct s_entry_b;
s_entry_b *function_19c1f0(long key);
struct s_type_7ba8e9;
struct s_type_7ba8e9 *function_137550(long group_index, short bitmap_index);
void function_2b0a7b(s_widget_view_2b0a *widget, s_type_7ba8e9 *bitmap);
void function_2b0ad3(long index, s_widget_view_2b0a *widget, long bitmap_index);
void function_253c3a(long block_index, long index, c_text_widget_45a5e0 *widget);

/* the sessions' entries: their bitmap groups */
struct s_session_entry_b_view
{
	byte unknown00[0xc];
	long s_type_b8a6a0;
};

struct s_session_entry_c_view
{
	byte unknown00[8];
	long s_type_b8a6a0;
};

/* the postgame statistics' text and the session's state bitmap */
// @retail 0x2503c0
void function_2503c0(c_class_1a2c81 *screen)
{
	c_class_1a2c81 *text = screen->find_child(6, 0x2a, false);
	c_class_1a2c81 *bitmap = screen->find_child(8, 4, false);

	if (text)
	{
		text->value6e = g_51ec08 > 0;
	}
	if (bitmap)
	{
		long index;

		switch (network_session_interface_get_value_5dd0())
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
			return;
		}
		function_2b0ad3(0, (s_widget_view_2b0a *)bitmap, index);
	}
}

/* bitmap 2 shows the current session entry's bitmap group */
// @retail 0x250332
void function_250332(c_class_1a2c81 *screen)
{
	if (function_199971())
	{
		c_class_1a2c81 *bitmap = screen->find_child(8, 2, false);

		if (bitmap)
		{
			function_2b0a7b((s_widget_view_2b0a *)bitmap, 0);
		}
	}
	else
	{
		long a;
		long b;

		if (function_19a84e(&b, &a))
		{
			long s_type_b8a6a0;
			c_class_1a2c81 *bitmap;

			if (function_199994())
			{
				s_session_entry_b_view *entry = (s_session_entry_b_view *)function_19c1f0(a);

				if (!entry)
				{
					return;
				}
				s_type_b8a6a0 = entry->s_type_b8a6a0;
			}
			else
			{
				s_session_entry_c_view *entry = (s_session_entry_c_view *)function_19c5f0(a);

				if (!entry)
				{
					return;
				}
				s_type_b8a6a0 = entry->s_type_b8a6a0;
			}
			bitmap = screen->find_child(8, 2, false);
			if (bitmap)
			{
				function_2b0a7b((s_widget_view_2b0a *)bitmap, function_137550(s_type_b8a6a0, 0));
			}
		}
	}
}

/* a text by the mode, shown with a value block by 0x19a161 */
// @retail 0x2508a8
void c_session_lobby_screen::function_2508a8()
{
	long index;

	switch (mode)
	{
	case 0:
		index = 0x30;
		break;
	case 1:
		index = 0x2b;
		break;
	case 2:
		index = 0x31;
		break;
	default:
		return;
	}

	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)find_text((short)index);
	if (text)
	{
		bool show = function_19a935() && function_199f34() <= 1;

		text->value6e = show;
		if (show)
		{
			long state = function_19a161();
			if (state == 2)
				function_253c3a(1, 0, text);
			else if (state == 1)
				function_253c3a(1, 1, text);
			else if (function_19989d() != 6)
				function_253c3a(1, 2, text);
			else
				function_253c3a(1, 3, text);
		}
	}
}

/* the sixteen items of a list of players */
struct s_player_widget_items
{
	s_widget_item items[16];

	s_player_widget_items();
};

// @retail 0x2507c3
s_player_widget_items::s_player_widget_items()
{
}

/* the four texts and bitmaps of a slot of the lobby, by the screen's mode */
static __forceinline void lobby_slot_widget_indices(short mode, long slot, short *text_a, short *text_b, short *bitmap_a, short *bitmap_b)
{
	switch (mode)
	{
	case 1:
		*text_a = slot + 0x18;
		*text_b = slot + 8;
		*bitmap_a = slot + 5;
		*bitmap_b = slot + 0x15;
		break;
	case 0:
	case 2:
		*text_a = slot + 0x1d;
		*text_b = slot + 0xd;
		*bitmap_a = slot + 5;
		*bitmap_b = slot + 0x15;
		break;
	default:
		*text_a = NONE;
		*text_b = NONE;
		*bitmap_a = NONE;
		*bitmap_b = NONE;
		break;
	}
}

/* the index of the player among the four local players, or NONE */
static __forceinline long local_player_find(long const *local_list, long player)
{
	for (long i = 0; i < 4; i++)
	{
		if (player == local_list[i])
		{
			return i;
		}
	}
	return NONE;
}

/* fills the lobby's player list (players, count of them), with the local
   players' chosen teams; hides the slots past the count */
// score 95 (was 95): the locals live in the other stack slots (shown count, the first
// loop's player, the second loop's four widget indices), retail reloads players[i] after the
// voice-state query, and the last loop keeps the screen in edi and bitmap_a in ebx where
// ours keeps the screen in ebx and false in edi.
// Earlier: find_bitmap now takes a long like find_text (round 104); named pointer to
// players[i] in the first loop 111; renamed count, flag, player and slot locals 111.
// Round 184: shared count 301; result-first voice 129; native team byte 165; count/voice 307;
// scoped text/all receivers 111/111; late team-result constants 171; complete combined graph 307.
// Original count, receivers, widths and outputs restored.
// Promoted first-loop team with numeric/reverse and absent-first/last switch cases 238 at 915 bytes
// in all four orders; default-result initialization on each 234 at 913. Scalar homes stay
// displaced; original short team and switch restored.
// Complete four-output publication schedules 182/192/114/151/184/158/192/182; original helper stores restored.
// Round 77: second-loop element pointer 180; element pointers in both loops 180; counter before count 111;
// counter before query 111; final receiver address reads 111; voice parameter address reads 111; player
// pointer arithmetic 111; interleaved widget-helper parameters 111. Original body and helper restored.
// Round 104 (on the long find_bitmap, 95): a separate counter per loop 93 (gives retail's two element-
// pointer slots, ebp-8 and ebp-0x18); with it the four widget indices declared in reverse 93, shown count
// declared after the matchmaking flag 98, no_team before team_index 93, a screen copy for the last loop 93.
// Round 128: the four widget indices declared once at function scope for both loops 92 (in reverse 92);
// with a separate counter for the second loop 93, for the second and third loops 93; the separate
// counter alone 93. The players[i] reload after the voice query is retail's view of 0x53b40's side
// effects (ours keeps the value across the call), so it may sit outside this file.
// Round 21 (lane 14): the second loop's text_a renamed 95 (same code: names don't order these
// slots); the four widget indices written in place in the second loop without the helper 95 (same
// code: they are plain spilled locals); the first loop's search through local_player_find 256 (933
// bytes, the NONE test stays); a counter per loop with the search index declared per loop 93 (with
// the separate counters ours is -8 elem1/bitmap_b, -0xc player/no_team, -0x10 shown count, -0x14
// bitmap_a, +0xc text_b, +0x1c text_a; retail -8 elem1/bitmap_a, -0xc shown count, -0x10 text_b,
// -0x14 player/no_team, +0xc text_a, +0x1c bitmap_b); no_team from the char team 97. The 8 extra
// bytes are the last loop: ours keeps the screen in EBX for all five calls, retail gives EBX to
// bitmap_a and loads the screen into ECX, then EDI after text_b's call.
// Round 24 (lane 15): shown_count renamed to sort first / last 95 / 95 (same code: names don't order
// these spill slots); the third loop with its own counter 95; the third loop's indices declared
// bitmaps first 95; the search index declared once at function scope 95; a separate counter for the
// first loop only 93. Lead for the reload: 0x53b40 reaches 0x53c30 through 0x53de0 with an
// out-parameter, so retail's view of its side effects may differ from ours (src/network_voice.cpp).
// @retail 0x25042d
void function_25042d(c_session_lobby_screen *screen, long valid, s_session_flags_view *session, long *players, long *local_list, long count)
{
	long shown_count = count;
	bool in_matchmaking = function_199994();
	long i;

	if (valid != NONE)
	{
		s_player_widget_items list;

		for (i = 0; i < count; i++)
		{
			s_widget_item *item = &list.items[i];
			s_matchmaking_player *player = (s_matchmaking_player *)function_19aaa5(players[i]);
			long local;

			item->flags = 0;
			item->value4 = (long)player;
			item->flags |= 1;
			function_2b01a2(player->value7e, item);
			memcpy(item->value48, player->value40, sizeof(item->value48));
			item->flags |= 2;
			item->value64 = player->value81;
			item->flags |= 0x100;
			if (!function_199971() && !in_matchmaking && session && TEST_FIELD_BIT(session->flag0))
			{
				char team = player->team;

				if (team >= 0 && team < 8)
				{
					item->value5c = team;
					item->value5f = false;
					item->flags |= 0x84;
				}
				else if (team == NONE)
				{
					item->value5c = NONE;
					item->value5f = true;
					item->flags |= 0x84;
				}
			}
			for (local = 0; local < 4; local++)
			{
				if (players[i] == local_list[local])
				{
					if (screen->teams[local].valid)
					{
						short team = screen->teams[local].team;
						long name;

						item->value5c = team;
						item->value5f = team == NONE;
						item->flags |= 0x84;
						switch (team)
						{
						case 0:
							name = 0x500020e;
							break;
						case 1:
							name = 0x500020f;
							break;
						case 2:
							name = 0x5000210;
							break;
						case 3:
							name = 0x5000211;
							break;
						case 4:
							name = 0x5000212;
							break;
						case 5:
							name = 0x5000213;
							break;
						case 6:
							name = 0x5000214;
							break;
						case 7:
							name = 0x5000215;
							break;
						case NONE:
							name = 0x90001c6;
							break;
						default:
							name = 0;
							break;
						}
						item->value58 = name;
						item->flags |= 8;
					}
					break;
				}
			}
		}
		function_22f042(list.items, screen, count);
	}
	else
	{
		function_22f042(NULL, screen, 0);
		shown_count = 0;
	}

	for (i = 0; i < shown_count; i++)
	{
		s_matchmaking_player *player = (s_matchmaking_player *)function_19aaa5(players[i]);
		short text_a;
		short text_b;
		short bitmap_a;
		short bitmap_b;
		long team_index = NONE;
		bool no_team = false;
		color3f colors[4];
		long local;
		c_class_1a2c81 *bitmap;

		local = local_player_find(local_list, players[i]);
		lobby_slot_widget_indices(screen->mode, i, &text_a, &text_b, &bitmap_a, &bitmap_b);
		if (!in_matchmaking && session && TEST_FIELD_BIT(session->flag0))
		{
			char team = player->team;

			if (team >= 0 && team < 8)
			{
				team_index = team;
				no_team = team_index == NONE;
			}
		}
		function_7f790(team_index, no_team, (s_player_appearance const *)player->value40, colors);
		bitmap = screen->find_text(text_a);
		if (bitmap)
		{
			bitmap->value6e = false;
		}
		bitmap = screen->find_text(text_b);
		if (bitmap)
		{
			bitmap->value6e = false;
		}
		bitmap = screen->find_bitmap(bitmap_a);
		if (bitmap)
		{
			bitmap->value6e = false;
		}
		bitmap = screen->find_bitmap(bitmap_b);
		if (bitmap)
		{
			short state;

			if (voice_port_flag0_only(players[i]))
			{
				state = !function_53750(players[i]);
			}
			else
			{
				state = 2;
			}
			function_2b0a14((s_widget_view_2b0a *)bitmap, state);
			bitmap->value6e = true;
		}
	}
	for (i = shown_count; i < 0x10; i++)
	{
		short text_a;
		short text_b;
		short bitmap_a;
		short bitmap_b;

		lobby_slot_widget_indices(screen->mode, i, &text_a, &text_b, &bitmap_a, &bitmap_b);
		screen->set_child_value6e(6, text_a, false);
		screen->set_child_value6e(6, text_b, false);
		screen->set_child_value6e(8, bitmap_a, false);
		screen->set_child_value6e(8, bitmap_b, false);
		screen->set_child_value6e(9, (short)i, false);
	}
}

/* bitmap 10 shows the current session entry's bitmap group */
// @retail 0x2520ff
void function_2520ff(c_class_1a2c81 *screen)
{
	long a;
	long b;

	if (function_19a84e(&b, &a))
	{
		s_session_entry_c_view *entry = (s_session_entry_c_view *)function_19c5f0(a);

		if (entry)
		{
			long s_type_b8a6a0 = *(long const volatile *)&entry->s_type_b8a6a0;
			c_class_1a2c81 *bitmap = screen->find_child(8, 10, false);

			if (bitmap)
			{
				function_2b0a7b((s_widget_view_2b0a *)bitmap, function_137550(s_type_b8a6a0, 0));
			}
		}
	}
}

/* ---- the network squad browser (0x2530a4..0x253319) ---- */

void function_199b45(void);
/* the focused squad */
// @retail 0x2530a4
byte *function_2530a4(c_class_252b72 *list)
{
	byte *result = 0;
	long datum_index = list->get_focused_datum();

	if (datum_index != NONE)
	{
		s_network_squad_datum *datum = &((s_network_squad_datum *)list->data->data)[datum_index & 0xffff];

		if (!datum->create)
		{
			long index = datum->index;

			if (index != NONE && function_199ba5(index))
			{
				result = function_199bbf(index);
			}
		}
	}
	return result;
}

/* whether the list has no squads */
// @retail 0x252b5e
bool function_252b5e(c_class_252b72 *list)
{
	return !list->data || !list->data->actual_count;
}

// @retail 0x253319
void c_network_squad_browser_screen::v2()
{
	if (value93d)
	{
		function_199b45();
		value93d = false;
	}
	c_class_1a2c81::v2();
}

c_class_1473c9 *__stdcall function_252433(s_screen_parameters *parameters);

// @retail 0x2536a6
bool c_network_squad_browser_screen::v10(s_widget_event *event)
{
	if (event->type == 5)
	{
		switch (event->param)
		{
		case 1:
		case 13:
			long channel = v20();
			long index = v21();

			if (!function_148044(channel, index, 0x1e))
			{
				s_screen_parameters parameters;

				parameters.field_c = 0;
				function_149f49((s_message *)&parameters, 7, 0, 1 << event->controller_index, channel, index, (long)function_252433);
				parameters.load(&parameters);
			}
			return true;
		}
	}
	return c_class_1473c9::v10(event);
}

void function_199b33(bool flag);
s_record_pool *function_19c6a0();
struct s_entry_c;
s_entry_c *function_19c5f0(long key);
void function_23625d(long tag_index);

/* a map of the list function_19c6a0 returns, and its entry */
struct s_map_item_view
{
	long unknown00;
	long key;
};

struct s_map_entry_view
{
	byte unknown00[8];
	long bitmap_tag_index;
};

// @retail 0x25329b
void c_network_squad_browser_screen::v19()
{
	if (!value93d)
	{
		function_199b33(alternate);
		value93d = true;
	}
	list.searching = true;
	set_user_flags(function_1901fc());

	s_list_item_iterator iterator;

	iterator.iterator.data = function_19c6a0();
	iterator.iterator.index = NONE;
	iterator.iterator.datum_index = NONE;
	while (function_2b2327(&iterator))
	{
		s_map_entry_view *entry = (s_map_entry_view *)function_19c5f0(((s_map_item_view *)iterator.item)->key);

		if (entry && entry->bitmap_tag_index != NONE)
		{
			function_23625d(entry->bitmap_tag_index);
		}
	}
	c_class_1473c9::v19();
}

bool network_session_manager_session_unready(void);
bool network_session_manager_is_joining(void);
struct s_localized_name;
struct s_localized_short_name;
wchar_t *localized_name_get(s_localized_name *definition);
wchar_t *localized_short_name_get(s_localized_short_name *definition);

/* the player list's items: a player's name, appearance and team, each with
   the flag that says the item has it */
PRIVATE inline void squad_item_set_name(s_widget_item *item, word const *name)
{
	item->flags |= 1;
	item->value4 = (long)name;
}

PRIVATE inline void squad_item_set_appearance(s_widget_item *item, dword const *appearance)
{
	item->flags |= 2;
	memcpy(item->value48, appearance, sizeof(item->value48));
}

PRIVATE inline void squad_item_set_team(s_widget_item *item, short team)
{
	item->flags |= 4;
	item->value5c = team;
}

// score 61 (was 61): frame 0x754 agrees; the item-loop receiver reload (retail: once after the loop),
// row-loop text lookup (retail inlines it; widget/counter registers swapped), animation byte
// (retail movsx) and the final receiver reloads differ.
// Tried: category descending 237; title default first 244; bitmap index long 237; title widget scopes
// 237; separate player row counters 237; existing widget lookups 175; native animation bit 233; widget
// lookups animation bit 179; map text lookup 237; text lookup helpers 156; bitmap lookup helpers 164;
// native widget query indices 180; heading widget lookups 237; text helper query indices 164; item
// pointer walk 301; game result width 309; row counter short 247; direct map names 218; direct search
// result 172; bitmap exhaustive result fixed 189; offline index range 195; exhaustive bitmap long 189;
// exhaustive bitmap case order serial 189; exhaustive bitmap range serial 195; game result
// initialization 310; bitmap result guards 167; offline bitmap first 100; map widget lifetimes 164; all
// widget phase lifetimes 159; map lifetimes bitmap guards 175; selected squad role 156; search result
// from acceptance 156; offline native animation bit 115; offline native text index 116; offline row
// lifetimes 100; offline native bit index 120; name/appearance/team/all item operations force-inline
// 156 each; unsigned word rows 203; loop-scoped row widgets 157; bitmap category order 156; player-item
// array home name 156; native/promoted retained item count 100/100; count-tested item while 100;
// independent item admissions 100; name-only item continue 100; current array-item pointer 294;
// complete bitmap/title guards on the offline-first body 100/100; row generic query 235; direct text
// queries 235; game at use 322; team availability 294; native item flags 100; row game observations
// 314; row team availability 304; row native game 314; game-first bitmap selection 156 (878 bytes,
// closest structure); with the row text index passed uncast 164 (875); with a separate row counter
// 164 (identical); with an unconverted bit test 179; with a bool-converted signed bit test 177.
// complete bitmap/title guards on the offline-first body 100/100. Original body retained.
// Round 164: row generic query 235; direct text queries 235; game at use 322; team availability 294;
// native item flags 100; row game observations 314; row team availability 304; row native game 314.
// Round 166: unsigned rows 100; bitmap presence 106; absent text 100; row while 100; native
// animation shift 117; unsigned presence/animation 106/117; presence text animation 109;
// recovered-prefix game-first/query/unsigned/animation 143/171/171/170.
// Round 183: existing name/appearance/team payloads before validity bits, all seven nonempty
// combinations 100 each; all three with one current-item receiver 293. Original setter bodies
// and complete target restored; receiver reload, row query and animation homes remain.
// Round 204: Real source-table cursors: names 106; appearances 105; both 106; teams 106; names/teams 108;
// appearances/teams 105; all three 107; all three plus destination cursor 294 at 889 bytes. The seven
// source-only forms stay 876 bytes. Original indexed body, helper bodies and memcpy restored; receiver
// reload, row/query and animation roles remain.
// Round 75: typed appearance assignment 100; typed appearance cursor 105; separate display counter 100;
// publication screen local 100; row-only generic query 235 (item reload, row registers and movsx agree;
// the title constant carrier returns); with game-first, map generic query, moved title case 3, separate
// map widget or explicit heading receiver 237 each. Original field, helpers and complete body restored.
// Round 303: shared nullable map-name publication, four acquisition graphs 271 each;
// each with the generic row query 273. All original map publications, lookups and body restored.
// Round 313: Child-domain 4 with original/shared bitmap/generic text/both queries 112/104/116/110;
// child-domain 5 with those four APIs 112/104/120/112. Receiver, bitmap and animation gaps remain;
// original body restored.
// Round 128: the map bitmap block without its goto (the 0x137550 call written in both arms, which VC
// tail-merges) gives the same code, kept. Without the inline_depth pair 302 (the item constructor loop
// inlines); the row text index passed uncast 116; the row text through find_child directly 235 (row
// loop as retail, but this moves out of ESI and the constant 3 takes it); the map-name block without its
// goto 206; game-first bitmap selection 156. Diagnostic: with a constant index the row loop's find_text
// inlines as in retail, so the variable-index call stays out of line on find_text's own cost: lead,
// 0x22ee17 taking a short index (src/unknown_22e27b.cpp, another lane's file this round).
// Round 21 (lane 14): find_text(short) with its cast dropped (diagnostic) leaves the row call out of
// line (116) and breaks four matched callers (0x24ffc9, 0x250155, 0x2501c0, 0x2508a8): retail's takes a
// long, so the lead is closed; the row index uncast 116 (873 bytes, still called); game-first bitmap
// selection 156 (the dispatch is retail's, but ours hosts the shared index 1 and 3 blocks in the
// category switch where retail hosts them in the valuea8 switch); diagnostic without the find_text(3)
// lookup 327 (the row call stays out of line, so it isn't the inline budget); the row text through
// find_child directly 240; with row-scoped text/bitmap locals 235; row-scoped locals with find_text 100.
// Round 24 (lane 15): the setter call written at the end of each bitmap-index branch (category switch
// first, then the valuea8 switch, and the no-squad arm) 70 (VC hosts the shared index blocks in the
// first-created switch); with valuea8's case 3 before case 2 (the `pop edx` tail goes to the last-created
// push block) 61, kept. On it: the row text through find_child 161 (`this` leaves ESI); the animation
// bit as a plain signed shift 78 (folds to `test byte`); the row bitmap through find_bitmap 74; the row
// text index uncast 77; the searching flag stored directly 78.
// Round 136 (lane 4): retail's row lookup is `find_child(6, (short)(i + 4), false)` (every matched variable-index caller
// calls find_text out of line, and this function's bitmaps already use find_child): 161 (879; item loop, row loop and
// section 2 as retail, but section 1's bitmap takes ESI from `this` and the tail keeps `this` in ESI); on it: row-scoped
// text and bitmap / text / bitmap 171/161/171, sections 1-2 through find_child(6, 2/3) 161, declarations bitmap,text,i /
// i,text,bitmap / i,bitmap,text 161 each, find_bitmap for section 1/2/both 161, the animation bit as `((char)x >> 1) & 1`
// / `(x >> 1) & 1` / ANIMATION_FLAG of `(char)x` 161 (874) / 161 (874) / 161 (retail's `movsx byte` read is unique).
// Diagnostics without the item loop / the appearance copy: find_text still called (not the inline budget).
// @retail 0x253339
void c_network_squad_browser_screen::v3()
{
	s_network_squad *squad = (s_network_squad *)function_2530a4(&list);
	bool has_game = squad && squad->category;
	c_class_1a2c81 *text;
	c_class_1a2c81 *bitmap;
	long i;

	if (squad)
	{
		/* retail constructs the items with the out-of-line constructor */
#pragma inline_depth(0)
		s_widget_item items[16];
#pragma inline_depth()

		for (i = 0; i < squad->player_count; i++)
		{
			squad_item_set_name(&items[i], squad->player_names[i]);
			if (has_game)
			{
				squad_item_set_appearance(&items[i], squad->player_appearances[i]);
				if (squad->has_teams)
				{
					squad_item_set_team(&items[i], squad->player_teams[i]);
				}
			}
		}
		function_22f042(items, this, squad->player_count);
	}
	else
	{
		function_22f042(NULL, this, 0);
	}

	text = find_text(2);
	bitmap = find_child(8, 4, false);
	if (text)
	{
		long string_handle;

		if (squad)
		{
			if (has_game)
			{
				switch (squad->category)
				{
				case 1:
					string_handle = 0x1000010d;
					break;
				case 2:
					string_handle = 0x600010f;
					break;
				case 3:
					string_handle = 0x7000110;
					break;
				case 4:
					string_handle = 0x4000237;
					break;
				case 7:
					string_handle = 0xa000113;
					break;
				case 8:
					string_handle = 0xb000115;
					break;
				case 9:
					string_handle = 0x700010e;
					break;
				default:
					string_handle = 0x7000180;
					break;
				}
			}
			else
			{
				string_handle = 0x4000423;
			}
		}
		else
		{
			string_handle = function_252b5e(&list) ? 0x8000234 : 0x7000180;
		}
		((c_text_widget_45a5e0 *)text)->function_253b1a(string_handle);
	}
	if (bitmap)
	{
		short index = 10;

		if (squad)
		{
			if (has_game)
			{
				switch (squad->category)
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
				}
				function_2b0a14((s_widget_view_2b0a *)bitmap, index);
			}
			else
			{
				switch (squad->valuea8)
				{
				case 0:
					index = 0;
					break;
				case 1:
					index = 1;
					break;
				case 3:
					index = 3;
					break;
				case 2:
					index = 2;
					break;
				}
				function_2b0a14((s_widget_view_2b0a *)bitmap, index);
			}
		}
		else
		{
			function_2b0a14((s_widget_view_2b0a *)bitmap, index);
		}
	}

	text = find_text(3);
	bitmap = find_child(8, 1, false);
	if (text)
	{
		if (squad)
		{
			if (has_game)
			{
				s_localized_short_name *entry = (s_localized_short_name *)function_19c5f0(squad->map_key);

				if (!entry)
				{
					goto no_map_name;
				}
				wchar_t *name = localized_short_name_get(entry);

				text->function_22f52e()->set_text((word *)name);
			}
			else
			{
				s_localized_name *entry = (s_localized_name *)function_19c1f0(squad->map_key);

				if (!entry)
				{
					goto no_map_name;
				}
				wchar_t *name = localized_name_get(entry);

				text->function_22f52e()->set_text((word *)name);
			}
		}
		else
		{
		no_map_name:
			((c_text_widget_45a5e0 *)text)->function_253b1a(0x7000180);
		}
	}
	if (bitmap)
	{
		function_2b0a7b((s_widget_view_2b0a *)bitmap, 0);
		if (squad)
		{
			if (squad->category)
			{
				s_session_entry_c_view *entry = (s_session_entry_c_view *)function_19c5f0(squad->map_key);

				if (entry)
				{
					function_2b0a7b((s_widget_view_2b0a *)bitmap, function_137550(entry->s_type_b8a6a0, 0));
				}
			}
			else
			{
				s_session_entry_b_view *entry = (s_session_entry_b_view *)function_19c1f0(squad->map_key);

				if (entry)
				{
					function_2b0a7b((s_widget_view_2b0a *)bitmap, function_137550(entry->s_type_b8a6a0, 0));
				}
			}
		}
	}
	for (i = 0; i < sizeof(squad->player_names) / sizeof(squad->player_names[0]); i++)
	{
		text = find_text((short)(i + 4));
		bitmap = find_child(8, (short)(i + 5), false);
		if (text)
		{
			if (squad && i < squad->player_count)
			{
				text->value6e = true;
				text->function_22f52e()->set_text(squad->player_names[i]);
			}
			else
			{
				text->value6e = false;
			}
		}
		if (bitmap)
		{
			if (squad)
			{
				bitmap->value6e = i < squad->player_count;
			}
			else
			{
				bitmap->value6e = false;
			}
		}
	}
	bool searching = true;

	if (ANIMATION_FLAG(animation, 1) || network_session_manager_session_unready() || network_session_manager_is_joining())
	{
		searching = false;
	}
	list.searching = searching;
	c_class_1a2c81::v3();
}

long function_230195(s_screen_definition *definition, long block_index, long index);
bool function_19a107(void);

// @retail 0x2501c0
void function_2501c0(c_session_lobby_screen *screen, s_game_variant *variant)
{
    short indices[5];
    long extra;
    long i;
    switch (screen->mode)
    {
    case 0:
        for (i=0; i<5; ++i) indices[i]=(short)(i+6);
        extra=47;
        break;
    case 1:
        memset(indices, 0xff, sizeof(indices));
        extra=42;
        break;
    case 2:
        for (i=0; i<5; ++i) indices[i]=(short)(i+6);
        extra=47;
        break;
    default:
        memset(indices, 0xff, sizeof(indices));
        extra=NONE;
        break;
    }
    c_class_1a2c81 *settings[5];
    for (i=0; i<5; ++i)
        settings[i]=screen->find_text(indices[i]);
    c_class_1a2c81 *title=screen->find_child(6, 3, false);
    c_text_widget_45a5e0 *caption=(c_text_widget_45a5e0 *)screen->find_child(6, 4, false);
    c_class_1a2c81 *bitmap=screen->find_child(8, 4, false);
    c_class_1a2c81 *extra_text=screen->find_text((short)extra);
    if (variant)
    {
        s_screen_definition *definition=function_22f871(screen);
        if (title) title->value6e=true;
        if (caption) caption->function_253b1a(0xf0001df);
        function_250a43((byte *)variant, bitmap);
        if (definition)
        {
            word separator[0x100];
            separator[0]=0;
            if (definition->string_list_index!=NONE)
                function_1a0180(definition->string_list_index, function_230195(definition, 0, 0), separator);
            for (i=0; i<5; ++i)
                function_25092c(variant, i, settings[i], separator, screen->text[i].text);
        }
    }
    if (extra_text) extra_text->value6e=g_51ec08>0;
}

// @retail 0x251d84
void function_251d84(c_matchmaking_screen *screen)
{
    struct s_local_text
    {
        word text[0x100];
        word *data() { return text; }
    };
    byte *variant=network_session_interface_get_data_4db0();
    c_class_1a2c81 *title=screen->find_child(6, 2, false);
    c_class_1a2c81 *teams=screen->find_child(6, 13, false);
    c_class_1a2c81 *setting0=screen->find_child(6, 6, false);
    c_class_1a2c81 *setting1=screen->find_child(6, 7, false);
    c_class_1a2c81 *setting2=screen->find_child(6, 8, false);
    c_class_1a2c81 *setting3=screen->find_child(6, 9, false);
    c_class_1a2c81 *setting4=screen->find_child(6, 10, false);
    c_class_1a2c81 *setting5=screen->find_child(6, 11, false);
    c_text_widget_45a5e0 *caption=(c_text_widget_45a5e0 *)screen->find_child(6, 4, false);
    c_class_1a2c81 *bitmap=screen->find_child(8, 12, false);
    if (caption) caption->function_253b1a(0xf0001df);
    if (function_19a107())
    {
        s_local_text buffer;
        buffer.text[0]=0;
        function_251c91(screen, buffer.text);
        teams->function_22f52e()->set_text(buffer.data());
    }
    if (variant && setting5)
    {
        s_screen_definition *definition=function_22f871(screen);
        function_250a43(variant, bitmap);
        if (definition)
        {
            s_local_text separator;
            separator.text[0]=0;
            if (definition->string_list_index!=NONE)
                function_1a0180(definition->string_list_index, function_230195(definition, 0, 0), separator.text);
            function_25092c((s_game_variant *)variant, 0, setting0, separator.text, screen->texts[0].text);
            function_25092c((s_game_variant *)variant, 1, setting1, separator.text, screen->texts[1].text);
            function_25092c((s_game_variant *)variant, 2, setting2, separator.text, screen->texts[2].text);
            function_25092c((s_game_variant *)variant, 3, setting3, separator.text, screen->texts[3].text);
            function_25092c((s_game_variant *)variant, 4, setting4, separator.text, screen->texts[4].text);
            function_25092c((s_game_variant *)variant, 5, setting5, separator.text, screen->texts[5].text);
        }
    }
    if (title) title->value6e=network_session_manager_get_value49ac()!=NONE;
    function_2520ff(screen);
}

void function_148523(void);

// @retail 0x251b39
void c_matchmaking_screen::v3()
{
    if (!ANIMATION_FLAG(animation, 1))
    {
        long state=function_25142f();
        if (value618!=state)
        {
            long delta=state;
            select_pane((short *)&delta);
            value610=function_199d7c();
            value618=state;
            value614=function_75870();
            if (value618>=1)
                function_148523();
            if (state==3)
                function_236299(12);
            function_22ecb4_2(true);
        }
        switch (state)
        {
        case 0: function_251c68(this); break;
        case 1: function_251f8d(this); break;
        case 2: goto poll_members;
        case 3: function_251d84(this); break;
        }
    poll_members:
        if (!function_6c7e0() || !player_slot_count_active())
        {
            value610=1;
            function_25217b(this, 0);
        }
    }
    c_class_1a2c81::v3();
}
