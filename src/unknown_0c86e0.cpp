// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0C86E0.CPP: a unit leaving its weapon's zoom. Lane S
   wrote it for the weapon functions 0x103b10..0x103ce0, which call it. */

#include "unknown_11c920.h"
#include "globals.h"

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
bool function_100f70(long weapon_index);
long function_1896c0(real scale, long tag_index);
long unit_get_player_index(long unit_index);

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

static __forceinline long weapon_get_zoom_out_sound(long weapon_index)
{
	if (weapon_index == NONE)
		return NONE;

	s_zoom_unit *weapon = ZOOM_UNIT_GET(weapon_index);

	return ((s_zoom_weapon_definition *)g_4e3b44[*(long *)weapon & 0xffff].bytes)->zoom_out_sound_index;
}
// Matched source: the unit is fetched at the entry through the file's macro; the weapon lookup fetches it again after
// the two player-index lookups through a copy of the object pool bound to a reference (the idiom src/unknown_0a76b0.cpp's
// 0xe8c10 uses), so the pool, its data and the unit are read again as retail does, with the data pointer as the address
// base; the current weapon is read from that second fetch in place (slot as a short, NONE when the slot is empty) rather
// than through 0xcbd50. was_zoomed is declared after the weapon query, the weapon flag is
// `weapon_index == NONE ? false : function_100f70(weapon_index)`, and the zoom-out sound helper returns early.
// @retail 0xc86e0
void function_c86e0(long unit_index, bool keep_weapon_zoom)
{
	s_zoom_unit *unit = ZOOM_UNIT_GET(unit_index);
	long user_index;
	if (unit_get_player_index(unit_index) == NONE)
		user_index = NONE;
	else
		user_index = ((s_zoom_player *)(g_4e8c24->data + (unit_get_player_index(unit_index) & 0xffff) * 0x21c))->user_index;
	s_record_pool *objects = g_4e0300;
	s_record_pool *&pool = objects;
	s_zoom_unit *weapon_unit = ((s_zoom_object_header *)pool->data)[unit_index & 0xffff].unit;
	short slot_index = weapon_unit->current_weapon_slot;
	long weapon_index = NONE;
	if (slot_index != NONE)
		weapon_index = weapon_unit->weapon_indices[slot_index];
	bool weapon_zoomed = weapon_index == NONE ? false : function_100f70(weapon_index);
	bool was_zoomed = false;

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
