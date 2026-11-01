// @flags /O2 /Gr /arch:SSE
/* UNKNOWN_11CB00.CPP: language and region codes, quaternion and ray math */

#include "unknown_11c920.h"
#include "segment_sphere.h"
#include "globals.h"
#include "unknown_11cb00.h"
#include "unknown_11cc90.h"
#include <math.h>
#include <xtl.h>

long g_47ff34 = -1;

void function_11d610(quaternionf *q);
void function_11d820(quaternionf const *a, quaternionf const *b, quaternionf *out);
short function_120850(vector3f const *v);

// @retail 0x11cb00
char const *function_11cb00(
	long language)
{
	struct
	{
		long language;
		char const *name;
	} const names[9] =
	{
		{ 0, "" },
		{ 1, "jpn" },
		{ 2, "de" },
		{ 3, "fr" },
		{ 4, "sp" },
		{ 5, "it" },
		{ 6, "kor" },
		{ 7, "cht" },
		{ 8, "pt" }
	};
	char const *name = "";
	dword i = 0;

	do
	{
		if (names[i].language == language)
		{
			name = names[i].name;
			break;
		}
		i++;
	}
	while (i < 9);
	return name;
}

// @retail 0x11cbb0
long function_11cbb0()
{
	if (g_47ff34 == NONE)
	{
		long region = 0;

		switch (XGetGameRegion())
		{
		case XC_GAME_REGION_NA:
			region = 0;
			break;
		case XC_GAME_REGION_JAPAN:
			region = 1;
			break;
		case XC_GAME_REGION_RESTOFWORLD:
			region = 2;
			break;
		case XC_GAME_REGION_MANUFACTURING:
			region = 3;
			break;
		}
		g_47ff34 = region;
	}
	return g_47ff34;
}

// @retail 0x11d950
void function_11d950(
	quaternionf const *b,
	quaternionf const *a,
	quaternionf *out,
	real t)
{
	real s = 1.f - t;
	real dot = a->k * b->k + a->j * b->j + b->w * a->w + b->i * a->i;

	if (dot < 0.f)
		t = 0.f - t;
	out->i = b->i * s + a->i * t;
	out->j = a->j * t + s * b->j;
	out->k = a->k * t + s * b->k;
	out->w = b->w * s + t * a->w;
	function_11d610(out);
}

struct s_11da11
{
	real field_0;
	real field_4;
	union
	{
		real field_8;
		long field_08;
	};
};

// @retail 0x11da10
void function_11da10(
	quaternionf const *a,
	quaternionf const *b,
	quaternionf *out,
	real t)
{
	s_11da11 local_0;
	local_0.field_4 = 1.f - t;
	local_0.field_0 = a->j * b->j + a->k * b->k + b->i * a->i + a->w * b->w;
	real sin_omega;
	real ta;

	if (local_0.field_0 > 1.f)
		local_0.field_0 = 1.f;
	else if (-1.f > local_0.field_0)
		local_0.field_0 = -1.f;
	local_0.field_8 = (real)acos(local_0.field_0);
	sin_omega = (real)sin(local_0.field_8);
	if (!(0.0001f > (real)fabs(sin_omega)))
	{
		real inv = 1.f / sin_omega;

		ta = (real)sin(local_0.field_8 * t) * inv;
		local_0.field_4 = (real)sin(local_0.field_8 * local_0.field_4) * inv;
		if (local_0.field_0 < 0.f)
			ta = 0.f - ta;
	}
	else
	{

		__asm
		{
			fld t
			fistp local_0.field_08
		}
		if (0.f > (real)local_0.field_08)
		{
			ta = 0.f;
		}
		else
		{
			__asm
			{
				fld t
				fistp local_0.field_08
			}
			if ((real)local_0.field_08 > 1.f)
			{
				ta = 1.f;
			}
			else
			{
				__asm
				{
					fld t
					fistp local_0.field_08
				}
				ta = (real)local_0.field_08;
			}
		}
		local_0.field_4 = 1.f - ta;
	}
	out->i = a->i * local_0.field_4 + b->i * ta;
	out->j = b->j * ta + local_0.field_4 * a->j;
	out->k = b->k * ta + local_0.field_4 * a->k;
	out->w = b->w * ta + a->w * local_0.field_4;
}

// rotates a point by a unit quaternion
__inline void quaternion_rotate_point(
	quaternionf const *q,
	point3f const *v,
	point3f *out)
{
	real w2 = q->w * 2.f;
	real m = q->w * q->w * 2.f - 1.f;
	real d = (q->j * v->y + v->x * q->i + v->z * q->k) * 2.f;
	real cx = q->j * v->z - v->y * q->k;
	real cy = v->x * q->k - q->i * v->z;
	real cz = q->i * v->y - q->j * v->x;

	out->x = cx * w2 + v->x * m + q->i * d;
	out->y = q->j * d + cy * w2 + m * v->y;
	out->z = v->z * m + cz * w2 + d * q->k;
}

// @retail 0x11dbb0
void function_11dbb0(
	real_quaternion_transform *out,
	real_quaternion_transform const *a,
	real_quaternion_transform const *b)
{
	real_quaternion_transform temp;

	if (a == out)
	{
		temp = *a;
		a = &temp;
	}
	if (b == out)
	{
		temp = *b;
		b = &temp;
	}
	function_11d820(&a->rotation, &b->rotation, &out->rotation);
	quaternion_rotate_point(&a->rotation, &b->position, &out->position);
	if (a->scale != 1.f)
	{
		out->position.x *= a->scale;
		out->position.y *= a->scale;
		out->position.z *= a->scale;
		out->scale = b->scale * a->scale;
	}
	else
	{
		out->scale = b->scale;
	}
	out->position.x = a->position.x + out->position.x;
	out->position.y = a->position.y + out->position.y;
	out->position.z = a->position.z + out->position.z;
}

/* p scaled by s into result (p and result may be the same point) */
static __inline point3f *point3f_scale(point3f const *p, real s, point3f *result)
{
	result->x = s * p->x;
	result->y = s * p->y;
	result->z = s * p->z;
	return result;
}

// @retail 0x11dd80
void function_11dd80(
	real_quaternion_transform const *in,
	real_quaternion_transform *out)
{
	real inv;

	out->rotation = in->rotation;
	out->rotation.w = out->rotation.w * -1.f;
	quaternion_rotate_point(&out->rotation, &in->position, &out->position);
	if (0.f == in->scale)
		inv = 10000.f;
	else if (1.f == in->scale)
		inv = 1.f;
	else
		inv = 1.f / in->scale;
	point3f_scale(&out->position, inv, &out->position);
	point3f_scale(&out->position, -1.f, &out->position);
	out->scale = inv;
}

// @retail 0x11df30
vector2f *function_11df30(
	vector2f *angles,
	vector3f const *v)
{
	angles->i = (real)atan2(v->j, v->i);
	angles->j = (real)atan2(v->k, sqrt(v->j * v->j + v->i * v->i));
	return angles;
}

// @retail 0x11e000
real function_11e000(
	point3f const *b,
	point3f const *a,
	vector3f const *d)
{
	if (d->j * d->j + d->k * d->k + d->i * d->i > 0.0001f)
	{
		vector3f v;
		real t;

		v.k = b->z - a->z;
		v.i = b->x - a->x;
		v.j = b->y - a->y;
		t = (d->i * v.i + v.k * d->k + v.j * d->j) / (d->j * d->j + d->k * d->k + d->i * d->i);
		real nt;
		real x;
		real y;
		real z;

		if (0.f > t)
			t = 0.f;
		else if (t > 1.f)
			t = 1.f;
		nt = 0.f - t;
		x = d->i * nt + v.i;
		y = nt * d->j + v.j;
		z = nt * d->k + v.k;
		return z * z + y * y + x * x;
	}
	else
	{
		vector3f u;

		u.j = a->y - b->y;
		u.i = a->x - b->x;
		u.k = a->z - b->z;
		return u.k * u.k + u.j * u.j + u.i * u.i;
	}
}

PRIVATE inline real function_11e5e1(vector3f const *arg_0, vector3f const *arg_1)
{
	return arg_0->k * arg_1->k + arg_0->j * arg_1->j + arg_0->i * arg_1->i;
}

// @retail 0x11e5e0
bool function_11e5e0(
	point3f const *segment_start,
	vector3f const *direction,
	point3f const *arg_28f920,
	real radius)
{
	vector3f delta;
	real c;

	delta.i = segment_start->x - arg_28f920->x;
	delta.j = segment_start->y - arg_28f920->y;
	delta.k = segment_start->z - arg_28f920->z;
	c = function_11e5e1(&delta, &delta) - radius * radius;

	if (c < 0.f)
		return true;
	else
	{
		vector3f d = *direction;
		real b = function_11e5e1(&d, &delta);

		if (b >= 0.f)
			return false;
		else
		{
			real a = function_11e5e1(&d, &d);
			real disc = b * b - a * c;

			if (disc <= 0.f)
				return false;
			else
			{
				real e = 0.f - a - b;

				if (e < 0.f)
					return true;
				return disc > e * e;
			}
		}
	}
}

PRIVATE __forceinline vector3f cross_11ea30(vector3f const *a, vector3f const *b)
{
	vector3f result;
	result.i = a->j * b->k - b->j * a->k;
	result.j = a->k * b->i - a->i * b->k;
	result.k = b->j * a->i - a->j * b->i;
	return result;
}

PRIVATE __forceinline real dot_11ea30(vector3f const *a, vector3f const *b)
{
	return a->k * b->k + a->j * b->j + a->i * b->i;
}

PRIVATE __forceinline real clamp_11ea30(real value)
{
	return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

PRIVATE __forceinline point3f point_11ea30(point3f const *origin, vector3f const *direction, real parameter)
{
	point3f result;
	result.x = direction->i * parameter + origin->x;
	result.y = direction->j * parameter + origin->y;
	result.z = direction->k * parameter + origin->z;
	return result;
}

// @retail 0x11ea30
long function_11ea30(point3f const *a, vector3f const *u, point3f const *b, vector3f const *v, real radius)
{
	vector3f normal = cross_11ea30(u, v);
	vector3f delta;
	vector3d_from_points3d(a, b, &delta);
	real normal_squared = dot_11ea30(&normal, &normal);
	real ta, tb;
	if (!(fabs(normal_squared) < 0.0001f))
	{
		real inverse = 1.0f / normal_squared;
		normal.i *= inverse;
		normal.j *= inverse;
		normal.k *= inverse;
		vector3f cross_a = cross_11ea30(&delta, v);
		vector3f cross_b = cross_11ea30(&delta, u);
		ta = dot_11ea30(&cross_a, &normal);
		tb = dot_11ea30(&cross_b, &normal);
		bool outside_a = ta < 0.0f || ta > 1.0f;
		bool outside_b = tb < 0.0f || tb > 1.0f;
		if (outside_a || outside_b)
		{
			point3f endpoint_a, endpoint_b;
			if (outside_a)
				endpoint_a = point_11ea30(a, u, ta < 0.0f ? 0.0f : 1.0f);
			if (outside_b)
				endpoint_b = point_11ea30(b, v, tb < 0.0f ? 0.0f : 1.0f);
			if ((outside_a && function_11e5e0(b, v, &endpoint_a, radius)) ||
				(outside_b && function_11e5e0(a, u, &endpoint_b, radius)))
				return 1;
			return 0;
		}
	}
	else
	{
		real uv = dot_11ea30(u, v);
		real uu = u->j * u->j + u->k * u->k + u->i * u->i;
		if (uu > 0.0001f)
		{
			real inverse = 1.0f / uu;
			real start = dot_11ea30(u, &delta) * inverse;
			real end = uv * inverse + start;
			ta = (clamp_11ea30(start) + clamp_11ea30(end)) * 0.5f;
		}
		else
			ta = 0.0f;
		real vv = v->j * v->j + v->k * v->k + v->i * v->i;
		if (vv > 0.0001f)
		{
			real inverse = 1.0f / vv;
			real start = 0.0f - dot_11ea30(v, &delta) * inverse;
			real end = uv * inverse + start;
			tb = (clamp_11ea30(start) + clamp_11ea30(end)) * 0.5f;
		}
		else
			tb = 0.0f;
	}
	point3f point_a = point_11ea30(a, u, ta);
	point3f point_b = point_11ea30(b, v, tb);
	vector3d_from_points3d(&point_a, &point_b, &delta);
	if (radius * radius >= dot_11ea30(&delta, &delta))
		return 1;
	return 0;
}

/* the direction comes before the center: callers set up register arguments last to first, and retail's
   0x1a8c30 loads the center before the direction */
// @retail 0x11e6f0
real function_11e6f0(
	point3f const *origin,
	vector3f const *direction,
	point3f const *center,
	real radius)
{
	vector3f delta;
	real c;

	delta.i = origin->x - center->x;
	delta.j = origin->y - center->y;
	delta.k = origin->z - center->z;
	c = delta.k * delta.k + delta.j * delta.j + delta.i * delta.i - radius * radius;

	if (c < 0.f)
		return 0.f;
	else
	{
		vector3f d = *direction;
		real b = d.i * delta.i + d.j * delta.j + d.k * delta.k;

		if (b >= 0.f)
			return 3.4028235e38f;
		else
		{
			real a = d.k * d.k + d.j * d.j + d.i * d.i;
			real disc = b * b - a * c;

			if (disc <= 0.f)
				return 3.4028235e38f;
			return (0.f - b - (real)sqrt(disc)) / a;
		}
	}
}

// @retail 0x11e800
bool function_11e800(
	point3f const *a,
	point3f const *b,
	point3f const *c,
	point3f const *p,
	real *u,
	real *v)
{
	real w[3];
	real e2[3];
	real e1[3];
	vector3f n;
	real d;

	w[0] = p->x - a->x;
	w[1] = p->y - a->y;
	w[2] = p->z - a->z;
	e1[0] = b->x - a->x;
	e1[1] = b->y - a->y;
	e1[2] = b->z - a->z;
	e2[0] = c->x - a->x;
	e2[1] = c->y - a->y;
	e2[2] = c->z - a->z;
	n.i = e1[2] * e2[1] - e1[1] * e2[2];
	n.j = e2[2] * e1[0] - e1[2] * e2[0];
	n.k = e1[1] * e2[0] - e2[1] * e1[0];
	d = n.j * w[1] + n.k * w[2] + n.i * w[0];
	if ((n.k * n.k + n.j * n.j + n.i * n.i) * 0.0001f > d * d)
	{
		short axis = function_120850(&n);
		bool positive = n.n[axis] > 0.f;
		short const *indices = g_440b94[axis * 2 + positive];
		long i0 = indices[0];
		long i1 = indices[1];
		real w1 = w[i1];
		real e2_1 = e2[i1];
		real e2_0 = e2[i0];
		real w0 = w[i0];
		real ca = w1 * e2_0 - e2_1 * w0;

		if (ca > -0.0001f)
		{
			real e1_1 = e1[i1];
			real e1_0 = e1[i0];
			real cb = e1_1 * w0 - w1 * e1_0;

			if (cb > -0.0001f)
			{
				real cd = e1_1 * e2_0 - e2_1 * e1_0;

				if (-0.0001f > cd || cd > 0.0001f)
				{
					if (cd + 0.0001f >= cb + ca)
					{
						real inv = 1.f / cd;

						*u = inv * cb;
						*v = inv * ca;
						return true;
					}
				}
			}
		}
	}
	return false;
}

// @retail 0x11f9a0
long function_11f9a0(
	box3f const *rectangle,
	long maximum_vertex_count,
	point3f vertices[])
{
	vertices[0].x = rectangle->x0;
	vertices[0].y = rectangle->y0;
	vertices[0].z = rectangle->z0;
	vertices[1].x = rectangle->x1;
	vertices[1].y = rectangle->y0;
	vertices[1].z = rectangle->z0;
	vertices[2].x = rectangle->x0;
	vertices[2].y = rectangle->y1;
	vertices[2].z = rectangle->z0;
	vertices[3].x = rectangle->x1;
	vertices[3].y = rectangle->y1;
	vertices[3].z = rectangle->z0;
	vertices[4].x = rectangle->x0;
	vertices[4].y = rectangle->y0;
	vertices[4].z = rectangle->z1;
	vertices[5].x = rectangle->x1;
	vertices[5].y = rectangle->y0;
	vertices[5].z = rectangle->z1;
	vertices[6].x = rectangle->x0;
	vertices[6].y = rectangle->y1;
	vertices[6].z = rectangle->z1;
	vertices[7].x = rectangle->x1;
	vertices[7].y = rectangle->y1;
	vertices[7].z = rectangle->z1;
	return 8;
}

PRIVATE __forceinline void cross_11e130(vector3f const *a, vector3f const *b, vector3f *out)
{
	out->i = b->k * a->j - a->k * b->j;
	out->j = b->i * a->k - b->k * a->i;
	out->k = a->i * b->j - b->i * a->j;
}

PRIVATE __forceinline void point_11e130(point3f const *origin, vector3f const *direction, real parameter, point3f *out)
{
	out->x = direction->i * parameter + origin->x;
	out->y = direction->j * parameter + origin->y;
	out->z = direction->k * parameter + origin->z;
}

// @retail 0x11e130
real function_11e130(point3f const *a, vector3f const *u, point3f const *b, vector3f const *v)
{
	vector3f normal;
	cross_11e130(u, v, &normal);
	vector3f delta;
	vector3d_from_points3d(a, b, &delta);
	real normal_squared = dot_11ea30(&normal, &normal);
	real ta, tb;
	if (!(fabs(normal_squared) < 0.0001f))
	{
		real inverse = 1.0f / normal_squared;
		normal.i *= inverse;
		normal.j *= inverse;
		normal.k *= inverse;
		vector3f cross_a;
		cross_11e130(&delta, v, &cross_a);
		ta = dot_11ea30(&cross_a, &normal);
		vector3f cross_b;
		cross_11e130(&delta, u, &cross_b);
		tb = dot_11ea30(&cross_b, &normal);
		bool outside_a = ta < 0.0f || ta > 1.0f;
		bool outside_b = tb < 0.0f || tb > 1.0f;
		if (outside_a || outside_b)
		{
			real distance_a = 3.402823466e+38f;
			real distance_b = 3.402823466e+38f;
			if (outside_a)
			{
				point3f endpoint;
				point_11e130(a, u, ta < 0.0f ? 0.0f : 1.0f, &endpoint);
				distance_a = function_11e000(&endpoint, b, v);
			}
			if (outside_b)
			{
				point3f endpoint;
				point_11e130(b, v, tb < 0.0f ? 0.0f : 1.0f, &endpoint);
				distance_b = function_11e000(&endpoint, a, u);
			}
			return distance_a > distance_b ? distance_b : distance_a;
		}
	}
	else
	{
		real uv = dot_11ea30(u, v);
		real uu = u->j * u->j + u->k * u->k + u->i * u->i;
		if (uu > 0.0001f)
		{
			real inverse = 1.0f / uu;
			real start = dot_11ea30(u, &delta) * inverse;
			real end = uv * inverse + start;
			ta = (clamp_11ea30(start) + clamp_11ea30(end)) * 0.5f;
		}
		else
			ta = 0.0f;
		real vv = v->j * v->j + v->k * v->k + v->i * v->i;
		if (vv > 0.0001f)
		{
			real inverse = 1.0f / vv;
			real start = 0.0f - dot_11ea30(v, &delta) * inverse;
			real end = uv * inverse + start;
			tb = (clamp_11ea30(start) + clamp_11ea30(end)) * 0.5f;
		}
		else
			tb = 0.0f;
	}
	point3f point_a;
	point_11e130(a, u, ta, &point_a);
	point3f point_b;
	point_11e130(b, v, tb, &point_b);
	vector3d_from_points3d(&point_a, &point_b, &delta);
	return dot_11ea30(&delta, &delta);
}
