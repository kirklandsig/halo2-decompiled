// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0C86E0.CPP: a unit leaving its weapon's zoom. Lane S
   wrote it for the weapon functions 0x103b10..0x103ce0, which call it. */

#include "unknown_11c920.h"
#include "globals.h"

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
bool function_100f70(long weapon_index);
long function_1896c0(real scale, long tag_index);

/* the unit (a view of the object data) */
struct s_zoom_unit
{
	byte unknown000[0x13c];
	long player_index;
	byte unknown140[0x212 - 0x140];
	char current_weapon_slot;
	byte unknown213[0x218 - 0x213];
	long weapon_indices[4];
	byte unknown228[0x240 - 0x228];
	char field_240;
	char desired_zoom_level;
	byte unknown242[0x26c - 0x242];
	real zoom_transition;
};

struct s_zoom_object_header
{
	byte unknown00[8];
	s_zoom_unit *unit;
};

/* the weapon's definition: its zoom-out sound */
struct s_zoom_weapon_definition
{
	byte unknown000[0x278];
	long zoom_out_sound_index;
};

/* the player (0x21c bytes): its local user */
struct s_zoom_player
{
	byte unknown00[0x28];
	short user_index;
};

/* the globals' sound tags */
struct s_zoom_sounds
{
	byte unknown00[0xd0];
	long zoom_out_sound_index;
};

struct s_zoom_globals
{
	byte unknown000[0x134];
	s_zoom_sounds *sounds;
};

#define ZOOM_UNIT_GET(index) (((s_zoom_object_header *)g_4e0300->data)[(index) & 0xffff].unit)

static inline long unit_get_player_index(long unit_index)
{
	s_zoom_unit *unit = (s_zoom_unit *)function_badc0(unit_index, 3);
	long result = NONE;

	if (unit)
		result = unit->player_index;
	return result;
}

static inline long unit_get_current_weapon(s_zoom_unit *unit)
{
	short slot = unit->current_weapon_slot;
	long result = NONE;

	if (slot != NONE)
		result = unit->weapon_indices[slot];
	return result;
}

static __forceinline long weapon_get_zoom_out_sound(long weapon_index)
{
	long result = NONE;

	if (weapon_index != NONE)
	{
		s_zoom_unit *weapon = ZOOM_UNIT_GET(weapon_index);
		result = ((s_zoom_weapon_definition *)g_4e3b44[*(long *)weapon & 0xffff].bytes)->zoom_out_sound_index;
	}
	return result;
}
// score 107 (was 107): Frameless entry saves, fresh unit lookup and late prior-state carrier remain.

// Earlier: this round: the fast check reads 107 at 362 bytes against retail's 376 again
// and no new form was tried; the difference is still blocked by 0xbadc0's memory-effect knowledge in
// src/unknown_0bad50.cpp, which makes our link keep the entry fetch.
// Earlier: this round: no new form. Still BLOCKED-BY 0xbadc0 (held file): retail
// re-fetches the unit after its two 0xbadc0 calls, our link proves the callee writes no memory
// and keeps the entry fetch, so every later register home shifts.
// Earlier: frameless entry saves, fresh unit reload and late prior-state carrier differ.
// frameless entry saves, unit reload and sentinel carriers differ.
// frameless entry saves, fresh unit reload and late prior-state carrier differ.
// Earlier: frameless entry saves, unit reload and sentinel carriers differ.
// entry saves, fresh unit reload and sentinel carriers still differ.
// frameless entry saves, unit reload and sentinel carriers differ.
// Native player-table stride and byte eligibility/prior-state combinations 131
// each (seven forms); full-width eligibility 127 (four-point gain rejected).
// Original record view, result types, observations and body restored.
// entry saves, fresh unit reload and sentinel carriers still differ.
// BLOCKED-BY 0xbadc0: retail re-fetches the unit after its two calls, ours reuses the entry
// fetch (our link knows the callee writes no memory).
// shared weapon producer 131; user-result arms 125; both 125; signed-word user 128;
// shared producer/word 128; word/result arms 122; all three 122; guarded weapon
// result 135. Retain signed-word user/result arms; the six-count arm-only gain is rejected.
// Unit fetched after the player queries 118, with a long user index 121; was_zoomed
// declared first 107 (it then lives in bl from entry, retail dl: positional only), with the
// late fetch and long user 109 (376 bytes, flag spilled), with long user 110; zoom-out
// call in each sound arm 124; weapon read through the entry unit 122.
// Retail re-fetches the unit after the two 0xbadc0 calls (index * 12 kept in esi); our link
// knows 0xbadc0 writes no memory and reuses the entry pointer. With 0xbadc0 opaque (one-file
// LTCG link) this body with a long user index gives retail's code apart from the callees'
// conventions. Existing 0x14de90 getter 122; plus 0xcbd50 for the weapon 122; long user
// index 125, with both getters 125; unit fetched at its first use 106 (376 bytes, but it
// moves retail's entry fetch after the calls).
// Round 26 (lane 9), from 122: zoom flag declared first 107 (kept: the entry fetch stays; the flag
// sits in bl from entry where retail sets dl late, but ebx no longer holds a -1 and the sentinels
// compare as immediates as in retail); plus long user index 110; weapon flag as if/else 126; user
// index as one ternary 128; zoom test as !(keep && zoomed) 122; reset entry written in full 124.
// On the 107 body: long user index 111; reset entry in full 109; weapon flag as if/else 111; zoom
// test as !(keep && zoomed) 107; sound index as one ternary 107.
// BLOCKED-BY 0xbadc0 (the re-fetch after its calls).
// Round 57: force-inline current-weapon helper 107; both helpers force-inline 107; user ordinal
// before the zoom flag 107; zoom flag after the entry unit 107; after the player queries 120;
// after the weapon lookup 120; weapon index declared early 107; sound index declared early 107.
// Round 202: Prior-state long/short/byte storage, eligibility long/short/byte storage, and both long/both short 106/105/107/103/97/107/102/96 fast.
// Short eligibility 97/376 plain; both short 96/376 fast add word tests and padding; aligned instructions worsen.
// Original executable source restored. Entry register saves, the fresh unit lookup and sentinel/result homes remain.
// Round 183: one reset/sound domain, separate/merged user guards 126/126; native prior-level
// snapshots 127/127; final sound-tag result 126/126; those result forms with paired reset
// stores chained 126/126. Original query graph, bool lifetimes and complete body restored.
// Player early/conditional returns 113/113; weapon-slot early/conditional returns 100/107;
// sound early/conditional returns 110/110; all producers early/conditional 105/114. The
// seven-point slot gain is below the required eight; original producers restored.
// Round 267: masked slot/header record/pool descriptor/header table 107/119/107/117;
// descriptor with byte/long slot 111/114; preserved-state return 126, with header 129.
// Original producers and complete executable body restored.
// Round 270: player/weapon/both sentinel declarations first 119/107/119;
// native signed-byte slot alone/with player/with weapon/with both 111/123/111/123;
// both sentinel declarations with consistent direct reset-unit observations 150.
// Original producers and complete executable body restored; fresh lookup remains.
// Round 76: late false 122; result arms 121; late false with long user 125; late fresh unit 122;
// original fresh unit 107; direct reset lookups 175; false after entry 107; false before weapon 120.
// Original body restored.
// Earlier: this round: no new form. Still BLOCKED-BY 0xbadc0 (src/unknown_0bad50.cpp):
// retail re-fetches the unit after the two calls because its compiler treats 0xbadc0 as writing
// memory, while our link knows it writes none, so the unit and the flag registers are homed
// differently and the entry prologue differs.
// Round 332: Retained absolute header ordinal long/dword/word/int, each with entry or late false
// result: 107/107/109/107/122/122/124/122.
// Original executable source restored; no eligible gain.
// @retail 0xc86e0
void function_c86e0(long unit_index, bool keep_weapon_zoom)
{
	bool was_zoomed = false;
	s_zoom_unit *unit = ZOOM_UNIT_GET(unit_index);
	short user_index;
	if (unit_get_player_index(unit_index) == NONE)
		user_index = NONE;
	else
		user_index = ((s_zoom_player *)(g_4e8c24->data + (unit_get_player_index(unit_index) & 0xffff) * 0x21c))->user_index;
	long weapon_index = unit_get_current_weapon(ZOOM_UNIT_GET(unit_index));
	bool weapon_zoomed = weapon_index != NONE ? function_100f70(weapon_index) : false;

	if (!keep_weapon_zoom || !weapon_zoomed)
	{
		was_zoomed = unit->field_240 != NONE;
		unit->field_240 = NONE;
		unit->desired_zoom_level = NONE;
		unit->zoom_transition = 0.0f;
		if (user_index != NONE)
		{
			s_unknown_185ab0_entry *entry = &g_4ed284->entries[user_index];

			entry->index = NONE;
			entry->flag = false;
		}
	}
	if (user_index != NONE && was_zoomed)
	{
		long sound_index;

		if (weapon_zoomed && !keep_weapon_zoom)
			sound_index = weapon_get_zoom_out_sound(weapon_index);
		else
			sound_index = ((s_zoom_globals *)g_4e034c)->sounds->zoom_out_sound_index;
		if (sound_index != NONE)
			function_1896c0(1.0f, sound_index);
	}
}
