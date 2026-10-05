// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_11cb00.h"

long function_11f9a0(box3f const *rectangle,
	long maximum_vertex_count, point3f vertices[]);

typedef point3f rectangle3d_edge[2];

// @retail 0x11f770
box3f *function_11f770(box3f *rectangle,
	long point_count, point3f const points[])
{
	for (long i = 0; i < point_count; i++)
	{
		if (rectangle->x0 > points[i].x)
			rectangle->x0 = points[i].x;
		if (points[i].x > rectangle->x1)
			rectangle->x1 = points[i].x;
		if (rectangle->y0 > points[i].y)
			rectangle->y0 = points[i].y;
		if (points[i].y > rectangle->y1)
			rectangle->y1 = points[i].y;
		if (rectangle->z0 > points[i].z)
			rectangle->z0 = points[i].z;
		if (points[i].z > rectangle->z1)
			rectangle->z1 = points[i].z;
	}
	return rectangle;
}

// @retail 0x11fa40
long function_11fa40(box3f const *rectangle,
	long maximum_edge_count, rectangle3d_edge edges[])
{
	long edge_vertices[12][2] =
	{
		{ 0, 2 }, { 2, 3 }, { 3, 1 }, { 1, 0 },
		{ 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 },
		{ 4, 5 }, { 5, 7 }, { 7, 6 }, { 6, 4 }
	};
	point3f vertices[8];
	function_11f9a0(rectangle, 8, vertices);
	/* Retail always emits twelve edges; the capacity argument is unused. */
	for (long i = 0; i < 12; i++)
	{
		edges[i][0] = vertices[edge_vertices[i][0]];
		edges[i][1] = vertices[edge_vertices[i][1]];
	}
	return 12;
}
