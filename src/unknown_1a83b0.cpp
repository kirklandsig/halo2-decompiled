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
// Round 54 (m3 lane 11, fast): the rotated point assigned y before x in each case 101; the facing copy assigned j
// before i 20, with the first sum's terms swapped 20, with the y-first point 101; the copy's j assigned after the
// first sum (written i*y first) 17 (retail's folded j re-read, but i*y goes through the copy and the j load stays
// after the point store), with the stores in retail's [3], [1], [2], [0] order 19, with the second/first sum's i
// read from the actor 94/27.
// Round 33 (lane 17): any store into the four-value array between the copy and the first sum gives retail's
// j re-read, but the stores then stay where written: the second sum and its stores first 30, the second sum
// first with the stores in [3], [1], [2], [0] order around the first sum 31; the point and the four values in
// one array 99; the second sum written j * y + i * x 17 (same code); the copy ahead of the switch 54.
// Round 163: the point's address taken in the final switch (read through a pointer, or only named there) 17/17, so
// address-taken is not what re-reads j; the point and four values in one record with the values passed on 27 (frame
// moves, j still folded); the y store placed after the facing copy, y carried in a local through the switch 24 (j still
// folded), in the one record 88.
// K148: both leading products with stores [3], [1], [2], [0] 21; dead score store 17;
// successive leading-product accumulators 17; those accumulators with a four-value initializer 18;
// separate score declarations 17; one-use second i copy 17; direct actor second i 38;
// second vector snapshot 146. NOT kept: no equality; original float block restored.
// Round 163: the point's address taken in the final switch (read through a pointer, or only named there) 17/17, so
// address-taken is not what re-reads j; the point and four values in one record with the values passed on 27 (frame
// moves, j still folded); the y store placed after the facing copy, y carried in a local through the switch 24 (j still
// folded), in the one record 88.
// Round 90 (m3 lane 16, fast): both sums with the y product first 17 (same code); the best index, identifier and
// score declared in reverse order 77; the sums written straight into the four-value array 21; the second sum as the
// existing dot_product2d (kept out of line by inline_depth(0)) 77; the copy declared before the array 17 (same code).
// Round 99 (m3 lane 12, fast): the rotated point declared at the function's top 17 and the value array declared
// before the first switch 17 (same code); the identical case 2 and 3 arms merged under one label 139 (540 bytes); the
// first sum as `i * y - j * x` 86 (a repeat of the listed subtraction form); the unrotated cases copying the point as
// an aggregate 128 (548 bytes). Diagnostic: the rotated point's address stored to a global after the last call 137
// (504 bytes: the point then stays in registers and the j re-read still does not appear).
// Round 140 (m3 lane 7, fast): new: the rotated point as two scalar reals 48 (frame 0x38: both go to dead argument
// slots, so retail's point is a two-float aggregate); both sums from the copy (negated copy j) 31; the negated j
// product written point-first 17 (same code); the rotated point written through the existing cross-file point2f_set
// 122 (524 bytes; inline_depth(0) keeps the setter a call), with the final switch through it too 137 (508 bytes).
// Re-measured (listed): y carried through the switch in a scalar and stored after the facing copy 24 (round 163),
// also with the final switch reading the point through a pointer 24 (the scalar takes the 0x50 slot and the point's
// y is never stored; j still folded); the value array and the copy declared before the first switch 17 (rounds 99, 90).
// Round 91 (m3 lane 11, fast): what folds j. Facing fields read in place are not merged at all (both i and j load
// twice, listed form re-measured 91), so retail's one i load with a register copy and two j loads look like i held
// in a local and j read where it is used. Real: i in a scalar local with j read in place in both sums 31 (the j
// re-read appears, but the second j load comes after the first sum and the registers move); that with the second
// sum first 36; the scalar i read ahead of the switch 54. The facing copy ahead of the switch (listed 54) does
// re-read j: the case stores between the copy and the negation stop the reuse, but the loads then sit before the
// jump table. On it: stores in [3], [1], [2], [0] order 54; the second sum first 106. Diagnostic only: the point's
// address stored to a global before the switch keeps the point in registers (508 bytes) and j stays folded.
// Round 115 (m3 lane 9, fast): what folds j, continued. A dead store into the value array, or a goto to a label, between
// the copy and the first sum 17/17 (same code: the dead store is removed first). Unary minus inside the sum 86 (it
// becomes a subtraction), as a named value 17 (same code). A perpendicular 2D local: its j assigned first from the copy
// 17 (same code), its i first 39, its j from the actor 38, no copy at all 93, with the second sum first 92 (reads in
// place never merge: four loads). A second actor pointer computed at the top for the negated read 46 (still folded).
// The negated j named ahead of the copy 17; with the sums stored straight into the array in [0], [2], [1], [3] order
// 17, in [0], [1], [2], [3] order 21, each sum stored where it is computed 17, the negations stored first 20 (all still
// folded: a one-use value is forwarded to its use, past the copy). The copy written inside every case arm, after,
// between or ahead of the point stores 30/31/91 (the loads sink to the join and j is still folded), so of all forms
// only the copy ahead of the switch (54, re-measured with both sums y product first) keeps the two j loads apart: the
// reuse does not cross the jump table. The best score, identifier and index initialised between the copy and the
// sums 23, the score alone there 22, all three ahead of the copy 23 (the score store is emitted where written).
// Diagnostics, not keepable (the opening pragma dropped so helpers inline; 0x1f57a0 then inlines too, 592 bytes): the
// perpendicular and both sums through inline 2D helpers reading the actor give four loads; with the copy as their
// input, one j load. So an inlined call between the copy and the sum does not split the reuse either. The negated j
// named ahead of the switch 48 (it is computed ahead of the jump table), a perpendicular 2D local there 53. Diagnostic,
// reverted: the compiler barrier intrinsic between the copy and the first sum 16 (retail's second j load, but the
// second sum's i product is still computed first and the four stores keep our order), and on it both term orders of
// each sum, either sum first and six store orders (48 forms) 16-31: the term order inside a sum never changes the code.
// Round 115, later (fast): LEAD. Retail's block reads as one 2D frame transform: left = i * y + (-j) * x and
// forward = j * y + i * x, the second sum's i a saved copy (xmm6) and the negated j read in place. The three
// initialisations written identifier, index, score give retail's tail (`or edi` ahead of the score store) on the kept
// body: 15 (not kept: a declaration order, a gain of 2); index, score, identifier 77. With that order and a facing
// copy plus a perpendicular 2D local (its i assigned `0.0f - j` from the actor ahead of the copy's j, its j assigned
// the actor's i), the sums written `copy.i * y + perpendicular.i * x` and `copy.j * y + perpendicular.j * x` and the
// stores in [2], [3], [0], [1] order: 27, every instruction retail's (the second j load, the copy ahead of the first
// sum, both sums in turn, the stores [3], [1], [2], [0]) and only the xmm numbering left: ours j 1, i 2, zero 3,
// perpendicular j 4, x 5, y 6 against retail's zero 1, i 2, j 3, x 4, y 5, copy 6. On it: the four assignments in
// every order, the perpendicular's j from the actor or from the copy (36 forms) 20-39 (the perpendicular's j from the
// copy ahead of its i is folded away: 20, base numbering, base schedule); the two locals as scalars 34-38, both 91-94;
// the six initialisation orders 27-31; ten declaration orders of the two locals, the point and the array 27 each (same
// code); the two locals grouped as the transform's rows, all 24 assignment orders 27-37; the perpendicular's j
// assigned after the first sum from the actor 80 (retail's numbering, but i is loaded again), from the copy 29-34 (the
// copy is then made at the first product and takes xmm1); only the negated j in an aggregate field with i used in both
// sums 15-21 (a one-field aggregate is forwarded early: folded j, base schedule); i and the negated j in one aggregate
// with a scalar j 28-29; a scalar i with the negated j and j in one aggregate 29-35; all three in one three-field
// aggregate 29-35. Diagnostic: the barrier with the perpendicular forms (9) 16-93. Reading: the numbering puts the
// perpendicular's j ahead of x and y whenever it is a named value assigned ahead of the first sum; retail numbers
// that copy last, as the compiler's own copy of a twice-used i, while still evaluating the negated j as a late leaf.
// Round 162 (m3 lane 8, fast with 0x1f5390 0x1f57a0 0x1f57f0 0x1a8c30; control 17): retail's left sum is the 2D dot product
// of the point with the facing's perpendicular (`(0 - j) * x + i * y`) and its forward sum the dot product with the facing,
// the transform src/unknown_0de080.cpp writes with dot_product2d; the inline_depth(0) pragma keeps any helper out of line
// here. Diagnostic (not keepable): the pragma dropped, the three callees declared noinline, both sums through dot_product2d
// with a perpendicular local 214 (592 bytes; the function's own convention changes, arg_0 no longer in EBX; 0x1a8c30 49).
// Round 122 (m3 lane 9, fast; control 17): 0-7 unused locals at the top change nothing here: i in a scalar with j read in
// place in both sums 31/31/29/29/29/29/29/29, the frame-transform form 31 at every phase, the identifier/index/score order
// 15 at every phase. On that order the array stores [0],[1],[2],[3] 16, [2],[3],[0],[1] 18, [3],[1],[2],[0] 19,
// [1],[0],[3],[2] 16; the actor pointer to const 17 (same code).
// Round 164 (m3 lane 7, fast): the final switch writing a separate 2D point for 0x1f57f0 (as 0x1a83b0 passes one),
// declared at the switch or with the rotated point 17/17 (same code, it shares the array's slots); the rotated point as a
// two-element real array 17 or typed as a 2D vector 17 (same code: its y store still does not stop the j reuse); the
// facing copy made before the 0x1f5390 call 170 (564 bytes); a 600 s permuter run, 54 tries, best 17.
// Round 166 (m3 lane 7, fast with 0x1f5390 0x1f57a0 0x1f57f0 0x1a8c30; control 17): the facing copy as a two-real array
// 17; the point's components named x and y ahead of the copy 17; the actor pointer declared `*const` 17 (all same code);
// the final switch's identical cases 2 and 3 under one label 69; the table entry's fields read in place instead of the
// three reference names 28.
// Round 89 (m3 lane 2, fast with 0x1a8c30 0x1f5390 0x1f57a0 0x1f57f0): the forward sum first in retail's
// term orders 31; the copy's j assigned after the first sum with both sums in retail's term orders 17 (the j
// re-read appears; the first sum's i product goes through the i copy), with retail's [3], [1], [2], [0]
// stores 19, with the identifier, index, score initialisation order 15 (the order alone gives that 15); the
// forward sum and its two stores first, then the left sum and its two 31, the same with the left terms in the
// original order 31 (same code), with the copy's j assigned before its i 31 (same code). Reading: computing
// the forward sum first reproduces retail's use of the i copy for the forward product and retail's re-read of
// j (the copy's j register dies in the forward product), but the point's x then takes XMM3, the forward j load
// follows the y store and the forward sum is scheduled first; retail has x in XMM4, both facing loads ahead of
// the y store and the left sum first. All restored.
// Round 174 (m3 lane 8, fast with 0x1a8c30; control 17): the rotation's negations as unary minus 17 (same code); the
// two negated values as unary minus 17; the facing copy as a const vector with an initializer 17; the four values
// stored in index order [0], [1], [2], [3] 18; the negated values read back from the stored [0] and [2] 17. All
// restored.
// Round 93 (m3 lane 2, fast; control 17): the point copied from the input then rotated in place by the switch (cases 2
// and 3 empty) 175 (564 bytes); the left sum reading i and j from the actor 29 (both still one load each, merged with
// the copy); the forward sum's i from the actor 38 (it is loaded again, late); the left sum accumulated in two
// statements 17 (same code); i and j as scalar locals with the negation reading j in place 31 (the scalar j is
// forwarded to its one use, so it loads late into XMM1, as the listed i-scalar form). Re-measured for the folding: the
// copy's j for the negation with the forward j in place 40 (the in-place j in the forward sum is never merged with an
// earlier load; the copy's j stays at the copy and is not folded); the i scalar with j in place twice 31 (retail's
// folded `subss xmm0, [esi+0x294]`, but the forward j load comes after the left sum in XMM1 and the zero takes XMM2).
// Reading: an aggregate copy's fields load at the copy, a one-use scalar's load moves to its use, and an in-place read
// in the left sum merges with the copy while one in the forward sum does not; retail needs the copy's j used once (j*y)
// and the left sum's negated j a separate folded load.
// Round 106 (m3 lane 14, fast with 0x1f5390 0x1f57a0 0x1f57f0 0x1a8c30; control 17): both sums accumulated through
// reference-bound locals (`real &left = ...; left = i * y; left += ...`) 91 (the xmm numbering moves, j still folded);
// y carried through the switch in a scalar and stored to the point after the facing copy, with the point read through
// a pointer in the final switch, 24 (j still folded: the point does not escape); the negated j through a fresh
// actor_get lookup 140 (556 bytes: the lookup is not merged); the negated j named between the copy's i and j, sums in
// retail's term order 17 (same code); the copy and the sums in their own block scope 17 (same code). Re-measured: the
// round-115 frame-transform lead (perpendicular local, identifier/index/score order, [2], [3], [0], [1] stores) 31 here.
// Round 210 (m3 lane 4, fast; control 17): the switch writing the rotated point through a reference-bound local 91, that
// reference also read by both sums 91 (j still folded); a reference bound to the facing copy for the sums 17 (same code);
// each product cast to real inside both sums 17, with retail's term orders (i*y and j*y first) 17 (same code).
// Round 212 (m3 lane 4, fast with 0x1a8c30 0x1f5390 0x1f57a0 0x1f57f0, all matched in each; control 17). Reading: retail's
// forward sum multiplies the copy of i (xmm6) and consumes j's register in place, and its left sum consumes i in place and
// reloads j, as if the forward products came first in the IL while the left sum is scheduled first. Combined leads: the
// copy's j assigned after the left sum, both sums in retail's term orders, with the identifier/index/score initialisation
// order 15 (the j re-read appears and the first difference moves to +0xb9; the left sum still multiplies a copy of i),
// that with the stores [3], [1], [2], [0] 17, with index-order stores 16, with the left sum's i read from the actor 25;
// the forward sum first in retail's term orders with that initialisation order 32; the same with the left sum's i and j
// read from the actor 116 (548 bytes). All restored.
// Round 185 (m3 lane 8, fast with 0x1a8c30 0x1f5390 0x1f57a0 0x1f57f0; control 17): the forward sum first in retail's
// term orders (`j * y + i * x`, then `i * y + (0 - j) * x`) with the stores [3], [1], [2], [0] 31 (the j re-read and
// the copy of i in the forward product appear, but the forward sum is scheduled first); the same with the left sum's i
// read from the actor 115 (548 bytes); with the identifier, index, score initialisation order 32; with the facing copied
// into two scalar locals 36; the forward sum first with the left sum in its current term order and stores 31.
// M3 lane 1 r121 (fast with the file's functions and 0x1f5390/0x1f57a0/0x1f57f0/0x1a8c30, all MATCH): the forward sum
// first in retail's term orders (`j * y + i * x` from the copy, then `i * y + (0 - actor j) * x`), stores in index
// order 31 (first difference +0x75: the j reload and the copy of i appear, but the forward sum is scheduled first and x
// takes XMM3); that with the copy's j assigned before its i 31; with unary minus for the three negations 31; with the
// negations read back from the stored [0] and [2] 17 (identical code to main's: the left-first schedule returns); that
// with the stores in [2], [0], [3], [1] order 31.
// M3 lane 10 r122 (fast with 0x1f5390/0x1f57a0/0x1f57f0/0x1a8c30, all MATCH each time): the facing copied by the planar
// aggregate cast (`vector2f local_17 = *(vector2f const *)&local_1->unknown290;`, the sibling functions' idiom) 118 (548
// bytes: the copy goes through two integer registers and the stack, though the negated j is then its own load); the facing
// copy made after the 0x1f5390 call, ahead of the first switch, 54 (its loads stay ahead of the jump table); the copy read
// through a `vector3f const *` to the facing 17, the negated j read through that pointer 17 (both same code); no copy, all
// four facing reads in place 91. Re-measured: the copy's j after the left sum with retail's term orders and the identifier,
// index, score order 15 (the j reload appears, but the copy's j load then waits for the left sum and the left product
// multiplies a copy of i: retail multiplies i in place and copies it for the forward product).
// M3 lane 8 r193 (compile-only screen of the float block, which reproduces this body's block exactly; then fast with
// 0x1f5390/0x1f57a0/0x1f57f0/0x1a8c30, all MATCH): 0-40 initialised dead locals ahead of the block leave it unchanged (no
// symbol-count tie); both sums in retail's term orders re-measured (same code). The forward sum first with both sums read
// from the copy (negated copy j) 89; the forward sum first with the copy as a const 2D vector with a brace initializer 31
// (screen: the forward j still loads at its use and its result moves out of XMM0); the left sum's i read from the actor
// with the forward first (screen: i loaded twice). On round 115's frame-transform lead (copy plus perpendicular, sums
// `copy.i * y + perpendicular.i * x` and `copy.j * y + perpendicular.j * x`, stores [2], [3], [0], [1], identifier/index/score
// order; screen: every instruction retail's but the xmm numbering): all 24 orders of the four component assignments give
// the zero XMM3 (or XMM2) and x/y XMM5/XMM6 (retail zero XMM1, x XMM4, y XMM5, copy XMM6); 0-16 dead locals inside the
// block or 0-14 ahead of it only permute i/j/copy among XMM1/XMM2/XMM4.
// M3 lane 10 r124 (fast with 0x1f5390/0x1f57a0/0x1f57f0/0x1a8c30, all MATCH each time; control 17): the four products
// named where they are computed (both sums from them) in the orders forward-x, forward-y, left-x, left-y / left-x, left-y,
// forward-x, forward-y / forward-x, forward-y, left-y, left-x / forward-y, forward-x, left-x, left-y 17 each (same code: the
// IL order is not the source's); the facing copied whole as a 3D vector 142 (564 bytes), that with the forward sum's terms
// swapped 142. Diagnostic (an invented layout, not keepable): the rotated point as the first two reals of a six-real value
// array (so its stores go to an address-taken local) 99 (frame 0x28; j is still loaded once, so the y store's aliasing is
// not what reloads j in retail).
// M3 lane 12 r151 (fast with 0x1f5390/0x1f57a0/0x1f57f0/0x1a8c30, all MATCH; control 17): the forward sum's `i * x` named
// ahead of the left sum with the copy's j assigned after the left sum 36 (the j re-read appears, the zero takes XMM2 even
// in the switch arms), with the copy's j before it 38; round 115's frame-transform lead re-measured 31, with the
// perpendicular's j taken from the copy's i 32; the four values as an array initializer after the sums
// (`{left, 0 - left, forward, 0 - forward}`) 18, with the identifier, index, score initialisation order 16 (a gain of one
// from an initializer and a declaration order: not kept).
// M3 lane 13 r122 (fast with 0x1f5390/0x1f57a0/0x1f57f0/0x1a8c30, all MATCH unless noted; control 17): the opening
// inline-depth pragma dropped 214 (592 bytes: 0x1f57a0, whose only caller this is, is expanded, and 0x1a8c30 goes to 49),
// with the 0x1f57a0 call wrapped in an inline helper 214, with the score test and the call in one inline helper 214
// (the call nested in a helper is still expanded, so the pragma stays); the forward sum first in retail's term orders
// (copy's j for `j * y`, the actor's j for the negation) re-measured 31, and on it 1-7 dead initialised locals at the top
// and 1-12 inside the block ahead of the array 31 each (flat: x keeps XMM3 and the copy's one-use j still loads at its
// use, so the xmm ranks there are no symbol-count state); i in a scalar with j read in place in both sums, forward first
// 36 (left first re-measured 31).
// M3 lane 6 r208 (fast with 0x1a8c30/0x1f5390/0x1f57a0/0x1f57f0, all MATCH; control 17): both sums written point-first 17
// (same code); retail's term orders (`i * y + (0 - actor j) * x`, `j * y + i * x`) with [3], [1], [2], [0] stores 21, with
// index-order stores 18 (ours then adds into the (0 - j) * x product where retail adds into i * y, so retail's negated j is a
// value computed ahead, as in round 115's perpendicular); round 115's frame-transform lead rebuilt 31 (every instruction
// retail's; xmm numbering i 1, j 2, zero 3, copy 4, x 5, y 6 against retail's zero 1, i 2, j 3, x 4, y 5, copy 6; main's body
// has retail's numbering), without the initialisation reorder 33.
// M3 lane 16 r151 (fast with 0x1a8c30, MATCH): the left sum as `(0 - copy.j) * x + actor.i * y` with the forward sum
// from the copy 38; that with the forward sum's j read from the actor 47; the left sum entirely from the copy with the
// forward sum's i from the actor 30; the first form with the left terms written i * y first 38; the table pointer (a
// constant initialiser) declared at the function's top 17 (same code).
// M3 lane 12 r157 (fast with 0x1a8c30, MATCH; control 17): both sums read through `real const &` bindings for their four
// stores 17; the sums assigned through references bound to the array elements [0] and [2] 17; the two sums as the fields
// of one 2D vector local 17; both negations named and the stores written [3], [1], [2], [0] 21; the sums reading the
// rotated point through a `point2f const &` 17. Re-measured: the identifier, index, score initialisation order 15.
// M3 lane 15 r118 (fast with 0x1a8c30, MATCH; control 17): the facing copy declared ahead of the first switch and
// assigned after it 17; the loop's score test written `score < value + bias` 17; the value array declared just before its
// stores 17; both sums declared ahead of the first switch and assigned in place 17; the result flag declared after the
// path-trace locals 17 (all the same code).
// M3 lane 1 r137 (H2M_FAST; control 17): a grid of both sums' term orders x which sum comes first x the negated j from
// the actor or the copy: the term orders never change the code; left first 17 (actor j) / 31 (copy j); forward first 31
// (actor j, +0x75: the j reload appears but x takes XMM3 and the forward is scheduled first) / 89 (copy j). A second
// aggregate copy of the facing for the forward sum 146 (564 bytes: the copy goes through the stack), with the negation
// from the first copy 147; the left sum's i through a `vector2f const &` to the copy, through a `real const &` to its i,
// and the negated j through a second const actor pointer 17 each (same code).
// M3 lane 6 r211 (fast with 0x1a8c30, MATCH; control 17; a compile-only screen of the float block reproduces it exactly,
// 12 differing xmm lines against retail): the value array zero-initialised at its declaration 110 (548 bytes); the facing
// copy as a 3D vector with only i and j assigned 17 (same code); the final switch's cases in all 24 orders and each case's
// two stores swapped (30 forms, screen) at best the same float block (cases 2, 0, 1, 3: 17), so later mentions of the
// point do not set the sums' operand sides here; the second sum first with the copy assigned i, j or j, i and the four
// stores in all 24 orders (48 forms, screen) 26-29 xmm lines against 12 (stores [2], [0], [1], [3]: 31); re-measured: the
// copy's j assigned between the sums 17 (round 119), retail's term orders 17 (same code).
// M3 lane 11 r140 (H2M_FAST with 0x1a8c30, MATCH; control 17). Reading: retail's forward product uses the copy of i and the
// left product consumes i, as if the forward i * x came first in the IL, so both sums were tried column by column (both x
// products, then both y products added): `left = (0 - actor j) * x; forward = i * x; left += i * y; forward += j * y;` 38;
// with the copy's j for the negation 30; with the identifier, index, score initialisation order 36 (the numbering moves
// and j stays folded). The actor pointer bound by `s_actor_view *const &` for the negated read 17, for the copy 17 (same
// code); the negated j named and read through a `real const &` 39.
// M3 lane 12 r161 (fast with 0x1a8c30, MATCH; control 17): the negated j read through the planar view of the facing 17
// (same code); the copy made through a `vector2f const *` to the facing with the negation read through it, as an
// aggregate copy 118 (548 bytes), field by field 17 (same code); the best score initialised between the copy and the sums
// 22 (j still folded, its store moves up), with the forward sum first 32; the forward sum first reading j in place
// (`actor j * y + copy i * x`, then `copy i * y + (0 - actor j) * x`) 34: the j re-read and the i copy in the forward
// product appear, but the forward sum is scheduled first and the xmm numbering is j 1, zero 2, i 3 with y copied from
// xmm0; on it no copy at all 34, the left terms swapped 34, stores [3], [1], [2], [0] 34, the identifier, index, score
// initialisation order 32.
// M3 lane 7 r204 (h2m fast with 0x1a8c30, MATCH): a perpendicular held in two scalar locals (`0 - actor j`, `actor i`)
// with the sums `copy.i * y + perp_i * x` / `copy.j * y + perp_j * x` and stores [2], [3], [0], [1] 38; the 2D-vector
// perpendicular set from the actor ahead of the copy 34, formed from the copy after it 37; a named negated j for the left
// sum, stores [2], [3], [0], [1] 20; the named negated j read ahead of the copy, left sum first 17 (same code). Compile-only
// screen of the float block (the file without /GL, exact here; differing block lines, ours 13): 1152 crossings of which sum
// comes first, both sums' term orders, the negated j from the actor or the copy, the copy assigned i, j / j, i / j between
// the sums and all 24 store orders, best 13 (ours); 132 perpendicular forms, best 13; the 2D perpendicular gives retail's
// instruction order with the registers renumbered (i 1, j 2, zero 3, copy 4, x 5, y 6 against retail's zero 1, i 2, j 3,
// x 4, y 5, copy 6) 18.
// M3 lane 12 r164 (fast with 0x1a8c30/0x1ae820/0x1f5390/0x1f57a0/0x1f57f0, all MATCH; control 17). The inline-depth pragma:
// dropped, with the score test and the call as nested ifs 214 (592 B, the callee expanded), the outer test split too 214,
// else arms 214, a `continue` form 214. One-object link-time threshold search (the build's threshold is 81): the call
// costs 41 as written, 48 / 64 / 96 with one / two / three extra written levels; re-spelled real branches stay 41. The
// candidate table lies in retail's read-only data: declared const and walked by `for (entry = table; entry->id != NONE;
// entry++)` 17 (same code, the entry test folds), and that loop counts as one level (48). Diagnostic (always-true tests,
// not keepable): two extra levels anywhere around the call keep it out of line without the pragma, 17. On the const table:
// fields read in place 28, an index loop 162 (552 B), a short index 205 (588 B), an entry pointer from the index 148/147;
// a non-const table walked by `for` 112 (548 B). Compile-only screen of the float block (main's block reproduced; block
// lines against retail's, base 10): the facing copy updated in place with a saved i (30 forms) 15-23; a four-value
// setter helper in all 24 parameter orders 10 (same block); 2-D dot and perpendicular helpers, 224 forms, 15 at best;
// a 2-D setter for the copy 10; one-statement transform helpers 17-22. Round 115's frame-transform lead re-screened
// (19); its perpendicular's j taken from the copy's i gives retail's head numbering but schedules the forward sum first
// (13). All restored.
// M3 lane 12 r168 (h2m fast with 0x1a8c30, MATCH; control 17; compile-only float screen first, base 10 block lines): both
// sums stored straight into the array ([0], [2]) with [1] and [3] negating the stored elements 21 (screen 11); the left sum
// and its negation stored before the forward sum is computed 21 (screen 11); [0], [1], [2], [3] stored straight in that
// order 25 (screen 12); the facing's planar part copied with memcpy 118 (548 B: through general registers, though the
// screen shows the negated j then read from memory and the left sum computed first); `__assume(local_1 != 0)` after the
// copy 17 (same code; screen 10 also before the copy, at the switch's top, and for the quadrant's range). Screen only: the
// rotated point written through a pointer local 22, through a forceinline quadrant helper 61, the copy through a pointer
// local 10, the negated j read as `n[1]` or by raw offset 10, the copy from `n[0]`/`n[1]` 10 (all still one j load).
// M3 lane 7 r206 (h2m fast with 0x1a8c30/0x1f5390/0x1f57a0/0x1f57f0, all MATCH; control 17): a planar `vector2f const *`
// view of the facing for the copy and the negated j 17 (same code), the copy as an aggregate copy through it 118 (548 B);
// the copy brace-initialised from the actor's i and j 17, with both sums in retail's term orders 17; the point read into
// scalar x and y locals after the switch 17 (all the 17s j read once).
// M3 lane 13 r138 (h2m fast with 0x1a8c30, MATCH; control 17): the forward sum computed and stored ([2], [3]) before the
// left sum is computed and stored ([0], [1]) 30: the address-taken array's stores split the two actor j reads, so the
// negated j is read again from memory as in retail, but the forward sum is then scheduled first and its store stays ahead
// of the reload (retail's second j load sits ahead of every store, so its split is not a store); the left sum computed and
// stored ([1], [0]) before the forward sum ([3], [2]) 21 (j shared again); the copy as a `vector3f` aggregate copy of the
// whole facing 142 (564 B: the copy goes through the stack by integer moves, and the negated j is then a separate read);
// the copy read through the union's `n[0]`, `n[1]` 17 (same code); the first switch's cases 2 and 3 under one label 139
// (540 B). All restored.
// M3 lane 4 r245 (h2m fast with 0x1a8c30, MATCH): i in a scalar local with j read in place in both sums 31, with 0-2
// redundant pairs over the five float sites 31/36 only. Reading on it: the negated j is folded from memory as retail's,
// but forward's j load sits after the first sum, where retail loads j right after i, ahead of the point's y store. The
// copy's j assigned between the sums 17 (the folded reload is retail's, forward's j load stays late and the first sum adds
// into the negated product with a copy of i); that with the stores [3], [1], [2], [0] 19; each sum stored and negated right
// after it 30; the first sum read from the actor ahead of the copy 87; the point's y carried through the switch in a
// scalar and stored to the point after the copy 24 (the scalar takes the 0x50 slot, j folded). All restored.
// M3 lane 7 r212 (h2m fast with 0x1a8c30, MATCH; control 17). Retail's join (0x1a8abf, case 0's jump target) begins
// with the copy's i and j loads, then the point's y store, then the negated j read from memory again, so a store sits
// between the two j reads there. The facing copy made ahead of the first switch 54 (the arms' point stores then split
// the reads and the negated j is reloaded from memory as in retail, but the copy's loads move ahead of the dispatch into
// XMM4/XMM5); that with the arms storing y before x 129 (548 B); the arms storing y before x alone 101; re-measured lane
// 4 r245's y carried in a scalar and stored after the copy 24 (the scalar takes the 0x50 home, the point's y store is
// forwarded away and j is shared). Diagnostics (duplicated statements, not keepable): the copy at the end of each arm 30
// (merged into the join ahead of the y store, j still shared); the copy in each arm between the x and y stores 31. The
// copy made at the function's top, ahead of the path-trace call, 170 (564 B: the copy is kept on the stack).
// M3 lane 14 r143 (h2m fast with 0x1a8c30, MATCH; control 17): the facing copy assigned j first 20; the rotated point
// declared at function scope 17, the facing copy declared ahead of the score array 17, the score array declared ahead of
// the rotated point 17 (the three the same code); the negated j named ahead of the first switch 48 (its load stays ahead
// of the dispatch). All restored.
// M3 lane 3 r296 (H2M_FAST with 0x1a8c30, MATCH; control 17): on the copy's j assigned between the sums (17, re-measured:
// the folded j reload is retail's, forward's j load stays late): the identifier, index, score initialisation order 15;
// stores [3], [1], [2], [0] 19, with the initialisation order 17; forward terms j*y first, left terms i*y first, both 17
// each (same code). The left sum reading the actor's i (the copy's i assigned first, its j between the sums) 27, with
// left terms i*y first 27, with the copy's j assigned before the sums 29, with stores [3], [1], [2], [0] 29, with the
// initialisation order 25. All restored.
// M3 lane 13 r144 (h2m fast with 0x1a8c30, MATCH; control 17): the rotated point and the value array as one local
// aggregate (retail's homes 0x10..0x24 are contiguous; a test of whether an escaped point store splits the j reads) 27
// (float block unchanged, j still shared, homes move); that with y carried in a scalar through the switch and stored to
// the point after the facing copy 88 (the y store sinks); the forward sum's i read from the actor in place 38 (left is
// then computed first, but i loads twice and j once); the value array by an initializer list after the sums 18 (same
// float block); left from the copy with the negated copy j, forward read in place, 118 (548 B). All restored.
// M3 lane 1 r156 (h2m fast with 0x1a8c30, MATCH; control 17): statements placed between the facing copy and the left
// sum to split the two j reads: the table pointer 17, the identifier and index initialisations 17, the identifier and
// the table pointer with the index and score after the sums 15 (the listed identifier, index, score order: tail only,
// float block unchanged, j still shared); the copy read through a planar `vector2f const *` view with the negation from
// the actor 17; the negated j named right after the copy 17 (all the 17s the same code).
// M3 lane 1 r158 (h2m fast with 0x1a8c30, MATCH; control 17): the facing copy as a two-element real array 17; both
// sums `real const` 17; every product written point first (`x * (0 - j) + y * i`, `x * i + y * j`) 17 (same code);
// on the copy's j assigned between the sums, products point first 17 and only the forward's point first 17 (both the
// j reload as retail, but the left product still multiplies a copy of i while retail multiplies i in place and copies
// it for the forward product, and forward's j load stays after the left sum).
// M3 lane 8 r226 (h2m fast with 0x1a8c30/0x1f5390/0x1f57a0/0x1f57f0, all MATCH each time; control 17; reference-bound
// locals, which kept a two-step sum apart in 0x211cc0): the facing copy stored through a `vector2f &` bound to it 17
// (same code); the negated j built through a reference (`minus_j = 0.0f; minus_j -= actor j;`) with the left sum
// written either way round 17/17 (same code: the binding is folded and j is still read once); the left sum
// accumulated through a reference (`left = i * y; left += (0 - actor j) * x;`) 84 (first difference +0x75, the xmm
// numbering moves); the left sum built in three steps through it (`left = 0 - actor j; left *= x; left += i * y;`) 37
// (the zero then stays out of XMM1 and the [1] store moves up). j is read once in all five. All restored.
// M3 lane 1 r161 (h2m fast with 0x1a8c30, MATCH each time; control 17): the facing copy declared at the function's top
// 17; the value array declared at the function's top 17; the result flag declared after the path-trace locals 17; the
// table pointer initialised ahead of the sums 17; the facing copy typed as the facing's own 3D vector (only i and j set)
// 17 (all the same code: j read once). Body unchanged.
// M3 lane 15 r140 (h2m fast with 0x1a8c30/0x1f5390/0x1f57a0/0x1f57f0, all MATCH; control 17): the left sum read in
// place from the actor (`i * y + (0 - j) * x`) with the forward sum from the copy 29; the left sum in two statements
// (`i * y`, then `+= (0 - j) * x`) 17, with the negated term first 17, both sums so 17 (the split is folded: same code);
// the forward sum in two statements (`i * x`, then `+= j * y`) 38. Body unchanged.
// M3 lane 13 r152 (h2m fast with 0x1a8c30, MATCH unless noted; control 17): forced allocator decisions (picks, priorities,
// cuts, reload flags, costs and the call alias sets, 77 of them) never give the second j load, so retail's reload is decided
// before registers are chosen. Forms: the first switch without its `default: __assume(0);` 163 (560 B: a range check comes
// back); its case 3 also the default 141 (540 B); the input point copied into a local and the first switch reading it 129
// (548 B); the first switch's cases written 3, 2, 1, 0 79; `return` of the 0x1f57f0 result in place and `return false` at
// the end with no result local 216 (584 B; 0x1a8c30 49).
// M3 lane 1 r166 (h2m fast with 0x1a8c30, MATCH; control 17): round 115's frame-transform lead rebuilt (31; every
// instruction retail's, xmm numbering left). Allocator reading of its xmm class: ours colours the temporary (120) to
// XMM0, i (100) and j (100, ties above the zero's) to XMM1/XMM2, the zero (100) to XMM3, the i copy (80) to XMM4, x (76)
// to XMM5 and y (a later piece) to XMM6; retail's numbering needs the zero ahead of i and j and both x and y ahead of the
// copy, three counts at once (a single forced decision gives 23 at best, 60 tried). On the lead: stores in index order
// 32; the negations read back from the stored [0]/[2] 34; the perpendicular's i as unary minus 31; x and y named from
// the point for both sums 31; the perpendicular's j from the copy's i 32. Body unchanged.
// M3 lane 1 r167 (h2m fast with 0x1a8c30, MATCH; control 17, the lead 31). Allocator reading: main's float block has 8
// referenced register candidates and the lead 10 (its perpendicular j is its own copy candidate and its negation a
// separate temporary); the per-block weight scales with that count, and at 10 the i and j webs (10 per candidate) beat
// the zero (6 per candidate plus 40 from the switch arms) and the i copy (8) beats x (4 plus 36), which is the lead's
// numbering; at 8 the order is retail's. So retail's block has main's candidate count with the lead's instructions. On
// the lead: both sums as plain dot products (`perpendicular.i * x + perpendicular.j * y`, `copy.i * x + copy.j * y`)
// 29; that with the perpendicular's j from the copy's i 32; the perpendicular declared and set ahead of the copy 31;
// stores in index order 33; both vectors brace-initialised 32. Body unchanged.
// M3 lane 7 r229 (fast with 0x1a8c30, MATCH each time; control 17; round 115's lead re-measured 31). Allocator
// reading of the lead: its join block references 10 float candidates (the two named sums are copies of the i and j webs
// they accumulate in, and the perpendicular's j is a copy of i), main's 8; with 8 the numbering is retail's. The
// negated j's load stays a folded memory operand only while the perpendicular is a two-member local set ahead of the
// copy's j (a one-member one is forwarded past the copy and the load merges). Forms: the lead without the
// perpendicular's j, the forward sum on the copy's i, 18 (8 candidates and retail's xmm numbering, but j merged and the
// forward sum computed first); that with the copy assigned j first 21; the perpendicular's j from the copy's i 32; the
// left sum on both perpendicular members with the forward sum on the copy 29, that with the copy set ahead of the
// perpendicular 32.
// M3 lane 8 r239 (h2m fast with 0x1a8c30, MATCH each time; control 17): the outer test split into two nested ifs 17;
// both sums with `(real)(...)` around each product, main's term orders 17 and retail's (`i * y` first, `j * y` first) 17;
// `(real)(0.0f - j)` around the negation alone, main's term orders 17 and retail's 17 (all five the same code: the casts
// are no-ops under SSE and do not make the negation a separate leaf, and j is still read once).
// M3 lane 11 r170 (h2m fast with 0x1a8c30, MATCH; control 17): the rotated y carried in a scalar through the switch
// and stored to the point after the facing copy, with the final switch reading the point through a `point2f const *`
// 24 (the point's y store is forwarded into the scalar's 0x50 home, j still folded); the rotated x carried the same way
// and stored after the copy 24; the copy's j assigned between the sums with the rotated y carried in a scalar and
// stored after the copy's i 22 (j then loads twice, the folded reload first, but the copy's j load stays after the left
// sum where retail loads it right after i); the negated j read through a `real const &` bound to the facing's j ahead
// of the outer test 17 (same code); the negated j through a fresh in-place actor lookup 157 (548 B, the frame grows).
// M3 lane 15 r150 (h2m fast with 0x1a8c30, MATCH each run; control 17), on the hypothesis that retail's IL has the
// forward `i * x` product ahead of the left sum and the copy's j load after the negated read: the forward product named
// first from the copy's i, the copy's j assigned after the left sum, forward = `j * y + product` 36 (first +0x6e); that
// with the stores [3], [1], [2], [0] 36; the sums accumulated in the value array (`[2] = i * x`, `[0] = left`, copy's j
// assigned, `[2] += j * y`, negations read back) 28 (the folded j reload and the left product on the original i appear,
// but the copy's j loads after the left sum, x takes XMM3 and the left sum adds into the negated product); that with the
// copy's j assigned first 30; both sums accumulated in the array in two steps each 92.
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
