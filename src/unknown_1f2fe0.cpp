// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1F2FE0.CPP: an actor following its path (the points at +0x548),
   and the plane and limits it keeps to while moving */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_20fe20.h"
#include "unknown_1e3920.h"
#include "slot_handler.h"

void function_1caa40(long object_index, point3f *position);
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
	}
	else
	{
		point3f point;

		function_210850(target, &point);
		vector3d_from_points3d(&point, position, &delta);
	}
	distance_squared = delta.k * delta.k + delta.j * delta.j + delta.i * delta.i;
	bool result = false;

	if (actor->unknown26c == NONE && actor->unknown4ae)
	{
		if (actor->path_index == actor->path_count - 1)
		{
			s_type_c3b527 *previous;
			vector3f segment;
			vector3f offset;

			if (actor->path_index - 1 >= 0)
				previous = &actor->path[actor->path_index - 1].node;
			else
				previous = &actor->path_start;

			function_210be0(previous, target, &segment);
			function_210c90(position, target, &offset);
			if (dot3f(&offset, &segment) < 0.0f)
				goto local_0;
		}
	}
	else if (radius * radius > distance_squared)
	{
		goto local_0;
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
				goto local_0;
		}
	}

	goto local_1;
local_0:
	result = true;
local_1:
	return result;
}

// @retail 0x1f3430
bool function_1f3430(long actor_index)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	bool result = false;

	if (actor->unknown50c && actor->unknown504 == 1)
	{
		if (actor->path_index >= actor->path_count)
		{
			function_1f8780(actor_index, true);
			return true;
		}
		else
		{
			real volatile local_0 = function_1e3920(actor_index);
			result = function_1f3230(actor_index, local_0);
			if (result)
				function_1f8780(actor_index, true);
		}
	}

	return result;
}

/* keeps the actor on the near side of a plane for some ticks */
// @retail 0x1f34b0
bool function_1f34b0(long actor_index, vector3f const *normal, point3f const *point, real distance, short ticks)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	plane3f *plane = &actor->unknown608;

	plane->n = *normal;
	plane->d = plane->k * point->z + plane->j * point->y + point->x * plane->i;
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
		{
			result = false;
			actor->unknown605 = false;
			actor->unknown61c = 0;
		}
		else
		{
			*normal = actor->unknown608.n;
			if (actor->unknown61c > 0)
				actor->unknown61c--;
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

void function_11d180(vector3f *left, vector3f const *in, vector3f *out, vector3f const *up, vector3f *forward);

// @retail 0x1f3e40
void __stdcall function_1f3e40(long object_index, vector3f const *forward, vector3f *vector)
{
	vector3f unused0;
	vector3f left;
	vector3f up;
	function_11d180(&left, forward, &up, g_4687b0, &unused0);
	vector3f object_forward;
	s_slot_object_view *object = object_get(object_index);
	if (object->parent_index == NONE)
		object_forward = object->forward;
	else
	{
		transform4x3f *matrix = object_node_matrix(object_get(object->parent_index), object->parent_node);
		real i = object->forward.i;
		real j = object->forward.j;
		real k = object->forward.k;
		object_forward.i = matrix->forward.i * i + matrix->left.i * j + matrix->up.i * k;
		object_forward.j = matrix->forward.j * i + matrix->left.j * j + matrix->up.j * k;
		object_forward.k = matrix->forward.k * i + matrix->left.k * j + matrix->up.k * k;
	}
	vector3f unused1;
	vector3f object_left;
	vector3f object_up;
	function_11d180(&object_left, &object_forward, &object_up, g_4687b0, &unused1);
	vector3f world;
	world.i = g_4687a4->i + object_forward.i * vector->i + object_left.i * vector->j + object_up.i * vector->k;
	world.j = g_4687a4->j + object_forward.j * vector->i + object_left.j * vector->j + object_up.j * vector->k;
	world.k = g_4687a4->k + object_forward.k * vector->i + object_left.k * vector->j + object_up.k * vector->k;
	vector3f result;
	result.i = forward->i * world.i + forward->j * world.j + forward->k * world.k;
	result.j = left.i * world.i + left.j * world.j + left.k * world.k;
	result.k = up.i * world.i + up.j * world.j + up.k * world.k;
	*vector = result;
}


#include "unit_requests.h"
#include <string.h>
void *function_1e50c0(long actor_index);
bool function_10f630(long object_index, long *first, long *second);
long function_113da0(long unit_index);
bool __stdcall function_10f430(long unit_index, long mode, long weapon_class, long weapon_type, long set, real blend, bool unknown, long flags);
long function_1469f0(real seconds);

// @retail 0x1f3640
void function_1f3640(long actor_index)
{
    s_actor_view *actor = actor_get(actor_index);
    bool result = false;
    bool active = false;
    byte *entry = (byte *)function_1e50c0(actor_index);
    if (!actor->unknown007 && entry)
    {
        bool enabled;
        if (*((bool *)actor + 0x6c0))
        {
            long first, second;
            if (function_10f630(actor->unknown018, &first, &second))
                active = second == 0x600005f;
        }
        if (*(long *)((byte *)actor + 0x858) != NONE)
            enabled = false;
        else if ((*(long *)((byte *)actor + 0x7fc) == 0x6000086 && (byte)function_113da0(actor->unknown018)) || actor->unknown086 >= 6)
            enabled = false;
        else if (!active && function_110ab0(actor->unknown018))
            enabled = false;
        else if (*(long *)((byte *)actor + 0x6c8))
            enabled = false;
        else if (!active && *(short *)((byte *)actor + 0x686) > 0)
            enabled = false;
        else
            enabled = (actor->unknown5d0 && *((bool *)actor + 0x485)) || ((!actor->unknown5d0 || active) && *((bool *)actor + 0x484));
        if (active && !enabled)
        {
            function_10f430(actor->unknown018, 0x7000101, 0x7000101, 0x7000101, 0x400000c, 0.2f, false, 0);
            goto store;
        }
        if (!*((bool *)actor + 0x6c0))
        {
            if (!enabled)
                goto store;
            real seconds = function_259d0(&g_4e7408->unknown0, 0, 0, *(real *)(entry + 4), *(real *)(entry + 8));
            real time = (real)g_510c54->field_2_3 * seconds;
            long ticks;
            __asm
            {
                fld time
                fistp ticks
            }
            *(long *)((byte *)actor + 0x6c4) = ticks;
        }
        if (enabled && !active && --*(long *)((byte *)actor + 0x6c4) <= 0)
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
store:
        result = enabled;
    }
    *((bool *)actor + 0x6c0) = result;
}
