// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0B5650.CPP: writes a simulation entity identifier (10 bits of
   index, 4 bits of salt) to a bitstream, and the writer of optional fields
   (include/flags_writer.h) */

#include "unknown_11c920.h"
#include "bitstream.h"
#include "flags_writer.h"

void function_194710(s_bitstream *stream, bool discard);

// @retail 0xb5650
void function_b5650(long identifier, s_bitstream *stream)
{
	long index = identifier & 0x3ff;
	byte salt = (dword)identifier >> 28;
	stream_write_checked(stream, index, 10);
	stream_write_checked(stream, (byte)salt, 4);
}

// @retail 0xb56e0
bool flags_writer_begin(s_flags_writer *writer, long index, char const *name)
{
	bool flag_slot_requested = false;
	writer->name = name;
	writer->index = index;
	writer->started |= 1 << index;
	s_bitstream *stream = writer->stream;
	stream->checkpoints[stream->checkpoint_count] = stream->bit_position;
	stream->checkpoint_count++;
	if (writer->requested & (1 << writer->index))
	{
		stream_write_bit(writer->stream, true);
		writer->written |= 1 << writer->index;
		flag_slot_requested = true;
	}
	return flag_slot_requested;
}

// @retail 0xb5760
void flags_writer_end(s_flags_writer *writer)
{
	dword bit = 1 << writer->index;
	dword written = writer->written;
	bool rewind = false;
	if (written & bit)
	{
		if (writer->discarded & bit)
		{
			rewind = true;
		}
		else if ((writer->stream->size_in_bytes << 3) - writer->stream->bit_position < writer->reserve)
		{
			writer->truncated |= bit;
			rewind = true;
		}
	}
	if (rewind)
	{
		writer->written = ~bit & written;
		function_194710(writer->stream, true);
	}
	else
	{
		writer->stream->checkpoint_count--;
	}
	if (!(writer->written & (1 << writer->index)))
		writer->stream->bit_position++;
	writer->reserve--;
	writer->index = NONE;
	writer->name = 0;
}
