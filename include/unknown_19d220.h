#pragma once
/* the game variant as the variant menus and the session pass it, its
   defaults and its checks (unknown_19d220.cpp, lane H) */

#include "unknown_11c920.h"
#include <stddef.h>
#include <string.h>
#include <wchar.h>
#include "globals.h"

/* the settings of capture the flag (1) and assault (9) */
struct s_game_variant_flag_settings
{
	dword flags;          // 8 bits
	long unknownf4;       // 0..0x7fff
	long unknownf8;       // 0..2
	long unknownfc;       // 0..1
	long unknown100;      // 0..3
	long unknown104;      // 0..2
	short unknown108;     // 0..15 (assault)
	short unknown10a;     // 0..15 (assault)
};

/* the settings of oddball (3) */
struct s_game_variant_ball_settings
{
	dword flags;          // 3 bits
	short unknownf4;      // 0..3
	short unknownf6;      // 0..1
	short unknownf8;      // 0..2
	short unknownfa;      // 0..3
};

/* the settings of king of the hill (4) and juggernaut (7) */
struct s_game_variant_king_settings
{
	dword flags;          // 5 bits (king), 7 bits (juggernaut)
	short unknownf4;
};

/* the settings of territories (8) */
struct s_game_variant_territories_settings
{
	short unknownf0;      // 1..8
	short unknownf2;      // 1..0x7fff
	short unknownf4;      // 1..0x7fff
};

/* a game variant (0x130 bytes): its name, its game engine and flags, as the
   variant menus, the session and the user interface pass it */
struct s_game_variant
{
	word flags;
	byte unknown02;
	char unknown03;
	wchar_t name[0x20];
	long field_xcb8724; // 1..9
	union
	{
		dword flags48;    // 15 bits
		struct
		{
			dword teams_enabled : 1;
			dword motion_sensor_enabled : 1;
			dword flags_bits2 : 30;
		};
	};
	long unknown4c;
	long unknown50;
	long unknown54;
	long unknown58;
	byte unknown5c[0x74 - 0x5c];
	long unknown74;
	long unknown78;
	long unknown7c;
	long unknown80;
	long unknown84;
	long unknown88;
	byte unknown8c[0xa4 - 0x8c];
	long unknowna4;
	long unknowna8;
	long unknownac;
	long unknownb0;
	long unknownb4;
	byte unknownb8[0xcc - 0xb8];
	char unknowncc;
	char unknowncd;
	char unknownce;
	char unknowncf;
	char unknownd0;
	char unknownd1;
	char unknownd2;
	char unknownd3;
	char unknownd4;
	char unknownd5;
	char unknownd6;
	char unknownd7;
	byte unknownd8[0xf0 - 0xd8];
	union
	{
		dword engine_flags;
		s_game_variant_flag_settings flag;
		s_game_variant_ball_settings ball;
		s_game_variant_king_settings king;
		s_game_variant_territories_settings territories;
		byte engine_settings[0x130 - 0xf0];
	};
};

s_game_variant *__stdcall function_19d220(s_game_variant *variant, long type);
bool function_19d620(s_game_variant *variant);

void function_1a0180(long tag_index, long string_handle, word *buffer);

/* the multiplayer globals tag the globals tag refers to: its first block
   element holds the string list of the default variant names */
struct s_multiplayer_globals_element
{
	byte unknown00[0x1c];
	long string_list_tag_index;
};

struct s_multiplayer_globals
{
	long count;
	s_multiplayer_globals_element *elements;
};

/* fills a variant with the defaults of a variant type (0 slayer, 1 oddball,
   2 juggernaut, 3 king of the hill, 4 capture the flag, 5 assault,
   6 territories): the body of function_19d220, which function_212c20
   inlines for slayer */
__forceinline s_game_variant *variant_defaults_initialize(s_game_variant *variant, long type)
{
	s_game_variant result;
	long engine = 2;
	long string_handle = 0;
	word name[0x100];

	memset(&result, 0, sizeof(result));
	switch (type)
	{
	case 0:
		engine = 2;
		string_handle = 0xe000780;
		break;
	case 2:
		engine = 7;
		string_handle = 0x12000783;
		break;
	case 3:
		engine = 4;
		string_handle = 0xc000782;
		break;
	case 1:
		engine = 3;
		string_handle = 0xf000781;
		break;
	case 4:
		engine = 1;
		string_handle = 0xb00077f;
		break;
	case 5:
		engine = 9;
		string_handle = 0xf000785;
		break;
	case 6:
		engine = 8;
		string_handle = 0x13000784;
		break;
	}
	name[0] = 0;
	if (g_4e034c && g_4e034c->index != NONE)
	{
		s_multiplayer_globals_element *elements = ((s_multiplayer_globals *)g_4e3b44[g_4e034c->index & 0xffff].bytes)->elements;

		if (elements->string_list_tag_index != NONE)
		{
			function_1a0180(elements->string_list_tag_index, string_handle, name);
		}
	}
	wcsncpy(result.name, name, 0x1f);
	result.name[0x1f] = 0;
	result.field_xcb8724 = engine;
	result.unknown03 = -1;
	/* capture the flag, assault and territories default to teams (flags48 bit 0) */
	bool teams_by_default = type >= 4 && type <= 6;

	if (teams_by_default)
	{
		result.flags48 |= 1;
	}
	else
	{
		result.flags48 &= ~1;
	}
	result.flags48 = (result.flags48 & 0xffff8f9b) | 0xf9a;
	result.unknown4c = 0;
	switch (type)
	{
	case 0:
		result.unknown50 = 0x19;
		break;
	case 2:
		result.unknown50 = 0xf;
		break;
	case 3:
		result.unknown50 = 0x78;
		break;
	case 1:
		result.unknown50 = 0x78;
		break;
	case 6:
		result.unknown50 = 0x12c;
		break;
	case 4:
		result.unknown50 = 3;
		break;
	case 5:
		result.unknown50 = 3;
		break;
	}
	result.unknown54 = 0x1e0;
	result.unknown58 = 0;
	result.unknown74 = 0x10;
	result.unknown78 = 0x10;
	result.unknown7c = 0;
	result.unknown80 = type >= 4 && type <= 6 ? 10 : 5;
	result.unknown84 = type >= 4 && type <= 6 ? 10 : 5;
	result.unknown88 = 0;
	result.unknowna4 = 0;
	result.unknowna8 = 2;
	result.unknownac = 10;
	result.unknownb0 = 0;
	result.unknowncc = 0;
	result.unknowncd = 0;
	result.unknownce = 0;
	result.unknowncf = 0;
	result.unknownd0 = 0;
	result.unknownd1 = 0;
	result.unknownd2 = 0;
	result.unknownd3 = 0;
	result.unknownd4 = 0;
	result.unknownd5 = 0;
	result.unknownd6 = 0;
	result.unknownd7 = 0;
	switch (type)
	{
	case 0:
		result.engine_flags = (result.engine_flags & ~5) | 2;
		break;
	case 2:
		result.king.unknownf4 = 2;
		result.king.flags = (result.king.flags & ~0x24) | 0x1b;
		break;
	case 3:
		result.king.flags &= ~0x1f;
		result.king.unknownf4 = 0x3c;
		break;
	case 1:
		result.ball.flags &= ~7;
		result.ball.unknownf4 = 1;
		result.ball.unknownf6 = 0;
		result.ball.unknownf8 = 0;
		result.ball.unknownfa = 0;
		break;
	case 5:
		result.flag.flags = (result.flag.flags & ~0xdc) | 0x22;
		result.flag.unknownfc = 0;
		result.flag.unknown108 = 5;
		result.flag.unknown10a = 3;
		result.flag.unknownf4 = 0x1e;
		result.flag.unknownf8 = 0;
		result.flag.unknown104 = 0;
		result.flag.unknown100 = 1;
		break;
	case 4:
		result.flag.flags = (result.flag.flags & ~0xfc) | 2;
		result.flag.unknownfc = 0;
		result.flag.unknown108 = 0;
		result.flag.unknown10a = 0;
		result.flag.unknownf4 = 0x1e;
		result.flag.unknownf8 = 0;
		result.flag.unknown104 = 0;
		result.flag.unknown100 = 0;
		break;
	case 6:
		result.territories.unknownf4 = 5;
		result.territories.unknownf0 = 3;
		result.territories.unknownf2 = 5;
		break;
	}
	*variant = result;
	return variant;
}
