// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1F2FE0.CPP: an actor following its path (the points at +0x548),
   and the plane and limits it keeps to while moving */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "normalize3d.h"
#include "unknown_20fe20.h"
#include "unknown_1e3920.h"
#include "slot_handler.h"

void function_1caa40(long object_index, point3f *position);
/* retail inlines the object's forward lookup in 0x1f3e40 */
__forceinline void object_get_forward(long object_index, vector3f *forward);
bool __stdcall function_1f8a70(long actor_index, long unknown);

/* aims the actor at its current path point */
// @retail 0x1f2fe0
void function_1f2fe0(long actor_index)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	point3f position;
	s_actor_path_point *point;

	actor->unknown5d2 = false;
	actor->unknown5d0 = true;
	if (actor->unknown229)
	{
		if (actor->unknown26c != NONE)
			function_1caa40(actor->unknown26c, &position);
		else
			function_1caa40(actor->unit_index, &position);
	}
	else
	{
		position = actor->position;
	}

	point = &actor->path[actor->path_index];
	if (point->node.output_index == NONE)
	{
		vector3d_from_points3d(&position, &point->node.point, &actor->unknown5ec);
	}
	else
	{
		point3f world;

		function_210850(&point->node, &world);
		vector3d_from_points3d(&position, &world, &actor->unknown5ec);
	}
}

/* moves on to the next path point; false at the end of the path */
// @retail 0x1f3100
bool function_1f3100(long actor_index)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	bool result = false;

	if (actor->unknown504 == 1 && actor->unknown50c)
	{
		if (actor->path_index < actor->path_count - 1)
		{
			actor->path_index++;
			function_1f2fe0(actor_index);
			result = true;
		}
		else if (actor->unknown538)
		{
			function_1f8780(actor_index, true);
			actor->unknown5d0 = false;
		}
		else
		{
			actor->unknown504 = 4;
			actor->unknown5d0 = false;
		}
	}

	return result;
}

// @retail 0x1f3190
void function_1f3190(long actor_index)
{
	bool active = actor_moving_get(actor_index)->unknown040;
	s_actor_moving *actor = actor_moving_get(actor_index);

	if (active && !actor->unknown506)
	{
		if (function_1f8660(actor_index) || actor->unknown504 == 3)
			function_1f8a70(actor_index, 0);
	}

	function_1f3430(actor_index);
	if (actor_moving_get(actor_index)->unknown50c && actor_moving_get(actor_index)->unknown504 == 1)
	{
		function_1f2fe0(actor_index);
	}
	else
	{
		actor->unknown5d0 = false;
		actor->unknown5d2 = false;
		actor->unknown50c = false;
	}
}

/* whether the actor has reached its target point (+0x4ec) */
// @retail 0x1f3230
bool function_1f3230(long actor_index, real radius)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	s_type_c3b527 *target = &actor->unknown4ec;
	point3f *position = &actor->position;
	vector3f delta;
	real distance_squared;

	if (target->output_index == NONE)
	{
		vector3d_from_points3d(&target->point, position, &delta);
		distance_squared = length_sq3f(&delta);
	}
	else
	{
		point3f point;

		function_210850(target, &point);
		vector3d_from_points3d(&point, position, &delta);
		distance_squared = length_sq3f(&delta);
	}
	bool result = false;

	if (actor->unknown26c == NONE && actor->unknown4ae)
	{
		if (actor->path_index == actor->path_count - 1)
		{
			vector3f segment;
			vector3f offset;

			if (actor->path_index - 1 >= 0)
				function_210be0(&actor->path[actor->path_index - 1].node, target, &segment);
			else
				function_210be0(&actor->path_start, target, &segment);
			function_210c90(position, target, &offset);
			result = offset.i * segment.i + segment.j * offset.j + segment.k * offset.k < 0.0f;
		}
	}
	else if (radius * radius > distance_squared)
	{
		result = true;
	}
	else if (actor->unknown4ae)
	{
		radius += 0.5f;
		if (radius * radius > distance_squared)
		{
			s_moving_object *unit = moving_object_get(actor->unit_index);
			vector3f offset;

			function_210c90(position, target, &offset);
			if (dot3f(&unit->velocity, &offset) < 0.0f)
				result = true;
		}
	}

	return result;
}

// @retail 0x1f3430
bool function_1f3430(long actor_index)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	bool result = false;

	if (actor->unknown50c && actor_moving_get(actor_index)->unknown504 == 1)
	{
		if (actor->path_index >= actor->path_count)
		{
			result = true;
		}
		else
		{
			real local_0 = function_1e3920(actor_index);
			result = function_1f3230(actor_index, local_0);
		}
		if (result)
			function_1f8780(actor_index, true);
	}

	return result;
}

/* keeps the actor on the near side of a plane for some ticks */
// The final product reads the point's x before multiplying by the plane's i,
// preserving retail's scalar SSE operand order.
// @retail 0x1f34b0
bool function_1f34b0(long actor_index, point3f const *point, vector3f const *normal, real distance, short ticks)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	plane3f *plane = &actor->unknown608;

	plane->n = *normal;
	plane->d = plane->k * point->z + plane->j * point->y + *(real const volatile *)&point->x * plane->i;
	actor->unknown618 = distance;
	actor->unknown61c = ticks;
	actor->unknown605 = true;
	actor->unknown5d8 = false;
	return true;
}

// @retail 0x1f3540
bool function_1f3540(long actor_index, vector3f *normal)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	bool result = false;

	if (actor->unknown605)
	{
		result = true;
		if (plane_distance_to_point(&actor->unknown608, &actor->position) > actor->unknown618 ||
			actor->unknown61c == 0 || actor->unknown5d8)
			result = false;
		if (result)
		{
			*normal = actor->unknown608.n;
			if (actor->unknown61c > 0)
				actor->unknown61c--;
		}
		else
		{
			actor->unknown605 = false;
			actor->unknown61c = 0;
		}
	}

	return result;
}

// @retail 0x1f3610
bool function_1f3610(long actor_index, short value)
{
	s_actor_moving *actor = actor_moving_get(actor_index);

	if (actor->unknown61e < value)
		actor->unknown61e = value;
	return true;
}

void function_11d180(vector3f const *in, vector3f const *up, vector3f *forward, vector3f *left, vector3f *out);

// @retail 0x1f3e40
void __stdcall function_1f3e40(long object_index, vector3f const *forward, vector3f *vector)
{
	vector3f unused0;
	vector3f left;
	vector3f up;
	function_11d180(forward, g_4687b0, &unused0, &left, &up);
	vector3f object_forward;
	object_get_forward(object_index, &object_forward);
	vector3f unused1;
	vector3f object_left;
	vector3f object_up;
	function_11d180(&object_forward, g_4687b0, &unused1, &object_left, &object_up);
	vector3f world;
	real input_i = vector->i;
	world.i = g_4687a4->i + object_forward.i * input_i;
	world.j = g_4687a4->j + object_forward.j * input_i;
	world.k = g_4687a4->k + object_forward.k * input_i;
	real input_j = vector->j;
	world.i += object_left.i * input_j;
	world.j += object_left.j * input_j;
	world.k += object_left.k * input_j;
	real input_k = vector->k;
	world.i += object_up.i * input_k;
	world.j += object_up.j * input_k;
	world.k += object_up.k * input_k;
	vector3f result;
	result.i = dot3f(forward, &world);
	result.j = dot3f(&left, &world);
	result.k = dot3f(&up, &world);
	*vector = result;
}


#include "unit_requests.h"
#include <string.h>
void *function_1e50c0(long actor_index);
bool function_10f630(long object_index, long *first, long *second);
long function_113da0(long unit_index);
bool __stdcall function_10f430(long unit_index, long mode, long weapon_class, long weapon_type, long set, real blend, bool unknown, long flags);
long function_1469f0(real seconds);

static __forceinline long real_to_long(real value);

// @retail 0x1f3640
void function_1f3640(long actor_index)
{
    s_actor_view *actor = actor_get(actor_index);
    bool result = false;
    bool active = false;
    byte *entry = (byte *)function_1e50c0(actor_index);
    if (!actor->unknown007 && entry)
    {
        if (*((bool *)actor + 0x6c0))
        {
            long first, second;
            if (function_10f630(actor->unknown018, &first, &second))
                active = second == 0x600005f;
        }
        if (*(long *)((byte *)actor + 0x858) != NONE)
            result = false;
        else if ((*(long *)((byte *)actor + 0x7fc) == 0x6000086 && (byte)function_113da0(actor->unknown018)) || actor->unknown086 >= 6)
            result = false;
        else
        {
            bool blocked = false;
            if (!active)
            {
                long unit_index = actor->unknown018;
                blocked = function_110ab0(unit_index);
            }
            if (blocked)
                result = false;
            else if (*(long *)((byte *)actor + 0x6c8))
                result = false;
            else if (!active && *(short *)((byte *)actor + 0x686) > 0)
                result = false;
            else
            {
                result = (actor->unknown5d0 && *((bool *)actor + 0x485)) ||
                    ((!actor->unknown5d0 || active) && *((bool *)actor + 0x484));
            }
        }
        if (active && !result)
        {
            function_10f430(actor->unknown018, 0x7000101, 0x7000101, 0x7000101, 0x400000c, 0.2f, false, 0);
        }
        else
        {
            if (!*((bool *)actor + 0x6c0) && result)
            {
                real seconds = function_259d0(&g_4e7408->unknown0, 0, 0, *(real *)(entry + 4), *(real *)(entry + 8));
                real time = (real)g_510c54->field_2_3 * seconds;
                long ticks;
                ticks = real_to_long(time);
                *(long *)((byte *)actor + 0x6c4) = ticks;
            }
            if (result && !active && --*(long *)((byte *)actor + 0x6c4) <= 0)
            {
                real delay = function_259d0(&g_4e7408->unknown0, 0, 0, *(real *)(entry + 4), *(real *)(entry + 8));
                s_actor_view *current = actor_get(actor_index);
                if (current->unknown26c == NONE)
                {
                    s_unit_request request;
                    memset(&request, 0, sizeof(request));
                    request.type = 0x19;
                    request.type19.animation = 0x600005f;
                    request.type19.mode = 2;
                    function_e6900(current->unknown018, &request);
                }
                *(long *)((byte *)actor + 0x6c4) = function_1469f0(delay);
            }
        }
    }
    *((bool *)actor + 0x6c0) = result;
}
