#ifndef SLOT_TABLE_372A40_H
#define SLOT_TABLE_372A40_H
// Havok (third-party physics library): a table of 64 slots that all start out
// pointing at the empty object it keeps after them (0x372a40 builds it); its
// blocks come from the allocator with tag 0x22.
#include "holder_30dc80.h"

// the empty object kept after the slots
struct empty_slot_372a40 : pair_handler_30dc80
{
	virtual void slot_1(void const *) {}
	virtual void slot_2(void const *) {}
};

class slot_table_372a40
{
public:
	void *slots[8][8];
	empty_slot_372a40 empty;

	static void *operator new(unsigned int size)
	{
		return g_memory_2d88a0->allocate_block(size, 0x22);
	}
	static void operator delete(void *block, unsigned int size)
	{
		g_memory_2d88a0->release_block(block, size, 0x22);
	}

	slot_table_372a40();
	~slot_table_372a40();
	void store_slot_372a40(int index, void *value)
	{
		slots[index >> 3][index & 7] = value;
	}
};

typedef slot_table_372a40 helper_3728b0;
#endif
