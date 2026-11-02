// @flags /O1 /Oi /arch:SSE /Gr
/* UNKNOWN_147F6D.CPP: the screen windows (0x54d598..), where a newly loaded
   screen is placed.

   The window manager object at 0x54d598 (constructed by 0x147645) holds the
   windows of each channel: three per-controller arrays of five (the fifth,
   index 4, is shared by all controllers) and three single windows that only
   take index 4. 0x148262 maps a channel and an index to one of them. The
   windows are the screen channels of unknown_234c64.h. */

#include "unknown_11c920.h"
#include "screen_widgets.h"
#include "unknown_030290.h"
#include "unknown_19b510.h"
#include "unknown_19b516.h"
#include "unknown_234c64.h"
#include "globals.h"
#include "online_tasks.h"
#include <string.h>

c_window_manager g_54d598;

// @retail 0x147645
c_window_manager::c_window_manager()
{
}

long g_47ff54;

struct s_user_interface_shared_globals *ui_mode_definition_get(void);

void function_236299(long sound);

s_profile_edit g_54e5d0;

struct s_player_profile; /* player_slots.cpp */
void player_slot_get_profile(long index, s_player_profile *profile, long *profile_index);
void __stdcall player_slot_store_profile(long player, s_player_profile_settings *settings, long profile_index);
void __stdcall function_215367(long player, long profile_index, void *data, long flags);
void __stdcall function_2153dd(long player, long profile_index, s_player_profile_settings *settings, long flags);

// @retail 0x147f6d
void c_class_1473c9::function_147f6d(s_screen_parameters *parameters)
{
	long index = v21();
	c_window_channel *window;

	switch (parameters->a)
	{
	case 0:
		window = index == 4 ? &g_54d598.window_0 : 0;
		break;
	case 1:
		window = &g_54d598.windows_1[index];
		break;
	case 2:
		window = index == 4 ? &g_54d598.window_2 : 0;
		break;
	case 3:
		window = &g_54d598.windows_3[index];
		break;
	case 4:
		window = index == 4 ? &g_54d598.window_4 : 0;
		break;
	case 5:
		window = &g_54d598.windows_5[index];
		break;
	default:
		window = 0;
		break;
	}

	if (window)
	{
		if (!(parameters->type & 1) && screen_id != 9)
		{
			function_236299(3);
		}
		window->set_next(this, parameters);
	}
	else
	{
		this->~c_class_1473c9();
		function_1a4826(this);
	}
}

/* the screen definitions of the user interface globals tag: a block of tag
   references whose tags start with their screen id at +4 */
struct s_screen_reference
{
	dword group_tag;
	long tag_index;
};

struct s_screen_references
{
	byte unknown00[8];
	long count;
	s_screen_reference *references;
};

// @retail 0x147f4f
long function_147f4f()
{
	long result = NONE;

	if (g_54d598.windows_5[4].focus)
	{
		c_class_1473c9 *screen = g_54d598.windows_5[4].focus->get_screen();
		if (screen)
		{
			result = screen->screen_id;
		}
	}
	return result;
}

// @retail 0x148098
long function_148098(long screen_id)
{
	s_screen_references *references = (s_screen_references *)ui_mode_definition_get();
	long result = NONE;

	if (references)
	{
		for (long i = 0; i < references->count; i++)
		{
			s_screen_reference *reference = &references->references[i];
			if (reference->tag_index != NONE &&
				((s_screen_definition *)g_4e3b44[reference->tag_index & 0xffff].bytes)->screen_id == screen_id)
			{
				result = reference->tag_index;
				break;
			}
		}
	}
	return result;
}

// @retail 0x1480ff
long function_1480ff(long screen_id)
{
	long result = function_148098(screen_id);

	if (result == NONE && screen_id != g_47ff54)
	{
		g_47ff54 = screen_id;
	}
	return result;
}

long function_146840(void);
bool window_manager_any_window_in_use(void);
void function_125a90(long value);

/* resumes the game time once no window is in use */
// @retail 0x1482c4
void function_1482c4(void)
{
	if ((byte)function_146840() && !window_manager_any_window_in_use())
	{
		g_510c54->unknown01 = false;
		function_125a90(1);
	}
}

// @retail 0x148262
c_window_channel *function_148262(long channel, long index)
{
	switch (channel)
	{
	case 0:
		return index == 4 ? &g_54d598.window_0 : 0;
	case 1:
		return &g_54d598.windows_1[index];
	case 2:
		return index == 4 ? &g_54d598.window_2 : 0;
	case 3:
		return &g_54d598.windows_3[index];
	case 4:
		return index == 4 ? &g_54d598.window_4 : 0;
	case 5:
		return &g_54d598.windows_5[index];
	}
	return &g_54d598.default_window;
}

/* whether a window has a screen or one coming */
// @retail 0x1473b6
inline bool function_1473b6(c_window_channel *window)
{
	return window->current != 0 || window->next != 0;
}

/* whether any window has a screen or one coming */
// @retail 0x147d13
bool function_147d13()
{
	bool active = false;

	if (function_1473b6(&g_54d598.default_window))
	{
		return true;
	}
	for (long i = 0; !active && i < 5; i++)
	{
		active = function_1473b6(&g_54d598.windows_5[i]) || function_1473b6(&g_54d598.windows_3[i]) ||
			function_1473b6(&g_54d598.windows_1[i]) || function_1473b6(&g_54d598.window_0) ||
			function_1473b6(&g_54d598.window_4) || function_1473b6(&g_54d598.window_2);
	}
	return active;
}

#define NUMBEROF(array) (sizeof(array) / sizeof((array)[0]))

bool window_manager_window_has_dialog(long channel, long index, long dialog_id);
void function_23538b(c_window_channel *channel);
void window_manager_window_2_start_animation(void);
void function_236299(long sound);

/* closes the dialog window, the windows of channel 3 and the channel 1
   windows that show one of the screens a dialog replaces */
// @retail 0x148523
void function_148523()
{
	if (function_1473b6(&g_54d598.window_4))
	{
		long screen_ids[0x23] =
		{
			0x2f, 0x34, 0x74, 0x75, 0x76, 0x7a, 0x7d, 0x7e, 0x7f, 0x83, 0x84, 0x85,
			0x86, 0x8c, 0x8d, 0x97, 0x98, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xaf,
			0xb0, 0xba, 0xbb, 0xb1, 0xb2, 0xbc, 0xbd, 0xbe, 0xb3, 0xb4, 0xb5
		};

		for (long i = 0; i < 5; i++)
		{
			if (function_1473b6(&g_54d598.windows_3[i]))
			{
				function_23538b(&g_54d598.windows_3[i]);
			}
			if (function_1473b6(&g_54d598.windows_1[i]))
			{
				for (dword j = 0; j < NUMBEROF(screen_ids); j++)
				{
					if (window_manager_window_has_dialog(1, i, screen_ids[j]))
					{
						function_23538b(&g_54d598.windows_1[i]);
						break;
					}
				}
			}
		}
		function_23538b(&g_54d598.window_4);
		function_236299(4);
	}
	window_manager_window_2_start_animation();
}

bool game_loop_running();
bool function_138820();
void function_199e2e(bool close);
bool __stdcall dialog_callback_dashboard_network_configuration(long controller_index);
bool __stdcall dialog_callback_leave_sessions(long controller_index);
bool __stdcall dialog_callback_dashboard_online_menu(long controller);
bool __stdcall dialog_callback_sign_out_to_main_menu(long controller);
bool __stdcall pause_game_quit_confirmed(long controller_index);

/* shows the dialog of a lost connection or failed sign in (by the error),
   unless one shows already, and leaves the game for most of them */
// @retail 0x1486b8
void function_1486b8(long error, bool keep)
{
	dialog_choice_callback first = dialog_callback_sign_out_to_main_menu;
	dialog_choice_callback second = dialog_callback_sign_out_to_main_menu;
	bool choice = false;
	bool leave = false;
	long dialog_id;

	switch (error)
	{
	case 0:
	case 1:
		return;
	case 3:
		dialog_id = 0x24;
		first = dialog_callback_leave_sessions;
		choice = true;
		leave = true;
		break;
	case 2:
		dialog_id = 0x27;
		first = dialog_callback_dashboard_online_menu;
		choice = true;
		if (keep)
		{
			second = NULL;
		}
		else
		{
			leave = true;
		}
		break;
	case 4:
		dialog_id = 0x23;
		first = dialog_callback_dashboard_network_configuration;
		choice = true;
		leave = true;
		break;
	case 6:
		dialog_id = 0x9a;
		first = dialog_callback_dashboard_online_menu;
		choice = true;
		if (keep)
		{
			second = NULL;
			break;
		}
		leave = true;
		break;
	case 7:
		dialog_id = 0x26;
		leave = true;
		break;
	case 8:
		dialog_id = 0x25;
		leave = true;
		break;
	case 5:
		dialog_id = 0x38;
		first = dialog_callback_dashboard_network_configuration;
		choice = true;
		leave = true;
		break;
	case 9:
		dialog_id = 0x38;
		first = dialog_callback_dashboard_network_configuration;
		choice = true;
		leave = true;
		break;
	default:
		dialog_id = 0x38;
		first = dialog_callback_dashboard_network_configuration;
		choice = true;
		leave = true;
		break;
	}

	bool in_game = game_loop_running() && g_4e6948->state == 1 && !function_138820();
	if (dialog_id == 0x38 && in_game)
	{
		choice = false;
		dialog_id = 0xb8;
		first = pause_game_quit_confirmed;
	}
	if (!window_manager_window_has_dialog(1, 4, dialog_id))
	{
		if (choice)
		{
			dialog_choice_show(1, dialog_id, 4, (long)NONE, first, second, 0);
		}
		else
		{
			dialog_ok_show(1, dialog_id, 4, 0xffff, first, 0);
		}
	}
	if (leave)
	{
		function_199e2e(true);
	}
}

/* takes a screen out of its window */
// @retail 0x148148
void function_148148(c_class_1473c9 *screen)
{
	c_window_channel *window = function_148262(screen->v20(), screen->v21());
	bool active;

	window->remove(screen);
	active = function_1473b6(&g_54d598.default_window) || function_1473b6(&g_54d598.window_0) ||
		function_1473b6(&g_54d598.window_4) || function_1473b6(&g_54d598.window_2);
	for (long i = 0; !active && i < 5; i++)
	{
		active = function_1473b6(&g_54d598.windows_5[i]) || function_1473b6(&g_54d598.windows_3[i]) ||
			function_1473b6(&g_54d598.windows_1[i]);
	}
	if (!active)
	{
		g_54d598.active = false;
	}
	for (unsigned long i = 0; i < 0x23; i++)
	{
		if (g_54d598.screens[i] == screen)
		{
			g_54d598.screens[i] = 0;
		}
	}
}

// @retail 0x14887e
void function_14887e(s_screen_settings_54dc6c *settings)
{
	if (settings)
	{
		*settings = g_54d598.settings;
	}
}

/* the screen a window shows */
// @retail 0x148d91
c_class_1473c9 *function_148d91(long channel, long index)
{
	c_window_channel *window;

	switch (channel)
	{
	case 0:
		window = index == 4 ? &g_54d598.window_0 : 0;
		break;
	case 1:
		window = &g_54d598.windows_1[index];
		break;
	case 2:
		window = index == 4 ? &g_54d598.window_2 : 0;
		break;
	case 3:
		window = &g_54d598.windows_3[index];
		break;
	case 4:
		window = index == 4 ? &g_54d598.window_4 : 0;
		break;
	case 5:
		window = &g_54d598.windows_5[index];
		break;
	default:
		window = &g_54d598.default_window;
		break;
	}
	return window->focus;
}

/* leaves the window's current screen */
// @retail 0x14800c
void function_14800c(long channel, long index)
{
	c_window_channel *window;

	switch (channel)
	{
	case 2:
		g_54d598.window_2.v7();
		break;
	case 5:
		((c_window_channel *)&g_54d598.windows_5[index])->v7();
		break;
	case 3:
		((c_window_channel *)&g_54d598.windows_3[index])->v7();
		break;
	}
	function_236299(4);
}

// @retail 0x148044
bool function_148044(long channel, long index, long value)
{
	bool result = false;
	c_window_channel *window;

	switch (channel)
	{
	case 2:
		g_54d598.window_2.v7();
		break;
	case 3:
		((c_window_channel *)&g_54d598.windows_3[index])->v7();
		break;
	case 5:
		result = ((c_window_channel_4599a8 *)&g_54d598.windows_5[index])->v12(value) > 0;
		break;
	}
	if (result)
	{
		function_236299(4);
	}
	return result;
}

/* whether a screen of this id has a definition */
// @retail 0x1480ed
bool function_1480ed(long screen_id)
{
	bool result = function_148098(screen_id) != NONE;

	return result;
}

/* remembers a screen (once) */
// @retail 0x148119
void function_148119(c_class_1473c9 *screen)
{
	unsigned long i;

	for (i = 0; i < 0x23; i++)
	{
		if (g_54d598.screens[i] == screen)
		{
			return;
		}
	}
	for (i = 0; i < 0x23; i++)
	{
		if (!g_54d598.screens[i])
		{
			g_54d598.screens[i] = screen;
			return;
		}
	}
}

/* whether a window's current screen has this id */
// @retail 0x148222
long function_148222(long channel, long index, long screen_id)
{
	c_class_1473c9 *screen = function_148262(channel, index)->current;

	if (screen && screen->screen_id == screen_id)
	{
		return 1;
	}
	return 0;
}

void function_23538b(c_window_channel *channel);

// @retail 0x148241
void function_148241(long channel, long index, long screen_id)
{
	c_window_channel *window = function_148262(channel, index);

	if (window && window->current && window->current->screen_id == screen_id)
	{
		function_23538b(window);
	}
}

struct s_window_manager_text
{
	long type;
	char text04[0xc];
	char text10[1];
};

// @retail 0x148956
const char *function_148956(s_window_manager_text *text)
{
	const char *result = "";

	switch (text->type)
	{
	case 1:
		result = text->text10;
		break;
	case 2:
		result = text->text10;
		break;
	case 3:
		result = text->text04;
		break;
	}
	return result;
}

// @retail 0x14896e
void function_14896e(s_window_manager_754 *a, s_window_manager_df6 *b)
{
	*a = g_54d598.team_details;
	*b = g_54d598.team_member_details;
}

// @retail 0x148995
void function_148995(s_window_manager_e94 *value)
{
	if (value)
	{
		g_54d598.me94 = *value;
	}
	else
	{
		memset(&g_54d598.me94, 0, sizeof(g_54d598.me94));
	}
}

// @retail 0x148a58
void function_148a58()
{
	if (g_54d598.mf04 != NONE)
	{
		if (!(bool)(((dword)g_54d598.mf04 >> 21) & 1))
		{
			function_215367(NONE, g_54d598.mf04, g_54d598.mf08, 0);
		}
		g_54d598.mf04 = NONE;
	}
	memset(g_54d598.mf08, 0, sizeof(g_54d598.mf08));
}

// @retail 0x148a8d
void function_148a8d()
{
	g_54d598.mf04 = NONE;
	memset(g_54d598.mf08, 0, sizeof(g_54d598.mf08));
}

bool __stdcall dialog_callback_dashboard_memory(long controller);
bool __stdcall dialog_callback_dashboard_memory_for_game_variant(long controller);
bool __stdcall function_2523b7(long controller);

/* tells the controllers why a game variant could not be saved */
// @retail 0x148aa3
void function_148aa3(long error, word controller_flags)
{
	switch (error)
	{
	case 1:
		dialog_choice_show(3, 0x4b, 4, controller_flags, dialog_callback_dashboard_memory, function_2523b7, 0);
		break;
	case 2:
		dialog_choice_show(3, 0x4f, 4, controller_flags, dialog_callback_dashboard_memory_for_game_variant, function_2523b7, 0);
		break;
	default:
		dialog_choice_show(3, 0xd, 4, controller_flags, dialog_callback_dashboard_memory_for_game_variant, function_2523b7, 0);
		break;
	}
}

/* starts editing a player's profile settings */
// @retail 0x148ada
void profile_edit_begin(long player, s_player_profile_settings *settings, long profile_index)
{
	g_54e5d0.player = player;
	g_54e5d0.profile_index = profile_index;
	memcpy(&g_54e5d0.settings, settings, sizeof(g_54e5d0.settings));
}

/* writes the edited settings back to the profile */
// @retail 0x148b8c
void profile_edit_save()
{
	long profile_index = g_54e5d0.profile_index;

	if (profile_index != NONE)
	{
		long player = g_54e5d0.player;
		long pinned = player < 0 ? 0 : (player > 3 ? 3 : player);

		if (pinned == player)
		{
			byte profile[0x1e0];
			long found_index;

			player_slot_get_profile(player, (s_player_profile *)profile, &found_index);
			if (found_index == profile_index)
			{
				player_slot_store_profile(player, &g_54e5d0.settings, found_index);
				return;
			}
		}
		if (!(bool)(((dword)profile_index >> 21) & 1))
		{
			function_2153dd(player, profile_index, &g_54e5d0.settings, 0);
		}
	}
}

/* saves the edited settings and stops editing */
// @retail 0x148c21
void function_148c21()
{
	memset(&g_54e5d0.settings, 0, sizeof(g_54e5d0.settings));
	g_54e5d0.player = NONE;
	g_54e5d0.profile_index = NONE;
}

/* unknown_216c40.cpp */
long saved_game_file_size_in_blocks(long size);
long saved_game_file_type_size_in_blocks(long type);

/* the blocks a saved film takes */
// @retail 0x148c9d
long saved_film_size_in_blocks()
{
	return saved_game_file_size_in_blocks(0x40c400);
}

/* the blocks a player profile, a game variant and a saved film take
   together, and one more */
// @retail 0x148d0d
long minimal_storage_size_in_blocks()
{
	long profile = saved_game_file_type_size_in_blocks(1);
	return saved_game_file_type_size_in_blocks(0) + profile + saved_film_size_in_blocks() + 1;
}

void function_236299(long sound);
void __stdcall function_148b27(long index);

/* stops editing the game variant: a built-in one only plays a sound */
// @retail 0x148a2c
void function_148a2c()
{
	if (g_54e49c != NONE)
	{
		if (!(bool)(((dword)g_54e49c >> 21) & 1))
		{
			function_148b27(g_54e49c);
		}
		else
		{
			function_236299(2);
		}
	}
	g_54e49c = NONE;
}

/* the same for the player profile */
// @retail 0x148afb
void function_148afb()
{
	if (g_54e5d0.profile_index != NONE)
	{
		if (!(bool)(((dword)g_54e5d0.profile_index >> 21) & 1))
		{
			function_148b27(g_54e5d0.profile_index);
		}
		else
		{
			function_236299(2);
		}
	}
	g_54e5d0.profile_index = NONE;
}

void function_121040(long value);

// @retail 0x148cfc
void function_148cfc(long value)
{
	g_54d598.m1220 = true;
	g_54d598.m1224 = value;
	function_121040(value);
}

// @retail 0x148d42
void function_148d42(long value)
{
	if (value < 0)
	{
		g_54d598.m1230 = 0;
	}
	else
	{
		g_54d598.m1230 = value > 3 ? 3 : value;
	}
}

struct s_entry_a;
s_entry_a *function_19c270(long key0, long key1);

// @retail 0x148d61
s_entry_a *function_148d61()
{
	return function_19c270(g_54d598.m1228, g_54d598.m122c);
}

// @retail 0x148d73
char *function_148d73()
{
	char *result = 0;
	byte *entry = (byte *)function_148d61();
	char *name = (char *)(entry + 8);

	if (entry && name && *name)
	{
		result = name;
	}
	return result;
}

// @retail 0x148bff
void profile_edit_end()
{
	profile_edit_save();
	memset(&g_54e5d0.settings, 0, sizeof(g_54e5d0.settings));
	g_54e5d0.player = NONE;
	g_54e5d0.profile_index = NONE;
}

/* the screen that waits for an online task (lane M's unknown_1a2ca7.cpp) */
struct s_online_task_screen_view
{
	byte unknown000[0x610];
	long callback;
	long value;
	long context;
	long task_index;
};

c_class_1473c9 *__stdcall online_task_screen_load(s_screen_parameters *parameters);

void network_session_manager_leave_session_a(bool close);
void network_session_manager_leave_session_b(bool close);
void function_6cb60(void);

/* leaves both sessions and lets go of the tasks the online task screens of
   channel 1 wait for */
// @retail 0x148823
void function_148823()
{
	network_session_manager_leave_session_a(true);
	network_session_manager_leave_session_b(true);
	for (long i = 0; i < 5; i++)
	{
		c_class_1473c9 *screens[3] =
		{
			g_54d598.windows_1[i].current,
			g_54d598.windows_1[i].next,
			g_54d598.windows_1[i].previous
		};

		for (dword j = 0; j < NUMBEROF(screens); j++)
		{
			c_class_1473c9 *screen = screens[j];

			if (screen && screen->screen_id == 0xb6)
			{
				((s_online_task_screen_view *)screen)->task_index = NONE;
			}
		}
	}
	function_6cb60();
}


/* loads the screen that waits for an online task, for one controller (all
   of them when it is 4 or more) */
// @retail 0x1487c3
void function_1487c3(long controller_index, long task_index, long callback, long value, long context)
{
	s_screen_parameters parameters;
	s_online_task_screen_view *screen;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, controller_index < 4 ? 1 << controller_index : 0xff, 1, 4, (long)online_task_screen_load);
	screen = (s_online_task_screen_view *)parameters.load(&parameters);
	screen->task_index = task_index;
	screen->callback = callback;
	screen->value = value;
	screen->context = context;
}

/* the selected player as the online screens pass it (the window manager's
   settings): a user (type 1) or a friend (type 2), each starting with the
   player's id */
#pragma pack(push, 4)
struct s_selection_xuid
{
	unsigned __int64 id;
	dword flags;
};

struct s_name_request
{
	long type;
	union
	{
		s_selection_xuid user_xuid;
		s_selection_xuid friend_xuid;
	};
	byte unknown10[0x78 - 0x10];
};
#pragma pack(pop)

/* the selected player's id */
static inline s_selection_xuid *selection_get_xuid(s_name_request *selection)
{
	s_selection_xuid *result = NULL;

	switch (selection->type)
	{
	case 1:
		result = &selection->user_xuid;
		break;
	case 2:
		result = &selection->friend_xuid;
		break;
	}
	return result;
}

struct _XUID;
long player_slot_find_signed_in(long mode);
long function_abc70(long controller_index, _XUID const *xuid);

/* selects the player the online screens act on (mode 2 keeps the clan
   lookups); a signed-in player's clans are looked up when mode is 0 */
// @retail 0x148893
void __stdcall function_148893(s_name_request *request, long mode)
{
	if (mode != 2)
	{
		if (g_54d598.team_task != NONE)
		{
			function_6b640(g_54d598.team_task);
			g_54d598.team_task = NONE;
		}
		if (g_54d598.team_members_task != NONE)
		{
			function_6b640(g_54d598.team_members_task);
			g_54d598.team_members_task = NONE;
		}
		memset(&g_54d598.team_details, 0, sizeof(g_54d598.team_details));
		memset(&g_54d598.team_member_details, 0, sizeof(g_54d598.team_member_details));
	}
	if (request)
	{
		s_name_request *selection = (s_name_request *)&g_54d598.settings;

		g_54d598.settings = *(s_screen_settings_54dc6c *)request;
		if (!mode)
		{
			s_selection_xuid *xuid = selection_get_xuid(selection);

			if (xuid && xuid->id && !(xuid->flags & 3))
			{
				long controller_index = player_slot_find_signed_in(1);

				if (controller_index >= 0 && controller_index < 4)
				{
					g_54d598.team_task = function_abc70(controller_index, (_XUID const *)xuid);
				}
			}
		}
	}
	else
	{
		memset(&g_54d598.settings, 0, sizeof(g_54d598.settings));
	}
}

void function_19987f(void);
void window_manager_reset_all_windows(void);
void function_1a479a(void);

/* the user interface's dispose (the subsystem table at 0x4414e0) */
// @retail 0x14783f
void function_14783f(void)
{
	function_19987f();
	window_manager_reset_all_windows();
	function_1a479a();
}

/* and its dispose from the old map: every window lets go of its screens */
// @retail 0x147920
void function_147920(void)
{
	long i;

	g_54d598.default_window.dispose();
	g_54d598.shell_mode = NONE;
	for (i = 0; i < 5; i++)
	{
		((c_window_channel *)&g_54d598.windows_5[i])->dispose();
		((c_window_channel *)&g_54d598.windows_3[i])->dispose();
		((c_window_channel *)&g_54d598.windows_1[i])->dispose();
		if (i == 4)
		{
			g_54d598.window_0.dispose();
			g_54d598.window_4.dispose();
			g_54d598.window_2.dispose();
		}
	}
	g_54d598.mf04 = NONE;
	g_54e5d0.profile_index = NONE;
	g_54d598.main_menu_music.m10 = NONE;
	g_54d598.active = false;
	g_54d598.main_menu_music.m0 = 0;
	g_54d598.main_menu_music.m4 = 0;
	g_54d598.main_menu_music.mc = 0;
	g_54d598.main_menu_music.m8 = 0;
}

bool __stdcall dialog_callback_dashboard_memory_for_profile(long controller);

/* tells the controllers why a game variant could not be loaded */
// @retail 0x148ca8
void function_148ca8(long error, short controller_flags)
{
	switch (error)
	{
	case 1:
		dialog_choice_show(3, 0x4a, 4, controller_flags, dialog_callback_dashboard_memory, function_2523b7, 0);
		break;
	case 2:
		dialog_choice_show(3, 0x4e, 4, controller_flags, dialog_callback_dashboard_memory_for_profile, function_2523b7, 0);
		break;
	case 4:
		dialog_choice_show(3, 0xc, 4, controller_flags, dialog_callback_dashboard_memory_for_profile, function_2523b7, 0);
		break;
	}
}

bool __stdcall function_215f40(long arg_9db745, byte *buffer);
void unicode_string_copy(word *destination, const word *source, long maximum_count);
void function_238c21(long controller, long type, word *name, long maximum_count);
extern long g_55c154;

/* reads the name saved for the profile into the edited profile and opens the
   keyboard on it; tells the controller when it cannot be read */
// @retail 0x148c3e
bool function_148c3e(long controller, long type)
{
	word name[0x80];
	bool result = false;

	g_55c154 = 0;
	if (function_215f40(0, (byte *)name))
	{
		long maximum_count = 0x20;
		long const *maximum_reference = &maximum_count;
		unicode_string_copy(g_54e5d0.settings.name, name, *maximum_reference);
		function_238c21(controller, type, g_54e5d0.settings.name, maximum_count);
		result = true;
	}
	else
	{
		function_148ca8(g_55c154, controller);
	}
	return result;
}

word function_1901fc(void);
c_class_1473c9 *__stdcall function_23334f(s_screen_parameters *parameters);

/* opens the postgame statistics */
// @retail 0x1484f4
void function_1484f4(void)
{
	s_screen_parameters parameters;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 4, 0, function_1901fc(), 5, 4, (long)function_23334f);
	parameters.load(&parameters);
}

void function_2352c0(c_window_channel *channel);

/* every window does 0x2352c0 */
// @retail 0x147ebe
void function_147ebe(void)
{
	function_2352c0(&g_54d598.default_window);
	for (long i = 0; i < 5; i++)
	{
		function_2352c0(&g_54d598.windows_5[i]);
		function_2352c0(&g_54d598.windows_3[i]);
		function_2352c0(&g_54d598.windows_1[i]);
	}
	function_2352c0(&g_54d598.window_0);
	function_2352c0(&g_54d598.window_4);
	function_2352c0(&g_54d598.window_2);
}

/* takes the user interface globals' color */
// @retail 0x147f1e
void function_147f1e(void)
{
	s_type_954545 *globals = ui_definition_get();

	if (globals)
	{
		g_54d598.color14.red = globals->value60.red;
		g_54d598.color14.green = globals->value60.green;
		g_54d598.color14.blue = globals->value60.blue;
	}
}

void function_23536a(c_window_channel *channel, c_class_1473c9 *screen);
bool function_235246(c_window_channel *channel);
long function_1910b8(long user_index);

/* function_148262, which retail inlines here */
static __forceinline c_window_channel *window_manager_get_window(long channel, long index)
{
	switch (channel)
	{
	case 0:
		return index == 4 ? &g_54d598.window_0 : 0;
	case 1:
		return &g_54d598.windows_1[index];
	case 2:
		return index == 4 ? &g_54d598.window_2 : 0;
	case 3:
		return &g_54d598.windows_3[index];
	case 4:
		return index == 4 ? &g_54d598.window_4 : 0;
	case 5:
		return &g_54d598.windows_5[index];
	}
	return &g_54d598.default_window;
}

/* focuses the widget in a window */
// @retail 0x148dfc
void function_148dfc(long channel, long index, c_class_1473c9 *screen)
{
	function_23536a(window_manager_get_window(channel, index), screen);
}

/* whether the user's focused screen takes the user's input: the user's own
   windows first, then the shared ones */
// @retail 0x148e6d
bool __stdcall function_148e6d(long user_index)
{
	bool result = false;
	long index = function_1910b8(user_index);

	if (index == NONE)
	{
		index = 4;
	}
	for (;;)
	{
		c_window_channel *window;

		if (index == 4 && function_235246(window = &g_54d598.window_0))
			result = window->function_235276(user_index);
		else if (function_235246(&g_54d598.windows_1[index]))
			result = g_54d598.windows_1[index].function_235276(user_index);
		else if (index == 4 && function_235246(&g_54d598.window_2))
			result = g_54d598.window_2.function_235276(user_index);
		else if (function_235246(&g_54d598.windows_3[index]))
			result = g_54d598.windows_3[index].function_235276(user_index);
		else if (index == 4 && function_235246(window = &g_54d598.window_4))
			result = window->function_235276(user_index);
		else if (function_235246(&g_54d598.windows_5[index]))
			result = g_54d598.windows_5[index].function_235276(user_index);
		if (index == 4)
		{
			break;
		}
		index = 4;
		if (result)
		{
			break;
		}
	}
	return result;
}

/* the same search with 0x235294 */
// @retail 0x148f36
bool __stdcall function_148f36(long controller)
{
	bool result = false;
	long index = function_1910b8(controller);

	if (index == NONE)
	{
		index = 4;
	}
	for (;;)
	{
		c_window_channel *window;

		if (index == 4 && function_235246(window = &g_54d598.window_0))
			result = window->function_235294(controller);
		else if (function_235246(&g_54d598.windows_1[index]))
			result = g_54d598.windows_1[index].function_235294(controller);
		else if (index == 4 && function_235246(&g_54d598.window_2))
			result = g_54d598.window_2.function_235294(controller);
		else if (function_235246(&g_54d598.windows_3[index]))
			result = g_54d598.windows_3[index].function_235294(controller);
		else if (index == 4 && function_235246(window = &g_54d598.window_4))
			result = window->function_235294(controller);
		else if (function_235246(&g_54d598.windows_5[index]))
			result = g_54d598.windows_5[index].function_235294(controller);
		if (index == 4)
		{
			break;
		}
		index = 4;
		if (result)
		{
			break;
		}
	}
	return result;
}

/* whether the user's frontmost screen takes the user's input */
// @retail 0x148fff standard
bool __stdcall function_148fff(long user_index)
{
	bool result = false;
	long index = function_1910b8(user_index);

	if (index == NONE)
	{
		index = 4;
	}
	for (;;)
	{
		c_window_channel *window = 0;
		bool found = false;

		if (index == 4 && function_1473b6(&g_54d598.window_0))
			result = g_54d598.window_0.function_235276(user_index);
		else if (function_1473b6(&g_54d598.windows_1[index]))
		{
			window = &g_54d598.windows_1[index];
			found = true;
		}
		else if (index == 4 && function_1473b6(&g_54d598.window_2))
			result = g_54d598.window_2.function_235276(user_index);
		else if (function_1473b6(&g_54d598.windows_3[index]))
		{
			window = &g_54d598.windows_3[index];
			found = true;
		}
		else if (index == 4 && function_1473b6(&g_54d598.window_4))
			result = g_54d598.window_4.function_235276(user_index);
		else if (function_1473b6(&g_54d598.windows_5[index]))
		{
			window = &g_54d598.windows_5[index];
			found = true;
		}
		if (found)
		{
			result = window->function_235276(user_index);
		}
		if (index == 4)
		{
			break;
		}
		index = 4;
		if (result)
		{
			break;
		}
	}
	return result;
}

long player_slot_find_session_user(long controller_id);

/* whether the controller's player slot is signed in to its user */
// @retail 0x147da5
bool function_147da5(long controller_id)
{
	bool result = false;
	long user = player_slot_find_session_user(controller_id);

	if (user != NONE)
	{
		result = function_148e6d(user);
	}
	return result;
}

void player_slots_refresh_profile_indices(void);
short player_slot_count_active(void);
bool function_6c7e0();
void function_1906b4(void);
bool function_199df9(bool offline, bool system_link);
void function_199a57(void);
void function_199a03(long mode);
c_class_1473c9 *__stdcall press_start_screen_open(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall legalese_open(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall main_menu_open(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2310b7(s_screen_parameters *parameters);
extern dword g_54d5b8;
extern dword g_54d5bc;

struct s_controller_event;
bool function_155f60(void);
void window_manager_handle_event(s_event const *event);
bool function_23515d(c_window_channel *channel, s_event *event);

/* sends input to the shared windows, then the controller's windows */
// @retail 0x147dbe
void function_147dbe(s_controller_event *event)
{
	s_event *message = (s_event *)event;

	if (function_155f60())
	{
		window_manager_handle_event(message);
	}
	else
	{
		long index = 4;
		for (;;)
		{
			bool handled = false;
			if (index == 4 && function_1473b6(&g_54d598.window_0))
				handled = function_23515d(&g_54d598.window_0, message);
			else if (function_1473b6(&g_54d598.windows_1[index]))
				handled = function_23515d(&g_54d598.windows_1[index], message);
			else if (index == 4 && function_1473b6(&g_54d598.window_2))
				handled = function_23515d(&g_54d598.window_2, message);
			else if (function_1473b6(&g_54d598.windows_3[index]))
				handled = function_23515d(&g_54d598.windows_3[index], message);
			else if (index == 4 && function_1473b6(&g_54d598.window_4))
				handled = function_23515d(&g_54d598.window_4, message);
			else if (function_1473b6(&g_54d598.windows_5[index]) && !function_1473b6(&g_54d598.windows_3[index]))
				handled = function_23515d(&g_54d598.windows_5[index], message);

			if (handled)
				break;
			if (index == 4)
			{
				index = function_1910b8(message->unknown04);
				if (index != NONE)
					continue;
			}
			window_manager_handle_event(message);
			break;
		}
	}
	g_54d5bc = g_54d5b8;
}

/* main_menu.cpp */
extern bool g_main_menu_campaign_chosen;
/* the legal screen shows once */
bool g_55e734;

/* opens the main screen for the reason the game went back to the menus.
   Standard convention (see docs/DECOMPILING.md):
   1. Retail keeps it __stdcall (the reason on the stack, ret 4; case 3 even
      keeps its bool in the reason's stack slot). With the marker this body
      matches byte for byte; without it LTCG passes the reason in ebx or edi.
   2. No data or code in retail holds its address. Its callers (0x1479c3,
      0x18e8b0, 0x18f1c0, 0x19ae0f, 0x230c7d, 0x2305e9, 0x236877, 0x2368c1,
      0x25286d) are all LTCG game code and push the reason.
   3. Tried: taking the reason's address (still a register, and a different
      body), writing three more callers (0x230c7d, 0x236877, 0x2368c1; no
      change), several switch and if shapes. */
// @retail 0x1483c3 standard
void __stdcall function_1483c3(long reason)
{
	if (!g_54d598.active)
	{
		player_slots_refresh_profile_indices();
		g_54d5bc = g_54d5b8;
	}
	g_54d598.active = true;
	if (reason > 1)
	{
		if (reason == 2 || reason != 3 && reason <= 6)
		{
			function_1484f4();
			goto done;
		}
		if (reason == 3)
		{
			bool online = function_6c7e0();

			if (function_199df9(true, online))
			{
				function_199a57();
				function_199a03(0);
			}
			goto done;
		}
		function_1906b4();
	}
	{
		short count = player_slot_count_active();
		s_screen_parameters parameters;

		parameters.field_c = 0;
		/* stored as a short (retail's or takes a sign-extended byte) */
		*(short *)&parameters.user_flags = NONE;
		parameters.a = 5;
		parameters.type = 4;
		parameters.b = 4;
		memset(parameters.id, NONE, sizeof(parameters.id));
		parameters.load = press_start_screen_open;
		if (count == 0)
		{
			if (g_54d598.value08 && !g_55e734)
			{
				parameters.load = legalese_open;
				g_55e734 = true;
			}
		}
		else if (function_6c7e0() && reason != 1)
		{
			parameters.user_flags = function_1901fc();
			if (count == 1 && g_main_menu_campaign_chosen)
			{
				parameters.load = main_menu_open;
			}
			else
			{
				parameters.load = function_2310b7;
			}
		}
		else if (count == 1)
		{
			parameters.user_flags = function_1901fc();
			parameters.load = main_menu_open;
		}
		parameters.load(&parameters);
	}
done:
	g_main_menu_campaign_chosen = false;
}

extern char g_54d5a8;
extern real g_54d5ac, g_54d5b0, g_54d5b4;
extern dword g_54e7fc;
void player_slots_initialize();

// @retail 0x1476c7
void function_1476c7()
{
    *(long *)g_54d598.unknown00 = NONE;
    g_54d598.active = false;
    g_54d598.unknown05[0] = false;
    g_54d598.unknown05[1] = false;
    g_54d598.unknown05[2] = false;
    g_54d598.value08 = false;
    g_54d598.pending_join_type = 0;
    g_54d5a8 = (char)0xff;
    g_54d5ac = 0.f;
    g_54d5b0 = 0.f;
    g_54d5b4 = 0.f;
    g_54d598.default_window.clear();
    for (long i = 0; i < 5; i++)
    {
        ((c_window_channel *)&g_54d598.windows_5[i])->clear();
        g_54d598.windows_5[i].m4 = i;
        ((c_window_channel *)&g_54d598.windows_3[i])->clear();
        g_54d598.windows_3[i].m4 = i;
        ((c_window_channel *)&g_54d598.windows_1[i])->clear();
        g_54d598.windows_1[i].m4 = i;
        if (i == 4)
        {
            g_54d598.window_0.c_window_channel::clear();
            g_54d598.window_0.m4 = i;
            g_54d598.window_4.c_window_channel::clear();
            g_54d598.window_4.m4 = i;
            g_54d598.window_2.c_window_channel::clear();
            g_54d598.window_2.m4 = i;
        }
    }
    player_slots_initialize();
    memset(&g_54d598.settings, 0, sizeof(g_54d598.settings));
    g_54d598.team_task = NONE;
    g_54d598.team_members_task = NONE;
    memset(&g_54d598.team_details, 0, sizeof(g_54d598.team_details));
    memset(&g_54d598.team_member_details, 0, sizeof(g_54d598.team_member_details));
    memset(g_54d598.unknowne88, 0, sizeof(g_54d598.unknowne88));
    g_54d598.m1220 = false;
    g_54d598.m1234 = false;
    g_54d598.m1264 = 0;
    memset(&g_4ee4c4, 0, sizeof(g_4ee4c4));
    g_54d598.value08 = false;
    g_54d598.mf04 = NONE;
    g_54d598.profile_edit.player = NONE;
    g_54d598.profile_edit.profile_index = NONE;
    g_54d598.m1224 = NONE;
    g_54d598.m1228 = NONE;
    g_54d598.m122c = NONE;
    g_54d598.m1230 = NONE;
    memset(g_54d598.m1236, 0, sizeof(g_54d598.m1236));
}


struct s_13c051
{
    short field_0;
    short field_2;
    short field_4;
    short field_6;
};

void interface_rectangle_fill(short_rectangle2d const *bounds, dword color);

// @retail 0x147cbc
void function_147cbc(s_13c051 const *window, dword color)
{
    s_13c051 bounds = *window;
    /* The color argument stays in its stack slot. */
    dword const *color_reference = &color;
    interface_rectangle_fill((short_rectangle2d const *)&bounds, *color_reference);
}

// @retail 0x147cdb
void c_render_window::function_147cdb(dword color)
{
    s_13c051 bounds = *(s_13c051 const *)this;
    short top = -bounds.field_0;
    short left = -bounds.field_2;
    bounds.field_0 += top;
    bounds.field_4 += top;
    bounds.field_2 += left;
    bounds.field_6 += left;
    interface_rectangle_fill((short_rectangle2d const *)&bounds, color);
}

void __stdcall function_215e60(long file_index);
long function_217300(long file_index, long removed_index);
void __stdcall player_slot_set_profile(long index, s_player_profile_settings *settings, long profile_index);

PRIVATE inline long next_controller_148b27(long index)
{
    long result = NONE;
    if (index >= 0 && index < 3)
        result = index + 1;
    return result;
}

// @retail 0x148b27
void __stdcall function_148b27(long index)
{
    function_215e60(index);
    long controller = 0;
    do
    {
        s_player_profile_settings profile;
        long profile_index;
        player_slot_get_profile(controller, (s_player_profile *)&profile, &profile_index);
        if (profile_index != NONE)
        {
            /* Read the two local slots in the caller's order. */
            long current_index = *(volatile long *)&profile_index;
            long removed_index = *(volatile long *)&index;
            long updated_index = function_217300(current_index, removed_index);
            if (updated_index != profile_index)
            {
                player_slot_set_profile(controller, &profile, updated_index);
            }
        }
        controller = next_controller_148b27(controller);
    } while (controller != NONE);
}


void function_2133a1();
void function_199893();
void window_manager_update_online();
void function_18f62b();
void function_199b08();
bool attract_movie_due();
void attract_mode_play_movie();
bool window_manager_main_menu_focused();
struct s_main_menu_music;
void main_menu_music_update(s_main_menu_music *music);

// @retail 0x1479c3
void __stdcall function_1479c3(real elapsed)
{
    g_54d5b8 += (long)(elapsed * 1000.f);
    function_2133a1();
    function_199893();
    function_147f1e();
    window_manager_update_online();
    function_18f62b();
    for (dword i = 0; i < 0x23; i++)
    {
        if (g_54d598.screens[i])
        {
            function_148148(g_54d598.screens[i]);
        }
    }
    g_54d598.default_window.update();
    for (long index = 0; index < 5; index++)
    {
        g_54d598.windows_5[index].update();
        g_54d598.windows_3[index].update();
        g_54d598.windows_1[index].update();
        if (index == 4)
        {
            g_54d598.window_0.update();
            g_54d598.window_4.update();
            g_54d598.window_2.update();
        }
    }
    function_199b08();
    if (g_54d5a8 == 2 && g_4e0350 && !function_155f60() &&
        !g_54d598.windows_5[4].current && !g_54d598.windows_5[4].next)
    {
        function_1483c3(0);
    }
    function_1482c4();
    if (attract_movie_due())
    {
        attract_mode_play_movie();
    }
    main_menu_music_update((s_main_menu_music *)&g_54d598.main_menu_music);
    if (game_loop_running() && g_4e6948->state == 3 && window_manager_main_menu_focused())
    {
        g_54e7fc = g_54d5b8;
    }
}


void function_213484();
void function_235d69(short_rectangle2d const *rectangle, real depth,
    short_rectangle2d const *screen, color4f const *color);

// @retail 0x147af1
void function_147af1(long index, long fallback_index, c_render_window *window)
{
    long const *fallback_reference = &fallback_index;
    c_render_window *const *window_reference = &window;
    function_213484();
    if (index == NONE && *fallback_reference != NONE)
    {
        index = *fallback_reference;
    }
    *(long *)g_54d598.unknown00 = index;
    if (!function_155f60())
    {
        if (index >= 0 && index < 5)
        {
            if ((byte)function_146840() && function_147d13())
            {
                (*window_reference)->function_147cdb(0x7f000000);
            }
            if (index == 4)
                g_54d598.default_window.c_window_channel::render((long)*window_reference);
            g_54d598.windows_5[index].render((long)*window_reference);
            if (index == 4)
                g_54d598.window_4.render((long)*window_reference);
            g_54d598.windows_3[index].render((long)*window_reference);
            if (index == 4)
                g_54d598.window_2.render((long)*window_reference);
            g_54d598.windows_1[index].render((long)*window_reference);
            if (index == 4)
                g_54d598.window_0.c_window_channel::render((long)*window_reference);
        }
        if (index == 4)
        {
            dword elapsed = g_54d5b8 - g_54d598.m28;
            dword color;
            if (elapsed <= 3000)
                color = 0xff000000;
            else if (elapsed <= 4000)
            {
                dword fade_time = elapsed - 3000;
                if (fade_time > 1000)
                    fade_time = 1000;
                real fade = (1.f - (real)fade_time * 0.001f) * 255.f;
                long alpha;
                __asm { fld fade }
                __asm { fistp alpha }
                color = (dword)alpha << 24;
            }
            else
                color = 0;
            if (game_loop_running() && (g_4e6948->state == 1 || g_4e6948->state == 2))
                color = 0;
            if (color)
                (*window_reference)->function_147cdb(color);
            if (g_54d598.unknown05[0])
            {
                color4f outline;
                short_rectangle2d bounds;
                outline.alpha = 1.f;
                outline.red = 0.f;
                outline.green = 1.f;
                outline.blue = 0.f;
                bounds.left = 272;
                bounds.right = -272;
                bounds.top = 204;
                bounds.bottom = -204;
                function_235d69(&bounds, 0.f, (short_rectangle2d *)*window_reference, &outline);
            }
        }
    }
}

struct s_scenario_ui_type
{
    byte unknown00[0x10];
    short type;
};

struct s_ui_initial_selection
{
    byte unknown00[0x11c];
    long selection;
};

struct s_ui_initial_screen
{
    byte unknown00[0x14];
    long tag_index;
};

void function_2157c0(long selection);
void __stdcall game_variant_settings_add_presets(void *screen);
void window_manager_send_pending_join();

// @retail 0x14784e
void function_14784e()
{
    s_scenario_ui_type *scenario = (s_scenario_ui_type *)g_4e0350;
    if (!g_54d598.pending_join_type)
        function_148893(NULL, 1);
    long type = scenario ? *(volatile short *)&scenario->type : NONE;
    g_54d5a8 = (char)type;
    g_54d598.m28 = g_54d5b8;
    g_54d598.default_window.c_window_channel_45997c::v2();
    for (long i = 0; i < 5; i++)
    {
        ((c_window_channel *)&g_54d598.windows_5[i])->v2();
        ((c_window_channel *)&g_54d598.windows_3[i])->v2();
        ((c_window_channel *)&g_54d598.windows_1[i])->v2();
    }
    if (g_54d5a8 == 2)
    {
        s_ui_initial_selection *globals = (s_ui_initial_selection *)ui_definition_get();
        if (globals)
            function_2157c0(globals->selection);
        s_ui_initial_screen *screen = (s_ui_initial_screen *)ui_mode_definition_get();
        if (screen && screen->tag_index != NONE)
            game_variant_settings_add_presets(g_4e3b44[screen->tag_index & 0xffff].bytes);
    }
    g_4ee4e0 = false;
    g_4ee4c4.session_booted = false;
    window_manager_send_pending_join();
    g_54e7fc = 0;
}
