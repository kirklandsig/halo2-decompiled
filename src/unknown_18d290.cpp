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
bool function_bafb0(long ancestor_index, long object_index);

bool function_14ddc0(long local_player_index);

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
			bool split = exists && ((1 << function_155760(i)) & 3) != 0;
			if (split)
			{
				long unit_index = function_14ddc0(i) ? ((s_player_view *)g_4e8c24->data)[g_4e8c20->entries[i] & 0xffff].unit_index : NONE;
				if (unit_index != NONE && (function_bafb0(unit_index, object_index) || function_bafb0(object_index, unit_index)))
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
