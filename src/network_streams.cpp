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

// score 7 (was 7): Frame 0x14 agrees; initial message EAX/EDX, copy-before-OR and final delivery branch remain.
// the first lookup's pointer is in EAX (retail EDX), the flags OR is done in
// place and copied (retail copies, then ORs: mov eax, edx; or eax, 1), and the last delivery
// test branches to the skip arm (retail jumps to the delivery arm). Kept: the bit index as a
// named base minus current (retail's unfolded lea/sub start; every spelling of
// acknowledged - current - 1 folds it to -1) and the type as an if/else (current EDI and the
// entry EBX as retail, movzx edi, al). Tried after: type as a ternary 21, as 4 + shift 21
// (lookup in EDX, type arithmetic differs), from (byte)local_1 86, through the field 93;
// flags value after the OR as dword 16 (lookup in EDX, movzx eax, dx); previous as a ternary 7;
// delivery test as an else-if 86, as a pending flag or !(a && b) 7-84; word/short flag types,
// statement orders and type polarity 7.
// Earlier at 81: the getter inlined (0x966d0 marked inline) and previous set in an if/else fix
// the head. Bit index derived from current in four spellings 81 (folded to -1);
// `(local_1 & 8) ? 5 : 4` 118 (current/message registers then as retail, but no movzx and the
// first lookup in eax); bit as the loop variable 144; loop over the sequence parameter 174;
// type after the flag store 120; message variable reused in the loop 81.
// Earlier at 94, the slot address is kept in EAX here and in EDX in
// retail, and ours zeroes EAX before the range test, so the loop head differs.
// Native message-bit predicate/publication/both 117/113/80; native repeated-bit type
// alone/with predicate/with publication/all 119/127/119/123; arithmetic native type 119.
// acknowledged/undelivered/both skips, fallback exit and ordinary while 81; iteration-bit value
// alone/with skips 105/105/105.
// Actual time operand 142; two-use elapsed 84; later type with actual time/reused elapsed 148/121;
// original flag type alone/with time 113/143; native type byte with time 142; direct publication/time 143.
// Read-only first message, fixed first/loop/both pointers, live original flags, live flags with fixed pointers, immutable flag stages and retained-value names all 7.
// Full-width original signed/unsigned 8/8; both signed/unsigned 9/8; separate flag copy/update 7; one word/dword flags value 68/69; named repeated predicate 7.
// Default previous 17; reversed message guard 38; in-place previous 7; early message declaration/
// previous before message 7/7; reset before/after lookup 39/138; reversed fallback order 10;
// default type 82; field/direct publication 70/70.
// Last delivery arm as `a ? true : b` 79, as a false default set true by the `||` test 7, as an
// else-if split 113; the whole decision in one condition 22; the window arm before the bit arm 111;
// delivered defaulted from current == acknowledged 93.
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
			word local_0 = entry->flags;
			if (!(local_0 & 1))
			{
				bool delivered;
				if (current == acknowledged)
					delivered = true;
				else if (bit < bit_count)
					delivered = (bits[bit >> 5] & (1 << (bit & 31))) == 0;
				else
					delivered = current > m_next_sequence || current > previous;
				if (delivered)
				{
					long start = entry->time;
					word local_1 = local_0 | 1;
					long type;
					if (local_1 & 8)
						type = 5;
					else
						type = 4;
					entry->flags = local_1;
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

// score 60 (was 60): ours strength-reduces next - oldest - 1 into esi (next in
// ecx) and compares next with the cached newest; retail keeps next in esi,
// recomputes the offset and re-reads window->newest each step (as 0x94df0 does).
// Passing &m_message_window to sequence_window_index (or declaring window inside
// the loop) gives retail's loop body, but oldest is then reloaded instead of
// reused from the count and time is cached in ebp (89, 221 bytes); the bound
// read in the condition with window kept (63). Tried every mix of window /
// &m_message_window for the count, the bound (local, window->newest or
// m_message_window.newest in the condition) and the index: 60, 63, 80, 88-91;
// a const window (60); reliable_stream_get_message (a call, 165).
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

// score 107 (was 141): frame agrees; aligned 50. Retail reads the oldest sequence once (EDX) for the
// count, the first sequence and the scan; keeps the result in a stack byte, made zero in CL at entry and
// returned with `mov al, cl` from early exits placed at the end, reloaded at the final return; sets the
// copy loop's fragment to NULL before the index test; and falls into the failure return after the
// decode test. Kept: the terminal flag in the copy loop's condition (aligned 55 -> 46), long result 141.
// Tried on the 141 body: short/word result 191, byte/char 217 (791 bytes), bool 234 (771 bytes);
// literal false failure return 161; first sequence before the count test 241; terminal flag declared
// first 140; bool result with literal false failure 224; bool single-exit count block 242; short
// terminal flag 203; copy-loop fragment NULL then assigned 156 (aligned 37), in both loops 156, plus a
// bool result 213 (aligned 33), plus literal false failure 203, literal false failure alone 174; the
// count test as a scope around the body with bool result 220, plus literal failure 210, long result 259.
// Tried on the earlier 172 body: scan getter 302; copy getter 325; both getters 361; unsigned byte
// progress 215; decode buffer first 213; forced-inline getters 223; readonly decode records 215; native
// stream value initialization 254; native copy size bits 215; native fragment result selection 213;
// existing window helper boundary 215; fragment result buffer publication 211; signed-byte/short/word
// results 210/193/193; read-only scan/copy entries 211/211; oldest with direct fragment selection 232,
// plus signed byte 234 or read-only scan 232; shared failure result 172; shared success exit 224;
// nonempty scope 211; scope/failure join 262; byte-total name 193; copy-sequence name 193; scalar
// names/failure join 172; scope/success join/scalar names 271; on the shared failure result,
// byte/bool/signed-byte/long results 193/219/193/238; native initial-window count/first with
// short/bool/byte/signed-byte results 269/278/274/274; single-result reset at payload/copy/CRC phases
// and validation tests, short 256/231/214/255, bool 229/268/265/276; existing release-helper
// composition 221; reused fragment data 213; terminal loop condition 191; first-sequence/saved-CRC
// homes 172/172; failure result before payload/after copy 250/224; failure publication order 172; data
// and terminal lifetimes 254; per-fragment terminal bool/header byte 192/250; conditional size with
// bool/byte 192/250; default size with bool/byte 244/251; native result with terminal bool/byte
// 266/218; independent bool/byte/short/long completion with literal returns 210/210/193/255, with
// separate callback results 206/206/189/253; oldest after/before result and result before receiver
// 229; shared initial newest with oldest-first/newest-first 202/202; oldest plus readonly types/stream
// helper/unsigned byte total 229; native valid/terminal/both fields 172 each; twice-used scan flags
// byte/native width 172/172, either with native copy terminal 172/172; full fragment observation with
// native terminal 238; availability accumulation: short logical OR 244, bitwise OR and normalized
// terminal bit 229/229, incomplete-only assignment 252, bool logical/bitwise OR 262/220, byte
// logical/bitwise OR 227/235. Diagnostics: frame pointer 289; global optimization 297; size
// preference 571.
// Earlier rounds on the terminal-loop graph: oldest/both initial endpoints 269/158; oldest with
// bool/byte/short/signed-byte result 276/268/267/268; oldest before false/newest first 269/158; both
// endpoints with bool/byte/short/signed-byte result 282/262/218/262; shared lookup index/fragment/
// both/all walk carriers 141 each, all with bool/short result 234/191; shared index-sequence/
// fragment-sequence 141/141; pre-lookup nullable scan/copy/both results with long 141/202/202, with
// bool 234/263/263; both lookups with byte/short result 243/214; actual decoder-result completion,
// long/bool/byte/short 262/175/190/259; common success exit, long/bool/byte/short 184/235/233/220.
// Round 9: the bool result (aligned 53 against long's 39 positional misalignment) is structurally
// retail's except at entry and the exits: retail keeps oldest in EDX and the result in CL across the
// count exit and the first > newest exit (both `mov al, cl`, own epilogues before `push ebx` and
// `push edi`), where ours reloads oldest and returns `xor al, al`, sharing the failure block's tail
// (ours stores the failure flag before the pops, retail after `pop ebx`). Bool 234 (771 bytes); bool
// with the count test as a scope 242; that with the failure as `return false` 232 (oldest and the
// zero are then cached, EDX/AL, but every push moves to the entry); bool with early count return and
// `return false` failure 224 (781 bytes, aligned 53); that with `return false` at the count exit 224.
// Frame 0x10048 agrees; 806/805 bytes; initial oldest/false carriers and allocator save differ.
// Frame and copy entry agree; oldest/false carriers, nullable copy result and allocator save differ.
// Earlier: Frame and length agree; retained oldest/false carriers and copy-loop entry differ.
// Frame 0x10048 agrees; oldest/false carriers, copy-loop entry and allocator save remain.
// Earlier 172 (was 172): Frame and length agree; retained oldest/false carriers and copy-loop entry differ.
// Frame and length agree; oldest/false carriers and copy-loop entry still differ.
// Earlier at 172 (was 172): Frame and length agree; retained oldest/false carriers and copy-loop entry differ.
// Earlier: retained oldest/false registers and copy-loop entry differ;
// scan getter 302; copy getter 325; both getters 361; unsigned byte progress 215; decode buffer first 213; forced-inline getters 223.
// readonly decode records 215; native stream value initialization 254; native copy size bits 215; native
// fragment result selection 213; frame pointer 289; global optimization 297; size preference 571; existing
// window helper boundary 215; fragment result buffer publication 211.
// Earlier: oldest/false carriers and copy-loop entry remain.
// Signed-byte/short/word results 210/193/193; read-only scan/copy entries 211/211;
// oldest with direct fragment selection 232, plus signed byte 234 or read-only scan 232.
// Shared failure result 172 at 805 bytes; shared success exit 224; nonempty scope 211;
// scope/failure join 262; byte-total name 193; copy-sequence name 193;
// scalar names/failure join 172; scope/success join/scalar names 271.
// On the shared failure-result body, byte/bool/signed-byte/long results give 193/219/193/238;
// native initial-window count/first with short/bool/byte/signed-byte results give 269/278/274/274.
// Single-result reset at payload/copy/CRC phases and validation tests, short
// 256/231/214/255, Boolean 229/268/265/276. All restored.
// Existing release-helper composition 221; reused fragment data 213; terminal loop condition 191;
// first-sequence/saved-CRC homes 172/172; failure result before payload/after copy 250/224;
// failure publication order 172; data and terminal lifetimes 254.
// Round 152: Per-fragment terminal bool/header byte 192/250; conditional size with bool/byte 192/250; default size with bool/byte 244/251; native result with terminal bool/byte 266/218.
// Independent bool/byte/short/long completion with literal returns 210/210/193/255;
// with separate callback results 206/206/189/253. All reverted.
// On common-failure result: oldest after/before result and result before receiver 229/229/229;
// shared initial newest with oldest-first/newest-first 202/202; oldest plus readonly types/stream
// helper/unsigned byte total 229/229/229.
// Native valid/terminal/both fields 172 each; twice-used scan flags byte/native width 172/172;
// either with native copy terminal 172/172; full fragment observation with native terminal 238.
// Original fragment declaration and body restored; oldest/false and copy entry still differ.
// Read-only availability accumulation: short logical OR 244; bitwise OR and normalized terminal bit 229/229; incomplete-only assignment 252; Boolean logical/bitwise OR 262/220; byte logical/bitwise OR 227/235. Original scan and short common-failure result restored.
// frame agrees; the copy loop now tests the terminal flag at the bottom
// and the newest sequence at the top as retail does. Retail's result is a stack byte kept in CL
// at entry (the early exits return it from duplicated epilogues) and reloaded at the final
// return; ours folds it. Retail also loads the oldest sequence once (EDX) for the count, the
// first sequence and the scan.
// Kept: the terminal flag in the copy loop's condition (aligned differences 55 -> 46), with a
// long result 141 (short 191, word 191, byte/char 217 at 791 bytes, bool 234 at 771 bytes,
// aligned 43 each for byte and bool).
// Tried on the kept body: literal false failure return 161; first sequence before the count
// test 241; terminal flag declared first 140; bool result with literal false failure 224; bool
// single-exit count block 242; short terminal flag 203. Earlier (on the 172 body): scan getter
// 302; copy getter 325; both getters 361; unsigned byte progress 215; decode buffer first 213;
// forced-inline getters 223; readonly decode records 215; native stream value initialization
// 254; native copy size bits 215; native fragment result selection 213; frame pointer 289;
// global optimization 297; size preference 571; existing window helper boundary 215; fragment
// result buffer publication 211; signed-byte/short/word results 210/193/193; read-only
// scan/copy entries 211/211; oldest with direct fragment selection 232, plus signed byte 234 or
// read-only scan 232; shared failure result 172; shared success exit 224; nonempty scope 211;
// scope/failure join 262; byte-total name 193; copy-sequence name 193; scalar names/failure join
// 172; scope/success join/scalar names 271; byte/bool/signed-byte/long results 193/219/193/238;
// native initial-window count/first with short/bool/byte/signed-byte results 269/278/274/274;
// single-result reset at payload/copy/CRC phases and validation tests, short 256/231/214/255,
// Boolean 229/268/265/276; existing release-helper composition 221; reused fragment data 213;
// terminal loop condition 191; first-sequence/saved-CRC homes 172/172; failure result before
// payload/after copy 250/224; failure publication order 172; data and terminal lifetimes 254;
// per-fragment terminal bool/header byte 192/250; conditional size with bool/byte 192/250;
// default size with bool/byte 244/251; native result with terminal bool/byte 266/218;
// independent bool/byte/short/long completion with literal returns 210/210/193/255, with
// separate callback results 206/206/189/253; oldest after/before result and result before
// receiver 229; shared initial newest with oldest-first/newest-first 202/202; oldest plus
// readonly types/stream helper/unsigned byte total 229; availability accumulation forms
// 220-262.
// Round 136, terminal-loop graph: oldest/both initial endpoints 269,158;
// oldest with bool/byte/short/signed-byte result 276,268,267,268;
// oldest before false/newest first 269,158;
// both endpoints with bool/byte/short/signed-byte result 282,262,218,262.
// Round 138: shared lookup index/fragment/both/all walk carriers 141/141/141/141;
// all with Boolean/short result 234/191; shared index-sequence/fragment-sequence 141/141.
// Original walk locals and long result restored; oldest/false homes and allocator save remain.
// Pre-lookup nullable scan/copy/both results with long 141/202/202; with bool 234/263/263;
// both lookups with byte/short result 243/214. Original terminal-loop body restored.
// Terminal-condition graph: actual decoder-result completion, long/bool/byte/short
// 262/175/190/259; common success exit, long/bool/byte/short 184/235/233/220.
// Original dword result, completion edges and every helper restored at 141.
// Round 184: child-window receivers, scan/copy/all with long result 162/248/263;
// scan/all/copy with byte result 259/178/261; scan/all with Boolean result 281/199.
// Shared empty-window exit, long/Boolean/byte/short result 260/242/224/259.
// All restored; oldest/false carriers, nullable copy lookup and allocator save remain.
// Round 80: result-first Boolean 234; separate Boolean/long availability 225/162;
// two long results 162; force-inline count 141; oldest reference 141;
// addressed Boolean empty/final exits 234/234. Original long-result body restored.
// Round 105 (kept, 141 -> 107): bool result, the count test as a scope, `if (!result) return
// result;` after the scan, a literal false failure, the copy loop's fragment set to NULL before
// the index test, and the release through the existing function_94bf3. Still differs: oldest in
// ECX with the zero stored as an immediate (retail EDX, zero in CL returned by the early exits),
// and the early exits share the final return. On it: success through one exit label 118; failure
// as `result = false` 123; early count return 121; result declared first 107; scan fragment NULL
// first 107; count `> 0` 106 (oldest in EDX, one extra test); an address-of-result diagnostic
// compiled identically (compile-only).
// Round 44: bool result with an early `if (!result) return result;` after the scan 224 (781 bytes); that with
// a `return false` failure 224; that with the count test as a scope 212 (789 bytes); long result with the early
// return and `return false` failure 161; bool result, count scope, `return false` failure 232. Original body kept.
// Round 51: the 14 missing bytes are retail's own epilogue for the scan-not-entered exit (before
// `push edi`, `mov al, cl`): retail returns a merged result from CL at both early exits, ours folds it
// to `xor al, al` and joins a shared full epilogue. `bool last; bool result = last = false;` 250; the
// copy/decode block under `if (result)` with literal returns 123; one final `return result;` reached
// by a goto from the decode success, failure `return false` 116; the same with the failure as
// `result = false` 108 (779 bytes: result made in a register at entry and stored, oldest in EDX and
// the failure epilogue as retail; but the result takes AL, coalesced with the return, where retail
// has CL with the count in EAX, so the early exits share the final epilogue's pops). On that form:
// result declared before `self`, declared then assigned, the count in a local 108 each; forceinline
// count helper 108; count `> 0` 116; a `window` pointer local 291; the count written out (diagnostic)
// 248, as `!=` 252 (count then in EAX, result in DL).
// Round 120 (lane 6, fast; aligned 50): the failure as `result = false; m_unknown05 = true; return
// result;` 107 (same code); the scan as a while loop with its own increment 107 (same code); the scan's
// terminal test as `result |= (flags & 2) != 0` 110 (aligned 72); the copy loop's fragment as one
// ternary 134; the scan stopping at the terminal fragment (`result = (flags & 2) != 0; if (result)
// break;`) 102 (807 bytes, aligned 78: retail scans on past it).
// @retail 0x95840
bool __stdcall function_095840(s_network_stream_header *header, long *message_type, long *message_size, void *message_data)
{
	c_network_unreliable_stream *self = (c_network_unreliable_stream *)header;
	bool result = false;
	if (sequence_window_count(&self->m_fragment_window))
	{
		long first = self->m_fragment_window.oldest + 1;
		for (long sequence = first; sequence <= self->m_fragment_window.newest; sequence++)
		{
			long index = sequence_window_index(&self->m_fragment_window, sequence);
			s_stream_fragment *fragment = index != NONE ? &self->m_fragments[index] : NULL;
			if (!fragment || !(fragment->flags & 1))
				break;
			if (fragment->flags & 2)
				result = true;
		}
		if (!result)
			return result;
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
			for (long sequence = first; !last && sequence <= self->m_fragment_window.newest; sequence++)
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
					if (type->decode(&stream, *message_size, message_data))
						return result;
				}
			}
			self->m_unknown05 = true;
			return false;
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
