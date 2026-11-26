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

// @retail 0x29d740
bool function_29d740(vector3f *arg_0, s_pathfinding_data const *arg_1, long arg_2)
{
	bool local_5 = false;
	s_pathfinding_edge const *local_0 = function_29d741(arg_1, arg_2);
	point3f const *local_2 = &arg_1->vertices[local_0->vertices[1]];
	point3f const *local_1 = &arg_1->vertices[local_0->vertices[0]];
	real local_3 = local_2->x - local_1->x;
	real local_4 = local_2->y - local_1->y;
	arg_0->i = local_4;
	arg_0->j = 0.0f - local_3;
	arg_0->k = 0.0f;
	if (function_30bf0(arg_0) > 0.0f)
		local_5 = true;
	return local_5;
}

#include "unknown_25fc30.h"

// score 117 (was 117): 0x29d740 call sets up &edge_normal last vs retail first; compiler hoists 0.4f into xmm0
// before if (direction), moving length to xmm7 and delta.i to xmm3 and emitting mulss xmm2, [esp+0x2c] vs retail
// movss xmm0, [esp+0x2c]; mulss xmm0, xmm3 (reloading length into xmm0 and loading 0.4f twice). Tried 0x29d740
// param order (pathfinding, index, &edge_normal) 114 (fixes lines 14-16); swapped/reordered dot expressions 114-123;
// 3-stmt k+j+i dot 103; 3-stmt k+i+j / i+k+j / basis_dot_xzy 92 (649 B, un-hoists 0.4f and matches lines 5-105
// and 123-190, but changes float grouping without matching); check_start/check_end defaults/long/nested 105-128;
// /Ob1 114; inline offset 121; in-place edge_normal rotation 139.
// @retail 0x29d7b0
bool function_29d7b0(long index, s_pathfinding_data *pathfinding, s_type_d4fbfa *definition,
	vector3f const *direction, point3f *point, vector3f *normal, char *side)
{
	bool result = false;
	if (index >= 0 && index < *(long *)((byte *)pathfinding + 8))
	{
		vector3f edge_normal;
		if (function_29d740(&edge_normal, pathfinding, index))
		{
			vector3f local_direction;
			bool facing = true;
			if (direction)
			{
				function_210770(definition->unknown0c, direction, &local_direction);
				if (0.0f > edge_normal.j * local_direction.j + edge_normal.i * local_direction.i + edge_normal.k * local_direction.k)
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
