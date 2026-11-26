// @flags /O2 /Gr /arch:SSE
/* UNKNOWN_11CB00.CPP: language and region codes, quaternion and ray math */

#include "unknown_11c920.h"
#include "segment_sphere.h"
#include "globals.h"
#include "unknown_11cb00.h"
#include "unknown_11cc90.h"
#include "vector_products.h"
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

// Gemini GB r19 (held as a patch, coordinator review): quaternion_rotate_point summed in i, j, k order with named
// square and dot locals and w2 computed after d, plus the existing point3f_scale for the scaled position: 8 (was 35),
// and 0x11dd80 4 (was 48). The locals and the w2 placement are statement and operand order, kept only with a byte
// match. Left at 8: the dot sums (j + i) + k in retail and cy multiplies q->i * v->z.
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

// score 37 (was 77): vector_products.h helpers (length_sq3f, vector3d_from_points3d, dot3f, vector3f_add_scaled) and inline ternary clamp match lines 1-46; left: two-return tail schedule and else subtraction order. Single-exit `real result;` matches byte-for-byte (0, 290/290 B; also improves 0x260670) but triggers LTCG auto-inlining in matched 0x1a8fa0 (435), so held as a patch; in-place v 48 (inlines 0x1a8fa0); kji/jki return sums 39; parenthesized xyz return 41; if/else clamp with helpers 78.
// @retail 0x11e000
real function_11e000(
	point3f const *b,
	point3f const *a,
	vector3f const *d)
{
	if (length_sq3f(d) > 0.0001f)
	{
		vector3f v;
		vector3d_from_points3d(a, b, &v);
		real t = dot3f(&v, d) / length_sq3f(d);
		vector3f diff;
		vector3f_add_scaled(&v, d, -(t < 0.f ? 0.f : (t > 1.f ? 1.f : t)), &diff);
		return length_sq3f(&diff);
	}
	else
	{
		vector3f u;
		vector3d_from_points3d(b, a, &u);
		return length_sq3f(&u);
	}
}

PRIVATE inline real function_11e5e1(vector3f const *arg_0, vector3f const *arg_1)
{
	return arg_0->i * arg_1->i + arg_0->j * arg_1->j + arg_0->k * arg_1->k;
}

// Matched in a full build with stand-in SDK headers (was 10): the delta is computed in i, j, k order and the
// file-local dot helper adds its terms in i, j, k order.
// Before:
// score 10 (was 28): the delta components are computed i, k, j and the
// commutative 0x11e5e1 dot product is called as (&delta, &d); both change the
// SSE register schedule. The residual is the y/z XMM pair (xmm4/xmm5) and the
// schedule that follows it.
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
		real b = function_11e5e1(&delta, &d);

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

PRIVATE __forceinline void cross_out_11ea30(vector3f const *a, vector3f const *b, vector3f *out)
{
	out->i = a->j * b->k - b->j * a->k;
	out->j = a->k * b->i - a->i * b->k;
	out->k = b->j * a->i - a->j * b->i;
}

PRIVATE __forceinline void point_out_11ea30(point3f const *origin, vector3f const *direction, real parameter, point3f *out)
{
	out->x = direction->i * parameter + origin->x;
	out->y = direction->j * parameter + origin->y;
	out->z = direction->k * parameter + origin->z;
}

// score 23 (was 652) in a full build with stand-in SDK headers: the cross and the point helpers write through
// out-pointers instead of returning vectors by value (the by-value forms were built as 21 dword copy pairs, frame
// 0x28 against retail's 0x1c), cross and dot are interleaved as in 0x11e130, the delta is computed before the
// normal, and the final difference has its own local. Left: the sum order of dot(u, delta) in the parallel arm and
// `radius * radius`, which retail computes last.
// Gemini GB r19 (held as a patch, coordinator review): MATCH for 0x11ea30 and 0x11e130 with cross3f in place of the
// file's cross copies, one point helper for both functions, the final test as `dot <= radius * radius`, vv grouped as
// i + (j + k), and a second dot helper (k, j, i accumulated) used only for the parallel arm's start while the shared
// one names its three products. Two dot texts picked per call site are per-site twins, so the match is not kept;
// one dot text for every caller is needed.
// @retail 0x11ea30
long function_11ea30(point3f const *a, vector3f const *u, point3f const *b, vector3f const *v, real radius)
{
	vector3f delta;
	vector3d_from_points3d(a, b, &delta);
	vector3f normal;
	cross_out_11ea30(u, v, &normal);
	real normal_squared = dot_11ea30(&normal, &normal);
	real ta, tb;
	if (!(fabs(normal_squared) < 0.0001f))
	{
		real inverse = 1.0f / normal_squared;
		normal.i *= inverse;
		normal.j *= inverse;
		normal.k *= inverse;
		vector3f cross_a;
		cross_out_11ea30(&delta, v, &cross_a);
		ta = dot_11ea30(&cross_a, &normal);
		vector3f cross_b;
		cross_out_11ea30(&delta, u, &cross_b);
		tb = dot_11ea30(&cross_b, &normal);
		bool outside_a = ta < 0.0f || ta > 1.0f;
		bool outside_b = tb < 0.0f || tb > 1.0f;
		if (outside_a || outside_b)
		{
			point3f endpoint_a, endpoint_b;
			if (outside_a)
				point_out_11ea30(a, u, ta < 0.0f ? 0.0f : 1.0f, &endpoint_a);
			if (outside_b)
				point_out_11ea30(b, v, tb < 0.0f ? 0.0f : 1.0f, &endpoint_b);
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
	point3f point_a;
	point_out_11ea30(a, u, ta, &point_a);
	point3f point_b;
	point_out_11ea30(b, v, tb, &point_b);
	vector3f difference;
	vector3d_from_points3d(&point_a, &point_b, &difference);
	if (radius * radius >= dot_11ea30(&difference, &difference))
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

// score 109 (was 126): w, e1 and e2 are vector3f built with vector3d_from_points3d in main's statement order, the normal comes from cross3f, d from dot3f and the length test from length_sq3f. Gemini lane GA r19 reached 27 (551/551 B, frame 0x30) by also declaring n first, computing b-a before c-a, reading the i1 index and fields before i0 (vector2f w2/b2/c2) and writing the last test as cb + ca <= cd + 0.0001f; those are declaration, statement and operand order changes, kept only with a byte match, so that form is held as a patch (left there: the dot3f y/z pair and the 2D projection's i1/i0 registers). Other forms from that round: both short indices loaded before projection 64; i0 before i1 with natural fields 35 (553 B) or swapped fields 27 (551 B); scalar reals for 2D coordinates 49 (frame 0x34); inline 2D helper 60/64 (matches lines 1-75); proj pointer or direct table indexing 36/72-97; dot_11ea30 62; function_120810 120.
// @retail 0x11e800
bool function_11e800(point3f const *p, point3f const *a, point3f const *b, point3f const *c, real *u, real *v)
{
	vector3f w;
	vector3f e2;
	vector3f e1;
	vector3f n;
	real d;

	vector3d_from_points3d(a, p, &w);
	vector3d_from_points3d(a, c, &e1);
	vector3d_from_points3d(a, b, &e2);
	cross3f(&e2, &e1, &n);
	d = dot3f(&n, &w);
	if (length_sq3f(&n) * 0.0001f > d * d)
	{
		short axis = function_120850(&n);
		bool positive = n.n[axis] > 0.f;
		short const *indices = g_440b94[axis * 2 + positive];
		long i0 = indices[0];
		long i1 = indices[1];
		real w1 = w.n[i1];
		real e2_1 = e2.n[i1];
		real e2_0 = e2.n[i0];
		real w0 = w.n[i0];
		real ca = w1 * e2_0 - e2_1 * w0;

		if (ca > -0.0001f)
		{
			real e1_1 = e1.n[i1];
			real e1_0 = e1.n[i0];
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
	out->i = a->j * b->k - b->j * a->k;
	out->j = a->k * b->i - a->i * b->k;
	out->k = b->j * a->i - a->j * b->i;
}

PRIVATE __forceinline void point_11e130(point3f const *origin, vector3f const *direction, real parameter, point3f *out)
{
	out->x = direction->i * parameter + origin->x;
	out->y = direction->j * parameter + origin->y;
	out->z = direction->k * parameter + origin->z;
}

// score 24 (was 243) in a full build with stand-in SDK headers: the delta between the points is computed before the
// normal's cross product, which shifts every product's operand order to retail's, and the final difference has its
// own local instead of reusing delta. Left: cross_a's j row (retail copies delta.k and folds [v.i]) and the sum
// order of dot(u, delta) in the parallel arm.
// @retail 0x11e130
real function_11e130(point3f const *a, vector3f const *u, point3f const *b, vector3f const *v)
{
	vector3f delta;
	vector3d_from_points3d(a, b, &delta);
	vector3f normal;
	cross_11e130(u, v, &normal);
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
	vector3f difference;
	vector3d_from_points3d(&point_a, &point_b, &difference);
	return dot_11ea30(&difference, &difference);
}
