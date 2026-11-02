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
// score 2 (was 2): one product's operand order remains: in the second root cross
// product (up x forward into left) retail loads up->k before forward->j.
// Round 134: a shared inline axis rebuild (normalize, two crosses) for root and mid with
// pointer parameters 2, taking the node instead 107; new_mid through root->forward 4;
// mid_left among the mid pointers, all 36 orders, best 4 (root_left first fixes this
// product but flips two in the first cross); root pointers in a block 2; names don't matter.
// Tried: lift as a vector (232) or three scalars (216); the direction kept in
// a delta vector (215); u = inverse * delta, u in k-i-j order, vector3f_scale
// (215-223); delta scaled in place (308); 1 / root_target written inline
// (253); a separate distance local (193); the cross product written to out
// directly, cross3f (174), with its components in i-j-k, j-i-k or j-k-i order
// (50; k-j-i, k-i-j and i-k-j give 26), or with each product's operands
// swapped (26); new_mid.x as root_forward->i * mid_root (26); the node axes
// written as root->forward etc. without the pointer locals (59); six product
// values 28; cross accumulators 34; first/second/both input copies 512/565/741;
// returned cross value 462; cross reference parameters 26; local interpolation
// helper 28. Round 69: k-i-j components with root_left declared after
// root_forward and mid_up before mid_forward 2; then height as a ternary 2;
// i-k-j components 2; root_up before root_left 2; new_mid declared at the top 2;
// new_mid.x as root_forward->i * mid_root 2; any one node pointer local dropped
// 18-39; new_mid computed earlier 31-52; other component store orders 8-29.
// Helper parameter b-a-out/a-out-b/b-out-a/out-b-a 2 each; cross component names,
// input/output role names, returned output pointer and all axis-pointer role names 2 each.
// Twice-used first-input scalar i/j/k 138/133/133; second-input i/j/k
// 124/121/136; complete first/second scalar inputs 240/273.
// Round 71: cross helper taking (out, a, b) 2; its locals declared up front 2;
// the other 23 orders of the four node pointer declarations 7-24.
// Round 7: shared cross at second root/middle-up/middle-left 62/63/67;
// helper stores j-k-i 90; rotation output/up/forward 16/20/2; all three 18;
// ordinary inline 2; all three with ordinary or forced inline 18 each.
// Round 14 (exact one-file harness): the existing cross3f_staged for the file's own cross
// copy 2 (kept); cross3f 174; to_mid through vector3d_from_points3d 183; limit sum
// order 2; clamp as position + u * limit 2; new_mid through vector3f_add_scaled 10;
// distance_squared arguments swapped 11/8/12; copies swapped 144; height default 228;
// root_left or root_up declared after the forward normalize 10/31; mid_left among the
// mid pointers 18; &root->left, &root->up or all three in the second cross 16/20/18.
// Round 38 (fast): the four root/mid axis pointers at the top of the function 297, the root ones
// only 302, the mid ones only 280; at the top of the inner block 279, the root ones only 292. In
// the second cross retail loads the higher-offset component first in all six products.
// Round 37 (fast): the first root cross writing &root->up 14 (the root part then matches, the
// mid crosses flip); also its normalize through &root->up 14; the normalize alone 2; second cross
// with &root->forward 2; first cross inputs direct 18; the same three forms for mid 14 each; no
// root/mid/any pointer locals 41/51/59; new_mid through the file's point-plus-scaled-vector
// helper 10, with the &root->up first cross 8 (root part and new_mid exact, 8 in the mid
// crosses); &root->up first cross with new_mid.z (or .y) read as root->forward.k 1 (mid crosses
// fixed, that read's base register differs). Compile-only harness (exact here): every 1-3 of the
// 25 pointer-local uses written direct, best 1 (above); from the helper form best 4; mid_forward or
// mid_up declared earlier 21-30; mid_left before each earlier statement 8-56; a shared axis
// rebuild helper for root and mid 2 (with the point helper 4); unused-local probes (diagnostic
// only) show the helper form needs mid_forward's numbering moved by 3-5 (mod 8).
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

		cross3f_staged(root_forward, root_left, root_up);
		function_30bf0(root_up);

		cross3f_staged(root_up, root_forward, root_left);

		point3f new_mid;
		new_mid.x = mid_root * root_forward->i + root->position.x;
		new_mid.y = root_forward->j * mid_root + root->position.y;
		new_mid.z = root_forward->k * mid_root + root->position.z;

		vector3f *mid_up = &mid->up;
		vector3f *mid_forward = &mid->forward;

		mid_forward->i = remaining * u.i - e.i * height;
		mid_forward->j = remaining * u.j - e.j * height;
		mid_forward->k = remaining * u.k - e.k * height;
		function_30bf0(mid_forward);

		cross3f_staged(mid_forward, mid_left, mid_up);
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
