// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_26C380.CPP: traces along a line across the sectors of the
   pathfinding data, edge by edge */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "unknown_1fa590.h"
#include "unknown_26c380.h"
#include "path.h"

bool function_270600(s_path_location const *location, long node_index);

/* an edge as the traces read it: its flags, and the sectors on either side */
struct s_sector_edge
{
	word vertices[2];
	word flags;
	word unknown06;
	word next_edges[2];
	word sectors[2];
};

struct vector2f
{
	real i;
	real j;
};

static inline real dot_product2d(point3f const *point, vector2f const *vector)
{
	return point->x * vector->i + point->y * vector->j;
}

static inline real cross_product2d(vector2f const *a, vector2f const *b)
{
	return a->i * b->j - a->j * b->i;
}

static inline void vector2d_from_points3d(point3f const *p0, point3f const *p1, vector2f *out)
{
	out->i = p1->x - p0->x;
	out->j = p1->y - p0->y;
}

static inline vector3f *cross3f(vector3f const *a, vector3f const *b, vector3f *result)
{
	result->i = b->k * a->j - a->k * b->j;
	result->j = b->i * a->k - b->k * a->i;
	result->k = b->j * a->i - b->i * a->j;
	return result;
}

static inline real magnitude3d(vector3f const *v)
{
	return (real)sqrt(v->i * v->i + v->j * v->j + v->k * v->k);
}

static inline void function_x697631(point3f const *point, vector3f const *vector, real t, point3f *result)
{
	result->x = vector->i * t + point->x;
	result->y = vector->j * t + point->y;
	result->z = vector->k * t + point->z;
}

static inline void point_from_line2d(point2f const *point, vector2f const *vector, real t, point2f *result)
{
	result->x = vector->i * t + point->x;
	result->y = vector->j * t + point->y;
}

/* starts walking the edges of a sector (and first those of its surfaces) */
__forceinline void edge_iterator_new(s_edge_iterator *iterator, s_pathfinding_data const *pathfinding,
	long sector_index, bool surfaces)
{
	s_pathfinding_node const *sector = &pathfinding->nodes[sector_index];

	iterator->edge_index = (word)NONE;
	iterator->first_edge_index = (word)sector->first_edge;
	iterator->next_edge_index = (word)sector->first_edge;
	iterator->surface = sector_index;
	iterator->vertex = NONE;
	iterator->previous_surface_index = NONE;
	iterator->surface_index = surfaces && pathfinding->surface_count > 0 ? sector->first_surface : NONE;
	iterator->pathfinding = (s_pathfinding_edges_view const *)pathfinding;
	iterator->count = 0;
}

/* where the trace crosses an edge of a sector, when the edge's two ends
   (point0, point1) lie on either side of the trace's line (distance0 <= 0 <=
   distance1): the sector beyond the edge, whether the trace is blocked there,
   and the point and distance along the trace */
// @retail 0x26c380
bool function_26c380(point3f const *origin, s_pathfinding_data const *pathfinding, bool forward,
	long sector_index, long edge_index, vector3f const *direction, long *next_sector_index,
	real distance0, real distance1, point3f const *point0, point3f const *point1,
	s_sector_trace_result *result)
{
	bool success = false;

	if (distance1 >= 0.0f && distance0 <= 0.0f)
	{
		s_sector_edge const *edge = &((s_sector_edge const *)pathfinding->edges)[edge_index];
		long next = edge->sectors[!forward];
		vector2f edge_vector;
		vector2f offset;
		real denominator;

		if (next == NONE || next == (word)NONE)
		{
			result->blocked = true;
		}
		else
		{
			s_pathfinding_node const *sector = &pathfinding->nodes[next];

			result->blocked = !(sector->flags & 1) || ((sector->flags & 2) && function_1fa6b0(sector, pathfinding));
		}
		result->sector_index = sector_index;
		result->edge_index = edge_index;
		vector2d_from_points3d(point1, point0, &edge_vector);
		vector2d_from_points3d(point1, origin, &offset);
		denominator = cross_product2d((vector2f const *)direction, &edge_vector);
		if (denominator != 0.0f)
		{
			result->distance = cross_product2d(&edge_vector, &offset) / denominator;
			point_from_line2d((point2f const *)origin, (vector2f const *)direction, result->distance,
				(point2f *)&result->point);
			result->point.z = origin->z;
			result->distance = 0.0f > result->distance ? 0.0f : result->distance;
		}
		else
		{
			result->distance = 0.0f;
			result->point = *origin;
		}
		if (next_sector_index)
		{
			*next_sector_index = next;
		}
		success = true;
	}
	return success;
}

// @retail 0x26c590
bool function_26c590(s_pathfinding_data const *pathfinding, point3f const *origin, long sector_index,
	long target_sector_index, vector3f const *direction, real distance, s_path_location const *location,
	s_path_trace_result *trace)
{
	s_sector_trace_result *result = (s_sector_trace_result *)trace;
	bool success = false;
	long current_sector_index = sector_index;
	short count = 0;
	real last_distance = -0.1f;
	s_pathfinding_edge *edge = NULL;

	result->distance = distance;
	result->unknown20 = 3.4028235e38f;
	result->blocked = false;
	result->sector_index = NONE;
	result->edge_index = (word)NONE;
	result->unknown1c = false;
	if (pathfinding && sector_index != target_sector_index && sector_index >= 0 && sector_index < pathfinding->node_count)
	{
		vector3f normal;

		cross3f(g_4687b0, direction, &normal);
		if (magnitude3d(&normal) > 1e-5)
		{
			vector2f line_normal;
			real line_distance;
			long next_sector_index = sector_index;

			line_normal.i = normal.i;
			line_normal.j = normal.j;
			line_distance = dot_product2d(origin, &line_normal);
			while (next_sector_index != NONE)
			{
				long previous_sector_index = current_sector_index;
				s_pathfinding_node const *sector;
				s_edge_iterator iterator;
				bool first = true;
				bool hit = false;
				point3f const *point1;
				real distance1;

				current_sector_index = next_sector_index;
				next_sector_index = NONE;
				sector = &pathfinding->nodes[current_sector_index];
				if (!(sector->flags & 0x10) && !result->unknown1c &&
					(!edge || (((s_sector_edge const *)edge)->flags & 0x200)))
				{
					result->unknown1c = true;
					result->unknown20 = last_distance;
				}
				if (current_sector_index == target_sector_index)
				{
					break;
				}
				edge_iterator_new(&iterator, pathfinding, current_sector_index, true);
				edge = function_1fa720(&iterator);
				if (!edge)
				{
					result->blocked = true;
					result->sector_index = current_sector_index;
					success = true;
					goto done;
				}
				do
				{
					bool forward = iterator.forward;
					point3f const *point0;
					real distance0;

					if (iterator.previous_surface_index != NONE)
					{
						point1 = &pathfinding->vertices[iterator.vertex];
						point0 = &pathfinding->vertices[forward ? edge->vertices[1] : edge->vertices[0]];
						distance0 = dot_product2d(point0, &line_normal) - line_distance;
					}
					else
					{
						if (first)
						{
							point0 = &pathfinding->vertices[forward ? edge->vertices[1] : edge->vertices[0]];
							distance0 = dot_product2d(point0, &line_normal) - line_distance;
							first = false;
						}
						else
						{
							point0 = point1;
							distance0 = distance1;
						}
						point1 = &pathfinding->vertices[iterator.vertex];
					}
					distance1 = dot_product2d(point1, &line_normal) - line_distance;
					hit = function_26c380(origin, pathfinding, forward, current_sector_index, iterator.edge_index,
						direction, &next_sector_index, distance0, distance1, point0, point1, result);
					if (hit)
					{
						if (result->blocked || next_sector_index != previous_sector_index)
						{
							break;
						}
						hit = false;
					}
					edge = function_1fa720(&iterator);
				} while (edge);
				if (hit)
				{
					if (result->distance - last_distance < -0.1 && count > 0)
					{
						break;
					}
					last_distance = result->distance;
					if (result->distance - distance > 0.0001)
					{
						if (target_sector_index != NONE && target_sector_index != result->sector_index)
						{
							if (next_sector_index == target_sector_index && result->distance - distance < 0.01)
							{
								result->sector_index = next_sector_index;
								result->blocked = false;
								result->edge_index = (word)NONE;
								result->distance = distance;
								function_x697631(origin, direction, distance, &result->point);
								success = false;
							}
							else
							{
								result->distance = 0.0f;
								result->blocked = true;
								result->edge_index = (word)NONE;
								function_x697631(origin, direction, distance, &result->point);
								success = true;
							}
						}
						else
						{
							result->blocked = false;
							result->edge_index = (word)NONE;
							result->distance = distance;
							function_x697631(origin, direction, distance, &result->point);
							success = false;
						}
						return success;
					}
					if (result->blocked)
					{
						success = true;
						return success;
					}
					if (location && (((s_sector_edge const *)edge)->flags & 0x80) &&
						function_270600(location, iterator.edge_index))
					{
						success = true;
						result->blocked = true;
						return success;
					}
					if (count > 200)
					{
						break;
					}
					count++;
				}
				else
				{
					result->blocked = true;
					result->sector_index = current_sector_index;
					success = true;
					goto done;
				}
			}
		}
	}
	result->blocked = false;
	result->sector_index = current_sector_index;
done:
	result->edge_index = (word)NONE;
	result->distance = distance;
	function_x697631(origin, direction, distance, &result->point);
	return success;
}


// @retail 0x26d370
bool function_26d370(point3f const *point, vector3f const *direction, plane3f const *plane, real *distance)
{
	bool result = true;
	real denominator = dot3f(direction, &plane->n);
	if (fabs(denominator) > 0.01)
		*distance = -(plane_distance_to_point(plane, point) / denominator);
	else
		result = false;
	return result;
}
