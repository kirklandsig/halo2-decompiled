// @flags /O1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"

long function_13a690(long mode);
bool function_22acb4(long local_player_index);

struct s_bitmap_widget_22ae21
{
	byte unknown00[0x1c];
	word mode;
	byte unknown1e[6];
	long bitmap_tag;
	byte unknown28[4];
	long unknown2c;
	char sequences[3];
};

struct s_bitmap_sprite_22ae21
{
	short bitmap_index;
	byte unknown02[6];
	box2f bounds;
	byte unknown18[8];
};

struct s_bitmap_sequence_22ae21
{
	byte unknown00[0x20];
	short first_bitmap;
	short bitmap_count;
	byte unknown24[0x10];
	long sprite_count;
	s_bitmap_sprite_22ae21 *sprites;
};

struct s_bitmap_dimensions_22ae21
{
	byte unknown00[4];
	short width;
	short height;
	byte unknown08[0x74 - 8];
};

struct s_bitmap_tag_22ae21
{
	byte unknown00[0x3c];
	long sequence_count;
	s_bitmap_sequence_22ae21 *sequences;
	long bitmap_count;
	s_bitmap_dimensions_22ae21 *bitmaps;
};

// @retail 0x22ae21
void function_22ae21(long local_player_index, s_bitmap_widget_22ae21 const *widget,
	long *bitmap_index, long *width, long *height, box2f *bounds)
{
	long mode = function_13a690(widget->mode);
	*bitmap_index = NONE;
	if (widget->unknown2c != NONE && widget->bitmap_tag != NONE)
	{
		s_bitmap_tag_22ae21 *tag = (s_bitmap_tag_22ae21 *)g_4e3b44[widget->bitmap_tag & 0xffff].bytes;
		long sequence_index = 0;
		switch (mode)
		{
		case 0:
			sequence_index = widget->sequences[0];
			break;
		case 1:
			sequence_index = widget->sequences[1];
			break;
		case 2:
			sequence_index = widget->sequences[2];
			break;
		}
		if (sequence_index >= 0)
		{
			if (sequence_index < tag->sequence_count)
			{
				s_bitmap_sequence_22ae21 *sequence = &tag->sequences[sequence_index];
				if (sequence->sprite_count > 0)
				{
					long index = 0;
					if (sequence->sprite_count > 1 && function_22acb4(local_player_index))
						index = 1;
					s_bitmap_sprite_22ae21 *sprite = &sequence->sprites[index];
					s_bitmap_dimensions_22ae21 *bitmap = &tag->bitmaps[sprite->bitmap_index];
					*width = bitmap->width;
					*height = bitmap->height;
					*bitmap_index = sprite->bitmap_index;
					*bounds = sprite->bounds;
				}
				else if (sequence->bitmap_count > 0)
				{
					long index = 0;
					if (sequence->bitmap_count > 1 && function_22acb4(local_player_index))
						index = 1;
					s_bitmap_dimensions_22ae21 *bitmap = &tag->bitmaps[sequence->first_bitmap + index];
					*width = bitmap->width;
					*height = bitmap->height;
					*bitmap_index = sequence->first_bitmap + index;
					bounds->x0 = 0.0f;
					bounds->x1 = 1.0f;
					bounds->y0 = 0.0f;
					bounds->y1 = 1.0f;
				}
			}
			else if (sequence_index == 0 && tag->sequence_count == 0 && tag->bitmap_count > 0)
			{
				s_bitmap_dimensions_22ae21 *bitmap = tag->bitmaps;
				*width = bitmap->width;
				*height = bitmap->height;
				*bitmap_index = 0;
				bounds->x0 = 0.0f;
				bounds->x1 = 1.0f;
				bounds->y0 = 0.0f;
				bounds->y1 = 1.0f;
			}
		}
	}
}

void function_126360(long sound_index);
long function_1896c0(real scale, long tag_index);
long function_18a5a0(long tag_index, long object_index, real value);
word *function_18a5e0(long datum_index);

struct s_interface_sound_reference
{
	dword group;
	long tag_index;
};

struct s_interface_sound_entry
{
	s_interface_sound_reference sound;
	dword flags;
	real scale;
	s_interface_sound_reference alternate;
};

struct s_interface_sound_block
{
	long count;
	s_interface_sound_entry const *entries;
};

// @retail 0x22beb5
void function_22beb5(long local_player_index, dword state, s_interface_sound_block const *block,
	long *sounds, word *playing)
{
	for (short index = 0; index < block->count; index++)
	{
		s_interface_sound_entry const *entry = &block->entries[index];
		s_interface_sound_reference const *sound;
		if (function_22acb4(local_player_index))
			sound = &entry->alternate;
		else
			sound = &entry->sound;

		if (entry->flags & state)
		{
			switch (sound->group)
			{
			case 0x736e6421:
				if (sounds[index] == NONE || !(*playing & (1 << index)))
				{
					if (sounds[index] != NONE)
						function_126360(sounds[index]);
					sounds[index] = function_1896c0(entry->scale, sound->tag_index);
				}
				break;
			case 0x6c736e64:
				if (sounds[index] == NONE)
					sounds[index] = function_18a5a0(sound->tag_index, NONE, entry->scale);
				break;
			}
			*playing |= (word)(1 << index);
		}
		else if (sounds[index] != NONE)
		{
			if (sound->group == 0x6c736e64)
				function_18a5e0(sounds[index]);
			sounds[index] = NONE;
			*playing &= (word)~(1 << index);
		}
	}
}
