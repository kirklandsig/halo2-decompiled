#ifndef BOUNDED_2E9530_H
#define BOUNDED_2E9530_H
// Havok (third-party physics library): an object holding a value, an array of
// 12-byte elements and two vectors that start at the largest float, and a
// kind derived from it.
#include "reference_counted_2d88a0.h"
#include <xmmintrin.h>

struct element_2e9530
{
	long value_0;
	long value_4;
	long value_8;
};

struct array_2e9530
{
	element_2e9530 *data;
	long size;
	unsigned long capacity_and_flags;

	array_2e9530() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~array_2e9530()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(element_2e9530), 0x12);
		}
	}
};

// an array of the same elements whose storage comes from the allocator's
// stack area, given back when it goes
struct stack_elements_2e93e0 : array_2e9530
{
	element_2e9530 *stack_block;
	long stack_capacity;

	stack_elements_2e93e0(long capacity);
	~stack_elements_2e93e0();
};

// orders keys from low to high
struct key_order_2e9360
{
};

// sorts the keys from low to high (both included)
void __cdecl function_2e9360(unsigned long *keys, int low, int high, key_order_2e9360 order);

class counted_2e9530
{
public:
	counted_2e9530(long value_) : value(value_) {}
	virtual ~counted_2e9530() {}
	unsigned short memory_size;
	unsigned short reference_count;
	long value;
};

struct typed_2e9210;
struct description_2e9210;
struct context_2e9210;

class bounded_2e9530 : public counted_2e9530
{
public:
	REFERENCED_NEW_2D88A0(0x1a)

	array_2e9530 elements;
	__m128 minimum;
	__m128 maximum;

	bounded_2e9530(long value_);
	~bounded_2e9530();
	// Releases each kept child, then destroys this object through its table.
	void function_2e90b0();

	// brings the objects kept for the shape's children up to date, with the
	// two sides the other way round (0x2e9210)
	void function_2e9210(typed_2e9210 **other, description_2e9210 const *description, context_2e9210 const *context);
	// the same with the two sides given the usual way round (0x2e9680)
	void function_2e9680(description_2e9210 const *description, typed_2e9210 **other, context_2e9210 const *context);
	// the same for a shape that wraps the wrapper, both ways round (0x2f9e40,
	// 0x2fac30)
	void function_2f9e40(description_2e9210 const *description, typed_2e9210 **other, context_2e9210 const *context);
	void function_2fac30(typed_2e9210 **other, description_2e9210 const *description, context_2e9210 const *context);
};

class derived_2eb4e0 : public bounded_2e9530
{
public:
	derived_2eb4e0(long value_) : bounded_2e9530(value_) {}
};

#endif
