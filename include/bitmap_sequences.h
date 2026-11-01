/* BITMAP_SEQUENCES.H: the parts of a bitmap tag that the interface's bitmap
   widgets read: its sequences, each sequence's sprites, and the bitmaps */

#ifndef BITMAP_SEQUENCES_H
#define BITMAP_SEQUENCES_H

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

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

#endif
