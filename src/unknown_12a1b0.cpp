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
#include "unknown_21e230.h"
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

// score 80 (was 93): The frameless prologue saves the wrong index register. The spatial query expands and the loop carries an element pointer.
// Earlier measurement 80 (was 93): the location query 0x11bed0 is still expanded here where retail calls it out of line, and the
// walk keeps the element pointer where retail keeps a packed datum index; a pool local with the typed stride, the
// inlined next-used scan and a sign test on the slot index reaches 80 (222 bytes); no one-use local here.
// Earlier at 93 (was 93): Called location-query boundary and native datum/element homes remain; restore the complete opening executable body.
// Earlier measurement 93 (was 93): Called location-query boundary and native datum/element homes remain; restore the complete opening executable body.
// MiniMax G (11-20): no new form, no lower score (93): 0x11bed0 is still expanded here where retail calls it out of line.
// MiniMax H (11-19): no new form, no lower score (93): the location query 0x11bed0 is still expanded here where retail calls it out of line, and its inline eligibility is fixed in src/unknown_11bed0.cpp, which this lane may not edit.
// MiniMax I (11-19): no new form, no lower score (93): the location query 0x11bed0 is still expanded here where retail calls it out of line, and its definition is in src/unknown_11bed0.cpp, which this lane may not edit; the one form that reaches 0 needs the redundant `&& data && sound` pointer tests, which the redundant-test ruling rejects. BLOCKED-BY 0x11bed0.
// MiniMax E (11-15): no new form, no lower score (93): the call boundaries agree with retail's except that 0x11bed0 is expanded here where retail calls it out of line, and its definition is in src/unknown_11bed0.cpp. BLOCKED-BY 0x11bed0.
// MiniMax D (11-15): no new form, no lower score (93): a hand-written index walk (the pool in a local, the datum
// index, the 0xd4 stride and data_next_absolute_index_inlined) was measured at 156 (277 bytes against retail's 217)
// and dropped: with the location query expanded here the walk is longer than retail's, and that helper's inline
// eligibility is decided in src/unknown_11bed0.cpp, which this lane may not edit.
// MiniMax E (11-16): no new form, no lower score (93): the location query 0x11bed0 is still expanded here where
// retail calls it out of line, and its inline eligibility is fixed in src/unknown_11bed0.cpp, which this lane
// may not edit, so the call boundary and the register homes that follow it stand. BLOCKED-BY 0x11bed0.
// MiniMax C (11-14): no new form, no lower score (93): the location query is still expanded here and the walk still keeps the iterator struct where retail keeps a packed datum index, so the call boundary and the register homes behind it stand; the callee's inline eligibility is fixed in another file.
// MiniMax H (11-12): no new form, no lower score (93); 0x11bed0 stays expanded in every form tried, so retail's call is not caused by our spelling of it.
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
// Earlier at 93 (was 93): Called location-query boundary and native datum/element homes remain; restore the complete opening executable body.
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
// Earlier at 93 (was 93): unchanged this round: the location query is still expanded here while retail calls it out of
// line, and that callee's inline eligibility is fixed in another file. BLOCKED-BY 0x11bed0.
// Earlier at 93 (was 93): unchanged this round: the location query 0x11bed0 is still expanded where retail calls it
// MiniMax H (11-14): no new form, no lower score (93): the location query 0x11bed0 is still expanded here while
// retail calls it out of line, and its inline eligibility is fixed in src/unknown_11bed0.cpp, which this lane may
// not edit, so the call boundary and the register homes that follow it stand. BLOCKED-BY 0x11bed0.
// MiniMax E (11-17): no new form, no lower score (93): the location query 0x11bed0 is still expanded here where retail keeps it out of line, and its definition is in src/unknown_11bed0.cpp, which this lane may not edit (BLOCKED-BY 0x11bed0).
// Earlier at 93 (was 93; a second score line, merged in round 104): unchanged that round: the location query 0x11bed0 is still expanded where retail calls it
// out of line, and its inline eligibility is fixed in src/unknown_11bed0.cpp, which this lane may not edit.
// M3 lane 7 r161 (fast with 0x11bed0/0x11be90/0x14a280/0x1d48f0/0x125920/0x1e1500/0x1697c0/0x17c0e0/0x16bc00): round 88's
// index walk (0x21cf80's spelling) as a guarded do-while 161 (281 B), the same code as the while walk. 0x11bed0 is
// expanded only at our two hottest sites (here and 0x1d48f0); no unlisted caller or callee form found. BLOCKED-BY 0x11bed0.
// MiniMax S (11-14): no new form, no lower score (93): the location query 0x11bed0 is still expanded here while
// retail calls it out of line, and its inline eligibility is fixed in src/unknown_11bed0.cpp, which this lane may
// not edit, so the call boundary and the register homes that follow it stand. BLOCKED-BY 0x11bed0.
// MiniMax I (11-16): no new form, no lower score (93): the location query 0x11bed0 is still expanded here while
// retail calls it out of line, and its inline eligibility is fixed in src/unknown_11bed0.cpp, which this lane may
// not edit; the one form that scored lower (83) needs a one-use pointer to the call's result, a banned form.
// BLOCKED-BY 0x11bed0.
// MiniMax H (11-18), NOT kept (coordinator): a datum walk with a pool local reached 0 only with `&& data && sound`
// added to the audible test, pointer tests implied by the reads before them that steer LTCG to call 0x11bed0 out of
// line; the 10-30 redundant-test ruling rejects that form (as lane 3 r166's same match, 11-04). Body back at 93.
// MiniMax C (11-17): no new form, no lower score (93): the location query 0x11bed0 is still expanded here while retail calls it out of line, and its inline eligibility is fixed in src/unknown_11bed0.cpp, which this lane may not edit, so the call boundary and the register homes that follow it stand; the one form that scored lower (83) needs a one-use pointer to the call's result, a banned form. BLOCKED-BY 0x11bed0.
// M3 lane 2 r104 (fast with 0x11bed0/0x11be90/0x14a280/0x1d48f0/0x125920/0x1e1500/0x1697c0/0x17c0e0/0x16bc00/0x16bbc0,
// control 93; 0x11bed0 stays expanded in all nine): round 88's index walk re-measured 161 (281 B); on it the type test
// and the call inside a static inline helper taking the source 160, taking the sound 161, the same helper as a plain
// `inline` 160 (the decision is taken after the helper is expanded into the loop, so a helper does not make the site
// colder); that helper on the native iterator 93 (same code). One step site, `long index = NONE; while ((index =
// step(data, index)) != NONE)`: through next_sound_datum 88 (207 B: the scan is then a call at the latch too, a
// shorter body further from retail, aligned 101 as the control), through the real 0x16bbc0 139 (155 B, it stays a
// call), through an inline step with the inline scan 127 (258 B: the header is not duplicated, so retail's folded
// first step needs two sites); the real 0x16bbc0 for the first step with the inline scan at the latch 118 (249 B).
// BLOCKED-BY 0x11bed0's inline decision: with it out of line round 88's walk is retail's code.
// Skip ineligible entries before each update, update consecutive eligible runs,
// seek before entry and at the latch, or skip at entry and after updates:
// element cursor 206/215/227/225; salted datum cursor 311/318/353/355.
// NOT kept: the additional scan paths increase the score and leave the
// location query expanded. Original traversal restored.
// Shared bitmap latch 80, shared entry and latch or inlined entry 126/126,
// packed entry via pool or sound helper 97/118, sound helper with shared latch
// 118, and shared bitmap absolute while/for 88/90. NOT kept: the latch ties,
// and the other forms raise the score without restoring the location call.
// Absolute slot with signed or sentinel guard 88/90; sentinel guard with pool stride 92; element traversal with pool stride 78.
// Signed audible interval or separate low and high nibble tests on the typed cursor 83/81, salted cursor 164/167.
// NOT kept: the pool-stride form gains two points, and the spatial query still expands.
// M3 lane 6 r239 (fast with 0x11bed0, its other callers, 0x16bc00/0x16bbc0 and the walk siblings; control 80): the type
// test and the 0x11bed0 call in a new plain helper taking the source, on 0x219d90's walk, 89 (197 B: the walk becomes
// retail's handle loop, but this /Ob1 file keeps the helper out of line); that helper `static` 89; defined in the location
// query's /O2 file 160 (281 B: the link expands the helper and then 0x11bed0 at the loop site); `__forceinline` here 160;
// the /O2-file helper on the current body 80 (same code). Our link expands callees from /O2 files and never from /Ob1 ones,
// the reverse of retail for 0x11bed0 and 0x16bbc0. Still BLOCKED-BY 0x11bed0.
// @retail 0x21a100
void looping_sound_update_locations(void)
{
	s_record_pool *data = LOOPING_SOUNDS;
	long index = function_16bc00(data, 0);
	s_type_5ef569 *sound = 0;
	if (index >= 0)
		sound = (s_type_5ef569 *)data->data + index;
	while (sound != 0)
	{
		if (sound->source.audible == 1)
			function_11bed0(&sound->source.spatial.location, &sound->source.spatial.position);
		index = data_next_absolute_index_inlined(data, index + 1);
		sound = index >= 0 ? (s_type_5ef569 *)data->data + index : 0;
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
	char unknown07;
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
		iterator->index = links->next_sound_index;
		if (links->next_sound_index == iterator->first)
			iterator->index = NONE;
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

struct s_animation_state
{
	byte unknown00[4];
	char a;
	char b;
	word c;

	void function_2192b0(s_animation_ref *ref, byte value);
	void function_219310(s_animation_ref *ref, short value);
};
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
short function_219370(s_animation_state *state, s_animation_ref *ref);

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
		((s_animation_state *)set)->function_2192b0((s_animation_ref *)playback, (byte)permutation);
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
// score 2 (was 0): matched until 2026-11-24, when the ring's shared step became store-then-test (the user's trade:
// 0x21be20, 0x21b870 and 0x21b0e0 match with it). Left: the two reloads after the 0x12a810 call, ours the first index
// (`mov ecx, [esp + 0x24]`) then the count (`mov edx, [esp + 0x18]`), retail the count then the first index.
// M3 lane 14 r154 (reduced image with 0x126df0, 0x12a810, 0x21be20 and 0x21b870; the three ring users MATCH at every
// run). Cause: the count and the first index both lose their registers across the call and are split around it; each
// split piece takes the next free candidate number, and the two reloads come out in piece-number order. The free
// numbers are those of values that died before allocation: the float constant and the duration, the fade's inlined
// index copy, the step's next-link value and the fade's element. With the old one-store step the next-link value
// stayed a value of its own, so the pieces were 14 for the first index and 12 for the count; with this step it is
// merged into the index and its number (21) is handed to the count ahead of 12. Three constants (NONE, zero and the
// 0xffff mask) are split ahead of the first index and take the first three free numbers; one of those more or less,
// one short-lived constant or float more or less, or the next-link number freed in another place would give
// retail's order. Forms on this step, 2 with the same piece numbers unless noted: the promotion pointer, the
// definition or the playing element unnamed 2/2/2; `short` promotion index 9 (287 B); the flags word named 2; the
// curve as a conditional expression 2, preset true and cleared under the flag 29; the definition index named 2; the
// element through the track view 2; the curve cast at the call 2; the index declared ahead of the iterator 2; a
// const promotion pointer 2; the promotion index unnamed 43; the duration copied to a local 2, read through its own
// address 2; the promotion accessor written out, with or without its table local 2/2; a named copy of the index for
// the fade call 2; the iterator built from a named copy of the first index 2; an early-return guard 2; the loop on
// the step's result with the body's index read from the iterator 2; `long` curve 44; the flag test as a shift and
// mask (four spellings), as `== 0` or through a bitfield member (two spellings) 2 each; no index local 2; the call or
// the lookup alone through `iterator.index` 2/2; the index declared in the loop body 2. In the index forms only the
// fade's index copy moves in the free order, never the next-link number. Later the same round: the curve converted
// into a named short for the call 2; the promotion index narrowed into a named short for the lookup 9 (287 B), with
// the named curve 9; the fade call's index or the looked-up index copied from the iterator inside the loop 2/2; the
// promotion byte named as a char and then widened 2 (each named copy moves one free number, the count's piece stays
// 21). The shared step with the wrap test in a bool ahead of the store, or with a copy of the index returned ahead of
// the count step: 2 with the same pieces (0x21be20 and 0x21b870 MATCH); the next link read through `long const &`
// (from the links pointer or straight from the lookup) 13/13 (0x21b870 7, 0x21be20 10); the compare written `first ==
// next` 3 (0x21b870 1, 0x21be20 1).
// Earlier rounds' forms on this step, moved here from the notes of 0x21be20 and 0x21b0e0 when those matched (in the
// first four paragraphs "this" is 0x21be20, in the last two "this body" is 0x21b0e0):
// M3 lane 6 r232 (h2m fast with 0x126df0, 0x12a810, 0x21bd00 and 0x21b870; control 24): the shared step spelled `index = next;
// if (next == first) index = NONE;` (one helper for all four users) gives this function MATCH and 0x21b870 MATCH, but 0x21bd00 2
// (its two latch reloads, count then first in retail, come out first then count). On that step, 0x21bd00's score: the step
// `index = NONE; if (next != first) index = next;` 64 (this 45, 0x21b870 33); 0x21bd00 without its duration pointer 2, with an
// early return 2, reading `iterator.index` in the body 2, without the guard 96; its parameter or the iterator's count or first
// field renamed to sort first or last 2 each; the step's two tests nested 2, `NONE != index` 2, `count = count + 1` 2; its body
// with the header lookup first 55, the curve written at the call 28, the playback lookup folded into the definition lookup 2;
// 0x126df0's fade-out half with `1000.0f * duration` 2, the curve and start stores swapped 4, the start and end stores
// swapped 4 (0x126df0 2 each); `__assume(first_sound_index != NONE)` inside 0x21bd00's guard 2; its guard as `NONE !=` 2; the
// iterator initialised count, index, first 4 (this 2, 0x21b870 2); the iterator laid out count, first, index or first, count,
// index 3/3 (this 2: only the count slot's displacements move, the reload order stays first then count). Restored.
// M3 lane 3 r300 (h2m fast with 0x126df0, 0x12a810, 0x21bd00 and 0x21b870; control 24). On round 232's shared step S1
// (`index = links->next; if (links->next == first) index = NONE;`: this MATCH, 0x21bd00 2, its count and first-index reloads
// after the fade-out call swapped), this / 0x21bd00: the compare written `first == next` 1/3; the step as an if/else (`== first`
// arm NONE) 24/MATCH; the stored field compared 47 (201 B)/71; a whole-sound pointer read twice 47 (208 B)/52; a copy of the
// index returned ahead of the count step MATCH/2; the wrap test in a bool ahead of the store MATCH/2; an early return in the
// stepped arm MATCH/2; `count++ > 0` 34/57; the index tested ahead of the count 4/6; the first field typed `dword` MATCH/2;
// iterator set-up orders index-first-count 2, index-count-first 2, count-first-index 4, first-count-index 4, a chained
// `index = first = ...` 2 (0x21bd00 each). 0x21bd00 bodies on S1: index declared first 2; the duration pointer inside the
// guard 2; iterator and index at function scope 2; `short curve` 28; the playing lookup through iterator.index 2; the curve
// test passed straight 28; the curve as `(flags & 0x400) == 0` 2; the call given `*reference` 2; `char` promotion index 10;
// the header as an if-set local 43; the playing lookup folded into the definition lookup 2; the promotion read at the
// curve 2; diagnostics 1-4 dead longs 2 each and 1-3 paren pairs on the curve 2 each (not a phase). 0x21b0e0 305 -> 418 on
// S1. LEAD, HELD (twin form): S1's step written out by hand in this body (`for (;;)` with the step, then `break` on NONE),
// the shared helper unchanged: this, 0x21bd00, 0x21b870, 0x126df0 and 0x12a810 all MATCH (check.py --fast too), 0x21b0e0
// 305 unchanged; not kept, a per-site spelling of the shared step.
// M3 lane 4 r255 (h2m fast with 0x21bd00, 0x21b870, 0x126df0 and 0x12a810; control 24). S1 re-measured: this MATCH,
// 0x21b870 MATCH, 0x21bd00 2 (only the latch's two reloads after the 0x12a810 call: retail the count `[esp+0x18]` then
// the first index from the parameter's home `[esp+0x24]`, ours the reverse), 0x21b0e0 305 -> 418 (so S1 alone also
// raises that user). On S1: the next index named first (`next = ...; index = next; if (next == first) ...`) 47 (201 B;
// 0x21bd00 71, 0x21b870 32); 0x21bd00's guard on the iterator's own first field after it is built 0x21bd00 77 (this and
// 0x21b870 MATCH); the count stepped first (`long count = iterator->count++; if (count > 0 && ...)`) 23 (213 B; 0x21bd00
// 51, 0x21b870 16). In 0x126df0 (file free this round) the fade-in gain as one conditional expression 37 (205 B;
// 0x126df0 15). Restored.
// M3 lane 4 r257 (h2m fast with 0x126df0, 0x12a810, 0x21bd00, 0x21b870 and 0x21b0e0; control 24). Measured ranking:
// the allocator colours the index ahead of the element, 166 against 144, and the first one coloured takes ESI. 120 of
// the index's 166 come from the shared step's stepped arm (five references to it in a block that touches six values:
// the mask and the multiply are done on the index itself); the element has 80 from the stores after the gain's join
// and 40 from the lookup block. On equal weight the element (created later) would go first, so 22 more for the
// element or 22 less for the index is the whole difference; one more value referenced in the post-join block would
// give exactly 164 each. The store-then-overwrite step gives the index 98 (three references in the stepped arm, the
// lookup done on a copy), which is why this function matches with it. On that step 0x21bd00's two rows are a
// scheduling tie: its two latch reloads have equal height and come out in the order of their split pieces' numbers;
// with main's step the count's piece takes the lower free number, with the other step the first index's does
// (main's next-link temporary keeps one number busy that the other step frees), and no spelling below moves that.
// Forms: the store-then-overwrite step with the lookup written without the by-value helper MATCH (0x21bd00 2, same
// code as with the helper), main's step with it 24; on the store-then-overwrite step, 0x21bd00's body with the
// promotion index unnamed 43, the promotion-table accessor written out without its table local 2, a for loop whose init and
// step are the next call 116 (318 B), an endless loop with its own exit 2, a do-while around a guarded body 2, no
// index local outside the loop (a body copy of iterator.index) 2. Main's conditional step with no links local 54
// (207 B; 0x21bd00 71, 0x21b870 32), with a named next link 54 (same code), with a whole-sound pointer 46 (208 B;
// 0x21bd00 52, 0x21b870 8, 0x21b0e0 418), the compare written `first != next` 24 (0x21bd00 1, 0x21b870 1), a named
// next link and no links local 54. 0x21b0e0 with the store-then-overwrite step: 418, aligned rows 167 -> 153. Restored.
// M3 lane 3 r293 (h2m fast with round 139's sixteen addresses; control 305). With the file's one ring step (the second
// step spelling is a held twin), round 139's natural parts alone: the sounds array by memset 335; the request's view at
// 0xa0 305 (frame 0x12c); the pitch through a bounds pointer and `lower` 304; a count local per pitch-range for-init 368;
// the active bit at each use 442; the char NONE store 426 (1907 B); memset + 0xa0 335, + lower 410, + counts and the bit
// 442, + the char store 331 (frame 0x128, first +0x39). The shared step itself respelled (`index = next; if (next ==
// first) index = NONE;`, all four users): 0x21be20 MATCH, 0x21b870 MATCH, 0x21bd00 2 (its latch reloads count and first
// in the other order), this body 418, with the natural parts 268 (1907 B), 311 without the char store. 0x21bd00's two
// reloads on that step, 2 unless noted: the iterator's fields in the five other orders 3/2/3/3/3, `0 < count`,
// `++count`, `count += 1`, the compare written `first == next` 3, a named next 71, the initialiser in three other orders
// 2/4/2, and three placements of its own locals: none restores its order.
// M3 lane 5 r238 (h2m fast with 0x21bd00, 0x21be20, 0x21b870, 0x21b940, 0x126df0, 0x128020 and 0x125b40; control 305).
// The blocker is still the shared ring step: written `index = next; if (next == first) index = NONE;` it gives 0x21be20
// MATCH, 0x21b870 MATCH, this body 418 and matched 0x21bd00 2 (its latch reloads the first index and then the count,
// retail the count and then the first index). 0x21bd00 on that step, all 2 (same code): without its address-of line,
// with an early-return guard, the existing sound_class_definition_get accessor fed a named long index
// (fed the char read directly 43), no index local (`iterator.index` at both uses), no `__stdcall` keyword; the permuter
// cannot search it (its reduced image reads 79 for the full image's 2). The step as `index = NONE; if (next != first)
// index = next;` 452 here (1939 B; 0x21b870 33, 0x21bd00 64, 0x21be20 45). Lane 14 r139's recipe rebuilt on this file
// with the shared step: 455 (1939 B; aligned 103 exact / 18 register-masked); on it the ring walk with no index local
// (`while (track_sound_iterator_next(&iterator) != NONE)` and `iterator.index` at both uses) 295 at retail's 1923 B and
// 535 instructions (aligned 97 / 14: the walk and the approach block are then retail's but for the index in EAX for
// ECX); with the entry looked up at each use too 358 / 428; the four setup statements in all 24 orders on that form
// 295..307, none with the mask store ahead of the gains. Not kept: the recipe is a stack of match-only forms.
// M3 lane 3 r308 (h2m fast with 0x126df0 and 0x12a810; this body reads 87 without them). The two reloads tie in the
// scheduler (equal height and successors) and follow their split pieces' numbers. The split pass takes NONE, zero and
// the 0xffff mask, then the first index, then the count, and hands out free numbers in the order 4, 19, 15, 14, 21,
// 12, 22 (19 and 15 are the duration and the 1000.0f constant). So the first index gets 14 and the count 21. One
// split value more or fewer below the count, or one float or dead value more or fewer ahead of 14, would give
// retail's order. Forms, 2 unless given: the definition through the existing sound_definition_get 2 (same code), with
// the playing element unnamed 2, with no index local (`long index = iterator.index;` in the body) 2; the promotion lookup
// through the out-of-line 0x221810 116 (236 B, a call); `long index = NONE;` 2 (removed before allocation).
// M3 lane 4 r261 (fast screen with 0x21be20, 0x21b870, 0x126df0, 0x12a810 and 0x21b0e0; control 2). The allocator's
// split pass: the integer candidates numbered below the first index are the NONE, zero, 0xffff and 0x80 constants; the
// pieces draw numbers in candidate order from the freed list 4, 19, 15, 14, 21, 12, 22 (19 and 15 are the float
// duration and 1000.0f), the 0x80 constant draws none, so the first index gets 14 and the count 21. Forms, 2 unless
// given: 0x126df0's fade-out arm with a named `long milliseconds` 2 (0x126df0 MATCH); 0x126df0 with one sound pointer
// declared at its top for both arms 2; its fade-out gain named ahead of the lookup 64, the lookup after the gain call
// 64 (0x126df0 45 each); one milliseconds local at 0x126df0's top for both arms 2 (0x126df0 52, 0x21be20 63); the
// definition through sound_definition_get with its promotion_index field 2 (same code). Diagnostics: the call's
// duration `* 1.0f` 2, `* 2.0f` 26 (294 B), `(real)(double)duration` 2. All restored.
// M3 lane 3 r310 (fast screen with 0x126df0, 0x12a810, 0x21be20 and 0x21b870; control 2). The integer pieces draw from
// the numbers no integer candidate holds: the dropped AND-1 constant of the curve test (4), the two float
// loop-invariant loads (19 the 1000.0f constant, 15 the duration), then 14, 21, 12, 22. 1000.0f as a file-scope named
// constant in 0x126df0 2 (same pieces: it folds); the iterator set-up as the for-init 2. The ring step as one
// conditional expression (`index = next == first ? NONE : next`, either compare) gives this function MATCH and
// 0x21b870 MATCH but 0x21be20 24 (its index and element pointer swap ESI and EDI: the allocator ranks the index 166
// against the element's 144, 120 of it from five index references in the stepped block; a forced-ranking search
// gives a byte match with the index at 143). On that step: a named next with no links local 71 (0x21be20 54,
// 0x21b870 32), as an if/else 71, with the links local kept 71; `first == next` 1 (0x21b870 1, 0x21be20 24); the
// if/else step re-measured 0 (0x21be20 24); 0x21be20's element named ahead of its state test 24, its state test as
// an early `continue` 24; 0x126df0's fade-in end time named 24. 0x21be20 needs one more candidate referenced in its
// post-join store block (164 each, the element then wins the tie) or 22 fewer index points. All restored.
// M3 lane 1 r170 (h2m fast with 0x21be20, 0x21b870, 0x126df0, 0x12a810 and 0x21b0e0; control 2; 0x126df0's file was held
// by another lane, so only this body was respelled): the curve as `(flags & 0x400) != 0x400` 22 (293 B); const
// definition and promotion pointers 2 (same code); a `dword` index 85 (298 B); the curve as `== 0 ? true : false` 2
// (same code); the definition looked up by `(word)` index 49. By the free-number reading above, the order flips if the
// curve's dropped AND-1 constant or the fade's index copy were not freed ahead of the first index.
// @retail 0x21bd00
void __stdcall looping_sound_fade_out(real duration, long first_sound_index)
{
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
			s_looping_promotion *promotion = (s_looping_promotion *)sound_class_definition_get((short)promotion_index);
			bool curve = !(promotion->flags & 0x400);
			function_126df0(curve, NONE, index, duration);
		}
	}
}

// Matches (2026-11-24) with the ring step stored and then tested; with one conditional store the index and the
// element pointer take ESI and EDI the other way round (24).
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
	byte unknown8e[0xa0 - 0x8e];
};

// MiniMax I (11-16): no new form, no lower score (110): retail calls the first-player query 0x14dde0 out of
// line where this build expands it, and retail's audible arm holds the offset in a stack temporary that is copied
// into the request after the 0x21bfb0 call where this build writes the request's storage directly; the query's
// declaration lives in another file this lane may not edit.
// MiniMax E (11-16): no new form, no lower score (110): retail calls the first-player query 0x14dde0 out of line
// where this build expands it, and retail's audible arm holds the offset in a stack temporary that is copied
// into the request after the 0x21bfb0 call where this build writes the request's storage directly (that arm
// measures 134); the query's body sits in a file this lane may not edit. The source permuter refuses to search
// this function: on the reduced image the original reads 117 differing instructions against the full build's
// 110, so its fast search is invalid here and only a full-build search would do, which does not fit the time
// budget.
// MiniMax S (11-16): matches (was 110): the audible arm holds the offset in the stack temporary and copies
// it into the request after the 0x21bfb0 call, the first-player query stays out of line as retail's 0x14dde0
// (the redundant second test on the audible byte is what keeps the query from being expanded), the camera
// lookup comes before the audible byte is set again, and the copy into the request sits after the flags, index
// and callbacks stores with the result flag set last.
// score 110 (was 110): frame 0xc and entry copy agree; audible offset uses request storage
// instead of retail's temporary/copy, and the first-player query still expands.
// MiniMax E (11-14): no new form, no lower score (110, was 110): retail calls the first-player query out of line
// where this build expands it, and retail's audible arm holds the offset in a stack temporary that is copied into the
// request after the 0x21bfb0 call; that arm measures 127 to 134. The query's declaration is in another file.
// MiniMax B (11-12): no new form, no lower score (110, was 110): retail calls the first-player query 0x14dde0 out of
// line where this build expands it, and retail's audible arm keeps the offset in a stack temporary that is copied into
// the request after the 0x21bfb0 call (that arm scores 127-134). Marking the query noinline gives 25 at 509 bytes but
// costs three matched callers, and the declaration of the query is not ours to change.
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
// M3 lane 7 r161: no unlisted form found (each candidate, from the query's position to the skip-loop and temp-offset
// shapes, is listed above); the first-player query's call boundary is still the whole difference.
// M3 lane 6 r182 (fast with the query's callers and its four inlining sites, control = full 110): 0x14dde0's scan
// through a walking element pointer 110 (the query still inlined; 0x14dde0 3 and its four inlining sites 2-4); 0x14dde0
// as a `while (i < 4 && ... == NONE) i++` search with a ternary return 129 (486 B; 0x14dde0 20, 0x13c250 268); the
// audible arm's `source->audible = sound->source.audible` 110 (same code); the player arm's four spatial stores
// through one `s_sound_position *spatial` 110 (same code); a true `__assume(sound->source.audible != 1)` at the top
// of the player block 110 (same code: an assumption is no conditional level for the call site). The query stays inlined.
// MiniMax S (11-14): no new form, no lower score (110): the acquisition schedule and the first-player query (expanded
// here, called in retail) are unchanged; the earlier rounds' result, workspace, copy and payload forms land at 110 or
// worse, and the call boundary of that query is the whole difference.
// P525: Actual request flags/mode/type full-width signed field storage, seven
// graphs then complete int units: eight 110 at 508/508 bytes. NOT kept: no native
// offset transport or first-player call recovery; original request record and complete body restored.
// Round 248 (m3 lane 3, fast with 0x21b0e0, control 110): the query stays inlined in all five. `if (!result)` in place of
// `else` 110 (same code: the flag test is folded before the inliner weighs the site, unlike round 166's memory re-test);
// the arms as two tests of a named `bool audible` 138 (518 bytes); the player-picking arm first with its own return and the
// tail written in both arms 159 (537 bytes); retail's audible arm (offset in a temporary copied into the request after the
// 0x21bfb0 call, result set last) 134 (534 bytes); that with `player` declared at function scope 134. The query's ten other
// call sites are all matched (six call it, four inline it), so its own body cannot move.
// P562: Actual count/remainder/selected-player declarations at function entry,
// six orders and two full lexical graphs: 110/110/110/110/110/110/110/110. NOT kept:
// every form ties; original declarations, producers and whole body restored.
// Frame 0xc agrees; native detail transport and first-player call remain.
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
		request->flags = 0x2f;
		request->looping_sound_index = looping_sound_index;
		request->callbacks = &g_44a130;
		request->offset = offset;
		request->tracking_data_size = sizeof(request->offset);
		result = true;
	}
	else if (sound->source.audible != 1)
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
			s_local_camera *camera = local_camera_get(player);
			source->audible = 1;
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
// Matches (m3 lane 14 r154, from lane 14 r139's lead) with: the sounds cleared by memset and the mask cleared ahead
// of the gains; a count local in each pitch-range loop; the pitch through a bounds pointer and `lower`, with the
// range-times-gain and playing pitch grouped ahead of `lower` (retail adds the playing pitch first; without the
// grouping pair the full build adds `lower` first, 2); the first loop's range pair read through a pointer at its
// minimum (bounds->minimum / maximum: 4); the active bit tested at each use; the range sound's index named, with the
// entry looked up from the name and the level stored through a second lookup of sounds[i] (retail's two index
// products; through `entry`: 236 at 1907 B); the range's level copied into a long ahead of the sum (retail's stack
// copy; gains[i] read directly: 119); the links' last byte typed char and set to NONE; the detail request's view at
// 0xa0 bytes; the track definition declared ahead of the track; and the ring step stored and then tested. The
// reduced image shows the full build's pitch order only with 0x127e20 on its address list.
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
				looping_sound_track_definition *track_definition = &definition->tracks[track_index];
				s_type_06b94f *track = (s_type_06b94f *)sound->track_storage + track_index;
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
					s_short_bounds *pitch_bounds = &((s_looping_sound_tables *)g_51ebd4)->pitch_modifiers[playback->playback_index].pitch_bounds;
					real lower = (real)pitch_bounds->lower;
					real pitch = (((real)pitch_bounds->upper - lower) * track->gain + playing->pitch) + lower;
					c_class_219e90 *controller = controller_get(sound->controller_index);
					if (controller->definition_index == NONE)
						random_next(&controller->field_8_4);
					dword seed = controller->field_8_4;
					short previous_pitch_range = playing->pitch_range_index;
					short selected = function_218f50(playback, previous_pitch_range, pitch);
					if (playback->flags & 0x2000)
					{
						long sounds[9];
						dword active_mask = 0;
						memset(sounds, NONE, sizeof(sounds));
						long gains[9] = { 0xc2800000, 0xc2800000, 0xc2800000, 0xc2800000, 0xc2800000, 0xc2800000, 0xc2800000, 0xc2800000, 0xc2800000 };
						s_track_sound_iterator iterator;
						long current;
						track_sound_iterator_new(&iterator, track->sound_index);
						while ((current = track_sound_iterator_next(&iterator)) != NONE)
						{
							s_looping_track_sound *entry = track_sound_get(g_4e637c, current);
							if (entry->definition_index == definition_index && entry->state == 2)
								sounds[entry->pitch_range_index] = current;
						}
						for (long i = 0, count = playback->pitch_range_count; i < count; ++i)
						{
							s_permutation_set *set = &g_51ebd4->sets[playback->pitch_range_base + i];
							short const *range = &((s_looping_pitch_tables *)g_51ebd4)->bounds[*(short *)((byte *)set + 2)].minimum;
							gains[i] = looping_sound_pitch_attenuation(playback, set, pitch);
							real clamped = pitch < (real)range[0] ? (real)range[0] :
								pitch > (real)range[1] ? (real)range[1] : pitch;
							if (clamped == pitch)
								active_mask |= 1 << i;
						}
						for (long i = 0, count = playback->pitch_range_count; i < count; ++i)
						{
							if (!(active_mask & (1 << i)) && sounds[i] != NONE)
							{
								function_127320(sounds[i], 8);
								sounds[i] = NONE;
							}
							if ((active_mask & (1 << i)) && sounds[i] == NONE)
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
							long sound_index = sounds[i];
							if (sound_index != NONE)
							{
								s_looping_track_sound *entry = track_sound_get(g_4e637c, sound_index);
								long gain_bits = gains[i];
								real gain = *(real *)&track_definition->value08 + *(real *)&gain_bits;
								track_sound_get(g_4e637c, sounds[i])->track_value = *(dword *)&gain;
								entry->loop.unknown07 = NONE;
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
	struct
	{
		dword flags;
		long format;
		real pitch;
		long gain_bits;
		byte unknown10[0x2c - 0x10];
		long controller_index;
	};
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

/* the pitch range and the permutation a playing track sound is on */
static __forceinline s_permutation_set *track_sound_pitch_range(s_looping_track_sound const *playing)
{
	s_looping_playback_definition *playback = (s_looping_playback_definition *)g_4e3b44[playing->definition_index & 0xffff].bytes;

	return &g_51ebd4->sets[playback->pitch_range_base + playing->pitch_range_index];
}

static __forceinline s_looping_permutation *track_sound_permutation(s_looping_track_sound const *playing)
{
	return &((s_looping_permutation *)g_51ebd4->chances)[track_sound_pitch_range(playing)->first_index + playing->permutation_index];
}

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
/* The entry locals are read before the two-part clear of the properties (retail copies the
   source scale and the track value into the frame first). The permutation a sound is on is looked
   up at each use in the chunk logic, and the stop arm is reached by clearing the permutation index. */
// @retail 0x21c400
void function_21c400(short voice_index, real gain, s_looping_channel_spatialization const *spatialization)
{
	s_looping_voice_view *voice = &((s_looping_voice_view *)g_4e6378)[voice_index];
	long sound_index = voice->sound_index;
	s_looping_track_sound *playing = track_sound_get(g_4e637c, sound_index);
	s_looping_playback_definition *playback = (s_looping_playback_definition *)g_4e3b44[playing->definition_index & 0xffff].bytes;
	s_permutation_set *arg_58ecd0 = track_sound_pitch_range(playing);
	s_type_5ef569 *sound = (s_type_5ef569 *)LOOPING_SOUNDS->data + (playing->looping_sound_index & 0xffff);
	s_type_06b94f *track = (s_type_06b94f *)sound->track_storage + playing->loop.track_index;
	looping_sound_definition *definition = (looping_sound_definition *)g_4e3b44[sound->definition_index & 0xffff].bytes;
	looping_sound_track_definition *track_definition = &definition->tracks[playing->loop.track_index];
	c_class_219e90 *controller = controller_get(sound->controller_index);
	real scale = playing->source.scale;
	s_short_bounds *pitch_bounds = &((s_looping_sound_tables *)g_51ebd4)->pitch_modifiers[playback->playback_index].pitch_bounds;
	real lower = (real)pitch_bounds->lower;
	real pitch = ((real)pitch_bounds->upper - lower) * track->gain + playing->pitch + lower;
	s_decibels track_value = *(s_decibels *)&playing->track_value;
	bool primary = track->sound_index == sound_index;
	s_looping_channel_properties properties = {{0}, {0}};
	s_decibels gain_sum = *(s_decibels *)&playing->source.unknown08 + (track_value + *(s_decibels *)&gain);
	properties.gain_bits = function_1251e0(playback, *(long *)&gain_sum, scale);
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
		function_21f8a0(((s_looping_voice_view *)g_4e6378)[voice_index].channel_index, &properties, &properties.effects.playback);
		function_21f720(&properties.effects.impulse);
		sound_voice_queue_chunk(voice_index, (s_sound_permutation const *)permutation, playing->current_chunk_index);
		if (definition->flags & 0x20)
		{
			char synch_count = NONE;
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
				short permutation_index = playing->permutation_index;
				long next_chunk;
				if (track_sound_permutation(playing) && playing->current_chunk_index < track_sound_permutation(playing)->chunk_count - 1)
					next_chunk = playing->current_chunk_index + 1;
				else
					next_chunk = NONE;
				bool last_chunk = (short)next_chunk == NONE;
				if (!(playing->loop.flags & 1) && primary && (byte)looping_sound_track_has_next_sound(track, track_definition))
				{
					if (last_chunk || (looping_sound_track_transition_requires_stop(track) &&
						TEST_FIELD_BIT(((s_looping_chunk_tables *)g_51ebd4)->chunks[track_sound_permutation(playing)->first_chunk + playing->current_chunk_index].transition_boundary)))
					{
						if (function_21c190(sound, track, voice->sound_index, seed))
							return;
						arg_58ecd0 = track_sound_pitch_range(playing);
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
						permutation_index = NONE;
					}
				}
				if (permutation_index != NONE)
				{
					sound_playback_set_chunk(voice->sound_index, playing->definition_index, playing->pitch_range_index, (short)permutation_index, (short)next_chunk);
					playing->transition_pending = true;
				}
				else
				{
					word_flags_set(&playing->flags, 6, true); /* stop requested */
					function_12a420(voice_index);
				}
			}
prepare_chunk:
			if (TEST_FIELD_BIT(playing->transition_pending))
			{
				s_looping_permutation *permutation = &((s_looping_permutation *)g_51ebd4->chances)[arg_58ecd0->first_index + playing->permutation_index];
				s_looping_track_sound *voice_sound = track_sound_get(g_4e637c, voice->sound_index);
				if ((byte)function_125e60(voice_sound) && channel_state != 3)
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
			word_flags_set(&playing->flags, 6, true); /* stop requested */
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
// MiniMax S (11-20): no new form, no lower score (597): retail reads the flags byte after the four pushes and masks it with the constant 1 there, where this build hoists the byte load to the entry; four explicit listener-gain stores instead of the memset score 767.
// MiniMax B (11-20): no new form, no lower score (597): the entry still hoists the flags byte into BL where retail masks it with 1 after the pushes (reading `alternate` inside the definition guard reproduces retail's entry but measures 615) and the track loop still holds the index in ESI, the track definition in EBP and NONE in EBX where retail keeps the index in memory, the track in EBP and NONE in ESI; what still differs is that entry mask, the creation arm's compare order and the track loop's register roles.
// score 597 (was 597): The 0x3c frame and 2518-byte extent agree. At +0x4 this build reads the request byte
// into BL before saving EBP. Retail saves EBP first, compares the requested state before admitting the
// definition, and loads the request byte after that gate. One in EBX then serves the initial writes and state
// logic. The trials test the fallback result’s dependence on definition admission and availability while
// keeping the lookup, creation and playback paths intact.
// Earlier at 597 (was 597): Retail delays the request-byte load until after the definition test, then keeps one in EBX. The track and state register roles still differ.
// Earlier score 597 (was 597): retail length recovered; saved-register order, record homes, energy-loop
// MiniMax G (11-20): no new form, no lower score (597): the entry still differs - retail tests the state before the definition index and reads the flags byte inside the definition guard, masking it with 1 kept in ebx, while this build reads the flags byte into bl at the top and computes the alternate flag before the guard; a constant-one local is folded away and swapping the two top statements changes nothing.
// Earlier: retail length recovered; saved-register order, record homes, energy-loop
// Earlier at 597 (was 597): retail length recovered; saved-register order, record homes, energy-loop
// [r153: see the m3 lane 12 r153 entry at the end of this note]
// padding and track/state ownership remain. Round 439: entry tag/handle/record/all-record
// declarations and entry Boolean declarations 716/716/716/716/716 reduced; alternate before
// initial result 694 reduced, 597 plain (kept); independent request state 811 reduced;
// complete geometry output scope 716 reduced (unchanged reduced control 716, full 615).
// MiniMax I (11-16): no new form, no lower score (597): the track loop's register roles are still this build's -
// retail holds the state in EBX, NONE in ESI and the track in EBP where this build holds NONE in EBX, the index
// in ESI and the track definition in EBP - and the entry still hoists the flags byte into BL where retail masks it
// with 1 after the pushes; the forms listed below score 590 or more.
// MiniMax C (11-14): no new form, no lower score (597): the entry still hoists the flags byte into BL where retail masks it with 1 after the pushes, and the track loop's register roles are still this build's; the form that reads `alternate` inside the definition guard measures 615 and is not kept.
// MiniMax S (11-11): no new form, no lower score (597); the entry still hoists the flags byte into BL where retail masks it with 1 after the pushes, and the known remaining deltas are the creation compare's order, the source copy from EDX into ESI, the gains clear in retail's store form and the track loop's register roles; reading `alternate` inside the definition guard reproduces retail's entry but measures 615.
// MiniMax H (11-14): no new form, no lower score (597): the entry still hoists the flags byte into BL where retail
// masks it with 1 after the pushes, and the saved-register order, the record homes, the energy loop's padding and
// the track and state ownership still differ.
// MiniMax E (11-16): no new form, no lower score (597): the entry still hoists the flags byte into BL where
// retail masks it with 1 after the pushes, and the saved-register order, the record homes, the energy loop's
// padding and the track and state ownership still differ; the form that reads `alternate` inside the definition
// guard measures 615 and is not kept.
// MiniMax S (11-16): no new form; the length 2518 agrees but the track loop's registers
// still differ - retail holds the state in EBX, the NONE state in ESI and the track in EBP, while this build
// holds NONE in EBX, the index in ESI and the track definition in EBP - and the entry's flag mask and the gains
// clear still use this build's forms.
// MiniMax A (11-18): no new form, no lower score (597): the entry still hoists the flags byte into BL where retail
// masks it with 1 after the pushes, and the track loop's register roles are still this build's - retail holds the
// state in ebx, the NONE state in esi and the track in ebp, where this build holds NONE in ebx, the index in esi and
// the track definition in ebp; the form that reads `alternate` inside the definition guard measures 615 and is not kept.
// MiniMax C (11-14): no new form, no lower score (597): the entry still hoists the flags byte into BL where retail masks it with 1 after the pushes, and the track loop's register roles are still this build's; the form that reads `alternate` inside the definition guard measures 615 and is not kept.
// MiniMax S (11-11): no new form, no lower score (597); the entry still hoists the flags byte into BL where retail masks it with 1 after the pushes, and the known remaining deltas are the creation compare's order, the source copy from EDX into ESI, the gains clear in retail's store form and the track loop's register roles; reading `alternate` inside the definition guard reproduces retail's entry but measures 615.
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
// MiniMax A (11-18): no new form, no lower score (597): the track loop's registers still differ - retail holds the state in ebx, NONE in esi and the track in ebp, while this build holds NONE in ebx, the index in esi and the track definition in ebp - and the request byte still occupies BL before the saved registers; the earlier rounds' forms of these measure 590 or more.
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
// M3 lane 7 r161 (fast with callers and callees = full 597; plain where marked): the second impulse truth computed after
// the first truth's two sets 667 (2497 B); the track flags read once into a dword local with each truth written at its
// two tests 669 (2510 B, frame 0x38: retail's unthreaded double tests, but dword-wide), TEST_FIELD_BIT at each test
// the same code; the four sets through flag_set_12a1b0 with the truth expression as each argument 675 (frame 0x38;
// the alternate pair is then retail's `and al, 1` / `test al, al`); `previous` read first and `next` taken from it 597
// (other code); the replacement fade as `if (state == 0) { if (flags & 1) ... } else 0.5f` 595 (retail's block order
// there; an arm swap, lead only); on that, alternate_end assigned last in each end-arm branch 649 (2525 B), the inlined
// fade-out half's looping bit through the existing word-flag setter 0xbfe40 (matched 0x126df0's spelling of the same
// store) 590; that setter alone 592 plain (that block's store order is then retail's; 5 points, under the keep bar). On
// the setter form: the fade duration read in place 636 (2500 B), the normalization scale as a ternary 681, one pointer to
// the listener gains for the clear and the sum 727 (2488 B), the one-use current_alternate bool written into its test
// 645 (the named bool is what gives retail's byte compare there); same code: chained `next_state = field_c_4 = value` at
// all three sites, sound_class's bit cleared through 0xbfe40, the listener matrix passed in place, impulse_states as long.
// M3 lane 6 r182 (fast with callers and callees, control = full 597; second number the register- and stack-masked aligned
// count, 163 for 597): the track loop's index as `short` 623 (2503 B, 154); the three alternate-state tests through one
// inline bool helper `((1 << state) & 0x58) != 0` 636 (2508 B, 153: retail's setne/byte compare at all three, the tail then
// shifts); `local_2e47c8` renamed impulse_definition 597 (same code: names do not move the 0x1c/0x20/0x24 homes); the
// old-end fade's looping bit through 0xbfe40 592 (161, as lane 7 r161; under the keep bar). Compile-only finding: the gains
// clear as `for (short listener_index = 0; listener_index < 4; ++listener_index) gains[i] = 0.f;` is retail's `xor` plus four
// direct dword stores (a long counter gives xorps/movss, memset the `lea` form); on the setter form it is 667 (2518 B, 158:
// the clear exact, the source-copy `mov esi, edx` then unpaired). Chain on the setter form, each retail's code where it
// acts, positional score up because the length moves: + short clear loop + bool helper 731 (2508 B, 148); + `alternate`
// inside the definition guard 722 (2492 B, 136 fast; entry exact to +0x68); + `created = true` before the creation call
// 722 (140). Left on the chain: the creation compare order, source in EDX copied to ESI, the energy loop's padding, state
// in EBX before the track loop, the track loop's register roles (track EBP, NONE ESI), the start arm's shared tail.
// MiniMax S (11-14): no new form, no lower score (597): the track loop's registers still differ - retail holds the
// state in ebx, NONE in esi and the track in ebp, while this build holds NONE in ebx, the index in esi and the track
// definition in ebp; the forms listed below score 590 or more.
// P525: Actual initialized/hardware/enabled full-byte field allocation, seven unsigned
// graphs then complete signed allocation: eight reduced 694 (control 694, full 597).
// NOT kept: native entry true/flags and record/track homes remain; original admission fields and complete body restored.
// Round 248 (m3 lane 3, plain; 0x18a7b0 and 0x21d630 stay matched): `alternate` inside the definition guard re-measured 615
// at 2500 bytes (entry and EBX = 1 as retail; the gain clear forms its address first where retail stores zero at
// [ebp+0xc4..], and the source pointer goes straight to ESI where retail copies it from EDX); on that form `distance`
// declared before `maximum` 615, `created = true` before the creation call 615, both 615 (the same code); `track_alternate`
// as one direct expression in place of the preset-then-clear form 662 (2503 bytes), on today's body 614 (2519 bytes).
// P562: Initial result from false-default/state-admission or true-default/state-
// rejection, bool/byte/short/long: 791/770/768/787/791/770/768/787 reduced (control 694,
// full 597). NOT kept: no equality or native true/flags recovery; original
// initial state test, completion polarity and whole body restored.
// P591: Complete reused start/resume classification after request-state adjustment,
// bool/byte/short/long in both polarities: 755/755/746/727/755/755/746/727. NOT kept: all regress
// against calibrated 597; original repeated tests and full body restored. Entry true/flags and record homes remain.
// M3 lane 1 r114 (plain): retail stores the start arm's field_c_4 = 0 and reloads it for next_state (a store written
// in each arm and merged); only `field_c_4 = 0` written at the end of each arm, next_state after the arms, 681 (2475 B:
// the constant is threaded into both arms, no reload). `track_alternate` as one `alternate && (... != NONE || ...)`
// expression 614 (2519 B); the impulse bits through the existing flag_set_12a1b0 597 (same code); `total` declared
// before `scale` 598; `previous` read before `next` and used for the first table index 597. Diagnostics (reverted):
// `(void)&index;` and a read through `long const *index_reference = &index;` give identical code (the index stays in
// ESI; the address does not keep it in memory).
// Invalid definition first 782; state-derived result on unavailable path 779;
// shifted start request bit 705; conditional alternate store 595 (NOT kept:
// two points and seven bytes short); conditional request stores 611; all
// conditional stores 609; joined request stores 661; three-bit write 675.
// Entry flags still occupy BL, and the record and track homes differ.
// MiniMax D (11-17): no new form, no lower score (597 plain, 694 on the reduced image): the track
// loop's registers still differ - retail holds the state in ebx, NONE in esi and the track in ebp with the index and the
// track definition in memory, while this build holds NONE in ebx, the index in esi and the track definition in ebp; the
// start arm's constant 1 still does not sit in ebx, the alternate compares still turn into int neg/sbb where retail
// compares bytes, and the forms the earlier rounds list (the old-end fade's looping bit through the existing 0xbfe40
// setter, the alternate inside the definition guard, one source pointer for every source use) measure 590 or more.
// The request byte still occupies BL before the saved registers.
// Alternate from explicit branches 597; normal-track complement 731; independent
// resume request 735; conditional state selection 597; shared low-three request bits
// 733; mask for both closing bit tests 623; four decision names 597; decision and
// track-mode names 597. Normal-track carrier with separate resume gates 768.
// NOT kept: the shared request mask raises the score and demotes a caller in the preview.
// MiniMax C (11-17): no new form, no lower score (597): the track loop's registers still differ - retail holds the state in ebx, NONE in esi and the track in ebp, while this build holds NONE in ebx, the index in esi and the track definition in ebp; the creation compare order, the source copied from edx to esi, the energy loop's padding and the start arm's shared tail also differ, and the earlier rounds' forms of these score 590 or more.
// M3 lane 15 r105 (plain with 0x21be20; aligned/masked = difflib counts with addresses / also registers masked, 261/134 at
// 597): the old-end fade's looping bit through the existing 0xbfe40 setter re-measured 592 (aligned 260), and on it: `alternate`
// inside the definition guard 615 (2500 B, aligned 248, masked 119); track before track_definition 592; memset of
// `&listener_gains[0]` 592 (same code); the seed as a ternary 610 (2504 B, aligned 228, masked 115). --fast builds this body
// at 694 (2471 B), so screen it plain. Left: the entry's flags byte in BL (retail EBX = 1 after the pushes), the gains clear
// (retail stores from a zeroed register at [ebp+0xc4..]), the source copy through EDX and the track loop's register roles.
// M3 lane 2 r104 (fast with callers and callees, control 597 = the full check; the three counts are aligned exact /
// stack-masked / register-masked differing instructions, 336/307/163 at 597). Lead, NOT kept (each piece is a form kept
// only with a match): 519 at 2518 bytes (the plain check agrees; 290/274/139), the first 200 instructions retail's but
// for the sound pointer's slot (0x1c for 0x20), from three pieces together: `alternate` declared inside the definition
// guard; the gains clear as `for (short listener_index = 0; listener_index < 4; ++listener_index)
// sound->listener_gains[listener_index] = 0.f;` (the file's matched clears are memset calls); and `if (index == NONE)
// result = false;` after `created = true` in the creation arm with the join's `else result = false` left in place (a
// repeated test). The last piece gives retail's creation block (`cmp eax, -1` ahead of the two stores, `je` to the far
// result store), the source pointer in EDX copied to ESI for the copy, the `lea` order at the 0x142700 call and the
// energy loop's padding. The arm's test alone 595, with the guard 624, the guard alone 615, the clear alone 667, guard
// and clear 697; the diff is build/r104/lead-21a250-b3.patch.
// Other spellings of the creation failure on guard + clear (697): the arm's test with no else at the join 729 (the
// creation arm moves out of line); the arm's `result = false; goto done` with no join test 739 (0x192d70 is then
// expanded), with the join kept 709; `result = index != NONE` in the arm with / without the join's else 638 / 698;
// `if (index != NONE) result = true` in the arm 591; an empty `if (index == NONE) { }` 697 (same code); the join as
// `if (!created || index != NONE)` 748, as `if (result)` after the arm's test 729, as `if (created && index == NONE)
// result = false; else` 739; the join's else as `else if (created) result = false` with / without the arm's test
// 519 (the lead's code) / 691; without the goto: that else 722, the arm's test with no else 739, `result = index !=
// NONE` 712; a pool local for the sound lookup 697 and `created = true` ahead of the creation call 697 (same code).
// On the plain body: the arm's test with no else, or with `goto done`, 750 each; the else-if-created join without the
// goto 640; `result = index != NONE` with no else 747, with the guard 709.
// On the lead (519): the start arm's `field_c_4 = 0; next_state = field_c_4;` at the end of each arm 581 (2506 B,
// 253/238/103: the track then takes EBP as retail), only the first of them in each arm 602 (2475 B); the old-end
// fade's looping bit through 0xbfe40 514; the replacement fade's arms swapped 517; the final listener test through a
// source pointer 518; 0x21b940's bounds pointer 519; the three alternate-state tests through one inline bool helper
// 558 (2508 B, 280/264/129); those five 553, all six 580 (2498 B, 233/220/85), with track declared ahead of
// track_definition 586 (2478 B, 219/205/81; build/r104/lead-21a250-b4.patch). Without the arm's test the same stacks
// read 715 (guard, clear and the per-arm stores, 318/288/111) and 733 (plus the five, 299/270/93).
// The impulse bits on the lead: through flag_set_12a1b0 with the two bools, or as conditional expressions, 519 (same
// code: the second test of each pair is still threaded); the truth written as each helper argument 615 (frame 0x38),
// 581 on the six-form stack; the second bool computed after the first pair 606 on that stack; built from zero with
// every bit a conditional on the field expression (`impulse_states = 0;`, then bit 0 -> 1, bit 1 -> 4 and 0x40, bit 2
// -> 8 and 0x20) 610 (frame 0x38, 330/265/104: retail's separate byte load of the flags for bit 0, its unthreaded
// double tests and NONE in ESI, but next_definition loses its home); the first bit alone from zero 557 (298/279/133);
// the first bit as a bool conversion 570 (frame 0x38); the last four as conditional expressions on the field
// expression 615. The two state bytes as 8-bit bit-fields 597, on the lead 519 (same code).
// The six forms with track declared first and the from-zero impulse bits 554 (2510 B, frame 0x3c, 234/221/83, 763
// instructions for retail's 767): state is then held in EBX from the release test on and reloaded at the latch as in
// retail, and both start calls share the hoisted seed push. On that stack, same code each (554): the track block's
// base in a local (declared before or after the track pointer), the element address through byte arithmetic, the
// index declared outside the `for`, `i++`, one index variable for the gain loops and the track loop, old_end declared
// at the top of the loop body; seed declared ahead of the two pointers 555.
// Left on that stack: track_definition takes ESI from the loop top to the transition walk where retail keeps it in
// EDX and its home (reloaded after each call) with NONE in ESI; old_end then takes EBX where retail has ESI and the
// constant 1 in EBX; the start arm's shared tail (retail stores the zero and reloads it); i and track_definition
// hold each other's homes (0x54/0x50, retail 0x50/0x54).
// M3 lane 5 r206 (plain): in the track loop retail keeps the index in memory, the track in EBP and NONE in ESI (ours: index
// ESI, track definition EBP, NONE EBX). The loop index as `short` 623 (2503 B); track_alternate as one direct `alternate &&
// (... != NONE || ... || (flags & 0x14))` expression 614 (2519 B); the seed pointer chosen after track_alternate 629 (2508 B);
// track_alternate computed after the created block 641; track_definition declared after the track and seed pointers 596
// (a declaration move under the keep bar).
// M3 lane 12 r153 ("fast" = the reduced image with 0x128020, 0x126df0, 0x126c30, 0x127d00, 0x21be20 and 0x21b0e0's
// callers on the checked list, 597 at 2518 bytes as the full check; second pair the aligned exact / register-masked
// counts, 307 / 161 for 597). Diagnostics first (reverted): `(short)0` for every `(short)i` argument 671 (2486 B), the
// track definition and NONE keep EBP / EBX, so the loop index's uses are not what holds NONE out of ESI; the track
// definition written as `definition->tracks[i]` at each use 797 (2647 B, NONE EDI, no spilled pointer forms), the track
// written as the indexed storage element at each use 732 (2636 B, 294 / 149). New forms: the three state pairs through an
// inline `track_state_set(track, state)` setter 597 (same code); the replacement fade as one call, `if (state != 0 ||
// (flags & 1)) function_126df0(1, replacement, NONE, state != 0 ? 0.5f : fade_in);` 634 (2523 B, 273 / 142: NONE then
// takes ESI as in retail, the start arm holds the constant 1 in EBX as retail, the index leaves ESI; but the ternary's
// value goes through a stack temporary, frame 0x40); the start arm's `field_c_4 = 0; next_state = field_c_4;` written at
// the end of each of its two arms 658 (2506 B, 271 / 125: the track takes EBP as in retail, the track definition ESI,
// NONE EBX); that with the one-call fade 664 (2518 B, 259 / 145, NONE back in EBX); that with the fade's arms swapped
// (`if (state == 0) { if (flags & 1) ... } else ... 0.5f`) 658 (2506 B, 266 / 120); the arm swap alone re-measured 595
// (300 / 157, lane 7 r161's form). Retail holds NONE in ESI, the track in EBP and the state in EBX across the loop top,
// with the track definition in the sound-flags home; no form here gives both the track in EBP and NONE in ESI.
// Y643: successive request shifts 605; two masked stores 661; playback byte carried through all three stores 597; one
// store for the last two bits 661; one store for all three bits 680; guarded alternate and successive shifts 642;
// guarded alternate and carried playback byte 644; guarded alternate and two masked stores 689.
// NOT kept: Carrying the stored byte ties 597 while shortening the body by eight bytes. The guard-scoped forms
// recover the saved-register prefix through +0x1c but raise the score. The other publication forms shorten the body
// and raise the score.
// Transition walk with prior phase read first 597, candidate tag read once 678,
// impulse phase handled first 597, and signed short transition phase 657.
// With alternate read after the definition guard, the same forms score 615/701/615/632.
// Separate playback state initialized at entry/definition guard/availability/lookup
// 734/732/723/690. Promote after finding or creating the sound, assignment/conditional
// 725/714; inside the valid-sound block 725/745. NOT kept: all grow the frame to 0x40
// and regress the matched caller. The early BL load and track-loop state home still differ.
// MiniMax B (11-19): no new form, no lower score (597): retail keeps the track loop's index in memory, the track in EBP
// and NONE in ESI, where this build holds the index in ESI, the track definition in EBP and NONE in EBX, so the track's
// base and the constant 1 land in the other registers; the 519 form needs the creation arm's `result = false; goto done`
// and the join's `else if (created) result = false`, which the coordinator does not keep.
// Shared byte flag setter on the three final playback bits, subsets 1 through 7:
// 597/597/597/598/598/598/598. All three with alternate under the definition guard 613.
// NOT kept: the guarded form restores the saved-register prefix and then differs at +0x1c,
// but shortens to 2500 bytes and raises the score. The request and track-state homes still differ.
// Final request bits tested by direct mask, second/third/both 592/598/617.
// Clear path first for alternate/second/third/all 600/600/597/603.
// All clear-first with both mask tests 619. NOT kept: the five-point reduction
// shortens the body by seven bytes and leaves the early BL load.
// Lookup miss first with a creation guard, state test first, result in each arm, or positive creation gate 607/645/607/607.
// The initial result store and the availability exit use the same frame byte after the EDI push.
// With alternate scoped to the definition guard 630/630/598/630. NOT kept: the per-arm result reaches retail length and recovers the saves but raises the score.
// Alternate request at record admission/update publication/retained-voice branch 760/780/780.
// Direct bit at both consumers 788, shared bit test 788, rejection at track use 757,
// admission at track use 757, track result from the complete test 745.
// NOT kept: all grow the frame to 0x40 and raise the score. The early BL load and record/track homes remain.
// Creation from lookup miss, retained existing decision, original lookup index
// and miss-first creation 740/740/740/740. With alternate decoded after the
// definition test, 731/731/725/731. The latter recover the entry to +0x1c
// but the acquisition outcome then uses different stores and the scores rise.
// Stopped state set from the resume bit 737, same with alternate decoded inside the definition guard 663.
// Active/pending/both phase snapshots 597/597/597. Late alternate-phase mask
// with direct, active, pending, both, or both and predecessor-first table input
// 650/650/650/650/650. The entry request-byte register and track homes still differ.
// Fallback from both inputs, with or without guarded alternate 762/773. Shared definition test, with or without
// guarded alternate 780/769. Stopped-state assignment under the definition gate, with or without guarded
// alternate 631/622. Direct missing/unavailable exits, with alternate between or after the gates 804/792. NOT
// kept: every form raises the score and the original saved-register sequence is still missing.
// MiniMax C (11-20): no new form, no lower score (597): retail masks the flags byte with 1 after the pushes and keeps
// the alternate byte at [esp+0x13] after the definition_index test, this build hoists the flag into BL before the
// test; the form that reads `alternate` inside the definition guard measures 615 and is not kept, and the track loop
// still exchanges the index and the NONE with retail's roles.
// M3 lane 14 r139 (H2M_FAST with 0x128020, 0x21b0e0, 0x126df0, 0x126c30, 0x127d00 and the matched callees on the list;
// control 597, aligned 336 exact / 161 register-masked rows; scores positional / exact / masked): `alternate` read
// inside the definition guard re-measured 615 / 322 / 146 (2500 B: the first 68 rows are then retail's, the constant 1
// in EBX and the flags tested in their home). On that form: the creation arm written first (`if (index == NONE) { ...
// create ... } else if (...) state = 1;`) 630 / 334 / 158; the listener gains cleared by four assignments 643 / 350 /
// 149 (2518 B); the source copied with memcpy 615 (same code; on main's body 597, same code); the track pointer
// declared ahead of the track definition 615 / 321 / 146 (on main's body 597 / 335 / 161); the seed pointer chosen by
// one conditional expression 653 / 287 / 122 (2488 B, saved with the guard form as build/r139/lead-21a250.patch: the
// best aligned count so far, not kept, positional above 597); the release test with the playing count first 613 / 328
// / 152; the track count in a local 619 / 383 / 152 (frame 0x40). Reading from retail's listing: positionally the body
// agrees up to the release test, where retail loads state into EBX (`mov ebx, [state]; cmp ebx, 2`) and keeps it there
// across the track loop (reloaded at the latch), with the track in EBP, the track definition spilled to the dead
// second parameter's home and NONE loaded into ESI inside each pass after the seed pointer; this build keeps the index
// and the track definition in ESI / EBP. Retail also copies the source pointer from EDX into ESI for the aggregate
// copy (this build loads it straight into ESI) and clears the four listener gains without forming their address.
// M3 lane 5 r238 (h2m fast with callers and callees, control 597 = the full check; aligned exact / register-masked rows
// 336 / 165). Lane 2 r104's stack replayed on today's file (alternate inside the definition guard, the short-counted
// gains clear, the creation arm's `if (index == NONE) result = false;`, the per-arm state stores, the fade's looping
// bit through word_flags_set, the alternate-state helper, the bounds pointer, the swapped fade arms, a source pointer
// for the final listener test, track declared ahead of the track definition): 586 at 2478 B, aligned 219 / 83, the
// first 226 instructions retail's. Reading of what is left there: retail loads `state` into EBX at the release test
// and reloads it at the latch, keeps the loop index and the track definition in the dead parameter homes ([esp+0x50],
// [esp+0x54]) and re-reads them after each call, and gives ESI / EDI to values of one region each (the seed base, NONE,
// the old end sound, the start sound index, the impulse states); this build keeps the index or the track definition
// and NONE in callee-saved registers for the whole pass. Retail's start arm stores `field_c_4 = 0` and reads the field
// back for `next_state` (`mov byte ptr [ebp + 0xc], 0` / `mov al, [ebp + 0xc]`), which no spelling here reproduces.
// The pieces alone: alternate inside the guard 615 (2500 B; 322 / 150), the short-counted clear 667 (361 / 162), both
// 697. Body unchanged.
// M3 lane 7 r221 (plain with 0x18a7b0, 0x21d630, 0x21bd00, 0x21aeb0, 0x21cf80 and 0x21d060, all MATCH; control 597).
// Re-measured: `alternate` inside the availability block 596 (2518 B, aligned 336), inside the definition guard 615 (2500
// B, aligned 322), lane 5 r238's replay of lane 2 r104's stack 586 (2478 B, aligned 219). New: on the guard form the pool
// data named once for the sound lookup 615 (same code); the creation arm testing its own result (`if (index == NONE) {
// result = false; goto done; }`, the join's test and its else dropped) 750 (2504 B), with the guard 711 (2518 B); the
// four impulse-bit sets through the existing setter 0xbfe20 (link-time inlined) 597 on main and 586 on the replay (same
// code each: the second test of each pair is still threaded). Body unchanged.
// M3 lane 6 r239 (fast with the callers 0x18a7b0/0x21d630 and 15 callees, control 597, aligned 267): `seed` declared ahead
// of the two track pointers 596; impulse_states' bit 0 as `(flags & 1) != 0` 648 (2524 B); on the old-end fade's looping
// bit through function_0bfe40 (592, as listed): the seed first 600, a pointer to the sound's source for the final listener
// test 591, that with 0x21b940's bounds pointer 591, that with track ahead of track_definition 591. Reading: retail's
// constant 1 sits in EBX across the start arm because EBX holds `state` there and state is dead in that arm (state == 0
// threads past the `state != 2` test); this build holds NONE in EBX for the whole pass. Body unchanged.
// M3 lane 11 r165 (fast screen with the two callers and the matched callees, control 597 at 2518 B). The allocator ranks
// the track definition's whole-loop range at 132 (576 for its 24 referencing blocks less 444 for the 64 blocks it is
// only live through) and gives it EBP, the last free callee-saved register; the old-end sound ranks 120. With the track
// definition ranked below the old-end sound (a forced order, not source) the count is 501, and with it ranked last the
// aligned rows fall by a third, leaving the entry's flags byte, the alternate compares and the constant 1: so 13
// points between those two ranges are what a source form has to move. Retail reuses `state`'s parameter home for the
// last listener test's float argument, which is why `state` sits in EBX from the latch to the final store. Forms: the
// three state pairs through a member setter of the track record 597 (same code); the old-end fade as an unguarded call
// to 0x126df0 779 (2400 B, stays a call), guarded 739 (re-measured); the start test as two nested ifs 597 (same code);
// the start sound named once for its test and both start calls 597; the track's short index named once for the six
// calls 597. Diagnostics, removed at once: a forced re-read of field_c_4 in the start arm gives retail's store, reload,
// store but each arm keeps its own copy and nothing else moves (597); a private forced-inline copy of 0x126df0's text
// at the old-end fade 595, so the hand-written block is already that expansion's code; in this loop the link expands a
// 0x126df0 call at the loop top or under one `if` and calls it under two, while retail's old-end fade sits under
// three. Body unchanged.
// M3 lane 12 r189 (reduced-image screen with the two callers and the matched callees listed, control 597 as the full
// check; with only the callers listed the image reads 694). Forms aimed at the track definition's reference count: the
// track's flags read once into a local at the top of the pass 688 (frame +12), the same local declared after the
// alternate test 684 (frame +12); the fade-in duration named once at the top of the start arm for its three uses 618
// (aligned rows 512 -> 430 at retail's frame); the start arm's two state stores as one value (`field_c_4 = 0;
// next_state = 0;`) 597 (same code); the old-end fade's duration read at its use with no local 635 (frame -4); the end
// sound index named ahead of the start test 597 (same code). Body unchanged.
// Lane AO r3 (fast with the two callers and 17 callees, control 597 at 2518 B): the creation arm tested first (`if
// (index == NONE)`, the 0x10 test in its else) 607; both energy loops as `for` loops 769 (2596 B); the floor clamp as an
// `if` 703 (2500 B); the three state pairs as chained assignments 597 (same code); the lookup identifier set by an `if`
// 669; the transition walk without its reference pointer (offset tested, the field read at each use) 589 at 2522 B, NOT
// kept: retail keeps the reference's NULL test (`add eax, edx; je`) and the length moves away; the gains array declared
// at the head of the combine block 597 (same code), with `alternate` inside the definition guard 615. Body unchanged.
// M3 lane 1 r171 (fast screen with the two callers and 13 callees, control 597; the allocator gives the entry's flags
// byte EBX at priority 31 over the entry blocks, where retail holds the constant 1 there): `alternate` as `(flags & 1) ?
// true : false` 597 (same code); `bool alternate = flags & 1;` 597 (same code); `result` declared above `alternate` 597
// (same code); the resume test as `state == 0 && (flags & 0x10)` 632 (2499 B); the missing-definition path as an early
// `return true` in place of the final else 779 (2497 B). Body unchanged.
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
							function_142700((transform4x3f const *)((byte *)g_4e6380 + 0x90 + listener * 0x48), &position, &position);
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
