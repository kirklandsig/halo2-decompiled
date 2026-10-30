// @flags /O2 /GL-
// Havok (third-party physics library): the constructor of the object that
// dispatches pair queries by the two shapes' types: an empty object in every
// slot of its 8x8 table of shared objects, a mask for each of 32 types, two
// empty arrays, and its 32x32 tables of handlers reset to their defaults
// (0x2e5990).
#include "reference_counted_2d88a0.h"

// the tables 0x2e5990 resets
#include "dispatcher_views_2e5990.h"

// an array that starts empty, without storage of its own
struct array_2e5fd0
{
	void *data;
	long size;
	unsigned long capacity_and_flags;

	array_2e5fd0() : data(0), size(0), capacity_and_flags(0x80000000) {}
};

// a reference-counted object that keeps the table's default entry
class dispatcher_base_2e5fd0
{
public:
	dispatcher_base_2e5fd0(void *default_entry_) : default_entry(default_entry_)
	{
		reference_count = 1;
	}
	virtual ~dispatcher_base_2e5fd0() {}

	unsigned short memory_size;
	unsigned short reference_count;
	void *default_entry;
};

class dispatcher_2e5fd0 : public dispatcher_base_2e5fd0
{
public:
	referenced_2d88a0 *shared[8][8];
	unsigned long type_mask[32];
	char unknown18c[0x4d8c - 0x18c];
	bool field_4d8c;
	bool field_4d8d;
	array_2e5fd0 array_4d90;
	array_2e5fd0 array_4d9c;

	dispatcher_2e5fd0(void *default_entry_, referenced_2d88a0 *empty);
	~dispatcher_2e5fd0() {}
};

// @retail 0x2e5fd0
dispatcher_2e5fd0::dispatcher_2e5fd0(void *default_entry_, referenced_2d88a0 *empty)
	: dispatcher_base_2e5fd0(default_entry_)
{
	field_4d8c = false;
	field_4d8d = true;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			shared[i][j] = empty;
			if (empty)
			{
				empty->reference_count++;
			}
		}
	}
	for (int type = 0; type < 32; type++)
	{
		type_mask[type] = 1 << type;
	}
	((table_2e5990 *)this)->reset();
}
