#ifndef KEYED_LIST_2EC680_H
#define KEYED_LIST_2EC680_H
// Havok (third-party physics library): a list of child objects, each kept
// with its key, for a shape that hands out its children by key (constructor
// 0x2ec680, refresh 0x2ec3d0).
#include <xmmintrin.h>
#include "child_list_2edd00.h"

// room for a child shape the collection builds on request
struct child_buffer_2ec680
{
	__m128 storage[16];
};

// a shape holding children it hands out by key
class collection_2ec680
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void slot_6() {}
	virtual void slot_7() {}
	virtual void slot_8() {}
	virtual void slot_9() {}
	virtual long child_count() const { return 0; }
	virtual long first_key() const { return 0; }
	virtual long next_key(long key) const { return 0; }
	virtual typed_2edd00 const *child(long key, child_buffer_2ec680 *buffer) const { return 0; }
};

struct keyed_child_2ec680
{
	long key;
	child_2edd00 *child;
};

// an array that starts out in the room for four children held inside it
struct keyed_children_2ec680
{
	ELEMENTS_2D91D0_MEMBERS(keyed_child_2ec680)
	keyed_child_2ec680 embedded[4];

	keyed_children_2ec680()
	{
		data = embedded;
		size = 0;
		capacity_and_flags = 0x80000004;
	}
	// a new last element, the array grown first when full
	keyed_child_2ec680 &added_slot()
	{
		if (size == (capacity_and_flags & 0x7fffffff))
		{
			function_2d91d0(this, sizeof(keyed_child_2ec680));
		}
		long const grown = size + 1;
		size = grown;
		return data[grown - 1];
	}
	keyed_child_2ec680 &last_slot()
	{
		return data[size - 1];
	}
	~keyed_children_2ec680()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(keyed_child_2ec680), 0x12);
		}
	}
};

class keyed_list_2ec680 : public sized_2edd00
{
public:
	keyed_children_2ec680 children;

	// the index of the child kept with the key, or -1 (the count read once,
	// the children reached through the list)
	long index_of_key(long key) const
	{
		long const count = children.size;
		for (long i = 0; i < count; i++)
		{
			if (children.data[i].key == key)
			{
				return i;
			}
		}
		return -1;
	}

	keyed_list_2ec680(description_2edd00 const *description, typed_2edd00 **other, context_2edd00 const *context, long arg_3);
	void function_2ec3d0(description_2edd00 const *description, typed_2edd00 **other, context_2edd00 const *context);
};

#endif
