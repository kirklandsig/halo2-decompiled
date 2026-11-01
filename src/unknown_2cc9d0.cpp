// @flags /O2 /Gr /arch:SSE
/* UNKNOWN_2CC9D0.CPP: the inflate loop of the game's zlib 1.1.4 copy (the
   rest of it is in src/unknown_2cb5b0.cpp; this part is built for SSE, so it
   selects with cmov). zlib is (C) 1995-2002 Jean-loup Gailly and Mark Adler,
   under the zlib license; this body is rebuilt from the game's binary */

#include "unknown_2cb5b0.h"

/* takes the next input byte of a stream (the caller has checked there is one) */
#define INFLATE_NEXT_BYTE(stream) ((stream)->input_left--, (stream)->total_input++, *(stream)->input++)

long __stdcall function_2ce9e0(s_block_state *blocks, s_inflate_stream *stream, long result);

/* inflates as much of the stream's input as it can; flush 4 asks for the
   end of the stream. 0, 1 at its end, 2 for a preset dictionary, or a
   negative error */
// @retail 0x2cc9d0
long function_2cc9d0(s_inflate_stream *stream, long flush)
{
	long result;
	dword flags;

	if (stream == NULL || stream->state == NULL || stream->input == NULL)
		return -2;
	flush = flush == 4 ? -5 : 0;
	result = -5;
	while (true)
	{
		switch (stream->state->mode)
		{
		case _inflate_method:
			if (stream->input_left == 0)
				return result;
			result = flush;
			if (((stream->state->sub.method = INFLATE_NEXT_BYTE(stream)) & 0xf) != 8)
			{
				stream->state->mode = _inflate_bad;
				stream->message = "unknown compression method";
				stream->state->sub.marker = 5;
				break;
			}
			if ((stream->state->sub.method >> 4) + 8 > stream->state->window_log)
			{
				stream->state->mode = _inflate_bad;
				stream->message = "invalid window size";
				stream->state->sub.marker = 5;
				break;
			}
			stream->state->mode = _inflate_flag;
		case _inflate_flag:
			if (stream->input_left == 0)
				return result;
			result = flush;
			flags = INFLATE_NEXT_BYTE(stream);
			if (((stream->state->sub.method << 8) + flags) % 31)
			{
				stream->state->mode = _inflate_bad;
				stream->message = "incorrect header check";
				stream->state->sub.marker = 5;
				break;
			}
			if (!(flags & 0x20))
			{
				stream->state->mode = _inflate_block_data;
				break;
			}
			stream->state->mode = _inflate_dictionary_4;
		case _inflate_dictionary_4:
			if (stream->input_left == 0)
				return result;
			result = flush;
			stream->state->sub.check.expected = (dword)INFLATE_NEXT_BYTE(stream) << 24;
			stream->state->mode = _inflate_dictionary_3;
		case _inflate_dictionary_3:
			if (stream->input_left == 0)
				return result;
			result = flush;
			stream->state->sub.check.expected += (dword)INFLATE_NEXT_BYTE(stream) << 16;
			stream->state->mode = _inflate_dictionary_2;
		case _inflate_dictionary_2:
			if (stream->input_left == 0)
				return result;
			result = flush;
			stream->state->sub.check.expected += (dword)INFLATE_NEXT_BYTE(stream) << 8;
			stream->state->mode = _inflate_dictionary_1;
		case _inflate_dictionary_1:
			if (stream->input_left == 0)
				return result;
			result = flush;
			stream->state->sub.check.expected += (dword)INFLATE_NEXT_BYTE(stream);
			stream->checksum = stream->state->sub.check.expected;
			stream->state->mode = _inflate_dictionary_0;
			return 2;
		case _inflate_dictionary_0:
			stream->state->mode = _inflate_bad;
			stream->message = "need dictionary";
			stream->state->sub.marker = 0;
			return -2;
		case _inflate_block_data:
			result = function_2ce9e0(stream->state->blocks, stream, result);
			if (result == -3)
			{
				stream->state->mode = _inflate_bad;
				stream->state->sub.marker = 0;
				break;
			}
			if (result == 0)
				result = flush;
			if (result != 1)
				return result;
			result = flush;
			function_2ce8e0(stream->state->blocks, stream, &stream->state->sub.check.computed);
			if (stream->state->raw)
			{
				stream->state->mode = _inflate_done;
				break;
			}
			stream->state->mode = _inflate_check_4;
		case _inflate_check_4:
			if (stream->input_left == 0)
				return result;
			result = flush;
			stream->state->sub.check.expected = (dword)INFLATE_NEXT_BYTE(stream) << 24;
			stream->state->mode = _inflate_check_3;
		case _inflate_check_3:
			if (stream->input_left == 0)
				return result;
			result = flush;
			stream->state->sub.check.expected += (dword)INFLATE_NEXT_BYTE(stream) << 16;
			stream->state->mode = _inflate_check_2;
		case _inflate_check_2:
			if (stream->input_left == 0)
				return result;
			result = flush;
			stream->state->sub.check.expected += (dword)INFLATE_NEXT_BYTE(stream) << 8;
			stream->state->mode = _inflate_check_1;
		case _inflate_check_1:
			if (stream->input_left == 0)
				return result;
			result = flush;
			stream->state->sub.check.expected += (dword)INFLATE_NEXT_BYTE(stream);
			if (stream->state->sub.check.computed != stream->state->sub.check.expected)
			{
				stream->state->mode = _inflate_bad;
				stream->message = "incorrect data check";
				stream->state->sub.marker = 5;
				break;
			}
			stream->state->mode = _inflate_done;
		case _inflate_done:
			return 1;
		case _inflate_bad:
			return -3;
		default:
			return -2;
		}
	}
}
