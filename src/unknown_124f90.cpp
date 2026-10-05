// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_124F90.CPP: the playing sounds and their definitions: per-sound
   overrides of the definition's class (distances, cone angles and gain) and
   the random gain and pitch drawn from the class. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "data_array.h"
#include "unknown_218850.h"
#include "unknown_2ae170.h"
#include "unknown_124f90.h"
#include "unknown_218ac0.h"
#include "unknown_221810.h"
#include "sound_driver.h"
#include <xtl.h>
#include <string.h>
#include <float.h>
#include <math.h>

#define k_pi 3.14159274f
#define PIN(value, lower, upper) ((lower) > (value) ? (lower) : ((value) > (upper) ? (upper) : (value)))

/* a sound being played */
struct s_sound
{
	word flag0 : 1;
	word flag1 : 1;
	word override_minimum_distance : 1;
	word override_maximum_distance : 1;
	word override_inner_cone_angle : 1;
	word override_outer_cone_angle : 1;
	word override_outer_cone_gain : 1;
	word unknown00 : 9;
	byte unknown02[6];
	word flag8_0 : 1;
	word flag8_1 : 1;
	word flag8_2 : 1;
	word flag8_3 : 1;
	word unknown08 : 12;
	word unknown0a_0 : 11;
	word flag0a_11 : 1;
	word unknown0a : 4;
	byte unknown0c[0x28];
	real minimum_distance;
	real maximum_distance;
	word inner_cone_angle;
	word outer_cone_angle;
	long outer_cone_gain;
};

/* a sound class, in the sound globals (0x38 bytes) */
struct s_sound_class
{
	real minimum_distance;
	real maximum_distance;
	real skip_fraction_scale;
	byte unknown0c[4];
	long gain_base;		/* decibels, as real bits */
	long gain_variance;
	short pitch_lower;
	short pitch_upper;
	real inner_cone_angle;
	real outer_cone_angle;
	long outer_cone_gain;
	byte unknown28[0x10];
};

struct s_sound_globals_classes_view
{
	byte unknown00[4];
	s_sound_class *classes;
};

/* how a sound class is ducked while an ambience plays (16 bytes): its gain
   in decibels (real bits), faded in, held, and faded out */
struct s_sound_class_ducking
{
	long gain;
	real fade_in_time;
	real hold_time;
	real fade_out_time;
};

/* a sound class of the sound classes tag (function_221810, 0x5c bytes) */
struct s_sound_promotion_view
{
	short definition_voice_limit;
	short source_voice_limit;
	long preemption_time;
	byte unknown08[4];
	short priority;
	byte unknown0e[0xa];
	real minimum_distance;
	real maximum_distance;
	long gain_lower;
	long gain_upper;
	s_sound_class_ducking duckings[2];
	byte unknown48[0x14];
};

struct s_unknown_5c;
s_unknown_5c *function_221810(short index);

static inline s_sound_class *function_xaa8231(short class_index)
{
	return &((s_sound_globals_classes_view *)g_51ebd4)->classes[class_index];
}

/* gains in decibels are kept as real bits in longs */
static inline long decibels_add(long a, long b)
{
	real result = *(real *)&a + *(real *)&b;

	return *(long *)&result;
}

static inline long decibels_interpolate(long a, long b, real t)
{
	real result = (*(real *)&b - *(real *)&a) * t + *(real *)&a;

	return *(long *)&result;
}

static inline real real_decompress_angle(long value)
{
	if (value == 0)
		return 0.f;
	if (value >= 0xffff)
		return k_pi;
	return ((0xffff - value) * 0.f + value * k_pi) * (1.f / 65535.f);
}

// @retail 0x124f90
bool function_124f90(s_sound const *sound)
{
	return TEST_FIELD_BIT(sound->flag8_1) || TEST_FIELD_BIT(sound->flag8_3) || TEST_FIELD_BIT(sound->flag0a_11);
}

// @retail 0x124fc0
real sound_get_minimum_distance(s_sound const *sound, long definition_index)
{
	s_sound_definition *definition;

	if (TEST_FIELD_BIT(sound->override_minimum_distance))
		return sound->minimum_distance;
	definition = sound_definition_get(definition_index);
	if (definition->flags & 0x400)
		return ((s_sound_promotion_view *)function_221810(definition->promotion_index))->minimum_distance;
	return function_xaa8231(definition->class_index)->minimum_distance;
}

// @retail 0x125010
real sound_get_maximum_distance(s_sound const *sound, long definition_index)
{
	s_sound_definition *definition;

	if (TEST_FIELD_BIT(sound->override_maximum_distance))
		return sound->maximum_distance;
	definition = sound_definition_get(definition_index);
	if (definition->flags & 0x800)
		return ((s_sound_promotion_view *)function_221810(definition->promotion_index))->maximum_distance;
	return function_xaa8231(definition->class_index)->maximum_distance;
}

// @retail 0x125060
long function_125060(s_sound const *sound, long definition_index)
{
	if (TEST_FIELD_BIT(sound->override_inner_cone_angle) ||
		TEST_FIELD_BIT(sound->override_outer_cone_angle) ||
		!(sound_definition_get(definition_index)->flags & 0x1000))
	{
		return 1;
	}
	return 0;
}

// @retail 0x1250a0
real sound_get_inner_cone_angle(s_sound const *sound, long definition_index)
{
	if (TEST_FIELD_BIT(sound->override_inner_cone_angle))
		return real_decompress_angle(sound->inner_cone_angle);
	return function_xaa8231(sound_definition_get(definition_index)->class_index)->inner_cone_angle;
}

// @retail 0x125120
real sound_get_outer_cone_angle(s_sound const *sound, long definition_index)
{
	if (TEST_FIELD_BIT(sound->override_outer_cone_angle))
		return real_decompress_angle(sound->outer_cone_angle);
	return function_xaa8231(sound_definition_get(definition_index)->class_index)->outer_cone_angle;
}

// @retail 0x1251a0
long sound_get_outer_cone_gain(s_sound const *sound, long definition_index)
{
	if (TEST_FIELD_BIT(sound->override_outer_cone_gain))
		return sound->outer_cone_gain;
	return function_xaa8231(sound_definition_get(definition_index)->class_index)->outer_cone_gain;
}

// @retail 0x125260
long sound_definition_random_gain(s_sound_definition const *definition)
{
	s_sound_class *sound_class = function_xaa8231(definition->class_index);

	return decibels_add(decibels_interpolate(0, sound_class->gain_variance, function_x82e52f(&g_4e7408->seed, __FILE__, __LINE__)), sound_class->gain_base);
}

// @retail 0x1252f0
real sound_definition_random_pitch(s_sound_definition const *definition, dword *seed)
{
	s_sound_class *sound_class = function_xaa8231(definition->class_index);

	return function_259d0(seed, __FILE__, __LINE__, (real)sound_class->pitch_lower, (real)sound_class->pitch_upper);
}

/* ---- the sound system's globals ---- */

struct s_4e6380;
extern s_4e6380 *g_4e6380;
extern s_record_pool *g_4e637c;
struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;

/* an environment the sound system fades between (0x1c bytes): how long it
   takes, how far it has faded in, which it is and its settings */
struct s_sound_environment
{
	real transition_time;
	real fade;
	long index;
	real unknown0c;
	real unknown10;
	real unknown14;
	real unknown18;
};

/* a listener of the sound system (0x48 bytes) */
struct s_sound_listener
{
	long leaf_index;
	short cluster_index;
	bool active;
	byte unknown07;
	real velocity_scale;
	vector3f forward;
	vector3f left;
	vector3f up;
	point3f position;
	vector3f velocity;
};

/* the sound system's state, as these functions read it */
struct s_sound_system_view
{
	byte unknown00[0x70];
	dword channel_bits[2];
	bool initialized;
	bool hardware_available;
	bool enabled;
	bool unknown7b;
	byte unknown7c;
	bool changing_pause;
	byte unknown7e[2];
	dword field_14_2;
	long time;
	s_sound_listener listeners[4];
	s_sound_environment environments[2];
	real elapsed_time;
	real master_fade;
	long master_fade_delay;
	vector3f master_fade_times;	/* delay, fade out and fade in, in seconds */
	long ambience_index;
	real ambience_fade;
	long previous_ambience_index;
	real previous_ambience_fade;
	short voice_count;
};

struct s_sound_channel_flags
{
	byte unknown00[3];
	byte flags;
};

/* 0x34 bytes */
struct s_sound_channel
{
	byte unknown00[0xc];
	s_sound_channel_flags state;
	byte unknown10[0x24];
};

struct s_sound_permutation;

/* 0x24 bytes */
struct s_sound_voice
{
	long sound_index;
	long driver_voice_index;
	byte definition_type;
	bool stream_reset;
	byte unknown0a;
	byte unknown0b;
	short channel_index;
	short unknown0e;
	real unknown10;
	byte unknown14[4];
	short chunk_index;
	short next_chunk_index;
	s_sound_permutation const *permutation;
	s_sound_permutation const *next_permutation;
};

struct s_sound_reference_holder
{
	byte unknown00[8];
	long reference_index;
};

/* a playing sound's reference count, in the 0x502104 array */
struct s_sound_reference
{
	byte unknown00[4];
	byte reference_count;
	byte unknown05[0xb];
};

struct s_sound_location_source
{
	byte unknown00[2];
	char type;
	char spatialization : 4;
	char unknown03 : 4;
	byte unknown04[8];
	point3f position;
	byte unknown18[0x18];
	real height;
};

struct s_sound_definition_flags
{
	byte unknown00[4];
	byte type;
	byte format;
	byte unknown06[2];
	short pitch_range_index;
	bool has_pitch_ranges;
};

struct s_sound_globals_entries_view
{
	byte unknown00[0x24];
	struct
	{
		byte unknown00[0xa];
		short count;
	} *pitch_ranges;
};

long g_4e6374;
s_sound_voice *g_4e6378;
void *g_502110;
s_record_pool *g_502114;
void *g_51ebd8;
void *g_51ebdc;
void *g_51ebe0;
void *g_47f0e4;

void function_2186b0(void);
void function_21eae0(void);

#define SOUND_SYSTEM ((s_sound_system_view *)g_4e6380)

// @retail 0x125600
void function_125600(void)
{
	if (--g_4e6374 == 0)
	{
		function_2186b0();
		function_21eae0();
		if (g_51ebe0)
			g_51ebe0 = NULL;
		if (g_502114)
		{
			g_502114->valid = false;
			g_502114 = NULL;
		}
		if (g_502110)
			g_502110 = NULL;
		if (g_51ebd8)
			g_51ebd8 = NULL;
		if (g_51ebdc)
			g_51ebdc = NULL;
		if (g_4e6380)
			g_4e6380 = NULL;
		if (g_4e637c)
		{
			g_4e637c->valid = false;
			g_4e637c = NULL;
		}
		if (g_4e6378)
			g_4e6378 = NULL;
	}
}

// @retail 0x125a60
long sound_system_available(void)
{
	if (SOUND_SYSTEM->initialized && SOUND_SYSTEM->hardware_available && SOUND_SYSTEM->enabled)
		return 1;
	return 0;
}

// @retail 0x125ef0
void sound_reference_release(s_sound_reference_holder *holder)
{
	s_sound_reference *reference = (s_sound_reference *)g_502104->data + (holder->reference_index & 0xffff);

	reference->reference_count--;
}

// @retail 0x126bd0
long function_126bd0(long definition_index)
{
	s_sound_definition_flags *definition = (s_sound_definition_flags *)g_4e3b44[definition_index & 0xffff].bytes;

	if ((definition->format == 1 && definition->type != 2) || (definition->type == 2 && g_47f0e4))
	{
		if (definition->has_pitch_ranges &&
			((s_sound_globals_entries_view *)g_51ebd4)->pitch_ranges[definition->pitch_range_index].count != 0)
		{
			return 1;
		}
	}
	return 0;
}

// @retail 0x1272c0
void sound_set_ambience(long ambience_index)
{
	s_sound_system_view *sound_system = SOUND_SYSTEM;

	if (ambience_index != sound_system->ambience_index)
	{
		if (ambience_index != NONE && ambience_index == sound_system->previous_ambience_index)
		{
			sound_system->ambience_index = sound_system->previous_ambience_index;
			sound_system->ambience_fade = sound_system->previous_ambience_fade;
			sound_system->previous_ambience_fade = 0.f;
		}
		else
		{
			sound_system->previous_ambience_index = sound_system->ambience_index;
			sound_system->ambience_index = ambience_index;
			sound_system->ambience_fade = 0.f;
			sound_system->previous_ambience_fade = 0.f;
		}
	}
}

// @retail 0x12a420
void function_12a420(short voice_index)
{
	s_sound_channel_flags *state = &((s_sound_channel *)g_51ebe4)[g_4e6378[voice_index].channel_index].state;

	state->flags |= 8;
}

// @retail 0x12abe0
void sound_source_get_position(s_sound_location_source const *source, long listener_index, point3f *position)
{
	switch (source->type)
	{
	case 0:
		*position = source->position;
		break;
	case 1:
		position->x = source->position.x;
		position->y = source->position.y;
		position->z = SOUND_SYSTEM->listeners[listener_index].position.z;
		break;
	}
}

/* how far a sound is from a listener: none for a sound without a position,
   the square of the distance for one placed in the world (on the ground
   plane, unless the listener is out of its height range), the distance from
   the listener for one attached to it */
real magnitude3d(vector3f const *v);

// @retail 0x127e20
real sound_source_get_listener_distance(s_sound_location_source const *source, long listener_index)
{
	real result;

	switch (source->spatialization)
	{
	case 0:
		result = 0.0f;
		break;
	case 1:
	{
		s_sound_listener const *listeners = SOUND_SYSTEM->listeners;
		real dz = listeners[listener_index].position.z - source->position.z;
		s_sound_listener const *listener = &listeners[listener_index];

		switch (source->type)
		{
		case 0:
		{
			real dx = listener->position.x - source->position.x;
			real dy = listener->position.y - source->position.y;

			result = dz * dz;
			result += dy * dy;
			result += dx * dx;
			break;
		}
		case 1:
		{
			real clamped = 0.0f;

			if (!(0.0f > dz))
			{
				clamped = dz > source->height ? source->height : dz;
			}
			if (clamped == dz)
			{
				real dy = source->position.y - listener->position.y;
				real dx = source->position.x - listener->position.x;

				result = dx * dx + dy * dy;
			}
			else
			{
				result = FLT_MAX;
			}
			break;
		}
		default:
			__assume(0);
		}
		break;
	}
	default:
		result = magnitude3d((vector3f const *)&source->position);
		break;
	}
	return result;
}

// @retail 0x12af90
void sound_channel_clear(short channel_index)
{
	if (channel_index != NONE)
		SOUND_SYSTEM->channel_bits[channel_index >> 5] &= ~(1 << (channel_index & 31));
}

// @retail 0x12afc0
void bit_vector_fill(dword *vector, long count, byte value)
{
	memset(vector, value, ((count + 31) >> 5) * sizeof(dword));
}

struct s_sound_promotion_flags
{
	byte unknown00[0xa];
	word disabled : 1;
	word unknown0a : 15;
};

struct s_sound_globals_tables_view
{
	byte unknown00[0x24];
	struct
	{
		byte unknown00[8];
		short first_permutation;
		short count;
	} *pitch_ranges;
	byte unknown28[4];
	struct
	{
		byte unknown00[0xc];
		short first_chunk;
		byte unknown0e[2];
	} *permutations;
	byte unknown30[0x14];
	struct
	{
		byte unknown00[8];
		long reference_index;
	} *chunks;
};

struct s_sound_system_channels_view
{
	dword free_bits[2];
	byte unknown08[0x38];
	long first_channel[3];
	long last_channel[3];
	dword available_bits[2];
	dword streaming_bits[2];
	byte unknown68[8];
	dword used_bits[2];
	byte unknown78[0x192];
	short channel_count;
};

// @retail 0x126960
void function_126960(s_sound_playback *sound)
{
	if (TEST_FIELD_BIT(sound->holds_reference))
	{
		long arg_58ecd0 = sound_definition_get(sound->definition_index)->pitch_range_base + sound->pitch_range_index;
		long permutation = ((s_sound_globals_tables_view *)g_51ebd4)->pitch_ranges[arg_58ecd0].first_permutation + sound->permutation_index;
		long chunk = ((s_sound_globals_tables_view *)g_51ebd4)->permutations[permutation].first_chunk + sound->chunk_index;
		s_sound_reference *reference = (s_sound_reference *)g_502104->data + (((s_sound_globals_tables_view *)g_51ebd4)->chunks[chunk].reference_index & 0xffff);

		reference->reference_count--;
		sound->holds_reference = false;
	}
}

// @retail 0x129fe0
bool sound_voice_promotion_enabled(short voice_index)
{
	long sound_index = g_4e6378[voice_index].sound_index;
	bool result = true;

	if (sound_index != NONE)
	{
		s_sound_playback *sound = (s_sound_playback *)g_4e637c->data + (sound_index & 0xffff);
		char promotion_index = sound_definition_get(sound->definition_index)->promotion_index;

		if ((promotion_index < 0 ? 0 : (promotion_index > 0x35 ? 0x35 : promotion_index)) == promotion_index)
			result = !TEST_FIELD_BIT(((s_sound_promotion_flags *)function_221810(promotion_index))->disabled);
	}
	return result;
}

// @retail 0x12af10
short sound_channel_allocate(void)
{
	s_sound_system_channels_view *sound_system = (s_sound_system_channels_view *)g_4e6380;
	short channel_count = sound_system->channel_count;
	short result = NONE;

	for (short i = 0; i < channel_count; i++)
	{
		if ((sound_system->available_bits[i >> 5] & (1 << (i & 31))) && !(sound_system->used_bits[i >> 5] & (1 << (i & 31))))
		{
			sound_system->used_bits[i >> 5] |= 1 << (i & 31);
			return i;
		}
	}
	return result;
}

/* the sound driver's streams, one per channel (g_51ebe4) */
struct s_sound_driver_streams_view
{
	byte unknown00[0xc];
	s_sound_stream streams[1];
};

#define SOUND_DRIVER_STREAMS ((s_sound_driver_streams_view *)g_51ebe4)

/* a permutation of a pitch range (16 bytes) */
struct s_sound_permutation
{
	byte unknown00[0xc];
	short first_chunk;
	byte unknown0e[2];
};

/* the tables of the sound globals, as the chunk lookups read them */
struct s_sound_globals_chunks_view
{
	byte unknown00[0x24];
	struct
	{
		byte unknown00[8];
		short first_permutation;
		short count;
	} *pitch_ranges;
	byte unknown28[4];
	s_sound_permutation *permutations;
	byte unknown30[0x14];
	s_sound_chunk *chunks;
};

#define SOUND_GLOBALS_CHUNKS ((s_sound_globals_chunks_view *)g_51ebd4)

/* what the sound cache request (function_218850) returns */
union s_sound_cache_request_result
{
	dword value;
	struct
	{
		dword loading : 1;
		dword loaded : 1;
		dword locked : 1;
		dword unknown03 : 29;
	};
};

/* requests the first chunk of a sound's first permutation */
// @retail 0x125f10
void sound_definition_request_first_chunk(long definition_index)
{
	if (definition_index != NONE)
	{
		s_sound_definition *definition = sound_definition_get(definition_index);

		if (definition->pitch_range_count)
		{
			if (SOUND_GLOBALS_CHUNKS->pitch_ranges[definition->pitch_range_base].count != 0)
			{
				long permutation = SOUND_GLOBALS_CHUNKS->pitch_ranges[definition->pitch_range_base].first_permutation;
				long chunk = SOUND_GLOBALS_CHUNKS->permutations[permutation].first_chunk;

				function_218850(definition_index, &SOUND_GLOBALS_CHUNKS->chunks[chunk], 2);
			}
		}
	}
}

// @retail 0x1268e0
void sound_playback_acquire_reference(s_sound_playback *sound)
{
	if (!TEST_FIELD_BIT(sound->holds_reference))
	{
		s_sound_playback_flags *flags = (s_sound_playback_flags *)&sound->flags;
		long arg_58ecd0;
		long permutation;
		long chunk;

		flags->holds_reference = true;
		arg_58ecd0 = sound_definition_get(sound->definition_index)->pitch_range_base + sound->pitch_range_index;
		permutation = SOUND_GLOBALS_CHUNKS->pitch_ranges[arg_58ecd0].first_permutation + sound->permutation_index;
		chunk = SOUND_GLOBALS_CHUNKS->permutations[permutation].first_chunk + sound->chunk_index;
		function_218850(NONE, &SOUND_GLOBALS_CHUNKS->chunks[chunk], 4);
	}
}

// @retail 0x1286b0
void sound_playback_set_chunk(long sound_index, long definition_index, char pitch_range_index, char permutation_index, short chunk_index)
{
	s_sound_playback *sound = (s_sound_playback *)g_4e637c->data + (sound_index & 0xffff);

	function_126960(sound);
	sound->definition_index = definition_index;
	sound->pitch_range_index = pitch_range_index;
	sound->permutation_index = permutation_index;
	sound->chunk_index = chunk_index;
}

/* queues a chunk on a voice's stream: the first one, or the one after it */
// @retail 0x129f20
void sound_voice_queue_chunk(short voice_index, s_sound_permutation const *permutation, short chunk_index)
{
	s_sound_voice *voice = &g_4e6378[voice_index];
	s_sound_chunk *chunks = SOUND_GLOBALS_CHUNKS->chunks;
	s_sound_chunk *chunk = &chunks[permutation->first_chunk + chunk_index];

	if (voice->next_permutation)
	{
		s_sound_chunk *next_chunk = &chunks[voice->next_permutation->first_chunk + voice->next_chunk_index];
		s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(next_chunk->cache_index);

		entry->lock_count--;
		voice->next_chunk_index = NONE;
	}
	sound_stream_add_chunk(&SOUND_DRIVER_STREAMS->streams[voice->channel_index], chunk);
	function_218850(NONE, chunk, 4);
	if (voice->permutation)
	{
		voice->next_chunk_index = chunk_index;
		voice->next_permutation = permutation;
	}
	else
	{
		voice->chunk_index = chunk_index;
		voice->permutation = permutation;
		voice->unknown10 = 0.0f;
	}
}

// @retail 0x12a060
void sound_voice_reset_stream(short voice_index)
{
	s_sound_voice *voice = &g_4e6378[voice_index];

	if (voice->channel_index != NONE && !voice->stream_reset && sound_voice_promotion_enabled(voice_index))
	{
		sound_stream_reset(&SOUND_DRIVER_STREAMS->streams[voice->channel_index]);
		voice->stream_reset = true;
	}
}

long sound_format_duration_to_bytes(long arg_da1d74, long encoding, long compression, real duration);
long sound_permutation_chunks_size(long chunk_count, s_sound_permutation const *permutation);
void function_21f5d0(long channel_index, long offset);

/* restarts a voice's reset stream where it had played to: its position in
   the current chunk and its chunks queued again */
// @retail 0x12a0b0
void sound_voice_restart_stream(short voice_index)
{
	s_sound_voice *voice = &g_4e6378[voice_index];

	if (voice->channel_index != NONE && voice->stream_reset)
	{
		if (voice->permutation)
		{
			s_sound_playback *sound = SOUND_PLAYBACK_GET(voice->sound_index);
			s_sound_definition *definition = sound_definition_get(sound->definition_index);
			long arg_da1d74 = (char)definition->unknown03;
			long offset = sound_format_duration_to_bytes(arg_da1d74, (char)definition->format, (char)definition->type, voice->unknown10);

			offset -= sound_permutation_chunks_size(voice->chunk_index, voice->permutation);
			function_21f5d0(voice->channel_index, offset);
			sound_stream_add_chunk(&SOUND_DRIVER_STREAMS->streams[voice->channel_index], &SOUND_GLOBALS_CHUNKS->chunks[voice->permutation->first_chunk + voice->chunk_index]);
			if (voice->next_permutation)
				sound_stream_add_chunk(&SOUND_DRIVER_STREAMS->streams[voice->channel_index], &SOUND_GLOBALS_CHUNKS->chunks[voice->next_permutation->first_chunk + voice->next_chunk_index]);
		}
		voice->stream_reset = false;
	}
}

struct s_permutation_group;
long function_219290(short index, s_permutation_group *group);
real function_21f650(long channel_index, long mode);

/* moves a voice on to its queued chunk once its stream has played the
   current one, and advances its play time */
// @retail 0x12a450
void sound_voice_update_chunks(short voice_index)
{
	s_sound_voice *voice = &g_4e6378[voice_index];

	if (voice->channel_index != NONE && !voice->stream_reset && voice->permutation)
	{
		short queued = SOUND_DRIVER_STREAMS->streams[voice->channel_index].state;

		if (voice->next_permutation && queued < 3)
		{
			s_sound_globals_chunks_view *tables = SOUND_GLOBALS_CHUNKS;
			s_sound_cache_request_result result;

			sound_reference_release((s_sound_reference_holder *)&tables->chunks[voice->permutation->first_chunk + voice->chunk_index]);
			if ((short)function_219290(voice->chunk_index, (s_permutation_group *)voice->permutation) == NONE)
				voice->unknown10 = 0.0f;
			voice->permutation = voice->next_permutation;
			voice->chunk_index = voice->next_chunk_index;
			voice->next_permutation = NULL;
			voice->next_chunk_index = NONE;
			result.value = function_218850(NONE, &tables->chunks[voice->permutation->first_chunk + voice->chunk_index], 0);
			if (!TEST_FIELD_BIT(result.loaded))
				queued = 0;
		}
		if (voice->permutation && queued < 2)
		{
			sound_reference_release((s_sound_reference_holder *)&SOUND_GLOBALS_CHUNKS->chunks[voice->permutation->first_chunk + voice->chunk_index]);
			voice->permutation = NULL;
			voice->chunk_index = NONE;
		}
		if (SOUND_DRIVER_STREAMS->streams[voice->channel_index].state > 0)
		{
			s_sound_playback *sound = (s_sound_playback *)g_4e637c->data + (voice->sound_index & 0xffff);
			s_sound_definition *definition = sound_definition_get(sound->definition_index);

			voice->unknown10 += function_21f650(voice->channel_index, (char)definition->unknown03) * SOUND_SYSTEM->elapsed_time;
		}
	}
}

/* the sound mix tag the globals name (0x14), as the sound system reads it */
struct s_globals_sound_view
{
	byte unknown00[0x14];
	long sound_mix_index;
};

struct s_sound_mix_view
{
	byte unknown00[0x18];
	dword levels_a[2];
	dword levels_b[2];
	byte settings[0x20];
	vector3f unknown48;
};

void __stdcall function_21f6d0(dword const *levels_a, dword const *levels_b, void const *settings);

/* applies the globals' sound mix to the sound driver */
// @retail 0x125df0
void sound_mix_apply(void)
{
	s_tag_header_globals *globals = g_4e034c;
	s_globals_sound_view *header = (s_globals_sound_view *)(globals->header ? globals->header_alt : NULL);
	long sound_mix_index = header->sound_mix_index;

	if (sound_mix_index != NONE)
	{
		s_sound_mix_view *sound_mix = (s_sound_mix_view *)g_4e3b44[sound_mix_index & 0xffff].bytes;

		function_21f6d0(sound_mix->levels_a, sound_mix->levels_b, sound_mix->settings);
		SOUND_SYSTEM->master_fade_times.i = sound_mix->unknown48.i;
		SOUND_SYSTEM->master_fade_times.j = sound_mix->unknown48.j;
		SOUND_SYSTEM->master_fade_times.k = sound_mix->unknown48.k;
	}
}

/* moves every voice on to its queued chunk */
// @retail 0x1290d0
void sound_voices_update_chunks(void)
{
	for (short i = 0; i < SOUND_SYSTEM->voice_count; i++)
		sound_voice_update_chunks(i);
}

void sound_voice_release(long voice_index);

/* frees a voice: unlocks its chunks, flushes its stream and releases its
   driver voice */
// @retail 0x12a5d0
void sound_voice_free(short voice_index)
{
	s_sound_voice *voice = &g_4e6378[voice_index];
	s_sound_chunk *chunks;
	s_sound_cache_entry *entry;

	voice->sound_index = NONE;
	if (voice->next_permutation)
	{
		chunks = SOUND_GLOBALS_CHUNKS->chunks;
		entry = SOUND_CACHE_ENTRY(chunks[voice->next_permutation->first_chunk + voice->next_chunk_index].cache_index);
		entry->lock_count--;
		voice->next_permutation = NULL;
	}
	if (voice->permutation)
	{
		chunks = SOUND_GLOBALS_CHUNKS->chunks;
		entry = SOUND_CACHE_ENTRY(chunks[voice->permutation->first_chunk + voice->chunk_index].cache_index);
		entry->lock_count--;
		voice->permutation = NULL;
	}
	if (voice->channel_index != NONE)
	{
		short channel_index;

		sound_stream_flush(&SOUND_DRIVER_STREAMS->streams[voice->channel_index]);
		channel_index = voice->channel_index;
		((s_sound_system_channels_view *)g_4e6380)->streaming_bits[channel_index >> 5] &= ~(1 << (channel_index & 31));
	}
	if (voice->driver_voice_index != NONE)
		sound_voice_release(voice->driver_voice_index);
	voice->unknown0a = 0;
	voice->unknown0b = 0;
	voice->stream_reset = false;
	voice->channel_index = NONE;
	voice->unknown0e = NONE;
	voice->driver_voice_index = NONE;
}

/* ---- how often a sound may start (the sound globals' rate limits) ---- */

/* advances a sound's rate limit to the current time; true while the sound
   must not start */
// @retail 0x126ec0
long sound_definition_rate_limited(long definition_index, long *stage_index)
{
	long result = 0;
	s_sound_definition *definition = sound_definition_get(definition_index);
	s_sound_rate_limit *limit = sound_rate_limit_get(definition->rate_limit_index);
	long i = 0;

	if (limit)
	{
		s_sound_system_view *sound_system = SOUND_SYSTEM;
		long elapsed;

		if (limit->end_time <= sound_system->time)
		{
			limit->field_0 = NONE;
			limit->end_time = 0;
		}
		elapsed = sound_system->time - limit->field_14_2;
		for (i = 0; i < limit->counter_count; i++)
		{
			long *counter = &limit->counters[i];

			*counter -= elapsed;
			*counter = *counter < 0 ? 0 : *counter;
		}
		for (i = 0; i < limit->stage_count; i++)
		{
			long *counter = &limit->counters[i];
			s_sound_rate_limit_stage *stage = &limit->stages[i];

			*counter += stage->increment;
			if (stage->count <= 0 || *counter < stage->threshold || i >= limit->stage_count - 1)
				break;
			*counter = 0;
		}
		*stage_index = i;
		result = i <= limit->field_0 && sound_system->time < limit->end_time;
		if (i >= limit->field_0 && !result)
		{
			limit->field_0 = i;
			limit->end_time = (long)(limit->stages[i].duration * 1000.0f + sound_system->time);
		}
		limit->field_14_2 = sound_system->time;
	}
	else
	{
		*stage_index = NONE;
	}
	return result;
}

/* asks a playing sound's source where it is; a sound whose source is gone
   keeps playing only while it may be heard */

/* the third caller of 0x127f10, 0x21d5a0 (sound effects, not decompiled
   yet), passes the flags as the address of a local: retail passes them on
   the stack and the source in edi */
// @retail 0x127f10
bool sound_playback_update_source(long sound_index, s_sound_source_callbacks const *source, s_sound_playback_flags *flags)
{
	s_sound_playback *sound = SOUND_PLAYBACK_GET(sound_index);
	long definition_index = sound->definition_index;
	s_sound_definition *definition = sound_definition_get(definition_index);

	if (source)
	{
		if (source->update(sound->object_index, definition_index, &sound->marker, &sound->location))
			return true;
		if (!((1 << SOUND_PLAYBACK_GET(sound_index)->state) & 0x1e) &&
			!function_124f90((s_sound const *)function_221810(definition->promotion_index)))
		{
			flags->source_updated = true;
			return true;
		}
		flags->source_updated = true;
		return false;
	}
	return true;
}
// @retail 0x127fc0
void sound_playback_update_location(long sound_index)
{
	s_sound_playback *sound = SOUND_PLAYBACK_GET(sound_index);
	s_sound_playback_flags *flags = (s_sound_playback_flags *)&sound->flags;

	if (!TEST_FIELD_BIT(flags->flag0) && !TEST_FIELD_BIT(flags->source_updated))
	{
		s_sound_source_callbacks const *source = sound->source;

		if (source && source->update && (sound->start_time < SOUND_SYSTEM->time || SOUND_SYSTEM->unknown7b))
			sound_playback_update_source(sound_index, source, flags);
	}
}
/* ---- gains in decibels, held as real bits ---- */

/* a sound class's volume fade (g_502118, unknown_221490.cpp) */
struct s_sound_class_fade
{
	dword target;
	dword current;
	real time;
	byte flags;
	byte unknownd[3];
};

extern s_sound_class_fade *g_502118;

long sound_definition_gain_lower(s_sound_definition const *definition);
long sound_definition_gain_upper(s_sound_definition const *definition);
real function_2195f0(real decibels);
long function_2197f0(real gain);

/* a sound class's gain in decibels: its fade, ducked by the current and the
   previous ambience */
// @retail 0x127010
long function_127010(short class_index)
{
	s_sound_class_fade *fade = &g_502118[class_index];
	long gain = fade->current;

	if (!TEST_FIELD_BIT(fade->flags & 1))
	{
		s_sound_promotion_view *sound_class = (s_sound_promotion_view *)function_221810(class_index);
		s_sound_system_view *sound_system = SOUND_SYSTEM;

		if (sound_system->previous_ambience_index != NONE && sound_system->ambience_index == sound_system->previous_ambience_index)
		{
			real elapsed = sound_system->ambience_fade - sound_system->previous_ambience_fade;
			s_sound_class_ducking *ducking = &sound_class->duckings[sound_system->ambience_index];

			if (elapsed > ducking->hold_time)
			{
				if (ducking->fade_in_time >= 0.001f)
				{
					real t;

					if (ducking->fade_out_time >= 0.001f && ducking->fade_out_time + ducking->hold_time > elapsed)
					{
						real inverse = 1.0f / ducking->fade_in_time;
						real time = sound_system->ambience_fade - elapsed + (elapsed - ducking->hold_time) * inverse * ducking->fade_out_time;

						t = 1.0f > time * inverse ? time / ducking->fade_in_time : 1.0f;
					}
					else
					{
						real time = (sound_system->ambience_fade - elapsed) / ducking->fade_in_time;

						t = 1.0f > time ? time : 1.0f;
					}
					return decibels_add(gain, decibels_interpolate(0, ducking->gain, t));
				}
			}
			else
			{
				return decibels_add(ducking->gain, gain);
			}
		}
		else
		{
			if (sound_system->previous_ambience_index != NONE)
			{
				s_sound_class_ducking *ducking = &sound_class->duckings[sound_system->previous_ambience_index];

				if (ducking->fade_out_time >= 0.001f)
				{
					real time = sound_system->previous_ambience_fade - ducking->hold_time / ducking->fade_out_time;
					real t = 0.0f > time ? 0.0f : (time > 1.0f ? 1.0f : time);

					gain = decibels_add(decibels_interpolate(ducking->gain, 0, t), gain);
				}
			}
			if (sound_system->ambience_index != NONE)
			{
				s_sound_class_ducking *ducking = &sound_class->duckings[sound_system->ambience_index];

				if (ducking->fade_in_time >= 0.001f)
				{
					real time = sound_system->ambience_fade / ducking->fade_in_time;
					real t = 1.0f > time ? time : 1.0f;

					return decibels_add(decibels_interpolate(0, ducking->gain, t), gain);
				}
			}
		}
	}
	return gain;
}

/* a sound's gain in decibels: between its definition's bounds, with its
   class's and the caller's */
// @retail 0x1251e0
long function_1251e0(void const *definition_pointer, long gain, real interpolation)
{
	s_sound_definition const *definition = (s_sound_definition const *)definition_pointer;
	long upper_decibels = sound_definition_gain_upper(definition);
	long lower_decibels = sound_definition_gain_lower(definition);
	real lower = function_2195f0(*(real *)&lower_decibels);
	real upper = function_2195f0(*(real *)&upper_decibels);
	long decibels = function_2197f0((upper - lower) * interpolation + lower);
	long class_gain = function_127010(definition->promotion_index);

	return decibels_add(decibels, decibels_add(class_gain, gain));
}

/* requests the chunk a playing sound is at; true once it is loaded. Retail
   keeps the sound on the stack (ret 4): its address is taken */
struct s_looping_track_sound;

// @retail 0x125e60
long __stdcall function_125e60(s_looping_track_sound *track)
{
	s_looping_track_sound *const *reference = &track;
	s_sound_playback *sound = (s_sound_playback *)*reference;
	long arg_58ecd0 = sound_definition_get(sound->definition_index)->pitch_range_base + sound->pitch_range_index;
	long permutation = SOUND_GLOBALS_CHUNKS->pitch_ranges[arg_58ecd0].first_permutation + sound->permutation_index;
	long chunk = SOUND_GLOBALS_CHUNKS->permutations[permutation].first_chunk + sound->chunk_index;
	s_sound_cache_request_result result;

	result.value = function_218850(sound->definition_index, &SOUND_GLOBALS_CHUNKS->chunks[chunk], 2);
	if (result.value & 3)
		sound_playback_acquire_reference(sound);
	return result.loaded;
}

#define MAXIMUM(a, b) ((a) > (b) ? (a) : (b))

real function_12aff0(real a, real b, real c, bool flag);

/* how loud a sound is at a distance: 1 inside its minimum distance, falling
   off with the distance and to nothing at its maximum distance */
// @retail 0x12ac20
real sound_get_distance_gain(long definition_index, s_sound const *sound, real distance)
{
	real minimum_distance = MAXIMUM(sound_get_minimum_distance(sound, definition_index), 0.001f);
	real maximum_distance = sound_get_maximum_distance(sound, definition_index);

	return minimum_distance / MAXIMUM(minimum_distance, distance) * function_12aff0(maximum_distance, minimum_distance, distance, false);
}

/* a looping sound's controller (g_51ebd8, unknown_12a1b0.cpp), as a
   playing sound's deletion reads it (0x1c bytes) */
struct s_sound_controller_view
{
	byte unknown00[3];
	byte playing_count;
	long unknown04;
	byte unknown08[0x14];
};

void looping_sound_controller_release(long index);

/* deletes a playing sound, letting go of its looping sound's controller */
// @retail 0x127390
void sound_playback_delete(long sound_index)
{
	s_sound_playback *sound = SOUND_PLAYBACK_GET(sound_index);

	if (sound->effect_index != NONE)
	{
		s_sound_controller_view *controller = (s_sound_controller_view *)((s_record_pool *)g_51ebd8)->data + (sound->effect_index & 0xffff);

		if (sound->unknown03 == NONE && controller->unknown04 != NONE)
			controller->playing_count = (controller->playing_count - 1) & 0x7f;
		looping_sound_controller_release(sound->effect_index);
	}
	record_pool_release(g_4e637c, sound_index);
}

/* stops a playing sound: frees its voice, lets go of its chunk, tells its
   source why it stopped and deletes it */
// @retail 0x127320
void __stdcall function_127320(long sound_index, long reason)
{
	s_sound_playback *sound = SOUND_PLAYBACK_GET(sound_index);

	if (sound->value_ac != NONE)
	{
		sound_voice_free(sound->value_ac);
		sound->value_ac = NONE;
	}
	function_126960(sound);
	if (sound->source && sound->source->stop)
		sound->source->stop(sound->object_index, sound_index, reason);
	sound_playback_delete(sound_index);
}

static inline short sound_definition_priority(s_sound_definition const *definition)
{
	return ((s_sound_promotion_view *)sound_class_definition_get(definition->promotion_index))->priority;
}

/* whether a playing sound should give way to another: the other's class has
   a higher priority, or the same sound plays it louder, or this one is
   farther than a distance */
// @retail 0x128b90
bool function_128b90(long sound_index, long other_index, real distance)
{
	s_sound_playback *sound = SOUND_PLAYBACK_GET(sound_index);
	s_sound_playback *other = SOUND_PLAYBACK_GET(other_index);
	s_sound_definition *definition = sound_definition_get(sound->definition_index);
	long other_priority = sound_definition_priority(sound_definition_get(other->definition_index));
	long priority = sound_definition_priority(definition);

	if (other_priority > priority)
		return true;
	if (other_priority == priority)
	{
		if (other->definition_index == sound->definition_index)
		{
			if (other->value_a0 > sound->value_a0)
				return true;
			if (other->value_a0 != sound->value_a0)
				return false;
		}
		if (sound_source_get_listener_distance((s_sound_location_source const *)&sound->location, sound->listener_index) > distance)
			return true;
	}
	return false;
}

/* finds a voice for a playing sound: a free one of its definition's type,
   or the one whose sound should give way to it the most (reason 9) */
// @retail 0x128920
short sound_voice_find(long sound_index, long *reason)
{
	s_sound_playback *sound = SOUND_PLAYBACK_GET(sound_index);
	s_sound_definition *definition = sound_definition_get(sound->definition_index);
	real distance = sound_source_get_listener_distance((s_sound_location_source const *)&sound->location, sound->listener_index);
	short result = NONE;
	long result_sound_index = NONE;
	real result_distance;

	*reason = 0;
	for (short i = 0; i < SOUND_SYSTEM->voice_count; i++)
	{
		s_sound_voice *voice = &g_4e6378[i];

		if (definition->type == voice->definition_type)
		{
			if (voice->sound_index == NONE)
				return i;
			if (function_128b90(voice->sound_index, sound_index, distance) &&
				(result == NONE || function_128b90(voice->sound_index, result_sound_index, result_distance)))
			{
				s_sound_playback *other = SOUND_PLAYBACK_GET(voice->sound_index);

				*reason = 9;
				result_sound_index = voice->sound_index;
				result = i;
				result_distance = sound_source_get_listener_distance((s_sound_location_source const *)&other->location, other->listener_index);
			}
		}
	}
	return result;
}


/* a gain in decibels between two gains in decibels (real bits), along a
   curve: linear in gain, or its power */
static __forceinline long sound_gain_interpolate_linear(real t)
{
	real fraction = t;
	long lower_decibels;
	long upper_decibels;
	real lower;
	real upper;

	if (0.0f > t)
		fraction = 0.0f;
	else if (t > 1.0f)
		fraction = 1.0f;
	lower_decibels = 0xc2800000;
	upper_decibels = 0;
	lower = function_2195f0(*(real *)&lower_decibels);
	upper = function_2195f0(*(real *)&upper_decibels);
	return function_2197f0((upper - lower) * fraction + lower);
}

static __forceinline long sound_gain_interpolate_power(real t)
{
	real fraction = t;
	long lower_decibels;
	long upper_decibels;
	real lower;
	real upper;

	if (0.0f > t)
		fraction = 0.0f;
	else if (t > 1.0f)
		fraction = 1.0f;
	lower_decibels = 0xc2800000;
	upper_decibels = 0;
	lower = function_2195f0(*(real *)&lower_decibels);
	upper = function_2195f0(*(real *)&upper_decibels);
	if (upper > lower)
		fraction = (real)sqrt(fraction);
	else
		fraction = 1.0f - (real)sqrt(1.0f - fraction);
	return function_2197f0((upper - lower) * fraction + lower);
}

/* the gain in decibels of a value within a range, along a curve; a negative
   range runs the other way */
// @retail 0x12a6d0
long function_12a6d0(short curve, real value, real range)
{
	real t = (real)fabs(value / range);
	long result;

	t = PIN(t, 0.0f, 1.0f);
	if (0.0f > range)
		t = 1.0f - t;
	switch (curve)
	{
	case 0:
		result = sound_gain_interpolate_linear(t);
		break;
	case 1:
		result = sound_gain_interpolate_power(t);
		break;
	}
	return result;
}

real function_12aff0(real a, real b, real c, bool flag);

/* a gain in decibels (real bits) pinned to [-64, 0] */
static inline long decibels_pin(long decibels)
{
	if (*(real *)&decibels < -64.0f)
		return 0xc2800000;
	else if (*(real *)&decibels > 0.0f)
		return 0;
	else
		return decibels;
}

/* the ends of a fade in decibels: full, and silence */
long g_440c48 = 0;
long g_440c4c = 0xc2800000;

/* a playing sound's fade in decibels: from its fade gain to silence or from
   silence to it, between its fade's start and end times */
// @retail 0x12a810
long function_12a810(long sound_index)
{
	s_sound_playback *sound = SOUND_PLAYBACK_GET(sound_index);
	long result = 0;

	if (TEST_FIELD_BIT(sound->fading))
	{
		s_sound_system_view *sound_system = SOUND_SYSTEM;
		long start;
		long end;
		long latest;
		real t;

		if (sound->fade_start_time == NONE || sound->fade_end_time == NONE)
		{
			sound->fade_start_time += sound_system->time + 1;
			sound->fade_end_time += sound_system->time + 1;
		}
		start = sound->fade_start_time;
		end = sound->fade_end_time;
		latest = start > end ? start : end;
		t = function_12aff0((real)(start - latest), (real)(end - latest), (real)(sound_system->time - latest), start < end);
		{
			long lower = *(start < end ? &sound->fade_gain : &g_440c4c);
			long upper = *(start < end ? &g_440c48 : &sound->fade_gain);

			switch (sound->fade_curve)
			{
			case 0:
			{
				real lower_gain = function_2195f0(*(real *)&lower);
				real upper_gain = function_2195f0(*(real *)&upper);

				result = function_2197f0((upper_gain - lower_gain) * t + lower_gain);
				break;
			}
			case 1:
			{
				real lower_gain = function_2195f0(*(real *)&lower);
				real upper_gain = function_2195f0(*(real *)&upper);
				real fraction;

				if (upper_gain > lower_gain)
					fraction = (real)sqrt(t);
				else
					fraction = 1.0f - (real)sqrt(1.0f - t);
				result = function_2197f0((upper_gain - lower_gain) * fraction + lower_gain);
				break;
			}
			}
		}
		return decibels_pin(result);
	}
	return result;
}

/* fades a playing sound out over 300 ms from where its fade is, after
   telling its source it is going */
// @retail 0x126360
void function_126360(long sound_index)
{
	if (datum_get_inlined(g_4e637c, sound_index))
	{
		s_sound_playback *sound = SOUND_PLAYBACK_GET(sound_index);

		if (sound->state == 0)
		{
			if (sound->source && sound->source->detach)
				sound->source->detach(sound->object_index, sound_index);
			sound = SOUND_PLAYBACK_GET(sound_index);
			sound->fade_gain = function_12a810(sound_index);
			sound->fade_curve = 0;
			sound->fade_start_time = 300;
			sound->fade_end_time = NONE;
			sound->fading = true;
		}
	}
}

/* fades one playing sound in and another out over a duration in seconds,
   along a curve */
// @retail 0x126df0
void function_126df0(long fade_in_index, long fade_out_index, short curve, real duration)
{
	if (fade_in_index != NONE)
	{
		s_sound_playback *sound = SOUND_PLAYBACK_GET(fade_in_index);

		if (TEST_FIELD_BIT(sound->fading))
			sound->fade_gain = function_12a810(fade_in_index);
		else
			sound->fade_gain = 0xc2800000;
		sound->fade_curve = curve;
		sound->fade_start_time = NONE;
		sound->fade_end_time = (long)(duration * 1000.0f);
		sound->fading = true;
	}
	if (fade_out_index != NONE)
	{
		s_sound_playback *sound = SOUND_PLAYBACK_GET(fade_out_index);

		sound->fade_gain = function_12a810(fade_out_index);
		sound->fade_curve = curve;
		sound->fade_start_time = (long)(duration * 1000.0f);
		sound->fade_end_time = NONE;
		sound->fading = true;
	}
}

/* which of a sound's voices to take over for another sound: the one playing
   longest past its class's preemption time, or one of a quieter sound */
// @retail 0x128a60
short function_128a60(long sound_index, short count, short const *voice_indices)
{
	s_sound_playback *sound = SOUND_PLAYBACK_GET(sound_index);
	s_sound_definition *definition = sound_definition_get(sound->definition_index);
	long best_age = 0;
	short best = NONE;

	for (short i = 0; i < count; i++)
	{
		short voice_index = voice_indices[i];
		s_sound_playback *voice_sound = SOUND_PLAYBACK_GET(g_4e6378[voice_index].sound_index);
		long age = SOUND_SYSTEM->time - voice_sound->start_time;

		if (age >= ((s_sound_promotion_view *)sound_class_definition_get(definition->promotion_index))->preemption_time && age > best_age ||
			sound->value_a0 > voice_sound->value_a0)
		{
			best = voice_index;
			best_age = age;
		}
	}
	return best;
}

/* a sound's voices playing the same definition, and those of them from the
   same source (unknown_12a1b0.cpp has the full structures) */
struct s_sound_voice_group
{
	short count;
	short voice_indices[16];
	short limit;
	bool started_this_tick;
	byte unknown25;
};

struct s_looping_voice_counts
{
	s_sound_voice_group definition;
	s_sound_voice_group source;
};

/* counts the other voices playing a sound's definition, and those of them
   from the same source, against its class's limits */
// @retail 0x128500
void function_128500(long sound_index, s_looping_voice_counts *counts)
{
	s_sound_playback *sound = SOUND_PLAYBACK_GET(sound_index);
	s_sound_definition *definition = sound_definition_get(sound->definition_index);

	counts->definition.started_this_tick = false;
	counts->definition.count = 0;
	counts->source.count = 0;
	counts->definition.limit = ((s_sound_promotion_view *)sound_class_definition_get(definition->promotion_index))->definition_voice_limit;
	counts->source.limit = ((s_sound_promotion_view *)sound_class_definition_get(definition->promotion_index))->source_voice_limit;
	for (short i = 0; i < SOUND_SYSTEM->voice_count; i++)
	{
		s_sound_voice *voice = &g_4e6378[i];

		if (voice->sound_index != NONE && voice->sound_index != sound_index)
		{
			s_sound_playback *other = SOUND_PLAYBACK_GET(voice->sound_index);

			if (definition->type == voice->definition_type && sound->definition_index == other->definition_index)
			{
				counts->definition.voice_indices[counts->definition.count++] = i;
				if (sound->object_index != NONE && other->object_index != NONE && sound->source == other->source &&
					(sound->object_index == other->object_index ||
					sound->source->same_source && sound->source->same_source(sound->object_index, (s_sound_source_state const *)&sound->marker, other->object_index, (s_sound_source_state const *)&other->marker)))
				{
					counts->source.voice_indices[counts->source.count++] = i;
					if (SOUND_SYSTEM->time == other->start_time)
						counts->definition.started_this_tick = true;
				}
			}
		}
	}
}

/* the flags of a sound class of the sound classes tag */
struct s_sound_class_flags_view
{
	byte unknown00[8];
	byte flag0 : 1;
	byte shares_object_voice : 1;
	byte unknown08 : 6;
};

/* finds a voice for a playing sound: the one it already has; for a class
   whose sounds share their object's voice, a voice of another such sound of
   the same object (reason 13, stopping that sound); otherwise within its
   class's voice limits (reasons 5 and 6 when it must take over one of its
   definition's or its source's voices) */
// @retail 0x128700
short sound_voice_acquire(long sound_index, long *reason)
{
	s_sound_playback *sound = SOUND_PLAYBACK_GET(sound_index);
	s_sound_definition *definition;

	*reason = 0;
	if (sound->value_ac != NONE)
		return sound->value_ac;
	definition = sound_definition_get(sound->definition_index);
	if (TEST_FIELD_BIT(((s_sound_class_flags_view *)function_221810(definition->promotion_index))->shares_object_voice) && sound->object_index != NONE)
	{
		long taken_sound_index = NONE;
		short taken_voice_index = NONE;
		short voice_index = NONE;
		short voice_count = SOUND_SYSTEM->voice_count;

		for (short i = 0; i < voice_count; i++)
		{
			s_sound_voice *voice = &g_4e6378[i];
			long other_index = voice->sound_index;

			if (other_index != NONE)
			{
				s_sound_playback *other = SOUND_PLAYBACK_GET(other_index);

				if (other->object_index == sound->object_index &&
					TEST_FIELD_BIT(((s_sound_class_flags_view *)function_221810(sound_definition_get(other->definition_index)->promotion_index))->shares_object_voice))
				{
					if (definition->type == voice->definition_type)
					{
						sound->location.audible = other->location.audible;
						*reason = 13;
						voice_index = i;
						break;
					}
					taken_voice_index = i;
					taken_sound_index = other_index;
				}
			}
		}
		if (voice_index == NONE)
			voice_index = sound_voice_find(sound_index, reason);
		if (voice_index != NONE && taken_sound_index != NONE)
		{
			function_127320(taken_sound_index, 13);
			sound_voice_free(taken_voice_index);
		}
		return voice_index;
	}
	else
	{
		s_looping_voice_counts counts;

		function_128500(sound_index, &counts);
		if (counts.definition.started_this_tick)
		{
			*reason = 5;
			return NONE;
		}
		if (counts.source.count >= counts.source.limit)
		{
			*reason = 5;
			return function_128a60(sound_index, counts.source.count, counts.source.voice_indices);
		}
		if (counts.definition.count >= counts.definition.limit)
		{
			*reason = 6;
			return function_128a60(sound_index, counts.definition.count, counts.definition.voice_indices);
		}
		return sound_voice_find(sound_index, reason);
	}
}

bool function_12be90(void);

/* advances the sound system's clock and the ambience fades, and fades the
   master gain out while the game asks for it (after its delay) and back in
   once nothing is busy */
// @retail 0x1269f0
void sound_system_update_time(void)
{
	dword now = GetTickCount();
	s_sound_system_view *sound_system = SOUND_SYSTEM;
	long elapsed = now - sound_system->field_14_2;

	sound_system->field_14_2 = now;
	sound_system->time += elapsed;
	sound_system->ambience_fade += (real)elapsed * 0.001f;
	sound_system->elapsed_time = (real)elapsed * 0.001f;
	sound_system->previous_ambience_fade += (real)elapsed * 0.001f;
	if (g_4e6948 && g_4e6948->flag1120 && g_4e6948->state == 1)
	{
		if (g_4e6948->flag1121)
		{
			if (sound_system->master_fade_delay == NONE)
				sound_system->master_fade_delay = (long)(sound_system->master_fade_times.i * 1000.0f);
			if (sound_system->master_fade_delay > 0)
			{
				long remaining = sound_system->master_fade_delay - elapsed;

				sound_system->master_fade_delay = remaining > 0 ? remaining : 0;
			}
			if (sound_system->master_fade_delay == 0)
			{
				real step = sound_system->elapsed_time / (0.001f > sound_system->master_fade_times.j ? 0.001f : sound_system->master_fade_times.j);

				sound_system->master_fade += PIN(0.0f - sound_system->master_fade, 0.0f - step, step);
			}
		}
		else if (!function_12be90())
		{
			real step = sound_system->elapsed_time / (0.001f > sound_system->master_fade_times.k ? 0.001f : sound_system->master_fade_times.k);

			sound_system->master_fade += PIN(1.0f - sound_system->master_fade, 0.0f - step, step);
			sound_system->master_fade_delay = NONE;
		}
	}
	else
	{
		sound_system->master_fade = 1.0f;
	}
}

struct s_bsp3d;
extern s_bsp3d *g_4e033c;
long function_14a280(s_bsp3d *bsp, point3f *point, long index);
void function_11bed0(s_location *location, point3f const *point);
long function_16bc00(s_record_pool *data, long index);
void sound_voices_update_locations(void);
void looping_sound_update_locations(void);

/* the structure bsp's leaves (8 bytes), as the listeners' locations read them */
struct s_structure_bsp_leaves_view
{
	byte unknown00[0x30];
	struct
	{
		short cluster_index;
		byte unknown02[6];
	} *leaves;
};

/* finds where in the structure bsp the playing sounds and the listeners are */
// @retail 0x125920
void sound_update_locations(void)
{
	s_sound_system_view *sound_system = SOUND_SYSTEM;

	if (sound_system->initialized && sound_system->hardware_available && sound_system->enabled)
	{
		s_record_pool *sounds = g_4e637c;
		long sound_index = data_datum_index(sounds, function_16bc00(sounds, 0));

		while (sound_index != NONE)
		{
			s_sound_playback *sound = (s_sound_playback *)sounds->data + (sound_index & 0xffff);

			if (sound->location.audible == 1)
			{
				s_location location;

				function_11bed0(&location, &sound->location.spatial.position);
				sound->location.spatial.location = location;
				sound_system = SOUND_SYSTEM;
			}
			sound_index = data_datum_index(sounds, function_16bc00(sounds, sound_index == NONE ? 0 : (sound_index & 0xffff) + 1));
		}
		for (long i = 0; i < 4; i++)
		{
			s_sound_listener *listener = &sound_system->listeners[i];

			if (listener->active)
			{
				listener->leaf_index = function_14a280(g_4e033c, &listener->position, 0);
				listener->cluster_index = listener->leaf_index != NONE ? ((s_structure_bsp_leaves_view *)g_4e0348)->leaves[listener->leaf_index].cluster_index : NONE;
			}
		}
		sound_voices_update_locations();
		looping_sound_update_locations();
	}
}

/* moves the sound system's two environments toward the two requested:
   each request takes the slot already holding it or a faded-out one and
   fades it in over its transition time; slots no request takes fade out */
// @retail 0x126660
void sound_environments_update(s_sound_environment const *requests)
{
	dword used[1];
	long i;

	used[0] = 0;
	for (long request_index = 0; request_index < 2; request_index++)
	{
		s_sound_environment const *request = &requests[request_index];

		if (request->index != NONE)
		{
			long slot = NONE;

			for (i = 0; i < 2; i++)
			{
				if (!(used[i >> 5] & (1 << (i & 31))))
				{
					if (SOUND_SYSTEM->environments[i].index == request->index)
					{
						slot = i;
						break;
					}
					if (0.001f >= SOUND_SYSTEM->environments[i].fade)
						slot = i;
				}
			}
			if (slot != NONE)
			{
				real transition_time = request->transition_time > 0.001f ? request->transition_time : 0.001f;
				real step = SOUND_SYSTEM->elapsed_time / transition_time;
				s_sound_environment *environment = &SOUND_SYSTEM->environments[slot];

				if (0.001f >= environment->fade)
				{
					environment->unknown0c = request->unknown0c;
					environment->unknown10 = request->unknown10;
				}
				else
				{
					real maximum = step * k_pi;

					environment->unknown0c += PIN(request->unknown0c - environment->unknown0c, 0.0f - maximum, maximum);
					environment->unknown10 += PIN(request->unknown10 - environment->unknown10, 0.0f - maximum, maximum);
				}
				environment->unknown18 += PIN(request->unknown18 - environment->unknown18, 0.0f - step, step);
				environment->unknown14 += PIN(request->unknown14 - environment->unknown14, 0.0f - step, step);
				environment->fade += PIN(1.0f - environment->fade, 0.0f - step, step);
				environment->transition_time = request->transition_time;
				environment->index = request->index;
				used[slot >> 5] |= 1 << (slot & 31);
			}
		}
	}
	for (i = 0; i < 2; i++)
	{
		if (!(used[i >> 5] & (1 << (i & 31))))
		{
			s_sound_environment *environment = &SOUND_SYSTEM->environments[i];
			real transition_time = environment->transition_time > 0.001f ? environment->transition_time : 0.001f;
			real step = SOUND_SYSTEM->elapsed_time / transition_time;

			environment->fade += PIN(0.0f - environment->fade, 0.0f - step, step);
		}
	}
}

/* ---- starting a sound ---- */

long __stdcall function_127d00(s_type_99c531 const *source, real maximum_distance, real *distance);
short sound_definition_rate_limit_pitch_range(long stage_index, s_sound_definition const *definition);
struct s_looping_playback_definition;
short function_218f50(s_looping_playback_definition *definition, short previous, real pitch);
byte __stdcall function_219070(long set_index);
struct s_sound_effect_request;
void function_2226b0(long tag_index, s_sound_effect_request *request);
long record_pool_allocate(s_record_pool *data);
long looping_sound_controller_find_and_reference(long definition_index);

/* a sound class of the sound classes tag, as starting a sound reads it */
struct s_sound_class_start_flags
{
	byte unknown00[0xa];
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word unknown0a : 13;
};

/* the flags a playing sound's location starts with, as bytes */
struct s_sound_location_byte_flags
{
	byte flag0 : 1;
	byte unknown00 : 6;
	byte flag7 : 1;
};

/* a flag bit of a word, tested on its zero-extended value */
#define SOUND_FLAG(flags, bit) ((bool)(((dword)(flags) >> (bit)) & 1))

class c_class_219e90
{
public:
	void function_219f60(long count);
};

c_class_219e90 *function_2198f0(long index);

struct s_sound_transmission_view;
bool function_221da0(long listener_index, s_sound_transmission_view const *sound, real scale);

/* the type of a sound's location (1: in the world) */
struct s_sound_location_type_view
{
	byte unknown00[3];
	byte type : 4;
	byte unknown03 : 4;
};

/* the first local player in use, or NONE */
static __forceinline long sound_local_player_first_index(void)
{
	long result = NONE;

	for (long i = 0; i < 4; i++)
	{
		if (g_4e8c20->entries[i] != NONE)
		{
			result = i;
			break;
		}
	}
	return result;
}

/* the listener nearest a sound within its maximum distance, and how far it
   is; any local player's for a sound not in the world */
// @retail 0x127d00
long __stdcall function_127d00(s_type_99c531 const *source, real maximum_distance, real *distance)
{
	s_type_99c531 const *const *source_reference = &source;
	long listener_index = NONE;
	real result;

	if (((s_sound_location_type_view const *)*source_reference)->type != 1)
	{
		listener_index = sound_local_player_first_index();
		result = 0.0f;
	}
	else
	{
		real best = FLT_MAX;

		for (long i = 0; i < 4; i++)
		{
			if (SOUND_SYSTEM->listeners[i].active)
			{
				real listener_distance = sound_source_get_listener_distance((s_sound_location_source const *)*source_reference, i);

				if (best > listener_distance)
				{
					listener_index = i;
					best = listener_distance;
				}
			}
		}
		result = (real)sqrt(best);
		if (function_221da0(listener_index, (s_sound_transmission_view const *)*source_reference, result / maximum_distance))
			listener_index = NONE;
		if (!SOUND_FLAG((*source_reference)->flags, 8) && !SOUND_FLAG((*source_reference)->flags, 11))
		{
			if (best > maximum_distance * maximum_distance)
			{
				listener_index = NONE;
				result = FLT_MAX;
			}
		}
		else if (listener_index == NONE)
		{
			listener_index = sound_local_player_first_index();
		}
	}
	if (distance)
		*distance = result;
	return listener_index;
}

/* how far sound travels in a millisecond, inverted (0x547f14) */
real g_547f14;

/* whether a sound would be heard: its definition can play, the random skip
   fraction lets it, a listener is in range and it is loud enough; the
   listener goes to *listener_index and why it would not play to *reason */
// @retail 0x126c30
bool function_126c30(s_sound_play_state *state, long tag_index, long *listener_index, long *reason)
{
	long const *local_947334 = &tag_index;
	s_type_99c531 *location = &state->location;
	bool result = false;
	long failure = 5;

	if (sound_system_available())
	{
		s_sound_definition *definition = sound_definition_get(*local_947334);

		if ((definition->format == 1 && definition->type != 2) || (definition->type == 2 && g_47f0e4))
		{
			real random = function_x82e52f(&g_4e7408->seed, __FILE__, __LINE__);
			s_sound_playback_parameters *playback = &SOUND_GLOBALS_DEFINITIONS->playback_parameters[definition->playback_index];
			real lower = playback->skip_fraction_lower;
			real local_cc7843 = lower + (playback->skip_fraction_upper - lower) * location->scale;

			failure = 1;
			if (random > local_cc7843 * function_xaa8231(definition->class_index)->skip_fraction_scale)
			{
				failure = 4;
				if (function_126bd0(*local_947334))
				{
					real distance = sound_get_maximum_distance((s_sound const *)location, *local_947334);

					*listener_index = function_127d00(location, distance, &distance);
					if (*listener_index != NONE)
					{
						real distance_gain = sound_get_distance_gain(*local_947334, (s_sound const *)location, distance);
						long gain = function_1251e0(definition, location->unknown08, location->scale);
						long distance_decibels = function_2197f0(distance_gain);
						long decibels = decibels_add(function_xaa8231(definition->class_index)->gain_base, decibels_add(gain, distance_decibels));

						if (SOUND_FLAG(location->flags, 11) || SOUND_FLAG(location->flags, 8) || *(real *)&decibels > -64.0f)
						{
							failure = 0;
							result = true;
						}
					}
				}
			}
		}
	}
	if (reason)
		*reason = failure;
	return result;
}

/* starts a sound from a play state: a new sound with the state's location,
   gain, pitch and source, at a pitch range and permutation; NONE if it would
   be too quiet or none is free */
// @retail 0x126000
long function_126000(long tag_index, long listener_index, s_sound_play_state *state, long rate_limit_stage)
{
	s_sound_definition *definition = sound_definition_get(tag_index);
	real const *base_gain = (state->flags & 0x80) ? &state->gain : (real const *)&g_440c48;
	long gain = sound_definition_random_gain(definition);
	*(real *)&gain = *base_gain + *(real *)&gain;
	long definition_gain = function_1251e0(definition, state->location.unknown08, state->location.scale);
	long decibels = decibels_add(definition_gain, gain);
	long result = NONE;

	if (*(real *)&decibels > -64.0f)
	{
		function_2226b0(tag_index, (s_sound_effect_request *)state);
		result = record_pool_allocate(g_4e637c);
		if (result != NONE)
		{
			s_sound_playback *sound = SOUND_PLAYBACK_GET(result);
			long delay = (long)(sqrt(sound_source_get_listener_distance((s_sound_location_source const *)&state->location, listener_index)) * g_547f14);
			long chunk_index;

			sound->state = 0;
			sound->value_ac = NONE;
			sound->priority = state->priority;
			sound->listener_index = (char)listener_index;
			sound->pitch = sound_definition_random_pitch(definition, &g_4e7408->seed);
			sound->gain = *(real *)&gain;
			sound->seed = g_4e7408->seed;
			sound->flags = (state->flags & 0x2) ? state->playback_flags : 0;
			sound->object_index = (state->flags & 0x8) ? state->object_index : NONE;
			sound->location = state->location;
			if (definition->flags & 0x8)
				((s_sound_location_byte_flags *)&sound->location)->flag0 = true;
			if (TEST_FIELD_BIT(((s_sound_class_start_flags *)function_221810(definition->promotion_index))->flag2))
				((s_sound_location_byte_flags *)&sound->location)->flag7 = true;
			sound->value_a2 = (char)((state->flags & 0x40) ? state->value8e : NONE);
			sound->value_a4 = (state->flags & 0x200) ? state->value90 : NONE;
			sound->platform_playback = (state->flags & 0x100) ? state->platform_playback : NONE;
			if (state->flags & 0x10)
			{
				sound->effect_index = looping_sound_controller_find_and_reference(state->effect_index);
				if (sound->effect_index != NONE)
					function_2198f0(sound->effect_index)->function_219f60(1);
			}
			else
			{
				sound->effect_index = NONE;
			}
			sound->unknown03 = NONE;
			if (state->flags & 0x20)
			{
				sound->source = state->source;
				memcpy(sound->source_data, state->source_data, (word)state->source_data_size > sizeof(sound->source_data) ? sizeof(sound->source_data) : state->source_data_size);
			}
			else
			{
				sound->source = NULL;
			}
			chunk_index = 0;
			if (state->flags & 0x400)
			{
				sound_playback_set_chunk(result, tag_index, (char)state->variant0, (char)state->variant1, (short)chunk_index);
			}
			else
			{
				short arg_58ecd0;

				if (rate_limit_stage != NONE)
				{
					arg_58ecd0 = sound_definition_rate_limit_pitch_range(rate_limit_stage, definition);
				}
				else
				{
					s_sound_playback_parameters *playback = &SOUND_GLOBALS_DEFINITIONS->playback_parameters[definition->playback_index];
					real lower = (real)playback->pitch_lower;

					arg_58ecd0 = function_218f50((s_looping_playback_definition *)definition, NONE, ((real)playback->pitch_upper - lower) * state->location.scale + sound->pitch + lower);
				}
				sound_playback_set_chunk(result, tag_index, (char)arg_58ecd0, (char)function_219070(arg_58ecd0), (short)chunk_index);
			}
			sound->value_a0 = (char)rate_limit_stage;
			sound->fade_end_time = chunk_index;
			sound->fade_start_time = chunk_index;
			if (delay > 100 && !SOUND_FLAG(sound->location.flags, 8) && !SOUND_FLAG(sound->location.flags, 11))
			{
				sound->start_time = SOUND_SYSTEM->time + delay;
				sound->flag0 = true;
			}
			else
			{
				sound->start_time = SOUND_SYSTEM->time;
			}
			function_125e60((s_looping_track_sound *)sound);
		}
	}
	return result;
}

/* starts a looping sound's track: as a sound is started from a play state,
   with why it did not start in *reason */
struct s_looping_detail_request;

// @retail 0x125f70
long function_125f70(long definition_index, s_looping_detail_request *request, long *reason)
{
	long result = NONE;
	long listener_index;

	if (sound_system_available() && function_126c30((s_sound_play_state *)request, definition_index, &listener_index, reason))
	{
		long rate_limit_stage;

		if (sound_definition_rate_limited(definition_index, &rate_limit_stage))
		{
			result = NONE;
			if (reason)
				*reason = 2;
		}
		else
		{
			result = function_126000(definition_index, listener_index, (s_sound_play_state *)request, rate_limit_stage);
			if (reason && result == NONE)
				*reason = 3;
		}
	}
	return result;
}

void function_191270(void);

/* the bytes of a sound address the driver's buffers take (unknown_124f90) */
long g_47f0e0;

/* the sound globals tag the globals name (0x20) */
struct s_globals_sound_globals_view
{
	byte unknown00[0x20];
	long sound_globals_index;
};

/* resets the sound system for a new map: the sound globals tag, the master
   fade, the clock, the ambiences, the environments and the listeners */
// @retail 0x125690
void function_125690(void)
{
	s_tag_header_globals *globals = g_4e034c;
	s_globals_sound_globals_view *header = (s_globals_sound_globals_view *)(globals->header ? globals->header_alt : NULL);
	long i;

	g_51ebd4 = (s_sound_globals *)g_4e3b44[header->sound_globals_index & 0xffff].bytes;
	SOUND_SYSTEM->master_fade = 1.0f;
	SOUND_SYSTEM->enabled = true;
	SOUND_SYSTEM->field_14_2 = GetTickCount();
	SOUND_SYSTEM->time = 0;
	SOUND_SYSTEM->ambience_index = NONE;
	SOUND_SYSTEM->previous_ambience_index = NONE;
	SOUND_SYSTEM->environments[0].fade = 0.0f;
	SOUND_SYSTEM->environments[0].index = NONE;
	SOUND_SYSTEM->environments[0].unknown0c = 0.0f;
	SOUND_SYSTEM->environments[0].unknown10 = 2.0f * k_pi;
	SOUND_SYSTEM->environments[1].fade = 0.0f;
	SOUND_SYSTEM->environments[1].index = NONE;
	SOUND_SYSTEM->environments[1].unknown0c = 0.0f;
	SOUND_SYSTEM->environments[1].unknown10 = 0.0f;
	for (i = 0; i < 4; i++)
		SOUND_SYSTEM->listeners[i].active = false;
	sound_mix_apply();
	function_191270();
	SOUND_DRIVER_GLOBALS->impulse_ids[0] = NONE;
	SOUND_DRIVER_GLOBALS->impulse_ids[1] = NONE;
	g_47f0e0 = g_4e6948->state == 3 ? 0x10000 : 0x4000;
}

/* normalizes a vector, returning its length (left alone when it is near zero) */
static inline real sound_vector_normalize(vector3f *v)
{
	real length = (real)sqrt(v->i * v->i + v->j * v->j + v->k * v->k);

	if (!(0.0001f > fabs(length)))
	{
		real inverse = 1.0f / length;

		v->i = inverse * v->i;
		v->j *= inverse;
		v->k *= inverse;
	}
	return length;
}

/* the doppler shift of a sound for a listener, in cents: from the speeds of
   the listener and the sound toward each other, the speed of sound 111.5
   world units a second, the frequency ratio between 1/8 and 4 */
// @retail 0x12a9d0
real function_12a9d0(long listener_index, s_sound_position const *position)
{
	s_sound_listener *listener = &SOUND_SYSTEM->listeners[listener_index];
	vector3f direction;
	vector3f velocity;
	vector3f listener_velocity;
	real listener_speed;
	real source_speed;
	real ratio;

	vector3d_from_points3d(&listener->position, &position->position, &direction);
	sound_vector_normalize(&direction);
	velocity.i = listener->velocity.i;
	velocity.j = listener->velocity.j;
	velocity.k = listener->velocity.k;
	if (listener->velocity_scale != 1.0f)
	{
		velocity.i = listener->velocity_scale * velocity.i;
		velocity.j = listener->velocity_scale * velocity.j;
		velocity.k = listener->velocity_scale * velocity.k;
	}
	listener_velocity.i = listener->up.i * velocity.k + listener->left.i * velocity.j + listener->forward.i * velocity.i;
	listener_velocity.j = listener->up.j * velocity.k + listener->left.j * velocity.j + listener->forward.j * velocity.i;
	listener_velocity.k = listener->up.k * velocity.k + listener->left.k * velocity.j + listener->forward.k * velocity.i;
	listener_speed = 0.0f - (listener_velocity.k * direction.k + listener_velocity.j * direction.j + listener_velocity.i * direction.i);
	source_speed = position->velocity.k * direction.k + position->velocity.j * direction.j + position->velocity.i * direction.i + 111.548553f;
	ratio = (111.548553f - listener_speed) / (source_speed > 0.001f ? source_speed : 0.001f);
	return (real)(log(PIN(ratio, 0.125, 4.0f)) * 1731.234f);
}

void looping_sound_controllers_synchronize(bool initial_playback);
void function_21f570(void);
void function_21f5a0(void);
void function_21d4d0(void);

/* pauses or resumes the voices and impulse buffers together */
// @retail 0x125a90
void function_125a90(long value)
{
	bool paused = value == 0;

	if (paused != SOUND_SYSTEM->unknown7b)
	{
		SOUND_SYSTEM->changing_pause = true;
		SOUND_SYSTEM->unknown7b = paused;
		switch (value)
		{
		case 0:
			for (long i = 0; i < SOUND_SYSTEM->voice_count; i++)
				sound_voice_reset_stream((short)i);
			function_21f570();
			break;
		case 1:
			for (long i = 0; i < SOUND_SYSTEM->voice_count; i++)
				sound_voice_restart_stream((short)i);
			looping_sound_controllers_synchronize(true);
			function_21f5a0();
			SOUND_SYSTEM->field_14_2 = GetTickCount();
			break;
		}
		SOUND_SYSTEM->changing_pause = false;
	}
}

/* clears the effects and stops sounds in the initial playback state */
// @retail 0x126400
void function_126400(void)
{
	if (SOUND_SYSTEM->initialized)
	{
		function_21d4d0();
		s_record_pool *sounds = g_4e637c;
		long sound_index = data_datum_index(sounds, function_16bc00(sounds, 0));

		while (sound_index != NONE)
		{
			s_sound_playback *sound = (s_sound_playback *)sounds->data + (sound_index & 0xffff);

			if (!sound->state)
			{
				function_127320(sound_index, 10);
				sounds = g_4e637c;
			}
			sound_index = data_datum_index(sounds, function_16bc00(sounds, sound_index == NONE ? 0 : (sound_index & 0xffff) + 1));
		}
	}
}

struct s_voice_playing_sound;
long sound_voice_find_or_create(s_voice_playing_sound const *sound);

struct s_driver_voice_index_view
{
	word identifier;
	short buffer_index;
	byte unknown04[0x6c - 4];
};

static inline short sound_voice_find_available_channel(char type)
{
	s_sound_system_channels_view *system = (s_sound_system_channels_view *)g_4e6380;
	short result = NONE;

	for (long i = system->first_channel[type]; i <= system->last_channel[type]; i++)
	{
		if ((system->free_bits[i >> 5] & (1 << (i & 31))) && !(system->streaming_bits[i >> 5] & (1 << (i & 31))))
		{
			result = (short)i;
			break;
		}
	}
	return result;
}

/* assigns a free channel and, when needed, a shared driver voice */
// @retail 0x1295e0
bool __stdcall function_1295e0(short voice_index)
{
	s_sound_voice *voice = &g_4e6378[voice_index];
	s_sound_playback *sound = SOUND_PLAYBACK_GET(voice->sound_index);
	bool result = true;

	if (voice->channel_index == NONE || ((sound->priority & 2) && voice->unknown0e == NONE))
	{
		short channel = sound_voice_find_available_channel((char)voice->definition_type);
		if (channel != NONE)
		{
			long driver_index = NONE;
			short buffer_index = NONE;
			char requested_buffer = (char)sound->value_a2;

			if (requested_buffer != NONE)
			{
				DWORD status;
				IDirectSoundBuffer_GetStatus(SOUND_DRIVER_GLOBALS->voices[requested_buffer].buffer, &status);
				if (!(status & DSBSTATUS_PLAYING))
					buffer_index = (char)sound->value_a2;
			}
			else if (sound->priority & 2)
			{
				driver_index = sound_voice_find_or_create((s_voice_playing_sound const *)sound);
				if (driver_index != NONE)
					buffer_index = ((s_driver_voice_index_view *)g_502114->data)[driver_index & 0xffff].buffer_index;
			}
			if (buffer_index != NONE || (!(sound->priority & 2) && (char)sound->value_a2 == NONE))
			{
				s_sound_system_channels_view *system = (s_sound_system_channels_view *)g_4e6380;
				voice->unknown0e = buffer_index;
				voice->channel_index = channel;
				voice->driver_voice_index = driver_index;
				system->streaming_bits[channel >> 5] |= 1 << (channel & 31);
				channel = voice->channel_index;
				system->free_bits[channel >> 5] &= ~(1 << (channel & 31));
				return result;
			}
		}
		result = false;
		function_127320(voice->sound_index, 7);
		sound_voice_free(voice_index);
	}
	return result;
}
