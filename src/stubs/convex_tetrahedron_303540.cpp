#include <xmmintrin.h>
// The convex-search reconstruction still uses this explicit boundary.
// Upstream's 0x303540 implementation is file-local in unknown_303110.cpp;
// its internal register convention is not exposed by this ordinary interface.
// Do not mark this stand-in as the now-implemented retail function.
int __cdecl convex_search_tetrahedron_boundary(__m128 const *point, __m128 const *tetrahedron, float orientation)
{
	return -1;
}
