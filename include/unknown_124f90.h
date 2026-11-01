/* UNKNOWN_124F90.H: the playing sounds of the sound manager (src/unknown_124f90.cpp)
   and the request a sound is started from (0x21d110 builds it) */
#ifndef UNKNOWN_124F90_H
#define UNKNOWN_124F90_H

#include "unknown_11c920.h"
#include "sound_sources.h"

/* what a sound playing through a sound effect (unknown_21d110.cpp) adds to its
   marker: the effect and the source the sound had before */
struct s_sound_effect_link
{
	long effect_index;
	s_sound_source_callbacks const *source;
};

struct s_sound_effect_marker
{
	s_sound_marker marker;
	s_sound_effect_link link;
};

/* what a sound is started from (0xa0 bytes); the flags say which of the
   optional fields are set */
enum
{
	_sound_play_state_has_flags_bit = 0x2,
	_sound_play_state_has_object_bit = 0x8,
	_sound_play_state_has_effect_bit = 0x10,
	_sound_play_state_has_source_bit = 0x20,
	_sound_play_state_has_value8e_bit = 0x40,
	_sound_play_state_has_gain_bit = 0x80,
	_sound_play_state_has_platform_playback_bit = 0x100,
	_sound_play_state_has_value90_bit = 0x200,
	_sound_play_state_has_variants_bit = 0x400,
};

struct s_sound_play_state
{
	dword flags;
	short priority;
	byte unknown06[2];
	long playback_flags;
	s_type_99c531 location;
	long object_index;
	long effect_index;
	s_sound_source_callbacks const *source;
	union
	{
		s_sound_marker marker;
		s_sound_effect_marker effect_marker;
		byte source_data[0x30];
	};
	short source_data_size;
	short value8e;
	char value90;
	byte unknown91[3];
	real gain;
	long platform_playback;
	word variant0;
	word variant1;
};

/* the flags of a playing sound */
struct s_sound_playback_flags
{
	word flag0 : 1;
	word holds_reference : 1;
	word flag2 : 1;
	word flag3 : 1;
	word flag4 : 1;
	word source_updated : 1;
	word unknown04 : 10;
};

/* a sound being played, in the 0x4e637c array (0xbc bytes) */
struct s_sound_playback
{
	word identifier;
	char state;
	char unknown03;
	union
	{
		word flags;
		struct
		{
			word flag0 : 1;
			word holds_reference : 1;
			word flag2 : 1;
			word flag3 : 1;
			word flag4 : 1;
			word source_updated : 1;
			word flag6 : 1;
			word fading : 1;
			word unknown04 : 8;
		};
	};
	short priority;
	long platform_playback;
	long definition_index;
	long object_index;
	s_sound_source_callbacks const *source;
	s_type_99c531 location;
	union
	{
		s_sound_marker marker;
		byte source_data[0x30];
	};
	long start_time;
	real pitch;
	real gain;
	dword seed;
	char pitch_range_index;
	char permutation_index;
	short chunk_index;
	char value_a0;
	char listener_index;
	short value_a2;
	char value_a4;
	byte unknown_a5[3];
	long effect_index;
	short value_ac;
	short fade_curve;
	long fade_gain;		/* decibels, as real bits */
	long fade_start_time;
	long fade_end_time;
};

/* clamps value to [lower, upper], testing the lower bound first */
#define PIN_126660(value, lower, upper) ((value) < (lower) ? (lower) : (value) > (upper) ? (upper) : (value))

/* adds delta to value, kept within [-maximum, maximum] */
static inline void approach_126660(real *value, real delta, real maximum)
{
	*value += PIN_126660(delta, 0.0f - maximum, maximum);
}

#define SOUND_PLAYBACK_GET(index) (&((s_sound_playback *)g_4e637c->data)[(index) & 0xffff])

#endif
