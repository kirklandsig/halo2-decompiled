// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_26e370.h"
#include "unknown_11cc90.h"
#include "vector2f_products.h"
#include "unknown_272b70.h"

/* slot type 0x7e: the actors of a group take the positions of a formation
   around their leader */

struct s_slot_7e
{
	s_slot_header header;
	bool unknown0c;
	byte unknown0d[3];
	long element_index;
	byte unknown14[0x1c - 0x14];
	short position_index;
	short unknown1e;
	long unknown20;
	bool unknown24;
	byte unknown25[3];
	long unknown28;
	long unknown2c;
	byte unknown30[0x40 - 0x30];
};

bool __stdcall function_1f4280(long actor_index);
real normalize2d(point2f *v);

/* the points function_25ab50 validates: g_4e0350 + 0x1dc */
struct s_point_block
{
	byte unknown00[0x24];
	byte *points;
	byte unknown28[0x30 - 0x28];
};

struct s_point_globals
{
	long count;
	s_point_block *blocks;
};

struct s_4e0350_point_view
{
	byte unknown000[0x1dc];
	s_point_globals *points;
};

struct s_point_entry
{
	byte unknown00[0x20];
	s_type_c3b527 location;
	long unknown30;
	byte unknown34[0x3c - 0x34];
};

__forceinline s_point_entry *point_entry_get(long reference)
{
	s_point_block *block = &((s_4e0350_point_view *)g_4e0350)->points->blocks[(reference >> 16) & 0xffff];

	return (s_point_entry *)block->points + (reference & 0xffff);
}

__forceinline point2f *point_get(long reference)
{
	return (point2f *)&point_entry_get(reference)->location;
}

vector2f *g_468778;

static inline real distance2d(point2f const *a, point2f const *b)
{
	real dx = a->x - b->x;
	real dy = a->y - b->y;
	real distance = dx * dx;

	distance += dy * dy;
	return (real)sqrt(distance);
}

short __stdcall function_1b0cd0(long actor_index);
bool __stdcall function_1b13b0(long actor_index, s_slot *slot, long index);

// @retail 0x1b0cd0
short __stdcall function_1b0cd0(long actor_index)
{
	s_slot_entry_iterator iterator;
	long result = 0;

	iterator.actor_index = actor_index;
	iterator.reference.unknown2 = 0x7e;
	iterator.reference.unknown0 = NONE;
	if (function_26f0c0(&iterator))
		result = 3;
	return result;
}

// @retail 0x1b0d10
short __stdcall function_1b0d10(long actor_index, s_slot *slot, bool active)
{
	s_slot_7e *state = (s_slot_7e *)slot;
	short result = g_46fbe8;

	if (element_502424_get(state->element_index)->unknown04 == NONE)
		result = g_46fbe4;
	return result;
}

/* returns the element index the slot keeps: the remembered entry's, or the
   newly chosen one (NONE when there is none) */
// @retail 0x1b0d50
long __stdcall function_1b0d50(long actor_index, s_slot *slot)
{
	s_slot_7e *state = (s_slot_7e *)slot;
	s_slot_entry_iterator iterator;

	iterator.actor_index = actor_index;
	iterator.reference.unknown2 = 0x7e;
	iterator.reference.unknown0 = NONE;
	for (s_slot_memory_entry *entry = function_26f0c0(&iterator); entry; entry = function_26f0c0(&iterator))
	{
		if (function_26ecc0(actor_index, iterator.reference.unknown0, (s_joint_behavior_state *)slot))
		{
			long entry_value = entry->unknown4;
			state->position_index = NONE;
			if (entry_value != NONE)
				return entry_value;
			break;
		}
	}

	long element_index = function_26e940(actor_index);

	if (element_index != NONE)
	{
		state->unknown0c = true;
		state->element_index = element_index;

		s_slot_target_list *list = (s_slot_target_list *)element_502424_get(element_index);

		state->position_index = 0;
		if (state->unknown24)
		{
			list->mode = state->unknown1e;
			list->unknown84 = state->unknown28;
			list->unknown88 = state->unknown2c;
			list->unknown80 = false;
		}
		else
		{
			state->unknown20 = NONE;
			list->mode = 5;
			list->unknown84 = NONE;
			list->unknown80 = false;
		}
	}
	return element_index;
}


PRIVATE __forceinline void function_1b0e41(short arg_0, vector2f const *arg_1,
    vector2f const *arg_2, point2f const *arg_3, point2f *arg_4)
{
	real row = (real)(short)((arg_0 + 1) / 2);
	real distance = row * -0.225f;

	arg_4->x = arg_2->i * distance + arg_3->x;
	arg_4->y = arg_2->j * distance + arg_3->y;

	real side = (arg_0 % 2 - 0.5) * (row * 0.9f);

	arg_4->x += side * arg_1->i;
	arg_4->y += side * arg_1->j;
}

/* the position of a formation slot: in a line behind the leader, or (mode 5)
   in two staggered lines */
// @retail 0x1b0e40
bool function_1b0e40(short index, point2f const *origin, vector2f const *forward, point2f *point,
	short mode, vector2f const *left)
{
	switch (mode)
	{
	case 0:
	{
		real distance = index * 0.45f;

		point->x = forward->i * distance + origin->x;
		point->y = forward->j * distance + origin->y;
		break;
	}
	case 5:
		function_1b0e41(index, left, forward, origin, point);
		break;
	}
	return true;
}

// @retail 0x1b0f20
void function_1b0f20(s_slot_target_list *list, vector2f *direction)
{
	if (function_25ab50(list->unknown84) && function_25ab50(list->unknown88))
	{
		point2f *end = point_get(*(long volatile *)&list->unknown88);
		point2f *start = point_get(*(long volatile *)&list->unknown84);

		direction->i = end->x - start->x;
		direction->j = end->y - start->y;
		if (normalize2d((point2f *)direction) != 0.0f)
			return;
	}
	else
	{
		s_actor_view *leader = actor_get(list->entries[0].actor_index);

		*direction = *(vector2f *)&leader->unknown290;
		if (normalize2d_fast(direction) != 0.0f)
			return;
	}
	*direction = *g_468778;
}

// @retail 0x1b1080
s_slot *function_1b1080(short index, s_slot_target_list *list)
{
	s_slot *result = NULL;

	if (index >= 0 && index < list->entry_count)
	{
		s_slot_target_entry *entry = &list->entries[index];

		if (entry->actor_index != NONE && entry->type == 3)
		{
			s_actor_view *actor = actor_get(entry->actor_index);

			if (actor->current >= 0 && actor->current < 4)
				result = &actor->slots[actor->current];
		}
	}
	return result;
}

// @retail 0x1b10e0
short function_1b10e0(short index, s_slot_target_list *list)
{
	short result = NONE;
	s_slot_7e *state = (s_slot_7e *)function_1b1080(index, list);

	if (state)
		result = state->position_index;
	return result;
}

/* chooses the free formation position nearest the actor */
// @retail 0x1b1100
short function_1b1100(long actor_index, s_slot_7e *state, s_slot_target_list *list, point2f const *origin,
	vector2f const *forward, vector2f const *left, point2f *position)
{
	s_actor_view *actor = actor_get(actor_index);
	short result;

	if (state->unknown0c)
	{
		result = 0;
	}
	else
	{
		real distances[10];
		point2f points[10];
		real best_distance = 3.4028235e38f;
		short best_index = NONE;
		short count = list->entry_count;

		for (short i = 1; i < count; i++)
		{
			if (function_1b0e40(i, origin, forward, &points[i], list->mode, left))
			{
				vector2f delta;

				delta.i = actor->position.x - points[i].x;
				delta.j = actor->position.y - points[i].y;

				real distance = magnitude2d(&delta);

				distances[i] = distance;
				if (best_distance > distance)
				{
					best_index = i;
					best_distance = distance;
				}
			}
			else
			{
				distances[i] = 3.4028235e38f;
			}
		}
		bool done = false;

		while (!done && best_index != NONE)
		{
			short other_index = NONE;

			for (short j = 0; j < 10; j++)
			{
				s_slot_7e *other = (s_slot_7e *)function_1b1080(j, list);

				if (other && other->position_index == best_index)
				{
					other_index = j;
					break;
				}
			}
			if (other_index == NONE)
			{
				done = true;
			}
			else
			{
				s_actor_view *other_actor = actor_get(list->entries[other_index].actor_index);

				if (distance2d((point2f *)&other_actor->position, &points[best_index]) + 0.1f > distances[best_index])
				{
					/* the other actor is not nearer to it by more than 0.1: take its position */
					((s_slot_7e *)&other_actor->slots[other_actor->current])->position_index = NONE;
					done = true;
				}
				else
				{
					distances[best_index] = 3.4028235e38f;
					best_index = NONE;
					best_distance = 3.4028235e38f;
					for (short k = 1; k < count; k++)
					{
						if (best_distance > distances[k])
						{
							best_distance = distances[k];
							best_index = k;
						}
					}
				}
			}
		}
		if (best_index >= 0 && best_index < 10 && position)
			*position = points[best_index];
		state->position_index = best_index;
		result = best_index;
	}
	return result;
}

/* whether the actor stands on the right side of its formation position */
// @retail 0x1b1970
bool function_1b1970(long actor_index, short mode, vector2f const *forward,
	vector2f const *left, short index)
{
	bool result = false;

	switch (mode)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		result = true;
		break;
	case 5:
		if (index == 0)
		{
			result = true;
		}
		else
		{
			s_actor_view *actor = actor_get(actor_index);
			point3f *target = &prop_node_state(prop_node_get(actor->prop_index))->position;
			point2f direction;

			direction.x = target->x - actor->position.x;
			direction.y = target->y - actor->position.y;
			if (normalize2d(&direction) != 0.0f)
			{
				real forward_dot = forward->i * direction.x + forward->j * direction.y;

				if (index % 2 == 0)
					result = !(left->i * direction.x + left->j * direction.y > 0.0f) || forward_dot >= 0.9f;
				else
					result = !(left->i * direction.x + left->j * direction.y < 0.0f) || forward_dot >= 0.9f;
			}
		}
		break;
	}
	return result;
}

// score 12 (was 12): the index is read zero-extended and tested as a short, as retail's
// xor/mov ax/cmp ax,0xffff does, and it keeps the list reloaded into EBX. Before that:
// the 0x1b0e40 call sets the origin first (retail last), and in
// the >= 7 block retail reads the index as a word (xor widening) and the mode late
// through the list in ebx; index and mode read in the call 66; short index with the
// mode in the call 66; short index before the mode local 79, after it 79; short index
// with the mode local inside the branch 42; short index and a bool result stored once 95.
// origin/forward setup and native index/mode arguments differ;
// origin pointer 19; origin reference 19; external index reference 79.
// external mode reference 46; no target vector setter 65; native index type 19.
// alias analysis 19; size preference 321; external field pair 78.
// A `short index` gives retail's xor/`cmp ax` read, but then the list is no longer
// reloaded into EBX after the 0x1b0e40 block (66); retail reloads it there and reads
// the mode through EBX. Tried: word index with 0xffff 66, with the mode local inside 42;
// word mode local inside 42; mode assigned inside 42; long index of the word 53; the
// NONE case first 44 (short index) / 26 (current form); the >= 7 block as a
// __forceinline helper 19-70.
// The slot's data block as its own struct read through slot->unknownc, at the
// state line or the top 12 (no change, alone or with the forms below); long index
// with the mode local after it 19; short index with a result local set in both
// arms 48. The 0x1b0e40 setup order is unchanged by an origin local, a const
// origin cast, a mode or index local at the call.
// Round 53: slot data view scoped to the active branch 12; current index with the mode in
// the valid block 26; whole-slot state scoped to the active branch 12; facing address before
// the dot test 18; facing address before the active branch 155; leader declaration split 12;
// basis declarations before the leader 12; state declaration split from its cast 12.
// Opening body restored.
// Round 41 (lane 10): the existing 0x1f86f0 test for the mode-2 check 208 (711 bytes); current
// index with the mode read at the call 54; short index with the mode at the call as a ternary
// 37; that form with `short index` declared at the top 66, with the point declared at the
// top of the active branch 66. With a short index and the mode at the call the >= 7 block is
// retail's except that the list is read from the stack (no EBX reload at the join) and the two
// closing flag stores share `mov al, 1`.
// @retail 0x1b1a90
void __stdcall function_1b1a90(long actor_index, s_slot *slot, s_slot_target_list *list)
{
	s_actor_view *actor = actor_get(actor_index);

	if (list->entries[0].actor_index == NONE)
		return;

	s_slot_7e *state = (s_slot_7e *)slot;
	s_actor_view *leader = actor_get(list->entries[0].actor_index);
	vector2f forward;
	vector2f left;

	forward.i = leader->unknown290.i;
	forward.j = leader->unknown290.j;
	normalize2d_fast(&forward);
	left.i = 0.0f - forward.j;
	left.j = forward.i;
	if (list->unknown80)
	{
		if (!state->unknown0c)
		{
			if (actor->unknown50c)
				function_1f4280(actor_index);
			if (leader->unknown5d0)
			{
				point3f point;

				if (function_1b0e40(state->position_index, (point2f *)&leader->position, &forward, (point2f *)&point,
					list->mode, &left))
				{
					point2f direction;

					direction.x = point.x - actor->position.x;
					direction.y = point.y - actor->position.y;
					normalize2d(&direction);
					if (dot_product2d((vector2f const *)&leader->unknown290, (vector2f const *)&direction) < 0.5f)
					{
						function_1f4280(actor_index);
					}
					else
					{
						vector3f *facing = &actor->unknown458;

						actor->unknown456 = true;
						facing->i = direction.x;
						facing->j = direction.y;
						facing->k = 0.0f;
					}
				}
			}
		}
		if (actor->unknown086 >= 7)
		{
			short mode = list->mode;
			long index = (word)state->position_index;

			if ((short)index != NONE)
				actor->unknown488 = function_1b1970(actor_index, mode, &forward, &left, index);
			else
				actor->unknown488 = false;
			actor->unknown41c = 3;
			actor->unknown420 = 2;
		}
		actor->unknown44a = true;
		actor->unknown449 = true;
	}
	else
	{
		actor->unknown4cc = 0.2f;
		if (actor_get(actor_index)->unknown504 == 2)
		{
			vector3f direction;

			function_1b0f20(list, (vector2f *)&direction);
			direction.k = 0.0f;
			actor->unknown420 = 4;
			actor->unknown424.vector = direction;
			actor->unknown44d = true;
			actor->unknown41c = 3;
		}
		else if (actor->unknown086 >= 7)
		{
			actor->unknown488 = false;
			actor->unknown420 = 2;
			actor->unknown41c = 3;
		}
	}
}

/* invites the actors of the actor's group (or squad) of the same team */
// @retail 0x1b1d90
short __stdcall function_1b1d90(long actor_index, long leader_index, s_slot *slot, long unknown)
{
	long count = 0;
	s_actor_view *actor = actor_get(actor_index);
	s_slot_7e *state = (s_slot_7e *)slot;
	s_ai_actor_iterator group_iterator;

	if (state->unknown20 != NONE)
	{
		ai_actor_iterator_new(state->unknown20, &group_iterator);
		for (s_actor_view *other = (s_actor_view *)ai_actor_iterator_next(&group_iterator); other;
			other = (s_actor_view *)ai_actor_iterator_next(&group_iterator))
		{
			if (other != actor && actor->unknown004 == other->unknown004 &&
				function_26eae0(leader_index, group_iterator.actor_index, 3, 1.0f))
			{
				count++;
			}
		}
	}
	else if (actor->unknown030 != NONE)
	{
		s_squad_actor_iterator squad_walk;

		function_204d30(&squad_walk, actor->unknown030);
		s_actor_view *other;

		while ((other = (s_actor_view *)squad_actors_next(&squad_walk)) != NULL)
		{
			long index = squad_walk.actor_index;
			if (other != actor && actor->unknown004 == other->unknown004 &&
				function_26eae0(leader_index, index, 3, 1.0f))
			{
				count++;
			}
		}
	}
	return (short)count;
}

s_slot_handler_2x g_47df60 =
{
	{
		{
			0x7e, 2, 0, -2, 0,
			function_1b0cd0, function_1b0d10, joint_initiate, joint_leave, NONE, {0},
			0, 0, 0, 0, 0, 0, 1
		},
		(t_slot_proc)joint_update, joint_activate, joint_deactivate
	},
	(t_slot_proc)function_1b0d50, 0, (t_slot_release)function_1b13b0, 0, function_1b1a90, (t_slot_proc4)function_1b1d90,
	3, 10, 1.0f, 0
};

#include "unknown_2605d0.h"
#include "unknown_2626b0.h"
#include "unknown_1f4460.h"
bool function_1f86f0(long arg_0);
long function_1fa7f0(void);
long __stdcall function_26d0e0(point3f const *arg_0, s_type_c3b527 *arg_1, long arg_2);
void function_210be0(s_type_c3b527 const *arg_0, s_type_c3b527 const *arg_1, vector3f *arg_2);

// @retail 0x1b13b0
bool __stdcall function_1b13b0(long arg_0, s_slot *arg_1, long arg_2)
{
    bool local_3 = true;
    {
        s_actor_view *local_0 = actor_get(arg_0);
        s_slot_7e *local_1 = (s_slot_7e *)arg_1;
        s_slot_target_list *local_2 = (s_slot_target_list *)arg_2;
        long local_4 = local_2->entries[0].actor_index;
        if (local_4 == NONE)
            { local_3 = false; goto done; }
        if (local_2->count != 1)
            goto done;
        if (local_2->unknown80)
        {
            if (!local_1->unknown0c || !local_0->unknown040)
                goto done;
            if (function_25ab50(local_2->unknown88))
            {
                s_point_entry *local_5 = point_entry_get(local_2->unknown88);
                if (!function_1f4460(arg_0, &local_5->location, local_5->unknown30, NONE, false))
                    local_3 = false;
                goto done;
            }
            if (!REFERENCE_EQUAL(local_0->unknown418, g_470fa0) && function_1f86f0(arg_0))
                function_262800(arg_0, local_0->unknown418, false);
            s_2605d0_request local_6;
            memset(&local_6, 0, sizeof(local_6));
            local_6.type = 4;
            local_6.unknown015 = true;
            local_6.unknown05b = true;
            *(real *)((byte *)&local_6 + 0x1c) = 50.0f;
            byte *local_7 = ai_scratch_buffer_get();
            long local_8;
            bool local_9;
            s_reference local_10 = function_2605d0(arg_0, &local_6, 0, (long)&local_8, local_7, &local_9);
            local_10 = function_2626b0(arg_0, local_10, local_8, local_7, local_9, true);
            if (REFERENCE_EQUAL(local_10, g_470fa0))
                local_3 = false;
            ai_scratch_buffer_release(local_7);
            goto done;
        }
        else
        {
            s_actor_view *local_11 = actor_get(local_4);
            s_type_c3b527 local_12;
            vector2f local_13;
            long local_14;
            bool local_15 = false;
            if (local_2->unknown84 != NONE && function_25ab50(local_2->unknown84) &&
                function_25ab50(local_2->unknown88))
            {
                s_point_entry *local_16 = point_entry_get(local_2->unknown84);
                s_type_c3b527 *local_17 = (s_type_c3b527 *)point_get(local_2->unknown88);
                vector3f local_18;
                function_210be0(&local_16->location, local_17, &local_18);
                local_13.i = local_18.i;
                local_13.j = local_18.j;
                if (normalize2d((point2f *)&local_13) == 0.0f)
                    local_13 = *g_468778;
                /* as in retail, this path leaves local_12 unfilled */
                local_14 = local_16->unknown30;
                local_15 = true;
            }
            else
            {
                function_26c180(local_4);
                local_12 = local_11->unknown27c.point;
                local_13.i = local_11->unknown290.i;
                local_13.j = local_11->unknown290.j;
                if (normalize2d((point2f *)&local_13) == 0.0f)
                    local_13 = *g_468778;
                local_14 = NONE;
            }
            point3f local_19;
            function_210850(&local_12, &local_19);
            vector2f local_20;
            local_20.i = 0.0f - local_13.j;
            local_20.j = local_13.i;
            if (local_1->unknown0c)
            {
                bool local_21 = true;
                if (local_15)
                {
                    real local_22 = local_11->position.x - local_19.x;
                    real local_23 = local_11->position.y - local_19.y;
                    if (local_23 * local_23 + local_22 * local_22 > 0.2f ||
                        1.0 - dot_product2d(&local_13, (vector2f const *)&local_11->unknown290) > 0.05f)
                    {
                        local_21 = false;
                        if (local_0->unknown040 && !function_1f4460(arg_0, &local_12, local_14, NONE, false))
                            local_3 = false;
                    }
                }
                if (local_21)
                {
                    for (short local_24 = 0; local_24 < 10; ++local_24)
                    {
                        s_slot_target_entry *local_25 = &local_2->entries[local_24];
                        if (local_25->type == 3 && local_25->actor_index != arg_0)
                        {
                            short local_26 = function_1b10e0(local_24, local_2);
                            if (local_26 == NONE)
                            {
                                local_21 = false;
                                break;
                            }
                            s_actor_view *local_27 = actor_get(local_25->actor_index);
                            point2f local_28;
                            if (function_1b0e40(local_26, (point2f *)&local_19, &local_13, &local_28,
                                local_2->mode, &local_20))
                            {
                                real local_29 = local_27->position.x - local_28.x;
                                real local_30 = local_27->position.y - local_28.y;
                                if (local_30 * local_30 + local_29 * local_29 > 0.2f ||
                                    1.0 - dot_product2d(&local_13, (vector2f const *)&local_27->unknown290) > 0.05f)
                                {
                                    local_21 = false;
                                    break;
                                }
                            }
                        }
                    }
                }
                local_2->unknown80 = local_21;
                goto done;
            }
            if (!local_0->unknown040)
                goto done;
            {
                point3f local_31;
                bool local_34 = false;
                if (local_1->position_index == NONE)
                {
                    if (function_1b1100(arg_0, local_1, local_2, (point2f *)&local_19, &local_13,
                        &local_20, (point2f *)&local_31) != NONE)
                        local_34 = true;
                }
                else
                    local_34 = function_1b0e40(local_1->position_index, (point2f *)&local_19, &local_13,
                        (point2f *)&local_31, local_2->mode, &local_20);
                if (!local_34)
                    { local_3 = false; goto done; }
                local_31.z = local_0->position.z;
                s_type_c3b527 local_32;
                long local_33 = function_26d0e0(&local_31, &local_32, function_1fa7f0());
                if (local_33 == NONE)
                    { local_3 = false; goto done; }
                if (!function_1f4460(arg_0, &local_32, local_33, NONE, false))
                    local_3 = false;
                goto done;
            }
        }
    }
done:
    return local_3;
}
