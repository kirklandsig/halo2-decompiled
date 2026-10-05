// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_02B400.CPP: vector math and rectangular grid placement */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <math.h>

#define k_real_epsilon 0.0001f

// @retail 0x48e70
long function_48e70(real value)
{
	long bits = *(long *)&value;
	long exponent = (bits >> 23) & 255;
	long shift = 150 - exponent;
	long mantissa = ((bits & 0x7fffff) | 0x800000) >> shift;
	long sign = bits >> 31;
	long small = (exponent - 127) >> 31;
	long result = ((mantissa ^ sign) - sign) & ~small;
	long increment;
	if (!sign && bits && (small || (((1 << shift) - 1) & bits & 0x7fffff)))
		increment = 1;
	else
		increment = 0;
	return result + increment;
}

struct s_sphere_plane_volume
{
	point3f center;
	real radius;
	long count;
	plane3f *planes;
};

// @retail 0x38390
bool function_38390(point3f const *point, s_sphere_plane_volume const *volume)
{
	vector3f delta;
	delta.i = point->x - volume->center.x;
	delta.j = point->y - volume->center.y;
	delta.k = point->z - volume->center.z;
	real distance = delta.i * delta.i;
	distance += delta.k * delta.k;
	distance += delta.j * delta.j;
	bool result = false;
	if (distance < volume->radius * volume->radius)
	{
		bool outside = false;
		for (long i = 0; i < volume->count; ++i)
		{
			plane3f const *plane = &volume->planes[i];
			real distance_to_plane = point->x * plane->i + plane->j * point->y + plane->k * point->z - plane->d;
			if (distance_to_plane > k_real_epsilon)
			{
				outside = true;
				break;
			}
		}
		result = !outside;
	}
	return result;
}

// @retail 0x1c1a0
double __cdecl function_1c1a0(real value)
{
	return fabs(value);
}

// @retail 0x1d6d0
double __stdcall function_1d6d0(real value)
{
	return ceil(value);
}

// @retail 0x2b460
double __cdecl function_2b460(real value)
{
	return floor(value);
}

// @retail 0x2b480
double __stdcall function_2b480(real value)
{
	return floor(value);
}

// @retail 0x2b400
real normalize2d(point2f *v)
{
	real m = (real)sqrt(v->x * v->x + v->y * v->y);
	if (!(fabs(m) < k_real_epsilon))
	{
		real inv = 1.f / m;
		v->x = v->x * inv;
		v->y = inv * v->y;
		return m;
	}
	return 0.f;
}

// @retail 0x4efb0
vector3f *function_4efb0(vector3f *v)
{
	real length_squared = v->i * v->i + v->j * v->j + v->k * v->k;
	if (length_squared != 0.0f)
	{
		real inverse = (real)(1.0f / sqrt(length_squared));
		v->i = (real)(v->i * inverse);
		v->j = (real)(v->j * inverse);
		v->k = (real)(v->k * inverse);
	}
	return v;
}

// @retail 0x461c0
long function_461c0(point3f const *a, point3f const *b, real radius)
{
	/* Retail receives the radius on the stack. */
	real const *radius_reference = &radius;
	vector3f delta;
	vector3d_from_points3d(a, b, &delta);
	real distance_squared = delta.i * delta.i;
	distance_squared += delta.k * delta.k;
	distance_squared += delta.j * delta.j;
	if (distance_squared <= *radius_reference * *radius_reference)
		return 1;
	return 0;
}

struct s_grid_pair
{
	short x;
	short y;
};

// @retail 0x2f600
void function_2f600(long count, long mode, s_grid_pair *out)
{
	long x = 1;
	long y = 1;
	bool narrow = mode == 1;
	while (x * y < count)
	{
		bool grow;
		if (narrow)
			grow = x < y;
		else
			grow = x <= y;
		if (grow)
			++x;
		else
		{
			x = 1;
			++y;
		}
	}
	out->x = (short)x;
	out->y = (short)y;
}

// @retail 0x2f640
void function_2f640(long index, s_grid_pair *extent, s_grid_pair const *dimensions,
	long count, long mode, s_grid_pair *out)
{
	extent->x = extent->y = 1;
	long width = dimensions->x;
	long area = width * dimensions->y;
	long offset = 0;
	if (count < area)
	{
		if (mode == 1)
		{
			if (!index)
				extent->x = 2;
			else
				offset = 1;
		}
		else if (!index)
			extent->y = 2;
		else
			offset = index / width;
	}
	long position = index + offset;
	out->x = (short)(position % dimensions->x);
	out->y = (short)(position / dimensions->x);
}
