// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1A83B0.CPP: the slot handlers of types 0x46, 0x2f, 0x2d and 0x58
   (0x47d980..0x47da7c), which watch a target moving towards the actor */

#include "unknown_11c920.h"
#include "slot_handler.h"
#include "ai_actor.h"
#include "unknown_2626b0.h"
#include "unknown_0259a0.h"
#include "unknown_11cc90.h"

/* the state of a slot of type 0x58 */
struct s_slot_58
{
	s_slot_header header;
	short ticks;
	byte unknown0e[0x40 - 0xe];
};

short __stdcall function_1a8c30(long actor_index, s_slot *slot);
bool __stdcall function_1a9400(long actor_index, s_slot *slot);
void __stdcall function_1a9760(long actor_index, s_slot *slot);
real function_30bf0(vector3f *v);
real function_11e000(point3f const *b, point3f const *a, vector3f const *d);
real function_11e130(point3f const *a0, vector3f const *a, point3f const *b0, vector3f const *b);

/* where the target (actor +0x360) stands relative to the line the actor
   watches (from +0x370 along +0x37c): whether the actor's own position, the
   point at +0x510 and the segment along +0x5ec come within reach */
#pragma optimize("g", off)
PRIVATE __forceinline real function_1a8fa1(vector3f const *arg_0)
{
    real local_2 = arg_0->k;
    real local_1 = arg_0->j;
    real local_0 = arg_0->i;
    return (real)sqrt(local_0 * local_0 + local_1 * local_1 + local_2 * local_2);
}
#pragma optimize("", on)

// @retail 0x1a8fa0
void function_1a8fa0(long actor_index, real distance, bool *near_point, bool *near_actor, bool *near_segment)
{
	s_ai_object *target = ai_object_get(actor_get(actor_index)->unknown360);
	s_actor_view *actor = actor_get(actor_index);
	bool segment = false;
	bool point;
	bool within;

	if (function_1a8fa1(&target->velocity) > 0.1f)
	{
		vector3f direction;
		direction.i = actor->unknown370.x - actor->position.x;
		direction.j = actor->unknown370.y - actor->position.y;
		direction.k = actor->unknown370.z - actor->position.z;
		function_30bf0(&direction);
		if (target->velocity.k * direction.k + target->velocity.j * direction.j + target->velocity.i * direction.i > 0.1f)
		{
			point = false;
			within = false;
			segment = false;
			goto done;
		}
	}
	{
		point3f *start = &actor->unknown370;
		point3f end;
		end.x = actor->unknown37c.i * 1.5f + start->x;
		end.y = actor->unknown37c.j * 1.5f + start->y;
		end.z = actor->unknown37c.k * 1.5f + start->z;
		vector3f line;
		vector3d_from_points3d(start, &end, &line);
		point3f *position = &actor->position;
		distance += actor->unknown36c;
		within = function_11e000(position, start, &line) < distance * distance;
		if (actor->unknown50c && (near_point || near_segment))
		{
			point3f other;
			function_210850(&actor->unknown510, &other);
			real reach = actor->unknown36c;
			point = function_11e000(&other, start, &line) < reach * reach;
			if (near_segment && actor->unknown5d0 && !within && !point)
			{
				vector3f segment_vector;
				segment_vector.i = actor->unknown5ec.i * 3.0f;
				segment_vector.j = actor->unknown5ec.j * 3.0f;
				segment_vector.k = actor->unknown5ec.k * 3.0f;
				reach = actor->unknown36c;
				if (function_11e130(position, &segment_vector, start, &line) < reach * reach)
					segment = true;
				else
					segment = false;
			}
		}
		else
		{
			point = within;
			segment = false;
		}
	}
done:
	if (near_point)
		*near_point = point;
	if (near_actor)
		*near_actor = within;
	if (near_segment)
		*near_segment = segment;
}

// @retail 0x1a83b0
short __stdcall function_1a83b0(long actor_index, short level, bool active)
{
	short result = function_1a79e0(actor_index, level, active);

	if (result == g_46fbe4)
		result = 0xe;
	return result;
}

// @retail 0x1a92b0
short __stdcall function_1a92b0(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (actor->unknown358 != 0 &&
		prop_node_get(actor->unknown368)->unknown24 >= 1 &&
		!actor->unknown35e)
	{
		real range = actor->unknown39c + 3.0f;
		if (!(actor->unknown398 > range * range) && !actor->unknown225)
		{
			bool near_point;
			bool near_actor;
			bool near_segment;

			result = 1;
			function_1a8fa0(actor_index, 0.0f, &near_point, &near_actor, &near_segment);
			if (near_point || near_segment && !near_actor)
			{
				if (!REFERENCE_EQUAL(actor->unknown418, g_470fa0))
					function_262800(actor_index, actor->unknown418, true);
			}
			if (near_actor || near_segment)
				result = 3;
		}
	}
	return result;
}

// @retail 0x1a93c0
short __stdcall function_1a93c0(long actor_index, s_slot *slot, bool active)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe8;

	if (actor->unknown358 == 0 || actor->unknown35e)
		result = g_46fbe4;
	return result;
}

// @retail 0x1a9400
bool __stdcall function_1a9400(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown040 && actor->unknown358)
	{
		byte *buffer = ai_scratch_buffer_get();
		s_prop_search search;
		long a;
		bool b;
		s_reference reference;

		memset(&search, 0, sizeof(search));
		search.type = 6;
		search.unknown14 = true;
		search.unknown59 = true;
		reference = function_261280(&search, actor_index, NULL, &a, buffer, &b);
		function_2626b0(actor_index, reference, a, buffer, b, true);
		ai_scratch_buffer_release(buffer);
	}
	return true;
}
#pragma inline_depth(0)
// @retail 0x1a94b0
void __stdcall function_1a94b0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = (s_actor_view *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_actor_view));

	if (actor->prop_index != NONE)
	{
		short *view = (short *)function_25d700(actor->prop_index);
		if (view && *view >= 6)
		{
			actor->unknown488 = true;
			actor->unknown41c = 4;
			actor->unknown420 = 2;
			return;
		}
	}
	if (actor->unknown358 > 0)
	{
		actor->unknown41c = 2;
		actor->unknown420 = 5;
	}
	else
	{
		actor->unknown41c = 2;
		actor->unknown420 = 2;
	}
}
#pragma inline_depth(255)

// @retail 0x1a9540
short __stdcall function_1a9540(long actor_index)
{
	short result = 0;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown270 == 3 &&
		actor->unknown358 != 0 &&
		prop_node_get(actor->unknown368)->unknown24 >= 1 &&
		!actor->unknown35e)
	{
		real range = actor->unknown39c + 3.0f;
		if (!(actor->unknown398 > range * range) && !actor->unknown225)
		{
			bool near_point;
			bool near_actor;
			bool near_segment;

			result = 1;
			function_1a8fa0(actor_index, 0.5f, &near_point, &near_actor, &near_segment);
			if (near_point || near_segment && !near_actor)
			{
				if (!REFERENCE_EQUAL(actor->unknown418, g_470fa0))
					function_262800(actor_index, actor->unknown418, true);
			}
			if (near_actor || near_segment)
				result = 3;
		}
	}
	return result;
}

// @retail 0x1a9660
bool __stdcall function_1a9660(long actor_index, s_slot *slot)
{
	s_slot_58 *state = (s_slot_58 *)slot;

	state->ticks = 0;
	long unit_index = actor_get(actor_index)->unknown018;
	if (unit_index != NONE)
		function_20ba60(0x26, unit_index, NONE, NONE, NONE, NULL);
	return true;
}

// @retail 0x1a96b0
short __stdcall function_1a96b0(long actor_index, s_slot *slot, bool active)
{
	s_slot_58 *state = (s_slot_58 *)slot;
	short result = g_46fbe8;

	if (actor_get(actor_index)->unknown358 == 0)
	{
		result = g_46fbe4;
	}
	else
	{
		bool near_actor;
		bool near_segment;
		function_1a8fa0(actor_index, 0.5f, NULL, &near_actor, &near_segment);
		if (!near_actor && !near_segment)
		{
			if ((real)state->ticks * g_510c54->rate > 1.0f)
				result = 0x54;
			else
				state->ticks++;
		}
		else
		{
			state->ticks = 0;
		}
	}
	return result;
}

/* ---- the handlers ---- */

s_slot_child g_46f390[1] =
{
	{2, 0, NONE, {0}, 0.0f, 0, 0},
};

s_slot_handler_1 g_47d980 =
{
	{
		0x46, 1, 0, -2, 0,
		0, function_1b2d90, slot_start_true, slot_proc_nothing, 1, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	function_1a83b0, 1, g_46f390
};

s_slot_handler_0 g_47d9cc =
{
	0x2f, 0, 0, -2, 0, function_1a8c30
};

s_slot_handler_2 g_47d9e0 =
{
	{
		0x2d, 2, 0, -2, 0,
		function_1a92b0, function_1a93c0, 0, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1a9400, 0, function_1a94b0
};

s_slot_handler_2 g_47da30 =
{
	{
		0x58, 2, 0, -2, 0,
		function_1a9540, function_1a96b0, function_1a9660, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)slot_start_true, 0, function_1a9760
};

#include "props.h"
#include "unknown_26c380.h"

struct s_1ae820
{
    byte field_0[0x120];
    real field_120;
};

real normalize2d(point2f *v);
long function_1e4990(long index);
bool function_1f5390(point2f const *heading, long actor_index, real distance,
    short *mode, real vertical_distance, bool *vertical_out, s_path_trace_result *trace);
bool function_1f57a0(long actor_index, long name);
bool function_1f57f0(long actor_index, long animation, long const *target);
bool function_1f50e0(long actor_index);

// @retail 0x1ae820
bool function_1ae820(long arg_0)
{
    bool local_0 = false;
    s_actor_view *local_1 = actor_get(arg_0);
    if (local_1->unknown26c != NONE)
        goto local_10;
    if (function_1f50e0(arg_0))
        goto local_10;
    if (local_1->prop_index == NONE || local_1->unknown264 || local_1->unknown018 == NONE)
        goto local_10;
    {
        s_1ae820 *local_2 = (s_1ae820 *)g_4e3b44[ai_object_get(local_1->unknown018)->definition_index & 0xffff].bytes;
        s_prop_view_fields *local_3 = prop_node_view(prop_node_get(local_1->prop_index));
        if (!local_3 || !(local_2->field_120 > g_45dbd8))
            goto local_10;
        point2f local_5;
        real local_11;
        if (*(byte *)function_1e4990(local_1->unknown054) & 2)
        {
            local_11 = dot3f(&local_3->unknown2c, &local_1->unknown290);
        }
        else
        {
            local_5 = *(point2f const *)&local_3->unknown2c;
            if (!(normalize2d(&local_5) > g_45dbd8))
                goto local_12;
            local_11 = local_1->unknown290.i * local_5.x + local_1->unknown290.j * local_5.y;
        }
        if (!(local_11 > 0.4f))
            goto local_10;
local_12:
        long local_6 = 4;
        local_5 = *(point2f const *)&local_3->unknown2c;
        if (normalize2d(&local_5) == g_45dbd8)
        {
            local_5 = *(point2f const *)&local_1->unknown290;
        }
        bool local_7;
        s_path_trace_result local_8;
        if (function_1f5390(&local_5, arg_0, local_2->field_120, (short *)&local_6, 0.0f, &local_7, &local_8))
        {
            long local_9 = (short)local_6 == 1 ? 0x0b00002e : 0x0a00002d;
            if (function_1f57a0(arg_0, local_9))
                local_0 = function_1f57f0(arg_0, local_9, (long const *)&local_5);
        }
    }
local_10:
    return local_0;
}


#include "unknown_1fb7e0.h"

struct s_1aea20
{
    real field_0;
    real field_4;
    real field_8;
    real field_c;
};

void *function_1e4c70(long actor_index);
real function_1c9ee0(real fraction);

// @retail 0x1aea20
short __stdcall function_1aea20(long arg_0, s_slot *arg_1)
{
    short local_0 = g_46fbe4;
    s_actor_view *local_1 = actor_get(arg_0);
    s_1aea20 *local_2 = (s_1aea20 *)function_1e4c70(arg_0);
    if (local_2)
    {
        long local_3 = local_1->prop_index;
        s_prop_node_view *local_4 = prop_node_get(local_3);
        real local_5 = local_2->field_0;
        if (local_3 != NONE && local_4->unknown27 >= 1 &&
            !local_1->unknown3dc[1] && !local_1->unknown3dc[0] &&
            (local_5 > g_45dbd8 && local_1->unknown3d8 > local_5 ||
                local_2->field_c > local_4->unknown28))
        {
            long local_6 = local_1->times[10];
            if (local_6 == NONE || (real)(g_510c54->game_time - local_6) * g_510c54->rate > local_2->field_4)
            {
                real local_7 = function_259a0(&g_4e7408->unknown0);
                if (function_1c9ee0(local_2->field_8) > local_7 && function_1ae820(arg_0))
                {
                    local_1->unknown3d8 = 0.0f;
                    local_1->times[10] = g_510c54->game_time;
                    local_1->unknown3f1 = true;
                    function_1fb7e0(0x25, arg_0, NULL, NONE, NONE);
                }
            }
        }
    }
    return local_0;
}

s_slot_handler_0 g_47de0c =
{
    0x2e, 0, 0, -2, 0, function_1aea20
};


struct s_1a8a10
{
    long field_0;
    short field_4;
    short field_6;
    real field_8;
};

s_1a8a10 g_4453c0[] =
{
    {0x09000012, 0, 0, 1.5f},
    {0x0a00002d, 0, 0, 0.0f},
    {0x0a000013, 1, 0, 1.5f},
    {0x0b00002e, 1, 0, 0.0f},
    {0x0a000010, 2, 0, 1.5f},
    {0x09000011, 3, 0, 1.5f},
    {NONE, NONE, 0, 0.0f}
};

#pragma inline_depth(0)
// score 17 (was 17): retail subtracts the local at [esi+0x294] inside the sum
// and keeps the two partial products in xmm2 and xmm3; ours subtracts after
// the products and multiplies the terms in the other order.
// Also tried: the second sum computed first 31; the facing read into two scalar
// locals 33; the facing read in place in both sums 93; the four stores in the
// order [3], [1], [2], [0] 21; the pragma replaced by a noinline declaration of
// 0x1f57a0 here 214 (more gets inlined). Retail reads j again after the y store
// of the rotated point, as if that store could alias the actor.
// The actor index comes before the point: retail's 0x1a8c30 loads the point (eax) before the index (ebx), and
// callers set up register arguments last to first.
// Round 24: the rotation as a forceinline helper (the pragma keeps it out of line)
// 164; the facing copy before the rotation 54; the facing copied as one 2D struct
// 118 (the j re-read appears, the copy goes through integer moves); a 2D reference
// to the facing 91; the copy's fields read through a 2D cast 17; a perpendicular
// vector local 38; the first sum's terms swapped 17; the rotated point written
// through a pointer 91.
// The actor index comes before the point: retail's 0x1a8c30 loads the point (eax) before the index (ebx), and
// callers set up register arguments last to first.
// Round 119: the facing copy as an aggregate initializer 17; a perpendicular vector (initializer from the
// actor) for the first sum, with or without the copy, 38/38 (retail's addss order for the first sum, but the
// copy registers move and the j re-read is still folded); the copy's j assigned after the first sum 17 (the
// j re-read appears, the addss order stays ours); the first sum read straight from the actor, copy after it
// 87. Compile-only screen (float block, base 21): perpendicular by name from the actor or the copy, left
// first, no copy, a scalar left i, crossed with term orders, sum order and store order (80 forms) 21-39; the
// negated j read through a cast, a vector pointer, a real pointer or a fresh lookup 21/21/21/35 (the
// lookup re-reads j but keeps our addss order); point components as scalars, a named negated j 21.
// Round 65: the existing actor accessor 150; the copy/actor j reads exchanged 40; the first sum as
// subtraction 86; the shared dot/cross helpers 114; an 8-byte library facing copy 118; the exchanged
// reads with the leading dot term 40; a j reference for every read 29, with the copy's j live 17;
// the rotated point as vector2f 17; a partial vector3f facing copy 17.
// @retail 0x1a8a10
bool function_1a8a10(long arg_0, point2f const *arg_1, short arg_2, real arg_3, real arg_4)
{
    s_actor_view *local_1 = (s_actor_view *)(g_4f55f0->data + (arg_0 & 0xffff) * sizeof(s_actor_view));
    bool local_0 = false;
    bool local_2;
    s_path_trace_result local_3;
    if (local_1->unknown26c == NONE && function_1f5390(arg_1, arg_0, arg_3, &arg_2, arg_4, &local_2, &local_3))
    {
        point2f local_4;
        switch (arg_2)
        {
        case 0:
            local_4.x = 0.0f - arg_1->y;
            local_4.y = arg_1->x;
            break;
        case 1:
            local_4.x = arg_1->y;
            local_4.y = 0.0f - arg_1->x;
            break;
        case 2:
            local_4.x = arg_1->x;
            local_4.y = arg_1->y;
            break;
        case 3:
            local_4.x = arg_1->x;
            local_4.y = arg_1->y;
            break;
        default:
            __assume(0);
        }
        real local_5[4];
        vector2f local_17;
        local_17.i = local_1->unknown290.i;
        local_17.j = local_1->unknown290.j;
        real local_10 = (0.0f - local_1->unknown290.j) * local_4.x + local_17.i * local_4.y;
        real local_11 = local_17.i * local_4.x + local_17.j * local_4.y;
        local_5[0] = local_10;
        local_5[2] = local_11;
        local_5[1] = 0.0f - local_10;
        local_5[3] = 0.0f - local_11;
        real local_6 = -0.5f;
        long local_7 = NONE;
        short local_8 = NONE;
        s_1a8a10 *local_9 = g_4453c0;
        do
        {
            real const &bias = local_9->field_8;
            short const &axis = local_9->field_4;
            long const &identifier = local_9->field_0;
            if (local_5[axis] + bias > local_6 && function_1f57a0(arg_0, identifier))
            {
                local_8 = axis;
                local_7 = identifier;
                local_6 = local_5[local_8] + bias;
            }
            local_9++;
        }
        while (local_9->field_0 != NONE);
        if (local_7 != NONE)
        {
            switch (local_8)
            {
            case 0:
                local_5[0] = local_4.y;
                local_5[1] = 0.0f - local_4.x;
                break;
            case 1:
                local_5[0] = 0.0f - local_4.y;
                local_5[1] = local_4.x;
                break;
            case 2:
                local_5[0] = local_4.x;
                local_5[1] = local_4.y;
                break;
            case 3:
                local_5[0] = local_4.x;
                local_5[1] = local_4.y;
                break;
            }
            local_0 = function_1f57f0(arg_0, local_7, (long const *)local_5);
        }
    }
    return local_0;
}
#pragma inline_depth(255)
