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

static inline void vector3d_set(vector3f *vector, real i, real j, real k)
{
	vector->i = i;
	vector->j = j;
	vector->k = k;
}

/* takes the pending facing (+0x622) unless the actor rides a vehicle */
// @retail 0x1f5560
bool function_1f5560(long actor_index, vector3f *facing)
{
	s_actor_moving *actor = actor_moving_get(actor_index);

	if (actor->unknown26c == NONE && actor->unknown622)
		vector3d_set(facing, actor->unknown624 * actor->unknown62c, actor->unknown628 * actor->unknown62c, actor->unknown630);
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
// score 11 (was 11): state is sign-extended and spilled as a dword; retail
// loads and spills a word. State 7 first 13; state before movement 86;
// animation first 19; promoted accessor return 11; unsigned accessor 11;
// indexed forward 11; indexed target 11; immutable long state 11.
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
// score 100 (was 100): retail's prologue saves ESI as well and pushes the
// registers in the order EBP, EBX, ESI, EDI; here only EBX, EBP and EDI are
// saved, so every stack slot after the prologue sits at another offset.
// @retail 0x1f4f40
bool function_1f4f40(long actor_index, vector3f const *facing, short unknown, s_type_c3b527 const *point, bool face_prop)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	byte result = 0;

	if (!function_110ab0(actor->unit_index))
	{
		point3f target;
		vector3f offset;

		function_210850(point, &target);
		vector3d_from_points3d(&target, &actor->position, &offset);
		if (!(length_sq3f(&offset) > 0.5f))
		{
			if (face_prop)
			{
				s_type_5cfb45 *state = function_25d690(prop_ref_get(actor->prop_index));
				vector3f direction;

				direction.i = state->position.x - actor->position.x;
				direction.j = state->position.y - actor->position.y;
				direction.k = 0.0f;
				if (!(function_30bf0(&direction) > 0.0f) || dot3f(facing, &direction) < 0.70710677f)
					return result;
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
// score 66 (was 66): retail saves ESI in the prologue, ours does not, and
// ours is ten bytes longer than retail.
// @retail 0x1f58c0
bool function_1f58c0(long actor_index, vector3f *facing, short *unknown)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	bool result = false;

	if (!function_110ab0(actor->unit_index) && g_510c54->game_time < actor->unknown634)
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

// score 108 (was 108): ours is five bytes longer than retail: the value that
// is compared is loaded into another register and the call that follows takes
// its argument from another stack slot.
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
	vector3f const *projection_direction;
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
		{
			direction_valid = true;
			s_type_c3b527 *previous = path->field_2e > 0 ? &path->field_30[path->field_2e - 1].point : (s_type_c3b527 *)path->field_1c;
			function_210be0(previous, &path->field_30[path->field_2e].point, &approach);
			if (function_30bf0(&approach) > 0.0f)
			{
				projection_direction = &approach;
				goto projection;
			}
		}
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
		else if (direction_valid && actor->unknown4d4)
		{
			projection_direction = &direction;
			goto projection;
		}
	}
	goto finish;
projection:
	{
		real remaining = dot3f((vector3f *)&path->field_30[path->field_2e].point.point, projection_direction) - dot3f((vector3f const *)point, projection_direction);
		if (*(real *)(definition + 4) * 1.2f > remaining)
			reached = true;
	}
finish:
	*last_out = last;
	if (last && !actor->unknown4d5)
		*direction_valid_out = false;
	else
	{
		*direction_valid_out = direction_valid;
		direction_out->i = direction.i;
		direction_out->j = direction.j;
		direction_out->k = direction.k;
	}
	return reached;
}

bool function_fa1a0(real speed, real gravity_scale, point3f const *origin, point3f const *target,
	real *minimum_speed, real const *time_scale, real const *forced_speed, bool high_arc, vector3f *direction,
	real *speed_out, real *time_out, real *distance, real *vertical_speed, real *horizontal_speed);

// score 5 (was 5): the arguments of the ballistics call are pushed in another
// order and the high arc flag goes into EDX; retail's function_fa1a0 takes the
// target before the origin and is defined in src/projectiles.cpp.
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

// score 172 (was 191): fast-byte initialization and trajectory call convention;
// search then selection 172; unsigned scan 178; fixed do/while 191;
// types arm first 191; fallback setters 195; split plus unsigned scan 223.
// signed-long search 192; signed-byte search 188; exact exhaustion 210;
// named first normalization result 172; global optimization off 542.
// @retail 0x1f8160
bool function_1f8160(long actor_index, signed char const *types, point3f const *target, bool force)
{
    bool valid = false;
    s_actor_view *actor = actor_get(actor_index);
    real minimum = 0.0f;
    bool fast = false;
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
                short i = 0;
                while (i < 6 && !(*types & (1 << i)))
                    ++i;
                if (i < 6)
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
                vertical = retry_vertical;
                horizontal = retry_horizontal;
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
                if (base_speed * 1.2f < chosen_speed)
                {
                    actor->unknown464 = true;
                    actor->unknown466 = true;
                    *(real *)((byte *)actor + 0x470) = horizontal;
                    *(real *)((byte *)actor + 0x474) = vertical > 1.2f ? vertical : 1.2f;
                    direction.k = 0.0f;
                    if (normalize2d((point2f *)&direction) <= 0.0f)
                    {
                        direction = actor->unknown290;
                        if (direction.k != 0.0f)
                        {
                            direction.k = 0.0f;
                            if (normalize2d((point2f *)&direction) <= 0.0f)
                            {
                                direction = *g_4687a8;
                                *(point2f *)((byte *)actor + 0x468) = *(point2f *)&direction;
                                return result;
                            }
                            *(point2f *)((byte *)actor + 0x468) = *(point2f *)&direction;
                            return result;
                        }
                    }
                    *(point2f *)((byte *)actor + 0x468) = *(point2f *)&direction;
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

// @retail 0x1f9e70
real function_1f9e70(long actor_index, long object_index, point2f const *position,
    vector2f const *path_direction, vector2f const *facing, vector2f const *desired,
    vector2f const *next_direction, real distance, bool ignore_facing)
{
    s_record_pool *local_0 = g_4f55f0;
    byte *local_1 = local_0->data;
    byte *actor = local_1 + (actor_index & 0xffff) * sizeof(s_actor_view);
    s_slot_object_view *object = object_get(object_index);
    byte *settings = (byte *)function_1e5450(actor_index, object->tag_index);
    real speed_scale;
    real turning_scale = 1.0f;
    real facing_scale;
    real velocity_scale;
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
        real length = (real)sqrt(velocity.j * velocity.j + velocity.i * velocity.i);
        if (!(fabs(length) < 0.0001f))
        {
            real inverse = 1.0f / length;
            velocity.i *= inverse;
            velocity.j *= inverse;
        }
        velocity_scale = (desired->i * velocity.i + desired->j * velocity.j + 2.0f) * 0.3333333432674408f;
    }
    else
    {
        facing_scale = 1.0f;
        velocity_scale = 1.0f;
    }
    real radius = *(real *)(settings + 0x1c);
    if (next_direction && radius * 2.0f > distance)
    {
        real minimum = *(real *)(settings + 0x58);
        double dot = (double)desired->j * next_direction->j + (double)desired->i * next_direction->i;
        if (dot < -1.0) dot = -1.0;
        else if (dot > 1.0) dot = 1.0;
        real value = (real)dot;
        if (value < -1.0f) value = -1.0f;
        else if (value > 1.0f) value = 1.0f;
        real angle = (real)(acos(value) * 0.31830987334251404f);
        real fraction = 1.0f - (angle > 1.0f ? 1.0f : angle);
        real scale = (1.0f - minimum) * fraction * fraction + minimum;
        turning_scale = (1.0f - scale) * distance / (radius * 2.0f) + scale;
    }
    real approach_scale = 1.0f;
    if (!actor[0x4ae])
    {
        double x = (double)*(real *)(actor + 0x510) - position->x;
        double y = (double)*(real *)(actor + 0x514) - position->y;
        real remaining = (real)sqrt(y * y + x * x);
        if (radius > remaining)
        {
            real minimum = *(real *)(settings + 0x54);
            approach_scale = (1.0f - minimum) / radius * remaining + minimum;
        }
    }
    else if (*(real *)(actor + 0x4b0) != 0.0f)
    {
        double x = (double)*(real *)(actor + 0x510) - position->x;
        double y = (double)*(real *)(actor + 0x514) - position->y;
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
        double x = (double)object->unknown030.x - other->unknown030.x;
        double y = (double)object->unknown030.y - other->unknown030.y;
        double z = (double)object->unknown030.z - other->unknown030.z;
        real gap = (real)(sqrt(z * z + y * y + x * x) - ((double)other->unknown03c + *(real *)(settings + 0x14)));
        if (gap <= 0.0f)
            gap = 0.0f;
        if (*(real *)(settings + 0x1c) > gap)
        {
            real closing = object->velocity.j * path_direction->j + path_direction->i * object->velocity.i -
                (path_direction->i * other->velocity.i + other->velocity.j * path_direction->j);
            if (closing > 0.0f)
            {
                real fraction = (*(real *)(settings + 0x1c) - gap) / *(real *)(settings + 0x1c);
                fraction = fraction < 0.0f ? 0.0f : fraction > 1.0f ? 1.0f : fraction;
                following_scale = (real)pow(1.0 - fraction, (double)closing * 0.5f);
            }
            if (gap < 2.0f)
            {
                real limit = 1.0f - (2.0f - gap) * 0.5f;
                if (following_scale > limit)
                    following_scale = limit;
            }
        }
    }
    real scale = turning_scale > approach_scale ? approach_scale : turning_scale;
    scale = facing_scale > scale ? scale : facing_scale;
    scale = scale > following_scale ? following_scale : scale;
    return scale * velocity_scale * speed_scale;
}


#include "object_markers.h"
#include "unknown_1f9240.h"
bool function_26f2d0(s_path_settings const *settings, point3f const *start, point3f const *end);
bool function_26f360(s_path_settings const *settings, point3f const *start, point3f const *end);
bool function_26fe90(long actor_index, long object_index, real distance, s_object_marker *out, point3f *point);
bool function_26f990(long actor_index, long object_index, real distance, s_object_marker *out, point3f *point);

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
        direction.i = target.x - actor->position.x;
        direction.j = target.y - actor->position.y;
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
    real length = (real)sqrt((double)v->x * v->x + (double)v->y * v->y); // retail sums on the x87 stack
    if (!(fabs(length) < 0.0001f))
    {
        real inverse = 1.0f / length;
        v->x *= inverse;
        v->y *= inverse;
        return length;
    }
    return 0.0f;
}

PRIVATE __forceinline real function_1f99d1(point2f *arg_0)
{
    real local_0 = arg_0->x;
    real local_1 = arg_0->y;
    real local_2 = (real)sqrt((double)local_0 * local_0 + (double)local_1 * local_1); // retail sums on the x87 stack
    if (!(fabs(local_2) < 0.0001f))
    {
        real local_3 = 1.0f / local_2;
        arg_0->x *= local_3;
        arg_0->y *= local_3;
        return local_2;
    }
    return 0.0f;
}

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
    point2f direction = { target->x - local_1->x, target->y - local_1->y };
    function_1f99d1(&direction);
    point2f side = { 0.0f - direction.y, direction.x };
    volatile bool left = true;
    if (!(facing->y * side.y + facing->x * side.x > 0.0f))
        left = false;
    point2f center;
    if (type == 0)
    {
        point2f perpendicular = { 0.0f - facing->y, facing->x };
        real signed_radius = left ? radius : 0.0f - radius;
        center.x = target->x + perpendicular.x * signed_radius;
        center.y = target->y + perpendicular.y * signed_radius;
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
            s_pathfinding_data *pathfinding = *(long *)(structure + 0xc4) > 0 ? *(s_pathfinding_data **)(structure + 0xc8) : 0;
            s_avoidance_trace trace;
            if (function_26ccb0(pathfinding, obstacles, NONE, (point2f *)&local_origin, sector, NONE,
                (point2f *)&local_direction, *(real *)(entry + 0x14), distance, false, false, false, 0, &trace))
                return result;
        }
        real cosine = desired.x * direction.x + desired.y * direction.y;
        cosine = (real)(cosine > 1.0f ? 1.0 : cosine);
        cosine = cosine < -1.0f ? -1.0f : (cosine > 1.0f ? 1.0f : cosine);
        result = (real)acos(cosine);
        if (left) result = 0.0f - result;
    }
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
