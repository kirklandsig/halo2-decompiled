// @flags /O2 /GL- /GX
#include "sized_reference_a9ef0.h"
#include "stream_holder_2d8e70.h"

bool __cdecl function_279740(long value);
extern c_sized_reference_a9ef0 *g_480180;

class standard_stream_factory_2d7570 : public c_sized_reference_a9ef0
{
public:
	virtual referenced_2d88a0 *open(long stream_id) = 0;
};
// Retail table 0x411d20: deleting destructor and the two stream accessors.
class c_stream_pair_2d7650 : public c_sized_reference_a9ef0
{
public:
	c_stream_pair_2d7650();
	virtual stream_holder_2d8e70 *first_stream();
	virtual stream_holder_2d8e70 *second_stream();
	static void __cdecl operator delete(void *object)
	{
		g_memory_2d88a0->release_block(object,
			((c_stream_pair_2d7650 *)object)->memory_size, 0x13);
	}
	stream_holder_2d8e70 first;
	stream_holder_2d8e70 second;
};

static void release_stream_reference(referenced_2d88a0 *stream)
{
	--stream->reference_count;
	if (stream->reference_count == 0)
	{
		if (function_279740((long)stream))
			stream->reference_count = 1;
		else
			delete stream;
	}
}

// The holders acquire references, then the factory's original references
// are released. Stream ids one and two select the two standard outputs.
// @retail 0x2d7570
c_stream_pair_2d7650::c_stream_pair_2d7650()
	: first(((standard_stream_factory_2d7570 *)g_480180)->open(1)),
	  second(((standard_stream_factory_2d7570 *)g_480180)->open(2))
{
	release_stream_reference(first.stream);
	release_stream_reference(second.stream);
}
// @retail 0x2d7650
stream_holder_2d8e70 *c_stream_pair_2d7650::first_stream()
{
	return &first;
}
// @retail 0x2d7660
stream_holder_2d8e70 *c_stream_pair_2d7650::second_stream()
{
	return &second;
}
// @retail 0x2d7670 deleting c_stream_pair_2d7650
// @retail 0x2d76a0 destructor c_stream_pair_2d7650
