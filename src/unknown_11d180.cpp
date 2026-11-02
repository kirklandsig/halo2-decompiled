// @flags /O2 /Ob1 /Gr /arch:SSE
/* UNKNOWN_11D180.CPP: vector and quaternion math */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_11cc90.h"
#include <math.h>
#include "vector_products.h"

real g_5476c8;
vector3f *g_4687bc;

real function_30bf0(vector3f *v);
vector3f *function_11d000(vector3f const *v, vector3f *out);

// @retail 0x11d180
void function_11d180(
	vector3f const *in,
	vector3f const *up,
	vector3f *forward,
	vector3f *left,
	vector3f *out)
{
	*forward = *in;
	if (function_30bf0(forward) == 0.f)
		*forward = *g_4687a8;
	{
		real local_0 = up->i * forward->j - up->j * forward->i;
		real local_3 = *(real const volatile *)&up->i * forward->k;
		real local_1 = *(real const volatile *)&forward->i * up->k - local_3;
		real local_2 = up->j * forward->k - up->k * forward->j;
		left->i = local_2;
		left->j = local_1;
		left->k = local_0;
	}
	if (function_30bf0(left) == 0.f)
	{
		vector3f axis;
		if ((fabs(forward->i - g_4687b0->i) < 0.0001f && fabs(forward->j - g_4687b0->j) < 0.0001f && fabs(forward->k - g_4687b0->k) < 0.0001f)
			|| (fabs(forward->i - g_4687bc->i) < 0.0001f && fabs(forward->j - g_4687bc->j) < 0.0001f && fabs(forward->k - g_4687bc->k) < 0.0001f))
			axis = *g_4687a8;
		else
			axis = *g_4687b0;
		{
			real local_0 = axis.i * forward->j - axis.j * forward->i;
			real local_1 = forward->i * axis.k - axis.i * forward->k;
			real local_2 = axis.j * forward->k - axis.k * forward->j;
			left->i = local_2;
			left->j = local_1;
			left->k = local_0;
		}
		function_30bf0(left);
	}
	{
		real local_0 = left->j * forward->i - left->i * forward->j;
		real local_1 = *(real const volatile *)&forward->k * left->i - forward->i * left->k;
		real local_2 = *(real const volatile *)&forward->j * left->k - left->j * forward->k;
		out->i = local_2;
		out->j = local_1;
		out->k = local_0;
	}
	function_30bf0(out);
}

// @retail 0x11d3b0
bool function_11d3b0(
	vector3f const *a,
	vector3f *out,
	vector3f const *b,
	real sine,
	real cosine)
{
	vector3f axis;
	if (cosine <= a->j * b->j + a->i * b->i + b->k * a->k)
	{
		*out = *a;
		return false;
	}
	axis.i = a->k * b->j - a->j * b->k;
	axis.j = a->i * b->k - b->i * a->k;
	axis.k = a->j * b->i - a->i * b->j;
	if (function_30bf0(&axis) == 0.f)
	{
		function_11d000(b, &axis);
		function_30bf0(&axis);
	}
	*out = *b;
	{
		real d = (out->k * axis.k + out->j * axis.j + out->i * axis.i) * (1.f - cosine);
		real t0 = out->j * axis.k - out->k * axis.j;
		real t1 = out->k * axis.i - out->i * axis.k;
		real t2 = out->i * axis.j - out->j * axis.i;
		out->i = out->i * cosine + d * axis.i - t0 * sine;
		out->j = out->j * cosine + d * axis.j - t1 * sine;
		out->k = out->k * cosine + d * axis.k - t2 * sine;
	}
	return true;
}

// @retail 0x11d580
void function_11d580(
	vector3f const *a,
	vector3f const *b,
	vector3f *projection,
	vector3f *rejection)
{
	vector3f local;
	real d = a->j * b->j + b->k * a->k + a->i * b->i;
	if (!projection)
		projection = &local;
	projection->i = b->i * d;
	projection->j = d * b->j;
	projection->k = b->k * d;
	if (rejection)
	{
		rejection->i = a->i - projection->i;
		rejection->j = a->j - projection->j;
		rejection->k = a->k - projection->k;
	}
}

// @retail 0x11d610
void function_11d610(quaternionf *q)
{
	real sum = q->i * q->i + q->j * q->j + q->k * q->k + q->w * q->w;
	if (sum > 0.f)
	{
		real inv = 1.f / (real)sqrt(sum);
		q->i = q->i * inv;
		q->j = q->j * inv;
		q->k = q->k * inv;
		q->w = q->w * inv;
	}
	else
	{
		q->i = 0.f;
		q->j = 0.f;
		q->k = 0.f;
		q->w = 1.f;
	}
}

// @retail 0x11d6a0
void function_11d6a0(
	quaternionf *out,
	vector3f const *a,
	vector3f const *b)
{
	real s = (dot3f(a, b) + 1.f) * 2.f;
	real w;
	if (!(s > 0.f))
		s = 0.f;
	s = (real)sqrt(s);
	if (s >= 0.001f)
	{
		real inv;
		cross3f_staged(a, b, (vector3f *)out);
		inv = 1.f / s;
		out->i *= inv;
		out->j *= inv;
		out->k *= inv;
		w = s * 0.5f;
	}
	else
	{
		function_11d000(a, (vector3f *)out);
		w = 0.f;
	}
	out->w = w;
}

// @retail 0x11d790
void function_11d790(
	quaternionf const *q,
	real *angle,
	vector3f *axis)
{
	real w;
	real m;
	real a;
	*axis = *(vector3f const *)q;
	w = q->w;
	m = function_30bf0(axis);
	a = (real)atan2(m, w) * 2.f;
	*angle = a;
	if (a > g_5476c4)
	{
		axis->i = 0.f - axis->i;
		axis->j = 0.f - axis->j;
		axis->k = 0.f - axis->k;
		*angle = g_5476c8 - *angle;
	}
}

// @retail 0x11d820
void function_11d820(
	quaternionf const *a,
	quaternionf const *b,
	quaternionf *out)
{
	quaternionf t;
	if (a == out)
	{
		t = *a;
		a = &t;
	}
	if (b == out)
	{
		t = *b;
		b = &t;
	}
	out->i = b->w * a->i + b->k * a->j + a->w * b->i - b->j * a->k;
	out->j = a->k * b->i + b->w * a->j + a->w * b->j - a->i * b->k;
	out->k = b->w * a->k + b->j * a->i + a->w * b->k - b->i * a->j;
	out->w = b->w * a->w - a->i * b->i - b->j * a->j - a->k * b->k;
}
