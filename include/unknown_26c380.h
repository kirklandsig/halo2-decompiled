/* UNKNOWN_26C380.H: traces across the sectors of the pathfinding data
   (src/unknown_26c380.cpp) */

#ifndef UNKNOWN_26C380_H
#define UNKNOWN_26C380_H

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "unknown_1fa590.h"
#include "path.h"

/* where a sector trace stops (0x24 bytes) */
struct s_path_trace_result
{
	long unknown00;
	point3f point;
	byte unknown10[8];
	real distance;
	byte unknown1c[8];
};

/* the same, with its fields named */
struct s_sector_trace_result
{
	bool blocked;
	byte unknown01[3];
	point3f point;
	long sector_index;
	long edge_index;
	real distance;
	bool unknown1c;
	byte unknown1d[3];
	real unknown20;
};

/* traces along direction from origin, from sector_index across the sectors'
   edges up to distance, or until it reaches target_sector_index */
bool function_26c590(s_pathfinding_data const *pathfinding, point3f const *origin, long sector_index,
	long target_sector_index, vector3f const *direction, real distance, s_path_location const *location,
	s_path_trace_result *trace);

/* the order the sources written before 0x26c590 call it in (its node index
   first, an unused point, the result, then the rest); they can call the
   function above directly instead and drop this */
inline bool function_26c590(long node_index, point3f const *unused, s_path_trace_result *result,
	s_pathfinding_data *pathfinding, point3f const *position, long target_node_index,
	vector3f const *direction, real distance, long location)
{
	return function_26c590(pathfinding, position, node_index, target_node_index, direction, distance,
		(s_path_location const *)location, result);
}

struct s_type_c3b527;

/* traces from start (in sector_index) toward end (in target_sector_index) */
bool function_26c4e0(s_pathfinding_data *pathfinding, s_type_c3b527 const *start, long sector_index,
	s_type_c3b527 const *end, long target_sector_index, long location, s_path_trace_result *trace);

#endif
