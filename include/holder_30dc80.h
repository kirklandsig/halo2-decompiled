#ifndef HOLDER_30DC80_H
#define HOLDER_30DC80_H
// Havok (third-party physics library): a handler the dispatcher tells of the
// pairs it runs, which notes the groups those pairs reach in an array with
// room for four on the object itself.
#include "reference_counted_2d88a0.h"
#include "pair_recorder_30dad0.h"

// the handler's interface: a table only (its destructor is 0x30c0e0's)
class pair_handler_30dc80
{
public:
	virtual ~pair_handler_30dc80() {}

	// gives back the size of the interface alone, whatever the object's
	static void operator delete(void *block)
	{
		g_memory_2d88a0->release_block(block, sizeof(pair_handler_30dc80), 0x1c);
	}
	virtual void slot_1(void const *pair) = 0;
	virtual void slot_2(void const *pair) = 0;
};

struct group_30dc80;

struct values_30dc80
{
	group_30dc80 **data;
	long size;
	unsigned long capacity_and_flags;

	~values_30dc80()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(group_30dc80 *), 0x12);
		}
	}
};

class holder_30dc80 : public pair_handler_30dc80
{
public:
	void *body;
	values_30dc80 values;
	group_30dc80 *storage[4];

	holder_30dc80(void *body_)
	{
		values.data = storage;
		values.size = 0;
		values.capacity_and_flags = 0x80000004;
		body = body_;
	}
	// the pairs a dispatcher adds and removes (0x30dad0, 0x30dba0)
	virtual void slot_1(void const *pair)
	{
		((class_30dad0 *)this)->function_30dad0((pair_30dc30 const *)pair);
	}
	virtual void slot_2(void const *pair)
	{
		((class_30dad0 *)this)->function_30dba0((pair_30dc30 const *)pair);
	}
};

#endif
