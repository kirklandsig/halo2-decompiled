#include "unknown_11c920.h"
#include <stdlib.h>
#include "globals.h"
#include "unknown_1fb7e0.h"
#include "unknown_25d020.h"
#include <string.h>

// @flags /O2 /arch:SSE /Gr

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
bool function_1df560(short team, short other_team);
void function_25b910(long actor_index, long prop_ref_index);
point3f *function_b9dd0(long object_index, point3f *position);
real function_30bf0(vector3f *vector);
long function_264260(vector3f const *direction, vector3f const *facing, real distance);
short ai_player_index_get(long player_index);
void function_268c60(long actor_index);
long function_25d810(long object_index, long actor_index, bool create);
bool function_25ccd0(s_type_5cfb45 *state, long object_index, short value, s_2640c0 *motion);
void function_265c30(long prop_index, long actor_index, bool active);
bool __stdcall function_265290(long actor_index, long prop_index);
bool __stdcall function_265550(long actor_index, long prop_index);
void function_ba1d0(long object_index, vector3f *linear, vector3f *angular);
void function_cb7e0(long unit_index, vector3f *vector);
long function_10f8f0(long object_index);
struct s_28fb50;
long function_28fb50(long object_index, s_28fb50 const *context, long target_index);
__forceinline long real_to_long(real value);

// @retail 0x265050
void function_265050(long actor_index, long prop_ref_index)
{
	struct { byte *definition; long volatile missing_owner; } scratch;
	long const volatile *prop_reference = &prop_ref_index;
	byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
	byte *reference = g_502418->data + (prop_ref_index & 0xffff) * 0x3c;
	long object_index = *(long *)(reference + 0x20);
	byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
	byte *&definition = scratch.definition;
	definition = *(byte **)((byte *)g_4e3b44 + (*(long *)object & 0xffff) * 16 + 8);
	if (*(real *)(definition + 0xd0) + 10.0f > *(real *)(reference + 0x28))
	{
		short state = *(short *)(actor + 0x358);
		if (state < 2 || (state == 2 && *(long *)(actor + 0x360) != object_index &&
			*(real *)(actor + 0x394) > *(real *)(reference + 0x28)))
		{
			memset(actor + 0x358, 0, 0x58);
			*(short *)(actor + 0x358) = 2;
			*(long *)(actor + 0x360) = *(long *)(reference + 0x20);
			*(real *)(actor + 0x36c) = *(real *)(definition + 0xd0);
			*(short *)(actor + 0x35a) = 0;
			*(long *)(actor + 0x368) = *prop_reference;
			long parent_index = *(long *)(object + 0xc8);
            scratch.missing_owner = NONE;
            long selected;
            if (parent_index == NONE) goto owner_missing;
            {
                byte *parent = (byte *)function_badc0(parent_index, NONE);
                if (!parent || !((1 << parent[0xaa]) & 3)) goto owner_missing;
                selected = parent_index;
                long unit_index = *(long *)(actor + 0x18);
                if (unit_index != NONE && selected == unit_index)
                    *(short *)(actor + 0x35a) = 2;
                else if (!function_1df560(*(short *)(actor + 0x24), *(short *)(parent + 0x138)))
                    *(short *)(actor + 0x35a) = 1;
            }
            goto owner_selected;
owner_missing:
            selected = scratch.missing_owner;
owner_selected:
            *(long *)(actor + 0x364) = selected;
			function_25b910(actor_index, *prop_reference);
		}
	}
}

// @retail 0x264940
void function_264940(long actor_index, long prop_ref_index)
{
	byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
	byte *reference = g_502418->data + (prop_ref_index & 0xffff) * 0x3c;
	byte *prop = g_50241c->data + (*(long *)(reference + 8) & 0xffff) * 0xc4;
	actor[0x30c] = false;
	if (!prop[0x23] && 3.0f > *(real *)(reference + 0x28) && *(char *)(reference + 0x27) >= 3)
	{
		long object_index = *(long *)(reference + 0x20);
		point3f position;
		function_b9dd0(object_index, &position);
		byte *headers = g_4e0300->data;
		byte *object = *(byte **)(headers + (object_index & 0xffff) * 12 + 8);
		vector3f facing = *(vector3f *)(object + 0x168);
		vector3f direction;
		direction.i = position.x - *(real *)(actor + 0x238);
		direction.j = position.y - *(real *)(actor + 0x23c);
		direction.k = position.z - *(real *)(actor + 0x240);
		if (function_30bf0(&direction) == 0.0f)
			direction = *g_4687a8;
		short category = (short)function_264260(&direction, &facing, *(real *)(reference + 0x28));
		if (!prop[0x31])
		{
			if (category <= 2)
			{
				function_1fb7e0(0xa3, actor_index, NULL, object_index, NONE);
				prop[0x31] = true;
				*(short *)(actor + 0x30e) = 0;
			}
		}
		else
		{
			object = *(byte **)(headers + (object_index & 0xffff) * 12 + 8);
			long player_index = *(long *)(object + 0x13c);
			if (player_index != NONE)
			{
				if (category <= 0)
				{
					actor[0x30c] = true;
					*(short *)(actor + 0x310) = ai_player_index_get(player_index);
				}
				else
				{
					actor[0x30c] = false;
					*(short *)(actor + 0x310) = NONE;
					*(short *)(actor + 0x30e) = 0;
				}
			}
		}
	}
	long child_index = *(long *)(actor + 0x7c);
	if (child_index != NONE && *(long *)(reference + 0x14) != NONE)
	{
		byte *tracking = g_502414->data + (*(long *)(reference + 0x14) & 0xffff) * 0x124;
		if (tracking && tracking + 0x70 && *(short *)(tracking + 0x70) >= 4 &&
			25.0f > *(real *)(reference + 0x28))
			function_268c60(child_index);
	}
}

// @retail 0x264330
void __stdcall function_264330(long actor_index, long prop_ref_index, s_2641c0 *context,
	s_2640c0 *motion, bool force)
{
	byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
	if (!actor[9]) return;
	s_prop_datum *reference = prop_ref_get(prop_ref_index);
	byte *object = *(byte **)(g_4e0300->data + (reference->object_index & 0xffff) * 12 + 8);
	byte *prop = (byte *)prop_get(reference->prop_index);
	long time = g_510c54->game_time;
	byte *unit = ((1 << object[0xaa]) & 3) ? object : NULL;
	bool update = false;
	if ((reference->state >= 1 && reference->state <= 2) || force ||
		(reference->state >= 1 && reference->unknown26 > 0))
		update = true;
	if (reference->state >= 1 && reference->state <= 2)
		reference->type = *(short *)(prop + 2);
	else if (reference->type != *(short *)(prop + 2) && unit && *(short *)(prop + 2) == 6 &&
		(real)(time - *(long *)(unit + 0x2e0)) * g_510c54->rate > 5.0f)
		reference->type = 6;
	s_type_f95cd3 *view = NULL;
	s_type_5cfb45 *state = NULL;
	if (reference->tracking_index != NONE)
	{
		s_type_e5ff81 *tracking = tracking_get(reference->tracking_index);
		state = &tracking->state;
		view = &tracking->view;
	}
	else if (g_470f10[*(short *)(prop + 2)].kind == 1 && g_510c54->game_time > *(long *)(prop + 0x58))
		state = (s_type_5cfb45 *)(prop + 0x58);
	if (state && update)
	{
		if (view && prop[0x22] && *(long *)(prop + 0x1c) != NONE)
		{
			byte *other = g_4f55f0->data + (*(long *)(prop + 0x1c) & 0xffff) * 0x888;
			long target = function_28fb50(*(long *)(other + 0x1c), (s_28fb50 const *)context, reference->object_index);
			if (target != reference->object_index) reference->object_index = target;
		}
		function_25ccd0(state, reference->object_index, reference->unknown1c, motion);
	}
	state = function_25d690(reference);
	if (!(view)) {
		real x = state->position.x - context->field_c.x;
		real y = state->position.y - context->field_c.y;
		real z = state->position.z - context->field_c.z;
		reference->unknown28 = (real)sqrt(z * z + y * y + x * x);
	} else {
		vector3f *direction = (vector3f *)((byte *)view + 0x2c);
		direction->i = state->position.x - context->field_c.x;
		direction->j = state->position.y - context->field_c.y;
		direction->k = state->position.z - context->field_c.z;
		reference->unknown28 = function_30bf0(direction);
		if (reference->unknown28 == 0.0f) *direction = *g_4687a8;
		if (view->unknown50 != NONE && view->unknown50 + real_to_long((real)g_510c54->field_2_3 * 5.0f) < time)
			function_265c30(prop_ref_index, actor_index, false);
		if (update)
		{
			vector3f velocity;
			function_ba1d0(reference->object_index, &velocity, NULL);
			real speed = (real)sqrt(velocity.k * velocity.k + velocity.j * velocity.j + velocity.i * velocity.i);
			byte *fields = (byte *)view;
			fields[0x3a] = speed < 0.1f ? 0 : speed < 0.5f ? 1 : speed < 1.0f ? 2 : 3;
			real approach = 0.0f - (direction->k * velocity.k + direction->j * velocity.j + direction->i * velocity.i);
			fields[0x3b] = approach < -1.0f ? 0 : approach < -0.5f ? 1 : approach < -0.1f ? 2 :
				approach < 0.1f ? 3 : approach < 0.5f ? 4 : approach < 1.0f ? 5 : 6;
			if (*(char *)(fields + 0x3b) < 4)
			{
				view->unknown64 = false;
				view->unknown66 = 0;
			}
			else
			{
				++view->unknown66;
				if (view->unknown66 >= real_to_long((real)g_510c54->field_2_3 * 0.2f))
				{
					view->unknown66 = g_510c54->field_2_3;
					view->unknown64 = true;
				}
			}
			fields[0x38] = reference->unknown28 < 1.0f ? 0 : reference->unknown28 < 6.0f ? 1 :
				reference->unknown28 < 10.0f ? 2 : reference->unknown28 < 30.0f ? 3 : 4;
			vector3f facing;
			if (unit) function_cb7e0(reference->object_index, &facing);
			else facing = *(vector3f *)(object + 0x70);
			view->unknown39 = (char)function_264260(direction, &facing, reference->unknown28);
		}
	}
	if (state->unknown66)
	{
		long related_index = function_25d810(state->unknown3c, actor_index, false);
		if (related_index != NONE)
		{
			s_prop_datum *related = prop_ref_get(related_index);
			point3f position;
			function_b9dd0(related->object_index, &position);
			real x = position.x - context->field_c.x;
			real y = position.y - context->field_c.y;
			real z = position.z - context->field_c.z;
			related->unknown28 = (real)sqrt(z * z + y * y + x * x);
			function_265290(actor_index, related_index);
		}
	}
	else if (unit && (state->unknown5f || function_10f8f0(reference->object_index) == 0x500000a))
		function_265550(actor_index, prop_ref_index);
	else if (reference->type == 3)
		function_265050(actor_index, prop_ref_index);
	else if (reference->type == 4)
		function_265290(actor_index, prop_ref_index);
	if (prop[0x25]) function_264940(actor_index, prop_ref_index);
	if (view) *(real *)((byte *)view + 0x3c) = function_265d30(actor_index, prop_ref_index);
}

extern long g_4de2fc;
extern bool g_4de2f8;
extern long g_4de300[0x800];
int __cdecl function_2631f0(void const *first, void const *second);
void function_25c780(long actor_index, long prop_ref_index);
bool function_25db60();
void prop_view_initialize(s_type_f95cd3 *view);
struct s_actor_object_sample;
bool function_28fa60(long perception_index, point3f const *point, s_actor_object_sample *sample);
struct s_object_motion_view;
void function_2640c0(long object_index, s_object_motion_view *result);

struct s_tracking_candidate
{
	long index;
	short priority;
};

PRIVATE inline void tracking_state_initialize(s_type_5cfb45 *state)
{
	state->unknown40 = NONE;
	state->unknown00 = NONE;
	state->unknown3c = NONE;
	state->unknown66 = false;
	state->unknown62 = false;
	state->unknown5e = false;
	state->unknown64 = false;
	state->unknown61 = false;
	state->unknown5f = false;
	state->unknown60 = true;
	state->unknown63 = false;
	state->unknown69 = false;
	state->unknown65 = false;
	state->unknown67 = false;
	state->unknown48 = *g_468788;
	state->unknown54 = NONE;
	state->unknown44 = NONE;
	state->unknown58 = false;
}

// @retail 0x263210
void __stdcall function_263210(long actor_index)
{
	long next_index = actor_prop_view_get(actor_index)->first_prop_index;
	short count = 0;
	short active_count = 0;
	short current = 0;
	bool reclaimed = false;
	s_tracking_candidate candidates[50];
	s_2640c0 motion;
	s_2641c0 sample;
	s_actor_prop_view *initial_actor = actor_prop_view_get(actor_index);
	s_actor_prop_view *volatile actor = initial_actor;
	++g_4de2fc;
	g_4de2f8 = true;
	long *tracked = initial_actor->tracked_prop_indices;
	long slots_remaining = 8;
	do
	{
		long index = *tracked;
		if (index != NONE)
		{
			s_prop_datum *reference = (s_prop_datum *)datum_get_inlined(g_502418, index);
			if (reference && reference->tracking_index != NONE)
			{
				long absolute_index = reference->object_index & 0xffff;
				if (g_4de300[absolute_index] != g_4de2fc)
					g_4de300[absolute_index] = g_4de2fc;
				short priority = *(short *)((byte *)prop_get(reference->prop_index) + 0xc);
				reference->unknown1a = priority;
				if (priority > 0 && g_470f10[reference->type].unknown8 == 2)
				{
					candidates[count].index = index;
					candidates[count].priority = priority;
					count++;
				}
				else
					function_25c780(actor_index, index);
			}
			else
				*tracked = NONE;
		}
		tracked++;
	}
	while (--slots_remaining);
	while (next_index != NONE)
	{
		long index = next_index;
		s_prop_datum *reference = prop_ref_get(index);
		next_index = reference->next_index;
		if (reference->state >= 1)
		{
			long absolute_index = reference->object_index & 0xffff;
			if (g_4de300[absolute_index] != g_4de2fc)
			{
				g_4de300[absolute_index] = g_4de2fc;
				short priority = *(short *)((byte *)prop_get(reference->prop_index) + 0xc);
				reference->unknown1a = priority;
				if (g_470f10[reference->type].unknown8 == 2 && count < 50)
				{
					candidates[count].index = index;
					candidates[count].priority = priority;
					count++;
				}
			}
		}
	}
	g_4de2f8 = false;
	if (count > 8)
		qsort(candidates, count, sizeof(s_tracking_candidate), function_2631f0);
	s_record_pool *tracking_pool = g_502414;
	while (current < count && active_count < 8)
	{
		long index = candidates[current].index;
		s_prop_datum *reference = prop_ref_get(index);
		if (reference->tracking_index == NONE)
		{
			reference->tracking_index = record_pool_allocate(tracking_pool);
			if (reference->tracking_index == NONE && !reclaimed)
			{
				if (function_25db60())
					reference->tracking_index = record_pool_allocate(tracking_pool);
				reclaimed = true;
			}
			if (reference->tracking_index != NONE)
			{
				s_type_e5ff81 *tracking = (s_type_e5ff81 *)(tracking_pool->data + (reference->tracking_index & 0xffff) * sizeof(s_type_e5ff81));
				tracking_state_initialize(&tracking->state);
				prop_view_initialize(&tracking->view);
				function_2640c0(reference->object_index, (s_object_motion_view *)&motion);
				byte *local_bbc190 = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
				bool sampled = true;
				if (local_bbc190[7] && *(long *)(local_bbc190 + 0x1c) != NONE)
					sampled = function_28fa60(*(long *)(local_bbc190 + 0x1c), &motion.field_c, (s_actor_object_sample *)&sample);
				else
					memcpy(&sample, local_bbc190 + 0x22c, sizeof(sample));
				if (sampled)
				{
					function_264330(actor_index, index, &sample, &motion, false);
					tracking_pool = g_502414;
				}
			}
		}
		if (reference->tracking_index != NONE)
			actor->tracked_prop_indices[active_count++] = index;
		current++;
	}
	if (active_count < 8)
		memset(&actor->tracked_prop_indices[active_count], 0xff, (word)(8 - active_count) * sizeof(long));
	for (; current < count; current++)
	{
		long index = candidates[current].index;
		s_prop_datum *reference = prop_ref_get(index);
		if (reference->tracking_index != NONE)
		{
			if (reference->tracking_index != NONE)
				function_25c780(actor_index, index);
			reference->state = 0;
			reference->unknown10 = 0.0f;
		}
	}
}

long function_baf40(long object_index);
short __stdcall function_1c8df0(long object_index, point3f const *target, short target_cell, short source_cell,
	void const *source, long mode, bool narrow, bool attached, bool alternate, long *hit_object);
long function_25c9d0(long actor_index, long prop_ref_index, bool player, long sight,
	void *point, byte *sample, long reported);
long function_25cb60(long actor_index, long object_index, short type, void *sample,
	void *point, long location, long sight);
short function_25cca0(long prop_ref_index);
short function_263810(point3f const *origin, long actor_index, point3f const *point, point3f const *endpoint, char posture, short mode, bool use_facing, bool *out_of_range);
void function_26bfa0(long object_index, long *location_index, s_location_view *location);
real function_296600(long actor_index, long prop_ref_index);

// @retail 0x264b50
void __stdcall function_264b50(long actor_index, long prop_ref_index, s_2641c0 *sample, s_2640c0 *motion)
{
	s_type_f95cd3 *view = NULL;
	byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
	byte *volatile saved_actor = actor;
	if (!actor[9])
		return;
	byte *squad = NULL;
	if (*(long *)(actor + 0x30) != NONE)
		squad = g_51e9d8->data + (*(long *)(actor + 0x30) & 0xffff) * 0x98;
	s_prop_datum *reference = prop_ref_get(prop_ref_index);
	if (reference->tracking_index != NONE)
	{
		s_type_e5ff81 *tracking = tracking_get(reference->tracking_index);
		if (tracking)
			view = &tracking->view;
	}
	byte *prop = (byte *)prop_get(reference->prop_index);
	long time = g_510c54->game_time;
	byte *object = *(byte **)(g_4e0300->data + (reference->object_index & 0xffff) * 12 + 8);
	bool suppressed = (squad && (squad[2] & 1)) || *(short *)(actor + 0x84) == 1 || actor[0x228];
	bool current = *(long *)(actor + 0x338) == prop_ref_index;
	byte *unit = object[0xaa] == 0 ? object : NULL;
	if (*(short *)(actor + 4) == 15 && *(long *)(actor + 0x26c) != NONE)
	{
		byte *parent = *(byte **)(g_4e0300->data + (*(long *)(actor + 0x26c) & 0xffff) * 12 + 8);
		long other_unit = *(long *)(parent + 0x248);
		s_type_f95cd3 *other_view = NULL;
		bool copied = false;
		if (other_unit != NONE && other_unit != *(long *)(actor + 0x18))
		{
			byte *other = *(byte **)(g_4e0300->data + (other_unit & 0xffff) * 12 + 8);
			long other_actor = *(long *)(other + 0x12c);
			if (other_actor != NONE)
			{
				long other_index = function_25d810(reference->object_index, other_actor, false);
				if (other_index != NONE)
				{
					s_prop_datum *other_reference = prop_ref_get(other_index);
					other_view = function_25d740((s_prop_node *)other_reference);
					reference->unknown26 = other_reference->unknown26;
					reference->unknown27 = other_reference->unknown27;
					copied = true;
				}
			}
		}
		if (!copied)
		{
			reference->unknown27 = 0;
			reference->unknown26 = 0;
		}
		if (view)
		{
			if (other_view)
			{
				view->unknown06 = other_view->unknown06;
				*(short *)((byte *)view + 2) = *(short *)((byte *)other_view + 2);
				*(short *)((byte *)view + 4) = *(short *)((byte *)other_view + 4);
				view->unknown8a = other_view->unknown8a;
				view->unknown8c = other_view->unknown8c;
			}
			else
			{
				view->unknown06 = 4;
				view->unknown8c = 4;
				*(short *)((byte *)view + 4) = 0;
				*(short *)((byte *)view + 2) = 0;
				view->unknown8a = 0;
			}
		}
	}
	else
	{
		long root = function_baf40(unit && *(long *)(unit + 0x14) != NONE ? *(long *)(unit + 0x14) : reference->object_index);
		long mode = prop[0x25] && prop[0x23] ? 2 : 0;
		bool alternate = !current;
		short sight = function_1c8df0(root, &motion->field_0, *(short *)((byte *)motion + 0x28),
			*(short *)((byte *)sample + 0x28), sample, mode, false,
			*(long *)(actor + 0x26c) != NONE, alternate, current ? (long *)(actor + 0x348) : NULL);
		actor = saved_actor;
		if (*(long *)(actor + 0x7c) != NONE && prop[0x25] && 25.0f > reference->unknown28 &&
			(sight == 0 || sight == 1))
			function_268c60(*(long *)(actor + 0x7c));
		if (view)
			view->unknown06 = (short)sight;
		if (!prop[0x26] && !suppressed)
		{
			short priority = (short)function_25c9d0(actor_index, prop_ref_index, prop[0x25] != 0, sight,
				&motion->field_c, (byte *)sample, (long)&suppressed);
			if (reference->state >= 3 && view)
			{
				s_type_5cfb45 *state = function_25d690(reference);
				if (!view->unknown88)
				{
					long other_mode = prop[0x25] && prop[0x23] ? 2 : 0;
					short other_sight = function_1c8df0(root, (point3f const *)((byte *)state + 0x30), *(short *)((byte *)state + 0x2c),
						*(short *)((byte *)sample + 0x28), sample, other_mode, false,
						*(long *)(actor + 0x26c) != NONE, alternate, current ? (long *)(actor + 0x348) : NULL);
					view->unknown8c = (short)other_sight;
					view->unknown8a = function_263810((point3f const *)sample, actor_index, (point3f const *)((byte *)state + 0x30), NULL, 2, (short)other_sight, true, NULL);
				}
				else
				{
					view->unknown8a = 3;
					if (current)
						*(long *)(actor + 0x348) = NONE;
				}
			}
			short object_priority = (short)function_25cb60(actor_index, reference->object_index, reference->unknown1c,
				sample, &motion->field_c, (long)&motion->field_24, sight);
			short extra_priority = function_25cca0(prop_ref_index);
			reference->unknown27 = (char)priority;
			if (view)
			{
				*(short *)((byte *)view + 2) = (short)object_priority;
				*(short *)((byte *)view + 4) = (short)extra_priority;
			}
			short combined = object_priority > extra_priority ? object_priority : extra_priority;
			reference->unknown26 = (char)(priority > combined ? priority : combined);
			if (reference->unknown26 == 1 && reference->state >= 1)
				reference->unknown26 = 2;
		}
		else
		{
			reference->unknown27 = 0;
			reference->unknown26 = 0;
			if (view)
			{
				*(short *)((byte *)view + 4) = 0;
				*(short *)((byte *)view + 2) = 0;
			}
		}
	}
	if (view)
	{
		if (reference->unknown26 >= 2)
			*(long *)((byte *)view + 0xc) = time;
		if (reference->unknown27 > 0)
		{
			function_26bfa0(reference->object_index, NULL, (s_location_view *)((byte *)view + 0x18));
			view->unknown10 = time;
		}
		*(real *)((byte *)view + 0x3c) = function_265d30(actor_index, prop_ref_index);
		*(real *)((byte *)view + 0x40) = function_296600(actor_index, prop_ref_index);
	}
	function_25d690(reference)->unknown68 = 1;
}


long function_1e4a10(long index);

// @retail 0x265550
bool __stdcall function_265550(long actor_index, long prop_ref_index)
{
 byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
 byte *reference = g_502418->data + (prop_ref_index & 0xffff) * 0x3c;
 bool result = false;
 real range = 0.0f;
 byte *definition = (byte *)function_1e4a10(*(long *)(actor + 0x54));
 if (definition)
  range = *(real *)(definition + 0x64);
 if ((double)range > 0.0 && *(short *)(reference + 0x24) >= 1 && range + 10.0f > *(real *)(reference + 0x28))
 {
  short state = *(short *)(actor + 0x358);
  if (state < 1 || (state == 1 && *(long *)(actor + 0x360) != *(long *)(reference + 0x20) &&
   *(real *)(actor + 0x394) > *(real *)(reference + 0x28)))
  {
   byte *prop = g_50241c->data + (*(long *)(reference + 8) & 0xffff) * 0xc4;
   memset(actor + 0x358, 0, 0x58);
   *(short *)(actor + 0x358) = 1;
   *(long *)(actor + 0x360) = *(long *)(reference + 0x20);
   *(real *)(actor + 0x36c) = range;
   *(short *)(actor + 0x35a) = !prop[0x23];
   function_25b910(actor_index, prop_ref_index);
   result = true;
  }

 }
 return result;
}



bool function_2651e0(long object_index, short *volatile output_index);

// @retail 0x265290
bool __stdcall function_265290(long actor_index, long prop_ref_index)
{
 byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
 byte *reference = g_502418->data + (prop_ref_index & 0xffff) * 0x3c;
 long object_index = *(long *)(reference + 0x20);
 byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
 byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
 bool result = false;
 bool const *result_reference = &result;
 if (*(long *)(object + 0x248) == NONE && 13.5f > *(real *)(reference + 0x28))
 {
  short state = *(short *)(actor + 0x358);
  if (state < 4 || (state == 4 && *(long *)(actor + 0x360) != object_index &&
   *(real *)(actor + 0x394) > *(real *)(reference + 0x28)))
  {
   result = function_2651e0(object_index, NULL);
   if (*result_reference)
   {
    memset(actor + 0x358, 0, 0x58);
    *(short *)(actor + 0x358) = 4;
    *(long *)(actor + 0x360) = *(long *)(reference + 0x20);
    *(real *)(actor + 0x36c) = 3.5f;
    *(short *)(actor + 0x35a) = 0;
    *(long *)(actor + 0x368) = prop_ref_index;
    *(long *)(actor + 0x364) = NONE;
    function_25b910(actor_index, prop_ref_index);
    goto done;
   }
  }
 }
 long other_index = *(long *)(actor + 0x26c);
 if (other_index != NONE)
 {
  byte *other = *(byte **)(g_4e0300->data + (other_index & 0xffff) * 12 + 8);
  byte *other_definition = g_4e3b44[*(long *)other & 0xffff].bytes;
  bool lower = *(short *)(other_definition + 0x244) < *(short *)(definition + 0x244);
  bool const *lower_reference = &lower;
  if (!*lower_reference)
   goto done;
 }
 if ((bool)(((dword)*(dword *)(definition + 0x1ec) >> 7) & 1))
 {
  vector3f *velocity = (vector3f *)(object + 0x88);
  if (velocity->i * velocity->i + velocity->j * velocity->j + velocity->k * velocity->k > 1.0f &&
   *(real *)(definition + 4) + 10.0f > *(real *)(reference + 0x28))
  {
   short state = *(short *)(actor + 0x358);
   if (state < 3 || (state == 3 && *(long *)(actor + 0x360) != *(long *)(reference + 0x20) &&
    *(real *)(actor + 0x394) > *(real *)(reference + 0x28)))
   {
    memset(actor + 0x358, 0, 0x58);
    *(short *)(actor + 0x358) = 3;
    *(long *)(actor + 0x360) = *(long *)(reference + 0x20);
    real range = *(real *)(definition + 4);
    long owner = *(long *)(object + 0x248);
    long const *owner_reference = &owner;
    *(long *)(actor + 0x364) = owner;
    *(real *)(actor + 0x36c) = range;
    *(short *)(actor + 0x35a) = 0;
    *(long *)(actor + 0x368) = prop_ref_index;
    if (*owner_reference != NONE)
    {
     byte *unit = *(byte **)(g_4e0300->data + (owner & 0xffff) * 12 + 8);
     if (!function_1df560(*(short *)(actor + 0x24), *(short *)(unit + 0x138)))
      *(short *)(actor + 0x35a) = 1;
    }
    function_25b910(actor_index, prop_ref_index);
    result = true;
   }
  }
 }
done:
 return *result_reference;
}

