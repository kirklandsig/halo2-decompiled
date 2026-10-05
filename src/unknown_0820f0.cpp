// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_0820F0.CPP: the connections (0xf8 bytes each, at 0x4d87d4) and
   the two kinds of stream they own (lane D) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0820f0.h"
#include <xtl.h>

bool g_4d8ba0;
s_connection_counter g_4e6398;

s_link g_528000;
byte g_528b28[4];
byte g_529188[4];
s_connection_config g_4cf6d4;

void function_094bf0(s_network_stream_header *stream);
void function_095cf0(s_network_stream_header *stream);

static inline long network_time_now(void)
{
	if (g_510548)
		return g_51054c;
	return GetTickCount();
}

static inline long function_75890(long time)
{
	return network_time_now() - time;
}

static inline long network_stream_window_space(s_network_stream_header *stream)
{
	s_network_stream_window *window = &stream->window;
	return (window->end - stream->window.next + 0x200) << 5;
}

// @retail 0x820f0
long network_reliable_stream_allocate(long owner)
{
	long result = NONE;

	if (g_4d8ba0 && g_4d87d0 > 0)
	{
		for (long i = 0; i < g_4d87d0; i++)
		{
			if (!network_reliable_stream_get(i)->active)
			{
				s_network_stream_header *stream = network_reliable_stream_get(i);
				function_095cf0(stream);
				stream->owner = owner;
				stream->active = true;
				result = i;
				break;
			}
		}
	}
	return result;
}

// @retail 0x82150
long network_stream_allocate(long owner)
{
	long result = NONE;

	if (g_4d8ba0 && g_4d87d0 > 0)
	{
		for (long i = 0; i < g_4d87d0; i++)
		{
			if (!network_stream_get(i)->active)
			{
				s_network_stream_header *stream = network_stream_get(i);
				function_094bf0(stream);
				stream->owner = owner;
				stream->active = true;
				stream->unknown0c = (void *)0x528588;
				result = i;
				break;
			}
		}
	}
	return result;
}

// @retail 0x880b0
bool network_connection_flags_valid(dword flags)
{
	bool valid = !(flags & 0xffffff00) && (!(flags & 1) || !(flags & 2)) && (!(flags & 0x40) || !(flags & 0x80));
	if (flags & 4)
		valid = valid && !(flags & 8);
	if (!(flags & 8))
		valid = valid && !(flags & 0x30);
	if (!(flags & 0x10))
		valid = valid && !(flags & 0x20);
	return valid;
}

// @retail 0x88650
void network_connection_close(s_network_connection *connection, long reason)
{
	if (connection->state == 5 && reason != 6)
	{
		struct
		{
			long remote_sequence;
			long local_sequence;
			long reason;
		} message;

		message.local_sequence = connection->local_sequence;
		message.remote_sequence = connection->remote_sequence;
		message.reason = reason;
		function_07b140(connection->link, (long)&connection->address, 7, sizeof(message), &message);
	}
	if (connection->callback)
		connection->callback->function(connection->callback->context);
	link_remove_entry(connection->link_list, connection->id);
	connection->previous_address = connection->address;
	connection->state = 2;
	connection->close_reason = reason;
	connection->local_sequence = NONE;
	connection->remote_sequence = NONE;
}

// @retail 0x886e0
void network_connection_dispose(s_network_connection *connection)
{
	if (connection->state > 2)
		network_connection_close(connection, 3);
	connection->owner = 0;
	if (connection->stream_index != NONE)
	{
		s_network_stream_header *stream = network_stream_get(connection->stream_index);
		function_094bf0(stream);
		stream->active = false;
		stream->owner = 0;
		connection->stream_index = NONE;
	}
	if (connection->reliable_stream_index != NONE)
	{
		s_network_stream_header *stream = network_reliable_stream_get(connection->reliable_stream_index);
		stream->active = false;
		stream->owner = 0;
		connection->reliable_stream_index = NONE;
	}
	connection->state = 0;
}

// @retail 0x88d20
void network_connection_reset_timers(s_network_connection *connection)
{
	for (long i = 0; i < 6; i++)
	{
		connection->timers[i].time = network_time_now();
		connection->timers[i].counter = g_4e6398;
	}
}

// @retail 0x88d70
void network_connection_reset_timer(s_network_connection *connection, long index)
{
	connection->timers[index].time = network_time_now();
	connection->timers[index].counter = g_4e6398;
}

// @retail 0x88fd0
long network_connection_get_reliable_value48(s_network_connection *connection)
{
	long result = 0;

	if (connection->state == 5 && (connection->flags & 8))
		result = *(long *)((byte *)network_reliable_stream_get(connection->reliable_stream_index) + 0x48);
	return result;
}

// @retail 0x89000
long network_connection_get_reliable_pending(s_network_connection *connection)
{
	long result = 0;

	if (connection->state == 5 && (connection->flags & 8))
	{
		byte *stream = (byte *)network_reliable_stream_get(connection->reliable_stream_index);
		result = *(long *)(stream + 0x30) - *(long *)(stream + 0x44);
	}
	return result;
}

// @retail 0x89030
long network_connection_get_reliable_window(s_network_connection *connection)
{
	long result = 0;

	if (connection->state == 5 && (connection->flags & 8))
	{
		byte *stream = (byte *)network_reliable_stream_get(connection->reliable_stream_index);
		result = *(long *)(stream + 0x94c) - *(long *)(stream + 0x30) + 0x80;
	}
	return result;
}

// @retail 0x89070
long network_connection_send_capacity(s_network_connection *connection)
{
	long result = 0;

	if (connection->state == 5 && (connection->flags & 0x10))
	{
		result = network_stream_window_space(network_stream_get(connection->stream_index));
	}
	return result;
}

// @retail 0x890b0
void network_connection_update_handshake(s_network_connection *connection)
{
	if (function_75890(connection->handshake_time) >= connection->config->connect_timeout)
	{
		network_connection_close(connection, 4);
		return;
	}
	if (function_75890(connection->handshake_next_time) >= 0 && connection->handshake_count < connection->config->retry_count)
	{
		struct
		{
			long sequence;
			dword flags;
		} message;

		message.flags = connection->flags;
		message.sequence = connection->local_sequence;
		function_07b140(connection->link, (long)&connection->address, 4, sizeof(message), &message);
		connection->handshake_next_time = network_time_now() + connection->config->retry_interval;
	}
}

// @retail 0x89180
void network_connection_send_acknowledge(s_network_connection *connection, bool reliable)
{
	struct
	{
		long remote_sequence;
		long local_sequence;
	} message;

	message.local_sequence = connection->local_sequence;
	message.remote_sequence = connection->remote_sequence;
	if (connection->flags & 0x10)
	{
		if (reliable)
			function_095580(network_stream_get(connection->stream_index), 6, sizeof(message), &message);
	}
	else
	{
		function_07b140(connection->link, (long)&connection->address, 6, sizeof(message), &message);
	}
}

// @retail 0x88360
void network_connection_establish(s_network_connection *connection, long remote_sequence)
{
	bool established = false;

	if (connection->state == 3)
	{
		connection->state = 4;
		connection->establish_time = network_time_now();
		connection->remote_sequence = remote_sequence;
		established = true;
		network_connection_reset_timers(connection);
		if (!(connection->flags & 0x10))
			connection->state = 5;
	}
	network_connection_send_acknowledge(connection, established);
}

/* ---- reading a connection's packets ---- */

#include "bitstream.h"
#include "unknown_0662e0.h"

/* the clients a connection hands its packets to, and the owner that hears
   about its traffic; their vtables are in the client code */
class c_connection_client
{
public:
	virtual void v0() {}
	virtual void v1() {}
	virtual void v2() {}
	virtual void v3() {}
	virtual void v4() {}
	virtual long read_packet(long *sequence, s_bitstream *stream) { return 0; }
	virtual void message_delivered(long sequence) {}
	virtual void message_lost(long sequence, bool resent) {}
};

class c_connection_owner
{
public:
	virtual void v0() {}
	virtual void packet_received(long connection_id, long packet_size) {}
	virtual void message_acknowledged(long connection_id, long size, long time) {}
	virtual void message_lost(long connection_id, bool resent, bool late) {}
};

/* src/network_streams.cpp (lane J) */
class c_network_reliable_stream
{
public:
	long function_965e0(long *sequence, long *size, long *time);
	void mark_received(long sequence);
};
bool __stdcall function_096ce0(c_network_reliable_stream *stream, bool force, long *type, long *sequence);
bool __stdcall function_095840(s_network_stream_header *stream, long *message_type, long *message_size, void *message);

/* src/unknown_075800.cpp (lane J) */
class c_class_938e0
{
public:
	void function_93aa0(long channel_index, long message_type, long message_size, const void *message);
};

/* the bit stream module (src/unknown_195720.cpp) */
bool function_1946f0(s_bitstream *stream);

/* 0x1947a0, which retail inlines here */
static inline void stream_begin_reading(s_bitstream *stream)
{
	stream->mode = 3;
	stream->bit_position = 0;
	stream->checkpoint_count = 0;
	stream->error = false;
	if (function_1959c0(stream, 32) == 0x64656267)
	{
		stream->error = true;
		return;
	}
	stream->bit_position = 0;
	stream->error = false;
}

/* the clients of a connection with all the bits of type set: first the
   connection's own, then its class's (index with the top bit set) */
struct s_connection_client_iterator
{
	dword type;
	long index;
	long absolute_index;
	dword client_type;
	c_connection_client *client;
};

static inline void connection_client_iterator_new(s_connection_client_iterator *iterator, dword type)
{
	iterator->type = type;
	iterator->index = NONE;
	iterator->absolute_index = NONE;
	iterator->client_type = 0;
	iterator->client = NULL;
}

// @retail 0x891e0
bool network_connection_next_client(s_network_connection *connection, s_connection_client_iterator *iterator)
{
	long index = iterator->index;
	long absolute_index = iterator->absolute_index;
	s_connection_handler *handler;
	do
	{
		bool started = false;
		handler = NULL;
		if (index == NONE)
		{
			index = 0;
			absolute_index = 0;
			started = true;
		}
		if (index >= 0)
		{
			long local_index = index & 0x7fffffff;
			if (!started)
			{
				local_index++;
				index = local_index & 0x7fffffff;
				started = true;
				absolute_index = local_index;
			}
			if (local_index >= 0 && local_index < connection->handler_count)
			{
				handler = &connection->handlers[local_index];
			}
			else
			{
				absolute_index = connection->handler_count;
				index = 0x80000000;
				started = true;
			}
		}
		if (!handler && index < 0)
		{
			long local_index = index & 0x7fffffff;
			s_connection_handler *handlers = NULL;
			long count = 0;
			if (!started)
			{
				absolute_index = connection->handler_count;
				local_index++;
				index = local_index | 0x80000000;
				absolute_index += local_index;
			}
			if (connection->callback)
			{
				count = connection->callback->handler_count;
				handlers = connection->callback->handlers;
			}
			if (local_index >= 0 && local_index < count)
			{
				handler = &handlers[local_index];
			}
			else
			{
				index = NONE;
				absolute_index = NONE;
			}
		}
	} while (index != NONE && (handler->type & iterator->type) != iterator->type);
	iterator->index = index;
	iterator->absolute_index = absolute_index;
	if (index == NONE)
	{
		iterator->client = NULL;
		iterator->client_type = 0;
	}
	else
	{
		iterator->client = handler->client;
		iterator->client_type = handler->type;
	}
	return iterator->index != NONE;
}

// @retail 0x88db0
void network_connection_update_reliable_stream(s_network_connection *connection)
{
	if (connection->flags & 8)
	{
		c_network_reliable_stream *stream = (c_network_reliable_stream *)network_reliable_stream_get(connection->reliable_stream_index);
		{
			long type;
			long sequence;
			while (function_096ce0(stream, false, &type, &sequence))
				;
		}
		long message_sequence;
		long size;
		long time;
		long event;
		for (event = stream->function_965e0(&message_sequence, &size, &time); event; event = stream->function_965e0(&message_sequence, &size, &time))
		{
			if (event == 4 || event == 1)
			{
				connection->timers[4].time = network_time_now();
				connection->timers[4].counter = g_4e6398;
				if (connection->state == 4)
					connection->state = 5;
				if (connection->owner)
					connection->owner->message_acknowledged(connection->id, size, time);
			}
			if (event == 4)
			{
				s_connection_client_iterator iterator;
				connection_client_iterator_new(&iterator, 4);
				while (network_connection_next_client(connection, &iterator))
					iterator.client->message_delivered(message_sequence);
			}
			else if (event == 1 || event == 2 || event == 3)
			{
				bool resent = event != 3;
				s_connection_client_iterator iterator;
				connection_client_iterator_new(&iterator, 8);
				while (network_connection_next_client(connection, &iterator))
					iterator.client->message_lost(message_sequence, resent);
				if (connection->owner)
				{
					bool late = false;
					if (resent)
					{
						real scaled_real = (real)*(long *)((byte *)stream + 0x968) * g_network_configuration.real16e8;
						long scaled;
						__asm
						{
							fld scaled_real
							fistp scaled
						}
						long padded = *(long *)((byte *)stream + 0x968) + g_network_configuration.value16ec;
						if (scaled > padded)
							padded = scaled;
						if (time >= padded)
							late = true;
					}
					else
					{
						late = true;
					}
					connection->owner->message_lost(connection->id, resent, late);
				}
			}
		}
	}
}

// @retail 0x88750
bool network_connection_read_packet(s_network_connection *connection, s_bitstream *stream, long packet_size, bool out_of_band)
{
	long sequence = NONE;
	bool result = false;
	long status = 0;
	long message_type;
	long message_size;
	s_connection_client_iterator iterator;
	byte data[0x600];
	byte message[0x10000];

	if (!out_of_band)
		stream_begin_reading(stream);
	dword type;
	if (connection->callback && connection->callback->unknown31)
	{
		type = 0;
		connection->unknown1d = true;
	}
	else
	{
		type = 1;
		connection->unknown1d = false;
	}
	connection_client_iterator_new(&iterator, type);
	while (network_connection_next_client(connection, &iterator))
	{
		status = iterator.client->read_packet(&sequence, stream);
		if (status)
			break;
	}
	if (!status && function_1957d0(stream))
	{
		long size = function_1959c0(stream, 14);
		if (size > 0 && size <= 0x3000)
		{
			function_195820(stream, data, size);
			if (function_1946f0(stream))
				status = 3;
		}
		else
		{
			status = 3;
		}
	}
	if (stream_overflowed(stream))
		status = 3;
	switch (status)
	{
	case 2:
		break;
	case 3:
		break;
	case 1:
		stream->bit_position = 8 * stream->size_in_bytes;
	case 0:
		stream->mode = 5;
		result = true;
		if (!out_of_band)
		{
			network_connection_reset_timer(connection, 3);
			network_connection_update_reliable_stream(connection);
			if (connection->flags & 8)
				((c_network_reliable_stream *)network_reliable_stream_get(connection->reliable_stream_index))->mark_received(sequence);
			if (connection->flags & 0x10)
			{
				s_network_stream_header *unreliable = network_stream_get(connection->stream_index);
				while (function_095840(unreliable, &message_type, &message_size, message))
					connection->handler->function_93aa0(connection->id, message_type, message_size, message);
			}
		}
		break;
	default:
		__assume(0);
	}
	if (connection->owner && !out_of_band)
		connection->owner->packet_received(connection->id, packet_size);
	return result;
}

/* src/unknown_092870.cpp (lane J) */
class c_class_93590;
bool network_link_add_route(c_class_93590 *link, long connection_index, long kind, s_type_99af70 const *address);

static inline bool transport_address_is_loopback_inline(s_type_99af70 const *address)
{
	bool result = false;
	if (address->address_length == k_ipv4_address_length)
		result = address->ipv4_address == 0x7f000001;
	return result;
}

/* the link's next connection sequence number (never NONE) */
static inline long link_next_sequence(s_link *link)
{
	long sequence = link->sequence;
	link->sequence = sequence + 1;
	if (link->sequence == NONE)
		link->sequence = 0;
	return sequence;
}

/* starts connecting to an address: the initiator sends the handshake */
// @retail 0x88220
void network_connection_connect(s_type_99af70 const *address, s_network_connection *connection, bool initiator)
{
	connection->address = *address;
	connection->state = 3;
	connection->initiator = initiator;
	if (!transport_address_is_loopback_inline(address))
		connection->flags &= ~0xc0;
	long sequence = link_next_sequence(connection->link_list);
	connection->local_sequence = sequence;
	connection->remote_sequence = NONE;
	if (network_link_add_route((c_class_93590 *)connection->link_list, connection->id, sequence, &connection->address))
	{
		if (connection->initiator)
		{
			connection->handshake_time = network_time_now();
			connection->handshake_next_time = network_time_now();
			connection->handshake_count = 0;
			network_connection_update_handshake(connection);
		}
		network_connection_reset_timers(connection);
		if (connection->reliable_stream_index != NONE)
			function_095cf0(network_reliable_stream_get(connection->reliable_stream_index));
		if (connection->stream_index != NONE)
			function_094bf0(network_stream_get(connection->stream_index));
	}
	else
	{
		network_connection_close(connection, 2);
	}
}

static inline void connection_add_handler(s_network_connection *connection, dword type, c_connection_client *client)
{
	s_connection_handler *handler = &connection->handlers[connection->handler_count];
	handler->type = type;
	handler->client = client;
	connection->handler_count++;
}

/* sets a connection up: its link, its handler and configuration, and the
   streams its flags ask for (the reliable stream, the unreliable stream and
   the connection's own client) */
// @retail 0x88110
bool network_connection_initialize(s_network_connection *connection, long id, dword flags, s_link *link_list, void *link, c_class_938e0 *handler, s_connection_config const *config)
{
	bool result = false;
	connection->state = 1;
	connection->close_reason = 0;
	memset(&connection->previous_address, 0, sizeof(connection->previous_address));
	connection->id = id;
	connection->flags = flags;
	connection->link_list = link_list;
	connection->link = link;
	connection->handler = handler;
	connection->local_sequence = NONE;
	connection->remote_sequence = NONE;
	connection->config = config;
	connection->handler_count = 0;
	memset(connection->handlers, 0, sizeof(connection->handlers));
	connection->callback = 0;
	connection->owner = 0;
	if (connection->flags & 8)
	{
		connection->reliable_stream_index = network_reliable_stream_allocate(0);
		if (connection->reliable_stream_index == NONE)
			goto done;
		connection_add_handler(connection, 0x31, (c_connection_client *)network_reliable_stream_get(connection->reliable_stream_index));
	}
	if (connection->flags & 0x10)
	{
		connection->stream_index = network_stream_allocate(0);
		if (connection->stream_index == NONE)
			goto done;
		connection_add_handler(connection, 0x19, (c_connection_client *)network_stream_get(connection->stream_index));
	}
	if (connection->flags & 0x20)
	{
		connection_add_handler(connection, 1, (c_connection_client *)&connection->unknown18);
	}
	result = true;
done:
	if (!result)
		network_connection_dispose(connection);
	return result;
}

// @retail 0x82060
long network_connection_allocate(long unused_owner, dword flags)
{
	long result = NONE;
	dword const *flags_reference = &flags;
	long const *owner_reference = &unused_owner;
	if (g_4d8ba0 && g_4d87d0 > 0)
	{
		for (long i = 0; i < g_4d87d0; i++)
		{
			if (function_x7665e0(i)->state == 0)
			{
				if (network_connection_initialize(function_x7665e0(i), i, flags, &g_528000,
					g_528b28, (c_class_938e0 *)g_529188, &g_4cf6d4))
					result = i;
				break;
			}
		}
	}
	return result;
}

typedef void (__stdcall *connection_callback_function)(void *context);

// @retail 0x892f0
void network_connection_callback_initialize(s_connection_callback *callback, c_connection_client *const *clients,
	void *context, connection_callback_function function, long count, const dword *types, bool active)
{
	callback->active = active;
	callback->context = context;
	callback->unknown00[0] = true;
	callback->function = function;
	callback->handler_count = count;
	for (long i = 0; i < count; i++)
	{
		s_connection_handler *handler = &callback->handlers[i];
		handler->type = types[i];
		handler->client = clients[i];
	}
	callback->unknown31 = false;
}
