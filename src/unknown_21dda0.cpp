// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_21DDA0.CPP: the sound voices (g_502114, "sound voices", 0x6c
   bytes each): playing sounds that share a source, a class and a position
   share one voice, which counts its sounds */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_0259d0.h"
#include "object_queries.h"
#include "sound_sources.h"
#include <math.h>
#include <string.h>

/* a playing sound in g_4e637c (0xbc bytes), as the voices see it */
struct s_voice_playing_sound
{
	byte unknown00[0xc];
	long definition_index;
	long object_index;
	s_sound_source_callbacks const *source;
	s_type_99c531 location;
	byte marker[0x30];
	byte unknown8c[0xbc - 0x8c];
};

/* a voice (0x6c bytes) */
struct s_sound_voice
{
	short salt;
	short sound_index;
	long sound_class;
	bool ambient;
	byte unknown09;
	short reference_count;
	long object_index;
	s_sound_source_callbacks const *source;
	byte marker[0x30];
	long time;
	s_sound_position position;
};

/* the sound settings the voices are sized by */
struct s_sound_voice_settings
{
	byte unknown00[0xc];
	short maximum_voice_count;
};

struct s_sound_class_view_221810
{
	byte unknown00[0x10];
	real gain_bounds;
	byte unknown14[0x50 - 0x14];
	real distance;
};

struct s_sound_tag_class_view
{
	byte unknown00[2];
	char sound_class;
};

/* the sound system's state, as the voices read it */
struct s_sound_system_voice_view
{
	byte unknown00[0x70];
	dword voice_bits[4];
	dword time;
};

struct s_4e6380;
extern s_4e6380 *g_4e6380;
/* a debug switch that keeps every sound on a voice of its own */
bool g_55e780;

/* unknown_124f90.cpp */
extern s_record_pool *g_502114;

struct s_bsp3d;
extern s_bsp3d *g_4e033c;
long function_14a280(s_bsp3d *bsp, long index, point3f *point);

struct s_structure_leaf_view
{
	short cluster_index;
	byte unknown02[6];
};

struct s_structure_bsp_leaves_view_21dde0
{
	byte unknown00[0x30];
	s_structure_leaf_view *leaves;
};

s_record_pool *function_11cc20(long maximum_count, const char *name, long size);
struct s_unknown_5c;
s_unknown_5c *function_221810(short index);
short sound_channel_allocate(void);
void sound_channel_clear(short channel_index);

#define TEST_BIT(flags, bit) (((flags) >> (bit)) & 1)

static inline s_sound_voice *sound_voice_get(long voice_index)
{
	return &((s_sound_voice *)g_502114->data)[voice_index & 0xffff];
}

// @retail 0x21dda0
bool sound_voices_initialize(s_sound_voice_settings const *settings)
{
	bool result = false;

	g_502114 = function_11cc20(settings->maximum_voice_count, "sound voices", sizeof(s_sound_voice));
	if (g_502114)
	{
		g_502114->valid = true;
		record_pool_release_all(g_502114);
		result = true;
	}
	return result;
}

// @retail 0x21dde0
void sound_voices_update_locations(void)
{
	s_record_pool_iterator iterator;
	s_sound_voice *voice;

	iterator.data = g_502114;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while ((voice = (s_sound_voice *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		s_sound_position *local_0 = &voice->position;

		if (g_4686c4 == NONE)
		{
			local_0->location.leaf_index = NONE;
			local_0->location.cluster_index = NONE;
		}
		else
		{
			long leaf_index = function_14a280(g_4e033c, 0, &local_0->position);
			local_0->location.leaf_index = leaf_index;
			local_0->location.cluster_index = leaf_index != NONE ? ((s_structure_bsp_leaves_view_21dde0 *)g_4e0348)->leaves[leaf_index].cluster_index : NONE;
		}
		local_0->location.bsp_index = g_4686c4;
	}
}

/* whether two sound classes play alike */
#pragma inline_depth(0)
// @retail 0x21e1d0
long sound_classes_match(long class_a, long class_b)
{
	if (class_a != class_b)
	{
		if (!(((s_sound_class_view_221810 *)function_221810((short)class_a))->gain_bounds == ((s_sound_class_view_221810 *)function_221810((short)class_b))->gain_bounds &&
			(real)fabs((double)(((s_sound_class_view_221810 *)function_221810((short)class_a))->distance - ((s_sound_class_view_221810 *)function_221810((short)class_b))->distance)) < 0.0001f))
			return false;
	}
	return true;
}
#pragma inline_depth(255)

/* whether a playing sound can share a voice */
// @retail 0x21e0b0
long sound_voice_matches(s_voice_playing_sound const *sound, s_sound_voice const *voice)
{
	s_sound_source_callbacks const *source = voice->source;
	bool matched = false;

	if (source == sound->source &&
		voice->ambient == TEST_BIT(sound->location.flags, 0) &&
		(byte)sound_classes_match((char)voice->sound_class, ((s_sound_tag_class_view *)g_4e3b44[sound->definition_index & 0xffff].bytes)->sound_class))
	{
		if (source)
		{
			if (voice->object_index == sound->object_index)
			{
				bool (__stdcall *local_0)(void const *, void const *) = source->compare;
				matched = local_0 && local_0(sound->marker, voice->marker);
			}
		}
		else
		{
			matched = sound->location.spatial.location.cluster_index == voice->position.location.cluster_index &&
				sound->location.spatial.location.leaf_index == voice->position.location.leaf_index &&
				fabs(sound->location.spatial.position.x - voice->position.position.x) < 0.0001f &&
				fabs(sound->location.spatial.position.y - voice->position.position.y) < 0.0001f &&
				fabs(sound->location.spatial.position.z - voice->position.position.z) < 0.0001f &&
				fabs(sound->location.spatial.velocity.i - voice->position.velocity.i) < 0.0001f &&
				fabs(sound->location.spatial.velocity.j - voice->position.velocity.j) < 0.0001f &&
				fabs(sound->location.spatial.velocity.k - voice->position.velocity.k) < 0.0001f;
		}
	}
	return matched && !g_55e780;
}

// @retail 0x21dff0
bool sound_voice_update(s_voice_playing_sound const *sound, bool *orphaned, long voice_index)
{
	bool result = false;
	s_sound_voice *voice = sound_voice_get(voice_index);
	dword time = ((s_sound_system_voice_view *)g_4e6380)->time;

	*orphaned = voice->time == NONE;
	if (voice->time != time)
	{
		voice->position = sound->location.spatial;
		voice->time = time;
		result = true;
	}
	return result;
}

// @retail 0x21e050
void sound_voice_release(long voice_index)
{
	if (voice_index != NONE)
	{
		s_sound_voice *voice = sound_voice_get(voice_index);

		if (--voice->reference_count == 0)
		{
			if (voice->sound_index != NONE)
			{
				dword *bits = ((s_sound_system_voice_view *)g_4e6380)->voice_bits;
				long bit = voice->sound_index;

				bits[bit >> 5] &= ~(1 << (bit & 31));
			}
			record_pool_release(g_502114, voice_index);
		}
	}
}

static inline s_sound_tag_class_view *sound_tag_get(long definition_index)
{
	return (s_sound_tag_class_view *)g_4e3b44[definition_index & 0xffff].bytes;
}

/* the voice a playing sound shares, or a new one; NONE when there are no
   voices left */
// @retail 0x21de90
long sound_voice_find_or_create(s_voice_playing_sound const *sound)
{
	long voice_index = NONE;
	s_record_pool_iterator iterator;
	s_sound_voice *voice;

	iterator.data = g_502114;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while ((voice = (s_sound_voice *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		if ((byte)sound_voice_matches(sound, voice))
		{
			voice_index = iterator.datum_index;
			break;
		}
	}

	if (voice_index == NONE && g_502114->actual_count < g_502114->maximum_count)
	{
		short sound_index = sound_channel_allocate();

		if (sound_index != NONE)
		{
			voice_index = record_pool_allocate(g_502114);
			if (voice_index != NONE)
			{
				s_sound_tag_class_view *tag;

				voice = sound_voice_get(voice_index);
				tag = sound_tag_get(sound->definition_index);
				voice->reference_count = 0;
				voice->time = NONE;
				voice->sound_class = tag->sound_class;
				voice->ambient = TEST_BIT(sound->location.flags, 0);
				voice->position = sound->location.spatial;
				memcpy(voice->marker, sound->marker, sizeof(voice->marker));
				voice->object_index = sound->object_index;
				voice->sound_index = sound_index;
				voice->source = sound->source;
			}
			else
			{
				sound_channel_clear(sound_index);
			}
		}
	}

	if (voice_index != NONE)
	{
		s_sound_voice *shared = sound_voice_get(voice_index);
		shared->reference_count++;
	}
	return voice_index;
}
