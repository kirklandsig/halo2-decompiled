/* BITSTREAM.H: the bit stream shared by the entity definitions (09a9f0,
   09fe30) and the 0x450c94 classes (096e90, 0984d0), and by the network
   message codecs: 195720 writes into it and 1959c0 reads from it. The
   module itself is src/unknown_1946f0.cpp */

#ifndef BITSTREAM_H
#define BITSTREAM_H

#include "unknown_11c920.h"

/* a bitstream: the data, its size in bytes, and the current bit position */
struct s_bitstream
{
	union
	{
		byte *data;
		dword *buffer;
	};
	long size_in_bytes;
	long unknown08;
	long mode;                 /* 1 is writing, 3 is reading */
	long bit_position;
	bool error;
	byte unknown15[3];
	long checkpoint_count;
	long checkpoints[4];       /* bit positions saved by the push, popped at 194710 */
	long unknown2c;
	long unknown30;
};

void function_195720(s_bitstream *stream, dword value, long count);
dword function_1959c0(s_bitstream *stream, long count);
bool function_1957d0(s_bitstream *stream);
void function_1955d0(s_bitstream *stream, void const *source, long bits);
void function_195820(s_bitstream *stream, void *destination, long bits);

/* 0xb66f0, src/unknown_0b66c0.cpp */
char *csprintf_256(char *buffer, char const *format, ...);

/* The small stream functions below have out-of-line copies in the module
   (src/unknown_195720.cpp, src/unknown_1946f0.cpp), but the network message
   codecs and the entity definitions have them expanded inline */

/* reads one bit; 0x974c0 and 0x97240 inline it */
inline bool stream_read_bit(s_bitstream *stream)
{
	long position = stream->bit_position;
	bool bit = false;
	if (position <= (stream->size_in_bytes << 3))
		bit = (stream->data[position / 8] & (1 << (position % 8))) != 0;
	stream->bit_position = position + 1;
	return bit;
}

/* writes one bit (0x194830) */
inline void stream_write_bit(s_bitstream *stream, bool value)
{
	if ((stream->size_in_bytes << 3) - stream->bit_position >= 1 && value)
		stream->data[stream->bit_position / 8] |= (byte)(1 << (stream->bit_position % 8));
	stream->bit_position++;
}

/* points a stream at a buffer, at its start (the streams built on the stack
   set unknown08 to 1 first) */
inline void stream_set_buffer(s_bitstream *stream, void const *data, long size)
{
	stream->data = (byte *)data;
	stream->size_in_bytes = size;
	stream->mode = 0;
	stream->bit_position = 0;
	stream->checkpoint_count = 0;
	stream->error = false;
}

/* saves the position, so that what is written after it can be taken back
   (194710) or kept (the checkpoint count stepped back) */
inline void stream_save_position(s_bitstream *stream)
{
	stream->checkpoints[stream->checkpoint_count++] = stream->bit_position;
}

/* writes a value that must fit in the given number of bits (0x1947e0) */
inline void stream_write_checked(s_bitstream *stream, dword value, long bits)
{
	if (bits < 32 && value >= (dword)(1 << bits))
	{
		char message[256];
		message[0] = 0;
		csprintf_256(message, "%u exceeds max value of %u", value, 1 << bits);
	}
	function_195720(stream, value, bits);
}

/* whether a read ran past the end of the data or failed (0x1946f0); forced
   inline, as some of the decoders in src/simulation_message_codecs.cpp need */
__forceinline bool stream_overflowed(s_bitstream *stream)
{
	bool result = stream->bit_position > (stream->size_in_bytes << 3);
	if (stream->error)
		result = true;
	return result;
}

/* reads a zero terminated string of at most count characters (0x194fa0 for
   words); a string that does not end in time fails the stream */
inline void read_word_string(s_bitstream *stream, word *buffer, long count)
{
	long i;
	for (i = 0; i < count; i++)
	{
		buffer[i] = (word)function_1959c0(stream, 16);
		if (buffer[i] == 0)
			break;
	}
	if (i >= count)
	{
		buffer[count - 1] = 0;
		stream->error = true;
	}
}

inline void read_byte_string(s_bitstream *stream, byte *buffer, long count)
{
	long i;
	for (i = 0; i < count; i++)
	{
		buffer[i] = (byte)function_1959c0(stream, 8);
		if (buffer[i] == 0)
			break;
	}
	if (i >= count)
	{
		buffer[count - 1] = 0;
		stream->error = true;
	}
}

/* the real a quantized value stands for (the event definitions in
   src/unknown_09a5e0.cpp, and 0x194ff0 on the logarithms of its bounds) */
static inline real event_dequantize_real(long value, long maximum, real minimum_value, real maximum_value)
{
	if (value == 0)
		return minimum_value;
	if (value >= maximum)
		return maximum_value;
	return (minimum_value * (maximum - value) + maximum_value * value) / maximum;
}

#endif
