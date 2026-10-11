// @flags /O1 /Oi /Gr
/* SCREEN_ONLINE_Y_MENU_PLAYERS_LIST.CPP: the online Y menu's list of the
   players met, and the menu's tab bar */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include "data_array.h"
#include "screen_widgets.h"
#include "unknown_19b510.h"
#include "unknown_19b516.h"
#include "unknown_18f576.h"
#include "unknown_2312b4.h"

struct s_message;
void function_149f49(s_message *message, word a, dword *id, short b, long c, long d, long e);
void function_238c21(long controller, long type, word *name, long maximum_count);
void function_236299(long sound);
struct s_friend;
void friend_get_online_friend(s_friend const *player, XONLINE_FRIEND *result);
bool function_18ff64(long index);
c_class_1473c9 *__stdcall function_2b7201(s_screen_parameters *parameters);

#pragma pack(push, 2)
struct s_player_identity
{
	unsigned __int64 id;
	byte data[0x6a2 - 8];
};
#pragma pack(pop)

bool player_slot_get_identity(long index, s_player_identity *identity);

/* what the name lookups take (unknown_2b2d7d.cpp) */
struct s_name_request
{
	long type;
	XONLINE_FRIEND field_xb3bdcf;
	byte unknown[0x78 - 4 - sizeof(XONLINE_FRIEND)];
};

void __stdcall function_148893(s_name_request *request, long flag);

/* a player met (unknown_1a2ca7.cpp) */
#pragma pack(push, 4)
struct s_friend_view
{
	byte unknown00[4];
	unsigned __int64 xuid;
};
#pragma pack(pop)

/* the players list's items: references to the players (unknown_1a2ca7.cpp) */
struct s_player_reference_view
{
	byte unknown00[4];
	long player_index;
};

/* the players' online status block */
struct s_player_status_view
{
	byte unknown00[0x1c];
	long value1c;
};

// @retail 0x2b3f15
c_y_menu_players_list::c_y_menu_players_list(word user_flags) :
	c_y_menu_list(user_flags),
	item_count(NONE),
	handler(this, (list_item_method)&c_y_menu_players_list::handle_item)
{
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2b3f78
void c_y_menu_players_list::v1()
{
	data = g_global_4acf62.field_8_2;
	item_count = NONE;
	((c_widget *)this)->m7f = 0;
	((c_widget *)this)->c_widget::v9();
}

// @retail 0x2b3f90
void c_y_menu_players_list::v3()
{
	data = g_global_4acf62.field_8_2;
	if (g_global_4acf62.field_8_2)
	{
		if (item_count != g_global_4acf62.field_8_2->actual_count)
		{
			item_count = g_global_4acf62.field_8_2->actual_count;
			function_24c0c4((c_widget *)this);
		}
	}
	else
	{
		item_count = NONE;
	}
	((c_widget *)this)->c_widget::v11();
}

/* a player met opens the player's screen; the empty item asks for a gamertag
   (or explains why the user cannot send a friend request) */
// @retail 0x2b4059
void c_y_menu_players_list::handle_item(s_controller_reference **controller, long *item)
{
	if (*item != NONE && function_6c7e0())
	{
		s_screen_parameters parameters;
		s_friend_view *player;
		s_record_pool *players;

		parameters.field_c = 0;
		player = 0;
		players = data;
		if (players && g_global_4acf62.clan_member_reference_data)
		{
			s_player_reference_view *reference = (s_player_reference_view *)record_pool_lookup(g_global_4acf62.clan_member_reference_data, *item);

			if (reference)
			{
				player = (s_friend_view *)record_pool_lookup(players, reference->player_index);
			}
		}
		function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, 0);
		if (player)
		{
			if (player->xuid == 0)
			{
				long controller_index = (*controller)->controller_index;
				s_player_identity identity;

				if (player_slot_get_identity(controller_index, &identity))
				{
					s_player_slot_blockb82 block;

					if (function_18ffc3(controller_index, &block) && ((s_player_status_view *)&block)->value1c >= 1)
					{
						function_148893(0, 1);
						name[0] = 0;
						function_238c21((*controller)->controller_index, 0xd, name, 0x10);
					}
					else
					{
						dialog_ok_show(1, 0x5d, 4, 1 << controller_index, 0, 0);
					}
				}
				else if (!function_18ff64(controller_index))
				{
					function_238c21(controller_index, 0x10, 0, 0);
				}
			}
			else
			{
				s_name_request request;

				request.type = 2;
				friend_get_online_friend((s_friend const *)player, &request.field_xb3bdcf);
				function_148893(&request, 0);
				parameters.load = function_2b7201;
				parameters.load(&parameters);
			}
			return;
		}
	}
	function_236299(2);
}

// @retail 0x2b41bc
c_y_menu_tab_bar::c_y_menu_tab_bar(word user_flags) :
	c_class_1a2c81(5, user_flags),
	focused(0)
{
}

/* slot 1 of the widget: remembers its first child, then runs the base slot */
// @retail 0x2b41db
void c_y_menu_tab_bar::v1()
{
	if (child)
		focused = child;
	((c_widget *)this)->function_22e315();
}

/* left and right move between the tabs that show, wrapping around */
// @retail 0x2b41ea
bool c_y_menu_tab_bar::v10(s_widget_event *event)
{
	bool result;

	switch (event->type)
	{
	case 4:
		if (focused)
		{
			c_class_1a2c81 *tab = focused;

			do
			{
				if (tab->next)
				{
					tab = tab->next;
				}
				else
				{
					while (tab->previous)
					{
						tab = tab->previous;
					}
				}
			} while (tab && !tab->value6e);
			if (tab && tab != focused)
			{
				v7(tab);
				focused = tab;
			}
		}
		function_236299(9);
		result = true;
		break;
	case 2:
		if (focused)
		{
			c_class_1a2c81 *tab = focused;

			do
			{
				if (tab->previous)
				{
					tab = tab->previous;
				}
				else
				{
					while (tab->next)
					{
						tab = tab->next;
					}
				}
			} while (tab && !tab->value6e);
			if (tab && tab != focused)
			{
				v7(tab);
				focused = tab;
			}
		}
		function_236299(9);
		result = true;
		break;
	default:
		result = c_class_1a2c81::v10(event);
		break;
	}
	return result;
}

bool function_6c7e0();
struct s_screen_view_2b3e;
long function_2b3efc(s_screen_view_2b3e *screen);
void function_22f042(s_widget_item *items, c_class_1a2c81 *widget, long count);
void __stdcall function_2b2ed4(c_y_menu_list *list, bool friends, bool empty,
    c_class_1a2c81 *item, s_friend *datum);

// @retail 0x2b3fca
void c_y_menu_players_list::v20(c_class_1a2c81 *item, long unused)
{
    if ((byte)function_2b3efc((s_screen_view_2b3e *)parent->parent->parent))
    {
        long index = function_6c7e0() ? widget_item(item)->value70 : NONE;
        s_friend *entry = 0;
        if (data && g_global_4acf62.clan_member_reference_data)
        {
            s_player_reference_view *reference = (s_player_reference_view *)record_pool_lookup(g_global_4acf62.clan_member_reference_data, index);
            if (reference)
                entry = (s_friend *)record_pool_lookup(data, reference->player_index);
        }
        if (entry)
        {
            bool empty = true;
            if (((s_friend_view *)entry)->xuid != 0)
                empty = false;
            function_2b2ed4(this, false, empty, item, entry);
        }
        else
            function_22f042(0, item, 0);
    }
}
