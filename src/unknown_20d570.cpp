// @flags /O2 /Gr /arch:SSE
/* UNKNOWN_20D570.CPP: an object's audio cue reaching the ai: the cue's
   event goes out to the actors that hear it, and the actor of the unit it
   names pauses to react */
#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "slot_handler.h"
#include "unknown_1fb7e0.h"

/* the tag header globals (g_4e034c) as the cue reads them */
struct s_cue_globals_view
{
	byte unknown00[0xc8];
	long count;
	struct s_cue_globals_block
	{
		byte unknown00[0x64];
		long tag_index;
	} *block;
};

/* an entry (0x60 bytes) of the tag's cue block */
struct s_cue_entry
{
	byte unknown00[0x14];
	short type;
	byte unknown16[0x42 - 0x16];
	short channel;
	byte unknown44[0x60 - 0x44];
};

struct s_cue_tag
{
	long count;
	s_cue_entry *entries;
};

/* a playing cue (g_4f9398, 0x54 bytes) */
struct s_cue_record
{
	byte unknown00[0x1c];
	short ticks;
	byte unknown1e[0x28 - 0x1e];
	long unknown28;
	byte unknown2c[0x41 - 0x2c];
	bool unknown41;
	byte unknown42[0x54 - 0x42];
};

/* what the caller hands over: the cue's target and its event */
struct s_cue_source
{
	byte unknown00[4];
	long target_index;
	s_1fb7e0_data data;
};

/* the actor's reaction timer */
struct s_cue_actor_view
{
	byte unknown000[0x684];
	short unknown684;
	short unknown686;
	short unknown688;
	byte unknown68a[2];
	long unknown68c;
};

struct s_object;
struct s_audio_selection;
s_object *function_badc0(long object_index, dword type_mask);
long function_1469f0(real seconds);
bool function_1df560(short team_a, short team_b);
bool __stdcall function_20cdc0(long arg_0, long arg_1, long arg_2, s_audio_selection *arg_3, long *arg_4);
void function_20d870(long actor_index, short channel, short value);
void __stdcall function_1fbac0(long unknown, long unit_index, bool unknown2, long unknown3, s_1fbac0_event *event);

// score 21 (was 21): the scaled-index computation swaps registers: ours uses EAX/EDX,
// retail uses EDX/EAX. Tried: a forceinline record getter for both lookups 21,
// for the first only 21; the masked index in a local first 21; typed record
// pointer arithmetic for both lookups 21; a record reference 21; the field read into a
// local before the flag store 21; a block-scoped record for each lookup 21; the pool in a
// local 21; the lookup, store and call through an inline helper taking both indices 21 (all
// identical code); record initialised to NULL at its declaration 222 (callers change).
// Only this lookup differs: retail colours pool/record eax, index/field edx (lea), ours
// index/record/field eax (add).
// Round 11 (lane 18), fast scores (the fast build gives 23 where the plain check gives 21): the tag
// header global read at each use instead of the globals local 23; the first lookup as a byte pointer
// with offset fields (the sibling files' form for this pool) 23; the tag pointer set in both arms
// instead of initialised 195; the record declared first 23; the object through the existing
// object_get 23 (identical code; kept: it replaces that helper's body written out). All identical
// code except the tag arms: the lookup's colouring does not follow any local form tried.
// @retail 0x20d570
void __stdcall function_20d570(short arg_1, long arg_0, long arg_2, void *arg_3)
{
	if (g_4f55d0->active)
	{
		s_slot_object_view *object = object_get(arg_2);
		s_cue_source *source = (s_cue_source *)arg_3;
		s_cue_globals_view *globals = (s_cue_globals_view *)g_4e034c;
		s_cue_tag *tag = NULL;
		long mode = 2;
		s_cue_entry *entry;
		s_cue_record *record;
		s_1fb7e0_data data;

		if (globals && globals->count > 0 && globals->block->tag_index != NONE)
			tag = (s_cue_tag *)g_4e3b44[globals->block->tag_index & 0xffff].bytes;
		entry = NULL;
		if (arg_1 != NONE && tag)
			entry = &tag->entries[arg_1];
		if (arg_0 != NONE)
		{
			record = (s_cue_record *)(g_4f9398->data + (arg_0 & 0xffff) * sizeof(s_cue_record));
			record->unknown41 = true;
			function_20cdc0(arg_2, record->unknown28, arg_0, NULL, NULL);
		}
		data.unknown00 = 0;
		if (source->data.unknown00 == 0)
		{
			if (entry)
			{
				switch (entry->type)
				{
				case 1:
					data.unknown00 = 3;
					data.unknown08 = arg_2;
					mode = 3;
					break;
				case 2:
					if (source->target_index != NONE)
					{
						s_slot_object_view *target = (s_slot_object_view *)function_badc0(source->target_index, NONE);

						if (target)
						{
							if (!((1 << target->type) & 3) || target->actor_index == NONE ||
								function_1df560(object->team, target->team))
							{
								data.unknown00 = 3;
								data.unknown08 = source->target_index;
							}
						}
					}
					break;
				}
			}
		}
		else
		{
			data = source->data;
		}
		if (data.unknown00 != 0)
		{
			s_1fbac0_event event;

			event.unknown00 = 0;
			event.unknown02 = 1;
			event.data = data;
			/* the callers pass the mode and the filter as longs */
			((void (__stdcall *)(long, long, long, long, s_1fbac0_event *))function_1fbac0)(object->actor_index, arg_2,
				mode, 1, &event);
		}
		if (source->target_index != NONE && source->target_index != arg_2)
		{
			s_slot_object_view *target = (s_slot_object_view *)function_badc0(source->target_index, NONE);

			if (target && ((1 << target->type) & 3) && target->actor_index != NONE)
			{
				s_cue_actor_view *actor = (s_cue_actor_view *)actor_get(target->actor_index);
				real seconds = 1.5f;

				record = NULL;
				if (arg_0 != NONE)
					record = (s_cue_record *)(g_4f9398->data + (arg_0 & 0xffff) * sizeof(s_cue_record));
				if (record)
					seconds = (real)record->ticks * g_510c54->rate + 1.0f;
				if (actor->unknown684 <= 0)
				{
					actor->unknown686 = (short)function_1469f0(seconds);
					actor->unknown688 = 6;
					actor->unknown68c = arg_2;
					actor->unknown684 = 0;
				}
				if (entry && record && entry->channel > 0)
					function_20d870(target->actor_index, entry->channel - 1, g_510c54->field_2_3 + record->ticks);
			}
		}
	}
}
