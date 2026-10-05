// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0B4DA0.CPP: the winsock transport endpoint (Bungie's
   socket_endpoint_part.cpp): its socket and options, binding,
   connecting, and reading and writing packets (outside lane J's region;
   decompiled for src/unknown_092870.cpp, which calls it). The endpoint itself
   is created by 0xb4d50 (src/unknown_0b49a0.cpp). */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_07aec0.h"
#include "unknown_0b4da0.h"
#include <xtl.h>
#include <string.h>

bool function_b53e0(s_transport_endpoint *endpoint, s_type_99af70 const *address);

/* a socket address: sockaddr_in for IPv4 (0x10 bytes), sockaddr_in6 for
   IPv6 (0x1c bytes) */
struct s_socket_address
{
	short family;
	word port;
	dword ipv4_address;
	word ipv6_address[8];
	dword scope;
};

static inline word byte_swap_word(word value)
{
	return (word)((value >> 8) | (value << 8));
}

/* a macro, not a function: retail's code reads the operand once per use and
   the compiler merges the reads in an order an inline function's parameter
   doesn't give */
#define byte_swap_long(value) ((((value) & 0xff0000) | ((value) >> 16)) >> 8 | ((((value) << 16) | ((value) & 0xff00)) << 8))

bool transport_address_to_socket_address(s_type_99af70 const *address, long *arg_181b43, s_socket_address *socket_address);

/* Standard convention (the `standard` marker): retail takes the option on the
   stack (`ret 4`).
   1. With the marker the body matches byte for byte; without it LTCG passes
      the option in eax.
   2. Nothing in retail holds the function's address; its callers, 0xb4e00
      and 0xb4e70, are LTCG code that pushes the option.
   3. Tried: the function on its own in a /GL- file, which keeps the stack
      argument but moves 0xb4e00's endpoint into esi (0xb4e00 then no longer
      matches). The parameter's address is never taken, so there is no
      address-taking idiom to try. */
// @retail 0xb4da0 standard
long __stdcall transport_endpoint_option_name(short option)
{
	switch (option)
	{
	case 0:
		return SO_REUSEADDR;
	case 1:
		return SO_LINGER;
	case 2:
		return SO_BROADCAST;
	case 3:
		return SO_SNDBUF;
	case 4:
		return SO_RCVBUF;
	case 5:
		return 0x4001;
	default:
		return NONE;
	}
}

// @retail 0xb4e00
long transport_endpoint_get_option(s_transport_endpoint *endpoint, short option)
{
	long value = 0;
	if (g_transport_globals.initialized && g_transport_globals.started && endpoint->socket != NONE)
	{
		short name = (short)transport_endpoint_option_name(option);
		if (name != -1)
		{
			long length = sizeof(value);
			if (getsockopt(endpoint->socket, SOL_SOCKET, name, (char *)&value, (int *)&length))
				WSAGetLastError();
		}
	}
	return value;
}

// @retail 0xb4e70
bool transport_endpoint_set_option(s_transport_endpoint *endpoint, short option, long value)
{
	bool result = false;
	if (g_transport_globals.initialized && g_transport_globals.started && endpoint->socket != NONE)
	{
		short name = (short)transport_endpoint_option_name(option);
		if (name != -1)
		{
			char const *option_value = (char const *)&value;
			if (option == 5)
				option_value = (char const *)value;
			if (setsockopt(endpoint->socket, SOL_SOCKET, name, option_value, sizeof(value)))
				WSAGetLastError();
			else
				result = true;
		}
	}
	return result;
}

// @retail 0xb4ed0
bool function_b4ed0(s_transport_endpoint *endpoint, s_type_99af70 const *address)
{
	bool result = false;
	if (g_transport_globals.initialized && g_transport_globals.started)
	{
		s_socket_address socket_address;
		long arg_181b43;
		if (transport_address_to_socket_address(address, &arg_181b43, &socket_address) &&
			function_b53e0(endpoint, address))
		{
			if (bind(endpoint->socket, (sockaddr const *)&socket_address, arg_181b43) == 0)
				result = true;
			else
				WSAGetLastError();
		}
	}
	return result;
}

// @retail 0xb4f50
void transport_endpoint_close(s_transport_endpoint *endpoint)
{
	if (endpoint->socket != NONE && g_transport_globals.initialized && g_transport_globals.started)
	{
		if (endpoint->flags & 1)
		{
			if (shutdown(endpoint->socket, 2))
				WSAGetLastError();
		}
		if (closesocket(endpoint->socket))
			WSAGetLastError();
	}
	endpoint->socket = NONE;
	endpoint->flags = 0;
}

// @retail 0xb5470
bool transport_address_to_socket_address(s_type_99af70 const *address, long *arg_181b43, s_socket_address *socket_address)
{
	*arg_181b43 = 0;
	bool result = false;
	switch (address->address_length)
	{
	case k_ipv4_address_length:
		socket_address->family = AF_INET;
		socket_address->ipv4_address = byte_swap_long(address->ipv4_address);
		socket_address->port = byte_swap_word(address->port);
		*arg_181b43 = 0x10;
		result = true;
		break;
	case k_ipv6_address_length:
		socket_address->family = 0x17;
		for (long i = 0; i < 8; i++)
			socket_address->ipv6_address[i] = byte_swap_word(address->ipv6_address[i]);
		socket_address->port = byte_swap_word(address->port);
		*arg_181b43 = 0x1c;
		result = true;
		break;
	}
	return result;
}

// @retail 0xb4fa0
short function_b4fa0(s_transport_endpoint *endpoint, void *buffer, short length)
{
	short result = 0;
	if (g_transport_globals.initialized && g_transport_globals.started && (endpoint->flags & 1))
	{
		result = (short)recv(endpoint->socket, (char *)buffer, length, 0);
		if (result == -1)
			return WSAGetLastError() == WSAEWOULDBLOCK ? -2 : -3;
		if (result == 0)
			endpoint->flags &= ~0x25;
	}
	return result;
}

// @retail 0xb5000
short function_b5000(s_transport_endpoint *endpoint, void const *buffer, short length)
{
	short result = 0;
	if (g_transport_globals.initialized && g_transport_globals.started && (endpoint->flags & 1))
	{
		result = (short)send(endpoint->socket, (char const *)buffer, length, 0);
		if (result == -1)
		{
			long error;
			switch (WSAGetLastError())
			{
			case WSAEWOULDBLOCK:
				error = -2;
				break;
			case WSAEHOSTUNREACH:
				error = -1;
				break;
			default:
				error = -3;
				break;
			}
			return (short)error;
		}
	}
	return result;
}

// @retail 0xb5560
bool socket_address_to_transport_address(s_socket_address const *socket_address, long arg_181b43, s_type_99af70 *address)
{
	bool result = false;
	switch (arg_181b43)
	{
	case 0x10:
		address->ipv4_address = byte_swap_long(socket_address->ipv4_address);
		address->port = byte_swap_word(socket_address->port);
		address->address_length = k_ipv4_address_length;
		result = true;
		break;
	case 0x1c:
		for (long i = 0; i < 8; i++)
			address->ipv6_address[i] = byte_swap_word(socket_address->ipv6_address[i]);
		address->port = byte_swap_word(socket_address->port);
		address->address_length = k_ipv6_address_length;
		result = true;
		break;
	default:
		memset(address, 0, sizeof(*address));
		break;
	}
	return result;
}

// @retail 0xb5060
short function_b5060(s_transport_endpoint *endpoint, void *buffer, short length, s_type_99af70 *address)
{
	union
	{
		char bytes[0x1c];
		s_socket_address address;
	} socket_address = {0};
	long arg_181b43 = sizeof(socket_address);
	short result = -3;
	if (g_transport_globals.initialized && g_transport_globals.started)
	{
		short read = (short)recvfrom(endpoint->socket, (char *)buffer, length, 0, (sockaddr *)&socket_address, (int *)&arg_181b43);
		if (read == -1)
			result = WSAGetLastError() == WSAEWOULDBLOCK ? -2 : -3;
		else
		{
			socket_address_to_transport_address(&socket_address.address, arg_181b43, address);
			result = read;
		}
	}
	return result;
}

// @retail 0xb5110
short function_b5110(s_transport_endpoint *endpoint, void const *buffer, short length, s_type_99af70 const *address)
{
	short result = -3;
	if (g_transport_globals.initialized && g_transport_globals.started)
	{
		s_socket_address socket_address;
		long arg_181b43;
		if (transport_address_to_socket_address(address, &arg_181b43, &socket_address))
		{
			short sent = (short)sendto(endpoint->socket, (char const *)buffer, length, 0, (sockaddr const *)&socket_address, arg_181b43);
			if (sent == -1)
			{
				long error = WSAGetLastError();
				long code;
				if (error == WSAEWOULDBLOCK)
					code = -2;
				else if (error == WSAEHOSTUNREACH)
					code = -1;
				else
					code = -3;
				return (short)code;
			}
			result = sent;
		}
	}
	return result;
}

/* 0x48480c: the value connect gives option 5 */
long g_48480c;

// @retail 0xb51b0
bool function_b51b0(s_transport_endpoint *endpoint, s_type_99af70 const *address)
{
	bool result = false;
	if (g_transport_globals.initialized && g_transport_globals.started)
	{
		s_socket_address socket_address;
		long arg_181b43;
		if (transport_address_to_socket_address(address, &arg_181b43, &socket_address) &&
			function_b53e0(endpoint, address))
		{
			if (endpoint->flags & 0x40)
				transport_endpoint_set_option(endpoint, 5, g_48480c);
			if (!transport_endpoint_set_nonblocking(endpoint))
				WSAGetLastError();
			else if (connect(endpoint->socket, (sockaddr const *)&socket_address, arg_181b43) == 0)
			{
				endpoint->flags |= 0x21;
				result = true;
			}
			else if (WSAGetLastError() != WSAEWOULDBLOCK)
				result = false;
			else
				result = true;
		}
	}
	return result;
}

// @retail 0xb52b0
bool transport_endpoint_test_connection(s_transport_endpoint *endpoint, bool *connected)
{
	bool result = false;
	fd_set write_set = {0};
	fd_set error_set = {0};
	*connected = false;
	timeval timeout = {0, 0};
	if (g_transport_globals.initialized && g_transport_globals.started && endpoint->socket != NONE)
	{
		FD_SET(endpoint->socket, &write_set);
		FD_SET(endpoint->socket, &error_set);
		if (select(0, NULL, &write_set, &error_set, &timeout) != -1)
		{
			if (!FD_ISSET(endpoint->socket, &error_set))
			{
				result = true;
				if (FD_ISSET(endpoint->socket, &write_set))
				{
					endpoint->flags |= 0x21;
					*connected = result;
				}
			}
		}
		else
			WSAGetLastError();
	}
	return result;
}

// @retail 0xb53e0
bool function_b53e0(s_transport_endpoint *endpoint, s_type_99af70 const *address)
{
	bool result = false;
	if (endpoint->socket == NONE)
	{
		long type = 0;
		long protocol = 0;
		long family = NONE;
		switch (endpoint->type)
		{
		case 2:
			type = SOCK_DGRAM;
			protocol = IPPROTO_UDP;
			break;
		case 3:
			type = SOCK_DGRAM;
			protocol = IPPROTO_VDP;
			break;
		case 4:
			type = SOCK_STREAM;
			protocol = IPPROTO_TCP;
			break;
		}
		switch (address->address_length)
		{
		case k_ipv4_address_length:
			family = AF_INET;
			break;
		case k_ipv6_address_length:
			family = 0x17;
			break;
		}
		endpoint->socket = socket(family, type, protocol);
	}
	if (endpoint->socket != NONE)
	{
		if (transport_endpoint_get_option(endpoint, 4))
			endpoint->blocking = true;
		result = true;
	}
	else
		WSAGetLastError();
	return result;
}
