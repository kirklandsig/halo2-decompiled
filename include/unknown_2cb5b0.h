/* UNKNOWN_2CB5B0.H: the streams and states of the game's zlib 1.1.4 inflate
   copy (src/unknown_2cb5b0.cpp, src/unknown_2cc9d0.cpp) */

#ifndef UNKNOWN_2CB5B0_H
#define UNKNOWN_2CB5B0_H

#include "unknown_11c920.h"

typedef void *(__stdcall *block_allocate)(void *, long, long);
typedef void (__stdcall *block_free)(void *, void *);
typedef dword (__stdcall *block_checksum)(dword checksum, byte const *buffer, dword size);

/* the state of the deflate-block decoder under an inflate stream */
struct s_block_state
{
	long mode;
	union
	{
		dword stored_left;
		struct
		{
			dword table;
			dword index;
			dword *lengths;
			dword depth;
			void *tree;
		} trees;
		struct
		{
			void *codes;
		} decode;
	} sub;
	dword last;
	dword bit_count;
	dword bit_buffer;
	void *tables;
	byte *window;
	byte *window_end;
	byte *read;
	byte *write;
	block_checksum checksum_function;
	dword checksum;
};
typedef char assert_block_state_size[(sizeof(s_block_state) == 0x40) ? 1 : -1];

/* the private state of an inflate stream */
struct s_inflate_state
{
	long mode;
	union
	{
		dword method;
		struct
		{
			dword computed;
			dword expected;
		} check;
		dword marker;
	} sub;
	long raw;
	dword window_log;
	s_block_state *blocks;
};

/* the stream an inflate works on */
struct s_inflate_stream
{
	byte const *input;
	dword input_left;
	dword total_input;
	byte *output;
	dword output_size;
	dword total_output;
	char const *message;
	s_inflate_state *state;
	block_allocate allocate;
	block_free release;
	void *opaque;
	long content_kind;
	dword checksum;
	dword reserved;
};
typedef char assert_inflate_stream_size[(sizeof(s_inflate_stream) == 0x38) ? 1 : -1];
typedef char assert_inflate_state_size[(sizeof(s_inflate_state) == 0x18) ? 1 : -1];

/* the inflate stream's modes */
enum
{
	_inflate_method = 0,
	_inflate_flag,
	_inflate_dictionary_4,
	_inflate_dictionary_3,
	_inflate_dictionary_2,
	_inflate_dictionary_1,
	_inflate_dictionary_0,
	_inflate_block_data,
	_inflate_check_4,
	_inflate_check_3,
	_inflate_check_2,
	_inflate_check_1,
	_inflate_done,
	_inflate_bad
};

/* the block decoder's modes that own memory */
enum
{
	_block_type = 0,
	_block_bit_tree = 4,
	_block_distance_tree,
	_block_codes
};

void function_2ce8e0(s_block_state *blocks, s_inflate_stream *stream, dword *checksum);
long function_2cc9d0(s_inflate_stream *stream, long flush);

#endif
