// @flags /O2 /GL- /GX
#include "stdio_reader_2d7880.h"
#include "buffered_reader_2d9c00.h"
#include "service_streams_2d8780.h"

bool __cdecl function_279740(long value);

// @retail 0x2d7750
c_sized_reference_a9ef0 *debug_stream_factory_2d76f0::open_reader(const char *name)
{
	stdio_reader_2d7880 *file_stream = new stdio_reader_2d7880(name);
	if (!file_stream->mark_supported())
	{
		c_sized_reference_a9ef0 *reader = new buffered_2d9c00(file_stream, 0x1000);
		file_stream->reference_count--;
		if (file_stream->reference_count == 0)
		{
			if (function_279740((long)file_stream))
			{
				file_stream->reference_count = 1;
			}
			else
			{
				delete file_stream;
			}
		}
		return reader;
	}
	return file_stream;
}
