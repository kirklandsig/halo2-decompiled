// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_26C380.CPP: traces along a line across the sectors of the
   pathfinding data, edge by edge */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "vector2f_products.h"
#include "globals.h"
#include "unknown_1fa590.h"
#include "unknown_26c380.h"
#include "path.h"

bool function_270600(long node_index, s_path_location const *location);

/* an edge as the traces read it: its flags, and the sectors on either side */
struct s_sector_edge
{
	word vertices[2];
	word flags;
	word unknown06;
	word next_edges[2];
	word sectors[2];
};

static inline real dot_product2d(point3f const *point, vector2f const *vector)
{
	return point->x * vector->i + point->y * vector->j;
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

// score 443 (was 443): frame 0x60 versus 0x64; full/planar normal overlap, pathfinding/zero roles, byte flags and tails differ.
// Earlier: frame 0x5c versus 0x64; full/planar normal overlap, pathfinding/zero roles, byte flags and tails differ.
// pathfinding pointer in EBP and the -1 edge seed in EDX where ours keeps them in EBX
// and EDI, and retail keeps a byte local at [esp+0xe] where ours has a long zero at
// [esp+0x14].
// Earlier 419 (was 419): frame 0x5c versus 0x64, independent normal homes, memory flags and distinct exit
// Frame 0x5c versus 0x64, independent normal homes, memory flags and distinct output tails remain.
// Earlier: frame 0x5c versus 0x64, independent normal homes, memory flags and distinct exit
// tails remain; tried late first independent normal 490; late first native result word 438; native edge
// orientation 438; existing planar product inline 435; late first unsigned count 435.
// frame 0x5c versus 0x64; normal, result/first/hit homes and exit tails differ.
// Count byte 451; first word 443; success declared first 435; previous edge split 440; hit byte 435.
// Hit at producer 435; sector history storage 435; orientation word 434; planar scalar/value inputs 435/538;
// magnitude value input 556; unsigned success 435; normal length result 435.
// Final blocked output 658; signed result byte 460; endpoint array index 443; direct node flags 435;
// vertex base 458; accepted distance 476; vertex base plus distance 501; endpoint index plus distance 432.
// nonempty edge while 435; rejected hit reset 460; blocked success test 529; count first progress guard 435;
// edge endpoint homes 435; iterator storage lifetime 435; paired edge workspace 435; overshoot expression 434.
// Iterator native node/first seed/both 433/435/433; count traversal scope 419 kept (plain);
// progress-history scope 433; native planar/full-normal copies 490/505; planar input order 435.
// Only constant counter initialization moves into the valid-normal walk. Frame 0x5c versus 0x64
// and normal/result/exit homes remain.
// Existing point producer output return alone/with forced point expansion/with magnitude/with point and
// magnitude/with cross/with point and cross/with magnitude and cross/with all three 419 each; every sibling
// retains its preview baseline. Original helpers restored.
// Native cross axis/direction/both values 522/522/579; planar point2 input 419; planar plus axis/direction/both
// 522/522/579; orientation at every use 418 (one-point gain rejected).
// Existing planar point/vector and magnitude input reference contracts, singly and in all seven combinations, 419 each;
// existing cross-producer output reference 419. Original helpers and body restored.
// Complete named products: planar/cross/magnitude 421/455/421; planar and cross 453;
// planar and magnitude 419; cross and magnitude 453; all three 455; point offsets 418. All restored.
// Edge-test / iterator / passability call boundaries, ordinary inline versus noinline,
// eight combinations 419/435/460/460/435/435/460/460. All contracts and bodies restored.
// Complete endpoint stages 421; planar sum accumulator 421; normal-first products 419;
// stages with accumulator / normal-first products 421/421; named completed projection 421;
// stages with forced planar expansion 421; stages with native planar memcpy 483. All restored.
// Round 129, traversal-count scope retained: native planar copy with long/bool/short/byte
// result 479/543/483/427; CRT planar copy with those result types 479/543/483/427.
// All original executable source restored.
// Round 223: explicit false results at all three false tails, long/bool/short/byte
// 554/583/546/561; same result forms with independent planar memcpy 510/544/503/517.
// Distinct tails collapse further; frame and result homes remain unresolved. All restored.
// Round 234: normal/endpoint-scan/node-admission consumption endings and combinations
// 421/418/421/418/421/418/421; all endings with Boolean success 583. Original body restored.
// Round 226: Complete endpoint/distance, progress-distance and sector-history names,
// all seven subsets and the complete Boolean-role group 418 each. All names and body restored.
// Round 254: magnitude accumulator/squared-sum/real-result/accumulator-result
// 428/418/423/425; each with all three product parts 426/417/417/428. One-point
// reductions rejected; original helper, normal storage and body restored.
// Round 236: shared vector type / squared-length helper / point setter / vector+length /
// vector+point / length+point / all / all with native normal initialization
// 418/418/469/418/469/469/469/469; all restored.
// Round 239: actual blocked/admission/both byte fields with long result 418/418/418;
// with bool result 577/583/577; both char fields with long/bool result 418/577. All restored.
// Blocked-field forms also move the matched edge helper to 10.
// Earlier 404 (was 418): the trace result fields are stored in retail's order, the traversal count is an
// int, the first flag is a word, the edge direction a byte, and the accepted and blocked paths return
// their result directly. The frame 0x5c versus 0x64 and the remaining field and register rotations remain.
// Round 205: complete scalar input reads in cross first/second/both 492/472/482;
// magnitude alone/with first/with second/with both 400/492/469/482;
// all with planar input reads 482. Four-point magnitude gain rejected; original producers restored.
// Round 215: first-edge constant at sector/pre-iterator/pre-edge phases in word storage 426/426/427;
// bool storage 420/420/429; original-phase bool 417; pre-iterator byte 420. Original first word and
// producer phase restored; frame 0x5c/0x64, full/planar overlap and byte/tail homes remain.
// Round 434: valid-walk edge seed, post-initialization success/sector seeds and all domain subsets
// 422/404/421/404/421/403/422/422; last includes the early planar declaration. Frame remains 0x5c at 1605 bytes.
// Post-initialization success plus sector 403 is a rejected one-point form; original complete body restored.
// DL202: CRT planar transfer 440, frame 0x60 and seven additional caller losses;
// existing magnitude/planar/both widened results 404/495/495; unused cross return
// removed alone/with magnitude/with planar/with both 404/404/495/495.
// Original helpers, result contracts and body restored; frame 0x5c versus 0x64,
// independent normal homes, byte state and separate output tails remain.
// Round 245: complete point/vector captures via native/const copies, aggregate inputs
// and complete output publication 412/412/453/453/458/458/464/464; mixed input
// constructions 492/488. Native copies gain 31 but demote matched 0x26ccb0;
// output publication demotes eight controls. Original helper and entire body restored.
// Round 259: native returned cross vector, component/aggregate construction, mutable/const
// normal and explicit/aggregate planar initialization, eight combinations 597 each.
// All original helper contracts, normal storage and executable source restored.
// @retail 0x26c590
bool function_26c590(s_pathfinding_data const *pathfinding, point3f const *origin, long sector_index,
	long target_sector_index, vector3f const *direction, real distance, s_path_location const *location,
	s_path_trace_result *trace)
{
	s_sector_trace_result *result = (s_sector_trace_result *)trace;
	long success = false;
	long current_sector_index = sector_index;
	s_pathfinding_edge *edge = NULL;

	result->unknown20 = 3.4028235e38f;
	result->blocked = false;
	result->edge_index = (word)NONE;
	result->distance = distance;
	result->sector_index = NONE;
	result->unknown1c = false;
	real last_distance = -0.1f;
	if (pathfinding && sector_index != target_sector_index && sector_index >= 0 && sector_index < pathfinding->node_count)
	{
		vector3f normal;

		cross3f(g_4687b0, direction, &normal);
		if (magnitude3d(&normal) > 1e-5)
		{
			int count;
			count = 0;
			vector2f line_normal;
			real line_distance;
			long next_sector_index;
			next_sector_index = sector_index;

			line_normal.i = normal.i;
			line_normal.j = normal.j;
			line_distance = dot_product2d(origin, &line_normal);
			while (NONE != next_sector_index)
			{
				long previous_sector_index = current_sector_index;
				s_pathfinding_node const *sector;
				s_edge_iterator iterator;
				word first;
				bool hit = false;
				real distance1;
				point3f const *point1;

				current_sector_index = next_sector_index;
				next_sector_index = NONE;
				sector = &pathfinding->nodes[current_sector_index];
				if (!(sector->flags & 0x10) && !result->unknown1c &&
					(edge == 0 || (((s_sector_edge const *)edge)->flags & 0x200)))
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
					result->sector_index = current_sector_index;
					result->blocked = true;
					result->edge_index = (word)NONE;
					result->distance = distance;
					function_x697631(origin, direction, distance, &result->point);
					bool tmp1 = result->blocked;
					success = tmp1;
					goto done;
				}
				first = true;
				do
				{
					byte forward = iterator.forward;
					point3f const *point0;
					real distance0;

					if (iterator.previous_surface_index != NONE)
					{
						long tmp3 = iterator.vertex;
						point1 = &pathfinding->vertices[tmp3];
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
							distance0 = distance1;
							point0 = point1;
						}
						point1 = &pathfinding->vertices[iterator.vertex];
					}
					distance1 = dot_product2d(point1, &line_normal) - line_distance;
					word tmp2 = iterator.edge_index;
					hit = function_26c380(origin, pathfinding, forward, current_sector_index, tmp2,
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
				if ((bool)hit)
				{
					if (result->distance - last_distance < -0.1 && count > 0)
					{
						break;
					}
					last_distance = result->distance;
					if (result->distance - distance > 0.0001)
					{
						long tmp0 = result->sector_index;
						if (target_sector_index != NONE && target_sector_index != tmp0)
						{
							if (next_sector_index == target_sector_index && result->distance - distance < 0.01)
							{
								result->sector_index = next_sector_index;
								result->blocked = false;
								result->edge_index = (word)NONE;
								result->distance = (real)distance;
								function_x697631(origin, direction, distance, &result->point);
								goto done;
							}
							else
							{
								result->distance = 0.0f;
								result->blocked = true;
								result->edge_index = (word)NONE;
								function_x697631(origin, direction, distance, &result->point);
								return result->blocked;
							}
						}
						else
						{
							result->blocked = false;
							result->distance = distance;
							result->edge_index = (dword)NONE;
							function_x697631(origin, direction, distance, &result->point);
							goto done;
						}
					}
					if (result->blocked)
					{
						success = hit;
						goto done;
					}
					if (location && (((s_sector_edge const *)edge)->flags & 0x80) &&
						function_270600(iterator.edge_index, location))
					{
						result->blocked = true;
						success = (bool)hit;
						goto done;
					}
					if (count > 200)
					{
						break;
					}
					++count;
				}
				else
				{
					result->sector_index = current_sector_index;
					result->blocked = true;
					result->edge_index = (word)NONE;
					result->distance = distance;
					function_x697631(origin, direction, distance, &result->point);
					success = result->blocked;
					goto done;
				}
			}
		}
	}
	result->edge_index = (word)NONE;
	result->sector_index = current_sector_index;
	result->blocked = false;
	result->distance = distance;
	function_x697631(origin, direction, distance, &result->point);
done:
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

/* Keep the local second component read separate from the first component store. */
PRIVATE inline real trace_normalize_direction(vector3f *vector)
{
	real distance = (real)sqrt(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
	if (!(fabs(distance) < 0.0001f))
	{
		real scale = 1.0f / distance;
		vector->i = scale * vector->i;
		vector->j = ((vector3f volatile *)vector)->j * scale;
		vector->k = vector->k * scale;
	}
	else
		distance = 0.0f;
	return distance;
}

struct s_type_c3b527;

// @retail 0x26c4e0
bool function_26c4e0(s_pathfinding_data *pathfinding, s_type_c3b527 const *start, long sector_index,
	s_type_c3b527 const *end, long target_sector_index, long location, s_path_trace_result *trace)
{
	point3f const *origin = (point3f const *)start;
	point3f const *target = (point3f const *)end;
	vector3f direction;
	direction.i = target->x - origin->x;
	direction.j = target->y - origin->y;
	direction.k = 0.0f;
	real distance = trace_normalize_direction(&direction);
	return function_26c590(pathfinding, origin, sector_index, target_sector_index,
		&direction, distance, (s_path_location const *)location, trace);
}

// @retail 0x26cbe0
bool function_26cbe0(s_pathfinding_data const *pathfinding, point3f const *origin,
	point3f const *target, long sector_index, long target_sector_index,
	s_path_location const *location)
{
	vector3f direction;
	vector3d_from_points3d(origin, target, &direction);
	real distance = (real)sqrt(direction.i * direction.i + direction.j * direction.j + direction.k * direction.k);
	if (!(fabs(distance) < 0.0001f))
	{
		real scale = 1.0f / distance;
		direction.i *= scale;
		/* Preserve the component access order while scaling the local direction. */
		((vector3f volatile *)&direction)->j = ((vector3f volatile *)&direction)->j * scale;
		direction.k = ((vector3f volatile *)&direction)->k * scale;
	}
	else
		distance = 0.0f;
	if (distance > 0.0f)
	{
		s_path_trace_result trace;
		return function_26c590(pathfinding, origin, sector_index, target_sector_index,
			&direction, distance, location, &trace);
	}
	return false;
}

struct s_obstacle_list;
struct s_obstacle_hit
{
	real distance;
	short index;
	short group;
};

struct s_avoidance_trace
{
	real distance;
	long sector;
	long edge;
	short obstacle;
	short group;
};

bool obstacle_list_cast_ray(s_obstacle_list const *list, short ignore_index, point2f const *origin,
	point2f const *direction, real radius, real maximum_distance, bool ignore_flagged, s_obstacle_hit *hit);

// @retail 0x26ccb0
bool __stdcall function_26ccb0(s_pathfinding_data const *pathfinding, s_obstacle_list const *obstacles,
	short ignored_obstacle, point2f const *origin, long sector, long target_sector, point2f const *direction,
	real radius, real distance, bool first, bool stop_at_goal, bool ignore_flagged,
	s_path_location const *location, s_avoidance_trace *trace)
{
	bool hit = false;
	trace->distance = distance;
	trace->sector = NONE;
	trace->edge = 0xffff;
	trace->obstacle = NONE;
	trace->group = NONE;
	if (sector == NONE || sector == 0xffff)
		return true;
	real limit = stop_at_goal ? distance - radius : distance;
	point3f start = {origin->x, origin->y, 0.0f};
	vector3f forward = {direction->x, direction->y, 0.0f};
	s_sector_trace_result offset, side, center;
	if (!first)
	{
		if (!(pathfinding->nodes[sector].flags & 0x10))
			return true;
		point3f end;
		function_x697631(&start, &forward, distance, &end);
		if (function_26c590(pathfinding, &start, sector, target_sector, &forward, distance, location,
			(s_path_trace_result *)&center) &&
			(center.distance < limit || (target_sector != NONE && center.sector_index != target_sector)))
		{
			hit = true;
			trace->distance = center.distance;
			trace->edge = center.edge_index;
		}
		vector3f lateral = {0.0f - direction->y, direction->x, 0.0f};
		function_26c590(pathfinding, &start, sector, NONE, &lateral, radius, NULL, (s_path_trace_result *)&offset);
		if (function_26c590(pathfinding, &offset.point, offset.sector_index, NONE, &forward,
			trace->distance, location, (s_path_trace_result *)&side) &&
			side.distance < trace->distance && side.distance < limit &&
			(center.blocked || function_26cbe0(pathfinding, &side.point, &end, side.sector_index, target_sector, location)))
		{
			hit = true;
			trace->distance = side.distance;
			trace->edge = side.edge_index;
		}
		lateral.i *= -1.0f;
		lateral.j *= -1.0f;
		lateral.k *= -1.0f;
		function_26c590(pathfinding, &start, sector, NONE, &lateral, radius, NULL, (s_path_trace_result *)&offset);
		if (function_26c590(pathfinding, &offset.point, offset.sector_index, NONE, &forward,
			trace->distance, location, (s_path_trace_result *)&side) &&
			side.distance < trace->distance && side.distance < limit &&
			(center.blocked || function_26cbe0(pathfinding, &side.point, &end, side.sector_index, target_sector, location)))
		{
			hit = true;
			trace->distance = side.distance;
			trace->edge = side.edge_index;
		}
	}
	if (obstacles)
	{
		s_obstacle_hit obstacle;
		if (obstacle_list_cast_ray(obstacles, ignored_obstacle, origin, direction, radius, distance, ignore_flagged, &obstacle) &&
			obstacle.distance < trace->distance && obstacle.distance < limit)
		{
			hit = true;
			trace->distance = obstacle.distance;
			trace->edge = 0xffff;
			trace->obstacle = obstacle.index;
			trace->group = obstacle.group;
		}
	}
	function_26c590(pathfinding, &start, sector, target_sector, &forward, trace->distance, location,
		(s_path_trace_result *)&center);
	trace->sector = center.sector_index;
	return hit;
}

struct s_trace_structure_view
{
	byte unknown00[0xc4];
	long count;
	s_pathfinding_data *pathfinding;
};

// @retail 0x26d290
bool function_26d290(point3f const *origin, point3f const *target, long sector_index, long *output_sector)
{
	vector3f direction;
	direction.i = target->x - origin->x;
	direction.j = target->y - origin->y;
	direction.k = 0.0f;
	real distance = trace_normalize_direction(&direction);
	bool result;
	if (distance > 0.0f)
	{
		s_trace_structure_view *structure = (s_trace_structure_view *)g_4e0348;
		s_pathfinding_data *pathfinding = NULL;
		if (structure->count > 0)
			pathfinding = structure->pathfinding;
		s_path_trace_result trace;
		result = !function_26c590(pathfinding, origin, sector_index, NONE,
			&direction, distance, NULL, &trace);
		if (result)
			*output_sector = ((s_sector_trace_result *)&trace)->sector_index;
	}
	else
	{
		result = true;
		*output_sector = sector_index;
	}
	return result;
}

struct s_collision_result_1697c0;
struct s_path_collision
{
    long type;
    real fraction;
    point3f point;
    byte field_14[0x3c - 0x14];
    long structure_index;
    long object_index;
    long field_44;
    long node_index;
    long field_4c;
    long surface_index;
    long field_54;
};
struct s_path_node_point
{
    point3f point;
    short output_index;
    short field_e;
};
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
    long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);
long function_1fa3a0(long structure_index, long surface_index, long object_index,
    long node_index, point3f const *point);
short function_210310(long object_index, long a);
bool function_210690(short output_index, point3f const *point, point3f *out);

// score 28 (was 28): retail computes start before direction, sets the NONE result before the call
// setup and keeps point on the stack. Round 61: with the parameters as (up, point, output, collision)
// (point then needs no `&point` read to stay on the stack), start first and result declared first it
// matches (fast), and so does 0x1c19b0, every matched caller kept: BLOCKED-BY 0x270400 (two call lines
// in contributor code). With this order: point read directly 35, plus start first 2. With this
// signature: start and result first 37; that through one `point3f const *` read of `&point` 37;
// point read directly 28 here but five matched callers lose their argument setup.
// Round 52 (fast only, not kept): an unused last parameter (the pathfinding data, which 0x26d0e0 receives as its
// unused third argument and 0x26d570 computes before its call) is dropped by LTCG: every caller's code unchanged,
// 28. Kept only with every call passing it: BLOCKED-BY 0x270400 (two call lines in contributor code); a
// defaulted parameter would avoid them but needs a ruling. The plain check hit a Wine hang; body restored.
// @retail 0x26d100
long function_26d100(vector3f const *up, long *unknown, s_collision_result_1697c0 *collision, point3f const *point)
{
    point3f const *const *point_reference = &point;
    vector3f direction;
    direction.i = up->i * -4.0f;
    direction.j = up->j * -4.0f;
    direction.k = up->k * -4.0f;
    point3f start;
    start.x = up->i * 0.5f + (*point_reference)->x;
    start.y = up->j * 0.5f + (*point_reference)->y;
    start.z = up->k * 0.5f + (*point_reference)->z;
    s_path_collision *hit = (s_path_collision *)collision;
    s_path_node_point *output = (s_path_node_point *)unknown;
    long result = NONE;
    if (function_1697c0(0x84000d, &start, &direction, NONE, NONE, collision))
    {
        result = function_1fa3a0(hit->structure_index, hit->surface_index, hit->object_index,
            hit->node_index, &start);
        if (output)
        {
            if (hit->object_index == NONE)
            {
                output->point = hit->point;
                output->output_index = NONE;
            }
            else
            {
                short index = function_210310(hit->object_index, hit->node_index);
                if (index == NONE)
                {
                    output->point = hit->point;
                    output->output_index = index;
                }
                else if (function_210690(index, &hit->point, &output->point))
                    output->output_index = index;
                else
                {
                    output->point = hit->point;
                    output->output_index = NONE;
                }
            }
        }
    }
    else if (output)
    {
        output->point = **point_reference;
        output->output_index = NONE;
    }
    return result;
}

struct s_downward_collision_result
{
    s_path_collision collision;
    long field58;
};

// @retail 0x26d0e0
long __stdcall function_26d0e0(point3f const *point, s_type_c3b527 *output, long unused)
{
    s_downward_collision_result collision;
    // retail keeps the unused third parameter, so it returns with ret 4
    long const *unused_reference = &unused;
    (void)unused_reference;
    *(short *)((byte *)&collision + 0x24) = NONE;
    return function_26d100(g_4687b0, (long *)output, (s_collision_result_1697c0 *)&collision, point);
}
