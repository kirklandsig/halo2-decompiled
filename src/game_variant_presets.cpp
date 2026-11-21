// @flags /O1 /Oi /Ob1 /Gr
/* GAME_VARIANT_PRESETS.CPP: the preset game variants of the game variant
   settings tag, added to the saved game files when the main menu starts */

#include "unknown_11c920.h"
#include <string.h>
#include "unknown_19b516.h"
#include "unknown_19d220.h"
#include <wchar.h>
#include "game_variant_presets.h"

long function_216ab0(long index);
void ui_game_variant_build_default(s_game_variant *variant, long type);
void function_1a0180(long tag_index, long string_handle, word *buffer);
void function_2373be(long index, void *base, long value);
bool function_19d650(s_game_variant *variant);
void __stdcall function_217080(s_game_variant const *variant);

/* builds each preset: its engine's default variant, named from the preset
   string list, with the preset's settings stored over it; each preset that
   needs no clamping (0x19d650) is added to the saved game files (0x217080) */
// @retail 0x149f88
void __stdcall game_variant_settings_add_presets(void *settings_tag)
{
    s_game_variant_settings *settings = (s_game_variant_settings *)settings_tag;
    for (long index = 0; index < settings->preset_count; ++index)
    {
        s_game_variant_preset *preset = &settings->presets[index];
        if (preset->engine_index >= 0 && preset->engine_index < 7)
        {
            s_game_variant variant;
            ui_game_variant_build_default(&variant, function_216ab0(preset->engine_index));
            word name[0x100];
            name[0] = 0;
            function_1a0180(settings->preset_string_list_index, preset->name_string, name);
            wcsncpy(variant.name, (wchar_t const *)name, 0x1f);
            variant.name[0x1f] = 0;
            long percentage = preset->percentage < 0 ? 0 : (preset->percentage > 100 ? 100 : preset->percentage);
            if (percentage == preset->percentage)
                variant.unknown03 = preset->percentage;
            for (long setting = 0; setting < preset->setting_count; ++setting)
                function_2373be(preset->settings[setting].setting_index, &variant, preset->settings[setting].value);
            if (function_19d650(&variant))
                function_217080(&variant);
        }
    }
}
