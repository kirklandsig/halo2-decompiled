#ifndef CONVEX_SEARCH_3048A0_H
#define CONVEX_SEARCH_3048A0_H
// Havok (third-party physics library): the search for the closest points of
// two convex shapes, run from a pair's search state, with its working state
// kept on the caller's stack.
#include <xmmintrin.h>

class convex_2fefd0;
struct c_query_transform;
struct pair_state_2fefd0;

// the closest point found, its distance in the fourth lane
struct closest_3048a0
{
	__m128 point;
};

class class_3048a0
{
public:
	char unknown0[0xc];
	bool field_c;
	// The triangle helper stores all four SIMD lanes at offset 0x160.
	char unknownd[0x170 - 0xd];

	// a status: nonzero passes the pair on without testing the distance
	int function_3048a0(convex_2fefd0 const *first, convex_2fefd0 const *second, c_query_transform const *transform, pair_state_2fefd0 *state, closest_3048a0 *closest);
	int function_303dd0(convex_2fefd0 const *first, convex_2fefd0 const *second, c_query_transform const *transform, pair_state_2fefd0 *state);
};

#endif
