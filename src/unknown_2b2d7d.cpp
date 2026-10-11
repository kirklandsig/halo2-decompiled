// @flags /O1 /Oi /Gr
/* UNKNOWN_2B2D7D.CPP: the online Y menu's friends list
   and the base it shares with the players list */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include "data_array.h"
#include "globals.h"
#include "screen_widgets.h"
#include "unknown_19b516.h"
#include "unknown_2312b4.h"

struct s_message;
void function_149f49(s_message *message, word a, dword *id, short b, long c, long d, long e);
void function_238c21(long controller, long type, word *name, long maximum_count);
struct s_friend;
void friend_get_online_friend(s_friend const *player, XONLINE_FRIEND *result);
c_class_1473c9 *__stdcall function_2b71f0(s_screen_parameters *parameters);

/* what the name lookups take: a friend (type 2), a player (type 1) or
   nobody (no request) */
struct s_name_request
{
	long type;
	XONLINE_FRIEND field_xb3bdcf;
	byte unknown[0x78 - 4 - sizeof(XONLINE_FRIEND)];
};

void __stdcall function_148893(s_name_request *request, long flag);

/* a friend of the friends list (unknown_1a2ca7.cpp) */
#pragma pack(push, 4)
struct s_friend_view
{
	byte unknown00[4];
	unsigned __int64 xuid;
};
#pragma pack(pop)

/* the scenario's type at +0x10 (2 is the main menu) */
struct s_scenario_type_view
{
	byte unknown00[0x10];
	short type;
};

// @retail 0x2b2d9a
c_y_menu_friends_list::c_y_menu_friends_list(word user_flags) :
	c_y_menu_list(user_flags),
	item_count(NONE),
	handler(this, (list_item_method)&c_y_menu_friends_list::handle_item)
{
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2b2e6b
c_y_menu_list::c_y_menu_list(word user_flags) :
	c_list_widget_with_items(user_flags),
	value88(NONE)
{
	if (g_4e0350 && ((s_scenario_type_view *)g_4e0350)->type != 2)
	{
		g_4e647b = true;
	}
}

// @retail 0x2b2ebd
c_y_menu_list::~c_y_menu_list()
{
	g_4e647b = false;
}

// @retail 0x2b2e9f deleting c_y_menu_list

/* the players list's deleting destructor is this one */
// @retail 0x2b2dfd deleting c_y_menu_friends_list

/* slot 1 of the list (c_widget::v9 in unknown_19b516.h's view) */
// @retail 0x2b2e19
void c_y_menu_friends_list::v1()
{
	data = g_global_4acf62.field_4_4;
	item_count = NONE;
	((c_widget *)this)->m7f = 0;
	((c_widget *)this)->c_widget::v9();
}

/* gives the items new data when the friends list's count changed */
// @retail 0x2b2e31
void c_y_menu_friends_list::v3()
{
	data = g_global_4acf62.field_4_4;
	if (g_global_4acf62.field_4_4)
	{
		if (item_count != g_global_4acf62.field_4_4->actual_count)
		{
			item_count = g_global_4acf62.field_4_4->actual_count;
			function_24c0c4((c_widget *)this);
		}
	}
	else
	{
		item_count = NONE;
	}
	((c_widget *)this)->c_widget::v11();
}

// @retail 0x2b3ef5
void *c_y_menu_friends_list::get_item_data()
{
	return items;
}

// @retail 0x2b2d7d
long c_y_menu_friends_list::get_item_count()
{
	return 8;
}

/* the user's pending online messages, held by the menu three levels up */
// @retail 0x2b419f
void *c_y_menu_friends_list::get_items(long *count)
{
	c_online_y_menu_screen *screen = (c_online_y_menu_screen *)parent->parent->parent;

	*count = screen->message_count;
	return screen->messages;
}

/* a friend opens the friend's screen; the empty item asks for a gamertag to
   send a friend request to */
// @retail 0x2b386f
void c_y_menu_friends_list::handle_item(s_controller_reference **controller, long *item)
{
	s_friend_view *player = data ? (s_friend_view *)record_pool_lookup(data, *item) : 0;

	if (player)
	{
		if (player->xuid == 0)
		{
			function_148893(0, 1);
			name[0] = 0;
			function_238c21((*controller)->controller_index, 0xc, name, 0x10);
		}
		else
		{
			s_name_request request;
			s_screen_parameters parameters;

			parameters.field_c = 0;
			request.type = 2;
			friend_get_online_friend((s_friend const *)player, &request.field_xb3bdcf);
			function_148893(&request, 1);
			function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)function_2b71f0);
			parameters.load(&parameters);
		}
	}
}

#include <string.h>
#include <wchar.h>
struct s_friend_request { byte data[0x6a2]; };
bool friend_request_get(s_friend_request *request);
struct s_name_buffer;
void function_08cc20(s_name_buffer *buffer, wchar_t const *name);
void function_18ff47(long player, dword *out);
bool xuid_equal(XUID const *a, XUID const *b, bool compare_guest_number);
bool network_session_interface_has_user(XUID const *xuid);
long function_19ad9c(XUID const *xuid);
long voice_find_player(long controller);
bool voice_port_flag0_only(long port);
bool voice_port_flag1(long port);
bool voice_test_unknownF0(long port, long bit);
bool network_session_manager_session_unready();
bool function_19acc6(XUID const *xuid);
long function_19adca(XUID const *xuid);
bool function_19abe4(long player_index);
bool online_friend_is_in_this_title(XONLINE_FRIEND const *friend_);
bool online_title_is_this_title(DWORD title_id);
void online_friend_get_title_name(XONLINE_FRIEND const *friend_, wchar_t *name, short name_length);
struct s_friend_presence_status
{
    long state;
    dword value04;
    dword value08;
    word time0c;
    word time0e;
};
bool function_8d230(XONLINE_FRIEND const *friend_, s_friend_presence_status *status);
dword function_217b70(word const *text);
word *unicode_string_append(word *destination, word const *source, long maximum_count);
word *function_1630e0(word *buffer, word const *format, ...);
struct s_widget_view_2b0a;
void function_2b0a14(s_widget_view_2b0a *widget, short index);
struct s_cached_player_identity;
void function_80440(s_cached_player_identity const *identity, s_recent_player *player);
struct s_player_status_values;
struct s_player_status_source;
void function_080de0(s_player_status_values *values, s_player_status_source const *source);
void function_2b01a2(long value, s_widget_item *item);
void function_22f042(s_widget_item *items, c_class_1a2c81 *widget, long count);

// @retail 0x2b2ed4
void __stdcall function_2b2ed4(c_y_menu_list *list, bool friends, bool empty,
    c_class_1a2c81 *item, s_friend *datum)
{
    if (!datum) return;
    c_class_1a2c81 *name = item->find_child(6, 0, false);
    c_text_widget_45a5e0 *status = (c_text_widget_45a5e0 *)item->find_child(6, 1, false);
    c_class_1a2c81 *invite = item->find_child(8, 3, false);
    c_class_1a2c81 *session = item->find_child(8, 5, false);
    c_class_1a2c81 *voice = item->find_child(8, 4, false);
    word display_name[256];
    display_name[0] = 0;
    long invitations = 0;
    volatile bool online = false, voice_enabled = false, pending = false, outgoing = false;
    bool player_pending = false, player_outgoing = false, received = false, sent = false, declined = false;
    bool in_session = false, voice_available = false, voice_disabled = false, muted = false;
    bool active = false, joinable = false, self = false;
    if (empty)
    {
        s_friend_request request;
        long text = friends ? 0x1600029e : friend_request_get(&request) ? 0x180002a2 : 0xb0002e7;
        ((c_widget *)list->get_screen())->function_230134(text, display_name);
    }
    else
    {
        dword flags = *(dword *)((byte *)datum + 0x20);
        dword identity[28];
        function_18ff47(list->get_controller_index(), identity);
        XUID const *xuid = (XUID const *)((byte *)datum + 4);
        self = xuid_equal(xuid, (XUID *)identity, false);
        if (!self)
        {
            long count;
            byte *entries = (byte *)list->get_items(&count);
            for (long i = 0; i < count; ++i, entries += 0x40)
            {
                if (xuid_equal(xuid, entries ? (XUID *)entries : 0, false) &&
                    !(bool)((*(dword *)(entries + 0x1c) >> (friends ? 12 : 10)) & 1))
                {
                    flags |= 0x200;
                    ++invitations;
                }
            }
        }
        function_08cc20((s_name_buffer *)display_name, (wchar_t *)((byte *)datum + 0x3c));
        pending = (bool)((flags >> 4) & 1);
        outgoing = (bool)((flags >> 5) & 1);
        player_pending = (bool)((flags >> 10) & 1);
        player_outgoing = (bool)((flags >> 11) & 1);
        in_session = network_session_interface_has_user(xuid);
        long player = in_session ? function_19ad9c(xuid) : NONE;
        long local = voice_find_player(list->value88);
        voice_available = player != NONE && voice_port_flag0_only(player);
        voice_disabled = !voice_available || (player != NONE && voice_port_flag1(player));
        muted = player != NONE && local != NONE && voice_test_unknownF0(local, player);
        long active_player = NONE;
        if (network_session_manager_session_unready() && function_19acc6(xuid))
        {
            active = true;
            active_player = function_19adca(xuid);
        }
        joinable = active_player != NONE && active && function_19abe4(active_player);
        bool allowed = friends ? !outgoing && !pending : !player_outgoing && !player_pending;
        online = allowed && (flags & 1);
        voice_enabled = online && (flags & 2);
        received = online && (flags & 0x80);
        sent = allowed && (flags & 0x200);
        declined = allowed && (flags & 0x100);
    }
    if (name) name->function_22f52e()->set_text(display_name);
    if (status)
    {
        word message[256];
        message[0] = 0;
        status->value6e = !empty && !self;
        long text = NONE;
        if (pending && friends) text = 0x16000266;
        else if (player_pending && !friends) text = 0x140002e8;
        else if (received && invitations > 0)
        {
            XONLINE_FRIEND friend_;
            friend_get_online_friend(datum, &friend_);
            text = online_friend_is_in_this_title(&friend_) ? 0x15000269 : 0x1a000268;
        }
        else if (player_outgoing && !friends) text = 0x100002e9;
        else if (outgoing && friends) text = 0x12000267;
        else if (!online) text = 0x700024e;
        else
        {
            word title[256], suffix[256];
            title[0] = suffix[0] = 0;
            XONLINE_FRIEND friend_;
            friend_get_online_friend(datum, &friend_);
            online_friend_get_title_name(&friend_, (wchar_t *)title, 256);
            s_friend_presence_status presence;
            bool known = function_8d230(&friend_, &presence);
            long suffix_id = NONE;
            if (online_title_is_this_title(friend_.dwTitleID) && known)
            {
                XNKID zero = {0};
                bool can_join = memcmp(&friend_.sessionID, &zero, sizeof(zero)) != 0 &&
                    (*(dword *)((byte *)datum + 0x20) & 8) && !active;
                long message_id;
                if (joinable) message_id = 0x18000250;
                else if (active) message_id = 0x14000251;
                else if (in_session) message_id = 0x13000252;
                else if (presence.state == 1)
                {
                    message_id = presence.value04 ? 0x11000253 : 0xe000258;
                    suffix_id = presence.value04 ? (presence.value04 == 2 ? 0x12000255 :
                        presence.value04 == 3 ? 0xd000254 : can_join ? 0xf000256 : 0x13000257) : 0x13000257;
                }
                else if (presence.state == 5) { message_id = 0x17000259; suffix_id = 0x13000257; }
                else if (presence.value08 == 1)
                {
                    message_id = 0x1900025a;
                    suffix_id = presence.value04 == 2 ? 0x12000255 : presence.value04 == 3 ? 0xd000254 :
                        can_join ? 0xf000256 : 0x13000257;
                }
                else if (presence.value08 == 2 || presence.value08 == 3)
                    { message_id = 0x1500025b; suffix_id = 0x13000257; }
                else message_id = 0xe00024f;
                ((c_widget *)list->get_screen())->function_230134(message_id, message);
            }
            else if (known && (long)function_217b70(title) > 0)
            {
                ((c_widget *)list->get_screen())->function_230134(0x1400025c, suffix);
                function_1630e0(message, suffix, title);
                switch (presence.state)
                {
                case 1: suffix_id = 0x1200025e; break;
                case 2: suffix_id = 0x1600025f; break;
                case 3: suffix_id = 0x11000260; break;
                case 4: suffix_id = 0x1e000261; break;
                case 5: suffix_id = 0x14000262; break;
                case 6: suffix_id = 0x1b000263; break;
                }
            }
            else if ((long)function_217b70(title) > 0)
            {
                ((c_widget *)list->get_screen())->function_230134(
                    (bool)((*(dword *)((byte *)datum + 0x20) >> 2) & 1) ? 0x1500025d : 0x1400025c, suffix);
                function_1630e0(message, suffix, title);
            }
            else text = 0xe00024f;
            if (suffix_id != NONE)
            {
                ((c_widget *)list->get_screen())->function_230134(suffix_id, suffix);
                unicode_string_append(message, suffix, 256);
                message[255] = 0;
            }
        }
        if (text != NONE) status->function_253b1a(text);
        else status->function_22f52e()->set_text(message);
    }
    if (invite)
    {
        short icon = NONE;
        if (!self)
        {
            if (invitations >= 10) icon = received ? 28 : 17;
            else if (invitations > 1) icon = (short)(invitations + (received ? 18 : 7));
            else if ((player_pending && !friends) || (pending && friends)) icon = 1;
            else if (received && invitations > 0) icon = 18;
            else if (sent) icon = 6;
            else if ((player_outgoing && !friends) || (outgoing && friends)) icon = 2;
            else if (declined) icon = 7;
        }
        invite->value6e = icon != NONE;
        if (icon != NONE) function_2b0a14((s_widget_view_2b0a *)invite, icon);
    }
    if (session)
    {
        session->value6e = online;
        if (online) function_2b0a14((s_widget_view_2b0a *)session, joinable ? 29 : active ? 19 : 0);
    }
    if (voice)
    {
        short icon = NONE;
        if (in_session)
        {
            if (muted) icon = 3;
            else if (voice_disabled) icon = 5;
            else if (voice_available) icon = 4;
        }
        else if (voice_enabled) icon = 4;
        voice->value6e = icon != NONE;
        if (icon != NONE) function_2b0a14((s_widget_view_2b0a *)voice, icon);
    }
    s_recent_player player;
    s_widget_item layout;
    dword values[4];
    layout.flags = 0;
    function_80440((s_cached_player_identity *)((byte *)datum + 4), &player);
    function_080de0((s_player_status_values *)values, (s_player_status_source *)&player);
    function_2b01a2(empty ? NONE : *(signed char *)((byte *)&player + 0x56), &layout);
    memcpy(layout.value48, values, sizeof(values));
    layout.flags |= 2;
    layout.flags |= 1;
    layout.value4 = 0;
    function_22f042(&layout, item, 1);
}

bool function_6c7e0();
struct s_screen_view_2b2d;
long function_2b2d81(s_screen_view_2b2d *screen);

// @retail 0x2b37fd
void c_y_menu_friends_list::v20(c_class_1a2c81 *item, long unused)
{
    if ((byte)function_2b2d81((s_screen_view_2b2d *)parent->parent->parent))
    {
        long index = function_6c7e0() ? widget_item(item)->value70 : NONE;
        s_friend *entry = data ? (s_friend *)record_pool_lookup(data, index) : 0;
        if (entry)
        {
            bool empty = true;
            if (((s_friend_view *)entry)->xuid != 0)
                empty = false;
            function_2b2ed4(this, true, empty, item, entry);
        }
        else
            function_22f042(0, item, 0);
    }
}
