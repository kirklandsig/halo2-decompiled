/* UNKNOWN_0259D0.H: points, vectors and the math on them */

#ifndef UNKNOWN_0259D0_H
#define UNKNOWN_0259D0_H

#include <math.h>

union point3f
{
	real n[3];
	struct { real x, y, z; };
};

union vector3f
{
	real n[3];
	struct { real i, j, k; };

	/* 0x24f7b0 (thiscall on a unit direction): true when both quantize to the
	   same 17 bit index, or decode to points within 0.05 of each other */
	bool quantized_equal(vector3f const *other) const;
};

struct color3f { real red, green, blue; };
struct color4f { real alpha, red, green, blue; };
struct hsv3f { real hue, saturation, value; };

struct point2f { real x, y; };
struct real_bounds { real lo, hi; };
struct box2f { real x0, x1, y0, y1; };

union quaternionf
{
	real n[4];
	struct { real i, j, k, w; };
	struct { vector3f vector; real scalar; };
};

struct matrix3x3
{
	vector3f forward;
	vector3f left;
	vector3f up;
};

/* scale, three axis vectors and a position: 13 reals */
struct transform4x3f
{
	real scale;
	union
	{
		matrix3x3 rotation;
		struct
		{
			vector3f forward;
			vector3f left;
			vector3f up;
		};
	};
	point3f position;
};

struct plane3f
{
	union
	{
		vector3f n;
		struct { real i, j, k; };
	};
	real d;
};

/* a float to integer truncation (cvttss2si), as retail's inlined casts */
__forceinline long real_truncate(real x)
{
	__asm
	{
		movss xmm0, x
		cvttss2si eax, xmm0
	}
}

/* a copy of function_30bf0 (normalize) for files where retail inlines it;
   unknown_030290.cpp is /Ob1, which keeps its own out of line */
static inline real normalize_inline(vector3f *v)
{
	real m = (real)sqrt(v->j * v->j + (v->i * v->i + v->k * v->k));
	if (!(fabs(m) < 0.0001f))
	{
		real inv = 1.f / m;
		v->i = inv * v->i;
		v->j = inv * v->j;
		v->k = v->k * inv;
		return m;
	}
	return 0.f;
}

/* retail's table at 0x440b94: for each side (axis * 2 + positive) the three
   coordinate indices, the projection axes last. Defined in unknown_11fc80.cpp. */
extern short const g_440b94[6][3];
/* retail's table at 0x440bb8: four index triples */
extern short const g_440bb8[4][3];

static inline real dot3f(vector3f const *a, vector3f const *b)
{
	return a->i * b->i + a->j * b->j + a->k * b->k;
}

static inline real length_sq3f(vector3f const *v)
{
	return v->i * v->i + v->j * v->j + v->k * v->k;
}

static inline void vector3d_from_points3d(point3f const *p0, point3f const *p1, vector3f *out)
{
	out->i = p1->x - p0->x;
	out->j = p1->y - p0->y;
	out->k = p1->z - p0->z;
}

static inline void vector2d_from_points2d(point2f const *p0, point2f const *p1, point2f *out)
{
	out->x = p1->x - p0->x;
	out->y = p1->y - p0->y;
}

__inline point3f *real_point3d_set(point3f *point, real x, real y, real z)
{
	point->x = x;
	point->y = y;
	point->z = z;
	return point;
}

static inline void vector3f_assign(vector3f *vector, real i, real j, real k)
{
	vector->i = i;
	vector->j = j;
	vector->k = k;
}

static inline real plane_distance_to_point(plane3f const *plane, point3f const *point)
{
	return plane->i * point->x + plane->j * point->y + plane->k * point->z - plane->d;
}

/* q scaled to unit length, or the identity rotation when it has no length;
   the inverse length is kept in double precision */
static __forceinline void quaternion_unit_or_identity(quaternionf *q)
{
	real length_squared = q->w * q->w + q->k * q->k + q->j * q->j + q->i * q->i;

	if (length_squared > 0.0f)
	{
		double scale = 1.0f / sqrt(length_squared);

		q->i = (real)(q->i * scale);
		q->j = (real)(q->j * scale);
		q->k = (real)(q->k * scale);
		q->w = (real)(q->w * scale);
	}
	else
	{
		q->i = 0.0f;
		q->j = 0.0f;
		q->k = 0.0f;
		q->w = 1.0f;
	}
}

/* the global random generator step (146240) and the 16 bit scaling of it */
static inline dword random_next(dword *seed)
{
	*seed = *seed * 0x19660d + 0x3c6ef35f;
	return *seed >> 16;
}

static inline short random_index(dword *seed, short range)
{
	return (short)((random_next(seed) * range) >> 16);
}

/* the generator step of the seeds in g_4e7408 and the real in [0, 1] drawn
   from it; retail inlines both and drops the file and line arguments */
static inline dword _random(dword *seed, char const *file, long line)
{
	*seed = 1664525 * *seed + 1013904223;
	return *seed >> 16;
}

static inline real function_x82e52f(dword *seed, char const *file, long line)
{
	return (real)_random(seed, file, line) * (1.f / 65535.f);
}

real distance3d(point3f const *a, point3f const *b);
real function_259d0(dword *seed, char const *file, long line, real lower_bound, real upper_bound);

#endif
