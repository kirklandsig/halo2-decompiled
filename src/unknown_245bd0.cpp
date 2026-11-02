// @flags /O2 /Gr /arch:SSE
/* UNKNOWN_245BD0.CPP: collision tests of points and rays against capsules, spheres
   and flat prisms, plus a few small object accessors */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "matrix_transform.h"
#include "data_array.h"
#include "vector_products.h"
#include "globals.h"
#include <float.h>
#include <math.h>

#define k_real_epsilon 0.0001f

real function_30bf0(vector3f *v);
void record_pool_release(s_record_pool *data, long datum_index);
matrix3x3 *function_142da0(real yaw, real pitch, real roll, matrix3x3 *out);
matrix3x3 *function_142eb0(matrix3x3 const *right, matrix3x3 const *left, matrix3x3 *out);
vector3f *function_143070(matrix3x3 const *m, vector3f const *v, vector3f *out);

/* what a shape belongs to (copied into a test's result) */
struct s_shape_header
{
	long unknown00;
	long unknown04;
	long unknown08;
	byte unknown0c;
	byte unknown0d;
	short unknown0e;
};

/* a segment (origin and vector) with a radius */
struct s_capsule
{
	s_shape_header header;
	point3f origin;
	vector3f vector;
	real radius;
};

struct s_sphere
{
	s_shape_header header;
	point3f center;
	real radius;
};

/* a slab: a plane, a thickness, and a convex polygon on the plane projected
   onto two axes */
struct s_prism
{
	s_shape_header header;
	plane3f plane;
	real thickness;
	short axis;
	byte side;
	byte unknown27;
	long point_count;
	point2f points[8];
};

/* projects a point on a prism's plane onto the two axes the prism was flattened along */
PRIVATE void prism_project(real const *q, short axis, byte side, point2f *out)
{
	out->x = q[g_440b94[side + axis * 2][0]];
	out->y = q[g_440b94[side + axis * 2][1]];
}

// @retail 0x245bd0
bool function_245bd0(
	point3f const *point,
	s_capsule const *capsule,
	plane3f *plane,
	real *distance)
{
	vector3f d;
	vector3d_from_points3d(&capsule->origin, point, &d);
	real dot = dot3f(&capsule->vector, &d);
	if (dot >= g_45dbd8)
	{
		real length2 = length_sq3f(&capsule->vector);
		if (length2 >= dot)
		{
			real d2 = length_sq3f(&d);
			if (d2 * length2 - dot * dot < capsule->radius * capsule->radius * length2)
			{
				if (length2 > g_45dbd8)
				{
					real t = dot / length2;
					vector3f scaled;
					scaled.i = capsule->vector.i * t;
					scaled.j = capsule->vector.j * t;
					scaled.k = capsule->vector.k * t;
					plane->i = d.i - scaled.i;
					plane->j = d.j - scaled.j;
					plane->k = d.k - scaled.k;
				}
				else
				{
					plane->n = d;
				}
				real magnitude = function_30bf0(&plane->n);
				if (magnitude == 0.f)
				{
					plane->i = 0.f;
					plane->j = 0.f;
					plane->k = 1.f;
				}
				plane->d = dot3f((vector3f const *)&capsule->origin, &plane->n) + capsule->radius;
				*distance = capsule->radius - magnitude;
				return true;
			}
		}
	}
	return false;
}

PRIVATE inline void shape_difference2d(point2f const *a, point2f const *b, point2f *out)
{
	out->x = a->x - b->x;
	out->y = a->y - b->y;
}

PRIVATE inline real shape_cross2d(point2f const *a, point2f const *b)
{
	return a->x * b->y - a->y * b->x;
}

// @retail 0x245d80
bool function_245d80(
	s_prism const *prism,
	point3f const *point,
	real *distance,
	plane3f *out)
{
	real height = plane_distance_to_point(&prism->plane, point);
	if (height >= 0.f && prism->thickness > height)
	{
		real offset = 0.f - height;
		real q[3];
		q[0] = offset * prism->plane.i + point->x;
		q[1] = prism->plane.j * offset + point->y;
		q[2] = prism->plane.k * offset + point->z;
		point2f p2;
		prism_project(q, prism->axis, prism->side, &p2);
		long count = prism->point_count;
		long i = 0;
		if (count > 0)
		{
			do
			{
				point2f const *a = &prism->points[i];
				point2f da;
				shape_difference2d(a, &p2, &da);
				long next = (i + 1 >= count) ? 0 : i + 1;
				point2f const *b = &prism->points[next];
				point2f db;
				shape_difference2d(b, &p2, &db);
				if (0.f > shape_cross2d(&da, &db))
				{
					return false;
				}
				i++;
			} while (i < count);
		}
		out->n = prism->plane.n;
		out->d = prism->plane.d + prism->thickness;
		*distance = prism->thickness - height;
		return true;
	}
	return false;
}

/* the shapes tested together: spheres, capsules and prisms */
struct s_shapes
{
	short counts[3];
	byte unknown06[2];
	s_sphere spheres[256];
	s_capsule capsules[256];
	s_prism prisms[64];
};

/* the deepest shape a point is in */
struct s_shape_result
{
	s_shape_header header;
	real depth;
	byte unknown14[0xc];
	plane3f plane;
};

PRIVATE inline void shape_scale3d(vector3f const *v, real scale, vector3f *out)
{
	out->i = v->i * scale;
	out->j = scale * v->j;
	out->k = v->k * scale;
}

/* the sum of two squares */
PRIVATE inline real shape_square_sum(real const &a, real const &b)
{
	real square = a * a;
	square += b * b;
	return square;
}

/* The point test inlined into the shape collection query. */
PRIVATE inline bool shape_sphere_point(s_sphere const *sphere, point3f const *point, plane3f *plane, real *depth)
{
	vector3f d;
	vector3d_from_points3d(&sphere->center, point, &d);
	real distance_squared = shape_square_sum(d.i, d.k);
	distance_squared += d.j * d.j;
	if (sphere->radius * sphere->radius > distance_squared)
	{
		real distance = (real)sqrt(distance_squared);
		if (distance > g_45dbd8)
		{
			real scale = 1.f / distance;
			shape_scale3d(&d, scale, &plane->n);
		}
		else
		{
			plane->i = 0.f;
			plane->j = 0.f;
			plane->k = 1.f;
		}
		plane->d = dot3f((vector3f const *)&sphere->center, &plane->n) + sphere->radius;
		*depth = sphere->radius - distance;
		return true;
	}
	return false;
}

PRIVATE inline void shape_copy_header(s_shape_header const *source, s_shape_header *out)
{
	out->unknown00 = source->unknown00;
	out->unknown04 = source->unknown04;
	out->unknown08 = source->unknown08;
	out->unknown0c = source->unknown0c;
	out->unknown0d = source->unknown0d;
	out->unknown0e = source->unknown0e;
}

// @retail 0x245ef0
bool function_245ef0(s_shapes const *shapes, point3f const *point, s_shape_result *result)
{
	short best_type = NONE;
	short best_index = NONE;
	real best_depth = -FLT_MAX;
	plane3f best_plane;
	for (short type = 0; type < 3; type++)
	{
		for (short index = 0; index < shapes->counts[type]; index++)
		{
			real depth;
			plane3f plane;
			if ((type == 0 && shape_sphere_point(&shapes->spheres[index], point, &plane, &depth)) ||
				(type == 1 && function_245bd0(point, &shapes->capsules[index], &plane, &depth)) ||
				(type == 2 && function_245d80(&shapes->prisms[index], point, &depth, &plane)))
			{
				if (depth > best_depth)
				{
					best_type = type;
					best_index = index;
					best_depth = depth;
					best_plane = plane;
				}
			}
		}
	}
	if (best_type != NONE)
	{
		result->depth = best_depth;
		result->plane = best_plane;
		switch (best_type)
		{
		case 0: shape_copy_header(&shapes->spheres[best_index].header, &result->header); break;
		case 1: shape_copy_header(&shapes->capsules[best_index].header, &result->header); break;
		case 2: shape_copy_header(&shapes->prisms[best_index].header, &result->header); break;
		}
		return true;
	}
	return false;
}

PRIVATE inline real shape_dot_reverse(vector3f const *a, vector3f const *b)
{
	return a->k * b->k + a->j * b->j + a->i * b->i;
}

/* the fraction of direction at which point, moving along it, first touches the sphere (0 when it
   starts inside), with the sphere's tangent plane there */
// @retail 0x2461a0
bool function_2461a0(
	s_sphere const *sphere,
	point3f const *point,
	plane3f *plane,
	vector3f const *direction,
	real *t)
{
	bool result = false;
	vector3f d;
	vector3d_from_points3d(point, &sphere->center, &d);
	real radius_squared = sphere->radius * sphere->radius;
	real c = shape_square_sum(d.k, d.i);
	c += d.j * d.j;
	c -= radius_squared;
	if (0.f >= c)
	{
		*t = 0.f;
		result = true;
	}
	else
	{
		/* retail sums these in this order */
		real b = direction->i * d.i + d.k * direction->k + d.j * direction->j;
		if (b > 0.f)
		{
			real a = direction->j * direction->j + direction->k * direction->k + direction->i * direction->i;
			real discriminant = b * b - a * c;
			if (discriminant >= 0.f)
			{
				/* retail compares the unrounded root and divides the rounded one */
				real root = b - sqrt(discriminant);
				if (a >= root)
				{
					*t = root / a;
					result = true;
				}
			}
		}
	}
	if (result)
	{
		vector3f scaled;
		shape_scale3d(direction, *t, &scaled);
		plane->i = scaled.i - d.i;
		plane->j = scaled.j - d.j;
		plane->k = scaled.k - d.k;
		if (function_30bf0(&plane->n) == 0.f)
		{
			plane->i = 0.f;
			plane->j = 0.f;
			plane->k = 1.f;
		}
		plane->d = dot3f(&plane->n, (vector3f const *)&sphere->center) + sphere->radius;
	}
	return result;
}

// score 104 (was 106): every sum now associates as in retail (the compiler reorders unparenthesized
// sums of three products); what still differs is scheduling of the first block: retail multiplies
// v.k by u.k from memory before summing a, and keeps a in XMM3 and uv in XMM0 (ours XMM4, XMM1), which
// swaps registers to the end. Tried: any subset of the four parenthesized sums (151-214), w computed
// k, j, i by hand (211), a or uu parenthesized (180, 206), uv unparenthesized in any order (214),
// uv as += statements (182), a/uv/uu statement orders (104-217), den = a * uu (104).
// @retail 0x246360
bool function_246360(
	vector3f const *direction,
	s_capsule const *capsule,
	point3f const *start,
	real *t,
	plane3f *plane)
{
	vector3f const *u = direction;
	vector3f const *v = &capsule->vector;
	real a = length_sq3f(v);
	real uv = (v->k * u->k + v->j * u->j) + v->i * u->i;
	real uu = length_sq3f(u);
	real den = uu * a - uv * uv;
	if (den == g_45dbd8)
	{
		return false;
	}
	vector3f w;
	vector3d_from_points3d(&capsule->origin, start, &w);
	real uw = (u->j * w.j + u->i * w.i) + u->k * w.k;
	real vw = (v->k * w.k + v->j * w.j) + v->i * w.i;
	real b = vw * uv - uw * a;
	real c = ((w.k * w.k + w.j * w.j) + w.i * w.i - capsule->radius * capsule->radius) * a - vw * vw;
	real disc = b * b - c * den;
	if (0.f > disc)
	{
		return false;
	}
	real root = (real)sqrt(disc);
	real inv = 1.f / den;
	real t_enter = (b - root) * inv;
	real t_exit = (root + b) * inv;
	if (t_enter > 1.f || 0.f > t_exit)
	{
		return false;
	}
	if (0.f > t_enter)
	{
		t_enter = 0.f;
	}
	if (t_exit > 1.f)
	{
		t_exit = 1.f;
	}
	if (uv != 0.f)
	{
		real inv_uv = 1.f / uv;
		real s0 = 0.f - inv_uv * vw;
		real s1 = (a - vw) * inv_uv;
		if (uv > 0.f)
		{
			if (s0 > t_enter)
			{
				t_enter = s0;
			}
			if (t_exit > s1)
			{
				t_exit = s1;
			}
		}
		else
		{
			if (s1 > t_enter)
			{
				t_enter = s1;
			}
			if (t_exit > s0)
			{
				t_exit = s0;
			}
		}
		if (t_enter > t_exit)
		{
			return false;
		}
	}
	else
	{
		if (0.f > vw || vw > a)
		{
			return false;
		}
	}
	*t = t_enter;
	vector3f q;
	q.i = u->i * t_enter + w.i;
	q.j = u->j * t_enter + w.j;
	q.k = u->k * t_enter + w.k;
	real along = ((v->k * q.k + v->j * q.j) + q.i * v->i) / a;
	real back = 0.f - along;
	plane->i = v->i * back + q.i;
	plane->j = v->j * back + q.j;
	plane->k = v->k * back + q.k;
	if (function_30bf0(&plane->n) == 0.f)
	{
		plane->i = 1.f;
		plane->j = 0.f;
		plane->k = 0.f;
	}
	plane->d = capsule->origin.z * plane->k + capsule->origin.y * plane->j + capsule->origin.x * plane->i + capsule->radius;
	return true;
}

/* where along start + t * direction (t in [0, 1]) the segment first enters a prism, and the
   prism's top plane */
// @retail 0x2466b0
bool function_2466b0(
	point3f const *start,
	vector3f const *direction,
	s_prism const *prism,
	real *t,
	plane3f *out)
{
	real b = plane_distance_to_point(&prism->plane, start);
	real t0 = 0.f;
	real t1 = 1.f;
	real a = shape_dot_reverse(&prism->plane.n, direction);
	if (!(fabs(a) < k_real_epsilon))
	{
		real inv = 1.f / a;
		real s0 = 0.f - inv * b;
		real s1 = 0.f - (b - prism->thickness) * inv;
		if (a > 0.f)
		{
			if (s0 > 0.f)
			{
				t0 = s0;
			}
			if (1.f > s1)
			{
				t1 = s1;
			}
		}
		else
		{
			if (s1 > 0.f)
			{
				t0 = s1;
			}
			if (1.f > s0)
			{
				t1 = s0;
			}
		}
		if (t0 > t1)
		{
			return false;
		}
	}
	else if (0.f > b || b >= prism->thickness)
	{
		return false;
	}
	vector3f q0;
	vector3f_add_scaled((vector3f const *)start, &prism->plane.n, -b, &q0);
	real na = 0.f - a;
	real q1[3];
	q1[0] = na * prism->plane.i + direction->i;
	q1[1] = prism->plane.j * na + direction->j;
	q1[2] = prism->plane.k * na + direction->k;
	point2f s2;
	prism_project(&q0.i, prism->axis, prism->side, &s2);
	point2f d2;
	prism_project(q1, prism->axis, prism->side, &d2);
	long count = prism->point_count;
	long i = 0;
	if (count > 0)
	{
		do
		{
			point2f const *vertex = &prism->points[i];
			long next = (i + 1 >= count) ? 0 : i + 1;
			point2f const *next_vertex = &prism->points[next];
			point2f edge;
			vector2d_from_points2d(vertex, next_vertex, &edge);
			point2f offset;
			vector2d_from_points2d(vertex, &s2, &offset);
			real denom = shape_cross2d(&d2, &edge);
			real num = shape_cross2d(&edge, &offset);
			if (!(fabs(denom) < k_real_epsilon))
			{
				real r = num / denom;
				if (denom < 0.f)
				{
					if (r > t0)
					{
						t0 = r;
					}
				}
				else
				{
					if (t1 > r)
					{
						t1 = r;
					}
				}
				if (t0 > t1)
				{
					return false;
				}
			}
			else if (0.f > num)
			{
				return false;
			}
			i++;
		} while (i < count);
	}
	*t = t0;
	out->n = prism->plane.n;
	out->d = prism->plane.d + prism->thickness;
	return true;
}

struct s_ray_shape_result
{
	s_shape_header header;
	real time;
	point3f position;
	plane3f plane;
};

// @retail 0x2469b0
bool function_2469b0(s_shapes const *shapes, point3f const *start,
	vector3f const *direction, s_ray_shape_result *result)
{
	short best_type = NONE;
	short best_index = NONE;
	real best_time = FLT_MAX;
	plane3f best_plane;
	for (short type = 0; type < 3; type++)
	{
		for (short index = 0; index < shapes->counts[type]; index++)
		{
			real time;
			plane3f plane;
			if (((type == 0 && function_2461a0(&shapes->spheres[index], start, &plane, direction, &time)) ||
				(type == 1 && function_246360(direction, &shapes->capsules[index], start, &time, &plane)) ||
				(type == 2 && function_2466b0(start, direction, &shapes->prisms[index], &time, &plane))) &&
				best_time > time && dot3f(direction, &plane.n) < -0.0001f)
			{
				best_time = time;
				best_type = type;
				best_index = index;
				best_plane = plane;
			}
		}
	}
	if (best_type != NONE)
	{
		result->time = best_time;
		point3f *position = &result->position;
		position->x = direction->i * best_time + start->x;
		position->y = direction->j * best_time + start->y;
		position->z = direction->k * best_time + start->z;
		result->plane = best_plane;
		switch (best_type)
		{
		case 0: shape_copy_header(&shapes->spheres[best_index].header, &result->header); break;
		case 1: shape_copy_header(&shapes->capsules[best_index].header, &result->header); break;
		case 2: shape_copy_header(&shapes->prisms[best_index].header, &result->header); break;
		}
		return true;
	}
	result->time = 1.f;
	result->position.x = start->x + direction->i;
	result->position.y = start->y + direction->j;
	result->position.z = start->z + direction->k;
	return false;
}

/* the object at 0x246eb0: clamp(|vector| - 0.5, 0, 1) of the vector at +0x28 */
struct s_object_2470e0
{
	byte unknown00[0x9c];
	vector3f vector9c;
	real unknowna8;
	real unknownac;
};

// @retail 0x2470e0
inline vector3f *function_2470e0(s_object_2470e0 *object)
{
	return &object->vector9c;
}

// @retail 0x2470f0
real *function_2470f0(s_object_2470e0 *object)
{
	return &object->unknowna8;
}

struct s_object_247120
{
	byte unknown00[0xc];
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word flag3 : 1;
	word flag4 : 1;
	word flag5 : 1;
	word flag6 : 1;
	word flag7 : 1;
	word flag8 : 1;
	word flag9 : 1;
	word flag10 : 1;
	word flag11 : 1;
	word flag12 : 1;
	word flag13 : 1;
	word flag14 : 1;
	word flag15 : 1;
	byte unknown0e[8];
	word bit0 : 1;
	word bit1 : 1;
	word bit2 : 1;
	word bit3 : 1;
	word bit4 : 1;
	word bit5 : 1;
	word bit6 : 1;
	word bit7 : 1;
	word bit8 : 1;
	word bit9 : 1;
	word bit10 : 1;
	word bit11 : 1;
	word bit12 : 1;
	word bit13 : 1;
	word bit14 : 1;
	word bit15 : 1;
};

// @retail 0x247120
long function_247120(s_object_247120 const *object)
{
	return object->bit7;
}

// @retail 0x247130
long function_247130(s_object_247120 const *object)
{
	return TEST_FIELD_BIT(object->flag9);
}

struct s_frame_2477b0
{
	byte unknown00[0x10];
	matrix3x3 rotation;
	point3f position;
};

// @retail 0x2477b0
void function_2477b0(
	s_object_2470e0 *object,
	s_frame_2477b0 *frame,
	transform4x3f const *source)
{
	if (source)
	{
		real const *angles = function_2470f0(object);
		frame->rotation.forward = source->forward;
		frame->rotation.left = source->left;
		frame->rotation.up = source->up;
		frame->position = source->position;
		vector3f offset;
		function_2477b1(&frame->rotation, function_2470e0(object), &offset);
		if (!(fabs(angles[1]) < k_real_epsilon) || !(fabs(angles[0]) < k_real_epsilon))
		{
			matrix3x3 rotation;
			function_142da0(angles[0], angles[1], 0.f, &rotation);
			function_142eb0(&frame->rotation, &rotation, &frame->rotation);
		}
		frame->position.x += offset.i;
		frame->position.y += offset.j;
		frame->position.z += offset.k;
	}
}
