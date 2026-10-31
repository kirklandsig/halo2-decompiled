#ifndef REFERENCE_HOLDER_30B3A0_H
#define REFERENCE_HOLDER_30B3A0_H
// Havok (third-party physics library): an object holding a reference to a
// shape and an array of 8-byte elements (built from a description, 0x312590;
// destructor 0x30b3a0, deleting destructor 0x30b590).
#include "unknown_0a9f20.h"
#include "reference_counted_2d88a0.h"
#include "description_312430.h"
#include "collidable_state_30b340.h"

struct element_30b3a0
{
	long field_0;
	long field_4;
};

struct elements_30b3a0
{
	element_30b3a0 *data;
	long size;
	long capacity_and_flags;

	elements_30b3a0() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~elements_30b3a0()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			element_30b3a0 *block = data;
			g_memory_2d88a0->release_block(block, (capacity_and_flags & 0x7fffffff) * sizeof(element_30b3a0), 0x12);
		}
	}
};

// three words, the last a value given when built
struct handle_30b3a0
{
	long field_0;
	long field_4;
	long value;

	handle_30b3a0(long value_) : field_0(0), value(value_)
	{
		field_4 = 0;
	}
};

// the shape with what goes with it, the object that owns them, and the
// elements
struct collidable_30b3a0
{
	body_30b3a0 body;
	handle_30b3a0 handle;
	long field_1c;
	void *owner;
	elements_30b3a0 elements;

	collidable_30b3a0(referenced_2d88a0 *shape, long value)
		: body(shape), handle(value), field_1c(0), owner(0)
	{
	}
};

class class_30b3a0 : public c_reference_a9f20
{
public:
	// built from a description: its shape and a byte from the description,
	// the rest set by 0x3124c0
	class_30b3a0(description_312430 const *description);

	long field_8;
	collidable_30b3a0 collidable;

	virtual ~class_30b3a0();
};

#endif
