// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_16E290.CPP: requests for the geometry blocks of the structure bsp
   and of render models (geometry_cache.h), and two small initializers */

#include "unknown_11c920.h"
#include "globals.h"
#include "object_queries.h"
#include "unknown_0259d0.h"
#include "geometry_cache.h"
#include "unknown_218850.h"
#include "unknown_03bcb0.h"

#include <string.h>

bool function_12dcb0(s_geometry_block_info *block);

extern dword g_4e6494;
extern vector3f *g_4687b0;

/* a structure bsp, as read here */
struct s_16e290_cluster
{
	byte unknown00[0xc];
	s_geometry_block_info block;
	byte unknown_end[0x38 - 0xc - sizeof(s_geometry_block_info)];
};

struct s_16e290_index
{
	word unknown0;
	word cluster_index;
	byte unknown4[8];
};

/* a bitmap reference of the structure bsp (4 bytes) */
struct s_16e290_bitmap_reference
{
	short bitmap_index;
	byte unknown2[2];
};

struct s_16e290_bsp
{
	byte unknown00[4];
	long checksum;
	byte unknown08[0x1c - 0x8];
	long unknown1c;
	byte unknown20[0x2c - 0x20];
	s_16e290_bitmap_reference *bitmaps2c;
	byte unknown30[0x40 - 0x30];
	long cluster_count;
	s_16e290_cluster *clusters;
	byte unknown48[4];
	s_16e290_bitmap_reference *bitmaps4c;
	byte unknown50[4];
	s_16e290_index *indices54;
	byte unknown58[0x64 - 0x58];
	s_16e290_index *indices64;
};

struct s_16e290_match_view
{
	byte unknown0[8];
	long checksum;
};

/* the object with a geometry block at +0x28 and a result at +0x50 */
struct s_16e290_resource
{
	byte unknown00[0x28];
	s_geometry_block_info block;
	byte unknown_pad[0x50 - 0x28 - sizeof(s_geometry_block_info)];
	long value;
};

__declspec(noinline) long function_16e290(s_16e290_resource *resource);

// @retail 0x16e290
long function_16e290(s_16e290_resource *resource)
{
	long result = 0;

	if (!g_4e6494 || function_12de70(&resource->block, 0))
	{
		result = resource->value;
	}
	return result;
}

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

/* 0x16e5a0 (one cluster's block request) is in unknown_16e5a0.cpp: retail
   calls it out of line, which needs an /Ob1 file */

/* the render model sections, as read here */
struct s_16e290_section
{
	byte unknown00[0x28];
	s_geometry_block_info block;
	byte unknown_pad[0xc8 - 0x28 - sizeof(s_geometry_block_info)];
};

struct s_16e290_permutation
{
	byte unknown00[0x34];
	short section_index;
	byte unknown36[0x58 - 0x36];
};

struct s_16e290_render_model
{
	byte unknown000[0x13c];
	s_16e290_section *sections;
	byte unknown140[4];
	s_16e290_permutation *permutations;
};

// @retail 0x16e770
bool function_16e770(long render_model_index, long permutation_index)
{
	s_16e290_render_model *render_model = (s_16e290_render_model *)g_4e3b44[render_model_index & 0xffff].bytes;
	s_16e290_section *section = &render_model->sections[render_model->permutations[permutation_index].section_index];

	return function_12dcb0(&section->block);
}

// @retail 0x16e890
bool function_16e890(long index)
{
	bool result = true;

	if (g_4e0344 && g_4e0344->count > 0 && g_4e0348)
	{
		s_16e290_bsp *bsp = g_4e0344->bsp;

		if (bsp->unknown1c != NONE && bsp->checksum == ((s_16e290_match_view *)g_4e0348)->checksum)
		{
			s_16e290_cluster *cluster = &bsp->clusters[bsp->indices64[index].cluster_index];

			result = function_12dcb0(&cluster->block);
		}
	}
	return result;
}

// @retail 0x16e8f0
bool function_16e8f0(long index)
{
	bool result = true;

	if (g_4e0344 && g_4e0344->count > 0 && g_4e0348)
	{
		s_16e290_bsp *bsp = g_4e0344->bsp;

		if (bsp->unknown1c != NONE && bsp->checksum == ((s_16e290_match_view *)g_4e0348)->checksum)
		{
			s_16e290_cluster *cluster = &bsp->clusters[bsp->indices54[index].cluster_index];

			result = function_12dcb0(&cluster->block);
		}
	}
	return result;
}

/* a 0x44 byte state cleared with the default vectors */
struct s_16f460
{
	byte unknown00[0x28];
	real scale;
	vector3f forward;
	vector3f up;
	byte unknown44[0xac - 0x44];
};

extern real g_54e854;

// @retail 0x16f460
void function_16f460(s_16f460 *state)
{
	memset(state, 0, sizeof(s_16f460));
	state->forward = *g_4687a8;
	state->up = *g_4687b0;
	state->scale = g_54e854;
}

bool g_4ea935;
extern byte g_4ea936;
extern long g_4e64a0;
extern long g_4e64a4;
extern long g_4e64ac; /* unknown_12de70.cpp */
extern long g_4e6470; /* unknown_12be90.cpp */
long g_4e6474;

// @retail 0x16f200
void function_16f200(void)
{
	if (g_4ea935)
	{
		if (g_4e6948->state != 2)
		{
			g_4e64a4 = 1;
			g_4e64a0 = 3;
			g_4e64ac = 0;
			g_4e6470 = 3;
			g_4e6474 = 1;
		}
		g_4ea935 = false;
	}
	if (g_4ea936)
	{
		if (g_4e6948->state != 2)
		{
			g_4e64a4 = 0;
			g_4e64a0 = 3;
			g_4e64ac = 0;
			g_4e6470 = 3;
			g_4e6474 = 0;
		}
		g_4ea936 = false;
	}
}

struct s_location;
void function_11bed0(s_location *location, point3f const *point);

/* the state at g_510c50 (13bf00), as read here */
struct s_16f120_state
{
	byte unknown0[5];
	bool active;
};

struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;

static inline bool local_user_in_use(long user_index)
{
	return g_4e8c20->entries[user_index] != NONE;
}

// @retail 0x16f120
void function_16f120(void)
{
	long user_index;

	for (user_index = 0; user_index < 4; user_index++)
	{
		if (user_index != NONE && local_user_in_use(user_index))
		{
			s_player_state *state = &g_4e9bd4[user_index].state;

			function_11bed0((s_location *)state->unknown0c, &state->position);
		}
	}
	if ((!g_510c50 || !((s_16f120_state *)g_510c50)->active) && g_510c54->game_time > 0)
	{
		g_4ea935 = true;
	}
}

/* a camera-like state: a point, a location and three vectors */
struct s_16f3c0
{
	point3f position;
	long unknown0c;
	short unknown10;
	short bsp_index;
	vector3f vector14;
	vector3f forward;
	vector3f up;
	real scale;
	byte unknown3c[4];
	real unknown40;
	real unknown44;
};

extern point3f *g_468788;
extern vector3f *g_4687a4;

// @retail 0x16f3c0
void function_16f3c0(s_16f3c0 *state)
{
	state->position = *g_468788;
	state->bsp_index = g_4686c4;
	state->unknown0c = NONE;
	state->unknown10 = NONE;
	state->vector14 = *g_4687a4;
	state->forward = *g_4687a8;
	state->up = *g_4687b0;
	state->scale = g_54e854;
	state->unknown40 = 0.0f;
	state->unknown44 = 0.0f;
}
struct s_portal_16de20
{
	short first_cluster;
	short second_cluster;
	byte unknown04[0x18 - 4];
	dword flags;
	byte unknown1c[0x24 - 0x1c];
};

struct s_cluster_16de20
{
	byte unknown00[0x80];
	dword flags;
	byte unknown84[8];
	long portal_count;
	short *portals;
	byte unknown94[0xb0 - 0x94];
};

/* a bit table of rows of count bits each (0x16e150) */
struct s_16e150_bits
{
	byte unknown00[0x54];
	dword size;
	dword *bits;
	long portal_count;
	s_portal_16de20 *portals;
	byte unknown64[0x9c - 0x64];
	long count;
	s_cluster_16de20 *clusters;
};

// @retail 0x16e150
dword *function_16e150(s_16e150_bits const *table, short index)
{
	long row_words = (table->count + 31) >> 5;
	long table_words = table->count * row_words;

	if ((dword)(table_words * 4) < table->size)
	{
		return &table->bits[index * row_words + table_words];
	}
	return &table->bits[((table->count + 31) >> 5) * index];
}

struct s_word_bit_iterator
{
	short count;
	short index;
	dword remaining;
	dword const *words;
};

bool function_44690(s_word_bit_iterator *iterator, long *out);

// @retail 0x16de20
void function_16de20(short index, s_16e150_bits const *table, dword *output)
{
	long word_count = (table->count + 31) >> 5;
	long bytes = word_count * sizeof(dword);
	dword const *base = table->bits + index * word_count;
	if (table->clusters[index].flags & 2)
	{
		dword portal_bits[16];
		dword expanded[16];
		dword visited[16];
		memcpy(output, function_16e150(table, index), bytes);
		memcpy(expanded, output, bytes);
		memset(visited, 0, ((table->portal_count + 31) >> 5) * sizeof(dword));
		for (;;)
		{
			s_word_bit_iterator iterator;
			iterator.count = (short)word_count;
			iterator.index = 0;
			iterator.remaining = *output;
			iterator.words = output;
			long cluster_index;
			while (function_44690(&iterator, &cluster_index))
			{
				s_cluster_16de20 const *cluster = &table->clusters[cluster_index];
				for (long i = 0; i < cluster->portal_count; ++i)
				{
					long portal_index = cluster->portals[i];
					dword bit = 1 << (portal_index & 31);
					dword *visited_word = &visited[portal_index >> 5];
					if (!(*visited_word & bit))
					{
						s_portal_16de20 const *portal = &table->portals[portal_index];
						if ((portal->flags & 4) && (((dword *)g_4f93a4)[portal_index >> 5] & bit))
						{
							memcpy(portal_bits, function_16e150(table, portal->first_cluster), bytes);
							dword const *other = function_16e150(table, portal->second_cluster);
							long j;
							for (j = word_count - 1; j >= 0; --j)
								portal_bits[j] |= other[j];
							for (j = word_count - 1; j >= 0; --j)
								portal_bits[j] &= base[j];
							for (j = word_count - 1; j >= 0; --j)
								expanded[j] |= portal_bits[j];
						}
						*visited_word |= bit;
					}
				}
			}
			if (!memcmp(expanded, output, bytes))
				break;
			memcpy(output, expanded, bytes);
		}
	}
	else
		memcpy(output, base, bytes);
}

/* the cache flags (bit 4 forces every cluster test to pass) */
struct s_4e64c8_flags
{
	dword unknown0 : 4;
	dword all_clusters : 1;
	dword unknown5 : 27;
};

s_4e64c8_flags g_4e64c8;

struct s_16e210_cluster
{
	byte unknown00[0x70];
	char cluster_reference;
	byte unknown71[0xb0 - 0x71];
};

struct s_16e210_reference
{
	short index;
	byte unknown02[0x18 - 2];
};

struct s_16e210_match_view
{
	byte unknown00[0x68];
	s_16e210_reference *references;
	byte unknown6c[0xa0 - 0x6c];
	s_16e210_cluster *clusters;
};

struct s_16e210_source
{
	byte unknown00[8];
	long value;
	byte unknown0c[4];
};

struct s_16e210_globals_view
{
	byte unknown000[0x34c];
	s_16e210_source *sources;
};

__forceinline s_16e210_cluster *prediction_cluster_get(s_16e210_match_view *match, long index)
{
	s_16e210_cluster *result = NULL;
	if (index != NONE)
	{
		result = &match->clusters[index];
	}
	return result;
}

// @retail 0x16e210
bool function_16e210(long cluster_index, long value)
{
	s_16e210_match_view *match = (s_16e210_match_view *)g_4e0348;
	s_16e210_globals_view *globals = (s_16e210_globals_view *)g_4e0350;
	bool result = false;

	if (globals && match && cluster_index != NONE)
	{
		if (TEST_FIELD_BIT(g_4e64c8.all_clusters))
		{
			result = true;
		}
		else
		{
			long reference = prediction_cluster_get(match, cluster_index)->cluster_reference;

			if ((char)reference != NONE)
			{
				if (value == NONE)
				{
					result = true;
				}
				else
				{
					long index;

					if (reference & 0x80)
					{
						index = match->references[reference & 0x7f].index;
					}
					else
					{
						index = reference & 0x7f;
					}
					if (index != NONE && globals->sources[index].value == value)
					{
						result = true;
					}
				}
			}
		}
	}
	return result;
}

/* a local player's state (g_4e9bd4, 0x358 bytes), as initialized here */
struct s_16f4b0_player
{
	dword signature;
	byte unknown004[4];
	s_16f460 view;
	byte unknown0b4;
	bool unknown0b5;
	bool unknown0b6;
	byte unknown0b7;
	s_16f3c0 state;
	byte unknown100[0x130 - 0x100];
	real scale;
	vector3f forward;
	vector3f up;
	byte unknown14c[0x354 - 0x14c];
	dword signature354;
};

// @retail 0x16f4b0
void __stdcall function_16f4b0(void *player_)
{
	s_16f4b0_player *player = (s_16f4b0_player *)player_;

	player->forward = *g_4687a8;
	player->up = *g_4687b0;
	player->scale = g_54e854;
	function_16f3c0(&player->state);
	function_16f460(&player->view);
	player->signature354 = 'rad!';
	player->signature = 'rad!';
	player->unknown0b4 = 1;
	player->unknown0b5 = false;
	player->unknown0b6 = false;
}

/* the bitmaps of a bitmap tag (0x74 bytes each), as read here */
struct s_bitmap_data;

struct s_16e290_bitmap_group
{
	byte unknown00[0x48];
	byte *bitmaps;
};

bool texture_cache_bitmap_request(s_bitmap_data *bitmap);

// @retail 0x16e7b0
bool function_16e7b0(long index)
{
	bool result = true;

	if (g_4e0344 && g_4e0344->count > 0 && g_4e0348)
	{
		s_16e290_bsp *bsp = g_4e0344->bsp;

		if (*(volatile long *)&bsp->unknown1c != NONE && bsp->checksum == ((s_16e290_match_view *)g_4e0348)->checksum)
		{
			short bitmap_index = bsp->bitmaps2c[index].bitmap_index;

			if (bitmap_index != NONE)
			{
				s_16e290_bitmap_group *group = (s_16e290_bitmap_group *)g_4e3b44[bsp->unknown1c & 0xffff].bytes;

				s_bitmap_data *bitmap = (s_bitmap_data *)(group->bitmaps + bitmap_index * 0x74);

				result = texture_cache_bitmap_request(bitmap);
			}
		}
	}
	return result;
}

// @retail 0x16e820
bool function_16e820(long index)
{
	bool result = true;

	if (g_4e0344 && g_4e0344->count > 0 && g_4e0348)
	{
		s_16e290_bsp *bsp = g_4e0344->bsp;

		if (*(volatile long *)&bsp->unknown1c != NONE && bsp->checksum == ((s_16e290_match_view *)g_4e0348)->checksum)
		{
			long bitmap_index = bsp->bitmaps4c[index].bitmap_index;

			if (bitmap_index != NONE)
			{
				s_16e290_bitmap_group *group = (s_16e290_bitmap_group *)g_4e3b44[bsp->unknown1c & 0xffff].bytes;

				s_bitmap_data *bitmap = (s_bitmap_data *)(group->bitmaps + bitmap_index * 0x74);

				result = texture_cache_bitmap_request(bitmap);
			}
		}
	}
	return result;
}

/* the geometry of a model-like tag, as read here */
struct s_16e950_section
{
	byte unknown00[0x38];
	s_geometry_block_info block;
};

struct s_16e950_part
{
	byte unknown00[4];
	short section_index;
	byte unknown06[0x10 - 6];
};

struct s_16e950_group
{
	byte unknown00[8];
	long part_count;
	s_16e950_part *parts;
};

struct s_16e950_definition
{
	byte unknown00[0x1c];
	long group_count;
	s_16e950_group *groups;
	long section_count;
	s_16e950_section *sections;
};

// @retail 0x16e950
bool function_16e950(long tag_index, short mode)
{
	s_16e950_definition *definition = (s_16e950_definition *)g_4e3b44[tag_index & 0xffff].bytes;
	bool result = true;

	if (mode != 1)
	{
		long section_index;

		for (section_index = 0; section_index < definition->section_count; section_index++)
		{
			s_16e950_section *section = &definition->sections[section_index];
			bool loaded = function_12dcb0(&section->block);

			result = result && loaded;
		}
	}
	else
	{
		long group_index;

		for (group_index = 0; group_index < definition->group_count; group_index++)
		{
			s_16e950_group *group = &definition->groups[group_index];
			long part_index;

			for (part_index = 0; part_index < group->part_count; part_index++)
			{
				s_16e950_section *section = &definition->sections[group->parts[part_index].section_index];
				bool loaded = function_12dcb0(&section->block);

				result = result && loaded;
			}
		}
	}
	return result;
}

/* the sound tables of the sound globals, as read here */
struct s_16ea60_pitch_range
{
	byte unknown00[8];
	short first_permutation;
	short permutation_count;
};

struct s_16ea60_permutation
{
	byte unknown00[0xc];
	short first_chunk;
	byte unknown0e[2];
};

struct s_16ea60_tables
{
	byte unknown00[0x24];
	s_16ea60_pitch_range *pitch_ranges;
	byte unknown28[4];
	s_16ea60_permutation *permutations;
	byte unknown30[0x44 - 0x30];
	s_sound_chunk *chunks;
};

struct s_16ea60_sound
{
	byte unknown00[8];
	short first_pitch_range;
	char pitch_range_count;
};

struct s_sound_globals;
extern s_sound_globals *g_51ebd4;

// @retail 0x16ea60
bool function_16ea60(long sound_index)
{
	s_16ea60_sound *volatile sound = (s_16ea60_sound *)g_4e3b44[sound_index & 0xffff].bytes;
	long pitch_range_count = sound->pitch_range_count;
	bool result = true;
	long pitch_range_index;

	for (pitch_range_index = 0; pitch_range_index < pitch_range_count; pitch_range_index++)
	{
		s_16ea60_tables *tables = (s_16ea60_tables *)g_51ebd4;
		s_16ea60_pitch_range *arg_58ecd0 = &tables->pitch_ranges[sound->first_pitch_range + pitch_range_index];
		short permutation_count = arg_58ecd0->permutation_count;
		long permutation_index;

		for (permutation_index = 0; permutation_index < permutation_count; permutation_index++)
		{
			s_16ea60_permutation *permutation = &tables->permutations[arg_58ecd0->first_permutation + permutation_index];
			dword flags = function_218850(sound_index, &tables->chunks[permutation->first_chunk], 8);

			result = result && (flags & 2);
			tables = (s_16ea60_tables *)g_51ebd4;
		}
	}
	return result;
}

extern byte g_4ea934;
void __stdcall function_16f4b0(void *player_);

// @retail 0x16f0e0
void function_16f0e0(void)
{
	long i;

	g_4ea934 = 1;
	for (i = 0; i < 4; i++)
	{
		function_16f4b0(&g_4e9bd4[i]);
	}
	for (i = 0; i < 5; i++)
	{
		g_510c70[i] = NONE;
	}
}

/* a list of ranges (0x48 bytes each) */
struct s_16e1b0_range
{
	byte unknown00[6];
	word first;
	word count;
	byte unknown0a[0x48 - 0xa];
};

struct s_16e1b0_list
{
	long count;
	s_16e1b0_range *ranges;
};

// @retail 0x16e1b0
void function_16e1b0(long value, s_16e1b0_list const *list, long *unknown, long *range_index, long *offset)
{
	long i;

	*unknown = 0;
	for (i = 0; i < list->count; i++)
	{
		s_16e1b0_range const *range = &list->ranges[i];

		if (range->first <= value && range->first + range->count > value)
		{
			*offset = value - range->first;
			break;
		}
	}
	if (i == list->count)
	{
		*range_index = NONE;
	}
	else
	{
		*range_index = i;
	}
}

/* a resource a tag predicts it will need (8 bytes) */
struct s_predicted_resource
{
	short type;
	short index;
	long tag_index;
};

struct s_predicted_resource_block
{
	long count;
	s_predicted_resource *resources;
};

/* the clusters of a structure bsp tag (0xb0 bytes each) */
struct s_16e5e0_cluster
{
	byte unknown00[0x28];
	s_geometry_block_info block;
	byte unknown4c[0xb0 - 0x28 - sizeof(s_geometry_block_info)];
};

struct s_16e5e0_bsp
{
	byte unknown00[0xa0];
	s_16e5e0_cluster *clusters;
};

// @retail 0x16e5e0
bool function_16e5e0(s_predicted_resource_block const *block, short mode)
{
	bool result = true;
	long i;

	for (i = 0; i < block->count; i++)
	{
		s_predicted_resource const *resource = &block->resources[i];
		bool loaded = true;

		switch (resource->type)
		{
		case 0:
			if (mode != 1)
			{
				if (mode == 2)
				{
					s_16e290_bitmap_group *group = (s_16e290_bitmap_group *)g_4e3b44[resource->tag_index & 0xffff].bytes;

					function_3bcb0((s_bitmap_data *)(group->bitmaps + resource->index * 0x74));
				}
				else
				{
					s_16e290_bitmap_group *group = (s_16e290_bitmap_group *)g_4e3b44[resource->tag_index & 0xffff].bytes;

					texture_cache_bitmap_request((s_bitmap_data *)(group->bitmaps + resource->index * 0x74));
				}
			}
			break;
		case 1:
			loaded = function_16ea60(resource->tag_index);
			break;
		case 3:
			{
				s_16e5e0_bsp *bsp = (s_16e5e0_bsp *)g_4e3b44[resource->tag_index & 0xffff].bytes;
				s_16e5e0_cluster *cluster = &bsp->clusters[resource->index];

				loaded = function_12dcb0(&cluster->block);
			}
			break;
		case 4:
			if (mode != 3)
			{
				loaded = function_16e770(resource->tag_index, resource->index);
			}
			break;
		case 7:
			if (mode != 1)
			{
				loaded = function_16e7b0(resource->index);
			}
			break;
		case 8:
			if (mode != 1)
			{
				loaded = function_16e820(resource->index);
			}
			break;
		case 5:
			loaded = function_16e890(resource->index);
			break;
		case 6:
			loaded = function_16e8f0(resource->index);
			break;
		case 2:
			loaded = function_16e950(resource->tag_index, mode);
			break;
		}
		result = result && loaded;
	}
	return result;
}

/* the structure bsp, as 0x16e510 reads it */
struct s_16e510_cluster
{
	byte unknown00[0x84];
	s_predicted_resource_block field_84;
	byte unknown8c[0xb0 - 0x8c];
};

struct s_16e510_bsp
{
	byte unknown000[0x9c];
	long cluster_count;
	s_16e510_cluster *clusters;
	byte unknown0a4[0x138 - 0xa4];
	long section_count;
	s_16e290_section *sections;
};

// @retail 0x16e510
void function_16e510(short bsp_index, long index, bool sections)
{
	if (bsp_index == g_4686c4)
	{
		s_16e510_bsp *bsp = (s_16e510_bsp *)g_4e0348;

		if (index == NONE)
		{
		}
		else if (!sections)
		{
			if (PIN(index, 0, bsp->cluster_count - 1) == index)
			{
				function_16e5e0(&bsp->clusters[index].field_84, 3);
			}
		}
		else
		{
			if (PIN(index, 0, bsp->section_count - 1) == index)
			{
				s_16e290_section *section = &bsp->sections[index];

				function_12dcb0(&section->block);
			}
		}
	}
}
/* a model's geometry sections (0x5c bytes) and the materials whose shaders
   name the bitmaps to predict (0x20 bytes), as 0x16e330 reads them */
struct s_16e330_section
{
	byte unknown00[0x38];
	s_geometry_block_info block;
	byte unknown_pad[0x5c - 0x38 - sizeof(s_geometry_block_info)];
};

struct s_16e330_material
{
	byte unknown00[0xc];
	long shader_tag_index;
	byte unknown10[0x20 - 0x10];
};

struct s_16e330_extra
{
	byte unknown00[0x34];
	s_geometry_block_info block;
};

struct s_16e330_definition
{
	byte unknown00[0x24];
	long section_count;
	s_16e330_section *sections;
	byte unknown2c[0x60 - 0x2c];
	long material_count;
	s_16e330_material *materials;
	byte unknown68[0x74 - 0x68];
	long extra_count;
	s_16e330_extra *extras;
};

/* a shader's bitmap references (0xc bytes each) */
struct s_16e330_bitmap_reference
{
	long bitmap_tag_index;
	byte unknown04[8];
};

struct s_16e330_shader_bitmaps
{
	byte unknown00[4];
	long count;
	s_16e330_bitmap_reference *references;
};

struct s_16e330_shader
{
	byte unknown00[0x24];
	s_16e330_shader_bitmaps *bitmaps;
};

struct s_16e330_bitmap_group
{
	byte unknown00[0x44];
	long bitmap_count;
	s_bitmap_predict_view *bitmaps;
};

long g_468d24 = NONE;

// @retail 0x16e330
void function_16e330(long tag_index, long section_index)
{
	if (tag_index != NONE)
	{
		s_16e330_definition *definition = (s_16e330_definition *)g_4e3b44[tag_index & 0xffff].bytes;

		if (section_index == NONE)
		{
			long i;

			for (i = 0; i < definition->section_count; i++)
			{
				function_12dcb0(&definition->sections[i].block);
			}
		}
		else if (PIN(section_index, 0, definition->section_count - 1) == section_index)
		{
			function_12dcb0(&definition->sections[section_index].block);
		}

		if (g_468d24 != tag_index)
		{
			long material_index;

			g_468d24 = tag_index;
			for (material_index = 0; material_index < definition->material_count; material_index++)
			{
				long shader_tag_index = definition->materials[material_index].shader_tag_index;

				if (shader_tag_index != NONE)
				{
					s_16e330_shader *shader = (s_16e330_shader *)g_4e3b44[shader_tag_index & 0xffff].bytes;
					s_16e330_shader_bitmaps *bitmaps = shader->bitmaps;
					long reference_index;

					for (reference_index = 0; reference_index < bitmaps->count; reference_index++)
					{
						long bitmap_tag_index = bitmaps->references[reference_index].bitmap_tag_index;

						if (bitmap_tag_index != NONE)
						{
							s_16e330_bitmap_group *group =
								(s_16e330_bitmap_group *)g_4e3b44[bitmap_tag_index & 0xffff].bytes;
							long bitmap_index;

							for (bitmap_index = 0; bitmap_index < group->bitmap_count; bitmap_index++)
							{
								bitmap_predict_inline(&group->bitmaps[bitmap_index], 0xe);
							}
						}
					}
				}
			}
			if (definition->extra_count > 0)
			{
				function_12dcb0(&definition->extras->block);
			}
		}
	}
}

/* a list of bitmap tags to predict (8 bytes each) */
struct s_16eb20_entry
{
	byte unknown00[4];
	long bitmap_tag_index;
};

struct s_16eb20_block
{
	long count;
	s_16eb20_entry *entries;
};

// @retail 0x16eb20
void function_16eb20(s_16eb20_block const *block)
{
	long i;

	for (i = 0; i < block->count; i++)
	{
		long bitmap_tag_index = block->entries[i].bitmap_tag_index;

		if (bitmap_tag_index != NONE)
		{
			s_16e330_bitmap_group *group = (s_16e330_bitmap_group *)g_4e3b44[bitmap_tag_index & 0xffff].bytes;
			long bitmap_index;

			for (bitmap_index = 0; bitmap_index < group->bitmap_count; bitmap_index++)
			{
				bitmap_predict_inline(&group->bitmaps[bitmap_index], 2);
			}
		}
	}
}

/* an observer command (lane R's observer code), as set here */
struct s_observer_command
{
	dword flags;
	byte unknown004[0x88 - 0x4];
	real unknown88;
	byte unknown8c[0x94 - 0x8c];
	vector3f unknown94;
	vector3f unknowna0;
};

/* a local player's observer (g_4e9bd4), as read here */
struct s_16f190_observer
{
	byte unknown000[4];
	s_observer_command *command;
	byte unknown008[0xb4 - 0x8];
	bool unknown0b4;
	bool unknown0b5;
	byte unknown0b6[0x358 - 0xb6];
};

void function_172520(s_observer_command *command);

__declspec(noinline) void function_16f190(long user_index, s_observer_command *command);

// @retail 0x16f190
void function_16f190(long user_index, s_observer_command *command)
{
	s_16f190_observer *observer = &((s_16f190_observer *)g_4e9bd4)[user_index];

	function_172520(command);
	observer->command = command;
	observer->unknown0b4 = false;
	if (!observer->unknown0b5)
	{
		observer->unknown0b5 = true;
		command->unknown88 = 0.0f;
		observer->command->flags |= 8;
		memset(&observer->command->unknown94, 0, 2 * sizeof(vector3f));
	}
}

/* the variants of a tag's entries (0xc8 bytes each) */
struct s_16e2c0_entry
{
	byte unknown00[0x70];
	byte unknown70[0x98 - 0x70];
	long count;
	byte unknown9c[0xb4 - 0x9c];
	long unknownb4;
	byte unknownb8[0xc8 - 0xb8];
};

struct s_16e2c0_definition
{
	byte unknown000[0x138];
	long entry_count;
	s_16e2c0_entry *entries;
};

void function_246c60(void *block, long unknown);

// @retail 0x16e2c0
void function_16e2c0(long tag_index)
{
	s_16e2c0_definition *definition = (s_16e2c0_definition *)g_4e3b44[tag_index & 0xffff].bytes;
	long i;

	for (i = 0; i < definition->entry_count; i++)
	{
		s_16e2c0_entry *entry = &definition->entries[i];

		if (entry->count > 0)
		{
			function_246c60(entry->unknown70, entry->unknownb4);
		}
	}
}

/* the observers' time step and its scale */
real g_4e9bd0;

extern short g_468d3c[6];

struct s_motion_channels_16ff10
{
	byte unknown000[8];
	dword flags;
	byte unknown00c[0x4c - 0xc];
	real velocity[3];
	byte unknown058[0x9c - 0x58];
	real times[6];
	byte unknown0b4[2];
	bool suppress_velocity;
	byte unknown0b7[0x180 - 0xb7];
	real first_derivative[13];
	real second_derivative[13];
	real fifth[13];
	real fourth[13];
	real third[13];
	real second[13];
	real first[13];
	real constant[13];
	real delta[13];
	byte unknown354[4];
};

// @retail 0x16ff10
void function_16ff10(long user_index)
{
	s_motion_channels_16ff10 *state = (s_motion_channels_16ff10 *)g_4e9bd4 + user_index;
	real *fifth = state->fifth;
	real *fourth = state->fourth;
	real *third = state->third;
	real *second = state->second;
	real *first = state->first;
	real *constant = state->constant;
	real *time = state->times;
	real *acceleration = state->second_derivative;
	real *velocity = state->first_derivative;
	real *delta = state->delta;
	function_172520((s_observer_command *)((byte *)state + 8));
	for (short group = 0; group < 6; ++time, ++group)
	{
		if ((state->flags & 1) && *time > g_4e9bd0)
		{
			real inverse = 1.0f / *time;
			real inverse2 = inverse * inverse;
			real inverse3 = inverse2 * inverse;
			real inverse4 = inverse3 * inverse;
			real inverse5 = inverse4 * inverse;
			for (short i = 0; i < g_468d3c[group]; ++i)
			{
				real fifth_delta = delta[i] * inverse5 * 6.0f;
				real fifth_velocity = velocity[i] * inverse4 * 3.0f;
				fifth[i] = acceleration[i] * inverse3 * 0.5f - (fifth_delta + fifth_velocity);
				fourth[i] = delta[i] * inverse4 * 15.0f + velocity[i] * inverse3 * 7.0f - acceleration[i] * inverse2;
				third[i] = acceleration[i] * inverse * 0.5f - (delta[i] * inverse3 * 10.0f + velocity[i] * inverse2 * 4.0f);
				second[i] = 0.0f;
				first[i] = 0.0f;
				constant[i] = delta[i];
				if (!group && !state->suppress_velocity)
				{
					real value = state->velocity[i];
					fifth[i] -= value * inverse4 * 3.0f;
					fourth[i] += value * inverse3 * 8.0f;
					third[i] -= value * inverse2 * 6.0f;
					first[i] += value;
				}
			}
		}
		long count = g_468d3c[group];
		fifth += count;
		fourth += count;
		third += count;
		second += count;
		first += count;
		real *next_constant = constant + count;
		acceleration += count;
		delta += count;
		velocity += count;
		constant = next_constant;
	}
}
real g_468d28 = 1.0f;

struct s_bsp3d;
long function_14a280(s_bsp3d *bsp, long index, point3f *point);
extern s_bsp3d *g_4e033c;
extern short g_4686c4;
struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;

void __stdcall function_16f570(long user_index);

void function_141590(transform4x3f const *in, transform4x3f *out);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
extern transform4x3f *g_4687d0;

PRIVATE inline void transform_vector_16f570(transform4x3f const *matrix, vector3f *vector)
{
	real x = vector->i;
	real y = vector->j;
	real z = vector->k;
	if (matrix->scale != 1.0f)
	{
		x *= matrix->scale;
		y *= matrix->scale;
		z *= matrix->scale;
	}
	vector->i = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x;
	vector->j = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x;
	vector->k = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x;
}

PRIVATE inline void transform_point_16f570(transform4x3f const *matrix, point3f *point)
{
	real x = point->x;
	real y = point->y;
	real z = point->z;
	if (matrix->scale != 1.0f)
	{
		x = matrix->scale * x;
		y = matrix->scale * y;
		z = matrix->scale * z;
	}
	point->x = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x + matrix->position.x;
	point->y = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x + matrix->position.y;
	point->z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z;
}

// @retail 0x16f570
void __stdcall function_16f570(long user_index)
{
	byte *state = (byte *)&g_4e9bd4[user_index];
	s_observer_command *command = *(s_observer_command **)(state + 4);
	real *durations = (real *)((byte *)command + 0x94);
	real *times = (real *)(state + 0x9c);
	byte *flags = (byte *)command + 0x8c;
	function_172520(command);
	if ((*(s_observer_command **)(state + 4))->flags & 1)
	{
		for (long remaining = 6; remaining; --remaining, ++durations, ++times, ++flags)
		{
			if (*flags & 1)
			{
				if (!(*flags & 2) && *times > *durations)
					*durations = *times > 2.0f ? 2.0f : *times;
			}
			else
			{
				command = *(s_observer_command **)(state + 4);
				if (*times > command->unknown88 && !(command->flags & 8))
					*durations = *times > 2.0f ? 2.0f : *times;
				else
					*durations = command->unknown88;
			}
		}
		bool previous = *(bool *)(state + 0xb6);
		if (previous || ((*(s_observer_command **)(state + 4))->flags & 0x40))
		{
			command = *(s_observer_command **)(state + 4);
			if ((char)((command->flags >> 6) & 1) != previous ||
				*(long *)((byte *)command + 0x84) != *(long *)(state + 0x8c))
			{
				transform4x3f matrix;
				if (*(byte volatile *)command & 0x40)
					function_141590((transform4x3f *)((byte *)command + 0x50), &matrix);
				else
					matrix = *g_4687d0;
				if (previous)
					function_142a60(&matrix, (transform4x3f *)(state + 0x14c), &matrix);
				transform_point_16f570(&matrix, (point3f *)(state + 0x10c));
				transform_vector_16f570(&matrix, (vector3f *)(state + 0x134));
				transform_vector_16f570(&matrix, (vector3f *)(state + 0x140));
				transform_vector_16f570(&matrix, (vector3f *)(state + 0x180));
				transform_vector_16f570(&matrix, (vector3f *)(state + 0x1a8));
				transform_vector_16f570(&matrix, (vector3f *)(state + 0x1b4));
				transform_vector_16f570(&matrix, (vector3f *)(state + 0x1dc));
				transform_vector_16f570(&matrix, (vector3f *)(state + 0x320));
				transform_vector_16f570(&matrix, (vector3f *)(state + 0x348));
			}
			command = *(s_observer_command **)(state + 4);
			*(bool *)(state + 0xb6) = (bool)((command->flags >> 6) & 1);
			if (*(bool *)(state + 0xb6))
				*(transform4x3f *)(state + 0x14c) = *(transform4x3f *)((byte *)command + 0x50);
		}
		memcpy(state + 8, *(s_observer_command **)(state + 4), 0xac);
	}
}
void function_16fe90(long user_index);

void function_170bb0(real const *first, real const *second, real *out);
void function_1701f0(long player_index);
void function_1703f0(long player_index);
void function_170630(long player_index);

// @retail 0x16fe90
void function_16fe90(long user_index)
{
	byte *state = (byte *)&g_4e9bd4[user_index];
	real *time = (real *)(state + 0x9c);
	if (!((*(s_observer_command **)(state + 4))->flags & 0x20))
	{
		function_170bb0((real *)(state + 0xc), (real *)(state + 0x10c), (real *)(state + 0x320));
		function_16ff10(user_index);
		function_1701f0(user_index);
		function_1703f0(user_index);
		function_170630(user_index);
		long remaining = 6;
		do
		{
			real value = *time - g_4e9bd0;
			*time++ = value > 0.0f ? value : 0.0f;
		} while (--remaining);
	}
}
void __stdcall function_170fd0(long user_index);
long function_155760(long user_index);
void __stdcall function_1546f0(long user_index, transform4x3f *matrix);
bool function_a74c0(vector3f const *forward, vector3f const *up);
real function_1201a0(vector3f *vector, vector3f const *fallback);
bool function_171d00(point3f const *start, point3f const *end, real *fraction, bool narrow);
struct s_16658d_group;
extern s_16658d_group *g_4e9bc8;

struct s_camera_clip_reference_16ebf0
{
	short field_0;
	short index;
	plane3f plane;
	byte field_14[4];
};

// @retail 0x16ebf0
void function_16ebf0(long user_index)
{
	transform4x3f adjustment = *g_4687d0;
	byte *state = (byte *)g_4e9bd4 + user_index * 0x358;
	byte *volatile saved_state = state;
	if (user_index != NONE)
	{
		if (function_155760(user_index) == 0)
		{
			byte *user = (byte *)g_4e9bc8 + user_index * 0x20cc;
			if (*(long *)(user + 0x2094) != NONE && (*user & 4))
				function_142a60(&adjustment, (transform4x3f *)(user + 0x2098), &adjustment);
		}
		if (!g_510c54->active || !g_510c54->unknown01)
		{
			switch (function_155760(user_index))
			{
			case 0:
			case 2:
				{
					void (__stdcall *const adjust_view)(long, transform4x3f *) = function_1546f0;
					adjust_view(user_index, &adjustment);
				}
				break;
			}
		}
	}
	vector3f *original_forward = (vector3f *)(state + 0xd8);
	vector3f *original_up = (vector3f *)(state + 0xe4);
	point3f *original_position = (point3f *)(state + 0xb8);
	vector3f forward;
	forward.i = original_forward->i;
	forward.j = original_forward->j;
	forward.k = original_forward->k;
	vector3f up;
	up.i = original_up->i;
	up.j = original_up->j;
	up.k = original_up->k;
	transform4x3f matrix;
	matrix.scale = 1.0f;
	matrix.forward = *original_forward;
	matrix.up = *original_up;
	matrix.position = *original_position;
	matrix.left.i = forward.k * up.j - up.k * forward.j;
	matrix.left.j = up.k * forward.i - forward.k * up.i;
	matrix.left.k = forward.j * up.i - up.j * forward.i;
	function_142a60(&matrix, &adjustment, &matrix);
	forward = matrix.forward;
	up = matrix.up;
	point3f point = matrix.position;
	if (!function_a74c0(&forward, &up))
	{
		function_1201a0(&forward, g_4687a8);
		function_1201a0(&up, g_4687a8);
		if (!function_a74c0(&forward, &up))
		{
			forward = *g_4687a8;
			up = *g_4687b0;
		}
	}
	if (!(*(dword *)(saved_state + 8) & 0x10))
	{
		s_location location;
		function_11bed0(&location, &point);
		bool narrow = false;
		if (location.cluster_index != NONE)
		{
			s_16e210_match_view *geometry = (s_16e210_match_view *)g_4e0348;
			byte reference = geometry->clusters[location.cluster_index].cluster_reference;
			if (reference != 0xff)
			{
				s_camera_clip_reference_16ebf0 *entry =
					&((s_camera_clip_reference_16ebf0 *)geometry->references)[reference & 0x7f];
				if (entry->index != NONE)
				{
					if (!(reference & 0x80) ||
						entry->plane.j * point.y + entry->plane.k * point.z + entry->plane.i * point.x - entry->plane.d < 0.0f)
						narrow = true;
				}
			}
		}
		real fraction;
		point3f *original = original_position;
		if (function_171d00(original, &point, &fraction, narrow))
		{
			real amount = fraction * 0.9f;
			real remainder = 1.0f - amount;
			point.x = original->x * remainder + point.x * amount;
			point.y = original->y * remainder + point.y * amount;
			point.z = original->z * remainder + point.z * amount;
		}
	}
	*original_position = point;
	*original_forward = forward;
	*original_up = up;
}
void function_3f450(long cluster_index);
void function_3f500(long cluster_index);

struct s_16f280_leaf
{
	short cluster_index;
	byte unknown02[6];
};

struct s_16f280_leaves_view
{
	byte unknown00[0x30];
	s_16f280_leaf *leaves;
};

struct s_16f280_flags
{
	byte unknown0[5];
	bool active;
};

static inline s_16f190_observer *function_xd356b3(long user_index)
{
	s_16f190_observer *result = NULL;

	if (user_index != NONE)
	{
		result = &((s_16f190_observer *)g_4e9bd4)[user_index];
	}
	return result;
}

static inline bool local_user_exists(long user_index)
{
	return g_4e8c20->entries[user_index] != NONE;
}

/* updates the local players' observers and predicts the cluster each looks from */
// @retail 0x16f280
void __stdcall function_16f280(real dt)
{
	long user_index;

	g_4e9bd0 = g_468d28 * dt;
	for (user_index = 0; user_index < 4; user_index++)
	{
		s_16f190_observer *observer = function_xd356b3(user_index);

		if (observer && local_user_exists(user_index))
		{
			short cluster_index;

			observer->unknown0b4 = true;
			function_16f570(user_index);
			if (g_4e9bd0 != 0.0f)
			{
				function_16fe90(user_index);
			}
			function_170fd0(user_index);
			function_16ebf0(user_index);
			cluster_index = NONE;
			if (g_4686c4 != NONE)
			{
				long leaf_index = function_14a280(g_4e033c, 0, &g_4e9bd4[user_index].state.position);

				if (leaf_index != NONE)
				{
					cluster_index = ((s_16f280_leaves_view *)g_4e0348)->leaves[leaf_index].cluster_index;
				}
				else
				{
					cluster_index = NONE;
				}
			}
			if (!g_510c50 || !((s_16f280_flags *)g_510c50)->active)
			{
				if (!(g_4ea934))
				{
					function_3f500(cluster_index);
				}
				else
				{
					if (cluster_index != NONE)
					{
						function_3f450(cluster_index);
					}
				}
			}
		}
	}
	g_4ea934 = 0;
}
