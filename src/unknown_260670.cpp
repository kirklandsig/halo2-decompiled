// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_260670.CPP: scoring an actor's candidate positions and choosing the
   reference to follow */

#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_0259a0.h"
#include "unknown_2626b0.h"
#include <float.h>
#include "unknown_25fc30.h"

bool function_260160(long actor_index, s_prop_search *search, s_261d20_entry *entry);
#include "props.h"
#include <string.h>
typedef bool (__stdcall *t_reference_compare)(long, long, const void *);
void sort_4byte(long *elements, unsigned long count, void *unused, t_reference_compare compare, const void *context);
bool __stdcall function_2600e0(long a, long b, void *unused);
void __stdcall function_25f7b0(long actor_index, s_type_967e20 *context, s_type_b36ac5 *position);
void function_270590(s_path_source *source, s_type_c3b527 const *point, long node_index);

struct firing_position_post_evaluator
{
	short flags;
	bool (__stdcall *proc)(long, s_type_967e20 *, s_type_b36ac5 *);
};
extern firing_position_post_evaluator g_44adf0[12];
extern s_type_b36ac5 *g_51eca0;
short g_51ec9c;

struct hash_node
{
	void *key;
	dword hash;
	hash_node *next;
	byte data[1];
};
struct hash_table
{
	byte unknown00[0x20];
	dword bucket_count;
	long maximum_count;
	long data_size;
	dword (__stdcall *hash_proc)(const void *key);
	bool (__stdcall *compare_proc)(const void *a, const void *b);
	c_data_allocator *allocator;
	hash_node *free_list;
	hash_node *buckets[1];
};
hash_node *function_13e2d0(hash_table *table, void *key);

PRIVATE inline bool reference_owner_find(hash_table *table, void *key, void *data)
{
	hash_node *node = function_13e2d0(table, key);
	if (node && data)
	{
		memcpy(data, node->data, table->data_size);
		return true;
	}
	return false;
}

struct s_reference_candidate_view;
struct s_reference_direction_request;
real function_30bf0(vector3f *vector);
void function_260530(s_reference_candidate_view *entry, s_reference_direction_request const *request);
void function_260060(long actor_index, long mode, s_type_967e20 *context, s_type_b36ac5 *position);
bool function_2600a0(long actor_index, s_type_967e20 *context, s_type_b36ac5 *position);
long function_1e4a50(long index);
struct s_260670_state
{
	s_reference result;
	bool within_limit;
	bool finite_distance;
	short selected;
	real best_score;
};

// score 700 (was 697): enabled from the draft: first difference +0x2a, 3148 bytes against 3077. The draft's two `goto done` as returns 700 (697 with them). Enabling it lets callers keep
// a bool output in a register across a later call, as retail (0x1b0540 matched with plain output locals).
// @retail 0x260670
s_reference __stdcall function_260670(long actor_index, s_2605d0_request const *request,
	s_261d20_entry *entries, short count, long reported_entry, long reported_owner, byte *scratch, bool *has_path)
{
	byte *actor = (byte *)actor_get(actor_index);
	s_type_967e20 *context = (s_type_967e20 *)request;
	byte *parameters = (byte *)context;
	s_type_b36ac5 *positions = (s_type_b36ac5 *)entries;
	s_260670_state state;
	state.result = g_470fa0;
	state.within_limit = false;
	state.finite_distance = false;
	state.selected = NONE;
	state.best_score = 0.0f;
	s_path_source source;
	s_path_settings settings;
	long sorted_indices[512];
	long sort_scratch;
	*has_path = false;
	((byte *)g_4f55d0)[3] = true;
	if (count == 0)
	{
		byte *history_actor = (byte *)actor_get(actor_index);
		*(short *)(history_actor + 0x3fe) = 3;
		byte *history = history_actor + 0x402;
		long remaining = 4;
		do
		{
			*(s_reference *)history = g_470fa0;
			history += 6;
		} while (--remaining);
		return g_470fa0;
	}
	if (parameters[0x618] && parameters[0x53])
	{
		if (parameters[0x56])
		{
			for (short i = 0; i < count; i++)
				function_260530((s_reference_candidate_view *)&positions[i], (s_reference_direction_request *)parameters);
		}
		else if (*(long *)(parameters + 0x64c) != NONE)
		{
			memset(&source, 0, sizeof(source));
			source.radius = *(real *)((byte *)function_1e4a50(*(long *)(actor + 0x54)) + 4);
			source.unknown04 = actor[0x3e4];
			source.object_index = NONE;
			source.unknown0c = NONE;
			function_270590(&source, (s_type_c3b527 *)(parameters + 0x650), *(long *)(parameters + 0x64c));
			source.unknown4c = 0.0f;
			source.unknown45 = true;
			source.unknown48 = 20.0f;
			function_1f9240(actor_index, &settings);
			byte *buffer = ai_scratch_buffer_get();
			function_271300((s_type_f17a25 *)buffer, NULL, &settings, &source, 0);
			function_2715a0(buffer);
			for (short i = 0; i < count; i++)
			{
				s_type_b36ac5 *position = &positions[i];
				if ((position->unknown58 && parameters[0x5a]) || (position->unknown59 && parameters[0x58]))
					function_260530((s_reference_candidate_view *)position, (s_reference_direction_request *)parameters);
				else
					function_270750(buffer, position->definition->unknown14, (s_actor_point_target *)position->definition,
						&position->unknown28, 0, parameters[0x54] ? (long)&position->unknown34 : 0);
			}
			ai_scratch_buffer_release(buffer);
		}
	}
	else if (parameters[0x54])
	{
		for (short i = 0; i < count; i++)
		{
			vector3d_from_points3d((point3f *)(parameters + 0x620), &positions[i].position, &positions[i].unknown34);
			function_30bf0(&positions[i].unknown34);
		}
	}
	if (parameters[0x55])
	{
		for (short i = 0; i < count; i++)
		{
			vector3d_from_points3d((point3f *)(parameters + 0x620), &positions[i].position, &positions[i].unknown40);
			normalize_inline(&positions[i].unknown40);
		}
	}
	if (!parameters[0x56])
	{
		function_1f90f0(actor_index, &source);
		source.unknown45 = true;
		source.unknown48 = *(real *)(parameters + 0x1c);
		if (parameters[0x46] && parameters[0x618])
		{
			long target_object = NONE;
			if (*(long *)(parameters + 0x664) != NONE)
				target_object = prop_ref_get(*(long *)(parameters + 0x664))->object_index;
			source.unknown28[0] = true;
			*(point3f *)(source.unknown28 + 4) = *(point3f *)(parameters + 0x620);
			*(long *)(source.unknown28 + 0x10) = target_object;
			*(real *)(source.unknown28 + 0x14) = *(real *)(parameters + 0x4c);
			source.unknown44 = !parameters[0x50];
			*(real *)(source.unknown28 + 0x18) = *(real *)(parameters + 0x48);
		}
		else if (*(short *)(actor + 0x358) > 0)
		{
			source.unknown28[0] = true;
			*(point3f *)(source.unknown28 + 4) = *(point3f *)(actor + 0x370);
			*(real *)(source.unknown28 + 0x14) = *(real *)(actor + 0x36c);
			*(long *)(source.unknown28 + 0x10) = *(long *)(actor + 0x360);
			source.unknown44 = true;
			*(real *)(source.unknown28 + 0x18) = 10.0f;
		}
		function_1f9240(actor_index, &settings);
		function_271300((s_type_f17a25 *)scratch, NULL, &settings, &source, 0);
		if (function_2715a0(scratch))
			*has_path = true;
	}
	for (short i = 0; i < count; i++)
	{
		s_type_b36ac5 *position = &positions[i];
		if (parameters[0x618])
		{
			vector3f delta;
			vector3d_from_points3d((point3f *)(parameters + 0x620), &position->position, &delta);
			position->unknown30 = delta.i * delta.i + delta.k * delta.k + delta.j * delta.j;
		}
		vector3f delta;
		vector3d_from_points3d((point3f *)(actor + 0x238), &position->position, &delta);
		real distance_squared = delta.i * delta.i + delta.k * delta.k + delta.j * delta.j;
		real range = *(real *)(parameters + 0x1c);
		if (range * range > distance_squared)
		{
			if (!parameters[0x56] && !(position->unknown58 && parameters[0x5a]) && !(position->unknown59 && parameters[0x58]))
				function_270750(scratch, position->definition->unknown14, (s_actor_point_target *)position->definition,
					&position->unknown18, (long)&position->unknown2c, parameters[0x51] ? (long)&position->unknown1c : 0);
			else
			{
				real perpendicular_squared;
				if (distance_squared > 0.0001f)
				{
					vector3f target_delta;
					vector3d_from_points3d((point3f *)(actor + 0x238), (point3f *)(parameters + 0x620), &target_delta);
					real t = (target_delta.k * delta.k + target_delta.j * delta.j + target_delta.i * delta.i) / distance_squared;
					t = t < 0.0f ? 0.0f : t > 1.0f ? 1.0f : t;
					t = 0.0f - t;
					target_delta.i = t * delta.i + target_delta.i;
					target_delta.j = t * delta.j + target_delta.j;
					target_delta.k = t * delta.k + target_delta.k;
					perpendicular_squared = target_delta.k * target_delta.k + target_delta.j * target_delta.j + target_delta.i * target_delta.i;
				}
				else
				{
					vector3f target_delta;
					vector3d_from_points3d((point3f *)(parameters + 0x620), (point3f *)(actor + 0x238), &target_delta);
					perpendicular_squared = target_delta.k * target_delta.k + target_delta.j * target_delta.j + target_delta.i * target_delta.i;
				}
				position->unknown2c = (real)sqrt((double)perpendicular_squared);
				real length = (real)sqrt((double)distance_squared);
				if ((double)0.0001f > fabs((double)length))
					length = 0.0f;
				else
				{
					real scale = 1.0f / length;
					delta.i *= scale;
					delta.j *= scale;
					delta.k *= scale;
				}
				position->unknown18 = length;
				if (parameters[0x51])
					position->unknown1c = delta;
			}
		}
		if (*(real *)(parameters + 0x1c) > position->unknown18)
			state.within_limit = true;
		else
		{
			position->unknown4c = false;
			if (FLT_MAX > position->unknown18)
				state.finite_distance = true;
		}
	}
	if (!state.within_limit && !state.finite_distance && !parameters[0x56] && !actor[0x264] &&
		(*(long *)(actor + 0x28c) == NONE || (*has_path && count > 0 && *(short *)(scratch + 0xae) < 8)))
		++*(short *)(actor + 0x5e8);
	else
		*(short *)(actor + 0x5e8) = 0;
	if (parameters[0x15] && !state.within_limit)
	{
		dword *seed = (dword *)g_4e7408;
		*seed = *seed * 0x19660d + 0x3c6ef35f;
		state.selected = (short)(((*seed >> 16) * count) >> 16);
		*has_path = false;
		byte *history_actor = (byte *)actor_get(actor_index);
		*(short *)(history_actor + 0x3fe) = 3;
		byte *history = history_actor + 0x402;
		long remaining = 4;
		do
		{
			*(s_reference *)history = g_470fa0;
			history += 6;
		} while (--remaining);
		if (!function_260160(actor_index, (s_prop_search *)context, &entries[state.selected]))
			return state.result;
	}
	else
	{
		function_260060(actor_index, count, context, positions);
		for (short i = 0; i < count; i++)
			sorted_indices[i] = i;
		g_51ec9c = count;
		g_51eca0 = positions;
		sort_4byte(sorted_indices, count, &sort_scratch, (t_reference_compare)function_2600e0, NULL);
		context->unknown680 = 0.0f;
		bool possible = true;
		for (long evaluator = 0; g_44adf0[evaluator].proc && possible; evaluator++)
		{
			if ((1 << context->type) & g_44adf0[evaluator].flags)
				possible = g_44adf0[evaluator].proc(actor_index, context, NULL);
		}
		parameters[0x67c] = possible;
		for (short i = 0; i < count; i++)
		{
			short index = (short)sorted_indices[i];
			s_type_b36ac5 *position = &positions[index];
			if (!position->unknown4c || (parameters[0x67c] && state.best_score >= position->score + context->unknown680))
				break;
			if (parameters[0x618])
				function_25f7b0(actor_index, context, position);
			position->unknown50 = position->score;
			if (function_2600a0(actor_index, context, position) && position->score > state.best_score)
			{
				state.selected = index;
				state.best_score = position->score;
			}
		}
		if (state.selected == NONE)
			return state.result;
	}
	if (reported_entry)
		memcpy((void *)reported_entry, &positions[state.selected], sizeof(s_type_b36ac5));
	state.result = positions[state.selected].reference;
	if (reported_owner)
	{
		long owner = NONE;
		reference_owner_find((hash_table *)g_557c6c, *(void **)&state.result, &owner);
		*(long *)reported_owner = owner != actor_index ? owner : NONE;
	}
	if (actor[0x3f2] && !actor[0x220] && !positions[state.selected].unknown5b)
		actor[0x3f2] = false;
	if (parameters[0x56])
	{
		function_1f90f0(actor_index, &source);
		s_type_f17a25 *path = (s_type_f17a25 *)scratch;
		path->unknown54 = false;
		path->unknown90 = NONE;
		path->unknownae = 0;
		path->heap_count = 0;
		path->location.unknown00 = 0;
		path->location.unknown02 = 0;
		path->pathfinding = *(long *)((byte *)g_4e0348 + 0xc4) > 0 ? *(void **)((byte *)g_4e0348 + 0xc8) : NULL;
		path->flags = 0;
		path->unknownac = false;
		path->source = source;
		memset(&path->settings, 0, sizeof(path->settings));
		path->unknown14188 = 0;
		memcpy(path->unknown140d4, parameters + 0x6a0, 0xb6);
		*has_path = true;
	}
	return state.result;
}
