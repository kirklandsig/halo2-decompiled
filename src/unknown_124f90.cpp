// @flags /O2 /arch:SSE /Gr
#include "pool_storage.h"
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
#include "crc.h"
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

/* a gain in decibels, copied as a whole */
struct s_decibels_view
{
	real value;
};

static inline real decibels_interpolate(long a, long b, real t)
{
	return (*(real *)&b - *(real *)&a) * t + *(real *)&a;
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

/* inline: callers that keep it out of line (0x21d060) then save their scratch
   registers around it, as retail does */
// @retail 0x125010
inline real sound_get_maximum_distance(s_sound const *sound, long definition_index)
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

struct s_125262
{
    real field_0;
};

// volatile, as retail does it: the zero is stored to the stack and loaded back
// as a float, and the scale is read through its pointer at the point of use
static __forceinline long function_125262(long arg_0, long arg_1, real const volatile *arg_2)
{
    long local_0 = arg_0;
    s_125262 local_1;
    local_1.field_0 = *(real *)&local_0;
    long volatile local_2 = 0;
    real local_3 = *(real const *)&local_2;
    local_1.field_0 = (local_1.field_0 - local_3) * *arg_2 + local_3;
    local_0 = arg_1;
    local_1.field_0 += *(real *)&local_0;
    *(real *)&local_0 = local_1.field_0;
    return local_0;
}

// @retail 0x125260
long sound_definition_random_gain(s_sound_definition const *definition)
{
	s_sound_class *sound_class = function_xaa8231(definition->class_index);

	real volatile local_0 = function_x82e52f(&g_4e7408->seed, __FILE__, __LINE__);
	return function_125262(sound_class->gain_variance, sound_class->gain_base, &local_0);
}

// @retail 0x1252f0
real sound_definition_random_pitch(s_sound_definition const *definition, dword *seed)
{
	real local_0;
	s_sound_class *sound_class = function_xaa8231(definition->class_index);
	local_0 = function_259d0(seed, __FILE__, __LINE__, (real)sound_class->pitch_lower, (real)sound_class->pitch_upper);
	return local_0;
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
	short maximum_voice_count;
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
real sound_source_get_listener_distance(long listener_index, s_sound_location_source const *source)
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

s_record_pool *function_11cc20(long maximum_count, char const *name, long size);
void sound_cache_initialize(void);
bool looping_sound_controllers_initialize(void);
bool function_21a0c0(void);
bool sound_effects_initialize(void);
struct s_sound_voice_settings;
bool sound_voices_initialize(s_sound_voice_settings const *settings);
struct s_sound_driver_counts
{
	short channels[3];
	byte unknown06[6];
	short voices;
};
bool function_21e4b0(s_sound_driver_counts const *counts);

static __forceinline void *sound_static_allocate(long size)
{
	byte *top = (byte *)(g_510804_pool_size + g_510800_pool_base);
	byte *memory = (byte *)(((dword)top + 3) & ~3);
	long aligned_size = (memory - top) + size;

	g_510804_pool_size += aligned_size;
	function_163ba0(&g_510808_pool_checksum, &aligned_size, sizeof(aligned_size));
	return memory;
}

static __forceinline void sound_environment_reset_1253d0(s_sound_environment *environment, real angle)
{
	environment->index = NONE;
	environment->unknown0c = 0.0f;
	environment->unknown10 = angle;
}

// score 3 (was 3): retail schedules the inner counter's `inc esi` after the voice's first two
// stores; ours puts it right after `inc edi`. Earlier rounds: while loop with the increment after
// those stores 3; voice index as its own statement 3; long voice index 56; word counter 5; both
// increments in the for 10; typed per-type count member 3; other store orders 3 (harness).
// Round 39: every store through the array element 3; voice reference 3; the first two stores
// through the array element ahead of the pointer 3 (they then precede `inc edi` too, which keeps
// `inc esi` beside it); index advanced after the first two stores 3, or at the end 3/3.
// Round 33 (lane 13; closure LTCG harness, base 3 = 3): definition type stored before the sound index
// 2 (the stores follow source order, `inc esi` stays after `inc edi`; not kept); `g_4e6378 + index++`
// 3; the inner counter declared before the for 3; a per-type count local used for the sum and the
// bound 49 (the bound then sits in a register); `(char)type` 3; `++i` 3.
// @retail 0x1253d0
void function_1253d0(void)
{
	static s_sound_driver_counts const counts = { {40, 41, 1}, {36, 0, 36, 0, 1, 0}, 51 };
	if (g_4e6374 == 0)
	{
		s_sound_system_view *system = (s_sound_system_view *)sound_static_allocate(0x20c);
		memset(system, 0, 0x20c);
		g_4e6380 = (s_4e6380 *)system;
		system->hardware_available = true;
		system->changing_pause = false;
		sound_cache_initialize();
		sound_environment_reset_1253d0(&SOUND_SYSTEM->environments[0], 2.0f * k_pi);
		sound_environment_reset_1253d0(&SOUND_SYSTEM->environments[1], 0.0f);
		g_4e637c = function_11cc20(0x180, "sound sources", 0xbc);
		g_4e6378 = (s_sound_voice *)sound_static_allocate(0x1200);
		if (g_4e637c && g_4e6378)
		{
			g_4e637c->valid = true;
			record_pool_release_all(g_4e637c);
			memset(g_4e6378, 0, 0x1200);
			if (looping_sound_controllers_initialize() && function_21a0c0() &&
				sound_effects_initialize() && sound_voices_initialize((s_sound_voice_settings const *)&counts) &&
				function_21e4b0(&counts))
			{
				system = SOUND_SYSTEM;
				short voice_index = 0;
				bit_vector_fill((dword *)((byte *)system + 0x60), 0x80, 0);
				bit_vector_fill(system->channel_bits, 0x40, 0);
				for (short type = 0; type < 3; type++)
				{
					SOUND_SYSTEM->voice_count += ((short const *)counts.unknown06)[type];
					for (short i = 0; i < ((short const *)counts.unknown06)[type]; i++)
					{
						s_sound_voice *voice = &g_4e6378[voice_index++];
						voice->sound_index = NONE;
						voice->definition_type = (byte)type;
						voice->unknown0a = 0;
						voice->unknown0b = 0;
						voice->channel_index = NONE;
						voice->unknown0e = NONE;
						voice->driver_voice_index = NONE;
						voice->permutation = NULL;
						voice->next_permutation = NULL;
						voice->stream_reset = false;
					}
				}
				SOUND_SYSTEM->maximum_voice_count = 0x33;
				SOUND_SYSTEM->initialized = true;
			}
		}
	}
	g_4e6374++;
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
	bool result = true;
	s_sound_voice *local_0 = g_4e6378 + voice_index;
	long sound_index = *(long const volatile *)&local_0->sound_index;

	if (sound_index != NONE)
	{
		s_sound_playback *sound = (s_sound_playback *)g_4e637c->data + (sound_index & 0xffff);
		char promotion_index = sound_definition_get(sound->definition_index)->promotion_index;

		if ((promotion_index < 0 ? 0 : (promotion_index > 0x35 ? 0x35 : promotion_index)) == promotion_index)
		{
			short const &argument = promotion_index;
			result = !TEST_FIELD_BIT(((s_sound_promotion_flags *)function_221810(argument))->disabled);
		}
	}
	return result;
}

static inline bool bit_test_12af10(dword const *bits, long index)
{
	return (bits[index >> 5] & (1 << (index & 31))) != 0;
}

// @retail 0x12af10
short sound_channel_allocate(void)
{
	short channel_count = ((s_sound_system_channels_view *)g_4e6380)->channel_count;
	short result = NONE;

	for (short i = 0; i < channel_count; i++)
	{
		if (bit_test_12af10(((s_sound_system_channels_view *)g_4e6380)->available_bits, i) &&
			!bit_test_12af10(((s_sound_system_channels_view *)g_4e6380)->used_bits, i))
		{
			((s_sound_system_channels_view *)g_4e6380)->used_bits[i >> 5] |= 1 << (i & 31);
			result = i;
			break;
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

long sound_format_duration_to_bytes(long encoding, long compression, long arg_da1d74, real duration);
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
			long offset = sound_format_duration_to_bytes((char)definition->format, (char)definition->type, (char)definition->unknown03, voice->unknown10);

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
long function_219290(s_permutation_group *group, short index);
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
			s_sound_chunk *chunk = &tables->chunks[voice->permutation->first_chunk + voice->chunk_index];
			s_sound_cache_request_result result;

			sound_reference_release((s_sound_reference_holder *)chunk);
			if ((short)function_219290((s_permutation_group *)voice->permutation, voice->chunk_index) == NONE)
				voice->unknown10 = 0.0f;
			voice->permutation = voice->next_permutation;
			voice->chunk_index = voice->next_chunk_index;
			voice->next_permutation = NULL;
			voice->next_chunk_index = NONE;
			chunk = &tables->chunks[voice->permutation->first_chunk + voice->chunk_index];
			result.value = function_218850(NONE, chunk, 0);
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
	bool result = true;
	s_sound_definition *definition = sound_definition_get(definition_index);

	if (source)
	{
		if (source->update(sound->object_index, definition_index, &sound->marker, &sound->location))
			return true;
		if (!((1 << SOUND_PLAYBACK_GET(sound_index)->state) & 0x1e) &&
			!function_124f90((s_sound const *)function_221810(definition->promotion_index)))
		{
			result = true;
			flags->source_updated = true;
		}
		else
		{
			result = false;
			flags->source_updated = true;
		}
	}
	return result;
}
static __forceinline bool function_127fc1(long arg_0, s_sound_playback *arg_1, s_sound_playback_flags *arg_2)
{
    bool local_1 = true;
    s_sound_source_callbacks const *local_0 = arg_1->source;
    if (local_0 && local_0->update && (arg_1->start_time < SOUND_SYSTEM->time || SOUND_SYSTEM->unknown7b))
        local_1 = sound_playback_update_source(arg_0, local_0, arg_2);
    return local_1;
}

// @retail 0x127fc0
bool sound_playback_update_location(long sound_index)
{
	s_sound_playback *sound = SOUND_PLAYBACK_GET(sound_index);
	s_sound_playback_flags *flags = (s_sound_playback_flags *)&sound->flags;
	bool result = true;
	if (!TEST_FIELD_BIT(flags->flag0) && !TEST_FIELD_BIT(flags->source_updated))
		result = function_127fc1(sound_index, sound, flags);
	return result;
}

/* ---- gains in decibels, held as real bits ---- */

/* a sound class's volume fade (g_502118, unknown_221490.cpp) */
struct s_sound_class_fade
{
	dword target;
	s_decibels_view current;
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
// score 137 (was 137): the gain is a decibel struct (retail copies decibels as a whole, as 0x218d70 does): it
// lives in its own stack home from entry, each sum is stored there and the result is returned from it, at
// retail's 678 bytes. Left: the entry (retail loads g_502118 first and pushes the unextended class index),
// the first branch's product/sum order (retail `(elapsed - hold) * inverse * fade_out`, then
// `ambience_fade - elapsed` plus it), the clamps' register (retail starts from 1.0 and replaces it), and the
// hold branch's copy of the ducking gain through a stack temporary.
// Earlier at 149: retail keeps the entry gain in a stack home that the gain helpers read in place; the
// helper's signature is in include/unknown_218ac0.h, so this file can only hand the gain over by copy.
// Earlier at 149: Frame 0xc agrees; early table load, gain home, delayed ESI save and fade scheduling differ.
// Retail keeps gain in a stack home from entry (stored before the flag test, EDI keeps a copy for the
// plain returns), the helpers read it there with no copy, and each sum is stored in that home.
// Round 72: decibels_add's second parameter a const reference 168 (retail's home store and EDI copy
// appear; the first-argument gain is still copied to a temporary, sums go to another slot); both
// parameters const references 173 (655 bytes: the tag gain is read in place); first parameter a const
// reference 152; the add done in place on its first parameter 147; `*(real *)&gain +=` with a real
// interpolation 183; `(void)&gain` 149; the class index used directly 162.
// Frame 0xc agrees; early fade-table load, entry gain home, delayed ESI save and fade scheduling remain.
// Common gain result 172; early system receiver 217; outer elapsed 162; hold-first branches 206; assigned clamps 165.
// Unsigned gain 162; entry gain declaration 162; disabled-ducking early return 172;
// shared fade-in duration 162; distinct-ambience case first 163.
// Forced interpolation inline 162; direct fade-table fields 169; readonly class views 162; system rereads 162;
// short-duration exit 162.
// Release fraction term 163; ducking-field rereads 313; shared fade fraction 162;
// shared fade time 162; both fade workspaces 162.
// Hold duration snapshot 165; ambience time snapshot 173; ducking gain snapshot 165;
// upper-first release clamp 162; all three snapshots 178 (fast); all restored.
// Both ambience indices 170; readonly fade 162; equality-first admission 198; shared ducking pointer 162; native class ordinal 149 kept plain; int ordinal 149; ordinal with unsigned gain 149; ordinal with common gain result 157.
// Shared sum operand order 174; interpolation start snapshot/accumulator 149/149;
// addition accumulator 147; start snapshot plus sum order 174; native endpoints 149;
// both accumulators 147; integer result home 149.
// Existing shared sum 173; shared sum with live gain/common result/readonly fade 188/142/173;
// live gain with value sum 211; shared common result with readonly fade/shared workspaces 142/142;
// shared live gain with direct table 188. Seven-point result forms and unmatched reference forms restored.
// Round 160: Interpolation input orders a/t/b 149, b/a/t 148, b/t/a 148, t/a/b 149, t/b/a 148; immutable
// base plus release gain 149; interpolation/addition home names 149/149. All executable lines and helper
// contracts restored. One-point previews excluded by the minimum-gain rule.
// Round 196: native table-array base 200; readonly table 200; short ordinal 170;
// gain inside gate 234; readonly class/base-release/common result/unsigned gain 200/200/212/200.
// All restored; table base changes incoming register transport without recovering the entry gain home.
// Round 161: real-returning interpolation with in-place sums on the gain's bits 158 (retail's entry home
// store and EDI reload appear); plus the raw class index for the lookup 165 (678 bytes); plus a guard
// return/`else return gain` for the flag test 171/174 (retail's late ESI save); returned sums as real
// results 174. Shifts dominate: aligned counts 91 -> 77/75/70/69/73. All restored.
// Round 193: branch-local/shared active/entry interpolation outputs 149/174/174;
// each with distinct final result 157/165/165; distinct result alone 157;
// shared entry outputs/result with role home names 165. All restored.
// Round 164: real gain local with in-place sums 233; first branch as divisions
// (`(elapsed - hold) / fade_in * fade_out`, `1.0f > time / fade_in ? time / fade_in : 1.0f`) with a
// fade_out local 160 (retail's reciprocal-first order, multiply compare and divss result); plus
// `t = 1.0f; if (time < 1.0f) t = time;` clamps 160/180 (the two tails cross-jump); if/else-if pin 160.
// Round 94: gain as `s_decibels_view` (one real; fade->current declared with that type here) copied from
// fade->current, sums stored into gain.value,
// decibels_interpolate returning real from its long-bit operands, `return *(long *)&gain` 137 (kept; 678
// bytes); on it, the lookup passed the raw class index 179 (670 bytes), and without the index local 179;
// the first branch written with divisions 207 (636 bytes); clamps as `t = time; if (t > 1.0f) t = 1.0f;`
// 136, as `t = 1.0f; if (time < 1.0f) t = time;` 190 (642 bytes). Compile-only: clamp ternary spellings
// and dot/product term orders are canonicalised (no change).
// Round 133 (one-file LTCG harness, exact here: 137): no index local, class index passed raw 179 (670 bytes:
// retail's `movsx eax, cx` and raw `push ecx`, but the table load folds into the add); the raw index for the
// call only 179; the hold sum through `decibels_interpolate(0, gain, 1.0f)` 184 (704 bytes), through the
// existing decibels_add on the gain's bits 163 (686 bytes; retail's long copy of the ducking gain appears);
// the first branch with divisions 207 (636 bytes), plus a fade_out local 207; a fade_out local with the
// inverse 141 (670 bytes); the time sum product-first 137 (identical). Left: the else branch and the first
// branch are retail's code with XMM2/XMM3 (and the 1.0 constant's register) swapped.
// Round 40 (one-file LTCG harness, exact: 137): the ambience test inverted with the branches swapped 137 (elapsed
// stays XMM2, retail XMM3); a fade_out local for the tests and the product 141 (670 bytes); the time sum product-first
// 137, `inverse * (elapsed - hold) * fade_out` 137 (same code); `(elapsed - hold) * (inverse * fade_out)` 172; the
// reciprocal computed before the fade-out tests 187 (656 bytes).
// @retail 0x127010
long function_127010(short class_index)
{
	long index = class_index;
	s_sound_class_fade *fade = &g_502118[index];
	s_decibels_view gain = fade->current;

	if ((byte)~fade->flags & 1)
	{
/* retail calls the class lookup out of line here */
#pragma inline_depth(0)
		s_sound_promotion_view *sound_class = (s_sound_promotion_view *)function_221810(index);
#pragma inline_depth(255)
		s_sound_system_view *sound_system = SOUND_SYSTEM;

		if (sound_system->previous_ambience_index != NONE && sound_system->ambience_index == sound_system->previous_ambience_index)
		{
			real elapsed = sound_system->ambience_fade - sound_system->previous_ambience_fade;
			s_sound_class_ducking *ducking = &sound_class->duckings[sound_system->ambience_index];

			if (elapsed > ducking->hold_time)
			{
				if (ducking->fade_in_time >= 0.001f)
				{
					if (ducking->fade_out_time >= 0.001f && ducking->fade_out_time + ducking->hold_time > elapsed)
					{
						real inverse = 1.0f / ducking->fade_in_time;
						real time = sound_system->ambience_fade - elapsed + (elapsed - ducking->hold_time) * inverse * ducking->fade_out_time;
						real t = 1.0f > time * inverse ? time / ducking->fade_in_time : 1.0f;

						gain.value = gain.value + decibels_interpolate(0, ducking->gain, t);
					}
					else
					{
						real time = (sound_system->ambience_fade - elapsed) / ducking->fade_in_time;
						real t = 1.0f > time ? time : 1.0f;

						gain.value = decibels_interpolate(0, ducking->gain, t) + gain.value;
					}
				}
			}
			else
			{
				gain.value = *(real const *)&ducking->gain + gain.value;
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

					gain.value = decibels_interpolate(ducking->gain, 0, t) + gain.value;
				}
			}
			if (sound_system->ambience_index != NONE)
			{
				s_sound_class_ducking *ducking = &sound_class->duckings[sound_system->ambience_index];

				if (ducking->fade_in_time >= 0.001f)
				{
					real time = sound_system->ambience_fade / ducking->fade_in_time;
					real t = 1.0f > time ? time : 1.0f;

					gain.value = decibels_interpolate(0, ducking->gain, t) + gain.value;
				}
			}
		}
	}
	return *(long *)&gain;
}

/* decibels_add on the values in place: retail reads both operands where they are */
static inline long decibels_sum(long const &a, long const &b)
{
	real result = *(real const *)&a + *(real const *)&b;

	return *(long *)&result;
}

/* a sound's gain in decibels: between its definition's bounds, with its
   class's and the caller's */
inline long function_1251e0(void const *definition_pointer, long gain, real interpolation);
// score 15 (was 15): retail's length now; everything up to the class gain call matches. Left: the
// final add: retail loads the converted gain into xmm1 right after the class gain call and adds
// the inner sum's register to it; ours folds the converted gain in as the add's memory operand
// (retail 0x127010 shows the same unfolded `decibels_add(gain, computed)`). Kept: decibels_sum for
// the outer sum, which keeps the converted gain in its stack home from its call on and replaces
// the volatile local (the volatile form was 15 at 128 bytes). Without either, the conversions
// move below the class gain call (they are pure); a pointer write to 0x2195f0's own parameter
// stops that (12 at retail's length) but changes 0x2195f0 (it gains a frame: screened).
// Tried: the outer sum's operands swapped 36; no volatile 27; no volatile with the operands
// swapped 49; the inner sum named, operands swapped 36. Round 30: the conversion call written as
// the outer sum's argument 27 (the class gain call runs before the upper conversion); the inner
// sum named 15; the converted gain read as a real local for the add 15; the inner operands
// swapped 16. Screened (compile-only), all folding the converted gain: by-value outer sum, the
// add in place on it, real locals read before or after the class gain call, by-value real or
// reference helper parameters, and (one diagnostic) `/Op`.
// @retail 0x1251e0
long function_1251e0(void const *definition_pointer, long gain, real interpolation)
{
	s_sound_definition const *definition = (s_sound_definition const *)definition_pointer;
	long upper_decibels = sound_definition_gain_upper(definition);
	long lower_decibels = sound_definition_gain_lower(definition);
	real lower = function_2195f0(*(real *)&lower_decibels);
	real upper = function_2195f0(*(real *)&upper_decibels);
	long interpolated_decibels = function_2197f0((upper - lower) * interpolation + lower);
	long class_gain = function_127010(definition->promotion_index);

	return decibels_sum(interpolated_decibels, decibels_add(gain, class_gain));
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
	real minimum_distance;
	real maximum_distance;
	real gain;

	/* the getter is expanded inline for the test and called for the value */
	if (sound_get_minimum_distance(sound, definition_index) > 0.001f)
	{
#pragma inline_depth(0)
		minimum_distance = sound_get_minimum_distance(sound, definition_index);
#pragma inline_depth()
	}
	else
	{
		minimum_distance = 0.001f;
	}
	maximum_distance = sound_get_maximum_distance(sound, definition_index);

	gain = function_12aff0(maximum_distance, minimum_distance, distance, false);

	return (minimum_distance / MAXIMUM(minimum_distance, distance)) * gain;
}

real function_30bf0(vector3f *vector);
real function_11cf50(vector3f const *a, vector3f const *b);

// @retail 0x12ad30
real function_12ad30(s_sound const *sound, vector3f const *offset, long definition_index, vector3f const *direction)
{
	real distance_gain = sound_get_distance_gain(definition_index, sound, (real)sqrt(length_sq3f(offset)));
	real cone_gain = 1.0f;
	if ((TEST_FIELD_BIT(sound->override_inner_cone_angle) || TEST_FIELD_BIT(sound->override_outer_cone_angle) ||
		!(sound_definition_get(definition_index)->flags & 0x1000)) &&
		!(fabs(distance_gain) < 0.0001f) &&
		offset->j * offset->j + offset->k * offset->k + offset->i * offset->i >= 0.001f * 0.001f)
	{
		vector3f reverse = *offset;
		reverse.i = 0.0f - reverse.i;
		reverse.j = 0.0f - reverse.j;
		reverse.k = 0.0f - reverse.k;
		if (function_30bf0(&reverse) > 0.0f)
		{
			long gain = sound_get_outer_cone_gain(sound, definition_index);
			/* the angle between the two is symmetric; the direction goes first */
			real angle = function_11cf50(direction, &reverse);
			real outer = sound_get_outer_cone_angle(sound, definition_index);
			real inner = sound_get_inner_cone_angle(sound, definition_index);
			real fraction = function_12aff0(inner, outer, angle, true);
			cone_gain = (function_2195f0(*(real *)&gain) - 1.0f) * fraction + 1.0f;
		}
	}
	real result = cone_gain * distance_gain;
	if ((bool)((*(word const *)sound >> 9) & 1))
		result = 1.0f - result;
	return result;
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
#pragma inline_depth(0)
// score 5 (was 5): Pool save remains EBP versus EBX; void tail result 5, inline release declaration 5,
// noinline declaration 5, controller workspace first 5, byte count workspace 5, complete decrement
// predicate 5, signed count view 5, indexed controller 5.
// playback pool retained in EBP instead of EBX; no frame, length 98 agrees.
// Controller reference/workspace 5 each; no-effect cleanup 43; decrement rejection 5;
// scoped controller lookup 39; receiver names 5; scoped reference 39; reference cleanup 43.
// Inline/forced-inline controller getter 40/40; open-depth inline getter with noinline release 122;
// playback/both receiver references 5/5; const playback/controller pointers 5/5;
// unsigned decrement/mask 5/5.
// Round 56: the byte decrement is what keeps the pool out of EBX (as a dword store it gets EBX);
// pool local 43 (stack convention); effect index local 5; byte count local 5; count pointer 5;
// nested decrement tests 5; harness: char field, cast, `--`/`&=`, `+ 0x7f`, long/short/dword
// temporaries, mask first, bitfield, release without the depth pragma all keep EBP or worsen.
// Round 107: a dword decrement moves the pool to EBX (diagnostic); tests swapped 40; index parameter
// read through its address 46; `(word)` controller index 31; `(count + 0xff) & 0x7f` 5; controller
// pool local 5; decrement helper (kept out of line by the pragma) 29, forced inline 29.
// Final pool-release inline/noinline declarations 5/5; controller/playback/both direct lookups
// 58/59/81; unsigned cyclic remainder 5; shared cleanup exit 5; controller-first predicate 40.
// Round 39: controller declared at function scope 5; unknown03 test in a bool before the lookup 5;
// pool release in both arms 33; controller test outside the unknown03 test 40; byte cast before the
// mask 5. Without the depth pragma the release inlines (122); with either test removed the pool
// still takes EBP, so the byte store's register is what moves it.
// Round 33 (lane 13; LTCG harness, base 5 = 5; the decrement already uses DL as in retail): the whole
// decrement cast to byte 5; an early-return form for no effect 33; the controller pool global typed as a
// record pool (no cast) 5; a `(word)` controller index 31; `(count + 0xff) & 0x7f` 5.
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
#pragma inline_depth(255)

/* stops a playing sound: frees its voice, lets go of its chunk, tells its
   source why it stopped and deletes it */
__declspec(noinline) void __stdcall function_127320(long sound_index, long reason);
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
bool function_128b90(long other_index, long sound_index, real distance)
{
	s_sound_playback *other = SOUND_PLAYBACK_GET(other_index);
	s_sound_definition *other_definition = sound_definition_get(other->definition_index);
	s_sound_playback *sound = SOUND_PLAYBACK_GET(sound_index);
	s_sound_definition *definition = sound_definition_get(sound->definition_index);
	long other_priority = sound_definition_priority(other_definition);
	long priority = sound_definition_priority(definition);

	return other_priority > priority ||
		other_priority == priority &&
		(other->definition_index == sound->definition_index ?
			other->value_a0 > sound->value_a0 || other->value_a0 == sound->value_a0 &&
				sound_source_get_listener_distance(sound->listener_index, (s_sound_location_source const *)&sound->location) > distance :
			sound_source_get_listener_distance(sound->listener_index, (s_sound_location_source const *)&sound->location) > distance);
}

/* finds a voice for a playing sound: a free one of its definition's type,
   or the one whose sound should give way to it the most (reason 9) */
// @retail 0x128920
short sound_voice_find(long sound_index, long *reason)
{
	s_sound_playback *sound = SOUND_PLAYBACK_GET(sound_index);
	s_sound_definition *definition = sound_definition_get(sound->definition_index);
	short result = NONE;
	long result_sound_index = NONE;
	real result_distance;
	real distance = sound_source_get_listener_distance(sound->listener_index, (s_sound_location_source const *)&sound->location);

	*reason = 0;
	for (short i = 0; i < SOUND_SYSTEM->voice_count; i++)
	{
		s_sound_voice *voice = &g_4e6378[i];

		if (definition->type == voice->definition_type)
		{
			if (voice->sound_index == NONE)
			{
				result = i;
				break;
			}
			if (function_128b90(sound_index, voice->sound_index, distance) &&
				(result == NONE || function_128b90(result_sound_index, voice->sound_index, result_distance)))
			{
				s_sound_playback *other = SOUND_PLAYBACK_GET(voice->sound_index);

				*reason = 9;
				result_sound_index = voice->sound_index;
				result = i;
				result_distance = sound_source_get_listener_distance(other->listener_index, (s_sound_location_source const *)&other->location);
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
			long upper = *(start >= end ? &sound->fade_gain : &g_440c48);

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
		result = decibels_pin(result);
	}
	return result;
}

/* fades a playing sound out over 300 ms from where its fade is, after
   telling its source it is going */
void function_0bfe40(word *flags, long bit, bool value);

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
			function_0bfe40(&sound->flags, 7, true);
		}
	}
}

/* fades one playing sound in and another out over a duration in seconds,
   along a curve */
// @retail 0x126df0
void function_126df0(short curve, long fade_in_index, long fade_out_index, real duration)
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
		function_0bfe40(&sound->flags, 7, true);
	}
	if (fade_out_index != NONE)
	{
		s_sound_playback *sound = SOUND_PLAYBACK_GET(fade_out_index);

		sound->fade_gain = function_12a810(fade_out_index);
		sound->fade_curve = curve;
		sound->fade_start_time = (long)(duration * 1000.0f);
		sound->fade_end_time = NONE;
		function_0bfe40(&sound->flags, 7, true);
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
	word flag0 : 1;
	word shares_object_voice : 1;
	word unknown08 : 14;
};

/* finds a voice for a playing sound: the one it already has; for a class
   whose sounds share their object's voice, a voice of another such sound of
   the same object (reason 13, stopping that sound); otherwise within its
   class's voice limits (reasons 5 and 6 when it must take over one of its
   definition's or its source's voices) */
#pragma inline_depth(0)
PRIVATE __forceinline bool function_128701(short index)
{
	return TEST_FIELD_BIT(((s_sound_class_flags_view *)function_221810(index))->shares_object_voice);
}
#pragma inline_depth()

// @retail 0x128700
short sound_voice_acquire(long sound_index, long *reason)
{
	s_sound_playback *sound = SOUND_PLAYBACK_GET(sound_index);
	s_sound_definition *definition;

	*reason = 0;
	if (sound->value_ac != NONE)
		return sound->value_ac;
	definition = sound_definition_get(sound->definition_index);
	if (function_128701(definition->promotion_index) && sound->object_index != NONE)
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
					function_128701(sound_definition_get(other->definition_index)->promotion_index))
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
// score 30 (was 30): retail reloads elapsed_time after function_12be90 (ours keeps it in xmm0;
// BLOCKED-BY 0x12be90, whose file another lane holds), so the fade-in clamp and every jump
// after it differ; the fade-out clamp's two subtractions sit in swapped registers. Tried:
// elapsed_time stored before seconds 28 (top then exact); the products' * and / swapped 30;
// fade-out through PIN_126660 74, as PIN_126660 + fade 68, fade + PIN_126660 68, via SOUND_SYSTEM
// 68; a change local before the step 77; fade-in without the elapsed local 44. Every value-first
// fade-out merges retail's two return tails.
// conversion and clamp registers, elapsed reload differ; shared quotient 30;
// shared elapsed 79; both time values 79; shared change 74; quotient/change 74; shared lower bound
// 74; quotient/lower bound 74; all clamp values 74.
// Round 32 (fast): elapsed_time stored first 28 (conversion copy then retail's); with it: the fade-out
// through approach_126660 72; `fade = fade + PIN(...)` 28; value-first clamp (value < lower) 68 (retail's
// copy/zero registers, but the two return tails merge and the add reloads the fade into XMM1); the same
// with lower-first branch arms 72, written result + fade 68, named clamp result 68; a fade local read
// once 65 (tails kept, fade held in XMM3); fade local for the clamp only 71. The whole function alone
// in a /GL- file 111 (retail keeps EDX across 0x12be90, so its register use is known there).
// @retail 0x1269f0
void sound_system_update_time(void)
{
	dword now = GetTickCount();
	s_sound_system_view *sound_system = SOUND_SYSTEM;
	long elapsed = now - sound_system->field_14_2;

	sound_system->field_14_2 = now;
	real seconds = (real)elapsed * 0.001f;
	sound_system->time += elapsed;
	sound_system->ambience_fade += seconds;
	sound_system->elapsed_time = (real)elapsed / 1000.0f;
	sound_system->previous_ambience_fade += seconds;
	if (g_4e6948 && g_4e6948->flag1120 && g_4e6948->state == 1)
	{
		if (g_4e6948->flag1121)
		{
			if (sound_system->master_fade_delay == NONE)
				sound_system->master_fade_delay = (long)(sound_system->master_fade_times.i * 1000.0f);
			if (sound_system->master_fade_delay > 0)
			{
				sound_system->master_fade_delay = sound_system->master_fade_delay - elapsed > 0 ? sound_system->master_fade_delay - elapsed : 0;
			}
			if (sound_system->master_fade_delay == 0)
			{
				real step = sound_system->elapsed_time / (0.001f > sound_system->master_fade_times.j ? 0.001f : sound_system->master_fade_times.j);

				sound_system->master_fade += PIN(0.0f - sound_system->master_fade, 0.0f - step, step);
			}
		}
		else if (!function_12be90())
		{
			real fade_seconds = sound_system->elapsed_time;
			real step = fade_seconds / (0.001f > sound_system->master_fade_times.k ? 0.001f : sound_system->master_fade_times.k);

			approach_126660(&sound_system->master_fade, 1.0f - sound_system->master_fade, 0.0f - step, step);
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
long function_14a280(s_bsp3d *bsp, long index, point3f *point);
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
				listener->leaf_index = function_14a280(g_4e033c, 0, &listener->position);
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
// score 17 (was 17): retail adds the clamped unknown18 step through a copy
// register and adds the clamped unknown14 step in place; ours does the
// opposite, so the two sites trade xmm1 and xmm2.
// Round 29: target-first helper on the first three/four clamps 33/33; forceinline 33;
// third clamp only 130; symmetric bounds 106; value-first 122; both fades 96;
// clamp-first addition 33; named delta 104; target pointer 99. Original restored.
// Round 39: the third/fourth add forms follow position, not spelling: written third, the helper,
// `+=`, `x = x + PIN` and `x = PIN + x` all add in place (the two swapped: 19, now in retail's
// add forms but stores 14/18 reversed). Fourth through the helper adds in place as retail but
// lengthens the rest (102). Both helper 133; both `+=` 34; `x = PIN + x` forms 34/34/17; named
// `0 - step` bound 140/119, with a named `0 - maximum` 152; a target-taking helper (delta inside)
// at the third 126/87, also on the first two 134; all 27 helper/`+=`/target combinations of the
// third, fourth and fade 34-136.
// Round 33 (lane 13; LTCG harness, base 17 = 17): the add forms follow the whole function's IL, not the
// line: the first loop's step without its transition-time local 75 (third add now a copy as retail,
// fourth still a copy, 188 instructions), the same in the fade-out loop alone 75 or both 75; on it the
// fourth through the helper 94, as `PIN + x` / `x + PIN` 75/75, the third as `+=` 29, the fade through
// the helper 94, third and fourth swapped 16. Fade test inverted (`fade > 0.001f` first) 138; the
// environment pointer declared first 106; a const environment pointer 17; `used` initialised in its
// declaration 17; the slot declared at function scope 75; the fade-out loop as a for 112-117. All 255
// combinations of helper or `+=` on each of the six adds with or without each loop's transition-time
// local: best 14 (first loop's step without the local, third as `+=`, fourth through the helper: third
// in place, fourth a copy); retail's third/fourth forms appear only with the first two adds or the
// fade-out add flipped and the 1.0 no longer hoisted (122).
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

					approach_126660(&environment->unknown0c, request->unknown0c - environment->unknown0c, 0.0f - maximum, maximum);
					approach_126660(&environment->unknown10, request->unknown10 - environment->unknown10, 0.0f - maximum, maximum);
				}
				approach_126660(&environment->unknown18, request->unknown18 - environment->unknown18, 0.0f - step, step);
				environment->unknown14 += PIN_126660(request->unknown14 - environment->unknown14, 0.0f - step, step);
				environment->fade += PIN_126660(1.0f - environment->fade, 0.0f - step, step);
				environment->transition_time = request->transition_time;
				environment->index = request->index;
				used[slot >> 5] |= 1 << (slot & 31);
			}
		}
	}
	s_sound_system_view *sound_system = SOUND_SYSTEM;

	i = 0;
	do
	{
		if (!(used[i >> 5] & (1 << (i & 31))))
		{
			s_sound_environment *environment = &sound_system->environments[i];
			real transition_time = environment->transition_time > 0.001f ? environment->transition_time : 0.001f;
			real step = sound_system->elapsed_time / transition_time;

			environment->fade += PIN_126660(0.0f - environment->fade, 0.0f - step, step);
		}
	} while (++i < 2);
}

/* ---- starting a sound ---- */

long __stdcall function_127d00(s_type_99c531 const *source, real maximum_distance, real *distance);
short sound_definition_rate_limit_pitch_range(long stage_index, s_sound_definition const *definition);
struct s_looping_playback_definition;
short function_218f50(s_looping_playback_definition *definition, short previous, real pitch);
short function_219070(long set_index, s_sound_definition const *definition);
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
	long listener_index = NONE;
	real result;

	if (((s_sound_location_type_view const *)source)->type != 1)
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
				real listener_distance = sound_source_get_listener_distance(i, (s_sound_location_source const *)source);

				if (best > listener_distance)
				{
					listener_index = i;
					best = listener_distance;
				}
			}
		}
		result = (real)sqrt(best);
		if (function_221da0(listener_index, (s_sound_transmission_view const *)source, result / maximum_distance))
			listener_index = NONE;
		if (!SOUND_FLAG(source->flags, 8) && !SOUND_FLAG(source->flags, 11))
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
// score 7 (was 7): only the last add differs: retail adds the class gain base (loaded into xmm1)
// and the inner sum before the two forced-flag tests; ours adds them after the tests, in the
// comparison's block. Every form that computes the sum before the tests folds the base load
// into it (`addss xmm0, [base]`) or reassociates it; VC sinks the unfolded sum otherwise (the
// same unfolded `a + computed` add is in retail 0x1251e0 and 0x127010). Round 30 (plain): a
// loudness local before the forced test 35; a loudness bool 44; loudness started from the base
// with the inner sum added in place 39; the inner sum added in place on the gain local 8; one
// combined `||` condition with a loudness local 42. Screened (compile-only, which reproduces our
// tail exactly): the gain or the base summed in place, decibels_add into the gain, a flags word
// local.
// @retail 0x126c30
bool function_126c30(s_sound_play_state *state, long tag_index, long *listener_index, long *reason)
{
	s_type_99c531 *location = &state->location;
	bool result = false;
	long failure = 5;

	if (sound_system_available())
	{
		s_sound_definition *definition = sound_definition_get(tag_index);

		if ((definition->format == 1 && definition->type != 2) || (definition->type == 2 && g_47f0e4))
		{
			failure = 1;
			real random = function_x82e52f(&g_4e7408->seed, __FILE__, __LINE__);
			real const *local_cc7843_2 = &SOUND_GLOBALS_DEFINITIONS->playback_parameters[definition->playback_index].skip_fraction_lower;
			real lower = local_cc7843_2[0];
			real local_cc7843 = lower + (local_cc7843_2[1] - lower) * location->scale;

			if (random > local_cc7843 * function_xaa8231(definition->class_index)->skip_fraction_scale)
			{
				failure = 4;
				if ((byte)function_126bd0(tag_index))
				{
					real distance = sound_get_maximum_distance((s_sound const *)location, tag_index);

					*listener_index = function_127d00(location, distance, &distance);
					if (*listener_index != NONE)
					{
						real distance_gain = sound_get_distance_gain(tag_index, (s_sound const *)location, distance);
						long gain = function_1251e0(definition, location->unknown08, location->scale);
						long distance_decibels = function_2197f0(distance_gain);
						real inner = *(real *)&distance_decibels + *(real *)&gain;
						real gain_base = *(real *)&function_xaa8231(definition->class_index)->gain_base;

						bool forced = SOUND_FLAG(location->flags, 11) || SOUND_FLAG(location->flags, 8);

						if (forced || gain_base + inner > -64.0f)
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
// score 23 (was 23): the two set-chunk calls narrow their indices to char
// (byte loads, a sign-extended range index) where retail passes words, and
// the delayed start sets flag0 before storing the start time. BLOCKED-BY
// 0x1286b0 (its two index parameters are short in retail: 0x128ca0 widens
// char fields to 16 bits for them) and 0x219070 (short set index). With both,
// a flags pointer for flag0 and the 0x219070 result in a named short, it
// matches. Tried here: that named short alone 21; the flags pointer alone 19. Round 30 (plain):
// flag0 set through the s_sound_playback_flags cast with no pointer local 23 (only a separate
// pointer keeps the bit store after the start-time store; screened: `flags |= 1`, the store
// order, `delay + time`, `+=` all hoist it); the pitch range local short 59; the named short
// with the flags pointer 17 (left: the byte variant loads and the 0x219070 movsx); one time
// local for both start-time stores 83 (825 bytes, the tails merge).
// @retail 0x126000
long function_126000(long tag_index, long listener_index, s_sound_play_state *state, long rate_limit_stage)
{
	s_sound_definition *definition = sound_definition_get(tag_index);
	real const *base_gain = (state->flags & 0x80) ? &state->gain : (real const *)&g_440c48;
	long gain = sound_definition_random_gain(definition);
	*(real *)&gain = *base_gain + *(real *)&gain;
	long definition_gain = function_1251e0(definition, state->location.unknown08, state->location.scale);
	long decibels = decibels_add(gain, definition_gain);
	long result = NONE;

	if (*(real *)&decibels > -64.0f)
	{
		function_2226b0(tag_index, (s_sound_effect_request *)state);
		result = record_pool_allocate(g_4e637c);
		if (result != NONE)
		{
			s_sound_playback *sound = SOUND_PLAYBACK_GET(result);
			long delay = (long)(sqrt(sound_source_get_listener_distance(listener_index, (s_sound_location_source const *)&state->location)) * g_547f14);
			long chunk_index;

			sound->state = 0;
			sound->value_ac = NONE;
			sound->priority = state->priority;
			sound->listener_index = (char)listener_index;
#pragma inline_depth(0)
			sound->pitch = sound_definition_random_pitch(definition, &g_4e7408->seed);
#pragma inline_depth()
			*(long *)&sound->gain = gain;
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
				long arg_58ecd0;

				if (rate_limit_stage != NONE)
				{
					arg_58ecd0 = sound_definition_rate_limit_pitch_range(rate_limit_stage, definition);
				}
				else
				{
					short const *pitch_bounds = &SOUND_GLOBALS_DEFINITIONS->playback_parameters[definition->playback_index].pitch_lower;
					real lower = (real)pitch_bounds[0];

					arg_58ecd0 = function_218f50((s_looping_playback_definition *)definition, NONE, ((real)pitch_bounds[1] - lower) * state->location.scale + sound->pitch + lower);
				}
				sound_playback_set_chunk(result, tag_index, (char)arg_58ecd0, (char)function_219070(arg_58ecd0, definition), (short)chunk_index);
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
	s_sound_environment *environments = SOUND_SYSTEM->environments;
	environments[0].fade = 0.0f;
	environments[0].index = NONE;
	environments[0].unknown0c = 0.0f;
	environments[0].unknown10 = 2.0f * k_pi;
	environments[1].fade = 0.0f;
	environments[1].index = NONE;
	environments[1].unknown0c = 0.0f;
	environments[1].unknown10 = 0.0f;
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

	real i = v->i;
	real j = v->j;
	real k = v->k;

	if (!(0.0001f > fabs(length)))
	{
		real inverse = 1.0f / length;

		v->i = inverse * i;
		v->j = j * inverse;
		v->k = k * inverse;
	}
	return length;
}

/* the doppler shift of a sound for a listener, in cents: from the speeds of
   the listener and the sound toward each other, the speed of sound 111.5
   world units a second, the frequency ratio between 1/8 and 4 */
// score 122 (was 122): direction j/k versus i register/memory split and length scheduling differ.
// Shared length helper 122; early source speed 143; projection order 133; cached direction 125; void normalizer 122.
// Closest form (129, ratio 0.96): normalize written in the body (not a helper) on the struct, if-body order j, k, i,
// and an inline copy of 0x142640's body (x/y/z copies, matrix->scale) for the velocity; only retail's j/k loads
// hoisted above the branch (no else block) still differ. Scalar j/k copies 170; scalars before the length 161.
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
	return (real)log(PIN(ratio, 0.125, 4.0f)) * 1731.234f;
}

void looping_sound_controllers_synchronize(bool initial_playback);
void function_21f570(void);
void function_21f5a0(void);
void function_21d4d0(void);

/* pauses or resumes the voices and impulse buffers together */
#pragma inline_depth(0)
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
#pragma inline_depth(255)

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

static __forceinline short sound_voice_find_available_channel(char type);

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
// score 96 (was 96): retail loads the voice index into ecx before the frame setup, keeps the `NONE` constant in
// cx and the result in al/bl with an 8-byte frame; ours loads the index into eax and holds the result in a stack
// byte with a 12-byte frame.
// Earlier: playback/channel roles recovered; frame 12 versus 8, status test and failure
// placement remain.
// Validity result 135; assigned result
// 135; playback handle 113; const playback 113; long buffer index 117; entry status 113;
// allocation-scope status 113; status name 113; allocation success result 122.
// Failure flag 130, entry failure
// 130, complemented status 132, long channel 147, driver-first/late-system commits 130/130,
// status plus driver-first 132, requested buffer first 130.
// `return true` on the success path gives retail's 8-byte frame and both
// exits (139, 415 bytes); with `~status & DSBSTATUS_PLAYING` 131 (430 bytes); with driver_index
// set before the channel test too 135 (430 bytes, aligned 101 of 124: channel spilled where retail
// spills buffer_index); result declared first 139; failure/success as one if/else 126.
// Requested-buffer rereads 113; long result 102, with rereads 102; rereads with success exit 139, long too
// 138; byte result with rereads 96.
// Rereads with valid guard 141, plus long/success exit 131. Isolated byte/char/short/word result
// 96/96/125/125; byte only kept, 437/430 bytes.
// Round 279: byte result from completed paths with short/long channel and buffer
// 148/154/144/150; driver result at channel selection with long buffer 130, completed-path too 148;
// completed byte result with complemented status, short/promoted outputs 151/160. Original byte-result body restored.
// Round 161: bool result 113 (430 bytes; retail's failure `xor bl, bl ... mov al, bl`, but the success
// exit reads the entry true from a slot); bool with `return true` on success 139 (result in BL from entry);
// plus `return result` in the failure block 131 (retail roles, frame 8, false after the calls); failure
// default after the search with success flag/`return true` 132/134.
// Round 193: failure-scoped byte with shared/direct true exits 134/131;
// those exits with low-byte complemented status 133/135; entry byte result 96;
// published-channel/per-use system/both commit observations 96/96/96. All restored.
// Round 164: search returning from the loop 96; bool result with `result = true` before the success
// goto 122 (430 bytes: retail's entry `mov al, 1`, frame 8, EBX sound, `xor bl, bl ... mov al, bl`;
// only the failure block's placement after the loop differs); result assigned on every path 135; that
// with both NONE defaults between search and test 128 (retail's block order, not-found unthreaded);
// driver default before/after the search 147/153; plus return-from-loop search 126.
// Round 235: chained driver/buffer defaults inside/before test/before search 91/147/171;
// with canonical success assignment 129/130/144; complemented status before test/inside
// 122/127. The 91 form is below the minimum gain; original byte-result body restored.
// Round 94: search helper taking `long type` (retail's direct movsx of the type byte) 128; success flag
// declared after/before the search with `if (!success)` failure block and early `return true` 138/143;
// result false after the search, true on success, `if (!result)` failure 132; bool result with the search
// index re-tested after a break loop 104 (retail's found-path-after-loop layout: VC places a loop's break
// target after the function's other blocks). Original byte-result body restored.
// Lane 16 r50 (fast): bool result with an `assigned` flag tested before the failure calls 123 (flag in a
// stack byte); the defaults before `if (channel != NONE) { buffer selection }` and then a failure-first
// `if (channel == NONE || (buffer_index == NONE && (priority & 2 || requested != NONE))) { failure } else
// { commit }`: byte result 151, bool result 126 (435 B: retail's block order and `xor bl, bl ... mov al, bl`
// failure, but NONE hoisted into EBP at entry and the not-found exit unthreaded), with `return true` after
// the commit 123; the defaults assigned inside the channel block 113 (threaded, failure back after the
// loop), with `return true` 139; only the driver default before the channel test 107 (430 B, retail's
// layout, `or ebp, -1` shared with the channel compare, but the not-found exit unthreaded and the channel
// spilled); that with the driver default before the search 135; the search returning from its loop on
// those 141/113; the kept form with the driver default before the channel test 132; the failure calls
// written in both the not-found else and the buffer arm 168/168 (455 B, not cross-jumped). Retail threads
// the not-found exit past a block that sets the driver default, which no form here reproduces.
// A `long` buffer index on the kept form 93 (433 B, gain of three: not kept), on the 107 form 109.
// @retail 0x1295e0
bool __stdcall function_1295e0(short voice_index)
{
	s_sound_voice *voice = &g_4e6378[voice_index];
	s_sound_playback *sound = SOUND_PLAYBACK_GET(voice->sound_index);
	byte result = true;

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
				goto channel_done;
			}
		}
		result = false;
		function_127320(voice->sound_index, 7);
		sound_voice_free(voice_index);
	}
channel_done:
	return result;
}

struct s_sound_driver_voice_parameters
{
	byte flags;
	byte unknown01[3];
	point3f position;
	real obstruction;
	real occlusion;
	real decibels;
	real occlusion_rate;
	real obstruction_rate;
};

bool sound_voice_update(s_voice_playing_sound const *sound, bool *orphaned, long voice_index);
point3f *function_142700(transform4x3f const *matrix, point3f const *point, point3f *out);
void sound_driver_voice_update(long voice_index, s_sound_driver_voice_parameters const *parameters);

static __forceinline void function_129791(s_sound_driver_voice *arg_0)
{
	arg_0->flags &= ~2;
}

// @retail 0x129790
void function_129790(short voice_index, vector3f const *attenuation)
{
	s_sound_voice *voice = &g_4e6378[voice_index];
	s_sound_playback *sound = SOUND_PLAYBACK_GET(voice->sound_index);
	voice->unknown0b = voice->unknown0a;
	voice->unknown0a = 0;
	switch (sound->location.audible)
	{
	case 1:
	case 2:
		if (sound->priority & 2)
		{
			bool orphaned = true;
			if (voice->unknown0e != NONE && (voice->driver_voice_index == NONE ||
				sound_voice_update((s_voice_playing_sound const *)sound, &orphaned, voice->driver_voice_index)))
			{
				long listener_index = sound->listener_index;
				s_sound_listener *listener = &SOUND_SYSTEM->listeners[listener_index];
				point3f position;
				if (sound->location.audible == 1)
				{
					sound_source_get_position((s_sound_location_source const *)&sound->location, listener_index, &position);
					function_142700((transform4x3f const *)&listener->velocity_scale, &position, &position);
				}
				else
					position = sound->location.spatial.position;
				if (orphaned)
					function_129791(SOUND_DRIVER_GLOBALS->voices + voice->unknown0e);
				s_sound_definition *definition = sound_definition_get(sound->definition_index);
				s_sound_driver_voice_parameters parameters;
				parameters.position = position;
				if (attenuation)
				{
					short index = definition->promotion_index;
					real scale = *(real *)((byte *)function_221810(index) + 0x50);
					real gain = 1.0f - attenuation->j * scale;
					parameters.occlusion = PIN(attenuation->i * scale, 0.0f, 1.0f);
					parameters.obstruction = PIN(attenuation->k * scale, 0.0f, 1.0f);
					long decibels = function_2197f0(gain);
					long base = *(long *)((byte *)function_221810(index) + 0x10);
					parameters.decibels = *(real *)&base + *(real *)&decibels;
					parameters.obstruction_rate = *(real *)((byte *)function_221810(index) + 0x54) * SOUND_SYSTEM->elapsed_time;
					parameters.occlusion_rate = *(real *)((byte *)function_221810(index) + 0x58) * SOUND_SYSTEM->elapsed_time;
				}
				else
				{
					parameters.occlusion = 0.0f;
					parameters.obstruction = 0.0f;
					*(long *)&parameters.decibels = *(long *)((byte *)function_221810(definition->promotion_index) + 0x10);
					parameters.obstruction_rate = 0.0f;
					parameters.occlusion_rate = 0.0f;
				}
				dword flags;
				if (listener->unknown07 != 0) flags = 1;
				else flags = 0;
				*(dword *)&parameters.flags = flags | 2;
				sound_driver_voice_update(voice->unknown0e, &parameters);
			}
		}
		break;
	case 0:
		if ((bool)(((dword)sound->location.flags >> 12) & 1))
		{
			if (attenuation)
			{
				real scale = *(real *)((byte *)function_221810(sound_definition_get(sound->definition_index)->promotion_index) + 0x50);
				sound->location.scale *= 1.0f - PIN(attenuation->k * scale, 0.0f, 1.0f);
			}
			else if (*(short *)((byte *)sound + 0x46) != g_4686c4)
				sound->location.scale = 0.0f;
		}
		break;
	}
}

// @retail 0x129380
void function_129380(vector3f const *const *voice_values, vector3f const *const *channel_values)
{
	for (long local_index = 0; local_index < SOUND_SYSTEM->voice_count; local_index++)
	{
		s_sound_voice *voice = &g_4e6378[(short)local_index];
		if (voice->sound_index != NONE && !voice->stream_reset)
		{
			if (voice->unknown0e != NONE)
				function_129790((short)local_index, voice_values[voice->unknown0e]);
			else if (voice->channel_index != NONE)
				function_129790((short)local_index, channel_values[voice->channel_index]);
		}
	}
}

// @retail 0x128020
void function_128020(void)
{
	s_record_pool *sounds = g_4e637c;
	long sound_index = data_datum_index(sounds, function_16bc00(sounds, 0));
	while (sound_index != NONE)
	{
		sounds = g_4e637c;
		s_sound_playback *sound = (s_sound_playback *)sounds->data + (sound_index & 0xffff);
		s_sound_definition *definition = sound_definition_get(sound->definition_index);
		s_sound_rate_limit *limit = sound_rate_limit_get(definition->rate_limit_index);
		short voice_index = sound->value_ac;
		if (voice_index != NONE && g_4e6378[voice_index].stream_reset)
			goto skip_to_next;
		if ((bool)(((dword)sound->flags >> 9) & 1) && TEST_FIELD_BIT(sound->flag6) &&
			!(*(byte *)((byte *)g_51ebe4 + g_4e6378[voice_index].channel_index * 0x34 + 0xd) & 7))
		{
			function_127320(sound_index, 2);
			goto skip_to_next;
		}
		if (limit && sound->value_a0 != NONE && sound->value_a0 < limit->field_0 &&
			sound->start_time == limit->field_14_2 && sound->start_time < limit->end_time)
		{
			function_127320(sound_index, 0xf);
			goto skip_to_next;
		}
/* retail calls these two out of line */
#pragma inline_depth(0)
		if (!sound_playback_update_location(sound_index))
#pragma inline_depth()
		{
			function_127320(sound_index, 3);
			goto skip_to_next;
		}
		if (definition->flags & 8)
			((s_sound_location_byte_flags *)&sound->location)->flag0 = true;
		{
			real maximum;
			s_type_99c531 *location = &sound->location;
			word location_flags = *(word *)location;
			if ((bool)((location_flags >> 3) & 1))
				maximum = *(real *)((byte *)location + 0x38);
			else
			{
				s_sound_definition *current = sound_definition_get(sound->definition_index);
				if (current->flags & 0x800)
				{
#pragma inline_depth(0)
					maximum = *(real *)((byte *)function_221810(current->promotion_index) + 0x1c);
#pragma inline_depth(255)
				}
				else
					maximum = function_xaa8231(current->class_index)->maximum_distance;
			}
			long listener = function_127d00(location, maximum, NULL);
			if (listener == NONE)
			{
				if (!TEST_FIELD_BIT(sound->flag2))
				{
					if (sound_index != NONE)
					{
						s_sound_playback *fading = SOUND_PLAYBACK_GET(sound_index);
						fading->fade_gain = function_12a810(sound_index);
						fading->fade_curve = 0;
						fading->fade_start_time = 2000;
						fading->fade_end_time = NONE;
						byte *flags = (byte *)&fading->flags;
						*flags |= 0x80;
					}
					sound->flag2 = true;
				}
			}
			else
			{
				sound->listener_index = (char)listener;
				if (TEST_FIELD_BIT(sound->flag2))
				{
#pragma inline_depth(0)
					function_126df0(0, sound_index, NONE, 0.5f);
#pragma inline_depth()
					sound->flag2 = false;
				}
			}
		}
	skip_to_next:
		sounds = g_4e637c;
		sound_index = data_datum_index(sounds, data_find_index(sounds, sound_index == NONE ? 0 : (sound_index & 0xffff) + 1));
	}
}

struct s_voice_position_entry
{
	long listener;
	byte unknown04[0x20];
	point3f position;
	vector3f result;
	long index;
	long object;
	bool flag44;
	bool flag45;
	bool flag46;
	bool flag47;
};

struct s_voice_position_batch
{
	long count;
	s_voice_position_entry entries[0x48];
};

/* the class flag 0x129100 reads (bit 6 of byte +0xa of a sound class) */
struct s_sound_class_flags_129100
{
	byte unknown00[0xa];
	word unknown0a : 6;
	word flag6 : 1;
	word unknown0a7 : 9;
};

/* the first flags of a playing sound's location, as a word */
struct s_sound_location_flags_129100
{
	word flag0 : 1;
	word unknown01 : 6;
	word flag7 : 1;
	word unknown08 : 8;
};

/* marks a voice buffer as seen */
static inline void seen_set_129100(dword *seen, long index)
{
	seen[index >> 5] |= 1 << (index & 31);
}

// @retail 0x129100
void function_129100(s_voice_position_batch *batch, vector3f **buffers, vector3f **channels)
{
	dword seen[3];

	memset(seen, 0, sizeof(seen));
	for (long i = 0; i < SOUND_SYSTEM->voice_count; i++)
	{
		if (batch->count == 0x48)
			break;
		s_sound_voice *voice = &g_4e6378[(short)i];
		if (voice->sound_index == NONE || voice->stream_reset || !function_1295e0((short)i))
			continue;
		s_sound_playback *sound = SOUND_PLAYBACK_GET(voice->sound_index);
		s_sound_location_source *source = (s_sound_location_source *)&sound->location;
		byte *location = (byte *)&sound->location;
		s_sound_location_flags_129100 *location_flags = (s_sound_location_flags_129100 *)&sound->location;
		bool spatial = source->spatialization == 1 && !TEST_FIELD_BIT(sound->flag2) &&
			!TEST_FIELD_BIT(location_flags->flag7) && !TEST_FIELD_BIT(sound->location.flag0);
		bool local = source->spatialization == 0 && !TEST_FIELD_BIT(sound->flag2) &&
			(bool)(((dword)*(word *)location >> 12) & 1) && *(short *)((byte *)sound + 0x46) == g_4686c4;
		if (spatial)
		{
			short buffer = voice->unknown0e;
			if (buffer == NONE || (seen[buffer >> 5] & (1 << (buffer & 31))))
				continue;
			seen_set_129100(seen, buffer);
			batch->count++;
			s_sound_definition *definition = sound_definition_get(sound->definition_index);
			s_voice_position_entry *entry = &batch->entries[batch->count - 1];
			entry->listener = sound->listener_index;
			entry->position = *(point3f *)((byte *)sound + 0x24);
			entry->index = *(short *)((byte *)sound + 0x44);
			entry->object = *(long *)((byte *)sound + 0x40);
			entry->flag44 = false;
			entry->flag46 = TEST_FIELD_BIT(location_flags->flag7);
			entry->flag45 = false;
			entry->flag47 = TEST_FIELD_BIT(((s_sound_class_flags_129100 *)function_221810(definition->promotion_index))->flag6) || (definition->flags & 0x20);
			entry->result.i = 0.0f;
			entry->result.k = 0.0f;
			entry->result.j = 0.0f;
			buffers[voice->unknown0e] = &entry->result;
		}
		else if (local && voice->channel_index != NONE)
		{
			s_voice_position_entry *entry = &batch->entries[++batch->count - 1];
			entry->listener = sound->listener_index;
			entry->position = *g_468788;
			entry->index = *(short *)((byte *)sound + 0x44);
			entry->object = NONE;
			entry->flag44 = true;
			entry->flag46 = true;
			entry->flag45 = false;
			entry->flag47 = true;
			entry->result.i = 0.0f;
			entry->result.k = 0.0f;
			entry->result.j = 0.0f;
			channels[voice->channel_index] = &entry->result;
		}
	}
}

struct s_animation_state;
struct s_animation_ref;
void function_219310(s_animation_state *state, s_animation_ref *ref, short value);
long function_2193d0(s_animation_state *state, s_animation_ref *ref);

// score 49 (was 49): the start-time test reads the sound through SOUND_PLAYBACK_GET and the sound pointer is
// declared inside it, which gives retail's loop top (base in ecx, `lea esi,[eax+ecx]`, time folded into the
// compare). Left: the mode switch is `test ax, ax` where retail has a two-case switch's `movsx eax, ax;
// sub eax, 0; jne` (3 bytes, shifting every later branch target), and the pool reload sits before the next-index
// ternary. Earlier (86): retail holds the playback base in ecx, builds the pointer with `lea esi,[eax+ecx]` and
// folds the sound-system time into the compare; ours hoists the time into edx and holds the base in edi. The
// mode switch still needs a second non-empty case.
// Frame 8 agrees; playback base, clock scheduling and promoted mode remain.
// Retained clock receiver 163; voice record reference 86; combined receivers 163; role names 86;
// native update/mode/both flag words 86/86/86; receiver plus native flags 163.
// playback base/clock scheduling and promoted mode remain; acquisition/free/load
// inline combinations and stdcall acquisition 86 each; declared mode/flags/both 86 each; shared datum
// iterator 196.
// Loop-entry playback/clock loads and mode comparison remain.
// pool playback 104; promoted mode 127; entry/loop reason 86/86; native record reference 86;
// promoted voice 132; shared absolute index 162; mode with entry reason 127.
// Retail's `movsx eax, ax; sub eax, 0; jne` is the compare chain of a switch with a second,
// non-empty case (an empty case or a single case gives `test ax, ax`). Clock on the left 91;
// pool data playback 104, with the clock on the left 109; while loop 267-270 (all fast).
// Failed-acquisition first 127; pending-chunk first 131; retained callback source 86;
// previous voice owner 86; mode zero guard 155; separate update guard 86;
// range pointer addition 86; callback-availability first 85 (one point, rejected).
// Loop-end search seed/explicit cases 86/86; first search result/both boundaries 86/86; existing tail
// search/seed plus search 86/86; first/both salt producers 183/203. Original body restored; playback/clock and
// native mode scheduling remain.
// Round 279: flag admission separated from class-mode production, short/long mode,
// switch/direct guard and repeated/shared class index: eight forms 153 each.
// Original conditional mode and loop restored; playback/clock schedule still differs.
// Round 161: reason/playback/voice index at function scope 86/86/86; mode switch with an empty case 1 and
// `default: __assume(0)` 86 (the empty case is dropped: still `test ax, ax`); empty case 1 alone 86.
// Round 193: callback procedure alone/with source receiver 86/86;
// one class-record observation short/with callback 105/105; long/with callback 171/171;
// readonly class with short/long plus callback 105/171. All restored.
// Round 164: pool global at each use 86 (identical); for loop with `case 1: continue;` 153/167 (gives
// retail's `movsx eax, ax; sub eax, 0; jne` but swaps the pool/index EBP/EBX); for loop alone 86.
// Round 209: Initial/successor pool epochs, immutable playback/voice pointers and readonly sound
// tag 86/86/86; conditional native sound-tag value 204; pool/value and receiver/value 204/204;
// pool/readonly 86; all 204. Native tag capture expands frame eight to 24 and length 558 to 588;
// original clock, iterator, mode and call inputs restored.
// Round 133 (one-file LTCG harness, exact: 86): matched 0x128020's loop-top `sounds = g_4e637c;` with its
// `(s_sound_playback *)sounds->data + index` pointer 86, the pointer form alone 104; the time in a local 86;
// `!(start > time)` 86; the getter-read test with the pointer inside it 49 (kept); on it, the pool global at each
// use 49; a for loop 49, with `case 1: continue;` 78 (retail's `sub eax, 0` switch, but pool/index swap
// EBP/EBX), `case 1: goto next;` in the do-while 78, case 1 first 78, case 0 ending in continue 78, long mode 89,
// the switch on the ternary 89, an if/else mode 78, `default: continue;` 49 (dropped); empty case 1 with or
// without `default: __assume(0)`, case 1 first, cases 1 and 2: 49 (empty cases are dropped).
// Round 18 (m3 lane 2, one-file LTCG harness, exact 49): on the for loop with `case 1: continue;` (78, retail's
// `sub eax, 0` with the index in EBP and the pool/voice/range pointers in EBX): range index operands swapped 78;
// range as pointer arithmetic 78; `NONE == (short)...` 78; mode as if/else 78, inverted ternary 84; `(short)`
// permutation argument 78; `NONE ==`/`NONE !=` tests 78/78; a do-while whose condition advances (continue goes
// to the advance) 78, with the pool global 78; pool local in the for init 78; the sibling 0x128020's loop-top pool
// read 78, its sound pointer at the loop top 167; bool-returning 0x125e60 78. One-target forms (`default:
// continue;`, `case 0` ending in continue, `case 1: continue; default: continue;`) 49 (`test ax, ax`).
// Round 40 (one-file LTCG harness, exact: 49; the for/continue and two-label forms confirmed with --fast): a for
// loop with `case 1: continue;` 78 (retail's `sub eax, 0` and pool reload after the next-index ternary; only the
// sound index and the pool/voice/range pointers swap EBX/EBP); on it, the range pointer written at both calls 31
// (registers right, address computed after the class-flag call), computed before the mode 24, the class-flag test
// without `(bool)` 72 and flag4 without TEST_FIELD_BIT 98 (registers right, the tests fold to `test byte`), both 97;
// `case 1: case 2: continue;` 17, cases 1, 2 and 3 17, `case 3: continue;` 17 (registers right, `test eax, eax`
// for `sub eax, 0`), `case 2: continue;` 78, `case 1:` then `case 2:` each continuing 113; case 1 as an empty inline
// or out-of-line call 78/78; default placed first or last 78; mode as if/else 78, switch on the ternary 89;
// early-continue start test 78, plus the value_ac test 78; reason, voice index, voice, definition or range declared
// at the top 78 each; voice arms swapped 108; the existing for-form find helper 49 (same code), with the for loop 78;
// the 0x218ac0 pitch-range getter 49/78; definition by table read 78; a while loop with goto 214 (592 bytes).
// Without the switch, dropping either `(bool)` swaps the registers too (120/136). 0x125e60 returning bool (callers
// without `(byte)`) 78 with the for form (0x125e60 itself unchanged).
// Round 9 (m3 lane 18, fast): 0x128020's loop-top `sounds = g_4e637c;` added to this body 49 (identical, the
// load is CSE'd); a plain while loop with that loop-top reload 268 (636 bytes, not rotated); a for loop reading
// the pool global in the init and increment with `case 1: continue;` 78 (retail's `sub eax, 0` and the pool
// reload after the next-index ternary; only the index/pool EBX/EBP swap and the prologue load before `push
// ebp`), with the entry pool through the named `sounds` local 78; `case 1: continue;` skipping a bottom
// `sounds = g_4e637c;` 183 (572 bytes).
// Round 102 (m3 lane 8; one-link LTCG harness, exact for 49 and 78): a dead store in a `case 1:` of the do-while gives
// exactly the for/`case 1: continue;` code (78), so the EBX/EBP swap comes from the two-case compare chain itself, not
// the continue edge. A 192-form grid of the for/continue loop (pool local or global in the init, both next-index
// ternary orders, case 1 before/after case 0 or with `default: break;`, mode as ternary or if/else, both range index
// orders, `&g_4e6378[i]` or `g_4e6378 + i`, start test through the macro or a loop-top pointer) never moves the swap
// (78 or 164). Fast: for/continue 78; with the update test inverted (pending arm first) 132; `case 1: case 2:
// continue;` 17; 0x128020's while + `skip_to_next:` with `case 1: goto skip_to_next;` 214 (592 bytes); index and mode
// declared at function scope 78.
// @retail 0x1282d0
void function_1282d0(void)
{
	s_record_pool *sounds = g_4e637c;
	long sound_index = data_datum_index(sounds, function_16bc00(sounds, 0));
	if (sound_index != NONE)
	{
		do
		{
			if (SOUND_PLAYBACK_GET(sound_index)->start_time <= SOUND_SYSTEM->time)
			{
				s_sound_playback *sound = SOUND_PLAYBACK_GET(sound_index);
				if ((byte)function_125e60((s_looping_track_sound *)sound))
				{
					long reason;
					short voice_index = sound_voice_acquire(sound_index, &reason);
					if (voice_index != NONE)
					{
						s_sound_voice *voice = &g_4e6378[voice_index];
						if (voice->sound_index != sound_index)
						{
							if (voice->sound_index != NONE)
							{
								function_127320(voice->sound_index, reason);
								sound_voice_free(voice_index);
							}
							voice->sound_index = sound_index;
							sound->start_time = SOUND_SYSTEM->time;
							sound->value_ac = voice_index;
						}
						if (!TEST_FIELD_BIT(sound->source_updated) && sound->source && sound->source->proc1)
							sound->source->proc1(sound->object_index, sound->definition_index, sound_index, (long)&sound->marker);
					}
					else
						function_127320(sound_index, 7);
				}
				else if (sound->value_ac == NONE)
				{
					s_sound_definition *definition = sound_definition_get(sound->definition_index);
					short mode = TEST_FIELD_BIT(sound->flag4) ? 1 : *(short *)((byte *)function_221810(definition->promotion_index) + 0xe);
					switch (mode)
					{
					case 0:
						{
							s_sound_pitch_range *range = &SOUND_GLOBALS_DEFINITIONS->pitch_ranges[definition->pitch_range_base + sound->pitch_range_index];
							if (!(bool)((*(word *)((byte *)function_221810(definition->promotion_index) + 0xa) >> 5) & 1) &&
								(short)function_2193d0((s_animation_state *)range, (s_animation_ref *)definition) == NONE)
								function_219310((s_animation_state *)range, (s_animation_ref *)definition, sound->permutation_index);
							function_127320(sound_index, 0xe);
						}
						break;
					}
				}
			}
			sounds = g_4e637c;
			sound_index = data_datum_index(sounds, data_find_index(sounds, sound_index == NONE ? 0 : (sound_index & 0xffff) + 1));
		}
		while (sound_index != NONE);
	}
}

struct s_looping_channel_spatialization;

struct s_type_12a1b0_four_values
{
	real field_0[4];
};

struct s_type_12a1b0_spatial
{
	dword field_0;
	long field_4;
	long field_8;
	long field_c;
	long field_10;
	real field_14;
	real field_18;
	vector3f field_1c;
	s_type_12a1b0_four_values field_28;
};

vector3f *vector3d_decompress(dword arg_0, vector3f *arg_1);

vector3f *function_1427f0(transform4x3f const *arg_0, vector3f const *arg_1, vector3f *arg_2);

// score 101 (was 310): LTCG inlines the class lookup function_221810 that retail calls out of line, which
// shifts the rest of the listing; with the call kept out of line (a #pragma inline_depth(0) pair around
// it inside the body, which DECOMPILING.md rules out) the score is 4: the no-distance path's flag set
// folds to a store (retail ors the byte), and the fade gain sum loads the distance gain first where
// retail loads the selected fade gain first. Not tried: /Ob1 for the file, which changes every function
// in it.
// Tried: one gain local stored after the if/else 35; decibels_sum with a by-value operand 128 (6 at site 2
// alone); the sum through decibels_sum instead of the local 6; 1000 - fade - time inside PIN 6.
// Depth zero 417, depth one 310, fade scope 310, shared attenuation 334, callback table 311.
// @retail 0x129aa0
void function_129aa0(short arg_0, long *arg_1, s_looping_channel_spatialization *arg_2)
{
	s_sound_voice *local_0 = &g_4e6378[arg_0];
	s_sound_playback *local_1 = SOUND_PLAYBACK_GET(local_0->sound_index);
	s_type_12a1b0_spatial *local_2 = (s_type_12a1b0_spatial *)arg_2;
	local_0->unknown0b = local_0->unknown0a;
	local_0->unknown0a = 0;
	switch (local_1->location.audible)
	{
	case 1:
	case 2:
		{
			s_sound_listener *local_3 = &SOUND_SYSTEM->listeners[local_1->listener_index];
			s_sound_position local_5;
			vector3f local_4 = *g_4687a4;
			if (local_1->location.audible == 1)
			{
				sound_source_get_position((s_sound_location_source const *)&local_1->location, local_1->listener_index, &local_5.position);
				function_142700((transform4x3f const *)&local_3->velocity_scale, &local_5.position, &local_5.position);
				if ((byte)function_125060((s_sound const *)&local_1->location, local_1->definition_index))
				{
					transform4x3f const *local_18 = (transform4x3f const *)&local_3->velocity_scale;
					vector3f *local_7 = vector3d_decompress(local_1->location.spatial.compressed_forward, &local_4);
					real local_8 = local_7->i;
					real local_9 = local_7->j;
					real local_10 = local_7->k;
					local_4.i = local_18->forward.k * local_10 + local_18->forward.j * local_9 + local_18->forward.i * local_8;
					local_4.j = local_18->left.k * local_10 + local_18->left.j * local_9 + local_18->left.i * local_8;
					local_4.k = local_18->up.k * local_10 + local_18->up.j * local_9 + local_18->up.i * local_8;
				}
				function_1427f0((transform4x3f const *)&local_3->velocity_scale, &local_1->location.spatial.velocity, &local_5.velocity);
			}
			else
			{
				local_5.position = local_1->location.spatial.position;
				if ((byte)function_125060((s_sound const *)&local_1->location, local_1->definition_index))
					vector3d_decompress(local_1->location.spatial.compressed_forward, &local_4);
				local_5.velocity = local_1->location.spatial.velocity;
			}
			if (local_1->priority & 2)
			{
				local_2->field_18 = function_12ad30((s_sound const *)&local_1->location, (vector3f const *)&local_5.position, local_1->definition_index, &local_4);
				local_0->unknown0a |= 2;
			}
			else
			{
				local_2->field_18 = function_12ad30((s_sound const *)&local_1->location, (vector3f const *)&local_5.position, local_1->definition_index, &local_4);
				*arg_1 = decibels_sum(*arg_1, function_2197f0(local_2->field_18));
				local_0->unknown0a |= 1;
			}
			if (SOUND_FLAG(local_1->location.flags, 11))
				local_0->unknown0a |= 1;
			local_2->field_1c = *(vector3f *)&local_5.position;
		}
		break;
	case 0:
		local_0->unknown0a |= 1;
		if (SOUND_FLAG(local_1->location.flags, 8))
			local_2->field_18 = 0.0f;
		else
			local_2->field_18 = 1.0f;
		local_2->field_1c = *(vector3f *)g_468788;
		break;
	}
	local_2->field_14 = (local_0->unknown0a & 1) ? 0.0f : 1.0f;
	long local_11 = SOUND_SYSTEM->time;
	if (!local_0->unknown0b)
	{
		*(long *)local_0->unknown14 = 0;
		local_2->field_8 = *((local_0->unknown0a & 1) ? &g_440c48 : &g_440c4c);
		/* retail selects the gain's address, calling only when the bit is set */
		long local_19;
		local_2->field_4 = *((local_0->unknown0a & 2) ? (local_19 = function_2197f0(local_2->field_18), &local_19) : &g_440c4c);
	}
	else
	{
		if (local_0->unknown0b != local_0->unknown0a)
		{
			long local_26 = 1000 - *(long *)local_0->unknown14;
			*(long *)local_0->unknown14 = local_11 - PIN(local_26 - local_11, 0, 1000);
		}
		long local_13 = 0;
		long local_14 = 0xc2800000;
		if (*(long *)local_0->unknown14 > 0)
		{
			long local_15 = local_11 - *(long *)local_0->unknown14;
			if (local_15 > 1000)
				*(long *)local_0->unknown14 = 0;
			else
			{
				local_13 = function_12a6d0(1, (real)local_15, 1000.0f);
				local_14 = function_12a6d0(1, (real)local_15, -1000.0f);
			}
		}
		long local_16 = (local_0->unknown0a & 2) ? local_13 : local_14;
		local_2->field_14 = function_2195f0(*(real *)&local_16);
		local_0->unknown0b = local_0->unknown0a;
		local_2->field_8 = *((local_0->unknown0a & 1) ? &local_13 : &local_14);
		long const *local_21 = (local_0->unknown0a & 2) ? &local_13 : &local_14;
		long local_24 = function_2197f0(local_2->field_18);
		*(real *)&local_24 += *(real const *)local_21;
		local_2->field_4 = local_24;
	}
	local_2->field_0 = local_0->unknown0a;
	local_2->field_c = 0;
	local_2->field_10 = 0;
	if (SOUND_FLAG(local_1->location.flags, 11))
	{
		s_sound_definition *local_22 = sound_definition_get(local_1->definition_index);
		s_unknown_5c *local_23 = function_221810(local_22->promotion_index);
		local_2->field_8 = decibels_sum(*(long *)((byte *)local_23 + 0x14), local_2->field_8);
	}
	if (!TEST_FIELD_BIT(local_1->source_updated) && local_1->source && local_1->source->spatialize)
	{
		s_type_12a1b0_spatial local_17 = *local_2;
		if (local_1->source->spatialize(local_1->object_index, local_1->definition_index, (s_sound_source_view const *)&local_1->marker, (s_sound_spatialization_view *)&local_17))
			*local_2 = local_17;
	}
}

void function_21ec00(void);
void function_1293f0(void);
void function_21ee80(void);
void sound_cache_new_frame(void);

// @retail 0x125d60
void function_125d60(void)
{
	long local_0 = SOUND_SYSTEM->field_14_2;
	local_0 = GetTickCount() - local_0;
	if (g_4e6948 && g_4e6948->flag1120 && (real)local_0 > g_510c54->rate * 1000.0f)
	{
		s_sound_system_view *local_1 = SOUND_SYSTEM;
		local_1->unknown7c = true;
		if (!local_1->changing_pause)
		{
			local_1->changing_pause = true;
			if ((byte)sound_system_available())
			{
				function_21ec00();
				sound_system_update_time();
				sound_voices_update_chunks();
				function_1293f0();
				function_21ee80();
				local_1 = SOUND_SYSTEM;
			}
			sound_cache_new_frame();
			local_1->changing_pause = false;
		}
		local_1->unknown7c = false;
	}
}

void __stdcall function_1264c0(dword arg_0);

/* stops every playing sound with a 0.3 s fade, waits up to 300 ms for the fades, then
   shuts the sound system down and forgets the impulse sounds */
// @retail 0x1257b0
void function_1257b0(void)
{
	if (!SOUND_SYSTEM->unknown7b && SOUND_SYSTEM->initialized && SOUND_SYSTEM->hardware_available && SOUND_SYSTEM->enabled)
	{
		long local_0 = GetTickCount();
		s_record_pool *local_1 = g_4e637c;
		long local_2 = data_datum_index(local_1, function_16bc00(local_1, 0));
		if (local_2 != NONE)
		{
			do
			{
				function_126df0(0, NONE, local_2, 0.3f);
				local_2 = data_datum_index(local_1, function_16bc00(local_1, local_2 == NONE ? 0 : (local_2 & 0xffff) + 1));
			} while (local_2 != NONE);
			real local_4 = (real)local_0;
			local_4 += 300.0f;
			bool waiting = true;
			while (waiting)
			{
				if (GetTickCount() < local_4)
					function_125d60();
				else
					waiting = false;
			}
		}
	}
	function_1264c0(0);
	SOUND_SYSTEM->enabled = false;
	if (g_502110)
		record_pool_release_all((s_record_pool *)g_502110);
	if (g_51ebd8)
		record_pool_release_all((s_record_pool *)g_51ebd8);
	g_51ebd4 = NULL;
	function_191270();
	for (short impulse = 0; impulse < 2; impulse++)
		SOUND_DRIVER_GLOBALS->impulse_ids[impulse] = NONE;
}

struct s_looping_channel_properties;
struct s_looping_effect_playback;
struct s_looping_impulse_parameters;
void function_12a1b0(short arg_0, s_looping_track_sound *arg_1, s_looping_channel_spatialization const *arg_2, s_looping_channel_properties *arg_3);
void function_21f8a0(long arg_0, s_looping_channel_properties const *arg_1, s_looping_effect_playback const *arg_2);
void function_21fa80(long arg_0, s_looping_channel_properties const *arg_1, s_looping_effect_playback const *arg_2, bool arg_3);
void __stdcall function_21f720(s_looping_impulse_parameters const *arg_0);

struct s_128ca0
{
	dword field_0;
	long field_4;
	real field_8;
	long field_c;
	byte field_10[0x1c];
	long field_2c;
	dword field_30;
	short field_34;
	byte field_36[0x48 - 0x36];
	byte field_48[0x420];
	byte field_468[0x10];
};

// score 215 (was 215): Frame 0x49c versus 0x494; initial gain argument homes and joined integer clamp stores remain.
// Aggregate packet 314; later packet lifetime 288; role names 241; real gain field 294; full memset 321.
// Existing shared bit clamp 234; unsigned gain carrier 241; shared branch gain workspace 241;
// long next-chunk candidate 254; completed-chunk early exit 241.
// Shared clamp common result 255; forced gain-add inline 234; gain before packet 234; long controller count 235; named
// flag masks 234.
// Both branch gain names 234; raw gain before pitch 268 (frame 0x494); playback-bound rereads 234;
// pitch-range rereads 258; existing-stream branch first 235; recovered-frame form with shared scale 237.
// Definition after packet initialization 237; branch gain writeback 234; current-chunk candidate 234;
// sequential gain workspace 234; direct processed-gain initialization 234 (fast); all restored.
// Pitch lower snapshot 273; native flag3 view 236; pitch workspace subtraction 234; zero-default controller gain 267; readonly parameter/range views 234 (fast); all restored.
// Shared sum accumulator 234; commuted sum 233; returned bit home 234; native sum
// inputs 234; native inputs plus bit home 234; early raw gain with accumulator,
// commuted sum or native inputs 268 each (frame 0x494, worse count).
// Native nested packet with original/part/whole/aggregate clears 234/304/316/309;
// reverse part clears/entry declaration/late packet/entry with part clears 304/234/283/304.
// Original packet view and clear boundaries restored.
// Round 160: Native permutation gain/count, controller gain array, promotion coefficient and all three
// together 234 each; native clear-member addresses 234; chained packet flags 293; upper-first shared pin
// 234; pin input home name 234. Original fields, clear ranges and helpers restored; protected chunk
// interface unchanged.
// Round 356: Native playback-parameter value 311; named pitch span and complete promotion
// adjustment 234/234; parameter/span 311; parameter/adjustment 311; span/adjustment 234;
// all three 311; with native pitch-range value 337. Original input pointers and expression parts restored.
// Round 254: live voice-table fields, alone/with scale/raw-gain-first/both 293/249/277/302;
// the same four with live pitch-parameter fields 293/249/293/244. All regress;
// original voice/parameter receivers, scale observations, gain schedule and body restored.
// Round 195: Processed-gain producer ordinary/noinline/combined declarations 234 each;
// forceinline 373. With raw gain before pitch 268/268/268/374; the 268 forms use frame
// 0x494. Both declaring files and original body restored; gain homes and clamp joins remain.
// Lane 10 r13: the extra frame words are decibels_add's copies of the gain and location fields
// (retail reads both in place and copies only the gain argument). With the file's decibels_sum:
// outer and inner 279 (frame 0x494; a field bound through the dword type still gets a copy);
// inner only 279; outer only 234 (identical); both with the location field cast to long 315;
// outer (cast) with decibels_add inside 272; both, with the gain argument a dword (so only it is
// copied, retail's loads and copies) 279: frame 0x494, but retail adds location + inner with the
// location loaded first and shares one scale load between the pitch and the 0x1251e0 argument,
// so ours is one instruction short and every later line shifts; that with a scale local shared by
// the pitch and the call 258 (retail's shared scale load; the location add and the pitch/argument
// temporaries' slots remain); with decibels_add outside 260. All restored.
// Round 219: readonly conditional next chunk 239; complete countdown arms 215 (plain, kept); both
// 227; word countdown 229; conditional/word 234; complete arms/word 224; all 217, with role names
// 217. Sub-eight gains 227/229 restored; all other forms restored.
// Round 154 (fast): one long gain seeded from arg_1, updated in place (`*(real *)&gain = location
// + (playback gain + gain)`, then `gain = function_1251e0(...)`) with one shared scale local 241
// (frame 0x494, opening exact up to the pitch bounds); plus 0x126000's pitch_bounds pointer with
// gain/pitch names 238 (first difference +0xba, aligned listing 108 against 147); gain declared
// first 256; real gain 295; outer operands swapped, decibels_sum on the variable, in-place
// permutation sums 238 each (identical code). Retail loads the location gain into its own register
// before the inner sum (one instruction more); every form folds it into addss, and that one-line
// shift dominates the positional score. Next: find that load, then the single-exit pin.
// Round 168: Native processed real gain with original/early declaration and signed/unsigned raw bits 215 each; all four with shared scale 287. Original body restored; gain homes and clamp joins remain.
// Round 247: sequential inner/outer/both reference-input gain sums 269/215/269; both with shared
// scale 242, before pitch 276, before pitch with scale 256, role names 269 and entry declaration
// with scale 242. Reference-input sequences recover frame 0x494 but regress; original body restored.
// Round 224: Live definition/permutation/playback, definition-permutation/definition-playback/
// permutation-playback/all/all-pitch-parameters 307/364/681/385/709/864/878/878. Definition-only
// recovers frame 0x494 but regresses; original receivers, gain homes and complete countdown arms restored.
// Round 270: actual signed-long location-gain field alone/outer reference sum/both
// reference sums 215/256/306; outer/both with shared scale 257/295; both with scale
// and native pitch bounds 295; both with early gain/early gain and scale 306/295.
// Outer sum recovers frame 0x494 but regresses; original body and shared field restored.
// Round 161: real gain sum (location + (playback gain + argument)) with one scale local 295 (frame 0x494;
// the argument is added in place where retail copies it, the location folded where retail loads it
// first); without the scale local 306; scale local with the original sums, pitch only/both 215/286;
// real outer sum over decibels_add's inner 255. All restored.
// Round 164: inner decibels_sum with `(dword)arg_1` inside the outer decibels_add 255 (frame 0x494,
// exact to +0x7e; the location field is still copied, retail loads it into XMM1 first); outer
// decibels_sum over the by-value inner 214; operands swapped by value 286; first form plus a shared
// scale local 255 (0x49c); both reference sums inner-first 255.
// Round 235: native real incoming gain with direct/inner-part/location-value/both forms
// 306/306/306/306; each with shared scale 295. Frame 0x494 recovered, gain and clamp
// loads still differ; original signature, caller argument and executable body restored.
// @retail 0x128ca0
void function_128ca0(short arg_0, long arg_1, s_looping_channel_spatialization const *arg_2)
{
	s_sound_voice *local_0 = &g_4e6378[arg_0];
	s_sound_playback *local_1 = SOUND_PLAYBACK_GET(local_0->sound_index);
	s_sound_definition *local_2 = sound_definition_get(local_1->definition_index);
	s_128ca0 local_3;
	local_3.field_0 = 0;
	memset((byte *)&local_3 + 4, 0, 0x2c);
	local_3.field_30 = 0;
	memset((byte *)&local_3 + 0x34, 0, 0x444);
	s_sound_playback_parameters *local_4 = &SOUND_GLOBALS_DEFINITIONS->playback_parameters[local_2->playback_index];
	real local_5 = ((real)local_4->pitch_upper - (real)local_4->pitch_lower) * local_1->location.scale + local_1->pitch + (real)local_4->pitch_lower;
	long local_6 = decibels_add(local_1->location.unknown08, decibels_add(*(long *)&local_1->gain, arg_1));
	local_6 = function_1251e0(local_2, local_6, local_1->location.scale);
	function_12a1b0(arg_0, (s_looping_track_sound *)local_1, arg_2, (s_looping_channel_properties *)&local_3);
	s_sound_pitch_range *local_7 = &SOUND_GLOBALS_DEFINITIONS->pitch_ranges[local_2->pitch_range_base + local_1->pitch_range_index];
	s_sound_permutation const *local_8 = &SOUND_GLOBALS_CHUNKS->permutations[local_7->first_permutation + local_1->permutation_index];
	local_3.field_34 = 0;
	local_3.field_2c = local_1->effect_index;
	local_3.field_8 = local_5 - (real)SOUND_GLOBALS_DEFINITIONS->pitch_bounds[local_7->bounds_index].unknown00;
	if (local_1->priority & 2)
	{
		/* retail calls the promotion lookup here */
#pragma inline_depth(0)
		s_unknown_5c *local_9 = function_221810(local_2->promotion_index);
#pragma inline_depth()
		real local_10;
		if (local_1->effect_index != NONE)
		{
			s_sound_controller_view *local_11 = (s_sound_controller_view *)((s_record_pool *)g_51ebd8)->data + (local_1->effect_index & 0xffff);
			local_10 = local_11->unknown04 != NONE ? ((real *)((byte *)local_11 + 0xc))[local_1->listener_index] : 0.0f;
		}
		else
			local_10 = function_12a9d0(local_1->listener_index, &local_1->location.spatial);
		local_3.field_8 += *(real *)((byte *)local_9 + 0x48) * ((s_type_12a1b0_spatial const *)arg_2)->field_14 * local_10;
	}
	if (!(bool)((local_1->flags >> 9) & 1))
	{
		real local_12 = *(real *)&local_6 + (real)*(signed char *)((byte const *)local_8 + 4);
		local_3.field_c = decibels_pin(*(long *)&local_12);
		function_21f8a0(local_0->channel_index, (s_looping_channel_properties *)&local_3, (s_looping_effect_playback *)local_3.field_48);
		function_21f720((s_looping_impulse_parameters *)local_3.field_468);
		sound_voice_queue_chunk(arg_0, local_8, local_1->chunk_index);
		if (local_1->effect_index != NONE)
		{
			s_sound_controller_view *local_13 = (s_sound_controller_view *)((s_record_pool *)g_51ebd8)->data + (local_1->effect_index & 0xffff);
			byte local_14;
			if (local_13->unknown04 != NONE)
			{
				local_14 = local_13->playing_count & 0x7f;
				local_13->playing_count = (local_13->playing_count - 1) & 0x7f;
			}
			else
				local_14 = 0xff;
			local_1->unknown03 = local_14;
		}
		local_1->flags |= 0x200;
	}
	else if (local_0->permutation)
	{
		real local_15 = *(real *)&local_6 + (real)*(signed char *)((byte const *)local_0->permutation + 4);
		local_3.field_c = decibels_pin(*(long *)&local_15);
		function_21fa80(local_0->channel_index, (s_looping_channel_properties *)&local_3, (s_looping_effect_playback *)local_3.field_48, true);
	}
	if (!TEST_FIELD_BIT(local_1->flag6))
	{
		if (TEST_FIELD_BIT(local_1->flag3) && !local_0->next_permutation && (byte)function_125e60((s_looping_track_sound *)local_1))
		{
			sound_voice_queue_chunk(arg_0, local_8, local_1->chunk_index);
			local_1->flag3 = false;
		}
		if (!TEST_FIELD_BIT(local_1->flag3))
		{
			short local_16 = NONE;
			if (local_8 && local_1->chunk_index < *(short *)((byte const *)local_8 + 0xe) - 1)
				local_16 = local_1->chunk_index + 1;
			if (local_16 != NONE)
			{
				sound_playback_set_chunk(local_0->sound_index, local_1->definition_index, local_1->pitch_range_index, local_1->permutation_index, local_16);
				function_125e60((s_looping_track_sound *)local_1);
				local_1->flag3 = true;
			}
			else
			{
				local_1->flag6 = true;
				SOUND_DRIVER_STREAMS->streams[g_4e6378[arg_0].channel_index].unknown03_3 = true;
			}
		}
	}
}

struct s_sound_driver_channel_usage;
void __stdcall function_21f360(s_sound_driver_channel_usage *arg_0);
void function_21c400(short arg_0, real arg_1, s_looping_channel_spatialization const *arg_2);

// @retail 0x1293f0
void function_1293f0(void)
{
	s_sound_system_view *local_0 = SOUND_SYSTEM;
	long local_1 = function_2197f0(local_0->master_fade);
	function_21f360((s_sound_driver_channel_usage *)local_0);
	for (long local_2 = 0; local_2 < SOUND_SYSTEM->voice_count; local_2++)
	{
		s_sound_voice *local_3 = &g_4e6378[(short)local_2];
		if (local_3->sound_index == NONE || local_3->stream_reset)
			continue;
		s_sound_playback *local_4 = SOUND_PLAYBACK_GET(local_3->sound_index);
		long local_5 = function_12a810(local_3->sound_index);
		long local_9 = local_5;
		if (*(real *)&local_9 + *(real *)&local_4->location.unknown08 <= -64.0f)
		{
			s_sound_playback *local_6 = SOUND_PLAYBACK_GET(local_3->sound_index);
			if (TEST_FIELD_BIT(local_6->fading) && local_6->fade_start_time > local_6->fade_end_time)
			{
				function_127320(local_3->sound_index, 4);
				local_3->sound_index = NONE;
				continue;
			}
		}
		s_sound_playback *local_7 = SOUND_PLAYBACK_GET(local_3->sound_index);
		if (TEST_FIELD_BIT(local_7->fading) && local_7->fade_end_time > local_7->fade_start_time && SOUND_SYSTEM->time >= local_7->fade_end_time)
			*(byte *)&local_7->flags &= ~0x80; /* the fading bit, in the low byte */
		if (local_3->channel_index == NONE)
			continue;
		s_type_12a1b0_spatial local_8;
		function_129aa0((short)local_2, &local_5, (s_looping_channel_spatialization *)&local_8);
		*(real *)&local_5 = *(real *)&local_5 + *(real *)&local_1;
		if ((1 << SOUND_PLAYBACK_GET(local_3->sound_index)->state) & 0x1e)
			function_21c400((short)local_2, *(real *)&local_5, (s_looping_channel_spatialization *)&local_8);
		else
			function_128ca0((short)local_2, local_5, (s_looping_channel_spatialization *)&local_8);
		if (!TEST_FIELD_BIT(local_4->source_updated) && local_4->source && local_4->source->proc2)
			local_4->source->proc2(local_4->object_index, (long)local_4->source_data, local_4->definition_index, local_4->pitch_range_index, (long)local_3->permutation, *(long *)&local_3->unknown10);
	}
}

long function_155760(long arg_0);
void function_caf60(long arg_0, point3f *arg_1);
void function_11d6a0(quaternionf *arg_2, vector3f const *arg_0, vector3f const *arg_1);
void function_11d950(quaternionf const *arg_0, quaternionf const *arg_1, quaternionf *arg_2, real arg_3);
extern quaternionf *g_4687cc;
void function_220790(s_sound_listener const *arg_0);

struct s_127401
{
	point3f field_0;
	s_location field_c;
	vector3f field_14;
	vector3f field_20;
	vector3f field_2c;
};

struct s_127402
{
	s_sound_driver_reverb const *field_0;
	s_sound_driver_occlusion field_4;
	real field_14;
};

struct s_127403
{
	point3f field_0;
	vector3f field_c;
	vector3f field_18;
	vector3f field_24;
	long field_30;
	s_127402 const *field_34;
};

PRIVATE inline void function_1275d8(quaternionf const *arg_0, vector3f const *arg_1, vector3f *arg_2)
{
	real rotation_i = arg_0->i;
	real rotation_j = arg_0->j;
	real rotation_k = arg_0->k;
	real rotation_w = arg_0->w;
	real local_0 = 2.0f * rotation_w * rotation_w - 1.0f;
	real local_1 = 2.0f * (rotation_j * arg_1->j + rotation_k * arg_1->k + rotation_i * arg_1->i);
	real local_2 = 2.0f * rotation_w;
	arg_2->i = (rotation_j * arg_1->k - rotation_k * arg_1->j) * local_2 + rotation_i * local_1 + local_0 * arg_1->i;
	arg_2->j = (rotation_k * arg_1->i - rotation_i * arg_1->k) * local_2 + rotation_j * local_1 + local_0 * arg_1->j;
	arg_2->k = (rotation_i * arg_1->j - rotation_j * arg_1->i) * local_2 + rotation_k * local_1 + local_0 * arg_1->k;
}

// score 535 (was 535): Frame 0x130 versus 0xe8; native rotation, environment and listener-index homes remain.
// Const reverb 551; outer records 551; shared cross product 635; direct blend reads 555; entry counter 552.
// Output-before-inputs callee order 551 (callee 15 to 11); all call argument roles preserved.
// Static const fallback 727 (frame 0xec); spatial role names 551; long success workspace 551;
// short listener counter 581; outer spatial workspace 551; interpolated point setter 615;
// position-zero setter 616.
// Quaternion input values 535 kept (plain, frame 0x130); vector input values 647; both input sets 1057; entry
// quaternion 551; entry queried position 551.
// Weighted cross terms 596; projection products 596; scaled-input terms 590; rotation subtotals 596;
// vector-first publication 535 (fast); all restored.
// Shared handle query 575; complete rotation outputs 1050; quaternion cross helper 1050; unsigned listener ordinal 557; plain environment cursor 649; positive entry scope 535 (fast); all restored.
// Native quaternion-reference 535; quaternion-value 992; vector-reference 535; vector-value 1011;
// both references 535; both values 956; quaternion/output references 535; value inputs/output reference 956.
// Round 160: Rotation scale declaration orders 0/2/1 583, 1/0/2 589, 1/2/0 589, 2/0/1 586, 2/1/0 1050;
// quaternion acquisitions w/i/j/k 538 and k/j/i/w 545; native scale-home names 535. Original quaternion
// snapshots, expressions, helper contract and body restored.
// Rotation output orders ikj/jik/jki/kij/kji 545/560/554/541/550;
// uniform projection/scaled-input/cross-scale multiply operand orders 535/535/535.
// publication aggregate 535; fallback entry lifetime 558; aggregate entry lifetime 558; environment
// observations 537; listener observations 586; record observations 619; aggregate record observations 619;
// forced existing rotation 535. All restored.
// Placement/environment status byte-bool, word-bool, bool-byte, bool-word, byte-byte, byte-word, word-byte,
// word-word; then five remaining complete quaternion/input/output formal orders:
// 535/535/535/554/535/554/535/554/560/568/568/560/535 (fast); all restored.
// Round 56: retail reads the default reverb from the existing const g_44a2a0 (0x48 bytes: our
// frame excess) and inlines 0x14ddc0/0x14de70 (the `i != NONE` and `setne cl` player test),
// 0x16f3a0 (the `cmovne` state pointer) and has the fallback copy in both arms. g_44a2a0 (made
// extern in sound_driver.h) 760 (fast; frame 0xe8, 2065 bytes); with the three helpers and the
// fallback in both arms 583 (frame 0xe0, 2243 bytes, aligned 650 -> 621; 0x16f3a0 stays out of
// line and the first 0x155760 call sinks); plus an `inline` redeclaration of 0x16f3a0 583;
// g_44a2a0 with only the helper loop head 637; the helper loop head alone 597. All restored.
// Native rotation return, pointer/quaternion-value/vector-value/both-value inputs 1009/948/965/912;
// those native returns with the existing fallback constant 1217/1162/1176/1126. All restored.
// Round 364: raw cross parts at each row/before outputs/before coefficients
// 535/1050/1050; named dot parts 544; with those raw-cross boundaries
// 542/1050/1050; joint entry coefficient declarations 1050. Original helper restored.
// Round 219: native rotation/left/rotation+left setters 1050/647/986; velocity setter 543;
// rotation+velocity 1045; left+velocity 646; all 986; with interpolated point setter 986. Original
// restored.
// Round 221: original native fallback copied from the existing immutable object by aggregate/library,
// each with aggregate/library occlusion and field/aggregate publication: 739 each, 2085 bytes.
// All eight restored; original helper and stack fallback retained; temporary declaration removed.
// Round 154 (fast): existing 0x16f3a0 getter for the state pointer 537 (aligned 486 -> 482); the
// fallback asking the getter again 543; getter plus 0x14ddc0/0x14de70 at the loop head 555 (aligned
// 461); that plus the existing const g_44a2a0 (extern in sound_driver.h) 719 (frame 0xe0, 2106 bytes,
// aligned 410); plus the fallback getter 747. Retail evaluates the inlined 0x16f3a0 cmov again in its
// second fallback copy and inlines 0x1420f0/0x1427f0; BLOCKED-BY 0x16f3a0 (noinline for its callers).
// Round 389: query directly into the final point 563; first/second/both rotations in place
// with complete vector snapshots 1030/1035/1008; query reuse with those graphs 1042/1047/1020;
// complete reuse with native output-pointer return 1020. Original distinct workspaces restored.
// Round 259: existing rotation output pointer/reference results, pointer/reference output
// formals and ordinary/forced expansion: eight forms 535 each. Original helper restored.
// Round 194: shared zone result/local/direct/entry 555/554/555; native player-state member 535;
// shared zone plus native member 555; with complete blend reads 582; with source-state reads 836;
// both complete observations 799. Original result, source pointer and body restored.
// Round 164: state pointer sunk into the branch and recomputed in the fallback 598; with 0x16f3a0
// calls 553; fallback in both arms 633, with 0x16f3a0 calls 706, plus 0x14ddc0/0x14de70 at the loop
// head 740 (retail's loop head, aligned 426 -> 402).
// @retail 0x127400
void function_127400(bool arg_0)
{
	if (!g_4e6948 || !g_4e6948->flag1120)
		return;
	for (long local_0 = 0; local_0 < 4; local_0++)
	{
		s_sound_listener *local_1 = &SOUND_SYSTEM->listeners[local_0];
		long local_2 = g_4e8c20->entries[local_0];
		if (local_2 != NONE)
		{
			long local_3 = *(long *)(g_4e8c24->data + (local_2 & 0xffff) * 0x21c + 0x2c);
			short local_4 = (short)function_155760(local_0);
			s_player_4e9bd4 const *local_5 = g_4e9bd4;
			s_127401 const *local_6 = g_4686c4 != NONE ? (s_127401 const *)((byte const *)(local_5 + local_0) + 0xb8) : NULL;
			vector3f local_7;
			vector3f local_8;
			point3f local_9;
			vector3f local_10;
			s_location local_11;
			bool local_12 = false;
			if (local_3 != NONE && ((1 << function_155760(local_0)) & 3))
			{
				byte *local_13 = *(byte **)(g_4e0300->data + (local_3 & 0xffff) * 12 + 8);
				long local_14 = *(long *)(local_13 + 0x14);
				short local_15 = *(short *)(local_13 + 0x1fc);
				if (local_14 != NONE && local_15 != NONE && local_4 != 0)
				{
					byte *local_16 = *(byte **)(g_4e0300->data + (local_14 & 0xffff) * 12 + 8);
					byte *local_17 = g_4e3b44[*(long *)local_16 & 0xffff].bytes;
					byte *local_18 = *(byte **)(local_17 + 0x1cc) + local_15 * 0xb0;
					real local_19 = *(real *)(local_18 + 0x40);
					point3f local_20;
					function_caf60(local_3, &local_20);
					local_9.x = (local_20.x - local_6->field_0.x) * local_19 + local_6->field_0.x;
					local_9.y = (local_20.y - local_6->field_0.y) * local_19 + local_6->field_0.y;
					local_9.z = (local_20.z - local_6->field_0.z) * local_19 + local_6->field_0.z;
					quaternionf local_21;
					function_11d6a0(&local_21, &local_6->field_20, (vector3f *)(local_13 + 0x180));
					function_11d950(g_4687cc, &local_21, &local_21, local_19);
					function_1275d8(&local_21, &local_6->field_20, &local_7);
					function_1275d8(&local_21, &local_6->field_2c, &local_8);
					function_ba1d0(local_3, &local_10, NULL);
					function_11bed0(&local_11, &local_9);
					if (local_11.cluster_index == NONE)
						local_11 = local_6->field_c;
					local_12 = true;
				}
			}
			if (!local_12)
			{
				local_7 = local_6->field_20;
				local_8 = local_6->field_2c;
				local_9 = local_6->field_0;
				local_10 = local_6->field_14;
				local_11 = local_6->field_c;
			}
			local_1->active = true;
			bool local_22 = false;
			if (local_11.cluster_index != NONE)
			{
				byte *local_23 = (byte *)g_4e0348;
				byte *local_24 = *(byte **)(local_23 + 0xa0) + local_11.cluster_index * 0xb0;
				byte local_25 = local_24[0x70];
				if (local_25 != 0xff)
				{
					byte *local_26 = *(byte **)(local_23 + 0x68) + (local_25 & 0x7f) * 0x18;
					if (*(short *)(local_26 + 2) != NONE)
					{
						if (!(local_25 & 0x80))
							local_22 = true;
						else
							local_22 = *(real *)(local_26 + 0xc) * local_9.z + *(real *)(local_26 + 8) * local_9.y + *(real *)(local_26 + 4) * local_9.x - *(real *)(local_26 + 0x10) < 0.0f;
					}
				}
			}
			local_1->unknown07 = local_22;
			local_1->velocity_scale = 1.0f;
			local_1->forward = local_7;
			local_1->left.i = local_8.j * local_7.k - local_8.k * local_7.j;
			local_1->left.j = local_8.k * local_7.i - local_8.i * local_7.k;
			local_1->left.k = local_8.i * local_7.j - local_8.j * local_7.i;
			local_1->up = local_8;
			local_1->position.x = 0.0f;
			local_1->position.y = 0.0f;
			local_1->position.z = 0.0f;
			local_1->position = local_9;
			if (local_1->velocity_scale != 1.0f)
			{
				real local_27 = 1.0f / local_1->velocity_scale;
				local_10.i *= local_27;
				local_10.j *= local_27;
				local_10.k *= local_27;
			}
			local_1->velocity.i = local_1->forward.j * local_10.j + local_1->forward.k * local_10.k + local_10.i * local_1->forward.i;
			local_1->velocity.j = local_1->left.i * local_10.i + local_1->left.j * local_10.j + local_1->left.k * local_10.k;
			local_1->velocity.k = local_1->up.j * local_10.j + local_1->up.k * local_10.k + local_1->up.i * local_10.i;
			local_1->leaf_index = local_11.leaf_index;
			local_1->cluster_index = local_11.cluster_index;
		}
		else
		{
			local_1->leaf_index = NONE;
			local_1->cluster_index = NONE;
		}
	}
	if (arg_0)
	{
		s_sound_driver_reverb local_28 = {
			{0, 0, 0, 0, 0, 0x80, 0, 0},
			-100.0f, -100.0f, 0.0f, 1.0f, 1.0f, -100.0f, 0.0f, -100.0f, 0.0f, 1.0f, 1.0f, 5000.0f
		};
		s_127402 local_29[2];
		for (long local_30 = 0; local_30 < 2; local_30++)
		{
			s_sound_environment *local_31 = &SOUND_SYSTEM->environments[local_30];
			local_29[local_30].field_0 = local_31->index == NONE ? &local_28 : (s_sound_driver_reverb *)g_4e3b44[local_31->index & 0xffff].bytes;
			local_29[local_30].field_4 = *(s_sound_driver_occlusion *)&local_31->unknown0c;
			local_29[local_30].field_14 = local_31->fade;
		}
		s_127403 local_32;
		local_32.field_0 = *g_468788;
		local_32.field_c = *g_4687a8;
		local_32.field_18 = *g_4687b0;
		local_32.field_24 = *g_4687a4;
		local_32.field_30 = 2;
		local_32.field_34 = local_29;
		function_220790((s_sound_listener const *)&local_32);
	}
}

#include "physical_memory.h"
void function_221900(real arg_0);
void looping_sound_controllers_update(void);
void function_21b0e0(bool arg_0);
long players_first_active_local_player(void);
void __stdcall function_221a70(long arg_0, s_voice_position_batch *arg_1);

/* the per-voice and per-channel value pointers of one spatialization pass, cleared when declared
   (a plain memset of a local this large makes VC realign the frame with `and esp, -8`; retail's
   clears don't, and a clearing constructor or member function is the only form found that avoids it) */
struct s_voice_value_table
{
	vector3f *values[0x40];

	s_voice_value_table()
	{
		memset(values, 0, sizeof(values));
	}
};

struct s_channel_value_table
{
	vector3f *values[0x80];

	s_channel_value_table()
	{
		memset(values, 0, sizeof(values));
	}
};

// @retail 0x125b40
void function_125b40(void)
{
	if (SOUND_SYSTEM->initialized && SOUND_SYSTEM->hardware_available && SOUND_SYSTEM->enabled)
	{
		SOUND_SYSTEM->changing_pause = true;
		function_21ec00();
		s_sound_system_view *local_0 = SOUND_SYSTEM;
		if (local_0->enabled)
		{
			if (local_0->unknown7b && (!g_510c54->active || !g_510c54->unknown01))
			{
				local_0->changing_pause = true;
				local_0->unknown7b = false;
				for (long local_1 = 0; local_1 < SOUND_SYSTEM->voice_count; local_1++)
					sound_voice_restart_stream((short)local_1);
				looping_sound_controllers_synchronize(true);
				function_21f5a0();
				dword local_2 = GetTickCount();
				local_0 = SOUND_SYSTEM;
				local_0->field_14_2 = local_2;
				local_0->changing_pause = false;
			}
			function_21f360((s_sound_driver_channel_usage *)local_0);
			if (!local_0->unknown7b)
			{
				sound_system_update_time();
				local_0 = SOUND_SYSTEM;
			}
			function_221900(local_0->elapsed_time);
			function_127400(true);
			looping_sound_controllers_update();
			function_21b0e0(*(bool *)&SOUND_SYSTEM->unknown7e[0]);
			sound_voices_update_chunks();
			function_128020();
			function_1282d0();
			s_voice_position_batch local_5;
			local_5.count = 0;
			s_voice_value_table local_3;
			s_channel_value_table local_4;
			function_129100(&local_5, local_3.values, local_4.values);
			long local_6 = players_first_active_local_player();
			while (local_6 != NONE)
			{
				function_221a70(local_6, &local_5);
				local_6 = local_player_next_inline(local_6);
			}
			function_129380(local_3.values, local_4.values);
			function_1293f0();
			if (SOUND_SYSTEM->unknown7e[0])
				SOUND_SYSTEM->unknown7e[1]++;
		}
		function_21ee80();
		looping_sound_controllers_synchronize(false);
		SOUND_SYSTEM->unknown7e[0] = 0;
		SOUND_SYSTEM->changing_pause = false;
	}
	physical_memory_new_frame((s_physical_object *)g_50210c);
}
