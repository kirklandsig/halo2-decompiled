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
	word unknown0a : 7;
	word flag23 : 1;
	word : 8;
};

struct s_sound_play_tag
{
	word flags;
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

// starts a sound from a request: builds the play state and starts it, then, when both of the sound's category
// flags are set, moves the looping-sound globals' end times out to when this sound ends
// @retail 0x189fe0
long function_189fe0(s_sound_request const *request, long tag_index)
{
	long result = NONE;

	if (tag_index != NONE)
	{
		s_sound_play_tag *sound = (s_sound_play_tag *)g_4e3b44[tag_index & 0xffff].bytes;
		s_sound_source_callbacks const *source = request->source;
		s_sound_play_state state;
		long priority = request->location.audible ? 3 : 1;
		state.flags = 5;
		source ? (state.flags |= 0x20) : (state.flags &= ~0x20);
		request->object_index != NONE ? (state.flags |= 8) : (state.flags &= ~8);
		request->platform_playback != NONE ? (state.flags |= 0x100) : (state.flags &= ~0x100);
		// Retail also clears the padding after the priority field.
		*(long *)&state.priority = priority;
		state.location = request->location;
		state.object_index = request->object_index;
		state.source = source;
		state.platform_playback = request->platform_playback;

		if (TEST_FIELD_BIT(((s_sound_class_play_flags *)function_221810(sound->class_index))->flag23) && !(sound->flags & 0x100))
		{
			state.location.flags |= 0x800;
		}

		if (request->marker)
		{
			state.marker = *request->marker;
			state.source_data_size = sizeof(s_sound_marker);
		}
		else
		{
			state.source_data_size = 0;
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
				if (TEST_FIELD_BIT(state.location.flag8) || TEST_FIELD_BIT(state.location.flag11) || !state.location.audible)
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
		if (!(((s_sound_play_tag *)g_4e3b44[tag_index & 0xffff].bytes)->flags & 4))
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
/* the distance is a long double: as a plain double local the full image (not a reduced one) puts the
   success exit's `mov al, 1` ahead of both pops, where retail has it after the first; other functions'
   double locals elsewhere in the program decide that, as they did for 0xaaca0's frame */
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
				long double distance = -sqrt((double)distance_squared);

				position.position.x = (real)(perpendicular.i * distance + camera->position.x);
				position.position.y = (real)(perpendicular.j * distance + camera->position.y);
				position.position.z = (real)(perpendicular.k * distance + camera->position.z);
				function_30bf0(&forward);
				position.compressed_forward = vector3d_compress(&forward);
				position.velocity = *g_4687a4;
				position.location = *location;
				function_1895f0(&position, 1.0f, tag_index);
				local_1 = true;
			}
		}
	}
local_2:
	return local_1;
}
#pragma inline_depth(255)
