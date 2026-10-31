/* VEHICLE_RAY.H: the ray record the vehicle and placement tests pass to the
   point and ray collision queries */

#ifndef VEHICLE_RAY_H
#define VEHICLE_RAY_H

#include "unknown_0259d0.h"

/* a ray: a point, a vector, the fraction along it something was hit and the
   normal there */
struct s_vehicle_ray
{
	point3f point;
	vector3f vector;
	real t;
	vector3f normal;
};
typedef char assert_s_vehicle_ray_size[(sizeof(s_vehicle_ray) == 0x28) ? 1 : -1];

#endif
