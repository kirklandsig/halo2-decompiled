// @flags /O2 /Ob1 /Gr
/* UNKNOWN_1936E0.CPP: building the built-in game variants: a variant's
   names and descriptions from the user interface's string list, its
   place-to-points table and its settings (lane H) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <wchar.h>
#include <string.h>
#include "globals.h"
#include "language.h"

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

/* the settings of a variant of type 1 or 2: the places that score */
struct s_variant_place_settings
{
	long first;
	long last;
};

/* the settings of a variant of type 3: the teams that score */
struct s_variant_team_settings
{
	long team_size;
	long first_team;
	long last_team;
	bool flag_c;
	byte unknown0d[3];
};

/* the settings of a variant of type 4 */
struct s_variant_type4_settings
{
	long team_size;
	long first_team;
	long last_team;
	long field_c;
	long field_10;
	long field_14;
	bool flag_18;
	byte unknown19[3];
	long field_1c;
	long field_20;
	long field_24;
};

/* the settings of a variant of type 5 */
struct s_variant_type5_settings
{
	long team_size;
	long first_team;
	long last_team;
	long field_c;
};

/* the variant word at +8, which the builders pass by value */
struct s_variant_field_8
{
	dword value;
};

/* a game variant (0x614 bytes); the checks are in unknown_193560.cpp */
struct s_built_variant
{
	long type;
	long field_4;
	s_variant_field_8 field_8;
	char names[9][0x10];
	char descriptions[9][0x80];
	char field_51c[128];
	long field_59c;
	long field_5a0;
	long points[16];
	byte unknown5e4[8];
	union
	{
		s_variant_place_settings place;
		s_variant_team_settings team;
		s_variant_type4_settings type4;
		s_variant_type5_settings type5;
	} settings;
};

/* the shared user interface globals: the string list of the variants'
   names at +0x1c */
struct s_variant_strings_view
{
	byte unknown00[0x1c];
	long string_list_index;
};

/* a language's string table and a string list's range of its strings
   (unknown_1a0120.cpp) */
struct s_string_reference;

struct s_string_table
{
	s_string_reference *references;
	char *data;
	long count;
	long data_size;
	long references_offset;
	long data_offset;
	bool loaded;
	byte unknown19[3];
};

struct s_variant_string_tables_view
{
	byte unknown000[0x188];
	s_string_table tables[1];
};

struct s_variant_language_range
{
	short first;
	short count;
};

struct s_variant_string_list_view
{
	byte unknown00[0x10];
	s_variant_language_range languages[1];
};

struct s_user_interface_shared_globals *ui_mode_definition_get(void);
long function_11ca80(long value);
const char *string_table_find(s_string_table *table, long first, long count, long string_handle);
void utf8_string_to_utf16_string(const char *source, word *destination, long destination_count);
void utf16_string_to_utf8_string(const word *source, char *destination, long destination_size);

/* a string of a table, as 0x1a0350, which retail inlines here */
__forceinline void variant_string_table_get(s_string_table *table, long first, long count, long string_handle, word *buffer)
{
	if (table->loaded)
	{
		word string[0x100];

		utf8_string_to_utf16_string(string_table_find(table, first, count, string_handle), string, 0x100);
		wcsncpy((wchar_t *)buffer, (wchar_t *)string, 0xff);
		buffer[0xff] = 0;
	}
}

/* a string of a string list in the current language: 0x1a0180 without
   its test for NONE, which the caller makes */
__forceinline void variant_string_get(long tag_index, long string_handle, word *buffer)
{
	get_current_language();
	s_variant_string_list_view const *list = (s_variant_string_list_view *)g_4e3b44[tag_index & 0xffff].bytes;
	s_variant_string_tables_view *tables = (s_variant_string_tables_view *)g_4e034c;

	variant_string_table_get(&tables->tables[g_47ff38], list->languages[g_47ff38].first,
		list->languages[g_47ff38].count, string_handle, buffer);
}

/* fills a variant: its type and settings, its nine names and descriptions
   and its tables */
// @retail 0x1936e0
void function_1936e0(s_built_variant *variant, long type, s_variant_field_8 field_8, long description_string, long field_5a0, long const *points, long field_59c, long name_string)
{
	word text[0x100];
	s_variant_strings_view *strings;
	long i;

	text[0] = 0;
	variant->type = type;
	variant->field_4 = 0;
	variant->field_8 = field_8;
	strings = (s_variant_strings_view *)ui_mode_definition_get();
	if (strings && strings->string_list_index != NONE)
	{
		long string_list_index = strings->string_list_index;

		for (long name_index = 0; name_index < 9; ++name_index)
		{
			variant_string_get(string_list_index, name_string, text);
			utf16_string_to_utf8_string(text, variant->names[name_index], sizeof(variant->names[0]));
		}
		for (long description_index = 0; description_index < 9; ++description_index)
		{
			variant_string_get(string_list_index, description_string, text);
			utf16_string_to_utf8_string(text, variant->descriptions[description_index], sizeof(variant->descriptions[0]));
		}
	}
	for (i = 0; i <= 5; i++)
		variant->field_51c[i] = PIN(i + 5, i, 127);
	for (i = 6; i <= 15; i++)
		variant->field_51c[i] = PIN(i + 6, i, 127);
	for (i = 16; i <= 25; i++)
		variant->field_51c[i] = PIN(i + 7, i, 127);
	for (i = 26; i <= 30; i++)
		variant->field_51c[i] = PIN(i + 8, i, 127);
	for (i = 31; i <= 35; i++)
		variant->field_51c[i] = PIN(i + 9, i, 127);
	for (i = 36; i <= 127; i++)
		variant->field_51c[i] = PIN(i + 10, i, 127);
	variant->field_59c = field_59c;
	variant->field_5a0 = field_5a0;
	long const *point_input = points;
	for (i = 0; i < 16; i++)
	{
		variant->points[i] = *point_input;
		++point_input;
	}
}

void matchmaking_hopper_build_player_start_delays(long scale, long first, long last, long *points);
void matchmaking_hopper_build_team_start_delays(long *points, long team_size, long last_team, long first_team, long scale);

/* fills a variant of type 4 */
// @retail 0x193ac0
void function_193ac0(s_built_variant *variant, long name_string, long description_string, s_variant_field_8 field_8, long scale, s_variant_type4_settings const *settings, long field_59c)
{
	long points[16];

	matchmaking_hopper_build_team_start_delays(points, settings->team_size, settings->last_team, settings->first_team, scale);
	function_1936e0(variant, 4, field_8, description_string, scale, points, field_59c, name_string);
	variant->settings.type4 = *settings;
}

/* fills a variant of type 3 */
// @retail 0x193b30
void function_193b30(s_built_variant *variant, long name_string, long description_string, s_variant_field_8 field_8, long scale, s_variant_team_settings const *settings, long field_59c)
{
	long points[16];

	matchmaking_hopper_build_team_start_delays(points, settings->team_size, settings->last_team, settings->first_team, scale);
	function_1936e0(variant, 3, field_8, description_string, scale, points, field_59c, name_string);
	variant->settings.team = *settings;
}

/* fills a variant of type 1, or of type 2 when flag is set */
// @retail 0x193ba0
void function_193ba0(bool flag, s_built_variant *variant, long name_string, long description_string, s_variant_field_8 field_8, s_variant_place_settings const *settings, long field_59c, long scale)
{
	long points[16];

	matchmaking_hopper_build_player_start_delays(scale, settings->first, settings->last, points);
	function_1936e0(variant, flag ? 2 : 1, field_8, description_string, scale, points, field_59c, name_string);
	variant->settings.place = *settings;
}

/* fills a variant of type 5 */
// @retail 0x193c00
void function_193c00(s_built_variant *variant, long name_string, long description_string, s_variant_field_8 field_8, s_variant_type5_settings const *settings, long field_59c)
{
	long points[16];

	matchmaking_hopper_build_team_start_delays(points, settings->team_size, settings->last_team, settings->first_team, 0);
	function_1936e0(variant, 5, field_8, description_string, 0, points, field_59c, name_string);
	variant->settings.type5 = *settings;
}

/* fills one of the seven built-in variants */
// @retail 0x193c70
void function_193c70(s_built_variant *variant, long index)
{
	s_variant_type5_settings type5_settings;
	s_variant_team_settings team_settings;
	memset(variant, 0, sizeof(*variant));
	switch (index)
	{
	case 0:
	{
		s_variant_field_8 field_8 = {1};
		s_variant_place_settings settings;

		settings.first = 4;
		settings.last = 8;
		function_193ba0(true, variant, 0x90007a4, 0xf0007a5, field_8, &settings, 0x249f0, 0x7530);
		break;
	}
	case 1:
	{
		s_variant_field_8 field_8 = {2};
		s_variant_type4_settings settings;

		memset(&settings, 0, sizeof(settings));
		settings.team_size = 2;
		settings.first_team = 3;
		settings.last_team = 4;
		settings.field_c = 0;
		settings.field_10 = 1;
		settings.field_14 = 4;
		settings.flag_18 = true;
		settings.field_1c = 3;
		settings.field_20 = 4;
		settings.field_24 = 0;
		function_193ac0(variant, 0x100007a6, 0x160007a7, field_8, 0x7530, &settings, 0x249f0);
		break;
	}
	case 2:
	{
		s_variant_field_8 field_8 = {3};
		s_variant_place_settings settings;

		settings.first = 2;
		settings.last = 2;
		function_193ba0(true, variant, 0x120007a8, 0x180007a9, field_8, &settings, 0x1d4c0, 0x7530);
		break;
	}
	case 3:
	{
		s_variant_field_8 field_8 = {4};
		s_variant_type4_settings settings;

		memset(&settings, 0, sizeof(settings));
		settings.team_size = 2;
		settings.first_team = 5;
		settings.last_team = 8;
		settings.field_c = 1;
		settings.field_10 = 1;
		settings.field_14 = 5;
		settings.flag_18 = true;
		settings.field_1c = 3;
		settings.field_20 = 5;
		settings.field_24 = 2;
		function_193ac0(variant, 0xe0007aa, 0x140007ab, field_8, 0xafc8, &settings, 0x493e0);
		break;
	}
	case 4:
	{
		s_variant_field_8 field_8 = {0x19};
		memset(&type5_settings, 0, sizeof(type5_settings));
		type5_settings.team_size = 2;
		type5_settings.first_team = 3;
		type5_settings.last_team = 4;
		type5_settings.field_c = 0;
		function_193c00(variant, 0x1a0007ac, 0x200007ad, field_8, &type5_settings, 0x1d4c0);
		break;
	}
	case 5:
	{
		s_variant_field_8 field_8 = {0x1a};
		memset(&type5_settings, 0, sizeof(type5_settings));
		type5_settings.team_size = 2;
		type5_settings.first_team = 6;
		type5_settings.last_team = 8;
		type5_settings.field_c = 2;
		function_193c00(variant, 0x180007ae, 0x1e0007af, field_8, &type5_settings, 0x1d4c0);
		break;
	}
	case 6:
	{
		s_variant_field_8 field_8 = {5};
		memset(&team_settings, 0, sizeof(team_settings));
		team_settings.team_size = 2;
		team_settings.first_team = 2;
		team_settings.last_team = 4;
		team_settings.flag_c = true;
		function_193b30(variant, 0x190007b0, 0x1f0007b1, field_8, 0x7530, &team_settings, 0x249f0);
		break;
	}
	}
}
