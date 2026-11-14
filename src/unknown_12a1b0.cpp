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

// MiniMax H (11-12): no new form, no lower score (93); 0x11bed0 stays expanded in every form tried, so retail's call is not caused by our spelling of it.
// score 93 (was 93): Called location-query boundary and native datum/element homes remain; restore the complete opening executable body.
// MiniMax A (11-11): no new form, no lower score (93); the location query (0x11bed0) is still expanded here while
// retail calls it out of line, and retail still packs the iterator handle as an index and a datum in one word and
// scans its bit vector in 32-bit words, where ours keeps the two in separate values; the call boundary and the
// helper's inline eligibility are fixed in src/unknown_11bed0.cpp, which this lane may not edit, so the body and
// the register homes that follow the call stand. BLOCKED-BY 0x11bed0.
// MiniMax C (11-09): no new form, no lower score (93); the location query is still expanded here and the packed datum walk is still missing.
// MiniMax S (11-06): no new form, no lower score: retail still packs the iterator handle as an index and a datum in one word and scans its bit vector in 32-bit words; ours keeps the two in separate values, and the location query's call boundary still differs (its owner is outside this lane).
// MiniMax E (11-11): no new form tried, no lower score (93): retail calls the location query 0x11bed0
// out of line while this build expands it, and retail packs the iterator handle as an index and a datum
// in one word and scans its bit vector in 32-bit words where ours keeps the two in separate values; the
// call boundary and the helper's inline eligibility are fixed in src/unknown_11bed0.cpp, which this lane
// may not edit, so the body stands. BLOCKED-BY 0x11bed0.
// score 93 (was 93): Called location-query boundary and native datum/element homes remain; restore the complete opening executable body.
// MiniMax S (11-06): no new form, no lower score: retail still packs the iterator handle as an index and a datum in one word and scans its bit vector in 32-bit words; ours keeps the two in separate values, and the location query's call boundary still differs (its owner is outside this lane).
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
// Round 142 (fast, control 93): the call in an inline helper taking the source 93, on the index walk 160; on the walk,
// switch on the source type with one or three cases 162/162; inline bool predicate, also through a bool local 161/161;
// inline getter returning the spatial pointer or NULL, ternary or if/else 163/163; `source && source->audible == 1` 162;
// `sound && ...` 161; `audible != 0 &&` and the same nested 167/167; `&& sound` 161; `&& data` 161; `&& sound && data` a byte match
// like round 166's order (implied tests, not kept); named location and position pointers both tested 56 at 217 bytes (not kept).
// One extra test never moves the call; two do. 0x11bed0 calling the existing 0x11be90 for its leaf half stays a byte match itself
// but leaves this at 93 (walk 160), so the size of 0x11bed0's source is not the cause; not kept.
// Round 111: location pointer, const position pointer, both pointers, and mutable position pointer 93 each;
// a pool local on the native iterator 93; the iterator's pool assigned last 93; guarded native do/while and
// its early-empty-return form 93 each. One-use receiver locals restored unmatched; the call still expands.
// MiniMax C (11-05): no new form, no lower score: the location call is still expanded here; the walk still
// keeps the iterator struct where retail keeps a packed datum index (that datum form is 127).
// Lane 13 round 66 (fast; aligned in brackets, control 93 [101]): index walk with a pool local and the existing
// datum_get_inlined 191 [76] (300 bytes); with a named source pointer 160 [51]; as a plain `for` with the advance in
// its header 161 [53]; on LOOPING_SOUNDS without a pool local 161 [53]; the native iterator as a `while` with the
// advance at the bottom 93 (same code). 0x11bed0 stays expanded in every form.
// Round 222 (m3 lane 3, fast): retail's walk is the real next-datum function (0x16bbc0) at both sites, inlined, with its
// 0x16bc00 call inlined only at the latch; our build keeps 0x16bbc0 out of line: both sites through record_pool_next_used as
// a `for` or a `while` 139/139 (155 bytes); it first and the inline latch 118 (249 bytes); next_sound_datum first and it at
// the latch 116 (179 bytes); the native iterator with both steps through record_pool_iterator_step 145 (151 bytes). 0x11bed0
// stays inlined in all five. Lead: 0x16bbc0 and 0x16bc00 sit in an /Ob1 file our LTCG never inlines from, 0x11bed0 in an
// /O2 file it always inlines from, the reverse of retail (build flags are the user's decision).
// Round 98 (m3 lane 16, fast; aligned count in brackets, control 93 [61]): an `inline` redeclaration of 0x11bed0 in this
// file 93 (same code, still expanded); the index walk through record_pool_next_used at both sites 139 [62] (155 bytes,
// confirmed), with a `__forceinline` or an `inline` redeclaration of it in this file 139/139 (it stays a call: a
// redeclaration here does not make another file's definition expandable); on the native iterator the audible test as
// a switch with a `case 1` and an empty default 99 [63] (233 bytes), the test and call in a `do { } while (false)`
// with a break 93 (same code). 0x11bed0 stays expanded in all six. BLOCKED-BY 0x11bed0's inline eligibility.
// MiniMax B (11-11): no new form, no lower score (93); the call to 0x11bed0 is still expanded here while retail
// keeps it out of line, and that callee is defined in src/unknown_11bed0.cpp; retail also packs the walk's index
// and its value into one register, where this build keeps the iterator struct.
// Round 113 (m3 lane 9, fast with 0x11bed0, 0x11be90, 0x14a280, 0x1d48f0, 0x125920, 0x1e1500, 0x1697c0, 0x17c0e0;
// src/unknown_11bed0.cpp was open this round, so callee forms): our build expands 0x11bed0 here and in 0x1d48f0, retail
// calls it from both. 0x11bed0 with NONE constants in its no-bsp branch 93; reading the stored leaf back 93; the no-bsp
// branch storing the leaf first 93; the leaf local declared first with a NONE default 93; an early return 93 (all the
// same code, 0x11bed0 still matched); its arms swapped 113 (0x11bed0 22); the cluster as a NONE default plus `if` 92
// (0x11bed0 25, 0x1697c0 591); one call of the existing 0x11be90 with a leaf ternary 90 (0x11bed0 35, 0x1697c0 575);
// the cluster read before the leaf store 93 (0x11bed0 14); the cluster/bsp stores chained 93 (0x11bed0 2). 0x11bed0
// stays expanded (227 bytes) in all ten, so its source spelling is not what makes retail call it.
// MiniMax G (11-13): no new form, no lower score (93): the location query is still expanded here while retail
// calls it out of line, and that callee's inline eligibility is fixed in another file, so the call boundary
// and the register homes that follow it stand. BLOCKED-BY 0x11bed0.
// score 93 (was 93): unchanged this round: the location query is still expanded here while retail calls it out of
// line, and that callee's inline eligibility is fixed in another file. BLOCKED-BY 0x11bed0.
// score 93 (was 93): unchanged this round: the location query 0x11bed0 is still expanded where retail calls it
// out of line, and its inline eligibility is fixed in src/unknown_11bed0.cpp, which this lane may not edit.
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

dword function_2194c0(s_animation_ref *ref, s_packed_value *value);
void function_219560(s_packed_value *value, s_animation_ref *ref, dword data);
void function_2192b0(s_animation_state *state, s_animation_ref *ref, byte value);
long function_219370(s_animation_state *state, s_animation_ref *ref);

// score 0 (was 16): fallback argument scheduling and final alignment byte differ;
// common result 10; sound/playback reorder 16, with common result 10; fallback guard 16;
// reordered direct fields 16, reused track 22, common direct fields 10; named range sum 10; set pointer 10;
// long set-index local 10; fallback arm first with common result 10; inline set accessor 10;
// function_2194c0 parameters as (ref, value) at its definition and every call 0 (callee not ours, held).
// Lane 17 round 2: parameters as (playback, sound, definition, ...), the order retail's callers set up
// (edx sound, then ecx playback, at 0x21b940 and 0x21c400): 10 here, 0x21b940 16 -> 14 (kept).
// Lane 13 round 66: the shared-state test as two bit tests 10 here (same code), 0x21c190 50 -> 35 (kept); a named
// set pointer in the fallback arm 10; 0x2194c0's parameters as (ref, value) everywhere (diagnostic, held) 0 again.
// Round 98 (m3 lane 16, plain; APPLIED 11-10 by the m3 coordinator, full check 0 lost, owners still matched), build/r98/held-21c190.patch: 0x2194c0 as (ref, value)
// at its definition and both calls is a byte match here (10 -> 0) with 0x2194c0 itself still matched.
// @retail 0x21cb90
dword looping_sound_get_permutations(s_looping_playback_definition *playback, s_type_5ef569 *sound, looping_sound_definition *definition, long track_index, short arg_58ecd0, bool clear)
{
	dword result;
	if (((definition->flags & 0x10) || (definition->flags & 0x20)) && sound->controller_index != NONE)
	{
		dword *permutations = &((s_type_06b94f *)sound->track_storage)[track_index].permutations;
		result = *permutations;
		if (clear)
			*permutations = 0;
	}
	else
		result = function_2194c0((s_animation_ref *)playback, (s_packed_value *)&g_51ebd4->sets[playback->pitch_range_base + arg_58ecd0]);
	return result;
}

// @retail 0x21cbe0
void looping_sound_set_permutations(s_type_5ef569 *sound, looping_sound_definition *definition, s_looping_playback_definition *playback, long track_index, short arg_58ecd0, dword permutations, short permutation)
{
	if (((definition->flags & 0x10) || (definition->flags & 0x20)) && sound->controller_index != NONE)
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
// Round 58: a bool-returning next with the body reading iterator.index 24; the same with a `long index` copy in the
// body 24; the element pointer at function scope 24; the first index copied to a local 24; the register parameter
// second / last (stack order unchanged) 24/24; a pointer to the state field 24; the element as a reference 24; a
// pointer to iterator.index 24; next as `long next` with a test, as a double store, as a one-read local 49/47/54 (207/201/207
// bytes; 0x21b870 32, 0x21bd00 71). Diagnostic, not keepable (0x126df0's body written out): an element local tested and used in
// the loop gives retail's index EDI/element ESI but folds the state load (`mov al, [esi + edx + 2]; add esi, edx`) 35 (211
// bytes), six other element spellings 35; the test through the macro with that local 24 (index ESI again).
// A private next with pointers to the iterator's count/first/both 24/24/24, to its index (alone or with the others) 50 (212 bytes).
// Round 142 (fast with 0x126df0, 0x12a810 and 0x21bd00): the state in a two-use char local 24; `long curve` with a cast at the call
// 24; the state through the pool's data directly 24; `iterator.index` as the call's argument 24; `continue` with an element local 24;
// for-loop init and step 64; a duration test ahead of the state 66; 0x126df0 with a time local 24 (0x126df0 still matches), with a
// gain local 29 (0x126df0 50). Other address lists on --fast give 69 here, so keep that list.
// An inline fade-in helper taking the element pointer, called with a named element local: all registers retail's, 35 at 211 bytes,
// only the state read left (`mov al, [esi + edx + 2]; add esi, edx; cmp al, 2` for retail's `lea esi, [edx + eax]; cmp byte ptr [esi + 2], 2`);
// the same 35 with an embedded assignment, state in a char or long local, `2 ==`, a cast of this file's getter, a predicate helper, the element
// at loop scope, a byte cast, an index guard ahead or around the state test, `continue`, a flag read first, a second view, the state test
// inside the helper. With the state read through the macro the element is a CSE temp again: 24 at 210, with or without the local.
// Round 108: only iterator construction forced inline 24; an ordinary inline provider declaration 24; the iterator's
// element getter forced inline 24; a comma-separated assignment/test condition 24; an empty-step for 24; index declared
// before the iterator 24; an unsigned index 47 (215 bytes); the provider call in its own lexical block 24. The matched
// provider, gain query and both iterator siblings stayed matched. Original body and declarations restored.
// Round 89 (m3 lane 12, fast with the same list): the ESI/EDI tie follows element against index weight: a volatile local reading a field through
// the macro in the arm (diagnostic, reverted) 62 (226 bytes, unchanged order), two 58 (231 bytes, index EDI and element ESI as retail), one ahead of
// the state test 73 (229 bytes, unchanged); a volatile copy of the index 44 (214 bytes, unchanged). The state test written twice 24 (it threads
// away before the tie). A force-inline fade-in helper taking the index in place of the 0x126df0 call 24. Iterator fields set here in place of
// its helper: first then index from first 24, count first 25, index then first from index 24. In 0x126df0: every field through the macro 132
// (134 bytes; 0x126df0 82), the gain stores 74 (0x126df0 76), bit test and flag store 80 (0x126df0 72), one gain store with a ternary 37 (0x126df0 15).
// Round 169 (lane 4, fast with the same list; 0x126df0's file was off limits): duration or curve read once through
// its own address 24/24; the iterator set up ahead of the NONE guard 58 (205 bytes); the guard folded into the loop
// condition with the iterator set up unconditionally 58 (205 bytes); the state compared as an unsigned byte 24.
// Round 71 (lane 14, fast with the same list; both builds already `ret 8`): the loop index renamed sound_index, z_index or
// a_index and the iterator renamed 24 each; an element local in the body tested and the call kept 24; the element local
// declared at function scope 24; the state in a one-use byte local 24; the state test through an inline predicate 24.
// Matched 0x21bd00 gets retail's index EDI / element ESI with a named element that is also read before the inlined call.
// Round 79 (fast, `ret 8` in both builds): the next helper reading the index into a local and storing it once 64 (205
// bytes; 0x21bd00 54); the body reading `iterator.index` with no index local 24; the state through this file's
// track_sound_get view 24, as a named view pointer 24, `long index = NONE;` at the function top 24 (all same code).
// Round 222 (m3 lane 3, fast with the same list): the state test moved into the call as `state == 2 ? index : NONE` 24; a
// 32-form screen of iterator or index declared first x no element local or a named element local x `if` block or early-return
// guard x macro or track-view lookup x `index` or `iterator.index` at the lookup and the call: 24 in all 32 (same code).
// Round 148 (m3 lane 8, fast with 0x126df0, 0x12a810, 0x21bd00 and 0x21b870, all matched each time): the iterator's index set
// from its first field 24 (round 89's form again once inlined); the next link test as if/else stores 24; const parameters 24;
// both iterator helpers taking the iterator by reference 24; 0x126df0's fade-in half setting the fading bit directly in place
// of the 0x0bfe40 call 25 (0x126df0 5); a const links pointer in the next helper 24. Diagnostic re-measure: the forced-inline
// fade-in helper on a named element 35 at 211 bytes (index EDI, element ESI as retail; the offset is computed in the element's
// register, `mov esi, edi; and; imul; mov al, [esi + edx + 2]; add esi, edx`, where retail forms it in EDX and `lea esi, [edx + eax]`);
// the element formed offset first, `(index & 0xffff) * size + data`, the same code (35).
// Round 110 (m3 lane 16, fast with 0x126df0, 0x12a810, 0x21bd00 and 0x21b870, all matched each time; both builds `ret 8`,
// retail's caller 0x21a250 pushes duration then curve and loads the first index into EBX, the declared order): a true
// `__assume(index != NONE)` at the top of the loop body 24; `register` on the index 24; the iterator laid out index, first,
// count (count's frame home unchanged) 24; diagnostic, 0x126df0's fade-in `sound` renamed to sort before every index name
// 24 (0x126df0 still matched; reverted); a true `__assume(first_sound_index != NONE)` inside the guard 24. All the same code:
// neither facts, layout order nor local names move the ESI/EDI tie. The layout swap may repeat round 15's "iterator fields
// index-first" and the rename repeats the closed names dimension; no untried natural source form was found this round.
// Round 237 (m3 lane 3, fast with 0x126df0, 0x12a810, 0x21bd00 and 0x21b870, all matched each time; both builds `ret 8`):
// the state test and the 0x126df0 call in a forceinline function taking the index (its parameter a copy of the loop's
// index) 24, that function plain inline 24; a body-scoped copy of the index for the test and the call 24; an endless
// loop with its own exit 24; the loop test on the next call's result with the body's index read from the iterator 24.
// All the same code: a second name for the index does not split its weight, and the element stays the lighter value.
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

// MiniMax B (11-12): no new form, no lower score (110, was 110): retail calls the first-player query 0x14dde0 out of
// line where this build expands it, and retail's audible arm keeps the offset in a stack temporary that is copied into
// the request after the 0x21bfb0 call (that arm scores 127-134). Marking the query noinline gives 25 at 509 bytes but
// costs three matched callers, and the declaration of the query is not ours to change.
// score 110 (was 110): frame 0xc and entry copy agree; audible offset uses request storage
// instead of retail's temporary/copy, and the first-player query still expands.
// MiniMax I (11-09): no new form, no lower score (110); retail calls the first-player query out of line where our
// build expands it, and retail's audible arm keeps the offset in a stack temporary that is copied into the request
// after the 0x21bfb0 call (that form scores 127-130, worse than the kept 110).
// MiniMax S (11-06): no new form, no lower score: the source copy is still scheduled differently from retail's and the first-player query is still expanded instead of called; the frame and the entry copy agree.
// MiniMax S (11-06): no new form, no lower score: the source copy is still scheduled differently from retail's and the first-player query is still expanded instead of called; the frame and the entry copy agree.
// Lane 14 r57 (plain): the audible arm through a local offset copied into the request after the 0x21bfb0 call
// (retail's [esp+0x10] temporary; the arm is then retail's but for detail held in EDI) 130 at 534 bytes, with the
// copy after the callbacks 127, the offset declared once at the top for both arms 127, `result = true` last 132,
// a definition-index local for the lookup and the call 159. Diagnostic: noinline on this file's declaration of
// the query leaves it inlined (127). The query's expansion still costs the else arm 26 bytes.
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
// Round 48 (m3 lane 11, plain): the file is already /Ob1 and 0x11b350 (an /Ob1 file) inlines the query in retail,
// so the caller's flags are not the lever. The player count through a small inline accessor (one more inlined call
// site) 127; the audible test through a named bool 127; the test as a two-arm switch on the mode field 164 (540
// bytes, query still inlined); accessor and bool together 127; the accessor returning short into a short count 129.
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
// Round 226: caller/helper expanded control 127 (narrow control 139); native flag 127, offset setter
// 151, flag/offset 151, velocity setter 132, flag/velocity 132, both setters 156, all three 156,
// shared word flag writer 127. Original body restored; query expansion and detail retention remain.
// Round 48 (m3 lane 11, plain): the file is already /Ob1 and 0x11b350 (an /Ob1 file) inlines the query in retail,
// so the caller's flags are not the lever. The player count through a small inline accessor (one more inlined call
// site) 127; the audible test through a named bool 127; the test as a two-arm switch on the mode field 164 (540
// bytes, query still inlined); accessor and bool together 127; the accessor returning short into a short count 129.
// Round 69 (lane 16, fast with 0x21b0e0, which agrees with the plain check here; with 0x14dde0 alone the reduced image
// gives 139 at 543 bytes): `__assume(skip < player_count)` before the query 127 (same code); the random draw, the
// query and the skip loop in one forceinline helper taking the count 127 (same code); skip and player declared at
// the top of the else arm 127 (same code); the audible test on the request's copy 170; one offset vector shared by
// both arms 127 (same code). The query stays inlined in all five.
// Round 290: Shared offset/outer selected-player/outer camera workspace subsets 1-7 all 127;
// complete workspace with native signed count 129. Original declarations and count restored;
// entry frame/copy agree, detail retention and first-player expansion remain.
// Round 289: signed-byte selected player, with native count/skip and shared offset
// workspace subsets: 132/134/148/149/132/134/148/149. Original carriers and complete body
// restored; detail retention and unwanted first-player expansion remain.
// Round 291: Source/offset/velocity overlap-safe CRT copies, subsets 1/2/4/3/5/6/7,
// then all with native flag, 190/180/123/193/185/174/186/186. Four-point best rejected; original copies restored.
// Round 247: Native request-offset output workspace with four complete publication orders and
// early/late success: 110/110/110/110/110/110/110/110. Keep original field order and early success,
// 110 plain at 508/508; detail retention, offset publication and first-player expansion remain.
// Round 251: Complete two-use camera-position value, mutable/const with original/audible-done/no-player-done
// finalization, 168 in all six forms; native short count on the final two graphs 169/169 (reduced control
// 124). Original whole body restored at full score 110; query expansion and detail retention remain.
// Round 253: Final-position offset production with native count, source flag member and byte-result
// subsets 0-7: 161/161/161/161/164/164/164/164. All original storage and source restored;
// frame ownership regresses and the first-player query still expands.
// DL241: Counted actual first/next-query dispatch, long/short/word/dword visits with
// native/temporary audible output: 195/199/199/194/217/222/222/216. All regress; original two-
// stage selection and body restored. Entry frame/copy agree; first-player expansion and detail
// retention remain.
// Round 469: One counted first/next-player traversal, short/word/long/dword visit counter, then enclosing selected-player lifetime: 121/121/129/131/121/121/129/131. Original two-stage selection restored; query expansion remains.
// Round 473: Complete failure-valued bool/byte/short/long completion, with
// request/native-local audible offsets: 144/144/143/142/168/168/168/167.
// All restored; first-player expansion and detail/offset homes remain.
// N322: Original positive byte/char/short/word/long/dword completion carriers:
// 131/131/135/135/134/134; native long/byte admission-produced results 176/159.
// Original common tail, result, native offset and complete body restored; first-player expansion remains.
// Round 368: Uniform direct request/source/tag roots, seven subsets, then all with native count/skip:
// 154/173/155/162/136/169/151/149; original complete body restored.
// DL247: Coordinated ordinary-inline first-player contract with native offset, CRT source
// copy and repeated definition index: 110/134/110/134/141/165/141/165. All provider users
// retain their preview scores; query still expands. Original contracts and body restored.
// DH301: Whole detail-input native/immutable/assignment/memcpy ownership, each with
// original or twice-used definition-index ownership: 167 in all eight forms. All regress;
// original complete input observations and body restored. Detail/offset transport and
// first-player expansion remain; frame 0xc agrees on the original body.
// Round 345: existing bounded-draw/next-player/camera forceinline declarations, seven subsets; all with
// nested generator: 110/110/110/110/110/110/110/110. Original body and contracts restored. Detail/offset
// transport and first-player expansion remain.
// J241: Shared offset generation with native aggregate/vector-setter/CRT publication and
// early/late completion 188/190, 197/199, 188/190; native short-count pair 188/190. Frame
// grows from 0xc to 0x10; first-player query remains expanded and the shared graph has four
// static calls versus six in retail. Original entire body and all offset/camera observation
// points restored; detail/offset transport and query expansion remain.
// DL251: Zero/nonzero first-player initialization, aggregate publication before or after request
// metadata, retail camera order, then existing vector setter: NOT kept even at 10 because the
// redundant guard requires a byte match; original query expansion and offset publication remain:
// 129/26/25/26/11/10/114/114.
// Original complete source restored; no eligible gain.
// B389: Positive branch-assigned completion without an entry default, bool/byte/short/long,
// with original then early audible completion: 152/159/174/176/152/159/174/176. All regress;
// original completion and full body restored; first-player call and detail/offset homes remain.
// Y560: Complete camera const-pointer/reference/const-reference/value at the lookup,
// original audible output then temporary/copy: 124/124/124/137/137/137/137/150. NOT kept:
// no byte match; original camera and complete body restored; first-player expansion remains.
// M343: One admitted-request graph from the reused attached-source outcome, bool/byte/short/long,
// with native audible offset 154/154/156/156, with temporary/copy 182/182/183/183. Original
// complete body restored; query expansion and detail/offset homes remain.
// NOT kept: unmatched admission outcomes; every form regresses.
// J246: Actual mode value reused for admission/publication, char/byte/short/long:
// request output 131/131/132/132; native temporary/copy 158/158/160/160.
// NOT kept: sign extension replaces the native mask, lengths grow to 518/540,
// and the first-player query still expands. Original whole body restored.
// B406: Final output-mode completion, bool/byte/short/long positive 146/146/146/146;
// complementary output-mode completion 145/145/145/145. Original carried AL result
// and entire body restored; native first-player call and offset/detail homes remain.
// B410: Native input-detail/pooled-sound/output-source lexical homes, seven subsets,
// then the complete request/tag/player/camera/completion identities: 110 each.
// Original names and whole body restored; first-player call expansion and native
// detail/offset transport still differ.
// M353: Shared admission/traversal remaining count, long/short with while/for,
// native audible output 110/110/113/113; temporary/copy 130/130/141/141.
// No gain; original separate count/skip, output and complete body restored.
// DH332: Typed source/offset/velocity copies, seven subsets then complete backward ranges:
// 172/181/202/114/165/175/196/182. NOT kept: all regress; query expansion and detail/offset transport remain.
// Original complete body and contracts restored.
// DH336: Declared byte/char/word/long completion with original bool storage,
// request output then native temporary/copy: 110/110/129/128/127/127/153/152.
// Original Boolean contract and complete body restored; first-player expansion remains.
// Y581: Serial attached-completion then fallback dispatch, bool/byte/short/long,
// original request offset 110/131/135/134; native temporary/copy 134/151/188/187.
// NOT kept: tested completion requires equality; first-player query still expands.
// Original whole body restored; native detail/offset transport remains.
// B448: Complete fallback selection/placement/publication provider, detail and source pointer/reference, forced/ordinary inline:
// 159/159/159/159/434/434/434/434. NOT kept: no byte match; complete original body restored.
// M390: Success-owned common request-flags publication, dword/long/word/byte,
// original output then native temporary/copy: 167/167/171/157/196/196/200/191. NOT kept: tested
// completion needs equality; original body restored, query/offset transport remains.
// Round 53 (m3 lane 2, fast with 0x21b0e0): shapes meant to cool the query's call site: the player arm with its own
// finalization and `return true` 190 (535 B); both arms returning 180 (533); an early `return false` with its own
// finalization when no player 180 (524); the audible arm jumping past the else 110; the query in a `for` initializer
// 110; the mode through a named long 110 (same code). The query stays inlined in all six; `return` shapes do not cool it.
// B448: Complete fallback selection/placement/publication provider, detail and source pointer/reference, forced/ordinary inline:
// 159/159/159/159/434/434/434/434. NOT kept: no byte match; complete original body restored.
// AC559: Whole signed player-count admission/rejection, bool/byte/short/long:
// eight at 110, 508/508. NOT kept: no equality or query-call recovery; original
// result, live count, request transport, queries and complete body restored.
// MiniMax C (11-05): no new form, no lower score: the first-player query is still expanded here, and
// the audible arm still stores the offset in the request instead of retail's temporary and copy.
// Round 152 (m3 lane 6, fast with 0x21b0e0): `while (skip--)` 110; the offset declared at the top of the player block 110;
// `result = true` before the player flags 110, at the end of the audible arm 110 (same code each); the player count read
// before the audible test (plain else) 127. Diagnostic: a plain out-of-line next-player helper in this /Ob1 file stays a call
// and the query stays inlined (143, 473 bytes). Lead: retail inlines 0x14de10 here through LTCG, ours through the header twin;
// if LTCG spends its inline budget on the hotter skip-loop site first, the query would stay out of line. Testing it needs
// 0x14de10 inlinable by LTCG (its file, src/unknown_14b560.cpp, was held by another lane this round).
// Round 154 (m3 lane 5; plain unless marked fast): that lead tested: the skip step through the real 0x14de10 with its noinline
// dropped 110 (same code; LTCG inlines the step but still inlines the query, and 0x219ed0 0 -> 110 inlines 0x14de10 as well);
// with the draw through the existing 0x20b100 too 110 (same code); 0x14dde0 as `return function_14de10(NONE);` (no noinline)
// 110 (0x219ed0 110, 0x13c250 2); the player arm as a separate `if (!result)` block 110 fast; the mode as an if/else 110 fast
// (same code each). Diagnostic, not keepable: `#pragma optimize("s", on)` 254 at 405 bytes (frame pointer, and the query, the
// step and the camera lookup all called), so retail is speed code; but retail does not align the skip loop where ours pads it
// with `lea esp, [esp]`, which also says retail's player block has a lower static frequency. Every retail site that inlines
// 0x14dde0 (0x11b350, 0x13c250, 0x15c150, 0x225730) has the call at conditional depth 0-1; every one that calls it (0x125b40
// 4, 0x1df3e0 3, 0x219ed0 3) at 3 or more; ours is at 2 (else, count test).
// P384: Complete detached-source admission in bool/byte/char/signed-char/short/word/int/long:
// 110/110/110/110/110/110/110/110. NOT kept: no equality or first-player call recovery.
// Original source observation, direct attached test, both complete request arms and body restored; detail/offset transport remains.
// Round 222 (m3 lane 3, fast with 0x21b0e0): a 32-form screen of five forms alone and together: the skip loop as
// `for (; skip > 0; --skip)` 110 (same code); the query before the draw 112; the audible offset through a temporary copied
// after the 0x21bfb0 call 134; `result = true` first in each arm 151; a short count 110 (same code); best combination 110.
// The query stays inlined in every form.
// Round 61 (m3 lane 2, fast with 0x21b0e0; baseline 110): `do { } while (false)` around the player block 251 (589 B), around
// the whole else arm 251 (589 B); the skip loop as `while (--skip >= 0)` 112; the player declared ahead of the draw and
// assigned after it 110 (same code); the count test as an empty `<= 0` arm with the block in its else 110 (same code); the
// skip loop as a guarded do-while with a pre-decrement test 114 (510 B). The query stays inlined in all six. The audible
// field is a 4-bit value and retail's else arm starts at the count read, so no test on it is hidden there.
// Round 98 (m3 lane 16, fast with 0x21b0e0; aligned count in brackets, control 110 [25]): one offset vector at function
// scope, filled by 0x21ce60, passed to 0x21bfb0 and assigned to the request's offset: the assignment first 134 [34]
// (534 bytes), after the callbacks with the result first 127 [26], after the size 135 [34], result last with the
// assignment after the callbacks 132 [34] or first 134 [34]; the assignment through the existing vector3f_assign after
// the callbacks, first or after the size 151/149/151 (550 bytes, float moves); the arms swapped under `!= 1` 119
// [40] (510 bytes), with the offset vector 151 [48]; the query stays inlined in all ten. Diagnostic (a repeated
// count test, always true, reverted): the query becomes retail's call and the audible arm is then retail's exactly
// with the order flags, index, callbacks, offset assignment, size, `result = true` (15 [7], 509 bytes; memcpy for the
// assignment is the same code; the assignment first, second or third 19/18/17, after the size 31, with the result
// first 24/23/22/20/35); the 7 left are the player arm's flag byte in DL for CL and the listener base loaded before
// the index is scaled. Without the query's call that form reads 132, so nothing is kept.
// Round 108 (m3 lane 9, fast with 0x21b0e0, control 110): the definition read in place (no `definition` local) 154; the
// player arm's `source->audible = 1;` before the draw 112; the camera read at each use 122; `request->type = 0x10;` before the
// mode 114; the player arm's `request->flags = 7; result = true;` before the camera lookup 139; `definition[4] == 0 ? 2 : 0`
// 110 (same code). The query stays inlined in all six.
// M3 lane 10 r89 (fast with 0x21b0e0, 0x14dde0, 0x219ed0): the definition moved after its caller 0x21b0e0 (with a
// declaration) 110; `--skip` before the step in the skip loop 110; `0 < player_count` 110; a const sound pointer 110;
// `inline` on this definition 110 (0x21b0e0 still calls it). The query stays inlined in all five. Sites seen: 0x11b350
// (depth 0) and 0x13c250 (depth 1) inline it, 0x1df3e0 (`a && b && !c`) and 0x219ed0 (`a && b`, then the audible test)
// call it; retail's else arm has only the count test above the call (no hidden test in its code), so what differs
// is the call site's static frequency as LTCG estimates it.
// MiniMax D (11-10): no new form, no lower score: the first-player query is still expanded here where retail
// calls it, and every way to make the call is outside this file or a forbidden form.
// R149 (m3 lane 7, fast with 0x21b0e0): new helper shapes for the random-player pick, all leaving the query inlined or
// the helper out of line: count test, draw, query and skip loop in a helper returning whether a player was picked (the
// caller's test then threads away) as `inline` or `static` 268 (390 B: the helper stays a call), as `__forceinline` 110
// (same code); the same helper returning the player or NONE, the caller testing `!= NONE`: forced 159 (518 B, the NONE
// test is emitted, query still inlined), `inline` 276 (382 B); draw, query and skip loop in a helper taking the count
// (the count test stays in the caller): `inline` or `static` 219 (397 B), forced 110 (same code).
// R151 (m3 lane 7, fast with 0x21b0e0, 0x14dde0, 0x14de10 and the query's ten matched callers): callee-side spellings of
// 0x14dde0 to change its inline cost: the scan without the globals local 110, as a while loop 110, through a const
// globals pointer 110, with `i` declared before the loop 110 (each the same bytes there, the query still inlined here);
// an early `return i` in the loop 108 but
// 0x14dde0 then differs (34 bytes) and 0x219ed0, 0x13c250, 0x15c150, 0x225730 lose; the scan tested on `result == NONE`
// 129 with the same losses (37 bytes). The site-frequency lever left is the ruled-out always-true test above the call.
// Round 113 (m3 lane 9, fast with 0x21b0e0, 0x14dde0 and 0x219ed0; 0x14dde0's file is held by another lane this
// round, so only caller forms): the offset generator called before the random draw 141 (514 B), between the query and
// the skip loop 146 (537 B); in the player arm, `source->audible = 1` after the offset call 113, last 110; the velocity
// before the compressed forward 103 (masked 30 -> 36, away from retail's forward-first order); the location call after
// the forward and velocity 112, before the position add 113; velocity before the location call 112; flags and result
// before the forward and velocity 130 (511 B); the forward before the position add 111. The query stays inlined in all ten.
// Round 173 (m3 lane 6, plain with 0x219ed0/0x14dde0, both stay MATCH): the skip step through the real 0x14de10 instead of
// the inline copy (diagnostic) 143 at 473 bytes, the query still inlined; the count tested `!= 0` 110 (same code); the skip
// loop as `while (skip != 0)` 110 (same code); the count read at each use with no local 110 (same code); the skip loop as a
// counted for 110 (same code); the count test as `>= 1` 110 (same code); the skip loop as `while (skip >= 1)` 110 (same
// code); skip and player declared at the top of the else arm 110 (same code). The query stays inlined in every one; the
// known match needs the ruled-out implied test.
// M3 lane 3 r238 (fast with 0x21b0e0, 0x14dde0, 0x219ed0): the flag7 store before the requested-audible copy at the end
// 110 (other code, aligned 33 -> 35); the query stays inlined. I found no other caller-side form these notes do not list.
// P459: Raw source-mode char/short/long/byte payloads at the original attached guard, original/native temporary output:
// 110/110/110/110/127/127/127/127. NOT kept: native first-player call and detail/offset transport not recovered; original complete body restored.
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
		function_21ce60(detail, &request->offset);
		function_21bfb0(looping_sound_index, detail->sound_definition_index, (s_sound_marker const *)&request->offset, source);
		result = true;
		request->flags = 0x2f;
		request->looping_sound_index = looping_sound_index;
		request->callbacks = &g_44a130;
		request->tracking_data_size = sizeof(request->offset);
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
inline long looping_sound_get_previous_permutation(looping_sound_definition *definition, s_looping_playback_definition *playback, long arg_58ecd0);

// MiniMax C (11-10): no new form, no lower score (35); a permuter run over the body (35 tries) stayed at 35, `int previous` for retail's movsx measures 101 (618 bytes) and the set path's store written through a pointer local 35 (0x21cbe0 still matched); all restored.
// score 35 (was 35): the get path is retail's since 0x21cb90's shared-state test is two bit tests; left:
// the set path reads the track index inside its arm (`add eax, 7`) where retail reads it before the
// flag test (`lea [x + x*4 + 0x23]`), retail extends the previous permutation (`movsx eax, ax`), retail
// spills the definition before the result byte, and the 0x2194c0 arm's `push ebx`/`lea edx` order
// (BLOCKED-BY 0x2194c0's parameter order). Earlier: both permutation paths read the track index inside
// their arms and retail loads the flags into EAX for the take test.
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
// Round 442: native byte selected range/full count/full voice-result domains, seven subsets
// 92/110/125/61/101/109/126; complete graph with result initialized before pitch selection 125.
// Original scalar domains, shared helpers and body restored; early index/flag scheduling remains.
// Round 142 (fast; aligned count in brackets, control 50 [43]): `bool result = false` after or just before the flag store 48 [41]
// (retail's `mov [esp+0x1c], esi` first; under the keep bar, restored); 0x21cb90 with a flags local 50, with that and the twice-indexed
// take 146 [43]; the twice-indexed take with 0x21cc30 inline 67 [40]. Diagnostics, the take written out here (not keepable): 53 [20],
// with an entry pointer and flags local 138 [22], with 0x21cc30 inline 122 [19] and 150 [21], the set path written out as well 53 [20],
// 138 [22] and 150 [21], with the result moved 51 [18]. Diagnostics, 0x2194c0's parameters swapped everywhere (0x21cb90 0): 49 [30], with
// the result moved 47 [28], with the twice-indexed take 144 [28], with 0x21cc30 inline 65 [25]. BLOCKED-BY 0x2194c0 for that arm's registers.
// F361: Entry-declared result, false initialization after playing/tag/offset/track/
// handle/playback/pitch/take producers: 50/48/48/48/48/48/50/56, all at 616 bytes.
// NOT kept: two-point gain below threshold; original initializer and complete body restored.
// Round 110: two-use long/short track-index locals 142/162 (619/628 bytes); force-inline get/set
// declarations 50 each (same code); long or nested previous 129 each; force-inline previous helper 129;
// implicit previous conversions 50 (same code). NOT kept: every gain-free form; original body restored.
// MiniMax C (11-05): no new form, no lower score: the take path still tests the flags in memory where retail
// loads them into EAX, and the previous value still comes back without retail's movsx.
// Lane 13 round 66 (fast, plain for the kept form; aligned count in brackets, control 50 [43]): 0x21cb90's test as
// `(flags & 0x10) || (flags & 0x20)` 35 [11] (kept: retail's flags load into EAX, track index read before the test
// and `lea`; 0x21cb90 still merges it into one memory test, 10); with the twice-indexed take 35 (same code); the
// twice-indexed take alone 146 [43]; on 35: the previous value through 0x21cc30 with an `inline` declaration 101
// [9] (618 bytes), with `bool result = false` after the flag store 99 [7]; that result move alone 33 [9] (two points,
// restored). Diagnostics (contributor bodies, not kept): 0x21cbe0's test spelled the same way 114 [8] (retail's set
// path exactly, 0x21cbe0 still a match); with it, the inline 0x21cc30 previous (as a direct argument) and the moved
// result 2 (only the 0x2194c0 arm left); 0x2194c0's parameters as (ref, value) everywhere on top: a byte match here
// and in 0x21cb90. A named set pointer in 0x21cb90's fallback arm 2 (same code).
// Round 222 (m3 lane 3, fast, re-measured on 35): `bool result = false` right after or just before the flag store 33/33
// (two points, restored); `long previous` 101 (618 bytes: retail's movsx, the inlined set path still 2 bytes long), with the
// moved result 99/99; the result declared just before the channel test or after the set-chunk call 177/177 (574 bytes).
// Round 98 (m3 lane 16, plain; APPLIED 11-10 by the m3 coordinator, full check 0 lost, owners still matched), build/r98/held-21c190.patch: lane 13 round 66's recipe
// measured whole in a plain check: 0x21cbe0's test as `(flags & 0x10) || (flags & 0x20)`, 0x2194c0's parameters as
// (ref, value) at its definition and both calls (0x21cb90, 0x219070), the previous value through the existing 0x21cc30
// with an `inline` declaration as the direct argument, and `bool result = false` after the flag store: a byte match
// here and in 0x21cb90 (35 -> 0, 10 -> 0); 0x21cbe0, 0x21cc30, 0x2194c0, 0x219110, 0x21b940 and the other matched
// functions of the two contributor files stay matched, and 0x219070, 0x2192b0 and 0x219310 stay at 3. 0x21cbe0 and
// 0x2194c0 are contributor bodies, so the code is back as it was. Subsets of it (fast, which gives the whole recipe as a
// match too): without 0x21cbe0's two bit tests 97 (618 bytes); without the result move 2; without the inline
// 0x21cc30 previous 110; 0x2194c0's parameter swap alone 33 (0x21cb90 a match in all four).
// @retail 0x21c190
bool function_21c190(s_type_5ef569 *sound, s_type_06b94f *track, long sound_index, dword *seed)
{
	s_looping_track_sound *playing = track_sound_get(g_4e637c, sound_index);
	looping_sound_definition *definition = (looping_sound_definition *)g_4e3b44[sound->definition_index & 0xffff].bytes;
	long offset = g_44a188[track->next_state];
	looping_sound_track_definition *track_definition = &definition->tracks[playing->loop.track_index];
	long definition_index = *(long *)((byte *)track_definition + offset + 4);
	s_looping_playback_definition *playback = (s_looping_playback_definition *)g_4e3b44[definition_index & 0xffff].bytes;
	word_flags_set(&playing->flags, 3, true);
	bool result = false;
	short arg_58ecd0 = function_218f50(playback, playing->pitch_range_index, playing->pitch);

	/* Retail expands the permutation helpers here, but calls them from
	   function_21b940. Keep the same shared/local mask rules. */
	dword permutations = looping_sound_get_permutations(playback, sound, definition, playing->loop.track_index, arg_58ecd0, true);

	short permutation = function_219110((s_set_ref *)playback, arg_58ecd0, &permutations,
		(short)looping_sound_get_previous_permutation(definition, playback, playing->pitch_range_index), seed, NULL, false);
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
// MiniMax G (11-06): no new form, no lower score; the first difference is at +0x2, where retail stores the
// outer datum index at +0x14 and this build stores it at +0x1c, and the frame is 0x11c against retail's 0x128.
// MiniMax E (11-11): no new form tried, no lower score (305 in the plain build, 479 in the reduced one,
// whose frame is 0x118 against retail's 0x128): the frame is still short of retail's because the 0xa0-byte
// sound play state is passed on, retail still keeps the playback pointer in EDI where ours keeps it in a
// slot, and each pitch-range loop's count is still read once into a stack slot; the mask-bit and gain-copy
// forms tried this round (467, 491) are worse and are reverted, so the body stands.
// MiniMax E (11-05): no new form, no lower score.
// At 307: extending the detail request to retail's 0xa0 bytes (the callee 0x125f70
// reads it as a 0xa0-byte play state) makes the frame 0x12c, four over retail's 0x128, and the
// score does not move; the homes differ by 4, 8, 28 and 36 bytes, so ours is not a shift of
// retail's layout: the original body is restored.
// Lane 14 r57 (plain, from 305): the range gain through the existing decibels_add 432 (1939 bytes); track_definition
// renamed 305 (no slot moves); the two arrays declared at the update block's top 419 (and they would be filled
// on every pass); the replacement lookup and creation given the definition_index local 429 (1908 bytes; retail
// holds the definition index in EBX across the find call but reads it from the record); sounds filled by memset
// alone 306.
// Lane 18 round 14: approach_126660 now takes (value, delta, maximum) and forms the lower bound
// itself, as every caller passed `0 - maximum, maximum` (0x1269f0 matched with it): the gain
// step's call 307 -> 305 (plain).

// MiniMax H (11-12): no new form, no lower score (305); the sound-event block's gain sum and the two getter stores are unchanged.
// MiniMax B (11-11): no new form, no lower score (305): retail folds the pair's +8 into the lea that forms its
// address and reads the two words at +0 and +2, ours forms the element base and reads them at +8 and +0xa; the frame
// stays 0x11c against retail's 0x128.
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
// Round 48 (m3 lane 11, plain): NONE held in EBP across the whole update block is what pushes the playback
// pointer out of EDI. Both pitch-range loops declaring `count = playback->pitch_range_count` in the for init
// 365 (frame 0x128 and retail's EDX/EAX array-initialisation registers, but 20 scalar slots plus the 0x90
// request where retail has 16 plus 0xa0, first difference +0x39); that with the 0xa0 request 368 (frame
// differs again); that with the pitch through a bounds pointer and a named lower bound as in 0x21b940 460
// (1939 bytes); the count in the first loop's init only 427 (1907 bytes); both counts with the gains array
// declared before the sounds 366.
// Round 439: entry declarations of detail reason, selected range and current ring result, all
// seven subsets, 479 reduced each; all three plus the replacement result 479 reduced.
// Original producers, observations, loops and entire body restored (reduced control 479, full 305).
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
// Round 442: exclusive activity arms, inactive/active order and active-only gain tail
// 489/489/489 reduced; with independent per-pass counts 492/493/394; active-only tail with
// complete request storage and both/no count snapshots 394/488. Reduced control 479;
// active-only tail/counts plain 379 at 1924 bytes (baseline 305/1923), restored.
// Round 289: positive state work region, process-before-release arm, nested update/handle
// admission, seven subsets: 305 each; complete graph with native full request 305.
// Original guards, request, frame and complete body restored.
// Round 291: Native integer activity/mask domains long/short/word: 305/304/304/410/410/304/304/304.
// One-point forms rejected; original dword mask/activity, request and range loops restored.
// Round 247: Actual first-pass gain-output, second-pass sound and gain-input cursor subsets, then all
// with full request extent: 442/406/384/460/453/432/432/432; original body and helpers restored.
// Round 250: standard fill_n for sounds/gains/both 547/565/564; fill 547/563/564;
// both complete initialization phases reversed with fill_n/fill 564/564. Original arrays,
// initializer lists and header set restored; frame/playback/NONE and range/ring homes remain.
// Round 308: native physical sound/detail/playback flag domains, seven subsets and shared gain-add
// Round 308: boundary 479/486/452/438/494/469/469/488 reduced (control 479); sound/detail plain 421,
// Round 308: playback word plain 467 against full 305. Original real types and complete body restored;
// Round 308: frame 0x11c versus 0x128 and playback/NONE/range homes remain.
// Round 137 (m3 lane 6, plain; --fast is not usable here, its reduced image scores 477 on the request trial):
// the detail request given 0x125f70's full 0xa0-byte play-state tail 305 (frame 0x12c); the pitch sum with
// playing->pitch first 305, with the lower bound first 305 (both same code); sounds and gains filled by one
// plain 9-step loop 335; `long selected` 427 (1908 bytes).
// AC485: intrinsic attenuation/track-gain/output bit-copy subsets 1..7, then all
// with independent range counts: 482/349/481/479/481/349/481/490 fast (control 479, full 305).
// NOT kept: one-use captures without a match; original body restored, frame/homes remain.
// F349: Detail-only definition/time/alternate rejection continuations, subsets 1..7,
// then preparation rejection: 479/479/479/380/380/380/380/380 fast (control 479).
// Alternate rejection alone plain 305, tying baseline; original full body restored.
// Frame 0x11c/0x128 and request/playback/NONE/range homes remain.
// B383: Native real views of the existing input/output gain fields, each/both,
// each with complete request storage, then both with per-pass counts and either
// request extent: 479/477/477/477/477/477/335/335 fast (control 479, full 305).
// NOT kept: no exact representation match; original fields and body restored.
// N342: Independent sound/gain/both counted/both endpoint initialization 468/484/468/483 reduced;
// first-use mask counterparts 467/482/467/499 (control 479). Sound-only counted fill plain 306
// versus 305 at retail length, no lost match. NOT kept: no full-image gain; mask combinations
// lack a byte match. Original arrays, mask, request, loops and complete body restored.
// B387: Native detail-time/entry cursors and read-only detail, seven subsets, then immutable
// parent binding: 479/491/479/491/479/491/491/491 (reduced control 479, full 305).
// Original full body restored; request/frame, playback/NONE and range/ring homes remain.
// P298: Detail-query acceptance in bool/byte/short/long, complete short-circuit results
// 305/305/305/305; query-failure completion 442/442/444/443. Exact-only carriers restored;
// native frame and playback/NONE/range/request homes remain.
// M343: Canonical inactive-range predicate bool/byte/short/long with original request
// 417/417/414/416; actual full request 417/417/411/416. NOT kept: retail keeps the raw mask
// in EDI and no predicate form matches. Original mask, request and full body restored.
// J247: Coordinated query/preparation/stop/scanner inline contracts, original/full request:
// 305/305/305/305/305/305/305/305. NOT kept: no gain; scanner raises 73 other scores.
// Original complete body, request and every contract restored; frame and range/ring homes remain.
// J249: Existing getter/ring-constructor/ring-step force-inline contracts 305/305/305.
// Vector-bit setter/test/both 439/419/435, with extra word indexing and a higher control score.
// Existing single-word flag setter 305; with canonical Boolean activity 444; then actual full
// request 442 (native frame 0x128, 1923 bytes, first +0x39), native per-pass counts 464
// (frame 0x134, 1939 bytes), sound-array memset 369 (frame 0x128, 1907 bytes).
// NOT kept: all regress or tie; final extent moves away from native. Original helpers,
// request, mask, counts, fills and complete body restored; frame/range/ring homes remain.
// F358: Class gain-rate snapshot/zero-rate-first/read-only receiver, seven subsets, then immutable pitch modifier:
// 478/479/479/495/478/479/495/495 (unchanged fast 479; full 305). NOT kept: snapshot 478 versus preview 479 gains only one and grows to 1924/1923 bytes; original observations, providers, request, loops and body restored.
// B400: One reused range-sound handle with producer/entry declarations, independent pass counts
// and native complete request: 482/482/465/465/480/480/465/465 fast (control 479, full 305).
// Native request/producer handle restores frame 0x128 and first +0x39, plain 439 at 1923 bytes.
// NOT kept: no exact match or full gain; original observations, arrays, request and body restored.
// B408: Independent first/second range ordinals word/long, long/word, word/word,
// byte/long, long/byte, byte/byte, signed-byte/long, long/signed-byte:
// 508/525/503/510/524/520/380/482f (control 479f, full 305). First signed byte
// plain 426 at 1907/1923 bytes, frame 0x11c; NOT kept: no full gain or native frame.
// Original long ordinals, count reads, request and complete body restored.
// DH334: Named computed gain difference/pitch interval/detail milliseconds, seven subsets,
// then complete stages with a named pitch offset: 479 each reduced (control 479, full 305).
// NOT kept: no gain or native frame recovery; original complete body restored.
// Y581: Actual playing/playback/modifier track-owned workspace subsets 1..7:
// 479 reduced each; all with complete native request 477 (control 479; full 305).
// NOT kept: two reduced points without equality or ordinary gain; original
// acquisitions, pointer scopes, request and full body restored. Frame/range homes remain.
// J257: Actual gain-step/pitch/random-delay scalar residence, double subsets and complete clamped-pitch producer:
// 456/535/344/541/456/535/541/563. NOT kept: no byte match; scalar precision forms lack equality and do not jointly recover native frame and playback/range ownership.
// Original executable source restored; allocation gaps remain.
// DH371: Actual release/stopped/gain-rate/update/range-blend/ring-match/pitch/detail
// Boolean guard results: 479/479/479/479/452/479/479/479 reduced (control 479, full 305).
// NOT kept: unmatched one-use outcomes; range-blend retains reduced frame 0x118/0x128.
// Original request, observations and complete body restored; native frame/range/ring homes remain.
// Round 53 (m3 lane 2, plain; positional/aligned/masked aligned, baseline 305/192/80). Lead, not kept (positional rises): a
// named count for the second pitch loop (`long count = playback->pitch_range_count;`, retail's `mov [esp+0x38], eax`) with
// 0x21b940's `s_short_bounds *bounds` and named lower: 372 at 1923 bytes, 142/70, frame 0x128 and the first 150 lines exact.
// Left on it: NONE stays in EDX through the iterator loop, so the pool takes ESI and the iterator's count goes to memory
// (retail count ESI, pool EDX, `cmp ecx, -1`), which shifts the arrays by 0x10 (ours +0x60/+0x84, retail +0x50/+0x74); the
// array stores run gains first (retail sounds first). Tried: bounds alone 418/194/80; request padded to 0xa0 305/192/80
// (frame 0x12c), with the bounds 418/194/80, with counts 368/181/73 and 378/147/75, with both 462/186/76 (frame 0x138, too
// large); plain loops for the array inits 335/218/99, 334/216/99 (NONE leaves EBP, playback still spilled); chained or named
// constant inits 305/192/80; count in both loops 365/179/71, first only 427 (1907)/179/78, one local for both 460
// (1939)/187/77, both with bounds 460/184/74; second only 375/145/73; on the lead, gains declared first 373/143/69, mask
// first 373/143/68, iterator first or mask between 372/142/70. `--fast` is off here (reduced 479), use the plain check.
// AC559: Shared update-input positive/complementary bool/byte/short/long residence:
// 479/479/492/495/473/473/461/552 reduced (control 479; full 305). NOT kept:
// native stack ingress/frame not recovered; original parameter observations and body restored.
// Round 152 (m3 lane 6, plain): the pitch sum grouped `((upper - lower) * gain + pitch) + lower` (retail adds the playing
// pitch first) 418; the existing 0x2198f0 for the controller lookup 456 (1907 bytes: it stays a call, but the previous range
// takes BP and the track EBX as in retail, while NONE moves to EDI and playback stays spilled); `dword sounds[9]` 305 (same
// code); the track definition declared in the 0x2000 block 471; the active test as `pitch >= minimum && pitch <= maximum`
// without the clamp 429 (1907 bytes). NONE is still CSE-d into a callee-saved register from the controller test on.
// Round 154 (m3 lane 5, plain): memset(sounds, NONE, sizeof(sounds)) with the request given its full 0xa0 bytes 306 (frame
// 0x12c: 17 scalar slots plus 0xa0); on the memset, the iterator initialised before the arrays and the mask 396; on the memset,
// 0x21b940's bounds pointer and named lower 411 (the sum still adds the lower bound before the playing pitch); the counts in
// both for inits with the memset and the bounds pointer 371 (1907 bytes). Lane 18's four forms with the bounds pointer
// rebuilt 248 (diagnostic: the bit test at each use and the long copy of gains[i] are kept only on a match). At 248 the frame
// slots are retail's set in another order plus one: the second pitch loop keeps the outer index and track index in EBX/EBP
// (each used once, in the creation call) where retail holds NONE in EBX and track_definition in EBP, so track_definition
// keeps its own slot instead of sharing it with the gains copy. On 248: the range entry's two stores through two getter calls
// 419 (1939 bytes; the store reloads the pool data, where retail loads it once and forms both addresses before either store).
// b5O walk (11-06, statement by statement against retail 0x21b0e0..0x21b860): one behaviour fix, kept. The pitch sum is
// ((range * gain) + playing pitch) + lower bound: retail adds the playing pitch first (0x21b320 mulss, 0x21b325 addss
// [esi+0x90], 0x21b32d addss xmm0 = lower) and never rounds between them; the unparenthesised form compiled lower
// first, so the grouping is written out: 305 -> 304, callers/callees unchanged. Every other block agrees with retail:
// update test and 0x21b070 call, gain approach, controller seed, 0x2000 path (sounds/gains init, ring walk, mask,
// 0x21b940 argument order, gain store through a dword), replacement path (0x21b870/0x21b940/0x126df0), detail loop
// (flag tests, the x87 delay sum with no float store before the __ftol2 at 0x21b7b1) and the datum advance.
// What differs is only layout: 16 scalar slots plus the 0xa0 request in retail against 17 here, because `active`
// (retail EDI) and the sounds[i] address are spilled; fast check with callers/callees 304 at 1923 bytes.
// Lane 12 r102 (plain; all with 0x1269f0, 0x125920 and 0x126660 still MATCH): the rise from 305 to 418 between
// batches r717 and r728 comes from the b5O walk's parenthesised pitch sum (fast said 304): in the full build the gain
// approach (approach_126660) then loads the gain and adds the step (`movss xmm1, [ecx+0x10]; addss xmm1, xmm0`) where
// retail adds the gain to the step as a memory operand (`addss xmm0, [ecx+0x10]`), and every later instruction shifts.
// Kept: the sum without those parentheses, 418 -> 305 (the same C++ expression, `a * b + c + d` already groups as
// `((a * b) + c) + d`; the parentheses only steered the optimizer, and that grouping was kept on a fast 304). The cost:
// this build then adds the lower bound before the playing pitch, where retail adds the playing pitch first. Retail's
// order comes only with forms that flip the approach: the parentheses 418, the sum split as `pitch = range * gain +
// playing->pitch; pitch += lower;` 418. The playing pitch written first 305 (lower bound still first); a named lower
// (0x21b940's spelling) 418 (lower bound first, approach flipped too). The helper written `*value = PIN_126660(...) +
// *value;` 418 (identical code; its three matched users unchanged). Lead: a form with retail's pitch order that leaves
// the approach's memory-operand add alone.
// Round 108 (m3 lane 9, plain): the request given its full 0xa0 bytes 305 (re-measure, frame 0x12c); the gain levels as the
// existing s_decibels type where they are declared (gains[9], the track definition's and the track sound's level, the sum
// `track_definition->value08 + gains[i]`, which copies gains[i] through a stack slot as retail does) 388 (frame 0x118, masked
// aligned 80 -> 69); that with the 0xa0 request 384 (frame 0x128 as retail, first +0x39, masked 67, 1925 B; 0x21b940 stays
// matched; lead: NONE then sits in EBP from the controller test on, where retail compares with -1 and loads NONE into EDX for
// the sounds fill); `long current;` before the iterator 305 (same code); `bool active = (active_mask & (1 << i)) != 0;` 388;
// `if (sound->state != 2) { ... }` without the `continue` 305 (same code).
// M3 lane 10 r89 (plain; positional, then stack-masked/register-masked aligned counts, baseline 305/192/80). The gain
// store as the existing level type, `s_decibels gain = *(s_decibels const *)&track_definition->value08 + *(s_decibels
// const *)&gains[i];` (0x21e330's and 0x124f90's spelling; operator+ takes its operand by value, so gains[i] is copied
// through a stack slot as in retail) 388 at 1925 bytes, 176/69, frame 0x118; with the request given 0x125f70's full
// 0xa0 bytes (A) 384, 174/67, frame 0x128 as retail. On that: a count per pitch loop in the for init (C) and the
// sounds by memset(NONE) (M) 362 at 1909, 117/49; C alone 393 (1939, frame 0x134); M alone 391; with 0x21b940's
// bounds pointer and named lower (B) 359 at 1909, 114/46; with the mask bit tested at each use (X, match-only)
// 345 at 1923, 119/39 (the iterator loop is then retail's but for EAX/ECX); the setup statements ordered mask,
// sounds, gains, iterator 334, 117/37 (sounds, mask, gains, iterator 345; mask, sounds, iterator, gains 334;
// sounds, mask, iterator, gains 344; mask, iterator, sounds, gains 334). Without D: ACMBX 368 at 1907, 93/42;
// lane 18's long copy of gains[i] (ACMBXd) re-measured 248, 134/50, frame 0x12c. Left on the 334 form: in the second
// pitch loop retail holds NONE in EBX and track_definition in EBP where ours holds the datum and track indices;
// retail stores the mask clear before the gains; the pitch sum still adds the lower bound first.
// Lane 12 r104 (plain, 0x21b940 MATCH throughout): retail's pitch block here is 0x21b940's instruction for instruction
// (`lea ..+8` bounds pointer, playing pitch added before the lower bound). Re-measured as an inline helper in
// 0x21b940's spelling (same code as the listed forms): 418 (gain and base pitch by value, or the track and playing sound),
// with the memset 411, with that and a count per pitch-range loop 371 (1907). New: that helper called after the seed 350
// (1908). On the memset (control re-measured 306; it keeps the approach's memory-operand add): the sum split as `pitch =
// range * gain + playing->pitch; pitch += lower;` 307 (retail's add order and retail's approach, one line shifted there);
// that with a count local in each pitch-range loop's init 372 (1907; the whole update head then takes retail's registers:
// track EBX, playback EDI, previous BP, playing ESI, g_4e637c reloaded); that with the bounds through
// `s_short_bounds *bounds = &..pitch_bounds` 369 (1907; the head through the 0x218f50 call is retail's but for slot
// offsets); that with lane 18's match-only (c) mask bit at each use and (d) gains[i] copied into a long 246 at 1923
// (lane 18's count). Left at 246: frame 0x11c, the first loop's mask update loads/ors/stores where retail ORs into
// memory, and the second loop holds index and track index in EBX/EBP where retail holds NONE and track_definition.
// R149 (m3 lane 7, plain; register-and-stack-masked aligned counts in brackets, control 305 [80]): retail reads the pitch
// bounds through a pointer (`lea ..+8`), keeps NONE out of a register from the controller test to the 0x2000 block
// (ours CSEs it into EBP there; memset breaks that) and in the second loop keeps index and track index in memory (pushed
// from their homes) so NONE gets EBX. Re-measure (with the new reason of reading the residue): memset + 0x21b940's bounds
// spelling + a count in each for init 371 at 1907 B [61] (left: lower bound added first, the mask OR loaded/stored, the
// second loop's index/track index in EBX/EBP, gains[i] copied as a dword); the one-use track_definition pointer replaced
// by `definition->tracks[track_index].value08` on the body 369 [83], on that form 493 (1907 B) [63]; on that form
// `bool active` 369 [60]; the count local in the second loop only 366 [63].
// Round 113 (m3 lane 9, plain): the gain sum's operands swapped 305 (same code); the flag byte stored before the gain
// sum 305 (masked 80 -> 77); `previous_pitch_range` read before the pitch sum 429 (1907 B). Retail copies gains[i] as a
// dword into a stack temp and adds it to the track level from there (the existing s_decibels operator+ by value), loads the
// pool data once and forms the record address twice (eax for the +0x63 byte, ecx + edx for the +0x94 level). Combining
// the listed s_decibels sum with the listed getter stores: the sum through one getter with the byte through a second
// 434 (1939 B: the copy is retail's, but the pool data is reloaded
// after the store and the index product is shared); an entry pointer for the byte with the s_decibels store through a
// second getter 465 (1939 B: the block is retail's but for the one shared index product).
// M3 lane 11 r96 (plain, with 0x21b940, 0x1269f0, 0x125920, 0x126660 MATCH each time; second number the aligned count, control
// 305 / 216): retail reads both short pairs through a pointer to the pair (`lea ..+8` for the pitch bounds, `lea ..+2` for the
// range's minimum/maximum). The range pair through `s_short_bounds const *range = (s_short_bounds const *)&bounds->minimum` 418 /
// 221 (the gain approach's add flips, as with every form that names part of the pitch block); that with the pitch bounds pointer
// 418 / 218; a block-level `long range_count = playback->pitch_range_count;` ahead of the second pitch loop 375 / 193 (frame 0x128
// and the whole update head as retail: first difference +0x39, homes off by 4 or 8); that with a `long count` ahead of the first
// loop too 365 / 216; a `short track_index` loop variable 465 / 230 (1891 bytes). On the second-loop count form the second loop
// still loads the datum index and the track index into EBX/EBP (one use each, in the creation call) and the sounds[i] address into
// EDI with `active` in a slot, where retail holds NONE, track_definition and `active` there and indexes sounds[i] at each use:
// those 14 bytes are what every natural combination listed above is short of.
// P444: first signed-short range ordinal 420f; second signed-short range ordinal 464f; native previous-range value 464f; exhaustive seed result 454f; preparation result 479f; pitch-bounds pointer only 479f; pitch-bounds record lvalue 479f; real request flag field 479f.
// NOT kept: no byte match or supported native frame/result recovery; original complete body restored.
// MiniMax G (11-13): no new form, no lower score (305): the frame is 0x11c against retail's 0x128, and the
// outer index, the definition and the ring homes still differ; the forms in the note below all score 305 or
// more, so the body stands.
// T103: Actual permutation-table input cursor alone, with per-pass counts, sound/gain/
// both local input cursors, gain output, all cursors and complete request storage:
// 466/482/417/436/431/433/515/517 (fast control 305). NOT kept: no byte equality;
// native indexed input observations, request, range/ring homes and full body restored.
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
// score 339 (was 339): the frame is 0x4b4 where retail's is 0x4bc, and the stack slots are not
// a shifted copy of each other: ours has homes at 0x74, 0x78 and 0x94 that retail does not use,
// and retail has 0x64, 0x80, 0x84 and 0x9c that ours does not, so retail carries two more dword
// locals in a different order and the gap cannot be closed by padding;
// retail reads the track value and the source scale before the clear, the one-fill clear and the
// inlined 0x221810 (BLOCKED-BY 0x221810, another file) and the inlined 0x12a420 at the
// stop_channel site remain.
// MiniMax A (11-11): no new form, no lower score (339); the frame is still 0x4b4 against retail's 0x4bc and the
// two slots we use ([esp+0x44] and [esp+0x34]) sit eight bytes below retail's ([esp+0x4c] and [esp+0x3c]), so
// retail carries two more dword locals than we do; retail also reads the track value and the source scale before the
// clear and calls 0x221810 and 0x12a420 out of line, while ours expands both. The helpers live in other files, so
// their call boundaries cannot be moved from here, and the statement walk the earlier rounds recorded found no
// behaviour difference. The original body is kept.
// MiniMax R (11-05): retail's pitch has no `+ lower` term and reads the scale before the clear; the early-scale, bounds-pointer, byte-view, nested and memset forms score 450-545, the shift tests 339: no new form, no lower score.
// MiniMax G (11-04): the pitch bounds read through a `s_short_bounds *` 339, the same bounds with a
// named lower 339, an early `real scale` local 485 and one declared after the bounds 513: no lower score.
// MiniMax E (11-11): no new form tried, no lower score (339): the frame is still 0x4b4 against retail's
// 0x4bc and the two slots we use ([esp+0x44] and [esp+0x34]) sit eight bytes below retail's ([esp+0x4c]
// and [esp+0x3c]), so retail carries two more dword locals in a different order; retail also reads the
// track value and the source scale before the clear and calls 0x221810 and 0x12a420 out of line, and
// those helpers live in other files, so their call boundaries cannot be moved from here and the body
// stands.
// score 339 (was 339): the frame is 0x4b4 where retail's is 0x4bc, and the stack slots are not
// a shifted copy of each other: ours has homes at 0x74, 0x78 and 0x94 that retail does not use,
// and retail has 0x64, 0x80, 0x84 and 0x9c that ours does not, so retail carries two more dword
// locals in a different order and the gap cannot be closed by padding;
// retail reads the track value and the source scale before the clear, the one-fill clear and the
// inlined 0x221810 (BLOCKED-BY 0x221810, another file) and the inlined 0x12a420 at the
// stop_channel site remain.
// Earlier: frame 0x4b4 versus 0x4bc (retail reads the track value and the source scale before the
// clear), the one-fill clear, 0x221810 inlined (BLOCKED-BY 0x221810) and 0x12a420 inlined at the stop_channel site remain.

// MiniMax H (11-12): no new form, no lower score (339); the frame is still 0x4b4 against retail's 0x4bc.
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
// Y499: retained gain-parameter graph, early track-gain/source-scale/source-gain
// input subsets 1-7: 339/467/465/464/462/481/481; all with flat zero aggregate 492.
// Exact-only one-use snapshots all restored; frame remains 0x4b4/0x4bc with scalar homes,
// class-query boundary, clear regions and stop-call expansion unresolved.
// Round 142 (fast; aligned count in brackets, control 339 [257]): the nested base struct with `= {{0}, {0}}` 496 at 1953 [257]; early
// track-gain and scale locals 465 at 1937 [237]; both 502 at 1958 [240]; `primary` after the clear 345 at 1932 [213], with the early
// locals 485 at 1942 [276], with the gain sum ahead of the clear 345 at 1932 [217]; `pitch` after the clear 466 at 1935 [244]; the total
// gain in its own local 349 at 1936 [263]. Retail tests `primary` before the clear, so the 213 is not its shape.
// P304: Native gain/flag-byte/effect views, subsets 1/2/4/3/5/6/7 then readonly modifier:
// 339/411/339/411/339/411/411/411. Flag-byte forms reach 1929/1929 but retain frame 0x4b4/0x4bc and regress.
// Original types/views, receivers and full body restored; early scalar/clear/query homes remain.
// MiniMax G (11-04): the pitch bounds read through a `s_short_bounds *` 339, the same bounds with a
// named lower 339, an early `real scale` local 485 and one declared after the bounds 513: no lower score.
// Lane 13 round 66 (fast; aligned in brackets, control 339 [257]): retail's two extra slots are the track value
// ([esp+0x28]) and the source scale ([esp+0x48]), copied with integer moves before the clear, and it copies the gain
// into its own slot as matched 0x1251e0 does for its `s_decibels` sum. The sum as `s_decibels`, `unknown08 +
// (track_value + s_decibels(gain))` 469 [262] (1940 bytes); written as 0x1251e0 writes it, with `*(s_decibels *)&gain`
// on the right, 485 [234] (1945, retail's gain copy appears); that stored back into the gain parameter 485 [234].
// Diagnostics (one-use captures): the track value and scale read into locals before the clear, with the first sum 498
// [206] (1960, frame 0x4bc), with the 0x1251e0-style sum 503 [201] (1965). Restored.
// b4G (11-06) statement-by-statement walk against retail 0x21c400-0x21cb86: no logic/behaviour difference found (fields,
// offsets, condition directions, callees, argument order, branch shape, float order all agree: unknown08 + (track_value +
// gain) 0x21c590-0x21c5a4, pitch = (upper - lower) * gain + pitch + lower 0x21c53c-0x21c565, doppler (scale * ...) 0x21c687-0x21c69e,
// gain + permutation->gain at 0x21c6f3 and 0x21cb3d; the only fstp dword is the 0x12a9d0 result at 0x21c67d, as ours).
// Width evidence, NOT kept (aligned diff lines 153 at 339): the controller snapshot is byte-wide in retail (mov cl, al /
// and cl, 0x7f at 0x21c752-0x21c757 and the 0xff in cl, as 0x128ca0's `byte local_14`): `char synch_count` makes that
// block exact but scores 405 (1932 bytes; later code drifts, aligned lines 152); a byte + else-arm form 428; the permutation
// index as `short` (retail's word store 0x21c82f, cmp ax, 0xffff at 0x21ca1c) 432 (aligned 189); stop_requested as a byte
// or at the state-else site (retail 0x21c77f `or byte [esi+4], 0x40`, ours caches the flags word) with the char count
// 374 at 1929 bytes. Still open: frame 0x4bc (the two early snapshots, [esp+0x48] scale at 0x21c50e and [esp+0x28] track
// value at 0x21c526), 0x221810 and 0x12a420 inlined by ours (retail calls both at 0x21c62b and 0x21c783/0x21ca5d).
// MiniMax B (11-11): no new form, no lower score (339); the frame is still 0x4b4 against retail's 0x4bc and the
// calls to 0x221810 and 0x12a420 are still expanded here while retail keeps them out of line; both callees are
// declared in files this lane may not edit (src/unknown_221490.cpp and src/unknown_124f90.cpp).
// Round 113 (m3 lane 9, fast with 0x12a420, 0x221810, 0x12a1b0, 0x21c190; 0x12a420's only caller is this function):
// 0x12a420 through a voice pointer local 338 (0x12a420 9, 42 B), as one expression 338 (0x12a420 3); both sites stay
// expanded either way. Retail copies the track value ([esp+0x28]) and the source scale ([esp+0x48]) with integer moves
// before the two-part clear and copies the gain parameter into its own slot for the sum. Re-measured: the gain sum
// above the clear 337. New: that sum as the existing s_decibels operator+ 484 (1945 B); the sum above an `= {0}`
// initialiser 467 (1942 B); the stop flag set after the 0x12a420 call at both sites 418 (1931 B); the effect through
// track_definition->effect 339 (same code); the final 0x21fa80 call passing voice->channel_index 503 (1929 B); the
// controller doppler as if/else 339 (same code); the doppler arms swapped (no controller first) 354.
// MiniMax G (11-13): no new form, no lower score (339): the frame is 0x4b4 against retail's 0x4bc, and the
// early scalar, primary and last-chunk homes still differ; the forms in the note below all score 339 or more,
// so the body stands.
// MiniMax G (11-14), NOT kept (coordinator: the 339 -> 321 comes from a bool set by the loop-flag test and read by
// the next `if` (`flag0`), a form kept only with a byte match; main's `&&` chain already calls "has next sound" only
// when the flag is clear and primary is set). G's two notes on that form follow.
// G's form, not kept: score 321 (was 339): unchanged this round: the frame is still 0x4b4 against retail's 0x4bc and the properties
// aggregate is still cleared in one piece where retail clears the base and the effects separately; taking the
// source scale and the track value into locals early makes our length retail's 1929 bytes but moves every stack
// slot (476), and adding the gain copy with them gives 471 at 1931 bytes; the permuter (964 s, 14 tries) found
// nothing lower.
// G's form, not kept: score 321 (was 339): the loop-flag test now calls "has next sound" only when the flag is clear, which is what
// retail does; the frame is still 0x4b4 against retail's 0x4bc and the early scalar, primary and last-chunk homes
// still differ.
// score 339 (was 339): unchanged this round: the frame is 0x4b4 against retail's 0x4bc, and the early scalar,
// primary and last-chunk homes still differ.
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
// MiniMax S (11-11): no new form, no lower score (597); the entry still hoists the flags byte into BL where retail masks it with 1 after the pushes, and the known remaining deltas are the creation compare's order, the source copy from EDX into ESI, the gains clear in retail's store form and the track loop's register roles; reading `alternate` inside the definition guard reproduces retail's entry but measures 615.
// score 597 (was 597): retail length recovered; saved-register order, record homes, energy-loop
// padding and track/state ownership remain. Round 439: entry tag/handle/record/all-record
// declarations and entry Boolean declarations 716/716/716/716/716 reduced; alternate before
// initial result 694 reduced, 597 plain (kept); independent request state 811 reduced;
// complete geometry output scope 716 reduced (unchanged reduced control 716, full 615).
// MiniMax E (11-05): no new form, no lower score; the alternate mask spelled `(flags & 1) == 1`
// scores 749 and is reverted. Retail's parameter order is ours (arg1 definition index, arg6 state,
// arg7 flags, eight stack args); only the entry's mask carrier differs - ours hoists the raw flags
// byte into BL before the pushes, retail masks it with 1 after them and keeps it in a stack slot.
// At 615: the entry and the homes through the creation call still differ, the gains
// clear uses retail's store form (retail stores [ebp+0xc4..] from eax), the energy loop keeps its
// padding jump in retail, the track loop holds the state in EBX and the track in EBP, and the
// impulse bits are still tested twice unmerged with the field_c_4 = 0 reload.
// At 615: entry and homes retail's through the creation call; the gains clear (memset's
// lea form, retail stores [ebp+0xc4..] from eax), the energy loop's padding (jmp + 8 bytes in retail), state held
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

// MiniMax H (11-12): no new form, no lower score (597); the update block's register and slot layout are unchanged.
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
// Round 442: alternate inside definition guard, combination result set by its test, creation truth
// from the lookup result, seven subsets 716/739/725/752/743/758/749 reduced; complete truth graph
// with alternate inside availability 791 (loses a caller). Reduced control 694, full 597.
// Original truth producers and body restored; entry flags occupy BL instead of retail's true EBX.
// Lane 18 round 17 (an LTCG link of this file's generated unit, 86 objects, equal to the full listing; 611 there for
// 615): the gains cleared by four `= 0.f` stores 641 at 2516 bytes (retail's direct [ebp+0xc4] stores, but from
// xorps/movss where retail stores eax = 0 as integers, so the clear is not a float store); memset with
// `4 * sizeof(real)` 611 (identical); `(definition->flags & 0x80) != 0` 611; `created = true` before the creation
// call 611; re-measured: memset of &listener_gains[0] 611 (listed above as memset on &listener_gains).
// Round 498: Alternate computed after source update/inside track phase/after acquisition,
// bool/short per-track carriers 730/744, 786/743, 782/742; direct parameter bits at both
// consumers 794/736 (exact-only). Reduced control 694, full 597. Frame/register and record/track
// homes remain; original parameter-derived producer, widths and body restored.
// Round 502: complete reused native class-word/request-mask payloads, short/word
// and byte/word combinations 694/694/702/694/702/702/694/694 against reduced control 694.
// Original parameter observations and complete body restored; entry/result and record/track homes remain.
// Round 289: native previous/next transition carriers char/long, short/long, long/char,
// long/short, char/char, short/short, char/short, short/char: 669/644/626/657/645/651/658/655.
// Original state graph restored; entry truth ownership and record/track homes remain.
// Round 248: Existing find/create boundary, reused lower-pitch value and plain energy-input cursor,
// seven subsets then both gain-input cursors: 668/597/668/680/745/680/745/723.
// Original late lookup/create pair, pitch observations, indexed gain loops and complete body restored.
// P252: Fixed four-gain ordinal as short/word/byte/signed byte 722 each; the same four with the complete
// normalization scale produced after energy 746 each (unchanged reduced control 694, full baseline 597).
// Original ordinal, scale lifetime and complete body restored; entry truth, record/track homes and loop
// alignment remain.
// Round 473: Native impulse-mask byte/word/signed-byte/short domains, then
// paired alternate-mask domains: 682/669/698/689/684/641/701/687.
// Reduced control 694; full 597. Plain paired-word/word/byte/paired-byte
// 658/661/672/675; all restored; entry truth and record/track homes remain.
// Round 308: native persisted sound/outer-tag/track-tag flag domains, seven subsets then guarded alternate,
// Round 308: 740/748/647/795/716/740/786/772 reduced (control 694); track fields alone plain 643,
// Round 308: against full 597. All real record declarations and executable body restored; entry true/flags
// Round 308: and record/track ownership remain.
// Round 370: Actual byte/char/word/short request-flag formals 617/617/601/601;
// paired with signed-word class payload 617/617/601/601 (fast control equals full 597).
// All three declarations and original body restored; entry true/flags, record/track homes and loop padding remain.
// DL245: Frame 0x3c and retail length agree; entry true/flags ownership, record/track homes and energy-loop alignment remain. Seed conditional value with bool/byte/short/word track truth 613/613/644/642; two explicit seed arms 613/613/644/642. Both forms regress; all RNG state identities, original seed producer, truth types, transitions, gains and body restored.
// F336: Paired initial/replacement state publication from their actual selected values,
// char/char, short/short, long/long, dword/dword, char/short, short/char, long/char, char/long:
// 694/693/693/693/690/697/697/690 (fast control 694, full 597). NOT kept: at most four reduced points;
// the wider forms also shorten the reduced extent farther from retail. Complete original body restored.
// M332: Actual attenuation distance/limit/floor scalar lexical identities, seven subsets,
// then complete normalization identities: 694/694/694/694/694/694/694/694 reduced, tying control 694 (full 597).
// Distance/maximum homes and entry true/flags remain; original complete body restored.
// N342: Terminal end/impulse truth pair byte/char/short/long 694/694/682/688 reduced;
// ordinary-end complements 694/694/682/688 (control 694, full 597). NOT kept: wider
// predicate storage has no byte match; native entry true/flags and record/track homes remain.
// F349: Existing source-record flag fields at copy/attenuation/release consumers,
// subsets 1..7, then immutable attenuation tail: 694 each (fast control 694, full 597).
// Original aliases, tail and full body restored; entry true/flags and record/track homes remain.
// B387: Existing byte-flag writer at all three final publication sites, seven subsets,
// then all with masked native truth arguments: 694 each (reduced control 694, full 597).
// Original complete body restored; entry true/flags, record/track homes and loop padding remain.
// P298: Independent energy/output ordinals, two do/do identities 597/597;
// for/do, do/for, for/for and while counterparts 672/724/769/672/724/769.
// Original loops and complete body restored; entry true/flags and record/track homes remain.
// M343: Complete pending-valued return graph, bool/byte/short/long direct initial state test
// 668/668/799/793; default-true state-test counterparts 755/755/800/794.
// All regress; original completion polarity, sole exit and complete body restored.
// NOT kept: all outcomes regress; wider domains grow the frame to 0x40.
// Lane 14 r57 (plain, from 597): `alternate` declared inside the definition guard 615 at 2500 bytes (the entry is
// then retail's to +0x6c: result first, EBX = 1 shared by the alternate mask and the true stores; the source copy
// keeps the pointer in ESI where retail copies it from EDX, which shifts the energy loops' padding); with `distance`
// renamed to sort after maximum/minimum 615 (no home moves); inside the availability block 596 at 2518 bytes; with
// distance declared before maximum 596; after the definition lookup 602.
// M343: Complete pending-valued return graph, bool/byte/short/long direct initial state test
// 668/668/799/793; default-true state-test counterparts 755/755/800/794.
// All regress; original completion polarity, sole exit and complete body restored.
// NOT kept: all outcomes regress; wider domains grow the frame to 0x40.
// J246: Actual lookup-missing/existing outcomes own dispatch and track creation,
// bool/byte/short/long channels: 740/740/736/748 and 740/740/736/748.
// NOT kept: all regress at 2518 bytes/frame 0x3c; entry flags/true and record/track
// homes remain. Original branch-owned creation flag and complete body restored.
// J249: Native source transfer through memcpy at entry/definition guard/availability guard/
// after lookup 597/615/596/602; corresponding memmove 734/720/765/765, losing matched
// 0x18a7b0 (117/117/124/124) and raising 0x21d630. NOT kept: the one-point memcpy gain
// is below the gate; overlap-safe transfer disturbs the native entry and caller contract.
// Coordinated __stdcall on the original/native-guard graph 597/615: same code curves.
// Original full body, transfer, lifetimes and all three declarations restored; native
// true/flags, record/scalar homes and energy/track ownership remain.
// B406: Completed acquisition supplies admission and return, bool/byte/short/long
// success 749/734/778/782; complementary pending outcomes 732/732/775/780. All regress;
// original branch-owned result, persistent input truth and complete body restored.
// B408: Actual primary definition/sound/input record reference subsets 1..7: 694f each;
// complete bindings with overlap-safe source transfer 780f (control 694f, full 597).
// NOT kept: entry true/flags and record/track homes do not recover; transfer grows frame
// to 0x40. Original roots, copy, qualifiers and complete body restored.
// P306: Actual track/track-tag/pitch-entry references, seven subsets, then read-only
// selected tag/pitch receivers: 694 each, tying reduced control (full 597). NOT kept:
// no gain or match; original body restored. Entry true/flags and record/track homes remain.
// M353: Complete per-track availability bool/byte/short/long conjunction results
// 680/680/671/680; conditional values 679/679/685/685 fast (control 694; full 597).
// NOT kept: retail first copies alternate, then clears only the unavailable path;
// complete-result stores differ. Original producer, widths and whole body restored.
// Y581: Raw zero/one alternate mask, mutable byte/word/long/dword
// 674/749/675/675; immutable counterparts 674/749/675/675 reduced (control 694,
// full 597). NOT kept: frame grows from 0x3c to 0x40/0x44; entry true/flags and
// record/track homes remain. Original Boolean producer and complete body restored.
// J257: One complete repeated stopping/continuing classification across result, release and track tests, bool/byte/short/long:
// 729/761/790/780/761/761/790/780. NOT kept: no byte match; shared terminal classification does not recover native entry truth and record/track ownership.
// Original executable source restored; allocation gaps remain.
// DH371: Actual definition/system/copy/listener/normalization/release/track-init/start
// Boolean guard results: 694/694/734/694/694/694/694/670 reduced (control 694, full 597).
// NOT kept: unmatched one-use outcomes, including the reduced-only 670; original complete
// body restored. Entry true/flags, record/scalar homes and track/state ownership remain.
// N398: Coherent byte/char/word/short/long/dword/int/uint returned truth and result carrier; reduced control 694, full 597 694/694/810/810/815/815/815/815.
// NOT kept: no byte equality or eligible native-shape gain; original complete body restored.
// M3 lane 4 r167 (plain; rank on the register-masked aligned count, 102 for the kept 597): alternate read inside the definition
// guard as bool 615 (2500 bytes, 767 instructions as retail, masked 89, first difference +0x1c: retail's `mov ebx, 1` hoisted constant and
// flags in memory appear; a score rise, not kept); as a byte 698 with combine_sources/lookup id as if/else (2492 bytes, masked 91, frame
// 0x40); read through the source-flags bitfield view on flags 615, with the two later flags tests through it too 615 (masked 90); if/else
// combine_sources alone 630 (2499 bytes, masked 94). One-file link (reduced, 694 for 597; aligned/shape): gains clear through a zeroed
// 16-byte local struct assigned to the field, reals or longs, 721/729 (direct [ebp+0xc4] stores from a zero register, as retail); chained
// `= 0.f` 744; memset through a real pointer or a byte offset 717 (same as the alternate form). Next: the bool form's masked list
// (index/created compare order, source in EDX copied to ESI for the rep movs, the track loop's per-iteration homes).
// Round 154 (m3 lane 5, plain). On the definition-guard alternate (615, 2500 bytes; its 18 missing bytes are retail's
// `mov esi, edx` before the copy and the gains clear, 7 bytes shorter as memset's `lea` form): track before track_definition
// 615; the gains cleared by four `= 0.f` stores 643 (2518 bytes, but xorps frees EAX and the next block's registers rotate);
// the impulse bits tested from the field at each of the four sets 689 (2494 bytes, first difference +0x2); `if (index ==
// NONE) result = false` in the creation arm in place of the final `else result = false` 711. On the original: alternate
// inside the availability block with track before track_definition 596 (2518 bytes). In the track loop retail holds
// `state` in EBX at the loop top (reloaded at the latch, stored to sound->state from BL), NONE in ESI, track in EBP and the
// constant 1 in EBX across the start arm (`test bl, cl`, `push ebx`, `mov [edi + 0xae], bx`); ours holds NONE in EBX and
// track_definition in EBP and spills track and state, and the start arm's field_c_4 = 0 is stored and reloaded in retail.
// w14K (11-06) statement walk against retail (fast with callers and callees: 597, the full score): every field offset,
// condition direction, callee and argument, loop and switch shape agrees; no logic error found. One behaviour difference
// stays: on the availability-fail path (0x21a289/0x21a294/0x21a29f -> 0x21ac18) retail returns the stack byte [esp+0x11],
// which that path never writes (0x21a2f6 and 0x21ac13 write it later), while `state == 2` is stored to [esp+0xd] at 0x21a261
// and never read. So retail has a separate `bool stopping = state == 2` and an unset `result` there; ours returns state == 2.
// Written that way (`bool stopping = state == 2; bool result;` with the two later `state == 2` tests via `stopping`) it
// reproduces the dead store and the 0x11 read: 645 (reduced 679), with `alternate` declared inside the definition guard
// (retail computes it after 0x21a26a) 637 / reduced 671 at 2500 bytes. Not kept: it returns an uninitialised bool and the
// length moves off retail's 2518. Tried (full-like list): the track-transition test as `!looping_sound_track_has_next_sound(
// track, track_definition)` 673 (not inlined, `call 0x21a050`; retail has none), the old-end fade as function_126df0(1,
// NONE, old_end, duration) 739 (a call; retail inlines its fade-out half), looping_track_get(index, i) 704, the source flag
// bits through s_type_99c531's own flag1..flag3 instead of the s_looping_source_flags view 597 (same).
// Lane 12 r102 (plain): `distance` renamed `source_distance` 597 and declared before `maximum` 597 (identical code:
// retail keeps maximum/minimum in the dead `source` slot 0x60 and the distance in the `sound_flags` slot 0x54, ours the
// other way round; neither the name nor the declaration order moves them); alternate inside the definition guard 615 (as
// lane 4 r167: retail's code to +0x1c, then the creation compare comes after the two stores and `source` rides in ESI
// where retail loads it into EDX and copies it to ESI), with the source copy as memcpy 615 (identical); alternate inside
// the availability block alone 596 (2518 bytes, first difference +0x1c).
// M3 lane 10 r89 (plain; positional, stack-masked/register-masked aligned 307/163 at 597): retail materialises
// the constant 1 in EBX after the definition test (`mov ebx, 1; and al, bl`, then `mov [result], bl`, `mov eax,
// ebx; shl eax, cl`, `state = 1` from EBX) where ours keeps the flags byte in BL from the entry. With alternate
// inside the definition guard (re-measured 615 at 2500 bytes, 767 instructions as retail, 293/148), EBX = 1 is
// retail's; on that: `created = true` before the creation call 615 (lane 18's form, 611 on the old base); `bool alternate = flags & 1;` 615; `result =
// true` before the lookup call 620; `created` declared before the lookup 619; the source through a const local
// for the flag test and the copy 615. Left on the guard form: the creation compare after the two stores, source
// loaded into ESI where retail uses EDX then copies it, the gains clear in memset's lea form, state in EBX and
// NONE in ESI at the track loop (ours NONE in EBX).
// Lane 14 r81 (fast with callers and callees, control 597 as the full check; second number the register-masked
// aligned count, 197 for 597): the two impulse truths as dword values 598 (197); impulse_states read after the two
// truths 598 (2508 bytes, 195). Re-measured: alternate inside the definition guard 615 plain (2500 bytes, as listed;
// the fast image gives 716 at 2455 bytes for it, so screen guard forms plain); the four impulse sets through
// flag_set_12a1b0 597 (same code); the four sets through the existing bit_vector_set (the mask is a per-state bit
// vector) 617 (2544 bytes, 300); diagnostic, reverted: the gains cleared by assigning a zeroed four-real aggregate 727
// (still `lea` plus a stack copy, not retail's direct stores). Retail reads the
// track flags twice (a byte for `& 1`, a dword for the shifted bits) and tests each truth twice without threading;
// every form here threads the second test into the first.
// Round 111 (m3 lane 9, fast with callers, callees and the file's other availability users, control 597): the availability
// test through the real sound_system_available() (0x125a60, which retail inlines here) 597 (same code); re-measured on this
// base (lane 13 r24's bounds pointer): the replacement pitch through matched sibling 0x21b940's `s_short_bounds *bounds` with a
// named lower 597, without it 597 (register-masked aligned 199 -> 198); the four impulse updates as conditional assignments 597
// (same code); both gain loops as
// plain for loops 769 (2596 bytes); the maximum or the minimum as a default with an `if` override 620 each; same code: the
// minimum clamp as `if (!(minimum > 0.001f)) minimum = 0.001f;`, the maximum as an if/else. Diagnostic only (the private availability copy is shared with contributor code): that copy
// reshaped to 0x125a60's `if (...) return 1; return 0;` gives the same code. Left: the track loop's registers (retail state EBX,
// NONE ESI, track EBP, index and track definition in memory; ours NONE EBX, index ESI, track definition EBP).
// Round 113 (m3 lane 9, plain; masked = register- and stack-masked aligned count, 163 at 597): the final listener test
// through a pointer to the sound's source 596 (retail reads the tail maximum at [eax+0x38] off the pushed source pointer;
// this gives that line, under the keep bar); that with the listed `alternate` inside the definition guard 615 (a
// re-measure of its score; 2500 B, masked 147);
// the start block's `field_c_4 = 0; next_state = field_c_4;` written at the end of each arm (the compiler merges them
// back) 658 (2506 B, masked 127: the track pointer then takes EBP as in retail, track_definition ESI, NONE EBX); all three
// 681 (2490 B, masked 113; left: the gains clear in memset's lea form, state not reloaded into EBX at the update test,
// the start arm's constant 1 not in EBX, the alternate compares as int neg/sbb where retail compares bytes, the impulse
// bits threaded); the one-use start_with_fade bool written into its test 648 (2506 B); one source pointer for every
// source use in the index block 770 (2484 B); the arm resets with the final-test source pointer 658 (2506 B, masked 126).
// MiniMax G (11-13): no new form, no lower score (597): the track loop's registers still differ - retail holds
// the state in ebx, NONE in esi and the track in ebp with the index and the track definition in memory, while
// this build holds NONE in ebx, the index in esi and the track definition in ebp; the forms in the note below
// all score 596 or more.
// P459: Independent update-on-start and two publication request-bit outcomes, seven subsets and all fixed Boolean producers:
// 735/767/781/769/780/777/776/776. NOT kept: native true/raw-flags ownership and record/track homes not recovered; original complete body restored.
// @retail 0x21a250
bool function_21a250(long definition_index, dword sound_flags, long identifier, long controller_definition_index,
	s_type_99c531 *source, long state, dword flags, real fade_duration)
{
	bool alternate = (flags & 1) != 0;
	bool result = state == 2;
	if (definition_index != NONE)
	{
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
