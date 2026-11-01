// @flags /O2 /GL-
#include <stdio.h>
#include "reference_base_318970.h"
#include "buffered_writer_2d9e40.h"

extern "C" void __stdcall OutputDebugStringA(char const *text);

// @retail 0x2d7860
void __cdecl release_stream_storage_2d7860(void *object)
{
	g_memory_2d88a0->release_block(object,
		((reference_base_318970 *)object)->memory_size, 0x15);
}

// Seven slots observed in the debug stream's retail table at 0x411d3c.
class debug_writer_2d7c00 : public reference_base_318970
{
public:
	static void *operator new(unsigned int size)
	{
		debug_writer_2d7c00 *object = (debug_writer_2d7c00 *)g_memory_2d88a0->allocate_block(size, 0x15);
		object->memory_size = (unsigned short)size;
		return object;
	}
	static void operator delete(void *object)
	{
		g_memory_2d88a0->release_block(object,
			((debug_writer_2d7c00 *)object)->memory_size, 0x15);
	}
	virtual bool_2d9e00 status() { return true; }
	virtual int write(void const *data, int count);
	virtual void flush() {}
	virtual void query(bool *result) { *result = false; }
	virtual long seek(long, long) { return 1; }
	virtual long tell() { return -1; }
};

// @retail 0x2d7720 deleting debug_writer_2d7c00

class debug_stream_factory_2d76f0
{
public:
	debug_writer_2d7c00 *open(long stream_id);
};

// @retail 0x2d76f0
debug_writer_2d7c00 *debug_stream_factory_2d76f0::open(long stream_id)
{
	return new debug_writer_2d7c00;
}

struct debug_text_buffer_2d7c00
{
	char *data;
	long capacity_and_flags;
	debug_text_buffer_2d7c00(long size) : capacity_and_flags(size)
	{
		data = (char *)g_memory_2d88a0->allocate_block(size, 0x12);
	}
	~debug_text_buffer_2d7c00()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
			g_memory_2d88a0->release_block(data, capacity_and_flags & 0x7fffffff, 0x12);
	}
};

// @retail 0x2d7c00
int debug_writer_2d7c00::write(void const *data, int count)
{
	if (count)
	{
		char const *text = (char const *)data;
		if (text[count - 1] == 0)
		{
			OutputDebugStringA(text);
			printf("%s", text);
		}
		else
		{
			debug_text_buffer_2d7c00 buffer(count + 1);
			for (int i = 0; i < count; ++i)
				buffer.data[i] = text[i];
			buffer.data[count] = 0;
			OutputDebugStringA(buffer.data);
			printf("%s", buffer.data);
		}
	}
	return count;
}
