// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_18D290.CPP: marking the object looping sounds (g_4ed28c) that play
   a sound tag */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_0259d0.h"

bool function_18d360(long tag_index);

/* an object looping sound (g_4ed28c, 0x18 bytes) */
struct s_looping_sound_datum
{
	short salt;
	byte unknown02;
	byte state;
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word flag3 : 1;
	word flag4 : 1;
	word flag5 : 1;
	word flag6 : 1;
	word flag7 : 1;
	word unknown04 : 8;
	byte unknown06[6];
	long tag_index;
	byte unknown10[8];
};

long __stdcall function_18d1c0(long tag_index);

static inline s_looping_sound_datum *looping_sound_get(long index)
{
	return (s_looping_sound_datum *)g_4ed28c->data + (index & 0xffff);
}

/* moves the tag's bit-5 mark from the looping sound found for it to the given one, when
   that one plays the tag (retail's callers load the index register before the tag's) */
// @retail 0x18d290
void function_18d290(long tag_index, long looping_sound_index)
{
	long index = function_18d1c0(tag_index);

	if (index != NONE)
	{
		s_looping_sound_datum *looping_sound = looping_sound_get(index);
		if (looping_sound->tag_index == tag_index)
		{
			looping_sound->flag5 = false;
		}
	}
	if (looping_sound_index != NONE)
	{
		s_looping_sound_datum *looping_sound = looping_sound_get(looping_sound_index);
		if (looping_sound->tag_index == tag_index)
		{
			looping_sound->flag5 = true;
		}
	}
}

// @retail 0x18d2e0
void function_18d2e0(long tag_index, long mode)
{
	if (tag_index != NONE)
	{
		long index = function_18d1c0(tag_index);
		if (index != NONE)
		{
			s_looping_sound_datum *looping_sound = looping_sound_get(index);
			looping_sound->flag5 = false;
			looping_sound_get(index)->flag1 = true;

			long other_index = function_18d1c0(tag_index);
			if (other_index != NONE)
			{
				s_looping_sound_datum *other = looping_sound_get(other_index);
				if (other->tag_index == tag_index)
				{
					other->flag5 = false;
				}
			}

			switch (mode)
			{
			case 1:
				looping_sound->flag2 = true;
				break;
			case 2:
				looping_sound->flag3 = true;
				break;
			}
		}
	}
}

// @retail 0x18d3d0
void function_18d3d0(void)
{
	s_record_pool *array = g_4ed28c;
	long datum = data_datum_index(array, function_16bc00(array, 0));

	while (datum != NONE)
	{
		s_looping_sound_datum *looping_sound = (s_looping_sound_datum *)array->data + (datum & 0xffff);

		if (looping_sound->state == 0 || looping_sound->state == 4)
		{
			long tag_index = looping_sound->tag_index;
			if (function_18d360(tag_index))
			{
				function_18d2e0(tag_index, 1);
			}
		}

		datum = data_datum_index(array, data_find_index(array, datum == NONE ? 0 : (datum & 0xffff) + 1));
	}
}

/* the players (g_4e8c24, 0x21c bytes) */
struct s_player_view
{
	byte unknown00[0x2c];
	long unit_index;
	byte unknown30[0x21c - 0x30];
};

long function_155760(long index);
bool function_bafb0(long object_index, long ancestor_index);

bool function_14ddc0(long local_player_index);

// score 44 (was 44): this round: the listings were re-read only; no new shape of the existence
// flag was found. Retail keeps the existence byte in BL across the 0x155760 call and re-tests it
// after the split mask, our compiler folds the re-test away and keeps the flag in AL; the loop
// index is in EBX where retail uses EDI and the unit in EDI where retail uses ESI.
// Earlier at 44 (from 44): this round: the existence test nested around the split test with the mask
// kept as a number gives 52 (188 bytes, retail's length, and the split test becomes retail's
// `test dl, 3`), the same with the existence tested on its own 52 (the compiler still folds the
// re-test away and keeps the flag in AL), and the mask kept as a number without the nesting 72
// (192 bytes). Still the existence byte in AL where retail keeps it in BL and re-tests it after
// the split mask, the loop index in EBX where retail uses EDI and the unit in EDI where retail
// uses ESI.
// Earlier at 44 (from 44): this round's further trials on the split mask and the existence flag: the
// mask kept in a long local 92 (204 bytes) and the same with the mask test as its own statement
// 95 (206); the mask test written into the condition 52 (188 bytes, retail's length, but the
// existence re-test is folded away); the existence test as a second call to 0x14ddc0 89 (203
// bytes), the same with the player index in a local 80 (204) and with the mask in a local 80;
// the second call nested inside the mask branch 89 (203), the same with the mask in a local 89
// and with the object parameter in a local 89 (203). None of them lowers the score.
// Earlier at 44 (from 44): the existence test written as a second helper call, so that the flag is
// kept across the 0x155760 call and tested again after the split mask, reproduces retail's
// `setne bl` and its second `test bl, bl`, but the loop index then takes EBP, the table
// pointer is spilled and the body grows to 203 bytes (89 against 44 at 191); the nested, the
// single-condition and the unit-ternary forms of the same idea give the same 89 at 203 bytes,
// and reading the unit through 0x14de70 changes nothing. Still the existence byte in AL where
// retail keeps it in BL, the loop index in EBX where retail uses EDI and the unit in EDI where
// retail uses ESI.
// Earlier at 44 (from 44): this round: the split mask as a long value with the existence test kept
// separate gave 92 (the flat shape loses retail's nested branches); the mask test written into the
// condition 52 (188 bytes, retail's length) and the same with the existence re-tested after the
// call 52 (the compiler folds the redundant test away); the object parameter copied into a local
// 44 (same code); the entry read directly instead of through 0x14ddc0 59 (loses retail's `cmp -1`);
// a local entries pointer before the loop 93; a while loop 244. Still the existence byte in
// AL where retail keeps it in BL and re-tests it, with the loop index in EBX and unit in EDI.
// This round, again: a permuter run was stopped during its baseline build (no result). Retail keeps
// the existence byte in BL across the 0x155760 call and re-tests it after the split mask, which
// frees EBX for the object argument and leaves ESI for the unit; our compiler folds the re-test
// away, keeps the flag in AL and reloads the object from the stack.
// Round 19: unchanged; only the helper-call chain reproduces retail's BL re-test (89 at 203 bytes:
// the table pointer spills and the index takes ebp) and every cached-entry form folds the re-test
// away (72-80); the existence byte and the loop/unit registers remain.
// Earlier at 44 (from 44): BLOCKED-BY 0xbafb0. The existence byte (retail BL, tested again after the
// MiniMax R (11-01): no new form, no lower score; retail keeps the object argument in EBX across
// both 0xbafb0 calls and uses EDI as the loop counter, while our counter takes EBX and the object
// is reloaded from the stack for each call, so the two argument orders cannot both be reproduced.
// score 44 (was 44): BLOCKED-BY 0xbafb0. The existence byte (retail BL, tested again after the
// split test) and the loop/unit registers differ. `if (function_14ddc0(i) && split)` with the unit read
// as `function_14ddc0(i) ? player(function_14de70(i))->unit_index : NONE` gives retail's two existence
// tests (89 alone: the second hierarchy call sets esi before eax, so the unit leaves esi and the
// table pointer spills); with 0xbafb0's parameters reordered to (ancestor, object) at its definition
// and both calls it matches, with 0xbafb0 still matched.
// Tried: index-accessor predicate 67; shared player index 65; short index 61; unsigned index 44;
// relationship result/guards 44/44; immutable existence 44; audible guard 44; inline/noinline
// controller 44/44; inline/noinline hierarchy 44/44; force-inline existence 44; controller range 58;
// membership 59; success break 44; eligibility input, eligibility guard, saved existence output,
// conditional merge 44 each, the same with the player accessor 44 each; long/word/byte split masks
// 72/59/69; char/long existence with mask 72/83; existing player accessor 44; related-player result
// 86; shared emitter index 44. Player-index accessor then NONE test 42; if-form unit 89; nested unit
// test 89; unit declared first 89; short index 122; dword index 89; reused index/unit variable 89;
// inline unit getter 89. Round 5: `exists && split` order 44; unit read as `exists ? ... : NONE`
// under `if (split)` 44; object read through its own address at the second call 44; split as one
// `exists && ...` expression 52 (188 bytes); split declared before exists 44.
// Round 113: `exists && split` 44; split declared first 44; shared local_player_exists 44; it
// twice with a split flag 96 (retail's BL re-test, split materialised); split test plus a
// local_player_exists unit ternary 89. All restored.
// Round 116: the unit read through 0x14de70 44; `if (split && function_14ddc0(i))` 96 (208 bytes),
// with the 0x14de70 read too 96; split as `exists ? ... : false` 44; both flags at loop scope 44.
// Round 30 (0xbafb0's file is on another lane's list, so its parameters stay): two existence
// tests with the player getter in the unit ternary 89 (our second call sets ESI before EAX, so
// the unit cannot stay in ESI); player getter then NONE-test ternary 42; that with the unit test
// nested 42; the first form with the hierarchy test nested 89; `exists && split` 44. Restored.
// Round 118: `continue` on each failed test 52 (188 bytes, aligned as before: the split test is no
// longer materialised, but the BL re-test is missing); nested tests 52; nested with the unit read as
// `exists ? ... : NONE` 52; the unit through a player-datum pointer 44; the existence call repeated
// in the test 96 (208 bytes). 0x18d4f0 is 0xbafb0's only caller.
// Round 69 (m3 lane 9, fast with 0xbafb0): `exists && split` with the unit read as `exists ? ... : NONE` 52
// (188 bytes; the re-test folds away); the same through 0x14de70 52; the 0x14de70 player index tested
// against NONE in place of the existence flag 57; the unit read through 0x14de70 on the current form 44;
// the second 0x14ddc0 call with the unit ternary 89 (203 bytes: retail's BL re-test, the unit leaves ESI).
// Still BLOCKED-BY 0xbafb0: its parameter order must change at its definition (src/unknown_0bad50.cpp, a
// machine 1 and contributor file), which earlier notes found matches this function with 0xbafb0 still matched.
// @retail 0x18d4f0
void function_18d4f0(long object_index, char *audible, long *local_player_index)
{
	if (local_player_index)
	{
		*local_player_index = NONE;
	}

	if (*audible == 1)
	{
		for (long i = 0; i < 4; i++)
		{
			bool exists = function_14ddc0(i);
			bool split = false;
			if (exists)
			{
				split = ((1 << function_155760(i)) & 3) != 0;
			}
			if (split && exists)
			{
				long unit_index = ((s_player_view *)g_4e8c24->data)[g_4e8c20->entries[i] & 0xffff].unit_index;
				if (unit_index != NONE && (function_bafb0(object_index, unit_index) || function_bafb0(unit_index, object_index)))
				{
					if (local_player_index)
					{
						*local_player_index = i;
					}
					*audible = 0;
					return;
				}
			}
		}
	}
}

struct s_sound_definition_flags
{
	byte flags;
};

// @retail 0x18d4b0
char function_18d4b0(long tag_index, char audible, long object_index, long *local_player_index)
{
	s_sound_definition_flags *sound = (s_sound_definition_flags *)g_4e3b44[tag_index & 0xffff].bytes;
	char result = audible;

	if (!(sound->flags & 4))
	{
		function_18d4f0(object_index, &result, local_player_index);
	}
	return result;
}

/* the local player cameras (g_4e6380) */
struct s_4e6380;
extern s_4e6380 *g_4e6380;

struct s_local_camera
{
	byte unknown00[0xe];
	bool active;
	byte unknown0f[0x38 - 0xf];
	point3f position;
	byte unknown44[0x48 - 0x44];
};

struct s_local_cameras
{
	byte unknown00[0x80];
	s_local_camera cameras[4];
};

static inline real distance_sq3f(point3f const *a, point3f const *b)
{
	vector3f vector;

	vector3d_from_points3d(a, b, &vector);
	return length_sq3f(&vector);
}

// @retail 0x18d670
bool function_18d670(point3f const *point, real radius)
{
	bool result = false;
	real radius_squared = radius * radius;

	for (long i = 0; i < 4; i++)
	{
		s_local_camera *camera = &((s_local_cameras *)g_4e6380)->cameras[i];
		if (camera->active)
		{
			if (radius_squared >= distance_sq3f(point, &camera->position))
			{
				result = true;
				break;
			}
		}
	}
	return result;
}

struct s_globals_sound_view
{
	byte unknown00[0x54];
	real value54;
};

struct s_tag_header_alt_view
{
	byte unknown00[0x14];
	long sound_globals_tag_index;
};

struct s_tag_header_globals_alt_view
{
	byte unknown00[0xc0];
	void *header;
	s_tag_header_alt_view *header_alt;
};

// @retail 0x18d6e0
real function_18d6e0(void)
{
	real result = 4.0f;
	s_tag_header_globals_alt_view *globals = (s_tag_header_globals_alt_view *)g_4e034c;
	s_tag_header_alt_view *header = globals->header ? globals->header_alt : NULL;
	long tag_index = header->sound_globals_tag_index;

	if (tag_index != NONE)
	{
		result = ((s_globals_sound_view *)g_4e3b44[tag_index & 0xffff].bytes)->value54;
	}
	return result;
}
