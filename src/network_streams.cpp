// @flags /O2 /Ob1 /arch:SSE /Gr
/* NETWORK_STREAMS.CPP: the two kinds of message stream a connection owns
   (src/unknown_0820f0.cpp allocates them): the unreliable stream (0x2850
   bytes, vtable 0x450db8) and the reliable stream (0x97c bytes, vtable
   0x450dd8). Both keep their messages in windows over sequence numbers. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0820f0.h"
#include "unknown_0662e0.h"
#include <xtl.h>
#include <stdlib.h>
#include <string.h>


/* a window over a range of sequence numbers: the messages oldest+1..newest,
   kept in a ring buffer from head */
struct s_sequence_window
{
	bool valid;
	byte unknown01[3];
	long capacity;
	long newest;
	long oldest;
	long size;
	long head;
	long count;
};

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

/* the release routine retail inlines here */
static inline void free_block(void *block)
{
	long info;
	if (!g_4d87f8->allocator->get_info(block, &info))
		info = NONE;
	s_allocator_globals *globals = g_4d87f8;
	globals->allocator->release(block, NONE);
	if (block != 0)
		globals->count--;
}

struct s_stream_message
{
	byte flags;
	byte size;
	word unknown02;
	void *data;
	long unknown08;
};

struct s_stream_fragment
{
	byte flags;
	byte size;
	word unknown02;
	void *data;
};

struct s_reliable_message
{
	long time;
	long size;
	long unknown08;
	word distance;
	word flags;
};

class c_network_stream
{
public:
	virtual long v0() { return 0; }
	virtual bool v1(long *reason) { return false; }
	virtual bool v2(bool *pending) { return false; }
	virtual long v3(long a, long b) { return 0; }
	virtual void v4() {}
	virtual void v5() {}
	virtual void v6() {}
	virtual void v7(long identifier, bool delivered) {}
};

class c_network_unreliable_stream : public c_network_stream
{
public:
	virtual bool v1(long *reason);
	virtual bool v2(bool *pending);
	virtual long v3(long a, long b);
	virtual void v7(long identifier, bool delivered);

	bool m_active;
	bool m_unknown05;
	long m_owner;
	void *m_unknown0c;
	s_sequence_window m_message_window;
	s_stream_message m_messages[512];
	s_sequence_window m_fragment_window;
	s_stream_fragment m_fragments[512];
	long m_message_bytes;
	long m_fragment_bytes;
};

class c_network_reliable_stream : public c_network_stream
{
public:
	virtual bool v1(long *reason);
	virtual bool v2(bool *pending);
	virtual long v3(long a, long b);
	void advance_acknowledgements();
	bool get_next_send(long *type, long *sequence, long *size, long *time);
	long function_965e0(long *sequence, long *size, long *time);
	long allocate_sequence(long time);
	long read_acknowledgement(long *message_sequence, long sequence, bool valid, long distance);
	void mark_received(long sequence);
	void update_round_trip(long type, long round_trip_time, long sequence, long time);

	bool m_active;
	bool m_unknown05;
	long m_owner;
	s_sequence_window m_acknowledgement_window;
	s_sequence_window m_message_window;
	long m_next_sequence;
	long m_bytes;
	word m_acknowledgements[0x80];
	s_reliable_message m_messages[0x80];
	long m_last_sequence;
	long m_unknown950;
	long m_unknown954;
	bool m_unknown958;
	bool m_unknown959;
	long m_unknown95c;
	long m_unknown960;
	long m_unknown964;
	long m_round_trip_minimum;
	long m_round_trip_average;
	long m_round_trip_deviation;
	long m_timeout;
	long m_backoff;
};

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
				free_block(message->data);
				message->data = 0;
			}
		}
		if (sequence_window_count(&stream->m_fragment_window))
		{
			for (long sequence = stream->m_fragment_window.oldest + 1; sequence <= stream->m_fragment_window.newest; sequence++)
			{
				s_stream_fragment *fragment;
				long index = sequence_window_index(&stream->m_fragment_window, sequence);
				fragment = 0;
				if (index != NONE)
					fragment = &stream->m_fragments[index];
				if (fragment->data)
				{
					stream->m_fragment_bytes -= fragment->size;
					free_block(fragment->data);
					fragment->data = 0;
				}
			}
		}
	}
	stream->m_unknown05 = false;
	stream->m_message_window.newest = 0;
	stream->m_message_window.oldest = 0;
	stream->m_message_window.head = 0;
	stream->m_message_window.count = 0;
	stream->m_message_window.valid = true;
	stream->m_fragment_window.newest = 0;
	stream->m_fragment_window.oldest = 0;
	stream->m_fragment_window.head = 0;
	stream->m_fragment_window.count = 0;
	stream->m_fragment_window.valid = true;
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
s_stream_fragment *unreliable_stream_get_fragment(c_network_unreliable_stream *stream, long sequence)
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

// @retail 0x95cf0
void function_095cf0(s_network_stream_header *header)
{
	c_network_reliable_stream *stream = (c_network_reliable_stream *)header;
	stream->m_unknown05 = false;
	long sequence = (dword)(g_network_configuration.value16a8 * network_time_now()) / 1000 & 0xff;
	sequence_window_initialize(&stream->m_message_window, sequence);
	sequence_window_reset(&stream->m_acknowledgement_window, 0);
	stream->m_acknowledgement_window.valid = false;
	stream->m_next_sequence = sequence;
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
s_reliable_message *reliable_stream_get_message(c_network_reliable_stream *stream, long sequence)
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
	long acknowledged = delta + newest;
	if (valid)
		*message_sequence = acknowledged;
	else
		*message_sequence = NONE;
	m_unknown958 = distance == 0;
	long oldest = acknowledged - distance;
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
	long result = 0;
	if (acknowledged < m_acknowledgement_window.oldest || acknowledged == m_acknowledgement_window.oldest && valid)
		result = 2;
	return result;
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
			long elapsed = network_time_now() - message->time;
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
		if (message->flags & 0xc)
		{
			message->flags |= 0x10;
			m_next_sequence = next;
			m_bytes -= message->size;
			result = true;
		}
	}
	return result;
}

// @retail 0x965e0
long c_network_reliable_stream::function_965e0(long *sequence, long *size, long *time)
{
	long type = 0;
	if (!get_next_send(&type, sequence, size, time) && sequence_window_count(&m_message_window))
	{
		long newest = m_message_window.newest;
		for (long next = m_next_sequence + 1; next < newest; next++)
		{
			s_reliable_message *message;
			long index = sequence_window_index(&m_message_window, next);
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
				return 4;
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

// @retail 0x96ce0
bool __stdcall function_096ce0(c_network_reliable_stream *stream, bool force, long *type, long *sequence)
{
	c_network_reliable_stream *const *stream_reference = &stream;
	long *const *type_reference = &type;
	bool result = false;
	**type_reference = 0;
	s_sequence_window *window = &(*stream_reference)->m_message_window;
	if (sequence_window_count(window))
	{
		long oldest = stream->m_message_window.oldest + 1;
		if (oldest <= stream->m_next_sequence)
		{
			s_reliable_message *message = reliable_stream_get_message(stream, oldest);
			word flags = message->flags;
			if ((flags & 4) || force || (flags & 1) ||
				function_75890(message->time) >= stream->m_timeout + g_network_configuration.value16b4)
			{
				*type = 6;
				result = true;
				*sequence = oldest;
				sequence_window_advance_1a4840(window, oldest);
			}
		}
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
		free_block(message->data);
		message->data = 0;
		sequence_window_advance(&m_message_window, sequence);
	}
}
