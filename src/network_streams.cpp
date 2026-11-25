// @flags /O2 /Ob1 /arch:SSE /Gr
/* NETWORK_STREAMS.CPP: the two kinds of message stream a connection owns
   (src/unknown_0820f0.cpp allocates them): the unreliable stream (0x2850
   bytes, vtable 0x450db8) and the reliable stream (0x97c bytes, vtable
   0x450dd8). Both keep their messages in windows over sequence numbers. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0820f0.h"
#include "unknown_0662e0.h"
#include "bitstream.h"
#include "network_message_types.h"
#include "network_streams.h"
#include <xtl.h>
#include <stdlib.h>
#include <string.h>




static inline long sequence_window_count(s_sequence_window const *window)
{
	return window->newest - window->oldest;
}

static inline long sequence_window_first(s_sequence_window const *window)
{
	return window->count == 0 ? NONE : window->head;
}

static inline long sequence_window_index(s_sequence_window const *window, long sequence)
{
	long index = NONE;
	if (sequence > window->oldest && sequence <= window->newest)
	{
		long offset = sequence - window->oldest - 1;
		index = (offset + sequence_window_first(window)) % window->capacity;
	}
	return index;
}

static inline void sequence_window_reset(s_sequence_window *window, long sequence)
{
	window->newest = sequence;
	window->oldest = sequence;
	window->head = 0;
	window->count = 0;
}

static inline void sequence_window_initialize(s_sequence_window *window, long sequence)
{
	sequence_window_reset(window, sequence);
	window->valid = true;
}

static inline void sequence_window_extend(s_sequence_window *window, long sequence)
{
	if (sequence > window->newest)
	{
		window->count += sequence - window->newest;
		window->newest = sequence;
	}
}

static inline void sequence_window_advance(s_sequence_window *window, long sequence)
{
	long advance = sequence - window->oldest;
	if (advance > 0)
	{
		if (window->count >= advance)
		{
			window->head = (window->head + advance) % window->size;
			window->count -= advance;
		}
		window->oldest = sequence;
	}
}


// @retail 0x81410
bool c_network_reliable_stream::v1(long *reason)
{
	bool result = false;
	if (m_active && m_unknown05)
	{
		result = true;
		if (reason)
			*reason = 11;
	}
	return result;
}

// @retail 0x814c0
bool c_network_unreliable_stream::v1(long *reason)
{
	bool result = false;
	if (m_active && m_unknown05)
	{
		result = true;
		if (reason)
			*reason = 12;
	}
	return result;
}

static inline dword network_time_now(void)
{
	if (g_510548)
		return g_51054c;
	return GetTickCount();
}

static __forceinline void function_94bf3(s_allocator_globals *arg_0, void *arg_1)
{
	c_allocator *local_0 = arg_0->allocator;
	local_0->release(arg_1, NONE);
	if (arg_1)
		arg_0->count--;
}

/* the release routine retail inlines here */
static inline void free_block(void *block)
{
	long info;
	if (!g_4d87f8->allocator->get_info(block, &info))
		info = NONE;
	function_94bf3(g_4d87f8, block);
}

static __forceinline void function_94bf4(s_allocator_globals *arg_0, void *arg_1, s_allocator_globals **arg_2)
{
	c_allocator *local_0 = arg_0->allocator;
	*arg_2 = arg_0;
	local_0->release(arg_1, NONE);
	if (arg_1)
		(*(s_allocator_globals *volatile *)arg_2)->count--;
}

static __forceinline void function_94bf7(s_sequence_window *arg_0)
{
	arg_0->newest = 0;
	arg_0->oldest = 0;
	arg_0->head = 0;
	arg_0->count = 0;
	arg_0->valid = true;
}

// Matched in a full build with stand-in SDK headers (was 2): the first loop's info lives in its own block and
// the second loop has a plain counter; the two share [esp + 0xc] because the block ends the info's lifetime, and
// the counter's reload now comes after the data store as in retail. Before, 2:
// score 2 (was 2): retail reloads the address-taken loop counter before it
// stores fragment->data = 0, ours stores the zero first.
// b14C (11-17) walk against retail, fast build on r847: every other statement agrees (both window loops, the
// get_info calls at 0x94c7b/0x94d55, release via [edx] at 0x94c9a/0x94d72, final window clears); the first loop's
// info slot and the second loop's counter share [esp + 0xc] in retail, as ours do. Only the order at 0x94d7b/0x94d7f
// differs (mov edi, [esp + 0xc] before mov [ebp + 4], 0). Tried, all 2 unless noted: ++work; while loop; the
// file's free_block helper (second loop only; both loops 8: frame 0xc not 8); store through a local void **;
// unreliable_stream_get_fragment for the element pointer; non-address-taken counter (8, extra stack slot);
// increment placed before the store in both branches (34).
// @retail 0x94bf0
void function_094bf0(s_network_stream_header *header)
{
	c_network_unreliable_stream *stream = (c_network_unreliable_stream *)header;
	if (stream->m_active)
	{
		if (sequence_window_count(&stream->m_message_window))
		{
			for (long sequence = stream->m_message_window.oldest + 1; sequence <= stream->m_message_window.newest; sequence++)
			{
				s_stream_message *message;
				long index = sequence_window_index(&stream->m_message_window, sequence);
				message = 0;
				if (index != NONE)
					message = &stream->m_messages[index];
				stream->m_message_bytes -= message->size;
				void *local_2 = message->data;
				{
					long info;
					if (!g_4d87f8->allocator->get_info(local_2, &info))
						info = NONE;
				}
				function_94bf3(g_4d87f8, local_2);
				message->data = 0;
			}
		}
		if (sequence_window_count(&stream->m_fragment_window))
		{
			for (long work = stream->m_fragment_window.oldest + 1; work <= stream->m_fragment_window.newest; work++)
			{
				s_stream_fragment *fragment;
				long index = sequence_window_index(&stream->m_fragment_window, work);
				fragment = 0;
				if (index != NONE)
					fragment = &stream->m_fragments[index];
				if (fragment->data)
				{
					stream->m_fragment_bytes -= fragment->size;
					void *local_2 = fragment->data;
					long local_0;
					if (!g_4d87f8->allocator->get_info(local_2, &local_0))
						local_0 = NONE;
					function_94bf3(g_4d87f8, local_2);
					fragment->data = 0;
				}
			}
		}
	}
	stream->m_unknown05 = false;
	function_94bf7(&stream->m_message_window);
	function_94bf7(&stream->m_fragment_window);
	stream->m_message_bytes = 0;
	stream->m_fragment_bytes = 0;
}

// @retail 0x94df0
bool c_network_unreliable_stream::v2(bool *pending)
{
	bool result = false;
	if (sequence_window_count(&m_message_window))
	{
		for (long sequence = m_message_window.oldest + 1; sequence <= m_message_window.newest; sequence++)
		{
			s_stream_message *message;
			long index = sequence_window_index(&m_message_window, sequence);
			message = 0;
			if (index != NONE)
				message = &m_messages[index];
			if (message->flags & 4)
			{
				result = true;
				break;
			}
		}
	}
	if (result)
		*pending = true;
	return result;
}

// @retail 0x94e80
long c_network_unreliable_stream::v3(long a, long b)
{
	return 2;
}

// @retail 0x95b70
inline s_stream_fragment *unreliable_stream_get_fragment(c_network_unreliable_stream *stream, long sequence)
{
	s_stream_fragment *result;
	long index = sequence_window_index(&stream->m_fragment_window, sequence);
	result = 0;
	if (index != NONE)
		result = &stream->m_fragments[index];
	return result;
}

void *function_96e90(long size);

// @retail 0x95bc0
bool unreliable_stream_add_fragment(c_network_unreliable_stream *stream, long sequence, bool reliable, word identifier, void const *data, long size)
{
	if (sequence <= stream->m_fragment_window.oldest)
		return true;
	while (stream->m_fragment_window.newest < sequence &&
		sequence_window_count(&stream->m_fragment_window) < stream->m_fragment_window.capacity)
	{
		long next = stream->m_fragment_window.newest + 1;
		sequence_window_extend(&stream->m_fragment_window, next);
		s_stream_fragment *fragment;
		long index = sequence_window_index(&stream->m_fragment_window, next);
		fragment = 0;
		if (index != NONE)
			fragment = &stream->m_fragments[index];
		memset(fragment, 0, sizeof(*fragment));
	}
	s_stream_fragment *fragment = unreliable_stream_get_fragment(stream, sequence);
	if (fragment)
	{
		if (!(fragment->flags & 1))
		{
			void *block = function_96e90(size);
			if (!block)
				return false;
			fragment->flags |= 1;
			if (reliable)
				fragment->flags |= 2;
			else
				fragment->flags &= ~2;
			fragment->data = block;
			fragment->unknown02 = identifier;
			fragment->size = (byte)size;
			memcpy(block, data, size);
			stream->m_fragment_bytes += size;
		}
		return true;
	}
	return false;
}

// @retail 0x94e90
bool c_network_unreliable_stream::v4(long identifier, s_bitstream *stream, long unused, long reserved_bits)
{
	bool result = false;
	if (identifier != NONE)
	{
		if (sequence_window_count(&m_message_window))
		{
			long previous = NONE;
			for (long sequence = m_message_window.oldest + 1; sequence <= m_message_window.newest; sequence++)
			{
				long index = sequence_window_index(&m_message_window, sequence);
				s_stream_message *message = 0;
				if (index != NONE)
					message = &m_messages[index];
				if (message->flags & 4)
				{
					long delta = sequence - previous;
					stream->checkpoints[stream->checkpoint_count] = stream->bit_position;
					stream->checkpoint_count++;
					if (previous != NONE && delta <= 16)
					{
						if (delta == 1)
						{
							stream_write_bit(stream, true);
							stream_write_bit(stream, true);
						}
						else
						{
							stream_write_bit(stream, true);
							stream_write_bit(stream, false);
							stream_write_checked(stream, delta - 1, 4);
						}
					}
					else
					{
						stream_write_bit(stream, false);
						stream_write_bit(stream, true);
						stream_write_checked(stream, sequence & 0x3ff, 10);
					}
					if (message->flags & 8)
					{
						stream_write_bit(stream, true);
						stream_write_checked(stream, message->unknown02 - 1, 8);
					}
					else
						stream_write_bit(stream, false);
					function_1955d0(stream, message->data, message->unknown02);
					if ((stream->size_in_bytes << 3) - stream->bit_position < reserved_bits + 2)
					{
						if (previous == NONE)
							m_unknown05 = true;
						result = true;
						stream->function_194710(true);
						break;
					}
					stream->checkpoint_count--;
					message->flags = (message->flags & ~4) | 2;
					message->unknown08 = identifier;
					previous = sequence;
				}
			}
		}
		stream_write_bit(stream, false);
		stream_write_bit(stream, false);
	}
	return result;
}

// @retail 0x951c0
long c_network_unreliable_stream::v5(long const *identifier, s_bitstream *stream)
{
	long return_value = 0;
	volatile long result = return_value;
	long previous = NONE;
	if (*identifier != NONE)
	{
		for (;;)
		{
			long capacity = stream->size_in_bytes;
			capacity <<= 3;
			long position = stream->bit_position;
			bool relative = false;
			if (position <= capacity)
				relative = (stream->data[position / 8] & (1 << (position % 8))) != 0;
			stream->bit_position = position + 1;
			long next_position = stream->bit_position;
			bool next = false;
			if (next_position <= capacity)
				next = (stream->data[next_position / 8] & (1 << (next_position % 8))) != 0;
			stream->bit_position = next_position + 1;
			long sequence = NONE;
			if (!relative)
			{
				if (!next)
					break;
				long value = function_1959c0(stream, 10);
				if (stream->mode != 4)
				{
					long delta = value - (m_fragment_window.newest & 0x3ff);
					if (delta > 0x200)
						delta -= 0x400;
					else if (delta <= -0x200)
						delta += 0x400;
					sequence = delta + m_fragment_window.newest;
					if (sequence - m_fragment_window.newest > 0x180 || m_fragment_window.oldest - sequence > 0x180)
					{
						result = 2;
						break;
					}
				}
			}
			else if (!next)
			{
				long delta = function_1959c0(stream, 4);
				if (stream->mode != 4)
				{
					if (previous == NONE)
					{
						result = 3;
						break;
					}
					sequence = delta + previous + 1;
				}
			}
			else if (stream->mode != 4)
			{
				if (previous == NONE)
				{
					result = 3;
					break;
				}
				sequence = previous + 1;
			}
			previous = sequence;
			long partial_capacity = stream->size_in_bytes;
			partial_capacity <<= 3;
			long partial_position = stream->bit_position;
			bool partial = false;
			if (partial_position <= partial_capacity)
				partial = (stream->data[partial_position / 8] & (1 << (partial_position % 8))) != 0;
			stream->bit_position = partial_position + 1;
			long bits;
			long size;
			if (partial)
			{
				bits = function_1959c0(stream, 8) + 1;
				size = (bits + 7) / 8;
			}
			else
			{
				bits = 256;
				size = 32;
			}
			byte data[32];
			function_195820(stream, data, bits);
			if (stream->mode != 4 && !unreliable_stream_add_fragment(this, sequence, partial, (word)bits, data, size))
			{
				result = 2;
				break;
			}
		}
		return_value = result;
	}
	return return_value;
}

// @retail 0x95cf0
void function_095cf0(s_network_stream_header *header)
{
	c_network_reliable_stream *stream = (c_network_reliable_stream *)header;
	stream->m_unknown05 = false;
	long sequence = (dword)(g_network_configuration.value16a8 * network_time_now()) / 1000 & 0xff;
	*(volatile long *)&stream->m_message_window.newest = sequence;
	*(volatile long *)&stream->m_message_window.oldest = sequence;
	*(volatile long *)&stream->m_message_window.head = 0;
	*(volatile long *)&stream->m_message_window.count = 0;
	stream->m_message_window.valid = true;
	sequence_window_reset(&stream->m_acknowledgement_window, 0);
	*(volatile bool *)&stream->m_acknowledgement_window.valid = false;
	*(volatile long *)&stream->m_next_sequence = sequence;
	stream->m_bytes = 0;
	stream->m_unknown950 = 0;
	stream->m_unknown954 = 0;
	stream->m_unknown95c = 0;
	stream->m_unknown960 = 0;
	stream->m_unknown964 = 0;
	stream->m_unknown958 = false;
	stream->m_last_sequence = sequence - 1;
	stream->m_unknown959 = true;
	stream->m_round_trip_minimum = g_network_configuration.value16cc;
	stream->m_round_trip_average = g_network_configuration.value16d0;
	stream->m_round_trip_deviation = g_network_configuration.value16d4;
	stream->m_timeout = g_network_configuration.value16d8;
	stream->m_backoff = 0;
}

// @retail 0x95dc0
bool c_network_reliable_stream::v2(bool *pending)
{
	bool result = false;
	if (m_acknowledgement_window.valid &&
		(m_unknown950 < m_acknowledgement_window.newest || m_unknown954 < m_acknowledgement_window.newest))
	{
		result = true;
	}
	if (!m_unknown959)
		result = true;
	return result;
}

// @retail 0x96c50
void c_network_reliable_stream::advance_acknowledgements()
{
	if (m_acknowledgement_window.valid && sequence_window_count(&m_acknowledgement_window))
	{
		do
		{
			long sequence = m_acknowledgement_window.oldest + 1;
			word *acknowledgement;
			long index = sequence_window_index(&m_acknowledgement_window, sequence);
			acknowledgement = 0;
			if (index != NONE)
				acknowledgement = &m_acknowledgements[index];
			if (!(*acknowledgement & 1))
				break;
			sequence_window_advance(&m_acknowledgement_window, sequence);
		}
		while (sequence_window_count(&m_acknowledgement_window));
	}
}

// @retail 0x95df0
long c_network_reliable_stream::v3(long a, long b)
{
	advance_acknowledgements();
	if (m_acknowledgement_window.valid)
	{
		if (!sequence_window_count(&m_acknowledgement_window))
			return 0x12;
		if (sequence_window_count(&m_acknowledgement_window) <= 9)
			return 0x1a;
		return sequence_window_count(&m_acknowledgement_window) + 0x19;
	}
	return 0x13;
}

// @retail 0x966d0
inline s_reliable_message *reliable_stream_get_message(c_network_reliable_stream *stream, long sequence)
{
	s_reliable_message *result;
	long index = sequence_window_index(&stream->m_message_window, sequence);
	result = 0;
	if (index != NONE)
		result = &stream->m_messages[index];
	return result;
}

// @retail 0x96810
void reliable_stream_set_message_size(c_network_reliable_stream *stream, long sequence, long size)
{
	s_reliable_message *message;
	long index = sequence_window_index(&stream->m_message_window, sequence);
	message = 0;
	if (index != NONE)
		message = &stream->m_messages[index];
	message->size = size;
	stream->m_bytes += size;
}

// @retail 0x96710
long c_network_reliable_stream::read_acknowledgement(long *message_sequence, long sequence, bool valid, long distance)
{
	long const *local_0 = &distance;
	if (!m_acknowledgement_window.valid)
	{
		sequence_window_initialize(&m_acknowledgement_window, sequence - 1);
	}
	long newest = m_acknowledgement_window.newest;
	long delta = sequence - (newest & 0xff);
	if (delta > 0x80)
		delta -= 0x100;
	else if (delta <= -0x80)
		delta += 0x100;
	long acknowledged = m_acknowledgement_window.newest + delta;
	if (valid)
		*message_sequence = acknowledged;
	else
		*message_sequence = NONE;
	m_unknown958 = *local_0 == 0;
	long oldest = acknowledged - *local_0;
	m_unknown954 = oldest > m_unknown954 ? oldest : m_unknown954;
	if (m_acknowledgement_window.newest <= oldest)
	{
		sequence_window_initialize(&m_acknowledgement_window, oldest);
	}
	else
	{
		while (m_acknowledgement_window.oldest < oldest)
			sequence_window_advance(&m_acknowledgement_window, m_acknowledgement_window.oldest + 1);
	}
	if (acknowledged <= m_acknowledgement_window.oldest)
	{
		if (acknowledged != m_acknowledgement_window.oldest || valid)
			return 2;
	}
	return 0;
}

// @retail 0x96860
void c_network_reliable_stream::mark_received(long sequence)
{
	if (sequence != NONE)
	{
		while (sequence > m_acknowledgement_window.newest)
		{
			sequence_window_extend(&m_acknowledgement_window, m_acknowledgement_window.newest + 1);
			word *acknowledgement;
			long index = sequence_window_index(&m_acknowledgement_window, m_acknowledgement_window.newest);
			acknowledgement = 0;
			if (index != NONE)
				acknowledgement = &m_acknowledgements[index];
			*acknowledgement = 0;
		}
		word *acknowledgement;
		long index = sequence_window_index(&m_acknowledgement_window, sequence);
		acknowledgement = 0;
		if (index != NONE)
			acknowledgement = &m_acknowledgements[index];
		if (!(*acknowledgement & 1))
			*acknowledgement |= 1;
	}
}

// @retail 0x96910
long c_network_reliable_stream::function_96910(long sequence, long bit_count, dword const *bits, bool complete)
{
	long result = 0;
	long delta = (m_message_window.newest & 0xff) - sequence;
	if (delta < 0)
		delta += 0x100;
	long acknowledged = m_message_window.newest - delta;
	s_reliable_message *message = reliable_stream_get_message(this, acknowledged);
	long previous;
	if (message)
		previous = acknowledged - message->distance;
	else
		previous = m_next_sequence;
	m_unknown959 = false;
	if (acknowledged <= m_message_window.oldest)
	{
		if (acknowledged < m_message_window.oldest - g_network_configuration.value16ac)
			result = 2;
	}
	else
	{
		long newest_bit = acknowledged - 1;
		for (long current = acknowledged; current > m_message_window.oldest; current--)
		{
			long bit = newest_bit - current;
			s_reliable_message *entry = reliable_stream_get_message(this, current);
			if (!(entry->flags & 1))
			{
				bool delivered;
				if (current == acknowledged)
					delivered = true;
				else if (bit < bit_count)
					delivered = (bits[bit >> 5] & (1 << (bit & 31))) == 0;
				else
				{
					delivered = true;
					if (current <= m_next_sequence && current <= previous)
						delivered = false;
				}
				if (delivered)
				{
					long start = entry->time;
					entry->flags |= 1;
					long type;
					if (entry->flags & 8)
						type = 5;
					else
						type = 4;
					entry->unknown08 = network_time_now() - start;
					update_round_trip(type, entry->unknown08, current, network_time_now());
				}
			}
		}
	}
	if (acknowledged == m_message_window.newest && m_next_sequence == m_message_window.newest && complete)
		m_unknown959 = true;
	return result;
}

// @retail 0x96360
long c_network_reliable_stream::v5(long const *identifier, s_bitstream *arg_1)
{
	long *arg_0 = (long *)identifier;
	long local_0 = 0;
	if (v1(0))
		return 2;
	bool local_1 = function_1957d0(arg_1);
	long local_2 = function_1959c0(arg_1, 8);
	long local_3 = function_1959c0(arg_1, 7);
	if (arg_1->mode == 4)
		*arg_0 = local_1 ? m_message_window.newest : NONE;
	else
	{
		local_0 = read_acknowledgement(arg_0, local_2, local_1, local_3);
		if (local_0)
			return local_0;
	}
	long local_4 = function_1959c0(arg_1, 8);
	bool local_5 = false;
	bool local_6 = function_1957d0(arg_1);
	bool local_7 = function_1957d0(arg_1);
	dword local_8[4];
	memset(local_8, 0, sizeof(local_8));
	long local_9;
	if (!local_6 && !local_7)
	{
		local_9 = 0;
		local_5 = function_1957d0(arg_1);
	}
	else if (local_6 && !local_7)
	{
		local_9 = 0;
		for (long local_10 = 0; local_10 < 8; local_10++)
		{
			if (function_1957d0(arg_1))
			{
				local_8[0] |= 1 << local_10;
				local_9 = local_10 + 1;
			}
		}
	}
	else if (!local_6 && local_7)
	{
		long local_10 = function_1959c0(arg_1, 3);
		local_8[0] |= 1 << local_10;
		local_9 = local_10 + 1;
	}
	else
	{
		if (function_1957d0(arg_1))
			return local_0;
		local_9 = function_1959c0(arg_1, 7);
		for (long local_10 = 0; local_10 < local_9; local_10++)
		{
			if (function_1957d0(arg_1))
				local_8[local_10 >> 5] |= 1 << (local_10 & 31);
			else
				local_8[local_10 >> 5] &= ~(1 << (local_10 & 31));
		}
	}
	if (arg_1->mode != 4)
		return function_96910(local_4, local_9, local_8, local_5);
	return local_0;
}

// @retail 0x96d80
void c_network_reliable_stream::update_round_trip(long type, long round_trip_time, long sequence, long time)
{
	if (type == 1 || type == 4 || type == 5)
	{
		m_unknown95c = time;
		m_unknown960 = round_trip_time;
		m_unknown964 = sequence;
		m_last_sequence = sequence > m_last_sequence ? sequence : m_last_sequence;
		m_round_trip_minimum = m_round_trip_minimum > round_trip_time ? round_trip_time : m_round_trip_minimum;
		long average = m_round_trip_average;
		long error = round_trip_time - average;
		m_round_trip_average = (error >> g_network_configuration.value16b8) + average;
		m_round_trip_deviation = ((abs(error) - m_round_trip_deviation) >> g_network_configuration.value16bc) + m_round_trip_deviation;
		long variance = m_round_trip_deviation * g_network_configuration.value16c0;
		m_timeout = (variance <= g_network_configuration.value16c4 ? g_network_configuration.value16c4 : variance) + m_round_trip_average;
		m_timeout = m_timeout <= g_network_configuration.value16c8 ? g_network_configuration.value16c8 : m_timeout;
		m_backoff -= g_network_configuration.value16e4;
		m_backoff = m_backoff <= 0 ? 0 : m_backoff;
	}
	else if (type == 3)
	{
		m_backoff += g_network_configuration.value16dc;
		m_backoff = m_backoff > g_network_configuration.value16e0 ? g_network_configuration.value16e0 : m_backoff;
	}
}

// @retail 0x96b00
bool c_network_reliable_stream::get_next_send(long *type, long *sequence, long *size, long *time)
{
	long *const *type_reference = &type;
	long next = m_next_sequence + 1;
	s_reliable_message *message;
	long index = sequence_window_index(&m_message_window, next);
	message = 0;
	if (index != NONE)
		message = &m_messages[index];
	bool result = false;
	if (message)
	{
		word flags = message->flags;
		if (flags & 1)
		{
			**type_reference = (flags & 2) ? 2 : 1;
			*sequence = next;
			*size = message->size;
			*time = message->unknown08;
			message->flags |= 4;
		}
		else
		{
			long local_1 = message->time;
			long elapsed = network_time_now() - local_1;
			if (elapsed >= m_timeout + m_backoff || m_last_sequence - next >= g_network_configuration.value16b0)
			{
				message->unknown08 = elapsed;
				**type_reference = 3;
				*sequence = next;
				*size = message->size;
				*time = message->unknown08;
				message->flags |= 8;
				network_time_now();
				m_backoff += g_network_configuration.value16dc;
				m_backoff = m_backoff > g_network_configuration.value16e0 ? g_network_configuration.value16e0 : m_backoff;
			}
		}
		word pending_flags = message->flags;
		if (pending_flags & 0xc)
		{
			result = true;
			pending_flags |= 0x10;
			message->flags = pending_flags;
			long remaining_bytes = m_bytes;
			m_next_sequence = next;
			remaining_bytes -= message->size;
			m_bytes = remaining_bytes;
		}
	}
	return result;
}

/* The result copy is repeated inside the scan arm: with one copy the compiler keeps it in ECX for the two
   early exits, where retail reads the type again at every exit (0x96678, 0x966ad, 0x966b8). */
// score 35 (was 35): The message and flag word use EAX and CX. Retail uses ECX and AX and sets the bit in memory.
// MiniMax C (11-20): no new form, no lower score (35): the only shape that gives retail's message register, flags register and byte OR is the second result variable recorded in the notes above (10 at retail's 225 bytes), left aside because it repeats the same assignment in both arms of the send test; what is still left is the order of the last two exit blocks (ours lays the send-and-empty-window exit ahead of the loop's zero-trip exit, retail after it).
// Earlier detail: The message and saved flags use EAX and CX. Retail uses ECX and AX and updates the low byte
// in place.
// Earlier score 35 (was 35): earlier 60. The message pointer and flag word still use EAX/CX, and the found path writes the saved word.
// The message pointer and flag word still use EAX/CX, and the found path writes the saved word.
// Earlier: retail keeps the message in ECX and the flags in AX (ours EAX / CX) and ends the found path with
// `or byte ptr [ecx + 0xe], 2` (ours `or ecx, 2` and a word store); m3 lane 3 r257's entry is the last one above the marker.
// Earlier first line: Retail uses one sequence counter and reloads newest in the index helper.
// Round 112 (m3 lane 10), NOT kept (coordinator 11-16): 44 needs the loop bound re-read as `m_message_window.newest` in
// place of the named `newest` local (with the local 117), a re-read replacing a named local, which is kept only with a
// match. The 44 form (fast, plain-confirmed with the file's 26 matched functions): the shared index
// helper reads oldest into a local (`long oldest = window->oldest;`, used by its test and its offset), the window pointer
// is declared ahead of the send call and serves the count too, and the loop test reads `m_message_window.newest` each
// pass (no bound local): the offset is then recomputed per pass (no second counter) and the count's oldest is shared
// with the loop, as retail. Left: the count's newest is kept in EBP (pushed before the count) and the helper's newest
// test reads that register, where retail does the count in ECX/EAX, reloads the bound into EBP after the test and
// re-reads newest per pass; next sits in ECX (retail ESI). Tried this round: the helper local with the member bound
// but the count through `&m_message_window` 89 (221 bytes; the count folds oldest and the loop reloads it); the kept
// form with the bound read through `window->newest` in the test 117 (197 bytes), or in a `long newest = window->newest`
// local 117; member access everywhere with the helper local 89; that with the existing reliable_stream_get_message(this,
// next) 89 (same code); the window declared after the send call in a nested if 44 (same code).
// MiniMax I (11-16): no new form, no lower score (60): the first difference is the loop counter's home - retail computes it in
// ESI (mov esi, [edi+0x44]; inc esi) where this build computes it in ECX and copies it into ESI at the loop
// head; the permuter (300 s, 5 tries) found no spelling that keeps it in ESI.
// MiniMax R (11-15): no new form, no lower score (60): the offset next - oldest - 1 is still strength-reduced into a second counter in esi with the next sequence in ecx, where retail keeps the next sequence in esi and recomputes the offset in eax on every pass; giving the index helper the member reproduces retail's loop body but moves the type local off its slot (the get_next_send setup pushes time in ebp) and the score rises to 89; the statement permuter over this body found no lower score either.
// MiniMax G (11-14): no new form, no lower score (60): ours strength-reduces next - oldest - 1 into a second counter
// (next in ecx, the reduced offset in esi) where retail keeps next in esi and recomputes the offset on every pass.
// The member form (the count, the bound and the index all through &m_message_window) reproduces retail's loop body
// but scores 89: the count folds oldest into its `sub` and the loop reloads it instead of keeping it in ebx, and the
// pointer form keeps oldest in ebx while the offset is reduced. No spelling gives both.
// MiniMax S (11-14): no new form, no lower score (60): this build still strength-reduces next - oldest - 1 into a second counter in ESI where retail keeps the next sequence in ESI and recomputes the offset in EAX on every pass, and it still compares the offset against the cached newest where retail re-reads the newest at the per-step test; the member form through &m_message_window reproduces retail's loop body but scores 89 (the count folds oldest into its subtraction and the loop reloads it instead of holding it in EBX), while the pointer form keeps oldest in EBX and reduces the offset, so no spelling of the window gives both.
// MiniMax C (11-15), on its not-kept 44 form (below; main stays 60): the count spelled as the window's newest minus its oldest and the index helper
// taking the window's oldest into a local of its own stop the strength reduction of next - oldest - 1, so the offset is
// recomputed in eax on every pass and the newest is re-read at the per-step test as retail does; what still differs is the
// loop head's registers (ours keeps the oldest in esi and the next sequence in ecx and copies the bound into ebx, retail
// keeps the next sequence in esi and the oldest in ebx) and the count, which retail computes from its own loads of newest
// and oldest before the bound is loaded.
// MiniMax E (11-16): no new form, no lower score (60): retail keeps the next sequence in ESI and recomputes the message offset each pass where this build folds it into the counter; the sequence window helpers are shared.
// Round 116 (m3 lane 9, fast with 0x96510, 0x94df0 and 0x94bf0): the flag test as nested ifs 60
// (same code as the listed `continue` form, so a re-measure). Still only the strength-reduced next - oldest - 1 (next
// in ECX, the reduced offset in ESI) against retail's per-pass offset with next in ESI. No untried natural form is left.
// MiniMax H (11-12): no new form, no lower score (60): the offset next - oldest - 1 is still strength-reduced into a second counter in esi where retail keeps the next sequence in esi and recomputes the offset in eax on every pass.
// MiniMax B (11-11): no new form, no lower score (60): ours keeps the next sequence in ECX and strength-reduces
// next - oldest - 1 into a second induction variable in ESI that is tested against the cached newest, while retail
// keeps the next sequence in ESI, recomputes the offset in EAX on every pass and re-reads the newest at the
// per-step test, holding the message pointer and the count scratch in ECX where we hold them in EAX; the
// prologue, the count and the found-path stores match retail byte for byte; the forms tried (the member-form
// index and bound, a local window count, the index inlined, the index written as statements, the window taken
// by reference and the statement-order permuter) all score 63 or worse.
// MiniMax A (11-11, run 58): score 60 (was 60): no lower score; retail keeps the next sequence in ESI and
// recomputes next - oldest - 1 in EAX on every pass, ours keeps the next sequence in ECX and strength-reduces the
// offset into a second induction variable in ESI; retail reads the newest three times (the count, the loop bound
// and the per-step test) and the oldest once, and with the window pointer declared before the send call for the
// count, the bound and the index (88, 235 bytes) our code reads the newest the same way but still reduces the
// offset and keeps the next sequence in ECX.
// Earlier at 60 (was 60): ours strength-reduces next - oldest - 1 into esi (next in
// ecx) and compares next with the cached newest; retail keeps next in esi,
// recomputes the offset and re-reads window->newest each step (as 0x94df0 does).
// Passing &m_message_window to sequence_window_index (or declaring window inside
// the loop) gives retail's loop body, but oldest is then reloaded instead of
// reused from the count and time is cached in ebp (89, 221 bytes); the bound
// read in the condition with window kept (63). Tried every mix of window /
// &m_message_window for the count, the bound (local, window->newest or
// m_message_window.newest in the condition) and the index: 60, 63, 80, 88-91;
// a const window (60); reliable_stream_get_message (a call, 165).
// Round 178 (m3 lane 4, fast): the 63 form (window pointer for the index, the bound read in the condition) is
// retail's code but for one thing: ours keeps `next - oldest - 1` as a second counter in esi (next in ecx), retail
// recomputes it each pass with next in esi. With &m_message_window at the index there is no second counter, but the
// hoisted oldest is reloaded, next sits in ecx and `time` takes ebp (type then reuses its home, no `push ecx`).
// Tried: the sibling 0x94df0's loop (bound in the condition, &m_message_window) 89; the stores after the loop
// through a found pointer 91; `return 4` in the loop 129 (187 bytes); an oldest local for the count 91;
// reliable_stream_get_message now inlines 89; a second read of oldest ahead of the loop (diagnostic) 86, still
// reloaded; `while (++next < bound)` with the direct window 89, with the pointer 63, with a newest local 89;
// newest declared ahead of the window pointer 63; a newest local with the direct window 89; a guarded do-while 89.
// Round 183 (m3 lane 4, fast; aligned/register-masked difflib counts 33/13 at 60): the bound read in the loop test
// with the window pointer for the index re-measures 60 (same code as the body below); the newest local with
// &m_message_window at the index re-measures 89 (retail's loop body; the count folds oldest into its `sub` and the
// loop hoists a second read into ESI, so `time` takes EBP). New: the window pointer declared before the
// get_next_send call for the count, the bound and the index 80 (aligned 40, register-masked 11; the count's newest
// and oldest are then both kept, EBX/EBP), with the bound from m_message_window.newest 88 (235 bytes), with the bound
// in the loop test 80; the `&&` written as nested ifs with the window between them 80 (same code as the top window),
// with &m_message_window at the index 91; the top window for the count with &m_message_window at the index 91; the
// found path after the loop (`break`, then `if (next < newest)`) with the window index 72 (229 bytes),
// &m_message_window index 86, bound in the test 97 (204 bytes). Retail shares only oldest between the count and
// the loop and reads newest three times (count, bound, index test); no spelling here separates them that way.
// m3 lane 7 r146 (fast; masked = registers and stack offsets masked, control 60 masked 23): the bound read as
// m_message_window.newest in the condition with window kept for the index (the 63 above) has masked 16 at 226 bytes:
// retail's per-step newest re-read, the oldest reused from the count in EBX and retail's frame. Only ours
// strength-reduces next - oldest - 1 into its own induction variable (next in ECX, retail ESI with the offset
// recomputed each pass). The siblings' &m_message_window form drops the reduction but hoists oldest apart from the
// count (89 at 221 bytes, masked 16; time cached in EBP and type packed into its dead home). On the 63 form: next
// declared at function scope 63, a while loop 63, a const window 63, window declared in the loop 89 (all masked 16).
// `return 4` in the loop on the current body 108 (194 bytes), on the sibling form 129 (187 bytes). Matched 0x94df0
// reuses oldest through its loop start (oldest + 1), which this loop does not have.
// M3 lane 13 r75 (fast): reliable_stream_get_message is `inline` now: with the cached bound, the member bound or a bound local 89 each
// (221 bytes; the same code as the member-window forms: retail's loop body, but `next` sits in ECX, which frees EBP for the `time`
// pointer across get_next_send, and the count's oldest is reloaded; retail keeps `next` in ESI and the bound in EBP); on the member
// form the flags read from the field at each test 89, `type = 4` before the stores 104 (209 bytes), the cursor declared before the loop 89.
// m3 lane 11 r88 (fast): on the 63 form (window pointer for the count and the index, `m_message_window.newest` in the loop test)
// ours has retail's hoisted bound in EBP and its per-step newest re-read; the strength-reduced `next - oldest - 1` is all
// that is left. The shared index helper's offset as `sequence - (window->oldest + 1)` 60 on this body, as `(sequence - 1) -
// window->oldest` 63 on the 63 form; on the 63 form `s_sequence_window *const window` 63 and `message` declared before the
// loop 63. All compile to the same code as before (the reduction does not depend on these spellings); 0x94bf0 stays at 2.
// Permuter on the 63 form (600 s, 68 variants): best 62, its baseline. On the 63 form: `return 4` in the loop 107 (195 bytes);
// the flags read at each test 63 (same code); `message` as `index != NONE ? &m_messages[index] : 0` 60 (masked 20, the
// reduction stays); the found test as `(flags & 3) == 1` 77 (223 bytes); the count as `newest != oldest` 78 (223); `long
// flags` 63 (masked 18).
// m3 lane 2 r70 (fast; control 60, masked 24): re-measured the member window everywhere 89 (221 bytes, masked 17: retail's loop
// body, next ECX, oldest reloaded into ESI, bound EBX, `time` cached in EBP) and `window->newest` in the loop test 60 (same code).
// On the member form: the found path's `*time` through `long *const *time_reference = &time;` 89, that read at the get_next_send
// argument 89, `message` declared before the loop 89, `next` at function scope 89, both 89 (all the same code). Retail keeps all
// four callee-saved registers on loop values (next ESI, the count's oldest EBX, bound EBP), so nothing caches `time`.
// M3 lane 10 r94 (fast, 0x96510/0x94df0 listed; base 60, aligned 42, masked 23): get_next_send's success as an early
// `return type` 79 (213 bytes), on the 63 form 78; the message pointer as a ternary 66 (224 bytes); `message` initialized to 0
// at its declaration ahead of the index 75 (222 bytes); the flags tested through the field at each test 60, `index` declared
// ahead of the loop 60 (both the same code); re-measured the 63 form 63 (with the flags through the field 63, same code). Still
// only the second induction variable differs on the 63 form (next - oldest - 1 kept in ESI with next in ECX).
// MiniMax R (11-11): no new form, no lower score (60): the offset next - oldest - 1 is strength-reduced into a
// second counter in esi (the next sequence in ecx) and the newest is compared against the hoisted register, while
// retail keeps the next sequence in esi, recomputes the offset in eax on every pass and re-reads window->newest at
// the per-step test; with &m_message_window passed to the index helper the offset is recomputed per pass, but then
// the frame and the register choice differ (89). The rest of the function agrees.
// m3 lane 2 r75 (fast, 0x96510/0x94df0 listed): the found path left in the loop (`type == 0 &&` in the loop test, no
// break) 80 (228 B: no second counter, but oldest is re-read every pass); the found test as `if (!(flags & 1) ||
// (flags & 2)) continue;` 60 (same code); the shared index helper summing `first + sequence - oldest - 1` 95 (219 B,
// and 17 matched users of the helper in this file lost); the count held in a named local, on this body 60 and on the
// member-window form 89 (same code each). Re-measured the member-window form 89: its count folds oldest into `sub eax,
// [edi + 0x34]` and the loop loads oldest again. Matched 0x94df0 has retail's exact loop (no second counter, the count's
// oldest kept for the index) because its loop start reads oldest (`oldest + 1`); this loop starts from m_next_sequence.
// MiniMax A (11-12): no new form, no lower score (60); the matched sibling 0x94df0's spelling (the bound read from
// the member in the loop condition and the index helper given &m_message_window) reproduces retail's loop body, with
// the offset recomputed on every pass and the newest re-read from memory, but the head and the register choice then
// differ (89); with the bound in a local and the helper given the member the head matches retail and the loop body
// does not (60). The strength reduction into the second counter is what frees ecx for the next sequence.
// MiniMax A (11-12, later): still no lower score (60); the index written out in the function with the oldest taken as
// a value (so the count keeps retail's mov/sub and the oldest in eax) still strength-reduces the offset into esi and
// still reads the newest from the hoisted bound (58); the member form keeps retail's loop body but folds the count's
// oldest into its sub, reloads the oldest in the loop and caches time in ebp (89); retail keeps the oldest in ebx from
// the count and still recomputes the offset, so only the strength reduction and the register choice are left.
// MiniMax A (11-12, third): still no lower score (60); handing the index helper the member (&m_message_window) kills
// the strength reduction and the loop body then matches retail's instruction for instruction, but the head and the
// register choice change with it (89): the next sequence stays in ecx where retail keeps it in esi, and retail reads
// the newest from memory inside the helper while this build reads it from the hoisted bound; writing the index out
// in the function with the member read gives 60 again, so the strength reduction is the whole of the difference.
// Round 175 (m3 lane 6, fast; control 60): the bound local read from the member (`m_message_window.newest`) with the
// window pointer kept for the index 63 (226 bytes; the same code as the listed 63 form); `message` as an if/else
// (`if (index != NONE) ... else message = 0;`) 66 (224 bytes; the same code as the listed ternary); a local copy of `this`
// for the index helper (`&stream->m_message_window`) or for the bound 89 each (the copy propagates away: the listed
// all-member code); `message` declared after the index with its zero initializer 60, `short flags` 60, the bound declared
// in the for initializer after `next` (`for (long next = ..., newest = window->newest; ...)`) 60 (all three the same code).
// M3 lane 13 r89 (fast with 0x96510/0x94df0, control 60): the found path's flag update through the flags local (`message->flags
// = flags | 2;`) 68 (231 bytes); the window helpers `__forceinline` 60 (same code); re-measured: the send and count tests as
// an early `if (get_next_send(...) || !count) return type;` 79 (213 bytes, the listed early return), the count alone as an early
// return inside the send test 79 (same code); the send result in a named bool 60; the start as `long next = m_next_sequence; for (next++; ...)` 60 (both the
// same code); the flag test with bit 2 first 60, `*time` stored before `*size` 60 (both change the code within lines that already
// differ); the flag update ahead of the found path's three stores 62; `*sequence` stored last 62. Diagnostics (behaviour
// changed, reverted): two values kept live across the loop 113/145 (they spill and the second
// counter stays: not register pressure); the loop started from `window->oldest + 1` 73 (still reduced). With the member window
// at the index, the loop's hoisted oldest is never merged with the count's read: a store of oldest before the loop 88, the start
// from oldest 101 (also with the bound in the test, 0x94df0's loop shape), the found path after the loop 86 (no stores in the
// loop), that with the start from oldest 86 and the bound in the test 111. Only a read of oldest in the loop's own preheader
// block (a clamp of the start against oldest, 79) merges with it. So the window pointer merges early and reduces, the member
// view never merges; retail (and matched 0x94df0) merge without reducing, which no view, loop shape or found-path placement
// here reproduces.
// M3 lane 4 r196 (fast with 0x96510/0x94df0; control 60). A one-TU LTCG link harness (build/r196/h/h965.py of m3-lane4, the
// function with the window helpers, get_next_send in a plain TU) reproduces 60, 63 and 89 and screened 96 spellings: count,
// bound and index each through the window pointer or the member, the found path as break, return or after the loop, and the
// loop as `!=`, `<= newest - 1`, `while (++next < newest)` and a dword counter. Every spelling whose index reads oldest
// through the window pointer strength-reduces `next - oldest - 1`; every member spelling reloads oldest apart from the
// count. Real: the loop as `while (next < newest) { ...; next++; }` 60, `long next;` ahead of the for 60, the start named
// `long first` 60, the flag update as `message->flags = message->flags | 2;` 60 (all the same code), `type = 4;` ahead of
// the found path's stores 79 (213 B).
// P472: Actual word-bit field first/second/both, explicit Boolean first/second/both, second-first and paired outcomes: 60/60/60/66/74/67/68/67.
// NOT kept: no exact native induction/allocation shape; original raw word layout, helpers, complete body and header restored.
// M3 lane 6 r179 (fast with 0x96510, 0x94df0, 0x94bf0, 0x96b00; control 60): `m_next_sequence` read into a local at the top,
// ahead of get_next_send, 82 (229 bytes); the message array taken into a pointer local at the top 77 (228 bytes), inside
// the block 78, inside the block with the bound read from the member in the loop test 78 (229 bytes); `next > oldest`
// tested ahead of the index helper with a named oldest (diagnostic, an implied test) 77 (230 bytes). The reduction stays.
// M3 lane 13 r93 (fast with 0x96510/0x94df0, both stay matched; control 60): dead-local phase scan (n = 0..7, removed) 60 at
// every n; the start `long next = m_next_sequence + 1;` read between the send test and the count test (nested ifs) 64; the
// lookup through the existing reliable_stream_get_message 89 (221 B: the member-view loop body, retail's, but the count
// folds oldest into `sub eax, [edi + 0x34]` and the preheader reloads it); the flag test as nested ifs 60 (same code); the
// window pointer declared ahead of the send call for the count only, the index through the member, with the bound in a
// local / in the loop test / the count in its own inner if 91/89/91 (221 B). Matched 0x94df0 reuses the count's oldest
// because its loop start reads oldest; here no spelling merges the count's oldest without the early merge that reduces.
// P500: Existing shared index-helper lower/upper guard and result graphs, eight complete forms:
// 76/86/86/86/91/87/60/60. NOT kept: Strength reduction remains or native extent/register ownership regresses; no
// exact result. Shared helper and every caller restored. Original complete executable body restored.
// M3 lane 10 r108 (fast with 0x96510, 0x94df0, 0x94bf0, 0x96b00; control 60): the window pointer declared ahead of the send
// call with the count read through it 80 (223 B), with the bound read through it in the loop test 80 (same code); the
// member form (re-measured 89) with the count written as `newest - oldest` on the member 91 (the count's newest joins the
// bound, oldest reloaded in the preheader); the count test as `> 0` 62 (227 B); a named oldest with `if (next <= oldest)
// continue;` at the loop top (diagnostic, implied by the helper) 82 (230 B). Same code: a guarded do-while, a const window
// pointer, `(flags & 2) == 0`, the message pointer declared with its zero as `m_messages + index`, the window pointer
// declared inside the loop body (the member form's code).
// m3 lane 14 r96 (fast with 0x96510, 0x94df0, 0x94bf0 2, 0x96b00; control 60): in retail `next` lives in ESI, saved
// with `push esi` ahead of the loop guard (EBX is pushed only inside it), where every form here keeps it in ECX. `next`
// declared ahead of the window pointer (`long next = m_next_sequence + 1;`, then `for (; next < newest; next++)`) 60, on the
// 63 form 63, on the member-window form 89; `next` declared at function scope ahead of `type` and assigned in the block 60;
// the member form started as `long next = m_next_sequence; for (next++; ...)` 89 (all the same code as their base forms:
// the declaration position does not move `next` into ESI or stop the second counter).
// M3 lane 13 r97 (one-file /GL link harness build/r97/h, ~1 s a variant, 450+ spellings; real builds fast with 0x96510 and
// 0x94df0, both MATCH): with the index through `&m_message_window`, the count's oldest is kept for the loop (no second
// counter) only when the count is a comparison: `newest != oldest`, `oldest != newest`, `oldest < newest`, `newest >
// oldest`, or `sequence_window_count(&m_message_window) > 0`; every `!= 0` or truth-value count (helper, direct
// difference, unsigned `> 0`, which folds to `!= 0`) folds oldest into the `sub` and the loop reloads it. With the
// bound through a window pointer the bound is re-read as retail, and `count > 0` gives retail's separate loads, `sub`,
// kept oldest and loop, but `test`/`jle` for retail's `je`. Real builds: `newest != oldest` with member bound and index
// 102 (212 B); `oldest != newest` with the window-pointer bound 101 (213 B); `count > 0` with the window-pointer bound
// in the test or in a local 91/91 (221 B); that with `return 4` in the found path 127 (190 B). In all of them `next`
// takes ECX (retail ESI) and `time` is then cached in EBP for the get_next_send argument and the found path.
// MiniMax C (11-16), NOT kept: 44 (was 60) with sequence_window_count expanded in place (window->newest -
// window->oldest), the loop bound re-read as m_message_window.newest instead of the newest local, and an oldest
// local in the shared sequence_window_index helper. Expanding an existing accessor is never kept; the helper's
// local alone gives 90 (fast).
// M3 lane 2 r92 (fast with 0x96510/0x94df0, control 60): the window as a const reference (`s_sequence_window const &`)
// for the bound local and the index 60 (same code); declared ahead of the send call and used for the count, the bound
// and the index 80 (223 B: the count's newest and oldest both kept, EBX/EBP); with the bound read from it in the loop
// test 60 (same code: the reference read joins the index helper's newest, both hoisted); that with the count as `> 0`
// 62 (227 B); the current body with the count as `> 0` 62 (227 B). Diagnostic: probe stores of 1 / -1 at the top 92
// (237 B), the second counter stays. Retail's shape reads as: the helper's first (unconditional) oldest test hoisted and
// merged with the count's load, its second (conditional) newest test left in the loop, and no reduction.
// P587: Existing shared head-selector empty/nonempty early exits, complete result branches,
// named empty/occupied predicates and head-first override/conditional: 60/60/60/60/60/60/74/60.
// NOT kept: no native induction repair; nonempty/override forms also regress shared users.
// Original head/index helpers and complete executable body restored.
// M3 lane 3 r254 (fast with the file's 28 functions; control 60, 0x94bf0 at 2 throughout, the rest MATCH): the window
// pointer declared ahead of the send call and passed to the existing count helper (`sequence_window_count(window)`, not
// written out), `long oldest = window->oldest;` named in the shared index helper (test and offset), the index through the
// window pointer and the bound read in the loop test as `m_message_window.newest` 44 (226 B; lead, not kept: the bound's
// member read beside the window pointer is a pointer/member mix); that with the helper's newest named too 44 (same code);
// the count through `&m_message_window` with the window pointer for the index only 89 (221 B); everything through
// `&m_message_window` with the helper's oldest named 89 (221 B: the loop body is retail's, the newest test reads memory
// each pass and no second counter, but the count folds oldest into `sub eax, [edi + 0x34]` and the preheader reloads it);
// the 44 form with main's `long newest = window->newest;` bound 117 (197 B); the 44 form with the index through
// `&m_message_window` 89.
// M3 lane 7 r173 (fast with 0x96510/0x94df0, control 60): `long next = m_next_sequence; while (++next < newest)` 60; the
// shared window-head helper as `count != 0 ? head : NONE` 60 (and 15 of the file's matched users lose: reverted at once);
// the window pointer declared at the top (before the send call) for the bound local and the index, the count through
// the member, 60; the bound compare written `newest > next` 60; a guarded do-while (`if (next < newest) do {...} while
// (++next < newest);`) on the window-pointer form 60. All the same code: the offset is still strength-reduced.
// Reference inputs for the existing count, index and head helpers, singly and
// in all combinations, 60 each. All three with the existing message lookup 89.
// The reference forms retain the second induction variable. The lookup restores
// per-pass subtraction but reloads oldest and changes the saved registers.
// Window references in index/count/first helpers, seven subsets, and all three with the loop
// limit read at use: 60 each. NOT kept: the latest-sequence load and reduced offset remain.
// Round 401 tested previous-sequence for loop, successor-before-step for loop, previous-sequence while, successor at both uses, then offset/head-accumulation forms on each scan: 77/113/77/70/60/60/77/77.
// The per-pass offset and newest reload remain. Original body and index helper restored.
// One-based displacement with prior head, head-first accumulation, displacement-
// first accumulation 73/95/73; displacement from first sequence or in-place
// decrement 60/60; unsigned displacement, prior head or origin 60/73/95.
// Head-first with shared count/index pointer and cached/member/live bound
// 118/76/118. NOT kept: the single-cursor forms miss native count reuse or
// the upper-bound reload. Original index helper restored.
// M3 lane 3 r257 (fast with 0x96510, 0x94df0, 0x94bf0 and 0x96b00; kept form plain-confirmed with the file's 28
// functions: 0x94bf0 at 2, 26 MATCH). KEPT 60 -> 35 (230 B): matched 0x94df0's loop spelling (the bound read as
// `m_message_window.newest` in the loop test, the index through `&m_message_window`) with the found path's flag update
// written through the flags local (`message->flags = flags | 2;`). The flags value then stays live through the found
// path, three scratch registers are busy there, `next` moves to ESI, nothing caches `time`, and the count's oldest stays
// in EAX for the loop: the prologue, the count, the guard, the loop (but for two registers) and the found path's three
// stores are retail's. Retail's own found path uses only EDX and ESI for its temporaries although no instruction there
// reads EAX, so a third value is live in EAX (where the flags were read) through it; the statement that keeps it live
// while the update stays `or byte ptr [ecx + 0xe], 2` is what is still missing.
// Tried this round (control 60): the member form with the count as `> 0` 91 (221 B: the count's oldest shared, in EDX,
// `test`/`jle`); `type = 4;` between the three stores and the flag update on the old body 79 (213 B: the store stays and
// the return reloads it), on the member `> 0` form 94 (209 B); `return 4` in the loop re-measured 108 (194 B: all four
// pushes at entry); the member `> 0` form with the flags-local update 47 (231 B), with that update ahead of the three
// stores 51 (231 B); the old window-pointer body with the flags-local update 68 (231 B); the kept form with `flags |=
// 2; message->flags = flags;` 35 (same code); flags as a `word &` to the field with `flags |= 2`, as a `word *`, the
// reference for the tests only 89/89/89 (all fold back to the member form); a found bool set first or last in the found
// path with `if (found) type = 4;` after the loop 91/91 (threaded away); the kept form with the lookup through the
// existing reliable_stream_get_message 35 (same code), that with the plain `|=` update 89; `long flags` with a word
// cast at the store 35, `short flags` 35; flags declared ahead of the message 35, at function scope 35, renamed to sort
// first 35 (diagnostic); the message as a ternary initialiser 45 (227 B: the message in ECX, the flags in AX and the
// found path's stores as retail, but the lookup's blocks change), as an if/else 45 (same code), zero-initialised ahead
// of the index 63 (224 B), after the index 35; the flags-local update ahead of the three stores 39.
// Diagnostics, reverted at once: a call in the found path 74 (242 B: `next` in a callee-saved register gives retail's
// head in ECX and flags in AX); the no-alias optimize pragma with `type = 4;` first 104 (209 B), alone 89 (the one
// compiler-setting trial); `__assume((flags & 1) != 0)` closing the found path 89 (no effect); a local `flags |= 2;`
// after or ahead of the memory `|=`, or `flags = message->flags;` after it, 89 each (removed before allocation, so
// the member form's code).
// Count-based bound with the index helper through pointer/member/reference or
// the message getter: reconstructed newest 88/91/88/91; wide distance from
// oldest 172/165/172/165. NOT kept: native loads and single cursor remain
// different, and all eight extents move away from 225 bytes.
// Scan selects the entry before publishing the outputs. Index with live cursor 83/67,
// message with saved sequence 91/35, saved index and sequence 83/67, loop test on
// pending message with one-past cursor 96/84. Each pair sets the bit in place or
// writes the saved flag word. NOT kept: the in-place forms still lose the retail
// message/flag registers. The message and saved sequence tie only with a word store.
// In-place resend update with size/time/both captured before sequence publication 90/90/90.
// Both captured with saved flags 88. Single exit from the found path with saved/in-place flags 35/89.
// Initial send to single exit 89. Complete eligibility with in-place update 89.
// NOT kept: the in-place update loses the native opening allocation.
// Separate query and resend types: early snapshot 81, conditional return 111, late
// assignment 35, nested query/count guards 81. With an in-place flag update 93/129/91/93.
// NOT kept: the result split ties or changes the entry and loses the native oldest reuse.
// Resend bit updated through byte/word/bool fields with saved word tests 86/89/86;
// sent from word and resend from field 88; sent from field and resend from word 88; both fields 84;
// packed test with byte field update 88; word fields with Boolean tests 88. NOT kept: the word field
// recovers the memory OR, but the prologue, oldest load and message register differ.
// Pending-bit exclusion first 36; sent gate with pending continue 89;
// combined mask with array base 48; separate resend type 129;
// selected resend return 124; byte memory OR 86; combined mask with byte OR 88;
// low-byte assignment from saved flags 34. NOT kept: gains one point and
// still assigns from the flag snapshot instead of ORing the bit into memory.
// Absent-index branch 90/43, absent-index conditional 90/43, table base in the valid
// branch 89/35, table base before validation 90/45 (in-place update / saved word).
// NOT kept: the saved-word absent-index forms recover ECX/AX but change the lookup
// branches and still write the full word. The in-place update loses the opening allocation.
// In-place resend bit with conditional selection, invalid-first selection, a scan-scope message,
// or a scan-scope array base 90/90/89/67. With the three output lvalues bound 90/90/89/67.
// NOT kept: the array base preserves the query setup and restores the byte memory OR, but
// moves the cursor and oldest to different registers. The other forms also change the query setup.
// M3 lane 16 r137 (fast with 0x96510/0x94df0; control 35): on the member update (`message->flags |= 2;`, 89: `time` is
// loaded into EBP at entry and its home reused for `type`, `next` takes ECX): `*time` written through a pointer to the
// parameter's home 89 (the indirection folds); `next` declared at function scope 89; the update ahead of the three
// stores 90; no flags local (tests on the member) 89. On the kept form: flags as `dword` with a `(word)` store 35
// (same code).
// Resend type selected by the sent bit, pending bit, both bits, or separate tests
// 52/52/89/54. Nullable eligible message via conditional or rejection branches 89/89.
// Qualifying cursor used for post-scan publication with an in-place or saved-word
// update 98/82. NOT kept: single-bit type selection gives the retail cursor and
// message pointer with a byte OR, but changes the flag read and return-value code.
// M3 lane 11 r119 (fast; control 35). Every in-place form (89) frees ECX in the loop: `next` then takes ECX and the
// window head EDX (retail: head ECX, next ESI, oldest EBX, message ECX, flags AX), and `time` moves to EBP, taking the
// frame word. Tried: 1 to 7 dead initialised locals 35 each (no symbol-count effect); the in-place update with
// `message` declared above the loop 89; the message through the existing reliable_stream_get_message with the in-place
// update 89; a no-code copy of the index between the lookup and the message zero 35 (same code); the update through the
// existing word bit setter 0xbfe40 (inlined as the in-place OR) 89.
// M3 lane 15 r104 (fast; control 35, all restored): the member update (`message->flags |= 2;`) with the three stores
// through a shared inline report helper 89 (same code as the member form); `type = 4;` first in the found path 42 (217 B:
// type is stored to its slot and reloaded, the stores may alias it); the member update with the message zero-initialised at
// its declaration ahead of the index lookup 59 (222 B: the entry, the count, the loop registers, the message in ECX, the
// flags in AX and `or byte ptr [ecx + 0xe], 2` are retail's; left: the zero is hoisted to the loop head where retail
// sets the index's NONE (`or edx, -1`) there and zeroes ECX after the divide, and the found path's temporaries take
// EAX/EDX where retail's take EDX/ESI with EAX untouched); that with `index` declared first 59 (same code); the member update
// with the message declared and zeroed after the lookup 89.
// Shared word flag setter with original, Boolean, named and split sent tests 89/89/89/89;
// Boolean and named bits with the saved update 64/64, separate sent rejection 35;
// setter after pending rejection 89. NOT kept: the setter gives the byte OR,
// but moves the entry/output allocation. Separate sent rejection retains the full-word store.
// Word flag setter on the message with saved/member/shifted/named bit tests 89/89/89/89.
// Setter with separate unsent/pending rejection 89/89 or paired mask comparison 106.
// Setter on saved word and assignment 35. NOT kept: the byte OR changes the entry and loop
// allocation, and the saved word ties with the same full-word store.
// Early message initialization with flag-word lvalue / direct flag reads / paired flag mask
// 59/59/73. Separate flag rejection / scan-scoped cursor / selected record lvalue 59/59/59.
// Scan-scope conditional 90; nested send and count guards 59. NOT kept: the byte OR and
// message registers come with a displaced initialization and output setup.
// Round 436. Pending-bit field assignment with saved word tests (word/byte/bool)
// 89/86/86, both fields tested directly 89/84/84, word/byte bit-view copy 89/84.
// NOT kept: setting the actual bit changes the entry and loop registers, even at
// 225 bytes. The selection still differs from retail. Original body restored.
// M3 lane 4 r220 (fast with 0x96510/0x94df0; control 35): the saved-word update ahead of the three output stores 39 (the
// word store stays: VC does not fuse a register value back into a memory OR even with no store in between); the in-place
// update ahead of the stores 90, with the tests on the member as well 90; the in-place update with `type = 4;` first in the
// found path 104 (209 bytes); the test inverted with `continue` and the found code after it 35 (same code), with the
// in-place update 89. Diagnostics, reverted: on the in-place form with the message zeroed at its declaration (59), a probe
// store of the flags word after the update keeps it live in AX and the found path then takes retail's EDX/ESI temporaries
// exactly (46, the probe's store aside): retail's flags word is live through its found path while the update is the memory
// OR. The source that keeps it live without code is still unknown.
// Word pending bit with saved word tests 89, Word pending bit with two native bit tests 88, Word pending bit with
// selected low bit pair 106, Word pending bit with separate sent and pending admissions 89, Byte pending bit with
// saved word tests 86, Byte pending bit with two native bit tests 84, Byte pending bit with selected low bit pair
// 88, Byte pending bit with separate sent and pending admissions 86. NOT kept: The bitfield update emits the byte
// OR, including at 225 bytes, but changes the opening register allocation and raises the score. The original
// message layout and body are restored.
// Read-only or mutable message record references across the local slot lookup or the shared message
// lookup: saved word 35 in all four forms, in-place bit update 89 in all four. NOT kept: The in-place
// forms emit the byte OR but lose retail query and scan allocation.
// M3 lane 3 r271 (fast with 0x96510, 0x94df0, 0x94bf0 and 0x96b00, matched or unchanged each time; control 35). The live
// flags word is confirmed as the one cause: with the in-place update and any read of `flags` after it, the scan, the
// message in ECX, the flags in AX, the found path's EDX/ESI temporaries, the pointer-first `*time` store and the byte OR
// are all retail's, and only the read's own instructions are left. Diagnostics, reverted at once: `type = (flags & 2) ?
// 2 : 4;` after the in-place update 28 (232 B: the five-instruction select in place of `mov eax, 4`; a test that is
// always false there), `type = (flags & 1) ? 4 : 0;` 36 (230 B), `type = (flags & 2) ? 0 : 4;` 90 (226 B: folded from
// the enclosing test before allocation, so the in-place form's allocation), `if (flags & 8) type = 4; else type = 4;`
// 91 (the equal arms fold early as well). Other forms: the in-place update with the message zeroed at its declaration
// re-measured 59, that with the flags read again into the local before the update (`flags = message->flags;
// message->flags = flags | 2;`) 59 (same code: fused into the memory OR before allocation); the kept structure with that
// re-read 89, with `flags = message->flags | 2; message->flags = flags;` 89; the in-place update under a second
// `if (!(flags & 2))` 87 (226 B); `type = 4;` between the three stores and the in-place update 103 (209 B); the update
// as `m_messages[index].flags |= 2;` 100 (240 B); the found block without `break` and `if (type) break;` after it 70
// (231 B). Left: a read of `flags` after the update that leaves no instruction; a value that the enclosing test already
// decides is folded before allocation, so it is not a repeated test.
// In-place resend update, two scan-local naming orders, each with original or renamed output
// parameters: clear after lookup 89 in each form; initialize before lookup 59 in each form.
// NOT kept: early initialization restores the ECX message and AX flags but places the
// zeroing and output loads differently. The other initialization changes the entry frame.
// MiniMax C (11-19): no gain (35, baseline; body restored). Two differences are left: the message pointer register (EAX here, ECX in retail, and with it the flags word in CX against AX) and the found path update (a full-word store here, the byte OR of the resend bit in retail). Every form that makes the update a byte OR (message->flags |= 2, or message->flags = message->flags | 2) frees the saved flags word after the tests, and the whole allocation then changes (the prologue type pointer moves from EAX to ECX, next takes ECX and the flags take DX): 89. The ternary (index != NONE ? &m_messages[index] : 0) gives retail message register ECX, flags register AX and found path exactly, but the ternary arms are laid out with the address in the fall-through and a jump over the zero, and the helper NONE initialisation is folded away: 45; with the test inverted, 43. Zeroing the message at its declaration, ahead of the lookup, gives the message in ECX and the flags in AX with the byte OR, but the zero is hoisted to the loop head (retail zeroes ECX after the divide) and the helper temporary takes EDX: 59. Zeroing it after the lookup is the kept body, and the message takes EAX.
// M3 lane 5 r213 (fast with 0x96510, 0x94df0, 0x94bf0 and 0x96b00, matched throughout; control 35): a reference to the
// flags member (`word &flags = message->flags;`, tests and update through it, `|=` or `= flags | 2`) 89, the same with a
// const reference and the member `|=` 89, with the message zeroed at its declaration 59 (the member update's codes);
// the flags local declared at function scope with the in-place update 89, with the saved-word update 35 (same code); the
// found test through an inline predicate on the flags with either update 89/35, on the message with the in-place update
// 89; the in-place update between the first and second output stores 90.
// M3 lane 15 r115 (a one-file LTCG harness that reproduces this function, about 290 forms at 2 s each; the leads
// confirmed fast with 0x96510, 0x94df0, 0x94bf0 and 0x96b00, matched each time; control 35). New cause: the flags word
// need not stay live. A second, plain result variable assigned 4 as the first statement of the found block (with
// the member update `message->flags |= 2;`) gives retail's loop, the message in ECX, the flags in AX, the found
// path and the byte OR: the result holds EAX through the found block, so the message cannot take it. Forms:
// `result = type` in both arms of `sent || !count` with one return 28 (214 B: the type is loaded once ahead of
// the loop, where retail loads it at each exit); `bool sent = get_next_send(...); long result = type;` ahead of
// the test 20 (224 B: the copy is held in ECX for the two early exits); LEAD: `result = type` after the failed
// send, the loop under the count test, `else result = type;` on the count test and on the send test, one return:
// 10 at retail's 225 bytes, the only difference the order of the last two exit blocks (ours lays the early exit
// ahead of the loop's zero-trip exit, retail after it). Not kept: the inner else repeats an assignment (a
// match-only form); saved as build/r115/lead-965e0-10.patch. On that lead (harness, same code or worse): the two
// tests as `||` or `&&` with the copy in a comma expression (same code), early returns of the type 218/207/216,
// the loop arm with its own return 218/212, the copy inside the count arm 214, a found flag guarding the copy
// after the loop 214 (the loop is no longer entered through a zero-trip test), a loop with its own exit test 214.
// Without the second variable (harness): the found block returning 4 itself 187 (no shrink-wrapped entry);
// a found flag or a zero result selected at the return 187/192; declaration orders of message, flags and next
// (7 forms) the member update's code; a grid of 195 spellings (five lookups, five flag tests, three updates,
// three statement orders) none at 225 bytes, nearest the ternary lookup with the saved-word update.
// MiniMax E (11-19): no gain (35, baseline; body restored): the message pointer is still in EAX where retail keeps it
// in ECX (and the flags word in CX against retail's AX), and the update is still a full-word store where retail ORs
// the resend bit in the message; the in-place update reaches retail's byte OR (or byte ptr [ecx+0xe], 2) but frees the
// flags word and changes the whole allocation (89), the ternary message pointer with the in-place update is 90, and
// the ternary alone keeps retail's registers but lays the arms out with the address in the fall-through (45).
// Resend result set before publication with return selection 129, or assigned ordinary type when none 120.
// Separate query/empty-window and active-scan outcomes 36; ordinary type snapshot 20, with sole exit 23.
// Bind time destination, name size at publication, or both 89/89/89.
// Resend goes straight to the common return, with ordinary type read at exhaustion 120.
// NOT kept: the 20 and 23 forms move the type read ahead of the admission branches and replace the early stack reads with ECX.
// M3 lane 13 r129 (fast with 0x96510, 0x94df0, 0x94bf0 and 0x96b00, matched throughout; control 35): the resend scan as a
// file-local bool helper (marks the message, returns true) under `&& helper) type = 4;` 35 forceinline with the saved-word
// update (same code), 91 with the member update; the caller returning 4 itself 111/129; as plain `inline` the helper stays a
// call (250/237). Lane 15 r115's second-variable lead re-measured 10 at 225 B (only the last two exit blocks swapped); on it:
// the count or the send test inverted 10/10 (same code), both 87, the copy inside the count arm 88, without the count
// test's else 95, `result` declared first 10, `next` at function scope 10, the count as `> 0` 22; the copy in a
// `!send && count` arm with an else 28, as `send || !count` arms 28; a result local in the failed-send block with two
// returns 94, an early `return type` on the send 94 (both push every register at entry).
// M3 lane 7 r197 (fast with 0x96510, 0x94df0, 0x94bf0 and 0x96b00, matched each time; control 35). Arrangements of the
// listed second-result lead (result = 4 first in the found block, the in-place update): the lead itself 10 (225 B), with
// the send test inverted 10, with the count test inverted (`if (!count) result = type; else` the loop) 10: the shared
// early exit is still laid out ahead of the loop's zero-trip exit. Both tests inverted 87 (216 B); no inner else 95 (205
// B); one combined test with the copy at the top of its arm and an else 28 (214 B), the same with a named `sent` 28, the
// combined test inverted (`send || !count` arm first) 28; `else if (!count)` chain 92 (210 B); `bool sent` and the copy
// before the test 20 (listed); an early return of the type on a successful send with the copy after it 94 (218 B), with
// the count test as a second early return 90 (212 B), with both tests in one early return 80 (216 B). The early exit
// follows the loop's zero-trip exit only when the early paths jump straight to the return (the kept form), and then the
// result has no register for the found block.
// MiniMax C (11-20): no new form, no lower score (35): the message pointer is in eax where retail has ecx and the flags in cx against ax;
// the polarity and else forms give retail registers but fold the window NONE init (43, 45).
// Single-exit jump from found, sent-or-empty, sent only or empty only, with saved word, 35 each.
// The same exits with the in-place OR give 89/91/89/89. The in-place store changes entry allocation.
// Pending bit XOR/add on saved flags 35/90, on the selected entry 88/88.
// With message initialized before the lookup, saved word 63/63 and entry 59/59.
// NOT kept: the XOR tie retains the full-word store; the others grow the difference from the
// retail entry/flags allocation or miss the byte OR.
// M3 lane 3 r284 (h2m fast screen with 0x96510, 0x94df0, 0x94bf0, 0x96b00, all MATCH; lead = lane 15 r115's second-result
// form, re-measured 10): the lead with `result = 4` last in the found block 91 (221 B); the lead with a named `bool sent`
// 10 and with the loop bound read into `long newest` 10 (same code each); main's body with `type = 4` first in the found
// block 42 (217 B), that with the in-place `|= 2` update 104 (209 B). In main every exit is a duplicated return (zero-trip
// ahead of early, as retail); the lead's `else result = type` arms are real blocks laid out in source order after the
// loop, so its early exit lands ahead of the zero-trip copy.
// Saved word tests with a direct low-byte OR or shared byte setter 86/86, byte tests and OR
// 84, paired word test and byte OR 88, message initialized at lookup 86. Success result before
// publication with the ordinary result at the sole exit 112 or split empty and exhausted
// outcomes 74. Separate window-count admission with byte OR 86. The compiled prologue or
// found tail differs in every form, including the byte-test form at retail length.
// Split result with shared lookup 10, available-first or missing-first conditional lookup
// 34/32, separate eligibility rejections or nested word tests 10/10. Joint admission with
// a separate result 28. Early admission exits with the result set before or after publication
// 80/80. Direct found return under common or nested admission 129/129, scan-local result
// return 80. NOT kept: the 10-point forms repeat the ordinary-result assignment and
// exchange the two late exits. The combined admission shortens to 214 bytes.
// @retail 0x965e0
long c_network_reliable_stream::function_965e0(long *sequence, long *size, long *time)
{
	long type = 0;
	bool sent = get_next_send(&type, sequence, size, time);
	long result = type;
	if (!sent && sequence_window_count(&m_message_window))
	{
		result = type;
		for (long next = m_next_sequence + 1; next < m_message_window.newest; next++)
		{
			s_reliable_message *message = reliable_stream_get_message(this, next);
			word flags = message->flags;
			if ((flags & 1) && !(flags & 2))
			{
				result = 4;
				*sequence = next;
				*size = message->size;
				*time = message->unknown08;
				message->flags |= 2;
				break;
			}
		}
	}
	return result;
}

bool __stdcall function_096ce0(c_network_reliable_stream *stream, bool force, long *type, long *sequence);

// @retail 0x96510
long c_network_reliable_stream::allocate_sequence(long time)
{
	long sequence = NONE;
	if (!v1(0))
	{
		if (sequence_window_count(&m_message_window) >= m_message_window.capacity)
		{
			long type;
			long dropped;
			function_096ce0(this, true, &type, &dropped);
		}
		long newest = m_message_window.newest;
		if (m_last_sequence - newest + 0x80 > 0 &&
			newest - m_message_window.oldest < m_message_window.capacity &&
			newest - m_next_sequence + 1 < 0x80)
		{
			sequence = newest + 1;
			sequence_window_extend(&m_message_window, sequence);
			long distance = sequence - m_next_sequence;
			m_unknown959 = false;
			s_reliable_message *message = reliable_stream_get_message(this, sequence);
			memset(message, 0, sizeof(*message));
			message->distance = (word)distance;
			message->time = time;
			message->unknown08 = NONE;
		}
		else
		{
			m_unknown05 = true;
		}
	}
	return sequence;
}

/* lane M's 0x1a4840: the out-of-line copy of the window advance */
void sequence_window_advance_1a4840(s_sequence_window *window, long sequence);
long function_75890(long time);

static __forceinline bool function_96ce1(c_network_reliable_stream *stream, bool force, long *oldest)
{
	bool result = false;
	if (sequence_window_count(&stream->m_message_window))
	{
		long first = stream->m_message_window.oldest + 1;
		if (first <= stream->m_next_sequence)
		{
			s_reliable_message *message = reliable_stream_get_message(stream, first);
			word flags = message->flags;
			if ((flags & 4) || force || (flags & 1) ||
				function_75890(message->time) >= stream->m_timeout + g_network_configuration.value16b4)
			{
				*oldest = first;
				result = true;
			}
		}
	}
	return result;
}

// @retail 0x96ce0
bool __stdcall function_096ce0(c_network_reliable_stream *stream, bool force, long *type, long *sequence)
{
	c_network_reliable_stream *const *stream_reference = &stream;
	long *volatile *type_reference = &type;
	bool result = false;
	**type_reference = 0;
	s_sequence_window *window = &(*stream_reference)->m_message_window;
	long oldest;
	if (function_96ce1(stream, force, &oldest))
	{
		**type_reference = 6;
		result = true;
		*sequence = oldest;
		sequence_window_advance_1a4840(window, oldest);
	}
	return result;
}

// @retail 0x95410
void c_network_unreliable_stream::v7(long identifier, bool delivered)
{
	s_stream_message *message;
	if (sequence_window_count(&m_message_window))
	{
		for (long sequence = m_message_window.oldest + 1; sequence <= m_message_window.newest; sequence++)
		{
			long index = sequence_window_index(&m_message_window, sequence);
			message = 0;
			if (index != NONE)
				message = &m_messages[index];
			if (message->unknown08 == identifier)
			{
				message->unknown08 = NONE;
				if (delivered)
					message->flags |= 1;
				else
					message->flags |= 4;
			}
		}
	}
	while (sequence_window_count(&m_message_window))
	{
		long sequence = m_message_window.oldest + 1;
		long index = sequence_window_index(&m_message_window, sequence);
		message = 0;
		if (index != NONE)
			message = &m_messages[index];
		if (!(message->flags & 1))
			break;
		m_message_bytes -= message->size;
		void *data = message->data;
		long info;
		if (!g_4d87f8->allocator->get_info(data, &info))
			info = NONE;
		function_94bf3(g_4d87f8, data);
		message->data = 0;
		sequence_window_advance(&m_message_window, sequence);
	}
}

void network_message_write_header(s_bitstream *stream, long type, long size);
void function_163ba0(dword *crc_reference, void const *buffer, long buffer_size);

static inline void stream_begin_payload(s_bitstream *stream)
{
	stream->bit_position = 0;
	stream->checkpoint_count = 0;
	stream->error = false;
	if (stream->mode == 1)
	{
		stream->unknown2c = 0;
		stream->unknown30 = 0;
	}
	else if (stream->mode == 3 || stream->mode == 4)
	{
		if (function_1959c0(stream, 32) == 0x64656267)
			stream->error = true;
		else
		{
			stream->bit_position = 0;
			stream->error = false;
		}
	}
}

static __forceinline void function_95581(s_bitstream *arg_0, void *arg_1, long arg_2)
{
	arg_0->data = (byte *)arg_1;
	arg_0->size_in_bytes = arg_2;
	arg_0->unknown08 = 1;
	arg_0->mode = 1;
	memset(arg_0->data, 0, arg_0->size_in_bytes);
	arg_0->bit_position = 0;
	arg_0->checkpoint_count = 0;
	arg_0->error = false;
	long local_0 = *(volatile long *)&arg_0->mode;
	if (local_0 == 1)
	{
		arg_0->unknown2c = 0;
		arg_0->unknown30 = 0;
	}
	else if (local_0 == 3 || local_0 == 4)
	{
		if (function_1959c0(arg_0, 32) == 0x64656267)
			arg_0->error = true;
		else
		{
			arg_0->bit_position = 0;
			arg_0->error = false;
		}
	}
}

// @retail 0x95580
void __stdcall function_095580(void *header, long message_type, long message_size, void const *message_data)
{
	struct
	{
		dword crc;
		byte data[0xffff];
	} payload;
	s_bitstream stream;
	function_95581(&stream, payload.data, sizeof(payload.data));
	c_network_unreliable_stream *self = (c_network_unreliable_stream *)header;
	c_type_659ceb *types = (c_type_659ceb *)self->m_unknown0c;
	network_message_write_header(&stream, message_type, message_size);
	types->m_types[message_type].encode(&stream, message_size, (void *)message_data);
	long bits = stream.bit_position + 32;
	long bytes = (bits + 7) / 8;
	stream.size_in_bytes = (stream.bit_position + 7) / 8;
	long remainder = stream.size_in_bytes % stream.unknown08;
	if (remainder)
		stream.size_in_bytes += stream.unknown08 - remainder;
	stream.mode = 2;
	payload.crc = NONE;
	function_163ba0(&payload.crc, &payload, bytes);
	byte const *data = (byte const *)&payload;
	long sent_bits = 0;
	bool done = false;
	while (!done && !self->v1(0))
	{
		if (sequence_window_count(&self->m_message_window) < self->m_message_window.capacity)
		{
			long remaining_bits = bits - sent_bits;
			bool last;
			long fragment_bits;
			long size;
			if (remaining_bits <= 256)
			{
				fragment_bits = remaining_bits;
				last = true;
				size = (remaining_bits + 7) / 8;
			}
			else
			{
				last = false;
				fragment_bits = 256;
				size = 32;
			}
			s_allocator_globals *globals = g_4d87f8;
			void *block = globals->allocator->allocate(size, 0, 0);
			if (!block)
			{
				globals->allocator->compact(0);
				block = globals->allocator->allocate(size, 0, 0);
			}
			if (block)
				globals->count++;
			if (block)
			{
				long sequence = self->m_message_window.newest + 1;
				sequence_window_extend(&self->m_message_window, sequence);
				long index = sequence_window_index(&self->m_message_window, sequence);
				s_stream_message *message = 0;
				if (index != NONE)
					message = &self->m_messages[index];
				memcpy(block, data, size);
				memset(message, 0, sizeof(*message));
				message->flags = 4;
				message->size = (byte)size;
				message->unknown08 = NONE;
				if (last)
					message->flags |= 8;
				else
					message->flags &= ~8;
				message->unknown02 = (word)fragment_bits;
				message->data = block;
				self->m_message_bytes += size;
				sent_bits += 256;
				data += 32;
				if (last)
					done = true;
			}
			else
				self->m_unknown05 = true;
		}
		else
			self->m_unknown05 = true;
	}
}

bool network_message_read_header(s_bitstream *stream, long *type, c_type_659ceb const *collection, long *size);

// @retail 0x95840
bool __stdcall function_095840(s_network_stream_header *header, long *message_type, long *message_size, void *message_data)
{
	c_network_unreliable_stream *self = (c_network_unreliable_stream *)header;
	bool result = false;
	if (sequence_window_count(&self->m_fragment_window))
	{
		for (long sequence = self->m_fragment_window.oldest + 1; sequence <= self->m_fragment_window.newest; sequence++)
		{
			s_stream_fragment *fragment = unreliable_stream_get_fragment(self, sequence);
			if (!fragment || !(fragment->flags & 1))
				break;
			if (fragment->flags & 2)
				result = true;
		}
		if (result)
		{
			struct
			{
				dword crc;
				byte data[0xffff];
			} payload;
			s_bitstream stream;
			stream.mode = 0;
			stream.bit_position = 0;
			stream.checkpoint_count = 0;
			stream.error = false;
			stream.unknown08 = 1;
			stream.data = payload.data;
			stream.size_in_bytes = sizeof(payload.data);
			long bytes = 0;
			bool last = false;
			for (long sequence = self->m_fragment_window.oldest + 1; !last && sequence <= self->m_fragment_window.newest; sequence++)
			{
				long index = sequence_window_index(&self->m_fragment_window, sequence);
				s_stream_fragment *fragment = NULL;
				if (index != NONE)
					fragment = &self->m_fragments[index];
				long size;
				if (fragment->flags & 2)
				{
					size = (fragment->unknown02 + 7) / 8;
					last = true;
				}
				else
					size = 32;
				memcpy((byte *)&payload + bytes, fragment->data, size);
				bytes += size;
				free_block(fragment->data);
				self->m_fragment_bytes -= fragment->size;
				sequence_window_advance(&self->m_fragment_window, sequence);
			}
			dword expected = payload.crc;
			payload.crc = NONE;
			function_163ba0(&payload.crc, &payload, bytes);
			if (expected == payload.crc)
			{
				stream.data = payload.data;
				stream.size_in_bytes = bytes - 4;
				stream.mode = 3;
				stream_begin_payload(&stream);
				c_type_659ceb *types = (c_type_659ceb *)self->m_unknown0c;
				if (network_message_read_header(&stream, message_type, types, message_size))
				{
					s_message_type *type = &types->m_types[*message_type];
					memset(message_data, 0, *message_size);
					if (!type->decode(&stream, *message_size, message_data))
					{
						self->m_unknown05 = true;
						result = false;
					}
				}
				else
				{
					self->m_unknown05 = true;
					result = false;
				}
			}
			else
			{
				self->m_unknown05 = true;
				result = false;
			}
		}
	}
	return result;
}


void function_194830(s_bitstream *arg_0, bool arg_1);

static __forceinline void function_95e41(s_bitstream *arg_0, volatile long const *arg_1, long arg_2)
{
	if (arg_2 < 32)
	{
		dword checked_value = (dword)*arg_1;
		if (checked_value >= (dword)(1 << arg_2))
		{
			char local_0[256];
			local_0[0] = 0;
			csprintf_256(local_0, "%u exceeds max value of %u", checked_value, 1 << arg_2);
		}
	}
	function_195720(arg_0, *arg_1, arg_2);
}

// @retail 0x95e40
bool c_network_reliable_stream::v4(long arg_0, s_bitstream *arg_1, long arg_2, long arg_3)
{
	long local_0 = m_message_window.newest;
	long local_1 = local_0 - m_next_sequence;
	stream_write_bit(arg_1, arg_0 != NONE);
	stream_write_checked(arg_1, local_0 & 0xff, 8);
	stream_write_checked(arg_1, local_1, 7);
	if (!local_1)
		m_unknown959 = true;
	advance_acknowledgements();
	if (m_acknowledgement_window.valid)
	{
		long local_2 = m_acknowledgement_window.newest;
		long local_13 = local_2 & 0xff;
		--local_2;
		stream_write_checked(arg_1, local_13, 8);
		m_unknown950 = m_acknowledgement_window.newest;
		if (!sequence_window_count(&m_acknowledgement_window))
		{
			stream_write_bit(arg_1, false);
			stream_write_bit(arg_1, false);
			function_194830(arg_1, m_unknown958);
		}
		else if (sequence_window_count(&m_acknowledgement_window) <= 9)
		{
			long cursor = local_2;
			dword local_3;
			volatile long local_4;
			bool local_5 = false;
			local_4 = NONE;
			local_3 = 0;
			long local_6 = 0;
			for (; cursor > m_acknowledgement_window.oldest; cursor--, local_6++)
			{
				long local_7 = sequence_window_index(&m_acknowledgement_window, cursor);
				word *local_8 = 0;
				if (local_7 != NONE)
					local_8 = &m_acknowledgements[local_7];
				if (!(*local_8 & 1))
				{
					local_3 |= 1 << local_6;
					if (local_4 == NONE)
						local_4 = local_6;
					else
						local_5 = true;
				}
			}
			if (local_5)
			{
				stream_write_bit(arg_1, true);
				stream_write_bit(arg_1, false);
				for (long local_9 = 0; local_9 < 8; local_9++)
					stream_write_bit(arg_1, (local_3 & (1UL << local_9)) != 0);
			}
			else
			{
				stream_write_bit(arg_1, false);
				stream_write_bit(arg_1, true);
				function_95e41(arg_1, &local_4, 3);
			}
		}
		else
		{
			stream_write_bit(arg_1, true);
			stream_write_bit(arg_1, true);
			stream_write_bit(arg_1, false);
			stream_write_checked(arg_1, sequence_window_count(&m_acknowledgement_window) - 1, 7);
			for (; local_2 > m_acknowledgement_window.oldest; local_2--)
			{
				long local_10 = sequence_window_index(&m_acknowledgement_window, local_2);
				word *local_11 = 0;
				if (local_10 != NONE)
					local_11 = &m_acknowledgements[local_10];
				stream_write_bit(arg_1, !(*local_11 & 1));
			}
		}
	}
	else
	{
		stream_write_checked(arg_1, 0, 8);
		stream_write_bit(arg_1, true);
		stream_write_bit(arg_1, true);
		stream_write_bit(arg_1, true);
	}
	return false;
}
