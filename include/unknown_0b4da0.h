/* UNKNOWN_0B4DA0.H: a transport endpoint, a socket of the transport
   layer (src/unknown_0b4da0.cpp; src/unknown_0b49a0.cpp creates them) */

#ifndef UNKNOWN_0B4DA0_H
#define UNKNOWN_0B4DA0_H

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_07aec0.h"
#include <xtl.h>

struct s_transport_endpoint
{
	long socket;
	union
	{
		word flags;
		struct
		{
			word connected : 1;
			word unknown1 : 3;
			word blocking : 1;
		};
	};
	short type;
};

bool function_b4ed0(s_transport_endpoint *endpoint, s_type_99af70 const *address);
bool transport_endpoint_set_option(s_transport_endpoint *endpoint, short option, long value);
void transport_endpoint_close(s_transport_endpoint *endpoint);
short function_b5060(s_transport_endpoint *endpoint, void *buffer, short length, s_type_99af70 *address);
short function_b5110(s_transport_endpoint *endpoint, void const *buffer, short length, s_type_99af70 const *address);

/* makes an endpoint's socket non-blocking; retail has it expanded in
   network_link_open_endpoint (0x92ae0) and function_b51b0
   (0xb51b0) */
__forceinline bool transport_endpoint_set_nonblocking(s_transport_endpoint *endpoint)
{
	bool result = true;
	if (g_transport_globals.initialized && g_transport_globals.started)
	{
		if (endpoint->socket == NONE)
			result = false;
		else if ((bool)(((dword)(short)endpoint->flags >> 4) & 1))
		{
			dword argument = 1;
			if (ioctlsocket(endpoint->socket, FIONBIO, &argument) == 0)
				endpoint->blocking = false;
			else
			{
				WSAGetLastError();
				result = false;
			}
		}
	}
	return result;
}

#endif
