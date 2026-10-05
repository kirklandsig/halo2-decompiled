// @flags /O2 /Gr /GL-
/* UNKNOWN_0259A0.CPP: the out of line copy of function_x82e52f (random_number_math)
   that some callers keep: a real in [0, 1] drawn from the seed */

#include "unknown_11c920.h"

// @retail 0x259a0
real function_259a0(dword *seed)
{
	*seed = 1664525 * *seed + 1013904223;
	return (real)(*seed >> 16) * (1.f / 65535.f);
}
