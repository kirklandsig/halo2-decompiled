/* UNKNOWN_2BD960.H: the hill of the engine state at g_51ecc8, which
   unknown_2bd960.cpp defines and unknown_2bd0b0.cpp also sets. */

#ifndef UNKNOWN_2BD960_H
#define UNKNOWN_2BD960_H

#include "unknown_0259d0.h"

/* the hill: a polygon of 32 vertices on a closed spline through up to 16
   markers, inside a circle and a height range */
struct s_polygon_2be
{
	/* sets up the hill of the index (0x2bdc70, unknown_2bd960.cpp) */
	void hill_set(long index);

	/* the markers (the spline's points) */
	long ids[16];
	/* the vertices each spline segment gets */
	long steps[8];
	long count;
	point2f vertices[32];
	real center_x;
	real center_y;
	real center_z;
	real radius;
	real z_min;
	real z_max;
	real perimeter;
};

#endif
