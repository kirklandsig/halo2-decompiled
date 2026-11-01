/* UNKNOWN_1FA590.H: the pathfinding data of the structure bsp (g_4e0348
   +0xc4) and the lookups of src/unknown_1fa590.cpp on it */

#ifndef UNKNOWN_1FA590_H
#define UNKNOWN_1FA590_H

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

/* a node: flags in the first word, then the first of its surfaces (8 bytes) */
struct s_pathfinding_node
{
	word flags;
	short first_surface;
	union
	{
		byte unknown4[4];
		long first_edge;
	};
};

/* a surface of a node, chained by next (0x14 bytes) */
struct s_pathfinding_surface
{
	short type;
	short next;
	long index;
	long object_index;
	byte unknown0c[0x14 - 0xc];
};

/* an edge of the pathfinding data (16 bytes, from +0xc): nodes are the
   nodes on its two sides */
struct s_pathfinding_edge
{
	word vertices[2];
	byte unknown04[4];
	word next_edges[2];
	union
	{
		struct
		{
			word unknown0c;
			word surface;
		};
		word nodes[2];
	};
};

struct s_pathfinding_data
{
	long node_count;
	s_pathfinding_node *nodes;
	byte unknown08[4];
	s_pathfinding_edge *edges;
	byte unknown10[0x2c - 0x10];
	point3f *vertices;
	byte unknown30[0x38 - 0x30];
	long surface_count;
	s_pathfinding_surface *surfaces;
};

struct s_pathfinding_edges_view
{
	byte unknown00[0xc];
	s_pathfinding_edge *edges;
	byte unknown10[0x3c - 0x10];
	s_pathfinding_surface *surfaces;
};

/* walks the edges around a vertex: first those of the surfaces chained from
   surface_index, then around the ring of next_edges */
struct s_edge_iterator
{
	word edge_index;
	word first_edge_index;
	word next_edge_index;
	byte unknown06[2];
	long surface;
	long vertex;
	long previous_surface_index;
	long surface_index;
	bool forward;
	s_pathfinding_edges_view const *pathfinding;
	short count;
};

s_pathfinding_edge *function_1fa720(s_edge_iterator *iterator);
bool function_1fa6b0(s_pathfinding_node const *node, s_pathfinding_data const *pathfinding);

#endif
