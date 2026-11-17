#ifndef AXIS_SWEEP_2E1090_H
#define AXIS_SWEEP_2E1090_H
// Havok (third-party physics library): a three-axis sweep of boxes. Each
// box's corners are quantized to 16 bits; each axis keeps the boxes'
// endpoints sorted, and each box records where its endpoints sit. An
// optional grid of cells, each standing for a box, holds the boxes inside it.
// (src/unknown_2e1020.cpp, src/unknown_2e24b0.cpp, which also removes a box)
#include <xmmintrin.h>
#include "growable_array_2d91d0.h"

// a box: where its endpoints sit on the y and z axes (lower y, lower z,
// upper y, upper z) and on the x axis, and its owner, whose low bit set
// means the owner is a grid cell (the rest its byte offset among the cells)
struct element_2e1090
{
	unsigned short slots[6];
	long owner;
};

ELEMENTS_2D91D0_ALIGNED(elements_2e1090, element_2e1090);

// one end of a box's interval on an axis: its value, whose low bit says
// whether it is the upper end, and the box's index (src/unknown_2e0f80.cpp)
struct endpoint_2e0f80
{
	unsigned short value;
	unsigned short id;
};

struct endpoints_2e0f80
{
	ELEMENTS_2D91D0_MEMBERS(endpoint_2e0f80)

	void function_2e0f80(long unused, long id, unsigned short low, unsigned short high,
		unsigned short *first_slot, unsigned short *second_slot);
};

// the first entry of a sorted range not below a key (src/unknown_2dfec0.cpp)
struct entry_2dfec0
{
	unsigned short key;
	unsigned short value;
};

class sorted_2dfec0
{
public:
	entry_2dfec0 *first_not_below(entry_2dfec0 *begin, entry_2dfec0 *end, unsigned short key) const;
};

// a grid cell: the box standing for it and the boxes it holds
ELEMENTS_2D91D0(shorts_2e1090, unsigned short);

struct cell_2e1090
{
	unsigned short box;
	shorts_2e1090 boxes;
};

// an overlapping pair: the first box's owner (or none) and the other's
struct pair_2e1090
{
	long *first;
	long second;
};

ELEMENTS_2D91D0(pairs_2e1090, pair_2e1090);

// a box given by its two corners
struct bounds_2e0020
{
	__m128 lower;
	__m128 upper;
};

ELEMENTS_2D91D0_ALIGNED(bounds_array_2e1020, bounds_2e0020);

// three lists of endpoints and their storage (src/unknown_2e3370.cpp)
struct overlap_endpoints_2e3370;

// a box swept along a segment, and what takes the boxes it reaches
// (src/unknown_2e3d40.cpp)
struct ray_2e4660;
class ray_collector_2e4660;
// segments from one point (src/unknown_2e3d40.cpp)
struct rays_2e3d40;

class class_2e1090
{
public:
	char unknown0[0x10];
	// added to the lower and upper corners before scaling
	__m128 lower_offset;
	__m128 upper_offset;
	__m128 scale;
	elements_2e1090 elements;
	endpoints_2e0f80 axes[3];
	long cells_used;
	long cell_shift;
	cell_2e1090 *cells;
	float field_7c;

	void function_2e0020(element_2e1090 const *element, bounds_2e0020 *bounds);
	void function_2e1020(bounds_array_2e1020 *out);
	void function_2e1090(long const *index, bounds_2e0020 *bounds);
	void function_2e10b0(bounds_2e0020 const *bounds, pairs_2e1090 *pairs);
	void function_2e24b0(long *owner, bounds_2e0020 const *bounds, pairs_2e1090 *pairs);
	void function_2e29b0(long *owner, pairs_2e1090 *pairs);
	void function_2e3370(bounds_2e0020 const *bounds, overlap_endpoints_2e3370 *out);
	void function_2e0350(long *const *owners, bounds_2e0020 const *bounds, long count, pairs_2e1090 *added, pairs_2e1090 *removed);
	void function_2e3b00();
	void function_2e3d40(rays_2e3d40 const *rays, ray_collector_2e4660 *collectors, long collector_stride);
	void function_2e4660(ray_2e4660 const *ray, ray_collector_2e4660 *collector);
};

// the quantizing constants: the corner words' ceiling and floor, and the
// bias that puts a value's integer part in its float's low bits
static __declspec(align(16)) const float g_4125c0[4] = {65536.0f, 65536.0f, 65536.0f, 65536.0f};
static __declspec(align(16)) const float g_4125d0[4] = {65532.0f, 65532.0f, 65532.0f, 65532.0f};
static __declspec(align(16)) const float g_4125e0[4] = {0.0f, 0.0f, 0.0f, 0.0f};

inline void toggle_bit_2e1090(long *bits, long id)
{
	bits[id >> 5] ^= 1 << (id & 31);
}

inline void clear_bit_2e1090(long *bits, long id)
{
	bits[id >> 5] &= ~(1 << (id & 31));
}

#endif
