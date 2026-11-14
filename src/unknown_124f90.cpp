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
#include "unknown_21e230.h"
#include "bit_vector.h"
#include "unknown_221810.h"
#include "sound_driver.h"
#include "crc.h"
#include "location_zones.h"
#include <xtl.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "matrix_transform.h"

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
	s_decibels gain;
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

static inline s_decibels decibels_interpolate(s_decibels a, s_decibels b, real t)
{
	return (b.value - a.value) * t + a.value;
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
// Round 73 (fast; the compile-only build hoists the g_4e6378 load the LTCG build reloads each pass, so
// it cannot screen this): a pointer to the per-type count for the sum and the bound 3; the voice pointer
// as `g_4e6378 + voice_index` with the index advanced in its own statement 3; a long type counter 28;
// the bound as a short member of the counts struct 3; `system->voice_count +=` 3, with the two final
// stores through system too 59 (561 bytes).
// Round 129: typed three-count array reference 3; whole count-record pointer 3; const voice pointer 3;
// short-sentinel chain 5; stream_reset beside definition_type 11; driver identifier beside sound
// identifier 8; pointer fields before short sentinels 8; definition_type after the zero bytes 5.
// Round 203 (fast, under a 0x219b40 CAVEAT; the fast baseline agreed with plain): the voice pointer declared at block scope 3; the type counter declared ahead of
// the voice index 3; a walking per-type count pointer advanced with the type 5; a long inner
// counter 40 (551 bytes); the two zero bytes as one chained store 5.
// Round 70 (lane 10, fast; a one-file LTCG harness reproduces the loop): the voice stores in a forceinline reset
// helper taking the voice pointer 3, taking the voice index 3, or with the index advanced in its own statement 3; a
// const pointer to the per-type counts for the sum and the bound 3; the counts table without const 3. Harness: probe
// locals (1-15 at the top, 1-8 in the loop) and the counts read through a pointer local move nothing.
// Round 171 (lane 4, fast): channel and secondary indices as one chained NONE store 5 (the word stores swap); both
// permutation pointers as one chained NULL store 5; the voice index declared after the bit-vector fills 9; the inner
// bound on the left of the compare 5 (`cmp [ebp], si`); the type counter declared ahead of its for 3.
// Lane 15 round 62 (fast): definition_type stored without the `(byte)` cast 3; voice_index declared before the
// system reload 3; the inner bound assembled from the count bytes 122 (607 bytes); the voice index advanced right
// after the first store 3; stream_reset stored as 0 3.
// Round 174 (m3 lane 4; one-file LTCG harness of this file with its eight callees' files and the ballast, exact at 3): the
// pool reset through the existing 0x16b790 34 (fast 34); the counts table at file scope 3, at file scope without const 3;
// the voice as `g_4e6378 + voice_index++` 3; a two-use count reference for the sum and the bound 3. Diagnostics (wrong
// values): the bound as a constant 70, read from an extern array 64, the sound index stored as 0 3, the type stored last
// 11, the voice index not advanced 62; `inc esi` stays right after `inc edi` in every one, so neither the table's aliasing
// nor the first stores' operands place it.
// Round 145 (m3 lane 8, fast under the 0x219b40 CAVEAT; the plain baseline and one plain diagnostic read 3 too): the stores from unknown0a on inside `do { } while (false)` 3; those
// stores through a forceinline clear helper taking the voice 3; the voice lookup with its first two stores in a
// forceinline helper returning the voice 3 (all three the same code); the voice as `g_4e6378[voice_index + i]` with the
// index advanced by the count after the inner loop 85 (574 bytes); the per-type loop in a forceinline helper returning
// the next voice index 54 (555 bytes). Permuter (47 tries, 600 s): best 2, the listed type-before-sound-index store
// swap, nothing else below 3. Diagnostic (wrong value): `unknown0b = (byte)i` 90; the scheduler then reorders the
// stores and each increment follows its register's last use, so the stores are not pinned to source order.
// Round 235 (m3 lane 3; one-file LTCG harness, exact at 3): `voice_count = voice_count + count` 3; the type loop as a while
// with `type++` at its end 3; `++voice_index;` as its own statement 3; the type stored before the sound index 2 (re-measured,
// match-only); the stores from unknown0a on inside a one-pass for 3, `switch (0) { case 0: }` 3, `while (true) { ... break; }` 3,
// `if (true)` 3; a label after the second store reached by a goto ahead of the loop 5; `__assume(voice != NULL)` before the
// stores, after the first or after the second 3 each. Diagnostics (wrong code, removed): two or four extra stores, or two fewer,
// leave `inc esi` right after `inc edi`; a real branch after the second store (`if (g_4e6374 == 7) voice->unknown10 = 0.0f;`) 79
// puts the inner counter's increment right after that join, ahead of the third store: retail's place. So retail's body has a
// block boundary after the first two stores that emits no code (the scheduler hoists `inc esi` only to the top of its block);
// none of the empty constructs above survive to scheduling.
// Round 113 (m3 lane 16, fast): a byte copy of the type made at the type loop's top for the store 3; the type field
// declared char 3; the per-type loop in a forceinline helper taking the voice index by pointer 3; the index advanced in
// its own statement with `++i` 3; a while loop counting before the voice lookup 3. Diagnostics (removed): an always-true
// `if (voice->sound_index == NONE)` around the later stores 3 (folded before scheduling, so a folded test leaves no block
// boundary); the inner counter renamed to sort first or last 3/3 (names do not move it); the later stores through
// `&g_4e6378[voice_index - 1]` 98 (589 bytes: the stores may alias the pool pointer, so it is reloaded). Also: the voice
// through a forceinline getter taking the index (no stores in it) 3.
// Round 101 (m3 lane 10, fast): the stores from unknown0a on under a three-case `switch (type)` with an unreachable default 3
// (folds before scheduling); the four bytes at +8 each declared `byte x : 8` 3 (same code); those four as bit-fields of one
// dword 6 (same instructions, but the three bit-field byte stores rise above both increments); the type and reset bytes as
// bit-fields of one word 3, with the type store now ahead of `inc esi` as in retail and only the sound-index store left behind
// it, but that declaration loses matched 0x128500, 0x128700 and 0x128920 (their type compares swap operands), so not kept; a
// while loop with the counter declared at the type loop, `i++` after the second store and a closing `continue` 59 (561
// bytes), and the same as a guarded do-while 59. Diagnostics (removed): the type store through a volatile cast 3 (no
// barrier); `voice->chunk_index += 1;` after the second store 41 (its `inc word ptr` is lifted above both first stores and
// `inc esi` follows it); a second sound-index store after the last one 11 (the optimizer drops the first); empty-bodied
// `if (type >= 3) { }` or `if (voice == NULL) { }` after the second store 3. So: the scheduler orders stores to distinct
// offsets of one base freely; stores into one bit-field unit form a chain, and a store with a later dependant is placed
// before `inc esi`; `inc esi` itself rises to just below the previous flag-writing instruction (`inc edi`, or a
// read-modify-write put after it). Retail therefore has both first stores carrying a later dependant (or a flag writer
// between the second and third store) at scheduling time; no plain-field form gives the sound-index store one. Later in the
// round: the two index dwords and the type/reset bytes reached as two-element arrays (diagnostic) 3, same code, so constant
// array elements do not chain; the two zero bytes cleared with a two-byte memset 87 (577 bytes: one word store, the pool
// pointer hoisted).
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
void sound_playback_set_chunk(long sound_index, long definition_index, short pitch_range_index, short permutation_index, short chunk_index)
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
	s_decibels current;
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
// MiniMax B (11-12): no new form, no lower score (94); a fade-in local with the scaled local reaches 87, but each holds a
// value used once, so the body stays as it is; the division written out of the branch scores 160 and the product inlined 94.
// MiniMax G (11-11): no new form, no lower score (94): the arm-1 clamp's divisor re-spelled through the class 99 and a
// fade-in local for the test and the inverse 101; retail's division-first clamp and its fade-out-last product stay out of
// reach. This round: the reciprocal from a fade-in local copy of the field 87 (684 bytes: a gain of seven and a one-use
// copy, so the body stays), the product with the division written inline 145, a fade-out local for the product 94 and
// the sum's addends swapped 87 (both the same code). The permuter (907 s, 201 tries) found no better form at all.
// MiniMax B (11-11): no new form, no lower score (94); the only lower form listed needs a one-use copy of a field, which
// this lane may not keep, so the body is unchanged.
// score 94 (was 94): unchanged this round: retail keeps the elapsed time in xmm3 and the reciprocal in xmm2 where
// ours keeps them in xmm2 and xmm1, and retail divides the clamped time by the fade-in time where our build
// multiplies by the reciprocal; splitting the product into a named factor and naming the fade-out time (94 and
// 98) both leave the arm's registers as they are.
// score 94 (was 94): 177 before lane 10 round 69. Frame 0xc and the entry agree. Left: the first arm's sum (retail loads the gain into a
// register, adds the interpolation into it and keeps the tails apart), its register rotation (inverse, elapsed,
// 1.0, fade-out), and the gain value carried into the last arm.
// MiniMax E (11-10): no new form, no lower score (94).
// MiniMax C (11-09): no new form, no lower score (94).
// MiniMax H (11-07): no new form, no lower score (94); the fade block still holds the running
// difference in another register (ours xmm2, retail xmm3) and still divides by the fade-in time in
// the other form (ours multiplies by its reciprocal).
// MiniMax R (11-06): no new form, no lower score (94).
// MiniMax D (11-06, run 49): no new form, no lower score (94); the first arm's sum still differs (retail loads the gain into a register and adds the interpolation into it), the register rotation (inverse, elapsed, 1.0, fade-out) still differs and the gain is still carried differently into the last arm.
// Earlier at 94 (was 177): Frame 0xc and the entry agree. Left: the first arm's sum (retail loads the gain into a
// register, adds the interpolation into it and keeps the tails apart), its register rotation (inverse, elapsed,
// 1.0, fade-out), and the gain value carried into the last arm.
// Earlier 94 (was 177): the same three differences (two stacked score lines merged in r229).
// Round 160: the gain and ducking gains as s_decibels, the interpolation by value and the hold sum through the
// by-value operator 127 (682 bytes); on it the raw index, `gain +=` in the first arm and divisions with a fade_out
// local 208 (640 bytes), `t = 1.0f; if (1.0f > time) t = time;` in arms 2 and 4 177 (672 bytes), the fade table read
// at each field without the fade_out local 125 (kept). On 125: inverse local with the compare `time * inverse` 163
// (648), plus an if for t 160 (648), `time * inverse` as the value 153 (669), with a fade_out local 171/169 (644);
// a one-caller by-value add for the first arm 98 (686 bytes, not kept: it copies EDI to a new slot), with the inverse
// 153/150 (688); `interp + gain` in the first arm 158 (680), in arm 3 157 (678), in arm 4 178; `gain +=` in arms 3
// and 4 125; arm returns 187 (658); a returned s_decibels 153 (668, EDI lost); the interpolation built on the operator
// 106 (678, aligned 64: a's load splits, frame 0x10); named sum operands 125 each.
// Review (m3): the clamps back as `1.0f > time ? time : 1.0f` and the first arm as `gain = gain + ...` 125 (678,
// same code; the if-form clamps and `+=` were score-picked spellings, not needed).
// Earlier at 137: Frame 0xc agrees; table/index entry, elapsed/interpolation homes and the copied hold gain remain.
// Earlier at 137 (was 137): unchanged this round: the entry still loads the class table into EDI where
// retail uses EDX, the first branch still multiplies the fade-out time before the inverse (so the
// elapsed difference stays in XMM2 where retail keeps it in XMM3 and every use of it differs), and
// the hold branch still copies the ducking gain through a stack temporary our code folds away.
// Frame 0xc and gain home agree; table/index entry order, elapsed/interpolation scheduling and hold-gain copy remain.
// Earlier at 137 (was 137): Frame 0xc and gain home agree; table/index entry order, elapsed/interpolation scheduling and hold-gain copy remain.
// Earlier at 137 (was 137): frame 0xc agrees; early table load, interpolation homes/scheduling and copied hold gain still differ.
// Earlier at 137 (was 137): the gain is a decibel struct (retail copies decibels as a whole, as 0x218d70 does): it
// this round: the entry still loads the class table into EDI where retail loads it into
// EDX, and the fade product is still multiplied by the fade-out time before the inverse, so the elapsed
// difference keeps XMM2 where retail keeps XMM3 and every use of it differs; a named local for the scaled
// product, a second local for the scaled-out product, a local for the elapsed-minus-hold difference and a
// local for the whole product all compile to the same code.
// Earlier at 137: the gain is a decibel struct (retail copies decibels as a whole, as 0x218d70 does): it
// Earlier at 137 (was 137): the gain is a decibel struct (retail copies decibels as a whole, as 0x218d70 does): it
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
// N round 257: Native real/aggregate ducking gains and endpoints, real/aggregate returns, expression/accumulator 322/322/322/322/198/198/198/198.
// Original value types, helper and complete body restored.
// Round 117 (m3 lane 8, fast): /Op around the function (diagnostic) 191 (643 bytes: the value's `time / fade_in`
// stays a divss as retail, but the product still takes fade_out before the inverse); the test and the value both as
// divisions 207 (636); `t = time / fade_in; if (!(1.0f > time * inverse)) t = 1.0f;` 138 (the value is still
// `inverse * time`); no index local 179 (670); the hold sum through decibels_add 163 (686); the hold branch's
// ducking gain through a long local read as a real 127 (682 bytes: that branch is then retail's exactly, retail
// reinterprets a by-value copy of the long gain, but a one-use local is kept only on a match).
// J221: Elapsed/reciprocal/both/fraction role names 137 each; fade-in/full timing/ambience snapshots 144/179/182; parallel interpolation outputs 137; native table base plus original short lookup input 146 at 672 bytes (entry instructions agree). All original executable code restored.
// J225: shared ducking-array receiver 164; external ducking-record references 137; array and references
// 164; native fade-record value 174; fade value and array 181; fade value and references 174; fade value,
// array and references 181; native ducking-record values 284. Original body restored.
// F320: on the aggregate gain graph, shared time/fraction/both producer workspaces 137/137/137;
// each with native table/raw class entry 146/146/146 (672 bytes, entry agrees). Both workspaces with
// negated hold-first arm 157; table entry too 160. Frame 0xc and elapsed/endpoint homes persist;
// original executable body restored.
// N313: Integer-contract interpolation with both real inputs captured, native table/raw-class entry,
// expression/staged result 146/146; shared hold addition 116/116; complete reused attack durations
// 155/155, with shared hold addition 116/116. The 116 forms write the aggregate through its own
// address and are restored without a match. Original helper, table receiver and full body restored;
// frame 0xc agrees; elapsed/interpolation and copied hold-gain homes remain.
// N314: Sequential composite time as base accumulator/release accumulator/complete time accumulator/
// staged release-time producer, each with original/table-first entry: 137/146, 137/146, 137/145, 137/145.
// All helpers, arithmetic associations, gain representation and complete original body restored.
// F338: Aggregate gain with completed same-ambience, hold-rejection and attack-duration
// terminal boundaries, seven subsets then all with disabled rejection: 183/195/187/195/187/189/189/188.
// All restored; table/index entry and interpolation/hold-copy homes remain.
// F340: Encoded/real scalar views for result/ducking field/both interpolation
// endpoints, seven subsets then the complete aggregate view: 137 each.
// NOT kept: unmatched scalar wrappers. Original types, helper and full body restored.
// M337: Release-end predicate sum, twice-used remaining attack time and distinct-release
// hold quotient at their producers, seven subsets [137, 193, 137, 193, 137, 193, 193], all with table-first
// entry 183. All restored; table/index entry, interpolation and hold-copy homes remain.
// Round 69 (m3 lane 10; baseline 177 at 642 bytes, the ternary clamps' tails cross-jump): `t = 1.0f; if (1.0f > time) t = time;` in arms 2 and 4
// 125 (678, aligned 55, masked 20; kept). First arm on that: an inverse local with `(elapsed - hold) * inverse * fade_out` and `t = 1.0f; if (1.0f >
// time * inverse) t = time / fade_in;` 94 (683, aligned 52, masked 17; kept); as a ternary 163 (648); `t = time * inverse` 127 (672); a fraction local 179
// (640); plus a fade_out local 98 (masked 15). No inverse: a fade_out local 128 (678, aligned 76, masked 14: retail's order, reciprocal first, with XMM
// colours cycled: elapsed 2 for 3, inverse 1 for 2, fade_out 4 for 1, 1.0 3 for 4); on it `gain = X + gain` 153 (682, masked 10, frame 0x10 from the by-value
// copy), `gain.value` sum spellings and a sum local 128, staged time, a named product or a named first term 128; an if-form clamp in arm 1 145 (681).
// AC529: Composite-fade clamp partial/completed predicates in bool/byte/short/long: 168/168/170/169/168/168/170/169.
// NOT kept: predicate residence grows the body without equality; native reciprocal/elapsed and gain-load schedules remain.
// B429: Complete native compound/scalar compound/scalar assignment/constructor
// addition boundaries 105/105/105/105 at 678 bytes; each with disabled early
// return 188 at 652. NOT kept: sums/tails regress; original body restored.
// DL264: Existing aggregate gain addition with receiver-first expression,
// result/copy storage, value/reference right input and immutable returns: 185/185/105/105/158/154/185/105.
// NOT kept: no eligible native repair with preserved shared users; original executable source restored.
// M385: Existing aggregate interpolation endpoint/result compound accumulation, native-copy,
// constructed difference/product, scalar product, twice-used start and uninitialized result:
// 94/94/102/94/94/94/94/94. NOT kept: no native reciprocal, gain-load or tail repair; full producer restored.
// M387: Composite fraction eager division/select/positive and complementary complete results,
// original then eager simple upper caps: 160/160/163/159/140/140/143/101. NOT kept: no gain;
// original body restored, native reciprocal/elapsed roles and first gain-load schedule remain.
// B449: Entry gain real construction/default-member/default-assignment/explicit-copy acquisition,
// each with encoded/real zero endpoints: 232/232/94/94/228/228/187/187.
// NOT kept: no exact construction repair; original entry gain, endpoint domain and complete body restored.
// AC554: Current native-entry reciprocal graph; reciprocal identities attack_scale/duration_inverse/
// fade_inverse/phase_inverse and elapsed identities ambience_elapsed/elapsed_time/hold_elapsed/
// transition_elapsed 94/94/94/94/94/94/94/94. NOT kept: no exact native allocation repair; original body restored.
// DL267: Explicit copy constructor member/body/object-byte/member-byte publication, then reference/value-return assignment with scalar/overlap-safe byte publication: 280/280/179/179/94/190/94/190.
// NOT kept: no byte match or eligible native repair; original executable source restored.
// DL272: Standard minimum at simple/current attack caps and standard release interval, seven subsets then the alternate complete interval nesting; reciprocal/elapsed and first gain-load gaps remain. Scores 144/103/160/235/211/213/189/189.
// DH382: Independent terminal completion for composite/simple/current attack, held gain,
// same-ambience group, absent-current gain, rejected attack duration and disabled admission:
// 183/184/176/184/184/184/165/175. NOT kept: every form regresses and shortens away from retail;
// original complete body restored; reciprocal/elapsed, first gain load and late attack join remain.
// Lane 15 round 62 (fast): retail's first arm keeps a true `divss` for the clamp value while its test multiplies by
// the reciprocal; ours turns `time / fade_in` into `inverse * time`. Divisions in place of the inverse local (the
// compiler then makes the reciprocal itself, tests by multiplying and keeps the value's divss as retail): with the
// if-form clamp 145 (681), as a ternary 125 (678 bytes, aligned 58), `t = time / fade_in; if (!(1.0f > t)) t = 1.0f;`
// 153 (669); the product still takes fade_out before the reciprocal in all three. The inverse local declared before
// the fade-out test 154 (665: the reciprocal is then hoisted above that test), with a ternary clamp 177, with the
// clamp's test as a division 184.
// T69: Twice-used attack reciprocal before elapsed/hold, inside hold/attack,
// mutable then immutable per phase: 183/183/183/183/186/186/154/154.
// NOT kept: all regress and shorten away from retail; original reciprocal phase and full body restored.
// MiniMax H (11-07): no new form, no lower score (94); the fade block still holds the running
// difference in another register (ours xmm2, retail xmm3) and still divides by the fade-in time in
// the other form (ours multiplies by its reciprocal).
// MiniMax R (11-06): no new form, no lower score (94).
// MiniMax D (11-06, run 49): no new form, no lower score (94); the first arm's sum still differs (retail loads the gain into a register and adds the interpolation into it), the register rotation (inverse, elapsed, 1.0, fade-out) still differs and the gain is still carried differently into the last arm.
// Round 158 (m3 lane 5, fast): the first arm without the inverse local, divisions only, `(elapsed - hold) / fade_in
// * fade_out` with the clamp `1.0f > time / fade_in ? time / fade_in : 1.0f` 125 (678, aligned 64: the reciprocal is
// made by the compiler and the clamp value keeps its divss as retail, but fade_out is multiplied from memory before
// the reciprocal); `fade_out * ((elapsed - hold) / fade_in)` 124, `(elapsed - hold) * fade_out / fade_in` 125,
// `fade_out * (elapsed - hold) / fade_in` 125, each with the clamp as `time / fade_in < 1.0f ? ... : 1.0f`
// 125/124/125/125; the if-form clamp 145 (681); the time sum product first 125. On the kept body the inverse product
// as `fade_out * (elapsed - hold) * inverse`, `fade_out * inverse * (elapsed - hold)`, `(elapsed - hold) * fade_out
// * inverse`, `inverse * fade_out * (elapsed - hold)` 94 each (identical); `gain +=` in the first arm 94 (identical);
// a `real fade_out` local for the test, sum and product 98, with `gain +=` 98; on the division form `gain +=` 125, the
// fade_out local 128/128 (678, aligned 68).
// P392: Complete composite/simple overlap admission, positive/complementary bool/byte/short/long:
// 94/94/94/94/94/94/94/94. NOT kept: no native elapsed/reciprocal/gain-load equality; one-use outcomes
// require a match. Original comparisons, floating groups and whole body restored.
// Round 139 (m3 lane 7, fast with 0x1251e0): the first arm's sum written interpolation first (`decibels_interpolate(...) + gain`, for
// retail's gain load into a register) 156 (687 bytes, frame 0x10 from the by-value copy); that on the division form 150 (682), with a
// fade_out local 153 (682). On the division form (no inverse local, ternary clamp, 678 bytes): a fade_out local declared inside the arm
// for the product only 125; a fade_out local with the test `hold + fade_out` 128 (same code as `fade_out + hold`); with the time sum
// product first 128 (same code). An inverse local with a fade_out local and the clamp as `1.0f > time * inverse ? time / fade_in :
// 1.0f` 171 (644 bytes), as `t = time / fade_in; if (!(1.0f > time * inverse)) t = 1.0f;` 169 (644). Without the pragma pair around
// the call to 0x221810 221 (717 bytes: that call then inlines). Left as before: the reciprocal is made after the product's first factor, and
// elapsed keeps XMM2 where retail has XMM3.
// Round 103 (m3 lane 9, fast): `elapsed` written out at each use 199 (715 bytes); `gain +=` in arms 3 and 4 94 (same code); the
// ducking pointer declared before `elapsed` 94 (same code); the division form with a `fade_out` local read right after the ducking
// pointer 144 (678: the load moves above the hold test and still takes the last XMM register); the hold arm as `gain +=
// ducking->gain` 105 (678: drops retail's by-value copy through a stack temporary). Our five extra bytes are all at the
// arm-3/arm-4 join: ours keeps the arm-3 sum in XMM0 and loads the gain on the bypass edge (jmp, movss, register add), retail
// re-reads the gain from its home in arm 4's add.
// Lane 10 r91 (fast; masked = aligned lines with registers and stack offsets masked, control 94 [17]): the listed `fade_out`
// local for the test, sum and product re-measured 98 [15]: the product then takes the reciprocal first as retail, but
// `fade_out + hold` loads hold first (retail copies fade_out and adds hold from memory). On it, retail's clamp shape (the value
// before the branch, `ja` over `t = 1.0f`): `t = time / fade_in; if (!(1.0f > time * inverse)) t = 1.0f;` 169 (644 B: the branch
// is retail's but the value is still `inverse * time`, and arms 1 and 2 then end alike and cross-jump), `if (time * inverse >=
// 1.0f)` 171, a ternary with the test multiplied and the value divided 171; with `gain +=` in arm 1 169 (same merge); with the
// arm-1 sum written interpolation first (retail's gain load into a register) 99 (684 B, frame changes at +0x2).
// R229 (m3 lane 3, fast with 0x1251e0): one to eight unused-local probes at the top (diagnostic, removed) 94 each (same
// code: the XMM colours do not follow symbol numbering here); `gain +=` in arm 4 alone, in arm 3 alone 94 each, and in
// both (a re-measure of round 160's form, 125 then, on the current body) 94 (same code: the last arm still takes the gain
// from XMM0 across the join where retail re-reads [esp+8]); one shared interpolation after the fade-out test's two arms
// (t chosen in each) 218 (606 bytes; retail keeps two copies); one ducking pointer for arms 3 and 4 94 (same code); the
// hold sum as `gain += ducking->gain` 105 (678 bytes: the ducking gain's stack copy goes); arm 3's clamp as
// `t = time; if (0.0f > time) t = 0.0f; else if (time > 1.0f) t = 1.0f;` 119 (688).
// M3 lane 12 r114 (fast with the file's 76 functions and 0x221810, control 94 [masked aligned 46, register-masked 12]): our five
// extra bytes are the arm-3/arm-4 join exactly: arm 3's sum stays in XMM0 for arm 4 (a `jmp` over a bypass-edge `movss xmm0,
// [esp+8]`, then `addss xmm3, xmm0`), where retail re-reads the gain from its home in arm 4's add (+8 - 3 bytes). Each of these
// compiles to the base's code (94): the interpolation helper's sum written start-first; arm 3's sum as `s_decibels(gain.value +
// ....value)`, as `gain.value = gain.value + ....value`; arm 4's sum as the constructor form; arm 1's as `gain.value += ....value`;
// arm 3's clamp through the existing PIN_126660; arm 4's index in a named local.
// Then forms that change arm 1's code (fast; [masked, register-masked]): re-measured lane 1 r92's retail-length division form
// (reciprocal inline in the product, `real fade_out` for the test/sum/product, division ternary) 128 (678 B) [56, 11], its product as
// `(elapsed - hold) * fade_out / fade_in` 128 (same code); on it the clamp as `t = time / fade_in; if (!(1.0f > t)) t = 1.0f;` 156
// (669 B: one division for test and value) [52, 7], the product's reciprocal from a fade_in local 150 (682 B) [53, 10]. On the kept
// body a fade_in local read by the outer test and the inverse (the value still divides by the field) 101 [53, 11], with arm 2's
// division through it as well 96 (682 B) [49, 12], with a fade_out local for the test, sum and product instead 100 [53, 10].
// m3 lane 1 r92 (fast with 0x1295e0/0x129100, control 94, register-masked aligned 17): retail keeps fade_out in XMM1 from
// its test to the product and makes the reciprocal before `elapsed - hold`. New: the reciprocal written inline in the
// product, `(elapsed - hold) * (1.0f / fade_in) * fade_out`, with the clamps as divisions 145 (681 bytes: fade_out is then
// multiplied from memory before the reciprocal); that with a `real fade_out` local for the test, sum and product 149 (681,
// masked 16: the product is then retail's order, reciprocal then fade_out from the register); that with the clamp as the
// division ternary `1.0f > time / fade_in ? time / fade_in : 1.0f` 128 (678, masked 14: the reciprocal is made first as
// retail; left are the XMM colours (elapsed 2 for 3, 1.0 3 for 4, reciprocal 1 for 2), the time sum taking the product as
// its destination, the value's divss after the test product and `fade_out + hold` loading hold first); on that a `real
// hold` local for its three uses 156, a `real fade_in` local for all its uses 191 (635). Body restored.
// Round 185 (m3 lane 4, fast with 0x1251e0/0x221810): retail's arm-1 clamp value keeps a true divss because its
// divisor is not the inverse's (a compile-only screen shows the reciprocal reuse keys on the divisor's spelling): the
// inverse from a fade-in local (`real fade_in = ducking->fade_in_time; real inverse = 1.0f / fade_in;`) with the value
// still `time / ducking->fade_in_time` 87 (684 bytes, aligned 46, masked 12: retail's divss; a one-use field copy beside
// field reads, match-only); on it the clamp as `t = time / ducking->fade_in_time; if (!(1.0f > time * inverse)) t = 1.0f;`
// 144 (681, masked 9: retail's division-then-test order), as a ternary 144; plus a fade_out local for the tests and the
// product 92 (684), with the division-first clamp 148 (681, masked 8: retail's inverse-then-fade-out product), with the
// ternary 148, with `gain +=` 148 (same code), with `interp + gain` 161; a fade_out local declared inside the arm 144 (same
// code as without it); the fade-in local also for the value 155 (687: the value is a reciprocal product again); one
// fade-in local for the test, the inverse, the value and arm 2 95. `return *(long *)&gain.value` 94 and
// `gain.value += ....value` in arms 3 and 4 94 (same code each). Left on the 87 form: fade_out multiplied from memory
// before the inverse (retail keeps the test's fade_out load in XMM1), the arm-1 sum adding the gain from memory where
// retail loads it into a register, the arm-3/arm-4 join and the XMM colours.
// Round 235 (m3 lane 3, fast): returning `s_decibels` by value (`return gain;`, the matched caller 0x1251e0 taking it as
// `s_decibels`) 153 (678 bytes: retail's EDI copy of the gain's bits goes, so retail returns a dword; 0x1251e0 then 6); arms 3
// and 4 written interpolation-first 124 (687); arm 1's time sum product-first 94 (same code); re-measured the fade-in-local form
// 87 (match-only), on it the product-first sum 87 (same code) and the clamp as if/else assignments 144 (681).
// m3 lane 8 r158 (fast with 0x1295e0): arm 1's product as `fade_out * ((elapsed - hold) * inverse)` 94 (other code: the
// inverse product is then formed first as retail, but fade_out is still multiplied from memory and the reciprocal is made
// after `elapsed - hold`), with a `real fade_out` local for the test and the product 98 (masked 11), with the fade-in local of
// the 87 form 87 (match-only; the 87 form re-measured 87); SOUND_SYSTEM read at each use (no sound_system local) 94 (same
// code). Diagnostic (a volatile local, reverted at once): one or two dead volatile loads ahead of `elapsed` 171/180 (696/700 B):
// elapsed stays in XMM2, so one more scratch step there does not give retail's XMM colouring.
// Round 178 (m3 lane 5, fast with 0x1251e0/0x1295e0/0x221810, control 94, 683 bytes; al = aligned differing lines, 52).
// Reading of retail: arms 2, 3 and 4 add the gain from its home into the interpolation (`addss xmm0, [gain]`, the
// by-value operand of operator+ plus `*this` read through its reference), arm 1 loads the gain into a register and adds
// the interpolation into it, and after arm 3 the long view is reloaded (`mov edi, [gain]`) while arm 4 reads the float from
// memory again. The local-count scan that matched 0x1e6360 (n unused locals, diagnostic) does nothing here: the division
// form with a fade_out local 128 for n = 0..7 (same code), the division form 125 for n = 0, 1, 2 (same code). New forms:
// every interpolation arm as `gain +=` 94 (same code); arm 1 interpolation first with the gain operand built from its
// float value (`... + s_decibels(gain.value)`) 176 (703 bytes), on the division form 165 (698), with the fade_out local
// 168; arm 1 as `s_decibels(gain.value + ....value)` 94 (same code); arm 1's interpolation in a named local added as
// `fade + gain` 156 (687, the code of the listed interpolation-first form), on the division form with the fade_out local
// 153 (682). Re-measured: interpolation first in arm 1 156 (687; the gain's by-value copy is stored from EDI to a new
// slot and added from memory), the division form 125 (678, al 55), both 150 (682), the division form with a fade_out
// local 128 (678).
// score 94 (was 94): unchanged this round: our link-time inlining expands 0x221810 (src/unknown_221490.cpp) where
// retail calls it out of line; that callee's inline eligibility is fixed in another file.
// @retail 0x127010
long function_127010(short class_index)
{
	s_decibels gain = g_502118[class_index].current;

	if ((byte)~g_502118[class_index].flags & 1)
	{
/* retail calls the class lookup out of line here */
#pragma inline_depth(0)
		s_sound_promotion_view *sound_class = (s_sound_promotion_view *)function_221810(class_index);
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
						real t = 1.0f;
						if (1.0f > time * inverse)
							t = time / ducking->fade_in_time;

						gain = gain + decibels_interpolate(s_decibels(0ul), ducking->gain, t);
					}
					else
					{
						real time = (sound_system->ambience_fade - elapsed) / ducking->fade_in_time;
						real t = 1.0f;
						if (1.0f > time)
							t = time;

						gain = gain + decibels_interpolate(s_decibels(0ul), ducking->gain, t);
					}
				}
			}
			else
			{
				gain = gain + ducking->gain;
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

					gain = gain + decibels_interpolate(ducking->gain, s_decibels(0ul), t);
				}
			}
			if (sound_system->ambience_index != NONE)
			{
				s_sound_class_ducking *ducking = &sound_class->duckings[sound_system->ambience_index];

				if (ducking->fade_in_time >= 0.001f)
				{
					real time = sound_system->ambience_fade / ducking->fade_in_time;
					real t = 1.0f;
					if (1.0f > time)
						t = time;

					gain = gain + decibels_interpolate(s_decibels(0ul), ducking->gain, t);
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

	s_decibels sum = *(s_decibels *)&interpolated_decibels + (*(s_decibels *)&class_gain + *(s_decibels *)&gain);

	return *(long *)&sum;
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
   playing sound's deletion reads it (0x1c bytes): deleting a synchronized
   sound counts it down and clears the controller's playback flag */
struct s_sound_controller_view
{
	byte unknown00[3];
	byte synch_count : 7;
	byte synch_playback : 1;
	long unknown04;
	byte unknown08[0x14];
};

void looping_sound_controller_release(long index);

/* deletes a playing sound, letting go of its looping sound's controller */
#pragma inline_depth(0)
// @retail 0x127390
void sound_playback_delete(long sound_index)
{
	s_sound_playback *sound = SOUND_PLAYBACK_GET(sound_index);
	if (sound->effect_index != NONE)
	{
		s_sound_controller_view *controller = (s_sound_controller_view *)((s_record_pool *)g_51ebd8)->data + (sound->effect_index & 0xffff);
		if (sound->unknown03 == NONE && controller->unknown04 != NONE)
		{
			controller->synch_count--;
			controller->synch_playback = false;
		}
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
// @retail 0x1269f0
void sound_system_update_time(void)
{
	dword now = GetTickCount();
	long elapsed = now - SOUND_SYSTEM->field_14_2;

	SOUND_SYSTEM->field_14_2 = now;
	SOUND_SYSTEM->elapsed_time = (real)elapsed / 1000.0f;
	real seconds = (real)elapsed * 0.001f;
	SOUND_SYSTEM->time += elapsed;
	SOUND_SYSTEM->ambience_fade += seconds;
	SOUND_SYSTEM->previous_ambience_fade += seconds;
	if (g_4e6948 && g_4e6948->flag1120 && g_4e6948->state == 1)
	{
		if (g_4e6948->flag1121)
		{
			if (SOUND_SYSTEM->master_fade_delay == NONE)
				SOUND_SYSTEM->master_fade_delay = (long)(SOUND_SYSTEM->master_fade_times.i * 1000.0f);
			if (SOUND_SYSTEM->master_fade_delay > 0)
			{
				SOUND_SYSTEM->master_fade_delay = SOUND_SYSTEM->master_fade_delay - elapsed > 0 ? SOUND_SYSTEM->master_fade_delay - elapsed : 0;
			}
			if (SOUND_SYSTEM->master_fade_delay == 0)
			{
				real step = SOUND_SYSTEM->elapsed_time / (0.001f > SOUND_SYSTEM->master_fade_times.j ? 0.001f : SOUND_SYSTEM->master_fade_times.j);

				approach_126660(&SOUND_SYSTEM->master_fade, 0.0f - SOUND_SYSTEM->master_fade, step);
			}
		}
		else if (!function_12be90())
		{
			real step = SOUND_SYSTEM->elapsed_time / (0.001f > SOUND_SYSTEM->master_fade_times.k ? 0.001f : SOUND_SYSTEM->master_fade_times.k);

			approach_126660(&SOUND_SYSTEM->master_fade, 1.0f - SOUND_SYSTEM->master_fade, step);
			SOUND_SYSTEM->master_fade_delay = NONE;
		}
	}
	else
	{
		SOUND_SYSTEM->master_fade = 1.0f;
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

/* moves an angle toward a target, by at most maximum */
static inline void approach_angle(real *angle, real target, real maximum)
{
	real delta = target - *angle;

	approach_126660(angle, delta, maximum);
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
				real step = SOUND_SYSTEM->elapsed_time /
					(request->transition_time > 0.001f ? request->transition_time : 0.001f);
				s_sound_environment *environment = &SOUND_SYSTEM->environments[slot];

				if (0.001f >= environment->fade)
				{
					environment->unknown0c = request->unknown0c;
					environment->unknown10 = request->unknown10;
				}
				else
				{
					real maximum = step * k_pi;

					approach_angle(&environment->unknown0c, request->unknown0c, maximum);
					approach_angle(&environment->unknown10, request->unknown10, maximum);
				}
				approach_126660(&environment->unknown18, request->unknown18 - environment->unknown18, step);
				approach_126660(&environment->unknown14, request->unknown14 - environment->unknown14, step);
				approach_126660(&environment->fade, 1.0f - environment->fade, step);
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

			approach_126660(&environment->fade, 0.0f - environment->fade, step);
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
						s_decibels inner(*(real *)&distance_decibels + *(real *)&gain);
						s_decibels gain_base(*(real *)&function_xaa8231(definition->class_index)->gain_base);
						s_decibels loudness = gain_base + inner;
						bool forced = SOUND_FLAG(location->flags, 11) || SOUND_FLAG(location->flags, 8);

						if (forced || loudness > s_decibels(-64.0f))
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
// MiniMax H (11-14): no new form, no lower score (17).
// MiniMax H (11-12): no new form, no lower score (17); the first difference is still 0x219070's result, and the one-use short local that lowers it to 15 stays a match-only form.
// MiniMax B (11-11): no new form, no lower score (17); the delayed branch's two stores still swap (retail keeps the
// start time and then the flag byte before the push, ours hoists the flag byte above the add and sinks the start time
// past the push), and the set index is narrowed to a short at the 0x219070 call because 0x218f50 returns short in
// another file; 0x219070's own set index is long here and in its definition.
// MiniMax B (11-11): no new form, no lower score (17); still BLOCKED-BY 0x219070's parameter list and 0x1286b0's variant
// indices, which live in other lanes' files, so the set index's width stays as it is.
// MiniMax C (11-11): the 0x219070 call is a statement of its own now, so the chunk index is pushed after it as
// in retail (15, was 17); left: the range index is still sign extended (`movsx ebp, ax`) where retail copies the
// whole eax (0x218f50 returns short), and the delayed start still sets flag0 before storing the start time.
// MiniMax E (11-11): no new form, no lower score (17); the range index is still sign extended to a short for the
// 0x219070 call and the delayed start still stores the flag byte before the start time; both widths are fixed by
// the callee declarations in other files, so the difference stays here.
// MiniMax G (11-11): no new form, no lower score (17): the range index is sign-extended (movsx ebp, ax) for 0x219070's long set index where retail moves the whole eax, and retail calls 0x219070 with the definition in edi and one stack argument where we push both; that callee's parameter order and its short return are declared in src/unknown_219070.cpp and the variant indices belong to 0x1286b0 in src/unknown_12a1b0.cpp, so this stays BLOCKED-BY 0x219070 and 0x1286b0.
// MiniMax R (11-09): no new form, no lower score; the set index is still narrowed to a short for the
// 0x219070 call and the delayed start still stores the flag byte before the start time.
// MiniMax D (11-08): score 17 (was 17): the set index is sign-extended for the 0x219070 call and that call
// passes its second argument on the stack where retail keeps it in a register (both follow the callee's own
// declaration, which lives in another file), and the delayed start stores the flag byte before the start time.
// MiniMax H (11-07): no new form, no lower score (17); the sound set index is still narrowed to a
// short here (`movsx ebp, ax`) where retail keeps the full value (`mov ebp, eax`), because the
// callee 0x219070 only reads the low half of that parameter, and the tail still stores the flag
// byte and the start time in the other order.
// MiniMax R (11-06): no lower score (17); the movsx at the 0x219070 call comes from 0x218f50 and 0x218f10 returning short (both byte-exact, src/unknown_218ac0.cpp) where retail's caller copies the dword, and declaring them long here scores 80 and leaves the call unresolved.
// MiniMax E (11-05): no new form, no lower score; merging the two set-chunk calls into one site with the
// range and permutation indices in named locals gives 193 (short indices) and 192 (long indices) - our frame
// drops to 8 against retail's 0xc. Reverted.
// MiniMax E (11-10): no new form, no lower score (17); the note below already lists the named short and the
// flags pointer, and both stay as they are.
// score 15 (was 17): left: the range index is sign-extended (`movsx ebp, ax`, one byte) for
// 0x219070's long set index and the delayed start sets flag0 before storing the start time.
// Earlier: the 0x219070 call ran after the chunk index push, and the
// two set-chunk calls narrow their indices to char
// 0x219070's long set index, the 0x219070 call runs after the chunk index push, and the
// delayed start sets flag0 before storing the start time. Earlier: the two set-chunk calls narrow their indices to char
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
// Round 121: 0x1286b0's two index parameters made short (retail's callers 0x128ca0 and
// 0x21c400 sign-extend char fields to 16 bits for them), every call updated: 23 -> 17 (kept).
// Then (plain-equivalent `--fast` with the set-chunk callers): the 0x219070 result in a named
// short 15 (BLOCKED-BY 0x219070: only its movsx and the flag0 order are left); that with a short
// range-index local 53; a short range-index local alone 53; the flags pointer (diagnostic) 13;
// a short chunk index 17; literal zero chunk index and fade times 17, as one chained store 19.
// Round 73 (plain): flag0 set before the start-time store 18; `delay + SOUND_SYSTEM->time` 17;
// `sound->flags |= 1` for flag0 17, before the start-time store 18; `delay + time` with flag0 first 18.
// P346: Existing word/byte delayed-start writers with selected-permutation and delay results:
// reduced 198/204/198/204/198/204/198/204 against control 197 (868 bytes; full 17/865).
// NOT kept: reduced scores and byte-writer length regress; all one-use results, include and body restored.
// MiniMax C (11-11), NOT kept (coordinator 11-11): 17 -> 15 only from 0x219070's result in a one-use short local passed to sound_playback_set_chunk (match-only form); body at 17.
// score 17 (was 17): unchanged this round: the sound set index is sign-extended where retail copies the dword
// (one byte longer) and 0x219070 is called with two arguments where retail passes one; both live in other files.
// score 17 (was 17): unchanged this round: our code is one byte longer - the sound set index is sign-extended
// (`movsx ebp, ax`) where retail copies the dword, and we call 0x219070 with two arguments where retail passes one.
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
				sound_playback_set_chunk(result, tag_index, state->variant0, state->variant1, (short)chunk_index);
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
				sound_playback_set_chunk(result, tag_index, (short)arg_58ecd0, function_219070(arg_58ecd0, definition), (short)chunk_index);
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
	/* the listener's scale, axes and position form a transform4x3f, as at the 0x142700 call below */
	transform4x3f const *matrix = (transform4x3f const *)&listener->velocity_scale;
	velocity.i = listener->velocity.i;
	velocity.j = listener->velocity.j;
	velocity.k = listener->velocity.k;
	if (matrix->scale != 1.0f)
	{
		velocity.i = matrix->scale * velocity.i;
		velocity.j = matrix->scale * velocity.j;
		velocity.k = matrix->scale * velocity.k;
	}
	function_2477b1(&matrix->rotation, &velocity, &listener_velocity);
	listener_speed = 0.0f - dot3f(&listener_velocity, &direction);
	source_speed = dot3f(&position->velocity, &direction) + 111.548553f;
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
		if (bit_vector_test(system->free_bits, i) && !bit_vector_test(system->streaming_bits, i))
		{
			result = (short)i;
			break;
		}
	}
	return result;
}

/* assigns a free channel and, when needed, a shared driver voice */
// MiniMax B (11-12): no new form, no lower score (96); a bool result gives 113 at retail's length, the success path
// returning early 133 and the failure path returning false with two other exits 135, so the shared exit with the result
// byte in its stack slot stays closest.
// MiniMax G (11-11): no new form, no lower score (96): retail's three returns (true at the entry, false in the failure
// block, true at the commit) measure 131, and the index still lands in eax where retail keeps it in ecx. This round:
// the failure block reached by a goto after the channel search, so that it follows the buffer tests as retail's
// does, measures 144 (449 bytes); the failure block after the shared exit 147, the failure label moved out of the
// work block 139, the early return as an explicit else 131, the same test negated as an early return 131 and a bool
// result 113. The permuter (608 s, 100 tries) ends at 89, and only by declaring the sound pointer volatile and
// swapping two declarations, so the body stays.
// MiniMax B (11-11): no new form, no lower score (96); the goto-free forms repeat earlier rounds' 131 and 139, so the
// result still lives in a stack byte where retail splits it between al and bl.
// score 96 (was 96): unchanged this round: the result still lives in a stack byte where retail keeps it in al
// and clears it through bl on the failure path, and our frame is twelve bytes against retail's eight; the lower
// forms listed need a moved declaration or a goto into an arm, which this lane may not keep.
// score 96 (was 96): unchanged this round: retail keeps the result in AL (set at the entry, cleared
// through BL on the failure path and set again after the commit) with an eight-byte frame, while ours
// keeps it in a stack byte with a twelve-byte frame; retail's failure block follows the success path
// and its channel search keeps the channel in EDI with one `or ebp, -1` shared between the channel
// compare and the driver and buffer defaults. A `bool` result carrier on this body measures 113 at
// retail's length and was reverted.
// MiniMax R (11-09): no new form, no lower score; the result still lives in a stack byte where retail keeps
// it in al, and the frame is still twelve bytes against retail's eight.
// MiniMax D (11-08): score 96 (was 96): retail returns from two places - the failure block clears the result
// byte and returns, the commit block returns true - while ours shares one return through a stack byte, and the
// argument roles differ (retail keeps the index in ecx, ours in eax).
// MiniMax H (11-07): no new form, no lower score (96); retail narrows the first parameter to a word
// and scales it by nine where ours narrows a later parameter and scales that one, the result still
// lives in a stack byte in ours and in al in retail, and the frame is 0xc against retail's eight
// bytes.
// MiniMax R (11-06): no new form, no lower score (96).
// MiniMax D (11-06, run 49): no new form, no lower score (96); retail still keeps the result in al (set at the entry, cleared through bl on the failure path and set again after the commit) with an eight-byte frame, ours keeps it in a stack byte with a twelve-byte frame, and retail's failure block still follows the success path.
// MiniMax E (11-10): no new form, no lower score (96); the body with two exits and no result variable, the
// same body with a bool result and the failure path returning false directly were measured (131, 113, 134)
// and the byte result with the shared exit stays the best.
// Earlier at 96 (was 96; a second score line, merged in round 185): unchanged this round: retail keeps the result in AL (set at the entry, cleared
// through BL on the failure path and set again after the commit) with an eight-byte frame, while ours
// keeps it in a stack byte with a twelve-byte frame; retail's failure block follows the success path
// and its channel search keeps the channel in EDI with one `or ebp, -1` shared between the channel
// compare and the driver and buffer defaults. A `bool` result carrier on this body measures 113 at
// retail's length and was reverted.
// The twelve-byte frame, byte result normalization, initial voice/global load order and
// Earlier at 96 (was 96): The twelve-byte frame, byte result normalization, initial voice/global load order and
// early failure tail remain; long search-result boundaries do not change the selected code.
// Earlier at 96 (was 96): retail loads the voice index into ecx before the frame setup, keeps the `NONE` constant in
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
// Round 253: Native byte API return with byte/bool/char/long result and common/direct success exits;
// giving 113/113/113/109/139/139/139/133.
// Original executable source restored; no qualifying gain.
// Round 34 (m3 lane 2, fast): retail shares one `or ebp, -1` between the found channel's `cmp di, bp` and
// the driver/buffer defaults and keeps the channel in EDI (the loop index). Bool result with `return true`
// after the commit and `result = false` before the failure calls 139 (415 bytes); with the driver default
// before the channel test 133 (415); plus the search returning `(short)i` from its loop 122 (432); that
// with the byte result 133 (430); byte result, success goto, driver default before the test and the
// returning search 131 (433); bool, `return true`, returning search, driver default in the block 139 (415).
// Round 117 (m3 lane 8, fast): bool result with `result = true` before the success goto 122 (430 bytes; the head's
// registers rotate and the failure block follows the search loop); on it the driver default before the channel test
// 153 (409 bytes: retail's block order, but the search index and the channel stop sharing EDI and the driver goes to
// the stack), both defaults there 128 (431), that with `~status & DSBSTATUS_PLAYING` 142 (430), the driver default
// before the search 147 (406), result declared first 146 (409); `else result = true` for the early path 135 (433); the
// search returning from inside its loop on these 122/136 (416)/135/133; the 122 form with `~status` 121 (430).
// J222: Long search workspace/return, long workspace with short return, promoted short result and native long
// direct search exits all 96; long workspace with Boolean body result and long/short helper returns 113/113;
// explicit committed success 122 (430 bytes); existing byte success declared at entry on the long search graph
// 96. Original executable body and private helper restored; every calibrated match preserved.
// F320: shared search mask/word ordinal/both 125/149/158; each with chained driver/buffer
// defaults 120/149/158; both with cached upper bound 158; all 158. The frame remains twelve
// versus eight, with a saved result byte and early failure tail. Original body/helper restored.
// N313: Channel-search receiver read at every field use, chained defaults/byte status complement
// subsets 0..3: 128/123/103/98; with explicit Boolean commit result 117/124/116/122.
// Original receiver, independent defaults, byte result and full body restored. Frame twelve versus
// eight, entry voice/global scheduling and early failure tail remain.
// DH292: Available-bit intersection / occupied-bit union in the existing finder, original break
// with byte/committed Boolean results 144/131/147/130; condition-controlled search
// 159/131/161/133. Original two bitmap tests, helper, result and entire body restored.
// DH294: Existing shared bitmap test/streaming-set/free-clear operations, seven subsets,
// 96 each; complete shared-operation graph with explicit committed Boolean success 122.
// Original private finder, raw bitmap operations, byte result, includes and full body restored.
// N314: Condition-controlled availability scan with assigned/direct exhaustion results, independent/chained
// defaults and native byte/committed Boolean results: 134/138/64/128/134/138/64/128. The 64 form (plain,
// 435/430: frame eight and the later failure tail recovered; entry registers, search exit checks, sentinel
// sharing and status test remain) needs a second `result = true;` before the commit's goto, where result
// is already true: a redundant store, not kept without a match (coordinator review 11-02). Without it the
// same scan scores 134. On that graph, long private search input/complemented status/both 107/130/60.
// Original helper, byte result and full body restored.
// N319: Split bitmap rejection through continues/nested guards, free/streaming first, byte/Boolean result: 96/96/99/99/113/113/113/113. Original helper and full body restored; frame twelve/eight, result normalization and early failure tail remain.
// N325: Complete branch-owned driver/buffer defaults, byte/Boolean results, then
// paired chained defaults: 91/126/120/133/153/149/137/130. Driver-only defaults 91 at 432/430:
// NOT kept: five-point gain below eight while above 30. Original defaults, byte result,
// helper and full body restored; frame, shared sentinel and search/failure joins remain.
// N329: Native pooled voice/playback/channel-system references, seven receiver subsets 96 each; complete
// references with complemented playing status 127. Original pointers, saved byte result, search and full
// body restored; frame twelve/eight and failure-tail placement remain.
// Round 552: direct unchanged/commit/failure Boolean returns without a saved carrier, native
// outer/early unchanged guards crossed with independent/chained defaults and original/complemented
// status 131/135/131/135/131/135/131/135. Original byte result, helper and complete body restored.
// NOT kept: no score gain or byte match.
// Round 160: bool result with `return true` after the commit on a while scan: assigned/direct exhaustion 130/130, chained
// defaults 130/130, complemented status 127/127; `result = true;` before the goto on the while scan 88 (430 bytes,
// aligned 46; a redundant store, not kept: left are the head registers, `or ebp, -1` shared at the channel test, the
// complemented status and the failure block's place), with complemented status 118, chained defaults too 118; the
// returning for-search or while-search on it 122/122, with complemented status 121, chained defaults 122/121; the
// failure calls written in two arms, inverted or not 168/145/168/145 (455/438 bytes), byte result 151, complemented
// status 178/155.
// N367: Tentative channel result before bitmap validation, short/int/long/dword in for scan 96/166/166/166;
// while scan 96/166/166/166. NOT kept: no gain or equality; original private finder and full body restored.
// Frame twelve/eight, normalized byte and early failure tail remain.
// B440: Native full-width first/last/both/entry-const range captures on the
// original inclusive scan: byte result 96/96/96/96, Boolean result 113/113/113/113.
// NOT kept: ties or regressions; frame twelve/eight and saved-byte/failure tail remain.
// Original private finder, result byte, independent defaults and full body restored.
// N379: Existing finder produces admission plus the narrowed channel; selected-local, direct-output,
// found-return and exhaustion-result forms with byte/bool body results: 96/96/130/134/113/113/129/104.
// NOT kept: ties or regressions; private finder signature, saved byte result and complete body restored.
// N381: Existing finder receives native voice/member pointer or reference, byte/Boolean body result:
// 125/125/128/128/105/105/128/128. NOT kept: no byte match; original private input,
// signed scan, byte result, status/error calls and complete body restored. Frame/result/failure joins remain.
// N383: Actual channel/buffer predicates own the return decision, byte/bool, success/failure
// polarity and constant/actual entry admission: 160/175/166/208/134/141/166/208.
// NOT kept: all regress; original result, private finder, observations, commits and body restored.
// N385: Independent finder completion state, found/searching polarity in bool/byte/long/dword:
// 125/125/125/125/125/125/125/125. NOT kept: no equality; completion flags and whole finder/body restored.
// Frame twelve/eight, saved byte normalization and early failure tail remain.
// P351: Low-byte requested-buffer bits in byte/word/long/dword with byte/Boolean result:
// 96/101/101/101/113/109/109/109. NOT kept: no byte match; frame twelve/eight, saved-byte normalization and failure joins remain.
// Original complete body and existing helpers restored.
// F399: Explicit candidate admission/rejection/staged-admission/independent bitmap
// predicates with byte final result 96/96/177/96, Boolean result 113/113/156/113.
// NOT kept: no gain or equality; original private scan, byte result and whole body
// restored. Twelve/eight-byte frame, saved true and early failure tail remain.
// F401: Actual free/streaming table bases separately/both, then native bitmap-word lvalues,
// with byte/Boolean final result: 96/142/142/96/113/132/132/113. NOT kept: ties or regressions; original finder
// and full body restored. Twelve/eight-byte frame, saved normalization and failure tail remain.
// N389: Complete byte/short/long/dword failure results at original/entry lifetimes:
// 130/130/104/104/136/136/136/136. NOT kept: no native result/frame recovery;
// original byte-success result, helper and full body restored.
// N391: Existing finder with actual system pointer/reference and char/signed-char/short/long
// selector 128/128/128/128/99/99/128/128. NOT kept: inline contracts remain unmatched;
// original finder, result, defaults and full body restored. Frame and failure tail remain.
// N397: Complete byte-result, channel, driver, buffer and status native home identities 96/96/96/96/96/96/96/96.
// NOT kept: no byte equality or eligible native-shape gain; original complete body restored.
// Lane 15 round 62 (fast): `~status & DSBSTATUS_PLAYING` on the byte result 127 (439 bytes), with a bool result 130
// (430 bytes, aligned 126); status declared at the top 96 or in the channel block 96 (same code); the driver and
// buffer defaults declared before the search 171 (468 bytes).
// MiniMax D (11-08), 96: retail returns from two places - the failure block clears the result
// byte and returns, the commit block returns true - while ours shares one return through a stack byte, and the
// argument roles differ (retail keeps the index in ecx, ours in eax).
// MiniMax H (11-07): no new form, no lower score (96); retail narrows the first parameter to a word
// and scales it by nine where ours narrows a later parameter and scales that one, the result still
// lives in a stack byte in ours and in al in retail, and the frame is 0xc against retail's eight
// bytes.
// MiniMax R (11-06): no new form, no lower score (96).
// MiniMax D (11-06, run 49): no new form, no lower score (96); retail still keeps the result in al (set at the entry, cleared through bl on the failure path and set again after the commit) with an eight-byte frame, ours keeps it in a stack byte with a twelve-byte frame, and retail's failure block still follows the success path.
// Round 158 (m3 lane 5, fast): a `bool result;` with no initializer assigned in every arm (the early path's
// `else result = true;`, `result = true;` before the commit's goto, `result = false;` in the failure block) 135
// (433 bytes: frame eight, the index load before `sub esp, 8` and the failure's `xor bl, bl ... mov al, bl` as retail,
// but the early path gets its own `mov al, 1` exit copy and the failure block still follows the search loop); on it
// the defaults before the channel test 138 (431), `~status & DSBSTATUS_PLAYING` 138 (435), both 141, a byte result
// 148 (441), the commit as `return true;` 136 (430), with the defaults first 136 (431). The private search as a
// condition-controlled while scan returning `i <= last ? (short)i : NONE` (or through a NONE result), and as a for
// loop with a plain break and the bound tested after it (both compile alike): byte result 134 (452, aligned 47),
// the arm-assigned bool 142 (435), `bool result = true` with the goto 104 (440), with `return true;` after the
// commit 109 (436, aligned 45: the failure block follows the buffer tests as retail, but the result sits in a stack
// byte and the bound is tested again after the loop), plus the defaults before the channel test 130/134.
// "Assume failure" (`result = false;` before the channel work, `result = true;` before the commit's goto, nothing in
// the failure block): after the search 132 (430, stack byte), before the search 129, with the defaults first 134
// (436), as a byte 137 (439), with the while scan 135/135 (441).
// Round 139 (m3 lane 7, fast): the channel search, buffer choice and commit as an inline step returning a bool, the
// caller keeping `bool result = true` and running the failure calls on a false step: with plain `inline` the step
// stays out of line, 464/464/465/464/464 (98 bytes) for the five forms below; forced inline: defaults before the
// channel test with `result = step` 128 (431 bytes), defaults inside the channel block 122 (430), defaults first with
// `if (!step) { result = false; ... }` 126 (435), defaults first with `(byte)~status & DSBSTATUS_PLAYING` 124 (433),
// defaults inside with that status test 121 (430). Frame eight and the entry index load before `sub esp, 8` appear in
// the forced forms, but the head's registers and the failure block's place still differ; none kept.
// Round 103 (m3 lane 9, fast): the search returning `(short)i` from its loop, bool result, driver default before the channel test
// 141 (438 bytes); with `~status & DSBSTATUS_PLAYING` and both defaults before the test 144 (440), as a failure-first if/else 144
// (440); the while scan, bool result, complemented status, defaults first, failure-first 148 (442), with the plain status 138 (435);
// the for/break search with that failure-first form 141 (437). N314's 64 form rebuilt for reading: its tail and failure block are
// retail's byte for byte; left are the entry registers, the scan's post-loop bound test (retail's found path goes straight to the
// `(short)i != NONE` test, as the for/break search does), the shared `or ebp, -1` and the complemented status.
// Lane 10 r91 (fast; masked = aligned lines with registers and stack offsets masked, control 96 [50]): the status as
// `(byte)~status & DSBSTATUS_PLAYING` (the idiom 0x127010's matched flags test uses) 127 [47]; with a bool result 130; with
// `return true` after the commit 129, both 131. N314's condition-controlled scan rebuilt: byte result 134, bool result 104 [30],
// with `return true` after the commit 109, bool with the redundant `result = true;` before the goto 64 [17] (diagnostic, the
// form review rejected; it reproduces N314). On the scan, the result assigned from the commit test (`result = buffer_index !=
// NONE || ...; if (result)`) 122 (bool) / 152 (byte), on the for scan 123/130; an uninitialized result assigned once on each
// path (early `else result = true;`, before the commit's goto, in the failure block) 142 (bool) / 152 (byte). On the 64 body
// left are the entry registers (index in ECX as retail only with the frame-8 forms), the type byte loaded then sign-extended
// (retail `movsx ecx, byte ptr [esi + 8]`), the scan's post-loop bound test, NONE shared in EBP and the complemented status.
// R229 (m3 lane 3, fast with 0x129100): `bool result = true;` with `result = false;` at the top of the channel block,
// `result = true;` after the commit and the failure calls under `if (!result)` 129 (430 bytes: retail's `mov al, 1`
// at the entry, but the false value lives in a stack byte and the failure block follows the search loop); the same
// with a byte result 151 (450); the false assigned in `else` arms of the buffer and channel tests, failure calls under
// `if (!result)`, 114 (430); bool result with `return true;` after the commit (re-measured 139, 415 bytes: the whole
// result in EBX, the sound in EDI) plus the driver default declared at the top of the channel block, before the
// search, 155 (407), at function scope 145 (442). Diagnostic re-measure: bool result on the kept goto body 113 (430:
// `mov al, 1` stored to a stack byte for the goto path, `xor bl, bl ... mov al, bl` failure as retail). The first form is
// a variant of round 94's (false after the search, 132) and the driver-before-search one of lane 16 r50's / round 117's
// on another base. Also: `result = false;` after the two failure calls, byte result 96 (same code), bool result 136 (430).
// M3 lane 12 r114 (fast with the file's 76 functions and 0x221810, control 96 [masked aligned 48, register-masked 33]; goto-free
// forms would be kept at 96 or lower since they drop main's goto): the commit block ending in `return result;` with the byte
// result and no label 130 (430 B, frame 8, but the result then lives in BL from the entry, `test bl, bl; setne al`) [91, 47]; the
// same with a bool result 139 (415 B, the listed `return true` numbers: likely the same code); on the goto body the driver
// default before the channel test with `(byte)~status & DSBSTATUS_PLAYING` 140 (436 B) [58, 26], with a bool result as well 139
// (430 B) [68, 31]; the `return result;` commit with the driver default before the channel test 133 (430 B) [92, 37]; the
// channel-system pointer read at the function's top for the commit 122 (430 B) [75, 34]; the search helper without its separate
// forced-inline declaration 195 (365 B: it stays out of line).
// m3 lane 1 r92 (fast with 0x129100, control 96, aligned 73, registers masked 55): retail's exits are three blocks (early
// path returning the entry AL, failure `xor bl, bl ... mov al, bl`, commit `mov al, 1`), so its commit path has its own true
// and the early and failure values are separate webs. New: the driver default before the channel test with `return true;`
// after the commit and `return result;` closing the failure block, bool result 144 (407 bytes: the early path folds to a
// constant and NONE is hoisted into EBP at the entry); the same with the byte result 144 (same code). Body restored.
// Round 185 (m3 lane 4, fast with 0x129100/0x127320/0x12a5d0/0x21de90): the channel work inside `do { ... } while
// (false);` with `break` for a missing channel and for a missing buffer, `return true;` after the commit and the
// failure calls after the loop, bool result 111 (430 bytes, register-masked aligned 38 -> 23: retail's block order, the
// failure block after the buffer tests with the entry true shared into its epilogue; left the entry registers, the sound
// in EDI for EBX, the channel in EBP and the driver in EBX, `cmp bp, -1` for retail's shared `or ebp, -1`, the status
// test); a steering wrapper, match-only. On it: a byte result 119 (433), byte result with the goto exit 144 (449), the
// driver default before the channel break 133 (415), with `~status & DSBSTATUS_PLAYING` 135 (430), the complemented
// status alone 130 (432, masked 21). Re-measured (diagnostic): bool result with `return true;` 139 (415).
// Round 235 (m3 lane 3, fast): bool result, `return true;` after the commit and the failure block ending `return result;` 131
// (431 bytes: the false folds to `xor al, al`); re-measured bool result with `return true;` after the commit 139 (415: retail's
// three exits, but `mov bl, 1` at the entry and the sound in EDI for EBX); that with `(byte)~status & DSBSTATUS_PLAYING` 131 (430),
// plus a byte result 129 (430), plus the search returning `(short)i` from its loop 131, plus the defaults ahead of the channel
// test 138 (435); `bool result = true;` declared first 139 (same code); a guard `return true;` for the early path with
// `bool result = false;`, `result = true;` after the commit and the failure under `if (!result)` 142 (440); the failure test
// written ahead of the commit (`if (channel == NONE || (buffer_index == NONE && (...))) { fail } else { commit; return true; }`,
// defaults before the channel test) 123 (433), with the complemented status 138, plus a byte result 142, without the commit's
// `return true` 141 / byte 155.
// m3 lane 8 r158 (fast with 0x127010, 0x127320, 0x12a5d0, 0x21de90, 0x129100): the commit's two bit updates through the
// existing bit_vector_set 96, the search helper's two bit tests through bit_vector_test 96, both 96 (same code each; both
// kept: existing helper calls in place of hand-written copies of their bodies); the requested-buffer local at every use (the
// buffer assignment and the final test, no field re-read) 139 (441 B), for the assignment only 138 (441 B).
// Round 178 (m3 lane 5, fast with 0x129100/0x127320/0x12a5d0/0x21de90/0x127010; al = aligned differing lines; control 96,
// al 73; about 60 forms). LEAD, 15 at 430 bytes (al 8; the form is saved as build/r178/held-1295e0-form15.txt): `bool result
// = true;` declared before the voice and the sound, the search helper taking `long type`, the missing channel as `if
// (channel == NONE) goto channel_failed;`, then `buffer_index = NONE; driver_index = NONE;` (buffer first), the status
// test as `(byte)~status & DSBSTATUS_PLAYING`, and a failure-first if/else whose failure arm holds the label and whose
// commit arm ends `result = true;`. Every instruction is retail's except three scheduling spots that have one cause: the
// two early exits join the commit block's epilogue (so `mov al, bl` sinks below three pops in the failure tail and the
// commit's pops stay together), where retail joins them to the failure block's epilogue. Not kept: it needs the second
// `result = true;` (a redundant store), the moved declaration and a goto into an arm, all match-only. What each part
// does (ablation on the goto-free twin of the lead, 74/79): result declared after the sound 116 (the entry index then
// takes EAX and NONE takes EBP); no initializer with `else result = true;` for the early path 138; no `result = true;`
// in the commit arm 161 (451 bytes, stack byte); plain status test 106; `char type` 79; driver default before the
// buffer default 74 (the compare and the buffer store then use immediates where retail shares `or ebp, -1`).
// Two families, by how the missing channel reaches the failure code. Implicit (`if (channel != NONE) { ... }` with the
// failure after it, or `channel == NONE ||` / `channel != NONE &&` in one if/else, goto or not, commit before or after
// the return): 79 (415 bytes, al 28), retail's code with the failure block and the return placed straight after the
// search loop instead of after the buffer tests. Explicit (the goto above, the failure arm first or second, the commit
// through `goto channel_done`, a label after the return, flat gotos with the exit label referenced first, or a one-pass
// `do { } while (false)` with breaks): 15 each, the same code. Other forms: the defaults ahead of the channel test (both
// families) 109-127 (al 20-44: the failure block is placed as retail's but the search result is not threaded, so the
// counter stays in EDX and is copied); chained defaults 109 / 74 / 79; the defaults ahead of the search 121-128; the
// failure written in both else arms 126, without the commit's `result = true;` 193; a `return true;` or `return result;`
// in the commit arm 125, `return result;` in the failure arm 128 (any second return merges the result into one register
// from the entry); the search returning from its loop gives the same code as the break form with `long type`, the
// condition-controlled scan 127-129 (442 bytes).
// On the lead with flat gotos, the early path as its own `return result;` or `return true;` 135 (431 bytes).
// score 96 (was 96): unchanged this round: the allocator keeps the result in BL where retail splits it (AL at the
// entry, BL in the failure block) and holds the sound in EBX.
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
				bit_vector_set(system->streaming_bits, channel, true);
				bit_vector_set(system->free_bits, voice->channel_index, false);
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

// score 24 (was 24): the loop is a for statement whose increment reloads the pool and advances the index, the
// switch has a `case 1: continue;` arm (so a non-zero mode skips the record work) and the pitch range is computed
// above the mode, which keeps the pool in ebp and the index in ebx as retail has them. Left: the range is built
// before the mode flag test where retail builds it inside case 0, so the flag byte load and the class lookup sit
// in a different order than retail's.
// Earlier (49, was 49): the start-time test reads the sound through SOUND_PLAYBACK_GET and the sound pointer is
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
// Round 117 (m3 lane 8, fast unless noted): round 40's harness 24 confirmed (fast 24, plain 24): the for loop with
// `case 1: continue;` and the range computed above the mode (before the first class lookup): every register, the
// `sub eax, 0` switch and the pool reload are retail's, only the range computation sits above the mode (retail:
// inside case 0).
// The range between the mode and the switch 99; in case 0 78 (EBX/EBP swapped from the entry), with a class pointer
// local 78, nested ifs 78, case 1 first 78; the sound pointer read once at the loop top 167; range, sound_index or
// voice renamed to sort first or last 78 each (names do not break the tie); case 0's work as an inline helper 251
// (474 bytes, called), forceinline 228 (604); the mode from an inline getter with two returns 113 (540, called),
// forceinline 214 (592 bytes: the class lookup inlines with it).
// J225: native class mode field 49; native class mode and flag fields 49; native fields with two-case for
// dispatch 78; dispatch with complete role names 78; native fields with range-record reference 49; dispatch
// with range reference 78; dispatch with shared class index 98; dispatch with range reference and class
// index 98. Original body restored.
// Lane 18 round 16 (fast; the one-link env inlines 0x221810, so not used): the next start index named before the
// loop-end `sounds = g_4e637c;` 49 (same code: the reload still precedes the ternary); the loop end through an
// inline (pool, index) next-datum helper with data_find_index 49; that helper in the for loop with `case 1:
// continue;` 78; a helper taking only the index and reading the pool global after the start index, in the for
// loop with `case 1: continue;` 78 and in the do-while 49.
// Round 130, with the range in case 0: definition reference 78; split range declaration 78; mode declared before
// definition 78; default-first mode 66 (entry registers right, `test al, dl` rather than `test dl, 1`); flag-first
// if/else 78; named predicate with default-first mode 66; const definition pointer 78; default-first self-select 66;
// default-first with a flag-arm assignment 78. Original 24 form restored.
// Round 38 (m3 lane 18, fast): with the range in case 0 (78, only the index and pool/voice/range swap EBX/EBP):
// range declared at function top 78; sound_index declared before the pool 78; the class flag read into a bool
// before the definition 78; a permuter run (45 tries) best 62 (a word voice index, which changes the NONE test).
// On the 24 form: the range index named before the mode with the pointer in case 0 102; the flag bool 24.
// Round 171 (lane 4, fast): with the range in case 0 (78): the class-flag result named before the range 103; the voice
// read through the array at each use 163 (572 bytes); a const range pointer 78; the callback table in a local 82
// (on the 24 form 28); 3-20 dead locals at the top, in case 0 or after the switch (diagnostic) 78 each.
// Lane 15 round 62 (fast): on the 24 form a const range pointer 24 (same code), a long mode 69; with the range in
// case 0 (78; register-blind only 5: a pure EBX/EBP mirror, the pool and range in EBX, the sound index in EBP): the
// range declared at the block top and assigned in case 0 78, reason declared before the lookup flag test 78, a const
// definition pointer 78; the pool global read directly at one of its two uses (entry datum, entry search, loop-end
// datum or loop-end search) 78 each.
// Round 158 (m3 lane 5, fast; range in case 0 unless noted): the loop-top start test and sound pointer read through
// the `sounds` local (`((s_sound_playback *)sounds->data)[sound_index & 0xffff]`) 29 (558 bytes: sound_index is in
// EBX and the whole body is retail's, only the pool takes ESI for EBP); on it the loop-end search through the global
// (`data_find_index(g_4e637c, ...)`, the datum through `sounds`) 6 (558 bytes, aligned 2: only the pool reload is
// scheduled before the next-index ternary; a mixed global/local read, not kept without a match), with the inverted
// ternary, pointer-arithmetic loop-top spellings or the for-form find helper 6 each; the start test or the pointer
// alone through the local 173 (572)/163 (with the 6-form increment 166/163); the pointer read once before the test
// 162 (561); a loop-top `sounds = g_4e637c;` 78; the datum through the global 29 (563); `sounds = g_4e637c` last in
// the increment 17 (560: the load follows the ternary but the pool splits ESI/EBP with a copy); the reload embedded in
// the find or datum argument 82-87 (24-form grid with entry/declaration orders); a named next index before the reload
// 82; a 45-form grid of increment, entry argument, declaration order and range placement best 6 (range above the mode
// 53/55, one global entry argument 11/31); the pool global at every use 78 (24 with the range above the mode);
// sound_index declared in the for init 78; a 600 s permuter run on the 6 form (63 tries) best 6.
// Round 84 (m3 lane 11, fast): retail has no pool reload at the loop top (the loop-end value in EBP feeds it),
// which fits the loop top read through the `sounds` local; with that and the range in case 0 (29 re-measured: every body
// register retail's but the pool in ESI, reloaded before the next-index ternary): sound_index scoped to the for
// statement 29 (same code); the mode as if/else 29 (same code). Re-measured listed forms on it: the reload embedded in
// the find's argument 82 (index and pool swap EBX/EBP), the sound pointer taken once before the start test 162 (561
// bytes), 0x128020's `(s_sound_playback *)sounds->data + index` spelling at the loop top 29 (same code). On the 6 form
// (the 29 form with the loop-end find through the global; re-measured 6, only the pool load sits between `cmp ebx, -1`
// and its jne where retail loads it after the ternary's join): the range declared at the arm's top and assigned in
// case 0 6 (same code); `reason` declared at function scope 6 (same code); the increment as the find through the
// global into sound_index, then `sounds = g_4e637c`, then the datum (measured only: the name reused for the
// absolute index) 78; re-measured, the reload embedded in the datum argument 82. Every form that puts the
// loop-carried pool load after the ternary swaps index and pool between EBX and EBP. Original 24 body restored.
// Round 235 (m3 lane 3, fast): re-measured the range in case 0 78 (a pure EBX/EBP swap of the index against the pool, voice and
// range); with the loop top through `sounds` 29 (the loop end then rotates pool/result/count/mask ESI/EDI/EBX/EBP for retail's
// EBP/ESI/EDI/EBX); the all-global increment with `sounds = g_4e637c` last 17 (the loop end is retail's exactly, the pool in EBP
// after the ternary; only `sounds` stays a separate web in ESI with a bottom `mov esi, ebp`); the existing for-form find helper on
// the 78 form 78 and `reason` at function scope (on the 29 form) 29, both listed forms on this base. New: the for-form find helper on
// the 29 form 29; `long sound_index;` declared ahead of the pool local (29 form) 29; a `dword` sound index (29 form) 29; on the
// range-in-case-0 base with the all-global increment, a loop-top `sounds = g_4e637c;` read by the start test and the sound
// pointer 78, a body-scoped pool local for them 78; on the 29 form, the find through the global into a function-scope `next_index`,
// then `sounds = g_4e637c`, then the datum through `sounds` 78 (the swap again).
// Round 237 (m3 lane 3, fast): the range in case 0 re-measured 78 (558 bytes: every instruction retail's but the index
// in EBP and the pool, voice and range pointers in EBX). On it: case 0's three range statements (class-flag test, find,
// start) as a forceinline function taking the range address as its argument 229 (605 bytes), that function plain
// inline 197 (522 bytes, called); the range bound as a reference first 78 (same code); the range held as the callee's
// own pointer type (one cast where it is formed, none at the calls) 78 (same code); the range named inside the
// class-flag test's arm, after the 0x221810 call 31 (559 bytes: index EBX and range EBP as retail, but the address is
// formed after the call). So the swap appears exactly when the range is live across the class-flag call inside case 0;
// above the mode (three calls) or after the flag call (one call) the index keeps EBX.
// M3 lane 16 r116 (fast): with the range in case 0 (78, re-measured): a `dword` sound index 78; the voice as a reference
// 78; the acquisition test inverted (failure arm first) 108; `!(start > time)` 78; the class-flag test through a
// TEST_FIELD_BIT of the shifted word 78, through a new word bitfield bit 5 in the existing start-flags view 78 (on the 24
// form 24, same code), through a short `mode` field at +0xe in the promotion view 78, `(short)1` in the mode ternary 78.
// The class-flag byte read as a byte (`*((byte *)... + 0xa) >> 5`) 72, or as a byte bitfield 72: every register is
// retail's (index EBX, range EBP), but the test folds to `test byte ptr [eax + 0xa], 0x20`; the word read keeps
// retail's `shr al, 5; test al, 1` and swaps EBX/EBP. The test as a plain-inline helper 73 (called, registers right),
// forceinline 228 (604 bytes). So the swap follows the flag test's word temporary when the range is live across it.
// Also: the class-flag byte read as a signed short 79, as a char 72 (folds); that flag in a named bool after the
// range 78; the voice's previous owner in a named local for its three uses 78; flag4 as a shift of the playback flags
// word 78; the source-updated test as a shift of that word 78.
// @retail 0x1282d0
void function_1282d0(void)
{
	s_record_pool *sounds = g_4e637c;
	long sound_index = data_datum_index(sounds, function_16bc00(sounds, 0));
	for (; sound_index != NONE;
	     sounds = g_4e637c, sound_index = data_datum_index(sounds, data_find_index(sounds, sound_index == NONE ? 0 : (sound_index & 0xffff) + 1)))
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
				s_sound_pitch_range *range = &SOUND_GLOBALS_DEFINITIONS->pitch_ranges[definition->pitch_range_base + sound->pitch_range_index];
				short mode = TEST_FIELD_BIT(sound->flag4) ? 1 : *(short *)((byte *)function_221810(definition->promotion_index) + 0xe);
				switch (mode)
				{
				case 0:
					{
						if (!(bool)((*(word *)((byte *)function_221810(definition->promotion_index) + 0xa) >> 5) & 1) &&
							(short)function_2193d0((s_animation_state *)range, (s_animation_ref *)definition) == NONE)
							function_219310((s_animation_state *)range, (s_animation_ref *)definition, sound->permutation_index);
						function_127320(sound_index, 0xe);
					}
					break;
				case 1:
					continue;
				}
			}
		}
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

// MiniMax H (11-12): no new form, no lower score (101); retail still expands 0x16f3a0 where ours keeps the call, which is a caller-side decision in a file I may not edit.
// MiniMax H (11-11): no new form, no lower score (101); the body is unchanged, because the lookup of the
// object type is still expanded where retail calls it out of line and the depth pair that would keep it
// out of line is ruled out; 0x221810 lives in src/unknown_221490.cpp, which this lane may not edit.
// MiniMax C (11-11): no new form, no lower score (101); the class lookup is still expanded by the compiler where retail calls 0x221810 out of line, and the depth pair that would keep it out of line is ruled out.
// MiniMax E (11-11): no new form, no lower score (101); the lookup of the object type is still expanded by the
// compiler where retail calls it out of line, and that expansion shifts the rest of the listing; the depth pair
// that would keep it out of line is ruled out, so the difference stays here.
// score 101 (was 101): unchanged this round: the class lookup 0x221810 is still expanded where retail calls it
// out of line, which shifts the rest of the listing; 0x221810 lives in src/unknown_221490.cpp, which this lane
// may not edit, and the depth pair that would keep it out of line is ruled out.
// score 101 (was 101): BLOCKED-BY 0x221810: LTCG inlines the class lookup function_221810 that retail calls out of line, which
// shifts the rest of the listing; with the call kept out of line (a #pragma inline_depth(0) pair around
// it inside the body, which DECOMPILING.md rules out) the score is 4: the no-distance path's flag set
// folds to a store (retail ors the byte), and the fade gain sum loads the distance gain first where
// retail loads the selected fade gain first. Not tried: /Ob1 for the file, which changes every function
// in it.
// Tried: one gain local stored after the if/else 35; decibels_sum with a by-value operand 128 (6 at site 2
// alone); the sum through decibels_sum instead of the local 6; 1000 - fade - time inside PIN 6.
// Depth zero 417, depth one 310, fade scope 310, shared attenuation 334, callback table 311.
// MiniMax H (11-11): no new form, no lower score (101); the body is unchanged, because the lookup of the
// object type is still expanded where retail calls it out of line and the depth pair that would keep it
// out of line is ruled out; 0x221810 lives in src/unknown_221490.cpp, which this lane may not edit.
// MiniMax E (11-11): no new form, no lower score (101); the lookup of the object type is still expanded by the
// compiler where retail calls it out of line, and that expansion shifts the rest of the listing; the depth pair
// that would keep it out of line is ruled out, so the difference stays here.
// Round 63 (m3 lane 2, fast): the lookup read as `((s_sound_promotion_view *)function_221810(...))->gain` (0x14) 101
// (same code); case 1's rotation rows through dot3f on a copied forward 365 (the lookup still expands, so the
// caller's inline count is not what decides it); fade gain sum written selected-first 101 (same code); the
// class-gain block ahead of the field_0/c/10 stores 107; the definition and class-table lookups in one expression 101. Ours
// expands the lookup in large callers (here, 0x128020, 0x128ca0, 0x127010, 0x189fe0) and calls it from small
// ones (0x124fc0, 0x125010); retail never expands it, which points at its file's build (avoid list, flags).
// Round 187 (m3 lane 4, fast with 0x221810): retail's call sits under one top-level flag test, as ours does, so call-site
// nesting does not explain the out-of-line call; keeping it out of line needs a noinline, pragma or flags change (all
// ruled out). Case 0's flag set after the gain store 232 (1196 bytes); case 0's gain as a ternary 206 (1177 bytes); case 0
// written first in the switch 101 (same code); the fade test's operands swapped 102; the elapsed-time test with its arms
// swapped 127 (1196 bytes).
// Round 235 (m3 lane 3, fast): the selected-fade sum as `*(real *)&local_24 = *(real const *)local_21 + *(real *)&local_24;` 101
// (same code: the operand order is rank-based), through decibels_sum 103, as an `s_decibels` sum into a new local 101; case 0's
// flag as `local_0->unknown0a = local_0->unknown0a | 1;` 101, through `g_4e6378[arg_0].unknown0a |= 1;` 100 (retail's
// `or byte ptr`, first difference +0x3c7; a mixed pointer/array read, match-only); the voice through `g_4e6378[arg_0]` at every
// use 117. The matched sibling 0x129790 keeps its six 0x221810 calls out of line without pragmas.
// score 101 (was 101): unchanged this round: our link-time inlining expands 0x221810 (src/unknown_221490.cpp) where
// retail calls it out of line; the first residual difference is the constant-forwarding fold `unknown0a |= 1`.
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

// score 222 (was 222): Frame 0x49c versus 0x494, initial gain argument homes and integer clamp-store joins remain.
// MiniMax D (11-01): no new form, no lower score; the frame keeps one 4-byte slot more than
// retail's and the gain argument's homes and the joined clamp stores still differ.
// Earlier at 222 (was 215): Frame 0x49c versus 0x494; initial gain argument homes and joined integer clamp stores remain.
// Frame 0x49c versus 0x494; initial gain argument homes and joined integer clamp stores remain.
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
// Round 253: Unsigned incoming gain bits with original/reference inner and outer sums and shared scale;
// scores 215/255/215/255/286/255/286/255.
// Original executable source restored; no qualifying gain.
// Round 34 (m3 lane 2, fast; the baseline is now 222): retail also passes each permutation-gain sum to the pin
// through memory (real -> long -> parameter copy -> real) and stores the pinned value once at the join from EAX,
// where ours stores in each arm. Outer decibels_sum over the by-value inner 222 (same code); location seeded as
// a real and `+=` the inner real sum 306 (1030 bytes); location seeded as a long and `+=` a named by-value inner
// 222 (same); the real sum written out 306; each pin argument as decibels_add(local_6, converted gain bits) 241
// (1073), operands the other way 291 (1116); this file's decibels_pin single-exit (as src/unknown_2197f0.cpp's)
// 237 (1036; 0x12a810 still matches), with the decibels_add pin arguments 231 (1052).
// J224: Native byte views for pending/completed/both/submitted/pending-submitted/
// completed-submitted/all state units 238/231/242/234/245/239/254; all with native short
// pitch bounds 254. Completed-byte/all-byte units with reference inner gain and native
// bounds 278/293; with both reference sums 278/293 (frame 0x494, first +0x44).
// Original record declaration, body, gain helpers and every call restored; the two
// extra argument homes and clamp joins remain. One incomplete-image comparison excluded.
// Round 73 (fast; positional score (length) / LCS-aligned count, base aligned 147): the controller count
// defaulted to 0xff and overwritten (retail's single store at the join) 234 (1045) / 144, a NONE char
// default 275 (1040) / 162; that join with this file's decibels_pin single-exit 251 (1027) / 154; join,
// single-exit pin and `decibels_pin(decibels_add(local_6, converted permutation gain bits))` at both pin
// sites 228 (1043) / 142, the decibels_add operands the other way 258 (1086) / 148; pin and arguments
// without the join 231 (1052) / 145; round 154's opening (shared scale local, pitch-bounds pointer, gain
// seeded from arg_1 and updated in place) 245 (1052; exact to +0xba) / 110, with join, pin and arguments
// 255 (1041) / 105 (42 aligned lines closer: the lead); that opening without the scale local 269 / 118,
// with join, pin and arguments 281 / 115; gain/pitch names on both 245 / 255 (the two slots stay put).
// Left in the lead: the unfolded location add at +0xba (as 0x1251e0 and 0x126c30) and retail's memory
// round trip between each pin argument's sum and the pin parameter.
// N314: Packet field owns both unpinned gain producers before the existing clamp; original/inner reference/
// shared scale/both 222/269/277/249, with shared branch gain workspace 222/269/277/249. Inner reference
// alone recovers frame 0x494 and first +0x44 but regresses; the packet stores fold away. Original body,
// gain homes, scale observations, helpers, countdown arms and protected call inputs restored.
// Round 308: actual playback gain-bit member with original/inner/outer/both in-place sums
// Round 308: 222/269/222/269; with native signed location-bit member 222/269/263/306.
// Round 308: Original physical types, both headers, sum helpers and body restored; gain homes and clamp joins remain.
// N329: Existing file-local gain sum with first-reference/second-value inputs at
// inner/outer/both/both-plus-scale: 262/221/262/248; first-value/second-reference 263/222/263/249.
// NOT kept: one-point gain below threshold; shared users regress. All signatures and body restored.
// F338: Incoming gain parameter owns the raw sum until the processing call, nested/sequential
// additions crossed with shared scale and named pitch span: 224/224/276/276/224/224/276/276.
// All restored; aligned frame 0x49c/0x494, initial gain homes and integer clamp joins remain.
// M337: Existing whole decibel values through raw/processed/both submission gain
// stages, seven subsets and complete entry lifetimes: [222, 222, 222, 222, 222, 222, 222, 222].
// NOT kept: unmatched scalar-record/copy forms; all restored, gain homes and clamp joins remain.
// m3 lane 16 r83 (fast): retail's three sums all read like one add helper taking a real first operand and a
// long-bits second (only the second is copied): playback gain + copied argument, location + that sum, the
// converted permutation byte + copied local_6. Such a helper (real a, long b) for both opening sums 269
// (frame 0x494, inner sum exact; the location add still folds), its parameters swapped 269 (same code), at
// the two pin sums too 247 (1060 bytes; pin operands as retail, but our real -> long -> pin copy collapses),
// with this file's pin single-exit 261 (0x12a810 still matches), default-first 275 (loses 0x12a810, 25);
// on 0x1251e0's tail the same helper 23 (was 15). Chunk index read into a local before the next-chunk test
// 222 (the load moves above the permutation test as retail, zero-extended into ECX); the stream bit through
// a stream pointer 221 (retail splits that store into load/or/store). All restored.
// N367: Complete initial-pitch/packet-pitch/parallel branch-gain adjacent producers, seven subsets
// 222/254/254/222/222/254/254; all with sequential promotion adjustment 254. NOT kept: ties or regressions;
// original complete arithmetic, packet stores and helpers restored. Frame 0x49c/0x494, gain-input homes and
// pin joins remain.
// B440: Existing gain operators at raw nested/argument/inner/update producers
// 262/262/262/269; parallel branches only 316, with raw nested/inner/update 332/332/321.
// MiniMax H (11-06): score 167 (was 222): the caller's gain and the location's unknown08 are added
// to the playback's gain through the decibels type (s_decibels' operator+, as 0x21e330's reverb sums do),
// and the scale is read once into a real local. The frame is retail's 0x494 and the scale stays in xmm2.
// Not kept: reading the pitch bounds as a pair through &pitch_lower (local_18[0]/[1]) gave 164, three
// points, under the bar. Still differs: the pitch result and the argument hold swapped stack homes
// ([0xc] against [0x10]) and the two clamp joins in the tail differ.
// Raw/complete operators with native reused scale 167/252; adjacent pitch bounds too
// 164/249. Four pitch/gain/raw-result role-name forms on that graph each 164. Native
// 0x494 frame and early location load recover, but pitch/gain homes and pin joins differ.
// NOT kept: one-use value record and bit copy, combined ownership forms without equality.
// Original complete body, primitive helpers, observations, clears and interfaces restored.
// N379: Incoming parameter owns raw and processed gain through both submissions; nested, sequential,
// native-real and mixed sums 223/223/312/269; with coherent twice-used scale 273/273/299/246.
// NOT kept: all regress; original parameter lifetime, gain homes, packet, helpers and body restored.
// N381: Existing incoming gain-value contract with scalar/member/operator/parameter-owned raw sums,
// without/with coherent scale: 222/306/275/275/277/295/287/287. NOT kept: no byte match;
// original incoming type, caller, gain homes, helpers and entire body restored. Frame and pin joins remain.
// N383: Both parallel branch gains through the existing compound operation, scalar/nested/compound/
// staged raw producers 241/245/247/245; coherent twice-used scale 302/188/242/188.
// NOT kept: unmatched record/copy operations; 188 grows to 1075/1057 bytes. Original body restored.
// N385: Existing pin argument-owned/early-lower mutation, unsigned value argument/completed result,
// signed readonly-reference early/completed and unsigned readonly-reference early/completed:
// 237/231/237/237/222/237/237/237. NOT kept: no byte match; original pin and full body restored.
// Frame 0x49c/0x494, initial gain homes and integer pin joins remain.
// P351: Pin lower-only/both bound comparison truths in bool/byte/short/long:
// 222/222/222/222/222/222/222/222. NOT kept: no byte match; frame 0x49c/0x494, gain-input homes and pin joins remain.
// Original complete body and existing helpers restored.
// F399: Existing native gain-class pin input with integer/class returns, early/common
// results and direct/overloaded comparisons: 222/227/247/216/251/241/247/216.
// NOT kept: six-point lead below eight, unmatched record contracts and regressed shared
// clamp user; original pin, all three calls and whole body restored. Frame and pin joins remain.
// F401: Existing reference sum at both parallel permutation-gain producers, mutable/readonly
// converted parts crossed with inner reference sum and coherent scale: 222/222/269/269/277/277/249/249. NOT kept:
// ties or regressions; original arithmetic/body restored. Frame/gain homes and pin joins remain.
// N389: Complete state-word bit6 extraction, paired bit3 extractions and paired bit3
// updates, seven subsets, then the complete bit6 update: 222/222/222/222/222/222/222/222.
// NOT kept: no frame/gain-home improvement; original state accesses and full body restored.
// N391: Paired mutable/immutable short/long/real/double pitch endpoints:
// 263/263/253/253/222/222/277/277. NOT kept: no equality; extra argument
// homes and pin joins remain. Original endpoint expression and full body restored.
// N397: Complete native pitch, processed-gain and both submission-gain home identities 222/222/222/222/222/222/222/222.
// NOT kept: no byte equality or eligible native-shape gain; original complete body restored.
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
	real local_17 = local_1->location.scale;
	real local_5 = ((real)local_4->pitch_upper - (real)local_4->pitch_lower) * local_17 + local_1->pitch + (real)local_4->pitch_lower;
	s_decibels gain_sum = *(s_decibels *)&local_1->location.unknown08 + (*(s_decibels *)&local_1->gain + *(s_decibels *)&arg_1);
	long local_6 = *(long *)&gain_sum;
	local_6 = function_1251e0(local_2, local_6, local_17);
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
				local_14 = local_13->synch_count;
				local_13->synch_count--;
				local_13->synch_playback = false;
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

// MiniMax H (11-12): no new form, no lower score (535); retail still expands 0x1420f0 and 0x11c120, which carry noinline declarations on main.
// score 535 (was 535): Frame 0x130 versus 0xe8; native rotation, environment and listener-index homes remain.
// MiniMax B (11-11): no new form, no lower score (535); the body is unchanged, and the forms listed above are exhausted.
// MiniMax C (11-11): no new form, no lower score (535); our frame is 0x134 against retail's 0xe8, the fallback reverb constant's linkage lives in another file, and every form tried here lengthens the body and regresses.
// MiniMax E (11-11): no new form, no lower score (535); the frame is still 0x134 against retail's 0xe8 because the
// fallback reverb is copied to the stack here while retail reads the existing const, whose declaration is in
// another file; the state pointer, the counter register and the anchor offsets still differ.
// MiniMax G (11-11): no new form, no lower score (535): retail's frame is 0xe8 bytes and ours 0x134, so every stack reference is out, and retail tests the pointer against zero with cmp eax, edi where we test eax alone; our body is nine bytes longer than retail's 2290.
// MiniMax D (11-10): 535 (was 554): the blend factor is read from [ebx+0x40] at each use and the state's base components are named locals, so the interpolation matches retail's instruction order; still different: the frame 0x134 against retail's 0xe8 (the fallback reverb is a 48-byte stack copy here while retail reads the existing const g_44a2a0, whose internal linkage is in another file), the loop counter in ebx against retail's edi, the state pointer masked with and against retail's cmovne, ebp anchored at +7 against retail's +0x20; the counter declared before the null test 547, the state pointer as a named local 545, the inverted test 535, the local player helpers 0x14ddc0/0x14de70 called 609/625 (they stay calls here), all restored; the permuter's best variant (the entry branch inverted, which puts the skip path first as retail has it, with a dword local and the split load) aligns 18 more instructions but is 24 bytes longer, so it scores 535 and the inversion alone 533, below the threshold, and stays restored.
// MiniMax S (11-10): no new form, no lower score (554); the frame is 0x130 against retail's 0xe8 because the fallback reverb is a 48-byte stack copy here while retail reads the existing const g_44a2a0, which needs a declaration outside this file; a file-static copy of the same data removes the copy and the frame with it but takes 218 bytes off the length, so the differing count stays 535 and the plain score rises to 734; the two entry bytes, the rotation call's registers and the listener records still differ.
// MiniMax R (11-01): retail's 0x11d6a0 call still sets edx, ecx, eax where ours sets edx, eax, ecx;
// the callee's own file is not mine, so the order cannot be produced from here.
// b2F (11-06) walk against retail, no logic difference found (statements, fields, offsets, conditions and callees agree). Calls the
// existing location_point_in_zone_inline (location_zones.h; it was written out here) 555 (535 + 20); player state as
// &g_4e9bd4[i].state 554. The listener query stays short, since retail tests it as 16 bits (below); as a long 553. The zone call assigned straight to unknown07 560.
// Retail differences that remain (not behaviour): the fallback reverb is the const g_44a2a0 at 0x127b70/0x127bd9, not a stack copy
// (extern in 21e330.cpp + use here 734, frame gone, 2091 bytes); loop head is 0x14ddc0/0x14de70 inlined (calling them 651);
// 0x16f3a0 inlined for the cmovne at 0x1274d7-0x1274e3 (calling it 576, stays a call); retail re-reads [ebx+0x40] per use and tests
// the 0x155760 result as 16 bits (test bx, bx at 0x127502; the result is 0..3).
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
// Round 609: existing fallback constant plus complete listener/player/environment lvalue identities,
// seven subsets and the actual entry lvalue, 760 each (2065 bytes, frame 0xe8). NOT kept: native frame alone
// leaves early saves, loop/helper expansion and record homes different; original body and constant linkage restored.
// Round 400: shared zone predicate with original/direct publication, then
// existing cross, point setter and both boundaries: 555/554/643/640/609/610/640/637.
// NOT kept: every form regresses and lengthens; original predicate, helper
// calls, includes and complete body restored. Frame excess remains.
// Native player-array cursor, ordinal for/while 566/566; cursor bounds with retained
// ordinal for/while 573/573; derived ordinal for/while/ordered bound 557/557/557;
// cursor increment with ordinal bound 567. Original traversal restored.
// Round 235 (m3 lane 3, fast): retail also expands 0x1420f0 (the listener frame rows) and 0x11c120; both, like 0x16f3a0, have
// noinline declarations on main. The velocity rows through the existing 0x1427f0 668 (2157 bytes: it stays a call); the loop
// head's player index through 0x14de70 alone 625 (2381); `SOUND_SYSTEM->listeners + local_0` 535 (same code); a short loop index
// 624 (2385; close to the listed short listener counter); the state pointer declared after `local_12` 535 (same code); the
// cluster index stored before the leaf index 535 (same code); 0xba1d0's velocity call right after 0xcaf60 534 (a call moved by
// score, not kept); the blend factor read ahead of the 0xcaf60 call 559 (2328).
// score 535 (was 535): unchanged this round: the frame is 0x130 against retail's 0xe8 and 0x16f3a0, 0x1420f0 and
// 0x11c120 still have to stay out of line, and their noinline declarations live in other files.
// score 535 (was 535): unchanged this round: 0x16f3a0, 0x1420f0 and 0x11c120 have to stay out of line, and their
// noinline declarations live in other files.
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
			s_127401 const *local_6 = g_4686c4 != NONE ? (s_127401 const *)&g_4e9bd4[local_0].state : NULL;
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
					point3f local_20;
					function_caf60(local_3, &local_20);
					real local_19 = *(real *)(local_18 + 0x40);
					real local_26_x = local_6->field_0.x;
					local_9.x = (local_20.x - local_26_x) * *(real *)(local_18 + 0x40) + local_26_x;
					real local_26_y = local_6->field_0.y;
					local_9.y = (local_20.y - local_26_y) * *(real *)(local_18 + 0x40) + local_26_y;
					real local_26_z = local_6->field_0.z;
					local_9.z = (local_20.z - local_26_z) * *(real *)(local_18 + 0x40) + local_26_z;
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
			bool local_22 = location_point_in_zone_inline(&local_11, &local_9, NULL);
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

/* the per-voice and per-channel value pointers of one spatialization pass, cleared before use
   (a plain memset of a local this large makes VC realign the frame with `and esp, -8`; retail's
   clears don't, and a clearing member function is the only form found that avoids it; a clearing
   constructor does too but changes 0x1c4590's registers through link-time code generation) */
struct s_voice_value_table
{
	vector3f *values[0x40];

	void clear()
	{
		memset(values, 0, sizeof(values));
	}
};

struct s_channel_value_table
{
	vector3f *values[0x80];

	void clear()
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
			local_3.clear();
			local_4.clear();
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
