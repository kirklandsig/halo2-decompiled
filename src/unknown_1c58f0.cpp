// @flags /O2 /arch:SSE /Gr /GL-
// The constructor is inline: Havok's capsule cast (0x2de1b0,
// src/unknown_2dddf0.cpp) sees its body through include/unknown_1c58f0.h.
#ifndef UNKNOWN_1C58F0_INLINE
#define UNKNOWN_1C58F0_INLINE
#include "unknown_1c58f0.h"

// @retail 0x1c58f0
inline s_1c58f0::s_1c58f0(real arg_0)
{
	field_0 = _mm_set_ss(arg_0);
}
#endif
