// @flags /O1 /arch:SSE /Gr
/* UNKNOWN_236299.CPP: playing the user interface sounds */

#include "unknown_11c920.h"
#include "globals.h"
#include <math.h>
#include "unknown_030290.h"
#include "unknown_234c64.h"
#include "unknown_24b5bc.h"
#include "unknown_19b510.h"
#include <string.h>

#pragma intrinsic(memset)

struct s_interface_sound_reference
{
	long index;
	byte unknown04[4];
};

struct s_interface_sound_globals
{
	byte unknown00[0x94];
	s_interface_sound_reference sounds[13];
};

struct s_sound_tag_view
{
	byte flags;
	byte unknown01[7];
	short first_set;
	char set_count;
};

struct s_sound_set_view
{
	short name;
	byte unknown02[0xa];
};

struct s_sound_globals_view
{
	byte unknown00[0x14];
	long *names;
	byte unknown18[0xc];
	s_sound_set_view *sets;
};

struct s_sound_play
{
	long object_index;
	long tag_index;
	real scale;
	byte *set;
};

struct s_type_954545;
s_type_954545 *ui_definition_get(void);
void function_1a0180(long tag_index, long string_handle, word *buffer);

struct s_user_interface_globals_strings
{
	byte unknown00[0x11c];
	long string_list_index;
};

struct s_bitmap_view
{
	byte unknown00[0x74];
};

struct s_bitmap_group_view
{
	byte unknown00[0x44];
	long bitmap_count;
	s_bitmap_view *bitmaps;
};

struct D3DTexture;
D3DTexture *function_12360(s_bitmap_view *bitmap, real priority);
long function_11cae0(void);
struct s_sound_definition;
short function_219070(long set_index, s_sound_definition const *definition);
/* unknown_189010.cpp */
struct s_sound_label_play;
long function_189760(s_sound_label_play const *play);
long function_1896c0(real scale, long tag_index);

/* a string of the user interface globals' string list */
// @retail 0x23620d
void function_23620d(long string_handle, word *buffer)
{
	buffer[0] = 0;
	if (string_handle != NONE)
	{
		s_user_interface_globals_strings *globals = (s_user_interface_globals_strings *)ui_definition_get();
		if (globals && globals->string_list_index != NONE)
		{
			function_1a0180(globals->string_list_index, string_handle, buffer);
		}
	}
}

/* loads every bitmap of a bitmap tag */
/* Retail callers retain the bitmap-loader call boundary. */
__declspec(noinline) void function_23625d(long tag_index);
// @retail 0x23625d
void function_23625d(long tag_index)
{
	if (tag_index != NONE)
	{
		s_bitmap_group_view *group = (s_bitmap_group_view *)g_4e3b44[tag_index & 0xffff].bytes;
		long count = group->bitmap_count;

		for (long i = 0; i < count; i++)
		{
			function_12360(&group->bitmaps[i], 0.0f);
		}
	}
}

// @retail 0x236299
void function_236299(long sound)
{
	s_sound_play play;
	byte set[8];
	s_interface_sound_globals *globals = (s_interface_sound_globals *)ui_definition_get();
	if (globals)
	{
		long tag_index = sound < 13 ? globals->sounds[sound].index : NONE;
		if (tag_index != NONE)
		{
			s_sound_tag_view *tag = (s_sound_tag_view *)g_4e3b44[tag_index & 0xffff].bytes;
			if (tag->flags & 0x80)
			{
				long name;
				switch (function_11cae0())
				{
				case 0:
					name = 0x7000770;
					break;
				case 1:
					name = 0x8000771;
					break;
				case 2:
					name = 0x6000772;
					break;
				case 3:
					name = 0x6000773;
					break;
				case 4:
					name = 0x7000774;
					break;
				case 5:
					name = 0x7000775;
					break;
				case 6:
					name = 0x6000776;
					break;
				case 7:
					name = 0x7000777;
					break;
				case 8:
					name = 0xa000778;
					break;
				default:
					__assume(0);
				}

				for (long i = 0; i < tag->set_count; i++)
				{
					if (((s_sound_globals_view *)g_51ebd4)->names[((s_sound_globals_view *)g_51ebd4)->sets[tag->first_set + i].name] == name)
					{
						set[0] = (byte)i;
						set[1] = (byte)function_219070(i, (s_sound_definition const *)tag);
						play.object_index = NONE;
						play.tag_index = tag_index;
						play.scale = 1.f;
						play.set = set;
						function_189760((s_sound_label_play const *)&play);
						break;
					}
				}
			}
			else
			{
				function_1896c0(1.f, tag_index);
			}
		}
	}
}

struct s_type_7ba8e9;
s_type_7ba8e9 *function_137610(long group_index, short sequence_index, short frame_index);

/* the user interface globals' cursor bitmap */
struct s_user_interface_globals_cursor
{
	byte unknown00[0x114];
	long cursor_bitmap_index;
};

/* a bitmap of the user interface globals' cursor bitmap group */
// @retail 0x236235
s_type_7ba8e9 *function_236235(short frame_index, short sequence_index)
{
	s_type_7ba8e9 *result = 0;
	s_user_interface_globals_cursor *globals = (s_user_interface_globals_cursor *)ui_definition_get();

	if (globals && globals->cursor_bitmap_index != NONE)
	{
		result = function_137610(globals->cursor_bitmap_index, sequence_index, frame_index);
	}
	return result;
}

/* projects a point onto the screen: the window manager's depth moves it away
   from the eye, and x and y scale about the bounds' centre */
/* true when a value is too close to zero to divide by */
static __forceinline bool real_is_tiny(real value)
{
	return (real)fabs(value) < 0.0001f;
}

// @retail 0x2360c3
void function_2360c3(point3f *point, short_rectangle2d const *bounds)
{
	real half_width = (real)(bounds->right - bounds->left) * 0.5f;
	real half_height = (real)(bounds->bottom - bounds->top) * 0.5f;
	real depth = g_54d598.color14.green;
	real scale;

	if (real_is_tiny(g_54d598.color14.green))
	{
		depth = 1.0f;
	}
	point->z += depth;
	if (point->z >= 0.0f)
	{
		point->z = point->z > 1.0f ? point->z : 1.0f;
	}
	else
	{
		point->z = -1.0f < point->z ? -1.0f : point->z;
	}
	scale = 1.0f / depth;
	scale = 1.0f / (scale * point->z);
	point->x = point->x * scale + half_width;
	point->y = half_height - point->y * scale;
	point->z -= depth;
}

/* projects both corners of a rectangle at one depth */
// @retail 0x23618e
s_float_rect *function_23618e(s_float_rect *rect, real depth, short_rectangle2d const *bounds)
{
	point3f p0;
	point3f p1;

	p0.x = rect->x0;
	p0.y = rect->y0;
	p0.z = depth;
	p1.x = rect->x1;
	p1.y = rect->y1;
	p1.z = depth;
	function_2360c3(&p0, bounds);
	function_2360c3(&p1, bounds);
	rect->x0 = p0.x;
	rect->y0 = p0.y;
	rect->x1 = p1.x;
	rect->y1 = p1.y;
	return rect;
}

/* how far a window's widgets slide in from: a quarter of the screen comes
   from its own corner, half of it from its own edge, the full screen from
   nowhere */
// @retail 0x2363d4
void function_2363d4(short_rectangle2d const *bounds, short *x, short *y)
{
	short bottom = bounds->bottom;
	short left = bounds->left;
	short top = bounds->top;
	short right = bounds->right;
	short width = right - left;
	short height = bottom - top;

	if (width == 320)
	{
		if (top == 0)
		{
			if (left == 0)
			{
				*x = 20;
				*y = -16;
			}
			else
			{
				*x = -20;
				*y = -16;
			}
		}
		else
		{
			if (left == 0)
			{
				*x = 20;
				*y = 16;
			}
			else
			{
				*x = -20;
				*y = 16;
			}
		}
	}
	else if (height == 240)
	{
		if (top == 0)
		{
			*x = 0;
			*y = -16;
		}
		else
		{
			*x = 0;
			*y = 16;
		}
	}
	else
	{
		*x = 0;
		*y = 0;
	}
}

/* a machine address */
struct s_machine_address
{
	byte bytes[6];
};

struct s_profile_record;
void function_18fd94(long index, dword *xuid, s_profile_record *record);

/* a signed in local player as the session is told of it (0xe4 bytes) */
struct s_local_player_entry
{
	bool valid;
	byte unknown01;
	short value02;
	long index;
	s_machine_address address;
	byte xuid[0x1c - 0xe];
	byte record[0xe4 - 0x1c];
};

/* lists the signed in local players with this machine's address */
// @retail 0x23654b
void __stdcall function_23654b(void *c, void *a, void *b)
{
	s_local_player_entry *entries = (s_local_player_entry *)c;
	s_machine_address *address = (s_machine_address *)a;
	long *count = (long *)b;

	*address = *(s_machine_address *)g_4cf7cc;
	long entry_count = 0;
	memset(entries, 0, 16 * sizeof(s_local_player_entry));
	s_local_player_entry *cursor = entries;
	long index = 0;
	do
	{
		if (TEST_FIELD_BIT(((s_player_slot_sign_in_view *)g_54e8e0)[index].signed_in))
		{
			entry_count++;
			s_local_player_entry *entry = cursor++;

			entry->valid = true;
			entry->address = *address;
			entry->value02 = (short)((s_player_slot_view_04 *)g_54e8e0)[index].value04;
			entry->index = index;
			function_18fd94(index, (dword *)entry->xuid, (s_profile_record *)entry->record);
		}
		index = next_controller_index(index);
	}
	while (index != NONE);
	*count = entry_count;
}

/* the options a game starts with (0x1118 bytes), as the game below fills
   them: the map, the host and the local players */
struct s_game_options
{
	long state;
	byte unknown004[0x10 - 0x4];
	dword time;
	long value14;
	long value18;
	char map_name[0x104];
	byte unknown120[0x12a - 0x120];
	short value12a;
	bool split_screen;
	bool single_player;
	byte unknown12e[0x268 - 0x12e];
	long value268;
	s_machine_address host_address;
	byte unknown272[0x2cc - 0x272];
	bool value2cc;
	s_machine_address address;
	byte unknown2d3;
	s_local_player_entry players[16];
	byte unknown_end[0x1118 - 0x2d4 - 16 * sizeof(s_local_player_entry)];
};

struct s_entry_a;
s_entry_a *function_148d61();
long map_location_progress_get(char const *map_name);
void function_138110(s_game_options *options);
dword function_1462b0(void);
void function_18e790(s_game_options const *options);

/* starts the chosen map with the signed in local players */
// @retail 0x23643f
void function_23643f(void)
{
	long volatile value18 = g_54e7c4;
	long value12a = g_54e7c8;
	long value14 = g_54e7c0;
	s_entry_a *entry = function_148d61();
	char const *map_name;

	if (entry && (map_name = (char const *)entry + 8) != NULL && map_location_progress_get(map_name) == 2)
	{
		s_game_options options;
		s_machine_address address;
		long count;

		function_138110(&options);
		options.value12a = *(short volatile *)&value12a;
		options.value18 = value18;
		options.state = 1;
		options.single_player = false;
		options.value14 = value14;
		strncpy(options.map_name, *(char const *volatile *)&map_name, sizeof(options.map_name));
		options.map_name[sizeof(options.map_name) - 1] = 0;
		options.time = function_1462b0();
		function_23654b(options.players, &address, &count);
		options.value2cc = true;
		options.address = address;
		options.value268 = 1;
		options.host_address = address;
		options.split_screen = count > 1;
		options.single_player = !options.split_screen;
		function_18e790(&options);
	}
}
/* the user interface globals' dialogs: groups of dialogs that share their
   string list, defaults and flags */
struct s_dialog_entry
{
	long dialog_id;
	byte flags;
	byte unknown05;
	char choices;
	byte unknown07;
	long title;
	long message;
	long first_choice;
	long second_choice;
};

struct s_dialog_group
{
	byte unknown00[4];
	byte flags;
	byte unknown05;
	char choices;
	byte unknown07[5];
	long string_list_index;
	long title;
	long message;
	long first_choice;
	long second_choice;
	long dialog_count;
	s_dialog_entry *dialogs;
};

struct s_dialog_globals_view
{
	byte unknown00[0x88];
	long group_count;
	s_dialog_group *groups;
};

/* the definition of a dialog: its group's defaults overridden by its own
   (the definition of dialog 1 for an unknown dialog) */
// @retail 0x23661f
void function_23661f(s_dialog_definition *definition, long dialog_id)
{
	bool found = false;
	s_dialog_globals_view *globals = (s_dialog_globals_view *)ui_definition_get();

	memset(definition, 0, sizeof(*definition));
	definition->dialog_id = dialog_id;
	definition->string_list_index = NONE;
	definition->title = 0;
	definition->message = 0;
	definition->screen_id = 7;
	if (globals)
	{
		for (long i = 0; !found && i < globals->group_count; i++)
		{
			s_dialog_group *group = &globals->groups[i];

			for (long j = 0; !found && j < group->dialog_count; j++)
			{
				s_dialog_entry *entry = &group->dialogs[j];

				if (entry->dialog_id == dialog_id)
				{
					found = true;
					if (group->string_list_index != NONE)
					{
						definition->string_list_index = group->string_list_index;
					}
					if (entry->title)
					{
						definition->title = entry->title;
					}
					else if (group->title)
					{
						definition->title = group->title;
					}
					if (entry->message)
					{
						definition->message = entry->message;
					}
					else if (group->message)
					{
						definition->message = group->message;
					}
					if (entry->first_choice)
					{
						definition->first_choice = entry->first_choice;
					}
					else if (group->first_choice)
					{
						definition->first_choice = group->first_choice;
					}
					if (entry->second_choice)
					{
						definition->second_choice = entry->second_choice;
					}
					else if (group->second_choice)
					{
						definition->second_choice = group->second_choice;
					}
					definition->choices = group->choices;
					if (entry->choices)
					{
						definition->choices = entry->choices;
					}
					long screen_id;
					if ((group->flags & 1) || (entry->flags & 1))
					{
						screen_id = 0xf0;
					}
					else
					{
						screen_id = 7;
					}
					definition->screen_id = (short)screen_id;
				}
			}
		}
	}
	if (!found && dialog_id != 1)
	{
		function_23661f(definition, 1);
	}
}

struct s_1ed70_point { short x, y; };
void function_1eb00();
void function_1ed70(color4f const *color, s_1ed70_point const *points, short count);
void function_1ee50();

// @retail 0x235d69
void function_235d69(short_rectangle2d const *rectangle, real depth,
    short_rectangle2d const *screen, color4f const *color)
{
    point3f a, b, c, d;
    a.x = (real)rectangle->left;
    a.y = (real)rectangle->top;
    a.z = depth;
    b.x = (real)rectangle->right;
    b.y = (real)rectangle->top;
    b.z = depth;
    c.x = (real)rectangle->left;
    c.y = (real)rectangle->bottom;
    c.z = depth;
    d.x = (real)rectangle->right;
    d.y = (real)rectangle->bottom;
    d.z = depth;
    function_2360c3(&a, screen);
    function_2360c3(&b, screen);
    function_2360c3(&c, screen);
    function_2360c3(&d, screen);
    s_1ed70_point points[5];
    points[0].x = (short)a.x; points[0].y = (short)a.y;
    points[1].x = (short)c.x; points[1].y = (short)c.y;
    points[2].x = (short)d.x; points[2].y = (short)d.y;
    points[3].x = (short)b.x; points[3].y = (short)b.y;
    points[4].x = (short)a.x; points[4].y = (short)a.y;
    function_1eb00();
    function_1ed70(color, points, 5);
    function_1ee50();
}

struct s_texture_rect
{
	real left, top, right, bottom;
};

struct s_interface_draw_vertex
{
	point2f position;
	point2f texture;
	dword color;
};

struct s_interface_draw_request
{
	dword flags;
	byte unknown04[8];
	s_bitmap_view *bitmap;
	byte unknown10[0x18];
	real scale28;
	real scale2c;
	byte unknown30[0x10];
	real scale40;
	real scale44;
	byte unknown48[0x4c];
	short mode;
	byte unknown96[2];
};

void __stdcall function_52040(void const *request, void const *vertices);

/* Draws a textured rectangle through the shared rendering request. */
// score 8 (was 8): retail loads x0 first and stores the first corner's x
// before its y, ours stores the y first; every other order scores worse.
// Tried: both extents first, then the corners in order 86 (retail's instructions,
// registers rotated: the bottom sum copies y0 instead of adding into height);
// chained corner stores 86; the bottom as `height += bounds->y0` 23; corners as a
// real[4][2] array 8; const extents 8. Lane 10 round 42 (a compile-only harness of the corner
// block reproduces 8 and 86; all 24 orders of the first corner's two stores and the two extents,
// each with both sum operand orders, all 8 x/y orders of the other corners, and 180 orders with named
// right and bottom sums stay at 8 or above; the extents-first form has retail's instructions with
// the three xmm registers rotated and the bottom added into a copy of y0): right and bottom named
// sums 85; height, y store, width, x store 19; an aggregate initialiser after both extents 85;
// a size point local 19; the other corners stored last to first 21.
// real[4][2] array 8; const extents 8.
// KIMI1 r94: width/height output helper 86; height-first output helper 85;
// ordinary inline extents helper 86; returned point extents 118; bounds corner
// helper 86; reversed corner helper 86; width-only and height-only helpers 86 each.
// @retail 0x23675a
void function_23675a(s_bitmap_view *bitmap, s_texture_rect const *texture, dword color, short mode, s_float_rect const *bounds)
{
	point2f corners[4];
	corners[0].y = bounds->y0;
	corners[0].x = bounds->x0;
	real height = bounds->y1 - bounds->y0;
	real width = bounds->x1 - bounds->x0;
	corners[1].x = bounds->x0 + width;
	corners[1].y = bounds->y0;
	corners[2].x = bounds->x0 + width;
	corners[2].y = bounds->y0 + height;
	corners[3].x = bounds->x0;
	corners[3].y = bounds->y0 + height;
	s_interface_draw_vertex vertices[4];
	for (long i = 0; i < 4; i++)
	{
		vertices[i].color = color;
		vertices[i].texture.x = i % 3 ? texture->right : texture->left;
		vertices[i].texture.y = i > 1 ? texture->bottom : texture->top;
		vertices[i].position = corners[i];
	}
	s_interface_draw_request request;
	memset(&request, 0, sizeof(request));
	request.mode = mode;
	request.scale44 = 1.0f;
	request.scale40 = 1.0f;
	request.scale2c = 1.0f;
	request.scale28 = 1.0f;
	request.bitmap = bitmap;
	if (function_12360(bitmap, 0.0f))
	{
		function_52040(&request, vertices);
	}
}

/* Scales texture coordinates to the sprite and optionally clips them. */
// @retail 0x235e5e
void function_235e5e(s_sprite_element *element, s_float_rect *from, s_float_rect *to, dword color, long clamp, long mode)
{
	real width = element->width;
	real height = element->height;
	real draw_width = from->x1 - from->x0;
	real draw_height = from->y1 - from->y0;
	s_texture_rect texture;
	texture.top = 0.0f;
	texture.left = 0.0f;
	texture.right = draw_width / (1.0f > width ? 1.0f : width);
	texture.bottom = draw_height / (1.0f > height ? 1.0f : height);
	if ((byte)clamp)
	{
		if (texture.right > 1.0f)
			texture.right = 1.0f;
		if (texture.bottom > 1.0f)
			texture.bottom = 1.0f;
	}
	if (((byte *)element)[0xe] & 0x10)
	{
		real scale_width = element->width;
		real scale_height = element->height;
		texture.left = scale_width * texture.left;
		texture.right = scale_width * texture.right;
		texture.top = scale_height * texture.top;
		texture.bottom = scale_height * texture.bottom;
	}
	function_23675a((s_bitmap_view *)element, &texture, color, (short)mode, to);
}

/* Draws the four portions of a rectangle around its split point. */
// @retail 0x235f31
void function_235f31(s_sprite_element *element, s_float_rect const *from, point2f const *split, dword color, short mode)
{
	(void)&color;
	(void)&mode;
	s_float_rect bounds;
	s_texture_rect texture;
	long i = 0;
	do
	{
		switch (i)
		{
		case 0:
			bounds.x0 = from->x0;
			bounds.x1 = (from->x1 - from->x0) * split->x + from->x0;
			bounds.y0 = from->y0;
			bounds.y1 = (from->y1 - from->y0) * split->y + from->y0;
			texture.left = 1.0f - split->x;
			texture.top = 1.0f - split->y;
			texture.right = 1.0f;
			texture.bottom = 1.0f;
			break;
		case 1:
			bounds.x0 = (from->x1 - from->x0) * split->x + from->x0;
			bounds.x1 = from->x1;
			bounds.y0 = from->y0;
			bounds.y1 = (from->y1 - from->y0) * split->y + from->y0;
			texture.left = 0.0f;
			texture.top = 1.0f - split->y;
			texture.right = 1.0f - split->x;
			texture.bottom = 1.0f;
			break;
		case 2:
			bounds.x0 = from->x0;
			bounds.x1 = (from->x1 - from->x0) * split->x + from->x0;
			bounds.y0 = (from->y1 - from->y0) * split->y + from->y0;
			bounds.y1 = from->y1;
			texture.left = 1.0f - split->x;
			texture.top = 0.0f;
			texture.right = 1.0f;
			texture.bottom = 1.0f - split->y;
			break;
		case 3:
			bounds.x0 = (from->x1 - from->x0) * split->x + from->x0;
			bounds.x1 = from->x1;
			bounds.y0 = (from->y1 - from->y0) * split->y + from->y0;
			bounds.y1 = from->y1;
			texture.left = 0.0f;
			texture.top = 0.0f;
			texture.right = 1.0f - split->x;
			texture.bottom = 1.0f - split->y;
			break;
		default:
			__assume(0);
		}
		if (bounds.x1 - bounds.x0 >= 1.0f && bounds.y1 - bounds.y0 >= 1.0f)
		{
			if (((byte *)element)[0xe] & 0x10)
			{
				real width = element->width;
				real height = element->height;
				texture.left = width * texture.left;
				texture.right = width * texture.right;
				texture.top = height * texture.top;
				texture.bottom = height * texture.bottom;
			}
			function_23675a((s_bitmap_view *)element, &texture, color, mode, &bounds);
		}
		i++;
	} while (i < 4);
}
