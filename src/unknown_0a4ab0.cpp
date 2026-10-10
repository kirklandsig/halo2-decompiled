// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0A4AB0.CPP: the update of the fields every game engine's globals
   entity has (game_engine_entity_definitions.cpp): the team mapping, the
   current state, whether the game finished, the current round and the
   round timer */

#include "unknown_11c920.h"
#include "bitstream.h"
#include "flags_writer.h"
#include "game_engine_globals_update.h"
#include <string.h>

/* 0x11c9a0, src/unknown_11c9a0.cpp */
unsigned long function_11c9a0(char const *string, unsigned long size);

static inline char *function_x91aa57(char *destination, char const *source, long size)
{
	strncpy(destination, source, size);
	destination[size - 1] = 0;
	return destination;
}

static inline char *function_xf9f5fa(char *destination, char const *source, unsigned long size)
{
	unsigned long length = function_11c9a0(destination, size);
	strncpy(destination + length, source, size - length);
	destination[size - 1] = 0;
	return destination;
}

/* the names of the fields an update holds (the flags are tested as masks
   1 to 4, as retail does, so the team mapping is never named) */
// @retail 0xa49b0
void game_engine_globals_describe_update(c_game_engine_entity_definition const *definition, dword const *flags,
	unsigned long size, char *buffer)
{
	dword update_flags = *flags;
	function_x91aa57(buffer, "", size);
	if (update_flags & 1)
		function_xf9f5fa(buffer, "current-state:", size);
	if (update_flags & 2)
		function_xf9f5fa(buffer, "game-finished:", size);
	if (update_flags & 3)
		function_xf9f5fa(buffer, "current-round:", size);
	if (update_flags & 4)
		function_xf9f5fa(buffer, "round-timer:", size);
}

// @retail 0xa4ab0
bool game_engine_globals_write_update(c_game_engine_entity_definition const *definition, long reserve_bits, dword requested, dword *written,
	s_game_engine_globals_update const *update, s_bitstream *stream)
{
	bool update_space_available = false;
	s_flags_writer writer;
	flags_writer_initialize(&writer, stream, 0, 5, requested, reserve_bits);
	if (writer.space)
	{
		if (flags_writer_begin(&writer, 0, "team-mapping-exists"))
		{
			stream_write_checked(stream, update->team_mapping0, 8);
			stream_write_checked(stream, update->team_mask, 9);
			stream_write_checked(stream, update->team_mapping4, 8);
			stream_write_checked(stream, update->team_mapping6, 8);
			stream_write_checked(stream, update->team_mapping8, 8);
			for (long i = 0; i < 9; i++)
			{
				if (update->team_mask & (1 << i))
					stream_write_checked(stream, update->team_indices[i], 4);
			}
		}
		flags_writer_end(&writer);
		if (flags_writer_begin(&writer, 1, "current-state-exists"))
			stream_write_checked(stream, update->field_c_4, 2);
		flags_writer_end(&writer);
		if (flags_writer_begin(&writer, 2, "game-finished-exists"))
			stream_write_bit(stream, update->game_finished);
		flags_writer_end(&writer);
		if (flags_writer_begin(&writer, 3, "current-round-exists"))
			stream_write_checked(stream, update->current_round, 5);
		flags_writer_end(&writer);
		if (flags_writer_begin(&writer, 4, "round-timer-exists"))
			stream_write_checked(stream, update->round_timer + 1, 16);
		flags_writer_end(&writer);
		*written |= writer.written;
		update_space_available = true;
	}
	return update_space_available;
}

// @retail 0xa4e20
bool game_engine_globals_read_update(c_game_engine_entity_definition const *definition, s_game_engine_globals_update *update, s_bitstream *stream, dword *read)
{
	dword mask = 0;
	bool valid = true;
	bool *valid_reference = &valid;
	if (function_1957d0(stream))
	{
		update->team_mapping0 = (word)function_1959c0(stream, 8);
		update->team_mask = (word)function_1959c0(stream, 9);
		update->team_mapping4 = (word)function_1959c0(stream, 8);
		update->team_mapping6 = (word)function_1959c0(stream, 8);
		update->team_mapping8 = (word)function_1959c0(stream, 8);
		if (!(update->team_mapping4 & ~update->team_mapping0) &&
			!(update->team_mapping8 & ~update->team_mapping4) &&
			!(update->team_mapping6 & ~update->team_mapping8))
		{
			valid = true;
		}
		else
		{
			valid = false;
		}
		for (long i = 0; valid && i < 9; i++)
		{
			if (update->team_mask & (1 << i))
			{
				update->team_indices[i] = (short)function_1959c0(stream, 4);
				if (valid && update->team_indices[i] >= 0 && update->team_indices[i] < 8)
					valid = true;
				else
					valid = false;
			}
			else
			{
				update->team_indices[i] = NONE;
			}
		}
		mask = 1;
	}
	if (stream_read_bit(stream))
	{
		update->field_c_4 = (byte)function_1959c0(stream, 2);
		mask |= 2;
	}
	long end = stream->size_in_bytes << 3;
	long position = stream->bit_position;
	bool present = false;
	if (position <= end)
		present = (stream->data[position / 8] & (1 << (position % 8))) != 0;
	stream->bit_position = position + 1;
	if (present)
	{
		position = stream->bit_position;
		bool finished = false;
		if (position <= end)
			finished = (stream->data[position / 8] & (1 << (position % 8))) != 0;
		stream->bit_position = position + 1;
		update->game_finished = finished;
		mask |= 4;
	}
	if (stream_read_bit(stream))
	{
		update->current_round = (short)function_1959c0(stream, 5);
		mask |= 8;
	}
	if (stream_read_bit(stream))
	{
		update->round_timer = (short)(function_1959c0(stream, 16) - 1);
		mask |= 0x10;
	}
	*read |= mask;
	return *valid_reference;
}
