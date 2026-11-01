#ifndef SPHERE_2DEDA0_H
#define SPHERE_2DEDA0_H
#include <stddef.h>
#include <xmmintrin.h>

// The one-byte cast answer, returned through a hidden result pointer.
struct answer_2deda0
{
	char value;
	answer_2deda0(bool value_) : value((char)value_) {}
};

struct segment_2deda0
{
	__m128 start;
	__m128 end;
};

struct hit_2deda0
{
	__m128 normal;
	long part;
	float fraction;
};

// The public cast entry point's sphere prefix. Constructor and virtual
// interfaces keep their existing public models; this view does not emit
// a replacement vtable or redefine their classes.
class sphere_2deda0
{
public:
	char unknown0[0xc];
	float radius;

	answer_2deda0 function_2deda0(segment_2deda0 const *segment, hit_2deda0 *hit) const;
};

typedef char sphere_2deda0_size_check[sizeof(sphere_2deda0) == 0x10 ? 1 : -1];
typedef char sphere_2deda0_radius_check[offsetof(sphere_2deda0, radius) == 0xc ? 1 : -1];

#endif
