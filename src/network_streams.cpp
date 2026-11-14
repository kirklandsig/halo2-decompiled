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

void function_194710(s_bitstream *stream, bool discard);



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

// score 2 (was 2): retail reloads the address-taken loop counter before it
// stores fragment->data = 0, ours stores the zero first.
// @retail 0x94bf0
void function_094bf0(s_network_stream_header *header)
{
	c_network_unreliable_stream *stream = (c_network_unreliable_stream *)header;
	long work;
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
				if (!g_4d87f8->allocator->get_info(local_2, &work))
					work = NONE;
				function_94bf3(g_4d87f8, local_2);
				message->data = 0;
			}
		}
		if (sequence_window_count(&stream->m_fragment_window))
		{
			for (work = stream->m_fragment_window.oldest + 1; work <= stream->m_fragment_window.newest; work++)
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
						function_194710(stream, true);
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

// MiniMax G (11-14): no new form, no lower score (60): ours strength-reduces next - oldest - 1 into a second counter
// (next in ecx, the reduced offset in esi) where retail keeps next in esi and recomputes the offset on every pass.
// The member form (the count, the bound and the index all through &m_message_window) reproduces retail's loop body
// but scores 89: the count folds oldest into its `sub` and the loop reloads it instead of keeping it in ebx, and the
// pointer form keeps oldest in ebx while the offset is reduced. No spelling gives both.
// score 60 (was 60): round 116 (m3 lane 9, fast with 0x96510, 0x94df0 and 0x94bf0): the flag test as nested ifs 60
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
// @retail 0x965e0
long c_network_reliable_stream::function_965e0(long *sequence, long *size, long *time)
{
	long type = 0;
	if (!get_next_send(&type, sequence, size, time) && sequence_window_count(&m_message_window))
	{
		s_sequence_window *window = &m_message_window;
		long newest = window->newest;
		for (long next = m_next_sequence + 1; next < newest; next++)
		{
			s_reliable_message *message;
			long index = sequence_window_index(window, next);
			message = 0;
			if (index != NONE)
				message = &m_messages[index];
			word flags = message->flags;
			if ((flags & 1) && !(flags & 2))
			{
				*sequence = next;
				*size = message->size;
				*time = message->unknown08;
				message->flags |= 2;
				type = 4;
				break;
			}
		}
	}
	return type;
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
