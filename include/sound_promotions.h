/* SOUND_PROMOTIONS.H: the sound globals' permutation sets and promotions, as
   the sound source callbacks (unknown_18c250.cpp) and 0x218e50 read them */
#ifndef SOUND_PROMOTIONS_H
#define SOUND_PROMOTIONS_H

#include "unknown_11c920.h"
#include "geometry_cache.h"

/* a sound tag, as the promotion code reads it */
struct s_sound_promotion_tag
{
	byte unknown00;
	byte flags;
	char class_index;
	byte unknown03[5];
	short permutation_base;
	byte unknown0a[4];
	short promotion_index;
};

/* a run of byte samples and a run of entries in the promotion data */
struct s_sound_promotion_entry
{
	long sample_offset;
	long sample_count;
	long data_offset;
	long data_count;
};

struct s_sound_promotion_data
{
	byte unknown00[4];
	byte *samples;
	byte unknown08[4];
	s_sound_promotion_entry *entries;
};

struct s_sound_promotion
{
	long count;
	s_sound_promotion_data *data;
	s_geometry_block_info block;
};

struct s_sound_permutation_set
{
	byte unknown00[8];
	short first_permutation;
	short permutation_count;
};

struct s_sound_permutation
{
	byte unknown00[5];
	char entry_index;
	byte unknown06[0x10 - 6];
};

struct s_sound_globals_promotion_view
{
	byte unknown00[0x24];
	s_sound_permutation_set *sets;
	byte unknown28[4];
	s_sound_permutation *permutations;
	byte unknown30[0x54 - 0x30];
	s_sound_promotion *promotions;
};

/* what 0x18ca20 hands 0x10e480: a run of promotion data */
struct s_sound_promotion_state
{
	long count;
	byte const *data;
};

real function_218e50(long tag_index, short set_index, short permutation_index, long sample_index);

#endif
