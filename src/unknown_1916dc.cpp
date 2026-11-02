#include "unknown_11c920.h"
#include "camera_storage.h"
#include "globals.h"
#include "object_markers.h"
#include "data_array.h"

// @flags /O1 /arch:SSE /Gr
#include "projection_storage.h"
#include "view_context_storage.h"

/* UNKNOWN_1916DC.CPP: the messages the HUD shows about the player's weapons */

struct s_type_2d4969
{
	byte unknown00[4];
	real age;
	byte unknown08[0x24 - 0x08];
	short total_rounds;
	byte unknown26[2];
	short loaded_rounds;
	short magazines;
	short charge;
};

// @retail 0x1916dc
bool function_1916dc(s_type_2d4969 const *state)
{
	bool depleted = false;

	if (state->total_rounds > 0 && state->magazines != 0 && state->loaded_rounds == 0 && state->charge == 0)
		depleted = true;

	if (state->age == 1.0f)
		depleted = true;

	return depleted;
}

/* the messages of an item definition (a weapon's pickup messages) */
struct s_item_message_definition
{
	byte unknown00[0xe0];
	long message;
	long singular_message;
	long plural_message;
};

void function_13925f(long string_handle, word *buffer);
void function_24caac(long player_index, word const *text, word const *plural_text, long count);
void function_24cb54(long player_index, word const *text, word const *plural_text);
void function_24cbbf(long player_index, long string_handle);

/* shows the item's message for one of it */
// @retail 0x191e51
void __stdcall function_191e51(long definition_index, long player_index)
{
	if (player_index != NONE)
	{
		s_item_message_definition *definition = (s_item_message_definition *)g_4e3b44[definition_index & 0xffff].bytes;
		word text[0x100];
		word plural_text[0x100];

		text[0] = 0;
		plural_text[0] = 0;
		function_13925f(definition->singular_message, text);
		function_13925f(definition->plural_message, plural_text);
		function_24caac(player_index, text, plural_text, 1);
	}
}

/* shows the item's message for a count of it */
// @retail 0x191ec4
void __stdcall function_191ec4(long definition_index, long player_index, short count)
{
	if (player_index != NONE)
	{
		s_item_message_definition *definition = (s_item_message_definition *)g_4e3b44[definition_index & 0xffff].bytes;
		word text[0x100];
		word plural_text[0x100];

		text[0] = 0;
		plural_text[0] = 0;
		function_13925f(definition->singular_message, text);
		function_13925f(definition->plural_message, plural_text);
		function_24caac(player_index, text, plural_text, count);
	}
}

// @retail 0x191f3a
void __stdcall function_191f3a(long definition_index, long player_index)
{
	if (player_index != NONE)
	{
		s_item_message_definition *definition = (s_item_message_definition *)g_4e3b44[definition_index & 0xffff].bytes;
		word text[0x100];
		word plural_text[0x100];

		text[0] = 0;
		plural_text[0] = 0;
		function_13925f(definition->singular_message, text);
		function_13925f(definition->plural_message, plural_text);
		function_24cb54(player_index, text, plural_text);
	}
}

// @retail 0x191fab
void __stdcall function_191fab(long definition_index, long player_index)
{
	if (player_index != NONE)
	{
		s_item_message_definition *definition = (s_item_message_definition *)g_4e3b44[definition_index & 0xffff].bytes;

		function_24cbbf(player_index, definition->message);
	}
}


struct s_target_candidate
{
    short priority;
    byte flags;
    byte unknown03;
    long object_index;
};
struct s_weapon_status;
struct s_object;
long function_14de70(long user_index);
long function_cbd50(long unit_index, short slot);
short function_ce020(long unit_index);
short function_cdff0(long unit_index, short grenade_type);
void function_100520(long weapon_index, s_weapon_status *status);
bool function_1518f0(s_target_candidate *candidate, dword type_mask);
s_object *function_badc0(long object_index, dword type_mask);
long function_11ba10(long control_index);
void function_24c98c(long index, bool flag);
void function_24ca1d(long player_index, word const *text, long sound, long tag_index);
void function_24c9e9(long player_index, long string_handle, long sound, long tag_index);

struct s_object_header_19188c { byte unknown00[8]; byte *object; };
static __forceinline byte *object_19188c(long index)
{
    return ((s_object_header_19188c *)g_4e0300->data)[index & 0xffff].object;
}
static __forceinline byte *definition_19188c(long index)
{
    return g_4e3b44[*(long *)object_19188c(index) & 0xffff].bytes;
}

// @retail 0x19188c
void function_19188c(long user_index)
{
    long const &player_index = function_14de70(user_index);
    byte *player = g_4e8c24->data + (player_index & 0xffff) * 0x21c;
    short state = *(short *)((byte *)g_4e8c20 + 0x9c);
    s_target_candidate *candidate = (s_target_candidate *)((byte *)g_4ed284 + user_index * 0x94 + 0x80);
    if (state != 0 && *(long *)(player + 0x2c) == NONE)
    {
        switch (state)
        {
        case 4: function_24c9e9(user_index, 0x1a0006a6, 1, *(long *)((byte *)g_510c94 + 0x42c)); break;
        case 5: function_24c9e9(user_index, 0x1a0006a7, 2, *(long *)((byte *)g_510c94 + 0x42c)); break;
        case 6: function_24c9e9(user_index, 0x1a0006a8, 4, *(long *)((byte *)g_510c94 + 0x42c)); break;
        case 7: function_24c9e9(user_index, 0x1b0006a9, 8, *(long *)((byte *)g_510c94 + 0x42c)); break;
        }
        return;
    }
    if (candidate->priority == 0)
    {
        long unit_index = *(long *)(player + 0x2c);
        if (unit_index == NONE)
        {
            function_24c98c(user_index, false);
        }
        else
        {
            long primary = function_cbd50(*(long *)(player + 0x2c), *(char *)(object_19188c(*(long *)(player + 0x2c)) + 0x212));
            long secondary = function_cbd50(*(long *)(player + 0x2c), *(char *)(object_19188c(*(long *)(player + 0x2c)) + 0x213));
            long message = 0;
            byte *unit = object_19188c(unit_index);
            bool allow_weapons = true;
            if (*(long *)(unit + 0x14) != NONE && *(short *)(unit + 0x1fc) != NONE)
            {
                byte *seat = *(byte **)(definition_19188c(*(long *)(unit + 0x14)) + 0x1cc) + *(short *)(unit + 0x1fc) * 0xb0;
                dword flags = *(dword *)seat;
                allow_weapons = !((bool)((flags >> 3) & 1) || (bool)((flags >> 2) & 1));
                if ((bool)((flags >> 11) & 1))
                {
                    if (function_cdff0(unit_index, function_ce020(unit_index)) > 0 && *(long *)(seat + 0x10))
                    {
                        s_object_marker marker;
                        if (function_b8d30(*(long *)(unit + 0x14), *(long *)(seat + 0x10), &marker, 1, false) == 1)
                            message = *(long *)(seat + 0x14);
                    }
                    long seat_message = *(long *)(seat + 0x18);
                    if (message)
                    {
                        function_24c9e9(user_index, message, (*(long *)(unit + 0x14) & 0xffff) + 0x10000, *(long *)((byte *)g_510c94 + 0x434));
                        return;
                    }
                    if (seat_message)
                    {
                        function_24c9e9(user_index, seat_message, (*(long *)(unit + 0x14) & 0xffff) + 0x8000, *(long *)((byte *)g_510c94 + 0x434));
                        return;
                    }
                }
            }
            if (primary != NONE && allow_weapons)
            {
                byte status[0x3c];
                function_100520(primary, (s_weapon_status *)status);
                bool depleted = function_1916dc((s_type_2d4969 *)status);
                if (secondary != NONE)
                {
                    function_100520(secondary, (s_weapon_status *)status);
                    depleted = depleted && function_1916dc((s_type_2d4969 *)status);
                }
                if (depleted)
                {
                    for (long i = 0; i < 4; i++)
                    {
                        long weapon = ((long *)(unit + 0x218))[i];
                        if (weapon != NONE && weapon != primary && weapon != secondary)
                        {
                            function_100520(weapon, (s_weapon_status *)status);
                            if (!function_1916dc((s_type_2d4969 *)status))
                            {
                                function_24c9e9(user_index, *(long *)(definition_19188c(weapon) + 0xec), (weapon & 0xffff) + 0x10, *(long *)((byte *)g_510c94 + 0x434));
                                return;
                            }
                        }
                    }
                }
            }
            function_24ca1d(user_index, (word const *)L"", NONE, NONE);
            function_24c98c(user_index, false);
        }
        return;
    }
    switch (candidate->priority)
    {
    case 8:
        if (function_1518f0(candidate, 2))
        {
            long object_index = candidate->object_index;
            function_24c9e9(user_index, *(long *)(definition_19188c(object_index) + 0x21c),
                (object_index & 0xffff) + 0x1000, *(long *)((byte *)g_510c94 + 0x42c));
        }
        break;
    case 4:
    case 5:
    case 6:
        if (function_1518f0(candidate, 3))
        {
            long object_index = candidate->object_index;
            byte *definition = definition_19188c(object_index);
            long seat = *(short *)((byte *)candidate + 2);
            long message = 0;
            if (seat >= 0 && seat < *(long *)(definition + 0x1c8))
                message = *(long *)(*(byte **)(definition + 0x1cc) + seat * 0xb0 + 0x84);
            function_24c9e9(user_index, message, (object_index & 0xffff) + 0x800, *(long *)((byte *)g_510c94 + 0x42c));
        }
        break;
    case 3:
        if (function_1518f0(candidate, 0x100))
        {
            long object_index = candidate->object_index;
            long message = function_11ba10(object_index);
            if (message)
                function_24c9e9(user_index, message, (object_index & 0xffff) + 0x400, *(long *)((byte *)g_510c94 + 0x42c));
        }
        break;
    case 2:
        if (function_badc0(candidate->object_index, 3))
        {
            long weapon = function_cbd50(candidate->object_index, *(char *)(object_19188c(candidate->object_index) + 0x212));
            if (weapon != NONE)
            {
                long message = *(long *)(definition_19188c(weapon) + 0xf0);
                if (message && message != NONE)
                    function_24c9e9(user_index, message, (candidate->object_index & 0xffff) + 0x4000, *(long *)((byte *)g_510c94 + 0x42c));
            }
        }
        break;
    case 1:
    case 7:
        if (function_1518f0(candidate, 4))
        {
            long object_index = candidate->object_index;
            dword flags = candidate->flags;
            byte *definition = definition_19188c(object_index);
            long message;
            long sound = object_index & 0xffff;
            long tag_index = NONE;
            if (!(flags & 12))
            {
                bool has_tag = true;
                if (flags & 1)
                {
                    message = *(long *)(definition + 0xcc);
                    sound += 0x100;
                }
                else if (flags & 2)
                {
                    message = *(long *)(definition + 0xd0);
                    sound += 0x200;
                }
                else
                {
                    message = *(long *)(definition + 0xcc);
                    has_tag = false;
                }
                if (has_tag)
                    tag_index = *(long *)((byte *)g_510c94 + 0x42c);
            }
            else
            {
                if (flags & 1)
                {
                    message = *(long *)(definition + 0xd4);
                    sound += 0x20;
                    tag_index = *(long *)((byte *)g_510c94 + 0x434);
                }
                else if (flags & 2)
                {
                    message = *(long *)(definition + 0xd8);
                    sound += 0x40;
                    tag_index = *(long *)((byte *)g_510c94 + 0x434);
                }
                else
                {
                    message = *(long *)(definition + 0xdc);
                    sound += 0x80;
                    tag_index = *(long *)((byte *)g_510c94 + 0x434);
                }
            }
            function_24c9e9(user_index, message, sound, tag_index);
        }
        break;
    }
}


void function_192129(long local_player_index);
struct s_view_globals;
extern s_view_globals *g_510c98;
bool function_159170(long player_index);
long function_155760(long local_player_index);
void function_24da7d(long local_player_index);
void function_24cf66(long local_player_index);
void function_13954b(void);
void function_191fd8(long player_index);

#include "unknown_030290.h"
struct s_bitmap_view;
struct D3DTexture;
struct s_type_7ba8e9;
struct s_13e703 { short field_0, field_2; };
void function_caf60(long unit_index, point3f *point);
point3f *transform4x3f_apply_point(transform4x3f const *matrix, point3f const *point, point3f *out);
bool function_30710(s_camera const *camera, short_rectangle2d const *rect,
	vector3f const *v, point2f *out, s_view const *view);
long function_13e615(short index);
s_type_7ba8e9 *function_137610(long group_index, short sequence_index, short frame_index);
D3DTexture *function_12360(s_bitmap_view *bitmap, real priority);
void function_13e703(byte const *bitmap, s_13e703 const *position, box2f const *bounds,
	real scale, real angle, real alpha);

// @retail 0x191fd8
void function_191fd8(long player_index)
{
	// Read the later camera and view values through const pointer views.
	s_camera const *camera = (s_camera const *)&g_4b9e14;
	s_view const *view = (s_view const *)&g_camera_storage;
	byte *player = g_4e8c24->data + (player_index & 0xffff) * 0x21c;
	point3f position, transformed;
	point2f projected;
	function_caf60(*(long *)(player + 0x2c), &position);
	position.z += 0.3f;
	transform4x3f_apply_point((transform4x3f const *)camera, &position, &transformed);
	if (function_30710(camera, NULL, (vector3f const *)&transformed, &projected, view))
	{
		s_type_7ba8e9 *bitmap = function_137610(function_13e615(9), 0, 0);
		if (function_12360((s_bitmap_view *)bitmap, 0.0f))
		{
			long x, y;
			s_13e703 screen;
			__asm
			{
				fld projected.x
				fistp x
			}
			screen.field_0 = (short)(x - g_4b9dd2);
			__asm
			{
				fld projected.y
				fistp y
			}
			screen.field_2 = (short)(y - g_4b9dd0);
			real fraction = 1.0f - ((-transformed.z - g_4b9de0) * 100.0f) / (g_4b9de4 - g_4b9de0);
			if (fraction < 0.075f) fraction = 0.075f;
			else if (fraction > 1.0f) fraction = 1.0f;
			{
				box2f bounds = { 0.0f, 1.0f, (1.0f - fraction) * 0.5f, 1.0f };
				function_13e703((byte const *)bitmap, &screen, &bounds, 1.0f, 0.0f, 1.0f);
			}
		}
	}
}

bool function_13cb40(void);
bool function_15e020(short first, short second);

struct s_hud_flag_view_1917eb
{
    bool draw_items;
    byte unknown01;
    bool draw_messages;
};

// @retail 0x1917eb
void function_1917eb(void)
{
    long local_player_index = g_4b9ed8;
    long player_index = function_14de70(local_player_index);
    if (player_index != NONE && function_159170(player_index))
    {
        short mode = (short)function_155760(local_player_index);
        if (((s_hud_flag_view_1917eb *)g_510c98)->draw_items)
        {
            bool draw = false;
            if (mode != 3 && mode != 2 && *(long *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x2c) != NONE)
                draw = true;
            function_19188c(local_player_index);
            if (draw)
            {
                function_192129(g_4b9ed8);
                function_24da7d(g_4b9ed8);
            }
        }
        if (((s_hud_flag_view_1917eb *)g_510c98)->draw_messages)
            function_24cf66(g_4b9ed8);
    }
    function_13954b();
}

// @retail 0x192129
void function_192129(long local_player_index)
{
    long player_index = function_14de70(local_player_index);
    byte *player = g_4e8c24->data + (player_index & 0xffff) * 0x21c;
    if (g_4e6948->state == 1 && !function_13cb40())
    {
        s_data_datum_iterator iterator;
        iterator.data = g_4e8c24;
        iterator.index = NONE;
        iterator.datum_index = NONE;
        while (data_datum_iterator_next(&iterator))
        {
            byte *other = iterator.datum;
            bool different;
            if (g_4e6948->state == 1)
                different = other[0xc0] != player[0xc0];
            else
                different = function_15e020((signed char)other[0xc0], (signed char)player[0xc0]);
            if (iterator.datum_index != player_index && !different && *(long *)(other + 0x2c) != NONE)
                function_191fd8(iterator.datum_index);
        }
    }
}
