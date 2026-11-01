// @flags /O2 /Gr
/* UNKNOWN_19D220.CPP: game variant defaults and checks (lane H) */

#include "unknown_11c920.h"
#include <string.h>
#include <wchar.h>
#include "globals.h"
#include "unknown_19d220.h"

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

__declspec(noinline) s_game_variant *__stdcall function_19d220(s_game_variant *variant, long type);

// @retail 0x19d220
s_game_variant *__stdcall function_19d220(s_game_variant *variant, long type)
{
	return variant_defaults_initialize(variant, type);
}

/* clamps a setting into its range in place */
static inline void pin_value(long *value, long lower, long upper)
{
	*value = PIN(*value, lower, upper);
}

static inline void pin_value(short *value, long lower, long upper)
{
	*value = PIN(*value, lower, upper);
}

static inline void pin_value(char *value, long lower, long upper)
{
	*value = PIN(*value, lower, upper);
}

/* the settings of capture the flag and assault */
static __forceinline void flag_settings_verify(s_game_variant_flag_settings *settings)
{
	settings->flags &= 0xff;
	pin_value(&settings->unknownf4, 0, 0x7fff);
	pin_value(&settings->unknownf8, 0, 2);
	pin_value(&settings->unknownfc, 0, 1);
	pin_value(&settings->unknown100, 0, 3);
	pin_value(&settings->unknown104, 0, 2);
}

/* the settings of king of the hill and juggernaut */
static __forceinline void king_settings_verify(s_game_variant_king_settings *settings, dword flags_mask, long maximum)
{
	settings->flags &= flags_mask;
	pin_value(&settings->unknownf4, 0, maximum);
}

/* clamps every setting of a variant into its range; a variant that needed
   any change is replaced by the slayer defaults */
// @retail 0x19d650
bool function_19d650(s_game_variant *variant)
{
	bool valid = true;
	s_game_variant original = *variant;

	variant->flags &= 1;
	variant->name[0x1f] = 0;
	variant->flags48 &= 0x7fff;
	variant->unknown4c = PIN(variant->unknown4c, 0, 6);
	pin_value(&variant->unknown50, 0, 0x7fff);
	pin_value(&variant->unknown54, 0, 0x7fff);
	pin_value(&variant->unknown58, 0, 2);
	pin_value(&variant->unknown74, 0, 0x10);
	pin_value(&variant->unknown78, 0, 0x10);
	pin_value(&variant->unknown7c, 0, 0x7fff);
	pin_value(&variant->unknown80, 0, 0x7fff);
	pin_value(&variant->unknown84, 0, 0x7fff);
	pin_value(&variant->unknown88, 0, 2);
	pin_value(&variant->unknowna4, 0, 2);
	pin_value(&variant->unknowna8, 0, 2);
	pin_value(&variant->unknownac, 0, 0x7fff);
	variant->unknownb0 = 0;
	variant->unknownb4 = PIN(variant->unknownb4, 0, 8);
	pin_value(&variant->unknowncc, 0, 3);
	pin_value(&variant->unknowncd, 0, 7);
	pin_value(&variant->unknownce, 0, 7);
	pin_value(&variant->unknowncf, 0, 4);
	pin_value(&variant->unknownd0, 0, 4);
	pin_value(&variant->unknownd1, 0, 4);
	pin_value(&variant->unknownd2, 0, 6);
	pin_value(&variant->unknownd3, 0, 6);
	pin_value(&variant->unknownd4, 0, 0x13);
	pin_value(&variant->unknownd5, 0, 3);
	pin_value(&variant->unknownd6, 0, 0x14);
	pin_value(&variant->unknownd7, 0, 0x14);
	pin_value(&variant->field_xcb8724, 1, 9);
	if (variant->field_xcb8724 == original.field_xcb8724)
	{
		switch (variant->field_xcb8724)
		{
		case 9:
			pin_value(&variant->flag.unknown108, 0, 0xf);
			pin_value(&variant->flag.unknown10a, 0, 0xf);
		case 1:
			variant->flags48 |= 1;
			flag_settings_verify(&variant->flag);
			break;
		case 2:
			variant->engine_flags &= 7;
			break;
		case 3:
			variant->ball.flags &= 7;
			pin_value(&variant->ball.unknownf4, 0, 3);
			pin_value(&variant->ball.unknownf6, 0, 1);
			pin_value(&variant->ball.unknownf8, 0, 2);
			pin_value(&variant->ball.unknownfa, 0, 3);
			break;
		case 4:
			king_settings_verify(&variant->king, 0x1f, 0x7fff);
			break;
		case 7:
			variant->flags48 &= ~1;
			king_settings_verify(&variant->king, 0x7f, 2);
			break;
		case 8:
			pin_value(&variant->territories.unknownf0, 1, 8);
			pin_value(&variant->territories.unknownf2, 1, 0x7fff);
			pin_value(&variant->territories.unknownf4, 1, 0x7fff);
			break;
		default:
			variant->field_xcb8724 = 10;
			break;
		}
	}
	if (memcmp(&original, variant, sizeof(original)) != 0)
	{
		variant_defaults_initialize(variant, 0);
		valid = false;
	}
	return valid;
}

/* whether the variant is valid as it is: checks a copy */
// @retail 0x19d620
bool function_19d620(s_game_variant *variant)
{
	s_game_variant copy = *variant;

	return function_19d650(&copy);
}
