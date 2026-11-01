/* UNKNOWN_2C3550.CPP: the search for a way around the obstacles in a path's
   way: its nodes, kept in a binary heap ordered by cost */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "unknown_2c0d00.h"
#include "unknown_26c380.h"
#include "globals.h"
#include <math.h>
#include <string.h>

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
		return distance;
	}
	return 0.0f;
}

/* adds a node at the point, passing the obstacle on the side, unless it
   leads back around an obstacle on its way; returns its index or NONE */
// score 92 (was 92): frame 0xc agrees; missing early Y load rotates normalized components and distance registers.
// Earlier: the scaled components are kept in different registers and the length is
// squared and summed in a different order. Retail loads y into XMM0 before the square root and
// keeps `y * inverse` and `other.y * y` in source operand order; ours copies the register
// operand and multiplies by memory. The compile-only build gives ours for every helper form.
// Products x*inv/inv*y 92; compound scaling 92; tiny case first with one return 92; scalar
// direction locals 165; y scaled first 93; helper reads both components first 156; dot test
// through the node array 153; zero on the left of the dot test 92.
// Normalized point returned with pointer/reference length outputs 280/280; whole input point
// by value with pointer/reference outputs 216/216; scalar inputs with pointer/reference
// outputs 156/156; distinct const-input/output pointers 156; const-reference input with
// returned point 280. Original in-place normalizer restored.
// Round 14: retail's early `movss xmm0, [y]` is the subtraction's y forwarded to both
// normalize paths (y * inverse in place, and direction_y on the short path); x is re-read.
// The existing normalize2d_fast through a cast 92 (arms swap); a vector2f direction with it
// 177; y read into a local first in the normalizer 92 (same code); no direction locals 92;
// x local first 92; cross term `direction_x * other.y` 92; dot terms local-first 92.
// Round 191: shared subtraction with original/shared/captured/native-real-root normalizer
// 91/92/155/155; complete native point with the same boundaries 92/92/156/156. One-point
// gain rejected; original point, private normalizer and body restored. Early Y preload remains.
// Lane 17 r1: normalizer through the file's scale helper with a zero-length else (0x294050's form,
// which gets the preload there) 93; direction from vector2d_from_points2d 91 (x scaled and stored
// first, as retail, but the squares start with x); both 93; with the sum written y first 91 (same
// code); with the file's length helper too 155.
// Round 232: Once-read node/goal/node-goal/heap/node-heap/goal-heap/all search scalars, then all with full-width captured counts
// 170, 175, 175, 92, 170, 175, 175, 167. Original executable source restored.
// Round 48: normalizer with one exit (distance zeroed in the tiny case) 92; with a result variable
// 101; direction as an aggregate initializer 92; goal through a pointer 92; distance declared at
// its assignment 92. A whole-file /Op build (diagnostic) makes the root a library call (far worse).
// Round 143 (lane 4, fast with the file, faithful 92): the node's direction copied field by field from the
// struct 177 (608 bytes: the direction lives in registers, no store-back, and still no early Y load), from
// the direction locals 177, with no direction locals 177; the direction from vector2d_from_points2d 91 (as
// lane 17 r1: Y is then popped and the squares start with X), that with no direction locals 91. Matched
// 0x294050 gets the early loads for every component but the first (j, k) under the same x87 sum.
// Compile-only screen (/O2 /arch:SSE, reproduces our code): the early Y load (and retail's XMM4 zero)
// appears only when the direction comes from vector2d_from_points2d and the walk reads it only through
// the existing dot_product2d and cross_product2d (the dot and the second cross factor, any normalizer
// form); it is then a hoisted load, so Y lands in XMM1 and the second factor loads other.x instead of
// multiplying Y in place as retail does (real 106, 634 bytes). Direct reads, locals, scalar locals or a
// helper copying into locals never give it. A hoisted load is never multiplied in place by a memory
// operand (a Y-first cross helper, the sibling/cross check moved after the walk loop, or no node copy:
// still `movss xmm5, [other.x]; mulss xmm5, y`), and the check after the loop adds an unthreaded
// `index != NONE` test; retail's in-place `mulss xmm0, [edx+0xc]` needs Y as a variable.
// Round 122 (lane 6, fast): the dot and both crosses through the existing dot_product2d/cross_product2d (vector2f
// casts, direction locals dropped) 152 (634 bytes); with the subtraction through vector2d_from_points2d and the
// normalizer's single exit (distance zeroed in an else) 106 (634: retail's early `movss xmm0, [y]` preload, its
// distance/zero registers, dot, first cross and loop entry, but y and the inverse swap XMM0/XMM1 and the second
// cross loads other.x where retail multiplies y in place); with the first cross by name 107; with the second
// cross by name 149 (630 bytes, the preload is lost); with the normalizer's distance zeroed in the then-arm 114.
// Compile-only screen: the preload appears only when y is read through the helpers' pointers after the
// subtraction helper; product orders, store orders, sum order and the helpers' argument orders change nothing.
// Round 258: Both captured normalized components used for native direction publication, y/x or
// x/y capture order and publication after value08/position/distance/parent: 177/177/177/177/177/177/177/177.
// Original native copy, captures and body restored; early Y preload and XMM allocation remain.
// Round 27 (m3 lane 2, fast): the node's direction filled by components 177 (608 bytes: direction kept in
// XMM0/XMM1 to the end), also without the direction locals 177; the direction locals declared first in the
// function and assigned after the normalize 92 (same code); the cross factors named 92 (same code); direction
// from vector2d_from_points2d with the locals x first 92. Compile-only (reproduces ours): no direction locals,
// locals declared at the block top, dot operands swapped, a named dot: none preloads y.
// Round 150 (compile-only screen is exact for this block): the y preload needs every reference to direction.y
// to go through a pointer (a pointer local to the whole direction for the subtraction, the normalizer and the
// direction locals gives it; not allowed); x is never preloaded either way. Real: vector2d_from_points2d +
// dot_product2d/cross_product2d + single-exit normalizer 106 (634 bytes; y in XMM1, inverse in XMM0, the second
// cross not in place), with the two-return normalizer 144; single-exit normalizer alone 92;
// vector2d_from_points2d + single exit 91 (622); that with the dot through dot_product2d 146 (630 bytes, loop
// entry layout as retail, no preload). Any direct read of direction.y (the cross written on the fields) drops it.
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

		direction.x = search->goal.x - point->x;
		direction.y = search->goal.y - point->y;
		distance = avoidance_normalize2d(&direction);
		real direction_y = direction.y;
		real direction_x = direction.x;
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

						if (other->direction.y * direction_y + other->direction.x * direction_x > 0.0f)
						{
							s_avoidance_node *from = &search->nodes[parent];

							if (0.0f > (from->direction.y * other->direction.x - other->direction.y * from->direction.x) *
								(direction_y * other->direction.x - other->direction.y * direction_x))
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

// score 675 (was 675): the 675 differing lines are 541 real ones (the rest are the unmatched
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

