// stubs for the game functions outside 0x230000..0x23ffff that lane G's code
// calls and that are not decompiled yet (and a few of lane G's own, until
// they are written)
#include "unknown_11c920.h"
#include "screen_widgets.h"
#include "unknown_234c64.h"
#include "user_interface_lists.h"
#include "unknown_2312b4.h"

/* UI lane round 4: callees of the campaign level select list and the game
   engine variant category list */

// @stub 0x215f40
bool __stdcall function_215f40(long arg_9db745, byte *buffer)
{
	return false;
}

// @stub 0x212380
long function_212380(long arg_9db745, long controller_index, byte *buffer)
{
	return 0;
}

// @stub 0x212bc0
bool function_212bc0(long file_index, s_game_variant *variant)
{
	return false;
}

// @stub 0x8c150
bool __stdcall function_8c150(void *cache, long a, unsigned __int64 *xuid, void *data, unsigned __int64 *clan_id)
{
	return false;
}

// @stub 0x252ed8
void __stdcall function_252ed8(void *list)
{
}

// @stub 0x2b2181
void __stdcall function_2b2181(void *list, long controller_index)
{
}



// @stub 0x124770
bool function_124770(long profile_index)
{
	return false;
}

// @stub 0x147cdb
void c_render_window::function_147cdb(dword color)
{
}

/* lane G's own, not written yet */

// @stub 0x235756
void function_235756(real fade)
{
}

// @stub 0x2359ce
void function_2359ce(c_window_channel_459a34 *channel)
{
}

/* the screens' create functions (lane G, not written yet) */


/* callees of the screen widget code */

// @stub 0x22fba9
void function_22fba9(c_class_1473c9 *screen)
{
}

// @stub 0x219070
byte __stdcall function_219070(long set_index)
{
	return 0;
}

// @stub 0x215367
void __stdcall function_215367(long player, long profile_index, void *data, long flags)
{
}

struct s_bitmap_view;

// @stub 0x12360
void function_12360(s_bitmap_view *bitmap, real priority)
{
}

/* lane K's 0x22387b */
// @stub 0x22387b
void function_22387b(void)
{
}

/* UI lane round 2: the custom game profile list (unknown_2c9ddb.cpp) */

// @stub 0x2ca284
void c_class_2c9e69::handle_item(s_controller_reference **controller, long *item)
{
}

// @stub 0x2ca0d9
void c_class_2c9e69::fill()
{
}

// @stub 0x120e70
long __stdcall function_120e70(byte *buffer)
{
	return 0;
}

// @stub 0x215b50
word *function_215b50(long variant, word *buffer)
{
	return 0;
}

// @stub 0x2305d0
void c_legalese_acceptance_list::handle_item(s_controller_reference **controller, long *item)
{
}

/* unknown_2b116a.cpp's list (only the member the stub defines) */
class c_potential_squad_leader_player_list
{
public:
	void handle_item(s_controller_reference **controller, long *item);
};

// @stub 0x2b8497
void c_potential_squad_leader_player_list::handle_item(s_controller_reference **controller, long *item)
{
}

/* UI lane round 3: callees of user_interface_text_parser.cpp */

// @stub 0x122dd0
real __stdcall function_122dd0(byte *map_name, long unknown)
{
	return 0.f;
}

// @stub 0x15ea80
void function_15ea80(long string_handle, long maximum_count, word *buffer)
{
}

/* UI lane round 3: callees of the actions list (unknown_2b116a.cpp) */


// @stub 0x148523
void function_148523()
{
}

/* UI lane round 5: callees of the press start screen */



/* lane M */
struct s_player_profile_settings;

/* the open region 0x180000..0x18ffff (lane F, paused) */
// @stub 0x18fb34
void __stdcall function_18fb34(long player, s_player_profile_settings *settings, long profile_index)
{
}

/* lane D */
struct _XONLINE_USER;
// @stub 0x6c8b0
long function_6c8b0(_XONLINE_USER *user, long player)
{
	return 0;
}

/* my own, not written yet */
// @stub 0x24b869
void __stdcall function_24b869(c_class_1473c9 *screen)
{
}


/* my own, the main menu's dialog callbacks, not written yet */
// @stub 0x236917
bool __stdcall function_236917(long controller_index)
{
	return false;
}

/* lane D */
// @stub 0x6cc10
long __stdcall function_6cc10(long controller_index)
{
	return 0;
}


/* UI lane round 7: my own, not written yet */
// @stub 0x238f3f
void __stdcall function_238f3f(long controller_index, void *message, unsigned __int64 value)
{
}

// @stub 0x23902b
void __stdcall function_23902b(void *message, long controller_index, unsigned __int64 value)
{
}

struct _XONLINE_FRIEND;
// @stub 0x2395dc
void __stdcall function_2395dc(_XONLINE_FRIEND *friend_, long controller_index, long mode)
{
}


struct s_widget_item;
class c_class_1a2c81;

// @stub 0x2afeae
void function_2afeae(s_widget_item *item, c_class_1a2c81 *widget)
{
}

/* UI lane round 14: callees of the campaign options list */

struct s_saved_game_header;
struct s_saved_game_read;
class c_campaign_options_list;

// @stub 0x124360
bool function_124360(s_saved_game_header *header, s_saved_game_read *read)
{
	return false;
}

// @stub 0x2acab4
void __stdcall function_2acab4(long a, long user_flags, long string_handle, bool (__stdcall *progress)(c_campaign_options_list *list, long unused, real *fraction, long *error), long b, c_campaign_options_list *list)
{
}

// @stub 0x215900
void __stdcall function_215900(long controller_index, long type, word *count, long *files, long a)
{
}
