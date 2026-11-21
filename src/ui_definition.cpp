// @flags /O1 /Gr
/* UI_DEFINITION.CPP: the user interface's tags: the current game
   mode's shared globals, the user interface globals they name, its skins,
   and the default game variant of a game engine type
   (ui_definition.h) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_234c64.h"
#include "unknown_19b516.h"
#include "ui_definition.h"

/* the shared globals tag of the scenario's game mode, if the global tag data
   has interface tags */
// @retail 0x1482e8
s_user_interface_shared_globals *ui_mode_definition_get(void)
{
	s_user_interface_shared_globals *result = 0;

	if (g_4e0350 && g_4e034c)
	{
		s_scenario_shell_mode_view *scenario = (s_scenario_shell_mode_view *)g_4e0350;
		s_global_interface_tags_view *global_tags = (s_global_interface_tags_view *)g_4e034c;
		s_ui_tag_references *references = global_tags->interface_tag_count ? global_tags->interface_tags : 0;

		if (references)
		{
			long tag_index;
			switch (scenario->shell_mode)
			{
			case _shell_mode_campaign:
				tag_index = references->campaign_shared_globals.tag_index;
				break;
			case _shell_mode_multiplayer:
				tag_index = references->multiplayer_shared_globals.tag_index;
				break;
			case _shell_mode_main_menu:
				tag_index = references->main_menu_shared_globals.tag_index;
				break;
			default:
				tag_index = NONE;
				break;
			}

			if (tag_index != NONE)
			{
				result = (s_user_interface_shared_globals *)g_4e3b44[tag_index & 0xffff].bytes;
			}
		}
	}
	return result;
}

/* the user interface globals tag the shared globals name */
// @retail 0x148350
s_type_954545 *ui_definition_get(void)
{
	s_type_954545 *ui_definition = 0;
	s_user_interface_shared_globals *shared_globals = ui_mode_definition_get();

	if (shared_globals && shared_globals->ui_definition.tag_index != NONE)
	{
		ui_definition = (s_type_954545 *)g_4e3b44[shared_globals->ui_definition.tag_index & 0xffff].bytes;
	}
	return ui_definition;
}

/* a list skin of the user interface globals */
// @retail 0x14837a
s_sprite_placement *ui_definition_get_skin(short index)
{
	s_sprite_placement *result = 0;
	s_type_954545 *ui_definition = ui_definition_get();

	if (ui_definition && index >= 0 && index < ui_definition->skin_count)
	{
		s_tag_reference_8 *skin = &ui_definition->skins[index];
		if (skin->tag_index != NONE)
		{
			result = (s_sprite_placement *)g_4e3b44[skin->tag_index & 0xffff].bytes;
		}
	}
	return result;
}

long function_216b00(long type);
bool function_19d650(s_game_variant *variant);
void function_2373be(long index, void *base, long value);

/* builds a game engine type's default variant: the engine's own defaults,
   then the settings the game variant settings tag lists for it, then
   clamped into range; both retail callers (0x149f88, 0x2ca284) compute the engine
   type before they push the variant, so the variant is the first parameter */
// @retail 0x1489b5
void ui_game_variant_build_default(s_game_variant *variant, long engine_type)
{
	function_19d220(variant, function_216b00(engine_type));
	long settings_tag_index = ui_mode_definition_get()->game_variant_settings.tag_index;
	if (settings_tag_index != NONE)
	{
		s_game_variant_settings *settings = (s_game_variant_settings *)g_4e3b44[settings_tag_index & 0xffff].bytes;
		s_game_engine_default_settings *defaults = &settings->engines[engine_type - 1];
		for (long i = 0; i < defaults->setting_count; i++)
			function_2373be(defaults->settings[i].setting_index, variant, defaults->settings[i].value);
	}
	function_19d650(variant);
}
