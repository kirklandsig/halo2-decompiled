// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_07B330.CPP: the message gateway's outgoing packet: the messages
   written for one address collect in a bit stream until the gateway sends
   them through the link as one out-of-band packet (lane J, for lane D's
   observer, 0x784a0) */

#include "unknown_11c920.h"
#include "bitstream.h"
#include "unknown_07aec0.h"
#include "unknown_092870.h"
#include <string.h>
#include "network_message_types.h"

class c_class_938e0
{
public:
	void function_938e0(const s_type_99af70 *address, long message_type, const void *message);
};

#ifndef MIN
#define MIN(a,b) ((a)>(b)?(b):(a))
#endif

struct s_network_message_gateway
{
	virtual bool read_packet(const s_type_99af70 *address, s_bitstream *stream);
	byte unknown04[4];
	c_class_93590 *link;
	c_type_659ceb *field_c_7;
	c_class_938e0 *handler;
	bool outgoing_packet_pending;
	byte outgoing_packet_storage[0x603];
	s_type_99af70 outgoing_packet_address;
	s_bitstream outgoing_packet;
};

/* ends a written stream: its size in bytes, rounded up to the stream's
   alignment */
static inline void stream_finish_writing(s_bitstream *stream)
{
	long size_in_bytes = (stream->bit_position + 7) / 8;
	stream->size_in_bytes = size_in_bytes;
	long remainder = size_in_bytes % stream->unknown08;
	if (remainder != 0)
		stream->size_in_bytes = stream->unknown08 - remainder + size_in_bytes;
	stream->mode = 2;
}

// @retail 0x7b330
void network_message_gateway_send_pending_messages(s_network_message_gateway *gateway)
{
	if (gateway->outgoing_packet_pending)
	{
		stream_write_bit(&gateway->outgoing_packet, false);
		stream_finish_writing(&gateway->outgoing_packet);
		network_link_send_out_of_band(gateway->link, &gateway->outgoing_packet, &gateway->outgoing_packet_address, NULL);
		gateway->outgoing_packet_pending = false;
	}
}

// @retail 0x7b390
void network_message_gateway_send_pending_messages_to_address(s_network_message_gateway *gateway, s_type_99af70 const *address)
{
	if (gateway->outgoing_packet_pending)
	{
		s_type_99af70 const *a = &gateway->outgoing_packet_address;
		short length = MIN(a->address_length, address->address_length);
		if (a->address_length > 0 && a->address_length == address->address_length && memcmp(a, address, length) == 0)
			network_message_gateway_send_pending_messages(gateway);
	}
}

bool network_message_read_header(s_bitstream *stream, long *type, c_type_659ceb const *collection, long *size);
void network_message_write_header(s_bitstream *stream, long type, long size);
void function_194710(s_bitstream *stream, bool discard);

static __forceinline void gateway_stream_begin(s_bitstream *stream)
{
	stream->unknown08 = 1;
	stream->mode = 1;
	memset(stream->data, 0, stream->size_in_bytes);
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
		if (function_1959c0(stream, 32) == 'debg')
			stream->error = true;
		else
		{
			stream->bit_position = 0;
			stream->error = false;
		}
	}
}

// @retail 0x7b140
bool __stdcall function_07b140(void *object, long destination, long type, long size, void *message)
{
	s_network_message_gateway *gateway = (s_network_message_gateway *)object;
	const s_type_99af70 *address = (const s_type_99af70 *)destination;
	bool result = false;
	if (gateway->outgoing_packet_pending)
	{
		const s_type_99af70 *old = &gateway->outgoing_packet_address;
		short length = MIN(old->address_length, address->address_length);
		if (!(old->address_length > 0 && old->address_length == address->address_length &&
			memcmp(old, address, length) == 0))
			network_message_gateway_send_pending_messages(gateway);
	}
	for (;;)
	{
		bool started = false;
		if (!gateway->outgoing_packet_pending)
		{
			stream_set_buffer(&gateway->outgoing_packet, gateway->outgoing_packet_storage, 0x518);
			gateway_stream_begin(&gateway->outgoing_packet);
			gateway->outgoing_packet_address = *address;
			gateway->outgoing_packet_pending = true;
			started = true;
		}
		stream_save_position(&gateway->outgoing_packet);
		stream_write_bit(&gateway->outgoing_packet, true);
		c_type_659ceb *collection = gateway->field_c_7;
		network_message_write_header(&gateway->outgoing_packet, type, size);
		collection->m_types[type].encode(&gateway->outgoing_packet, size, message);
		s_bitstream *stream = &gateway->outgoing_packet;
		if (stream->bit_position + 1 <= stream->size_in_bytes << 3)
		{
			stream->checkpoint_count--;
			result = true;
			break;
		}
		if (started)
		{
			function_194710(stream, true);
			break;
		}
		function_194710(stream, true);
		network_message_gateway_send_pending_messages(gateway);
	}
	return result;
}

// @retail 0x7afd0
bool s_network_message_gateway::read_packet(const s_type_99af70 *address, s_bitstream *stream)
{
	bool result = true;
	unsigned __int64 storage[8192];
	stream->mode = 3;
	stream->bit_position = 0;
	stream->checkpoint_count = 0;
	stream->error = false;
	if (function_1959c0(stream, 32) == 'debg')
		stream->error = true;
	else
	{
		stream->bit_position = 0;
		stream->error = false;
	}
	if (!stream_overflowed(stream))
	{
		while (stream_read_bit(stream))
		{
			long type = NONE;
			long size = 0;
			c_type_659ceb *collection = field_c_7;
			bool decoded = false;
			if (network_message_read_header(stream, &type, collection, &size))
			{
				memset(storage, 0, size);
				decoded = collection->m_types[type].decode(stream, size, storage);
			}
			result = decoded;
			if (!result)
				break;
			if (handler)
				handler->function_938e0(address, type, storage);
		}
		if (result)
			stream->mode = 5;
	}
	else
		result = false;
	return result;
}
