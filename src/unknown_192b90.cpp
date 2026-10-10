#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <math.h>

// @flags /O2 /arch:SSE /Gr

/* UNKNOWN_192B90.CPP: real helpers */

real function_12aff0(real a, real b, real c, bool flag);

// @retail 0x192d70
real function_192d70(real a, real b, real c)
{
	real pre_ratio_scale = function_12aff0(b, a, c, false);
	real maximum = c;

	if (a > maximum)
		maximum = a;

	return pre_ratio_scale * (a / maximum);
}

/* the five speaker directions: front left, front right, back left, back
   right and center */
static point3f const g_444c84[5] =
{
	{ 0.7071067690849304f, 0.7071067690849304f, 0.0f },
	{ 0.7071067690849304f, -0.7071067690849304f, 0.0f },
	{ -0.7071067690849304f, 0.7071067690849304f, 0.0f },
	{ -0.7071067690849304f, -0.7071067690849304f, 0.0f },
	{ 0.7071067690849304f, 0.0f, 0.0f },
};

// @retail 0x192b90
real function_192b90(point3f const *direction, long speaker, bool linear)
{
	real attenuation = 0.0f;
	long clamped = speaker < 0 ? 0 : (speaker > 4 ? 4 : speaker);

	if (clamped == speaker)
	{
		vector3f delta;

		if (linear)
		{
			real x = (direction->x - g_444c84[speaker].x) * 0.5f;
			real y = (direction->y - g_444c84[speaker].y) * 0.5f;
			real z = (direction->z - g_444c84[speaker].z) * 0.5f;
			attenuation = (z * z + y * y) + x * x;
			if (attenuation < 0.0f)
				attenuation = 0.0f;
			else if (attenuation > 1.0f)
				attenuation = 1.0f;
		}
		else
		{
			real distance;

			delta.i = direction->x - g_444c84[speaker].x;
			delta.j = direction->y - g_444c84[speaker].y;
			delta.k = direction->z - g_444c84[speaker].z;
			distance = (real)sqrt(length_sq3f(&delta));
			attenuation = 1.0f - (real)exp((real)pow(2.0, (double)distance) * -0.2f);
		}
	}

	return 1.0f - attenuation;
}

// @retail 0x192ce0
void function_192ce0(point3f const *direction, real *gains, bool linear)
{
	real values[5];
	long i;

	for (i = 0; i < 4; i++)
		values[i] = function_192b90(direction, i, linear);

	for (i = 0; i < 3; i++)
		gains[i] = values[i];
	return (void)(gains[3] = values[3]);
}

// @retail 0x192d40
void function_192d40(point3f const *direction, real *gains, bool linear)
{
	long i;

	for (i = 0; i < 5; i++)
		gains[i] = function_192b90(direction, i, linear);
}