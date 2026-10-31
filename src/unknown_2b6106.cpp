// @flags /O1 /Oi /Gr
/* UNKNOWN_2B6106.CPP: the screen of what can be
   done to the player the online Y menu selected (vtable 0x45bbd0): its list, the selected player's friend
   state and the load procedures of its five modes */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include "screen_widgets.h"
#include "unknown_19b516.h"
#include "unknown_234c64.h"
#include "unknown_2b6106.h"

void function_18ff47(long player, dword *out);
void function_148995(s_window_manager_e94 *value);
bool window_manager_channel_window_in_use(long channel, long index);
void function_1a303b(long controller_index);
void function_14887e(s_screen_settings_54dc6c *settings);
void function_6b640(long task_index);
void qos_release(long handle);
void friends_list_reset(bool dispose);
bool online_messages_find_from(const XUID *sender, DWORD controller_index, long kind);

/* the window manager's selected player (function_14887e copies it out) */
#pragma pack(push, 4)
struct s_name_request
{
	long type;
	union
	{
		XUID user_xuid;
		XONLINE_FRIEND field_xb3bdcf;
	};
	byte unknown5c[0x78 - 0x5c];
};
#pragma pack(pop)

/* the selected player's id */
static inline XUID *selection_get_xuid(s_name_request *selection)
{
	XUID *result = NULL;

	switch (selection->type)
	{
	case 1:
		result = &selection->user_xuid;
		break;
	case 2:
		result = &selection->field_xb3bdcf.xuid;
		break;
	}
	return result;
}

void __stdcall function_148893(s_name_request *request, long mode);

/* the flags friends_lists_get_user reports */
struct s_friend_user_flags
{
	dword online : 1;
	dword bits1 : 3;
	dword bit4 : 1;
	dword bit5 : 1;
	dword bits6 : 4;
	dword bit10 : 1;
	dword bit11 : 1;
	dword bits12 : 20;
};

void friends_lists_get_user(XUID const *xuid, bool *arg_a721be, bool *is_player, dword *flags, DWORD *title_id, bool *in_session, XONLINE_FRIEND *field_xb3bdcf);

struct s_friend_session_2b642e;

/* the player selected screen (vtable 0x45bbd0) */
class c_y_menu_player_selected_screen : public c_screen_with_menu
{
public:
	c_y_menu_player_selected_screen(long a, long b, word user_flags);
	virtual ~c_y_menu_player_selected_screen();
	virtual void v3();

	virtual screen_load_proc get_load_proc();

	void update_selection();
	void update_join_text();
	void update_friend_status(XONLINE_FRIEND const *friend_, s_friend_session_2b642e *session);
	void update_friend_details(XONLINE_FRIEND const *friend_, s_friend_session_2b642e *session);

	c_y_menu_player_selected_list list;
	long task_index;
	long qos_handle;
	byte unknown9c4[0x10d8 - 0x9c4];
	long value10d8;
	long value10dc;
	long value10e0;
	bool value10e4;
	bool value10e5;
	bool value10e6;
	bool in_session;
	long mode;
};

c_class_1473c9 *__stdcall function_2b7152(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b71f0(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b7201(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b7212(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b7223(s_screen_parameters *parameters);

// @retail 0x2b6106
c_y_menu_player_selected_screen::c_y_menu_player_selected_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0x1f, a, b, user_flags, &list),
	list(user_flags),
	task_index(NONE),
	qos_handle(NONE),
	value10d8(0),
	value10dc(0),
	value10e0(0),
	value10e4(false),
	value10e5(false),
	value10e6(false),
	in_session(false),
	mode(0)
{
	s_window_manager_e94 value;

	function_18ff47(get_controller_index(), value.data);
	function_148995(&value);
	if (!window_manager_channel_window_in_use(4, 4))
	{
		function_1a303b(get_controller_index());
	}
	update_selection();
}

// @retail 0x2b61b2 deleting

// @retail 0x2b6301
c_y_menu_player_selected_screen::~c_y_menu_player_selected_screen()
{
	if (task_index != NONE)
	{
		function_6b640(task_index);
		task_index = NONE;
	}
	if (qos_handle != NONE)
	{
		qos_release(qos_handle);
		qos_handle = NONE;
	}
	if (!window_manager_channel_window_in_use(4, 4))
	{
		friends_list_reset(true);
	}
}

/* looks the selected player up in the friends lists: whether the player can
   be invited and joined */
// @retail 0x2b6208
void c_y_menu_player_selected_screen::update_selection()
{
	s_name_request selection;
	XONLINE_FRIEND field_xb3bdcf;
	s_friend_user_flags flags;
	DWORD title_id;
	bool arg_a721be;
	bool is_player;
	XUID *xuid;

	value10e5 = false;
	value10e6 = false;
	function_14887e((s_screen_settings_54dc6c *)&selection);
	xuid = selection_get_xuid(&selection);
	if (xuid && xuid->qwUserID != 0)
	{
		friends_lists_get_user(xuid, &arg_a721be, &is_player, (dword *)&flags, &title_id, &in_session, &field_xb3bdcf);
		if (!in_session)
		{
			value10e4 = false;
		}
		if (arg_a721be || is_player)
		{
			bool bit10 = TEST_FIELD_BIT(flags.bit10);
			bool bit11 = TEST_FIELD_BIT(flags.bit11);

			bool joinable = arg_a721be && !TEST_FIELD_BIT(flags.bit5) && !TEST_FIELD_BIT(flags.bit4) ||
				is_player && !bit11 && !bit10;

			value10e6 = joinable;
			value10e5 = joinable && TEST_FIELD_BIT(flags.online);
			selection.field_xb3bdcf = field_xb3bdcf;
			selection.type = 2;
		}
		function_148893(&selection, 2);
	}
}

// @retail 0x2b7234
screen_load_proc c_y_menu_player_selected_screen::get_load_proc()
{
	screen_load_proc result = function_2b7152;

	switch (mode)
	{
	case 1:
		result = function_2b71f0;
		break;
	case 2:
		result = function_2b7201;
		break;
	case 3:
		result = function_2b7212;
		break;
	case 4:
		result = function_2b7223;
		break;
	}
	return result;
}

/* the first controller of a set of user flags (none when no flag is set) */
static __forceinline long user_flags_get_controller(word user_flags)
{
	long controller_index;

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
	return controller_index;
}

/* loads the screen in a mode */
// @retail 0x2b71a6
c_class_1473c9 *function_2b71a6(s_screen_parameters *parameters, long mode)
{
	c_y_menu_player_selected_screen *screen = new c_y_menu_player_selected_screen(parameters->a, parameters->b, parameters->user_flags);

	if (screen)
	{
		screen->m6c = true;
		screen->function_147f6d(parameters);
		screen->mode = mode;
		screen->list.value3a0 = mode;
	}
	return screen;
}

/* the same, unless the selected player sent the user a message of the
   mode's kind: then the clan member screen opens instead */
// @retail 0x2b70a3
c_class_1473c9 *function_2b70a3(s_screen_parameters *parameters, long mode)
{
	long controller_index = user_flags_get_controller(parameters->user_flags);
	s_name_request selection;
	c_y_menu_player_selected_screen *screen;

	XUID *xuid;

	function_14887e((s_screen_settings_54dc6c *)&selection);
	xuid = selection_get_xuid(&selection);
	if (xuid && xuid->qwUserID != 0 && online_messages_find_from(xuid, controller_index, mode))
	{
		screen = (c_y_menu_player_selected_screen *)function_2b61ce((short)parameters->user_flags, mode);
	}
	else
	{
		screen = new c_y_menu_player_selected_screen(parameters->a, parameters->b, parameters->user_flags);

		if (screen)
		{
			screen->m6c = true;
			screen->function_147f6d(parameters);
			screen->mode = mode;
			screen->list.value3a0 = mode;
		}
	}
	return screen;
}

// @retail 0x2b7152
c_class_1473c9 *__stdcall function_2b7152(s_screen_parameters *parameters)
{
	return function_2b71a6(parameters, 0);
}

// @retail 0x2b7162
c_class_1473c9 *__stdcall function_2b7162(s_screen_parameters *parameters)
{
	return function_2b71a6(parameters, 1);
}

// @retail 0x2b7173
c_class_1473c9 *__stdcall function_2b7173(s_screen_parameters *parameters)
{
	return function_2b71a6(parameters, 2);
}

// @retail 0x2b7184
c_class_1473c9 *__stdcall function_2b7184(s_screen_parameters *parameters)
{
	return function_2b71a6(parameters, 3);
}

// @retail 0x2b7195
c_class_1473c9 *__stdcall function_2b7195(s_screen_parameters *parameters)
{
	return function_2b71a6(parameters, 4);
}

// @retail 0x2b71f0
c_class_1473c9 *__stdcall function_2b71f0(s_screen_parameters *parameters)
{
	return function_2b70a3(parameters, 1);
}

// @retail 0x2b7201
c_class_1473c9 *__stdcall function_2b7201(s_screen_parameters *parameters)
{
	return function_2b70a3(parameters, 2);
}

// @retail 0x2b7212
c_class_1473c9 *__stdcall function_2b7212(s_screen_parameters *parameters)
{
	return function_2b70a3(parameters, 3);
}

// @retail 0x2b7223
c_class_1473c9 *__stdcall function_2b7223(s_screen_parameters *parameters)
{
	return function_2b70a3(parameters, 4);
}

/* the second text says whether the player can be joined */
// @retail 0x2b6e21
void c_y_menu_player_selected_screen::update_join_text()
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)find_child(6, 2, false);

	if (text)
	{
		text->value6e = true;
		if (value10e6)
		{
			text->function_253b1a(0x700024e);
		}
		else
		{
			text->function_253b1a(0x12000280);
		}
	}
}

/* a time left in minutes and seconds (a game's or a session's) */
struct s_time_left
{
	byte unknown00[0xc];
	short game_seconds;
	short session_seconds;
};

word *function_1630e0(word *buffer, const word *format, ...);

/* formats the time left with the screen's string for it */
// @retail 0x2b6db1
bool function_2b6db1(s_time_left *time, c_widget *screen, word *result_string)
{
	bool result = false;
	struct s_string_buffer
	{
		word text[0x100];
		void load(c_widget *screen, long handle) { screen->function_230134(handle, text); }
	} format;
	long minutes;
	long seconds;

	format.text[0] = 0;
	if (time->game_seconds > 0)
	{
		seconds = time->game_seconds % 60;
		minutes = time->game_seconds / 60;
		format.load(screen, 0x13000264);
		function_1630e0(result_string, format.text, minutes, seconds);
		result = true;
	}
	else if (time->session_seconds > 0)
	{
		seconds = time->session_seconds % 60;
		minutes = time->session_seconds / 60;
		format.load(screen, 0x15000265);
		function_1630e0(result_string, format.text, minutes, seconds);
		result = true;
	}
	return result;
}

dword online_friend_get_flags(XONLINE_FRIEND const *friend_);
void online_friend_get_title_name(XONLINE_FRIEND const *friend_, wchar_t *name, short name_length);
dword function_217b70(word const *text);

// @retail 0x2b635f
void function_2b635f(XONLINE_FRIEND const *friend_, c_widget *screen)
{
    word message[256], format[256], title[256];
    message[0] = 0;
    format[0] = 0;
    title[0] = 0;
    dword flags = online_friend_get_flags(friend_);
    c_class_1a2c81 *text = ((c_class_1a2c81 *)screen)->find_child(6, 2, false);
    online_friend_get_title_name(friend_, (wchar_t *)title, 256);
    if ((long)function_217b70(title) > 0)
    {
        if ((bool)((flags >> 2) & 1))
        {
            screen->function_230134(0x1500025d, format);
            function_1630e0(message, format, title);
        }
        else
        {
            screen->function_230134(0x1400025c, format);
            function_1630e0(message, format, title);
        }
    }
    else
        screen->function_230134(0x0e00024f, message);
    if (text)
    {
        text->value6e = true;
        text->function_22f52e()->set_text(message);
    }
}

word *unicode_string_append(word *destination, word const *source, long maximum_count);

struct s_friend_session_2b642e
{
    long type;
    long state;
    long activity;
    short game_seconds;
    short session_seconds;
};

#include "network_qos.h"
struct s_match_result_data
{
    XNKEY key;
    XNKID id;
    XNADDR address;
    DWORD public_filled, public_open, private_filled, private_open;
    long properties[7];
};
struct s_qos_target { XNKID kid; XNKEY key; XNADDR xna; };
extern byte g_4771c8[0x2580];
extern dword g_54d5b8;
bool __stdcall function_8c150(void *, long, unsigned __int64 *, void *, unsigned __int64 *);
void function_1a31ff();
struct s_window_manager_text;
const char *function_148956(s_window_manager_text *);
void ascii_string_to_unicode(const char *, word *, long);
long online_task_poll(long);
bool function_90160(long, s_match_result_data *);
long qos_lookup(long, long, long, s_qos_target *);
bool qos_is_complete(long);
bool session_description_decode_buffer(long, byte const *, void *);
struct s_friend_presence_status;
bool function_8d230(XONLINE_FRIEND const *, s_friend_presence_status *);
bool online_title_is_this_title(DWORD);
// @retail 0x2b6e55
void c_y_menu_player_selected_screen::v3()
{
	c_y_menu_player_selected_screen *screen = this;
	word formatted[256];
	s_global_string_buffer format;
	word name[16], group_name[16];
	formatted[0] = 0;
	format.text[0] = 0;
	if (!window_manager_channel_window_in_use(4, 4)) function_1a31ff();
	screen->update_selection();
	s_name_request selection;
	function_14887e((s_screen_settings_54dc6c *)&selection);
	XUID *user = selection_get_xuid(&selection);
	ascii_string_to_unicode(function_148956((s_window_manager_text *)&selection), name, 16);
	XUID group;
	if (function_8c150(g_4771c8, 0, (unsigned __int64 *)user, group_name, &group.qwUserID) && group.qwUserID)
	{
		format.load(0x220006bd);
		function_1630e0(formatted, format.text, name, group_name);
		((c_class_1a2c81 *)((byte *)screen + 0x80))->function_22f52e()->set_text(formatted);
	}
	else
		((c_class_1a2c81 *)((byte *)screen + 0x80))->function_22f52e()->set_text(name);
	if (screen->task_index != NONE)
	{
		switch (online_task_poll(screen->task_index))
		{
		case 0:
			break;
		case 1:
			break;
		case 2:
		{
			/* the match search finished: look up the session it found */
			s_match_result_data result;
			if (function_90160(screen->task_index, &result))
			{
				s_qos_target target;
				target.kid = result.id;
				target.key = result.key;
				target.xna = result.address;
				screen->qos_handle = qos_lookup(1, 1, 0x10000, &target);
			}
		}
			/* fall through: the task is done */
		default:
			function_6b640(screen->task_index);
			screen->task_index = NONE;
			break;
		}
	}
	else if (screen->qos_handle != NONE && qos_is_complete(screen->qos_handle))
	{
		s_qos_result information;
		if (qos_target_result(screen->qos_handle, 0, &information) &&
			session_description_decode_buffer(information.data_size, information.data, screen->unknown9c4))
		{
			screen->value10e4 = true;
			screen->value10e0 = g_54d5b8;
		}
		qos_release(screen->qos_handle);
		screen->qos_handle = NONE;
	}
	for (long i = 0; i < 8; ++i)
	{
		c_class_1a2c81 *widget = screen->find_text(i + 2);
		if (widget) widget->value6e = false;
	}
	if (screen->value10e5)
	{
		s_name_request friend_selection;
		function_14887e((s_screen_settings_54dc6c *)&friend_selection);
		s_friend_session_2b642e session;
		bool present = function_8d230(&friend_selection.field_xb3bdcf, (s_friend_presence_status *)&session);
		if (online_title_is_this_title(friend_selection.field_xb3bdcf.dwTitleID))
			screen->update_friend_details(&friend_selection.field_xb3bdcf, &session);
		else if (present)
			screen->update_friend_status(&friend_selection.field_xb3bdcf, &session);
		else function_2b635f(&friend_selection.field_xb3bdcf, (c_widget *)screen);
	}
	else screen->update_join_text();
	screen->c_class_1a2c81::v3();
}



// @retail 0x2b642e
void c_y_menu_player_selected_screen::update_friend_status(XONLINE_FRIEND const *friend_, s_friend_session_2b642e *session)
{
    c_class_1a2c81 *status = find_child(6, 3, false);
    c_class_1a2c81 *time = find_child(6, 4, false);
    word message[256], suffix[256];
    message[0] = 0;
    suffix[0] = 0;
    bool joinable = (bool)((online_friend_get_flags(friend_) >> 3) & 1);
    function_2b635f(friend_, (c_widget *)this);
    /* volatile: retail keeps session on the stack and reads it from there
       after the calls, which leaves edi to this; a plain read makes LTCG
       pass session in edi instead */
    s_friend_session_2b642e *current = *(s_friend_session_2b642e * const volatile *)&session;
    if (current->type == 1)
    {
        if (current->state)
        {
            ((c_widget *)this)->function_230134(0x11000253, message);
            goto activity;
        }
        ((c_widget *)this)->function_230134(0xe000258, message);
        goto default_suffix;
    }
    else if (current->type == 5)
    {
        ((c_widget *)this)->function_230134(0x17000259, message);
        goto default_suffix;
    }
    else if (current->activity == 1)
    {
        ((c_widget *)this)->function_230134(0x1900025a, message);
    activity:
        switch (session->state)
        {
        case 2: ((c_widget *)this)->function_230134(0x12000255, suffix); break;
        case 3: ((c_widget *)this)->function_230134(0xd000254, suffix); break;
        default:
            if (joinable) ((c_widget *)this)->function_230134(0xf000256, suffix);
            else ((c_widget *)this)->function_230134(0x13000257, suffix);
            break;
        }
    }
    else if (current->activity == 2 || current->activity == 3)
    {
        ((c_widget *)this)->function_230134(0x1500025b, message);
    default_suffix:
        ((c_widget *)this)->function_230134(0x13000257, suffix);
    }
    else
    {
        ((c_widget *)this)->function_230134(0xe00024f, message);
        goto show;
    }
    unicode_string_append(message, suffix, 256);
    message[255] = 0;
show:
    if (status)
    {
        status->value6e = true;
        status->function_22f52e()->set_text(message);
    }
    if (time && function_2b6db1((s_time_left *)session, (c_widget *)this, message))
    {
        time->value6e = true;
        time->function_22f52e()->set_text(message);
    }
}


extern dword g_54d5b8;
bool network_session_interface_has_user(XUID const *xuid);
bool network_session_manager_session_unready();
bool function_19acc6(XUID const *xuid);
long function_19adca(XUID const *xuid);
bool function_19abe4(long player_index);
long online_match_session_find(XNKID const *session_id);
bool xuid_equal(XUID const *a, XUID const *b, bool compare_guest_number);
struct s_entry_b;
struct s_entry_c;
struct s_localized_name;
struct s_localized_short_name;
s_entry_b *function_19c1f0(long key);
s_entry_c *function_19c5f0(long key);
wchar_t *localized_name_get(s_localized_name *definition);
wchar_t *localized_short_name_get(s_localized_short_name *definition);

PRIVATE __forceinline void show_friend_detail(c_text_widget_45a5e0 *widget, word *text)
{
    widget->value6e = true;
    widget->function_22f52e()->set_text(text);
}

// score 530 (was 557): Frame 0x864 versus retail 0x85c; EBP bias, flag/clock and score-scan homes remain.
// Earlier: retail biases EBP by 0x70 (lea ebp,[esp-0x70]) and keeps the friend flags in
// ESI and the timestamp in EDI; ours uses a plain frame, spills the flags to a slot and packs the six
// small locals differently.
// Frame 0x85c agrees; EBP bias 0x70, flags/time/state homes and status-helper transport remain.
// Earlier: frame 0x85c agrees; EBP bias and flag/time/state
// frame 0x85c agrees; EBP bias and flag/time/state
// Frame 0x85c agrees; EBP bias 0x70 and early flags/time/state homes remain.
// Earlier: frame 0x85c agrees; EBP bias and flag/time/state
// frame 0x85c agrees; EBP bias and flag/time/state homes differ.
// Frame 0x85c agrees; EBP bias and flags/time/state homes remain. Guard-owned state
// 576; session before elapsed test 578; word team scan 574 (three-point gain rejected); conditional
// maxima 577; shared state formatting 717.
// Earlier 577 (was 598): frame 0x85c agrees; EBP bias and flag/time/state
// carriers and status-helper convention still differ. Flag bit at uses 647;
// long active 624; long local 623; long campaign 598 kept; branch-local
// game-name table 624. Buffers, loops, calls and common exit are unchanged.
// Long joinability 600; word saved bit 625; timestamp rename 598; long count
// 577 kept; explicit score exclusion 598. Original count values are preserved.
// Earlier 577 (was 577): frame 0x85c agrees; EBP bias and flag/time/state homes remain.
// Data-branch format buffer 644; both data-branch buffers 644; word widget index 631;
// branch-assigned score maximum 575 (below threshold, frame 0x860); row increment after
// publication 577; auxiliary-buffer role names 577.
// Player/team/both candidate score values 577/577/577; key at uses 574 (below threshold);
// activity at uses 573 kept; native ages 577; native flag byte 630; combined native observations 574.
// Entry-owned formatting 605; with key 605, activity 601, both 601; original formatting restored.
// EBP bias and initial flags/timestamp homes remain; original frame, loops and all call arguments retained.
// Native real payload fields: state/activity, count, selection/totals, keys, timers, game/team,
// score arrays and the complete view 573 each; native declaration restored. EBP bias remains.
// Unsigned/signed word and signed-long flags 574/574/574; word flags with
// long local/activity/joinability results 650/584/583; local/activity pair 653;
// all native truth results 653. EBP bias and initial carriers still differ;
// original dword flags, Boolean results and activity observations retained.
// player-table 587; team-table 587.
// both-tables 592; player-table-before-search 587.
// team-table-before-search 587; both-tables-before-search 592.
// tables-score-values 592; early-tables-score-values 592.
// State/count/both at every use 572/594/594; flags/truth/row/all scalar role names
// 573/573/573/573; all observations with role names 594. Original body restored.
// Native Boolean campaign 605; signed-word count 594; both 623; with consistent state fields 623,
// explicit session true test 621, early flag producer 664, read-only local table 623, word widget ordinal 640.
// Separate time dividend/results in both arms, immutable results and clock-role homes 573/573/573;
// with native campaign/count/both/state-fields/session-test 605/594/623/623/621. All executable forms restored.
// Round 161: clock at every use 642; table at entry/pre-query/pre-widget 619/618/601; those three with consistent clock reads 654/654/643; read-only local table 573. All restored.
// flags inline 699; flags noinline 573; flags inline noinline 573; user inline 573; user noinline 573; user
// inline noinline 573; both inline noinline 573; both inline 699.
// Round 211: Main message/format/both output pointers 573 each; auxiliary game/teams/both
// output pointers 573 each; all four and message with both auxiliary outputs 573 each.
// Round 237: Complementary local/activity/joinability states independently and together
// 648/575/648/574/648/576/648; complementary campaign 573. Original truth states restored.
// Three identity-query caller-inline contracts in all seven combinations 573 each; joint
// caller-noinline contract 573. Original declarations restored.
// Round 227: Native identifier copy/assignment/CRT, complete friend copy/assignment/CRT,
// complete friend plus identifier copy/CRT 632/632/632/627/627/627/632/632 (exact-only). Restored.
// Publication helper native widget/buffer-array/both references 573 each; array input
// first 573; ordinary inline with widget/array/both references 573 each; both references
// with text-first formals 573. Original helper and every publication call restored.
// Round 380: Bit extraction after widgets at bool/byte/short/long width 642/642/640/628; in the data branch 639/639/633/640. Original extraction and carriers restored.
// Round 384: publication widget/text/external-widget-reference results under forced expansion
// 573/573/573; ordinary inline results 693/716/693; outlined void/widget results
// 716/693. Original publication helper, complete screen body and held interfaces restored.
// Round 168: Byte row/widget/fixed-team ordinals in seven subsets 675/629/570/685/668/625/679;
// all with byte saved bit 679. The team-only three-point gain is dropped. Complete status-arm
// orders 678/704/584/695/704/681/707/609; original ordinals, arm order and body restored.
// Round 409: row declared with outputs 680; widened state 598; both 700; common total 573;
// total with row/state/both 680/598/700; all with explicit status-text result 700.
// Original scalar lifetimes, state width and complete body restored.
// Round 189: real widget-array discovery cursor, long/short/byte/char query ordinals,
// bounded ordinal latch 637/614/619/619; real array-end latch 628/610/607/607. Original
// discovery restored; frame bias and initial flags/clock homes remain unresolved.
// Round 290: Activity/player result and implied-activity graphs 573/651/651; explicit session test
// combinations 571/651/651; native campaign/state combinations 658/651. Two-point gain rejected.
// Original executable source restored.
// Round 308: Complete primary-slot/team-mode/paired-total input subsets 636/606/662/668/687/667/680;
// all with long primary-slot result 679. All regress and retain ordinary EBP; original observations,
// count expressions and executable body restored. Frame bias and flag/time homes remain.
// Round 441: Signed-short dynamic search/competitor/list subsets 565/569/568/566/557/562/561;
// all dynamic plus fixed-team short 556 (two extra edits gain one against selected 557).
// Search/list short kept at 557 plain, 1964/2041 bytes; frame 0x864, unbiased EBP and flags/time homes remain.
// Round 447: Raw zero/0x80 saved bit in dword/word/byte/long at the original extraction point, then at the API result
// 613/581/618/613/648/645/648/648. Current frame 0x864 versus 0x85c; EBP bias and flag/time homes remain.
// Original executable source and all trial helper/interface changes restored.
// Lane 10 round 34 (fast; aligned counts from a difflib alignment of the listing in brackets, baseline [294]):
// retail calls the name formatter in each lookup arm with no NULL test of the name (tail-merged), so the arms
// formatting and showing directly give retail's block exactly but score 559 [285]; retail's player search ends
// with `cmp esi, -1` (a found index tested after the loop, long counter): index-after-loop form 565 [266]; bool
// campaign 588 [283]; short count 579 [297]; all four with a long final-loop counter 619 [274]; search plus long
// final counter 571 [263]. The positional score rises while the bias is missing. Bias diagnostics (none biased):
// widget array declared first, six field locals, the status call removed, the body cut after the widget loop or
// at any later block. Retail's other /O1 EBP frames bias only when a slot lands in the unbiased [-0xf0,-0x81]
// window (0x2b6208, 0x2b4059, 0x2b0565), which 0x2b65b8 and 0x2b642e do not have, so the trigger is not that.
// Round 215: player score cursor with long ordinal 557; team score cursor with long ordinal 557;
// both score cursors with long ordinals 557; player score cursor with short ordinal and retail-
// sized frame 562; team score cursor with short ordinal 549; both score cursors with short
// ordinals 557; player score cursor with real pointer difference bound 544; team score cursor with
// real array end 539; signed-short team ordinal alone on the current search/list graph 549; team
// score cursor with unsigned-word ordinal 549; pointer-difference player scan with array-end team
// scan 530. Keep t11 combined actual player/team score-array scans, plain-confirmed 557 to 530 at
// 1991/2041 bytes; all 11489 baseline matches preserved. No arithmetic, input observation or call
// tuple changes.
// @retail 0x2b65b8
void c_y_menu_player_selected_screen::update_friend_details(XONLINE_FRIEND const *friend_, s_friend_session_2b642e *session)
{
    word message[256], format[256];
    message[0] = 0;
    format[0] = 0;
    unsigned row = 0;
    dword flags = online_friend_get_flags(friend_);
    bool local = network_session_interface_has_user(&friend_->xuid);
    bool active;
    long player;
    if (network_session_manager_session_unready() && function_19acc6(&friend_->xuid))
    {
        active = true;
        player = function_19adca(&friend_->xuid);
    }
    else
    {
        active = false;
        player = NONE;
    }
    bool joinable = player != NONE && active && function_19abe4(player);
    dword now = g_54d5b8;
    bool flag7 = (bool)((flags >> 7) & 1);
    c_text_widget_45a5e0 *text[8];
    for (unsigned i = 0; i < 8; i++)
    {
        text[i] = (c_text_widget_45a5e0 *)find_text(i + 2);
        if (!text[i]) goto done;
    }
    if (task_index == NONE && qos_handle == NONE && now - value10e0 > 60000 && in_session)
        task_index = online_match_session_find((XNKID const *)((byte const *)friend_ + 0x28));
    if (joinable)
    {
        text[0]->value6e = true;
        text[0]->function_253b1a(0x18000250);
    }
    else if (active)
    {
        text[0]->value6e = true;
        text[0]->function_253b1a(0x14000251);
    }
    else if (local)
    {
        text[0]->value6e = true;
        text[0]->function_253b1a(0x13000252);
    }
    else
    {
        byte *details = (byte *)this;
        short state = *(short *)(details + 0xa62);
        if (!value10e4 || !state)
            update_friend_status(friend_, session);
        else
        {
            long campaign = *(long *)(details + 0xa68) != NONE && *(long *)(details + 0xa70) != NONE;
            long game_names[11] = {0x7000180, 0x3000439, 0x600010f, 0x7000110, 0x4000237,
                0x4000112, 0xa000114, 0xa000113, 0xb000115, 0x700010e, 0x7000180};
            long count = *(short *)(details + 0xa80);
            if (count > 0 && now - value10dc > 1000)
            {
                value10dc = now;
                value10d8 = (value10d8 + 1) % count;
            }
            if (state == 1)
            {
                short total;
                short kind = *(short *)(details + 0x9d8);
                if (kind == 0 || (kind == 1 && flag7))
                {
                    total = *(short *)(details + 0xa58) + *(short *)(details + 0xa5a);
                    ((c_widget *)this)->function_230134(0x1500026b, format);
                }
                else
                {
                    total = *(short *)(details + 0xa5c) + *(short *)(details + 0xa5e);
                    ((c_widget *)this)->function_230134(0x1700026c, format);
                }
                function_1630e0(message, format, (long)total);
                show_friend_detail(text[row++], message);
            }
            else if (state == 2 || state == 3)
            {
                if (campaign)
                {
                    ((c_widget *)this)->function_230134(0x17000259, message);
                    show_friend_detail(text[row++], message);
                }
                else
                {
                    short total;
                    short kind = *(short *)(details + 0x9d8);
                    if (kind == 0 || (kind == 1 && flag7))
                    {
                        total = *(short *)(details + 0xa58) + *(short *)(details + 0xa5a);
                        ((c_widget *)this)->function_230134(0x1500026f, format);
                    }
                    else
                    {
                        total = *(short *)(details + 0xa5c) + *(short *)(details + 0xa5e);
                        ((c_widget *)this)->function_230134(0x17000270, format);
                    }
                    function_1630e0(message, format, (long)total);
                    show_friend_detail(text[row++], message);
                }
            }
            else if (state == 4 || state == 9)
            {
                short total;
                short kind = *(short *)(details + 0x9d8);
                if (kind == 0 || (kind == 1 && flag7))
                {
                    total = *(short *)(details + 0xa58) + *(short *)(details + 0xa5a);
                    ((c_widget *)this)->function_230134(0x1600026d, format);
                }
                else
                {
                    total = *(short *)(details + 0xa5c) + *(short *)(details + 0xa5e);
                    ((c_widget *)this)->function_230134(0x1800026e, format);
                }
                function_1630e0(message, format, (long)total);
                show_friend_detail(text[row++], message);
            }
            else if (state == 5)
            {
                ((c_widget *)this)->function_230134(0xe000271, message);
                show_friend_detail(text[row++], message);
            }
            else if (state == 6)
            {
                short total = *(short *)(details + 0xa5c) + *(short *)(details + 0xa5e);
                ((c_widget *)this)->function_230134(0x1300043d, format);
                function_1630e0(message, format, (long)total);
                show_friend_detail(text[row++], message);
            }
            else if (state == 7 || state == 8)
            {
                short total = *(short *)(details + 0xa5c) + *(short *)(details + 0xa5e);
                ((c_widget *)this)->function_230134(0x16000272, format);
                function_1630e0(message, format, (long)total);
                show_friend_detail(text[row++], message);
            }
            state = *(short *)(details + 0xa62);
            if (state != 8 && state != 3 && *(short *)(details + 0xa60))
            {
                ((c_widget *)this)->function_230134(campaign ? 0xe000276 : *(short *)(details + 0xa60) == 1 ? 0x12000273 :
                               *(short *)(details + 0xa60) == 3 ? 0xe000274 : 0xf000275, message);
                show_friend_detail(text[row++], message);
            }
            if (*(short *)(details + 0xa62) == 8 || *(short *)(details + 0xa62) == 3 || *(short *)(details + 0xa60) == 1)
            {
                wchar_t *name = NULL;
                long key = *(long *)(details + 0xa70);
                if (*(long *)(details + 0xa68) != NONE && key != NONE)
                {
                    s_entry_b *entry = function_19c1f0(key);
                    if (entry)
                    {
                        ((c_widget *)this)->function_230134(0x30005f4, format);
                        name = localized_name_get((s_localized_name *)entry);
                    }
                }
                else if (key != NONE)
                {
                    s_entry_c *entry = function_19c5f0(key);
                    if (entry)
                    {
                        ((c_widget *)this)->function_230134(0x30005f4, format);
                        name = localized_short_name_get((s_localized_short_name *)entry);
                    }
                }
                if (name)
                {
                    function_1630e0(message, format, name);
                    show_friend_detail(text[row++], message);
                }
            }
            if (*(long *)(details + 0xa64))
            {
                word game[256], teams[256];
                game[0] = 0;
                teams[0] = 0;
                ((c_widget *)this)->function_230134(0x9000277, format);
                ((c_widget *)this)->function_230134(game_names[*(long *)(details + 0xa64)], game);
                ((c_widget *)this)->function_230134(details[0xa74] ? 0x4000278 : 0x3000279, teams);
                function_1630e0(message, format, teams, game);
                show_friend_detail(text[row++], message);
            }
            state = *(short *)(details + 0xa62);
            if (!campaign && (state == 3 || state == 8))
            {
                long timer = *(long *)(details + 0xa78);
                if (timer == 2)
                {
                    long seconds = *(long *)(details + 0xa7c);
                    long minutes = seconds / 60;
                    seconds %= 60;
                    ((c_widget *)this)->function_230134(0x1400027c, format);
                    function_1630e0(message, format, minutes, seconds);
                    show_friend_detail(text[row++], message);
                }
                else if (timer == 1)
                {
                    long seconds = *(long *)(details + 0xa7c);
                    long minutes = seconds / 60;
                    seconds %= 60;
                    ((c_widget *)this)->function_230134(0x1200027d, format);
                    function_1630e0(message, format, minutes, seconds);
                    show_friend_detail(text[row++], message);
                }
                for (short i = 0; i < *(short *)(details + 0xa80); i++)
                {
                    if (xuid_equal((XUID const *)(details + 0xa82), &friend_->xuid, true))
                    {
                        long best = 0;
                        long score;
                        if (!details[0xa74])
                        {
                            long const *player_scores = (long const *)(details + 0xf44);
                            for (long const *player_score = player_scores;
                                player_score - player_scores < *(short *)(details + 0xa80); ++player_score)
                                if (player_score != player_scores + i && *player_score > best)
                                    best = *player_score;
                            ((c_widget *)this)->function_230134(0xc00027a, format);
                            score = *(long *)(details + 0xf44 + i * 4);
                        }
                        else
                        {
                            long team = *(short *)(details + 0xf84 + i * 2);
                            long const *team_scores = (long const *)(details + 0x10a8);
                            for (long const *team_score = team_scores; team_score < team_scores + 8; ++team_score)
                            {
                                long team_index = team_score - team_scores;
                                if ((*(long *)(details + 0x10a4) & (1 << team_index)) && team_index != team &&
                                    *team_score > best)
                                    best = *team_score;
                            }
                            ((c_widget *)this)->function_230134(0xa00027b, format);
                            score = *(long *)(details + 0x10a8 + team * 4);
                        }
                        function_1630e0(message, format, score, best);
                        show_friend_detail(text[row++], message);
                        break;
                    }
                }
            }
            if (8 - row >= (unsigned)*(short *)(details + 0xa80)) value10d8 = 0;
            for (short i = 0; row < 8 && i < *(short *)(details + 0xa80); i++)
            {
                long index = (value10d8 + i) % *(short *)(details + 0xa80);
                ((c_widget *)this)->function_230134(0x1200027e, format);
                function_1630e0(message, format, index + 1, details + 0xb42 + index * 64);
                show_friend_detail(text[row++], message);
            }
        }
    }
done:
    return;
}
