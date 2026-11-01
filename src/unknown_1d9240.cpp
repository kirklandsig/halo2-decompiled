// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_1D9240.CPP */

#include "unknown_11c920.h"
#include "slot_handler.h"
#include "globals.h"
#include "unknown_0259d0.h"

struct s_1d9240
{
	char value;
	char count;
	char unknown02;
	byte flag;
};
#pragma optimize("p", on)
PRIVATE __forceinline real function_1d9241(real arg_0, real arg_1)
{
 return arg_0 / arg_1;
}
#pragma optimize("", on)


static __forceinline long real_to_long(real value);

// @retail 0x1d9240
void function_1d9240(s_1d9240 *p, char flag, real x)
{
	long value;
	long n;
	long q;

	x = g_510c54->field_2_3 * x;
	q = real_to_long(x);
	n = q;

	if (n < 1)
		n = 1;
	else if (n > 0x7f)
		n = 0x7f;
	n--;

	if (n > 0)
	{
		if (p->count)
		{
			long forward;

			if (p->flag & 1)
				forward = 1;
			else
				forward = 0;
			if (flag == (char)forward && n == p->count)
				return;
		}

		if (p->count)
		{
			real denominator = (real)p->count;

			if (!(denominator > 1.0f))
				denominator = 1.0f;
			x = function_1d9241((real)p->value, denominator) * (real)(n - 1);
			value = real_truncate(x);
		}
		else
			value = flag ? 1 : n - 1;

		p->count = (char)n;
		p->value = (char)value;
		p->flag = flag != 0;
	}
	else
	{
		p->count = 0;
		p->value = 0;
		p->flag = 0;
	}
}

/* advances the counter one tick towards its end (up when flag bit 0 is set,
   down otherwise); false once it has finished */
// @retail 0x1d9320
bool function_1d9320(s_1d9240 *p)
{
	bool result = false;

	if (p->count && !(p->flag & 2))
	{
		if (p->flag & 1)
		{
			p->value++;
			if (p->value >= p->count)
			{
				p->flag |= 2;
				p->value = p->count;
			}
		}
		else
		{
			p->value--;
			if (p->value <= 0)
			{
				p->flag |= 2;
				p->value = 0;
			}
		}
		result = true;
	}
	return result;
}

/* the seconds the counter has left to run */
// @retail 0x1d9430
real function_1d9430(s_1d9240 const *p)
{
	long local_0 = 0;
	real result = 0.0f;

	if (p->count)
	{
		if (p->flag & 1)
		{
			local_0 = p->count - p->value;
		}
		else
		{
			local_0 = p->value;
		}
		result = (real)local_0 * g_510c54->rate;
	}
	return result;
}

#define PIN(value, lower, upper) ((value) < (lower) ? (lower) : (value) > (upper) ? (upper) : (value))

/* 6t^5 - 15t^4 + 10t^3, clamped to [0, 1] */
__declspec(noinline) inline real function_1d9670(real t);

// @retail 0x1d9670
inline real function_1d9670(real t)
{
	real t3 = t * t * t;
	real t4 = t3 * t;
	real result = t4 * t * 6.0f - t4 * 15.0f + t3 * 10.0f;

	return PIN(result, 0.0f, 1.0f);
}

real function_10e8e0(real t);

/* the counter's position through its curve: value / count shaped by the
   counter's curve type, clamped to [0, 1] */
// @retail 0x1d9370
real function_1d9370(s_1d9240 const *p)
{
	real result = 0.0f;
	if (p->count)
	{
		real count = (real)p->count;

		if (!(count > 1.0f))
		{
			count = 1.0f;
		}
		result = (real)p->value / count;
		switch (p->unknown02)
		{
		case 1:
			result = function_10e8e0(result);
			break;
		case 2:
			result = function_1d9670(result);
			break;
		case 3:
			result -= 1.0f;
			result = 1.0f - result * result;
			break;
		case 4:
		{
			result = 1.0f - (1.0f - result) * (1.0f - result) * (1.0f - result);
			break;
		}
		}
		if (0.0f > result)
			result = 0.0f;
		else if (result > 1.0f)
			result = 1.0f;
	}
	return result;
}

/* a node's orientation: a quaternion, a translation and a scale */
struct s_blend_orientation
{
	real quaternion[4];
	real translation[3];
	real scale;
};

/* blends the nodes in the mask (all of them without one) from the targets
   towards the orientations by the counter's position, renormalizing the
   quaternions */
// @retail 0x1d9470
void function_1d9470(s_1d9240 const *p, long count, dword const *mask, s_blend_orientation const *targets, s_blend_orientation *orientations)
{
	if (p->count)
	{
		real t = function_1d9370(p);
		short i;

		for (i = 0; i < count; targets++, orientations++, i++)
		{
			if (!mask || (mask[i >> 5] & (1 << (i & 0x1f))))
			{
				real first_product = orientations->quaternion[0] * targets->quaternion[0];
				real dot = first_product +
					targets->quaternion[1] * orientations->quaternion[1] +
					targets->quaternion[2] * orientations->quaternion[2] +
					orientations->quaternion[3] * targets->quaternion[3];
				real weight = t;
				real target_weight = 1.0f - t;
				real length_squared;

				if (0.0f > dot)
				{
					weight = 0.0f - t;
				}
				orientations->quaternion[0] = targets->quaternion[0] * target_weight + orientations->quaternion[0] * weight;
				orientations->quaternion[1] = orientations->quaternion[1] * weight + target_weight * targets->quaternion[1];
				orientations->quaternion[2] = orientations->quaternion[2] * weight + target_weight * targets->quaternion[2];
				orientations->quaternion[3] = targets->quaternion[3] * target_weight + orientations->quaternion[3] * weight;
				length_squared = orientations->quaternion[0] * orientations->quaternion[0] +
					orientations->quaternion[1] * orientations->quaternion[1] +
					orientations->quaternion[2] * orientations->quaternion[2] +
					orientations->quaternion[3] * orientations->quaternion[3];
				if (length_squared > 0.0f)
				{
					real inverse = (real)(1.0f / sqrt(length_squared));

					orientations->quaternion[0] *= inverse;
					orientations->quaternion[1] *= inverse;
					orientations->quaternion[2] *= inverse;
					orientations->quaternion[3] *= inverse;
				}
				else
				{
					real zero = 0.0f;
					orientations->quaternion[0] = zero;
					orientations->quaternion[1] = zero;
					orientations->quaternion[2] = zero;
					orientations->quaternion[3] = zero + 1.0f;
				}
				orientations->translation[0] = (orientations->translation[0] - targets->translation[0]) * t + targets->translation[0];
				orientations->translation[1] = (orientations->translation[1] - targets->translation[1]) * t + targets->translation[1];
				orientations->translation[2] = (orientations->translation[2] - targets->translation[2]) * t + targets->translation[2];
				orientations->scale = (orientations->scale - targets->scale) * t + targets->scale;
			}
		}
	}
}
