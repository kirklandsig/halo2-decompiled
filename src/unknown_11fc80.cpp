// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_11FC80.CPP: polygon clipping and vector helpers (the worker job
   queue that followed is in async.cpp, the profile setters in
   unknown_120d80.cpp) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "vector_products.h"
#include <math.h>
#include <string.h>
#include <xtl.h>

#define k_real_epsilon 0.0001f

real function_30bf0(vector3f *v);

/* ---- globals ---- */

extern short const g_440b94[6][3] =
{
	{ 2, 1, 0 },
	{ 1, 2, 0 },
	{ 0, 2, 1 },
	{ 2, 0, 1 },
	{ 1, 0, 2 },
	{ 0, 1, 2 },
};

PRIVATE __forceinline void function_11fc81(point3f *arg_0, point3f const *arg_1, vector3f const *arg_2, real arg_3)
{
	real local_0 = arg_2->i * arg_3;
	local_0 += arg_1->x;
	arg_0->x = local_0;
	local_0 = arg_3 * arg_2->j;
	local_0 += arg_1->y;
	arg_0->y = local_0;
	arg_3 *= arg_2->k;
	arg_0->z = arg_3 + arg_1->z;
}

/* where the edge from p0 to p1 crosses the plane, from the two points' distances to it */
PRIVATE inline void plane_edge_crossing(point3f const *p0, point3f const *p1, real d0, real d1, point3f *result)
{
	vector3f delta;
	vector3d_from_points3d(p0, p1, &delta);
	real t = d0 / (d0 - d1);
	function_11fc81(result, p0, &delta, t);
}

/* clips a polygon against a plane: keeps the points on the kept side (offset by the
   tolerance) and adds a point where each edge crosses; returns the count written */
// @retail 0x11fc80
long function_11fc80(
	plane3f const *plane,
	bool keep_inside,
	real tolerance,
	long point_count,
	point3f const *points,
	long maximum_count,
	point3f *out)
{
	long count = 0;

	if (point_count > 0)
	{
		real offset = keep_inside ? tolerance : 0.f - tolerance;
		point3f const *previous = &points[point_count - 1];
		real previous_distance = plane_distance_to_point(plane, previous);
		previous_distance -= offset;
		bool previous_inside = previous_distance >= 0.f;

		for (long i = 0; i < point_count; i++)
		{
			point3f const *current = &points[i];
			real distance = plane_distance_to_point(plane, current) - offset;
			bool inside = distance >= 0.f;

			if (inside != previous_inside)
			{
				if (count >= maximum_count)
					break;
				plane_edge_crossing(previous, current, previous_distance, distance, &out[count]);
				count++;
			}
			if (inside == keep_inside)
			{
				if (count >= maximum_count)
					break;
				out[count] = *current;
				count++;
			}
			previous = current;
			previous_distance = distance;
			previous_inside = inside;
		}
	}
	return count;
}

static inline real distance_squared(point3f const *a, point3f const *b)
{
	real dx = a->x - b->x;
	real dy = a->y - b->y;
	real dz = a->z - b->z;

	return dz * dz + dy * dy + dx * dx;
}

/* two-bone inverse kinematics: moves the middle node so that the chain from
   the root reaches the target (clamped to the chain's reach), rebuilds the
   root and middle axes and copies the target into the end node */
// @retail 0x120220
void function_120220(
	transform4x3f *target,
	transform4x3f *end,
	transform4x3f *root,
	transform4x3f *mid)
{
	real mid_root_squared = distance_squared(&mid->position, &root->position);
	real end_mid_squared = distance_squared(&end->position, &mid->position);
	real root_target_squared = distance_squared(&root->position, &target->position);

	if (mid_root_squared > k_real_epsilon && end_mid_squared > k_real_epsilon && root_target_squared > k_real_epsilon)
	{
		real mid_root = (real)sqrt(mid_root_squared);
		real end_mid = (real)sqrt(end_mid_squared);
		real root_target = (real)sqrt(root_target_squared);

		vector3f to_mid;
		to_mid.i = mid->position.x - root->position.x;
		to_mid.j = mid->position.y - root->position.y;
		to_mid.k = mid->position.z - root->position.z;

		real inverse = 1.f / root_target;
		vector3f u;
		u.i = (target->position.x - root->position.x) * inverse;
		u.j = (target->position.y - root->position.y) * inverse;
		u.k = (target->position.z - root->position.z) * inverse;

		vector3f normal;
		cross3f_staged(&u, &to_mid, &normal);
		function_30bf0(&normal);

		vector3f e;
		cross3f_staged(&normal, &u, &e);

		real limit = (end_mid + mid_root) * 0.98f;
		if (root_target > limit)
		{
			target->position.x = limit * u.i + root->position.x;
			target->position.y = limit * u.j + root->position.y;
			target->position.z = limit * u.k + root->position.z;
			root_target = limit;
		}

		real along = (root_target * root_target + mid_root * mid_root - end_mid * end_mid) / (root_target * 2.f);
		real remaining = root_target - along;
		real height_squared = mid_root * mid_root - along * along;
		real height;
		if (height_squared >= k_real_epsilon)
			height = (real)sqrt(height_squared);
		else
			height = 0.f;

		vector3f *mid_left = &mid->left;
		vector3f *root_forward = &root->forward;
		vector3f *root_left = &root->left;
		vector3f *root_up = &root->up;

		root_forward->i = along * u.i + e.i * height;
		root_forward->j = along * u.j + e.j * height;
		root_forward->k = along * u.k + e.k * height;
		function_30bf0(root_forward);

		cross3f_staged(root_forward, root_left, &root->up);
		function_30bf0(root_up);

		cross3f_staged(root_up, root_forward, root_left);

		point3f new_mid;
		new_mid.x = root->position.x;
		new_mid.x += root_forward->i * mid_root;
		new_mid.y = root->position.y;
		new_mid.y += root_forward->j * mid_root;
		new_mid.z = root->position.z;
		new_mid.z += root_forward->k * mid_root;

		vector3f *mid_up = &mid->up;
		vector3f *mid_forward = &mid->forward;

		mid_forward->i = remaining * u.i - e.i * height;
		mid_forward->j = remaining * u.j - e.j * height;
		mid_forward->k = remaining * u.k - e.k * height;
		function_30bf0(mid_forward);

		cross3f_staged(mid_forward, mid_left, &mid->up);
		function_30bf0(mid_up);

		cross3f_staged(mid_up, mid_forward, mid_left);

		mid->position = new_mid;
		*end = *target;
	}
}

// @retail 0x1201a0
real function_1201a0(vector3f *v, vector3f const *fallback)
{
	real m = (real)sqrt(v->i * v->i + v->j * v->j + v->k * v->k);
	if (!(fabs(m) < k_real_epsilon))
	{
		real inv = 1.f / m;
		v->i = v->i * inv;
		v->j = inv * v->j;
		v->k = inv * v->k;
		return m;
	}
	*v = *fallback;
	return 0.f;
}

struct s_120791
{
    real field_0;
};

static __forceinline s_120791 function_120791(real const *arg_0, real const *arg_1, short arg_2, short arg_3, short arg_4)
{
    s_120791 local_0;
    local_0.field_0 = arg_0[3] - arg_0[arg_2] * arg_1[0] - arg_0[arg_3] * arg_1[1];
    local_0.field_0 /= arg_0[arg_4];
    return local_0;
}

// @retail 0x120790
real *function_120790(real *out, real const *plane, real const *point, short axis, byte side)
{
	long local_0 = side + axis * 2;
	short a = g_440b94[local_0][0];
	short b = g_440b94[local_0][1];

	out[a] = point[0];
	out[b] = point[1];
	if (fabs(plane[axis]) < k_real_epsilon)
		out[axis] = 0.f;
	else
		out[axis] = function_120791(plane, point, a, b, axis).field_0;
	return out;
}

// @retail 0x120810
real *function_120810(real const *point, short axis, byte side, real *out)
{
	long index = side + axis * 2;
	real x = point[g_440b94[index][0]];
	real y = point[g_440b94[index][1]];

	out[0] = x;
	out[1] = y;
	return out;
}
