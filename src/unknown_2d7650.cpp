// @flags /O2 /GL- /GX
#include "sized_reference_a9ef0.h"
#include "stream_holder_2d8e70.h"

bool __cdecl function_279740(long value);
extern c_sized_reference_a9ef0 *g_480180;

#include "service_streams_2d8780.h"

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
