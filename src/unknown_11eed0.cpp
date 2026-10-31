// @flags /O2 /Gr /arch:SSE
/* UNKNOWN_11EED0.CPP: motion smoothing and quantization helpers */

#include "unknown_11c920.h"
#include "globals.h"
#include <math.h>

real function_30bf0(vector3f *v);

// @retail 0x11eed0
bool function_11eed0(
	real *position,
	real *velocity,
	real target,
	real time_step,
	real speed_limit,
	real acceleration_limit,
	bool wrap,
	real lo,
	real hi)
{
	real v0 = *velocity;
	real p0 = *position;
	real speed_limit_squared = speed_limit * speed_limit;
	real distance = target - p0;
	bool reached = false;
	real limit;
	real accel;
	real newv;
	real newp;

	if (wrap)
	{
		real half = (hi - lo) * 0.5f;
		if (distance > half)
			distance -= half * 2.f;
		else if (0.f - half > distance)
			distance = half * 2.f + distance;
	}

	real local_0 = time_step * time_step * acceleration_limit;
	limit = time_step * speed_limit;
	if (!(local_0 > limit))
		limit = local_0;

	if (limit >= (real)fabs(distance - v0 * time_step))
	{
		newv = 0.f;
		newp = target;
		if (lo > newp)
			newp = lo;
		else if (newp > hi)
			newp = hi;
		reached = true;
	}
	else
	{
		double x = fabs(distance) * acceleration_limit;
		x = x + x;
		real speed = x >= speed_limit_squared ? speed_limit : (real)sqrt(x);
		if (0.f > distance)
			speed = 0.f - speed;

		accel = (speed - v0) / time_step;
		if ((real)fabs(accel) > acceleration_limit)
		{
			long s = accel != 0.f ? (0.f > accel ? -1 : 1) : 0;
			accel = (real)s * acceleration_limit;
		}

		{
			real dv = accel * time_step;
			newv = dv + v0;
			newp = (dv * 0.5f + newv) * time_step + p0;
		}
		if (wrap)
		{
			if (lo > newp)
				newp = hi - lo + newp;
			else if (newp > hi)
				newp = newp - (hi - lo);
		}
		if (lo > newp)
			newp = lo;
		else if (newp > hi)
			newp = hi;
	}
	*velocity = newv;
	*position = newp;
	return reached;
}

real function_50650(real cosine);

// score 2 (was 2): one product of the cross product has its factors in the opposite order:
// ours keeps the forward vector's k field in the register, retail the target's i field.
// Round 100 (lane 7): the clamped arccosine through the existing 0x50650 2 (kept; LTCG inlines it as
// retail does). All 64 operand orders of the cross compile alike; of 144 one-expression and 72
// split-statement dot products only the i-and-j-first groupings flip that product, and they change
// the dot itself (29-84). The named `double s` makes this function's LTCG callers keep 8-byte stack
// parity (0x1197b0's early ESI save); retail has none: its code here (float quotient, then a double
// multiply per component that keeps the quotient on the x87 stack) is also what a double parameter
// of an inlined helper gives, without that effect (0x1197b0 73, this function 2); `real s` or any
// float or cast form folds the last multiply (89).
// @retail 0x11f0d0
void function_11f0d0(
	vector3f *position,
	vector3f *forward,
	vector3f const *target,
	real rate,
	real max_angle,
	real scale)
{
	vector3f axis;
	vector3f diff;
	real d;
	double angle;
	real step;
	real dist2;
	real theta;

	if (0.f >= scale && 0.f >= max_angle)
	{
		*position = *g_4687a4;
		*forward = *target;
		return;
	}

	d = forward->i * target->i + (target->j * forward->j + target->k * forward->k);
	d = d < -1.f ? -1.f : d > 1.f ? 1.f : d;
	angle = function_50650(d);
	angle = angle * scale;
	angle = angle + angle;
	real limit = angle >= max_angle * max_angle ? max_angle : (real)sqrt(angle);

	axis.i = target->k * forward->j - forward->k * target->j;
	axis.j = target->i * forward->k - target->k * forward->i;
	axis.k = forward->i * target->j - target->i * forward->j;
	function_30bf0(&axis);

	axis.i = axis.i * limit;
	axis.j = axis.j * limit;
	axis.k = axis.k * limit;
	diff.i = axis.i - position->i;
	diff.j = axis.j - position->j;
	diff.k = axis.k - position->k;
	dist2 = diff.k * diff.k + diff.j * diff.j + diff.i * diff.i;
	step = rate * scale;

	if (step * step * 5.f > dist2)
	{
		if (step > limit)
		{
			*position = *g_4687a4;
			*forward = *target;
			return;
		}
		*position = axis;
	}
	else
	{
		double s = step / sqrt(dist2);
		position->i = diff.i * s + position->i;
		position->j = diff.j * s + position->j;
		position->k = diff.k * s + position->k;
	}

	vector3f rotation_axis = *position;
	theta = function_30bf0(&rotation_axis) * rate;
	if (theta != 0.f)
	{
		real s = (real)sin(theta);
		real c = (real)cos(theta);
		real dot = (forward->i * rotation_axis.i +
			(forward->j * rotation_axis.j + forward->k * rotation_axis.k)) * (1.f - c);
		real cx = forward->k * rotation_axis.i - forward->i * rotation_axis.k;
		real cy = forward->i * rotation_axis.j - forward->j * rotation_axis.i;
		real cz = forward->j * rotation_axis.k - forward->k * rotation_axis.j;
		forward->i = forward->i * c + rotation_axis.i * dot - cz * s;
		forward->j = forward->j * c + rotation_axis.j * dot - cx * s;
		forward->k = forward->k * c + rotation_axis.k * dot - cy * s;
		function_30bf0(forward);
	}
}

// @retail 0x11f470
byte function_11f470(real lo, real hi, real value)
{
	real range = hi - lo;
	byte result = 0;

	if (range > 0.0001f)
	{
		result = (byte)((value - lo) / range * 255.f);
		while (result > 0)
		{
			real v;
			if (result == 255)
				v = hi;
			else
				v = range * ((real)result * (1.f / 255.f)) + lo;
			if (!(v > value))
				break;
			result--;
		}
		return result;
	}
	return 0;
}

// @retail 0x11f4f0
void function_11f4f0(
	real const *in,
	real_bounds const *ranges,
	long bits,
	long *out)
{
	long i = 0;

	do
	{
		real v;
		long q;
		real lo;
		real hi;

		if (ranges[i].lo > in[i])
			v = ranges[i].lo;
		else if (in[i] > ranges[i].hi)
			v = ranges[i].hi;
		else
			v = in[i];
		lo = ranges[i].lo;
		hi = ranges[i].hi;
		v = (v - lo) / ((hi - lo) / (real)((1 << bits) - 1));
		__asm
		{
			fld v
			fistp q
		}
		out[i] = q;
		i++;
	}
	while (i < 3);
}

// @retail 0x11f580
void function_11f580(
	real const *ranges,
	long bits,
	real *out,
	long const *in)
{
	long i = 0;
	long max = (1 << bits) - 1;

	do
	{
		long q = in[i];
		real hi = ranges[i * 2 + 1];
		real lo = ranges[i * 2];
		real v;

		if (q == 0)
			v = lo;
		else if (q >= max)
			v = hi;
		else
			v = ((real)(max - q) * lo + (real)q * hi) / (real)max;
		out[i] = v;
		i++;
	}
	while (i < 3);
}


// @retail 0x11f5f0
void function_11f5f0(
	box2f *bounds,
	point2f const *points,
	long count)
{
	long i;

	for (i = 0; i < count; i++)
	{
		point2f const *p = points + i;
		if (bounds->x0 > p->x)
			bounds->x0 = p->x;
		if (p->x > bounds->x1)
			bounds->x1 = p->x;
		if (bounds->y0 > p->y)
			bounds->y0 = p->y;
		if (p->y > bounds->y1)
			bounds->y1 = p->y;
	}
}
