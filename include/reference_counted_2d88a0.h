#ifndef REFERENCE_COUNTED_2D88A0_H
#define REFERENCE_COUNTED_2D88A0_H
// Havok (third-party physics library): the allocator interface it reaches
// through g_480118, and its reference-counted object base: a vtable, the
// block's size and a reference count.
#include "globals.h"

class memory_2d88a0
{
public:
	virtual void *allocate(long size, long tag) { return 0; }
	virtual void release(void *block) {}
	virtual void *allocate_aligned(long alignment, long size, long tag) { return 0; }
	virtual void release_aligned(void *block) {}
	virtual void *allocate_block(long size, long tag) { return 0; }
	virtual void release_block(void *block, long size, long tag) {}
	virtual void slot6() {}
	virtual void slot7() {}
	virtual void slot8() {}
	virtual void *allocate_stack_overflow(long size) { return 0; }
	virtual void release_stack_overflow(void *block, long size) {}

	long field_4;
	char *stack_cursor;
	long stack_free;
	long field_10;
	void *field_14;

	// a block from the top of the stack area, or from slot 9 when the area
	// is too small
	void *allocate_stack(long size)
	{
		if (size > stack_free)
		{
			return allocate_stack_overflow(size);
		}
		char *block = stack_cursor;
		stack_cursor += size;
		stack_free -= size;
		return block;
	}

	void release_stack(void *block, long size)
	{
		if (size != 0 && (block == field_14 || field_14 == 0))
		{
			release_stack_overflow(block, size);
		}
		else
		{
			stack_cursor -= size;
			stack_free += size;
		}
	}
};

#define g_memory_2d88a0 ((memory_2d88a0 *)g_480118)

class referenced_2d88a0
{
public:
	// The contact manager preserves this otherwise unused allocation word.
	struct preserve_reference {};
	referenced_2d88a0(preserve_reference) {}
	unsigned short memory_size;
	unsigned short reference_count;

	referenced_2d88a0()
	{
		reference_count = 1;
	}
	virtual ~referenced_2d88a0() {}
};

// a class's own operator new: a block from the allocator with the class's tag,
// its size recorded in the object
#define REFERENCED_NEW_2D88A0(tag) \
	static void *operator new(unsigned int size) \
	{ \
		referenced_2d88a0 *object = (referenced_2d88a0 *)g_memory_2d88a0->allocate_block(size, tag); \
		object->memory_size = (unsigned short)size; \
		return object; \
	} \
	static void operator delete(void *object) \
	{ \
		g_memory_2d88a0->release_block(object, ((referenced_2d88a0 *)object)->memory_size, tag); \
	}

#endif
