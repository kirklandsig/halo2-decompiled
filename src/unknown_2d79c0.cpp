// @flags /O2 /GL- /GX
// Open a file writer, then wrap it in a 4 KB buffered writer.
#include "stdio_writer_2d7aa0.h"
#include "service_streams_2d8780.h"
bool __cdecl function_279740(long value);
// @retail 0x2d79c0
stream_writer_2d9e40 *debug_stream_factory_2d76f0::open_writer(const char *name)
{
	stdio_writer_2d7aa0 *file_stream = new stdio_writer_2d7aa0(name);
	stream_writer_2d9e40 *writer = new buffered_writer_2d9e40(file_stream, 0x1000);
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
	return writer;
}
