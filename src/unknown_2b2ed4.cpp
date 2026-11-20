// @flags /O1 /Oi /arch:SSE /Gr
/* UNKNOWN_2B2ED4.CPP: filling in one item of the online Y menu's friends or
   players list (built with SSE, apart from its neighbours in
   unknown_2b2d7d.cpp) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include "data_array.h"
#include "globals.h"
#include "screen_widgets.h"
#include "unknown_19b516.h"
#include "unknown_2312b4.h"
#include "online_message_entries.h"

struct s_friend;
void friend_get_online_friend(s_friend const *player, XONLINE_FRIEND *result);

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

PRIVATE __forceinline long function_2b2ed6(XUID const *xuid, bool *active)
{
 long result;
 if (network_session_manager_session_unready() && function_19acc6(xuid))
 {
  *active = true;
  result = function_19adca(xuid);
 }
 else
 {
  *active = false;
  result = NONE;
 }
 return result;
}
/* fills one row of the online friends or players list: the name, the status text and the
   invitation, session and voice icons */
// @retail 0x2b2ed4
void __stdcall function_2b2ed4(c_y_menu_list *list, bool friends, bool empty,
    c_class_1a2c81 *item, s_friend *datum)
{
    if (!datum) return;
    /* a string buffer the screen's strings load into */
    struct s_string_buffer
    {
        word text[0x100];
        void load(c_widget *screen, long handle) { screen->function_230134(handle, text); }
    };
    c_class_1a2c81 *name = item->find_child(6, 0, false);
    c_text_widget_45a5e0 *status = (c_text_widget_45a5e0 *)item->find_child(6, 1, false);
    c_class_1a2c81 *invite = item->find_child(8, 3, false);
    c_class_1a2c81 *session = item->find_child(8, 5, false);
    c_class_1a2c81 *voice = item->find_child(8, 4, false);
    s_string_buffer display_name;
    display_name.text[0] = 0;
    long invitations = 0;
    bool online, voice_enabled, pending, outgoing;
    bool player_pending, player_outgoing, received, sent, declined;
    bool in_session, voice_available, voice_disabled, muted;
    bool active, joinable, self;
    if (empty)
    {
        s_friend_request request;
        if (friends)
            display_name.load((c_widget *)list->get_screen(), 0x1600029e);
        else if (friend_request_get(&request))
            display_name.load((c_widget *)list->get_screen(), 0x180002a2);
        else
            display_name.load((c_widget *)list->get_screen(), 0xb0002e7);
        online = false;
        voice_enabled = false;
        pending = false;
        outgoing = false;
        received = false;
        sent = false;
        player_pending = false;
        player_outgoing = false;
        in_session = false;
        voice_available = false;
        voice_disabled = false;
        muted = false;
        active = false;
        joinable = false;
        self = false;
        declined = false;
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
            s_entry *items = (s_entry *)list->get_items(&count);
            for (long i = 0; i < count; ++i)
            {
                s_entry *entry = &items[i];
                XUID const *other = NULL;
                if (entry) other = (XUID const *)entry;
                if (xuid_equal(xuid, other, false))
                {
                    dword entry_flags = entry->flags;
                    if ((friends && !(bool)((entry_flags >> 12) & 1)) ||
                        (!friends && !(bool)((entry_flags >> 10) & 1)))
                    {
                        flags |= 0x200;
                        ++invitations;
                    }
                }
            }
        }
        function_08cc20((s_name_buffer *)display_name.text, (wchar_t *)((byte *)datum + 0x3c));
        pending = (bool)((flags >> 4) & 1);
        outgoing = (bool)((flags >> 5) & 1);
        player_pending = (bool)((flags >> 10) & 1);
        player_outgoing = (bool)((flags >> 11) & 1);
        in_session = network_session_interface_has_user(xuid);
        long player = in_session ? function_19ad9c(xuid) : NONE;
        long local = voice_find_player(list->value88);
        voice_available = player != NONE ? voice_port_flag0_only(player) : false;
        voice_disabled = !voice_available || (player != NONE ? voice_port_flag1(player) : false);
        muted = player != NONE && local != NONE ? voice_test_unknownF0(local, player) : false;
        long active_player = function_2b2ed6(xuid, &active);
        joinable = active_player != NONE && active && function_19abe4(active_player);
        bool allowed = friends ? !outgoing && !pending : !player_outgoing && !player_pending;
        online = allowed && (flags & 1);
        voice_enabled = online && (flags & 2);
        received = online && (flags & 0x80);
        sent = allowed && (flags & 0x200);
        declined = allowed && (flags & 0x100);
    }
    if (name) name->function_22f52e()->set_text(display_name.text);
    if (status)
    {
        s_string_buffer message;
        message.text[0] = 0;
        status->value6e = (!empty && !self) ? true : false;
        long text = NONE;
        if (pending && friends) text = 0x16000266;
        else if (player_pending && !friends) text = 0x140002e8;
        else if (received && invitations > 0)
        {
            XONLINE_FRIEND friend_;
            friend_get_online_friend(datum, &friend_);
            if (online_friend_is_in_this_title(&friend_)) text = 0x15000269;
            else text = 0x1a000268;
        }
        else if (player_outgoing && !friends) text = 0x100002e9;
        else if (outgoing && friends) text = 0x12000267;
        else if (online)
        {
            word title[256];
            s_string_buffer suffix;
            title[0] = 0;
            suffix.text[0] = 0;
            XONLINE_FRIEND friend_;
            friend_get_online_friend(datum, &friend_);
            online_friend_get_title_name(&friend_, (wchar_t *)title, 256);
            s_friend_presence_status presence;
            bool known = function_8d230(&friend_, &presence);
            if (online_title_is_this_title(friend_.dwTitleID) && known)
            {
                XNKID zero = {0};
                bool can_join = memcmp(&friend_.sessionID, &zero, sizeof(zero)) != 0 &&
                    (*(dword *)((byte *)datum + 0x20) & 8) && !active;
                if (joinable) message.load((c_widget *)list->get_screen(), 0x18000250);
                else if (active) message.load((c_widget *)list->get_screen(), 0x14000251);
                else if (in_session) message.load((c_widget *)list->get_screen(), 0x13000252);
                else if (presence.state == 1)
                {
                    if (presence.value04)
                    {
                        message.load((c_widget *)list->get_screen(), 0x11000253);
                        switch (presence.value04)
                        {
                        case 2:
                            suffix.load((c_widget *)list->get_screen(), 0x12000255);
                            unicode_string_append(message.text, suffix.text, 256);
                            message.text[255] = 0;
                            break;
                        case 3:
                            suffix.load((c_widget *)list->get_screen(), 0xd000254);
                            unicode_string_append(message.text, suffix.text, 256);
                            message.text[255] = 0;
                            break;
                        default:
                            if (!can_join)
                            {
                                suffix.load((c_widget *)list->get_screen(), 0x13000257);
                                unicode_string_append(message.text, suffix.text, 256);
                                message.text[255] = 0;
                            }
                            else
                            {
                                suffix.load((c_widget *)list->get_screen(), 0xf000256);
                                unicode_string_append(message.text, suffix.text, 256);
                                message.text[255] = 0;
                            }
                            break;
                        }
                    }
                    else
                    {
                        message.load((c_widget *)list->get_screen(), 0xe000258);
                        suffix.load((c_widget *)list->get_screen(), 0x13000257);
                        unicode_string_append(message.text, suffix.text, 256);
                        message.text[255] = 0;
                    }
                }
                else if (presence.state == 5)
                {
                    message.load((c_widget *)list->get_screen(), 0x17000259);
                    suffix.load((c_widget *)list->get_screen(), 0x13000257);
                    unicode_string_append(message.text, suffix.text, 256);
                    message.text[255] = 0;
                }
                else if (presence.value08 == 1)
                {
                    message.load((c_widget *)list->get_screen(), 0x1900025a);
                    switch (presence.value04)
                    {
                    case 2:
                        suffix.load((c_widget *)list->get_screen(), 0x12000255);
                        unicode_string_append(message.text, suffix.text, 256);
                        message.text[255] = 0;
                        break;
                    case 3:
                        suffix.load((c_widget *)list->get_screen(), 0xd000254);
                        unicode_string_append(message.text, suffix.text, 256);
                        message.text[255] = 0;
                        break;
                    default:
                        if (can_join)
                        {
                            suffix.load((c_widget *)list->get_screen(), 0xf000256);
                            unicode_string_append(message.text, suffix.text, 256);
                            message.text[255] = 0;
                        }
                        else
                        {
                            suffix.load((c_widget *)list->get_screen(), 0x13000257);
                            unicode_string_append(message.text, suffix.text, 256);
                            message.text[255] = 0;
                        }
                        break;
                    }
                }
                else if (presence.value08 == 2 || presence.value08 == 3)
                {
                    message.load((c_widget *)list->get_screen(), 0x1500025b);
                    suffix.load((c_widget *)list->get_screen(), 0x13000257);
                    unicode_string_append(message.text, suffix.text, 256);
                    message.text[255] = 0;
                }
                else message.load((c_widget *)list->get_screen(), 0xe00024f);
            }
            else if (known && (long)function_217b70(title) > 0)
            {
                suffix.load((c_widget *)list->get_screen(), 0x1400025c);
                function_1630e0(message.text, suffix.text, title);
                switch (presence.state)
                {
                case 1:
                    suffix.load((c_widget *)list->get_screen(), 0x1200025e);
                    unicode_string_append(message.text, suffix.text, 256);
                    message.text[255] = 0;
                    break;
                case 2:
                    suffix.load((c_widget *)list->get_screen(), 0x1600025f);
                    unicode_string_append(message.text, suffix.text, 256);
                    message.text[255] = 0;
                    break;
                case 3:
                    suffix.load((c_widget *)list->get_screen(), 0x11000260);
                    unicode_string_append(message.text, suffix.text, 256);
                    message.text[255] = 0;
                    break;
                case 4:
                    suffix.load((c_widget *)list->get_screen(), 0x1e000261);
                    unicode_string_append(message.text, suffix.text, 256);
                    message.text[255] = 0;
                    break;
                case 5:
                    suffix.load((c_widget *)list->get_screen(), 0x14000262);
                    unicode_string_append(message.text, suffix.text, 256);
                    message.text[255] = 0;
                    break;
                case 6:
                    suffix.load((c_widget *)list->get_screen(), 0x1b000263);
                    unicode_string_append(message.text, suffix.text, 256);
                    message.text[255] = 0;
                    break;
                }
            }
            else if ((long)function_217b70(title) > 0)
            {
                if ((bool)((*(dword *)((byte *)datum + 0x20) >> 2) & 1))
                    suffix.load((c_widget *)list->get_screen(), 0x1500025d);
                else
                    suffix.load((c_widget *)list->get_screen(), 0x1400025c);
                function_1630e0(message.text, suffix.text, title);
            }
            else text = 0xe00024f;
        }
        else text = 0x700024e;
        if (text != NONE) status->function_253b1a(text);
        else status->function_22f52e()->set_text(message.text);
    }
    if (invite)
    {
        if (self) invite->value6e = false;
        else if (invitations >= 10)
        {
            invite->value6e = true;
            if (received) function_2b0a14((s_widget_view_2b0a *)invite, 28);
            else function_2b0a14((s_widget_view_2b0a *)invite, 17);
        }
        else if (invitations > 1)
        {
            invite->value6e = true;
            if (received) function_2b0a14((s_widget_view_2b0a *)invite, (short)(invitations + 18));
            else function_2b0a14((s_widget_view_2b0a *)invite, (short)(invitations + 7));
        }
        else if ((player_pending && !friends) || (pending && friends))
        {
            invite->value6e = true;
            function_2b0a14((s_widget_view_2b0a *)invite, 1);
        }
        else if (received && invitations > 0)
        {
            invite->value6e = true;
            function_2b0a14((s_widget_view_2b0a *)invite, 18);
        }
        else if (sent)
        {
            invite->value6e = true;
            function_2b0a14((s_widget_view_2b0a *)invite, 6);
        }
        else if ((player_outgoing && !friends) || (outgoing && friends))
        {
            invite->value6e = true;
            function_2b0a14((s_widget_view_2b0a *)invite, 2);
        }
        else if (declined)
        {
            invite->value6e = true;
            function_2b0a14((s_widget_view_2b0a *)invite, 7);
        }
        else invite->value6e = false;
    }
    if (session)
    {
        if (online)
        {
            session->value6e = true;
            if (joinable) function_2b0a14((s_widget_view_2b0a *)session, 29);
            else if (active) function_2b0a14((s_widget_view_2b0a *)session, 19);
            else function_2b0a14((s_widget_view_2b0a *)session, 0);
        }
        else session->value6e = false;
    }
    if (voice)
    {
        if (in_session)
        {
            if (muted)
            {
                voice->value6e = true;
                function_2b0a14((s_widget_view_2b0a *)voice, 3);
            }
            else if (voice_disabled)
            {
                voice->value6e = true;
                function_2b0a14((s_widget_view_2b0a *)voice, 5);
            }
            else if (voice_available)
            {
                voice->value6e = true;
                function_2b0a14((s_widget_view_2b0a *)voice, 4);
            }
            else voice->value6e = false;
        }
        else if (voice_enabled)
        {
            voice->value6e = true;
            function_2b0a14((s_widget_view_2b0a *)voice, 4);
        }
        else voice->value6e = false;
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
    layout.value4 = 0;
    layout.flags |= 1;
    function_22f042(&layout, item, 1);
}
