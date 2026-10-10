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
			effect->priority = (short)state->priority;
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
			s_sound_source_callbacks const *local_1 = playing_sound_get(local_0)->marker.link.source;
			s_sound_playback_flags flags;
			*(volatile word *)&flags = 0;
			result = sound_playback_update_source(local_0, local_1, &flags);
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
	if (effect && !TEST_FIELD_BIT(effect->flag0))
	{
		volatile bool local_0 = true;
		if (!TEST_FIELD_BIT(effect->flag4))
		{
			sound_effect_update_location(effect_index, location);
			return local_0;
		}
		return true;
	}
	return false;
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
	struct { long state; bool alternate; } loop_snapshot;
	if (effect->type == 2)
	{
		s_effect_looping_view *looping = &((s_effect_looping_view *)g_4ed28c->data)[effect->sound_index & 0xffff];
		loop_snapshot.alternate = (bool)((looping->flags >> 4) & 1);
		loop_snapshot.state = looping->state;
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
				state.flags |= 0x80;
				*(long *)&state.gain = *(long *)&entry->gain;
				if (sound_system_available())
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
				long state;
				bool alternate;
				if ((entry_flags & 0x20) && effect->type == 2)
				{
					state = g_44a20c[loop_snapshot.state];
					alternate = loop_snapshot.alternate;
				}
				else
				{
					state = g_44a200[mode];
					alternate = (entry_flags & 8) ||
						((bool)effect->flag4 && (bool)effect->flag1 && (entry_flags & 0x10));
				}
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
