// @flags /O2 /GL- /GX
#include "reference_counted_2d88a0.h"
#include "string_block_2d8e50.h"

struct pointer_buffer_372680
{
	void **data;
	long size;
	unsigned long capacity_and_flags;
	pointer_buffer_372680() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~pointer_buffer_372680()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(void *), 0x12);
	}
};
class pointer_storage_372680 : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x0c)
	pointer_buffer_372680 values;
	virtual ~pointer_storage_372680();
};
struct text_block_372800 : class_2d8e50
{
	long references;
};
struct text_record_372800
{
	char *text;
	long unknown4, unknown8;
	~text_record_372800()
	{
		text_block_372800 *block = (text_block_372800 *)(text - 0xc);
		if (--block->references < 0)
			block->release();
	}
};
struct record_buffer_372760
{
	text_record_372800 *data;
	long size;
	unsigned long capacity_and_flags;
	record_buffer_372760() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~record_buffer_372760();
};
struct records_372800 : record_buffer_372760
{
	// Retail expands the owning-array cleanup into its owner's implicit
	// destructor; the buffer base also supplies the unwind cleanup entry.
	__forceinline ~records_372800()
	{
		for (long i = 0; i < size; ++i)
			data[i].~text_record_372800();
	}
};
class record_storage_372800 : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x13)
	long unknown8;
	records_372800 records;
	record_storage_372800() : unknown8(0) {}
};

#include <stddef.h>

typedef char pointer_storage_size[sizeof(pointer_storage_372680) == 0x14 ? 1 : -1];
typedef char record_storage_size[sizeof(record_storage_372800) == 0x18 ? 1 : -1];
typedef char text_record_size[sizeof(text_record_372800) == 0xc ? 1 : -1];
typedef char record_buffer_offset[offsetof(record_storage_372800, records) == 0xc ? 1 : -1];

// @retail 0x372680
pointer_storage_372680::~pointer_storage_372680() {}
// @retail 0x372730 deleting pointer_storage_372680

// @retail 0x372760
inline record_buffer_372760::~record_buffer_372760()
{
	if ((capacity_and_flags & 0x80000000) == 0)
		g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(text_record_372800), 0x12);
}
// @retail 0x372800 destructor record_storage_372800
// @retail 0x3727d0 deleting record_storage_372800

// @retail 0x3726f0
pointer_storage_372680 *__cdecl allocate_pointer_storage_3726f0()
{
	return new pointer_storage_372680;
}
// @retail 0x372790
record_storage_372800 *__cdecl allocate_record_storage_372790()
{
	return new record_storage_372800;
}
