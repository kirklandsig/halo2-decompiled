// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_092870.CPP: the network link: its transport endpoints, its routes to
   the connections, its traffic statistics, and the packets it sends and
   receives */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0820f0.h"
#include "bitstream.h"
#include "unknown_092870.h"
#include "unknown_092870_2.h"
#include "unknown_0b4da0.h"
#include <xtl.h>
#include <string.h>

#define MAXIMUM_LINK_ROUTES 16

static inline dword network_time_now(void)
{
	if (g_510548)
		return g_51054c;
	return GetTickCount();
}

struct s_block_header;
s_block_header *function_0b4d50(word tag);

static inline void transport_endpoint_free(s_transport_endpoint *endpoint)
{
	if (!VirtualFree(endpoint, 0, MEM_RELEASE))
		GetLastError();
}

/* a window of timed samples */
struct s_network_samples
{
	long count;
	long index;
	struct
	{
		dword time;
		long value;
	} entries[32];
	long total;
	long elapsed;
};

struct s_link_route
{
	long connection_index;
	long kind;
	bool pending;
	byte unknown09[3];
	s_type_99af70 address;
};

struct s_link_packet
{
	long type;
	bool unknown04;
	byte unknown05[3];
	s_type_99af70 address;
	long payload_size;
	byte payload[0x600];
	long extra_size;
	byte extra[0x200];
};

/* what the link hands the out-of-band packets it receives to (the message
   gateway) */
class c_type_96c0b1
{
public:
	virtual void function_x102dd8(s_type_99af70 const *address, s_bitstream *packet) {}
};

class c_class_93590
{
public:
	void function_93590(s_link_packet const *packet, long *size, byte *buffer, long buffer_size) const;
	bool function_93610(long size, byte const *buffer, s_link_packet *packet) const;

	bool m_initialized;
	long m_sequence;
	bool m_open;
	s_transport_endpoint *m_endpoints[4];
	c_type_96c0b1 *m_out_of_band_consumer;
	long m_route_count;
	s_link_route m_routes[MAXIMUM_LINK_ROUTES];
	long m_unknown224;
	s_network_statistics m_statistics[4];
};

/* clears a direction's traffic */
static inline void network_statistics_reset(s_network_statistics *statistics)
{
	statistics->packets = 0;
	statistics->bytes = 0;
	statistics->period_start = 0;
	memset(&statistics->current, 0, sizeof(statistics->current));
	statistics->sample_index = 0;
	memset(statistics->samples, 0, sizeof(statistics->samples));
	memset(&statistics->total, 0, sizeof(statistics->total));
}

// @retail 0x92870
void network_statistics_initialize(s_network_statistics *statistics, long interval)
{
	statistics->interval = interval;
	statistics->period = interval / NUMBER_OF_STATISTICS_SAMPLES;
	statistics->rate_scale = 1000.0f / interval;
	network_statistics_reset(statistics);
}

// @retail 0x928e0
void network_statistics_update(s_network_statistics *statistics)
{
	dword now = network_time_now();
	if (statistics->period_start == 0)
		statistics->period_start = now;
	if (now >= statistics->period_start + statistics->period)
	{
		if ((long)((now - statistics->period_start) / statistics->period) > NUMBER_OF_STATISTICS_SAMPLES)
		{
			statistics->total.packets = 0;
			statistics->total.bytes = 0;
			memset(statistics->samples, 0, sizeof(statistics->samples));
			statistics->period_start = now;
			return;
		}
	}
	while (now >= statistics->period_start + statistics->period)
	{
		statistics->total.packets -= statistics->samples[statistics->sample_index].packets;
		statistics->total.bytes -= statistics->samples[statistics->sample_index].bytes;
		statistics->total.packets += statistics->current.packets;
		statistics->total.bytes += statistics->current.bytes;
		statistics->samples[statistics->sample_index] = statistics->current;
		statistics->sample_index = (statistics->sample_index + 1) % NUMBER_OF_STATISTICS_SAMPLES;
		statistics->current.packets = 0;
		statistics->current.bytes = 0;
		statistics->period_start += statistics->period;
	}
}

// @retail 0x929d0
void network_samples_reset(s_network_samples *samples)
{
	dword now = network_time_now();
	for (long i = 0; i < samples->count; i++)
	{
		samples->entries[i].time = now;
		samples->entries[i].value = 0;
	}
	samples->index = 0;
	samples->total = 0;
	samples->elapsed = 0;
}

// @retail 0x92a30
void network_samples_add(s_network_samples *samples, long value)
{
	dword now = network_time_now();
	samples->elapsed = now - samples->entries[samples->index].time;
	samples->total -= samples->entries[samples->index].value;
	samples->entries[samples->index].time = now;
	samples->entries[samples->index].value = value;
	samples->total += value;
	samples->index = (samples->index + 1) % samples->count;
}

// @retail 0x92ae0
bool network_link_open_endpoint(long type, word port, bool broadcast, s_transport_endpoint **endpoint_out)
{
	s_transport_endpoint *endpoint = (s_transport_endpoint *)function_0b4d50((word)type);
	if (endpoint)
	{
		s_type_99af70 address;
		address.ipv4_address = 0;
		address.port = port;
		address.address_length = k_ipv4_address_length;
		bool success = function_b4ed0(endpoint, &address) && transport_endpoint_set_nonblocking(endpoint);
		if (broadcast)
			success = success && transport_endpoint_set_option(endpoint, 2, true);
		if (success)
			*endpoint_out = endpoint;
		else
		{
			transport_endpoint_close(endpoint);
			transport_endpoint_free(endpoint);
		}
		return success;
	}
	return false;
}

// @retail 0x92c70
void network_link_close(c_class_93590 *link)
{
	for (long i = 0; i < 4; i++)
	{
		s_transport_endpoint *endpoint = link->m_endpoints[i];
		if (endpoint)
		{
			transport_endpoint_close(endpoint);
			transport_endpoint_free(endpoint);
			link->m_endpoints[i] = 0;
		}
	}
	link->m_open = false;
}

// @retail 0x92bf0
bool network_link_open(c_class_93590 *link)
{
	if (network_link_open_endpoint(3, 1000, false, &link->m_endpoints[0]) &&
		network_link_open_endpoint(2, 1001, true, &link->m_endpoints[3]) &&
		network_link_open_endpoint(3, 1005, false, &link->m_endpoints[1]) &&
		network_link_open_endpoint(3, 1006, false, &link->m_endpoints[2]))
	{
		link->m_open = true;
	}
	else
		network_link_close(link);
	return link->m_open;
}

// @retail 0x92a90
bool network_link_initialize(c_class_93590 *link)
{
	network_statistics_initialize(&link->m_statistics[0], 2000);
	network_statistics_initialize(&link->m_statistics[1], 2000);
	network_statistics_initialize(&link->m_statistics[2], 2000);
	network_statistics_initialize(&link->m_statistics[3], 2000);
	network_link_open(link);
	link->m_initialized = true;
	return true;
}

/* whether two addresses are the same */
static inline bool link_address_match(s_type_99af70 const *a, s_type_99af70 const *b)
{
	short length = a->address_length > b->address_length ? b->address_length : a->address_length;
	return a->address_length > 0 && a->address_length == b->address_length && memcmp(a, b, length) == 0;
}

// @retail 0x92e50
long network_link_find_route(c_class_93590 *link, long kind, s_type_99af70 const *address)
{
	long result = NONE;
	for (long i = 0; i < link->m_route_count; i++)
	{
		s_link_route *route = &link->m_routes[i];
		if (link_address_match(address, &route->address))
		{
			s_network_connection *connection = function_x7665e0(route->connection_index);
			bool found;
			switch (kind)
			{
			case 1:
				found = connection->state > 2 && (connection->flags & 0x80);
				break;
			case 2:
				found = connection->state > 2 && (connection->flags & 0x40);
				break;
			default:
				found = true;
				break;
			}
			if (found)
			{
				result = i;
				break;
			}
		}
	}
	return result;
}

// @retail 0x92ef0
inline long network_link_find_connection(c_class_93590 *link, long kind, s_type_99af70 const *address)
{
	long result = NONE;
	long index = network_link_find_route(link, kind, address);
	if (index != NONE)
		result = link->m_routes[index].connection_index;
	return result;
}

// @retail 0x92d10
bool network_link_add_route(c_class_93590 *link, long connection_index, long kind, s_type_99af70 const *address)
{
	bool result = true;
	s_network_connection *connection = function_x7665e0(connection_index);
	long existing;
	if (connection->state > 2 && (connection->flags & 0x80))
		existing = network_link_find_connection(link, 1, address);
	else if (connection->state > 2 && (connection->flags & 0x40))
		existing = network_link_find_connection(link, 2, address);
	else
		existing = network_link_find_connection(link, 0, address);
	if (existing != connection_index)
	{
		if (existing != NONE)
		{
			if (connection->local_sequence - function_x7665e0(existing)->local_sequence == 0)
				result = false;
			else
				network_connection_dispose(function_x7665e0(existing));
		}
		if (result)
		{
			if (link->m_route_count < MAXIMUM_LINK_ROUTES)
			{
				link->m_routes[link->m_route_count].connection_index = connection_index;
				link->m_routes[link->m_route_count].kind = kind;
				link->m_routes[link->m_route_count].pending = false;
				link->m_routes[link->m_route_count].address = *address;
				link->m_route_count++;
			}
			else
				result = false;
		}
	}
	return result;
}

// @retail 0x92f10
void network_link_close_connections(c_class_93590 *link)
{
	for (long i = 0; i < link->m_route_count; i++)
		network_connection_close(function_x7665e0(link->m_routes[i].connection_index), 1);
	link->m_route_count = 0;
}

/* a connection's update (lane D's region, not decompiled yet:
   src/stubs/lane_j.cpp) */
void __stdcall function_0883c0(s_network_connection *connection);

/* updates the routes' connections and closes those whose route was dropped */
// @retail 0x93090
void network_link_update_connections(c_class_93590 *link)
{
	for (long i = 0; i < link->m_route_count; i++)
	{
		s_link_route *route = &link->m_routes[i];
		s_network_connection *connection = function_x7665e0(route->connection_index);
		function_0883c0(connection);
		if (route->pending)
		{
			if (connection->state > 2)
				network_connection_close(connection, 9);
			route->pending = false;
		}
	}
}

// @retail 0x93590
void c_class_93590::function_93590(s_link_packet const *packet, long *size, byte *buffer, long buffer_size) const
{
	if (packet->type != 3)
	{
		*(word *)buffer = (word)packet->payload_size;
		memcpy(buffer + 2, packet->payload, packet->payload_size);
		memcpy(buffer + 2 + packet->payload_size, packet->extra, packet->extra_size);
		*size = packet->extra_size + packet->payload_size + 2;
	}
	else
	{
		memcpy(buffer, packet->payload, packet->payload_size);
		*size = packet->payload_size;
	}
}

// @retail 0x93610
bool c_class_93590::function_93610(long size, byte const *buffer, s_link_packet *packet) const
{
	bool result = true;
	if (packet->type != 3)
	{
		if (size >= 2)
		{
			packet->payload_size = *(word const *)buffer;
			packet->extra_size = size - packet->payload_size - 2;
			if (packet->payload_size >= 0 && packet->payload_size <= sizeof(packet->payload) &&
				packet->extra_size >= 0 && packet->extra_size <= sizeof(packet->extra))
			{
				memcpy(packet->payload, buffer + 2, packet->payload_size);
				memcpy(packet->extra, buffer + packet->payload_size + 2, packet->extra_size);
			}
			else
				result = false;
		}
		else
			result = false;
	}
	else
	{
		if (size <= sizeof(packet->payload))
		{
			packet->payload_size = size;
			memcpy(packet->payload, buffer, size);
		}
		else
			result = false;
	}
	return result;
}

/* the transport protocol a packet goes out on */
static inline long link_packet_protocol(long type)
{
	return type != 3 ? 3 : 2;
}

/* the bytes a transport protocol adds to each packet */
static inline long transport_protocol_overhead(long protocol)
{
	long overhead = 0;
	switch (protocol)
	{
	case 2:
		overhead = 0x2c;
		break;
	case 3:
		overhead = 0x2d;
		break;
	case 4:
		overhead = 0x38;
		break;
	}
	return overhead;
}

// @retail 0x936c0
long network_link_packet_size(s_link_packet const *packet)
{
	long type = packet->type;
	long payload_size = packet->payload_size;
	long protocol = link_packet_protocol(type);
	if (payload_size % 8 > 0)
		payload_size += 8 - payload_size % 8;
	return transport_protocol_overhead(protocol) + packet->extra_size + payload_size;
}

static inline bool function_x31493c(s_type_99af70 const *address)
{
	bool result = false;
	if (address->address_length == k_ipv4_address_length)
		result = address->ipv4_address == 0x7f000001;
	return result;
}

// @retail 0x93730
void network_link_send_to(c_class_93590 *link, long endpoint_index, s_type_99af70 const *address, long size, byte const *buffer)
{
	s_type_99af70 send_address = *address;
	switch (endpoint_index)
	{
	case 0:
		send_address.port = 1000;
		break;
	case 1:
		send_address.port = 1005;
		break;
	case 2:
		send_address.port = 1006;
		break;
	case 3:
		send_address.port = 1001;
		break;
	default:
		__assume(0);
	}
	s_transport_endpoint *endpoint = link->m_endpoints[endpoint_index];
	if (endpoint)
	{
		long written = function_b5110(endpoint, buffer, (short)size, &send_address);
		if (written != size && written == -1)
		{
			long route_index = network_link_find_route(link, endpoint_index, address);
			if (route_index != NONE)
				link->m_routes[route_index].pending = true;
		}
	}
}

// @retail 0x932f0
void network_link_send_packet(c_class_93590 *link, s_link_packet const *packet)
{
	long size;
	byte buffer[0x1000];
	link->function_93590(packet, &size, buffer, sizeof(buffer));
	if (!function_x31493c(&packet->address))
	{
		network_statistics_add(&link->m_statistics[0], packet->payload_size);
		network_statistics_add(&link->m_statistics[2], network_link_packet_size(packet));
	}
	if (size <= 0x518 && size > 0)
		network_link_send_to(link, packet->type, &packet->address, size, buffer);
}

static inline bool network_link_packet_set_payload(s_link_packet *packet, s_bitstream const *stream)
{
	packet->payload_size = stream->size_in_bytes;
	if (stream->size_in_bytes > sizeof(packet->payload))
		return false;
	memcpy(packet->payload, stream->data, stream->size_in_bytes);
	return true;
}

// @retail 0x93100
void network_link_send_out_of_band(c_class_93590 *link, s_bitstream const *stream, s_type_99af70 const *address, long *size_out)
{
	long result = 0;
	s_link_packet packet;
	memset(&packet, 0, sizeof(packet));
	packet.type = 3;
	packet.address = *address;
	if (network_link_packet_set_payload(&packet, stream))
	{
		network_link_send_packet(link, &packet);
		result = network_link_packet_size(&packet);
	}
	if (size_out)
		*size_out = result;
}

// @retail 0x931a0
void network_link_send_connection_packet(c_class_93590 *link, long connection_index, s_bitstream const *stream, long extra_size, void const *extra, long *size_out)
{
	long result = 0;
	s_network_connection *connection = function_x7665e0(connection_index);
	s_link_packet packet;
	memset(&packet, 0, sizeof(packet));
	if (connection->state != 0 && connection->state != 1)
	{
		packet.address = connection->address;
		if (function_x31493c(&packet.address))
		{
			if (connection->state > 2 && (connection->flags & 0x40))
				packet.type = 1;
			else if (connection->state > 2 && (connection->flags & 0x80))
				packet.type = 2;
			else
				goto done;
		}
		else
			packet.type = 0;
		packet.unknown04 = false;
		if (stream)
		{
			packet.payload_size = stream->size_in_bytes;
			if (stream->size_in_bytes > sizeof(packet.payload))
				goto done;
			memcpy(packet.payload, stream->data, stream->size_in_bytes);
		}
		if (extra_size > 0)
		{
			packet.extra_size = extra_size;
			if (extra_size > sizeof(packet.extra))
				goto done;
			memcpy(packet.extra, extra, extra_size);
		}
		network_link_send_packet(link, &packet);
		result = network_link_packet_size(&packet);
	}
done:
	if (size_out)
		*size_out = result;
}

/* src/unknown_0820f0.cpp (lane D) */
bool network_connection_read_packet(s_network_connection *connection, s_bitstream *stream, long packet_size, bool out_of_band);
void __stdcall function_054810(void const *data, long size);

/* a stream over data that is read */
static inline void stream_set_data(s_bitstream *stream, void const *data, long size)
{
	stream->unknown08 = 1;
	stream->data = (byte *)data;
	stream->size_in_bytes = size;
	stream->mode = 0;
	stream->bit_position = 0;
	stream->checkpoint_count = 0;
	stream->error = false;
}

// @retail 0x933f0
void network_link_receive_packet(c_class_93590 *link, s_link_packet const *packet)
{
	long packet_size = network_link_packet_size(packet);
	if (!function_x31493c(&packet->address))
	{
		network_statistics_add(&link->m_statistics[1], packet->payload_size);
		network_statistics_add(&link->m_statistics[3], packet_size);
	}
	if (packet->type > 2)
	{
		s_bitstream stream;
		stream_set_data(&stream, packet->payload, packet->payload_size);
		if (link->m_out_of_band_consumer)
			link->m_out_of_band_consumer->function_x102dd8(&packet->address, &stream);
	}
	else
	{
		long route_index = network_link_find_route(link, packet->type, &packet->address);
		if (route_index != NONE)
		{
			long connection_index = link->m_routes[route_index].connection_index;
			if (connection_index != NONE)
			{
				s_network_connection *connection = function_x7665e0(connection_index);
				if (packet->payload_size > 0)
				{
					s_bitstream stream;
					stream_set_data(&stream, packet->payload, packet->payload_size);
					network_connection_read_packet(connection, &stream, packet_size, false);
				}
				if (packet->extra_size > 0)
				{
					function_054810(packet->extra, packet->extra_size);
					network_connection_reset_timer(connection, 5);
				}
			}
		}
	}
}

// @retail 0x92f80
void network_link_receive(c_class_93590 *link)
{
	bool received;
	do
	{
		received = false;
		struct
		{
			s_type_99af70 address;
			long endpoint_index;
			long size;
		} incoming;
		byte buffer[0x1000];
		for (long i = 0; !received && i < 4; i++)
		{
			s_transport_endpoint *endpoint = link->m_endpoints[i];
			if (endpoint)
			{
				long read = function_b5060(endpoint, buffer, sizeof(buffer), &incoming.address);
				if (read > 0 && function_7af40(&incoming.address))
				{
					incoming.size = read;
					incoming.endpoint_index = i;
					received = true;
				}
			}
		}
		if (received)
		{
			s_link_packet packet;
			memset(&packet, 0, sizeof(packet));
			packet.type = incoming.endpoint_index;
			packet.address = incoming.address;
			if (link->function_93610(incoming.size, buffer, &packet))
				network_link_receive_packet(link, &packet);
		}
	} while (received);
}
