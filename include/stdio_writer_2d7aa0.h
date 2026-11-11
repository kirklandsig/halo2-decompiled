#ifndef STDIO_WRITER_2D7AA0_H
#define STDIO_WRITER_2D7AA0_H
#include <stdio.h>
#include "buffered_writer_2d9e40.h"
// Seven slots in retail table 0x411d88, in order.
class stdio_writer_2d7aa0 : public stream_writer_2d9e40
{
public:
	static void *operator new(unsigned int size)
	{
		stdio_writer_2d7aa0 *object = (stdio_writer_2d7aa0 *)g_memory_2d88a0->allocate_block(size, 0x15);
		object->memory_size = (unsigned short)size;
		return object;
	}
	static void __cdecl operator delete(void *object)
	{
		g_memory_2d88a0->release_block(object,
			((stdio_writer_2d7aa0 *)object)->memory_size, 0x15);
	}
	FILE *file;
	bool_2d9e40 owns_file;
	stdio_writer_2d7aa0(const char *name);
	void close();
	virtual ~stdio_writer_2d7aa0();
	virtual bool_2d9e00 status();
	virtual int write(const void *data, int count);
	virtual void flush_stream();
	virtual void query(bool *result) { *result = true; }
	virtual long seek(long offset, long origin);
	virtual long tell();
};
#include "service_streams_2d8780.h"
typedef debug_stream_factory_2d76f0 stdio_stream_factory_2d79c0;
#endif
