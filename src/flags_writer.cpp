// @flags /O2 /arch:SSE /Gr
/* FLAGS_WRITER.CPP: the writer of optional fields (include/flags_writer.h):
   each field is preceded by a bit saying whether it follows, and a field
   that was discarded or no longer fits is rewound */

#include "unknown_11c920.h"
#include "bitstream.h"
#include "flags_writer.h"


/* starts a field: marks the stream's position and writes the field's bit,
   set when the field was requested; returns whether the field follows */
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

/* ends the current field: rewinds it when it was discarded or the stream
   no longer has room for the bits reserved, and reserves one bit less */
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
		writer->stream->function_194710(true);
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
