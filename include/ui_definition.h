/* UI_DEFINITION.H: the user interface's tags, as the lookups in
   ui_definition.cpp reach them.

   The global tag data (g_4e034c) holds a block of interface tag references.
   Its first element names one "shared globals" tag per game mode; the game
   mode is the scenario's (g_4e0350, +0x10) and the window manager's
   (g_54d598.shell_mode). The shared globals name the user interface globals tag
   (s_type_954545, unknown_234c64.h), the screen definitions and the game
   variant settings tag. */
#ifndef UI_DEFINITION_H
#define UI_DEFINITION_H

#include "unknown_234c64.h"
#include "unknown_19d220.h"
#include "game_variant_settings.h"
#include <stddef.h>

/* the game modes: the shared globals each picks, and the window manager's
   handler of its events (0x14954a) */
enum e_shell_mode
{
	_shell_mode_campaign = 0,
	_shell_mode_multiplayer,
	_shell_mode_main_menu
};

/* the interface tag references (the first element of the global tag data's
   block at +0x110); only the three shared globals references are read */
struct s_ui_tag_references
{
	byte unknown00[0x80];
	s_tag_reference_8 main_menu_shared_globals;
	s_tag_reference_8 campaign_shared_globals;
	s_tag_reference_8 multiplayer_shared_globals;
};
typedef char assert_interface_tag_references_size[(sizeof(s_ui_tag_references) == 0x98) ? 1 : -1];
typedef char assert_interface_tag_references_main_menu[(offsetof(s_ui_tag_references, main_menu_shared_globals.tag_index) == 0x84) ? 1 : -1];
typedef char assert_interface_tag_references_campaign[(offsetof(s_ui_tag_references, campaign_shared_globals.tag_index) == 0x8c) ? 1 : -1];
typedef char assert_interface_tag_references_multiplayer[(offsetof(s_ui_tag_references, multiplayer_shared_globals.tag_index) == 0x94) ? 1 : -1];

/* the part of the global tag data (g_4e034c) read here: the block of
   interface tag references (an empty block counts as none) */
struct s_global_interface_tags_view
{
	byte unknown000[0x110];
	long interface_tag_count;
	s_ui_tag_references *interface_tags;
};
typedef char assert_global_interface_tags_view_size[(sizeof(s_global_interface_tags_view) == 0x118) ? 1 : -1];

/* the part of the scenario (g_4e0350) read here: its e_shell_mode */
struct s_scenario_shell_mode_view
{
	byte unknown00[0x10];
	short shell_mode;
};
typedef char assert_scenario_shell_mode_view_offset[(offsetof(s_scenario_shell_mode_view, shell_mode) == 0x10) ? 1 : -1];

/* one game mode's shared globals tag (the part read here) */
struct s_user_interface_shared_globals
{
	/* the user interface globals tag (s_type_954545) */
	s_tag_reference_8 ui_definition;
	/* the screen definitions the window manager looks up by id (0x148098) */
	long screen_count;
	s_tag_reference_8 *screens;
	/* the game variant settings tag (s_game_variant_settings,
	   game_variant_settings.h) */
	s_tag_reference_8 game_variant_settings;
	/* the string list of the game variants' names (0x1936e0) */
	s_tag_reference_8 game_variant_strings;
};
typedef char assert_ui_mode_definition_size[(sizeof(s_user_interface_shared_globals) == 0x20) ? 1 : -1];
typedef char assert_ui_mode_definition_settings[(offsetof(s_user_interface_shared_globals, game_variant_settings.tag_index) == 0x14) ? 1 : -1];

/* ui_definition.cpp (ui_definition_get is declared in
   unknown_234c64.h, ui_definition_get_skin in unknown_19b516.h) */
s_user_interface_shared_globals *ui_mode_definition_get(void);
void ui_game_variant_build_default(s_game_variant *variant, long engine_type);

/* ui_definition_lookups.cpp */
void ui_definition_get_subtitle_text(long subtitle_type, word *buffer);
long ui_definition_find_range(long value);

#endif
