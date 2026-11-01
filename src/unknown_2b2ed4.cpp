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
// score 330 (was 330): frame 0xfd4 agrees; child/text/layout homes and suffix re-derivations differ.
// merging the two XONLINE_FRIEND copies into one local in the status block
// left the bytes and the score unchanged, so it was reverted; the stack homes still differ.
// the block order follows retail's apart from a few blocks, but stack offsets
// still differ: the widget pointers, several buffers and layout, notably the status text's message and
// suffix (-0x2cc and -0x4cc against retail's -0x258 and -0x458). Retail places the loop's
// friends == false shift out of line (still within the loop), clears ebx inside the received-invite
// branch, branches directly to its later offline-text block, and has no NONE test after the last
// title text. In the state == 1 suffix, retail loads each text and the suffix buffer in a different
// order. And retail takes the suffix's address again for function_1630e0 where ours reuses esi.
// Disjoint UI/final-item scopes, branch-owned text result, and independent child-pointer
// declarations, seven nonempty combinations 358 each; all with online arm first 701/2373.
// The 0xfd4 frame now agrees; scoped workspaces do not change the remaining buffer homes.
// Round 224: only final layout constructed after the null guard/child lookups/
// before name publication/after status publication, with whole-word or native-bit
// flag stores: 694/694/670/670/526/526/463/463. Original construction and body restored.
// Round 47 (aligned count 56): retail's buffers run display_name, title, layout, suffix, message
// upwards from -0x8cc; ours title, display_name, suffix, message, layout. suffix declared before
// title, friend_ before them, title/suffix at the top of the status block, title at function
// scope: 358 each (same code). The friends test as `!friends ? >> 10 : >> 12` 359; negated per arm
// 605/607 (retail keeps the >> 10 arm out of line after the session lookup).
// Lane 14 r16 (aligned difference 62: the four text buffers' and the final layout's homes, the
// child-pointer slots and four block spots): title renamed to sort before the name buffer 358 (same
// code); the bit test inside each arm of the friends/players shift ternary 605; that ternary inverted
// 359; the final text test inverted 496; separate title and suffix clears in retail's order 358.
// Body restored.
// Round 6 (lane 17): frame locals are ordered by reference density (count / size, densest nearest EBP):
// one item-record reference fewer gives retail's buffer homes exactly (record flags merged into `|= 3`
// 347 at 2328 bytes, aligned 22; no value4 store 360, aligned 23); retail's suffix has ~4 more references,
// two of them the `lea` re-derived after 0x230134 where ours reuses ESI (open callee case). The explicit
// record clear dropped (constructor clears it) 358; bitfield ORs 358; flags |= 2 before the copy 359;
// value08 switch appending per case 330 (its default then speculates the text id: not retail's), with
// whole sequences in one/both can_join arms 562/563/610; one empty-branch call with a chosen id 653;
// direct setter calls without the text variable 533 (retail loads the id into EAX); received text as
// if/else 662 at 2345 bytes (NONE test threaded as retail, registers ripple); state == 1 case 3 first or
// default first 358/358, `if (can_join)` first 478, with the whole sequence in both arms 477 (that switch
// then exactly retail's code), plus split clears 477, plus `|= 3` 477, plus received if/else 662.
// The optimize pragma around the active-player helper removed (identical code). Kept: the value08 switch
// appending in each case, 330: its extra message references move the message buffer to retail's home
// (a side effect, not the cause: suffix still lacks the post-0x230134 re-derivations), and its default
// now speculates the text id where retail branches. Remaining in order: the loop's `>> 10` arm (retail
// out of line after the session lookup), the received arm's threading, the `!online` arm as the setter
// call's fall-through host, suffix/record/title homes and the session/voice pointer slots.
// Round 449: native four-member status output, existing range member and
// uninitialized recent-player entry lifetime, all seven combinations 358;
// complete graph with native status workspace also at entry 358. Original
// array, member observations, output scopes and full body restored; homes remain.
// Round 49 (lane 16, fast = plain here): the buffer homes follow reference weight, not names or
// declaration places: display_name declared first, message at function scope, title and suffix
// beside message 358 each (same code); player/layout/values declared at the top 670; the empty
// row's text chosen first and set through one call 652 (display_name then takes retail's -0x8cc,
// but the three screen lookups merge). Diagnostics: six extra title stores put display_name at
// -0x8cc and title at -0x6cc as in retail (516); extra display_name stores 694. So retail weighs
// title above display_name and layout below suffix; ours the other way round.
// Round 58 (lane 12, fast = plain here; none kept): with frame offsets, branch targets and globals
// masked the body is 13 aligned lines from retail (the >> 10 arm, the received arm's ebx clear and
// threading, the !online host, five suffix `lea` re-derivations after 0x230134, one case-label
// load), so nearly all of the count is frame homes, and those follow the suffix's reference
// weight: BLOCKED-BY 0x230134 (retail callers re-derive the buffer after it). Tried: `session`
// renamed to sort after `voice` 330 (the two slots tie at two references each; names do not
// break the tie); `voice` declared before `session` 330; the invitation tail written out in both
// friends arms 342 (cross-jumped back to the same layout); the received text as if/else 661
// (confirms the listed 662); the online block first with the offline text as the final else 708.
// Round 52 (lane 16, fast = plain): retail sends every text arm straight to one function_253b1a call (the
// received arm's select and the last title-less arm share `xor ebx, ebx; jmp`), with no NONE test; ours keeps a
// test that the received select and the last arm share. Received text as if/else 661 (2338 bytes; the test is
// threaded but every widget-pointer slot moves); `else if (online) {...} else text = 0x700024e;` 708 (2379
// bytes); message arms calling set_text themselves and the last arm calling function_253b1a(0xe00024f), the
// dispatch keeping only `if (text != NONE)`, 548 (2352 bytes); the null-datum guard as a wrapping if 330 (same
// code); the session-player lookup as if/else 330 (same code). The suffix buffer's home also needs retail's
// re-derived `lea` after each 0x230134 call (an open callee case: BLOCKED-BY 0x230134).
// Round 283: complete session/voice/both visibility stores 391/444/453; coherent final
// flags alone/session/voice/both 330/389/426/436; all with constructor-owned initial clear 436.
// Coherent flags alone: 2327 bytes, count 330; original body and stores restored.
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
    bool online, voice_enabled, pending, outgoing;
    bool player_pending, player_outgoing, received, sent, declined;
    bool in_session, voice_available, voice_disabled, muted;
    bool active, joinable, self;
    if (empty)
    {
        s_friend_request request;
        if (friends)
            ((c_widget *)list->get_screen())->function_230134(0x1600029e, display_name);
        else if (friend_request_get(&request))
            ((c_widget *)list->get_screen())->function_230134(0x180002a2, display_name);
        else
            ((c_widget *)list->get_screen())->function_230134(0xb0002e7, display_name);
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
                if (xuid_equal(xuid, other, false) &&
                    !(bool)((friends ? entry->flags >> 12 : entry->flags >> 10) & 1))
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
    if (name) name->function_22f52e()->set_text(display_name);
    if (status)
    {
        word message[256];
        message[0] = 0;
        status->value6e = (!empty && !self) ? true : false;
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
            if (online_title_is_this_title(friend_.dwTitleID) && known)
            {
                XNKID zero = {0};
                bool can_join = memcmp(&friend_.sessionID, &zero, sizeof(zero)) != 0 &&
                    (*(dword *)((byte *)datum + 0x20) & 8) && !active;
                if (joinable) ((c_widget *)list->get_screen())->function_230134(0x18000250, message);
                else if (active) ((c_widget *)list->get_screen())->function_230134(0x14000251, message);
                else if (in_session) ((c_widget *)list->get_screen())->function_230134(0x13000252, message);
                else if (presence.state == 1)
                {
                    if (presence.value04)
                    {
                        ((c_widget *)list->get_screen())->function_230134(0x11000253, message);
                        switch (presence.value04)
                        {
                        case 2:
                            ((c_widget *)list->get_screen())->function_230134(0x12000255, suffix);
                            unicode_string_append(message, suffix, 256);
                            message[255] = 0;
                            break;
                        case 3:
                            ((c_widget *)list->get_screen())->function_230134(0xd000254, suffix);
                            unicode_string_append(message, suffix, 256);
                            message[255] = 0;
                            break;
                        default:
                            if (!can_join) ((c_widget *)list->get_screen())->function_230134(0x13000257, suffix);
                            else ((c_widget *)list->get_screen())->function_230134(0xf000256, suffix);
                            unicode_string_append(message, suffix, 256);
                            message[255] = 0;
                            break;
                        }
                    }
                    else
                    {
                        ((c_widget *)list->get_screen())->function_230134(0xe000258, message);
                        ((c_widget *)list->get_screen())->function_230134(0x13000257, suffix);
                        unicode_string_append(message, suffix, 256);
                        message[255] = 0;
                    }
                }
                else if (presence.state == 5)
                {
                    ((c_widget *)list->get_screen())->function_230134(0x17000259, message);
                    ((c_widget *)list->get_screen())->function_230134(0x13000257, suffix);
                    unicode_string_append(message, suffix, 256);
                    message[255] = 0;
                }
                else if (presence.value08 == 1)
                {
                    ((c_widget *)list->get_screen())->function_230134(0x1900025a, message);
                    switch (presence.value04)
                    {
                    case 2:
                        ((c_widget *)list->get_screen())->function_230134(0x12000255, suffix);
                        unicode_string_append(message, suffix, 256);
                        message[255] = 0;
                        break;
                    case 3:
                        ((c_widget *)list->get_screen())->function_230134(0xd000254, suffix);
                        unicode_string_append(message, suffix, 256);
                        message[255] = 0;
                        break;
                    default:
                        if (can_join) ((c_widget *)list->get_screen())->function_230134(0xf000256, suffix);
                        else ((c_widget *)list->get_screen())->function_230134(0x13000257, suffix);
                        unicode_string_append(message, suffix, 256);
                        message[255] = 0;
                        break;
                    }
                }
                else if (presence.value08 == 2 || presence.value08 == 3)
                {
                    ((c_widget *)list->get_screen())->function_230134(0x1500025b, message);
                    ((c_widget *)list->get_screen())->function_230134(0x13000257, suffix);
                    unicode_string_append(message, suffix, 256);
                    message[255] = 0;
                }
                else ((c_widget *)list->get_screen())->function_230134(0xe00024f, message);
            }
            else if (known && (long)function_217b70(title) > 0)
            {
                ((c_widget *)list->get_screen())->function_230134(0x1400025c, suffix);
                function_1630e0(message, suffix, title);
                switch (presence.state)
                {
                case 1:
                    ((c_widget *)list->get_screen())->function_230134(0x1200025e, suffix);
                    unicode_string_append(message, suffix, 256);
                    message[255] = 0;
                    break;
                case 2:
                    ((c_widget *)list->get_screen())->function_230134(0x1600025f, suffix);
                    unicode_string_append(message, suffix, 256);
                    message[255] = 0;
                    break;
                case 3:
                    ((c_widget *)list->get_screen())->function_230134(0x11000260, suffix);
                    unicode_string_append(message, suffix, 256);
                    message[255] = 0;
                    break;
                case 4:
                    ((c_widget *)list->get_screen())->function_230134(0x1e000261, suffix);
                    unicode_string_append(message, suffix, 256);
                    message[255] = 0;
                    break;
                case 5:
                    ((c_widget *)list->get_screen())->function_230134(0x14000262, suffix);
                    unicode_string_append(message, suffix, 256);
                    message[255] = 0;
                    break;
                case 6:
                    ((c_widget *)list->get_screen())->function_230134(0x1b000263, suffix);
                    unicode_string_append(message, suffix, 256);
                    message[255] = 0;
                    break;
                }
            }
            else if ((long)function_217b70(title) > 0)
            {
                if ((bool)((*(dword *)((byte *)datum + 0x20) >> 2) & 1))
                    ((c_widget *)list->get_screen())->function_230134(0x1500025d, suffix);
                else
                    ((c_widget *)list->get_screen())->function_230134(0x1400025c, suffix);
                function_1630e0(message, suffix, title);
            }
            else text = 0xe00024f;
        }
        if (text != NONE) status->function_253b1a(text);
        else status->function_22f52e()->set_text(message);
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
