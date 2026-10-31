// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_21D110.CPP: the sound effects in g_51ebe0 ("sounds effects",
   0x10 elements of 0x14 bytes): a sound started through an effect plays from
   the effect's source table g_44a1c0, which wraps the sound's own source and
   keeps a sound record for it */

#include "unknown_11c920.h"
#include "data_array.h"
#include "sound_sources.h"
#include "sound_records.h"
#include <string.h>
#include <stddef.h>

/* the permutations a sound effect plays (0x2c..0x34 of a sound class or
   platform playback) */
struct s_sound_effect_definition
{
	byte unknown00[8];
	long count;
	struct s_sound_effect_entry *entries;
};

struct s_sound_effect_entry
{
	dword group;
	long tag_index;
	real gain;
	dword flags;
};

struct s_sound_effect_definition_block
{
	byte unknown00[0x2c];
	long count;
	s_sound_effect_definition *definitions;
};

struct s_sound_effect_class_flags
{
	byte unknown00[8];
	byte flags;
};

#define FLAG(bit) (1 << (bit))
#define TEST_BIT(flags, bit) (((flags) >> (bit)) & 1)
#define SET_BIT(flags, bit, value) ((value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)))

enum
{
	name_756383 = 0,
	_sound_effect_flag1_bit,
	_sound_effect_flag2_bit,
	_sound_effect_unmanaged_bit,
	_sound_effect_stopped_bit
};

/* a sound effect (0x14 bytes) */
struct s_sound_effect
{
	short salt;
	byte unknown02;
	char type;
	union
	{
		struct { byte flags; byte unknown05; };
		struct
		{
			word flag0 : 1;
			word flag1 : 1;
			word flag2 : 1;
			word flag3 : 1;
			word flag4 : 1;
			word unknown_flags : 11;
		};
	};
	short priority;
	long record_index;
	union
	{
		long sound_index;
		real scale;
	};
	s_sound_effect_definition *definition;
};

/* a sound's state as unknown_124f90.cpp sees it (the first 0x44 bytes are a
   s_type_99c531) */
struct s_sound
{
	word flag0 : 1;
	word flag1 : 1;
	word override_minimum_distance : 1;
	word override_maximum_distance : 1;
	word unknown00_4 : 4;
	word flag8 : 1;
	word flag9 : 1;
	word unknown00 : 6;
	byte unknown02[0x34 - 2];
	real minimum_distance;
	real maximum_distance;
	byte unknown3c[8];
};

/* a playing sound in g_4e637c (0xbc bytes) */
struct s_playing_sound
{
	byte unknown00[0xc];
	long definition_index;
	byte unknown10[4];
	s_sound_source_callbacks const *source;
	s_type_99c531 location;
	s_sound_effect_marker marker;
	byte unknown8c[0xbc - 0x8c];
};

/* the sound system's state, as these functions read it */
struct s_sound_system_effect_view
{
	byte unknown00[0x78];
	bool initialized;
	bool hardware_available;
	bool enabled;
};

struct s_4e6380;
extern s_4e6380 *g_4e6380;
extern s_record_pool *g_4e637c;
extern void *g_51ebd8;
extern void *g_51ebe0;

void *function_18d090(long tag_index, long handle);
struct s_sound_class_definition;
s_sound_class_definition *sound_get_class(long tag_index);
real sound_get_minimum_distance(s_sound const *sound, long definition_index);
real sound_get_maximum_distance(s_sound const *sound, long definition_index);
s_record_pool *function_11cc20(long maximum_count, const char *name, long size);

bool function_126c30(s_sound_play_state *state, long tag_index, long *listener_index, long *reason);
long sound_definition_rate_limited(long definition_index, long *stage_index); /* unknown_124f90.cpp */
long function_126000(long tag_index, long listener_index, s_sound_play_state *state, long rate_limit_stage);
bool sound_playback_update_source(long sound_index, s_sound_source_callbacks const *source, s_sound_playback_flags *flags); /* unknown_124f90.cpp */
void __stdcall function_21d630(long effect_index, long mode);
void function_18cbc0(long looping_sound_index, s_type_99c531 *location);

extern s_sound_source_callbacks const g_44a1c0;

static inline bool sound_system_available(void)
{
	s_sound_system_effect_view *sound_system = (s_sound_system_effect_view *)g_4e6380;
	return sound_system->initialized && sound_system->hardware_available && sound_system->enabled;
}

static inline s_record_pool *function_x39bdd5(void)
{
	return (s_record_pool *)g_51ebe0;
}

static inline s_sound_effect *sound_effect_get(long effect_index)
{
	return &((s_sound_effect *)function_x39bdd5()->data)[effect_index & 0xffff];
}

static inline s_playing_sound *playing_sound_get(long sound_index)
{
	return &((s_playing_sound *)g_4e637c->data)[sound_index & 0xffff];
}

static inline long sound_effect_get_sound_index(long effect_index, s_sound_effect *effect)
{
	return effect->type == 0 ? (effect_index | 0x4000) : effect->sound_index;
}

static inline long sound_effect_new(s_sound_effect_definition *definition)
{
	long effect_index = NONE;

	if (definition->count)
	{
		effect_index = record_pool_allocate(function_x39bdd5());
		if (effect_index == NONE)
		{
			return effect_index;
		}

		s_sound_effect *effect = sound_effect_get(effect_index);
		effect->definition = definition;
		effect->record_index = NONE;
	}
	return effect_index;
}

/* starts a sound without an effect */
static inline long sound_start(s_sound_play_state *state, long tag_index)
{
	long result = NONE;
	long listener_index;
	long rate_limit_stage;

	if (sound_system_available() && function_126c30(state, tag_index, &listener_index, NULL))
	{
		if (sound_definition_rate_limited(tag_index, &rate_limit_stage))
		{
			result = NONE;
		}
		else
		{
			result = function_126000(tag_index, listener_index, state, rate_limit_stage);
		}
	}
	return result;
}

// @retail 0x21dd30
bool sound_effect_get_definition(long tag_index, long platform_playback, s_sound_effect_definition **definition)
{
	bool result = false;
	long const *local_0 = &platform_playback;
	s_sound_effect_definition_block *playback = (s_sound_effect_definition_block *)function_18d090(tag_index, *local_0);
	s_sound_effect_definition_block *sound_class = (s_sound_effect_definition_block *)sound_get_class(tag_index);
	s_sound_effect_definition_block *block = NULL;
	if (playback && playback->count)
	{
		block = playback;
	}
	else if (*local_0 == NONE && sound_class && sound_class->count)
	{
		block = sound_class;
	}
	if (block && playback->count > 0)
	{
		s_sound_effect_definition *definitions = playback->definitions;
		if (definitions)
		{
			result = definitions->count > 0;
			*definition = definitions;
		}
	}
	return result;
}

// @retail 0x21dc60
void sound_effect_attach(long effect_index, s_sound_play_state *state)
{
	s_sound_effect *effect = sound_effect_get(effect_index);
	volatile long *local_0 = &state->effect_marker.link.effect_index;
	state->source_data_size = sizeof(s_sound_effect_marker);
	if (state->flags & 0x20)
	{
		state->effect_marker.link.source = state->source;
	}
	else
	{
		effect->flags |= FLAG(_sound_effect_unmanaged_bit);
		state->effect_marker.link.source = NULL;
	}
	*local_0 = effect_index;
	state->source = &g_44a1c0;
	state->flags |= 0x20;
	if (state->flags & 0x10)
	{
		effect->record_index = looping_sound_controller_find_and_reference(state->effect_index);
	}
	else
	{
		long key = effect_index | 0x4000;
		effect->record_index = function_219a90(key);
		if (effect->record_index != NONE)
		{
			state->effect_index = key;
			state->flags |= 0x10;
		}
	}
}

#pragma inline_depth(0)
PRIVATE __forceinline void function_21dcf2(long arg_0, s_record_pool *arg_1)
{
    s_sound_effect *local_0 = &((s_sound_effect *)arg_1->data)[arg_0 & 0xffff];
    if (local_0->record_index != NONE)
    {
        looping_sound_controller_release(local_0->record_index);
        local_0->record_index = NONE;
    }
    record_pool_release(arg_1, arg_0);
}
#pragma inline_depth(255)

#pragma inline_depth(1)
// @retail 0x21dcf0
void __cdecl sound_effect_delete(long effect_index)
{
    function_21dcf2(effect_index, (s_record_pool *)g_51ebe0);
}
#pragma inline_depth(255)


PRIVATE __forceinline void function_21dcf1(long effect_index)
{
	s_record_pool *effects = function_x39bdd5();
	s_sound_effect *effect = &((s_sound_effect *)effects->data)[effect_index & 0xffff];

	if (effect->record_index != NONE)
	{
		looping_sound_controller_release(effect->record_index);
		effect->record_index = NONE;
	}
	record_pool_release(effects, effect_index);
}

#define sound_effect_delete function_21dcf1
// score 204 (was 204): BLOCKED-BY 0x189fe0. Frame eight versus four, the state/tag registers and the
// opposite start/delete expansion remain (ours takes the tag in EAX and the state on the stack).
// Round 102: the Round 72 form again 52 (429 B), 0x189fe0 110 -> 135 and its eight callers lost
// (0x189fe0 then expands the first class lookup and keeps the request in EDX; retail calls both
// lookups); with the parameters declared tag first, consistently at the caller and header, the same;
// the private starter forceinline 256 (542 B, callers lost); the priority stored through the effect
// pointer 220; the allocation as one conditional expression 233.
// Round 105: only the no-effect start through 0x125f70 with the delete out of line 224 (callers
// kept); only the effect start through it with the delete out of line 175 (state in EAX, the eight
// callers lost); the first with the priority through the effect pointer 214, with the start-result
// arms swapped 225. Every form that gives the state EAX (the effect start through 0x125f70) moves
// 0x189fe0's request to EDX, where it then stays (no copy to EBX).
// Round 72: both starts as calls to the existing
// 0x125f70 (reason NULL), which LTCG inlines as retail, plus the out-of-line delete (this file's
// delete macro moved below this function) give 52 plain (429/427): retail's frame and its
// state-in-eax convention; with the priority stored through the same effect pointer 94 at 427
// bytes, every block retail's except that our build threads the NONE start results straight to
// the delete where retail rejoins its `cmp esi, -1` (inverted test 110, single result 94). Not kept:
// 0x189fe0 then takes its request in edx, not eax, and its eight matched callers (0x1890c0,
// 0x189340, 0x1895f0, 0x189650, 0x1896c0, 0x189710, 0x189760, 0x189a50) lose their match.
// Round 74 (0x189fe0's file held by another lane): parameters declared tag first, consistently at the
// caller 204 (convention unchanged); out-of-line delete alone 314 (271 bytes); starts through 0x125f70
// alone 256 (542 bytes, the eight callers lost); allocation through the existing function_21d2c1 204;
// definition and effect declared before the platform handle 221.
// Earlier forms: joined query/allocation 220, unforced failure-first 221, effect pointer scope 220,
// shifted platform flag 220, inline depth zero 391; uninitialized definition 236; scoped definition
// and query-result value 220; pre-start receiver 208 (lost callers); record getter at each store
// 208 (kept); kind/sound publication orders and class predicate 220; allocator reuse/const
// class/request reference/tested result/combined 220/220/220/236/220; global/size/depth-two
// policies 246/359/220; share record type sound 204; share record sound priority 211; share
// record type priority 204; getter body single result 207; getter body definition guard 224; type
// sound with single result 229; type sound with definition guard 221; external published record
// 220; native effect handle 215; immutable playback/result 208; query/attach/both call boundaries
// 208/208/208; record plus native handle 221; native boundaries 215; native class byte/bool bits
// with Boolean/direct tests 204 each; byte bits plus shared allocator 204; byte bits/settings and
// bool bits/parameters 204/204; byte bits with shared allocator/settings 204;
// availability/allocation/accessor expansion declarations and all three together 204 each;
// immutable state/tag/both input bindings 204 each; all boundaries with both bindings 204;
// creator noinline/inline+noinline/inline 204 each; outlined creator with shared start expansion
// 80/80 and query/attachment/both call boundaries with it 80 each (all lose the eight callers);
// complete admission scope with branch-defined allocation/nested class guards 204 each; shared
// start with those forms 232/226/232/226; query-owned handle 204; availability
// failure/result/admission 204/204/204, with the query-owned handle 204/204/204; preserved class
// bit 180 plain (eight caller losses); logical/masked/clear-only flag 165/215/174; scoped logical
// flag 165; at-use record getters with compound/logical/masked/clear-only flags 185/164/220/177
// (all gains lose callers); class/activation/listener/rate-query inline-noinline declarations 204
// each; listener/stage/both output names 204 each; reversed output declarations 204;
// listener/stage/both outputs initialized absent or zero 204 each; per-use playback selection
// alone/with absent outputs 199 (caller losses); complete start result 226; positive start
// admission 204; exhaustive start arms 226; rate result selection 204; query owned stage 204;
// admission owned outputs 204; scoped positive rate 204; scoped exhaustive results 226;
// long/word effect slots at admission, playback and publication 206/214 each; long/word slots
// with common result 181/224; long-result 181 plain (eight caller losses).
// Native sound/type/priority, priority/type/sound, type/priority/sound, sound/priority/type,
// priority/sound/type publication 204/219/204/204/219; split-result entry home with
// original/sound-first/priority-first publication 204/204/219.
// Round 130: native sound/playback carriers 204/204; native effect 212; playback
// with sound 204; effect with sound/playback/both 212 each; all native handles
// with entry sound declaration 212. Effect-domain forms lose eight caller controls.
// Round 327: current deletion record/pool/both 211/204/206; public creator noinline 204;
// with record/pool/both 211/204/206; both observations and boundary with signed class byte 206.
// Round 177: Complete start admission bool/byte/short/long and their complete complements
// 204 each; every sensitive caller control remains matched. Original helper and body restored;
// eight-byte frame and opposite start/delete expansion remain.
// Round 207: existing allocator as native definition member, with original failure return,
// success nesting, count-selected result or empty-definition return, all 204; those same
// four with record-link initialization first also 204. Original helper and body restored.
// Round 214: direct activation returns through sequential, combined, mixed and nested admission, each
// with entry/query output declarations, all 226 (357/427); frame four recovered, eight caller controls intact. Restored.
// Round 148: coherent private start tag/state order, unsigned result, unsigned tag, their pair/all
// combinations and order with default-NONE rate admission 204 each.
// Round 179: global availability fields at each use, direct/complete/failure/staged result
// at Boolean storage 204/204/204/204; native byte storage 204/204/204/204.
// Original availability receiver, helper, field types and expansion policy restored.
// Round 249: existing starter static ordinary, external inline/ordinary, static
// noinline ordinary/inline, external noinline ordinary/inline, and ordinary starter
// plus ordinary availability declarations 204 each; all eight caller controls preserved.
// Round 61 (lane 7): the Round 72 form reproduced, 52 plain (429 B), 0x189fe0 135 and the eight callers
// lost; only the effect path's start through 0x125f70 175 (still state in EAX, callers lost); only the
// no-effect start through it 224 (callers kept, 0x189fe0 132); with 0x189fe0 reading the tag record at
// each use 52 (0x189fe0 134, callers lost); with a noinline declaration ahead of 0x221810 52 (0x189fe0
// on stack arguments, 156, callers lost); that declaration alone also loses the callers. Retail
// calls 0x221810 out of line and re-reads the state after it, which keeps 0x189fe0's request in EBX.
// Round 206: Five remaining availability-field conjunction orders and three complementary
// failure-first orders 204 each; all eight sensitive caller controls remain matched.
// Round 151: Existing two-use starter output result, four private parameter-role orders: final publication 196 each, direct publication 189 each; every form loses eight sensitive callers in the fast image. All restored.
// Frame eight/four, state/tag homes and opposite start/delete expansion remain; protected caller cycle unchanged.
// Round 27 (lane 9), each with the Round 72 form (52 at 429 bytes): in 0x189fe0 a named object_index local 52/0x189fe0 135
// (request still in EDX, the eight callers one to four lines off); with a noinline declaration ahead of 0x221810 52/156 (both
// arguments on the stack, request homed in EBP, flags in EBX); priority and flags formed from the request before the location
// copy (retail's order) 52/136, with the noinline declaration 52/151 (stack arguments). Callers lost in every form; restored.
// Round 63: platform handle by an if statement 211; nested class/flag tests 204; definition query and
// allocation as one ternary 233; start-result arms swapped (delete first) 206; effect index declared
// before the definition pointer 204.
// Round 17: noinline target with the forced start/direct delete/common result form 80 (eight caller
// losses); whole-body tag reference 207 (eight caller losses); single address-based tag reads at the
// definition query, class lookup, effect start and fallback start 204 each; noinline target alone and
// with the fallback address read 204 each. Original body and declarations restored.
// Round 258: accessor word/short/dword inputs 213/213/204; separate noinline
// declarations with those inputs 256 each; external inline word/short inputs
// 213/213. All eight sensitive callers preserved; original accessor restored.
// Round 206: Native C contracts on private starter/allocator/record accessor, seven subsets
// [204, 204, 204, 204, 204, 204, 204]; all three standard callee-cleanup 204. Original contracts restored.
// Round 179: the private starter forced inline with the real out-of-line delete 52 (429 B; 0x189fe0
// 110 -> 135, its request moves from EAX to EDX and all eight matched callers lose their match: the
// same blocker as the 0x125f70 route); the rate-limit test negated with no NONE arm 204; the platform
// handle computed after the definition/effect declarations 221 (0x189fe0 132); sound_index declared
// at the top 204; the allocation folded into the definition test 204. Still BLOCKED-BY 0x189fe0.
// @retail 0x21d110
long function_21d110(s_sound_play_state *state, long tag_index)
{
	long platform_playback = (state->flags & 0x100) ? state->platform_playback : NONE;
	s_sound_effect_definition *definition = NULL;
	long effect_index = NONE;

	if (sound_effect_get_definition(tag_index, platform_playback, &definition))
	{
		effect_index = sound_effect_new(definition);
	}

	if (effect_index != NONE)
	{
		s_sound_effect_class_flags *sound_class = (s_sound_effect_class_flags *)function_18d090(tag_index, platform_playback);
		if (sound_class && (sound_class->flags & 1))
		{
			state->location.flag8 = true;
		}
		sound_effect_attach(effect_index, state);

		long sound_index = sound_start(state, tag_index);
		if (sound_index != NONE)
		{
			s_sound_effect *effect = sound_effect_get(effect_index);
			effect->type = 1;
			effect->sound_index = sound_index;
			sound_effect_get(effect_index)->priority = (short)state->priority;
			function_21d630(effect_index, 0);
		}
		else
		{
			sound_effect_delete(effect_index);
		}
		return sound_index;
	}
	return sound_start(state, tag_index);
}

PRIVATE __forceinline long function_21d2c1(s_sound_effect_definition *arg_0)
{
	long local_0 = NONE;
	if (arg_0->count)
	{
		local_0 = record_pool_allocate(function_x39bdd5());
		if (local_0 != NONE)
		{
			s_sound_effect *local_1 = sound_effect_get(local_0);
			local_1->definition = arg_0;
			local_1->record_index = NONE;
		}
	}
	return local_0;
}

// @retail 0x21d2c0
long function_21d2c0(long platform_playback, real scale, short priority)
{
	long effect_index = NONE;
	s_sound_effect_definition *definition = NULL;

	if (sound_effect_get_definition(NONE, platform_playback, &definition))
	{
		effect_index = function_21d2c1(definition);
		if (effect_index != NONE)
		{
			s_sound_effect *effect = sound_effect_get(effect_index);
			effect->type = 0;
			sound_effect_get(effect_index)->scale = scale;
			effect->priority = priority;
			function_21d630(effect_index, 0);
		}
	}
	return effect_index;
}

// @retail 0x21d360
void sound_effect_stop(long effect_index)
{
	s_sound_effect *effect = sound_effect_get(effect_index);

	effect->flag3 = true;
	effect->flag4 = true;
	function_21d630(effect_index, 2);
	effect->flag0 = true;
}

// @retail 0x21d5a0
bool function_21d5a0(long effect_index)
{
	bool result = false;
	s_sound_effect *effect = sound_effect_get(effect_index);

	switch (effect->type)
	{
	case 0:
		result = !TEST_BIT(effect->flags, _sound_effect_stopped_bit);
		break;
	case 2:
		return result;
	case 1:
		if (!(effect->flags & (FLAG(_sound_effect_unmanaged_bit) | FLAG(_sound_effect_stopped_bit))))
		{
			long local_0 = effect->sound_index;
			s_sound_effect_link const &local_1 = playing_sound_get(local_0)->marker.link;
			s_sound_playback_flags flags;
			memset(&flags, 0, sizeof(flags));
			result = sound_playback_update_source(local_0, local_1.source, &flags);
			if (TEST_FIELD_BIT(flags.source_updated))
				effect->flags |= FLAG(_sound_effect_unmanaged_bit);
		}
		else
		{
			result = true;
		}
		break;

	}
	return result;
}

// @retail 0x21d390
void sound_effects_update(void)
{
	struct { s_sound_effect *field_0; s_record_pool_iterator field_4; } local_0;

	local_0.field_4.data = function_x39bdd5();
	local_0.field_4.index = NONE;
	local_0.field_4.datum_index = NONE;
	while ((local_0.field_0 = (s_sound_effect *)data_iterator_next_inlined(&local_0.field_4)) != NULL)
	{
		if (TEST_BIT(local_0.field_0->flags, name_756383))
		{
			sound_effect_delete(local_0.field_4.datum_index);
		}
		else if (function_21d5a0(local_0.field_4.datum_index))
		{
			function_21d630(local_0.field_4.datum_index, 1);
		}
	}
}

// @retail 0x21d490
bool sound_effects_initialize(void)
{
	g_51ebe0 = function_11cc20(0x10, "sounds effects", sizeof(s_sound_effect));
	if (g_51ebe0)
	{
		s_record_pool *effects = function_x39bdd5();
		effects->valid = true;
		record_pool_release_all(effects);
	}
	return g_51ebe0 != NULL;
}

// @retail 0x21d4d0
void function_21d4d0(void)
{
	struct { byte *field_0; s_record_pool_iterator field_4; } local_0;

	local_0.field_4.data = function_x39bdd5();
	local_0.field_4.index = NONE;
	local_0.field_4.datum_index = NONE;
	while ((local_0.field_0 = data_iterator_next_inlined(&local_0.field_4)) != NULL)
	{
		sound_effect_delete(local_0.field_4.datum_index);
	}
}

// @retail 0x21db80
void sound_effect_update_location(long effect_index, s_type_99c531 *location)
{
	s_sound_effect *effect = sound_effect_get(effect_index);

	switch (effect->type)
	{
	case 0:
		location->audible = 0;
		location->requested_audible = 0;
		location->scale = effect->scale;
		location->unknown08 = 0;
		break;
	case 1:
		{
			s_playing_sound *sound = playing_sound_get(sound_effect_get_sound_index(effect_index, effect));
			s_type_99c531 *arg_26d7e7 = &sound->location;

			if (location != arg_26d7e7)
			{
				*location = *arg_26d7e7;
				if (location->flags & 0x100)
				{
					s_sound *distances = (s_sound *)location;

					distances->minimum_distance = sound_get_minimum_distance((s_sound *)arg_26d7e7, sound->definition_index);
					location->flags |= 4;
					distances->maximum_distance = sound_get_maximum_distance((s_sound *)arg_26d7e7, sound->definition_index);
					location->flags |= 0x208;
				}
			}
		}
		break;
	case 2:
		function_18cbc0(sound_effect_get_sound_index(effect_index, effect), location);
		break;
	}
}

static inline s_sound_effect_marker const *sound_effect_marker(void const *marker)
{
	return (s_sound_effect_marker const *)marker;
}

// @retail 0x21d970
bool __stdcall sound_effect_only_update(long object_index, long tag_index, s_sound_marker const *marker, s_type_99c531 *location)
{
	long effect_index = sound_effect_marker(marker)->link.effect_index;
	s_sound_effect *effect = (s_sound_effect *)datum_get_inlined(function_x39bdd5(), effect_index);
	bool result = effect && !TEST_FIELD_BIT(effect->flag0);
	if (result && !TEST_FIELD_BIT(effect->flag4))
		sound_effect_update_location(effect_index, location);
	return result;
}

// @retail 0x21d9f0
bool __stdcall sound_effect_source_update(long object_index, long tag_index, s_sound_marker const *marker, s_type_99c531 *location)
{
	long const volatile *local_0 = &sound_effect_marker(marker)->link.effect_index;
	long effect_index = *local_0;
	bool result = !TEST_BIT(sound_effect_get(effect_index)->flags, _sound_effect_stopped_bit);

	if (result)
	{
		sound_effect_update_location(effect_index, location);
	}
	return result;
}

// @retail 0x21da30
void __stdcall sound_effect_source_proc1(long object_index, long tag_index, long a, long marker)
{
	s_sound_source_callbacks const *source = sound_effect_marker((void const *)marker)->link.source;

	if (source && source->proc1)
	{
		source->proc1(object_index, tag_index, a, marker);
	}
}

// @retail 0x21da50
void __stdcall sound_effect_source_proc2(long object_index, long marker, long tag_index, long set_index, long permutation, long scale)
{
	s_sound_source_callbacks const *source = sound_effect_marker((void const *)marker)->link.source;

	if (source && source->proc2)
	{
		source->proc2(object_index, marker, tag_index, set_index, permutation, scale);
	}
}

// @retail 0x21da70
bool __stdcall sound_effect_source_spatialize(long object_index, long tag_index, s_sound_source_view const *source, s_sound_spatialization_view *spatialization)
{
	s_sound_source_callbacks const *previous = sound_effect_marker(source)->link.source;
	bool result = false;

	if (previous && previous->spatialize)
	{
		result = previous->spatialize(object_index, tag_index, source, spatialization);
	}
	return result;
}

// @retail 0x21da90
void __stdcall sound_effect_source_stop(long object_index, long sound_index, long reason)
{
	if (reason != 10)
	{
		s_sound_effect_link *link = &playing_sound_get(sound_index)->marker.link;
		s_sound_effect *effect = sound_effect_get(link->effect_index);

		if (link->source && link->source->stop)
		{
			link->source->stop(object_index, sound_index, reason);
		}
		if (reason != 2)
			effect->flag1 = true;
		else
			effect->flag1 = false;
		sound_effect_stop(link->effect_index);
	}
}

#pragma inline_depth(0)
// @retail 0x21db30
void __stdcall sound_effect_source_detach(long object_index, long sound_index)
{
	sound_effect_source_stop(object_index, sound_index, 1);
	s_playing_sound *sound = &((s_playing_sound *)g_4e637c->data)[sound_index & 0xffff];
	s_sound_effect_link *link = &sound->marker.link;
	sound->source = link->source;
	memset(link, 0, sizeof(*link));
}
#pragma inline_depth(255)

extern s_sound_source_callbacks const g_44a1c0 = { sound_effect_source_update, sound_effect_source_proc1, sound_effect_source_proc2, sound_effect_source_spatialize, sound_effect_source_stop, sound_effect_source_detach, NULL, NULL };
extern s_sound_source_callbacks const g_44a1e0 = { sound_effect_only_update, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

struct s_effect_controller_view
{
	long unknown00;
	long definition_index;
	byte unknown08[0x1c - 8];
};

struct s_effect_looping_view
{
	short salt;
	char state;
	byte unknown03;
	byte flags;
	byte unknown05[0x18 - 5];
};

typedef char check_sound_effect_size[sizeof(s_sound_effect) == 0x14 ? 1 : -1];
typedef char check_effect_controller_size[sizeof(s_effect_controller_view) == 0x1c ? 1 : -1];
typedef char check_effect_looping_size[sizeof(s_effect_looping_view) == 0x18 ? 1 : -1];
typedef char check_effect_request_gain_offset[offsetof(s_sound_play_state, gain) == 0x94 ? 1 : -1];

extern void *g_51ebdc;
bool function_21a250(long definition_index, dword sound_flags, long identifier, long controller_definition_index,
	s_type_99c531 *source, long state, dword flags, real fade_duration);

PRIVATE long const g_44a200[] = { 0, 1, 2 };
PRIVATE long const g_44a20c[] = { 0, 1, 2, 2 };

// score 30 (was 30): every remaining difference is a stack offset. Retail keeps the loop bool
// every remaining difference is a stack offset. Retail keeps the loop bool in a
// dword-aligned unit (its low byte) just above repeat_state; ours is a one-byte home in the lowest unit,
// which shifts the lighter dwords by four and reorders the heavier ones. Retail's order: sound flags,
// effect, entry offset, controller, state, index, rate stage, repeat state, bool, listener, tag,
// definition. Holding repeat_state and the bool in one 8-byte local record matches byte for byte
// (diagnostic only: a record that only groups locals is not kept). Any size-1 local takes the lowest
// unit (one-element array, one-member struct, address taken); a size-4 bitfield struct moves it but
// changes the code. The 'lsnd' state/alternate as two ternaries on one named condition gave retail's
// register roles (60 -> 30).
// Tried (fast previews, fast baseline 58-67): the record before the ternaries 32; the tag lookup before
// the entry flags 78; the entry flags read at each use 78; alternate as an if/else-if chain 133; state
// before alternate in the looping arm 67; alternate before state in the other arm 136, with a long flag
// 137, with a short flag 194; the group switch as if/else-if 114; renaming either local, or five locals
// to either end of the alphabet (no slot moves); the two declared the other way round 67; both
// initialised 92; the bool initialised 201-202; defaults in an else arm 208 (839 bytes); `(flags & 0x10)
// != 0` 58; listener/rate stage declared the other way round 58; a byte flag 140; a long flag 148; a
// short flag 129; a char repeat state 190; dword/word/byte flags with long/char/short state 151-199; a
// complete looping snapshot 226; the existing sound-system availability helper for the 'snd!' case's
// three locals 156 (816 bytes); the definition read from the effect at each use 188; the controller
// chosen with a ternary 77; direct alternate in SET_BIT 139; function-scope looping pointer 58; the
// looping datum through datum_get_inlined 259 (856 bytes); the looping pointer as a function-scope
// ternary tested for NULL 197; repeat_state read before the bool 192; the pair declared at the top 58;
// use_loop_state const 58. Screened (one-file LTCG and compile-only frame listings, which reproduce our
// slot order): bool types long/dword/short/word/char/byte, an inline out-parameter helper, the loop in an
// inline helper taking the pair as parameters, const definition, entry pointer arithmetic, the tag read
// after its test, the controller lookup in its own scope or through 0x2198f0, the loop in its own scope,
// the definition or tag index declared earlier: the bool stays at the bottom.
// Round 48 (Kimi 2): repeat_state and loop_alternate held in one named 8-byte record matched byte for byte
// (0 at 823); not kept, a struct that only groups locals to move their stack homes.
// Earlier note (30, was 60): every remaining difference is a stack offset. Retail keeps the loop bool
// score 30 (was 30): every remaining difference is a stack offset. Retail keeps the loop bool
// in a dword-aligned unit (its low byte) between repeat_state and the listener; ours is a
// one-byte home in the lowest unit, which shifts the lighter dwords by four and reorders the
// heavier ones. Retail's order: sound flags, effect, entry offset, controller, state, index,
// rate stage, repeat state, bool, listener, tag, definition.
// Round 103: the 'lsnd' state/alternate if/else as two ternaries on one named condition gave
// retail's register roles (60 -> 30). On it, holding repeat_state and the bool in one 8-byte local
// record matches byte for byte (diagnostic only: a record that only groups locals is not kept).
// Tried: the record before the ternaries 32; the tag lookup before the entry flags 78; the entry
// flags read at each use 78; alternate as an if/else-if chain 133; state before alternate in the
// looping arm 67; the group switch as if/else-if 114; renaming either local, or five locals to
// either end of the alphabet (no slot moves); the two declared the other way round 67; both
// initialised 92 (round 103, fast against 58: 80); the bool initialised 202 (fast); `(flags & 0x10)
// != 0` 58 (fast, same as the base); listener/rate stage declared the other way round 58 (fast);
// a byte flag 140; a char
// repeat state 190; dword/word/byte flags with long/char/short state 151-199; a complete looping
// snapshot 226; the existing sound-system availability helper for the 'snd!' case's three locals
// 156 (816 bytes); the definition read from the effect at each use 188. Screened (one-file LTCG
// and /FAs): bool types long/dword/short/word/char/byte, an inline out-parameter helper, const
// definition, entry pointer arithmetic and the tag read after its test leave the frame unchanged.
// the local variables sit in stack slots that are four bytes apart from retail's,
// so every load and store of a local uses a different offset; the 'lsnd' case holds the entry flags in
// eax and the tag base in ecx (retail the reverse).
// The lone bool loop_alternate always takes a byte slot below the dwords; retail gives it a dword slot
// just above repeat_state (both frames leave the lowest dword unused). Holding the two in one 8-byte
// record gives retail's frame exactly (32 at 823 bytes; not kept: a record that only groups locals).
// Our order from the bottom: bool, sound flags, effect, entry offset, controller, state, index, rate
// stage, definition, listener, repeat state, tag; retail's: sound flags, effect, entry offset,
// controller, state, index, rate stage, repeat state, bool, listener, tag, definition.
// Tried: the tag lookup before the entry flags 78 (fast; baseline fast 67); alternate as an
// if/else-if chain 133; state before alternate in the looping arm 67; the group switch as if/else-if
// 114; renaming either local 67 (no slot moves: names do not order this frame); the two declared the
// other way round 67; both initialised 92 (repeat_state reaches retail's slot, the bool stays a byte);
// the entry flags read at each use 78; dword flags/long state 199; dword flags/char state 192; word
// flags/long state 151; word flags/char state 162; byte flags/long state 193; byte flags/char state 195;
// dword flags/short state 152; complete native looping snapshot 226; the bool renamed to sort first or
// last 67 each; a byte flag 140; a char repeat state 190; the existing sound-system availability helper
// for the three locals of the 'snd!' case 156 (816 bytes); the definition read from the effect at each
// use 188. Round 20 (fast, baseline 67): a long flag 148, a short flag 129 (both land in the dword area
// but elsewhere); alternate before state in the other arm 136, with a long flag 137, with a short flag
// 194. Screened (one-file LTCG harness, our frame reproduced): the flag initialised, either local
// address-taken, filled by an inline out-param helper (pointer or reference), declared at the top, the
// controller lookup in its own scope or through the existing 0x2198f0 (called out of line), the loop
// in its own scope, the definition in the loop condition or at each use, the tag pointer after the
// arms, a long entry flags, the definition or tag index declared earlier, `!= 0` or ternary spellings
// of the flag: the bool stays at the bottom and the entry flags in eax (only dropping the definition
// local moves the bool among the dwords).
// so every load and store of a local uses a different offset.
// Round 43: the lone bool loop_alternate always takes a byte slot below the dwords; retail gives it
// a dword slot just above repeat_state. Holding the two in one 8-byte record gives retail's frame
// exactly (32 at 823 bytes; not kept: a record that only groups locals). The 'lsnd' case still
// holds the entry flags in eax and the tag base in ecx (retail the reverse). Tried: the tag lookup
// before the entry flags 78 (fast; baseline fast 67); alternate as an if/else-if chain 133; state
// before alternate in the looping arm 67; the group switch as if/else-if 114; renaming either
// local 67 (no slot moves: names do not order this frame); the two declared the other way round
// 67; both initialised 92 (repeat_state reaches retail's slot, the bool stays a byte); the entry
// flags read at each use 78. Screened (one-file LTCG): declared at the top, the definition or tag
// index declared earlier, the definition read at each use: the bool stays at the bottom.
// before alternate in the looping arm 67; the group switch as if/else-if 114.
// DL180 fast previews: dword flags/long state 199; dword flags/char state 192; word flags/long
// state 151; word flags/char state 162; byte flags/long state 193; byte flags/char state 195;
// dword flags/short state 152; complete native looping snapshot 226. Original executable body
// restored.
// Round 5 (fast previews; baseline fast 67): the bool renamed to sort first or last 67 each (same
// frame); a byte flag 140; a char repeat state 190; the existing sound-system availability helper
// for the three locals of the 'snd!' case 156 (816 bytes); the definition read from the effect at
// each use 188. A compile-only frame listing reproduces our slot order (bool at the bottom, then
// sound flags, effect, state, controller, index, rate stage, definition, listener, repeat state,
// tag) and shows it unchanged under the bool's declaration order, initialisation, `!= 0` or ternary
// spellings and statement orders; only dropping the definition local moves the bool among the
// dwords. Retail's order: sound flags, effect, entry offset, controller, state, index, rate stage,
// repeat state, bool, listener, tag, definition.
// so every load and store of a local uses a different offset. Retail keeps the bool loop_alternate in a
// dword slot just above repeat_state and leaves four bytes unused at the frame bottom; ours gives the bool
// a byte slot at the bottom. Holding the two in one 8-byte record gives retail's frame exactly (32 at 823
// bytes; not kept: a record that only groups locals). The 'lsnd' case still holds the entry flags in eax
// and the tag base in ecx (retail the reverse). Retail's slot order: sound flags, effect, entry offset,
// Tried (fast previews, fast baseline 67): the tag lookup before the entry flags 78; alternate as an
// if/else-if chain 133; state before alternate in the looping arm 67; the group switch as if/else-if 114;
// renaming either local 67 (no slot moves: names do not order this frame); the two declared the other way
// round 67; both initialised 92 (repeat_state reaches retail's slot, the bool stays a byte); the entry flags
// read at each use 78; the bool renamed to sort first or last 67 each; a byte flag 140; a char repeat state
// 190; the existing sound-system availability helper for the three locals of the 'snd!' case 156 (816
// bytes); the definition read from the effect at each use 188; dword/word/byte flags with long/char/short
// state 199/192/151/162/193/195/152; complete native looping snapshot 226. Screened (compile-only frame
// listing, which reproduces our slot order): declared at the top, the definition or tag index declared
// earlier, the definition read at each use, the bool's declaration order, initialisation, `!= 0` or
// ternary spellings and statement orders: the bool stays at the bottom; only dropping the definition local
// moves the bool among the dwords.
// Round 37: defaults in an else arm 208 (839 bytes); `bool loop_alternate = false;` 201; the 'lsnd' else
// arm computing alternate before state 136; `alternate` declared before `state` 67; the controller chosen
// with a ternary 77. Screens: renaming definition/repeat_state/tag_index moves no slot; initialising
// repeat_state reorders several slots but the bool stays a byte; the looping state fetched through an
// inlined helper with out-parameters leaves the frame unchanged.
// Round 126 (fast, fast baseline 58 at 824 bytes; its callees build differently in the reduced image):
// the 'snd!' case's three one-use locals written as direct reads 65 (827 bytes); with a named
// sound-system pointer 65; the loop flag as `(flags & 0x10) != 0` 58; as a ternary 58; as a word
// bitfield (the looping record's flags as a word of bits) 58.
// Round 49 (fast, baseline 58): the alternate expression directly in SET_BIT 139; direct raw gain 58;
// both 139; a function-scope looping pointer with direct gain 58; cumulative const effect, controller index,
// tag flags and tag index on that neutral graph 58 each. Opening body restored.
// Round 29 (lane 11, plain checks; --fast reads 58 here): the definition read before the looping block 201
// (the bool stays at the bottom); alternate as an if/else assignment in the 'lsnd' arm (state ternary kept) 30;
// the flag as `(flags & 0x10) ? true : false` 30; the flag through the file's TEST_BIT 30; the looping pointer
// declared at function scope 30 (its round 49 score was a --fast preview). Compile-only frame listing
// (reproduces our frame: bool at the bottom, then sound flags, entry offset temp, effect, state, controller,
// index, rate stage, definition, listener, repeat state, tag): the pair declared the other way round,
// repeat_state read first with TEST_BIT, repeat_state/loop_alternate renamed to sort first, definition renamed
// to sort first or last: no slot moves. Diagnostics (not keepable): the bool alone with 4-byte alignment moves
// only the bool; the {long, bool} record gives retail's whole order (definition to the top), so retail's flag
// lives in a 4-byte-aligned aggregate with repeat_state.
// Round 9 (fast baseline 58, plain 30): the 8-byte {repeat state, alternate} record now matches byte for
// byte in the plain check (fast 28); not kept, a struct that only groups two locals. A one-file frame probe
// shows a lone bool always takes the bottom byte slot whatever its uses, so retail's dword slot needs the
// pair in one aggregate. Real trials (fast): the looping test named and used twice 210 (830 bytes); the
// definition read before the looping block 201; a function-scope looping pointer 58; one if/else choosing
// state and alternate 67.
// @retail 0x21d630
void __stdcall function_21d630(long effect_index, long mode)
{
	s_sound_effect *effect = sound_effect_get(effect_index);
	s_effect_controller_view *controller = (s_effect_controller_view *)g_51ebdc;
	if (effect->record_index != NONE)
		controller = &((s_effect_controller_view *)((s_record_pool *)g_51ebd8)->data)[effect->record_index & 0xffff];
	long controller_definition_index = controller->definition_index;
	s_type_99c531 location;
	location.flags = 0;
	sound_effect_update_location(effect_index, &location);
	long repeat_state;
	bool loop_alternate;
	if (effect->type == 2)
	{
		s_effect_looping_view *looping = &((s_effect_looping_view *)g_4ed28c->data)[effect->sound_index & 0xffff];
		loop_alternate = (bool)((looping->flags >> 4) & 1);
		repeat_state = looping->state;
	}
	s_sound_effect_definition *definition = effect->definition;
	for (long i = 0; i < definition->count; i++)
	{
		s_sound_effect_entry *entry = &definition->entries[i];
		long tag_index = entry->tag_index;
		if (tag_index == NONE)
			continue;
		switch (entry->group)
		{
		case 0x736e6421:
			if ((mode == 0 && !(entry->flags & 1)) || (mode == 2 && (entry->flags & 2)))
			{
				s_sound_play_state state;
				state.flags = 0x15;
				*(dword *)&state.priority = (word)effect->priority;
				state.location = location;
				if (mode != 2)
				{
					state.flags |= 0x20;
					state.effect_index = controller_definition_index;
					state.source = &g_44a1e0;
					memset(state.source_data, 0, sizeof(state.source_data));
					state.source_data_size = sizeof(state.source_data);
					state.effect_marker.link.effect_index = effect_index;
				}
				long local_0 = *(long *)&entry->gain;
				s_sound_system_effect_view *local_1 = (s_sound_system_effect_view *)g_4e6380;
				bool local_2 = local_1->initialized;
				*(long *)&state.gain = local_0;
				state.flags |= 0x80;
				if (local_2 && local_1->hardware_available && local_1->enabled)
				{
					long listener_index;
					long rate_limit_stage;
					if (function_126c30(&state, tag_index, &listener_index, NULL) &&
						!sound_definition_rate_limited(tag_index, &rate_limit_stage))
						function_126000(tag_index, listener_index, &state, rate_limit_stage);
				}
			}
			break;
		case 0x6c736e64:
			{
				dword entry_flags = entry->flags;
				dword *tag_flags = (dword *)g_4e3b44[tag_index & 0xffff].bytes;
				bool use_loop_state = (entry_flags & 0x20) && effect->type == 2;
				long state = use_loop_state ? g_44a20c[repeat_state] : g_44a200[mode];
				bool alternate = use_loop_state ? loop_alternate : (entry_flags & 8) ||
					((bool)effect->flag4 && (bool)effect->flag1 && (entry_flags & 0x10));
				dword sound_flags = (word)effect->priority;
				if (*tag_flags & 8)
					sound_flags &= ~1;
				if (*tag_flags & 0x40)
					sound_flags &= ~2;
				dword flags = 0;
				SET_BIT(flags, 0, alternate);
				SET_BIT(flags, 1, mode == 2);
				s_type_99c531 source = location;
				*(real *)&source.unknown08 += entry->gain;
				function_21a250(tag_index, sound_flags, sound_effect_get_sound_index(effect_index, effect),
					controller_definition_index, &source, state, flags, 0.0f);
			}
			break;
		}
	}
}

#undef sound_effect_delete
