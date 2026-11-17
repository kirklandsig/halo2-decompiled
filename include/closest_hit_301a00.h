#ifndef CLOSEST_HIT_301A00_H
#define CLOSEST_HIT_301A00_H
#include <xmmintrin.h>

struct s_301a00_hit
{
	__m128 position;
	__m128 normal;
	float distance() const { return position.m128_f32[3]; }
};

// Shared view of the collector whose operation keeps the nearest hit.
class closest_interface_301a00
{
public:
 virtual ~closest_interface_301a00() {}
 virtual void add_hit(s_301a00_hit const *) = 0;
 float early_out;
};
class class_301a00 : public closest_interface_301a00
{
public:
	bool has_hit;
	s_301a00_hit hit;

	virtual void add_hit(const s_301a00_hit *candidate);
};

typedef char assert_closest_prefix_301a00[(sizeof(closest_interface_301a00) == 8) ? 1 : -1];
#endif
