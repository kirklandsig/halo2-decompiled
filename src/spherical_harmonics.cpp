// @flags /O2 /Gr /arch:SSE
/* SPHERICAL_HARMONICS.CPP: real spherical harmonics up to the fourth order
   (16 coefficients): the basis evaluated in a direction, and a directional
   light projected onto it */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <math.h>

#define k_spherical_harmonics_pi 3.14159265f

/* the basis functions of the first order*order bands in a direction */
// @retail 0x143360
void spherical_harmonics_evaluate_direction(vector3f const *direction, dword order, real *result)
{
	real x = direction->i;
	real y = direction->j;
	real z = direction->k;
	real inverse_sqrt_pi = (real)(1.0f / sqrt(k_spherical_harmonics_pi));
	real sqrt3 = (real)sqrt(3.0);
	real band1 = sqrt3 * inverse_sqrt_pi;

	result[0] = inverse_sqrt_pi * 0.5f;
	result[1] = band1 * y * -0.5f;
	result[2] = band1 * z * 0.5f;
	result[3] = band1 * x * -0.5f;
	if (order > 2)
	{
		real sqrt15 = sqrtf(15.0f);
		real band2 = sqrt15 * inverse_sqrt_pi;
		real xy = y * x;
		real xx = x * x;
		real yy = y * y;
		real xx_yy = xx - yy;
		real zz = z * z;

		result[4] = xy * band2 * 0.5f;
		result[5] = band2 * z * y * -0.5f;
		real sqrt5 = (real)sqrt(5.0);
		result[6] = sqrt5 * (zz * 3.0f - 1.0f) * inverse_sqrt_pi * 0.25f;
		real quadratic_xz = z * x;
		quadratic_xz *= band2;
		result[7] = quadratic_xz * -0.5f;
		result[8] = xx_yy * sqrt15 * inverse_sqrt_pi * 0.25f;
		if (order > 3)
		{
			real sqrt2 = (real)sqrt(2.0);
			real sqrt35 = (real)sqrt(35.0);
			real band3_35 = sqrt35 * sqrt2;
			real zz5 = zz * 5.0f;
			real sqrt105 = (real)sqrt(105.0);
			real sqrt21 = (real)sqrt(21.0);
			real band3_21 = sqrt21 * sqrt2;
			result[9] = (xx * 3.0f - yy) * band3_35 * inverse_sqrt_pi * y * -0.125f;
			result[10] = xy * sqrt105 * inverse_sqrt_pi * z * 0.5f;
			real sqrt7 = (real)sqrt(7.0);
			result[12] = sqrt7 * (zz5 - 3.0f) * inverse_sqrt_pi * z * 0.25f;
			real xx_3yy = xx - yy * 3.0f;
			result[11] = (zz5 - 1.0f) * band3_21 * inverse_sqrt_pi * y * -0.125f;
			real cubic_x = inverse_sqrt_pi * x;
			cubic_x *= zz5 - 1.0f;
			cubic_x *= band3_21;
			result[13] = cubic_x * -0.125f;
			real cubic_difference = inverse_sqrt_pi * z;
			cubic_difference *= sqrt105;
			cubic_difference *= xx_yy;
			result[14] = cubic_difference * 0.25f;
			result[15] = xx_3yy * band3_35 * inverse_sqrt_pi * x * -0.125f;
		}
	}
}

/* a directional light's colour in the basis of the first order*order bands */
// @retail 0x146020
bool spherical_harmonics_evaluate_directional_light(vector3f const *direction, dword order, real red, real green, real blue,
	real *red_result, real *green_result, real *blue_result)
{
	real coefficients[16];
	real normalization;
	real scale;
	dword count;
	dword i;

	spherical_harmonics_evaluate_direction(direction, order, coefficients);
	normalization = 0.75f;
	if (order > 2)
	{
		normalization = 1.0625f;
	}
	normalization = 3.141593f / normalization;
	count = order * order;

	scale = normalization * red;
	for (i = 0; i < count; i++)
	{
		red_result[i] = coefficients[i] * scale;
	}
	if (green_result)
	{
		scale = normalization * green;
		for (i = 0; i < count; i++)
		{
			green_result[i] = coefficients[i] * scale;
		}
	}
	if (blue_result)
	{
		scale = normalization * blue;
		for (i = 0; i < count; i++)
		{
			blue_result[i] = coefficients[i] * scale;
		}
	}
	return true;
}
