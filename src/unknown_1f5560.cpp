// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1F5560.CPP: small accessors of an actor's movement state */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1e3920.h"
#include "object_tag_definitions.h"
#include "slot_handler.h"
#include "unknown_2626b0.h"
#include "unit_requests.h"
#include "props.h"
#include "unknown_0259d0.h"
#include "unknown_11cc90.h"
#include "unknown_124f90.h"
#include <string.h>
#include <math.h>
#include <float.h>
#include "unknown_26c380.h"
#include "vector2f_products.h"

struct s_slot_entry_list;
struct s_collision_bsp_test_vector_result
{
    real t;
    plane3f const *plane;
    long surface_reference[3];
    byte surface_flags[2];
    short surface_index;
    long leaf_count;
    long leaves[0x100];
};
extern s_slot_entry_list *g_4e0340;
extern vector3f *g_4687bc;
bool function_1de630(dword flags, s_slot_entry_list *bsp, s_collision_bsp_test_vector_result *result,
    real fraction, long count, byte const *mask, point3f const *origin, vector3f const *direction);

// @retail 0x1f5110
bool function_1f5110(long actor_index, point2f const *direction, real distance,
    real vertical_distance, bool *vertical_out, s_path_trace_result *trace)
{
    bool result = false;
    bool vertical = false;
    s_actor_moving *actor = actor_moving_get(actor_index);
    if (!actor->unknown229)
    {
        byte *structure = (byte *)g_4e0348;
        s_pathfinding_data *pathfinding = NULL;
        if (*(long *)(structure + 0xc4) > 0)
            pathfinding = *(s_pathfinding_data **)(structure + 0xc8);
        function_26c180(actor_index);
        vector3f horizontal = { direction->x, direction->y, 0.0f };
        if (pathfinding)
        {
            vector3f local;
            function_210770(actor->location.cluster_index, &horizontal, &local);
            long sector = actor->unknown28c;
            if (!function_26c590(pathfinding, (point3f *)&actor->location, sector,
                NONE, &local, distance, NULL, trace) && ((s_sector_trace_result *)trace)->sector_index != NONE)
            {
                result = true;
                goto done;
            }
        }
        if (vertical_distance > 0.0f)
        {
            point3f origin;
            origin.x = 0.5f * (actor->position.x + *(real volatile *)((byte *)actor + 0x22c));
            origin.y = 0.5f * (actor->position.y + *(real volatile *)((byte *)actor + 0x230));
            origin.z = 0.5f * (actor->position.z + *(real volatile *)((byte *)actor + 0x234));
            horizontal.i = direction->x * distance;
            horizontal.j = direction->y * distance;
            horizontal.k = 0.0f;
            s_collision_bsp_test_vector_result hit;
            s_slot_entry_list *bsp = g_4e0340;
            if (!function_1de630(0x1808c2d, bsp, &hit, FLT_MAX, 0, NULL, &origin, &horizontal))
            {
                result = true;
                vertical = true;
                if (vertical_distance < FLT_MAX)
                {
                    point3f end = { *(real volatile *)&horizontal.i + origin.x, *(real volatile *)&horizontal.j + origin.y, horizontal.k + origin.z };
                    vector3f down = { g_4687bc->i * vertical_distance, g_4687bc->j * vertical_distance, g_4687bc->k * vertical_distance };
                    if (!function_1de630(0x1808c2d, bsp, &hit, FLT_MAX, 0, NULL, &end, &down))
                        result = false;
                }
            }
        }
    }
done:
    if (vertical_out)
        *vertical_out = vertical;
    return result;
}

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x1f5390
bool function_1f5390(point2f const *heading, long actor_index, real distance,
    short *mode, real vertical_distance, bool *vertical_out, s_path_trace_result *trace)
{
    bool local_0 = false;
    short attempts = 1;
    short selected = *mode;
    point2f direction;
    switch (selected)
    {
    case 0:
        direction.x = 0.0f - heading->y;
        direction.y = heading->x;
        break;
    case 1:
        direction.x = heading->y;
        direction.y = 0.0f - heading->x;
        break;
    case 2:
        direction.x = heading->x;
        direction.y = heading->y;
        break;
    case 3:
        direction.x = 0.0f - heading->x;
        direction.y = 0.0f - heading->y;
        break;
    case 4:
        {
            s_actor_moving *actor = actor_moving_get(actor_index);
            attempts = 2;
            if (*(byte *)((byte *)actor + 0x5d0))
            {
                real forward = heading->x;
                _ReadWriteBarrier();
                real side_direction = 0.0f - heading->y;
                real side = *(real *)((byte *)actor + 0x5f0) * forward;
                side += *(real *)((byte *)actor + 0x5ec) * side_direction;
                if (side > 0.5f)
                {
                    selected = 0;
                    direction.x = 0.0f - heading->y;
                    direction.y = heading->x;
                }
                else if (side < -0.5f)
                {
                    selected = 1;
                    direction.x = heading->y;
                    direction.y = 0.0f - heading->x;
                }
                else
                    attempts = 0;
            }
            else
            {
                dword value;
                dword *seed = &g_4e7408->unknown0;
                value = *seed * 0x19660d + 0x3c6ef35f;
                *seed = value;
                if ((value & 0x80000000) > 0)
                {
                    selected = 0;
                    direction.x = 0.0f - heading->y;
                    direction.y = heading->x;
                }
                else
                {
                    selected = 1;
                    direction.x = heading->y;
                    direction.y = 0.0f - heading->x;
                }
            }
        }
        break;
    }
    for (short attempt = 0; attempt < attempts; attempt++, selected ^= 1)
    {
        if (function_1f5110(actor_index, &direction, distance, vertical_distance, vertical_out, trace))
        {
            local_0 = true;
            goto local_1;
        }
        direction.x = 0.0f - direction.x;
        direction.y = 0.0f - direction.y;
    }
local_1:
    *mode = local_0 ? selected : NONE;
    return local_0;
}
#pragma function(_ReadWriteBarrier)

long function_1e4a50(long index);
real function_26f290(short type);

struct s_movement_speed_definition
{
	byte unknown00[0x1a];
	short type;
};

// @retail 0x1f8050
real function_1f8050(long actor_index, bool *fast)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	s_moving_object *object = moving_object_get(actor->unit_index);
	s_movement_speed_definition *definition = (s_movement_speed_definition *)function_1e4a50(actor->tag_index);
	real result = 1.5f;
	if (fast)
		*fast = false;
	if (definition && definition->type > 0)
	{
		short type = definition->type;
		result = function_26f290((type < 6 ? type : 6) - 1);
		if (fast)
			*fast = type >= 7;
	}
	else if (*(byte *)((byte *)object + 0xaa) == 0)
	{
		result = *(real *)(g_4e3b44[object->tag_index & 0xffff].bytes + 0x1f8);
	}
	return result;
}

/* takes the pending facing (+0x622) unless the actor rides a vehicle; the last
   argument goes unused (the link drops it, its caller still computes it) */
// @retail 0x1f5560
bool function_1f5560(long actor_index, vector3f *facing, bool unused)
{
	s_actor_moving *actor = actor_moving_get(actor_index);

	if (actor->unknown26c == NONE && actor->unknown622)
		vector3f_assign(facing, actor->unknown624 * actor->unknown62c, actor->unknown628 * actor->unknown62c, actor->unknown630);
	actor->unknown622 = false;
	return true;
}

// @retail 0x1f9450
void function_1f9450(long actor_index, long value)
{
	s_actor_moving *actor = actor_moving_get(actor_index);

	actor->unknown300 = value;
	actor->unknown304 = g_510c54->field_2_3 * 5;
}

/* asks the actor's unit (unless it rides a vehicle) to play an animation,
   request 0x19, aimed at a target when one is given */
// @retail 0x1f57f0
bool function_1f57f0(long actor_index, long animation, long const *target)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	bool result = false;

	if (actor->unknown26c == NONE)
	{
		s_unit_request request;

		memset(&request, 0, sizeof(request));
		request.type = 0x19;
		request.type19.animation = animation;
		if (target)
		{
			request.type19.target[0] = target[0];
			request.type19.target[1] = target[1];
			request.type19.field_x4d3867 = true;
		}

		switch (animation)
		{
		case 0x9000011:
		case 0x9000012:
		case 0xa000010:
		case 0xa000013:
		case 0xd00002b:
		case 0xe00002a:
			request.type19.mode = 1;
			break;
		default:
			request.type19.mode = 2;
			break;
		}

		result = function_e6900(actor->unit_index, &request);
	}

	return result;
}
/* the parts of the actor's unit that say where it is heading */
struct s_heading_unit
{
	byte unknown000[0x70];
	vector3f forward;
	byte unknown07c[0xaa - 0x7c];
	byte type;
	byte unknown0ab[0x344 - 0xab];
	short unknown344;
	short state_offset;
	byte unknown348[0x3dc - 0x348];
	byte movement_type;
	byte unknown3dd[0x3ec - 0x3dd];
	point3f unknown3ec;
};

struct s_heading_unit_header
{
	byte unknown00[8];
	s_heading_unit *object;
};

/* the actor's unit heading: the animation state it is in, the two values the
   animation state machine reports for it, and the point it is heading for */
struct heading_motion
{
	short animation_state;
	long first;
	long second;
	point3f target;
};

extern vector3f *g_4687bc;
real function_30bf0(vector3f *v);
bool function_10f630(long object_index, long *first, long *second);
point3f *function_210850(s_type_c3b527 const *point, point3f *out);

static inline short heading_unit_get_state(s_heading_unit *unit)
{
	return *(short *)((byte *)unit + unit->state_offset + 0x36);
}

/* where the actor's unit (a biped) is heading: backwards out of its current
   animation, or toward the actor's target point */
// MiniMax H (11-03): the animation state, the two values the state machine reports for it and the
// point it is heading for in one heading_motion record, all four members used; the state stays in a
// word on the stack and the record's frame is retail's. The plain check matches (0).
// MiniMax D (11-01): the state in a plain short local 90 and the same read written out in the
// initialiser 90: the load is zero-extended and the spill stays a dword; no lower score.
// Not kept (MiniMax R, 11-01): the state in a one-element unsigned short array (animation_state[1]) also
// matches (0, plain): an array of one steering the state's word stack home is the same form as the
// one-member struct below (Y201, MiniMax G), so it is reverted; the body stays at 11.
// Not kept (MiniMax G, 10-31): the state in a named one-member struct local matches (0); that is the
// one-member struct steering a stack home that Y201 already found here (held, ruled out).
// score 11 (was 11): the state is loaded with movsx and spilled as a dword;
// retail loads it into cx and keeps it in a word on the stack. Tried: the
// state through function_e70b0's out parameter 124; this helper with an out
// parameter and a short state 90.
// state is sign-extended and spilled as a dword; retail
// loads and spills a word. State 7 first 13; state before movement 86;
// animation first 19; promoted accessor return 11; unsigned accessor 11;
// indexed forward 11; indexed target 11; immutable long state 11.
// Round 34: the existing short-output helper with first/second hoisted, reversed or clustered
// with target 124 each; read-only unit and actor carriers on that form 124 each; cached unit
// index 127; initialized short output 124; initialized short assigned by the return accessor 90.
// @retail 0x1f55e0
bool function_1f55e0(long actor_index, point3f *point, vector3f *direction)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	s_heading_unit *unit = ((s_heading_unit_header *)g_4e0300->data)[actor->unit_index & 0xffff].object;
	bool result = false;

	if (unit->type == 0)
	{
		heading_motion motion;

		motion.animation_state = heading_unit_get_state(unit);

		if (motion.animation_state == 6 || motion.animation_state == 7)
		{
			if (unit->movement_type == 4)
			{
				point3f *position = &unit->unknown3ec;

				if (position)
				{
					*point = *position;
					*direction = *g_4687bc;
				}
			}
		}
		else
		{
			function_10f630(actor->unit_index, &motion.first, &motion.second);
			if (unit->movement_type == 5 || motion.animation_state == 5 || motion.first == 0x50000cb)
			{
				direction->i = unit->forward.i * -1.0f;
				direction->j = unit->forward.j * -1.0f;
				direction->k = unit->forward.k * -1.0f;
				result = true;
			}
			else if (actor->unknown264 && g_510c54->game_time < actor->unknown634)
			{
				function_210850(&actor->unknown638, &motion.target);
				direction->i = motion.target.x - actor->position.x;
				direction->j = motion.target.y - actor->position.y;
				direction->k = motion.target.z - actor->position.z;
				if (function_30bf0(direction) > 0.0f)
				{
					result = true;
				}
			}
		}
	}

	return result;
}
bool __stdcall function_110ab0(long unit_index);

/* asks the actor's unit to move to the point facing the given way (request
   0x25), once it is within half a world unit of it; when asked, only while
   it faces within 45 degrees of its prop */
// The facing is the last parameter: retail's body takes the actor in EAX and the facing in ECX with the
// other three on the stack, and both callers' argument setup (0x1afb30, 0x1b89d0) matches retail only
// with the facing declared last.
// @retail 0x1f4f40
bool function_1f4f40(long actor_index, short unknown, s_type_c3b527 const *point, bool face_prop, vector3f const *facing)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	bool result = false;

	long unit_index = actor->unit_index;
	if (!function_110ab0(unit_index))
	{
		point3f target;
		vector3f offset;

		function_210850(point, &target);
		vector3d_from_points3d(&target, &actor->position, &offset);
		if (!(length_sq3f(&offset) > 0.5f))
		{
			if (face_prop)
			{
				point3f const *prop_position = &function_25d690(prop_ref_get(actor->prop_index))->position;
				vector3f direction;

				vector3d_from_points3d(&actor->position, prop_position, &direction);
				direction.k = 0.0f;
				if (!(function_30bf0(&direction) > 0.0f) || dot3f(facing, &direction) < 0.70710677f)
					goto done;
			}

			{
				s_unit_request request;

				request.type = 0x25;
				request.type25.point = target;
				request.type25.facing = *facing;
				request.type25.unknown1c = unknown;
				result = function_e6900(actor->unit_index, &request);
			}
		}
	}

done:
	return result;
}

bool function_2105b0(short output_index, vector3f const *vector, vector3f *out);

static inline real heading_distance3d(point3f const *a, point3f const *b)
{
	vector3f v;

	v.i = b->x - a->x;
	v.j = b->y - a->y;
	v.k = b->z - a->z;
	return (real)sqrt(v.k * v.k + v.j * v.j + v.i * v.i);
}

/* while the actor's target point (+0x638) is fresh: faces it, and once
   within half a world unit of it asks the unit to stop there (request 0x2d) */
// @retail 0x1f58c0
bool function_1f58c0(long actor_index, vector3f *facing, short *unknown)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	bool result = false;

	long unit_index = actor->unit_index;
	if (!function_110ab0(unit_index) && g_510c54->game_time < actor->unknown634)
	{
		point3f target;
		vector3f direction;

		result = true;
		function_210850(&actor->unknown638, &target);
		vector3d_from_points3d(&actor->position, &target, &direction);
		if (function_30bf0(&direction) > 0.0f)
		{
			*facing = direction;
			*unknown = 0;
			actor->unknown6d1 = false;
		}

		if (heading_distance3d(&actor->position, &target) < 0.5f)
		{
			s_unit_request request;

			request.type = 0x2d;
			request.type2d.unknown4 = 2;
			request.type2d.point = target;
			if (!function_2105b0(actor->unknown638.output_index, &actor->unknown648, &request.type2d.vector))
				request.type2d.vector = actor->unknown648;
			if (function_e6900(actor->unit_index, &request))
				actor->unknown634 = NONE;
		}
	}

	return result;
}

bool function_e7020(long unit_index, bool *alternate, long name);
bool function_10f340(long unit_index, long mode, long set);
bool function_10f9b0(long unit_index, long mode, long set, long lookup_flags, transform4x3f *matrix, bool any_weapon);

// score 5 (was 5): retail reads the argument of the callee into EDX before it
// reads the field into EDI; here the field is read first. The registers the
// callee takes are its own choice, and function_e7020 in src/unknown_0a76b0.cpp
// is far from retail.
// @retail 0x1f57a0
bool function_1f57a0(long actor_index, long name)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	bool result = false;
	/* Retail keeps the animation name in its argument slot. */
	long const *name_reference = &name;

	if (actor->unknown26c == NONE)
	{
		bool alternate;
		result = function_e7020(actor->unit_index, &alternate, *name_reference);
	}
	return result;
}

// @retail 0x1f5a60
real function_1f5a60(long unit_index)
{
	real result = 0.92f;
	transform4x3f matrix;

	if (function_10f9b0(unit_index, 0x50000cb, 0x60000cd, 3, &matrix, false))
		result = 0.0f - matrix.position.z;
	return result;
}

// @retail 0x1f5ef0
bool function_1f5ef0(long actor_index, long mode)
{
	bool result = false;
	s_actor_moving *actor = actor_moving_get(actor_index);

	if (actor->unit_index != NONE)
		result = function_10f340(actor->unit_index, mode, 0x400000c);
	return result;
}

/* Chooses the unit animation mode from the actor's requested mode and state. */
// @retail 0x1f5f30
long function_1f5f30(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	long result;

	if (actor->unknown450 != NONE)
		result = actor->unknown450;
	else
	{
		result = 0x6000086;
		if (actor->unknown44b[1])
			result = 0x7000039;
		else if (actor->unknown44b[0])
			result = 0x4000089;
		else if (actor->unknown225 &&
			(*(long *)((byte *)actor + 0x7fc) == 0x700002c || function_1f5ef0(actor_index, 0x700002c)))
			result = 0x700002c;
		else
		{
			switch (actor->unknown084)
			{
			case 1: result = 0x6000084; break;
			case 2: break;
			case 3: result = 0x6000085; break;
			case 4: result = 0x6000086; break;
			case 5: result = 0x4000089; break;
			}
		}
	}
	if (result == 0x4000089 && !actor->unknown5d0 && actor->unknown024 != 1 && team_is_enemy(actor->unknown024, 1))
		result = 0x6000086;
	return result;
}

struct s_actor_move_request
{
	bool active;
	byte unknown01[3];
	s_path_point target;
	long target_index;
	real distance;
	s_path_point start;
	bool flag2c;
	bool flag2d;
	bool flag2e;
	byte unknown2f;
	short mode;
	short value32;
	long next_index;
	short value38;
	byte unknown3a[2];
	s_path_point next_target;
	byte unknown4c[0xa0 - 0x4c];
	long valuea0;
	short valuea4;
	byte unknowna6[0xc4 - 0xa6];
};

struct s_actor_move_target
{
	s_path_point point;
	long unknown10;
	long index;
};

/* Seeds a movement request from an actor location and a valid target. */
// @retail 0x1f9490
bool function_1f9490(long actor_index, s_reference reference, s_actor_move_request *request)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	s_actor_move_target *target = (s_actor_move_target *)function_262b40(reference);
	bool result = false;

	if (target)
	{
		function_26c180(actor_index);
		memset(request, 0, sizeof(*request));
		request->start = actor->location;
		request->target = target->point;
		request->target_index = target->index;
		request->distance = 0.0f;
		request->flag2c = true;
		request->flag2d = true;
		request->flag2e = false;
		request->value32 = 0;
		request->value38 = NONE;
		request->next_target = target->point;
		request->next_index = target->index;
		request->mode = 6;
		request->valuea0 = NONE;
		request->valuea4 = NONE;
		request->active = true;
		result = true;
	}
	return result;
}

struct s_1f9762
{
	byte field_0[0x1c];
	byte field_1c[0x11];
	char field_2d;
	char field_2e;
	byte field_2f;
	s_actor_point_entry field_30[4];
};

// MiniMax B (11-18): later still, ~170 spellings tried in total: VC7 gives EDX to whichever operand of the
// entry test carries the plus or minus one, so every spelling that increments the index (`index + 1 >=
// count`, the same with the index or the count in an int, long, short or char local, the increment folded
// into the local's initialiser, the test negated, both sides widened, either side cast to char or short,
// and the same test with an addend on each side) puts the index+1 in EDX and the count in EAX and so
// changes the load order as well (4 to 6). The one spelling that reproduces retail's entry test exactly
// (`inc eax; cmp eax, edx; setge al`, the count in EDX and the index+1 in EAX) reads the count through a
// function-scope path pointer at actor+0x50c: `actor->unknown53a + 1 >= path->field_2d` scores 2, because
// the second dot product's i term then takes the direction as multiplicand where retail takes the point;
// with the index read through the path as well (`path->field_2e + 1 >= path->field_2d`) the whole float
// block re-rotates (the j and k terms swap, score 10), and the path declared before the definition line
// scores 10 too. That i term does not move under the helper's argument order, under a named local for
// either pointer or under the two dot products in locals (2, 4), so the committed body (score 1) stands.
// MiniMax B (11-18): later still, ~140 spellings tried in total: the path-pointer entry test and the two
// dot products cannot both be right at once. With the path-pointer test `path->field_2e + 1 >=
// path->field_2d` and the current point still read through the actor (`unknown53c[unknown53a]`), retail's
// `inc eax` comes out but the whole float block re-rotates (the j and k terms swap, score 10); with the
// current point read through the path the float block keeps retail's shape and only the second dot
// product's i term flips (score 2). The second dot product's i term is invariant under the helper's
// argument order (`dot3f(point, direction)` and `dot3f(direction, point)` compile the same), under a
// named local for the input pointer and under `&point->n[0]`, so it is an allocator tie; the committed
// body (score 1) stands.
// MiniMax B (11-18): later, ~80 more spellings tried: a function-scope `s_1f9762 *path` (count, index and
// entries all read through actor+0x50c) with the entry test written `path->field_2e + 1 >= path->field_2d`
// reproduces retail's `inc eax`/`cmp eax, edx`/`setge al` exactly, but the second dot product's i term then
// takes the direction as multiplicand where retail takes the point (score 2, worse than the committed 1).
// A 16-way grid over arm/previous/approach/projection read through the path or the actor's own fields
// (`unknown53c`, `unknown539`, `unknown53a`) scores 2 at best and always differs in the same two
// instructions (`movss xmm2, [eax]`/`mulss xmm2, [ecx]` against retail's `movss xmm2, [ecx]`/
// `mulss xmm2, [eax]`); the `arm=actor` rows score 17-39 because retail folds the path base into
// `esi + 0x50c` (`lea eax, [ecx + esi + 0x50c]`) and the actor form loses that base. The i-term operand
// choice is an allocator tie that source-level reordering, named pointer locals and helper argument
// swaps do not move, so the body stands.
// MiniMax B (11-18): score 1 (unchanged): the entry test is the only difference (`dec edx` where retail has
// `inc eax`; every other byte agrees). Tried here (~30 spellings of that one test): `count <= index + 1` (4)
// and `count - 1 <= index` (3) put the count in EDX and the index in EAX exactly as retail does, but the
// adjustment then lands on the EDX side and the condition comes out setle; `index + 1 >= count` (4) and the same
// with the index or the count held in int/long/short/char locals, the increment folded into the local's
// initialiser, a pre-increment local and `index++` (6 each) hoist the index load ahead of the definition chain
// and fuse the increment into EDX (`inc edx; cmp edx, eax`), so the load order changes as well; the baseline
// (`index >= count - 1`) keeps retail's load order but decrements the count instead of incrementing the index.
// No spelling found puts the incremented index in EAX against the count in EDX, so the body stands.
// MiniMax A (11-18): score 1 (was 4): the entry test is written `index >= count - 1` (the same test as
// `index + 1 >= count`), which loads the count into EDX and the index into EAX as retail does; left: the
// test ends in `dec edx` where retail has `inc eax` (the count is decremented, the index is not).
// A function-scope path pointer (count, index and entries read through the path at actor+0x50c)
// makes the entry test load the count first and gives retail's `inc eax`/`cmp eax, edx` exactly, but the
// dot products then differ (score 2: the second dot's i term takes the direction as multiplicand) and the
// previous point still reads the actor's own fields; the projection's point read through the path gives 2,
// through the actor's own field with the path pointer live gives 10.
// Writing the test the other way round (`index + 1 >= count`, `count <= index + 1`, the same test negated and
// the same test with both sides widened) all measure 4: the increment moves to EDX with the index and the count
// takes EAX, so the compare and the load order change as well. The body stands.
// score 4 (was 10): Frame and extent match. The entry swaps the homes of the path count and current index.
// The path block is read through the actor's own fields except in the not-last arm, which
// takes a pointer to it for the segment (the shape retail's 0x2967b0 shows: +0x50c-based segment points, direct
// count/index reads); left: the entry test loads the index before the count.
// Before that, 10 (was 12): the first dot product takes the projection vector first; left: the two dot
// products evaluate their j and k terms in opposite orders (ours j,k / k,j; retail k,j / j,k) and the
// second dot's i term keeps the projection vector in the register where retail has the point.
// Round 103: on the projection index read through the actor, a second projection pointer, input pointer,
// projection or input reference, direct comparison and subtract-assignment 2 each; current dot named first 4;
// input dot named first 2; input dot alone named 4; two-component input product 14. Original body restored.
// MiniMax G (10-31): the first dot written out by hand (i term projection-first, j and k terms
// point-first, an order the helper cannot express) 7: an existing helper's body by hand, kept only
// with a match; both dots projection-first 10 (same code).
// Earlier 12: layout, frame and code now as retail; left: both dot products evaluate their
// Frame and block layout agree; dot-product term/operand order remains.
// Earlier: layout, frame and code now as retail; left: both dot products evaluate their
// y and z terms in swapped order with each x term's operands swapped (ours y,z / z,y; retail z,y /
// y,z), a value-numbering tie that flips with the CSE sharing of the current-point address.
// Earlier state: approach as its own statement after the arms (no gotos), struct copy of the
// direction and `remaining <` on the left give retail's length and code (frame 0x1c and entry
// agree); left: retail lays the previous-point false arm and the approach call after the first
// return (a jump-threaded copy) with the last-path projection falling into the shared test; ours
// keeps them between the last arm (which jumps to the projection) and the projection. Tried:
// direction-first dots 67/67; `direction_valid && !last` 191 (also with the last arm's tests
// swapped or nested, or `&& !projection_direction`: the approach moves after the first return as
// retail does but the projection pointer stays in edi and the actor index in ecx); nested
// `direction_valid` then `!last` 198; `!last` then `direction_valid` nesting 67; per-path
// projections 320; the approach inside the not-last arm under its direction test 104;
// `!projection_direction &&` first 67; the last arm's `actor->unknown4d4 && direction_valid` 69
// and nested 67; permuter 56 tries, best 67. Normal admission first 111; admitted last result
// 125; combined forced/last result 208; path-defined validity 131; true-default validity 135;
// path-defined reached 149; path-defined projection 67; admitted last with complete validity
// 149; separate approach body: first/last/both normalized-result assignments 111/105/116; entry
// valid default 127; long validity/last/both 161/161/173; conditional last projection 67;
// reached word/native 110/111; short validity/last/both 163/164/178; direction-publication-first
// 67; publication-first with word reached 110; readonly actor/path receivers 67 (fast).
// Round 11: the previous point as if/else 67 (same code), as a default plus `if` 115 (623
// bytes); the last arm written before the not-last arm (`else if (last)`) 105; the approach
// after the direction test inside the not-last arm 104; `!reached &&` on the projection test 67.
// A compile-only layout screen moved the approach block with none of ~10 forms.
// Selected projection by value across four three-vector lexical orders, last/validity-first guards
// 133/264 in every order; extra vector storage grows the frame and does not recover native tails.
// Round 16: the approach call written in each arm of the previous-point choice 114 (arms swapped
// 118, the magnitude test in each arm too 114); `!last` then `direction_valid` nested 67; the
// reverse nesting 191; the approach inside the not-last arm under or after its direction test
// 104/104; the projection as a forced-inline test called from both paths 320 (not cross-jumped).
// Compile-only screens (~70 forms): the previous-point false arm and the approach call are
// compiler-made copies; a `direction_valid`-first approach test appends them before the final
// exit block as retail does, but the reached path then keeps a runtime projection test (pointer
// live from entry); a `last`-first test threads every path and keeps the copies in place.
// `!reached` or `!projection_direction` guards, the last arm's test as `<=` or nested, and the
// last arm written first change neither.
// Boolean result/home role names in four lexical orders 67/67/67/67; each with a shared
// output predicate used for both validity and vector publication 83/83/83/83 (616 bytes).
// Round 82: the unknown456 arm without its `direction_valid = false` 127 (retail keeps that store);
// an explicit `else direction_valid = false` in the not-last arm 111; validity assigned in every arm
// with no initializer 128; the projection test as one condition without `remaining` 67; the
// reached test inverted (`<= distance` arm first) 88; the output if/else inverted 67.
// Round 9: a bool `project` flag beside the pointer 67, with `direction_valid && !last` 152; the
// projection as an inline predicate on each path 320; the previous point picked inside the valid
// branch of the not-last arm 108; `if (!reached && direction_valid && unknown4d4)` in place of the
// last arm's else-if 67; `projection_direction = mag > 0.0f ? &approach : NULL;` 67. Compile-only
// screens: VC lays blocks out as fall-through runs in IL order, so retail's projection must be the
// IL successor of the last arm's pointer store, with the approach (false arm, join and call) placed
// just before the final return block, where VC puts blocks it creates late; no form moved it there
// with the pointer in EAX.
// Round 11 (lane 15): `bool use_approach = !last && direction_valid;` 67 (same code); the approach
// inside the not-last arm's valid branch with a ternary reversed, if/else or nested `if
// (direction_valid)` 104/104/104, with a default plus `if` 111. Compile-only: only `direction_valid
// && !last` (listed) moves the approach after the first return, and it keeps the pointer in EDI.
// Round 444: caller ordinary-inline difference/normalizer/query boundaries, seven
// subsets 67 each; all with direction-valid-first guard 191. All declarations and
// the complete body restored; native approach placement remains.
// Round 38 (lane 1): kept 12: the last arm keeps only the distance test and its pointer choice moves
// to `else if (!reached && direction_valid && unknown4d4)` after an `if (!last) { if (direction_valid)
// { approach } }` (VC pulls the projection/return chain up behind the last arm only when the
// approach's store is not the projection's IL predecessor). On that graph: the actor's own path
// fields with a typed 0x528 start 23 (loses the +0x50c base); the previous point or the projection
// index read through the actor's own fields beside `path` 4/4 (mixed views), with `dot3f(direction,
// point)` first 2/2, with the other argument orders 4/2, with both mixes 10; compile-only screens
// (~150 forms: else-if condition orders, declaration positions, names, previous-point if/else or
// two calls, the path-records layout at +0x50c, a typed 0x4d8 vector, operand orders, a one-use
// current-point pointer in the projection) keep y,z.
// Round 14 (lane 2; fast, aligned in brackets, 30 at the start): only the first dot direction first
// 67 [28] (retail's i product is direction first in the first dot, point first in the second; ours
// the reverse); only the second 67 [30]; `last` as `field_2d <= field_2e + 1` 69; the previous-point
// ternary with its arms reversed 92, with the first dot swapped 92; diagnostics, one-caller helpers
// (not keepable): the previous point as a forceinline accessor 67 [28], the approach computation
// as a forceinline helper returning whether it has length 67 [36].
// Round 101 (lane 8; exact one-link LTCG harness, fast-confirmed): the first dot's arguments
// reversed 10, the `next` local dropped 10, the projection as one condition 10 (each fixes only the
// first dot's x-term); field_1c typed as the point record x `next` x remaining/one condition/two
// named dots x both argument orders x `previous` local, 96 forms 10-12; the point's dot named first
// 12 (canonicalised); `field_2e >= field_2d - 1` 13, `field_2d <= field_2e + 1` 14, `field_2e >= 1`
// 14; path-point or current-point accessors 10-12; char/long/short index locals in the approach
// 12/45/8 (short flips y/z but loads a word, 19 fast), in the direction arm 137-140; renames 12; the
// projection as an inline helper in all parameter and argument orders 10. Diagnostic: the path read
// through an embedded member at the projection only, first dot reversed, 2 (a mixed read); the second
// dot's x-term never moved in 12 forms on that graph.
// Round 456: Native persistent path reference on recovered block graph, readonly path, actual point value at projection and first-dot operand graphs:
// 12/10/108/108/12/10/108/108. Original executable body and supporting declarations restored.
// Round 116 (m3 lane 10, fast unless marked; a compile-only screen reproduces the dot orders): the y/z order of both dots
// follows how the current point's address is formed: with only the projection's point read as
// `actor->unknown53c[actor->unknown53a]` beside main's `path` 2 (not kept: a mix of the two views in one scope) both
// dots take retail's y/z order. With no `path` local at all
// (actor fields everywhere) all three x/y/z orders are retail's, but the not-last arm loses retail's +0x50c base; that
// arm with its own arm-scoped `path` (as 0x2967b0's segment pointer) and actor fields elsewhere 4 (kept; left: the entry
// loads the index before the count). Computing a function-scope `path` before the entry test gives retail's count-first
// loads and flips the second dot's x term (2); computing it after the test keeps the x term and loads the index first
// (4); declared early but assigned after the locals 4, assigned just before the test 2. On the arm-scoped form: the
// test as `count <= index + 1` 4, `1 + index >= count` 4, either field through the +0x50c cast 4/4, through a block
// or an inline helper taking the block 6/6/6 (helper inline or forceinline), the test before the definition read 143.
// On the kept form or the function-scope-path form: the 32 combinations of `path` or actor fields at the entry test and
// the approach's three reads (ten at 2, all the same pair); the locals in five other orders 2 each (with the
// projection pointer before `path` the first dot's x term flips instead); `path` renamed route/actor_path/zpath/
// a_path/segment 2 each; `path` as the address of the +0x50c flag 2; the second dot's arguments swapped or
// non-const 2/2/2; the first dot named 2, the second named 4, both 4, the sum negated 37; a const current point 2;
// a non-const projection pointer 2; the three vector locals in all six orders 2; actor fields in the arm too 21.
// Lane N round 411 (fast control 4).
// Count before tag 6.
// Count after tag 6.
// Final ordinal test 1 (NOT kept: three-point gain with a decrement still differing).
// Points remaining test 135.
// Not before final 4.
// Count then successor 6.
// Final ordinal local 3 (NOT kept: one-point gain with count decrement and a dot operand order still differing).
// Count byte before tag 6.
// Object cull radius 4 (kept after plain check).
// Count branch result 4.
// Count first complement 4.
// Narrow next ordinal 29.
// Both end ordinals 131.
// Next ordinal increment 6.
// Reversed final ordinal 3 (NOT kept: one-point gain without a match).
// Typed tag final ordinal 1 (NOT kept: three-point gain with a decrement still differing).
// Terminal fields before tag 6.
// Referenced count field 6.
// Can advance result 107.
// tag index before count 6; tag index before count and successor 4; last between tag index and table
// 143; object header tag lookup 4; object receiver before count 6; scoped count and successor 6; count
// and prefix successor 6; index via point bound 18. NOT kept because the forms tie or raise the score.
// Tag entry pointer/reference across terminal test 143/143; table base local 6;
// entry pointer/reference with count before definition 6/6; table base then count 6; explicit
// successor/terminal result arms 4; named available successor 200. NOT kept: the count and index
// sign extensions, increment and compare still use the opposite registers.
// @retail 0x1f9760
bool function_1f9760(long actor_index, long object_index, point3f const *point, real distance,
	bool *last_out, bool *direction_valid_out, vector3f *direction_out)
{
	bool reached = false;
	s_actor_view *actor = actor_get(actor_index);
	s_object_definition const *definition = (s_object_definition const *)g_4e3b44[moving_object_get(object_index)->tag_index & 0xffff].bytes;
	bool last = actor->unknown53a >= actor->unknown539 - 1;
	bool direction_valid = false;
	vector3f direction;
	vector3f approach;
	vector3f const *projection_direction = NULL;
	if (actor->unknown456)
	{
		direction_valid = false;
		last = true;
	}
	else if (!last)
	{
		s_1f9762 *path = (s_1f9762 *)((byte *)actor + 0x50c);
		s_type_c3b527 *next = &path->field_30[path->field_2e + 1].point;
		function_210be0(&path->field_30[path->field_2e].point, next, &direction);
		if (function_30bf0(&direction) > 0.0001f)
			direction_valid = true;
	}
	else
	{
		if (actor->unknown4d5 || actor->unknown4d4)
		{
			direction = *(vector3f *)((byte *)actor + 0x4d8);
			if (function_30bf0(&direction) != 0.0f)
				direction_valid = true;
		}
		if (function_1e3920(actor_index) > distance)
			reached = true;
	}
	if (!last)
	{
		if (direction_valid)
		{
			s_type_c3b527 *previous = actor->unknown53a > 0 ? &actor->unknown53c[actor->unknown53a - 1].point : (s_type_c3b527 *)actor->unknown528;
			function_210be0(previous, &actor->unknown53c[actor->unknown53a].point, &approach);
			if (function_30bf0(&approach) > 0.0f)
				projection_direction = &approach;
		}
	}
	else if (!reached && direction_valid && actor->unknown4d4)
		projection_direction = &direction;
	if (projection_direction)
	{
		real remaining = dot3f(projection_direction, (vector3f *)&actor->unknown53c[actor->unknown53a].point.point) - dot3f((vector3f const *)point, projection_direction);
		if (remaining < definition->cull_radius * 1.2f)
			reached = true;
	}
	*last_out = last;
	if (last && !actor->unknown4d5)
		*direction_valid_out = false;
	else
	{
		*direction_valid_out = direction_valid;
		*direction_out = direction;
	}
	return reached;
}

bool function_fa1a0(real speed, real gravity_scale, point3f const *origin, point3f const *target,
	real *minimum_speed, real const *time_scale, real const *forced_speed, bool high_arc, vector3f *direction,
	real *speed_out, real *time_out, real *distance, real *vertical_speed, real *horizontal_speed);

// @retail 0x1f8100
bool function_1f8100(long actor_index, point3f const *origin, point3f const *target)
{
	bool fast;
	real speed = function_1f8050(actor_index, &fast);
	if (fast)
		return true;
	vector3f direction;
	return function_fa1a0(speed, 1.0f, origin, target, NULL, NULL, NULL, false, &direction,
		NULL, NULL, NULL, NULL, NULL);
}

real normalize2d(point2f *v);

/* tries to launch the actor at a point on a ballistic arc: the trajectory at the given speed, then (when
   forced, or when the speed allows it) again at just above the minimum speed the first query reported. A
   unit in action state 5 gets the launch as a unit request; any other unit, unless the first movement
   type was chosen or the base speed already covers it, has the launch speeds and a planar facing stored
   on the actor. Retail inlines this into 0x1f8160 with its own result and exits: the caller's validity
   flag takes the returned value, which is why its invalid exit returns the flag itself, and the minimum
   is the caller's local (cleared at its entry), so it comes in by address. */
static __forceinline bool actor_try_ballistic_launch(s_actor_view *actor, point3f const *target, real speed, bool force,
    bool first_type, real *minimum)
{
    vector3f direction;
    real vertical;
    real horizontal;
    real chosen_speed = speed;
    bool result = function_fa1a0(speed, 1.0f, &actor->position, target, minimum, NULL, NULL, true,
        &direction, NULL, NULL, NULL, &vertical, &horizontal);
    if (force || speed * 1.25f > *minimum)
    {
        real retry_speed = *minimum + 0.03f;
        real retry_vertical;
        real retry_horizontal;
        if (function_fa1a0(retry_speed, 1.0f, &actor->position, target, minimum, NULL, NULL, true,
            &direction, NULL, NULL, NULL, &retry_vertical, &retry_horizontal))
        {
            result = true;
            horizontal = retry_horizontal;
            vertical = retry_vertical;
            chosen_speed = retry_speed;
        }
    }
    if (result)
    {
        byte *unit = (byte *)object_get(actor->unknown018);
        if (*(short *)(unit + *(short *)(unit + 0x346) + 0x36) == 5)
        {
            s_unit_request request;
            request.type = 0x2e;
            *(point3f *)((byte *)&request + 4) = *target;
            return function_e6900(actor->unknown018, &request);
        }
        if (!first_type)
        {
            real base_speed = 0.0f;
            byte *globals = (byte *)g_4e034c;
            if (globals && *(long *)(globals + 0xc8) > 0)
                base_speed = *(real *)(*(byte **)(globals + 0xcc) + 0x80);
            if (base_speed * 1.2f >= chosen_speed)
                return result;
            actor->unknown464 = true;
            actor->unknown466 = true;
            *(real *)((byte *)actor + 0x470) = horizontal;
            *(real *)((byte *)actor + 0x474) = vertical > 1.2f ? vertical : 1.2f;
            direction.k = 0.0f;
            if (normalize2d((point2f *)&direction) > 0.0f)
            {
                *(point2f *)((byte *)actor + 0x468) = *(point2f *)&direction;
            }
            else
            {
                vector3f facing = actor->unknown290;
                if (facing.k != 0.0f)
                {
                    facing.k = 0.0f;
                    if (normalize2d((point2f *)&facing) <= 0.0f)
                        facing = *g_4687a8;
                }
                *(point2f *)((byte *)actor + 0x468) = *(point2f *)&facing;
            }
            return result;
        }
    }
    return result;
}

// @retail 0x1f8160
bool function_1f8160(long actor_index, signed char const *types, point3f const *target, bool force)
{
    s_actor_view *actor = actor_get(actor_index);
    real minimum = 0.0f;
    bool fast = false;
    bool valid = false;
    bool first_type = false;
    real speed = function_1f8050(actor_index, &fast);
    if (!types)
        valid = true;
    else
    {
        s_movement_speed_definition *definition = (s_movement_speed_definition *)function_1e4a50(actor->unknown054);
        if (definition)
        {
            short selected_type = (definition->type < 6 ? definition->type : 6) - 1;
            if (force)
            {
                if ((short)(((1 << (selected_type + 1)) - 1) | *types))
                {
                    first_type = (*types & 1) != 0;
                    valid = true;
                }
            }
            else
            {
                for (short i = 0; i < 6; i++)
                {
                    if (*types & (1 << i))
                    {
                        if (i <= selected_type)
                            selected_type = i;
                        real selected_speed = 0.0f;
                        byte *globals = (byte *)g_4e034c;
                        if (selected_type >= 0 && selected_type < 6 && globals && *(long *)(globals + 0xc8) > 0)
                            selected_speed = *(real *)(*(byte **)(globals + 0xcc) + 0x80 + selected_type * 4);
                        speed = selected_speed;
                        first_type = i == 0;
                        valid = true;
                        break;
                    }
                }
            }
        }
    }
    if (fast)
        force = true;
    if (valid)
        valid = actor_try_ballistic_launch(actor, target, speed, force, first_type, &minimum);
    return valid;
}

long function_1fa7f0(void);
void function_b9fc0(long object_index, vector3f *forward, vector3f *up);
real function_210ac0(s_type_c3b527 const *a, point3f const *b);
bool function_1f8660(long actor_index);

// @retail 0x1f5ab0
void __stdcall function_1f5ab0(long actor_index, vector3f const *input, vector3f *facing_out,
    short *type_out, vector3f *movement_out, bool *blocked)
{
    byte *actor = (byte *)actor_get(actor_index);
    real distance = 3.402823466e38f;
    byte *entry = NULL;
    byte *surface = NULL;
    vector3f movement;
    vector3f facing = *(vector3f *)(actor + 0x290);
    movement = *g_4687a4;
    actor[0x6d1] = false;
    point3f position = *(point3f *)(actor + 0x238);
    if (*(long *)(actor + 0x18) != NONE)
    {
        byte *unit = (byte *)object_get(*(long *)(actor + 0x18));
        position.z = *(real *)(unit + *(short *)(unit + 0x116) + 0x30);
    }
    long const &unit_index = *(long *)(actor + 0x18);
    if (actor[0x5d0] && !actor[0x5d2] && *(signed char *)(actor + 0x53a) >= 0 && *(signed char *)(actor + 0x53a) < *(signed char *)(actor + 0x539))
    {
        if (function_1f8660(actor_index))
        {
            entry = actor + 0x53c + *(signed char *)(actor + 0x53a) * 0x1c;
            long surface_index = *(short *)(entry + 8);
            if (surface_index != NONE)
                surface = *(byte **)((byte *)function_1fa7f0() + 0x3c) + surface_index * 0x14;
            distance = function_210ac0((s_type_c3b527 *)(entry + 0xc), &position);
        }
        vector3f direction = *input;
        if (function_30bf0(&direction) == 0.0f)
            direction = *g_4687b0;
        if (surface)
        {
            facing = ((vector3f *)*(byte **)((byte *)function_1fa7f0() + 0x2c))[*(word *)(surface + 8)];
            facing.k = 0.0f;
            if (function_30bf0(&facing) == 0.0f)
            {
                facing = *(vector3f *)(actor + 0x290);
                facing.k = 0.0f;
                if (function_30bf0(&facing) == 0.0f)
                    facing = *g_4687a8;
            }
        }
        else
        {
            function_b9fc0(unit_index, &facing, NULL);
            facing.k = 0.0f;
            if (function_30bf0(&facing) == 0.0f)
            {
                facing = *(vector3f *)(actor + 0x290);
                facing.k = 0.0f;
                if (function_30bf0(&facing) == 0.0f)
                    facing = *g_4687a8;
            }
        }
        if (actor[0x5d0])
        {
            if (surface && *(short *)surface == 2)
            {
                if (*(short *)(surface + 0xa) == 2 && function_1f5a60(unit_index) > distance)
                {
                    point3f target;
                    function_210850((s_type_c3b527 *)(entry + 0xc), &target);
                    s_unit_request request;
                    request.type = 0x23;
                    *(point3f *)((byte *)&request + 0x14) = target;
                    *(vector3f *)((byte *)&request + 8) = facing;
                    function_e6900(unit_index, &request);
                    *blocked = true;
                    goto done;
                }
                if (function_1e3920(actor_index) > distance)
                {
                    *blocked = true;
                    goto done;
                }
            }
            else
            {
                s_unit_request request;
                request.type = 0x2d;
                *(short *)((byte *)&request + 4) = 2;
                *(point3f *)((byte *)&request + 8) = position;
                *(vector3f *)((byte *)&request + 0x14) = facing;
                function_e6900(unit_index, &request);
            }
        }
        if (!*blocked)
        {
            if (direction.k > 0.0f)
            {
                movement.j = movement.i = 0.0f;
                movement.k = 1.0f;
            }
            else if (direction.k < 0.0f)
            {
                movement.j = movement.i = 0.0f;
                movement.k = -1.0f;
            }
        }
    }
done:
    if (facing_out)
        *facing_out = facing;
    if (type_out)
        *type_out = 0;
    if (movement_out)
        *movement_out = movement;
}

real function_50650(real cosine);
#define MIN(a, b) ((a) > (b) ? (b) : (a))

// score 112 (was 120; 137 before): Frame 0x1c agrees. Retail scales the actor index before the pool load and
// holds the actor in EDI and the object in ESI. This build exchanges those homes.
// M3 lane 14 r112 (fast; control 120): the closing term as `dot_product2d((vector2f const *)&object->velocity,
// path_direction) - dot_product2d(path_direction, (vector2f const *)&other->velocity)` 114 (that block then has retail's
// loads and operand sides exactly, only the ESI/EDI names differ; six points, under the bar, not kept); on it the velocity
// dot and the facing dot through dot_product2d 114/114 (same code); the actor through the existing actor_datum_get 120
// (same code); one to seven dead initialised locals (diagnostic) 120 each with identical listings (no symbol-count effect
// here); the normalize written with `i = inv * i; j = j * inv` 122, in the normalize3d shape (`if (!(fabs(m) < eps))`
// scale helper, else m = 0) 114; a named velocity j or i after the normalize 115/115; the turn clamp spelled `1.0 <
// cosine` or as an if/else on a named double 115/115 (the `fld 1.0; fld st(1)` stays where retail has `fld st(0); fld
// 1.0; fxch`). Diagnostic: with nearly every actor read removed (approach blocks and the following test) the actor still
// takes ESI, so the exchange is not a use-count tie: the actor's web takes the scaled index's register (`add esi, edi`)
// where retail forms it with `lea edi, [edx + esi]` from a scratch index.
// M3 lane 14 r114 (fast; control 120, aligned 47 raw / 38 register-masked): the in/out planar scale helper pair
// (as src/unknown_1ff010.cpp has it) for the velocity normalize, with the closing term through dot_product2d, the
// velocity copied as `*(vector2f const *)&object->velocity` and its dot as `dot_product2d(desired, &velocity)` 263 at
// retail's 1324 B (masked aligned 22-25: the normalize block is then retail's exactly; left are the actor/object
// exchange, the two 1.0 entry stores, the velocity dot loading desired's j first, the clamp's `fld st(1)` and the
// final product's slot); the same without the velocity dot helper 114 (masked 26), without the copy 306 (1324 B); the
// closing term alone 114, with the copy 114. Diagnostics (one-object link screen, restored): the actor offset stays
// coalesced with the actor (`add esi, edi`) with any one of the four blocks deleted, with two extra object reads, an
// extra actor read, one fewer actor read, the actor read through actor_get at every use, and with one or two extra
// long-lived integer values; retail's lea form is in 26 matched functions (0x1c1d50 the closest: offset in EDX, pool
// in ESI, `lea ebx, [edx + esi]`), with no source difference found in their actor lookups.
// MiniMax R run 92 (11-17): no new form, no lower score (120): retail still holds the actor in edi and the object in
// esi and scales the actor index into edx before it loads the pool (lea edi, [edx+esi]) where this build loads the
// pool first and keeps the actor in esi; the two 1.0f entry stores still come out in the opposite order and the
// callee's declaration order (object index first) is fixed by four other files.
// MiniMax D (11-16): no new form, no lower score (120): this round read the retail diff against ours again and the
// remaining delta is the prologue and the two pointers: retail masks and scales the actor index into EDX before it
// pushes and forms the actor with `lea edi, [edx + esi]` (actor in EDI, object in ESI) where this build loads the pool
// first, keeps the scaled index in ESI and forms the actor with `add esi, edi` (actor in ESI, object in EDI); the two
// 1.0 entry stores are still the other way round (ours +0x10 then +0x1c, retail +0x1c then +0x10) and the length is
// 1326 against retail's 1324; retail's own call site takes the object index first and the same function is declared
// in four other files: BLOCKED-BY those declarations.
// MiniMax R (11-17): no new form, no lower score (120): retail holds the actor in edi and the object in esi and
// scales the actor index into edx before it loads the pool (lea edi, [edx+esi]) where this build loads the pool first
// and keeps the actor in esi and the object in edi; the two 1.0f entry stores still come out in the opposite order, the
// velocity's j preload belongs to the shared normalizer in include/unknown_11cc90.h, and the callee declaration
// order (object index first) is fixed by four other files.
// Earlier score 120 (was 137): round 183 (m3 lane 6, plain): the turn clamp through the existing PIN_126660 on the dot expression, with no
// named double (the double clamp kept: retail's fld qword 1.0), 137 -> 120; it also ends #581's padding of 0x1f38c0's
// callers, so 0x1f38c0's body is enabled. Earlier at 137: no new form and no lower score; retail still computes the actor offset in EDX before
// the frame pushes and keeps the actor pointer in EDI with the object pointer in ESI, where this build loads
// the actor pool before the pushes and keeps the actor in ESI; a named offset local, the object lookup
// first and the existing actor getter all measure 137 or worse.
// MiniMax A (11-13): no new form, no lower score (137; the fast check reads 117): the actor address as the
// folded sum of the pool pointer and the masked index compiles to the same bytes; the actor and object
// pointers still take the other way round (ours actor in ESI and object in EDI, retail actor in EDI and
// object in ESI), and retail still scales the actor index into EDX before it loads the pool where ours
// loads the pool first and scales the index into ESI.
// MiniMax G (11-12): no new form, no lower score (137; the fast check reads 117): the actor offset as a named local compiles to the same bytes as the folded expression.
// MiniMax D (11-10): no new form, no lower score (137); retail scales the actor index into EDX before the
// register saves and keeps the actor pointer in EDI with the object in ESI, ours hoists the global loads and
// keeps the actor in ESI with the object in EDI; retail's function_1e5450 also takes its definition index
// from the stack where our call leaves it in EDX.
// MiniMax D (11-10): no new form, no lower score (137); retail computes the masked actor offset into EDX
// before the prologue and adds the pool data with an address LEA (`lea edi, [edx + esi]`), ours loads the
// pool global first, keeps the offset in ESI and adds it with `add esi, edi`; the two pointers keep the
// other way round (ours actor ESI / object EDI, retail actor EDI / object ESI), which is what decides
// the LEA against the ADD, and the two entry stores of the 1.0 factors come out in the opposite order
// (ours `[esp + 0x10]` then `[esp + 0x1c]`, retail `[esp + 0x1c]` then `[esp + 0x10]`); the actor and
// object pointers as const pointers change nothing (137).
// MiniMax C (11-14): no new form, no lower score (137): retail masks the actor index and multiplies it
// before it loads the pool, then forms the actor with lea edi,[edx+esi] (actor EDI, object ESI); ours
// loads the pool first and leaves the actor in ESI, so the prologue is two bytes longer and the rows pair
// wrongly; the offset-first expression and the scaled-offset local both give the same code. The normalize
// block still differs in the pre-load of the velocity's j before fsqrt and in the product order (that
// helper is shared in include/unknown_11cc90.h), and the two 1.0f scale stores are swapped against retail.
// MiniMax S (11-10): no new form, no lower score (137): the actor and object pointers still take the other way round (ours actor in esi / object in edi, retail actor in edi / object in esi), so the prologue is one byte shorter and every later row pairs with the wrong row.
// MiniMax C (11-09): no new form, no lower score (137); the body differs only by the actor/object pointer pair (ours actor ESI / object EDI, retail actor EDI / object ESI), so the prologue is one byte shorter and every later row pairs with the wrong row.
// Round 16: retail passes the object index in EAX and the actor index in ECX, ours passes the
// actor index in ECX and keeps the object index elsewhere; retail computes the masked actor
// offset before the pool load and holds the actor in EDI.
// MiniMax C (11-09): no new form, no lower score (137); the actor address as a subscript, the object lookup
// before it and the index-first expression all leave the code unchanged, so the actor/object register pair
// (ours actor ESI / object EDI, retail actor EDI / object ESI) still differs.
// MiniMax D (11-14): no new form, no lower score (137; the fast check reads 117): retail's own call site
// puts the object index in EAX and the actor index in ECX, so retail's declaration takes the object index
// first; the same function is declared in four other files, so the order is reported and left alone, and the
// permuter's six tries over the reduced image (with --allow-mismatch, since the reduced score 335 differs
// from the full 135) found nothing below the baseline.
// Earlier at 137: frame 0x1c agrees; actor/object homes, velocity preload and final minimum remain (caller-inclusive fast 137).
// Earlier: frame 0x1c agrees; two excess bytes, actor/object roles (EDI/ESI), the hoisted velocity j load,
// the first clamp's x87 order and the epilogue pops' place in the final minimum remain.
// Independent facing/velocity scopes 331; late turning minimum 350; branch-defined following
// factor 335; frame pointer 399; inline depth one 308 (fast). Scalar velocity 309;
// approach/following squared sums 308/308; combined radius 308; nested minimum 314 (fast). object
// receiver first 309; approach component scopes 308; following component scope 308; shared minimum
// scalar 308; library velocity copy 309 (fast control 308). conditional-body factor names 308;
// velocity field pointer 308; goal field pointer 308; speed field pointer 308; radius field
// pointer 308 (fast control 308). entry velocity workspace 307; native actor fields 307; external
// object reference 307; branch-assigned approach 352; native actor plus entry velocity 307
// (plain). Shared approach geometry 377; conditional-body entry result 308; direct turning
// fraction 321; turning workspace scope 308; conditional-body ordered minimum branches 308 (fast
// control 308).
// Round 124: Existing shared magnitude/normalizer 308/308; parallel double turning products 308;
// paired complete speed projections 308; both scalar boundaries 308 (fast control 308, full baseline 307).
// Original body and every math operation restored.
// Round 54: all four factors 1.0 at entry 333; outer/inner MIN macro 327, inner MIN with a named
// scale 206 (kept); float pow arguments 203; clamped arccosine call 269; float approach offsets 137;
// float gap with `!(gap > 0)` 128; shared normalize 117 fast, 137 plain (kept); ternary first clamp
// 304; inverse-first products 312; velocity copy j first 123; facing factor declared first 115;
// actor offset first 117 (fast; plain base 137).
// Round 245 (fast control 117): Native indexed actor fields 117; actor/current-object/both accessors
// at every use 383/383/444; radius at every use 319; all observations 425; native actor plus
// shared inner minimum 335; all observations plus shared inner minimum 381. Original receivers restored.
// Round 342 (fast control 117): Shared X/Y/XY offsets 117/129/117; shared separation 135;
// X/Y/XY plus separation 135/147/135; all with native velocity initializer 135. Original full-score 137 body restored.
// Round 59 (fast control 117): offset-first actor expression 117; actor through the actor getter 117;
// object before actor 118; speed factor declared last 117; ternary first clamp 117 (same code). The
// velocity j preload matches the shape the in-place 3-D normalize gives in matched 0x179fb0 (every
// component but the first preloaded before fsqrt); a hand-written 2-D copy of that shape 128.
// Direct speed-factor default 302; all minimum spans 117; speed/spans 302; paired closing
// projections 117; speed/projections 302; spans/projections 117; all 302; all with facing default
// first 300 (caller-inclusive fast 137). All restored; actor/object homes, velocity preload
// and the final minimum/epilogue placement remain.
// Round 403: native settings 117; readonly settings 117; native settings actor 117; readonly native
// actor 117; readonly object 117; readonly settings actor 117; readonly native receivers 117; native
// settings getter 117. Opening body restored.
// Round 39: shared planar normalizer common/separate result, either arm first 117;
// complete native input with those result forms 382 (fast control 117). Shared helper restored.
// Round 22 (one-file harness with the 0x1e5450 and arccosine files, 119 at 1326 bytes; it differs
// from the full build only in the epilogue pops): actor getter 119; object first 120; settings
// from an inline object lookup 119; tag index local 119; scaled-offset local 119; 2-D normalize
// shapes (else-zero, `s * v->c`, sum j first, `v->c * s`, early return, i or j local) identical
// code 119; velocity dot operand orders 119, `2.0f + (...)` 122, `a + (b + 2.0f)` 120.
// Ordered turn/approach, facing and following results, seven subsets
// 334/137/334/137/334/137/334; all with the staged final velocity product 334.
// All restored; the caller-inclusive unchanged preview is 137.
// Lane 15 round 18 (caller-inclusive fast, each compared with the baseline code): the actor address as one
// expression without the two one-use pool locals, identical code (kept); upper clamp `1.0 < dot`, the
// clamp through a PIN macro, a named return value, and the velocity through the file's own planar
// normalizer: identical code each (137). Retail preloads the velocity's j before fsqrt and scales i from
// memory into a reciprocal copy; the epilogue pops sit before the final minimum's branches.
// Round 29 (lane 11, fast; control 137): the whole minimum as one return expression 283 (1422 bytes: the
// macro repeats the nested minimum); a two-pointer 2-D dot helper (as in 0x1b0cd0) for the velocity dot 137,
// for the facing and velocity dots 137 (same code); facing_scale declared before turning_scale 135 (retail's
// entry store order: facing, turning, velocity, following); the velocity copy as `= { i, j }` 137. Compile-only
// listing: the 2-D normalize written out (plain or `*=` products) gives normalize2d_fast's code; retail keeps
// velocity.j loaded across the length test (`mulss j, inv`, i through a copy of inv), ours folds both; the
// non-LTCG listing places the four pops after the last merge's comiss as retail does, our LTCG build does not.
// MiniMax R (11-10): no new form, no lower score (137): ours is 1326 bytes against retail's 1324 and the
// first difference is at +0x4; what stands between the two is that retail keeps the velocity's j in a
// register across the length test and scales i through a copy of the reciprocal where we fold both
// products, and that retail's four pops sit after the last merge's comiss where our LTCG build puts
// them before; the earlier rounds' forms (the clamp shapes, the two-pointer dot helper, the velocity
// copy, the staged final product) all measure 137 or worse.
// MiniMax D (11-15): no new form, no lower score (137; the fast check reads 117): the five scale locals hold retail's
// values and the closing product multiplies the velocity scale before the speed scale as retail does, but retail stores
// the facing scale's 1.0f before the turning scale's while this build stores the turning scale first (the declaration
// order that fixes it measures 135), and the actor and the object still take retail's registers the other way round
// (retail holds the actor in EDI and the object in ESI), so the prologue is one byte shorter and every later row pairs
// with the wrong row; the actor written as the scaled offset first and the offset held in its own local both compile to
// the same code.
// MiniMax D (11-16): no new form, no lower score (120 in the fast check; ours is 1326 bytes against retail's 1324):
// retail masks the actor index and multiplies it into edx before it loads the pool, then forms the actor with
// lea edi,[edx+esi] and keeps the object in esi, while this build loads the pool first, holds the actor in esi and the
// object in edi, so the prologue is two bytes longer and every later row pairs with the wrong row; the offset-first
// expression, the scaled offset in a local of its own and the actor getter all compile to the same code, and the
// velocity's j preload belongs to the shared normalizer in include/unknown_11cc90.h, which this lane may not edit.
// Round 605. The 0x1c frame agrees. The early actor pool load and reversed actor/object registers remain.
// Shared following-point difference 128. Object-header getter 120. Named turn span 122. Following radius read
// once 266. Angle clamped before complement 120. Distinct scale minimum names 120. Positive-gap ternary 120.
// Checked following-object index 120.
// actor pool read after settings 333; actor getter after settings 333; early actor offset deferred
// pool read 333; early actor slot deferred indexed pool read 333; object receiver loaded after
// settings 120; object receiver in each use phase 117; early offset with receivers after settings 333;
// actor pool and entry data separate 333. NOT kept because phase-specific object lookups gain only
// three points and leave the two-byte excess and first difference in place. The other forms tie or
// regress.
// The index-scale lead and actor/object register exchange still differ.
// point difference in both approach paths 133; point difference and magnitude in both approach paths 117;
// shared point difference with separate minimum selection 297; shared point difference and magnitude 297;
// typed actor and point differences in each branch 133; typed actor and shared point difference 297;
// one override-speed read 122; typed actor and point difference with magnitude in each branch 117.
// NOT kept: separate approach paths with the shared magnitude gain only three points and leave the receiver split.
// M3 lane 5 r205 (fast, then plain): kept 112: the closing term through dot_product2d (round 112's form, 114 alone) and the
// final product in two steps, `scale = MIN(scale, following_scale) * velocity_scale; return scale * speed_scale;`, which
// gives retail's velocity-then-speed multiply (every one-expression product of the three, in any operand order, compiles
// speed first: `MIN(...) * velocity_scale * speed_scale`, `* speed_scale * velocity_scale`, `velocity_scale * MIN(...) *
// speed_scale` and a running `scale` minimum 114 each with the helper; grouped `speed_scale * (MIN(...) *
// velocity_scale)` 112). Not kept (statement order picked by score): the four 1.0 initialisers as one chained assignment
// `following_scale = velocity_scale = turning_scale = facing_scale = 1.0f;` gives retail's entry store order (+0x1c,
// +0x10, +0x20, +0x14), 118 alone, 110 with both kept forms; the chain the other way round 122. Same code: speed_scale
// declared after velocity_scale; the velocity dot as dot_product2d(&velocity, desired), with each product's operands
// swapped, or with the j term first; the velocity as an aggregate copy of the object's planar velocity with
// normalize2d_fast. Diagnostic: round 114's planar normalize (in/out scale helper, aggregate velocity copy, its dot through
// dot_product2d) on the 110 body gives retail's normalize block exactly at retail's 1324 B but 262: the prologue's `add
// esi, edi` is a byte shorter than retail's `lea edi, [edx + esi]`, so every later address sits off by one; the prologue
// has to be solved first. Still differing: the actor/object exchange, the normalize block (velocity j preload, `scale *
// i`), the velocity dot's i term first, the turn clamp's `fld st(0); fld 1.0; fxch`, the entry store order.
// @retail 0x1f9e70
real function_1f9e70(long actor_index, long object_index, point2f const *position,
    vector2f const *path_direction, vector2f const *facing, vector2f const *desired,
    vector2f const *next_direction, real distance, bool ignore_facing)
{
    byte *actor = g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_actor_view);
    s_slot_object_view *object = object_get(object_index);
    byte *settings = (byte *)function_1e5450(actor_index, object->tag_index);
    real speed_scale;
    real turning_scale = 1.0f;
    real facing_scale = 1.0f;
    real velocity_scale = 1.0f;
    real following_scale = 1.0f;
    speed_scale = *(real *)(actor + 0x4b4) != 0.0f ? *(real *)(actor + 0x4b4) : 1.0f;
    if (!ignore_facing)
    {
        real minimum = *(real *)(settings + 0x5c);
        real dot = facing->j * desired->j + facing->i * desired->i;
        if (dot < 0.0f)
            dot = 0.0f;
        facing_scale = (1.0f - minimum) * dot + minimum;
        vector2f velocity;
        velocity.i = object->velocity.i;
        velocity.j = object->velocity.j;
        normalize2d_fast(&velocity);
        velocity_scale = (desired->i * velocity.i + desired->j * velocity.j + 2.0f) * 0.3333333432674408f;
    }
    real radius = *(real *)(settings + 0x1c);
    if (next_direction && radius * 2.0f > distance)
    {
        real minimum = *(real *)(settings + 0x58);
        real angle = function_50650((real)PIN_126660((double)desired->j * next_direction->j +
            (double)desired->i * next_direction->i, -1.0, 1.0)) * 0.31830987334251404f;
        real fraction = 1.0f - (angle > 1.0f ? 1.0f : angle);
        real scale = (1.0f - minimum) * fraction * fraction + minimum;
        turning_scale = (1.0f - scale) * distance / (radius * 2.0f) + scale;
    }
    real approach_scale = 1.0f;
    if (!actor[0x4ae])
    {
        real x = *(real *)(actor + 0x510) - position->x;
        real y = *(real *)(actor + 0x514) - position->y;
        real remaining = (real)sqrt(y * y + x * x);
        if (radius > remaining)
        {
            real minimum = *(real *)(settings + 0x54);
            approach_scale = (1.0f - minimum) / radius * remaining + minimum;
        }
    }
    else if (*(real *)(actor + 0x4b0) != 0.0f)
    {
        real x = *(real *)(actor + 0x510) - position->x;
        real y = *(real *)(actor + 0x514) - position->y;
        real remaining = (real)sqrt(y * y + x * x);
        if (radius > remaining)
        {
            real minimum = *(real *)(actor + 0x4b0) / speed_scale;
            approach_scale = (1.0f - minimum) / radius * remaining + minimum;
        }
    }
    if (actor[0x50c] && *(short *)(actor + 0x504) == 1 && *(short *)(actor + 0x5b0) == 7 &&
        *(long *)(actor + 0x5ac) != NONE)
    {
        s_slot_object_view *other = object_get(*(long *)(actor + 0x5ac));
        real x = object->unknown030.x - other->unknown030.x;
        real y = object->unknown030.y - other->unknown030.y;
        real z = object->unknown030.z - other->unknown030.z;
        real gap = (real)sqrt(z * z + y * y + x * x) - (other->unknown03c + *(real *)(settings + 0x14));
        if (!(gap > 0.0f))
            gap = 0.0f;
        if (*(real *)(settings + 0x1c) > gap)
        {
            real closing = dot_product2d((vector2f const *)&object->velocity, path_direction) -
                dot_product2d(path_direction, (vector2f const *)&other->velocity);
            if (closing > 0.0f)
            {
                real fraction = (*(real *)(settings + 0x1c) - gap) / *(real *)(settings + 0x1c);
                fraction = fraction < 0.0f ? 0.0f : fraction > 1.0f ? 1.0f : fraction;
                following_scale = (real)pow(1.0f - fraction, closing * 0.5f);
            }
            if (gap < 2.0f)
            {
                real limit = 1.0f - (2.0f - gap) * 0.5f;
                if (following_scale > limit)
                    following_scale = limit;
            }
        }
    }
    real scale = MIN(facing_scale, MIN(turning_scale, approach_scale));
    scale = MIN(scale, following_scale) * velocity_scale;
    return scale * speed_scale;
}


#include "object_markers.h"
#include "unknown_1f9240.h"
bool function_26f2d0(point3f const *start, point3f const *end, s_path_settings const *settings);
bool function_26f360(point3f const *start, point3f const *end, s_path_settings const *settings);
bool function_26fe90(long actor_index, long object_index, real distance, s_object_marker *out, point3f *point);
bool function_26f990(long actor_index, long object_index, real distance, s_object_marker *out, point3f *point);

/* handles actor movement for transition types 3 and 5, selecting a target
   marker and either issuing the corresponding movement request or producing
   steering output toward the target. */
// @retail 0x1f7c70
bool __stdcall function_1f7c70(long actor_index, short type, bool allow_jump, vector3f *direction_out, short *type_out, vector3f *movement_out)
{
    s_actor_view *actor = actor_get(actor_index);
    long object_index = actor->unknown5ac;
    bool result = false;
    byte *character = (byte *)function_1e4a50(actor->unknown054);
    bool failed = false;
    transform4x3f animation;
    s_object_marker marker;
    point3f target;
    real minimum;
    switch (type)
    {
    case 5:
        if (!function_10f9b0(actor->unknown018, 0x5000534, 0x5000049, 3, &animation, false) ||
            !function_26fe90(actor_index, object_index, *(real *)(character + 4), &marker, &target))
            goto failed;
        minimum = 0.8660253882408142f;
        break;

    case 3:
        if (!function_10f9b0(actor->unknown018, 0x5000281, 0x5000049, 3, &animation, false) ||
            !function_26f990(actor_index, object_index, *(real *)(character + 4), &marker, &target))
            goto failed;
        minimum = 0.95f;
        break;

    default:
        goto failed;
    }

    {
        vector3f direction;
        vector3d_from_points3d(&actor->position, &target, &direction);
        direction.k = 0.0f;
        real distance = function_30bf0(&direction);

        if (distance > 0.0f &&
            animation.position.x * -1.3f > distance &&
            dot3f(&actor->unknown290, &direction) > 0.0f)
        {
            if (fabs(dot3f(&marker.matrix.left, &direction)) < minimum)
                goto failed;

            if (dot3f(&actor->unknown290, &direction) > 0.99f)
            {
                result = false;
                function_1f9450(actor_index, object_index);

                s_path_settings settings;
                function_1f9240(actor_index, &settings);

                s_unit_request request;
                bool can_move = false;

                switch (type)
                {
                case 5:
                    if (function_26f2d0(&actor->position, &target, &settings))
                    {
                        request.type = 0x2b;
                        can_move = true;
                    }
                    break;

                case 3:
                    if (function_26f360(&actor->position, &target, &settings))
                    {
                        request.type = 0x2a;
                        can_move = true;
                    }
                    break;
                }

                if (can_move)
                {
                    *(vector3f *)((byte *)&request + 0x10) = direction;
                    *(point3f *)((byte *)&request + 4) = target;
                    result = function_e6900(actor->unknown018, &request);

                    if (result)
                    {
                        actor->unknown5ac = NONE;
                        actor->unknown5b0 = NONE;
                        goto completed;
                    }
                }

                if (allow_jump && function_1f8160(actor_index, 0, &target, true))
                {
                    actor->unknown5ac = NONE;
                    actor->unknown5b0 = NONE;
                }

                if (!result)
                    failed = true;
            }
            else
            {
                *direction_out = direction;
                *type_out = 0;
                *movement_out = *g_4687a4;
                *((bool *)actor + 0x6d2) = true;
                *((bool *)actor + 0x6d1) = false;
                actor->unknown5d0 = false;
            }

completed:
            result = true;
            if (failed)
                goto failed;

            return result;
        }

        goto done;
    }

failed:
    *movement_out = *g_4687a4;
    actor = actor_get(actor_index);
    *(long *)((byte *)actor + 0x300) = object_index;
    *(short *)((byte *)actor + 0x304) = g_510c54->field_2_3 * 5;

done:
    return result;
}

struct s_pathfinding_data;
struct s_obstacle_list;
struct s_path_location;
struct s_avoidance_trace
{
    real distance;
    long sector;
    long edge;
    short obstacle;
    short group;
};
bool __stdcall function_26ccb0(s_pathfinding_data const *pathfinding, s_obstacle_list const *obstacles,
    short ignored_obstacle, point2f const *origin, long sector, long target_sector, point2f const *direction,
    real radius, real distance, bool first, bool stop_at_goal, bool ignore_flagged,
    s_path_location const *location, s_avoidance_trace *trace);
real normalize2d(point2f *v);

PRIVATE inline real movement_normalize2d(point2f *v)
{
    real length = (real)sqrt(v->x * v->x + v->y * v->y);
    if (!(fabs(length) < 0.0001f))
    {
        real inverse = 1.0f / length;
        v->x *= inverse;
        v->y *= inverse;
        return length;
    }
    return 0.0f;
}

// score 239 (was 239): The zero and radius have different XMM homes, and unsupported types use a separate return path.
// The 0x3c frame agrees. The zero/radius registers and type-arm join differ.
// M3 lane 14 r112 (fast; control 239, aligned 198, register-masked 85): retail keeps the result in XMM0 from its zero
// (`movaps xmm0, xmm1`, then the spill to +0x10) up to the type exit, which jumps past the final reload with XMM0 still
// live; this build folds that exit's value to the zero register in an exit of its own. Tried: `else goto done;` for the
// unsupported type 260 at 1173 B (aligned 191, masked 72: the zero becomes XMM1 as in retail, but the result is stored
// straight from it and every later row is one off); the result declared first 262, both 284 (the zero then comes before
// the call); one to eight dead initialised locals (diagnostic) 239/239/239/239/274/274/237/239 (five and six give
// 1173 B but masked 152; the zero stays XMM2 in all).
// MiniMax D (11-16): no new form, no lower score (238): this round compared the two bodies side by side and the delta
// is still the frame's slot map and the colouring of the vector block: this build keeps the two squared components at
// +0x28/+0x2c, the normalizer's length at +0x14 and the flag byte at +0x54, where retail keeps the components at
// +0x1c/+0x20, the length in the dead argument slot at +0x54 and the flag byte later; the products after that are
// coloured xmm2/xmm5/xmm6/xmm7 against retail's xmm1/xmm3/xmm4/xmm6/xmm7, and the length is 1182 against retail's
// 1173.
// The 0x3c frame and the direction, radius and magnitude slots agree.
// Retail keeps the zero in XMM1, copies it to XMM0 for the unsupported type and holds radius in XMM5.
// The build uses XMM2 and XMM7 and materializes the unsupported-type result separately.
// Earlier default-flag form: direction at +0x28/+0x2c, radius at +0x20 and length at +0x14
// instead of +0x1c/+0x20, +0x14 and +0x54. The direction's products also used the opposite order.
// The 0x3c frame, entry argument roles and initial direction homes agree. The first
// difference is XMM2 versus retail XMM1 for zero. The radius then occupies XMM7 versus XMM5, and the build
// threads the type-zero center around the addition and side test that retail shares with type one.
// Earlier score 239 (was 238): retail keeps the direction at +0x1c/+0x20, the radius at
// +0x14 and the normalizer's length in the dead argument slot at +0x54, where this build keeps the direction at
// +0x28/+0x2c, the radius at +0x20 and the length at +0x14; it also zeroes in xmm2 where retail zeroes in
// xmm1 and multiplies the direction's y first where retail multiplies the x first.
// MiniMax C (11-14): no new form, no lower score (238): with the side test written as a flag
// (bool left = <dot> > 0.0f) every stack home lines up with retail (the direction at +0x1c/+0x20, the
// radius at +0x14, the length in the dead origin slot at +0x54) but it scores 239, because the flag stays
// live from the test while retail materialises it at the comparison; with the flag in its original form the
// only difference left is the SSE allocation: retail zeroes with xorps xmm1 and movaps xmm0,xmm1 so the
// 0.0f result stays in xmm0, this build keeps it in xmm2.
// MiniMax A (11-13): no new form, no lower score (238): retail's and our prologue agree byte for byte
// (the actor index in ECX, the object index in EAX, the target in EDX and the type in the first stack
// slot), so the argument order is not what differs; what differs is the vector arithmetic's allocation
// (retail keeps the zero in xmm1 and the direction's components at +0x1c and +0x20, ours keeps the zero
// in xmm2 and the components at +0x28 and +0x2c), and retail still materialises the side flag only at the
// comparison where this build stores it at the top and keeps it live from there.
// MiniMax G (11-12): no new form, no lower score (238): the side vector written as two scalars instead of a point2f compiles to the same bytes.
// MiniMax D (11-10): no new form, no lower score (238); our frame puts the two saved values at
// [esp + 0x28] and [esp + 0x2c] where retail has [esp + 0x1c] and [esp + 0x20], and retail materialises
// the side flag only at the comparison while this build stores 1 at the top.
// MiniMax S (11-10): no lower score (238): dropping the pointer-to-parameter origin copies gives 255 (worse) and is reverted; retail still materialises the side flag only at the comparison (store 1, ja, store 0) while this build stores 1 at the top and keeps the flag live from there.
// MiniMax C (11-09): no new form, no lower score (238); retail materialises the side flag only at the comparison (store 1, ja, store 0) while this build stores 1 at the top and keeps the flag live from there.
// MiniMax G (11-06): no new form, no lower score; retail keeps the normalizer length in the dead target
// argument slot at [esp+0x54] and the radius at [esp+0x14], this build uses fresh local slots, and retail's
// zero register is XMM1 where this build uses XMM2.
// MiniMax R (11-05): no new form, no lower score; the plain origin copy 278 and the flipped side test 239;
// retail keeps its length in the dead origin argument slot at [esp+0x54] and its radius at [esp+0x14].
// MiniMax H (11-04): the left flag declared just before its comparison 287; not kept, the
// original body is restored.
// MiniMax R (10-31): origin read directly 255, plain origin copy 278, inlined direction normalization 272; permuter found no better variant; no lower score.
// MiniMax C (11-09): no new form, no lower score (238); retail keeps the radius at +0x14 and the length in the
// dead origin slot at +0x54 while this build keeps them at +0x20 and +0x14, so the later rows pair wrongly.
// MiniMax D (11-14): no new form, no lower score (238): an earlier round's named float local for the
// entry's +0x14 value at the call is recorded at 217, but that local holds a value used once and is kept
// only with a byte match, so the body stands; the permuter's 23 tries over the reduced image
// (with --allow-mismatch, since the reduced score 254 differs from the full 231) found nothing below the
// baseline either.
// Earlier score 238 (was 238): Frame 0x3c, result/flag homes and exits agree (1182 bytes against 1173). Retail's zero, radius and
// direction sit in other SSE registers (xmm1/xmm5/xmm2-3; it keeps the result in xmm0 up to the type exit),
// shares the left type-0 arm's `+ target.y` and flag re-test with the type-1 arm (ours threads that arm),
// spills the lateral term to the type slot, and puts radius/desired, then direction, below the
// difference/transformed-origin slot.
// Round 16: retail's result sits at [esp+0x10] and its radius at [esp+0x14], ours the other way
// round, and retail materialises both direction components before the x87 length.
// With ordinary input/side lifetimes: persistent target 313, both input points 297, shared
// arccosine 296, late radius 312, frame pointer 401. Ordinary lifetimes: facing value 307; scalar
// side 309; scalar center 306; origin constructed first 306; reused negative radius 329. Ordinary
// input/side lifetimes: point pair plus arc helper 344; pair plus direction output 299;
// origin/facing values 292 (plain 292, two-point gain rejected); target/facing values 306; three
// point values 328. ordinary inputs/side with early 3D origin 291; entry trace 306; reused
// threshold 306; separate centers 306; early origin plus centers 291 (fast). Ordinary input/side
// lifetimes: external origin reference 311; target reference 306; facing reference 306; all three
// references 305; full-width side state 301 (fast). Sector before transforms 306; origin transform
// first 306; pathfinding result first 306; late result on ordinary convention 307; redundant upper
// clamp removed 318 (fast).
// Round 54: float magnitudes 295; plain side flag 315; per-arm centre 294; shared point difference
// with float magnitudes and per-arm centre 278 (kept); with plain flag 301; result/radius renames 298;
// trace-hit single exit (goto or nested if) 278, with plain flag 275; negated early return 288;
// facing reference 302; difference through the point-difference helper 278/275 (plain flag).
// Round 218, recovered direction graph with ordinary inputs/side: radius at each use
// 274 at retail length (gain four rejected); negative radius 295; reused threshold 302;
// negative/field radius with threshold 295/279; direct side result alone/with negative
// radius/with negative and threshold 302/295/295. Actual trace common exit with field/
// negative radius 278/299. All restored; zero/radius registers and side threading remain.
// Round 152: Ordinary origin/side with symmetric lateral/forward x products 285, y products 302, all products 302; complete dot terms 302, with x/y/all products 285/302/302; complete terms with shared trace exit 276 (1175 bytes). Two-point exit gain rejected; original body restored.
// Round 221: common radial-offset target addition 281/278/292/291;
// target-seeded center 319/303/315/299. Each group varies early
// side-radius and common trace exit on ordinary inputs/side; all restored.
// Round 347: ordinary origin with direct byte/short left state 302/305, Boolean/byte right 281/281;
// complete facing scalar observations with those four states 284/298/300/300.
// Last two reach 1173 bytes, but input/SSE register ownership regresses; original body restored.
// Round 61 (lane 7): plain flag with the not-left arm assigning y first 306; trace hit through a
// `blocked` result plus a negated direct return 288, with a plain flag 287; radius declared before
// the result 278; plain flag declared first 269 (kept), then declared just before the result 262
// (kept: flag in the origin slot as retail), declared there without a value 301; trace hit by one
// `goto done` plus `if (left) return 0.0f - result;` 252 at 1173 bytes (kept); type exit also by
// goto 264; transformed origin declared first 252; result declared first 275; trace_origin/
// trace_direction names 252; origin read directly 267.
// Round 151: Existing normalizer value/pointer inputs with output pointer 348/302, returned point 359/359, scalar input/output or return 319/298; void/value and returned/value movement contracts 350/339. All ordinary origin/side forms restored.
// Frame 0x3c and original homes agree; zero/radius registers, facing transport and type-zero continuation remain.
// Round 63: the direction normalized by the file's existing movement normalizer (identical code; the
// duplicate forceinline normalizer removed) 252, kept; the side offset as if/else arms with the products
// in each arm 263 (aligned 217 -> 197: zero in xmm1 and the left type-zero arm joining the type-one tail
// as retail, but the flag's `true` stays at entry, one line ahead of retail's); with the flag declared
// or assigned at the side test 293/293 (flag then in the facing slot, result in the origin slot); with
// that and the result declared first 303.
// Round 26 (lane 9): new names for the stack points (desired, direction, difference, 3D origin) 252
// each, no home moves; flag declared at the side test 287 (direction then at retail's 0x1c, but
// result takes the origin slot, length the facing slot and facing moves to ebp); with result/radius/
// flag renamed 287 each; with origin read directly 288; with facing read once through its address 287.
// Round 31 (lane 12): retail stores the flag's `true` after the inlined normalization, whose length
// takes the origin slot first; the flag declared after the normalization 287 (direction in retail's
// slots, but the result takes the origin slot and facing EBP), with the result declared there too 296;
// radius read at each use 277 (1165 B); not-left type-zero arm assigning y first 285; type exit
// returning 0.0f 252 (identical).
// Round 10 (lane 11): one signed offset for the type-zero arm 293 (1140 bytes); `return 0.0f` at the type
// exit 252 (same code); direction renamed to sort last 252 (same code); flag declared after the
// normalization 287; side dot product with the side terms first 252 (same code).
// Round 28: Ordinary origin with eight consistent native parameter orders 272 each;
// register inputs before/between/after ordered type/origin/facing/obstacles, 1173 or 1164 bytes.
// Original definition, declarations and both callers restored; early flag/normalization homes remain.
// Ordinary origin with existing 2D product producer across paired projections,
// cosine and side classification 267 in all seven combinations; complete products
// plus side-offset arms 278. Exact length throughout; original body restored.
// Round 225: ordinary origin with fraction branches, desired-point subtraction and paired
// 3D setters, seven nonempty combinations 267/295/295/267/267/295/295;
// all producers with native long side 289. Original body restored; ordinary input
// and steering producer forms do not recover the zero/radius or initial homes.
// Round 72 (lane 8): kept 238: the pathfinding pointer preset to NULL and set by an `if` (retail's
// `xor eax, eax` before the count test) with the clamp and arccosine through the existing 0x50650
// (inlined as retail; the hand-written copy removed). The preset alone 252 (block now as retail),
// the helper alone 263. Retail also pushes the entry's +0x14 value through a stack slot: a named
// float local for it at the call 217 (argument block and both exits then as retail, 1178 B), with
// the helper 252, declared before the sector 242 (one-use local, not kept); cosine dot y-first 238;
// the type exit as `goto done` 251 (1173 B).
// Round 412: Ordinary origin with entry getter inline 255; origin/direction/both transform inline
// 274 each; getter with origin/direction/both inline 274 each; noinline getter with both
// transforms inline 274. All body and declarations restored.
// Ordinary origin on the shared arc/trace graph, late side flag, early zero result and
// both 255/288/262/292; each with existing normalizer force-inline 255/288/262/292.
// All ordinary sources move the entry homes and regress; original body/helper restored.
// Ordinary origin with paired radial/side/desired product stages, seven subsets 255 each;
// all with the complete scalar fraction producer 255. All restored.
// Round 444: Ordinary origin with zero result after type/side/projection geometry
// 292/284/284; assigned-sign counterparts 292/281/281; forward-scoped result with
// direct/assigned sign 300/300. All restored; zero/radius and vector homes remain.
// Round 208: Forward-before-lateral projections with ordinary origin, y-first type/side center pairs
// and origin-workspace-first masks 0..7: 257/286/286/279/257/286/286/279. Original projections, row
// stores and complete body restored; natural input convention, zero/radius and direction homes
// remain.
// Round 38 (lane 1): the side offset as if/else arms with the products in each arm 270 (aligned
// 215 -> 194: zero in xmm1 and the type-zero arms closer, entry homes unchanged); with the flag
// declared after the normalization 305 (1188 B); radius renamed to sort before the stack points 238,
// `center` declared beside `direction` 238, radius declared before the flag and result 238 (same code).
// Round 216: Early origin/target/both/facing/origin-facing/target-facing/all point values before lookup 322/311/346/331/351/347/380; all with entry result 380. All restored; zero/radius and native input/vector homes remain.
// Round 19 (lane 18): retail keeps the 0.0f result in xmm0 from entry (`movaps xmm0, xmm1`) and its
// type exit jumps past the epilogue's reload, so the normalization uses xmm2/xmm3 and the radius
// xmm5; retail's fld qword 1.0 confirms the double clamp. The flag as `bool left = <side dot> > 0.0f;`
// at the side test (VC compiles it as retail's store-1/ja/store-0 after the comiss) 239 (aligned
// 215 -> 198: length in the origin slot and direction at 0x1c/0x20 as retail), with the side-offset
// if/else arms 279 (aligned 175, the best structure so far: lateral spill to the type slot, the
// 0x26ccb0 argument order and the zero exit remain); on the first: result declared after the radius
// 239, `return 0.0f` at the type exit 239, result declared before the entry lookup 262. The volatile
// origin read removed 255 (0x29cbb0 300 -> 319). All restored.
// N271: existing normalizer single-result entry-default/exhaustive/reused-magnitude/acceptance
// graphs, then caller assigned-sign completion: 273/255/275/254; 255/260/265/259.
// All ordinary-origin forms restored with the original helper; zero/radius and vector homes remain.
// MiniMax R (11-10): no new form, no lower score (238), and the difference is reported rather than
// repaired: retail's own call site (0x29a7fc) puts the actor in ecx, the object in eax, the target in
// edx (`lea edx, [eax + ebp + 0x548]`) and the tag type as the first stack argument (`push eax` after
// `sete al`), so retail's declaration is (actor_index, object_index, target, type, origin, facing,
// obstacles), while this repository's declaration has the type and the target swapped and so puts our
// type in edx and our target at [esp+4]; the two stack slots and the frame size otherwise agree with
// retail's, and changing the parameter order is not a form this round may use. The left/right direction
// swap (239), the swap with the shared origin local (289) and the swap with the operands exchanged
// (291) all score worse than the 238 they start from, so the body was restored.
// MiniMax D (11-15): no new form, no lower score (238): the permuter's five tries over the reduced image scored 231 at
// the baseline and none below it, and the direction swap (239), the swap with the shared origin local (289) and the
// swapped operands (291) still measure worse, so the body stands; the parameter order retail uses is the one the note
// below reports and is left alone.
// MiniMax D (11-16): no new form, no lower score (238 in the fast check): the parameter order retail's own call site
// uses (the actor in ecx, the object in eax, the target in edx and the tag type as the first stack argument) is the one
// the note below reports; the declaration is shared with four other files, so it stays as it is, and the direction swap,
// the swap with the shared origin local and the swapped operands still measure worse than the 238 they start from.
// b7J (11-17) walk against retail, fast build on r830: went statement by statement through every block (entry and
// radius, direction, side test, type 0/1 centres, lateral/forward, fraction, desired point, both transforms, trace
// call, cosine and arccosine, exits) and every field, offset, jcc sense, argument and constant (0x45e238 0.11,
// 0x45dff0 1.3, 0x45e2f8 1.19, 0x43ff08 1.5, 0x45dbdc 1e-4, 0x45dbc0 1.0, 0x45dbcc -1.0, qword 0x45dcc0 1.0): no
// behaviour difference found; the dot terms add in retail's order (0x1f9b4d, 0x1f9b5a, 0x1f9b81, 0x1f9b91). Changed,
// same behaviour: the side flag is `bool left = <dot> > 0.0f;` instead of a flag set true and then overridden (retail
// stores 1, ja, stores 0 at 0x1f9ac4/0x1f9acb after the comiss at 0x1f9abb, which VC emits for this form) 238 -> 239.
// Not kept: the first cosine clamp through the file's MIN macro, `MIN(cosine, 1.0)` 224 (the macro compares in double
// where retail compares the float with comiss at 0x1f9de5 before its fld qword 1.0 at 0x1f9df0, so the written-out
// clamp is retail's form), `MIN(cosine, 1.0f)` 264; without the volatile origin read 273 at 1173 bytes. 0x29a190
// stays MATCH and 0x29cbb0 378.
// F417: Shared radial axis and center member/aggregate/in-place scaling 296/296/299;
// per-type offsets and member/aggregate translation 288/288; shared difference helper 296;
// signed scale at type join 310; center as scaled offset then translation 299.
// NOT kept: the zero/radius registers remain and the scores regress. Original body restored.
// Round 625: direction before result and radius 264; radius then direction then result 239;
// direction between result and radius 264, with direct initialization 260. Named squared length 239,
// float square root 240, separate squared components 240, small-length early exit 240.
// Single return with in-place sign 255, conditional sign assignment 275, explicit sign arms 275.
// Restored. The XMM zero and radius allocation and separate invalid-type epilogue remain.
// Round 628: type switch with 0/1/default, 1/0/default and default/0/1 gives 284 each;
// switch literal zero 284, ordinary origin 299, single exit 254, shared Y translation 299.
// The existing difference helper scores 239 in the original dispatch and 284 in the switch.
// The matching entry and frame still diverge at the XMM2 zero and unsupported-type exit.
// shared type zero point offset output 239; shared both type point offset output 239; shared both type point
// offset value 360; shared point offset output for center and side 239; shared point offset value for center
// and side 389; shared scaled direction for centers 399; shared scaled direction for centers and side 458;
// shared scaled direction for type zero and side 407. NOT kept: the shared output-pointer forms keep the
// original score and extent. Returning the point or materializing a scaled direction lengthens the body and
// increases the score. The native zero, radius and center-join allocation is unresolved. The opening
// executable source is restored.
// Tried reject the unsupported type before choosing its geometry 265, both supported types enclose the
// geometric result 286, reject insufficient forward distance before calculating the arc 283, trace
// rejection is assigned before the approved arc block 239, blocked trace returns the default arc
// immediately 271, approved arc returns its signed selection directly 284, projection components also
// supply the desired point and trace origin 284, native trace point also supplies the planar projection
// 238.
// NOT kept: The trace-origin point recovers retail length and gains one. It samples the height and stages
// the planar input ahead of the retail trace.
// Round 182 (lane 7, fast; nothing kept, every form raises the count while the xmm ranks stay off): retail returns in
// XMM0 and holds the result there from `result = 0.0f` (`movaps xmm0, xmm1`, zero in XMM1) up to the type exit, which
// jumps into the shared exit past the reload at 0x1f9e55; the short-forward, trace-hit and not-left exits share that
// reload, so retail has one `return result;` and never treats the result as a known zero. Retail's radius argument to
// the trace goes through a stack temporary (0x1f9d91, 0x1f9d99), the mark of a by-value float parameter of an inlined
// forwarding function, and the left type-zero arm jumps to the type-one arm's `+ target.y` and re-tests the flag
// (0x1f9c2f) where this build threads it. Counts below; "shape" is the aligned count with xmm numbers masked (the
// kept body is 98, aligned 198).
// Type exit by `goto done` 260 (1173 B, aligned 191; result only in memory); with `if (left) result = 0.0f - result;`
// and one return 255 (1187 B; the type exit gets its own `movaps xmm0, xmm2`); side offset as if/else arms with the
// products in each arm 279 (aligned 175, shape 64); arms with the goto 280; arms with the goto and one return 255
// (1185 B, shape 63: entry is then retail's `movaps xmm0, xmm1` and store from XMM0, radius XMM4 against XMM5 and the
// direction XMM6/XMM2 against XMM2/XMM3); result declared after the radius on the arms 279; that single-exit form with
// the trace through a forceinline forwarding function taking its arguments by value 265 (1193 B, aligned 192, shape 47:
// the argument block is then retail's), with one more unread parameter 265, with the pathfinding pointer from an inline
// getter 265 (both the same code); a dead-local scan K = 0..31 on it 265 at every K (diagnostic; the xmm ranks do not
// follow the symbol count here); origin read directly on it 282 (the side test's loads become retail's, but the target
// takes EDI and the origin EBX); the whole body after the lookups as an inline function 292 (1131 B); the type-zero
// centre through one signed radius 260 (1173 B, shape 130); product-first centre expressions with the not-left arm
// assigning y first 261 (aligned 158, shape 50); the forwarding function alone on the kept body 252 (1190 B), with
// one return 243, with the goto and one return 263. Open: what gives the radius XMM5 and the direction XMM2/XMM3 once
// the result holds XMM0, and why retail does not thread the left type-zero arm.
// M3 lane 2 r108 (fast; control 239): the direction normalized by the shared header's 2-D normalizer (through a
// vector2f cast, as unit_object_type.cpp does) 239 (same code), the desired point too 240; ten locals renamed to sort
// first (result, radius, direction, side, left, center, perpendicular, difference, side_radius, lateral) 239 each;
// const radius/side/flag/perpendicular 239; the side offset as if/else arms with a named negative radius plus the
// shared normalizer 287 (1193 B). Reading: retail's left type-zero arm shares the type-one arm's last instruction
// (`addss xmm0, [ebx + 4]`, center.y in XMM0 in both), so the tail merge happens before threading; ours gives the two
// arms' center.y different registers, so nothing merges and the arm is threaded: one register-allocation cause.
// @retail 0x1f99d0
real function_1f99d0(long actor_index, long object_index, short type, point2f const *target,
    point2f const *origin, point2f const *facing, s_obstacle_list const *obstacles)
{
    point2f const *volatile *local_0 = &origin;
    point2f const *local_1 = *local_0;
    s_actor_view *actor = actor_get(actor_index);
    byte *entry = (byte *)function_1e5450(actor_index, object_get(object_index)->tag_index);
    real result = 0.0f;
    real radius = *(real *)(entry + 0x28);
    point2f direction;
    vector2d_from_points2d(local_1, target, &direction);
    movement_normalize2d(&direction);
    point2f side = { 0.0f - direction.y, direction.x };
    bool left = facing->y * side.y + facing->x * side.x > 0.0f;
    point2f center;
    if (type == 0)
    {
        point2f perpendicular = { 0.0f - facing->y, facing->x };
        if (left)
        {
            center.x = target->x + perpendicular.x * radius;
            center.y = target->y + perpendicular.y * radius;
        }
        else
        {
            center.x = target->x + perpendicular.x * (0.0f - radius);
            center.y = target->y + perpendicular.y * (0.0f - radius);
        }
    }
    else if (type == 1)
    {
        point2f difference = { direction.x - facing->x, direction.y - facing->y };
        normalize2d(&difference);
        center.x = target->x + difference.x * (0.0f - radius);
        center.y = target->y + difference.y * (0.0f - radius);
    }
    else return result;
    real side_radius = left ? 0.0f - radius : radius;
    center.x += side.x * side_radius;
    center.y += side.y * side_radius;
    real lateral = center.y * side.y + center.x * side.x - (local_1->y * side.y + local_1->x * side.x);
    real forward = center.y * direction.y + center.x * direction.x - (local_1->x * direction.x + local_1->y * direction.y);
    if (forward > radius * 0.11f)
    {
        real fraction = forward > radius * 1.3f ? 1.0f : (forward - radius * 0.11f) / (radius * 1.189999938f);
        real offset = (fraction * 1.5f - 1.0f) * lateral;
        point2f desired = { center.x + offset * side.x - local_1->x, center.y + offset * side.y - local_1->y };
        real distance = movement_normalize2d(&desired);
        vector3f local_direction = { desired.x, desired.y, 0.0f };
        point3f local_origin = { local_1->x, local_1->y, actor->position.z };
        function_210770(actor->unknown27c.point.output_index, &local_direction, &local_direction);
        function_210690(actor->unknown27c.point.output_index, &local_origin, &local_origin);
        long sector = *(long *)((byte *)actor + 0x28c);
        if (sector != NONE)
        {
            byte *structure = (byte *)g_4e0348;
            s_pathfinding_data *pathfinding = NULL;
            if (*(long *)(structure + 0xc4) > 0)
                pathfinding = *(s_pathfinding_data **)(structure + 0xc8);
            s_avoidance_trace trace;
            if (function_26ccb0(pathfinding, obstacles, NONE, (point2f *)&local_origin, sector, NONE,
                (point2f *)&local_direction, *(real *)(entry + 0x14), distance, false, false, false, 0, &trace))
                goto done;
        }
        real cosine = desired.x * direction.x + desired.y * direction.y;
        cosine = (real)(cosine > 1.0f ? 1.0 : cosine);
        result = function_50650(cosine);
        if (left) return 0.0f - result;
    }
done:
    return result;
}

struct s_collision_result_1697c0;
struct s_obstacle_collision
{
    long type;
    real fraction;
    byte field_8[0x24 - 8];
    short material;
    byte field_26[0x5c - 0x26];
};
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
    long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);
void function_caf90(long unit_index, point3f *position);
bool function_29d6c0(vector3f *direction, s_reference reference);
real normalize2d(point2f *vector);
long function_baf80(long object_index);

PRIVATE __forceinline real function_1f9581(point2f const *arg_0, point2f const *arg_1)
{
    real local_0 = 0.0f;
    local_0 = arg_0->x * arg_1->x + arg_0->y * arg_1->y;
    return local_0;
}

// @retail 0x1f9580
bool function_1f9580(long actor_index, s_reference reference)
{
    (void)&reference;
    s_actor_moving *actor = actor_moving_get(actor_index);
    s_type_c3b527 *target = (s_type_c3b527 *)function_262b40(reference);
    bool result = false;
    if (target && (*(word *)((byte *)target + 0xe) & 0x20) && actor->unit_index != NONE)
    {
        point3f origin, destination;
        function_caf90(actor->unit_index, &origin);
        function_210850(target, &destination);
        vector3f local_0;
        vector3d_from_points3d(&destination, &origin, &local_0);
        real distance_squared = local_0.i * local_0.i + local_0.j * local_0.j + local_0.k * local_0.k;
        if (distance_squared < 12.25f)
        {
            vector3f facing;
            function_29d6c0(&facing, reference);
            point2f direction;
            direction.x = facing.i;
            direction.y = facing.j;
            if (normalize2d(&direction) > 0.0f)
            {
                point2f local_1;
                local_1.x = origin.x - destination.x;
                local_1.y = origin.y - destination.y;
                if (function_1f9581(&direction, &local_1) > 1.2f && actor->unit_index != NONE)
                {
                    s_obstacle_collision collision;
                    collision.material = NONE;
                    vector3f ray;
                    vector3d_from_points3d(&origin, &destination, &ray);
                    if (!function_1697c0(0x1808c2d, &origin, &ray, function_baf80(actor->unit_index),
                        NONE, (s_collision_result_1697c0 *)&collision) || collision.fraction >= 1.0f ||
                        (1.0f-collision.fraction)*(1.0f-collision.fraction)*distance_squared < 0.1f)
                        result = true;
                }
            }
        }
    }
    return result;
}
