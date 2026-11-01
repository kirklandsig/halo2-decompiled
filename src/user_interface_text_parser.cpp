// @flags /O1 /Oi /arch:SSE /Gr
#include "view_context_storage.h"
/* USER_INTERFACE_TEXT_PARSER.CPP: the user interface's text parser. A
   string's private use characters (0xe000..0xf8ff) are either glyphs (the
   controller buttons) or stand for a value the parser fills in: the map
   name, the game type, a player's gamertag, the time and so on. Emoticons
   become their glyphs too. */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include <wchar.h>
#include "globals.h"
#include "data_array.h"
#include "screen_widgets.h"
#include "unknown_234c64.h"
#include "unknown_x8d43e5.h"

#define NUMBEROF(array) (sizeof(array) / sizeof((array)[0]))

enum
{
	k_parse_buffer_length = 0x100
};

/* ---- callees ---- */

struct s_name_buffer;
void function_08cc20(s_name_buffer *buffer, const wchar_t *name);
void unicode_string_copy(word *destination, const word *source, long maximum_count);
void ascii_string_to_unicode(const char *source, word *destination, long maximum_count);
word *function_1630e0(word *buffer, const word *format, ...);
void function_23620d(long string_handle, word *buffer);
void function_1a0180(long tag_index, long string_handle, word *buffer);
bool unicode_character_is_private_use(word c);

byte *network_session_interface_get_data_4db0(void);
long network_session_interface_get_value_49c8(void);
long network_session_interface_find_player_49b8(void);
short network_session_interface_get_value_5dd0(void);
bool network_session_interface_get_unknown64(byte *data, long *unknown84, long *unknown88, long *unknown8c);
long network_session_manager_get_value49ac(void);

bool game_variant_get_name(long index, word *name);
bool game_variant_get_description(long index, word *description);

bool function_19a84e(long *a, long *b);
long function_19989d(void);
long function_199ebc(void);
long function_199ef8(void);
byte *function_19aaa5(long player_index);
bool function_19a902(void);
long function_19a8d0(void);
short function_19ac53(void);

struct s_entry_a;
struct s_entry_b;
struct s_entry_c;
s_entry_a *function_148d61();
s_entry_b *function_19c1f0(long key);
s_entry_c *function_19c5f0(long key);

struct s_localized_name;
struct s_localized_description;
struct s_localized_short_name;
wchar_t *localized_name_get(s_localized_name *definition);
wchar_t *localized_description_get(s_localized_description *definition);
wchar_t *localized_short_name_get(s_localized_short_name *definition);

struct s_player_profile
{
	dword data[0x78];
};

struct s_player_slot_blockb82;
void player_slot_get_profile(long index, s_player_profile *profile, long *profile_index);
void function_18ff47(long player, dword *out);
bool function_18ffc3(long index, s_player_slot_blockb82 *block);

struct s_window_manager_text;
void function_14887e(s_screen_settings_54dc6c *settings);
const char *function_148956(s_window_manager_text *text);
void function_14896e(s_window_manager_754 *a, s_window_manager_df6 *b);
long function_14de70(long local_player_index);

long saved_film_size_in_blocks();
long minimal_storage_size_in_blocks();
long saved_game_file_type_size_in_blocks(long type);
real __stdcall function_122dd0(byte *map_name, long mode, long type = 0);
struct s_name_buffer;
void function_13934d(s_name_buffer *buffer, long string_handle);
void function_15ea80(long string_handle, word *buffer, long maximum_count);
struct s_friend_request;
bool friend_request_get(s_friend_request *request);
void friends_lists_get_user(XUID const *xuid, bool *arg_a721be, bool *is_player, dword *flags, DWORD *title_id, bool *in_session, XONLINE_FRIEND *field_xb3bdcf);
void title_name_get(wchar_t *name, long name_length, dword title_id);

/* ---- data ---- */

s_54e49c g_54e49c_storage;

struct s_510c4c;
extern s_510c4c *g_510c4c;

/* the game engine globals' views the hud strings read */
struct s_game_engine_globals_view
{
	byte unknown000[0x1b0];
	long hud_quantity;
	byte unknown1b4[4];
	long betraying_player;
	void *scoreboard;
};

/* a controller's button layout (0x1c bytes, by input user) */
struct s_button_layout
{
	byte unknown00[8];
	byte buttons[0x10];
	short stick_layout;
	byte unknown1a[2];
};

/* the input users' button layouts (unknown_217c20.cpp) */
extern byte g_51ea18[];

/* a player (0x21c bytes) */
struct s_player_view
{
	byte unknown000[0x24];
	long input_user;
	byte unknown028[0x21c - 0x28];
};

/* the strings the procedures keep (the session's last map, g_54e7c0 and
   g_54e7c4, is in unknown_234c64.h) */
wchar_t g_55c2e4[k_parse_buffer_length];
wchar_t g_55c4e4[k_parse_buffer_length];
wchar_t g_55c6e4[k_parse_buffer_length];
wchar_t g_55c8e4[k_parse_buffer_length];
long g_55c2e0;
long g_55cae4;
long g_55cae8;
extern bool g_51055d;

/* the system time the clock strings show (defined with the message display screen) */
extern SYSTEMTIME g_54e7ce;

extern dword g_54d5b8;

typedef void (__stdcall *text_parse_proc)(long string_handle, word *buffer);

/* a special character: its name, the character and what it stands for (no
   procedure for a glyph) */
struct s_text_parse_entry
{
	const wchar_t *name;
	long character;
	text_parse_proc proc;
};

/* an emoticon and its glyph */
struct s_emoticon
{
	const wchar_t *text;
	long character;
};

static inline s_game_variant *get_game_variant()
{
	s_game_variant *variant = (s_game_variant *)network_session_interface_get_data_4db0();

	if (!variant)
	{
		variant = &g_54e4a0;
	}
	return variant;
}

static inline void parse_copy(word *buffer, const wchar_t *string)
{
	function_08cc20((s_name_buffer *)buffer, string);
}

/* ---- the procedures ---- */

/* the characters the font can't show become a box */
// @retail 0x22d28d
void parse_replace_missing_characters(word *string, long count)
{
	while (count > 0 && *string)
	{
		if (unicode_character_is_private_use(*string))
		{
			*string = 0x25a1;
		}
		string++;
		count--;
	}
}

// @retail 0x22d5a7
void __stdcall parse_mapname(long string_handle, word *buffer)
{
	long campaign_id;
	long map_id;
	const wchar_t *name;

	if (function_19a84e(&campaign_id, &map_id))
	{
		if (campaign_id != NONE)
		{
			s_entry_b *level = function_19c1f0(map_id);

			if (!level)
			{
				goto fallback;
			}
			name = localized_name_get((s_localized_name *)level);
		}
		else
		{
			s_entry_c *map = function_19c5f0(map_id);

			if (!map)
			{
				goto fallback;
			}
			name = localized_short_name_get((s_localized_short_name *)map);
		}
		if (name)
		{
			goto copy;
		}
	}
fallback:
	if (g_54e7c0 == NONE || g_54e7c4 == NONE)
	{
		goto done;
	}
	{
		s_entry_b *level = function_19c1f0(g_54e7c4);

		if (!level)
		{
			goto done;
		}
		name = localized_name_get((s_localized_name *)level);
		if (!name)
		{
			goto done;
		}
	}
copy:
	parse_copy((word *)g_55c2e4, name);
done:
	parse_copy(buffer, g_55c2e4);
}

// score 32 (was 32): earlier, at 49: our size-optimized build hoists the buffer address 0x55c6e4 into EBX
// Round 102 (lane 7, fast): the output parameter in a named local used by both copies 32; the selector
// in a named engine local 32, with the tag read after it 32; on the one-copy form the output local 54
// and the engine local 54 (the buffer address still hoisted into EBX at the definition test). In all
// of retail's size-built game code only this function and unmatched 0x2ca284 both push an address
// constant and load it into a register (0x2ca284 has calls between its uses), so no matched sibling
// shows the idiom.
// (saved in the prologue) for its two uses, the translator's pushed argument and the copy's
// EAX source; retail pushes it as an immediate and moves it into EAX again. VC7.1 /O1 always
// keeps a twice-used address constant in a callee-saved register when one use is on every
// path (compile-only probes); matched 0x148b8c pushes its constant twice only because its
// two uses lie on exclusive paths. Retail also reads the tag between the variant test and its
// branch, keeps the selector in EAX and lays the cases out 0/1/9/2/3/4/7/8 (ascending string
// IDs); with case 9 written third and the variant read raw with the tag load before the NULL
// fallback (that writes the variant getter out) only the five EBX rows differ (54 positional).
// Tried: native name ID, const globals, name declaration first, default first and absent
// cases 54; selected-name fallback result 54; numeric case order 49; descending 52; native
// copy 54; direct selector 57 (171 bytes); const variant 54; scalar names 54; entry
// variant/tag declarations 54/54; native buffer object 49; a word-typed buffer 49; seven
// adjacent case-pair layouts 48-51; translator inline/noinline 49/49; native copy
// inline/noinline 49/49; both inline 49; selected parameter 49; even-priority cases 56;
// low/high pairs 51; the translator call in each case 37 (171 bytes; ESI then holds the
// buffer), the same in retail's block order 42, with the tag read in each call 45; the buffer
// named once 49; `&g_55c6e4[0]` or `(word *)&g_55c6e4` at the translator 49/54; a static
// buffer 54; the copy called directly 54; coordinated getter inline/inline-noinline/noinline
// 49/49/49; parser inline 49; null-definition exit 72; native tag member 49 (with null exit
// 72); terminal case first 54; name-block orders 51-55; session getter and variant-wrapper
// declaration crosses 49-52 (getter inline loses two checked callers, wrapper outlining loses
// the checked variant parser); the tag read before the variant call 55; the default case
// copying and returning 58 (180 bytes); the selector read straight from the variant getter 55;
// the getter as a ternary 56; `name_id = NONE` with an `if (name_id != NONE)` guard 49/54;
// `if (!globals) goto done` 54; an explicit `goto done` after the call 54. Round 9: the switch
// and call moved into a plain static helper 150 (not inlined, 63 bytes); global optimization
// off 112 (220 bytes, diagnostic: both immediates come back, everything else spills); the copy
// written after the translator with its own return, or in each branch, 38 (178 bytes: the
// translator path still caches the buffer in ESI), with distinct address spellings there 38.
// our size-optimized build keeps the buffer address 0x55c6e4 in EBX for its two
// uses (a size tie under /O1, saved in the prologue); retail pushes it as an immediate for the
// translator and moves it into EAX again for the copy (no matched size-built function does that; a
// favour-speed setting gives it, not kept). With case 9 written third (retail's block order
// 0/1/9/2/3/4/7/8, ascending string IDs) and the variant read raw with the tag load before the default
// fallback, every other instruction lines up (54 positional: only the EBX rows differ). Tried: native
// name ID, const globals, name declaration first, default first and absent cases 54; direct tag field
// 45 (171 bytes); existing getter/copy force-inline 54/54; selected-name fallback result 54; numeric
// case order 49; descending 52; native copy 54; direct selector 57 (171 bytes); const variant 54;
// scalar names 54; entry variant/tag declarations 54/54; native buffer object 49; seven adjacent
// case-pair layouts 48/49/51/51/51/51/50; translator inline/noinline 49/49; native copy
// our size-optimized build keeps the buffer address 0x55c6e4 in EBX for its two
// uses (saved in the prologue; in bytes a tie under /O1: push/pop ebx + mov ebx, imm against push imm +
// mov eax, imm); retail pushes it as an immediate for the translator and moves it into EAX again for the
// copy (no matched size-built function does that; a favour-speed setting gives it, not kept). With case
// 9 written third (retail's block order 0/1/9/2/3/4/7/8, ascending string IDs) and the variant read raw
// with the tag load before the default fallback, every other instruction lines up (54 positional: only
// the EBX rows differ).
// Tried: native name ID, const globals, name declaration first, default first and absent cases 54;
// direct tag field 45 (171 bytes); existing getter/copy force-inline 54/54; selected-name fallback
// result 54; numeric case order 49; descending 52; native copy 54; direct selector 57 (171 bytes); const
// variant 54; scalar names 54; entry variant/tag declarations 54/54; native buffer object 49; seven
// adjacent case-pair layouts 48/49/51/51/51/51/50; translator inline/noinline 49/49; native copy
// inline/noinline 49/49; both inline 49; selected parameter 49; even-priority cases 56; low/high pairs
// 51; the translator call in each case 37 (171 bytes; ESI then holds the buffer), the same in retail's
// block order 42 (171 bytes), with the tag read in each call 45 (171 bytes: only the EBX buffer and the
// tag load differ); the buffer named once 49; `&g_55c6e4[0]` at the translator 49; coordinated getter
// inline/inline-noinline/noinline 49/49/49; parser inline 49; null-definition exit 72; native tag
// member 49; native member with null exit 72; terminal case first 54; name-block order 54;
// reverse/encoded name order 51/51; entry/middle/tail name pairs 55/53/53; tail/middle groups first
// 53/54; session getter inline/noinline crossed with ordinary, forceinline, noinline and inline-noinline
// shared variant-wrapper declarations 49/49/52/52/49/49/52/52 (getter inline loses two checked callers,
// wrapper outlining loses the checked variant parser); the tag read before the variant call 55 (171
// bytes); the default case copying and returning 58 (180 bytes); a const tag 49 (identical); the
// selector read straight from the variant getter 55 (171 bytes); the copy repeated after the translator
// with its own return 38 at 178 bytes (the switch path then caches the buffer in ESI, the default path
// uses the immediate; not retail's shape); the getter as a ternary 56; case 9 third 54; the variant
// read raw with the tag load before the fallback 49; both 54; with both, a static buffer, the copy
// called directly and `&g_55c6e4` at the translator 54/54/54; retail's case order with `name_id = NONE`
// and an `if (name_id != NONE)` guard in place of the default's `goto done` 54; with `if (!globals)
// goto done;` 54; retail's order with the tag read in the call plus the guard 45, plus the early exit
// 45; on the retail-order body `&g_55c6e4[0]` at the copy 54, an early `goto done` for a missing
// definition 54, the selector straight from the getter 57 (171 bytes), an explicit `goto done` after
// the call 54; the buffer as a word array (no cast at the translator) 49, on the retail-order body 54;
// the name preset to NONE with the call guarded by `name_id != NONE` 49/54 (identical code: the guard
// is threaded away); a file-static buffer 49; explicit empty cases 5 and 6 on the retail-order body
// 54. The name typed as dword 49/54.
// Round 35 (more): retail's case order alone 54; with `name_id = NONE` and an `if (name_id != NONE)`
// guard in place of the default's `goto done` 54; with `if (!globals) goto done;` 54; retail's order
// with the tag read in the call plus the guard 45, plus the early exit 45. DL179: the getter/wrapper
// declaration crosses keep 37/37/39/39/37/37/39/39 code rows.
// Round 103 (fast; parse_variant stays matched): the copy called directly 49; `(word *)&g_55c6e4` at
// the translator 49; the tag read in the shared call 45 (171 bytes); the name declared at the top 49;
// the tag read before the variant call 55 again (171 bytes; a repeat of the listed form); the unused
// `string_handle` parameter holding the case's string handle 49.
// our size-optimized build keeps the buffer address in EBX for its two uses; retail
// our size-optimized build hoists the buffer address 0x55c6e4 into EBX for its
// two uses (saved in the prologue); retail pushes it as an immediate for the translator and moves
// it into EAX again for the copy (no matched size-built function does that; a favour-speed setting
// gives it, not kept). With case 9 written third (retail's block order 0/1/9/2/3/4/7/8) and the
// variant read raw with the tag load before the default fallback, every other instruction lines
// up (54 positional: only the five EBX rows differ). Tried: native name ID, const globals, name
// declaration first, default first and absent cases 54; direct tag field 45 (171 bytes); existing
// getter/copy force-inline 54/54; selected-name fallback result 54; numeric case order 49;
// descending 52; native copy 54; direct selector 57 (171 bytes); const variant 54; scalar names
// 54; entry variant/tag declarations 54/54; native buffer object 49; seven adjacent case-pair
// layouts 48/49/51/51/51/51/50; translator inline/noinline 49/49; native copy inline/noinline
// 49/49; both inline 49; selected parameter 49; even-priority cases 56; low/high pairs 51; the
// translator call in each case 37 (171 bytes; ESI then holds the buffer), the same in retail's
// block order 42 (171 bytes), with the tag read in each call 45 (171 bytes); the buffer named once
// 49; `&g_55c6e4[0]` at the translator 49; coordinated getter inline/inline-noinline/noinline
// 49/49/49; parser inline 49; null-definition exit 72; native tag member 49; native member with
// null exit 72; terminal case first 54; name-block order 54; reverse/encoded name order 51/51;
// entry/middle/tail name pairs 55/53/53; tail/middle groups first 53/54; session getter and
// variant-wrapper declaration crosses 49/49/52/52/49/49/52/52; the tag read before the variant
// call 55 (171 bytes); the default case copying and returning 58 (180 bytes); a const tag 49; the
// pushes it as an immediate and moves it into EAX again (no matched size-built function does that; a
// favour-speed setting gives it, not kept), and the tag/selector schedule follows. In bytes the EBX
// cache is a tie (push/pop ebx + mov ebx, imm against push imm + mov eax, imm). Tried: native name
// ID, const globals, name declaration first, default first and absent cases 54; direct tag field 45
// (171 bytes; still 39 code rows); existing getter/copy force-inline 54/54; selected-name fallback
// result 54; numeric case order 49; descending 52; native copy 54; direct selector 57 at 171 bytes;
// const variant 54; scalar names 54; entry variant/tag declarations 54/54; native buffer object 49;
// seven adjacent case-pair layouts 48/49/51/51/51/51/50; translator inline/noinline 49/49; native
// copy inline/noinline 49/49; both inline 49; selected parameter 49; even-priority cases 56; low/high
// pairs 51; the translator call in each case 37 (171 bytes; ESI then holds the buffer), the same in
// retail's block order 0/1/9/2/3/4/7/8 42 (171 bytes), with the tag read in each call 45 (171 bytes:
// retail's block order and late EDI save, only the EBX buffer and the tag load differ); the buffer
// named once 49; `&g_55c6e4[0]` at the translator 49; coordinated getter inline/inline-noinline/
// noinline 49/49/49; parser inline 49; null-definition exit 72; native tag member 49; native member
// with null exit 72; terminal case first 54; name-block order 54; reverse/encoded name order 51/51;
// entry/middle/tail name pairs 55/53/53; tail/middle groups first 53/54; session getter inline/
// noinline crossed with ordinary, forceinline, noinline and inline-noinline shared variant-wrapper
// declarations 49/49/52/52/49/49/52/52 (the getter inline loses two checked callers; wrapper
// outlining loses the checked variant parser); the tag read before the variant call 55 (171 bytes);
// the default case copying and returning 58 (180 bytes); a const tag 49; the selector read straight
// from the variant getter 55 (171 bytes); the copy repeated after the translator with its own return
// 38 at 178 bytes (the switch path then caches the buffer in ESI; not retail's shape). Round 35:
// retail's case order alone 54; with `name_id = NONE` and an `if (name_id != NONE)` guard in place
// of the default's `goto done` 54; with `if (!globals) goto done;` 54; retail's order with the tag
// read in the call plus the guard 45, plus the early exit 45.
// favour-speed setting gives it, not kept), and the tag/selector schedule follows. Native name ID,
// const globals, name declaration first, default first and absent cases 54; direct tag field 45 (171
// bytes); existing getter/copy force-inline 54/54; selected-name fallback result 54; numeric case
// order 49; descending 52; native copy 54; direct selector 57 at 171 bytes; const variant 54; scalar
// names 54; entry variant/tag declarations 54/54; native buffer object 49; seven adjacent case-pair
// layouts 48/49/51/51/51/51/50; translator inline/noinline 49/49; native copy inline/noinline 49/49;
// both inline 49; selected parameter 49; even-priority cases 56; low/high pairs 51; the translator
// call in each case 37 (171 bytes; ESI then holds the buffer), the same in retail's block order
// 0/1/9/2/3/4/7/8 42 (171 bytes), with the tag read in each call 45 (171 bytes: retail's block order
// and late EDI save, only the EBX buffer and the tag load differ); the buffer named once 49;
// `&g_55c6e4[0]` at the translator 49. direct tag field 45 (171 bytes; still 39 code rows); selected-
// name fallback result 54. Numeric case order 49; entry variant/tag declarations 54/54. Coordinated
// getter inline/inline-noinline/noinline 49/49/49; parser inline 49; null-definition exit 72; native
// tag member 49; native member with null exit 72; terminal case first 54. name-block order 54;
// reverse/encoded name order 51/51; entry/middle/tail name pairs 55/53/53; tail/middle groups first
// 53/54. Executable rows do not improve; original cases restored. Native buffer object 49; seven
// adjacent case-pair layouts 48/49/51/51/51/51/50. Best executable reduction is one row. Translator
// inline/noinline 49/49; low/high pairs 51. Round 74: the tag read before the variant call 55 (171
// bytes); the default case copying and returning 58 (180 bytes); a const tag 49 (identical); the
// selector read straight from the variant getter 55 (171 bytes); the copy repeated after the
// translator with its own return 38 at 178 bytes (the switch path then caches the buffer in ESI, the
// default path uses the immediate; not retail's shape, not kept).
// our size-optimized build keeps the buffer address in EBX for its two
// uses; retail pushes it as an immediate and moves it into EAX again (no matched size-built
// function does that; a favour-speed setting gives it, not kept), and the tag/selector
// schedule follows. Native name ID, const globals, name declaration first, default first
// and absent cases 54; direct tag field 45 (171 bytes); existing getter/copy force-inline
// 54/54; selected-name fallback result 54; numeric case order 49; descending 52; native copy
// 54; direct selector 57 at 171 bytes; const variant 54; scalar names 54; entry variant/tag
// declarations 54/54; native buffer object 49; seven adjacent case-pair layouts
// 48/49/51/51/51/51/50; translator inline/noinline 49/49; native copy inline/noinline 49/49;
// both inline 49; selected parameter 49; even-priority cases 56; low/high pairs 51; the
// translator call in each case 37 (171 bytes; ESI then holds the buffer), the same in
// retail's block order 0/1/9/2/3/4/7/8 42 (171 bytes), with the tag read in each call 45
// (171 bytes: retail's block order and late EDI save, only the EBX buffer and the tag load
// differ); the buffer named once 49; `&g_55c6e4[0]` at the translator 49.
// output-buffer EBX cache and tag/selector scheduling differ.
// Native name ID, const globals, name declaration first, default first and absent cases 54;
// direct tag field 45 (171 bytes; still 39 code rows); existing getter/copy force-inline 54/54;
// selected-name fallback result 54.
// buffer register and selector/tag scheduling remain different.
// Numeric case order 49; descending 52; native copy 54; direct selector 57 at 171 bytes;
// const variant 54; scalar names 54; entry variant/tag declarations 54/54.
// Earlier: extra buffer EBX cache and selector/tag schedule remain.
// Coordinated getter inline/inline-noinline/noinline 49/49/49; parser inline 49; null-definition exit 72;
// native tag member 49; native member with null exit 72; terminal case first 54.
// extra buffer save and selector/tag schedule remain; name-block order 54;
// reverse/encoded name order 51/51; entry/middle/tail name pairs 55/53/53;
// tail/middle groups first 53/54. Executable rows do not improve; original cases restored.
// Extra EBX buffer cache and tag/selector schedule remain. Native buffer
// object 49; seven adjacent case-pair layouts 48/49/51/51/51/51/50. Best executable reduction is
// one row.
// extra EBX buffer cache and selector/tag scheduling remain.
// Translator inline/noinline 49/49; native copy inline/noinline 49/49; both inline 49;
// selected parameter 49; even-priority cases 56; low/high pairs 51.
// DL179: Session getter inline/noinline crossed with ordinary, forceinline, noinline
// and inline-noinline shared variant-wrapper declarations 49/49/52/52/49/49/52/52;
// code rows 37/37/39/39/37/37/39/39. Getter inline loses two checked callers;
// wrapper outlining loses the checked variant parser. All original declarations restored.
// translator with its own return 38 (178 bytes; not retail's shape). Round 5: the getter as a
// ternary 56; case 9 third 54; the variant read raw with the tag load before the fallback 49
// (retail's tag/selector schedule); both 54; with both, a static buffer, the copy called directly
// and `&g_55c6e4` at the translator 54/54/54.
// our size-optimized build keeps the buffer address in EBX for its two uses
// (a size tie under /O1); retail pushes it as an immediate and moves it into EAX again (no matched
// size-built function does that; a favour-speed setting gives it, not kept), and the tag/selector
// schedule follows: retail reads the tag between the variant test and its branch and keeps the
// selector in EAX. Retail lays the case blocks out as 0/1/9/2/3/4/7/8 (ascending string IDs).
// Tried: native name ID, const globals, name declaration first, default first and absent cases 54;
// selected-name fallback result 54; numeric case order 49; descending 52; native copy 54; direct
// selector 57 at 171 bytes; const variant 54; scalar names 54; entry variant/tag declarations
// 54/54; native buffer object 49; seven adjacent case-pair layouts 48/49/51/51/51/51/50;
// translator inline/noinline 49/49; native copy inline/noinline 49/49; both inline 49; selected
// parameter 49; even-priority cases 56; low/high pairs 51; the translator call in each case 37
// (171 bytes; ESI then holds the buffer), the same in retail's block order 42 (171 bytes), with the
// tag read in each call 45 (171 bytes: retail's block order and late EDI save, only the EBX buffer
// and the tag load differ); the buffer named once 49; `&g_55c6e4[0]` at the translator 49;
// coordinated getter inline/inline-noinline/noinline 49/49/49; parser inline 49; null-definition
// exit 72; native tag member 49; native member with null exit 72; terminal case first 54;
// name-block order 54; reverse/encoded name order 51/51; entry/middle/tail name pairs 55/53/53;
// tail/middle groups first 53/54; session getter inline/noinline crossed with ordinary,
// forceinline, noinline and inline-noinline shared variant-wrapper declarations
// copying and returning 58 (180 bytes); a const tag 49 (identical); the selector read straight from
// the variant getter 55 (171 bytes); the copy repeated after the translator with its own return 38
// at 178 bytes (the switch path then caches the buffer in ESI, the default path uses the
// immediate). Round 35, all with the shared call: the cases in retail's block order 54 (retail's
// layout, still the EBX buffer, two bytes ahead); on it `&g_55c6e4[0]` at the copy 54, an early
// `goto done` for a missing definition 54, the selector straight from the getter 57 (171 bytes),
// an explicit `goto done` after the call 54.
// Round 19 (lane 11): the buffer only this function references (no other retail use of 0x55c6e4),
// with internal linkage 49; the copy in the default case with its own return 58 (180 bytes); a
// NONE name tested before the translator 49; the tag read at the translator call 45 (171 bytes);
// both 45 (171 bytes).
// 54; the name typed as dword 49/54. Round 36, on the retail-order body: the missing-definition path
// copying and returning early 72 (189 bytes); `&g_55c6e4[0]` at both uses 54; the buffer named in a
// local for both uses 54; const definition pointer and tag read 54; the default label first 54.
// Round 317: Existing getter default-first/complete-result arms, crossed with case-9 priority
// and direct tag consumption, 52/56/59/54/63/45/61/45. Four-count leads rejected;
// all original getter statements, shared buffer and parser body restored.
// Later (32, from 49): retail's case order (0, 1, 9, 2, 3, 4, 7, 8) with the copy repeated after the
// translation; ours still keeps the switch's value in EDI from the entry and pushes the buffer address
// in ESI where retail pushes it as an immediate, and the selector load lands in ECX.
// Round 109 (lane 5, fast; parse_variant stays matched), on the 32 form: the tag read before the
// variant call 44 (tag in EBX, buffer still cached in ESI); the tag read in the translator call 45
// (globals in EBX, selector in EAX as retail); `if (!globals) goto done;` 32 (same code); numeric
// case order 38; the variant read raw with the tag load before the fallback (getter written out,
// diagnostic) 33 (tag load in retail's slot but in ECX, so ESI stays free to cache the buffer). A
// speed-preference diagnostic inlines the definition getter and the copy (118 at 224 bytes), so a
// speed setting alone does not give retail's immediates. Matched siblings parse_variant and
// parse_mapname cache their buffer constant in ESI; here retail's ESI holds the tag.
// Round 109 (lane 6, fast): one shared copy after the `if (globals)` block (the switch's default
// skipping the translator) 54: the tag then sits in ESI, EDI is pushed only around the switch and
// popped after the translator, as retail, but the buffer address is hoisted into EBX at the globals
// test for its two uses; on it the tag read before the variant call 57, the selector straight from
// the getter with the tag first 57, `if (!globals) goto done;` 54, the tag read at the translator call
// 45 (171 bytes; the selector then in EAX as retail, still the EBX buffer).
// Round 98 (lane 8; LTCG env link, address-taken entry; env rows positional/aligned, base 42/35): one
// shared copy after the `if (globals)` block with the default's `goto done` 47/29 (173 bytes, buffer
// hoisted into EBX); with `&g_55c6e4[0]` at the translator 47/29; the tag read at the translator call
// 43/33 (174 bytes); `name_id` declared before the definition lookup 42/35 (identical); the translator
// called in each case with the shared copy 42/35 (170 bytes).
// Round 24 (lane 14, fast), all on the natural one-copy form (54: buffer address hoisted into EBX at the
// globals test, selector in ECX, tag loaded after the range compare): the buffer address read once into a
// named `wchar_t *` local used by the translator and the copy 54 (same code); numeric case order 49; an
// early `if (!globals) goto done;` with function-scope locals 54; `name_id` preset NONE with `default:
// break` and the translator under `if (name_id != NONE)` 54; the four parse buffers as one two-dimensional
// array (sym+offset constants) 54. Retail's translator takes the name in EDI (its matched body), so EDI's
// late push is the call's argument register; the EBX hoist is the only thing the natural form lacks.
// Round 181 (lane 3): a compile-only /O1 screen hoists 0x55c6e4 into a callee-saved register in all
// six exit shapes tried (natural, copy on both paths, early null return, default copy and return,
// both exits, per-case translator), so no exit shape removes it. Real build: the natural form 54
// (173 bytes, every row aligned but the EBX hoist and the late tag load); an early copy-and-return
// for a missing definition on the current form 77 (194 bytes); the default case copying and
// returning 32 (same count); the natural form with the tag read before the variant call 57 (171
// bytes) and just before the translator 45 (171 bytes).
// Round 9 (lane 17, fast): a compile-only /O1 screen of 128 combinations (tag/variant/name declaration
// orders, retail or numeric case order, `goto done` or a NONE-guarded call, shared or repeated copy,
// `if (globals)` or `if (!globals) goto done`, the buffer named in a local or not) keeps 0x55c6e4 in a
// register on at least one path in every one. Real: tag first, NONE guard, early goto and a named
// buffer 57 (171 bytes); the name declared before the tag with a named buffer 54 (173 bytes); the name
// first, NONE guard and repeated copy 68 (189 bytes); tag first, repeated copy and early goto 44 (178
// bytes); numeric order, NONE guard, repeated copy, early goto and a named buffer 72 (189 bytes).
// Round 41 (lane 11, fast): the natural one-copy form again 54 (173 bytes; `mov ebx, 0x55c6e4` placed
// right after the globals test, ahead of its `je`, so it dominates both uses; the selector then lands
// in ECX and the tag load after the range compare); on it a NONE-defaulting switch with the translator
// under `if (name_id != NONE)` and one copy 54 (173 bytes), the tag read before the variant call 57 (171
// bytes); the tag (or the tag, variant and name) declared at function scope 54/54 (same code). Matched
// parse_leader caches its buffer in ESI only because its first copy needs it there.
// Round 119 (fast, on the 32 form): the name declared before the variant 32; the selector read into a local
// before the switch 32; a const tag 32 (all identical code); one shared copy after the `if (globals)` block 54;
// the tag read before the variant call 44. In retail the tag load sits between the variant's NULL test and
// its branch, i.e. before the getter's fallback in the IR; the shared getter keeps it after.
// Round 126 (m3 lane 6, fast; parse_variant stays matched): the shared getter as a ternary on the 32 form 48 (the
// select copies the variant into ECX, the tag still loads after the range compare), with the tag read before the
// variant call 44; on the one-copy form (default skipping the translator) the ternary getter 54 (173 bytes; buffer
// address still hoisted into EBX), with the tag read first 57 (171 bytes), with a NONE-defaulted name and a guard 54.
// Round 45 (lane 11, fast), all on the natural one-copy form (54, 173 bytes; only the EBX constant, the
// selector's ECX and the late tag load differ): the selector read into a named local before the tag 54;
// the shared getter with two returns (`if (variant) return variant; return &g_54e4a0;`) 54, with the
// selector local 54 (parse_variant stays matched); the variant declared at function scope 54; an explicit
// `else goto done;` on the globals test 54. Retail's .text holds 0x55c6e4 twice and no data table holds
// any parse buffer; the matched siblings each hold their buffer constant once (cached in ESI).
// Round 108 (lane 8; a one-file LTCG harness with the callee files and the ballast object gives this code
// exactly): the variant getter's fallback written as a ternary 48 (178 bytes; parse_variant stays matched); one
// shared copy after the `if (globals)` block 54 (173 bytes, buffer hoisted into EBX), with the ternary getter 54;
// a named pointer to the tag field read at the translator call 45 (178 bytes, globals in EBX); the tag declared
// at function scope 32 (identical). All real (fast).
// Round 14 (lane 17, fast; parse_variant stays matched), on the natural one-copy form (54): the shared
// getter's fallback as a ternary 54 (same code: the tag load stays after the fallback join), with the tag
// read before the variant call 57 (171 bytes; the tag loads before the call); the selector read into a
// local before the tag 54; numeric case order 49 (173 bytes); numeric order with the tag read first 55
// (171 bytes). Retail's tag load sits between `test eax, eax` and `jne` in the call's block, so in its IR
// the tag read precedes the fallback branch.
// Round 13 (lane 18, fast; parse_variant stays matched): a compile-only /O1 screen hoists 0x55c6e4 exactly
// when the translator path uses that address twice (a different buffer at the copy removes it), and the
// matched siblings here hoist theirs in retail too, so a file-flag change would not fit them (the earlier
// favour-speed diagnostic stays a flags lead only). Real: numeric-to-retail case order with the tag read in
// the translator call 39; `if (!globals) goto done;` with the tag read in the call 45; `name_id` preset NONE
// with `default: break` and the translator/copy under `if (name_id != NONE)` 32 (same count); the buffer
// named once for the translator and the copy 32; the tag read in the translator call alone 45.
// Round 110 (m3 lane 7, fast): a compile-only /O1 screen puts the tag load between the getter's test and branch
// only with the getter written out (call, tag read, then the fallback; not keepable). On the natural one-copy form:
// the variant declared bare before the definition getter 54 (as round 41's function-scope declarations); the default case copying and returning 63 (180
// bytes); `case 5: case 6: default: goto done;` 54; the tag and name declared before the variant, the tag assigned
// after it 54; `if (globals != NULL)` 54.
// DL218: Zero-default name results with local/entry initialization and complete signed/unsigned
// arms tie 32 with the repeated exit, 54 with one copy. No reduction of entry EDI save or scratch-
// buffer cache; original complete parser and all call values restored.
// @retail 0x22d456
void __stdcall parse_gametype(long string_handle, word *buffer)
{
	s_type_954545 *globals = ui_definition_get();

	if (globals)
	{
		s_game_variant *variant = get_game_variant();
		long tag_index = *(long *)((byte *)globals + 0x14c);
		long name_id;

		switch (variant->field_xcb8724)
		{
		case 0:
			name_id = 0x800010b;
			break;
		case 1:
			name_id = 0x1000010d;
			break;
		case 9:
			name_id = 0x700010e;
			break;
		case 2:
			name_id = 0x600010f;
			break;
		case 3:
			name_id = 0x7000110;
			break;
		case 4:
			name_id = 0x10000111;
			break;
		case 7:
			name_id = 0xa000113;
			break;
		case 8:
			name_id = 0xb000115;
			break;
		default:
			goto done;
		}
		function_1a0180(tag_index, name_id, (word *)g_55c6e4);
		parse_copy(buffer, g_55c6e4);
		return;
	}
done:
	parse_copy(buffer, g_55c6e4);
}
// @retail 0x22d503
void __stdcall parse_variant(long string_handle, word *buffer)
{
	parse_copy((word *)g_55c4e4, get_game_variant()->name);
	parse_copy(buffer, g_55c4e4);
}

// @retail 0x22d52e
void __stdcall parse_hopper_name(long string_handle, word *buffer)
{
	word name[0x10];
	long hopper = network_session_interface_get_value_49c8();

	if (hopper != NONE)
	{
		g_55c2e0 = hopper;
	}
	if (game_variant_get_name(g_55c2e0, name))
	{
		parse_copy(buffer, name);
	}
}

// @retail 0x22d566
void __stdcall parse_hopper_description(long string_handle, word *buffer)
{
	word description[0x80];
	long hopper = network_session_interface_get_value_49c8();

	if (hopper != NONE)
	{
		g_55c2e0 = hopper;
	}
	if (game_variant_get_description(g_55c2e0, description))
	{
		parse_copy(buffer, description);
	}
}

// @retail 0x22d62b
void __stdcall parse_motion_sensor_enabled(long string_handle, word *volatile buffer)
{
	s_game_variant *variant = (s_game_variant *)network_session_interface_get_data_4db0();
	word *output = buffer;

	if (variant && TEST_FIELD_BIT(variant->motion_sensor_enabled))
	{
		function_23620d(0x2000181, output);
	}
	else
	{
		function_23620d(0x3000182, output);
	}
}

// @retail 0x22d655
void __stdcall parse_teams_enabled(long string_handle, word *volatile buffer)
{
	s_game_variant *variant = (s_game_variant *)network_session_interface_get_data_4db0();
	word *output = buffer;

	if (variant && TEST_FIELD_BIT(variant->teams_enabled))
	{
		function_23620d(0x7000183, output);
	}
	else
	{
		function_23620d(0x8000184, output);
	}
}

/* the vehicle set and the build number show nothing */
// @retail 0x22d82c
void __stdcall parse_nothing(long string_handle, word *buffer)
{
}

// @retail 0x22d67c
void __stdcall parse_weapon_set(long string_handle, word *buffer)
{
	function_23620d(0x60000b8, buffer);
}

// @retail 0x22d68f
void __stdcall parse_player_profile_name(long string_handle, word *buffer)
{
	s_player_profile profile;
	long player;

	switch (string_handle)
	{
	default:
		player = NONE;
		break;
	case 0xe40f:
		player = 0;
		break;
	case 0xe410:
		player = 1;
		break;
	case 0xe411:
		player = 2;
		break;
	case 0xe412:
		player = 3;
		break;
	}
	if (player == NONE)
	{
		parse_copy(buffer, (wchar_t *)g_54e5d0.settings.name);
		return;
	}
	player_slot_get_profile(player, &profile, &string_handle);
	parse_copy(buffer, (wchar_t *)&profile.data[2]);
}
// @retail 0x22d6ee
void __stdcall parse_player_gamertag(long string_handle, word *buffer)
{
	dword identity[0x1c];
	long player;

	switch (string_handle)
	{
	case 0xe429:
		player = 0;
		break;
	case 0xe42a:
		player = 1;
		break;
	case 0xe42b:
		player = 2;
		break;
	default:
		player = 3;
		break;
	}
	function_18ff47(player, identity);
	ascii_string_to_unicode((const char *)&identity[3], buffer, k_parse_buffer_length);
}

// @retail 0x22d730
void __stdcall parse_countdown(long string_handle, word *buffer)
{
	if (function_19a902())
	{
		long seconds = function_19a8d0();
		long minutes = seconds / 60;

		function_1630e0(buffer, (const word *)L"%d:%02d", minutes, seconds - minutes * 60);
	}
	else
	{
		parse_copy(buffer, L"-:--");
	}
}

// @retail 0x22d775
void __stdcall parse_matchmaking_countdown(long string_handle, word *buffer)
{
	long seconds = network_session_manager_get_value49ac();

	if (seconds != NONE)
	{
		function_1630e0(buffer, (const word *)L":%02d", seconds);
	}
	else
	{
		parse_copy(buffer, L":--");
	}
}

// @retail 0x22d7a1
void __stdcall parse_new_content(long string_handle, word *buffer)
{
	if (g_51055d)
	{
		function_1630e0(buffer, (const word *)L"%c", 0xe007);
	}
	else
	{
		buffer[0] = 0;
	}
}

// @retail 0x22d7ce
void __stdcall parse_map_load_percent(long string_handle, word *buffer)
{
	real progress = 0.0f;
	byte *entry = (byte *)function_148d61();

	if (entry)
	{
		progress = function_122dd0(entry + 8, 0);
	}
	function_1630e0(buffer, (const word *)L"%d", (long)(progress * 100.0f));
}

// @retail 0x22d80b
void __stdcall parse_name_of_last_person_to_delay_countdown(long string_handle, word *buffer)
{
	long player_index = network_session_interface_find_player_49b8();

	if (player_index != NONE)
	{
		byte *player = function_19aaa5(player_index);

		if (player)
		{
			parse_copy(buffer, (wchar_t *)player);
		}
	}
}

// @retail 0x22d82f
void __stdcall parse_leader(long string_handle, word *buffer)
{
	long player_index = function_19ac53();

	if (player_index != NONE)
	{
		byte *player = function_19aaa5(player_index);

		if (player)
		{
			parse_copy((word *)g_55c8e4, (wchar_t *)player);
		}
	}
	parse_copy(buffer, g_55c8e4);
}

/* the quality of service values */
// @retail 0x22d85f
void __stdcall parse_qos(long string_handle, word *buffer)
{
	struct
	{
		long probes_sent;
		long probes_received;
		long rtt_minimum;
		long rtt_median;
		long bandwidth_up;
		long bandwidth_down;
		byte unknown18[8];
	} qos;
	long estimated_bandwidth;
	long maximum_machines;
	long nat_type;
	long value = 0;
	real kilobits;

	if (network_session_interface_get_unknown64((byte *)&qos, &nat_type, &estimated_bandwidth, &maximum_machines))
	{
		switch (string_handle)
		{
		case 0xe417:
			value = qos.probes_sent;
			break;
		case 0xe418:
			value = qos.probes_received;
			break;
		case 0xe419:
			value = qos.rtt_minimum;
			break;
		case 0xe41a:
			value = qos.rtt_median;
			break;
		case 0xe41b:
			kilobits = (real)qos.bandwidth_down * (1.0f / 1024.0f);
			goto print_real;
		case 0xe41c:
			kilobits = (real)qos.bandwidth_up * (1.0f / 1024.0f);
			goto print_real;
		case 0xe41d:
			value = maximum_machines;
			break;
		case 0xe41e:
			value = nat_type;
			break;
		case 0xe432:
			value = (long)((real)estimated_bandwidth * (1.0f / 1024.0f));
			break;
		}
	}
	function_1630e0(buffer, (const word *)L"%d", value);
	return;

print_real:
	if (kilobits != 0.0f)
	{
		function_1630e0(buffer, (const word *)L"%.1f", (double)kilobits);
		return;
	}
	function_1630e0(buffer, (const word *)L"%d", value);
}

// @retail 0x22d93b
void __stdcall parse_active_multiplayer_protocol(long string_handle, word *buffer)
{
	long name_id;
	switch (function_19989d())
	{
	case 0:
		name_id = 0x100005be;
		break;
	case 1:
		name_id = 0x120005c0;
		break;
	case 2:
		name_id = 0x100005c1;
		break;
	case 3:
		name_id = 0x120005c3;
		break;
	case 4:
		name_id = 0x90005c4;
		break;
	case 5:
		name_id = 0xb0005c6;
		break;
	case 6:
		name_id = 0xe0005c8;
		break;
	default:
		goto unchanged;
	}
	g_55cae8 = name_id;
unchanged:
	function_23620d(g_55cae8, buffer);
}
// @retail 0x22d99e
void __stdcall parse_live_ui_driver_gamertag(long string_handle, word *buffer)
{
	ascii_string_to_unicode((const char *)0x54e438, buffer, k_parse_buffer_length);
}

// @retail 0x22d9b4
void __stdcall parse_live_ui_driver_clan_name(long string_handle, word *buffer)
{
	byte clan[0x6a4];

	if (friend_request_get((s_friend_request *)clan))
	{
		parse_copy(buffer, (wchar_t *)&clan[0xc]);
	}
}

// @retail 0x22d9e0
void __stdcall parse_hud_quantity(long string_handle, word *buffer)
{
	long quantity = 0;

	if (string_handle == 0xe422)
	{
		quantity = ((s_game_engine_globals_view *)g_510c4c)->hud_quantity;
	}
	function_1630e0(buffer, (const word *)L"%d", quantity);
}

// @retail 0x22da0d
void __stdcall parse_hud_betraying_player(long string_handle, word *buffer)
{
	long player_index = NONE;

	if (string_handle == 0xe458)
	{
		player_index = ((s_game_engine_globals_view *)g_510c4c)->betraying_player;
	}
	function_1630e0(buffer, (const word *)L"??");
	if (player_index != NONE)
	{
		byte *player = record_pool_lookup(g_4e8c24, player_index);

		if (player)
		{
			function_1630e0(buffer, (const word *)L"%s", player + 0x44);
		}
	}
}

// @retail 0x22da65
void __stdcall parse_hud_scoreboard(volatile long string_handle, word *volatile buffer)
{
    /* Read the buffer before the token. */
    s_name_buffer *output = (s_name_buffer *)buffer;
    long token = string_handle;
    function_13934d(output, token);
}

// @retail 0x22da75
void __stdcall parse_ge_round_time_left(long string_handle, word *buffer)
{
	word time[0x14];

	if (g_4e6948->state == 2)
	{
		function_15ea80(string_handle, time, NUMBEROF(time));
		parse_copy(buffer, time);
	}
}

// @retail 0x22daa5
void __stdcall parse_target_player_gamertag(long string_handle, word *buffer)
{
	s_screen_settings_54dc6c settings;
	const char *gamertag;

	function_14887e(&settings);
	gamertag = function_148956((s_window_manager_text *)&settings);
	if (gamertag)
	{
		ascii_string_to_unicode(gamertag, buffer, k_parse_buffer_length);
	}
}

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)

// @retail 0x22dad1
void __stdcall parse_game_player_count(long string_handle, word *buffer)
{
	long names[0x11];
	long count;

	names[0] = 0x100030a;
	names[1] = 0x100030b;
	names[2] = 0x100030c;
	names[3] = 0x100030d;
	names[4] = 0x100030e;
	names[5] = 0x100030f;
	names[6] = 0x1000310;
	names[7] = 0x1000311;
	names[8] = 0x1000312;
	names[9] = 0x1000313;
	names[10] = 0x2000314;
	names[11] = 0x2000315;
	names[12] = 0x2000316;
	names[13] = 0x2000317;
	names[14] = 0x2000318;
	names[15] = 0x2000319;
	names[16] = 0x200031a;
	count = function_199ef8();
	// Keep the session-count reads ordered through whole-program optimization.
	_ReadWriteBarrier();
	if (function_199ebc() > count)
	{
		count = function_199ebc();
	}
	else
	{
		count = function_199ef8();
	}
	if (count >= 0)
	{
		if (count > 0x10)
		{
			count = 0x10;
		}
		if (count)
		{
			g_55cae4 = count;
		}
	}
	function_23620d(names[g_55cae4], buffer);
}

#pragma function(_ReadWriteBarrier)

// @retail 0x22db96
void __stdcall parse_coop_level_name(long string_handle, word *buffer)
{
	long campaign_id = NONE;
	long map_id = NONE;

	buffer[0] = 0;
	function_19a84e(&campaign_id, &map_id);
	if (campaign_id != NONE && map_id != NONE)
	{
		s_entry_b *level = function_19c1f0(map_id);

		if (level)
		{
			parse_copy(buffer, localized_name_get((s_localized_name *)level));
		}
	}
}

// @retail 0x22dbe0
void __stdcall parse_coop_level_description(long string_handle, word *buffer)
{
	long campaign_id = NONE;
	long map_id = NONE;

	buffer[0] = 0;
	function_19a84e(&campaign_id, &map_id);
	if (campaign_id != NONE && map_id != NONE)
	{
		s_entry_b *level = function_19c1f0(map_id);

		if (level)
		{
			parse_copy(buffer, localized_description_get((s_localized_description *)level));
		}
	}
}

// @retail 0x22dc2a
void __stdcall parse_coop_difficulty(long string_handle, word *buffer)
{
	long name_id;

	switch (network_session_interface_get_value_5dd0())
	{
	case 0:
		name_id = 0x400028e;
		break;
	case 1:
		name_id = 0x60000b8;
		break;
	case 2:
		name_id = 0x600028f;
		break;
	case 3:
		name_id = 0x9000290;
		break;
	default:
		name_id = 0x60000b8;
		break;
	}
	function_23620d(name_id, buffer);
}

/* sizes in blocks */
// @retail 0x22dc68
void __stdcall parse_blocks(long string_handle, word *buffer)
{
	long blocks = 0;

	switch (string_handle)
	{
	case 0xe433:
		blocks = saved_game_file_type_size_in_blocks(0);
		break;
	case 0xe434:
		blocks = saved_game_file_type_size_in_blocks(1);
		break;
	case 0xe436:
		blocks = 0x4a2b1e2;
		break;
	case 0xe444:
		blocks = saved_film_size_in_blocks();
		break;
	case 0xe445:
		blocks = minimal_storage_size_in_blocks();
		break;
	}
	function_1630e0(buffer, (const word *)L"%d", blocks);
}

// @retail 0x22e1ed
void __stdcall parse_animating_thumbstick(long string_handle, word *buffer)
{
	word character[2];
	dword phase = g_54d5b8 % 500;

	if (phase < 0xa6)
	{
		switch (string_handle)
		{
		case 0xe459:
			character[0] = 0xe12c;
			break;
		case 0xe45a:
			character[0] = 0xe12f;
			break;
		default:
			character[0] = '?';
			break;
		}
	}
	else if (phase < 0x14d)
	{
		switch (string_handle)
		{
		case 0xe459:
			character[0] = 0xe12d;
			break;
		case 0xe45a:
			character[0] = 0xe130;
			break;
		default:
			character[0] = '?';
			break;
		}
	}
	else
	{
		switch (string_handle)
		{
		case 0xe459:
			character[0] = 0xe12e;
			break;
		case 0xe45a:
			character[0] = 0xe131;
			break;
		default:
			character[0] = '?';
			break;
		}
	}
	character[1] = 0;
	parse_copy(buffer, character);
}

PRIVATE __forceinline long selected_button_22dcbc(s_button_layout const *layout, char index)
{
	if (index != NONE)
		return layout->buttons[index];
	else if (layout->buttons[7] == 7)
		return 6;
	return 7;
}

// @retail 0x22dcbc
void __stdcall parse_button(long string_handle, word *buffer)
{
	long player_index = function_14de70(g_4b9ed8);
	char index;
	word character[2];

	switch (string_handle)
	{
	case 0xe446:
		index = 0;
		break;
	case 0xe447:
		index = 1;
		break;
	case 0xe448:
		index = 2;
		break;
	case 0xe449:
		index = 3;
		break;
	case 0xe44a:
		index = 4;
		break;
	case 0xe44b:
		index = 5;
		break;
	case 0xe44c:
		index = 6;
		break;
	case 0xe44d:
		index = 7;
		break;
	case 0xe44e:
		index = 8;
		break;
	case 0xe44f:
		index = 9;
		break;
	case 0xe450:
		index = 10;
		break;
	case 0xe451:
		index = 11;
		break;
	case 0xe452:
		index = 12;
		break;
	case 0xe453:
		index = 13;
		break;
	case 0xe454:
		index = 14;
		break;
	case 0xe455:
		index = 15;
		break;
	case 0xe45e:
		index = NONE;
		break;
	default:
		__assume(0);
	}
	if (player_index != NONE)
	{
		s_player_view *player = &((s_player_view *)g_4e8c24->data)[player_index & 0xffff];
		s_player_view *const *player_reference = &player;
		long input_user = (*player_reference)->input_user;

		if (input_user != NONE)
		{
			s_button_layout layout;
			long button = 7;
			word thumbstick[k_parse_buffer_length];

			thumbstick[0] = 0;
			character[1] = 0;
			layout = ((s_button_layout *)g_51ea18)[input_user];
			character[0] = '?';
			button = selected_button_22dcbc(&layout, index);
			switch (button)
			{
			case 0:
				character[0] = 0xe100;
				break;
			case 1:
				character[0] = 0xe101;
				break;
			case 2:
				character[0] = 0xe102;
				break;
			case 3:
				character[0] = 0xe103;
				break;
			case 4:
				character[0] = 0xe104;
				break;
			case 5:
				character[0] = 0xe105;
				break;
			case 6:
				character[0] = 0xe106;
				break;
			case 7:
				character[0] = 0xe107;
				break;
			case 8:
				character[0] = 0xe108;
				break;
			case 9:
				character[0] = 0xe109;
				break;
			case 10:
				character[0] = 0xe10a;
				break;
			case 11:
				character[0] = 0xe10b;
				break;
			case 12:
				character[0] = 0xe10c;
				break;
			case 13:
				character[0] = 0xe10d;
				break;
			case 14:
				parse_animating_thumbstick(0xe459, thumbstick);
				character[0] = thumbstick[0];
				break;
			case 15:
				parse_animating_thumbstick(0xe45a, thumbstick);
				character[0] = thumbstick[0];
				break;
			}
			parse_copy(buffer, character);
		}
	}
	else
	{
		function_1630e0(buffer, (const word *)L"?");
	}
}

// @retail 0x22df04
void __stdcall parse_stick(long string_handle, word *buffer)
{
	long player_index = function_14de70(g_4b9ed8);
	word character[4];

	memset(character, 0, sizeof(character));

	if (player_index != NONE)
	{
		s_player_view *player = &((s_player_view *)g_4e8c24->data)[player_index & 0xffff];
		s_player_view *const *player_reference = &player;
		long input_user = (*player_reference)->input_user;

		if (input_user != NONE)
		{
			s_button_layout layout = ((s_button_layout *)g_51ea18)[input_user];

			if (string_handle == 0xe456)
			{
				switch (layout.stick_layout)
				{
				case 0:
					character[0] = 0xe110;
					break;
				case 1:
					character[0] = 0xe111;
					break;
				case 2:
					character[0] = 0xe110;
					break;
				case 3:
					character[0] = 0xe111;
					break;
				}
			}
			else if (string_handle == 0xe457)
			{
				switch (layout.stick_layout)
				{
				case 0:
					character[0] = 0xe111;
					break;
				case 1:
					character[0] = 0xe110;
					break;
				case 2:
					character[0] = 0xe111;
					break;
				case 3:
					character[0] = 0xe110;
					break;
				}
			}
		}
	}
	parse_copy(buffer, character);
}

#pragma pack(push, 4)
/* the player a screen targets, as the window manager keeps it (0x78
   bytes) */
struct s_target_player
{
	long type;
	unsigned __int64 xuid;
	byte unknown0c[0x78 - 0xc];
};

/* a player's identity (0x6a2 bytes) and clan (0x92 bytes) */
#pragma pack(push, 2)
struct s_player_identity_view
{
	unsigned __int64 xuid;
	long unknown08;
	wchar_t field_c_3[0x34b];
};
#pragma pack(pop)

struct s_clan_view
{
	unsigned __int64 clan_id;
	byte unknown08[0x1c - 0x08];
	long level;
	byte unknown20[0x92 - 0x20];
};
#pragma pack(pop)

// @retail 0x22dfa9
void __stdcall parse_target_player_clan_name(long string_handle, word *buffer)
{
	s_target_player target;

	function_14887e((s_screen_settings_54dc6c *)&target);
	switch (target.type)
	{
	case 1:
		if (target.xuid) goto selected;
		return;
	case 2:
		if (target.xuid) goto selected;
		return;
	default:
		return;
	}
selected:
	{
		s_player_identity_view identity;
		s_window_manager_df6 clan;

		function_14896e((s_window_manager_754 *)&identity, (s_window_manager_df6 *)&clan);
		if (identity.xuid)
		{
			parse_copy(buffer, identity.field_c_3);
		}
	}
}

// @retail 0x22e001
void __stdcall parse_target_player_game_title_name(long string_handle, word *buffer)
{
	DWORD title_id;
	bool online;
	bool joinable;
	bool playing;
	dword state;
	{
		s_target_player target;
		function_14887e((s_screen_settings_54dc6c *)&target);
		switch (target.type)
		{
		case 1:
			if (target.xuid) goto selected;
			return;
		case 2:
			if (target.xuid) goto selected;
			return;
		default:
			return;
		}
selected:
		friends_lists_get_user((XUID const *)&target.xuid, &online, &joinable,
			&state, &title_id, &playing, 0);
	}
	if (title_id)
	{
		word title[0x40];
		title_name_get((wchar_t *)title, NUMBEROF(title), title_id);
		parse_copy(buffer, title);
	}
}

// @retail 0x22e06c
void __stdcall parse_target_player_clan_level(long string_handle, word *buffer)
{
	s_player_identity_view identity;
	s_clan_view clan;
	long name_id = 0;

	function_14896e((s_window_manager_754 *)&identity, (s_window_manager_df6 *)&clan);
	if (identity.xuid && clan.clan_id)
	{
		switch (clan.level)
		{
		case 0:
			name_id = 0x40002c6;
			break;
		case 1:
			name_id = 0x60002c7;
			break;
		case 2:
			name_id = 0xd0002c8;
			break;
		case 3:
			name_id = 0x90002c9;
			break;
		}
	}
	function_23620d(name_id, buffer);
}

// @retail 0x22e0df
void __stdcall parse_live_ui_driver_clan_level(long string_handle, word *buffer)
{
	s_clan_view clan;
	long name_id = 0;

	if (function_18ffc3(g_global_4acf62.controller_index, (s_player_slot_blockb82 *)&clan) && clan.clan_id)
	{
		switch (clan.level)
		{
		case 0:
			name_id = 0x40002c6;
			break;
		case 1:
			name_id = 0x60002c7;
			break;
		case 2:
			name_id = 0xd0002c8;
			break;
		case 3:
			name_id = 0x90002c9;
			break;
		}
	}
	function_23620d(name_id, buffer);
}

// @retail 0x22e148
void __stdcall parse_hour(long string_handle, word *buffer)
{
	short hour = g_54e7ce.wHour;
	short shown = hour;

	if (string_handle == 0xe43e && (word)hour > 12)
	{
		shown = hour - 12;
	}
	function_1630e0(buffer, (const word *)L"%d", (word)shown);
}

// @retail 0x22e179
void __stdcall parse_minute(long string_handle, word *buffer)
{
	function_1630e0(buffer, (const word *)L"%02d", g_54e7ce.wMinute);
}

// @retail 0x22e196
void __stdcall parse_day(long string_handle, word *buffer)
{
	function_1630e0(buffer, (const word *)L"%d", g_54e7ce.wDay);
}

// @retail 0x22e1b3
void __stdcall parse_month(long string_handle, word *buffer)
{
	function_1630e0(buffer, (const word *)L"%d", g_54e7ce.wMonth);
}

// @retail 0x22e1d0
void __stdcall parse_year(long string_handle, word *buffer)
{
	function_1630e0(buffer, (const word *)L"%d", g_54e7ce.wYear);
}

/* ---- the tables ---- */

const s_emoticon g_44a3f0[] =
{
	{ L":)", 0x263b },
	{ L":-)", 0x263b },
	{ L":o)", 0x263b },
	{ L":P", 0xe004 },
	{ L":-P", 0xe004 },
	{ L":oP", 0xe004 },
	{ L":(", 0x2639 },
	{ L":-(", 0x2639 },
	{ L":o(", 0x2639 },
	{ L":|", 0xe002 },
	{ L":-|", 0xe002 },
	{ L":o|", 0xe002 },
	{ L":D", 0xe001 },
	{ L":-D", 0xe001 },
	{ L":oD", 0xe001 },
	{ L";)", 0xe000 },
	{ L";-)", 0xe000 },
	{ L";o)", 0xe000 }
};

s_text_parse_entry g_470200[] =
{
	{ L"&", 0x26, 0 },
	{ L"a-button", 0xe100, 0 },
	{ L"b-button", 0xe101, 0 },
	{ L"x-button", 0xe102, 0 },
	{ L"y-button", 0xe103, 0 },
	{ L"black-button", 0xe104, 0 },
	{ L"white-button", 0xe105, 0 },
	{ L"left-trigger", 0xe106, 0 },
	{ L"right-trigger", 0xe107, 0 },
	{ L"dpad-up", 0xe108, 0 },
	{ L"dpad-down", 0xe109, 0 },
	{ L"dpad-left", 0xe10a, 0 },
	{ L"dpad-right", 0xe10b, 0 },
	{ L"start-button", 0xe10c, 0 },
	{ L"back-button", 0xe10d, 0 },
	{ L"left-thumb", 0xe10e, 0 },
	{ L"right-thumb", 0xe10f, 0 },
	{ L"left-stick", 0xe110, 0 },
	{ L"right-stick", 0xe111, 0 },
	{ L"mapname", 0xe408, parse_mapname },
	{ L"gametype", 0xe409, parse_gametype },
	{ L"variant", 0xe40a, parse_variant },
	{ L"hopper_name", 0xe43a, parse_hopper_name },
	{ L"hopper_description", 0xe43b, parse_hopper_description },
	{ L"motion_sensor_enabled", 0xe40b, parse_motion_sensor_enabled },
	{ L"teams_enabled", 0xe40c, parse_teams_enabled },
	{ L"vehicle_set", 0xe40d, parse_nothing },
	{ L"weapon_set", 0xe40e, parse_weapon_set },
	{ L"player0_profile_name", 0xe40f, parse_player_profile_name },
	{ L"player1_profile_name", 0xe410, parse_player_profile_name },
	{ L"player2_profile_name", 0xe411, parse_player_profile_name },
	{ L"player3_profile_name", 0xe412, parse_player_profile_name },
	{ L"player0_gamertag", 0xe429, parse_player_gamertag },
	{ L"player1_gamertag", 0xe42a, parse_player_gamertag },
	{ L"player2_gamertag", 0xe42b, parse_player_gamertag },
	{ L"player3_gamertag", 0xe42c, parse_player_gamertag },
	{ L"countdown", 0xe413, parse_countdown },
	{ L"name_of_last_person_to_delay_countdown", 0xe414, parse_name_of_last_person_to_delay_countdown },
	{ L"build_number", 0xe415, parse_nothing },
	{ L"leader", 0xe416, parse_leader },
	{ L"qprs", 0xe417, parse_qos },
	{ L"qprr", 0xe418, parse_qos },
	{ L"qpimi", 0xe419, parse_qos },
	{ L"qpime", 0xe41a, parse_qos },
	{ L"qbwd", 0xe41b, parse_qos },
	{ L"qbwu", 0xe41c, parse_qos },
	{ L"qmaxm", 0xe41d, parse_qos },
	{ L"qebw", 0xe432, parse_qos },
	{ L"qnat", 0xe41e, parse_qos },
	{ L"active_multiplayer_protocol", 0xe41f, parse_active_multiplayer_protocol },
	{ L"live_ui_driver_gamertag", 0xe420, parse_live_ui_driver_gamertag },
	{ L"live_ui_driver_clan_name", 0xe421, parse_live_ui_driver_clan_name },
	{ L"battle_rifle", 0xe112, 0 },
	{ L"bomb", 0xe113, 0 },
	{ L"brute_shot", 0xe114, 0 },
	{ L"carbine", 0xe115, 0 },
	{ L"flag", 0xe116, 0 },
	{ L"flak_cannon", 0xe117, 0 },
	{ L"flamethrower", 0xe118, 0 },
	{ L"fuel_rod", 0xe119, 0 },
	{ L"magnum", 0xe11a, 0 },
	{ L"needler", 0xe11b, 0 },
	{ L"oddball", 0xe11c, 0 },
	{ L"plasma_pistol", 0xe11d, 0 },
	{ L"plasma_rifle", 0xe11e, 0 },
	{ L"plasma_sword", 0xe11f, 0 },
	{ L"rocket_launcher", 0xe120, 0 },
	{ L"shotgun", 0xe121, 0 },
	{ L"smg", 0xe122, 0 },
	{ L"silenced_smg", 0xe12b, 0 },
	{ L"sniper", 0xe123, 0 },
	{ L"beam_rifle", 0xe124, 0 },
	{ L"disintegrator", 0xe125, 0 },
	{ L"sentinel_beam_weapon", 0xe126, 0 },
	{ L"sentinel_needle_weapon", 0xe127, 0 },
	{ L"sentinel_grenade_weapon", 0xe128, 0 },
	{ L"sentinel_charge_weapon", 0xe129, 0 },
	{ L"brute_plasma_rifle", 0xe12a, 0 },
	{ L"hud_quantity", 0xe422, parse_hud_quantity },
	{ L"hud_betraying_player", 0xe458, parse_hud_betraying_player },
	{ L"ge_round_time_left", 0xe423, parse_ge_round_time_left },
	{ L"target_player_gamertag", 0xe424, parse_target_player_gamertag },
	{ L"game_player_count", 0xe425, parse_game_player_count },
	{ L"coop_level_name", 0xe426, parse_coop_level_name },
	{ L"coop_level_description", 0xe427, parse_coop_level_description },
	{ L"coop_difficulty", 0xe428, parse_coop_difficulty },
	{ L"hud_scoreboard_player_score", 0xe42d, parse_hud_scoreboard },
	{ L"hud_scoreboard_other_player_score", 0xe42e, parse_hud_scoreboard },
	{ L"hud_scoreboard_variant_name", 0xe42f, parse_hud_scoreboard },
	{ L"hud_scoreboard_time_left", 0xe430, parse_hud_scoreboard },
	{ L"edit_player_profile_name", 0xe431, parse_player_profile_name },
	{ L"player_profile_blocks", 0xe433, parse_blocks },
	{ L"game_variant_blocks", 0xe434, parse_blocks },
	{ L"playlist_blocks", 0xe435, parse_blocks },
	{ L"saved_film_blocks", 0xe436, parse_blocks },
	{ L"target_player_clan_name", 0xe437, parse_target_player_clan_name },
	{ L"target_player_game_title_name", 0xe438, parse_target_player_game_title_name },
	{ L"target_player_clan_level", 0xe43c, parse_target_player_clan_level },
	{ L"live_ui_driver_clan_level", 0xe43d, parse_live_ui_driver_clan_level },
	{ L"hour_12", 0xe43e, parse_hour },
	{ L"hour_24", 0xe43f, parse_hour },
	{ L"minute", 0xe440, parse_minute },
	{ L"day", 0xe441, parse_day },
	{ L"month", 0xe442, parse_month },
	{ L"year", 0xe443, parse_year },
	{ L"saved_game_size", 0xe444, parse_blocks },
	{ L"total_minimal_storage_blocks", 0xe445, parse_blocks },
	{ L"button_jump", 0xe446, parse_button },
	{ L"button_switch_grenade", 0xe447, parse_button },
	{ L"button_action_reload", 0xe448, parse_button },
	{ L"button_switch_weapon", 0xe449, parse_button },
	{ L"button_melee_attack", 0xe44a, parse_button },
	{ L"button_flashlight", 0xe44b, parse_button },
	{ L"button_throw_grenade", 0xe44c, parse_button },
	{ L"button_fire", 0xe44d, parse_button },
	{ L"button_start", 0xe44e, parse_button },
	{ L"button_back", 0xe44f, parse_button },
	{ L"button_crouch", 0xe450, parse_button },
	{ L"button_scope_zoom", 0xe451, parse_button },
	{ L"button_lean_left", 0xe452, parse_button },
	{ L"button_lean_right", 0xe453, parse_button },
	{ L"button_accept", 0xe454, parse_button },
	{ L"button_cancel", 0xe455, parse_button },
	{ L"stick_move", 0xe456, parse_stick },
	{ L"stick_look", 0xe457, parse_stick },
	{ L"animating_left_thumbstick", 0xe459, parse_animating_thumbstick },
	{ L"animating_right_thumbstick", 0xe45a, parse_animating_thumbstick },
	{ L"matchmaking_countdown", 0xe45b, parse_matchmaking_countdown },
	{ L"new-content", 0xe45c, parse_new_content },
	{ L"map_load_percent", 0xe45d, parse_map_load_percent },
	{ L"button_dual_fire", 0xe45e, parse_button }
};

/* fills in the string's special characters and emoticons, as long as the
   string stays shorter than the maximum length */
// @retail 0x22d2ee
void function_22d2ee(word *string, long maximum_length)
{
	word *character = string;
	long length = wcslen(string);

	while (*character)
	{
		word replacement[k_parse_buffer_length];
		long replaced_length;
		long c = *character;
		long i;

		replacement[0] = 0;
		replaced_length = 0;
		if (c >= 0xe000 && c <= 0xf8ff)
		{
			for (i = 0; i < NUMBEROF(g_470200); i++)
			{
				if (c == g_470200[i].character)
				{
					if (g_470200[i].proc)
					{
						replacement[0] = 0;
						g_470200[i].proc(c, replacement);
						replaced_length = 1;
						goto replace;
					}
					break;
				}
			}
		}
		for (i = 0; i < NUMBEROF(g_44a3f0); i++)
		{
			long text_length = wcslen(g_44a3f0[i].text);
			wchar_t const *text = g_44a3f0[i].text;

			if (!wcsncmp(text, character, text_length))
			{
				word glyph[2];

				glyph[0] = (word)g_44a3f0[i].character;
				glyph[1] = 0;
				wcsncpy(replacement, glyph, k_parse_buffer_length - 1);
				replacement[k_parse_buffer_length - 1] = 0;
				replaced_length = text_length;
			}
		}
replace:
		{
			long replacement_length = wcslen(replacement);

			if (replacement_length - replaced_length + length < maximum_length)
			{
				memmove(character + replacement_length, character + replaced_length, (length - (character - string)) * sizeof(word));
				memcpy(character, replacement, replacement_length * sizeof(word));
				string[replacement_length - replaced_length + length] = 0;
				length = replacement_length - replaced_length + length;
			}
		}
		character++;
	}
}

/* parses a string in place */
// @retail 0x22d2b7
void parse_text(word *string)
{
	word buffer[k_parse_buffer_length];

	unicode_string_copy(buffer, string, k_parse_buffer_length);
	function_22d2ee(buffer, k_parse_buffer_length);
	parse_copy(string, buffer);
}
