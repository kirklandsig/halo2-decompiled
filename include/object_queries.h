/* OBJECT_QUERIES.H: object queries decompiled by lane F (unknown_0b8bd0.cpp) */
#ifndef OBJECT_QUERIES_H
#define OBJECT_QUERIES_H

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

/* where an object is: a leaf and a cluster of the structure bsp */
struct s_location
{
	long leaf_index;
	short cluster_index;
	short bsp_index;
};

transform4x3f *function_b8bd0(long object_index, short node_index);
bool object_or_parent_hidden(long object_index);
s_location *object_get_root_location(long object_index, s_location *location);
void function_ba1d0(long object_index, vector3f *linear_velocity, vector3f *angular_velocity);

#endif
