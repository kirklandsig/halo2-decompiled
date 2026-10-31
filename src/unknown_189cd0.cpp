// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_189CD0.CPP: plays a looping sound slot's sound, positioned on an
   object or not. Retail keeps the standard convention with all seven
   arguments (callers push seven words, ret 0x1c), though the fourth and
   fifth are unused. */

#include "unknown_11c920.h"
#include "globals.h"
#include "sound_sources.h"
#include "sound_records.h"
#include "object_markers.h"
#include "local_cameras.h"

struct s_sound_source_description
{
	char value0;
	byte flags;
	byte unknown02[2];
	point3f position;
	vector3f direction;
	byte unknown1c[8];
	long value24;
};

long function_189ee0(long tag_index);
real sound_permutation_reference_duration(long definition_index, s_sound_permutation_reference const *reference);
void object_dispatch_sound_started(long object_index, long tag_index, long sound_index);
extern s_sound_source_callbacks const g_444b3c;
void function_1892a0(short value, point3f const *position, vector3f const *direction, long tag_index, long object_index, s_sound_source_description *description);
long function_18d5b0(long label);
long function_189fe0(s_sound_request const *request, long tag_index);
long function_189340(long object_index, long tag_index, long audible, s_sound_marker const *marker, s_sound_source_callbacks const *source, real scale, long platform_playback, char const *variant);

// score 21 (was 21): retail reads the object index once, after the playback call, into
// ebx and copies it to esi for the call; ours reads it into esi at the call and again
// into ebx for the started test (526 bytes). With plain reads at the call, test and
// dispatch (round 45, 90) the late read is one load as in retail, but into esi (the
// call's register) with the sound index in edi, and the NONE branch stores esi for the
// object index. Round 45: the early object local for every later use 116; the started
// test nested 21, with plain late reads 90; plain call read with the test and dispatch
// through the reference 28. Earlier: late object carrier/reload still adds five bytes;
// qualified object refresh 100f/85p;
// playback then refresh 101f/35p; chained request zeros 121f; initial result 118f; refresh with zeros
// 103f/85p; object-first admission 118f; byte positioned state 118f; refreshed nested admission 100f/85p.
// Argument-list assignment 85; plain reads
// throughout 116; second half plain 90; all three through the reference 28; first half
// plain 41; the first test alone through the reference 169; non-volatile reference 116.
// frame 0xa0 agrees; +0x115 consumes the slot pointer
// for the variant, then reloads it; object playback setup and source-result
// allocation also differ. Earlier pointer/result scopes tied at 38; standalone
// argument playback gave 83/523. Ordinary trials: indexed final store 156/537;
// descriptive homes 35; shared source mode 97/521; slot reference 35; argument
// playback with early request variant 81/523; argument playback with indexed
// final store 148/539; failed-duration tail 35; early variant and argument
// playback with descriptive homes 81/523; object retained during argument
// evaluation 85/523. Opening executable body restored.
// slot base/result registers at +0x115 and object playback setup remain;
// direct duration 35; duration with argument playback 85; shared permutation 123;
// permutation with argument playback 123; completion result 55; request variant first 41;
// callback fields first 60; separate standard noinline declaration 35 (ordinary scores).
// With plain reads throughout the object index takes ebp and the class spatialization is
// spilled (frame 0xa4); retail keeps spatialization in edi and the object index in memory.
// Call plain with test and started call through the reference 28; the existing class
// spatialization helper 154 (plain reads) and 126 (as here), its ternary losing the NULL
// store; spatialization declared first with plain reads 130; test through the reference 28.
// Shared spatial query 134f; native positioned value 114f/109p; both 143f;
// outer description/priority scale 118f each; spatial/playback/request call summaries 118f each.
// Native description bit 141f; native positioned bit 118f; both bits 141f; request workspace 118f;
// request and native bit 141f; native bit and result scope 141f; native bit and request admission
// 141f; native bit with both workspaces 141f (fast scores; unchanged fast 118).
// Post-playback notification object 118f; two-phase object 118f; full source-result join 118f;
// result with notification object 118f; positioned-source-first admission 118f; admission with
// notification 118f; admission with result 118f; all source/object lifetimes 118f (control 118f).
// Round 55 plain: const spatialization data 21; const slot 21; const class 21; const tag data 21;
// const class/spatialization 21; const slot/spatialization 21; const slot/class 21; all four 21.
// @retail 0x189cd0 standard
void __stdcall function_189cd0(long tag_index, long object_index, real scale, long a, long b, long label, long duration_address)
{
	long const volatile *object_reference = &object_index;
	real *duration = (real *)duration_address;

	if (tag_index != NONE)
	{
		long slot_index = function_189ee0(tag_index);
		if (slot_index != NONE)
		{
			long sound_index;
			s_looping_sound_slot *slot = &g_4ed288->slots[slot_index];
			if (duration)
			{
				*duration = sound_permutation_reference_duration(tag_index, &slot->permutation);
			}
			s_sound_tag *tag = (s_sound_tag *)g_4e3b44[tag_index & 0xffff].bytes;
			s_sound_class_view *sound_class = &((s_sound_globals_view *)g_51ebd4)->classes[tag->class_index];
			s_sound_class_spatialization *spatialization = NULL;
			if (sound_class->spatialization.flags & 7)
				spatialization = &sound_class->spatialization;
			bool positioned = spatialization && (spatialization->flags & 1);

			if (scale < 0.0f)
				scale = 0.0f;
			else if (scale > 1.0f)
				scale = 1.0f;

			long object = *object_reference;
			if (object == NONE && !positioned)
			{
				s_sound_request request;
				request.location.unknown02 = 0;
				request.location.flags = 0x400;
				request.location.audible = 0;
				request.location.requested_audible = 0;
				request.location.scale = scale;
				request.location.unknown08 = 0;
				request.object_index = NONE;
				request.platform_playback = function_18d5b0(label);
				request.variant = (char const *)&slot->permutation;
				request.marker = NULL;
				request.source = &g_444b3c;
				sound_index = function_189fe0(&request, tag_index);
			}
			else
			{
				s_sound_source_description description;
				description.flags = 0;
				if (object != NONE)
				{
					s_object_marker local_4f80fe;
					function_b8d30(object, 0x4000095, &local_4f80fe, 1, false);
					function_1892a0(local_4f80fe.node_index, &local_4f80fe.node_matrix.position,
						&local_4f80fe.node_matrix.forward, tag_index, object, &description);
				}
				if (spatialization)
					description.flags |= 1;
				else
					description.flags &= ~1;
				sound_index = function_189340(*object_reference, tag_index, positioned ? 2 : 1, (s_sound_marker *)&description,
					&g_444b3c, scale, function_18d5b0(label), (char const *)&slot->permutation);
				if (sound_index != NONE && object_index != NONE)
				{
					object_dispatch_sound_started(object_index, tag_index, sound_index);
				}
			}
			slot->source_index = sound_index;
			if (sound_index != NONE)
				return;
		}
	}
	if (duration)
	{
		*duration = 0.0f;
	}
}
