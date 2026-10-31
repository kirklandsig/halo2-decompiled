// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1F5560.CPP: small accessors of an actor's movement state */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1e3920.h"
#include "slot_handler.h"
#include "unknown_2626b0.h"
#include "unit_requests.h"
#include "props.h"
#include "unknown_0259d0.h"
#include "unknown_11cc90.h"
#include <string.h>
#include <math.h>
#include <float.h>
#include "unknown_26c380.h"

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

/* takes the pending facing (+0x622) unless the actor rides a vehicle */
// @retail 0x1f5560
bool function_1f5560(long actor_index, vector3f *facing)
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
		long state = heading_unit_get_state(unit);

		if ((short)state == 6 || (short)state == 7)
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
			long first;
			long second;

			function_10f630(actor->unit_index, &first, &second);
			if (unit->movement_type == 5 || (short)state == 5 || first == 0x50000cb)
			{
				direction->i = unit->forward.i * -1.0f;
				direction->j = unit->forward.j * -1.0f;
				direction->k = unit->forward.k * -1.0f;
				result = true;
			}
			else if (actor->unknown264 && g_510c54->game_time < actor->unknown634)
			{
				point3f target;

				function_210850(&actor->unknown638, &target);
				direction->i = target.x - actor->position.x;
				direction->j = target.y - actor->position.y;
				direction->k = target.z - actor->position.z;
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
// @retail 0x1f4f40
bool function_1f4f40(long actor_index, vector3f const *facing, short unknown, s_type_c3b527 const *point, bool face_prop)
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

// score 10 (was 12): the first dot product takes the projection vector first; left: the two dot
// products evaluate their j and k terms in opposite orders (ours j,k / k,j; retail k,j / j,k) and the
// second dot's i term keeps the projection vector in the register where retail has the point.
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
// @retail 0x1f9760
bool function_1f9760(long actor_index, long object_index, point3f const *point, real distance,
	bool *last_out, bool *direction_valid_out, vector3f *direction_out)
{
	bool reached = false;
	s_actor_view *actor = actor_get(actor_index);
	s_1f9762 *path = (s_1f9762 *)((byte *)actor + 0x50c);
	byte *definition = g_4e3b44[moving_object_get(object_index)->tag_index & 0xffff].bytes;
	bool last = path->field_2e + 1 >= path->field_2d;
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
			s_type_c3b527 *previous = path->field_2e > 0 ? &path->field_30[path->field_2e - 1].point : (s_type_c3b527 *)path->field_1c;
			function_210be0(previous, &path->field_30[path->field_2e].point, &approach);
			if (function_30bf0(&approach) > 0.0f)
				projection_direction = &approach;
		}
	}
	else if (!reached && direction_valid && actor->unknown4d4)
		projection_direction = &direction;
	if (projection_direction)
	{
		real remaining = dot3f(projection_direction, (vector3f *)&path->field_30[path->field_2e].point.point) - dot3f((vector3f const *)point, projection_direction);
		if (remaining < *(real *)(definition + 4) * 1.2f)
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

// score 135 (was 135): frame 0x48 agrees and the code matches retail to the first trajectory call;
// retail then caches the trajectory result in DL (spilled around calls, `mov al, dl` on the short
// exits, no store after the request) where ours keeps it only on the stack, so the unit-state table
// goes to EDX not ESI and the first facing store loads EAX/ECX; retail also returns the validity
// flag (`mov al, bl`) where ours folds that exit to `xor al, al`, 12 bytes short in all.
// The trajectory helper uses retail ret 0x2c; the older extra-EDX blocker is absent.
// frame 0x48, entry initialization and trajectory convention agree.
// The found-type scan has an extra bound test; trajectory result registers, retry scalar homes,
// object/query allocation and separate movement-return tails still differ.
// Forms tried: search then selection 172; unsigned scan 178; fixed do/while 191; types arm first 191;
// fallback setters 195; split plus unsigned scan 223; signed-long search 192; signed-byte search 188;
// exact exhaustion 210; named first normalization result 172; low-bit presence guard 228; native type
// clamp 236; branch-defined chosen speed 220; target snapshot 234; bounded for search 172; unsigned
// byte scan 233; chosen speed after query 190; planar component stores 265; native masks/metadata
// snapshot 172 each; retry requirement 196; first type from bits 277; validity initialized in typed arm
// 241; first type initialized by branch 198; read-only metadata 172; read-only speed tables 172;
// vertical output branches 187; shared movement flag stores 174; retry speed first 172; native planar
// output copies 172; query inline/noinline declarations 174/172; byte/word validity 172/170; byte/word
// first-type state 172/208; decisions after query 177; both byte decisions 172; separate retry force
// 196; long first-type result 235; minimum after speed query 228; first type after speed query 175;
// shared movement destination 172; native request copy 172; earlier result declaration 172; minimum
// and retry force 227; retry/movement/both threshold products 172 each; clamped upward result 172, with
// both products 172; retry/movement/both eligibility results 172 each; validation after query 220, with
// late minimum 220; early query output 171; minimum first 174; early output/minimum 170; first-type
// first 172; shared Boolean initialization 173; early output with late validation 218; fast-byte memset
// 172; actual actor/requested-type/maximum-type references 172 each; actual destination/origin
// references 172 each; read-only unit view 172; native two-component normalization workspace 206; both
// lookup globals at each use 172; request point setter 175; entry direction workspace 172;
// globals/setter 175; globals/workspace 172; setter/workspace 175; all 175; word trajectory result 211;
// initial/retry/both scalar outputs at method entry 172 each; retry outputs at valid scope 172; first
// entry plus retry valid 172; both entry with false validity return 172; retry valid with false
// validity return 172; false validity return alone 172; complementary validation/base-check/trajectory/
// validation-base/validation-trajectory/base-trajectory/all/byte trajectory 217/173/223/172/212/226/
// 225/223; post-query force OR/guarded/conditional/complete arms 303/197/172/273; exact search bound
// 211; mask-first search 228; complete requested-byte snapshot 240; snapshot plus force OR 300; shared
// validation/query result 172; two-use unit index 172; native origin value 229; result+index 172;
// result+origin 229; index+origin 229; all 229; complete origin memcpy 229; word validity with early
// query, early query/minimum, post-query first type and all three 169/169/173/174 (best at 935 bytes);
// byte validity with the same four 171/171/175/177; global optimization off 542; inline depth zero 231.
// Round 61: selection inside a `for` scan with `break` 191; the flag declared first 171; declarations
// in retail's store order (actor, minimum, flag, validity, first type) 168, kept; then the scan with
// the selection and `break` inside 183 (matches retail up to the first 0xfa1a0 call at +0x111; the
// six saved bytes shift the rest); retry outputs declared horizontal first 168; first normalization
// tested `> 0.0f` with an early store-return 215 (890 B, our stores merge), in if/else arms 215;
// `base_speed * 1.2f >= chosen_speed` early return 212 (retail's jae polarity).
// Round 151: two-use retry success with logical/bitwise/reversed logical joins before/after publication
// 216/239/218/254/215/243; promoted long/word result joins 200/203. Native vector setter for planar
// zeros/actor fallback/default axis 172/218/187; zero-actor/zero-axis/actor-axis/all 218/187/211/211; all
// plus request setter 222.
// Round 63: validity and trajectory result in one success variable 229 (908 B); `!(base_speed * 1.2f >=
// chosen_speed)` (retail's comiss/jae) 167; `return valid;` on the invalid path 168 (folded to xor al, al);
// retry outputs assigned horizontal first 168 (retail's slot sharing with the speed, aligned 83 -> 79);
// those two with the `for` scan and break 183 (aligned 75; matches retail through +0x111).
// Round 31 (lane 12): first normalization as `!(... > 0.0f)` 167; with `!(base_speed * 1.2f >= chosen_speed)`
// 166; then the success store as the `if` arm 192 (942 B); the scan with selection and break on that graph
// 183 (matches through +0x111); 0xfa1a0 time scale read once into a value 166 (0xfa1a0 262, still EDX).
// Round 10 (lane 11): both of those with `return valid` 167; with a plain `for`/`break` scan 167; with a
// bottom-tested scan 207; with the selection inside the scan 183; retry vertical renamed to sort first 168.
// Round 236: native request/movement/selected-type result and all combinations, then native table
// fields 171 each; all restored.
// Round 162, with the corrected trajectory convention: found-bit selection, both output-pair orders,
// false-validity return, overall result and the combined output/result form 174 each; early first-type
// 230; byte/word trajectory success 189/177. Coordinated trajectory inline declarations on the
// original/found-bit graphs 171/174. All bodies, call tuples and outside declarations restored.
// Round 224: Native initial/retry speed pairs with full-width result, seven subsets
// 174/172/175/199/199/202/202; both pairs with enclosing retry scope 175. No match; all pair
// storage, result width and body restored.
// Round 224 continuation: Native planar normalization/output workspace on the corrected trajectory convention, crossed with
// earlier result, entry planar declaration and complete memcpy publication, eight forms 267 each.
// Original 3D output, planar uses, declarations, comparisons and full body restored.
// Round 72 (lane 8): kept 135: the `for` scan with the selection and `break` inside, the facing
// fallback in its own `vector3f facing` local (retail's separate first-success store and shared
// facing store, default-axis store with the dead k store), retry outputs assigned horizontal first,
// and the request result assigned to `result` in an `if`/`else if` with the planar arm. Tried on the
// way: one result for validity and trajectory with a single exit 284, with the original returns 171
// (invalid exit still folded), with the new tail 182/236/243; inline unit-state helper call 171
// (same code); explicit returns in both facing arms 206 (result takes EBX, target leaves it), in one
// arm 135; typed or per-field request point 135/199; `result` declared at the top, before validity
// or before the actor 135 each; fast-to-force inside the valid block 246; `return valid;` 135 (folded);
// the original while scan on this graph 204 (935 B); `!(base_speed * 1.2f >= chosen_speed)` 135 (gives
// retail's comiss/jae, same count).
// Round 412: Native unit-state accessor 135; fallback-vector setters 192; joined two-use retry
// result 213; accessor/setters 192; accessor/retry 213; setters/retry 187; all three 187; all with
// short result 213. All restored.
// Early trajectory-failure exit on the retained facing graph, bool/byte/long/short result
// 135/186/151/159; each with direct request return 182/193/173/153. All restored.
// Separate trajectory admission/final result, bool/byte/word/long carrier 199/157/149/150;
// each with the final result declared before outputs 199/157/149/150. All restored.
// Round 430: One selected-speed lifetime through retry, with entry result declaration, valid-scope
// retry outputs and both native axis-pair names: all eight 204 at 917/935 bytes. Original two speed
// lifetimes and complete body restored.
// Lane 15 round 18: retail's trajectory result is a spilled register candidate (DL copy, store after the
// force test, DL reloaded after the retry and returned by a shared `mov al, dl` exit; the request arm
// returns its call's AL); ours keeps it memory-homed (immediate store, dead store before the request
// return, folded false exit). Request result returned from its arm 182 (919 B); early false return after
// the retry 135; retry admission and call joined by && 135; unit index local 135; validity block
// flattened into an early false return 232. All restored.
// Lane 16 r46: `return valid;` folds to `xor al, al` (VC propagates the branch value), so retail's
// `mov al, bl` returns a value it cannot prove zero. `bool result = valid;` with one exit 236 (877 B,
// result takes BL); `result = false; if (query) result = true;` 182; the facing fallback written into
// `direction` 135 (same code); both speed lookups as calls to the existing 0x26f290 (same body) 316
// (/Ob1 file, stays out of line even with a __forceinline redeclaration; a local inline copy 135, same
// code); the result renamed reachable/arrived/success 135 each.
// Round 208: Retry-outcome-owned chosen-speed defaults with early result declaration, valid-scope
// retry outputs and horizontal-first pair declarations, masks 0..7: 212 each at 914/935 bytes.
// Original chosen-speed initialization and complete body restored; retained DL result and
// movement/request exits remain unresolved.
// Round 38 (lane 1): validity reused for the trajectory result with one final `return validity`
// and the request call returned from its arm 236 (877 B: every exit `mov al, bl`, retail keeps two
// variables, BL validity and a DL trajectory result); the request call returned from its arm
// alone 182 (919 B, result still memory-homed), also with the first-type test as its own `if` 182;
// the retry admission as `minimum < speed * 1.25f` 135; the retry success flag set after the
// output copies 135 (same code). The compile-only harness gives the result BL, so it cannot screen
// the LTCG memory-homing; the unmatched trajectory callee (frame 0x28 against 0x30) may feed it.
// Round 216: Native direction/request/facing declarations at entry, seven subsets 135 each; all with entry result 135. Request scope changes lookup/copy registers, not DL residence; all restored.
// Round 40 (lane 11, fast): `if (valid == true)` with `return valid;` 138 (cmp bl, 1, but the invalid
// exit is retail's unfolded `mov al, bl`: VC folds the value only from a test against zero); a copy
// `bool result = valid;` tested, `return valid;` 135 (copy propagated, folded); the jump publication
// as a __forceinline helper 135 (same code); the request arm returning its call with the movement
// test as its own `if` 182 (919 B). Diagnostics, all reverted: the facing normalization removed 358,
// the retry removed 485, the type scan removed 496; the result stays memory-homed in all three, so its
// homing does not come from the retry call, the facing calls or the scan.
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
    {
        vector3f direction;
        real vertical;
        real horizontal;
        real chosen_speed = speed;
        bool result = function_fa1a0(speed, 1.0f, &actor->position, target, &minimum, NULL, NULL, true,
            &direction, NULL, NULL, NULL, &vertical, &horizontal);
        if (force || speed * 1.25f > minimum)
        {
            real retry_speed = minimum + 0.03f;
            real retry_vertical;
            real retry_horizontal;
            if (function_fa1a0(retry_speed, 1.0f, &actor->position, target, &minimum, NULL, NULL, true,
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
                result = function_e6900(actor->unknown018, &request);
            }
            else if (!first_type)
            {
                real base_speed = 0.0f;
                byte *globals = (byte *)g_4e034c;
                if (globals && *(long *)(globals + 0xc8) > 0)
                    base_speed = *(real *)(*(byte **)(globals + 0xcc) + 0x80);
                if (base_speed * 1.2f < chosen_speed)
                {
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
                }
            }
        }
        return result;
    }
    return false;
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

// Round 16: retail passes the object index in EAX and the actor index in ECX, ours passes the
// actor index in ECX and keeps the object index elsewhere; retail computes the masked actor
// offset before the pool load and holds the actor in EDI.
// score 137 (was 137): frame 0x1c agrees; actor/object homes, velocity preload and final minimum remain (caller-inclusive fast 137).
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
        double dot = (double)desired->j * next_direction->j + (double)desired->i * next_direction->i;
        if (dot < -1.0) dot = -1.0;
        else if (dot > 1.0) dot = 1.0;
        real angle = function_50650((real)dot) * 0.31830987334251404f;
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
            real closing = object->velocity.j * path_direction->j + path_direction->i * object->velocity.i -
                (path_direction->i * other->velocity.i + other->velocity.j * path_direction->j);
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
    return MIN(scale, following_scale) * velocity_scale * speed_scale;
}


#include "object_markers.h"
#include "unknown_1f9240.h"
bool function_26f2d0(s_path_settings const *settings, point3f const *start, point3f const *end);
bool function_26f360(s_path_settings const *settings, point3f const *start, point3f const *end);
bool function_26fe90(long actor_index, long object_index, real distance, s_object_marker *out, point3f *point);
bool function_26f990(long actor_index, long object_index, real distance, s_object_marker *out, point3f *point);

// score 81 (was 235): Frame 0xe4 agrees; actor/character registers, selector tests and target/direction homes still differ.
// Shared point difference 240; entry request reservation 236; both 242; initial if admission 226;
// if admission plus shared difference 202 kept (1002 bytes); entry request plus if 228;
// complete graph/role names 209/209. Ascending admission 226, with shared difference 202,
// with typed request fields 214, with both 210. Original game-call values and orders retained.
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
    if (type == 5)
    {
        if (!function_10f9b0(actor->unknown018, 0x5000534, 0x5000049, 3, &animation, false) ||
            !function_26fe90(actor_index, object_index, *(real *)(character + 4), &marker, &target))
            goto failed;
        minimum = 0.8660253882408142f;
    }
    else if (type == 3)
    {
        if (!function_10f9b0(actor->unknown018, 0x5000281, 0x5000049, 3, &animation, false) ||
            !function_26f990(actor_index, object_index, *(real *)(character + 4), &marker, &target))
            goto failed;
        minimum = 0.95f;
    }
    else
        goto failed;
    {
        vector3f direction;
        vector3d_from_points3d(&actor->position, &target, &direction);
        direction.k = 0.0f;
        real distance = function_30bf0(&direction);
        if (distance > 0.0f && animation.position.x * -1.3f > distance && dot3f(&actor->unknown290, &direction) > 0.0f)
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
                    if (function_26f2d0(&settings, &actor->position, &target))
                    {
                        request.type = 0x2b;
                        can_move = true;
                    }
                    break;
                case 3:
                    if (function_26f360(&settings, &actor->position, &target))
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
                if (!result) failed = true;
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
            if (failed) goto failed;
        }
        return result;
    }
failed:
    *movement_out = *g_4687a4;
    actor = actor_get(actor_index);
    *(long *)((byte *)actor + 0x300) = object_index;
    *(short *)((byte *)actor + 0x304) = g_510c54->field_2_3 * 5;
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

// score 238 (was 238): Frame 0x3c, result/flag homes and exits agree (1182 bytes against 1173). Retail's zero, radius and
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
// @retail 0x1f99d0
real function_1f99d0(long actor_index, long object_index, short type, point2f const *target,
    point2f const *origin, point2f const *facing, s_obstacle_list const *obstacles)
{
    point2f const *volatile *local_0 = &origin;
    point2f const *local_1 = *local_0;
    s_actor_view *actor = actor_get(actor_index);
    byte *entry = (byte *)function_1e5450(actor_index, object_get(object_index)->tag_index);
    bool left = true;
    real result = 0.0f;
    real radius = *(real *)(entry + 0x28);
    point2f direction;
    vector2d_from_points2d(local_1, target, &direction);
    movement_normalize2d(&direction);
    point2f side = { 0.0f - direction.y, direction.x };
    if (!(facing->y * side.y + facing->x * side.x > 0.0f))
        left = false;
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
