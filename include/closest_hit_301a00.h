#ifndef CLOSEST_HIT_301A00_H
#define CLOSEST_HIT_301A00_H
#include <xmmintrin.h>

struct s_301a00_hit
{
	__m128 position;
	__m128 normal;
};

// Shared view of the collector whose operation keeps the nearest hit.
class class_301a00
{
public:
	void *vtable;
	float early_out;
	bool has_hit;
	s_301a00_hit hit;

	void add_hit(const s_301a00_hit *candidate);
};

#endif
