// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_02B400.CPP: 2d vector math */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <math.h>

#define k_real_epsilon 0.0001f

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
