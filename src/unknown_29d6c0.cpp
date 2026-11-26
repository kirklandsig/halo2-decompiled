#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "unknown_20fe20.h"
#include "unknown_2626b0.h"
#include "unknown_1fa590.h"
#include <math.h>
// @flags /O2 /arch:SSE /Gr

real function_30bf0(vector3f *arg_0);

struct s_29d6c0
{
	byte field_0[0xc];
	short field_c;
	byte field_e;
	byte field_f[9];
	real field_18;
	real field_1c;
};

// @retail 0x29d6c0
bool function_29d6c0(vector3f *arg_0, s_reference arg_1)
{
	s_29d6c0 *local_0 = (s_29d6c0 *)function_262b40(arg_1);
	if (local_0->field_e & 0x80)
	{
		*arg_0 = *g_4687b0;
		return true;
	}
	vector3f local_1;
	real local_2 = (real)cos(local_0->field_1c);
	local_1.i = (real)cos(local_0->field_18) * local_2;
	local_1.j = (real)sin(local_0->field_18) * local_2;
	local_1.k = (real)sin(local_0->field_1c);
	return function_2105b0(local_0->field_c, &local_1, arg_0);
}

PRIVATE __forceinline s_pathfinding_edge const *function_29d741(s_pathfinding_data const *arg_0, long arg_1)
{
	return &arg_0->edges[arg_1];
}

/* parameter order (pathfinding, index, normal) is retail's: 0x29d7b0 sets up the normal (EAX) first and the
   pathfinding data (EDX) last, last to first in the declared order */
// @retail 0x29d740
bool function_29d740(s_pathfinding_data const *arg_0, long arg_1, vector3f *arg_2)
{
	bool local_5 = false;
	s_pathfinding_edge const *local_0 = function_29d741(arg_0, arg_1);
	point3f const *local_2 = &arg_0->vertices[local_0->vertices[1]];
	point3f const *local_1 = &arg_0->vertices[local_0->vertices[0]];
	real local_3 = local_2->x - local_1->x;
	real local_4 = local_2->y - local_1->y;
	arg_2->i = local_4;
	arg_2->j = 0.0f - local_3;
	arg_2->k = 0.0f;
	if (function_30bf0(arg_2) > 0.0f)
		local_5 = true;
	return local_5;
}

#include "unknown_25fc30.h"

// @retail 0x29d7b0
bool function_29d7b0(long index, s_pathfinding_data *pathfinding, s_type_d4fbfa *definition,
	vector3f const *direction, point3f *point, vector3f *normal, char *side)
{
	bool result = false;
	if (index >= 0 && index < *(long *)((byte *)pathfinding + 8))
	{
		vector3f edge_normal;
		if (function_29d740(pathfinding, index, &edge_normal))
		{
			vector3f local_direction;
			bool facing = true;
			if (direction)
			{
				function_210770(definition->unknown0c, direction, &local_direction);
				if (0.0f > dot3f(&edge_normal, &local_direction))
					facing = false;
			}
			if (facing)
			{
				if (normal)
					*normal = edge_normal;
				s_pathfinding_edge *edge = &pathfinding->edges[index];
				point3f *first = &pathfinding->vertices[edge->vertices[0]];
				vector3f delta;
				vector3d_from_points3d(first, &pathfinding->vertices[edge->vertices[1]], &delta);
				real length = function_30bf0(&delta);
				if (length > 0.0f)
				{
					vector3f offset;
					vector3d_from_points3d(first, (point3f const *)definition, &offset);
					real position = dot3f(&offset, &delta);
					if (0.0f > position)
						position = 0.0f;
					else if (position > length)
						position = length;
					if (side)
						*side = 0;
					vector3f edge_direction;
					edge_direction.i = edge_normal.j;
					edge_direction.j = 0.0f - edge_normal.i;
					edge_direction.k = edge_normal.k;
					bool check_start;
					bool check_end;
					if (direction)
					{
						if (dot3f(&edge_direction, &local_direction) > 0.0f)
						{
							check_start = true;
							check_end = false;
						}
						else
						{
							check_start = false;
							check_end = true;
						}
					}
					else
					{
						check_start = true;
						check_end = true;
					}
					if (check_start && 0.4f > position && (((byte *)edge)[5] & 0x10))
					{
						result = true;
						position = 0.0f;
						if (side)
							*side = 1;
					}
					if (check_end)
					{
						if (0.4f > length - position && (((byte *)edge)[5] & 0x20))
						{
							result = true;
							position = length;
						}
						if (result && side)
							*side = 2;
					}
					point3f projected;
					projected.x = delta.i * position + first->x;
					projected.y = delta.j * position + first->y;
					projected.z = delta.k * position + first->z;
					if (point)
						*point = projected;
				}
			}
		}
	}
	return result;
}
