/* UNKNOWN_2C3550.CPP: the search for a way around the obstacles in a path's
   way: its nodes, kept in a binary heap ordered by cost */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "unknown_2c0d00.h"
#include "unknown_26c380.h"
#include "globals.h"
#include <math.h>
#include <string.h>
#include "vector2f_products.h"

// @flags /O2 /arch:SSE /Gr

/* a node of the search (0x34 bytes) */
struct s_avoidance_node
{
	point2f position;
	long value08;
	/* the direction from the point to the search's goal */
	point2f direction;
	/* the distance from the point to the search's goal */
	real distance;
	short obstacle;
	short value1a;
	bool side;
	byte unknown1d;
	/* the next nodes around the obstacle, on either side */
	short children[2];
	byte unknown22[2];
	real cost;
	short parent;
	/* how many of its children must be reached before it is */
	short required_count;
	short reached_count;
	short value;
	bool marked;
	byte unknown31[3];
};

/* the search's state */
struct s_avoidance_search
{
	real radius;
	bool value04;
	byte unknown05[3];
	s_obstacle_list *obstacles;
	s_match_globals const *value0c;
	point2f goal;
	long value18;
	/* the obstacle that holds the goal */
	short goal_obstacle;
	short value1e;
	short best_index;
	byte unknown22[2];
	real best_distance;
	bool value28;
	bool value29;
	short node_count;
	s_avoidance_node nodes[64];
	short heap_count;
	short heap[64];
	short result;
};

/* moves a heap entry up while its parent costs more */
// @retail 0x2c3550
void avoidance_heap_sift_up(s_avoidance_search *search, short index)
{
	while (index > 0)
	{
		short parent = (index - 1) >> 1;
		short parent_node = search->heap[parent];

		if (!(search->nodes[parent_node].cost > search->nodes[search->heap[index]].cost))
		{
			break;
		}
		search->heap[parent] = search->heap[index];
		search->heap[index] = parent_node;
		index = parent;
	}
}

/* moves a heap entry down while a child costs less */
// @retail 0x2c35c0
void avoidance_heap_sift_down(s_avoidance_search *search, short index)
{
	if (index >= search->heap_count)
	{
		return;
	}
	for (;;)
	{
		short count = search->heap_count;
		short smallest = index;
		short child = index * 2 + 1;

		if (child < count && search->nodes[search->heap[index]].cost > search->nodes[search->heap[child]].cost)
		{
			smallest = child;
		}
		child = index * 2 + 2;
		if (child < count && search->nodes[search->heap[smallest]].cost > search->nodes[search->heap[child]].cost)
		{
			smallest = child;
		}
		if (smallest == index)
		{
			break;
		}
		{
			short node = search->heap[index];
			short other = search->heap[smallest];

			search->heap[smallest] = node;
			search->heap[index] = other;
		}
		index = smallest;
	}
}

/* marks the node and its parents back to the root, as far as each parent
   has had all its children reached; the root's value is the result */
// @retail 0x2c3cc0
void avoidance_mark_path(s_avoidance_search *search, short index, short value)
{
	while (index != NONE)
	{
		s_avoidance_node *node = &search->nodes[index];
		s_avoidance_node *parent;

		node->marked = true;
		if (node->value == NONE && value != NONE)
		{
			node->value = value;
		}
		if (node->parent == NONE)
		{
			search->result = node->value;
			return;
		}
		parent = &search->nodes[node->parent];
		parent->reached_count++;
		if (parent->reached_count < parent->required_count)
		{
			return;
		}
		index = node->parent;
	}
}

static __forceinline real avoidance_normalize2d(point2f *direction)
{
	real distance = (real)sqrt(direction->x * direction->x + direction->y * direction->y);

	if (!(fabs(distance) < 0.0001f))
	{
		real inverse = 1.0f / distance;

		direction->x = inverse * direction->x;
		direction->y = direction->y * inverse;
	}
	else
	{
		distance = 0.0f;
	}
	return distance;
}

/* the z component of a vector given by its components crossed with the direction */
static __forceinline real avoidance_cross2d(point2f const *direction, real x, real y)
{
	return x * direction->y - y * direction->x;
}

/* adds a node at the point, passing the obstacle on the side, unless it
   leads back around an obstacle on its way; returns its index or NONE */
// @retail 0x2c3680
short avoidance_add_node(s_avoidance_search *search, point2f const *point, bool side, long value08, short obstacle,
	short value1a, real cost, short parent)
{
	short result = NONE;

	if (search->node_count < 64)
	{
		bool reaches_goal_obstacle = false;
		point2f direction;
		real distance;
		short index;
		s_avoidance_node *node;

		vector2d_from_points2d(point, &search->goal, &direction);
		distance = avoidance_normalize2d(&direction);
		for (index = parent; index != NONE; index = node->parent)
		{
			node = &search->nodes[index];
			if (node->obstacle != obstacle)
			{
				if (obstacle == search->goal_obstacle && search->goal_obstacle != NONE)
				{
					short sibling;
					short *child;

					reaches_goal_obstacle = true;
					sibling = node->children[!side];
					if (sibling != NONE)
					{
						s_avoidance_node *other = &search->nodes[sibling];

						if (dot_product2d((vector2f const *)&other->direction, (vector2f const *)&direction) > 0.0f)
						{
							s_avoidance_node *from = &search->nodes[parent];

							if (0.0f > cross_product2d((vector2f const *)&other->direction, (vector2f const *)&from->direction) *
								avoidance_cross2d(&direction, other->direction.x, other->direction.y))
							{
								goto done;
							}
						}
					}
					child = &node->children[side];
					if (*child == parent || *child == NONE)
					{
						*child = search->node_count;
					}
				}
				break;
			}
			if (node->side != side)
			{
				goto done;
			}
			if (obstacle == search->goal_obstacle && search->goal_obstacle != NONE && distance > node->distance)
			{
				goto done;
			}
		}
		index = search->node_count++;
		node = &search->nodes[index];
		node->position.x = point->x;
		node->position.y = point->y;
		node->value08 = value08;
		node->direction = direction;
		node->value1a = value1a;
		node->required_count = 0;
		node->reached_count = 0;
		node->value = NONE;
		node->distance = distance;
		node->obstacle = obstacle;
		node->side = side;
		node->cost = node->distance + cost;
		node->parent = parent;
		node->marked = false;
		memset(node->children, NONE, sizeof(node->children));
		if (reaches_goal_obstacle && search->best_distance > node->distance)
		{
			search->best_distance = node->distance;
			search->best_index = index;
		}
		if (search->heap_count < 64)
		{
			short heap_index = search->heap_count++;

			search->heap[heap_index] = index;
			avoidance_heap_sift_up(search, heap_index);
		}
		result = index;
	}
done:
	return result;
}

/* starts a search from the start to the goal around the obstacles, and
   flags the obstacles the goal is within reach of */
// @retail 0x2c3900
void avoidance_search_begin(s_avoidance_search *search, bool value04, s_obstacle_list *obstacles, s_match_globals const *value0c, real radius,
	point2f const *start, long value08, point2f const *goal, long value18, bool value29)
{
	short containing;

	search->radius = radius;
	search->obstacles = obstacles;
	search->value0c = value0c;
	search->value04 = value04;
	search->value28 = false;
	search->goal = *goal;
	search->value18 = value18;
	containing = obstacle_list_find_containing(obstacles, NONE, goal, radius);
	search->goal_obstacle = containing != NONE ? obstacles->obstacles[containing].group : NONE;
	search->value29 = value29;
	search->value1e = NONE;
	search->best_index = NONE;
	search->result = NONE;
	search->best_distance = 3.4028235e38f;
	search->node_count = 0;
	search->heap_count = 0;
	avoidance_add_node(search, start, false, value08, NONE, NONE, 0.0f, NONE);
	for (short i = 0; i < obstacles->count; i++)
	{
		s_obstacle *obstacle = &obstacles->obstacles[i];
		real dx = obstacle->center.x - goal->x;
		real dy = obstacle->center.y - goal->y;
		real distance = dx * dx;

		distance += dy * dy;
		if ((real)sqrt(distance) < radius + obstacle->radius)
		{
			obstacle->flags |= 2;
		}
	}
}

struct s_avoidance_trace
{
	real distance;
	long sector;
	long edge;
	short obstacle;
	short group;
};

struct s_path_location;
bool __stdcall function_26ccb0(s_pathfinding_data const *pathfinding, s_obstacle_list const *obstacles,
	short ignored_obstacle, point2f const *origin, long sector, long target_sector, point2f const *direction,
	real radius, real distance, bool first, bool stop_at_goal, bool value29, s_path_location const *location, s_avoidance_trace *trace);
void obstacle_tangent_directions(point2f const *point, s_obstacle_list const *list, short index, real radius,
	point2f *left, point2f *right, real *tangent_length);

static inline bool avoidance_bit_test(dword const *bits, long index)
{
	return (bits[index >> 5] & (1 << (index & 31))) != 0;
}

static inline void avoidance_bit_set(dword *bits, long index)
{
	bits[index >> 5] |= 1 << (index & 31);
}

static __forceinline point2f *avoidance_scale2d(point2f const *direction, real distance, point2f *scaled)
{
	scaled->x = distance * direction->x;
	scaled->y = direction->y * distance;
	return scaled;
}

static __forceinline point2f *avoidance_point_along2d(point2f const *origin, point2f const *direction,
	real distance, point2f *point)
{
	point->x = direction->x * distance + origin->x;
	point->y = direction->y * distance + origin->y;
	return point;
}

static __forceinline real avoidance_length2d(point2f const *direction)
{
	real square = direction->x * direction->x;

	square += direction->y * direction->y;
	return (real)sqrt(square);
}

/* Flood the obstacles met by either tangent and add a node on each clear side. */
// @retail 0x2c39f0
short __stdcall avoidance_expand_obstacle(s_pathfinding_data const *pathfinding, s_avoidance_search *search,
	short node_index, short obstacle_index)
{
	s_avoidance_node *node = &search->nodes[node_index];
	dword visited[2];
	short pending[64];
	short pending_count;
	short added;

	memset(visited, 0, ((search->obstacles->count + 31) >> 5) * sizeof(dword));
	avoidance_bit_set(visited, obstacle_index);
	added = 0;
	pending[0] = obstacle_index;
	pending_count = 1;
	do
	{
		pending_count--;
		short current = pending[pending_count];
		short group = current != NONE ? search->obstacles->obstacles[current].group : NONE;
		point2f directions[2];
		real tangent_length;

		obstacle_tangent_directions(&node->position, search->obstacles, current, search->radius,
			&directions[0], &directions[1], &tangent_length);
		if (search->radius > tangent_length)
		{
			tangent_length = search->radius;
		}
		real distance = search->obstacles->obstacles[current].radius + search->radius + tangent_length;
		for (short side = 0; side < 2; side++)
		{
			s_avoidance_trace trace;

			function_26ccb0(pathfinding, search->obstacles, current, &node->position, node->value08, NONE,
				&directions[side], search->radius, distance, false, false, search->value29, false, &trace);
			if (trace.obstacle != NONE && !avoidance_bit_test(visited, trace.obstacle))
			{
				avoidance_bit_set(visited, trace.obstacle);
				pending[pending_count++] = trace.obstacle;
			}
			if (trace.distance > tangent_length && trace.group != group)
			{
				distance = (trace.distance + tangent_length) * 0.5f;
				point3f origin;
				vector3f direction;
				s_path_trace_result sector_trace;

				origin.x = node->position.x;
				origin.y = node->position.y;
				origin.z = 0.0f;
				direction.i = directions[side].x;
				direction.j = directions[side].y;
				direction.k = 0.0f;
				function_26c590(pathfinding, &origin, node->value08, NONE, &direction, distance, NULL, &sector_trace);
				if (avoidance_add_node(search, (point2f const *)&sector_trace.point, side > 0,
					((s_sector_trace_result *)&sector_trace)->sector_index, group, obstacle_index,
					node->cost - node->distance + distance, node_index) != NONE)
				{
					added++;
				}
			}
		}
	}
	while (pending_count > 0);
	return added;
}

// @retail 0x2c3d40
bool avoidance_search_step(s_avoidance_search *search, s_pathfinding_data const *pathfinding, bool allow_partial)
{
	if (search->heap_count > 0)
	{
		short index = search->heap[0];

		search->heap_count--;
		search->heap[0] = search->heap[search->heap_count];
		avoidance_heap_sift_down(search, 0);
		if ((short)index != NONE)
		{
			s_avoidance_node *node = &search->nodes[(short)index];
			short added = 0;
			s_avoidance_trace trace;

			function_26ccb0(pathfinding, search->obstacles, NONE, &node->position, node->value08,
				search->value18, &node->direction, search->radius, node->distance, node->parent == NONE,
				true, search->value29, false, &trace);
			if (trace.edge == 0xffff)
			{
				if (trace.obstacle == NONE)
				{
					if (trace.sector == search->value18 || node->distance < 0.2f)
					{
						point2f point;

						avoidance_point_along2d(&node->position, &node->direction, trace.distance, &point);
						search->value1e = avoidance_add_node(search, &point, false, search->value18, NONE, NONE,
							node->cost - node->distance + trace.distance, (short)index);
						added = 1;
					}
				}
				else
				{
					point2f const *direction = &node->direction;
					s_obstacle *obstacle = &search->obstacles->obstacles[trace.obstacle];

					if (allow_partial && trace.group == search->goal_obstacle)
					{
						point2f offset;
						point2f point;
						point2f movement;

						offset.x = search->goal.x - obstacle->center.x;
						offset.y = search->goal.y - obstacle->center.y;
						point = node->position;
						avoidance_scale2d(direction, trace.distance, &movement);
						point.x += movement.x;
						point.y += movement.y;
						real dx = point.x - search->goal.x;
						real dy = point.y - search->goal.y;
						if (dy * dy + dx * dx < obstacle->radius * obstacle->radius &&
							obstacle->radius - sqrt(offset.y * offset.y + offset.x * offset.x) < obstacle->radius * 0.5f)
						{
							search->value1e = avoidance_add_node(search, &point, false, search->value18, NONE, NONE,
								node->cost, (short)index);
							added = 1;
						}
					}
					if (!added)
					{
						if (trace.group == search->goal_obstacle && search->best_distance > node->distance &&
							node->obstacle == trace.group)
						{
							search->best_distance = node->distance;
							search->best_index = (short)index;
						}
						added = avoidance_expand_obstacle(pathfinding, search, (short)index, trace.obstacle);
					}
				}
			}
			else if (node->value1a != NONE)
			{
				added = avoidance_expand_obstacle(pathfinding, search, (short)index, node->value1a);
				trace.obstacle = node->value1a;
			}
			node->required_count = added;
			if (!added)
			{
				long group = NONE;

				if (trace.obstacle != NONE)
				{
					group = search->obstacles->obstacles[trace.obstacle].group;
				}

				avoidance_mark_path(search, (short)index, (short)group);
			}
		}
	}
	return search->value1e == NONE && search->heap_count > 0;
}

struct s_avoidance_limits
{
	byte unknown00[0x6c];
	real distance;
};

// @retail 0x2c4030
bool avoidance_search(s_pathfinding_data const *pathfinding, s_avoidance_search *search,
	s_avoidance_limits const *limits, bool value04, s_obstacle_list *obstacles, real radius,
	point2f const *start, long sector, point2f const *goal, long target_sector, bool allow_partial, bool value29)
{
	avoidance_search_begin(search, value04, obstacles, g_4e0348, radius, start, sector, goal, target_sector, value29);
	while (avoidance_search_step(search, pathfinding, allow_partial))
	{
	}
	if (search->value1e != NONE)
	{
		search->value28 = true;
	}
	else if (search->best_index != NONE)
	{
		if (allow_partial)
		{
			s_avoidance_node *node = &search->nodes[search->best_index];
			bool accept = limits->distance > 0.0f && limits->distance > node->distance;

			if (!accept)
			{
				s_obstacle *obstacle = &obstacles->obstacles[node->value1a];
				point2f offset;

				offset.x = goal->x - obstacle->center.x;
				offset.y = goal->y - obstacle->center.y;
				accept = obstacle->radius > node->distance &&
					obstacle->radius - avoidance_length2d(&offset) < obstacle->radius * 0.5f;
			}
			if (accept)
			{
				point2f point;

				real distance = node->distance;
                point.x = distance * node->direction.x + node->position.x;
                point.y = node->direction.y * distance + node->position.y;
				search->value1e = avoidance_add_node(search, &point, false, search->value18, NONE, NONE,
					node->distance, search->best_index);
			}
		}
		else
		{
			search->value1e = search->best_index;
		}
	}
	return search->value1e != NONE;
}



#include "path.h"
#include "slot_handler.h"
struct s_path_step_view
{
 short type;
 short unknown02;
 long node_index;
 short link_index;
 short unknown0a;
 s_type_c3b527 point;
};
real function_30bf0(vector3f *vector);
bool function_2104b0(short index, point3f const *point, point3f *output);
bool function_210690(short index, point3f const *point, point3f *output);
void __stdcall function_2c0d60(vector3f const *previous_direction, long actor_index, byte flags,
 s_path_settings const *settings, s_obstacle_list *obstacles, s_obstacle_list *blocking_obstacles,
 point3f const *position, real radius, vector3f const *direction, long unit_index, long ignore_index);
void obstacle_list_group(s_obstacle_list *obstacles, real radius);

bool obstacle_list_add(s_obstacle_list *list, word flags, point2f const *center, long object_index, real radius);

// MiniMax R (11-14): no lower score (675): the permuter ran over this body (300 s, three tries) and
// never beat its baseline; the frame 0x1f88 and the length agree, retail keeps the success result in
// a stack byte where ours keeps a dword, loads the source radius before the 0.2f constant, and the
// path pointer and the obstacle, search and step homes differ.
// MiniMax G (11-10): no new form, no lower score (675).
// score 675 (was 675): the 675 differing lines are 541 real ones (the rest are the unmatched
// MiniMax I (11-06): retail's prologue interleaves the compare with the pushes and keeps the
// pathfinding read after the radius block; no new form, no lower score.
// MiniMax A (11-04): pathfinding read after the radius block 674, final_complete below the height 697,
// radius as one ternary (retail's load order) 704, pathfinding after the radius with a short frame
// identifier 703, long step counter 764, bool result 736 and 756 with that order, the type guard split
// into three sibling tests as retail does (setup, search-or-copy, object-index pass) 770; the permuter
// (113 tries, annealed) found no variant below 675; no keepable gain.
// callees and the relocated globals); the frame 0x1f88 and the length agree, but retail keeps the
// success result in a stack byte where ours keeps a dword, loads the source radius before the 0.2f
// constant, and the path pointer and the obstacle, search and step homes differ.
// Earlier: the frame 0x1f88 and the length agree, but retail keeps the success result
// in a stack byte where ours keeps a dword, loads the source radius before the 0.2f constant, and
// the path pointer and the obstacle, search and step homes differ.
// Round 134: retail's result is a stack byte (ebx = pathfinding, then NONE): bool result 757; radius as
// one ternary (retail's load order, one store) 744, with bool result 773, with the pathfinding read after
// the radius 740, all three 775.
// Frame 0x1f88 agrees; entry completion/result/path and obstacle/search/step homes differ.
// Earlier 711 from 735: uniform native forward obstacle indices recover the frame.
// Frame 0x1f88 agrees; radius/pathfinding entry schedule, success-result and avoidance-workspace homes differ.
// frame 0x1f88 agrees; result/path pointer and obstacle/search/step homes differ.
// Short/signed byte result 740/788; long/byte inserted count 730/741; word flags 742.
// Long inserted count has retail frame but 2965 bytes; native reverse index 770/2925 bytes; original length preserved.
// Round 121: Byte last-step predicate 735 kept; insertion-only/all avoidance storage scopes 742/763; Boolean enable bit 740; word retry 740; bit plus retry 742; byte-last plus bit/retry/both 735/737/734. Extra one-point combination rejected.
// Shared slot predicate 782; chained list resets 742; complete step memcpy transfers 753;
// unsigned word result 740 (two-point gain rejected); direct search decision 742 (fast).
// Original complete body and existing insertion helper call restored.
// Late constant success 771; early completion observation 740; unsigned-word input ordinal 775;
// word truncation result 754; unsigned-word inserted count 742 (fast). Two-point gain rejected.
// Round 155: double radius/height and short completion storage, then unsigned flags with double height: radius 777; height 728; radius height 797; completion 757; radius completion 795; height completion 758; radius height completion 769; height native mask 728. Double-height forms add qword x87 traffic and frame 0x1f98; all restored.
// Settings mask/cache/both at uses 735/736/736; all five forward obstacle indices 711;
// byte truncation 735; mask with indices 711; cache with indices 719; all observations/indices/byte 719.
// Native index in obstacle-transform/blocking-transform/retry/failure/ignored loops alone
// 735/756/723/735/735. Uniform native forward indices selected; reverse/output/step indices unchanged.
// Initial incompleteness bool/byte/word/long 711/711/752/770; with bool success
// 766/766/773/754. Original saved completion, long result and all native indices restored.
// Round 155 on native forward-index frame: bool/byte success 757/774; radius before pathfinding 710;
// radius with bool/byte result 759/776; later long/bool/byte result 710/777/786. One-point gains rejected.
// Negative failure bool/byte/short/long, original entry order: 774/774/766/766;
// negative failure with radius before pathfinding: 775/775/762/762 (fast).
// Original success result, insertion path and five native forward indices restored.
// Pathfinding at every use 785; one actual transform workspace 760; both 825; paired segment start/node 735;
// paired with live pathfinding/shared scratch/both 777/763/812; complete receiver graph with word frame 810. Original source restored.
// Radius from field or ternary, before/after path and long/bool success:
// 766/773/764/774 and 744/773/740/775; original selection restored.
// Round 354 recovered-frame graph: shared 3D/2D/both difference producers 711/711/711;
// native signed-word frame alone/with 3D/with 2D/with both 709/709/709/709 (two-count gains dropped);
// complete producer graph with Boolean success 750. Original complete body restored.
// Earlier native retry Boolean 675 from 705. Round 282: goal insertion helper, original/short
// forward indices and long/Boolean success [712,795,748,812]; flag-count before count
// publication with the same storage combinations [675,754,737,777]. Original complete body restored.
// Round 171: actual first/retry searches produce the returned result; bool/byte/short/long
// with long retry 773/751/753/727, with bool retry 761/752/748/722.
// Shared slot-enabled predicate on the native forward-index frame 777. Original complete body restored.
// Round 72 (lane 8, plain checks): retail picks start and start_node with one test (an if/else
// assigning both) after the out-count test, then computes the step pointer, keeps the frame index
// as a short ([esp+0x40] word store) and zeroes first_output at entry with flags. The if/else alone
// 707 (2976 B), with the step pointer after it 750 (2960 B); first_output zeroed at entry 700; short
// frame index 704. All restored.
// Round 191: native byte success with completion/ordinal/segment scalar identity groups,
// all eight subsets 754 each. Lexical scalar identities do not recover the byte/word
// homes; original dword success and all original identities restored.
// Round 203: Actual geometry producers: existing point setter, all complete point memcpy transfers,
// prior-direction memcpy, and all seven nonempty combinations 675 each; complete memmove graph 779/2942
// bytes. Geometry transfers canonicalize without recovering byte success, radius/pathfinding ownership or
// scalar homes. Original body, helpers, copies and all five native forward indices restored.
// Round 201: Existing search inline/default and stdcall 675; cdecl variants 680 and lose matched
// 2c4030. Boolean success with inline/cdecl/stdcall 754/753/754. Existing actor getter, exact
// shared magnitude, actual const actor record and all seven native producer combinations 675;
// complete graph with Boolean success 754. Original body, includes, layout assertions and search
// declaration restored. Reduced control 2c39f0 already differs in the unchanged preview; no full-
// image loss accepted.
// Round 232: Once-read radius/options/both, per-use flags alone/with radius/with options/with both, then all with Boolean success
// 754, 748, 773, 675, 754, 768, 773, 786. Original executable source restored.
// T47: Input-step cursor / forward inserted-step cursor / repeated blocking-segment receiver
// subsets 730/728/766/675/730/728/766; all with Boolean success 752. Original body restored.
// Round 260: Six direction/previous-direction/world-start declaration schedules 675/675/675/675/675/675;
// reversed schedule with bool/byte success 754/768. Original evaluations and entire body restored.
// Round 261: all six relative lexical schedules of direction/previous-direction/world-start
// 675/675/675/675/675/675; world/direction/previous with Boolean success and then signed-word frame
// 754/751. Original names, types and complete body restored; byte success and scalar/geometry homes remain.
// Round 481: Independent obstacle/blocking/source transform-output lexical identities, all seven
// subsets 675 each; complete transform graph with travel-direction identity 675. Original body restored.
// Round 115 (lane 7, fast; aligned = difflib count, 601 at 675): bool result re-measured 754 (aligned 609; the
// epilogue is then duplicated and the failure path threads to its own `xor al, al` exit as in retail, but
// inline where retail places it last); the failure path as `return false;` 776 (the result's slot goes, frame
// 0x1f84); bool result with the retry search assigning `found` and one `if (!found)` failure block 746 (2974
// bytes, aligned 590), with retail's flag loop (short index, `flag` tested per obstacle under
// `step == count - 1`) alone 752, both 746 (2969 bytes, aligned 571; the failure block then sits last,
// identical to retail's); the copy loop through a local output count stored once after it 714 (2974 bytes,
// aligned 634), with the previous combination 749 (aligned 583; retail's copy-loop shape, other registers).
// Round 291: conditional/default-high actor-height producers, signed/unsigned flags and
// long/Boolean success, 727/760/727/760/729/750/729/750. Original complete body restored.
// Round 481: Independent obstacle/blocking/source transform-output lexical identities, all seven
// subsets 675 each; complete transform graph with travel-direction identity 675. Original body restored.
// Round 261: all six relative lexical schedules of direction/previous-direction/world-start
// 675/675/675/675/675/675; world/direction/previous with Boolean success and then signed-word frame
// 754/751. Original names, types and complete body restored; byte success and scalar/geometry homes remain.
// DH272: Actual parent-chain node cursor with frame/group/success long-long-long, long-long-bool, long-
// short-long, long-short-bool, short-long-long, short-long-bool, short-short-long, short-short-bool
// 707/761/682/748/707/768/699/755. All regress; original parent ordinal, signed native fields, result,
// copies and body restored. Byte success, radius/pathfinding and scalar/geometry homes remain.
// Round 300: Complete ten-byte obstacle/blocking CRT header resets and whole-step memmove
// subsets 1/2/4/3/5/6/7, then all with Boolean success:
// 749/726/745/768/731/739/743/762. Original complete body restored; success/radius/pathfinding and workspace homes remain.
// AC475: Native outgoing mask byte/signed-byte/word/dword with long success
// 774/774/675/675; with Boolean success 775/775/754/754. Original mask, result and full body restored.
// Round 200 (m3 lane 3, fast control 675): first_output zeroed at the top after flags 700, between actor
// and flags 701, with bool result 773; actor through actor_get with the typed field 675 (same code);
// radius ternary with early first_output 676, plus pathfinding read after the radius (retail's entry
// order) 672, plus bool result 745 (2968 bytes: retail's NONE in EBX and result byte). All restored.
// B384: Single actor-action predicate bool/byte/short/long, original/native shifted global
// bit consumer: 773/773/783/782/773/773/783/782. NOT kept: new predicate storage is established inside a test
// without an exact match. Original actor observations, bit consumer and full body restored.
// m3 lane 11 r63 (fast): actor pointer declared first 757 (2995 B); `*complete` read after the actor/height
// block 673, after the output clears 676, just before the step loop 704; that 673 form with bool result 755;
// `ignored` set after the out-count test 675; `radius < source.radius` 675. All restored.
// B388: Plain two-list transform table, long/short list ordinal, indexed/advancing
// obstacle receiver and long/Boolean success: 827/861/827/861/794/806/794/806.
// All regress and shorten away from retail; original distinct transform loops restored.
// Round 67 (m3 lane 10, fast; aligned difflib counts in brackets, 601 at 675): the enabled-bit test written as the
// matched 0x1b5aa0 writes it, `(bool)((g_557c40[9 >> 5] >> (9 & 31)) & 1)`, 702 [602] (retail's `shr eax, 9; test al,
// 1` shape, ours in EDX); that without the outer parentheses 702 [602]; with the mask test as `(g_4ee4ec & mask)` 702
// [602]; with a bool result 749 [610]; plus the radius ternary ahead of the pathfinding read 777 [607].
// P314: Cache-versus-stack workspace producers, independent conditional/branch/default
// obstacle/search assignments: 743/730/730/730/743/730/730/730. All regress; original
// pointer initialization, once-read cache selection and complete body restored.
// P314 further: Existing normalizer with ordered-positive trace admission, original/shared
// 2D difference: 712/714, frames 0x1f8c against 0x1f88, lengths 2986/2988 against 2986.
// NOT kept: both regress; unordered working-value changes require equality. All restored.
// DH352: Entry radius/height/outgoing-mask lexical identities, seven subsets 675 each;
// complete graph with Boolean success 754. Original entry scalars and full body restored;
// byte success, radius/pathfinding schedule and scalar/geometry homes remain.
// Round 129 (m3 lane 7, fast; aligned/masked control 415/270): entry grid of radius ternary, radius before pathfinding, (bool) enabled bit, final_complete after the actor block, first_output at the top,
// 31 combinations: 674 twice (radius before pathfinding with the ternary and complete moved; gain 1), the rest 675-703. Retail's failure block: retry assigns `found`, one `if (!found)` block, short flag
// loop testing `flag` per obstacle, `goto done` 741 (2968 B, masked 246), with `bool result` 746 (2969, masked 228), with `return false;` 793 (masked 234); on 746: radius ternary first 789 (2953), (bool) bit
// 775 (masked 224); without bool 764. Types seen in retail: `short ignored` 706 (2976), retry-loop index short 744 (2965), ignored-loop index short 675 (same code), `short frame` 704, ignored+frame 701,
// all three index/ignored 724; on `bool result` + failure block: all three 739 at retail's 2986 B (masked 223, the closest yet, but positional), with frame 749 (masked 221). Left there: copy loop and `truncated` in BL.
// B437: Stored passthrough-point producers with four count/receiver contracts
// 738/760/741/744; with bool success 766/783/773/755. NOT kept: all regress;
// byte success, radius/path acquisition and scalar/geometry homes remain.
// Round 148 (m3 lane 6, fast; aligned in brackets, control 675 [601]): short index and entry pointer in both transform loops
// (retail's movsx/lea/cmp si form) 697 [569]; bool result 754; radius ternary 708; first_output zeroed at entry 700;
// inserted count and truncation flag before the value28 test 710 (2976); start and start node if/else 750 (2960); retail's
// retry flow, two calls 779 (3012); all together 789 (2974) [623], without bool result 721 (2965) [596]. Greedy on aligned:
// transform loops plus early count and flag 681 [567]; with short node index 734 (2963) [543]; with the frame re-read 732 [632];
// positive obstacle-count arm first 715 [527]; all four 734 (2964) [631]. `inline` on 0x210690, 0x2104b0, both, or 0x2c4030 675.
// B437: Stored passthrough-point producers with four count/receiver contracts
// 738/760/741/744; with bool success 766/783/773/755. NOT kept: all regress;
// byte success, radius/path acquisition and scalar/geometry homes remain.
// P351: Complementary complete-step truth in bool/byte/short/long with long/Boolean result:
// 675/675/719/705/754/754/713/719. NOT kept: no byte match; radius/path acquisition, success byte and scalar/geometry homes remain.
// Original complete body and existing helpers restored.
// Round 74 (m3 lane 10, fast, caller 0x270930 MATCH in each): count read once into a named short 675; last step index in a long local 732; input pointer after start and start node 675; start node before the start
// point 707 (2976); `bool last` 736 (2959); `ignored` declared at the loop top 675; first_output declared in the for 675.
// Then 20 mechanical spellings through the same check (ternary and if, compare forms, operand order, constant declarations swapped): 675 eighteen times, 676, and `!(a <= b)` for a compare 698.
// w14F walk (r718), statement by statement against retail 0x2c41b0-0x2c4d57: no wrong field, offset, condition direction,
// callee or argument; every block (entry radius/height/actor, flags, step loop, transforms, search/retry/failure, node walk,
// copy-out, object-index pass, type chain 2/3/1/4/5/6/7) agrees in behaviour. What retail shows that this source does not:
// (1) after the radius normalisation retail stores 0.0f at [esp+0x58] (0x2c4ba2), i.e. the segment direction is a three-float
// local with k = 0; `vector3f delta; ... delta.k = 0.0f;` passed as `(point2f const *)&delta` is 676 (2988 B, frame 0x1f8c);
// (2) local types: ignored is a short (0x2c4a13 cmp word [esp+0x3c], -1; stored from the word at +0xdae), frame is a short
// (0x2c4405 word store; the callees take a short), node_index is a short (0x2c48ba test cx,cx), loop indices are shorts
// (0x2c4570 movsx/cmp si), result is a stack byte (0x2c41f1). Alone: ignored 703, frame 707, node_index 676, result 762,
// transform loops 712, retry loop 730. ignored+frame+node_index+(1) is 672 (2986 B, frame 0x1f8c) but the mnemonic
// alignment gets worse (diff 391 against 348), so nothing is kept;
// (3) callee types, not editable here: the out parameter `type` is a short* (retail writes words at 0x2c4c52..0x2c4cc0 and the
// caller 0x270b8f stores the low word of its slot), `short *type` with the casts gone is 675 here, identical code, but
// needs path.cpp's declaration and caller local changed; state->source.unknown04 reaches avoidance_search as a raw
// zero-extended byte (0x2c4741 xor ecx,ecx; mov cl,[eax+4]) where ours emits test/setne, so the field is a bool in retail
// (path.h declares byte); the (byte) cast on flags is codegen-neutral (dropped = 675).
// M3 lane 12 r108 (fast, caller 0x270930 MATCH throughout): `real radius = state->source.radius; if (radius < 0.2f) radius =
// 0.2f;` 754; every `goto done` as its own return (`return false;` on failure) 776 (frame changes: `result` folds away, retail
// keeps it as the stack byte [esp+0x13]); every `goto done` as a `break` out of the step loop 745 (2971 B); that with `bool
// result` 754 (2986 B); the copy-out loop on a `short output_count = *out_count;` local written back after the loop 714 (2974
// B, frame 0x1f8c: the local spills). Retail's three epilogues: the loop end and the truncation share `return result` (the
// byte reloaded), the `*out_count >= 4` test has its own `*complete = false;` epilogue, and the failure path returns 0 (`xor
// al, al`) after the flag loop; retail keeps the out count in AX through the copy-out loop and stores it once after it. Restored.
// MiniMax H (11-13): no new form, no lower score (675); the frame agrees at 0x1f8c, but retail keeps the state
// in EDX and the object index in ESI where ours keeps the count in EBP and the state in EDI, and the out
// parameter's short type is still blocked by the declaration and the caller in path.cpp.
// score 675 (was 675): retail's result is a stack byte, NONE sits in EBX and the step loop computes start and start_node
// before the input pointer; register homes and the transform loops' short indices differ.
// M3 lane 3 r225 (fast agrees with plain at 675; aligned [exact, stack-masked, register-masked] base [602, 524, 471]): bool result
// 754 [610, 531, 458]; with first_output declared at entry (retail zeroes [esp+0x68] there and stores the word count into it at
// each step's end) 773; bool result with start and start_node computed before the input pointer 724 [608, 513, 437]; all three 745;
// start and start_node before the input alone 675 (same score). The out parameter's short type stays BLOCKED by its declaration and
// caller in a file another lane holds this round.
// Lane 5 r166 (fast alone, control 675; CAVEAT on callee 0x2c0d60). Retail inlines obstacle_list_add (0x2c0d00) for the
// state-point add: it loads the object index and radius before the count test and stores x then y with no spill. The existing
// helper called there 712 (frame 0x1f8c: the helper volatile-reads radius and y to keep its own copy matched, so the inlined radius
// spills); BLOCKED-BY 0x2c0d00 for that form (src/unknown_2c0d00.cpp was another lane's file this round). The source
// record's byte at +4 (passed to both searches) declared bool in include/path.h 729 (2960 bytes); result as an unsigned byte 768 (2958); the radius clamp
// written from the source value (`radius = source; if (radius < 0.2f) radius = 0.2f;`) 754; ignored, frame and node_index
// as shorts together, without the delta change 701 (2976). All restored.
// m3 lane 14 r77 (fast, 0x270930 and 0x2c4030 matched each time; difflib counts with addresses masked / registers and
// offsets and XMM/integer registers masked too, control 418/275): retail passes the end-node local ([esp+0x18], the one carried to the next step) as
// both searches' goal node: previous_node there 735 (2960 B, masked 273). Retail's three sibling `input->type == NONE`
// tests (setup; search-or-copy; object-index pass, the node walk then reading input->point.output_index; MiniMax A's
// listed 770) re-measured 822 (2906 B, masked 237), with that goal 778 (2945 B, 216), with the copy block reading the step back from its output entry (retail's
// `lea edx, [eax + 0xc]` after the rep movsd) 813 (217), both 759 (2945 B, 213). On 759: the start/start_node if/else 769
// (206), short transform-loop indices 764 (203), short frame 752 (211), all three 782 (194). On that: retail's retry flow
// (a second search assigning `found` in each branch, one `if (!found)` failure block), `!avoid` first / `avoid` first
// 730/758 (2937 B, frame 0x1f8c: the byte `last` is normalised into a new slot), with retail's flag loop (short index,
// `flag` tested per obstacle under `step == count - 1`) 730/758 (masked 191/187); plus `bool last` 779/769 (2921 B,
// masked 186/183); plus bool result 741/745 (2938 B, 772 against 775 instructions, masked 178/175: retail's separate
// `xor al, al` and out-count epilogues appear). On 745: obstacle/search pointers set before `last` 754 (168), input after
// start/start_node 772 (162), both 778 (162); on 778: the radius ternary 809, pathfinding after the radius 780,
// first_output zeroed at entry 798, ternary with first_output 771 (plain 771, 2938 B, aligned 357, masked 155; the whole
// form is build/r77/2c41b0-r77-shape.patch in worktree m3-lane14). Left there: ours keeps count in BP and state in EDI
// through the prologue (retail: state EDX, object_index ESI, flag EDI, EBP scratch), and source.unknown04 is a byte here
// (test/setne at both searches) where retail zero-extends a raw byte; that field is assigned in held 0x261280 and lane
// 13's 0x260670, so it was not retyped. Nothing kept: every form scores above 675.
// MiniMax D (11-10): no new form, no lower score: the permuter ran past its time limit without a result.
// MiniMax A (11-12, run 62): no new form, no lower score (675); the prologue already differs at +0x14, where retail
// takes the state argument into EBX and indexes the g_4e0300 pool's data with it (twelve-byte entries) while ours
// takes the actor index into EAX and indexes g_4f55f0's data, and the count stays in EBP where retail keeps the
// state in EDX, so the whole body is laid out differently.
// @retail 0x2c41b0
bool __stdcall function_2c41b0(long actor_index, s_type_f17a25 *state, short count,
 s_path_step_view const *steps, bool avoid, short *out_count,
 s_path_step_view *out, bool *complete, long *object_index, long *type, bool *flag)
{
 long result = true;
 s_pathfinding_data const *pathfinding = (s_pathfinding_data const *)state->pathfinding;
 real radius = 0.2f;
 if (state->source.radius > radius) radius = state->source.radius;
 bool final_complete = *complete;
 long previous_node;
 point3f previous_point;
 byte *actor = NULL;
 long flags = 0;
 real maximum_height = 4.0f;
 if (actor_index != NONE)
 {
  actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
  if (*(long *)(actor + 0x26c) != NONE)
   maximum_height = 20.0f;
 }
 if (object_index) *object_index = NONE;
 byte *cache = *(byte **)((byte *)state + 0x50);
 if (cache && !cache[0x147b0]) *(short *)(cache + 0x147b2) = 0;
 if (flag) *flag = false;
 if (actor)
 {
  flags = 1;
  if (*(long *)(actor + 0x26c) == NONE)
  {
   flags = 3;
   if (*(short *)(actor + 0x86) >= 3 || avoid)
   {
    flags = 7;
    long mask = state->settings.flags;
    if ((mask & 8) && (state->settings.unknown04 & 3)) flags = 15;
    if ((mask & 0x10) && (state->settings.unknown04 & 0xe0)) flags |= 0x10;
    if ((mask & 0x20) && (state->settings.unknown04 & 0x1800)) flags |= 0x20;
    if (avoid && (g_46eeb8[9]->unknown8 != g_46f348 && (g_46eeb8[9]->mask & g_4ee4ec) == g_4ee4ec && ((g_557c40[0] >> 9) & 1) != 0)) flags |= 0x40;
   }
  }
 }
 short first_output = 0;
 for (short step = 0; step < count; ++step)
 {
  byte last = step == count - 1 && final_complete;
  s_obstacle_list local_obstacles;
  s_obstacle_list blocking;
  s_avoidance_search local_search;
  s_path_step_view inserted[64];
  s_obstacle_list *obstacles = &local_obstacles;
  s_avoidance_search *search = &local_search;
  long ignored = NONE;
  if (*out_count >= 4)
  {
   *complete = false;
   goto done;
  }
  s_path_step_view const *input = &steps[step];
  point3f const *start = step > 0 ? &previous_point : &state->source.point.point;
  long start_node = step > 0 ? previous_node : state->source.unknown24;
  previous_node = input->node_index;
  if (input->type == NONE)
  {
   long frame = input->point.output_index;
   cache = *(byte **)((byte *)state + 0x50);
   if (cache)
   {
    obstacles = (s_obstacle_list *)(cache + 0x147b4 + step * 0x50c);
    search = (s_avoidance_search *)(cache + 0x15be4 + step * 0xdb0);
   }
   vector3f direction;
   direction.i = input->point.point.x - start->x;
   direction.j = input->point.point.y - start->y;
   direction.k = input->point.point.z - start->z;
   function_30bf0(&direction);
   vector3f previous_direction;
   if (actor) previous_direction = *(vector3f *)(actor + 0x290);
   point3f world_start;
   if (!function_2104b0(frame, start, &world_start)) world_start = *start;
   obstacles->group_count = 0;
   obstacles->count = 0;
   obstacles->flag0_count = 0;
   obstacles->flag3_count = 0;
   *(short *)obstacles->unknown08 = 0;
   blocking.group_count = 0;
   blocking.count = 0;
   blocking.flag0_count = 0;
   blocking.flag3_count = 0;
   *(short *)blocking.unknown08 = 0;
   function_2c0d60(actor ? &previous_direction : NULL, actor_index, (byte)flags, &state->settings,
    obstacles, &blocking, &world_start, maximum_height, &direction, state->source.object_index, state->source.unknown0c);
   for (long i = 0; i < obstacles->count; ++i)
   {
    point3f transformed;
    transformed.x = obstacles->obstacles[i].center.x;
    transformed.y = obstacles->obstacles[i].center.y;
    transformed.z = 0.0f;
    function_210690(frame, &transformed, &transformed);
    obstacles->obstacles[i].center.x = transformed.x;
    obstacles->obstacles[i].center.y = transformed.y;
   }
   for (long i = 0; i < blocking.count; ++i)
   {
    point3f transformed;
    transformed.x = blocking.obstacles[i].center.x;
    transformed.y = blocking.obstacles[i].center.y;
    transformed.z = 0.0f;
    function_210690(frame, &transformed, &transformed);
    blocking.obstacles[i].center.x = transformed.x;
    blocking.obstacles[i].center.y = transformed.y;
   }
   if (state->source.unknown28[0] && state->source.unknown44)
   {
    point3f transformed;
    function_210690(frame, (point3f const *)((byte *)state + 0x2c), &transformed);
    if (obstacles->count != 64)
    {
     s_obstacle *entry = &obstacles->obstacles[obstacles->count++];
     ++obstacles->flag0_count;
     entry->flags = 1; entry->group = NONE;
     entry->object_index = *(long *)((byte *)state + 0x38);
     entry->center.x = transformed.x; entry->center.y = transformed.y;
     entry->radius = *(real *)((byte *)state + 0x3c);
    }
   }
   obstacle_list_group(obstacles, radius);
   cache = *(byte **)((byte *)state + 0x50);
   if (cache && !cache[0x147b0]) ++*(short *)(cache + 0x147b2);
   if (!(obstacles->count > 0))
   {
    out[(*out_count)++] = *input;
    previous_point = input->point.point;
   }
   else
   {
    bool found = avoidance_search(pathfinding, search, (s_avoidance_limits const *)state, state->source.unknown04,
     obstacles, radius, (point2f const *)start, start_node, (point2f const *)&input->point.point,
     input->node_index, last, false);
    if (!found)
    {
     bool retry = obstacles->flag0_count > 0;
     if (avoid)
     {
      retry = retry || (obstacles->flag3_count > 0 && search->result != NONE);
      if (retry)
      {
       for (long i = 0; i < obstacles->count; ++i)
       {
        s_obstacle *entry = &obstacles->obstacles[i];
        if (entry->group == search->result && (entry->flags & 8)) entry->flags |= 1;
        else entry->flags &= ~1;
       }
       ignored = search->result;
       if (step > 0)
       {
        start = &steps[step - 1].point.point;
        start_node = steps[step - 1].node_index;
       }
      }
     }
     if (!retry || !avoidance_search(pathfinding, search, (s_avoidance_limits const *)state, state->source.unknown04,
      obstacles, radius, (point2f const *)start, start_node, (point2f const *)&input->point.point,
      input->node_index, last, true))
     {
      if (step == count - 1 && flag)
       for (long i = 0; i < obstacles->count; ++i) if (obstacles->obstacles[i].flags & 2) *flag = true;
      result = false; goto done;
     }
    }
    if (search->value28)
     previous_point = input->point.point;
    else
    {
     s_avoidance_node *node = &search->nodes[search->value1e];
     previous_point.x = node->position.x; previous_point.y = node->position.y;
     previous_point.z = input->point.point.z;
     previous_node = node->value08;
    }
    short inserted_count = 0;
    bool truncated = false;
    long node_index = search->value1e;
    while (node_index != 0)
    {
     s_avoidance_node *node = &search->nodes[node_index];
     s_path_step_view *entry = &inserted[inserted_count++];
     entry->node_index = node->value08;
     entry->point.point.x = node->position.x; entry->point.point.y = node->position.y;
     entry->point.point.z = input->point.point.z;
     entry->type = NONE; entry->link_index = NONE; entry->unknown02 = 0;
     entry->point.output_index = frame;
     node_index = node->parent;
     if (inserted_count >= 64) { truncated = true; break; }
    }
    for (short i = inserted_count - 1; i >= 0; --i)
    {
     if (*out_count >= 4) { truncated = true; break; }
     out[(*out_count)++] = inserted[i];
    }
    if (truncated) { *complete = false; goto done; }
   }
   if (object_index && *object_index == NONE && (blocking.count > 0 || ignored != NONE))
   {
    if (ignored != NONE)
    {
     for (long i = 0; i < obstacles->count; ++i)
     {
      s_obstacle *entry = &obstacles->obstacles[i];
      if (entry->group != ignored) continue;
      obstacle_list_add(&blocking, entry->flags, &entry->center, entry->object_index, entry->radius);
     }
    }
    for (short i = first_output; i < *out_count; ++i)
    {
     if (out[i].type != NONE) continue;
     point2f const *segment_start = i == 0 ? (point2f const *)&state->source.point.point : (point2f const *)&out[i - 1].point.point;
     long sector = i == 0 ? state->source.unknown24 : out[i - 1].node_index;
     point2f delta;
     delta.x = out[i].point.point.x - segment_start->x;
     delta.y = out[i].point.point.y - segment_start->y;
     real distance = sqrt(delta.x * delta.x + delta.y * delta.y);
     if (fabs(distance) >= 0.0001f)
     { real inverse = 1.0f / distance; delta.x *= inverse; delta.y *= inverse; }
     else distance = 0.0f;
     if (last && i == *out_count - 1) distance -= 0.25f;
     if (distance <= 0.0f) continue;
     s_avoidance_trace trace;
     function_26ccb0(pathfinding, &blocking, NONE, segment_start, sector, NONE, &delta, radius,
      distance, true, false, false, NULL, &trace);
     if (trace.obstacle < 0 || trace.obstacle >= blocking.count) continue;
     s_obstacle *entry = &blocking.obstacles[trace.obstacle];
     *object_index = entry->object_index;
     if (entry->flags & 0x10) *(short *)type = 2;
     else if (entry->flags & 0x40) *(short *)type = 3;
     else if (entry->flags & 0x20) *(short *)type = 1;
     else if (entry->flags & 0x100) *(short *)type = 4;
     else if (entry->flags & 0x80) *(short *)type = 5;
     else if (entry->flags & 8) *(short *)type = 6;
     else if (entry->flags & 0x200) *(short *)type = 7;
     break;
    }
   }
  }
  else
  {
   out[(*out_count)++] = *input;
   previous_point = input->point.point;
  }
  first_output = *out_count;
 }
done:
 return result;
}

