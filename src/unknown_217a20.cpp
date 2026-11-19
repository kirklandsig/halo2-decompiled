#include "unknown_11c920.h"
#include <string.h>

// @flags /O2 /Gr

// Defined inline though every caller calls it out of line: an inline callee carries no register-preservation
// summary, so 0x251f8d keeps the message in EDI across the call as retail does (0x251f8d 20 -> MATCH; the
// lever is andrewnakas's, #1417).
// @retail 0x217b70
inline dword function_217b70(word const *text)
{
	dword length;
	for (length = 0; length < 255; length++)
	{
		if (!text[length])
			break;
	}
	return length;
}

wchar_t const *saved_game_storage_get_default_profile_name(long language);
wchar_t const *saved_game_type_get_profile_name(long language);
wchar_t const *saved_game_type_get_slayer_name(long language);
wchar_t const *saved_game_type_get_king_of_the_hill_name(long language);
wchar_t const *saved_game_type_get_race_name(long language);
wchar_t const *saved_game_type_get_oddball_name(long language);
wchar_t const *saved_game_type_get_juggernaut_name(long language);
wchar_t const *saved_game_type_get_headhunter_name(long language);
wchar_t const *saved_game_type_get_capture_the_flag_name(long language);
wchar_t const *saved_game_type_get_assault_name(long language);
wchar_t const *saved_game_type_get_territories_name(long language);
wchar_t const *saved_game_type_get_playlist_name(long language);

// @retail 0x217a80
void function_217a80(long string_handle_2, long language, word *buffer)
{
	const word *text = L"";
	switch (string_handle_2)
	{
	case 0x1b0006e8: text = saved_game_storage_get_default_profile_name(language); break;
	case 0x1b0006e9: text = saved_game_type_get_profile_name(language); break;
	case 0x130006ea: text = saved_game_type_get_slayer_name(language); break;
	case 0x110006eb: text = saved_game_type_get_king_of_the_hill_name(language); break;
	case 0x110006ec: text = saved_game_type_get_race_name(language); break;
	case 0x140006ed: text = saved_game_type_get_oddball_name(language); break;
	case 0x170006ee: text = saved_game_type_get_juggernaut_name(language); break;
	case 0x170006ef: text = saved_game_type_get_headhunter_name(language); break;
	case 0x100006f0: text = saved_game_type_get_capture_the_flag_name(language); break;
	case 0x140006f1: text = saved_game_type_get_assault_name(language); break;
	case 0x180006f2: text = saved_game_type_get_territories_name(language); break;
	case 0x150006f3: text = saved_game_type_get_playlist_name(language); break;
	}
	wcsncpy(buffer, text, 0xff);
	buffer[0xff] = 0;
}
