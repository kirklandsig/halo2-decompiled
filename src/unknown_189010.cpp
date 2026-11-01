// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_189010.CPP: sound sources: whether a sound plays from an object
   of the vehicle type, and the source description a sound starts from */

#include "unknown_11c920.h"
#include "globals.h"
#include "sound_sources.h"
#include "sound_records.h"
#include "object_markers.h"
#include "local_cameras.h"
#include <math.h>

long function_189ee0(long tag_index);
real sound_permutation_reference_duration(long definition_index, s_sound_permutation_reference const *reference);
void object_dispatch_sound_started(long object_index, long tag_index, long sound_index);
extern s_sound_source_callbacks const g_444b3c;

struct s_sound_tag_flags
{
	byte flags;
};

struct s_source_object
{
	byte unknown00[0x12c];
	byte flags12c;
	byte unknown12d[0x154 - 0x12d];
	long value154;
};

struct s_source_object_header
{
	byte unknown00[3];
	byte type;
	byte unknown04[4];
	s_source_object *object;
};

struct s_sound_source_description
{
	char value0;
	byte flags;
	byte unknown02[2];
	point3f position;
	vector3f direction;
	long value1c;
	long value20;
	long value24;
};

static inline s_source_object_header *source_object_header(long object_index)
{
	return (s_source_object_header *)g_4e0300->data + (object_index & 0xffff);
}

// @retail 0x189010
bool function_189010(long tag_index, long object_index)
{
	bool result = false;

	if (tag_index != NONE && (((s_sound_tag_flags *)g_4e3b44[tag_index & 0xffff].bytes)->flags & 0x40) && object_index != NONE)
	{
		if ((1 << source_object_header(object_index)->type) & 4)
		{
			result = true;
		}
	}
	return result;
}

dword vector3d_compress(vector3f const *vector);
struct s_sound_label_play;
long function_1890c0(s_sound_label_play const *play, long object_index, short value, point3f const *position, vector3f const *direction);
long function_189400(s_sound_position const *position, long object_index, long tag_index, real scale);

// @retail 0x1892a0
void function_1892a0(short value, point3f const *position, vector3f const *direction, long tag_index, long object_index, s_sound_source_description *description)
{
	description->position = *position;
	description->direction = *direction;
	description->value24 = NONE;
	description->value0 = value == NONE ? (char)0 : (char)value;

	if (object_index != NONE && function_189010(tag_index, object_index))
	{
		s_source_object_header *header = source_object_header(object_index);
		if ((1 << header->type) & 4)
		{
			s_source_object *object = header->object;
			if (object->flags12c & 1)
			{
				long value154 = object->value154;
				if (value154 != NONE)
				{
					description->value24 = value154;
				}
			}
		}
	}
}

struct s_sound_class_play_flags
{
	byte unknown00[8];
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word flag3 : 1;
	word flag4 : 1;
	word flag5 : 1;
	word flag6 : 1;
	word flag7 : 1;
	word flag8 : 1;
	word flag9 : 1;
	word flag10 : 1;
	word flag11 : 1;
	word flag12 : 1;
	word flag13 : 1;
	word flag14 : 1;
	word flag15 : 1;
	byte unknown0a : 7;
	byte flag23 : 1;
};

struct s_sound_play_tag
{
	byte unknown00;
	byte flags;
	char class_index;
	byte unknown03[0x10 - 3];
	long duration;
};

struct s_unknown_5c;
s_unknown_5c *function_221810(short index);
long function_18d5b0(long label);
void function_18d4f0(long object_index, char *audible, long *local_player_index);

static __forceinline long game_seconds_to_ticks_round(real seconds)
{
	real ticks_real = (real)g_510c54->field_2_3 * seconds;
	long ticks;

	__asm
	{
		fld ticks_real
		fistp ticks
	}
	return ticks;
}

// score 110 (was 110): this round: the fast check reads 110 at 473 bytes against retail's 470 again
// and no new form was tried; the difference is still blocked by 0x221810, which retail calls twice
// and our link inlines, in src/unknown_221490.cpp.
// Earlier: this round: no new form. Still BLOCKED-BY 0x21d110/0x221810 (held files):
// retail homes the request in EBX and reloads the state flags after the first 0x221810 call,
// ours forwards them across it and builds the flag word in EDX where retail builds it in EAX.
// Earlier: Frame 0xa8 and incoming request agree; class-query expansion and state/flag/result homes remain.
// Frame 0xa8 and request EAX agree; first class-query expansion and header homes differ.
// BLOCKED-BY 0x21d110 and 0x221810 (their tag/state registers): retail homes the request in EBX and
// reloads the state flags after the first 0x221810 call, ours forwards them in a callee-saved
// register across it, and retail builds the flag word in eax where ours builds it in edx.
// frame 0xa8 and the request in EAX agree; the first class lookup expands
// where retail calls it (and 0x21d110 takes the tag in EAX where retail passes the state there), so
// the flag/priority/result homes, push placement and the end-test spill differ. Round 54: the
// seconds-to-ticks rounding helper with the looping-sound global read at each use 110 kept
// (helper alone 115); bitfield end tests 137 and on the kept form 122 (each loses eight callers);
// mask bit 8 with bitfield bit 11 132; renamed rounding temporary 110; priority from the request's
// audible field 109; that with the location copy after the flags 140. Earlier: word flags 158;
// index-order variant guards 134; flag11 field store 134; combined location mask 118 kept;
// original priority observation 133; priority arms 182; nested variant tests 182; shared exit 182;
// marker memcpy 182; base-first flags 178 (caller losses); role names 182; source snapshot 178;
// object snapshot 183; priority after flags 192; conditional flags then priority 187; deferred
// flags publication 176; location first with copied audible priority 167 kept; location memcpy
// 182; conditional end-time publications 175; outer state scope 167; early optional-size default
// 164; size before marker copy 167; nested class tests 167; platform-flags block first 167;
// platform index snapshot 167; redundant object/platform clears omitted 149 kept; scoped header on
// clean flags 149; native flag8 conversion 157 (eight caller losses); repeated class index on
// clean flags 159; location/priority phase after flags/header 188/194; first/shared class
// receiver 167/167; seconds/ticks terms 167/167; phase with seconds 188; shared receiver with
// seconds 167; late tag receiver 184; branch-assigned result 193; shared marker observation 167;
// absent-marker-first 165; native short priority and padding memset 176; whole source flags 149;
// clean state flags 169 (eight caller losses); variant fields before flag 149; audible-first
// admission 149; post-query marker default 134 kept; size conditional 147; looping global reads
// 130 fast/135 plain; time-global reads 134; size override before copy 134; whole
// request/tag/class records and combinations 181/196/228/146/197/215/247 (request/tag copies lose
// eight callers); first class record 165; consistent variant-pointer field reads 118; disjoint
// source mask OR 118; conditional optional flags 161 and full flag expression 162 (eight caller
// losses each); nested class admission 118; real class-word mask with direct query 135;
// state/result/flags/both names 118/118/118/118; early object/source/platform/all stores
// 118/135/118/133; swapped object-source/source-platform stores 118/118.
// Round 187 named duration/source expression parts and both 110/110/110; complete fresh
// tag lookup alone and with those parts 158 each; all plus signed native flags 158. Restored.
// Round 93: with 0x21d110 starting its sounds inline (state in EAX; 0x21d110 204 -> 52) the request
// moves to EDX: 135 at 470 bytes, each caller one line off. A noinline declaration of 0x221810 in
// this file leaves the first lookup expanded; one ahead of its definition calls both lookups
// (0x129aa0 101 -> 4) but this function then takes both arguments on the stack (156, 450 bytes;
// with set-or-clear object/platform flags, the priority read from the request and flag8/flag11
// end tests 132 at 470 bytes). Those body forms with the inline start alone 153, alone 163.
// Round 95 (plain checks; --fast builds this function differently): the marker size set in an
// if/else 135 (475 bytes, aligned 94 from 97); that with the priority and flags formed from the request
// before the location copy 150 (487 bytes, aligned 74); that with a noinline declaration ahead of
// 0x221810 149, stack arguments, eight callers lost; with set-or-clear object/platform flags 166 (500
// bytes, aligned 73); that with flag8/flag11 bitfield end tests 191, stack arguments, eight callers
// lost. Whenever the body homes the request in EBP, LTCG passes both arguments on the stack; homed
// in EBX it arrives in EAX (retail). Retail reloads the state flags after the first 0x221810 call (its
// compiler treats that call as opaque); ours forwards them in a callee-saved register across it,
// which pushes the request into EBP. BLOCKED-BY 0x21d110 (its tag/state registers) and 0x221810.
// Round 410: Joint source/object, source/platform, object/platform and all-three two-use header
// snapshots 110/110/110/110; each with priority from the request 109/109/109/109 (472 bytes).
// Reduced-image context differs; all one-point gains are rejected. All restored; first class-query
// expansion, header homes and playback convention remain.
// Round 419: existing rounding helper with explicit short/long/real rate inputs, each
// rate first/last, and native millisecond input in both orders 110/110/110/110/110/110/110/110.
// All tie; original helper and body restored. Query expansion, state/flags/result homes and
// clock-conversion spill remain.
// Round 194: native source-location mutable/const with source/copied priority 181/181/110/110;
// the same four with library publication 181/181/110/110. Original location-first
// body restored; input ownership regresses, and single-consumer copies only tie.
// Round 85: retail-order header with conditional object/platform clears 163 (498 bytes);
// separate location masks 110; the high-byte flag8 view 128 (486); an ordinary inline query
// declaration 110; state/tag declarations swapped 110; tag pointer renamed 110; immutable tag
// pointer 110; early uninitialized flags 110; split tag declaration/assignment 110. All restored;
// first query expansion, initial tag spill and playback convention remain.
// Earlier: this round: no new form. The first class lookup is still expanded here and
// called in retail: our link inlines 0x221810 (src/unknown_221490.cpp, matched) into it, so our
// compiler forwards the state flags across that call, while retail treats the call as opaque and
// reloads them. Suppressing the inline (inline depth zero) costs six matched callers of this
// function. Still BLOCKED-BY 0x221810 and by 0x21d110 (src/unknown_221490.cpp), whose tag and
// state registers decide where the request is homed.
// MiniMax R (10-31): no new form, no lower score.
// DL218: Complete query/playback/both inline contracts 110/110/110; query and combined contracts
// lose matched control 0x129790. Playback contract with first/shared query receivers 110/110,
// complete marker-size arms 135, request priority 109 and combined marker/priority 134. One-point
// gain rejected; all declarations, helpers and original body restored. Frame 0xa8, first query
// expansion, state/flags/result homes and playback convention remain.
// @retail 0x189fe0
long function_189fe0(s_sound_request const *request, long tag_index)
{
	long result = NONE;

	if (tag_index != NONE)
	{
		s_sound_play_tag *sound = (s_sound_play_tag *)g_4e3b44[tag_index & 0xffff].bytes;
		s_sound_play_state state;
		state.location = request->location;
		// Retail also clears the padding after the priority field.
		*(long *)&state.priority = state.location.audible ? 3 : 1;
		dword flags = (request->source ? 0x20 : 0) + 5;

		if (request->object_index != NONE)
		{
			flags |= 8;
		}
		if (request->platform_playback != NONE)
		{
			flags |= 0x100;
		}
		state.flags = flags;
		state.object_index = request->object_index;
		state.source = request->source;
		state.platform_playback = request->platform_playback;

		if (TEST_FIELD_BIT(((s_sound_class_play_flags *)function_221810(sound->class_index))->flag23) && !(sound->flags & 1))
		{
			state.location.flags |= 0x800;
		}

		state.source_data_size = 0;
		if (request->marker)
		{
			state.marker = *request->marker;
			state.source_data_size = sizeof(s_sound_marker);
		}

		char const *variant = request->variant;
		if (variant && variant[1] != NONE && variant[0] != NONE)
		{
			state.flags |= 0x400;
			state.variant0 = request->variant[0];
			state.variant1 = request->variant[1];
		}

		result = function_21d110(&state, tag_index);
		if (result != NONE)
		{
			s_sound_class_play_flags *sound_class = (s_sound_class_play_flags *)function_221810(sound->class_index);
			if (TEST_FIELD_BIT(sound_class->flag1) && TEST_FIELD_BIT(sound_class->flag2))
			{
				long end_time = g_510c54->game_time + game_seconds_to_ticks_round((real)sound->duration * 0.001f) + 10;

				g_4ed288->value20 = end_time > g_4ed288->value20 ? end_time : g_4ed288->value20;
				if ((state.location.flags & 0x900) || !state.location.audible)
				{
					g_4ed288->value24 = end_time > g_4ed288->value24 ? end_time : g_4ed288->value24;
				}
			}
		}
	}
	return result;
}

// @retail 0x1895f0
long function_1895f0(s_sound_position const *position, real scale, long tag_index)
{
	s_sound_request request;

	request.location.unknown02 = 0;
	request.location.flags = 0;
	request.location.spatial = *position;
	request.location.unknown08 = 0;
	request.marker = NULL;
	request.source = NULL;
	request.variant = NULL;
	long local_0 = *(long const volatile *)&tag_index;
	request.location.scale = scale;
	request.location.audible = 1;
	request.location.requested_audible = 1;
	request.object_index = NONE;
	request.platform_playback = NONE;
	return function_189fe0(&request, local_0);
}

struct s_sound_label_play
{
	long label;
	long tag_index;
	real scale;
	char const *variant;
};

// @retail 0x189650
long function_189650(s_sound_position const *position, s_sound_label_play const *play)
{
	s_sound_request request;

	request.location.unknown02 = 0;
	request.location.flags = 0;
	request.location.spatial = *position;
	request.location.scale = play->scale;
	request.location.audible = 1;
	request.location.requested_audible = 1;
	request.location.unknown08 = 0;
	request.object_index = NONE;
	request.platform_playback = function_18d5b0(play->label);
	request.marker = NULL;
	request.source = NULL;
	request.variant = NULL;
	return function_189fe0(&request, play->tag_index);
}

// @retail 0x1896c0
long function_1896c0(real scale, long tag_index)
{
	s_sound_request request;

	request.location.unknown02 = 0;
	request.location.flags = 0;
	request.location.audible = 0;
	request.location.requested_audible = 0;
	request.location.unknown08 = 0;
	request.marker = NULL;
	request.source = NULL;
	request.variant = NULL;
	long local_0 = *(long const volatile *)&tag_index;
	request.location.scale = scale;
	request.object_index = NONE;
	request.platform_playback = NONE;
	return function_189fe0(&request, local_0);
}

extern s_sound_source_callbacks const g_444b7c;

// @retail 0x189710
long function_189710(real scale, long tag_index)
{
	s_sound_request request;

	request.location.unknown02 = 0;
	request.location.flags = 0;
	request.location.audible = 0;
	request.location.requested_audible = 0;
	request.location.unknown08 = 0;
	request.marker = NULL;
	request.variant = NULL;
	long local_0 = *(long const volatile *)&tag_index;
	request.location.scale = scale;
	request.object_index = NONE;
	request.platform_playback = NONE;
	request.source = &g_444b7c;
	return function_189fe0(&request, local_0);
}

// @retail 0x189760
long function_189760(s_sound_label_play const *play)
{
	s_sound_request request;

	request.location.unknown02 = 0;
	request.location.flags = 0;
	request.location.audible = 0;
	request.location.requested_audible = 0;
	request.location.scale = play->scale;
	request.location.unknown08 = 0;
	request.object_index = NONE;
	request.platform_playback = function_18d5b0(play->label);
	request.marker = NULL;
	request.source = NULL;
	request.variant = NULL;
	return function_189fe0(&request, play->tag_index);
}

// @retail 0x189340
long function_189340(long object_index, long tag_index, long audible, s_sound_marker const *marker, s_sound_source_callbacks const *source, real scale, long platform_playback, char const *variant)
{
	s_sound_request request;

	request.location.flags = 0;
	char local_audible = *(char const volatile *)&audible;
	function_18d4f0(object_index, &local_audible, NULL);
	request.location.audible = local_audible;
	request.location.requested_audible = local_audible;
	request.location.unknown02 = 0;
	request.location.flags = 0;
	request.location.unknown08 = 0;
	request.location.spatial.location.leaf_index = NONE;
	request.location.spatial.location.cluster_index = NONE;
	request.location.spatial.location.bsp_index = g_4686c4;
	request.location.scale = scale;
	request.variant = variant;

	if (source->update(object_index, tag_index, marker, &request.location))
	{
		request.platform_playback = platform_playback;
		request.object_index = object_index;
		request.marker = marker;
		request.source = source;
		return function_189fe0(&request, tag_index);
	}
	return NONE;
}

void function_1892a0(short value, point3f const *position, vector3f const *direction, long tag_index, long object_index, s_sound_source_description *description);
extern point3f *g_468788;
extern s_sound_source_callbacks const g_444b5c;

struct s_sound_globals_side_view
{
	long front[2];
	long side[2];
	long back[2];
};

struct s_tag_header_alt_sound_view
{
	byte unknown00[0x14];
	long sound_globals_tag_index;
};

struct s_tag_header_globals_sound_view
{
	byte unknown00[0xc0];
	void *header;
	s_tag_header_alt_sound_view *header_alt;
};

struct s_local_camera_view
{
	byte unknown00[0x18];
	vector3f forward;
	byte unknown24[0x30 - 0x24];
	point3f position;
};

// @retail 0x189400
long function_189400(s_sound_position const *position, long object_index, long tag_index, real scale)
{
	long local_player_index = NONE;
	bool play_at_position = true;

	if (object_index != NONE)
	{
		char audible = 1;
		if (!(((s_sound_play_tag *)g_4e3b44[tag_index & 0xffff].bytes)->unknown00 & 4))
		{
			function_18d4f0(object_index, &audible, &local_player_index);
		}

		play_at_position = audible == 1;
		if (!play_at_position && local_player_index != NONE)
			play_at_position = !local_camera_get(local_player_index)->active;
	}

	if (play_at_position)
		return function_1895f0(position, scale, tag_index);

	s_sound_source_description description;

	description.flags = 0;
	function_1892a0(NONE, g_468788, (vector3f *)g_4687a8, tag_index, object_index, &description);

	s_tag_header_globals_sound_view *globals = (s_tag_header_globals_sound_view *)g_4e034c;
	s_tag_header_alt_sound_view *header = globals->header ? globals->header_alt : NULL;
	long sound_globals_tag_index = header->sound_globals_tag_index;
	if (sound_globals_tag_index != NONE)
	{
		s_sound_globals_side_view *sides = (s_sound_globals_side_view *)g_4e3b44[sound_globals_tag_index & 0xffff].bytes;
		s_local_camera_view *camera = (s_local_camera_view *)local_camera_get(local_player_index);
		plane3f plane;

		plane.n = camera->forward;
		plane.d = dot3f((vector3f const *)&camera->position, &plane.n);
		real distance = plane_distance_to_point(&plane, &position->position);

		if ((real)fabs(distance) <= 0.01f)
		{
			description.value1c = sides->side[0];
			description.value20 = sides->side[1];
		}
		else if (distance > 0.0f)
		{
			description.value1c = sides->front[0];
			description.value20 = sides->front[1];
		}
		else
		{
			description.value1c = sides->back[0];
			description.value20 = sides->back[1];
		}
		description.flags |= 2;
	}
	return function_189340(object_index, tag_index, 0, (s_sound_marker *)&description, &g_444b5c, scale, NONE, NULL);
}

extern s_sound_source_callbacks const g_444afc;
extern s_sound_source_callbacks const g_444b1c;

// @retail 0x189060
long function_189060(long object_index, short value, real scale, point3f const *position, vector3f const *direction, long tag_index)
{
	short const *local_0 = &value;
	s_sound_source_description description;

	description.flags = 0;
	function_1892a0(*local_0, position, direction, tag_index, object_index, &description);
	return function_189340(object_index, tag_index, 1, (s_sound_marker *)&description, function_189010(tag_index, object_index) ? &g_444b1c : &g_444afc, scale, NONE, NULL);
}

// @retail 0x1890c0
long function_1890c0(s_sound_label_play const *play, long object_index, short value, point3f const *position, vector3f const *direction)
{
	long result = NONE;
	s_sound_source_description description;
	s_sound_request request;
	char audible;

	description.flags = 0;
	audible = 1;
	function_18d4f0(object_index, &audible, NULL);
	function_1892a0(value, position, direction, play->tag_index, object_index, &description);
	request.location.audible = audible;
	request.location.requested_audible = audible;
	request.location.spatial.location.leaf_index = NONE;
	request.location.spatial.location.cluster_index = NONE;
	request.location.spatial.location.bsp_index = g_4686c4;
	request.variant = play->variant;
	request.location.unknown02 = 0;
	request.location.flags = 0;
	request.location.unknown08 = 0;
	request.location.scale = play->scale;

	if (function_18c3b0(object_index, play->tag_index, (s_sound_marker *)&description, &request.location))
	{
		request.object_index = object_index;
		request.marker = (s_sound_marker *)&description;
		request.platform_playback = function_18d5b0(play->label);
		request.source = function_189010(play->tag_index, object_index) ? &g_444b1c : &g_444afc;
		result = function_189fe0(&request, play->tag_index);
	}
	return result;
}

// @retail 0x1891d0
long function_1891d0(long object_index, long marker_name, s_sound_label_play const *play)
{
	s_object_marker marker;

	function_b8d30(object_index, marker_name, &marker, 1, false);
	return function_1890c0(play, object_index, marker.node_index, &marker.node_matrix.position, &marker.node_matrix.forward);
}

// score 51 (was 51): Root-location registers, saves and stacked play remain; matrix
// pointer/reference/mutable bindings 51 each; complete submission copy 87; matrix with submission
// copy 87; library submission copy 87; matrix with velocity copy 51; reference with submission copy
// 87.
// BLOCKED-BY 0xba300 (src/unknown_0b8bd0.cpp): retail passes the object in ECX and the location
// in EAX to the root-location helper, ours passes the object in EAX and the location in ESI;
// with the helper's own body matching, this function loses only the 0x189400 call.
// frame agrees; saved registers, root-location contract and stacked play differ.
// Plain workspace order 51; workspace names 51; zero initialization 99; aggregate construction 64;
// native point copy 51; compression first 52; velocity first 49; separate location output 73.
// Earlier: root-location convention still adds saves and stacks the play pointer.
// Shared point setter 63; native velocity copy 51; both 63; marker scope 51;
// late playback record 69; pre-query playback record 64; root before velocity/compression 52/52.
// Earlier: root-location registers and stacked playback input remain.
// root-location setup, saved registers and stacked play remain.
// Marker value/clear 99/89; position clear 101; transform value/library copy 78/78;
// forward value 86; marker array 51; initialized marker array 99.
// root-location convention still adds saves and stacks the play pointer.
// root-location registers and stacked playback input remain.
// Entry/post-marker playback records 82/63; entry tag/scale/both 64/64/54;
// root before marker/point 60/52; entry record with root first 82.
// Post-marker tag/scale/both 52/63/53; post-compression 52/63/52;
// post-velocity tag/scale 52/63; all one-use snapshots restored.
// Play first/middle parameter orders 51/51 (same code); play fields read at entry 54 (play
// becomes a register argument, in eax). Retail keeps the object in esi and play in edi; ours
// needs esi for 0xba300's location argument (BLOCKED-BY 0xba300's convention).
// Round 6: unchanged; the first difference (+0x7, play stacked and EBP saved) still comes from
// 0xba300's object EAX / location ESI arguments (retail: object ECX, location EAX).
// Round 282: shared velocity setter with global/pointer/value inputs 63/63/95;
// point plus velocity setters 75; partial point/point+direction construction with
// velocity setter 96/90; velocity before compression 63, with both setters 75.
// All restored; root-location registers, saves and stacked playback input remain.
// Round 7 (lane 14): velocity first 52; named result 51; play fields through `(*play).` 51.
// Round 30: named result with one return 51; velocity/root/point/compression 52;
// point/velocity/root/compression 49; compression/velocity/point/root 52;
// point/root/velocity/compression 52. Restored; still BLOCKED-BY 0xba300's convention.
// The play record comes before the marker name: callers set up register arguments last to first, and retail's
// 0xc5890 loads the marker (eax) before the play record (edi).
// @retail 0x189210
long function_189210(long object_index, s_sound_label_play const *play, long marker_name)
{
	s_object_marker marker;
	s_sound_position position;

	function_b8d30(object_index, marker_name, &marker, 1, false);
	position.position = marker.matrix.position;
	position.compressed_forward = vector3d_compress(&marker.matrix.forward);
	position.velocity = *g_4687a4;
	object_get_root_location(object_index, &position.location);
	return function_189400(&position, object_index, play->tag_index, play->scale);
}

long function_155760(long local_player_index);
real function_218d30(long definition_index);
real function_30bf0(vector3f *vector);
long function_1895f0(s_sound_position const *position, real scale, long tag_index);

#pragma inline_depth(0)
// score 2 (was 2): success result/pop schedule (retail pops ESI before `mov al, 1`). It follows
// the link-time context, not this body or its caller's call: a 67-source reduced image matches,
// any image of 541+ sources (0xf8eb0's, or any of 0xfc330/0xfcdd0/0x10c880/0x17a8a0/0x184440)
// gives the swap, and in 0xf8eb0 the result ignored or the call removed still gives it; the
// 0x189fe0 tick conversion simplified, 0x189210's result used, `inline` on 0x1895f0 2 each.
// Round 25: early false returns 175; one nested eligibility condition, no gotos 157; the
// distance test written `distance_squared <` 2; 0x1895f0 reading tag_index plainly 2 (0x1895f0 8).
// Earlier, BLOCKED-BY 0xf8eb0 (a reduced build without that caller matched). Round 52 (fast, caller included):
// noinline declaration 2; plain returns 175; no inner block 2; success falling through to the
// label 2. Earlier: result before submit/request 181/181;
// forward/velocity/location library copies 2/2/2; location before velocity 23;
// read-only camera 2; forward assignment 2.
// success result/pop order remains; positive eligibility 157; nested eligibility 157; shared point difference 170; vector names 2; request lifetime 2; distance declaration 2; shared point setter 73; shared length 162; all scores plain.
// success result/pop order remains; camera helper 34; result/scalar names 2/2;
// length squared first 2; entry maximum/outer owner index 2/2; distance before position 2;
// submission name 2; delayed false initialization 5; inverted success 2; all scores plain.
// Success result/pop order remains. Request/forward/distance/combined scope endings 2 each plain;
// projection component loop 2 plain; difference/paired component loops fast 4/4;
// projection library clear fast 161. Reduced exact results need plain confirmation.
// Round 11 (lane 17, plain): without the inline_depth pragmas 269 (callees inlined, 733 bytes); the
// success result set before the final call 181; length_squared reused for the second length 160;
// dot3f for the projection length 166, for the perpendicular distance 158. The sources that bring the
// swap (0xfc330, 0x10c880, 0x17a8a0, 0x184440) are all callers of 0x1895f0 that ignore its result.
// @retail 0x1897c0
bool function_1897c0(long local_player_index, long unit_index, long tag_index, s_location const *location, point3f const *origin, vector3f const *direction)
{
	bool local_1 = false;
	{
		s_local_camera *camera = &((s_local_cameras *)g_4e6380)->cameras[local_player_index];

		if (!camera->active)
		{
			goto local_2;
		}

		if ((1 << function_155760(local_player_index)) & 3)
		{
			long player_unit_index = NONE;
			if (local_player_index != NONE)
				player_unit_index = g_4e8c20->entries[local_player_index];
			if (unit_index == player_unit_index)
			{
				goto local_2;
			}
		}

		real maximum_distance = function_218d30(tag_index);
		vector3f to_camera;
		vector3f perpendicular;
		vector3f projection;
		real length_squared;

		to_camera.i = camera->position.x - origin->x;
		to_camera.j = camera->position.y - origin->y;
		to_camera.k = camera->position.z - origin->z;
		length_squared = (direction->j * direction->j + direction->k * direction->k + direction->i * direction->i);
		if (length_squared != 0.0f)
		{
			real local_0 = direction->k * to_camera.k;
			local_0 += direction->i * to_camera.i;
			local_0 += direction->j * to_camera.j;
			real t = local_0 / length_squared;

			projection.i = direction->i * t;
			projection.j = direction->j * t;
			projection.k = direction->k * t;
			perpendicular.i = to_camera.i - projection.i;
			perpendicular.j = to_camera.j - projection.j;
			perpendicular.k = to_camera.k - projection.k;
		}
		else
		{
			perpendicular = to_camera;
			projection.i = 0.0f;
			projection.j = 0.0f;
			projection.k = 0.0f;
		}

		real along = (direction->k * projection.k + direction->j * projection.j + direction->i * projection.i);
		if (along >= 0.0f && (direction->j * direction->j + direction->k * direction->k + direction->i * direction->i) > along)
		{
			real distance_squared = (perpendicular.k * perpendicular.k + perpendicular.j * perpendicular.j + perpendicular.i * perpendicular.i);

			if (maximum_distance * maximum_distance > distance_squared)
			{
				s_sound_position position;
				vector3f forward = *direction;
				double distance = -sqrt((double)distance_squared);

				position.position.x = (real)(perpendicular.i * distance + camera->position.x);
				position.position.y = (real)(perpendicular.j * distance + camera->position.y);
				position.position.z = (real)(perpendicular.k * distance + camera->position.z);
				function_30bf0(&forward);
				position.compressed_forward = vector3d_compress(&forward);
				position.velocity = *g_4687a4;
				position.location = *location;
				function_1895f0(&position, 1.0f, tag_index);
				local_1 = true;
				goto local_2;
			}
		}
		goto local_2;

	}
local_2:
	return local_1;
}
#pragma inline_depth(255)
