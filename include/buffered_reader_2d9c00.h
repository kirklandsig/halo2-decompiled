#ifndef LANE_A_BUFFERED_READER_H
#define LANE_A_BUFFERED_READER_H
#include "stream_reader_2d9c80.h"

// Shared declaration of the existing 36-byte buffered reader. Its method
// bodies remain in unknown_2d9c00.cpp; no second constructor implementation.
class buffered_2d9c00 : public reader_2d9c80
{
public:
	static void *operator new(unsigned int size)
	{
		buffered_2d9c00 *object = (buffered_2d9c00 *)g_memory_2d88a0->allocate_block(size, 0x15);
		object->memory_size = (unsigned short)size;
		return object;
	}
	static void __cdecl operator delete(void *object)
	{
		g_memory_2d88a0->release_block(object,
			((buffered_2d9c00 *)object)->memory_size, 0x15);
	}
	reader_2d9c80 *source;
	char *buffer;
	int position;
	int end;
	int capacity;
	int mark;
	int mark_limit;
	buffered_2d9c00(reader_2d9c80 *source_, long size_);
	virtual ~buffered_2d9c00();
	virtual bool_2d9e00 status();
	virtual int read(void *destination, int count);
	virtual long skip(long count);
	virtual reader_bool_2d7750 mark_supported();
	virtual long set_mark(long limit);
	virtual long rewind_mark();
	virtual bool_2d9e00 seek_supported();
	virtual long seek(long offset, long origin);
	virtual long tell();
	int refill();
};

#include "service_streams_2d8780.h"
typedef debug_stream_factory_2d76f0 reader_factory_2d7750;
#endif
