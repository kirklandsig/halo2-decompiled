// @flags /O2 /GL-
#include <math.h>

// Quadrant-aware cubic approximation; coefficients and branch order come
// from retail, rather than replacing it with the CRT's atan2 implementation.
// @retail 0x3725e0
float __cdecl approximate_angle_3725e0(float y, float x)
{
	float ay = (float)fabs(y);
	float ax = (float)fabs(x);
	float ratio, angle;
	if (ay <= ax)
	{
		ratio = ay / (ax + 1.1920928955078125e-7f);
		angle = (ratio - 0.12107899785041809f * ratio * ratio)
			- ratio * ratio * ratio * 0.0935228168964386f;
	}
	else
	{
		ratio = ax / (ay + 1.1920928955078125e-7f);
		angle = 1.5707963705062866f - ((ratio - 0.12107899785041809f * ratio * ratio)
			- ratio * ratio * ratio * 0.0935228168964386f);
	}
	if (x < 0.0f)
		angle = 3.1415927410125732f - angle;
	if (y < 0.0f)
		angle = -angle;
	return angle;
}
