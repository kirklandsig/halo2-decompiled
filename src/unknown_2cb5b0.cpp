// @flags /O2 /Gr
/* UNKNOWN_2CB5B0.CPP: the inflate side of the zlib 1.1.4 copy the game links
   (built with the game's link-time code generation, so it takes its arguments
   in registers): unpacking a whole block with caller-supplied allocators, and
   setting up, resetting and ending an inflate stream, and the block
   decoder's allocation and reset (inflating itself is in
   src/unknown_2cc9d0.cpp). zlib is (C) 1995-2002 Jean-loup Gailly and
   Mark Adler, under the zlib license; these bodies are rebuilt from the
   game's binary */

#include "unknown_2cb5b0.h"
#include <stdlib.h>

dword __stdcall function_2ce7b0(dword checksum, byte const *buffer, dword size);

/* the default allocator of a stream that brings none */
// @retail 0x2ce780
void *__stdcall function_2ce780(void *opaque, long count, long size)
{
	return calloc(count, size);
}

/* the default release of a stream that brings none */
// @retail 0x2ce7a0
void __stdcall function_2ce7a0(void *opaque, void *address)
{
	free(address);
}

/* returns the block decoder to the start of a block, freeing what its mode
   holds; checksum (if not NULL) gets the check of the output so far */
// @retail 0x2ce8e0
void function_2ce8e0(s_block_state *blocks, s_inflate_stream *stream, dword *checksum)
{
	if (checksum != NULL)
		*checksum = blocks->checksum;
	if (blocks->mode == _block_bit_tree || blocks->mode == _block_distance_tree)
		stream->release(stream->opaque, blocks->sub.trees.lengths);
	if (blocks->mode == _block_codes)
		stream->release(stream->opaque, blocks->sub.decode.codes);
	blocks->mode = _block_type;
	blocks->bit_count = 0;
	blocks->bit_buffer = 0;
	blocks->read = blocks->write = blocks->window;
	if (blocks->checksum_function != NULL)
		stream->checksum = blocks->checksum = blocks->checksum_function(0, NULL, 0);
}

/* allocates a block decoder with a window of window_length bytes; NULL when
   out of memory */
// @retail 0x2ce950
s_block_state *function_2ce950(s_inflate_stream *stream, block_checksum checksum_function, dword window_length)
{
	s_block_state *blocks;

	if ((blocks = (s_block_state *)stream->allocate(stream->opaque, 1, sizeof(s_block_state))) == NULL)
		return blocks;
	if ((blocks->tables = stream->allocate(stream->opaque, 8, 1440)) == NULL)
	{
		stream->release(stream->opaque, blocks);
		return NULL;
	}
	if ((blocks->window = (byte *)stream->allocate(stream->opaque, 1, window_length)) == NULL)
	{
		stream->release(stream->opaque, blocks->tables);
		stream->release(stream->opaque, blocks);
		return NULL;
	}
	blocks->window_end = blocks->window + window_length;
	blocks->checksum_function = checksum_function;
	blocks->mode = _block_type;
	function_2ce8e0(blocks, stream, NULL);
	return blocks;
}

/* frees a block decoder and everything it holds */
// @retail 0x2cf480
long function_2cf480(s_block_state *blocks, s_inflate_stream *stream)
{
	function_2ce8e0(blocks, stream, NULL);
	stream->release(stream->opaque, blocks->window);
	stream->release(stream->opaque, blocks->tables);
	stream->release(stream->opaque, blocks);
	return 0;
}

/* starts a stream over at its header (or at its blocks, without one) */
// @retail 0x2cc850
long function_2cc850(s_inflate_stream *stream)
{
	if (stream == NULL || stream->state == NULL)
		return -2;
	stream->total_input = stream->total_output = 0;
	stream->message = NULL;
	stream->state->mode = stream->state->raw ? _inflate_block_data : _inflate_method;
	function_2ce8e0(stream->state->blocks, stream, NULL);
	return 0;
}

/* frees a stream's state; 0, or a negative error */
// @retail 0x2cc890
long function_2cc890(s_inflate_stream *stream)
{
	if (stream == NULL || stream->state == NULL || stream->release == NULL)
		return -2;
	if (stream->state->blocks != NULL)
		function_2cf480(stream->state->blocks, stream);
	stream->release(stream->opaque, stream->state);
	stream->state = NULL;
	return 0;
}

/* sets up a stream for inflating with a window of 2^window_log bytes
   (negative: no header or checksum); 0, or a negative error */
// @retail 0x2cc8d0
long function_2cc8d0(s_inflate_stream *stream, long window_log, char const *version, long stream_size)
{
	if (version == NULL || version[0] != '1' || stream_size != sizeof(s_inflate_stream))
		return -6;
	if (stream == NULL)
		return -2;
	stream->message = NULL;
	if (stream->allocate == NULL)
	{
		stream->allocate = function_2ce780;
		stream->opaque = NULL;
	}
	if (stream->release == NULL)
		stream->release = function_2ce7a0;
	if ((stream->state = (s_inflate_state *)stream->allocate(stream->opaque, 1, sizeof(s_inflate_state))) == NULL)
		return -4;
	stream->state->blocks = NULL;
	stream->state->raw = 0;
	if (window_log < 0)
	{
		window_log = -window_log;
		stream->state->raw = 1;
	}
	if (window_log < 8 || window_log > 15)
	{
		function_2cc890(stream);
		return -2;
	}
	stream->state->window_log = window_log;
	if ((stream->state->blocks = function_2ce950(stream, stream->state->raw ? NULL : function_2ce7b0,
		(dword)1 << window_log)) == NULL)
	{
		function_2cc890(stream);
		return -4;
	}
	function_2cc850(stream);
	return 0;
}

/* inflates source into destination in one pass; destination_size is the room
   on entry and the size produced on return; 0, or a negative error */
// @retail 0x2cb5b0
long __stdcall function_2cb5b0(byte *destination, long *destination_size, byte const *source, long source_size,
	block_allocate allocate, block_free release, void *opaque)
{
	s_inflate_stream stream;
	long error;

	stream.input = source;
	stream.input_left = source_size;
	stream.output = destination;
	stream.output_size = *destination_size;
	stream.allocate = allocate;
	stream.release = release;
	stream.opaque = opaque;
	error = function_2cc8d0(&stream, 15, "1.1.4", sizeof(stream));
	if (error != 0)
		return error;
	error = function_2cc9d0(&stream, 4);
	if (error != 1)
	{
		function_2cc890(&stream);
		return error == 0 ? -5 : error;
	}
	*destination_size = stream.total_output;
	error = function_2cc890(&stream);
	return error;
}
