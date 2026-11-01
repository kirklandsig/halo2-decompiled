// @flags /O2 /Ob1 /arch:SSE /Gr
#include "pool_storage.h"
/* UNKNOWN_12A1B0.CPP: controllers and looping playback. */

#include "unknown_11c920.h"
#include "normalize3d.h"
#include "vector_angles.h"
#include "data_array.h"
#include "local_cameras.h"
#include "sound_sources.h"
#include "crc.h"
#include "unknown_218ac0.h"
#include "unknown_221810.h"
#include "unknown_124f90.h"
#include "vector_products.h"
#include "bit_vector.h"
#include <string.h>
#include <math.h>

class c_class_219e90
{
public:
	short salt;
	byte reference_count : 7;
	byte source_updated : 1;
	byte synch_count : 7;
	byte synch_playback : 1;
	long definition_index;
	dword field_8_4;
	real listener_values[4];

	void initialize(long definition_index, dword field_8_4, bool source_updated);
	void function_219f60(long count);
	void function_219ed0(s_type_99c531 const *source, bool source_updated);
	inline char function_xfd77ec(void) { return ++reference_count; }
};

struct s_type_5ef569
{
	short salt;
	short sound_class;
	long definition_index;
	long identifier;
	s_type_99c531 source;
	byte flags;
	byte playing_sound_count;
	byte state;
	byte update_phase;
	long controller_index;
	long detail_sound_times[12];
	byte track_storage[0xc4 - 0x88];
	real listener_gains[4];
};

/* These globals are owned by unknown_124f90.cpp. */
extern void *g_51ebd8;
extern void *g_51ebdc;
extern void *g_502110;

#define CONTROLLERS ((s_record_pool *)g_51ebd8)
#define LOOPING_SOUNDS ((s_record_pool *)g_502110)

struct s_looping_sound_system
{
	byte unknown00[0x78];
	bool initialized;
	bool hardware_available;
	bool enabled;
	byte unknown7b[4];
	byte source_update_phase : 1;
	byte unknown7f : 7;
	byte unknown80[4];
	long time;
	byte unknown88[0x1e0 - 0x88];
	real elapsed_time;
};

static inline bool looping_sound_system_available(void)
{
	s_looping_sound_system *system = (s_looping_sound_system *)g_4e6380;
	return system->initialized && system->hardware_available && system->enabled;
}

static inline c_class_219e90 *controller_get(long index)
{
	c_class_219e90 *result = (c_class_219e90 *)g_51ebdc;
	if (index != NONE)
		result = (c_class_219e90 *)CONTROLLERS->data + (index & 0xffff);
	return result;
}

static inline long next_sound_datum(s_record_pool *data, long index)
{
	long start = index == NONE ? 0 : (index & 0xffff) + 1;
	return data_datum_index(data, function_16bc00(data, start));
}

// @retail 0x2198f0
c_class_219e90 *function_2198f0(long index)
{
	return controller_get(index);
}

// @retail 0x219910
c_class_219e90 *looping_sound_controller_try_and_get(long index)
{
	c_class_219e90 *result = (c_class_219e90 *)g_51ebdc;
	if (index != NONE)
		result = (c_class_219e90 *)datum_get_inlined(CONTROLLERS, index);
	return result;
}

// @retail 0x219960
long function_219960(long definition_index)
{
	long result = NONE;
	if (looping_sound_system_available() && definition_index != NONE)
	{
		s_record_pool *data = CONTROLLERS;
		for (result = next_sound_datum(data, NONE); result != NONE; result = next_sound_datum(data, result))
		{
			if (controller_get(result)->definition_index == definition_index)
				break;
		}
	}
	return result;
}

// @retail 0x219a30
long looping_sound_controller_find_and_reference(long definition_index)
{
	long result = NONE;
	if (looping_sound_system_available() && definition_index != NONE)
	{
		result = function_219960(definition_index);
		if (result != NONE)
			((c_class_219e90 *)CONTROLLERS->data)[result & 0xffff].function_xfd77ec();
	}
	return result;
}

// @retail 0x219c80
inline void looping_sound_controller_release(long index)
{
	if (index != NONE)
	{
		c_class_219e90 *controller = (c_class_219e90 *)CONTROLLERS->data + (index & 0xffff);
		if (--controller->reference_count == 0)
			record_pool_release(CONTROLLERS, index);
	}
}

// @retail 0x219cc0
void looping_sound_controllers_update(void)
{
	s_record_pool *data = CONTROLLERS;
	long index = next_sound_datum(data, NONE);
	while (index != NONE)
	{
		c_class_219e90 *controller = controller_get(index);
		controller->field_8_4 = controller->field_8_4 * 1664525 + 1013904223;
		long start = index == NONE ? 0 : (index & 0xffff) + 1;
		index = data_datum_index(data, data_next_absolute_index_inlined(data, start));
	}
}

static inline void controller_initialize(c_class_219e90 *controller, long definition_index, dword field_8_4, bool source_updated)
{
	controller->definition_index = definition_index;
	controller->field_8_4 = field_8_4;
	controller->source_updated = !source_updated;
	controller->synch_count = 0;
	controller->synch_playback = true;
	memset(controller->listener_values, 0, sizeof(controller->listener_values));
}

// @retail 0x219e90
void c_class_219e90::initialize(long definition_index, dword field_8_4, bool source_updated)
{
	controller_initialize(this, definition_index, field_8_4, source_updated);
}

// @retail 0x219f60
void c_class_219e90::function_219f60(long count)
{
	if (definition_index != NONE)
		synch_count += count;
}

s_record_pool *function_11cc20(long maximum_count, const char *name, long size);

// @retail 0x21a0c0
bool function_21a0c0(void)
{
	g_502110 = function_11cc20(128, "looping sounds", sizeof(s_type_5ef569));
	if (g_502110)
	{
		LOOPING_SOUNDS->valid = true;
		record_pool_release_all(LOOPING_SOUNDS);
	}
	return g_502110 != NULL;
}

// @retail 0x21b070
void function_21b070(long index)
{
	s_type_5ef569 *sound = (s_type_5ef569 *)LOOPING_SOUNDS->data + (index & 0xffff);
	looping_sound_controller_release(sound->controller_index);
	record_pool_release(LOOPING_SOUNDS, index);
}

/* Dispose the looping sounds from the old map; retain the shared declaration. */
// @retail 0x21a1e0
void function_21a1e0(void)
{
	s_record_pool *data = LOOPING_SOUNDS;
	if (data)
	{
		s_record_pool_iterator iterator = { data, NONE, NONE };
		while (data_iterator_next_inlined(&iterator))
		{
			function_21b070(iterator.datum_index);
		}
	}
}

/* Existing helpers retain their upstream declarations. */
dword function_1462b0(void);
long players_first_active_local_player(void);
long function_14de10(long index);
real function_12a9d0(long listener_index, s_sound_position const *position);
void __stdcall function_21f430(long controller_index);


// @retail 0x219a90
long function_219a90(long definition_index)
{
	s_looping_sound_system *system = (s_looping_sound_system *)g_4e6380;
	long result = NONE;
	if (system->initialized && system->hardware_available && system->enabled && definition_index != NONE)
	{
		result = function_219960(definition_index);
		if (result == NONE)
			result = record_pool_allocate(CONTROLLERS);
		if (result != NONE)
		{
			c_class_219e90 *controller = (c_class_219e90 *)g_51ebdc;
			if (result != NONE)
				controller = (c_class_219e90 *)CONTROLLERS->data + (result & 0xffff);
			if (controller->function_xfd77ec() == 1)
				controller->initialize(definition_index, function_1462b0(), system->source_update_phase);
		}
	}
	return result;
}

// @retail 0x219b40
bool looping_sound_controllers_initialize(void)
{
	g_51ebd8 = function_11cc20(64, "sound playback controllers", sizeof(c_class_219e90));
	byte *top = (byte *)(g_510804_pool_size + g_510800_pool_base);
	byte *memory = (byte *)(((dword)top + 3) & ~3);
	long aligned_size = (memory - top) + sizeof(c_class_219e90);
	g_510804_pool_size += aligned_size;
	function_163ba0(&g_510808_pool_checksum, &aligned_size, sizeof(aligned_size));
	g_51ebdc = memory;
	if (g_51ebd8 && g_51ebdc)
	{
		CONTROLLERS->valid = true;
		record_pool_release_all(CONTROLLERS);
		byte phase = *((byte *)g_4e6380 + 0x7f);
		dword seed = function_1462b0();
		c_class_219e90 *controller = (c_class_219e90 *)g_51ebdc;
		controller->definition_index = NONE;
		controller->field_8_4 = seed;
		controller->source_updated = ~phase;
		controller->synch_count = 0;
		controller->synch_playback = true;
		memset(controller->listener_values, 0, sizeof(controller->listener_values));
	}
	return g_51ebd8 && g_51ebdc;
}

// @retail 0x219ed0
void c_class_219e90::function_219ed0(s_type_99c531 const *source, bool source_updated)
{
	if (definition_index != NONE && this->source_updated != source_updated)
	{
		if (source->audible == 1)
		{
			for (long index = players_first_active_local_player(); index != NONE; index = function_14de10(index))
				listener_values[index] = function_12a9d0(index, &source->spatial);
		}
		else
		{
			memset(listener_values, 0, sizeof(listener_values));
		}
		this->source_updated = source_updated;
	}
}

// @retail 0x219c30
void function_219c30(long definition_index, s_type_99c531 const *source)
{
	s_looping_sound_system *system = (s_looping_sound_system *)g_4e6380;
	if (system->initialized && system->hardware_available && system->enabled && definition_index != NONE)
	{
		long index = function_219960(definition_index);
		if (index != NONE)
		{
			c_class_219e90 *controller = looping_sound_controller_try_and_get(index);
			if (controller)
				controller->function_219ed0(source, system->source_update_phase);
		}
	}
}

// @retail 0x219d90
void looping_sound_controllers_synchronize(bool initial_playback)
{
	long index = next_sound_datum(CONTROLLERS, NONE);
	while (index != NONE)
	{
		c_class_219e90 *controller = controller_get(index);
		if (initial_playback ? (controller->synch_playback && controller->definition_index != NONE && controller->synch_count == 0) :
			(!controller->synch_playback && controller->definition_index != NONE && controller->synch_count == 0))
		{
			function_21f430(index);
			if (!initial_playback)
				controller->synch_playback = true;
		}
		long start = index == NONE ? 0 : (index & 0xffff) + 1;
		index = data_datum_index(CONTROLLERS, data_next_absolute_index_inlined(CONTROLLERS, start));
	}
}

void function_11bed0(s_location *location, point3f const *point);

// score 93 (was 93): Called location-query boundary and native datum/element homes remain; restore the complete opening executable body.
// Round 28 (lane G): what still differs is the location call itself: retail calls 0x11bed0 out of
// line here and our build inlines it, and the datum and element register homes follow from that;
// the loop body is retail's otherwise. The only forms that keep the call out of line are the
// implied-test ones the 2026-10-30 ruling rejects, so the native-iterator body stands.
// Earlier: native iterator keeps the initial called scan and subsequent inline scan; location lookup still expands.
// Published pool per use 161; shared bitmap scan 161; plain for 161; reused absolute slot 161;
// shared next-datum advance 116; source receiver 160; bool datum iterator 134 (fast). Native iterator 93 fast/plain.
// Native iterator aggregate 93; datum-index receiver 83 (one-use call-result pointer; unmatched, restored);
// absolute-slot receiver 110; absolute slot/source receiver 110; aggregate/absolute slot 110;
// shared salted getter 117; native while 93; absolute-slot continue 110;
// returned-datum eligibility/native index 147; typed returned datum 147; typed datum/aggregate 147;
// returned-datum eligibility/shared getter 192. Location expansion and datum/element register ownership remain.
// Round 88: 0x219d90's datum walk with a pool local (`s_record_pool *data = LOOPING_SOUNDS;`,
// next_sound_datum first, then the inline scan) is retail's code once 0x11bed0 is called: it
// matches with 0x11bed0 kept out of line (161 while it is inlined); without the pool local 161;
// the called advance 98. BLOCKED-BY 0x1d48f0: with 0x11bed0 out of line, a caller that declares
// a 16-byte-aligned local itself (0x1d48f0's Havok locals) moves 0x14a280's bsp to the stack; an
// aligned local that comes from an inlined helper does not, and no other caller does.
// Native iterator source/spatial receivers 93/99; location-result workspace alone,
// with source or spatial receiver 118 each; eligibility continue with source, spatial
// or location workspace 93/99/118. Restored.
// Complete spatial/source/sound snapshots by assignment or memcpy 165/165/188/188/187/187; Boolean iterator
// plus spatial/source snapshot 121/139. Restored.
// Round 191: Point-only assignment/CRT/setter copies at eligibility scope 140/140/142; at loop scope 140/140/142.
// Round 191: Point-only assignment/CRT copies with a separate complete location output 159/159. No exact match; every single-use point copy restored.
// Round 166: retail calls 0x11bed0 here because its call site is cold: round 88's index walk with
// `if (sound->source.audible == 1 && data && sound)` (pointer tests implied by the reads before them, folded
// away; LTCG's static profile still counts them) is a byte match, not kept (implied tests). With `&& index != NONE`
// alone the full link still inlines (162); `&& index != NONE && data && sound` 49 (the NONE test stays);
// the advance through the real next-datum function 98.
// Lane 13 round 24: retail never inlines 0x11bed0 (41 callers); a diagnostic `#pragma auto_inline(off)` around its
// definition makes the index walk a byte match here but changes 0x11bed0's own convention and loses 0x125920, 0x14a280
// and 0x1e1500 (reverted; BLOCKED-BY 0x11bed0's inline eligibility). Both iterator steps inlined 93; index walk with a
// source pointer 160, with a spatial pointer in the arm 162; the audible test in a bool local 93; every step through
// the calling iterator 106 (0x11bed0 still inlined).
// Lane 5 round 113: diagnostic (reverted; build flags are the user's call): unknown_11bed0.cpp built /Ob1 (0x11bed0
// never inlined, as at all 41 retail call sites) with round 88's index walk: a byte match here, but 0x11bed0 19,
// 0x125920 11, 0x14a280 14, 0x1e1500 42, 0x1697c0 118, 0x17c0e0 149 (0x14a280's bsp moves to the stack, round 88's
// 0x1d48f0 cause). Aggregate-initialized iterator 93 (same code); source pointer on the native iterator 93 (same code);
// index walk 161 (confirmed). BLOCKED-BY 0x11bed0's inline eligibility.
// @retail 0x21a100
void looping_sound_update_locations(void)
{
	s_record_pool_iterator iterator;
	iterator.data = LOOPING_SOUNDS;
	iterator.datum_index = NONE;
	iterator.index = -1;
	for (s_type_5ef569 *sound = (s_type_5ef569 *)data_iterator_next_calling(&iterator); sound;
		sound = (s_type_5ef569 *)data_iterator_next_inlined(&iterator))
	{
		if (sound->source.audible == 1)
			function_11bed0(&sound->source.spatial.location, &sound->source.spatial.position);
	}
}

/* Runtime track storage starts at +0x88. Keep the tail as a byte view until
   all of the retail array bounds have been recovered. */
struct s_type_06b94f
{
	long sound_index;
	dword permutations;
	dword field_8_4;
	char field_c_4;
	char next_state;
	byte completed_count;
	byte unknown0f;
	real gain;
	inline byte notify_completion(void) { return ++completed_count; }
};

struct s_type_a82d9f
{
	byte unknown00[8];
	long sound_definition_index;
	real period_lower;
	real period_upper;
	byte unknown14[4];
	dword flags;
	real yaw_lower;
	real yaw_upper;
	real pitch_lower;
	real pitch_upper;
	real distance_lower;
	real distance_upper;
};

struct looping_sound_track_definition
{
	long unknown00;
	dword flags;
	dword value08;
	real fade_in_duration;
	real fade_out_duration;
	long start_group;
	long start_sound_index;
	long loop_group;
	long loop_sound_index;
	long end_group;
	long end_sound_index;
	long alternate_loop_group;
	long alternate_loop_sound_index;
	long alternate_end_group;
	long alternate_end_sound_index;
	short effect;
	short unknown3e;
	long alternate_start_group;
	long alternate_start_sound_index;
	long alternate_transition_group;
	long alternate_transition_sound_index;
	real crossfade_duration;
	real alternate_fade_out_duration;
};

struct looping_sound_definition
{
	dword flags;
	byte unknown04[8];
	real minimum_distance;
	real maximum_distance;
	byte unknown14[8];
	long track_count;
	looping_sound_track_definition *tracks;
	long detail_sound_count;
	s_type_a82d9f *detail_sounds;
};

/* A playing sound's list links are inside its loop state at +0x5c. */
struct s_looping_track_links
{
	long controller_index;
	char track_index;
	byte flags;
	char synch_state;
	byte unknown07;
	dword field_8_4;
	dword permutations;
	long next_sound_index;
	long previous_sound_index;
};

struct s_looping_track_sound
{
	short salt;
	char state;
	byte unknown03;
	union
	{
		word flags;
		struct
		{
			word unknown04 : 3;
			word transition_pending : 1;
			word unknown04mid : 2;
			word stop_requested : 1;
			word looping : 1;
			word unknown04bit8 : 1;
			word channel_initialized : 1;
			word unknown04hi : 6;
		};
	};
	short sound_class;
	byte unknown08[4];
	long definition_index;
	long looping_sound_index;
	s_sound_source_callbacks const *callbacks;
	s_type_99c531 source;
	s_looping_track_links loop;
	byte unknown74[0x8c - 0x74];
	long time;
	real pitch;
	dword track_value;
	dword field_8_4;
	char pitch_range_index;
	char permutation_index;
	short current_chunk_index;
	byte priority;
	char listener_index;
	short chunk_index;
	char unknowna4;
	byte unknowna5[3];
	long handle;
	short channel_index;
	short fade_curve;
	long fade_gain_bits;
	long fade_start_time;
	long fade_end_time;
};

static inline s_type_06b94f *looping_track_get(long index, short track_index)
{
	s_type_5ef569 *sound = (s_type_5ef569 *)LOOPING_SOUNDS->data + (index & 0xffff);
	return (s_type_06b94f *)sound->track_storage + track_index;
}

static inline s_looping_track_sound *track_sound_get(s_record_pool *data, long index)
{
	return (s_looping_track_sound *)data->data + (index & 0xffff);
}

// @retail 0x21aeb0
long function_21aeb0(long definition_index, short sound_class, long identifier, long controller_definition_index)
{
	long result = NONE;
	if (looping_sound_system_available())
	{
		result = record_pool_allocate(LOOPING_SOUNDS);
		if (result == NONE)
			goto done;
		{
			s_type_5ef569 *sound = (s_type_5ef569 *)LOOPING_SOUNDS->data + (result & 0xffff);
			sound->sound_class = sound_class;
			sound->definition_index = definition_index;
			sound->identifier = identifier;
			sound->controller_index = function_219a90(controller_definition_index);
			sound->playing_sound_count = 0;
			sound->flags = 0;
			looping_sound_definition *definition = (looping_sound_definition *)g_4e3b44[definition_index & 0xffff].bytes;
			for (long i = 0; i < definition->detail_sound_count; i++)
			{
				s_type_a82d9f *detail = &definition->detail_sounds[i];
				real delay;
				if (detail->flags & 4)
					delay = 0;
				else
					delay = function_259d0(&g_4e7408->seed, __FILE__, __LINE__, detail->period_lower, detail->period_upper);
				sound->detail_sound_times[i] = (long)(delay * 1000.f + (real)((s_looping_sound_system *)g_4e6380)->time);
			}
			if (definition->flags & 0x30)
			{
				c_class_219e90 *controller = controller_get(sound->controller_index);
				s_type_06b94f *track = (s_type_06b94f *)sound->track_storage;
				for (long i = 0; i < definition->track_count; i++, track++)
				{
					if (controller->definition_index == NONE)
						random_next(&controller->field_8_4);
					track->field_8_4 = controller->field_8_4;
				}
			}
		}
	}
done:
	return result;
}

/* walks a track's ring of playing sounds once, from its first sound */
struct s_track_sound_iterator
{
	long first;
	long index;
	long count;
};

static inline void track_sound_iterator_new(s_track_sound_iterator *iterator, long first)
{
	iterator->first = first;
	iterator->index = first;
	iterator->count = 0;
}

static inline long track_sound_iterator_next(s_track_sound_iterator *iterator)
{
	if (iterator->count > 0 && iterator->index != NONE)
	{
		s_looping_track_links *links = &track_sound_get(g_4e637c, iterator->index)->loop;
		iterator->index = links->next_sound_index != iterator->first ? links->next_sound_index : NONE;
	}
	iterator->count++;
	return iterator->index;
}

// @retail 0x21b870
long looping_sound_find_sound(long looping_sound_index, long definition_index, long pitch_range_index, short track_index)
{
	s_track_sound_iterator iterator;
	long index;
	track_sound_iterator_new(&iterator, looping_track_get(looping_sound_index, track_index)->sound_index);
	while ((index = track_sound_iterator_next(&iterator)) != NONE)
	{
		s_looping_track_sound *sound = track_sound_get(g_4e637c, index);
		if (sound->definition_index == definition_index &&
			(pitch_range_index == NONE || sound->pitch_range_index == pitch_range_index))
			return index;
	}
	return NONE;
}

// @retail 0x21bc80
void looping_sound_link_sound(long looping_sound_index, short track_index, long sound_index)
{
	s_type_06b94f *track = looping_track_get(looping_sound_index, track_index);
	s_looping_track_links *sound = &track_sound_get(g_4e637c, sound_index)->loop;
	if (track->sound_index != NONE)
	{
		s_looping_track_links *first = &track_sound_get(g_4e637c, track->sound_index)->loop;
		sound->next_sound_index = first->next_sound_index;
		sound->previous_sound_index = track->sound_index;
		first->next_sound_index = sound_index;
		track_sound_get(g_4e637c, sound->next_sound_index)->loop.previous_sound_index = sound_index;
	}
}

// @retail 0x21cf80
long function_21cf80(long definition_index, long identifier)
{
	s_record_pool *data = LOOPING_SOUNDS;
	long index = next_sound_datum(data, NONE);
	while (index != NONE)
	{
		s_type_5ef569 *sound = (s_type_5ef569 *)data->data + (index & 0xffff);
		if (sound->definition_index == definition_index && sound->identifier == identifier)
			break;
		long start = index == NONE ? 0 : (index & 0xffff) + 1;
		index = data_datum_index(data, data_next_absolute_index_inlined(data, start));
	}
	return index;
}

// @retail 0x21bf00
bool __stdcall function_21bf00(long looping_sound_index, long definition_index, s_sound_marker const *marker, s_type_99c531 *source)
{
	bool result = false;
	s_type_5ef569 *sound = (s_type_5ef569 *)datum_get_inlined(LOOPING_SOUNDS, looping_sound_index);
	if (sound)
	{
		c_class_219e90 *controller = controller_get(sound->controller_index);
		if (sound->controller_index == NONE || controller->synch_playback)
		{
			*source = sound->source;
			result = true;
			if (sound->identifier == (sound->definition_index | 0x6000))
				source->audible = 0;
		}
	}
	return result;
}

long const g_44a150[7] = { 1, 1, 2, 5, 5, 1, 6 };
long const g_44a16c[7] = { 3, 3, 2, 4, 4, 3, 6 };
long const g_44a188[7] = { 20, 28, 36, 64, 44, 72, 52 };

// @retail 0x21a050
long looping_sound_track_has_next_sound(s_type_06b94f const *track, looping_sound_track_definition const *definition)
{
	if (track->field_c_4 != track->next_state)
	{
		long offset = g_44a188[track->next_state];
		if (*(long const *)((byte const *)definition + offset + 4) != NONE)
			return true;
	}
	return false;
}

static inline bool looping_sound_track_transition_is_immediate(s_type_06b94f const *track)
{
	if ((1 << track->field_c_4) & 0x29)
	{
		if (g_44a16c[track->field_c_4] == track->next_state ||
			g_44a150[track->field_c_4] == track->next_state)
			return true;
	}
	return false;
}

// @retail 0x21a080
bool looping_sound_track_transition_requires_stop(s_type_06b94f const *track)
{
	return !looping_sound_track_transition_is_immediate(track);
}

// @retail 0x21c140
bool __stdcall looping_sound_positions_equal(point3f const *a, point3f const *b)
{
	return fabs(a->x - b->x) < 0.0001f && fabs(a->y - b->y) < 0.0001f && fabs(a->z - b->z) < 0.0001f;
}

struct s_looping_source_flags
{
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word flag3 : 1;
	word flag4 : 1;
	word flag5 : 1;
	word flag6 : 1;
	word flag7 : 1;
	word unknown : 8;
};

struct s_looping_source_tail
{
	long unknown30;
	real value34;
	real value38;
	short value3c;
	short value3e;
	long value40;
};

struct s_looping_spatialization
{
	dword flags;
	byte unknown04[0x24];
	long listener_gains[4]; /* decibels, as real bits */
};

long function_2197f0(real gain);
inline long function_1251e0(void const *definition, long gain_bits, real scale);
dword vector3d_compress(vector3f const *vector);

// @retail 0x21ac30
void __stdcall looping_sound_track_notify(long looping_sound_index, long definition_index, long sound_index, long arg_dc61a6)
{
	s_looping_track_sound *playing = track_sound_get(g_4e637c, sound_index);
	if (!TEST_FIELD_BIT(playing->looping) || playing->fade_start_time <= playing->fade_end_time)
	{
		s_type_5ef569 *sound = (s_type_5ef569 *)datum_get_inlined(LOOPING_SOUNDS, looping_sound_index);
		if (sound)
		{
			s_looping_track_links const *tracking = (s_looping_track_links const *)arg_dc61a6;
			byte *completed = &((s_type_06b94f *)sound->track_storage)[tracking->track_index].completed_count;
			++*completed;
		}
	}
}

// @retail 0x21acc0
bool __stdcall looping_sound_track_spatialize(long looping_sound_index, long definition_index, s_sound_source_view const *source, s_sound_spatialization_view *spatialization)
{
	bool result = false;
	s_type_5ef569 *sound = (s_type_5ef569 *)datum_get_inlined(LOOPING_SOUNDS, looping_sound_index);
	if (sound && sound->identifier == (sound->definition_index | 0x6000))
	{
		void const *definition = g_4e3b44[definition_index & 0xffff].bytes;
		s_looping_spatialization *output = (s_looping_spatialization *)spatialization;
		for (short i = 0; i < 4; ++i)
		{
			long gain_bits = function_2197f0(sound->listener_gains[i]);
			long value_bits = function_1251e0(definition, gain_bits, sound->source.scale);
			output->listener_gains[i] = decibels_add(sound->source.unknown08, value_bits);
		}
		output->flags |= 0x10;
		result = true;
	}
	return result;
}

// @retail 0x21adb0
void __stdcall looping_sound_track_stopped(long looping_sound_index, long sound_index, long unused)
{
	s_type_5ef569 *sound = (s_type_5ef569 *)datum_get_inlined(LOOPING_SOUNDS, looping_sound_index);
	s_looping_track_links *tracking = &track_sound_get(g_4e637c, sound_index)->loop;
	long track_index = tracking->track_index;
	long next = NONE;
	if (tracking->next_sound_index != sound_index)
	{
		s_looping_track_links *previous = &track_sound_get(g_4e637c, tracking->previous_sound_index)->loop;
		s_looping_track_links *following = &track_sound_get(g_4e637c, tracking->next_sound_index)->loop;
		previous->next_sound_index = tracking->next_sound_index;
		following->previous_sound_index = tracking->previous_sound_index;
		next = tracking->next_sound_index;
	}
	if (sound)
	{
		--sound->playing_sound_count;
		s_type_06b94f *track = (s_type_06b94f *)sound->track_storage + track_index;
		if (track->sound_index == sound_index)
			track->sound_index = next;
	}
	if (tracking->controller_index != NONE)
	{
		c_class_219e90 *controller = looping_sound_controller_try_and_get(tracking->controller_index);
		if (controller && tracking->synch_state == NONE && controller->definition_index != NONE)
		{
			controller->synch_count--;
			controller->synch_playback = false;
		}
	}
}

// @retail 0x21bfb0
bool __stdcall function_21bfb0(long looping_sound_index, long definition_index, s_sound_marker const *arg_dc61a6, s_type_99c531 *source)
{
	s_type_5ef569 *sound = (s_type_5ef569 *)datum_get_inlined(LOOPING_SOUNDS, looping_sound_index);
	bool result = false;
	if (sound)
	{
		if (sound->source.audible)
		{
			source->spatial.velocity = sound->source.spatial.velocity;
			source->spatial.compressed_forward = sound->source.spatial.compressed_forward;
			source->spatial.location = sound->source.spatial.location;
			if (TEST_FIELD_BIT(((s_looping_source_flags *)&sound->source)->flag2))
				((s_looping_source_flags *)source)->flag2 = true;
			else
				((s_looping_source_flags *)source)->flag2 = false;
			s_looping_source_tail const *input = (s_looping_source_tail const *)sound->source.unknown30;
			s_looping_source_tail *output = (s_looping_source_tail *)source->unknown30;
			output->value34 = input->value34;
			if (TEST_FIELD_BIT(((s_looping_source_flags *)&sound->source)->flag3))
				((s_looping_source_flags *)source)->flag3 = true;
			else
				((s_looping_source_flags *)source)->flag3 = false;
			output->value38 = input->value38;
			if (TEST_FIELD_BIT(((s_looping_source_flags *)&sound->source)->flag4))
				((s_looping_source_flags *)source)->flag4 = true;
			else
				((s_looping_source_flags *)source)->flag4 = false;
			output->value3c = input->value3c;
			if (TEST_FIELD_BIT(((s_looping_source_flags *)&sound->source)->flag5))
				((s_looping_source_flags *)source)->flag5 = true;
			else
				((s_looping_source_flags *)source)->flag5 = false;
			output->value3e = input->value3e;
			if (TEST_FIELD_BIT(((s_looping_source_flags *)&sound->source)->flag6))
				((s_looping_source_flags *)source)->flag6 = true;
			else
				((s_looping_source_flags *)source)->flag6 = false;
			output->value40 = input->value40;
		}
		else
		{
			source->spatial.compressed_forward = vector3d_compress(g_4687a8);
			source->spatial.velocity = *g_4687a4;
		}
		source->spatial.position = *(point3f const *)arg_dc61a6;
		if (source->audible == 1)
		{
			source->spatial.position.x += sound->source.spatial.position.x;
			source->spatial.position.y += sound->source.spatial.position.y;
			source->spatial.position.z += sound->source.spatial.position.z;
		}
		result = true;
	}
	return result;
}

/* Retail folds this always-true callback with other identical functions
   at 0xa5750, already marked in unknown_09a9f0.cpp. */
static bool __stdcall looping_sound_tracking_data_equal(void const *, void const *)
{
	return true;
}

extern s_sound_source_callbacks const g_44a110 = {
	function_21bf00, looping_sound_track_notify, NULL,
	(bool (__stdcall *)(long, long, s_sound_source_view const *, s_sound_spatialization_view *))looping_sound_track_spatialize,
	looping_sound_track_stopped, NULL, looping_sound_tracking_data_equal, NULL
};

extern s_sound_source_callbacks const g_44a130 = {
	function_21bfb0, NULL, NULL, NULL, NULL, NULL,
	(bool (__stdcall *)(void const *, void const *))looping_sound_positions_equal, NULL
};

struct s_animation_ref;
struct s_animation_state;
struct s_packed_value;
struct s_set_ref;
struct s_sound;

struct s_looping_playback_definition
{
	word flags;
	byte unknown02[4];
	short sound_class;
	short pitch_range_base;
	char pitch_range_count;
	char playback_index;
};

struct s_short_bounds
{
	short lower;
	short upper;
};

struct s_looping_pitch_modifier
{
	byte unknown00[8];
	s_short_bounds pitch_bounds;
	byte unknown0c[8];
};

struct s_looping_sound_class
{
	byte unknown00[0xc];
	real gain_change_rate;
	byte unknown10[8];
	short pitch;
	byte unknown1a[0x38 - 0x1a];
};

struct s_looping_sound_tables
{
	byte unknown00[4];
	s_looping_sound_class *classes;
	byte unknown08[4];
	s_looping_pitch_modifier *pitch_modifiers;
};

dword function_2194c0(s_packed_value *value, s_animation_ref *ref);
void function_219560(s_packed_value *value, s_animation_ref *ref, dword data);
void function_2192b0(s_animation_state *state, s_animation_ref *ref, byte value);
long function_219370(s_animation_state *state, s_animation_ref *ref);

// score 10 (was 16): fallback argument scheduling and final alignment byte differ;
// common result 10; sound/playback reorder 16, with common result 10; fallback guard 16;
// reordered direct fields 16, reused track 22, common direct fields 10; named range sum 10; set pointer 10;
// long set-index local 10; fallback arm first with common result 10; inline set accessor 10;
// function_2194c0 parameters as (ref, value) at its definition and every call 0 (callee not ours, held).
// Lane 17 round 2: parameters as (playback, sound, definition, ...), the order retail's callers set up
// (edx sound, then ecx playback, at 0x21b940 and 0x21c400): 10 here, 0x21b940 16 -> 14 (kept).
// @retail 0x21cb90
dword looping_sound_get_permutations(s_looping_playback_definition *playback, s_type_5ef569 *sound, looping_sound_definition *definition, long track_index, short arg_58ecd0, bool clear)
{
	dword result;
	if ((definition->flags & 0x30) && sound->controller_index != NONE)
	{
		dword *permutations = &((s_type_06b94f *)sound->track_storage)[track_index].permutations;
		result = *permutations;
		if (clear)
			*permutations = 0;
	}
	else
		result = function_2194c0((s_packed_value *)&g_51ebd4->sets[playback->pitch_range_base + arg_58ecd0], (s_animation_ref *)playback);
	return result;
}

// @retail 0x21cbe0
void looping_sound_set_permutations(s_type_5ef569 *sound, looping_sound_definition *definition, s_looping_playback_definition *playback, long track_index, short arg_58ecd0, dword permutations, short permutation)
{
	if ((definition->flags & 0x30) && sound->controller_index != NONE)
		((s_type_06b94f *)sound->track_storage)[track_index].permutations = permutations;
	else
	{
		s_permutation_set *set = &g_51ebd4->sets[playback->pitch_range_base + arg_58ecd0];
		function_219560((s_packed_value *)set, (s_animation_ref *)playback, permutations);
		function_2192b0((s_animation_state *)set, (s_animation_ref *)playback, (byte)permutation);
	}
}

// @retail 0x21cc30
long looping_sound_get_previous_permutation(looping_sound_definition *definition, s_looping_playback_definition *playback, long arg_58ecd0)
{
	if (!(definition->flags & 0x30))
		return (short)function_219370((s_animation_state *)&g_51ebd4->sets[playback->pitch_range_base + arg_58ecd0], (s_animation_ref *)playback);
	return NONE;
}

long function_126bd0(long definition_index);
real sound_get_maximum_distance(s_sound const *sound, long definition_index);
long __stdcall function_127d00(s_type_99c531 const *source, real maximum_distance, real *distance);
short function_218f50(s_looping_playback_definition *definition, short previous, real pitch);
short function_219110(s_set_ref *ref, short offset, dword *used_mask, short previous, dword *seed, bool *all_used, bool mirror);
void sound_playback_set_chunk(long sound_index, long definition_index, short pitch_range_index, short permutation_index, short chunk_index);
long __stdcall function_125e60(s_looping_track_sound *sound);

// Creates one looping track sound; byte-matched at 832 bytes.
// @retail 0x21b940
long function_21b940(long looping_sound_index, long definition_index, long pitch_range_index, short track_index, short state, dword *seed)
{
	s_type_5ef569 *sound = (s_type_5ef569 *)LOOPING_SOUNDS->data + (looping_sound_index & 0xffff);
	looping_sound_definition *definition = (looping_sound_definition *)g_4e3b44[sound->definition_index & 0xffff].bytes;
	looping_sound_track_definition *track_definition = &definition->tracks[track_index];
	long result = NONE;
	s_type_99c531 *source = &sound->source;
	if ((byte)function_126bd0(definition_index))
	{
		s_looping_playback_definition *playback = (s_looping_playback_definition *)g_4e3b44[definition_index & 0xffff].bytes;
		real maximum_distance = sound_get_maximum_distance((s_sound const *)source, definition_index);
		long listener_index = function_127d00(source, maximum_distance, NULL);
		if (listener_index != NONE)
		{
			result = record_pool_allocate(g_4e637c);
			if (result != NONE)
			{
				s_looping_sound_tables *tables = (s_looping_sound_tables *)g_51ebd4;
				s_looping_sound_class *sound_class = &tables->classes[playback->sound_class];
				s_type_06b94f *track = (s_type_06b94f *)sound->track_storage + track_index;
				s_looping_track_sound *playing = track_sound_get(g_4e637c, result);
				playing->listener_index = (char)listener_index;
				playing->channel_index = NONE;
				playing->flags = 0;
				playing->pitch = function_259d0(seed, __FILE__, __LINE__, (real)sound_class->pitch, (real)sound_class->pitch);
				playing->track_value = track_definition->value08;
				playing->field_8_4 = *seed;
				playing->looping_sound_index = looping_sound_index;
				playing->source = *source;
				playing->state = (char)state;
				playing->sound_class = sound->sound_class;
				playing->time = ((s_looping_sound_system *)g_4e6380)->time;
				playing->callbacks = &g_44a110;
				playing->fade_end_time = 0;
				playing->fade_start_time = 0;
				playing->chunk_index = NONE;
				memset(&playing->unknowna4, 0xff, sizeof(playing->unknowna4));
				playing->handle = NONE;
				s_looping_track_links *links = &playing->loop;
				links->track_index = (char)track_index;
				playing->loop.flags = 0;
				links->field_8_4 = track->field_8_4;
				links->permutations = track->permutations;
				links->next_sound_index = result;
				links->previous_sound_index = result;
				if (playing->state == 2 && (playback->flags & 0x2000))
					links->flags |= 1;
				if (definition->flags & 0x20)
				{
					function_2198f0(sound->controller_index)->function_219f60(1);
					links->controller_index = sound->controller_index;
					links->synch_state = NONE;
				}
				else
					links->controller_index = NONE;
				short chosen_range;
				if (pitch_range_index != NONE)
					chosen_range = (short)pitch_range_index;
				else
				{
					s_short_bounds *bounds = &tables->pitch_modifiers[playback->playback_index].pitch_bounds;
					real lower = (real)bounds->lower;
					real pitch = ((real)bounds->upper - lower) * track->gain + playing->pitch + lower;
					chosen_range = function_218f50(playback, NONE, pitch);
				}
				dword permutations = looping_sound_get_permutations(playback, sound, definition, track_index, chosen_range, state == 2);
				short previous = (short)looping_sound_get_previous_permutation(definition, playback, chosen_range);
				short permutation = function_219110((s_set_ref *)playback, chosen_range, &permutations, previous, seed, NULL, false);
				looping_sound_set_permutations(sound, definition, playback, track_index, chosen_range, permutations, permutation);
				sound_playback_set_chunk(result, definition_index, chosen_range, permutation, 0);
				function_125e60(playing);
				looping_sound_link_sound(looping_sound_index, track_index, result);
				++sound->playing_sound_count;
			}
		}
	}
	return result;
}

// @retail 0x21b910
long function_21b910(long looping_sound_index, long definition_index, long pitch_range_index, short track_index, short state, dword *seed)
{
	long result = looping_sound_find_sound(looping_sound_index, definition_index, pitch_range_index, track_index);
	if (result == NONE)
		result = function_21b940(looping_sound_index, definition_index, pitch_range_index, track_index, state, seed);
	return result;
}

struct s_looping_pitch_bounds
{
	short unknown00;
	short minimum;
	short maximum;
	short natural_minimum;
	short natural_maximum;
};

struct s_looping_pitch_tables
{
	byte unknown00[0x1c];
	s_looping_pitch_bounds *bounds;
};

real function_12aff0(real a, real b, real c, bool flag);
long __stdcall function_2197b0(long curve, real gain, real scale);
long function_12a810(long sound_index);
real g_44a0f0 = 0.0f;

// @retail 0x219f80
long looping_sound_pitch_attenuation(s_looping_playback_definition const *definition, s_permutation_set const *arg_58ecd0, real pitch)
{
	long result = 0;
	if (definition->flags & 0x2000)
	{
		s_looping_pitch_bounds *bounds = &((s_looping_pitch_tables *)g_51ebd4)->bounds[*(short const *)((byte const *)arg_58ecd0 + 2)];
		real lower = (real)bounds->natural_minimum;
		if (!(fabs(pitch - lower) < 0.0001f) && pitch < lower)
			result = function_2197b0(2, g_44a0f0, function_12aff0((real)bounds->minimum, lower, pitch, true));
		else
		{
			real upper = (real)bounds->natural_maximum;
			if (!(fabs(pitch - upper) < 0.0001f) && pitch > upper)
				result = function_2197b0(2, g_44a0f0, function_12aff0((real)bounds->maximum, upper, pitch, false));
		}
	}
	return result;
}

struct s_looping_promotion
{
	byte unknown00[0xa];
	word flags;
	byte unknown0c[0x5c - 0xc];
};

struct s_looping_promotion_table
{
	long count;
	s_looping_promotion *entries;
};

void function_126df0(short curve, long new_sound, long old_sound, real duration);

/* fades out every playing sound of a track's ring over a duration in seconds */
// @retail 0x21bd00
void __stdcall looping_sound_fade_out(real duration, long first_sound_index)
{
	/* retail keeps duration on the stack */
	real const *reference = &duration;
	if (first_sound_index != NONE)
	{
		s_track_sound_iterator iterator;
		long index;
		track_sound_iterator_new(&iterator, first_sound_index);
		while ((index = track_sound_iterator_next(&iterator)) != NONE)
		{
			s_sound_playback *playing = SOUND_PLAYBACK_GET(index);
			s_looping_playback_definition *definition = (s_looping_playback_definition *)g_4e3b44[playing->definition_index & 0xffff].bytes;
			long promotion_index = (char)definition->unknown02[0];
			s_tag_header *header = g_4e034c->header ? g_4e034c->header_alt : NULL;
			s_looping_promotion_table *promotions = (s_looping_promotion_table *)g_4e3b44[header->datum_index & 0xffff].bytes;
			s_looping_promotion *promotion = &promotions->entries[(short)promotion_index];
			bool curve = !(promotion->flags & 0x400);
			function_126df0(curve, NONE, index, duration);
		}
	}
}

// score 24 (was 24): retail's code is this loop with 0x126df0 inlined (its fade-in half); the
// index and the element pointer have ESI and EDI the other way round. Earlier: the state load was
// folded before the element pointer was formed and the final looping store was scheduled early;
// pool at each use 66; count declared first 66; count renamed 66; state guard 66;
// plain for 66; iterator record 66; count-first iterator record 66; flag mask 66;
// shared iterator helper reading the next link at each use 64; the shared iterator record
// (first, index, count; next link read at each use) 35 (kept).
// playing at function scope, const playing, reversed state comparison, pool declared after
// the iterator, index before the iterator and looping before the duration store 35 each;
// conditional fade gain and named fade-gain result 37 each; getter repeated for the final store 81.
// Consistent per-use element receivers 94; canonical iterator for 59; both 113;
// canonical iterator continue 35; curve after endpoints 35; computed endpoint first 35;
// native word flag store 35; native word flag test/store 25, plain confirmed at 207/210 bytes.
// Element/state fold and early flag store remain.
// Native iterator-index receiver: word mask/shifted bool/bitfield/zero-first
// 25/35/35/30; the same with force-inline getter 25/35/35/30.
// Element formation and final flag-store schedule remain.
// Native word flag snapshot inside/before state with mask and first/last publication 75 each;
// boolean extraction under each lifetime/publication 82 each. Restored.
// Round 73: the bitfield looping test (retail's shr/test) 35; the flag set through 0x0bfe40 (retail's
// store order) 35; the state tested through the getter and the element taken in the branch 24; the
// 0x126df0 call with the state tested through this file's getter 42, through the callee's own
// SOUND_PLAYBACK_GET 24 (kept); then a one-use sound local, index first, iterator index, pool local,
// NONE-first loop test, `2 ==`, `index = NONE` and the state read through iterator.index 24 each.
// Round 15: convention already retail's (EBX, ret 8); `continue` on state != 2, early return,
// iterator fields index-first, iterator/both at function scope 24 each. Compile-only with a
// forced-inline 0x126df0 copy gives retail's index EDI/element ESI when the count gets EBX;
// with the first index in EBX (LTCG) ours swaps them.
// Round 57: aggregate iterator initialization 24; first-initialized index 24; iterator pointer 50;
// index declared after initialization 24; first-next while/do-while 64/64; current-index rename 24;
// named state predicate 24 (one-use local, restored). Plain checks throughout; original body restored.
// Lane 18 round 15 (fast with 0x126df0, 0x12a810 and the matched siblings; it reproduces 24): 0x126df0's
// fade-in gain as a ternary 37 (0x126df0 15); one `sound` local shared by 0x126df0's two halves 24; the
// iterator's next link tested `== first ? NONE :` 24; both iterator helpers __forceinline 24, only the
// next helper 24; a __forceinline declaration of 0x126df0 in this file 24. Diagnostic: without the state
// test the first index leaves EBX for the stack and the index takes EDI (48), so the conditional arm
// weighs into the tie. A 600 s fast annealing permuter run: 64 tries, none below 24 (its reduced image scores the
// original 57, so it cannot see this tie).
// @retail 0x21be20
void looping_sound_fade_in(long first_sound_index, short curve, real duration)
{
	if (first_sound_index != NONE)
	{
		s_track_sound_iterator iterator;
		long index;
		track_sound_iterator_new(&iterator, first_sound_index);
		while ((index = track_sound_iterator_next(&iterator)) != NONE)
		{
			if (SOUND_PLAYBACK_GET(index)->state == 2)
				function_126df0(curve, index, NONE, duration);
		}
	}
}

/* a detail sound's random offset: a random distance along a random yaw and pitch */
// @retail 0x21ce60
void function_21ce60(s_type_a82d9f const *detail, vector3f *offset)
{
	real distance = function_259d0(&g_4e7408->seed, __FILE__, __LINE__, detail->distance_lower, detail->distance_upper);
	if (distance != 0.f)
	{
		real pitch = function_259d0(&g_4e7408->seed, __FILE__, __LINE__, detail->pitch_lower, detail->pitch_upper);
		real yaw = function_259d0(&g_4e7408->seed, __FILE__, __LINE__, detail->yaw_lower, detail->yaw_upper);
		direction_from_angles(offset, yaw, pitch);
		vector3f_scale(offset, distance, offset);
	}
	else
		*offset = *g_4687a4;
}

struct s_looping_detail_request
{
	long flags;
	long mode;
	long type;
	s_type_99c531 source;
	long looping_sound_index;
	long unknown54;
	s_sound_source_callbacks const *callbacks;
	vector3f offset;
	byte unknown68[0x8c - 0x68];
	short tracking_data_size;
};

// score 127 (was 127): return result first restores entry load allocation; the first-player expansion remains (retail
// calls 0x14dde0, ours inlines it), and the audible arm keeps the detail in EDI where retail reloads it.
// Round 28 (lane G): what still differs is the first-player query: retail calls 0x14dde0 out of line
// and our build inlines it, and the audible arm's detail then stays in EDI where retail reloads it;
// the rest of the body is retail's. The out-of-line forms are the implied-test ones the 2026-10-30
// ruling rejects, so the body stands.
// Round 8: the first-player query stays inlined whatever the else arm's shape: the next-player step
// through the out-of-line 0x14de10 (diagnostic) 97 with the query still inlined; the skip loop as a
// for, `while (skip-- > 0)`, an inverted count test and a guarded do-while all unchanged (154 fast).
// Shared offset 170f; direct source 192f; memcpy 170f; tag first 192f; mode arms/type last 168f;
// offset publication last 167f; camera-position receiver 169f. Fast control 170, result first 154f/138 plain.
// Round 178: native short selection/count/both and unsigned selection 146/140/147/146;
// shared position add 138, with native count 140; preparation noinline/inline 138/138.
// Original body and interfaces restored; source-copy scheduling and first-player expansion remain.
// Round 396: result initialization after copy/clear/mode 154/154/176; success-default 183;
// admission result 186; result homes prepared/request_ready/success 138 each.
// Existing first-player query noinline 25 at 509/508, plain; rejected for three matched-caller losses.
// Round 398: input-source pointer/reference/value/const-value 157/157/194/194;
// const pool receiver with source pointer 157; direct destination with input pointer/value/const-value
// 188/184/184. Original complete source observations and body restored.
// Round 186: caller-only query noinline/inline 138/138; mode predicate 138;
// destination after copy/reference 138/138; offset workspace name 138;
// caller noinline with later destination 138; mode with later destination 138.
// Round 77: a noinline first-player query plus a header copy for its three inlining callers matched,
// but DECOMPILING.md rules out added noinline, so it was reverted; mode before type, audible stores in
// struct order, vector3f_add for the position and the camera taken first in the else arm 133 (under the
// keep bar, restored).
// Round 166: a third conditional level above the query keeps it out of line (LTCG's static profile counts
// tests the optimizer later drops): `else if (sound->source.audible != 1)` 25 at 509/508, and with round 77's
// three forms a byte match, not kept (the test is implied by the `if` above). Plain else with the struct-order
// stores, vector3f_add and the camera first 133 (under the keep bar); vector3f_add alone 138, kept.
// Round 108 (plain checks): player count read before the audible test with `else if (player_count > 0)` 153;
// the skip loop as a for with the query in its init 138; the arms swapped (`!= 1` first) 155; camera fetched
// before the audible store alone 139 (535 bytes); then-arm stores in struct order alone 136. The query stays
// inlined in all five. Original body restored.
// Round 319: Enclosing player-count lifetime with test-produced result, inactive-first
// arm and native publication-order subsets 165/210/173/208/164/210/172/208
// (reduced control 141, full 138). All restored; query expansion remains.
// Lane 17 round 5 (plain): out-of-line 0x14de10 for the skip step 97 at 495 bytes (retail inlines that step; not
// kept); counted-up skip loop 138; camera looked up at each use 157; flags before result in the else arm 138;
// type before the source clear 137; mode before type 134 (prefix exact to +0x7c); with it, detail address taken
// 134 (diagnostic); offset copy after the callbacks 132, between flags and index 133; with that, the position sum as
// offset + camera 132 (retail's operand order); with that, size before flags 132, result before the request stores
// 127 (kept: mode before type, offset after callbacks, offset + camera, result first), audible after the camera 133.
// Lane 5 round 113 (previous note 127, was 138; fast with 0x21b0e0, agrees with plain): the existing
// sound_definition_get with the named type field 127 (same code); the mode as `!definition[4] ? 2 : 0` 127 (same code);
// the else arm as `else if (count > 0)` reading the count in the condition 129; a short skip 137; the query called
// before the random draw 137 (still inlined); a colder call site in 0x21b0e0 (diagnostic, folded pointer tests) 127:
// the caller's site does not decide the query's inlining here.
// Lane 10 round 34 (fast): count through a table pointer local 139; camera after the offset call 152; skip loop as a
// counted for 139; result set before the random draw 197; diagnostic: a real extra test on the count makes the query
// a call (124 with the extra compare), so the cause is still the call site's static frequency.
// Round 254: Reduced common finalization with unavailable-player/completed-request exit and
// bool/byte/char/long result carriers 139/168/168/185/139/168/168/185. All restored.
// Lane 9 round 67 (from 127; fast alone with 0x21cc60 controls 139 at 543 bytes, the reduced image differs from the
// one built with 0x21b0e0): the existing 0x20b100 for the draw 127 plain (inlined to the same code); the count read
// at each use 141 fast; the draw through _random with file and line 139 fast (same code); `player_count != 0` 139 fast;
// `while (skip != 0)` 139 fast. The query stays inlined in all five. Diagnostic: an extra inlined vector add in the
// audible arm leaves it inlined too (607 bytes), so it is the call site's static frequency, not an inline budget.
// Lane 18 round 17 (an LTCG link of this file's generated unit, 85 objects, equal to the full listing): the skip
// loop as `while (skip-- != 0)` 127; the count test `!= 0` 127; `player` declared before the draw 127 (all three
// identical, the query stays inlined); the skip as the for's own counter harness 121 against the control's 110
// (533 bytes); `result = true` at the arm's top harness 177. The query's body (0x14dde0) sits in a file another
// lane holds this round, so its inline cost was not tried from the callee side.
// Round 128 (m3 lane 6, plain): retail calls 0x14dde0 from seven sites (0x1132f0, 0x125b40, 0x1391ed, 0x1df3e0,
// 0x219ed0, 0x240820 and this one) and inlines it at four (0x11b350, 0x13c250, 0x15c150, 0x225730); only this
// site differs. 0x14dde0's body as `return local_player_next_inline(NONE);` (same bytes, larger before folding)
// 127, every caller unchanged; the count test as `>= 1` 128, as `!= 0` (diagnostic) 127, the query still
// inlined; the definition index named once for the lookup and the audible arm's call 159; the camera position
// as one pointer local for the add and 0x11bed0 125 (two points, not kept).
// Round 48 (m3 lane 11, plain): the file is already /Ob1 and 0x11b350 (an /Ob1 file) inlines the query in retail,
// so the caller's flags are not the lever. The player count through a small inline accessor (one more inlined call
// site) 127; the audible test through a named bool 127; the test as a two-arm switch on the mode field 164 (540
// bytes, query still inlined); accessor and bool together 127; the accessor returning short into a short count 129.
// @retail 0x21cc60
bool looping_sound_prepare_detail(s_type_a82d9f const *detail, long looping_sound_index, s_looping_detail_request *request)
{
	bool result = false;
	s_type_5ef569 *sound = (s_type_5ef569 *)LOOPING_SOUNDS->data + (looping_sound_index & 0xffff);
	byte const *definition = g_4e3b44[detail->sound_definition_index & 0xffff].bytes;
	s_type_99c531 *source = &request->source;
	*source = sound->source;
	source->unknown02 = 0;
	request->mode = definition[4] ? 0 : 2;
	request->type = 0x10;
	if (sound->source.audible == 1)
	{
		source->audible = 1;
		vector3f offset;
		function_21ce60(detail, &offset);
		function_21bfb0(looping_sound_index, detail->sound_definition_index, (s_sound_marker const *)&offset, source);
		result = true;
		request->flags = 0x2f;
		request->looping_sound_index = looping_sound_index;
		request->callbacks = &g_44a130;
		request->offset = offset;
		request->tracking_data_size = sizeof(offset);
	}
	else
	{
		long player_count = *(short *)&g_4e8c20->unknown00[8];
		if (player_count > 0)
		{
			long skip = random_index(&g_4e7408->seed, (short)player_count);
			long player = players_first_active_local_player();
			while (skip > 0)
			{
				player = local_player_next_inline(player);
				--skip;
			}
			source->audible = 1;
			s_local_camera *camera = local_camera_get(player);
			vector3f offset;
			function_21ce60(detail, &offset);
			vector3f_add(&offset, (vector3f const *)&camera->position, (vector3f *)&source->spatial.position);
			function_11bed0(&source->spatial.location, &camera->position);
			source->spatial.compressed_forward = vector3d_compress(g_4687a8);
			source->spatial.velocity = *g_4687a4;
			request->flags = 7;
			result = true;
		}
	}
	source->requested_audible = source->audible;
	((s_looping_source_flags *)source)->flag7 = true;
	return result;
}

bool g_4e636d;
real sound_get_minimum_distance(s_sound const *sound, long definition_index);

/* reads the distances of the definition's first track that has a loop sound
   (the maximum is read and dropped) and then scans the detail sounds; retail
   tests the first track on every pass and keeps both distance reads out of line */
// @retail 0x21d060
void function_21d060(long definition_index, s_type_99c531 const *source)
{
	if (g_4e636d && source->audible == 1)
	{
		looping_sound_definition *definition = (looping_sound_definition *)g_4e3b44[definition_index & 0xffff].bytes;
		short i = 0;
		while (i < definition->track_count)
		{
			looping_sound_track_definition *track = definition->tracks; /* never advanced */
			if (track->loop_sound_index != NONE)
			{
				real minimum = sound_get_minimum_distance((s_sound const *)source, track->loop_sound_index);
				sound_get_maximum_distance((s_sound const *)source, track->loop_sound_index);
				if (minimum != 0.f)
					return;
				break;
			}
			++i;
		}
		for (long i = 0; i < definition->detail_sound_count; ++i)
		{
			if (definition->detail_sounds[i].sound_definition_index != NONE)
				break;
		}
	}
}

struct s_looping_voice_group
{
	short count;
	short voice_indices[16];
	short limit;
	bool started_this_tick;
	byte unknown25;
};

struct s_looping_voice_counts
{
	s_looping_voice_group definition;
	s_looping_voice_group source;
};

struct s_sound_playback;
struct s_sound_voice;
extern s_sound_voice *g_4e6378;

struct s_looping_voice_view
{
	long sound_index;
	byte unknown04[8];
	short channel_index;
	byte unknown0e[0x24 - 0xe];
};

void function_126960(s_sound_playback *sound);
__declspec(noinline) void __stdcall function_127320(long datum, long count);
void function_128500(long sound_index, s_looping_voice_counts *counts);
short function_128a60(long sound_index, short count, short const *voice_indices);

/* retail expands the permutation getter in function_21c190 and calls it elsewhere */
inline dword looping_sound_get_permutations(s_looping_playback_definition *playback, s_type_5ef569 *sound, looping_sound_definition *definition, long track_index, short arg_58ecd0, bool clear);
inline void looping_sound_set_permutations(s_type_5ef569 *sound, looping_sound_definition *definition, s_looping_playback_definition *playback, long track_index, short arg_58ecd0, dword permutations, short permutation);

// score 50 (was 50): the prologue through the pitch-range call agrees; left: both permutation
// paths read the track index inside their arms (`add eax, 7`) where retail reads it before the
// flag test (`lea [x + x*4 + 0x23]`), retail loads the flags into EAX for the take test, the
// shared take path schedules as the 0x21cb90 note says (BLOCKED-BY 0x2194c0's parameter order),
// and retail extends the previous permutation (`movsx eax, ax`).
// Round 36: the take twin indexing the track entry twice 146 (confirmed); the previous value through
// the existing 0x21cc30 with an `inline` declaration 129 (retail's `movsx`, confirmed); the helper
// without `inline` 172 (called out of line); `long previous` 129 (the same `movsx`). All restored.
// Round 106: the transition flag set through the existing word_flags_set 75 (kept: the flag store
// through a pointer stops the track-index read being reused across the pitch-range call, so the
// definition takes ESI); the playing record before the definition 50 (kept); on that, `result`
// declared before the pitch-range call 48 (two points, not kept); the previous value through the
// existing 0x21cc30 with an `inline` declaration 127 (retail's `movsx`, one instruction more);
// the set through the existing 0x21cbe0 with an `inline` declaration (same code as written);
// 0x21cb90 indexing the track entry twice 146 (its own 10 unchanged; the take path then reads
// the index before the test with retail's `lea`); with `result` late and 0x21cc30 65; `long
// previous` 65 (same code).
// Earlier: result first 176; tag per use 236; tag/range/mask names 176; outputs declared first 176;
// counts at entry 176; reasons before span selection 176; native selected voice group 188; short
// previous 166. Round 180: existing get/previous/set helper calls, seven subsets 215/199/245/224/282/
// 269/315 (called out of line); short previous in if/else arms 166. Round 189: direct offset/track/both
// 166/166/166; update receiver at each use 187; char/short/long result 172/159/160; short result with
// offset/track/both 159; separate success return 137 (frame 0x5c, no result byte); on it long
// previous/range/permutation/all 166/152/177/165; direct false default 137. Round 191: admission
// result in bool/byte/short/long 146/147/160/156, with tag/mask/range names the same.
// Round 82: one `result` set by the stop path 166 (retail frame and both tails); existing set-chunk
// call (same code); the take twin written single-exit 97; reason before indices 93; the existing get
// helper with an `inline` declaration in place of the twin 93 (same code, the big callers unchanged);
// existing set-permutations call 224 (out of line); existing previous call 192 (out of line), with an
// `inline` declaration 163 (inlined, adds retail's `movsx eax, ax`, which shifts the listing; all
// three helpers so 161); `long previous` 163 (the same `movsx`); take twin `__forceinline` 166; flag
// store through a second record lookup 156 (88 on 97); the set-path index read into a local before
// the flag test 163 (still read inside the arm); the record before the definition 94; the track
// definition before the state offset 93.
// Round 307: Complete stable track/policy/range input subsets 109/174/179/194/195/188/199;
// all with widened previous result 201. Original field observations and native scalars restored;
// track-index retention, definition/mask/range homes and flag/index schedule remain.
// Lane 13 round 24 (previous note 50, was 93): 0x21cb90 indexing the entry twice with a long previous 67; the
// pitch-range result as long 79; previous preset to NONE and set under the flag test (long) 136; 0x21cb90 with the
// fallback arm first 50 (same code); 0x21cb90's track index as char 50 (0x21cb90 24, 0x21b940 23, 0x21c400 378).
// Lane 5 round 113 (fast, agrees with plain): the set path through the existing 0x21cbe0 with an `inline` declaration
// in place of its body written by hand 50 (same code, kept); 0x21cb90's track index short 50 (0x21cb90 24, 0x21c400
// 370); a flags local with the twice-indexed take 146; 0x21cc30 with an `inline` declaration on this body 129 (619
// bytes, retail's `movsx eax, ax`), with the twice-indexed take 67 (aligned 25 of 180 against 26: round 106's and lane
// 13's forms, confirmed); the twice-indexed take alone 146 (confirmed). Diagnostic: 0x2194c0's parameters swapped
// everywhere 49 (0x21cb90 matches); the 0x2194c0 arm's argument schedule here mirrors the 0x219370 site's, not that
// cause.
// @retail 0x21c190
bool function_21c190(s_type_5ef569 *sound, s_type_06b94f *track, long sound_index, dword *seed)
{
	bool result = false;
	s_looping_track_sound *playing = track_sound_get(g_4e637c, sound_index);
	looping_sound_definition *definition = (looping_sound_definition *)g_4e3b44[sound->definition_index & 0xffff].bytes;
	long offset = g_44a188[track->next_state];
	looping_sound_track_definition *track_definition = &definition->tracks[playing->loop.track_index];
	long definition_index = *(long *)((byte *)track_definition + offset + 4);
	s_looping_playback_definition *playback = (s_looping_playback_definition *)g_4e3b44[definition_index & 0xffff].bytes;
	word_flags_set(&playing->flags, 3, true);
	short arg_58ecd0 = function_218f50(playback, playing->pitch_range_index, playing->pitch);

	/* Retail expands the permutation helpers here, but calls them from
	   function_21b940. Keep the same shared/local mask rules. */
	dword permutations = looping_sound_get_permutations(playback, sound, definition, playing->loop.track_index, arg_58ecd0, true);

	short previous = !(definition->flags & 0x30) ?
		(short)function_219370((s_animation_state *)&g_51ebd4->sets[playback->pitch_range_base + playing->pitch_range_index], (s_animation_ref *)playback) : NONE;
	short permutation = function_219110((s_set_ref *)playback, arg_58ecd0, &permutations, (short)previous, seed, NULL, false);
	looping_sound_set_permutations(sound, definition, playback, playing->loop.track_index, arg_58ecd0, permutations, permutation);

	sound_playback_set_chunk(sound_index, definition_index, arg_58ecd0, permutation, 0);
	if (playing->channel_index != NONE)
	{
		s_looping_voice_counts counts;
		function_128500(sound_index, &counts);
		short count;
		short const *indices;
		long reason;
		if (counts.source.count >= counts.source.limit)
		{
			count = counts.source.count;
			reason = 5;
			indices = counts.source.voice_indices;
		}
		else if (counts.definition.count >= counts.definition.limit)
		{
			count = counts.definition.count;
			reason = 6;
			indices = counts.definition.voice_indices;
		}
		else
			count = 0;
		if (count > 0)
		{
			short voice_index = function_128a60(sound_index, count, indices);
			long stopped_sound = voice_index == NONE ? sound_index : ((s_looping_voice_view *)g_4e6378)[voice_index].sound_index;
			if (stopped_sound != NONE)
			{
				function_127320(stopped_sound, reason);
				result = stopped_sound == sound_index;
			}
		}
	}
	track->field_c_4 = track->next_state;
	return result;
}

long function_125f70(long definition_index, s_looping_detail_request *request, long *reason);
// score 305 (was 305): the frame is 0x11c where retail's is 0x128, because the 0xa0-byte sound play
// state is passed on (this file's view of the detail request stops at 0x90); retail also keeps the
// playback pointer in EDI where ours keeps it in a slot, and reads each pitch-range loop's count
// once into a stack slot. The definition, index and ring homes still differ.
// At 307: extending the detail request to retail's 0xa0 bytes (the callee 0x125f70
// reads it as a 0xa0-byte play state) makes the frame 0x12c, four over retail's 0x128, and the
// score does not move; the homes differ by 4, 8, 28 and 36 bytes, so ours is not a shift of
// retail's layout: the original body is restored.
// Lane 18 round 14: approach_126660 now takes (value, delta, maximum) and forms the lower bound
// itself, as every caller passed `0 - maximum, maximum` (0x1269f0 matched with it): the gain
// step's call 307 -> 305 (plain).

// Earlier: frame 0x128 matched with the stop helper out of line;
// index homes, definition register and ring traversal still differ.
// Separate noinline declaration 501; wide selection full 502 (reduced 315);
// reduced control 429; indexed tracks 495; loop pool 427; ring pool 429; all pools 427;
// indexed fills 389; shared lower pitch 427; array creation order 430; early ring plus wide selection 315.
// Reduced-only improvements restored; the full image did not validate the width gain.
// Round 235: request/ring/seed lexical lifetime subsets and entry request with both,
// eight reduced ties at 461; unchanged reduced control 461 (frame 0x124, fade callee caveat).
// Complete executable body restored; ordinary frame/index homes remain 501.
// Round 185: Outer/track index, range arrays/mask and ring/seed/count role-name groups, seven
// subsets and alternate complete lexical order: reduced 461 each. Original names, executable body,
// existing call boundaries and all field observations restored.
// J185: direct range bounds full 469; with indexed tracks and consistent pools full 457 kept.
// Reduced control 429; clamp/real table/both 429/451/451; bounds with clamp 384, with real table/both 494/494;
// native outer iterator 514. On bounds: indexed/selection/both 373/476/469; pools/indexed+pool/selection+pool/all 369/358/474/460.
// On 457, activity Boolean/previous byte/range-word subsets 476/481/398/494/527/492/527; long previous with both 536.
// Width and producer tails restored; original calls, selected type and arithmetic grouping retained.
// Round 189: Full control 457, reduced control 358. tag at each use 533f; playback at each use 494f; direct
// track definition 478f; class at each use 476f; tag and class 537f; ring for 358f; header memset 457f; all tags
// with ring for 566f.
// Original body retained; frame 0x11c versus 0x128 and index/tag/ring homes remain. No reduced-only gain selected.
// Round 421: track/detail count snapshots and shared range ordinal, seven subsets:
// reduced 513/510/474/401/407/520/474; all with full-width selection 511. Reduced
// control 521/1908 differs from full 457/1923. Shared ordinal plain 457 (tie);
// all executable forms restored. Frame and index/range/ring homes remain.
// Lane 13 round 24 (previous note 457, was 457): the gain step through the existing approach_126660 (moved with
// PIN_126660 into include/unknown_124f90.h; retail's clamp-plus-gain adds) 307 (kept). Before it, the request view
// given its full 0xa0 bytes 455 (frame 0x12c). On 307: the pitch bounds through a bounds pointer with a named lower
// bound 306; with the 0xa0 request 306; one pitch-range count local 370 (playback moves to EDI); the count in each for
// 365 (frame 0x138); the entry gain through decibels_add 433.
// Lane 5 round 113 (previous note 307, was 455; plain): the second pitch-range loop bounded by a count local read
// between the loops 377 (frame 0x128 as retail, first difference +0x39, playback in EDI as retail, aligned 144 of 535
// against 168; the scalar slots come out permuted and the iterator index and `active` spill); with the 0xa0 request
// 380; the count in the second for's init 377; the count declared before the 0x2000 test 377; one count local bounding
// both loops 462 (1939 bytes).
// Round 254: Reduced shared full request type with no/first/second/both long range counts,
// first/second/both short counts and both byte counts 479/491/497/363/465/451/469/508. All restored.
// Ordinary full-request/two-long-count form 370 at 1923 bytes; rejected against full 307.
// Lane 9 round 67 (plain, from 305): the getter at each use for the range gain store 410 (the pool data
// is reloaded between the stores; retail forms two entry addresses from one data load); a count local
// before each pitch-range loop 365 (frame 0x128, first difference +0x39; the update branch through the
// 0x218f50 call is retail's: track EBX, playback EDI, previous BP); with 0x21b940's bounds-pointer pitch
// 460 (1939 bytes: the gain step's inlined add flips); with the sounds filled by memset(0xff) instead 374
// (the 0x2000 block's stores, pool EDX, iterator count ESI and immediate NONE tests are retail's; frame
// 0x11c; the second loop keeps index and track index in EBX/EBP and &sounds[i] in EDI where retail keeps
// the track definition in EBP and NONE in EBX, and `active` spills); with that and the 0xa0 request 374
// (frame 0x12c: the `active` spill is the one extra slot). The pitch sum still adds the lower bound first.
// Lane 18 round 17 (plain where marked, else an LTCG link of this file's generated unit with 133 objects that
// reproduces the full listing, 304 there for 305): what retail shows is (a) a count local per pitch-range loop
// (`for (long i = 0, count = ...; i < count;` 363; read once and reassigned 364/429), (b) the sounds cleared with
// memset(sounds, NONE, sizeof(sounds)) (the iterator loop is then retail's: count in ESI, pool in EDX, NONE as an
// immediate; 367 with (a)), (c) the mask bit tested at each use in the second loop, which CSEs into EDI after jump
// threading and is tested twice as retail (440 with (a)/(b); the `active` local as dword spills, as bool 353), (d)
// gains[i] copied into a long before `*(real *)&` (retail's copy in track_definition's slot) 247, plain 246 at
// 1923 bytes (kept nothing: (c) and (d) are kept only on a match). On that 246 form: all 24 orders of iterator,
// memset, gains and mask 246-250; gain stores as two gets 353, entry + get 366, through decibels_add 303-458;
// without the modifier bounds pointer and named lower (0x21b940's form) 252; the bounds as a {lower, upper} sub-struct
// pointer (retail's `lea ebx, [..+2]`, same count); the 0xa0 request (same count); arm 1 nested, created-arm orders,
// no short cast 247-367; track_definition/current/count declarations moved 247. Diagnostic (volatile reads of
// index and track_index at the creation call, reverted) 323: the frame becomes 0x128 with retail's homes, but the
// creation constant takes EBP where retail holds NONE in EBX and track_definition in EBP; renames move no home.
// Round 128 (m3 lane 6, plain): retail's frame is 16 scalar slots, the two arrays and a 0xa0-byte request with
// nothing shared (0x40 + 0x48 + 0xa0 = 0x128); ours has 17 slots (playback and `active` spilled, no count) and the
// 0x90 request. Retail's count slot lets playback die before the second pitch-range loop, so `active` takes EDI
// there and NONE sits in EBX. Tried: `gains` declared before `sounds` 305 (identical code); one count local read
// before each loop 460 (1939 bytes, frame 0x128, playback in EDI as retail, but the iterator counter spills and
// &sounds[i] takes EDI); that with the gain through the existing decibels_add 481 (1955 bytes, frame 0x12c);
// the 0xa0 request with decibels_add 432 (1939 bytes); all three 481 (1955 bytes, frame 0x13c).
// Round 48 (m3 lane 11, plain): NONE held in EBP across the whole update block is what pushes the playback
// pointer out of EDI. Both pitch-range loops declaring `count = playback->pitch_range_count` in the for init
// 365 (frame 0x128 and retail's EDX/EAX array-initialisation registers, but 20 scalar slots plus the 0x90
// request where retail has 16 plus 0xa0, first difference +0x39); that with the 0xa0 request 368 (frame
// differs again); that with the pitch through a bounds pointer and a named lower bound as in 0x21b940 460
// (1939 bytes); the count in the first loop's init only 427 (1907 bytes); both counts with the gains array
// declared before the sounds 366.
// @retail 0x21b0e0
void function_21b0e0(bool update)
{
	long index = next_sound_datum(LOOPING_SOUNDS, NONE);
	while (index != NONE)
	{
		s_type_5ef569 *sound = (s_type_5ef569 *)LOOPING_SOUNDS->data + (index & 0xffff);
		if (update && sound->update_phase != *((byte *)g_4e6380 + 0x7f) &&
			(!(sound->flags & 2) || sound->playing_sound_count == 0))
		{
			function_21b070(index);
		}
		else
		{
			looping_sound_definition *definition = (looping_sound_definition *)g_4e3b44[sound->definition_index & 0xffff].bytes;
			for (long track_index = 0; track_index < definition->track_count; ++track_index)
			{
				s_type_06b94f *track = (s_type_06b94f *)sound->track_storage + track_index;
				looping_sound_track_definition *track_definition = &definition->tracks[track_index];
				track->completed_count = 0;
				if (sound->state == 2)
					continue;
				if (track->sound_index != NONE)
				{
					s_looping_track_sound *playing = track_sound_get(g_4e637c, track->sound_index);
					s_looping_playback_definition *playback = (s_looping_playback_definition *)g_4e3b44[playing->definition_index & 0xffff].bytes;
					s_looping_sound_class *sound_class = &((s_looping_sound_tables *)g_51ebd4)->classes[playback->sound_class];
					if (!(fabs(sound_class->gain_change_rate) < 0.0001f))
					{
						real step = ((s_looping_sound_system *)g_4e6380)->elapsed_time * sound_class->gain_change_rate;
						approach_126660(&track->gain, sound->source.scale - track->gain, step);
					}
					else
						track->gain = sound->source.scale;
				}
				if (update && track->sound_index != NONE)
				{
					s_looping_track_sound *playing = track_sound_get(g_4e637c, track->sound_index);
					long definition_index = playing->definition_index;
					s_looping_playback_definition *playback = (s_looping_playback_definition *)g_4e3b44[definition_index & 0xffff].bytes;
					s_looping_pitch_modifier *modifier = &((s_looping_sound_tables *)g_51ebd4)->pitch_modifiers[playback->playback_index];
					real pitch = ((real)modifier->pitch_bounds.upper - (real)modifier->pitch_bounds.lower) * track->gain + playing->pitch + (real)modifier->pitch_bounds.lower;
					c_class_219e90 *controller = controller_get(sound->controller_index);
					if (controller->definition_index == NONE)
						random_next(&controller->field_8_4);
					dword seed = controller->field_8_4;
					short previous_pitch_range = playing->pitch_range_index;
					short selected = function_218f50(playback, previous_pitch_range, pitch);
					if (playback->flags & 0x2000)
					{
						long sounds[9] = { NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE };
						long gains[9] = { 0xc2800000, 0xc2800000, 0xc2800000, 0xc2800000, 0xc2800000, 0xc2800000, 0xc2800000, 0xc2800000, 0xc2800000 };
						dword active_mask = 0;
						s_track_sound_iterator iterator;
						long current;
						track_sound_iterator_new(&iterator, track->sound_index);
						while ((current = track_sound_iterator_next(&iterator)) != NONE)
						{
							s_looping_track_sound *entry = track_sound_get(g_4e637c, current);
							if (entry->definition_index == definition_index && entry->state == 2)
								sounds[entry->pitch_range_index] = current;
						}
						for (long i = 0; i < playback->pitch_range_count; ++i)
						{
							s_permutation_set *set = &g_51ebd4->sets[playback->pitch_range_base + i];
							s_looping_pitch_bounds *bounds = &((s_looping_pitch_tables *)g_51ebd4)->bounds[*(short *)((byte *)set + 2)];
							gains[i] = looping_sound_pitch_attenuation(playback, set, pitch);
							real clamped = pitch < (real)bounds->minimum ? (real)bounds->minimum :
								pitch > (real)bounds->maximum ? (real)bounds->maximum : pitch;
							if (clamped == pitch)
								active_mask |= 1 << i;
						}
						for (long i = 0; i < playback->pitch_range_count; ++i)
						{
							dword active = active_mask & (1 << i);
							if (!active && sounds[i] != NONE)
							{
								function_127320(sounds[i], 8);
								sounds[i] = NONE;
							}
							if (active && sounds[i] == NONE)
							{
								long created = function_21b940(index, definition_index, i, (short)track_index, 2, &seed);
								if (created != NONE)
								{
									if (selected == i)
										track->sound_index = created;
									track_sound_get(g_4e637c, created)->track_value = 0xc2800000;
									sounds[i] = created;
								}
							}
							if (sounds[i] != NONE)
							{
								s_looping_track_sound *entry = track_sound_get(g_4e637c, sounds[i]);
								real gain = *(real *)&track_definition->value08 + *(real *)&gains[i];
								entry->track_value = *(dword *)&gain;
								entry->loop.unknown07 = 0xff;
							}
						}
					}
					else if (selected != previous_pitch_range)
					{
						long replacement = looping_sound_find_sound(playing->looping_sound_index, playing->definition_index, selected, (short)track_index);
						if (replacement == NONE)
							replacement = function_21b940(playing->looping_sound_index, playing->definition_index, selected, (short)track_index, 2, &seed);
						if (replacement != NONE)
						{
							track_sound_get(g_4e637c, replacement)->pitch = playing->pitch;
							function_126df0(1, replacement, track->sound_index, 0.75f);
							track->sound_index = replacement;
						}
					}
				}
			}
			if (sound->state != 2 && !(sound->flags & 4))
			{
				for (long i = 0; i < definition->detail_sound_count; ++i)
				{
					s_type_a82d9f *detail = &definition->detail_sounds[i];
					if (detail->sound_definition_index != NONE && sound->detail_sound_times[i] < ((s_looping_sound_system *)g_4e6380)->time &&
						(!(detail->flags & 1) || !(sound->flags & 1)) && (!(detail->flags & 2) || (sound->flags & 1)))
					{
						s_looping_detail_request request;
						*(word *)&request.source = 0;
						if (looping_sound_prepare_detail(detail, index, &request))
						{
							long reason;
							if (function_125f70(detail->sound_definition_index, &request, &reason) != NONE || reason == 1)
							{
								real delay = function_259d0(&g_4e7408->seed, __FILE__, __LINE__, detail->period_lower, detail->period_upper);
								sound->detail_sound_times[i] = (long)((double)(delay * 1000.f) + ((s_looping_sound_system *)g_4e6380)->time);
							}
						}
					}
				}
			}
		}
		long start = index == NONE ? 0 : (index & 0xffff) + 1;
		index = data_datum_index(LOOPING_SOUNDS, data_next_absolute_index_inlined(LOOPING_SOUNDS, start));
	}
}

/* Channel properties consist of a 0x30-byte base and 0x448 bytes of
   effect/impulse state. Retail clears those two parts separately. */
struct s_looping_effect_playback
{
	byte data[0x420];
};

struct s_looping_impulse_parameters
{
	short type;
	short index;
	real lower;
	real upper;
	long identifier;
};

struct s_looping_channel_effects
{
	dword flags;
	short effect;
	byte unknown06[0x18 - 6];
	s_looping_effect_playback playback;
	s_looping_impulse_parameters impulse;
};

struct s_looping_channel_properties
{
	dword flags;
	long format;
	real pitch;
	long gain_bits;
	byte unknown10[0x2c - 0x10];
	long controller_index;
	s_looping_channel_effects effects;
};

struct s_looping_channel_spatialization
{
	byte unknown00[0x14];
	real doppler_scale;
	byte unknown18[0x38 - 0x18];
};

struct s_effect_inputs
{
	real values[4];
	vector3f direction;
};
struct s_sound_playback_effects;
void function_222770(long tag_index, long handle, s_effect_inputs const *inputs, s_sound_playback_effects *effects);
void function_222930(long tag_index, long handle, s_looping_impulse_parameters *parameters);

struct s_channel_class_flags
{
	byte unknown00[0xa];
	word bit0 : 1;
	word bit1 : 1;
	word bit2 : 1;
	word bit3 : 1;
	word unknown_bits : 5;
	word bit9 : 1;
	word remaining : 6;
	byte unknown0c[0x40];
	char mode;
};

static inline void flag_set_12a1b0(dword *flags, dword mask, bool value)
{
	if (value) *flags |= mask;
	else *flags &= ~mask;
}

// @retail 0x12a1b0
void function_12a1b0(short voice_index, s_looping_track_sound *sound, s_looping_channel_spatialization const *spatialization, s_looping_channel_properties *properties)
{
	long definition_index = sound->definition_index;
	s_looping_voice_view *voice = &((s_looping_voice_view *)g_4e6378)[voice_index];
	s_sound_definition *definition = sound_definition_get(definition_index);
	s_channel_class_flags *sound_class = (s_channel_class_flags *)sound_class_definition_get(definition->promotion_index);
	byte *output = (byte *)properties;
	byte const *spatial = (byte const *)spatialization;
	properties->format = (char)definition->unknown03;
	*(short *)(output + 0x10) = definition->promotion_index;
	*(short *)(output + 0x12) = sound->unknowna4;
	*(short *)(output + 0x24) = *(short *)((byte *)voice + 0xe);
	*(long *)(output + 0x14) = *(long *)((byte *)sound_class_definition_get(definition->promotion_index) + 0x10);
	*(long *)(output + 0x18) = *(long const *)(spatial + 8);
	*(long *)(output + 0x28) = *(long const *)(spatial + 4);
	*(long *)(output + 0x1c) = *(long const *)(spatial + 0xc);
	*(long *)(output + 0x20) = *(long const *)(spatial + 0x10);
	bool flag = (bool)(((byte)sound->sound_class >> 3) & 1);
	if (TEST_FIELD_BIT(sound_class->bit3)) properties->effects.flags |= 2;
	else properties->effects.flags &= ~2;
	if (TEST_FIELD_BIT(sound_class->bit1)) properties->effects.flags |= 8;
	else properties->effects.flags &= ~8;
	if (sound_class->mode == 1) properties->effects.flags |= 0x10;
	else properties->effects.flags &= ~0x10;
	flag_set_12a1b0(&properties->effects.flags, 4, TEST_FIELD_BIT(sound_class->bit1) || flag);
	if (TEST_FIELD_BIT(sound_class->bit9)) properties->effects.flags |= 0x40;
	else properties->effects.flags &= ~0x40;
	if (*spatial & 0x10)
	{
		properties->effects.flags |= 0x20;
		memcpy(output + 0x38, spatial + 0x28, 0x10);
	}
	if (*(short *)(output + 0x12) != NONE)
		properties->effects.flags |= 1;
	s_effect_inputs inputs;
	inputs.values[0] = 0.0f;
	inputs.values[1] = (real)((((s_looping_sound_system *)g_4e6380)->time - sound->time) / 1000);
	inputs.values[2] = *(real *)((byte *)sound + 0x1c);
	inputs.values[3] = *(real const *)(spatial + 0x18);
	inputs.direction = *(vector3f const *)(spatial + 0x1c);
	function_222770(definition_index, *(long *)sound->unknown08, &inputs, (s_sound_playback_effects *)&properties->effects.playback);
	function_222930(definition_index, *(long *)sound->unknown08, &properties->effects.impulse);
	*(dword *)(output + 0x48) = sound->field_8_4;
	real scale;
	if (*((byte *)sound + 0x19) & 1)
		scale = (real)(sqrt(1.0f - *(real const *)(spatial + 0x18)) * *(real *)((byte *)sound + 0x1c));
	else
		scale = *(real *)((byte *)sound + 0x1c);
	*(real *)(output + 0x4c) = scale;
}

struct s_looping_driver_channel
{
	byte unknown00[0xd];
	signed char state : 3;
	byte unknown0d : 5;
	byte unknown0e[0x34 - 0xe];
};

struct s_looping_permutation
{
	byte unknown00[4];
	char gain;
	byte unknown05[7];
	short first_chunk;
	short chunk_count;
};

struct s_looping_chunk
{
	byte unknown00[4];
	dword unknown04 : 31;
	dword transition_boundary : 1;
	byte unknown08[4];
};

struct s_looping_chunk_tables
{
	byte unknown00[0x44];
	s_looping_chunk *chunks;
};

struct s_unknown_5c;
struct s_sound_permutation;
struct s_permutation_group;
struct s_bink_sound_settings;
extern s_bink_sound_settings *g_51ebe4;

s_unknown_5c *function_221810(short index);
long function_219290(s_permutation_group *group, short index);
void function_12a420(short voice_index);
void sound_voice_queue_chunk(short voice_index, s_sound_permutation const *permutation, short chunk_index);
void function_12a1b0(short voice_index, s_looping_track_sound *sound, s_looping_channel_spatialization const *spatialization, s_looping_channel_properties *properties);
void function_21f8a0(long channel_index, s_looping_channel_properties const *properties, s_looping_effect_playback const *effects);
void __stdcall function_21f720(s_looping_impulse_parameters const *parameters);
void function_21fa80(long channel_index, s_looping_channel_properties const *properties, s_looping_effect_playback const *effects, bool force);
// score 335 (was 335): the frame is 0x4b4 where retail's is 0x4bc, and the stack slots are not
// a shifted copy of each other: ours has homes at 0x74, 0x78 and 0x94 that retail does not use,
// and retail has 0x64, 0x80, 0x84 and 0x9c that ours does not, so retail carries two more dword
// locals in a different order and the gap cannot be closed by padding;
// retail reads the track value and the source scale before the clear, the one-fill clear and the
// inlined 0x221810 (BLOCKED-BY 0x221810, another file) and the inlined 0x12a420 at the
// stop_channel site remain.
// score 335 (was 335): frame 0x4b4 versus 0x4bc (retail reads the track value and the source scale before the
// clear), the one-fill clear, 0x221810 inlined (BLOCKED-BY 0x221810) and 0x12a420 inlined at the stop_channel site remain.

// Earlier: frame 0x4b4 versus 0x4bc, entry snapshots and the class lookup expansion still differ.
// Full-width permutation selection keeps its values; both index widths 529 (three-point tail dropped),
// chunk width alone 560, state width 550, frame names/property declaration 550; existing next-chunk
// helper with short/long/wide selection 556/568/536; direct gain fields 526 (six-point tail dropped),
// direct modifier fields 529 (three-point tail dropped), direct voice index/shared lower pitch 532.
// Round 179: whole clear 511; two whole subobject clears 527; zero aggregate 476; branch chunk 532;
// zero-first Doppler 526; whole clear/branch 471; wide branch 532; split clear/both results 527;
// aggregate with chunk/Doppler/both 525/517/471; aggregate direct gain/modifier/both 510/479/510.
// Whole clear/branch with direct gain fields 447 kept; modifier/both 468/447. On 447, zero-first
// Doppler 471; wide chunk 443 (four-point tail dropped); direct sound index/lower pitch 447/447.
// Round 235: last-chunk char/short/long/dword 532/531/533/533; each with one exit
// 532/531/533/533. One-point short result restored; missing frame bytes remain.
// Round 185: Primary-track truth byte/signed-byte/short/long 532/532/540/544; with complete native
// modifier copy 597/597/609/611. Original Boolean predicate, modifier receiver, scalar observations,
// helpers and complete body restored.
// J185: native long old synchronization count 414 fast/plain kept; branch permutation/seed/both 447/447/447;
// each with long count 414, all with short count 467. On 414, short selection/wide chunk/both 481/457/490;
// direct controller observations alone/with selection/with chunk/with both 663/680/663/691.
// The existing count snapshot remains before decrement; all extra producer, receiver and width forms restored.
// Round 189: base flag clear 527; effect flag clear 523; both flag clears 534; inner gain carrier 414; post-
// decrement count 414; headers plus gain 534; headers plus count 534; all producers 534; consumer-owned pitch
// 498; controller property arms 414; pitch plus property arms 498.
// Original body retained; frame 0x4b4 versus 0x4bc, initial scalar homes and class-query expansion remain.
// Round 421: continuation before terminal stop 345 (plain kept), 1936/1929 bytes; on it,
// short permutation/long chunk/both 450/422/461; effect receiver alone/with those
// 345/450/422/461. Original widths, effects and call tuples retained.
// Lane 17 round 2: retail clears the properties as two aggregates (first dword stored, then 11 dwords; the
// effects' first dword, then 0x111 dwords): the base fields grouped in a nested struct with `= {{0}, {0}}`
// gives exactly that clear but 503 (1955 bytes); `= {0}` on the flat struct 478; the pitch bounds read
// through a bounds pointer with a named lower bound 345 (retail's `lea [ecx+edx*4+8]`); both 503; early
// track-value/scale snapshots (diagnostic) 501; properties declared at entry 345; total gain as successive
// statements 345. Retail also calls 0x221810 out of line where ours inlines it (its file is another lane's),
// and snapshots the track value and the source scale before the clear (frame 0x4bc).
// Lane 13 round 24 (previous note 345, was 414): the gain parameter updated in place and passed as bits 335 (kept: one
// slot seeded from the argument, as retail shows); both sums through decibels_add 505; the parameter typed long
// (0x1293f0 passing its bits) 335 (same code). On 335: the base fields as an unnamed nested struct with `= {{0}, {0}}`
// (retail's two fills exactly) 497; with the bounds pointer 497; plus early track-value/scale locals (diagnostic) 516
// (retail's frame 0x4bc); the bounds pointer alone 335; the initialized-flag test through the flags word 335.
// Lane 5 round 113 (previous note 335, was 345; fast, agrees with plain): the nested base struct with `= {{0}, {0}}`
// 497; the playback lookups through the existing sound_definition_get 335 (same code); the stop flag through
// word_flags_set at the state-else site 419, at both sites 419; the gain sum above the clear 333 (under the keep bar),
// with the nested clear 498.
// @retail 0x21c400
void function_21c400(short voice_index, real gain, s_looping_channel_spatialization const *spatialization)
{
	s_looping_voice_view *voice = &((s_looping_voice_view *)g_4e6378)[voice_index];
	long sound_index = voice->sound_index;
	s_looping_track_sound *playing = track_sound_get(g_4e637c, sound_index);
	s_looping_playback_definition *playback = (s_looping_playback_definition *)g_4e3b44[playing->definition_index & 0xffff].bytes;
	s_permutation_set *arg_58ecd0 = &g_51ebd4->sets[playback->pitch_range_base + playing->pitch_range_index];
	s_type_5ef569 *sound = (s_type_5ef569 *)LOOPING_SOUNDS->data + (playing->looping_sound_index & 0xffff);
	s_type_06b94f *track = (s_type_06b94f *)sound->track_storage + playing->loop.track_index;
	looping_sound_definition *definition = (looping_sound_definition *)g_4e3b44[sound->definition_index & 0xffff].bytes;
	looping_sound_track_definition *track_definition = &definition->tracks[playing->loop.track_index];
	c_class_219e90 *controller = controller_get(sound->controller_index);
	s_looping_pitch_modifier *modifier = &((s_looping_sound_tables *)g_51ebd4)->pitch_modifiers[playback->playback_index];
	real pitch = ((real)modifier->pitch_bounds.upper - (real)modifier->pitch_bounds.lower) * track->gain + playing->pitch + (real)modifier->pitch_bounds.lower;
	bool primary = track->sound_index == sound_index;
	s_looping_channel_properties properties;
	memset(&properties, 0, sizeof(properties));
	gain = *(real *)&playing->source.unknown08 + (*(real *)&playing->track_value + gain);
	properties.gain_bits = function_1251e0(playback, *(long *)&gain, playing->source.scale);
	properties.effects.effect = *(short *)((byte *)track_definition + 0x3c);
	if (properties.effects.effect)
		properties.effects.flags |= 1;
	properties.controller_index = definition->flags & 0x20 ? sound->controller_index : NONE;
	s_looping_pitch_bounds *bounds = &((s_looping_pitch_tables *)g_51ebd4)->bounds[*(short *)((byte *)arg_58ecd0 + 2)];
	properties.pitch = pitch - (real)bounds->unknown00;
	if (playing->sound_class & 2)
	{
		byte const *promotion = (byte const *)function_221810((char)playback->unknown02[0]);
		real doppler;
		if (sound->controller_index != NONE)
		{
			c_class_219e90 *source_controller = controller_get(sound->controller_index);
			doppler = source_controller->definition_index != NONE ? source_controller->listener_values[playing->listener_index] : 0.f;
		}
		else
			doppler = function_12a9d0(playing->listener_index, &playing->source.spatial);
		properties.pitch += *(real const *)(promotion + 0x48) * spatialization->doppler_scale * doppler;
	}
	function_12a1b0(voice_index, playing, spatialization, &properties);
	if (!TEST_FIELD_BIT(playing->channel_initialized))
	{
		s_looping_permutation *permutation = &((s_looping_permutation *)g_51ebd4->chances)[arg_58ecd0->first_index + playing->permutation_index];
		*(real *)&properties.gain_bits += (real)permutation->gain;
		function_21f8a0(voice->channel_index, &properties, &properties.effects.playback);
		function_21f720(&properties.effects.impulse);
		sound_voice_queue_chunk(voice_index, (s_sound_permutation const *)permutation, playing->current_chunk_index);
		if (definition->flags & 0x20)
		{
			long synch_count = NONE;
			if (controller->definition_index != NONE)
			{
				synch_count = controller->synch_count;
				--controller->synch_count;
				controller->synch_playback = false;
			}
			playing->loop.synch_state = synch_count;
		}
		playing->channel_initialized = true;
	}
	else
	{
		if (playing->state == 2 || playing->state == 3 || playing->state == 1)
		{
			short channel = ((s_looping_voice_view *)g_4e6378)[playing->channel_index].channel_index;
			short channel_state = ((s_looping_driver_channel *)g_51ebe4)[channel].state;
			if (!TEST_FIELD_BIT(playing->transition_pending) &&
				(channel_state != 3 || (byte)looping_sound_track_has_next_sound(track, track_definition)))
			{
				dword *seed = &g_4e7408->seed;
				if (definition->flags & 0x30)
					seed = primary ? &track->field_8_4 : &playing->loop.field_8_4;
				long permutation_index = playing->permutation_index;
				s_looping_playback_definition *current = (s_looping_playback_definition *)g_4e3b44[playing->definition_index & 0xffff].bytes;
				s_permutation_set *current_range = &g_51ebd4->sets[current->pitch_range_base + playing->pitch_range_index];
				s_looping_permutation *permutation = &((s_looping_permutation *)g_51ebd4->chances)[current_range->first_index + playing->permutation_index];
				short next_chunk;
				if (permutation && playing->current_chunk_index < permutation->chunk_count - 1)
					next_chunk = playing->current_chunk_index + 1;
				else
					next_chunk = NONE;
				bool last_chunk = next_chunk == NONE;
				if (!(playing->loop.flags & 1) && primary && (byte)looping_sound_track_has_next_sound(track, track_definition))
				{
					if (last_chunk || (looping_sound_track_transition_requires_stop(track) &&
						TEST_FIELD_BIT(((s_looping_chunk_tables *)g_51ebd4)->chunks[permutation->first_chunk + playing->current_chunk_index].transition_boundary)))
					{
						if (function_21c190(sound, track, voice->sound_index, seed))
							return;
						current = (s_looping_playback_definition *)g_4e3b44[playing->definition_index & 0xffff].bytes;
						arg_58ecd0 = &g_51ebd4->sets[current->pitch_range_base + playing->pitch_range_index];
						playing->transition_pending = true;
						goto prepare_chunk;
					}
				}
				else if (last_chunk)
				{
					if (!(definition->flags & 2))
					{
						dword permutations = primary ? looping_sound_get_permutations(playback, sound, definition, playing->loop.track_index, playing->pitch_range_index, false) : playing->loop.permutations;
						permutation_index = function_219110((s_set_ref *)playback, playing->pitch_range_index, &permutations, playing->permutation_index, seed, NULL, false);
						if (primary)
							looping_sound_set_permutations(sound, definition, playback, playing->loop.track_index, playing->pitch_range_index, permutations, permutation_index);
						else
							playing->loop.permutations = permutations;
						next_chunk = 0;
					}
					else
					{
						playing->state = 4;
						if (primary)
							sound->flags |= 8;
						goto stop_channel;
					}
				}
				if (permutation_index != NONE)
				{
					sound_playback_set_chunk(voice->sound_index, playing->definition_index, playing->pitch_range_index, (short)permutation_index, next_chunk);
					playing->transition_pending = true;
				}
				else
				{
stop_channel:
					playing->stop_requested = true;
					function_12a420(voice_index);
				}
			}
prepare_chunk:
			if (TEST_FIELD_BIT(playing->transition_pending))
			{
				s_looping_permutation *permutation = &((s_looping_permutation *)g_51ebd4->chances)[arg_58ecd0->first_index + playing->permutation_index];
				if ((byte)function_125e60(track_sound_get(g_4e637c, voice->sound_index)) && channel_state != 3)
				{
					sound_voice_queue_chunk(voice_index, (s_sound_permutation const *)permutation, playing->current_chunk_index);
					playing->transition_pending = false;
					if (primary && !(byte)looping_sound_track_has_next_sound(track, track_definition) &&
						(short)function_219290((s_permutation_group *)permutation, playing->current_chunk_index) == NONE)
					{
						if (playing->state == 1)
							playing->state = 2;
						else if (playing->state == 3)
							playing->state = 4;
					}
				}
			}
		}
		else
		{
			playing->stop_requested = true;
			function_12a420(voice_index);
		}
		s_looping_permutation *permutation = &((s_looping_permutation *)g_51ebd4->chances)[arg_58ecd0->first_index + playing->permutation_index];
		*(real *)&properties.gain_bits += (real)permutation->gain;
		function_21fa80(((s_looping_voice_view *)g_4e6378)[voice_index].channel_index, &properties, &properties.effects.playback, false);
	}
	if (primary)
	{
		playing->loop.field_8_4 = track->field_8_4;
		playing->loop.permutations = track->permutations;
	}
}

struct s_sound_location_source;
void sound_source_get_position(s_sound_location_source const *source, long listener_index, point3f *position);
real function_192d70(real minimum, real maximum, real distance);
point3f *function_142700(transform4x3f const *matrix, point3f const *point, point3f *out);
real function_30bf0(vector3f *vector);
void function_192ce0(point3f const *direction, real *gains, bool linear);
// score 615 (was 615): the entry and the homes through the creation call still differ, the gains
// clear uses retail's store form (retail stores [ebp+0xc4..] from eax), the energy loop keeps its
// padding jump in retail, the track loop holds the state in EBX and the track in EBP, and the
// impulse bits are still tested twice unmerged with the field_c_4 = 0 reload.
// Same causes, as an earlier note put them: entry and homes retail's through the creation call; the gains
// clear (memset's lea form, retail stores [ebp+0xc4..] from eax), the energy loop's padding (jmp + 8 bytes in
// retail), state held
// in EBX and track (not track_definition/i) in EBP in the track loop, the impulse bits tested twice unmerged and
// the field_c_4 = 0 reload still differ.
// Round 166 (kept): single exit with `else result = true` last, lookup_identifier in place of reassigning
// identifier, creation as `else if ((1 << state) & 3) {...; result = index != NONE; }` then `if (index != NONE)`,
// release exits as nested if/else, second gain loop rolled, bool track_alternate, TEST_FIELD_BIT shift tests
// for flags bits 1/2: 723 -> 709. With a `goto done` for the not-allowed path and `else result = false` 614 (a goto
// is kept only with a match); not-allowed early `return result` 720 (inline `mov al, bl` exit); `if (index == NONE)
// result = false` in the creation arm 711; `else if (created) result = false` 625 (flag tested afterwards, not
// kept); creation-failure `return false` 761 (zero constant back in EBX).
// Tried: combine_sources renamed 744f (no change); SET_FLAG ternaries and a chained
// `next_state = field_c_4 = 0` identical; a `= 0.f` clear loop 714f (xorps); track before track_definition 645;
// impulse bits from the field at each use 646 (retail's double tests, but computed at each use); bool-typed
// alternate-state comparisons 679 (retail's setne/cmp form, 4 fewer instructions shift the tail); the pitch
// modifier's lower/upper as a {lower, upper} member at +8 read through a pointer 614 (retail's `lea [esi+eax*4+8]`
// at all four pitch sites; 0x21b940 77 -> 74, 0x21b0e0/0x21c400 unchanged; under the keep bar); track definition
// read at each use 807; `!TEST_FIELD_BIT(flags & 1)` start test 614.
// frame 0x3c agrees; definition/identifier, result and alternate homes still differ.

// Earlier: frame 0x3c and retail length recovered by the availability guard join;
// definition/identifier carriers and flag/result lifetimes still differ.
// The fixed four-entry energy traversal is rolled; both gain loops rolled 795, listener loop alone 798,
// entry-result region 788, full result/single exit 792; direct impulse fields/shared setter/field setter
// 781/776/781; derived lookup key in either form 775, key with energy loop 770, entry with energy 772.
// Round 178: partial definition/availability guard joins 751/748; native track/playback/
// both cursors 799/754/779; canonical scalar names 759; readonly tag 759; definition
// join with both cursors 781. On the availability join: creation arms 773, derived key
// 763, playback cursor 771, creation/key 772. Availability-only join kept at plain 748;
// the five-point playback-only gain is below the retention gate.
// Earlier: frame 0x38 versus 0x3c, definition/identifier carriers, later flag and result lifetimes differed.
// Round 398: full-width result/combine/created storage, seven subsets
// 776/775/775/772/775/774/792; all three as bytes 777. Complete original result
// producers, widths, observations, loop bodies and exits restored.
// J185: per-track signed byte/short/long/dword 748/723/763/763; corresponding outer+track widths 748/756/773/773.
// Signed short per-track truth kept at full 723. On it, word result/combine/created subsets 773/768/737/769/738/728/770;
// complete word truth carriers 778. Original result/combine/created/outer widths, all producers, calls and loops restored.
// Round 189: Entry-only true exit join 764; delayed initial state result 766; fallback-only state
// result 764 (loses a caller); joined result-home names 764. Existing guarded/direct old-end fade
// helper 824/722; with entry join 815/764. One-point gain rejected; original body restored.
// Lane 13 round 24 (previous note 709, was 723): the not-allowed path as one `goto done` to the single exit with `else
// result = false` after the creation 615 (kept; one goto to a single exit is allowed now). On it: the replacement pitch
// bounds through a bounds pointer 615; track before track definition 615; both 615; the loop index named track_index
// 615; the impulse bits through flag_set_12a1b0 615.
// Lane 5 round 113 (previous note 615, was 709; plain, --fast is off here): distance declared before maximum 615
// (retail swaps their dead-parameter homes 0x54/0x60); impulse states from `(byte)flags & 1` 699; memset on
// &listener_gains 615; the seed as a ternary 653 (aligned 221 against 249, NONE moves to ESI as retail);
// next_definition/local_2e47c8 at the loop top 698.
// Lane 18 round 17 (an LTCG link of this file's generated unit, 86 objects, equal to the full listing; 611 there for
// 615): the gains cleared by four `= 0.f` stores 641 at 2516 bytes (retail's direct [ebp+0xc4] stores, but from
// xorps/movss where retail stores eax = 0 as integers, so the clear is not a float store); memset with
// `4 * sizeof(real)` 611 (identical); `(definition->flags & 0x80) != 0` 611; `created = true` before the creation
// call 611; re-measured: memset of &listener_gains[0] 611 (listed above as memset on &listener_gains).
// @retail 0x21a250
bool function_21a250(long definition_index, dword sound_flags, long identifier, long controller_definition_index,
	s_type_99c531 *source, long state, dword flags, real fade_duration)
{
	bool result = state == 2;
	if (definition_index != NONE)
	{
		bool alternate = (flags & 1) != 0;
		if (looping_sound_system_available())
		{
			looping_sound_definition *definition = (looping_sound_definition *)g_4e3b44[definition_index & 0xffff].bytes;
			bool combine_sources = source->audible != 0 && (definition->flags & 0x80);
			long lookup_identifier = combine_sources ? definition_index | 0x6000 : identifier;
			long index = function_21cf80(definition_index, lookup_identifier);
			bool created = false;
			result = true;
			if (index != NONE)
			{
				if ((flags & 0x10) && state == 0)
					state = 1;
			}
			else if ((1 << state) & 3)
			{
				index = function_21aeb0(definition_index, (short)sound_flags, lookup_identifier, controller_definition_index);
				created = true;
			}
			else
				goto done;
			if (index != NONE)
			{
				s_type_5ef569 *sound = (s_type_5ef569 *)LOOPING_SOUNDS->data + (index & 0xffff);
				sound->sound_class = (short)sound_flags;
				if (!TEST_FIELD_BIT(((s_looping_source_flags *)source)->flag1))
					sound->source = *source;
				function_21d060(definition_index, &sound->source);
				if (combine_sources)
				{
					if (sound->update_phase != *((byte *)g_4e6380 + 0x7f))
						memset(sound->listener_gains, 0, sizeof(sound->listener_gains));
					s_looping_source_tail *tail = (s_looping_source_tail *)sound->source.unknown30;
					real maximum = TEST_FIELD_BIT(((s_looping_source_flags *)&sound->source)->flag3) ? tail->value38 : definition->maximum_distance;
					real distance;
					long listener = function_127d00(&sound->source, maximum, &distance);
					if (listener != NONE)
					{
						real minimum = TEST_FIELD_BIT(((s_looping_source_flags *)&sound->source)->flag2) ? tail->value34 : definition->minimum_distance;
						minimum = minimum > 0.001f ? minimum : 0.001f;
						real gain = function_192d70(minimum, maximum, distance);
						point3f position;
						sound_source_get_position((s_sound_location_source const *)&sound->source, listener, &position);
						if (sound->source.audible == 1)
						{
							transform4x3f const *matrix = (transform4x3f const *)((byte *)g_4e6380 + 0x90 + listener * 0x48);
							function_142700(matrix, &position, &position);
						}
						if (distance > minimum)
							function_30bf0((vector3f *)&position);
						real gains[4];
						function_192ce0(&position, gains, true);
						real scale = 1.f;
						real total = 0.f;
						long gain_index = 0;
						do
						{
							real value = gains[gain_index] * gain;
							total += value * value;
							++gain_index;
						} while (gain_index < 4);
						if (total > 1.f)
							scale = 1.f / total;
						gain_index = 0;
						do
						{
							sound->listener_gains[gain_index] += gains[gain_index] * scale * gain;
							++gain_index;
						} while (gain_index < 4);
					}
					sound->sound_class &= ~2;
				}
				sound->update_phase = *((byte *)g_4e6380 + 0x7f);
				if ((state == 2 || (sound->flags & 8)) && sound->playing_sound_count == 0)
					function_21b070(index);
				else
				{
					result = false;
					for (long i = 0; i < definition->track_count; ++i)
					{
						looping_sound_track_definition *track_definition = &definition->tracks[i];
						s_type_06b94f *track = (s_type_06b94f *)sound->track_storage + i;
						dword *seed = &g_4e7408->seed;
						if (definition->flags & 0x30)
							seed = &track->field_8_4;
						bool track_alternate = alternate;
						if (track_definition->alternate_start_sound_index == NONE && track_definition->alternate_loop_sound_index == NONE &&
							track_definition->alternate_end_sound_index == NONE && track_definition->alternate_transition_sound_index == NONE &&
							!(track_definition->flags & 0x14))
							track_alternate = false;
						if (created)
						{
							track->gain = sound->source.scale;
							track->completed_count = 0;
						}
						if (created || track->sound_index == NONE)
						{
							track->sound_index = NONE;
							track->field_c_4 = track_alternate ? 3 : 0;
							track->next_state = track->field_c_4;
						}
						if (state == 0 && track_definition->start_sound_index != NONE)
						{
							long old_end = looping_sound_find_sound(index, track_definition->end_sound_index, NONE, (short)i);
							if (!(track_definition->flags & 1))
								track->sound_index = function_21b940(index, track_definition->start_sound_index, NONE, (short)i, 1, seed);
							else
							{
								function_21b940(index, track_definition->start_sound_index, NONE, (short)i, 0, seed);
								if (old_end != NONE)
								{
									real duration = track_definition->fade_in_duration;
									s_looping_track_sound *playing = track_sound_get(g_4e637c, old_end);
									playing->fade_gain_bits = function_12a810(old_end);
									playing->fade_curve = 1;
									playing->fade_start_time = (long)(duration * 1000.f);
									playing->fade_end_time = NONE;
									playing->looping = true;
								}
								looping_sound_fade_in(track->sound_index, 1, track_definition->fade_in_duration);
							}
							track->field_c_4 = 0;
							track->next_state = track->field_c_4;
						}
						if (state != 2 && !(sound->flags & 8))
						{
							long next_definition = NONE;
							long local_2e47c8 = NONE;
							dword impulse_states = track_definition->flags & 1;
							bool impulse_start = TEST_FIELD_BIT((track_definition->flags >> 1) & 1);
							bool impulse_alternate = TEST_FIELD_BIT((track_definition->flags >> 2) & 1);
							if (impulse_start)
								impulse_states |= 4;
							else
								impulse_states &= ~4;
							if (impulse_start)
								impulse_states |= 0x40;
							else
								impulse_states &= ~0x40;
							if (impulse_alternate)
								impulse_states |= 8;
							else
								impulse_states &= ~8;
							if (impulse_alternate)
								impulse_states |= 0x20;
							else
								impulse_states &= ~0x20;
							bool current_alternate = ((1 << track->field_c_4) & 0x58) != 0;
							if (track->field_c_4 == track->next_state ||
								*(long *)((byte *)track_definition + g_44a188[track->next_state] + 4) == NONE || track_alternate != current_alternate)
							{
								long next = track_alternate ? g_44a16c[track->field_c_4] : g_44a150[track->field_c_4];
								long previous = track->field_c_4;
								while (next != previous)
								{
									long offset = g_44a188[next];
									byte *reference = offset == NONE ? NULL : (byte *)track_definition + offset;
									if (reference && *(long *)(reference + 4) != NONE)
									{
										if (!(impulse_states & (1 << next)))
										{
											next_definition = *(long *)(reference + 4);
											break;
										}
										local_2e47c8 = *(long *)(reference + 4);
									}
									previous = next;
									next = track_alternate ? g_44a16c[next] : g_44a150[next];
								}
								track->next_state = (char)next;
								if (local_2e47c8 != NONE)
									function_21b940(index, local_2e47c8, NONE, (short)i, 0, seed);
							}
							if (next_definition == NONE && track_alternate == (((1 << track->field_c_4) & 0x58) != 0))
								continue;
							bool start_with_fade = state == 0 && (track_definition->flags & 1);
							if (track->sound_index != NONE && !start_with_fade)
							{
								s_looping_track_sound *playing = track_sound_get(g_4e637c, track->sound_index);
								if (playing->definition_index == next_definition ||
									track_alternate == (((1 << track->field_c_4) & 0x58) != 0) || !(track_definition->flags & 4))
									continue;
								long replacement = NONE;
								if (next_definition != NONE)
								{
									replacement = function_21b910(index, next_definition, NONE, (short)i, 2, seed);
									if (replacement == NONE)
										continue;
								}
								function_126df0(1, replacement, track->sound_index, track_definition->crossfade_duration);
								track->sound_index = replacement;
								track->field_c_4 = track_alternate ? 4 : 1;
								track->next_state = track->field_c_4;
							}
							else if (next_definition != NONE)
							{
								s_looping_playback_definition *playback = (s_looping_playback_definition *)g_4e3b44[next_definition & 0xffff].bytes;
								s_looping_sound_tables *tables = (s_looping_sound_tables *)g_51ebd4;
								s_looping_pitch_modifier *modifier = &tables->pitch_modifiers[playback->playback_index];
								real pitch = ((real)modifier->pitch_bounds.upper - (real)modifier->pitch_bounds.lower) * track->gain + (real)tables->classes[playback->sound_class].pitch + (real)modifier->pitch_bounds.lower;
								short arg_58ecd0 = function_218f50(playback, NONE, pitch);
								long replacement = looping_sound_find_sound(index, next_definition, arg_58ecd0, (short)i);
								if (replacement == NONE)
									replacement = function_21b940(index, next_definition, arg_58ecd0, (short)i, 2, seed);
								if (replacement != NONE)
								{
									if (state != 0)
										function_126df0(1, replacement, NONE, 0.5f);
									else if (track_definition->flags & 1)
										function_126df0(1, replacement, NONE, track_definition->fade_in_duration);
									track->sound_index = replacement;
									track->field_c_4 = track->next_state;
								}
							}
						}
						else if (sound->state != 2)
						{
							if (fade_duration != 0.f)
							{
								if (track->sound_index != NONE)
									looping_sound_fade_out(fade_duration, track->sound_index);
								continue;
							}
							dword current_alternate = (1 << track->field_c_4) & 0x58;
							bool alternate_end;
							long end_definition;
							bool impulse_end;
							if (current_alternate && ((track_definition->flags & 0x10) || track_definition->alternate_end_sound_index != NONE))
							{
								end_definition = track_definition->alternate_end_sound_index;
								alternate_end = true;
								impulse_end = TEST_FIELD_BIT((track_definition->flags >> 4) & 1);
							}
							else
							{
								end_definition = track_definition->end_sound_index;
								alternate_end = false;
								impulse_end = TEST_FIELD_BIT((track_definition->flags >> 1) & 1);
							}
							if (track->sound_index != NONE)
							{
								if (!impulse_end && end_definition == NONE && (definition->flags & 2))
									continue;
								if (impulse_end || end_definition == NONE)
								{
									real duration = current_alternate && (track_definition->flags & 0x10) ?
										track_definition->alternate_fade_out_duration : track_definition->fade_out_duration;
									looping_sound_fade_out(duration, track->sound_index);
								}
							}
							if (end_definition != NONE)
							{
								if (impulse_end)
									function_21b940(index, end_definition, NONE, (short)i, 0, seed);
								else if (track->sound_index != NONE)
								{
									s_looping_track_sound *playing = track_sound_get(g_4e637c, track->sound_index);
									if (playing->channel_index != NONE)
									{
										track->next_state = alternate_end ? 6 : 2;
										playing->state = 3;
									}
								}
							}
						}
					}
					if (sound->playing_sound_count == 0 && function_127d00(&sound->source, TEST_FIELD_BIT(((s_looping_source_flags *)&sound->source)->flag3) ?
						((s_looping_source_tail *)sound->source.unknown30)->value38 :
						((looping_sound_definition *)g_4e3b44[sound->definition_index & 0xffff].bytes)->maximum_distance, NULL) == NONE)
						function_21b070(index);
					else
					{
						if (alternate)
							sound->flags |= 1;
						else
							sound->flags &= ~1;
						if (TEST_FIELD_BIT((flags >> 1) & 1))
							sound->flags |= 2;
						else
							sound->flags &= ~2;
						if (TEST_FIELD_BIT((flags >> 2) & 1))
							sound->flags |= 4;
						else
							sound->flags &= ~4;
						sound->state = (byte)state;
					}
				}
			}
			else
				result = false;
		}
	}
	else
		result = true;
done:
	return result;
}
