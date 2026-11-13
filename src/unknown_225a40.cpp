// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_225A40.CPP: the lifecycle callbacks of entry 47, and the two
   sound tracks it holds (each plays a sound effect and an impulse) */

#include "unknown_11c920.h"
#include "unknown_123b30.h"
#include "globals.h"
#include "unknown_222930.h"
#include <string.h>

#define PIN(n, floor, ceiling) ((n) < (floor) ? (floor) : ((n) > (ceiling) ? (ceiling) : (n)))

struct s_unknown_225a40
{
	long values[10];
};

s_unknown_225a40 *g_51ebf4;

// @retail 0x225a40
void function_225a40(void)
{
	g_51ebf4 = (s_unknown_225a40 *)function_123d40("unknown", "unknown", sizeof(s_unknown_225a40));
}

// @retail 0x225a80
void function_225a80(void)
{
	memset(g_51ebf4, 0, sizeof(*g_51ebf4));
}

/* a sound track (0x14 bytes) */
struct s_sound_track
{
	bool active;
	byte unknown01[3];
	long effect_index;
	real unknown08;
	long label;	/* track 0: its definition's tag index */
	union
	{
		long entry_index;	/* track 0 */
		real unknown10;	/* track 1 */
	};
};

#define SOUND_TRACKS ((s_sound_track *)g_51ebf4)

/* a tag function's data, as the evaluator reads it */
struct s_sound_track_function
{
	long size;
	byte *address;
};

struct s_sound_track_definition_entry
{
	byte unknown00[0x3c];
	long label;
	real duration;
	s_sound_track_function scale;
};

struct s_tag_block_view
{
	long count;
	void *address;
	long unused;
};

struct s_sound_track_definition
{
	byte unknown00[0x6c];
	s_tag_block_view entries;
};

static inline void *function_x332d0d(s_tag_block_view const *block, long index, long element_size)
{
	return (byte *)block->address + index * element_size;
}

/* the definition entry track 0 plays, NULL while the track is not active */
static inline s_sound_track_definition_entry *sound_track_definition_entry(s_sound_track const *track)
{
	s_sound_track_definition_entry *entry = NULL;

	if (track->active)
	{
		entry = (s_sound_track_definition_entry *)function_x332d0d(
			&((s_sound_track_definition *)g_4e3b44[track->label & 0xffff].bytes)->entries,
			track->entry_index, sizeof(s_sound_track_definition_entry));
	}
	return entry;
}

long function_18d5b0(long label);
long function_21d2c0(long platform_playback, real scale, short priority);
void sound_effect_stop(long effect_index);
void __stdcall function_225b60(real elapsed);

/* plays the impulse of a sound effect in one of the driver's impulse buffers */
static inline void sound_track_start_impulse(long index, long handle)
{
	if (PIN(index, 0, 1) == index)
	{
		s_looping_impulse_parameters parameters;

		function_222930(NONE, handle, &parameters);
		parameters.index = (short)index;
		function_21f720(&parameters);
	}
}

/* starts the impulse of each active track again, then runs the update with no time elapsed.
   Track 0's label comes through sound_track_definition_entry: retail forms that entry's address in
   one register (`add eax, [edx + 0x70]`), the code of an entry pointer that is NULL on another path. */
// @retail 0x225ab0
void function_225ab0(void)
{
	for (long i = 0; i < 2; i++)
	{
		s_sound_track *track = &SOUND_TRACKS[i];

		if (track->active)
		{
			long label;

			switch (i)
			{
			case 0:
				label = sound_track_definition_entry(track)->label;
				break;
			case 1:
				label = track->label;
				break;
			default:
				continue;
			}

			long handle = function_18d5b0(label);
			if (handle != NONE)
			{
				sound_track_start_impulse(i, handle);
			}
		}
	}
	function_225b60(0);
}

real function_13b390(void const *function, real input, real range);

/* a tag function's value at 0, mapped to its range */
static inline real sound_track_function_evaluate(s_sound_track_function const *function)
{
	real value;

	if (function->address && function->size > 0)
	{
		value = function_13b390(function, 0.0f, 0.0f);
		byte const *data = function->address;

		if (!(data[1] & 0xf0))
		{
			real lower = *(real const *)(data + 4);
			real upper = *(real const *)(data + 8);

			value = lower + (upper - lower) * PIN(value, 0.0f, 1.0f);
		}
	}
	else
	{
		value = 0.0f;
	}
	return value;
}

/* starts track 0 with an entry of a definition */
// @retail 0x225df0
void function_225df0(long tag_index, long entry_index)
{
	s_sound_track_definition_entry *entry = (s_sound_track_definition_entry *)function_x332d0d(
		&((s_sound_track_definition *)g_4e3b44[tag_index & 0xffff].bytes)->entries, entry_index, sizeof(s_sound_track_definition_entry));
	long handle = function_18d5b0(entry->label);

	if (handle != NONE)
	{
		s_sound_track *track = &SOUND_TRACKS[0];

		if (track->active && track->effect_index != NONE)
		{
			sound_effect_stop(track->effect_index);
		}
		real scale = sound_track_function_evaluate(&entry->scale);

		track->active = true;
		track->effect_index = function_21d2c0(handle, scale, 8);
		track->unknown08 = 0.0f;
		track->label = tag_index;
		track->entry_index = entry_index;
		sound_track_start_impulse(0, handle);
	}
}

// @retail 0x225ef0
void function_225ef0(long label)
{
	long handle = function_18d5b0(label);

	if (handle != NONE)
	{
		s_sound_track *track = &SOUND_TRACKS[1];

		if (track->active && track->effect_index != NONE)
		{
			sound_effect_stop(track->effect_index);
		}
		track->active = true;
		track->effect_index = function_21d2c0(handle, 1.0f, 8);
		track->unknown08 = 0.0f;
		track->label = label;
		track->unknown10 = 1.0f;
		sound_track_start_impulse(1, handle);
	}
}

struct s_sound_effect_buffer
{
	long data[0x100];
	long size;
};

struct s_effect_inputs
{
	real values[4];
	vector3f direction;
};

struct s_track_effect_view
{
	byte unknown00[4];
	byte flags;
	byte unknown05[7];
	real scale;
	byte unknown10[4];
};

typedef char check_track_effect_view_size[sizeof(s_track_effect_view) == 0x14 ? 1 : -1];

extern void *g_51ebe0;
void *function_18d090(long tag_index, long handle);
void function_2228d0(long handle, s_sound_effect_buffer *buffer, s_effect_inputs const *inputs);
void function_21f960(dword size, dword const *buffer);

static inline real sound_track_function_at_time(s_sound_track_function const *function, real input)
{
	real value;
	if (function->address && function->size > 0)
	{
		value = function_13b390(function, input, 0.0f);
		byte const *data = function->address;
		if (!(data[1] & 0xf0))
		{
			real lower = *(real const *)(data + 4);
			real upper = *(real const *)(data + 8);
			value = lower + (upper - lower) * PIN(value, 0.0f, 1.0f);
		}
	}
	else
		value = 0.0f;
	return value;
}

void __stdcall function_21d630(long effect_index, long mode);

struct s_225b60
{
	byte field_0[4];
	word field_4 : 3;
	word field_5 : 2;
	word field_6 : 11;
};

// @retail 0x225b60
void __stdcall function_225b60(real elapsed)
{
	s_sound_effect_buffer output = {0};
	struct { real scale; long handle; long index; } update;
	for (update.index = 0; update.index < 2; update.index++)
	{
		s_sound_track *track = &SOUND_TRACKS[update.index];
		if (track->active)
		{
			bool finished = false;
			update.handle = NONE;
			switch (update.index)
			{
			case 0:
				{
					s_sound_track_definition_entry *entry = sound_track_definition_entry(track);
					real duration = entry->duration > 0.001f ? entry->duration : 0.001f;
					update.scale = sound_track_function_at_time(&entry->scale, track->unknown08 / duration);
					update.handle = function_18d5b0(entry->label);
					finished = track->unknown08 > entry->duration;
				}
				break;
			case 1:
				update.scale = track->unknown10;
				update.handle = function_18d5b0(track->label);
				break;
			}
			if (track->effect_index != NONE)
				((s_track_effect_view *)((s_record_pool *)g_51ebe0)->data)[track->effect_index & 0xffff].scale = update.scale;
			if (function_18d090(NONE, update.handle))
			{
				s_sound_effect_buffer buffer = {0};
				s_effect_inputs inputs;
				inputs.values[0] = 0.0f;
				inputs.values[1] = track->unknown08;
				inputs.values[2] = update.scale;
				inputs.values[3] = 1.0f;
				inputs.direction = *(vector3f *)g_468788;
				function_2228d0(update.handle, &buffer, &inputs);
				if (buffer.size > 0)
					output = buffer;
			}
			track->unknown08 += elapsed;
			if (finished)
			{
				long local_0 = track->effect_index;
				if (local_0 != NONE)
				{
					s_track_effect_view *effect = &((s_track_effect_view *)((s_record_pool *)g_51ebe0)->data)[local_0 & 0xffff];
					((s_225b60 *)effect)->field_5 = 3;
					function_21d630(local_0, 2);
					effect->flags |= 1;
				}
				track->active = false;
			}
		}
	}
	function_21f960(output.size, (dword const *)output.data);
}
